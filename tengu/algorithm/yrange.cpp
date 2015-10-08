#include "yrange.hpp"
#include "evaluator.hpp"
#include <algorithm>
#include <array>
#include <functional>
#include <vector>
#include <boost/format.hpp>
#include <QFile>
#include <QVector>
#include <QtConcurrent/QtConcurrent>
#include <QtConcurrent/QtConcurrentMap>
#include <QFuture>
#include <QIODevice>

yrange::yrange(problem_type _problem, evaluator eval):eval(eval)
{
    algorithm_name = "yrange";
    pre_problem = _problem;
}

yrange::~yrange()
{
}

void yrange::run()
{
    qDebug("yrange start");
/*
    QVector<std::tuple<problem_type,int,int,std::size_t>> data;
    data.reserve((FIELD_SIZE+STONE_SIZE)*(FIELD_SIZE+STONE_SIZE)*8);
    for(int l = 1-STONE_SIZE; l < FIELD_SIZE; ++l) for(int m = 1-STONE_SIZE; m  < FIELD_SIZE; ++m)
    {
        for(std::size_t rotate = 0; rotate < 8; ++rotate)
        {
            data.push_back(std::make_tuple(pre_problem,l,m,rotate));
        }

        QFuture<void> threads = QtConcurrent::map(
            data,
            [this](auto& tup)
            {
                this->one_try(std::get<0>(tup), std::get<1>(tup), std::get<2>(tup), std::get<3>(tup));
            });
        threads.waitForFinished();

        for(auto& each_data : data)
        {
            auto& problem = std::get<0>(each_data);
            if(problem.field.get_score() < best_score)
            {
                answer_send(problem.field);
                print_text((boost::format("score = %d")%problem.field.get_score()).str());
                best_score = problem.field.get_score();
            }
        }
    }
*/


    std::array<int,39> start_y{{24,-7,29,13,-1,9,30,19,15,-4,18,17,20,-2,23,10,4,-6,7,-5,31,3,8,21,5,16,-3,0,11,22,14,27,2,26,28,1,12,6,25}};
    std::array<int,39> start_x{{5,19,13,26,16,20,8,10,3,23,2,27,0,29,6,-5,-7,11,-4,7,22,-3,1,-6,-2,24,-1,30,31,9,14,25,17,28,21,15,18,12,4}};

    for(int y : start_y) for(int x : start_x) for(std::size_t rotate = 0; rotate < 8; ++rotate)
    {
        problem_type problem = pre_problem;
        //if(one_try(pre_problem, l, m, rotate) != pre_problem.field.get_score()) return;
        one_try(problem, y, x, rotate);
        //if(problem.field.list_of_stones().size() == 0) continue;
        answer_send(problem.field);
        std::string const flip = problem.stones.front().get_side() == stone_type::Sides::Head ? "Head" : "Tail";
        qDebug("emit starting by %2d,%2d %3lu %s score = %3zu",y,x,rotate / 2 * 90,flip.c_str(), problem.field.get_score());
        //print_text((boost::format("score = %d")%problem.field.get_score()).str());
    }
}

void yrange::one_try(problem_type& problem, int y, int x, std::size_t const rotate)
{
    problem.stones.at(0).rotate(rotate / 2  * 90);
    if(rotate %2 == 1) problem.stones.at(0).flip();

    if(problem.field.is_puttable_basic(problem.stones.front(),y,x) == true)
    {
        //1個目
        problem.field.put_stone_basic(problem.stones.front(),y,x);

        //２個目以降
        for(std::size_t ishi = 1; ishi < problem.stones.size(); ++ishi)
        {
            stone_type& each_stone = problem.stones.at(ishi);
            search_type next = std::move(search(problem.field,each_stone));
            if(next.point.y == -FIELD_SIZE) continue;//どこにも置けなかった
            if(pass(next,each_stone) == true) continue;
            each_stone.set_angle(next.angle).set_side(next.side);
            problem.field.put_stone_basic(each_stone,next.point.y,next.point.x);
        }
    }
}

//おける場所の中から評価値の高いものを選んで返す
yrange::search_type yrange::search(field_type& _field, stone_type& stone)
{
    search_type best = {{-FIELD_SIZE,-FIELD_SIZE},0,stone_type::Sides::Head,-1,-2};
    //おける可能性がある場所すべてにおいてみる
    for(int y = 1 - STONE_SIZE; y < FIELD_SIZE; ++y) for(int x = 1 - STONE_SIZE; x < FIELD_SIZE; ++x) for(std::size_t angle = 0; angle < 360; angle += 90) for(int side = 0; side < 2; ++side)
    {
        stone.set_angle(angle).set_side(side == 0 ? stone_type::Sides::Head : stone_type::Sides::Tail);
        if(_field.is_puttable_basic(stone,y,x) == true)
        {
            int const score = eval.normalized_contact(_field,pre_problem.stones,bit_process_type(stone.get_nth(),static_cast<int>(side),angle,point_type{y,x}));
            _field.put_stone_basic(stone,y,x);
            //置けたら接してる辺を数えて配列に挿入
            double field_complexity = _field.evaluate_normalized_complexity();
            if(best.score < score || (best.score == score && best.complexity > field_complexity))
            {
                best = {point_type{y,x}, angle, static_cast<stone_type::Sides>(side), score, field_complexity};
            }
            _field.remove_stone_basic(stone);
        }
    }
    return best;
}

bool yrange::pass(search_type const& search, stone_type const& stone)
{
    if((static_cast<double>(search.score) / static_cast<double>(stone.get_side_length())) < 0.4) return true;
    else return false;
}

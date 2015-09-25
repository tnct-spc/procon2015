#include "yrange.hpp"
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

yrange::yrange(problem_type _problem)
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
        if(problem.field.list_of_stones().size() == 0) continue;
        answer_send(problem.field);
        std::string const flip = problem.stones.front().get_side() == stone_type::Sides::Head ? "Head" : "Tail";
        qDebug("emit starting by %2d,%2d %3lu %s score = %3zu",y,x,rotate / 2 * 90,flip.c_str(), problem.field.get_score());
        print_text((boost::format("score = %d")%problem.field.get_score()).str());
    }
}

void yrange::one_try(problem_type& problem, int y, int x, std::size_t const rotate)
{
    problem.stones.at(0).rotate(rotate / 2  * 90);
    if(rotate %2 == 1) problem.stones.at(0).flip();

    if(problem.field.is_puttable(problem.stones.front(),y,x) == true)
    {
        //1個目
        problem.field.put_stone(problem.stones.front(),y,x);

        //２個目以降
        for(std::size_t ishi = 1; ishi < problem.stones.size(); ++ishi)
        {
            stone_type& each_stone = problem.stones.at(ishi);
            search_type next = std::move(search(problem.field,each_stone));
            if(next.point.y == FIELD_SIZE) continue;//どこにも置けなかった
            if(pass(next,each_stone) == true) continue;
            each_stone.set_angle(next.angle).set_side(next.side);
            problem.field.put_stone(each_stone,next.point.y,next.point.x);
        }
    }
}

//評価関数
int yrange::evaluate(field_type const& field, stone_type stone,int const i, int const j)const
{
    int const n = stone.get_nth();
    int count = 0;
    for(int k = (i < 2) ? 0 : i - 1 ;k < i + STONE_SIZE && k + 1 < FIELD_SIZE; ++k) for(int l = (j < 2) ? 0 : j - 1; l < j + STONE_SIZE && l + 1 < FIELD_SIZE; ++l)
    {
        int const kl  = (field.get_raw_data().at(k).at(l) != 0 && field.get_raw_data().at(k).at(l) != n) ? 1 : 0;
        int const kl1 = (field.get_raw_data().at(k).at(l+1) != 0 && field.get_raw_data().at(k).at(l+1) != n) ? 1 : 0;
        int const k1l = (field.get_raw_data().at(k+1).at(l) != 0 && field.get_raw_data().at(k+1).at(l) != n) ? 1 : 0;

        if(field.get_raw_data().at(k).at(l) == n)
        {
            count += (kl1 + k1l);
            if(k == 0 || k == FIELD_SIZE - 1) count++;
            if(l == 0 || l == FIELD_SIZE - 1) count++;
        }
        if(field.get_raw_data().at(k).at(l+1) == n) count += kl;
        if(field.get_raw_data().at(k+1).at(l) == n) count += kl;
    }
    return count;
}

//おける場所の中から評価値の高いものを選んで返す
yrange::search_type yrange::search(field_type& _field, stone_type& stone)
{
    search_type best = {{FIELD_SIZE,FIELD_SIZE},0,stone_type::Sides::Head,-1,-2};
    //おける可能性がある場所すべてにおいてみる
    for(int i = 1 - STONE_SIZE; i < FIELD_SIZE; ++i) for(int j = 1 - STONE_SIZE; j < FIELD_SIZE; ++j) for(std::size_t angle = 0; angle < 360; angle += 90) for(int side = 0; side < 2; ++side)
    {
        stone.set_angle(angle).set_side(side == 0 ? stone_type::Sides::Head : stone_type::Sides::Tail);
        if(_field.is_puttable(stone,i,j) == true)
        {
            _field.put_stone(stone,i,j);
            //置けたら接してる辺を数えて配列に挿入
            int const score = evaluate(_field,stone,i,j);
            int const island = get_island(_field.get_raw_data());
            if(best.score < score || (best.score == score && best.island > island))
            {
                best = {point_type{i,j}, angle, side == 0 ? stone_type::Sides::Head : stone_type::Sides::Tail, score, island};
            }
            _field.remove_large_most_number_stone();
        }
    }
    return best;
}

int yrange::get_island(field_type::raw_field_type field)
{
    int num = -2;
    std::function<void(int,int,int)> recurision = [&recurision,&field](int y, int x, int num) -> void
    {
        field.at(y).at(x) = num;
        if(0 < y && field.at(y-1).at(x) == 0) recurision(y-1,x,num);
        if(y < FIELD_SIZE - 1 && field.at(y+1).at(x) == 0) recurision(y+1,x,num);
        if(0 < x && field.at(y).at(x-1) == 0) recurision(y,x-1,num);
        if(x < FIELD_SIZE - 1 && field.at(y).at(x+1) == 0) recurision(y,x+1,num);
        return;
    };
    for(int i = 0; i < FIELD_SIZE; ++i) for(int j = 0; j < FIELD_SIZE; ++j)
    {
        if(field.at(i).at(j) == 0) recurision(i,j,num--);
    }
    return -1 * num - 2;
}

bool yrange::pass(search_type const& search, stone_type const& stone)
{
    if((static_cast<double>(search.score) / static_cast<double>(stone.get_side_length())) < 0.4) return true;
    else return false;
}

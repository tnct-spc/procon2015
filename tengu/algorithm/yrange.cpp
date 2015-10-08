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

yrange::yrange(problem_type _problem, int _time_limit, evaluator _eval):time_limit(_time_limit),eval(_eval)
{
    algorithm_name = "yrange";
    origin_problem = _problem;
}

yrange::~yrange()
{
}

void yrange::run()
{
    //制限時間まで解く→完成次第送信
    print_text("解答開始");
    solve();
    //exit
    print_text("終了");
}

void yrange::solve()
{
    problem_type base_problem = origin_problem;
    limit_timer.start();
    qDebug("yrange start");
/*
    QVector<std::tuple<problem_type,int,int,std::size_t>> data;
    data.reserve((FIELD_SIZE+STONE_SIZE)*(FIELD_SIZE+STONE_SIZE)*8);
    for(int l = 1-STONE_SIZE; l < FIELD_SIZE; ++l) for(int m = 1-STONE_SIZE; m  < FIELD_SIZE; ++m)
    {
        for(std::size_t rotate = 0; rotate < 8; ++rotate)
        {
            data.push_back(std::make_tuple(origin_problem,l,m,rotate));
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
    int best_score = FIELD_SIZE * FIELD_SIZE;

    for(std::size_t stone_num = 0; stone_num < base_problem.stones.size(); ++stone_num)
    {
        for(std::size_t i = 0; i < start_x.size(); ++i) for(std::size_t j = 0; j < start_x.size(); ++j)
        {
            int y = start_y[j];
            int x = start_x[(j+i < start_x.size()) ? j+i : j+i-start_x.size()];
            for(std::size_t angle = 0; angle < 360; angle += 90) for(int side = 0; side < 2; ++side)
            {
                base_problem.stones[stone_num].set_angle(angle).set_side(static_cast<stone_type::Sides>(side));
                if(base_problem.field.is_puttable_basic(base_problem.stones[stone_num],y,x) == true)
                {
                    //Start Solve
                    problem_type problem = base_problem;
                    problem.field.put_stone_basic(base_problem.stones[stone_num],y,x);
                    one_try(problem, stone_num);
                    //Send
                    answer_send(problem.field);

                    //----------------------------------------------------------------------------------------------------------
                    int t = static_cast<int>(limit_timer.elapsed());
                    if(t > time_limit){
                        qDebug("time limit!");
                        return;
                    }
                    static int emit_count=0;
                    emit_count++;
                    int const score = problem.field.get_score();
                    std::string const flip = side == 0 ? "Head" : "Tail";
                    qDebug("emit starting by stone=%3lu x=%2d, y=%2d angle=%3lu %s score = %3d time = %d emit_count= %d",stone_num, y,x,angle,flip.c_str(), score,t,emit_count);
                    if(best_score > score)
                    {
                        print_text((boost::format("score = %d")%problem.field.get_score()).str());
                        best_score = score;
                    }
                    //-----------------------------------------------------------------------------------------------------------
                }
            }
        }
    }
}

void yrange::one_try(problem_type& problem, std::size_t stone_num)
{
    //２個目以降
    for(stone_num++; stone_num < problem.stones.size(); ++stone_num)
    {
        stone_type& each_stone = problem.stones.at(stone_num);
        search_type next = std::move(search(problem.field,each_stone));
        if(next.point.y == -FIELD_SIZE) continue;//どこにも置けなかった
        if(pass(next) == true) continue;
        each_stone.set_angle(next.angle).set_side(next.side);
        problem.field.put_stone_basic(each_stone,next.point.y,next.point.x);
    }
}

//おける場所の中から評価値の高いものを選んで返す
yrange::search_type yrange::search(field_type& _field, stone_type& stone)
{
    search_type best = {{-FIELD_SIZE,-FIELD_SIZE},0,stone_type::Sides::Head,-1,-2};
    //おける可能性がある場所すべてにおいてみる
    for(int y = 1 - STONE_SIZE; y < FIELD_SIZE; ++y) for(int x = 1 - STONE_SIZE; x < FIELD_SIZE; ++x) for(std::size_t angle = 0; angle < 360; angle += 90) for(int side = 0; side < 2; ++side)
    {
        stone.set_angle(angle).set_side(static_cast<stone_type::Sides>(side));
        if(_field.is_puttable_basic(stone,y,x) == true)
        {
            double const score = eval.normalized_contact(_field,
                                                         origin_problem.stones,
                                                         bit_process_type(stone.get_nth(),static_cast<int>(side),angle,point_type{y,x}));
            _field.put_stone_basic(stone,y,x);
            double const field_complexity = _field.evaluate_normalized_complexity();
            if(best.score < score || (best.score == score && best.complexity > field_complexity))
            {
                best = {point_type{y,x}, angle, static_cast<stone_type::Sides>(side), score, field_complexity};
            }
            _field.remove_stone_basic(stone);
        }
    }
    return best;
}

bool yrange::pass(search_type const& search)
{
    if(search.score  < 0.35) return true;
    else return false;
}

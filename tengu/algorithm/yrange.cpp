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

yrange::yrange(problem_type _problem, int _time_limit, evaluator _eval):eval(_eval),time_limit(_time_limit)
{
    algorithm_name = "yrange";
    origin_problem = _problem;
}

yrange::~yrange()
{
    print_text((boost::format("best score = %d emit answers = %d")%best_score %emit_count).str());
}

void yrange::run()
{
    limit_timer.start();
    qDebug("yrange start");

    std::array<int,39> start_y{{24,-7,29,13,-1,9,30,19,15,-4,18,17,20,-2,23,10,4,-6,7,-5,31,3,8,21,5,16,-3,0,11,22,14,27,2,26,28,1,12,6,25}};
    std::array<int,39> start_x{{5,19,13,26,16,20,8,10,3,23,2,27,0,29,6,-5,-7,11,-4,7,22,-3,1,-6,-2,24,-1,30,31,9,14,25,17,28,21,15,18,12,4}};

    for(std::size_t stone_num = 0; stone_num < origin_problem.stones.size(); ++stone_num)
    {
        for(std::size_t i = 0; i < start_x.size(); ++i) for(std::size_t j = 0; j < start_x.size(); ++j)
        {
            int y = start_y[j];
            int x = start_x[(j+i < start_x.size()) ? j+i : j+i-start_x.size()];
            for(std::size_t angle = 0; angle < 360; angle += 90) for(int side = 0; side < 2; ++side)
            {
                origin_problem.stones[stone_num].set_angle(angle).set_side(static_cast<stone_type::Sides>(side));
                if(origin_problem.field.is_puttable_basic(origin_problem.stones[stone_num],y,x) == true)
                {
                    //Start Solve
                    field_type field = origin_problem.field;
                    field.put_stone_basic(origin_problem.stones[stone_num],y,x);
                    one_try(field, stone_num);
                    //Send
                    answer_send(field);

                    //----------勝敗には関係ない----------------------------------------------------------------------------------
                    int const t = static_cast<int>(limit_timer.elapsed());
                    if(t > time_limit)
                    {
                        qDebug("time limit!");
                        return;
                    }
                    emit_count++;
                    int const score = field.get_score();
                    std::string const flip = side == 0 ? "Head" : "Tail";
                    qDebug("emit starting by stone=%3lu x=%2d, y=%2d angle=%3lu %s score = %3d time = %d emit_count= %d",stone_num, y,x,angle,flip.c_str(), score,t,emit_count);
                    if(best_score > score)
                    {
                        print_text((boost::format("score = %d")%field.get_score()).str());
                        best_score = score;
                    }
                    //-----------------------------------------------------------------------------------------------------------
                }
            }
        }
    }

}

void yrange::one_try(field_type& field, std::size_t stone_num)
{
    //２個目以降
    for(stone_num++; stone_num < origin_problem.stones.size(); ++stone_num)
    {
        stone_type& each_stone = origin_problem.stones.at(stone_num);
        search_type next = std::move(search(field,each_stone));
        //どこにも置けなかった or pass
        if(next.point.y == -FIELD_SIZE || pass(next)) continue;
        each_stone.set_angle(next.angle).set_side(next.side);
        field.put_stone_basic(each_stone,next.point.y,next.point.x);
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

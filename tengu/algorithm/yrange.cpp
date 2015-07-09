#include "yrange.hpp"
#include <queue>
#include <algorithm>
#include <QFile>
#include <QIODevice>

yrange::yrange(problem_type _problem)
{
    pre_problem = _problem;
}

yrange::~yrange()
{

}

void yrange::run()
{
    /*
    //障害物情報　事前収集
    int const row_obstacle = 0;
    int const col_obstacle = 0;
*/
    //角に適したものを探す
    /*
    std::vector<corner_type> each_corners;
    for(auto & stone : problem.stones) each_corners.push_back(stone.get_corner());
    std::vector<int>::iterator iter = std::max_element(each_corners.begin(), each_corners.end(),[](corner_type const& lhs, corner_type const& rhs){return lhs > rhs;});
    std::size_t const best = std::distance(each_corners.begin(), iter);
    */

    /**/
    //ひとつ目の石を置く　ひとつ目の石は最も角に適しているもの
    /*
    if(problem.field.is_puttable(problem.stones.at(best),FIELD_SIZE - STONE_SIZE - 1 - each_corners.at(best).point.y,-each_corners.at(best).point.x) == true)
    {
        problem.field.put_stone(problem.stones.at(best),FIELD_SIZE - STONE_SIZE - 1 - each_corners.at(best).point.y,-each_corners.at(best).point.x);
    }
    //
    for(std::size_t i = best - 1; i >= 0; --i)
    {
        if(each_zk.at(i) < 3) continue;

    }
    */
    //次の石を今おいた石の隣に置く 次の石はn[zk]以上
    //
    /**/

    //始めの一つを置く
    for(int m = 1-STONE_SIZE; m < FIELD_SIZE; ++m)for(int n = 1-STONE_SIZE; n  < FIELD_SIZE; ++n)
    {
        if(problem.field.is_puttable(problem.stones.at(0),m,n) == true)
        {
            field_type field = problem.field;
            field.put_stone(problem.stones.at(0),m,n);

            //残りを置く
            for(std::size_t stone_num = 0; stone_num < problem.stones.size(); ++stone_num)
            {
                std::vector<yrange_type> first_que;
                std::vector<yrange_type> second_que;
                std::vector<yrange_type> third_que;

                //３手先まで読む
                for(int k = 0; k < 4; ++k)
                {
                    problem.stones.at(stone_num).rotate(90);
                    for(int l = 0; l < 2; ++l)
                    {
                        for(int i = 1 - STONE_SIZE; i < FIELD_SIZE; ++i) for(int j = 1 - STONE_SIZE; j < FIELD_SIZE; ++j)
                        {

                            problem.stones.at(stone_num).flip();
                            if(field.is_puttable(problem.stones.at(stone_num),i,j)== true)
                            {
                                yrange_type first = {
                                    field.put_stone(problem.stones.at(stone_num),i,j),
                                    problem.stones.at(stone_num),
                                    {i,j}, 0, k, l, {i,j}
                                };
                                first.value = evaluate(first);
                                first_que.push_back(first);
                            }
                        }
                    }
                }
                if(first_que.size() > 10)
                {
                    std::sort(first_que.begin(),first_que.end(),[&](yrange_type lhs, yrange_type rhs){return lhs.value > rhs.value;});
                    first_que.resize(10);
                }
                for(auto each_1que : first_que)
                {
                    for(int k = 0; k < 4; ++k)
                    {
                        problem.stones.at(stone_num).rotate(90);
                        for(int l = 0; l < 2; ++l)
                        {
                            for(int i = 1 - STONE_SIZE; i < FIELD_SIZE; ++i) for(int j = 1 - STONE_SIZE; j < FIELD_SIZE; ++j)
                            {

                                problem.stones.at(stone_num).flip();
                                if(field.is_puttable(problem.stones.at(stone_num),i,j) == true)
                                {
                                    yrange_type second = each_1que;
                                    second.field.put_stone(problem.stones.at(stone_num + 1), i, j);
                                    second.stone = problem.stones.at(stone_num + 1);
                                    second.point = point_type{i,j};
                                    second.value += evaluate(second);
                                    second_que.push_back(second);
                                }
                            }
                        }
                    }
                }
                if(second_que.size() > 10)
                {
                    std::sort(second_que.begin(),second_que.end(),[&](yrange_type lhs, yrange_type rhs){return lhs.value > rhs.value;});
                    second_que.resize(10);
                }
                for(auto & each_2que : second_que)
                {
                    for(int k = 0; k < 4; ++k)
                    {
                        problem.stones.at(stone_num).rotate(90);
                        for(int l = 0; l < 2; ++l)
                        {
                            for(int i = 1 - STONE_SIZE; i < FIELD_SIZE; ++i) for(int j = 1 - STONE_SIZE; j < FIELD_SIZE; ++j)
                            {
                                problem.stones.at(stone_num).flip();
                                if(field.is_puttable(problem.stones.at(stone_num),i,j) == true)
                                {
                                    yrange_type third = each_2que;
                                    third.field.put_stone(problem.stones.at(stone_num + 2), i, j);
                                    third.stone = problem.stones.at(stone_num + 2);
                                    third.point = point_type{i,j};
                                    third.value += evaluate(third);
                                    third_que.push_back(third);
                                }
                            }
                        }
                    }
                }
                //評価値の良い物を選んで置く
                yrange_type best_element = *std::max_element(third_que.begin(),third_que.end(),[&](yrange_type lhs,yrange_type rhs){return lhs.value < rhs.value;});
                problem.stones.at(stone_num).rotate(best_element.angle * 90);
                if(best_element.flip == 0)problem.stones.at(stone_num).flip();
                field.put_stone(problem.stones.at(stone_num),best_element.first_point.y,best_element.first_point.x);
            }

            //解答の送信
            qDebug("emit starting by %d,%d",m,n);
            emit answer_ready(field);
        }
    }
}

double yrange::evaluate(yrange_type const& one)const
{
    int const y = one.point.y;
    int const x = one.point.x;
    double count = 0;
    for(int i  = y; i < y + STONE_SIZE - 1 || FIELD_SIZE - 1; i++) for(int j = x; j < x + STONE_SIZE - 1 || FIELD_SIZE - 1; ++j)
    {
        if(one.field.get_raw_data().at(i).at(j) == one.stone.get_nth() && one.field.get_raw_data().at(i).at(j+1) > 0) count++;
        if(one.field.get_raw_data().at(i).at(j) == one.stone.get_nth() && one.field.get_raw_data().at(i+1).at(j) > 0) count++;
    }
    return count / static_cast<double>(one.stone.get_side_length());
}


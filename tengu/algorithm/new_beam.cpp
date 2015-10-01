#include "new_beam.hpp"
#include "evaluator.hpp"
#include <algorithm>
#include <vector>
#include <functional>
#include <boost/format.hpp>
#include <QFile>
#include <QVector>
#include <QIODevice>
#include <QtConcurrent/QtConcurrent>
#include <QtConcurrent/QtConcurrentMap>
#include <QFuture>

new_beam::new_beam(problem_type _problem)
{
    origin_problem = _problem;
    algorithm_name = "new_beam";
    ALL_STONES_NUM = origin_problem.stones.size();

}

new_beam::~new_beam()
{
}



void new_beam::run()
{
    qDebug("new_beam start");
/*
    //はじめに置く石の場所、角度、反転分のループ
    for(int y = 1-STONE_SIZE; y < FIELD_SIZE; ++y) for(int x = 1-STONE_SIZE; x  < FIELD_SIZE; ++x) for(int angle = 0; angle < 360; angle += 90) for(int side = 0; side < 2; ++side)
    {
        origin_problem.stones.at(0).set_angle(angle).set_side(static_cast<stone_type::Sides>(side));
        //置ければ始める
        if(origin_problem.field.is_puttable(origin_problem.stones.front(),y,x) == true)
        {
            one_try(origin_problem,y,x,angle,side);
        }
    }
    */
    only_one_try(origin_problem);

}

//はじめに置く石から探索を開始する
void new_beam::only_one_try(problem_type problem)
{
    std::cout << "start only one try." << std::endl;
    for(std::size_t stone_num = 0; stone_num < problem.stones.size(); ++stone_num)
    {
        origin_problem.stones.at(stone_num).print_stone();
        std::cout << "side length = " << origin_problem.stones.at(stone_num).get_side_length() << std::endl;
        search_type next;//1層目用　空のまま渡す
        std::vector<search_type> search_vec;
        search(search_vec, std::move(next), problem.field, stone_num);

        if(search_vec .size() == 0) continue;

        for(std::size_t i = 0; i < search_vec .size(); ++i)
        {
            auto max = std::min_element(search_vec.begin(),search_vec.end(),[](const search_type& lhs, const search_type& rhs)
            {
                return lhs.score > rhs.score;
            });
            problem.stones.at(stone_num).set_side(max->stones_info_vec[0].side).set_angle(max->stones_info_vec[0].angle);
            if(eval.should_pass(problem.field,
                                {problem.stones.at(stone_num),{max->stones_info_vec[0].point.y, max->stones_info_vec[0].point.x}},
                                get_rem_stone_zk(stone_num+1))== true)
            {
                max->score *= -1;
                continue;
            }
            problem.field.put_stone_basic(problem.stones.at(stone_num), max->stones_info_vec[0].point.y, max->stones_info_vec[0].point.x);
            std::cout << stone_num << "th stone putted" << std::endl;
            break;
        }
    }
    qDebug("emit only one try. score = %3zu",problem.field.get_score());
    answer_send(problem.field);
}

//はじめに置く場所、角度、反転を固定しての試行1回
void new_beam::one_try(problem_type problem, int y, int x, std::size_t const angle, int const side)
{
    problem.stones.at(0).set_angle(angle).set_side(static_cast<stone_type::Sides>(side));

    //1個目
    problem.field.put_stone_basic(problem.stones.front(),y,x);

    //2個目以降
    for(std::size_t stone_num = 1; stone_num < problem.stones.size(); ++stone_num)
    {
        search_type next;//1層目用　空のまま渡す
        std::vector<search_type> search_vec;
        search(search_vec, std::move(next), problem.field, stone_num);

        if(search_vec .size() == 0) continue;

        for(std::size_t i = 0; i < search_vec .size(); ++i)
        {
            auto max = std::min_element(search_vec.begin(),search_vec.end(),[](const search_type& lhs, const search_type& rhs)
            {
                return lhs.score > rhs.score;
            });
            problem.stones.at(stone_num).set_side(max->stones_info_vec[0].side).set_angle(max->stones_info_vec[0].angle);
            if(eval.should_pass(problem.field,
                                {problem.stones.at(stone_num),{max->stones_info_vec[0].point.y, max->stones_info_vec[0].point.x}},
                                get_rem_stone_zk(stone_num+1))== true)
            {
                max->score *= -1;
                continue;
            }
            problem.field.put_stone_basic(problem.stones.at(stone_num), max->stones_info_vec[0].point.y, max->stones_info_vec[0].point.x);
            std::cout << stone_num << "th stone putted" << std::endl;
            break;
        }
    }

    std::string const flip = problem.stones.front().get_side() == stone_type::Sides::Head ? "Head" : "Tail";
    qDebug("emit starting by %2d,%2d %3lu %s score = %3zu",y,x,angle,flip.c_str(), problem.field.get_score());
    answer_send(problem.field);
}

//おける場所の中から評価値の高いもの3つを選びsearch_depthまで潜る
int new_beam::search(std::vector<search_type>& parental_search_vec, search_type parent, field_type& _field, std::size_t const stone_num)
{
    int count = 0;
    stone_type stone = origin_problem.stones.at(stone_num);
    std::vector<search_type> search_vec;

    //おける可能性がある場所すべてにおいてみる
    for(int y = 1 - STONE_SIZE; y < FIELD_SIZE; ++y) for(int x = 1 - STONE_SIZE; x < FIELD_SIZE; ++x) for(std::size_t angle = 0; angle < 360; angle += 90) for(int side = 0; side < 2; ++side)
    {
        stone.set_angle(angle).set_side(static_cast<stone_type::Sides>(side));

        if(_field.is_puttable_basic(stone,y,x) == true)
        {
            count++;
            _field.put_stone_basic(stone,y,x);
            double const score = stone_num == origin_problem.stones.size() - 1 ? eval.move_goodness(_field,{stone,{y,x}}) : eval.move_goodness(_field,{stone,{y,x}},origin_problem.stones.at(stone_num+1));
            //置けたら接してる辺を数えて配列に挿入
            if(search_vec.size() < 14) //14個貯まるまでは追加する
            {
                if(parent.stones_info_vec.size() == 0)
                {
                    //std::cout << eval.normalized_contact(_field,{stone,{y,x}}) << std::endl;
                    //std::cout << MAX_SEARCH_DEPTH - eval.normalized_contact(_field,{stone,{y,x}}) * MAX_SEARCH_DEPTH << std::endl;
                    search_vec.emplace_back(
                            std::vector<stones_info_type>{{point_type{y,x},angle,static_cast<stone_type::Sides>(side)}},
                            score,
                            3
                        );
                }
                else
                {
                    search_vec.emplace_back(
                            parent.stones_info_vec,
                            score,
                            parent.search_depth
                       );
                    search_vec.back().stones_info_vec.emplace_back(point_type{y,x},angle,static_cast<stone_type::Sides>(side));
                }
            }
            else
            {
                //保持している中での最悪手
                auto worst = std::min_element(search_vec.begin(),search_vec.end(),[](auto const&lhs, auto const& rhs)
                    {
                        return lhs.score < rhs.score;
                    });
                if(parent.stones_info_vec.size() == 0 && (worst->score <= score)) //1層目　保持している中の最悪手より良い
                {
                    //std::cout << eval.normalized_contact(_field,{stone,{y,x}}) << std::endl;
                    //std::cout << MAX_SEARCH_DEPTH - eval.normalized_contact(_field,{stone,{y,x}}) * MAX_SEARCH_DEPTH << std::endl;
                    search_vec.emplace_back(
                            std::vector<stones_info_type>{{point_type{y,x},angle,static_cast<stone_type::Sides>(side)}},
                            score,
                            3
                        );
                }
                else if(parent.stones_info_vec.size() > 0 && (worst->score <= parent.score + score)) //2層目以上　保持している中の最悪手より良い
                {
                    search_vec.emplace_back(
                            parent.stones_info_vec,
                            score,
                            parent.search_depth
                       );
                    search_vec.back().stones_info_vec.emplace_back(point_type{y,x},angle,static_cast<stone_type::Sides>(side));
                }
            }
            _field.remove_stone_basic();
        }
    }

    //uniqueのためにsortが必要
    std::sort(search_vec.begin(),search_vec.end(),[&](const search_type& lhs, const search_type& rhs)
        {
            return lhs.score > rhs.score;
        });
    search_vec.erase(std::unique(search_vec.begin(), search_vec.end()), search_vec.end());
    //上位3つだけ残す
    if(search_vec.size() > 3) search_vec.resize(3);

    //最下層だったら結果を親ベクトルに入れる
    if(parent.stones_info_vec.size()+1 >= MAX_SEARCH_DEPTH || stone_num >= ALL_STONES_NUM-1)
    {
        std::copy(search_vec.begin(),search_vec.end(),std::back_inserter(parental_search_vec));
    }
    //そうでなければ石を置いて潜る、帰ってきたら石を取り除く
    else
    {
        for(auto& each_ele : search_vec)
        {
            stone.set_angle(each_ele.stones_info_vec.back().angle).set_side(each_ele.stones_info_vec.back().side);
            _field.put_stone_basic(stone,each_ele.stones_info_vec.back().point.y,each_ele.stones_info_vec.back().point.x);
            if(search(parental_search_vec, each_ele, _field, stone_num+1) == 0) parental_search_vec.push_back(each_ele);
            _field.remove_stone_basic();
            stone.set_angle(0).set_side(stone_type::Sides::Head);
        }
    }
    return count;
}

int new_beam::get_rem_stone_zk(int stone_num)
{
    int sum = 0;
    for(std::size_t i = stone_num; i < origin_problem.stones.size(); ++i)
    {
        sum += origin_problem.stones.at(i).get_area();
    }
    return sum;
}

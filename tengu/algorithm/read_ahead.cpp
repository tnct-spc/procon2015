#include "read_ahead.hpp"
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

read_ahead::read_ahead(problem_type _problem)
{
    pre_problem = _problem;
    algorithm_name = "read_ahead";
    search_depth = 3;
    ALL_STONES_NUM = pre_problem.stones.size();
}

read_ahead::read_ahead(problem_type _problem, evaluator _eval) : eval(_eval)
{
    pre_problem = _problem;
    algorithm_name = "read_ahead";
    search_depth = 3;
    ALL_STONES_NUM = pre_problem.stones.size();
}

read_ahead::~read_ahead()
{
}



void read_ahead::run()
{
    qDebug("read_ahead start");

    //はじめに置く石の場所、角度、反転分のループ
    for(int y = 1-STONE_SIZE; y < FIELD_SIZE; ++y) for(int x = 1-STONE_SIZE; x  < FIELD_SIZE; ++x) for(int angle = 0; angle < 360; angle += 90) for(int side = 0; side < 2; ++side)
    {
        pre_problem.stones.at(0).set_angle(angle).set_side(static_cast<stone_type::Sides>(side));
        //置ければ始める
        if(pre_problem.field.is_puttable_basic(pre_problem.stones.front(),y,x) == true)
        {
            one_try(pre_problem,y,x,angle,side);
        }
    }
}

//はじめに置く場所、角度、反転を固定しての試行1回
void read_ahead::one_try(problem_type problem, int y, int x, std::size_t const angle, int const side)
{
    problem.stones.at(0).set_angle(angle).set_side(static_cast<stone_type::Sides>(side));
    if(problem.field.is_puttable_basic(problem.stones.front(),y,x) == false)
    {
        std::cout << "okenai" << std::endl;
        return;
    }
    //1個目
    problem.field.put_stone_basic(problem.stones.front(),y,x);

    //search_type next;//1層目用　空のまま渡す
    //2個目以降
    for(std::size_t stone_num = 1; stone_num < problem.stones.size(); ++stone_num)
    {
        search_type next;//1層目用　空のまま渡す
        std::vector<search_type> search_vec;
        search(search_vec, std::move(next), problem.field, stone_num);

        if(search_vec .size() == 0) continue;

        for(std::size_t i = 0; i < search_vec .size(); ++i)
        {
            auto max = std::min_element(search_vec .begin(),search_vec .end(),[](const search_type& lhs, const search_type& rhs)
            {
                return lhs.score == rhs.score ? lhs.island < rhs.island : lhs.score > rhs.score;
            });
            if(pass(*max,problem.stones.at(stone_num)) == true)
            {
                max->score *= -1;
                continue;
            }
            problem.stones.at(stone_num).set_side(max->stones_info_vec[0].side).set_angle(max->stones_info_vec[0].angle);
            if(problem.field.is_puttable_basic(problem.stones.at(stone_num), max->stones_info_vec[0].point.y, max->stones_info_vec[0].point.x) == false)
            {
                std::cout << "dame" << std::endl;
            }
            problem.field.put_stone_basic(problem.stones.at(stone_num), max->stones_info_vec[0].point.y, max->stones_info_vec[0].point.x);
            //print_text((boost::format("putted %dth stone")%stone_num).str());
            std::cout << stone_num << "th stone putted" << std::endl;
/*
            next = std::move(*max);
            std::cout << "vec size = " << next.stones_info_vec.size() << std::endl;
            next.stones_info_vec.erase(next.stones_info_vec.begin(),next.stones_info_vec.begin()+1);
            std::cout << "vec size = " << next.stones_info_vec.size() << std::endl << "----------------------" << std::endl;*/
            break;
        }
    }

    std::string const flip = problem.stones.front().get_side() == stone_type::Sides::Head ? "Head" : "Tail";
    qDebug("emit starting by %2d,%2d %3lu %s score = %3zu",y,x,angle,flip.c_str(), problem.field.get_score());
    emit answer_ready(problem.field);
}

//おける場所の中から評価値の高いもの3つを選びsearch_depthまで潜る
int read_ahead::search(std::vector<search_type>& parental_search_vec, search_type s, field_type& _field, std::size_t const stone_num)
{
    int count = 0;
    stone_type stone = pre_problem.stones.at(stone_num);
    std::vector<search_type> search_vec;

    //おける可能性がある場所すべてにおいてみる
    for(int y = 1 - STONE_SIZE; y < FIELD_SIZE; ++y) for(int x = 1 - STONE_SIZE; x < FIELD_SIZE; ++x) for(std::size_t angle = 0; angle < 360; angle += 90) for(int side = 0; side < 2; ++side)
    {
        stone.set_angle(angle).set_side(static_cast<stone_type::Sides>(side));

        if(_field.is_puttable_basic(stone,y,x) == true)
        {
            count++;
            int const score = eval.normalized_contact(_field,
                                                      pre_problem.stones,
                                                      bit_process_type(stone_num+1,side,angle,point_type{y,x})
                                                      );
            _field.put_stone_basic(stone,y,x);
            double const field_complexity = _field.evaluate_normalized_complexity();
            //置けたら接してる辺を数えて配列に挿入
            if(search_vec.size() < 14) //14個貯まるまでは追加する
            {
                if(s.stones_info_vec.size() == 0)
                {
                    search_vec.emplace_back(
                            std::vector<stones_info_type>{{point_type{y,x},angle,static_cast<stone_type::Sides>(side)}},
                            score,
                            field_complexity
                        );
                }
                else
                {
                    search_vec.emplace_back(
                            s.stones_info_vec,
                            s.score + score,
                            field_complexity
                       );
                    search_vec.back().stones_info_vec.emplace_back(point_type{y,x},angle,static_cast<stone_type::Sides>(side));
                }
            }
            else
            {
                //保持している中での最悪手
                auto min = std::min_element(search_vec.begin(),search_vec.end(),[](auto const&lhs, auto const& rhs)
                    {
                        return lhs.score == rhs.score ? lhs.island > rhs.island : lhs.score < rhs.score;
                    });
                if(s.stones_info_vec.size() == 0 && (min->score <= score)) //1層目　保持している中の最悪手より良い
                {
                    search_vec.emplace_back(
                            std::vector<stones_info_type>{{point_type{y,x},angle,static_cast<stone_type::Sides>(side)}},
                            score,
                            field_complexity
                        );
                }
                else if(s.stones_info_vec.size() > 0 && (min->score <= s.score + score)) //2層目以上　保持している中の最悪手より良い
                {
                    search_vec.emplace_back(
                            s.stones_info_vec,
                            s.score + score,
                            field_complexity
                       );
                    search_vec.back().stones_info_vec.emplace_back(point_type{y,x},angle,static_cast<stone_type::Sides>(side));
                }
            }
            _field.remove_stone_basic(stone);
        }
    }

    //uniqueのためにsortが必要
    std::sort(search_vec.begin(),search_vec.end(),[&](const search_type& lhs, const search_type& rhs)
        {
            return lhs.score == rhs.score ? lhs.complexity < rhs.complexity : lhs.score > rhs.score;
        });
    search_vec.erase(std::unique(search_vec.begin(), search_vec.end()), search_vec.end());
    //上位3つだけ残す
    if(search_vec.size() > 3) search_vec.resize(3);

    //最下層だったら結果を親ベクトルに入れる
    if(s.stones_info_vec.size()+1 >= search_depth || stone_num >= ALL_STONES_NUM-1)
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
            _field.remove_stone_basic(stone);
            stone.set_angle(0).set_side(stone_type::Sides::Head);
        }
    }
    return count;
}

bool read_ahead::pass(search_type const& search, stone_type const& stone)
{
    if((static_cast<double>(search.score) / static_cast<double>(stone.get_side_length())) < 0.5) return true;
    else return false;
}


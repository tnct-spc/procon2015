#include "read_ahead.hpp"
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

    //LAH = 1600 / pre_problem.stones.size();
    LAH = 3;
    //print_text((boost::format("LAH = %d")%LAH).str());
    qDebug("LAH = %d",LAH);
    STONE_NUM = pre_problem.stones.size();
}

read_ahead::~read_ahead()
{
}



void read_ahead::run()
{
    qDebug("read_ahead start");
/*
    QVector<std::tuple<int,int,std::size_t>> data;
    data.reserve((FIELD_SIZE+STONE_SIZE)*(FIELD_SIZE+STONE_SIZE)*8);
    for(int l = 1-STONE_SIZE; l < FIELD_SIZE; ++l) for(int m = 1-STONE_SIZE; m  < FIELD_SIZE; ++m) for(int rotate = 0; rotate < 8; ++rotate)
    {
        data.push_back(std::make_tuple(l,m,rotate));
    }
    QFuture<void> threads = QtConcurrent::map(
                data,
                [this](auto& tup)
                {
                    this->one_try(pre_problem,std::get<0>(tup),std::get<1>(tup),std::get<2>(tup));

                });
    threads.waitForFinished();
*/
/*
    for(int l = 1-STONE_SIZE; l < FIELD_SIZE; ++l) for(int m = 1-STONE_SIZE; m  < FIELD_SIZE; ++m) for(int rotate = 0; rotate < 8; ++rotate)
    {
        one_try(pre_problem,l,m,rotate);
    }
*/
    //-4, 4  90 Tail
    one_try(pre_problem,-4,4,3);

}

//
void read_ahead::one_try(problem_type problem, int y, int x, std::size_t const rotate)
{
    problem.stones.at(0).rotate(rotate / 2 * 90);
    if(rotate %2 == 1) problem.stones.at(0).flip();

    if(problem.field.is_puttable(problem.stones.front(),y,x) == true)
    {
        //1個目
        problem.field.put_stone(problem.stones.front(),y,x);

        //2個目以降
        for(std::size_t ishi = 1; ishi < problem.stones.size(); ++ishi)
        {
            std::vector<search_type> sv;
            search_type one;
            one.field = problem.field;
            search(sv, std::move(one), ishi);

            if(sv.size() == 0) continue;
            std::sort(sv.begin(),sv.end(),[&](const search_type& lhs, const search_type& rhs)
                {
                    return lhs.score == rhs.score ? get_island(lhs.field.get_raw_data()) < get_island(rhs.field.get_raw_data()) : lhs.score > rhs.score;
                });

            /*
            for(auto const& each_ele : sv)
            {
                std::string const flip = each_ele.flip == stone_type::Sides::Head ? "Head" : "Tail";
                std::cout << each_ele.point.y << " " << each_ele.point.x << " " << each_ele.rotate << " " << flip << " "<< each_ele.score << std::endl;
            }
            std::cout << std::endl;
            */

            for(auto& each_ele : sv)
            {
                if(pass(each_ele,problem.stones.at(ishi)) == true) continue;
                else
                {
                    if(each_ele.iv[0].side == stone_type::Sides::Tail) problem.stones.at(ishi).flip();
                    problem.stones.at(ishi).rotate(each_ele.iv[0].angle);
                    problem.field.put_stone(problem.stones.at(ishi), each_ele.iv[0].point.y, each_ele.iv[0].point.x);
                    print_text((boost::format("putted %dth stone")%ishi).str());
                    //std::cout << ishi << "th stone putted" << std::endl;
                    break;
                }
            }
        }

        std::string const flip = problem.stones.front().get_side() == stone_type::Sides::Head ? "Head" : "Tail";
        qDebug("emit starting by %2d,%2d %3lu %s score = %3zu",y,x,rotate / 2 * 90,flip.c_str(), problem.field.get_score());
        emit answer_ready(problem.field);
    }
}

//評価関数
double read_ahead::evaluate(field_type const& field, stone_type stone,int const i, int const j)const
{
    int const n = stone.get_nth();
    double count = 0;
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
int read_ahead::search(std::vector<search_type>& sv, search_type s, std::size_t const ishi)
{
    int count = 0;
    stone_type stone = pre_problem.stones.at(ishi);
    std::vector<search_type> search_vec;
    //おける可能性がある場所すべてにおいてみる
    for(int i = 1 - STONE_SIZE; i < FIELD_SIZE; ++i) for(int j = 1 - STONE_SIZE; j < FIELD_SIZE; ++j) for(int rotate = 0; rotate < 8; ++rotate)
    {
        if(rotate %2 == 0) stone.rotate(90);
        else stone.flip();
        if(s.field.is_puttable(stone,i,j) == true)
        {
            count++;
            field_type& field = s.field;
            field.put_stone(stone,i,j);
            double const score = evaluate(field,stone,i,j);
            int const island = get_island(field.get_raw_data());
            //置けたら接してる辺を数えて配列に挿入
            if(search_vec.size() < 14) //14個貯まるまでは追加する
            {
                if(s.iv.size() == 0)
                {
                    search_vec.emplace_back(
                            field,
                            std::vector<stones_info_type>{{point_type{i,j},stone.get_angle(),stone.get_side()}},
                            score,
                            island
                        );
                }
                else
                {
                    search_vec.emplace_back(
                            field,
                            s.iv,
                            s.score + score,
                            island
                       );
                    search_vec.back().iv.emplace_back(point_type{i,j},stone.get_angle(),stone.get_side());
                }
            }
            else
            {
                //保持している中での最悪手
                auto min = std::min_element(search_vec.begin(),search_vec.end(),[](auto const&lhs, auto const& rhs)
                    {
                        return lhs.score == rhs.score ? lhs.island > rhs.island : lhs.score < rhs.score;
                    });
                if(s.iv.size() == 0 && (min->score <= score)) //1層目　保持している中の最悪手より良い
                {
                    search_vec.emplace_back(
                            field,
                            std::vector<stones_info_type>{{point_type{i,j},stone.get_angle(),stone.get_side()}},
                            score,
                            island
                        );
                }
                else if(s.iv.size() > 0 && (min->score <= s.score + score)) //2層目以上　保持している中の最悪手より良い
                {
                    search_vec.emplace_back(
                            field,
                            s.iv,
                            s.score + score,
                            island
                       );
                    search_vec.back().iv.emplace_back(point_type{i,j},stone.get_angle(),stone.get_side());
                }
            }
            field.remove_large_most_number_and_just_before_stone();
        }
    }

    std::sort(search_vec.begin(),search_vec.end(),[&](const search_type& lhs, const search_type& rhs)
        {
            return lhs.score == rhs.score ? lhs.island < rhs.island : lhs.score > rhs.score;
        });
    search_vec.erase(std::unique(search_vec.begin(), search_vec.end()), search_vec.end());
    if(search_vec.size() > 3) search_vec.resize(3);

    if(s.iv.size()+1 >= LAH || ishi >= STONE_NUM-1)
    {
        std::copy(search_vec.begin(),search_vec.end(),std::back_inserter(sv));
    }
    else
    {
        for(auto& each_ele : search_vec)
        {
            if(search(sv, each_ele, ishi+1) == 0) sv.push_back(each_ele);
        }
    }
    return count;
}


int read_ahead::get_island(field_type::raw_field_type field)
{
    int num = -1;
    int const y_min = 0;
    int const y_max = FIELD_SIZE;
    int const x_min = 0;
    int const x_max = FIELD_SIZE;

    std::vector<int> result (FIELD_SIZE * FIELD_SIZE,0);
    std::function<void(int,int,int)> recurision = [&recurision,&field](int y, int x, int num) -> void
    {
        field.at(y).at(x) = num;
        if(1 < y && field.at(y-1).at(x) == 0) recurision(y-1,x,num);
        if(y < FIELD_SIZE - 1 && field.at(y+1).at(x) == 0) recurision(y+1,x,num);
        if(1 < x && field.at(y).at(x-1) == 0) recurision(y,x-1,num);
        if(x < FIELD_SIZE - 1 && field.at(y).at(x+1) == 0) recurision(y,x+1,num);
        return;
    };
    for(int i = y_min; i < y_max; ++i) for(int j = x_min; j < x_max; ++j)
    {
        if(field.at(i).at(j) == 0) recurision(i,j,num--);
    }
    for(int i = y_min; i < y_max; ++i) for(int j = x_min; j < x_max; ++j)
    {
        if(field.at(i).at(j) < 0)
        {
            result.at(field.at(i).at(j) * -1)++;
        }
    }
    return std::count_if(result.begin(),result.end(),[&](int hs){return 0 < hs && hs < 5;});
}

bool read_ahead::pass(search_type const& search, stone_type const& stone)
{
    if((static_cast<double>(search.score) / static_cast<double>(stone.get_side_length())) < 0.5) return true;
    else return false;
}


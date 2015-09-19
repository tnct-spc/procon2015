#include "raq.hpp"
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

raq::raq(problem_type _problem)
{
    pre_problem = _problem;
}

raq::~raq()
{
}

void raq::run()
{
    qDebug("raq start");

    //
    stone_type& stone = pre_problem.stones.at(0);
    for(int l = 1-STONE_SIZE; l < FIELD_SIZE; ++l) for(int m = 1-STONE_SIZE; m  < FIELD_SIZE; ++m)
    {
        for(std::size_t rotate = 0; rotate < 8; ++rotate)
        {
            if(rotate %2 == 0) stone.rotate(90);
            else stone.flip();
            if(pre_problem.field.is_puttable(stone,l,m) == true)
            {
                field_type field;
                field.put_stone(stone,l,m);
                std::vector<raq::search_type> one {{field,0}};
                steps(one);
            }
        }
    }
}

void raq::one_step(field_type& _field, std::size_t ishi,std::vector<search_type>& sv)
{
    if(ishi == pre_problem.stones.size())
    {
        emit answer_ready(_field);
        print_text((boost::format("score = %d")%_field.get_score()).str());
        return;
    }

    stone_type stone = pre_problem.stones.at(ishi);
    for(int i = 1 - STONE_SIZE; i < FIELD_SIZE; ++i) for(int j = 1 - STONE_SIZE; j < FIELD_SIZE; ++j) for(int rotate = 0; rotate < 8; ++rotate)
    {
        if(rotate % 2 == 0) stone.rotate(90);
        else stone.flip();
        if(problem.field.is_puttable(stone,i,j) == true)
        {
            field_type field = _field;
            field.put_stone(stone,i,j);
            //置けたら接してる辺を数えて配列に挿入
            if(pass(field, pre_problem.stones,evaluate(field,ishi,i,j),ishi) == true) continue;
            sv.push_back({field,evaluate(field,ishi,i,j)});
        }
    }

    std::sort(sv.begin(),sv.end(),[&](const search_type& lhs, const search_type& rhs)
    {
        return lhs.score == rhs.score ? get_island(lhs.field.get_raw_data()) < get_island(rhs.field.get_raw_data()) : lhs.score > rhs.score;
    });

    return;
}

void raq::steps(std::vector<raq::search_type> result)
{
    /*
    QVector<std::tuple<problem_type,std::vector<raq::search_type>>> data;
    std::vector<raq::search_type> r;
    int ishi = 1;

    while(result.size() > 0)
    {
        for(auto const& each : result)
        {
            data.push_back(std::make_tuple(each,r));
        }

        QFuture<void> threads = QtConcurrent::map(
            data,
            [this,&ishi](auto& tup)
            {
                this->one_step(std::get<0>(tup), ishi, std::get<1>(tup));
            });
        threads.waitForFinished();

        result.clear();
        for(auto const& tup : data)
        {
            std::copy(std::get<1>(tup).begin(),std::get<1>(tup).end(),std::back_inserter(result));
        }

        std::sort(result.begin(),result.end(),[&](search_type const& lhs, search_type const& rhs)
        {
            return lhs.score == rhs.score ? get_island(lhs.field.get_raw_data()) < get_island(rhs.field.get_raw_data()) : lhs.score > rhs.score;
        });

        if(result.size() > 10) result.resize(10);

        ishi++;
    }
    */
}

//評価関数
int raq::evaluate(field_type& _field, stone_type stone,int const y, int const x)const
{
    field_type field = _field;
    int const n = stone.get_nth();
    int score = 0; // 接している辺
    int complexity = field.evaluate_normalized_complexity(); //場の複雑さ
    field.put_stone(stone,y,x);

    for(int k = (y < 2) ? 0 : y - 1 ;k < y + STONE_SIZE && k + 1 < FIELD_SIZE; ++k) for(int l = (x < 2) ? 0 : x - 1; l < x + STONE_SIZE && l + 1 < FIELD_SIZE; ++l)
    {
        int const kl  = (field.get_raw_data().at(k).at(l) != 0 && field.get_raw_data().at(k).at(l) < n) ? 1 : 0;
        int const kl1 = (field.get_raw_data().at(k).at(l+1) != 0 && field.get_raw_data().at(k).at(l+1) < n) ? 1 : 0;
        int const k1l = (field.get_raw_data().at(k+1).at(l) != 0 && field.get_raw_data().at(k+1).at(l) < n) ? 1 : 0;

        if(field.get_raw_data().at(k).at(l) == n)
        {
            score += (kl1 + k1l);
            if(k == 0 || k == FIELD_SIZE - 1) score++;
            if(l == 0 || l == FIELD_SIZE - 1) score++;
        }
        if(field.get_raw_data().at(k).at(l+1) == n) score += kl;
        if(field.get_raw_data().at(k+1).at(l) == n) score += kl;
    }

    return complexity - score;
}

int raq::get_island(field_type::raw_field_type field)
{
    int num = -1;
    int const y_min = 0;
    int const y_max = FIELD_SIZE;
    int const x_min = 0;
    int const x_max = FIELD_SIZE;

    std::vector<int> result (FIELD_SIZE * FIELD_SIZE,0);
    std::function<void(int,int,int)> recurision = [&recurision,&field,&y_min,&y_max,&x_min,&x_max](int y, int x, int num) -> void
    {
        field.at(y).at(x) = num;
        if(y_min + 1 < y && field.at(y-1).at(x) == 0) recurision(y-1,x,num);
        if(y < y_max - 1 && field.at(y+1).at(x) == 0) recurision(y+1,x,num);
        if(x_min + 1 < x && field.at(y).at(x-1) == 0) recurision(y,x-1,num);
        if(x < x_max - 1 && field.at(y).at(x+1) == 0) recurision(y,x+1,num);
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
    return std::count_if(result.begin(),result.end(),[&](int hs){return hs != 0;});
}

bool raq::pass(field_type& field, std::vector<stone_type> const& stones,double score, int stone_num)
{
    int sum = 0;
    for(std::size_t i = stone_num; i < stones.size(); ++i) sum += stones.at(stone_num).get_area();
    int remnant = field.get_score() - sum;
    return (static_cast<double>(score) / static_cast<double>(stones.at(stone_num).get_side_length())) + remnant / FIELD_SIZE * FIELD_SIZE < 0.55;
}
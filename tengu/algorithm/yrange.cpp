#include "yrange.hpp"
#include <algorithm>
#include <array>
#include <functional>
#include <vector>
#include <thread>
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
    qDebug("yrange start");
    std::vector<std::thread> threads ((FIELD_SIZE+STONE_SIZE)*(FIELD_SIZE+STONE_SIZE)*8);
    for(int l = 1-STONE_SIZE; l < FIELD_SIZE; ++l) for(int m = 1-STONE_SIZE; m  < FIELD_SIZE; ++m) for(std::size_t rotate = 0; rotate < 8; ++rotate)
    {
        threads.emplace_back(std::bind(&yrange::one_try, this, pre_problem,l,m,rotate));
    }
    for (std::thread &th : threads) th.join();
}

void yrange::one_try(problem_type problem, int x, int y, std::size_t const rotate)
{
    problem.stones.at(0).rotate(rotate / 2  * 90);
    if(rotate %2 == 1) problem.stones.at(0).flip();

    if(problem.field.is_puttable(problem.stones.at(0),x,y) == true)
    {
        //1個目
        problem.field.put_stone(problem.stones.at(0),x,y);
        problem.stones.erase(problem.stones.begin());
        //２個目以降
        for(auto& each_stone : problem.stones)
        {
            search_type next = std::move(search(problem.field,each_stone));
            if(next.point.y == FIELD_SIZE) continue;//どこにも置けなかった
            if(next.flip != each_stone.get_side()) each_stone.flip();
            each_stone.rotate(next.rotate);
            problem.field.put_stone(each_stone,next.point.y,next.point.x);
        }
        std::string const flip = problem.stones.at(0).get_side() == stone_type::Sides::Head ? "Head" : "Tail";
        qDebug("emit starting by %2d,%2d %2lu %s",x,y,rotate / 2,flip.c_str());
        emit answer_ready(problem.field);
    }

}

//評価関数
int yrange::evaluate(field_type const& field, stone_type stone,int const i, int const j)const
{
    int const n = stone.get_nth();
    int count = 0;
    for(int k = (i < 2) ? 0 : i - 1 ;k < i + STONE_SIZE && k + 1 < FIELD_SIZE; ++k) for(int l = (j < 2) ? 0 : j - 1; l < j + STONE_SIZE && l + 1 < FIELD_SIZE; ++l)
    {
        int const kl  = (field.get_raw_data().at(k).at(l) != 0 && field.get_raw_data().at(k).at(l) < n) ? 1 : 0;
        int const kl1 = (field.get_raw_data().at(k).at(l+1) != 0 && field.get_raw_data().at(k).at(l+1) < n) ? 1 : 0;
        int const k1l = (field.get_raw_data().at(k+1).at(l) != 0 && field.get_raw_data().at(k+1).at(l) < n) ? 1 : 0;

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
search_type yrange::search(field_type& _field, stone_type& stone)
{
    std::vector<search_type> search_vec;
    //おける可能性がある場所すべてにおいてみる
    for(int i = 1 - STONE_SIZE; i < FIELD_SIZE; ++i) for(int j = 1 - STONE_SIZE; j < FIELD_SIZE; ++j) for(int rotate = 0; rotate < 8; ++rotate)
    {
        if(rotate % 2 == 0) stone.rotate(90);
        else stone.flip();
        if(_field.is_puttable(stone,i,j) == true)
        {
            field_type field = _field;
            field.put_stone(stone,i,j);
            //置けたら接してる辺を数えて配列に挿入
            search_vec.push_back(search_type{point_type{i,j},stone.get_angle(),stone.get_side(),evaluate(field,stone,i,j)});
        }
    }
    if(search_vec.size() == 0) return search_type{point_type{FIELD_SIZE,FIELD_SIZE},0,stone_type::Sides::Head,0};
    std::sort(search_vec.begin(),search_vec.end(),[](const search_type& lhs, const search_type& rhs) {return lhs.score > rhs.score;});
    return std::move(search_vec.at(0));
}

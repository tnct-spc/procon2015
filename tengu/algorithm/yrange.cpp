#include "yrange.hpp"
#include <queue>
#include <algorithm>
#include <vector>
#include <cmath>
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
    for(int l = 1-STONE_SIZE; l < FIELD_SIZE; ++l) for(int m = 1-STONE_SIZE; m  < FIELD_SIZE; ++m)
    {
        for(int rotate = 0; rotate < 4; ++rotate) for(int flip = 0; flip < 2; ++flip)
        {
            problem = pre_problem;
            stone_type& stone = problem.stones.at(0);
            stone.rotate(rotate * 90);
            if(flip == 1) stone.flip();

            if(problem.field.is_puttable(stone,l,m) == true)
            {
                std::vector<stone_type> small_stones;
                for(auto& each_stone : problem.stones)
                {
                    //1個目
                    if(each_stone.get_nth() == 1)
                    {
                        problem.field.put_stone(each_stone,l,m);
                        continue;
                    }
                    //２個目以降threshold_zkより大きい物
                    small_stones.clear();
                    search_type next = search(problem.field,each_stone);
                    if(next.point.y == FIELD_SIZE) continue;
                    if(next.flip == 1) each_stone.flip();
                    each_stone.rotate(next.rotate);
                    problem.field.put_stone(each_stone,next.point.y,next.point.x);
                    //std::cout << each_stone.get_nth() << std::endl;
                    l = next.point.y;
                    m = next.point.x;
                }
                qDebug("emit starting by %2d,%2d %2d %2d",l,m,rotate,flip);
                emit answer_ready(problem.field);
            }
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
/*
        if(field.get_raw_data().at(k).at(l) == n && field.get_raw_data().at(k).at(l+1) != 0 && field.get_raw_data().at(k).at(l+1) < n)
        {
            //std::cout << "1";
            count++;
        }
        if(field.get_raw_data().at(k).at(l+1) == n && field.get_raw_data().at(k).at(l) != 0 && field.get_raw_data().at(k).at(l) < n)
        {
            //std::cout << "2";
            count++;
        }
        if(field.get_raw_data().at(k+1).at(l) == n && field.get_raw_data().at(k).at(l) != 0 && field.get_raw_data().at(k).at(l) < n)
        {
            count++;
            //std::cout << "3";
        }
        if(field.get_raw_data().at(k).at(l) == n && field.get_raw_data().at(k+1).at(l) != 0 && field.get_raw_data().at(k+1).at(l) < n)
        {
            //std::cout << "4";
            count++;
        }
        if((k == 0 && field.get_raw_data().at(k).at(l) == n) || (k == FIELD_SIZE-1 && field.get_raw_data().at(k).at(l) == n))
        {
            //std::cout << "5";
            count++;
        }
        if((l == 0 && field.get_raw_data().at(k).at(l) == n) || (l == FIELD_SIZE-1 && field.get_raw_data().at(k).at(l) == n))
        {
            //std::cout << "6";
            count++;
        }
        //std::cout << std::endl;
        */
    }
    return count;
/*
    for(auto const& each_row : field.get_raw_data())
    {
        for(auto const& each_block : each_row)
        {
            std::printf("%2d ",each_block);
        }
        std::cout << std::endl;
    }
    std::cout << i << " " << j << " " << n << " " << count << std::endl;
*/

}

//おける場所の中から評価値の高いものを選んで返す
search_type yrange::search(field_type& _field, stone_type const& _stone)
{
    //if(n > 10) return search_type{point_type{FIELD_SIZE,FIELD_SIZE},0,0,0};
    std::vector<search_type> search_vec;
    //おける可能性がある場所すべてにおいてみる
    for(int i = 1 - STONE_SIZE; i < FIELD_SIZE; ++i) for(int j = 1 - STONE_SIZE; j < FIELD_SIZE; ++j) for(int rotate = 0; rotate < 4; ++rotate) for(int flip = 0; flip < 2; ++flip)
    {
        if(i == 0 && j == 0)continue;
        stone_type stone = _stone;
        if(flip == 1) stone.flip();
        stone.rotate(rotate * 90);
        if(_field.is_puttable(stone,i,j) == true)
        {
            field_type field = _field;
            field.put_stone(stone,i,j);
            //置けたら接してる辺を数える
            search_vec.push_back(search_type{point_type{i,j},rotate*90,flip,evaluate(field,stone,i,j)});
        }
    }
    if(search_vec.size() == 0) return search_type{point_type{FIELD_SIZE,FIELD_SIZE},0,0,0};
    std::sort(search_vec.begin(),search_vec.end(),[](const search_type& lhs, const search_type& rhs) {return lhs.score > rhs.score;});
    return search_vec.at(0);
}

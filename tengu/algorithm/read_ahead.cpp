#include "read_ahead.hpp"
#include <queue>
#include <algorithm>
#include <vector>
#include <boost/format.hpp>
#include <QFile>
#include <QIODevice>

read_ahead::read_ahead(problem_type _problem)
{
    pre_problem = _problem;
}

read_ahead::~read_ahead()
{
}

void read_ahead::run()
{
    qDebug("read_ahead start");
    for(int l = 1-STONE_SIZE; l < FIELD_SIZE; ++l) for(int m = 1-STONE_SIZE; m  < FIELD_SIZE; ++m) for(int rotate = 0; rotate < 4; ++rotate) for(int flip = 0; flip < 2; ++flip)
    {
        problem = pre_problem;
        problem.stones.at(0).rotate(rotate * 90);
        if(flip == 1) problem.stones.at(0).flip();

        for(std::size_t ishi = 1; ishi < problem.stones.size(); ++ishi)
        {
            std::vector<search_type> sv;

            search_type first;
            first.field = problem.field;
            search(sv, first, ishi);

            if(sv.size() == 0) continue;
            std::sort(sv.begin(),sv.end(),[&](search_type const& lhs, search_type const&rhs){return lhs.score > rhs.score;});
            if(sv.at(0).flip == stone_type::Sides::Tail) problem.stones.at(ishi).flip();
            problem.stones.at(ishi).rotate(sv.at(0).rotate);
            problem.field.put_stone(problem.stones.at(ishi), sv.at(0).point.y, sv.at(0).point.x);
            print_text((boost::format("%d th stone puted.")%ishi).str());
        }
        qDebug("emit starting by %2d,%2d %2d %2d",l,m,rotate,flip);
        emit answer_ready(problem.field);
        return;
    }
}

//評価関数
int read_ahead::evaluate(field_type const& field, stone_type stone,int const i, int const j)const
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
void read_ahead::search(std::vector<search_type>& sv, search_type s, std::size_t const ishi)
{
    if(s.rank == 3 || ishi >= problem.stones.size())
    {
        sv.push_back(s);
        return;
    }
    std::vector<search_type> search_vec;
    //おける可能性がある場所すべてにおいてみる
    for(int i = 1 - STONE_SIZE; i < FIELD_SIZE; ++i) for(int j = 1 - STONE_SIZE; j < FIELD_SIZE; ++j) for(int rotate = 0; rotate < 4; ++rotate) for(int flip = 0; flip < 2; ++flip)
    {
        //std::cout << "koko1" << std::endl;
        stone_type stone = problem.stones.at(ishi);
        if(flip == 1) stone.flip();
        stone.rotate(rotate * 90);
        //std::cout << "koko2" << std::endl;
        if(s.field.is_puttable(stone,i,j) == true)
        {
            field_type field = s.field;
            field.put_stone(stone,i,j);
            //置けたら接してる辺を数えて配列に挿入
            if(s.rank == 1)
            {
                search_vec.push_back(search_type
                {
                                         field,
                                         point_type{i,j},
                                         stone.get_angle(),
                                         stone.get_side(),
                                         s.score + evaluate(field,stone,i,j),
                                         s.rank + 1
                                     });
            }
            else
            {
                search_vec.push_back(search_type
                {
                                         field,
                                         s.point,
                                         s.rotate,
                                         s.flip,
                                         s.score + evaluate(field,stone,i,j),
                                         s.rank + 1
                                     });
            }
        }
        //std::cout << "koko3" << std::endl;
    }

    std::sort(search_vec.begin(),search_vec.end(),[](const search_type& lhs, const search_type& rhs) {return lhs.score > rhs.score;});

    std::size_t i;
    for(i = 1; i < 10 && i < search_vec.size() && search_vec.at(i).score == search_vec.at(i - 1).score; ++i);
    if(search_vec.size() > i) search_vec.resize(i);

    if(s.rank == 2 || ishi >= problem.stones.size())
    {
        for(auto& each_ele : search_vec)
        {
            sv.push_back(each_ele);
        }
    }
    else
    {
        for(auto& each_ele : search_vec)
        {
            search(sv, each_ele, ishi+1);
        }
    }
}

#include "yrange3.hpp"
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

yrange3::yrange3(problem_type _problem)
{
    pre_problem = _problem;
}

yrange3::~yrange3()
{
}

void yrange3::run()
{
    qDebug("yrange3 start");
    field_type& field = pre_problem.field;
    std::vector<stone_type>& stones = pre_problem.stones;
    for(std::size_t stone_num = stones.size() - 1; stone_num > stones.size() / 2; --stone_num)
    {
        stone_type& stone = stones.at(stone_num);
        std::vector<search_type> sv (4);
        sv.push_back(ur_search(field,stone));
        sv.push_back(ul_search(field,stone));
        sv.push_back(dr_search(field,stone));
        sv.push_back(dl_search(field,stone));

        auto const best = *std::max_element(sv.begin(),sv.end(),[](auto const& rhs, auto const& lhs){return rhs.score < lhs.score;});
        stone.rotate(best.rotate);
        if(stone.get_side() != best.flip) stone.flip();
        field.put_stone_force(stone,best.point.y,best.point.x);
    }

    for(std::size_t stone_num = 1; stone_num <= stones.size() / 2; ++stone_num)
    {
        stone_type& stone = stones.at(stone_num);
        search_type next = std::move(search(field,stone));
        if(next.point.y == FIELD_SIZE) continue;//どこにも置けなかった
        if(pass(next,stone) == true) continue;
        if(next.flip != stone.get_side()) stone.flip();
        stone.rotate(next.rotate);
        problem.field.put_stone(stone,next.point.y,next.point.x);
    }

}

void yrange3::one_try(problem_type& problem, int y, int x, std::size_t const rotate)
{
    int stone_num = problem.stones.size()-1;
    problem.stones.at(stone_num).rotate(rotate / 2  * 90);
    if(rotate %2 == 1) problem.stones.at(stone_num).flip();

    if(problem.field.is_puttable_force(problem.stones.at(stone_num),y,x) == true)
    {
        //1個目
        problem.field.put_stone(problem.stones.front(),y,x);
        stone_num--;

        //２個目以降
        for(std::size_t ishi = 1; ishi < problem.stones.size(); ++ishi)
        {
            stone_type& each_stone = problem.stones.at(ishi);
            search_type next = std::move(search(problem.field,each_stone));
            if(next.point.y == FIELD_SIZE) continue;//どこにも置けなかった
            if(pass(next,each_stone) == true) continue;
            if(next.flip != each_stone.get_side()) each_stone.flip();
            each_stone.rotate(next.rotate);
            problem.field.put_stone(each_stone,next.point.y,next.point.x);
        }
        /*
        std::string const flip = problem.stones.front().get_side() == stone_type::Sides::Head ? "Head" : "Tail";
        qDebug("emit starting by %2d,%2d %2lu %s score = %3zu",y,x,rotate / 2 * 90,flip.c_str(),problem.field.get_score());
        emit answer_ready(problem.field);
        */
    }
}

//評価関数
int yrange3::evaluate(field_type const& field, stone_type stone,int const i, int const j)const
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
yrange3::search_type yrange3::search(field_type& _field, stone_type& stone)
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
            search_vec.push_back({point_type{i,j},stone.get_angle(),stone.get_side(),evaluate(field,stone,i,j),get_island(field.get_raw_data(),point_type{i,j})});
        }
    }
    if(search_vec.size() == 0) return search_type{point_type{FIELD_SIZE,FIELD_SIZE},0,stone_type::Sides::Head,0,0};
    std::sort(search_vec.begin(),search_vec.end(),
                [](const search_type& lhs, const search_type& rhs)
                {
                    return lhs.score == rhs.score ? lhs.island < rhs.island : lhs.score > rhs.score;
                });
    return std::move(search_vec.at(0));
}

int yrange3::get_island(field_type::raw_field_type field, point_type const& point)
{
    int num = -1;
    int const y_min = (point.y < 1) ? 0 : point.y - 1;
    int const y_max = (point.y + STONE_SIZE + 1) < FIELD_SIZE ? point.y + STONE_SIZE + 1 : FIELD_SIZE;
    int const x_min = (point.x < 1) ? 0 : point.x - 1;
    int const x_max = (point.x + STONE_SIZE + 1) < FIELD_SIZE ? point.x + STONE_SIZE + 1 : FIELD_SIZE;

    std::vector<int> result (32,0);
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

bool yrange3::pass(search_type const& search, stone_type const& stone)
{
    if((static_cast<double>(search.score) / static_cast<double>(stone.get_side_length())) < 0.5) return true;
    else return false;
}

std::vector<yrange3::connect_type> yrange3::conect_stone(stone_type& a, stone_type& b)
{
    std::vector<connect_type> connectv;
    field_type _field;
    _field.put_stone(a,8,8);
    for(int i = 0; i < 24; ++i) for(int j = 0; j < 24; ++j)
    {
        if(_field.is_puttable(b,i,j) == true)
        {
            field_type field;
            field.put_stone(b,i,j);
            int const score = evaluate(field,b,i,j);
            if((a.get_side_length() + b.get_side_length()) / 6 < score)
            {
                connectv.emplace_back(point_type{i-8,j-8},score,get_island(field.get_raw_data(),point_type{i,j}));
            }
        }
    }
    std::sort(connectv.begin(),connectv.end(),[](connect_type const& lhs, connect_type const& rhs){return lhs.island < rhs.island;});
    return std::move(connectv);
}

yrange3::search_type yrange3::ur_search(field_type const& field,stone_type const& stone)
{
    std::vector<search_type> sv;
    for(int i = 31; i >= 0; --i) for(int x = i; x < 32; ++x)
    {
        const int y = x - i;
        for(int rotate = 0; rotate < 8; ++rotate)
        {
            if(field.is_puttable_force(stone,y,x) == true)
            {
                sv.push_back(search_type{
                                 point_type{y,x},
                                 stone.get_angle(),
                                 stone.get_side(),
                                 nonput_evaluate(field,stone,y,x),
                                 0});
            }
        }
    }
    return std::move(*max_element(sv.begin(),sv.end(),
        [](search_type const& rhs, search_type const& lhs){return rhs.score < lhs.score;}));
}

yrange3::search_type yrange3::ul_search(field_type const& field,stone_type const& stone)
{
    std::vector<search_type> sv;
    for(int i = 0; i < 32; ++i) for(int x = 0; x <= i; ++x)
    {
        const int y = i - x;
        for(int rotate = 0; rotate < 8; ++rotate)
        {
            if(field.is_puttable_force(stone,y,x) == true)
            {
                sv.push_back(search_type{
                                 point_type{y,x},
                                 stone.get_angle(),
                                 stone.get_side(),
                                 nonput_evaluate(field,stone,y,x),
                                 0});
            }
        }
    }
    return std::move(*max_element(sv.begin(),sv.end(),
        [](search_type const& rhs, search_type const& lhs){return rhs.score < lhs.score;}));
}

yrange3::search_type yrange3::dr_search(field_type const& field,stone_type const& stone)
{
    std::vector<search_type> sv;
    for(int i = 0; i < 31; ++i) for(int x = i + 1; x < 32; ++x)
    {
        const int y = 32 + i - x;
        for(int rotate = 0; rotate < 8; ++rotate)
        {
            if(field.is_puttable_force(stone,y,x) == true)
            {
                sv.push_back(search_type{
                                 point_type{y,x},
                                 stone.get_angle(),
                                 stone.get_side(),
                                 nonput_evaluate(field,stone,y,x),
                                 0});
            }
        }
    }
    return std::move(*max_element(sv.begin(),sv.end(),
        [](search_type const& rhs, search_type const& lhs){return rhs.score < lhs.score;}));
}

yrange3::search_type yrange3::dl_search(field_type const& field,stone_type const& stone)
{
    std::vector<search_type> sv;
    for(int i = 31; i >= 0; --i) for(int x = 0; x < 32 - i; ++x)
    {
        const int y = x + i;
        for(int rotate = 0; rotate < 8; ++rotate)
        {
            if(field.is_puttable_force(stone,y,x) == true)
            {
                sv.push_back(search_type{
                                 point_type{y,x},
                                 stone.get_angle(),
                                 stone.get_side(),
                                 nonput_evaluate(field,stone,y,x),
                                 0});
            }
        }
    }
    return std::move(*max_element(sv.begin(),sv.end(),
        [](search_type const& rhs, search_type const& lhs){return rhs.score < lhs.score;}));
}

int yrange3::nonput_evaluate(field_type const& field, stone_type const& stone, int y, int x) const
{
    int const n = stone.get_nth();
    int score = 0;
    const int init_y = (y < 2) ? 0 : y - 1;
    const int init_x = (x < 2) ? 0 : x - 1;
    for(int i = init_y; i < y + STONE_SIZE && y + 1 < FIELD_SIZE; ++i) for(int j = init_x; j < x + STONE_SIZE && x + 1 < FIELD_SIZE; ++j)
    {
        const int ij  = (field.get_raw_data().at(i).at(j) != 0 && stone.get_raw_data().at(i - init_y).at(j - init_x) < n) ? 1 : 0;
        const int ij1 = (field.get_raw_data().at(i).at(j+1) != 0 && stone.get_raw_data().at(i - init_y).at(j+1) - init_x < n) ? 1 : 0;
        const int i1j = (field.get_raw_data().at(i+1).at(j) != 0 && stone.get_raw_data().at(i+1 - init_y).at(j - init_x) < n) ? 1 : 0;

        if(stone.get_raw_data().at(i - init_y).at(j - init_x) == n)
        {
            score += (ij1 + i1j);
            if(i == 0 || i == FIELD_SIZE - 1) score++;
            if(j == 0 || j == FIELD_SIZE - 1) score++;
        }
        if(stone.get_raw_data().at(i - init_y).at(j + 1 - init_x) == n) score += ij;
        if(stone.get_raw_data().at(i + 1 - init_y).at(j - init_x) == n) score += ij;
    }
    return score;
}

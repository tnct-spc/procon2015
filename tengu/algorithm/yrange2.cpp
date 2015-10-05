#include "yrange2.hpp"
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

yrange2::yrange2(problem_type _problem)
{
    algorithm_name = "yrange2";
    pre_problem = _problem;
}

yrange2::~yrange2()
{
}

void yrange2::run()
{
    qDebug("yrange2 start");

    std::vector<bool> stone_list(pre_problem.stones.size(),false);

    int rdy = 0, rdx = 0, dry = 0, drx = 0;
    std::size_t stone_num = 0;
    while(stone_num < pre_problem.stones.size())
    {
        stone_type& stone = pre_problem.stones.at(stone_num++);
        while(rdx < 32 && rdy < 32 && pre_problem.field.get_raw_data().at(rdy).at(rdx) != 0)
        {
            if(rdx == 31) rdy++;
            else rdx++;
        }
        if(rdy < 32 && pre_problem.field.get_raw_data().at(rdy).at(rdx) == 0 && local_put(pre_problem.field,stone,rdy,rdx) == true)
        {
            if(rdx == 31) rdy++;
            else rdx++;
            stone_list[stone_num-1] = true;
            stone_num++;
            continue;
        }

        while(drx < 32 && dry < 32 && pre_problem.field.get_raw_data().at(dry).at(drx) != 0)
        {
            if(dry == 31) drx++;
            else dry++;
        }
        if(drx < 32 && pre_problem.field.get_raw_data().at(dry).at(drx) == 0 && local_put(pre_problem.field,stone,dry,drx) == true)
        {
            if(dry == 31) drx++;
            else dry++;
            stone_list[stone_num-1] = true;
            stone_num++;
            continue;
        }
    }

    for(std::size_t i = 0; i < stone_list.size(); ++i) if(stone_list[i] == true) std::cout << i << std::endl;

    print_text((boost::format("done score = %d")%pre_problem.field.get_score()).str());
    pre_problem.field.print_field();
    std::cout << std::endl;
    answer_send(pre_problem.field);

    bool next = true;
    while(next == true)
    {
        for(std::size_t i = 0; i < pre_problem.stones.size(); ++i)
        {
            next = false;
            if(stone_list[i] == false && search(pre_problem.field,pre_problem.stones[i]) == true)
            {
                stone_list[i] = true;
                next = true;
                break;
            }
        }
        //pre_problem.field.print_field();
        //std::cout << std::endl;
    }
    print_text((boost::format("done score = %d")%pre_problem.field.get_score()).str());
    pre_problem.field.print_field();
    answer_send(pre_problem.field);
}

//評価関数
int yrange2::evaluate(field_type const& field, stone_type stone,int const i, int const j)const
{
    int const n = stone.get_nth();
    int count = 0;
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

//評価関数
int yrange2::locale_evaluate(field_type const& field, stone_type stone,int const i, int const j)const
{
    int const n = stone.get_nth();
    int count = 0;
    for(int k = (i < 2) ? 0 : i - 1 ;k < i + STONE_SIZE && k + 1 < FIELD_SIZE; ++k) for(int l = (j < 2) ? 0 : j - 1; l < j + STONE_SIZE && l + 1 < FIELD_SIZE; ++l)
    {
        int const kl  = (field.get_raw_data().at(k).at(l) != 0 && field.get_raw_data().at(k).at(l) != n) ? 1 : 0;
        int const kl1 = (field.get_raw_data().at(k).at(l+1) != 0 && field.get_raw_data().at(k).at(l+1) != n) ? 1 : 0;
        int const k1l = (field.get_raw_data().at(k+1).at(l) != 0 && field.get_raw_data().at(k+1).at(l) != n) ? 1 : 0;

        if(field.get_raw_data().at(k).at(l) == n)
        {
            count += (kl1 + k1l);
            if(k == 0 || k == FIELD_SIZE - 1) count += 3;
            if(l == 0 || l == FIELD_SIZE - 1) count += 3;
        }
        if(field.get_raw_data().at(k).at(l+1) == n) count += kl;
        if(field.get_raw_data().at(k+1).at(l) == n) count += kl;
    }
    return count;
}

//おける場所の中から評価値の高いものを選んで返す
bool yrange2::search(field_type& _field, stone_type& stone)
{
    search_type best = {{0,0},0,stone_type::Sides::Head,-1,-1};
    //おける可能性がある場所すべてにおいてみる
    for(int i = 1 - STONE_SIZE; i < FIELD_SIZE; ++i) for(int j = 1 - STONE_SIZE; j < FIELD_SIZE; ++j) for(int rotate = 0; rotate < 8; ++rotate)
    {
        if(rotate % 2 == 0) stone.rotate(90);
        else stone.flip();
        if(_field.is_puttable_basic(stone,i,j) == true)
        {
            _field.put_stone_basic(stone,i,j);
            //置けたら接してる辺を数えて良ければ置き換え
            search_type one = {
                point_type{i,j},
                stone.get_angle(),
                stone.get_side(),
                evaluate(_field,stone,i,j),
                get_island(_field.get_raw_data())
            };
            if(one.score > best.score || (one.score == best.score && one.island < best.island)) best = one;
            _field.remove_stone_basic();
        }
    }
    if(best.score < 0 || pass(best,stone) == false) return false;
    _field.put_stone_basic(stone.set_angle(best.angle).set_side(best.side),best.point.y,best.point.x);
    return true;
}

int yrange2::get_island(field_type::raw_field_type field)
{
    int num = -2;
    std::vector<int> result (FIELD_SIZE * FIELD_SIZE,0);
    std::function<void(int,int,int)> recurision = [&recurision,&field](int y, int x, int num) -> void
    {
        field.at(y).at(x) = num;
        if(0 < y && field.at(y-1).at(x) == 0) recurision(y-1,x,num);
        if(y < FIELD_SIZE - 1 && field.at(y+1).at(x) == 0) recurision(y+1,x,num);
        if(0 < x && field.at(y).at(x-1) == 0) recurision(y,x-1,num);
        if(x < FIELD_SIZE - 1 && field.at(y).at(x+1) == 0) recurision(y,x+1,num);
        return;
    };
    for(int i = 0; i < FIELD_SIZE; ++i) for(int j = 0; j < FIELD_SIZE; ++j)
    {
        if(field.at(i).at(j) == 0) recurision(i,j,num--);
    }
    for(int i = 0; i < FIELD_SIZE; ++i) for(int j = 0; j < FIELD_SIZE; ++j)
    {
        if(field.at(i).at(j) < 0)
        {
            result.at(field.at(i).at(j) * -1)++;
        }
    }
    return -1 * num - 2;
}

bool yrange2::pass(search_type const& search, stone_type const& stone)
{
    if((static_cast<double>(search.score) / static_cast<double>(stone.get_side_length())) < 0.55) return true;
    else return false;
}

bool yrange2::pass(locale_search_type const& search, stone_type const& stone)
{
    if((static_cast<double>(search.score) / static_cast<double>(stone.get_side_length())) < 0.55) return true;
    else return false;
}

bool yrange2::local_put(field_type &field, stone_type &stone, int _y, int _x)
{
    locale_search_type best = locale_search_type{{0,0},0,stone_type::Sides::Head,-1};
    for(int y = _y-7; y <= _y; ++y) for(int x = _x-7; x <= _x; ++x)
    {
        for(int rotate = 0; rotate < 8; ++rotate)
        {
            if(rotate %2 == 0) stone.rotate(90);
            else stone.flip();
            if(field.is_puttable_basic(stone,y,x) == true)
            {
                field.put_stone_basic(stone,y,x);
                if(field.get_raw_data().at(_y).at(_x) == stone.get_nth())
                {
                    int const score =locale_evaluate(field,stone,y,x);
                    if(best.score < score)
                    {
                        best = {
                            {y,x},
                            stone.get_angle(),
                            stone.get_side(),
                            score
                        };
                    }
                }
                //field.remove_large_most_number_and_just_before_stone();//?
                field.remove_stone_basic();
            }
        }
    }
    if(best.score > 0)
    {
        field.put_stone_basic(stone.set_angle(best.rotate).set_side(best.side),best.point.y,best.point.x);
        return true;
    }
    return false;
}

#include "raw_stone.hpp"
#include <QDebug>
#include <algorithm>
#include <functional>

raw_stone::raw_stone(int const algo, int const field_zk)
{
    switch(algo)
    {
    case 1:
        simple_create(field_zk);
        break;
    default:
        break;
    }
}


void raw_stone::create(int zk)
{
    qDebug() << zk;
}

void raw_stone::create()
{
    auto rnd = std::bind(std::uniform_int_distribution<int>(0, 3), std::mt19937());
    int zk;
    zk = rnd();
    create(zk);
}

void raw_stone::simple_create(int const field_zk)
{
    std::random_device seed_gen;
    std::default_random_engine engine(seed_gen());
    std::uniform_int_distribution<> dist_zk(8, 16);
    std::uniform_int_distribution<> dist_need_zk(field_zk * 0.8, field_zk * 1.2);
    std::uniform_int_distribution<> dist_x(0, STONE_SIZE);
    std::uniform_int_distribution<> dist_y(0, STONE_SIZE);
    std::uniform_int_distribution<> dist_move(-1, 1);


    int total_zk = 0;
    int const need_zk = dist_need_zk(engine);

    while(total_zk < need_zk)
    {
        raw_stone_type stone;
        for(auto each_raw : stone) each_raw.fill(0);
        int const zk = dist_zk(engine);
        int x = dist_x(engine);
        int y = dist_y(engine);
        int count = 0;
        while(count < zk)
        {
            if(stone.at(y).at(x) == 0)
            {
                stone.at(y).at(x) = 1;
                count++;
                x += dist_move(engine);
                y += dist_move(engine);
            }
        }
        data.push_back(stone);
        total_zk += zk;
    }
}

int raw_stone::get_fill_zk(raw_stone_type stone)
{
    int sum = 0;
    for(auto each_row : stone)
    {
        sum += std::count(each_row.begin(),each_row.end(),1);
    }
    return sum;
}

#include "raw_stone.hpp"
#include <QDebug>
#include <algorithm>
#include <functional>
#include <iostream>

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
    std::uniform_int_distribution<> dist_x(0, STONE_SIZE - 1);
    std::uniform_int_distribution<> dist_y(0, STONE_SIZE - 1);
    std::uniform_int_distribution<> dist_move(-1, 1);


    int total_zk = 0;
    int const need_zk = dist_need_zk(engine);
    std::cout << "need zk =" << need_zk << std::endl;
    int stone_count = 0;
    while(total_zk < need_zk)
    {
        raw_stone_type stone;
        for(auto &each_raw : stone) each_raw.fill(0);
        int const zk = dist_zk(engine);
        int count = 0;
        int insrance = 0;
        stone.at(dist_x(engine)).at(dist_y(engine)) = 1;

        while(count < zk)
        {
            int const x = dist_x(engine);
            int const y = dist_y(engine);
            if(stone.at(y).at(x) == 1) continue;
            count++;
            if(0 <= y-1 && stone.at(y-1).at(x) == 1)
                stone.at(y).at(x) = 1;
            else if(y+1 < STONE_SIZE - 1 && stone.at(y+1).at(x) == 1)
                stone.at(y).at(x) = 1;
            else if(0 <= x-1 && stone.at(y).at(x-1) == 1)
                stone.at(y).at(x) = 1;
            else if(x+1 < STONE_SIZE - 1 && stone.at(y).at(x+1) == 1)
                stone.at(y).at(x) = 1;
            else count--;
            if(insrance++ > FIELD_SIZE * FIELD_SIZE * FIELD_SIZE) break;
        }
        for(int i = 0; i < STONE_SIZE; ++i) for(int j = 0; j < STONE_SIZE; ++j)
        {
            //穴を潰す処理
        }
        data.push_back(stone);
        total_zk += count;
        print_stone(data.size()-1);
    }
    std::cout << "stone make complete" << std::endl;
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

void raw_stone::print_stone(std::size_t const nth)
{
    if(data.size() <= nth) return;
    for(auto each_row : data.at(nth))
    {
        for(auto const& each_block : each_row)
        {
            std::cout << each_block;
        }
        std::cout << std::endl;
    }
    std::cout << std::endl;
}

#include "raw_stone.hpp"
#include <QDebug>
#include <functional>

raw_stone::raw_stone()
{
    for(int i = 0; i < STONE_SIZE; i++) {
        for(int j = 0; j < STONE_SIZE; j++) {
            data[i][j] = false;
        }
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

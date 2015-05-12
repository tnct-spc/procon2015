#include <QCoreApplication>
#include <QDebug>
#include <array>
#include <random>
#include <functional>
#include <iostream>

class raw_stone {
private:
    int static constexpr STONE_SIZE = 8;
    std::array<std::array<bool, STONE_SIZE>, STONE_SIZE> data;
    enum class direction { UP, RIGHT, DOWN, LEFT };

public:
    raw_stone()
    {
        for(int i = 0; i < STONE_SIZE; i++) {
            for(int j = 0; j < STONE_SIZE; j++) {
                data[i][j] = false;
            }
        }
    }

    void create(int zk)
    {
        qDebug() << zk;
    }
    void create()
    {
        auto rnd = std::bind(std::uniform_int_distribution<int>(0, 3), std::mt19937());
        int zk;
        zk = rnd();
        create(zk);
    }
};


int main(int argc, char *argv[])
{
    //QCoreApplication a(argc, argv);

    raw_stone stone;
    stone.create();

    //return a.exec();
    return 0;
}

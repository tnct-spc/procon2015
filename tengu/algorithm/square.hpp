#ifndef SQUARE_H
#define SQUARE_H

#include <takao.hpp>

class square : public algorithm_type
{
    Q_OBJECT
public:
    square(problem_type _problem);
    ~square();
    void run();

private:
    problem_type original_problem;

    void solve(stone_type stone, int direction);
    void put_stone(stone_type stone,int dy,int dx);
    int count_hole();
    void direction_change(int *dy, int *dx, int direction);
    bool is_hole(int dy, int dx, int size);

    bool field[34][34];
    int min_num;
    int min_x;
    int min_y;
    stone_type min_stone;

};

#endif // SQUARE_H

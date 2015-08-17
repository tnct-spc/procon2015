#include "yrange.hpp"
#include <queue>
#include <algorithm>
#include <QFile>
#include <QIODevice>

yrange::yrange(problem_type _problem)
{
    pre_problem = _problem;
    int const row_obstacle = 0;
    int const col_obstacle = 0;
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
                for(auto& each_stone : problem.stones)
                {
                    //1個目
                    if(each_stone.get_nth() == 1)
                    {
                        problem.field.put_stone(each_stone,l,m);
                        continue;
                    }
                    //２個目以降
                    search_type next = search(problem.field,each_stone,l,m);
                    if(next.point.y == FIELD_SIZE) continue;
                    if(next.flip == 1) each_stone.flip();
                    each_stone.rotate(next.rotate);
                    problem.field.put_stone(each_stone,next.point.y,next.point.x);
                    std::cout << each_stone.get_nth() << std::endl;
                    l = next.point.y;
                    m = next.point.x;
                }
                qDebug("emit starting by %d,%d %d %d",l,m,rotate,flip);
                emit answer_ready(problem.field);
            }
        }
    }
}

/*
void yrange::run()//old run
{
    problem = pre_problem;
    qDebug("yrange start");

    //障害物情報　事前収集
    int const row_obstacle = 0;
    int const col_obstacle = 0;
*/
    //角に適したものを探す
    /*
    std::vector<corner_type> each_corners;
    for(auto & stone : problem.stones) each_corners.push_back(stone.get_corner());
    std::vector<int>::iterator iter = std::max_element(each_corners.begin(), each_corners.end(),[](corner_type const& lhs, corner_type const& rhs){return lhs > rhs;});
    std::size_t const best = std::distance(each_corners.begin(), iter);
    */

    /**/
    //ひとつ目の石を置く　ひとつ目の石は最も角に適しているもの
    /*
    if(problem.field.is_puttable(problem.stones.at(best),FIELD_SIZE - STONE_SIZE - 1 - each_corners.at(best).point.y,-each_corners.at(best).point.x) == true)
    {
        problem.field.put_stone(problem.stones.at(best),FIELD_SIZE - STONE_SIZE - 1 - each_corners.at(best).point.y,-each_corners.at(best).point.x);
    }
    //
    for(std::size_t i = best - 1; i >= 0; --i)
    {
        if(each_zk.at(i) < 3) continue;

    }
    */
    //次の石を今おいた石の隣に置く 次の石はn[zk]以上
    //
/*
    //始めの一つを置く
    for(int m = 1-STONE_SIZE; m < FIELD_SIZE; ++m)for(int n = 1-STONE_SIZE; n  < FIELD_SIZE; ++n)
    {
        //std::cout << "m = " << m << " n = " << n << std::endl;
        if(problem.field.is_puttable(problem.stones.at(0),m,n) == true)
        {
            std::cout << "okeru" << std::endl;
            field_type field = problem.field;
            field.put_stone(problem.stones.at(0),m,n);

            place(field,m,n);
        }
    }
}
*/
//評価関数
double yrange::evaluate(yrange_type const& one)const
{
    int const y = one.point.y;
    int const x = one.point.x;
    double count = 0;
    for(int i  = y; i < y + STONE_SIZE - 1 || FIELD_SIZE - 1; i++) for(int j = x; j < x + STONE_SIZE - 1 || FIELD_SIZE - 1; ++j)
    {
        if(one.field.get_raw_data().at(i).at(j) == one.stone.get_nth() && one.field.get_raw_data().at(i).at(j+1) > 0) count++;
        if(one.field.get_raw_data().at(i).at(j) == one.stone.get_nth() && one.field.get_raw_data().at(i+1).at(j) > 0) count++;
    }
    return count / static_cast<double>(one.stone.get_side_length());
}

//残りを置く
void yrange::place(field_type& field, int const m, int const n)
{
    std::cout << "problem.stones.size() = " << problem.stones.size() << std::endl;
    for(std::size_t stone_num = 1; stone_num < problem.stones.size(); ++stone_num)
    {
        std::vector<yrange_type> first_que;
        std::vector<yrange_type> second_que;
        std::vector<yrange_type> third_que;

        //３手先まで読む
        for(int k = 0; k < 4; ++k)
        {
            problem.stones.at(stone_num).rotate(90);
            for(int l = 0; l < 2; ++l)
            {
                for(int i = 1 - STONE_SIZE; i < FIELD_SIZE; ++i) for(int j = 1 - STONE_SIZE; j < FIELD_SIZE; ++j)
                {
                    std::cout << "k = " << k << " l = " << l << " i = " << i << " j = " << j << std::endl;
                    problem.stones.at(stone_num).flip();
                    if(field.is_puttable(problem.stones.at(stone_num),i,j)== true)
                    {
                        std::cout << "###oketa" << std::endl;
                        yrange_type first = {
                            field.put_stone(problem.stones.at(stone_num),i,j),
                            problem.stones.at(stone_num),
                            {i,j}, 0, k, l, {i,j}
                        };
                        first.value = evaluate(first);
                        first_que.push_back(first);
                    }
                    std::cout << "k = " << k << " l = " << l << " i = " << i << " j = " << j << " end" << std::endl;
                }
            }
        }
        if(first_que.size() == 0)
        {
            std::cout << "return" << std::endl;
            return;
        }
        else if(first_que.size() > 10)
        {
            std::sort(first_que.begin(),first_que.end(),[&](yrange_type lhs, yrange_type rhs){return lhs.value > rhs.value;});
            first_que.resize(10);
        }
        std::cout << "kita2" << std::endl;
        std::cout << "first_que.size() = " << first_que.size() << std::endl;
        for(auto each_1que : first_que)
        {
            std::cout << "kita3" << std::endl;
            for(int k = 0; k < 4; ++k)
            {
                std::cout << "kita4" << std::endl;
                problem.stones.at(stone_num).rotate(90);
                for(int l = 0; l < 2; ++l)
                {
                    std::cout << "kita5" << std::endl;
                    for(int i = 1 - STONE_SIZE; i < FIELD_SIZE; ++i) for(int j = 1 - STONE_SIZE; j < FIELD_SIZE; ++j)
                    {
                        std::cout << "k = " << k << " l = " << l << " i = " << i << " j = " << j << std::endl;
                        problem.stones.at(stone_num).flip();
                        if(field.is_puttable(problem.stones.at(stone_num),i,j) == true)
                        {
                            yrange_type second = each_1que;
                            second.field.put_stone(problem.stones.at(stone_num + 1), i, j);
                            second.stone = problem.stones.at(stone_num + 1);
                            second.point = point_type{i,j};
                            second.value += evaluate(second);
                            second_que.push_back(second);
                        }
                        std::cout << "k = " << k << " l = " << l << " i = " << i << " j = " << j << " end" << std::endl;
                    }

                }
            }
        }
        if(second_que.size() > 10)
        {
            std::sort(second_que.begin(),second_que.end(),[&](yrange_type lhs, yrange_type rhs){return lhs.value > rhs.value;});
            second_que.resize(10);
        }
        for(auto & each_2que : second_que)
        {
            for(int k = 0; k < 4; ++k)
            {
                problem.stones.at(stone_num).rotate(90);
                for(int l = 0; l < 2; ++l)
                {
                    for(int i = 1 - STONE_SIZE; i < FIELD_SIZE; ++i) for(int j = 1 - STONE_SIZE; j < FIELD_SIZE; ++j)
                    {
                        problem.stones.at(stone_num).flip();
                        if(field.is_puttable(problem.stones.at(stone_num),i,j) == true)
                        {
                            yrange_type third = each_2que;
                            third.field.put_stone(problem.stones.at(stone_num + 2), i, j);
                            third.stone = problem.stones.at(stone_num + 2);
                            third.point = point_type{i,j};
                            third.value += evaluate(third);
                            third_que.push_back(third);
                        }
                    }
                }
            }
        }
        //評価値の良い物を選んで置く
        yrange_type best_element = *std::max_element(third_que.begin(),third_que.end(),[&](yrange_type lhs,yrange_type rhs){return lhs.value < rhs.value;});
        problem.stones.at(stone_num).rotate(best_element.angle * 90);
        if(best_element.flip == 0)problem.stones.at(stone_num).flip();
        field.put_stone(problem.stones.at(stone_num),best_element.first_point.y,best_element.first_point.x);
    }

    //解答の送信
    qDebug("emit starting by %d,%d",m,n);
    emit answer_ready(field);

}

//おける場所の中から評価値の高いものを選んで返す
search_type yrange::search(field_type& _field, stone_type const& _stone, int const x, int const y)
{
    std::vector<search_type> search_vec;
    //おける可能性がある場所すべてにおいてみる
    //for(int i = x; i < x + STONE_SIZE + 1; ++i) for(int j = y; j < y + STONE_SIZE + 1; ++j) for(int rotate = 0; rotate < 4; ++rotate) for(int flip = 0; flip < 1; ++flip)
    for(int i = 1 - STONE_SIZE; i < FIELD_SIZE; ++i) for(int j = 1 - STONE_SIZE; j < FIELD_SIZE; ++j) for(int rotate = 0; rotate < 4; ++rotate) for(int flip = 0; flip < 1; ++flip)
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
            int count = 0;
            int k = i > 1 - STONE_SIZE ? i - 1 : i;
            int l = j > 1 - STONE_SIZE ? j - 1 : j;
            for(k = k < 0 ? 0 : k; k < (i + STONE_SIZE) && (k + 1 < FIELD_SIZE); ++k) for(l = l < 0 ? 0 : l ; (l < j + STONE_SIZE) && (l + 1 < FIELD_SIZE); ++l)
            {
                int const n = stone.get_nth();
                if(field.get_raw_data().at(k).at(l) == n && field.get_raw_data().at(k).at(l+1) < n) count++;
                if(field.get_raw_data().at(k).at(l+1) == n && field.get_raw_data().at(k).at(l) < n) count++;
                if(field.get_raw_data().at(k+1).at(l) == n && field.get_raw_data().at(k).at(l) < n) count++;
                if(field.get_raw_data().at(k).at(l) == n && field.get_raw_data().at(k+1).at(l) < n) count++;
            }
            search_vec.push_back(search_type{point_type{i,j},rotate*90,flip,count});
        }
    }
    if(search_vec.size() == 0) return search_type{point_type{FIELD_SIZE,FIELD_SIZE},0,0,0};
    std::sort(search_vec.begin(),search_vec.end(),[](const search_type& lhs, const search_type& rhs) {return lhs.score < rhs.score;});
    return search_vec.at(0);
}

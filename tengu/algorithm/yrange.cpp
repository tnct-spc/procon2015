#include "yrange.hpp"
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

    std::size_t best_score = FIELD_SIZE * FIELD_SIZE;
    QVector<std::tuple<problem_type,int,int,std::size_t>> data;
    data.reserve((FIELD_SIZE+STONE_SIZE)*(FIELD_SIZE+STONE_SIZE)*8);
    for(int l = 1-STONE_SIZE; l < FIELD_SIZE; ++l) for(int m = 1-STONE_SIZE; m  < FIELD_SIZE; ++m)
    {
        for(std::size_t rotate = 0; rotate < 8; ++rotate)
        {
            data.push_back(std::make_tuple(pre_problem,l,m,rotate));
        }

        QFuture<void> threads = QtConcurrent::map(
            data,
            [this](auto& tup)
            {
                this->one_try(std::get<0>(tup), std::get<1>(tup), std::get<2>(tup), std::get<3>(tup));
            });
        threads.waitForFinished();
        for(auto& each_data : data)
        {
            auto& problem = std::get<0>(each_data);
            if(problem.field.list_of_stones().size() == 0) continue;
            std::string const flip = problem.stones.front().get_side() == stone_type::Sides::Head ? "Head" : "Tail";
            qDebug("find starting by %2d,%2d %2lu %s score = %3zu",
                   std::get<1>(each_data),std::get<2>(each_data),std::get<3>(each_data) / 2 * 90,flip.c_str(),
                   problem.field.get_score());
            if(best_score > problem.field.get_score())
            {
                qDebug("emit!");
                emit answer_ready(problem.field);
                best_score = problem.field.get_score();
                print_text((boost::format("score = %d")%problem.field.get_score()).str());
            }
        }

    }
    /*
    for(int l = 1-STONE_SIZE; l < FIELD_SIZE; ++l) for(int m = 1-STONE_SIZE; m  < FIELD_SIZE; ++m) for(std::size_t rotate = 0; rotate < 8; ++rotate)
    {
        one_try(pre_problem, l, m, rotate);
    }
    */
}

void yrange::one_try(problem_type& problem, int y, int x, std::size_t const rotate)
{
    problem.stones.at(0).rotate(rotate / 2  * 90);
    if(rotate %2 == 1) problem.stones.at(0).flip();

    if(problem.field.is_puttable(problem.stones.front(),y,x) == true)
    {
        //1個目
        problem.field.put_stone(problem.stones.front(),y,x);

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
    }
    return count;
}

//おける場所の中から評価値の高いものを選んで返す
yrange::search_type yrange::search(field_type& _field, stone_type& stone)
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
            search_vec.push_back(search_type{point_type{i,j},stone.get_angle(),stone.get_side(),evaluate(field,stone,i,j),get_island(field.get_raw_data(),point_type{i,j})});
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

int yrange::get_island(field_type::raw_field_type field, point_type const& point)
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

bool yrange::pass(search_type const& search, stone_type const& stone)
{
    if((static_cast<double>(search.score) / static_cast<double>(stone.get_side_length())) < 0.7) return true;
    else return false;
}

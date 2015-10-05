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

yrange::yrange(problem_type _problem, int time_limit):time_limit(time_limit)
{
    algorithm_name = "yrange";
    origin_problem = _problem;
}

yrange::~yrange()
{
}

void yrange::run()
{
    //制限時間まで解く→完成次第送信
    print_text("解答開始");
    solve();
    //最良だった解答のテコ入れ→完成次第送信
    print_text("テコ入れ開始");
    improve();
    //exit
    print_text("終了");
}

void yrange::solve()
{
    limit_timer.start();
    qDebug("yrange start");
/*
    QVector<std::tuple<problem_type,int,int,std::size_t>> data;
    data.reserve((FIELD_SIZE+STONE_SIZE)*(FIELD_SIZE+STONE_SIZE)*8);
    for(int l = 1-STONE_SIZE; l < FIELD_SIZE; ++l) for(int m = 1-STONE_SIZE; m  < FIELD_SIZE; ++m)
    {
        for(std::size_t rotate = 0; rotate < 8; ++rotate)
        {
            data.push_back(std::make_tuple(origin_problem,l,m,rotate));
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
            if(problem.field.get_score() < best_score)
            {
                answer_send(problem.field);
                print_text((boost::format("score = %d")%problem.field.get_score()).str());
                best_score = problem.field.get_score();
            }
        }
    }
*/


    std::array<int,39> start_y{{24,-7,29,13,-1,9,30,19,15,-4,18,17,20,-2,23,10,4,-6,7,-5,31,3,8,21,5,16,-3,0,11,22,14,27,2,26,28,1,12,6,25}};
    std::array<int,39> start_x{{5,19,13,26,16,20,8,10,3,23,2,27,0,29,6,-5,-7,11,-4,7,22,-3,1,-6,-2,24,-1,30,31,9,14,25,17,28,21,15,18,12,4}};
    int best_score = FIELD_SIZE * FIELD_SIZE;

    for(std::size_t stone_num = 0; stone_num < origin_problem.stones.size(); ++stone_num)
    {
        for(std::size_t i = 0; i < start_x.size(); ++i) for(std::size_t j = 0; j < start_x.size(); ++j)
        {
            int y = start_y[j];
            int x = start_x[(j+i < start_x.size()) ? j+i : j+i-start_x.size()];
            for(std::size_t angle = 0; angle < 360; angle += 90) for(int side = 0; side < 2; ++side)
            {
                origin_problem.stones[stone_num].set_angle(angle).set_side(static_cast<stone_type::Sides>(side));
                if(origin_problem.field.is_puttable_basic(origin_problem.stones[stone_num],y,x) == true)
                {
                    //Start Solve
                    problem_type problem = origin_problem;
                    problem.field.put_stone_basic(origin_problem.stones[stone_num],y,x);
                    one_try(problem, stone_num);
                    //Send
                    answer_send(problem.field);

                    //----------------------------------------------------------------------------------------------------------
                    int t = static_cast<int>(limit_timer.elapsed());
                    if(t > time_limit){
                        qDebug("time limit!");
                        return;
                    }
                    static int emit_count=0;
                    emit_count++;
                    int const score = problem.field.get_score();
                    std::string const flip = side == 0 ? "Head" : "Tail";
                    qDebug("emit starting by stone=%3lu x=%2d, y=%2d angle=%3lu %s score = %3d time = %d emit_count= %d",stone_num, y,x,angle,flip.c_str(), score,t,emit_count);
                    if(best_score > score)
                    {
                        print_text((boost::format("score = %d")%problem.field.get_score()).str());
                        best_score = score;
                        //Save processes
                        std::copy(problem.field.get_processes().begin(),problem.field.get_processes().end(),std::back_inserter(best_processes));
                    }
                    //-----------------------------------------------------------------------------------------------------------
                }
            }
        }
    }
}

void yrange::one_try(problem_type& problem, std::size_t stone_num)
{
    //２個目以降
    for(stone_num++; stone_num < problem.stones.size(); ++stone_num)
    {
        stone_type& each_stone = problem.stones.at(stone_num);
        search_type next = std::move(search(problem.field,each_stone));
        if(next.point.y == FIELD_SIZE) continue;//どこにも置けなかった
        if(pass(next,each_stone) == true) continue;
        each_stone.set_angle(next.angle).set_side(next.side);
        problem.field.put_stone_basic(each_stone,next.point.y,next.point.x);
    }
}

//評価関数
int yrange::evaluate(field_type const& field, stone_type stone,int const i, int const j)const
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

//おける場所の中から評価値の高いものを選んで返す
yrange::search_type yrange::search(field_type& _field, stone_type& stone)
{
    search_type best = {{FIELD_SIZE,FIELD_SIZE},0,stone_type::Sides::Head,-1,-2};
    //おける可能性がある場所すべてにおいてみる
    for(int i = 1 - STONE_SIZE; i < FIELD_SIZE; ++i) for(int j = 1 - STONE_SIZE; j < FIELD_SIZE; ++j) for(std::size_t angle = 0; angle < 360; angle += 90) for(int side = 0; side < 2; ++side)
    {
        stone.set_angle(angle).set_side(static_cast<stone_type::Sides>(side));
        if(_field.is_puttable_basic(stone,i,j) == true)
        {
            _field.put_stone_basic(stone,i,j);
            //置けたら接してる辺を数えて良ければ置き換え
            int const score = evaluate(_field,stone,i,j);
            int const island = get_island(_field.get_raw_data());
            if(best.score < score || (best.score == score && best.island > island))
            {
                best = {point_type{i,j}, angle, static_cast<stone_type::Sides>(side), score, island};
            }
            _field.remove_stone_basic();
        }
    }
    return best;
}

int yrange::get_island(field_type::raw_field_type field)
{
    int num = -2;
    std::function<void(int,int)> recurision = [&num,&recurision,&field](int y, int x) -> void
    {
        field[y][x] = num;
        if(0 < y && field[y-1][x] == 0) recurision(y-1,x);
        if(y < FIELD_SIZE - 1 && field[y+1][x] == 0) recurision(y+1,x);
        if(0 < x && field[y][x-1] == 0) recurision(y,x-1);
        if(x < FIELD_SIZE - 1 && field[y][x+1] == 0) recurision(y,x+1);
        return;
    };
    for(int i = 0; i < FIELD_SIZE; ++i) for(int j = 0; j < FIELD_SIZE; ++j)
    {
        if(field[i][j] == 0){
            num--;
            recurision(i,j);
        }
    }
    return -1 * num - 2;
 /*
    int label = -2;
    int count1, count2,count3 = 0;

    //次の空白を見つけ、そこを--labelにして帰る
    auto next_find = [&label, &field]()
    {
        for(int i = 0; i < FIELD_SIZE; ++i) for(int j = 0; j < FIELD_SIZE; ++j)
        {
            if(field[i][j] == 0)
            {
                field[i][j] = --label;
                return;
            }
        }
        return;
    };

    //ただのラベリング　再帰だと遅いらし
    while(label < count3)
    {
        count3 = label;
        next_find();
        count1 = 0;
        count2 = -1;
        while(count1 > count2)
        {
            count2 = count1;
            for(int i = 0; i < FIELD_SIZE - 1; ++i) for(int j = 0; j < FIELD_SIZE - 1; ++j)
            {
                if(field[i][j] == label)
                {
                    if(field[i][j+1] == 0)
                    {
                        field[i][j+1] = field[i][j];
                        count1++;
                    }
                    if(field[i+1][j] == 0)
                    {
                        field[i+1][j] = field[i][j];
                        count1++;
                    }
                }
            }
            for(int i = FIELD_SIZE - 1; i > 0; --i) for(int j = FIELD_SIZE - 1; j > 0; --j)
            {
                if(field[i][j] == label)
                {
                    if(field[i][j-1] == 0)
                    {
                        field[i][j-1] = field[i][j];
                        count1++;
                    }
                    if(field[i-1][j] == 0)
                    {
                        field[i-1][j] = field[i][j];
                        count1++;
                    }
                }
            }
        }
     }
     return -1 * label -2;
*/
}

bool yrange::pass(search_type const& search, stone_type const& stone)
{
    if((static_cast<double>(search.score) / static_cast<double>(stone.get_side_length())) < 0.35) return true;
    else return false;
}

void yrange::improve()
{
    return;
}

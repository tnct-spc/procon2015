#include "yrange_based_yayoi.h"
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

yrange_based_yayoi::yrange_based_yayoi(problem_type _problem, int time_limit, evaluator _eval):time_limit(time_limit),eval(_eval)
{
    algorithm_name = "yrange_based_yayoi";
    origin_problem = _problem;
}

yrange_based_yayoi::~yrange_based_yayoi()
{
}

void yrange_based_yayoi::run()
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

void yrange_based_yayoi::solve()
{
    problem_type base_problem = origin_problem;
    limit_timer.start();
    qDebug("yrange_based_yayoi start");
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

    for(std::size_t stone_num = 0; stone_num < base_problem.stones.size(); ++stone_num)
    {
        for(std::size_t i = 0; i < start_x.size(); ++i) for(std::size_t j = 0; j < start_x.size(); ++j)
        {
            int y = start_y[j];
            int x = start_x[(j+i < start_x.size()) ? j+i : j+i-start_x.size()];
            for(std::size_t angle = 0; angle < 360; angle += 90) for(int side = 0; side < 2; ++side)
            {
                base_problem.stones[stone_num].set_angle(angle).set_side(static_cast<stone_type::Sides>(side));
                if(base_problem.field.is_puttable_basic(base_problem.stones[stone_num],y,x) == true)
                {
                    //Start Solve
                    problem_type problem = base_problem;
                    problem.field.put_stone_basic(base_problem.stones[stone_num],y,x);
                    one_try(problem, stone_num);
                    //Send
                    answer_send(problem.field);

                    //----------------------------------------------------------------------------------------------------------
                    int t = static_cast<int>(limit_timer.elapsed());
                    if(t > time_limit - 30000){//30秒前
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
                        best_processes.clear();
                        std::copy(problem.field.processes.begin(),problem.field.processes.end(),std::back_inserter(best_processes));
                        //Save field
                        uint64_t const (&field_bits)[64] = problem.field.get_bit_plain_field();
                        for(int i=0;i<64;i++) best_field[i]=field_bits[i];
                    }
                    //-----------------------------------------------------------------------------------------------------------
                }
            }
        }
    }
}

void yrange_based_yayoi::one_try(problem_type& problem, std::size_t stone_num)
{
    //２個目以降
    for(stone_num++; stone_num < problem.stones.size(); ++stone_num)
    {
        stone_type& each_stone = problem.stones.at(stone_num);
        search_type next = std::move(search(problem.field,each_stone));
        if(next.point.y == FIELD_SIZE) continue;//どこにも置けなかった
        if(pass(next) == true) continue;
        each_stone.set_angle(next.angle).set_side(next.side);
        problem.field.put_stone_basic(each_stone,next.point.y,next.point.x);
    }
}

//おける場所の中から評価値の高いものを選んで返す
yrange_based_yayoi::search_type yrange_based_yayoi::search(field_type& _field, stone_type& stone)
{
    search_type best = {{FIELD_SIZE,FIELD_SIZE},0,stone_type::Sides::Head,-1,-2};
    //おける可能性がある場所すべてにおいてみる
    for(int i = 1 - STONE_SIZE; i < FIELD_SIZE; ++i) for(int j = 1 - STONE_SIZE; j < FIELD_SIZE; ++j) for(std::size_t angle = 0; angle < 360; angle += 90) for(int side = 0; side < 2; ++side)
    {
        stone.set_angle(angle).set_side(static_cast<stone_type::Sides>(side));
        if(_field.is_puttable_basic(stone,i,j) == true)
        {
            double const score = eval.normalized_contact(_field,origin_problem.stones,bit_process_type(stone.get_nth(),angle,side,point_type{i,j}));
            _field.put_stone_basic(stone,i,j);
            //置けたら接してる辺を数えて良ければ置き換え
            //3個以下の石で、露出度が8割を切っriていたらskip(when skip_minimum_stone = true)
            if(stone.get_area()<=3 && stone.get_nth() >= 64 && degree_of_exposure(score)<0.45){
                //qDebug("skip. stone_nth=%d stone_area=%zu",stone.get_nth(),stone.get_area());
            }else{
                double const field_complexity = _field.evaluate_normalized_complexity();
                if(best.score < score || (best.complexity == score && best.complexity > field_complexity))
                {
                    best = {point_type{i,j}, angle, static_cast<stone_type::Sides>(side), score, field_complexity};
                }
            }
            _field.remove_stone_basic(stone);
        }
    }
    return best;
}

//おける場所の中から評価値の高いものを選んで返す
yrange_based_yayoi::search_type yrange_based_yayoi::search_when_second(field_type& _field, stone_type& stone)
{
    //best_fieldと＆をとって、重なってしまわないか検査する
    auto is_puttable = [&](stone_type& stone, int dy, int dx){
        //get_bit_plain_stonesはxが+1されているのでbit_plain_stonesを使う場合は+1し忘れないこと
        stone_type::bit_stones_type const& bit_plain_stones = stone.get_raw_bit_plain_stones();
        int avx_collision = 0;
        __m256i avx_bit_stone = _mm256_loadu_si256((__m256i*)&bit_plain_stones[dx+7+1][static_cast<int>(stone.get_side())][stone.get_angle()/90][0]);
        __m256i avx_bit_field = _mm256_loadu_si256((__m256i*)&best_field[16+dy+0]);
        avx_collision = !_mm256_testz_si256(avx_bit_field,avx_bit_stone);
        avx_bit_stone = _mm256_loadu_si256((__m256i*)&bit_plain_stones[dx+7+1][static_cast<int>(stone.get_side())][stone.get_angle()/90][4]);
        avx_bit_field = _mm256_loadu_si256((__m256i*)&best_field[16+dy+4]);
        avx_collision |= !_mm256_testz_si256(avx_bit_field,avx_bit_stone);
        return !avx_collision;
    };


    search_type best = {{FIELD_SIZE,FIELD_SIZE},0,stone_type::Sides::Head,-1,-2};
    //おける可能性がある場所すべてにおいてみる
    for(int i = 1 - STONE_SIZE; i < FIELD_SIZE; ++i) for(int j = 1 - STONE_SIZE; j < FIELD_SIZE; ++j) for(std::size_t angle = 0; angle < 360; angle += 90) for(int side = 0; side < 2; ++side)
    {
        stone.set_angle(angle).set_side(static_cast<stone_type::Sides>(side));
        if(_field.is_puttable_basic(stone,i,j) && is_puttable(stone,i,j))
        {
            _field.put_stone_basic(stone,i,j);
            //置けたら接してる辺を数えて良ければ置き換え
            double const score = eval.normalized_contact(_field,origin_problem.stones,bit_process_type(stone.get_nth(),angle,side,point_type{i,j}));
            double const field_complexity = _field.evaluate_normalized_complexity();
            if(best.score < score || (best.score == score && best.complexity> field_complexity))
            {
                best = {point_type{i,j}, angle, static_cast<stone_type::Sides>(side), score, field_complexity};
            }
            _field.remove_stone_basic(stone);
        }
    }
    return best;
}

bool yrange_based_yayoi::pass(search_type const& search)
{
    if(degree_of_exposure(search.score) < 0.35) return true;
    else return false;
}

double yrange_based_yayoi::degree_of_exposure(int score)
{
    return score;
}


void yrange_based_yayoi::improve()
{
    problem_type problem = origin_problem;

    auto put_stone = [&](int stone_itr){
        //すでにプロセスにある場所を除く置ける場所を探して、置く

        search_type best_position = std::move(search_when_second(problem.field,problem.stones[stone_itr]));

        if(best_position.score != -1){//置ける場所があったか
            //if(pass(next,each_stone) == true) continue;
            problem.stones[stone_itr].set_angle(best_position.angle).set_side(best_position.side);
            problem.field.put_stone_basic(problem.stones[stone_itr],best_position.point.y,best_position.point.x);
        }
    };

    //仕上げ Start Solve Second
    for(size_t count=0, stone_nth=1; count < best_processes.size(); ++count){
        while(static_cast<size_t>(best_processes[count].nth) != stone_nth){

            put_stone(stone_nth-1);

            stone_nth++;
        }
        problem.field.put_stone_basic(origin_problem.stones.at(best_processes[count].nth-1),best_processes[count].position.y,best_processes[count].position.x);
        stone_nth++;
    }
    for(size_t stone_nth=best_processes[best_processes.size()-1].nth+1; stone_nth <= problem.stones.size(); ++stone_nth){
        put_stone(stone_nth-1);
        stone_nth++;
    }
    //Send
    answer_send(problem.field);

    //----------------------------------------------------------------------------------------------------------
    int t = static_cast<int>(limit_timer.elapsed());
    int const score = problem.field.get_score();
    qDebug("***last*** emit starting by score = %3d time = %d",score,t);
    //-----------------------------------------------------------------------------------------------------------
    return;
}

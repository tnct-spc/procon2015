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

yrange_based_yayoi::yrange_based_yayoi(problem_type _problem, int time_limit):time_limit(time_limit)
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
                        std::copy(problem.field.get_processes().begin(),problem.field.get_processes().end(),std::back_inserter(best_processes));
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
        if(pass(next,each_stone) == true) continue;
        each_stone.set_angle(next.angle).set_side(next.side);
        problem.field.put_stone_basic(each_stone,next.point.y,next.point.x);
    }
}

//評価関数
int yrange_based_yayoi::evaluate(field_type const& field, stone_type stone,int const i, int const j)const
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
yrange_based_yayoi::search_type yrange_based_yayoi::search(field_type& _field, stone_type& stone)
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
            //3個以下の石で、露出度が8割を切っていたらskip
            if(stone.get_area()<=3 && degree_of_exposure(score,stone)<0.8){
                //qDebug("skip. stone_nth=%d stone_area=%zu",stone.get_nth(),stone.get_area());
            }else{
                int const island = get_island(_field.get_raw_data());
                if(best.score < score || (best.score == score && best.island > island))
                {
                    best = {point_type{i,j}, angle, static_cast<stone_type::Sides>(side), score, island};
                }
            }
            _field.remove_stone_basic();
        }
    }
    return best;
}

int yrange_based_yayoi::get_island(field_type::raw_field_type field)
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

bool yrange_based_yayoi::pass(search_type const& search, stone_type const& stone)
{
    if(degree_of_exposure(search.score,stone) < 0.35) return true;
    else return false;
}

double yrange_based_yayoi::degree_of_exposure(int score, stone_type const& stone)
{
    return (static_cast<double>(score) / static_cast<double>(stone.get_side_length()));
}


void yrange_based_yayoi::improve()
{
    //仕上げ Start Solve Second
    problem_type problem = origin_problem;
    for(size_t count=0, stone_nth=1; count < best_processes.size(); ++count){
        while(static_cast<size_t>(best_processes[count].stone.get_nth()) != stone_nth){
            //すでにプロセスにある場所を除く置ける場所を探して、置く

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

            //石を置ける場所を探して置く
            auto put_stone = [&](stone_type& stone){
                //設置可能範囲は左上座標(-7,-7)から左上座標(31,31)まで
                for(int dy=-7;dy<32;dy++){
                    for(int dx=-7;dx<32;dx++){
                        //反転
                        for(int flip = 0; flip < 2; flip ++){
                            //回転
                            for(int angle = 0; angle < 4; angle ++){
                                //フィールドに置けるかチェック
                                if(is_puttable(stone,dy,dx) && problem.field.is_puttable_basic(stone,dy,dx)){
                                    //設置
                                    problem.field.put_stone_basic(stone,dy,dx);
                                    return;
                                }
                                stone.rotate(90);
                            }
                            stone.flip();
                        }
                    }
                }
            };

            put_stone(problem.stones[stone_nth-1]);
            stone_nth++;
        }
        problem.field.put_stone_basic(best_processes[count].stone,best_processes[count].position.y,best_processes[count].position.x);
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

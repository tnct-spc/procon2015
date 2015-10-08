//#define _DEBUGMODE
//#define _DEBUG
#if defined(_DEBUG) || defined(_DEBUGMODE)
#include <QDebug>
#include <iostream>
#endif
#include <iostream>
#include <iomanip>
#include <algorithm>
#include <sstream>
#include "field_type.hpp"
#include "process_type.hpp"
#include "utils.hpp"
#include <immintrin.h>
#include <QDebug>
#include <cstdlib>
#include <random>

bool field_type::is_puttable_basic(const stone_type &stone, int y, int x) const
{
    //まだ置かれていないか確かめる
    #ifdef _DEBUGMODE
    if(is_placed(stone)==true)return false;
    #endif
    //get_bit_plain_stonesはxが+1されているのでbit_plain_stonesを使う場合は+1し忘れないこと
    stone_type::bit_stones_type const& bit_plain_stones = stone.get_raw_bit_plain_stones();

    //石が他の石や障害物と重なっているか調べる
    int avx_collision = 0;
    __m256i avx_bit_stone = _mm256_loadu_si256((__m256i*)&bit_plain_stones[x+7+1][static_cast<int>(stone.get_side())][stone.get_angle()/90][0]);
    __m256i avx_bit_field = _mm256_loadu_si256((__m256i*)&bit_plain_field[16+y+0]);
    avx_collision = !_mm256_testz_si256(avx_bit_field,avx_bit_stone);
    avx_bit_stone = _mm256_loadu_si256((__m256i*)&bit_plain_stones[x+7+1][static_cast<int>(stone.get_side())][stone.get_angle()/90][4]);
    avx_bit_field = _mm256_loadu_si256((__m256i*)&bit_plain_field[16+y+4]);
    avx_collision |= !_mm256_testz_si256(avx_bit_field,avx_bit_stone);
    if(avx_collision) return false;

    //始めの石なら繋がりは必要ない
    if(processes.size() == 0) return true;

    //石が他の自分より若い石と接しているか調べる
    avx_bit_field = _mm256_loadu_si256((__m256i*)&bit_sides_field[16+y+4]);
    avx_collision = !_mm256_testz_si256(avx_bit_field,avx_bit_stone);
    avx_bit_stone = _mm256_loadu_si256((__m256i*)&bit_plain_stones[x+7+1][static_cast<int>(stone.get_side())][stone.get_angle()/90][0]);
    avx_bit_field = _mm256_loadu_si256((__m256i*)&bit_sides_field[16+y+0]);
    avx_collision |= !_mm256_testz_si256(avx_bit_field,avx_bit_stone);
    if(avx_collision==0) return false;

    return true;
}

field_type& field_type::put_stone_basic(const stone_type &stone, int y, int x)
{
#ifdef _DEBUGMODE
    if(is_placed(stone) == true)throw std::runtime_error("The stone is placed!");
    if(is_puttable_basic(stone,y,x) == false) std::runtime_error("The stone can't place!");
#endif
    int stone_nth = stone.get_nth();
    is_placed_stone[stone_nth-1]=true;

    /*ビットフィールドに置く #get_bit_plain_stonesはxが+1されているのでbit_plain_stonesを使う場合は+1し忘れないこと*/

    //フィールドに石を、サイドフィールドに石の辺を置く
    for(int i=0;i<8;i++){
        bit_plain_field[16+y+i] |= (stone).get_bit_plain_stones(x+7,static_cast<int>(stone.get_side()),stone.get_angle()/90,i);//add stone
        bit_plain_field_only_stones[16+y+i] |= (stone).get_bit_plain_stones(x+7,static_cast<int>(stone.get_side()),stone.get_angle()/90,i);//add stone
        bit_sides_field[16+y+i+1] |= (stone).get_bit_plain_stones(x+7,static_cast<int>(stone.get_side()),stone.get_angle()/90,i);//upper
        bit_sides_field[16+y+i-1] |= (stone).get_bit_plain_stones(x+7,static_cast<int>(stone.get_side()),stone.get_angle()/90,i);//under
        bit_sides_field[16+y+i] |= (stone).get_bit_plain_stones(x+7-1,static_cast<int>(stone.get_side()),stone.get_angle()/90,i);//left
        bit_sides_field[16+y+i] |= (stone).get_bit_plain_stones(x+7+1,static_cast<int>(stone.get_side()),stone.get_angle()/90,i);//right
    }
    /*
    if(processes.size() == 1){
        init_route_map();
    }
    */
    processes.emplace_back(stone_nth,
                           static_cast<int>(stone.get_side()),
                           stone.get_angle() / 90,
                           point_type{y, x});
    return *this;
}

// bit_process_type化により引数にremoveする石をとるように変更
field_type& field_type::remove_stone_basic(stone_type const& stone)
{
#ifdef _DEBUGMODE
    if(processes.size() == 0)throw std::runtime_error("Stone is not even placed one!");
#endif
    int processes_end = processes.size()-1;
    int last_stone_nth = processes[processes_end].nth;
#ifdef QT_DEBUG
    if(stone.get_nth() != last_stone_nth)
        throw std::runtime_error("remove_stone_basic: nth does not match");
#endif
    is_placed_stone[last_stone_nth-1]=false;

    //remove from bit field
    //フィールドから石を取り除く
    for(int i=0;i<8;i++){
        bit_plain_field[16+(processes[processes_end].position.y)+i] = ((bit_plain_field[16+(processes[processes_end].position.y)+i]) & (~((stone).get_bit_plain_stones((processes[processes_end].position.x)+7,(int)stone.get_side(),(int)((stone.get_angle())/90),i))));
        //bit_plain_field_only_stones[16+(processes[processes_end].position.y)+i] = ((bit_plain_field_only_stones[16+(processes[processes_end].position.y)+i]) & (~((processes[processes_end].stone).get_bit_plain_stones((processes[processes_end].position.x)+7,(int)processes[processes_end].stone.get_side(),(int)((processes[processes_end].stone.get_angle())/90),i))));
    }
    //サイドフィールドを前の状態に復元する
    for(int i=0;i<64;i++){
        bit_sides_field[i] = 0;
    }
    for(int i=0;i<64;i++){
        if(i!=0) bit_sides_field[i+1] |= bit_plain_field_only_stones[i];//upper
        if(i!=63) bit_sides_field[i-1] |= bit_plain_field_only_stones[i];//under
        bit_sides_field[i] |= bit_plain_field_only_stones[i]<<1;//left
        bit_sides_field[i] |= bit_plain_field_only_stones[i]>>1;//right
    }
    //remove from raw data
//    int stone_position_x = processes[processes_end].position.x;
//    int stone_position_y = processes[processes_end].position.y;
//    int stone_position_back_x = processes[processes_end].position.x + 7;
//    int stone_position_back_y = processes[processes_end].position.y + 7;
//    if(stone_position_x < 0) stone_position_x = 0;
//    if(stone_position_y < 0) stone_position_y = 0;
//    if(stone_position_back_x > 31) stone_position_back_x = 31;
//    if(stone_position_back_y > 31) stone_position_back_y = 31;
//
//    for(int i = stone_position_y; i <= stone_position_back_y; ++i){
//        for(int j = stone_position_x; j <= stone_position_back_x; ++j){
//            if(raw_data.at(i).at(j) == last_stone_nth){
//                raw_data.at(i).at(j) = 0;
//            }
//        }
//    }

    //remove stone from processes
    //processes.erase(processes.end());
    processes.pop_back();
    return *this;
}

//石が置かれているか否かを返す 置かれているときtrue 置かれていないときfalse
bool field_type::is_placed(stone_type const& stone) const
{
    return is_placed_stone[stone.get_nth()-1];
}

//現在の状態における得点を返す
size_t field_type::get_score() const
{
    uint64_t sum = 0;
    for(int i = 16; i < 48; i ++)
    {
        sum += _mm_popcnt_u64(bit_plain_field[i]);
    }
    return FIELD_SIZE * FIELD_SIZE * 2 - sum;
}

field_type::field_type(std::string const & raw_field_text, std::size_t stone_nth)
{
    //has_limit = true;
    provided_stones = stone_nth;
    raw_field_type raw_data;
    auto rows = _split(raw_field_text, "\r\n");
    for (std::size_t i = 0; i < raw_data.size(); ++i) {
        std::transform(rows[i].begin(), rows[i].end(), raw_data[i].begin(),
                       [](auto const & c) { return c == '1' ? -1 : 0; });
    }
    make_bit(raw_data);
    //init_edge();
}

field_type::field_type(const int obstacles, const int cols, const int rows){
    //has_limit = true;
    set_random(obstacles,cols,rows);
    //init_edge();
}

void field_type::print_field()
{
    std::cerr << "gyaaaaaa" << std::endl;
//    for(auto const&each_raw : raw_data)
//    {
//        for(auto const each_block : each_raw)
//        {
//            std::cout << std::setw(3) << each_block;
//        }
//        std::cout << std::endl;
//    }
}

std::string field_type::get_answer()
{
    std::string result;
    unsigned int prev_nth = 0;
    int process_count = 0;
    //石の番号順にソート
    std::sort(processes.begin(), processes.end(),
              [](bit_process_type const &lhs, bit_process_type const &rhs) { return lhs.nth < rhs.nth; });

    for (auto const & process : processes) {
        std::string line;
        auto current_nth = process.nth;

        // スキップ分の改行を挿入する
        for (unsigned int i = prev_nth + 1; i < current_nth; ++i)
            result.append("\r\n");

        if (process_count != 0)
            result.append("\r\n");

        line += std::to_string(process.position.x)
                + " "
                + std::to_string(process.position.y)
                + " "
                + "HT"[process.flip]
                + " "
                + std::to_string(process.rotate * 90);
        result.append(line);
        prev_nth = current_nth;
        process_count++;
    }
    for (std::size_t i = prev_nth;i <= provided_stones; i++)
        result.append("\r\n");

    //正しく動かない環境でのみADDITION_NEW_LINEを環境変数に書いてください.
    if(std::getenv("ADDITION_NEW_LINE") != nullptr)
        result.append("\r\n");

    return result;
}

void field_type::set_random(int const obstacle, int const col, int const row)
{
    raw_field_type raw_data;
    // fill outer zone
    for(int i = 0; i < FIELD_SIZE; i++)
        for(int j = 0; j < FIELD_SIZE; j++)
            raw_data[i][j] = -(col <= j || row <= i);

    int count = 0;
    int insurance = 0;
    std::random_device seed_gen;
    std::default_random_engine engine(seed_gen());
    std::uniform_int_distribution<> dist_x(0, col - 1);
    std::uniform_int_distribution<> dist_y(0, row - 1);

    while (count < obstacle) {
        int x = dist_x(engine);
        int y = dist_y(engine);
        if(raw_data.at(y).at(x) == 0) {
            raw_data.at(y).at(x) = -1;
            count++;
            if(insurance++ > FIELD_SIZE * FIELD_SIZE)
                return;
        }
    }
    make_bit(raw_data);
}

/* 1 new line at the end of the output */
std::string field_type::str()
{
    std::ostringstream ss;
    for(int i = 0; i < 32; i++)
        ss << (std::bitset<32>)(bit_plain_field[i + 16] >> 16 & 0xffffffff) << "\r\n";
    return std::move(ss.str());
}

void field_type::set_provided_stones(size_t ps)
{
    provided_stones = ps;
}

//#BitSystem
//bitデータの作成
void field_type::make_bit(raw_field_type const& raw_data)
{
    //make bit plain field
    //bit_plaint_field_only_obstacleは実際には石をおいていない初期のフィールド
    for(int i=0;i<64;i++){
        bit_plain_field[i] = 0xffffffffffffffff;
        bit_plain_field_only_flame_and_obstacle[i] = 0xffffffffffffffff;
        bit_plain_field_only_stones[i] = 0;
    }
    for(int y=0;y<32;y++){
        for(int x=0;x<32;x++){
            bit_plain_field[y+16] -= (uint64_t)(raw_data.at(y).at(x) + 1) << ((64-17)-x);
            bit_plain_field_only_flame_and_obstacle[y+16] -= (uint64_t)(raw_data.at(y).at(x) + 1) << ((64-17)-x);
        }
    }

    //make bit sides field
    for(int i=0;i<64;i++){
        bit_sides_field[i] = 0;
    }

#ifdef _DEBUG
    std::cout<<"bit plain field"<<std::endl;
    for(int i=0;i<64;i++){
        std::cout<<static_cast<std::bitset<64>>(bit_plain_field[i]);
        std::cout<<std::endl;
    }
    std::cout<<"bit sides field"<<std::endl;
    for(int i=0;i<64;i++){
        std::cout<<static_cast<std::bitset<64>>(bit_sides_field[i]);
        std::cout<<std::endl;
    }
#endif
}

double field_type::evaluate_normalized_complexity() const
{
    uint64_t side = 0;
    for(int i = 1; i < 63; i ++){
        side += _mm_popcnt_u64((bit_plain_field[i] << 1)^(bit_plain_field[i]));
        side += _mm_popcnt_u64((bit_plain_field[i] >> 1)^(bit_plain_field[i]));
        side += _mm_popcnt_u64((bit_plain_field[i+1])^   (bit_plain_field[i]));
        side += _mm_popcnt_u64((bit_plain_field[i-1])^   (bit_plain_field[i]));
    }
    return static_cast<double>(side * side) / static_cast<double>(get_block_num());
}
void field_type::init_route_map(){
    uint64_t bit_slide_field[64];
    std::random_device rnd;
    std::mt19937 mt(rnd());
    std::array<std::array<int, FIELD_SIZE>, FIELD_SIZE> distance_map = {0};
    weighted_route_map = distance_map;//明らかにおかしいけど,初期化のため仕方がない
    for(int i = 0;i < 64;i++){
        //c++が書きたい
        bit_slide_field[i] = bit_plain_field_only_stones[i];
    }
    while(true){
        uint64_t tmp[64] = {0};
        bool match_flag = true;
        for(int y = 0; y < 64; y++){
            if(y > 0)tmp[y] |= bit_slide_field[y-1];
            if(y < 63)tmp[y] |= bit_slide_field[y+1];
            tmp[y] |= bit_slide_field[y] << 1;
            tmp[y] |= bit_slide_field[y] >> 1;
            tmp[y] |= bit_slide_field[y];
            tmp[y] = tmp[y] & (~bit_plain_field_only_flame_and_obstacle[y]);
            if(tmp[y] != bit_slide_field[y])match_flag = false;
        }
        if(match_flag)break;
        for(int y = 0; y < 64; y++){
            bit_slide_field[y] = tmp[y];
        }
        for(int y = 16; y <= 47; y++){
            int x = 0;
            for(u_int64_t mask = 0b0000000000000000100000000000000000000000000000000000000000000000;
                mask != 0b0000000000000000000000000000000000000000000000001000000000000000;
                mask >>= 1,x++){
                distance_map[y -16][x]  += (bit_slide_field[y] & mask ? 1 : 0);
            }
        }
    }
    /*
    for(auto y : distance_map){
        for(auto  x : y){
            std::cout << std::setw(3) << x;
        }
        std::cout << std::endl;
    }
    */
    for(int y = 0; y < 64; y ++){
        for(int x = 0; x < 64; x ++){
            int pos_y = y;
            int pos_x = x;
            while(true){
                int my_distance = distance_map[pos_y][pos_x];
                int up_way = -1, down_way = -1, left_way = -1, right_way = -1;
                if(my_distance == 0)break;
                weighted_route_map[pos_y][pos_x] += 1;
                int ways = 0;
                if(pos_y > 0  && distance_map[pos_y -1][pos_x] == my_distance+1){
                    down_way = ways;
                    ways++;
                }
                if(pos_y < 63 && distance_map[pos_y +1][pos_x] == my_distance+1){
                    up_way = ways;
                    ways++;
                }
                if(pos_x > 0  && distance_map[pos_y][pos_x -1] == my_distance+1){
                    left_way = ways;
                    ways++;
                }
                if(pos_x < 63 && distance_map[pos_y][pos_x +1] == my_distance+1){
                    right_way = ways;
                    ways++;
                }
                if(ways == 0)break;
                std::uniform_int_distribution<int> uni_rand(0,ways-1);
                int rand_way = uni_rand(mt);
                if(down_way == rand_way){
                    pos_y--;
                }else if(up_way == rand_way){
                    pos_y++;
                }else if(left_way == rand_way){
                    pos_x--;
                }else if(right_way == rand_way){
                    pos_x++;
                }else{
                    qDebug() << "いやおかしいよ";
                }
            }
        }
    }
    /*
    for(auto y : weighted_route_map){
        for(auto  x : y){
            std::cout << std::setw(4) << x;
        }
        std::cout << std::endl;
    }
    */
}
double field_type::evaluate_ken_o_expwy() const
{

}


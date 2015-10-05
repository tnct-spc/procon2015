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
#include "utils.hpp"
#include <immintrin.h>
#include <QDebug>
#include <cstdlib>

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
    //#avx_collision = 0; (この行まで来るということは必ずavx_collision=0)
    //#上ですでにbit_plain_stonesの[4]~[7]が読んであるので再利用
    //avx_bit_stone = _mm256_loadu_si256((__m256i*)&bit_plain_stones[x+7+1][static_cast<int>(stone.get_side())][stone.get_angle()/90][4]);
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

    for(int i=0;i<64;i++){
        bit_sides_field_just_before[processes.size()][i] = bit_sides_field[i];
    }

    //フィールドに石を、サイドフィールドに石の辺を置く
    for(int i=0;i<8;i++){
        bit_plain_field[16+y+i] |= (stone).get_bit_plain_stones(x+7,static_cast<int>(stone.get_side()),stone.get_angle()/90,i);//add stone
        bit_sides_field[16+y+i+1] |= (stone).get_bit_plain_stones(x+7,static_cast<int>(stone.get_side()),stone.get_angle()/90,i);//upper
        bit_sides_field[16+y+i-1] |= (stone).get_bit_plain_stones(x+7,static_cast<int>(stone.get_side()),stone.get_angle()/90,i);//under
        bit_sides_field[16+y+i] |= (stone).get_bit_plain_stones(x+7-1,static_cast<int>(stone.get_side()),stone.get_angle()/90,i);//left
        bit_sides_field[16+y+i] |= (stone).get_bit_plain_stones(x+7+1,static_cast<int>(stone.get_side()),stone.get_angle()/90,i);//right
    }
    processes.emplace_back(stone, point_type{y, x});
    return *this;
}

field_type& field_type::remove_stone_basic()
{
#ifdef _DEBUGMODE
    if(processes.size() == 0)throw std::runtime_error("Stone is not even placed one!");
#endif
    int processes_end = processes.size()-1;
    int last_stone_nth = processes[processes_end].stone.get_nth();

    is_placed_stone[last_stone_nth-1]=false;

    //remove from bit field
    //フィールドから石を取り除く
    for(int i=0;i<8;i++){
        bit_plain_field[16+(processes[processes_end].position.y)+i] = ((bit_plain_field[16+(processes[processes_end].position.y)+i]) & (~((processes[processes_end].stone).get_bit_plain_stones((processes[processes_end].position.x)+7,(int)processes[processes_end].stone.get_side(),(int)((processes[processes_end].stone.get_angle())/90),i))));
    }
    //サイドフィールドを前の状態に復元する
    for(int i=0;i<64;i++){
        bit_sides_field[i] = bit_sides_field_just_before[processes_end][i];
    }
    //remove stone from processes
    processes.erase(processes.end());
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



//置かれた石の一覧を表す配列を返す
std::vector<stone_type> field_type::list_of_stones() const
{
    std::vector<stone_type> result;
    std::transform(processes.begin(), processes.end(), std::back_inserter(result),
            [](auto const & process) { return process.stone; });
    return result;
}

field_type::field_type(std::string const & raw_field_text, std::size_t stone_nth)
{
    provided_stones = stone_nth;
    raw_field_type raw_data;
    auto rows = _split(raw_field_text, "\r\n");
    for (std::size_t i = 0; i < raw_data.size(); ++i) {
        std::transform(rows[i].begin(), rows[i].end(), raw_data[i].begin(),
                       [](auto const & c) { return c == '1' ? -1 : 0; });
    }
    make_bit(raw_data);
}
field_type::field_type(const int obstacles, const int cols, const int rows){
    raw_field_type raw_data;
    set_random(raw_data,obstacles,cols,rows);
    make_bit(raw_data);
}

void field_type::print_field()
{
    for(int i=0;i<64;i++){
        std::cout << static_cast<std::bitset<64>>(bit_plain_field[i]) << std::endl;
    }
}

std::string field_type::get_answer()
{
    std::string result;
    int prev_nth = 0;
    int process_count=0;
    //石の番号順にソート
    std::sort(processes.begin(),processes.end(),[](process_type const &lhs,process_type const &rhs){return lhs.stone.get_nth()<rhs.stone.get_nth();});
    for (auto const & process : processes)
    {

        std::string line;

        auto current_nth = process.stone.get_nth();

        // スキップ分の改行を挿入する
        for (int i = prev_nth + 1; i < current_nth; ++i) result.append("\r\n");
        if(process_count!=0) result.append("\r\n");
        line += std::to_string(process.position.x)
                + " "
                + std::to_string(process.position.y)
                + " "
                + (process.stone.get_side() == stone_type::Sides::Head ? "H" : "T")
                + " "
                + std::to_string(process.stone.get_angle());
        result.append(line);
        prev_nth = current_nth;
        process_count++;
    }
    for(std::size_t i = prev_nth;i <= provided_stones; i++)result.append("\r\n");
    //正しく動かない環境でのみADDITION_NEW_LINEを環境変数に書いてください.
    if(std::getenv("ADDITION_NEW_LINE") != nullptr)result.append("\r\n");
    return result;
}
void field_type::set_random(raw_field_type raw_data, int const obstacle, int const col, int const row)
{
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
}

//        sum += std::count(each_row.begin(), each_row.end(),0);
/* 1 new line at the end of the output */
std::string field_type::str()
{
    std::ostringstream ss;
    for(size_t i = 0; i < 64; i++){
        ss << static_cast<std::bitset<64>>(bit_plain_field[i]) << "\r\n";
    }
    return std::move(ss.str());
}

void field_type::set_provided_stones(size_t ps)
{
    provided_stones = ps;
}
//#BitSystem
//bitデータの作成
void field_type::make_bit(const raw_field_type raw_data)
{
    //make bit plain field
    for(int i=0;i<64;i++){
        bit_plain_field[i] = 0xffffffffffffffff;
    }
    for(int y=0;y<32;y++){
        for(int x=0;x<32;x++){
            bit_plain_field[y+16] -= (uint64_t)(raw_data.at(y).at(x) + 1) << ((64-17)-x);
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

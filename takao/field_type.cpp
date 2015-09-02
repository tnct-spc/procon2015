//#define _DEBUGMODE

#if defined(_DEBUG) || defined(_DEBUGMODE)
#include <QDebug>
#include <iostream>
#endif
#include <iostream>
#include <iomanip>
#include <algorithm>
#include "field_type.hpp"
#include "utils.hpp"
#include <immintrin.h>
//石が置かれているか否かを返す 置かれているときtrue 置かれていないときfalse
bool field_type::is_placed(stone_type const& stone)
{
    return std::find_if(processes.begin(), processes.end(),
                        [& stone](auto const & process) { return process.stone.get_nth() == stone.get_nth(); }
           ) != processes.end();
}

//現在の状態における得点を返す
size_t field_type::get_score()
{
    size_t sum = 0;
    for (auto const & row : raw_data)
    {
        sum += std::count(row.begin(), row.end(), 0);
    }
    return sum;
}

//石を置く  自身への参照を返す   失敗したら例外を出す
field_type& field_type::put_stone(stone_type const& stone, int y, int x)
{
#ifdef _DEBUGMODE
    //さきに置けるか確かめる
    if(is_puttable(stone,y,x) == false)throw std::runtime_error("The stone cannot put.");
#endif
    //置く
    //#bit_system
    //get_bit_plain_stonesはxが+1されているのでbit_plain_stonesを使う場合は+1し忘れないこと
    //std::vector<std::vector<std::vector<std::vector<uint64_t>>>> const& bit_plain_stones = stone.get_raw_bit_plain_stones();
    for(int i=0;i<64;i++){
        bit_sides_field_just_before[processes.size()][i] = bit_sides_field[i];
    }
    for(int i=0;i<8;i++){
        //upper
        bit_sides_field[16+y+i+1] |= (stone).get_bit_plain_stones(x+7,(int)stone.get_side(),(int)(stone.get_angle()/90),i);
        //bit_sides_field[16+y+i+1] |= bit_plain_stones[x+7+1][static_cast<int>(stone.get_side())][stone.get_angle()/90][i];
        //under
        bit_sides_field[16+y+i-1] |= (stone).get_bit_plain_stones(x+7,(int)stone.get_side(),(int)(stone.get_angle()/90),i);
        //bit_sides_field[16+y+i-1] |= bit_plain_stones[x+7+1][static_cast<int>(stone.get_side())][stone.get_angle()/90][i];
        //left
        bit_sides_field[16+y+i] |= (stone).get_bit_plain_stones(x+7-1,(int)stone.get_side(),(int)(stone.get_angle()/90),i);
        //bit_sides_field[16+y+i] |= bit_plain_stones[x+7+1-1][static_cast<int>(stone.get_side())][stone.get_angle()/90][i];
        //right
        bit_sides_field[16+y+i] |= (stone).get_bit_plain_stones(x+7+1,(int)stone.get_side(),(int)(stone.get_angle()/90),i);
        //bit_sides_field[16+y+i] |= bit_plain_stones[x+7+1+1][static_cast<int>(stone.get_side())][stone.get_angle()/90][i];
        //add stone
        bit_plain_field[16+y+i] |= (stone).get_bit_plain_stones(x+7,(int)stone.get_side(),(int)(stone.get_angle()/90),i);
        //bit_sides_field[16+y+i] |= bit_plain_stones[x+7+1][static_cast<int>(stone.get_side())][stone.get_angle()/90][i];
    }
    for(int i = 0; i < STONE_SIZE; ++i) for(int j = 0; j < STONE_SIZE; ++j)
    {
        if(stone.at(i,j) == 0)//石がないならどうでもいい
        {
            continue;
        }
        else
        {
            raw_data.at(i+y).at(j+x) = stone.get_nth();
        }
    }
    //processes.emplace_back(stone, point_type{y, x});
    processes.push_back({stone,point_type{y,x}});
    return *this;
}

//指定された場所に指定された石が置けるかどうかを返す
bool field_type::is_puttable(stone_type const& stone, int y, int x)
{
    //vectorのgetterで値をとるよりvector全体を参照で受け取って[]でアクセスしたほうが1.3倍位早い
#ifdef _DEBUGMODE
    if(is_placed(stone)==true) return false;
#endif
    uint64_t collision = 0;
    uint64_t avx_cllis = 0;
    //get_bit_plain_stonesはxが+1されているのでbit_plain_stonesを使う場合は+1し忘れないこと
    stone_type::bit_stones_type const& bit_plain_stones = stone.get_raw_bit_plain_stones();
    //avx
    __m256i avx_bit_stone = _mm256_set_epi64x( bit_plain_stones[x+7+1][static_cast<int>(stone.get_side())][stone.get_angle()/90][3],
                                               bit_plain_stones[x+7+1][static_cast<int>(stone.get_side())][stone.get_angle()/90][2],
                                               bit_plain_stones[x+7+1][static_cast<int>(stone.get_side())][stone.get_angle()/90][1],
                                               bit_plain_stones[x+7+1][static_cast<int>(stone.get_side())][stone.get_angle()/90][0]);

    __m256i avx_bit_field = _mm256_set_epi64x(bit_plain_field[16+y+3],
                                              bit_plain_field[16+y+2],
                                              bit_plain_field[16+y+1],
                                              bit_plain_field[16+y+0]);

    avx_cllis = !_mm256_testz_si256(avx_bit_field,avx_bit_stone);

    avx_bit_stone = _mm256_set_epi64x( bit_plain_stones[x+7+1][static_cast<int>(stone.get_side())][stone.get_angle()/90][7],
                                       bit_plain_stones[x+7+1][static_cast<int>(stone.get_side())][stone.get_angle()/90][6],
                                       bit_plain_stones[x+7+1][static_cast<int>(stone.get_side())][stone.get_angle()/90][5],
                                       bit_plain_stones[x+7+1][static_cast<int>(stone.get_side())][stone.get_angle()/90][4]);

    avx_bit_field = _mm256_set_epi64x(bit_plain_field[16+y+7],
                                      bit_plain_field[16+y+6],
                                      bit_plain_field[16+y+5],
                                      bit_plain_field[16+y+4]);

    avx_cllis |= !_mm256_testz_si256(avx_bit_field,avx_bit_stone);

    /*
    for(int i=0;i<8;i++){
        //collision |= bit_plain_field[16+y+i] & stone.get_bit_plain_stones(x+7,static_cast<int>(stone.get_side()),stone.get_angle()/90,i);
        collision |= bit_plain_field[16+y+i] & bit_plain_stones[x+7+1][static_cast<int>(stone.get_side())][stone.get_angle()/90][i];
    }
    if((!!avx_cllis) != (!!collision)){
        std::cout << "おかしい" << std::endl;
    }
    */
    //if(collision) return false;
    if(avx_cllis) return false;

    if(processes.size() == 0) return true;//始めの石なら繋がりは必要ない

    //collision = 0;
    //この行まで来るということは必ずcollision=0
    avx_bit_field = _mm256_set_epi64x(bit_sides_field[16+y+7],
                                      bit_sides_field[16+y+6],
                                      bit_sides_field[16+y+5],
                                      bit_sides_field[16+y+4]);

    avx_cllis = !_mm256_testz_si256(avx_bit_field,avx_bit_stone);

    avx_bit_stone = _mm256_set_epi64x( bit_plain_stones[x+7+1][static_cast<int>(stone.get_side())][stone.get_angle()/90][3],
                                       bit_plain_stones[x+7+1][static_cast<int>(stone.get_side())][stone.get_angle()/90][2],
                                       bit_plain_stones[x+7+1][static_cast<int>(stone.get_side())][stone.get_angle()/90][1],
                                       bit_plain_stones[x+7+1][static_cast<int>(stone.get_side())][stone.get_angle()/90][0]);

    avx_bit_field = _mm256_set_epi64x(bit_sides_field[16+y+3],
                                      bit_sides_field[16+y+2],
                                      bit_sides_field[16+y+1],
                                      bit_sides_field[16+y+0]);

    avx_cllis = !_mm256_testz_si256(avx_bit_field,avx_bit_stone);

    /*
    for(int i=0;i<8;i++){
        //collision |= bit_sides_field[16+y+i] & stone.get_bit_plain_stones(x+7,static_cast<int>(stone.get_side()),stone.get_angle()/90,i);
        collision |= bit_sides_field[16+y+i] & bit_plain_stones[x+7+1][static_cast<int>(stone.get_side())][stone.get_angle()/90][i];
    }

    if(collision==0) return false;
    */
    if(avx_cllis==0) return false;
    return true;
}

field_type& field_type::remove_just_before_stone(stone_type const& stone)
{
#ifdef _DEBUGMODE
    if(stone.get_nth() != processes[processes.size()-1].stone.get_nth()){
        throw std::runtime_error("The stone isn't just before place in remove_stone!");
        return *this;
    }
    if(is_placed(stone) == false){
        throw std::runtime_error("The stone isn't placed...in remove_stone");
        return *this;
    }
#endif
    //remove
    int processes_at = processes.size()-1;
    for(int i=0;i<8;i++){
        bit_plain_field[16+(processes[processes_at].position.y)+i] = ((bit_plain_field[16+(processes[processes_at].position.y)+i]) & (~((processes[processes_at].stone).get_bit_plain_stones((processes[processes_at].position.x)+7,(int)processes[processes_at].stone.get_side(),(int)((processes[processes_at].stone.get_angle())/90),i))));
    }
    for(int i=0;i<64;i++){
        bit_sides_field[i] = bit_sides_field_just_before[processes.size()-1][i];
    }
    for(int i = 0; i < 32; ++i) for(int j = 0; j < 32; ++j)
    {
        if(raw_data.at(i).at(j) == stone.get_nth()) raw_data.at(i).at(j) = 0;
    }
    processes.erase(processes.end());
    return *this;
}

/*
//指定された石を取り除く．その石が置かれていない場合, 取り除いた場合に不整合が生じる場合は例外を出す
field_type& field_type::remove_stone(stone_type const& stone)
{
    if (is_placed(stone) == false)
    {
        throw std::runtime_error("The stone isn't placed...in remove_stone");
    }
    else if(is_removable(stone) == false)
    {
        throw std::runtime_error("The stone can't remove.");
    }
    for(int i = 0; i < 32; ++i) for(int j = 0; j < 32; ++j)
    {
        if(raw_data.at(i).at(j) == stone.get_nth()) raw_data.at(i).at(j) = 0;
    }
    processes.erase(std::remove_if(processes.begin(), processes.end(),
                                   [& stone](auto const & process) { return process.stone.get_nth() == stone.get_nth(); }),
                    processes.end());
    return *this;
 }

//指定された石を取り除けるかどうかを返す
bool field_type::is_removable(stone_type const& stone)
 {
     std::vector<pair_type> pair_list;
     std::vector<pair_type> remove_list;
     if(processes[processes.size()-1].stone.get_nth()==stone.get_nth()) return true;
     if(is_placed(stone) == false) return false;
     if(processes.size() == 1)return true;
     //継ぎ目を検出
     for(size_t i = 0; i < 32; ++i) for(size_t j = 0; j < 32; ++j)
     {
         int const c = raw_data.at(i).at(j);
         if(i!=31){
             int const d = raw_data.at(i+1).at(j);
             if(c != d) pair_list.push_back(pair_type{c,d});
         }
         if(j!=31){
             int const r = raw_data.at(i).at(j+1);
             if(c != r) pair_list.push_back(pair_type{c,r});
         }
     }
     //取り除きたい石に隣接している石リストを作りながら、取り除きたい石を含む要素を消す
     for(std::vector<pair_type>::iterator it = pair_list.begin();it != pair_list.end();)
     {
         if(it->a == stone.get_nth() || it->b == stone.get_nth())
         {
             remove_list.push_back(*it);
             it = pair_list.erase(it);
         }
         else ++it;
     }
     //取り除きたい石に隣接している石リストに含まれる石それぞれに、不整合が生じていないか見ていく
     bool ans = false;
     for(auto const& each_remove_list : remove_list)
     {
         int const target_stone_num = (each_remove_list.a == stone.get_nth())?each_remove_list.b:each_remove_list.a;
         for(auto const& each_pea_list : pair_list)
         {
             if((each_pea_list.a == target_stone_num && each_pea_list.a > each_pea_list.b && each_pea_list.b > 0) ||
                (each_pea_list.b == target_stone_num && each_pea_list.b > each_pea_list.a && each_pea_list.a > 0))
             {
                 ans = true;
                 break;
             }
             else continue;
         }
         if(ans == false) return false;
         else continue;
     }
     return true;
 }
 */

 //置かれた石の一覧を表す配列を返す
 std::vector<stone_type> field_type::list_of_stones() const
 {
     std::vector<stone_type> result;
     std::transform(processes.begin(), processes.end(), std::back_inserter(result),
                    [](auto const & process) { return process.stone; });
     return result;
 }

//コメント書こう
 placed_stone_type field_type::get_stone(std::size_t const & y, std::size_t const & x)
{
    auto nth = raw_data.at(y).at(x);
    if (nth == 0 || nth == -1) {
        throw std::runtime_error("There is no stone.");
    }

    point_type pf = std::find_if(processes.begin(), processes.end(),
                                 [& nth](auto const & process) { return process.stone.get_nth() == nth; }
                                )->position;
    point_type ps = {static_cast<int>(y) - pf.y, static_cast<int>(x) - pf.x};
    const stone_type * stone;

    for (auto & process : processes) {
        if (process.stone.get_nth() == nth) {
            stone = &process.stone;
            break;
        }
    }
    return placed_stone_type(*stone, pf, ps);
}

 //コメント書こう
field_type::field_type(std::string const & raw_field_text, std::size_t stone_nth)
{
    provided_stones = stone_nth;
    auto rows = _split(raw_field_text, "\r\n");
    for (std::size_t i = 0; i < raw_data.size(); ++i) {
        std::transform(rows[i].begin(), rows[i].end(), raw_data[i].begin(),
                       [](auto const & c) { return c == '1' ? -1 : 0; });
    }

    make_bit();

    //edges
    //upper
    upper_edge = [&]
    {
        for(int i = 0; i < FIELD_SIZE; ++i) for(int j = 0; j < FIELD_SIZE; ++j)
        {
            if(raw_data.at(i).at(j) != -1) return i;
        }
        return FIELD_SIZE;
    }();
    //right
    right_edge = [&]
    {
        for(int i = FIELD_SIZE - 1; i >= 0; --i) for(int j = 0; j < FIELD_SIZE; ++j)
        {
            if(raw_data.at(j).at(i) != -1) return FIELD_SIZE - i - 1;
        }
        return FIELD_SIZE;
    }();
    //buttom
    buttom_edge = [&]
    {
        for(int i = FIELD_SIZE - 1; i >= 0; --i) for(int j = 0; j < FIELD_SIZE; ++j)
        {
            if(raw_data.at(i).at(j) != -1) return FIELD_SIZE - i - 1;
        }
        return FIELD_SIZE;
    }();
    //left
    left_edge = [&]
    {
        for(int i = 0; i < FIELD_SIZE; ++i) for(int j = 0; j < FIELD_SIZE; ++j)
        {
            if(raw_data.at(i).at(j) != -1) return i;
        }
        return FIELD_SIZE;
    }();
}

field_type::raw_field_type const & field_type::get_raw_data() const
{
    return raw_data;
}

void field_type::print_field()
{
    for(auto const&each_raw : raw_data)
    {
        for(auto each_block : each_raw)
        {
            std::cout << std::setw(3) << each_block;
        }
        std::cout << std::endl;
    }
}

std::string field_type::get_answer() const
{
    std::string result;
    int prev_nth = 0;
    int process_count=0;
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
    for(size_t i = prev_nth;i < provided_stones; i++)result.append("\r\n");
    return result;
}

//#BitSystem
//bitデータの作成
void field_type::make_bit()
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

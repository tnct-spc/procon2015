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
#include <random>
void field_type::cancellation_of_restriction()
{
    has_limit = false;
    bit_sides_field_at_stone_nth = new uint64_t*[257];
    for(int i=0;i<257;i++){
        bit_sides_field_at_stone_nth[i] = new uint64_t[64];
    }
    //石の番号ごとのサイドフィールドの更新
    for(int i=0;i<=256;i++){
        for(int j=0;j<64;j++){
            bit_sides_field_at_stone_nth[i][j]=0;
        }
    }
    stone_type p_stone;
    int p_y;
    int p_x;
    int p_nth;
    stone_type::Sides p_side;
    int p_angle90;
    for(size_t i=0;i<processes.size();i++){
        p_stone = processes[i].stone;
        p_side = processes[i].stone.get_side();
        p_angle90 = processes[i].stone.get_angle()/90;
        p_y = processes[i].position.y;
        p_x = processes[i].position.x;
        p_nth = processes[i].stone.get_nth();
        for(int j=p_nth;j<=256;j++){
            //put_side_stone
            for(int k=0;k<8;k++){
                bit_sides_field_at_stone_nth[j][16+p_y+k+1] |= (p_stone).get_bit_plain_stones(p_x+7,(int)p_side,(int)p_angle90,k);
                bit_sides_field_at_stone_nth[j][16+p_y+k-1] |= (p_stone).get_bit_plain_stones(p_x+7,(int)p_side,(int)p_angle90,k);
                bit_sides_field_at_stone_nth[j][16+p_y+k] |= (p_stone).get_bit_plain_stones(p_x+7-1,(int)p_side,(int)p_angle90,k);
                bit_sides_field_at_stone_nth[j][16+p_y+k] |= (p_stone).get_bit_plain_stones(p_x+7+1,(int)p_side,(int)p_angle90,k);
            }
        }
    }
}

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

    //フィールドに石を、サイドフィールドに石の辺を置く
    for(int i=0;i<8;i++){
        bit_plain_field[16+y+i] |= (stone).get_bit_plain_stones(x+7,static_cast<int>(stone.get_side()),stone.get_angle()/90,i);//add stone
        bit_plain_field_only_stones[16+y+i] |= (stone).get_bit_plain_stones(x+7,static_cast<int>(stone.get_side()),stone.get_angle()/90,i);//add stone
        bit_sides_field[16+y+i+1] |= (stone).get_bit_plain_stones(x+7,static_cast<int>(stone.get_side()),stone.get_angle()/90,i);//upper
        bit_sides_field[16+y+i-1] |= (stone).get_bit_plain_stones(x+7,static_cast<int>(stone.get_side()),stone.get_angle()/90,i);//under
        bit_sides_field[16+y+i] |= (stone).get_bit_plain_stones(x+7-1,static_cast<int>(stone.get_side()),stone.get_angle()/90,i);//left
        bit_sides_field[16+y+i] |= (stone).get_bit_plain_stones(x+7+1,static_cast<int>(stone.get_side()),stone.get_angle()/90,i);//right
    }

    //ロウデータに置く
    for(int i = 0; i < STONE_SIZE; ++i) for(int j = 0; j < STONE_SIZE; ++j)
    {
        if(stone.at(i,j) == 0)//石がないならどうでもいい
        {
            continue;
        }
        else
        {
            raw_data.at(i+y).at(j+x) = stone_nth;
        }
    }
    processes.emplace_back(stone, point_type{y, x});
    if(processes.size() == 1){
        init_route_map();
    }
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
        bit_plain_field_only_stones[16+(processes[processes_end].position.y)+i] = ((bit_plain_field_only_stones[16+(processes[processes_end].position.y)+i]) & (~((processes[processes_end].stone).get_bit_plain_stones((processes[processes_end].position.x)+7,(int)processes[processes_end].stone.get_side(),(int)((processes[processes_end].stone.get_angle())/90),i))));
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
    int stone_position_x = processes[processes_end].position.x;
    int stone_position_y = processes[processes_end].position.y;
    int stone_position_back_x = processes[processes_end].position.x + 7;
    int stone_position_back_y = processes[processes_end].position.y + 7;
    if(stone_position_x < 0) stone_position_x = 0;
    if(stone_position_y < 0) stone_position_y = 0;
    if(stone_position_back_x > 31) stone_position_back_x = 31;
    if(stone_position_back_y > 31) stone_position_back_y = 31;

    for(int i = stone_position_y; i <= stone_position_back_y; ++i){
        for(int j = stone_position_x; j <= stone_position_back_x; ++j){
            if(raw_data.at(i).at(j) == last_stone_nth){
                raw_data.at(i).at(j) = 0;
            }
        }
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

//石を置く
field_type& field_type::put_stone(stone_type const& stone, int y, int x)
{
#ifdef _DEBUGMODE
    if(is_placed(stone) == true)throw std::runtime_error("The stone is placed!");
#endif
    int stone_nth = stone.get_nth();
    is_placed_stone[stone_nth-1]=true;
    /*ビットフィールドに置く #get_bit_plain_stonesはxが+1されているのでbit_plain_stonesを使う場合は+1し忘れないこと*/

    //フィールドに石を、サイドフィールドに石の辺を置く
    for(int i=0;i<8;i++){
        //add stone
        bit_plain_field[16+y+i] |= (stone).get_bit_plain_stones(x+7,static_cast<int>(stone.get_side()),stone.get_angle()/90,i);
        //upper
        bit_sides_field[16+y+i+1] |= (stone).get_bit_plain_stones(x+7,static_cast<int>(stone.get_side()),stone.get_angle()/90,i);
        //under
        bit_sides_field[16+y+i-1] |= (stone).get_bit_plain_stones(x+7,static_cast<int>(stone.get_side()),stone.get_angle()/90,i);
        //left
        bit_sides_field[16+y+i] |= (stone).get_bit_plain_stones(x+7-1,static_cast<int>(stone.get_side()),stone.get_angle()/90,i);
        //right
        bit_sides_field[16+y+i] |= (stone).get_bit_plain_stones(x+7+1,static_cast<int>(stone.get_side()),stone.get_angle()/90,i);
    }

    //石の番号ごとのサイドフィールドに石の辺を置く
    uint64_t bit_side_field_temp[8][3];
    for(int i = 0; i < 8; ++i)
    {
        bit_side_field_temp[i][0] = (stone).get_bit_plain_stones(x+7,(int)stone.get_side(),(int)(stone.get_angle()/90),i);//upper under
        bit_side_field_temp[i][1] = (stone).get_bit_plain_stones(x+7-1,(int)stone.get_side(),(int)(stone.get_angle()/90),i);//left
        bit_side_field_temp[i][2] = (stone).get_bit_plain_stones(x+7+1,(int)stone.get_side(),(int)(stone.get_angle()/90),i);//right
    }

    for(int j=stone_nth;j<=256;j++){
        for(int i=0;i<8;i++){
            //upper
            bit_sides_field_at_stone_nth[j][16+y+i+1] |= bit_side_field_temp[i][0];
            //under
            bit_sides_field_at_stone_nth[j][16+y+i-1] |= bit_side_field_temp[i][0];
            //left
            bit_sides_field_at_stone_nth[j][16+y+i] |= bit_side_field_temp[i][1];
            //right
            bit_sides_field_at_stone_nth[j][16+y+i] |= bit_side_field_temp[i][2];
      }
    }

    //ロウデータに置く
    for(int i = 0; i < STONE_SIZE; ++i) for(int j = 0; j < STONE_SIZE; ++j)
    {
        if(stone.at(i,j) == 0)//石がないならどうでもいい
        {
            continue;
        }
        else
        {
            raw_data.at(i+y).at(j+x) = stone_nth;
        }
    }
    processes.emplace_back(stone, point_type{y, x});
    return *this;
}

//接していなくても石を置けるか
bool field_type::is_puttable_force(const stone_type &stone, int y, int x)
{
    //まだ置かれていないか確かめる
    if(is_placed(stone)==true)return false;

    //get_bit_plain_stonesはxが+1されているのでbit_plain_stonesを使う場合は+1し忘れないこと
    //_mm256_testz_si256  256bitのandで1つでも1があれば1,なければ0
    //_mm256_loadu_si256  アラインメントが揃っていないデータを読み込む
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

    return true;
}

//石を置けるか
bool field_type::is_puttable(stone_type const& stone, int y, int x) const
{
    //まだ置かれていないか確かめる
    if(is_placed(stone)==true)return false;

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
    avx_bit_field = _mm256_loadu_si256((__m256i*)&bit_sides_field_at_stone_nth[stone.get_nth()-1][16+y+4]);
    avx_collision = !_mm256_testz_si256(avx_bit_field,avx_bit_stone);
    avx_bit_stone = _mm256_loadu_si256((__m256i*)&bit_plain_stones[x+7+1][static_cast<int>(stone.get_side())][stone.get_angle()/90][0]);
    avx_bit_field = _mm256_loadu_si256((__m256i*)&bit_sides_field_at_stone_nth[stone.get_nth()-1][16+y+0]);
    avx_collision |= !_mm256_testz_si256(avx_bit_field,avx_bit_stone);
    if(avx_collision==0) return false;

    return true;
}

int field_type::processes_min_stone_nth()
{
    int8_t min_nth=INT8_MAX;
    for(unsigned int i=0;i<processes.size();i++){
        if(processes[i].stone.get_nth() < min_nth) min_nth = processes[i].stone.get_nth();
    }
    return min_nth;
}

//石を取り除けるか
bool field_type::is_removable(const stone_type &stone)
{
    std::vector<pair_type> pair_list;
    std::vector<pair_type> remove_list;
    if(processes[processes.size()-1].stone.get_nth()==stone.get_nth()) return true;
    if(is_placed(stone) == false) return false;
    if(processes.size() == 1)return true;
    if(processes_min_stone_nth()==stone.get_nth()) return true;
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
        if((it->a == stone.get_nth() && it->b > stone.get_nth()) || (it->b == stone.get_nth() && it->a > stone.get_nth()))
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

//一番番号の大きくて、一番最後に置いたを取り除く
field_type& field_type::remove_large_most_number_and_just_before_stone()
{
#ifdef _DEBUGMODE
    if(processes.size() == 0)throw std::runtime_error("Stone is not even placed one!");
#endif
    int processes_end = processes.size()-1;
    int most_large_stone_nth = processes[processes_end].stone.get_nth();

    is_placed_stone[most_large_stone_nth-1]=false;

    //remove from bit field
    //フィールドから石を取り除く
    for(int i=0;i<8;i++){
        bit_plain_field[16+(processes[processes_end].position.y)+i] = ((bit_plain_field[16+(processes[processes_end].position.y)+i]) & (~((processes[processes_end].stone).get_bit_plain_stones((processes[processes_end].position.x)+7,(int)processes[processes_end].stone.get_side(),(int)((processes[processes_end].stone.get_angle())/90),i))));
    }
    //remove stone from processes
    processes.erase(processes.end());
    //石の番号ごとのサイドフィールドの更新
    for(int i=most_large_stone_nth;i<=256;i++){
        for(int j=0;j<64;j++) bit_sides_field_at_stone_nth[i][j] = bit_sides_field_at_stone_nth[most_large_stone_nth-1][j];
    }
    //サイドフィールドを前の状態に復元する
    for(int i=0;i<64;i++){
        bit_sides_field[i] = bit_sides_field_at_stone_nth[256][i];
    }
    //remove from raw data
    for(int i = 0; i < 32; ++i) for(int j = 0; j < 32; ++j)
    {
        if(raw_data.at(i).at(j) == most_large_stone_nth) raw_data.at(i).at(j) = 0;
    }
    return *this;
}

//一番番号の大きい石を取り除く
field_type& field_type::remove_large_most_number_stone()
{
#ifdef _DEBUGMODE
    if(processes.size() == 0)throw std::runtime_error("Stone is not even placed one!");
#endif
    //Search large most number
    int most_large_stone_nth = 1;
    int large_stone_at=0;
    for(size_t i=0;i<processes.size();i++){
        if(processes[i].stone.get_nth() > most_large_stone_nth){
            most_large_stone_nth = processes[i].stone.get_nth();
            large_stone_at = i;
        }
    }

    is_placed_stone[most_large_stone_nth-1]=false;

    //remove from bit field
    //フィールドから石を取り除く
    for(int i=0;i<8;i++){
        bit_plain_field[16+(processes[large_stone_at].position.y)+i] = ((bit_plain_field[16+(processes[large_stone_at].position.y)+i]) & (~((processes[large_stone_at].stone).get_bit_plain_stones((processes[large_stone_at].position.x)+7,(int)processes[large_stone_at].stone.get_side(),(int)((processes[large_stone_at].stone.get_angle())/90),i))));
    }
    //remove stone from processes
    processes.erase(processes.begin() + large_stone_at);
    //石の番号ごとのサイドフィールドの更新
    for(int i=most_large_stone_nth;i<=256;i++){
        for(int j=0;j<64;j++) bit_sides_field_at_stone_nth[i][j] = bit_sides_field_at_stone_nth[most_large_stone_nth-1][j];
    }
    //サイドフィールドを前の状態に復元する
    for(int i=0;i<64;i++){
        bit_sides_field[i] = bit_sides_field_at_stone_nth[256][i];
    }
    //remove from raw data
    for(int i = 0; i < 32; ++i) for(int j = 0; j < 32; ++j)
    {
        if(raw_data.at(i).at(j) == most_large_stone_nth) raw_data.at(i).at(j) = 0;
    }
    return *this;
}

bool field_type::is_removable_force(const stone_type &stone)
{
    return is_placed(stone);
}

//石を取り除く
field_type& field_type::remove_stone(stone_type const& stone)
{
#ifdef _DEBUGMODE
    if(is_placed(stone) == false)throw std::runtime_error("The stone isn't placed!");
#endif

    int stone_nth = stone.get_nth();
    is_placed_stone[stone_nth-1]=false;

    //Search stone
    int stone_processes_at=0;
    for(size_t i=0;i<processes.size();i++){
        if(processes[i].stone.get_nth() == stone_nth){
            stone_processes_at = i;
            break;
        }
    }
    //remove from bit field
    //フィールドから石を取り除く
    for(int i=0;i<8;i++){
        bit_plain_field[16+(processes[stone_processes_at].position.y)+i] = ((bit_plain_field[16+(processes[stone_processes_at].position.y)+i]) & (~((processes[stone_processes_at].stone).get_bit_plain_stones((processes[stone_processes_at].position.x)+7,(int)processes[stone_processes_at].stone.get_side(),(int)((processes[stone_processes_at].stone.get_angle())/90),i))));
    }
    //remove stone from processes
    processes.erase(processes.begin() + stone_processes_at);
    //石の番号ごとのサイドフィールドの更新
    for(int i=0;i<=256;i++){
        for(int j=0;j<64;j++){
            bit_sides_field_at_stone_nth[i][j]=0;
        }
    }
    stone_type p_stone;
    int p_y;
    int p_x;
    int p_nth;
    stone_type::Sides p_side;
    int p_angle90;
    for(size_t i=0;i<processes.size();i++){
        p_stone = processes[i].stone;
        p_side = processes[i].stone.get_side();
        p_angle90 = processes[i].stone.get_angle()/90;
        p_y = processes[i].position.y;
        p_x = processes[i].position.x;
        p_nth = processes[i].stone.get_nth();
        for(int j=p_nth;j<=256;j++){
            //put_side_stone
            for(int k=0;k<8;k++){
                bit_sides_field_at_stone_nth[j][16+p_y+k+1] |= (p_stone).get_bit_plain_stones(p_x+7,(int)p_side,(int)p_angle90,k);
                bit_sides_field_at_stone_nth[j][16+p_y+k-1] |= (p_stone).get_bit_plain_stones(p_x+7,(int)p_side,(int)p_angle90,k);
                bit_sides_field_at_stone_nth[j][16+p_y+k] |= (p_stone).get_bit_plain_stones(p_x+7-1,(int)p_side,(int)p_angle90,k);
                bit_sides_field_at_stone_nth[j][16+p_y+k] |= (p_stone).get_bit_plain_stones(p_x+7+1,(int)p_side,(int)p_angle90,k);
            }
        }
    }
    //サイドフィールドを前の状態に復元する
    for(int i=0;i<64;i++){
        bit_sides_field[i] = bit_sides_field_at_stone_nth[256][i];
    }
    //remove from raw data
    for(int i = 0; i < 32; ++i) for(int j = 0; j < 32; ++j)
    {
        if(raw_data.at(i).at(j) == stone_nth) raw_data.at(i).at(j) = 0;
    }

    return *this;
}

//自分より若い石に接していない石を探してすべて返す
std::vector<stone_type> field_type::search_not_in_contact_stones()
{
    std::vector<stone_type> result;
    int is_contact_flag;
    int stone_nth;
    int min_stone_nth = processes_min_stone_nth();

    for(size_t i=0;i<processes.size();i++){
        is_contact_flag=false;
        stone_nth = processes[i].stone.get_nth();
        if(min_stone_nth==stone_nth) continue;//一番若い石
        for(size_t y = 0; y < 32; ++y) for(size_t x = 0; x < 32; ++x)
        {
            if(raw_data.at(y).at(x)==stone_nth){
                if(y!=31) if(raw_data.at(y+1).at(x)<stone_nth) is_contact_flag = true;
                if(y!=0) if(raw_data.at(y-1).at(x)<stone_nth) is_contact_flag = true;
                if(x!=31) if(raw_data.at(y).at(x+1)<stone_nth) is_contact_flag = true;
                if(x!=0) if(raw_data.at(y).at(x-1)<stone_nth) is_contact_flag = true;
            }
        }
        if(is_contact_flag==false){
            //not contact
            //add
            result.push_back(processes[i].stone);
        }
    }
    return result;
}

//すべての石が自分より若い石に接しているか確認する
bool field_type::is_stones_contact()
{
    int is_contact_flag;
    int stone_nth;
    int min_stone_nth = processes_min_stone_nth();

    for(size_t i=0;i<processes.size();i++){
        is_contact_flag=false;
        stone_nth = processes[i].stone.get_nth();
        if(min_stone_nth==stone_nth) continue;//一番若い石
        for(size_t y = 0; y < 32; ++y) for(size_t x = 0; x < 32; ++x)
        {
            if(raw_data.at(y).at(x)==stone_nth){
                if(y!=31) if(raw_data.at(y+1).at(x)>0 && raw_data.at(y+1).at(x)<stone_nth) is_contact_flag = true;
                if(y!=0) if(raw_data.at(y-1).at(x)>0 && raw_data.at(y-1).at(x)<stone_nth) is_contact_flag = true;
                if(x!=31) if(raw_data.at(y).at(x+1)>0 && raw_data.at(y).at(x+1)<stone_nth) is_contact_flag = true;
                if(x!=0) if(raw_data.at(y).at(x-1)>0 && raw_data.at(y).at(x-1)<stone_nth) is_contact_flag = true;
            }
        }
        if(is_contact_flag==false){
            //not contact
            return false;
        }
    }
    return true;
}

//自分より若い石に接することができない(制約違反)石を探してすべて返す
std::vector<stone_type> field_type::search_cannot_be_in_contact_stones()
{
    std::vector<stone_type> result;
    int stone_nth;
    int min_stone_nth = processes_min_stone_nth();
    for(size_t i=0;i<processes.size();i++){
        stone_nth = processes[i].stone.get_nth();
        if(min_stone_nth==stone_nth) continue;
        for(size_t y = 0; y < 32; ++y) for(size_t x = 0; x < 32; ++x)
        {
            if(raw_data.at(y).at(x)==stone_nth){
                if(y!=31) if(raw_data.at(y+1).at(x)==0 || raw_data.at(y+1).at(x)<stone_nth) continue;
                if(y!=0) if(raw_data.at(y-1).at(x)==0 || raw_data.at(y-1).at(x)<stone_nth) continue;
                if(x!=31) if(raw_data.at(y).at(x+1)==0 || raw_data.at(y).at(x+1)<stone_nth) continue;
                if(x!=0) if(raw_data.at(y).at(x-1)==0 || raw_data.at(y).at(x-1)<stone_nth) continue;
            }
        }
        //add
        result.push_back(processes[i].stone);
    }
    return result;
}

//すべての石が自分より若い石に接することができるか確認する
bool field_type::is_stones_can_contact()
{
    int stone_nth;
    int min_stone_nth = processes_min_stone_nth();
    for(size_t i=0;i<processes.size();i++){
        stone_nth = processes[i].stone.get_nth();
        if(min_stone_nth==stone_nth) continue;
        for(size_t y = 0; y < 32; ++y) for(size_t x = 0; x < 32; ++x)
        {
            if(raw_data.at(y).at(x)==stone_nth){
                if(y!=31) if(raw_data.at(y+1).at(x)==0 || raw_data.at(y+1).at(x)<stone_nth) continue;
                if(y!=0) if(raw_data.at(y-1).at(x)==0 || raw_data.at(y-1).at(x)<stone_nth) continue;
                if(x!=31) if(raw_data.at(y).at(x+1)==0 || raw_data.at(y).at(x+1)<stone_nth) continue;
                if(x!=0) if(raw_data.at(y).at(x-1)==0 || raw_data.at(y).at(x-1)<stone_nth) continue;
            }
        }
        //not contact
        return false;
    }
    return true;
}

//置かれた石の一覧を表す配列を返す
std::vector<stone_type> field_type::list_of_stones() const
{
    std::vector<stone_type> result;
    std::transform(processes.begin(), processes.end(), std::back_inserter(result),
            [](auto const & process) { return process.stone; });
    return result;
}

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

void field_type::init_edge(){
    for(int i=0;i<256;i++)is_placed_stone[i]=false;

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

field_type::field_type(std::string const & raw_field_text, std::size_t stone_nth)
{
    has_limit = true;
    provided_stones = stone_nth;
    auto rows = _split(raw_field_text, "\r\n");
    for (std::size_t i = 0; i < raw_data.size(); ++i) {
        std::transform(rows[i].begin(), rows[i].end(), raw_data[i].begin(),
                       [](auto const & c) { return c == '1' ? -1 : 0; });
    }
    make_bit();
    init_edge();
}
field_type::field_type(const int obstacles, const int cols, const int rows){
    has_limit = true;
    set_random(obstacles,cols,rows);
    make_bit();
    init_edge();
}

field_type::raw_field_type const & field_type::get_raw_data() const
{
    return raw_data;
}

void field_type::print_field()
{
    for(auto const&each_raw : raw_data)
    {
        for(auto const each_block : each_raw)
        {
            std::cout << std::setw(3) << each_block;
        }
        std::cout << std::endl;
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
void field_type::set_random(int const obstacle, int const col, int const row)
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
    for(auto row : raw_data) {
        for(auto block : row) {
            ss << (block != 0);
        }
        ss << "\r\n";
    }
    return std::move(ss.str());
}

void field_type::set_provided_stones(size_t ps)
{
    provided_stones = ps;
}
//#BitSystem
//bitデータの作成
void field_type::make_bit()
{
    //make bit plain field
    //bit_plaint_field_only_obstacleは実際には石をおいていない初期のフィールド
    for(int i=0;i<64;i++){
        bit_plain_field[i] = 0xffffffffffffffff;
        bit_only_flame_and_obstacle_field[i] = 0xffffffffffffffff;
        bit_plain_field_only_stones[i] = 0;
    }
    for(int i=16;i<48;i++){
        bit_only_flame_and_obstacle_field[i] = 0xffff00000000ffff;
    }
    for(int y=0;y<32;y++){
        for(int x=0;x<32;x++){
            bit_plain_field[y+16] -= (uint64_t)(raw_data.at(y).at(x) + 1) << ((64-17)-x);
            bit_only_flame_and_obstacle_field[y+16] -= (uint64_t)(raw_data.at(y).at(x)) << ((64-17)-x);
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
            tmp[y] = tmp[y] & (~bit_only_flame_and_obstacle_field[y]);
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
bool field_type::get_has_limit() const{return has_limit;}

int (*field_type::make_manhattan_field(uint64_t const bit_manhattan_start_field_[]))[64]
{
    uint64_t bit_field[64];
    uint64_t bit_field_buffer[64];
    int (*manhattan_field)[64];
    manhattan_field = new int[64][64];
    uint64_t mask = 1;

    auto end_check = [&](){
        for(int i=0;i<64;i++){
            if((bit_field[i] | bit_only_flame_and_obstacle_field[i]) != 0xffffffffffffffff) return false;
        }
        return true;
    };

    auto bit_field_spread = [&](){
        //copy buffer
        for(int i=0;i<64;i++){
            bit_field_buffer[i] = bit_field[i];
        }
        for(int i=0;i<64;i++){
            if(i!=0) bit_field[i] |= bit_field_buffer[i-1];//upper
            if(i!=63) bit_field[i] |= bit_field_buffer[i+1];//under
            bit_field[i] |= bit_field_buffer[i] << 1;//left
            bit_field[i] |= bit_field_buffer[i] >> 1;//right
        }
        for(int i=0;i<64;i++){
            bit_field[i] &= ~bit_only_flame_and_obstacle_field[i];
        }
    };

    auto add_manhattan = [&](){
        for(int i=0;i<64;i++){
            for(int j=0;j<64;j++){
                manhattan_field[i][63 - j] += bit_field[i] >> j & mask;
            }
        }
    };

    //copy
    for(int i=0;i<64;i++){
        bit_field[i] = bit_manhattan_start_field_[i];
    }
    //copy field
    for(int i=0;i<64;i++){
        for(int j=0;j<64;j++){
            manhattan_field[i][63 - j] = bit_field[i] >> j & mask;
        }
    }

    //spread field
    while(1){
        bit_field_spread();
        add_manhattan();
        if(end_check()) break;
    }
    return manhattan_field;
}

#include "yrange3.hpp"
#include <algorithm>
#include <array>
#include <functional>
#include <vector>
#include <boost/format.hpp>

yrange3::yrange3(problem_type _problem)
{
    pre_problem = _problem;
}

yrange3::~yrange3()
{
}

void yrange3::run()
{
    qDebug("yrange3 start");
    field_type& field = pre_problem.field;
    std::vector<stone_type>& stones = pre_problem.stones;

    std::vector<stone_type> c_stones = pre_problem.stones;
    c_stones.erase(c_stones.begin(),c_stones.begin()+c_stones.size()/2);

    ur_atack(field,c_stones);
    std::cout << c_stones.size() << std::endl;
    field.print_field();

    ul_atack(field,c_stones);
    std::cout << c_stones.size() << std::endl;
    field.print_field();
return;
    dr_atack(field,c_stones);
    std::cout << c_stones.size() << std::endl;
    field.print_field();

    dl_atack(field,c_stones);
    std::cout << c_stones.size() << std::endl;
    field.print_field();

    return;



    std::cout << "corner atack done" << std::endl;
    pre_problem.field.print_field();

    for(std::size_t stone_num = 1; stone_num <= stones.size() / 2; ++stone_num)
    {
        stone_type& stone = stones.at(stone_num);
        search_type next = std::move(search(field,stone));
        if(next.point.y == FIELD_SIZE) continue;//どこにも置けなかった
        if(pass(next,stone) == true) continue;
        if(next.flip != stone.get_side()) stone.flip();
        stone.rotate(next.rotate);
        problem.field.put_stone(stone,next.point.y,next.point.x);
    }

}

//おける場所の中から評価値の高いものを選んで返す
yrange3::search_type yrange3::search(field_type& _field, stone_type& stone)
{
    std::vector<search_type> search_vec;
    //おける可能性がある場所すべてにおいてみる
    for(int i = 1 - STONE_SIZE; i < FIELD_SIZE; ++i) for(int j = 1 - STONE_SIZE; j < FIELD_SIZE; ++j) for(int rotate = 0; rotate < 8; ++rotate)
    {
        if(rotate % 2 == 0) stone.rotate(90);
        else stone.flip();
        if(_field.is_puttable(stone,i,j) == true)
        {
            //置けたら接してる辺を数えて配列に挿入
            search_vec.push_back({
                                     point_type{i,j},
                                     stone.get_angle(),
                                     stone.get_side(),
                                     nonput_evaluate(_field,stone,i,j),
                                     0
                                 });
        }
    }
    if(search_vec.size() == 0) return search_type{point_type{FIELD_SIZE,FIELD_SIZE},0,stone_type::Sides::Head,0,0};
    return *std::max_element(search_vec.begin(),search_vec.end(),
                [&](const search_type& lhs, const search_type& rhs){return lhs.score < rhs.score;});
}

int yrange3::get_island(field_type::raw_field_type field/*, point_type const& point*/)
{
    int num = -2;
    std::vector<int> result (FIELD_SIZE * FIELD_SIZE,0);
    std::function<void(int,int,int)> recurision = [&recurision,&field](int y, int x, int num) -> void
    {
        field.at(y).at(x) = num;
        if(0 < y && field.at(y-1).at(x) == 0) recurision(y-1,x,num);
        if(y < FIELD_SIZE - 1 && field.at(y+1).at(x) == 0) recurision(y+1,x,num);
        if(0 < x && field.at(y).at(x-1) == 0) recurision(y,x-1,num);
        if(x < FIELD_SIZE - 1 && field.at(y).at(x+1) == 0) recurision(y,x+1,num);
        return;
    };
    for(int i = 0; i < FIELD_SIZE; ++i) for(int j = 0; j < FIELD_SIZE; ++j)
    {
        if(field.at(i).at(j) == 0) recurision(i,j,num--);
    }
    for(int i = 0; i < FIELD_SIZE; ++i) for(int j = 0; j < FIELD_SIZE; ++j)
    {
        if(field.at(i).at(j) < 0)
        {
            result.at(field.at(i).at(j) * -1)++;
        }
    }
/*
    for(auto& each : field)
    {
        for(auto& block : each) std::cout << block;
        std::cout << std::endl;
    }
    std::cout << "island = " << num*-1 -2 << std::endl;
*/
    return num*-1 -2;
}

bool yrange3::pass(search_type const& search, stone_type const& stone)
{
    if((static_cast<double>(search.score) / static_cast<double>(stone.get_side_length())) < 0.5) return true;
    else return false;
}

bool yrange3::pass(double const score, stone_type const& stone, double threshold)
{
    return (score / static_cast<double>(stone.get_side_length())) < threshold;
}

void yrange3::ur_atack(field_type& field,std::vector<stone_type>& stones)
{       
    int count1 = 1,count2 = 0;
    while(count1 > count2)
    {
        count2 = count1;
        search_type best = {{FIELD_SIZE,FIELD_SIZE},0,stone_type::Sides::Head,-1,0};
        for(std::size_t stone_num = stones.size() - 1; stone_num > 0 ; --stone_num)
        {
            stone_type& stone = stones.at(stone_num);
            for(int i = 31; i >= 16; --i) for(int x = i; x < 32; ++x)
            {
                const int y = x - i - 7;
                atack(y,x,stone,stone_num,count1,field,best,(FIELD_SIZE-y)+x);
            }

            if(count1 == 0 && stone_num < stones.size()*0.9)
            {
                break;
            }
        }
        if(best.score < 0) break;
        field.put_stone_force(stones.at(best.stone).set_angle(best.rotate).set_side(best.flip),best.point.y,best.point.x);
        stones.erase(stones.begin()+best.stone);
        count1++;
    }
}

void yrange3::ul_atack(field_type& field,std::vector<stone_type>& stones)
{
    int count1 = 1,count2 = 0;
    while(count1 > count2)
    {
        count2 = count1;
        search_type best = {{FIELD_SIZE,FIELD_SIZE},0,stone_type::Sides::Head,-1,0};
        for(std::size_t stone_num = stones.size() - 1; stone_num > 0 ; --stone_num)
        {
            stone_type& stone = stones.at(stone_num);
            for(int i = 0; i < 10; ++i) for(int x = -7; x <= i; ++x)
            {
                const int y = i - x - 7;
                atack(y,x,stone,stone_num,count1,field,best,(FIELD_SIZE-y)+(FIELD_SIZE-x));
            }

            if(count1 == 0 && stone_num < stones.size()*0.9)
            {
                break;
            }
        }
        if(best.score < 0) break;
        field.put_stone_force(stones.at(best.stone).set_angle(best.rotate).set_side(best.flip),best.point.y,best.point.x);

        std::cout << stones.at(best.stone).get_nth() << " " << best.score << std::endl;
        stones.erase(stones.begin()+best.stone);
        count1++;

    }
}

void yrange3::dr_atack(field_type& field,std::vector<stone_type>& stones)
{
    int count1 = 0,count2 = 1;
    while(count1 < count2)
    {
        count2 = count1;
        search_type best = {{FIELD_SIZE,FIELD_SIZE},0,stone_type::Sides::Head,-1,0};
        for(std::size_t stone_num = stones.size() - 1; stone_num > 0 ; --stone_num)
        {
            stone_type& stone = stones.at(stone_num);
            for(int i = 0; i < 18; ++i) for(int x = 32 - i; x < 32; ++x)
            {
                const int y = 63 - i - x;
                atack(y,x,stone,stone_num,count1,field,best,(x+y)/2);
                if(count1 == 0)
                {
                    if(best.score < 0) continue;
                    field.put_stone_force(stones.at(best.stone).set_angle(best.rotate).set_side(best.flip),best.point.y,best.point.x);
                    stones.erase(stones.begin()+best.stone);
                    count1++;
                    best = {{FIELD_SIZE,FIELD_SIZE},0,stone_type::Sides::Head,-1,0};
                    break;
                }
            }
        }
        if(best.score < 0) break;
        field.put_stone_force(stones.at(best.stone).set_angle(best.rotate).set_side(best.flip),best.point.y,best.point.x);
        stones.erase(stones.begin()+best.stone);
        count1++;
    }
}

void yrange3::dl_atack(field_type& field, std::vector<stone_type> &stones)
{
    int count1 = 0,count2 = 1;
    while(count1 < count2)
    {
        count2 = count1;
        search_type best = {{FIELD_SIZE,FIELD_SIZE},0,stone_type::Sides::Head,-1,0};
        for(std::size_t stone_num = stones.size() - 1; stone_num > 0 ; --stone_num)
        {
            stone_type& stone = stones.at(stone_num);
            for(int i = 31; i >= 16; --i) for(int x = -7; x < 32 - i; ++x)
            {
                const int y = x + i + 7;
                atack(y,x,stone,stone_num,count1,field,best,(y+(FIELD_SIZE-x))/2);
                if(count1 == 0)
                {
                    if(best.score < 0) continue;
                    field.put_stone_force(stones.at(best.stone).set_angle(best.rotate).set_side(best.flip),best.point.y,best.point.x);
                    stones.erase(stones.begin()+best.stone);
                    count1++;
                    best = {{FIELD_SIZE,FIELD_SIZE},0,stone_type::Sides::Head,-1,0};
                    break;
                }
            }
        }
        if(best.score < 0) break;
        field.put_stone_force(stones.at(best.stone).set_angle(best.rotate).set_side(best.flip),best.point.y,best.point.x);
        stones.erase(stones.begin()+best.stone);
        count1++;
    }
}

int yrange3::nonput_evaluate(field_type const& field, stone_type const& stone, int y, int x) const
{
    int score = 0;
    const int init_y = (y < 2) ? 0 : y - 1;
    const int init_x = (x < 2) ? 0 : x - 1;
    for(int i = init_y; i < init_y + STONE_SIZE - 1&& i + 1 < FIELD_SIZE; ++i) for(int j = init_x; j < init_x + STONE_SIZE -1 && j + 1 < FIELD_SIZE; ++j)
    {
        if(field.get_raw_data().at(i).at(j) != 0)
        {
            if(stone.get_raw_data().at(i - init_y).at(j - init_x + 1) == 1) score++;
            if(stone.get_raw_data().at(i - init_y + 1).at(j - init_x) == 1) score++;
        }
        if(stone.get_raw_data().at(i - init_y).at(j - init_x) == 1)
        {
            if(field.get_raw_data().at(i).at(j + 1) != 0) score++;
            if(field.get_raw_data().at(i + 1).at(j) != 0) score++;
            if(i == FIELD_SIZE - 1 || i == 0) score++;
            if(j == FIELD_SIZE - 1 || j == 0) score++;
        }
    }
    return score;
}

int yrange3::evaluate_reverse(field_type const& field, stone_type stone,int const i, int const j)const
{
    int const n = stone.get_nth();
    int count = 0;
    bool conection = false;
    for(int k = (i < 2) ? 0 : i - 1 ;k < i + STONE_SIZE && k + 1 < FIELD_SIZE; ++k) for(int l = (j < 2) ? 0 : j - 1; l < j + STONE_SIZE && l + 1 < FIELD_SIZE; ++l)
    {
        if(conection == false && (field.get_raw_data().at(k).at(l+1) > n || field.get_raw_data().at(k+1).at(l) > n)) conection = true;
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
    if(conection == true) return count;
    else return count * -1;
}

//評価関数
int yrange3::evaluate(field_type const& field, stone_type stone,int const i, int const j)const
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

void yrange3::atack(int y, int x, stone_type& stone, std::size_t stone_num, int& count, field_type& field, search_type& best, int weight)
{
    //std::cout << count << std::endl;
    for(int rotate = 0; rotate < 8; ++rotate)
    {
        if(rotate %2 == 0) stone.rotate(90);
        else stone.flip();
        if(field.is_puttable_force(stone,y,x) == true)
        {
            //本当はコピーじゃなくてremove_stoneしたい
            field_type i_field = field;
            i_field.put_stone_force(stone,y,x);
            int const island = get_island(i_field.get_raw_data());
            int score = evaluate_reverse(field,stone,y,x);
            if(count == 0 && evaluate(field,stone,y,x)+weight > best.score && island < 2)
            {
                best = search_type{point_type{y,x}, stone.get_angle(), stone.get_side(), evaluate(field,stone,y,x)+weight, stone_num};
                //std::cout << stone_num+129 << " " << y << " " <<  " " << x << " " << score << std::endl;
            }
            else if(count > 0 && score > best.score)
            {
                //std::cout << stone_num+129 << " " << y << " " <<  " " << x << " " << score << std::endl;
                if(island < 2)
                {
                    best = search_type{point_type{y,x}, stone.get_angle(), stone.get_side(), score, stone_num};
                }
            }
        }
    }
}

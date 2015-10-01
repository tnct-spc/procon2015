#include "square.hpp"
#include <QFile>
#include <QIODevice>
#include <algorithm>
#include "QElapsedTimer"

square::square(problem_type _problem)
{
    algorithm_name = "square";
    original_problem = _problem;
}

square::~square()
{

}

void square::run()
{
        QElapsedTimer et;
        et.start();
        qDebug("@square start direction");
        problem=original_problem;
        //Make answer
        //problem.field.cancellation_of_restriction();
        for(stone_type stone : problem.stones){
            solve(stone);
        }
        //解答の送信
        qDebug("@square emit direction");
        int64_t time = et.elapsed();
        print_text(std::to_string(time));
        answer_send(problem.field);
}

void square::solve(stone_type stone)
{
    //init
    for(int i=0;i<34;i++){
        for(int j=0;j<34;j++){
            field[i][j]=true;
        }
    }
    field_type::raw_field_type raw_field = problem.field.get_raw_data();
    for(int y=0;y<32;y++){
        for(int x=0;x<32;x++){
            if(raw_field[y][x]==0){
                field[y+1][x+1]=false;
            }else{
                field[y+1][x+1]=true;
            }
        }
    }
    max_num=-1;//min-1
    min_num=32*32+1;//max+1
    //斜めに置ける場所を探して石を挿入していく
    //side
    for(int dy=-7;dy<32;dy++){
        for(int dx=-7;dx<32;dx++){
            put_stone_side(stone,dy,dx);
        }
    }
    //hole
    if(max_num!=-1){
        int size = max_stone.size();
        for(int i=0;i<size;i++){
            put_stone_hole(max_stone[i],max_y[i],max_x[i]);
        }
    }
    //設置
    //qDebug("put stone at %d:%d %d:%d",min_y,min_x,min_stone.get_side(), min_stone.get_angle());
    if(min_num!=32*32+1) problem.field.put_stone_basic(min_stone,min_y,min_x);
    //qDebug("put ok");
}


void square::put_stone_side(stone_type& stone,int dy,int dx)
{
    int side_num;
    //反転
    for(int flip = 0; flip < 2; flip ++){
        //回転
        for(int angle = 0; angle < 4; angle ++){
            //フィールドに置けるかチェック
            if(problem.field.is_puttable_basic(stone,dy,dx)){
                //qDebug("go at %d:%d:%d:%d",dy,dx,flip,angle);
                side_num = count_side(stone,dy,dx);
                //update
                if(side_num > max_num){
                    max_num = side_num;
                    max_y.clear();
                    max_x.clear();
                    max_stone.clear();
                    max_y.push_back(dy);
                    max_x.push_back(dx);
                    max_stone.push_back(stone);
                }else if(side_num == max_num){
                    max_y.push_back(dy);
                    max_x.push_back(dx);
                    max_stone.push_back(stone);
                }
                //if(flip==1 && angle==1) qDebug("1");
            }else{
                //if(flip==1 && angle==1) qDebug("0");
            }
            stone.rotate(90);
        }
        stone.flip();
    }
}

int square::count_side(stone_type const& stone, int dy,int dx)
{
    int side_num=0;
    int count;
    //Sideの数を数える
    for(int y=0;y<8;y++){
        for(int x=0;x<8;x++){
            if(stone.at(y,x)){
                count=0;
                if(dy+y>=0 && field[dy+1+y-1][dx+1+x]==true) count++;
                if(dy+y<=31 && field[dy+1+y+1][dx+1+x]==true) count++;
                if(dx+x>=0 && field[dy+1+y][dx+1+x-1]==true) count++;
                if(dx+x<=31 && field[dy+1+y][dx+1+x+1]==true) count++;
                if(count==1) side_num+=1;
                if(count==2) side_num+=3;
                if(count==3) side_num+=5;
                if(count==4) side_num+=16;
            }
        }
    }

    return side_num;
}

void square::put_stone_hole(stone_type& stone,int dy,int dx)
{
    int hole_num;
    problem.field.put_stone_basic(stone,dy,dx);
    field_type::raw_field_type raw_field = problem.field.get_raw_data();
    for(int y=0;y<32;y++){
        for(int x=0;x<32;x++){
            if(raw_field[y][x]>=1 || raw_field[y][x]==-1){
                field[y+1][x+1]=true;
            }else{
                field[y+1][x+1]=false;
            }
        }
    }
    hole_num = count_hole();
    //update
    if(hole_num < min_num){
        min_num = hole_num;
        min_y = dy;
        min_x = dx;
        min_stone = stone;
    }
    problem.field.remove_stone_basic();
}


int square::count_hole()
{
    int hole_num=0;
    //穴の数を数える
    bool hole_flag;
    do{
        hole_flag=false;
        for(int size=0;size<32;size++){//'0'=1*1, 'n'=(n+1)*(n+1)
            for(int y=0;y<32-size;y++){
                for(int x=0;x<32-size;x++){
                    switch(is_depression_or_hole_or_none(y,x,size)){
                    case 1:
                        hole_flag=true;
                        hole_num += size+1;
                        break;
                    case 2:
                        hole_flag=true;
                        hole_num += size+10;
                        break;
                    default:
                        break;
                    }
                }
            }
        }
    }while(hole_flag);

    return hole_num;
}

int square::is_depression_or_hole_or_none(int dy, int dx, int size)
{
    bool flag;
    int count=0;
    for(int y=dy ; y<=dy+size ; y++){
        for(int x=dx ; x<=dx+size ; x++){
            if(field[y+1][x+1]==true) return false;
        }
    }
    flag=true;
    for(int x=dx; x<=dx+size ; x++)
        if(field[dy][x+1]==false) flag=false;
    if(flag){
        count++;
    }
    flag=true;
    for(int x=dx; x<=dx+size ; x++)
        if(field[dy+size+2][x+1]==false) flag=false;
    if(flag){
        count++;
    }
    flag=true;
    for(int y=dy; y<=dy+size ; y++)
        if(field[y+1][dx]==false) flag=false;
    if(flag){
        count++;
    }
    flag=true;
    for(int y=dy; y<=dy+size ; y++)
        if(field[y+1][dx+size+2]==false) flag=false;
    if(flag){
        count++;
    }
    if(count<3) return 0;//none
    //is_hole_or_depression
    for(int y=dy ; y<=dy+size ; y++){
        for(int x=dx ; x<=dx+size ; x++){
            field[y+1][x+1]=true;
        }
    }
    if(count==4) return 2;//hole
    return 1;//depression
}

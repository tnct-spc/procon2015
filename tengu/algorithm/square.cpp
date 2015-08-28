#include "square.hpp"
#include <QFile>
#include <QIODevice>
#include <algorithm>

square::square(problem_type _problem)
{
    original_problem = _problem;
}

square::~square()
{

}

void square::run()
{
    for(int direction=0; direction<8; direction++){
        qDebug("start direction%d",direction);
        problem=original_problem;
        //Make answer
        for(stone_type stone : problem.stones){
            solve(stone,direction);
        }
        //解答の送信
        qDebug("emit direction%d",direction);
        emit answer_ready(problem.field);
    }
}

void square::direction_change(int *dy, int *dx, int direction)
{
    int temp;
    if(direction%4 >= 2) *dy=39-*dy;//上下反転
    if(direction%2 == 1) *dx=39-*dx;//左右反転
    if(direction >= 4){//向き回転
        temp=*dy;
        *dy=*dx;
        *dx=temp;
    }
}

void square::solve(stone_type stone, int direction)
{
    int y,x;
    //init
    for(int i=0;i<34;i++){
        for(int j=0;j<34;j++){
            field[i][j]=true;
        }
    }
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
    max_num=-1;//min-1
    min_num=32*32+1;//max+1
    //斜めに置ける場所を探して石を挿入していく
    //side
    for(int i=0;i<40;i++){
        y=0;
        x=i;
        for(int j=0;j<=i;j++){
            direction_change(&y,&x,direction);
            put_stone_side(stone,y-7,x-7);
            y++;
            x--;
        }
    }
    for(int i=38;i>=0;i--){
        y=39-i;
        x=39;
        for(int j=0;j<=i;j++){
            direction_change(&y,&x,direction);
            put_stone_side(stone,y-7,x-7);
            y++;
            x--;
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
    if(min_num!=32*32+1) problem.field.put_stone(min_stone,min_y,min_x);
    //qDebug("put ok");
}


void square::put_stone_side(stone_type stone,int dy,int dx)
{
    int side_num;
    //反転
    for(int flip = 0; flip < 2; flip ++){
        //回転
        for(int angle = 0; angle < 4; angle ++){
            //フィールドに置けるかチェック
            if(problem.field.is_puttable(stone,dy,dx)){
                //qDebug("go at %d:%d:%d:%d",dy,dx,flip,angle);
                problem.field.put_stone(stone,dy,dx);
                side_num = count_side(stone);
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
                problem.field.remove_stone(stone);
                //if(flip==1 && angle==1) qDebug("1");
            }else{
                //if(flip==1 && angle==1) qDebug("0");
            }
            stone.rotate(90);
        }
        stone.flip();
    }
}

int square::count_side(stone_type stone)
{
    int side_num=0;
    int count;
    //Sideの数を数える
    field_type::raw_field_type raw_field = problem.field.get_raw_data();
    int stone_nth = stone.get_nth();
    for(int y=1;y<33;y++){
        for(int x=1;x<33;x++){
            if(raw_field[y-1][x-1]==stone_nth){
                count=0;
                if(field[y-1][x]==true) count++;
                if(field[y+1][x]==true) count++;
                if(field[y][x-1]==true) count++;
                if(field[y][x+1]==true) count++;
                side_num+=count*count;
            }
        }
    }

    return side_num;
}

void square::put_stone_hole(stone_type stone,int dy,int dx)
{
    int hole_num;
    problem.field.put_stone(stone,dy,dx);
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
    problem.field.remove_stone(stone);
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
                    if(is_hole(y,x,size)){
                        hole_flag=true;
                        hole_num += size;
                    }
                }
            }
        }
    }while(hole_flag);

    return hole_num;
}

bool square::is_hole(int dy, int dx, int size)
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
    if(count<3) return false;
    //ok
    for(int y=dy ; y<=dy+size ; y++){
        for(int x=dx ; x<=dx+size ; x++){
            field[y+1][x+1]=true;
        }
    }
    return true;
}

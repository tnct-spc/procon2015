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
    if(direction%4 >= 2) *dy=39-(*dy+7)-7;//上下反転
    if(direction%2 == 1) *dx=39-(*dx+7)-7;//左右反転
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
    min_num=32*32+1;//max+1
    for(int i=0;i<34;i++){
        for(int j=0;j<34;j++){
            field[i][j]=true;
        }
    }
    //斜めに置ける場所を探して石を挿入していく
    //設置可能範囲は左上座標(-7,-7)から左上座標(31,31)まで
    for(int i=0;i<40;i++){
        y=0;
        x=i;
        for(int j=0;j<=i;j++){
            direction_change(&y,&x,direction);
            put_stone(stone,y,x);
            y++;
            x--;
        }
    }
    for(int i=38;i>=0;i--){
        y=i;
        x=40;
        for(int j=0;j<=i;j++){
            direction_change(&y,&x,direction);
            put_stone(stone,y,x);
            y++;
            x--;
        }
    }

    /*
    for(int dy=-7;dy<32;dy++){
        //qDebug("\n");
        for(int dx=-7;dx<32;dx++){
            y=dy;
            x=dx;
            direction_change(&y,&x,direction);
            put_stone(stone,y,x);
        }
    }
    */
    qDebug("minnum=%d",min_num);
/*
    for(int dy=0;dy<=40;dy++){
        for(int dx=0;dx<=40;dx++){
            qDebug("%d",point[dy*40*2*4+dx*2*4+2]);
        }
        qDebug("@");
    }
*/
    //設置
    qDebug("put stone at %d:%d %d:%d",min_y,min_x,min_stone.get_side(), min_stone.get_angle());
    if(min_num!=32*32+1) problem.field.put_stone(min_stone,min_y,min_x);
    qDebug("put ok");
}

void square::put_stone(stone_type stone,int dy,int dx)
{
    int hole_num;
    //反転
    for(int flip = 0; flip < 2; flip ++){
        //回転
        for(int angle = 0; angle < 4; angle ++){
            //フィールドに置けるかチェック
            if(problem.field.is_puttable(stone,dy,dx)){
                //qDebug("go at %d:%d:%d:%d",dy,dx,flip,angle);
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
                //if(flip==1 && angle==1) qDebug("1");
            }else{
                //if(flip==1 && angle==1) qDebug("0");
            }
            stone.rotate(90);
        }
        stone.flip();
    }
}


int square::count_hole()
{
    int hole_num=0;
    //穴の数を数える
    bool hole_flag;
    do{
        for(int size=0;size<32;size++){//'0'=1*1, 'n'=(n+1)*(n+1)
            for(int y=0;y<32-size;y++){
                for(int x=0;x<32-size;x++){
                    hole_num += /*size * */(int)is_hole(y,x,size);
                }
            }
        }
    }while(hole_flag);

    return hole_num;
}

bool square::is_hole(int dy, int dx, int size)
{
    bool flag,count=0;
    for(int y=dy ; y<=dy+size ; y++){
        for(int x=dx ; x<=dx+size ; x++){
            if(field[y][x]==true) return false;
        }
    }
    flag=false;
    for(int x=dx-1; x<=dx+size+1 ; x++)
        if(field[dy-1][x]==false) flag=true;
    if(flag)count++;
    flag=false;
    for(int x=dx-1; x<=dx+size+1 ; x++)
        if(field[dy+size+1][x]==false) flag=true;
    if(flag)count++;
    flag=false;
    for(int y=dy; y<=dy+size ; y++)
        if(field[y][dx-1]==false) flag=true;
    if(flag)count++;
    flag=false;
    for(int y=dy; y<=dy+size ; y++)
        if(field[y][dx+size+1]==false) flag=true;
    if(flag)count++;
    if(count<3) return false;
    return true;
}

/*
int square::count_hole()
{
    int hole_num=0;
    //穴の数を数える
    bool hole_flag;
    do{
        hole_flag=false;
        for(int y=2;y<=33;y++){
            for(int x=2;x<=33;x++){
                if(field[y][x]==0 && field[y+1][x]+field[y-1][x]+field[y][x+1]+field[y][x-1]>=2){
                    //穴である
                    hole_flag=true;
                    field[y][x]=true;
                    hole_num++;
                }

                if(field[y][x]==0 &&
                   field[y+1][x]==0 &&
                   field[y+1][x+1]==0 &&
                   field[y][x+1]==0
                ){
                    int fcount=0;
                    if(field[y-1][x] && field[y-1][x+1])fcount++;
                    if(field[y][x+2] && field[y+1][x+2])fcount++;
                    if(field[y+2][x] && field[y+2][x+1])fcount++;
                    if(field[y][x-1] && field[y+1][x-1])fcount++;
                    if(fcount >= 3 ){
                        //qDebug("anaatta %d:%d %d,%d",dy,dx,flip,angle);
                        //oo穴である
                        hole_flag=true;
                        field[y][x]=true;
                        field[y+1][x]=true;
                        field[y+1][x+1]=true;
                        field[y][x+1]=true;
                        hole_num+=4;
                    }
                }
            }
        }
    }while(hole_flag);
    return hole_num;
}
*/

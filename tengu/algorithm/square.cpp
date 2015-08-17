#include "square.hpp"
#include <QFile>
#include <QIODevice>
#include <algorithm>

square::square(problem_type _problem)
{
    pre_problem = _problem;
}

square::~square()
{

}

void square::run(){
    for(int i=0;i<1;i++){
        problem=pre_problem;
        //石を設置
        for(auto stone : problem.stones){
            solve(i,stone);
        }
        //解答の送信
        qDebug("emit square-No.%d",i);
        emit answer_ready(problem.field);
    }
}

void square::solve(int mode,auto stone){
    int temp;
    int point[40*40*2*4]; //about10,000
    for(int i=0;i<40*40*2*4;i++){
        point[i]=-1;
    }
    bool field[34][34];
    for(int i=0;i<34;i++){
        for(int j=0;j<34;j++){
            field[i][j]=1;
        }
    }
    //端から置ける場所を探して石を挿入していく
    //設置可能範囲は左上座標(-7,-7)から左上座標(31,31)まで
    for(int dy=-7;dy<=32;dy++){
        for(int dx=-7;dx<=32;dx++){
/*
            //8beam
            if(mode%4 >= 2) dy=25-dy;//上下反転
            if(mode%2 == 1) dx=25-dx;//左右反転
            if(mode   >= 4){//向き回転
                temp=dy;
                dy=dx;
                dx=temp;
            }
*/
            //反転
            for(int flip = 0; flip < 2; flip ++){
                //回転
                for(int angle = 0; angle < 4; angle ++){
                    //フィールドに置けるかチェック
                    if(problem.field.is_puttable(stone,dy,dx)){
                        point[(dy+7)*40*2*4+(dx+7)*2*4+flip*4+angle]=0;
                        //設置
                        problem.field.put_stone(stone,dy,dx);
                        std::array<std::array<int, 32>, 32> raw_field = problem.field.get_raw_data();
                        for(int y=0;y<32;y++){
                            for(int x=0;x<32;x++){
                                if(raw_field[y][x]>=1){
                                    field[y+1][x+1]=true;
                                }else{
                                    field[y+1][x+1]=false;
                                }
                            }
                        }
                        //穴の数を数える
                        bool hole_flag;
                        do{
                            hole_flag=false;
                            for(int y=1;y<=32;y++){
                                for(int x=1;x<=32;x++){
                                    if(field[y][x]==0 && field[y+1][x]+field[y-1][x]+field[y][x+1]+field[y][x-1]>=3){
                                        //穴である
                                        hole_flag=true;
                                        field[y][x]=true;
                                        point[(dy+7)*40*2*4+(dx+7)*2*4+flip*4+angle]++;
                                    }
                                    if(field[y][x]==0 && field[y-1][x]==1 && field[y][x-1]==1 &&
                                       field[y+1][x]==0 && field[y+2][x]==1 && field[y+1][x-1]==1 &&
                                       field[y+1][x+1]==0 && field[y+2][x+1]==1 && field[y+1][x+2]==1 &&
                                       field[y][x+1]==0 && field[y-1][x+1]==1 && field[y][x+2]==1
                                    ){
                                        //oo穴である
                                        hole_flag=true;
                                        field[y][x]=true;
                                        field[y+1][x]=true;
                                        field[y+1][x+1]=true;
                                        field[y][x+1]=true;
                                        point[(dy+7)*40*2*4+(dx+7)*2*4+flip*4+angle]+=4;
                                    }
                                }
                            }
                        }while(hole_flag);
                        //取り除く
                        problem.field.remove_stone(stone);
                    }
                    stone.rotate(90);
                }
                stone.flip();
            }
/*
            //8beamを元に戻す
            if(mode   >= 4){
                temp=dy;
                dy=dx;
                dx=temp;
            }
            if(mode%2 == 1) dx=25-dx;
            if(mode%4 >= 2) dy=25-dy;
*/
        }
    }
    //一番小さいのを設置
    int min_num=40*40+1;
    int min_point[4];
    for(int dy=0;dy<40;dy++){
        for(int dx=0;dx<40;dx++){
            for(int flip = 0; flip < 2; flip ++){
                for(int angle = 0; angle < 4; angle ++){
                    if(point[dy*40*2*4+dx*2*4+flip*4+angle]!=-1){
                        if(point[dy*40*2*4+dx*2*4+flip*4+angle] < min_num){
                            qDebug("min at %d:%d %d,%d",dy,dx,flip,angle);
                            min_num=point[dy*40*2*4+dx*2*4+flip*4+angle];
                            min_point[0]=dy-7;
                            min_point[1]=dx-7;
                            min_point[2]=flip;
                            min_point[3]=angle;
                        }
                    }
                }
            }
        }
    }
    qDebug("minnum=%d",min_num);
    /*
    for(int dy=0;dy<=40;dy++){
        for(int dx=0;dx<=40;dx++){
            qDebug("%d",point[dy*40*2*4+dx*2*4]);
        }
        qDebug("@");
    }*/
/*
    //8beam
    if(mode%4 >= 2) min_point[0]=25-min_point[0];//上下反転
    if(mode%2 == 1) min_point[1]=25-min_point[1];//左右反転
    if(mode   >= 4){//向き回転
        temp=min_point[0];
        min_point[0]=min_point[1];
        min_point[1]=temp;
    }
*/
    for(int flip = 0; flip < min_point[2]; flip ++){
        stone.flip();
    }
    for(int angle = 0; angle < min_point[3]; angle ++){
       stone.rotate(90);
    }
    //設置
    qDebug("put stone at %d:%d %d,%d",min_point[0],min_point[1],min_point[2],min_point[3]);
    if(min_num!=40*40+1) problem.field.put_stone(stone,min_point[0],min_point[1]);
    qDebug("put ok");
}

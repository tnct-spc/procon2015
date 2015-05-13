#ifndef GAMESTAGE_H
#define GAMESTAGE_H

#include <QWidget>
#include "ui_outside_server.h"
#include <QGraphicsScene>
#include <QGraphicsRectItem>
#include <QGraphicsItem>
#include <QFile>
#include <QTimer>
#include <global.h>

struct point{
    int x;
    int y;
};

class GameStage : public QObject
{
    Q_OBJECT

public:
    explicit GameStage(QObject *parent = 0);
    //Intialize
    void IntializeStage(QGraphicsScene *stage,int base_x,int base_y,int block_size);
    //Intialize stage
    void MakeStageData(/*bool stage_state[48][48], bool stone_state[256][8][8], int stone_num*/);
    //Read Answer(answer.txt)
    void StartAnswer(int answer_flow[256][5],int answer_num,QString userid, int answer_point);
    //
    void update_ranking_tag(int ranking);

private: 
    //field
    QGraphicsScene *field_; //Field pointer
    //stage
    QGraphicsRectItem *stage_[48][48];//Stage [8+32+8][8+32+8]
    bool stage_state_[48][48]; //Stage State
    point based_point_;
    int block_size_;
    //stone
    QGraphicsRectItem *stone_[256][8][8]; //Stone [8][8]
    bool stone_state_[256][8][8]; //Stone State
    int stone_num_;
    //tag
    QGraphicsTextItem *tag_name_; //NameTag
    QGraphicsTextItem *tag_point_; //PointTag
    QGraphicsTextItem *tag_ranking_; //Ranking
    //answer
    int answer_flow_[256][5];
    int answer_num_;
    int answer_point_;
    //answer animation
    int stone_flow_count_;//animation count
    QTimer *answer_animation_timer_;

private slots:
    //Animation Answer
    void AnswerAnimation();
};

#endif // GAMESTAGE_H

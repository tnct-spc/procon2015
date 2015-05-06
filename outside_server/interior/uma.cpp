#include "uma.h"

UMA::UMA(QObject *parent) : QObject(parent){
    count_uma = 0;
    cmb = 0.3;
    cmx = 256 / 0.1;
    cmr = 256 / cmb;
    cms = 2 / cmb;

    QTimer *uma_timer;
    uma_timer = new QTimer();
    QObject::connect(uma_timer, SIGNAL(timeout()), this, SLOT(Call()));
    uma_timer->start(16/*interval 16*/);
}

void UMA::Call(){
    Dance();
}

void UMA::StartUp(QGraphicsScene *stage,double opacity){

    //各部位の場所と角度と回転軸
    int usendo=5;
    atama = new QGraphicsRectItem(1295, 150, 200, 200);
    dou = new QGraphicsRectItem(1300, 400, 200, 400);
    dou->setTransformOriginPoint(1400, 400);
    ude2_l = new QGraphicsRectItem(1170, 350, 200, 50);
    ude2_l->setTransformOriginPoint(1170, 350);
    ude2_l->setRotation(30);
    ude1_l = new QGraphicsRectItem(1120, 350, 200, 50);
    ude1_l->setTransformOriginPoint(1120, 350);
    ude1_l->setRotation(-70);
    hand_l = new QGraphicsRectItem(1120, 120, 120, 60);
    hand_l->setTransformOriginPoint(1230, 170);
    hand_l->setRotation(30);
    asi_l = new QGraphicsRectItem(1420, 780, 70, 250);
    asi_l->setTransformOriginPoint(1455, 1030);
    ude2_r = new QGraphicsRectItem(1630, 400, 200, 50);
    ude2_r->setTransformOriginPoint(1630, 400);
    ude2_r->setRotation(150);
    ude1_r = new QGraphicsRectItem(1630, 400, 200, 50);
    ude1_r->setTransformOriginPoint(1630, 400);
    ude1_r->setRotation(-110);
    hand_r = new QGraphicsRectItem(1570, 150, 120, 60);
    hand_r->setTransformOriginPoint(1570, 210);
    hand_r->setRotation(-30);
    asi_r = new QGraphicsRectItem(1310, 780, 70, 250);
    asi_r->setTransformOriginPoint(1345, 1030);

    //usendo
    atama->setZValue(usendo);
    dou->setZValue(usendo);
    ude2_l->setZValue(usendo);
    ude1_l->setZValue(usendo);
    hand_l->setZValue(usendo);
    asi_l->setZValue(usendo);
    ude2_r->setZValue(usendo);
    ude1_r->setZValue(usendo);
    hand_r->setZValue(usendo);
    asi_r->setZValue(usendo);


    //透明度
    atama->setOpacity(opacity);
    dou->setOpacity(opacity);
    ude2_l->setOpacity(opacity);
    ude1_l->setOpacity(opacity);
    hand_l->setOpacity(opacity);
    asi_l->setOpacity(opacity);
    ude2_r->setOpacity(opacity);
    ude1_r->setOpacity(opacity);
    hand_r->setOpacity(opacity);
    asi_r->setOpacity(opacity);

    //追加
    stage->addItem(atama);
    stage->addItem(dou);
    stage->addItem(ude2_l);
    stage->addItem(ude1_l);
    stage->addItem(hand_l);
    stage->addItem(asi_l);
    stage->addItem(ude2_r);
    stage->addItem(ude1_r);
    stage->addItem(hand_r);
    stage->addItem(asi_r);
}

void UMA::Dance(){
    //頭,腕,手の動作
    if (count_uma % (cmr / 2) < (cmr / 4)){
        atama->moveBy(0, 10);
        ude1_r->moveBy(0, 2);
        hand_r->setRotation(-30 + (count_uma % (cmr / 4)) / cms * 2);
        ude2_r->moveBy(0, 2);
        ude1_l->moveBy(0, 2);
        hand_l->setRotation(30 - (count_uma % (cmr / 4)) / cms * 2);
        ude2_l->moveBy(0, 2);
    }
    else if (count_uma % (cmr / 2) < (cmr / 2)){
        atama->moveBy(0, -10);
        ude1_r->moveBy(0, -2);
        hand_r->setRotation(-30 + (64) - (count_uma % (cmr / 4)) / cms * 2);
        ude2_r->moveBy(0, -2);
        ude1_l->moveBy(0, -2);
        hand_l->setRotation(30 - (64) + (count_uma % (cmr / 4)) / cms * 2);
        ude2_l->moveBy(0, -2);
    }

    //胴体,足の動作
    if (count_uma < (cmr / 4)){
        dou->setRotation((count_uma % (cmr / 4)) / cms);
        asi_r->setRotation(-(count_uma % (cmr / 4)) / cms);
        asi_l->setRotation(-(count_uma % (cmr / 4)) / cms);
    }
    else if (count_uma < (cmr / 2)){
        dou->setRotation(((cmr / 4) - count_uma % (cmr / 4)) / cms);
        asi_r->setRotation((-(cmr / 4) + (count_uma % (cmr / 4))) / cms);
        asi_l->setRotation((-(cmr / 4) + (count_uma % (cmr / 4))) / cms);
    }
    else if (count_uma < ((cmr / 4) * 3)){
        dou->setRotation(-((count_uma % (cmr / 4)) / cms));
        asi_r->setRotation((count_uma % (cmr / 4)) / cms);
        asi_l->setRotation((count_uma % (cmr / 4)) / cms);
    }
    else if (count_uma < cmr){
        dou->setRotation((-(cmr / 4) + (count_uma % (cmr / 4))) / cms);
        asi_r->setRotation(((cmr / 4) - count_uma % (cmr / 4)) / cms);
        asi_l->setRotation(((cmr / 4) - count_uma % (cmr / 4)) / cms);
    }


    count_uma += 16;
    if (count_uma >= cmr)count_uma = 0;

}


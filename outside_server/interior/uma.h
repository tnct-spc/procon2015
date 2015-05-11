#ifndef UMA_H
#define UMA_H

#include <QWidget>
#include "ui_outside_server.h"
#include <QGraphicsScene>
#include <QGraphicsRectItem>
#include <QTimer>

class UMA : public QObject
{
    Q_OBJECT
public:
    explicit UMA(QObject *parent = 0);
    void StartUp(QGraphicsScene *stage, double opacity);

private slots:
    void Call();

private:
    void Dance();
    //上から頭、胴体、左腕２、左腕１、左手、左足、右腕２、右腕１、右手、右足
    QGraphicsRectItem *atama;
    QGraphicsRectItem *dou;
    QGraphicsRectItem *ude2_l;
    QGraphicsRectItem *ude1_l;
    QGraphicsRectItem *hand_l;
    QGraphicsRectItem *asi_l;
    QGraphicsRectItem *ude2_r;
    QGraphicsRectItem *ude1_r;
    QGraphicsRectItem *hand_r;
    QGraphicsRectItem *asi_r;
    //ダンス
    int count_uma;
    double cmb;
    int cmx;
    int cmr;
    int cms;
};

#endif // UMA_H

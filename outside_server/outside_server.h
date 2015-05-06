#ifndef OUTSIDESERVER_H
#define OUTSIDESERVER_H

#include <QWidget>
#include <QGraphicsScene>
#include <QGraphicsRectItem>
#include <QTimer>
#include <QFile>
#include <QFileDialog>
#include <interior/uma.h>
#include <QGraphicsTextItem>
#include <game_stage.h>
//only use SimulateAnswerPoint
#include <http/answer_form.h>

class OutsideServer : public QWidget
{
    Q_OBJECT

public:
    explicit OutsideServer(QWidget *parent = 0);
    ~OutsideServer();

public slots:
    //ui
    void loadbutton_clicked();
    void uisizebutton_clicked();
    void pnspinbutton_changed(QString num);
    //in class
    void ReserveAnswer();

private:
    //UMA
    UMA uma;

    //
    AnswerForm pointer;

    //return answer num
    int ConvertAnswer(QString filename_answer,int answer_flow[256][4]);
    //reset folder
    bool ResetFolder(const QString & dirName);



    //ui
    Ui::procon26server *ui;
    QGraphicsScene *mainstage;
    QString problem_number_spin_button_value_;

    //stage class
    GameStage game_stage_[6];//If error,change "[6]" to "[7]".

    //answer allocation
    int answer_points_[6];//-1 is Empty.
    QString answer_players_[6];

    //pass
    QString ProblemFolderName=QCoreApplication::applicationDirPath()+"/docroot/files/";
    QString AnswerFolderName=QCoreApplication::applicationDirPath()+"/etc/answer/";

    //problem ok flag
    bool problem_flag; 

};

#endif // OUTSIDESERVER_H

#ifndef OUTSIDESERVER_H
#define OUTSIDESERVER_H

#include <QWidget>
#include <QMatrix>
#include <QGraphicsScene>
#include <QGraphicsRectItem>
#include <QGraphicsTextItem>
#include <QTimer>
#include <QFile>
#include <QFileDialog>

#include <interior/uma.h>
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
    //ui_func
    void loadbutton_clicked();
    void uisizebutton_clicked();
    void pnspinbutton_changed(QString num);
    void push_go();
    void push_back();
    void push_stop();
    void push_restart();
    //func
    void ReserveAnswer();

private:
    //UMA
    UMA uma;

    //only use get_point
    AnswerForm pointer;

    //reset folder
    bool ResetFolder(const QString & dirName);
    //
    void resizeEvent(QResizeEvent *event);
    //
    void Decode_problem_and_set(QString problem_file_name);



    //ui
    Ui::procon26server *ui;
    QGraphicsScene *mainstage;
    QString problem_number_spin_button_value_;

    //stage class
    GameStage game_stage_[6];

    //answer allocation
    int answer_points_[6];//-1 is Empty.
    QString answer_players_[6];

    //pass
    QString ProblemFolderName=QCoreApplication::applicationDirPath()+"/docroot/problem/";

    //problem ok flag
    bool problem_flag; 

};

#endif // OUTSIDESERVER_H

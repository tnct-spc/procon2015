#ifndef ANSWERFORM_H
#define ANSWERFORM_H

#include <src/qhttpserver.h>
#include <src/qhttprequest.h>
#include <src/qhttpresponse.h>
#include <QCoreApplication>
#include <global.h>
#include <QFile>
#include <QUrlQuery>
#include <QRegExp>


class AnswerForm : public QObject
{
    Q_OBJECT
public:
    AnswerForm(QObject* parent=0);
    //
    void Service(QHttpRequest* request, QHttpResponse* response);
    //return point of answer
    QString SimulateAnswerPoint(QString plaintext_answer_data);

private:
    //put a stone on stage
    bool PutStone();
    //
    bool format_check(QString plain_data,QString format_type);

    //pass
    QString AnswerFolderName=QCoreApplication::applicationDirPath()+"/etc/answer/";

    //stage data
    int stage_state_[48][48];//0=empty;1=block;2=answer_block
    bool stone_state_[256][8][8];
    int stone_num_;
    //answer data
    int answer_flow_[256][4];
    int answer_num_;
    int stone_flow_count_;

};

#endif // ANSWERFORM_H

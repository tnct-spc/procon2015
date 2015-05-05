#include "answer_form.h"
#include <http/request_mapper.h>


AnswerForm::AnswerForm(QObject* parent) : HttpRequestHandler(parent) {
    // empty
}

void AnswerForm::service(HttpRequest &request, HttpResponse &response) {
    QByteArray user_id=request.getParameter("userid");
    QByteArray answer_data=request.getParameter("answerdata");
    QString answer_point;

    response.setHeader("Content-Type", "text/html; charset=UTF-8");
    response.write("<html><head><title>ANSWER FORM</title></head><body>");

    if (!user_id.isEmpty()) {
        QString filename_answer=AnswerFolderName+user_id+".txt";
        QString t_filename_answer=AnswerFolderName+"Temporary_"+user_id+".txt";

        //save answer to Temporary file
        QFile t_answer(t_filename_answer);
        t_answer.open(QIODevice::WriteOnly);
        t_answer.write(answer_data);
        t_answer.close();

        //responce answer point
        answer_point=SimulateAnswerPoint(t_filename_answer);

        //remove t_answer
        t_answer.remove();

        if(answer_point=="-1"){
            response.write("***Format Error***");
        }else{
            response.write(answer_point.toUtf8());
            //save answer to file
            QFile answer(filename_answer);
            answer.open(QIODevice::WriteOnly);
            answer.write(answer_data);
            answer.close();
            //save userid to file
            QFile userids(AnswerFolderName+"userids.txt");
            userids.open(QIODevice::Append);
            userids.write(user_id);
            userids.write("\n");
            userids.close();
        }

    }else{
        response.write("<form method='POST' action='/answer'>");
        response.write("Answer From:<br>");
        response.write("UserId:  <input type='text' name='userid'><br>");
        response.write("ProblemNumber: <input type='text' name='problemnumber'><br>");
        response.write("Answer: <textarea name='answerdata' cols='100' rows='40'></textarea><br>");
        response.write("<input type='submit'>");
        response.write("</form>");
        response.write("</body></html>",true);
    }
        qDebug("requested answer4");
}

QString AnswerForm::SimulateAnswerPoint(QString filename_answer){

    //copy data
    for(int y=0;y<48;y++){
        for(int x=0;x<48;x++){
            stage_state_[y][x]=g_stage_state_[y][x];
        }
    }
    stone_num_=g_stone_num_;
    for(int n=0;n<stone_num_;n++){
        for(int y=0;y<8;y++){
            for(int x=0;x<8;x++){
                stone_state_[n][y][x]=g_stone_state_[n][y][x];
            }
        }
    }



    //アンサー情報のロード
    QFile questionfile(filename_answer);
    questionfile.open(QIODevice::ReadOnly);
    QString line,line2;
    //ファイル解析
    //フィールド情報を配列に格納,フィールドの更新
    int i=0;
    while(1){
        if(questionfile.atEnd()){
            questionfile.close();
            answer_num_=i;
            break;
        }
        //左右
        line = questionfile.read(1);
        line2 = questionfile.read(1);
        if (line2 != " "){
            line = line + line2;
            line2 = questionfile.read(1);
        }
        answer_flow_[i][0] = line.toInt();
        //上下
        line = questionfile.read(1);
        line2 = questionfile.read(1);
        if (line2 != " "){
            line = line + line2;
            line2 = questionfile.read(1);
        }
        answer_flow_[i][1] = line.toInt();
        //裏表
        line = questionfile.read(1);
        if (line == "H") answer_flow_[i][2] = 0;
        if (line == "T") answer_flow_[i][2] = 1;
        line = questionfile.read(1);//空白分
        //角度
        line = questionfile.readLine(8);
        answer_flow_[i][3] = line.toInt();
        i++;
    }
    stone_flow_count_=0;



    //put a stone on stage
    while(PutStone());



    //Simulate point
    int point=0;
    for (int y = 0; y < 32; y++){
        for (int x = 0; x < 32; x++){
            if (stage_state_[8+y][8+x]==2){
                point+=1;
            }
        }
    }

    //return
    return QString::number(point);
}


bool AnswerForm::PutStone(){
    /*反転させる*/
    if (answer_flow_[stone_flow_count_][2]){
        bool buff[8][8];
        for (int y = 0; y < 8; y++){
            for (int x = 0; x < 8; x++){
                buff[y][x] = stone_state_[stone_flow_count_][y][x];
            }
        }
        for (int y = 0; y < 8; y++){
            for (int x = 0; x < 8; x++){
                stone_state_[stone_flow_count_][y][x] = buff[y][8 - x - 1];
            }
        }
    }
    /*回転させる*/
    if (answer_flow_[stone_flow_count_][3]==90){
        bool buff[8][8];
        for (int y = 0; y < 8; y++){
            for (int x = 0; x < 8; x++){
                buff[y][x] = stone_state_[stone_flow_count_][y][x];
            }
        }
        for (int y = 0; y < 8; y++){
            for (int x = 0; x < 8; x++){
                stone_state_[stone_flow_count_][y][x] = buff[8 - x - 1][y];
            }
        }
    }
    else if (answer_flow_[stone_flow_count_][3] == 180){
        bool buff[8][8];
        for (int y = 0; y < 8; y++){
            for (int x = 0; x < 8; x++){
                buff[y][x] = stone_state_[stone_flow_count_][y][x];
            }
        }
        for (int y = 0; y < 8; y++){
            for (int x = 0; x < 8; x++){
                stone_state_[stone_flow_count_][y][x] = buff[8 - y - 1][8 - x - 1];
            }
        }
    }
    else if(answer_flow_[stone_flow_count_][3] == 270){
        bool buff[8][8];
        for (int y = 0; y < 8; y++){
            for (int x = 0; x < 8; x++){
                buff[y][x] = stone_state_[stone_flow_count_][y][x];
            }
        }
        for (int y = 0; y < 8; y++){
            for (int x = 0; x < 8; x++){
                stone_state_[stone_flow_count_][y][x] = buff[x][8 - y - 1];
            }
        }
    }
    /*設置*/
    //フィールドに置く
    for (int y = 0; y < 8; y++){
        for (int x = 0; x < 8; x++){
            if (stone_state_[stone_flow_count_][y][x]){
                stage_state_[8 + y + answer_flow_[stone_flow_count_][1]][8 + x + answer_flow_[stone_flow_count_][0]]=2;
            }
        }
    }
    /*後処理*/
    stone_flow_count_++;
    if (stone_flow_count_ >= answer_num_){
        return false;
    }
    return true;
}

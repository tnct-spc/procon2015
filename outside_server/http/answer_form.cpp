#include "answer_form.h"
#include <http/request_mapper.h>


AnswerForm::AnswerForm(QObject *parent) : QObject(parent) {
    // empty
}

void AnswerForm::Service(QHttpRequest *request, QHttpResponse *response) {

    //Get GET data
    QUrlQuery url_query(request->url());
    QString plaintext_user_id=url_query.queryItemValue("userid");
    QString plaintext_answer_data=url_query.queryItemValue("answerdata");

    //Decode
    plaintext_user_id.replace("+"," ");
    plaintext_answer_data.replace("+"," ");
    plaintext_answer_data.replace("%0D%0A","\n");
    QByteArray user_id=plaintext_user_id.toUtf8();
    QByteArray answer_data=plaintext_answer_data.toUtf8();
    QString answer_point;

    //response head
    response->setHeader("Content-Type", "text/html; charset=UTF-8");
    response->writeHead(200);
    response->write("<html><head><title>ANSWER FORM</title></head><body>");


    /*Save answer and Display answer_point*/
    if (!user_id.isEmpty()) {
        //responce answer point
        answer_point=SimulateAnswerPoint(plaintext_answer_data);

        if(answer_point=="-1"){
            response->write("FormatError");
        }else{
            response->write(answer_point.toUtf8());
            //save in global
            //すでにあれば更新
            int user_data_itr=-1;
            for(unsigned long i=0;i<g_user_data.size();i++){
                if(user_id==g_user_data[i].userid){
                    g_user_data[i].is_renewal=true;
                    user_data_itr=i;
                }
            }
            //なければ新規作成
            if(user_data_itr==-1){
                user_data_type new_user_data;
                new_user_data.userid=user_id;
                new_user_data.append_stage_number=-1;
                new_user_data.is_renewal=true;
                g_user_data.push_back(new_user_data);
                user_data_itr=g_user_data.size()-1;
            }
            //解答ファイルを入れる
            g_user_data[user_data_itr].answer_point=answer_point.toInt();
            g_user_data[user_data_itr].answer_num=answer_num_;
            for(int i=0;i<answer_num_;i++){
                for(int j=0;j<4;j++){
                    g_user_data[user_data_itr].answer_flow[i][j]=answer_flow_[i][j];
                }
            }
        }

    /*Display Form*/
    }else{
        response->write("<form method='GET' action='/answer'>");
        response->write("Answer From:<br>");
        response->write("UserId:  <input type='text' name='userid'><br>");
        response->write("ProblemNumber: <input type='text' name='problemnumber'><br>");
        response->write("Answer: <textarea name='answerdata' cols='100' rows='40'></textarea><br>");
        response->write("<input type='submit'>");
        response->write("</form>");
        response->write("</body></html>");
    }

    //end
    response->end();
}

QString AnswerForm::SimulateAnswerPoint(QString plaintext_answer_data){

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

    //フィールド情報を配列に格納,フィールドの更新
    QString d=plaintext_answer_data;
    int pos;
    int flow_count=0;
    QStringList list=d.split("\n");
    answer_num_=list.size();
    while(flow_count<answer_num_){
        if(list[flow_count]==""){
            //パスする石
            answer_flow_[flow_count][0]=-1;
            flow_count++;
            continue;
        }
        pos=0;
        //左右
        if (list[flow_count].mid(pos+1,1) == " "){
            answer_flow_[flow_count][0] = list[flow_count].mid(pos,1).toInt();
        }else{
            answer_flow_[flow_count][0] = list[flow_count].mid(pos,2).toInt();
            pos++;
        }
        pos+=2;
        //上下
        if (list[flow_count].mid(pos+1,1) == " "){
            answer_flow_[flow_count][1] = list[flow_count].mid(pos,1).toInt();
        }else{
            answer_flow_[flow_count][1] = list[flow_count].mid(pos,2).toInt();
            pos++;
        }
        pos+=2;
        //裏表
        if (list[flow_count].mid(pos,1) == "H") answer_flow_[flow_count][2] = 0;
        if (list[flow_count].mid(pos,1) == "T") answer_flow_[flow_count][2] = 1;
        pos+=2;
        //角度
        answer_flow_[flow_count][3] = list[flow_count].mid(pos).toInt();
        flow_count++;
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
    while(answer_flow_[stone_flow_count_][0]==-1)stone_flow_count_++;//-1ならパスする
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

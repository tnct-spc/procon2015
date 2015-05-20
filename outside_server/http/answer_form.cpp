#include "answer_form.h"
#include <http/request_mapper.h>
//#define _DEBUG

AnswerForm::AnswerForm(QObject *parent) : QObject(parent) {

}

void AnswerForm::Service(QHttpRequest *request, QHttpResponse *response) {
    new_response_=response;

    if(request->method()==QHttpRequest::HTTP_POST && request->body().isEmpty()){
        //Wait get body
        connect(request,SIGNAL(data(QByteArray)),this,SLOT(ServiceRequestCompleted(QByteArray)));
    }else{
        //Form

        response->setHeader("Content-Type", "text/html; charset=UTF-8");
        response->writeHead(200);
        response->write("<html><head><title>ANSWER FORM</title></head><body>");
        response->write("<form method='POST' action='/answer'>");
        response->write("Answer From:<br>");
        response->write("UserId:  <input type='text' name='id'><br>");
        response->write("ProblemNumber: <input type='text' name='quest_number'><br>");
        response->write("Answer: <textarea name='answer' cols='100' rows='40'></textarea><br>");
        response->write("<input type='submit'>");
        response->write("</form>");
        response->write("</body></html>");

        response->end();
    }
}

void AnswerForm::ServiceRequestCompleted(QByteArray lowdata){
    QHttpResponse *response=new_response_;

    //Get request data
    QUrlQuery url_query(lowdata);
    QString plaintext_user_id=url_query.queryItemValue("id");
    QString plaintext_answer_data=url_query.queryItemValue("answer");

    //Decode request data
    plaintext_user_id.replace("+"," ");
    plaintext_answer_data.replace("+"," ");
    plaintext_answer_data.replace("%0D%0A","\n");
    QByteArray user_id=plaintext_user_id.toUtf8();
    QString answer_point;

    //response head
    response->setHeader("Content-Type", "text/html; charset=UTF-8");
    response->writeHead(200);


    if (user_id.isEmpty()) {
        response->write("error userid is empty.");
    }else{
        if(!FormatCheck(plaintext_answer_data)){
            response->write("FormatError(このフォーマットチェック関数はテストが不十分です.)");
#ifdef _DEBUG
        }
        if(false){
#endif
        }else{
            /*Save answer and Display answer_point*/

            //responce answer point
            answer_point=SimulateAnswerPoint(plaintext_answer_data);
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
                for(int j=0;j<5;j++){
                    g_user_data[user_data_itr].answer_flow[i][j]=answer_flow_[i][j];
                }
            }

            //得点を表示する
            response->write(answer_point.toUtf8());
            //update_flagを立てる
            g_user_data_updated=true;
        }
    }
    response->end();
}

bool AnswerForm::FormatCheck(QString plain_data){

    int pos;
    int flow_count=0;
    int answer_num;
    int answer_flow[256][5];

    //正規表現
    QRegExp re_num("^\\d$");
    //改行で分割
    QStringList list=plain_data.split("\n");
    if(list.size()==1 && list[0]==""){
        answer_num=0;
    }else{
        answer_num=list.size();
    }
    if(answer_num > g_stone_num_){
#ifdef _DEBUG
        qDebug("***format error*** 解答の石数が問題の石数より多い");
#endif
        return false;
    }

    while(flow_count<answer_num){
        if(list[flow_count]==""){
            //パスする石
            answer_flow[flow_count][4]=1;
            flow_count++;
            continue;
        }else{
            answer_flow[flow_count][4]=0;
        }
        pos=0;
        //左右
        bool is_minus=false;
        if (!(re_num.exactMatch(list[flow_count].mid(pos,1)))){
            if(list[flow_count].mid(pos,1)=="-"){
                //add minus
                is_minus=true;
                pos++;
            }else{
#ifdef _DEBUG
        qDebug("***format error*** %d行目のxの書式が違います",flow_count+1);
#endif
                return false;
            }
        }
        if (list[flow_count].mid(pos+1,1) == " "){
            if(is_minus){
                answer_flow[flow_count][0] = -list[flow_count].mid(pos,1).toInt();
            }else{
                answer_flow[flow_count][0] = list[flow_count].mid(pos,1).toInt();
            }
        }else{
            if (!re_num.exactMatch(list[flow_count].mid(pos+1,1))){
#ifdef _DEBUG
        qDebug("***format error*** %d行目のxの書式が違います",flow_count+1);
#endif
                return false;
            }
            if (!(list[flow_count].mid(pos+2,1)==" ")){
#ifdef _DEBUG
        qDebug("***format error*** %d行目のxの書式が違います",flow_count+1);
#endif
                return false;
            }
            if(is_minus){
                answer_flow[flow_count][0] = -list[flow_count].mid(pos,2).toInt();
            }else{
                answer_flow[flow_count][0] = list[flow_count].mid(pos,2).toInt();
            }
            pos++;
        }
        pos+=2;
        //上下
        is_minus=false;
        if (!re_num.exactMatch(list[flow_count].mid(pos,1))){
            if(list[flow_count].mid(pos,1)=="-"){
                //add minus
                is_minus=true;
                pos++;
            }else{
#ifdef _DEBUG
        qDebug("***format error*** %d行目のyの書式が違います",flow_count+1);
#endif
                return false;
            }
        }
        if (list[flow_count].mid(pos+1,1) == " "){
            if(is_minus){
                answer_flow[flow_count][1] = -list[flow_count].mid(pos,1).toInt();
            }else{
                answer_flow[flow_count][1] = list[flow_count].mid(pos,1).toInt();
            }
        }else{
            if (!re_num.exactMatch(list[flow_count].mid(pos+1,1))){
#ifdef _DEBUG
        qDebug("***format error*** %d行目のyの書式が違います",flow_count+1);
#endif
                return false;
            }
            if (!(list[flow_count].mid(pos+2,1)==" ")){
#ifdef _DEBUG
        qDebug("***format error*** %d行目のyの書式が違います",flow_count+1);
#endif
                return false;
            }
            if(is_minus){
                answer_flow[flow_count][1] = -list[flow_count].mid(pos,2).toInt();
            }else{
                answer_flow[flow_count][1] = list[flow_count].mid(pos,2).toInt();
            }
            pos++;
        }
        pos+=2;
        //裏表
        if (list[flow_count].mid(pos,1) == "H"){
            answer_flow[flow_count][2] = 0;
        }else if (list[flow_count].mid(pos,1) == "T"){
            answer_flow[flow_count][2] = 1;
        }else{
#ifdef _DEBUG
        qDebug("***format error*** %d行目のHorTの書式が違います",flow_count+1);
#endif
            return false;
        }
        if (!(list[flow_count].mid(pos+1,1)==" ")){
#ifdef _DEBUG
        qDebug("***format error*** %d行目のHorTの書式が違います",flow_count+1);
#endif
            return false;
        }
        pos+=2;
        //角度
        /*
         * 0
         * 90
         * 180
         * 270
        */
        if(!(list[flow_count].mid(pos)=="0" || list[flow_count].mid(pos)=="90" || list[flow_count].mid(pos)=="180" || list[flow_count].mid(pos)=="270")){
#ifdef _DEBUG
        qDebug("***format error*** %d行目のangleの書式が違います",flow_count+1);
#endif
            return false;
        }
        answer_flow[flow_count][3] = list[flow_count].mid(pos).toInt();
        flow_count++;
    }
    //copy data
    int stage_state[48][48];//0=empty;1=block;2=answer_block
    bool stone_state[256][8][8];
    int stone_num;
    int stone_flow_count=0;

    for(int y=0;y<48;y++){
        for(int x=0;x<48;x++){
            stage_state[y][x]=g_stage_state_[y][x];
        }
    }
    stone_num=g_stone_num_;
    for(int n=0;n<stone_num;n++){
        for(int y=0;y<8;y++){
            for(int x=0;x<8;x++){
                stone_state[n][y][x]=g_stone_state_[n][y][x];
            }
        }
    }
    bool start_append_block=false;//stone has passed, while false.
    while(1){
        while(answer_flow[stone_flow_count][4]==1)stone_flow_count++;//パスする
        //if pass is end of stone flow, finish.
        if (stone_flow_count >= answer_num){
            return true;
        }
        /*反転させる*/
        if (answer_flow[stone_flow_count][2]){
            bool buff[8][8];
            for (int y = 0; y < 8; y++){
                for (int x = 0; x < 8; x++){
                    buff[y][x] = stone_state[stone_flow_count][y][x];
                }
            }
            for (int y = 0; y < 8; y++){
                for (int x = 0; x < 8; x++){
                    stone_state[stone_flow_count][y][x] = buff[y][8 - x - 1];
                }
            }
        }
        /*回転させる*/
        if (answer_flow[stone_flow_count][3]==90){
            bool buff[8][8];
            for (int y = 0; y < 8; y++){
                for (int x = 0; x < 8; x++){
                    buff[y][x] = stone_state[stone_flow_count][y][x];
                }
            }
            for (int y = 0; y < 8; y++){
                for (int x = 0; x < 8; x++){
                    stone_state[stone_flow_count][y][x] = buff[8 - x - 1][y];
                }
            }
        }
        else if (answer_flow[stone_flow_count][3] == 180){
            bool buff[8][8];
            for (int y = 0; y < 8; y++){
                for (int x = 0; x < 8; x++){
                    buff[y][x] = stone_state[stone_flow_count][y][x];
                }
            }
            for (int y = 0; y < 8; y++){
                for (int x = 0; x < 8; x++){
                    stone_state[stone_flow_count][y][x] = buff[8 - y - 1][8 - x - 1];
                }
            }
        }
        else if(answer_flow[stone_flow_count][3] == 270){
            bool buff[8][8];
            for (int y = 0; y < 8; y++){
                for (int x = 0; x < 8; x++){
                    buff[y][x] = stone_state[stone_flow_count][y][x];
                }
            }
            for (int y = 0; y < 8; y++){
                for (int x = 0; x < 8; x++){
                    stone_state[stone_flow_count][y][x] = buff[x][8 - y - 1];
                }
            }
        }
        /*設置*/
        //フィールドに置く
        //フォーマットチェック
        //既存のブロックに接してないとアウト(最初以外)
        bool touch_other_block_flag=false;
        for (int y = 0; y < 8; y++){
            for (int x = 0; x < 8; x++){
                if (stone_state[stone_flow_count][y][x]){
                    //範囲超えたら(はみ出したら)アウト
                    if(y + answer_flow[stone_flow_count][1] >= 32 || y + answer_flow[stone_flow_count][1] < 0){
#ifdef _DEBUG
        qDebug("***format error*** %d個目がフィールドの範囲を越えています",stone_flow_count+1);
#endif
                        return false;
                    }
                    if(x + answer_flow[stone_flow_count][0] >= 32 || x + answer_flow[stone_flow_count][0] < 0){
#ifdef _DEBUG
        qDebug("***format error*** %d個目がフィールドの範囲を越えています",stone_flow_count+1);
#endif
                        return false;
                    }
                    //被ったらアウト
                    if(stage_state[8 + y + answer_flow[stone_flow_count][1]][8 + x + answer_flow[stone_flow_count][0]] > 0){
#ifdef _DEBUG
        qDebug("***format error*** %d個目がフィールドの他の石や障害物と被っています",stone_flow_count+1);
#endif
                        return false;
                    }
                    //既存のブロックに接しているか
                    if(y + answer_flow[stone_flow_count][1] != 31){//下
                        if(stage_state[8 + y + answer_flow[stone_flow_count][1] + 1][8 + x + answer_flow[stone_flow_count][0]]==2) touch_other_block_flag=true;
                    }
                    if(y + answer_flow[stone_flow_count][1] != 0){//上
                        if(stage_state[8 + y + answer_flow[stone_flow_count][1] - 1][8 + x + answer_flow[stone_flow_count][0]]==2) touch_other_block_flag=true;
                    }
                    if(x + answer_flow[stone_flow_count][0] != 31){//右
                        if(stage_state[8 + y + answer_flow[stone_flow_count][1]][8 + x + answer_flow[stone_flow_count][0] + 1]==2) touch_other_block_flag=true;
                    }
                    if(x + answer_flow[stone_flow_count][0] != 0){//左
                        if(stage_state[8 + y + answer_flow[stone_flow_count][1]][8 + x + answer_flow[stone_flow_count][0] - 1]==2) touch_other_block_flag=true;
                    }
                }
            }
        }
        //初めのパス以外で、置く予定のブロックが置いてあるブロックに接していなければダメ
        if(!start_append_block){
            start_append_block=true;
        }else{
            if(!touch_other_block_flag) {
#ifdef _DEBUG
        qDebug("***format error*** %d個目がどの石とも繋がっていません",stone_flow_count+1);
#endif
                return false;
            }
        }
        //ブロックの挿入
        for (int y = 0; y < 8; y++){
            for (int x = 0; x < 8; x++){
                if (stone_state[stone_flow_count][y][x]){
                    //ok
                    stage_state[8 + y + answer_flow[stone_flow_count][1]][8 + x + answer_flow[stone_flow_count][0]]=2;
                }
            }
        }
        /*後処理*/
        stone_flow_count++;
        if (stone_flow_count >= answer_num){
            break;
        }
    }


    return true;
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
    DecodeAnswer(plaintext_answer_data);

    //put a stone on stage
    while(PutStone());

    //Simulate point
    int point=1024/*32*32*/;
    for (int y = 0; y < 32; y++){
        for (int x = 0; x < 32; x++){
            if (stage_state_[8+y][8+x] > 0){
                point-=1;
            }
        }
    }

    //return
    return QString::number(point);
}

void AnswerForm::DecodeAnswer(QString rawdata){
    int pos;
    int flow_count=0;
    QStringList list=rawdata.split("\n");
    if(list.size()==1 && list[0]==""){
        answer_num_=0;
    }else{
        answer_num_=list.size();
    }
    while(flow_count<answer_num_){
        if(list[flow_count]==""){
            //パスする石
            answer_flow_[flow_count][4]=1;
            flow_count++;
            continue;
        }else{
            answer_flow_[flow_count][4]=0;
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
}

bool AnswerForm::PutStone(){
    while(answer_flow_[stone_flow_count_][4]==1)stone_flow_count_++;//パスする
    //if pass is end of stone flow, finish.
    if (stone_flow_count_ >= answer_num_){
        return false;
    }
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
    return true;
}

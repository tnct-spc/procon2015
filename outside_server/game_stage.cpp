#include "game_stage.h"

GameStage::GameStage(QObject *parent) : QObject(parent)
{

}

void GameStage::IntializeStage(QGraphicsScene *field,int base_x,int base_y,int block_size){
    field_=field;
    based_point_={base_x,base_y};
    block_size_=block_size;


    /*display stage*/
    for (int y = 0; y < 32; y++){
        for (int x = 0; x < 32; x++){
            stage_[8 + y][8 + x] = field_-> addRect(x * block_size_ + based_point_.x, y * block_size_ + based_point_.y, block_size_, block_size_);
            stage_[8 + y][8 + x]->setBrush(Qt::white);
            stage_[8 + y][8 + x]->setPen(QPen(QColor(Qt::gray)));
            stage_[8 + y][8 + x]->setZValue(2);
        }
    }


    /*display tag*/
    QFont f;
    f.setPixelSize(38);
    //NameTag
    tag_name_ = field_->addText("",f);
    tag_name_->setX(based_point_.x-40);
    tag_name_->setY(based_point_.y-50);
    tag_name_->setZValue(3);
    //PointTag
    tag_point_ = field_->addText("",f);
    tag_point_->setX(based_point_.x-40+450);
    tag_point_->setY(based_point_.y-50);
    tag_point_->setZValue(3);
    //RankingTag
    f.setPixelSize(150);
    tag_ranking_ = field_->addText("",f);
    tag_ranking_->setX(based_point_.x-40);
    tag_ranking_->setY(based_point_.y-50);
    tag_ranking_->setZValue(3);


    //answer animation timer set
    answer_animation_timer_ = new QTimer();
    QObject::connect(answer_animation_timer_, SIGNAL(timeout()), this, SLOT(AnswerAnimation()));
}

void GameStage::MakeStageData(/*bool stage_state[48][48], bool stone_state[256][8][8], int stone_num*/){
    //Set stage
    for (int y = 0; y < 48; y++){
        for (int x = 0; x < 48; x++){
            stage_state_[y][x]=0;
        }
    }
    for (int y = 0; y < 32; y++){
        for (int x = 0; x < 32; x++){
            stage_state_[y+8][x+8]=g_stage_state_[y+8][x+8];
            if(g_stage_state_[y+8][x+8]){
                stage_[y+8][x+8]->setBrush(Qt::darkBlue);
                stage_[y+8][x+8]->setPen(QPen(QColor(Qt::white)));
                stage_[y+8][x+8]->setZValue(1);
            }else{
                stage_[y+8][x+8]->setBrush(Qt::white);
                stage_[y+8][x+8]->setPen(QPen(QColor(Qt::gray)));
                stage_[y+8][x+8]->setZValue(2);
            }
        }
    }
    //Set stone
    stone_num_=g_stone_num_;
    for (int n = 0; n < g_stone_num_; n++){
        for (int y = 0; y < 8; y++){
            for (int x = 0; x < 8; x++){
                stone_state_[n][y][x]=g_stone_state_[n][y][x];
            }
        }
    }
    //Reset tag
    tag_name_->setPlainText("");
    tag_point_->setPlainText("");
    tag_ranking_->setPlainText("");
}


void GameStage::StartAnswer(int answer_flow[256][5],int answer_num,QString userid,int answer_point){
    //nametag name
    tag_name_->setPlainText(userid);
    //pointtag
    tag_point_->setPlainText("???");
    //copy answer
    answer_point_=answer_point;
    answer_num_=answer_num;
    for (int i = 0; i < answer_num; i++){
        for(int j=0;j<5;j++){
            //qDebug("%s",qPrintable(QString::number(answer_flow[i][j])));
            answer_flow_[i][j]=answer_flow[i][j];
        }
    }
    //解答処理を開始する
    stone_flow_count_=0;
    answer_animation_timer_->start(400);
}

void GameStage::AnswerAnimation(){
    while(answer_flow_[stone_flow_count_][4]==1){
        stone_flow_count_++;//パスする
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
                stage_[8 + y + answer_flow_[stone_flow_count_][1]][8 + x + answer_flow_[stone_flow_count_][0]]->setBrush(Qt::green);
                stage_[8 + y + answer_flow_[stone_flow_count_][1]][8 + x + answer_flow_[stone_flow_count_][0]]->setPen(QPen(QColor(Qt::white)));
                stage_[8 + y + answer_flow_[stone_flow_count_][1]][8 + x + answer_flow_[stone_flow_count_][0]]->setZValue(1);
            }
        }
    }
    //後処理
    stone_flow_count_++;
    if (stone_flow_count_ >= answer_num_){
        //show point
        tag_point_->setPlainText(QString::number(answer_point_));
        //flag update rankingtag
        g_need_rankingtag_updated=true;
        //stop timer
        answer_animation_timer_->stop();
    }
}

void GameStage::update_ranking_tag(int ranking){
    switch(ranking){
    case 1:
        tag_ranking_->setDefaultTextColor(QColor("gold"));
        tag_ranking_->setPlainText("1st");
        break;
    case 2:
        tag_ranking_->setDefaultTextColor(QColor("silver"));
        tag_ranking_->setPlainText("2nd");
        break;
    case 3:
        tag_ranking_->setDefaultTextColor(QColor("blonze"));
        tag_ranking_->setPlainText("3rd");
        break;
    case 4:
        tag_ranking_->setDefaultTextColor(QColor("pink"));
        tag_ranking_->setPlainText("4th");
        break;
    case 5:
        tag_ranking_->setDefaultTextColor(QColor("pink"));
        tag_ranking_->setPlainText("5th");
        break;
    case 6:
        tag_ranking_->setDefaultTextColor(QColor("pink"));
        tag_ranking_->setPlainText("6th");
        break;
    default:
        qDebug("***error*** in GameStage::update_ranking_tag, ranking_str(int) value is invalid");
        break;
    }
}

#include "outside_server.h"
#include "ui_outside_server.h"

//コンストラクタ
OutsideServer::OutsideServer(QWidget *parent) : QWidget(parent),ui(new Ui::procon26server)
{
    //ui
    ui->setupUi(this);
    mainstage = new QGraphicsScene(0, 0, 1719, 1079);
    ui->graphicsView->setScene(mainstage);

    //UMA
    uma.StartUp(mainstage,0.2);

    //Answer reserve interval timer set
    QTimer *reserve_answer_timer;
    reserve_answer_timer = new QTimer();
    connect(reserve_answer_timer, SIGNAL(timeout()), this, SLOT(ReserveAnswer()));
    connect(ui->horizontalSlider,SIGNAL(valueChanged(int)),this,SLOT(viewzoom(int)));
    connect(ui->verticalSlider,SIGNAL(valueChanged(int)),this,SLOT(viewresize(int)));
    reserve_answer_timer->start(1/*interval 1*/);

    //intialize
    problem_flag=false;
    problem_number_spin_button_value_="1";
    for (int i=0;i<6;i++){
        answer_points_[i]=-1;
    }
    for (int i=0;i<6;i++){
        answer_players_[i]="";
    }
    for (int y = 0; y < 48; y++){
        for (int x = 0; x < 48; x++){
            g_stage_state_[y][x]=0;
        }
    }
    int block_size=15;
    for (int h=0;h<2;h++){
        for (int w=0;w<3;w++){
            game_stage_[h*3+w].IntializeStage(mainstage,40+w*32*block_size+w*100,50+h*32*block_size+h*50,block_size);
        }
    }
}

//デストラクタ
OutsideServer::~OutsideServer()
{
    //Reset problem
    ResetFolder(ProblemFolderName);
    delete ui;
}

void OutsideServer::ReserveAnswer(){

    if (problem_flag){
        QFile questionfile(AnswerFolderName+"userids.txt");
        QString userid;
        int answer_flow[256][4];

        //解答が送られてきたか確認(get userid for userids.txt)
        if(QFile::exists(AnswerFolderName+"userids.txt")){
            questionfile.open(QIODevice::ReadOnly);
            while(1){
                userid = questionfile.read(256);
                userid = userid.trimmed();
                if(userid.isEmpty()) break;


                /*Convert Answer(txt file to int.answer_flow)*/
                int answer_num=ConvertAnswer(AnswerFolderName+userid+".txt",answer_flow);

                /*Allocation*/
                int set_pos=-1;
                int answer_point=pointer.SimulateAnswerPoint(AnswerFolderName+userid+".txt").toInt();

                if(set_pos==-1){
                    //同じ名前があればそれに更新
                    for(int i=0;i<6;i++){
                        if(userid==answer_players_[i]){
                            set_pos=i;
                            break;
                        }
                    }
                }
                if(set_pos==-1){
                    //何も入っていないものから埋める
                    for(int i=0;i<6;i++){
                        if(answer_points_[i]==-1){
                            set_pos=i;
                            break;
                        }
                    }
                }
                if(set_pos==-1){
                    //得点が小さい順にソート
                    int sort_point[6]={0,1,2,3,4,5};
                    int buff;
                    //bubble sort
                    for(int i=0;i<6;i++){
                        for(int j=1;j<6;j++){
                            if(answer_points_[sort_point[j-1]] > answer_points_[sort_point[j]]){
                                buff=sort_point[j];
                                sort_point[j]=sort_point[j-1];
                                sort_point[j-1]=buff;
                            }
                        }
                    }
                    if(answer_point > answer_points_[sort_point[0]]){//表示されている人の中で特典が一番小さい人より,特典が大きければそのステージを更新
                        set_pos=sort_point[0];
                    }
                }
                if(set_pos==-1){
                    //表示する価値なし
                }
                if(set_pos!=-1){
                    //submit
                    game_stage_[set_pos].MakeStageData(/*g_stage_state_,g_stone_state_,g_stone_num_*/);
                    game_stage_[set_pos].StartAnswer(answer_flow,answer_num,userid);
                    answer_points_[set_pos]=answer_point;
                    answer_players_[set_pos]=userid;
                }
            }
            questionfile.close();
            questionfile.remove();
        }
    }
}

void OutsideServer::uisizebutton_clicked(){
    static bool toggleuisize=1;
    if(toggleuisize==1){
        toggleuisize=0;
        this->showNormal();
        this->showMaximized();
    }else{
        toggleuisize=1;
        this->showFullScreen();
    }
}

void OutsideServer::pnspinbutton_changed(QString num){
    problem_number_spin_button_value_=num;
}

/*フィールド情報のロード*/
void OutsideServer::loadbutton_clicked()
{
    QFileDialog::Options options;
    QString strSelectedFilter;

    //問題ファイルを開く
    QString problem_file_name = QFileDialog::getOpenFileName(this, "", "./etc/", "*.txt",&strSelectedFilter, options);

    if (!problem_file_name.isEmpty())
    {
        problem_flag = true;

        QFile problem_file(problem_file_name);
        problem_file.open(QIODevice::ReadOnly);
        QString line;
        //フィールド情報を配列に格納,フィールドの更新
        for (int y = 0; y < 32; y++){
            for (int x = 0; x < 32; x++){
                line = problem_file.read(1);
                //フィールドに置く
                bool state=line.toInt();
                g_stage_state_[y+8][x+8]=state;
            }
            problem_file.read(2);//改行分
        }
        problem_file.read(2);//改行分
        //ストーンの個数を取得

        g_stone_num_ = problem_file.read(1).toInt();
        //それぞれのストーンを取得
        for (int n = 0; n < g_stone_num_; n++){
            problem_file.read(2);//改行分
            for (int y = 0; y < 8; y++){
                for (int x = 0; x < 8; x++){
                    line = problem_file.read(1);
                    if (line == "0"){
                        //置かれていない
                        g_stone_state_[n][y][x] = false;
                    }
                    else if (line == "1"){
                        //置かれている
                        g_stone_state_[n][y][x] = true;
                    }
                }
                problem_file.read(2);//改行分
            }
        }
        problem_file.close();


        //make all gamestage data
        for(int m=0;m<6;m++){
            game_stage_[m].MakeStageData(/*g_stage_state_,g_stone_state_,g_stone_num_*/);
        }
        //Reset answer
        ResetFolder(AnswerFolderName);
        //Copy file to problemfolder (Name is "problem[problem_number_spin_button_value_].txt")
        QFile::copy(problem_file_name,ProblemFolderName+"problem"+problem_number_spin_button_value_+".txt");
    }
}

/*アンサー情報のロード*/
int OutsideServer::ConvertAnswer(QString filename_answer,int answer_flow[256][4]){

    QFile answer_file(filename_answer);
    answer_file.open(QIODevice::ReadOnly);
    QString line,line2;

    //ファイル解析
    //フィールド情報を配列に格納,フィールドの更新
    int i=0;
    while(1){
        if(answer_file.atEnd()){
            answer_file.close();
            return i;
        }
        //左右
        line = answer_file.read(1);
        line2 = answer_file.read(1);
        if (line2 != " "){
            line = line + line2;
            line2 = answer_file.read(1);
        }
        answer_flow[i][0] = line.toInt();
        //上下
        line = answer_file.read(1);
        line2 = answer_file.read(1);
        if (line2 != " "){
            line = line + line2;
            line2 = answer_file.read(1);
        }
        answer_flow[i][1] = line.toInt();
        //裏表
        line = answer_file.read(1);
        if (line == "H") answer_flow[i][2] = 0;
        if (line == "T") answer_flow[i][2] = 1;
        line = answer_file.read(1);//空白分
        //角度
        line = answer_file.readLine(8);
        answer_flow[i][3] = line.toInt();
        i++;
    }
}

bool OutsideServer::ResetFolder(const QString &dir_name)
{
    bool result = true;
    QDir dir(dir_name);

    if (dir.exists(dir_name)) {
        Q_FOREACH(QFileInfo info, dir.entryInfoList(QDir::NoDotAndDotDot | QDir::System | QDir::Hidden  | QDir::AllDirs | QDir::Files, QDir::DirsFirst)) {
            if (info.isDir()) {
                result = ResetFolder(info.absoluteFilePath());
            }
            else {
                result = QFile::remove(info.absoluteFilePath());
            }

            if (!result) {
                return result;
            }
        }
        result = true;
    }
    return result;
}

void OutsideServer::viewzoom(int value)
{
    //qreal scale = qPow(qreal(2), (value - 250) / qreal(50));

    QMatrix matrix;
    matrix.scale((double)value / 100.0, (double)value / 100.0);
    //graphicsView->setMatrix(matrix);
    ui->graphicsView->setMatrix(matrix);
}
void OutsideServer::viewresize(int value)
{
    ui->graphicsView->resize(value * 8 ,value * 8);
}

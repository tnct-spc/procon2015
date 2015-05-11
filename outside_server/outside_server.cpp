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

//1ミリ秒間隔で画面に表示する解答の更新を行う
void OutsideServer::ReserveAnswer(){

    if (problem_flag){
        if(g_need_rankingtag_updated || g_user_data_updated){
            g_user_data_updated=false;
            //user_sortを介してg_user_dataを得点が高い順にソート
            std::vector<int> user_sort;
            for(unsigned long i=0;i<g_user_data.size();i++){
                user_sort.push_back(i);
            }
            int buff;
            //bubble sort
            for(unsigned long i=0;i<g_user_data.size();i++){
                for(unsigned long j=1;j<g_user_data.size();j++){
                    if(g_user_data[user_sort[j-1]].answer_point < g_user_data[user_sort[j]].answer_point){
                        buff=user_sort[j];
                        user_sort[j]=user_sort[j-1];
                        user_sort[j-1]=buff;
                    }
                }
            }
            //得点が高い順に最大6つ,ステージに配置していく
            unsigned long stage_max;
            if(g_user_data.size()<6){
                stage_max=g_user_data.size();
            }else{
                stage_max=6;
            }
            for(unsigned long i=0;i<stage_max;i++){
                if(g_user_data[user_sort[i]].append_stage_number >= 0){
                    //すでにステージにある
                    if(g_user_data[user_sort[i]].is_renewal){
                        //[1]更新されているので書き換える
                        game_stage_[g_user_data[user_sort[i]].append_stage_number].MakeStageData();
                        game_stage_[g_user_data[user_sort[i]].append_stage_number].StartAnswer(g_user_data[user_sort[i]].answer_flow,g_user_data[user_sort[i]].answer_num,g_user_data[user_sort[i]].userid);
                        if(g_need_rankingtag_updated) game_stage_[g_user_data[user_sort[i]].append_stage_number].update_ranking_tag(i+1/*Ranking*/);
                    }else{
                        //[2]もとのデータのままなので順位を更新して飛ばす
                        if(g_need_rankingtag_updated) game_stage_[g_user_data[user_sort[i]].append_stage_number].update_ranking_tag(i+1/*Ranking*/);
                        continue;
                    }
                }else{
                    //[3]ステージに無いので空いている場所又はステージにある解答の中で一番得点が小さいものと交換する
                    int append_minimum_stage_num;
                    if(g_user_data.size()<=6){
                        append_minimum_stage_num=g_user_data.size()-1;
                    }else{
                        for(unsigned long j=g_user_data.size();j>0;j--){
                            if(g_user_data[user_sort[j-1]].append_stage_number > 0){
                                append_minimum_stage_num=g_user_data[user_sort[j-1]].append_stage_number;
                                g_user_data[user_sort[j-1]].append_stage_number=-1;
                                break;
                            }
                        }
                    }
                    //書き換える
                    g_user_data[user_sort[i]].append_stage_number=append_minimum_stage_num;
                    game_stage_[append_minimum_stage_num].MakeStageData();
                    game_stage_[append_minimum_stage_num].StartAnswer(g_user_data[user_sort[i]].answer_flow,g_user_data[user_sort[i]].answer_num,g_user_data[user_sort[i]].userid);
                    if(g_need_rankingtag_updated) game_stage_[append_minimum_stage_num].update_ranking_tag(i+1/*Ranking*/);
                }
            }
            //すべての更新フラグ(renewal)をfalseに
            for(unsigned long i=0;i<g_user_data.size();i++) g_user_data[i].is_renewal=false;
            g_need_rankingtag_updated=false;
        }
    }
}

void OutsideServer::uisizebutton_clicked(){
    static bool toggleuisize=1;
    if(toggleuisize==1){
        toggleuisize=0;
        this->showNormal();
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

void OutsideServer::resizeEvent(QResizeEvent *event)
{
    (void)event;
    QMatrix matrix;
    if((double)(width()-200)/(double)height()-1720.0/1080.0>0){
        matrix.scale((double)height() / 1080.0, (double)height() / 1080.0);
    }else{
        matrix.scale((double)(width()-200) / 1720.0, (double)(width()-200) / 1720.0);
    }
    ui->graphicsView->setMatrix(matrix);
    ui->graphicsView->resize(width()-200 ,height());
}

#include "outside_server.h"
#include "ui_outside_server.h"

//コンストラクタ
OutsideServer::OutsideServer(QWidget *parent) : QWidget(parent),ui(new Ui::procon26server)
{
    //ui
    ui->setupUi(this);
    mainstage = new QGraphicsScene(0, 0, 1720, 1080);
    ui->graphicsView->setScene(mainstage);

    //UMA
    uma.StartUp(mainstage,0.2);

    //Set answer reserve interval timer
    QTimer *reserve_answer_timer;
    reserve_answer_timer = new QTimer();
    connect(reserve_answer_timer, SIGNAL(timeout()), this, SLOT(ReserveAnswer()));
    reserve_answer_timer->start(1/*interval 1ms*/);

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

//呼ばれるごとに画面に表示する解答の更新を行う
void OutsideServer::ReserveAnswer(){

    if (problem_flag){
        //TODO: need_mutex
        if(g_need_rankingtag_updated || g_user_data_updated){
            //user_sortを介してg_user_dataを得点が高い順にソート
            std::vector<int> user_sort;
            for(unsigned long i=0;i<g_user_data.size();i++){
                user_sort.push_back(i);
            }
            int buff;
            //bubble sort
            for(unsigned long i=0;i<g_user_data.size();i++){
                for(unsigned long j=1;j<g_user_data.size()-i;j++){
                    if(g_user_data[user_sort[j-1]].answer_putstone_num > g_user_data[user_sort[j]].answer_putstone_num){
                        buff=user_sort[j];
                        user_sort[j]=user_sort[j-1];
                        user_sort[j-1]=buff;
                    }
                }
            }
            for(unsigned long i=0;i<g_user_data.size();i++){
                for(unsigned long j=1;j<g_user_data.size()-i;j++){
                    if(g_user_data[user_sort[j-1]].answer_point > g_user_data[user_sort[j]].answer_point){
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
            //set ranking
            int ranking_count=1;
            std::vector<int> user_ranking(g_user_data.size());
            for(unsigned long i=0;i<stage_max;i++){
                if(!(g_user_data[user_sort[i]].is_now_animation)){
                    user_ranking[user_sort[i]]=ranking_count;
                    ranking_count++;
                }
            }
            //set
            for(unsigned long i=0;i<stage_max;i++){
                if(g_user_data[user_sort[i]].append_stage_number >= 0){
                    //すでにステージにある
                    if(g_user_data[user_sort[i]].is_renewal){
                        //[1]更新されているので書き換える
                        game_stage_[g_user_data[user_sort[i]].append_stage_number].MakeStageData(user_sort[i]);
                        game_stage_[g_user_data[user_sort[i]].append_stage_number].StartAnswer(g_user_data[user_sort[i]].answer_flow,g_user_data[user_sort[i]].answer_num,g_user_data[user_sort[i]].userid,g_user_data[user_sort[i]].answer_point);
                        if(g_need_rankingtag_updated && g_user_data[user_sort[i]].is_now_animation==false) game_stage_[g_user_data[user_sort[i]].append_stage_number].update_ranking_tag(user_ranking[user_sort[i]]);
                        g_user_data[user_sort[i]].is_now_animation=true;
                    }else{
                        //[2]もとのデータのままなので順位を更新して飛ばす
                        if(g_need_rankingtag_updated && g_user_data[user_sort[i]].is_now_animation==false) game_stage_[g_user_data[user_sort[i]].append_stage_number].update_ranking_tag(user_ranking[user_sort[i]]);
                        continue;
                    }
                }else{
                    //[3]ステージに無いので空いている場所又はステージにある解答の中で一番得点が小さいものと交換する
                    int append_minimum_stage_num;
                    if(g_user_data.size()<=6){
                        append_minimum_stage_num=g_user_data.size()-1;
                    }else{
                        for(unsigned long j=g_user_data.size();j>0;j--){
                            if(g_user_data[user_sort[j-1]].append_stage_number >= 0){
                                append_minimum_stage_num=g_user_data[user_sort[j-1]].append_stage_number;
                                g_user_data[user_sort[j-1]].append_stage_number=-1;
                                break;
                            }
                        }
                    }
                    //書き換える
                    g_user_data[user_sort[i]].append_stage_number=append_minimum_stage_num;
                    game_stage_[append_minimum_stage_num].MakeStageData(user_sort[i]);
                    game_stage_[append_minimum_stage_num].StartAnswer(g_user_data[user_sort[i]].answer_flow,g_user_data[user_sort[i]].answer_num,g_user_data[user_sort[i]].userid,g_user_data[user_sort[i]].answer_point);
                    if(g_need_rankingtag_updated && g_user_data[user_sort[i]].is_now_animation==false) game_stage_[append_minimum_stage_num].update_ranking_tag(user_ranking[user_sort[i]]);
                    g_user_data[user_sort[i]].is_now_animation=true;
                }
            }
            //すべての更新フラグをfalseに
            for(unsigned long i=0;i<g_user_data.size();i++) g_user_data[i].is_renewal=false;
            g_user_data_updated=false;
            g_need_rankingtag_updated=false;
        }
    }
}

void OutsideServer::uisizebutton_clicked(){
    static bool toggleuisize=0;
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
    QString problem_file_name = QFileDialog::getOpenFileName(this, "", "./", "*.txt",&strSelectedFilter, options);

    if (!problem_file_name.isEmpty())
    {
        //TODO: need mutex

        //stop animation
        for(int i=0;i<6;i++){
            game_stage_[i].stop_animation_timer();
        }

        //Reset problem_folder
        ResetFolder(ProblemFolderName);

        //get problem
        Decode_problem_and_set(problem_file_name);

        //make all gamestage data
        for(int m=0;m<6;m++){
            game_stage_[m].MakeStageData(m/*,g_stage_state_,g_stone_state_,g_stone_num_*/);
        }
        //Reset answer
        g_user_data.clear();

        //Copy file to problemfolder (Name is "problem[problem_number_spin_button_value_].txt")
        QFile::copy(problem_file_name,ProblemFolderName+"problem"+problem_number_spin_button_value_+".txt");

        //unlock
        g_problem_number=problem_number_spin_button_value_.toInt();
        problem_flag = true;
    }
}

void OutsideServer::Decode_problem_and_set(QString problem_file_name){
    QFile rawdata(problem_file_name);
    rawdata.open(QIODevice::ReadOnly);

    QString line;
    //フィールド情報を配列に格納,フィールドの更新
    for (int y = 0; y < 32; y++){
        for (int x = 0; x < 32; x++){
            line = rawdata.read(1);
            //フィールドに置く
            bool state=line.toInt();
            g_stage_state_[y+8][x+8]=state;
        }
        rawdata.read(2);//改行分
    }
    rawdata.read(2);//改行分
    //ストーンの個数を取得
    QString s1,s2,s3;
    s1 = rawdata.read(1);
    s2 = rawdata.read(1);
    s3 = rawdata.read(1);
    if(s3=="0" || s3=="1" || s3=="2" || s3=="3" || s3=="4" || s3=="5" || s3=="6" || s3=="7" || s3=="8" || s3=="9"){
        //3keta
        g_stone_num_=s1.toInt()*100 + s2.toInt()*10 + s3.toInt();
        rawdata.read(2);//改行分
    }else if(s2=="0" || s2=="1" || s2=="2" || s2=="3" || s2=="4" || s2=="5" || s2=="6" || s2=="7" || s2=="8" || s2=="9"){
        //2keta
        g_stone_num_=s1.toInt()*10 + s2.toInt();
        rawdata.read(1);
    }else{
        //1keta
        g_stone_num_=s1.toInt();
    }
    //それぞれのストーンを取得
    for (int n = 0; n < g_stone_num_; n++){
        if(n!=0) rawdata.read(2);//改行分
        for (int y = 0; y < 8; y++){
            for (int x = 0; x < 8; x++){
                line = rawdata.read(1);
                if (line == "0"){
                    //置かれていない
                    g_stone_state_[n][y][x] = false;
                }
                else if (line == "1"){
                    //置かれている
                    g_stone_state_[n][y][x] = true;
                }
            }
            rawdata.read(2);//改行分
        }
    }
    rawdata.close();
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

void OutsideServer::push_go()
{
    game_stage_[0].GoAnswer();
}

void OutsideServer::push_back()
{
    game_stage_[0].BackAnswer();
}

void OutsideServer::push_stop()
{
    game_stage_[0].stop_animation_timer();
}

void OutsideServer::push_restart()
{
    game_stage_[0].restart_animation_timer();
}

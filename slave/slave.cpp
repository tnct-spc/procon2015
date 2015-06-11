#include "slave.h"
#include "ui_slave.h"
#include <QUrl>
#include <string>
#include "takao.hpp"
#include "muen_zuka.hpp"
#include "algorithm_manager.hpp"
#include <iostream>
//#define _DEBUG
#ifdef _DEBUG
    #include <QDebug>
    #include <iostream>
#endif

Slave::Slave(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::Slave)
{
    ui->setupUi(this);
    connect(ui->Clear_button,&QPushButton::clicked,this,&Slave::text_box_clear);
}

Slave::~Slave()
{
    delete ui;
}

void Slave::clicked_run_button(){

    //get problem
    //net network(gethost,posthost,"testman",1);
    network = new net(ui->get_line_edit->text(),
                      ui->post_line_edit->text(),
                      ui->prob_num_line_edit->text().toInt()
                      );
    auto str = network->get();
    if(network->is_error()){
        //ui->net_label->setText(QString("エラーコード ") + QString().setNum(network->what_error()));
        ui->textBrowser->setPlainText( ui->textBrowser->toPlainText() + QString("ネットワークエラーコード ") + QString().setNum(network->what_error()) + QString("\n"));
        return;
    }
    //ui->net_label->setText("ネットは生きてる");
    ui->textBrowser->setPlainText( ui->textBrowser->toPlainText() + QString("問題を受信しました\n"));
    problem_type problem(str);

    //solve
    algo_manager = new algorithm_manager(problem);
    connect(algo_manager,&algorithm_manager::answer_ready,this,&Slave::answer_send);
    //connect(algo_manager,&algorithm_manager::finished,[&](){delete algo_manager;std::cout << "manager殺した" << std::endl;});
    algo_manager->run();

}
void Slave::answer_send(field_type answer){
    //std::cout<<"answer=\n\""<<answer.get_answer()<<"\""<<std::endl;
    net_mtx.lock();
    std::string res = network->send(answer);
    ui->textBrowser->setPlainText( ui->textBrowser->toPlainText() + QString("回答を送信しました\n"));
    ui->textBrowser->setPlainText( ui->textBrowser->toPlainText() + QString("動作中アルゴリズム数 ") + QString().setNum(algo_manager->run_thread_num()) + QString("\n"));
    ui->textBrowser->setPlainText( ui->textBrowser->toPlainText() + QString(res.c_str()) + QString("\n"));
    net_mtx.unlock();
}
void Slave::text_box_clear(){
    ui->textBrowser->setPlainText("");
}

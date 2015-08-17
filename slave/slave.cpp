#include "slave.h"
#include "ui_slave.h"
#include <QUrl>
#include <string>
#include <QFileDialog>
#include <QFile>
#include "takao.hpp"
#include "muen_zuka.hpp"
#include "algorithm_manager.hpp"
#include <iostream>
#include <fstream>
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
    connect(ui->answer_save_action,&QAction::triggered,this,&Slave::answer_save_to_file);
    connect(ui->problem_load_action,&QAction::triggered,this,&Slave::problem_load_from_file);

    //setting load
    settings = new QSettings("setting.ini",QSettings::IniFormat);
    settings->beginGroup("SETTING");
    if(!QFile::exists("setting.ini")){
        //init
        settings->setValue("POST_BUTTON",1);
        settings->setValue("GET_BUTTON",1);
        settings->setValue("POST_LINE_EDIT_1","http://127.0.0.1:8081");
        settings->setValue("POST_LINE_EDIT_2","http://127.0.0.1:8081");
        settings->setValue("GET_LINE_EDIT_1","http://127.0.0.1:8080/problem");
        settings->setValue("GET_LINE_EDIT_2","http://127.0.0.1:8080/problem");
        settings->setValue("GET_LINE_EDIT_3","http://127.0.0.1:8080/problem");
    }
    switch(settings->value("POST_BUTTON").toInt()){
    case 1:
        ui->post_button_1->setChecked(true);
        break;
    case 2:
        ui->post_button_2->setChecked(true);
        break;
    }
    switch(settings->value("GET_BUTTON").toInt()){
    case 1:
        ui->get_button_1->setChecked(true);
        break;
    case 2:
        ui->get_button_2->setChecked(true);
        break;
    case 3:
        ui->get_button_3->setChecked(true);
        break;
    }
    ui->post_line_edit_1->setText(settings->value("POST_LINE_EDIT_1").toString());
    ui->post_line_edit_2->setText(settings->value("POST_LINE_EDIT_2").toString());
    ui->get_line_edit_1->setText(settings->value("GET_LINE_EDIT_1").toString());
    ui->get_line_edit_2->setText(settings->value("GET_LINE_EDIT_2").toString());
    ui->get_line_edit_3->setText(settings->value("GET_LINE_EDIT_3").toString());
}

Slave::~Slave()
{
    delete ui;
}

void Slave::clicked_run_button(){

    //get problem
    //net network(gethost,posthost,"testman",1);
    network = new net(get_geturl(),
                      get_posturl(),
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
    if(ui->discharge->isChecked()){
        std::ofstream fp("../greatest_answer.txt");
        fp<<answer.get_answer();
        fp.close();
        ui->textBrowser->setPlainText( ui->textBrowser->toPlainText() + QString("回答をoutputしました\n"));
        ui->textBrowser->setPlainText( ui->textBrowser->toPlainText() + QString("動作中アルゴリズム数 ") + QString().setNum(algo_manager->run_thread_num()) + QString("\n"));

    }else{
        net_mtx.lock();
        std::string res = network->send(answer);
        ui->textBrowser->setPlainText( ui->textBrowser->toPlainText() + QString("回答を送信しました\n"));
        ui->textBrowser->setPlainText( ui->textBrowser->toPlainText() + QString("動作中アルゴリズム数 ") + QString().setNum(algo_manager->run_thread_num()) + QString("\n"));
        ui->textBrowser->setPlainText( ui->textBrowser->toPlainText() + QString(res.c_str()) + QString("\n"));
        net_mtx.unlock();
    }
}
void Slave::text_box_clear(){
    ui->textBrowser->setPlainText("");
}
void Slave::answer_save_to_file(){
    auto filename = QFileDialog::getOpenFileName(this);
    if(filename.isEmpty())return;
    QFile file(filename);
    if(!file.open(QIODevice::WriteOnly))return;
    QTextStream out(&file);
    out << _answer.field.get_answer().c_str() << endl;
}
void Slave::problem_load_from_file(){
    auto filename = QFileDialog::getOpenFileName(this);
    if(filename.isEmpty())return;
    QFile file(filename);
    if(!file.open(QIODevice::ReadOnly))return;
    QTextStream in(&file);
    _problem = problem_type(in.readAll().toStdString());
    //std::cout << _problem.stones.size() << std::endl;
}


QString Slave::get_posturl(){
    if(ui->post_button_1->isChecked()) return ui->post_line_edit_1->text();
    if(ui->post_button_2->isChecked()) return ui->post_line_edit_2->text();
    return "";
}

QString Slave::get_geturl(){
    if(ui->get_button_1->isChecked()) return ui->get_line_edit_1->text();
    if(ui->get_button_2->isChecked()) return ui->get_line_edit_2->text();
    if(ui->get_button_3->isChecked()) return ui->get_line_edit_3->text();
    return "";
}

void Slave::post_button_1_pushed(){settings->setValue("POST_BUTTON",1);}
void Slave::post_button_2_pushed(){settings->setValue("POST_BUTTON",2);}
void Slave::get_button_1_pushed(){settings->setValue("GET_BUTTON",1);}
void Slave::get_button_2_pushed(){settings->setValue("GET_BUTTON",2);}
void Slave::get_button_3_pushed(){settings->setValue("GET_BUTTON",3);}
void Slave::post_line_edit_1_changed(){settings->setValue("POST_LINE_EDIT_1",ui->post_line_edit_1->text());}
void Slave::post_line_edit_2_changed(){settings->setValue("POST_LINE_EDIT_2",ui->post_line_edit_2->text());}
void Slave::get_line_edit_1_changed(){settings->setValue("GET_LINE_EDIT_1",ui->get_line_edit_1->text());}
void Slave::get_line_edit_2_changed(){settings->setValue("GET_LINE_EDIT_2",ui->get_line_edit_2->text());}
void Slave::get_line_edit_3_changed(){settings->setValue("GET_LINE_EDIT_3",ui->get_line_edit_3->text());}

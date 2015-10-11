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
    srand((unsigned int)time(NULL));
    ui->setupUi(this);
    connect(ui->Clear_button,&QPushButton::clicked,this,&Slave::text_box_clear);
    connect(ui->answer_save_action,&QAction::triggered,this,&Slave::answer_save_to_file);
    connect(ui->problem_load_action,&QAction::triggered,this,&Slave::problem_load_from_file);
    //setting load
    settings = new QSettings("setting.ini",QSettings::IniFormat);
    settings->beginGroup("SETTING");
    if(!QFile::exists("setting.ini")){
        //init
        settings->setValue("PLAYERID","名無し-"+QString::number(rand()%10000));
        settings->setValue("POST_BUTTON",1);
        settings->setValue("GET_BUTTON",1);
        settings->setValue("POST_LINE_EDIT_1","http://127.0.0.1:8080/answer");
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
    ui->playerid_lineEdit->setText(settings->value("PLAYERID").toString());
    ui->post_line_edit_1->setText(settings->value("POST_LINE_EDIT_1").toString());
    ui->post_line_edit_2->setText(settings->value("POST_LINE_EDIT_2").toString());
    ui->get_line_edit_1->setText(settings->value("GET_LINE_EDIT_1").toString());
    ui->get_line_edit_2->setText(settings->value("GET_LINE_EDIT_2").toString());
    ui->get_line_edit_3->setText(settings->value("GET_LINE_EDIT_3").toString());
    br = ui->textBrowser->verticalScrollBar();
}

Slave::~Slave()
{
    delete ui;
}

void Slave::clicked_run_button(){
    if(!ui->ofline_mode_check_box->isChecked()){
        network = new net(get_geturl(),
                          get_posturl(),
                          settings->value("PLAYERID").toString(),
                          ui->prob_num_line_edit->text().toInt()
                          );
        std::string str;
        if(ui->checkBoxofRaciveOfiicialServer->isChecked()){
            str = network->get_from_official_server();
        }else{
            str = network->get();
        }
        if(network->is_error()){
            print_text(QString("🍣ネットワークエラーコード ") + QString::number(network->what_error()));
            return;
        }
        print_text("問題を受信しました");
        problem_type problem(str);
        _problem = problem;
    }
    //solve
    std::vector<bool> enable_algo(10);
    ui->checkBox_0->isChecked() ? enable_algo.at(0) = true : enable_algo.at(0) = false;
    ui->checkBox_1->isChecked() ? enable_algo.at(1) = true : enable_algo.at(1) = false;
    ui->checkBox_2->isChecked() ? enable_algo.at(2) = true : enable_algo.at(2) = false;
    ui->checkBox_3->isChecked() ? enable_algo.at(3) = true : enable_algo.at(3) = false;
    ui->checkBox_4->isChecked() ? enable_algo.at(4) = true : enable_algo.at(4) = false;
    ui->checkBox_5->isChecked() ? enable_algo.at(5) = true : enable_algo.at(5) = false;
    ui->checkBox_6->isChecked() ? enable_algo.at(6) = true : enable_algo.at(6) = false;
    ui->checkBox_7->isChecked() ? enable_algo.at(7) = true : enable_algo.at(7) = false;
    ui->checkBox_8->isChecked() ? enable_algo.at(8) = true : enable_algo.at(8) = false;
    ui->checkBox_9->isChecked() ? enable_algo.at(9) = true : enable_algo.at(9) = false;
    int time_limit = 1000 * (ui->limit_m->text().toInt() * 60 + ui->limit_s->text().toInt());
    algo_manager = new algorithm_manager(_problem,enable_algo,time_limit);
    algo_manager->setParent(this);
    connect(algo_manager,&algorithm_manager::answer_ready,this,&Slave::answer_send);
    connect(algo_manager,&algorithm_manager::answer_ready,this,&Slave::answer_auto_save_to_file);
    connect(algo_manager,&algorithm_manager::send_text,this,&Slave::print_algorithm_message);
    connect(algo_manager,&algorithm_manager::destroyed,[=](){std::cout << "manager殺した" << std::endl;});
    connect(algo_manager,&algorithm_manager::finished,&algorithm_manager::deleteLater);
    algo_manager->run();

}
void Slave::answer_send(field_type answer){
    _answer = answer;
    if(ui->ofline_mode_check_box->isChecked()){
        print_text(QString("動作中アルゴリズム数 ") + QString::number(algo_manager->run_thread_num()));

    }else{
        net_mtx.lock();
        std::string res;
        if(ui->checkBoxofSendOfiicialServer->isChecked()){
            res = network->send_to_official_server(answer);
        }else{
            res = network->send(answer);
        }
        print_text(QString(res.c_str()));
        print_text("回答を送信しました");
        print_text(QString("動作中アルゴリズム数 ") + QString::number(algo_manager->run_thread_num()));

        net_mtx.unlock();
    }
}
void Slave::text_box_clear(){
    ui->textBrowser->setPlainText("");
}
void Slave::answer_save_to_file(){
    auto filename = QFileDialog::getSaveFileName(this);
    if(filename.isEmpty())return;
    QFile file(filename);
    if(!file.open(QIODevice::WriteOnly))return;
    QTextStream out(&file);
    out << _answer.get_answer().c_str();
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

void Slave::print_text(QString str){
    ui->textBrowser->setPlainText( ui->textBrowser->toPlainText() + str + QString("\n"));
    br->setValue(br->maximum());
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
void Slave::print_algorithm_message(std::string str){
    print_text(QString(str.c_str()));
}
void Slave::get_from_official_server(){
    network = new net(get_geturl(),get_posturl());
    network->setParent(this);
    auto str = network->get_from_official_server();
    qDebug() << str.c_str();
    _problem = problem_type(str.c_str());
}
void Slave::sent_to_official_server(){
    network = new net(get_geturl(),get_posturl());
    network->setParent(this);
    auto str = network->send_to_official_server(_answer);
    qDebug() << str.c_str();
}
void Slave::answer_auto_save_to_file(field_type answer){
    file_mtx.lock();
    QDateTime dateTime = QDateTime::currentDateTimeUtc();
    QString filename;
    filename += QString("S") += QString::number(answer.get_score()) += QString("N") += QString::number(answer.get_stone_num()) += QString("T") += dateTime.time().toString("hh:mm:ss") += QString(".txt");
    QFile file(filename);
    if(!file.open(QIODevice::WriteOnly))return;
    QTextStream out(&file);
    out << _answer.get_answer().c_str();
    file.close();
    file_mtx.unlock();
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
void Slave::ui_button_playerid_pushed(){settings->setValue("PLAYERID",ui->playerid_lineEdit->text());}

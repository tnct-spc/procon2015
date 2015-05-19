#include "slave.h"
#include "ui_slave.h"
#include <QUrl>
#include <string>
#include "takao.hpp"
#include "muen_zuka.hpp"
#include "algorithm_manager.hpp"
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
                      std::string(ui->token_line_edit->text().toLocal8Bit()),
                      ui->prob_num_line_edit->text().toInt()
                      );
    auto str = network->get();
    if(network->is_error()){
        ui->net_label->setText("ネットが死んでる");
        return;
    }
    ui->net_label->setText("ネットは生きてる");
    problem_type problem(str);

    //solve
    algo_manager = new algorithm_manager(problem);
    connect(algo_manager,&algorithm_manager::answer_ready,this,&Slave::answer_send);
    algo_manager->run();

}
void Slave::answer_send(field_type answer){
    //std::cout<<"answer=\n\""<<answer.get_answer()<<"\""<<std::endl;
    net_mtx.lock();
    network->send(answer);
    net_mtx.unlock();
}

#include "slave.h"
#include "ui_slave.h"
#include <QUrl>
#include <string>
#include <simple_algorithm.hpp>
#include "takao.hpp"

#define _DEBUG
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

    //set url
    QUrl gethost("http://localhost:8080/files/problem1.txt");
    QUrl posthost("http://localhost:8080/answer");

    //get problem
    //net network(gethost,posthost,"testman",1);
    network = new net(gethost,posthost,"testman",1);

    //std::string problem_data = network->get();
    problem_type problem(network->get());

    //solve
    //simple_algorithm algorithm;
    simple_algorithm algorithm(problem);

    connect(&algorithm,&simple_algorithm::answer_ready,this,&Slave::answer_send);

    //algorithm.run(problem_data);
    algorithm.run();
}
void Slave::answer_send(field_type answer){
    std::cout<<"answer=\n\""<<answer.get_answer()<<"\""<<std::endl;
    net_mtx.lock();
    network->send(answer);
    net_mtx.unlock();
}

#include "slave.h"
#include "ui_slave.h"
#include <QUrl>
#include <string>
#include "takao.hpp"

#include <algorithm/simple_algorithm.hpp>

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

    //set url
    QUrl gethost("http://localhost:8080/files/problem1.txt");
    QUrl posthost("http://localhost:8080/answer");

    //get problem
    net network(gethost,posthost,"testman",1);
    std::string problem_data = network.get();

    //solve
    simple_algorithm algorithm;
    field_type answer = algorithm.run(problem_data);
#ifdef _DEBUG
    std::cout<<"answer=\n\""<<answer.get_answer()<<"\""<<std::endl;
#endif

    //send
    network.send(answer);
}

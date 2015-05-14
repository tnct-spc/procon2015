#include "slave.h"
#include "ui_slave.h"

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

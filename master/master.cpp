#include "master.h"
#include "ui_master.h"

Master::Master(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::Master)
{
    ui->setupUi(this);
}

Master::~Master()
{
    delete ui;
}

void Master::change_token_box(){
    g_token_name = ui->token_name_box->text();
}

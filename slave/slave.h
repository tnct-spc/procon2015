#ifndef SLAVE_H
#define SLAVE_H

#include <QWidget>
#include <net.hpp>
#include <mutex>
#include "takao.hpp"
#include "muen_zuka.hpp"
namespace Ui {
class Slave;
}

class Slave : public QWidget
{
    Q_OBJECT

public:
    explicit Slave(QWidget *parent = 0);
    ~Slave();

private:
    Ui::Slave *ui;
    net *network;
    std::mutex net_mtx;
private slots:
    void clicked_run_button();
    void answer_send(field_type answer);
};

#endif // SLAVE_H
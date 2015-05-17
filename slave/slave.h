#ifndef SLAVE_H
#define SLAVE_H

#include <QWidget>
#include <net.hpp>

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

private slots:
    void clicked_run_button();
};

#endif // SLAVE_H

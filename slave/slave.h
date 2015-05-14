#ifndef SLAVE_H
#define SLAVE_H

#include <QWidget>

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
};

#endif // SLAVE_H

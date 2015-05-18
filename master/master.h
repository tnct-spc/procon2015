#ifndef MASTER_H
#define MASTER_H

#include <QWidget>

namespace Ui {
class Master;
}

class Master : public QWidget
{
    Q_OBJECT

public:
    explicit Master(QWidget *parent = 0);
    ~Master();

private:
    Ui::Master *ui;
};

#endif // MASTER_H

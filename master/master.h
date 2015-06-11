#ifndef MASTER_H
#define MASTER_H

#include <QWidget>
#include "global.h"

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

private slots:
    void change_token_box();
};

#endif // MASTER_H

#ifndef ALGORITHM_MANAGER_H
#define ALGORITHM_MANAGER_H

#include <QObject>

class algorithm_manager : public QObject
{
    Q_OBJECT
public:
    explicit algorithm_manager(QObject *parent = 0);
    ~algorithm_manager();

signals:

public slots:
};

#endif // ALGORITHM_MANAGER_H

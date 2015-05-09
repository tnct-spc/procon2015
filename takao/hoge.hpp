#ifndef HOGE_H
#define HOGE_H

#include <QObject>

class hoge : public QObject
{
    Q_OBJECT
public:
    explicit hoge(QObject *parent = 0);
    ~hoge();

signals:

public slots:
};

#endif // HOGE_H

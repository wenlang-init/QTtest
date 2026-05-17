#ifndef EPOLLTEST_H
#define EPOLLTEST_H

#include <QObject>

class epollTest : public QObject
{
    Q_OBJECT
public:
    explicit epollTest(QObject *parent = nullptr);

signals:
};

#endif // EPOLLTEST_H

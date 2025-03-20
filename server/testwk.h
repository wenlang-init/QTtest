#ifndef TESTWK_H
#define TESTWK_H

#include <QObject>
#include <QThread>

class TestWk : public QThread
{
    Q_OBJECT
public:
    explicit TestWk(QThread *parent = nullptr);
    ~TestWk();
    void startrun();
    void stoprun();

protected:
    void run() override;
signals:
private:
    bool isrun;
};

#endif // TESTWK_H

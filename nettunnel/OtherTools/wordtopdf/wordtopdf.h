#ifndef WORDTOPDF_H
#define WORDTOPDF_H

#include <QWidget>

namespace Ui {
class WordToPdf;
}

class WTP : public QObject
{
    Q_OBJECT

public:
    explicit WTP(QObject *parent = nullptr);
private slots:
    void wtpfun(QString srcname,QString dstname);
};

class WordToPdf : public QWidget
{
    Q_OBJECT

public:
    explicit WordToPdf(QWidget *parent = nullptr);
    ~WordToPdf();
protected:
private slots:
    void on_pushButton_clicked();
signals:
    void wtpfun(QString srcname,QString dstname);
private:
    Ui::WordToPdf *ui;
    WTP *wtp;
    QThread *thread;
};

#endif // WORDTOPDF_H

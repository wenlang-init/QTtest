#ifndef SHOUYINW_H
#define SHOUYINW_H

#include <QWidget>

namespace Ui {
class ShouYinW;
}

class ShouYinW : public QWidget
{
    Q_OBJECT

public:
    explicit ShouYinW(QWidget *parent = nullptr);
    ~ShouYinW();

private slots:
    void on_pushButton1_clicked();

    void on_pushButton2_clicked();

    void on_checkBox1_clicked();

    void on_checkBox1_2_clicked();

    void on_checkBox3_clicked();

private:
    Ui::ShouYinW *ui;
    int checkindex;
};

#endif // SHOUYINW_H

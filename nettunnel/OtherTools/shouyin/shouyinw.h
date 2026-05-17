#ifndef SHOUYINW_H
#define SHOUYINW_H

#include <QWidget>

namespace Ui {
class ShouYinW;
}

class ShouYinW : public QWidget {
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

    void on_pushButtonb1_clicked();

    void on_pushButtonb2_clicked();

    void on_pushButton_clicked();

    void on_pushButtonu_clicked();

    void on_pushButtonmd51_clicked();

    void on_pushButtonmd52_clicked();

    void on_pushButtonmd53_clicked();

    int  getHashValue(int value);

signals:

    void getFileHash(QString filePath,
                     int     hash);

private:

    Ui::ShouYinW *ui;
    int checkindex;
};

#endif // SHOUYINW_H

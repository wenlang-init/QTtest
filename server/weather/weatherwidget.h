#ifndef WEATHERWIDGET_H
#define WEATHERWIDGET_H

#include <QWidget>
#include "weather.h"
#include <QPlainTextEdit>

namespace Ui {
class WeatherWidget;
}

class WeatherWidget : public QWidget
{
    Q_OBJECT

public:
    explicit WeatherWidget(QWidget *parent = nullptr);
    ~WeatherWidget();
private slots:
    void on_pushButton_clicked();

    void on_pushButtonlocation_clicked();

    void on_pushButtonhome_clicked();

    void on_pushButtonp_clicked();

    void on_pushButtonn_clicked();
    void on_pushButtony_clicked();

    void on_pushButtonr_clicked();

protected:
    void insertText(QString text, QPlainTextEdit* plainTextEdit,int fontSize,
                    QColor fontColor, QColor backColor);
private:
    Ui::WeatherWidget *ui;
    WeatherInfo m_winfo;
    int currentIndex = -1;
    bool isok = false;
};

#endif // WEATHERWIDGET_H

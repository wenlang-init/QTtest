#ifndef MAPVIEW3WIDGET_H
#define MAPVIEW3WIDGET_H

#include <QWidget>

QT_BEGIN_NAMESPACE

namespace Ui {
class MapView3Widget;
}

QT_END_NAMESPACE

class GetUrl;

class MapView3Widget : public QWidget {
    Q_OBJECT

public:

    Q_INVOKABLE MapView3Widget(QWidget *parent = nullptr);
    ~MapView3Widget();

private slots:

    void on_com_url_activated(int index);

    void on_com_url_editTextChanged(const QString& arg1);

    void on_but_clear_clicked();

private:

    void showRect(QRect rect);

private:

    Ui::MapView3Widget *ui;
    GetUrl *m_geturl = nullptr;
};
#endif // WIDGET_H

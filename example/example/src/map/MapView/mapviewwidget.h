#ifndef MAPVIEWWIDGET_H
#define MAPVIEWWIDGET_H

#include <QWidget>

QT_BEGIN_NAMESPACE
namespace Ui { class MapViewWidget; }
QT_END_NAMESPACE

class MapViewWidget : public QWidget {
    Q_OBJECT

public:

    MapViewWidget(QWidget *parent = nullptr);
    ~MapViewWidget();

private slots:

    void on_pushButton_clicked();

private:

    Ui::MapViewWidget *ui;
};
#endif // MAPVIEWWIDGET_H

#ifndef HOMEWIDGET_H
#define HOMEWIDGET_H

#include <QWidget>
class DeskTopWidget;
class homewidget : public QWidget {
    Q_OBJECT

public:

    explicit homewidget(QWidget *parent = nullptr);
    ~homewidget();

protected:

    void init();
    void showWidget(int index);
    void keyPressEvent(QKeyEvent *event) override;

    struct  widgetItem {
        QWidget *widget;
        QString  name;
        QString  imageurl;
        widgetItem() {
            widget = nullptr;
        }
    };

protected slots:

    void widgetDestroyed();

private:

    QList<widgetItem>m_widgetList;
    DeskTopWidget *desktopwidget;
};
#endif // ifndef HOMEWIDGET_H

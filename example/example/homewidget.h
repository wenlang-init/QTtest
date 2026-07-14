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
    void initDesktopFile();
    void showWidget(int index);
    void showW(int index);
    void keyPressEvent(QKeyEvent *event) override;

    struct  widgetItem {
        QWidget           *widget;
        QString            name;
        QString            imageurl;
        const QMetaObject *metaObject;
        widgetItem() {
            widget = nullptr;
            metaObject = nullptr;
        }
    };

    struct  desktopItem {
        QString lnkName;
        QString name;
        QString fname;
        QPixmap pixmap;
    };

protected slots:

    void widgetDestroyed();

private:

    QVector<widgetItem>m_widgetList;
    QVector<desktopItem>m_desktopList;
    DeskTopWidget *desktopwidget;
};
#endif // ifndef HOMEWIDGET_H

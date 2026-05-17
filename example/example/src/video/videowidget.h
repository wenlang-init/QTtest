#ifndef VIDEOWIDGET_H
#define VIDEOWIDGET_H

#include <QWidget>
class QMenu;
class mediaPlayerWidget;
class QTimer;
class QTableWidget;
class videoWidget : public QWidget {
    Q_OBJECT

public:

    explicit videoWidget(QWidget *parent = nullptr);
    ~videoWidget();

protected:

    bool eventFilter(QObject *watched,
                     QEvent  *event) override;

signals:

private:

    QMenu *m_menu;
    mediaPlayerWidget *mpw;
    bool isfuls = false;
    QTableWidget *tablewidget;
};

#endif // VIDEOWIDGET_H

#ifndef DRAWWIDGET_H
#define DRAWWIDGET_H

#include <QWidget>

class drawWidget : public QWidget {
    Q_OBJECT

public:

    explicit drawWidget(QWidget *parent = nullptr);
    virtual ~drawWidget() = default;

protected:

    void paintEvent(QPaintEvent *event) override;

signals:
};

#endif // DRAWWIDGET_H

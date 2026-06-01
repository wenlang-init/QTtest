#ifndef QUICKW_H
#define QUICKW_H

#include <QWidget>
#include <QQuickWidget>

class quickW : public QWidget {
    Q_OBJECT

public:

    explicit quickW(QWidget *parent = nullptr);

    void    setText(const QString& text);
    QString getText();

signals:

private:

    QQuickWidget *quickw;
};

#endif // QUICKW_H

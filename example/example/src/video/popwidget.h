#ifndef POPWIDGET_H
#define POPWIDGET_H

#include <QDialog>
#include <QLabel>
class popWidget : public QDialog {
    Q_OBJECT

public:

    explicit popWidget(QString  text = "",
                       QWidget *parent = nullptr);
    void settext(const QString& text);
    void setfont(const QFont& font);

signals:

private:

    QLabel *label;
};

#endif // POPWIDGET_H

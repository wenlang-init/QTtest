#ifndef SETAPPWINFO_H
#define SETAPPWINFO_H

#include <QWidget>
#include <QSharedPointer>

namespace Ui {
class setAppWInfo;
}
class QFontDialog;
class QColorDialog;
class setAppWInfo : public QWidget {
    Q_OBJECT

public:

    Q_INVOKABLE explicit setAppWInfo(QWidget *parent = nullptr);
    ~setAppWInfo();

private slots:

    void on_pushButton_clicked();

    void on_pushButton_2_clicked();

private:

    Ui::setAppWInfo *ui;

    QStringList m_fontlist;
    QFontDialog *m_FontDialog;
    QSharedPointer<QColorDialog>m_SPColorDialog;
};

#endif // SETAPPWINFO_H

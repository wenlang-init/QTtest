#ifndef CODEEDITTESTW_H
#define CODEEDITTESTW_H

#include <QWidget>
#include "codeeditor.h"
#include "highlighter.h"

namespace Ui {
class codeEditTestW;
}

class codeEditTestW : public QWidget
{
    Q_OBJECT

public:
    explicit codeEditTestW(QWidget *parent = nullptr);
    ~codeEditTestW();

private slots:
    void on_pushButton_clicked();

private:
    CodeEditor *editor;
    QTextEdit *textedit;
    Ui::codeEditTestW *ui;
    QStringList strlist;
};

#endif // CODEEDITTESTW_H

#include "widget.h"
#include "printFunction.h"
#include <QApplication>

int main(int argc, char *argv[])
{
    REDIRECT_QTMESSAGE(nullptr);

    QApplication a(argc, argv);
    Widget w;
    w.show();
    return a.exec();
}

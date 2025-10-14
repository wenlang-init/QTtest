#include "netinfo.h"
#include <QTimer>
#include "public/funchelper.h"

NetInfo::NetInfo(QObject *parent)
    : QObject{parent}
{
    QStringList headers = {
        "Interface",         "Receive_Bytes",         "Receive_Packets",
        "Receive_Errs",      "Receive_Drop",
        "Receive_Fifo",      "Receive_Frame",         "Receive_Compressed",
        "Receive_Multicast",
        "Transmit_Bytes",    "Transmit_Packets",      "Transmit_Errs",
        "Transmit_Drop",
        "Transmit_Fifo",     "Transmit_Colls",        "Transmit_Carrier",
        "Transmit_Compressed"
    };

    Q_UNUSED(headers)

    QTimer *timer = new QTimer(this);
    timer->start(1000);
    connect(timer, &QTimer::timeout, this, [ = ]() {
        QString ret =
            FuncHelper::getInstance().executeCmd("cat",
                                                 QStringList("/proc/net/dev"));
        QStringList slist = ret.split("\n");

        QString str;

        for (int i = 0; i < slist.size(); i++) {
            QString line = slist.at(i).simplified();

            if (line.startsWith("Inter") || line.startsWith("face") ||
                line.isEmpty()) {
                str.append(slist[i]);
                str.append("\n");
                continue;
            }
            QStringList list = line.split(":");

            if (list.size() != 2) {
                continue;
            }
            QString iface = list.at(0).trimmed();
            QString data = list.at(1).simplified();
            QStringList dlist = data.split(" ");

            if (dlist.size() != 16) {
                continue;
            }

            // 只显示非lo的网络接口
            // if (iface == "lo") continue;
            str +=
                QString(
                    "\n%1 %2 %3 %4 %5 %6 %7 %8 %9 %10 %11 %12 %13 %14 %15 %16 %17")
                .arg(iface,        -11, ' ')
                .arg(dlist.at(0),  -11, ' ')
                .arg(dlist.at(1),  -11, ' ')
                .arg(dlist.at(2),  -11, ' ')
                .arg(dlist.at(3),  -11, ' ')
                .arg(dlist.at(4),  -11, ' ')
                .arg(dlist.at(5),  -11, ' ')
                .arg(dlist.at(6),  -11, ' ')
                .arg(dlist.at(7),  -11, ' ')
                .arg(dlist.at(8),  -11, ' ')
                .arg(dlist.at(9),  -11, ' ')
                .arg(dlist.at(10), -11, ' ')
                .arg(dlist.at(11), -11, ' ')
                .arg(dlist.at(12), -11, ' ')
                .arg(dlist.at(13), -11, ' ')
                .arg(dlist.at(14), -11, ' ')
                .arg(dlist.at(15), -11, ' ');
        }

        // ret = ret.simplified();
        emit sig_netInfo(str);
    });
}

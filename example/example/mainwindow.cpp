#include <QDebug>
#include <QTimer>
#include <QThread>
#include <QDateTime>
#include <QFile>
#include <QResizeEvent>
#include "mainwindow.h"
#include "./ui_mainwindow.h"

// #include <QMqttClient>
#include "src/sql_engine.h"

// #define TTTT
#ifdef TTTT
# include "src/messageWidget/listmessageview.h"
# include "src/video/videowidget.h"
# include "src/graphics/graphicswidget.h"
# include "src/qmlList/listw.h"
# include "src/audio/widget.h"
# include "src/layout/wlayout.h"
# include "src/fftw/widegtfft.h"
#endif // ifdef TTTT


#include <QVBoxLayout>
#include "XxwCustomPlot.h"

#include <random>
#include <chrono>

#if defined(WIN32) || defined(WIN64)
# include <QWindow>
# include <QFileInfo>
# include <QProcess>
# include <windows.h>
# pragma comment(lib, "User32.lib")
#endif // if defined(WIN32) || defined(WIN64)

#define qdebug qDebug().noquote() << "[" << __FILE__ << ":" << __LINE__ << "] " << \
        __FUNCTION__ << "() "

static Sql_Engine *sqle = new Sql_Engine;
static QThread    *mthread;

static XxwCustomPlot *customPlot;

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    // addWindow("calc.exe",    "ApplicationFrameWindow", "计算器");
    // addWindow("calc.exe",    nullptr,      "计算器");
    // addWindow("mspaint.exe", "MSPaintApp", "无标题 - 画图");
    // addWindow("msconfig",    "#32769",                 "[Desktop]");

    QString sqlname = QCoreApplication::applicationDirPath() + "/testsql.db";

    // if (QFile::exists(sqlname)) qDebug().noquote() << QFile::remove(sqlname);
    qDebug().noquote() << QSqlDatabase::drivers();
    sqle->openDb(sqlname);

    // sqle->sqliteVacuumAuto("FULL");
    sqle->dropTable("testtablename");
    sqle->sqliteVacuum();

    QString tablename = "testtablename";
    QString cmd = QString("create table %1 "
                          "(id integer primary key,"
                          "label varchar(256),"
                          "valuei integer,"
                          "valuef real,"
                          "datetime timestamp,"
                          "datetimei bigint)").arg(tablename);
    qDebug().noquote() << sqle->createTable(tablename, cmd);

    QTimer *timer = new QTimer;
    timer->start(3000);
    srand(0); // std::srand(0);
    connect(timer, &QTimer::timeout, [ = ]() {
        qDebug().noquote() << "timeout";
        QStringList tablenames;
        sqle->getAllTablename(tablenames);
        qDebug().noquote() << tablenames;
        QMap<QString, QString>tableinfo;
        sqle->queryTabbleInfo(tablename,
                              tableinfo);
        qDebug().noquote() << tableinfo;
        qDebug().noquote() << sqle->queryTableRowCount(tablename);

        QStringList cmds;

        qint64 mtime = QDateTime::currentMSecsSinceEpoch();

        int cnt = 300000;

        for (int i = 0; i < cnt; i++) {
            QString cmd =
                QString("insert into %1 (label,valuei,valuef,datetime,datetimei) "
                        "values('%2','%3','%4','%5','%6')").
                arg("testtablename",
                    QString::number(mtime + i * 1.0 / cnt, 'f'),
                    QString::number(i),
                    QString::number(i + rand() % 1000),
                    QString::number(i + rand() % 1000),
                    QString::number(i + rand() % 1000)
                    );
            cmds.append(cmd);
        }
        qDebug().noquote() <<  sqle->executeMultitudeCmd(cmds);
    });
    mthread = new QThread(this);
    timer->moveToThread(mthread);

    // mthread->start();

    _init();

#ifdef TTTT

    //////////////////////////
    videoWidget *vw = new videoWidget;
    vw->resize(600, 800);
    vw->show();
    vw->setAttribute(Qt::WA_DeleteOnClose, true);
    connect(vw, &videoWidget::destroyed, this, [&]() {
        qDebug() << "videoWidget -------" << sender();
    });

    ListMessageView *mqw = new ListMessageView;
    mqw->resize(600, 800);
    mqw->show();
    mqw->setAttribute(Qt::WA_DeleteOnClose, true);
    connect(mqw, &ListMessageView::destroyed, this, [&]() {
        qDebug() << "ListMessageView -------" << sender();
    });

    GraphicsWidget *gpw = new GraphicsWidget;
    gpw->resize(600, 800);
    gpw->show();
    gpw->setAttribute(Qt::WA_DeleteOnClose, true);
    connect(gpw, &GraphicsWidget::destroyed, this, [&]() {
        qDebug() << "GraphicsWidget -------" << sender();
    });

    ListW *lw = new ListW;
    lw->resize(600, 800);
    lw->show();
    lw->setAttribute(Qt::WA_DeleteOnClose, true);
    connect(lw, &ListW::destroyed, this, [&]() {
        qDebug() << "ListW -------" << sender();
    });

    Widget *audiow = new Widget;
    audiow->resize(600, 800);
    audiow->show();
    audiow->setAttribute(Qt::WA_DeleteOnClose, true);
    connect(audiow, &Widget::destroyed, this, [&]() {
        qDebug() << "Widget -------" << sender();
    });

    wLayout *wl = new wLayout;
    wl->resize(600, 800);
    wl->show();
    wl->setAttribute(Qt::WA_DeleteOnClose, true);
    connect(wl, &wLayout::destroyed, this, [&]() {
        qDebug() << "wLayout -------" << sender();
    });
    widegtFFT *wft = new widegtFFT;
    wft->resize(600, 800);
    wft->show();
    wft->setAttribute(Qt::WA_DeleteOnClose, true);
    connect(wft, &widegtFFT::destroyed, this, [&]() {
        qDebug() << "widegtFFT -------" << sender();
    });

#endif // if 0
}

MainWindow::~MainWindow()
{
    delete ui;
    mthread->quit();
    mthread->wait();
    delete mthread;
}

void MainWindow::addWindow(QString     processName,
                           const char *lpClassName,
                           const char *lpWindowName)
{
#if defined(WIN32) || defined(WIN64)
    QProcess *process = new QProcess(this);

    connect(qApp, &QApplication::aboutToQuit, [process]() {
        process->kill();
        process->waitForFinished();
    });
    process->start(processName);
    process->waitForStarted();
    process->waitForFinished(2000);

# if 0
    HWND wid = FindWindowW((LPCWSTR)QString::fromUtf8(lpClassName).utf16(),
                           (LPCWSTR)QString::fromUtf8(lpWindowName).utf16());
# else // if 0
    HWND wid = FindWindowA(QString::fromUtf8(lpClassName).toLocal8Bit(),
                           QString::fromUtf8(lpWindowName).toLocal8Bit());
# endif // if 0
    qDebug() << wid;

    // if (wid) {
    //     // 获取窗口位置
    //     RECT rect;
    //     GetWindowRect(wid, &rect);
    //     // 移动窗口并调整大小
    //     MoveWindow(wid, rect.left + 100, rect.top + 100, 800, 600, TRUE);
    // }
    if (wid) {
        QWindow *window = QWindow::fromWinId((WId)wid);
        QWidget *widget = QWidget::createWindowContainer(window, ui->tabWidget);

        QFileInfo info(processName);
        ui->tabWidget->addTab(widget, QStringLiteral("进程：") + info.fileName());

        ui->tabWidget->setCurrentWidget(widget);
    }
#endif // if defined(WIN32) || defined(WIN64)
}

int MainWindow::lottery(int count,
                        int cnt1,
                        int cnt2,
                        double prob_early,
                        double prob_late,
                        int& underway,
                        QList<tnode>& vlist, QList<tnode>& vlistbase) {
    vlist.resize(cnt1 + cnt2);

    for (int i = 0; i < vlist.size(); i++) {
        vlist[i].index = i;
        vlist[i].cnt = 0;
    }
    int allcnt = cnt1 + cnt2;

    // 随机数生成器（使用当前时间作为种子）
    std::mt19937 rng(
        std::chrono::steady_clock::now().time_since_epoch().count());

    for (int i = 0; i < count; i++) {
        // // 随机数生成器（使用当前时间作为种子）
        // std::mt19937 rng(
        //     std::chrono::steady_clock::now().time_since_epoch().count());

        if (allcnt - 1 == underway) {
            vlist[underway].cnt++;
            underway = 0;
        } else if (underway < cnt1) {
            if (std::bernoulli_distribution(prob_early)(rng)) {
                vlist[underway].cnt++;
                underway = 0;
            } else {
                underway++;
            }
        } else {
            if (std::bernoulli_distribution(prob_late)(rng)) {
                vlist[underway].cnt++;
                underway = 0;
            } else {
                underway++;
            }
        }
    }

    // 均匀分布 [0, 1)
    // std::uniform_real_distribution<double> dist(0.0, 1.0);
    vlistbase = vlist;
    std::sort(vlist.begin(), vlist.end(), [ = ](const tnode& a, const tnode& b) {
        return a.cnt > b.cnt;
    });
#if 0

    for (int i = 0; i < vlist.size(); i++) {
        printf("%d:%d\t", vlist[i].index, vlist[i].cnt);
    }
    printf("\n"); fflush(stdout);
#endif // if 0
    return vlist[0].index;
}

void MainWindow::_init()
{
    customPlot = new XxwCustomPlot;
    customPlot->showTracer(true);

    QVBoxLayout *mainLayout = new QVBoxLayout(ui->widget);
    mainLayout->addWidget(customPlot);

    customPlot->addGraph();
    customPlot->addGraph();

    for (int i = 0; i < customPlot->graphCount(); i++)
    {
        customPlot->graph(i)->setLineStyle(QCPGraph::lsLine);
        customPlot->graph(i)->setScatterStyle(QCPScatterStyle::ssNone);
    }

    // customPlot->graph(0)->setBrush(QBrush(QColor(0, 0, 255, 20))); // 设置曲线下方的背景颜色
    customPlot->graph(0)->setPen(QPen(Qt::blue));
    customPlot->graph(0)->setName("抽奖统计曲线");

    customPlot->graph(1)->setPen(QPen(Qt::red));
    customPlot->graph(1)->setName("抽奖统计累计曲线");

    customPlot->xAxis->setVisible(true);
    customPlot->xAxis->setTickLabels(true);
    customPlot->yAxis->setVisible(true);
    customPlot->yAxis->setTickLabels(true);

    customPlot->xAxis2->setVisible(false);
    customPlot->xAxis2->setTickLabels(false); // 隐藏x轴刻度线
    customPlot->yAxis2->setVisible(false);
    customPlot->yAxis2->setTickLabels(false);

    // 设置多选键
    customPlot->setMultiSelectModifier(Qt::KeyboardModifier::ControlModifier);

    // 可拖动、可缩放、轴可选、图例可选、绘图可选,可多选
    customPlot->setInteractions(
        QCP::iRangeDrag | QCP::iRangeZoom | QCP::iSelectAxes |
        QCP::iSelectLegend | QCP::iSelectPlottables | QCP::iMultiSelect);

    // 鼠标滚动缩放倍率，默认0.85
    // customPlot->axisRect()->setRangeZoomFactor(0.5);
    // 设置鼠标滚轮轴方向
    customPlot->axisRect()->setRangeZoom(Qt::Vertical | Qt::Horizontal);

    // 显示图例
    customPlot->legend->setVisible(true);
    customPlot->legend->setBrush(QBrush(QColor(0, 0, 255, 20)));
    customPlot->legend->setTextColor(QColor(Qt::lightGray));
    customPlot->legend->setBorderPen(Qt::NoPen);          // 无边框
    customPlot->legend->setMargins(QMargins(0, 0, 0, 0)); // 文字与边框的距离
    // customPlot->legend->setFillOrder(QCPLayoutGrid::foRowsFirst);
    // customPlot->legend->setWrap(2); // 设置2个图例自动换行
    // 设置图例位置，这里选择显示在QCPAxisRect下方
    // customPlot->plotLayout()->addElement(1, 0, customPlot->legend);
    // 设置显示比例
    // customPlot->plotLayout()->setRowStretchFactor(1, 0.001);

    // 默认情况下，图例位于主轴矩形的插入布局中，所以这就是我们如何访问它来改变图例的位置：
    customPlot->axisRect()->insetLayout()->setInsetAlignment(0,
                                                             Qt::AlignTop |
                                                             Qt::AlignCenter);

    customPlot->legend->setSelectableParts(QCPLegend::spItems);

    // 使左轴和下轴始终将其范围转移到右轴和上轴:
    connect(customPlot->xAxis,
            SIGNAL(rangeChanged(QCPRange)),
            customPlot->xAxis2,
            SLOT(setRange(QCPRange)));
    connect(customPlot->yAxis,
            SIGNAL(rangeChanged(QCPRange)),
            customPlot->yAxis2,
            SLOT(setRange(QCPRange)));

    // connect(customPlot, &XxwCustomPlot::selectionChangedByUser, this, [ = ]()
    // {
    //     // 使上下轴同步选择，并处理轴,并勾选标签作为一个可选择的对象：
    //     if (customPlot->xAxis->selectedParts().testFlag(QCPAxis::spAxis) ||
    //
    //       customPlot->xAxis->selectedParts().testFlag(QCPAxis::spTickLabels)
    // ||
    //         customPlot->xAxis2->selectedParts().testFlag(QCPAxis::spAxis) ||
    //
    //
    //    customPlot->xAxis2->selectedParts().testFlag(QCPAxis::spTickLabels))
    //     {
    //         customPlot->xAxis2->setSelectedParts(QCPAxis::spAxis |
    //                                              QCPAxis::spTickLabels);
    //         customPlot->xAxis->setSelectedParts(QCPAxis::spAxis |
    //                                             QCPAxis::spTickLabels);
    //     }

    //     // 使左右轴同步选择，并处理轴,并勾选标签作为一个可选择的对象
    //     if (customPlot->yAxis->selectedParts().testFlag(QCPAxis::spAxis) ||
    //
    //       customPlot->yAxis->selectedParts().testFlag(QCPAxis::spTickLabels)
    // ||
    //         customPlot->yAxis2->selectedParts().testFlag(QCPAxis::spAxis) ||
    //
    //
    //    customPlot->yAxis2->selectedParts().testFlag(QCPAxis::spTickLabels))
    //     {
    //         customPlot->yAxis2->setSelectedParts(QCPAxis::spAxis |
    //                                              QCPAxis::spTickLabels);
    //         customPlot->yAxis->setSelectedParts(QCPAxis::spAxis |
    //                                             QCPAxis::spTickLabels);
    //     }

    //     // 将图形的选择与相应图例项的选择同步
    //     for (int i = 0; i < customPlot->graphCount(); ++i)
    //     {
    //         QCPGraph *graph = customPlot->graph(i);
    //         QCPPlottableLegendItem *item =
    //             customPlot->legend->itemWithPlottable(graph);

    //         if (item->selected() || graph->selected())
    //         {
    //             item->setSelected(true);
    //
    //
    //        graph->setSelection(QCPDataSelection(graph->data()->dataRange()));
    //         }
    //     }
    // });

    customPlot->xAxis->setLabel("在第X次出货");
    customPlot->yAxis->setLabel("在X次累计出货数");
}

// void MainWindow::resizeEvent(QResizeEvent *event)
// {
//     MainWindow::resizeEvent(event);

//     // ui->centralwidget->resize(event->size());
// }

void MainWindow::on_pushButton_clicked()
{
    int cnt1 = ui->spinBox_2->value();
    int cnt2 = ui->spinBox_3->value();
    double prob_early = ui->doubleSpinBox->value();
    double prob_late = ui->doubleSpinBox_2->value();
    int    count = ui->spinBox->value();

    QList<tnode> vlist;
    QList<tnode> vlistbase;

    qDebug() <<
        lottery(count,
                cnt1,
                cnt2,
                prob_early,
                prob_late,
                m_underway,
                vlist,
                vlistbase);

    if (m_vlistbase.size() != vlistbase.size()) {
        m_vlistbase.clear();
        m_vlistbase.resize(vlistbase.size(), 0);
    }

    for (int i = 0; i < m_vlistbase.size(); i++) {
        m_vlistbase[i] += vlistbase[i].cnt;
    }

    m_count += count;

    QString str;
    str += "最大次数:" + QString::number(vlist[0].index + 1) + ","
           + QString::number(vlist[0].cnt) + "\n";
    int cnt = 0;

    foreach(tnode node, vlist) {
        cnt += node.cnt;
    }
    qint64 cnta = 0;
    foreach(qint64 node, m_vlistbase) {
        cnta += node;
    }
    str += "前" + QString::number(cnt1) + "次概率:"
           + QString::number(prob_early * 100, 'f', 2) + "%\n";
    str += "后" + QString::number(cnt2) + "次概率:"
           + QString::number(prob_late * 100, 'f', 2) + "%\n";
    str += "共抽" + QString::number(count) + "次,综合概率："
           + QString::number((double)cnt / count * 100, 'f', 4) + "%\n";
    str += "共抽中" + QString::number(cnt) + "个\n";
    str += "累计共抽" + QString::number(m_count) + "次,综合概率："
           + QString::number((double)cnta / m_count * 100, 'f', 4) + "%\n";
    str += "共抽中" + QString::number(cnta) + "个\n";

    str += "--------------从大到小--------------------\n";

    for (int i = 0; i < vlist.size(); i++) {
        str += QString::number(vlist[i].index + 1) + ","
               + QString::number(vlist[i].cnt * 100.0 / cnt, 'f', 2) + "%,"
               + QString::number(vlist[i].cnt) + "\t";
    }
    str += "\n";

    str += "--------------顺序--------------------\n";

    for (int i = 0; i < vlistbase.size(); i++) {
        str += QString::number(i + 1) + ","
               + QString::number(vlistbase[i].cnt * 100.0 / cnt, 'f', 2) + "%,"
               + QString::number(vlistbase[i].cnt) + "\t";
    }
    str += "\n";

    str += "---------------累计------------------\n";

    for (int i = 0; i < m_vlistbase.size(); i++) {
        str += QString::number(i + 1) + ","
               + QString::number(m_vlistbase[i] * 100.0 / cnta, 'f', 2) + "%,"
               + QString::number(m_vlistbase[i]) + "\t";
    }
    str += "\n";

    ui->textEdit->setPlainText(str);

    // 画曲线
    customPlot->graph(0)->data().data()->clear();
    customPlot->graph(1)->data().data()->clear();
    customPlot->xAxis->setRange(1, cnt1 + cnt2);
    customPlot->yAxis->setRange(0, vlist[0].cnt);

    for (int i = 0; i < vlistbase.size(); i++)
    {
        customPlot->graph(0)->addData(i + 1, vlistbase[i].cnt);
    }

    for (int i = 0; i < m_vlistbase.size(); i++)
    {
        customPlot->graph(1)->addData(i + 1, m_vlistbase[i]);
    }

    customPlot->replot();
}

void MainWindow::on_pushButton_2_clicked()
{
    m_underway = 0;
    customPlot->graph(0)->data().data()->clear();
    customPlot->graph(1)->data().data()->clear();
    m_count = 0;
    m_vlistbase.clear();
    m_vlistbase.resize(ui->spinBox_2->value() + ui->spinBox_2->value(), 0);
}

void MainWindow::on_checkBox_checkStateChanged(const Qt::CheckState& arg1)
{
    if (arg1 == Qt::Checked) {
        customPlot->showTracer(true);
    } else {
        customPlot->showTracer(false);
        customPlot->replot();
    }
}

// void testqmqtt() {
//     QMqttClient *qmc = new QMqttClient;

//     qmc->setHostname("127.0.0.1");
//     qmc->setPort(1883);

//     // qmc->publish(QMqttTopicName("topic"), "message info");
//     // qmc->subscribe(QMqttTopicFilter("topic"));

//     // connect(qmc, &QMqttClient::stateChanged, this,
//     // [=](QMqttClient::ClientState state){});
//     // connect(qmc, &QMqttClient::disconnected, this, [=](){});

//     // connect(qmc, &QMqttClient::messageReceived, this, [this](const
// QByteArray
//     // &message, const QMqttTopicName &topic) {
//     //     const QString content = QDateTime::currentDateTime().toString()
//     //             + " Received Topic: "_L1
//     //             + topic.name()
//     //             + " Message: "_L1
//     //             + message
//     //             + u'\n';

//     // });

//     // connect(qmc, &QMqttClient::pingResponseReceived, this, [this]() {
//     //     const QString content = QDateTime::currentDateTime().toString()
//     //             + "PingResponse\n"_L1;
//     // });
// }

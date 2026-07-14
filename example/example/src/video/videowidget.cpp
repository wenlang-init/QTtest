#include "videowidget.h"
#include "mediaplayerwidget.h"
#include <QHBoxLayout>
#include <QEvent>
#include <QKeyEvent>
#include <QMouseEvent>
#include <QMenu>
#include <QFileDialog>
#include <QStandardPaths>
#include <QTimer>
#include <QDebug>
#include <QTableWidget>

// #include "ffmpegvideo.h"

#define qdebug qDebug().noquote()

videoWidget::videoWidget(QWidget *parent)
    : QWidget{parent}
{
#if 0
    ffmpegVideo *ffmpegvideo = new ffmpegVideo(this);
    QString url = "D:\\杂项\\视频\\[OmoFun]刀剑神域进击篇：暮色黄昏-正片(OmoFun-05).mp4";

    ffmpegvideo->restartwork(url);
    connect(ffmpegvideo, &ffmpegVideo::gotFrame,  this, [ = ](QImage image) {
        qdebug << "Got frame:" << image.size();
    });
    connect(ffmpegvideo, &ffmpegVideo::errorInfo, this, [ = ](QString error) {
        qdebug << "Error:" << error;
    });
    connect(ffmpegvideo, &ffmpegVideo::videoInfo, this, [ = ](QString info) {
        qdebug << "Info:" << info;
    });
#endif // if 0
    mpw = new mediaPlayerWidget(this);
    QHBoxLayout *hlayout = new QHBoxLayout(this);
    hlayout->setContentsMargins(0, 0, 0, 0);

    hlayout->addWidget(mpw);

    tablewidget = new QTableWidget();
    tablewidget->setWindowTitle("视频信息");
    tablewidget->hide();
    tablewidget->setEditTriggers(QAbstractItemView::NoEditTriggers); // 不能编辑
    tablewidget->setColumnCount(2);
    QStringList la;
    la << "属性" << "值";
    tablewidget->setHorizontalHeaderLabels(la);
    tablewidget->setMinimumSize(200, 600);

    // QPoint point = mapToGlobal(pos());
    // pop->move(point.x() + (width() - pop->width()) / 2,
    //           point.y() + (height() - pop->height()) / 2);

    m_menu = new QMenu(this);
    QAction *action1 = new QAction("打开视频", m_menu);
    QAction *action2 = new QAction("视频信息", m_menu);
    m_menu->addAction(action1);
    m_menu->addAction(action2);
    connect(m_menu, &QMenu::triggered, this, [ = ](QAction *action) {
        if (action == action1) {
            QFileDialog fileDialog(this);
            fileDialog.setAcceptMode(QFileDialog::AcceptOpen);
            fileDialog.setWindowTitle(tr("打开视频"));
            fileDialog.setDirectory(QStandardPaths::standardLocations(
                                        QStandardPaths::
                                        MoviesLocation)
                                    .value(0, QDir::homePath()));

            if (fileDialog.exec() == QDialog::Accepted) {
                const QUrl url = fileDialog.selectedUrls().constFirst();
                mpw->setUrl(url);
            }
        } else if (action == action2) {
            // QPoint point = mapToGlobal(pos());
            // pop->move(point.x() + (width() - pop->width()) / 2,
            //           point.y() + (height() - pop->height()) / 2);
            QMap<QString, QString>data = mpw->getMetaData();
            tablewidget->setRowCount(0);
            QMap<QString, QString>::const_iterator i = data.constBegin();

            while (i != data.constEnd()) {
                tablewidget->insertRow(tablewidget->rowCount());
                QTableWidgetItem *item = new QTableWidgetItem;
                item->setTextAlignment(Qt::AlignCenter);
                item->setText(i.key());
                tablewidget->setItem(tablewidget->rowCount() - 1, 0, item);
                item = new QTableWidgetItem;
                item->setTextAlignment(Qt::AlignCenter);
                item->setText(i.value());
                tablewidget->setItem(tablewidget->rowCount() - 1, 1, item);
                ++i;
            }
            tablewidget->show();
        }
    });

    installEventFilter(this);
}

videoWidget::~videoWidget()
{
    delete tablewidget;
}

bool videoWidget::eventFilter(QObject *watched, QEvent *event)
{
    if (event->type() == QEvent::KeyPress) {
        QKeyEvent *e = static_cast<QKeyEvent *>(event);

        if ((e->key() == Qt::Key_Enter) || (e->key() == Qt::Key_Return)) {
            showFullScreen();
            mpw->setShowM(false);
            isfuls = true;

            // mpw->setFullScreen(true);
        } else if (e->key() == Qt::Key_Escape) {
            showNormal();
            mpw->setShowM(true);
            isfuls = false;

            // mpw->setFullScreen(false);
        }
    } else if (event->type() == QEvent::MouseButtonPress) {
        QMouseEvent *e = static_cast<QMouseEvent *>(event);

        if (e->button() == Qt::RightButton) {
            m_menu->exec(mapToGlobal(e->pos()));
        } else if (e->button() == Qt::LeftButton) {}
    } else if (event->type() ==
               QEvent::MouseButtonRelease) {} else if (event->type() ==
                                                       QEvent::MouseButtonDblClick)
    {
        QMouseEvent *e = static_cast<QMouseEvent *>(event);

        if (e->button() == Qt::LeftButton) {
            // if (isfuls)
            {
                mpw->setShowM(!mpw->isShowM());
            }
        }
    }
    else if (event->type() == QEvent::MouseMove) {}
    else if (event->type() == QEvent::Enter) {}
    else if (event->type() == QEvent::Leave) {}
    return QWidget::eventFilter(watched, event);
}

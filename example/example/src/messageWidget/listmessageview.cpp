#include "listmessageview.h"
#include <QListView>
#include <QMenu>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QDebug>
#include <QDateTime>
#include "messagemodel.h"
#include "qdatetime.h"
#include "testproxymodel.h"
#include "messagedelegate.h"

#define qdebug qDebug().noquote() << "[" << __FILE__ << ":" << __LINE__ << "] " << \
        __FUNCTION__ << "() "

ListMessageView::ListMessageView(QWidget *parent)
    : QWidget{parent}
{
    QHBoxLayout *hboxlayout = new QHBoxLayout(this);
    QListView   *listview = new QListView(this);

    hboxlayout->addWidget(listview);

    // listview->setBatchSize(20);

    // 设置元素之间的间隔
    listview->setSpacing(5);

    // 设置尺寸变化策略
    listview->setResizeMode(QListView::Adjust);

    // 设置元素增减的时候的变化模式
    listview->setFlow(QListView::LeftToRight);

    // 设置伸缩的时候是否自动换行
    listview->setWrapping(true);

    // 设置每个元素的代理
    listview->setItemDelegate(new MessageDelegate(this));

    MessageModel   *mmodel = new MessageModel(this);
    TestProxyModel *tproxymodel = new TestProxyModel(this);

    tproxymodel->setSourceModel(mmodel);
    QItemSelectionModel *selectmodel = new QItemSelectionModel(tproxymodel, this);

    // listview->setLayoutMode(QListView::SinglePass);

    // 设置数据模型
    listview->setModel(tproxymodel);

    // 设置选中的数据模型
    listview->setSelectionModel(selectmodel);

    // 开启自定义的菜单
    listview->setContextMenuPolicy(Qt::CustomContextMenu);

    QMenu   *m_func_menu = new QMenu(listview);
    QAction *act1 = new QAction("添加", m_func_menu);
    QAction *act2 = new QAction("删除", m_func_menu);
    m_func_menu->addAction(act1);
    m_func_menu->addAction(act2);
    static int  cnt = 0;
    static bool sts = true;
    int testcount = 1000;

    for (int i = 0; i < testcount; i++) {
        QString imagename = "D:/work/2.ico";
        QString message = QString::number(cnt++) + "-";
        int     aaa = rand() % 1000;
        message += QString::number(aaa) + " : ";

        for (int i = 0; i < aaa; i++) {
            message += QString(QChar(rand() % 10 + '0'));
        }
        QDateTime time = QDateTime::currentDateTime();
        sts = !sts;
        tproxymodel->addData(time, sts, message, imagename);
    }
    QModelIndex modelIndex = mmodel->index(mmodel->rowCount() - 1, 0);
    listview->setCurrentIndex(modelIndex);
    connect(act1, &QAction::triggered, this, [ = ]() {
        QString imagename = "D:/work/2.ico";
        QString message = QString::number(cnt++) + "-";
        int aaa = rand() % 1000;
        message += QString::number(aaa) + " : ";

        for (int i = 0; i < aaa; i++) {
            message += QString(QChar(rand() % 10 + '0'));
        }
        QDateTime time = QDateTime::currentDateTime();
        sts = !sts;
        tproxymodel->addData(time, sts, message, imagename);

        QModelIndex modelIndex = mmodel->index(mmodel->rowCount() - 1, 0);
        listview->setCurrentIndex(modelIndex);
    });
    connect(act2, &QAction::triggered, this, [ = ]() {
        int row = selectmodel->currentIndex().row();

        mmodel->removeRow(row);

        // tproxymodel->deleteData(row);

        //选中前一个
        QModelIndex previousModelIndex = mmodel->index(row - 1, 0);

        if (previousModelIndex.isValid()) {
            previousModelIndex = mmodel->index(previousModelIndex.row(),
                                               previousModelIndex.column());
            selectmodel->setCurrentIndex(previousModelIndex,
                                         QItemSelectionModel::SelectCurrent);
            return;
        }

        //选中后一个图片
        QModelIndex nextModelIndex = mmodel->index(row, 0);

        if (nextModelIndex.isValid()) {
            nextModelIndex = mmodel->index(nextModelIndex.row(),
                                           nextModelIndex.column());
            selectmodel->setCurrentIndex(nextModelIndex,
                                         QItemSelectionModel::SelectCurrent);
            return;
        }
    });
    connect(listview, &QListView::customContextMenuRequested, this,
            [ = ](const QPoint& pos) {
        QPoint point = pos;

        // QModelIndex mCurrentIndex = listview->indexAt(pos);
        // if (mCurrentIndex.isValid() && (mCurrentIndex.row() >= 0))
        {
            m_func_menu->exec(listview->mapToGlobal(point));
        }
    });
    connect(listview, &QListView::clicked, this, [ = ](const QModelIndex& index) {
        // messageTime, isSend, messageData, headPortrait,
        unsigned long long messageTime =
            tproxymodel->data(index,
                              MessageModel::Roles::messageTime).toULongLong();
        unsigned long long headPortrait =
            tproxymodel->data(index,
                              MessageModel::Roles::headPortrait).toULongLong();
        QString messageData =
            tproxymodel->data(index, MessageModel::Roles::messageData).toString();
        bool isSend = tproxymodel->data(index,
                                        MessageModel::Roles::isSend).toBool();
        QDateTime datetime = QDateTime::fromMSecsSinceEpoch(messageTime);
        QImage *image = (QImage *)headPortrait;
        QString str = isSend ? "发送" : "接受";
        str += "(" + datetime.toString("yyyy-MM-dd hh:mm:ss.zzz") + ")";

        if (image) {
            str += " (" + QString::number(image->size().width());
            str += QString::number(image->size().height()) + ")";
        }
        str += messageData;
        qdebug << str;
    });
}

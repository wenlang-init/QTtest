#include "mylistview.h"
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QPushButton>
#include <QListView>
#include <QMenu>
#include "picturedelegate.h"
#include "pictureproxymodel.h"
#include "picturemodel.h"
#include <QFileDialog>
#include <QStandardPaths>

MyListView::MyListView(QWidget *parent)
    : QWidget{parent}
{
    QHBoxLayout *hboxlayout = new QHBoxLayout;
    QVBoxLayout *vboxlayout = new QVBoxLayout(this);
    QPushButton *btn1 = new QPushButton(this);

    btn1->setText("添加");
    QPushButton *btn2 = new QPushButton(this);
    btn2->setText("修改");
    QPushButton *btn3 = new QPushButton(this);
    btn3->setText("从列表删除");
    QPushButton *btn4 = new QPushButton(this);
    btn4->setText("清空所有");
    hboxlayout->addWidget(btn1);
    hboxlayout->addWidget(btn2);
    hboxlayout->addWidget(btn3);
    hboxlayout->addWidget(btn4);
    btn1->setMaximumHeight(30);
    btn2->setMaximumHeight(30);
    btn3->setMaximumHeight(30);
    btn4->setMaximumHeight(30);
    listview = new QListView(this);

    // 设置元素之间的间隔
    listview->setSpacing(5);

    // 设置尺寸变化策略
    listview->setResizeMode(QListView::Adjust);

    // 设置元素增减的时候的变化模式
    listview->setFlow(QListView::LeftToRight);

    // 设置伸缩的时候是否自动换行
    listview->setWrapping(true);

    // 设置每个元素的代理
    listview->setItemDelegate(new PictureDelegate(this));

    // 初始化功能菜单
    m_func_menu = new QMenu(listview);
    m_del_current_pic = new QAction("从列表删除当前图片", this);
    m_func_menu->addAction(m_del_current_pic);
    connect(m_del_current_pic,
            &QAction::triggered,
            this,
            &MyListView::delCurrentPicture);
    connect(btn1,
            &QPushButton::clicked,
            this,
            &MyListView::addPictures);
    connect(btn2,
            &QPushButton::clicked,
            this,
            &MyListView::updatePictures);
    connect(btn3,
            &QPushButton::clicked,
            this,
            &MyListView::delPictures);
    connect(btn4,
            &QPushButton::clicked,
            this,
            &MyListView::clearPicture);
    connect(listview,
            &QListView::customContextMenuRequested,
            this,
            &MyListView::showCustomMenu);

    // 开启自定义的菜单
    listview->setContextMenuPolicy(Qt::CustomContextMenu);

    vboxlayout->addLayout(hboxlayout);

    // vboxlayout->addStretch();
    vboxlayout->addWidget(listview);

    PictureModel *pic_model = new PictureModel(this);
    mPictureModel = new PictureProxyModel(this);
    mPictureModel->setSourceModel(pic_model);
    mPictureSelectionModel = new QItemSelectionModel(
        mPictureModel,
        this);

    // 设置数据模型
    listview->setModel(mPictureModel);

    // 设置选中的数据模型
    listview->setSelectionModel(mPictureSelectionModel);
}

MyListView::~MyListView()
{}

void MyListView::addPictures()
{
    QStringList filenames =
        QFileDialog::getOpenFileNames(this,
                                      "添加图片",
                                      QStandardPaths::writableLocation(
                                          QStandardPaths::DesktopLocation),
                                      "Picture files (*.jpg *.png)");

    if (!filenames.isEmpty()) {
        for (auto filename : filenames) {
            mPictureModel->pictureModel()->addPictureFromUrl(QUrl(filename));
        }

        QModelIndex modelIndex = mPictureModel->pictureModel()->index(
            mPictureModel->pictureModel()->rowCount() - 1,
            0);
        listview->setCurrentIndex(modelIndex);
    }
}

void MyListView::delPictures()
{
    if (mPictureSelectionModel->selectedIndexes().isEmpty()) {
        return;
    }

    // mPictureSelectionModel->selectedRows()
    int row = mPictureSelectionModel->currentIndex().row();
    mPictureModel->sourceModel()->removeRow(row);

    //选中前一个图片
    QModelIndex previousModelIndex = mPictureModel->sourceModel()->index(row - 1,
                                                                         0);

    if (previousModelIndex.isValid()) {
        previousModelIndex = mPictureModel->index(previousModelIndex.row(),
                                                  previousModelIndex.column());
        mPictureSelectionModel->setCurrentIndex(previousModelIndex,
                                                QItemSelectionModel::SelectCurrent);
        return;
    }

    //选中后一个图片
    QModelIndex nextModelIndex = mPictureModel->sourceModel()->index(row, 0);

    if (nextModelIndex.isValid()) {
        nextModelIndex = mPictureModel->index(nextModelIndex.row(),
                                              nextModelIndex.column());
        mPictureSelectionModel->setCurrentIndex(nextModelIndex,
                                                QItemSelectionModel::SelectCurrent);
        return;
    }
}

void MyListView::updatePictures()
{
    if (mPictureSelectionModel->selectedIndexes().isEmpty()) {
        return;
    }
    QFileDialog fileDialog(this);

    fileDialog.setAcceptMode(QFileDialog::AcceptOpen);
    fileDialog.setWindowTitle(tr("修改图片"));
    fileDialog.setNameFilter("Picture files (*.jpg *.png)");
    fileDialog.setDirectory(QStandardPaths::standardLocations(QStandardPaths::
                                                              MoviesLocation)
                            .value(0, QDir::homePath()));

    if (fileDialog.exec() == QDialog::Accepted) {
        if (fileDialog.selectedFiles().isEmpty()) return;

        QString file = fileDialog.selectedFiles().constFirst();
        PictureModel *pic_model = (PictureModel *)mPictureModel->sourceModel();
        pic_model->updateData(
            mPictureSelectionModel->selectedIndexes().constFirst().row(),
            QUrl(file));
    }
}

void MyListView::clearPicture()
{
    PictureModel *pic_model = (PictureModel *)mPictureModel->sourceModel();

    pic_model->clearPictures();
}

void MyListView::delCurrentPicture()
{
    if (mCurrentIndex.isValid())
    {
        PictureModel *pic_model = (PictureModel *)mPictureModel->sourceModel();
        pic_model->removeRow(mCurrentIndex.row());
    }
}

void MyListView::showCustomMenu(const QPoint& pos)
{
    QPoint point = pos;

    mCurrentIndex = listview->indexAt(pos); qDebug() << mCurrentIndex;

    if (mCurrentIndex.isValid() && (mCurrentIndex.row() >= 0))
    {
        m_func_menu->exec(listview->mapToGlobal(point));
    }
}

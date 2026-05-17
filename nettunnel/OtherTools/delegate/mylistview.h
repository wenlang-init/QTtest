#ifndef MYLISTVIEW_H
#define MYLISTVIEW_H

#include <QWidget>
#include <QItemSelectionModel>
#include <QMouseEvent>
class QListView;
class PictureProxyModel;
class MyListView : public QWidget {
    Q_OBJECT

public:

    explicit MyListView(QWidget *parent = nullptr);
    ~MyListView();

private slots:

    void addPictures();
    void delPictures();
    void updatePictures();
    void clearPicture();
    void delCurrentPicture();
    void showCustomMenu(const QPoint& pos);

private:

    // 图片数据模型
    PictureProxyModel *mPictureModel;

    //选中元素的数据模型
    QItemSelectionModel *mPictureSelectionModel;
    QModelIndex mCurrentIndex;
    QMenu *m_func_menu = nullptr;
    QAction *m_del_current_pic = nullptr;
    QListView *listview;
};

#endif // MYLISTVIEW_H

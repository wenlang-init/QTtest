#ifndef PICTUREMODEL_H
#define PICTUREMODEL_H
#include <QAbstractListModel>
#include <QUrl>
#include <QPixmap>

struct Picture {
    // class Picture : public QObject {
    //     Q_OBJECT

    // public:

    int      mPictureId;  // 图片ID
    QUrl     mPictureUrl; // 图片的地址
    QPixmap *pixmap;      // 图片数据
    Picture() {
        mPictureId = -1;
        pixmap = nullptr;
    }

    // Picture(QObject *parent = nullptr) : QObject(parent) {
    //     mPictureId = -1;
    //     pixmap = nullptr;

    //     // mPictureUrl
    // }
};

class PictureModel : public QAbstractListModel {
    Q_OBJECT

public:

    // 自定义每个元素的数据类型
    enum Roles {
        UrlRole = Qt::UserRole + 1,
        FilePathRole,
        AllVaule
    };
    Q_ENUM(Roles);
    PictureModel(QObject *parent = nullptr);
    virtual ~PictureModel();

    // 向数据模型中添加单个数据
    QModelIndex      addPicture(const Picture& picture);
    Q_INVOKABLE void addPictureFromUrl(const QUrl& fileUrl);
    bool             updateData(const int & index,
                                const QUrl& fileUrl);

    // 模型的行数
    int      rowCount(const QModelIndex& parent =
                                    QModelIndex()) const override;
    int      rowCount();

    // 获取某个元素的数据
    QVariant data(const QModelIndex& index,
                  int                role) const override;

    //删除某几行数据
    Q_INVOKABLE bool removeRows(int                row,
                                int                count,
                                const QModelIndex& parent =
                                    QModelIndex()) override;

    // 每个元素类别的名称
    QHash<int, QByteArray>roleNames() const override;

    // 加载用户图片
    Q_INVOKABLE void      loadPictures();

    // 清空模型的中的数据,但不移除本地文件数据
    void                  clearPictures();

public slots:

    // 清空模型,删除本地文件中的数据
    void deleteAllPictures();

private:

    QList<Picture>picData;
};

#endif // PICTUREMODEL_H

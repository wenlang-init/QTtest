#include "picturemodel.h"
#include <QMetaEnum>
#include <QFile>
#include <QDebug>
const unsigned int PICTURE_SIZE = 350 * 3;
PictureModel::PictureModel(QObject *parent) : QAbstractListModel(parent)
{}

PictureModel::~PictureModel()
{
    clearPictures();
}

QModelIndex PictureModel::addPicture(const Picture& picture)
{
    int rows = rowCount();

    beginInsertRows(QModelIndex(), rows, rows);
    picData.append(picture);
    endInsertRows();
    return index(rows, 0);
}

void PictureModel::addPictureFromUrl(const QUrl& fileUrl)
{
    Picture picture;

    picture.mPictureUrl = fileUrl;
    QPixmap pixmap(fileUrl.toString());

    // picture.pixmap = new QPixmap(pixmap);
    // picture.pixmap->fill(Qt::transparent); // 用透明色填充
    picture.pixmap = new QPixmap(pixmap.scaled(PICTURE_SIZE, PICTURE_SIZE,
                                               Qt::KeepAspectRatio,
                                               Qt::SmoothTransformation));


    addPicture(picture);
}

bool PictureModel::updateData(const int& index, const QUrl& fileUrl)
{
    if ((index < 0) || (index >= rowCount()))
    {
        return false;
    }

    if (picData[index].pixmap)
    {
        delete picData[index].pixmap;
        picData[index].pixmap = nullptr;
    }
    picData[index].mPictureUrl = fileUrl;
    QPixmap pixmap(fileUrl.toString());
    picData[index].pixmap = new QPixmap(pixmap.scaled(PICTURE_SIZE, PICTURE_SIZE,
                                                      Qt::KeepAspectRatio,
                                                      Qt::SmoothTransformation));

    // beginResetModel();
    // endResetModel();
    // QList<int> roles;
    // roles << Qt::DisplayRole<< Roles::UrlRole<< Roles::FilePathRole<<
    // Roles::AllVaule;
    // emit dataChanged(modelIndex, modelIndex, roles);
    return true;
}

int PictureModel::rowCount(const QModelIndex& parent) const
{
    Q_UNUSED(parent)
    return picData.size();
}

int PictureModel::rowCount()
{
    return picData.size();
}

QVariant PictureModel::data(const QModelIndex& index, int role) const
{
    if (!index.isValid())
    {
        return QVariant();
    }

    const Picture& picture = picData.at(index.row());

    switch (role) {
    // 展示数据为图片的名称
    case Qt::DisplayRole:
        return picture.mPictureUrl.fileName();

        break;

    // 图片的URL
    case Roles::UrlRole:
        return picture.mPictureUrl;

        break;

    // 图片地址
    case Roles::FilePathRole:
        return picture.mPictureUrl.toString(); // toLocalFile();

        break;

    // 所有数据的指针
    case Roles::AllVaule: {
        const Picture *p = &picData[index.row()];
        unsigned long long ptrvaule = (unsigned long long)(p);
        return ptrvaule;
    }

    default:
        return QVariant();
    }
}

bool PictureModel::removeRows(int                row,
                              int                count,
                              const QModelIndex& parent)
{
    if ((row < 0)
        || (row >= rowCount())
        || (count < 0)
        || ((row + count) > rowCount())) {
        return false;
    }
    beginRemoveRows(parent, row, row + count - 1);

    for (int i = row; i < row + count; i++) {
        if (picData[i].pixmap) delete picData[i].pixmap;
    }
    picData.remove(row, count);
    endRemoveRows();
    return true;
}

QHash<int, QByteArray>PictureModel::roleNames() const
{
#if 0
    QHash<int, QByteArray> roles;
    roles[Qt::DisplayRole] = "name";
    roles[Roles::FilePathRole] = "filepath";
    roles[Roles::UrlRole] = "url";
    return roles;

#else // if 0
    QHash<int, QByteArray> roles;
    QMetaEnum metaEnum = QMetaEnum::fromType<Roles>();

    for (int i = 0; i < metaEnum.keyCount(); ++i)
    {
        roles[metaEnum.value(i)] = QByteArray(metaEnum.key(i));
    }

    return roles;

#endif // if 0
}

void PictureModel::loadPictures()
{
    beginResetModel();

    // picData = ; // 读取配置保存记录
    endResetModel();
}

void PictureModel::clearPictures()
{
    beginResetModel();

    for (int i = 0; i < picData.size(); i++) {
        if (picData[i].pixmap) delete picData[i].pixmap;
    }
    picData.clear();
    endResetModel();
}

void PictureModel::deleteAllPictures()
{
    beginResetModel();

    for (int i = 0; i < picData.size(); i++) {
        if (picData[i].pixmap) delete picData[i].pixmap;
        QFile::remove(picData[i].mPictureUrl.toLocalFile());
    }
    picData.clear();
    endResetModel();
}

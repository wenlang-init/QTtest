#include "pictureproxymodel.h"
#include "PictureModel.h"
const unsigned int PICTURE_SIZE = 350;
PictureProxyModel::PictureProxyModel(QObject *parent) :
    QIdentityProxyModel(parent) {}

QVariant PictureProxyModel::data(const QModelIndex& index, int role) const
{
    if (!index.isValid())
    {
        return QVariant();
    }

    // qDebug() << __FILE_NAME__ << __LINE__ << role << index.row();

    // 对原始数据模型中的数据进行二次加工处理
    // 供前端调用
    if (role != Qt::DecorationRole) {
        return QIdentityProxyModel::data(index, role);
    }
    unsigned long long ptrvalue =
        sourceModel()->data(index,
                            PictureModel::Roles::AllVaule).toLongLong();
    Picture *picture = (Picture *)ptrvalue;

    // qDebug() << __FILE_NAME__ << __LINE__ << picture->pixmap->size();
    return *picture->pixmap;
}

void PictureProxyModel::setSourceModel(QAbstractItemModel *sourceModel)
{
    QIdentityProxyModel::setSourceModel(sourceModel);

    if (!sourceModel) {
        return;
    }
    connect(sourceModel, &QAbstractItemModel::modelReset, [this] {
        // static_cast<PictureModel *>(sourceModel())->reset();
    });
    connect(sourceModel, &QAbstractItemModel::rowsInserted,
            [this](const QModelIndex& /*parent*/, int first, int last) {
        // static_cast<PictureModel *>(sourceModel())->inserted();
    });
}

PictureModel * PictureProxyModel::pictureModel() const
{
    return static_cast<PictureModel *>(sourceModel());
}

#ifndef PICTUREPROXYMODEL_H
#define PICTUREPROXYMODEL_H
#include <QIdentityProxyModel>
class PictureModel;

class PictureProxyModel : public QIdentityProxyModel {
public:

    PictureProxyModel(QObject *parent = nullptr);

    // 通过重写data接口对数据进行二次处理
    QVariant data(const QModelIndex& index,
                  int                role) const override;

    // 设置获取源数据模型
    void          setSourceModel(QAbstractItemModel *sourceModel) override;
    PictureModel* pictureModel() const;

private:
};

#endif // PICTUREPROXYMODEL_H

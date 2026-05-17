#ifndef TESTPROXYMODEL_H
#define TESTPROXYMODEL_H

#include <QIdentityProxyModel>

class TestProxyModel : public QIdentityProxyModel {
    Q_OBJECT

public:

    explicit TestProxyModel(QObject *parent = nullptr);

    // 通过重写data接口对数据进行二次处理(根据不同需求进行不同的处理)
    QVariant data(const QModelIndex& index,
                  int                role) const override;

    // 设置获取源数据模型
    void setSourceModel(QAbstractItemModel *sourceModel) override;

    bool addData(QDateTime& messageTime,
                 bool     & isSend,
                 QString  & messageData,
                 QString  & imagename);
    bool updateData(const int& index,
                    QDateTime& messageTime,
                    bool     & isSend,
                    QString  & messageData,
                    QString  & imagename);
    bool deleteData(const int& row);
    void clearData();
    int  rowCount();
    int  rowCount(const QModelIndex& parent) const override;

signals:
};

#endif // TESTPROXYMODEL_H

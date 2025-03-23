#include "listmodel.h"
#include <QMetaEnum>
#include <QMetaObject>

ListModel::ListModel(QObject *parent)
    : QAbstractListModel{parent}
{}

int ListModel::rowCount(const QModelIndex &parent) const
{
    if (parent.isValid())
    {
        return 0;
    }
    return m_modleData.size();
}

QVariant ListModel::data(const QModelIndex &index, int role) const
{
    if (!index.isValid())
    {
        return QVariant();
    }
    if (ListDataRoles::firstRoles <= role && ListDataRoles::lastRoles >= role)
    {
        QHash<int, QByteArray> temp = roleNames();
        QString key =  temp.value(role);

        QMutexLocker locker(const_cast<QMutex*>(&m_modleDataMutex)); // 死锁？
        auto modeldate = m_modleData[index.row()];
        const QMetaObject *theMetaObject = modeldate->metaObject(); //定义一个QMetaObject对象指针，用来获取类classTestClass的相关信息
        int iPropertyCount = theMetaObject->propertyCount();
        for (int i = 1; i < iPropertyCount; i++)
        {
            auto oneProperty = theMetaObject->property(i);
            auto nameT = QString::fromStdString(oneProperty.name());
            if (nameT == key)
            {
                auto tempData = oneProperty.read(modeldate);
                return tempData;
            }
        }
    }

    return QVariant();
}

QHash<int, QByteArray> ListModel::roleNames() const
{
    QHash<int, QByteArray> roles;
    QMetaEnum metaEnum = QMetaEnum::fromType<ListDataRoles>();
    for (int i = 0; i < metaEnum.keyCount(); ++i)
    {
        roles[metaEnum.value(i)] = QByteArray(metaEnum.key(i));
    }

    return roles;
}

bool ListModel::addData(const ListDataInfo &data)
{
    QMutexLocker locker(&m_modleDataMutex);
    beginInsertRows(QModelIndex(), m_modleData.size(), m_modleData.size());
    //beginInsertRows(QModelIndex(), 0,0);
    ListDataInfo *node = new ListDataInfo;
    node->setValue(data);
    m_modleData.append(node);
    //m_modleData.prepend(node);
    endInsertRows();
    return true;
}

bool ListModel::updateData(const int &index, const ListDataInfo &data)
{
    // QMetaObject::invokeMethod(this, [this]() {
    //     beginResetModel();
    //     // 更新数据
    //     endResetModel();
    // }, Qt::AutoConnection);

    QMutexLocker locker(&m_modleDataMutex);
    if (index < 0)
    {
        return false;
    }

    m_modleData[index]->setValue(data);
    QModelIndex modelIndex = this->index(index, 0, QModelIndex());
    locker.unlock();
    emit dataChanged(modelIndex, modelIndex); // 这里会调用data函数?在不同线程发出这个信号应该不会死锁。在这之前解锁暂未出现死锁
    return true;
}

bool ListModel::deleteData(const QModelIndex &index)
{
    QMutexLocker locker(&m_modleDataMutex);
    if (index.isValid())
    {
        return false;
    }
    beginRemoveRows(QModelIndex(), index.row(), index.row());
    ListDataInfo *node = m_modleData.takeAt(index.row());
    endRemoveRows();
    delete node;
    return true;
}

bool ListModel::deleteData(const int &row)
{
    QMutexLocker locker(&m_modleDataMutex);
    if(m_modleData.size() <= row){
        return false;
    }
    beginRemoveRows(QModelIndex(), row, row);
    ListDataInfo *node = m_modleData.takeAt(row);
    endRemoveRows();
    delete node;
    return true;
}

bool ListModel::clearData()
{
    beginResetModel();
    QMutexLocker locker(&m_modleDataMutex);
    qDeleteAll(m_modleData);
    m_modleData.clear();
    locker.unlock();
    endResetModel();
    return true;
}

bool ListModel::getData(const int &row,ListDataInfo &data)
{
    QMutexLocker locker(&m_modleDataMutex);
    if(row < 0 || row >= m_modleData.size()){
        return false;
    }
    data.setValue(*m_modleData[row]);
    return true;
}

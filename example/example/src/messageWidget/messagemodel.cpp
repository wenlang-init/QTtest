#include "messagemodel.h"
#include <QMetaEnum>
#include <QDebug>
#include <QDateTime>
#include <QHash>
#include <QThread>
#define qdebug qDebug().noquote() << "[" << __FILE__ << ":" << __LINE__ << "] " << \
        __FUNCTION__ << "() "


MessageInfo::~MessageInfo() {
    // qdebug;
}

MessageModel::MessageModel(QObject *parent)
    : QAbstractListModel{parent}
{
    // m_dataCache.setMaxCost(1000); // 设置缓存的最大成本（可以根据需要调整）
    // dataWorker *dw = new dataWorker;
    // connect(  dw, &dataWorker::dataReady,     this,
    // &MessageModel::onDataReady);
    // connect(this, &MessageModel::requestData, dw,   &dataWorker::getData);
    // QThread *thread = new QThread(this);
    // dw->moveToThread(thread);
    // thread->start();
}

MessageModel::~MessageModel()
{
    qdebug;
}

int MessageModel::rowCount()
{
    return m_modleData.size();
}

int MessageModel::rowCount(const QModelIndex& parent) const
{
    Q_UNUSED(parent)

    // if (!parent.isValid())
    // {
    //     return 0;
    // }
    return m_modleData.size();
}

// QModelIndex MessageModel::index(int row, int column,
//                                 const QModelIndex& parent) const
// {
// #if 0
//     Q_UNUSED(column)
//     Q_UNUSED(parent)

//     if ((row < 0) || (row >= m_modleData.size()))
//     {
//         return QModelIndex();
//     }
//     return createIndex(row, 0, m_modleData[row]);
// #else
//     if (!hasIndex(row, column, parent) || parent.isValid())
//         return {};
//     return createIndex(row, column);
// #endif
// }

QVariant MessageModel::data(const QModelIndex& index, int role) const
{
    // QDateTime dt = QDateTime::currentDateTime();

    if (!index.isValid())
    {
        return QVariant();
    }

    // if ((Roles::firstRoles <= role) && (Roles::lastRoles >= role))
    QHash<int, QByteArray> temp = roleNames();
    QString key =  temp.value(role);

    if (!key.isEmpty()) {
        // if (m_dataCache.contains(index.row())) {
        //     // 需要注意这里的线程安全问题，保存的obj是在dataWorker线程中的，所以在读取的时候需要加锁,使用OBJ是为了通用性
        //     // 这里如果是确定的数据类型可以直接拷贝一份，使用QChache缓存会很方便，就不需要加锁了
        //     // QMutexLocker locker(lock);
        //     QObject *modeldate = m_dataCache[index.row()].data;
        //     m_dataCache[index.row()].count++; // 更新访问次数
        //     int *v =const_cast<int*>(&m_dataCache[index.row()].count);
        //     (*v)++;
        //     const QMetaObject *theMetaObject = modeldate->metaObject();
        //     int propIndex = theMetaObject->indexOfProperty(
        //         key.toUtf8().constData());

        //     if (propIndex != -1) {
        //         QMetaProperty metaProp = theMetaObject->property(propIndex);
        //         return metaProp.read(modeldate);
        //     }
        // } else {
        //     emit requestData(index.row(), key);
        // }


        // QMutexLocker locker(const_cast<QMutex *>(&m_modleDataMutex));
        QObject *modeldate = m_modleData[index.row()];
        const QMetaObject *theMetaObject = modeldate->metaObject(); // 定义一个QMetaObject对象指针，用来获取类classTestClass的相关信息
        int propIndex = theMetaObject->indexOfProperty(key.toUtf8().constData());

        if (propIndex != -1) {
            QMetaProperty metaProp = theMetaObject->property(propIndex);
            return metaProp.read(modeldate);
        }

        // int iPropertyCount = theMetaObject->propertyCount();
        // for (int i = 1; i < iPropertyCount; i++)
        // {
        //     QMetaProperty oneProperty = theMetaObject->property(i);
        //     QString nameT = QString::fromStdString(oneProperty.name());

        //     if (nameT == key)
        //     {
        //         QVariant tempData = oneProperty.read(modeldate);

        //         // qdebug << dt.msecsTo(QDateTime::currentDateTime());
        //         return tempData;
        //     }
        // }
    }
    return QVariant();
}

bool MessageModel::removeRows(int row, int count, const QModelIndex& parent)
{
    // QMutexLocker locker(&m_modleDataMutex);

    if ((row < 0)
        || (row >= rowCount())
        || (count < 0)
        || ((row + count) > rowCount())) {
        return false;
    }
    beginRemoveRows(parent, row, row + count - 1);

    for (int i = row; i < row + count; i++) {
        if (m_modleData[i]) {
            delete m_modleData[i];
        }
    }
    m_modleData.remove(row, count);
    endRemoveRows();
    return true;
}

QHash<int, QByteArray>MessageModel::roleNames() const
{
    QHash<int, QByteArray> roles;
    QMetaEnum metaEnum = QMetaEnum::fromType<Roles>();

    for (int i = 0; i < metaEnum.keyCount(); ++i)
    {
        roles[metaEnum.value(i)] = QByteArray(metaEnum.key(i));
    }

    return roles;
}

bool MessageModel::addData(QObject *data)
{
    // QMutexLocker locker(&m_modleDataMutex);

    beginInsertRows(QModelIndex(), m_modleData.size(), m_modleData.size());
    m_modleData.append(data);
    endInsertRows();
    return true;
}

bool MessageModel::addData(QList<QObject *>& datas)
{
    if (datas.size() < 1) return false;

    beginInsertRows(QModelIndex(),
                    m_modleData.size(),
                    m_modleData.size() + datas.size() - 1);
    qdebug << datas.size();
    m_modleData.append(datas);
    endInsertRows();
    return true;
}

bool MessageModel::updateData(const int& index, QObject   *data)
{
    // QMutexLocker locker(&m_modleDataMutex);

    if ((index < 0) || (index >= m_modleData.size()))
    {
        return false;
    }

    delete m_modleData[index];
    m_modleData[index] = data;

    // locker.unlock();
    QModelIndex modelIndex = this->index(index, 0, QModelIndex());
    emit dataChanged(modelIndex, modelIndex); // 这里会调用data函数?
    return true;
}

bool MessageModel::deleteData(const QModelIndex& index)
{
    // QMutexLocker locker(&m_modleDataMutex);

    if (index.isValid())
    {
        return false;
    }
    beginRemoveRows(QModelIndex(), index.row(), index.row());
    QObject *node = m_modleData.takeAt(index.row());
    endRemoveRows();
    delete node;
    return true;
}

bool MessageModel::deleteData(const int& row)
{
    // QMutexLocker locker(&m_modleDataMutex);

    if ((row < 0) || (m_modleData.size() <= row)) {
        return false;
    }
    beginRemoveRows(QModelIndex(), row, row);
    QObject *node = m_modleData.takeAt(row);
    endRemoveRows();
    delete node;
    return true;
}

void MessageModel::clearData()
{
    // QMutexLocker locker(&m_modleDataMutex);

    beginResetModel();

    for (int i = 0; i < m_modleData.size(); i++) {
        if (m_modleData[i]) delete m_modleData[i];
    }
    m_modleData.clear();
    endResetModel();
}

QList<QObject *> * MessageModel::getData()
{
    return &m_modleData;
}

QMutex * MessageModel::getMutex()
{
    return &m_modleDataMutex;
}

void MessageModel::onDataReady(QObject *obj, const int& row, QVariant& data)
{
    if (obj != nullptr) {
        if (m_dataCache.contains(row)) {
            m_dataCache[row].data = obj;
        } else {
            CacheNode node;
            node.data = obj;
            m_dataCache[row] = node;
        }

        if (m_dataCache.size() > maxcachesize) {
            // 实现LRU淘汰机制，移除最少使用的缓存项
            int leastUsedRow = -1;
            int leastUsedCount = INT_MAX;

            for (auto it = m_dataCache.begin(); it != m_dataCache.end(); ++it) {
                if (it.value().count < leastUsedCount) {
                    leastUsedRow = it.key();
                    leastUsedCount = it.value().count;
                }
            }

            if (leastUsedRow != -1) {
                m_dataCache.remove(leastUsedRow);
            }
        }
    }

    if (!data.isNull()) {
        // 通知视图刷新这一行
        auto idx = index(row, 0);
        emit dataChanged(idx, idx);
    }
}

void dataWorker::getData(const int& row, const QString& key)
{
    QVariant data;
    QObject *obj = nullptr;

    if (m_dataHash.contains(row)) {
        obj = m_dataHash.value(row);
        const QMetaObject *metaObj = obj->metaObject();
        int propIndex = metaObj->indexOfProperty(key.toUtf8().constData());

        if (propIndex != -1) {
            QMetaProperty metaProp = metaObj->property(propIndex);
            data = metaProp.read(obj);
        }
    }
    emit dataReady(obj, row, data);
}

#ifndef MESSAGEMODEL_H
#define MESSAGEMODEL_H

#include <QAbstractListModel>
#include <QMutex>

#include <QObject>
#include <QImage>
#include <QQuickItem>
#include <QCache>

#define A_PROPERTY(T, NAME, INITVALUE)                                     \
private:                                                                   \
    Q_PROPERTY(T NAME READ NAME WRITE set_ ## NAME NOTIFY NAME ## Changed) \
public:                                                                    \
    const T& NAME() const                                                  \
    {                                                                      \
        return m_ ## NAME;                                                 \
    }                                                                      \
    void set_ ## NAME(const T& value)                                      \
    {                                                                      \
        if (m_ ## NAME == value)                                           \
        return;                                                            \
        m_ ## NAME = value;                                                \
        emit NAME ## Changed (value);                                      \
    }                                                                      \
    Q_SIGNAL void NAME ## Changed(const T& value);                         \
private:                                                                   \
    T m_ ## NAME = INITVALUE;

// 通用模版
class MessageInfo : public QObject {
    Q_OBJECT

    // QML_ANONYMOUS

    // 使用 Q_PROPERTY 宏定义的成员会被元对象系统识别为属性
    // 因为在model中用QMetaProperty::read获取数据的,所以数据要这样定义
    // 是根据变量名查找的，要保证变量名和Roles里的一致的才能找到
    /////////////////////////////////////////
    A_PROPERTY(unsigned long long, messageTime,  0);
    A_PROPERTY(bool,               isSend,   false);
    A_PROPERTY(QString,            messageData, "");

    // 头像名
    A_PROPERTY(QString,            headPortrait, 0);

    // 用于其他数据结构的指针
    A_PROPERTY(unsigned long long, dataPtr,      0);

public:

    explicit MessageInfo(QObject *parent = nullptr) : QObject(parent) {}

    MessageInfo(MessageInfo& data) : QObject() {
        setValue(data);
    }

    virtual void setValue(const MessageInfo& data) {
        set_messageTime(data.messageTime());
        set_isSend(data.isSend());
        set_messageData(data.messageData());
        set_headPortrait(data.headPortrait());
        set_dataPtr(data.dataPtr());
    }

    virtual ~MessageInfo();
};

class MessageModel : public QAbstractListModel {
    Q_OBJECT

public:

    // 自定义每个元素的数据类型
    enum Roles {
        firstRoles = Qt::UserRole + 1,
        messageTime, isSend, messageData, headPortrait,
        dataPtr,
        lastRoles
    };
    Q_ENUM(Roles);
    explicit MessageModel(QObject *parent = nullptr);
    ~MessageModel();
    int rowCount();

    // 模型的行数
    int rowCount(const QModelIndex& parent =
                                    QModelIndex()) const override;

    // QModelIndex index(int                row,
    //                   int                column,
    //                   const QModelIndex& parent = QModelIndex()
    //                   ) const override;

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

    bool                  addData(QObject *data);
    bool                  addData(QList<QObject *>& datas);
    bool                  updateData(const int& index,
                                     QObject   *data);
    bool                  deleteData(const QModelIndex& index);
    bool                  deleteData(const int& row);
    void                  clearData();

    QList<QObject *>    * getData();
    QMutex              * getMutex();

signals:

    // void addDataSignal(QObject *obj);
    // void addDataSignal(QHash<int, QObject *>data);
    // void insertDataSignal(const int& index, QObject *data);
    // void updateDataSignal(const int& index, QObject *);
    // void deleteDataSignal(const int& index,int count=1);
    // void clearDataSignal();
    void requestData(const int    & index,
                     const QString& key)const;

protected slots:

    void onDataReady(QObject   *obj,
                     const int& index,
                     QVariant & data);

private:

    QMutex m_modleDataMutex;

    QList<QObject *>m_modleData;

    struct CacheNode {
        int      count;
        QObject *data;
        CacheNode() : count(0), data(nullptr) {}
    };

    // LRU（最近最少使用）淘汰机制的缓存，需要去实现一个LRUCache类来管理这个缓存
    mutable QHash<int, CacheNode>m_dataCache;

    // QHash<int, CacheNode>m_dataCache;
    int maxcachesize = 100; // 定义一个最大缓存大小
    int m_count = 0;
};

class dataWorker : public QObject {
    Q_OBJECT

public:

    explicit dataWorker(QObject *parent = nullptr) : QObject(parent) {}

public slots:

    // void addData(QObject *obj);
    // void addDataSignal(QHash<int, QObject *>data);
    // void insertData(const int& index, QObject *data);
    // void updateData(const int& index, QObject *);
    // void deleteData(const int& index,int count=1);
    // void clearData();
    void getData(const int    & row,
                 const QString& key);

signals:

    void dataReady(QObject   *obj,
                   const int& index,
                   QVariant & data);

private:

    QHash<int, QObject *>m_dataHash;
};

#endif // MESSAGEMODEL_H

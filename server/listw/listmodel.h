#ifndef LISTMODEL_H
#define LISTMODEL_H

#include <QAbstractListModel>
#include <QMutex>

#define A_PROPERTY(T,NAME,INITVALUE) \
private:\
Q_PROPERTY(T NAME READ NAME WRITE set_##NAME NOTIFY NAME##Changed)\
public:\
const T& NAME() const\
{\
    return m_##NAME;\
}\
void set_##NAME(const T& value)\
{\
    if(m_##NAME == value)\
        return;\
    m_##NAME = value;\
    emit NAME##Changed(value);\
}\
Q_SIGNAL void NAME##Changed(const T& value);\
private:\
T m_##NAME = INITVALUE;

class ListDataInfo : public QObject{
    Q_OBJECT

    /////////////////////////////////////////
    A_PROPERTY(int,t1,0);
    A_PROPERTY(bool,t2,false);
    A_PROPERTY(QString,t3,"");

public:
    explicit ListDataInfo(QObject *parent = nullptr):QObject(parent){}
    void setValue(const ListDataInfo &data){
        m_t1 = data.t1();
        m_t2 = data.t2();
        m_t3 = data.t3();
    }
    // int t1;
    // bool t2;
    // QString t3;
};

class ListModel : public QAbstractListModel
{
    Q_OBJECT

    A_PROPERTY(int,currentIndext,-1);
public:
    enum ListDataRoles {
        firstRoles = Qt::UserRole+1,
        t1,t2,t3,
        lastRoles
    };
    Q_ENUM(ListDataRoles);

    explicit ListModel(QObject *parent = nullptr);

    int rowCount(const QModelIndex &parent = QModelIndex()) const override;
    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const override;
    QHash<int, QByteArray> roleNames() const override;

    bool addData(const ListDataInfo &data);
    bool updateData(const int &index,const ListDataInfo &data);
    bool deleteData(const QModelIndex &index);
    bool deleteData(const int &row);
    bool clearData();

    bool getData(const int &row,ListDataInfo &data);

signals:
private:
    QMutex m_modleDataMutex;
    QList<ListDataInfo*> m_modleData;
};

#endif // LISTMODEL_H

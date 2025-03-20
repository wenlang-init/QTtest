#include "sql_engine.h"
#include <QDebug>
#include <QTime>
#include <QThread>
#define qdebug qDebug().noquote()<<__FILE__<<__LINE__<<__FUNCTION__

Sql_Engine::Sql_Engine() : _sql_db(nullptr)
{
    qdebug<<QThread::currentThreadId()<<QSqlDatabase::drivers();
}

Sql_Engine::~Sql_Engine()
{
    qdebug<<"xxxxxxxxxxxxxxxxxxxxxxxxxxxx"<<this;
    closeDb();
}

bool Sql_Engine::openDb(const QString &txt)
{
    if(_sql_db == nullptr){
        connectionName = txt.split("/").last() + "." + QString::number(QDateTime::currentDateTime().toMSecsSinceEpoch());
        //qdebug << connectionName << QSqlDatabase::connectionNames();
        // 使用驱动程序类型和连接名称connectionName将数据库添加到数据库连接列表中。如果已经存在名为connectionName的数据库连接，则删除该连接
        _sql_db = new QSqlDatabase(QSqlDatabase::addDatabase("QSQLITE",connectionName));
        //_sql_db = new QSqlDatabase(QSqlDatabase::addDatabase("QODBC",connectionName));
        //_sql_db = new QSqlDatabase(QSqlDatabase::addDatabase("QPSQL",connectionName));
        //_sql_db = new QSqlDatabase(QSqlDatabase::addDatabase("QMYSQL",connectionName));
        _sql_db->setDatabaseName(txt); // 设置数据库文件
        //_sql_db->setHostName("127.0.0.1");
        //_sql_db->setPort(3306);
        //_sql_db->setUserName("root");
        //_sql_db->setPassword("123456");
    }
    if(_sql_db->isOpen()){
        return true;
    }
    if(false == _sql_db->open())
    {
        qdebug << "open faile, err=" << _sql_db->lastError().text();
        return false;
    }
    QStringList tables = _sql_db->tables();
    qdebug << tables;
    return true;
}

void Sql_Engine::closeDb()
{
    if(_sql_db == nullptr)return;
    _sql_db->close();
    delete _sql_db;
    _sql_db = nullptr;

    QSqlDatabase::removeDatabase(connectionName);
}

bool Sql_Engine::isOpen()
{
    if(_sql_db == nullptr)return false;
    return _sql_db->isOpen();
}

QSqlDatabase *Sql_Engine::getQSqlDatabase()
{
    return _sql_db;
}

bool Sql_Engine::isExistTable(const QString &tablename)
{
    if(isOpen() == false)return false;
    QSqlQuery devconf(*_sql_db);
    if(_sql_db->tables().contains(tablename)){
        return true;
    }
    return false;
}

bool Sql_Engine::createTable(const QString &tablename,const QString &createtablecmd,bool iscover)
{
    if(isOpen() == false)return false;

    QSqlQuery devconf(*_sql_db);
    if(_sql_db->tables().contains(tablename)){
        if(iscover == false){
            qdebug << QString("table %1 already existed").arg(tablename);
            return false;
        } else {
            if(false == dropTable(tablename)){
                return false;
            }
        }
    }
    if(false == devconf.prepare(createtablecmd)){
        qdebug << "prepare faile, err=" << _sql_db->lastError().text();
        return false;
    }
    if(false == devconf.exec()){
        return false;
    }

    return true;
}

bool Sql_Engine::dropTable(const QString &tablename)
{
    if(isOpen() == false)return false;

    QStringList tables = _sql_db->tables();
    if(!tables.contains(tablename)) return true;

    QString cmd = QString("drop table %1").arg(tablename);
    QSqlQuery query(*_sql_db);
    if(false == query.exec(cmd)){
        qdebug<<QString("%1, err=%2").arg(cmd).arg(_sql_db->lastError().text());
        return false;
    }
    return true;
}

bool Sql_Engine::getAllTablename(QStringList &tablenames)
{
    if(isOpen() == false)return false;

    tablenames = _sql_db->tables();

    return true;
}

bool Sql_Engine::queryTabbleInfo(const QString &tablename,QMap<QString,QString> &tableinfo)
{
    if(isOpen() == false)return false;

    QString cmd = QString("pragma table_info(%1)").arg(tablename);
    QSqlQuery query_data(*_sql_db);//_sql_db->transaction();_sql_db->commit();
    query_data.setForwardOnly(true); // 禁用缓存
    //QTime t;t.start();<t.elapsed();
    if(!query_data.exec(cmd))
    {
        qdebug << QString(cmd) << query_data.lastError().text();
        return false;
    }
    else
    {
        //QVariant::int "cid" "0"
        //QVariant::QString "name" ""
        //QVariant::QString "type" ""
        //QVariant::int "notnull" "0"
        //QVariant::Invalid "dflt_value" ""
        //QVariant::int "pk" "0"
        //QSqlRecord qsqlrd = query_data.record();qdebug<<qsqlrd.count();for(int i=0;i<qsqlrd.count();i++) qdebug<<qsqlrd.field(i)<<qsqlrd.fieldName(i)<<qsqlrd.value(i).toString();
        while(query_data.next()) {
            tableinfo[query_data.value("name").toString()] = query_data.value("type").toString();
        }
    }

    return true;
}

long long Sql_Engine::queryTableRowCount(const QString &tablename)
{
    if(isOpen() == false)return -1;

    QSqlQuery query_data(*_sql_db);
    query_data.setForwardOnly(true); // 禁用缓存
    QString cmd = QString("select count(*) from %1").arg(tablename);
    if(!query_data.exec(cmd))
    {
        qdebug << QString(cmd) << query_data.lastError().text();
        return -2;
    } else {
        if(query_data.next()){
            long long count = query_data.value("count(*)").toLongLong();
            return count;
        } else {
            return -3;
        }
    }
}

bool Sql_Engine::executeSingleCmd(const QString &cmd)
{
    if(isOpen() == false)return false;

    QSqlQuery query_data(*_sql_db);
    //query_data.setForwardOnly(true); // 禁用缓存
    //QTime t;t.start();<t.elapsed();
    if(!query_data.exec(cmd))
    {
        qdebug << QString(cmd) << query_data.lastError().text();
        return false;
    }

    return true;
}

bool Sql_Engine::executeMultitudeCmd(const QStringList &cmds)
{
    if(isOpen() == false)return false;

    QSqlQuery query_data(*_sql_db);
    _sql_db->transaction();
    for(int i=0;i<cmds.size();i++){
        if(!query_data.exec(cmds[i]))
        {
            qdebug << QString(cmds[i]) << query_data.lastError().text();
            //return false;
        }
    }

    if(false == _sql_db->commit()){
        qdebug << query_data.lastError().text();
        return false;
    }

    return true;
}

void Sql_Engine::sqrtparams(QList<QString> &params, QList<int> &index)
{
    QStringList params_temp = params;
    int left = 0;
    int right = params.size() - 1;
    int lastSwapIndex = 0;      // 记录最后一次交换的位置
    bool hasSwap = false;    // 标志位
    QString temp;
    while (left < right) {
        for (int i = left; i < right; i++) { // 保证 a[right] 是最大的
            if (params[i] > params[i+1]) {
                temp = params[i];
                params[i] = params[i+1];
                params[i+1] = temp;

                hasSwap = true;
                lastSwapIndex = i;
            }
        }
        right = lastSwapIndex;  // 将最后一次交换的位置作为右边界
        if (!hasSwap) { // 上一轮没有交换，提前结束
            break;
        }
        hasSwap = false;
        for (int i = right; i > left; i--) { // 保证 a[left] 是最小的
            if (params[i] < params[i-1]) {
                temp = params[i];
                params[i] = params[i-1];
                params[i-1] = temp;

                hasSwap = true;
                lastSwapIndex = i;
            }
        }
        left = lastSwapIndex;  // 将最后一次交换的位置作为左边界
        if (!hasSwap) { // 上一轮没有交换，提前结束
            break;
        }
        hasSwap = false;
    }
    index.clear();
    for(int i=0;i<params.size();i++){
        for(int j=0;j<params_temp.size();j++){
            if(params[i] == params_temp[j]){
                index.append(j);
                break;
            }
        }
    }
    if(index.size() != params.size()){
        index.clear();
    }
}

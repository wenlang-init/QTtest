#include "testsql.h"
#include <QApplication>
#include <QDebug>
#include <QTime>

#define qdebug qDebug().noquote()<<__FILE__<<__LINE__<<__FUNCTION__

TestSql::TestSql(QObject *parent): QObject(parent)
{

}

bool TestSql::initial()
{
    QString sqlname = QCoreApplication::applicationDirPath() + "/testsql.db";
    if(false == openDb(sqlname)){
        return false;
    }
    QString tablename = "testtablename";
    QString cmd = QString("create table %1 "
                          "(id integer primary key,"
                          "label varchar(256),"
                          "valuei integer,"
                          "valuef real,"
                          "datetime timestamp,"
                          "datetimei bigint)").arg(tablename);
    if(false == isExistTable(tablename) && false == createTable(tablename,cmd)){
        return false;
    }
    QStringList tablenames;
    getAllTablename(tablenames);
    qdebug<<tablenames;
    QMap<QString,QString> tableinfo;
    queryTabbleInfo(tablename,tableinfo);
    qdebug<<tableinfo;
    return true;
}

bool TestSql::insertData(const QString &label, const int &valuei, const double &valuef, const unsigned long long &datetime)
{
    if(isOpen() == false)return false;

    QString cmd = QString("insert into %1 (label,valuei,valuef,datetime,datetimei) "
                          "values('%2','%3','%4','%5','%6')")
                      .arg("testtablename").arg(label).arg(valuei).arg(valuef).arg(datetime).arg(datetime);
    return executeSingleCmd(cmd);
}

bool TestSql::insertDatas(const QList<TestData> &datas)
{
    if(isOpen() == false)return false;

    QStringList cmds;
    for(int i=0;i<datas.size();i++){
        QString cmd = QString("insert into %1 (label,valuei,valuef,datetime,datetimei) "
                              "values('%2','%3','%4','%5','%6')")
                          .arg("testtablename").arg(datas[i].label).arg(datas[i].valuei).arg(datas[i].valuef).arg(datas[i].datetime).arg(datas[i].datetime);
        cmds.append(cmd);
    }

    return executeMultitudeCmd(cmds);
}

bool TestSql::updateData(const int &id, const QString &label, const int &valuei, const double &valuef, const unsigned long long &datetime)
{
    if(isOpen() == false)return false;

    QString cmd = QString("update %1 set label='%2',valuei=%3,valuef=%4,datetime=%5,datetimei=%6 where id=%7")
                      .arg("testtablename").arg(label).arg(valuei).arg(valuef).arg(datetime).arg(datetime).arg(id);
    return executeSingleCmd(cmd);
}

bool TestSql::updateDatas(const int &id, const QList<TestData> &datas)
{
    if(isOpen() == false)return false;

    QStringList cmds;
    for(int i=0;i<datas.size();i++){
        QString cmd = QString("update %1 set label='%2',valuei=%3,valuef=%4,datetime=%5,datetimei=%6 where id=%7")
        .arg("testtablename").arg(datas[i].label).arg(datas[i].valuei).arg(datas[i].valuef).arg(datas[i].datetime).arg(datas[i].datetime).arg(id);
        cmds.append(cmd);
    }

    return executeMultitudeCmd(cmds);
}

bool TestSql::deleteDataIsId(const int &id)
{
    if(isOpen() == false)return false;

    QString cmd = QString("delete from %1 where id=%2")
                      .arg("testtablename").arg(id);
    return executeSingleCmd(cmd);
}

bool TestSql::queryAllData(QList<TestData> &datas)
{
    if(isOpen() == false)return false;

    QString cmd = QString("select * from %1").arg("testtablename");
    QSqlQuery query(*getQSqlDatabase());
    query.setForwardOnly(true); // 禁用缓存
    //QTime t = QTime::currentTime();
    if(false == query.exec(cmd)){
        qdebug<<QString("%1, err=%2").arg(cmd).arg(getQSqlDatabase()->lastError().text());
        return false;
    }
    //qdebug<<t.msecsTo(QTime::currentTime());
    //t = QTime::currentTime();

    while (query.next())
    {
        TestData data;
        data.id = query.value("id").toInt();
        data.label = query.value("label").toString();
        data.valuei = query.value("valuei").toInt();
        data.valuef = query.value("valuef").toDouble();
        data.datetime = query.value("datetimei").toULongLong();
        datas.append(data);
    }
    //qdebug<<t.msecsTo(QTime::currentTime());

    return true;
}

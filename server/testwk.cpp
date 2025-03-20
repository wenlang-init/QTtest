#include "testwk.h"
#include "sql/testsql.h"
#include <QDebug>
#include <QTime>

#define qdebug qDebug().noquote()<<__FILE__<<__LINE__<<__FUNCTION__

TestWk::TestWk(QThread *parent)
    : QThread{parent},isrun{false}
{}

TestWk::~TestWk()
{
    stoprun();
}

void TestWk::startrun()
{
    isrun = true;
    start();
}

void TestWk::stoprun()
{
    isrun = false;
    quit();
    wait();
}

void TestWk::run()
{
    TestSql *sql = new TestSql;
    sql->initial();
    //sql->deleteLater();
    int step = 1000; // 1000ms
    while(isrun){
        QTime t = QTime::currentTime();
        QList<TestData> datas;
        for(int i=0;i<1000*1;i++){
            quint64 ms = QDateTime::currentDateTime().toMSecsSinceEpoch();
            QString label = QString("label.%1").arg(ms);
            int valuei = rand();
            double valuef = rand()*1.0/valuei;
            unsigned long long datetime = ms;

            //sql->insertData(label,valuei,valuef,datetime);

            TestData data;
            data.label = label;
            data.valuei = valuei;
            data.valuef = valuef;
            data.datetime = datetime;
            datas.append(data);
        }

        qdebug << t.msecsTo(QTime::currentTime());
        sql->insertDatas(datas);
        qdebug << t.msecsTo(QTime::currentTime());
        //qdebug << sql->queryTableRowCount("testtablename");
        //qdebug << t.msecsTo(QTime::currentTime());

        //QList<TestData> datasq;sql->queryAllData(datasq);qdebug<<datasq.size();
        int tt = t.msecsTo(QTime::currentTime());
        if(tt < step){
            msleep(step - tt);
        }
        qdebug << tt << t << QTime::currentTime();
    }

    delete sql;
}

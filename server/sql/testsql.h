#ifndef TESTSQL_H
#define TESTSQL_H

#include "sql_engine.h"
#include <QObject>

struct TestData{
    int                 id;
    QString             label;
    int                 valuei;
    double              valuef;
    unsigned long long  datetime;
    TestData(){
        id = 0;
        label = "";
        valuei = 0;
        valuef = 0;
        datetime = 0;
    }
};

class TestSql : public QObject,public Sql_Engine
{
    Q_OBJECT
public:
    explicit TestSql(QObject *parent = nullptr);

    bool initial();

    bool insertData(const QString &label,const int &valuei,const double &valuef,const unsigned long long &datetime);
    bool insertDatas(const QList<TestData> &datas);

    bool updateData(const int &id,const QString &label,const int &valuei,const double &valuef,const unsigned long long &datetime);
    bool updateDatas(const int &id,const QList<TestData> &datas);

    bool deleteDataIsId(const int &id);

    bool queryAllData(QList<TestData> &datas);
};

#endif // TESTSQL_H

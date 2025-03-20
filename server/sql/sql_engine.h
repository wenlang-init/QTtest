#ifndef SQL_ENGINE_H
#define SQL_ENGINE_H

#include <QObject>
#include <QtSql/QSqlDatabase>
#include <QtSql/QSqlError>
#include <QtSql/QSqlQuery>
#include <QtSql/QSqlRecord>
#include <QtSql/QSqlField>
//#include <QtSql/QSql>

class Sql_Engine
{
public:
    explicit Sql_Engine();
    //static Sql_Engine& instance(){static Sql_Engine sqlengine;return sqlengine;}
    virtual ~Sql_Engine();
    // 打开数据库。数据库文件,表名
    bool openDb(const QString &txt);
    void closeDb();
    bool isOpen();
    QSqlDatabase *getQSqlDatabase();
    // 是否存在表
    bool isExistTable(const QString &tablename);
    // 创建表
    bool createTable(const QString &tablename,const QString &createtablecmd,bool iscover = false);
    // 删除表
    bool dropTable(const QString &tablename);
    // 获取所有表名
    bool getAllTablename(QStringList &tablenames);
    // 查询表信息;列名,类型
    bool queryTabbleInfo(const QString &tablename,QMap<QString,QString> &tableinfo);
    // 查询表行数
    long long queryTableRowCount(const QString &tablename);
    // 执行1条指令
    bool executeSingleCmd(const QString &cmd);
    // 执行多条指令
    bool executeMultitudeCmd(const QStringList &cmds);


protected:
    // 升序
    void sqrtparams(QList<QString> &params,QList<int> &index);

private:
    QSqlDatabase *_sql_db;
    QString connectionName;
};

#endif // SQL_ENGINE_H

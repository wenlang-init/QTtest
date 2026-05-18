#ifndef QBREAKPADHANDLER_H
#define QBREAKPADHANDLER_H


#include <string>
#include <QString>
#include <QUrl>
#include "singletone/singleton.h"

namespace google_breakpad {
    class ExceptionHandler;
    class MinidumpDescriptor;
}
class QBreakpadHandlerPrivate;
class QBreakpadHandler: public QObject
{
    Q_OBJECT
public:
    static QString version();

    QBreakpadHandler();
    ~QBreakpadHandler();

    QString uploadUrl() const;
    QString dumpPath() const;
    QStringList dumpFileList() const;

    void setDumpPath(const QString& path);
    void setUploadUrl(const QUrl& url);
    std::string helloworld();
public slots:
    void sendDumps();

private:
    QBreakpadHandlerPrivate* d;
    #define QBreakpadInstance Singleton<QBreakpadHandler>::instance()
};

#endif // QBREAKPADHANDLER_H

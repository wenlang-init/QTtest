#ifndef USECODE_H
#define USECODE_H

#include <QObject>

class useCode : public QObject {
    Q_OBJECT

public:

    explicit useCode(QObject *parent = nullptr);
    QByteArray getCode(QByteArray    sdata,
                       unsigned char enkey);

    QByteArray getDecode(QByteArray sdata);
#if !defined(Q_OS_ANDROID)
    QByteArray GzipCompress(QByteArray postBody);
    QByteArray GZipUnCompress(QByteArray src);
    bool       GzipCompress(const QString& srcFilename,
                            const QString& destFilename);
    bool       GZipUnCompress(const QString& srcFilename,
                              const QString& destFilename);
#endif // if !defined(Q_OS_ANDROID)

signals:
};

#endif // USECODE_H

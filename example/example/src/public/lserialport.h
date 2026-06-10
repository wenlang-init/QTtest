#ifndef LSERIALPORT_H
#define LSERIALPORT_H

#include <QObject>
#include <QVector>

#if (QT_VERSION >= QT_VERSION_CHECK(5, 0, 0))
class QSerialPort;
#endif // if (QT_VERSION >= QT_VERSION_CHECK(5, 0, 0))

class LSerialPort : public QObject {
    Q_OBJECT

public:

    explicit LSerialPort(QObject *parent = NULL);
    ~LSerialPort();

    int recv(char        *buf,
             unsigned int size);
    int send(char        *buf,
             unsigned int size);

    /* *
     * name :串口名
     * pRate:波特率
     * databit:数据位,8,7,6,5
     * stopbit:停止位,1,1.5,2
     * parity：校验位,n,e,o
     * */
    int init_serial(const char  *name,
                    unsigned int pRate = 115200,
                    int          databit = 8,
                    const char  *stopbit = "1",
                    char         parity = 'N',
                    bool         block = false);

#if (QT_VERSION >= QT_VERSION_CHECK(5, 0, 0))
    static QStringList get_com_list();
#endif // if (QT_VERSION >= QT_VERSION_CHECK(5, 0, 0))
    QString            err_string();

protected:

#if (QT_VERSION < QT_VERSION_CHECK(5, 0, 0))
# ifdef __linux__
    void setBaudrate(struct termios *opt,
                     unsigned int    baudrate);
    void set_data_bit(struct termios *opt,
                      unsigned int    databit);
    void set_parity(struct termios *opt,
                    char            parity);
    void set_stopbit(struct termios *opt,
                     const char     *stopbit);
    int  set_port_attr(int         fd,
                       int         baudrate,
                       int         databit,
                       const char *stopbit,
                       char        parity,
                       int         vtime,
                       int         vmin);
# endif // ifdef __linux__

#endif // if (QT_VERSION < QT_VERSION_CHECK(5, 0, 0))

private:

#if (QT_VERSION >= QT_VERSION_CHECK(5, 0, 0))
    QSerialPort * serialport;
#else // if (QT_VERSION >= QT_VERSION_CHECK(5, 0, 0))
# ifdef __linux__
    int serialfd;
# endif // ifdef __linux__

#endif // if (QT_VERSION >= QT_VERSION_CHECK(5, 0, 0))

    bool m_block; // 阻塞

signals:

    void recv_serial_data(QByteArray data);

public slots:
};

#endif // LSERIALPORT_H

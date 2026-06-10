#include "lserialport.h"
#include <QDebug>

#if (QT_VERSION >= QT_VERSION_CHECK(5, 0, 0))
# include <QSerialPort>
# include <QSerialPortInfo> // 本机可用串口列表
#else // if (QT_VERSION >= QT_VERSION_CHECK(5, 0, 0))

# ifdef __linux__
#  include <termios.h>

// #include <sys/select.h>
// #include <unistd.h>
#  include <fcntl.h>
#  include <errno.h>
# endif // ifdef __linux__

#endif  // if (QT_VERSION >= QT_VERSION_CHECK(5, 0, 0))

LSerialPort::LSerialPort(QObject *parent) :
    QObject(parent)
{
    m_block = false;

#if (QT_VERSION >= QT_VERSION_CHECK(5, 0, 0))
    serialport = new QSerialPort(this);
#else // if (QT_VERSION >= QT_VERSION_CHECK(5, 0, 0))

# ifdef __linux__
    serialfd = -1;
# endif // ifdef __linux__

#endif  // if (QT_VERSION >= QT_VERSION_CHECK(5, 0, 0))
}

LSerialPort::~LSerialPort()
{
#if (QT_VERSION >= QT_VERSION_CHECK(5, 0, 0))
    serialport->close();
    serialport->deleteLater();
#else // if (QT_VERSION >= QT_VERSION_CHECK(5, 0, 0))
# ifdef __linux__

    if (serialfd > 0) {
        close(serialfd);
    }
# endif // ifdef __linux__
#endif  // if (QT_VERSION >= QT_VERSION_CHECK(5, 0, 0))
}

int LSerialPort::recv(char *buf, unsigned int size)
{
#if (QT_VERSION >= QT_VERSION_CHECK(5, 0, 0))

    if (serialport->isReadable()) {
        quint64 recv_cnt = serialport->bytesAvailable();
        unsigned int ret = 0;
        int cnt = 0;

        if (m_block == true) {
            while (ret < size) {
                cnt = serialport->read(buf + ret, size - ret);
                serialport->waitForReadyRead();

                if (cnt > 0) {
                    ret += cnt;
                }
            }
        } else {
            if (recv_cnt <= 0) return 0;

            if (recv_cnt < size) {
                size = recv_cnt;
            }
            cnt = serialport->read(buf, size);

            if (cnt < 0) {
                qDebug() << "recv-read:" << serialport->errorString();
            } else {
                ret = cnt;
            }
        }
        return ret;
    } else {
        return 0;
    }
#else // if (QT_VERSION >= QT_VERSION_CHECK(5, 0, 0))
# ifdef __linux__

    if (serialfd < 0) {
        return 0;
    }
    int ret = read(serialfd, buf, size);

    if (ret < 0) {
#  if defined(__ARM_EABI__)

        qDebug() << "recv-read:" << strerror(errno);
#  endif // if defined(__ARM_EABI__)
    }
    return ret;

# endif // ifdef __linux__
#endif  // if (QT_VERSION >= QT_VERSION_CHECK(5, 0, 0))
}

int LSerialPort::send(char *buf, unsigned int size)
{
#if (QT_VERSION >= QT_VERSION_CHECK(5, 0, 0))

    if (serialport->isWritable()) {
        unsigned int ret = 0;
        int cnt = 0;

        while (ret < size) {
            cnt = serialport->write(buf + ret, size - ret);

            if (cnt == 0) {
                break;
            } else if (cnt < 0) {
                qDebug() << "send-write:" << serialport->errorString();
                break;
            }
            ret += cnt;
        }
        return ret;
    } else {
        return 0;
    }
#else // if (QT_VERSION >= QT_VERSION_CHECK(5, 0, 0))
# ifdef __linux__

    if (serialfd < 0) {
        return 0;
    }
    unsigned int ret = 0;
    int cnt;

    while (ret < size) {
        cnt = write(serialfd, buf + ret, size - ret);

        if (cnt == 0) {
            break;
        } else if (cnt < 0) {
            qDebug() << "recv-write:" << strerror(errno);
            break;
        }
        ret += cnt;
    }

    return ret;

# endif // ifdef __linux__
#endif  // if (QT_VERSION >= QT_VERSION_CHECK(5, 0, 0))
}

int LSerialPort::init_serial(const char  *name,
                             unsigned int pRate,
                             int          databi,
                             const char  *stopbit,
                             char         parity,
                             bool         block)
{
    m_block = block;

#if (QT_VERSION >= QT_VERSION_CHECK(5, 0, 0))
    serialport->setPortName(QLatin1String(name));

    if (false == serialport->setBaudRate(pRate)) {
        qDebug() << "open:" << name << serialport->errorString();
        return -1;
    }

    if (false == serialport->setDataBits((QSerialPort::DataBits)databi)) {
        qDebug() << "open:" << name << serialport->errorString();
        return -1;
    }

    QSerialPort::StopBits mstopbit = QSerialPort::OneStop;

    if (0 == strcmp(stopbit, "1"))
    {
        mstopbit = QSerialPort::OneStop;
    }
    else if (0 == strcmp(stopbit, "1.5"))
    {
        mstopbit = QSerialPort::OneAndHalfStop;
    }
    else if (0 == strcmp(stopbit, "2"))
    {
        mstopbit = QSerialPort::TwoStop;
    }
    else
    {
        mstopbit = QSerialPort::OneStop;
    }

    if (false == serialport->setStopBits(mstopbit)) {
        qDebug() << "open:" << name << serialport->errorString();
        return -1;
    }

    QSerialPort::Parity mparity = QSerialPort::NoParity;

    switch (parity)
    {
    case 'E':
    case 'e':
        mparity = QSerialPort::EvenParity;
        break;

    case 'O':
    case 'o':
        mparity = QSerialPort::OddParity;
        break;

    case 'S':
    case 's':
        mparity = QSerialPort::SpaceParity;
        break;

    case 'M':
    case 'm':
        mparity = QSerialPort::MarkParity;
        break;

    case 'N':
    case 'n':
    default:
        mparity = QSerialPort::NoParity;
        break;
    }

    if (false == serialport->setParity(mparity)) {
        qDebug() << "open:" << name << serialport->errorString();
        return -1;
    }

    // 无控制位
    if (false == serialport->setFlowControl(QSerialPort::NoFlowControl)) {
        qDebug() << "open:" << name << serialport->errorString();
        return -1;
    }

    if (true == serialport->open(QIODevice::ReadWrite)) {
        return 0;
    } else {
        qDebug() << "open:" << name << serialport->errorString();
        return -1;
    }
#else // if (QT_VERSION >= QT_VERSION_CHECK(5, 0, 0))
# ifdef __linux__

    // O_NDELAY，O_NONBLOCK都是设置非阻塞；前者读操作无数据返回0。后者返回-1并设置errno为EAGAIN
    // int fd = open(name,O_RDWR|O_NOCTTY|O_NDELAY|O_NONBLOCK); // 读写，非控制终端，非阻塞
    // int fd = open(name,O_RDWR|O_NOCTTY); // 阻塞
    int fd;

    if (block == false) {
        fd = open(name, O_RDWR | O_NOCTTY | O_NDELAY | O_NONBLOCK);
    } else {
        fd = open(name, O_RDWR | O_NOCTTY);
    }
    serialfd = fd;

    if (fd < 0) {
        qDebug() << "open:" << name << strerror(errno);
        return serialfd;
    }

    // fcntl(fd,F_SETFL,FNDELAY|O_NONBLOCK); // 非阻塞

    unsigned int result = B115200;

    switch (pRate) {
    case 0:
        result = B0;
        break;

    case 50:
        result = B50;
        break;

    case 75:
        result = B75;
        break;

    case 110:
        result = B110;
        break;

    case 134:
        result = B134;
        break;

    case 150:
        result = B150;
        break;

    case 200:
        result = B200;
        break;

    case 300:
        result = B300;
        break;

    case 600:
        result = B600;
        break;

    case 1200:
        result = B1200;
        break;

    case 1800:
        result = B1800;
        break;

    case 2400:
        result = B2400;
        break;

    case 4800:
        result = B4800;
        break;

    case 9600:
        result = B9600;
        break;

    case 19200:
        result = B19200;
        break;

    case 38400:
        result = B38400;
        break;

    case 57600:
        result = B57600;
        break;

    case 115200:
        result = B115200;
        break;

    case 230400:
        result = B230400;
        break;

    case 460800:
        result = B460800;
        break;

    case 500000:
        result = B500000;
        break;

    case 576000:
        result = B576000;
        break;

    case 921600:
        result = B921600;
        break;

    case 1000000:
        result = B1000000;
        break;

    case 1152000:
        result = B1152000;
        break;

    case 1500000:
        result = B1500000;
        break;

    case 2000000:
        result = B2000000;
        break;

    case 2500000:
        result = B2500000;
        break;

    case 3000000:
        result = B3000000;
        break;

    case 3500000:
        result = B3500000;
        break;

    case 4000000:
        result = B4000000;
        break;

    default:
        result = B115200;
        break;
    }

    if (set_port_attr(fd, result, databi, stopbit, parity, 150, 255)) {
        qDebug() << "tcgetattr:" << name << strerror(errno);
        return serialfd;
    }


    qDebug() << "init %s success:"
             << name,
             << pRate,
             << databi,
             << stopbit,
             << parity;

    return serialfd;

# endif // ifdef __linux__
#endif  // if (QT_VERSION >= QT_VERSION_CHECK(5, 0, 0))
}

#if (QT_VERSION >= QT_VERSION_CHECK(5, 0, 0))
QStringList LSerialPort::get_com_list()
{
    QStringList strlist;
    QSerialPortInfo info;

    foreach(const QSerialPortInfo& info, QSerialPortInfo::availablePorts())
    {
        strlist.append(info.portName());
    }
    return strlist;
}

#endif // if (QT_VERSION >= QT_VERSION_CHECK(5, 0, 0))

QString LSerialPort::err_string()
{
    QString errstr;

    #if (QT_VERSION >= QT_VERSION_CHECK(5, 0, 0))
    errstr = serialport->errorString();
    #else // if (QT_VERSION >= QT_VERSION_CHECK(5, 0, 0))
    char errbuff[256];
    sprintf(errbuff, "%s\n", strerror(errno));
    errstr.append(errbuff);
    #endif // if (QT_VERSION >= QT_VERSION_CHECK(5, 0, 0))
    return errstr;
}

#if (QT_VERSION < QT_VERSION_CHECK(5, 0, 0))
# ifdef __linux__
void LSerialPort::setBaudrate(struct termios *opt, unsigned int baudrate)
{
    cfsetispeed(opt, baudrate);
    cfsetospeed(opt, baudrate);
}

void LSerialPort::set_data_bit(struct termios *opt, unsigned int databit)
{
    opt->c_cflag &= ~CSIZE;

    switch (databit)
    {
    case 8:
        opt->c_cflag |= CS8;
        break;

    case 7:
        opt->c_cflag |= CS7;
        break;

    case 6:
        opt->c_cflag |= CS6;
        break;

    case 5:
        opt->c_cflag |= CS5;
        break;

    default:
        opt->c_cflag |= CS8;
        break;
    }
}

void LSerialPort::set_parity(struct termios *opt, char parity)
{
    switch (parity)
    {
    case 'N':
    case 'n':
        opt->c_cflag &= ~PARENB;
        break;

    case 'E':
    case 'e':
        opt->c_cflag |= PARENB;
        opt->c_cflag &= ~PARODD;
        break;

    case 'O':
    case 'o':
        opt->c_cflag |= PARENB;
        opt->c_cflag |= ~PARODD;
        break;

    default:
        opt->c_cflag &= ~PARENB;
        break;
    }
}

void LSerialPort::set_stopbit(struct termios *opt, const char *stopbit)
{
    if (0 == strcmp(stopbit, "1"))
    {
        opt->c_cflag &= ~CSTOPB;
    }
    else if (0 == strcmp(stopbit, "1.5"))
    {
        opt->c_cflag &= ~CSTOPB;
    }
    else if (0 == strcmp(stopbit, "2"))
    {
        opt->c_cflag |= CSTOPB;
    }
    else
    {
        opt->c_cflag &= ~CSTOPB;
    }
}

int LSerialPort::set_port_attr(int         fd,
                               int         baudrate,
                               int         databit,
                               const char *stopbit,
                               char        parity,
                               int         vtime,
                               int         vmin)
{
    struct termios opt;

    memset(&opt, 0, sizeof(struct termios));

    tcgetattr(fd, &opt);

    setBaudrate(&opt, baudrate);

    // opt.c_cflag |= CLOCAL|CREAD;
    opt.c_cflag |= CREAD; // 打开接收者
    set_data_bit(&opt, databit);
    set_parity(&opt, parity);
    set_stopbit(&opt, stopbit);
    opt.c_iflag = 0;
    opt.c_oflag = 0;
    opt.c_lflag = 0;

    // opt.c_oflag &= ~OPOST;
    opt.c_cc[VTIME] = vtime; // 非规范模式读取时超时时间（阻塞模式有效）
    opt.c_cc[VMIN] = vmin;   // 非规范模式读取时最小字符数（阻塞模式有效）

    // options.c_lflag &= ~(ICANON | ECHO | ECHOE | ECHOK | ECHONL | NOFLSH); //
    // ICANON标准输入模式;
    // options.c_lflag &= ~(ICANON | ECHO); // 关闭回显

    // options.c_cflag |= CREAD; // 打开接收者
    // options.c_cflag &= ~CRTSCTS; // 禁用硬件流控制
    // options.c_iflag |= (IXON|IXOFF|IXANY); // 使能软件流控制
    // options.c_iflag &= ~(IXON|IXOFF|IXANY); // 禁用软件流控制
    // options.c_iflag |= (IGNPAR|ICRNL); // 忽略奇偶校验错误，将CR映射成NU
    // options.c_oflag &= ~OPOST; // 执行输出处理 现在就关闭状态
    // options.c_iflag |=IGNPAR; /*ignore parity on input */
    // options.c_oflag &= ~(OPOST | ONLCR | OLCUC | OCRNL | ONOCR | ONLRET |
    // OFILL);

    tcflush(fd, TCIFLUSH);
    return tcsetattr(fd, TCSANOW, &opt);
}

# endif // ifdef __linux__
#endif  // if (QT_VERSION < QT_VERSION_CHECK(5, 0, 0))

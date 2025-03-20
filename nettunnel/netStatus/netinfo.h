#ifndef NETINFO_H
#define NETINFO_H

/*
Linux中的网络流量常见类型
在Linux中，网络流量可以分为几种不同的类型。以下是一些常见的网络流量类型：
TCP流量：TCP（传输控制协议）是一种可靠的、面向连接的协议。TCP流量通常用于传输可靠的数据，例如网页浏览、文件传输等。
UDP流量：UDP（用户数据报协议）是一种无连接的协议，不保证数据的可靠性和顺序。UDP流量通常用于实时应用，如VoIP（网络电话）、视频流传输等。
ICMP流量：ICMP（Internet控制消息协议）用于传输网络控制消息和错误报告。ICMP流量通常用于网络故障排查、Ping命令等。
HTTP流量：HTTP（超文本传输协议）是一种应用层协议，用于在Web浏览器和服务器之间传输数据。HTTP流量通常用于网页浏览、下载文件等Web应用。
HTTPS流量：HTTPS是HTTP的安全版本，使用SSL（安全套接字层）或TLS（传输层安全）协议加密通信。HTTPS流量通常用于安全的Web传输，例如在线银行、电子商务等。
DNS流量：DNS（域名系统）用于将域名与IP地址相互映射。DNS流量通常用于解析域名、获取IP地址等。
FTP流量：FTP（文件传输协议）用于在客户端和服务器之间传输文件。FTP流量通常用于文件上传、下载等
SSH流量：SSH（安全壳层）是一种加密的网络协议，用于安全和远程登录。SSH流量通常用于安全的远程管理和文件传输。

要实现Linux上的网络流量分析，可以使用一些专门的工具和技术。下面是一些常用的方法和原理：
使用抓包工具：在Linux上，可以使用像tcpdump、Wireshark这样的抓包工具来捕获网络数据包。这些工具可以监听网络接口，并将捕获到的数据包保存到文件中进行分析。
使用流量监控工具：像ntop、iftop、bmon等工具可以实时监控网络接口的流量情况，并提供统计信息和图形化展示。
分析捕获的数据包：捕获到的数据包可以使用Wireshark等工具进行解析和分析。Wireshark可以显示捕获的数据包的详细信息，如源IP地址、目的IP地址、协议类型、端口等，并提供过滤机制来筛选感兴趣的数据。
使用流量分析工具：除了抓包工具和流量监控工具，还有一些专门用于网络流量分析的工具，如tcpflow、Bro等。这些工具可以解析并分析网络流量中的各种协议，提供更深入的网络流量分析功能。

在Linux文件系统的/proc目录下，可以找到一些与网络流量相关的信息。以下是一些常见的文件和目录：
/proc/net/dev：这个文件提供了网络设备的收发网络流量统计信息，包括每个网络接口的接收和发送数据包数量、错误统计、丢弃的数据包数量等。
使用awk或者sed命令进一步处理：你可以使用awk或者sed命令来处理和过滤数据，例如：cat /proc/net/dev | awk '{print $1, $2}'可以只打印出网络设备名称和接收流量的统计值.
bytes: 接口发送或接收的数据的总字节数
packets: 接口发送或接收的数据包总数
errs: 由设备驱动程序检测到的发送或接收错误的总数
drop: 设备驱动程序丢弃的数据包总数
fifo: FIFO缓冲区错误的数量
frame: 分组帧错误的数量
colls: 接口上检测到的冲突数
compressed: 设备驱动程序发送或接收的压缩数据包数
carrier: 由设备驱动程序检测到的载波损耗的数量
multicast: 设备驱动程序发送或接收的多播帧数
Inter-|   Receive                                                |  Transmit
 face |bytes    packets errs drop fifo frame compressed multicast|bytes    packets errs drop fifo colls carrier compressed
    lo: 52372248   20108    0    0    0     0          0         0 52372248   20108    0    0    0     0       0          0
  eth0: 17748776    4871    0    0    0     0          0       507   280619    3192    0    0    0     0       0          0

/proc/net/tcp：这个文件提供了当前TCP连接的详细信息，包括本地和远程IP地址、端口号，连接状态，以及一些统计信息。
使用grep命令筛选：你可以使用grep命令来筛选你感兴趣的信息，例如：grep "ESTABLISHED" /proc/net/tcp可以查看当前所有TCP连接中状态为ESTABLISHED的连接信息。

/proc/net/udp：类似于/proc/net/tcp文件，提供了当前UDP连接的详细信息。

/proc/net/raw：提供了当前原始套接字（RAW socket）的相关信息，包括本地和远程IP地址、协议类型等。

/proc/net/snmp：提供了简单网络管理协议（SNMP）相关的统计信息，包括接口、IP、TCP、UDP等各层的统计信息。

/proc/net/ip_conntrack：如果系统启用了连接跟踪（conntrack）功能，这个文件提供了关于当前连接的跟踪信息，包括连接状态、源IP地址、目的IP地址、端口号等。
 */

// cat /proc/net/netstat |  awk '(f==0) {name=$1; i=2; while ( i<=NF) {n[i] = $i; i++ }; f=1; next} (f==1){ i=2; while ( i<=NF){ printf "%s%s = %d\n", name, n[i], $i; i++}; f=0} '


#include <QObject>

class NetInfo : public QObject
{
    Q_OBJECT
public:
    explicit NetInfo(QObject *parent = nullptr);

signals:
};

#endif // NETINFO_H

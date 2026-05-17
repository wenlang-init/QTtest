#ifdef __linux__
#include "../printFunction.h"
#include <sys/ioctl.h>
#include <linux/i2c.h>
#include <linux/i2c-dev.h>
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>
#include <string.h>

int setI2CData(const char *i2cDevName,unsigned char addr,unsigned char *buf,int len)
{
  struct i2c_rdwr_ioctl_data i2c;
  struct i2c_msg msgs[5];
  int fd;
  i2c.nmsgs = 1;
  i2c.msgs = msgs;
  fd = open(i2cDevName,O_RDWR); // /dev/i2c-2
  if(fd < 0){
    DEBUG_PRINT_LOG("open %s error:%s\n",i2cDevName,strerror(errno));
    return -1;
  }
  ioctl(fd,I2C_TIMEOUT,1); // 超时时间
  //ioctl(fd,I2C_RETRIES,2); // 重复次数
  ioctl(fd,I2C_RETRIES,1); // 重复次数

  i2c.nmsgs = 1;
  i2c.msgs[0].len = len;
  i2c.msgs[0].addr = addr;
  i2c.msgs[0].flags = 0; // 写标志
  i2c.msgs[0].buf = buf;

  //memset(i2c.msgs[1].buf,0,i2c.msgs[1].len);
  int ret = ioctl(fd,I2C_RDWR,(unsigned long)&i2c); // 读写i2c
  if(ret < 0){
    DEBUG_PRINT_LOG("ioctl:%s\n",strerror(errno));
    close(fd);
    return -2;
  }
  close(fd);
  return 0;
}

int getI2CData(const char *i2cDevName,unsigned short base_addr,unsigned char addr,unsigned char *buf,int len)
{
  struct i2c_rdwr_ioctl_data i2c;
  struct i2c_msg msgs[5];
  int fd;
  i2c.nmsgs = 2; // 1个用于读，1个用于写
  i2c.msgs = msgs;
  fd = open(i2cDevName,O_RDWR); // /dev/i2c-2
  if(fd < 0){
    DEBUG_PRINT_LOG("open %s error:%s\n",i2cDevName,strerror(errno));
    return -1;
  }
  ioctl(fd,I2C_TIMEOUT,1); // 超时时间
  //ioctl(fd,I2C_RETRIES,2); // 重复次数
  ioctl(fd,I2C_RETRIES,1); // 重复次数

  unsigned char write_buf[256];
  // 读i2c
  i2c.nmsgs = 2; // 读需要2条消息:第一条是写，需要告诉读数据的地址；第二条读i2c
  i2c.msgs[0].len = 1;
  i2c.msgs[0].addr = base_addr;
  i2c.msgs[0].flags = 0; // 写标志
  i2c.msgs[0].buf = write_buf;
  i2c.msgs[0].buf[0] = addr;

  i2c.msgs[1].len = len;
  i2c.msgs[1].addr = base_addr;
  i2c.msgs[1].flags = I2C_M_RD; // 读标志
  i2c.msgs[1].buf = buf;
  //memset(i2c.msgs[1].buf,0,i2c.msgs[1].len);
  int ret = ioctl(fd,I2C_RDWR,(unsigned long)&i2c); // 读写i2c
  if(ret < 0){
    DEBUG_PRINT_LOG("ioctl:%s\n",strerror(errno));
    close(fd);
    return -2;
  }

#if 1
  DEBUG_PRINT_LOG("read:base=0x%02x,addr=%d,len=%d,value=",base_addr,i2c.msgs[0].buf[0]&0xff,i2c.msgs[1].len);
  printf("\nrow\\column\t0\t1\t2\t3\t4\t5\t6\t7\t8\t9\tA\tB\tC\tD\tE\tF");
  for(ret=0;ret<i2c.msgs[1].len;ret++){
      if(ret % 16 == 0){
          printf("\n");
          printf("%02d\t\t",ret/16);
      }
      printf("%02x\t",i2c.msgs[1].buf[ret]&0xff);
  }
  printf("\n");fflush(stdout);
#endif

  close(fd);
  return 0;
}

///////////////////////////////////////////////////////////////////////////////////////////////////////

#define SYSFS_GPIO_DIR "/sys/class/gpio"

int gpioExport(unsigned int gpio)
{
  char buf[100];
  int fd = open(SYSFS_GPIO_DIR "/export", O_WRONLY);
  if (fd < 0)
  {
    DEBUG_PRINT_LOG("open:%s\n",strerror(errno));
    return fd;
  }
  int len = snprintf(buf, sizeof(buf), "%d", gpio);
  if(0 >= write(fd, buf, len)){
    DEBUG_PRINT_LOG("write:%s\n",strerror(errno));
    close(fd);
    return -1;
  }
  close(fd);
  return 0;
}

int gpioUnexport(unsigned int gpio)
{
  char buf[100];
  int fd = open(SYSFS_GPIO_DIR "/unexport", O_WRONLY);
  if (fd < 0)
  {
    DEBUG_PRINT_LOG("open:%s\n",strerror(errno));
    return fd;
  }
  int len = snprintf(buf, sizeof(buf), "%d", gpio);
  if(0 >= write(fd, buf, len)){
    DEBUG_PRINT_LOG("write:%s\n",strerror(errno));
    close(fd);
    return -1;
  }
  close(fd);
  return 0;
}

int gpioSetDirection(unsigned int gpio,int Direction)
{
  char buf[100];
  snprintf(buf, sizeof(buf), SYSFS_GPIO_DIR  "/gpio%d/direction", gpio);
  int fd = open(buf, O_WRONLY);
  if (fd < 0)
  {
    DEBUG_PRINT_LOG("open:%s\n",strerror(errno));
    return fd;
  }
  if(Direction == 0){
    if(0 >= write(fd, "out", 4)){
      DEBUG_PRINT_LOG("write:%s\n",strerror(errno));
      close(fd);
      return -1;
    }
  } else {
    if(0 >= write(fd, "in", 3)){
      DEBUG_PRINT_LOG("write:%s\n",strerror(errno));
      close(fd);
      return -1;
    }
  }
  close(fd);
  return 0;
}

int gpioSetValue(unsigned int gpio,int value)
{
  char buf[100];
  snprintf(buf, sizeof(buf), SYSFS_GPIO_DIR "/gpio%d/value", gpio);

  int fd = open(buf, O_WRONLY);
  if (fd < 0)
  {
    DEBUG_PRINT_LOG("open:%s\n",strerror(errno));
    return fd;
  }
  snprintf(buf, sizeof(buf), "%d", value);
  if(0 >= write(fd, buf, strlen(buf))){
    DEBUG_PRINT_LOG("write:%s\n",strerror(errno));
    close(fd);
    return -1;
  }
  close(fd);
  return 0;
}

int gpioGetValue(unsigned int gpio,int *value)
{
  char buf[100];
  snprintf(buf, sizeof(buf), SYSFS_GPIO_DIR "/gpio%d/value", gpio);

  int fd = open(buf, O_RDONLY);
  if (fd < 0)
  {
    DEBUG_PRINT_LOG("open:%s\n",strerror(errno));
    return fd;
  }
  char c;
  if(0 >= read(fd,&c,1)){
    DEBUG_PRINT_LOG("read:%s\n",strerror(errno));
    close(fd);
    return -1;
  }

  *value = c - '0';

  close(fd);
  return 0;
}

int gpioSetEdge(unsigned int gpio, char *edge)
{
  char buf[100];
  snprintf(buf, sizeof(buf), SYSFS_GPIO_DIR "/gpio%d/edge", gpio);

  int fd = open(buf, O_WRONLY);
  if (fd < 0)
  {
    DEBUG_PRINT_LOG("open:%s\n",strerror(errno));
    return fd;
  }
  if(0 >= write(fd, edge, strlen(edge) + 1)){
    DEBUG_PRINT_LOG("write:%s\n",strerror(errno));
    close(fd);
  }
  close(fd);
  return 0;
}

#include <fcntl.h>
#include <stdio.h>
#include <linux/spi/spidev.h>
#include <sys/ioctl.h>

#if 0
struct spi_device {
    structdevice dev;
    structspi_master *master;
    u32 max_speed_hz; /* 通信时钟最大频率 */
    u8 chip_select; /* 片选号 */
    // spi_device的mode成员有两个比特位含义很重要：
    // SPI_CPHA选择对数据线采样的时机，0选择每个时钟周期的第一个沿跳变时采样数据，1选择第二个时钟沿采样数据；
    // SPI_CPOL选择每个时钟周期开始的极性，0表示时钟以低电平开始，1选择高电平开始。
    // 这两个比特有四种组合，对应SPI_MODE_0～SPI_MODE_3。
    u8 mode; /*SPI设备的模式，下面的宏是它各bit的含义 */
    #define SPI_CPHA 0x01 /* 采样的时钟相位 */
    #define SPI_CPOL 0x02 /* 时钟信号起始相位：高或者是低电平 */
    #define SPI_MODE_0 (0|0)
    #define SPI_MODE_1 (0|SPI_CPHA)
    #define SPI_MODE_2 (SPI_CPOL|0)
    #define SPI_MODE_3 (SPI_CPOL|SPI_CPHA)
    #define SPI_CS_HIGH 0x04 /* 为1时片选的有效信号是高电平 */
    #define SPI_LSB_FIRST 0x08 /* 发送时低比特在前 */
    #define SPI_3WIRE 0x10 /* 输入输出信号使用同一根信号线 */
    #define SPI_LOOP 0x20 /* 回环模式 */
    // bits_per_word。这个成员指定每次读写的字长，单位是比特。
    // 虽然大部分SPI接口的字长是8或者16，仍然会有一些特殊的例子。需要说明的是，如果这个成员为零的话，默认使用8作为字长。
    u8 bits_per_word; /* 每个通信字的字长（比特数） */
    int irq; /*使用到的中断 */
    void *controller_state;
    void *controller_data;
    char modalias[32]; /* 设备驱动的名字 */
};

struct spi_ioc_transfer {
    __u64        tx_buf;
    __u64        rx_buf;
    // 每个 spi_ioc_transfer都可以包含读和写的请求,读和写的长度必须相等
    // SPI控制器驱动会先将tx_buf写到SPI总线上，然后再读取len长度的内容到rx_buf。如果只想进行一个方向的传输，把另一个方向的缓冲置为0就可以了
    __u32        len;
    __u32        speed_hz; // 为每次通信配置不同的通信速率,为0的话就会使用spi_device中的配置

    __u16        delay_usecs; // 指定两个spi_ioc_transfer之间的延时，单位是微妙。一般不用定义
    __u8         bits_per_word; // 为每次通信配置不同的通信字长，为0的话就会使用spi_device中的配置
    // 指定这个cs_change结束之后是否需要改变片选线。一般针对同一设备的连续的几个spi_ioc_transfer，只有最后一个需要将这个成员置位。这样省去了来回改变片选线的时间，有助于提高通信速率
    __u8         cs_change;
    __u32        pad;
};
#endif

int spi_init(char *spi_dev, int mode, int bits, int speed)
{
    int fd = open(spi_dev, O_RDWR);
    if (fd < 0) {
        printf("打开 SPI 设备文件失败\n");
        return -1;
    }

    // 查看设备传输的时候是否先传输低比特位。如果是的话，lsb返回1 ioctl(fd,SPI_IOC_RD_LSB_FIRST, &lsb);
    // 设置传输的时候是否先传输低比特位。如果是的话，lsb置1 ioctl(fd,SPI_IOC_WR_LSB_FIRST, &lsb);
    // 读取SPI设备的字长 ioctl(fd, SPI_IOC_RD_BITS_PER_WORD, &bits);
    // 设置SPI设备的字长 ioctl(fd, SPI_IOC_WR_BITS_PER_WORD, &bits);
    // 读取SPI设备的最大通信速度 ioctl(fd, SPI_IOC_RD_MAX_SPEED_HZ, &speed);
    // 设置SPI设备的最大通信速度 ioctl(fd, SPI_IOC_WR_MAX_SPEED_HZ, &speed);
    // 读取SPI设备的通信模式 ioctl(fd, SPI_IOC_RD_MODE, &mode);
    // 设置SPI设备的通信模式 ioctl(fd, SPI_IOC_WR_MODE, &mode);
    // 读取SPI设备的片选号是否为高电平有效 ioctl(fd, SPI_IOC_RD_CS_HIGH, &cs_high);
    // 设置SPI设备的片选号是否为高电平有效 ioctl(fd, SPI_IOC_WR_CS_HIGH, &cs_high);
    // 读取SPI设备的3线模式 ioctl(fd, SPI_IOC_RD_3WIRE, &three_wire);
    // 设置SPI设备的3线模式 ioctl(fd, SPI_IOC_WR_3WIRE, &three_wire);
    // 读取SPI设备的回环模式 ioctl(fd, SPI_IOC_RD_LOOP, &loop);
    // 设置SPI设备的回环模式 ioctl(fd, SPI_IOC_WR_LOOP, &loop);
    // struct spi_ioc_transfer xfer[2];
    // status= ioctl(fd, SPI_IOC_MESSAGE(2), xfer);


    // 设置 SPI 通信模式 SPI设备对应的spi_device.mode
    if (ioctl(fd, SPI_IOC_WR_MODE, &mode) == -1) {
        printf("设置 SPI 模式失败:%d,%s\n",mode,strerror(errno));
        return -1;
    }
    // 读取当前的 SPI 通信模式
    if (ioctl(fd, SPI_IOC_RD_MODE, &mode) == -1) {
        printf("获取 SPI 模式失败:%d,%s\n",mode,strerror(errno));
        return -1;
    }

    // 设置 SPI 数据位数
    if (ioctl(fd, SPI_IOC_WR_BITS_PER_WORD, &bits) == -1) {
        printf("设置 SPI 数据位数失败:%d,%s\n",bits,strerror(errno));
        return -1;
    }
    // 读取当前的 SPI 数据位数
    if (ioctl(fd, SPI_IOC_RD_BITS_PER_WORD, &bits) == -1) {
        printf("获取 SPI 数据位数失败:%d,%s\n",bits,strerror(errno));
        return -1;
    }

    // 设置 SPI 最大通信速度
    if (ioctl(fd, SPI_IOC_WR_MAX_SPEED_HZ, &speed) == -1) {
        printf("设置 SPI 最大速度失败:%d,%s\n",speed,strerror(errno));
        return -1;
    }
    // 读取当前的 SPI 最大通信速度
    if (ioctl(fd, SPI_IOC_RD_MAX_SPEED_HZ, &speed) == -1) {
        printf("获取 SPI 最大速度失败:%d,%s\n",speed,strerror(errno));
        return -1;
    }

    printf("SPI 设备初始化成功\n");
    return fd;
}

int spi_readwrite(int fd, unsigned char *txbuf, unsigned char *rxbuf, int len)
{
    struct spi_ioc_transfer tr = {
        .tx_buf = (unsigned long)txbuf,
        .rx_buf = (unsigned long)rxbuf,
        .len = len,
        .delay_usecs = 0,
        .speed_hz = 0,
        .bits_per_word = 0,
    };

    if (ioctl(fd, SPI_IOC_MESSAGE(1), &tr) < 1) {
        printf("SPI 读写操作失败\n");
        return -1;
    }

    printf("SPI 数据读写成功\n");
    return 0;
}

int spi_close(int fd) {
   if (close(fd) == -1) {
       printf("关闭 SPI 设备失败\n");
       return -1;
   }
   printf("SPI 设备关闭成功\n");
   return 0;
}

#endif

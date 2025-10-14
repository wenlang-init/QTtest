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

#endif

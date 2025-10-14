#ifndef IO_H
#define IO_H

#ifdef __cplusplus
extern "C" {
#endif

#ifdef __linux__

int setI2CData(const char *i2cDevName,unsigned char addr,unsigned char *buf,int len);
int getI2CData(const char *i2cDevName,unsigned short base_addr,unsigned char addr,unsigned char *buf,int len);

int gpioExport(unsigned int gpio);
int gpioUnexport(unsigned int gpio);
int gpioSetDirection(unsigned int gpio,int Direction);
int gpioSetValue(unsigned int gpio,int value);
int gpioGetValue(unsigned int gpio,int *value);
int gpioSetEdge(unsigned int gpio, char *edge);

#endif

#ifdef __cplusplus
}
#endif

#endif // IO_H

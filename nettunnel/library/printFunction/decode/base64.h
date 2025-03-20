#ifndef BASE64_H
#define BASE64_H

#ifdef __cplusplus
extern "C" {
#endif
/*
*  base64编码, 3byte --> 4byte , 得到4byte倍的编码数据
*  编码后的字符串以'\0'结尾
*  使用完后需要释放内存
*  src: 源数据
*  size: 源数据长度
*  return: 编码后的数据
*/
char *toBase64(unsigned char *src, unsigned int length);

/*
*  base64解码, 4byte --> 3byte, 得到3byte倍的解码数据
*  使用完后需要释放内存
*  src: 源数据
*  size: 源数据长度
*  destsize: 解码后的数据长度
*  return: 解码后的数据
*/
char *fromBase64(char *src, unsigned int length,unsigned int *destsize);
#ifdef __cplusplus
}
#endif
#endif
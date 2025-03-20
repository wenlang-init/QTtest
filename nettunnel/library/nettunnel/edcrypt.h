#ifndef EDCRYPT_H
#define EDCRYPT_H

#ifdef __cplusplus
extern "C" {
#endif
/*
 * 加密算法,对数据进行异或操作
 *
 * @param p 数据指针
 * @param size 数据长度
 * @param key 加密密钥
 * @return 解密密钥
 *
 * @note 加密后的数据可以通过decrypt函数解密
*/
unsigned char encrypt(unsigned char *p, unsigned int size, unsigned char key);
/*
 * 解密算法,对数据进行异或操作
 *
 * @param p 数据指针
 * @param size 数据长度
 * @param key 解密密钥
 *
 * @note 解密密钥可以通过encrypt函数获取
*/
void decrypt(unsigned char *p, unsigned int size, unsigned char key);

/*
 * 加密算法,对数据进行异或操作
 *
 * @param p 数据指针
 * @param size 数据长度
 * @param key 加密密钥
 * @param dekey 解密密钥（输出）
 * @return 不足4个字符的余数或-1
 *
 * @note 加密后的数据可以通过decrypt函数解密
*/
int encrypt4(unsigned char *p, unsigned int size, unsigned char key[4],unsigned char dekey[4]);
/*
 * 解密算法,对数据进行异或操作
 *
 * @param p 数据指针
 * @param size 数据长度
 * @param key 解密密钥
 *
 * @note 解密密钥可以通过encrypt4函数获取
*/
void decrypt4(unsigned char *p, unsigned int size, unsigned char key[4]);

#ifdef __cplusplus
}
#endif

#endif // EDCRYPT_H

#ifndef EDCRYPT_H
#define EDCRYPT_H

#ifdef __cplusplus
extern "C" {
#endif

unsigned char encrypt(unsigned char *p, unsigned int size, unsigned char key);
void decrypt(unsigned char *p, unsigned int size, unsigned char key);

#ifdef __cplusplus
}
#endif

#endif // EDCRYPT_H

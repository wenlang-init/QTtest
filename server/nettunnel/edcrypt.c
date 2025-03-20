#include "edcrypt.h"

static unsigned char enKey = 0xC2;

unsigned char encrypt(unsigned char *p, unsigned int size, unsigned char key)
{
    for(unsigned int i=0; i<size; i++)
    {
        p[i] ^= key;
    }
    return key^enKey;
}

void decrypt(unsigned char *p, unsigned int size, unsigned char key)
{
    key ^= enKey;
    for(unsigned int i=0; i<size; i++)
    {
        p[i] ^= key;
    }
}

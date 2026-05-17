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

int encrypt4(unsigned char *p, unsigned int size, unsigned char key[4], unsigned char dekey[4])
{
    if(!key || !dekey){
        return -1;
    }

    dekey[0] ^= key[0]^enKey;
    dekey[1] ^= key[1]^enKey;
    dekey[2] ^= key[2]^enKey;
    dekey[3] ^= key[3]^enKey;

    unsigned int imod = size%4;
    unsigned int icnt = size/4;
    if(imod == 3){
        p[size - 1] ^= dekey[2];
        p[size - 2] ^= dekey[1];
        p[size - 3] ^= dekey[0];
    } else if(imod == 2){
        p[size - 1] ^= dekey[1];
        p[size - 2] ^= dekey[0];
    } else if(imod == 1){
        p[size - 1] ^= dekey[0];
    }

    for(unsigned int i=0; i<icnt*4; i+=4)
    {
        p[i] ^= key[0];
        p[i+1] ^= key[1];
        p[i+2] ^= key[2];
        p[i+3] ^= key[3];
    }
    return imod;
}

void decrypt4(unsigned char *p, unsigned int size, unsigned char key[4])
{
    unsigned char dekey[4]={0};
    dekey[0] ^= key[0]^enKey;
    dekey[1] ^= key[1]^enKey;
    dekey[2] ^= key[2]^enKey;
    dekey[3] ^= key[3]^enKey;

    unsigned int imod = size%4;
    unsigned int icnt = size/4;
    if(imod == 3){
        p[size - 1] ^= dekey[2];
        p[size - 2] ^= dekey[1];
        p[size - 3] ^= dekey[0];
    } else if(imod == 2){
        p[size - 1] ^= dekey[1];
        p[size - 2] ^= dekey[0];
    } else if(imod == 1){
        p[size - 1] ^= dekey[0];
    }

    for(unsigned int i=0; i<icnt*4; i+=4)
    {
        p[i] ^= dekey[0];
        p[i+1] ^= dekey[1];
        p[i+2] ^= dekey[2];
        p[i+3] ^= dekey[3];
    }
}

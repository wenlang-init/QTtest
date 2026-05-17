#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// base64编码表
const char base64Tab[] = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/";

/*
*  获取base64编码表序号
*/
#define Base64ToIndex(src) (src>='A' && src<='Z'? src-'A': src>='a' && src<='z'? src-'a'+26: src>='0' && src<='9'? src-'0'+52: src=='+' ? 62: src=='/'? 63: -1)

char *toBase64(unsigned char *src, unsigned int length)
{
	if(length==0){
		return NULL;
	}
	int tlen = length%3;
	int len = ((tlen ? 1:0) + length/3)*4;
    char *pdata = (char*)malloc(len+1);
	if(!pdata) return NULL;
	char *p = pdata;
	for(unsigned int i=0; i<length/3; i++)	
	{
		*p++ = base64Tab[*src>>2];
		*p++ = base64Tab[(*src<<4)&0x3f|(*(src+1)>>4)];
		*p++ = base64Tab[(*(src+1)<<2)&0x3f|(*(src+2)>>6)];
		*p++ = base64Tab[*(src+2)&0x3f];
		src += 3;
	}
	if(tlen==1){
		*p++ = base64Tab[*src>>2];
		*p++ = base64Tab[(*src<<4)&0x3f];
		*p++ = '=';
		*p++ = '=';
	} else if(tlen==2){
		*p++ = base64Tab[*src>>2];
		*p++ = base64Tab[(*src<<4)&0x3f|(*(src+1)>>4)];
		*p++ = base64Tab[(*(src+1)<<2)&0x3f];
		*p++ = '=';
	}
	*p = '\0';
	return pdata;
}

char *fromBase64(char *src, unsigned int length,unsigned int *destsize)
{
	if(length==0){
		return NULL;
	}
	int tlen = length%4;
	int len = length/4*3;
	*destsize = 0;
    char *pdata;
	if(tlen == 2){
		if(Base64ToIndex(src[length-1])==-1 || Base64ToIndex(src[length-2])==-1){
			// 无效数据
			pdata = (char*)malloc(len);
			if(!pdata) return NULL;
		} else {
			len += 1;
			pdata = (char*)malloc(len);
			if(!pdata) return NULL;
			char c1 = Base64ToIndex(src[length-2]);
			char c2 = Base64ToIndex(src[length-1]);
			pdata[len-1] = (c1<<2)&0xfc|(c2>>4)&0x03;
		}
	} else if(tlen == 3){
		if(Base64ToIndex(src[length-2])==-1 || Base64ToIndex(src[length-3])==-1){
			// 无效数据
			pdata = (char*)malloc(len);
			if(!pdata) return NULL;
		} else {
			if(src[length-1]=='='){
				len += 1;
				pdata = (char*)malloc(len);
				if(!pdata) return NULL;
				char c1 = Base64ToIndex(src[length-3]);
				char c2 = Base64ToIndex(src[length-2]);
				if(c1 < 0 || c2 < 0){
					free(pdata);
					return NULL;
				}
				pdata[len-1] = (c1<<2)&0xfc|(c2>>4)&0x03;
			} else {
				len += 2;
				pdata = (char*)malloc(len);
				if(!pdata) return NULL;
				char c1 = Base64ToIndex(src[length-3]);
				char c2 = Base64ToIndex(src[length-2]);
				char c3 = Base64ToIndex(src[length-1]);
				if(c1 < 0 || c2 < 0 || c3 < 0){
					free(pdata);
					return NULL;
				}
				pdata[len-2] = (c1<<2)&0xfc|(c2>>4)&0x03;
				pdata[len-1] = (c2<<4)&0xf0|(c3>>2)&0x0f;
			}
		}
	} else {
		if(len < 3){
			return NULL;
		}
		if(tlen == 0){
			// 处理需要补齐的数据
			if(Base64ToIndex(src[length-3])==-1 || Base64ToIndex(src[length-4])==-1){
				// 无效数据
				printf("invalid data:%c %c %c %c\n",src[length-4],src[length-3],src[length-2],src[length-1]);
				return NULL;
			} else {
				if(src[length-1]=='='){
					if(src[length-2]=='='){
						char c1 = Base64ToIndex(src[length-4]);
						char c2 = Base64ToIndex(src[length-3]);
						if(c1 < 0 || c2 < 0){
							return NULL;
						}
						len -= 2;
						pdata = (char*)malloc(len);
						if(!pdata) return NULL;
						pdata[len-1] = (c1<<2)&0xfc|(c2>>4)&0x03;
					} else {
						char c1 = Base64ToIndex(src[length-4]);
						char c2 = Base64ToIndex(src[length-3]);
						char c3 = Base64ToIndex(src[length-2]);
						if(c1 < 0 || c2 < 0 || c3 < 0){
							return NULL;
						}
						len -= 1;
						pdata = (char*)malloc(len);
						if(!pdata) return NULL;
						pdata[len-2] = (c1<<2)&0xfc|(c2>>4)&0x03;
						pdata[len-1] = (c2<<4)&0xf0|(c3>>2)&0x0f;
					}
				} else {
					char c1 = Base64ToIndex(src[length-4]);
					char c2 = Base64ToIndex(src[length-3]);
					char c3 = Base64ToIndex(src[length-2]);
					char c4 = Base64ToIndex(src[length-1]);
					if(c1 < 0 || c2 < 0 || c3 < 0 || c4 < 0){
						return NULL;
					}
					pdata = (char*)malloc(len);
					if(!pdata) return NULL;
					pdata[len-3] = (c1<<2)&0xfc|(c2>>4)&0x03;
					pdata[len-2] = (c2<<4)&0xf0|(c3>>2)&0x0f;
					pdata[len-1] = (c3<<6)&0xc0|(c4)&0x3f;
				}
			}
		} else {
			pdata = (char*)malloc(len);
			if(!pdata) return NULL;
		}
	}
	
	char *p = pdata;
	for(int i=0; i<(int)length/4 - 1; i++)
	{
		char c1 = Base64ToIndex(src[0]);
		char c2 = Base64ToIndex(src[1]);
		char c3 = Base64ToIndex(src[2]);
		char c4 = Base64ToIndex(src[3]);
		if(c1 < 0 || c2 < 0 || c3 < 0 || c4 < 0){
			free(pdata);
			return NULL;
		}
		*p++ = (c1<<2)&0xfc|(c2>>4)&0x03;
		*p++ = (c2<<4)&0xf0|(c3>>2)&0x0f;
		*p++ = (c3<<6)&0xc0|(c4)&0x3f;
		src += 4;
	}
	*destsize  = len;
	return pdata;
}

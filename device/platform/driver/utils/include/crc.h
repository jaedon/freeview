#ifndef __CRC_H__
#define __CRC_H__

unsigned long DRV_UTIL_GetCrc32(void *pvBuf, unsigned long n);
unsigned short DRV_UTIL_GetCrc16(void *pvBuf, unsigned short n);
unsigned long DRV_UTIL_Get32Bits(unsigned char *pBuf);
unsigned short DRV_UTIL_Get16Bits(unsigned char *pBuf);

#endif //__CRC_H__


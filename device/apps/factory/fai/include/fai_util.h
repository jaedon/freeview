/******************************************************************************
* $Workfile:$
* $Modtime:$
* Author : D.K Lee
* description :
*
* Copyright (c) 2006 by Humax Co., Ltd.
* All right reserved
******************************************************************************/

#ifndef _FAI_UTIL_H_
#define _FAI_UTIL_H_

/******************************************************************************
* header and definitions
******************************************************************************/
#include "ftype.h"

/******************************************************************************
* Definitions, typdef & const data
******************************************************************************/
#define HDF_HEADER_BLOCK_SIZE 			20
#define HDF_DEFAULT_DATA_BLOCK_SIZE 	12
#define MAX_HDF_DATA_BLOCK_SIZE			(0x8000-10)

#define HDF_DATA_TYPE_MASK				0x00FF
#define	HDF_DATA_TYPE_LOADER			0x00
#define	HDF_DATA_TYPE_APPLICATION		0x01
#define	HDF_DATA_TYPE_EEPROM			0x02
#define	HDF_DATA_TYPE_FLASH				0x03
#define	HDF_DATA_TYPE_SYSTEMID			0x04
#define HDF_DATA_TYPE_BETACODE			0x07
#define	HDF_DATA_TYPE_SIGNATURE			0x80
#define	HDF_DATA_TYPE_COMPRESS			0x8000
#define HDF_DATA_TYPE_OBSCURE			0x0800

typedef enum 
{
	HDF_RELOCATE_TO_RAM, 
	HDF_RELOCATE_TO_ROM
} HDF_RELOCATE_TYPE;

typedef struct header_block 
{
  unsigned short usSize;
  unsigned short usCRC16;
  unsigned short usHDFVersion;
  unsigned short usNumOfDataBlock;
  unsigned long ulStartID;
  unsigned long ulEndID;
  unsigned long ulDataSize;
} HDF_HEADER_BLOCK_T;

typedef struct data_block 
{
  unsigned short usSize;		// usSize를 제외한 size
  unsigned short usCRC16;
  unsigned short usFlag;
  unsigned short usOrigDataSize;
  unsigned long ulDataAddr;
  unsigned char *aucData; 		// size : usSize - 10
} HDF_DATA_BLOCK_T;

#define FAI_UTIL_ENDIAN_SWAP16(x) (((x & 0x00ff) << 8) | (x >> 8))
#define FAI_UTIL_ENDIAN_SWAP32(x) ((x << 24) | ((x & 0xff00) << 8) | ((x & 0xff0000) >> 8) | (x >> 24))

/******************************************************************************
* Global variables and structures
******************************************************************************/

/******************************************************************************
* External variables and functions
******************************************************************************/

/******************************************************************************
* Static variables and structures
******************************************************************************/

/******************************************************************************
* Static function prototypes
******************************************************************************/

/*******************************************************************************
* function : 
* description :
* input :
* output :
* return :
* 기타 참고자료 및 파일
*******************************************************************************/
unsigned long FAI_UTIL_GetBits(unsigned char *pucbuf, unsigned char ucByteIdx, unsigned char ucBitIdx, unsigned char ucLen);

/*******************************************************************************
* function : 
* description :
* input :
* output :
* return :
* 기타 참고자료 및 파일
*******************************************************************************/
FAI_ERR_CODE FAI_UTIL_GetHdfHeaderBlock(unsigned char *pucBuf, HDF_HEADER_BLOCK_T *ptHeaderBlock);

/*******************************************************************************
* function : 
* description :
* input :
* output :
* return :
* 기타 참고자료 및 파일
*******************************************************************************/
FAI_ERR_CODE FAI_UTIL_GetHdfDataBlock(unsigned char *pucBuf, HDF_DATA_BLOCK_T *ptDataBlock);

/*******************************************************************************
* function : FAI_UTIL_CRC16Byte
* description :
* input :
* output :
* return :
* 기타 참고자료 및 파일
*******************************************************************************/
unsigned short FAI_UTIL_GetCRC16(unsigned short usCrc, unsigned char *pucBuffer, unsigned int unLen);
unsigned long FAI_UTIL_GetCRC32(unsigned long ulCrc, unsigned char *pucBuffer, unsigned int unLen);
unsigned long FAI_UTIL_GetCRC32Fast(void *inP, unsigned long NumberOfByte);


#endif /* _FAI_UTIL_H_ */

/* end of file */


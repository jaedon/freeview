/******************************************************************************
* $Workfile:$
* $Modtime:$
* Author : 
* description :		 Factory Util
*
* Copyright (c) 2008 by Humax Co., Ltd.
* All right reserved
******************************************************************************/

/*******************************************************************************
* header and definitions
*******************************************************************************/
/* global header files */

/* chip manufacture's  header files */

/* humax header files */
#include "fvk.h"
#include "fai_err.h"
#include "fai_common.h"
#include "fai_util.h"
#include "decompress.h"

/*******************************************************************************
* Debug level
*******************************************************************************/

/*******************************************************************************
* Definitions, typdef & const data
*******************************************************************************/

/*******************************************************************************
* External variables and functions
*******************************************************************************/

/*******************************************************************************
* Global variables and structures
*******************************************************************************/
unsigned short const ausCrc16Table[256] = {
  0x0000, 0xC0C1, 0xC181, 0x0140, 0xC301, 0x03C0, 0x0280, 0xC241,
  0xC601, 0x06C0, 0x0780, 0xC741, 0x0500, 0xC5C1, 0xC481, 0x0440,
  0xCC01, 0x0CC0, 0x0D80, 0xCD41, 0x0F00, 0xCFC1, 0xCE81, 0x0E40,
  0x0A00, 0xCAC1, 0xCB81, 0x0B40, 0xC901, 0x09C0, 0x0880, 0xC841,
  0xD801, 0x18C0, 0x1980, 0xD941, 0x1B00, 0xDBC1, 0xDA81, 0x1A40,
  0x1E00, 0xDEC1, 0xDF81, 0x1F40, 0xDD01, 0x1DC0, 0x1C80, 0xDC41,
  0x1400, 0xD4C1, 0xD581, 0x1540, 0xD701, 0x17C0, 0x1680, 0xD641,
  0xD201, 0x12C0, 0x1380, 0xD341, 0x1100, 0xD1C1, 0xD081, 0x1040,
  0xF001, 0x30C0, 0x3180, 0xF141, 0x3300, 0xF3C1, 0xF281, 0x3240,
  0x3600, 0xF6C1, 0xF781, 0x3740, 0xF501, 0x35C0, 0x3480, 0xF441,
  0x3C00, 0xFCC1, 0xFD81, 0x3D40, 0xFF01, 0x3FC0, 0x3E80, 0xFE41,
  0xFA01, 0x3AC0, 0x3B80, 0xFB41, 0x3900, 0xF9C1, 0xF881, 0x3840,
  0x2800, 0xE8C1, 0xE981, 0x2940, 0xEB01, 0x2BC0, 0x2A80, 0xEA41,
  0xEE01, 0x2EC0, 0x2F80, 0xEF41, 0x2D00, 0xEDC1, 0xEC81, 0x2C40,
  0xE401, 0x24C0, 0x2580, 0xE541, 0x2700, 0xE7C1, 0xE681, 0x2640,
  0x2200, 0xE2C1, 0xE381, 0x2340, 0xE101, 0x21C0, 0x2080, 0xE041,
  0xA001, 0x60C0, 0x6180, 0xA141, 0x6300, 0xA3C1, 0xA281, 0x6240,
  0x6600, 0xA6C1, 0xA781, 0x6740, 0xA501, 0x65C0, 0x6480, 0xA441,
  0x6C00, 0xACC1, 0xAD81, 0x6D40, 0xAF01, 0x6FC0, 0x6E80, 0xAE41,
  0xAA01, 0x6AC0, 0x6B80, 0xAB41, 0x6900, 0xA9C1, 0xA881, 0x6840,
  0x7800, 0xB8C1, 0xB981, 0x7940, 0xBB01, 0x7BC0, 0x7A80, 0xBA41,
  0xBE01, 0x7EC0, 0x7F80, 0xBF41, 0x7D00, 0xBDC1, 0xBC81, 0x7C40,
  0xB401, 0x74C0, 0x7580, 0xB541, 0x7700, 0xB7C1, 0xB681, 0x7640,
  0x7200, 0xB2C1, 0xB381, 0x7340, 0xB101, 0x71C0, 0x7080, 0xB041,
  0x5000, 0x90C1, 0x9181, 0x5140, 0x9301, 0x53C0, 0x5280, 0x9241,
  0x9601, 0x56C0, 0x5780, 0x9741, 0x5500, 0x95C1, 0x9481, 0x5440,
  0x9C01, 0x5CC0, 0x5D80, 0x9D41, 0x5F00, 0x9FC1, 0x9E81, 0x5E40,
  0x5A00, 0x9AC1, 0x9B81, 0x5B40, 0x9901, 0x59C0, 0x5880, 0x9841,
  0x8801, 0x48C0, 0x4980, 0x8941, 0x4B00, 0x8BC1, 0x8A81, 0x4A40,
  0x4E00, 0x8EC1, 0x8F81, 0x4F40, 0x8D01, 0x4DC0, 0x4C80, 0x8C41,
  0x4400, 0x84C1, 0x8581, 0x4540, 0x8701, 0x47C0, 0x4680, 0x8641,
  0x8201, 0x42C0, 0x4380, 0x8341, 0x4100, 0x81C1, 0x8081, 0x4040
};

const unsigned long	aulCrc32Table[256] =
{
	0x00000000, 0x04c11db7, 0x09823b6e, 0x0d4326d9,
	0x130476dc, 0x17c56b6b, 0x1a864db2, 0x1e475005,
	0x2608edb8, 0x22c9f00f, 0x2f8ad6d6, 0x2b4bcb61,
	0x350c9b64, 0x31cd86d3, 0x3c8ea00a, 0x384fbdbd,
	0x4c11db70, 0x48d0c6c7, 0x4593e01e, 0x4152fda9,
	0x5f15adac, 0x5bd4b01b, 0x569796c2, 0x52568b75,
	0x6a1936c8, 0x6ed82b7f, 0x639b0da6, 0x675a1011,
	0x791d4014, 0x7ddc5da3, 0x709f7b7a, 0x745e66cd,
	0x9823b6e0, 0x9ce2ab57, 0x91a18d8e, 0x95609039,
	0x8b27c03c, 0x8fe6dd8b, 0x82a5fb52, 0x8664e6e5,
	0xbe2b5b58, 0xbaea46ef, 0xb7a96036, 0xb3687d81,
	0xad2f2d84, 0xa9ee3033, 0xa4ad16ea, 0xa06c0b5d,
	0xd4326d90, 0xd0f37027, 0xddb056fe, 0xd9714b49,
	0xc7361b4c, 0xc3f706fb, 0xceb42022, 0xca753d95,
	0xf23a8028, 0xf6fb9d9f, 0xfbb8bb46, 0xff79a6f1,
	0xe13ef6f4, 0xe5ffeb43, 0xe8bccd9a, 0xec7dd02d,
	0x34867077, 0x30476dc0, 0x3d044b19, 0x39c556ae,
	0x278206ab, 0x23431b1c, 0x2e003dc5, 0x2ac12072,
	0x128e9dcf, 0x164f8078, 0x1b0ca6a1, 0x1fcdbb16,
	0x018aeb13, 0x054bf6a4, 0x0808d07d, 0x0cc9cdca,
	0x7897ab07, 0x7c56b6b0, 0x71159069, 0x75d48dde,
	0x6b93dddb, 0x6f52c06c, 0x6211e6b5, 0x66d0fb02,
	0x5e9f46bf, 0x5a5e5b08, 0x571d7dd1, 0x53dc6066,
	0x4d9b3063, 0x495a2dd4, 0x44190b0d, 0x40d816ba,
	0xaca5c697, 0xa864db20, 0xa527fdf9, 0xa1e6e04e,
	0xbfa1b04b, 0xbb60adfc, 0xb6238b25, 0xb2e29692,
	0x8aad2b2f, 0x8e6c3698, 0x832f1041, 0x87ee0df6,
	0x99a95df3, 0x9d684044, 0x902b669d, 0x94ea7b2a,
	0xe0b41de7, 0xe4750050, 0xe9362689, 0xedf73b3e,
	0xf3b06b3b, 0xf771768c, 0xfa325055, 0xfef34de2,
	0xc6bcf05f, 0xc27dede8, 0xcf3ecb31, 0xcbffd686,
	0xd5b88683, 0xd1799b34, 0xdc3abded, 0xd8fba05a,
	0x690ce0ee, 0x6dcdfd59, 0x608edb80, 0x644fc637,
	0x7a089632, 0x7ec98b85, 0x738aad5c, 0x774bb0eb,
	0x4f040d56, 0x4bc510e1, 0x46863638, 0x42472b8f,
	0x5c007b8a, 0x58c1663d, 0x558240e4, 0x51435d53,
	0x251d3b9e, 0x21dc2629, 0x2c9f00f0, 0x285e1d47,
	0x36194d42, 0x32d850f5, 0x3f9b762c, 0x3b5a6b9b,
	0x0315d626, 0x07d4cb91, 0x0a97ed48, 0x0e56f0ff,
	0x1011a0fa, 0x14d0bd4d, 0x19939b94, 0x1d528623,
	0xf12f560e, 0xf5ee4bb9, 0xf8ad6d60, 0xfc6c70d7,
	0xe22b20d2, 0xe6ea3d65, 0xeba91bbc, 0xef68060b,
	0xd727bbb6, 0xd3e6a601, 0xdea580d8, 0xda649d6f,
	0xc423cd6a, 0xc0e2d0dd, 0xcda1f604, 0xc960ebb3,
	0xbd3e8d7e, 0xb9ff90c9, 0xb4bcb610, 0xb07daba7,
	0xae3afba2, 0xaafbe615, 0xa7b8c0cc, 0xa379dd7b,
	0x9b3660c6, 0x9ff77d71, 0x92b45ba8, 0x9675461f,
	0x8832161a, 0x8cf30bad, 0x81b02d74, 0x857130c3,
	0x5d8a9099, 0x594b8d2e, 0x5408abf7, 0x50c9b640,
	0x4e8ee645, 0x4a4ffbf2, 0x470cdd2b, 0x43cdc09c,
	0x7b827d21, 0x7f436096, 0x7200464f, 0x76c15bf8,
	0x68860bfd, 0x6c47164a, 0x61043093, 0x65c52d24,
	0x119b4be9, 0x155a565e, 0x18197087, 0x1cd86d30,
	0x029f3d35, 0x065e2082, 0x0b1d065b, 0x0fdc1bec,
	0x3793a651, 0x3352bbe6, 0x3e119d3f, 0x3ad08088,
	0x2497d08d, 0x2056cd3a, 0x2d15ebe3, 0x29d4f654,
	0xc5a92679, 0xc1683bce, 0xcc2b1d17, 0xc8ea00a0,
	0xd6ad50a5, 0xd26c4d12, 0xdf2f6bcb, 0xdbee767c,
	0xe3a1cbc1, 0xe760d676, 0xea23f0af, 0xeee2ed18,
	0xf0a5bd1d, 0xf464a0aa, 0xf9278673, 0xfde69bc4,
	0x89b8fd09, 0x8d79e0be, 0x803ac667, 0x84fbdbd0,
	0x9abc8bd5, 0x9e7d9662, 0x933eb0bb, 0x97ffad0c,
	0xafb010b1, 0xab710d06, 0xa6322bdf, 0xa2f33668,
	0xbcb4666d, 0xb8757bda, 0xb5365d03, 0xb1f740b4
};

unsigned char aucBitPosMapTbl[8] = {0x80, 0x40, 0x20, 0x10, 0x08, 0x04, 0x02, 0x01};

static unsigned short INTRN_FAI_UTIL_CRC16Byte(unsigned short usCrc, const unsigned char ucData);
unsigned char INTRN_FAI_UTIL_GetBit(unsigned char ucByte, unsigned char ucIdx);
unsigned long INTRN_FAI_UTIL_GetBits(unsigned char *pucBuf, unsigned char ucByteOffset, unsigned char ucBitOffset, unsigned char ucLen);

/*******************************************************************************
* function : 
* description :
* input :
* output :
* return :
* 기타 참고자료 및 파일
*******************************************************************************/
unsigned long FAI_UTIL_GetBits(unsigned char *pucbuf, unsigned char ucByteIdx, unsigned char ucBitIdx, unsigned char ucLen)
{
	unsigned long ulValue;

	ulValue = INTRN_FAI_UTIL_GetBits(pucbuf, ucByteIdx, ucBitIdx, ucLen);

	return ulValue;
}

/*******************************************************************************
* function : 
* description :
* input :
* output :
* return :
* 기타 참고자료 및 파일
*******************************************************************************/
FAI_ERR_CODE FAI_UTIL_GetHdfHeaderBlock(unsigned char *pucBuf, HDF_HEADER_BLOCK_T *ptHeaderBlock)
{
	if( pucBuf == NULL || ptHeaderBlock == NULL )
	{
		return FAI_ERR;
	}

	/* Get Header block */	
	ptHeaderBlock->usSize 			= FAI_UTIL_GetBits(pucBuf, 0, 0, 16);
	ptHeaderBlock->usCRC16 			= FAI_UTIL_GetBits(pucBuf, 2, 0, 16); 
	ptHeaderBlock->usHDFVersion 	= FAI_UTIL_GetBits(pucBuf, 4, 0, 16);
	ptHeaderBlock->usNumOfDataBlock = FAI_UTIL_GetBits(pucBuf, 6, 0, 16);
	ptHeaderBlock->ulStartID 		= FAI_UTIL_GetBits(pucBuf, 8, 0, 32);
	ptHeaderBlock->ulEndID 			= FAI_UTIL_GetBits(pucBuf, 12, 0, 32);
	ptHeaderBlock->ulDataSize 		= FAI_UTIL_GetBits(pucBuf, 16, 0, 32);

	FAI_PRINT(FAI_PRT_DBGINFO, ("[INTRN_FAI_FLASH_DataCompare] : usSize=%x\n", ptHeaderBlock->usSize));
	FAI_PRINT(FAI_PRT_DBGINFO, ("[INTRN_FAI_FLASH_DataCompare] : usCRC16=%x\n", ptHeaderBlock->usCRC16));
	FAI_PRINT(FAI_PRT_DBGINFO, ("[INTRN_FAI_FLASH_DataCompare] : usHDFVersion=%x\n", ptHeaderBlock->usHDFVersion));
	FAI_PRINT(FAI_PRT_DBGINFO, ("[INTRN_FAI_FLASH_DataCompare] : usNumOfDataBlock=%x\n", ptHeaderBlock->usNumOfDataBlock));
	FAI_PRINT(FAI_PRT_DBGINFO, ("[INTRN_FAI_FLASH_DataCompare] : ulStartID=%x\n", ptHeaderBlock->ulStartID));
	FAI_PRINT(FAI_PRT_DBGINFO, ("[INTRN_FAI_FLASH_DataCompare] : ulEndID=%x\n", ptHeaderBlock->ulEndID));
	FAI_PRINT(FAI_PRT_DBGINFO, ("[INTRN_FAI_FLASH_DataCompare] : ulDataSize=%x\n\n", ptHeaderBlock->ulDataSize));

	/* Header block CRC check */
	if( FAI_UTIL_GetCRC16(0, pucBuf+4, 16) != ptHeaderBlock->usCRC16 )
	{
		FAI_PRINT(FAI_PRT_ERROR, ("[FAI_UTIL_GetHdfHeaderBlock] : Header Block CRC Error!!\n"));
		return FAI_ERR;
	}
	
	return FAI_NO_ERR;
}

/*******************************************************************************
* function : 
* description :
* input :
* output :
* return :
* 기타 참고자료 및 파일
*******************************************************************************/
FAI_ERR_CODE FAI_UTIL_GetHdfDataBlock(unsigned char *pucBuf, HDF_DATA_BLOCK_T *ptDataBlock)
{
	if( pucBuf == NULL || ptDataBlock == NULL )
	{
		return FAI_ERR;
	}

	/* Get data block */	
	ptDataBlock->usSize 		= FAI_UTIL_GetBits(pucBuf, 0, 0, 16);
	ptDataBlock->usCRC16 		= FAI_UTIL_GetBits(pucBuf, 2, 0, 16);
	ptDataBlock->usFlag 		= FAI_UTIL_GetBits(pucBuf, 4, 0, 16);
	ptDataBlock->usOrigDataSize = FAI_UTIL_GetBits(pucBuf, 6, 0, 16);
	ptDataBlock->ulDataAddr 	= FAI_UTIL_GetBits(pucBuf, 8, 0, 32);
	
	FAI_PRINT(FAI_PRT_DBGINFO, ("[FAI_UTIL_GetHdfDataBlock] : usSize=%x\n", ptDataBlock->usSize));
	FAI_PRINT(FAI_PRT_DBGINFO, ("[FAI_UTIL_GetHdfDataBlock] : usCRC16=%x\n", ptDataBlock->usCRC16));
	FAI_PRINT(FAI_PRT_DBGINFO, ("[FAI_UTIL_GetHdfDataBlock] : usFlag=%x\n", ptDataBlock->usFlag));
	FAI_PRINT(FAI_PRT_DBGINFO, ("[FAI_UTIL_GetHdfDataBlock] : usOrigDataSize=%x\n", ptDataBlock->usOrigDataSize));
	FAI_PRINT(FAI_PRT_DBGINFO, ("[FAI_UTIL_GetHdfDataBlock] : ulDataAddr=%x\n", ptDataBlock->ulDataAddr));

	/* Data block Crc check */
	if( FAI_UTIL_GetCRC16(0, pucBuf+4, ptDataBlock->usSize-2) != ptDataBlock->usCRC16 )
	{
		FAI_PRINT(FAI_PRT_ERROR, ("[FAI_UTIL_GetHdfDataBlock] : Crc Error!!\n"));
		return FAI_ERR; 
	}

	if( ptDataBlock->usFlag & HDF_DATA_TYPE_OBSCURE )
	{
		/* todo: */
		FAI_PRINT(FAI_PRT_ERROR, ("[FAI_UTIL_GetHdfDataBlock] : HDF_DATA_TYPE_OBSCURE!!\n"));
		return FAI_NO_ERR;
	}
	
	if( ptDataBlock->usFlag & HDF_DATA_TYPE_COMPRESS ) /* compressed data?? */
	{
		Decompress(pucBuf+12, ptDataBlock->aucData, ptDataBlock->usSize-10, ptDataBlock->usOrigDataSize);
	}
	else
	{
		FVK_MEM_Copy(ptDataBlock->aucData, pucBuf+12, ptDataBlock->usOrigDataSize);
	}

	return FAI_NO_ERR;
}

/*******************************************************************************
* function : FAI_UTIL_CRC16Byte
* description :
* input :
* output :
* return :
* 기타 참고자료 및 파일
*******************************************************************************/
static unsigned short INTRN_FAI_UTIL_CRC16Byte(unsigned short usCrc, const unsigned char ucData)
{
  	return (usCrc >> 8) ^ ausCrc16Table[(usCrc ^ ucData) & 0xff];
}

/*******************************************************************************
* function : FAI_UTIL_CRC16Byte
* description :
* input :
* output :
* return :
* 기타 참고자료 및 파일
*******************************************************************************/
unsigned short FAI_UTIL_GetCRC16(unsigned short usCrc, unsigned char *pucBuffer, unsigned int unLen)
{
	while (unLen--)
		usCrc = INTRN_FAI_UTIL_CRC16Byte(usCrc, *pucBuffer++);
  	return usCrc;
}

/*******************************************************************************
* function : FAI_UTIL_GetCRC32
* description :
* input :
* output :
* return :
* 기타 참고자료 및 파일
*******************************************************************************/
unsigned long FAI_UTIL_GetCRC32(unsigned long ulCrc, unsigned char *pucBuffer, unsigned int unLen)
{
	register unsigned long	crc, *tbl;
	unsigned char		*p = (unsigned char *)pucBuffer;

	crc = ulCrc;
	tbl = (unsigned long *)aulCrc32Table;
	while (unLen--)
	{
		crc = (crc << 8) ^ tbl[((crc >> 24) ^ *p++) & 0xff];
	}
	return crc;
}

unsigned long FAI_UTIL_GetCRC32Fast(void *inP, unsigned long NumberOfByte)
{
	register unsigned long		CRC ;
	unsigned long *aligned_4;
	unsigned long pre_count, end_count,loop_4;
	unsigned long data;
	unsigned char *BufferPtr = inP;

	CRC = 0xFFFFFFFF;	
	if (NumberOfByte < 4)
	{
		return FAI_UTIL_GetCRC32(CRC, inP, NumberOfByte);
	}
	pre_count = ((unsigned long)BufferPtr & 0x03);
	if(pre_count!=0)
		pre_count = 4-pre_count;
	aligned_4 = (unsigned long *)((unsigned long)BufferPtr + pre_count);
	loop_4 = (NumberOfByte-pre_count)>>2;
	end_count = NumberOfByte-(loop_4<<2)-pre_count;

	while((pre_count--)!=0)
	{
		CRC = (CRC << 8) ^ aulCrc32Table[((CRC >> 24) ^ *BufferPtr++) & 0xff] ;
	}

	BufferPtr=(unsigned char *)aligned_4+(loop_4<<2);

	while((loop_4--)!=0)
	{
		data= *aligned_4++;
#if defined(CONFIG_BIG_ENDIAN)
		CRC = (CRC << 8) ^ aulCrc32Table[((CRC >> 24) ^ (data>>24)) & 0xff];
		CRC = (CRC << 8) ^ aulCrc32Table[((CRC >> 24) ^ (data>>16)) & 0xff];
		CRC = (CRC << 8) ^ aulCrc32Table[((CRC >> 24) ^ (data>> 8)) & 0xff];
		CRC = (CRC << 8) ^ aulCrc32Table[((CRC >> 24) ^ (data	 )) & 0xff];
#else
		CRC = (CRC << 8) ^ aulCrc32Table[((CRC >> 24) ^ (data	 )) & 0xff];
		CRC = (CRC << 8) ^ aulCrc32Table[((CRC >> 24) ^ (data>> 8)) & 0xff];
		CRC = (CRC << 8) ^ aulCrc32Table[((CRC >> 24) ^ (data>>16)) & 0xff];
		CRC = (CRC << 8) ^ aulCrc32Table[((CRC >> 24) ^ (data>>24)) & 0xff];
#endif
	}

	while((end_count--)!=0)
	{
		CRC = (CRC << 8) ^ aulCrc32Table[((CRC >> 24) ^ *BufferPtr++) & 0xff] ;
	}

	return CRC;

}

/*set prevCRC as 0xFFFFFFFF as initial value.*/
unsigned long FAI_UTIL_GetCRC32Fast_Partial(unsigned long prevCRC, void *inP, unsigned long NumberOfByte)
{
	register unsigned long		CRC ;
	unsigned long *aligned_4;
	unsigned long pre_count, end_count,loop_4;
	unsigned long data;
	unsigned char *BufferPtr = inP;

	CRC = prevCRC;	
	if (NumberOfByte < 4)
	{
		return FAI_UTIL_GetCRC32(CRC, inP, NumberOfByte);
	}
	pre_count = ((unsigned long)BufferPtr & 0x03);
	if(pre_count!=0)
		pre_count = 4-pre_count;
	aligned_4 = (unsigned long *)((unsigned long)BufferPtr + pre_count);
	loop_4 = (NumberOfByte-pre_count)>>2;
	end_count = NumberOfByte-(loop_4<<2)-pre_count;

	while((pre_count--)!=0)
	{
		CRC = (CRC << 8) ^ aulCrc32Table[((CRC >> 24) ^ *BufferPtr++) & 0xff] ;
	}

	BufferPtr=(unsigned char *)aligned_4+(loop_4<<2);

	while((loop_4--)!=0)
	{
		data= *aligned_4++;
#if defined(CONFIG_BIG_ENDIAN)
		CRC = (CRC << 8) ^ aulCrc32Table[((CRC >> 24) ^ (data>>24)) & 0xff];
		CRC = (CRC << 8) ^ aulCrc32Table[((CRC >> 24) ^ (data>>16)) & 0xff];
		CRC = (CRC << 8) ^ aulCrc32Table[((CRC >> 24) ^ (data>> 8)) & 0xff];
		CRC = (CRC << 8) ^ aulCrc32Table[((CRC >> 24) ^ (data	 )) & 0xff];
#else
		CRC = (CRC << 8) ^ aulCrc32Table[((CRC >> 24) ^ (data	 )) & 0xff];
		CRC = (CRC << 8) ^ aulCrc32Table[((CRC >> 24) ^ (data>> 8)) & 0xff];
		CRC = (CRC << 8) ^ aulCrc32Table[((CRC >> 24) ^ (data>>16)) & 0xff];
		CRC = (CRC << 8) ^ aulCrc32Table[((CRC >> 24) ^ (data>>24)) & 0xff];
#endif
	}

	while((end_count--)!=0)
	{
		CRC = (CRC << 8) ^ aulCrc32Table[((CRC >> 24) ^ *BufferPtr++) & 0xff] ;
	}

	return CRC;

}


/*******************************************************************************
* function : 
* description :
* input :
* output :
* return :
* 기타 참고자료 및 파일
*******************************************************************************/
unsigned char INTRN_FAI_UTIL_GetBit(unsigned char ucByte, unsigned char ucIdx)
{
#if 1
	return (ucByte & aucBitPosMapTbl[ucIdx]) ? 1 : 0;
#else
	return (unsigned char)(ucByte<<ucIdx)>>7;
#endif
}


/*******************************************************************************
* function : 
* description :
* input :
* output :
* return :
* 기타 참고자료 및 파일
*******************************************************************************/
unsigned long INTRN_FAI_UTIL_GetBits(unsigned char *pucBuf, unsigned char ucByteOffset, unsigned char ucBitOffset, unsigned char ucLen)
{
	unsigned char *pucStart;
	unsigned char ucBitIdx;
	unsigned long ulValue = 0;

	pucStart = &pucBuf[ucByteOffset];
	ucBitIdx = ucBitOffset;
	while(ucLen--)
	{
		ulValue <<= 1;
		ulValue |= INTRN_FAI_UTIL_GetBit(*pucStart, ucBitIdx);
		if( ucBitIdx == 7 )
		{
			ucBitIdx = 0;
			pucStart++;
		}
		else
		{
			ucBitIdx++;
		}
	}
	return ulValue;
}

void INTRN_FAI_UTIL_GetBerString(char * cBer, float fBer)
{
	unsigned int	ulExp = 0;
	unsigned int	ulDecimal = 0;
	unsigned int	ulFloat = 0;
	unsigned int	ulBer = (unsigned int)fBer;

	if(ulBer > 999999)
	{
		ulExp = -2;
		ulDecimal = ulBer / 1000000;
		ulFloat = (ulBer % 1000000) / 10000;
	}
	else if(ulBer > 99999)
	{
		ulExp = -3;
		ulDecimal = ulBer / 100000;
		ulFloat = (ulBer % 100000) / 1000;
	}
	else if(ulBer > 9999)
	{
		ulExp = -4;
		ulDecimal = ulBer / 10000;
		ulFloat = (ulBer % 10000) / 100;
	}
	else if(ulBer > 999)
	{
		ulExp = -5;
		ulDecimal = ulBer / 1000;
		ulFloat = (ulBer % 1000) / 10;
	}
	else if(ulBer > 99)
	{
		ulExp = -6;
		ulDecimal = ulBer / 100;
		ulFloat = ulBer % 100;
	}
	else if(ulBer > 9)
	{
		ulExp = -7;
		ulDecimal = ulBer / 10;
		ulFloat = ulBer % 10;
	}
	else
	{
		ulExp = -8;
		ulDecimal = ulBer;
	}

	if(fBer == 0)
	{
		sprintf(cBer, "(0)");
	}
	else
	{
		sprintf(cBer, "%d.%d*E(%d)", ulDecimal, ulFloat, ulExp);
	}
}

void INTRN_FAI_UTIL_GetBerNum(unsigned long *dec,unsigned long *exp,float fBer)
{
	unsigned int	ulExp = 0;
	unsigned int	ulDecimal = 0;
	unsigned int	ulFloat = 0;
	unsigned int	ulBer = (unsigned int)fBer;

	if(ulBer > 999999)
	{
		ulExp = -2;
		ulDecimal = ulBer / 1000000;
		ulFloat = (ulBer % 1000000) / 10000;
		if(ulFloat == 0)
		{
			
		}
		else if(ulFloat < 10 )
		{
			ulExp =-1;
			ulDecimal = ulBer / 100000;
		}
		else
		{
			ulExp =-2;
			ulDecimal = ulBer / 10000;
		}
	}
	else if(ulBer > 99999)
	{
		ulExp = -3;
		ulDecimal = ulBer / 100000;
		ulFloat = (ulBer % 100000) / 1000;
		if(ulFloat == 0)
		{
			
		}
		else if(ulFloat < 10 )
		{
			ulExp =-1;
			ulDecimal = ulBer / 10000;
		}
		else
		{
			ulExp =-2;
			ulDecimal = ulBer / 1000;
		}
	}
	else if(ulBer > 9999)
	{
		ulExp = -4;
		ulDecimal = ulBer / 10000;
		ulFloat = (ulBer % 10000) / 100;
		if(ulFloat == 0)
		{
			
		}
		else if(ulFloat < 10 )
		{
			ulExp =-1;
			ulDecimal = ulBer / 1000;
		}
		else
		{
			ulExp =-2;
			ulDecimal = ulBer / 100;
		}
	}
	else if(ulBer > 999)
	{
		ulExp = -5;
		ulDecimal = ulBer / 1000;
		ulFloat = (ulBer % 1000) / 10;
		if(ulFloat == 0)
		{
			
		}
		else if(ulFloat < 10 )
		{
			ulExp =-1;
			ulDecimal = ulBer / 100;
		}
		else
		{
			ulExp =-2;
			ulDecimal = ulBer / 10;
		}
	}
	else if(ulBer > 99)
	{
		ulExp = -6;
		ulDecimal = ulBer / 100;
		ulFloat = ulBer % 100;
		if(ulFloat == 0)
		{
			
		}
		else if(ulFloat < 10 )
		{
			ulExp =-1;
			ulDecimal = ulBer / 10;
		}
		else
		{
			ulExp =-2;
			ulDecimal = ulBer / 1;
		}
	}
	else if(ulBer > 9)
	{
		ulExp = -7;
		ulDecimal = ulBer / 10;
		ulFloat = ulBer % 10;
		
		if(ulFloat == 0)
		{
			
		}
		else if(ulFloat < 10 )
		{
			ulExp =-1;
			ulDecimal = ulBer / 1;
		}
		else
		{
			ulExp =-2;
			ulDecimal = ulBer *10;
		}

	}
	else
	{
		ulExp = -8;
		ulDecimal = ulBer;
	}
	*dec = ulDecimal;
	*exp = ulExp;

}



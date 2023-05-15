/******************************************************************************
* $Workfile:$
* $Modtime:$
* Author :
* description :		 Factory Application Library
*
* Copyright (c) 2008 by Humax Co., Ltd.
* All right reserved
******************************************************************************/
#ifndef _FAI_FLASH_H_
#define _FAI_FLASH_H_

/******************************************************************************
* header and definitions
******************************************************************************/
#include "fai_err.h"

/******************************************************************************
* Definitions, typdef & const data
******************************************************************************/
typedef enum
{
	FAI_FLASH_TMD_ERR_FILE_INVALID = 0,
	FAI_FLASH_TMD_ERR_FILE_OPEN_FAIL ,
	FAI_FLASH_TMD_ERR_FILE_SEEK_FAIL ,
	FAI_FLASH_TMD_ERR_FILE_READ_FAIL ,
	FAI_FLASH_TMD_ERR_NAND_READ_FAIL ,
	FAI_FLASH_TMD_ERR_NAND_WRITE_FAIL ,
	FAI_FLASH_TMD_ERR_NAND_SYNC_FAIL ,
	FAI_FLASH_TMD_ERR_NOR_READ_FAIL ,
	FAI_FLASH_TMD_ERR_ERASE_FAIL ,
	FAI_FLASH_TMD_ERR_MALLOC_FAIL ,
	FAI_FLASH_TMD_ERR_VALID_FAIL ,
	FAI_FLASH_TMD_ERR_NO_IMAGE ,
	FAI_FLASH_TMD_ERR_INVALID_FLASH_TYPE,
	FAI_FLASH_TMD_ERR_UNKNOWN,
	FAI_FLASH_TMD_ERR_NO					 /* err 없슴 */
} FAI_FLASH_TMD_ERR;

typedef enum
{
	FAI_FLASH_IMAGE_PRELOADER		=0x00,
	FAI_FLASH_IMAGE_LOADER			=0x01,
	FAI_FLASH_IMAGE_APPLICATION		=0x02,
	FAI_FLASH_IMAGE_HUMAXTV			=0x03,
	FAI_FLASH_IMAGE_ROOTFS			=0x04,
	FAI_FLASH_IMAGE_LINUX			=0x05,
	FAI_FLASH_IMAGE_SHARE			=0x06,
	
	FAI_FLASH_IMAGE_PRELOADER2		=0x10,
	FAI_FLASH_IMAGE_LOADER2			=0x11,
	FAI_FLASH_IMAGE_APPLICATION2	=0x12,
	FAI_FLASH_IMAGE_HUMAXTV2		=0x13,
	FAI_FLASH_IMAGE_ROOTFS2			=0x14,
	FAI_FLASH_IMAGE_LINUX2			=0x15,
	FAI_FLASH_IMAGE_SHARE2			=0x16
}FAI_FLASH_IMAGE_TYPE;
/******************************************************************************
* Global variables and structures
******************************************************************************/
/* global variables and structures */

/******************************************************************************
* External variables and functions
******************************************************************************/
/* external variables  and functions */

/******************************************************************************
* Static variables and structures
******************************************************************************/
/* static variables and structures */


/******************************************************************************
* Static function prototypes
******************************************************************************/
/* static function prototypes */

/*******************************************************************************
* function :
* description :
* input :
* output :
* return :
* 기타 참고자료 및 파일
*******************************************************************************/
FAI_ERR_CODE FAI_FLASH_Init();

/*******************************************************************************
* function :
* description :
* input :
* output :
* return :
* 기타 참고자료 및 파일
*******************************************************************************/
FAI_ERR_CODE FAI_FLASH_DataCompare(unsigned short usMsgId,unsigned char ucArgc, unsigned char *pucArgList);

/*******************************************************************************
* function :
* description :
* input :
* output :
* return :
* 기타 참고자료 및 파일
*******************************************************************************/
FAI_ERR_CODE FAI_FLASH_CrcCheck(unsigned short usMsgId,unsigned char ucArgc, unsigned char *pucArgList);

/*******************************************************************************
* function :
* description :
* input :
* output :
* return :
* 기타 참고자료 및 파일
*******************************************************************************/
FAI_ERR_CODE FAI_FLASH_EraseCheck(unsigned short usMsgId,unsigned char ucArgc, unsigned char *pucArgList);

/*******************************************************************************
* function :
* description :
* input :
* output :
* return :
* 기타 참고자료 및 파일
*******************************************************************************/
FAI_ERR_CODE FAI_FLASH_PatternTest(unsigned short usMsgId,unsigned char ucArgc, unsigned char *pucArgList);


/*******************************************************************************
* function :
* description :
* input :
* output :
* return :
* 기타 참고자료 및 파일
*******************************************************************************/
FAI_ERR_CODE FAI_FLASH_NandDownload(unsigned short usMsgId,unsigned char ucArgc, unsigned char *pucArgList);

/*******************************************************************************
* function :
* description :
* input :
* output :
* return :
* 기타 참고자료 및 파일
*******************************************************************************/
FAI_ERR_CODE FAI_FLASH_eMMCDownload(unsigned short usMsgId,unsigned char ucArgc, unsigned char *pucArgList);


/*******************************************************************************
* function :
* description :
* input :
* output :
* return :
* 기타 참고자료 및 파일
*******************************************************************************/
FAI_ERR_CODE FAI_FLASH_DataCompare_UI(void);

/*******************************************************************************
* function :
* description :
* input :
* output :
* return :
* 기타 참고자료 및 파일
*******************************************************************************/
FAI_ERR_CODE FAI_FLASH_FlashCRC_UI(void);


/*******************************************************************************
* function :
* description :
* input :
* output :
* return :
* 기타 참고자료 및 파일
*******************************************************************************/
FAI_ERR_CODE FAI_FLASH_NandDownload_UI(void);

/*******************************************************************************
* function :
* description :
* input :
* output :
* return :
* 기타 참고자료 및 파일
*******************************************************************************/
FAI_ERR_CODE FAI_FLASH_eMMCDownload_UI(void);

/*******************************************************************************
* function :
* description :
* input :
* output :
* return :
* 기타 참고자료 및 파일
*******************************************************************************/
FAI_ERR_CODE FAI_FLASH_BadBlock(unsigned short usMsgId,unsigned char ucArgc, unsigned char *pucArgList);

/*******************************************************************************
* function :
* description :
* input :
* output :
* return :
* 기타 참고자료 및 파일
*******************************************************************************/
FAI_ERR_CODE FAI_FLASH_NandBadBlock_UI(void);
#endif


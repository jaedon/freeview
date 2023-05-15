/******************************************************************************
* $Workfile:$
* $Modtime:$
* Author :
* description :		 Factory Application Library
*
* Copyright (c) 2008 by Humax Co., Ltd.
* All right reserved
******************************************************************************/
#ifndef _FAI_HDD_H_
#define _FAI_HDD_H_

/******************************************************************************
* header and definitions
******************************************************************************/
#include "fai_err.h"

/******************************************************************************
* Definitions, typdef & const data
******************************************************************************/
typedef enum
{
	FAI_HDD_TMD_ERR_DISCONNECT = 0,
	FAI_HDD_TMD_ERR_OPEN_FAIL,
	FAI_HDD_TMD_ERR_WRITE_FAIL,
	FAI_HDD_TMD_ERR_READ_FAIL,
	FAI_HDD_TMD_ERR_VALID_FAIL,
	FAI_HDD_TMD_ERR_CLOSE_FAIL,
	FAI_HDD_TMD_ERR_GETINFO_FAIL,
	FAI_HDD_TMD_ERR_GET_DEVNAME_FAIL,
	FAI_HDD_TMD_ERR_DSTOPEN_FAIL,
	FAI_HDD_TMD_ERR_DSTSTS_FAIL,
	FAI_HDD_TMD_ERR_ATASMART_EN_FAIL,
	FAI_HDD_TMD_ERR_ATASMART_OFFLINE_FAIL,
	FAI_HDD_TMD_ERR_ATASMART_RD_ATTR_FAIL,
	FAI_HDD_TMD_ERR_DST_NOT_STARTED_FAIL,
	FAI_HDD_TMD_ERR_UNKNOWN,
	FAI_HDD_TMD_ERR_NO
} FAI_HDD_TMD_ERR;

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
FAI_ERR_CODE FAI_HDD_Init(void);

/*******************************************************************************
* function :
* description :
* input :
* output :
* return :
* 기타 참고자료 및 파일
*******************************************************************************/
FAI_ERR_CODE FAI_HDD_Format(unsigned short usMsgId,unsigned char ucArgc, unsigned char *pucArgList);

/*******************************************************************************
* function :
* description :
* input :
* output :
* return :
* 기타 참고자료 및 파일
*******************************************************************************/
FAI_ERR_CODE FAI_HDD_DataCompare(unsigned short usMsgId,unsigned char ucArgc, unsigned char *pucArgList);

/*******************************************************************************
* function :
* description :
* input :
* output :
* return :
* 기타 참고자료 및 파일
*******************************************************************************/
FAI_ERR_CODE FAI_HDD_GetInfo(unsigned short usMsgId,unsigned char ucArgc, unsigned char *pucArgList);

/*******************************************************************************
* function :
* description :
* input :
* output :
* return :
* 기타 참고자료 및 파일
*******************************************************************************/
FAI_ERR_CODE FAI_HDD_SetParking(unsigned short usMsgId,unsigned char ucArgc, unsigned char *pucArgList);

/*******************************************************************************
* function :
* description :
* input :
* output :
* return :
* 기타 참고자료 및 파일
*******************************************************************************/
FAI_ERR_CODE FAI_HDD_Defect(unsigned short usMsgId,unsigned char ucArgc, unsigned char *pucArgList);

/*******************************************************************************
* function :
* description :
* input :
* output :
* return :
* 기타 참고자료 및 파일
*******************************************************************************/
FAI_ERR_CODE FAI_HDD_ReadPOH(unsigned short usMsgId,unsigned char ucArgc, unsigned char *pucArgList);

/*******************************************************************************
* function :
* description :
* input :
* output :
* return :
* 기타 참고자료 및 파일
*******************************************************************************/
FAI_ERR_CODE FAI_HDD_Smart_ShortDST(unsigned short usMsgId,unsigned char ucArgc, unsigned char *pucArgList);

/*******************************************************************************
* function :
* description :
* input :
* output :
* return :
* 기타 참고자료 및 파일
*******************************************************************************/
FAI_ERR_CODE FAI_HDD_Format_UI(void);


/*******************************************************************************
* function :
* description :
* input :
* output :
* return :
* 기타 참고자료 및 파일
*******************************************************************************/
FAI_ERR_CODE FAI_HDD_Test_UI(void);

#endif


/******************************************************************************
* $Workfile:$
* $Modtime:$
* Author :
* description :		 Factory Application Library
*
* Copyright (c) 2008 by Humax Co., Ltd.
* All right reserved
******************************************************************************/
#ifndef _FAI_EEPROM_H_
#define _FAI_EEPROM_H_

/******************************************************************************
* header and definitions
******************************************************************************/
#include "fai_err.h"

/******************************************************************************
* Definitions, typdef & const data
******************************************************************************/
typedef enum
{
	FAI_EEPROM_TMD_ERR_RD_FAIL =0,
	FAI_EEPROM_TMD_ERR_WR_FAIL,
	FAI_EEPROM_TMD_ERR_VALID_FAIL,
	FAI_EEPROM_TMD_ERR_MALLOC_FAIL,
	FAI_EEPROM_TMD_ERR_CAPA_FAIL,
	FAI_EEPROM_TMD_ERR_UNKNOWN,
	FAI_EEPROM_TMD_ERR_NO					 /* err 없슴 */
} FAI_EEPROM_TMD_ERR;

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
FAI_ERR_CODE FAI_EEPROM_Init(void);

/*******************************************************************************
* function :
* description :
* input :
* output :
* return :
* 기타 참고자료 및 파일
*******************************************************************************/
FAI_ERR_CODE FAI_EEPROM_Initialize_UI(void);

/*******************************************************************************
* function :
* description :
* input :
* output :
* return :
* 기타 참고자료 및 파일
*******************************************************************************/
FAI_ERR_CODE FAI_EEPROM_EraseAll_UI(void);

/*******************************************************************************
* function :
* description :
* input :
* output :
* return :
* 기타 참고자료 및 파일
*******************************************************************************/
FAI_ERR_CODE FAI_EEPROM_EraseAll(unsigned short usMsgId,unsigned char ucArgc, unsigned char *pucArgList);

/*******************************************************************************
* function :
* description :
* input :
* output :
* return :
* 기타 참고자료 및 파일
*******************************************************************************/
FAI_ERR_CODE FAI_EEPROM_Erase(unsigned short usMsgId,unsigned char ucArgc, unsigned char *pucArgList);

/*******************************************************************************
* function :
* description :
* input :
* output :
* return :
* 기타 참고자료 및 파일
*******************************************************************************/
FAI_ERR_CODE FAI_EEPROM_Test(unsigned short usMsgId,unsigned char ucArgc, unsigned char *pucArgList);

/*******************************************************************************
* function :
* description :
* input :
* output :
* return :
* 기타 참고자료 및 파일
*******************************************************************************/
FAI_ERR_CODE FAI_EEPROM_Initialize(unsigned short usMsgId,unsigned char ucArgc, unsigned char *pucArgList);



#endif


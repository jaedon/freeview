/******************************************************************************
* $Workfile:$
* $Modtime:$
* Author :
* description :		 Factory Application Library
*
* Copyright (c) 2008 by Humax Co., Ltd.
* All right reserved
******************************************************************************/
#ifndef _FAI_MMC_H_
#define _FAI_MMC_H_

/******************************************************************************
* header and definitions
******************************************************************************/
#include "fai_err.h"

/******************************************************************************
* Definitions, typdef & const data
******************************************************************************/

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
FAI_ERR_CODE FAI_MMC_Init(void);

/*******************************************************************************
* function : 
* description : 
* input :
* output :
* return :
* 기타 참고자료 및 파일
*******************************************************************************/
FAI_ERR_CODE FAI_MMC_CheckStatus(unsigned char ucArgc, unsigned char *pucArgList);
/*******************************************************************************
* function : 
* description : 
* input :
* output :
* return :
* 기타 참고자료 및 파일
*******************************************************************************/
FAI_ERR_CODE FAI_MMC_ReadTest(unsigned char ucArgc, unsigned char *pucArgList);


/*******************************************************************************
* function : 
* description : 
* input :
* output :
* return :
* 기타 참고자료 및 파일
*******************************************************************************/
FAI_ERR_CODE FAI_MMC_GetWriteLockState(unsigned char ucArgc, unsigned char *pucArgList);
/*******************************************************************************
* function : 
* description : 
* input :
* output :
* return :
* 기타 참고자료 및 파일
*******************************************************************************/

FAI_ERR_CODE FAI_MMC_RESULT_WriteLockState(unsigned char ucArgc, unsigned char *pucArgList);
/*******************************************************************************
* function : 
* description : 
* input :
* output :
* return :
* 기타 참고자료 및 파일
*******************************************************************************/

FAI_ERR_CODE FAI_MMC_Test_UI(void);

FAI_ERR_CODE FAI_MMC_File_Read(unsigned short usMsgId,unsigned char ucArgc, unsigned char *pucArgList);
FAI_ERR_CODE FAI_MMC_File_Write(unsigned short usMsgId,unsigned char ucArgc, unsigned char *pucArgList);
FAI_ERR_CODE FAI_MMC_File_Delete(unsigned short usMsgId,unsigned char ucArgc, unsigned char *pucArgList);
FAI_ERR_CODE FAI_MMC_Format(unsigned short usMsgId,unsigned char ucArgc, unsigned char *pucArgList);
FAI_ERR_CODE FAI_MMC_UploadInfo(unsigned short usMsgId,unsigned char ucArgc, unsigned char *pucArgList);




#endif



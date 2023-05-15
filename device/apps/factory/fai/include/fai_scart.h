/******************************************************************************
* $Workfile:$
* $Modtime:$
* Author :
* description :		 Factory Application Library
*
* Copyright (c) 2008 by Humax Co., Ltd.
* All right reserved
******************************************************************************/
#ifndef _FAI_SCART_H_
#define _FAI_SCART_H_

/******************************************************************************
* header and definitions
******************************************************************************/
#include "fai_err.h"

/******************************************************************************
* Definitions, typdef & const data
******************************************************************************/
typedef enum
{
	FAI_SCART_TMD_ERR_VIDEO_FORMAT = 0,
	FAI_SCART_TMD_ERR_ASPECT_RATIO, 
	FAI_SCART_TMD_ERR_BYPASS
}FAI_SCART_TMD_ERR;

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
FAI_ERR_CODE FAI_SCART_Init(void);

/*******************************************************************************
* function : 
* description : 
* input :
* output :
* return :
* 기타 참고자료 및 파일
*******************************************************************************/
FAI_ERR_CODE FAI_SCART_Output(unsigned short usMsgId,unsigned char ucArgc, unsigned char *pucArgList);

/*******************************************************************************
* function : 
* description : 
* input :
* output :
* return :
* 기타 참고자료 및 파일
*******************************************************************************/
FAI_ERR_CODE FAI_SCART_FBlanking(unsigned char ucArgc, unsigned char *pucArgList);

/*******************************************************************************
* function : 
* description : 
* input :
* output :
* return :
* 기타 참고자료 및 파일
*******************************************************************************/
FAI_ERR_CODE FAI_SCART_Bypass(unsigned short usMsgId,unsigned char ucArgc, unsigned char *pucArgList);

/*******************************************************************************
* function : 
* description : 
* input :
* output :
* return :
* 기타 참고자료 및 파일
*******************************************************************************/
FAI_ERR_CODE FAI_SCART_VcrBypass_UI(void);

/*******************************************************************************
* function : 
* description : 
* input :
* output :
* return :
* 기타 참고자료 및 파일
*******************************************************************************/
FAI_ERR_CODE FAI_SCART_TvBypass_UI(void);

/*******************************************************************************
* function : 
* description : 
* input :
* output :
* return :
* 기타 참고자료 및 파일
*******************************************************************************/
FAI_ERR_CODE FAI_SCART_Output_UI(void);

#endif


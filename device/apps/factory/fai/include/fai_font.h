/******************************************************************************
* $Workfile:$
* $Modtime:$
* Author :
* description :		 Factory Application Library
*
* Copyright (c) 2008 by Humax Co., Ltd.
* All right reserved
******************************************************************************/
#ifndef _FAI_FONT_H_
#define _FAI_FONT_H_

/******************************************************************************
* header and definitions
******************************************************************************/
#include "fdi_osd.h"

#include "fai_err.h"

/******************************************************************************
* Definitions, typdef & const data
******************************************************************************/
#define TEXT_HEIGHT								16

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

/******************************************************************************
* function : FAI_FONT_DrawString
* description :
* input :
* output :
* return :
* 기타 참고자료 및 파일
******************************************************************************/
FAI_ERR_CODE FAI_FONT_DrawString(unsigned int unX,
								unsigned int unY,
								char *pcStr,
								unsigned long ulColor);

/******************************************************************************
* function : FAI_FONT_GetStringWidth
* description :
* input :
* output :
* return :
* 기타 참고자료 및 파일
******************************************************************************/
int FAI_FONT_GetStringWidth(char *pcStr);

#endif /* fai_font.h */


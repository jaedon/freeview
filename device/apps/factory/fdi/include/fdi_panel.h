/******************************************************************************
* $Workfile:$
* $Modtime:$
* Author :
* description :		 Factory System Driver Interface
*
* Copyright (c) 2009 by Humax Co., Ltd.
* All right reserved
******************************************************************************/
#ifndef _FDI_PANEL_H_
#define _FDI_PANEL_H_

/******************************************************************************
* header and definitions
******************************************************************************/

/******************************************************************************
* Definitions, typdef & const data
******************************************************************************/
typedef enum FDI_PANEL_TYPE
{
	FDI_FRONT_PANEL_TYPE_7SEG = 0,
	FDI_FRONT_PANEL_TYPE_VFD,
	FDI_FRONT_PANEL_TYPE_LED,
	FDI_FRONT_PANEL_TYPE_LCD
}FDI_FRONT_PANEL_TYPE;

typedef struct FDI_PANNEL_CAP
{
	FDI_FRONT_PANEL_TYPE 	type;
	unsigned char  			max_length;
}FDI_FRONT_PANEL_CAP;

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
FDI_ERR_CODE FDI_PANEL_Open(void);

/*******************************************************************************
* function : 
* description :
* input :
* output :
* return :
* 기타 참고자료 및 파일
*******************************************************************************/
FDI_ERR_CODE FDI_PANEL_GetCapability(FDI_FRONT_PANEL_CAP *ptFrontCap);

/*******************************************************************************
* function :
* description :
* input :
* output :
* return :
* 기타 참고자료 및 파일
*******************************************************************************/
FDI_ERR_CODE FDI_PANEL_SetPanelPower(BOOL bOnOff);

/*******************************************************************************
* function : 
* description :
* input :
* output :
* return :
* 기타 참고자료 및 파일
*******************************************************************************/
FDI_ERR_CODE FDI_PANEL_Display(unsigned char *pucString);

/*******************************************************************************
* function : 
* description :
* input :
* output :
* return :
* 기타 참고자료 및 파일
*******************************************************************************/
FDI_ERR_CODE FDI_PANEL_ControlAll(BOOL bOnOff);

#endif


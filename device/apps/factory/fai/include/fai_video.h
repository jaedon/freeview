/******************************************************************************
* $Workfile:$
* $Modtime:$
* Author :
* description :		 Factory Application Library
*
* Copyright (c) 2008 by Humax Co., Ltd.
* All right reserved
******************************************************************************/
#ifndef _FAI_VIDEO_H_
#define _FAI_VIDEO_H_

/******************************************************************************
* header and definitions
******************************************************************************/
#include "fai_err.h"

/******************************************************************************
* Definitions, typdef & const data
******************************************************************************/
typedef enum
{
	FAI_DCONNECTOR_TMD_ERR_RESOLUTION = 0,
	FAI_DCONNECTOR_TMD_ERR_ASPECT,
	FAI_DCONNECTOR_TMD_ERR_DPORT_FUNCTION	
}FAI_DCONNECTOR_TMD_ERR;

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
* ��Ÿ �����ڷ� �� ����
*******************************************************************************/
FAI_ERR_CODE FAI_VIDEO_Init(void);

/*******************************************************************************
* function : 
* description : 
* input :
* output :
* return :
* ��Ÿ �����ڷ� �� ����
*******************************************************************************/
FAI_ERR_CODE FAI_VIDEO_SVideoOutput(unsigned short usMsgId,unsigned char ucArgc, unsigned char *pucArgList);

/*******************************************************************************
* function : 
* description : 
* input :
* output :
* return :
* ��Ÿ �����ڷ� �� ����
*******************************************************************************/
FAI_ERR_CODE FAI_VIDEO_CompositeAudioDetection(unsigned short usMsgId,unsigned char ucArgc, unsigned char *pucArgList);

/*******************************************************************************
* function : 
* description : 
* input :
* output :
* return :
* ��Ÿ �����ڷ� �� ����
*******************************************************************************/
FAI_ERR_CODE FAI_VIDEO_DConnectorOutput(unsigned short usMsgId,unsigned char ucArgc, unsigned char *pucArgList);

/*******************************************************************************
* function : 
* description : 
* input :
* output :
* return :
* ��Ÿ �����ڷ� �� ����
*******************************************************************************/
FAI_ERR_CODE FAI_VIDEO_DConnectorOutput_UI(void);

#endif


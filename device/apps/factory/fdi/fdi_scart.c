/******************************************************************************
* $Workfile:$
* $Modtime:$
* Author :
* description :		 Factory Application
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
#include "fdi_common.h"
#include "fdi_scart.h"

#include "di_scart.h"

/*******************************************************************************
* Debug level
*******************************************************************************/

/*******************************************************************************
* Definitions, typdef & const data
*******************************************************************************/

/*******************************************************************************
* External variables and functions
*******************************************************************************/
/* external variables  and functions */

/*******************************************************************************
* Global variables and structures
*******************************************************************************/
/* global variables and structures */

/*******************************************************************************
* function :
* description :
* input :
* output :
* return :
* 기타 참고자료 및 파일
*******************************************************************************/
FDI_ERR_CODE FDI_SCART_Open(void)
{
	DI_SCART_SETTINGS stScartSettings;
	DI_ERR_CODE	eDiErr;

	/* set normal mode */
	DI_SCART_GetSettings(&stScartSettings);
	stScartSettings.eMode=DI_SCART_MODE_NORMAL;
	stScartSettings.stNormalMode.eTvAspectRatioOutput = DI_SCART_AR_4_3;
	stScartSettings.stNormalMode.eTvVideoOutput = DI_SCART_OUTPUT_RGB;
	eDiErr = DI_SCART_SetSettings(&stScartSettings);
	if(eDiErr != DI_ERR_OK)
	{
		return FDI_ERR;
	}
	return FDI_NO_ERR;
}

/*******************************************************************************
* function :
* description :
* input :
* output :
* return :
* 기타 참고자료 및 파일
*******************************************************************************/
FDI_ERR_CODE FDI_SCART_GetCapability(unsigned long *pulNumOfScart)
{
	*pulNumOfScart = 1;
	return FDI_NO_ERR;
}

/******************************************************************************
* function : PDI_SCART_SetBypass
* description :
* input :
* output :
* return :
* 기타 참고자료 및 파일
******************************************************************************/
/*  Video Sync detect 모드로 세팅. */
FDI_ERR_CODE FDI_SCART_SetHwBypass(BOOL bEnable)
{
	DI_ERR_CODE eDiErr;
	DI_SCART_SETTINGS stScartSettings;
	DI_SCART_GetSettings(&stScartSettings);

	if( bEnable == TRUE )
	{
		stScartSettings.eMode=DI_SCART_MODE_HW_BYPASS;

	}
	else
	{
		stScartSettings.eMode=DI_SCART_MODE_NORMAL;
	}
	eDiErr = DI_SCART_SetSettings(&stScartSettings);
	if(eDiErr != DI_ERR_OK)
	{
		return FDI_ERR;
	}

	return FDI_NO_ERR;
}

/******************************************************************************
* function : PDI_SCART_SetBypass
* description :
* input :
* output :
* return :
* 기타 참고자료 및 파일
******************************************************************************/
/*	Video Sync 무시 하도록 세팅. */
FDI_ERR_CODE FDI_SCART_SetBypass(BOOL bEnable)
{
	DI_ERR_CODE eDiErr;
	DI_SCART_SETTINGS stScartSettings;
	DI_SCART_GetSettings(&stScartSettings);

	if( bEnable == TRUE )
	{
		stScartSettings.eMode=DI_SCART_MODE_HW_BYPASS_NO_DET;

	}
	else
	{
		stScartSettings.eMode=DI_SCART_MODE_NORMAL;
	}
	eDiErr = DI_SCART_SetSettings(&stScartSettings);
	if(eDiErr != DI_ERR_OK)
	{
		return FDI_ERR;
	}

	return FDI_NO_ERR;

}

/******************************************************************************
* function : FDI_SCART_SetAspectRatio
* description :
* input :
* output :
* return :
* 기타 참고자료 및 파일
******************************************************************************/
FDI_ERR_CODE FDI_SCART_SetAspectRatio(FDI_SCART_OUTPUT_e eOutput, FDI_SCART_AR_e eAspectRatio)
{
	DI_ERR_CODE eDiErr;
	DI_SCART_SETTINGS stScartSettings;

	DI_SCART_GetSettings(&stScartSettings);

	if( eOutput == FDI_SCART_OUTPUT_TV )
	{
		if( eAspectRatio == FDI_SCART_AR_NONE )
		{
			stScartSettings.stNormalMode.eTvAspectRatioOutput=DI_SCART_NO_SIGNAL;
		}
		else if( eAspectRatio == FDI_SCART_AR_4_3 )
		{
			stScartSettings.stNormalMode.eTvAspectRatioOutput=DI_SCART_AR_4_3;
		}
		else if( eAspectRatio == FDI_SCART_AR_16_9 )
		{
			stScartSettings.stNormalMode.eTvAspectRatioOutput=DI_SCART_AR_16_9;
		}
	}
	else if( eOutput == FDI_SCART_OUTPUT_VCR )
	{
		if( eAspectRatio == FDI_SCART_AR_NONE )
		{
			stScartSettings.stNormalMode.eVcrAspectRatioOutput=DI_SCART_NO_SIGNAL;
		}
		else if( eAspectRatio == FDI_SCART_AR_4_3 )
		{
			stScartSettings.stNormalMode.eVcrAspectRatioOutput=DI_SCART_AR_4_3;
		}
		else if( eAspectRatio == FDI_SCART_AR_16_9 )
		{
			stScartSettings.stNormalMode.eVcrAspectRatioOutput=DI_SCART_AR_16_9;
		}
	}

	eDiErr = DI_SCART_SetSettings(&stScartSettings);
	if(eDiErr != DI_ERR_OK)
	{
		return FDI_ERR;
	}

	return FDI_NO_ERR;
}

/******************************************************************************
* function : FDI_SCART_SetVideoFormat
* description :
* input :
* output :
* return :
* 기타 참고자료 및 파일
******************************************************************************/
FDI_ERR_CODE FDI_SCART_SetVideoFormat(FDI_SCART_OUTPUT_e eOutput, FDI_SCART_VFORMAT_e eVForamt)
{
	DI_ERR_CODE eDiErr;
	DI_SCART_SETTINGS stScartSettings;

	DI_SCART_GetSettings(&stScartSettings);
	if( eOutput == FDI_SCART_OUTPUT_TV )
	{
		if( eVForamt == FDI_SCART_VFORMAT_CVBS )
		{
			stScartSettings.stNormalMode.eTvVideoOutput=DI_SCART_OUTPUT_CVBS;
		}
		else if( eVForamt == FDI_SCART_VFORMAT_RGB )
		{
			stScartSettings.stNormalMode.eTvVideoOutput=DI_SCART_OUTPUT_RGB;
		}
		else if( eVForamt == FDI_SCART_VFORMAT_SVHS )
		{
			stScartSettings.stNormalMode.eTvVideoOutput=DI_SCART_OUTPUT_SVIDEO;
		}
	}
	else if( eOutput == FDI_SCART_OUTPUT_VCR )
	{
		if( eVForamt == FDI_SCART_VFORMAT_CVBS )
		{
			stScartSettings.stNormalMode.eVcrVideoOutput=DI_SCART_OUTPUT_CVBS;
		}
		else if( eVForamt == FDI_SCART_VFORMAT_RGB )
		{
			/* don't exist */
		}
		else if( eVForamt == FDI_SCART_VFORMAT_SVHS )
		{
			stScartSettings.stNormalMode.eVcrVideoOutput=DI_SCART_OUTPUT_SVIDEO;
		}
	}
	eDiErr = DI_SCART_SetSettings(&stScartSettings);
	if(eDiErr != DI_ERR_OK)
	{
		return FDI_ERR;
	}

	return FDI_NO_ERR;
}

/******************************************************************************
* function : FDI_SCART_RegisterCallback
* description :
* input :
* output :
* return :
* 기타 참고자료 및 파일
******************************************************************************/
FDI_ERR_CODE FDI_SCART_RegisterVCRCallback(FDI_SCART_CALLBACK ptnCallback)
{
	DI_ERR_CODE eDiErr;

	eDiErr = DI_SCART_RegisterEventNotify(DS_EVENT_VCR_PWR_CHANGE, ptnCallback);
	if(eDiErr != DI_ERR_OK)
	{
		return FDI_ERR;
	}

	return FDI_NO_ERR;
}


/******************************************************************************
* $Workfile:$
* $Modtime:$
* Author :
* description :		 Factory Application Main
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
#include "ftype.h"
#include "fai_common.h"
#include "fai_init.h"
#include "fai_app.h"
#include "fai_tmd.h"
#include "fdi_common.h"
#include "fdi_system.h"

/* model specific configuration header */
#include "factorysw_config.h"

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
int g_nFaiDebugLevel = FAI_PRT_ERROR;
int g_nFdiDebugLevel = FDI_PRT_DBGINFO;

/*******************************************************************************
* function : FAI_Main
* description :
* input :
* output :
* return :
* 기타 참고자료 및 파일
*******************************************************************************/
int FAI_Main()
{
	BOOL bFlag;

	if( FDI_SYSTEM_CheckFactoryMode(&bFlag) != FDI_NO_ERR )
	{
		return FAI_ERR;
	}
	
	if( bFlag == TRUE )
	{
		FAI_ERR_CHECK(FAI_MODULE_Init());

		FAI_ERR_CHECK(FAI_APP_Init());

#if (TMD_INPUT_TYPE	!= TMD_INPUT_NONE)
		FAI_ERR_CHECK(FAI_TMD_Init());
#endif
#if (FACTORY_IMAGE_TYPE==IMAGE_TYPE_EMBEDDED)
		while(1)
		{
			FVK_TASK_Sleep(1000);
		}
#endif
	}
	return FAI_NO_ERR;
}


/*
 * (c) 2011-2013 Humax Co., Ltd.
 * This program is produced by Humax Co., Ltd. ("Humax") and
 * the proprietary Software of Humax and its licensors. Humax provides you, as an Authorized Licensee,
 * non-assignable, non-transferable and non-exclusive license to use this Software.
 * You acknowledge that this Software contains valuable trade secrets of Humax and by using this Software
 * you agree to the responsibility to take all reasonable efforts to protect the any information
 * you receive from Humax. You are not permitted to duplicate, modify, distribute, sell or lease and
 * reverse engineer or extract the source code of this Software unless you have Humax's written permission to do so.
 * If you have no authorized license, discontinue using this Software immediately.
 *
 * THE SOFTWARE IS PROVIDED "AS IS" AND HUMAX MAKES NO PROMISES, REPRESENTATIONS OR WARRANTIES, EITHER EXPRESS,
 * IMPLIED OR STATUTORY, OR OTHERWISE, WITH RESPECT TO THE SOFTWARE.
 * IN NO EVENT SHALL HUMAX BE LIABLE FOR LOST PROFITS, REVENUES, OR DATA, FINANCIAL LOSSES OR INDIRECT, SPECIAL,
 * CONSEQUENTIAL, EXEMPLARTY OR PUNITIVE DAMAGES WHATSOEVER RELATING TO YOUR USE OR INABILITY TO USE THE SOFTWARE.

 * This License is effective until terminated. You may terminate this License at any time by destroying all copies
 * of the Software including all documentation. This License will terminate immediately without notice from Humax
 * to you if you fail to comply with any provision of this License. Upon termination, you must destroy all copies
 * of the Software and all documentation.

 * The laws of the Republic of Korea will apply to any disputes arising out of or relating to this Copyright Notice.
 * All claims arising out of or relating to this Copyright Notice will be litigated in the Seoul Central District Court,
 * in the Republic of Korea.
*/

/******************************************************************************/
/**
 * @file	  		nx_prism_install_model.c
 *
 * 	Description:  													\n
 *
 * @author															\n
 * @date															\n
 * @Attention		Copyright (c) 2013 HUMAX Co., Ltd. - All rights reserved.
 *
 **/
/******************************************************************************/


/******************************************************************************/
#define _________Private_Include______________________________________________
/******************************************************************************/
#include <nx_prism_install_main_ui.h>
#include <nx_prism_install_model.h>
#include <dlib_base_types.h>

/*******************************************************************/
/********************      Defines         *************************/
/*******************************************************************/

/******************************************************************************/
#define _________Private_Struct_______________________________________________
/******************************************************************************/
typedef struct
{
    DxAntennaType_e lAntType;
    HINT32          index;
} Nx_Antena_Info_t;

/******************************************************************************/
#define _________Private_Macro________________________________________________
/******************************************************************************/


#if defined(CONFIG_PROD_HA7100S)
//Sub Page
#define	NX_PRISM_INSTALL_SUBITEM_NUM_IN_CONTENTS    1 // Install Sub Page Number
static NxInstallPage_t s_NxInstallPages[NX_PRISM_INSTALL_SUBITEM_NUM_IN_CONTENTS] = {eNxInstallPage_Search_Auto};
//Antena Info List
#define NX_PRISM_INSTALL_MAXIMUM_ANTENA_INFO_COUNT  1

static Nx_Antena_Info_t stAntenaInfoArray[NX_PRISM_INSTALL_MAXIMUM_ANTENA_INFO_COUNT] = {
        {eDxANT_TYPE_LNB_ONLY}
};


#elif defined(CONFIG_PROD_HMS1000S)

#define	NX_PRISM_INSTALL_SUBITEM_NUM_IN_CONTENTS    6 // Install Sub Page Number
static NxInstallPage_t s_NxInstallPages[NX_PRISM_INSTALL_SUBITEM_NUM_IN_CONTENTS] = {eNxInstallPage_Welcome,eNxInstallPage_Language,eNxInstallPage_Network,eNxInstallPage_Country,eNxInstallPage_Search,eNxInstallPage_Result};

//#define	NX_PRISM_INSTALL_SUBITEM_NUM_IN_CONTENTS    3 // Install Sub Page Number // molink_ksjung_0151119
//static NxInstallPage_t s_NxInstallPages[NX_PRISM_INSTALL_SUBITEM_NUM_IN_CONTENTS] = {eNxInstallPage_Language, eNxInstallPage_Country,eNxInstallPage_Search}; // molink_ksjung_0151119

#define NX_PRISM_INSTALL_MAXIMUM_ANTENA_INFO_COUNT  1

static Nx_Antena_Info_t stAntenaInfoArray[NX_PRISM_INSTALL_MAXIMUM_ANTENA_INFO_COUNT] = {
        {eDxANT_TYPE_LNB_ONLY}
};

#elif defined(CONFIG_PROD_M1)

#define	NX_PRISM_INSTALL_SUBITEM_NUM_IN_CONTENTS    6 // Install Sub Page Number
static NxInstallPage_t s_NxInstallPages[NX_PRISM_INSTALL_SUBITEM_NUM_IN_CONTENTS] = {eNxInstallPage_Welcome,eNxInstallPage_Language,eNxInstallPage_Network,eNxInstallPage_Country,eNxInstallPage_Search,eNxInstallPage_Result};


#define NX_PRISM_INSTALL_MAXIMUM_ANTENA_INFO_COUNT  1

static Nx_Antena_Info_t stAntenaInfoArray[NX_PRISM_INSTALL_MAXIMUM_ANTENA_INFO_COUNT] = {
        {eDxANT_TYPE_LNB_ONLY}
};


#else

//Default Value BASE

#define	NX_PRISM_INSTALL_SUBITEM_NUM_IN_CONTENTS    6 // Install Sub Page Number
static NxInstallPage_t s_NxInstallPages[NX_PRISM_INSTALL_SUBITEM_NUM_IN_CONTENTS] = {eNxInstallPage_Welcome,eNxInstallPage_Language,eNxInstallPage_Network,eNxInstallPage_Country,eNxInstallPage_Search,eNxInstallPage_Result};

#define NX_PRISM_INSTALL_MAXIMUM_ANTENA_INFO_COUNT  1

static Nx_Antena_Info_t stAntenaInfoArray[NX_PRISM_INSTALL_MAXIMUM_ANTENA_INFO_COUNT] = {
        {eDxANT_TYPE_LNB_ONLY}
};

#endif

/******************************************************************************/
#define _________Private_functions____________________________________________
/******************************************************************************/

ONDK_Result_t   nx_install_model_GetPageInfo(NxInstallPage_t **subPageList, HINT32 *subPageCount)
{
    if(NULL != *subPageList)
    {
        HxLOG_Error("ERROR!!! nx_install_model_GetPageInfo \n");
		return	ONDK_RET_FAIL;
    }

    *subPageList = (NxInstallPage_t *)NX_APP_Malloc (sizeof(NxInstallPage_t) * NX_PRISM_INSTALL_SUBITEM_NUM_IN_CONTENTS);
    HxSTD_memcpy(*subPageList,s_NxInstallPages, sizeof(NxInstallPage_t) * NX_PRISM_INSTALL_SUBITEM_NUM_IN_CONTENTS);
    *subPageCount = NX_PRISM_INSTALL_SUBITEM_NUM_IN_CONTENTS;

    return GWM_RET_OK;
}

ONDK_Result_t   nx_install_model_GetAntenaInfo(DxAntInfo_t **stAntInfo, HINT32 *countAntena)
{

    ONDK_Result_t	eResult = ONDK_RET_OK;
    HERROR			hError = ERR_FAIL;
    HINT32			lAntIndex = 0;
    DxAntInfo_t     *stItemAntInfo;
    DxAntInfo_t     *stRetDxAntInfoArray;

    //need to implementation. it depands on STB Model
	if (NULL != *stAntInfo)
	{
        HxLOG_Error("ERROR!!! nx_install_model_GetAntenaInfo \n");
		return	ONDK_RET_FAIL;
	}

    stRetDxAntInfoArray = (DxAntInfo_t *)NX_APP_Malloc (sizeof(DxAntInfo_t) * NX_PRISM_INSTALL_MAXIMUM_ANTENA_INFO_COUNT);

    for(lAntIndex = 0; lAntIndex < NX_PRISM_INSTALL_MAXIMUM_ANTENA_INFO_COUNT ; lAntIndex++)
    {
        stItemAntInfo = &(stRetDxAntInfoArray[lAntIndex]);
        HxSTD_memset(stItemAntInfo, 0x00, sizeof(DxAntInfo_t));
        hError = NX_ANTENNA_MakeDefaultAntennaInfo(stAntenaInfoArray[lAntIndex].lAntType, stItemAntInfo);
    }


    *stAntInfo = (DxAntInfo_t *)NX_APP_Malloc (sizeof(DxAntInfo_t) * NX_PRISM_INSTALL_MAXIMUM_ANTENA_INFO_COUNT);
    HxSTD_memcpy(*stAntInfo,stRetDxAntInfoArray, sizeof(DxAntInfo_t) * NX_PRISM_INSTALL_MAXIMUM_ANTENA_INFO_COUNT);
    *countAntena = NX_PRISM_INSTALL_MAXIMUM_ANTENA_INFO_COUNT;
    NX_APP_Free(stRetDxAntInfoArray);

	return eResult;
}

/******************************************************************************/
#define _________End_Of_File____________________________________________________
/******************************************************************************/

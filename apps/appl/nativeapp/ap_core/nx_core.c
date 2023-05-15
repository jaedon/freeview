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
 * @file	  		nx_core.c
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
#include <nx_core.h>


/******************************************************************************/
#define _________Private_Macro________________________________________________
/******************************************************************************/


/******************************************************************************/
#define _________Private_Struct_______________________________________________
/******************************************************************************/


/******************************************************************************/
#define _________Private_Value________________________________________________
/******************************************************************************/


/******************************************************************************/
#define _________Private_functions____________________________________________
/******************************************************************************/


/******************************************************************************/
#define _________Private_Prototype____________________________________________
/******************************************************************************/


/******************************************************************************/
#define _________Public_Functions_____________________________________________
/******************************************************************************/

/******************************************************************************/
#define ___________________Public_functions_________________________________
/******************************************************************************/
HERROR NX_CORE_Init(void)
{
	/****************************************
	* NX_PORT init.
	*****************************************/
	HxLOG_Warning("###################### NX_PORT init ######################\n");
	NX_PORT_Init();


	/****************************************
	* NX_CORE init.
	*****************************************/
	HxLOG_Warning("###################### NX_CORE init #####################\n");
	NX_AV_Init();
	NX_METADATA_SEARCH_Init();
	NX_ANTENNA_Init();
	NX_SEARCH_Init();
	NX_CHANNEL_Init();
	NX_SYSTEM_Init();
#if defined(CONFIG_MW_CAS)
	NX_CAS_Init();
#endif

#if defined(CONFIG_PROD_DEVICE_FILE_SYSTEM)
	NX_FILEMGR_STORAGE_Init();
	NX_FILEMGR_BROWSER_Init();
#endif
	NX_SCHEDULER_Init();

#if defined(CONFIG_MW_MM_PVR)
	NX_RECORDER_Init();
#endif

	NX_MEDIA_PLAY_Init();
	NX_PLAYER_Init();

#if	defined(CONFIG_MW_MM_DLNA)
	NX_DLNA_Init();
#endif

#if defined(CONFIG_SUPPORT_NIMA)
	NX_CONTENTS_Init();
#endif

	return	ERR_OK;
}



/******************************************************************************/
#define _________End_Of_File____________________________________________________
/******************************************************************************/


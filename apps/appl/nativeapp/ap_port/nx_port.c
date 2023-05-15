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
 * @file	  		nx_port.c
 *
 * 	Description:  													\n
 *
 * @author															\n
 * @date															\n
 * @Attention		Copyright (c) 2013 HUMAX Co., Ltd. - All rights reserved.
 *
 **/
/******************************************************************************/

#include <nx_port.h>

HERROR	NX_PORT_Init(void)
{
	HERROR	hError = ERR_FAIL;
	HBOOL	fRes = FALSE;

	HxLOG_Warning("# APK_EVENT_Init init\n");
	hError = APK_EVENT_Init();
	if (hError != ERR_OK)
	{
		HxLOG_Error("ERROR init : APK_EVENT_Init !!!!!!!!!!!!!!!\n");
	}

	HxLOG_Warning("# APK_SYSINFO_STB_Init init\n");
	hError = APK_SYSINFO_STB_Init();
	if (hError != ERR_OK)
	{
		HxLOG_Error("ERROR init : APK_SYSINFO_STB_Init !!!!!!!!!!!!!!!\n");
	}

	HxLOG_Warning("# APK_SCENARIO_Init init\n");
	hError = APK_SCENARIO_Init();
	if (hError != ERR_OK)
	{
		HxLOG_Error("ERROR init : APK_SCENARIO_Init !!!!!!!!!!!!!!!\n");
	}

	HxLOG_Warning("# APK_POWER_Init init\n");
	hError = APK_POWER_Init();
	if (hError != ERR_OK)
	{
		HxLOG_Error("ERROR init : APK_POWER_Init !!!!!!!!!!!!!!!\n");
	}

	HxLOG_Warning("# APK_DB_Init init\n");
	hError = APK_DB_Init();
	if (hError != ERR_OK)
	{
		HxLOG_Error("ERROR init : APK_DB_Init / set to Default !!!!!!!!!!!!!!!\n");
		NX_PORT_DB_Default();
	}

	HxLOG_Warning("# APK_META_SVC_Init init\n");
	fRes = APK_META_SVC_Init(TRUE);
	if (fRes != TRUE)
	{
		HxLOG_Error("ERROR init : APK_META_SVC_Init !!!!!!!!!!!!!!!\n");
	}

	HxLOG_Warning("# APK_SYSINFO_TUNER_Init init\n");
	hError = APK_SYSINFO_TUNER_Init();
	if (hError != ERR_OK)
	{
		HxLOG_Error("ERROR init : APK_SYSINFO_TUNER_Init !!!!!!!!!!!!!!!\n");
	}

	HxLOG_Warning("# APK_MEDIA_PLAY_Init init\n");
	hError = APK_MEDIA_PLAY_Init();
	if (hError != ERR_OK)
	{
		HxLOG_Error("ERROR init : APK_MEDIA_PLAY_Init !!!!!!!!!!!!!!!\n");
	}

	HxLOG_Warning("# APK_BUILDER_Init init\n");
	hError = APK_BUILDER_Init();
	if (hError != ERR_OK)
	{
		HxLOG_Error("ERROR init : APK_BUILDER_Init !!!!!!!!!!!!!!!\n");
	}

	HxLOG_Warning("# APK_META_ANTENNA_Init init\n");
	fRes = APK_META_ANTENNA_Init(TRUE);
	if (fRes != TRUE)
	{
		HxLOG_Error("ERROR init : APK_META_ANTENNA_Init !!!!!!!!!!!!!!!\n");
	}

	HxLOG_Warning("# APK_META_QUERY_Init init\n");
	APK_META_QUERY_Init(TRUE);

	HxLOG_Warning("# APK_OUTPUT_PANEL_Init init\n");
	hError = APK_OUTPUT_PANEL_Init();
	if (hError != ERR_OK)
	{
		HxLOG_Error("ERROR init : APK_OUTPUT_PANEL_Init !!!!!!!!!!!!!!!\n");
	}

	HxLOG_Warning("# APK_OUTPUT_AUDIO_Init init\n");
	hError = APK_OUTPUT_AUDIO_Init();
	if (hError != ERR_OK)
	{
		HxLOG_Error("ERROR init : APK_OUTPUT_AUDIO_Init !!!!!!!!!!!!!!!\n");
	}

	HxLOG_Warning("# APK_OUTPUT_VIDEO_Init init\n");
	hError = APK_OUTPUT_VIDEO_Init();
	if (hError != ERR_OK)
	{
		HxLOG_Error("ERROR init : APK_OUTPUT_VIDEO_Init !!!!!!!!!!!!!!!\n");
	}

	HxLOG_Warning("# APK_OUTPUT_HDMI_Init init\n");
	hError = APK_OUTPUT_HDMI_Init();
	if (hError != ERR_OK)
	{
		HxLOG_Error("ERROR init : APK_OUTPUT_HDMI_Init !!!!!!!!!!!!!!!\n");
	}

#if defined(CONFIG_MW_SYS_THERMAL_PROTECTION)
	HxLOG_Warning("# APK_OUTPUT_THERMAL_Init init\n");
	hError = APK_OUTPUT_THERMAL_Init();
	if (hError != ERR_OK)
	{
		HxLOG_Error("ERROR init : APK_OUTPUT_THERMAL_Init !!!!!!!!!!!!!!!\n");
	}
#endif

#if defined(CONFIG_SUPPORT_UMMA)
	HxLOG_Warning("# APK_FILEMGR_STORAGE_Init init\n");
	hError = APK_FILEMGR_STORAGE_Init();
	if (hError != ERR_OK)
	{
		HxLOG_Error("ERROR init : APK_FILEMGR_STORAGE_Init !!!!!!!!!!!!!!!\n");
	}
#endif

#if defined(CONFIG_MW_MM_PVR)
	HxLOG_Warning("# APK_MEDIA_RECORD_Init init\n");
	hError = APK_MEDIA_RECORD_Init();
	if (hError != ERR_OK)
	{
		HxLOG_Error("ERROR init : APK_MEDIA_RECORD_Init !!!!!!!!!!!!!!!\n");
	}

	HxLOG_Warning("# APK_META_RECORD_Init init\n");
	hError = APK_META_RECORD_Init();
	if (hError != ERR_OK)
	{
		HxLOG_Error("ERROR init : APK_META_RECORD_Init !!!!!!!!!!!!!!!\n");
	}
#endif

#if defined(CONFIG_SUPPORT_NIMA)
	HxLOG_Warning("# APK_NETWORK_Init init\n");
	fRes = APK_NETWORK_Init();
	if (fRes != TRUE)
	{
		HxLOG_Error("ERROR init : APK_NETWORK_Init !!!!!!!!!!!!!!!\n");
	}
#endif

#if defined(CONFIG_MW_MM_DLNA)
	APK_DLNADMS_Init();
	APK_DLNADMP_Init();
	APK_DLNADMR_Init();
#endif

#if defined(CONFIG_SUPPORT_UMMA_SAMBA_CLIENT)
	APK_FILEMGR_SAMBACLIENT_Init();
#endif

#if defined(CONFIG_SUPPORT_UMMA_SAMBA_SERVER)
	APK_FILEMGR_SAMBASERVER_Init();
#endif

	HxLOG_Warning("# APK_SWUPDATE_Init init\n");
	APK_SWUPDATE_Init();

#if defined(CONFIG_MW_CAS)
	NX_PORT_CAS_Init();

#if defined(CONFIG_MW_CAS_NAGRA)
	NX_PORT_CAS_NAGRA_Init();
#endif

#if defined(CONFIG_MW_CAS_IRDETO)
	NX_PORT_CAS_IRDETO_Init();
#endif

#if defined(CONFIG_MW_CI)
	APK_CAS_CI_Init();
#endif

#endif	// End of defined(CONFIG_MW_CAS)

	return ERR_OK;
}


/******************************************************************************/
#define _________End_Of_File____________________________________________________
/******************************************************************************/


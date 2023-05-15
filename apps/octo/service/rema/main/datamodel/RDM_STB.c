/**************************************************************
*	@file	rema.c
*	http://www.humaxdigital.com
*	@author			humax
*	@brief			rema
**************************************************************/

/*
* ¨Ï 2011-2012 Humax Co., Ltd.
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
*  CONSEQUENTIAL, EXEMPLARTY OR PUNITIVE DAMAGES WHATSOEVER RELATING TO YOUR USE OR INABILITY TO USE THE SOFTWARE.
*
* This License is effective until terminated. You may terminate this License at any time by destroying all copies
* of the Software including all documentation. This License will terminate immediately without notice from Humax
* to you if you fail to comply with any provision of this License. Upon termination, you must destroy all copies
* of the Software and all documentation.
*
* The laws of the Republic of Korea will apply to any disputes arising out of or relating to this Copyright Notice.
* All claims arising out of or relating to this Copyright Notice will be litigated in the Seoul Central District Court,
* in the Republic of Korea.
*/

#define ___HEADER_FILES___

#include <vkernel.h>
#include <hlib.h>

#include <rema_int.h>

#include "rema_cwmp.h"
#include "RDM_STB.h"
#include "RDM_STB_Capabilities.h"


#define ___DEFINES___

#define ___MACROS___

#define ___TYPEDEF___

#define ___INTERNAL_FUNCTIONS_DEFINED__
#if 0
static hCwmpResult_e get_STB_Dummy(HCWMP_GETTER_PARAM);
static hCwmpResult_e set_STB_Dummy(HCWMP_SETTER_PARAM);
#define ___STATIC_VARIABLES___
/*
DMO_Object_Profile_t stRDMObject={
    {"Device.Services.STBService.{i}.Capabilities",NULL,NULL,NULL,NULL},
};
*/
#define ___INTERNAL_FUNCTIONS___

static hCwmpResult_e get_STB_Dummy(HCWMP_GETTER_PARAM)
{
	REM_FUNC_ENTER;

	REM_FUNC_LEAVE;

	return ehCwmp_R_OK;
}

static hCwmpResult_e set_STB_Dummy(HCWMP_SETTER_PARAM)
{
	REM_FUNC_ENTER;

	REM_FUNC_LEAVE;

	return ehCwmp_R_OK;
}
#define ___PUBLIC_FUNCTIONS___


HBOOL RDM_STB_Init(void)
{
	REM_FUNC_ENTER;

	REM_FUNC_LEAVE;

	return TRUE;
}

HBOOL RDM_STB_Reister(hCwmpDMManager_t *pstDMManager)
{

	REM_FUNC_ENTER;

	rema_cwmp_AddObject(pstDMManager, 
		"Device.Services.STBService", NULL, NULL, NULL, NULL, NULL);

	rema_cwmp_AddObject(pstDMManager, 
	    "Device.Services.STBService.{i}", NULL, NULL, NULL, NULL, NULL);

	rema_cwmp_AddObject(pstDMManager, 
	    "Device.Services.STBService.{i}.Capabilities", NULL, NULL, NULL, NULL, NULL);

	rema_cwmp_AddParameter(pstDMManager, 
		"Device.Services.STBService.{i}.Capabilities.MaxActiveAVStreams",
		ehCwmp_PTYPE_INT, NULL,
		get_STB_Dummy,
		NULL,
		ehCwmp_NFB_NORMAL, false);

	rema_cwmp_AddParameter(pstDMManager, 
		"Device.Services.STBService.{i}.Capabilities.MaxActiveAVPlayers",
		ehCwmp_PTYPE_INT, NULL,
		get_STB_Dummy,
		NULL,
		ehCwmp_NFB_NORMAL, false);

	rema_cwmp_AddObject(pstDMManager, 
	    "Device.Services.STBService.{i}.Capabilities.PVR", NULL, NULL, NULL, NULL, NULL);


	rema_cwmp_AddParameter(pstDMManager, 
	    "Device.Services.STBService.{i}.Capabilities.PVR.MaxIOStreams",
	    ehCwmp_PTYPE_INT, NULL,
	    get_STB_Dummy,
	    NULL,
	    ehCwmp_NFB_NORMAL, false);

	rema_cwmp_AddParameter(pstDMManager, 
	    "Device.Services.STBService.{i}.Capabilities.PVR.MaxRecordingStreams",
	    ehCwmp_PTYPE_INT, NULL,
	    get_STB_Dummy,
	    NULL,
	    ehCwmp_NFB_NORMAL, false);
	rema_cwmp_AddParameter(pstDMManager, 
	    "Device.Services.STBService.{i}.Capabilities.PVR.MaxPlaybackStreams",
	    ehCwmp_PTYPE_INT, NULL,
	    get_STB_Dummy,
	    NULL,
	    ehCwmp_NFB_NORMAL, false);

	rema_cwmp_AddParameter(pstDMManager, 
	    "Device.Services.STBService.{i}.Capabilities.PVR.MaxTimeDelay",
	    ehCwmp_PTYPE_INT, NULL,
	    get_STB_Dummy,
	    NULL,
	    ehCwmp_NFB_NORMAL, false);



	rema_cwmp_AddObject(pstDMManager, 
	    "Device.Services.STBService.{i}.Capabilities.AudioDecoder", NULL, NULL, NULL, NULL, NULL);


	rema_cwmp_AddParameter(pstDMManager, 
	    "Device.Services.STBService.{i}.Capabilities.AudioDecoder.AudioStandards",
	    ehCwmp_PTYPE_STRING, NULL,
	    get_STB_Dummy,
	    NULL,
	    ehCwmp_NFB_NORMAL, false);


	rema_cwmp_AddObject(pstDMManager, 
	    "Device.Services.STBService.{i}.Capabilities.VideoDecoder", NULL, NULL, NULL, NULL, NULL);


	rema_cwmp_AddParameter(pstDMManager, 
	    "Device.Services.STBService.{i}.Capabilities.VideoDecoder.AudioStandards",
	    ehCwmp_PTYPE_STRING, NULL,
	    get_STB_Dummy,
	    NULL,
	    ehCwmp_NFB_NORMAL, false);


	
	rema_cwmp_AddObject(pstDMManager, 
	    "Device.Services.STBService.{i}.Capabilities.VideoDecoder.MPEG2Part2", NULL, NULL, NULL, NULL, NULL);


	rema_cwmp_AddParameter(pstDMManager, 
	    "Device.Services.STBService.{i}.Capabilities.VideoDecoder.MPEG2Part2.AudioStandards",
	    ehCwmp_PTYPE_STRING, NULL,
	    get_STB_Dummy,
	    NULL,
	    ehCwmp_NFB_NORMAL, false);


	rema_cwmp_AddParameter(pstDMManager, 
	    "Device.Services.STBService.{i}.Capabilities.VideoDecoder.MPEG2Part2.ProfileLevelNumberOfEntries",
	    ehCwmp_PTYPE_UINT, NULL,
	    get_STB_Dummy,
	    NULL,
	    ehCwmp_NFB_NORMAL, false);

	rema_cwmp_AddObject(pstDMManager, 
	    "Device.Services.STBService.{i}.Capabilities.VideoDecoder.MPEG2Part2.ProfileLevel.{i}", 
	    NULL, NULL, NULL, NULL, NULL);


	rema_cwmp_AddParameter(pstDMManager, 
	    "Device.Services.STBService.{i}.Capabilities.VideoDecoder.MPEG2Part2.ProfileLevel.{i}.Profile",
	    ehCwmp_PTYPE_STRING, NULL,
	    get_STB_Dummy,
	    NULL,
	    ehCwmp_NFB_NORMAL, false);

	rema_cwmp_AddParameter(pstDMManager, 
	    "Device.Services.STBService.{i}.Capabilities.VideoDecoder.MPEG2Part2.ProfileLevel.{i}.Level",
	    ehCwmp_PTYPE_STRING, NULL,
	    get_STB_Dummy,
	    NULL,
	    ehCwmp_NFB_NORMAL, false);

	rema_cwmp_AddParameter(pstDMManager, 
	    "Device.Services.STBService.{i}.Capabilities.VideoDecoder.MPEG2Part2.ProfileLevel.{i}.MaximumDecodingCapability",
	    ehCwmp_PTYPE_UINT, NULL,
	    get_STB_Dummy,
	    NULL,
	    ehCwmp_NFB_NORMAL, false);


	rema_cwmp_AddObject(pstDMManager, 
	    "Device.Services.STBService.{i}.Capabilities.VideoDecoder.MPEG4Part10", 
	    NULL, NULL, NULL, NULL, NULL);

	rema_cwmp_AddParameter(pstDMManager, 
	    "Device.Services.STBService.{i}.Capabilities.VideoDecoder.MPEG4Part10.AudioStandards",
	    ehCwmp_PTYPE_STRING, NULL,
	    get_STB_Dummy,
	    NULL,
	    ehCwmp_NFB_NORMAL, false);

	rema_cwmp_AddParameter(pstDMManager, 
	    "Device.Services.STBService.{i}.Capabilities.VideoDecoder.MPEG4Part10.ProfileLevelNumberOfEntries",
	    ehCwmp_PTYPE_UINT, NULL,
	    get_STB_Dummy,
	    NULL,
	    ehCwmp_NFB_NORMAL, false);


	rema_cwmp_AddObject(pstDMManager, 
	    "Device.Services.STBService.{i}.Capabilities.VideoDecoder.MPEG4Part10.ProfileLevel.{i}", 
	    NULL, NULL, NULL, NULL, NULL);

	rema_cwmp_AddParameter(pstDMManager, 
	    "Device.Services.STBService.{i}.Capabilities.VideoDecoder.MPEG4Part10.ProfileLevel.{i}.Profile",
	    ehCwmp_PTYPE_STRING, NULL,
	    get_STB_Dummy,
	    NULL,
	    ehCwmp_NFB_NORMAL, false);

	rema_cwmp_AddParameter(pstDMManager, 
	    "Device.Services.STBService.{i}.Capabilities.VideoDecoder.MPEG4Part10.ProfileLevel.{i}.Level",
	    ehCwmp_PTYPE_STRING, NULL,
	    get_STB_Dummy,
	    NULL,
	    ehCwmp_NFB_NORMAL, false);

	rema_cwmp_AddParameter(pstDMManager, 
	    "Device.Services.STBService.{i}.Capabilities.VideoDecoder.MPEG4Part10.ProfileLevel.{i}.MaximumDecodingCapability",
	    ehCwmp_PTYPE_UINT, NULL,
	    get_STB_Dummy,
	    NULL,
	    ehCwmp_NFB_NORMAL, false);



	rema_cwmp_AddObject(pstDMManager, 
	    "Device.Services.STBService.{i}.Capabilities.VideoDecoder.SMPTEVC1", 
	    NULL, NULL, NULL, NULL, NULL);

	rema_cwmp_AddParameter(pstDMManager, 
	    "Device.Services.STBService.{i}.Capabilities.VideoDecoder.SMPTEVC1.AudioStandards",
	    ehCwmp_PTYPE_STRING, NULL,
	    get_STB_Dummy,
	    NULL,
	    ehCwmp_NFB_NORMAL, false);

	rema_cwmp_AddParameter(pstDMManager, 
	    "Device.Services.STBService.{i}.Capabilities.VideoDecoder.SMPTEVC1.ProfileLevelNumberOfEntries",
	    ehCwmp_PTYPE_UINT, NULL,
	    get_STB_Dummy,
	    NULL,
	    ehCwmp_NFB_NORMAL, false);


	rema_cwmp_AddObject(pstDMManager, 
	    "Device.Services.STBService.{i}.Capabilities.VideoDecoder.SMPTEVC1.ProfileLevel.{i}", 
	    NULL, NULL, NULL, NULL, NULL);

	rema_cwmp_AddParameter(pstDMManager, 
	    "Device.Services.STBService.{i}.Capabilities.VideoDecoder.SMPTEVC1.ProfileLevel.{i}.Profile",
	    ehCwmp_PTYPE_STRING, NULL,
	    get_STB_Dummy,
	    NULL,
	    ehCwmp_NFB_NORMAL, false);

	rema_cwmp_AddParameter(pstDMManager, 
	    "Device.Services.STBService.{i}.Capabilities.VideoDecoder.SMPTEVC1.ProfileLevel.{i}.Level",
	    ehCwmp_PTYPE_STRING, NULL,
	    get_STB_Dummy,
	    NULL,
	    ehCwmp_NFB_NORMAL, false);

	rema_cwmp_AddParameter(pstDMManager, 
	    "Device.Services.STBService.{i}.Capabilities.VideoDecoder.SMPTEVC1.ProfileLevel.{i}.MaximumDecodingCapability",
	    ehCwmp_PTYPE_UINT, NULL,
	    get_STB_Dummy,
	    NULL,
	    ehCwmp_NFB_NORMAL, false);

	rema_cwmp_AddObject(pstDMManager, 
	    "Device.Services.STBService.{i}.Capabilities.AudioOutput", 
	    NULL, NULL, NULL, NULL, NULL);

	rema_cwmp_AddParameter(pstDMManager, 
	    "Device.Services.STBService.{i}.Capabilities.AudioOutput.AudioFormats",
	    ehCwmp_PTYPE_STRING, NULL,
	    get_STB_Dummy,
	    NULL,
	    ehCwmp_NFB_NORMAL, false);


	rema_cwmp_AddObject(pstDMManager, 
	    "Device.Services.STBService.{i}.Capabilities.VideoOutput", 
	    NULL, NULL, NULL, NULL, NULL);

	rema_cwmp_AddParameter(pstDMManager, 
	    "Device.Services.STBService.{i}.Capabilities.VideoOutput.CompositeVideoStandards",
	    ehCwmp_PTYPE_STRING, NULL,
	    get_STB_Dummy,
	    NULL,
	    ehCwmp_NFB_NORMAL, false);

	rema_cwmp_AddParameter(pstDMManager, 
	    "Device.Services.STBService.{i}.Capabilities.VideoOutput.VideoFormats",
	    ehCwmp_PTYPE_STRING, NULL,
	    get_STB_Dummy,
	    NULL,
	    ehCwmp_NFB_NORMAL, false);


	rema_cwmp_AddParameter(pstDMManager, 
	    "Device.Services.STBService.{i}.Capabilities.VideoOutput.Macrovision",
	    ehCwmp_PTYPE_STRING, NULL,
	    get_STB_Dummy,
	    NULL,
	    ehCwmp_NFB_NORMAL, false);

	rema_cwmp_AddParameter(pstDMManager, 
	    "Device.Services.STBService.{i}.Capabilities.VideoOutput.HDCP",
	    ehCwmp_PTYPE_STRING, NULL,
	    get_STB_Dummy,
	    NULL,
	    ehCwmp_NFB_NORMAL, false);



	rema_cwmp_AddObject(pstDMManager, 
	    "Device.Services.STBService.{i}.Capabilities.CA", 
	    NULL, NULL, NULL, NULL, NULL);

	rema_cwmp_AddParameter(pstDMManager, 
	    "Device.Services.STBService.{i}.Capabilities.CA.CASystems",
	    ehCwmp_PTYPE_STRING, NULL,
	    get_STB_Dummy,
	    NULL,
	    ehCwmp_NFB_NORMAL, false);


	rema_cwmp_AddObject(pstDMManager, 
	    "Device.Services.STBService.{i}.Capabilities.DRM", 
	    NULL, NULL, NULL, NULL, NULL);

	rema_cwmp_AddParameter(pstDMManager, 
	    "Device.Services.STBService.{i}.Capabilities.DRM.DRMSystems",
	    ehCwmp_PTYPE_STRING, NULL,
	    get_STB_Dummy,
	    NULL,
	    ehCwmp_NFB_NORMAL, false);


	rema_cwmp_AddObject(pstDMManager, 
	    "Device.Services.STBService.{i}.Capabilities.ServiceMonitoring", 
	    NULL, NULL, NULL, NULL, NULL);

	rema_cwmp_AddParameter(pstDMManager, 
	    "Device.Services.STBService.{i}.Capabilities.ServiceMonitoring.MaxActiveMainStreams",
	    ehCwmp_PTYPE_INT, NULL,
	    get_STB_Dummy,
	    NULL,
	    ehCwmp_NFB_NORMAL, false);

	rema_cwmp_AddParameter(pstDMManager, 
	    "Device.Services.STBService.{i}.Capabilities.ServiceMonitoring.MinSampleInterval",
	    ehCwmp_PTYPE_INT, NULL,
	    get_STB_Dummy,
	    NULL,
	    ehCwmp_NFB_NORMAL, false);

	rema_cwmp_AddParameter(pstDMManager, 
	    "Device.Services.STBService.{i}.Capabilities.ServiceMonitoring.MaxReportSamples",
	    ehCwmp_PTYPE_INT, NULL,
	    get_STB_Dummy,
	    NULL,
	    ehCwmp_NFB_NORMAL, false);

	rema_cwmp_AddParameter(pstDMManager, 
	    "Device.Services.STBService.{i}.Capabilities.ServiceMonitoring.HighLevelMetricNames",
	    ehCwmp_PTYPE_STRING, NULL,
	    get_STB_Dummy,
	    NULL,
	    ehCwmp_NFB_NORMAL, false);



	rema_cwmp_AddObject(pstDMManager, 
	    "Device.Services.STBService.{i}.Components", 
	    NULL, NULL, NULL, NULL, NULL);

	rema_cwmp_AddParameter(pstDMManager, 
	    "Device.Services.STBService.{i}.Components.FrontEndNumberOfEntries",
	    ehCwmp_PTYPE_UINT, NULL,
	    get_STB_Dummy,
	    NULL,
	    ehCwmp_NFB_NORMAL, false);

	rema_cwmp_AddParameter(pstDMManager, 
	    "Device.Services.STBService.{i}.Components.AudioDecoderNumberOfEntries",
	    ehCwmp_PTYPE_UINT, NULL,
	    get_STB_Dummy,
	    NULL,
	    ehCwmp_NFB_NORMAL, false);

	rema_cwmp_AddParameter(pstDMManager, 
	    "Device.Services.STBService.{i}.Components.VideoDecoderNumberOfEntries",
	    ehCwmp_PTYPE_UINT, NULL,
	    get_STB_Dummy,
	    NULL,
	    ehCwmp_NFB_NORMAL, false);

	rema_cwmp_AddParameter(pstDMManager, 
	    "Device.Services.STBService.{i}.Components.AudioOutputNumberOfEntries",
	    ehCwmp_PTYPE_UINT, NULL,
	    get_STB_Dummy,
	    NULL,
	    ehCwmp_NFB_NORMAL, false);


	rema_cwmp_AddParameter(pstDMManager, 
	    "Device.Services.STBService.{i}.Components.VideoOutputNumberOfEntries",
	    ehCwmp_PTYPE_UINT, NULL,
	    get_STB_Dummy,
	    NULL,
	    ehCwmp_NFB_NORMAL, false);

	rema_cwmp_AddObject(pstDMManager, 
	    "Device.Services.STBService.{i}.Components.FrontEnd.{i}", 
	    NULL, NULL, NULL, NULL, NULL);

	rema_cwmp_AddParameter(pstDMManager, 
	    "Device.Services.STBService.{i}.Components.FrontEnd.{i}.Status",
	    ehCwmp_PTYPE_STRING, NULL,
	    get_STB_Dummy,
	    NULL,
	    ehCwmp_NFB_NORMAL, false);

	rema_cwmp_AddParameter(pstDMManager, 
	    "Device.Services.STBService.{i}.Components.FrontEnd.{i}.Name",
	    ehCwmp_PTYPE_STRING, NULL,
	    get_STB_Dummy,
	    NULL,
	    ehCwmp_NFB_NORMAL, false);

	rema_cwmp_AddObject(pstDMManager, 
	    "Device.Services.STBService.{i}.Components.AudioDecoder.{i}", 
	    NULL, NULL, NULL, NULL, NULL);

	rema_cwmp_AddParameter(pstDMManager, 
	    "Device.Services.STBService.{i}.Components.AudioDecoder.{i}.Status",
	    ehCwmp_PTYPE_STRING, NULL,
	    get_STB_Dummy,
	    NULL,
	    ehCwmp_NFB_NORMAL, false);
	rema_cwmp_AddParameter(pstDMManager, 
	    "Device.Services.STBService.{i}.Components.AudioDecoder.{i}.Name",
	    ehCwmp_PTYPE_STRING, NULL,
	    get_STB_Dummy,
	    NULL,
	    ehCwmp_NFB_NORMAL, false);
	rema_cwmp_AddParameter(pstDMManager, 
	    "Device.Services.STBService.{i}.Components.AudioDecoder.{i}.AudioStandard",
	    ehCwmp_PTYPE_STRING, NULL,
	    get_STB_Dummy,
	    NULL,
	    ehCwmp_NFB_NORMAL, false);

	rema_cwmp_AddObject(pstDMManager, 
	    "Device.Services.STBService.{i}.Components.VideoDecoder.{i}", 
	    NULL, NULL, NULL, NULL, NULL);

	rema_cwmp_AddParameter(pstDMManager, 
	    "Device.Services.STBService.{i}.Components.VideoDecoder.{i}.Status",
	    ehCwmp_PTYPE_STRING, NULL,
	    get_STB_Dummy,
	    NULL,
	    ehCwmp_NFB_NORMAL, false);

	rema_cwmp_AddParameter(pstDMManager, 
	    "Device.Services.STBService.{i}.Components.VideoDecoder.{i}.Name",
	    ehCwmp_PTYPE_STRING, NULL,
	    get_STB_Dummy,
	    NULL,
	    ehCwmp_NFB_NORMAL, false);
	rema_cwmp_AddParameter(pstDMManager, 
	    "Device.Services.STBService.{i}.Components.VideoDecoder.{i}.MPEG2Part2",
	    ehCwmp_PTYPE_STRING, NULL,
	    get_STB_Dummy,
	    NULL,
	    ehCwmp_NFB_NORMAL, false);
	rema_cwmp_AddParameter(pstDMManager, 
	    "Device.Services.STBService.{i}.Components.VideoDecoder.{i}.MPEG4Part2",
	    ehCwmp_PTYPE_STRING, NULL,
	    get_STB_Dummy,
	    NULL,
	    ehCwmp_NFB_NORMAL, false);
	rema_cwmp_AddParameter(pstDMManager, 
	    "Device.Services.STBService.{i}.Components.VideoDecoder.{i}.MPEG4Part10",
	    ehCwmp_PTYPE_STRING, NULL,
	    get_STB_Dummy,
	    NULL,
	    ehCwmp_NFB_NORMAL, false);

	rema_cwmp_AddParameter(pstDMManager, 
	    "Device.Services.STBService.{i}.Components.VideoDecoder.{i}.SMPTEVC1",
	    ehCwmp_PTYPE_STRING, NULL,
	    get_STB_Dummy,
	    NULL,
	    ehCwmp_NFB_NORMAL, false);

	rema_cwmp_AddParameter(pstDMManager, 
	    "Device.Services.STBService.{i}.Components.VideoDecoder.{i}.ContentAspectRatio",
	    ehCwmp_PTYPE_STRING, NULL,
	    get_STB_Dummy,
	    NULL,
	    ehCwmp_NFB_NORMAL, false);

	rema_cwmp_AddObject(pstDMManager, 
	    "Device.Services.STBService.{i}.Components.AudioOutput.{i}", 
	    NULL, NULL, NULL, NULL, NULL);

	rema_cwmp_AddParameter(pstDMManager, 
	    "Device.Services.STBService.{i}.Components.AudioOutput.{i}.Status",
	    ehCwmp_PTYPE_STRING, NULL,
	    get_STB_Dummy,
	    NULL,
	    ehCwmp_NFB_NORMAL, false);
	rema_cwmp_AddParameter(pstDMManager, 
	    "Device.Services.STBService.{i}.Components.AudioOutput.{i}.Name",
	    ehCwmp_PTYPE_STRING, NULL,
	    get_STB_Dummy,
	    NULL,
	    ehCwmp_NFB_NORMAL, false);
	rema_cwmp_AddParameter(pstDMManager, 
	    "Device.Services.STBService.{i}.Components.AudioOutput.{i}.AudioFormat",
	    ehCwmp_PTYPE_STRING, NULL,
	    get_STB_Dummy,
	    NULL,
	    ehCwmp_NFB_NORMAL, false);
	rema_cwmp_AddParameter(pstDMManager, 
	    "Device.Services.STBService.{i}.Components.AudioOutput.{i}.AudioLevel",
	    ehCwmp_PTYPE_UINT, NULL,
	    get_STB_Dummy,
	    set_STB_Dummy,
	    ehCwmp_NFB_NORMAL, false);

	rema_cwmp_AddParameter(pstDMManager, 
	    "Device.Services.STBService.{i}.Components.AudioOutput.{i}.CancelMute",
	    ehCwmp_PTYPE_BOOL, NULL,
	    get_STB_Dummy,
	    set_STB_Dummy,
	    ehCwmp_NFB_NORMAL, false);



	rema_cwmp_AddObject(pstDMManager, 
	    "Device.Services.STBService.{i}.Components.VideoOutput.{i}", 
	    NULL, NULL, NULL, NULL, NULL);

	rema_cwmp_AddParameter(pstDMManager, 
	    "Device.Services.STBService.{i}.Components.VideoOutput.{i}.Status",
	    ehCwmp_PTYPE_STRING, NULL,
	    get_STB_Dummy,
	    NULL,
	    ehCwmp_NFB_NORMAL, false);
	rema_cwmp_AddParameter(pstDMManager, 
	    "Device.Services.STBService.{i}.Components.VideoOutput.{i}.Name",
	    ehCwmp_PTYPE_STRING, NULL,
	    get_STB_Dummy,
	    NULL,
	    ehCwmp_NFB_NORMAL, false);
	rema_cwmp_AddParameter(pstDMManager, 
	    "Device.Services.STBService.{i}.Components.VideoOutput.{i}.CompositeVideoStandard",
	    ehCwmp_PTYPE_STRING, NULL,
	    get_STB_Dummy,
	    NULL,
	    ehCwmp_NFB_NORMAL, false);
	rema_cwmp_AddParameter(pstDMManager, 
	    "Device.Services.STBService.{i}.Components.VideoOutput.{i}.VideoFormat",
	    ehCwmp_PTYPE_STRING, NULL,
	    get_STB_Dummy,
	    set_STB_Dummy,
	    ehCwmp_NFB_NORMAL, false);
	rema_cwmp_AddParameter(pstDMManager, 
	    "Device.Services.STBService.{i}.Components.VideoOutput.{i}.AspectRatioBehaviour",
	    ehCwmp_PTYPE_STRING, NULL,
	    get_STB_Dummy,
	    NULL,
	    ehCwmp_NFB_NORMAL, false);



	rema_cwmp_AddObject(pstDMManager, 
	    "Device.Services.STBService.{i}.AVStreams", 
	    NULL, NULL, NULL, NULL, NULL);
	rema_cwmp_AddObject(pstDMManager, 
	    "Device.Services.STBService.{i}.AVStreams.AVStream.{i}", 
	    NULL, NULL, NULL, NULL, NULL);

	rema_cwmp_AddParameter(pstDMManager, 
	    "Device.Services.STBService.{i}.AVStreams.AVStream.{i}.Status",
	    ehCwmp_PTYPE_STRING, NULL,
	    get_STB_Dummy,
	    NULL,
	    ehCwmp_NFB_NORMAL, false);
	rema_cwmp_AddParameter(pstDMManager, 
	    "Device.Services.STBService.{i}.AVStreams.AVStream.{i}.Name",
	    ehCwmp_PTYPE_STRING, NULL,
	    get_STB_Dummy,
	    NULL,
	    ehCwmp_NFB_NORMAL, false);
	rema_cwmp_AddParameter(pstDMManager, 
	    "Device.Services.STBService.{i}.AVStreams.AVStream.{i}.FrontEnd",
	    ehCwmp_PTYPE_STRING, NULL,
	    get_STB_Dummy,
	    NULL,
	    ehCwmp_NFB_NORMAL, false);
	rema_cwmp_AddParameter(pstDMManager, 
	    "Device.Services.STBService.{i}.AVStreams.AVStream.{i}.AudioDecoder",
	    ehCwmp_PTYPE_STRING, NULL,
	    get_STB_Dummy,
	    NULL,
	    ehCwmp_NFB_NORMAL, false);
	rema_cwmp_AddParameter(pstDMManager, 
	    "Device.Services.STBService.{i}.AVStreams.AVStream.{i}.VideoDecoder",
	    ehCwmp_PTYPE_STRING, NULL,
	    get_STB_Dummy,
	    NULL,
	    ehCwmp_NFB_NORMAL, false);


	rema_cwmp_AddObject(pstDMManager, 
	    "Device.Services.STBService.{i}.AVPlayers", 
	    NULL, NULL, NULL, NULL, NULL);

	rema_cwmp_AddParameter(pstDMManager, 
	    "Device.Services.STBService.{i}.AVPlayers.ActiveAVPlayers",
	    ehCwmp_PTYPE_UINT, NULL,
	    get_STB_Dummy,
	    NULL,
	    ehCwmp_NFB_NORMAL, false);
	rema_cwmp_AddParameter(pstDMManager, 
	    "Device.Services.STBService.{i}.AVPlayers.AVPlayerNumberOfEntries",
	    ehCwmp_PTYPE_UINT, NULL,
	    get_STB_Dummy,
	    NULL,
	    ehCwmp_NFB_NORMAL, false);
	rema_cwmp_AddParameter(pstDMManager, 
	    "Device.Services.STBService.{i}.AVPlayers.PreferredAudioLanguage",
	    ehCwmp_PTYPE_STRING, NULL,
	    get_STB_Dummy,
	    NULL,
	    ehCwmp_NFB_NORMAL, false);

	rema_cwmp_AddParameter(pstDMManager, 
	    "Device.Services.STBService.{i}.AVPlayers.PreferredSubtitlingLanguage",
	    ehCwmp_PTYPE_STRING, NULL,
	    get_STB_Dummy,
	    NULL,
	    ehCwmp_NFB_NORMAL, false);



	rema_cwmp_AddObject(pstDMManager, 
	    "Device.Services.STBService.{i}.AVPlayers.AVPlayer.{i}", 
	    NULL, NULL, NULL, NULL, NULL);

	rema_cwmp_AddParameter(pstDMManager, 
	    "Device.Services.STBService.{i}.AVPlayers.AVPlayer.{i}.Status",
	    ehCwmp_PTYPE_STRING, NULL,
	    get_STB_Dummy,
	    NULL,
	    ehCwmp_NFB_NORMAL, false);
	rema_cwmp_AddParameter(pstDMManager, 
	    "Device.Services.STBService.{i}.AVPlayers.AVPlayer.{i}.Name",
	    ehCwmp_PTYPE_STRING, NULL,
	    get_STB_Dummy,
	    NULL,
	    ehCwmp_NFB_NORMAL, false);

	rema_cwmp_AddParameter(pstDMManager, 
	    "Device.Services.STBService.{i}.AVPlayers.AVPlayer.{i}.AudioLanguage",
	    ehCwmp_PTYPE_STRING, NULL,
	    get_STB_Dummy,
	    NULL,
	    ehCwmp_NFB_NORMAL, false);

	rema_cwmp_AddParameter(pstDMManager, 
	    "Device.Services.STBService.{i}.AVPlayers.AVPlayer.{i}.SubtitlingStatus",
	    ehCwmp_PTYPE_STRING, NULL,
	    get_STB_Dummy,
	    NULL,
	    ehCwmp_NFB_NORMAL, false);

	rema_cwmp_AddParameter(pstDMManager, 
	    "Device.Services.STBService.{i}.AVPlayers.AVPlayer.{i}.SubtitlingLanguage",
	    ehCwmp_PTYPE_STRING, NULL,
	    get_STB_Dummy,
	    NULL,
	    ehCwmp_NFB_NORMAL, false);

	rema_cwmp_AddParameter(pstDMManager, 
	    "Device.Services.STBService.{i}.AVPlayers.AVPlayer.{i}.AudioOutputs",
	    ehCwmp_PTYPE_STRING, NULL,
	    get_STB_Dummy,
	    NULL,
	    ehCwmp_NFB_NORMAL, false);


	rema_cwmp_AddParameter(pstDMManager, 
	    "Device.Services.STBService.{i}.AVPlayers.AVPlayer.{i}.VideoOutputs",
	    ehCwmp_PTYPE_STRING, NULL,
	    get_STB_Dummy,
	    NULL,
	    ehCwmp_NFB_NORMAL, false);

	rema_cwmp_AddParameter(pstDMManager, 
	    "Device.Services.STBService.{i}.AVPlayers.AVPlayer.{i}.MainStream",
	    ehCwmp_PTYPE_STRING, NULL,
	    get_STB_Dummy,
	    NULL,
	    ehCwmp_NFB_NORMAL, false);

	rema_cwmp_AddParameter(pstDMManager, 
	    "Device.Services.STBService.{i}.AVPlayers.AVPlayer.{i}.PIPStreams",
	    ehCwmp_PTYPE_STRING, NULL,
	    get_STB_Dummy,
	    NULL,
	    ehCwmp_NFB_NORMAL, false);


	HINT32 nPD=-1;
	nPD = rema_cwmp_FindPidOfParam(pstDMManager, "Device.Services.STBService.");
	rema_cwmp_AddMultiObject(pstDMManager, nPD, 1, NULL);
	REM_FUNC_LEAVE;

	return TRUE;
}

void RDM_STB_Deinit(void)
{
	REM_FUNC_ENTER;

	REM_FUNC_LEAVE;
}
#endif
#define ___EXTERNAL_FUNCTIONS___

#define _____END_OF_FILE_________


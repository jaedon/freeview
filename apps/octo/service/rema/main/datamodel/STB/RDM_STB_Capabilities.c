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
#include <hapi.h>
#include <oapi.h>
#include <di_system.h>

#include "rema_cwmp.h"
#include "RDM_STB_Capabilities.h"

#include <di_audio.h>







#define ___DEFINES___

#define ___MACROS___

#define ___TYPEDEF___

#define  DMAX_AUDIOFORMAT 20

#define ___STRUCT___
typedef struct AudTypeStr_t
{
    DI_AUDIO_FORMAT eType;
    HCHAR           szFormat[125];
}AudTypeStr_t;

#define ___STATIC_VARIABLES___

#define ___INTERNAL_FUNCTIONS___
#define ___PUBLIC_FUNCTIONS___

#if  0
//STBService.{i}.Capabilities.AudioDecoder.AudioStandardsÀÇ getter
hCwmpResult_e get_RDM_STB_Capa_Adecoder_AudioStandards(HCWMP_GETTER_PARAM)
{
    /* dead code
    hCwmpResult_e	eRs = ehCwmp_R_INTERNAL_ERROR;
    DI_ERR_CODE		diErr = ERR_FAIL;
    HUINT32		ucNumOfDevice = 0;
    DI_AUDIO_CAP_t	*pCapInfo=NULL;
    DI_AUDIO_FORMAT	eAFormat = DI_AUDFORMAT_NONE;
    int			i=0;
    int			nMax=0;

    diErr = DI_AUDIO_GetCapability(&ucNumOfDevice,&pCapInfo);
    if(DI_ERR_OK != diErr)
    {
	RemLOG_Error ("[%s:%d] err\n", __FUNCTION__, __LINE__);
	return eRs;
    }

    eRs = ehCwmp_R_OK;
    eAFormat = pCapInfo->support_format;
    if(eAFormat != DI_AUDFORMAT_NONE)
    {
	AudTypeStr_t sAudFormat[DMAX_AUDIOFORMAT]={
	    {DI_AUDFORMAT_MPEG, "MPEG1-Part3-Layer2,MPEG2-Part3-Layer2"},
	    {DI_AUDFORMAT_MP3, "MPEG1-Part3-Layer3,MPEG2-Part3-Layer3"},
	    {DI_AUDFORMAT_AAC, "MPEG2-AAC-LC"},
	    {DI_AUDFORMAT_AAC_ADTS, ""},
	  //  {DI_AUDFORMAT_AAC_LOAS, "MPEG2-AAC-LC"}, //DI_AUDFORMAT_AAC¶û µ¿ÀÏÇÑ flag
	    {DI_AUDFORMAT_AAC_PLUS, "MPEG4-AAC-LC"},
	   // {DI_AUDFORMAT_AAC_PLUS_ADTS, "MPEG4-AAC-LC"}, //DI_AUDFORMAT_AAC_PLUS¶û µ¿ÀÏÇÑ flag
	    {DI_AUDFORMAT_AC3, "DOLBY-AC3"},
	    {DI_AUDFORMAT_AC3_PLUS, ""},
	    {DI_AUDFORMAT_PCM, ""},
	    {DI_AUDFORMAT_DTS, "DTS"},
	    {DI_AUDFORMAT_LPCM_HD_DVD, ""},
	    {DI_AUDFORMAT_LPCM_BLUERAY, ""},
	    {DI_AUDFORMAT_DTS_HD, "DTS-HD"},
	    {DI_AUDFORMAT_WMA_STD, ""},
	    {DI_AUDFORMAT_WMA_PRO, ""},
	    {DI_AUDFORMAT_LPCM_DVD, ""},
	    {DI_AUDFORMAT_PCMWAVE, ""},
	    {DI_AUDFORMAT_DRA, ""},
	    {DI_AUDFORMAT_FLAC, ""},
	    {DI_AUDFORMAT_ADPCM, ""},
	    {DI_AUDFORMAT_VORBIS, ""},
	    {DI_AUDFORMAT_APE, ""},
	};
	nMax = DMAX_AUDIOFORMAT;
	for(i=0; i<nMax; i++)
	{
	    if(eAFormat & sAudFormat[i].eType)
	    {
		if(strlen(sAudFormat[i].szFormat) > 0)
		{
		    HxSTD_StrCat(pszBuf, sAudFormat[i].szFormat);
		}
	    }
	}
    }
    
    return eRs;
    */
}
#endif
#define ___EXTERNAL_FUNCTIONS__


#define _____END_OF_FILE_________


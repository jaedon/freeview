/*******************************************************************
	File Description
********************************************************************/
/**
	@file	  svc_si_cmd_es_audio.c
	@brief

	Description:  									\n
	Module: SVC / SI_NEW					\n

	Copyright (c) 2013 HUMAX Co., Ltd.				\n
	All rights reserved.							\n
*/

/*
* Character encoding.
*
* MS949
*
* This source file that uses MS949 encoding.
* MS949 encoding 을 사용하는 source file 입니다.
* MS949 encodingを使用して source fileです。
* Quelldatei, die MS949-Codierung verwendet.
*
*/
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

/*******************************************************************/
/********************      Header Files    *************************/
/*******************************************************************/
#include <hlib.h>
#include <silib.h>
#include <vkernel.h>
#include <octo_common.h>
#include <isosvc.h>

#include <pal_sirawpool.h>
#include <pal_pipe.h>
//#include <mwc_util.h>
#include <svc_si.h>

#include <_svc_si.h>
#include <_svc_si_lib.h>
#include <_svc_si_filtering.h>
#include <_svc_si_search.h>
#include <_svc_si_ut.h>

#include "cutest.h"

#include "_svc_si_cmd.h"

/*******************************************************************/
/********************      Defines         *************************/
/*******************************************************************/

/*******************************************************************/
/********************      Typedef         *************************/
/*******************************************************************/

/*******************************************************************/
/********************      Global Variables     ********************/
/*******************************************************************/

/*******************************************************************/
/********************      Static Variables     ********************/
/*******************************************************************/

STATIC svcSi_AudioEs_t s_astAudioEsList[] =
{
	[0] = {
		.usPid					= 0x1001,
		.ucComponentTag			= 0,
		.eAudCodec				= eDxAUDIO_CODEC_DOLBY_AC3,
		.astIsoLangInfo			= {
			[0] = {  .eAudType	= eAUDIO_TYPE_VISUAL_IMPAIRED,	.szIso639LangCode =	{ 'e', 'n', 'g' }	},
			[1] = {  .eAudType	= eAUDIO_TYPE_UNDEFINED,	.szIso639LangCode =	{ 0, }	},
			[2] = {  .eAudType	= eAUDIO_TYPE_UNDEFINED,	.szIso639LangCode =	{ 0, }	},
			[3] = {  .eAudType	= eAUDIO_TYPE_UNDEFINED,	.szIso639LangCode =	{ 0, }	}
		},
		.bCaDesFound			= FALSE,
		.ucChannelNum			= 1,
		.eSuppAudioEditorial	= eAUDIO_TYPE_SUPP_AD_VISUAL_IMPARED,
		.ucMixType				= 1
	},
	[1] = {
		.usPid					= 0x1002,
		.ucComponentTag			= 0,
		.eAudCodec				= eDxAUDIO_CODEC_DOLBY_AC3,
		.astIsoLangInfo			= {
			[0] = {  .eAudType	= eAUDIO_TYPE_UNDEFINED,	.szIso639LangCode =	{ 'e', 'n', 'g' }	},
			[1] = {  .eAudType	= eAUDIO_TYPE_UNDEFINED,	.szIso639LangCode =	{ 0, }	},
			[2] = {  .eAudType	= eAUDIO_TYPE_UNDEFINED,	.szIso639LangCode =	{ 0, }	},
			[3] = {  .eAudType	= eAUDIO_TYPE_UNDEFINED,	.szIso639LangCode =	{ 0, }	}
		},
		.bCaDesFound			= FALSE,
		.ucChannelNum			= 1,
		.eSuppAudioEditorial	= eAUDIO_TYPE_SUPP_MAIN,
		.ucMixType				= 0
	},
	[2] = {
		.usPid					= 0x1003,
		.ucComponentTag			= 0,
		.eAudCodec				= eDxAUDIO_CODEC_MPEG,
		.astIsoLangInfo			= {
			[0] = {  .eAudType	= eAUDIO_TYPE_VISUAL_IMPAIRED,	.szIso639LangCode =	{ 'e', 'n', 'g' }	},
			[1] = {  .eAudType	= eAUDIO_TYPE_UNDEFINED,	.szIso639LangCode =	{ 0, }	},
			[2] = {  .eAudType	= eAUDIO_TYPE_UNDEFINED,	.szIso639LangCode =	{ 0, }	},
			[3] = {  .eAudType	= eAUDIO_TYPE_UNDEFINED,	.szIso639LangCode =	{ 0, }	}
		},
		.bCaDesFound			= FALSE,
		.ucChannelNum			= 1,
		.eSuppAudioEditorial	= eAUDIO_TYPE_SUPP_AD_VISUAL_IMPARED,
		.ucMixType				= 1,
	},
	[3] = {
		.usPid					= 0x1004,
		.ucComponentTag			= 0,
		.eAudCodec				= eDxAUDIO_CODEC_MPEG,
		.astIsoLangInfo			= {
			[0] = {  .eAudType	= eAUDIO_TYPE_UNDEFINED,	.szIso639LangCode =	{ 'e', 'n', 'g' }	},
			[1] = {  .eAudType	= eAUDIO_TYPE_UNDEFINED,	.szIso639LangCode =	{ 0, }	},
			[2] = {  .eAudType	= eAUDIO_TYPE_UNDEFINED,	.szIso639LangCode =	{ 0, }	},
			[3] = {  .eAudType	= eAUDIO_TYPE_UNDEFINED,	.szIso639LangCode =	{ 0, }	}
		},
		.bCaDesFound			= FALSE,
		.ucChannelNum			= 1,
		.eSuppAudioEditorial	= eAUDIO_TYPE_SUPP_MAIN,
		.ucMixType				= 0
	},
	[4] = {
		.usPid					= 0x1005,
		.ucComponentTag			= 0,
		.eAudCodec				= eDxAUDIO_CODEC_DOLBY_AC3,
		.astIsoLangInfo			= {
			[0] = {  .eAudType	= eAUDIO_TYPE_VISUAL_IMPAIRED,	.szIso639LangCode =	{ 'g', 'e', 'r' }	},
			[1] = {  .eAudType	= eAUDIO_TYPE_UNDEFINED,	.szIso639LangCode =	{ 0, }	},
			[2] = {  .eAudType	= eAUDIO_TYPE_UNDEFINED,	.szIso639LangCode =	{ 0, }	},
			[3] = {  .eAudType	= eAUDIO_TYPE_UNDEFINED,	.szIso639LangCode =	{ 0, }	}
		},
		.bCaDesFound			= FALSE,
		.ucChannelNum			= 1,
		.eSuppAudioEditorial	= eAUDIO_TYPE_SUPP_AD_VISUAL_IMPARED,
		.ucMixType				= 1,
	},
	[5] = {
		.usPid					= 0x1006,
		.ucComponentTag			= 0,
		.eAudCodec				= eDxAUDIO_CODEC_DOLBY_AC3,
		.astIsoLangInfo			= {
			[0] = {  .eAudType	= eAUDIO_TYPE_UNDEFINED,	.szIso639LangCode =	{ 'g', 'e', 'r' }	},
			[1] = {  .eAudType	= eAUDIO_TYPE_UNDEFINED,	.szIso639LangCode =	{ 0, }	},
			[2] = {  .eAudType	= eAUDIO_TYPE_UNDEFINED,	.szIso639LangCode =	{ 0, }	},
			[3] = {  .eAudType	= eAUDIO_TYPE_UNDEFINED,	.szIso639LangCode =	{ 0, }	}
		},
		.bCaDesFound			= FALSE,
		.ucChannelNum			= 1,
		.eSuppAudioEditorial	= eAUDIO_TYPE_SUPP_MAIN,
		.ucMixType				= 0
	},
	[6] = {
		.usPid					= 0x1007,
		.ucComponentTag			= 0,
		.eAudCodec				= eDxAUDIO_CODEC_MPEG,
		.astIsoLangInfo			= {
			[0] = {  .eAudType	= eAUDIO_TYPE_VISUAL_IMPAIRED,	.szIso639LangCode =	{ 'g', 'e', 'r' }	},
			[1] = {  .eAudType	= eAUDIO_TYPE_UNDEFINED,	.szIso639LangCode =	{ 0, }	},
			[2] = {  .eAudType	= eAUDIO_TYPE_UNDEFINED,	.szIso639LangCode =	{ 0, }	},
			[3] = {  .eAudType	= eAUDIO_TYPE_UNDEFINED,	.szIso639LangCode =	{ 0, }	}
		},
		.bCaDesFound			= FALSE,
		.ucChannelNum			= 1,
		.eSuppAudioEditorial	= eAUDIO_TYPE_SUPP_AD_VISUAL_IMPARED,
		.ucMixType				= 1
	},
	[7] = {
		.usPid					= 0x1008,
		.ucComponentTag			= 0,
		.eAudCodec				= eDxAUDIO_CODEC_MPEG,
		.astIsoLangInfo			= {
			[0] = {  .eAudType	= eAUDIO_TYPE_UNDEFINED,	.szIso639LangCode =	{ 'g', 'e', 'r' }	},
			[1] = {  .eAudType	= eAUDIO_TYPE_UNDEFINED,	.szIso639LangCode =	{ 0, }	},
			[2] = {  .eAudType	= eAUDIO_TYPE_UNDEFINED,	.szIso639LangCode =	{ 0, }	},
			[3] = {  .eAudType	= eAUDIO_TYPE_UNDEFINED,	.szIso639LangCode =	{ 0, }	}
		},
		.bCaDesFound			= FALSE,
		.ucChannelNum			= 1,
		.eSuppAudioEditorial	= eAUDIO_TYPE_SUPP_MAIN,
		.ucMixType				= 0
	},
	[8] = {
		.usPid					= 0x1009,
		.ucComponentTag			= 0,
		.eAudCodec				= eDxAUDIO_CODEC_DOLBY_AC3,
		.astIsoLangInfo			= {
			[0] = {  .eAudType	= eAUDIO_TYPE_VISUAL_IMPAIRED,	.szIso639LangCode =	{ 's', 'w', 'e' }	},
			[1] = {  .eAudType	= eAUDIO_TYPE_UNDEFINED,	.szIso639LangCode =	{ 0, }	},
			[2] = {  .eAudType	= eAUDIO_TYPE_UNDEFINED,	.szIso639LangCode =	{ 0, }	},
			[3] = {  .eAudType	= eAUDIO_TYPE_UNDEFINED,	.szIso639LangCode =	{ 0, }	}
		},
		.bCaDesFound			= FALSE,
		.ucChannelNum			= 1,
		.eSuppAudioEditorial	= eAUDIO_TYPE_SUPP_AD_VISUAL_IMPARED,
		.ucMixType				= 1
	},
	[9] = {
		.usPid					= 0x100A,
		.ucComponentTag			= 0,
		.eAudCodec				= eDxAUDIO_CODEC_DOLBY_AC3,
		.astIsoLangInfo			= {
			[0] = {  .eAudType	= eAUDIO_TYPE_UNDEFINED,	.szIso639LangCode =	{ 's', 'w', 'e' }	},
			[1] = {  .eAudType	= eAUDIO_TYPE_UNDEFINED,	.szIso639LangCode =	{ 0, }	},
			[2] = {  .eAudType	= eAUDIO_TYPE_UNDEFINED,	.szIso639LangCode =	{ 0, }	},
			[3] = {  .eAudType	= eAUDIO_TYPE_UNDEFINED,	.szIso639LangCode =	{ 0, }	}
		},
		.bCaDesFound			= FALSE,
		.ucChannelNum			= 1,
		.eSuppAudioEditorial	= eAUDIO_TYPE_SUPP_MAIN,
		.ucMixType				= 0
	},
	[10] = {
		.usPid					= 0x100B,
		.ucComponentTag			= 0,
		.eAudCodec				= eDxAUDIO_CODEC_MPEG,
		.astIsoLangInfo			= {
			[0] = {  .eAudType	= eAUDIO_TYPE_VISUAL_IMPAIRED,	.szIso639LangCode =	{ 's', 'w', 'e' }	},
			[1] = {  .eAudType	= eAUDIO_TYPE_UNDEFINED,	.szIso639LangCode =	{ 0, }	},
			[2] = {  .eAudType	= eAUDIO_TYPE_UNDEFINED,	.szIso639LangCode =	{ 0, }	},
			[3] = {  .eAudType	= eAUDIO_TYPE_UNDEFINED,	.szIso639LangCode =	{ 0, }	}
		},
		.bCaDesFound			= FALSE,
		.ucChannelNum			= 1,
		.eSuppAudioEditorial	= eAUDIO_TYPE_SUPP_AD_VISUAL_IMPARED,
		.ucMixType				= 1,
	},
	[11] = {
		.usPid					= 0x100C,
		.ucComponentTag			= 0,
		.eAudCodec				= eDxAUDIO_CODEC_MPEG,
		.astIsoLangInfo			= {
			[0] = {  .eAudType	= eAUDIO_TYPE_UNDEFINED,	.szIso639LangCode =	{ 's', 'w', 'e' }	},
			[1] = {  .eAudType	= eAUDIO_TYPE_UNDEFINED,	.szIso639LangCode =	{ 0, }	},
			[2] = {  .eAudType	= eAUDIO_TYPE_UNDEFINED,	.szIso639LangCode =	{ 0, }	},
			[3] = {  .eAudType	= eAUDIO_TYPE_UNDEFINED,	.szIso639LangCode =	{ 0, }	}
		},
		.bCaDesFound			= FALSE,
		.ucChannelNum			= 1,
		.eSuppAudioEditorial	= eAUDIO_TYPE_SUPP_MAIN,
		.ucMixType				= 0
	},
	[12] = {
		.usPid					= 0x100D,
		.ucComponentTag			= 0,
		.eAudCodec				= eDxAUDIO_CODEC_DOLBY_AC3,
		.astIsoLangInfo			= {
			[0] = {  .eAudType	= eAUDIO_TYPE_VISUAL_IMPAIRED,	.szIso639LangCode =	{ 'n', 'o', 'r' }	},
			[1] = {  .eAudType	= eAUDIO_TYPE_UNDEFINED,	.szIso639LangCode =	{ 0, }	},
			[2] = {  .eAudType	= eAUDIO_TYPE_UNDEFINED,	.szIso639LangCode =	{ 0, }	},
			[3] = {  .eAudType	= eAUDIO_TYPE_UNDEFINED,	.szIso639LangCode =	{ 0, }	}
		},
		.bCaDesFound			= FALSE,
		.ucChannelNum			= 1,
		.eSuppAudioEditorial	= eAUDIO_TYPE_SUPP_AD_VISUAL_IMPARED,
		.ucMixType				= 1
	},
	[13] = {
		.usPid					= 0x100E,
		.ucComponentTag			= 0,
		.eAudCodec				= eDxAUDIO_CODEC_DOLBY_AC3,
		.astIsoLangInfo			= {
			[0] = {  .eAudType	= eAUDIO_TYPE_UNDEFINED,	.szIso639LangCode =	{ 'n', 'o', 'r' }	},
			[1] = {  .eAudType	= eAUDIO_TYPE_UNDEFINED,	.szIso639LangCode =	{ 0, }	},
			[2] = {  .eAudType	= eAUDIO_TYPE_UNDEFINED,	.szIso639LangCode =	{ 0, }	},
			[3] = {  .eAudType	= eAUDIO_TYPE_UNDEFINED,	.szIso639LangCode =	{ 0, }	}
		},
		.bCaDesFound			= FALSE,
		.ucChannelNum			= 1,
		.eSuppAudioEditorial	= eAUDIO_TYPE_SUPP_MAIN,
		.ucMixType				= 0
	},
	[14] = {
		.usPid					= 0x100F,
		.ucComponentTag			= 0,
		.eAudCodec				= eDxAUDIO_CODEC_MPEG,
		.astIsoLangInfo			= {
			[0] = {  .eAudType	= eAUDIO_TYPE_VISUAL_IMPAIRED,	.szIso639LangCode =	{ 'n', 'o', 'r' }	},
			[1] = {  .eAudType	= eAUDIO_TYPE_UNDEFINED,	.szIso639LangCode =	{ 0, }	},
			[2] = {  .eAudType	= eAUDIO_TYPE_UNDEFINED,	.szIso639LangCode =	{ 0, }	},
			[3] = {  .eAudType	= eAUDIO_TYPE_UNDEFINED,	.szIso639LangCode =	{ 0, }	}
		},
		.bCaDesFound			= FALSE,
		.ucChannelNum			= 1,
		.eSuppAudioEditorial	= eAUDIO_TYPE_SUPP_AD_VISUAL_IMPARED,
		.ucMixType				= 1,
	},
	[15] = {
		.usPid					= 0x1010,
		.ucComponentTag			= 0,
		.eAudCodec				= eDxAUDIO_CODEC_MPEG,
		.astIsoLangInfo			= {
			[0] = {  .eAudType	= eAUDIO_TYPE_UNDEFINED,	.szIso639LangCode =	{ 'n', 'o', 'r' }	},
			[1] = {  .eAudType	= eAUDIO_TYPE_UNDEFINED,	.szIso639LangCode =	{ 0, }	},
			[2] = {  .eAudType	= eAUDIO_TYPE_UNDEFINED,	.szIso639LangCode =	{ 0, }	},
			[3] = {  .eAudType	= eAUDIO_TYPE_UNDEFINED,	.szIso639LangCode =	{ 0, }	}
		},
		.bCaDesFound			= FALSE,
		.ucChannelNum			= 1,
		.eSuppAudioEditorial	= eAUDIO_TYPE_SUPP_MAIN,
		.ucMixType				= 0
	},
	[16] = {
		.usPid					= 0x1010,
		.ucComponentTag 		= 0,
		.eAudCodec				= eDxAUDIO_CODEC_MPEG,
		.astIsoLangInfo 		= {
			[0] = {  .eAudType	= eAUDIO_TYPE_UNDEFINED,	.szIso639LangCode = { 'e', 'n', 'g' }	},
			[1] = {  .eAudType	= eAUDIO_TYPE_UNDEFINED,	.szIso639LangCode = { 's', 'w', 'e' }	},
			[2] = {  .eAudType	= eAUDIO_TYPE_UNDEFINED,	.szIso639LangCode = { 0, }	},
			[3] = {  .eAudType	= eAUDIO_TYPE_UNDEFINED,	.szIso639LangCode = { 0, }	}
		},
		.bCaDesFound			= FALSE,
		.ucChannelNum			= 1,
		.eSuppAudioEditorial	= eAUDIO_TYPE_SUPP_MAIN,
		.ucMixType				= 0
	},
	[17] = {
		.usPid					= 0x1011,
		.ucComponentTag 		= 0,
		.eAudCodec				= eDxAUDIO_CODEC_MPEG,
		.astIsoLangInfo 		= {
			[0] = {  .eAudType	= eAUDIO_TYPE_UNDEFINED,	.szIso639LangCode = { 'd', 'a', 'n' }	},
			[1] = {  .eAudType	= eAUDIO_TYPE_UNDEFINED,	.szIso639LangCode = { 'n', 'o', 'r' }	},
			[2] = {  .eAudType	= eAUDIO_TYPE_UNDEFINED,	.szIso639LangCode = { 0, }	},
			[3] = {  .eAudType	= eAUDIO_TYPE_UNDEFINED,	.szIso639LangCode = { 0, }	}
		},
		.bCaDesFound			= FALSE,
		.ucChannelNum			= 1,
		.eSuppAudioEditorial	= eAUDIO_TYPE_SUPP_MAIN,
		.ucMixType				= 0
	},
	[18] = {
		.usPid					= 0x1012,
		.ucComponentTag 		= 0,
		.eAudCodec				= eDxAUDIO_CODEC_MPEG,
		.astIsoLangInfo 		= {
			[0] = {  .eAudType	= eAUDIO_TYPE_UNDEFINED,	.szIso639LangCode = { 'f', 'i', 'n' }	},
			[1] = {  .eAudType	= eAUDIO_TYPE_UNDEFINED,	.szIso639LangCode = { 'r', 'u', 's' }	},
			[2] = {  .eAudType	= eAUDIO_TYPE_UNDEFINED,	.szIso639LangCode = { 0, }	},
			[3] = {  .eAudType	= eAUDIO_TYPE_UNDEFINED,	.szIso639LangCode = { 0, }	}
		},
		.bCaDesFound			= FALSE,
		.ucChannelNum			= 1,
		.eSuppAudioEditorial	= eAUDIO_TYPE_SUPP_MAIN,
		.ucMixType				= 0
	},
	[19] = {
		.usPid					= 0x1013,
		.ucComponentTag 		= 0,
		.eAudCodec				= eDxAUDIO_CODEC_MPEG,
		.astIsoLangInfo 		= {
			[0] = {  .eAudType	= eAUDIO_TYPE_UNDEFINED,	.szIso639LangCode = { 'f', 'r', 'a' }	},
			[1] = {  .eAudType	= eAUDIO_TYPE_UNDEFINED,	.szIso639LangCode = { 'g', 'e', 'r' }	},
			[2] = {  .eAudType	= eAUDIO_TYPE_UNDEFINED,	.szIso639LangCode = { 0, }	},
			[3] = {  .eAudType	= eAUDIO_TYPE_UNDEFINED,	.szIso639LangCode = { 0, }	}
		},
		.bCaDesFound			= FALSE,
		.ucChannelNum			= 1,
		.eSuppAudioEditorial	= eAUDIO_TYPE_SUPP_MAIN,
		.ucMixType				= 0
	},
	[20] = {
		.usPid		= 0x1014,
		.ucComponentTag 	= 0,
		.eAudCodec		= eDxAUDIO_CODEC_MPEG,
		.astIsoLangInfo 	= {
			[0] = {  .eAudType	= eAUDIO_TYPE_UNDEFINED,	.szIso639LangCode = { 's', 'w', 'e' }	},
			[1] = {  .eAudType	= eAUDIO_TYPE_UNDEFINED,	.szIso639LangCode = { 'e', 'n', 'g' }	},
			[2] = {  .eAudType	= eAUDIO_TYPE_UNDEFINED,	.szIso639LangCode = { 0, }	},
			[3] = {  .eAudType	= eAUDIO_TYPE_UNDEFINED,	.szIso639LangCode = { 0, }	}
		},
		.bCaDesFound			= FALSE,
		.ucChannelNum			= 1,
		.eSuppAudioEditorial	= eAUDIO_TYPE_SUPP_MAIN,
		.ucMixType				= 0
	},
	[21] = {
		.usPid					= 0x1015,
		.ucComponentTag 		= 0,
		.eAudCodec				= eDxAUDIO_CODEC_MPEG,
		.astIsoLangInfo 		= {
			[0] = {  .eAudType	= eAUDIO_TYPE_UNDEFINED,	.szIso639LangCode = { 'n', 'o', 'r' }	},
			[1] = {  .eAudType	= eAUDIO_TYPE_UNDEFINED,	.szIso639LangCode = { 'd', 'a', 'n' }	},
			[2] = {  .eAudType	= eAUDIO_TYPE_UNDEFINED,	.szIso639LangCode = { 0, }	},
			[3] = {  .eAudType	= eAUDIO_TYPE_UNDEFINED,	.szIso639LangCode = { 0, }	}
		},
		.bCaDesFound			= FALSE,
		.ucChannelNum			= 1,
		.eSuppAudioEditorial	= eAUDIO_TYPE_SUPP_MAIN,
		.ucMixType				= 0
	},
	[22] = {
		.usPid					= 0x1016,
		.ucComponentTag 		= 0,
		.eAudCodec				= eDxAUDIO_CODEC_MPEG,
		.astIsoLangInfo 		= {
			[0] = {  .eAudType	= eAUDIO_TYPE_UNDEFINED,	.szIso639LangCode = { 'r', 'u', 's' }	},
			[1] = {  .eAudType	= eAUDIO_TYPE_UNDEFINED,	.szIso639LangCode = { 'f', 'i', 'n' }	},
			[2] = {  .eAudType	= eAUDIO_TYPE_UNDEFINED,	.szIso639LangCode = { 0, }	},
			[3] = {  .eAudType	= eAUDIO_TYPE_UNDEFINED,	.szIso639LangCode = { 0, }	}
		},
		.bCaDesFound			= FALSE,
		.ucChannelNum			= 1,
		.eSuppAudioEditorial	= eAUDIO_TYPE_SUPP_MAIN,
		.ucMixType				= 0
	},
	[23] = {
		.usPid					= 0x1017,
		.ucComponentTag 		= 0,
		.eAudCodec				= eDxAUDIO_CODEC_MPEG,
		.astIsoLangInfo	 		= {
			[0] = {  .eAudType	= eAUDIO_TYPE_UNDEFINED,	.szIso639LangCode = { 'g', 'e', 'r' }	},
			[1] = {  .eAudType	= eAUDIO_TYPE_UNDEFINED,	.szIso639LangCode = { 'f', 'r', 'a' }	},
			[2] = {  .eAudType	= eAUDIO_TYPE_UNDEFINED,	.szIso639LangCode = { 0, }	},
			[3] = {  .eAudType	= eAUDIO_TYPE_UNDEFINED,	.szIso639LangCode = { 0, }	}
		},
		.bCaDesFound			= FALSE,
		.ucChannelNum			= 1,
		.eSuppAudioEditorial	= eAUDIO_TYPE_SUPP_MAIN,
		.ucMixType				= 0
	},
	[24] = {
		.usPid					= 0x1018,
		.ucComponentTag 		= 0,
		.eAudCodec				= eDxAUDIO_CODEC_MPEG,
		.astIsoLangInfo 		= {
			[0] = {  .eAudType	= eAUDIO_TYPE_VISUAL_IMPAIRED,	.szIso639LangCode = { 0, }	},
			[1] = {  .eAudType	= eAUDIO_TYPE_UNDEFINED,	.szIso639LangCode = { 0, }	},
			[2] = {  .eAudType	= eAUDIO_TYPE_UNDEFINED,	.szIso639LangCode = { 0, }	},
			[3] = {  .eAudType	= eAUDIO_TYPE_UNDEFINED,	.szIso639LangCode = { 0, }	}
		},
		.bCaDesFound			= FALSE,
		.ucChannelNum			= 1,
		.eSuppAudioEditorial	= eAUDIO_TYPE_SUPP_AD_VISUAL_IMPARED,
		.ucMixType				= 1
	},
	[25] = {
		.usPid					= 0x1019,
		.ucComponentTag 		= 0,
		.eAudCodec				= eDxAUDIO_CODEC_DOLBY_AC3,
		.astIsoLangInfo 		= {
			[0] = {  .eAudType	= eAUDIO_TYPE_UNDEFINED,	.szIso639LangCode = { 0, }	},
			[1] = {  .eAudType	= eAUDIO_TYPE_UNDEFINED,	.szIso639LangCode = { 0, }	},
			[2] = {  .eAudType	= eAUDIO_TYPE_UNDEFINED,	.szIso639LangCode = { 0, }	},
			[3] = {  .eAudType	= eAUDIO_TYPE_UNDEFINED,	.szIso639LangCode = { 0, }	}
		},
		.bCaDesFound			= FALSE,
		.ucChannelNum			= 1,
		.eSuppAudioEditorial	= eAUDIO_TYPE_SUPP_MAIN,
		.ucMixType				= 0
	},
	[26] = {
		.usPid					= 0x101A,
		.ucComponentTag 		= 0,
		.eAudCodec				= eDxAUDIO_CODEC_DOLBY_AC3,
		.astIsoLangInfo 		= {
			[0] = {  .eAudType	= eAUDIO_TYPE_VISUAL_IMPAIRED,	.szIso639LangCode = { 0, }	},
			[1] = {  .eAudType	= eAUDIO_TYPE_UNDEFINED,	.szIso639LangCode = { 0, }	},
			[2] = {  .eAudType	= eAUDIO_TYPE_UNDEFINED,	.szIso639LangCode = { 0, }	},
			[3] = {  .eAudType	= eAUDIO_TYPE_UNDEFINED,	.szIso639LangCode = { 0, }	}
		},
		.bCaDesFound			= FALSE,
		.ucChannelNum			= 1,
		.eSuppAudioEditorial	= eAUDIO_TYPE_SUPP_AD_VISUAL_IMPARED,
		.ucMixType				= 1,
	},
	[27] = {
		.usPid					= 0x101B,
		.ucComponentTag 		= 0,
		.eAudCodec				= eDxAUDIO_CODEC_MPEG,
		.astIsoLangInfo 		= {
			[0] = {  .eAudType	= eAUDIO_TYPE_UNDEFINED,	.szIso639LangCode = { 'f', 'i', 'n' }	},
			[1] = {  .eAudType	= eAUDIO_TYPE_UNDEFINED,	.szIso639LangCode = { 0, }	},
			[2] = {  .eAudType	= eAUDIO_TYPE_UNDEFINED,	.szIso639LangCode = { 0, }	},
			[3] = {  .eAudType	= eAUDIO_TYPE_UNDEFINED,	.szIso639LangCode = { 0, }	}
		},
		.bCaDesFound			= FALSE,
		.ucChannelNum			= 1,
		.eSuppAudioEditorial	= eAUDIO_TYPE_SUPP_MAIN,
		.ucMixType				= 0
	},
	[28] = {
		.usPid					= 0x101C,
		.ucComponentTag 		= 0,
		.eAudCodec				= eDxAUDIO_CODEC_MPEG,
		.astIsoLangInfo 		= {
			[0] = {  .eAudType	= eAUDIO_TYPE_VISUAL_IMPAIRED,	.szIso639LangCode = { 'f', 'i', 'n' }	},
			[1] = {  .eAudType	= eAUDIO_TYPE_UNDEFINED,	.szIso639LangCode = { 0, }	},
			[2] = {  .eAudType	= eAUDIO_TYPE_UNDEFINED,	.szIso639LangCode = { 0, }	},
			[3] = {  .eAudType	= eAUDIO_TYPE_UNDEFINED,	.szIso639LangCode = { 0, }	}
		},
		.bCaDesFound			= FALSE,
		.ucChannelNum			= 1,
		.eSuppAudioEditorial	= eAUDIO_TYPE_SUPP_AD_VISUAL_IMPARED,
		.ucMixType				= 1
	},
	[29] = {
		.usPid					= 0x101D,
		.ucComponentTag 		= 0,
		.eAudCodec				= eDxAUDIO_CODEC_DOLBY_AC3,
		.astIsoLangInfo 		= {
			[0] = {  .eAudType	= eAUDIO_TYPE_UNDEFINED,	.szIso639LangCode = { 'f', 'i', 'n' }	},
			[1] = {  .eAudType	= eAUDIO_TYPE_UNDEFINED,	.szIso639LangCode = { 0, }	},
			[2] = {  .eAudType	= eAUDIO_TYPE_UNDEFINED,	.szIso639LangCode = { 0, }	},
			[3] = {  .eAudType	= eAUDIO_TYPE_UNDEFINED,	.szIso639LangCode = { 0, }	}
		},
		.bCaDesFound			= FALSE,
		.ucChannelNum			= 1,
		.eSuppAudioEditorial	= eAUDIO_TYPE_SUPP_MAIN,
		.ucMixType				= 0
	},
	[30] = {
		.usPid					= 0x101E,
		.ucComponentTag 		= 0,
		.eAudCodec				= eDxAUDIO_CODEC_DOLBY_AC3,
		.astIsoLangInfo 		= {
			[0] = {  .eAudType	= eAUDIO_TYPE_VISUAL_IMPAIRED,	.szIso639LangCode = { 'f', 'i', 'n' }	},
			[1] = {  .eAudType	= eAUDIO_TYPE_UNDEFINED,	.szIso639LangCode = { 0, }	},
			[2] = {  .eAudType	= eAUDIO_TYPE_UNDEFINED,	.szIso639LangCode = { 0, }	},
			[3] = {  .eAudType	= eAUDIO_TYPE_UNDEFINED,	.szIso639LangCode = { 0, }	}
		},
		.bCaDesFound			= FALSE,
		.ucChannelNum			= 1,
		.eSuppAudioEditorial	= eAUDIO_TYPE_SUPP_AD_VISUAL_IMPARED,
		.ucMixType				= 1
	}
};

#define ___LOCAL_FUNCTIONS___

/**********************************************
	Command 수행 함수를 등록하는 통합 등록 함수.
***********************************************/
STATIC HUINT32 svc_si_UtAudioEsGetEsTableItemNum(void)
{
	return sizeof(s_astAudioEsList) / sizeof(svcSi_AudioEs_t);
}

STATIC svcSi_AudioEs_t* svc_si_UtAudioEsGetEsTableItem(HUINT32 ulIdx)
{
	if (svc_si_UtAudioEsGetEsTableItemNum() <= ulIdx)
		return NULL;

	return &s_astAudioEsList[ulIdx];
}

STATIC void svc_si_UtAudioEsPrintSimple(svcSi_AudioEs_t *pstAudioEs, HUINT32 ulIndex)
{
#if defined(CONFIG_DEBUG)
	HxLOG_Print("  [%02d] : Pid(0x%04x), Codec(%d), Lang[0](%d,[%3s]) Lang[1](%d,[%3s])\n", ulIndex,
				pstAudioEs->usPid, pstAudioEs->eAudCodec,
				pstAudioEs->astIsoLangInfo[0].eAudType,	pstAudioEs->astIsoLangInfo[0].szIso639LangCode,
				pstAudioEs->astIsoLangInfo[1].eAudType,	pstAudioEs->astIsoLangInfo[1].szIso639LangCode);
#endif
}

STATIC HBOOL svc_si_UtAudioEsPrintInfo(void *pvData, void *pvUserData)
{
	svcSi_AudioEs_t 	*pstAudioEs = (svcSi_AudioEs_t *)pvData;
	HUINT32				*pulNum = (HUINT32 *)pvUserData;

	if (NULL == pstAudioEs)
		return FALSE;

	if (NULL == pulNum)
		return FALSE;

	svc_si_UtAudioEsPrintSimple(pstAudioEs, (*pulNum)++);

	return FALSE;
}

STATIC void svc_si_UtAudioEsPrintList(HxList_t *pstEsList)
{
#if defined(CONFIG_DEBUG) //&& 0
	if (pstEsList)
	{
		HUINT32 ulCount;

		HxLOG_Print("=================================================================\n");
		HxLOG_Print("== ES List (%02d) =================================================\n", HLIB_LIST_Length(pstEsList));
		HxLOG_Print("=================================================================\n");

		ulCount = 0;
		HLIB_LIST_ForeachEx(pstEsList, svc_si_UtAudioEsPrintInfo, (void *)&ulCount);

		HxLOG_Print("=================================================================\n");
	}
#endif
}


STATIC HERROR svc_si_UtAudioEsSetPriorityTable(void)
{
	STATIC SvcSi_EsTypePriority_t s_astPriorityTable_Ut[] =
	{
		{ eSvcSi_AUD_PRIORITY_LANG_BASE,							eSvcSi_AUD_SEL_TYPE_NUM,							1000	},
		{ eSvcSi_AUD_PRIORITY_LANG_USER,							eSvcSi_AUD_SEL_LANG_USER,							8000	},
		{ eSvcSi_AUD_PRIORITY_LANG_USER_WITH_PID,					eSvcSi_AUD_SEL_LANG_USER_WITH_PID,					9000	},
		{ eSvcSi_AUD_PRIORITY_LANG_0,								eSvcSi_AUD_SEL_LANG_0,								7000	},
		{ eSvcSi_AUD_PRIORITY_LANG_1,								eSvcSi_AUD_SEL_LANG_1,								6000	},
		{ eSvcSi_AUD_PRIORITY_LANG_OSD,								eSvcSi_AUD_SEL_LANG_OSD,							5000	},
		{ eSvcSi_AUD_PRIORITY_LANG_UNDEFINED,						eSvcSi_AUD_SEL_LANG_UNDEFINED,						4000	},
		{ eSvcSi_AUD_PRIORITY_LANG_ORIGINAL,						eSvcSi_AUD_SEL_LANG_ORIGINAL,						3000	},
		{ eSvcSi_AUD_PRIORITY_LANG_DEFAULT,							eSvcSi_AUD_SEL_LANG_DEFAULT, 						2000	},
		{ eSvcSi_AUD_PRIORITY_DESCRIPTION_BROADCASTER_MIX_USER, 	eSvcSi_AUD_SEL_DESCRIPTION_BROADCASTER_MIX_USER,	70000	},
		{ eSvcSi_AUD_PRIORITY_DESCRIPTION_BROADCASTER_MIX_MENU, 	eSvcSi_AUD_SEL_DESCRIPTION_BROADCASTER_MIX_MENU,	60000	},
		{ eSvcSi_AUD_PRIORITY_DESCRIPTION_OFF_BROADCASTER_MIX_ON,	eSvcSi_AUD_SEL_DESCRIPTION_OFF_BROADCASTER_MIX_ON,	20000	},
		{ eSvcSi_AUD_PRIORITY_DESCRIPTION_BROADCASTER_MIX_BASE, 	eSvcSi_AUD_SEL_TYPE_NUM,							10000	},
		{ eSvcSi_AUD_PRIORITY_DESCRIPTION_RECEIVER_MIX_USER,		eSvcSi_AUD_SEL_DESCRIPTION_RECEIVER_MIX_USER,		50000	},
		{ eSvcSi_AUD_PRIORITY_DESCRIPTION_RECEIVER_MIX_MENU,		eSvcSi_AUD_SEL_DESCRIPTION_RECEIVER_MIX_MENU,		40000	},
		{ eSvcSi_AUD_PRIORITY_DESCRIPTION_OFF_RECEIVER_MIX_ON,		eSvcSi_AUD_SEL_DESCRIPTION_OFF_RECEIVER_MIX_ON,		20000	},
		{ eSvcSi_AUD_PRIORITY_DESCRIPTION_RECEIVER_MIX_BASE,		eSvcSi_AUD_SEL_TYPE_NUM,							10000	},
		{ eSvcSi_AUD_PRIORITY_DESCRIPTION_OFF_BROADCASTER_MIX_OFF,	eSvcSi_AUD_SEL_DESCRIPTION_OFF_BROADCASTER_MIX_OFF,	30000	},
		{ eSvcSi_AUD_PRIORITY_DESCRIPTION_OFF_RECEIVER_MIX_OFF,		eSvcSi_AUD_SEL_DESCRIPTION_OFF_RECEIVER_MIX_OFF,	30000	},
		{ eSvcSi_AUD_PRIORITY_DESCRIPTION_ON_BROADCASTER_MIX_OFF,	eSvcSi_AUD_SEL_DESCRIPTION_ON_BROADCASTER_MIX_OFF,	30000	},
		{ eSvcSi_AUD_PRIORITY_DESCRIPTION_ON_RECEIVER_MIX_OFF,		eSvcSi_AUD_SEL_DESCRIPTION_ON_RECEIVER_MIX_OFF,		30000	},
		{ eSvcSi_AUD_PRIORITY_MULTICHANNEL_BASE,					eSvcSi_AUD_SEL_TYPE_NUM,							0		},
		{ eSvcSi_AUD_PRIORITY_MULTICHANNEL_USER,					eSvcSi_AUD_SEL_MULTICHANNEL_USER,					200 	},
		{ eSvcSi_AUD_PRIORITY_MULTICHANNEL_MENU,					eSvcSi_AUD_SEL_MULTICHANNEL_MENU,					100 	},
		{ eSvcSi_AUD_PRIORITY_NORMAL_AUDIO,					eSvcSi_AUD_SEL_TYPE_NUM,							2		},
		{ eSvcSi_AUD_PRIORITY_FIRST_TRACK,							eSvcSi_AUD_SEL_TYPE_NUM, 							1		},
		{ eSvcSi_AUD_PRIORITY_DUAL_MONO,							eSvcSi_AUD_SEL_TYPE_NUM,							-10 	},
		{ eSvcSi_AUD_PRIORITY_TYPE_NUM, 							eSvcSi_AUD_SEL_TYPE_NUM,							-1		}
	};
	HUINT32	 ulNum;

	ulNum = sizeof(s_astPriorityTable_Ut) / sizeof(SvcSi_EsTypePriority_t);

	return SVC_SI_SetAudioPriorityTable (s_astPriorityTable_Ut, ulNum);
}

STATIC void svc_si_UtAudioEsSetScenario(Cutest_TestCase_t *pstTc)
{
	HERROR	hErr;
	HUINT8	szPrintBuf[svcSi_CMD_STR_BUFF_SIZE] = {0, };

	hErr = svc_si_UtAudioEsSetPriorityTable();
	if(hErr != ERR_OK)
	{
		HxSTD_snprintf((char *)&szPrintBuf, svcSi_CMD_STR_BUFF_SIZE, (const char *)"Fail to Get Audio Select Priority Table for this Test : err[0x%x]", hErr);
		CutestFail(pstTc, (const char *)&szPrintBuf);
	}
}

const HxList_t *svc_si_UtAudioEsGetInfo(HUINT32 ulEsNum, ...)
{
	va_list				 ap;
	HUINT32 			 aulTrackList[32] = { 0, };
	HxList_t			*pstAudioList = NULL;
	svcSi_AudioEs_t		*pstAudioEs, *pstAudioEsSource;
	HUINT32				 ulEsTableItemNum;
	HUINT32 			 ulCount, ulTrack;
	HUINT32 			 ulItemNum;

    va_start(ap, ulEsNum);
	ulItemNum = svc_si_UtSetTrackIdx(aulTrackList, ulEsNum, svc_si_UtAudioEsGetEsTableItemNum()-1, ap);
	va_end(ap);

	if (32 <= ulItemNum)
	{
		HxLOG_Warning("32 <= ulItemNum\n");
		return NULL;
	}

	if (ulEsNum < ulItemNum)
		ulItemNum = ulEsNum;

	if (ulItemNum > 0)
	{
		ulEsTableItemNum = svc_si_UtAudioEsGetEsTableItemNum();
		for (ulCount=0; ulCount<ulItemNum; ulCount++)
		{
			ulTrack = aulTrackList[ulCount];
			if (ulTrack >= ulEsTableItemNum)	continue;

			pstAudioEsSource = svc_si_UtAudioEsGetEsTableItem(ulTrack);
			if (pstAudioEsSource)
			{
				pstAudioEs = OxSI_MemDup(pstAudioEsSource, sizeof(svcSi_AudioEs_t));
				if (pstAudioEs)
				{
					pstAudioList = HLIB_LIST_Append(pstAudioList, (void *)pstAudioEs);
				}
			}
		}

		svc_si_UtAudioEsPrintList(pstAudioList);
	}

	return (const HxList_t *)pstAudioList;
}

STATIC const svcSi_PmtEsInfo_t *svc_si_UtAudioEsSetList(svcSi_PmtEsInfo_t *pstPmtEsInfo, const HxList_t *pstAudioList)
{
	return svc_si_UtSetEsListToPmtEsInfo(pstPmtEsInfo, eEsType_Audio, pstAudioList);
}

STATIC void svc_si_UtAudioEsVeritySelectOutput(Cutest_TestCase_t *pstTc, HERROR hErr, SvcSi_SelectedEs_t *pstSelectedEs, HINT32 nExpectedTrackIdx, DxDualMonoSelect_e eDualMonoSelect)
{
	svc_si_UtVeritySelectAudioOutput(pstTc, hErr,
									pstSelectedEs, nExpectedTrackIdx,
									"svc_si_LibSvcEsSelectAudio() fail",
									(HUINT32)eDualMonoSelect, 0, 0);
}

STATIC void	svc_si_UtAudioEsVeritySelectResult(Cutest_TestCase_t	*pstTc,
												const HCHAR			*pszFuncName,
												const HCHAR			*pszResultType,
												HINT32				 nActualResult,
												HINT32				 nExpectedResult)
{
	svc_si_UtVeritySelectEsResult(pstTc, pszFuncName, pszResultType,
									nActualResult, nExpectedResult);
}

STATIC const SvcSi_Config_t *svc_si_UtAudioEsGetConfig(const HUINT8		 szAudLang0[],
														const HUINT8	 szAudLang1[],
														const HUINT8	 szOsdLang[],
														HBOOL			 bMultiMenu,
														HBOOL			 bAdMenu,
														const HUINT8	 szAudLangUser[],
														HBOOL			 bMultiUser,
														HBOOL			 bAdUser)
{
	HERROR			 hErr;
	SvcSi_Config_t	*pstConfig;

	hErr = svc_si_UtAudioEsSetPriorityTable();
	if(hErr != ERR_OK)
	{
		HxLOG_Warning("svc_si_UtAudioEsSetPriorityTable() Error!\n");
		return NULL;
	}

	pstConfig = OxSI_Malloc(sizeof(SvcSi_Config_t));
	if (pstConfig)
	{
		pstConfig->stSystem.eOperator = eDxOPERATOR_None;
		pstConfig->stSystem.eCountry = eCountryID_GBR;
		pstConfig->stSystem.eOsdLang = svcSi_UtGetLangId(szOsdLang);

		pstConfig->stAudioLanguage.aeLangId[eSvcSi_CFG_AUD_LANG_User] = svcSi_UtGetLangId(szAudLangUser);
		pstConfig->stAudioLanguage.aeLangId[eSvcSi_CFG_AUD_LANG_User_With_Pid] = svcSi_UtGetLangId(szAudLangUser);
		pstConfig->stAudioLanguage.aeLangId[eSvcSi_CFG_AUD_LANG_0] = svcSi_UtGetLangId(szAudLang0);
		pstConfig->stAudioLanguage.aeLangId[eSvcSi_CFG_AUD_LANG_1] = svcSi_UtGetLangId(szAudLang1);

		pstConfig->stAudioOutput.eStereoSelect = eDxSTEREO_SELECT_STEREO;
		pstConfig->stAudioOutput.abOutputFlag[eSvcSi_CFG_AUD_OUTPUT_Multichannel_User] = bMultiUser;
		pstConfig->stAudioOutput.abOutputFlag[eSvcSi_CFG_AUD_OUTPUT_Multichannel_Menu] = bMultiMenu;

		pstConfig->stAudioDesc.abAdFlag[eSvcSi_CFG_AUD_DESCRIPTION_User] = bAdUser;
		pstConfig->stAudioDesc.abAdFlag[eSvcSi_CFG_AUD_DESCRIPTION_Menu] = bAdMenu;
	}

	svc_si_UtPrintConfig((const SvcSi_Config_t *)pstConfig, TRUE, FALSE);

	return (const SvcSi_Config_t *)pstConfig;
}

STATIC HERROR svc_si_UtAudioEsCheckParam(Cutest_TestCase_t *pstTc, const svcSi_PmtEsInfo_t *pstPmtEsInfo, const SvcSi_Config_t *pstConfig, SvcSi_SelectedEs_t *pstSelectedAudio)
{
	HUINT8	szPrintBuf[svcSi_CMD_STR_BUFF_SIZE] = {0, };

	if (NULL == pstPmtEsInfo)
	{
		HxSTD_snprintf((char *)&szPrintBuf, svcSi_CMD_STR_BUFF_SIZE, (const char *)"SOURCE NULL : (NULL == pstPmtEsInfo)");
		CutestFail(pstTc, (const char *)&szPrintBuf);
		return ERR_FAIL;
	}

	if (NULL == pstConfig)
	{
		HxSTD_snprintf((char *)&szPrintBuf, svcSi_CMD_STR_BUFF_SIZE, (const char *)"SOURCE NULL : (NULL == pstConfig)");
		CutestFail(pstTc, (const char *)&szPrintBuf);
		return ERR_FAIL;
	}

	if (NULL == pstSelectedAudio)
	{
		HxSTD_snprintf((char *)&szPrintBuf, svcSi_CMD_STR_BUFF_SIZE, (const char *)"TARGET NULL : (NULL == pstSelectedAudio)");
		CutestFail(pstTc, (const char *)&szPrintBuf);
		return ERR_FAIL;
	}

	return ERR_OK;
}

STATIC HERROR svc_si_UtAudioEsSelect (Cutest_TestCase_t *pstTc, HBOOL bCheckParam, const svcSi_PmtEsInfo_t *pstPmtEsInfo, const SvcSi_Config_t *pstConfig, SvcSi_SelectedEs_t *pstSelectedAudio)
{
	HERROR	hErr = ERR_OK;

	if (bCheckParam)
	{
		hErr = svc_si_UtAudioEsCheckParam (pstTc, pstPmtEsInfo, pstConfig, pstSelectedAudio);
	}

	if (ERR_OK != hErr)
	{
		return hErr;
	}

	return svc_si_LibSvcEsSelectAudio(pstPmtEsInfo, pstConfig, pstSelectedAudio);
}

/*	SUT 			: svc_si_UtAudioEsSelect
 *	Expected Result : ERR_FAIL
 *	Type			: SOURCE NULL (pstPmtEsInfo)
 */
void svc_si_TcAudioSelectSourceNull1(Cutest_TestCase_t *pstTc)
{
	HERROR					 hErr;
	SvcSi_SelectedEs_t		 stSelectedAudio;
	const svcSi_PmtEsInfo_t	*pstPmtEsInfo;
	const SvcSi_Config_t	*pstConfig;

	// 1. Setup Fixture
	svc_si_UtAudioEsSetScenario (pstTc);		//	Set Up Test Stub
	pstPmtEsInfo	= NULL;
	pstConfig		= svc_si_UtAudioEsGetConfig (	"rus",	"dan",	"fra",	// szAudLang0, szAudLang1, szOsdLang
													FALSE, 	FALSE,			// bMultiMenu, bAdMenu,
													"",						// szAudLangUser
													FALSE, 	FALSE);			// bMultiUser, bAdUser
	svc_si_UtInitSelectedEs (&stSelectedAudio);

	// 2. Exercise
	hErr = svc_si_UtAudioEsSelect (pstTc, FALSE, pstPmtEsInfo, pstConfig, &stSelectedAudio);

	// 3. Verify
	svc_si_UtAudioEsVeritySelectResult (pstTc, __FUNCTION__, HxNAMEOF(HERROR), hErr, ERR_FAIL);

	// 4. Teardown
	svc_si_UtFreeConfig ((SvcSi_Config_t *)pstConfig);
	svc_si_UtFreeEsInfo ((svcSi_PmtEsInfo_t *)pstPmtEsInfo);

	return;
}

/*	SUT 			: svc_si_UtAudioEsSelect
 *	Expected Result : ERR_FAIL
 *	Type			: SOURCE NULL (pstConfig)
 */
void svc_si_TcAudioSelectSourceNull2(Cutest_TestCase_t *pstTc)
{
	HERROR					 hErr;
	SvcSi_SelectedEs_t		 stSelectedAudio;
	const HxList_t			*pstAudioList;
	const svcSi_PmtEsInfo_t	*pstPmtEsInfo;
	const SvcSi_Config_t	*pstConfig;

	// 1. Setup Fixture
	svc_si_UtAudioEsSetScenario (pstTc);		//	Set Up Test Stub
	pstAudioList	= svc_si_UtAudioEsGetInfo (	4,		//	ItemNum
												3,		//	[0:"eng", 1:"", Multi:"no", AD:"no"]
												7,		//	[0:"ger", 1:"", Multi:"no", AD:"no"]
												11, 	//	[0:"swe", 1:"", Multi:"no", AD:"no"]
												15,	 	//	[0:"nor", 1:"", Multi:"no", AD:"no"]
												(HUINT32)-1);
	pstPmtEsInfo	= svc_si_UtAudioEsSetList (NULL, pstAudioList);
	pstConfig		= NULL;
	svc_si_UtInitSelectedEs (&stSelectedAudio);

	// 2. Exercise
	hErr = svc_si_UtAudioEsSelect (pstTc, FALSE, pstPmtEsInfo, pstConfig, &stSelectedAudio);

	// 3. Verify
	svc_si_UtAudioEsVeritySelectResult (pstTc, __FUNCTION__, HxNAMEOF(HERROR), hErr, ERR_FAIL);

	// 4. Teardown
	svc_si_UtFreeConfig ((SvcSi_Config_t *)pstConfig);
	svc_si_UtFreeEsInfo ((svcSi_PmtEsInfo_t *)pstPmtEsInfo);

	return;
}

/*	SUT 			: svc_si_UtAudioEsSelect
 *	Expected Result : ERR_FAIL
 *	Type			: TARGET NULL (SelectedAudio)
 */
void svc_si_TcAudioSelectTargetNull(Cutest_TestCase_t *pstTc)
{
	HERROR					 hErr;
	const HxList_t			*pstAudioList;
	const svcSi_PmtEsInfo_t	*pstPmtEsInfo;
	const SvcSi_Config_t	*pstConfig;

	// 1. Setup Fixture
	svc_si_UtAudioEsSetScenario (pstTc);		//	Set Up Test Stub
	pstAudioList	= svc_si_UtAudioEsGetInfo (	4,		//	ItemNum
												3,		//	[0:"eng", 1:"", Multi:"no", AD:"no"]
												7,		//	[0:"ger", 1:"", Multi:"no", AD:"no"]
												11, 	//	[0:"swe", 1:"", Multi:"no", AD:"no"]
												15,	 	//	[0:"nor", 1:"", Multi:"no", AD:"no"]
												(HUINT32)-1);
	pstPmtEsInfo	= svc_si_UtAudioEsSetList (NULL, pstAudioList);
	pstConfig		= svc_si_UtAudioEsGetConfig (	"rus",	"dan",	"fra",	// szAudLang0, szAudLang1, szOsdLang
													FALSE, 	FALSE,			// bMultiMenu, bAdMenu,
													"",						// szAudLangUser
													FALSE, 	FALSE);			// bMultiUser, bAdUser

	// 2. Exercise
	hErr = svc_si_UtAudioEsSelect (pstTc, FALSE, pstPmtEsInfo, pstConfig, NULL);

	// 3. Verify
	svc_si_UtAudioEsVeritySelectResult (pstTc, __FUNCTION__, HxNAMEOF(HERROR), hErr, ERR_FAIL);

	// 4. Teardown
	svc_si_UtFreeConfig ((SvcSi_Config_t *)pstConfig);
	svc_si_UtFreeEsInfo ((svcSi_PmtEsInfo_t *)pstPmtEsInfo);

	return;
}

/*	SUT 			: svc_si_UtAudioEsSelect
 *	Expected Result : ERR_FAIL
 *	Type			: SOURCE INVALID (pstPmtEsInfo)
 *	Comments		: Number of Audio ES Item in pmtEsInfo is "0"
 */
void svc_si_TcAudioSelectSourceInvalid(Cutest_TestCase_t *pstTc)
{
	HERROR					 hErr;
	SvcSi_SelectedEs_t		 stSelectedAudio;
	const HxList_t			*pstAudioList;
	const svcSi_PmtEsInfo_t	*pstPmtEsInfo;
	const SvcSi_Config_t	*pstConfig;

	// 1. Setup Fixture
	svc_si_UtAudioEsSetScenario (pstTc);		//	Set Up Test Stub
	pstAudioList	= svc_si_UtAudioEsGetInfo (	0,		//	ItemNum
											(HUINT32)-1);
	pstPmtEsInfo	= svc_si_UtAudioEsSetList (NULL, pstAudioList);
	pstConfig		= svc_si_UtAudioEsGetConfig (	"rus",	"dan",	"fra",	// szAudLang0, szAudLang1, szOsdLang
													FALSE, 	FALSE,			// bMultiMenu, bAdMenu,
													"",						// szAudLangUser
													FALSE, 	FALSE);			// bMultiUser, bAdUser
	svc_si_UtInitSelectedEs (&stSelectedAudio);

	// 2. Exercise
	hErr = svc_si_UtAudioEsSelect (pstTc, FALSE, pstPmtEsInfo, pstConfig, &stSelectedAudio);

	// 3. Verify
	svc_si_UtAudioEsVeritySelectResult (pstTc, __FUNCTION__, HxNAMEOF(HERROR), hErr, ERR_FAIL);

	// 4. Teardown
	svc_si_UtFreeConfig ((SvcSi_Config_t *)pstConfig);
	svc_si_UtFreeEsInfo ((svcSi_PmtEsInfo_t *)pstPmtEsInfo);

	return;
}

/*	SUT 			: svc_si_UtAudioEsSelect
 *	Expected Result : ERR_OK
 *	Type			: Check 1st Track Has top priority
 *	Comments		: No Language, Multichannel and AudioDescription Matched
 *					  In this case 1st Track Has Top Priority
 */
void svc_si_TcAudioSelectNoMatchFirstTrack(Cutest_TestCase_t *pstTc)
{
	HERROR					 hErr;
	SvcSi_SelectedEs_t		 stSelectedAudio;
	const HxList_t			*pstAudioList;
	const svcSi_PmtEsInfo_t	*pstPmtEsInfo;
	const SvcSi_Config_t	*pstConfig;

	// 1. Setup Fixture
	svc_si_UtAudioEsSetScenario (pstTc);		//	Set Up Test Stub
	pstAudioList	= svc_si_UtAudioEsGetInfo (	4,		//	ItemNum
												3,		//	[0:"eng", 1:"", Multi:"no", AD:"no"]
												7,		//	[0:"ger", 1:"", Multi:"no", AD:"no"]
												11, 	//	[0:"swe", 1:"", Multi:"no", AD:"no"]
												15,	 	//	[0:"nor", 1:"", Multi:"no", AD:"no"]
												(HUINT32)-1);
	pstPmtEsInfo	= svc_si_UtAudioEsSetList (NULL, pstAudioList);
	pstConfig		= svc_si_UtAudioEsGetConfig (	"rus",	"dan",	"fra",	// szAudLang0, szAudLang1, szOsdLang
													FALSE, 	FALSE,			// bMultiMenu, bAdMenu,
													"",						// szAudLangUser
													FALSE, 	FALSE);			// bMultiUser, bAdUser
	svc_si_UtInitSelectedEs (&stSelectedAudio);

	// 2. Exercise
	hErr = svc_si_UtAudioEsSelect (pstTc, TRUE, pstPmtEsInfo, pstConfig, &stSelectedAudio);

	// 3. Verify
	svc_si_UtAudioEsVeritySelectOutput(pstTc, hErr, &stSelectedAudio, 0, eDxDUALMONO_SELECT_UNKNOWN);

	// 4. Teardown
	svc_si_UtFreeConfig ((SvcSi_Config_t *)pstConfig);
	svc_si_UtFreeEsInfo ((svcSi_PmtEsInfo_t *)pstPmtEsInfo);

	return;
}

/*	SUT 			: svc_si_UtAudioEsSelect
 *	Expected Result : ERR_OK
 *	Type			: Lang Audio0
 *	Comments		: Find Lang Audio0 Matched Track
 */
void svc_si_TcAudioSelectStereoAud0(Cutest_TestCase_t *pstTc)
{
	HERROR					 hErr;
	SvcSi_SelectedEs_t		 stSelectedAudio;
	const HxList_t			*pstAudioList;
	const svcSi_PmtEsInfo_t	*pstPmtEsInfo;
	const SvcSi_Config_t	*pstConfig;

	// 1. Setup Fixture
	svc_si_UtAudioEsSetScenario (pstTc);		//	Set Up Test Stub
	pstAudioList	= svc_si_UtAudioEsGetInfo (	4,		//	ItemNum
												3,		//	[0:"eng", 1:"", Multi:"no", AD:"no"]
												7,		//	[0:"ger", 1:"", Multi:"no", AD:"no"]
												11, 	//	[0:"swe", 1:"", Multi:"no", AD:"no"]
												15,	 	//	[0:"nor", 1:"", Multi:"no", AD:"no"]
												(HUINT32)-1);
	pstPmtEsInfo	= svc_si_UtAudioEsSetList (NULL, pstAudioList);
	pstConfig		= svc_si_UtAudioEsGetConfig (	"swe",	"fin",	"eng",	// szAudLang0, szAudLang1, szOsdLang
													FALSE, 	FALSE,			// bMultiMenu, bAdMenu,
													"",						// szAudLangUser
													FALSE, 	FALSE);			// bMultiUser, bAdUser
	svc_si_UtInitSelectedEs (&stSelectedAudio);

	// 2. Exercise
	hErr = svc_si_UtAudioEsSelect (pstTc, TRUE, pstPmtEsInfo, pstConfig, &stSelectedAudio);

	// 3. Verify
	svc_si_UtAudioEsVeritySelectOutput(pstTc, hErr, &stSelectedAudio, 2, eDxDUALMONO_SELECT_UNKNOWN);

	// 4. Teardown
	svc_si_UtFreeConfig ((SvcSi_Config_t *)pstConfig);
	svc_si_UtFreeEsInfo ((svcSi_PmtEsInfo_t *)pstPmtEsInfo);

	return;
}

/*	SUT 			: svc_si_UtAudioEsSelect
 *	Expected Result : ERR_OK
 *	Type			: Lang Audio1
 *	Comments		: Find Lang Audio1 Matched Track
 */
void svc_si_TcAudioSelectStereoAud1(Cutest_TestCase_t *pstTc)
{
	HERROR					 hErr;
	SvcSi_SelectedEs_t		 stSelectedAudio;
	const HxList_t			*pstAudioList;
	const svcSi_PmtEsInfo_t	*pstPmtEsInfo;
	const SvcSi_Config_t	*pstConfig;

	// 1. Setup Fixture
	svc_si_UtAudioEsSetScenario (pstTc);		//	Set Up Test Stub
	pstAudioList	= svc_si_UtAudioEsGetInfo (	4,		//	ItemNum
												3,		//	[0:"eng", 1:"", Multi:"no", AD:"no"]
												7,		//	[0:"ger", 1:"", Multi:"no", AD:"no"]
												11, 	//	[0:"swe", 1:"", Multi:"no", AD:"no"]
												15,	 	//	[0:"nor", 1:"", Multi:"no", AD:"no"]
												(HUINT32)-1);
	pstPmtEsInfo	= svc_si_UtAudioEsSetList (NULL, pstAudioList);
	pstConfig		= svc_si_UtAudioEsGetConfig (	"rus",	"nor",	"dan",	// szAudLang0, szAudLang1, szOsdLang
													TRUE, 	FALSE,			// bMultiMenu, bAdMenu,
													"",						// szAudLangUser
													FALSE, 	FALSE);			// bMultiUser, bAdUser
	svc_si_UtInitSelectedEs (&stSelectedAudio);

	// 2. Exercise
	hErr = svc_si_UtAudioEsSelect (pstTc, TRUE, pstPmtEsInfo, pstConfig, &stSelectedAudio);

	// 3. Verify
	svc_si_UtAudioEsVeritySelectOutput(pstTc, hErr, &stSelectedAudio, 3, eDxDUALMONO_SELECT_UNKNOWN);

	// 4. Teardown
	svc_si_UtFreeConfig ((SvcSi_Config_t *)pstConfig);
	svc_si_UtFreeEsInfo ((svcSi_PmtEsInfo_t *)pstPmtEsInfo);

	return;
}

/*	SUT 			: svc_si_UtAudioEsSelect
 *	Expected Result : ERR_OK
 *	Type			: Lang Menu
 *	Comments		: Find Lang Menu Matched Track
 */
void svc_si_TcAudioSelectStereoMenu(Cutest_TestCase_t *pstTc)
{
	HERROR					 hErr;
	SvcSi_SelectedEs_t		 stSelectedAudio;
	const HxList_t			*pstAudioList;
	const svcSi_PmtEsInfo_t	*pstPmtEsInfo;
	const SvcSi_Config_t	*pstConfig;

	// 1. Setup Fixture
	svc_si_UtAudioEsSetScenario (pstTc);		//	Set Up Test Stub
	pstAudioList	= svc_si_UtAudioEsGetInfo (	4,		//	ItemNum
												3, 		//	[0:"eng", 1:"", Multi:"no", AD:"no"]
												7, 		//	[0:"ger", 1:"", Multi:"no", AD:"no"]
												11, 	//	[0:"swe", 1:"", Multi:"no", AD:"no"]
												15,	 	//	[0:"nor", 1:"", Multi:"no", AD:"no"]
												(HUINT32)-1);
	pstPmtEsInfo	= svc_si_UtAudioEsSetList (NULL, pstAudioList);
	pstConfig		= svc_si_UtAudioEsGetConfig (	"rus",	"dan",	"eng",	// szAudLang0, szAudLang1, szOsdLang
													TRUE, 	FALSE,			// bMultiMenu, bAdMenu,
													"",						// szAudLangUser
													FALSE, 	FALSE);			// bMultiUser, bAdUser
	svc_si_UtInitSelectedEs (&stSelectedAudio);

	// 2. Exercise
	hErr = svc_si_UtAudioEsSelect (pstTc, TRUE, pstPmtEsInfo, pstConfig, &stSelectedAudio);

	// 3. Verify
	svc_si_UtAudioEsVeritySelectOutput(pstTc, hErr, &stSelectedAudio, 0, eDxDUALMONO_SELECT_UNKNOWN);

	// 4. Teardown
	svc_si_UtFreeConfig ((SvcSi_Config_t *)pstConfig);
	svc_si_UtFreeEsInfo ((svcSi_PmtEsInfo_t *)pstPmtEsInfo);

	return;
}

/*	SUT 			: svc_si_UtAudioEsSelect
 *	Expected Result : ERR_OK
 *	Type			: Lang User
 *	Comments		: Find Lang User Matched Track
 */
void svc_si_TcAudioSelectStereoUser(Cutest_TestCase_t *pstTc)
{
	HERROR					 hErr;
	SvcSi_SelectedEs_t 		 stSelectedAudio;
	const HxList_t			*pstAudioList;
	const svcSi_PmtEsInfo_t	*pstPmtEsInfo;
	const SvcSi_Config_t	*pstConfig;

	// 1. Setup Fixture
	svc_si_UtAudioEsSetScenario (pstTc);		//	Set Up Test Stub
	pstAudioList	= svc_si_UtAudioEsGetInfo (	4,		//	ItemNum
												3,		//	[0:"eng", 1:"", Multi:"no", AD:"no"]
												7,		//	[0:"ger", 1:"", Multi:"no", AD:"no"]
												11, 	//	[0:"swe", 1:"", Multi:"no", AD:"no"]
												15,	 	//	[0:"nor", 1:"", Multi:"no", AD:"no"]
												(HUINT32)-1);
	pstPmtEsInfo	= svc_si_UtAudioEsSetList (NULL, pstAudioList);
	pstConfig		= svc_si_UtAudioEsGetConfig (	"ger",	"eng",	"nor",	// szAudLang0, szAudLang1, szOsdLang
													TRUE, 	FALSE,			// bMultiMenu, bAdMenu,
													"swe",					// szAudLangUser
													FALSE, 	FALSE);			// bMultiUser, bAdUser
	svc_si_UtInitSelectedEs (&stSelectedAudio);

	// 2. Exercise
	hErr = svc_si_UtAudioEsSelect (pstTc, TRUE, pstPmtEsInfo, pstConfig, &stSelectedAudio);

	// 3. Verify
	svc_si_UtAudioEsVeritySelectOutput(pstTc, hErr, &stSelectedAudio, 2, eDxDUALMONO_SELECT_UNKNOWN);

	// 4. Teardown
	svc_si_UtFreeConfig ((SvcSi_Config_t *)pstConfig);
	svc_si_UtFreeEsInfo ((svcSi_PmtEsInfo_t *)pstPmtEsInfo);

	return;
}

/*	SUT 			: svc_si_UtAudioEsSelect
 *	Expected Result : ERR_OK
 *	Type			: Multichannel On by User
 *	Comments		: Find Multichannel Track
 */
void svc_si_TcAudioSelectMultiUser1(Cutest_TestCase_t *pstTc)
{
	HERROR					 hErr;
	SvcSi_SelectedEs_t		 stSelectedAudio;
	const HxList_t			*pstAudioList;
	const svcSi_PmtEsInfo_t	*pstPmtEsInfo;
	const SvcSi_Config_t	*pstConfig;

	// 1. Setup Fixture
	svc_si_UtAudioEsSetScenario (pstTc);		//	Set Up Test Stub
	pstAudioList	= svc_si_UtAudioEsGetInfo (	4,		//	ItemNum
												3,		//	[0:"eng", 1:"", Multi:"no",  AD:"no" ]
												7,		//	[0:"ger", 1:"", Multi:"no",  AD:"no" ]
												9,		//	[0:"swe", 1:"", Multi:"yes", AD:"no" ]
												11,	 	//	[0:"swe", 1:"", Multi:"no",  AD:"no" ]
												(HUINT32)-1);
	pstPmtEsInfo	= svc_si_UtAudioEsSetList (NULL, pstAudioList);
	pstConfig		= svc_si_UtAudioEsGetConfig (	"ger",	"eng",	"nor",	// szAudLang0, szAudLang1, szOsdLang
													FALSE, 	FALSE,			// bMultiMenu, bAdMenu,
													"swe",					// szAudLangUser
													TRUE, 	FALSE);			// bMultiUser, bAdUser
	svc_si_UtInitSelectedEs (&stSelectedAudio);

	// 2. Exercise
	hErr = svc_si_UtAudioEsSelect (pstTc, TRUE, pstPmtEsInfo, pstConfig, &stSelectedAudio);

	// 3. Verify
	svc_si_UtAudioEsVeritySelectOutput(pstTc, hErr, &stSelectedAudio, 2, eDxDUALMONO_SELECT_UNKNOWN);

	// 4. Teardown
	svc_si_UtFreeConfig ((SvcSi_Config_t *)pstConfig);
	svc_si_UtFreeEsInfo ((svcSi_PmtEsInfo_t *)pstPmtEsInfo);

	return;
}

/*	SUT 			: svc_si_UtAudioEsSelect
 *	Expected Result : ERR_OK
 *	Type			: Multichannel On by User and Audio Description Off
 *	Comments		: Find Multichannel Track (avoid AudioDescription selection if possible)
 */
void svc_si_TcAudioSelectMultiUser2(Cutest_TestCase_t *pstTc)
{
	HERROR					 hErr;
	SvcSi_SelectedEs_t		 stSelectedAudio;
	const HxList_t			*pstAudioList;
	const svcSi_PmtEsInfo_t	*pstPmtEsInfo;
	const SvcSi_Config_t	*pstConfig;

	// 1. Setup Fixture
	svc_si_UtAudioEsSetScenario (pstTc);		//	Set Up Test Stub
	pstAudioList	= svc_si_UtAudioEsGetInfo (	5,		//	ItemNum
												3,		//	[0:"eng", 1:"", Multi:"no",  AD:"no" ]
												7,		//	[0:"ger", 1:"", Multi:"no",  AD:"no" ]
												8,		//	[0:"swe", 1:"", Multi:"yes", AD:"yes"]
												9,		//	[0:"swe", 1:"", Multi:"yes", AD:"no" ]
												11,	 	//	[0:"swe", 1:"", Multi:"no",  AD:"no" ]
												(HUINT32)-1);
	pstPmtEsInfo	= svc_si_UtAudioEsSetList (NULL, pstAudioList);
	pstConfig		= svc_si_UtAudioEsGetConfig (	"ger",	"eng",	"nor",	// szAudLang0, szAudLang1, szOsdLang
													FALSE, 	FALSE,			// bMultiMenu, bAdMenu,
													"swe",					// szAudLangUser
													TRUE, 	FALSE);			// bMultiUser, bAdUser
	svc_si_UtInitSelectedEs (&stSelectedAudio);

	// 2. Exercise
	hErr = svc_si_UtAudioEsSelect (pstTc, TRUE, pstPmtEsInfo, pstConfig, &stSelectedAudio);

	// 3. Verify
	svc_si_UtAudioEsVeritySelectOutput(pstTc, hErr, &stSelectedAudio, 3, eDxDUALMONO_SELECT_UNKNOWN);

	// 4. Teardown
	svc_si_UtFreeConfig ((SvcSi_Config_t *)pstConfig);
	svc_si_UtFreeEsInfo ((svcSi_PmtEsInfo_t *)pstPmtEsInfo);

	return;
}

/*	SUT 			: svc_si_UtAudioEsSelect
 *	Expected Result : ERR_OK
 *	Type			: Multichannel on Audio Description off and Language Matched (Many Multichannel Audio)
 *	Comments		: Find Multichannel Track and Lang Aud0
 */
void svc_si_TcAudioSelectMultiAud0(Cutest_TestCase_t *pstTc)
{
	HERROR					 hErr;
	SvcSi_SelectedEs_t 		 stSelectedAudio;
	const HxList_t			*pstAudioList;
	const svcSi_PmtEsInfo_t	*pstPmtEsInfo;
	const SvcSi_Config_t	*pstConfig;

	// 1. Setup Fixture
	svc_si_UtAudioEsSetScenario (pstTc);		//	Set Up Test Stub
	pstAudioList	= svc_si_UtAudioEsGetInfo (	5,		//	ItemNum
												4,		//	[0:"ger", 1:"", Multi:"yes", AD:"yes"]
												5,		//	[0:"ger", 1:"", Multi:"yes", AD:"no" ]
												8,		//	[0:"swe", 1:"", Multi:"yes", AD:"yes"]
												9,		//	[0:"swe", 1:"", Multi:"yes", AD:"no" ]
												11,	 	//	[0:"swe", 1:"", Multi:"no",  AD:"no" ]
												(HUINT32)-1);
	pstPmtEsInfo	= svc_si_UtAudioEsSetList (NULL, pstAudioList);
	pstConfig		= svc_si_UtAudioEsGetConfig (	"ger",	"eng",	"nor",	// szAudLang0, szAudLang1, szOsdLang
													TRUE, 	FALSE,			// bMultiMenu, bAdMenu,
													"0",					// szAudLangUser
													FALSE, 	FALSE);			// bMultiUser, bAdUser
	svc_si_UtInitSelectedEs (&stSelectedAudio);

	// 2. Exercise
	hErr = svc_si_UtAudioEsSelect (pstTc, TRUE, pstPmtEsInfo, pstConfig, &stSelectedAudio);

	// 3. Verify
	svc_si_UtAudioEsVeritySelectOutput(pstTc, hErr, &stSelectedAudio, 1, eDxDUALMONO_SELECT_UNKNOWN);

	// 4. Teardown
	svc_si_UtFreeConfig ((SvcSi_Config_t *)pstConfig);
	svc_si_UtFreeEsInfo ((svcSi_PmtEsInfo_t *)pstPmtEsInfo);

	return;
}

/*	SUT 			: svc_si_UtAudioEsSelect
 *	Expected Result : ERR_OK
 *	Type			: Audio Description and Multichannel On by Menu
 *	Comments		: (Lang Aud0 Matched)
 */
void svc_si_TcAudioSelectAdMultiAud0(Cutest_TestCase_t *pstTc)
{
	HERROR					 hErr;
	SvcSi_SelectedEs_t		 stSelectedAudio;
	const HxList_t			*pstAudioList;
	const svcSi_PmtEsInfo_t	*pstPmtEsInfo;
	const SvcSi_Config_t	*pstConfig;

	// 1. Setup Fixture
	svc_si_UtAudioEsSetScenario (pstTc);		//	Set Up Test Stub
	pstAudioList	= svc_si_UtAudioEsGetInfo (	5,		//	ItemNum
												4,		//	[0:"ger", 1:"", Multi:"yes", AD:"yes"]
												5,		//	[0:"ger", 1:"", Multi:"yes", AD:"no" ]
												6,		//	[0:"ger", 1:"", Multi:"no",  AD:"yes"]
												7,		//	[0:"ger", 1:"", Multi:"no",  AD:"no" ]
												1,	 	//	[0:"swe", 1:"", Multi:"no",  AD:"yes"]
												(HUINT32)-1);
	pstPmtEsInfo	= svc_si_UtAudioEsSetList (NULL, pstAudioList);
	pstConfig		= svc_si_UtAudioEsGetConfig (	"ger",	"eng",	"nor",	// szAudLang0, szAudLang1, szOsdLang
													TRUE, 	TRUE,			// bMultiMenu, bAdMenu,
													"0",					// szAudLangUser
													FALSE, 	FALSE);			// bMultiUser, bAdUser
	svc_si_UtInitSelectedEs (&stSelectedAudio);

	// 2. Exercise
	hErr = svc_si_UtAudioEsSelect (pstTc, TRUE, pstPmtEsInfo, pstConfig, &stSelectedAudio);

	// 3. Verify
	svc_si_UtAudioEsVeritySelectOutput(pstTc, hErr, &stSelectedAudio, 0, eDxDUALMONO_SELECT_UNKNOWN);

	// 4. Teardown
	svc_si_UtFreeConfig ((SvcSi_Config_t *)pstConfig);
	svc_si_UtFreeEsInfo ((svcSi_PmtEsInfo_t *)pstPmtEsInfo);

	return;
}

/*	SUT 			: svc_si_UtAudioEsSelect
 *	Expected Result : ERR_OK
 *	Type			: Find Lang User
 *	Comments		: User Lang is on Dual Mono Right Track
 */
void svc_si_TcAudioSelectDualMonoRightUserLang(Cutest_TestCase_t *pstTc)
{
	HERROR					 hErr;
	SvcSi_SelectedEs_t		 stSelectedAudio;
	const HxList_t			*pstAudioList;
	const svcSi_PmtEsInfo_t	*pstPmtEsInfo;
	const SvcSi_Config_t	*pstConfig;

	// 1. Setup Fixture
	svc_si_UtAudioEsSetScenario (pstTc);		//	Set Up Test Stub
	pstAudioList	= svc_si_UtAudioEsGetInfo (	5,		//	ItemNum
												16,		//	[0:"eng", 1:"swe", Multi:"no",  AD:"no" ]
												17,		//	[0:"dan", 1:"nor", Multi:"no",  AD:"no" ]
												18,		//	[0:"fin", 1:"rus", Multi:"no",  AD:"no" ]
												19,		//	[0:"fra", 1:"ger", Multi:"no",  AD:"no" ]
												27,		//	[0:"fin", 1:"   ", Multi:"no",  AD:"no" ]
												(HUINT32)-1);
	pstPmtEsInfo	= svc_si_UtAudioEsSetList (NULL, pstAudioList);
	pstConfig		= svc_si_UtAudioEsGetConfig (	"ger",	"eng",	"nor",	// szAudLang0, szAudLang1, szOsdLang
													FALSE, 	FALSE,			// bMultiMenu, bAdMenu,
													"swe",					// szAudLangUser
													FALSE, 	FALSE);			// bMultiUser, bAdUser
	svc_si_UtInitSelectedEs (&stSelectedAudio);

	// 2. Exercise
	hErr = svc_si_UtAudioEsSelect (pstTc, TRUE, pstPmtEsInfo, pstConfig, &stSelectedAudio);

	// 3. Verify
	svc_si_UtAudioEsVeritySelectOutput(pstTc, hErr, &stSelectedAudio, 0, eDxDUALMONO_SELECT_RIGHT);

	// 4. Teardown
	svc_si_UtFreeConfig ((SvcSi_Config_t *)pstConfig);
	svc_si_UtFreeEsInfo ((svcSi_PmtEsInfo_t *)pstPmtEsInfo);

	return;
}

/*	SUT 			: svc_si_UtAudioEsSelect
 *	Expected Result : ERR_OK
 *	Type			: Find Lang User
 *	Comments		: User Lang Track is on both Dual Mono Right and Stereo Track
 *					  Stereo has higher priority than dual mono
 */
void svc_si_TcAudioSelectDualMonoNonUserLang(Cutest_TestCase_t *pstTc)
{
	HERROR					 hErr;
	SvcSi_SelectedEs_t		 stSelectedAudio;
	const HxList_t			*pstAudioList;
	const svcSi_PmtEsInfo_t	*pstPmtEsInfo;
	const SvcSi_Config_t	*pstConfig;

	// 1. Setup Fixture
	svc_si_UtAudioEsSetScenario (pstTc);		//	Set Up Test Stub
	pstAudioList	= svc_si_UtAudioEsGetInfo (	6,		//	ItemNum
												16,		//	[0:"eng", 1:"swe", Multi:"no",  AD:"no" ]
												17,		//	[0:"dan", 1:"nor", Multi:"no",  AD:"no" ]
												18,		//	[0:"fin", 1:"rus", Multi:"no",  AD:"no" ]
												19,		//	[0:"fra", 1:"ger", Multi:"no",  AD:"no" ]
												27,		//	[0:"fin", 1:"   ", Multi:"no",  AD:"no" ]
												11,		//	[0:"swe", 1:"   ", Multi:"no",  AD:"no" ]
												(HUINT32)-1);
	pstPmtEsInfo	= svc_si_UtAudioEsSetList (NULL, pstAudioList);
	pstConfig		= svc_si_UtAudioEsGetConfig (	"ger",	"eng",	"nor",	// szAudLang0, szAudLang1, szOsdLang
													FALSE, 	FALSE,			// bMultiMenu, bAdMenu,
													"swe",					// szAudLangUser
													FALSE, 	FALSE);			// bMultiUser, bAdUser
	svc_si_UtInitSelectedEs (&stSelectedAudio);

	// 2. Exercise
	hErr = svc_si_UtAudioEsSelect (pstTc, TRUE, pstPmtEsInfo, pstConfig, &stSelectedAudio);

	// 3. Verify
	svc_si_UtAudioEsVeritySelectOutput(pstTc, hErr, &stSelectedAudio, 5, eDxDUALMONO_SELECT_UNKNOWN);

	// 4. Teardown
	svc_si_UtFreeConfig ((SvcSi_Config_t *)pstConfig);
	svc_si_UtFreeEsInfo ((svcSi_PmtEsInfo_t *)pstPmtEsInfo);

	return;
}

/*	SUT 			: svc_si_UtAudioEsSelect
 *	Expected Result : ERR_OK
 *	Type			: Find Lang Aud0
 *	Comments		: Lang Aud0 is on Dual Mono Left Track
 */
void svc_si_TcAudioSelectDualMonoLeftAud0(Cutest_TestCase_t *pstTc)
{
	HERROR					 hErr;
	SvcSi_SelectedEs_t		 stSelectedAudio;
	const HxList_t			*pstAudioList;
	const svcSi_PmtEsInfo_t	*pstPmtEsInfo;
	const SvcSi_Config_t	*pstConfig;

	// 1. Setup Fixture
	svc_si_UtAudioEsSetScenario (pstTc);		//	Set Up Test Stub
	pstAudioList	= svc_si_UtAudioEsGetInfo (	6,		//	ItemNum
												16,		//	[0:"eng", 1:"swe", Multi:"no",  AD:"no" ]
												17,		//	[0:"dan", 1:"nor", Multi:"no",  AD:"no" ]
												18,		//	[0:"fin", 1:"rus", Multi:"no",  AD:"no" ]
												19,		//	[0:"fra", 1:"ger", Multi:"no",  AD:"no" ]
												27,		//	[0:"fin", 1:"   ", Multi:"no",  AD:"no" ]
												11,		//	[0:"swe", 1:"   ", Multi:"no",  AD:"no" ]
												(HUINT32)-1);
	pstPmtEsInfo	= svc_si_UtAudioEsSetList (NULL, pstAudioList);
	pstConfig		= svc_si_UtAudioEsGetConfig (	"dan",	"eng",	"nor",	// szAudLang0, szAudLang1, szOsdLang
													FALSE, 	FALSE,			// bMultiMenu, bAdMenu,
													"",						// szAudLangUser
													FALSE, 	FALSE);			// bMultiUser, bAdUser
	svc_si_UtInitSelectedEs (&stSelectedAudio);

	// 2. Exercise
	hErr = svc_si_UtAudioEsSelect (pstTc, TRUE, pstPmtEsInfo, pstConfig, &stSelectedAudio);

	// 3. Verify
	svc_si_UtAudioEsVeritySelectOutput(pstTc, hErr, &stSelectedAudio, 1, eDxDUALMONO_SELECT_LEFT);

	// 4. Teardown
	svc_si_UtFreeConfig ((SvcSi_Config_t *)pstConfig);
	svc_si_UtFreeEsInfo ((svcSi_PmtEsInfo_t *)pstPmtEsInfo);

	return;
}

/*	SUT 			: svc_si_UtAudioEsSelect
 *	Expected Result : ERR_OK
 *	Type			: Find Audio Description Menu and Lang Menu
 *	Comments		:
 */
void svc_si_TcAudioSelectAdMenuLang(Cutest_TestCase_t *pstTc)
{
	HERROR					 hErr;
	SvcSi_SelectedEs_t		 stSelectedAudio;
	const HxList_t			*pstAudioList;
	const svcSi_PmtEsInfo_t	*pstPmtEsInfo;
	const SvcSi_Config_t	*pstConfig;

	// 1. Setup Fixture
	svc_si_UtAudioEsSetScenario (pstTc);		//	Set Up Test Stub
	pstAudioList	= svc_si_UtAudioEsGetInfo (	5,		//	ItemNum
												3,		//	[0:"eng", 1:"", Multi:"no",  AD:"no" ]
												6,		//	[0:"ger", 1:"", Multi:"no",  AD:"yes"]
												14,		//	[0:"nor", 1:"", Multi:"no",  AD:"yes"]
												11,		//	[0:"swe", 1:"", Multi:"no",  AD:"no" ]
												28,		//	[0:"fin", 1:"", Multi:"no",  AD:"yes"]
												(HUINT32)-1);
	pstPmtEsInfo	= svc_si_UtAudioEsSetList (NULL, pstAudioList);
	pstConfig		= svc_si_UtAudioEsGetConfig (	"dan",	"eng",	"nor",	// szAudLang0, szAudLang1, szOsdLang
													FALSE, 	TRUE,			// bMultiMenu, bAdMenu,
													"swe",					// szAudLangUser
													FALSE, 	FALSE);			// bMultiUser, bAdUser
	svc_si_UtInitSelectedEs (&stSelectedAudio);

	// 2. Exercise
	hErr = svc_si_UtAudioEsSelect (pstTc, TRUE, pstPmtEsInfo, pstConfig, &stSelectedAudio);

	// 3. Verify
	svc_si_UtAudioEsVeritySelectOutput(pstTc, hErr, &stSelectedAudio, 2, eDxDUALMONO_SELECT_UNKNOWN);

	// 4. Teardown
	svc_si_UtFreeConfig ((SvcSi_Config_t *)pstConfig);
	svc_si_UtFreeEsInfo ((svcSi_PmtEsInfo_t *)pstPmtEsInfo);

	return;
}

/*	SUT 			: svc_si_UtAudioEsSelect
 *	Expected Result : ERR_OK
 *	Type			: Find Audio Description and Lang
 *	Comments		: Found AD Track but language is not matched.
					  Audio Description has higher priority than language
 */
void svc_si_TcAudioSelectAdLangNotMatched1(Cutest_TestCase_t *pstTc)
{
	HERROR					 hErr;
	SvcSi_SelectedEs_t		 stSelectedAudio;
	const HxList_t			*pstAudioList;
	const svcSi_PmtEsInfo_t	*pstPmtEsInfo;
	const SvcSi_Config_t	*pstConfig;

	// 1. Setup Fixture
	svc_si_UtAudioEsSetScenario (pstTc);		//	Set Up Test Stub
	pstAudioList	= svc_si_UtAudioEsGetInfo (	5,		//	ItemNum
												3,		//	[0:"eng", 1:"", Multi:"no",  AD:"no" ]
												7,		//	[0:"ger", 1:"", Multi:"no",  AD:"no" ]
												15,		//	[0:"nor", 1:"", Multi:"no",  AD:"no" ]
												11,		//	[0:"swe", 1:"", Multi:"no",  AD:"no" ]
												28,		//	[0:"fin", 1:"", Multi:"no",  AD:"yes"]
												(HUINT32)-1);
	pstPmtEsInfo	= svc_si_UtAudioEsSetList (NULL, pstAudioList);
	pstConfig		= svc_si_UtAudioEsGetConfig (	"dan",	"eng",	"nor",	// szAudLang0, szAudLang1, szOsdLang
													FALSE, 	TRUE,			// bMultiMenu, bAdMenu,
													"swe",					// szAudLangUser
													FALSE, 	FALSE);			// bMultiUser, bAdUser
	svc_si_UtInitSelectedEs (&stSelectedAudio);

	// 2. Exercise
	hErr = svc_si_UtAudioEsSelect (pstTc, TRUE, pstPmtEsInfo, pstConfig, &stSelectedAudio);

	// 3. Verify
	svc_si_UtAudioEsVeritySelectOutput(pstTc, hErr, &stSelectedAudio, 4, eDxDUALMONO_SELECT_UNKNOWN);

	// 4. Teardown
	svc_si_UtFreeConfig ((SvcSi_Config_t *)pstConfig);
	svc_si_UtFreeEsInfo ((svcSi_PmtEsInfo_t *)pstPmtEsInfo);

	return;
}

/*	SUT 			: svc_si_UtAudioEsSelect
 *	Expected Result : ERR_OK
 *	Type			: Find Audio Description and Lang
 *	Comments		: Found AD Track but no language information.
					  Even though there is no langauge information, Audio Description has higher priority.
 */
void svc_si_TcAudioSelectAdLangNotMatched2(Cutest_TestCase_t *pstTc)
{
	HERROR					 hErr;
	SvcSi_SelectedEs_t		 stSelectedAudio;
	const HxList_t			*pstAudioList;
	const svcSi_PmtEsInfo_t	*pstPmtEsInfo;
	const SvcSi_Config_t	*pstConfig;

	// 1. Setup Fixture
	svc_si_UtAudioEsSetScenario (pstTc);		//	Set Up Test Stub
	pstAudioList	= svc_si_UtAudioEsGetInfo (	5,		//	ItemNum
												3,		//	[0:"eng", 1:"", Multi:"no",  AD:"no" ]
												7,		//	[0:"ger", 1:"", Multi:"no",  AD:"no" ]
												24,		//	[0:"   ", 1:"", Multi:"no",  AD:"yes"]
												15,		//	[0:"nor", 1:"", Multi:"no",  AD:"no" ]
												27,		//	[0:"fin", 1:"", Multi:"no",  AD:"no" ]
												(HUINT32)-1);
	pstPmtEsInfo	= svc_si_UtAudioEsSetList (NULL, pstAudioList);
	pstConfig		= svc_si_UtAudioEsGetConfig (	"dan",	"eng",	"nor",	// szAudLang0, szAudLang1, szOsdLang
													FALSE, 	TRUE,			// bMultiMenu, bAdMenu,
													"swe",					// szAudLangUser
													FALSE, 	FALSE);			// bMultiUser, bAdUser
	svc_si_UtInitSelectedEs (&stSelectedAudio);

	// 2. Exercise
	hErr = svc_si_UtAudioEsSelect (pstTc, TRUE, pstPmtEsInfo, pstConfig, &stSelectedAudio);

	// 3. Verify
	svc_si_UtAudioEsVeritySelectOutput(pstTc, hErr, &stSelectedAudio, 2, eDxDUALMONO_SELECT_UNKNOWN);

	// 4. Teardown
	svc_si_UtFreeConfig ((SvcSi_Config_t *)pstConfig);
	svc_si_UtFreeEsInfo ((svcSi_PmtEsInfo_t *)pstPmtEsInfo);

	return;
}

/*	SUT 			: svc_si_UtAudioEsSelect
 *	Expected Result : ERR_OK
 *	Type			: Find Audio Description and Lang
 *	Comments		: Found AD Track and Lang Aud0
 */
void svc_si_TcAudioSelectAdAud1(Cutest_TestCase_t *pstTc)
{
	HERROR					 hErr;
	SvcSi_SelectedEs_t		 stSelectedAudio;
	const HxList_t			*pstAudioList;
	const svcSi_PmtEsInfo_t	*pstPmtEsInfo;
	const SvcSi_Config_t	*pstConfig;

	// 1. Setup Fixture
	svc_si_UtAudioEsSetScenario (pstTc);		//	Set Up Test Stub
	pstAudioList	= svc_si_UtAudioEsGetInfo (	4,		//	ItemNum
												3,		//	[0:"eng", 1:"", Multi:"no",  AD:"no" ]
												7,		//	[0:"ger", 1:"", Multi:"no",  AD:"no" ]
												15,		//	[0:"nor", 1:"", Multi:"no",  AD:"no" ]
												27,		//	[0:"fin", 1:"", Multi:"no",  AD:"no" ]
												(HUINT32)-1);
	pstPmtEsInfo	= svc_si_UtAudioEsSetList (NULL, pstAudioList);
	pstConfig		= svc_si_UtAudioEsGetConfig (	"dan",	"eng",	"nor",	// szAudLang0, szAudLang1, szOsdLang
													FALSE, 	TRUE,			// bMultiMenu, bAdMenu,
													"swe",					// szAudLangUser
													FALSE, 	FALSE);			// bMultiUser, bAdUser
	svc_si_UtInitSelectedEs (&stSelectedAudio);

	// 2. Exercise
	hErr = svc_si_UtAudioEsSelect (pstTc, TRUE, pstPmtEsInfo, pstConfig, &stSelectedAudio);

	// 3. Verify
	svc_si_UtAudioEsVeritySelectOutput(pstTc, hErr, &stSelectedAudio, 0, eDxDUALMONO_SELECT_UNKNOWN);

	// 4. Teardown
	svc_si_UtFreeConfig ((SvcSi_Config_t *)pstConfig);
	svc_si_UtFreeEsInfo ((svcSi_PmtEsInfo_t *)pstPmtEsInfo);

	return;
}

/* end of file */

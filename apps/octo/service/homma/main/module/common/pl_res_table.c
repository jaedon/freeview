/*************************************************************************************************************
	File            : pl_dmr.h
	author          : IPM tipmsw1@humaxdigital.com
	comment         :
	date            :
	attention       :

	(c) 2011-2012 Humax Co., Ltd.
	This program is produced by Humax Co., Ltd. (Humax) and
	the proprietary Software of Humax and its licensors. Humax provides you, as an Authorized Licensee,
	non-assignable, non-transferable and non-exclusive license to use this Software.
	You acknowledge that this Software contains valuable trade secrets of Humax and by using this Software
	you agree to the responsibility to take all reasonable efforts to protect the any information
	you receive from Humax. You are not permitted to duplicate, modify, distribute, sell or lease and
	reverse engineer or extract the source code of this Software unless you have Humax's written permission
	to do so.
	If you have no authorized license, discontinue using this Software immediately.

	THE SOFTWARE IS PROVIDED ¡°AS IS¡± AND HUMAX MAKES NO PROMISES, REPRESENTATIONS OR WARRANTIES,
	EITHER EXPRESS, IMPLIED OR STATUTORY, OR OTHERWISE, WITH RESPECT TO THE SOFTWARE.
	IN NO EVENT SHALL HUMAX BE LIABLE FOR LOST PROFITS, REVENUES, OR DATA, FINANCIAL LOSSES OR
	INDIRECT, SPECIAL, CONSEQUENTIAL, EXEMPLARTY OR PUNITIVE DAMAGES WHATSOEVER RELATING TO YOUR USE OR
	INABILITY TO USE THE SOFTWARE.

	This License is effective until terminated. You may terminate this License at any time
	by destroying all copies of the Software including all documentation. This License will terminate
	immediately without notice from Humax to you if you fail to comply with any provision of this License.
	Upon termination, you must destroy all copies of the Software and all documentation.

	The laws of the Republic of Korea will apply to any disputes arising out of or relating to
	this Copyright Notice. All claims arising out of or relating to this Copyright Notice will be litigated
	in the Seoul Central District Court, in the Republic of Korea.
*************************************************************************************************************/

#define	________________Header_Files_______________________________ 
#include "mxres_ref.h"
#include "mxmedia.h"

#include "mxres.h"
#include "mxfile.h"
#include "mxres_ref.h"
#include "mxmedia.h"

#define ________________Private_Static_Variables___________________

// --------------------------------------------------------------------------------
// 우선순위를 선택하기 위한 PreCmdTable
// --------------------------------------------------------------------------------
// Usages :
//			- 이테이블에 값이 있는 순서대로 우선 순위가 높다.
//			- 우선순위로서 사용하고 싶지 않은 명령이 있으면 명시적으로 ExCommon_NOP 값을 넣는다.
// 주의	:
//			- maxBit 값은 수정하지 말것..연관도가 높은 갚임.
//			- maxBit 값은 각 항목이 차지하는 최대 비트 수 이므로 건드리면 안된다.
// --------------------------------------------------------------------------------
MXRES_PRE_TABLE g_preVideoCmdTable[ ExVideo_Max ] =
{
	// command	,		maxBit
	{ ExVideo_BPlayable,	2	},
	{ ExVideo_Direct,		2	},
	{ ExVideo_ByteSeek,		2	},
	{ ExVideo_TimeSeek,		2	},
	{ ExVideo_Speed,		2	},
	{ ExVideo_PN,			8	},
	{ ExVideo_Label,		4	},
	{ ExVideo_RES_Count,	8	},
};

MXRES_PRE_TABLE g_preAudioCmdTable[ ExAudio_Max ] =
{
	// command	,		maxBit
	{ ExAudio_BPlayable,	2	},
	{ ExAudio_Direct,		2	},
	{ ExAudio_ByteSeek,		2	},
	{ ExAudio_TimeSeek,		2	},
	{ ExAudio_Speed,		2	},
	{ ExAudio_PN,			8	},
	{ ExAudio_Channel,		4	},
	{ ExAudio_RES_Count,	8	},

};

MXRES_PRE_TABLE g_preImageCmdTable[ ExImage_Max ] =
{
	// command	,				maxBit
	{ ExImage_BPlayable,		 2	},
	{ ExImage_Direct ,			 2	},
	{ ExImage_Resolution,		 2	},
	{ ExImage_PN,				 8	},	//{ ExCommon_NOP,	 0	},
	{ ExImage_RES_Count,		 8	},
};



// 가장 잘 보이는 방식
// 큰게 장땡이다.

// 사용법 :
// 뒤에 있는 숫자값만 손대시오...
// 숫자가 작을 수록 우선순위가 높은것입니다.
// 값의 범위는 0 ~ 255입니다.
// 앞의 스트링은 수정 및 삽입,삭제를 하지 마시오.
MXPN_PRI_MAP
	//-----------------------------------------------------------------------------------------//
	PNPRI_IMAGE_MAP[] =
	//-----------------------------------------------------------------------------------------//
{
	{ "JPEG_SM" ,       3 } ,		// max = 640 x 480
    { "JPEG_MED" ,      2 } ,		// max = 1024 x 768
    { "JPEG_LRG" ,      1 } ,		// max = 4096 x 4096
    { "JPEG_TN" ,       4 } ,		// max = 160 x 160
    { "JPEG_SM_ICO" ,   6 } ,		// max = 48 x 48
    { "JPEG_LRG_ICO" ,  5 } ,		// max = 120 x 120


    { "PNG_LRG" ,		1 } ,		// max = 4096 x 4096
    { "PNG_TN" ,		4 } ,		// max = 160 x 160
    { "PNG_SM_ICO" ,	6 } ,		// max = 48 x 48
    { "PNG_LRG_ICO" ,	5 } ,		// max = 120 x 120

	{  UNKNOWN_STR,		3} ,		// UNKNOWN - BMP
	{  0,				0} ,

};

MXPN_PRI_MAP
	//-----------------------------------------------------------------------------------------//
	PNPRI_AUDIO_MAP[] =
	//-----------------------------------------------------------------------------------------//
{

	//-----------------------------------------------------------------------------------------//
	//profileID						priority													mime-type				channel		usage
	//-----------------------------------------------------------------------------------------//

	//AC3 profiles
    { "AC3" ,			0	} ,		// audio/vnd.dolby.dd-raw , 2ch multi , dvd audio

	//AMR profiles
	{ "AMR_3GPP" ,		0	} ,		// audio/3gpp, audio/mp4 , mono , cellular Phones and PDA
	{ "AMR_WBplus" ,	0	} ,		// audio/3gpp,				2ch , cellular Phones and PDA , DVB

	//ATRAC3Plus Profiles		// Sony Compress Codec
	{ "ATRAC3Plus" ,	0	} ,		// audio/x-sony-oma,		2ch , md , pc ,multi channel audio

	//LPCM Profiles
	{ "LPCM" ,			0	},		// audio/L16,				2ch ,	Radio Broadcasts , User Created Audio Content

	//MP3 Profiles
	{ "MP3" ,			0	} ,		// audio/mpeg,				2ch ,
	{ "MP3X" ,			0	} ,		// audio/mpeg,				2ch ,	Radio BroadCasts , Ripping CD Audio Content , internet radio broadcasting

	//MPEG-4(AAC) Profiles
	{ "AAC_ADTS" ,		0	} ,	// audio/vnd.dlna.adts,		2ch ,	Radio BroadCasts , internet radio broadcasting , ARIB Radio
	{ "AAC_ADTS_320" ,	0	} ,	// audio/vnd.dlna.adts,		2ch ,	Radio BroadCasts , Digital radio broadcasting , ARIB Radio
	{ "AAC_LTP_ISO"	 ,	0	} ,	// audio/mp4,audio/3gpp,	2ch ,	Radio BroadCasts , Audio component of TV Broadcast
																								// audio/vnd.dlna.adts
	{ "AAC_LTP_MULT5_ISO",	0} ,// audio/mp4,audio/3gpp,	multi ,	Radio BroadCasts , Audio component of TV Broadcast ( 5.1 channels )
																								// audio/vnd.dlna.adts
	{ "AAC_LTP_MULT7_ISO",	0} ,// audio/mp4,audio/3gpp,	multi ,	Radio BroadCasts , Audio component of TV Broadcast ( 7.1 channels )
																								// audio/vnd.dlna.adts
	{ "AAC_MULT5_ADTS",	0} ,	// audio/vnd.dlna.adts,		multi ,	Radio BroadCasts , internet radio broadcasting , (5.1 Channels )
	{ "AAC_MULT5_ISO",	0} ,	// audio/mp4,audio/3gpp,	multi ,	required in ISMA (5.1 Channels )

	{ "HEAAC_L2_ADTS",	0} ,	// audio/vnd.dlna.adts,		2ch,	Radio BroadCasts , Digital radio broadcasting
	{ "HEAAC_L2_ISO",	0} ,	// audio/mp4,audio/3gpp,	2ch,	Radio BroadCasts , Digital radio broadcasting
	{ "HEAAC_L3_ADTS",	0} ,	// audio/vnd.dlna.adts,		2ch,	Radio BroadCasts , Digital radio broadcasting
	{ "HEAAC_L3_ISO",	0} ,	// audio/mp4,audio/3gpp,	2ch,	Radio BroadCasts , Digital radio broadcasting
	{ "HEAAC_MULT5_ADTS",0} ,	// audio/vnd.dlna.adts,		multi ,	Radio BroadCasts , DVD-A , Multichannel Music Tracks from AV Content , (5.1 Channels )
	{ "HEAAC_MULT5_ISO",0} ,	// audio/mp4,audio/3gpp,	multi ,	required in ISMA (5.1 Channels ) , 3GPP , ISMA , DVB
	{ "HEAAC_ADTS_320",	0} ,	// audio/vnd.dlna.adts,		2ch ,	Radio BroadCasts , Digital radio broadcasting ,
	{ "HEAAC_L2_ISO_320",	0} ,// audio/mp4,audio/3gpp,	2ch,	Radio BroadCasts , Digital radio broadcasting

	{ "BSAC_ISO",		0} ,	// audio/mp4,audio/3gpp,	2ch,	Radio BroadCasts , Audio component of TV Broadcast
	{ "BSAC_MULT5_ISO",	0} ,	// audio/mp4,audio/3gpp,	2ch,	Radio BroadCasts , Audio component of TV Broadcast ( 5.1 Channels )


	//WMA Profiles
	{ "WMABASE" ,	0 } ,		// audio/x-ms-wma,				2ch ,	Internet Music Services , Internet Radio , wma content ( bitrates less than 193kbps )
	{ "WMAFULL" ,	0 } ,		// audio/x-ms-wma,				2ch ,	Internet Music Services , Internet Radio
	{ "WMAPRO" ,	0 } ,		// audio/x-ms-wma,				2ch  multi,	Internet Music Services , Internet Radio ( WMA Professional version )


	{ UNKNOWN_STR,	0 } ,		// UNKNOWN
    {  0,				0} ,

};

MXPN_PRI_MAP
	//-----------------------------------------------------------------------------------------//
	PNPRI_VIDEO_MAP[] =
	//-----------------------------------------------------------------------------------------//
{

	//-----------------------------------------------------------------------------------------//
	//		Profile ID			MIME Type						Media Profile Define
	//-----------------------------------------------------------------------------------------//
	//-----------------------------------------------------------------------------------------//
	//MPEG 1
	//-----------------------------------------------------------------------------------------//
	{"MPEG1",					0	}	,//0
	//-----------------------------------------------------------------------------------------//


	//-----------------------------------------------------------------------------------------//
	//MPEG 2
	//-----------------------------------------------------------------------------------------//
	{"MPEG_PS_NTSC"	,			0	}	,	//1
	{"MPEG_PS_NTSC_XAC3",		0	}	,
	{"MPEG_PS_PAL"	,			0	}	,
	{"MPEG_PS_PAL_XAC3"	,		0	}	,
	{"MPEG_TS_SD_NA"	,		0	}	,
	  //
	{"MPEG_TS_SD_NA_T"	,		0	}	,
	{"MPEG_TS_SD_NA_ISO",		0	}	,
	{"MPEG_TS_HD_NA"	,		0	}	,
	{"MPEG_TS_HD_NA_T"	,		0	}	,
	  //
	{"MPEG_TS_HD_NA_ISO",		0	}	,
	{"MPEG_TS_SD_EU"	,		0	}	,
	{"MPEG_TS_SD_EU_T"	,		0	}	,
	{"MPEG_TS_SD_EU_ISO",		0	}	,
	  //
	{"MPEG_TS_SD_KO"	,		0	}	,
	{"MPEG_TS_SD_KO_T"	,		0	}	,
	{"MPEG_TS_SD_KO_ISO",		0	}	,
	  //
	{"MPEG_TS_HD_KO"	,		0	}	,
	{"MPEG_TS_HD_KO_T"	,		0	}	,
	{"MPEG_TS_HD_KO_ISO"	,	0	}	,
	  //
	{"MPEG_TS_HD_KO_XAC3"	,	0	}	,
	{"MPEG_TS_HD_KO_XAC3_T"	,	0	}	,
	{"MPEG_TS_HD_KO_XAC3_ISO",	0	}	,
	  //
	{"MPEG_TS_HD_NA_XAC3"	,	0	}	,
	{"MPEG_TS_HD_NA_XAC3_T"	,	0	}	,
	{"MPEG_TS_HD_NA_XAC3_ISO",	0	}	,
	  //
	{"MPEG_TS_SD_KO_XAC3"	,	0	}	,
	{"MPEG_TS_SD_KO_XAC3_T"	,	0	}	,
	{"MPEG_TS_SD_KO_XAC3_ISO",	0	}	,
	  //
	{"MPEG_TS_SD_NA_XAC3"	,	0	}	,
	{"MPEG_TS_SD_NA_XAC3_T"	,	0	}	,
	{"MPEG_TS_SD_NA_XAC3_ISO",	0	}	,
	{"MPEG_TS_MP_LL_AAC"	,	0	}	,
	  //
	{"MPEG_TS_MP_LL_AAC_T"	,	0	}	,
	{"MPEG_TS_MP_LL_AAC_ISO",	0	}	,
	{"MPEG_ES_PAL"	,			0	}	,
	{"MPEG_ES_NTSC"	,			0	}	,
	{"MPEG_ES_PAL_XAC3"	,		0	}	,
	  //
	{"MPEG_ES_NTSC_XAC3"	,	0	}	,
	//-----------------------------------------------------------------------------------------//


	//-----------------------------------------------------------------------------------------//
	//MPEG 4 Part 2
	//-----------------------------------------------------------------------------------------//
	{"MPEG4_P2_MP4_SP_AAC",					0	}	,
	{"MPEG4_P2_MP4_SP_HEAAC" ,				0	}	,
	{"MPEG4_P2_MP4_SP_ATRAC3plus" ,			0	}	,
	{"MPEG4_P2_MP4_SP_AAC_LTP" ,			0	}	,
	{"MPEG4_P2_MP4_SP_L2_AAC" ,				0	}	,
	  //
	{"MPEG4_P2_MP4_SP_L2_AMR" ,				0	}	,
	{"MPEG4_P2_TS_SP_AAC"	,				0	}	,
	{"MPEG4_P2_TS_SP_AAC_T"	,				0	}	,
	{"MPEG4_P2_TS_SP_AAC_ISO" ,				0	}	,
	{"MPEG4_P2_TS_SP_MPEG1_L3" ,			0	}	,
	  //
	{"MPEG4_P2_TS_SP_MPEG1_L3_T" ,			0	}	,
	{"MPEG4_P2_TS_SP_MPEG1_L3_ISO" ,		0	}	,
	{"MPEG4_P2_TS_SP_AC3"	,				0	}	,
	{"MPEG4_P2_TS_SP_AC3_T"	,				0	}	,
	  //
	{"MPEG4_P2_TS_SP_AC3_ISO" ,				0	}	,
	{"MPEG4_P2_TS_SP_MPEG2_L2" ,			0	}	,
	{"MPEG4_P2_TS_SP_MPEG2_L2_T" ,			0	}	,
	{"MPEG4_P2_TS_SP_MPEG2_L2_ISO" ,		0	}	,
	  //
	{"MPEG4_P2_ASF_SP_G726"	,				0	}	,
	{"MPEG4_P2_MP4_SP_VGA_AAC" ,			0	}	,
	{"MPEG4_P2_MP4_SP_VGA_HEAAC" ,			0	}	,
	{"MPEG4_P2_MP4_ASP_AAC"	,				0	}	,
	{"MPEG4_P2_MP4_ASP_HEAAC" ,				0	}	,
	{"MPEG4_P2_MP4_ASP_HEAAC_MULT5",		0	}	,
	  //
	{"MPEG4_P2_MP4_ASP_ATRAC3plus" ,		0	}	,
	{"MPEG4_P2_TS_ASP_AAC"	,				0	}	,
	{"MPEG4_P2_TS_ASP_AAC_T" ,				0	}	,
	{"MPEG4_P2_TS_ASP_AAC_ISO" ,			0	}	,
	  //
	{"MPEG4_P2_TS_ASP_MPEG1_L3" ,			0	}	,
	{"MPEG4_P2_TS_ASP_MPEG1_L3_T" ,			0	}	,
	{"MPEG4_P2_TS_ASP_MPEG1_L3_ISO",		0	}	,
	{"MPEG4_P2_TS_ASP_AC3"	,				0	}	,
	  //
	{"MPEG4_P2_TS_ASP_AC3_T" ,				0	}	,
	{"MPEG4_P2_TS_ASP_AC3_ISO" ,			0	}	,
	{"MPEG4_P2_MP4_ASP_L5_SO_AAC" ,			0	}	,
	{"MPEG4_P2_MP4_ASP_L5_SO_HEAAC",		0	}	,
	  //
	{"MPEG4_P2_MP4_ASP_L5_SO_HEAAC_MULT5" ,	0	}	,
	{"MPEG4_P2_ASF_ASP_L5_SO_G726" ,		0	}	,
	{"MPEG4_P2_MP4_ASP_L4_SO_AAC" ,			0	}	,
	{"MPEG4_P2_MP4_ASP_L4_SO_HEAAC" ,		0	}	,
	  //
	{"MPEG4_P2_MP4_ASP_L4_SO_HEAAC_MULT5" ,	0	}	,
	{"MPEG4_P2_ASF_ASP_L4_SO_G726" ,		0	}	,
	{"MPEG4_H263_MP4_P0_L10_AAC" ,			0	}	,
	{"MPEG4_H263_MP4_P0_L10_AAC_LTP" ,		0	}	,
	{"MPEG4_H263_3GPP_P0_L10_AMR_WBplus" ,	0	}	,
	  //
	{"MPEG4_P2_TS_CO_AC3"	,				0	}	,
	{"MPEG4_P2_TS_CO_AC3_T"	,				0	}	,
	{"MPEG4_P2_TS_CO_AC3_ISO" ,				0	}	,
	{"MPEG4_P2_TS_CO_MPEG2_L2" ,			0	}	,
	  //
	{"MPEG4_P2_TS_CO_MPEG2_L2_T" ,			0	}	,
	{"MPEG4_P2_TS_CO_MPEG2_L2_ISO" ,		0	}	,
	{"MPEG4_P2_3GPP_SP_L0B_AAC"	,			0	}	,
	{"MPEG4_P2_3GPP_SP_L0B_AMR"	,			0	}	,
	{"MPEG4_H263_3GPP_P3_L10_AMR" ,			0	}	,
	//-----------------------------------------------------------------------------------------//


	//-----------------------------------------------------------------------------------------//
	//MPEG 4 Part 10 ( VALUPS_CODEC_AVC )
	//-----------------------------------------------------------------------------------------//
	{"AVC_TS_MP_SD_AAC_MULT5"	,			0	}	,
	{"AVC_TS_MP_SD_AAC_MULT5_T"	,			0	}	,
	{"AVC_TS_MP_SD_AAC_MULT5_ISO" ,			0	}	,
	{"AVC_TS_MP_SD_HEAAC_L2"	,			0	}	,
	  //
	{"AVC_TS_MP_SD_HEAAC_L2_T"	,			0	}	,
	{"AVC_TS_MP_SD_HEAAC_L2_ISO" ,			0	}	,
	{"AVC_TS_MP_SD_MPEG1_L3"	,			0	}	,
	{"AVC_TS_MP_SD_MPEG1_L3_T"	,			0	}	,
	  //
	{"AVC_TS_MP_SD_MPEG1_L3_ISO" ,			0	}	,
	{"AVC_TS_MP_SD_AC3"	,					0	}	,
	{"AVC_TS_MP_SD_AC3_T"	,				0	}	,
	{"AVC_TS_MP_SD_AC3_ISO"	,				0	}	,
	  //
	{"AVC_TS_MP_SD_AAC_LTP"	,				0	}	,
	{"AVC_TS_MP_SD_AAC_LTP_T" ,				0	}	,
	{"AVC_TS_MP_SD_AAC_LTP_ISO" ,			0	}	,
	{"AVC_TS_MP_SD_AAC_LTP_MULT5" ,			0	}	,
	  //
	{"AVC_TS_MP_SD_AAC_LTP_MULT5_T" ,		0	}	,
	{"AVC_TS_MP_SD_AAC_LTP_MULT5_ISO" ,		0	}	,
	{"AVC_TS_MP_SD_AAC_LTP_MULT7" ,			0	}	,
	{"AVC_TS_MP_SD_AAC_LTP_MULT7_T" ,		0	}	,
	  //
	{"AVC_TS_MP_SD_AAC_LTP_MULT7_ISO" ,		0	}	,
	{"AVC_TS_MP_SD_BSAC"	,				0	}	,
	{"AVC_TS_MP_SD_BSAC_T"	,				0	}	,
	{"AVC_TS_MP_SD_BSAC_ISO"	,			0	}	,
	  //
	{"AVC_MP4_MP_SD_AAC_MULT5"	,			0	}	,
	{"AVC_MP4_MP_SD_HEAAC_L2"	,			0	}	,
	{"AVC_MP4_MP_SD_MPEG1_L3"	,			0	}	,
	{"AVC_MP4_MP_SD_AC3"	,				0	}	,
	{"AVC_MP4_MP_SD_AAC_LTP" ,				0	}	,
	{"AVC_MP4_MP_SD_AAC_LTP_MULT5" ,		0	}	,
	  //
	{"AVC_MP4_MP_SD_AAC_LTP_MULT7" ,		0	}	,
	{"AVC_MP4_MP_SD_ATRAC3plus"	,			0	}	,
	{"AVC_MP4_BL_L3L_SD_AAC"	,			0	}	,
	{"AVC_MP4_BL_L3L_SD_HEAAC"	,			0	}	,
	{"AVC_MP4_BL_L3_SD_AAC"	,				0	}	,
	{"AVC_MP4_MP_SD_BSAC"	,				0	}	,
	  //
	{"AVC_TS_BL_CIF30_AAC_MULT5" ,			0	}	,
	{"AVC_TS_BL_CIF30_AAC_MULT5_T" ,		0	}	,
	{"AVC_TS_BL_CIF30_AAC_MULT5_ISO" ,		0	}	,
	  //
	{"AVC_TS_BL_CIF30_HEAAC_L2"	,			0	}	,
	{"AVC_TS_BL_CIF30_HEAAC_L2_T" ,			0	}	,
	{"AVC_TS_BL_CIF30_HEAAC_L2_ISO" ,		0	}	,
	  //
	{"AVC_TS_BL_CIF30_MPEG1_L3"	,			0	}	,
	{"AVC_TS_BL_CIF30_MPEG1_L3_T" ,			0	}	,
	{"AVC_TS_BL_CIF30_MPEG1_L3_ISO" ,		0	}	,
	{"AVC_TS_BL_CIF30_AC3"	,				0	}	,
	  //
	{"AVC_TS_BL_CIF30_AC3_T"	,			0	}	,
	{"AVC_TS_BL_CIF30_AC3_ISO"	,			0	}	,
	{"AVC_TS_BL_CIF30_AAC_LTP"	,			0	}	,
	{"AVC_TS_BL_CIF30_AAC_LTP_T" ,			0	}	,
	  //
	{"AVC_TS_BL_CIF30_AAC_LTP_ISO" ,		0	}	,
	{"AVC_TS_BL_CIF30_AAC_LTP_MULT5" ,		0	}	,
	{"AVC_TS_BL_CIF30_AAC_LTP_MULT5_T" ,	0	}	,
	{"AVC_TS_BL_CIF30_AAC_LTP_MULT5_ISO" ,	0	}	,
	  //
	{"AVC_TS_BL_CIF30_AAC_940"	,			0	}	,
	{"AVC_TS_BL_CIF30_AAC_940_T" ,			0	}	,
	{"AVC_TS_BL_CIF30_AAC_940_ISO" ,		0	}	,
	{"AVC_MP4_BL_CIF30_AAC_MULT5" ,			0	}	,
	  //
	{"AVC_MP4_BL_CIF30_HEAAC_L2" ,			0	}	,
	{"AVC_MP4_BL_CIF30_MPEG1_L3" ,			0	}	,
	{"AVC_MP4_BL_CIF30_AC3"	,				0	}	,
	{"AVC_MP4_BL_CIF30_AAC_LTP"	,			0	}	,
	{"AVC_MP4_BL_CIF30_AAC_LTP_MULT5" ,		0	}	,
	  //
	{"AVC_MP4_BL_L2_CIF30_AAC" ,			0	}	,
	{"AVC_MP4_BL_CIF30_BSAC" ,				0	}	,
	{"AVC_MP4_BL_CIF30_BSAC_MULT5" ,		0	}	,
	{"AVC_MP4_BL_CIF15_HEAAC" ,				0	}	,
	{"AVC_MP4_BL_CIF15_AMR"	,				0	}	,
	  //
	{"AVC_TS_MP_HD_AAC_MULT5" ,				0	}	,
	{"AVC_TS_MP_HD_AAC_MULT5_T" ,			0	}	,
	{"AVC_TS_MP_HD_AAC_MULT5_ISO" ,			0	}	,
	{"AVC_TS_MP_HD_HEAAC_L2" ,				0	}	,
	  //
	{"AVC_TS_MP_HD_HEAAC_L2_T" ,			0	}	,
	{"AVC_TS_MP_HD_HEAAC_L2_ISO" ,			0	}	,
	{"AVC_TS_MP_HD_MPEG1_L3"	,			0	}	,
	{"AVC_TS_MP_HD_MPEG1_L3_T"	,			0	}	,
	{"AVC_TS_MP_HD_MPEG1_L3_ISO" ,			0	}	,
	  //
	{"AVC_TS_MP_HD_AC3"	,					0	}	,
	{"AVC_TS_MP_HD_AC3_T"	,				0	}	,
	{"AVC_TS_MP_HD_AC3_ISO"	,				0	}	,
	{"AVC_TS_MP_HD_AAC"	,					0	}	,
	{"AVC_TS_MP_HD_AAC_T"	,				0	}	,
	  //
	{"AVC_TS_MP_HD_AAC_ISO"	,				0	}	,
	{"AVC_TS_MP_HD_AAC_LTP"	,				0	}	,
	{"AVC_TS_MP_HD_AAC_LTP_T" ,				0	}	,
	{"AVC_TS_MP_HD_AAC_LTP_ISO" ,			0	}	,
	{"AVC_TS_MP_HD_AAC_LTP_MULT5" ,			0	}	,
	  //
	{"AVC_TS_MP_HD_AAC_LTP_MULT5_T" ,		0	}	,
	{"AVC_TS_MP_HD_AAC_LTP_MULT5_ISO" ,		0	}	,
	{"AVC_TS_MP_HD_AAC_LTP_MULT7" ,			0	}	,
	{"AVC_TS_MP_HD_AAC_LTP_MULT7_T" ,		0	}	,
	{"AVC_TS_MP_HD_AAC_LTP_MULT7_ISO" ,		0	}	,
	  //
	{"AVC_TS_BL_CIF15_AAC"	,				0	}	,
	{"AVC_TS_BL_CIF15_AAC_T" ,				0	}	,
	{"AVC_TS_BL_CIF15_AAC_ISO" ,			0	}	,
	  //
	{"AVC_TS_BL_CIF15_AAC_540" ,			0	}	,
	{"AVC_TS_BL_CIF15_AAC_540_T" ,			0	}	,
	{"AVC_TS_BL_CIF15_AAC_540_ISO" ,		0	}	,
	{"AVC_TS_BL_CIF15_AAC_LTP" ,			0	}	,
	  //
	{"AVC_TS_BL_CIF15_AAC_LTP_T" ,			0	}	,
	{"AVC_TS_BL_CIF15_AAC_LTP_ISO" ,		0	}	,
	{"AVC_TS_BL_CIF15_BSAC"	,				0	}	,
	{"AVC_TS_BL_CIF15_BSAC_T" ,				0	}	,
	  //
	{"AVC_TS_BL_CIF15_BSAC_ISO" ,			0	}	,
	{"AVC_MP4_BL_CIF15_AAC"	,				0	}	,
	{"AVC_MP4_BL_CIF15_AAC_520"	,			0	}	,
	  //
	{"AVC_MP4_BL_CIF15_AAC_LTP" ,			0	}	,
	{"AVC_MP4_BL_CIF15_AAC_LTP_520" ,		0	}	,
	{"AVC_MP4_BL_CIF15_BSAC"	,			0	}	,
	{"AVC_MP4_BL_L12_CIF15_HEAAC" ,			0	}	,
	  //
	{"AVC_MP4_BL_L1B_QCIF15_HEAAC" ,		0	}	,
	{"AVC_3GPP_BL_CIF30_AMR_WBplus" ,		0	}	,
	{"AVC_3GPP_BL_CIF15_AMR_WBplus" ,		0	}	,
	{"AVC_3GPP_BL_QCIF15_AAC"	,			0	}	,
	{"AVC_3GPP_BL_QCIF15_AAC_LTP" ,			0	}	,
	  //
	{"AVC_3GPP_BL_QCIF15_HEAAC"	,			0	}	,
	{"AVC_3GPP_BL_QCIF15_AMR_WBplus" ,		0	}	,
	{"AVC_3GPP_BL_QCIF15_AMR" ,				0	}	,
	//-----------------------------------------------------------------------------------------//

	 //-----------------------------------------------------------------------------------------//
	// MKV
	//-----------------------------------------------------------------------------------------//
	{"MKV_MP_HD"			,		0	}	,
	{"MKV_HP_HD"			,		0	}	,
	{"MKV_HD_ACC_MULT5490"	,		0	}	,
	{"MKV_HD_HEAAC_L4490"	,		0	}	,
	{"MKV_HD_AC3"			,		0	}	,
	{"MKV_HD_MPEG1_L3491"	,		0	}	,
	
	//-----------------------------------------------------------------------------------------//
	//WMV 9
	//-----------------------------------------------------------------------------------------//
	{"WMVMED_BASE"	,		0	}	,
	{"WMVMED_FULL"	,		0	}	,
	{"WMVMED_PRO"	,		0	}	,
	{"WMVHIGH_FULL"	,		0	}	,
	{"WMVHIGH_PRO"	,		0	}	,
	{"WMVHM_BASE"	,		0	}	,
	{"WMVSPLL_BASE"	,		0	}	,
	{"WMVSPML_BASE"	,		0	}	,
	{"WMVSPML_MP3"	,		0	}	,
	//-----------------------------------------------------------------------------------------//



	{ UNKNOWN_STR,			0	}	,
    {  0,					0	}	,

};

#define ________________Internal_Functions_________________________ 
/* mxRes_init() 에 사용될 Resouce 관련 Callback */
MXRES_PRE_TABLE *__CbGetPreCmdTable( MX_INT32 media_type )
{
	MXRES_PRE_TABLE * retCode = 0;

	switch ( media_type )
	{
		case MF_AUDIO :		retCode = g_preAudioCmdTable;	break;
		case MF_VIDEO :		retCode = g_preVideoCmdTable;	break;
		case MF_IMAGE :		retCode = g_preImageCmdTable;	break;
		default :
							break;
	}

	return retCode;
}

/* mxRes_init() 에 사용될 Resouce 관련 Callback */
MXPN_PRI_MAP *__CbGetProfilePriTable( MX_INT32 media_type )
{
	MXPN_PRI_MAP * retValue = 0;

	switch ( media_type )
	{
		case MF_IMAGE :		retValue = PNPRI_IMAGE_MAP;		break;
		case MF_AUDIO :		retValue = PNPRI_AUDIO_MAP;		break;
		case MF_VIDEO :		retValue = PNPRI_VIDEO_MAP;		break;
	}

	return retValue;
}

/* mxRes_init() 에 사용될 Resouce 관련 Callback */
MX_INT32 __CbIsAvailablePlay( MX_INT32 format, char *mime )
{
	switch ( format )
	{
		case MF_IMAGE_JPG:
		case MF_IMAGE_BMP:
			//case MF_IMAGE_GIF:
		case MF_IMAGE_PNG:

		case MF_AUDIO_MP3:
		case MF_AUDIO_WMA:
		case MF_AUDIO_WAV:
		case MF_AUDIO_AAC:

		case MF_VIDEO_WMV:
		case MF_VIDEO_MPG:
		case MF_VIDEO_AVI:
		case MF_VIDEO_MP4:

			return 1;
	}
	return 0;
}

#define ________________Public_Functions___________________________

void	PL_DLNA_ResourceTableInit(void)
{
	MXRES_CALLBACK fnMxResCallback = { 0, };
	/* 아래 Callback들은 변경시 문제가 생길수 있으므로, 변경 Interface를 제공하지 않고 그대로 사용함 */
	fnMxResCallback.fn_GetPreCmdTable       = __CbGetPreCmdTable;
	fnMxResCallback.fn_GetProfilePriTable   = __CbGetProfilePriTable;
	fnMxResCallback.fnIsAvailablePlay       = __CbIsAvailablePlay;

	mxRes_init( &fnMxResCallback ); // Resource Init.
}

/*
 * (c) 2011-2012 Humax Co., Ltd.
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
/*******************************************************************
	File Description
********************************************************************/
/**
	@file	pal_output.c
	@brief	pal_output.c

	Description: video, audio 등의 output에 대한 정의 및 처리.		\n
	Module: PAL /OUTPUT												\n

	Copyright (c) 2009 HUMAX Co., Ltd.								\n
	All rights reserved.											\n
*/

/*******************************************************************/
/********************      Header Files    *************************/
/*******************************************************************/
#include <di_video.h>
#include <di_audio.h>
#include <di_scart.h>
#include <di_hdmi.h>
#include <di_osd.h>
#include <di_rf.h>
#include <di_nvram.h>

#include <hlib.h>
#include <get_enum_str.h>

#include <pal_hdmi.h>
//#include <pal_scart.h>
#include <pal_output.h>

//#include <db_param.h>

/*******************************************************************/
/********************      Global Variables     ********************/
/*******************************************************************/


/*******************************************************************/
/********************      Defines         *************************/
/*******************************************************************/


/*******************************************************************/
/********************      Typedef         *************************/
/*******************************************************************/

/*******************************************************************/
/********************      Static Variables     ********************/
/*******************************************************************/
STATIC HULONG						s_ulPalOutSemId;

STATIC HBOOL						s_bStbOutputEnable; 			/* AV 전체 Output State */

STATIC HBOOL						s_bMasterAudioMute; 				/* User Mute : Mute Button. */
STATIC HBOOL						s_bHdcpAudioMute; 				/* User Mute : Mute Button. */
STATIC HBOOL						s_bDecoderAudioMute;			/* Decoding 상태에 따라 mute를 거는 경우 */
STATIC HBOOL						s_bBluetoothAudioMute;			/* bluetooth audio 연결시 mute */

/* STB Output Setting */
STATIC DxAudioCodec_e				s_eStreamAudioCodec;
STATIC DxDigitalAudioFormat_e		s_eHdmiAudioFormat; 				/* HDMI Audio Output setting */
STATIC DxAudioCodec_e				s_eHdmiTransCodec[eDxAUDIO_CODEC_MAX_NUM]; /* 각 source codec 별 HDMI transcodec table */
STATIC DxDigitalAudioFormat_e		s_eSpdifAudioFormat; 				/* SPDIF Audio Output setting */
STATIC DxAudioCodec_e				s_eSpdifTransCodec[eDxAUDIO_CODEC_MAX_NUM]; /* 각 source codec 별 SPDIF transcodec table */

/* Actual Audio Output Codec */
STATIC DxAudioCodec_e				s_eHdmiAudioCodec;
STATIC DxAudioCodec_e				s_eSpdifAudioCodec;

/* Video blank setting */
STATIC Macrovision_t				s_eVideoMacrovision= -1;		/* APS : Macrovision */
STATIC Macrovision_t				s_eCgmsMacrovision= -1;			/* APS : Macrovision */
STATIC CopyRight_t					s_eCsmsCopyRight = -1;			/* SCMS Status */
STATIC CopyRight_t 					s_eVideoBlankCopyRight = -1;	/* WSS/CGMS Copy right */
STATIC VIDEO_Wss_t					s_eVideoBlankWss = -1;			/* Wss aspect ratio */

/* Volume setting */
STATIC HUINT32 						s_ulVolume;					/* Audio Volume */
STATIC HUINT32 						s_ulVolumeRange;

STATIC VideoBlankSpec_e				s_eVideoBlankSpec;

/* Current DI Status */
STATIC HBOOL						s_bDiAudioMute = -1;
STATIC DI_VIDEO_AOUT_MODE_t			s_stDiAnalogVideoOut; 		/* 현재 di에 설정된 analog video */

STATIC DI_VIDEO_WSS_SETTINGS_t		s_stDiWss;
STATIC DI_VIDEO_CGMS_SETTINGS		s_stDiCgms;

//#define DEBUG_SEMA
#ifdef DEBUG_SEMA
STATIC HUINT8						s_aucSemGetFunc[64];
#define	ENTER_CRITICAL_PALOUT		{VK_SEM_Get(s_ulPalOutSemId);\
										strcpy(&s_aucSemGetFunc, __FUNCTION__);}
#else
#define	ENTER_CRITICAL_PALOUT		VK_SEM_Get(s_ulPalOutSemId)
#endif

#define	LEAVE_CRITICAL_PALOUT		VK_SEM_Release(s_ulPalOutSemId)




/*******************************************************************/
/********************      Functions       *************************/
/*******************************************************************/
/*
1. output list

STB
- SPDIF
- HDMI
- SCART 1 (TV)
- SCART 2 (VCR)
- COMPOSITE
- COMPONENT

2. output 단위 requirement.

AUDIO/VIDEO
- output enable/disable.

AUDIO
- volume control.
- user mute on/off.

DIGITAL AUDIO
- output format 설정 (bit stream out, pcm)
- transcoding (dolby, dts)

ANALOG VIDEO
- macrovision/cgms-a control

SPDIF
- scms control
- output enable/disable

SCART
- output format (cvbs, s-video, rgb)
- output enable/disable

COMPONENT
- output enable/disable

*/

STATIC void pal_out_SetDiCgms(void)
{
	HxLOG_Debug("\t%s(for %s)\n", __FUNCTION__, (s_eVideoBlankSpec == eVIDEO_BLANK_SPEC_CGMS_JAPAN) ? "JAPAN" : "EIA");

	DI_VIDEO_SetCGMS(TRUE, s_stDiCgms, DI_VIDEO_WIN_SD);
}

STATIC HERROR pal_out_SetCgmsCopyRight(CopyRight_t eCopyRight)
{
	DI_VIDEO_CGMS_A				eDiCgmsA;

	if( (s_eVideoBlankSpec != eVIDEO_BLANK_SPEC_CGMS)
		&& (s_eVideoBlankSpec != eVIDEO_BLANK_SPEC_CGMS_JAPAN)
	)
	{
		return ERR_FAIL;
	}

	HxLOG_Debug("\t%s(%s)\n", __FUNCTION__, OTL_ENUM2STR_CopyRight(eCopyRight));

	switch(eCopyRight)
	{
		case eCOPYRIGHT_COPY_FREE:
			eDiCgmsA = DI_VIDEO_CGMS_COPY_NO_RSTRICT;
			break;

		case eCOPYRIGHT_COPY_ONCE:
			eDiCgmsA = DI_VIDEO_CGMS_ONE_GEN_COPY;
			break;

		case eCOPYRIGHT_COPY_NOMORE:
			eDiCgmsA = DI_VIDEO_CGMS_NO_CONDITION;
			break;

		case eCOPYRIGHT_COPY_NEVER:
			eDiCgmsA = DI_VIDEO_CGMS_NO_COPY;
			break;

		default:
			HxLOG_Critical("\n\n");
			return ERR_FAIL;
	}

	if(s_eVideoBlankSpec == eVIDEO_BLANK_SPEC_CGMS)
	{
		s_stDiCgms.uTypeStatus.tEiaCea805.eCgmsA= eDiCgmsA;
	}
	else if(s_eVideoBlankSpec == eVIDEO_BLANK_SPEC_CGMS_JAPAN)
	{
		s_stDiCgms.uTypeStatus.tArib.eCgmsA = eDiCgmsA;
	}

	pal_out_SetDiCgms();

	return ERR_OK;
}

STATIC HERROR pal_out_SetCgmsAspectRatio(VIDEO_Wss_t eWss)
{
	DI_VIDEO_AspectRatio_t		eDiCgmsAspectRatio;

	if(s_eVideoBlankSpec != eVIDEO_BLANK_SPEC_CGMS_JAPAN)
	{
		return ERR_FAIL;
	}

	HxLOG_Debug("\t%s(%s)\n", __FUNCTION__, OTL_ENUM2STR_Wss(eWss));

	switch(eWss)
	{
		case eWss_43_Full:
		case eWss_43_LB149_Center:
		case eWss_43_SnP149:
			eDiCgmsAspectRatio = DI_VIDEO_ASPECTRATIO_4_3;
			break;

		case eWss_43_LB169_Center:
			eDiCgmsAspectRatio = DI_VIDEO_ASPECTRATIO_4_3_LETTER;
			break;

		case eWss_169_Full:
			eDiCgmsAspectRatio = DI_VIDEO_ASPECTRATIO_16_9;
			break;

		default:
			HxLOG_Error("\t\tERROR \n");
			return ERR_FAIL;
	}

	s_stDiCgms.uTypeStatus.tArib.eAspectRatio = eDiCgmsAspectRatio;

	pal_out_SetDiCgms();

	return ERR_OK;
}

STATIC HERROR pal_out_SetWssCopyRight(CopyRight_t eCopyRight)
{
	DI_ERR_CODE		diErr;
/*
0	0	CopyFreely	Unlimited copies may be made of the content.
0	1	CopyNoMore* One generation of copies has already been made; no further copying is allowed.
1	0	CopyOnce	One generation of copies may be made
1	1	CopyNever	No copies may be made of the content.
*/

/**
  *		WSS CGMS-A (en_300294v010401p.pdf, 9p)
  *
  *		12 bit - 0=no copy right, 1=copy right
  *		13 bit - 0=copy not restrict, 1=copy restrict
  *
  *		DVB CPCM (ETSI TS 102 825-10, 14p)
  *		0  0  - Copy control not asserted
  *		0  1  - Copy No More
  *		1  0  - Copy Once
  *		1  1  - Copy Never
  *
**/

	if(s_eVideoBlankSpec != eVIDEO_BLANK_SPEC_WSS)
	{
		return ERR_FAIL;
	}

	HxLOG_Debug("\t%s(%s)\n", __FUNCTION__, OTL_ENUM2STR_CopyRight(eCopyRight));

	switch (eCopyRight)
	{
		case eCOPYRIGHT_COPY_FREE:
			s_stDiWss.eCpbit = 0;
			s_stDiWss.eGebit = 0;
			break;

		case eCOPYRIGHT_COPY_NOMORE:
			s_stDiWss.eCpbit = 0;
			s_stDiWss.eGebit = 1;
			break;

		case eCOPYRIGHT_COPY_ONCE:
			s_stDiWss.eCpbit = 1;
			s_stDiWss.eGebit = 0;
			break;

		case eCOPYRIGHT_COPY_NEVER:
			s_stDiWss.eCpbit = 1;
			s_stDiWss.eGebit = 1;
			break;

		default:
			s_stDiWss.eCpbit = 1;
			s_stDiWss.eGebit = 1;
			HxLOG_Error("\t\tERROR Invalid eCopyRight :0x%X\n",eCopyRight);
			break;
	}

	HxLOG_Debug("\t\tstSet.cpBit (%d) Ge(%d) \n", s_stDiWss.eCpbit, s_stDiWss.eGebit);

	diErr = DI_VIDEO_SetWSS(TRUE, s_stDiWss);
	if(diErr != ERR_OK)
	{
		HxLOG_Error("\t\tERROR DI_VIDEO_SetWSS ret:0x%X\n", diErr);
	}

	return ERR_OK;

}

STATIC HERROR pal_out_SetWssAspectRatio(VIDEO_Wss_t eWss)
{
	DI_ERR_CODE						diErr;
	DI_VIDEO_WSS_AFD				eDiWssAfd;

	if(s_eVideoBlankSpec != eVIDEO_BLANK_SPEC_WSS)
	{
		return ERR_OK;
	}

	HxLOG_Debug("\t%s(%s)\n", __FUNCTION__, OTL_ENUM2STR_Wss(eWss));

	switch(eWss)
	{
		case eWss_43_Full:
			eDiWssAfd = DI_VIDEO_WSS_AFD_FULL_FORMAT_4_3;
			break;

		case eWss_43_LB149_Center:
			eDiWssAfd = DI_VIDEO_WSS_AFD_BOX_14_9_CENTRE;
			break;

		case eWss_43_LB169_Center:
			eDiWssAfd = DI_VIDEO_WSS_AFD_BOX_16_9_CENTRE;
			break;

		case eWss_43_SnP149:
			eDiWssAfd = DI_VIDEO_WSS_AFD_FULL_FORMAT_4_3_SNP;
			break;

		case eWss_169_Full:
			eDiWssAfd = DI_VIDEO_WSS_AFD_FULL_FORMAT_16_9_ANAMORPHIC;
			break;

		default:
			return ERR_FAIL;
	}

	HxLOG_Debug("\t\tChange DI SCART WSS AFD (%d) -> (%d)\n", s_stDiWss.eAfd, eDiWssAfd);

	s_stDiWss.eAfd = eDiWssAfd;

	diErr = DI_VIDEO_SetWSS(TRUE, s_stDiWss);
	if(diErr != ERR_OK)
	{
		HxLOG_Error("\t\tERROR DI_VIDEO_SetWSS ret:0x%X\n", diErr);
	}

	return ERR_OK;
}

STATIC HUINT32 pal_out_CheckComposite(void)
{
	DI_ERR_CODE		diErr;

	HxLOG_Debug("\t%s()\n", __FUNCTION__);

	if( (s_bStbOutputEnable == TRUE) && (s_stDiAnalogVideoOut.bCVBS == 0) )
	{ /* 항상 둘다 enable 해 놓고 switch로 선택 하는 것이 DI의 요구 사항임. */
		s_stDiAnalogVideoOut.bCVBS = 1;
		s_stDiAnalogVideoOut.bRGB = 1;

		diErr = DI_VIDEO_SetOutputSettings(s_stDiAnalogVideoOut);
		if(diErr != DI_ERR_OK)
		{
			HxLOG_Error("\t\tERROR Critical Eror!!!\n");
		}

#if defined(CONFIG_MW_OUTPUT_SCART)
		PAL_SCART_SetOutput(TRUE);
#endif
	}
	else if( (s_bStbOutputEnable == FALSE) && (s_stDiAnalogVideoOut.bCVBS == 1) )
	{
		s_stDiAnalogVideoOut.bCVBS = 0;
		s_stDiAnalogVideoOut.bRGB = 0;

		diErr = DI_VIDEO_SetOutputSettings(s_stDiAnalogVideoOut);
		if(diErr != DI_ERR_OK)
		{
			HxLOG_Error("\t\tERROR Critical Eror!!!\n");
		}

#if defined(CONFIG_MW_OUTPUT_SCART)
		PAL_SCART_SetOutput(FALSE);
#endif
	}

	return ERR_OK;
}


/* AUDIO */
STATIC HUINT32 pal_out_CheckAudioMute(void)
{
	HBOOL	bDiAudioMute;

	HxLOG_Debug("\t%s()\n", __FUNCTION__);

	if (s_bStbOutputEnable == FALSE)
	{
		bDiAudioMute = TRUE;
		HxLOG_Debug("\t\tSTB Output Disabled.\n");
	}
	else
	{
	 	if( (s_bMasterAudioMute == TRUE)
			|| (s_bHdcpAudioMute == TRUE)
			|| (s_bDecoderAudioMute == TRUE)
			|| (s_bBluetoothAudioMute == TRUE)
		)
	 	{
			bDiAudioMute = TRUE;
			HxLOG_Debug("\t\tAudio Mute (User:%d, Decoder:%d, Bluetooth:%d).\n"
									, s_bMasterAudioMute
									, s_bDecoderAudioMute
									, s_bBluetoothAudioMute
									);
	 	}
		else
		{
			HxLOG_Debug("\t\tNOT in Mute State.\n");
			bDiAudioMute = FALSE;
		}
	}

	if(bDiAudioMute != s_bDiAudioMute)
	{
		HxLOG_Debug("\t\tSet DI Mute : %d.\n", bDiAudioMute);

		DI_AUDIO_SetMute(DI_AUDIO_OUTPUT_DAC, bDiAudioMute);
		DI_AUDIO_SetMute(DI_AUDIO_OUTPUT_HDMI, bDiAudioMute);
		DI_AUDIO_SetMute(DI_AUDIO_OUTPUT_SPDIF, bDiAudioMute);

		s_bDiAudioMute = bDiAudioMute;
	}

	return ERR_OK;
}

STATIC void pal_out_ConvertAudioTransCodecPal2Di(DxAudioCodec_e eAudioTransCodec, DI_AUDIO_FORMAT *eDiCodec)
{
	switch(eAudioTransCodec)
	{
		case eDxAUDIO_CODEC_DOLBY_AC3:
			*eDiCodec = DI_AUDFORMAT_AC3;
			break;
		case eDxAUDIO_CODEC_DOLBY_AC3P:
			*eDiCodec = DI_AUDFORMAT_AC3_PLUS;
			break;
		case eDxAUDIO_CODEC_AAC:
			*eDiCodec = DI_AUDFORMAT_AAC;
			break;
		case eDxAUDIO_CODEC_AAC_LOAS:
			*eDiCodec = DI_AUDFORMAT_AAC_LOAS;
			break;
		case eDxAUDIO_CODEC_AAC_PLUS:
			*eDiCodec = DI_AUDFORMAT_AAC_PLUS;
			break;
		case eDxAUDIO_CODEC_AAC_PLUS_ADTS:
			*eDiCodec = DI_AUDFORMAT_AAC_PLUS_ADTS;
			break;
		case eDxAUDIO_CODEC_DTS:
			*eDiCodec = DI_AUDFORMAT_DTS;
			break;
		case eDxAUDIO_CODEC_DTS_HD:
			*eDiCodec = DI_AUDFORMAT_DTS_HD;
			break;
		default:
			HxLOG_Debug("\t\tnot supported codec (%s) \n", OTL_ENUM2STR_AudioCodec(eAudioTransCodec));
			*eDiCodec = DI_AUDFORMAT_NONE;
			break;
	}

	return;
}

HERROR pal_out_CheckHdmiAudioOut(void)
{
	DI_ERR_CODE				diErr;
	DI_AUDIO_HDMI_MODE		eDiHdmiAudioMode;
	DI_AUDIO_FORMAT			eDiHdmiTransCode = DI_AUDFORMAT_NONE;

	HxLOG_Debug("\t%s()\n", __FUNCTION__);
	HxLOG_Debug("\t\tHDMI Audio Out Format : %s, Stream Codec : %s\n"
								, OTL_ENUM2STR_DigitalAudioOutput(s_eHdmiAudioFormat)
								, OTL_ENUM2STR_AudioCodec(s_eStreamAudioCodec)
								);

	if(s_eHdmiAudioFormat == eDxDIGITALAUDIO_FORMAT_MULTICHANNEL)
	{
		if(PAL_HDMI_CheckAudioCodecAvailable(s_eStreamAudioCodec) == ERR_OK)
		{
			HxLOG_Debug("\t\tBypass stream codec %s\n", OTL_ENUM2STR_AudioCodec(s_eStreamAudioCodec));
			s_eHdmiAudioCodec = s_eStreamAudioCodec;

			eDiHdmiAudioMode = DI_AUDOUT_HDMI_BYPASS;
		}
		else if(PAL_HDMI_CheckAudioCodecAvailable(s_eHdmiTransCodec[s_eStreamAudioCodec]) == ERR_OK)
		{
			HxLOG_Debug("\t\tTranscode stream codec %s to %s\n"
				, OTL_ENUM2STR_AudioCodec(s_eStreamAudioCodec)
				, OTL_ENUM2STR_AudioCodec(s_eHdmiTransCodec[s_eStreamAudioCodec])
				);

			s_eHdmiAudioCodec = s_eHdmiTransCodec[s_eStreamAudioCodec];

			eDiHdmiAudioMode = DI_AUDOUT_HDMI_TRANSCODING;
			pal_out_ConvertAudioTransCodecPal2Di(s_eHdmiTransCodec[s_eStreamAudioCodec], &eDiHdmiTransCode);
		}
		else
		{
			HxLOG_Debug("\t\tForce Down Mix\n");
			s_eHdmiAudioCodec = eDxAUDIO_CODEC_PCM;

			eDiHdmiAudioMode = DI_AUDOUT_HDMI_DOWNMIX;
		}
	}
	else
	{
		HxLOG_Debug("\t\tDefault Down Mix\n");
		s_eHdmiAudioCodec = eDxAUDIO_CODEC_PCM;
		eDiHdmiAudioMode = DI_AUDOUT_HDMI_DOWNMIX;
	}

	HxLOG_Debug("\t\tDI HDMI trans code : %d, DI HDMI audio out : %d\n", eDiHdmiTransCode, eDiHdmiAudioMode);

	diErr = DI_AUDIO_SetTranscoding(DI_AUDIO_OUTPUT_HDMI, (eDiHdmiAudioMode == DI_AUDOUT_HDMI_TRANSCODING), eDiHdmiTransCode);
	if(diErr != DI_ERR_OK)
	{
		HxLOG_Debug("ERROR from DI_AUDIO_SetTranscoding(). Force Down Mix\n");
		s_eHdmiAudioCodec = eDxAUDIO_CODEC_PCM;
		eDiHdmiAudioMode = DI_AUDOUT_HDMI_DOWNMIX;
	}

	diErr = DI_AUDIO_SetHdmiOutput(eDiHdmiAudioMode);
	if(diErr != ERR_OK)
	{
		HxLOG_Error("ERROR from DI_AUDIO_SetHdmiOutput().\n");
	}

	return ERR_OK;
}

HERROR pal_out_CheckSpdifAudioOut(void)
{
	DI_ERR_CODE				diErr;
	DI_AUDIO_SPDIF_MODE		eDiSpdifAudioMode;
	DI_AUDIO_FORMAT 		eDiSpdifTransCode = DI_AUDFORMAT_NONE;

	HxLOG_Debug("\t%s()\n", __FUNCTION__);
	HxLOG_Debug("\t\tSPDIF Audio Out Format : %s, Stream Codec : %s\n"
								, OTL_ENUM2STR_DigitalAudioOutput(s_eSpdifAudioFormat)
								, OTL_ENUM2STR_AudioCodec(s_eStreamAudioCodec)
								);

	if(s_eSpdifAudioFormat == eDxDIGITALAUDIO_FORMAT_MULTICHANNEL)
	{
		if(s_eStreamAudioCodec == s_eSpdifTransCodec[s_eStreamAudioCodec])
		{
			HxLOG_Debug("\t\t\tSelect SPDIF BYPASS\n");
			s_eSpdifAudioCodec = s_eStreamAudioCodec;
			eDiSpdifAudioMode = DI_AUDOUT_SPDIF_BYPASS;
		}
		else if(s_eSpdifTransCodec[s_eStreamAudioCodec] != eDxAUDIO_CODEC_UNKNOWN)
		{
			HxLOG_Debug("\t\tTranscode stream codec %s to %s\n"
				, OTL_ENUM2STR_AudioCodec(s_eStreamAudioCodec)
				, OTL_ENUM2STR_AudioCodec(s_eSpdifTransCodec[s_eStreamAudioCodec])
				);

			s_eSpdifAudioCodec = s_eSpdifTransCodec[s_eStreamAudioCodec];
			eDiSpdifAudioMode = DI_AUDOUT_SPDIF_TRANSCODING;
			pal_out_ConvertAudioTransCodecPal2Di(s_eSpdifTransCodec[s_eStreamAudioCodec], &eDiSpdifTransCode);
		}
		else
		{
			HxLOG_Debug("\t\t\tTranscode NOT setup. Force SPDIF PCM\n");
			s_eSpdifAudioCodec = eDxAUDIO_CODEC_PCM;
			eDiSpdifAudioMode = DI_AUDOUT_SPDIF_DOWNMIX;
		}
	}
	else
	{
		HxLOG_Debug("\t\t\tSelect SPDIF PCM\n");
		s_eSpdifAudioCodec = eDxAUDIO_CODEC_PCM;
		eDiSpdifAudioMode = DI_AUDOUT_SPDIF_DOWNMIX;
	}

	HxLOG_Debug("\t\tDI SPDIF trans code : %d, DI SPDIF audio out : %d\n", eDiSpdifTransCode, eDiSpdifAudioMode);

	diErr = DI_AUDIO_SetTranscoding(DI_AUDIO_OUTPUT_SPDIF, (eDiSpdifAudioMode == DI_AUDOUT_SPDIF_TRANSCODING), eDiSpdifTransCode);
	if(diErr != DI_ERR_OK)
	{
		HxLOG_Debug("ERROR from DI_AUDIO_SetTranscoding(). Force Down Mix\n");
		s_eSpdifAudioCodec = eDxAUDIO_CODEC_PCM;
		eDiSpdifAudioMode = DI_AUDOUT_SPDIF_DOWNMIX;
	}

	diErr = DI_AUDIO_SetSpdifOutput(eDiSpdifAudioMode);
	if(diErr != ERR_OK)
	{
		HxLOG_Error("ERROR from DI_AUDIO_SetSpdifOutput()\n");
	}

	return ERR_OK;
}

STATIC void pal_out_PrintStatus(void)
{
	HUINT32		i;

	HLIB_CMD_Printf("===================== output status ============================ \n");
	HLIB_CMD_Printf(" bStbOutputEnable : %d \n", s_bStbOutputEnable);

	HLIB_CMD_Printf(" bUserAudioMute : %d \n", s_bMasterAudioMute);
	HLIB_CMD_Printf(" bDecoderAudioMute : %d \n", s_bDecoderAudioMute);
	HLIB_CMD_Printf(" s_bBluetoothAudioMute : %d \n", s_bBluetoothAudioMute);

	HLIB_CMD_Printf(" eStreamAudioCodec : %s \n", OTL_ENUM2STR_AudioCodec(s_eStreamAudioCodec));

	HLIB_CMD_Printf(" eHdmiAudioOut : %s \n", OTL_ENUM2STR_DigitalAudioOutput(s_eHdmiAudioFormat));
	for(i = 0; i < eDxAUDIO_CODEC_MAX_NUM; i++)
	{
		HLIB_CMD_Printf(" \teHdmiTransCodec : %s -> %s\n", OTL_ENUM2STR_AudioCodec(i), OTL_ENUM2STR_AudioCodec(s_eHdmiTransCodec[i]));
	}

	HLIB_CMD_Printf(" eSpdifAudioOut : %s \n", OTL_ENUM2STR_DigitalAudioOutput(s_eSpdifAudioFormat));
	for(i = 0; i < eDxAUDIO_CODEC_MAX_NUM; i++)
	{
		HLIB_CMD_Printf(" \teSpdifTransCodec : %s -> %s\n", OTL_ENUM2STR_AudioCodec(i), OTL_ENUM2STR_AudioCodec(s_eSpdifTransCodec[i]));
	}

	HLIB_CMD_Printf(" eHdmiAudioCodec : %s \n", OTL_ENUM2STR_AudioCodec(s_eHdmiAudioCodec));
	HLIB_CMD_Printf(" eSpdifAudioCodec : %s \n", OTL_ENUM2STR_AudioCodec(s_eSpdifAudioCodec));

	HLIB_CMD_Printf(" eMacrovision : %s \n", OTL_ENUM2STR_Macrovision(s_eVideoMacrovision));
	HLIB_CMD_Printf(" eCgmsMacrovision : %s \n", OTL_ENUM2STR_Macrovision(s_eCgmsMacrovision));
	HLIB_CMD_Printf(" eScms : %s \n", OTL_ENUM2STR_CopyRight(s_eCsmsCopyRight));
	HLIB_CMD_Printf(" s_eVideoBlankCopyRight : %s \n", OTL_ENUM2STR_CopyRight(s_eVideoBlankCopyRight));
	HLIB_CMD_Printf(" s_eVideoBlankWss : %s \n", OTL_ENUM2STR_Wss(s_eVideoBlankWss));
	HLIB_CMD_Printf(" ulVolume : %d \n", s_ulVolume);
	HLIB_CMD_Printf(" ulVolumeRange : %d \n", s_ulVolumeRange);
	HLIB_CMD_Printf("===================== end of output status ============================ \n");

	return;
}

STATIC int pal_out_ProcessCommand_status(void *szArgList)
{
	pal_out_PrintStatus();

	return HxCMD_OK;
}

STATIC int pal_out_ProcessCommand_cgmsa_cp(void *szArgList)
{
	char			*aucArg=NULL;
	char			aucStr[20];

	aucArg = HLIB_CMD_GetNextArg((HCHAR**)&szArgList);
	if( aucArg != NULL )
	{
		if( sscanf(aucArg, "%s", aucStr) != 0 )
		{
			if(strcmp(aucStr, "free") == 0)
			{
				PAL_OUT_SetVideoBlankCopyRight(eCOPYRIGHT_COPY_FREE);
			}
			else if(strcmp(aucStr, "once") == 0)
			{
				PAL_OUT_SetVideoBlankCopyRight(eCOPYRIGHT_COPY_ONCE);
			}
			else if(strcmp(aucStr, "nomore") == 0)
			{
				PAL_OUT_SetVideoBlankCopyRight(eCOPYRIGHT_COPY_NOMORE);
			}
			else if(strcmp(aucStr, "never") == 0)
			{
				PAL_OUT_SetVideoBlankCopyRight(eCOPYRIGHT_COPY_NEVER);
			}
		}
		else
		{
			return HxCMD_ERR;
		}
	}
	else
	{
		return HxCMD_ERR;
	}

	return HxCMD_OK;
}

STATIC int pal_out_ProcessCommand_scms(void *szArgList)
{
	char			*aucArg=NULL;
	char			aucStr[20];

	aucArg = HLIB_CMD_GetNextArg((HCHAR**)&szArgList);
	if( aucArg != NULL )
	{
		if( sscanf(aucArg, "%s", aucStr) != 0 )
		{
			if(strcmp(aucStr, "free") == 0)
			{
				PAL_OUT_SetScmsCopyRight(eCOPYRIGHT_COPY_FREE);
			}
			else if(strcmp(aucStr, "once") == 0)
			{
				PAL_OUT_SetScmsCopyRight(eCOPYRIGHT_COPY_ONCE);
			}
			else if(strcmp(aucStr, "nomore") == 0)
			{
				PAL_OUT_SetScmsCopyRight(eCOPYRIGHT_COPY_NOMORE);
			}
			else if(strcmp(aucStr, "never") == 0)
			{
				PAL_OUT_SetScmsCopyRight(eCOPYRIGHT_COPY_NEVER);
			}
		}
		else
		{
			return HxCMD_ERR;
		}
	}
	else
	{
		return HxCMD_ERR;
	}

	return HxCMD_OK;
}

STATIC int pal_out_ProcessCommand_macrovision(void *szArgList)
{
	char			*aucArg=NULL;
	char			aucStr[20];

	aucArg = HLIB_CMD_GetNextArg((HCHAR**)&szArgList);
	if( aucArg != NULL )
	{
		if( sscanf(aucArg, "%s", aucStr) != 0 )
		{
			if(strcmp(aucStr, "off") == 0)
			{
				PAL_OUT_SetMacrovision(eMACROVISION_OFF);
			}
			else if(strcmp(aucStr, "agc") == 0)
			{
				PAL_OUT_SetMacrovision(eMACROVISION_AGC);
			}
			else if(strcmp(aucStr, "2line") == 0)
			{
				PAL_OUT_SetMacrovision(eMACROVISION_2LINE);
			}
			else if(strcmp(aucStr, "4line") == 0)
			{
				PAL_OUT_SetMacrovision(eMACROVISION_4LINE);
			}
		}
	}
	else
	{
		return HxCMD_ERR;
	}

	return HxCMD_OK;
}

STATIC int pal_out_ProcessCommand_cgmsa_ar(void *szArgList)
{
	char			*aucArg=NULL;
	char			aucStr[20];
	HERROR		err;

	aucArg = HLIB_CMD_GetNextArg((HCHAR**)&szArgList);
	if( aucArg != NULL )
	{
		if( sscanf(aucArg, "%s", aucStr) != 0 )
		{
			if(strcmp(aucStr, "169") == 0)
			{
				err = pal_out_SetCgmsAspectRatio(eWss_43_Full);
			}
			else if(strcmp(aucStr, "43") == 0)
			{
				err = pal_out_SetCgmsAspectRatio(eWss_169_Full);
			}
			else if(strcmp(aucStr, "43LB") == 0)
			{
				err = pal_out_SetCgmsAspectRatio(eWss_43_LB169_Center);
			}
			else
			{
				return HxCMD_ERR;
			}

			if(err != ERR_OK)
			{
				HLIB_CMD_Printf("!!!!!!!!!!!!!!! ERROR from pal_out_SetCgmsAspectRatio()\n");
			}
		}
	}
	else
	{
		return HxCMD_ERR;
	}

	return HxCMD_OK;
}

STATIC int pal_out_ProcessCommand_ping(void *szArgList)
{
	HLIB_CMD_Printf("Ping pal_out API semaphore ...\n");

	ENTER_CRITICAL_PALOUT;

	HLIB_CMD_Printf("\t\ts_ulPalOutSemId alive ...\n");

	LEAVE_CRITICAL_PALOUT;

	return HxCMD_OK;
}

STATIC int pal_out_ProcessCommand_show_sema4lock(void *szArgList)
{
#ifdef DEBUG_SEMA
	HLIB_CMD_Printf("Last sema get function : %s\n", s_aucSemGetFunc);
#else
	HLIB_CMD_Printf("Need to define DEBUG_SEMA first !!!\n");
#endif

	return HxCMD_OK;
}

#define	hCmdHandle		"pal_output"

STATIC void pal_out_RegisterCommand(void)
{
	HLIB_CMD_RegisterWord(hCmdHandle,
						pal_out_ProcessCommand_status,
						"status",
						"Print output status",
						"status");

	HLIB_CMD_RegisterWord(hCmdHandle,
						pal_out_ProcessCommand_cgmsa_cp,
						"cgmsa_cp",
						"Set video blank CGMS_A copy right",
						"cgmsa_cp [free|once|nomore|never]");

	HLIB_CMD_RegisterWord(hCmdHandle,
						pal_out_ProcessCommand_scms,
						"scms",
						"Set digital audio copy right",
						"scms [free|once|nomore|never]");

	HLIB_CMD_RegisterWord(hCmdHandle,
						pal_out_ProcessCommand_macrovision,
						"macrovision",
						"Set video macrovision.",
						"macrovision [off|agc|2line|4line]");

	HLIB_CMD_RegisterWord(hCmdHandle,
						pal_out_ProcessCommand_cgmsa_ar,
						"cgmsa_ar",
						"Set CGMS_A aprect ratio.",
						"cgmsa_ar [43|43lb|169]");

	HLIB_CMD_RegisterWord(hCmdHandle,
						pal_out_ProcessCommand_ping,
						"ping",
						"Probe if semaphore is alive.",
						"ping");

	HLIB_CMD_RegisterWord(hCmdHandle,
						pal_out_ProcessCommand_show_sema4lock,
						"show_sema4lock",
						"Show semaphore locked function.",
						"show_sema4lock");
}

#if 0
HERROR PAL_OUT_Init(void)
{
	HUINT32				nResult = 0;
	DI_AUDIO_CAP_t		*pInfo = (DI_AUDIO_CAP_t *)NULL;
	HUINT32				ulNumDec;

	HxLOG_Debug("%s()\n", __FUNCTION__);

	nResult = VK_SEM_Create(&s_ulPalOutSemId, "PalOut", VK_SUSPENDTYPE_PRIORITY);
	if(nResult != ERR_OK)
	{
		HxLOG_Error("ERROR VK_SEM_Create ret:0x%X\n",nResult);
	}

	nResult = DI_AUDIO_GetCapability(&ulNumDec, &pInfo);
	if(nResult == DI_ERR_OK)
	{
		s_ulVolumeRange = pInfo->volume_max;
	}
	else
	{
		s_ulVolumeRange = 0;
	}

	pal_out_RegisterCommand();

	return ERR_OK;
}
#endif

HERROR PAL_OUT_SetInitParameters(
							HBOOL						bVbiTeletextEnable,
							VideoBlankSpec_e			eVideoBlankSpec,
							DxDigitalAudioFormat_e		eHdmiAudioOutFormat,
							DxDigitalAudioFormat_e		eSpdifAudioOutFormat
							)
{
	printf("%s()\n", __FUNCTION__);
	printf("\tbVbiTeletextEnable : %d\n", bVbiTeletextEnable);
	printf("\teVideoBlankSpec : %s\n", OTL_ENUM2STR_VideoBlankSpec(eVideoBlankSpec));
	printf("\tHDMI audio out : %s\n", OTL_ENUM2STR_DigitalAudioOutput(eHdmiAudioOutFormat));
	printf("\tSPDIF audio out : %s\n", OTL_ENUM2STR_DigitalAudioOutput(eSpdifAudioOutFormat));

	if(bVbiTeletextEnable == TRUE)
	{
		DI_VIDEO_SetVBITeletext(TRUE);
	}

	s_eVideoBlankSpec = eVideoBlankSpec;

	if(eVideoBlankSpec == eVIDEO_BLANK_SPEC_CGMS)
	{
		s_stDiCgms.eType = DI_VIDEO_CGMS_EIA_CEA_805;
	}
	else if(eVideoBlankSpec == eVIDEO_BLANK_SPEC_CGMS_JAPAN)
	{
		s_stDiCgms.eType = DI_VIDEO_CGMS_ARIB;
	}

	s_eHdmiAudioFormat = eHdmiAudioOutFormat;
	s_eSpdifAudioFormat = eSpdifAudioOutFormat;

	return ERR_OK;
}

/*
	전체 output
*/
HERROR PAL_OUT_SetAvEnable(HBOOL bEnable)
{
	ENTER_CRITICAL_PALOUT;

	HxLOG_Debug("%s(%d)\n", __FUNCTION__, bEnable);

	s_bStbOutputEnable = bEnable;

#ifdef CONFIG_MW_OUTPUT_HDMI
	PAL_HDMI_SetOutputEnable(bEnable);
#endif
	pal_out_CheckComposite();

	pal_out_CheckAudioMute();

	if(bEnable != TRUE)
	{ /* Warm boot 로 인하여 DI status 는 invalidate 한다. */
		s_eVideoMacrovision= -1;
		s_eCgmsMacrovision= -1;
		s_eCsmsCopyRight = -1;
		s_eVideoBlankCopyRight = -1;
		s_eVideoBlankWss = -1;
		s_bDiAudioMute = -1;
	}

	LEAVE_CRITICAL_PALOUT;

	return ERR_OK;
}

HERROR PAL_OUT_GetAvEnable(HBOOL *pbEnable)
{
	ENTER_CRITICAL_PALOUT;

	*pbEnable = s_bStbOutputEnable;

	LEAVE_CRITICAL_PALOUT;

	return ERR_OK;
}

HERROR PAL_OUT_SetMasterAudioMute(HBOOL bMute)
{
	ENTER_CRITICAL_PALOUT;

	HxLOG_Debug("%s(%d)\n", __FUNCTION__, bMute);

	s_bMasterAudioMute = bMute;

	pal_out_CheckAudioMute();

	LEAVE_CRITICAL_PALOUT;

	return ERR_OK;
}

HBOOL PAL_OUT_GetMasterAudioMute(void)
{
	HBOOL bMute;

	ENTER_CRITICAL_PALOUT;
	bMute = s_bMasterAudioMute;
	LEAVE_CRITICAL_PALOUT;

	return bMute;
}

HERROR PAL_OUT_SetHdcpAudioMute(HBOOL bMute)
{
	ENTER_CRITICAL_PALOUT;

	HxLOG_Debug("%s(%d)\n", __FUNCTION__, bMute);

	s_bHdcpAudioMute = bMute;

	pal_out_CheckAudioMute();

	LEAVE_CRITICAL_PALOUT;

	return ERR_OK;
}

HERROR PAL_OUT_SetDecoderAudioMute(HBOOL bMute)
{
	ENTER_CRITICAL_PALOUT;

	HxLOG_Debug("%s(%d)\n", __FUNCTION__, bMute);

	s_bDecoderAudioMute = bMute;

	pal_out_CheckAudioMute();

	LEAVE_CRITICAL_PALOUT;

	return ERR_OK;
}

HERROR PAL_OUT_SetBluetoothAudioMute(HBOOL bMute)
{
	ENTER_CRITICAL_PALOUT;

	HxLOG_Debug("%s(%d)\n", __FUNCTION__, bMute);

	s_bBluetoothAudioMute = bMute;

	pal_out_CheckAudioMute();

	LEAVE_CRITICAL_PALOUT;

	return ERR_OK;
}

HERROR PAL_OUT_GetVolumeRange(HUINT32 *pulVolumeRange)
{
	HxLOG_Debug("%s(%d)\n", __FUNCTION__, s_ulVolumeRange);

	if(pulVolumeRange != NULL)
	{
		*pulVolumeRange = s_ulVolumeRange;
	}

	return ERR_OK;
}

HERROR PAL_OUT_SetVolume(HUINT32 ulVolume)
{
	ENTER_CRITICAL_PALOUT;

	HxLOG_Debug("%s(%d)\n", __FUNCTION__, ulVolume);

	if(DI_AUDIO_SetVolume(0, ulVolume) != ERR_OK)
	{
		LEAVE_CRITICAL_PALOUT;
		return ERR_FAIL;
	}
	s_ulVolume = ulVolume;

	LEAVE_CRITICAL_PALOUT;

	return ERR_OK;
}

HERROR PAL_OUT_GetVolume(HUINT32 *pulVolume)
{
	if(pulVolume != NULL)
	{
		ENTER_CRITICAL_PALOUT;
		*pulVolume = s_ulVolume;
		LEAVE_CRITICAL_PALOUT;
	}

	return ERR_OK;
}

HERROR PAL_OUT_SetDigitalAudioOutCodec(DxAudioCodec_e eStreamAudioCodec)
{
	ENTER_CRITICAL_PALOUT;

	HxLOG_Debug("%s(%s)\n", __FUNCTION__, OTL_ENUM2STR_AudioCodec(eStreamAudioCodec));

	s_eStreamAudioCodec = eStreamAudioCodec;

	pal_out_CheckHdmiAudioOut();
	pal_out_CheckSpdifAudioOut();

	LEAVE_CRITICAL_PALOUT;

	return ERR_OK;
}

HERROR PAL_OUT_CheckDigitalAudioOut(void)
{
	ENTER_CRITICAL_PALOUT;

	HxLOG_Debug("%s()\n", __FUNCTION__);

	pal_out_CheckHdmiAudioOut();
	pal_out_CheckSpdifAudioOut();

	LEAVE_CRITICAL_PALOUT;

	return ERR_OK;
}

HERROR PAL_OUT_SetHdmiAudioTransCode(DxAudioCodec_e eSourceCodec, DxAudioCodec_e eTransCodec)
{
	ENTER_CRITICAL_PALOUT;

	HxLOG_Debug("%s(src:%s, trans:%s)\n", __FUNCTION__
				, OTL_ENUM2STR_AudioCodec(eSourceCodec)
				, OTL_ENUM2STR_AudioCodec(eTransCodec)
				);

	s_eHdmiTransCodec[eSourceCodec] = eTransCodec;

	LEAVE_CRITICAL_PALOUT;

	return ERR_OK;
}

HERROR PAL_OUT_GetHdmiAudioTransCode(DxAudioCodec_e eSourceCodec, DxAudioCodec_e *peTransCodec)
{
	if(peTransCodec != NULL)
	{
		ENTER_CRITICAL_PALOUT;
		*peTransCodec = s_eHdmiTransCodec[eSourceCodec];
		LEAVE_CRITICAL_PALOUT;
	}

	return ERR_OK;
}

HERROR PAL_OUT_SetHdmiAudioFormat(DxDigitalAudioFormat_e eDigitalAudioFormat)
{
	ENTER_CRITICAL_PALOUT;

	HxLOG_Debug("%s(%s)\n", __FUNCTION__, OTL_ENUM2STR_DigitalAudioOutput(eDigitalAudioFormat));

	s_eHdmiAudioFormat = eDigitalAudioFormat;

	pal_out_CheckHdmiAudioOut();

	LEAVE_CRITICAL_PALOUT;

	return ERR_OK;
}

HERROR PAL_OUT_GetHdmiAudioFormat(DxDigitalAudioFormat_e *peDigitalAudioFormat)
{
	if(peDigitalAudioFormat != NULL)
	{
		ENTER_CRITICAL_PALOUT;
		*peDigitalAudioFormat = s_eHdmiAudioFormat;
		LEAVE_CRITICAL_PALOUT;
	}

	return ERR_OK;
}

HERROR PAL_OUT_SetSpdifAudioTransCode(DxAudioCodec_e eSourceCodec, DxAudioCodec_e eTransCodec)
{
	ENTER_CRITICAL_PALOUT;

	HxLOG_Debug("%s(src:%s, trans:%s)\n", __FUNCTION__
				, OTL_ENUM2STR_AudioCodec(eSourceCodec)
				, OTL_ENUM2STR_AudioCodec(eTransCodec)
				);

	s_eSpdifTransCodec[eSourceCodec] = eTransCodec;

	LEAVE_CRITICAL_PALOUT;

	return ERR_OK;
}

HERROR PAL_OUT_GetSpdifAudioTransCode(DxAudioCodec_e eSourceCodec, DxAudioCodec_e *peTransCodec)
{
	if(peTransCodec != NULL)
	{
		ENTER_CRITICAL_PALOUT;
		*peTransCodec = s_eSpdifTransCodec[eSourceCodec];
		LEAVE_CRITICAL_PALOUT;
	}

	return ERR_OK;
}


HERROR PAL_OUT_SetSpdifAudioFormat(DxDigitalAudioFormat_e eDigitalAudioFormat)
{
	ENTER_CRITICAL_PALOUT;

	HxLOG_Debug("%s(%s)\n", __FUNCTION__, OTL_ENUM2STR_DigitalAudioOutput(eDigitalAudioFormat));

	s_eSpdifAudioFormat = eDigitalAudioFormat;

	pal_out_CheckSpdifAudioOut();

	LEAVE_CRITICAL_PALOUT;

	return ERR_OK;
}



HERROR PAL_OUT_GetSpdifAudioFormat(DxDigitalAudioFormat_e *peDigitalAudioFormat)
{
	if(peDigitalAudioFormat != NULL)
	{
		ENTER_CRITICAL_PALOUT;
		*peDigitalAudioFormat = s_eSpdifAudioFormat;
		LEAVE_CRITICAL_PALOUT;
	}

	return ERR_OK;
}

HERROR PAL_OUT_SetMacrovision(Macrovision_t eMacrovision)
{
	DI_ERR_CODE			diErr;
	DI_VIDEO_MV_MODE	eDiMacrovision;

	HxLOG_Debug("%s(%s)\n", __FUNCTION__, OTL_ENUM2STR_Macrovision(eMacrovision));

	switch(eMacrovision)
	{
		case eMACROVISION_OFF:
			eDiMacrovision = DI_VIDEO_MV_NONE;
			break;

		case eMACROVISION_AGC:
			eDiMacrovision = DI_VIDEO_MV_AGC_ONLY;
			break;

		case eMACROVISION_2LINE:
			eDiMacrovision = DI_VIDEO_MV_AGC_2LINES;
			break;

		case eMACROVISION_4LINE:
			eDiMacrovision = DI_VIDEO_MV_AGC_4LINES;
			break;

		case eMACROVISION_TEST1:
			eDiMacrovision = DI_VIDEO_MV_TEST1;
			break;

		case eMACROVISION_TEST2:
			eDiMacrovision = DI_VIDEO_MV_TEST2;
			break;

		default:
#if defined(CONFIG_MW_OUTPUT_ANALOG_STANDARD_NTSC)
			eDiMacrovision = DI_VIDEO_MV_STANDARD_NTSC;
#else
			eDiMacrovision = DI_VIDEO_MV_STANDARD_PAL;
#endif
			break;
	}

	if(eMacrovision != s_eVideoMacrovision)
	{
		diErr = DI_VIDEO_SetMacroVision(eDiMacrovision);
		if(diErr != ERR_OK)
		{
			HxLOG_Error("\tERROR DI_VIDEO_SetMacroVision ret:0x%X\n", diErr);
			return ERR_FAIL;
		}
	}
	else
	{
		HxLOG_Debug("\tSame ... Exit ...\n");
	}

	s_eVideoMacrovision = eMacrovision;

	return ERR_OK;
}

HERROR PAL_OUT_SetCgmsMacrovision(Macrovision_t eMacrovision)
{
	DI_VIDEO_APS_TRIGGER		eDiApsTriggger;

	if( (s_eVideoBlankSpec != eVIDEO_BLANK_SPEC_CGMS)
		&& (s_eVideoBlankSpec != eVIDEO_BLANK_SPEC_CGMS_JAPAN)
	)
	{
		return ERR_FAIL;
	}

	HxLOG_Debug("%s(%s)\n", __FUNCTION__, OTL_ENUM2STR_Macrovision(eMacrovision));

	if(eMacrovision == s_eCgmsMacrovision)
	{
		HxLOG_Debug("\tAlready applied macrovision. Exit ...\n");
		return ERR_OK;
	}

	switch(eMacrovision)
	{
		case eMACROVISION_OFF:
			eDiApsTriggger = DI_VIDEO_CGMS_PSP_OFF;
			break;

		case eMACROVISION_AGC:
			eDiApsTriggger = DI_VIDEO_CGMS_PSP_ON_SBURST_OFF;
			break;

		case eMACROVISION_2LINE:
			eDiApsTriggger = DI_VIDEO_CGMS_PSP_ON_2Line_SBURST;
			break;

		case eMACROVISION_4LINE:
			eDiApsTriggger = DI_VIDEO_CGMS_PSP_ON_4Line_SBURST;
			break;

		default:
			HxLOG_Error("\t\tERROR \n");
			return ERR_FAIL;
	}

	if(s_eVideoBlankSpec == eVIDEO_BLANK_SPEC_CGMS)
	{
		s_stDiCgms.uTypeStatus.tEiaCea805.eApsTrigger = eDiApsTriggger;
	}
	else if(s_eVideoBlankSpec == eVIDEO_BLANK_SPEC_CGMS_JAPAN)
	{
		s_stDiCgms.uTypeStatus.tArib.eApsTrigger = eDiApsTriggger;
	}

	pal_out_SetDiCgms();

	return ERR_OK;
}

HERROR PAL_OUT_SetScmsCopyRight(CopyRight_t eCopyRight)
{
	DI_ERR_CODE					nDiErr;
	DI_AUDIO_COPYRIGHT_MODE		diAudioCp;

	HxLOG_Debug("%s(%s)\n", __FUNCTION__, OTL_ENUM2STR_CopyRight(eCopyRight));

	if(eCopyRight == eCOPYRIGHT_COPY_FREE)
	{
		diAudioCp = DI_AUDOUT_COPYRIGHT_NO_RESTRICTION;
	}
	else if(eCopyRight == eCOPYRIGHT_COPY_NOMORE)
	{
		diAudioCp = DI_AUDOUT_COPYRIGHT_NO_COPY;
	}
	else if(eCopyRight == eCOPYRIGHT_COPY_ONCE)
	{
			diAudioCp = DI_AUDOUT_COPYRIGHT_ONE_COPY;
	}
	else if(eCopyRight == eCOPYRIGHT_COPY_NEVER)
	{
			diAudioCp = DI_AUDOUT_COPYRIGHT_NO_COPY;
	}
	else
	{
		HxLOG_Error("\tERROR Invalid eMode:0x%X\n",eCopyRight);
		return ERR_FAIL;
	}

	ENTER_CRITICAL_PALOUT;

	if(eCopyRight != s_eCsmsCopyRight)
	{
		HxLOG_Debug("\tdiAudioCp(%d) \n", diAudioCp);

		nDiErr = DI_AUDIO_SetSCMS(diAudioCp);
		if(DI_ERR_OK != nDiErr)
		{
			HxLOG_Error("\tERROR DI_AUDIO_SetSCMS ret:0x%X\n",nDiErr);
			LEAVE_CRITICAL_PALOUT;
			return ERR_FAIL;
		}

		s_eCsmsCopyRight = eCopyRight;
	}
	else
	{
		HxLOG_Debug("\tSame ... Exit ...\n");
	}

	LEAVE_CRITICAL_PALOUT;

	return ERR_OK;
}

HERROR PAL_OUT_SetVideoBlankCopyRight(CopyRight_t eCopyRight)
{
	HxLOG_Debug("%s(%s)\n", __FUNCTION__, OTL_ENUM2STR_CopyRight(eCopyRight));

	if(eCopyRight != s_eVideoBlankCopyRight)
	{
		pal_out_SetWssCopyRight(eCopyRight);
		pal_out_SetCgmsCopyRight(eCopyRight);

		s_eVideoBlankCopyRight = eCopyRight;
	}
	else
	{
		HxLOG_Debug("\tSame ... Exit ...\n");
	}

	return ERR_OK;
}

HERROR PAL_OUT_SetVideoBlankAspectRatio(VIDEO_Wss_t eWss)
{
	HxLOG_Debug("%s(%s)\n", __FUNCTION__, OTL_ENUM2STR_Wss(eWss));

	if(eWss != s_eVideoBlankWss)
	{
		pal_out_SetWssAspectRatio(eWss);
		pal_out_SetCgmsAspectRatio(eWss);

		s_eVideoBlankWss = eWss;
	}
	else
	{
		HxLOG_Debug("\tSame ... Exit ...\n");
	}

	return ERR_OK;
}

HERROR PAL_OUT_GetOutputStatus(
							DxAudioCodec_e		*peHdmiAudioCodec,
							DxAudioCodec_e		*peSpdifAudioCodec,
							Macrovision_t		*peVideoMacrovision,
							Macrovision_t		*peCgmsMacrovision,
							CopyRight_t			*peCsmsCopyRight,
							CopyRight_t			*peVideoBlankCopyRight,
							VIDEO_Wss_t			*peVideoBlankWss
							)
{
	ENTER_CRITICAL_PALOUT;

	if(peHdmiAudioCodec != NULL)
	{
		*peHdmiAudioCodec = s_eHdmiAudioCodec;
	}

	if(peSpdifAudioCodec != NULL)
	{
		*peSpdifAudioCodec = s_eSpdifAudioCodec;
	}

	if(peVideoMacrovision != NULL)
	{
		*peVideoMacrovision = s_eVideoMacrovision;
	}

	if(peCgmsMacrovision != NULL)
	{
		*peCgmsMacrovision = s_eCgmsMacrovision;
	}

	if(peCsmsCopyRight != NULL)
	{
		*peCsmsCopyRight = s_eCsmsCopyRight;
	}

	if(peVideoBlankCopyRight != NULL)
	{
		*peVideoBlankCopyRight = s_eVideoBlankCopyRight;
	}

	if(peVideoBlankWss != NULL)
	{
		*peVideoBlankWss = s_eVideoBlankWss;
	}

	LEAVE_CRITICAL_PALOUT;

	return ERR_OK;
}





#define _____DEBUG_API______________________________________________________________________________________________________________________________


/*********************** End of File ******************************/

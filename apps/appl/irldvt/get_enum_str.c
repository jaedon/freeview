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

//#include <octo_common.h>
#include <get_enum_str.h>

const HINT8 *OTL_ENUM2STR_ChDeliveryType(DxDeliveryType_e eDeliveryType)
{
	switch(eDeliveryType)
	{
		ENUM_TO_STR(eDxDELIVERY_TYPE_ALL);
		ENUM_TO_STR(eDxDELIVERY_TYPE_SAT);
		ENUM_TO_STR(eDxDELIVERY_TYPE_TER);
		ENUM_TO_STR(eDxDELIVERY_TYPE_CAB);
		ENUM_TO_STR(eDxDELIVERY_TYPE_MULTICAST);
		default:
			break;
	}

	HxLOG_Error("ERROR!!Unknown Channel Delivery Type[%d]\n", eDeliveryType);
	return "UNKNOWN";
}

const HINT8 *OTL_ENUM2STR_VideoCodec(DxVideoCodec_e eVideoCodec)
{
	switch (eVideoCodec)
	{
		ENUM_TO_STR(eDxVIDEO_CODEC_UNKNOWN);
		ENUM_TO_STR(eDxVIDEO_CODEC_MPEG1);
		ENUM_TO_STR(eDxVIDEO_CODEC_MPEG2);
		ENUM_TO_STR(eDxVIDEO_CODEC_MPEG4_PART2);
		ENUM_TO_STR(eDxVIDEO_CODEC_H263);
		ENUM_TO_STR(eDxVIDEO_CODEC_H264);
		ENUM_TO_STR(eDxVIDEO_CODEC_H264_SVC);
		ENUM_TO_STR(eDxVIDEO_CODEC_H264_MVC);
		ENUM_TO_STR(eDxVIDEO_CODEC_VC1);
		ENUM_TO_STR(eDxVIDEO_CODEC_VC1_SIMPLEMAIN);
		ENUM_TO_STR(eDxVIDEO_CODEC_AVS);
		ENUM_TO_STR(eDxVIDEO_CODEC_RV40);
		ENUM_TO_STR(eDxVIDEO_CODEC_VP6);
		ENUM_TO_STR(eDxVIDEO_CODEC_DIVX311);
		default: break;
	}
	return ("Unknown");
}

const HINT8 *OTL_ENUM2STR_VideoFrameRate(VIDEO_FrameRate_t eFrameRate)
{
	switch(eFrameRate)
	{
		ENUM_TO_STR(eFrameRate_Unknown);
		ENUM_TO_STR(eFrameRate_24);
		ENUM_TO_STR(eFrameRate_25);
		ENUM_TO_STR(eFrameRate_30);
		ENUM_TO_STR(eFrameRate_50);
		ENUM_TO_STR(eFrameRate_60);
		default: break;
	}

	return ("Unknown");
}

const HINT8 *OTL_ENUM2STR_Video3DFormat(DxVideo3DFormat_e e3DFormat)
{
	switch(e3DFormat)
	{
		ENUM_TO_STR(eDxVIDEO_3DFORMAT_NONE);
		ENUM_TO_STR(eDxVIDEO_3DFORMAT_SIDEBYSIDE);
		ENUM_TO_STR(eDxVIDEO_3DFORMAT_TOPANDBOTTOM);
		ENUM_TO_STR(eDxVIDEO_3DFORMAT_FRAMEPACKING);
		default: break;
	}

	return ("Unknown");
}

const HINT8 *OTL_ENUM2STR_Video3DSelect(DxHdmi3DSelect_e e3DSelect)
{
	switch(e3DSelect)
	{
		ENUM_TO_STR(eDxHDMI_3DSELECT_OFF);
		ENUM_TO_STR(eDxHDMI_3DSELECT_SIDEBYSIDE);
		ENUM_TO_STR(eDxHDMI_3DSELECT_TOPANDBOTTOM);
		ENUM_TO_STR(eDxHDMI_3DSELECT_FRAMEPACKING);
		ENUM_TO_STR(eDxHDMI_3DSELECT_AUTO);
		default: break;
	}

	return ("Unknown");
}


const HINT8 *OTL_ENUM2STR_VideoPictureRate(DxPictureRate_e ePictureRate)
{
	switch(ePictureRate)
	{
		ENUM_TO_STR(eDxPICTURERATE_UNKNOWN);
		ENUM_TO_STR(eDxPICTURERATE_24P);
		ENUM_TO_STR(eDxPICTURERATE_25P);
		ENUM_TO_STR(eDxPICTURERATE_30P);
		ENUM_TO_STR(eDxPICTURERATE_50I);
		ENUM_TO_STR(eDxPICTURERATE_60I);
		ENUM_TO_STR(eDxPICTURERATE_50P);
		ENUM_TO_STR(eDxPICTURERATE_60P);
		default: break;
	}

	return ("Unknown");
}

const HINT8 *OTL_ENUM2STR_Afd(VIDEO_Afd_t eAfd)
{
	switch (eAfd)
	{
		ENUM_TO_STR(eAfd_None);
		ENUM_TO_STR(eAfd_Full);
		ENUM_TO_STR(eAfd_43);
		ENUM_TO_STR(eAfd_169);
		ENUM_TO_STR(eAfd_149);
		ENUM_TO_STR(eAfd_43_SnP149);
		ENUM_TO_STR(eAfd_169_SnP149);
		ENUM_TO_STR(eAfd_169_SnP43);
		default: break;
	}
	return ("Unknown");
}

const HINT8 *OTL_ENUM2STR_ResolutionSelect(DxResolutionSelect_e eVideoOutputSelect)
{
	switch (eVideoOutputSelect)
	{
		ENUM_TO_STR(eDxRESOLUTION_SELECT_UNKNOWN);
		ENUM_TO_STR(eDxRESOLUTION_SELECT_ORIGINAL);
		ENUM_TO_STR(eDxRESOLUTION_SELECT_1080P);
		ENUM_TO_STR(eDxRESOLUTION_SELECT_1080I);
		ENUM_TO_STR(eDxRESOLUTION_SELECT_720P);
		ENUM_TO_STR(eDxRESOLUTION_SELECT_480P);
		ENUM_TO_STR(eDxRESOLUTION_SELECT_480I);
		ENUM_TO_STR(eDxRESOLUTION_SELECT_576P);
		ENUM_TO_STR(eDxRESOLUTION_SELECT_576I);
		ENUM_TO_STR(eDxRESOLUTION_SELECT_EDID_MAX);
		default: break;
	}

	return ("Unknown");
}

const HINT8 *OTL_ENUM2STR_ResolutionStatus(DxResolutionStatus_e eVideoRes)
{
	switch (eVideoRes)
	{
		ENUM_TO_STR(eDxRESOLUTION_STATUS_UNKNOWN);
		ENUM_TO_STR(eDxRESOLUTION_STATUS_480I);
		ENUM_TO_STR(eDxRESOLUTION_STATUS_480P);
		ENUM_TO_STR(eDxRESOLUTION_STATUS_576I);
		ENUM_TO_STR(eDxRESOLUTION_STATUS_576P);
		ENUM_TO_STR(eDxRESOLUTION_STATUS_720P);
		ENUM_TO_STR(eDxRESOLUTION_STATUS_1080I);
		ENUM_TO_STR(eDxRESOLUTION_STATUS_1080P);
		default: break;
	}

	return ("Unknown");
}

const HINT8 *OTL_ENUM2STR_AspectRatio(DxAspectRatio_e eAspectRatio)
{
	switch (eAspectRatio)
	{
		ENUM_TO_STR(eDxASPECTRATIO_UNKNOWN);
		ENUM_TO_STR(eDxASPECTRATIO_4X3);
		ENUM_TO_STR(eDxASPECTRATIO_16X9);
		ENUM_TO_STR(eDxASPECTRATIO_1X1);
		ENUM_TO_STR(eDxASPECTRATIO_14X9);
		ENUM_TO_STR(eDxASPECTRATIO_MAX_NUM);
		default: break;
	}

	return ("Unknown");
}

const HINT8 *OTL_ENUM2STR_DfcSelect(DxDfcSelect_e eDfc)
{
	switch (eDfc)
	{
		ENUM_TO_STR(eDxDFC_SELECT_UNKNOWN);
		ENUM_TO_STR(eDxDFC_SELECT_DEFAULT);
		ENUM_TO_STR(eDxDFC_SELECT_WIDESIGNAL);
		ENUM_TO_STR(eDxDFC_SELECT_SQUEEZE);
		ENUM_TO_STR(eDxDFC_SELECT_SQUEEZE149);
		ENUM_TO_STR(eDxDFC_SELECT_CENTERCUTOUT);
		ENUM_TO_STR(eDxDFC_SELECT_CENTERCUTOUT149);
		ENUM_TO_STR(eDxDFC_SELECT_FULLSCREEN);
		ENUM_TO_STR(eDxDFC_SELECT_LETTERBOX);
		ENUM_TO_STR(eDxDFC_SELECT_LETTERBOX149);
		ENUM_TO_STR(eDxDFC_SELECT_PILLARBOX);
		ENUM_TO_STR(eDxDFC_SELECT_PILLARBOX149);
		ENUM_TO_STR(eDxDFC_SELECT_ZOOM);
		ENUM_TO_STR(eDxDFC_SELECT_ZOOM149);
		ENUM_TO_STR(eDxDFC_SELECT_SIDECUT);
		ENUM_TO_STR(eDxDFC_SELECT_SIDECUT149);
		ENUM_TO_STR(eDxDFC_SELECT_MAX_NUM);
		default: break;
	}

	return ("Unknown");
}

const HINT8 *OTL_ENUM2STR_Wss(VIDEO_Wss_t eWss)
{
	switch (eWss)
	{
		ENUM_TO_STR(eWss_43_Full);
		ENUM_TO_STR(eWss_43_LB149_Center);
		ENUM_TO_STR(eWss_43_LB169_Center);
		ENUM_TO_STR(eWss_43_SnP149);
		ENUM_TO_STR(eWss_169_Full);
		default: break;
	}

	return ("Unknown");
}

const HINT8 *OTL_ENUM2STR_ScartFormat(DxScartFormat_e eScartFormat)
{
	switch (eScartFormat)
	{
		ENUM_TO_STR(eDxSCART_FORMAT_UNKNOWN);
		ENUM_TO_STR(eDxSCART_FORMAT_CVBS);
		ENUM_TO_STR(eDxSCART_FORMAT_RGB);
		ENUM_TO_STR(eDxSCART_FORMAT_SVIDEO);
		default: break;
	}

	return ("Unknown");
}

const HINT8 *OTL_ENUM2STR_ScartPath(DxScartPath_e eScartPath)
{
	switch (eScartPath)
	{
		ENUM_TO_STR(eDxSCART_PATH_STB2TV);
		ENUM_TO_STR(eDxSCART_PATH_VCR2TV);
		ENUM_TO_STR(eDxSCART_PATH_HWBYPASS);
		default: break;
	}

	return ("Unknown");
}

const HINT8 *OTL_ENUM2STR_HdcpStatus(DxHdcpStatus_e eHdcpStatus)
{
	switch (eHdcpStatus)
	{
		ENUM_TO_STR(eDxHDCPSTATUS_OFF);
		ENUM_TO_STR(eDxHDCPSTATUS_TRY);
		ENUM_TO_STR(eDxHDCPSTATUS_OK);
		ENUM_TO_STR(eDxHDCPSTATUS_FAIL);
		ENUM_TO_STR(eDxHDCPSTATUS_NODEVICE);
		default: break;
	}

	return ("Unknown");
}

const HINT8 *OTL_ENUM2STR_CompositeStandard(DxCompositeStandard_e eCompositeStandard)
{
	switch (eCompositeStandard)
	{
		ENUM_TO_STR(eDxCOMPOSITE_STANDARD_UNKNOWN);
		ENUM_TO_STR(eDxCOMPOSITE_STANDARD_PAL);
		ENUM_TO_STR(eDxCOMPOSITE_STANDARD_PAL_M);
		ENUM_TO_STR(eDxCOMPOSITE_STANDARD_PAL_N);
		ENUM_TO_STR(eDxCOMPOSITE_STANDARD_NTSC);
		ENUM_TO_STR(eDxCOMPOSITE_STANDARD_SECAM);
		default: break;
	}

	return ("Unknown");
}

const HINT8 *OTL_ENUM2STR_VideoBlankSpec(VideoBlankSpec_e eVideoBlankSpec)
{
	switch(eVideoBlankSpec)
	{
		ENUM_TO_STR(eVIDEO_BLANK_SPEC_NONE);
		ENUM_TO_STR(eVIDEO_BLANK_SPEC_WSS);
		ENUM_TO_STR(eVIDEO_BLANK_SPEC_CGMS);
		ENUM_TO_STR(eVIDEO_BLANK_SPEC_CGMS_JAPAN);
		default: break;
	}

	return ("Unknown");
}

const HINT8 *OTL_ENUM2STR_AudioDecType(AudioDecType_t eAudioDecType)
{
	switch(eAudioDecType)
	{
		ENUM_TO_STR(eAUDIO_DEC_MAIN);
		ENUM_TO_STR(eAUDIO_DEC_SUB);
		ENUM_TO_STR(eAUDIO_DEC_PASS);
		default: break;
	}

	return ("Unknown");
}

const HINT8 *OTL_ENUM2STR_AudioCodec(DxAudioCodec_e eAudioCodec)
{
	switch (eAudioCodec)
	{
		ENUM_TO_STR(eDxAUDIO_CODEC_UNKNOWN);
		ENUM_TO_STR(eDxAUDIO_CODEC_MPEG);
		ENUM_TO_STR(eDxAUDIO_CODEC_MP3);
		ENUM_TO_STR(eDxAUDIO_CODEC_DOLBY_AC3);
		ENUM_TO_STR(eDxAUDIO_CODEC_DOLBY_AC3P);
		ENUM_TO_STR(eDxAUDIO_CODEC_PCM);
		ENUM_TO_STR(eDxAUDIO_CODEC_AAC);
		ENUM_TO_STR(eDxAUDIO_CODEC_AAC_LOAS);
		ENUM_TO_STR(eDxAUDIO_CODEC_AAC_PLUS);
		ENUM_TO_STR(eDxAUDIO_CODEC_AAC_PLUS_ADTS);
		ENUM_TO_STR(eDxAUDIO_CODEC_DTS);
		ENUM_TO_STR(eDxAUDIO_CODEC_DTS_HD);
		ENUM_TO_STR(eDxAUDIO_CODEC_LPCM_DVD);
		ENUM_TO_STR(eDxAUDIO_CODEC_LPCM_HD_DVD);
		ENUM_TO_STR(eDxAUDIO_CODEC_LPCM_BLUERAY);
		ENUM_TO_STR(eDxAUDIO_CODEC_WMA_STD);
		ENUM_TO_STR(eDxAUDIO_CODEC_WMA_PRO);
		ENUM_TO_STR(eDxAUDIO_CODEC_PCMWAVE);
		ENUM_TO_STR(eDxAUDIO_CODEC_DRA);
		ENUM_TO_STR(eDxAUDIO_CODEC_AIFF);
		ENUM_TO_STR(eDxAUDIO_CODEC_RIFF);
		ENUM_TO_STR(eDxAUDIO_CODEC_MAX_NUM);
		default: break;
	}

	return ("Unknown");
}

const HINT8 *OTL_ENUM2STR_DigitalAudioOutput(DxDigitalAudioFormat_e eDAudioOutMode)
{
	switch (eDAudioOutMode)
	{
		ENUM_TO_STR(eDxDIGITALAUDIO_FORMAT_UNKNOWN);
		ENUM_TO_STR(eDxDIGITALAUDIO_FORMAT_PCM);
		ENUM_TO_STR(eDxDIGITALAUDIO_FORMAT_MULTICHANNEL);
		default: break;
	}

	return ("Unknown");
}

const HINT8 *OTL_ENUM2STR_StereoFormat(DxStereoFormat_e eStereoFormat)
{
	switch (eStereoFormat)
	{
		ENUM_TO_STR(eDxSTEREO_FORMAT_UNKNOWN);
		ENUM_TO_STR(eDxSTEREO_FORMAT_2CH);
		ENUM_TO_STR(eDxSTEREO_FORMAT_MULTI);
		ENUM_TO_STR(eDxSTEREO_FORMAT_MONO);
		ENUM_TO_STR(eDxSTEREO_FORMAT_DUALMONO);
		default: break;
	}

	return ("Unknown");
}

const HINT8 *OTL_ENUM2STR_StereoSelect(DxStereoSelect_e eStereoSelect)
{
	switch (eStereoSelect)
	{
		ENUM_TO_STR(eDxSTEREO_SELECT_UNKNOWN);
		ENUM_TO_STR(eDxSTEREO_SELECT_STEREO);
		ENUM_TO_STR(eDxSTEREO_SELECT_MONOLEFT);
		ENUM_TO_STR(eDxSTEREO_SELECT_MONORIGHT);
		ENUM_TO_STR(eDxSTEREO_SELECT_MONOMIXED);
		default: break;
	}

	return ("Unknown");
}

const HINT8 *OTL_ENUM2STR_DualMonoSelect(DxDualMonoSelect_e eDualMonoSelect)
{
	switch (eDualMonoSelect)
	{
		ENUM_TO_STR(eDxDUALMONO_SELECT_UNKNOWN);
		ENUM_TO_STR(eDxDUALMONO_SELECT_BOTH);
		ENUM_TO_STR(eDxDUALMONO_SELECT_LEFT);
		ENUM_TO_STR(eDxDUALMONO_SELECT_RIGHT);
		default: break;
	}

	return ("Unknown");
}

const HINT8 *OTL_ENUM2STR_Macrovision(Macrovision_t eMacrovision)
{
	switch (eMacrovision)
	{
		ENUM_TO_STR(eMACROVISION_OFF);
		ENUM_TO_STR(eMACROVISION_AGC);
		ENUM_TO_STR(eMACROVISION_2LINE);
		ENUM_TO_STR(eMACROVISION_4LINE);
		default: break;
	}

	return ("Unknown");
}

const HINT8 *OTL_ENUM2STR_CopyRight(CopyRight_t eCopyRight)
{
	switch (eCopyRight)
	{
		ENUM_TO_STR(eCOPYRIGHT_COPY_FREE);
		ENUM_TO_STR(eCOPYRIGHT_COPY_NOMORE);
		ENUM_TO_STR(eCOPYRIGHT_COPY_ONCE);
		ENUM_TO_STR(eCOPYRIGHT_COPY_NEVER);
		default: break;
	}

	return ("Unknown");
}

const HINT8 *OTL_ENUM2STR_HdcpBlockMode(HdcpBlockMode_e eHdcpBlockMode)
{
	switch (eHdcpBlockMode)
	{
		ENUM_TO_STR(eHDCP_BLOCK_NONE);
		ENUM_TO_STR((eHDCP_BLOCK_AVMUTE));
		ENUM_TO_STR((eHDCP_BLOCK_HDMIBLANK));
		ENUM_TO_STR((eHDCP_BLOCK_DOWNSCALE));
		default: break;
	}

	return ("Unknown");
}

const HINT8 *OTL_ENUM2STR_DeliveryType(DxDeliveryType_e eDeliveryType)
{
	switch (eDeliveryType)
	{
		ENUM_TO_STR(eDxDELIVERY_TYPE_ALL);
		ENUM_TO_STR(eDxDELIVERY_TYPE_SAT);
		ENUM_TO_STR(eDxDELIVERY_TYPE_TER);
		ENUM_TO_STR(eDxDELIVERY_TYPE_CAB);
		ENUM_TO_STR(eDxDELIVERY_TYPE_MULTICAST);
		default: break;
	}

	return ("Unknown");
}

const HINT8 *OTL_ENUM2STR_TrickModeCtrl(eDxCopyrightControl_SesTrickModeCtrl_e eTrickModeCtrl)
{
#if 0
	switch (eTrickModeCtrl)
	{
		ENUM_TO_STR(eDxCopyrightControl_SesTrickModeCtrl_NoRestriction);
		ENUM_TO_STR(eDxCopyrightControl_SesTrickModeCtrl_NoSkip_FF2x_Visible);
		ENUM_TO_STR(eDxCopyrightControl_SesTrickModeCtrl_NoSkip_FF4x_Visible);
		ENUM_TO_STR(eDxCopyrightControl_SesTrickModeCtrl_NoSkip_FF8x_Visible);
		ENUM_TO_STR(eDxCopyrightControl_SesTrickModeCtrl_NoSkip_Jump_FF);
		default: break;
	}
#endif
	return ("Unknown");
}


const HINT8 *OTL_ENUM2STR_StorageRight(eDxCopyrightControl_SesStorageRight_e eStorageRight)
{
	switch (eStorageRight)
	{
		ENUM_TO_STR(eDxCopyrightControl_SesStorageRight_NoRestriction);
		ENUM_TO_STR(eDxCopyrightControl_SesStorageRight_NoMore90Min);
		ENUM_TO_STR(eDxCopyrightControl_SesStorageRight_NoRecording);
		ENUM_TO_STR(eDxCopyrightControl_SesStorageRight_ExpireSameDay);
		ENUM_TO_STR(eDxCopyrightControl_SesStorageRight_Expire3Day);
		ENUM_TO_STR(eDxCopyrightControl_SesStorageRight_Expire7Day);
		ENUM_TO_STR(eDxCopyrightControl_SesStorageRight_Expire60Day);
		default: break;
	}

	return ("Unknown");
}

const HINT8 *OTL_ENUM2STR_CompVidCtrl(eDxCopyrightControl_SesComponentCtrl_e eCompVidCtrl)
{
	switch (eCompVidCtrl)
	{
		ENUM_TO_STR(eDxCopyrightControl_SesComponentCtrl_NoRestriction);
		ENUM_TO_STR(eDxCopyrightControl_SesComponentCtrl_DownScale);
		ENUM_TO_STR(eDxCopyrightControl_SesComponentCtrl_DisableOutput);
		default: break;
	}

	return ("Unknown");
}


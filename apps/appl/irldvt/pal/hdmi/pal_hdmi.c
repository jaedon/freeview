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

/**
	@file	pal_hdmi.c
	@brief	pal_hdmi.c

	Description: HDMI 동작 처리. 실제 상태를 저장. \n
	Module: PAL /HDMI												\n

	Copyright (c) 2008 HUMAX Co., Ltd.								\n
	All rights reserved.											\n
*/

/*******************************************************************/
/********************      Header Files    *************************/
/*******************************************************************/
#include <di_video.h>
#include <di_hdmi.h>

#include <get_enum_str.h>
#include <hlib.h>

#include <pal_hdmi.h>

/*******************************************************************/
/********************      Global Variables     ********************/
/*******************************************************************/

/*******************************************************************/
/********************      Defines         *************************/
/*******************************************************************/
#define palHdmi_MAX_TIMING_LIST			64

/**************************************************************************/
/********************      Typedef            *****************************/
/**************************************************************************/


/**************************************************************************/
/********************      Static Variables      **************************/
/**************************************************************************/
STATIC PalHdmi_Status_t				s_stHdmiStatus;	/* hdmi 출력이 2개인 stb은 고려하지 않음. 추후에 필요한 경우 늘려서 처리. */

STATIC HUINT32						s_ulNumOfHdmiVideoTiming;
STATIC VIDEO_Timing_t				s_stHdmiVideoTimingList[palHdmi_MAX_TIMING_LIST];

STATIC HUINT32						s_ulNumOfHdmiAudioCodec;
STATIC DxAudioCodec_e 				s_eHdmiAudioCodecList[eDxAUDIO_CODEC_MAX_NUM];

STATIC DxVideo3DFormat_b			s_ul3DCapability;

/* HDCP 결과, Hot Plug 등의 device event를 받기 위한 callback */
void (*s_pfnHdmiCallbackFunc)(PalHdmi_Event_e);

STATIC HULONG						s_ulPalHdmiSemId;
STATIC HULONG						s_ulPalHdmiMsgQId;
STATIC HULONG						s_ulPalHdmiTaskId;

	/* HDMI avi frame info */
STATIC DI_HDMI_AUX_VIDEO_INFO		s_stDiAviInfoFrame;
STATIC HBOOL						s_bDiHdmiPower = -1;
STATIC HBOOL						s_bDiHdmiOutput = -1;

#define	palHdmi_ENTER_CRITICAL	VK_SEM_Get(s_ulPalHdmiSemId)
#define	palHdmi_LEAVE_CRITICAL	VK_SEM_Release(s_ulPalHdmiSemId)


/**************************************************************************/
/********************      Functions       ********************************/
/**************************************************************************/
STATIC HERROR pal_hdmi_PrintStatus(HBOOL bDebug);
STATIC HERROR pal_hdmi_PrintCapability(void);

#if 0
STATIC const HINT8 *pal_hdmi_GetDiHdmiResString(DI_HDMI_VIDEO_RESOLUTION eDiHdmiRes)
{
	switch (eDiHdmiRes)
	{
		ENUM_TO_STR(DI_HDMI_RESOLUTION_720x240P);
		ENUM_TO_STR(DI_HDMI_RESOLUTION_2880x240P);
		ENUM_TO_STR(DI_HDMI_RESOLUTION_640x480P);
		ENUM_TO_STR(DI_HDMI_RESOLUTION_720x480I);
		ENUM_TO_STR(DI_HDMI_RESOLUTION_720x480P);
		ENUM_TO_STR(DI_HDMI_RESOLUTION_1440x480P);
		ENUM_TO_STR(DI_HDMI_RESOLUTION_2880x480I);
		ENUM_TO_STR(DI_HDMI_RESOLUTION_2880x480P);
		ENUM_TO_STR(DI_HDMI_RESOLUTION_720x288P);
		ENUM_TO_STR(DI_HDMI_RESOLUTION_2880x288P);
		ENUM_TO_STR(DI_HDMI_RESOLUTION_720x576I);
		ENUM_TO_STR(DI_HDMI_RESOLUTION_720x576P);
		ENUM_TO_STR(DI_HDMI_RESOLUTION_1440x576P);
		ENUM_TO_STR(DI_HDMI_RESOLUTION_2880x576I);
		ENUM_TO_STR(DI_HDMI_RESOLUTION_2880x576P);
		ENUM_TO_STR(DI_HDMI_RESOLUTION_1080x720P);
		ENUM_TO_STR(DI_HDMI_RESOLUTION_1280x720P);
		ENUM_TO_STR(DI_HDMI_RESOLUTION_1920x1080I);
		ENUM_TO_STR(DI_HDMI_RESOLUTION_1920x1080P);
		ENUM_TO_STR(DI_HDMI_RESOLUTION_DUMMY);
		default: break;
	}

	return VALUE_TO_STR(DI_HDMI_RESOLUTION_Unknown);
}


STATIC DxResolutionStatus_e pal_hdmi_convert_di2pal_Resolution(DI_HDMI_VIDEO_RESOLUTION eDiHdmiRes)
{
	switch (eDiHdmiRes)
	{
		case DI_HDMI_RESOLUTION_720x480I:
			return eDxRESOLUTION_STATUS_480I;

		case DI_HDMI_RESOLUTION_720x480P:
			return eDxRESOLUTION_STATUS_480P;

		case DI_HDMI_RESOLUTION_720x576I:
			return eDxRESOLUTION_STATUS_576I;

		case DI_HDMI_RESOLUTION_720x576P:
			return eDxRESOLUTION_STATUS_576P;

		case DI_HDMI_RESOLUTION_1280x720P:
			return eDxRESOLUTION_STATUS_720P;

		case DI_HDMI_RESOLUTION_1920x1080I:
			return eDxRESOLUTION_STATUS_1080I;

		case DI_HDMI_RESOLUTION_1920x1080P:
			return eDxRESOLUTION_STATUS_1080P;

		default:
			HxLOG_Warning("\t\tUnsupported DI HDMI Resolution : 0x%X[%s] \n",
						eDiHdmiRes, pal_hdmi_GetDiHdmiResString(eDiHdmiRes));
			return eDxRESOLUTION_STATUS_UNKNOWN;
	}
}
#endif
STATIC DxPictureRate_e pal_hdmi_Convert_Di2Pal_PictureRate(HBOOL bProgressive, DI_HDMI_VIDEO_FIELD_RATE eDiHdmiFieldRate)
{
	switch (eDiHdmiFieldRate)
	{
		case DI_HDMI_FIELD_RATE_24HZ:
			return eDxPICTURERATE_24P;

		case DI_HDMI_FIELD_RATE_25HZ:
			return eDxPICTURERATE_25P;

		case DI_HDMI_FIELD_RATE_30HZ:
			return eDxPICTURERATE_30P;

		case DI_HDMI_FIELD_RATE_50HZ:
		case DI_HDMI_FIELD_RATE_100HZ:
		case DI_HDMI_FIELD_RATE_200HZ:
			if(bProgressive == TRUE)
			{
				return eDxPICTURERATE_50P;
			}
			else
			{
				return eDxPICTURERATE_50I;
			}

		case DI_HDMI_FIELD_RATE_60HZ:
		case DI_HDMI_FIELD_RATE_120HZ:
		case DI_HDMI_FIELD_RATE_240HZ:
			if(bProgressive == TRUE)
			{
				return eDxPICTURERATE_60P;
			}
			else
			{
				return eDxPICTURERATE_60I;
			}

		default:
			HxLOG_Error("\t\tUnsupported DI HDMI Frame Rate : %d \n", eDiHdmiFieldRate);
			return eDxPICTURERATE_UNKNOWN;
	}
}


STATIC DxAspectRatio_e pal_hdmi_convert_di2pal_AspectRatio(DI_HDMI_VIDEO_AR eDiHdmiAr)
{
	switch (eDiHdmiAr)
	{
		case DI_HDMI_VIDEO_AR_e4_3:
			return eDxASPECTRATIO_4X3;

		case DI_HDMI_VIDEO_AR_e16_9:
			return eDxASPECTRATIO_16X9;

		case DI_HDMI_VIDEO_AR_eNoData:
		default:
			HxLOG_Error("\t\tUnsupported DI HDMI Aspect Ratio : %d \n", eDiHdmiAr);
			return eDxASPECTRATIO_UNKNOWN;
	}
}

STATIC HBOOL pal_hdmi_UpdateHdmiStatus(void)
{
	HBOOL							bChanged;

	HDMI_RECEIVER_INFO				stDiReceiverInfo = {0};
	HDMI_RECEIVER_INFO_OPTION		ulDiInfoOption;

	HxLOG_Debug("\t%s()\n", __FUNCTION__);

	ulDiInfoOption = IS_CONNECTED | IS_HDMI_DEVICE;
	DI_HDMI_GetState(ulDiInfoOption, &stDiReceiverInfo);

	if(stDiReceiverInfo.bIsConnected != s_stHdmiStatus.bPlug)
	{
		s_stHdmiStatus.bPlug = stDiReceiverInfo.bIsConnected;
		bChanged = TRUE;
	}
	else
	{
		bChanged = FALSE;
	}

	s_stHdmiStatus.bIsHdmiDevice = stDiReceiverInfo.bIsHdmiDevice;
	s_stHdmiStatus.bIsHdcpDevice = TRUE; // TODO: 일단 TRUE. DI에서 항목 생성되면 대체.

	return bChanged;
}

#if 0
STATIC HBOOL pal_hdmi_UpdateCapability(void)
{
	DxResolutionStatus_e			eResolution;
	DxPictureRate_e					ePictureRate;
	DxAspectRatio_e					eAspectRatio;
	HBOOL							bCapabilityValid;
	HBOOL							bChanged;

	DI_HDMI_AVAILABLE_FORMATS		stDiCapa;
	HUINT32							i;
	DI_HDMI_EDID_VFORMAT			stDiEdidVFormat;

	HxLOG_Debug("\t%s()\n", __FUNCTION__);

	s_ulNumOfHdmiVideoTiming = 0;
	HxSTD_memset(&s_stHdmiVideoTimingList, 0x0, sizeof(VIDEO_Timing_t)*palHdmi_MAX_TIMING_LIST);

	s_ulNumOfHdmiAudioCodec = 0;
	HxSTD_memset(&s_eHdmiAudioCodecList, 0x0, sizeof(HUINT32) * eDxAUDIO_CODEC_MAX_NUM);

	s_ul3DCapability = eDxVIDEO_3DFORMAT_NONE;

	if(s_stHdmiStatus.bPlug == FALSE)
	{
		HxLOG_Debug("\t\thdmi is not connected.\n");
		bCapabilityValid = FALSE;
	}
	else if(DI_HDMI_GetCapability(&stDiCapa) != DI_ERR_OK)
	{
		HxLOG_Error("\t\tERROR from DI_HDMI_GetCapability(). \n");
		bCapabilityValid = FALSE;
	}
	else
	{
		HxLOG_Debug("\t\tulSupportVFormat (%x), ulSupportAFormat (%x) \n", stDiCapa.ulVideoFormats, stDiCapa.ulAudioFormats);

		s_ulNumOfHdmiVideoTiming = 0;
		for (i = 0; i < stDiCapa.ulNumOfVFormat; i++)
		{
			stDiEdidVFormat = stDiCapa.stVformats[i];

			HxLOG_Debug("\t\tvFormat:0x%X FieldRate:0x%X AspectRatio:0x%X\n",
																			stDiEdidVFormat.ulVFormat,
																			stDiEdidVFormat.ulFieldRate,
																			stDiEdidVFormat.ulAspectRatio);

			eResolution = pal_hdmi_convert_di2pal_Resolution(stDiEdidVFormat.ulVFormat);

			switch(eResolution)
			{
				case eDxRESOLUTION_STATUS_480I:
				case eDxRESOLUTION_STATUS_576I:
				case eDxRESOLUTION_STATUS_1080I:
					ePictureRate = pal_hdmi_Convert_Di2Pal_PictureRate(FALSE, stDiEdidVFormat.ulFieldRate);
					break;

				case eDxRESOLUTION_STATUS_480P:
				case eDxRESOLUTION_STATUS_576P:
				case eDxRESOLUTION_STATUS_720P:
				case eDxRESOLUTION_STATUS_1080P:
					ePictureRate = pal_hdmi_Convert_Di2Pal_PictureRate(TRUE, stDiEdidVFormat.ulFieldRate);
					break;

				default:
					ePictureRate = eDxPICTURERATE_UNKNOWN;
					break;
			}

			eAspectRatio = pal_hdmi_convert_di2pal_AspectRatio(stDiEdidVFormat.ulAspectRatio);

			if( (eResolution != eDxRESOLUTION_STATUS_UNKNOWN)
				&& (ePictureRate != eDxPICTURERATE_UNKNOWN)
				&& (eAspectRatio != eDxASPECTRATIO_UNKNOWN)
			)
			{
				s_stHdmiVideoTimingList[s_ulNumOfHdmiVideoTiming].eResolution = eResolution;
				s_stHdmiVideoTimingList[s_ulNumOfHdmiVideoTiming].ePictureRate = ePictureRate;
				s_stHdmiVideoTimingList[s_ulNumOfHdmiVideoTiming].eAspectRatio = eAspectRatio;

				s_ulNumOfHdmiVideoTiming++;
			}
		}

		/* Audio */
		if(stDiCapa.ulAudioFormats & DI_HDMI_AFORMAT_PCM)
		{
			s_eHdmiAudioCodecList[s_ulNumOfHdmiAudioCodec++] = eDxAUDIO_CODEC_PCM;
		}

		if(stDiCapa.ulAudioFormats & DI_HDMI_AFORMAT_AC3)
		{
			s_eHdmiAudioCodecList[s_ulNumOfHdmiAudioCodec++] = eDxAUDIO_CODEC_DOLBY_AC3;
		}

		if( (stDiCapa.ulAudioFormats & DI_HDMI_AFORMAT_MPEG1)
			|| (stDiCapa.ulAudioFormats & DI_HDMI_AFORMAT_MPEG2)
		)
		{
			s_eHdmiAudioCodecList[s_ulNumOfHdmiAudioCodec++] = eDxAUDIO_CODEC_MPEG;
		}

		if(stDiCapa.ulAudioFormats & DI_HDMI_AFORMAT_MP3)
		{
			s_eHdmiAudioCodecList[s_ulNumOfHdmiAudioCodec++] = eDxAUDIO_CODEC_MP3;
		}

		if(stDiCapa.ulAudioFormats & DI_HDMI_AFORMAT_AAC)
		{
			s_eHdmiAudioCodecList[s_ulNumOfHdmiAudioCodec++] = eDxAUDIO_CODEC_AAC;
		}

		if(stDiCapa.ulAudioFormats & DI_HDMI_AFORMAT_DTS)
		{
			s_eHdmiAudioCodecList[s_ulNumOfHdmiAudioCodec++] = eDxAUDIO_CODEC_DTS;
		}

		if(stDiCapa.ulAudioFormats & DI_HDMI_AFORMAT_AC3PLUS)
		{
			s_eHdmiAudioCodecList[s_ulNumOfHdmiAudioCodec++] = eDxAUDIO_CODEC_DOLBY_AC3P;
		}

		/* 3D format */
		if(stDiCapa.ul3dFormats & DI_HDMI_3D_FORMAT_SIDEBYSIDE)
		{
			s_ul3DCapability |= eDxVIDEO_3DFORMAT_SIDEBYSIDE;
		}

		if(stDiCapa.ul3dFormats & DI_HDMI_3D_FORMAT_TOPANDBOTTOM)
		{
			s_ul3DCapability |= eDxVIDEO_3DFORMAT_TOPANDBOTTOM;
		}

		if(stDiCapa.ul3dFormats & DI_HDMI_3D_FORMAT_FRAMEPACKING)
		{
			s_ul3DCapability |= eDxVIDEO_3DFORMAT_FRAMEPACKING;
		}

		bCapabilityValid = TRUE;
	}

	if(bCapabilityValid != s_stHdmiStatus.bCapabilityValid)
	{
		s_stHdmiStatus.bCapabilityValid = bCapabilityValid;
		bChanged = TRUE;
	}
	else
	{
		bChanged = FALSE;
	}

	return bChanged;
}
#endif
STATIC HBOOL pal_hdmi_SetHdcpState(DxHdcpStatus_e eHdcpStatus)
{
	HBOOL		bHcdpStatusNotify = FALSE;

	HxLOG_Debug("\t%s(%s)\n", __FUNCTION__, OTL_ENUM2STR_HdcpStatus(eHdcpStatus));

	if(eHdcpStatus != s_stHdmiStatus.eHdcpStatus)
	{
		HxLOG_Debug("\t\tChange HDCP status %s => %s\n"
					, OTL_ENUM2STR_HdcpStatus(s_stHdmiStatus.eHdcpStatus)
					, OTL_ENUM2STR_HdcpStatus(eHdcpStatus)
					);

		switch(eHdcpStatus)
		{
			case eDxHDCPSTATUS_OFF:
				bHcdpStatusNotify = TRUE;
				break;

			case eDxHDCPSTATUS_OK:

				if( (s_stHdmiStatus.eHdcpStatus == eDxHDCPSTATUS_TRY)
					/* TRY 중에 먼저 FAIL/NODEVICE 를 준 다음 나중에 OK가 올라오는 경우가 있음 */
					|| (s_stHdmiStatus.eHdcpStatus == eDxHDCPSTATUS_NODEVICE)
					|| (s_stHdmiStatus.eHdcpStatus == eDxHDCPSTATUS_FAIL)
				)
				{
					bHcdpStatusNotify = TRUE;
				}

				break;

			case eDxHDCPSTATUS_FAIL:
			case eDxHDCPSTATUS_NODEVICE:

				if(s_stHdmiStatus.eHdcpStatus == eDxHDCPSTATUS_TRY)
				{
					bHcdpStatusNotify = TRUE;
				}

				break;

			default:
				/* eDxHDCPSTATUS_TRY --> Do nothing */
				break;
		}

		s_stHdmiStatus.eHdcpStatus = eHdcpStatus;
	}
	else
	{
		HxLOG_Debug("\t\tSame state... Do nothing...\n");
	}

	return bHcdpStatusNotify;
}

STATIC HBOOL pal_hdmi_CheckHdcp(void)
{
	DI_ERR_CODE		diErr;

	HxLOG_Debug("\t%s(bOutputEnable:%d, bPlug:%d, bHdcpEnable:%d)\n"
				, __FUNCTION__
				, s_stHdmiStatus.bOutputEnable
				, s_stHdmiStatus.bPlug
				, s_stHdmiStatus.bHdcpEnable
				);

	if( (s_stHdmiStatus.bOutputEnable == TRUE)
		&& (s_stHdmiStatus.bPlug == TRUE)
		&& (s_stHdmiStatus.bHdcpEnable == TRUE)
	)
	{ /* HDCP 켜야 하는 상황 */
		if(s_stHdmiStatus.eHdcpStatus == eDxHDCPSTATUS_OFF)
		{ /* HDCP 안 켜져 있음. 켜야 함. */
			diErr = DI_HDMI_EnableHDCP(TRUE);

			if(diErr == ERR_OK)
			{
				return pal_hdmi_SetHdcpState(eDxHDCPSTATUS_TRY);
			}
			else
			{
				HxLOG_Error("\t\tLeave Error !! (diErr:0x%X\n",diErr);
			}
		}
		else
		{ /* HDCP 이미 켜져 있음. Do nothing */
			HxLOG_Debug("\t\tHDCP already turned ON ... exit ...\n");
		}
	}
	else
	{ /* HDCP 꺼야 하는 상황 */
		if(s_stHdmiStatus.eHdcpStatus != eDxHDCPSTATUS_OFF)
		{ /* HDCP 켜져 있음. 꺼야 함. */
			diErr = DI_HDMI_EnableHDCP(FALSE);

			if(diErr == ERR_OK)
			{
				return pal_hdmi_SetHdcpState(eDxHDCPSTATUS_OFF);
			}
			else
			{
				HxLOG_Error("\t\tLeave Error !! (diErr:0x%X\n",diErr);
			}
		}
		else
		{ /* HDCP 이미 꺼져 있음. Do nothing */
			HxLOG_Debug("\t\tHDCP already turned OFF ... exit ...\n");
		}
	}

	return FALSE;
}

STATIC HERROR pal_hdmi_CheckOutput(void)
{
	HBOOL		bHdmiPower;
	HBOOL		bHdmiOutput;

	DI_ERR_CODE	diErr;

	HxLOG_Debug("\t%s(bOutputEnable:%d, bPlug:%d)\n", __FUNCTION__
				, s_stHdmiStatus.bOutputEnable, s_stHdmiStatus.bPlug);

	if(s_stHdmiStatus.bOutputEnable == TRUE)
	{
		bHdmiPower = TRUE;

		if(s_stHdmiStatus.bPlug == TRUE)
		{
			bHdmiOutput = TRUE;
		}
		else
		{
			bHdmiOutput = FALSE;
		}
	}
	else
	{
		bHdmiPower = FALSE;
		bHdmiOutput = FALSE;
	}

	HxLOG_Debug("\t\tbHdmiPower:%d, bHdmiOutput:%d\n", bHdmiPower, bHdmiOutput);

	// shall be turned off hdmi power after turning off output
	if(bHdmiOutput != s_bDiHdmiOutput)
	{
		HxLOG_Debug("\t\tApply bHdmiOutput to DI\n");

		diErr = DI_HDMI_EnableOutput(bHdmiOutput);
		if(diErr != DI_ERR_OK)
		{
			HxLOG_Error("\t\tDI_HDMI_EnableOutput return error :0x%X \n", diErr);
			return ERR_FAIL;
		}

		s_bDiHdmiOutput = bHdmiOutput;
	}

	if(bHdmiPower != s_bDiHdmiPower)
	{
		HxLOG_Debug("\t\tApply bHdmiPower to DI\n");

		diErr = DI_HDMI_SetPowerState(bHdmiPower);
		if(diErr == DI_ERR_OK)
		{
			if(bHdmiPower == FALSE)
			{
				s_stHdmiStatus.bPlug = FALSE;
				s_stHdmiStatus.bCapabilityValid = FALSE;
			}
		}
		else
		{
			HxLOG_Error("\t\tDI_HDMI_SetPowerState return error :0x%X \n", diErr);
		}

		s_bDiHdmiPower = bHdmiPower;
	}

	if( (s_bDiHdmiOutput == FALSE) || (s_bDiHdmiPower == FALSE)	)
	{ /* Warm boot를 위하여 HDMI를 off 하는 경우에는 AVI info의 DI status도 invalidate 한다. */
		s_stDiAviInfoFrame.eAR = -1;
		s_stDiAviInfoFrame.eAFD = -1;
	}

	return ERR_OK;
}

STATIC void pal_hdmi_NotifyEvent(PalHdmi_Event_e eHdmiEvent)
{
	if (s_pfnHdmiCallbackFunc != NULL)
	{
		s_pfnHdmiCallbackFunc(eHdmiEvent);
	}
}

#if 0
STATIC void pal_hdmi_Task(void)
{
	HUINT32				nResult = 0;
	HBOOL				bHdmiChanged = FALSE;
	HBOOL				bHdcpChanged = FALSE;

	DI_HDMI_EVENT		eDiEvent;

	while(1)
	{
		nResult = (HUINT32)VK_MSG_Receive(s_ulPalHdmiMsgQId, &eDiEvent, sizeof(DI_HDMI_EVENT));
		if(nResult == ERR_OK)
		{
			palHdmi_ENTER_CRITICAL;

			switch(eDiEvent)
			{
				case DH_EVENT_HOTPLUG_CONNECT:
				case DH_EVENT_HOTPLUG_DISCONNECT:
					HxLOG_Debug("\t%s(DH_EVENT_HOTPLUG_%s)\n", __FUNCTION__
							, (eDiEvent == DH_EVENT_HOTPLUG_CONNECT) ? "CONNECT" : "DISCONNECT");

					bHdmiChanged = pal_hdmi_UpdateHdmiStatus();
					pal_hdmi_UpdateCapability();

					pal_hdmi_CheckOutput();
					bHdcpChanged = pal_hdmi_CheckHdcp();

					pal_hdmi_PrintStatus(TRUE);

					break;

				case DH_EVENT_HDCP_SUCCESS:
					HxLOG_Debug("\t%s(DH_EVENT_HDCP_SUCCESS)\n", __FUNCTION__);

					if( (s_stHdmiStatus.eHdcpStatus == eDxHDCPSTATUS_TRY)
						|| (s_stHdmiStatus.eHdcpStatus == eDxHDCPSTATUS_FAIL)
					)
					{
						bHdcpChanged = pal_hdmi_SetHdcpState(eDxHDCPSTATUS_OK);
					}
					else
					{
						HxLOG_Error("\tHECK!!! Unexpected event in %s\n", OTL_ENUM2STR_HdcpStatus(s_stHdmiStatus.eHdcpStatus));
					}

					break;

				case DH_EVENT_HDCP_FAIL:
				{
					HxLOG_Debug("\t%s(DH_EVENT_HDCP_FAIL)\n", __FUNCTION__);

					if( (s_stHdmiStatus.eHdcpStatus == eDxHDCPSTATUS_TRY)
						|| (s_stHdmiStatus.eHdcpStatus == eDxHDCPSTATUS_OK)
					)
					{
						HDMI_RECEIVER_INFO				stDiReceiverInfo;
						HDMI_RECEIVER_INFO_OPTION		ulDiInfoOption;
						DI_ERR_CODE 					diErr;

						ulDiInfoOption = IS_AUTHENCATED;
						stDiReceiverInfo.bIsAuthencated = FALSE;

						diErr = DI_HDMI_GetState(ulDiInfoOption, &stDiReceiverInfo);
						if(diErr != DI_ERR_OK)
						{
							HxLOG_Error("\tHECK!!! DI_HDMI_GetState() is returned ERROR. Check DI.....\n");
							break;
						}
#if 0
						if(stDiReceiverInfo.bIsHdcpDevice != TRUE)
						{ /* HDCP 지원 device가 아님. */
							bHdcpChanged = pal_hdmi_SetHdcpState(eDxHDCPSTATUS_NODEVICE);
						}
						else
#endif
						if(stDiReceiverInfo.bIsAuthencated != TRUE)
						{ /* HDCP authentication fail */
							bHdcpChanged = pal_hdmi_SetHdcpState(eDxHDCPSTATUS_FAIL);
						}
						else
						{
							HxLOG_Error("\tHECK!!! Authentication is TRUE. Check DI.....\n");
						}
					}
					else
					{
						HxLOG_Error("\tHECK!!! Unexpected event in %s\n", OTL_ENUM2STR_HdcpStatus(s_stHdmiStatus.eHdcpStatus));
					}


					break;
				}

				default:
					break;
			}

			palHdmi_LEAVE_CRITICAL;

			if(bHdmiChanged == TRUE)
			{
				pal_hdmi_NotifyEvent(eHDMI_EVENT_STATUS_CHANGED);
			}

			if(bHdcpChanged == TRUE)
			{
				pal_hdmi_NotifyEvent(eHDMI_EVENT_HDCP_CHANGED);
			}
		}
	}
}

#endif
STATIC void pal_hdmi_DiCallback(void *pvParam)
{
	DI_HDMI_EVENT		eDiEvent;
	HUINT32				nResult = 0;

	eDiEvent = *(HUINT8 *)pvParam;

	HxLOG_Debug("\t%s(%d)\n", __FUNCTION__, eDiEvent);

	nResult = (HUINT32)VK_MSG_SendTimeout(s_ulPalHdmiMsgQId, &eDiEvent, sizeof(DI_HDMI_EVENT), 0);
	if(nResult != ERR_OK)
	{
		HxLOG_Error("\n");
	}

	return;
}

STATIC HERROR pal_hdmi_PrintStatus(HBOOL bDebug)
{
	if(bDebug == TRUE)
	{
		HxLOG_Debug("======== HDMI Staus ========\n");
		HxLOG_Debug("\t\tOutput Enable : %d\n", s_stHdmiStatus.bOutputEnable);
		HxLOG_Debug("\t\tbHdcpEnable : %d\n", s_stHdmiStatus.bHdcpEnable);
		HxLOG_Debug("\t\tbPlug : %d\n", s_stHdmiStatus.bPlug);
		HxLOG_Debug("\t\tbIsHdmiDevice : %d\n", s_stHdmiStatus.bIsHdmiDevice);
		HxLOG_Debug("\t\tbIsHdcpDevice : %d\n", s_stHdmiStatus.bIsHdcpDevice);
		HxLOG_Debug("\t\tbCapabilityValid : %d\n", s_stHdmiStatus.bCapabilityValid);
		HxLOG_Debug("\t\teHdcpStatus : %s\n", OTL_ENUM2STR_HdcpStatus(s_stHdmiStatus.eHdcpStatus));
	}
	else
	{
		HLIB_CMD_Printf("======== HDMI Staus ========\n");
		HLIB_CMD_Printf("\t\tOutput Enable : %d \n", s_stHdmiStatus.bOutputEnable);
		HLIB_CMD_Printf("\t\tbHdcpEnable : %d\n", s_stHdmiStatus.bHdcpEnable);
		HLIB_CMD_Printf("\t\tbPlug : %d\n", s_stHdmiStatus.bPlug);
		HLIB_CMD_Printf("\t\tbIsHdmiDevice : %d\n", s_stHdmiStatus.bIsHdmiDevice);
		HLIB_CMD_Printf("\t\tbIsHdcpDevice : %d\n", s_stHdmiStatus.bIsHdcpDevice);
		HLIB_CMD_Printf("\t\tbCapabilityValid : %d\n", s_stHdmiStatus.bCapabilityValid);
		HLIB_CMD_Printf("\t\teHdcpStatus : %s\n", OTL_ENUM2STR_HdcpStatus(s_stHdmiStatus.eHdcpStatus));
	}

	return ERR_OK;
}

STATIC HERROR pal_hdmi_PrintCapability(void)
{
	HUINT32 i = 0;

	HxLOG_Print("\t\t---hdmi video capability\n");

	for(i = 0; i < s_ulNumOfHdmiVideoTiming; i++)
	{
		HLIB_CMD_Printf("\t\t[%s, %s, %s]\n",
					OTL_ENUM2STR_ResolutionStatus(s_stHdmiVideoTimingList[i].eResolution),
					OTL_ENUM2STR_AspectRatio(s_stHdmiVideoTimingList[i].eAspectRatio),
					OTL_ENUM2STR_VideoPictureRate(s_stHdmiVideoTimingList[i].ePictureRate));
	}

	HLIB_CMD_Printf("\t\t---hdmi audio capability\n");
	for(i=0; i<eDxAUDIO_CODEC_MAX_NUM; i++)
	{
		if(s_eHdmiAudioCodecList[i] != eDxAUDIO_CODEC_UNKNOWN)
		{
			HLIB_CMD_Printf("\t\t(%s)  \n", OTL_ENUM2STR_AudioCodec(s_eHdmiAudioCodecList[i]));
		}
	}

	HLIB_CMD_Printf("\t\t---hdmi 3D format (0x%x)\n", s_ul3DCapability);

	return ERR_OK;
}

STATIC int pal_hdmi_ProcessCommand_status(void *szArgList)
{
	pal_hdmi_PrintStatus(FALSE);

	return HxCMD_OK;
}

STATIC int pal_hdmi_ProcessCommand_capa(void *szArgList)
{
	pal_hdmi_PrintCapability();

	return HxCMD_OK;
}

STATIC int pal_hdmi_ProcessCommand_output(void *szArgList)
{
	char			*aucArg=NULL;
	char			aucStr[20];

	aucArg = HLIB_CMD_GetNextArg((HCHAR**)&szArgList);
	if( aucArg == NULL )
	{
		return HxCMD_ERR;
	}
	else
	{
		if( sscanf(aucArg, "%s", (char *)&aucStr) != 0 )
		{
			if(strcmp(aucStr, "on") == 0)
			{
				PAL_HDMI_SetOutputEnable(TRUE);
			}
			else if(strcmp(aucStr, "off") == 0)
			{
				PAL_HDMI_SetOutputEnable(FALSE);
			}
		}
	}

	return HxCMD_OK;
}

STATIC int pal_hdmi_ProcessCommand_avinfo(void *szArgList)
{
	char			*aucArg=NULL;
	char			aucStr[20];
	DxAspectRatio_e	eAr;
	VIDEO_Afd_t		eAfd;

	aucArg = HLIB_CMD_GetNextArg((HCHAR**)&szArgList);

	if( aucArg == NULL )
	{
		return HxCMD_ERR;
	}
	else
	{
		if( sscanf(aucArg, "%s", (char *)&aucStr) != 0 )
		{
			if(strcmp(aucStr, "43") == 0)
			{
				eAr = eDxASPECTRATIO_4X3;
			}
			else if(strcmp(aucStr, "169") == 0)
			{
				eAr = eDxASPECTRATIO_16X9;
			}
			else
			{
				return HxCMD_ERR;
			}

			aucArg = HLIB_CMD_GetNextArg((HCHAR**)&szArgList);
			if( aucArg == NULL )
			{
				return HxCMD_ERR;
			}
			else
			{
				if( sscanf(aucArg, "%s", (char *)&aucStr) != 0 )
				{
					if(strcmp(aucStr, "full") == 0)
					{
						eAfd = eAfd_Full;
					}
					else if(strcmp(aucStr, "43") == 0)
					{
						eAfd = eAfd_43;
					}
					else if(strcmp(aucStr, "169") == 0)
					{
						eAfd = eAfd_169;
					}
					else if(strcmp(aucStr, "149") == 0)
					{
						eAfd = eAfd_149;
					}
					else if(strcmp(aucStr, "43snp149") == 0)
					{
						eAfd = eAfd_43_SnP149;
					}
					else if(strcmp(aucStr, "169snp149") == 0)
					{
						eAfd = eAfd_169_SnP149;
					}
					else if(strcmp(aucStr, "169snp43") == 0)
					{
						eAfd = eAfd_169_SnP43;
					}
					else
					{
						return HxCMD_ERR;
					}

					PAL_HDMI_SetAviInfoFrame(eAr, eAfd);
				}
				else
				{
					return HxCMD_ERR;
				}
			}
		}
		else
		{
			return HxCMD_ERR;
		}
	}

	return HxCMD_OK;
}

STATIC int pal_hdmi_ProcessCommand_hdcp_enable(void *szArgList)
{
	char			*aucArg=NULL;
	char			aucStr[20];

	aucArg = HLIB_CMD_GetNextArg((HCHAR**)&szArgList);
	if( aucArg == NULL )
	{
		return HxCMD_ERR;
	}
	else
	{
		if( sscanf(aucArg, "%s", (char *)&aucStr) != 0 )
		{
			if(strcmp(aucStr, "on") == 0)
			{
				PAL_HDMI_SetHdcp(TRUE);
			}
			else if(strcmp(aucStr, "off") == 0)
			{
				PAL_HDMI_SetHdcp(FALSE);
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
	}

	return HxCMD_OK;
}

STATIC int pal_hdmi_ProcessCommand_hdcp_event(void *szArgList)
{
	char			*aucArg=NULL;
	char			aucStr[20];

	aucArg = HLIB_CMD_GetNextArg((HCHAR**)&szArgList);
	if( aucArg == NULL )
	{
		return HxCMD_ERR;
	}
	else
	{
		if( sscanf(aucArg, "%s", (char *)&aucStr) != 0 )
		{
			if(strcmp(aucStr, "ok") == 0)
			{
				pal_hdmi_SetHdcpState(eDxHDCPSTATUS_OK);
			}
			else if(strcmp(aucStr, "fail") == 0)
			{
				pal_hdmi_SetHdcpState(eDxHDCPSTATUS_FAIL);
			}
			else if(strcmp(aucStr, "nodevice") == 0)
			{
				pal_hdmi_SetHdcpState(eDxHDCPSTATUS_NODEVICE);
			}
			else
			{
				return HxCMD_ERR;
			}

			pal_hdmi_NotifyEvent(eHDMI_EVENT_HDCP_CHANGED);
		}
		else
		{
			return HxCMD_ERR;
		}
	}

	return HxCMD_OK;
}

#define	hCmdHandle		"pal_hdmi"

STATIC void pal_hdmi_RegisterCommand(void)
{
	HLIB_CMD_RegisterWord(hCmdHandle,
						pal_hdmi_ProcessCommand_status,
						"status",
						"Print HDMI status.",
						"status");

	HLIB_CMD_RegisterWord(hCmdHandle,
						pal_hdmi_ProcessCommand_capa,
						"capa",
						"Print HDMI TV capability.",
						"capa");

	HLIB_CMD_RegisterWord(hCmdHandle,
						pal_hdmi_ProcessCommand_output,
						"output",
						"Enable/Disable HDMI output.",
						"output [on|off]");

	HLIB_CMD_RegisterWord(hCmdHandle,
						pal_hdmi_ProcessCommand_avinfo,
						"avinfo",
						"Set AVI infoframe.",
						"avinfo [43|169] [full|43|169|149|43snp149|169snp149|169snp43]");

	HLIB_CMD_RegisterWord(hCmdHandle,
						pal_hdmi_ProcessCommand_hdcp_enable,
						"hdcp_enable",
						"HDCP on/off",
						"hdcp_enable [on|off]");

	HLIB_CMD_RegisterWord(hCmdHandle,
						pal_hdmi_ProcessCommand_hdcp_event,
						"hdcp_event",
						"Send HDCP event to appkit",
						"hdcp_event [ok|fail|nodevice]");

}

#if 0
void PAL_HDMI_Init(void)
{

//	HLIB_LOG_SetLevel (HxLOG_DOMAIN, HxLOG_ALL);

	HxSTD_memset(&s_stHdmiStatus, 0, sizeof(PalHdmi_Status_t));

	/* 맨 처음 셋팅이 내려가도록 -1 로 초기화 */
	s_stDiAviInfoFrame.eAR = -1;
	s_stDiAviInfoFrame.eAFD = -1;

	/* Message Q & task for DI callback */
	if(VK_SEM_Create(&s_ulPalHdmiSemId, "PalHdmiSem", VK_SUSPENDTYPE_PRIORITY) != ERR_OK)
	{
		HxLOG_Error("\t\tFailure in Semaphore Creation \n");
	}

	if(VK_MSG_Create(32, sizeof(DI_HDMI_EVENT), "PalHdmiMsgQ", &s_ulPalHdmiMsgQId, VK_SUSPENDTYPE_PRIORITY) != ERR_OK)
	{
		HxLOG_Error(" \n");
		return;
	}

	if(VK_TASK_Create((void*)pal_hdmi_Task,
					PAL_DICALLBACK_TASK_PRIORITY,
					PAL_HDMI_TASK_STACK_SIZE,
					"PalHdmiTask",
					NULL,
					&s_ulPalHdmiTaskId,
					0) != ERR_OK)
	{
		HxLOG_Error(" \n");
		return;
	}

	if(VK_TASK_Start(s_ulPalHdmiTaskId) != ERR_OK)
	{
		HxLOG_Error(" \n");
		return;
	}

	pal_hdmi_UpdateHdmiStatus();
	pal_hdmi_UpdateCapability();

	s_pfnHdmiCallbackFunc = NULL;
	DI_HDMI_RegisterEventNotify(DH_EVENT_HOTPLUG_CONNECT, pal_hdmi_DiCallback);
	DI_HDMI_RegisterEventNotify(DH_EVENT_HOTPLUG_DISCONNECT, pal_hdmi_DiCallback);
	DI_HDMI_RegisterEventNotify(DH_EVENT_HDCP_SUCCESS, pal_hdmi_DiCallback);
	DI_HDMI_RegisterEventNotify(DH_EVENT_HDCP_FAIL, pal_hdmi_DiCallback);

	pal_hdmi_RegisterCommand();

	return;
}

#endif

HERROR PAL_HDMI_RegisterCallback(void (*hdmiCallbackFunc)(PalHdmi_Event_e eEvent))
{
	s_pfnHdmiCallbackFunc = hdmiCallbackFunc;

	return ERR_OK;
}

HERROR PAL_HDMI_SetOutputEnable(HBOOL bEnable)
{
	HBOOL bHdcpChanged = FALSE;

	palHdmi_ENTER_CRITICAL;

	HxLOG_Debug("%s(%d)\n", __FUNCTION__, bEnable);

	s_stHdmiStatus.bOutputEnable = bEnable;

	if (bEnable == TRUE)
	{
		pal_hdmi_CheckOutput();
		bHdcpChanged = pal_hdmi_CheckHdcp();

	}
	else
	{
		bHdcpChanged = pal_hdmi_CheckHdcp();
		pal_hdmi_CheckOutput();
	}

	palHdmi_LEAVE_CRITICAL;

	if(bHdcpChanged == TRUE)
	{
		pal_hdmi_NotifyEvent(eHDMI_EVENT_HDCP_CHANGED);
	}

	return ERR_OK;
}

HERROR PAL_HDMI_SetBlank(HBOOL bEnable)
{
	DI_ERR_CODE		diErr;

	palHdmi_ENTER_CRITICAL;

	HxLOG_Debug("%s(%d)\n", __FUNCTION__, bEnable);

	if( (s_stHdmiStatus.bOutputEnable == TRUE) && (s_stHdmiStatus.bPlug == TRUE) )
	{
		diErr = DI_HDMI_SetAVBlank(bEnable);
		if(diErr != ERR_OK)
		{
			HxLOG_Error("\t\tLeave Error !! (diErr:0x%X\n",diErr);
			return ERR_FAIL;
		}
	}
	else
	{
		HxLOG_Debug("%s(bOutputEnable:%d, bPlug:%d) ===> Ignored.\n", __FUNCTION__
					, s_stHdmiStatus.bOutputEnable, s_stHdmiStatus.bPlug);
	}

	palHdmi_LEAVE_CRITICAL;

	return ERR_OK;
}

HERROR PAL_HDMI_GetSupportVideoTimingList(HUINT32 *pulNumOfList, VIDEO_Timing_t **ppstVideoTimingList)
{
	HERROR		err;

	palHdmi_ENTER_CRITICAL;

	HxLOG_Debug("%s()\n", __FUNCTION__);

	if(pulNumOfList != NULL)
	{
		*pulNumOfList = s_ulNumOfHdmiVideoTiming;
	}

	if(ppstVideoTimingList != NULL)
	{
		*ppstVideoTimingList = s_stHdmiVideoTimingList;
	}

	/* HDMI Connected & capability is already updated. */
	if ( (s_stHdmiStatus.bCapabilityValid == TRUE)
		&& (s_stHdmiStatus.bPlug == TRUE)
		&& ( (s_stHdmiStatus.eHdcpStatus == eDxHDCPSTATUS_OK)
			|| (s_stHdmiStatus.eHdcpStatus == eDxHDCPSTATUS_OFF)
			)
	)
	{
		err = ERR_OK;
	}
	else
	{
		HxLOG_Debug("\tVideo timing list is NOT available by the follow reason.\n");
		pal_hdmi_PrintStatus(TRUE);
		err = ERR_FAIL;
	}

	palHdmi_LEAVE_CRITICAL;

	return err;
}

HERROR PAL_HDMI_GetSupportAudioCodecList(HUINT32 *pulNumOfList, DxAudioCodec_e **ppeAudioCodecList)
{
	HERROR		err;

	palHdmi_ENTER_CRITICAL;

	HxLOG_Debug("%s()\n", __FUNCTION__);

	if(pulNumOfList != NULL)
	{
		*pulNumOfList = s_ulNumOfHdmiAudioCodec;
	}

	if(ppeAudioCodecList != NULL)
	{
		*ppeAudioCodecList = s_eHdmiAudioCodecList;
	}

	/* HDMI Connected & capability is already updated. */
	if (s_stHdmiStatus.bCapabilityValid == TRUE && s_stHdmiStatus.bPlug == TRUE)
	{
		HxLOG_Debug("\t\tHDMI is connected & capability is already updated.\n");
		err = ERR_OK;
	}
	else
	{
		HxLOG_Debug("\t\tHDMI is NOT connected OR capability is NOT updated.\n");
		err = ERR_FAIL;
	}

	palHdmi_LEAVE_CRITICAL;

	return err;
}

HERROR PAL_HDMI_GetStatus(PalHdmi_Status_t *pstStatus)
{
	palHdmi_ENTER_CRITICAL;

	HxLOG_Debug("%s()\n", __FUNCTION__);
	*pstStatus = s_stHdmiStatus;

	palHdmi_LEAVE_CRITICAL;

	return ERR_OK;
}

HBOOL PAL_HDMI_IsPlugedIn(void)
{
	 return s_stHdmiStatus.bPlug;
}

HERROR PAL_HDMI_SetHdcp(HBOOL bEnable)
{
	HBOOL		bHdcpChanged;

	palHdmi_ENTER_CRITICAL;

	HxLOG_Debug("%s(%d)\n", __FUNCTION__, bEnable);

	s_stHdmiStatus.bHdcpEnable = bEnable;

	bHdcpChanged = pal_hdmi_CheckHdcp();

	palHdmi_LEAVE_CRITICAL;

	if(bHdcpChanged == TRUE)
	{
		pal_hdmi_NotifyEvent(eHDMI_EVENT_HDCP_CHANGED);
	}

	return ERR_OK;
}

void PAL_HDMI_GetHdcp(HBOOL *pbEnable)
{
	*pbEnable = s_stHdmiStatus.bHdcpEnable;
}

HERROR PAL_HDMI_CheckVideoTimingAvailable(VIDEO_Timing_t stTiming)
{
	HERROR		err;
	HBOOL		bAspectRatioMatch, bPictureRateMatch;
	HUINT32		i;

	palHdmi_ENTER_CRITICAL;

	HxLOG_Debug("%s(%s, %s, %s)\n", __FUNCTION__,
				OTL_ENUM2STR_ResolutionStatus(stTiming.eResolution),
				OTL_ENUM2STR_AspectRatio(stTiming.eAspectRatio),
				OTL_ENUM2STR_VideoPictureRate(stTiming.ePictureRate));

	if( (s_stHdmiStatus.bCapabilityValid != TRUE) || (s_stHdmiStatus.bPlug != TRUE) )
	{
		HxLOG_Debug("\tHDMI is unconnected !!\n");
		err = ERR_FAIL;

		goto exit;
	}

	for(i = 0; i < s_ulNumOfHdmiVideoTiming; i++)
	{
		bAspectRatioMatch = FALSE;
		bPictureRateMatch = FALSE;

		if(stTiming.eResolution == s_stHdmiVideoTimingList[i].eResolution)
		{
			if( (stTiming.eAspectRatio == eDxASPECTRATIO_UNKNOWN)
				|| (stTiming.eAspectRatio == s_stHdmiVideoTimingList[i].eAspectRatio)
			)
			{
				bAspectRatioMatch = TRUE;
			}

			if( (stTiming.ePictureRate == eDxPICTURERATE_UNKNOWN)
				|| (stTiming.ePictureRate == s_stHdmiVideoTimingList[i].ePictureRate)
			)
			{
				bPictureRateMatch = TRUE;
			}

			if( (bAspectRatioMatch == TRUE) && (bPictureRateMatch == TRUE) )
			{
				break;
			}
		}
	}

	if(i == s_ulNumOfHdmiVideoTiming)
	{
		HxLOG_Debug("\tNOT support this video timing. FAIL\n");
		err = ERR_FAIL;
	}
	else
	{
		HxLOG_Debug("\tSupport this video timing. OK\n");
		err = ERR_OK;
	}

exit:
	palHdmi_LEAVE_CRITICAL;

	return err;
}

HERROR PAL_HDMI_CheckAudioCodecAvailable(DxAudioCodec_e eAudioCodec)
{
	HUINT32		i;
	HERROR		err;

	palHdmi_ENTER_CRITICAL;

	HxLOG_Debug("%s(%s)\n", __FUNCTION__, OTL_ENUM2STR_AudioCodec(eAudioCodec));

	if( (s_stHdmiStatus.bCapabilityValid != TRUE) || (s_stHdmiStatus.bPlug != TRUE) )
	{
		HxLOG_Debug("\t\tHDMI is unconnected !!\n");
		err = ERR_FAIL;

		goto exit;
	}

	for(i = 0; i < s_ulNumOfHdmiAudioCodec; i++)
	{
		if(eAudioCodec == s_eHdmiAudioCodecList[i])
		{
			HxLOG_Debug("\t\tSupport this audio codec. OK\n");
			err = ERR_OK;

			goto exit;
		}
	}

	HxLOG_Debug("\t\tNOT support this audio codec. FAIL\n");
	err = ERR_FAIL;

exit:
	palHdmi_LEAVE_CRITICAL;

	return err;
}

HERROR PAL_HDMI_SetAviInfoFrame(DxAspectRatio_e eAspectRatio, VIDEO_Afd_t eAfd)
{
	DI_HDMI_VIDEO_AR		eDiHdmiAspectRatio;
	DI_HDMI_VIDEO_AFD		eDiHdmiAfd;

	HxLOG_Debug("%s(%s, %s)\n", __FUNCTION__, OTL_ENUM2STR_AspectRatio(eAspectRatio), OTL_ENUM2STR_Afd(eAfd));

	if(s_stHdmiStatus.bPlug != TRUE)
	{
		s_stHdmiStatus.eAspectRatio = eDxASPECTRATIO_UNKNOWN;
		s_stHdmiStatus.eAfd = eAfd_None;

		s_stDiAviInfoFrame.eAR = -1;
		s_stDiAviInfoFrame.eAFD = -1;

		return ERR_FAIL;
	}
	else
	{
		s_stHdmiStatus.eAspectRatio = eAspectRatio;
		s_stHdmiStatus.eAfd = eAfd;
	}

	if(eAspectRatio == eDxASPECTRATIO_4X3)
	{
		eDiHdmiAspectRatio = DI_HDMI_VIDEO_AR_e4_3;
	}
	else if(eAspectRatio == eDxASPECTRATIO_16X9)
	{
		eDiHdmiAspectRatio = DI_HDMI_VIDEO_AR_e16_9;
	}
	else
	{
		eDiHdmiAspectRatio = DI_HDMI_VIDEO_AR_eNoData;
	}

	switch(eAfd)
	{
		case eAfd_43:
			eDiHdmiAfd = DI_HDMI_VIDEO_AFD_e4_3Center;
			break;

		case eAfd_169:
			eDiHdmiAfd = DI_HDMI_VIDEO_AFD_e16_9Center;
			break;

		case eAfd_149:
			eDiHdmiAfd = DI_HDMI_VIDEO_AFD_e14_9Center;
			break;

		case eAfd_43_SnP149:
			eDiHdmiAfd = DI_HDMI_VIDEO_AFD_e4_3_SNP_14_9Center;
			break;

		case eAfd_169_SnP149:
			eDiHdmiAfd = DI_HDMI_VIDEO_AFD_e16_9_SNP_14_9Center;
			break;

		case eAfd_169_SnP43:
			eDiHdmiAfd = DI_HDMI_VIDEO_AFD_e16_9_SNP_4_3Center;
			break;

		default: // eAfd_None, eAfd_Full
			eDiHdmiAfd = DI_HDMI_VIDEO_AFD_ePicture;
			break;
	}

	if( (s_stDiAviInfoFrame.eAR != eDiHdmiAspectRatio)
		|| (s_stDiAviInfoFrame.eAFD != eDiHdmiAfd)
	)
	{
		HxLOG_Debug("\t\tSet DI AVI info frame AR(%d) -> (%d), AFD(%d) -> (%d)\n"
								, s_stDiAviInfoFrame.eAR
								, eDiHdmiAspectRatio
								, s_stDiAviInfoFrame.eAFD
								, eDiHdmiAfd
								);

		s_stDiAviInfoFrame.eAR = eDiHdmiAspectRatio;
		s_stDiAviInfoFrame.eAFD = eDiHdmiAfd;

		DI_HDMI_SetAuxVideoInfo(s_stDiAviInfoFrame);
	}

	return ERR_OK;
}

HERROR PAL_HDMI_Get3DFormatCapability(DxVideo3DFormat_b *pulHdmi3DCapa)
{
	if(s_stHdmiStatus.bCapabilityValid == TRUE)
	{
		*pulHdmi3DCapa = s_ul3DCapability;

		return ERR_OK;
	}
	else
	{
		*pulHdmi3DCapa = eDxVIDEO_3DFORMAT_NONE;

		return ERR_FAIL;
	}
}


/*---------------------------------------------------------------------------*/


/*********************** End of File ******************************/

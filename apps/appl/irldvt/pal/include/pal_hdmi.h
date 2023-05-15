/*******************************************************************
	File Description
********************************************************************/
/**
	@file	  pal_hdmi.h
	@brief	  hdmi driver�� adaptation layer 													\n

	Description: hdmi ���۰� �� parameter�� ������ �Ǵ��Ͽ�, HMDI Setting�� �Ѵ�. 				\n
	Module: PAL/HDMI			 																\n

	Copyright (c) 2008 HUMAX Co., Ltd.															\n
	All rights reserved.																		\n
*/


#ifndef	__PAL_HDMI_H__
#define	__PAL_HDMI_H__


/*******************************************************************/
/********************      Header Files    *************************/
/*******************************************************************/
//#include <octo_common.h>

#include <dlib.h>

/*******************************************************************/
/********************      Global Variables     ********************/
/*******************************************************************/

/*******************************************************************/
/********************      Macro Definition     ********************/
/*******************************************************************/

/*******************************************************************/
/********************      Typedef         *************************/
/*******************************************************************/
/* HDMI Status ����..*/
typedef struct
{
	/* User Setting */
	HBOOL				bOutputEnable;				/*Application scenario�� ���� en/disable ó��. �� Flag�� Active standby ���� ���¸� ���� ����. */
	HBOOL				bHdcpEnable;				/* Application�� HDCP ���� ����. */

	/* DI Status */
	HBOOL				bPlug;						/* HDMI ���� ���� ����.. ����Ǹ� TRUE, ���ŵǸ� FALSE, Application���� ������ �� �ƴ�. */
	HBOOL 				bIsHdmiDevice;				/* DVI or HDMI ����. */
	HBOOL 				bIsHdcpDevice;				/* HDCP ���� ���� */

	/* DI Capability */
	HBOOL				bCapabilityValid;			/* Capability Update ����. */

	/* Output Status */
	DxHdcpStatus_e		eHdcpStatus;				/* HDCP status : OK/authentication fail/not HDCP device */
	DxAspectRatio_e	eAspectRatio;
	VIDEO_Afd_t			eAfd;
} PalHdmi_Status_t;

typedef enum
{
	eHDMI_EVENT_STATUS_CHANGED,
	eHDMI_EVENT_HDCP_CHANGED
} PalHdmi_Event_e;

/*******************************************************************/

void PAL_HDMI_Init(void);

/* ������ output handling? */
HERROR PAL_HDMI_SetOutputEnable(HBOOL bEnable);
HERROR PAL_HDMI_SetBlank(HBOOL bEnable);
HERROR PAL_HDMI_GetStatus(PalHdmi_Status_t *pstStatus);
HERROR PAL_HDMI_GetSupportVideoTimingList(HUINT32 *pulNumOfList, VIDEO_Timing_t **ppstVideoTimingList);
HERROR PAL_HDMI_GetSupportAudioCodecList(HUINT32 *pulNumOfList, DxAudioCodec_e **ppeAudioCodecList);
HBOOL PAL_HDMI_IsPlugedIn(void);

/* hdcp handling */
HERROR PAL_HDMI_SetHdcp(HBOOL bEnable);
void PAL_HDMI_GetHdcp(HBOOL *pbEnable);

HERROR PAL_HDMI_RegisterCallback(void (*hdmiCallbackFunc)(PalHdmi_Event_e eHdmiEvent));

HERROR PAL_HDMI_CheckVideoTimingAvailable(VIDEO_Timing_t stTiming);
HERROR PAL_HDMI_CheckAudioCodecAvailable(DxAudioCodec_e eAudioCodec);
HERROR PAL_HDMI_SetAviInfoFrame(DxAspectRatio_e eAspectRatio, VIDEO_Afd_t eAfd);

HERROR PAL_HDMI_Get3DFormatCapability(DxVideo3DFormat_b *pulHdmi3DCapa);

/*******************************************************************/


#endif	//__PAL_VIDEO_H__


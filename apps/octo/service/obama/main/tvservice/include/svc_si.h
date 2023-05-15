/*******************************************************************
	File Description
********************************************************************/
/**
	@file	  svc_si.h
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

#ifndef __SVC_SI__
#define __SVC_SI__

/*******************************************************************/
/********************      Header Files    *************************/
/*******************************************************************/
#include <silib.h>
#include <octo_common.h>
#include <db_svc.h>

/*******************************************************************/
/********************      Defines         *************************/
/*******************************************************************/
#define SvcSi_SUPPORT_LIB_ES_NEW

#define EVT_CPNT_VID_MPEG2				0x00000001
#define EVT_CPNT_VID_H264				0x00000002
#define EVT_CPNT_VID_ASPECT_4_3			0x00000004
#define EVT_CPNT_VID_ASPECT_16_9		0x00000008
#define EVT_CPNT_VID_HD					0x00000010
#define EVT_CPNT_VID_30HZ				0x00000020
#define EVT_CPNT_VID_25HZ				0x00000040
#define EVT_CPNT_VID_1080I				0x00000080
#define EVT_CPNT_VID_576I				0x00000100

#define EVT_CPNT_AUD_SINGLEMONO			0x00000001
#define EVT_CPNT_AUD_DUALMONO			0x00000002
#define EVT_CPNT_AUD_STEREO				0x00000004
#define EVT_CPNT_AUD_MULTILINGUAL		0x00000008
#define EVT_CPNT_AUD_SURROUND			0x00000010
#define EVT_CPNT_AUD_DOLBY				0x00000020
#define EVT_CPNT_AUD_MULTI_TRACK		0x00000040

#define EVT_CPNT_SUBTITLE				0x00000001
#define EVT_CPNT_TELETEXT				0x00000002

/** @brief one TP에서 찾을 수 있는 개수 */
#define SvcSi_MAX_NUM_SVC								50		//?? temp
#define SvcSi_MAX_NUM_PPVSVC							15		//?? temp
#define SvcSi_MAX_NUM_TS								1
#define SvcSi_MAX_NUM_PROVIDERS							10
#define SvcSi_MAX_NUM_NETWORKS							2
#define SvcSi_MAX_NUM_BOUQUETS							5
#define SvcSi_MAX_NUM_DELI_TS							20
#define SvcSi_MAX_NUM_LCN								25
#define SvcSi_MAX_NUM_LCD_LIST							5

#define SvcSi_AUDIO_CHANNEL_NUM_NA						0x00
#define SvcSi_AUDIO_CHANNEL_NUM_STEREO					0x01
#define SvcSi_AUDIO_CHANNEL_NUM_MULTI					0x02

#define SvcSi_SUBTITLE_INDEX_NULL						0xff			/* Search 후 초기 값이기도 하며 Off 항목을 의미하기도 한다. */

// Search
#define SvcSi_NETID_SEARCH_NITACT						0x30000			// NIT ACT 만을 Network Search 할 경우의 Net ID
#define SvcSi_NETID_SEARCH_NOTNIT						0x40000			// Network Search를 하지 않을 경우의 Net ID

// NIT
#define SvcSi_NIT_ACT_HEAD								0x00
#define SvcSi_NIT_OTH_HEAD								0x01

#define SvcSi_MAX_SUBTITLE_NUM							12				/* SvcSi_MAX_SUBTTL_ES_NUM 의 ES 개수와 맞춘다. */

#define SvcSi_MAX_AUD_NUM 								4
#define SvcSi_MAX_AUD_NAME_LEN							50

#define SvcSi_MAX_SHORT_EVENT_NAME						256
#define SvcSi_MAX_EVENT_TEXT							256

// SWUP Type :
#define SvcSi_SWUP_TYPE_USER_SELECT						0				// User Select
#define SvcSi_SWUP_TYPE_FORCED							1				// Forced SWUP
#define SvcSi_SWUP_TYPE_BURST_USER_SELECT				3
#define SvcSi_SWUP_TYPE_BURST_FORCED					4

// ES Num
#define	SvcSi_MAX_VIDEO_ES_NUM							4		/* 개수 변경 되면 MAX_VIDEO_NUM도 필 변경 바람. */
#define	SvcSi_MAX_AUDIO_ES_NUM							16		/* 개수 변경 되면 MAX_AUDIO_NUM도 필 변경 바람. */
#define	SvcSi_MAX_SUBTTL_ES_NUM							12		/* 개수 변경 되면 MAX_SUBTTL_NUM도 필 변경 바람. */
#define	SvcSi_MAX_TELTXT_ES_NUM							8		/* 개수 변경 되면 MAX_TTX_INITAIL_PAGE도 필 변경 바람. */
#define	SvcSi_MAX_CAPTION_ES_NUM						2		/* Super 포함 */
#define	SvcSi_MAX_DATA_ES_NUM							12		/* TR-B14 제3편 2.1.2.5 ES수의 제한 */
#define	SvcSi_MAX_CA_PID_NUM							20


//burst update
#define	SvcSi_MAX_SCHUDULE								3		/* BURST UPDATE SCHEDULE 저장 최대 갯수 */

#define	SvcSi_ES_AUDIO_IS_AD_TYPE(eAudType)									(eAUDIO_TYPE_VISUAL_IMPAIRED == eAudType)
#if defined(CONFIG_OP_NORDIG)
#define	SvcSi_ES_AUDIO_IS_AD_BM_TYPE(eAudType, ucMixType, eSubAudType)		(((eAUDIO_TYPE_VISUAL_IMPAIRED == eAudType) && ((ucMixType == 0xff) && (eAUDIO_TYPE_SUPP_UNKNOWN == eSubAudType))) || ((eAUDIO_TYPE_VISUAL_IMPAIRED == eAudType) || ( eAUDIO_TYPE_UNDEFINED == eAudType) || (eAUDIO_TYPE_CLEAN_EFFECTS == eAudType)) && ( ((ucMixType == 1) && (eAUDIO_TYPE_SUPP_AD_VISUAL_IMPARED == eSubAudType))))
#define	SvcSi_ES_AUDIO_IS_AD_RM_TYPE(eAudType, ucMixType, eSubAudType)		(((eAUDIO_TYPE_VISUAL_IMPAIRED == eAudType) && ((ucMixType == 0) && (eAUDIO_TYPE_SUPP_AD_VISUAL_IMPARED == eSubAudType))))
#else
#define	SvcSi_ES_AUDIO_IS_AD_BM_TYPE(eAudType, ucMixType, eSubAudType)		((eAUDIO_TYPE_VISUAL_IMPAIRED == eAudType) || (((eAUDIO_TYPE_VISUAL_IMPAIRED == eAudType) || ( eAUDIO_TYPE_UNDEFINED == eAudType) || (eAUDIO_TYPE_CLEAN_EFFECTS == eAudType)) && ((ucMixType == 1) && (eAUDIO_TYPE_SUPP_AD_VISUAL_IMPARED == eSubAudType))))
#define	SvcSi_ES_AUDIO_IS_AD_RM_TYPE(eAudType, ucMixType, eSubAudType)		((eAUDIO_TYPE_VISUAL_IMPAIRED == eAudType) || ((eAUDIO_TYPE_VISUAL_IMPAIRED == eAudType) && ((ucMixType == 0) && (eAUDIO_TYPE_SUPP_AD_VISUAL_IMPARED == eSubAudType))))
#endif
#define	SvcSi_ES_AUDIO_IS_DOLBY_TYPE(eAudType)								((eDxAUDIO_CODEC_DOLBY_AC3 == eAudType) || (eDxAUDIO_CODEC_DOLBY_AC3P == eAudType))
#define	SvcSi_ES_AUDIO_IS_DTS_MULTICHANNEL_TYPE(eAudType)					((eDxAUDIO_CODEC_DTS == eAudType) || (eDxAUDIO_CODEC_DTS_HD == eAudType))
#define	SvcSi_ES_AUDIO_IS_AAC_MULTICHANNEL_TYPE(eAudType)					(eDxAUDIO_CODEC_AAC_PLUS == eAudType)
#define	SvcSi_ES_AUDIO_IS_MULTICHANNEL_TYPE(eAudType)						(SvcSi_ES_AUDIO_IS_DOLBY_TYPE(eAudType) || SvcSi_ES_AUDIO_IS_DTS_MULTICHANNEL_TYPE(eAudType) || SvcSi_ES_AUDIO_IS_AAC_MULTICHANNEL_TYPE(eAudType))

#define SvcSi_OrgSvcType_IS_DATA(eSvcType)	(eSvcType == eDxOrgSVC_TYPE_DATA_BROADCAST)
#if defined(CONFIG_DB_SVC_TYPE_HD)
#define SvcSi_OrgSvcType_IS_TV(eSvcType)	((eSvcType == eDxOrgSVC_TYPE_DIGITAL_TV) || (eSvcType == eDxOrgSVC_TYPE_AVC_SD_DIGITAL_TV) || (eSvcType == eDxOrgSVC_TYPE_MPEG2_HD) || (eSvcType == eDxOrgSVC_TYPE_AVC_HD_DIGITAL_TV) || (eSvcType == eDxOrgSVC_TYPE_AVC_HD_DIGITAL_TV2))
#else
#define SvcSi_OrgSvcType_IS_TV(eSvcType)	(eSvcType ==eDxOrgSVC_TYPE_DIGITAL_TV)
#endif

// Subtitle
#define	SvcSi_DVB_MIN_NORMAL_SUBTILE		(0x10)
#define	SvcSi_DVB_MAX_NORMAL_SUBTILE		(0x14)					/* 300.468 2008.11월 기준 0x24가 추가.*/
#define	SvcSi_DVB_MIN_HOH_SUBTITLE			(0x20)
#define	SvcSi_DVB_MAX_HOH_SUBTITLE			(0x24)					/* 300.468 2008.11월 기준 0x24가 추가.*/
#define	SvcSi_DVB_OPEN_DEAF_SUBTITLE		(0x30)					/* 언제 사용하는지 모름. */
#define	SvcSi_DVB_CLOSED_DEAF_SUBTITLE		(0x31)					/* 언제 사용하는지 모름. */

#define	SvcSi_SUBTITLE_TYPE_NORMAL_NO_ASPECT			(0x10)
#define	SvcSi_SUBTITLE_TYPE_NORMAL_4_3_ASPECT			(0x11)
#define	SvcSi_SUBTITLE_TYPE_NORMAL_16_9_ASPECT			(0x12)
#define	SvcSi_SUBTITLE_TYPE_NORMAL_221_1_ASPECT			(0x13)
#define	SvcSi_SUBTITLE_TYPE_NORMAL_HD					(0x14)
#define	SvcSi_SUBTITLE_TYPE_HARD_HEAR_NO_ASPECT			(0x20)
#define	SvcSi_SUBTITLE_TYPE_HARD_HEAR_4_3_ASPECT		(0x21)
#define	SvcSi_SUBTITLE_TYPE_HARD_HEAR_16_9_ASPECT		(0x22)
#define	SvcSi_SUBTITLE_TYPE_HARD_HEAR_221_1_ASPECT		(0x23)
#define	SvcSi_SUBTITLE_TYPE_HARD_HEAR_HD				(0x24)

#define	SvcSi_IsEBUSubtitle(ucType) 		(ucType <= 0x05)
#define	SvcSi_IsDVBSubtitle(ucType)			(ucType >= SvcSi_DVB_MIN_NORMAL_SUBTILE && ucType <= SvcSi_DVB_MAX_NORMAL_SUBTILE)
#define	SvcSi_IsHOHSubtitle(ucType)			(ucType >= SvcSi_DVB_MIN_HOH_SUBTITLE && ucType <= SvcSi_DVB_MAX_HOH_SUBTITLE)
#define	SvcSi_IsHDSubtitle(ucType)			(ucType == SvcSi_SUBTITLE_TYPE_NORMAL_HD || ucType == SvcSi_SUBTITLE_TYPE_HARD_HEAR_HD)
#define	SvcSi_IsSubtitle4x3(ucType)			(ucType == SvcSi_SUBTITLE_TYPE_NORMAL_4_3_ASPECT || ucType == SvcSi_SUBTITLE_TYPE_HARD_HEAR_4_3_ASPECT)
#define	SvcSi_IsSubtitle16x9(ucType)		(ucType == SvcSi_SUBTITLE_TYPE_NORMAL_16_9_ASPECT || ucType == SvcSi_SUBTITLE_TYPE_HARD_HEAR_16_9_ASPECT)
#define	SvcSi_IsSubtitle211x1(ucType)		(ucType == SvcSi_SUBTITLE_TYPE_NORMAL_221_1_ASPECT || ucType == SvcSi_SUBTITLE_TYPE_HARD_HEAR_221_1_ASPECT)
#define	SvcSi_IsSubtitleNoAR(ucType)		(ucType == SvcSi_SUBTITLE_TYPE_NORMAL_NO_ASPECT || ucType == SvcSi_SUBTITLE_TYPE_HARD_HEAR_NO_ASPECT)

/*******************************************************************/
/********************      Data Type (enum)         *************************/
/*******************************************************************/
/** @brief SI event messages */
enum
{
	eSEVT_SI_ = eSEVT_SI_START,

	/* - Description : Stop을 요청한 action에게 session이 stop되었다고 통보하는 이벤트 메시지.
	   - Parameters Usage :
		   handle : Action handle.
		   p1 : TBD
		   p2 : TBD
		   p3 : TBD */
	eSEVT_SI_STOPPED,

	/* - Description : PAT가 없거나 중단되었음을 알리는 이벤트 메시지.
	   - Parameters Usage :
		   handle : Action handle.
		   p1 : TBD
		   p2 : TBD
		   p3 : TBD */
	eSEVT_SI_PAT_TIMEOUT,

	/* - Description : PMT 처음 도착 또는 변경되었음을 알리는 이벤트 메시지.
	   - Parameters Usage :
		   handle : Action handle.
		   p1 : TBD
		   p2 : TBD
		   p3 : TBD */
	eSEVT_SI_PMT,


	/* - Description : PMT가 없거나 중단되었음을 알리는 이벤트 메시지.
	   - Parameters Usage :
		   handle : Action handle.
		   p1 : TBD
		   p2 : TBD
		   p3 : TBD */
	eSEVT_SI_PMT_TIMEOUT,

	/* - Description : PMT PID가 변경 되는 경우 알리는 이벤트 메시지.
	   - Parameters Usage :
		   handle : Action handle.
		   p1 : TBD
		   p2 : TBD
		   p3 : TBD */
	eSEVT_SI_PMT_PIDCHANGED,

	/* - Description : PAT에서 PMT PID가 삭제되는 경우 알리는 이벤트 메시지.
	   - Parameters Usage :
		   handle : Action handle.
		   p1 : TBD
		   p2 : TBD
		   p3 : TBD */
	eSEVT_SI_PMT_PIDREMOVED,

	/* - Description : SDT가 업데이트 되었음을 알리는 이벤트 메시지
	   - Parameters Usage :
		   handle : Action handle.
		   p1 : TBD
		   p2 : TBD
		   p3 : TBD */
	eSEVT_SI_SDT,

	/* - Description :SDT가 없거나 중단되었음을 알리는 이벤트 메시지.
	   - Parameters Usage :
		   handle : Action handle.
		   p1 : TBD
		   p2 : TBD
		   p3 : TBD */
	eSEVT_SI_SDT_TIMEOUT,

	/* - Description : BAT 처음 도착 또는 변경되었음을 알리는 이벤트 메시지.
	   - Parameters Usage :
		   handle : Action handle.
		   p1 : TBD
		   p2 : TBD
		   p3 : TBD */
	eSEVT_SI_BAT,

	/* - Description : BAT가 없거나 중단되었음을 알리는 이벤트 메시지.
	   - Parameters Usage :
		   handle : Action handle.
		   p1 : TBD
		   p2 : TBD
		   p3 : TBD */
	eSEVT_SI_BAT_TIMEOUT,

	/* - Description : tdt가 최초 받거나 update되는 경우를 알리는 이벤트 메시지
	   - Parameters Usage :
		   handle : Action handle.
		   p1 : TBD
		   p2 : TBD
		   p3 : TBD */
	eSEVT_SI_TDT,

	/* - Description :TDT가 없거나 중단되었음을 알리는 이벤트 메시지.
	   - Parameters Usage :
		   handle : Action handle.
		   p1 : TBD
		   p2 : TBD
		   p3 : TBD */
	eSEVT_SI_TDT_TIMEOUT,

	/* - Description : tot가 최초 받거나 update되는 경우를 알리는 이벤트 메시지
	   - Parameters Usage :
		   handle : Action handle.
		   p1 : TBD
		   p2 : TBD
		   p3 : TBD */
	eSEVT_SI_TOT,

	/* - Description :TOT가 없거나 중단되었음을 알리는 이벤트 메시지.
	   - Parameters Usage :
		   handle : Action handle.
		   p1 : TBD
		   p2 : TBD
		   p3 : TBD */
	eSEVT_SI_TOT_TIMEOUT,

	/* - Description : EIT처음 도착 또는 변경되었음을 알리는 이벤트 메시지.
	   - Parameters Usage :
		   handle : Action handle.
		   p1 : TBD
		   p2 : TBD
		   p3 : TBD */
	eSEVT_SI_EIT,

	/* - Description : EIT가 없거나 중단되었음을 알리는 이벤트 메시지.
	   - Parameters Usage :
		   handle : Action handle.
		   p1 : TBD
		   p2 : TBD
		   p3 : TBD */
	eSEVT_SI_EIT_TIMEOUT,

	/* - Description : AIT처음 도착 또는 변경되었음을 알리는 이벤트 메시지.
	   - Parameters Usage :
		   handle : Action handle.
		   p1 : TBD
		   p2 : TBD
		   p3 : TBD */
	eSEVT_SI_AIT,
        /* - Description : AIT가 없을 경우 발생하는 이벤트 메세지.
           - Parameters Usage :
                   handle : Action handle.
		   p1 : TBD
		   p2 : TBD
		   p3 : TBD */
	eSEVT_SI_NONE_AIT,


	/* - Description : AIT가 없거나 중단되었음을 알리는 이벤트 메시지.
	   - Parameters Usage :
		   handle : Action handle.
		   p1 : TBD
		   p2 : TBD
		   p3 : TBD */
	eSEVT_SI_AIT_TIMEOUT,

	/* - Description : Freesat BAT 처음 도착 또는 변경되었음을 알리는 이벤트 메시지.
	   - Parameters Usage :
		   handle : Action handle.
		   p1 : TBD
		   p2 : TBD
		   p3 : TBD */
	eSEVT_SI_T_BAT,


	/* - Description : One TP search가 Success로  종료 되었을 때 알리는 이벤트 메시지.
	   - Parameters Usage :
		   handle : Action handle.
		   p1 : TBD
		   p2 : TBD
		   p3 : TBD */
	eSEVT_SI_SEARCH_DONE,

	/* - Description : One TP search가 Fail 종료 되었을 때 알리는 이벤트 메시지.
	   - Parameters Usage :
		   handle : Action handle.
		   p1 : TBD
		   p2 : TBD
		   p3 : TBD */
	eSEVT_SI_SEARCH_FAIL,

	/* - Description : SDT free CA mode 변경에 따른 replacement service 요청 이벤트.
	   * WTV에서 이 이벤트를 받으면 SDT의 free_CA_mode가 조건에 부합되고 linkage 정보가 있다는 것을 의미하며 다음과 같은 동작을 해야 한다.
		1) origin 일 때 : AV 상태를 체크. AV가 available하면 이후 처리 없음. 만약 AV가 available하지 않으면 전달받은 linkage 서비스로 전환.
		2) Replacement 일 때 : origin 서비스로 복귀

	   * notify의 조건 :
		1) origin일 때 : free_CA_mode가 1이고 해당 linkage 정보가 있는 경우에만.
		2) replacement 일 때: origin의 free_CA_mode가 0일 때.

	   - Parameters Usage :
		   handle : Action handle.
		   p1 : Replacement service handle or NULL(org 복귀인 경우)
		   p2 : TBD
		   p3 : TBD */
	eSEVT_SI_SDT_FREE_CA_MODE_CHANGED,

	/* - Description : SDT의 running status 변경에 따른 replacement service 요청 이벤트.

		* WTV에서 이 이벤트를 받으면 SDT의 running_status가 조건에 부합되고 linkage 정보가 있다는 것을 의미하며 다음과 같은 동작을 해야 한다.
		1) Origin 일 때 : 전달받은 linkage 서비스로 전환
		2) Replacement 일 때 : origin 서비스로 복귀

		* eSEVT_SI_SDT_RUNNING_STATUS_CHANGED notify의 조건 :
		1) Origin일 때 : running_status가 not running이고 해당 linkage 정보가 있는 경우에만.
		2) Replacement 일 때: origin의 running_status가 running일 때.

	   - Parameters Usage :
		   handle : Action handle.
		   p1 : Replacement service handle or NULL(org 복귀인 경우)
		   p2 : [SvcSi_SdtRunningStatusProcessType_e] 향별로 차이나는 spec 종류를 지정한다.
		   p3 : [SvcSi_SdtRunningStatusType_e] p2에 따라 running status 값이 들어갈 수도 있다.  */
	eSEVT_SI_SDT_RUNNING_STATUS_CHANGED,

 	/* - Description : AP에서 StartTime으로 Time Receiving을 걸었을 경우 그 작업이 성공 혹은 실패해서 끝났음을 알려주는 메시지
	   - Parameters Usage :
		   handle : TBD
		   p1 : bRcvTime : time을 받았으면 TRUE, 못 받았으면 False
		   p2 : TBD
		   p3 : TBD */
	eSEVT_SI_TIME_RECEIVING_FINISHED,

	// AP에서 StartTime으로 Time Receiving을 걸었을 경우 그 작업이 성공 혹은 실패해서 끝났음을 알려주는 메시지
	eSEVT_SI_MULTIFEED_DETECTED,


	// SI에서 Descriptor 준비 되었을 때
	eSEVT_SI_DESCRIPTOR_SUCCESS,

	// Service Move Detected
	eSEVT_SI_SERVICE_MOVE,					// p1 : orgSvcHandle p2 : newSvcHandle



	// ----------------------------------------
	// DRM 명령을 위한 SI Message
	eSEVT_SI_DRM_SIGNALLED,				// p1 : DrmModelType_t,   p2, p3 : 각자 사양대로 가져 가기로 한다. (논의 필요)

	// NIT Changed Notify
	eSEVT_SI_NETWORK_CHANGED_NOTIFY,


	// KDG 사양 (live 에서 NIT monitoring 중에 update_id 변경사항을 전달
	eSEVT_SI_UPDATE_ID_CHANGED,

	/* - Description : IP Streaming PMT 처음 도착 또는 변경되었음을 알리는 이벤트 메시지.
	   - Parameters Usage :
		   handle : Action handle.
		   p1 : TBD
		   p2 : TBD
		   p3 : TBD */
	eSEVT_SI_IP_STREAMING_PMT,


	// ----------------------------------------
	// Japan Ch Masking 관련
	eSEVT_SI_NIT,
	eSEVT_SI_NIT_TIMEOUT,

	// ----------------------------------------
	// Time Zone 변경 관련
	eSEVT_SI_TIMEZONE_CHANGED,



	/* - Description : Dynamic update가 Success로  종료 되었을 때 알리는 이벤트 메시지.
	   - Parameters Usage :
		   handle : Action handle.
		   p1 : TBD
		   p2 : TBD
		   p3 : TBD */
	eSEVT_SI_DYNAMIC_UPDATE_DONE,

	/* - Description : Dynamic update가 Fail 종료 되었을 때 알리는 이벤트 메시지.
	   - Parameters Usage :
		   handle : Action handle.
		   p1 : TBD
		   p2 : TBD
		   p3 : TBD */
	eSEVT_SI_DYNAMIC_UPDATE_FAIL,


	/* - Description :
	   - Parameters Usage :
		   handle : Action handle.
		   p1 : TBD
		   p2 : TBD
		   p3 : TBD */
	eSEVT_SI_CAT,

	/* - Description :
	   - Parameters Usage :
		   handle : Action handle.
		   p1 : TBD
		   p2 : TBD
		   p3 : TBD */
	eSEVT_SI_CAT_TIMEOUT,


	// FTA Content Management Signaling.
	// It can be from NIT, SDT, EIT (This is UK specification), The other place have to reade their specification.
	/*- Parameters Usage :
		handle : Action handle.
		p1 : usTsIdx or NetIdx
		p2 :ulFtaContentValue
		p3 : TBD */
	eSEVT_SI_FTA_CONTENTS_SIGNAL,

	/* - Description : RCT처음 도착 또는 변경되었음을 알리는 이벤트 메시지.
	   - Parameters Usage :
		   handle : Action handle.
		   p1 : TBD
		   p2 : TBD
		   p3 : TBD */
	eSEVT_SI_RCT,

	/* - Description : RCT가 없거나 중단되었음을 알리는 이벤트 메시지.
	   - Parameters Usage :
		   handle : Action handle.
		   p1 : TBD
		   p2 : TBD
		   p3 : TBD */
	eSEVT_SI_RCT_TIMEOUT,

        /* - Description : RCT가 없을 경우 발생하는 이벤트 메세지.
           - Parameters Usage :
                   handle : Action handle.
		   p1 : TBD
		   p2 : TBD
		   p3 : TBD */
	eSEVT_SI_RCT_NONE,

	/* - Description : DSMCC 로 들어오는 RCT Image 를 download 하라고 mheg에 알림.
	   - Parameters Usage :
		handle : Action handle
	   	p1 : url
	   	p2 : TBD
	   	p3 : TBD */
	eSEVT_SI_REQUEST_RCT_IMAGE_BY_MHEG,

        /* - Description : NCD dialog를 표시할 시간을 알리는 이벤트 메세지
           - Parameters Usage :
                   handle : TBD
		   p1 : Schedule Start Time
		   p2 : TBD
		   p3 : TBD */
	eSEVT_SI_UPDATE_NCD_SCHEDULE,

    /* - Description : Notify network uri prefix is changed
           - Parameters Usage :
                   handle : TBD
		   p1 : TBD
		   p2 : TBD
		   p3 : TBD */
	eSEVT_SI_NETWORK_URI_PREFIX_CHANGED,

	// ----------------------------------------
	eSEVT_SI_END /* enum 끝 표시. 사용하는 메시지 아님. */
};

// Module Action Kind
typedef enum
{
	eSVCSI_ACT_KIND_None = 0,
	eSVCSI_ACT_KIND_Live,			// Live, Live Swup Check
	eSVCSI_ACT_KIND_Search,			// Search
	eSVCSI_ACT_KIND_Record,			// Record
	eSVCSI_ACT_KIND_Playback, 		// Playback
	eSVCSI_ACT_KIND_MediaPlayback, 	// Media playback
	eSVCSI_ACT_KIND_ExtSearch,		// Ext Search
	eSVCSI_ACT_KIND_SatRecord,		// Sat Record
	eSVCSI_ACT_KIND_SwupCheck,		// Swup Check (For Menu)
	eSVCSI_ACT_KIND_LiveSwupCheck,	// Swup Check (For Live)
	eSVCSI_ACT_KIND_TimeUpdate,		// Time Update,
	eSVCSI_ACT_KIND_DynamicUpdate,	// Dynamic update
	eSVCSI_ACT_KIND_Download,		// Downlaod Data
	eSVCSI_ACT_KIND_HomeTpEpg,		// Barker EPG
	eSVCSI_ACT_KIND_Satip,			// SAT>IP
	eSVCSI_ACT_KIND_HbbTv,			// HBB-TV

	// 필요한 Action은 아래에 추가하세요...
	eSVCSI_ACT_KIND_MAX
} SvcSi_ActionKind_e;

typedef enum
{
	eSVCSI_ACT_SPEC_None = 0,
} SvcSi_ActionSpec_e;

// Module Action Kind
typedef enum
{
	eSVCSI_ES_SELECT_Normal,
	eSVCSI_ES_SELECT_Freesat,
	eSVCSI_ES_SELECT_Ukdtt,
	eSVCSI_ES_SELECT_Nordig,

	// 필요한 방식은 위에 추가하세요...
	eSVCSI_ES_SELECT_MAX
} SvcSi_EsSelect_e;

// Time Update
typedef enum
{
	eSVCSI_TIME_UPDATE_None,

	eSVCSI_TIME_UPDATE_Normal,
	eSVCSI_TIME_UPDATE_OnlyFirstTp,
	eSVCSI_TIME_UPDATE_MiddelEast,		// prefix된 몇개의 tp에서만 update
	eSVCSI_TIME_UPDATE_EU,
	eSVCSI_TIME_UPDATE_Ukdtt,

	eSVCSI_TIME_UPDATE_EndOfCase
} SvcSi_TimeUpdate_e;

/*
	For Search
*/
typedef enum
{
	eSVCSI_SEARCHTYPE_None					= 0,

	/* base */
	eSVCSI_SEARCHTYPE_Normal,
	eSVCSI_SEARCHTYPE_PidSearch,

	/* ertelecom */
	eSVCSI_SEARCHTYPE_ERTELECOM_Search,

	/* nordig */
	eSVCSI_SEARCHTYPE_NORDIG_NitActBased,
	eSVCSI_SEARCHTYPE_NORDIG_NitOthBased,
	eSVCSI_SEARCHTYPE_NORDIG_WithoutNit,
	eSVCSI_SEARCHTYPE_NORDIG_NitActBased_DynamicUpdate,

	/* ses */
	eSVCSI_SEARCHTYPE_SES_Start				= 0x300,
	eSVCSI_SEARCHTYPE_SES_LCN_HOME_TP,					// Home TP
	eSVCSI_SEARCHTYPE_SES_LCN_REFNIT_TP,				// Reference NIT TP
	eSVCSI_SEARCHTYPE_SES_LCN_SGT_TP,					// SGT TP
	eSVCSI_SEARCHTYPE_SES_ASTRA_PORTAL_REFNIT_TP,		// Astra Portal Reference NIT TP
	eSVCSI_SEARCHTYPE_SES_ASTRA_PORTAL_TP,				// Astra Portal TP
	eSVCSI_SEARCHTYPE_SES_ASTRA_REGIONAL_PORTAL_TP, 	// Astra Regional TP
	eSVCSI_SEARCHTYPE_SES_End				= 0x3FF,

	eSVCSI_SEARCHTYPE_EndOfCase
} SvcSi_SearchType_e;

typedef enum
{
	eSVCSI_SEARCH_RESULT_None				= 0,
	eSVCSI_SEARCH_RESULT_Success,
	eSVCSI_SEARCH_RESULT_Failed
} SvcSi_SearchResult_e;

typedef enum
{
	eSI_SRCH_INFOTYPE_Bouquet,			// DbSvc_BouqInfo_t
	eSI_SRCH_INFOTYPE_Network,			// DbSvc_NetInfo_t
	eSI_SRCH_INFOTYPE_Transponder,		// DbSvc_TsInfo_t
	eSI_SRCH_INFOTYPE_Service,			// DbSvc_Info_t
	eSI_SRCH_INFOTYPE_Provider,			// DbSvc_PrvInfo_t
	eSI_SRCH_INFOTYPE_Delivery,			// DbSvc_TsInfo_t
	eSI_SRCH_INFOTYPE_Group,			// DbSvc_GroupInfo_t

	eSI_SRCH_INFOTYPE_MAX
} SvcSi_SrchInfoType_e;

/*
	For SatRecord
*/
typedef enum
{
	eSVCSI_SATRECORDTYPE_None					= 0,

	eSVCSI_SATRECORDTYPE_Normal,

	eSVCSI_SATRECORDTYPE_SES_Start 			= 0x100,
	eSVCSI_SATRECORDTYPE_SES_HOME_TP,
	eSVCSI_SATRECORDTYPE_SES_REFNIT_TP,
	eSVCSI_SATRECORDTYPE_SES_SATREC_TP,
	eSVCSI_SATRECORDTYPE_SES_End			= 0x1FF,

	eSVCSI_SATRECORDTYPE_EndOfCase
} SvcSi_SatRecordType_e;

typedef struct
{
	HUINT32				ulStartSystemId;
	HUINT32				ulEndSystemId;
	HUINT16				usHdfVer;
	VersionType_t			eDataType;		// 시그널링 온 Data Type : eVERSION_TYPE_LOADER, eVERSION_TYPE_APPL,  eVERSION_TYPE_EEPROM 등
	HUINT16				usDataType;
	HUINT32				ulDataVersion;
	HUINT8				ucSwupType;		// Forced, Normal
	HUINT16				usSvcId;
	HUINT16				usPid;

//  이 이후 추가되는 것은 계속 밑으로 추가한다.
// DB에 저장되는 항목이므로 위의 순서를 바꾸거나 하는 행위는 절대 있어선 안 된다.
// APS
//	HUINT16				usConditionId;		//[2014.2.7. ymseo]현재 사용하지 않는 것으로 보임. 향후 삭제 예정.
// UPC
	HUINT8				ucUsage;
// IR
#if defined(CONFIG_OTA_IRDETO)
	HUINT8				ucTableId;
#if defined(CONFIG_OTA_IRDETO_LOADER_V5)
	HUINT16				usDlownloadSeqNumber;
#else
	HUINT8				ucLoadSeqNumber;
	HxDATETIME_Time_t	stDownloadTime;
#endif
#endif
// 추가..
} SvcSi_SwupSignal_t;

/** SWUP Retrieve Mode : 특정 SWUP 중에서도 TP나 상황에 따라 받아야 하는 SI Table, 정책 등이 약간씩 달라질 수 있다.   */
/* 이 항목은 그러한 부분 Mode를 나누는 항목이다. */

/** EXT Search Data Type : SVC_SI_GetExtSearchData 를 통해서 SI Data를 가져 올 경우 필요한 종류를 지정하는 enum. */
typedef enum
{
	eSVCSI_EXTSEARCH_DATA_None					= 0,

	// 여기서 부터는 OP / SPEC 에 따르므로 여기에 선언하지 않는다.
	// 다만 영역만 만들어 준다. -> 정리 필요..
	eSVCSI_EXTSEARCH_DATA_SES_Start 			= 0x100,
	eSVCSI_EXTSEARCH_DATA_SES_DELIVERY_TSINFO,					// NIT 에 DeliverySystem Des를 통해 오는 SvcTsInfo (On/TsId + TuningInfo)
	eSVCSI_EXTSEARCH_DATA_SES_REFNITLINK_TRIPLEID,				// NIT 에 존재하는 Reference NIT LINKAGE Triple ID -> to REF-NIT
	eSVCSI_EXTSEARCH_DATA_SES_SGTLINK_TRIPLEID,					// NIT 에 존재하는 SGT Linkage Triple ID -> to SGT TP
	eSVCSI_EXTSEARCH_DATA_SES_SVCLISTINFO,						// SES LCN 으로 찾은 Service List
	eSVCSI_EXTSEARCH_DATA_SES_SERVICE_SVCINFO,					// SES LCN 으로 찾은 Services
	eSVCSI_EXTSEARCH_DATA_SES_TS_TSINFO,						// SES LCN 으로 찾은 TS
	eSVCSI_EXTSEARCH_DATA_SES_BOUQUET_BQINFO,					// SES LCN 으로 찾은 Bouquet
	eSVCSI_EXTSEARCH_DATA_SES_NETWORK_NETINFO,					// SES LCN 으로 찾은 Network
	eSVCSI_EXTSEARCH_DATA_SES_ASTRA_PORTAL_TS_INFO,				// Astra Portal TS Info
	eSVCSI_EXTSEARCH_DATA_SES_ASTRA_REGIONAL_PORTAL_TS_INFO,	// Astra Regional Portal TS Info
	eSVCSI_EXTSEARCH_DATA_SES_PORTAL_INFO,						// Portal Svc Info
	eSVCSI_EXTSEARCH_DATA_SES_End 				= 0x1FF,

	eSVCSI_EXTSEARCH_DATA_EndOfCase
} SvcSi_ExtSrchDataType_t;

typedef enum
{
	eSVCSI_EXTSEARCH_OPTION_None							= 0,

	// 여기서 부터는 OP / SPEC 에 따르므로 여기에 선언하지 않는다.
	// 다만 영역만 만들어 준다. -> 정리 필요..
	eSVCSI_EXTSEARCH_OPTION_SES_Start						= 0x100,
	eSVCSI_EXTSEARCH_OPTION_SES_SVCLISTID,					// Service List ID 설정 : p1=Service List ID, p2, p3 = N/A
	eSVCSI_EXTSEARCH_OPTION_SES_End							= 0x1FF,

	eSVCSI_EXTSEARCH_OPTION_EndOfCase
} SvcSi_ExtSrchOptionType_t;

/** Sat Record Data Type : SVC_SI_GetSatRecordData 를 통해서 SI Data를 가져 올 경우 필요한 종류를 지정하는 enum. */
typedef enum
{
	eSVCSI_SATRECORD_DATA_None					= 0,

	// 여기서 부터는 OP / SPEC 에 따르므로 여기에 선언하지 않는다.
	// 다만 영역만 만들어 준다. -> 정리 필요..
	eSVCSI_SATRECORD_DATA_SES_Start 			= 0x100,
	eSVCSI_SATRECORD_DATA_SES_DELIVERY_TSINFO,					// NIT 에 DeliverySystem Des를 통해 오는 SvcTsInfo (On/TsId + TuningInfo)
	eSVCSI_SATRECORD_DATA_SES_REFNITLINK_TRIPLEID,				// NIT 에 존재하는 Reference NIT LINKAGE Triple ID -> to REF-NIT
	eSVCSI_SATRECORD_DATA_SES_ASTRA_RR_TRIPLEID,				// BAT에 존재하는 SAT RECORD TS의 Triple ID
	eSVCSI_SATRECORD_DATA_SES_HDPLUS_RR_TRIPLEID,				// BAT에 존재하는 SAT RECORD TS의 Triple ID
	eSVCSI_SATRECORD_DATA_SES_SATREC_DSMCCINFO,					// PMT에 존재하는 SAT RECORD DSMCC INFO
	eSVCSI_SATRECORD_DATA_SES_End 				= 0x1FF,

	eSVCSI_SATRECORD_DATA_EndOfCase
} SvcSi_SatRecordDataType_t;

typedef enum
{
	eSVCSI_SATRECORD_OPTION_None							= 0,

	// 여기서 부터는 OP / SPEC 에 따르므로 여기에 선언하지 않는다.
	// 다만 영역만 만들어 준다. -> 정리 필요..
	eSVCSI_SATRECORD_OPTION_SES_Start						= 0x100,
	eSVCSI_SATRECORD_OPTION_SES_End							= 0x1FF,

	eSVCSI_SATRECORD_OPTION_EndOfCase
} SvcSi_SatRecordOptionType_t;

/**
  *		Section Composing (Making) 기능을 위한 설정들
**/

typedef enum
{
	eSVCSI_MAKESEC_PMT_SiSections,		// SI Filter를 걸어 받은 SI Raw Section들을 통해 합성한다. 기본적으로 모든 PID는 다 기록 (PVR REC용)
	eSVCSI_MAKESEC_PMT_SetByArgs,		// PID, CODEC 정보 등을 다 MakeSection을 부르는 곳에서 정해서 주도록 한다.
	eSVCSI_MAKESEC_PMT_Transcode,		// Transcode에 맞는 PMT를 만든다. Audio, Video StreamType 변경 등 수행

	eSVCSI_MAKESEC_PMT_EndOfCase
} SvcSi_MakeSec_PmtSrcType_e;

// eSEVT_SI_SDT_RUNNING_STATUS_CHANGED 메시지에 따라 달라지는 처리를 정의한다.
typedef enum
{
	eSdtRunningStatusChanged_ByHumaxRule = 0	/* do nothing */
	,eSdtRunningStatusChanged_ByNordig		/* replacement service 처리 */
	,eSdtRunningStatusChanged_ByUkDtt			/* replacement service 처리 - nordig과 같음 */
	,eSdtRunningStatusChanged_ByZiggo			/* AV block */
	,eSdtRunningStatusChanged_BySkyD			/* AV Block */
	,eSdtRunningStatusChanged_ByMiddleEast		/* AV block */
	,eSdtRunningStatusChanged_ByDigiturk
	,eSdtRunningStatusChanged_ByTrueVisions
	// 추가적인 처리가 필요한 경우 계속 추가
} SvcSi_SdtRunningStatusProcessType_e;

// SDT의 running status에 따라 STB에서 처리할 type 정의
//dvb 300468 table-6 의 내용.
typedef enum
{
	eSdtRunningStatusType_UNDEFINED = 0,
	eSdtRunningStatusType_NOT_RUNNING,
	eSdtRunningStatusType_START_IN_A_FEW_SEC,
	eSdtRunningStatusType_PAUSING,
	eSdtRunningStatusType_RUNNING,
	eSdtRunningStatusType_RESERCED,
	eSdtRunningStatusType_MAX
} SvcSi_SdtRunningStatusType_e;

// Download 명령 시 Data (File)을 받는 방식을 정하는 enum
typedef enum
{
	eDNLD_FS_RAMDISK = 0,

	eDNLD_FS_MAX,
	eDNLD_FS_LAST = 0xff
} SvcSi_DownloadFsType_e;

/**@Breif For FTA Content management */
/* FTA Content Management의 Level을 위한 Enum, 다른 메시지랑 섞어야 해서 8바이트만 사용해야하 한다.
DB저장될 값이므로 FTA Mgr이 존재하는지를 확인 위해 eFTA_CONTENT_LEVEL_NONE를 둠. */
typedef enum
{
	eFTA_CONTENT_LEVEL_NONE = 0,
	eFTA_CONTENT_NET_LEVEL,
	eFTA_CONTENT_TS_LEVEL,
	eFTA_CONTENT_SVC_LEVEL,
	eFTA_CONTENT_EVT_LEVEL
} SvcSi_FTA_CONTENT_e;

typedef enum
{
	eSVCSI_RunStatus_Undefined = 0,
	eSVCSI_RunStatus_NotRunning = 1,
	eSVCSI_RunStatus_StartSoon = 2,
	eSVCSI_RunStatus_Pausing = 3,
	eSVCSI_RunStatus_Running = 4
} SvcSi_RunningStatus_t;

typedef enum
{
	eSVCSI_LiveType_Normal		= 0,
	eSVCSI_LiveType_Playback,
	eSVCSI_LiveType_Record,
	eSVCSI_LiveType_Fcc,
	eSVCSI_LiveType_Mheg,
	eSVCSI_LiveType_Hbbtv,
	eSVCSI_LiveType_SatIp,

	eSVCSI_LiveType_EndOfCase
} SvcSi_LiveType_e;

/*******************************************************************/
/********************      Data Type (Structure)      *************************/
/*******************************************************************/

typedef	struct tagBouquetInfoTable
{
	HUINT32				 ulBqNum, ulArrayLen;
	DbSvc_BouqInfo_t	*pstBqArray;
} SvcSi_BouquetInfoTable_t;

typedef struct tagServiceInfoTable
{
	HUINT32				 ulSvcNum, ulArrayLen;
	DbSvc_Info_t		*pstSvcArray;
} SvcSi_ServiceInfoTable_t;

typedef	struct tagProviderInfoTable
{
	HUINT32			 ulProviderNum, ulArrayLen;
	DbSvc_PrvInfo_t	*pstProviderArray;
} SvcSi_ProviderInfoTable_t;

typedef	struct tagNetworkInfoTable
{
	HUINT32			 ulNetworkNum, ulArrayLen;
	DbSvc_NetInfo_t	*pstNetworkArray;
} SvcSi_NetworkInfoTable_t;

typedef	struct tagTsInfoTable
{
	HUINT32			 ulTsNum, ulArrayLen;
	DbSvc_TsInfo_t	*pstTsArray;
} SvcSi_TsInfoTable_t;


// Spec 모든 것들은 eActionKind/eActionSpec 둘 다 순서대로 있어야 한다.
typedef struct
{
	SvcSi_ActionKind_e		 eActionKind;
	HUINT32					 ulActionSpec;
} SvcSi_TemplateSpec_t;

typedef struct
{
	SvcSi_ActionKind_e		 eActionKind;
	HUINT32					 ulActionSpec;

	// ES Type:
	SvcSi_EsSelect_e		 eVideo;
	SvcSi_EsSelect_e		 eAudio;
	SvcSi_EsSelect_e		 eSubtitle;
	SvcSi_EsSelect_e		 eTeletext;

	// Time Update
	SvcSi_TimeUpdate_e		 eTimeUpdate;
} SvcSi_LiveSpec_t;

typedef struct
{
	SvcSi_ActionKind_e		 ulActionKind;
	HUINT32					 ulActionSpec;
} SvcSi_SearchSpec_t;

typedef struct
{
	SvcSi_ActionKind_e		 eActionKind;
	HUINT32					 ulActionSpec;
} SvcSi_ExtSearchSpec_t;

typedef struct
{
	SvcSi_ActionKind_e		 eActionKind;
	HUINT32					 ulActionSpec;
} SvcSi_SatRecordSpec_t;

typedef struct
{
	SvcSi_ActionKind_e		 eActionKind;
	HUINT32					 ulActionSpec;
} SvcSi_SatipSpec_t;

typedef struct
{
	SvcSi_ActionKind_e		 eActionKind;
	HUINT32					 ulActionSpec;
} SvcSi_SwupCheckSpec_t;

typedef struct
{
	SvcSi_ActionKind_e		 eActionKind;
	HUINT32					 ulActionSpec;
} SvcSi_LiveSwupSpec_t;

typedef struct
{
	SvcSi_ActionKind_e		 eActionKind;
	HUINT32					 ulActionSpec;
} SvcSi_RecSpec_t;

typedef struct
{
	SvcSi_ActionKind_e		 eActionKind;
	HUINT32					 ulActionSpec;

	// ES Type:
	SvcSi_EsSelect_e		 eVideo;
	SvcSi_EsSelect_e		 eAudio;
	SvcSi_EsSelect_e		 eSubtitle;
	SvcSi_EsSelect_e		 eTeletext;

	// Time Update
	SvcSi_TimeUpdate_e		 eTimeUpdate;

} SvcSi_PbSpec_t;


typedef struct
{
	SvcSi_ActionKind_e		 eActionKind;
	HUINT32					 ulActionSpec;
} SvcSi_MediaPbSpec_t;

typedef struct
{
	SvcSi_ActionKind_e		 eActionKind;
	HUINT32					 ulActionSpec;
} SvcSi_TimeUpdateSpec_t;

typedef struct
{
	SvcSi_ActionKind_e			 eActionKind;
	HUINT32						 ulActionSpec;
} SvcSi_DynamicUpdateSpec_t;

typedef struct
{
	SvcSi_ActionKind_e			 eActionKind;	// reserved for future use
	HUINT32						 ulActionSpec;
} SvcSi_MakeSectionSpec_t;

typedef union
{
	SvcSi_TemplateSpec_t	 	stTemplate;
	SvcSi_LiveSpec_t		 	stLiveSpec;
	SvcSi_RecSpec_t				stRecSpec;
	SvcSi_PbSpec_t				stPbSpec;
	SvcSi_MediaPbSpec_t			stMediaPbSpec;
	SvcSi_SearchSpec_t		 	stSrchSpec;
	SvcSi_ExtSearchSpec_t	 	stExtSrchSpec;
	SvcSi_SatRecordSpec_t	 	stSatRecordSpec;
	SvcSi_SwupCheckSpec_t	 	stSwupCheckSpec;
	SvcSi_LiveSwupSpec_t 		stLiveSwupSpec;
	SvcSi_TimeUpdateSpec_t	 	stTimeUpdateSpec;
	SvcSi_MakeSectionSpec_t	 	stMakeSectionSpec;
	SvcSi_DynamicUpdateSpec_t	stDynamicUpdateSpec;
} SvcSi_SiSpec_t;

// Session Related:
typedef HUINT32				 SvcSi_SessionH_t;

// Search Related:
typedef struct
{
	HUINT32					 ulSearchType;			// SvcSi_SearchType_e
	HUINT32					 ulSvcType;				// DbSvc_Type_e
	HUINT32					 ulCasType;				// DbSvc_CasType_e
	SvcSi_SessionH_t		 hSession;				// Session Handle:

	HUINT32					 ulNetId;
} SvcSi_SearchOption_t;

// Ext Search Related:
typedef struct
{
	HUINT32					ulExtSearchType;		// SvcSi_SearchType_e
	HUINT16		 			usExtId1;
	HUINT16		 			usExtId2;
	HUINT16		 			usExtId3;
} SvcSi_ExtSearchOption_t;

// SatRecord Related:
typedef struct
{
	HUINT32					ulSatRecordMode;		// SvcSi_SatRecordType_e
	HUINT16		 			usExtId1;
	HUINT16		 			usExtId2;
	HUINT16		 			usExtId3;
} SvcSi_SatRecordOption_t;

typedef struct
{
	HUINT8		 bookingIdFormat;
	HUINT8		 transportProtocol;
	HUINT8		 dsmccPid;
} SatRecordInfo_t;

// Playback Related:
typedef struct
{
	HUINT16				usPmtPid;
	HUINT16				usSvcId;
	HUINT16				usTsId;
} SvcSi_PlaybackOption_t;

// Dynamic update Related:
typedef struct
{
	HUINT32					 ulReserved;	// reserved for future use
} SvcSi_DynamicUpdateOption_t;


// SW Update Related
typedef struct
{
	HUINT32					ulSwupMode;
	HUINT16					usExtId1;
	HUINT16					usExtId2;
} SvcSi_SwupCheckOption_t;

// Live SW Update Related:
typedef struct
{
	Handle_t				 hLiveSvcHandle;	// live service handle
	HUINT32					 ulReserved;		// reserved for future use
} SvcSi_LiveSwupOption_t;

typedef struct _SvcSi_SwupFlag_t
{
	SIxTable_Type_e					eSiTableType;
	HUINT32							ulSize;
	void							*pvData;
} SvcSi_SwupFlag_t;

typedef struct
{
	HUINT32			 ulDataVersion;

	UNIXTIME		startDateTime[SvcSi_MAX_SCHUDULE];			/** < start_date_time of scheduling_descriptor in UNT				*/
	UNIXTIME		endDateTime[SvcSi_MAX_SCHUDULE];			/** < end_date_time of scheduling_descriptor in UNT					*/
	DbSvc_TripleId_t	stTripleId;
} SvcSi_SwupSchedule_t;
/**
  *		Section Composing (Making) 기능을 위한 설정들
**/
typedef struct
{
	SIxTable_Type_e		 eSiType;
	HUINT16				 usTsId;
	HUINT16				 usSvcId;
	HUINT16				 usPmtPid;
} SvcSi_MakeSec_Pat_t;

typedef struct
{
	HUINT32				 ulReserved;
} SvcSi_MakeSec_PmtSection_t;	// SI Filter를 걸어 받은 SI Raw Section들을 통해 합성한다. 기본적으로 모든 PID는 다 기록 (PVR REC용)

typedef struct
{
	HUINT16				 usPcrPid;

	HUINT8				 ucVideoStreamType;		// ISO_VIDEO_1 if MPEG2, ISO_VIDEO_4 if H.264/AVC
	HUINT16				 usVideoPid;

	HUINT8				 ucAudioStreamType;		// ISO_AUDIO_1 if MPEG Audio,  ISO_AUDIO_4 if MPEG4 AAC
	HUINT16				 usAudioPid;
} SvcSi_MakeSec_PmtSetting_t;	// PID, CODEC 정보 등을 다 MakeSection을 부르는 곳에서 정해서 주도록 한다.

typedef struct
{
	SIxTable_Type_e		 eSiType;
	Handle_t			 hSvc;
	HUINT16				 usTsUniqId;
	HUINT16				 usSvcId;

	SvcSi_MakeSec_PmtSrcType_e		 eMakeSrcType;
	union
	{
		SvcSi_MakeSec_PmtSection_t	 stSectionArg;
		SvcSi_MakeSec_PmtSetting_t	 stSettingArg;
	} unArgs;
} SvcSi_MakeSec_Pmt_t;

typedef struct
{
	SIxTable_Type_e		 eSiType;
	Handle_t			 hSvc;
	HUINT16				 usTsUniqId;
	HUINT16				 usOnId, usTsId, usSvcId;
} SvcSi_MakeSec_Sit_t;

typedef union
{
	SIxTable_Type_e 		eSiType;
	SvcSi_MakeSec_Pat_t		stPat;
	SvcSi_MakeSec_Pmt_t		stPmt;
	SvcSi_MakeSec_Sit_t		stSit;	// 실제로 동작하지는 않음
} SvcSi_MakeSecArg_t;

typedef enum
{
	eSI_AUD_CPNT_UNKNWON = 0,
	eSI_AUD_CPNT_SINGLE_MONO ,
	eSI_AUD_CPNT_DUAL_MONO,
	eSI_AUD_CPNT_STEREO,
	eSI_AUD_CPNT_MULTILINGUAL,
	eSI_AUD_CPNT_SURROUND,
	eSI_AUD_CPNT_DOLBY
} SvcSi_AudCpnt_e;

typedef struct tagSvcSi_ISO_639_LAN_INFO
{
	AUDIO_Type_t	eAudType;
	HUINT8			szIso639LangCode[4];
} SvcSi_Iso639LangInfo_t;


/*************************************
  	Audio List
**************************************/
typedef struct
{
	HUINT16			usPid;					/**< Video PID */
	DxVideoCodec_e	eCodec;					/**< Video Codec */
	HUINT8			ucComponentType;
	HUINT8			ucComponentTag;
	HBOOL			bCaDesFound;
	HBOOL			bIsHd;
} SvcSi_VideoInfo_t;

typedef struct
{
	HUINT32				 nVideo;
	SvcSi_VideoInfo_t	*pstVideoInfo;
} SvcSi_VideoList_t;

/*************************************
  	Audio List
**************************************/
typedef struct
{
	SvcSi_AudCpnt_e	eAudCpnt;				/**< Audio Component Type (EVT_CPNT_AUD_SINGLEMONO, EVT_CPNT_AUD_DUALMONO etc 중 하나의 값만 가짐. */
	HUINT16			usPid;					/**< Audio PID */
	DxAudioCodec_e	eCodec;					/**< Audio Codec */
	HBOOL			bIsDolby;				/**< Dolby? */
	SvcSi_Iso639LangInfo_t	stIsoLangInfo[SvcSi_MAX_AUD_NUM];

	HUINT8			ucComponentTag;
	HUINT8			aucStreamName[SvcSi_MAX_AUD_NAME_LEN];

	HBOOL			bIsDualMono;			/**< Dual Mono? */

	SUPPLEMENTARY_AUDIO_Type_t		suppAudioEditorial;
	HUINT8			ucMixType;

} SvcSi_AudioInfo_t;		/* AUD_Info_t 에서 이름만 변경 */

typedef struct
{
	HUINT32				 nAudio;
	SvcSi_AudioInfo_t	*pstAudioInfo;
} SvcSi_AudioList_t;	/* AUD_InfoList_t 에서 이름만 변경 */

/*************************************
  	ECM List
**************************************/
typedef struct
{
	HUINT16			usPid;					/**< ECM PID */
	HUINT16			usCaSysId;				/**< . */
}SvcSi_EcmInfo_t;

typedef struct
{
	HUINT32				 nEcm;
	SvcSi_EcmInfo_t		*pstEcmInfo;
} SvcSi_EcmList_t;

/*************************************
  	EMM List
**************************************/
typedef struct
{
	HUINT16			usPid;					/**< ECM PID */
	HUINT16			usCaSysId;				/**< . */
}SvcSi_EmmInfo_t;

typedef struct
{
	HUINT32				 nEmm;
	SvcSi_EmmInfo_t		*pstEmmInfo;
} SvcSi_EmmList_t;

/*************************************
  	Subtitle List
**************************************/
typedef enum
{
    eSI_SUBTITLE_DVB = 0,
    eSI_SUBTITLE_EBU,
    eSI_SUBTITLE_SUPERCC,				/**< Superimpose + ClosedCaption */
} SvcSi_Subtitle_Type_e;

typedef struct
{
	HUINT8		ucType;					/**< TYPE */
	HUINT8		ucComponentTag;
    HUINT16     usCompositionPageId;	/**< Composition Page ID */
    HUINT16		usAncillaryPageId;		/**< Ancillary Page ID */
   	HUINT8		aucLangCode [4];		/**< Language Code */
} SvcSi_SubtitleDvbInfo_t;

typedef struct
{
	HUINT8		ucType;					/**< TYPE */
	HUINT8		ucComponentTag;
    HUINT8      ucMagazineNum;			/**< Magazine Number */
    HUINT8      ucPageNum;				/**< Page Number */
	HUINT8		aucLangCode [4];		/**< Language Code */
} SvcSi_SubtitleEbuInfo_t;

typedef struct
{
	HUINT8		ucComponentTag;			/**< CC(0x30~0x37), Super(0x38) */
} SvcSi_SubtitleSuperCcInfo_t;			/**< Superimpose + ClosedCaption */

typedef struct
{
    HUINT16     				usPid;				/**< PID */
    SvcSi_Subtitle_Type_e		eType;				/**< TYPE : DVB or EBU */
    union
    {
		SvcSi_SubtitleDvbInfo_t		stDvbInfo;
		SvcSi_SubtitleEbuInfo_t		stEbuInfo;
		SvcSi_SubtitleSuperCcInfo_t	stSuperCcInfo;
    } utArg;
} SvcSi_SubtitleInfo_t;

typedef struct
{
    HUINT32     				ulSubtitleNum;		/**< Subtitle 개수 */
	SvcSi_SubtitleInfo_t		*pstSubtitleInfo;		/**< Subtitle Info */
} SvcSi_SubtitleList_t;	/* SBTL_InfoList_t에서 이름만 변경 */

typedef struct tagWEB_PMT_INFO
{
	HUINT8			ucStreamType;
	HUINT16			usPid;
	HUINT8			ucComponentTag;
	HUINT16			usDataBroadcastTag;
	HUINT32			ulCarouselIDTag;
} SvcSi_WebPmtInfo_t;

// For SWUP Information :
// HUMAX에서 SI Value로 지정한 SWUP Data Type
typedef enum
{
	eSWUP_IMAGE_TYPE_LOADER		= 1,
	eSWUP_IMAGE_TYPE_APP		= 2,
	eSWUP_IMAGE_TYPE_EEPROM		= 3,
	eSWUP_IMAGE_TYPE_FLASH		= 4,
	eSWUP_IMAGE_TYPE_PRODID		= 5,
	eSWUP_IMAGE_TYPE_GAME		= 6,
	eSWUP_IMAGE_TYPE_DVD		= 7,

	eSWUP_IMAGE_TYPE_MAXVALUE	= 0xff
} SvcSi_SwupImageType_e;

typedef	struct	tagEvent
{
// Plain data from EIT
	HINT32			nEvtId;					// Event ID
	HUINT32			ulStartTime;			// unit : sec
	HUINT32			ulDuration;				// unit : sec
	HUINT8			ucRunStatus;
	HUINT8			ucParentRating;			// Parental Rating
											// 0x00:    undefined
											// 0x01-0F: age = rating + 3 yrs
					   						// 0x10-FF: defined by broadcaster
					   						// country를 고려해 rating 중 하나를 선택해 값을 채움

	HUINT8 			ucContent;				// Content nibble
	HUINT8 			ucContent2nd;			// Content Nibble

// String data from EIT (short EVENT des)
	HUINT8			szName[SvcSi_MAX_SHORT_EVENT_NAME];
	HUINT8			szText[SvcSi_MAX_EVENT_TEXT];

// from content descriptors in EIT or element status in PMT
	HUINT32			ulVidComponentStatus;		// Video Components Status
	HUINT32			ulAudComponentStatus;		// Audio Components Status
	HUINT32			ulAuxComponentStatus;		// Subtitle/Teletext/ETC Components Status

// from CA identifier descriptor or CA descriptor in EIT or PMT
	HUINT32			ulCasType;

// from fta content management descriptor in EIT
	HUINT8			ucDoNotScramble;
	HUINT8			ucCtrlRAccOverINet;
	HUINT8			ucDoNotApplyRevocation;

/*************************
 * flag for extended descriptor *
 *************************/
	HBOOL			bExtEvtInfo; 	/* TRUE if there is extended event */
	HBOOL			bComponent;	/* TRUE if there is component */

/***
  *	descriptor list
  *
  *	pstRatingDescList : 모든 rating 정보를 list로 저장
  *	pstEitDescList : eit 정보 가져올 때 기본적으로 처리하지 않는 desc.들만 여기에 list로 달아둠   *	(ex : uk dtt에서 사용하는 guidance desc)
  *	event structur 사용을 마치면 반드시 free 해줘야 함 -> SVC_SI_FreeEventInfo() 호출할 것 !
***/
	HxList_t		*pstRatingDescList;		/* SIxDescriptor_t */
	HxList_t		*pstEitDescList;		/* SIxDescriptor_t */

	struct tagEvent	*pNext;
} SvcSi_Event_t;

typedef struct
{
	HUINT8			*szItemDesc;
	HUINT8			*szItemChar;
} SvcSi_ExtEventItem_t;

typedef struct
{
	HBOOL					 bAvailable;		//
	HUINT8					 ucDesNum;			// This two arguments are used only when getting the data from the descriptors:
										// No need to use in the AP or other MW modules.

	HUINT8					 aucLang[4];

	HINT32					 nNumItem;
	SvcSi_ExtEventItem_t	*astEvtItem;

	HUINT8					*szTextChar;
} SvcSi_ExtEvent_t;

typedef struct
{
	HINT32				 nNumEvt;
	SvcSi_ExtEvent_t	*astEvtArray;
} ExtEventArray_t;


// UNT Descriptor
typedef struct
{
	HUINT8	ucIsAuto;	// update_flag of update_descriptor in UNT
	HUINT8	ucMethod;	// update method of update_descriptor in UNT
	HUINT8	ucPriority;	// update priority of update_descriptor in UNT, 0 is the highest priority.
} SvcSi_SsuUpdateInfo_t;

typedef struct
{
	HxDATETIME_t	startDateTime;	// start_date_time of scheduling_descriptor in UNT
	HxDATETIME_t	endDateTime;	// end_date_time of scheduling_descriptor in UNT
	HUINT8			ucIsFinalAvailable;	// final_availability of scheduling_descriptor in UNT
	HUINT8			ucIsPeriodic;	// periodicity_flag of scheduling_descriptor in UNT
									// unit : Sec(0), Min(1), Hour(2), Day(3)
	HUINT8			ucPeriodUnit;	// period_unit of scheduling_descriptor in UNT
	HUINT8			ucDurationUnit;	// duration_unit of scheduling_descriptor in UNT
	HUINT8			ucEstimatedCycleUnit;	// estimated_cycle_time_unit of scheduling_descriptor in UNT
	HUINT8			ucPeriod;	// period of scheduling_descriptor in UNT
	HUINT8			ucDuration;	// duration of scheduling_descriptor in UNT
	HUINT8			ucEstimatedCycle;	// estimated_cycle_time of scheduling_descriptor in UNT
} SvcSi_SsuScheduleInfo_t;

typedef struct tagSSU_MESSAGE_INFO
{
	HUINT8	ucLangCode[4];	// ISO_639_language_code of message_descriptor in UNT
	HUINT8	*pucText;		// text_char of message_descriptor in UNT
	HUINT16	usTextLen;		// text length
	struct tagSSU_MESSAGE_INFO *pNextInfo;
} SvcSi_SsuMessageInfo_t;

typedef struct
{
	SvcSi_SsuUpdateInfo_t		*pUpdateInfo;
	SvcSi_SsuScheduleInfo_t		*pScheduleInfo;
	SvcSi_SsuMessageInfo_t		*pMsgInfo;
	// to be added...
} SvcSi_SsuDescInfo_t;

typedef struct
{
	SvcSi_SsuUpdateInfo_t		*pUpdateInfo;
	SvcSi_SsuScheduleInfo_t		*pScheduleInfo;
	SvcSi_SsuMessageInfo_t		*pMessageInfo;
	HUINT16						usNumUpdateInfo;
	HUINT16						usNumScheduleInfo;
	HUINT16						usNumMessageInfo;
} SvcSi_SsuDescriptorInfo_t;


#if 1
/*
	Section Composing (Making) 기능을 위한 설정들
*/
typedef struct
{
	SiTableType_e		 eSiType;
	HUINT16				 usTsId;
	HUINT16				 usSvcId;
	HUINT16				 usPmtPid;
} MwcMakeSecArg_Pat_t;

typedef enum
{
	eMakePmtSrcType_SiSections,				// SI Filter를 걸어 받은 SI Raw Section들을 통해 합성한다. 기본적으로 모든 PID는 다 기록 (PVR REC용)
	eMakePmtSrcType_SetByArgs,				// PID, CODEC 정보 등을 다 MakeSection을 부르는 곳에서 정해서 주도록 한다.

	eMakePmtSrcType_Transcode,				// Transcode에 맞는 PMT를 만든다. Audio, Video StreamType 변경 등 수행
	eMakePmtSrcType_EndOfCase
} MwcMakeSecArg_PmtSrcType_e;

typedef struct
{
	HUINT32				 ulReserved;
} MwcMakeSec_Pmt_Section_Arg_t;	// SI Filter를 걸어 받은 SI Raw Section들을 통해 합성한다. 기본적으로 모든 PID는 다 기록 (PVR REC용)

typedef struct
{
	HUINT16				 usPcrPid;

	HUINT8				 ucVideoStreamType;		// ISO_VIDEO_1 if MPEG2, ISO_VIDEO_4 if H.264/AVC
	HUINT16				 usVideoPid;

	HUINT8				 ucAudioStreamType;		// ISO_AUDIO_1 if MPEG Audio,  ISO_AUDIO_4 if MPEG4 AAC
	HUINT16				 usAudioPid;

} MwcMakeSec_Pmt_Setting_Arg_t;	// PID, CODEC 정보 등을 다 MakeSection을 부르는 곳에서 정해서 주도록 한다.

typedef struct
{
	SiTableType_e		 eSiType;
	Handle_t			 hSvc;
	HUINT16				 usTsUniqId;
	HUINT16				 usSvcId;

	MwcMakeSecArg_PmtSrcType_e		 eMakeSrcType;
	union
	{
		MwcMakeSec_Pmt_Section_Arg_t	 stSectionArg;
		MwcMakeSec_Pmt_Setting_Arg_t	 stSettingArg;
	} unArgs;
} MwcMakeSecArg_Pmt_t;

typedef struct
{
	SiTableType_e		 eSiType;
	Handle_t			 hSvc;
	HUINT16				 usTsUniqId;
	HUINT16				 usOnId, usTsId, usSvcId;
} MwcMakeSecArg_Sit_t;

typedef union
{
	SiTableType_e			 eSiType;
	MwcMakeSecArg_Pat_t		 stPatArg;
	MwcMakeSecArg_Pmt_t		 stPmtArg;
	MwcMakeSecArg_Sit_t		 stSitArg;
} MwcMakeSecArg_t;
#endif


typedef struct tagSI_ModuleSpecifier
{
	Region_Idx_t						eRegion;
	HxCountry_e							eCountryId;
	DxDeliveryType_e					eDeliType;
	DxOperator_e						eOperatorType;
	DxOPTYPE_e							eSvcOpType;
} SvcSi_ModuleSpecifier_t;

typedef struct tagSI_LcnProperty
{
	DefaultLcnPolicy_t					eLcnPolicy;
	PreProgrammedChNumPolicy_t			ePreChNumPolicy;
} SvcSi_LcnProperty_t;

typedef struct tagSiModuleInfo
{
	// Module Specifier
	SvcSi_ModuleSpecifier_t		stSpecifier;
	// LCN Policy ...
	SvcSi_LcnProperty_t			stLcnInfo;
} SvcSi_ModuleInfo_t;

#define 	SvcSi_LCN_SVC_LIST_NAME_LEN			64

typedef struct
{
	HUINT16		 usSvcListId;
	HUINT8		 ucSecVer;
	HUINT8		 szSvcListName[SvcSi_LCN_SVC_LIST_NAME_LEN];
} SvcSi_LcnSvcListInfo_t;

typedef enum
{
	eSvcSi_CFG_AUD_LANG_User	= 0,		// From DbSvc_Info_t
	eSvcSi_CFG_AUD_LANG_User_With_Pid, 		//
	eSvcSi_CFG_AUD_LANG_0,					// From DB_PARAM : eDB_PARAM_ITEM_LANG_AUDIO0
	eSvcSi_CFG_AUD_LANG_1,					// From DB_PARAM : eDB_PARAM_ITEM_LANG_AUDIO1
	eSvcSi_CFG_AUD_LANG_Undefined,			//
	eSvcSi_CFG_AUD_LANG_Original,			//
	eSvcSi_CFG_AUD_LANG_Default, 			//
	eSvcSi_CFG_AUD_LANG_NUM,
} SvcSi_CfgAudioLang_e;

typedef enum
{
	eSvcSi_CFG_AUD_OUTPUT_Multichannel_User = 0,
	eSvcSi_CFG_AUD_OUTPUT_Multichannel_Menu,
	eSvcSi_CFG_AUD_OUTPUT_NUM
} SvcSi_CfgAudioOutput_e;

typedef enum
{
	eSvcSi_CFG_AUD_DESCRIPTION_User = 0,
	eSvcSi_CFG_AUD_DESCRIPTION_Menu,
	eSvcSi_CFG_AUD_DESCRIPTION_NUM
} SvcSi_CfgAudioDesc_e;

typedef enum
{
	eSvcSi_CFG_SUBTTL_LANG_User	= 0,		// From DbSvc_Info_t
	eSvcSi_CFG_SUBTTL_LANG_User_With_Idx,	//
	eSvcSi_CFG_SUBTTL_LANG_0,				// From DB_PARAM : eDB_PARAM_ITEM_LANG_SUBTITLE0
	eSvcSi_CFG_SUBTTL_LANG_1,				// From DB_PARAM : eDB_PARAM_ITEM_LANG_SUBTITLE1
	eSvcSi_CFG_SUBTTL_LANG_Undefined,			//
	eSvcSi_CFG_SUBTTL_LANG_Original,				//
	eSvcSi_CFG_SUBTTL_LANG_Default,				//
	eSvcSi_CFG_SUBTTL_LANG_NUM,
} SvcSi_CfgSubtitleLang_e;

typedef enum
{
	eSvcSi_CFG_SUBTTL_HOH_User	= 0,		// From DbSvc_Info_t
	eSvcSi_CFG_SUBTTL_HOH_Menu,				// From DB_PARAM : eDB_PARAM_ITEM_HARDOFHEARING
	eSvcSi_CFG_SUBTTL_HOH_NUM,
} SvcSi_CfgSubtitleHoh_e;

typedef enum
{
	eSvcSi_CFG_SUBTTL_DVB_User	= 0,		// From DbSvc_Info_t
	eSvcSi_CFG_SUBTTL_DVB_Menu,				// From DB_PARAM :
	eSvcSi_CFG_SUBTTL_DVB_NUM
} SvcSi_CfgSubtitleDvb_e;

typedef enum
{
	eSvcSi_CFG_SUBTTL_HD_User	= 0,		// From DbSvc_Info_t
	eSvcSi_CFG_SUBTTL_HD_Menu,				// From DB_PARAM :
	eSvcSi_CFG_SUBTTL_HD_NUM
} SvcSi_CfgSubtitleHd_e;

typedef struct
{
	HxLANG_Id_e				aeLangId[eSvcSi_CFG_AUD_LANG_NUM];
	HINT32					nPid;
} SvcSi_CfgAudioLang_t;

typedef struct
{
	DxStereoSelect_e		eStereoSelect;
	HBOOL					abOutputFlag[eSvcSi_CFG_AUD_OUTPUT_NUM];
} SvcSi_CfgAudioOutput_t;

typedef struct
{
	HBOOL					abAdFlag[eSvcSi_CFG_AUD_DESCRIPTION_NUM];
} SvcSi_CfgAudioDesc_t;

typedef struct
{
	DxOperator_e			eOperator;				// eDB_PARAM_ITEM_OPERATOR_TYPE
	HxCountry_e				eCountry;				// eDB_PARAM_ITEM_COUNTRY_ID
	DxAspectRatio_e			eAspectRatio;			// For Subtitle
	HxLANG_Id_e				eOsdLang;				// Menu OSD Langauge
} SvcSi_CfgSystem_t;

typedef struct
{
	HxLANG_Id_e				aeLangId[eSvcSi_CFG_SUBTTL_LANG_NUM];
	HINT32					nTrack;
	HBOOL					bUserSelectedOff;		// user selected 'OFF' track of subtitle
} SvcSi_CfgSubtitleLang_t;

typedef struct
{
	HBOOL					abHoh[eSvcSi_CFG_SUBTTL_HOH_NUM];
	HBOOL					abDvb[eSvcSi_CFG_SUBTTL_DVB_NUM];
	HBOOL					abHD[eSvcSi_CFG_SUBTTL_HD_NUM];
//	DxAspectRatio_e			eAspectRatio;
} SvcSi_CfgSubtitleType_t;

typedef struct
{
	SvcSi_CfgSystem_t			stSystem;				//

	// Audio Related
	SvcSi_CfgAudioLang_t		stAudioLanguage;		// eDB_PARAM_ITEM_LANG_MAIN, eDB_PARAM_ITEM_LANG_AUDIO0, eDB_PARAM_ITEM_LANG_AUDIO1,
	SvcSi_CfgAudioOutput_t		stAudioOutput;			//
	SvcSi_CfgAudioDesc_t		stAudioDesc;			//

	// Subtitle Related
	SvcSi_CfgSubtitleLang_t		stSubtitleLangauge;		//
	SvcSi_CfgSubtitleType_t		stSubtitleType;			// HOH, DVB

	// Add to Here	: Teletext, ...
} SvcSi_Config_t;

typedef struct
{
	HBOOL						bUserSelected;
	HxLANG_Id_e					eLang;
	HINT32						nPid;
	HBOOL						bDolby;
	HBOOL						bAudioDescription;
} SvcSi_UserAudioSettings_t;

typedef struct
{
	HBOOL				bUserSelected;
	HBOOL				bUserSelectedOff;	// user selected 'OFF' track of subtitle
	HxLANG_Id_e			eLang;
	HINT32				nTrack;
	HBOOL				bHoh;	// Hard Of Hearing
	HBOOL				bDvb;	// Dvb
	HBOOL				bHd;	// HD
} SvcSi_UserSubtitleSettings_t;

typedef struct
{
	SvcSi_UserAudioSettings_t			stAudio;
	SvcSi_UserSubtitleSettings_t		stSubtitle;
} SvcSi_UserSettings_t;

typedef enum
{
	eSvcSi_CFG_SYS_ALL		= 0x0000000F,
	eSvcSi_CFG_AUD_LANG		= 0x00000010,
	eSvcSi_CFG_AUD_OUTPUT	= 0x00000020,
	eSvcSi_CFG_AUD_DESC		= 0x00000040,
	eSvcSi_CFG_AUD_ALL		= 0x000000F0,
	eSvcSi_CFG_VID_ALL		= 0x00000F00,
	eSvcSi_CFG_SUBTTL_ALL	= 0x0000F000,
	eSvcSi_CFG_ALL			= 0xFFFFFFFF,
} SvcSi_ConfigClass_e;

typedef HERROR (* SvcSi_GetSysConfigCb_t)(SvcSi_ConfigClass_e bwClass, SvcSi_Config_t *pstConfig, SvcSi_UserSettings_t *pstUserSettings);

#if 1	// For Audio Priority Table
typedef enum
{
	eSvcSi_AUD_SEL_LANG_USER		= 0,
	eSvcSi_AUD_SEL_LANG_USER_WITH_PID,
	eSvcSi_AUD_SEL_LANG_0,
	eSvcSi_AUD_SEL_LANG_1,
	eSvcSi_AUD_SEL_LANG_OSD,
	eSvcSi_AUD_SEL_LANG_UNDEFINED,
	eSvcSi_AUD_SEL_LANG_ORIGINAL,
	eSvcSi_AUD_SEL_LANG_DEFAULT,
	eSvcSi_AUD_SEL_TYPE_UNDEFINED,
	eSvcSi_AUD_SEL_TYPE_CLEANEFFECT,
	eSvcSi_AUD_SEL_TYPE_HEARING_IMPAIRED,
	eSvcSi_AUD_SEL_TYPE_VISUAL_IMPAIRED,
	eSvcSi_AUD_SEL_DESCRIPTION_BROADCASTER_MIX_USER,
	eSvcSi_AUD_SEL_DESCRIPTION_BROADCASTER_MIX_MENU,
	eSvcSi_AUD_SEL_DESCRIPTION_RECEIVER_MIX_USER,			// Not Implemented
	eSvcSi_AUD_SEL_DESCRIPTION_RECEIVER_MIX_MENU,			// Not Implemented
	eSvcSi_AUD_SEL_DESCRIPTION_OFF_BROADCASTER_MIX_ON,
	eSvcSi_AUD_SEL_DESCRIPTION_OFF_RECEIVER_MIX_ON,
	eSvcSi_AUD_SEL_DESCRIPTION_OFF_BROADCASTER_MIX_OFF,
	eSvcSi_AUD_SEL_DESCRIPTION_OFF_RECEIVER_MIX_OFF,
	eSvcSi_AUD_SEL_DESCRIPTION_ON_BROADCASTER_MIX_OFF,
	eSvcSi_AUD_SEL_DESCRIPTION_ON_RECEIVER_MIX_OFF,
	eSvcSi_AUD_SEL_MULTICHANNEL_USER,
	eSvcSi_AUD_SEL_MULTICHANNEL_MENU,						// Dolby
	eSvcSi_AUD_SEL_TYPE_NUM
} SvcSi_EsAudioSelType_e;

typedef enum
{
	eSvcSi_AUD_PRIORITY_LANG_BASE		= 0,
	eSvcSi_AUD_PRIORITY_LANG_USER,
	eSvcSi_AUD_PRIORITY_LANG_USER_WITH_PID,
	eSvcSi_AUD_PRIORITY_LANG_0,
	eSvcSi_AUD_PRIORITY_LANG_1,
	eSvcSi_AUD_PRIORITY_LANG_OSD,
	eSvcSi_AUD_PRIORITY_LANG_UNDEFINED,
	eSvcSi_AUD_PRIORITY_LANG_ORIGINAL,
	eSvcSi_AUD_PRIORITY_LANG_DEFAULT,
	eSvcSi_AUD_PRIORITY_TYPE_UNDEFINED,
	eSvcSi_AUD_PRIORITY_TYPE_CLEANEFFECT,
	eSvcSi_AUD_PRIORITY_TYPE_HEARING_IMPAIRED,
	eSvcSi_AUD_PRIORITY_TYPE_VISUAL_IMPAIRED,
	eSvcSi_AUD_PRIORITY_DESCRIPTION_BROADCASTER_MIX_USER,
	eSvcSi_AUD_PRIORITY_DESCRIPTION_BROADCASTER_MIX_MENU,
	eSvcSi_AUD_PRIORITY_DESCRIPTION_BROADCASTER_MIX_BASE,
	eSvcSi_AUD_PRIORITY_DESCRIPTION_RECEIVER_MIX_USER,			// Not Implemented
	eSvcSi_AUD_PRIORITY_DESCRIPTION_RECEIVER_MIX_MENU,			// Not Implemented
	eSvcSi_AUD_PRIORITY_DESCRIPTION_RECEIVER_MIX_BASE,
	eSvcSi_AUD_PRIORITY_DESCRIPTION_OFF_BROADCASTER_MIX_ON,
	eSvcSi_AUD_PRIORITY_DESCRIPTION_OFF_RECEIVER_MIX_ON,
	eSvcSi_AUD_PRIORITY_DESCRIPTION_OFF_BROADCASTER_MIX_OFF,
	eSvcSi_AUD_PRIORITY_DESCRIPTION_OFF_RECEIVER_MIX_OFF,
	eSvcSi_AUD_PRIORITY_DESCRIPTION_ON_BROADCASTER_MIX_OFF,
	eSvcSi_AUD_PRIORITY_DESCRIPTION_ON_RECEIVER_MIX_OFF,
	eSvcSi_AUD_PRIORITY_MULTICHANNEL_USER,
	eSvcSi_AUD_PRIORITY_MULTICHANNEL_MENU,						// Dolby
	eSvcSi_AUD_PRIORITY_MULTICHANNEL_BASE,
	eSvcSi_AUD_PRIORITY_NORMAL_AUDIO,					// audio type(ISO 639 language descriptor ) is undefined  || supplementary audio MAIN
	eSvcSi_AUD_PRIORITY_FIRST_TRACK,
	eSvcSi_AUD_PRIORITY_DUAL_MONO,
	eSvcSi_AUD_PRIORITY_TYPE_NUM
} SvcSi_AudioPriorityType_e;

typedef enum
{
	eSvcSi_SUBTTL_SEL_LANG_USER		= 0,
	eSvcSi_SUBTTL_SEL_LANG_USER_WITH_IDX,
	eSvcSi_SUBTTL_SEL_LANG_0,
	eSvcSi_SUBTTL_SEL_LANG_1,
	eSvcSi_SUBTTL_SEL_LANG_OSD,
	eSvcSi_SUBTTL_SEL_LANG_UNDEFINED,
	eSvcSi_SUBTTL_SEL_LANG_ORIGINAL,
	eSvcSi_SUBTTL_SEL_LANG_DEFAULT,
	eSvcSi_SUBTTL_SEL_TYPE_DVB_USER,
	eSvcSi_SUBTTL_SEL_TYPE_DVB_MENU,
	eSvcSi_SUBTTL_SEL_TYPE_DVB_AUTO,
	eSvcSi_SUBTTL_SEL_TYPE_EBU_USER,
	eSvcSi_SUBTTL_SEL_TYPE_EBU_MENU,
	eSvcSi_SUBTTL_SEL_TYPE_EBU_AUTO,
	eSvcSi_SUBTTL_SEL_HOH_USER,
	eSvcSi_SUBTTL_SEL_HOH_MENU,
	eSvcSi_SUBTTL_SEL_HOH_OFF_ES_YES,
	eSvcSi_SUBTTL_SEL_HOH_OFF_ES_NO,
	eSvcSi_SUBTTL_SEL_HOH_ON_ES_NO,
	eSvcSi_SUBTTL_SEL_ASPECTRATIO_NA,
	eSvcSi_SUBTTL_SEL_ASPECTRATIO_YES,
	eSvcSi_SUBTTL_SEL_HD_NA,
	eSvcSi_SUBTTL_SEL_HD_USER,
	eSvcSi_SUBTTL_SEL_HD_MENU,
	eSvcSi_SUBTTL_SEL_TRACK,
	eSvcSi_SUBTTL_SEL_TYPE_NUM
} SvcSi_EsSubtitleSelType_e;

typedef enum
{
	eSvcSi_SUBTTL_PRIORITY_LANG_BASE		= 0,
	eSvcSi_SUBTTL_PRIORITY_LANG_USER,
	eSvcSi_SUBTTL_PRIORITY_LANG_USER_WITH_IDX,
	eSvcSi_SUBTTL_PRIORITY_LANG_MENU_0,
	eSvcSi_SUBTTL_PRIORITY_LANG_MENU_1,
	eSvcSi_SUBTTL_PRIORITY_LANG_OSD,
	eSvcSi_SUBTTL_PRIORITY_LANG_UNDEFINED,
	eSvcSi_SUBTTL_PRIORITY_LANG_ORIGINAL,
	eSvcSi_SUBTTL_PRIORITY_LANG_DEFAULT,
	eSvcSi_SUBTTL_PRIORITY_TYPE_BASE,
	eSvcSi_SUBTTL_PRIORITY_TYPE_DVB_MATCHED_USER,
	eSvcSi_SUBTTL_PRIORITY_TYPE_DVB_MATCHED_MENU,
	eSvcSi_SUBTTL_PRIORITY_TYPE_DVB_MATCHED_AUTO,
	eSvcSi_SUBTTL_PRIORITY_TYPE_EBU_MATCHED_USER,
	eSvcSi_SUBTTL_PRIORITY_TYPE_EBU_MATCHED_MENU,
	eSvcSi_SUBTTL_PRIORITY_TYPE_EBU_MATCHED_AUTO,
	eSvcSi_SUBTTL_PRIORITY_HOH_BASE,
	eSvcSi_SUBTTL_PRIORITY_HOH_MATCHED_USER,
	eSvcSi_SUBTTL_PRIORITY_HOH_MATCHED_MENU,
	eSvcSi_SUBTTL_PRIORITY_HOH_OFF_ES_YES,
	eSvcSi_SUBTTL_PRIORITY_HOH_OFF_ES_NO,
	eSvcSi_SUBTTL_PRIORITY_HOH_ON_ES_NO,
	eSvcSi_SUBTTL_PRIORITY_ASPECTRATIO_BASE,
	eSvcSi_SUBTTL_PRIORITY_ASPECTRATIO_NA,
	eSvcSi_SUBTTL_PRIORITY_ASPECTRATIO_MATCHED_YES,
	eSvcSi_SUBTTL_PRIORITY_HD_BASE,
	eSvcSi_SUBTTL_PRIORITY_HD_NA,
	eSvcSi_SUBTTL_PRIORITY_HD_MATCHED_USER,
	eSvcSi_SUBTTL_PRIORITY_HD_MATCHED_MENU,
	eSvcSi_SUBTTL_PRIORITY_FIRST_TRACK_BASE,
	eSvcSi_SUBTTL_PRIORITY_FIRST_TRACK_YES,
	eSvcSi_SUBTTL_PRIORITY_FIRST_TRACK_NO,
	eSvcSi_SUBTTL_PRIORITY_TYPE_NUM
} SvcSi_SubtitlePriorityType_e;

typedef struct
{
	HINT32		nPriorityType;
	HINT32		nSelType;
	HINT32		nPriority;
} SvcSi_EsTypePriority_t;


typedef struct
{
	SvcSi_EsTypePriority_t			*astAudio;
	HUINT32							 ulItemNumAudio;
	SvcSi_EsTypePriority_t			*astSubtitle;
	HUINT32							 ulItemNumSubtitle;
	// Add to Here
} SvcSi_EsPriorityTable_t;

#endif

typedef struct
{
	HBOOL			 bFound;
	HINT32			 nIndexMain;				// 선택되지 않았으면 -1
	HINT32			 nIndexAux;					// For Audio Es : Audio Description 선택되지 않았으면 -1
	HUINT16			 usPid;
	HUINT8			 ucComponentTag;
	HUINT32			 ulCodec;
	HxLANG_Id_e		 eLangIdMain;				// For Main Index
	HxLANG_Id_e		 eLangIdAux;				// For Aux Index
	HUINT32		 	 ulParam1;					// For Audio ES : eDualMonoSelect;
	HUINT32		 	 ulParam2;					//
	HUINT32		 	 ulParam3;					//
} SvcSi_SelectedEs_t;

#if defined(SvcSi_SUPPORT_LIB_ES_NEW)
typedef enum
{
	eES_UPDATE_Video		= 0,
	eES_UPDATE_Audio,
	eES_UPDATE_Subtitle,
	eES_UPDATE_Teletext,
} SvcSi_EsUpdateType_e;

typedef struct
{
	HUINT16			 usServiceId;						/** < Service ID */
	HUINT16			 usPcrPid;							/** < PCR PID */

	HUINT32			 ulPmtCRC;							/** PMT CRC_32 */
	HUINT32			 ulCasType;							/** < it's same value with DbSvc_CasType_e */

	Handle_t		 hVideoList;						/** < HxList_t of svcSi_VideoEs_t */
	Handle_t		 hAudioList;						/** < HxList_t of svcSi_AudioEs_t */
	Handle_t		 hSubttlList;						/** < HxList_t of svcSi_SubtitleEs_t */
	Handle_t		 hTeltxtList;						/** < HxList_t of svcSi_TeletextEs_t */
} SvcSi_PmtEsInfo_t;

typedef HERROR (*SvcSi_UpdateEtcInfoCb_t)(const SvcSi_PmtEsInfo_t *pstPmtEsInfo, void *pvSvcInfo);
typedef HERROR (*SvcSi_UpdateEsInfoCb_t)(SvcSi_EsUpdateType_e eType, Handle_t hEsInfoList, void *pvSvcInfo, SvcSi_SelectedEs_t *pstSelectedVideo);
typedef HBOOL (*SvcSi_CompareSvcCb_t)(const void *pvInfo1, const void *pvInfo2, HUINT32 ulSize);

typedef struct
{
	void						*pvOrgInfo;
	void						*pvDstInfo;
	HUINT32						 ulSize;
	SvcSi_UserSettings_t		 stUserSettings;

	SvcSi_UpdateEsInfoCb_t		 fnUpdateEs;
	SvcSi_UpdateEtcInfoCb_t		 fnUpdateSvc;
	SvcSi_CompareSvcCb_t		 fnCompareSvc;
} SvcSi_UpdateEs_t;
#endif


typedef enum
{
	eSvcSi_OTA_FAILREASON_Unknown			= 0,
	eSvcSi_OTA_FAILREASON_Latest_Version,		// Current installed SW is latest version. (Signalled SW version is same or lower than installed SW.)
	eSvcSi_OTA_FAILREASON_Action,
	eSvcSi_OTA_FAILREASON_Max,
} SvcSi_OtaFailReason_e;


/*******************************************************************/
/********************      Global Variables     ********************/
/*******************************************************************/

/*******************************************************************/
/********************      API Functions     ********************/
/*******************************************************************/
extern HERROR	SVC_SI_Init (PostMsgToMgrCb_t fnPostFunc, PostDataToMgrCb_t fnPostData);
extern HERROR	SVC_SI_SetCallback (SvcSi_GetSysConfigCb_t fnSetSysConfig);
extern HERROR	SVC_SI_SetAudioPriorityTable (SvcSi_EsTypePriority_t *astTable, HUINT32 ulNum);
extern HERROR	SVC_SI_SetSubtitlePriorityTable (SvcSi_EsTypePriority_t *astTable, HUINT32 ulNum);

extern HERROR	SVC_SI_StartLive (Handle_t hAction, SvcSi_LiveSpec_t *pstSiSpec, Handle_t hSvc, HBOOL bSync, HBOOL bFccSvc);
extern HERROR	SVC_SI_StopLive (Handle_t hAction, Handle_t hNextSvc, HBOOL bSync);

extern HERROR	SVC_SI_StartRec (Handle_t hAction, SvcSi_RecSpec_t *pstSiSpec, Handle_t hSvc, HBOOL bSync);
extern HERROR	SVC_SI_StopRec (Handle_t hAction, Handle_t hNextSvc, HBOOL bSync);

extern HERROR	SVC_SI_StartSatip (Handle_t hAction, SvcSi_SatipSpec_t *pstSiSpec, Handle_t hSvc, HBOOL bSync);
extern HERROR	SVC_SI_StopSatip (Handle_t hAction, Handle_t hNextSvc, HBOOL bSync);

extern HERROR	SVC_SI_StartHbbtv (Handle_t hAction, HUINT16 usOnId, HUINT16 usTsId, HUINT16 usSvcId, HBOOL bSync);
extern HERROR	SVC_SI_StopHbbtv (Handle_t hAction, HBOOL bSync);

extern HERROR	SVC_SI_StartPb (Handle_t hAction, SvcSi_PbSpec_t *pstSiSpec, SvcSi_PlaybackOption_t *pstPbOption, HBOOL bSync);
extern HERROR	SVC_SI_StopPb (Handle_t hAction, HBOOL bSync);

extern HERROR 	SVC_SI_StartMediaPb (Handle_t hAction, SvcSi_MediaPbSpec_t *pstSiSpec, HBOOL bSync);
extern HERROR 	SVC_SI_StopMediaPb (Handle_t hAction, HBOOL bSync);

extern HERROR	SVC_SI_StartSearch (Handle_t hAction, SvcSi_SearchSpec_t *pstSrchSpec, SvcSi_SearchOption_t *pstSrchOption, HBOOL bSync);
extern HERROR	SVC_SI_StopSearch (Handle_t hAction, HBOOL bSync);

extern HERROR	SVC_SI_StartExtSearch (Handle_t hAction, SvcSi_ExtSearchSpec_t *pstSrchSpec, SvcSi_ExtSearchOption_t *pstSrchOption, HBOOL bSync);
extern HERROR	SVC_SI_StopExtSearch (Handle_t hAction, HBOOL bSync);
extern HERROR 	SVC_SI_SetExtSearchOption(Handle_t hAction, SvcSi_ExtSearchSpec_t *pstExtSrchSpec, SvcSi_ExtSrchOptionType_t eOptionType, HUINT32 ulParam1, HUINT32 ulParam2, HUINT32 ulParam3);
extern HERROR 	SVC_SI_GetExtSearchData(Handle_t hAction, SvcSi_ExtSearchSpec_t *pstExtSrchSpec, SvcSi_ExtSrchDataType_t eDataType, HUINT32 *pulDataNum, void **ppvDataArray);
extern HERROR 	SVC_SI_FreeExtSearchData(Handle_t hAction, SvcSi_ExtSearchSpec_t *pstExtSrchSpec, SvcSi_ExtSrchDataType_t eDataType, void *pvDataArray);

extern HERROR 	SVC_SI_StartSatRecord (Handle_t hAction, SvcSi_SatRecordSpec_t *pstSrchSpec, SvcSi_SatRecordOption_t *pstSrchOption, HBOOL bSync);
extern HERROR 	SVC_SI_StopSatRecord (Handle_t hAction, HBOOL bSync);
extern HERROR 	SVC_SI_GetSatRecordData(Handle_t hAction, SvcSi_SatRecordSpec_t *pstSatRecordSpec, SvcSi_SatRecordDataType_t eDataType, HUINT32 *pulDataNum, void **ppvDataArray);
extern HERROR 	SVC_SI_FreeSatRecordData(Handle_t hAction, SvcSi_SatRecordSpec_t *pstSatRecordSpec, SvcSi_SatRecordDataType_t eDataType, void *pvDataArray);

extern HERROR	SVC_SI_StartSwupCheck (Handle_t hAction, SvcSi_SwupCheckSpec_t *pstSwupSpec, SvcSi_SwupCheckOption_t *pstOption, HBOOL bSync);
extern HERROR	SVC_SI_StopSwupCheck (Handle_t hAction, HBOOL bSync);
extern HERROR	SVC_SI_SetSwupCheckParam (SvcSi_SwupCheckSpec_t *pstSwupSpec, SvcSi_SwupFlag_t *pstSwupCheckParam);
extern HERROR 	SVC_SI_GetSwupCheckData(Handle_t hAction, SvcSi_SwupCheckSpec_t *pstSwupSpec, HUINT32 ulDataType, HUINT32 *pulDataNum, void **ppvDataArray);
extern HERROR 	SVC_SI_FreeSwupCheckData(Handle_t hAction, SvcSi_SwupCheckSpec_t *pstSwupCheckSpec, HUINT32 ulDataType, void *pvDataArray);

extern HERROR 	SVC_SI_StartLiveSwupCheck (Handle_t hAction, SvcSi_LiveSwupSpec_t *pstSpec, SvcSi_LiveSwupOption_t *pstOption, HBOOL bSync);
extern HERROR 	SVC_SI_StopLiveSwupCheck (Handle_t hAction, HBOOL bSync);

extern HERROR 	SVC_SI_StartTimeupdate(Handle_t hAction, SvcSi_TimeUpdateSpec_t *pstSiSpec, HBOOL bUseTot, HBOOL bSync);
extern HERROR	SVC_SI_StopTimeupdate(Handle_t hAction, HBOOL bSync);

extern HERROR 	SVC_SI_StartDynamicUpdate (Handle_t hAction, SvcSi_DynamicUpdateSpec_t *pstSpec, SvcSi_DynamicUpdateOption_t *pstOption, HBOOL bSync);
extern HERROR 	SVC_SI_StopDynamicUpdate (Handle_t hAction, HBOOL bSync);

extern HERROR	SVC_SI_GetPmtPidBySvcId (Handle_t hAction, HUINT32 ulSvcId, HUINT16 *pusPmtPid);
extern HERROR	SVC_SI_GetLiveServiceInfoPid (Handle_t hAction, HUINT32 ulSvcId, DbSvc_Info_t *pstSrcSvcInfo, DbSvc_Info_t *pstDstSvcInfo, HBOOL *pbChanged);
extern HERROR	SVC_SI_GetLiveServiceInfo(Handle_t hAction, HUINT32 ulSvcId, DbSvc_Info_t *pstSrcSvcInfo, DbSvc_Info_t *pstDstSvcInfo, HBOOL *pbChanged);
#if 0
extern HERROR	SVC_SI_GetServiceInfoPid (Handle_t hAction, HUINT32 ulSvcId, void *pvSrcInfo, void *pvDstInfo, HBOOL *pbChanged);
#endif

#if defined(SvcSi_SUPPORT_LIB_ES_NEW)
extern HERROR	SVC_SI_UpdateServiceInfo (Handle_t hAction, HUINT32 ulSvcId, SvcSi_UpdateEs_t *pstUpdate, HBOOL *pbChanged);
extern HERROR	SVC_SI_GetServiceInfoPid2 (Handle_t hAction, HUINT32 ulSvcId, SvcSi_UpdateEs_t *pstUpdate, HBOOL *pbChanged);

extern HERROR	SVC_SI_GetEsInfo (SvcSi_EsUpdateType_e eType, Handle_t hEsInfoList, HINT32 nIndex, HUINT16 *pusPid, HUINT32 *pulCodec, HUINT8 *pucComponentTag, HUINT32 *pulParam);
#endif

extern HERROR	SVC_SI_UpdateAllService_inSDT(HUINT32 ulActionId, const DbSvc_Info_t *pstSvcInfo, HBOOL *pbChanged);
extern HERROR 	SVC_SI_GetEvtInfoFromPfEvent (HUINT32 ulActionId, DbSvc_TripleId_t stTripleId, HBOOL bNow, HBOOL bOnlyEit, SvcSi_Event_t *pEvent, SvcSi_ExtEvent_t *pExtEvent);
extern HERROR 	SVC_SI_FreeEventInfo (SvcSi_Event_t *pstEvent);
extern HERROR	SVC_SI_FreeExtEventInfo(SvcSi_ExtEvent_t * pstExtEvent);
extern HERROR	SVC_SI_GetVideoList (HUINT32 ulActionId, HUINT16 usSvcId, SvcSi_VideoList_t *pstVideoList);
extern HERROR 	SVC_SI_GetAudioList(HUINT32 ulActionId, HUINT16 usSvcId, SvcSi_AudioList_t *pstAudioList);
extern HERROR	SVC_SI_GetEcmList (HUINT32 ulActionId, HUINT16 usSvcId, SvcSi_EcmList_t *pstEcmList);
extern HERROR 	SVC_SI_GetSubtitleList(HUINT32 ulActionId, HUINT16 usSvcId, SvcSi_SubtitleList_t *pstSubtitleList);
extern HERROR 	SVC_SI_GetSuperCcList (HUINT32 ulActionId, HUINT16 usSvcId, SvcSi_SubtitleList_t *pstSuperCcList);
extern HERROR	SVC_SI_FreeVideoList (SvcSi_VideoList_t *pstVideoList);
extern HERROR 	SVC_SI_FreeAudioList (SvcSi_AudioList_t *pstAudioList);
extern HERROR 	SVC_SI_FreeSubtitleList (SvcSi_SubtitleList_t *pstSubtitleList);
extern HERROR 	SVC_SI_FreeSuperCcList (SvcSi_SubtitleList_t *pstSubtitleList);
extern HERROR	SVC_SI_FreeEcmList (SvcSi_EcmList_t *pstEcmList);

extern HERROR	SVC_SI_GetEmmList (HUINT32 ulActionId, HUINT16 usSvcId, SvcSi_EmmList_t *pstEmmList);
extern HERROR	SVC_SI_FreeEmmList (SvcSi_EmmList_t *pstEmmList);

extern HERROR	SVC_SI_CountSearchedResult (HUINT32 ulActionId, SvcSi_SrchInfoType_e eInfoType, HUINT32 *pulInfoNum);
extern HERROR	SVC_SI_GetSearchedResults (HUINT32 ulActionId, SvcSi_SrchInfoType_e eInfoType, HUINT32 ulMaxArrayNum, HUINT32 *pulInfoNum, void *pvInfoArray);

extern SIxTable_PsiTable_t*	SVC_SI_ParseTable (HUINT32 ulActionId, SIxTable_Type_e eSiType, HUINT32 ulExtId);
extern void		SVC_SI_FreeTable (SIxTable_PsiTable_t *pstTable);

#if defined(CONFIG_3RD_MHEG)
extern HERROR 	SVC_SI_GetMhegPatRawSection(HUINT32 ulActionId, HUINT32 ulRawArrayMax, HUINT8 **ppucRawArray, HUINT32 *pulListCount);
extern HERROR 	SVC_SI_GetMhegPmtRawSection(HUINT32 ulActionId, HUINT32 ulRawArrayMax, HUINT8 **ppucRawArray, HUINT32 *pulListCount);
#endif

extern HERROR 	SVC_SI_MakeSiSection (HUINT32 ulActionId, SvcSi_MakeSectionSpec_t *pstSpec, SvcSi_MakeSecArg_t *punArg, HUINT8 *pucRawSec, HUINT32 *pulSecLen);
extern HERROR 	SVC_SI_GetAitRawSection(HUINT32 ulActionId, HUINT32 ulRawArrayMax, HUINT8 **ppucRawArray, HUINT32 *pulListCount);
extern HERROR	SVC_SI_GetAitPidFromPMT(HUINT32 ulActionId, HxList_t **pstAitList);
extern HERROR	SVC_SI_SetLcnServiceNumber(Handle_t hAction, HBOOL bIsNewTp, HUINT32 ulNum, DbSvc_Info_t *pstSvcArray, SRCHSVC_AppendFlag_t *peFlagArray);
extern HERROR	SVC_SI_SetSelectedConflictLcn(Handle_t hAction, HUINT32 ulNum, DxBuilder_SearchedInfo_t* pstInfoArray);
extern HERROR 	SVC_SI_PostProcessForSvcNumbering (Handle_t hAction, DefaultLcnPolicy_t eLcnPolicy, DxDeliveryType_e eDeliType);
extern HERROR	SVC_SI_PostProcessForSimulcastService(Handle_t hAction);
extern HERROR 	SVC_SI_UpdateLiveTimeByTot (Handle_t hAction);

#if defined(CONFIG_MW_SI_LOCALTIME)
extern HERROR 	SVC_SI_UpdateLocaltime_linkTzDB(HxCountry_e eCountryId, HINT32 nTimeRegion);
#endif

extern HERROR 	SVC_SI_GetSiProperty (HUINT32 ulActionId, SIxSpec_e *peSpec, SIxTextEncoding_e *peTextEnc);
extern HERROR	SVC_SI_SetSiMode (DxSIMode_e ulSiMode);
extern HERROR	SVC_SI_GetSiMode (DxSIMode_e *pulSiMode);
extern HERROR	SVC_SI_GetSwupFlag (Handle_t hAction, HUINT32 *pulSwupFlag);

extern HERROR	SVC_SI_GetWebPmtList(Handle_t hAction, SvcSi_WebPmtInfo_t **apList, HUINT32 *punListCount, HUINT16 *pusTsUniqId);
extern HERROR 	SVC_SI_GetRctInfo(Handle_t hAction, Handle_t hSvc, DxService_RctInfo_t *pstRctInfo, HBOOL bMhegInterruptibleFlag);

extern HERROR 	SVC_SI_GetTrdConflictedList(Handle_t hAction, HUINT32 ulDepth,HUINT32 *pulconflictcount,HxList_t **ppConflictList);
extern HERROR	SVC_SI_SetTrdConflictedList(HUINT32 ulDepthIdx, HUINT32 index);
extern HERROR 	SVC_SI_InitTrdConflicteSvcHandleTable(void);
extern HERROR	SVC_SI_SetNcdNotifyData(HUINT32 ulASONotifyFlag);
extern HERROR	SVC_SI_ClearNcdNotifyData(void);

extern HERROR	SVC_SI_HomeChannelProcessNit(Handle_t hAction, HINT32 nDeliType, HUINT16 usTsUniqId, HUINT32 ulHead);
extern HERROR	SVC_SI_HomeChannelGetChannelInfo(DbSvc_TripleId_t *pstChannelInfo, HINT32 *pnOrgDeliType, HBOOL *pbValid);


#endif	// __SVC_SI__
/* end of file */

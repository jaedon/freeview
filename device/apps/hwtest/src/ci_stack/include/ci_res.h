/* $Header:   //BASE/archives/STING_T/mw/cas/ci/include/ci_res.h-arc   1.0   17 Dec 2004 10:32:38   jhkim1  $ */
/***************************************************************
* $Workfile:   ci_res.h  $
* $Modtime:   13 Dec 2004 09:27:32  $
* Auther : Jaehee Cho / hmkim (CI+)
*
* Desc :
*
* Copyright (c) 2002 by Humax Co., Ltd.
* All right reserved
****************************************************************/

#ifndef _CI_RES_H_
#define _CI_RES_H_

#ifdef __cplusplus
extern "C" {
#endif

/***************************************************************
* include
***************************************************************/


/***************************************************************
* definition
***************************************************************/

/* resource class */
#define CI_RM_CLASS		0x0001	/* Resource Manager */
#define CI_AI_CLASS		0x0002	/* Application Information */
#define CI_CAS_CLASS		0x0003	/* Conditional Access Support */
#define CI_HCI_CLASS		0x0020	/* Host Control & Information */
#define CI_DT_CLASS		0x0024	/* Date & Time */
#define CI_MMI_CLASS		0x0040 /* Low & High Level MMI */
#define CI_AMMI_CLASS		0x0041	/* CI+ Browser Application MMI */
#define CI_LSC_CLASS		0x0060	/* Low Speed Communication */
#define CI_CC_CLASS		0x008c	/* Content Control */
#define CI_HLC_CLASS		0x008d	/* Host Language & Country */
#define CI_CU_CLASS		0x008e	/* CAM Firmware Upgrade */
#define CI_SAS_CLASS		0x0096	/* Specific Application Support for MHP */
#define CI_CAPVR_CLASS	0x0097	/* CA PVR */
#define CI_AUTH_CLASS		0x0010	/* Authentication *//* used with _INCLUDE_BD_CI_ only */
#define CI_CP_CLASS		0x0004	/* Copy Protection *//* used with _INCLUDE_BD_CI_ only */

/* Max. Number of Session Of Resource */
#define MAX_CI_RM_SS		16	// has no session limit (CI 스펙)
#define MAX_CI_AI_SS		16	// has no session limit (CI 스펙) -> 하지만 사실상 slot 당 1개 세션 밖에 운용될 수 없다
#define MAX_CI_CAS_SS		16	// has no session limit (CI 스펙) -> 하지만 사실상 slot 당 1개 세션 밖에 운용될 수 없다
#define MAX_CI_HCI_SS		2	// Only the host provides the DVB Host Control resource, and it can only support one session at a time (CI 스펙) -> 2 slot 지원까지 고려하여 2로 define
#define MAX_CI_DT_SS		16	// unlimited sessions (CI 스펙)
#define MAX_CI_MMI_SS		16	// Whether the host can support more than one MMI session simultaneously is a host design choice (CI 스펙)
#define MAX_CI_AMMI_SS	16	// CI+ 스펙에선 명확히 기술되어 있지 않지만 MMI에 준할 것으로 생각된다 (단, MHEG library는 한 세션 밖에 처리할 수 없음)
#define MAX_CI_LSC_SS		2	// The host supports only one connection per session, but the host may support several sessions in parallel (CI/CI+ 스펙)
#define MAX_CI_CC_SS		2	// The host shall support only one session to the CC resource per CI+ slot (CI+ 스펙) -> 2 slot 지원까지 고려하여 2로 define
#define MAX_CI_HLC_SS		2	// The resource shall support one session per CICAM (CI+ 스펙) -> 2 slot 지원까지 고려하여 2로 define
#define MAX_CI_CU_SS		2	// CI+ 스펙에선 명확히 기술되어 있지 않지만 slot 당 1개 세션일 것으로 생각된다
#define MAX_CI_SAS_SS		16
#define MAX_CI_CAPVR_SS	16
#define MAX_CI_AUTH_SS	2	/* used with _INCLUDE_BD_CI_ only */
#define MAX_CI_CP_SS		16	/* used with _INCLUDE_BD_CI_ only -> CP 리소스는 CC 리소스와 함께 사용될 수는 있으나 동시에 세션을 오픈해서는 안된다 (CI+ 스펙) */

/************************************************/
/*************** APDU Tag Value *****************/
/************************************************/
/* RM(Resource Manager) */
#define CI_PROFILE_ENQ				0x9f8010	// host <-> app
#define CI_PROFILE_REPLY				0x9f8011	// host <-> app
#define CI_PROFILE_CHANGED			0x9f8012	// host <-> app
#define CI_MODULE_ID_SEND 			0x9f8013 	// host <-- app, 현재 지원하지 않음 (현재로선 불필요).
#define CI_MODULE_ID_COMMAND  		0x9f8014 	// host --> app, 현재 지원하지 않음 (현재로선 불필요).

/* AI(Application Information) */
#define CI_APPL_INFO_ENQ				0x9f8020	// host --> app
#define CI_APPL_INFO_REPLY			0x9f8021	// host <-- app
#define CI_ENTER_MENU					0x9f8022	// host --> app
#define CI_REQUEST_CICAM_RESET		0x9f8023	// host <-- app, CI+ mandatory.
#define CI_DATA_RATE_INFO  			0x9f8024	// host --> app, CI+ mandatory.

/* CAS(Conditional Access Support) */
#define CI_CA_INFO_ENQ				0x9f8030	// host --> app
#define CI_CA_INFO_REPLY				0x9f8031	// host <-- app
#define CI_CA_PMT						0x9f8032	// host --> app
#define CI_CA_PMT_REPLY				0x9f8033	// host <-- app, CI+ 에선 CAM mandatory 사항이다.

/* HCI(Host Control & Information) */	// CI+ mandatory.
#define CI_TUNE						0x9f8400	// host <-- app
#define CI_REPLACE						0x9f8401	// host <-- app
#define CI_CLEAR_REPLACE				0x9f8402	// host <-- app
#define CI_ASK_RELEASE				0x9f8403	// host --> app, 현재 지원하지 않음 (현재로선 불필요).

/* DT(Date & Time) */
#define CI_DATE_TIME_ENQ				0x9f8440	// host <-- app
#define CI_DATE_TIME_REPLY			0x9f8441	// host --> app

/* MMI */
#define CI_CLOSE_MMI					0x9f8800	// host <-> app
#define CI_DISPLAY_CONTROL			0x9f8801	// host <-- app
#define CI_DISPLAY_REPLY				0x9f8802	// host --> app
#define CI_TEXT_LAST					0x9f8803	// host <-- app
#define CI_TEXT_MORE					0x9f8804	// host <-- app
#define CI_KEYPAD_CONTROL				0x9f8805	// host <-- app, 현재 지원하지 않음 (현재로선 불필요).
#define CI_KEYPRESS					0x9f8806	// host --> app, 현재 지원하지 않음 (현재로선 불필요).
#define CI_ENQ							0x9f8807	// host <-- app
#define CI_ANSW						0x9f8808	// host --> app
#define CI_MENU_LAST					0x9f8809	// host <-- app
#define CI_MENU_MORE					0x9f880a	// host <-- app
#define CI_MENU_ANSW					0x9f880b	// host --> app
#define CI_LIST_LAST					0x9f880c	// host <-- app
#define CI_LIST_MORE					0x9f880d	// host <-- app
#define CI_SUBTITLE_SEG_LAST			0x9f880e	// host <-- app, 현재 지원하지 않음 (현재로선 불필요).
#define CI_SUBTITLE_SEG_MORE			0x9f880f	// host --> app, 현재 지원하지 않음 (현재로선 불필요).
#define CI_DISPLAY_MSG				0x9f8810	// host <-- app, 현재 지원하지 않음 (현재로선 불필요).
#define CI_SCENE_END_MARK				0x9f8811	// host <-- app, 현재 지원하지 않음 (현재로선 불필요).
#define CI_SCENE_DONE					0x9f8812	// host <-- app, 현재 지원하지 않음 (현재로선 불필요).
#define CI_SCENE_CONTROL				0x9f8813	// host --> app, 현재 지원하지 않음 (현재로선 불필요).
#define CI_SUBTITLE_DOWNLOAD_LAST	0x9f8814	// host <-- app, 현재 지원하지 않음 (현재로선 불필요).
#define CI_SUBTITLE_DOWNLOAD_MORE	0x9f8815	// host --> app, 현재 지원하지 않음 (현재로선 불필요).
#define CI_FLUSH_DOWNLOAD				0x9f8816	// host <-- app, 현재 지원하지 않음 (현재로선 불필요).
#define CI_DOWNLOAD_REPLY				0x9f8817	// host <-- app, 현재 지원하지 않음 (현재로선 불필요).

/* LSC(Low Speed Communication) */	// 현재 지원하지 않음 (현재로선 불필요).
#define CI_COMMS_CMD					0x9f8c00	// host <-- app
#define CI_CONNECTION_DESCRIPTOR		0x9f8c01	// host <-- app
#define CI_COMMS_REPLY				0x9f8c02	// host --> app
#define CI_COMMS_SEND_LAST			0x9f8c03	// host <-- app
#define CI_COMMS_SEND_MORE			0x9f8c04	// host <-- app
#define CI_COMMS_RCV_LAST				0x9f8c05	// host --> app
#define CI_COMMS_RCV_MORE				0x9f8c06	// host --> app

/* CC(Content Control) */	// CI+ mandatory.
#define CI_CC_OPEN_REQ				0x9f9001	// host <-- app
#define CI_CC_OPEN_CNF				0x9f9002	// host --> app
#define CI_CC_DATA_REQ				0x9f9003	// host <-- app
#define CI_CC_DATA_CNF				0x9f9004	// host --> app
#define CI_CC_SYNC_REQ				0x9f9005	// host <-- app
#define CI_CC_SYNC_CNF				0x9f9006	// host --> app
#define CI_CC_SAC_DATA_REQ			0x9f9007	// host <-- app
#define CI_CC_SAC_DATA_CNF			0x9f9008	// host --> app
#define CI_CC_SAC_SYNC_REQ			0x9f9009	// host <-- app
#define CI_CC_SAC_SYNC_CNF			0x9f9010	// host --> app

/* HLC(Host Language & Country) */	// CI+ mandatory.
#define CI_HOST_COUNTRY_ENQ			0x9f8100	// host <-- app
#define CI_HOST_COUNTRY_REPLY		0x9f8101	// host --> app
#define CI_HOST_LANGUAGE_ENQ			0x9f8110	// host <-- app
#define CI_HOST_LANGUAGE_REPLY		0x9f8111	// host --> app

/* CAM Firmware Upgrade */	// CI+ mandatory.
#define CI_CAM_UPGRADE				0x9f9d01	// host <-- app
#define CI_CAM_UPGRADE_REPLY			0x9f9d02	// host --> app
#define CI_CAM_UPGRADE_PROGRESS		0x9f9d03	// host <-- app
#define CI_CAM_UPGRADE_COMPLETE		0x9f9d04	// host <-- app

/* Specific Application Support for MHP */	// 현재 지원하지 않음 (현재로선 불필요).
#define CI_SAS_CONNECT_RQST			0x9f9a00	// host --> app
#define CI_SAS_CONNECT_CNF			0x9f9a01	// host <-- app
#define CI_SAS_DATA_RQST				0x9f9a02	// host <-> app
#define CI_SAS_DATA_AV				0x9f9a03	// host <-> app
#define CI_SAS_DATA_CNF				0x9f9a04	// host <-> app
#define CI_SAS_SERVER_QUERY			0x9f9a05	// host <-> app
#define CI_SAS_SERVER_REPLY			0x9f9a06	// host <-> app
#define CI_SAS_ASYNC_MSG				0x9f9a07	// host <-> app

/* CA PVR */	// 현재 지원하지 않음 (현재로선 불필요).
#define CI_CAPVR_INFO_ENQ				0x9fa401	// host --> app
#define CI_CAPVR_INFO_REPLY			0x9fa402	// host <-- app
#define CI_CAPVR_PMT					0x9fa403	// host --> app
#define CI_CAPVR_PMT_REPLY			0x9fa404	// host <-- app
#define CI_CAPVR_CAT					0x9fa405	// host --> app
#define CI_CAPVR_CAT_REPLY			0x9fa406	// host <-- app
#define CI_CAPVR_EMM_CMD				0x9fa407	// host --> app
#define CI_CAPVR_EMM_CMD_REPLY		0x9fa408	// host <-- app
#define CI_CAPVR_ECM_CMD				0x9fa409	// host --> app
#define CI_CAPVR_ECM_CMD_REPLY		0x9fa40a	// host <-- app
#define CI_CAPVR_PINCODE_CMD			0x9fa40b	// host --> app
#define CI_CAPVR_PINCODE_CMD_REPLY	0x9fa40c	// host <-- app

/* CI+ Browser Application MMI (see CI+ 스펙 1.2, TS 101 699 1.1.1, D-BOOK 5.0) */	// CI+ mandatory.
#define CI_REQ_START					0x9f8000	// host <-- app, TS 101 699 스펙에 명시된 syntax 사용
#define CI_REQ_START_ACK				0x9f8001	// host --> app, TS 101 699 스펙에 명시된 syntax 사용
#define CI_FILE_REQ					0x9f8002	// host --> app, CI+ 스펙에 명시된 syntax 사용
#define CI_FlLE_ACK					0x9f8003	// host <-- app, CI+ 스펙에 명시된 syntax 사용
#define CI_APP_ABORT_REQ				0x9f8004	// host <-> app, TS 101 699 스펙에 명시된 syntax 사용 (abort codes는 CI+ 스펙에 명시된 것 사용)
#define CI_APP_ABORT_ACK				0x9f8005	// host <-> app, TS 101 699 스펙에 명시된 syntax 사용

/* AUTH(Authentication) */	// used with _INCLUDE_BD_CI_ only
#define CI_AUTH_REQ					0x9f8200	// host <-- app
#define CI_AUTH_RESP					0x9f8201	// host --> app

/* CP(Copy Protection) */	// used with _INCLUDE_BD_CI_ only
#define CI_CP_QUERY					0x9f8000	// host <-- app
#define CI_CP_REPLY					0x9f8001	// host --> app
#define CI_CP_COMMAND					0x9f8002	// host <-- app
#define CI_CP_RESPONSE				0x9f8003	// host --> app


#define	CI_APDU_TIMER					0xf0
#define	CI_DT_TIMER					CI_APDU_TIMER


/***************************************************************
* typedef
***************************************************************/


/***************************************************************
* global function prototypes
***************************************************************/


#ifdef __cplusplus
}
#endif

#endif /* _CI_RES_H_ */


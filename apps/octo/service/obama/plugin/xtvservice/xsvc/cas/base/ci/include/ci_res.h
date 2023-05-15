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

/*--- Resource IDs ---*/

#define	CI_RM_RES_V1_ID				0x00010041		// Resource Manager v1
#define	CI_RM_RES_V2_ID				0x00010042		// Resource Manager v2 : currently v1 APDUs only supported.
#define	CI_AI_RES_V1_ID				0x00020041		// Application Information v1
#define	CI_AI_RES_V2_ID				0x00020042		// Application Information v2
#define	CI_AI_RES_V3_ID				0x00020043		// Application Information v3 : CI+ mandatory.
#define	CI_CAS_RES_V1_ID			0x00030041		// Conditional Access Support
#define	CI_HCI_RES_V1_ID			0x00200041		// Host Control Interface v1 : CI+ mandatory.
#define	CI_HCI_RES_V2_ID			0x00200042		// Host Control Interface v2 : CI+ v1.3 mandatory.
#define	CI_DT_RES_V1_ID				0x00240041		// Date-Time
#define	CI_MMI_RES_V1_ID			0x00400041		// Low & High Level MMI : currently HL-MMI only supported and it's enough.
#define	CI_AMMI_RES_V1_ID			0x00410041		// CI+ Browser Application MMI v1 : CI+ mandatory.
#define	CI_AMMI_RES_V2_ID			0x00410042		// CI+ Browser Application MMI v2 : CI+ v1.3 mandatory.
#define	CI_LSC_RES_V1_ID			0x00600001		// Low Speed Communication v1 (00 60 xx x1)
#define	CI_LSC_RES_V2_ID			0x00600002		// Low Speed Communication v2 (00 60 xx x2)
// Low Speed Communication V3 에서 IP Connection 에 대한 resource ID 값은 0x006060X3 값을 갖음. X에 대한 값은 0~3 번까지 나타 낼 수 있으며, Host가 Network 장치를 여러개 포함할 때, IP(network) device number를 나타낸다.
#define	CI_LSC_IP_RES_V3_ID			0x00606003		// Low Speed Communication v3 IP Connection + 0번 IP device  : CI+ v1.3 mandatory (where Host IP supports exists)
//#define	CI_LSC_IP_RES_V3_ID_2	0x00606013		// Low Speed Communication v3 IP Connection + 1번 IP device
//#define	CI_LSC_IP_RES_V3_ID_3	0x00606023		// Low Speed Communication v3 IP Connection + 2번 IP device
//#define	CI_LSC_IP_RES_V3_ID_4	0x00606033		// Low Speed Communication v3 IP Connection + 3번 IP device
#define	CI_CC_RES_V1_ID				0x008c1001		// Content Control v1 : CI+ mandatory.
#define	CI_CC_RES_V2_ID				0x008c1002		// Content Control v2 : CI+ v1.3 mandatory.

#define CI_HLC_RES_V1_ID			0x008d1001		// Host Language & Country : CI+ mandatory.
#define CI_CU_RES_V1_ID				0x008e1001		// CAM Firmware Upgrade : CI+ mandatory.
#define	CI_OP_RES_V1_ID				0x008f1001		// Operator Profile : CI+ v1.3 mandatory.
#define CI_SAS_RES_V1_ID			0x00961001		// Specific Application Support for MHP : CI+ optional.
#define CI_CAPVR_RES_V1_ID			0x00971001		// CA PVR : CI+ optional.	(removed from CI+ v1.3).

#define	CI_AUTH_RES_V1_ID			0x00100041		// Authentication : used with _INCLUDE_BD_CI_ only (also CONFIG_OP_CANALREADY)
#define	CI_CP_RES_V2_ID				0x00041001		// Copy Protection : used with _INCLUDE_BD_CI_ only -> CP 리소스는 CC 리소스와 함께 사용될 수는 있으나 동시에 세션을 오픈해서는 안된다 (CI+ 스펙)

/*--- Resource Classes ---*/

#define	CI_RM_CLASS					0x0001		// Resource Manager
#define	CI_AI_CLASS					0x0002		// Application Information
#define	CI_CAS_CLASS				0x0003		// Conditional Access Support
#define	CI_HCI_CLASS				0x0020		// Host Control Interface
#define	CI_DT_CLASS					0x0024		// Date-Time
#define	CI_MMI_CLASS				0x0040		// Low & High Level MMI
#define	CI_AMMI_CLASS				0x0041		// CI+ Browser Application MMI
#define	CI_LSC_CLASS				0x0060		// Low Speed Communication
#define	CI_CC_CLASS					0x008c		// Content Control
#define	CI_HLC_CLASS				0x008d		// Host Language & Country
#define	CI_CU_CLASS					0x008e		// CAM Firmware Upgrade
#define	CI_OP_CLASS					0x008f		// Operator Profile
#define	CI_SAS_CLASS				0x0096		// Specific Application Support for MHP
#define	CI_CAPVR_CLASS				0x0097		// CA PVR

#define	CI_AUTH_CLASS				0x0010		// Authentication : used with _INCLUDE_BD_CI_ only (also CONFIG_OP_CANALREADY)
#define	CI_CP_CLASS					0x0004		// Copy Protection : used with _INCLUDE_BD_CI_ only

/*--- Max. Number of Session for each resource ---*/

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
#define	MAX_CI_OP_SS			2			// CI+ 스펙에선 명확히 기술되어 있지 않지만 slot 당 1개 세션일 것으로 생각된다 (확인 필요)
#define MAX_CI_SAS_SS		16
#define MAX_CI_CAPVR_SS	16	// CI+ 스펙에선 명확히 기술되어 있지 않지만 CAS에 준할 것으로 생각된다

#define	MAX_CI_AUTH_SS		2	// used with _INCLUDE_BD_CI_ only (also CONFIG_OP_CANALREADY)
#define	MAX_CI_CP_SS		16	// used with _INCLUDE_BD_CI_ only -> CP 리소스는 CC 리소스와 함께 사용될 수는 있으나 동시에 세션을 오픈해서는 안된다 (CI+ 스펙)

/*--- Other definitions related with resources ---*/

#define	CI_HOST_RES			0xff		// CI Host Resource
#define	CI_RES_PRI_TYPE		3			// 0~2 for public resource, 3 for private resource
#define	MAX_CI_RES_INST		20			// temporary value. to be changed optimal value

#define	CI_APDU_TIMER		0xf0
#define	CI_DT_TIMER			CI_APDU_TIMER

/************************************************/
/*************** APDU Tag Value *****************/
/************************************************/
/* RM (Resource Manager) */
#define CI_PROFILE_ENQ				0x9f8010	// host <-> app
#define CI_PROFILE_REPLY				0x9f8011	// host <-> app
#define CI_PROFILE_CHANGED			0x9f8012	// host <-> app
#define CI_MODULE_ID_SEND 			0x9f8013 	// host <-- app : 현재 지원하지 않음 (현재로선 불필요).
#define CI_MODULE_ID_COMMAND  		0x9f8014 	// host --> app : 현재 지원하지 않음 (현재로선 불필요).

/* AI (Application Information) */
#define CI_APPL_INFO_ENQ				0x9f8020	// host --> app
#define CI_APPL_INFO_REPLY			0x9f8021	// host <-- app
#define CI_ENTER_MENU					0x9f8022	// host --> app
#define CI_REQUEST_CICAM_RESET		0x9f8023	// host <-- app : CI+ mandatory.
#define CI_DATA_RATE_INFO  			0x9f8024	// host --> app : CI+ mandatory.

/* CAS (Conditional Access Support) */
#define CI_CA_INFO_ENQ				0x9f8030	// host --> app
#define CI_CA_INFO_REPLY				0x9f8031	// host <-- app
#define CI_CA_PMT						0x9f8032	// host --> app
#define CI_CA_PMT_REPLY				0x9f8033	// host <-- app : CI+ 에선 CAM mandatory 사항이다 (CI_CA_PMT with "query" or "ok_mmi" 에 대한 응답으로). 하나 현재 CI mgr 에서 CI_CPCI_QUERY 를 지원하지 않고 있다.

/* HCI (Host Control & Information) */				// CI+ mandatory.
#define CI_TUNE						0x9f8400	// host <-- app
#define CI_REPLACE						0x9f8401	// host <-- app
#define CI_CLEAR_REPLACE				0x9f8402	// host <-- app
#define CI_ASK_RELEASE				0x9f8403	// host --> app : 현재 지원하지 않음 (현재로선 불필요).
#define CI_TUNE_BROADCAST_REQ			0x9f8404	// host <-- app : CI+ v1.3 mandatory.
#define CI_TUNE_REPLY					0x9f8405	// host --> app : CI+ v1.3 mandatory (CI_TUNE 또는 CI_TUNE_BROADCAST_REQ 에 대한 응답).
#define CI_ASK_RELEASE_REPLY			0x9f8406	// host <-- app : CI+ v1.3 mandatory (CI_ASK_RELEASE 에 대한 응답).

/* DT (Date & Time) */
#define CI_DATE_TIME_ENQ				0x9f8440	// host <-- app
#define CI_DATE_TIME_REPLY			0x9f8441	// host --> app

/* MMI (Man Machine Interface) */
#define CI_CLOSE_MMI					0x9f8800	// host <-> app
#define CI_DISPLAY_CONTROL			0x9f8801	// host <-- app
#define CI_DISPLAY_REPLY				0x9f8802	// host --> app
#define CI_TEXT_LAST					0x9f8803	// host <-- app
#define CI_TEXT_MORE					0x9f8804	// host <-- app
#define CI_KEYPAD_CONTROL				0x9f8805	// host <-- app : 현재 지원하지 않음 (현재로선 불필요).
#define CI_KEYPRESS					0x9f8806	// host --> app : 현재 지원하지 않음 (현재로선 불필요).
#define CI_ENQ							0x9f8807	// host <-- app
#define CI_ANSW						0x9f8808	// host --> app
#define CI_MENU_LAST					0x9f8809	// host <-- app
#define CI_MENU_MORE					0x9f880a	// host <-- app
#define CI_MENU_ANSW					0x9f880b	// host --> app
#define CI_LIST_LAST					0x9f880c	// host <-- app
#define CI_LIST_MORE					0x9f880d	// host <-- app
#define CI_SUBTITLE_SEG_LAST			0x9f880e	// host <-- app : 현재 지원하지 않음 (현재로선 불필요).
#define CI_SUBTITLE_SEG_MORE			0x9f880f	// host --> app : 현재 지원하지 않음 (현재로선 불필요).
#define CI_DISPLAY_MSG				0x9f8810	// host <-- app : 현재 지원하지 않음 (현재로선 불필요).
#define CI_SCENE_END_MARK				0x9f8811	// host <-- app : 현재 지원하지 않음 (현재로선 불필요).
#define CI_SCENE_DONE					0x9f8812	// host <-- app : 현재 지원하지 않음 (현재로선 불필요).
#define CI_SCENE_CONTROL				0x9f8813	// host --> app : 현재 지원하지 않음 (현재로선 불필요).
#define CI_SUBTITLE_DOWNLOAD_LAST	0x9f8814	// host <-- app : 현재 지원하지 않음 (현재로선 불필요).
#define CI_SUBTITLE_DOWNLOAD_MORE	0x9f8815	// host --> app : 현재 지원하지 않음 (현재로선 불필요).
#define CI_FLUSH_DOWNLOAD				0x9f8816	// host <-- app : 현재 지원하지 않음 (현재로선 불필요).
#define CI_DOWNLOAD_REPLY				0x9f8817	// host <-- app : 현재 지원하지 않음 (현재로선 불필요).

/* CI+ Browser Application MMI (see CI+ 스펙 1.2 & 1.3, TS 101 699 1.1.1, D-BOOK 5.0) */ // CI+ mandatory.
#define CI_REQ_START					0x9f8000	// host <-- app : TS 101 699 스펙에 명시된 syntax 사용
#define CI_REQ_START_ACK				0x9f8001	// host --> app : TS 101 699 스펙에 명시된 syntax 사용
#define CI_FILE_REQ						0x9f8002	// host --> app : CI+ 스펙에 명시된 syntax 사용
#define CI_FlLE_ACK						0x9f8003	// host <-- app : CI+ 스펙에 명시된 syntax 사용
#define CI_APP_ABORT_REQ				0x9f8004	// host <-> app : TS 101 699 스펙에 명시된 syntax 사용 (abort codes는 CI+ 스펙에 명시된 것 사용)
#define CI_APP_ABORT_ACK				0x9f8005	// host <-> app : TS 101 699 스펙에 명시된 syntax 사용

/* LSC (Low Speed Communication) */					// 현재 지원하지 않음 (현재로선 불필요) -> CI+ v1.3 mandatory (where Host IP supports exists).
#define CI_COMMS_CMD					0x9f8c00	// host <-- app
#define CI_CONNECTION_DESCRIPTOR	0x9f8c01	// host <-- app
#define CI_COMMS_REPLY				0x9f8c02	// host --> app
#define CI_COMMS_SEND_LAST			0x9f8c03	// host <-- app
#define CI_COMMS_SEND_MORE			0x9f8c04	// host <-- app
#define CI_COMMS_RCV_LAST				0x9f8c05	// host --> app
#define CI_COMMS_RCV_MORE				0x9f8c06	// host --> app

/* CC (Content Control) */							// CI+ mandatory.
#define CI_CC_OPEN_REQ				0x9f9001	// host <-- app
#define CI_CC_OPEN_CNF				0x9f9002	// host --> app
#define CI_CC_DATA_REQ				0x9f9003	// host <-- app
#define CI_CC_DATA_CNF				0x9f9004	// host --> app
#define CI_CC_SYNC_REQ				0x9f9005	// host <-- app
#define CI_CC_SYNC_CNF				0x9f9006	// host --> app
#define CI_CC_SAC_DATA_REQ				0x9f9007	// host <-- app : host <-> app from CI+ v1.3
#define CI_CC_SAC_DATA_CNF				0x9f9008	// host --> app : host <-> app from CI+ v1.3
#define CI_CC_SAC_SYNC_REQ			0x9f9009	// host <-- app
#define CI_CC_SAC_SYNC_CNF			0x9f9010	// host --> app
#define CI_CC_PIN_CAPABILITIES_REQ		0x9f9011	// host --> app : CI+ v1.3 mandatory.
#define CI_CC_PIN_CAPABILITIES_REPLY	0x9f9012	// host <-- app : CI+ v1.3 mandatory.
#define CI_CC_PIN_CMD					0x9f9013	// host --> app : CI+ v1.3 mandatory.
#define CI_CC_PIN_REPLY					0x9f9014	// host <-- app : CI+ v1.3 mandatory.
#define CI_CC_PIN_EVENT					0x9f9015	// host <-- app : CI+ v1.3 mandatory.
#define CI_CC_PIN_PLAYBACK				0x9f9016	// host --> app : CI+ v1.3 mandatory.
#define CI_CC_PIN_MMI_REQ				0x9f9017	// host --> app : CI+ v1.3 mandatory.

/* HLC (Host Language & Country) */					// CI+ mandatory.
#define CI_HOST_COUNTRY_ENQ			0x9f8100	// host <-- app
#define CI_HOST_COUNTRY_REPLY		0x9f8101	// host --> app
#define CI_HOST_LANGUAGE_ENQ			0x9f8110	// host <-- app
#define CI_HOST_LANGUAGE_REPLY		0x9f8111	// host --> app

/* CU (CAM Firmware Upgrade) */						// CI+ mandatory.
#define CI_CAM_UPGRADE				0x9f9d01	// host <-- app
#define CI_CAM_UPGRADE_REPLY			0x9f9d02	// host --> app
#define CI_CAM_UPGRADE_PROGRESS		0x9f9d03	// host <-- app
#define CI_CAM_UPGRADE_COMPLETE		0x9f9d04	// host <-- app

/* OP (Operator Profile) */								// CI+ v1.3 mandatory.
#define CI_OPERATOR_STATUS_REQ		0x9f9c00	// host --> app
#define CI_OPERATOR_STATUS				0x9f9c01	// host <-- app
#define CI_OPERATOR_NIT_REQ			0x9f9c02	// host --> app
#define CI_OPERATOR_NIT_REPLY			0x9f9c03	// host <-- app
#define CI_OPERATOR_INFO_REQ			0x9f9c04	// host --> app
#define CI_OPERATOR_INFO_REPLY		0x9f9c05	// host <-- app
#define CI_OPERATOR_SEARCH_START		0x9f9c06	// host --> app
#define CI_OPERATOR_SEARCH_CANCEL	0x9f9c0c	// host --> app : CI+ v1.3.1
#define CI_OPERATOR_SEARCH_STATUS	0x9f9c07	// host <-- app
#define CI_OPERATOR_EXIT				0x9f9c08	// host --> app
#define CI_OPERATOR_TUNE				0x9f9c09	// host <-- app
#define CI_OPERATOR_TUNE_STATUS		0x9f9c0a	// host --> app
#define CI_OPERATOR_ENTITLEMENT_ACK	0x9f9c0b	// host --> app

/* SAS (Specific Application Support for MHP) */	// CI+ optional.
#define CI_SAS_CONNECT_RQST			0x9f9a00	// host --> app
#define CI_SAS_CONNECT_CNF			0x9f9a01	// host <-- app
#define CI_SAS_DATA_RQST				0x9f9a02	// host <-> app
#define CI_SAS_DATA_AV				0x9f9a03	// host <-> app
#define CI_SAS_DATA_CNF				0x9f9a04	// host <-> app
#define CI_SAS_SERVER_QUERY			0x9f9a05	// host <-> app
#define CI_SAS_SERVER_REPLY			0x9f9a06	// host <-> app
#define CI_SAS_ASYNC_MSG				0x9f9a07	// host <-> app

/* CA PVR - The PVR resource enables the CICAM CAS to manage its own security of content stored and replayed by the host */ // CI+ optional (CI+ v1.3 에서 제거된 feature 임).
#define CI_CA_PVR_INFO_ENQ			0x9fa401	// host --> app
#define CI_CA_PVR_INFO_REPLY			0x9fa402	// host <-- app
#define CI_CA_PVR_PMT					0x9fa403	// host --> app
#define CI_CA_PVR_PMT_REPLY			0x9fa404	// host <-- app
#define CI_CA_PVR_CAT					0x9fa405	// host --> app
#define CI_CA_PVR_CAT_REPLY			0x9fa406	// host <-- app
#define CI_CA_PVR_EMM_CMD				0x9fa407	// host --> app
#define CI_CA_PVR_EMM_CMD_REPLY		0x9fa408	// host <-- app
#define CI_CA_PVR_ECM_CMD				0x9fa409	// host --> app
#define CI_CA_PVR_ECM_CMD_REPLY		0x9fa40a	// host <-- app
#define CI_CA_PVR_PINCODE_CMD		0x9fa40b	// host --> app
#define CI_CA_PVR_PINCODE_CMD_REPLY	0x9fa40c	// host <-- app

/* AUTH (Authentication) */	// used with _INCLUDE_BD_CI_ only (also CONFIG_OP_CANALREADY)
#define CI_AUTH_REQ					0x9f8200	// host <-- app
#define CI_AUTH_RESP					0x9f8201	// host --> app

/* CP (Copy Protection) */	// used with _INCLUDE_BD_CI_ only
#define CI_CP_QUERY					0x9f8000	// host <-- app
#define CI_CP_REPLY					0x9f8001	// host --> app
#define CI_CP_COMMAND					0x9f8002	// host <-- app
#define CI_CP_RESPONSE				0x9f8003	// host --> app


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


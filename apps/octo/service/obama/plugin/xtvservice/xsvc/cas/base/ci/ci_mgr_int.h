/***************************************************************
* $Workfile:   ci_mgr_int.h  $
* $Modtime:   Jan 12 2007 23:26:48  $
* Auther : Jaehee Cho / hmkim (CI+)
*
* Desc :
*
* Copyright (c) 2002 by Humax Co., Ltd.
* All right reserved
****************************************************************/

#ifndef __CIMGR_INT_H__
#define __CIMGR_INT_H__

#ifdef __cplusplus
extern "C" {
#endif

/***************************************************************
* include
***************************************************************/

#include <octo_common.h>

#include <_svc_cas_sub_dev_api.h>


#if defined(CONFIG_MW_CI_PLUS)
#include <ci_plus.h>
#endif

//#if defined(CONFIG_MW_CI_PLUS)
//#include <pal_dsc.h>
//#endif

/***************************************************************
* definition
***************************************************************/
#ifdef CONFIG_MW_CI

	/*------- CI and CI+ host mandatory -------*/

	/* <TODO_CI/TODO_CI_PLUS> : �Ʒ��� CIv1 �� CI+ ���忡 ������� �ʱ� ���� �۾� �ʿ��� ���׵��� */

	/* to use not_selected command in CAPMT :
	   ���� _INCLUDE_NOT_SELECTED_CAPMT_ define ��
	   ä�� ��ȯ�� stop �뵵�� ����ϴ� define ���� ���忡 ���� �ʴ� �߸��� ����̹Ƿ� ������� �ʵ��� �ؾ� �Ѵ�!
	   ��� FTA / other ��ũ���� ������� ca_pmt �� �������� �ؾ� ���忡 �´�!
	   "not selected" ca_pmt �� ä�� ��ȯ�� ������� CI_CPCI_NOT_SELECTED �ּ� �����θ� ���Ǿ�� �Ѵ�! -> �̷� �뵵�� �ʿ��ϸ� �۾� �ʿ��ϴ�.
	   �̿� ���� ������ _INCLUDE_NOT_SELECTED_CAPMT_ define �� _Support_Not_Selected_CA_PMT_ �� _Send_Not_Selected_CA_PMT_on_Zapping_or_RetrySvc_ ���� �����ϰ�
	   _Send_Not_Selected_CA_PMT_on_Zapping_or_RetrySvc_ define�� ��쿣 ������ �ʵ��� �����Ѵ�. */
	//#define _INCLUDE_NOT_SELECTED_CAPMT_	/* old define */
	//#define _Support_Not_Selected_CA_PMT_ /* �̰� ����ϵ� ���ϵ�. ���� �̷� �뵵�� �ʿ�ġ ������ ������� �� ��. CI_CPCI_NOT_SELECTED �ּ� ������ ��. */
	//#define _Send_Not_Selected_CA_PMT_on_Zapping_or_RetrySvc_ /* ������� �� �� */

	/* to send the ca_pmt object even when the selected programme is in the clear. (see CIv1 and CI+ ���� E.2 CA_PMT in Clear) */
#define _Support_CA_PMT_in_Clear_

	/* When the PMT version is changed, the CA_PMT_Update object shall be used... (see CIv1 and CI+ ���� E.3 CA_PMT Clear to Scrambled / Scrambled to Clear and E.4 PMT Update and New CA_PMT) */
#define _Support_CA_PMT_Update_	/* �� OCTO �������� �̰��� �̹� �����Ǿ� �ִ� �������� �߰� �ڵ��� �ʿ�ġ �ʴ� */

	/* The CA PMT contains all the CA_descriptors of the selected programme. (see CIv1 and CI+ ���忡 �� �߰��� ����) */
	//#define _Contains_all_the_CA_descriptors_ /* TODO : �̰� ����ؾ߸� �ϴµ� ���̵� ���� �����ñ��� �ϴ� ���̽����� ���Ƶд�. ����ϴ��� CI+ ķ�� ���ؼ��� �����ϵ��� ó������. */

#endif

#define INVALID_TIMER_ID				VK_TIMERID_NULL

#define	_LIMIT_CI_RETRY_COUNT_			// RetrySvc ó���� Retry Ƚ���� ������ �ش�. see CIMGR_MAX_NUM_OF_RETRY.
#define	CIMGR_MAX_NUM_OF_RETRY			5  // 3 // BS : �̰� ����Ȱ�...
#define	CIMGR_MIN_RETRY_PERIOD			12000 // 8000 //5000
#define	CIMGR_RETRY_PERIOD				CIMGR_MIN_RETRY_PERIOD

/* CIMGR_MSG_SIZE */
#define	MW_CAS_CIMGR_MSG_SIZE			64 // 24

/* CAS mgr ���� SI�� ���� */
#define CAS_SI_STATUS_VALID				TRUE
#define CAS_SI_STATUS_INVALID 			FALSE

#if defined(CONFIG_MW_CI_PLUS)
#define URI_CONTROL_USE_AV_BLOCK
#endif

#define	CI_MAX_NUM_OF_ACT				(CI_MAX_NUM_OF_CAM * CI_MAX_NUM_OF_CAM_SVC)
//#define CI_MAX_NUM_OF_DMX				NUM_MAX_ACTION												// OCTO�� Action �� Dmx 1���� ���� ��.
#define CI_MAX_NUM_OF_DMX				16												// OCTO2 ������ ���� dmx ������ �����Ѵ�.

#define TAG_CI_PROTECTION_DESCRIPTOR	(0xCE)

#define	CI_NUM_OF_BRAND_ID		(30)		// for CI+

/***************************************************************
* typedef
***************************************************************/

typedef enum {
	CI_PLAY_NONE = 0,
	CI_PLAY_STOPPED,
	CI_PLAY_STARTED
} CI_PLAY_STATE;

typedef enum {
	CI_SVC_NOT_EXIST = 0,
	CI_SVC_STOPPED,
	CI_SVC_STARTED,
	CI_SVC_NOT_DESCRAMBLING,
	CI_SVC_DESCRAMBLING
} CI_SVC_STATE;

typedef enum {
	CI_STREAM_UNKNOWN = 0,
	CI_STREAM_SCRAMBLED,
	CI_STREAM_PLAYED
} CI_STREAM_STATE;

typedef enum {
	CI_STREAM_TYPE_Video = 0,
	CI_STREAM_TYPE_Audio,
	CI_STREAM_TYPE_Subtitle,
	CI_STREAM_TYPE_Teletext,
	CI_STREAM_TYPE_Other,
	CI_STREAM_TYPE_Max
} CI_STREAM_TYPE; // TODO : EsType_t �� �ִµ� ��?

typedef struct {
	EsType_t			eEsType;
	HUINT16				usEsPid;
//	CI_STREAM_STATE		ePlayState;
	SvcCas_PmtElement_t	*pEs;						// CAS mgr ���� ����
} CI_ES_INFO;

typedef enum { /* ���� : CI mgr �� ���ο� �̺�Ʈ�� �߰��� ���� �켱 ������, �� ���� �÷ο� ������ ���� ���� ���� �߰��ϵ��� �� ��! */

/*--- events for service ---*/

	CI_START = 1,					// event from CAS mgr
	CI_STOP,						// event from CAS mgr

	CI_PAUSE,
	CI_RESUME,

	CI_SETPID,						// event from CAS mgr
	CI_MANAGE_CA_INST,				// event from CAS mgr

//	CI_UPDATE_SVC,					// event from CAS mgr - ����μ� ������ ����

	CI_RETRY,						// event from CI mgr itself by timer callback

	CI_CHECK_AV,

/*--- events for SI ---*/

	CI_PMT_CHANGED,					// event from CAS mgr
	CI_SDT_CHANGED,					// event from CAS mgr - ����μ� CONFIG_MW_CI_PLUS ���� �ʿ��� �����̳� ���� ���⼭�� #if ó���� ���� �ʴ´�
	CI_PMT_CHANGED_LOCATION,

/*--- events for instance management ---*/


	CI_UPDATE_CA_INST,				// event from CI stack (CAM)
	CI_UPDATE_CC,					// event from ci plus stack (cc resource)
	CI_ADD_DELAYED_CA_INST,			// event from CI mgr itself by timer callback

#if defined(CI_PLUS_PVR_ENABLE)
	CI_UPDATE_CA_PVR_INST,			// event from CI+ stack (CI+ CAM)
#endif

/*--- events for CAM state ---*/

	CI_UPDATE_CAM_STATE,			// event from CI stack (CAM)

/*--- events for CAS state ---*/

#if 1 // #if defined(CONFIG_MW_CI_PLUS_PVR) : ������ CA PVR ������ ���� �߰��� �������� Multiple CA application selection procedure ���� ���� �ٸ� �뵵�� Ȯ���ؼ� �� ������ �����Ƿ� �Ϲ������� ������ ��.
	CI_UPDATE_CAS_STATE,			// event from CI stack (CAM)
#endif

/*--- events for CI+ ---*/

#if defined(CONFIG_MW_CI_PLUS)

	CI_PLUS_KEY_CHANGED,			// event from CI+ stack (CI+ CAM)
	CI_PLUS_URI_CONTROL,			// event from CI+ stack (CI+ CAM)
	CI_PLUS_SRM_CONTROL,			// event from CI+ stack (CI+ CAM)
	CI_PLUS_CAM_TUNE,				// event from CI+ stack (CI+ CAM)
	CI_PLUS_CAM_PIN_CAPA_INFO,		// event from CI+ stack (CI+ CAM)

#if defined(CONFIG_OP_CANALREADY)
	CI_PLUS_CANALREADY_INIT_NOTIFY,	// event from CI+ stack (CanalReady CI+ CAM)
#endif	// CONFIG_OP_CANALREADY

	CI_PLUS_OPERATOR_PREPARE_REQ,
	CI_PLUS_OPERATOR_TUNE,
	CI_PLUS_OPERATOR_NIT,

#endif	// CONFIG_MW_CI_PLUS

/*--- events for stream state ---*/

	CI_STREAM_STATE_CHANGED,		// event from A/V decoder

	EVT_CIREV_MAX

} CI_RCV_EVENT;

typedef struct {
	Handle_t 			hAction;
	HUINT32			unSlotId;
} CI_COMMAND_PARAM;

typedef struct {
	Handle_t			hAction;
	HUINT32			unSlotId;
	Handle_t			hSvc;
	SvcCas_ActionType_e	eActType;
} CI_ADD_SVC_PARAM;

typedef struct {
	Handle_t			hAction;
	HUINT32			unSlotId;
	Handle_t			hNextSvc;
} CI_DELETE_SVC_PARAM;

typedef struct {
	Handle_t			hAction;
	HUINT32			unSlotId;
	CI_ES_INFO		*pEsInfo;	// ���õ� ES ���� ����
	HUINT8			ucNumEs;
	SvcCas_PidInfo_t 		*pPidInfo;
} CI_SETDSC_PARAM;

typedef struct {
	Handle_t			hAction;
	HUINT32			unSlotId;
} CI_STOP_SVC_PARAM;

typedef struct {
	Handle_t				hAction;
	HUINT32				unSlotId;
	CI_STREAM_TYPE		eStreamType;
	HUINT16				usPid;
} CI_UPDATE_SVC_PARAM;

typedef struct {
	Handle_t			hAction;
	HUINT32			unSlotId;
	Handle_t			hSvc;
} CI_RETRY_SVC_PARAM;

typedef struct {
	Handle_t			hAction;
	HUINT32			unSlotId;
	Handle_t			hSvc;
} CI_CHECK_AV_PARAM;

typedef struct {
	Handle_t			hAction;
	HUINT32			unSlotId;
	unsigned char		*pucTbl;
	unsigned short		usTblLen;
} CI_PMT_CHANGED_PARAM;

typedef struct {
	Handle_t		hAction;
	HUINT32			unSlotId;
	SvcCas_PmtInfoForTune_t	*pstPmtInfo;
} CI_PMT_CHANGED_LOCATION_PARAM;

typedef struct {
	Handle_t			hAction;
	HUINT32			unSlotId;
	HUINT16			usUniqueId;
	unsigned char		*pucTbl;
} CI_SDT_CHANGED_PARAM;	// ����μ� CONFIG_MW_CI_PLUS ���� �ʿ��� �����̳� ���� ���⼭�� #if ó���� ���� �ʴ´�

typedef struct {
	Handle_t			hAction;
	HUINT32			unSlotId;
	Handle_t			hCasDevice;	// CAS mgr �� Instance Table Id.
	HBOOL			bConnect;
} CI_MANAGE_CA_INST_PARAM;

typedef struct {
	CI_SLOT_ID			usSlotId;
	unsigned short		usCaInfoCnt;	// the number of ca info : ausCaSysId
	unsigned short		*ausCaSysId;
} CI_UPDATE_CA_INST_PARAM;

#if defined(CI_PLUS_PVR_ENABLE)
typedef struct {
	CI_SLOT_ID			usSlotId;
	unsigned short		usCaPvrInfoCnt;	// the number of ca pvr info : ausCaPvrCaSysId and aucCaRecMode
	unsigned short		*ausCaPvrCaSysId;
	CI_CA_PVR_REC_MODE	*aucCaRecMode;
} CI_UPDATE_CA_PVR_INST_PARAM;
#endif

typedef struct {
	CI_SLOT_ID			usSlotId;
	CI_CAM_STATE		eCamState;
} CI_UPDATE_CAM_STATE_PARAM;

#if 1 // #if defined(CONFIG_MW_CI_PLUS_PVR) : ������ CA PVR ������ ���� �߰��� �������� Multiple CA application selection procedure ���� ���� �ٸ� �뵵�� Ȯ���ؼ� �� ������ �����Ƿ� �Ϲ������� ������ ��.
typedef struct {
	CI_SLOT_ID			usSlotId;
	CI_CAS_STATE		eCasState;
} CI_UPDATE_CAS_STATE_PARAM;
#endif

#if defined(CONFIG_MW_CI_PLUS)

typedef struct {
	CI_SLOT_ID			usSlotId;
	SvcCas_DevDscType_e 		eDscType;
	SvcCas_DevDscKeyParity_e	eParity;
	unsigned char 		ucCck[16];	// AES-128-CBC ����. DES-56-ECB �� ��쿣 [0-7] �� ���.
	unsigned char 		ucCiv[16];	// AES-128-CBC ����. DES-56-ECB �� ��쿣 ������ ����.
} CI_PLUS_KEY_CHANGE_PARAM;

typedef struct {
	CI_SLOT_ID			usSlotId;
	CIPLUS_UriInfo_t	stUriInfo;
} CI_PLUS_URI_CONTROL_PARAM;

typedef struct {
	CI_SLOT_ID			usSlotId;
	unsigned char		*pucSrmData;
	unsigned short		usSrmDataLen;
} CI_PLUS_SRM_CONTROL_PARAM;

typedef struct {
	CI_SLOT_ID			usSlotId;
	SvcCas_CiPlusTuneInfo_t	stTuneInfo;
} CI_PLUS_CAM_TUNE_PARAM;

#if defined(CONFIG_OP_CANALREADY)
typedef struct {
	CI_SLOT_ID			usSlotId;
	SvcCas_CiAuthInfo_t			stCiAuthInfo;
} CI_PLUS_CANALREADY_INIT_INFO_PARAM;
#endif	// CONFIG_OP_CANALREADY

#if defined(CONFIG_MW_CI_PLUS_V_1_3)
typedef struct
{
	CI_SLOT_ID				usSlotId;
	CI_OP_REFRESH_REQUEST	eOpRefreshReq;
	HUINT8					*pszProfileName;
} CI_PLUS_OPERATOR_PREPARE_PARAM;

typedef struct
{
	CI_SLOT_ID	usSlotId;
	HUINT16		usDesLoopLen;
	HUINT8		*pucDesLoop;
} CI_PLUS_OPERATOR_TUNE_PARAM;

typedef struct
{
	CI_SLOT_ID	usSlotId;
	HUINT8		*pszProfileName;
	HUINT8		aucTextCoding[7];
	HUINT8		*pucCiCamNit;
	HUINT16		usCiCamNitLen;
	HBOOL		bSimpleNitChange;
} CI_PLUS_OPERATOR_NIT_PARAM;
#endif

#endif	// CONFIG_MW_CI_PLUS

typedef struct {
	Handle_t			hAction;
//	unsigned char		ulTsNode;
	unsigned short		usPid;
//	unsigned char		ucStreamType;
	CI_STREAM_STATE		eStreamState;
	HUINT32				ulDevId;
} CI_STREAM_STATE_CHANGED_PARAM;

typedef struct {
	unsigned char		*pucRaw;
	unsigned short		usRawLen;
} CI_CA_PMT_DATA;

typedef struct {
	SvcCas_PmtInfo_t		*pPmtInfo;						// CAS mgr si�� structure pointer
	SvcCas_ChannelType_e	eChType;
	CI_ES_INFO			aEsInfo[CAS_MAX_NUM_OF_ES];
	unsigned char		ucNumEs;						// StartSvc �Ǵ� PmtChanged (�Ǵ� UpdateSvc) �ÿ� ���õ�.
	HUINT32				ulPrePmtCrc;// pmt update�� �߻� ��. crc�� version number�� �����ϸ�, update ó���� ���� �ʵ��� �ϱ� ���ؼ�. ci context�� program info�� crc�� version number�� �߰���.
	HUINT8				ucPrePmtVer;// pmt update�� �߻� ��. crc�� version number�� �����ϸ�, update ó���� ���� �ʵ��� �ϱ� ���ؼ�. ci context�� program info�� crc�� version number�� �߰���.
} CI_PROGRAM_INFO;

typedef struct
{
	HUINT16 usPid;
	HUINT32 ulDscId;
} CI_DSC_INFO;

/* CAS Device�� Context. */
typedef struct {

	CI_SLOT_ID				usSlotId;						// s_astCiCam["usSlotId"]."usSlotId" �� �� usSlotId ���� �ٸ� �� ������ ������ ��. �ʱⰪ�� CARM_INVALID_CI_SLOT_ID
	CI_CAM_STATE			eCamState;
	unsigned short			usManufacturer;					// Ư�� ķ�� ���� ó���� �ʿ��ϴٸ� usManufacturer �� ausCaSysId ������ �Բ� ����ϵ��� �Ѵ�
	unsigned short			usCaInfoCnt;					// the number of ca info : ausCaSysId
	unsigned short			*ausCaSysId;

	Handle_t					hCasDevice;						// CAS mgr �� Device Context�� Handle

	/*-- Ca Pmt�� ���� �ʿ��� ���� �� ���. ���� pid�� ������ ca pmt raw bytes ---*/
	HUINT32 					ulNumOfPidForCaPmt;					// ca pmt�� ���� pid ����.
	SvcCas_PidInfo_t				PidInfoForCaPmt[CAS_MAX_NUM_OF_ES];	// max ���� ���ؼ��� ���� �ʿ����� Ȯ��.

	CI_CA_PMT_DATA			caPmtOkDsc;

	HUINT8					ucCurPmtVer;
	HUINT32					ulCurPmtCrc;

	HUINT32 					ulNumOfAct;
	Handle_t					hActList[CAS_MAX_NUM_OF_ACTION];

#if defined(CONFIG_MW_CI_PLUS)
	/*--- ��ũ���� ���ҽ� ��뿡 �ʿ��� ���� ---*/

	HUINT32 				ulNumOfDsc[CI_MAX_NUM_OF_DMX];	// ���µ� ��ũ���� ����. CI_CamInst_t �ʱ�ȭ��, ��ũ���� ���� �� ķ ���Ž� ���µǰ�, ��ũ���� ���ý� ���õ�.

	CI_DSC_INFO			stDscInfo[CI_MAX_NUM_OF_DMX][CAS_MAX_NUM_OF_DSC];		// Dmx Id , Dsc Id

	/*--- ��ũ���� Ű ���� ---*/
	HBOOL				bKeyInfoAvailable;				// CI_CamInst_t �ʱ�ȭ��, ķ ���Ž� ���µǰ�, ķ���κ��� Ű ������ �� ���̶� ������ ���õ�. ���� ķ�� ���� �ִ� ���¿����� ä�� ��ȯ�ϴ��� �������� �ʵ��� �Ѵ�.
	SvcCas_DevDscType_e 		eDscType;						// ���� Ű ���� ������ bKeyInfoAvailable �� ���õǾ� ���� ���� ��ȿ�Կ� ������ ��.
	SvcCas_DevDscKeyParity_e	eParity;
	unsigned char 		ucCck[16];						// AES-128-CBC ����. DES-56-ECB �� ��쿣 [0-7] �� ���.
	unsigned char 		ucCiv[16];						// AES-128-CBC ����. DES-56-ECB �� ��쿣 ������ ����.

	Handle_t			hLastSvc;
  #if 0
	SvcCas_CiPlusCcReportMode_e eCcReport;
  #else
	HBOOL				bAuthFinish;
  #endif
	HBOOL				bClearCaPmt;
  #if defined(CONFIG_MW_CI_PLUS_V_1_3)
  	HBOOL				bOpSearchReq;
  	CI_PLUS_OPERATOR_PREPARE_PARAM stOpParam;
  #endif
#endif

#if defined(CI_PLUS_PVR_ENABLE)
	unsigned short			usCaPvrInfoCnt;					// the number of ca pvr info : ausCaPvrCaSysId and aucCaRecMode
	unsigned short			*ausCaPvrCaSysId;
	CI_CA_PVR_REC_MODE	*aucCaRecMode;
#endif

} CI_CamInst_t;

typedef enum
{
	eCiMgrState_Stopped,
	eCiMgrState_Started,
	eCiMgrState_StartCaPmt,

	eCiMgrState_Max
}CiMgrState_t;

/* CAM�� ���ڸ�ŭ �����ǰ�, CAS mgr ���� Device Context�� ���� �ȴ�. */
typedef struct {

	Handle_t				hAction;
	Handle_t				hSvc;
	SvcCas_ActionType_e		eActType;
	CiMgrState_t			eState;
	HBOOL				bPathConnected;
	HBOOL				bSendCaPmt;
	SvcCas_ChannelType_e	eChType;

	CI_PROGRAM_INFO	stProgInfo;
	CI_CamInst_t 		*pCam;

	HUINT32				ulTunerId; /* HUINT32 */
	HUINT32				ulDemuxId; /* HUINT32 */


#if defined(CONFIG_MW_CI_PLUS)
	HUINT16 				usUniqueId;
	HUINT16 				usSvcId;
	HUINT16 				usTsId;
	HUINT16 				usOnId;

	SvcCas_SiTableState_e	eSdtTblState;

	HBOOL				bSvcShunned;
	HBOOL				bRcvUri;
  #if defined (CONFIG_MW_CI_PLUS_V_1_3)
	HBOOL				bRecordProcessDone;
  #endif
#endif

	HULONG				ulRetryTimerId;
	HULONG				ulCheckAvTimerId;

#if defined(_LIMIT_CI_RETRY_COUNT_)
	unsigned char			ucRetryCnt;
#endif

	unsigned char		ucCheckAvCount;
} CI_Context_t;

/*
	this descriptor in ci plus spec.
*/
typedef struct
{
	HUINT16					usSvcId;
	HBOOL					bCiDscFlag;							// Descriptor �� �����ϳ�.
	HBOOL					bFreeCiFlag;							// Free ci mode flag
	HBOOL					bMatchBrandFlag;						// Match brand flag
	HUINT8					ucNumOfBrandId;						// number of entries
	HUINT16					usCiBrandId[CI_NUM_OF_BRAND_ID];		// cicam_brand_identifier
	HUINT8					*pDesRaw;
}CAS_CiDscInfo_t;											// ci plus ��.

/*******************************************************************/
/********************      Public Functions     ********************/
/*******************************************************************/

HERROR CiMgr_GetMmiData(HUINT16 usSlotId, HUINT32 ulVersion, SvcCas_MmiData_t **stMmiData);
HERROR CiMgr_GetCcReport(HUINT16 usSlotId, HUINT32 ulEvtVersion, SvcCas_CcReport_t **pData);
HERROR CiMgr_GetSlotIdByActionId(HUINT32 ulActionId, HUINT16 *pusSlotId);

#if defined(CONFIG_MW_CI_PLUS)
HERROR CiMgr_ReleaseDrmInfo(HUINT8 ucVersion, HERROR hConsumedStatus);
HERROR CiMgr_GetUpgradeStatus(HUINT16 usSlotId, SvcCas_CiPlusUpgrade_t **ppstUpdateStatus);
HERROR CiMgr_GetOpNetNum(HUINT16 usSlotId, void *pvOut);
HERROR CiMgr_GetOpNetInfo(HUINT16 usSlotId, HUINT16 usNetIdx, void *pvOut);
HERROR CiMgr_GetOpTsNum(HUINT16 usSlotId, HUINT16 usNetIdx, void *pvOut);
HERROR CiMgr_GetOpTsInfo(HUINT16 usSlotId, HUINT16 usNetIdx, HUINT16 usTsIdx, void *pvOut);
HERROR CiMgr_GetOpSvcNum(HUINT16 usSlotId, HUINT16 usNetIdx, HUINT16 usTsIdx, void *pvOut);
HERROR CiMgr_GetOpSvcInfo(HUINT16 usSlotId, HUINT16 usNetIdx, HUINT16 usTsIdx, void *pvOut);
HERROR CiMgr_GetOpName(HUINT16 usSlotId, HUINT8 *pucOpName);
#if defined(CONFIG_MW_CI_PLUS_V_1_3)
HERROR CiMgr_NotifyHciPmtUse(Handle_t hAction, HUINT32 unSlotId);
HERROR CiMgr_GetNextOperatorTune(CI_SLOT_ID usSlotId, HUINT8 ucNextDesNum);
#endif // #if defined(CONFIG_MW_CI_PLUS_V_1_3)
#endif // #if defined(CONFIG_MW_CI_PLUS)

#ifdef __cplusplus
}
#endif

#endif /* __CIMGR_INT_H__ */


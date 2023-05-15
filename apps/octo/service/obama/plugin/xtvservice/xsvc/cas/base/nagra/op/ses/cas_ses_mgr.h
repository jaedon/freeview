#ifndef	__CAS_MGR_APS_H__
#define	__CAS_MGR_APS_H__

#include <_svc_cas_mgr_common.h>

/*******************************************************************/
/********************      Header Files    *************************/
/*******************************************************************/


/*******************************************************************/
/********************      Global Variables     ********************/
/*******************************************************************/


/*******************************************************************/
/********************      Macro Definition     ********************/
/*******************************************************************/
#if defined(CONFIG_DEBUG)
#define		COMMAND_NAGRA_APS_TEST			1
#endif
/*******************************************************************/
/********************      Typedef         *************************/
/*******************************************************************/
typedef enum
{
	eCAS_APS_CHECK_HDVALID = 0,
	eCAS_APS_CHECK_PPV
} eCAS_APS_CHECK_TYPE;


typedef enum
{
	eCAS_APS_MSG_REF130A = 0,
	eCAS_APS_MSG_REF130B,
	eCAS_APS_MSG_REF131,
	eCAS_APS_MSG_REF132A,
	eCAS_APS_MSG_REF132B,
	eCAS_APS_MSG_REF133,
	eCAS_APS_MSG_REF134,
	eCAS_APS_MSG_REF135,
	eCAS_APS_MSG_REF136,
	eCAS_APS_MSG_REF140
} eCAS_APS_VALID_MESSAGE;

typedef enum
{
	eCAS_APS_MSG_T_BLOCK = 0,
	eCAS_APS_MSG_T_DIALOG
} eCAS_APS_VALID_TYPE;

typedef enum
{
	eCAS_APS_UPDATE_OK = 0,
	eCAS_APS_UPDATE_PREV
} eCAS_APS_UPDATE_TYPE;

typedef struct
{
	HUINT8			nCurrentIdx;
	HUINT8			arrReserved[3];
	HxDATETIME_Date_t		stData;
} NA_IRDCMD_IDX_PROFILE_t;

typedef struct
{
	Handle_t 					hAction;
	eCAS_APS_VALID_TYPE 		eCasApsValidType;
	eCAS_APS_VALID_MESSAGE		eCasApsValidMessage;
	HUINT8						bDisplayWarning;
	HUINT8						bRemainDay;
	HxDATETIME_t 				bExpiredDate;
} NaCasApsValidCheck_t;

typedef struct
{
	eCAS_APS_CHECK_TYPE			eCasCheckType;
	union
	{
		NaCasApsValidCheck_t	stCasApsValidCheck;
		// if you need other type of message, add the structure here.
	} context;
} NaCasApsCheck_t;

typedef struct
{
	HUINT8 ucTestType;	// 0:alarm message, 1:bdc

	//related bdc
	HUINT8 ucBdcCmd;
	HUINT8 ucBdcType;
	HUINT8 strBdcMessage[128];

	//related alarm
	HUINT8 ucAlarmType;
	HUINT32 ulAlarmMsg;

} NaCasAPSCmdTest_t;

/*******************************************************************/
/********************      Public Functions     ********************/
/*******************************************************************/
HERROR	CASMGR_APS_SaveSCSerialNum_ExpireDate(UNIXTIME		currentUnixTime, HxDATETIME_t stCurrentDateTime);
HERROR	CASMGR_APS_SetServiceStatus(HUINT8 bflag);
HERROR	CASMGR_APS_UpdateCasFromAP(eCAS_APS_UPDATE_TYPE type, HUINT32 actionId);
HERROR	CASMGR_APS_UpdateCasMessage(Handle_t hAction, SvcCas_MsgType_e eMsgType, DxCAS_GroupId_e eCasId, void *pMessage, SvcCas_ShowType_e eShowType);
HERROR	CASMGR_APS_CMD_Test(Handle_t hAction, NaCasAPSCmdTest_t *pstCmdTest);
HERROR	CASMGR_APS_NaGetIndexDisplayProfile(NA_IRDCMD_IDX_PROFILE_t *pstCurrentProfile, HUINT8 ucProfieType);
HERROR	CASMGR_APS_NaPrintDisplayProfile(HUINT8 ucProfieType);

#endif /* !__CAS_MGR_APS_H__ */

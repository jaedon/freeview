/**
@file     ap_snmp_mib_japan.c
@brief Installation wizard install option(simple/advanced) configuration file.

Description: snmp mib data set to DI \n
Module: Application(Layer), util(module) \n
Remarks : \n

Copyright (c) 2009 HUMAX Co., Ltd.				\n
All rights reserved.							\n
*/

/**************************************************************************************************/
#define _________XMGR_Snmp_Mib_Private_Include_________________________________________________
/**************************************************************************************************/

#include <bus.h>

#include <svc_sys.h>
#include <bcas.h>
#include <svc_pipe.h>
#include <svc_cas.h>
#include <mgr_action.h>
#include <mgr_live.h>
#include "./local_include/_xmgr_snmp_mib.h"

/**************************************************************************************************/
#define _________XMGR_Snmp_Mib_Private_Macro_________________________________________________
/**************************************************************************************************/

/* debug print */
//#define 		DEBUG_PRINT		1

/* jc5100 따라서 하드코딩 값으로 한다. */
#define	 	SYS_MIDDLEWARE_VERSION_STR 		 	"0.10"
#define	 	SYS_BROSWER_VERSION_STR				"0.10"

/* temp String size */
#define 		TEMP_STRING_SIZE 				10
#define 		STBID_STRING_SIZE 			6
#define 		CARD_STRING_SIZE 			21
#define 		CARD_MANUFACTURE_SIZE		5
/**************************************************************************************************/
#define _________XMGR_Snmp_Mib_Private_Struct_________________________________________________
/**************************************************************************************************/

typedef struct SnmpMib_Context
{
	SvcSys_SnmpInformation_t stSnmpMwInfo;
	SvcSys_SnmpInformation_t stOldSnmpMwInfo;
} SnmpMib_Contents_t;

/**************************************************************************************************/
#define _________XMGR_Snmp_Mib_Private_Value_________________________________________________
/**************************************************************************************************/

static	SnmpMib_Contents_t		s_SnmpMib_Context;

/**************************************************************************************************/
#define _________XMGR_Snmp_Mib_Private_Prototype_________________________________________________
/**************************************************************************************************/
//init
static SnmpMib_Contents_t* local_Snmp_Mib_GetContext   ( void );
static BUS_Result_t local_Snmp_Mib_InitContent(SnmpMib_Contents_t *pstContents , HINT32 p1);

//update

//process
static HERROR local_Snmp_Mib_SetCardInfo(SnmpMib_Contents_t *pstContents );
static HERROR local_Snmp_Mib_SetActionSatuts( SnmpMib_Contents_t *pstContents );
static HERROR local_Snmp_Mib_ChangePowerStatus( SnmpMib_Contents_t *pstContents );
static void local_Snmp_Mib_PrintInfo( SnmpMib_Contents_t *pstContents);
static HERROR local_Snmp_Mib_CopyOldSnmp( SnmpMib_Contents_t *pstContents );
static HERROR local_Snmp_Mib_ChangeStatus( SnmpMib_Contents_t *pstContents );
//destroy

//ui

//msg run
static BUS_Result_t local_Snmp_Mib_MsgBusCreate  ( SnmpMib_Contents_t *pstContents, HINT32 p1, HINT32 p2, HINT32 p3);
static BUS_Result_t local_Snmp_Mib_MsgBusDestroy  (SnmpMib_Contents_t *pstContents, HINT32 p1, HINT32 p2, HINT32 p3 );
static BUS_Result_t local_Snmp_Mib_MsgBusGoStandBy(SnmpMib_Contents_t *pstContents, HINT32 p1, HINT32 p2, HINT32 p3);
static BUS_Result_t local_Snmp_Mib_MsgBusQuitStandBy(SnmpMib_Contents_t *pstContents, HINT32 p1, HINT32 p2, HINT32 p3);
static BUS_Result_t local_Snmp_Mib_EvtCasScRemoved(SnmpMib_Contents_t *pstContents, HINT32 p1, HINT32 p2, HINT32 p3);
static BUS_Result_t local_Snmp_Mib_EvtCasScInserted(SnmpMib_Contents_t *pstContents, HINT32 p1, HINT32 p2, HINT32 p3);

/**************************************************************************************************/
#define _________XMGR_Snmp_Mib_Private_Init__________________________________________________________
/**************************************************************************************************/

static SnmpMib_Contents_t* local_Snmp_Mib_GetContext(void)
{
	return &s_SnmpMib_Context;
}

static BUS_Result_t local_Snmp_Mib_InitContent(SnmpMib_Contents_t *pstContents , HINT32 p1)
{
	HERROR		hErr = ERR_FAIL;
	HUINT8		tempStr[TEMP_STRING_SIZE];
	HUINT32		ulSWVersion = 0;

	memset ( &pstContents->stSnmpMwInfo, 0x00, sizeof(SvcSys_SnmpInformation_t));
	memset ( &pstContents->stOldSnmpMwInfo, 0x00, sizeof(SvcSys_SnmpInformation_t));

	/* system id */
	memcpy( pstContents->stSnmpMwInfo.szSystemId , SYSTEM_ID, TEMP_STRING_SIZE );

	/* app version */
	memset( tempStr, 0x00, TEMP_STRING_SIZE );
	ulSWVersion = SYSTEM_APP_VERSION;
	snprintf( tempStr, TEMP_STRING_SIZE, "%d.%03d",(( ulSWVersion >> 16 ) & 0xFF), (( ulSWVersion >> 8 ) & 0xFF ));
	HLIB_STD_StrNCpySafe( pstContents->stSnmpMwInfo.szApplicationVer , tempStr, TEMP_STRING_SIZE );

	/* mw version */
	HLIB_STD_StrNCpySafe( pstContents->stSnmpMwInfo.szMiddleWareVer, SYS_MIDDLEWARE_VERSION_STR, TEMP_STRING_SIZE );

	/* browser version */
	HLIB_STD_StrNCpySafe( pstContents->stSnmpMwInfo.szSoftBrowserVer, SYS_BROSWER_VERSION_STR, TEMP_STRING_SIZE );

	/* stb id */
	hErr = SVC_SYS_GetStbID( pstContents->stSnmpMwInfo.szStbID , STBID_STRING_SIZE );

	/* b-cas card id */
	/* c-cas card id */
	hErr |= local_Snmp_Mib_SetCardInfo(  pstContents );

	/* watching status */
	hErr |= local_Snmp_Mib_SetActionSatuts( pstContents );

	/* power status */
	if( p1 == TRUE )
	{
		pstContents->stSnmpMwInfo.etPowerStatus = MW_STB_POWER_STATE_ON;
	}
	else
	{
		pstContents->stSnmpMwInfo.etPowerStatus = MW_STB_POWER_STATE_OFF;
	}

	/* 시작하자마다 recording 하지 않으니까 default 는 -1로 해준다. */
	/* rec tuner 1 */
	pstContents->stSnmpMwInfo.ulTuner1ChannelId= -1;
	pstContents->stSnmpMwInfo.ulTuner1NetworkId= -1;
	pstContents->stSnmpMwInfo.ulTuner1Frequency = -1;

	/* rec tuner 2 */
	pstContents->stSnmpMwInfo.ulTuner2ChannelId= -1;
	pstContents->stSnmpMwInfo.ulTuner2NetworkId= -1;
	pstContents->stSnmpMwInfo.ulTuner2Frequency = -1;

	return hErr;
}

/**************************************************************************************************/
#define _________XMGR_Snmp_Mib_Private_Update__________________________________________________________
/**************************************************************************************************/

/**************************************************************************************************/
#define _________XMGR_Snmp_Mib_Private_Process__________________________________________________________
/**************************************************************************************************/
static HERROR local_Snmp_Mib_SetCardInfo( SnmpMib_Contents_t *pstContents )
{
	HERROR		hErr = ERR_FAIL;
	HUINT8		szCardString[CARD_STRING_SIZE], szManufactureStr[CARD_MANUFACTURE_SIZE];

	/* b-cas card id */
	memset( szCardString, 0x00, CARD_STRING_SIZE );
	memset( szManufactureStr, 0x00, CARD_MANUFACTURE_SIZE );
	memset( pstContents->stSnmpMwInfo.szBcasCardId, 0x00, 32  );
	hErr = MW_CAS_BCAS_GetCardIdString(BCAS_CASYSTEMID_BCAS, szManufactureStr, szCardString);
	memcpy( pstContents->stSnmpMwInfo.szBcasCardId , szCardString, CARD_STRING_SIZE );

	/* c-cas card id */
	memset( szCardString, 0x00, CARD_STRING_SIZE );
	memset( szManufactureStr, 0x00, CARD_MANUFACTURE_SIZE );
	memset( pstContents->stSnmpMwInfo.szCcasCardId, 0x00, 32  );
	hErr |= MW_CAS_BCAS_GetCardIdString(BCAS_CASYSTEMID_CCAS, szManufactureStr, szCardString);
	memcpy( pstContents->stSnmpMwInfo.szCcasCardId , szCardString, CARD_STRING_SIZE );

	return hErr;
}

static HERROR local_Snmp_Mib_SetActionSatuts( SnmpMib_Contents_t *pstContents )
{
	HERROR			hErr =ERR_FAIL;
	HUINT32			ulActionId =0;
	MgrAction_Type_e	eActionType =eActionType_NONE;
	Handle_t			hSvcHandle = HANDLE_NULL;
	DbSvc_Info_t		stSvcInfo;

	/* watchtv 관련 default 값 setting */
	pstContents->stSnmpMwInfo.ulDownFrequency = -1;
	pstContents->stSnmpMwInfo.ulWatchingChannelId = -1;
	pstContents->stSnmpMwInfo.ulWatchingNetworkId = -1;
	pstContents->stSnmpMwInfo.ulWatchingTunerGroupId = -1;
	pstContents->stSnmpMwInfo.etWatchingChannelType = -1;

	/* 실제 값 setting , 실패시 default 값 유지 */
	ulActionId = MGR_ACTION_GetMainActionId();
	eActionType = MGR_ACTION_GetType(ulActionId);
	switch(eActionType)
	{
	case eViewType_WTV:
		pstContents->stSnmpMwInfo.etPlayStatus = MW_STB_PLAY_STATE_WTV;

		hErr = MGR_ACTION_GetMasterSvcHandle(ulActionId, &hSvcHandle);
		if(hErr == ERR_OK)
		{
			memset( &stSvcInfo , 0x00 , sizeof (DbSvc_Info_t ));
			hErr = DB_SVC_GetServiceInfo(hSvcHandle, &stSvcInfo);
			if(hErr == ERR_OK)
			{
				pstContents->stSnmpMwInfo.ulDownFrequency = stSvcInfo.ulFrequency;
				pstContents->stSnmpMwInfo.ulWatchingChannelId = (HUINT32)DbSvc_GetSvcId(&stSvcInfo);
				pstContents->stSnmpMwInfo.ulWatchingNetworkId = (HUINT32)DbSvc_GetOnId(&stSvcInfo);
				pstContents->stSnmpMwInfo.etWatchingChannelType = DbSvc_GetDeliType(stSvcInfo);
			}
			else
			{
				HxLOG_Error("[%s,%d]Err : %d \n", __FUNCTION__ , __LINE__, hErr);
			}

			hErr = SVC_PIPE_GetResourceId(ulActionId, eRxRSCTYPE_TUNER_GROUP, &pstContents->stSnmpMwInfo.ulWatchingTunerGroupId);
			if( hErr != ERR_OK )
			{
				HxLOG_Error("[%s,%d]Err : %d \n", __FUNCTION__ , __LINE__, hErr);
			}
		}
		else
		{
			HxLOG_Error("[%s,%d] Err : %d \n", __FUNCTION__ , __LINE__, hErr);
		}
		break;


#if defined(CONFIG_3RD_RFVOD)
	case eViewType_RFVOD:
		pstContents->stSnmpMwInfo.etPlayStatus = MW_STB_PLAY_STATE_VOD;
		break;
#endif

#if defined(CONFIG_MW_MM_PVR)
	case eViewType_PB:
	case eViewType_TSR_PB:
	case eViewType_CHASE_PB:
		pstContents->stSnmpMwInfo.etPlayStatus = MW_STB_PLAY_STATE_HDD_PLAY;
		break;
#endif

	default :
		pstContents->stSnmpMwInfo.etPlayStatus = MW_STB_PLAY_STATE_ETC;
		HxLOG_Error("[%s,%d] Invalid Action Type : %d \n", __FUNCTION__ , __LINE__, eActionType);
		break;
	}

	return hErr;
}

static HERROR local_Snmp_Mib_ChangePowerStatus( SnmpMib_Contents_t *pstContents )
{
	/* 전원 on / off 한쪽이 연속으로 올 수 가 없으므로 메시지 받을 때 마다 변경 */
	if( pstContents->stSnmpMwInfo.etPowerStatus == MW_STB_POWER_STATE_ON )
	{
		pstContents->stSnmpMwInfo.etPowerStatus = MW_STB_POWER_STATE_OFF;
	}
	else
	{
		pstContents->stSnmpMwInfo.etPowerStatus = MW_STB_POWER_STATE_ON;
	}

	return ERR_OK;
}

static HERROR local_Snmp_Mib_ChangeTunerInfo( SnmpMib_Contents_t *pstContents, Handle_t hAction, HBOOL bIsStart )
{
	HERROR					hErr = ERR_FAIL;
	HUINT32					ulActionId =0;
	MgrRec_RecInfo_t		stRecInfo;
	DbSvc_Info_t				stSvcInfo;

	ulActionId =  SVC_PIPE_GetActionId( hAction );

	if( bIsStart == TRUE )
	{
		/* 값 세팅 */
		memset ( &stRecInfo, 0x00, sizeof( MgrRec_RecInfo_t ));
		hErr = MGR_RECORDER_GetRecordingSvcInfo( ulActionId , &stRecInfo );
		if( hErr == ERR_OK )
		{
			memset( &stSvcInfo , 0x00, sizeof( DbSvc_Info_t ));
			hErr = DB_SVC_GetServiceInfo(stRecInfo.hSvc, &stSvcInfo);
			if(hErr == ERR_OK)
			{
				switch( ulActionId )
				{
					case eActionId_REC_FIRST + 0:
						pstContents->stSnmpMwInfo.ulTuner1ChannelId= (HUINT32)DbSvc_GetSvcId(&stSvcInfo);
						pstContents->stSnmpMwInfo.ulTuner1NetworkId= (HUINT32)DbSvc_GetOnId(&stSvcInfo);
						pstContents->stSnmpMwInfo.ulTuner1Frequency = stSvcInfo.ulFrequency;
						break;

					case eActionId_REC_FIRST + 1:
						pstContents->stSnmpMwInfo.ulTuner2ChannelId= (HUINT32)DbSvc_GetSvcId(&stSvcInfo);
						pstContents->stSnmpMwInfo.ulTuner2NetworkId= (HUINT32)DbSvc_GetOnId(&stSvcInfo);
						pstContents->stSnmpMwInfo.ulTuner2Frequency = stSvcInfo.ulFrequency;
						break;

					default:
						HxLOG_Error("[%s,%d] Invalid Action ID : %d \n", __FUNCTION__ , __LINE__, ulActionId);
						break;
				}
			}
			else
			{
				HxLOG_Error("[%s,%d] DB_SVC_GetServiceInfo Err : %d \n", __FUNCTION__ , __LINE__, hErr);
			}
		}
		else
		{
			HxLOG_Error("[%s,%d] MGR_RECORDER_GetRecordingSvcInfo Err : %d \n", __FUNCTION__ , __LINE__, hErr);
		}
	}
	else
	{
		/* 초기 값으로 .. */
		switch( ulActionId )
		{
			case eActionId_REC_FIRST + 0:
				pstContents->stSnmpMwInfo.ulTuner1ChannelId= -1;
				pstContents->stSnmpMwInfo.ulTuner1NetworkId= -1;
				pstContents->stSnmpMwInfo.ulTuner1Frequency = -1;
				break;

			case eActionId_REC_FIRST + 1:
				pstContents->stSnmpMwInfo.ulTuner2ChannelId= -1;
				pstContents->stSnmpMwInfo.ulTuner2NetworkId= -1;
				pstContents->stSnmpMwInfo.ulTuner2Frequency = -1;
				break;

			default:
				HxLOG_Error("[%s,%d] Invalid Action ID : %d \n", __FUNCTION__ , __LINE__, ulActionId);
				break;
		}
	}
	return hErr;
}

static HERROR local_Snmp_Mib_CopyOldSnmp( SnmpMib_Contents_t *pstContents )
{
	memset( &pstContents->stOldSnmpMwInfo , 0x00, sizeof( SvcSys_SnmpInformation_t ));
	memcpy( &pstContents->stOldSnmpMwInfo, &pstContents->stSnmpMwInfo, sizeof( SvcSys_SnmpInformation_t ));
}

static HERROR local_Snmp_Mib_ChangeStatus( SnmpMib_Contents_t *pstContents )
{
	if( memcmp ( &pstContents->stOldSnmpMwInfo, &pstContents->stSnmpMwInfo, sizeof( SvcSys_SnmpInformation_t ) ) != 0 )
	{
		SVC_SYS_SetMWPrivateMIBInfo( &pstContents->stSnmpMwInfo );

		local_Snmp_Mib_PrintInfo(pstContents);
	}

	return ERR_OK;
}

static void local_Snmp_Mib_PrintInfo( SnmpMib_Contents_t *pstContents)
{
#if defined(DEBUG_PRINT)
	HxLOG_Print("\n\n =======================\n" );
	HxLOG_Print("======SNMP MIB Print======\n" );
	HxLOG_Print("=======================\n\n" );

	HxLOG_Print( "szSystemId : %s\n" , pstContents->stSnmpMwInfo.szSystemId );
	HxLOG_Print( "szApplicationVer : %s\n" , pstContents->stSnmpMwInfo.szApplicationVer );
	HxLOG_Print( "szMiddleWareVer : %s\n" , pstContents->stSnmpMwInfo.szMiddleWareVer );
	HxLOG_Print( "szSoftBrowserVer : %s\n" , pstContents->stSnmpMwInfo.szSoftBrowserVer );
	HxLOG_Print( "szStbID : %02X%02X%02X%02X%02X%02X\n" , pstContents->stSnmpMwInfo.szStbID[0],
		pstContents->stSnmpMwInfo.szStbID[1],pstContents->stSnmpMwInfo.szStbID[2],pstContents->stSnmpMwInfo.szStbID[3],
		pstContents->stSnmpMwInfo.szStbID[4],pstContents->stSnmpMwInfo.szStbID[5]);

	HxLOG_Print( "szBcasCardId : %s\n" , pstContents->stSnmpMwInfo.szBcasCardId );
	HxLOG_Print( "szCcasCardId : %s\n" , pstContents->stSnmpMwInfo.szCcasCardId );

	HxLOG_Print( "ulDownFrequency : %d\n", pstContents->stSnmpMwInfo.ulDownFrequency );
	HxLOG_Print( "ulWatchingChannelId : %d\n", pstContents->stSnmpMwInfo.ulWatchingChannelId );
	HxLOG_Print( "ulWatchingNetworkId : %d\n", pstContents->stSnmpMwInfo.ulWatchingNetworkId );

	HxLOG_Print( "ulWatchingTunerGroupId : %d\n", (HINT32)pstContents->stSnmpMwInfo.ulWatchingTunerGroupId );
	HxLOG_Print( "etWatchingChannelType : %d\n", (HINT32)pstContents->stSnmpMwInfo.etWatchingChannelType );

	HxLOG_Print( "etPowerStatus : %d\n", (HINT32)pstContents->stSnmpMwInfo.etPowerStatus );
	HxLOG_Print( "etPlayStatus : %d\n", (HINT32)pstContents->stSnmpMwInfo.etPlayStatus );

	HxLOG_Print( "ulTuner1ChannelId : %d\n", pstContents->stSnmpMwInfo.ulTuner1ChannelId );
	HxLOG_Print( "ulTuner1NetworkId : %d\n", pstContents->stSnmpMwInfo.ulTuner1NetworkId );
	HxLOG_Print( "ulTuner1Frequency : %d\n", pstContents->stSnmpMwInfo.ulTuner1Frequency );

	HxLOG_Print( "ulTuner2ChannelId : %d\n", pstContents->stSnmpMwInfo.ulTuner2ChannelId );
	HxLOG_Print( "ulTuner2NetworkId : %d\n", pstContents->stSnmpMwInfo.ulTuner2NetworkId );
	HxLOG_Print( "ulTuner2Frequency : %d\n", pstContents->stSnmpMwInfo.ulTuner2Frequency );

	HxLOG_Print("\n\n =======================\n" );
	HxLOG_Print("======SNMP MIB End=======\n" );
	HxLOG_Print("=======================\n\n" );
#endif
}

/**************************************************************************************************/
#define _________XMGR_Snmp_Mib_Private_Destroy__________________________________________________________
/**************************************************************************************************/

/**************************************************************************************************/
#define _________XMGR_Snmp_Mib_Private_Proc_UI__________________________________________________________
/**************************************************************************************************/

/**************************************************************************************************/
#define _________XMGR_Snmp_Mib_Private_Proc_MSG_RUNNIG__________________________________________________________
/**************************************************************************************************/

//MSG_GWM_CREATE
static BUS_Result_t local_Snmp_Mib_MsgBusCreate(SnmpMib_Contents_t *pstContents, HINT32 p1, HINT32 p2, HINT32 p3)
{
	local_Snmp_Mib_InitContent(pstContents , p1);
	local_Snmp_Mib_ChangeStatus( pstContents );

	return ERR_BUS_NO_ERROR;
}

//MSG_GWM_DESTROY
static BUS_Result_t local_Snmp_Mib_MsgBusDestroy(SnmpMib_Contents_t *pstContents, HINT32 p1, HINT32 p2, HINT32 p3)
{
	/* do nothing */
	return ERR_BUS_NO_ERROR;
}

// MSG_GWM_GO_STANDBY
static BUS_Result_t local_Snmp_Mib_MsgBusGoStandBy(SnmpMib_Contents_t *pstContents, HINT32 p1, HINT32 p2, HINT32 p3)
{
	local_Snmp_Mib_CopyOldSnmp( pstContents );
	local_Snmp_Mib_ChangePowerStatus( pstContents );
	local_Snmp_Mib_ChangeStatus( pstContents );

	return MESSAGE_PASS;
}

// MSG_GWM_QUIT_STANDBY
static BUS_Result_t local_Snmp_Mib_MsgBusQuitStandBy(SnmpMib_Contents_t *pstContents, HINT32 p1, HINT32 p2, HINT32 p3)
{
	local_Snmp_Mib_CopyOldSnmp( pstContents );
	local_Snmp_Mib_ChangePowerStatus( pstContents );
	local_Snmp_Mib_ChangeStatus( pstContents );

	return MESSAGE_PASS;
}

// eSEVT_CAS_SC_REMOVED
static BUS_Result_t local_Snmp_Mib_EvtCasScRemoved(SnmpMib_Contents_t *pstContents, HINT32 p1, HINT32 p2, HINT32 p3)
{
	local_Snmp_Mib_CopyOldSnmp( pstContents );
	local_Snmp_Mib_SetCardInfo( pstContents );
	local_Snmp_Mib_ChangeStatus( pstContents );

	return ERR_BUS_NO_ERROR;
}

// eSEVT_CAS_SC_INSERTED
static BUS_Result_t local_Snmp_Mib_EvtCasScInserted(SnmpMib_Contents_t *pstContents, HINT32 p1, HINT32 p2, HINT32 p3)
{
	local_Snmp_Mib_CopyOldSnmp( pstContents );
	local_Snmp_Mib_SetCardInfo( pstContents );
	local_Snmp_Mib_ChangeStatus( pstContents );

	return ERR_BUS_NO_ERROR;
}

// MSG_AM_ACTION_STARTED
static BUS_Result_t local_Snmp_Mib_MsgMgrActionStarted(SnmpMib_Contents_t *pstContents, Handle_t handle, HINT32 p1, HINT32 p2, HINT32 p3)
{
	local_Snmp_Mib_CopyOldSnmp( pstContents );
	local_Snmp_Mib_ChangeTunerInfo( pstContents , handle, TRUE );
	local_Snmp_Mib_SetActionSatuts( pstContents );
	local_Snmp_Mib_ChangeStatus( pstContents );

	return ERR_BUS_NO_ERROR;
}

// MSG_AM_ACTION_STOPPED
static BUS_Result_t local_Snmp_Mib_MsgMgrActionStopped(SnmpMib_Contents_t *pstContents, Handle_t handle, HINT32 p1, HINT32 p2, HINT32 p3)
{
	local_Snmp_Mib_CopyOldSnmp( pstContents );
	local_Snmp_Mib_ChangeTunerInfo( pstContents , handle, FALSE );
	local_Snmp_Mib_SetActionSatuts( pstContents );
	local_Snmp_Mib_ChangeStatus( pstContents );

	return ERR_BUS_NO_ERROR;
}

// MSG_AM_ACTION_STOPPED
static BUS_Result_t local_Snmp_Mib_MsgMgrActionTypeChanged(SnmpMib_Contents_t *pstContents, Handle_t handle, HINT32 p1, HINT32 p2, HINT32 p3)
{
	local_Snmp_Mib_CopyOldSnmp( pstContents );
	local_Snmp_Mib_SetActionSatuts( pstContents );
	local_Snmp_Mib_ChangeStatus( pstContents );

	return ERR_BUS_NO_ERROR;
}

// eMEVT_LIVE_NOTIFY_SVC_CHANGED
static BUS_Result_t local_Snmp_Mib_MsgWtvSvcChanged(SnmpMib_Contents_t *pstContents, Handle_t handle, HINT32 p1, HINT32 p2, HINT32 p3)
{
	local_Snmp_Mib_CopyOldSnmp( pstContents );
	local_Snmp_Mib_SetActionSatuts( pstContents );
	local_Snmp_Mib_ChangeStatus( pstContents );

	return ERR_BUS_NO_ERROR;
}

STATIC BUS_Result_t xproc_SNMPMIB(HINT32 nMessage, Handle_t handle, HINT32 p1, HINT32 p2, HINT32 p3)
{
	BUS_Result_t			eResult	= ERR_BUS_NO_ERROR;
	SnmpMib_Contents_t *pstContents = local_Snmp_Mib_GetContext();

	switch (nMessage)
	{
	case eMEVT_BUS_CREATE:
		eResult = local_Snmp_Mib_MsgBusCreate(pstContents, p1, p2, p3);
		break;

	case eMEVT_BUS_DESTROY:
		eResult = local_Snmp_Mib_MsgBusDestroy(pstContents, p1, p2, p3);
		break;

	case eMEVT_BUS_READY_SHUTDOWN:
		eResult = local_Snmp_Mib_MsgBusGoStandBy(pstContents, p1, p2, p3);
		break;

	case eMEVT_BUS_GO_OPERATION :
		eResult = local_Snmp_Mib_MsgBusQuitStandBy(pstContents, p1, p2, p3);
		break;

	case eSEVT_CAS_SC_REMOVED:
		eResult = local_Snmp_Mib_EvtCasScRemoved(pstContents, p1, p2, p3);
		break;

	case eSEVT_CAS_SC_INSERTED:
		eResult = local_Snmp_Mib_EvtCasScInserted(pstContents, p1, p2, p3);
		break;

	case eMEVT_ACTION_NOTIFY_TAKEN:
		eResult = local_Snmp_Mib_MsgMgrActionStarted(pstContents, handle, p1, p2, p3);
		break;

	case eMEVT_ACTION_NOTIFY_RELEASED:
		eResult = local_Snmp_Mib_MsgMgrActionStopped(pstContents, handle, p1, p2, p3);
		break;

	case eMEVT_ACTION_NOTIFY_TYPE_CHANGED:
		eResult = local_Snmp_Mib_MsgMgrActionTypeChanged(pstContents, handle, p1, p2, p3);
		break;

	case eMEVT_LIVE_NOTIFY_SVC_CHANGED:
		eResult = local_Snmp_Mib_MsgWtvSvcChanged(pstContents, handle, p1, p2, p3);
		break;

	default:
		break;
	}

	if(eResult != ERR_BUS_NO_ERROR)
	{
		return eResult;
	}

	return BUS_ProcessMessageDefault(nMessage, handle, p1, p2, p3);
}

/**************************************************************************************************/
#define _________XMGR_Snmp_Mib_Public_Proc____________________________________________________
/**************************************************************************************************/

HERROR XMGR_SNMP_MIB_Create_Japan(HBOOL bStartNormal)
{
	BUS_Result_t		hRes;

	hRes = BUS_MGR_Create("xproc_SNMPMIB", APP_SNMP_MIB_PRIORITY, xproc_SNMPMIB, 0, bStartNormal, 0, 0);

	return (ERR_BUS_NO_ERROR != hRes) ? ERR_FAIL : ERR_OK;
}

/**************************************************************************************************/
#define _________XMGR_Snmp_Mib_End_Of_File____________________________________________________
/**************************************************************************************************/




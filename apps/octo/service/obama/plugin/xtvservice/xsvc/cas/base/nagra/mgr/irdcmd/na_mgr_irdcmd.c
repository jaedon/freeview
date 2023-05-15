/**************************************************************************************************/
#define ________CA_CAK_IRDCMD_Private_Include______________________________________________
/**************************************************************************************************/
#include "vkernel.h"
#include "na_glob.h"

#include "db_svc.h"
#include "util.h"
#include "na_mgr_cak_irdcmd.h"
#include <_svc_cas_mgr_nvram.h>
#include <_svc_cas_mgr_common.h>

/**************************************************************************************************/
#define ________CA_CAK_IRDCMD_Golbal_Value________________________________________________
/**************************************************************************************************/


/**************************************************************************************************/
#define ________CA_CAK_IRDCMD_Private_Define______________________________________________
/**************************************************************************************************/




#define EMM_COMMAND_VALUE						(0x64)
#define MAX_IRD_COMMAND_LENGTH					(0x80) // in case  of Merlin
#define IRDCMD_CHECK_SUM_START_OFFSET			(0x06)		// Data Check Sum을 하기 위한 시작 위치
#define MAX_SEQUENCE_NUMBER_SIZE 				(16)

#if defined(CONFIG_MW_CAS_NAGRA_IRDCMD_MAIL)
#define MAX_MAIL_SEQUENCE_NUMBER_SIZE 			(300)
#endif

#if defined(CONFIG_MW_CAS_NAGRA_IRDCMD_POPUP)
#define MAX_POPUP_MSG_SEQUENCE_NUMBER_SIZE	(300)
#endif

#if defined(CONFIG_MW_CAS_NAGRA_IRDCMD_EMMWAKEUP)
#define TEST_EMM_WAKE							(0)

#if (TEST_EMM_WAKE == 1)
#define DEFALT_SLEEPING_DURATION				(2*60) 		// 2 min
#define DEFALT_WAKING_DURATION					(3*60)  	// 3 min
#else
#define DEFALT_SLEEPING_DURATION				(23*60*60)  // 23 hour
#define DEFALT_WAKING_DURATION					(1*60*60)  	// 1 hour
#endif
#endif


/**************************************************************************************************/
#define ________CA_CAK_IRDCMD_Private_Type_Define_________________________________________
/**************************************************************************************************/
#if defined(CONFIG_MW_CAS_NAGRA_OP_AMX)
typedef enum tagIrdCmdType
{
	eResetPincode = 0,
	eSetPincode,
	eForceDefSetting,
	eZoneID,
	eEmmWakeUp,
	eMAIL,
	eDisplayPopUp,
	eIrdCmdNum,
	eIrdCmdInvalid
} IRDCMD_TYPE_t;
#else
typedef enum tagIrdCmdType
{
	eApsHdPlus = 0,
	eResetPincode,
	eForceIdentification,
	eForceTune,
	eSetPincode,
	eForceStandby,
	eForceDefSetting,
	eChangeUsageID,
	eLogicalDiskFormat,
	eEmmWakeUp,
	eRebootStb,
	eMAIL,
	eDisplayPopUp,
	eIrdCmdNum,
	eIrdCmdInvalid
} IRDCMD_TYPE_t;
#endif

typedef struct tagIrdCmdSeqnum
{
	TUnsignedInt32 ulSequenceNumberCmd[MAX_SEQUENCE_NUMBER_SIZE];			//	4 * 16
	TUnsignedInt32 ulNumberOfSequenceProcessed;								//	4
} IRDCMD_SEQNUM_t;															// 	68

#if defined(CONFIG_MW_CAS_NAGRA_IRDCMD_MAIL)
typedef struct tagIrdCmdMailSeqnum
{
	TUnsignedInt32 ulSequenceNumberCmd[MAX_MAIL_SEQUENCE_NUMBER_SIZE];		//	4 * 300
	TUnsignedInt32 ulNumberOfSequenceProcessed;								//	4
} IRDCMD_MAIL_SEQNUM_t;														//	1024
#endif

#if defined(CONFIG_MW_CAS_NAGRA_IRDCMD_POPUP)
typedef struct tagIrdCmdPopupSeqnum
{
	TUnsignedInt32 ulSequenceNumberCmd[MAX_POPUP_MSG_SEQUENCE_NUMBER_SIZE];			//	4 * 300
	TUnsignedInt32 ulNumberOfSequenceProcessed;										//	4
} IRDCMD_POPUP_MSG_SEQNUM_t;														//	1204
#endif


/**************************************************************************************************/
#define ________CA_CAK_IRDCMD_Private_Static_Value_________________________________________
/**************************************************************************************************/
static IRDCMD_SEQNUM_t	 			s_stIrdCmdSeqNum[eIrdCmdNum-2];					//	68 * eIrdCmdNum (mail과 pop-up은 달리 처리)
#if defined(CONFIG_MW_CAS_NAGRA_IRDCMD_MAIL)
static IRDCMD_MAIL_SEQNUM_t			s_stMailSeqNum;									//	1024
#endif
#if defined(CONFIG_MW_CAS_NAGRA_IRDCMD_POPUP)
static IRDCMD_POPUP_MSG_SEQNUM_t	s_stPopMsgSeqNum;								//	1024
#endif
static TIrdCmdCallbackFunc			s_pfnIrdCmdCallback = NULL;
#if defined(CONFIG_MW_CAS_NAGRA_IRDCMD_EMMWAKEUP)
static NA_EMM_WAKEUP_INFO			s_stEmmWakeupInfo;
static TUnsignedInt8				s_ucCurrWakingLoopCnt = 0;
#endif


/**************************************************************************************************/
#define ________CA_CAK_IRDCMD_Private_Static_Prototype______________________________________
/**************************************************************************************************/
static HERROR local_irdCmd_WriteDataToEEPROM(IRDCMD_TYPE_t eIrdCmdType, IRDCMD_SEQNUM_t *pstCmdSeqNum);
static HERROR local_irdCmd_ReadDataFromEEPROM(IRDCMD_TYPE_t eIrdCmdType, IRDCMD_SEQNUM_t *pstCmdSeqNum);

#if defined(CONFIG_MW_CAS_NAGRA_IRDCMD_MAIL)
static HERROR local_irdCmd_WriteMailDataToEEPROM(IRDCMD_MAIL_SEQNUM_t *pstCmdMailSeqNum);
static HERROR local_irdCmd_ReadMailDataFromEEPROM(IRDCMD_MAIL_SEQNUM_t *pstCmdMailSeqNum);
#endif

#if defined(CONFIG_MW_CAS_NAGRA_IRDCMD_POPUP)
static HERROR local_irdCmd_WritePopMsgDataToEEPROM(IRDCMD_POPUP_MSG_SEQNUM_t *pstCmdPopSeqNum);
static HERROR local_irdCmd_ReadPopMsgDataFromEEPROM(IRDCMD_POPUP_MSG_SEQNUM_t *pstCmdPopSeqNum);
#endif

static HERROR local_irdCmd_ProcessSequenceNumber(IRDCMD_TYPE_t eIrdCmdType, TUnsignedInt8 *pxIrdCmd);
static HERROR local_irdCmd_GetIRDCmdTypeByRDCmdId(NA_IRDCMD_ID_t eIrdCmdId, IRDCMD_TYPE_t *peCmdType);
static HERROR local_irdCmd_IsValidCommand(TUnsignedInt8 *pxIrdCmd);
static HERROR local_irdCmd_ClearSequencenNumber(void);

static HERROR local_irdCmd_ProcGetPincodeByCommand(TUnsignedInt8 *pxIrdCmd, TUnsignedInt8 *pszPincode, TUnsignedInt8 *pucLen);
static HERROR local_irdCmd_ProcGetUsageIdByCommand(TUnsignedInt8 *pxIrdCmd, TUnsignedInt8 *pszUsageId);
static HERROR local_irdCmd_ProcGetServiceHandleByCommand(TUnsignedInt8 *pxIrdCmd, TUnsignedInt32 *pSvcHandle);
#if defined(CONFIG_MW_CAS_NAGRA_OP_SES)
static HERROR local_irdCmd_ProcDisplayPorfileByCommand(TUnsignedInt8 *pxIrdCmd);
static HERROR local_irdCmd_ProcMessageByCommand(TUnsignedInt8 *pxIrdCmd);
#endif
#if defined(CONFIG_MW_CAS_NAGRA_IRDCMD_MAIL)
static HERROR local_irdCmd_ProcBuildMailMsg(TUnsignedInt8 *pxIrdCmd, TUnsignedInt16 *pusMailId, TUnsignedInt16 *pusDeletedMailId);
#endif
#if defined(CONFIG_MW_CAS_NAGRA_IRDCMD_POPUP)
static HERROR local_irdCmd_ProcBuildPopupMsgMsg(TUnsignedInt8 *pxIrdCmd, TUnsignedInt16 *pusPopupId);
static HERROR local_irdCmd_ProcRemovePopupMsg(void);
#endif
#if defined(CONFIG_MW_CAS_NAGRA_IRDCMD_EMMWAKEUP)
static HERROR local_irdCmd_ProcDisableEmmWakeUp(void);
static HERROR local_irdCmd_ProcEnableEmmWakeUp(TUnsignedInt8 *pxIrdCmd);
static HERROR local_irdCmd_EMM_GetWakeupTime(UNIXTIME *pulTime);
static HERROR local_irdCmd_EMM_ClearTable(void);
static HERROR local_irdCmd_EMM_InitWakingTable(void);
static HERROR local_irdCmd_EMM_HasMoreNextWaking(TBoolean *pbNext);
static HERROR local_irdCmd_EMM_GetWakingTuneInfo(TUnsignedInt8 *pucNetworkIdFlag, TUnsignedInt16 *pusNetworkId,
															TUnsignedInt8 *pucTsIdFlag, TUnsignedInt16 *pusTsId,
															TUnsignedInt8 *pucServiceIdFlag, TUnsignedInt16 *pusServiceId);
static HERROR local_irdCmd_EMM_GetWakingTimer(UNIXTIME *pulTime);
#endif
#if defined(CONFIG_MW_CAS_NAGRA_IRDCMD_ZONEID)
static HERROR local_irdCmd_ProcSetZoneId(TUnsignedInt8 *pxIrdCmd);
static HERROR local_irdCmd_ProcCancelZoneId(void);
static HERROR local_irdCmd_GetZoneId(TUnsignedInt8* pZoneId, TUnsignedInt32 nMaxLen);
static HERROR local_irdCmd_ProcSetCountryCode(TUnsignedInt8 *pxIrdCmd);
static HERROR local_irdCmd_GetCountryCode(TUnsignedInt8* pCountryCode, TUnsignedInt32 nMaxLen);
#endif


/**************************************************************************************************/
#define ________CA_CAK_IRDCMD_Private_Static_Prototype_Body____________________________________
/**************************************************************************************************/
static HERROR local_irdCmd_WriteDataToEEPROM(IRDCMD_TYPE_t eIrdCmdType, IRDCMD_SEQNUM_t *pstCmdSeqNum)
{
	TUnsignedInt16 usOffSet = 0;

	if ( eIrdCmdType >= eIrdCmdNum-2 )
	{
		HxLOG_Error("eIrdCmdType >= eIrdCmdNum-2\n");
		return ERR_FAIL;
	}

	if ( pstCmdSeqNum == NULL )
	{
		HxLOG_Error("pstCmdSeqNum == NULL\n");
		return ERR_FAIL;
	}

	HxLOG_Print("IrdCmd_type = (0x%x), NumberOfSequenceProcessed = (%d)\n", eIrdCmdType, (int)pstCmdSeqNum->ulNumberOfSequenceProcessed);
	usOffSet = sizeof(IRDCMD_SEQNUM_t)*eIrdCmdType;
	if ( svc_cas_DevPortNVRAMWriteData(eDxCAS_GROUPID_NA, CA_NVRAM_IRDCMD_SEQNUM_OFFSET+usOffSet, sizeof(IRDCMD_SEQNUM_t), (TUnsignedInt8*)pstCmdSeqNum) != ERR_OK )
	{
		return ERR_FAIL;
	}
	return ERR_OK;
}


static HERROR local_irdCmd_ReadDataFromEEPROM(IRDCMD_TYPE_t eIrdCmdType, IRDCMD_SEQNUM_t *pstCmdSeqNum)
{
	TUnsignedInt16 usOffSet = 0;

	if ( eIrdCmdType >= eIrdCmdNum-2 )
	{
		HxLOG_Error("eIrdCmdType >= eIrdCmdNum-2\n");
		return ERR_FAIL;
	}

	if ( pstCmdSeqNum == NULL )
	{
		HxLOG_Error("pstCmdSeqNum == NULL\n");
		return ERR_FAIL;
	}

	HxLOG_Print("IrdCmd_type = (0x%x), NumberOfSequenceProcessed = (%d)\n", eIrdCmdType, (int)pstCmdSeqNum->ulNumberOfSequenceProcessed);
	usOffSet = sizeof(IRDCMD_SEQNUM_t)*eIrdCmdType;
	if ( svc_cas_DevPortNVRAMReadData(eDxCAS_GROUPID_NA, CA_NVRAM_IRDCMD_SEQNUM_OFFSET+usOffSet, sizeof(IRDCMD_SEQNUM_t), (TUnsignedInt8*)pstCmdSeqNum) != ERR_OK )
	{
		return ERR_FAIL;
	}
	return ERR_OK;
}


#if defined(CONFIG_MW_CAS_NAGRA_IRDCMD_MAIL)
static HERROR local_irdCmd_WriteMailDataToEEPROM(IRDCMD_MAIL_SEQNUM_t *pstCmdMailSeqNum)
{
	if ( pstCmdMailSeqNum == NULL )
	{
		HxLOG_Error("pstCmdMailSeqNum == NULL\n");
		return ERR_FAIL;
	}

	HxLOG_Print("nNumberOfSequenceProcessed = (%d)\n",  (HINT32)pstCmdMailSeqNum->ulNumberOfSequenceProcessed);
	if ( svc_cas_DevPortNVRAMWriteData(eDxCAS_GROUPID_NA, CA_NVRAM_IRDCMD_MAIL_SEQNUM_OFFSET, sizeof(IRDCMD_MAIL_SEQNUM_t), (HUINT8*)pstCmdMailSeqNum) != ERR_OK )
	{
		return ERR_FAIL;
	}
	return ERR_OK;
}


static HERROR local_irdCmd_ReadMailDataFromEEPROM(IRDCMD_MAIL_SEQNUM_t *pstCmdMailSeqNum)
{
	if ( pstCmdMailSeqNum == NULL )
	{
		HxLOG_Error("pstCmdMailSeqNum == NULL\n");
		return ERR_FAIL;
	}

	HxLOG_Print("nNumberOfSequenceProcessed = (%d)\n",  (HINT32)pstCmdMailSeqNum->ulNumberOfSequenceProcessed);
	if ( svc_cas_DevPortNVRAMReadData(eDxCAS_GROUPID_NA, CA_NVRAM_IRDCMD_MAIL_SEQNUM_OFFSET, sizeof(IRDCMD_MAIL_SEQNUM_t), (HUINT8*)pstCmdMailSeqNum) != ERR_OK )
	{
		return ERR_FAIL;
	}
	return ERR_OK;
}
#endif


#if defined(CONFIG_MW_CAS_NAGRA_IRDCMD_POPUP)
static HERROR local_irdCmd_WritePopMsgDataToEEPROM(IRDCMD_POPUP_MSG_SEQNUM_t *pstCmdPopSeqNum)
{
	if ( pstCmdPopSeqNum == NULL )
	{
		HxLOG_Error("pstCmdPopSeqNum == NULL\n");
		return ERR_FAIL;
	}

	HxLOG_Print("nNumberOfSequenceProcessed = (%d)\n",  (HINT32)pstCmdPopSeqNum->ulNumberOfSequenceProcessed);
	if ( svc_cas_DevPortNVRAMWriteData(eDxCAS_GROUPID_NA, CA_NVRAM_IRDCMD_POPUP_SEQNUM_OFFSET, sizeof(IRDCMD_POPUP_MSG_SEQNUM_t), (HUINT8*)pstCmdPopSeqNum) != ERR_OK )
	{
		return ERR_FAIL;
	}
	return ERR_OK;
}


static HERROR local_irdCmd_ReadPopMsgDataFromEEPROM(IRDCMD_POPUP_MSG_SEQNUM_t *pstCmdPopSeqNum)
{
	if ( pstCmdPopSeqNum == NULL )
	{
		HxLOG_Error("pstCmdPopSeqNum == NULL\n");
		return ERR_FAIL;
	}

	HxLOG_Print("nNumberOfSequenceProcessed = (%d)\n",  (HINT32)pstCmdPopSeqNum->ulNumberOfSequenceProcessed);
	if ( svc_cas_DevPortNVRAMReadData(eDxCAS_GROUPID_NA, CA_NVRAM_IRDCMD_POPUP_SEQNUM_OFFSET, sizeof(IRDCMD_POPUP_MSG_SEQNUM_t), (HUINT8*)pstCmdPopSeqNum) != ERR_OK )
	{
		return ERR_FAIL;
	}
	return ERR_OK;
}
#endif

#if defined(CONFIG_MW_CAS_NAGRA_IRDCMD_ZONEID)
static HERROR local_irdCmd_ProcSetZoneId(TUnsignedInt8 *pxIrdCmd)
{

	TUnsignedInt8 	ucLen = 0;
	TUnsignedInt8		*pucCmd;

	NA_EMM_ZONEID_INFO 	stZoneId;

	if ( pxIrdCmd == NULL )
	{
		HxLOG_Error("pIrdCmd == NULL\n");
		return ERR_FAIL;
	}

	pucCmd = pxIrdCmd;

	// get length
	ucLen = *(pucCmd + 1);
	if(ucLen < 13)
	{
		HxLOG_Error("cmd length error(%d) \n", ucLen);
		return ERR_FAIL;
	}

	HxSTD_memset(&stZoneId, 0x00, sizeof(NA_EMM_ZONEID_INFO));
	// Skip Header Field : EMM Header, Length, Sequence Number, Command ID, Operation ID
	pucCmd += 7;
	if(*pucCmd & EMM_IRD_ZONEID_MASK)
	{
		// Skip zone id, if there is zone id indicator.
		pucCmd += 6;
	}
	pucCmd += 1;
	
	if(ucLen  < 25)
	{
		// there is zone id only.
		stZoneId.ucZoneId[0] = pucCmd[0];
		stZoneId.ucZoneId[1] = pucCmd[1];
		stZoneId.ucZoneId[2] = pucCmd[2];
		stZoneId.ucZoneId[3] = pucCmd[3];
		stZoneId.ucZoneId[4] = pucCmd[4];
		stZoneId.ucZoneId[5] = pucCmd[5];

#if defined(CONFIG_OP_TELEFONICA)
	{
		  HUINT32 ulDbZoneId;
		  
		  ulDbZoneId = stZoneId.ucZoneId[4]<<8 |stZoneId.ucZoneId[5];
		  HxLOG_Print("\n\n\n[%s] IrdZoneId(0x%x, 0x%x, 0x%x, 0x%x, 0x%x, 0x%x) \n", __FUNCTION__, 
		  	stZoneId.ucZoneId[0], stZoneId.ucZoneId[1], stZoneId.ucZoneId[2], stZoneId.ucZoneId[3], stZoneId.ucZoneId[4], stZoneId.ucZoneId[5]);

		  HxLOG_Print("[%s] nDbZoneId(0x%x) \n\n\n", __FUNCTION__, ulDbZoneId);
		  
		  DB_SETUP_SetUserItemValue(USERCONFIG_ITEMNAME_ZONE_ID, ulDbZoneId);
		  DB_SETUP_SyncUserItem(USERCONFIG_ITEMNAME_ZONE_ID);
	 }
#endif
		
	}
	else
	{
		// there are options value.
		stZoneId.ucZoneId[0] = pucCmd[0];
		stZoneId.ucZoneId[1] = pucCmd[1];
		stZoneId.ucZoneId[2] = pucCmd[2];
		stZoneId.ucZoneId[3] = pucCmd[3];
		stZoneId.ucZoneId[4] = pucCmd[4];
		stZoneId.ucZoneId[5] = pucCmd[5];

		stZoneId.nIpAddress = (pucCmd[6] << 24) | (pucCmd[7] << 16) | (pucCmd[8] << 8) | pucCmd[9];
		stZoneId.usPortNumber = (pucCmd[10] << 8) | pucCmd[11];
		
		stZoneId.ucAccountId[0] = pucCmd[12];
		stZoneId.ucAccountId[1] = pucCmd[13];
		stZoneId.ucAccountId[2] = pucCmd[14];
		stZoneId.ucAccountId[3] = pucCmd[15];
		stZoneId.ucAccountId[4] = pucCmd[16];
		stZoneId.ucAccountId[5] = pucCmd[17];

		HxLOG_Print("Ip Address : %08x\n", (unsigned int)stZoneId.nIpAddress);
		HxLOG_Print("Port Number : %04x\n", stZoneId.usPortNumber);
		HxLOG_Print("Account ID : %02x%02x%02x%02x%02x%02x\n", 
			stZoneId.ucAccountId[0], stZoneId.ucAccountId[1], stZoneId.ucAccountId[2], stZoneId.ucAccountId[3], stZoneId.ucAccountId[4], stZoneId.ucAccountId[5]);
	}

	HxLOG_Print("Zone ID : %02x%02x%02x%02x%02x%02x\n", 
		stZoneId.ucZoneId[0], stZoneId.ucZoneId[1], stZoneId.ucZoneId[2], stZoneId.ucZoneId[3], stZoneId.ucZoneId[4], stZoneId.ucZoneId[5]);
	
	stZoneId.ucReceived = 1;

	// EEPROM에 새로운 IRD Command 저장
	if ( svc_cas_DevPortNVRAMWriteData(eDxCAS_GROUPID_NA, EEPM_EXT_IRDCMD_ZONEID_OFFSET, sizeof(NA_EMM_ZONEID_INFO), (HUINT8 *)&stZoneId) != ERR_OK )
	{
		HxLOG_Error("error in svc_cas_DevPortNVRAMWriteData...\n");
		return ERR_FAIL;
	}

	return ERR_OK;
}

static HERROR local_irdCmd_ProcCancelZoneId(void)
{
	NA_EMM_ZONEID_INFO stZoneId;

	HxSTD_memset(&stZoneId, 0x00, sizeof(NA_EMM_ZONEID_INFO));

	stZoneId.ucReceived = 0;
	if ( svc_cas_DevPortNVRAMWriteData(eDxCAS_GROUPID_NA, EEPM_EXT_IRDCMD_ZONEID_OFFSET, sizeof(NA_EMM_ZONEID_INFO), (HUINT8 *)&stZoneId) != ERR_OK )
	{
		HxLOG_Error("error in svc_cas_DevPortNVRAMWriteData...\n");
		return ERR_FAIL;
	}
	return ERR_OK;
}

static HERROR local_irdCmd_GetZoneId(TUnsignedInt8* pZoneId, TUnsignedInt32 nMaxLen)
{
	NA_EMM_ZONEID_INFO stZoneId;

	if(nMaxLen < MAX_ZONE_ID_LENGTH)
	{
		HxLOG_Print("invalid parameter...\n");
		return ERR_FAIL;
	}
	
	VK_memset(&stZoneId, 0x00, sizeof(NA_EMM_ZONEID_INFO));

	if ( svc_cas_DevPortNVRAMReadData(eDxCAS_GROUPID_NA, EEPM_EXT_IRDCMD_ZONEID_OFFSET, sizeof(NA_EMM_ZONEID_INFO), (HUINT8 *)&stZoneId) != ERR_OK )
	{
		HxLOG_Error("error in svc_cas_DevPortNVRAMReadData...\n");
		return ERR_FAIL;
	}


	if(stZoneId.ucReceived != 1)
	{
		return ERR_FAIL;
	}

	pZoneId[0] = stZoneId.ucZoneId[0];
	pZoneId[1] = stZoneId.ucZoneId[1];
	pZoneId[2] = stZoneId.ucZoneId[2];
	pZoneId[3] = stZoneId.ucZoneId[3];
	pZoneId[4] = stZoneId.ucZoneId[4];
	pZoneId[5] = stZoneId.ucZoneId[5];
	
	return ERR_OK;
}

static HERROR local_irdCmd_ProcSetCountryCode(TUnsignedInt8 *pxIrdCmd)
{

	TUnsignedInt8 	ucLen = 0;
	TUnsignedInt8		*pucCmd;

	NA_EMM_COUNTRYCODE_INFO 	stCountryCode;

	if ( pxIrdCmd == NULL )
	{
		HxLOG_Error("pIrdCmd == NULL\n");
		return ERR_FAIL;
	}

	pucCmd = pxIrdCmd;

	// get length
	ucLen = *(pucCmd + 1);
	if(ucLen < 9)
	{
		HxLOG_Error("cmd length error(%d) \n", ucLen);
		return ERR_FAIL;
	}

	HxSTD_memset(&stCountryCode, 0x00, sizeof(NA_EMM_COUNTRYCODE_INFO));
	// Skip Header Field : EMM Header, Length, Sequence Number, Command ID, Operation ID
	pucCmd += 7;
	if(*pucCmd & EMM_IRD_ZONEID_MASK)
	{
		// Skip zone id, if there is zone id indicator.
		pucCmd += 6;
	}
	pucCmd += 1;

	stCountryCode.ucCountryCode[0] = pucCmd[0];
	stCountryCode.ucCountryCode[1] = pucCmd[1];
	stCountryCode.ucCountryCode[2] = pucCmd[2];
	if(ucLen  > 22)
	{
		stCountryCode.nIpAddress = (pucCmd[6] << 24) | (pucCmd[7] << 16) | (pucCmd[8] << 8) | pucCmd[9];
		stCountryCode.usPortNumber = (pucCmd[10] << 8) | pucCmd[11];
		
		stCountryCode.ucAccountId[0] = pucCmd[12];
		stCountryCode.ucAccountId[1] = pucCmd[13];
		stCountryCode.ucAccountId[2] = pucCmd[14];
		stCountryCode.ucAccountId[3] = pucCmd[15];
		stCountryCode.ucAccountId[4] = pucCmd[16];
		stCountryCode.ucAccountId[5] = pucCmd[17];

		HxLOG_Print("Ip Address : %08x\n", (unsigned int)stCountryCode.nIpAddress);
		HxLOG_Print("Port Number : %04x\n", stCountryCode.usPortNumber);
		HxLOG_Print("Account ID : %02x%02x%02x%02x%02x%02x\n", 
			stCountryCode.ucAccountId[0], stCountryCode.ucAccountId[1], stCountryCode.ucAccountId[2], stCountryCode.ucAccountId[3], stCountryCode.ucAccountId[4], stCountryCode.ucAccountId[5]);
	}

	HxLOG_Print("Country Code : %02x%02x%02x\n", 
		stCountryCode.ucCountryCode[0], stCountryCode.ucCountryCode[1], stCountryCode.ucCountryCode[2]);
	
	stCountryCode.ucReceived = 1;

	// EEPROM에 새로운 IRD Command 저장
	if ( svc_cas_DevPortNVRAMWriteData(eDxCAS_GROUPID_NA, EEPM_EXT_IRDCMD_COUNTRYCODE_OFFSET, sizeof(NA_EMM_COUNTRYCODE_INFO), (HUINT8 *)&stCountryCode) != ERR_OK )
	{
		HxLOG_Error("error in svc_cas_DevPortNVRAMWriteData...\n");
		return ERR_FAIL;
	}

	return ERR_OK;
}

static HERROR local_irdCmd_GetCountryCode(TUnsignedInt8* pCountryCode, TUnsignedInt32 nMaxLen)
{
	NA_EMM_COUNTRYCODE_INFO stCountryCode;

	if(nMaxLen < MAX_COUNTRY_CODE_LENGTH)
	{
		HxLOG_Print("invalid parameter...\n");
		return ERR_FAIL;
	}
	
	HxSTD_memset(&stCountryCode, 0x00, sizeof(NA_EMM_COUNTRYCODE_INFO));

	if ( svc_cas_DevPortNVRAMReadData(eDxCAS_GROUPID_NA, EEPM_EXT_IRDCMD_COUNTRYCODE_OFFSET, sizeof(NA_EMM_COUNTRYCODE_INFO), (HUINT8 *)&stCountryCode) != ERR_OK )
	{
		HxLOG_Error("error in svc_cas_DevPortNVRAMReadData...\n");
		return ERR_FAIL;
	}


	if(stCountryCode.ucReceived != 1)
	{
		return ERR_FAIL;
	}

	pCountryCode[0] = stCountryCode.ucCountryCode[0];
	pCountryCode[1] = stCountryCode.ucCountryCode[1];
	pCountryCode[2] = stCountryCode.ucCountryCode[2];
	
	return ERR_OK;
}
#endif

static HERROR local_irdCmd_ProcessSequenceNumber(IRDCMD_TYPE_t eIrdCmdType, TUnsignedInt8 *pxIrdCmd)
{
	TUnsignedInt8	*pucCmd;
	TSignedInt32	i, ulLastIdx;
	TUnsignedInt32	nSeqNum;
	TUnsignedInt8	ucCmdLength = 0;

	if ( pxIrdCmd == NULL )
	{
		HxLOG_Error("pxIrdCmd is NULL\n");
		return ERR_FAIL;
	}

	if ( (eIrdCmdType < 0) || (eIrdCmdType >= eIrdCmdNum) )
	{
		HxLOG_Error("Invalid Value : 0x%x\n", eIrdCmdType);
		return ERR_FAIL;
	}

	pucCmd = pxIrdCmd;
	ucCmdLength = *(pucCmd+1);
	HxLOG_Print("Command Length : %d\n", ucCmdLength);
	nSeqNum = get32bit(pucCmd+2);
	HxLOG_Print("IRD Command Sequence Number : %d\n", (int)nSeqNum);
	pucCmd += 8;	// Sequence Number(4) + command_id(1) + operation(1)

#if defined(CONFIG_MW_CAS_NAGRA_IRDCMD_MAIL)
	if ( eIrdCmdType == eMAIL )
	{
			if ( local_irdCmd_ReadMailDataFromEEPROM(&s_stMailSeqNum) != ERR_OK )
			{
				HxLOG_Error("local_irdCmd_ReadMailDataFromEEPROM() error\n");
				HxSTD_memset(&s_stMailSeqNum, 0, sizeof(IRDCMD_MAIL_SEQNUM_t));
			}

			ulLastIdx = ( s_stMailSeqNum.ulNumberOfSequenceProcessed >= MAX_MAIL_SEQUENCE_NUMBER_SIZE ) ? MAX_MAIL_SEQUENCE_NUMBER_SIZE : s_stMailSeqNum.ulNumberOfSequenceProcessed;
			for ( i = 0 ; i < ulLastIdx ; i++ )
			{
				// 이미 처리된 Command이므로 처리하지 않는다.
				if ( s_stMailSeqNum.ulSequenceNumberCmd[i] == nSeqNum )
				{
					HxLOG_Print("Already Processed Command!Sequence Number : [0x%x]\n", (HUINT32)nSeqNum);
					return ERR_FAIL;
				}
			}
		
			if ( s_stMailSeqNum.ulNumberOfSequenceProcessed < MAX_MAIL_SEQUENCE_NUMBER_SIZE )
			{
				s_stMailSeqNum.ulSequenceNumberCmd[s_stMailSeqNum.ulNumberOfSequenceProcessed] = nSeqNum;
				s_stMailSeqNum.ulNumberOfSequenceProcessed++;
			}
			else
			{
				for ( i=0 ; i<MAX_MAIL_SEQUENCE_NUMBER_SIZE -1 ; i++ )
				{
					// 제일 오래된(첫번째에 저장된 값) 값은 버린다.
					s_stMailSeqNum.ulSequenceNumberCmd[i] = s_stMailSeqNum.ulSequenceNumberCmd[i+1];
				}
				// 제일 마지막에 저장
				s_stMailSeqNum.ulSequenceNumberCmd[i] = nSeqNum;
			}

			if ( local_irdCmd_WriteMailDataToEEPROM(&s_stMailSeqNum) != ERR_OK )
			{
				HxLOG_Error("local_irdCmd_WriteMailDataToEEPROM() error\n");
				return ERR_FAIL;
			}
		return ERR_OK;
	}
#endif

#if defined(CONFIG_MW_CAS_NAGRA_IRDCMD_POPUP)
	if ( eIrdCmdType == eDisplayPopUp )
	{
		if ( local_irdCmd_ReadPopMsgDataFromEEPROM(&s_stPopMsgSeqNum) != ERR_OK )
		{
			HxLOG_Error("local_irdCmd_ReadPopMsgDataFromEEPROM() error\n");
			HxSTD_memset(&s_stPopMsgSeqNum, 0, sizeof(IRDCMD_POPUP_MSG_SEQNUM_t));
		}

		ulLastIdx = ( s_stPopMsgSeqNum.ulNumberOfSequenceProcessed >= MAX_POPUP_MSG_SEQUENCE_NUMBER_SIZE ) ? MAX_POPUP_MSG_SEQUENCE_NUMBER_SIZE : s_stPopMsgSeqNum.ulNumberOfSequenceProcessed;
		for ( i = 0 ; i < ulLastIdx ; i++ )
		{
			// 이미 처리된 Command이므로 처리하지 않는다.
			if ( s_stPopMsgSeqNum.ulSequenceNumberCmd[i] == nSeqNum )
			{
				HxLOG_Error("Already Processed Command!Sequence Number : [0x%x]\n", (HUINT32)nSeqNum);
				return ERR_FAIL;
			}
		}

		if ( s_stPopMsgSeqNum.ulNumberOfSequenceProcessed < MAX_POPUP_MSG_SEQUENCE_NUMBER_SIZE )
		{
			s_stPopMsgSeqNum.ulSequenceNumberCmd[s_stPopMsgSeqNum.ulNumberOfSequenceProcessed] = nSeqNum;
			s_stPopMsgSeqNum.ulNumberOfSequenceProcessed++;
		}
		else
		{
			for ( i=0 ; i<MAX_POPUP_MSG_SEQUENCE_NUMBER_SIZE -1 ; i++ )
			{
				// 제일 오래된(첫번째에 저장된 값) 값은 버린다.
				s_stPopMsgSeqNum.ulSequenceNumberCmd[i] = s_stPopMsgSeqNum.ulSequenceNumberCmd[i+1];
			}
			// 제일 마지막에 저장
			s_stPopMsgSeqNum.ulSequenceNumberCmd[i] = nSeqNum;
		}

		if ( local_irdCmd_WritePopMsgDataToEEPROM(&s_stPopMsgSeqNum) != ERR_OK )
		{
			HxLOG_Error("local_irdCmd_WritePopMsgDataToEEPROM() error\n");
			return ERR_FAIL;
		}
		return ERR_OK;
	}
#endif

	if (eIrdCmdType >= eIrdCmdNum-2)
	{
		HxLOG_Error("Invalid eIrdCmdType : 0x%x\n", eIrdCmdType);
		return ERR_FAIL;
	}

	if ( local_irdCmd_ReadDataFromEEPROM(eIrdCmdType, &s_stIrdCmdSeqNum[eIrdCmdType]) != ERR_OK )
	{
		HxLOG_Error("local_irdCmd_ReadDataFromEEPROM() error\n");
		return ERR_FAIL;
	}

	ulLastIdx = ( s_stIrdCmdSeqNum[eIrdCmdType].ulNumberOfSequenceProcessed >= MAX_SEQUENCE_NUMBER_SIZE ) ? MAX_SEQUENCE_NUMBER_SIZE : s_stIrdCmdSeqNum[eIrdCmdType].ulNumberOfSequenceProcessed;
	for (i=0 ; i<ulLastIdx ; i++)
	{
		// 이미 처리된 Command이므로 처리하지 않는다.
		if ( s_stIrdCmdSeqNum[eIrdCmdType].ulSequenceNumberCmd[i] == nSeqNum )
		{
			HxLOG_Error("Already Processed Command!Sequence Number : %d\n", (int)nSeqNum);
			return ERR_FAIL;
		}
	}

	if ( s_stIrdCmdSeqNum[eIrdCmdType].ulNumberOfSequenceProcessed < MAX_SEQUENCE_NUMBER_SIZE )
	{
		s_stIrdCmdSeqNum[eIrdCmdType].ulSequenceNumberCmd[s_stIrdCmdSeqNum[eIrdCmdType].ulNumberOfSequenceProcessed] = nSeqNum;
		s_stIrdCmdSeqNum[eIrdCmdType].ulNumberOfSequenceProcessed++;
	}
	else
	{
		for (i=0 ; i<MAX_SEQUENCE_NUMBER_SIZE-1 ; i++)
		{
			// 제일 오래된(첫번째에 저장된 값) 값은 버린다.
			s_stIrdCmdSeqNum[eIrdCmdType].ulSequenceNumberCmd[i] = s_stIrdCmdSeqNum[eIrdCmdType].ulSequenceNumberCmd[i+1];
		}
		// 제일 마지막에 저장
		s_stIrdCmdSeqNum[eIrdCmdType].ulSequenceNumberCmd[i] = nSeqNum;
		HxLOG_Error("s_stIrdCmdSeqNum[eIrdCmdType].ulSequenceNumberCmd[%d] = %d\n", i, nSeqNum);
	}

	if ( local_irdCmd_WriteDataToEEPROM(eIrdCmdType, &s_stIrdCmdSeqNum[eIrdCmdType]) != ERR_OK )
	{
		HxLOG_Error("local_irdCmd_WriteDataToEEPROM() error\n");
		return ERR_FAIL;
	}
	return ERR_OK;
}


static HERROR local_irdCmd_GetIRDCmdTypeByRDCmdId(NA_IRDCMD_ID_t eIrdCmdId, IRDCMD_TYPE_t *peCmdType)
{
	IRDCMD_TYPE_t eCmdType = eIrdCmdInvalid;

	if ( peCmdType == NULL )
		return ERR_FAIL;

	switch ( eIrdCmdId )
	{
		case eComIDResetPincode:
#if defined(CONFIG_MW_CAS_NAGRA_IRDCMD_PINCTL)
			eCmdType = eResetPincode;
#else
			eCmdType = eIrdCmdInvalid;
#endif
			break;
		case eComIDSetPincode:
#if defined(CONFIG_MW_CAS_NAGRA_IRDCMD_PINCTL)
			eCmdType = eSetPincode;
#else
			eCmdType = eIrdCmdInvalid;
#endif
			break;

		case eComIDForceIdentification:
#if defined(CONFIG_MW_CAS_NAGRA_IRDCMD_FINGER)
			eCmdType = eForceIdentification;
#else
			eCmdType = eIrdCmdInvalid;
#endif
			break;

		case eComIDForceTune:
#if defined(CONFIG_MW_CAS_NAGRA_IRDCMD_TUNE)
			eCmdType = eForceTune;
#else
			eCmdType = eIrdCmdInvalid;
#endif
			break;

		case eComIDMail:
#if defined(CONFIG_MW_CAS_NAGRA_IRDCMD_MAIL)
			eCmdType = eMAIL;
#else
			eCmdType = eIrdCmdInvalid;
#endif
			break;

		case eComIDSetForceStandby:
#if defined(CONFIG_MW_CAS_NAGRA_IRDCMD_STANDBY)
			eCmdType = eForceStandby;
#else
			eCmdType = eIrdCmdInvalid;
#endif
			break;

		case eComIDSetForceDefSetting:
#if defined(CONFIG_MW_CAS_NAGRA_IRDCMD_DEFSETTING)
			eCmdType = eForceDefSetting;
#else
			eCmdType = eIrdCmdInvalid;
#endif
			break;

		case eComIDDisplayPopUp:
#if defined(CONFIG_MW_CAS_NAGRA_IRDCMD_POPUP)
			eCmdType = eDisplayPopUp;
#else
			eCmdType = eIrdCmdInvalid;
#endif
			break;

		case eComIDChangeUsageID:
#if defined(CONFIG_MW_CAS_NAGRA_IRDCMD_USAGE)
			eCmdType = eChangeUsageID;
#else
			eCmdType = eIrdCmdInvalid;
#endif
			break;

		case eComIDLogicalDiskFormat:
#if defined(CONFIG_MW_CAS_NAGRA_IRDCMD_FORMATDISK)
			eCmdType = eLogicalDiskFormat;
#else
			eCmdType = eIrdCmdInvalid;
#endif
			break;

		case eComIDZoneID:
#if defined(CONFIG_MW_CAS_NAGRA_IRDCMD_ZONEID)
			eCmdType = eZoneID;
#else
			eCmdType = eIrdCmdInvalid;
#endif
			break;

		case eComIDEmmWakeUp:
#if defined(CONFIG_MW_CAS_NAGRA_IRDCMD_EMMWAKEUP)
			eCmdType = eEmmWakeUp;
#else
			eCmdType = eIrdCmdInvalid;
#endif
			break;

		case eComIDRebootStb:
#if defined(CONFIG_MW_CAS_NAGRA_IRDCMD_REBOOT)
			eCmdType = eRebootStb;
#else
			eCmdType = eIrdCmdInvalid;
#endif
			break;

		case eComIDApsHdPlus:
#if defined(CONFIG_MW_CAS_NAGRA_OP_SES)
			eCmdType = eApsHdPlus;
#else
			eCmdType = eIrdCmdInvalid;
#endif
			break;

		case eComIDSetMacrovisionCps:
		case eComIDConfigureStb:
		case eComIDSetNetworkID:
		case eComIDMasterSlave:
		case eComIDUnknown:
			eCmdType = eIrdCmdInvalid;
			break;
	}

	if ( eCmdType == eIrdCmdInvalid )
	{
		return ERR_FAIL;
	}

	*peCmdType = eCmdType;
	return ERR_OK;
}


static HERROR local_irdCmd_IsValidCommand(TUnsignedInt8 *pxIrdCmd)
{
	TUnsignedInt8	*pData;
	TUnsignedInt8	i, ucSum;
	TUnsignedInt32	ulDataSize;

	if ( pxIrdCmd == NULL )
	{
		HxLOG_Error("pIrdCmd == NULL\n");
		return ERR_FAIL;
	}

	if ( *(pxIrdCmd) != EMM_COMMAND_VALUE )
	{
		HxLOG_Error("EMM_COMMAND_VALUE Wrong : 0x%02x. It should be 0x64\n", *(pxIrdCmd));
		return ERR_FAIL;
	}

	pData = pxIrdCmd + IRDCMD_CHECK_SUM_START_OFFSET;
	ulDataSize = *(pxIrdCmd+1) + 2 - IRDCMD_CHECK_SUM_START_OFFSET;
	if ( ulDataSize > (MAX_IRD_COMMAND_LENGTH-IRDCMD_CHECK_SUM_START_OFFSET+2) )
	{
		HxLOG_Error("Data Size is too Big : %d\n", (int)ulDataSize);
		ulDataSize= MAX_IRD_COMMAND_LENGTH-IRDCMD_CHECK_SUM_START_OFFSET+2;
		return ERR_FAIL;
	}

	ucSum = 0;
	for (i=0 ; i<ulDataSize ; i++)
	{
		ucSum = (TUnsignedInt8)(0x00FF & (TUnsignedInt16)(ucSum + *(pData + i)));
	}

	if (ucSum != 0)
	{
		HxLOG_Error("Check Sum Error : Value : %d\n", ucSum);
		return ERR_FAIL;
	}
	return ERR_OK;
}


static HERROR local_irdCmd_ClearSequencenNumber(void)
{
	TUnsignedInt8 ucBuffer[CA_NVRAM_IRDCMD_SEQNUM_SIZE];

	HxSTD_memset(ucBuffer, 0x00, CA_NVRAM_IRDCMD_SEQNUM_SIZE);

	HxLOG_Error("Clear Saved Sequence Number! [Normal Command]\n");
	if ( svc_cas_DevPortNVRAMWriteData(eDxCAS_GROUPID_NA, CA_NVRAM_IRDCMD_SEQNUM_OFFSET, CA_NVRAM_IRDCMD_SEQNUM_SIZE, (TUnsignedInt8 *)ucBuffer) != ERR_OK )
	{
		HxLOG_Info("error in svc_cas_DevPortNVRAMWriteData\n");
		return ERR_FAIL;
	}

#if defined(CONFIG_MW_CAS_NAGRA_IRDCMD_MAIL)
{
	TUnsignedInt8 ucMailBuffer[CA_NVRAM_IRDCMD_MAIL_SEQNUM_SIZE];

	HxSTD_memset(ucMailBuffer, 0x00, CA_NVRAM_IRDCMD_MAIL_SEQNUM_SIZE);
	if ( svc_cas_DevPortNVRAMWriteData(eDxCAS_GROUPID_NA, CA_NVRAM_IRDCMD_MAIL_SEQNUM_OFFSET, CA_NVRAM_IRDCMD_MAIL_SEQNUM_SIZE, (HUINT8*)ucMailBuffer) != ERR_OK )
	{
		return ERR_FAIL;
	}
}
#endif

#if defined(CONFIG_MW_CAS_NAGRA_IRDCMD_POPUP)
{
	TUnsignedInt8 ucPopBuffer[CA_NVRAM_IRDCMD_POPUP_SEQNUM_SIZE];

	HxSTD_memset(ucPopBuffer, 0x00, CA_NVRAM_IRDCMD_POPUP_SEQNUM_SIZE);
	if ( svc_cas_DevPortNVRAMWriteData(eDxCAS_GROUPID_NA, CA_NVRAM_IRDCMD_POPUP_SEQNUM_OFFSET, CA_NVRAM_IRDCMD_POPUP_SEQNUM_SIZE, (HUINT8*)ucPopBuffer) != ERR_OK )
	{
		return ERR_FAIL;
	}
}
#endif

	return ERR_OK;
}


static HERROR local_irdCmd_ProcGetPincodeByCommand(TUnsignedInt8 *pxIrdCmd, TUnsignedInt8 *pszPincode, TUnsignedInt8 *pucLen)
{
	TUnsignedInt8	i = 0;
	TUnsignedInt8	*pucCmd;
	TUnsignedInt8	ucPinLen = 0;

	if ( pxIrdCmd == NULL )
	{
		HxLOG_Info("Source Null!\n");
		return ERR_FAIL;
	}

	if ( (pszPincode == NULL) || (pucLen == NULL) )
	{
		HxLOG_Info("Target Null!\n");
		return ERR_FAIL;
	}

	pucCmd = pxIrdCmd;

	// Skip Header Field : EMM Header, Length, Sequence Number, Command ID, Operation ID
	pucCmd += 8;

	ucPinLen = *(pucCmd++);
	if ( ucPinLen > NA_IRDCMD_MAX_PINCODE_LEN )
		return ERR_FAIL;

	HxLOG_Print("Pincode Length : [%d]\n", ucPinLen);
	for (i=0 ; i<ucPinLen ; i++)
	{
		pszPincode[i] = *(pucCmd++);
	}

	pszPincode[i] = 0x00;
	(*pucLen) = i;

	HxLOG_Print("New Pincode : %s\n", pszPincode);
	return ERR_OK;
}


static HERROR local_irdCmd_ProcGetServiceHandleByCommand(TUnsignedInt8 *pxIrdCmd, TUnsignedInt32 *pSvcHandle)
{
	TUnsignedInt8		*pucCmd;
	DxDeliveryType_e 	eOrgDeliType;

	TUnsignedInt16		nNetId;
	TUnsignedInt16		nNetIdx;

	TUnsignedInt32		hService;
	DbSvc_TripleId_t		stTripleId;

	TSignedInt32		i;
	TSignedInt32		nPreTsNum;
	DbSvc_TsCondition_t	stTsCond;
	DbSvc_TsInfo_t		*pstTsArray = NULL;
	TSignedInt32		nPreSvcNum;
	DbSvc_Condition_t 	stSvcCond;
	Handle_t			*phPreSvcHandleArray = NULL;

#if defined(CONFIG_MW_CH_SATELLITE)
	eOrgDeliType = eDxDELIVERY_TYPE_SAT;
#elif defined(CONFIG_MW_CH_TERRESTRIAL)
	eOrgDeliType = eDxDELIVERY_TYPE_TER;
#elif defined(CONFIG_MW_CH_CABLE)
	eOrgDeliType = eDxDELIVERY_TYPE_CAB;
#else
	eOrgDeliType = eDxDELIVERY_TYPE_ALL;
#endif

	if ( pxIrdCmd == NULL )
	{
		HxLOG_Info("Source Null!\n");
		return ERR_FAIL;
	}

	if ( pSvcHandle == NULL)
	{
		HxLOG_Info("Target Null!\n");
		return ERR_FAIL;
	}

	if ( local_irdCmd_IsValidCommand(pxIrdCmd) != ERR_OK )
	{
		HxLOG_Info("Invalid Check Sum, Maybe The IRD Command Data is Broken!!!! Please Check the Data.\n");
		return ERR_FAIL;
	}

	// Skip Header Field : EMM Header, Length, Sequence Number, Command ID, Operation ID
	pucCmd = pxIrdCmd + 8;

	nNetId				= (TUnsignedInt16)(*(pucCmd) << 8 | *(pucCmd+1));	// network_id
	stTripleId.usTsId	= (TUnsignedInt16)(*(pucCmd+2) << 8 | *(pucCmd+3));	// ts_id
	stTripleId.usSvcId	= (TUnsignedInt16)(*(pucCmd+4) << 8 | *(pucCmd+5));	// svc_id

	HxLOG_Print("TripleId : NETID : 0x%x, TSID : 0x%x, SID : 0x%x\n", nNetId, stTripleId.usTsId, stTripleId.usSvcId);

#if (0)
	if (DB_SVC_FindServiceHandleByTripleId(eChType, stTripleId, (unsigned int*)&hService) != ERR_OK )
	{
		HxLOG_Info("error in DB_SVC_FindServiceHandleByTripleId\n");
		return ERR_FAIL;
	}
#else
	if ( DB_SVC_FindNetworkIndexById(eOrgDeliType, nNetId, &nNetIdx) != ERR_OK )
	{
		HxLOG_Info("error in DB_SVC_FindNetworkIndexById\n");
		return ERR_FAIL;
	}

	DB_SVC_InitTsFindCondition(&stTsCond);
	stTsCond.nDeliType 	= eOrgDeliType;
	stTsCond.nNetIdx 	= nNetIdx;
	stTsCond.nTsId 		= stTripleId.usTsId;

	if ( (DB_SVC_FindTsInfoList(&stTsCond, (HINT32 *)&nPreTsNum, &pstTsArray) != ERR_OK) || (nPreTsNum <= 0) )
	{
		HxLOG_Info("error in DB_SVC_FindTsInfoList\n");
                DB_SVC_FreeTsInfoList (pstTsArray);
		return ERR_FAIL;
	}

	for (i=0; i<nPreTsNum; i++)
	{
		stTripleId.usOnId 		= pstTsArray[i].usOnId;

		DB_SVC_InitSvcFindCondition(&stSvcCond);
		stSvcCond.nOrgDeliType	= (TSignedInt32)eOrgDeliType;
		stSvcCond.nOnId			= (TSignedInt32)stTripleId.usOnId;
		stSvcCond.nTsId 		= (TSignedInt32)stTripleId.usTsId;
		stSvcCond.nSvcId		= (TSignedInt32)stTripleId.usSvcId;

		phPreSvcHandleArray		= NULL;
		if ( (DB_SVC_FindSvcHandleList(&stSvcCond, (HINT32 *)&nPreSvcNum, &phPreSvcHandleArray) != ERR_OK) || (nPreSvcNum <= 0) )
		{
			HxLOG_Info("error in DB_SVC_FindSvcHandleList\n");
     	                if(phPreSvcHandleArray != NULL)
			{
				DB_SVC_FreeServiceList(phPreSvcHandleArray);
			}
			continue;
		}

		hService = phPreSvcHandleArray[0];
		DB_SVC_FreeServiceList(phPreSvcHandleArray);
		break;
	}
	DB_SVC_FreeTsInfoList(pstTsArray);
#endif

	if (i < nPreTsNum)
	{
		HxLOG_Print("Servcie Handle : 0x%x\n", (unsigned int)hService);
		*pSvcHandle = hService;
		return ERR_OK;
	}
	return ERR_FAIL;
}


static HERROR local_irdCmd_ProcGetUsageIdByCommand(TUnsignedInt8 *pxIrdCmd, TUnsignedInt8 *pszUsageId)
{
	TUnsignedInt8 *pucCmd;

	if ( pxIrdCmd == NULL )
	{
		HxLOG_Info("Source Null!\n");
		return ERR_FAIL;
	}

	if ( pszUsageId == NULL)
	{
		HxLOG_Info("Target Null!\n");
		return ERR_FAIL;
	}

	pucCmd = pxIrdCmd;

	// Skip Header Field : EMM Header, Length, Sequence Number, Command ID, Operation ID
	pucCmd += 8;
	*pszUsageId = *(pucCmd);

	HxLOG_Print("New Usage ID : 0x%x\n", *pszUsageId);
	return ERR_OK;
}


#if defined(CONFIG_MW_CAS_NAGRA_OP_SES)
static HERROR local_irdCmd_ProcDisplayPorfileByCommand(TUnsignedInt8 *pxIrdCmd)
{
	TUnsignedInt8 			i, j;
	TUnsignedInt8 			*pucCmd;
	TUnsignedInt8 			ucNumOfProfile;
	TUnsignedInt8 			ucProfileId;
	TUnsignedInt8 			ucNumOfPeriods;

	TBoolean				bSet[2] = {FALSE, FALSE};
	NA_IRDCMD_PROFILE_t 	stDisplayProfile[2];

	if ( pxIrdCmd == NULL )
	{
		HxLOG_Info("Source Null!\n");
		return ERR_FAIL;
	}

#if (0)
	if ( local_irdCmd_IsValidCommand(pxIrdCmd) != ERR_OK)
	{
		HxLOG_Info("Invalid Check Sum, Maybe The IRD Command Data is Broken!!!! Please Check the Data.\n");
		return ERR_FAIL;
	}
#endif

	for (i=0; i<2; i++)
	{
		HxSTD_memset(&stDisplayProfile[i], 0x00, sizeof(NA_IRDCMD_PROFILE_t));
		if (svc_cas_DevPortNVRAMReadData(eDxCAS_GROUPID_NA, (CA_NVRAM_IRDCMD_PROFILE_OFFESET + (i * 21)),
												sizeof(NA_IRDCMD_PROFILE_t),
												(TUnsignedInt8*)&stDisplayProfile[i]) != ERR_OK)
		{
			HxLOG_Info("error in svc_cas_DevPortNVRAMReadData\n");
			return ERR_FAIL;
		}
	}

	pucCmd = pxIrdCmd;

	// Skip Header Field : EMM Header, Length, Sequence Number, Command ID, Operation ID
	pucCmd += 8;
	ucNumOfProfile = *(pucCmd++);

	HxLOG_Print("\t\n ======== Display profile info start ========\n");
	for (i=0; i<ucNumOfProfile; i++)
	{
		ucProfileId = ((*(pucCmd) >> 4) & 0xF);
		ucNumOfPeriods = (*(pucCmd++) & 0xF);

		HxLOG_Print("\t\n %d) ProfileId =%x, NumOfPeriod = %d\n", i, ucProfileId, ucNumOfPeriods);
		if ( (ucProfileId >= eProfileSmartcard) && (ucProfileId <= eProfileProducts) )
		{
			bSet[ucProfileId-1] = TRUE;
			stDisplayProfile[ucProfileId-1].ucProfieType = ucProfileId;

			for (j=0; j<ucNumOfPeriods; j++)
			{
				if (j < NA_IRDCMD_MAX_PROFILE)
				{
					stDisplayProfile[ucProfileId-1].ucPeroid[j] = j+1;
					stDisplayProfile[ucProfileId-1].ucBeginOfPeroid[j] = *(pucCmd++);
					stDisplayProfile[ucProfileId-1].ucEndOfPeroid[j] = *(pucCmd++);
					stDisplayProfile[ucProfileId-1].ucFrequency[j] = *(pucCmd++);
					HxLOG_Print("\t\t %d) ucPeroid =%02d, BeginOfPeroid = %02d, EndOfPeroid = %02d, Frequency = %02d\n", j,
						stDisplayProfile[ucProfileId-1].ucPeroid[j], stDisplayProfile[ucProfileId-1].ucBeginOfPeroid[j], stDisplayProfile[ucProfileId-1].ucEndOfPeroid[j], stDisplayProfile[ucProfileId-1].ucFrequency[j]);
				}
				else
				{
					pucCmd += 3;
				}
			}
		}
		else
		{
			pucCmd += (ucNumOfPeriods * 3);
		}
	}
	HxLOG_Print("\t\n ======== Display profile info end ========\n");

	for (i=0; i<2; i++)
	{
		if (bSet[i] == TRUE)
		{
			if (svc_cas_DevPortNVRAMWriteData(eDxCAS_GROUPID_NA, (CA_NVRAM_IRDCMD_PROFILE_OFFESET + (i * 21)),
											sizeof(NA_IRDCMD_PROFILE_t),
											(TUnsignedInt8*)&stDisplayProfile[i]) != ERR_OK)
			{
				HxLOG_Info("error in svc_cas_DevPortNVRAMWriteData\n");
				return ERR_FAIL;
			}
		}
	}
	return ERR_OK;
}


static HERROR local_irdCmd_ProcMessageByCommand(TUnsignedInt8 *pxIrdCmd)
{
	TUnsignedInt8 i;
	TUnsignedInt8 *pucCmd;
	TUnsignedInt8 ucNumOfMsg;
	TUnsignedInt8 ucMsgType;
	TUnsignedInt8 ucMsgLen;

	TBoolean	bSet[2] = {FALSE, FALSE};
	NA_IRDCMD_MESSAGE_t	 	stMsgInfo[2];

	if ( pxIrdCmd == NULL )
	{
		HxLOG_Info("Source Null!\n");
		return ERR_FAIL;
	}

#if (0)
	if ( local_irdCmd_IsValidCommand(pxIrdCmd) != ERR_OK)
	{

		HxLOG_Info("Invalid Check Sum, Maybe The IRD Command Data is Broken!!!! Please Check the Data.\n");
		return ERR_FAIL;
	}
#endif

	for (i=0; i<2; i++)
	{
		HxSTD_memset(&stMsgInfo[i], 0x00, sizeof(NA_IRDCMD_MESSAGE_t));
		if (svc_cas_DevPortNVRAMReadData(eDxCAS_GROUPID_NA, (CA_NVRAM_IRDCMD_MESSAGE_OFFSET + (i * 51)),
											sizeof(NA_IRDCMD_MESSAGE_t),
											(TUnsignedInt8*)&stMsgInfo[i]) != ERR_OK)
		{
			HxLOG_Info("error in svc_cas_DevPortNVRAMReadData\n");
			return ERR_FAIL;
		}
	}

	pucCmd = pxIrdCmd;

	// Skip Header Field : EMM Header, Length, Sequence Number, Command ID, Operation ID
	pucCmd += 8;
	ucNumOfMsg = *(pucCmd++);

	HxLOG_Print("\t\n ======== Message info start ========\n");
	for (i=0; i<ucNumOfMsg; i++)
	{
		ucMsgType = ((*(pucCmd) >> 5) & 0x7);
		ucMsgLen = (*(pucCmd++) & 0x1F);

		HxLOG_Print("\t\n %d) Msg Type =%x, Msg Len = %d\n", i, ucMsgType, ucMsgLen);
		if ( (ucMsgType >= eMsgHotlineNum) && (ucMsgType <= eMsgCallCharge) )
		{
			bSet[ucMsgType-1] = TRUE;
			stMsgInfo[ucMsgType-1].ucMsgType = ucMsgType;

			HxSTD_memset(stMsgInfo[ucMsgType-1].ucMsgContent, 0x00, NA_IRDCMD_MAX_MSG_LEN);

			if (ucMsgLen < NA_IRDCMD_MAX_MSG_LEN)
				HxSTD_memcpy(stMsgInfo[ucMsgType-1].ucMsgContent, pucCmd, ucMsgLen);
			else
				HxSTD_memcpy(stMsgInfo[ucMsgType-1].ucMsgContent, pucCmd, NA_IRDCMD_MAX_MSG_LEN-1);

			pucCmd += ucMsgLen;
		}
                if ((ucMsgType-1) < 2)
		{
		        HxLOG_Print("\t\n %s\n", stMsgInfo[ucMsgType-1].ucMsgContent);
                }
	}
	HxLOG_Print("\t\n ======== Message info end ========\n");

	for (i=0; i<2; i++)
	{
		if (bSet[i] == TRUE)
		{
			if (svc_cas_DevPortNVRAMWriteData(eDxCAS_GROUPID_NA, (CA_NVRAM_IRDCMD_MESSAGE_OFFSET + (i * 51)),
											sizeof(NA_IRDCMD_MESSAGE_t),
											(TUnsignedInt8*)&stMsgInfo[i]) != ERR_OK)
			{
				return ERR_FAIL;
			}
		}
	}
	return ERR_OK;
}
#endif


#if defined(CONFIG_MW_CAS_NAGRA_IRDCMD_MAIL)
static HERROR local_irdCmd_ProcBuildMailMsg(TUnsignedInt8 *pxIrdCmd, TUnsignedInt16 *pusMailId, TUnsignedInt16 *pusDeletedMailId)
{
	HERROR		nRes;
	TBoolean	bCompleted = FALSE;

	if ( pxIrdCmd == NULL || pusMailId == NULL || pusDeletedMailId == NULL)
	{
		HxLOG_Error("pIrdCmd == NULL || pnMailId == NULL || pnDeletedMailId == NULL\n");
		return ERR_FAIL;
	}

	nRes = CA_NA_IRDCMD_MAIL_MakeMailMessage(pxIrdCmd, pusMailId, &bCompleted, pusDeletedMailId);
	if ( nRes == ERR_OK)
	{
		if(bCompleted == TRUE)
		{
			HxLOG_Print("Mail id [%d] : Build Mail Message Completed!!!!\n", *pusMailId);
			return ERR_OK;
		}
		else
		{
			HxLOG_Print("Not completed!!\n");
		}
	}
	else
	{
		HxLOG_Error("ERROR!!\n");
	}

	return ERR_FAIL;
}
#endif


#if defined(CONFIG_MW_CAS_NAGRA_IRDCMD_POPUP)
static HERROR local_irdCmd_ProcBuildPopupMsgMsg(TUnsignedInt8 *pxIrdCmd, TUnsignedInt16 *pusPopupId)
{
	HERROR		nRes;
	TBoolean	bCompleted = FALSE;

	if ( pxIrdCmd == NULL || pusPopupId == NULL )
	{
		HxLOG_Error("pIrdCmd == NULL || pusPopupId == NULL \n");
		return ERR_FAIL;
	}

	nRes = CA_NA_IRDCMD_POPUP_MakePopupMessage(pxIrdCmd, pusPopupId, &bCompleted);
	if ( nRes == ERR_OK && bCompleted == TRUE )
	{
		if(bCompleted == TRUE)
		{
			HxLOG_Print("Popup id [%d] : Build Popup Message Completed!!!!\n", *pusPopupId);
			return ERR_OK;
		}
		else
		{
			HxLOG_Print("Not completed!!\n");
		}
	}
	else
	{
		HxLOG_Error("ERROR!!\n");
	}

	return ERR_FAIL;
}


static HERROR local_irdCmd_ProcRemovePopupMsg(void)
{
	HERROR	nRes;

	nRes = CA_NA_IRDCMD_POPUP_RemovePopupMessage();
	if ( nRes == ERR_OK )
	{
		HxLOG_Print("Remove Popup Message Completed!!!!\n");
		return ERR_OK;
	}

	HxLOG_Error("MW_CA_NA_POPUP_RemovePopupMessage() Fail!!\n");
	return ERR_FAIL;
}
#endif


#if defined(CONFIG_MW_CAS_NAGRA_IRDCMD_EMMWAKEUP)
static HERROR local_irdCmd_ProcDisableEmmWakeUp(void)
{
	NA_EMM_WAKEUP_INFO stWakeup;

	// 이전 Enable EmmWakeup Parameter 삭제
	HxSTD_memset(&stWakeup, 0x00, sizeof(NA_EMM_WAKEUP_INFO));

	stWakeup.ucReceived = 1;
	if ( svc_cas_DevPortNVRAMWriteData(eDxCAS_GROUPID_NA, CA_NVRAM_IRDCMD_EMMWAKEUP_OFFSET, sizeof(NA_EMM_WAKEUP_INFO), (HUINT8 *)&stWakeup) != ERR_OK )
	{
		HxLOG_Info("error in svc_cas_DevPortNVRAMWriteData...\n");
		return ERR_FAIL;
	}
	return ERR_OK;
}


static HERROR local_irdCmd_ProcEnableEmmWakeUp(TUnsignedInt8 *pxIrdCmd)
{
	TUnsignedInt8	*pucCmd;

	NA_EMM_WAKEUP_INFO 		stWakeup;
	NA_WAKING_LOOP_SLOT 	stWakingLoopSlot;

	if ( pxIrdCmd == NULL )
	{
		HxLOG_Info("pIrdCmd == NULL\n");
		return ERR_FAIL;
	}

	pucCmd = pxIrdCmd;

	// Skip Header Field : EMM Header, Length, Sequence Number, Command ID, Operation ID
	pucCmd += 8;

	HxSTD_memset(&stWakeup, 0x00, sizeof(NA_EMM_WAKEUP_INFO));
	HxSTD_memset(&stWakingLoopSlot, 0x00, sizeof(NA_WAKING_LOOP_SLOT));

	// reserved 8
	HxLOG_Print("reserved - pucCmd : 0x%x  \n", pucCmd[0]);
	pucCmd += 1;

	// reserved 5, network_flag 1, transport_flag 1, service_flag 1 = 8
	HxLOG_Print("reserved5, flag1, flag1, flag1 - pucCmd : 0x%x \n", pucCmd[0]);
	stWakingLoopSlot.ucReserved 	= (pucCmd[0] >> 3) & 0x1F;
	stWakingLoopSlot.ucNetFlag 		= (pucCmd[0] >> 2) & 0x01;
	stWakingLoopSlot.ucTsFlag		= (pucCmd[0] >> 1) & 0x01;
	stWakingLoopSlot.ucSvcFlag 		= (pucCmd[0] ) & 0x01;
	pucCmd += 1;

	// network_id 16
	HxLOG_Print("\t\t network_flag:%d\n", stWakingLoopSlot.ucNetFlag);
	if ( stWakingLoopSlot.ucNetFlag )
	{
		HxLOG_Print("network_id -pucCmd : 0x%x 0x%x \n", pucCmd[0], pucCmd[1]);

		stWakingLoopSlot.usNetId = get16bit(&pucCmd[0]);
		pucCmd += 2;
		HxLOG_Print("\t\t\t network_id:0x%x\n", stWakingLoopSlot.usNetId);
	}

	// transport_stream_id 16
	HxLOG_Print("\t\t transport_flag:%d\n", stWakingLoopSlot.ucTsFlag);
	if ( stWakingLoopSlot.ucTsFlag )
	{
		HxLOG_Print("transport_stream_id - pucCmd : 0x%x 0x%x \n", pucCmd[0], pucCmd[1]);

		stWakingLoopSlot.usTsId = get16bit(&pucCmd[0]);
		pucCmd += 2;
		HxLOG_Print("\t\t\t transport_stream_id:0x%x\n", stWakingLoopSlot.usTsId);
	}

	// service_id 16
	HxLOG_Print("\t\t service_flag:%d\n", stWakingLoopSlot.ucSvcFlag);
	if ( stWakingLoopSlot.ucSvcFlag )
	{
		HxLOG_Print("service_flag - pucCmd : 0x%x 0x%x \n", pucCmd[0], pucCmd[1]);

		stWakingLoopSlot.usSvcId = get16bit(&pucCmd[0]);
		pucCmd += 2;
		HxLOG_Print("\t\t\t service_id:0x%X\r\n", stWakingLoopSlot.usSvcId);
	}

	// waking_duration 16
	HxLOG_Print("waking_duration - pucCmd : 0x%x 0x%x \n", pucCmd[0], pucCmd[1]);
	stWakingLoopSlot.usWakingDur = get16bit(&pucCmd[0]);
	pucCmd += 2;
	HxLOG_Print("\t\t waking_duration:%d\n", stWakingLoopSlot.usWakingDur);

	// reserved 8
	HxLOG_Print("reserved - pucCmd : 0x%x  \n", pucCmd[0]);
	pucCmd += 1;

	// sleeping_duration 16
	HxLOG_Print("sleeping_duration - pucCmd : 0x%x 0x%x \n", pucCmd[0], pucCmd[1]);
	stWakeup.usSleepingDur = get16bit(&pucCmd[0]);
	pucCmd += 2;
	HxLOG_Print("\t\t sleeping_duration:%d \n", stWakeup.usSleepingDur);


	if (stWakingLoopSlot.usWakingDur != 0 && stWakeup.usSleepingDur != 0)
	{
		stWakeup.ucWakingLoopCnt = 1;
		HxLOG_Print("\t\t ucWakingLoopCnt:%d \n", stWakeup.ucWakingLoopCnt);
	}

	// CakSpeEmmWak1.0.3 대비 CakSpeEmmWak2.0.1 은 구조가 바뀌었다.
	// 일단 파싱부분만 변경한다. 다음번에는 구조체 자체도 변경하자.
	VK_memcpy(&stWakeup.stWakingSlot[0], &stWakingLoopSlot, sizeof(NA_WAKING_LOOP_SLOT));

	stWakeup.ucReceived = 1;
	// EEPROM에 새로운 IRD Command 저장
	if ( svc_cas_DevPortNVRAMWriteData(eDxCAS_GROUPID_NA, CA_NVRAM_IRDCMD_EMMWAKEUP_OFFSET, sizeof(NA_EMM_WAKEUP_INFO), (HUINT8 *)&stWakeup) != ERR_OK )
	{
		HxLOG_Info("error in svc_cas_DevPortNVRAMWriteData...\n");
		return ERR_FAIL;
	}
	return ERR_OK;
}


static HERROR local_irdCmd_EMM_GetWakeupTime(UNIXTIME *pulTime)
{
	UNIXTIME ulLocalTime = 0, ulWakingTime = 0xFFFFFFFF, ulWakeupTime = 0xFFFFFFFF;
#if 0
	HxDATETIME_t 			stDateTime;
#endif
	NA_EMM_WAKEUP_INFO 		stWakeup;
	HUINT32				ulCurTime = 0;
#if defined(CONFIG_DEBUG)
	HxDATETIME_t			stCurtTime;
#endif

	if ( pulTime == NULL )
		return ERR_FAIL;

	if ( VK_CLOCK_GetTime( (unsigned long *)&ulLocalTime ) != ERR_OK )
	{
		HxLOG_Info("error in VK_CLOCK_GetTime...\n");
		return ERR_FAIL;
	}
#if defined(CONFIG_DEBUG)
	if(HLIB_DATETIME_ConvertUnixTimeToDateTime(ulLocalTime, &stCurtTime) == ERR_FAIL)
	{
		HxLOG_Error(" Fail to Convert Current Time.\n");
		return ERR_FAIL;
	}
	HxLOG_Error("Current Time(%d) : %d-%d-%d %d:%d:%d \n", ulLocalTime,
				stCurtTime.stDate.usYear, stCurtTime.stDate.ucMonth, stCurtTime.stDate.ucDay,
				stCurtTime.stTime.ucHour, stCurtTime.stTime.ucMinute, stCurtTime.stTime.ucSecond);
#endif

	HxSTD_memset(&stWakeup, 0x00, sizeof(NA_EMM_WAKEUP_INFO));
	if ( svc_cas_DevPortNVRAMReadData(eDxCAS_GROUPID_NA, CA_NVRAM_IRDCMD_EMMWAKEUP_OFFSET, sizeof(NA_EMM_WAKEUP_INFO), (HUINT8 *)&stWakeup) != ERR_OK )
	{
		HxLOG_Info("error in svc_cas_DevPortNVRAMReadData...\n");
		return ERR_FAIL;
	}

	// Emm wake up time이 없는 경우 처리 ...
	if ( stWakeup.ucReceived != 1 )
	{
		HxLOG_Error(" There is no Emm wakeup time!!!\n");
		*pulTime = 0;
		return ERR_FAIL;
	}

	// sleeping duration - not used
	if ( stWakeup.ucSleepingDurFlag )
	{
		ulWakingTime = (ulLocalTime + (stWakeup.usSleepingDur * 60));
	}

	// wake up time
#if 0
	if ( stWakeup.ucWakeupTimeFlag )
	{
		stDateTime.stDate.usYear = stWakeup.ucWakeupYear + 1900;
		stDateTime.stDate.ucMonth = stWakeup.ucWakeupMonth;
		stDateTime.stDate.ucDay = stWakeup.ucWakeupDay;

		stDateTime.stTime.ucHour = stWakeup.ucWakeupHour;
		stDateTime.stTime.ucMinute = stWakeup.ucWakeupMin;
		stDateTime.stTime.ucSecond = 0;

		if ( HLIB_DATETIME_ConvertDateTimeToUnixTimeWithOutOffset( &stDateTime, &ulWakeupTime ) == ERR_OK )
		{
			if ( ulWakeupTime <= ulLocalTime )
				ulWakeupTime = 0xFFFFFFFF;
		}
		else
		{
			HxLOG_Info("error in HLIB_DATETIME_ConvertDateTimeToUnixTimeWithOutOffset...\n");
			ulWakeupTime = 0xFFFFFFFF;
		}
	}
#else
	// 지금까지는 wakeup Time이 스펙으로 들어왔지만, new spec에서는 sleeping duration만 있다.
	// 따라서 현재시간, 즉 standby 진입시의 현재시간에 sleeping duration 을 더한 시간을 리턴한다.
	if (stWakeup.usSleepingDur != 0)
	{

#if 1 // 이 값을 받아 사용하는 곳의 시간단위에 맞춤
		// wakeup time = sleepingDur * 60 (sec) 한 시간
		ulWakeupTime = stWakeup.usSleepingDur * 60;
		HxLOG_Error("ulWakeupTime=%d sec \n", ulWakeupTime);
#else
		// wakeup time = 현재시간 + sleepingDur * 60 (min) 한 시간
		ulWakeupTime = (ulLocalTime + (stWakeup.usSleepingDur * 60));
#if defined(CONFIG_DEBUG)
		if(HLIB_DATETIME_ConvertUnixTimeToDateTime(ulWakeupTime, &stDateTime) == ERR_FAIL)
		{
			HxLOG_Error(" Fail to Convert Current Time.\n");
			return ERR_FAIL;
		}
		HxLOG_Error("ulWakeupTime(%d) : %d-%d-%d %d:%d:%d \n", ulWakeupTime,
					stDateTime.stDate.usYear, stDateTime.stDate.ucMonth, stDateTime.stDate.ucDay,
					stDateTime.stTime.ucHour, stDateTime.stTime.ucMinute, stDateTime.stTime.ucSecond);
#endif
#endif
	}
#endif

	if ( (ulWakeupTime == 0xFFFFFFFF) && (ulWakingTime == 0xFFFFFFFF) )
	{
		HxLOG_Info("ulWakingTime and ulWakeupTime don't exist\n");
		return ERR_FAIL;
	}

	if ( ulWakingTime <= ulWakeupTime )
	{
		*pulTime = ulWakingTime;
	}
	else
	{	// 여기로만 들어온다.
		*pulTime = ulWakeupTime;
	}
	return ERR_OK;
}


static HERROR local_irdCmd_EMM_ClearTable(void)
{
	NA_EMM_WAKEUP_INFO stWakeup;

	// 이전 Enable EmmWakeup Parameter 삭제
	HxSTD_memset(&stWakeup, 0x00, sizeof(NA_EMM_WAKEUP_INFO));
	if ( svc_cas_DevPortNVRAMWriteData(eDxCAS_GROUPID_NA, CA_NVRAM_IRDCMD_EMMWAKEUP_OFFSET, sizeof(NA_EMM_WAKEUP_INFO), (HUINT8 *)&stWakeup) != ERR_OK )
	{
		HxLOG_Info("error in svc_cas_DevPortNVRAMWriteData...\n");
		return ERR_FAIL;
	}
	return ERR_OK;
}


static HERROR local_irdCmd_EMM_InitWakingTable(void)
{
	s_ucCurrWakingLoopCnt = 0;
	HxSTD_memset(&s_stEmmWakeupInfo, 0x00, sizeof(NA_EMM_WAKEUP_INFO));

	if ( svc_cas_DevPortNVRAMReadData(eDxCAS_GROUPID_NA, CA_NVRAM_IRDCMD_EMMWAKEUP_OFFSET, sizeof(NA_EMM_WAKEUP_INFO), (HUINT8 *)&s_stEmmWakeupInfo) != ERR_OK )
	{
		HxLOG_Info("error in svc_cas_DevPortNVRAMReadData...\n");
		return ERR_FAIL;
	}
	return ERR_OK;
}


static HERROR local_irdCmd_EMM_HasMoreNextWaking(TBoolean *pbNext)
{
	NA_EMM_WAKEUP_INFO stWakeup;

	if ( pbNext == NULL )
	{
		HxLOG_Info("target is null..\n");
		return ERR_FAIL;
	}

	*pbNext = TRUE;
	HxSTD_memset(&stWakeup, 0x00, sizeof(NA_EMM_WAKEUP_INFO));
	HxSTD_memcpy(&stWakeup, &s_stEmmWakeupInfo, sizeof(NA_EMM_WAKEUP_INFO));

	// Emm wake up time이 없는 경우 처리 ...
	// Default wake up 처리
	if ( stWakeup.ucReceived != 1 )
	{
		if ( s_ucCurrWakingLoopCnt == 0 )
			s_ucCurrWakingLoopCnt++;
		else
			*pbNext = FALSE;
		return ERR_OK;
	}

	if ( stWakeup.ucWakingLoopCnt == s_ucCurrWakingLoopCnt )
	{
		*pbNext = FALSE;
	}
	return ERR_OK;
}


static HERROR local_irdCmd_EMM_GetWakingTuneInfo(TUnsignedInt8 *pucNetworkIdFlag, TUnsignedInt16 *pusNetworkId,
															TUnsignedInt8 *pucTsIdFlag, TUnsignedInt16 *pusTsId,
															TUnsignedInt8 *pucServiceIdFlag, TUnsignedInt16 *pusServiceId)
{
	TUnsignedInt8 ucCurrentIndex = 0;
	NA_EMM_WAKEUP_INFO stWakeup;

	if ( (pucNetworkIdFlag == NULL) || (pusNetworkId == NULL)
		|| (pucTsIdFlag == NULL) || (pusTsId == NULL)
		|| (pucServiceIdFlag == NULL) || (pusServiceId == NULL) )
	{
		HxLOG_Info("Target are null..\n");
		return ERR_FAIL;
	}

	HxSTD_memset(&stWakeup, 0x00, sizeof(NA_EMM_WAKEUP_INFO));
	HxSTD_memcpy(&stWakeup, &s_stEmmWakeupInfo, sizeof(NA_EMM_WAKEUP_INFO));

	// Emm wake up time이 없는 경우 처리 ...
	if ( stWakeup.ucReceived != 1 )
	{
		*pucNetworkIdFlag = 0;
		*pucTsIdFlag = 0;
		*pucServiceIdFlag = 0;
		return ERR_OK;
	}

	// waking_loop_count
	if ( stWakeup.ucWakingLoopCnt > 0 )
	{
		ucCurrentIndex = s_ucCurrWakingLoopCnt;
		if ( ucCurrentIndex >= MAX_WAKING_SLOT ) ucCurrentIndex = 0;

		*pucNetworkIdFlag = stWakeup.stWakingSlot[ucCurrentIndex].ucNetFlag;
		*pusNetworkId = stWakeup.stWakingSlot[ucCurrentIndex].usNetId;

		*pucTsIdFlag = stWakeup.stWakingSlot[ucCurrentIndex].ucTsFlag;
		*pusTsId = stWakeup.stWakingSlot[ucCurrentIndex].usTsId;

		*pucServiceIdFlag = stWakeup.stWakingSlot[ucCurrentIndex].ucSvcFlag;
		*pusServiceId = stWakeup.stWakingSlot[ucCurrentIndex].usSvcId;

		HxLOG_Print("NetworkId(%d, 0x%x), TsId(%d, 0x%x), ServiceId(%d, 0x%x)\n",
									*pucNetworkIdFlag, *pusNetworkId, *pucTsIdFlag, *pusTsId, *pucServiceIdFlag, *pusServiceId);
	}
	else
	{
		*pucNetworkIdFlag = 0;
		*pucTsIdFlag = 0;
		*pucServiceIdFlag = 0;
		HxLOG_Print("Triple Id doesn't exist...\n");
	}
	return ERR_OK;
}


static HERROR local_irdCmd_EMM_GetWakingTimer(UNIXTIME *pulTime)
{
	TUnsignedInt8 	ucCurrentIndex = 0;
	TUnsignedInt32 	ulWakingDuration = 0, ulWakeupDuration = 0;
	UNIXTIME 		ulLocalTime = 0, ulWakeupTime = 0;

	HxDATETIME_t 	stDateTime;
	NA_EMM_WAKEUP_INFO stWakeup;

	if ( pulTime == NULL )
	{
		HxLOG_Error("target is null..\n");
		return ERR_FAIL;
	}

	*pulTime = DEFALT_WAKING_DURATION;
	HxSTD_memset(&stWakeup, 0x00, sizeof(NA_EMM_WAKEUP_INFO));
	HxSTD_memcpy(&stWakeup, &s_stEmmWakeupInfo, sizeof(NA_EMM_WAKEUP_INFO));

	// Emm wake up time이 없는 경우 처리 ...
	if ( stWakeup.ucReceived != 1 )
	{
		HxLOG_Error("stWakeup.ucReceived != 1..\n");
		*pulTime = 0;

		if ( svc_cas_DevPortNVRAMReadData(eDxCAS_GROUPID_NA, CA_NVRAM_IRDCMD_EMMWAKEUP_OFFSET, sizeof(NA_EMM_WAKEUP_INFO), (HUINT8 *)&stWakeup) != ERR_OK )
		{
			HxLOG_Error("error in svc_cas_DevPortNVRAMReadData...\n");
			return ERR_OK;
		}
	}

	// waking_loop_count
	if ( stWakeup.ucWakingLoopCnt > 0 )
	{
		ucCurrentIndex = s_ucCurrWakingLoopCnt;
		if ( ucCurrentIndex >= MAX_WAKING_SLOT )
		{
			HxLOG_Error("return ERR_FAIL (ucCurrentIndex=%d)\n", ucCurrentIndex);
			return ERR_FAIL;
		}

		ulWakingDuration = stWakeup.stWakingSlot[ucCurrentIndex].usWakingDur * 60;

		ulWakeupDuration = 0xFFFFFFFF;

		#if 0
		if ( stWakeup.ucWakeupTimeFlag )
		{
			if ( VK_CLOCK_GetTime( (unsigned long *)&ulLocalTime ) != ERR_OK )
			{
				HxLOG_Info("error in VK_CLOCK_GetTime...\n");
				return ERR_OK;
			}

			stDateTime.stDate.usYear = stWakeup.ucWakeupYear + 1900;
			stDateTime.stDate.ucMonth = stWakeup.ucWakeupMonth;
			stDateTime.stDate.ucDay = stWakeup.ucWakeupDay;

			stDateTime.stTime.ucHour = stWakeup.ucWakeupHour;
			stDateTime.stTime.ucMinute = stWakeup.ucWakeupMin;
			stDateTime.stTime.ucSecond = 0;

			if ( HLIB_DATETIME_ConvertDateTimeToUnixTimeWithOutOffset( &stDateTime, &ulWakeupTime ) != ERR_OK )
			{
				HxLOG_Info("error in HLIB_DATETIME_ConvertDateTimeToUnixTimeWithOutOffset...\n");
				return ERR_OK;
			}

			if ( ulWakeupTime > ulLocalTime )
			{
				ulWakeupDuration = ulWakeupTime - ulLocalTime;
			}
		}
		#endif

		if ( ulWakingDuration < ulWakeupDuration )
		{	// 여기로만 들어온다.
			*pulTime = ulWakingDuration;
			HxLOG_Error("ulWakingDuration=%d sec \n", ulWakingDuration);
		}
		else
		{
			*pulTime = ulWakeupDuration;
		}

		s_ucCurrWakingLoopCnt++;
		if ( s_ucCurrWakingLoopCnt > MAX_WAKING_SLOT)
			s_ucCurrWakingLoopCnt = MAX_WAKING_SLOT;
		return ERR_OK;
	}

	// wakeup time만 존재하는 경우처리...
	// new spec에서는 이 조건절로 들어갈일 없다.
	if ( stWakeup.ucWakeupTimeFlag )
	{
		if ( VK_CLOCK_GetTime( (unsigned long *)&ulLocalTime ) != ERR_OK )
		{
			HxLOG_Info("error in VK_CLOCK_GetTime...\n");
			return ERR_OK;
		}

		stDateTime.stDate.usYear = stWakeup.ucWakeupYear + 1900;
		stDateTime.stDate.ucMonth = stWakeup.ucWakeupMonth;
		stDateTime.stDate.ucDay = stWakeup.ucWakeupDay;

		stDateTime.stTime.ucHour = stWakeup.ucWakeupHour;
		stDateTime.stTime.ucMinute = stWakeup.ucWakeupMin;
		stDateTime.stTime.ucSecond = 0;

		if ( HLIB_DATETIME_ConvertDateTimeToUnixTimeWithOutOffset( &stDateTime, &ulWakeupTime ) != ERR_OK )
		{
			HxLOG_Info("error in HLIB_DATETIME_ConvertDateTimeToUnixTimeWithOutOffset...\n");
			return ERR_OK;
		}

		if ( ulWakeupTime > ulLocalTime )
		{
			ulWakeupDuration = ulWakeupTime - ulLocalTime;
			*pulTime = ulWakeupDuration;
		}
	}
	return ERR_OK;
}
#endif


/**************************************************************************************************/
#define ________CA_CAK_IRDCMD_Public_Functions_Body________________________________________
/**************************************************************************************************/
HERROR CA_NA_IRDCMD_InitIrdVar(void)
{
	HxSTD_memset(&s_stIrdCmdSeqNum, 0, (sizeof(IRDCMD_SEQNUM_t) * (eIrdCmdNum-2)));
	return ERR_OK;
}


HERROR CA_NA_IRDCMD_ResetIRDCmdData(void)
{
	if ( local_irdCmd_ClearSequencenNumber() != ERR_OK )
	{
		HxLOG_Info("CA_NA_IRDCMD_ResetIRDCmdData() Fail!\n");
		return ERR_FAIL;
	}

#if defined(CONFIG_MW_CAS_NAGRA_IRDCMD_EMMWAKEUP)
	if ( local_irdCmd_EMM_ClearTable() != ERR_OK )
	{
		HxLOG_Info("local_irdCmd_EMM_ClearTable() Fail!\n");
		return ERR_FAIL;
	}
#endif

#if defined(CONFIG_MW_CAS_NAGRA_IRDCMD_MAIL)
	if ( CA_NA_IRDCMD_MAIL_ClearMailMessageHeader() != ERR_OK )
	{
		HxLOG_Info("CA_NA_IRDCMD_MAIL_ClearMailMessageHeader() Fail!\n");
		return ERR_FAIL;
	}
#endif

#if defined(CONFIG_MW_CAS_NAGRA_IRDCMD_POPUP)
	if ( CA_NA_IRDCMD_POPUP_DeletePopupMessage() != ERR_OK )
	{
		HxLOG_Info("CA_NA_IRDCMD_POPUP_DeletePopupMessage() Fail!\n");
		return ERR_FAIL;
	}
#endif
	return ERR_OK;
}


HERROR CA_NA_IRDCMD_IsValidIRDCmd(TUnsignedInt8 *pxIrdCmd)
{
	if ( pxIrdCmd == NULL )
	{
		HxLOG_Error("CA_NA_IRDCMD_IsValidIRDCmd() Fail!\n");
		return ERR_FAIL;
	}
	return local_irdCmd_IsValidCommand(pxIrdCmd);
}


HERROR CA_NA_IRDCMD_ProcessSequenceNumber(NA_IRDCMD_ID_t eIrdCmdId, TUnsignedInt8 *pxIrdCmd)
{
	IRDCMD_TYPE_t eIrdCmdType;

	if ( pxIrdCmd == NULL )
	{
		HxLOG_Error("pxIrdCmd == NULL\n");
		return ERR_FAIL;
	}

	if ( eIrdCmdId >= eComIDUnknown )
	{
		HxLOG_Error("eIrdCmdId >= eComIDUnknown\n");
		return ERR_FAIL;
	}

	if ( local_irdCmd_GetIRDCmdTypeByRDCmdId(eIrdCmdId, &eIrdCmdType) != ERR_OK )
	{
		HxLOG_Error("Command Id [0x%x] is Not Supported or Invalid!\n", eIrdCmdId);
		return ERR_FAIL;
	}

	if ( eIrdCmdType == eIrdCmdInvalid )
	{
		HxLOG_Error("Command Id [0x%x] is Not Supported or Invalid!\n", eIrdCmdId);
		return ERR_FAIL;
	}
	return local_irdCmd_ProcessSequenceNumber(eIrdCmdType, pxIrdCmd);
}


HERROR CA_NA_IRDCMD_GetIRDCommand(TUnsignedInt8 *pxIrdCmd, NA_IRDCMD_ID_t *pIrdCmdId, NA_IRDCMD_OPCODE_t *pOperationCode)
{
	TUnsignedInt8 *pucCmd;
	TUnsignedInt8 ucCmdLength = 0;
#if defined(CONFIG_MW_CAS_NAGRA_IRDCMD_ZONEID)
	TUnsignedInt8 aucZoneId[MAX_ZONE_ID_LENGTH];
	TUnsignedInt8 aucMyZoneId[MAX_ZONE_ID_LENGTH];
#endif

	if ( pxIrdCmd == NULL )
	{
		HxLOG_Info("pxIrdCmd is NULL\n");
		return ERR_FAIL;
	}

	if ( pIrdCmdId == NULL )
	{
		HxLOG_Info("pIrdCmdId is NULL\n");
		return ERR_FAIL;
	}

	if ( pOperationCode == NULL )
	{
		HxLOG_Info("pOperationCode is NULL\n");
		return ERR_FAIL;
	}

	pucCmd = pxIrdCmd;

	// Check EMM_COMMAND
	if ( *pucCmd != EMM_COMMAND_VALUE )
	{
		HxLOG_Info("Incorrect EMM Command Value : [0x%x]\n", *pucCmd);
		return ERR_FAIL;
	}

	HxLOG_Error("IRD CMD Data [0 ~ 7] : [0x%02x][0x%02x][0x%02x][0x%02x][0x%02x][0x%02x][0x%02x][0x%02x]\n",
			pucCmd[0], pucCmd[1], pucCmd[2], pucCmd[3], pucCmd[4], pucCmd[5], pucCmd[6], pucCmd[7]);

	// Check Length
	ucCmdLength = *(++pucCmd);
	HxLOG_Print("Command Length : [%d]\n", ucCmdLength);
	if ( ucCmdLength > MAX_IRD_COMMAND_LENGTH )
	{
		HxLOG_Info("Command Length too Big : [%d]\n", ucCmdLength);
		return ERR_FAIL;
	}

	HxLOG_Print("IRD Command Sequence Number : 0x%x\n", get32bit(pucCmd+1));
	pucCmd += 5;	// Skip Sequence Number
	switch (*pucCmd)
	{
		case eComIDResetPincode:
		case eComIDMail:
		case eComIDForceTune:
		case eComIDForceIdentification:
		case eComIDSetMacrovisionCps:
		case eComIDConfigureStb:
		case eComIDSetNetworkID:
		case eComIDSetPincode:
		case eComIDSetForceStandby:
		case eComIDSetForceDefSetting:
		case eComIDDisplayPopUp:
		case eComIDChangeUsageID:
		case eComIDLogicalDiskFormat:
#if defined(CONFIG_MW_CAS_NAGRA_IRDCMD_ZONEID)
		case eComIDZoneID:
#endif
		case eComIDEmmWakeUp:
		case eComIDRebootStb:
		case eComIDApsHdPlus:
			*pIrdCmdId = (NA_IRDCMD_ID_t)(*pucCmd);
			break;
		default:
			*pIrdCmdId = eComIDUnknown;
			break;
	}

	*pOperationCode = *(++pucCmd);
#if defined(CONFIG_MW_CAS_NAGRA_IRDCMD_ZONEID)
	/*
	*	Check zone ID, if there is zone id. 
	*/
	if(*pucCmd & EMM_IRD_ZONEID_MASK)
	{
		aucZoneId[0] = *(++pucCmd);
		aucZoneId[1] = *(++pucCmd);
		aucZoneId[2] = *(++pucCmd);
		aucZoneId[3] = *(++pucCmd);
		aucZoneId[4] = *(++pucCmd);
		aucZoneId[5] = *(++pucCmd);

		HxLOG_Print("IRD Cmd ZoneIDID : 0x%02x 0x%02x 0x%02x 0x%02x 0x%02x 0x%02x  \n", aucZoneId[0], aucZoneId[1], aucZoneId[2], aucZoneId[3], aucZoneId[4], aucZoneId[5]);
		if(local_irdCmd_GetZoneId(aucMyZoneId, MAX_ZONE_ID_LENGTH) != ERR_OK)
		{
			HxLOG_Error("Can't get ZoneID to compare with IRD Cmd zoneID");
			return ERR_FAIL;
		}

		if(HxSTD_memcmp(aucZoneId, aucMyZoneId, MAX_ZONE_ID_LENGTH))
		{
			HxLOG_Error("Differs the STB ZoneID with IRD CMD's zoneID");
			return ERR_FAIL;
		}
	}
#endif

	HxLOG_Print("IRD Cmd ID : [%d=0x%x], Operation Code : [%d]\n", *pIrdCmdId, *pIrdCmdId, *pOperationCode);
	return ERR_OK;
}


HERROR CA_NA_IRDCMD_ProcessIRDCmd(TUnsignedInt8 *pxIrdCmd)
{
	NA_IRDCMD_ID_t 			eIrdCmdId;
	NA_IRDCMD_OPCODE_t 		eOperationCode;
	HERROR					nErr = ERR_OK;

	HxLOG_Print("IRD_CMD_ALARM Received!\n");

	if ( pxIrdCmd == NULL )
	{
		HxLOG_Error("pIrdCmd Null!\n");
		return ERR_OK;
	}

	if ( CA_NA_IRDCMD_GetIRDCommand((TUnsignedInt8 *)pxIrdCmd, &eIrdCmdId, &eOperationCode) == ERR_OK )
	{
		HxLOG_Print("IRD Command Id [0x%x], Operation Code [%d]\n", eIrdCmdId, eOperationCode);
		if ( CA_NA_IRDCMD_ProcessSequenceNumber(eIrdCmdId, pxIrdCmd) == ERR_OK )
		{
			switch (eIrdCmdId)
			{
#if defined(CONFIG_MW_CAS_NAGRA_IRDCMD_PINCTL)
			case eComIDResetPincode:
				HxLOG_Print("Command ID : eComIDResetPincode!\n");
				if ( eOperationCode == eOpCodeDefault )
				{
					if ( s_pfnIrdCmdCallback )
						(* s_pfnIrdCmdCallback)(eComIDResetPincode, eOpCodeDefault, (TUnsignedInt32)0, (TUnsignedInt32)0);
				}
				break;

			case eComIDSetPincode:
				HxLOG_Print("Command ID : eComIDSetPincode!\n");
				{
					TUnsignedInt8 ucPinLen = 0;
					static TUnsignedInt8 ucPinCode[NA_IRDCMD_MAX_PINCODE_LEN];

					HxSTD_memset(ucPinCode, 0x00, NA_IRDCMD_MAX_PINCODE_LEN);
					if ( local_irdCmd_ProcGetPincodeByCommand((TUnsignedInt8 *)pxIrdCmd, (TUnsignedInt8 *)ucPinCode, (TUnsignedInt8 *)&ucPinLen) == ERR_OK )
					{
						if ( s_pfnIrdCmdCallback )
							(* s_pfnIrdCmdCallback)(eComIDSetPincode, eOpCodeDefault, (TUnsignedInt32)ucPinCode, (TUnsignedInt32)ucPinLen);
					}
				}
				break;
#endif

#if defined(CONFIG_MW_CAS_NAGRA_IRDCMD_FINGER)
			case eComIDForceIdentification:
				HxLOG_Print("Command ID : eComIDForceIdentification!\n");
				if ( eOperationCode == eOpCodeDefault )
				{
					if ( s_pfnIrdCmdCallback )
						(* s_pfnIrdCmdCallback)(eComIDForceIdentification, eOpCodeDefault, (TUnsignedInt32)0, (TUnsignedInt32)0);
				}
				break;
#endif

#if defined(CONFIG_MW_CAS_NAGRA_IRDCMD_TUNE)
			case eComIDForceTune:
				HxLOG_Print("Command ID : eComIDForceTune!\n");
				if ( eOperationCode == eOpCodeDefault )
				{
					TUnsignedInt32 hSvcHandle;

					if ( local_irdCmd_ProcGetServiceHandleByCommand((TUnsignedInt8 *)pxIrdCmd, &hSvcHandle) == ERR_OK )
					{
						if ( s_pfnIrdCmdCallback )
							(* s_pfnIrdCmdCallback)(eComIDForceTune, eOpCodeDefault, (TUnsignedInt32)hSvcHandle, (TUnsignedInt32)0);
					}
				}
				break;
#endif

#if defined(CONFIG_MW_CAS_NAGRA_IRDCMD_DEFSETTING)
			case eComIDSetForceDefSetting:
				HxLOG_Print("Command ID : eComID_SET_FORCE_STAND_BY!\n");
				if ( eOperationCode == eOpCodeDefault )
				{
					if ( s_pfnIrdCmdCallback )
						(* s_pfnIrdCmdCallback)(eComIDSetForceDefSetting, eOpCodeDefault, (TUnsignedInt32)0, (TUnsignedInt32)0);
				}
				break;
#endif

#if defined(CONFIG_MW_CAS_NAGRA_IRDCMD_USAGE)
			case eComIDChangeUsageID:
				HxLOG_Print("Command ID : eComIDChangeUsageID!\n");
				{
					 TUnsignedInt8 ucBuffer;
					if ( local_irdCmd_ProcGetUsageIdByCommand((TUnsignedInt8 *)pxIrdCmd, (TUnsignedInt8 *)&ucBuffer) == ERR_OK )
					{
						if ( eOperationCode == eOpCodeInit )
						{
							if ( s_pfnIrdCmdCallback )
								(* s_pfnIrdCmdCallback)(eComIDChangeUsageID, eOpCodeInit, (TUnsignedInt32)ucBuffer, (TUnsignedInt32)0);
						}
					}
				}
				break;
#endif

#if defined(CONFIG_MW_CAS_NAGRA_IRDCMD_REBOOT)
			case eComIDRebootStb:
				HxLOG_Print("Command ID : eComIDRebootStb!\n");
				if ( eOperationCode == eOpCodeInit )
				{
					if ( s_pfnIrdCmdCallback )
						(* s_pfnIrdCmdCallback)(eComIDRebootStb, eOpCodeInit, (TUnsignedInt32)0, (TUnsignedInt32)0);
				}
				break;
#endif

#if defined(CONFIG_MW_CAS_NAGRA_IRDCMD_STANDBY)
			case eComIDSetForceStandby:
				HxLOG_Print("Command ID : eComIDSetForceStandby!\n");
				if ( eOperationCode == eOpCodeDefault )
				{
					if ( s_pfnIrdCmdCallback )
						(* s_pfnIrdCmdCallback)(eComIDSetForceStandby, eOpCodeDefault, (TUnsignedInt32)0, (TUnsignedInt32)0);
				}
				break;
#endif

#if defined(CONFIG_MW_CAS_NAGRA_IRDCMD_MAIL)
			case eComIDMail:
				HxLOG_Print("Command ID : eComIDMail!\n");
				if ( eOperationCode == eOpCodeDefault )
				{
					TUnsignedInt16 usMailId, usDeletedMailId;

					usDeletedMailId = INVALID_MAIL_ID;
					if ( local_irdCmd_ProcBuildMailMsg((TUnsignedInt8 *)pxIrdCmd, &usMailId, &usDeletedMailId) == ERR_OK )
					{
						if ( s_pfnIrdCmdCallback )
							(* s_pfnIrdCmdCallback)(eComIDMail, eOpCodeDefault, (TUnsignedInt32)usMailId, (TUnsignedInt32)usDeletedMailId);
					}
				}
				break;
#endif

#if defined(CONFIG_MW_CAS_NAGRA_IRDCMD_POPUP)
			case eComIDDisplayPopUp:
				HxLOG_Print("Command ID : eComIDDisplayPopUp!\n");
				if ( eOperationCode == eOpCodeInit )
				{
					TUnsignedInt16 usPopupId;
					if(local_irdCmd_ProcBuildPopupMsgMsg((TUnsignedInt8 *)pxIrdCmd, &usPopupId) != ERR_OK)
					{
						// nothing to do
					}
				}
				else if ( eOperationCode == eOpCodeDefault)
				{
					local_irdCmd_ProcRemovePopupMsg();
				}
				break;
#endif

#if defined(CONFIG_MW_CAS_NAGRA_IRDCMD_FORMATDISK)
		 	case eComIDLogicalDiskFormat:
				HxLOG_Print("Command ID : eComIDLogicalDiskFormat!\n");
				if ( eOperationCode == eOpCodeInit)
				{
					if ( s_pfnIrdCmdCallback )
						(* s_pfnIrdCmdCallback)(eComIDLogicalDiskFormat, eOpCodeInit, (TUnsignedInt32)0, (TUnsignedInt32)0);
				}
		 		break;
#endif
#if defined(CONFIG_MW_CAS_NAGRA_IRDCMD_ZONEID)
			case eComIDZoneID:
				HxLOG_Print("Command ID : eComIDZoneID! (eOperationCode : 0x%x) \n", eOperationCode);
				switch ( eOperationCode )
				{
					case eOpCodeInit:
						nErr = local_irdCmd_ProcSetZoneId((TUnsignedInt8 *)pxIrdCmd);
						break;

					case eOpCodeDefault:
						nErr = local_irdCmd_ProcCancelZoneId();
						break;

					case eOpCodeCountryCode:
						nErr = local_irdCmd_ProcSetCountryCode((TUnsignedInt8 *)pxIrdCmd);
						break;
						
					default:
						HxLOG_Error("IRD cmd error, command_id:0x%X, operation:0x%X\r\n", eIrdCmdId, eOperationCode);
						break;
				}

				if(nErr == ERR_OK)
				{
					if ( s_pfnIrdCmdCallback )
						(* s_pfnIrdCmdCallback)(eComIDZoneID, eOperationCode, 0, 0);
				}
				break;
#endif
#if defined(CONFIG_MW_CAS_NAGRA_IRDCMD_EMMWAKEUP)
			case eComIDEmmWakeUp:
				switch ( eOperationCode )
				{
					case eOpCodeInit:
						local_irdCmd_ProcDisableEmmWakeUp();
						break;

					case eOpCodeDefault:
						local_irdCmd_ProcEnableEmmWakeUp((TUnsignedInt8 *)pxIrdCmd);
						break;

					default:
						HxLOG_Info("IRD cmd error, command_id:0x%X, operation:0x%X\r\n", (HUINT32)eIrdCmdId, (HUINT32)eOperationCode);
						break;
				}
				break;
#endif

#if defined(CONFIG_MW_CAS_NAGRA_OP_SES)
			case eComIDApsHdPlus:
				HxLOG_Print("Command ID : eComIDApsHdPlus!\n");
				if ( eOperationCode == eOpCodeDisplayProfile )
				{
					if (local_irdCmd_ProcDisplayPorfileByCommand((TUnsignedInt8 *)pxIrdCmd) == ERR_OK )
					{
						if ( s_pfnIrdCmdCallback )
							(* s_pfnIrdCmdCallback)(eComIDApsHdPlus, eOpCodeDisplayProfile, (TUnsignedInt32)0, (TUnsignedInt32)0);
					}
				}
				else if ( eOperationCode == eOpCodeMessage )
				{
					if (local_irdCmd_ProcMessageByCommand((TUnsignedInt8 *)pxIrdCmd) == ERR_OK )
					{
						if ( s_pfnIrdCmdCallback )
							(* s_pfnIrdCmdCallback)(eComIDApsHdPlus, eOpCodeMessage, (TUnsignedInt32)0, (TUnsignedInt32)0);
					}
				}
#if defined(CONFIG_SUPPORT_NOVA_APP)
				else if ( eOperationCode == eOpCodeMultiscreen )
				{
                                        #if 0
					/*if (local_irdCmd_ProcMessageByCommand((TUnsignedInt8 *)pxIrdCmd) == ERR_OK ) */
					{
						if ( s_pfnIrdCmdCallback )
							(* s_pfnIrdCmdCallback)(eComIDApsHdPlus, eOpCodeMultiscreen, pxIrdCmd, (TUnsignedInt32)0);
					}
                                        #endif
				}
#endif
				break;
#endif

			default:
				HxLOG_Warning("Not Support this Command!! IRD Command Id [0x%x], Operation Code [%d]\n", eIrdCmdId, eOperationCode);
				break;
			}
		}
		else
		{
			HxLOG_Print("Do Not Process this IRD Command!! IRD Command Id [0x%x], Operation Code [%d]\n", eIrdCmdId, eOperationCode);
		}
	}

	return ERR_OK;
}


HERROR CA_NA_IRDCMD_RegisterCallback(TIrdCmdCallbackFunc pfnCallback)
{
	if ( pfnCallback == NULL )
	{
		HxLOG_Error("CA_NA_IRDCMD_RegisterCallback() Fail!\n");
		return ERR_FAIL;
	}

	s_pfnIrdCmdCallback = pfnCallback;
	return ERR_OK;
}

#if defined(CONFIG_MW_CAS_NAGRA_IRDCMD_EMMWAKEUP)
HERROR CA_NA_IRDCMD_EMM_GetWakeupTime(UNIXTIME *pulTime)
{
	return local_irdCmd_EMM_GetWakeupTime(pulTime);
}


HERROR CA_NA_IRDCMD_EMM_InitWakingTable(void)
{
	return local_irdCmd_EMM_InitWakingTable();
}


HERROR CA_NA_IRDCMD_EMM_HasMoreNextWaking(TBoolean *pbNext)
{
	return local_irdCmd_EMM_HasMoreNextWaking(pbNext);
}


HERROR CA_NA_IRDCMD_EMM_GetWakingTuneInfo(TUnsignedInt8 *pucNetworkIdFlag, TUnsignedInt16 *pusNetworkId,
															TUnsignedInt8 *pucTsIdFlag, TUnsignedInt16 *pusTsId,
															TUnsignedInt8 *pucServiceIdFlag, TUnsignedInt16 *pusServiceId)
{
	return local_irdCmd_EMM_GetWakingTuneInfo(pucNetworkIdFlag, pusNetworkId,
													pucTsIdFlag, pusTsId,
													pucServiceIdFlag, pusServiceId);
}


HERROR CA_NA_IRDCMD_EMM_GetWakingTimer(UNIXTIME *pulTime)
{
	return local_irdCmd_EMM_GetWakingTimer(pulTime);
}
#endif

#if defined(CONFIG_MW_CAS_NAGRA_IRDCMD_ZONEID)
HERROR CA_NA_IRDCMD_ZoneId_GetID(TUnsignedInt8 *pZoneId, TUnsignedInt32 unMaxLen)
{
	return local_irdCmd_GetZoneId(pZoneId, unMaxLen);
}
HERROR CA_NA_IRDCMD_CountryCode_GetID(TUnsignedInt8 *pCountryCode, TUnsignedInt32 unMaxLen)
{
	return local_irdCmd_GetCountryCode(pCountryCode, unMaxLen);
}
#endif



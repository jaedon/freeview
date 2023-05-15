/**************************************************************************************************/
#define ________CA_CAK_IRDCMD_MAIL_Private_Include________________________________________
/**************************************************************************************************/
#include "vkernel.h"
#include "na_glob.h"

#include "linkedlist.h"

#include <_svc_cas_mgr_nvram.h>
#include <_svc_cas_mgr_common.h>

/**************************************************************************************************/
#define ________CA_CAK_IRDCMD_MAIL_Golbal_Value___________________________________________
/**************************************************************************************************/


/**************************************************************************************************/
#define ________CA_CAK_IRDCMD_MAIL_Private_Define_________________________________________
/**************************************************************************************************/





#define MAIL_MSG_HEADER_SLOT			7
#define MAIL_BLK_SIZE					8192	// 8K

#define UL_PARAM_UNCHANGED				(TUnsignedInt32)0xFFFFFFFF
#define US_PARAM_UNCHANGED				(TUnsignedInt16)0xFFFF
#define UC_PARAM_UNCHANGED				(TUnsignedInt8)0xFF

#define MAIL_NOT_USED					0
#define MAIL_USED						1
#define MAIL_UNREAD						0
#define MAIL_READ						1

#define QUEUE_SIZE						MAX_MAIL_MSG_NUM
#define MAX_MAIL_MSG_SLOT				7
#define MAX_MAIL_MSG_NUM				7

#ifdef WIN32
#define MAIL_JFFS2_PATH					""
#else
#define MAIL_JFFS2_PATH					CAS_NA_JFFS2_PATH
#endif
#define MAIL_FILE_NAME 					"NagraMailData"

//#define IRDCMD_MAIL_DBG


/**************************************************************************************************/
#define ________CA_CAK_IRDCMD_MAIL_Private_Type_Define____________________________________
/**************************************************************************************************/
typedef struct SMailMsgInfo
{
	TUnsignedInt32	nDataSize;
	TBoolean		bUsed;
	TBoolean		bRead;
	TUnsignedInt8	aucReserved[2];		// For Byte Align
} TSMailMsgInfo;

// Update가 자주 일어날 Data 이므로 EEP Rom에 저장한다.
typedef struct tagMailMsgInfo
{
	TUnsignedInt32	nSlotNum;	// Not Used				//	1					: 4 Byte
	TSMailMsgInfo	stMailMsgInfo[MAX_MAIL_MSG_SLOT];	//	8 * MAX_MAIL_MSG_SLOT : 56 Byte
} TSMailInfo;

typedef	struct tagMessageInfo
{
	TUnsignedInt32	hSave;
	TBoolean		bRead;			// 0 : unread, 1 : read
	TUnsignedInt8	ucReserved[3];
} MSG_INFO;

typedef	struct tagMailHeader
{
	TUnsignedInt32	nNumMsg;		// 저장된 Message 수
	MSG_INFO		stMsgInfo[MAX_MAIL_MSG_NUM];
} MAIL_HEADER;

typedef struct tagMailSegment
{
	TUnsignedInt16	nSegmentNum;
	TUnsignedInt8	szMsg[MAX_MAIL_LENG_PER_SEGMENT + 1];
	struct tagMailSegment *pNext;
} MAIL_SEGMENT;

typedef struct tagMailInfo
{
	NA_MAIL_MSG		stMailMsg;
	TUnsignedInt8	nTotalSegment;
	TUnsignedInt8	aulSegmentReceiveFlag[8];		// 64개의 Segment를 가질 수 있으므로 64bit 변수가 필요
	MAIL_SEGMENT	*pMailSegmentHeader;
} MAIL_INFO;

typedef struct tagFlashMailMessage
{
	TUnsignedInt16	nMailId;
	TUnsignedInt8	nPriority;
	UNIXTIME		nSystime;
	TUnsignedInt8	szText[MAX_MAIL_LENGTH];
} FLASH_MSG;


/**************************************************************************************************/
#define ________CA_CAK_IRDCMD_MAIL_Private_Static_Value____________________________________
/**************************************************************************************************/
static TSMailInfo		s_stMailInfo;
static NA_MAIL_MSG		*s_pstMailList[MAX_MAIL_MSG_NUM];
static POINTER_LIST_T	*s_pMailQueue = NULL;


/**************************************************************************************************/
#define ________CA_CAK_IRDCMD_MAIL_Private_Static_Prototype_________________________________
/**************************************************************************************************/
static HERROR		local_irdCmd_Mail_GetMailInfoData(TSMailInfo *pstMailInfo, TUnsignedInt32 *pnFlashSlot, TUnsignedInt32 *pSaveHandle);
static void			local_irdCmd_Mail_FreeMailMsg(TSMailInfo *pstMailInfo, TSMailMsgInfo *ptr);
static HERROR		local_irdCmd_Mail_GetFlashSlotAndDataSizeFromSaveHandle(TUnsignedInt32 hSave, TUnsignedInt32 *pnSlot, TUnsignedInt32 *pnDataSize);
static HERROR		local_irdCmd_Mail_UpdateMailMsgInfo(TSMailInfo *pstMailInfo, TUnsignedInt32 ulSlot, TBoolean bUsed, TBoolean bRead, TUnsignedInt32 nDataSize);
static HERROR		local_irdCmd_Mail_SaveMailMsgInfo(TSMailInfo *pstMailInfo);
static HERROR		local_irdCmd_Mail_ClearMailMsgInfo(void);
static HERROR		local_irdCmd_Mail_WriteFlash(TUnsignedInt32 ucSlotNum, TUnsignedInt32 ulSize, void *pvData);
static HERROR		local_irdCmd_Mail_ReadFlash(TUnsignedInt32 ucSlotNum, TUnsignedInt32 ulSize, void *pvData);
static HERROR		local_irdCmd_Mail_SetReadFlag(TUnsignedInt32 saveHandle, TBoolean ucRead);
static HERROR		local_irdCmd_Mail_MakeMailMessage(TUnsignedInt8 *pxIrdCmd, TUnsignedInt16	*pnMailId, TBoolean *pfCompleted, TUnsignedInt16 *pnDeletedMailId);
static HERROR		local_irdCmd_Mail_FindMailInfo(TUnsignedInt16 nMailId, MAIL_INFO **ppMailInfo);
static TSignedInt8	local_irdCmd_Mail_RankMailDate(void *varItem, void *staticItem);
static TBoolean		local_irdCmd_Mail_FilterSavedMessage(void *varItem, void *filter);
static void			local_irdCmd_Mail_FreeMailData(void *pItem);
static HERROR		local_irdCmd_Mail_GetNewMailInfo(TUnsignedInt16 nMailId, MAIL_INFO **ppMailInfo);
static HERROR		local_irdCmd_Mail_IsValidSaveHandle(TUnsignedInt32 hMsgSave);
static HERROR		local_irdCmd_Mail_FreeMailInfo(MAIL_INFO *pMailInfo);
static HERROR		local_irdCmd_Mail_MakeMailMessageHeaderFromMailInfo(TSMailInfo *pstMailInfo, MAIL_HEADER *pstMailHeader);
static HERROR		local_irdCmd_Mail_ReadMailMessageHeaderFromEEPRom(MAIL_HEADER *pstMailHeader);
static HERROR		local_irdCmd_Mail_MakeMailMessageArray(NA_MAIL_MSG *pastFlashMailInfo, TUnsignedInt32 *nMsgCnt);
static HERROR		local_irdCmd_Mail_LoadMailMessages(MAIL_INFO **pMailInfo, TUnsignedInt32 *pnCnt);
static HERROR		local_irdCmd_Mail_GetMailMessage(TUnsignedInt32 hSaveHandle , TUnsignedInt8 *pucData, TUnsignedInt32 *pnSize);
static HERROR		local_irdCmd_Mail_SaveMailToFlash(TUnsignedInt8 *pucData, TUnsignedInt32 nSize, TUnsignedInt32 *pMsgSave);
static HERROR		local_irdCmd_Mail_DeleteMailFromFlash(TUnsignedInt32 hMsgSave);
static HERROR		local_irdCmd_Mail_CreateMailSegment(TUnsignedInt8 nSegmentNum, MAIL_SEGMENT **ppstMailSegment);
static HERROR		local_irdCmd_Mail_DeleteOldMessage(TUnsignedInt16 *pusDeletedMailId);
static HERROR		local_irdCmd_Mail_SaveMailMessage(MAIL_INFO *pMailInfo);
static HERROR		local_irdCmd_Mail_DeleteMailMessage(MAIL_INFO *pMailInfo, TBoolean bDeleteFlash);
static HERROR		local_irdCmd_Mail_GetMailPriority(TUnsignedInt16 nMailId, NA_MAIL_PRIORITY *pnPriority);


/**************************************************************************************************/
#define ________CA_CAK_IRDCMD_MAIL_Private_Static_Prototype_Body____________________________
/**************************************************************************************************/
static HERROR local_irdCmd_Mail_GetMailInfoData(TSMailInfo *pstMailInfo, TUnsignedInt32 *pnFlashSlot, TUnsignedInt32 *pSaveHandle)
{
	TUnsignedInt8 i;

	for ( i=0 ; i<MAX_MAIL_MSG_SLOT; i++ )
	{
		// Not Used
		if ( pstMailInfo->stMailMsgInfo[i].bUsed == MAIL_NOT_USED )
		{
			// Set Used
			pstMailInfo->stMailMsgInfo[i].bUsed = MAIL_USED;
			if ( pnFlashSlot )
			{
				(*pnFlashSlot) = i;
				HxLOG_Print("Slot Number = [%d]\n", (HUINT32)*pnFlashSlot);
			}
			if ( pSaveHandle )
			{
				(*pSaveHandle) = (TUnsignedInt32)&pstMailInfo->stMailMsgInfo[i];
				HxLOG_Print("Save Handle = [0x%x]\n", (HUINT32)*pSaveHandle);
			}
			return ERR_OK;
		}
	}
	HxLOG_Info("Pool overflow...\n");
	return ERR_FAIL;
}


static void local_irdCmd_Mail_FreeMailMsg(TSMailInfo *pstMailInfo, TSMailMsgInfo *ptr)
{
	TUnsignedInt8 i;

	for ( i=0 ; i<MAX_MAIL_MSG_SLOT ; i++ )
	{
		if ( ptr == (&pstMailInfo->stMailMsgInfo[i]) )
		{
			pstMailInfo->stMailMsgInfo[i].bUsed = 0;
			pstMailInfo->stMailMsgInfo[i].nDataSize = 0;
			return;
		}
	}
}


static HERROR local_irdCmd_Mail_GetFlashSlotAndDataSizeFromSaveHandle(TUnsignedInt32 hSave, TUnsignedInt32 *pnSlot, TUnsignedInt32 *pnDataSize)
{
	TUnsignedInt8 i;

	if ( hSave == INAVLID_HANDLE )
	{
		HxLOG_Info("Invalild Save Handle\n");
		return ERR_FAIL;
	}

	if ( pnSlot == NULL || pnDataSize == NULL )
	{
		HxLOG_Info("pnSlot == NULL || pnDataSize == NULL \n");
		return ERR_FAIL;
	}

	HxLOG_Print("hSave = [0x%x]\n", hSave);

	for ( i=0 ; i<MAX_MAIL_MSG_SLOT ; i++ )
	{
		if ( hSave == (TUnsignedInt32)&s_stMailInfo.stMailMsgInfo[i] )
		{
			(*pnSlot) = i;
			(*pnDataSize) = s_stMailInfo.stMailMsgInfo[i].nDataSize;
			return ERR_OK;
		}
	}

	HxLOG_Info("Can not find matching Save handle! hSave : [0x%x] \n", hSave);
	return ERR_FAIL;
}


static HERROR local_irdCmd_Mail_UpdateMailMsgInfo(TSMailInfo *pstMailInfo, TUnsignedInt32 ulSlot, TBoolean bUsed, TBoolean bRead, TUnsignedInt32 nDataSize)
{
	if ( ulSlot >= MAX_MAIL_MSG_SLOT )
	{
		HxLOG_Info("ulSlot >= MAX_MAIL_MSG_SLOT\n");
		return ERR_FAIL;
	}

	if ( (nDataSize != UL_PARAM_UNCHANGED) && (nDataSize > MAIL_BLK_SIZE) )
	{
		HxLOG_Info("nDataSize(%d) >= MAIL_BLK_SIZE\n", nDataSize);
		nDataSize = MAIL_BLK_SIZE;
	}

	if ( bUsed != UC_PARAM_UNCHANGED )
	{
		pstMailInfo->stMailMsgInfo[ulSlot].bUsed = bUsed;
	}

	if ( bUsed != MAIL_NOT_USED )
	{
		if ( bRead != UC_PARAM_UNCHANGED )
		{
			pstMailInfo->stMailMsgInfo[ulSlot].bRead = bRead;
		}

		if ( nDataSize != UL_PARAM_UNCHANGED )
		{
			pstMailInfo->stMailMsgInfo[ulSlot].nDataSize = nDataSize;
		}
	}
	return ERR_OK;
}


static HERROR local_irdCmd_Mail_ClearMailMsgInfo(void)
{
	HERROR	hRes;
	TUnsignedInt8 ucBuffer[CA_NVRAM_IRDCMD_MAIL_HEADER_SIZE];

	HxSTD_memset(ucBuffer, 0x00, CA_NVRAM_IRDCMD_MAIL_HEADER_SIZE);
	HxLOG_Print("Clear Mail Header Data!\n");
	hRes = svc_cas_DevPortNVRAMWriteData(eDxCAS_GROUPID_NA, CA_NVRAM_IRDCMD_MAIL_HEADER_OFFSET, CA_NVRAM_IRDCMD_MAIL_HEADER_SIZE, (TUnsignedInt8 *)ucBuffer);
	if ( hRes )
	{
		HxLOG_Info("Eeprom Write Error! error code = [0x%x]\n", hRes);
		return ERR_FAIL;
	}
	return ERR_OK;
}


static HERROR local_irdCmd_Mail_SaveMailMsgInfo(TSMailInfo *pstMailInfo)
{
	HERROR	nRes;

	nRes = svc_cas_DevPortNVRAMWriteData(eDxCAS_GROUPID_NA, CA_NVRAM_IRDCMD_MAIL_HEADER_OFFSET, CA_NVRAM_IRDCMD_MAIL_HEADER_SIZE, (TUnsignedInt8 *)pstMailInfo);
	if ( nRes )
	{
		HxLOG_Info("Eeprom Write Error! error code = [0x%x]\n", nRes);
		return ERR_FAIL;
	}
	return ERR_OK;
}


static HERROR local_irdCmd_Mail_WriteFlash(TUnsignedInt32 ulSlotNum, TUnsignedInt32 ulSize, void *pvData)
{
	CASDEV_FS_UINT32	hFileHandle;
	CASDEV_FS_INT64	nPosOffset;
	CASDEV_FS_UINT32	ulWrittenSize;
	TSignedInt32	nErrCode;
	TUnsignedInt8	ucFileName[128];

	if ( ulSlotNum >= MAX_MAIL_MSG_SLOT )
	{
		HxLOG_Info("Invalid Slot Number = [%d]\n", ulSlotNum);
		return ERR_FAIL;
	}

	if ( (pvData == NULL) || (ulSize <= 0) )
	{
		HxLOG_Info("target is null...\n");
		return ERR_FAIL;
	}

	HxLOG_Print("Slot No[%d], Size[%d]\n", (int)ulSlotNum, (int)ulSize);

#if 0 //Nagra Init 과정에서 이미 check 했음
	if (svc_cas_DevFsIsDirExist(MAIL_JFFS2_PATH) != TRUE)
	{
		if (svc_cas_DevFsMkdir(MAIL_JFFS2_PATH) != ERR_OK)
		{
			HxLOG_Info("error occurred in svc_cas_DevFsMkdir\n");
			return ERR_FAIL;
		}
	}
#endif

	HxSTD_snprintf(ucFileName, 128, "%21s%13s%02d", MAIL_JFFS2_PATH, MAIL_FILE_NAME, ulSlotNum);

	nErrCode = svc_cas_DevFsOpen(0, ucFileName, PAL_FS_MODE_WRITE, &hFileHandle);
	if (nErrCode)
	{
		HxLOG_Info("error occurred in svc_cas_DevFsOpen(%d).\n", (unsigned int)nErrCode);
		return ERR_FAIL;
	}

	nErrCode = svc_cas_DevFsLSeek(hFileHandle, 0, PAL_FS_POS_START, &nPosOffset);
	if (nErrCode)
	{
		HxLOG_Info("error occurred in svc_cas_DevFsLSeek(%d).\n", (unsigned int)nErrCode);
		svc_cas_DevFsClose(hFileHandle);
		return ERR_FAIL;
	}

	nErrCode = svc_cas_DevFsWrite(hFileHandle, pvData, ulSize, &ulWrittenSize);
	if (nErrCode)
	{
		HxLOG_Info("error occurred in svc_cas_DevFsWrite(%d).\n", (unsigned int)nErrCode);
		svc_cas_DevFsClose(hFileHandle);
		return ERR_FAIL;
	}

	svc_cas_DevFsClose(hFileHandle);

#if defined(CONFIG_DROP_ROOT_PRIVILEGES)
	/* obama만 access 가능하도록 file permission 변경 */
	HLIB_EXTRA_Chmod(ucFileName, 0600);

	{
		HBOOL bRoot = FALSE;
		if(HLIB_EXTRA_AmIRoot(&bRoot) == ERR_OK)
		{
			if(bRoot == TRUE)
			{
				if(HLIB_EXTRA_Chown(ucFileName, "obama", "obama") != ERR_OK)
				{
					HxLOG_Error("ERROR In [HLIB_EXTRA_Chown]. [%s]\n", ucFileName);
				}
			}
		}
	}
#endif

	return ERR_OK;
}


static HERROR local_irdCmd_Mail_ReadFlash(TUnsignedInt32 ulSlotNum, TUnsignedInt32 ulSize, void *pvData)
{
	CASDEV_FS_UINT32	hFileHandle;
	CASDEV_FS_INT64	nPosOffset;
	CASDEV_FS_UINT32	ulReadSize;
	TSignedInt32	nErrCode;
	TUnsignedInt8	ucFileName[128];

	if ( ulSlotNum >= MAX_MAIL_MSG_SLOT )
	{
		HxLOG_Info("Invalid Slot Number = [%d]\n", ulSlotNum);
		return ERR_FAIL;
	}

	if ( (pvData == NULL) || (ulSize <= 0) )
	{
		HxLOG_Info("target is null...\n");
		return ERR_FAIL;
	}

	HxLOG_Print("Slot No[%d], Size[%d]\n", (int)ulSlotNum, (int)ulSize);
	if (svc_cas_DevFsIsDirExist(MAIL_JFFS2_PATH) != TRUE)
	{
		HxLOG_Info("No data...\n");
		return ERR_FAIL;
	}

	HxSTD_snprintf(ucFileName, 128, "%21s%13s%02d", MAIL_JFFS2_PATH, MAIL_FILE_NAME, ulSlotNum);

	nErrCode = svc_cas_DevFsOpen(0, ucFileName, PAL_FS_MODE_WRITE, &hFileHandle);
	if (nErrCode)
	{
		HxLOG_Info("error occurred in svc_cas_DevFsOpen(%d).\n", (unsigned int)nErrCode);
		return ERR_FAIL;
	}

	nErrCode = svc_cas_DevFsLSeek(hFileHandle, 0, PAL_FS_POS_START, &nPosOffset);
	if (nErrCode)
	{
		HxLOG_Info("error occurred in svc_cas_DevFsLSeek(%d).\n", (unsigned int)nErrCode);
		svc_cas_DevFsClose(hFileHandle);
		return ERR_FAIL;
	}

	nErrCode = svc_cas_DevFsRead(hFileHandle, pvData, ulSize, &ulReadSize);
	if (nErrCode)
	{
		HxLOG_Info("error occurred in svc_cas_DevFsRead(%d).\n", (unsigned int)nErrCode);
		svc_cas_DevFsClose(hFileHandle);
		return ERR_FAIL;
	}

	svc_cas_DevFsClose(hFileHandle);
	return ERR_OK;
}


static HERROR local_irdCmd_Mail_SetReadFlag(TUnsignedInt32 saveHandle, TBoolean ucRead)
{
	HERROR			nRes;
	TUnsignedInt8	i, ucSlot;
	TBoolean		bRead = 0;

	for ( i=0 ; i<MAX_MAIL_MSG_SLOT; i++ )
	{
		if ( saveHandle == (TUnsignedInt32)&s_stMailInfo.stMailMsgInfo[i] )
		{
			ucSlot = i;
			break;
		}
	}

	if ( i >= MAX_MAIL_MSG_SLOT )
	{
		HxLOG_Info("Invalid save Handle ! saveHandle [0x%x] \n", saveHandle );
		return ERR_FAIL;
	}

	bRead = ( ucRead == 1 ) ? MAIL_READ : MAIL_UNREAD;

	// Set Read flas to "1"
	nRes = local_irdCmd_Mail_UpdateMailMsgInfo(&s_stMailInfo, ucSlot, UC_PARAM_UNCHANGED, bRead, UL_PARAM_UNCHANGED);
	if ( nRes )
	{
		HxLOG_Info("Fail! error code = [0x%x]\n", nRes);
		return ERR_FAIL;
	}

	nRes = local_irdCmd_Mail_SaveMailMsgInfo(&s_stMailInfo);
	if ( nRes )
	{
		HxLOG_Info("Fail! error code = [0x%x]\n", nRes);
		return ERR_FAIL;
	}
	return ERR_OK;
}


static HERROR local_irdCmd_Mail_MakeMailMessage(TUnsignedInt8 *pxIrdCmd, TUnsignedInt16	*pnMailId, TBoolean *pfCompleted, TUnsignedInt16 *pnDeletedMailId)
{
	HERROR			nRes = ERR_FAIL;
	TUnsignedInt8	*pCmd = NULL;
	TUnsignedInt32	nLength, nMsgLength;
	TUnsignedInt16	nMailId;
	TUnsignedInt8	nTotalSegmentNum = 0;
	TUnsignedInt8	nPriority = eMailPriortyInvalid;
	TUnsignedInt8	nSegmentNum = 0;
	MAIL_INFO		*pNewMailInfo = NULL;
	MAIL_SEGMENT	*pNewSegment = NULL;

	if ( pxIrdCmd == NULL )
	{
		HxLOG_Info("pIrdCmd == NULL\n");
		return ERR_FAIL;
	}

	if ( pnMailId == NULL )
	{
		HxLOG_Info("pnMailId == NULL\n");
		return ERR_FAIL;
	}

	if ( pfCompleted == NULL )
	{
		HxLOG_Info("pfCompleted == NULL\n");
		return ERR_FAIL;
	}

	if ( pnDeletedMailId == NULL )
	{
		HxLOG_Info("pnDeletedMailId == NULL\n");
		return ERR_FAIL;
	}

	//3// Check Sum Test
	if ( CA_NA_IRDCMD_IsValidIRDCmd(pxIrdCmd) == ERR_FAIL )
	{
		HxLOG_Error("Invalid Check Sum, Maybe The IRD Command Data is Broken!!!! Please Check the Data.\n");
		return ERR_FAIL;
	}

	*pfCompleted = FALSE;
	*pnDeletedMailId = INVALID_MAIL_ID;
	nRes = ERR_OK;

	nLength = *(pxIrdCmd + 1);
	HxLOG_Print("nLength = 0x%x\n", nLength);

	// Skip Header Field : EMM Header, Length, Sequence Numver, Command ID, Operation ID
	pCmd = pxIrdCmd;
	pCmd += 7;
	if(*pCmd & EMM_IRD_ZONEID_MASK)
	{
		// Skip zone id, if there is zone id indicator.
		pCmd += 6;
	}
	pCmd += 1;

	// 10 bit
	nMailId = (TUnsignedInt16)(((*(pCmd)<<8 | *(pCmd+1))>>6) & 0x3fff);
	HxLOG_Print("[0x%02x][0x%02x] : Mail Id = 0x%x\n", *pCmd, *(pCmd+1), nMailId);
	pCmd++;

	// 6bit
	nTotalSegmentNum = (TUnsignedInt8)(*(pCmd) & 0x3f);
	HxLOG_Print("[0x%02x] : nTotalSegmentNum = %d\n", *pCmd, nTotalSegmentNum);
	pCmd++;

	// 2bit
	nPriority = (TUnsignedInt8)((*(pCmd)>>6) & 0x03);
	HxLOG_Print("[0x%02x] : nPriority = %d\n", *pCmd, nPriority);

	// 6bit
	nSegmentNum = (TUnsignedInt8)(*(pCmd) & 0x3f);
	HxLOG_Print("[0x%02x] : nSegmentNum = %d\n", *pCmd, nSegmentNum);
	pCmd++;

	nMsgLength = nLength - 10;	// 10 : seguence_number 4, command_id 1, operation 1, mail_id + total_segment 2, priority + segment_number 1, checksum 1
	nMsgLength = (nMsgLength > MAX_MAIL_LENG_PER_SEGMENT) ? MAX_MAIL_LENG_PER_SEGMENT : nMsgLength;
	HxLOG_Print("nMsgLength = %d\n", nMsgLength);

	if ( nTotalSegmentNum == 0 )
	{
		HxLOG_Error("Invalid Total Segment Number \n");
		return ERR_FAIL;
	}

	if ( nTotalSegmentNum > MAX_MAIL_SEGMENT_NUM )
	{
		HxLOG_Warning("nTotalSegmentNum > MAX_MAIL_SEGMENT_NUM, Set Maximun Value\n");
		nTotalSegmentNum = MAX_MAIL_SEGMENT_NUM;
	}

	// If Not Found Create New Mail Info
	pNewMailInfo = NULL;
	nRes = local_irdCmd_Mail_FindMailInfo(nMailId, &pNewMailInfo);
	if ( nRes == ERR_OK && pNewMailInfo == NULL )
	{
		static TUnsignedInt8 ucSaFlag[] = { 0x00, 0x80, 0xc0, 0xe0, 0xf0, 0xf8, 0xfc, 0xfe };
		TUnsignedInt8 i;

		//3// Create MAIL_INFO Data and Set Time
		nRes = local_irdCmd_Mail_GetNewMailInfo(nMailId, &pNewMailInfo);
		if ( nRes != ERR_OK || pNewMailInfo == NULL )
		{
			HxLOG_Error("pNewMailInfo == NULL\n");
			return ERR_FAIL;
		}

		pNewMailInfo->nTotalSegment			= nTotalSegmentNum;
		pNewMailInfo->stMailMsg.nMailId		= nMailId;
		pNewMailInfo->stMailMsg.nPriority	= nPriority;
		pNewMailInfo->stMailMsg.bRead		= FALSE;
		pNewMailInfo->pMailSegmentHeader	= NULL;
		pNewMailInfo->stMailMsg.hMsgSave	= INAVLID_HANDLE;	// Flash에 저장하는 시점에 저장된다.
		pNewMailInfo->stMailMsg.nStatus		= eMsgStatusUpdating;
		HxSTD_memset(pNewMailInfo->stMailMsg.szText, 0x00, MAX_MAIL_LENGTH);

		i = 0;
		if ( pNewMailInfo->nTotalSegment >= 8 )
		{
			for ( i=0 ; i<(pNewMailInfo->nTotalSegment/8) ; i++ )
			{
				pNewMailInfo->aulSegmentReceiveFlag[i] = 0xFF;
			}
		}

		if ( (pNewMailInfo->nTotalSegment%8) > 0 )
		{
			pNewMailInfo->aulSegmentReceiveFlag[i] = ucSaFlag[pNewMailInfo->nTotalSegment%8];
		}
	}
	else
	{
		// If Found existing mail data check mail message is completed
		if ( pNewMailInfo->stMailMsg.nStatus == eMsgStatusCompeleted )
		{
			HxLOG_Print("Notice!!! This Mail[id:%d] is already Saved!! Ignore this Msg!!\n", pNewMailInfo->stMailMsg.nMailId);
			return ERR_FAIL;
		}
	}

	//3// Create New Segment Node
	HxLOG_Print("local_irdCmd_Mail_CreateMailSegment(%d) \n", nSegmentNum);
	nRes = local_irdCmd_Mail_CreateMailSegment(nSegmentNum, &pNewSegment);
	if ( nRes == ERR_FAIL || pNewSegment == NULL )
	{
		HxLOG_Error("nRes [0x%x]n", nRes);
		return ERR_FAIL;
	}

	HxSTD_memcpy(pNewSegment->szMsg, pCmd, nMsgLength);
	pNewSegment->szMsg[nMsgLength] = '\0';

	HxLOG_Print("pNewMailInfo->aulSegmentReceiveFlag[%d] = 0x%x\n", (nSegmentNum)/8, pNewMailInfo->aulSegmentReceiveFlag[(nSegmentNum)/8]);
	pNewMailInfo->aulSegmentReceiveFlag[(nSegmentNum)/8] &= ~((0x80)>>(nSegmentNum%8));
	HxLOG_Print("pNewMailInfo->aulSegmentReceiveFlag[%d] = 0x%x\n", (nSegmentNum)/8, pNewMailInfo->aulSegmentReceiveFlag[(nSegmentNum)/8]);

	//3// Add New Node to List
	if ( pNewMailInfo->pMailSegmentHeader == NULL )
	{
		HxLOG_Print("Set Header Segment [%d]\n", pNewSegment->nSegmentNum);
		pNewMailInfo->pMailSegmentHeader = pNewSegment;
	}
	else
	{
		TBoolean bFound = FALSE;
		MAIL_SEGMENT *pPrev = NULL, *pSegment = NULL;

		pSegment = pPrev = pNewMailInfo->pMailSegmentHeader;
		while ( pSegment )
		{
			if ( pSegment->nSegmentNum >= pNewSegment->nSegmentNum )
			{
				if ( pSegment->nSegmentNum == pNewSegment->nSegmentNum )
				{
					HxLOG_Print("Same Segment Exist!! nSegmentNum  : %d\n", pNewSegment->nSegmentNum);
					pNewSegment->pNext = pSegment->pNext;
					if(pSegment != NULL)
						OxCAS_Free(pSegment);
					bFound = TRUE;
					break;
				}
				else
				{
					pNewSegment->pNext = pSegment;
				}

				if ( pSegment == pPrev && pPrev == pNewMailInfo->pMailSegmentHeader )
				{
					pNewMailInfo->pMailSegmentHeader = pNewSegment;
				}
				else
				{
					pPrev->pNext = pNewSegment;
				}
				bFound = TRUE;
				break;
			}
			pPrev = pSegment;
			pSegment = pSegment->pNext;
		}

		if ( !bFound )
		{
			pNewSegment->pNext = NULL;
			pPrev->pNext = pNewSegment;
		}

#if defined(IRDCMD_MAIL_DBG)
		pSegment = pPrev = pNewMailInfo->pMailSegmentHeader;
		while( pSegment )
		{
			HxLOG_Print("Segment Address [0x%x], Segment[%d].message : [%s]\n", pSegment, pSegment->nSegmentNum, pSegment->szMsg);
			pSegment = pSegment->pNext;
		}
#endif
	}

	// TODO: Create MAIL_SEGMENT DATA
	{
		TUnsignedInt8 i, nNumArr;
		TBoolean bCompleted = TRUE;

		nNumArr = (pNewMailInfo->nTotalSegment+7)/8;

#if defined(IRDCMD_MAIL_DBG)
		HxLOG_Print("Check Array Num = [%d] \n", nNumArr);
		HxLOG_Print("pNewMailInfo->aulSegmentReceiveFlag ");
		for ( i = 0 ; i < nNumArr ; i++ ) {
			HxLOG_Print("[0x%02x]", pNewMailInfo->aulSegmentReceiveFlag[i]);
		}
		HxLOG_Print("\n");
#endif

		for ( i=0 ; i<nNumArr ; i++ )
		{
			if ( pNewMailInfo->aulSegmentReceiveFlag[i] != 0 )
			{
				bCompleted = FALSE;
				break;
			}
		}

		HxLOG_Print("bCompleted = [%s]\n", (bCompleted) ? "TRUE" : "FALSE");

		nRes = ERR_OK;
		if ( bCompleted )
		{
			TUnsignedInt16 nDeletedMailId = INVALID_MAIL_ID;
			if ( local_irdCmd_Mail_DeleteOldMessage(&nDeletedMailId) == ERR_OK )
			{
				*pnDeletedMailId = nDeletedMailId;
				HxLOG_Print("Deleted Mail Id = [%d]\n", nDeletedMailId);
				if ( local_irdCmd_Mail_SaveMailMessage(pNewMailInfo) == ERR_OK )
				{
					pNewMailInfo->stMailMsg.nStatus = eMsgStatusCompeleted;

					*pnMailId		= pNewMailInfo->stMailMsg.nMailId;
					*pfCompleted	= TRUE;
					nRes = ERR_OK;
				}
				else
				{
					*pnMailId		= pNewMailInfo->stMailMsg.nMailId;
					*pfCompleted	= FALSE;
					nRes = ERR_FAIL;
					HxLOG_Info("Fail!!!\n");
				}
			}
			else
			{
				*pnMailId		= INVALID_MAIL_ID;
				*pfCompleted	= FALSE;
				nRes = ERR_FAIL;
				HxLOG_Error("Fail!!!\n");
			}
			if ( nRes != ERR_OK )
			{
				// 이 경우 Mail data를 Free 해주어야 한다.
				HxLOG_Print("Delete Mail Message(FALSE)\n");
				local_irdCmd_Mail_DeleteMailMessage(pNewMailInfo, FALSE);
			}
		}
	}
	return nRes;
}


static HERROR local_irdCmd_Mail_FindMailInfo(TUnsignedInt16 nMailId, MAIL_INFO **ppMailInfo)
{
	POINTER_LIST_T	*found, *current;
	MAIL_INFO		*pMailInfo;

	if ( nMailId == INVALID_MAIL_ID )
	{
		HxLOG_Info("nMailId == INVALID_MAIL_ID\n");
		return ERR_FAIL;
	}

	if ( ppMailInfo == NULL )
	{
		HxLOG_Info("ppMailInfo == NULL\n");
		return ERR_FAIL;
	}

	*ppMailInfo = NULL;

	found = NULL;
	current = s_pMailQueue;
	while ( current )
	{
		pMailInfo = (MAIL_INFO *)current->pvContents;
		if ( pMailInfo )
		{
			if ( pMailInfo->stMailMsg.nMailId == nMailId )
			{
				HxLOG_Print("Found matching Mail Id [%d]\n", pMailInfo->stMailMsg.nMailId);
				found = current;
				break;
			}
		}
		current = current->pNext;
	 	if ( current == s_pMailQueue )
	 		break;
	}

	if ( found )
	{
		*ppMailInfo = (MAIL_INFO *)found->pvContents;
	}
#if defined(IRDCMD_MAIL_DBG)
	else
	{
		HxLOG_Print("Not Found matching Mail Id : [%d]\n", nMailId);
	}
#endif
	return ERR_OK;
}


static TSignedInt8 local_irdCmd_Mail_RankMailDate(void *varItem, void *staticItem)
{
	MAIL_INFO *pVarMailInfo, *pStaticMailInfo;

	pVarMailInfo = (MAIL_INFO *)varItem;
	pStaticMailInfo = (MAIL_INFO *)staticItem;

	if ( pVarMailInfo == NULL || pStaticMailInfo == NULL )
	{
		return 1;
	}

	if ( pVarMailInfo->stMailMsg.nSystime <= pStaticMailInfo->stMailMsg.nSystime )
	{
		return 1;
	}
	return 0;
}


static TBoolean local_irdCmd_Mail_FilterSavedMessage(void *varItem, void *filter)
{
	MAIL_INFO *pVarMailInfo;

	pVarMailInfo = (MAIL_INFO *)varItem;

	if ( pVarMailInfo == NULL )
	{
		return FALSE;
	}

	if ( pVarMailInfo->stMailMsg.nStatus == eMsgStatusCompeleted )
	{
		return TRUE;
	}
	return FALSE;
}


static void local_irdCmd_Mail_FreeMailData(void *pItem)
{
	if (local_irdCmd_Mail_FreeMailInfo((MAIL_INFO *)pItem) != ERR_OK)
	{
		HxLOG_Info("error in local_irdCmd_Mail_FreeMailInfo...\n");
	}
}


static HERROR local_irdCmd_Mail_GetNewMailInfo(TUnsignedInt16 nMailId, MAIL_INFO **ppMailInfo)
{
	POINTER_LIST_T	*pRoot;
	MAIL_INFO		*pNewMailInfo = NULL;

	if ( ppMailInfo == NULL )
	{
		HxLOG_Info("ppMailInfo == NULL\n");
		return ERR_FAIL;
	}

	*ppMailInfo = NULL;

	//3// 새로운 공간을 Alloc한다.
	pNewMailInfo = (MAIL_INFO *)OxCAS_Malloc(sizeof(MAIL_INFO));
	if ( pNewMailInfo == NULL )
	{
		HxLOG_Info("pNewMailInfo == NULL\n");
		return ERR_FAIL;
	}

	HxSTD_memset(pNewMailInfo, 0x00, sizeof(MAIL_INFO));
	(void)VK_CLOCK_GetTime(&pNewMailInfo->stMailMsg.nSystime);
	pRoot = UTIL_LINKEDLIST_InsertListItemToPointerList(s_pMailQueue, (void *)pNewMailInfo, (RANK_FUNC)local_irdCmd_Mail_RankMailDate);
	if ( pRoot == NULL )
	{
		HxLOG_Info("Fail!!!\n");
		OxCAS_Free(pNewMailInfo);
		return ERR_FAIL;
	}

	s_pMailQueue = pRoot;
	*ppMailInfo = pNewMailInfo;
	return ERR_OK;
}


static HERROR local_irdCmd_Mail_IsValidSaveHandle(TUnsignedInt32 hMsgSave)
{
	if ( hMsgSave == INAVLID_HANDLE )
		return ERR_FAIL;
	return ERR_OK;
}

static HERROR local_irdCmd_Mail_FreeMailInfo(MAIL_INFO *pMailInfo)
{
	HERROR nRes = FALSE;

	if ( pMailInfo == NULL )
	{
		return ERR_FAIL;
	}

	// Mail이 완전히 받아지지 않았을  경우 Alloc 했던 Mail Segment data를 Free 해야 한다.
	if ( pMailInfo->stMailMsg.nStatus != eMsgStatusCompeleted )
	{
		MAIL_SEGMENT *pMailSegment, *pSegmentHeader, *pCurSegment;
		pSegmentHeader = pMailInfo->pMailSegmentHeader;
		if ( pSegmentHeader )
		{
			pMailSegment = pSegmentHeader;
			while ( pMailSegment )
			{
				pCurSegment = pMailSegment;
				pMailSegment = pMailSegment->pNext;
				if ( pCurSegment != NULL )
				{
					OxCAS_Free(pCurSegment);
				}
			}
		}
		pMailInfo->pMailSegmentHeader = NULL;
	}

	// Delete Mail Message From Flash
	if ( local_irdCmd_Mail_IsValidSaveHandle(pMailInfo->stMailMsg.hMsgSave) == ERR_OK )
	{
		nRes = local_irdCmd_Mail_DeleteMailFromFlash(pMailInfo->stMailMsg.hMsgSave);
	}

	HxSTD_memset(pMailInfo, 0, sizeof(MAIL_INFO));
	OxCAS_Free(pMailInfo);
	return nRes;
}


static HERROR local_irdCmd_Mail_MakeMailMessageHeaderFromMailInfo(TSMailInfo *pstMailInfo, MAIL_HEADER *pstMailHeader)
{
	TUnsignedInt8 i, j;

	if ( pstMailInfo == NULL || pstMailHeader == NULL )
	{
		HxLOG_Info("pstMailInfo == NULL || pstMailHeader == NULL \n");
		return ERR_FAIL;
	}

	j = 0;
	for ( i=0 ; i<MAX_MAIL_MSG_SLOT ; i++ )
	{
		if ( j >= MAX_MAIL_MSG_NUM )
		{
			break;
		}

		if ( pstMailInfo->stMailMsgInfo[i].bUsed == MAIL_USED )
		{
			pstMailHeader->stMsgInfo[j].bRead = pstMailInfo->stMailMsgInfo[i].bRead;
			pstMailHeader->stMsgInfo[j].hSave = (TUnsignedInt32)&pstMailInfo->stMailMsgInfo[i];
			j++;
		}
	}

	HxLOG_Print("Total Mail Number = [%d]\n", j);
	pstMailHeader->nNumMsg = j;
	return ERR_OK;
}


static HERROR local_irdCmd_Mail_ReadMailMessageHeaderFromEEPRom(MAIL_HEADER *pstMailHeader)
{
	HERROR	nRes;

	nRes = svc_cas_DevPortNVRAMWriteData(eDxCAS_GROUPID_NA, CA_NVRAM_IRDCMD_MAIL_HEADER_OFFSET, CA_NVRAM_IRDCMD_MAIL_HEADER_SIZE, (TUnsignedInt8 *)&s_stMailInfo);
	if ( nRes )
	{
		HxLOG_Info("Read Eeprom Error! res code = [0x%x]\n", nRes);
		return ERR_FAIL;
	}

#if defined(IRDCMD_MAIL_DBG)
	{
		TUnsignedInt8 i;

		HxLOG_Print("==============================\n");
		HxLOG_Print("EEPROM Data\n");
		for ( i=0 ; i<MAX_MAIL_MSG_SLOT ; i++ )
		{
			HxLOG_Print("FlashSlot[%d][U:%d, R:%d, S:%d]\n", i,
			s_stMailInfo.stMailMsgInfo[i].bUsed, s_stMailInfo.stMailMsgInfo[i].bRead, s_stMailInfo.stMailMsgInfo[i].nDataSize));
		}
		HxLOG_Print("==============================\n");
	}
#endif

	nRes = local_irdCmd_Mail_MakeMailMessageHeaderFromMailInfo(&s_stMailInfo, pstMailHeader);
	if ( nRes )
	{
		HxLOG_Info("Fail. Error Code [0x%x]\n", nRes);
		return ERR_FAIL;
	}
	return ERR_OK;
}


static HERROR local_irdCmd_Mail_MakeMailMessageArray(NA_MAIL_MSG *pastFlashMailInfo, TUnsignedInt32 *nMsgCnt)
{
	MAIL_HEADER		stMsgHeader;
	TUnsignedInt32	nSize = 0;
	HERROR			nRes;

	if ( local_irdCmd_Mail_ReadMailMessageHeaderFromEEPRom(&stMsgHeader) == ERR_OK )
	{
		TUnsignedInt32	i, j;
		FLASH_MSG		*pFlashMsg = NULL;

		pFlashMsg = OxCAS_Malloc(sizeof(FLASH_MSG));
		if ( pFlashMsg == NULL )
		{
			HxLOG_Info("Insufficient Memory!!\n");
			return ERR_FAIL;
		}

		HxLOG_Print("stMsgHeader.nNumMsg = [%d]\n", stMsgHeader.nNumMsg);

		j = 0;
		for ( i=0 ; i<stMsgHeader.nNumMsg ; i++ )
		{
			nRes = local_irdCmd_Mail_GetMailMessage(stMsgHeader.stMsgInfo[i].hSave, (TUnsignedInt8 *)pFlashMsg, &nSize);
			if ( nRes != ERR_OK )
			{
				HxLOG_Info("local_irdCmd_Mail_GetMailMessage() Fail!\n");
				continue;
			}

			pastFlashMailInfo[j].nSize		= nSize;
			pastFlashMailInfo[j].bRead 		= stMsgHeader.stMsgInfo[i].bRead;
			pastFlashMailInfo[j].hMsgSave	= stMsgHeader.stMsgInfo[i].hSave;
			pastFlashMailInfo[j].nMailId	= pFlashMsg->nMailId;
			pastFlashMailInfo[j].nPriority	= pFlashMsg->nPriority;
			pastFlashMailInfo[j].nSystime	= pFlashMsg->nSystime;
			HxSTD_memcpy(pastFlashMailInfo[j].szText, pFlashMsg->szText, MAX_MAIL_LENGTH);
			pastFlashMailInfo[j].nStatus	= eMsgStatusCompeleted;
			j++;
		}

		if ( nMsgCnt )
		{
			(*nMsgCnt) = j;
		}

		OxCAS_Free(pFlashMsg);
		return ERR_OK;
	}

	HxLOG_Info("Fail!!!!\n");
	return ERR_FAIL;
}


static HERROR local_irdCmd_Mail_LoadMailMessages(MAIL_INFO **pMailInfo, TUnsignedInt32 *pnCnt)
{
	TUnsignedInt32	i, nMsgCnt;
	NA_MAIL_MSG		astFlashMailInfo[MAX_MAIL_MSG_NUM];
	MAIL_INFO		*pMailItem = NULL;

	HxLOG_Print("============ ( + ) \n");

	// Mail Message Header를 읽고 그 정보를 보고 Flash에 저장된 Mail Contents를 읽는다.
	if ( local_irdCmd_Mail_MakeMailMessageArray(astFlashMailInfo, &nMsgCnt) == ERR_FAIL )
	{
		return ERR_FAIL;
	}

	HxLOG_Print("Mail Message Count = [%d]\n", nMsgCnt);

	//3// Make Mail message Link
	s_pMailQueue = NULL;
	for ( i=0 ; i<nMsgCnt ; i++ )
	{
		pMailItem = (MAIL_INFO *)OxCAS_Malloc(sizeof(MAIL_INFO));
		if ( pMailItem == NULL )
		{
			HxLOG_Info("pMailInfo == NULL \n");
			return ERR_FAIL;
		}
		HxSTD_memset(pMailItem, 0x00, sizeof(MAIL_INFO));
		HxSTD_memcpy(&pMailItem->stMailMsg, &astFlashMailInfo[i], sizeof(NA_MAIL_MSG));
		s_pMailQueue = UTIL_LINKEDLIST_InsertListItemToPointerList(s_pMailQueue, (TUnsignedInt32 *)pMailItem, (RANK_FUNC)local_irdCmd_Mail_RankMailDate);
	}

	HxLOG_Print("Number Of Mail = [%d]\n", UTIL_LINKEDLIST_GetNumListItemsInPointerList(s_pMailQueue, NULL, NULL));
	HxLOG_Print("============ ( - ) \n\n");
	return ERR_OK;
}


static HERROR local_irdCmd_Mail_GetMailMessage(TUnsignedInt32 hSaveHandle , TUnsignedInt8 *pucData, TUnsignedInt32 *pnSize)
{
	HERROR	nRes;
	TUnsignedInt32 ulSlot, nDataSize;

	if (!hSaveHandle)
	{
		HxLOG_Info("hSaveHandle == NULL\n");
		return ERR_FAIL;
	}

	nRes = local_irdCmd_Mail_GetFlashSlotAndDataSizeFromSaveHandle(hSaveHandle, &ulSlot, &nDataSize);
	if ( nRes )
	{
		HxLOG_Info("Fail, Error code = [0x%x]\n", nRes);
		return ERR_FAIL;
	}

 	HxLOG_Print("Slot Num [%d], DataSize [%d], Savehandle [0x%x]\n", ulSlot, nDataSize, hSaveHandle);

	HxSTD_memset(pucData, 0x00, sizeof(FLASH_MSG));
	nRes = local_irdCmd_Mail_ReadFlash (ulSlot, nDataSize, (void *)pucData);
	if ( nRes )
	{
		HxLOG_Info("Fail, Error code = [0x%x]\n", nRes);
		return ERR_FAIL;
	}

	*pnSize = nDataSize;
	return ERR_OK;
}


static HERROR local_irdCmd_Mail_SaveMailToFlash(TUnsignedInt8 *pucData, TUnsignedInt32 nSize, TUnsignedInt32 *pMsgSave)
{
	HERROR			nRes;
	TUnsignedInt32	nSlotIdx;
	TUnsignedInt32	hSaveHandle = INAVLID_HANDLE;

	// Check Value Handle
	if ( pMsgSave == NULL )
	{
		HxLOG_Info("Invalid Handle Value\n");
		return ERR_FAIL;
	}

	*pMsgSave = INAVLID_HANDLE;
	nRes = local_irdCmd_Mail_GetMailInfoData(&s_stMailInfo, &nSlotIdx, &hSaveHandle);
	if ( nRes )
	{
		HxLOG_Info("Flash Memory Full!! No Empty Slot!!\n");
		return ERR_FAIL;
	}

	HxLOG_Print("Slot Num [%d], hSaveHandle [0x%x]\n", nSlotIdx, hSaveHandle);

	nRes = local_irdCmd_Mail_WriteFlash(nSlotIdx, nSize, (void *)pucData);
	if ( nRes )
	{
		HxLOG_Info("Flash Write Error! error code = [0x%x]\n", nRes);
		return ERR_FAIL;
	}

	nRes = local_irdCmd_Mail_UpdateMailMsgInfo(&s_stMailInfo, nSlotIdx, MAIL_USED, UC_PARAM_UNCHANGED, nSize);
	if ( nRes )
	{
		HxLOG_Info("local_irdCmd_Mail_UpdateMailMsgInfo() Fail!\n");
		return ERR_FAIL;
	}

	*pMsgSave = (TUnsignedInt32)hSaveHandle;
	nRes = local_irdCmd_Mail_SaveMailMsgInfo(&s_stMailInfo);
	if ( nRes )
	{
		HxLOG_Info("Fail!\n");
		return ERR_FAIL;
	}

	if ( *pMsgSave == INAVLID_HANDLE )
	{
		HxLOG_Info("Invalid Handle Value : INAVLID_HANDLE\n");
		return ERR_FAIL;
	}

	HxLOG_Print("Message Save Handle = [0x%x]\n", *pMsgSave);
	return ERR_OK;
}


static HERROR local_irdCmd_Mail_DeleteMailFromFlash(TUnsignedInt32 hMsgSave)
{
	HERROR	nRes;

	// Check Value Handle
	if ( hMsgSave == INAVLID_HANDLE )
	{
		HxLOG_Info("Invalide Handle Value\n");
		return ERR_FAIL;
	}

	local_irdCmd_Mail_FreeMailMsg(&s_stMailInfo, (TSMailMsgInfo *)hMsgSave);
	nRes = local_irdCmd_Mail_SaveMailMsgInfo(&s_stMailInfo);
	if ( nRes != ERR_OK )
	{
		HxLOG_Info("Error : result 0x%x\n", nRes);
		return ERR_FAIL;
	}
	return ERR_OK;
}


static HERROR local_irdCmd_Mail_CreateMailSegment(TUnsignedInt8 nSegmentNum, MAIL_SEGMENT **ppstMailSegment)
{
	MAIL_SEGMENT *pMailSegment = NULL;

	if ( ppstMailSegment == NULL )
	{
		HxLOG_Info("pstMailSegment == NULL\n");
		return ERR_FAIL;
	}

	pMailSegment = OxCAS_Malloc(sizeof(MAIL_SEGMENT));
	if ( pMailSegment )
	{
		HxSTD_memset(pMailSegment, 0, sizeof(MAIL_SEGMENT));
		pMailSegment->nSegmentNum = nSegmentNum;
		*ppstMailSegment = pMailSegment;
		return ERR_OK;
	}

	*ppstMailSegment = NULL;
	return ERR_FAIL;
}


static HERROR local_irdCmd_Mail_DeleteOldMessage(TUnsignedInt16 *pusDeletedMailId)
{
	TUnsignedInt16 nNumList = 0;

	if ( pusDeletedMailId )
	{
		*pusDeletedMailId = INVALID_MAIL_ID;
	}

	nNumList = UTIL_LINKEDLIST_GetNumListItemsInPointerList(s_pMailQueue, (FILTER_FUNC)local_irdCmd_Mail_FilterSavedMessage, NULL);
	HxLOG_Print("Saved Item Number : [%d]\n", nNumList);

	//3// 제일 오래된 Message를 지우고
	if ( nNumList >= QUEUE_SIZE )
	{
		POINTER_LIST_T	*pToDelete, *root;
		MAIL_INFO		*pMailToDelete = NULL;
		TUnsignedInt16	nMailIdToDelete = INVALID_MAIL_ID;
		TUnsignedInt32	hToDelete;

#if defined(IRDCMD_MAIL_DBG)
		{
			POINTER_LIST_T	*current;
			MAIL_INFO		*pMailInfo = NULL;

			HxLOG_Print("=======================================================\n");
			current = s_pMailQueue;
			while ( current )
			{
				pMailInfo = (MAIL_INFO *)current->pvContents;
				if ( pMailInfo )
				{
					HxLOG_Print("MailId:[%d], SystemTime:[0x%x], nStatus:[%d], Priority:[%d], bRead:[%d], hMsgSave:[0x%x]\n",
							pMailInfo->stMailMsg.nMailId, pMailInfo->stMailMsg.nSystime, pMailInfo->stMailMsg.nStatus,
							pMailInfo->stMailMsg.nPriority, pMailInfo->stMailMsg.bRead, pMailInfo->stMailMsg.hMsgSave));
				}
				current = current->pNext;
			 	if ( current == s_pMailQueue )
			 		break;
			}
			HxLOG_Print("=======================================================\n\n");
		}
#endif
		HxLOG_Print("Find Oldest Saved Mail!\n");

		pToDelete = NULL;
		{
			POINTER_LIST_T	*current;
			MAIL_INFO		*pMailInfo = NULL;
			UNIXTIME		nMinTime = (TUnsignedInt32)-1;

			current = s_pMailQueue;
			while ( current )
			{
				pMailInfo = (MAIL_INFO *)current->pvContents;
				if ( pMailInfo ) {
					if ( pMailInfo->stMailMsg.nStatus == eMsgStatusCompeleted )
					{
						if ( nMinTime > pMailInfo->stMailMsg.nSystime )
						{
							nMinTime = pMailInfo->stMailMsg.nSystime;
							pToDelete = current;
						}
					}
				}
				current = current->pNext;
			 	if ( current == s_pMailQueue )
			 		break;
			}
		}

		if ( pToDelete == NULL )
		{
			HxLOG_Info("pToDelete == NULL\n");
			return ERR_FAIL;
		}

		pMailToDelete = (MAIL_INFO *)pToDelete->pvContents;
		if ( pMailToDelete )
		{
			hToDelete = pMailToDelete->stMailMsg.hMsgSave;
			nMailIdToDelete = pMailToDelete->stMailMsg.nMailId;
			HxLOG_Print("Mail To Delete : Id : [%d], SystemTime [0x%x], Save Handle [0x%x]\n",
					pMailToDelete->stMailMsg.nMailId, pMailToDelete->stMailMsg.nSystime, pMailToDelete->stMailMsg.hMsgSave);
		}
		root = UTIL_LINKEDLIST_DeleteListItemFromPointerList((POINTER_LIST_T *)s_pMailQueue, (POINTER_LIST_T *)pToDelete, (FREE_FUNC)local_irdCmd_Mail_FreeMailData) ;
		if ( root == NULL )
		{
			HxLOG_Info("Fail!!!\n");
			return ERR_FAIL;
		}
		s_pMailQueue = root;
		if ( local_irdCmd_Mail_DeleteMailFromFlash(hToDelete) == FALSE )
		{
			HxLOG_Info("Fail!!!\n");
			return ERR_FAIL;
		}
		if ( pusDeletedMailId )
		{
			*pusDeletedMailId = nMailIdToDelete;
		}
		return ERR_OK;
	}
	return ERR_OK;
}


static HERROR local_irdCmd_Mail_SaveMailMessage(MAIL_INFO *pMailInfo)
{
	HERROR			nRes;
	TUnsignedInt8	aucMessage[MAX_MAIL_LENGTH];
	TUnsignedInt8	*pszMsg = NULL;
	MAIL_SEGMENT	*pMailSegment = NULL;
	MAIL_SEGMENT	*pTmpSegment = NULL;
	TUnsignedInt32	nStrLength = 0;
	FLASH_MSG		*pFlashMsg = NULL;

	if ( pMailInfo == NULL )
	{
		HxLOG_Info("pMailInfo == NULL\n");
		return ERR_FAIL;
	}

	HxSTD_memset(aucMessage, 0x00, MAX_MAIL_LENGTH);

	pMailSegment = pMailInfo->pMailSegmentHeader;
	pszMsg = (TUnsignedInt8 *)aucMessage;
	while ( pMailSegment )
	{
		pTmpSegment = pMailSegment;
#if 0
		nStrLength = MWC_UTIL_DvbStrlen((TUnsignedInt8 *)pMailSegment->szMsg);	// MWC_UTIL_DvbStrlen() API 내부가 왜 HxSTD_StrLen으로 변경된 원인을 알아야 한다.
#else
		nStrLength = MWC_UTIL_DvbStrlenStrOnly(pMailSegment->szMsg) + MWC_UTIL_GetLocaleCodeLen(pMailSegment->szMsg);
#endif
		HxLOG_Print("Segment[%d] Message Lenght = %d\n", pMailSegment->nSegmentNum, nStrLength);
		HxSTD_memcpy(pszMsg, pMailSegment->szMsg, nStrLength);
		pszMsg += nStrLength;
		pMailSegment = pMailSegment->pNext;
		if ( pTmpSegment != NULL )
		{
			OxCAS_Free(pTmpSegment);
		}
	}
	pszMsg[nStrLength] = '\0';

	HxLOG_Print("Mail Message [%s]\n", aucMessage);
	pMailInfo->pMailSegmentHeader = NULL;
	HxSTD_memset(pMailInfo->aulSegmentReceiveFlag, 0x00, sizeof(TUnsignedInt8)*8);

	pFlashMsg = OxCAS_Malloc(sizeof(FLASH_MSG));
	if ( pFlashMsg == NULL )
	{
		HxLOG_Info("pFlashMsg == NULL\n");
		return ERR_FAIL;
	}

	HxSTD_memset(pFlashMsg, 0x00, sizeof(FLASH_MSG));
	pFlashMsg->nMailId		= pMailInfo->stMailMsg.nMailId;
	pFlashMsg->nPriority	= pMailInfo->stMailMsg.nPriority;
	pFlashMsg->nSystime		= pMailInfo->stMailMsg.nSystime;
	HxSTD_memcpy(pMailInfo->stMailMsg.szText, aucMessage, MAX_MAIL_LENGTH);
	HxSTD_memcpy(pFlashMsg->szText, aucMessage, MAX_MAIL_LENGTH);

	HxLOG_Print("= Buffer Mail Contents =\n");
	HxLOG_Print("MailId : [0x%x], Priority : [%d], nSystemTime : [0x%x]\n",  pFlashMsg->nMailId,  pFlashMsg->nPriority,  pFlashMsg->nSystime);
	HxLOG_Print("[%s]\n", pFlashMsg->szText);

	//3// Get Valid Flash Save Handle, Save Message to Flash
	nRes = local_irdCmd_Mail_SaveMailToFlash((TUnsignedInt8 *)pFlashMsg, sizeof(FLASH_MSG), &pMailInfo->stMailMsg.hMsgSave);
	OxCAS_Free(pFlashMsg);
	if ( nRes == ERR_FAIL )
	{
		HxLOG_Info(" Fail\n");
		return ERR_FAIL;
	}

	HxLOG_Print("Mail ID [%d] : Save handle [0x%x]\n", pMailInfo->stMailMsg.nMailId, pMailInfo->stMailMsg.hMsgSave);
	return ERR_OK;
}


static HERROR local_irdCmd_Mail_DeleteMailMessage(MAIL_INFO *pMailInfo, TBoolean bDeleteFlash)
{
	POINTER_LIST_T	*pToDelete, *current;
	MAIL_INFO		*pMailInfoToDelete;
	TUnsignedInt32	hSaveHandle = INAVLID_HANDLE;

	if ( pMailInfo == NULL )
	{
		HxLOG_Info("pMailInfo == NULL\n");
		return ERR_FAIL;
	}

	// Delete Mail Message From Mail List
	pToDelete = NULL;
	current = s_pMailQueue;
	while ( current )
	{
		pMailInfoToDelete = (MAIL_INFO *)current->pvContents;
		if ( pMailInfoToDelete )
		{
			if ( pMailInfoToDelete->stMailMsg.nMailId == pMailInfo->stMailMsg.nMailId )
			{
				HxLOG_Print("Mail : ID [%d], Save Handle [0x%x]\n", pMailInfoToDelete->stMailMsg.nMailId, pMailInfoToDelete->stMailMsg.hMsgSave);
				pToDelete = current;
				hSaveHandle = pMailInfoToDelete->stMailMsg.hMsgSave;
				break;
			}
		}
		current = current->pNext;
	 	if ( current == s_pMailQueue )
	 		break;
	}

	if ( pToDelete == NULL )
	{
		HxLOG_Info("pToDelete == NULL\n");
		return ERR_FAIL;
	}

	s_pMailQueue = UTIL_LINKEDLIST_DeleteListItemFromPointerList((POINTER_LIST_T *)s_pMailQueue, (POINTER_LIST_T *)pToDelete, (FREE_FUNC)local_irdCmd_Mail_FreeMailData) ;
	if ( bDeleteFlash )
	{
		if ( local_irdCmd_Mail_DeleteMailFromFlash(hSaveHandle) != ERR_OK )
		{
			HxLOG_Info("Fail!!!\n");
			return ERR_FAIL;
		}
	}
	return ERR_OK;
}


static HERROR local_irdCmd_Mail_GetMailPriority(TUnsignedInt16 nMailId, NA_MAIL_PRIORITY *pnPriority)
{
	HERROR		nRes;
	MAIL_INFO	*pMailInfo = NULL;

	if ( pnPriority == NULL )
	{
		return ERR_FAIL;
	}

	nRes = local_irdCmd_Mail_FindMailInfo(nMailId, &pMailInfo);
	if ( nRes == ERR_OK )
	{
		if ( pMailInfo )
		{
			*pnPriority = pMailInfo->stMailMsg.nPriority;
			return ERR_OK;
		}
	}

	*pnPriority = eMailPriortyInvalid;
	return ERR_FAIL;
}


/**************************************************************************************************/
#define ________CA_CAK_IRDCMD_MAIL_Public_Functions_Body___________________________________
/**************************************************************************************************/
HERROR CA_NA_IRDCMD_MAIL_CheckUnreadMessage(void)
{
	POINTER_LIST_T	*pItem;
	MAIL_INFO 		*pMail;

	pItem = s_pMailQueue;
	while ( pItem )
	{
		pMail = (MAIL_INFO *)pItem->pvContents;
		if ( pMail )
		{
			if ( pMail->stMailMsg.nStatus == eMsgStatusCompeleted )
			{
				if ( pMail->stMailMsg.bRead == FALSE )
				{
					return ERR_OK;
				}
			}
		}
		pItem = pItem->pNext;
	 	if ( pItem == s_pMailQueue )
	 		break;
	}
	return ERR_FAIL;
}


HERROR CA_NA_IRDCMD_MAIL_ClearMailMessageHeader(void)
{
	return local_irdCmd_Mail_ClearMailMsgInfo();
}


HERROR CA_NA_IRDCMD_MAIL_MakeMailMessage(TUnsignedInt8 *pxIrdCmd, TUnsignedInt16	*pnMailId, TBoolean *pfCompleted, TUnsignedInt16 *pnDeletedMailId)
{
	return local_irdCmd_Mail_MakeMailMessage(pxIrdCmd, pnMailId, pfCompleted, pnDeletedMailId);
}


HERROR CA_NA_IRDCMD_MAIL_LoadMailMessages(void)
{
	return local_irdCmd_Mail_LoadMailMessages(NULL, NULL);
}


HERROR CA_NA_IRDCMD_MAIL_GetMailPriority(TUnsignedInt16 nMailId, NA_MAIL_PRIORITY *pnPriority)
{
	return local_irdCmd_Mail_GetMailPriority(nMailId, pnPriority);
}


HERROR CA_NA_IRDCMD_MAIL_GetMailList(NA_MAIL_MSG ***pppMailList, TUnsignedInt32 *pnCount)
{
	TUnsignedInt8	i;
	POINTER_LIST_T	*found, *current;
	MAIL_INFO		*pMailInfo;

	if ( pppMailList == NULL || pnCount == NULL )
	{
		HxLOG_Info("pppMailList == NULL || pnCount == NULL\n");
		return ERR_FAIL;
	}

	HxSTD_memset(s_pstMailList, 0x00, sizeof(NA_MAIL_MSG *)*MAX_MAIL_MSG_NUM);
	i = 0;
	found = NULL;
	current = s_pMailQueue;
	while ( current )
	{
		if ( i >= MAX_MAIL_MSG_NUM )
		{
			break;
		}
		pMailInfo = (MAIL_INFO *)current->pvContents;
		if ( pMailInfo )
		{
			if ( pMailInfo->stMailMsg.nStatus == eMsgStatusCompeleted )
			{
				s_pstMailList[i++] = &pMailInfo->stMailMsg;
				HxLOG_Print("[MW_CA_NA_MAIL_GetMailList] Mail Id:[%d], Priority[%d], Read[%d]\n", pMailInfo->stMailMsg.nMailId, pMailInfo->stMailMsg.nPriority, pMailInfo->stMailMsg.bRead);
				HxLOG_Print("[MW_CA_NA_MAIL_GetMailList] Mail hMsgSave:[0x%x], SaveTime:[0x%x]\n", pMailInfo->stMailMsg.hMsgSave, pMailInfo->stMailMsg.nSystime);
				HxLOG_Print("[MW_CA_NA_MAIL_GetMailList] Mail pszTexe = [0x%x]\n", pMailInfo->stMailMsg.szText[0]);
				HxLOG_Print("[MW_CA_NA_MAIL_GetMailList] ============================================\n");
			}
		}
		current = current->pNext;
	 	if ( current == s_pMailQueue )
	 		break;
	}
	HxLOG_Print("\n");

	*pnCount = i;
	*pppMailList = s_pstMailList;

	HxLOG_Print("Number Of Mail : [%d]\n", *pnCount);
	return ERR_OK;
}


HERROR CA_NA_IRDCMD_MAIL_GetMailData(TUnsignedInt16 nMailId, NA_MAIL_MSG **ppMailData)
{
	HERROR nRes;
	MAIL_INFO *pMailInfo;

	if ( ppMailData == NULL )
	{
		return ERR_FAIL;
	}

	nRes = local_irdCmd_Mail_FindMailInfo(nMailId, &pMailInfo);
	if ( nRes == ERR_OK && pMailInfo != NULL )
	{
		*ppMailData = &pMailInfo->stMailMsg;
		return ERR_OK;
	}
	return ERR_FAIL;
}


HERROR CA_NA_IRDCMD_MAIL_DeleteMailMessage(TUnsignedInt16 nMailId)
{
	MAIL_INFO	*pMailInfo = NULL;
	HERROR		nRes;

	nRes = local_irdCmd_Mail_FindMailInfo(nMailId, &pMailInfo);
	if ( nRes != ERR_OK || pMailInfo == NULL )
	{
		HxLOG_Info("nRes != ERR_OK || pMailInfo == NULL\n");
		return ERR_FAIL;
	}

	if ( local_irdCmd_Mail_DeleteMailMessage(pMailInfo, TRUE) != ERR_OK )
	{
		HxLOG_Info("Fail!\n");
	}
	return ERR_OK;
}


HERROR CA_NA_IRDCMD_MAIL_UpdateReadFlag(TUnsignedInt16 nMailId, TBoolean bRead)
{
	HERROR nRes;
	MAIL_INFO *pMailInfo = NULL;

	nRes = local_irdCmd_Mail_FindMailInfo(nMailId, &pMailInfo);
	if ( nRes != ERR_OK || pMailInfo == NULL )
	{
		HxLOG_Info("nRes != ERR_OK || pMailInfo == NULL\n");
		return ERR_FAIL;
	}
	pMailInfo->stMailMsg.bRead = bRead;

	nRes = local_irdCmd_Mail_SetReadFlag(pMailInfo->stMailMsg.hMsgSave, (bRead == TRUE) ? 1 : 0);
	if ( nRes )
	{
		HxLOG_Info("Fail!\n");
		return ERR_FAIL;
	}
	return ERR_OK;
}



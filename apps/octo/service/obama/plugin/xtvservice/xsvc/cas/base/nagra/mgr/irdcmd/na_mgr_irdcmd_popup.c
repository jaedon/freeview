/**************************************************************************************************/
#define ________CA_CAK_IRDCMD_POPUP_Private_Include_______________________________________
/**************************************************************************************************/
#include "vkernel.h"
#include "na_glob.h"

#include "linkedlist.h"

#include <_svc_cas_mgr_common.h>


/**************************************************************************************************/
#define ________CA_CAK_IRDCMD_POPUP_Golbal_Value_________________________________________
/**************************************************************************************************/


/**************************************************************************************************/
#define ________CA_CAK_IRDCMD_POPUPL_Private_Define_______________________________________
/**************************************************************************************************/





#define QUEUE_SIZE							1

#define MAX_POPUP_SEGMENT_NUM				63
#define MAX_POPUP_LENG_PER_SEGMENT			45

#ifdef WIN32
#define POPUP_JFFS2_PATH					""
#else
#define POPUP_JFFS2_PATH					CAS_NA_JFFS2_PATH
#endif
#define POPUP_FILE_NAME 					"NagraPopupData"

//#define IRDCMD_POPUP_DBG


/**************************************************************************************************/
#define ________CA_CAK_IRDCMD_POPUP_Private_Type_Define___________________________________
/**************************************************************************************************/
typedef struct tagPopupSegment
{
	TUnsignedInt16		nSegmentNum;
	TUnsignedInt8		szMsg[MAX_POPUP_LENG_PER_SEGMENT + 1];
	struct POPUP_SEGMENT *pNext;
} POPUP_SEGMENT;

typedef struct tagPopupInfo
{
	NA_POPUP_MSG	stPopupMsg;
	TUnsignedInt8	nTotalSegment;
	TUnsignedInt8	aulSegmentReceiveFlag[8];		// 64개의 Segment를 가질 수 있으므로 64bit 변수가 필요
	POPUP_SEGMENT	*pPopupSegmentHeader;
} POPUP_INFO;


/**************************************************************************************************/
#define ________CA_CAK_IRDCMD_POPUP_Private_Static_Value___________________________________
/**************************************************************************************************/
static POINTER_LIST_T *s_pPopupQueue = NULL;


/**************************************************************************************************/
#define ________CA_CAK_IRDCMD_POPUP_Private_Static_Prototype________________________________
/**************************************************************************************************/
static HERROR		local_irdCmd_Popup_ReadPopupMessage(NA_POPUP_MSG *pPopupMsg);
static HERROR		local_irdCmd_Popup_SavePopupMessage(POPUP_INFO *pPopupInfo);
static HERROR 		local_irdCmd_Popup_DisplayPopupMessage(POPUP_INFO *pPopupInfo);
static HERROR 		local_irdCmd_Popup_RemovePopupMessage(void);
static HERROR		local_irdCmd_Popup_MakePopupMessage(TUnsignedInt8 *pxIrdCmd, TUnsignedInt16 *pnPopupId, TBoolean*pfCompleted);
static HERROR 		local_irdCmd_Popup_FindPopupInfo(TUnsignedInt16 nPopupId, POPUP_INFO **ppPopupInfo);
static TSignedInt8	local_irdCmd_Popup_RankPopupDate(TUnsignedInt32 *varItem, TUnsignedInt32 *staticItem);
static TBoolean 	local_irdCmd_Popup_FilterProcessdMessage(void *varItem, void *filter);
static void			local_irdCmd_Popup_FreePopupData(void *pItem);
static HERROR		local_irdCmd_Popup_GetNewPopupInfo(TUnsignedInt16 nPopupId, POPUP_INFO **ppPopupInfo);
static HERROR		local_irdCmd_Popup_FreePopupInfo(POPUP_INFO *pPopupInfo);
static HERROR 		local_irdCmd_Popup_CreatePopupSegment(TUnsignedInt8 nSegmentNum, POPUP_SEGMENT **ppstPopupSegment);
static HERROR 		local_irdCmd_Popup_DeleteOldMessage(TUnsignedInt16 *pusDeletedPopupId);
static HERROR		local_irdCmd_Popup_JoinPopupMessage(POPUP_INFO *pPopupInfo);
static HERROR		local_irdCmd_Popup_DeletePopupMessage(POPUP_INFO *pPupupInfo);


/**************************************************************************************************/
#define ________CA_CAK_IRDCMD_POPUP_Private_Static_Prototype_Body__________________________
/**************************************************************************************************/
static HERROR local_irdCmd_Popup_ReadPopupMessage(NA_POPUP_MSG *pPopupMsg)
{
	CASDEV_FS_UINT32	hFileHandle;
	CASDEV_FS_INT64	nPosOffset;
	CASDEV_FS_UINT32	ulReadSize;
	TSignedInt32	nErrCode;
	TUnsignedInt8	ucFileName[128];

	void 			*pvData = NULL;
	TSignedInt32	ulSize = 0;

	if (pPopupMsg == NULL)
	{
		HxLOG_Error("target is null...\n");
		return ERR_FAIL;
	}

	if (svc_cas_DevFsIsDirExist(POPUP_JFFS2_PATH) != TRUE)
	{
		HxLOG_Error("No data...\n");
		return ERR_FAIL;
	}

	HxSTD_snprintf(ucFileName, 128, "%21s%14s", POPUP_JFFS2_PATH, POPUP_FILE_NAME);

	nErrCode = svc_cas_DevFsOpen(0, ucFileName, PAL_FS_MODE_READ, &hFileHandle);
	if (nErrCode)
	{
		HxLOG_Error("error occurred in svc_cas_DevFsOpen(%d).\n", (unsigned int)nErrCode);
		return ERR_FAIL;
	}

	nErrCode = svc_cas_DevFsLSeek(hFileHandle, 0, PAL_FS_POS_START, &nPosOffset);
	if (nErrCode)
	{
		HxLOG_Error("error occurred in svc_cas_DevFsLSeek(%d).\n", (unsigned int)nErrCode);
		svc_cas_DevFsClose(hFileHandle);
		return ERR_FAIL;
	}

	pvData = pPopupMsg;
	ulSize = sizeof(NA_POPUP_MSG);

	nErrCode = svc_cas_DevFsRead(hFileHandle, pvData, ulSize, &ulReadSize);
	if (nErrCode)
	{
		HxLOG_Error("error occurred in svc_cas_DevFsRead(%d).\n", (unsigned int)nErrCode);
		svc_cas_DevFsClose(hFileHandle);
		return ERR_FAIL;
	}

	svc_cas_DevFsClose(hFileHandle);
	return ERR_OK;
}


static HERROR local_irdCmd_Popup_SavePopupMessage(POPUP_INFO *pPopupInfo)
{
	CASDEV_FS_UINT32	hFileHandle;
	CASDEV_FS_INT64	nPosOffset;
	CASDEV_FS_UINT32	ulWrittenSize;
	TSignedInt32	nErrCode;
	TUnsignedInt8	ucFileName[128];

	void 			*pvData = NULL;
	TSignedInt32	ulSize = 0;

	if (pPopupInfo == NULL)
	{
		HxLOG_Error("target is null...\n");
		return ERR_FAIL;
	}

	if (pPopupInfo->stPopupMsg.nPersistence != POPUP_PERSISTENCE_INVALID)
	{
#if 0 //Nagra Init 과정에서 이미 check 했음
		if (svc_cas_DevFsIsDirExist(POPUP_JFFS2_PATH) != TRUE)
		{
			if (svc_cas_DevFsMkdir(POPUP_JFFS2_PATH) != ERR_OK)
			{
				HxLOG_Info("error occurred in svc_cas_DevFsMkdir\n");
				return ERR_FAIL;
			}
		}
#endif

		HxSTD_snprintf(ucFileName, 128, "%21s%14s", POPUP_JFFS2_PATH, POPUP_FILE_NAME);

		nErrCode = svc_cas_DevFsOpen(0, ucFileName, PAL_FS_MODE_WRITE, &hFileHandle);
		if (nErrCode)
		{
			HxLOG_Error("error occurred in svc_cas_DevFsOpen(%d).\n", (unsigned int)nErrCode);
			return ERR_FAIL;
		}

		nErrCode = svc_cas_DevFsLSeek(hFileHandle, 0, PAL_FS_POS_START, &nPosOffset);
		if (nErrCode)
		{
			HxLOG_Error("error occurred in svc_cas_DevFsLSeek(%d).\n", (unsigned int)nErrCode);
			svc_cas_DevFsClose(hFileHandle);
			return ERR_FAIL;
		}

		pvData = &pPopupInfo->stPopupMsg;
		ulSize = sizeof(NA_POPUP_MSG);

		nErrCode = svc_cas_DevFsWrite(hFileHandle, pvData, ulSize, &ulWrittenSize);
		if (nErrCode)
		{
			HxLOG_Error("error occurred in svc_cas_DevFsWrite(%d).\n", (unsigned int)nErrCode);
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
	return ERR_FAIL;
}

#if defined(CONFIG_MW_CAS_NAGRA_DALAPI_7_1_ABOVE)
static HERROR local_irdCmd_Popup_DisplayPopupMessage(POPUP_INFO *pPopupInfo)
{
	TBdcStatus status;
	TBdcPopUpContent stPopupContent;
	NaPopupUI_t		*pstPopupUi = NULL;

	if (pPopupInfo == NULL)
	{
		HxLOG_Error("pPopupInfo == NULL\n");
		return ERR_FAIL;
	}

	if (pPopupInfo->stPopupMsg.nStrLen <= 0)
	{
		return ERR_FAIL;
	}

	pstPopupUi = (NaPopupUI_t *)OxCAS_Calloc(sizeof(NaPopupUI_t));
	if(pstPopupUi == NULL)
	{
		HxLOG_Error("pstPopupUi is NULL\n");
		return ERR_FAIL;
	}
	pstPopupUi->eType = ePopup_Message;
	if(pPopupInfo->stPopupMsg.szText != NULL)
	{
		pstPopupUi->pucMsg = (HUINT8 *)OxCAS_Calloc(pPopupInfo->stPopupMsg.nStrLen + 1);
		if(pstPopupUi->pucMsg != NULL)
		{
			HxSTD_memcpy(pstPopupUi->pucMsg, pPopupInfo->stPopupMsg.szText, pPopupInfo->stPopupMsg.nStrLen);
		}
	}
		
	if (pPopupInfo->stPopupMsg.nPersistence == POPUP_PERSISTENCE_NORMAL)
		pstPopupUi->ucPersistence = POPUP_PERSISTENCE_NORMAL;
	else if (pPopupInfo->stPopupMsg.nPersistence == POPUP_PERSISTENCE_TIMEOUT)
		pstPopupUi->ucPersistence = POPUP_PERSISTENCE_TIMEOUT;
	else if (pPopupInfo->stPopupMsg.nPersistence == POPUP_PERSISTENCE_USER)
		pstPopupUi->ucPersistence = POPUP_PERSISTENCE_USER;
	else
		return ERR_FAIL;

	status = NAMGR_CAKMGR_SendPopupMessage(pstPopupUi);
	if (status == ERR_OK)
	{
		local_irdCmd_Popup_SavePopupMessage(pPopupInfo);
		local_irdCmd_Popup_DeletePopupMessage(pPopupInfo);
		return ERR_OK;
	}
	local_irdCmd_Popup_DeletePopupMessage(pPopupInfo);
	return ERR_FAIL;
}

static HERROR local_irdCmd_Popup_RemovePopupMessage(void)
{
	TBdcStatus status;
	NaPopupUI_t		*pstPopupUi = NULL;
	
	pstPopupUi = (NaPopupUI_t *)OxCAS_Calloc(sizeof(NaPopupUI_t));
	if(pstPopupUi == NULL)
	{
		HxLOG_Error("pstPopupUi is NULL\n");
		return ERR_FAIL;
	}
	pstPopupUi->eType = ePopup_Message;
		
	status = NAMGR_CAKMGR_SendPopupMessage(pstPopupUi);
	if (status == ERR_OK)
	{
		CA_NA_IRDCMD_POPUP_DeletePopupMessage();
		return ERR_OK;
	}
	return ERR_FAIL;
}

#else
static HERROR local_irdCmd_Popup_DisplayPopupMessage(POPUP_INFO *pPopupInfo)
{
#if defined(CONFIG_MW_CAS_NAGRA_CA_PRODUCT_ICME)
	NaPopupUI_t	*pstPopupUi = NULL;
	HERROR hError;
#else
	TBdcStatus status;
	TBdcPopUpPersistence nPersistence;
	TBdcPopUpContent stPopupContent;
	TBdcIsoLanguage nLangCode = {"eng"};
#endif

	if (pPopupInfo == NULL)
	{
		HxLOG_Error("pPopupInfo == NULL\n");
		return ERR_FAIL;
	}

#if defined(CONFIG_MW_CAS_NAGRA_CA_PRODUCT_ICME)
	if(pPopupInfo->stPopupMsg.nPersistence >= POPUP_PERSISTENCE_INVALID)
	{
		HxLOG_Error("[%s:%d] Invalid pPopupInfo->stPopupMsg.nPersistence (%d)\n", __FUNCTION__, __HxLINE__, pPopupInfo->stPopupMsg.nPersistence);
		return ERR_FAIL;
	}

	pstPopupUi = (NaPopupUI_t *)OxCAS_Calloc(sizeof(NaPopupUI_t));
	if(pstPopupUi == NULL)
	{
		HxLOG_Error("[%s:%d] pstPopupUi is NULL\n", __FUNCTION__, __HxLINE__);
		return ERR_FAIL;
	}

	pstPopupUi->eType = ePopup_Message;
	pstPopupUi->ucPersistence = pPopupInfo->stPopupMsg.nPersistence;
	pstPopupUi->pucMsg = (HUINT8 *)OxCAS_Calloc(pPopupInfo->stPopupMsg.nStrLen + 1);
	if(pstPopupUi->pucMsg != NULL)
		HxSTD_StrNCpy(pstPopupUi->pucMsg, pPopupInfo->stPopupMsg.szText, pPopupInfo->stPopupMsg.nStrLen);

	hError = NAMGR_CAKMGR_SendPopupMessage(pstPopupUi);
	if(hError == ERR_OK)
#else
	HxSTD_memset((void*)&stPopupContent, 0, sizeof(TBdcPopUpContent));
	if (pPopupInfo->stPopupMsg.nStrLen <= 0)
		return ERR_FAIL;

	if (pPopupInfo->stPopupMsg.nPersistence == POPUP_PERSISTENCE_NORMAL)
		nPersistence = BDC_POP_UP_PERSISTENCE_ALWAYS;
	else if (pPopupInfo->stPopupMsg.nPersistence == POPUP_PERSISTENCE_TIMEOUT)
		nPersistence = BDC_POP_UP_PERSISTENCE_TIMEOUT;
	else if (pPopupInfo->stPopupMsg.nPersistence == POPUP_PERSISTENCE_USER)
		nPersistence = BDC_POP_UP_PERSISTENCE_USER;
	else
		return ERR_FAIL;
	HxLOG_Error("pPopupInfo->stPopupMsg.nPersistence (%d)\n", pPopupInfo->stPopupMsg.nPersistence);

	memcpy(stPopupContent.language, nLangCode, sizeof(TBdcIsoLanguage));
	stPopupContent.numBytes = pPopupInfo->stPopupMsg.nStrLen;
	stPopupContent.pContent = (TUnsignedInt8*)pPopupInfo->stPopupMsg.szText;

	HxLOG_Error("stPopupContent.pContent = [%s] \n", stPopupContent.pContent);

	status = bdcDisplayPopUp(nPersistence, 1, &stPopupContent);
	if (status == BDC_NO_ERROR)
#endif
	{
		local_irdCmd_Popup_SavePopupMessage(pPopupInfo);
		local_irdCmd_Popup_DeletePopupMessage(pPopupInfo);
		return ERR_OK;
	}
	local_irdCmd_Popup_DeletePopupMessage(pPopupInfo);
	return ERR_FAIL;
}

static HERROR local_irdCmd_Popup_RemovePopupMessage(void)
{
#if defined(CONFIG_MW_CAS_NAGRA_CA_PRODUCT_ICME)
	NaPopupUI_t	*pstPopupUi = NULL;
	HERROR hError;
#else
	TBdcStatus status;
#endif

#if defined(CONFIG_MW_CAS_NAGRA_CA_PRODUCT_ICME)
	pstPopupUi = (NaPopupUI_t *)OxCAS_Calloc(sizeof(NaPopupUI_t));
	if(pstPopupUi == NULL)
	{
		HxLOG_Error("[%s:%d] pstPopupUi is NULL\n", __FUNCTION__, __HxLINE__);
		return ERR_FAIL;
	}

	pstPopupUi->eType = ePopup_Message;

	hError = NAMGR_CAKMGR_SendPopupMessage(pstPopupUi);
	if(hError == ERR_OK)
#else
#if defined(CONFIG_PROD_HGS1000S)
	status = BDC_ERROR;
#else
	status = bdcRemovePopUp();
#endif
	if (status == BDC_NO_ERROR)
#endif
	{
		CA_NA_IRDCMD_POPUP_DeletePopupMessage();
		return ERR_OK;
	}
	return ERR_FAIL;
}
#endif

static HERROR local_irdCmd_Popup_MakePopupMessage(TUnsignedInt8 *pxIrdCmd, TUnsignedInt16 *pnPopupId, TBoolean*pfCompleted)
{
	HERROR			nRes = ERR_FAIL;
	TUnsignedInt8	*pCmd = NULL;
	TUnsignedInt32	nLength, nMsgLength;
	TUnsignedInt16	nPopupId;
	TUnsignedInt8	nTotalSegmentNum = 0;
	TUnsignedInt8	nPersistence = POPUP_PERSISTENCE_INVALID;
	TUnsignedInt8	nSegmentNum = 0;

	POPUP_INFO		*pNewPopupInfo = NULL;
	POPUP_SEGMENT	*pNewSegment = NULL;

	if (pxIrdCmd == NULL)
	{
		HxLOG_Error("pIrdCmd == NULL\n");
		return ERR_FAIL;
	}

	if (pnPopupId == NULL)
	{
		HxLOG_Error("pnMailId == NULL\n");
		return ERR_FAIL;
	}

	if (pfCompleted == NULL)
	{
		HxLOG_Error("pfCompleted == NULL\n");
		return ERR_FAIL;
	}

	//3// Check Sum Test
	if (CA_NA_IRDCMD_IsValidIRDCmd(pxIrdCmd)   == ERR_FAIL)
	{
		HxLOG_Error("Invalid Check Sum, Maybe The IRD Command Data is Broken!!!! Please Check the Data.\n");
		return ERR_FAIL;
	}

	*pfCompleted = FALSE;
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
	nPopupId = (TUnsignedInt16)(((*(pCmd)<<8 | *(pCmd+1))>>6) & 0x3fff);
	HxLOG_Print("[0x%02x][0x%02x] : Popup Id = 0x%x\n", *pCmd, *(pCmd+1), nPopupId);
	pCmd++;

	// 6bit
	nTotalSegmentNum = (TUnsignedInt8)(*(pCmd) & 0x3f);
	HxLOG_Print("[0x%02x] : nTotalSegmentNum = %d\n", *pCmd, nTotalSegmentNum);
	pCmd++;

	// 2bit
	nPersistence = (TUnsignedInt8)((*(pCmd)>>6) & 0x03);
	HxLOG_Print("[0x%02x] : nPersistence = %d\n", *pCmd, nPersistence);

	// 6bit
	nSegmentNum = (TUnsignedInt8)(*(pCmd) & 0x3f);
	HxLOG_Print("[0x%02x] : nSegmentNum = %d\n", *pCmd, nSegmentNum);
	pCmd++;

	// 10 : seguence_number 4, command_id 1, operation 1, Popup_id + total_segment 2, priority + segment_number 1, checksum 1
	nMsgLength = (TUnsignedInt32)(nLength - 10);
	if ( nMsgLength > MAX_POPUP_LENG_PER_SEGMENT )
	{
		nMsgLength = MAX_POPUP_LENG_PER_SEGMENT;
	}

	HxLOG_Print("nMsgLength = %d\n", (int)nMsgLength);

	if (nTotalSegmentNum == 0)
	{
		HxLOG_Error("Invalid Total Segment Number \n");
		return ERR_FAIL;
	}

	if (nTotalSegmentNum > MAX_POPUP_SEGMENT_NUM)
	{
		HxLOG_Error("nTotalSegmentNum > MAX_MAIL_SEGMENT_NUM, Set Maximun Value\n");
		nTotalSegmentNum = MAX_POPUP_SEGMENT_NUM;
	}

	// If Not Found Create New Mail Info
	pNewPopupInfo = NULL;
	nRes = local_irdCmd_Popup_FindPopupInfo(nPopupId, &pNewPopupInfo);
	if (nRes == ERR_OK && pNewPopupInfo == NULL)
	{
		TUnsignedInt8 i;
		static TUnsignedInt8 ucSaFlag[] = { 0x00, 0x80, 0xc0, 0xe0, 0xf0, 0xf8, 0xfc, 0xfe };

		//3// Create MAIL_INFO Data and Set Time
		nRes = local_irdCmd_Popup_GetNewPopupInfo(nPopupId, &pNewPopupInfo);
		if ( nRes != ERR_OK || pNewPopupInfo == NULL )
		{
			HxLOG_Error("pNewPopupInfo == NULL\n");
			return ERR_FAIL;
		}
		pNewPopupInfo->nTotalSegment			= nTotalSegmentNum;
		pNewPopupInfo->stPopupMsg.nPopupId		= nPopupId;
		pNewPopupInfo->stPopupMsg.nPersistence 	= nPersistence;
		pNewPopupInfo->pPopupSegmentHeader		= NULL;
		pNewPopupInfo->stPopupMsg.nStatus		= MSG_STATUS_UPDATING;

		i = 0;
		if ( pNewPopupInfo->nTotalSegment >= 8 )
		{
			for ( i=0 ; i<pNewPopupInfo->nTotalSegment / 8 ; i++ )
			{
				pNewPopupInfo->aulSegmentReceiveFlag[i] = 0xFF;
			}
		}
		if ( (pNewPopupInfo->nTotalSegment%8) > 0 )
		{
			pNewPopupInfo->aulSegmentReceiveFlag[i] = ucSaFlag[pNewPopupInfo->nTotalSegment%8];
		}
	}
	else
	{
		// If Found existing mail data check mail message is completed
		if ( pNewPopupInfo->stPopupMsg.nStatus == MSG_STATUS_COMPLETED )
		{
			HxLOG_Print("Notice!!! This Popup[id:%d] is already Processed!! Ignore this Msg!!\n", pNewPopupInfo->stPopupMsg.nPopupId);
			return ERR_FAIL;
		}
	}

	//3// Create New Segment Node
	HxLOG_Print("local_irdCmd_Popup_MakePopupMessage(%d) \n", nSegmentNum);
	nRes = local_irdCmd_Popup_CreatePopupSegment(nSegmentNum, &pNewSegment);
	if ( nRes == ERR_FAIL || pNewSegment == NULL )
	{
		HxLOG_Error("bRes == FALSE || pNewSegment == NULL\n");
		if (pNewSegment != NULL)
		{
			OxCAS_Free(pNewSegment);
			pNewSegment = NULL;
		}
		return ERR_FAIL;
	}

	HxSTD_memset(pNewSegment->szMsg, 0, MAX_POPUP_LENG_PER_SEGMENT);
	memcpy(pNewSegment->szMsg, pCmd, nMsgLength);
	pNewSegment->szMsg[nMsgLength] = '\0';

	HxLOG_Print("pNewPopupInfo->aulSegmentReceiveFlag[%d] = 0x%x\n", (nSegmentNum)/8, pNewPopupInfo->aulSegmentReceiveFlag[(nSegmentNum)/8]);
	pNewPopupInfo->aulSegmentReceiveFlag[(nSegmentNum)/8] &= ~((0x80)>>(nSegmentNum%8));
	HxLOG_Print("pNewPopupInfo->aulSegmentReceiveFlag[%d] = 0x%x\n", (nSegmentNum)/8, pNewPopupInfo->aulSegmentReceiveFlag[(nSegmentNum)/8]);

	//3// Add New Node to List
	if ( pNewPopupInfo->pPopupSegmentHeader == NULL )
	{
		HxLOG_Print("Set Header Segment [%d]\n", pNewSegment->nSegmentNum);
		pNewPopupInfo->pPopupSegmentHeader = pNewSegment;
	}
	else
	{
		TBoolean		bFound = FALSE;
		POPUP_SEGMENT	*pPrev = NULL, *pSegment = NULL;

		pSegment = pPrev = pNewPopupInfo->pPopupSegmentHeader;
		while ( pSegment )
		{
			if ( pSegment->nSegmentNum >= pNewSegment->nSegmentNum )
			{
				if ( pSegment->nSegmentNum == pNewSegment->nSegmentNum )
				{
					HxLOG_Print("Same Segment Exist!! nSegmentNum : %d\n", pNewSegment->nSegmentNum);
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

				if ( pSegment == pPrev && pPrev == pNewPopupInfo->pPopupSegmentHeader )
				{
					pNewPopupInfo->pPopupSegmentHeader = pNewSegment;
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

#if defined(IRDCMD_POPUP_DBG)
		pSegment = pPrev = pNewPopupInfo->pPopupSegmentHeader;
		while( pSegment )
		{
			HxLOG_Print("Segment Address [0x%x], Segment[%d].message : [%s]\n", pSegment, pSegment->nSegmentNum, pSegment->szMsg);
			pSegment = pSegment->pNext;
		}
#endif
	}

	// TODO: Create POPUP_SEGMENT DATA
	{
		TUnsignedInt8	i, nNumArr;
		TBoolean		bCompleted = TRUE;

		nNumArr = (pNewPopupInfo->nTotalSegment+7)/8;

#if defined(IRDCMD_POPUP_DBG)
		HxLOG_Print("Check Array Num = [%d] \n", nNumArr);
		HxLOG_Print("pNewPopupInfo->aulSegmentReceiveFlag ");
		for ( i = 0 ; i < nNumArr ; i++ )
		{
			HxLOG_Print("[0x%02x]", pNewPopupInfo->aulSegmentReceiveFlag[i]);
		}
		HxLOG_Print("\n");
#endif
		for ( i = 0 ; i < nNumArr ; i++ )
		{
			if ( pNewPopupInfo->aulSegmentReceiveFlag[i] != 0 )
			{
				bCompleted = FALSE;
				break;
			}
		}
		HxLOG_Error("bCompleted = [%s]\n", (bCompleted) ? "TRUE" : "FALSE");

		nRes = ERR_OK;
		if ( bCompleted )
		{
			TUnsignedInt16	nDeletedPopupId = INVALID_POPUP_ID;
			if ( local_irdCmd_Popup_DeleteOldMessage(&nDeletedPopupId) == ERR_OK )
			{
				//*pnDeletedPopupId = nDeletedPopupId;
				HxLOG_Print("Deleted Popup Id = [%d]\n", nDeletedPopupId);
				if ( local_irdCmd_Popup_JoinPopupMessage(pNewPopupInfo) == ERR_OK )
				{
					pNewPopupInfo->stPopupMsg.nStatus = MSG_STATUS_COMPLETED;

					*pnPopupId		= pNewPopupInfo->stPopupMsg.nPopupId;
					*pfCompleted	= TRUE;

					local_irdCmd_Popup_DisplayPopupMessage(pNewPopupInfo);
					nRes = ERR_OK;
				}
				else
				{
					*pnPopupId		= pNewPopupInfo->stPopupMsg.nPopupId;
					*pfCompleted	= FALSE;
					nRes = ERR_FAIL;
					HxLOG_Error("local_irdCmd_Popup_JoinPopupMessage() Fail!!!\n");
				}
			}
			else
			{
				*pnPopupId		= INVALID_POPUP_ID;
				*pfCompleted	= FALSE;
				nRes = ERR_FAIL;
				HxLOG_Error("local_irdCmd_Popup_DeleteOldMessage() Fail!!!\n");
			}
			if ( nRes != ERR_OK )
			{
				HxLOG_Print("local_irdCmd_Popup_DeletePopupMessage\n");
				local_irdCmd_Popup_DeletePopupMessage(pNewPopupInfo);
			}
		}
	}

	return nRes;
}


static HERROR local_irdCmd_Popup_FindPopupInfo(TUnsignedInt16 nPopupId, POPUP_INFO **ppPopupInfo)
{
	POINTER_LIST_T	*found, *current;
	POPUP_INFO		*pPopupInfo;

	if ( nPopupId == INVALID_POPUP_ID )
	{
		HxLOG_Info("nMailId == INVALID_MAIL_ID\n");
		return ERR_FAIL;
	}

	if ( ppPopupInfo == NULL )
	{
		HxLOG_Info("ppMailInfo == NULL\n");
		return ERR_FAIL;
	}

	*ppPopupInfo = NULL;

	found = NULL;
	current = s_pPopupQueue;
	while ( current )
	{
		pPopupInfo = (POPUP_INFO *)current->pvContents;
		if ( pPopupInfo )
		{
			if ( pPopupInfo->stPopupMsg.nPopupId == nPopupId )
			{
				HxLOG_Print("Found matching Popup Id [%d]\n", pPopupInfo->stPopupMsg.nPopupId);
				found = current;
				break;
			}
		}
		current = current->pNext;
	 	if ( current == s_pPopupQueue )
	 		break;
	}

	if ( found )
	{
		*ppPopupInfo = (POPUP_INFO *)found->pvContents;
	}
#if defined(IRDCMD_POPUP_DBG)
	else
	{
		HxLOG_Print("Not Found matching Popup Id : [%d]\n", nPopupId);
	}
#endif
	return ERR_OK;
}


static TSignedInt8 local_irdCmd_Popup_RankPopupDate(TUnsignedInt32 *varItem, TUnsignedInt32 *staticItem)
{
	POPUP_INFO *pVarPopupInfo, *pStaticPopupInfo;

	pVarPopupInfo = (POPUP_INFO *)varItem;
	pStaticPopupInfo = (POPUP_INFO *)staticItem;

	if ( pVarPopupInfo == NULL || pStaticPopupInfo == NULL )
	{
		return 1;
	}

	if ( pVarPopupInfo->stPopupMsg.nSystime <= pStaticPopupInfo->stPopupMsg.nSystime )
	{
		return 1;
	}
	return 0;
}


static TBoolean local_irdCmd_Popup_FilterProcessdMessage(void *varItem, void *filter)
{
	POPUP_INFO *pVarPopupInfo;

	pVarPopupInfo = (POPUP_INFO *)varItem;

	if ( pVarPopupInfo == NULL )
	{
		return FALSE;
	}

	if ( pVarPopupInfo->stPopupMsg.nStatus == MSG_STATUS_COMPLETED )
	{
		return TRUE;
	}
	return FALSE;
}


static void local_irdCmd_Popup_FreePopupData(void *pItem)
{
	local_irdCmd_Popup_FreePopupInfo((POPUP_INFO *)pItem);
}


static HERROR local_irdCmd_Popup_GetNewPopupInfo(TUnsignedInt16 nPopupId, POPUP_INFO **ppPopupInfo)
{
	POINTER_LIST_T	*pRoot;
	POPUP_INFO		*pNewPopupInfo = NULL;
	HERROR			hErr;

	if ( ppPopupInfo == NULL )
	{
		HxLOG_Info("ppPopupInfo == NULL\n");
		return ERR_FAIL;
	}

	*ppPopupInfo = NULL;

	//3// 새로운 공간을 Alloc한다.
	pNewPopupInfo = (POPUP_INFO *)OxCAS_Malloc(sizeof(POPUP_INFO));
	if ( pNewPopupInfo == NULL )
	{
		HxLOG_Info("pNewPopupInfo == NULL\n");
		return ERR_FAIL;
	}

	HxSTD_memset(pNewPopupInfo, 0x00, sizeof(POPUP_INFO));
	hErr = VK_CLOCK_GetTime(&pNewPopupInfo->stPopupMsg.nSystime);
	if (hErr != ERR_OK)
	{
		HxLOG_Info("VK_CLOCK_GetTime() fail\n");
		OxCAS_Free(pNewPopupInfo);
		return ERR_FAIL;
	}

	pRoot = UTIL_LINKEDLIST_InsertListItemToPointerList(s_pPopupQueue, (TUnsignedInt32 *)pNewPopupInfo, (RANK_FUNC)local_irdCmd_Popup_RankPopupDate);
	if ( pRoot == NULL )
	{
		HxLOG_Info("UTIL_InsertListItemToPointerList() Fail!!!\n");
		OxCAS_Free(pNewPopupInfo);
		return ERR_FAIL;
	}
	s_pPopupQueue = pRoot;
	*ppPopupInfo = pNewPopupInfo;
	return ERR_OK;
}


static HERROR local_irdCmd_Popup_FreePopupInfo(POPUP_INFO *pPopupInfo)
{
	if ( pPopupInfo == NULL )
	{
		return ERR_FAIL;
	}

	// Mail이 완전히 받아지지 않았을  경우 Alloc 했던 Mail Segment data를 Free 해야 한다.
	if ( pPopupInfo->stPopupMsg.nStatus != MSG_STATUS_COMPLETED )
	{
		POPUP_SEGMENT	*pPopupSegment = NULL, *pTmpSegment = NULL;

		pPopupSegment = pPopupInfo->pPopupSegmentHeader;
		while ( pPopupSegment )
		{
			pTmpSegment = pPopupSegment;
				pPopupSegment = pPopupSegment->pNext;
			if ( pTmpSegment != NULL)
			{
				OxCAS_Free(pTmpSegment);
				pTmpSegment = NULL;
			}
		}
		pPopupInfo->pPopupSegmentHeader = NULL;
	}

	HxSTD_memset(pPopupInfo, 0, sizeof(POPUP_INFO));
	OxCAS_Free(pPopupInfo);
	return ERR_OK;
}


static HERROR local_irdCmd_Popup_CreatePopupSegment(TUnsignedInt8 nSegmentNum, POPUP_SEGMENT **ppstPopupSegment)
{
	POPUP_SEGMENT *pPopupSegment = NULL;

	if ( ppstPopupSegment == NULL )
	{
		HxLOG_Info("ppstPopupSegment == NULL\n");
		return ERR_FAIL;
	}

	pPopupSegment = (POPUP_SEGMENT *)OxCAS_Malloc(sizeof(POPUP_SEGMENT));
	if ( pPopupSegment )
	{
		HxSTD_memset(pPopupSegment, 0, sizeof(POPUP_SEGMENT));
		pPopupSegment->nSegmentNum = nSegmentNum;
		*ppstPopupSegment = pPopupSegment;
		return ERR_OK;
	}

	*ppstPopupSegment = NULL;
	return ERR_FAIL;
}


static HERROR local_irdCmd_Popup_DeleteOldMessage(TUnsignedInt16 *pusDeletedPopupId)
{
	TUnsignedInt16 nNumList = 0;

	if ( pusDeletedPopupId )
	{
		*pusDeletedPopupId = INVALID_POPUP_ID;
	}

	nNumList = UTIL_LINKEDLIST_GetNumListItemsInPointerList(s_pPopupQueue, (FILTER_FUNC)local_irdCmd_Popup_FilterProcessdMessage, NULL);
	HxLOG_Print("Processed Item Number : [%d]\n", nNumList);

	//3// 제일 오래된 Message를 지우고
	if ( nNumList >= QUEUE_SIZE )
	{
		POINTER_LIST_T	*pToDelete, *root;
		POPUP_INFO		*pPopupToDelete = NULL;
		TUnsignedInt16	nPopupIdToDelete = INVALID_POPUP_ID;

#if defined(IRDCMD_POPUP_DBG)
		{
			POINTER_LIST_T	*current;
			POPUP_INFO		*pPopupInfo = NULL;

			HxLOG_Print("=======================================================\n");
			current = s_pPopupQueue;
			while ( current )
			{
				pPopupInfo = (POPUP_INFO *)current->contentsPointer;
				if ( pPopupInfo )
				{
					HxLOG_Print("PopuplId:[%d], SystemTime:[0x%x], nStatus:[%d], Persistence:[%d]\n",
							pPopupInfo->stPopupMsg.nPopupId, pPopupInfo->stPopupMsg.nSystime, pPopupInfo->stPopupMsg.nStatus,
							pPopupInfo->stPopupMsg.nPersistence));
				}
				current = current->nextPointer;
			 	if ( current == s_pPopupQueue )
			 		break;
			}
			HxLOG_Print("=======================================================\n\n");
		}
#endif
		HxLOG_Print("Find Oldest Saved Mail!\n");

		pToDelete = NULL;
		{
			POINTER_LIST_T	*current;
			POPUP_INFO		*pPopupInfo = NULL;
			TUnsignedInt32	nMinTime = (TUnsignedInt32)-1;

			current = s_pPopupQueue;
			while ( current )
			{
				pPopupInfo = (POPUP_INFO *)current->pvContents;
				if ( pPopupInfo )
				{
					if ( pPopupInfo->stPopupMsg.nStatus == MSG_STATUS_COMPLETED )
					{
						if ( nMinTime > pPopupInfo->stPopupMsg.nSystime )
						{
							nMinTime = pPopupInfo->stPopupMsg.nSystime;
							pToDelete = current;
						}
					}
				}
				current = current->pNext;
			 	if ( current == s_pPopupQueue )
			 		break;
			}
		}

		if ( pToDelete == NULL )
		{
			HxLOG_Info("pToDelete == NULL\n");
			return ERR_FAIL;
		}

		pPopupToDelete = (POPUP_INFO *)pToDelete->pvContents;
		if ( pPopupToDelete )
		{
			nPopupIdToDelete = pPopupToDelete->stPopupMsg.nPopupId;
			HxLOG_Print("Popup To Delete : Id : [%d], SystemTime [0x%x]\n", pPopupToDelete->stPopupMsg.nPopupId, pPopupToDelete->stPopupMsg.nSystime);
		}
		root = UTIL_LINKEDLIST_DeleteListItemFromPointerList((POINTER_LIST_T *)s_pPopupQueue, (POINTER_LIST_T *)pToDelete, (FREE_FUNC)local_irdCmd_Popup_FreePopupData) ;
		if ( root == NULL )
		{
			HxLOG_Info("UTIL_DeleteListItemFromPointerList() Fail!!!\n");
			return ERR_FAIL;
		}
		s_pPopupQueue = root;

		if ( pusDeletedPopupId )
		{
			*pusDeletedPopupId = nPopupIdToDelete;
		}
		return ERR_OK;
	}
	return ERR_OK;
}


static HERROR local_irdCmd_Popup_JoinPopupMessage(POPUP_INFO *pPopupInfo)
{
	TUnsignedInt8			*pszMsg = NULL;
	POPUP_SEGMENT	*pPopupSegment = NULL;
	POPUP_SEGMENT	*pTmpSegment = NULL;
	TUnsignedInt32			nStrLength = 0;

	if ( pPopupInfo == NULL )
	{
		HxLOG_Error("pPopupInfo == NULL\n");
		return ERR_FAIL;
	}

	HxSTD_memset(pPopupInfo->stPopupMsg.szText, 0, MAX_POPUP_LENGTH);
	pPopupInfo->stPopupMsg.nStrLen = 0;
	//3// Popup Segment Data를 모아서 하나의 Message를 만든다.
	//3// Free Popup msg Segment List
	pPopupSegment = pPopupInfo->pPopupSegmentHeader;
	pszMsg = (TUnsignedInt8 *)pPopupInfo->stPopupMsg.szText;
	while ( pPopupSegment )
	{
		pTmpSegment = pPopupSegment;
		nStrLength = svc_cas_MgrUtilStrLen((TUnsignedInt8 *)pPopupSegment->szMsg);
		HxLOG_Print("Segment[%d] Message Lenght = %d\n", pPopupSegment->nSegmentNum, nStrLength);
		memcpy(pszMsg, pPopupSegment->szMsg, nStrLength);
		pszMsg += nStrLength;
		pPopupInfo->stPopupMsg.nStrLen += nStrLength;
		pPopupSegment = pPopupSegment->pNext;
		if ( pTmpSegment != NULL)
		{
			OxCAS_Free(pTmpSegment);
		}
	}
	pszMsg[nStrLength] = '\0';

	HxLOG_Error("Popup Message [%s]\n", pPopupInfo->stPopupMsg.szText);

	pPopupInfo->pPopupSegmentHeader = NULL;
	HxSTD_memset(pPopupInfo->aulSegmentReceiveFlag, 0x00, sizeof(TUnsignedInt8)*8);
	return ERR_OK;
}


static HERROR local_irdCmd_Popup_DeletePopupMessage(POPUP_INFO *pPupupInfo)
{
	POINTER_LIST_T	*pToDelete, *current;
	POPUP_INFO		*pPopupInfoToDelete;

	if ( pPupupInfo == NULL )
	{
		HxLOG_Error("pPupupInfo == NULL\n");
		return ERR_FAIL;
	}

	// Delete Mail Message From Mail List
	pToDelete = NULL;
	current = s_pPopupQueue;
	while ( current )
	{
		pPopupInfoToDelete = (POPUP_INFO *)current->pvContents;
		if ( pPopupInfoToDelete )
		{
			if ( pPopupInfoToDelete->stPopupMsg.nPopupId == pPupupInfo->stPopupMsg.nPopupId )
			{
				HxLOG_Print("Popup : ID [%d]\n", pPopupInfoToDelete->stPopupMsg.nPopupId);
				pToDelete = current;
				break;
			}
		}
		current = current->pNext;
	 	if ( current == s_pPopupQueue )
	 		break;
	}

	if ( pToDelete == NULL )
	{
		HxLOG_Error("pToDelete == NULL\n");
		return ERR_FAIL;
	}

	s_pPopupQueue = UTIL_LINKEDLIST_DeleteListItemFromPointerList((POINTER_LIST_T *)s_pPopupQueue, (POINTER_LIST_T *)pToDelete, (FREE_FUNC)local_irdCmd_Popup_FreePopupData) ;
	return ERR_OK;
}


/**************************************************************************************************/
#define ________CA_CAK_IRDCMD_POPUP_Public_Functions_Body_________________________________
/**************************************************************************************************/
HERROR CA_NA_IRDCMD_POPUP_DisplayPopupMessage(void)
{
#if defined(CONFIG_MW_CAS_NAGRA_DALAPI_7_1_ABOVE)
	HxLOG_Error("Dummy Function. May be implemented\n");
#else
	TBdcStatus status;
	TBdcPopUpPersistence nPersistence;
	TBdcPopUpContent stPopupContent;
	TBdcIsoLanguage nLangCode = {"eng"};
	NA_POPUP_MSG stPopupMsg;

	HxSTD_memset( (void*)&stPopupMsg, 0, sizeof(NA_POPUP_MSG) );

	if (local_irdCmd_Popup_ReadPopupMessage(&stPopupMsg) != ERR_OK)
	{
		return ERR_FAIL;
	}

	if (stPopupMsg.nPopupId == INVALID_POPUP_ID)
	{
		return ERR_FAIL;
	}

	HxSTD_memset( (void*)&stPopupContent, 0, sizeof(TBdcPopUpContent) );

	nPersistence = BDC_POP_UP_PERSISTENCE_ALWAYS;

	memcpy(stPopupContent.language, nLangCode, sizeof(TBdcIsoLanguage));
	stPopupContent.numBytes = stPopupMsg.nStrLen;
	stPopupContent.pContent = (TUnsignedInt8*)stPopupMsg.szText;
#if defined(CONFIG_PROD_HGS1000S)
#else
	status = bdcDisplayPopUp(nPersistence, 1, &stPopupContent);
#endif
	if (status == BDC_NO_ERROR)
	{
		return ERR_OK;
	}
#endif
	return ERR_FAIL;
}


HERROR CA_NA_IRDCMD_POPUP_DeletePopupMessage(void)
{
	CASDEV_FS_UINT32	hFileHandle;
	CASDEV_FS_UINT32	ulWriteSize;
	TSignedInt32	nErrCode;
	TUnsignedInt8	ucFileName[128];
	NA_POPUP_MSG 	stPopupMsg;

	HxSTD_memset( (void*)&stPopupMsg, 0, sizeof(NA_POPUP_MSG) );

	if (local_irdCmd_Popup_ReadPopupMessage(&stPopupMsg) != ERR_OK)
	{
		return ERR_FAIL;
	}

	if (stPopupMsg.nPopupId == INVALID_POPUP_ID)
	{
		return ERR_FAIL;
	}

#if 0	// 앞에서 이미 ERROR 발생.
	if (svc_cas_DevFsIsDirExist(POPUP_JFFS2_PATH) != TRUE)
	{
		HxLOG_Error("Directory(%s) is not found.\n", POPUP_JFFS2_PATH);
		return ERR_FAIL;
	}
#endif

	HxSTD_snprintf(ucFileName, 128, "%21s%14s", POPUP_JFFS2_PATH, POPUP_FILE_NAME);

	nErrCode = svc_cas_DevFsOpen(0, ucFileName, PAL_FS_MODE_WRITE, &hFileHandle);
	if (nErrCode)
	{
		HxLOG_Error("error occurred in svc_cas_DevFsOpen(%d).\n", nErrCode);
		return ERR_FAIL;
	}

	HxSTD_memset(&stPopupMsg, 0, sizeof(stPopupMsg));

	stPopupMsg.nPopupId = INVALID_POPUP_ID;

	nErrCode = svc_cas_DevFsWrite(hFileHandle, (HUINT8 *)&stPopupMsg, sizeof(stPopupMsg), &ulWriteSize);
	if(nErrCode)
	{
		HxLOG_Error("error occurred in svc_cas_DevFsWrite(%d).\n", nErrCode);
		svc_cas_DevFsClose(hFileHandle);
		return ERR_FAIL;
	}

	svc_cas_DevFsClose(hFileHandle);
	return ERR_OK;
}


HERROR CA_NA_IRDCMD_POPUP_MakePopupMessage(TUnsignedInt8 *pxIrdCmd, TUnsignedInt16 *pnPopupId, TBoolean *pfCompleted)
{
	return local_irdCmd_Popup_MakePopupMessage(pxIrdCmd, pnPopupId, pfCompleted);
}


HERROR CA_NA_IRDCMD_POPUP_RemovePopupMessage(void)
{
	return local_irdCmd_Popup_RemovePopupMessage();
}

#if defined(CONFIG_DEBUG)
HERROR CA_NA_IRDCMD_POPUP_DisplayTestPopupAlways(void)
{
	TBdcStatus status;
	TBdcPopUpPersistence nPersistence;
	TBdcPopUpContent stPopupContent;
	TBdcIsoLanguage nLangCode = {"eng"};
	static HUINT8 szText[] = "Test BDC Display : BDC_POP_UP_PERSISTENCE_ALWAYS";

	HxSTD_memset( (void*)&stPopupContent, 0, sizeof(TBdcPopUpContent) );

	nPersistence = BDC_POP_UP_PERSISTENCE_ALWAYS;

	memcpy(stPopupContent.language, nLangCode, sizeof(TBdcIsoLanguage));
	stPopupContent.numBytes = strlen(szText);
	stPopupContent.pContent = (TUnsignedInt8 *)szText;
#if defined(CONFIG_PROD_HGS1000S)
#else
	status = bdcDisplayPopUp(nPersistence, 1, &stPopupContent);
#endif
	if (status == BDC_NO_ERROR)
	{
		return ERR_OK;
	}
	return ERR_FAIL;
}

HERROR CA_NA_IRDCMD_POPUP_DisplayTestPopupTimeOut(void)
{
	TBdcStatus status;
	TBdcPopUpPersistence nPersistence;
	TBdcPopUpContent stPopupContent;
	TBdcIsoLanguage nLangCode = {"eng"};
	static HUINT8 szText[] = "Test BDC Display : BDC_POP_UP_PERSISTENCE_TIMEOUT";

	HxSTD_memset( (void*)&stPopupContent, 0, sizeof(TBdcPopUpContent) );

	nPersistence = BDC_POP_UP_PERSISTENCE_TIMEOUT;

	memcpy(stPopupContent.language, nLangCode, sizeof(TBdcIsoLanguage));
	stPopupContent.numBytes = strlen(szText);
	stPopupContent.pContent = (TUnsignedInt8 *)szText;
#if defined(CONFIG_PROD_HGS1000S)
#else
	status = bdcDisplayPopUp(nPersistence, 1, &stPopupContent);
#endif
	if (status == BDC_NO_ERROR)
	{
		return ERR_OK;
	}
	return ERR_FAIL;
}

HERROR CA_NA_IRDCMD_POPUP_DisplayTestPopupUser(void)
{
	TBdcStatus status;
	TBdcPopUpPersistence nPersistence;
	TBdcPopUpContent stPopupContent;
	TBdcIsoLanguage nLangCode = {"eng"};
	static HUINT8 szText[] = "Test BDC Display : BDC_POP_UP_PERSISTENCE_USER";

	HxSTD_memset( (void*)&stPopupContent, 0, sizeof(TBdcPopUpContent) );

	nPersistence = BDC_POP_UP_PERSISTENCE_USER;

	memcpy(stPopupContent.language, nLangCode, sizeof(TBdcIsoLanguage));
	stPopupContent.numBytes = strlen(szText);
	stPopupContent.pContent = (TUnsignedInt8 *)szText;
#if defined(CONFIG_PROD_HGS1000S)
#else
	status = bdcDisplayPopUp(nPersistence, 1, &stPopupContent);
#endif
	if (status == BDC_NO_ERROR)
	{
		return ERR_OK;
	}
	return ERR_FAIL;
}
#endif


/**************************************************************************************************/
#define ________CA_OS_EXT_Private_Include_________________________________________________
/**************************************************************************************************/
#include "stdio.h"
#include "string.h"
#include "vkernel.h"
#include "na_glob.h"

#include <_svc_cas_mgr_nvram.h>
#include <_svc_cas_mgr_common.h>


/**************************************************************************************************/
#define ________CA_OS_EXT_Golbal_Value___________________________________________________
/**************************************************************************************************/

/**************************************************************************************************/
#define ________CA_OS_EXT_Private_Define__________________________________________________
/**************************************************************************************************/
#if defined(CONFIG_MW_CAS_NAGRA_DALTEST)
#define OsyExtLogTrace(message) 	//(HLIB_CMD_Printf(C_CODE_F_PURPLE"[CA OSY EXT](%s:%d)"C_CODE_RESET, __FUNCTION__, __LINE__), (HLIB_CMD_Printf message))
#define OsyExtLogError(message) 	//(HLIB_CMD_Printf(C_CODE_F_RED"[CA OSY EXT](%s:%d)"C_CODE_RESET, __FUNCTION__, __LINE__), (HLIB_CMD_Printf message))
#define OsyExtLogWarning(message) 	//(HLIB_CMD_Printf(C_CODE_F_RED_B_BLACK"[CA OSY EXT](%s:%d)"C_CODE_RESET, __FUNCTION__, __LINE__), (HLIB_CMD_Printf message))
#define OsyExtLogInfo(message)		//((HLIB_CMD_Printf message))


#if defined (HxLOG_Print)
#undef	HxLOG_Print
#define HxLOG_Print(...)	OsyExtLogTrace((__VA_ARGS__))
#endif

#if defined (HxLOG_Error)
#undef	HxLOG_Error
#define HxLOG_Error(...)	OsyExtLogError((__VA_ARGS__))
#endif

#if defined (HxLOG_Warning)
#undef	HxLOG_Warning
#define HxLOG_Warning(...)	OsyExtLogWarning((__VA_ARGS__))
#endif

#if defined (HxLOG_Info)
#undef	HxLOG_Info
#define HxLOG_Info(...)		OsyExtLogInfo((__VA_ARGS__))
#endif

#endif


/**************************************************************************************************/
#define ________CA_OS_EXT_Public_Functions_Body___________________________________________
/**************************************************************************************************/
Event_Handle_t CA_NA_OsEventCreate(void)
{
	Event_Handle_t 	hEvent = (Event_Handle_t)NULL;
	TSignedInt32		nErrCode = VK_OK;
	TUnsignedInt8		*ucQueueName[10];
	static TSignedInt32 s_ulQueueNum = 0;

	hEvent = (Event_Handle_t)OxCAS_Malloc(sizeof(Event_Instance_t));
	if (hEvent)
	{
		HxSTD_memset(hEvent, 0, sizeof (Event_Instance_t));
		HxSTD_snprintf((TChar*)ucQueueName, 16, "%4s%5d", (TUnsignedInt8*)EVENT_Q_NAME, (int)s_ulQueueNum++);

		nErrCode = VK_MSG_Create((TUnsignedInt32)EVENT_BUF_SIZE, (TUnsignedInt32)EVENT_DATA_SIZE, (TUnsignedInt8*)ucQueueName, (unsigned long*)&(hEvent->hQueue), VK_SUSPENDTYPE_PRIORITY);
		if (nErrCode != ERR_OK)
		{
			HxLOG_Error("error in VK_MSG_Create (0x%x)\n", (unsigned int)nErrCode);
			OxCAS_Free((void*)hEvent);
			return NULL;
		}
	}
	return hEvent;
}


HERROR CA_NA_OsEventDelete(Event_Handle_t hEvent)
{
	TSignedInt32 	nErrCode = VK_OK;
	TUnsignedInt8 	ucBuff[EVENT_BUF_SIZE];

	if (hEvent == NULL)
	{
		HxLOG_Error("target is null...\n");
		return ERR_FAIL;
	}

	while (1)
	{
		if ((nErrCode= VK_MSG_ReceiveTimeout((TUnsignedInt32)hEvent->hQueue, (void *)ucBuff, (TUnsignedInt32)EVENT_DATA_SIZE, (TUnsignedInt32)NO_WAIT)) != ERR_OK)
			break;
	}

	nErrCode = VK_MSG_Destroy(hEvent->hQueue);
	if (nErrCode != VK_OK)
	{
		HxLOG_Error("error in VK_MSG_Destroy (0x%x)\n", (unsigned int)nErrCode);
		return ERR_FAIL;
	}

	OxCAS_Free((void*)hEvent);
	hEvent = NULL;
	return ERR_OK;
}


HERROR CA_NA_OsEventSend(Event_Handle_t hEvent, TUnsignedInt32 ulEvent)
{
	TSignedInt32 		nErrCode = VK_OK;
	TUnsignedInt8 	ucBuff[EVENT_BUF_SIZE];

	if (hEvent == NULL)
	{
		HxLOG_Error("target is null...\n");
		return ERR_FAIL;
	}

	HxSTD_memcpy(ucBuff, &ulEvent, EVENT_DATA_SIZE);
#if (CONFIG_FOR_DAL_TEST_DEF == 1)
	nErrCode = VK_MSG_SendTimeout((TUnsignedInt32)hEvent->hQueue, (void *)ucBuff, (TUnsignedInt32)EVENT_DATA_SIZE, (TUnsignedInt32)WAIT_FOREVER);
#else
	nErrCode = VK_MSG_SendTimeout((TUnsignedInt32)hEvent->hQueue, (void *)ucBuff, (TUnsignedInt32)EVENT_DATA_SIZE, (TUnsignedInt32)5000);
#endif
	if (nErrCode != VK_OK)
	{
		HxLOG_Error("error in VK_MSG_SendTimeout (0x%x)\n", (unsigned int)nErrCode);
		return ERR_FAIL;
	}
	return ERR_OK;
}


HERROR CA_NA_OsEventReceive(Event_Handle_t hEvent, TUnsignedInt32 ulEventMask, TUnsignedInt32 *pulEvent,
					   					TUnsignedInt32 ulEventOpt, TUnsignedInt32 ulTimeout)
{
	TUnsignedInt32 	ulBuff;
	TSignedInt32		nErrCode;
	TUnsignedInt8 	ucBuff[EVENT_BUF_SIZE];

	if (hEvent == NULL || pulEvent == NULL)
	{
		HxLOG_Error("targets are null...\n");
		return ERR_FAIL;
	}

	*pulEvent = 0;
	if (ulEventOpt & eWait_Any_Event)
	{
		while (1)
		{
			if (ulEventOpt & eEvent_No_Wait)
			{
				nErrCode = VK_MSG_ReceiveTimeout((TUnsignedInt32)hEvent->hQueue, (void *)ucBuff, (TUnsignedInt32)EVENT_DATA_SIZE, (TUnsignedInt32)NO_WAIT);
				if (nErrCode != VK_OK)
				{
					if (hEvent->ulEvent & ulEventMask)
					{
						*pulEvent = (hEvent->ulEvent & ulEventMask);
						hEvent->ulEvent = (hEvent->ulEvent & ~ulEventMask);
					}
					HxLOG_Error("error in VK_MSG_ReceiveTimeout 1(0x%x)\n", (unsigned int)nErrCode);
#if 0//defined(CONFIG_MW_CAS_NAGRA_DALTEST)
					return ERR_OK;
#else
					return ERR_FAIL;
#endif
				}
			}
			else
			{
				if (ulTimeout == 0)
				{
					nErrCode = VK_MSG_ReceiveTimeout((unsigned long)hEvent->hQueue, (void *)ucBuff, (TUnsignedInt32)EVENT_DATA_SIZE, (TUnsignedInt32)WAIT_FOREVER);
					if (nErrCode != VK_OK)
					{
						HxLOG_Error("error in VK_MSG_ReceiveTimeout 2(0x%x)\n", (unsigned int)nErrCode);
						return ERR_FAIL;
					}
				}
				else
				{
					nErrCode = VK_MSG_ReceiveTimeout((unsigned long)hEvent->hQueue, (void *)ucBuff, (TUnsignedInt32)EVENT_DATA_SIZE, (TUnsignedInt32)ulTimeout+10);
					if (nErrCode != VK_OK)
					{
						if (hEvent->ulEvent & ulEventMask)
						{
							*pulEvent = (hEvent->ulEvent & ulEventMask);
							hEvent->ulEvent = (hEvent->ulEvent & ~ulEventMask);
						}
						HxLOG_Error("error in VK_MSG_ReceiveTimeout 3(0x%x)\n", (unsigned int)nErrCode);
						return ERR_FAIL;
					}
				}
			}

			while (1)
			{
				HxSTD_memcpy(&ulBuff, ucBuff, sizeof(TUnsignedInt32));

				hEvent->ulEvent |= ulBuff;
				nErrCode = VK_MSG_ReceiveTimeout((TUnsignedInt32)hEvent->hQueue, (void *)ucBuff, (TUnsignedInt32)EVENT_DATA_SIZE, (TUnsignedInt32)NO_WAIT);
				if (nErrCode != VK_OK)
				{
					HxLOG_Debug("error in VK_MSG_ReceiveTimeout 4(0x%x)\n", (unsigned int)nErrCode);
					break;
				}
			}

			if (hEvent->ulEvent & ulEventMask)
			{
				*pulEvent = (hEvent->ulEvent & ulEventMask);
				hEvent->ulEvent = (hEvent->ulEvent & ~ulEventMask);
				break;
			}
		}
		hEvent->ulEvent = (hEvent->ulEvent & ~ulEventMask);
	}

	return ERR_OK;
}


HERROR CA_NA_OsFlashRead(TSignedInt32 ulStorageId, TUnsignedInt32 ulOffset, TUnsignedInt32 ulSize, TUnsignedInt8 *pucData)
{
	CASDEV_FS_UINT32	hFileHandle;
	CASDEV_FS_INT64		nPosOffset;
	CASDEV_FS_UINT32	ulReadSize;
	TSignedInt32		nErrCode;
	TUnsignedInt8		ucFileName[128];

	if (pucData == NULL)
	{
		HxLOG_Error("target is null...\n");
		return ERR_FAIL;
	}

	HxLOG_Print("Offset[%d], Size[%d]\n", (int)ulOffset, (int)ulSize);
	if (svc_cas_DevFsIsDirExist(CAS_NA_JFFS2_PATH) != TRUE)
	{
		HxSTD_memset(pucData, 0xFF, ulSize);
		return ERR_OK;
	}

	HxSTD_snprintf(ucFileName, 128, "%21s%9s%d", CAS_NA_JFFS2_PATH, CAS_FILE_PERSISTENT_MEM, ulStorageId);
	if (svc_cas_DevFsIsFileExist(ucFileName) != TRUE)
	{
		HxSTD_memset(pucData, 0xFF, ulSize);
		return ERR_OK;
	}

	nErrCode = svc_cas_DevFsOpen(0, ucFileName, CASDEV_FS_MODE_READ, &hFileHandle);
	if (nErrCode)
	{
		HxLOG_Error("error occurred in svc_cas_DevFsOpen(%d).\n", (unsigned int)nErrCode);
		return ERR_FAIL;
	}

	nErrCode = svc_cas_DevFsLSeek(hFileHandle, ulOffset, CASDEV_FS_POS_START, &nPosOffset);
	if (nErrCode)
	{
		HxLOG_Error("error occurred in svc_cas_DevFsLSeek(%d).\n", (unsigned int)nErrCode);
		svc_cas_DevFsClose(hFileHandle);
		return ERR_FAIL;
	}

	nErrCode = svc_cas_DevFsRead(hFileHandle, pucData, ulSize, &ulReadSize);
	if (nErrCode)
	{
		HxLOG_Error("error occurred in svc_cas_DevFsRead(%d).\n", (unsigned int)nErrCode);
		svc_cas_DevFsClose(hFileHandle);
		return ERR_FAIL;
	}

	svc_cas_DevFsClose(hFileHandle);
	return ERR_OK;
}


HERROR CA_NA_OsFlashWrite(TSignedInt32 ulStorageId, TUnsignedInt32 ulOffset, TUnsignedInt32 ulSize, TUnsignedInt8 *pucData)
{
	CASDEV_FS_UINT32	hFileHandle;
	CASDEV_FS_INT64		nPosOffset;
	CASDEV_FS_UINT32	ulWrittenSize;
	TSignedInt32		nErrCode;
	TUnsignedInt8		ucFileName[128];

	if (pucData == NULL)
	{
		HxLOG_Error("target is null...\n");
		return ERR_FAIL;
	}

	HxLOG_Print("Offset[%d], Size[%d]\n", (int)ulOffset, (int)ulSize);

#if 0 //Nagra Init 과정에서 이미 check 했음
	if (svc_cas_DevFsIsDirExist(CAS_NA_JFFS2_PATH) != TRUE)
	{
		if (svc_cas_DevFsMkdir(CAS_NA_JFFS2_PATH) != ERR_OK)
		{
			HxLOG_Error("error occurred in svc_cas_DevFsMkdir\n");
			return ERR_FAIL;
		}
	}
#endif

	HxSTD_snprintf(ucFileName, 128, "%21s%9s%d", CAS_NA_JFFS2_PATH, CAS_FILE_PERSISTENT_MEM, ulStorageId);
	nErrCode = svc_cas_DevFsOpen(0, ucFileName, CASDEV_FS_MODE_WRITE, &hFileHandle);
	if (nErrCode)
	{
		HxLOG_Error("error occurred in svc_cas_DevFsOpen(%d).\n", (unsigned int)nErrCode);
		return ERR_FAIL;
	}

	nErrCode = svc_cas_DevFsLSeek(hFileHandle, ulOffset, CASDEV_FS_POS_START, &nPosOffset);
	if (nErrCode)
	{
		HxLOG_Error("error occurred in svc_cas_DevFsLSeek(%d).\n", (unsigned int)nErrCode);
		svc_cas_DevFsClose(hFileHandle);
		return ERR_FAIL;
	}

	nErrCode = svc_cas_DevFsWrite(hFileHandle, pucData, ulSize, &ulWrittenSize);
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


HERROR CA_NA_OsEepromRead(TSignedInt32 ulStorageId, TUnsignedInt32 ulOffset, TUnsignedInt32 ulSize, TUnsignedInt8 *pucData)
{
	HxLOG_Print("Offset[%d], Size[%d]\n", (int)ulOffset, (int)ulSize);

	if (ulStorageId == STORAGE_ID_0) // eeprom
	{
		if (svc_cas_DevPortNVRAMReadData(eDxCAS_GROUPID_NA, CA_NVRAM_CAS_NA+ulOffset, ulSize, pucData) == ERR_OK)
			return ERR_OK;
	}
#if !defined (CONFIG_MW_CAS_NAGRA_NOCS_3_X)
	else if (ulStorageId == STORAGE_ID_4) // NVRam
	{
		if (svc_cas_DevPortNVRAMReadData(eDxCAS_GROUPID_NA, CA_NVRAM_CAS_NA_CSCD_OFFSET+ulOffset, ulSize, pucData) == ERR_OK)
			return ERR_OK;
	}
#endif
	return ERR_FAIL;
}


HERROR CA_NA_OsEepromWrite(TSignedInt32 ulStorageId, TUnsignedInt32 ulOffset, TUnsignedInt32 ulSize, TUnsignedInt8 *pucData)
{
	HxLOG_Print("Offset[%d], Size[%d]\n", (int)ulOffset, (int)ulSize);

	if (ulStorageId == STORAGE_ID_0) // eeprom
	{
		if (svc_cas_DevPortNVRAMWriteData(eDxCAS_GROUPID_NA, CA_NVRAM_CAS_NA+ulOffset, ulSize, pucData) == ERR_OK)
			return ERR_OK;
	}
#if !defined (CONFIG_MW_CAS_NAGRA_NOCS_3_X)
	else if (ulStorageId == STORAGE_ID_4) // NVRam
	{
		if (svc_cas_DevPortNVRAMWriteData(eDxCAS_GROUPID_NA, CA_NVRAM_CAS_NA_CSCD_OFFSET+ulOffset, ulSize, pucData) == ERR_OK)
			return ERR_OK;
	}
#endif
	return ERR_FAIL;
}

/*
*	Description : Before CAK initialize, it would check the version field of the nvram.
*				If there is no version field or lower version than current, 
*				the data format of nvram will be converted to current version data format.
*/
HERROR CA_NA_OsCheckEepromVersion()
{
	HERROR nErr = ERR_OK;
	HUINT8 aucVersion[8] = {0,};
	HUINT32 ulVersionHeader = 0;
	HUINT32 ulVersionValue = 0;
	HUINT8* pucCurDataField = NULL;
	HUINT8* pucOldDataField = NULL;

	// 1. To get Version Field and check whether it is current version or not.
	nErr = svc_cas_DevPortNVRAMReadData(eDxCAS_GROUPID_NA, CA_NVRAM_CAS_NA_VERSION, CA_NVRAM_CAS_NA_VERSION_SIZE, aucVersion);
	if (nErr != ERR_OK)
	{
		HxLOG_Error("Can't get version filed from the NVRAM\n");
		goto FUNCTION_EXIT;
	}
	
	ulVersionHeader = (aucVersion[0] << 24) | (aucVersion[1] << 16) | (aucVersion[2] << 8) | (aucVersion[3]);
	ulVersionValue = (aucVersion[4] << 24) | (aucVersion[5] << 16) | (aucVersion[6] << 8) | (aucVersion[7]);

	HxLOG_Print("Version Field Data--------------------\n");
	HLIB_LOG_Dump(aucVersion, 8, 0, TRUE);
	HxLOG_Print("Header[%x] Version[%x]\n",ulVersionHeader, ulVersionValue);
	if(ulVersionHeader == CA_NVRAM_CAS_NA_VERSION_HEADER && ulVersionValue == CA_NVRAM_CAS_NA_VERSION_VALUE)
	{
		HxLOG_Print("It's not necessary to convert NVRAM data format\n");
		goto FUNCTION_EXIT;
	}

	// 2.  To read old data format from NVARAM
	pucOldDataField = (HUINT8*)OxCAS_Malloc(CA_NVRAM_CAS_NA_CSCD_END_OLD);
	if(pucOldDataField == NULL)
	{
		HxLOG_Error("Can't allocate memory for old data format\n");
		nErr = ERR_FAIL;
		goto FUNCTION_EXIT;
	}
	HxSTD_memset(pucOldDataField, 0xFF, CA_NVRAM_CAS_NA_CSCD_END_OLD);

	nErr = svc_cas_DevPortNVRAMReadData(eDxCAS_GROUPID_NA, CA_NVRAM_CAS_NA_OLD, CA_NVRAM_CAS_NA_CSCD_END_OLD, pucOldDataField);
	if (nErr != ERR_OK)
	{
		HxLOG_Error("Can't get old data filed from the NVRAM\n");
		goto FUNCTION_EXIT;
	}
	
	// 3. To make current version data format.
	pucCurDataField = (HUINT8*)OxCAS_Malloc(CA_NVRAM_CAS_NA_END_OFFSET);
	if(pucCurDataField == NULL)
	{
		HxLOG_Error("Can't allocate memory for new data format\n");
		nErr = ERR_FAIL;
		goto FUNCTION_EXIT;
	}
	HxSTD_memset(pucCurDataField, 0xFF, CA_NVRAM_CAS_NA_END_OFFSET);

	// 4. Make version field.
	aucVersion[0] = (CA_NVRAM_CAS_NA_VERSION_HEADER >> 24) & 0xFF;
	aucVersion[1] = (CA_NVRAM_CAS_NA_VERSION_HEADER >> 16) & 0xFF;
	aucVersion[2] = (CA_NVRAM_CAS_NA_VERSION_HEADER >> 8) & 0xFF;
	aucVersion[3] = CA_NVRAM_CAS_NA_VERSION_HEADER & 0xFF;

	aucVersion[4] = (CA_NVRAM_CAS_NA_VERSION_VALUE >> 24) & 0xFF;
	aucVersion[5] = (CA_NVRAM_CAS_NA_VERSION_VALUE >> 16) & 0xFF;
	aucVersion[6] = (CA_NVRAM_CAS_NA_VERSION_VALUE >> 8) & 0xFF;
	aucVersion[7] = CA_NVRAM_CAS_NA_VERSION_VALUE & 0xFF;
	HxSTD_memcpy(pucCurDataField+CA_NVRAM_CAS_NA_VERSION, aucVersion, CA_NVRAM_CAS_NA_VERSION_SIZE);

	// 5. Copy NAGRA Data
	HxSTD_memcpy(pucCurDataField+CA_NVRAM_CAS_NA, pucOldDataField + CA_NVRAM_CAS_NA_OLD, CA_NVRAM_CAS_NA_SIZE_OLD);

	// 6. Copy PIN Information Data
	HxSTD_memcpy(pucCurDataField+CA_NVRAM_PIN_INFORMATION_OFFSET, pucOldDataField + CA_NVRAM_PIN_INFORMATION_OFFSET_OLD, CA_NVRAM_PIN_INFORMATION_SIZE_OLD);

	// 7. Copy SEQ Number Data
	HxSTD_memcpy(pucCurDataField+CA_NVRAM_IRDCMD_SEQNUM_OFFSET, pucOldDataField + CA_NVRAM_IRDCMD_SEQNUM_OFFSET_OLD, CA_NVRAM_IRDCMD_SEQNUM_SIZE_OLD);

#if defined(CONFIG_MW_CAS_NAGRA_IRDCMD_MAIL)
	// 8. Copy Mail Data
	HxSTD_memcpy(pucCurDataField+CA_NVRAM_IRDCMD_MAIL_SEQNUM_OFFSET, pucOldDataField + CA_NVRAM_IRDCMD_MAIL_SEQNUM_OFFSET_OLD, CA_NVRAM_IRDCMD_MAIL_SEQNUM_SIZE_OLD);
	HxSTD_memcpy(pucCurDataField+CA_NVRAM_IRDCMD_MAIL_HEADER_OFFSET, pucOldDataField + CA_NVRAM_IRDCMD_MAIL_HEADER_OFFSET_OLD, CA_NVRAM_IRDCMD_MAIL_HEADER_SIZE_OLD);
#endif

#if defined(CONFIG_MW_CAS_NAGRA_IRDCMD_POPUP)
	// 9. Copy popup Data
	HxSTD_memcpy(pucCurDataField+CA_NVRAM_IRDCMD_POPUP_SEQNUM_OFFSET, pucOldDataField + CA_NVRAM_IRDCMD_POPUP_SEQNUM_OFFSET_OLD, CA_NVRAM_IRDCMD_POPUP_SEQNUM_SIZE_OLD);
#endif

#if defined(CONFIG_MW_CAS_NAGRA_IRDCMD_EMMWAKEUP)
	// 10. Copy EMMWakeup Data
	HxSTD_memcpy(pucCurDataField+CA_NVRAM_IRDCMD_EMMWAKEUP_OFFSET, pucOldDataField + CA_NVRAM_IRDCMD_EMMWAKEUP_OFFSET_OLD, CA_NVRAM_IRDCMD_EMMWAKEUP_SIZE_OLD);
#endif

#if defined(CONFIG_MW_CAS_NAGRA_OP_SES)
	// 11. Copy SES Data......... I hate SES option. If I get a chance to remove it, I would definitely remove this option.
	HxSTD_memcpy(pucCurDataField+CA_NVRAM_IRDCMD_MAGICCODE_OFFESET, pucOldDataField + CA_NVRAM_IRDCMD_MAGICCODE_OFFESET_OLD, CA_NVRAM_IRDCMD_MAGICCODE_SIZE_OLD);
	HxSTD_memcpy(pucCurDataField+CA_NVRAM_IRDCMD_CUR_PROFILE_OFFESET, pucOldDataField + CA_NVRAM_IRDCMD_CUR_PROFILE_OFFESET_OLD, CA_NVRAM_IRDCMD_CUR_PROFILE_SIZE_OLD);
	HxSTD_memcpy(pucCurDataField+CA_NVRAM_IRDCMD_PROFILE_OFFESET, pucOldDataField + CA_NVRAM_IRDCMD_PROFILE_OFFESET_OLD, CA_NVRAM_IRDCMD_PROFILE_SIZE_OLD);
	HxSTD_memcpy(pucCurDataField+CA_NVRAM_IRDCMD_MESSAGE_OFFSET, pucOldDataField + CA_NVRAM_IRDCMD_MESSAGE_OFFSET_OLD, CA_NVRAM_IRDCMD_MESSAGE_SIZE_OLD);
#endif

#if defined(CONFIG_MW_CAS_NAGRA_OP_RCS)
	// 12. Copy RCS Data
	HxSTD_memcpy(pucCurDataField+CA_NVRAM_CREDIT_INFO_OFFSET, pucOldDataField + CA_NVRAM_CREDIT_INFO_OFFSET_OLD, CA_NVRAM_CREDIT_INFO_SIZE_OLD);
#endif


	// 13. Copy CSC Data
#if defined (CONFIG_MW_CAS_NAGRA_NOCS_3_X)
	nErr = CA_NA_OsFlashWrite((TSignedInt32)STORAGE_ID_4, 0, CA_NVRAM_CAS_NA_CSCD_SIZE_OLD, pucOldDataField + CA_NVRAM_CAS_NA_CSCD_OFFSET_OLD);
	if(nErr != ERR_OK)
	{
		HxLOG_Error("Can't write CSC data to Flash\n");
		// Although it was failed to write the csc to the flash, keep going to write the current data field to NVRAM.
	}
#else
	HxSTD_memcpy(pucCurDataField+CA_NVRAM_CAS_NA_CSCD_OFFSET, pucOldDataField + CA_NVRAM_CAS_NA_CSCD_OFFSET_OLD, CA_NVRAM_CAS_NA_CSCD_SIZE_OLD);
#endif

	// 14. Write Current Data Format to NVRAM
	nErr = svc_cas_DevPortNVRAMWriteData(eDxCAS_GROUPID_NA, CA_NVRAM_CAS_NA_VERSION, CA_NVRAM_CAS_NA_END_OFFSET, pucCurDataField);
	if (nErr != ERR_OK)
	{
		HxLOG_Error("Can't write current version data format to the NVRAM\n");
		goto FUNCTION_EXIT;
	}

FUNCTION_EXIT:

	if(pucOldDataField != NULL)
	{
		OxCAS_Free(pucOldDataField);
		pucOldDataField= NULL;
	}
	
	if(pucCurDataField != NULL)
	{
		OxCAS_Free(pucCurDataField);
		pucCurDataField= NULL;
	}
	
	return nErr;
}



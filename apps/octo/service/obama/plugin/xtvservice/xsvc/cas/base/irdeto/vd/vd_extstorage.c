
#include <pal_sc.h>

#include <_svc_cas_sub_dev_api.h>

#include "vd_humax.h"

#include "ir_platform.h"
#include "ir_svc.h"
#include "ir_main.h"

#include "s3_vd_external_storage.h"
#if defined(IRPLAT_SYS_PVR)
#include <svc_fs.h>
#include <svc_pvr.h>
#endif



//#include "filemanager.h"

//	non sync mode file access 를 지원하는 모델.
#define	IREXT_NONSYNC

#if defined(IRPLAT_SYS_PVR)
/********************************************************************
*	Definitions (for this source file)
********************************************************************/





#define VD_NULL_FILE_HANDLE		0xFFFFFFFF

#if	defined(CONFIG_MW_CAS_IRDETO_PVR)
#define	EXT_SES_FILE						".ses"
#endif

#define EXT_SESSION_FILE	 	EXT_SES_FILE
/********************************************************************
*	External variables
********************************************************************/

/********************************************************************
*	External function prototypes
********************************************************************/

/********************************************************************
*	Global variables
********************************************************************/
HUMAX_VD_ExtStorage_st Humax_VD_ExtStorage[HVD_EXTSTORAGE_FILE_CNT_MAX];

/********************************************************************
*	Static variables
********************************************************************/
HUINT32	s_ulExtStorageAPISem;

/********************************************************************
*	Static functions Prototypes
*	The following functions will be used as inner functions
********************************************************************/
static	void	VDExtSDump(HINT32 nLength, HUINT8 *p)
{
	HUINT32 i;

	for(i=0;i<16;i++)
		HxLOG_Print(".%02X", p[i]);
	HxLOG_Print("\n");

	for( i = 0; i < nLength; i++ )
	{
		HxLOG_Print("%02X ", p[8+i]);
		if( i % 16 == 15 )
			HxLOG_Print("\n");
	}
	HxLOG_Print("\n");
}


/********************************************************************
*	function name :	 HUMAX_VD_ExtStorage_Initialise
*	input : void(none)
*	output : result
*	description : External storage관련하여 생성된 file들에 대한 정보를 가지고 있는 Humax_VD_ExtStorage라는 변수를
*			  HDD에서 읽어와서 copy해둠.
********************************************************************/
ia_result HUMAX_VD_ExtStorage_Initialise(void)
{
	HUINT32						i;
	ia_result					result = IA_SUCCESS;

	HxLOG_Debug("-->>\n");

	if( VK_SEM_Create((unsigned long*)&s_ulExtStorageAPISem, "s_ulExtStorageAPISem", VK_SUSPENDTYPE_FIFO ) != VK_OK )
	{
		HxLOG_Print("\t Init - Semaphore Create Error\n");
		return IA_FAIL;
	}

	for( i = 0; i < HVD_EXTSTORAGE_FILE_CNT_MAX; i++ )
	{
		HxSTD_memset(&Humax_VD_ExtStorage[i], 0, sizeof(HUMAX_VD_ExtStorage_st));

		Humax_VD_ExtStorage[i].referenceID = VD_REF_ID_ERROR;
		Humax_VD_ExtStorage[i].resourceID = VD_RSC_ID_INVALID;
		Humax_VD_ExtStorage[i].ulFileHandle = VD_NULL_FILE_HANDLE;
	}

	HxLOG_Debug("<<--\n");
	return result;
}

#define PVR_HIDDEN_CRID_DIR_NAME					".crid"

/********************************************************************
*	function name :	 HUMAX_VD_ExtStorage_Open
*	input : wResourceID / callback
*		   GetResource를 사용안하므로 resource ID는 항상 0일 것인가...
*	output : reference ID
*		   Open이 불리울 때 마다 file을 하나 생성하고 reference ID를 다르게 할당.
-->resourceID에 해당하는 파일을 open.
*	description : pvData를 통해서 callback function이 오게 되고, 이 callback function을 통해서
*			   smartcard in/out/error 등의 event를 softcell에 알려준다.
********************************************************************/
vd_ref_id HUMAX_VD_ExtStorage_Open( vd_rsc_id wResourceID, void *pvData )
{
	HUINT8 						szFileName[FM_FS_MAX_FILE_NAME_LEN + 16];
	HUINT32						res, ulFileHandle, i;
	vd_es_open_st				*es_open_st;

	SvcMeta_Record_t				stRecInfo;
	IR_SERVICE_INFO				*pstIrSvcInfo;

	HUINT32 						ucFileMode;

	HxLOG_Debug("Resource ID=-0x%X-->\n",(unsigned int) wResourceID);

	ucFileMode = 0;
	VK_SEM_Get(s_ulExtStorageAPISem);

	es_open_st = (vd_es_open_st *)pvData;

	HxLOG_Print("\twRecordId=0x%X\n",(unsigned int) es_open_st->wRecordId);
	HxLOG_Print("\teAccessMode=0x%X\n",(unsigned int) es_open_st->eAccessMode);


	// 빈 reference ID를 하나 선택.
	for( i = 0; i < HVD_EXTSTORAGE_FILE_CNT_MAX; i++ )
	{
		if( Humax_VD_ExtStorage[i].referenceID == VD_REF_ID_ERROR )
			break;
	}

	if( i == HVD_EXTSTORAGE_FILE_CNT_MAX )
	{
		HxLOG_Print(" Open - ERROR no more empty reference ID space\n");
		VK_SEM_Release(s_ulExtStorageAPISem);
		return VD_REF_ID_ERROR;
	}

	HxLOG_Print(" Open -  referenceId=%d.\n", i);

	Humax_VD_ExtStorage[i].referenceID = i;
	Humax_VD_ExtStorage[i].resourceID = wResourceID;

	Humax_VD_ExtStorage[i].es_open_st.pfnErrorNotify = es_open_st->pfnErrorNotify;
	Humax_VD_ExtStorage[i].es_open_st.wSoftCellVal = es_open_st->wSoftCellVal;
	Humax_VD_ExtStorage[i].es_open_st.eAccessMode = es_open_st->eAccessMode;
	Humax_VD_ExtStorage[i].es_open_st.wRecordId = es_open_st->wRecordId;

	switch(Humax_VD_ExtStorage[i].es_open_st.eAccessMode)
	{
		case VD_ES_ACCESS_MODE_READONLY:
			ucFileMode = PAL_FS_MODE_READ;
			break;
		case VD_ES_ACCESS_MODE_WRITEONLY:
			ucFileMode = PAL_FS_MODE_WRITE;
			break;
		case VD_ES_ACCESS_MODE_READWRITE:
			ucFileMode = PAL_FS_MODE_RDWR;
			break;
	}

#ifdef	IREXT_NONSYNC
	//	non-sync mode
	ucFileMode |= PAL_FS_MODE_NONBLOCK;
#endif

	pstIrSvcInfo = CAS_IR_DRV_FindIrSvcInfoBySCELLHandle(wResourceID);
	if (pstIrSvcInfo == NULL)
	{
		HxLOG_Print("Can't find SCELL Handle(%x) in IrSvcInfo List\n",(unsigned int) wResourceID);
		// open 실패 시 할당한 reference ID 회수.
		HxSTD_memset(&Humax_VD_ExtStorage[i], 0x00, sizeof(HUMAX_VD_ExtStorage_st));
		Humax_VD_ExtStorage[i].referenceID = VD_REF_ID_ERROR;
		Humax_VD_ExtStorage[i].resourceID = VD_RSC_ID_INVALID;
		Humax_VD_ExtStorage[i].ulFileHandle = VD_NULL_FILE_HANDLE;

		VK_SEM_Release(s_ulExtStorageAPISem);
		return VD_REF_ID_ERROR;
	}

	res = SVC_META_GetRecordInfo(pstIrSvcInfo->ulContentsId, &stRecInfo);
	if ( ERR_OK != res)
	{
		HxLOG_Print("SVC_META_GetRecordInfo Fail!!! - [%d]\n",(unsigned int) wResourceID);

		// open 실패 시 할당한 reference ID 회수.
		HxSTD_memset(&Humax_VD_ExtStorage[i], 0x00, sizeof(HUMAX_VD_ExtStorage_st));
		Humax_VD_ExtStorage[i].referenceID = VD_REF_ID_ERROR;
		Humax_VD_ExtStorage[i].resourceID = VD_RSC_ID_INVALID;
		Humax_VD_ExtStorage[i].ulFileHandle = VD_NULL_FILE_HANDLE;

		VK_SEM_Release(s_ulExtStorageAPISem);
		return VD_REF_ID_ERROR;
	}
	// prevent fix
	snprintf(szFileName, sizeof(szFileName),"%s%s%s.%08X", stRecInfo.aucPath, stRecInfo.aucFileName, EXT_SESSION_FILE, (unsigned int)es_open_st->wRecordId);


	//	TSR은 무시함.
	//	Record / PB시에도 CRID가 같은 것은 하나의 Directory에 존재한다는 가정으로 구현함.
	//	실제는 안 그럴수 있지만이 부분 고려하기에는 현재는 어려움.
#if	defined(IRPLAT_PVR_DRM)
	//	Step 1 : Check PVR Mode : Playback or Record.
	HxLOG_Info("VD_IO - PVR Mode : [%x]\n", pstIrSvcInfo->eCasActionType);
	//	Chase Playback이 TSR Play로 내려오네...
	if ((pstIrSvcInfo->eCasActionType == eCAS_ActionType_Playback) || (pstIrSvcInfo->eCasActionType == eCAS_ActionType_Tsr_Play))
	{
		HUINT8		szCridName[FM_FS_MAX_FILE_NAME_LEN];

		HxLOG_Info("Check TSR Mode - [%s]/[%s]\n", stRecInfo.aucPath, PAL_PVR_HIDDEN_TSR_DIR_NAME);
		if (strstr(stRecInfo.aucPath, PAL_PVR_HIDDEN_TSR_DIR_NAME) == NULL)
		{
			HxLOG_Info("VD_IO - Playback Mode!\n");
			//	If the ext file is for RR Data?
			if (es_open_st->wRecordId == pstIrSvcInfo->pstPVRSvcInfo->stPVRDrmInfo.stDrmBasicInfo.ulRightRecId)
			{
				HBOOL		bFound = FALSE;
				HUINT32		ulRet;
				HUINT8		*drivePath = NULL, aucBuf[FM_FS_MAX_FILE_NAME_LEN];

				if( CARM_PVR_GetAvailableRecordPath(ePvr_Action_NONE, &drivePath) == ERR_OK )
				{
					if(drivePath == NULL)
					{
						VDExtStoError(DBG_ASSERT, ("fail to Get Record Path \n "));
						// open 실패 시 할당한 reference ID 회수.
						HxSTD_memset(&Humax_VD_ExtStorage[i], 0x00, sizeof(HUMAX_VD_ExtStorage_st));
						Humax_VD_ExtStorage[i].referenceID = VD_REF_ID_ERROR;
						Humax_VD_ExtStorage[i].resourceID = VD_RSC_ID_INVALID;
						Humax_VD_ExtStorage[i].ulFileHandle = VD_NULL_FILE_HANDLE;

						VK_SEM_Release(s_ulExtStorageAPISem);
						return VD_REF_ID_ERROR;
					}

					//slash is included in drivepath.
					snprintf(aucBuf, FM_FS_MAX_FILE_NAME_LEN, "%s%s", drivePath, PVR_HIDDEN_CRID_DIR_NAME);


					bFound = CARM_PVR_IsDirExist(aucBuf);
					if (bFound == FALSE)
					{
						ulRet = CARM_PVR_MakeDir(aucBuf);
						if (ulRet != ERR_OK/*PAL_PVR_OK*/)
						{
							HxLOG_Error("fail to make directory(%s) for CRID: 0x%x\n", aucBuf, ulRet);
						}
						else
						{
							HxLOG_Info("make directory for CRID : %s\n", aucBuf);
						}
					}

					HxLOG_Info("\tAccess RR File [0x%08X]\n", (unsigned int) es_open_st->wRecordId);
					//	주어진 CRID로 생성된 RR File이 존재하는가?
#if 0
					sprintf(szCridName, "%s0x%08X.crid", stRecInfo.aucPath, pstIrSvcInfo->pstPVRSvcInfo->stPVRDrmInfo.stDrmRightRecInfo.ulCRID);
#else
					snprintf(szCridName, FM_FS_MAX_FILE_NAME_LEN, "%s/%s0x%08X.crid", aucBuf, stRecInfo.aucFileName, pstIrSvcInfo->pstPVRSvcInfo->stPVRDrmInfo.stDrmRightRecInfo.ulCRID);
#endif
					if (CARM_PVR_IsFileExist(szCridName) == FALSE)
					{
						HxLOG_Info("\tCRID file is not existed!! --> Copy Crid File from RR Data\n");
						CARM_PVR_CopyFileSyncMode(szFileName, szCridName);

						// 위 함수는 실제 sync mode로 동작 하지 않는다. pal_fs_copySync를 사용하니 더 많은 문제 발생 그래서 궁여 지책으로 아래와 같이 확인 하고 넘어 간다.
						// 확인을 하지 않으면 Message send로 copy 해 놓고 바로 아래에서 read 해 버려서 error 발생 해서 playback 실패가 되어 버림.
						{
							int jj;


							for(jj = 0; jj < 10; jj++)
							{
								res = CARM_PVR_OpenFile(ucFileMode, szCridName, &ulFileHandle);
								if(res == ERR_OK)
								{
									CARM_PVR_CloseFile(ulFileHandle);
									break;
								}
								VK_TASK_Sleep(100);
							}
						}
					}
					strncpy(szFileName, szCridName, FM_FS_MAX_FILE_NAME_LEN);
					HxLOG_Info("\tNew CRID File Name is [%s]\n", szFileName);
				}

			}
		}
	}
#endif

	res = CARM_PVR_OpenFile(ucFileMode,  szFileName, &ulFileHandle);
	HxLOG_Info("[%d][%s] --> Handle [%d] - [%d]\n", res, szFileName, ulFileHandle, ucFileMode);
	if ( ERR_OK != res)
	{
		HxLOG_Print(" CARM_PVR_OpenFile Failed, %s\n", szFileName);

		// open 실패 시 할당한 reference ID 회수.
		HxSTD_memset(&Humax_VD_ExtStorage[i], 0x00, sizeof(HUMAX_VD_ExtStorage_st));
		Humax_VD_ExtStorage[i].referenceID = VD_REF_ID_ERROR;
		Humax_VD_ExtStorage[i].resourceID = VD_RSC_ID_INVALID;
		Humax_VD_ExtStorage[i].ulFileHandle = VD_NULL_FILE_HANDLE;

		VK_SEM_Release(s_ulExtStorageAPISem);
		return res;
	}

	Humax_VD_ExtStorage[i].ulFileHandle = ulFileHandle;

	VK_SEM_Release(s_ulExtStorageAPISem);
	HxLOG_Debug("<<--\n");
	return i;
}

/********************************************************************
*	function name :	 HUMAX_VD_ExtStorage_Close
*	input : wRefId
*	output : result
*	description :
********************************************************************/
ia_result HUMAX_VD_ExtStorage_Close( vd_ref_id wRefId )
{
	HxLOG_Debug("(RefId= 0x%X -->>\n", wRefId);

	if (wRefId >= HVD_EXTSTORAGE_FILE_CNT_MAX)
	{
		HxLOG_Print("[HUMAX_VD_ExtStorage_Close] error wRefId(%d)\n", wRefId);
		return IA_FAIL;
	}
	VK_SEM_Get(s_ulExtStorageAPISem);
	if (VD_NULL_FILE_HANDLE != Humax_VD_ExtStorage[wRefId].ulFileHandle)
	{
		CARM_PVR_CloseFile(Humax_VD_ExtStorage[wRefId].ulFileHandle);
	}

	HxSTD_memset(&Humax_VD_ExtStorage[wRefId], 0, sizeof(HUMAX_VD_ExtStorage_st));

	Humax_VD_ExtStorage[wRefId].referenceID = VD_REF_ID_ERROR;
	Humax_VD_ExtStorage[wRefId].resourceID = VD_RSC_ID_INVALID;
	Humax_VD_ExtStorage[wRefId].ulFileHandle = VD_NULL_FILE_HANDLE;
	HxLOG_Debug("<<--\n");
	VK_SEM_Release(s_ulExtStorageAPISem);

	return IA_SUCCESS;
}

/********************************************************************
*	function name :	 HUMAX_VD_ExtStorage_IO
*	input : wRefId
*	output : result
*	description : Open할 때 등록된 callback함수를 close.
********************************************************************/
extern	HUINT32 HUMAX_VD_ExtStorage_DelayWrite(vd_ref_id wRefId, ia_word32 ulStart, ia_word16 ulLen, void *pucData);

ia_result HUMAX_VD_ExtStorage_IO( vd_ref_id wRefId, ia_word16 wOpCode, ia_word32 wDataWriteSize, void *pvDataWrite, ia_word32 *pwDataReadSize, void *pvDataRead )
{
	HUINT64  		ullFilePos;
	HUINT8		*p;
	HUINT32		wStart, wLength;

#if !defined(IREXT_NONSYNC)
	CAS_IR_BASE_MSG 	*pMsg;
#endif

	HUINT32		res, ulFileHandle, sizeToRead, ulSize;
	ia_result		result = IA_SUCCESS;
#if defined(CONFIG_OP_DIGITURK)//#Bugzilla 2161
	IR_SERVICE_INFO 			*pstIrSvcInfo;
	ia_word32				wRecordId;
#endif

	HxLOG_Debug("-->>\n");

	if (wRefId >= HVD_EXTSTORAGE_FILE_CNT_MAX)
	{
		HxLOG_Print("[HUMAX_VD_ExtStorage_IO] error wRefId(%d)\n", wRefId);
		return IA_FAIL;
	}
#if defined(CONFIG_OP_DIGITURK)
		pstIrSvcInfo = CAS_IR_DRV_FindIrSvcInfoBySCELLHandle(Humax_VD_ExtStorage[wRefId].resourceID); //#Bugzilla 2161
#endif

	VK_SEM_Get(s_ulExtStorageAPISem);

	ulFileHandle = Humax_VD_ExtStorage[wRefId].ulFileHandle;
	p = (HUINT8*)pvDataWrite;

	wStart = *(HUINT32*)p;
	wLength = *(HUINT32*)(p+4);

	switch(wOpCode)
	{
		case VD_ES_WRITE:
			HxLOG_Print(" Write - wRefId(0x%X), wOpCode(0x%X), wDataWriteSize(0x%X)\n", wRefId, wOpCode, wDataWriteSize);
			HxLOG_Print(" Write - wStart = 0x%X, wLength = 0x%X]\n", wStart, wLength);
			VDExtSDump(wLength, p);

#if defined (CONFIG_OP_DIGITURK) && defined(IRPLAT_PVR_DRM)//#Bugzilla 2161 -s
            wRecordId = Humax_VD_ExtStorage[wRefId].es_open_st.wRecordId;
            if(pstIrSvcInfo != NULL)
            {
                if(pstIrSvcInfo->pstPVRSvcInfo)
                {
                    if((pstIrSvcInfo->pstPVRSvcInfo->stPVRDrmInfo.stDrmBasicInfo.ulRightRecId != 0 )&&
                        (pstIrSvcInfo->pstPVRSvcInfo->stPVRDrmInfo.stDrmBasicInfo.ulSeqRecId != 0 ))
                    {
                        if(wRecordId == pstIrSvcInfo->pstPVRSvcInfo->stPVRDrmInfo.stDrmBasicInfo.ulRightRecId)
                        {
                            HxLOG_Print(" RR DATA IO Data CRID = [0x%08X]\n", *(HUINT32*)(p+16));
                            HxLOG_Print(" RR DATA IO Current CRID = [0x%08X]\n", pstIrSvcInfo->pstPVRSvcInfo->stPVRDrmInfo.stDrmRightRecInfo.ulCRID);
                            if (*(HUINT32*)(p+16) == 0)
                            {
                                break;
                            }
                        }
                        else if(wRecordId == pstIrSvcInfo->pstPVRSvcInfo->stPVRDrmInfo.stDrmBasicInfo.ulSeqRecId)
                        {
                            HxLOG_Print(" SR DATA IO Current CRID = [0x%08X]\n", pstIrSvcInfo->pstPVRSvcInfo->stPVRDrmInfo.stDrmRightRecInfo.ulCRID);
                            HxLOG_Print(" SR DATA IO Data CRID = [0x%08X]\n", *(HUINT32*)(p+17));

                            if(*(HUINT32*)(p+17) == 0)
                            {
                                break;
                            }
                        }
                    }
                }
            }
#endif

#ifdef	IREXT_NONSYNC
			HUMAX_VD_ExtStorage_DelayWrite(wRefId, wStart, wLength, p + 8);
#else
			pMsg = (CAS_IR_BASE_MSG *)OxCAS_Malloc(sizeof(CAS_IR_BASE_MSG));
			HxSTD_memset(pMsg, 0, sizeof(CAS_IR_BASE_MSG));
			pMsg->msg.FILE.RefId = wRefId;
			pMsg->msg.FILE.pucData = (HUINT8 *)OxCAS_Malloc(wLength);
			if( pMsg->msg.FILE.pucData != NULL)
			{
				HxSTD_memcpy((HUINT8 *)(pMsg->msg.FILE.pucData), p+8, wLength);
			}
			pMsg->msg.FILE.ulStart = wStart;
			pMsg->msg.FILE.ulLen = wLength;
			CAS_IR_SendMessage(eIR_MSG_CLASS_CAS, IR_CATASK_FILE_WRITE, (void *)pMsg);
#endif
			break;

		case VD_ES_READ:
			HxLOG_Info("ES_READ\n");

#if defined(CONFIG_DEBUG) && defined(IRPLAT_PVR_DRM)
			{
				IR_SERVICE_INFO *pstIrSvcInfo;

				pstIrSvcInfo = CAS_IR_DRV_FindIrSvcInfoBySCELLHandle(Humax_VD_ExtStorage[wRefId].resourceID);
				HxLOG_Info("[%08X] : crId : [%08X] rrId : [%08X] srId : [%08X]\n", Humax_VD_ExtStorage[wRefId].es_open_st.wRecordId, pstIrSvcInfo->pstPVRSvcInfo->stPVRDrmInfo.stDrmRightRecInfo.ulCRID, pstIrSvcInfo->pstPVRSvcInfo->stPVRDrmInfo.stDrmBasicInfo.ulRightRecId,pstIrSvcInfo->pstPVRSvcInfo->stPVRDrmInfo.stDrmBasicInfo.ulSeqRecId);
			}
#endif
			HxLOG_Print(" Read - wRefId(0x%X), wOpCode(0x%X), wDataWriteSize(0x%X)\n", wRefId, wOpCode, wDataWriteSize);
			HxLOG_Print(" Read - wStart = 0x%X, wLength = 0x%X\n", wStart, wLength);

			ullFilePos = wStart;
			sizeToRead = wLength;

			CARM_PVR_Seek(ulFileHandle, ullFilePos, ePAL_PB_SEEKPOSITION_START, FALSE);

			// 원래는 20ms안에 이 함수를 return해주기 위해서 실제 write/read동작은 다른 task에서 수행해야 하지만..
			res = CARM_PVR_ReadFile(ulFileHandle, (HUINT8*)pvDataRead, sizeToRead, &ulSize);
			if( res )
			{
				HxLOG_Print(" Read - ERROR : ReadFile\n");
				result = IA_FAIL;
			}
			*pwDataReadSize = ulSize;
			VDExtSDump(ulSize, p);
			break;
	}

	VK_SEM_Release(s_ulExtStorageAPISem);
	HxLOG_Debug("<--\n");
	return result;
}

HUINT32 HUMAX_VD_ExtStorage_DelayWrite(vd_ref_id wRefId, ia_word32 ulStart, ia_word16 ulLen, void *pucData)
{
	HUINT64 		ullFilePos;
	HUINT32		res, ulSize;
	HUINT32		ulFileHandle;

	if (wRefId >= HVD_EXTSTORAGE_FILE_CNT_MAX)
	{
		return 1;
	}

	HxLOG_Debug("-->>\n");

#ifndef	IREXT_NONSYNC
	VK_SEM_Get(s_ulExtStorageAPISem);
#endif

	ulFileHandle = Humax_VD_ExtStorage[wRefId].ulFileHandle;

	HxLOG_Print(" DelayWrite[%x] - [%x] - wStart = 0x%X, wLength = 0x%X]\n", ulFileHandle, wRefId, ulStart, ulLen);
	if (ulFileHandle == VD_NULL_FILE_HANDLE)
	{
#ifndef	IREXT_NONSYNC
		VK_SEM_Release(s_ulExtStorageAPISem);
#endif
		return 1;
	}

#ifndef	IREXT_NONSYNC
	VDExtSDump(ulLen, pucData);
#endif

	ullFilePos = ulStart;
	res = CARM_PVR_Seek(ulFileHandle, ullFilePos, ePAL_PB_SEEKPOSITION_START, TRUE);

	if(res != ERR_OK)		//Fix prevent defects
	{
#ifndef	IREXT_NONSYNC
		VK_SEM_Release(s_ulExtStorageAPISem);
#endif
		return 1;
	}
	// 원래는 20ms안에 이 함수를 return해주기 위해서 실제 write/read동작은 다른 task에서 수행해야 하지만..
	res = CARM_PVR_WriteFile(ulFileHandle, (HUINT8*)pucData, ulLen, &ulSize);
	if( res == ERR_OK)
	{
		HxLOG_Print(" DelayWrite - SUCCESS (callback Func address = 0x%X)\n", Humax_VD_ExtStorage[wRefId].es_open_st.pfnErrorNotify);

		if (Humax_VD_ExtStorage[wRefId].es_open_st.pfnErrorNotify != NULL)
				Humax_VD_ExtStorage[wRefId].es_open_st.pfnErrorNotify(
					Humax_VD_ExtStorage[wRefId].referenceID,
					Humax_VD_ExtStorage[wRefId].es_open_st.wSoftCellVal, IA_SUCCESS);
	}
	else
	{
		HxLOG_Print(" DelayWrite - ERROR : WriteFile\n");

		HxLOG_Print(" DelayWrite- FAIL (callback Func address = 0x%X)\n", Humax_VD_ExtStorage[wRefId].es_open_st.pfnErrorNotify);
		if (Humax_VD_ExtStorage[wRefId].es_open_st.pfnErrorNotify != NULL)
		{
				Humax_VD_ExtStorage[wRefId].es_open_st.pfnErrorNotify(
					Humax_VD_ExtStorage[wRefId].referenceID,
					Humax_VD_ExtStorage[wRefId].es_open_st.wSoftCellVal, IA_FAIL);
		}

	}
	HxLOG_Debug("<--End\n");
#ifndef	IREXT_NONSYNC
	VK_SEM_Release(s_ulExtStorageAPISem);
#endif
	return 0;
}
#endif

/* end of file */


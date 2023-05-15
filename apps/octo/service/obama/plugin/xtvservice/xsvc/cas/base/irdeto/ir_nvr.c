#include <stdio.h>

//#include "di_err.h"
#include "octo_common.h"
#include "ir_platform.h"
#include "ir_nvr.h"
#include "ir_loader.h"

#include <_svc_cas_sub_dev_api.h>
#include <_svc_cas_mgr_nvram.h>

#define _______________ACCESS_NVRAM____________________







#define IR_CAS_FILE_JFFS2_PATH		OBAMA_DIR_DATA "/cas/"

#define IR_CAS_FILE_MAIL 			"IrdetoMailData"

#if defined(CONFIG_MW_CAS_IRDETO_PPV)
#define IR_CAS_FILE_PPVLIST 			"IrdetoPPVData"
#endif

HIR_ERR CAS_IR_NVR_FlashReadMail(HUINT32 ulOffset, HUINT32 ulSize, HUINT8 *pucData)
{
	PAL_FS_UINT32	hFileHandle;
	PAL_FS_INT64	nPosOffset;
	PAL_FS_UINT32	ulReadSize;
	HINT32	nErrCode = ERR_FAIL;
	HUINT8	ucFileName[128+1];

	if (pucData == NULL)
	{
		HxLOG_Info("target is null...\n");
		return HIR_NOT_OK;
	}

	HxLOG_Print("Offset[%d], Size[%d]\n", ulOffset, ulSize);

	snprintf(ucFileName, 128, "%s%s", IR_CAS_FILE_JFFS2_PATH, IR_CAS_FILE_MAIL);
	nErrCode = svc_cas_DevFsOpen(0, ucFileName, PAL_FS_MODE_READ, &hFileHandle);
	if (nErrCode)
	{
		HxLOG_Info("error occurred in svc_cas_DevFsOpen(%d).\n", nErrCode);
		return HIR_NOT_OK;
	}

	nErrCode = svc_cas_DevFsLSeek(hFileHandle, ulOffset, PAL_FS_POS_START, &nPosOffset);
	if (nErrCode)
	{
		HxLOG_Info("error occurred in svc_cas_DevFsLSeek(%d).\n", nErrCode);
		svc_cas_DevFsClose(hFileHandle);
		return HIR_NOT_OK;
	}

	nErrCode = svc_cas_DevFsRead(hFileHandle, pucData, ulSize, &ulReadSize);
	if (nErrCode)
	{
		HxLOG_Info("error occurred in svc_cas_DevFsRead(%d).\n", nErrCode);
		svc_cas_DevFsClose(hFileHandle);
		return HIR_NOT_OK;
	}

	svc_cas_DevFsClose(hFileHandle);

	return HIR_OK;
}


HIR_ERR CAS_IR_NVR_FlashWriteMail(HUINT32 ulOffset, HUINT32 ulSize, HUINT8 *pucData)
{
	PAL_FS_UINT32	hFileHandle;
	PAL_FS_INT64	nPosOffset;
	PAL_FS_UINT32	ulWrittenSize;
	HINT32	nErrCode;
	HUINT8	ucFileName[128];

	if (pucData == NULL)
	{
		HxLOG_Info("target is null...\n");
		return HIR_NOT_OK;
	}

	HxLOG_Print("Offset[%d], Size[%d]\n", ulOffset, ulSize);
	if (svc_cas_DevFsIsDirExist(IR_CAS_FILE_JFFS2_PATH) != TRUE)
	{
		if (svc_cas_DevFsMkdir(IR_CAS_FILE_JFFS2_PATH) != ERR_OK)
		{
			HxLOG_Info("error occurred in svc_cas_DevFsMkdir\n");
			return HIR_NOT_OK;
		}
	}

#define BUFF_SIZE 128
	snprintf(ucFileName, BUFF_SIZE, "%s%s", IR_CAS_FILE_JFFS2_PATH, IR_CAS_FILE_MAIL);
	nErrCode = svc_cas_DevFsOpen(0, ucFileName, PAL_FS_MODE_WRITE, &hFileHandle);
	if (nErrCode)
	{
		HxLOG_Info("error occurred in svc_cas_DevFsOpen(%d).\n", nErrCode);
		return HIR_NOT_OK;
	}

	nErrCode = svc_cas_DevFsLSeek(hFileHandle, ulOffset, PAL_FS_POS_START, &nPosOffset);
	if (nErrCode)
	{
		HxLOG_Info("error occurred in svc_cas_DevFsLSeek(%d).\n", nErrCode);
		svc_cas_DevFsClose(hFileHandle);
		return HIR_NOT_OK;
	}

	nErrCode = svc_cas_DevFsWrite(hFileHandle, pucData, ulSize, &ulWrittenSize);
	if (nErrCode)
	{
		HxLOG_Info("error occurred in svc_cas_DevFsWrite(%d).\n", nErrCode);
		svc_cas_DevFsClose(hFileHandle);
		return HIR_NOT_OK;
	}

	svc_cas_DevFsClose(hFileHandle);

#if defined(CONFIG_DB_USE_BACKUP_PARTITION)
	if (svc_cas_DevFsIsDirExist(IR_CAS_FILE_JFFS2_BACKUP_PATH) != TRUE)
	{
		if (svc_cas_DevFsMkdir(IR_CAS_FILE_JFFS2_BACKUP_PATH) != ERR_OK)
		{
			HxLOG_Error("error occurred in PAL_FS_Mkdir\n");
			return HIR_NOT_OK;
		}
	}

	snprintf(ucFileName, BUFF_SIZE, "cp %s%s %s", IR_CAS_FILE_JFFS2_PATH, IR_CAS_FILE_MAIL, IR_CAS_FILE_JFFS2_BACKUP_PATH);
	if(system(ucFileName) == -1)
	{
		HxLOG_Critical("\n");
	}
#endif
	return HIR_OK;
}


HIR_ERR CAS_IR_NVR_FlashEraseMailFile(void)
{
	PAL_FS_UINT32	hFileHandle;
	HINT32	nErrCode;
	HUINT8	ucFileName[128+1];

	HxLOG_Info("\n");

	if (svc_cas_DevFsIsDirExist(IR_CAS_FILE_JFFS2_PATH) != TRUE)
	{
		HxLOG_Print("Directory(%s) is not found.\n", IR_CAS_FILE_JFFS2_PATH);
		return HIR_NOT_OK;
	}

	snprintf(ucFileName, 128, "%s%s", IR_CAS_FILE_JFFS2_PATH, IR_CAS_FILE_MAIL);

	nErrCode = svc_cas_DevFsOpen(0, ucFileName, PAL_FS_MODE_WRITE, &hFileHandle);
	if (nErrCode)
	{
		HxLOG_Info("error occurred in svc_cas_DevFsOpen(%d).\n", nErrCode);
		return HIR_NOT_OK;
	}

	nErrCode = svc_cas_DevFsDelete(0, ucFileName);
	if (nErrCode)
	{
		HxLOG_Info("error occurred in PAL_FS_Erase(%d).\n", nErrCode);
		svc_cas_DevFsClose(hFileHandle);
		return HIR_NOT_OK;
	}

	svc_cas_DevFsClose(hFileHandle);
	return HIR_OK;
}

HIR_ERR CAS_IR_NVR_Partition_Read( IRNVR_PART_Type ePartType, HUINT32 ulPartOffset,HUINT8 *pucData, HUINT32 ulSize )
{
	SvcCas_DevFlashField_e	eType;
	HUINT32				ulMaxOffset;

	HxLOG_Print("Offset[%d], Size[%d]\n", ulPartOffset, ulSize);

	if(pucData ==NULL)
	{
		HxLOG_Info("null pointer\n");
		return HIR_NOT_OK;
	}

	switch(ePartType)
	{
		case IRNVR_PART_IRDETO :
			eType = eCAS_DEV_FLASH_FIELD_IRDETO_IR_PARTITION;
			ulMaxOffset = IR_PARTITION_IRDETO_SIZE;
			break;

		case IRNVR_PART_CA :
			eType = eCAS_DEV_FLASH_FIELD_IRDETO_CA_PARTITION;
			ulMaxOffset = IR_PARTITION_CA_SIZE;
			break;
		case IRNVR_PART_LOADER :
			eType = eCAS_DEV_FLASH_FIELD_IRDETO_LOADER_PARTITION;
			ulMaxOffset = IR_PARTITION_LOADER_SIZE;
			break;

		case IRNVR_WHOLE_AREA:
		default:
			HxLOG_Info("Invalid partition(%d)\n", ePartType);
			return HIR_NOT_OK;
	}

	if((ulPartOffset + ulSize)> ulMaxOffset)
	{
		HxLOG_Info("Invalid offset(0x%X)\n", ulPartOffset);
		return HIR_NOT_OK;
	}

	if(ERR_OK != svc_cas_DevSysGetField(eType, ulPartOffset, pucData, ulSize) )
	{
		HxLOG_Info("svc_cas_DevSysGetField error\n ");
		return HIR_NOT_OK;
	}

	return HIR_OK;
}

HIR_ERR CAS_IR_NVR_Partition_Write( IRNVR_PART_Type ePartType, HUINT32 ulPartOffset, HUINT8 *pucData, HUINT32 ulSize)
{
	SvcCas_DevFlashField_e	eType;
	HUINT32				ulMaxOffset;

	HxLOG_Print("Offset[%d], Size[%d]\n", ulPartOffset, ulSize);

	if(pucData ==NULL)
	{
		HxLOG_Info("null pointer\n");
		return HIR_NOT_OK;
	}

	switch(ePartType)
	{
		case IRNVR_PART_IRDETO :
			eType = eCAS_DEV_FLASH_FIELD_IRDETO_IR_PARTITION;
			ulMaxOffset = IR_PARTITION_IRDETO_SIZE;
			break;

		case IRNVR_PART_CA :
			eType = eCAS_DEV_FLASH_FIELD_IRDETO_CA_PARTITION;
			ulMaxOffset = IR_PARTITION_CA_SIZE;
			break;
		case IRNVR_PART_LOADER :
			eType = eCAS_DEV_FLASH_FIELD_IRDETO_LOADER_PARTITION;
			ulMaxOffset = IR_PARTITION_LOADER_SIZE;
			break;

		case IRNVR_WHOLE_AREA:
		default:
			HxLOG_Info("Invalid partition(%d)\n", ePartType);
			return HIR_NOT_OK;
	}

	if((ulPartOffset + ulSize)> ulMaxOffset)
	{
		HxLOG_Info("Invalid offset(0x%X)\n", ulPartOffset);
		return HIR_NOT_OK;
	}

	if(ERR_OK != svc_cas_DevSysSetField(eType, ulPartOffset, pucData, ulSize) )
	{
		HxLOG_Info("svc_cas_DevSysSetField error\n ");
		return HIR_NOT_OK;
	}

	return HIR_OK;
}

HIR_ERR CAS_IR_NVR_FlashReadPPVList(HUINT32 ulOffset, HUINT32 ulSize, HUINT8 *pucData)
{
#if defined(CONFIG_MW_CAS_IRDETO_PPV)
	PAL_FS_UINT32	hFileHandle;
	PAL_FS_INT64	nPosOffset;
	PAL_FS_UINT32	ulReadSize;
	HINT32	nErrCode;
	HUINT8	ucFileName[128];

	if (pucData == NULL)
	{
		HxLOG_Info("target is null...\n");
		return HIR_NOT_OK;
	}


	HxLOG_Print("Offset[%d], Size[%d]\n", ulOffset, ulSize);

	sprintf(ucFileName, "%s%s", IR_CAS_FILE_JFFS2_PATH, IR_CAS_FILE_PPVLIST);
	nErrCode = svc_cas_DevFsOpen(0, ucFileName, PAL_FS_MODE_READ, &hFileHandle);
	if (nErrCode)
	{
		HxLOG_Info("error occurred in svc_cas_DevFsOpen(%d).\n", nErrCode);
		return HIR_NOT_OK;
	}

	nErrCode = svc_cas_DevFsLSeek(hFileHandle, ulOffset, PAL_FS_POS_START, &nPosOffset);
	if (nErrCode)
	{
		HxLOG_Info("error occurred in svc_cas_DevFsLSeek(%d).\n", nErrCode);
		svc_cas_DevFsClose(hFileHandle);
		return HIR_NOT_OK;
	}

	nErrCode = svc_cas_DevFsRead(hFileHandle, pucData, ulSize, &ulReadSize);
	if (nErrCode)
	{
		HxLOG_Info("error occurred in svc_cas_DevFsRead(%d).\n", nErrCode);
		svc_cas_DevFsClose(hFileHandle);
		return HIR_NOT_OK;
	}

	svc_cas_DevFsClose(hFileHandle);

#endif	// CONFIG_MW_CAS_IRDETO_PPV
	return HIR_OK;
}


HIR_ERR CAS_IR_NVR_FlashWritePPVList(HUINT32 ulOffset, HUINT32 ulSize, HUINT8 *pucData)
{
#if defined(CONFIG_MW_CAS_IRDETO_PPV)
	PAL_FS_UINT32	hFileHandle;
	PAL_FS_INT64	nPosOffset;
	PAL_FS_UINT32	ulWrittenSize;
	HINT32	nErrCode;
	HUINT8	ucFileName[128];

	if (pucData == NULL)
	{
		HxLOG_Info("target is null...\n");
		return HIR_NOT_OK;
	}

	HxLOG_Print("Offset[%d], Size[%d]\n", ulOffset, ulSize);
	if (svc_cas_DevFsIsDirExist(IR_CAS_FILE_JFFS2_PATH) != TRUE)
	{
		if (svc_cas_DevFsMkdir(IR_CAS_FILE_JFFS2_PATH) != ERR_OK)
		{
			HxLOG_Info("error occurred in svc_cas_DevFsMkdir\n");
			return HIR_NOT_OK;
		}
	}

	sprintf(ucFileName, "%s%s", IR_CAS_FILE_JFFS2_PATH, IR_CAS_FILE_PPVLIST);
	nErrCode = svc_cas_DevFsOpen(0, ucFileName, PAL_FS_MODE_WRITE, &hFileHandle);
	if (nErrCode)
	{
		HxLOG_Info("error occurred in svc_cas_DevFsOpen(%d).\n", nErrCode);
		return HIR_NOT_OK;
	}

	nErrCode = svc_cas_DevFsLSeek(hFileHandle, ulOffset, PAL_FS_POS_START, &nPosOffset);
	if (nErrCode)
	{
		HxLOG_Info("error occurred in svc_cas_DevFsLSeek(%d).\n", nErrCode);
		svc_cas_DevFsClose(hFileHandle);
		return HIR_NOT_OK;
	}

	nErrCode = svc_cas_DevFsWrite(hFileHandle, pucData, ulSize, &ulWrittenSize);
	if (nErrCode)
	{
		HxLOG_Info("error occurred in svc_cas_DevFsWrite(%d).\n", nErrCode);
		svc_cas_DevFsClose(hFileHandle);
		return HIR_NOT_OK;
	}

	svc_cas_DevFsClose(hFileHandle);
#endif	// CONFIG_MW_CAS_IRDETO_PPV
	return HIR_OK;
}



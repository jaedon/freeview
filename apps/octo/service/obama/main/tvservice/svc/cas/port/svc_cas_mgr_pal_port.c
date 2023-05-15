/**
	@file		casmgr_pal_port.c
	@brief    	casmgr_pal_port.c (CAS Service)

	Description:\n
	Module: \n
	Remarks :\n

	Copyright (c) 2012 HUMAX Co., Ltd. \n
	All rights reserved. \n
*/

/********************************************************************
* Header Files
********************************************************************/
#ifndef _GNU_SOURCE
#define _GNU_SOURCE 1
#endif

#define ___01_Header_Files___
#include <stb_params.h>

#include <pal_ci.h>
#include <pal_power.h>
#include <pal_sys.h>
#include <pal_param.h>
#include <pal_fuse.h>
#include <pal_panel.h>
#include <pal_dsc.h>
#if defined(CONFIG_MW_CAS_NUM_SC_SLOT)
#include <pal_sc.h>
#endif

#include <pal_demux.h>
#include <pal_cipher.h>

//#include <pdriver.h>

#include <_svc_cas_sub_dev_api.h>
#include <_svc_cas_mgr_pal_port.h>
#include <uapi.h>

//	DRV_DEV는 /device/include/cmd/pdriver.h에 정의되어 있음.
//	어떤 이유로 pdriver.h가 include에서 제외 되었으므로,
//	우선 compile만 되게 아래에 재 선언함.
//	관련자의 수정이 필요.
#ifndef	DRV_DEV
#define	DRV_DEV		"/dev/ci"
#endif

/********************************************************************
* For Debugging
********************************************************************/
#define ___02_Debugging___




/********************************************************************
* Local Definitions : #define / enum / struct / union
********************************************************************/
#define ___03_Local_Definitions___

#ifdef WIN32
#define CARM_JFFS2_PATH				""
#else
#define CARM_JFFS2_PATH				OBAMA_DIR_DATA "/cas/"
#endif

#define CARM_NAGRA_NVRAM_FILE_NAME 		"NagraNvramData"
#define CARM_VIACCESS_NVRAM_FILE_NAME 	"ViaccessNvramData"
#define CARM_CONAX_NVRAM_FILE_NAME 		"ConaxNvramData"
#define CARM_IRDETO_NVRAM_FILE_NAME 	"IrdetoNvramData"
#define CARM_CI_NVRAM_FILE_NAME 		"CiNvramData"
/********************************************************************
* Extern Variables / Function Prototypes
********************************************************************/
#define ___04_Extern_Vars_Func_Prototypes___

/********************************************************************
* Static Variables / Function Prototypes
********************************************************************/
#define ___05_Local_Vars_Func_Prototypes___

HERROR local_CasMgrPalPort_nvram_ReadFile(DxCAS_GroupId_e eCasGroupId, HUINT32 ulOffset, HUINT32 ulSize, HUINT8 *pucData);
HERROR local_CasMgrPalPort_nvram_WriteFile(DxCAS_GroupId_e eCasGroupId, HUINT32 ulOffset, HUINT32 ulSize, HUINT8 *pucData);
HERROR local_CasMgrPalPort_nvram_GetFileName(DxCAS_GroupId_e eCasGroupId, HUINT8 **ppszFileName);
/********************************************************************
* Static Functions
* Naming rule : "local"_"CasMgrPalPort" _ "Function Name"
********************************************************************/
#define ___06_Local_APIs___

/* system */
HERROR	local_svc_cas_PalPortSysConvertFlashFieldTypeRmToPal(SvcCas_DevFlashField_e inType, PAL_FLASH_FIELD_t *pCnvtType)
{
	if (NULL == pCnvtType)
	{
		return ERR_FAIL;
	}

	switch (inType)
	{
	case eCAS_DEV_FLASH_FIELD_SERIAL_NO:
		*pCnvtType = PAL_FLASH_FIELD_SERIAL_NO;
		break;

	case eCAS_DEV_FLASH_FIELD_DTCP_KEY:
		*pCnvtType = PAL_FLASH_FIELD_DTCP_KEY;
		break;

	case eCAS_DEV_FLASH_FIELD_CI_PLUS_KEY:
		*pCnvtType = PAL_FLASH_FIELD_CI_PLUS_KEY;
		break;

	case eCAS_DEV_FLASH_FIELD_MAC_ADDR:
		*pCnvtType = PAL_FLASH_FIELD_MAC_ADDR;
		break;

	case eCAS_DEV_FLASH_FIELD_HDCP_KEY:
		*pCnvtType = PAL_FLASH_FIELD_HDCP_KEY;
		break;

	case eCAS_DEV_FLASH_FIELD_PARING_DATA:
		*pCnvtType = PAL_FLASH_FIELD_PARING_DATA;
		break;

	case eCAS_DEV_FLASH_FIELD_SYSTEM_ID:
		*pCnvtType = PAL_FLASH_FIELD_SYSTEM_ID;
		break;

	case eCAS_DEV_FLASH_FIELD_LOADER_VER:
		*pCnvtType = PAL_FLASH_FIELD_LOADER_VER;
		break;

	case eCAS_DEV_FLASH_FIELD_APP_VER:
		*pCnvtType = PAL_FLASH_FIELD_APP_VER;
		break;

	case eCAS_DEV_FLASH_FIELD_PRIVATE:
		*pCnvtType = PAL_FLASH_FIELD_PRIVATE;
		break;

	case eCAS_DEV_FLASH_FIELD_CSC_KEY:
		*pCnvtType = PAL_FLASH_FIELD_CSC_KEY;
		break;

	case eCAS_DEV_FLASH_FIELD_IRDETO_BBCB:
		*pCnvtType = PAL_FLASH_FIELD_IRDETO_BBCB;
		break;

	case eCAS_DEV_FLASH_FIELD_IRDETO_UC_SECURE_ID:
		*pCnvtType = PAL_FLASH_FIELD_IRDETO_UC_SECURE_ID;
		break;

	case eCAS_DEV_FLASH_FIELD_IRDETO_UC_PRIVATE_DATA:
		*pCnvtType = PAL_FLASH_FIELD_IRDETO_UC_PRIVATE_DATA;
		break;

	case eCAS_DEV_FLASH_FIELD_IRDETO_CPCB:
		*pCnvtType = PAL_FLASH_FIELD_IRDETO_CPCB;
		break;

	case eCAS_DEV_FLASH_FIELD_IRDETO_CPCB_ENCRYPTED:
		*pCnvtType = PAL_FLASH_FIELD_IRDETO_CPCB_ENCRYPTED;
		break;

	case eCAS_DEV_FLASH_FIELD_IRDETO_CA_PARTITION:
		*pCnvtType = PAL_FLASH_FIELD_IRDETO_CA_PARTITION;
		break;

	case eCAS_DEV_FLASH_FIELD_IRDETO_LOADER_PARTITION:
		*pCnvtType = PAL_FLASH_FIELD_IRDETO_LOADER_PARTITION;
		break;

	case eCAS_DEV_FLASH_FIELD_IRDETO_IR_PARTITION:
		*pCnvtType = PAL_FLASH_FIELD_IRDETO_IR_PARTITION;
		break;

	case eCAS_DEV_FLASH_FIELD_IRDETO_IR_BACKUP_PARTITION:
		*pCnvtType = PAL_FLASH_FIELD_IRDETO_IR_BACKUP_PARTITION;
		break;

	case eCAS_DEV_FLASH_FIELD_MAX:
	default:
		*pCnvtType = PAL_FLASH_FIELD_MAX;
		break;
	}

	return ERR_OK;
}

HERROR	svc_cas_PalPortSysConvertFlashFieldTypeRmToPalParam(SvcCas_DevFlashField_e inType, PAL_PARAM_Item_e *peCnvtType)
{
	if (NULL == peCnvtType)
	{
		return ERR_FAIL;
	}

	switch (inType)
	{
	case eCAS_DEV_FLASH_FIELD_SERIAL_NO:
		*peCnvtType = ePAL_PARAM_SERIAL_NO;
		break;

	case eCAS_DEV_FLASH_FIELD_DTCP_KEY:
		*peCnvtType = ePAL_PARAM_DTCP_KEY;
		break;

	case eCAS_DEV_FLASH_FIELD_CI_PLUS_KEY:
		*peCnvtType = ePAL_PARAM_CI_PLUS_KEY;
		break;

	case eCAS_DEV_FLASH_FIELD_MAC_ADDR:
		*peCnvtType = ePAL_PARAM_MAC_ADDR;
		break;

	case eCAS_DEV_FLASH_FIELD_HDCP_KEY:
		*peCnvtType = ePAL_PARAM_HDCP_KEY;
		break;

	case eCAS_DEV_FLASH_FIELD_PARING_DATA:
		*peCnvtType = ePAL_PARAM_PARING_DATA;
		break;

	case eCAS_DEV_FLASH_FIELD_SYSTEM_ID:
		*peCnvtType = ePAL_PARAM_SYSTEM_ID;
		break;

	case eCAS_DEV_FLASH_FIELD_LOADER_VER:
		*peCnvtType = ePAL_PARAM_LOADER_VERSION;
		break;

	case eCAS_DEV_FLASH_FIELD_APP_VER:
		*peCnvtType = ePAL_PARAM_APP_VERSION;
		break;

	case eCAS_DEV_FLASH_FIELD_PRIVATE:
		*peCnvtType = ePAL_PARAM_PRIVATE;
		break;

	case eCAS_DEV_FLASH_FIELD_CSC_KEY:
		*peCnvtType = ePAL_PARAM_CSC_KEY;
		break;

	case eCAS_DEV_FLASH_FIELD_IRDETO_BBCB:
		*peCnvtType = ePAL_PARAM_IRDETO_BBCB;
		break;

	case eCAS_DEV_FLASH_FIELD_IRDETO_UC_SECURE_ID:
		*peCnvtType = ePAL_PARAM_IRDETO_UC_SECURE_ID;
		break;

	case eCAS_DEV_FLASH_FIELD_IRDETO_UC_PRIVATE_DATA:
		*peCnvtType = ePAL_PARAM_IRDETO_UC_PRIVATE_DATA;
		break;

	case eCAS_DEV_FLASH_FIELD_IRDETO_CPCB:
		*peCnvtType = ePAL_PARAM_IRDETO_CPCB;
		break;

	case eCAS_DEV_FLASH_FIELD_IRDETO_CPCB_ENCRYPTED:
		*peCnvtType = ePAL_PARAM_IRDETO_CPCB_ENCRYPTED;
		break;

	case eCAS_DEV_FLASH_FIELD_IRDETO_CA_PARTITION:
		*peCnvtType = ePAL_PARAM_IRDETO_CA_PARTITION;
		break;

	case eCAS_DEV_FLASH_FIELD_IRDETO_LOADER_PARTITION:
		*peCnvtType = ePAL_PARAM_IRDETO_LOADER_PARTITION;
		break;

	case eCAS_DEV_FLASH_FIELD_IRDETO_IR_PARTITION:
		*peCnvtType = ePAL_PARAM_IRDETO_IR_PARTITION;
		break;

	case eCAS_DEV_FLASH_FIELD_IRDETO_IR_BACKUP_PARTITION:
		*peCnvtType = ePAL_PARAM_IRDETO_IR_BACKUP_PARTITION;
		break;

	case eCAS_DEV_FLASH_FIELD_MAX:
	default:
		*peCnvtType = ePAL_PARAM_MAX;
		break;
	}

	return ERR_OK;
}

#if defined(CONFIG_MW_CI)
static HERROR	local_svc_cas_PalPortCiConvertCamStatePalToRm(PalCi_State_e inState, SvcCas_DevCiState_e *pCnvtState)
{
	if (NULL == pCnvtState)
	{
		return ERR_FAIL;
	}

	switch (inState)
	{
		case ePAL_CI_EXTRACTED:
			*pCnvtState = eCAS_DEV_CI_EXTRACTED;
			break;

		case ePAL_CI_INSERTED:
			*pCnvtState = eCAS_DEV_CI_INSERTED;
			break;
		default:
			return ERR_FAIL;
	}

	return ERR_OK;
}

static HERROR	local_svc_cas_PalPortCiConvertPathRmToPal(SvcCas_DevCiPath_e inPath, PalCi_Path_e *pCnvtPath)
{
	if (NULL == pCnvtPath)
	{
		return ERR_FAIL;
	}

	switch (inPath)
	{
		case eCAS_DEV_CI_BYPASS:
			*pCnvtPath = ePAL_CI_BYPASS;
			break;

		case eCAS_DEV_CI_PASSTHROUGH:
			*pCnvtPath = ePAL_CI_PASSTHROUGH;
			break;
		default:
			return ERR_FAIL;
	}

	return ERR_OK;
}

static HERROR	local_svc_cas_PalPortCiConvertCamTypeRmToPal(SvcCas_DevCiCamType_e inType, PalCi_CamType_e *pCnvtType)
{
	if (NULL == pCnvtType)
	{
		return ERR_FAIL;
	}

	switch (inType)
	{
		case eCAS_CAM_TYPE_CI_V1:
			*pCnvtType = ePAL_CI_CI_V1;
			break;

		case eCAS_CAM_TYPE_CI_PLUS:
			*pCnvtType = ePAL_CI_CI_PLUS;
			break;
		default:
			return ERR_FAIL;
	}

	return ERR_OK;
}
#endif

HERROR	local_svc_cas_PalPortDscConvertDscTypeRmToPal(SvcCas_DevDscType_e inType, PalDsc_DscType_e *pCnvtType)
{
	if (NULL == pCnvtType) return ERR_FAIL;

	switch(inType)
	{
	case eCAS_DSC_TYPE_NONEXDscType_None:			*pCnvtType = ePAL_DSC_DSCTYPE_NONE;		break;
	case eCAS_DSC_TYPE_DVB:			*pCnvtType = ePAL_DSC_DSCTYPE_DVB;		break;
	case eCAS_DSC_TYPE_AES_CLEAR:	*pCnvtType = ePAL_DSC_DSCTYPE_AES_CLEAR;	break;
	case eCAS_DSC_TYPE_AES_BLOCK:	*pCnvtType = ePAL_DSC_DSCTYPE_AES_BLOCK;	break;
	case eCAS_DSC_TYPE_DES_CLEAR:	*pCnvtType = ePAL_DSC_DSCTYPE_DES_CLEAR;	break;
	case eCAS_DSC_TYPE_DES_BLOCK:	*pCnvtType = ePAL_DSC_DSCTYPE_DES_BLOCK;	break;
	case eCAS_DSC_TYPE_MAX:			*pCnvtType = ePAL_DSC_DSCTYPE_MAX;		break;
	default:	HxLOG_Critical("\n\n");	return ERR_FAIL;
	}

	return ERR_OK;
}

HERROR	local_svc_cas_PalPortDscConvertDscDataTypeRmToPal(SvcCas_DscDataType_e inType, PalDsc_DataType_e *pCnvtType)
{
	if (NULL == pCnvtType) return ERR_FAIL;

	switch(inType)
	{
	case eCAS_DSC_DATA_TYPE_NONE:		*pCnvtType = ePAL_DSC_DATATYPE_NONE;		break;
	case eCAS_DSC_DATA_TYPE_VIDEO_PES:	*pCnvtType = ePAL_DSC_DATATYPE_VIDEOPES;	break;
	case eCAS_DSC_DATA_TYPE_AUDIO_PES:	*pCnvtType = ePAL_DSC_DATATYPE_AUDIOPES;	break;
	case eCAS_DSC_DATA_TYPE_PES_PES:	*pCnvtType = ePAL_DSC_DATATYPE_PES;	break;
	case eCAS_DSC_DATA_TYPE_PSI_PES:	*pCnvtType = ePAL_DSC_DATATYPE_PSIPES;	break;
	case eCAS_DSC_DATA_TYPE_MAX:		*pCnvtType = ePAL_DSC_DATATYPE_MAX;		break;
	default: 	HxLOG_Critical("\n\n");	return ERR_FAIL;
	}

	return ERR_OK;
}

HERROR	local_svc_cas_PalPortDscConvertDscKeyParityRmToPal(SvcCas_DevDscKeyParity_e inType, PalDsc_KeyParity_e *pCnvtType)
{
	if (NULL == pCnvtType) return ERR_FAIL;

	switch(inType)
	{
	case eCAS_DSC_PARITY_ODD:		*pCnvtType = ePAL_DSC_KEYPARITY_ODD;	break;
	case eCAS_DSC_PARITY_EVEN:	*pCnvtType = ePAL_DSC_KEYPARITY_EVEN;	break;
	default:		HxLOG_Critical("\n\n"); return ERR_FAIL;
	}

	return ERR_OK;
}

#if defined(CONFIG_MW_CI_USE_EXTERNAL_CONTROLLER)
static HERROR local_svc_cas_PalPortCiConvertCasMgrCiSource2PalCiSource(SvcCas_DevCiSource_e eCamSource, PalCi_CamSource_e *pePalCiCamSource)
{
	switch(eCamSource)
	{
	case eCAS_DEV_CI_SOURCE_INPUT0:			*pePalCiCamSource = ePAL_CI_CAM_INPUT0;			return ERR_OK;
	case eCAS_DEV_CI_SOURCE_INPUT1:			*pePalCiCamSource = ePAL_CI_CAM_INPUT1;			return ERR_OK;
	case eCAS_DEV_CI_SOURCE_OTHERMODULE:		*pePalCiCamSource = ePAL_CI_CAM_OTHERMODULE;	return ERR_OK;
	case eCAS_DEV_CI_SOURCE_SOURCE_MAX:		*pePalCiCamSource = ePAL_CI_CAM_SOURCE_MAX;	return ERR_OK;
	default: return ERR_FAIL;
	}
}
#endif

#if defined(CONFIG_MW_CI)
static HERROR	local_svc_cas_PalPortCiConvertCasMgrPalConnectState2CasCiConnectState(PalCi_State_e ePalCiState, SvcCas_DevCiState_e *peCiConnectState)
{
	switch(ePalCiState)
	{
	case ePAL_CI_EXTRACTED:	*peCiConnectState = eCAS_DEV_CI_EXTRACTED;	return ERR_OK;
	case ePAL_CI_INSERTED:	*peCiConnectState = eCAS_DEV_CI_INSERTED;	return ERR_OK;
	default: return ERR_FAIL;
	}

	return ERR_OK;
}

static HERROR	local_svc_cas_PalPortCiConvertCasMgrPalConnectWay2CasCiConnectWay(PalCi_ConnectWay_e ePalConnectWay, SvcCas_DevConnectWay_e *peCiConnectWay)
{
	switch(ePalConnectWay)
	{
	case ePAL_CI_CONNECT_NORMAL:		*peCiConnectWay = eCAS_CI_CONNECT_NORMAL;		return ERR_OK;
	case ePAL_CI_CONNECT_DAISYCHAIN:	*peCiConnectWay = eCAS_CI_CONNECT_DAISYCHAIN;	return ERR_OK;
	case ePAL_CI_CONNECT_SPECIAL:		*peCiConnectWay = eCAS_CI_CONNECT_SPECIAL;		return ERR_OK;
	case ePAL_CI_CONNECT_PLAYBACK:		*peCiConnectWay = eCAS_CI_CONNECT_PLAYBACK;		return ERR_OK;
	default: return ERR_FAIL;
	}

	return ERR_OK;
}
#endif

/********************************************************************
* Module Internal Functions
* Naming rule : "CasMgrPalPort" _ "Function Name"
********************************************************************/
#define ___07_Module_Internal_APIs___

#define _________System_____________

HERROR svc_cas_PalPortSysGetField(SvcCas_DevFlashField_e eType, HUINT32 ulOffset, void *pData, HUINT32 ulSize)
{
	if (PalParam_Activated())
	{
		PAL_PARAM_Item_e	eCnvtType;

		if(svc_cas_PalPortSysConvertFlashFieldTypeRmToPalParam(eType, &eCnvtType) != ERR_OK)
		{
			return ERR_FAIL;
		}

		return PAL_PARAM_GetFieldOffset(eCnvtType, ulOffset, pData, ulSize);
	}
	else
	{
		PAL_FLASH_FIELD_t	eCnvtType;

		if(local_svc_cas_PalPortSysConvertFlashFieldTypeRmToPal(eType, &eCnvtType) != ERR_OK)
		{
			return ERR_FAIL;
		}

		return PAL_SYS_GetFieldOffset(eCnvtType, ulOffset, pData, ulSize);
	}
}

HERROR svc_cas_PalPortSysSetField(SvcCas_DevFlashField_e eType, HUINT32 ulOffset, void *pData, HUINT32 ulSize)
{
	if (PalParam_Activated())
	{
		PAL_PARAM_Item_e	eCnvtType;

		if(svc_cas_PalPortSysConvertFlashFieldTypeRmToPalParam(eType, &eCnvtType) != ERR_OK)
		{
			return ERR_FAIL;
		}

		return PAL_PARAM_SetFieldOffset(eCnvtType, ulOffset, pData, ulSize);
	}
	else
	{
		PAL_FLASH_FIELD_t	eCnvtType;

		if(local_svc_cas_PalPortSysConvertFlashFieldTypeRmToPal(eType, &eCnvtType) != ERR_OK)
		{
			return ERR_FAIL;
		}

		return PAL_SYS_SetFieldOffset(eCnvtType, ulOffset, pData, ulSize);
	}
}

HERROR svc_cas_PalPortSysGetSystemSerialNumber(HUINT8 *szSerialNoStr, HUINT32 ulSize)
{
	if (PalParam_Activated())
	{
		return PAL_PARAM_GetField(ePAL_PARAM_SERIAL_NO, (void *)szSerialNoStr, ulSize);
	}
	else
	{
		return PAL_SYS_GetField(PAL_FLASH_FIELD_SERIAL_NO, (void*)szSerialNoStr, ulSize);
	}
}

HERROR svc_cas_PalPortSysGetFusingInfo(HUINT8 *pucSCS, HUINT8 *pucJTAG, HUINT8 *pucOTP, HUINT8 *pucCWE)
{
	Sys_FusingInfo_t	stFusingInfo;

	if (NULL == pucSCS || NULL == pucJTAG || NULL == pucOTP || NULL == pucCWE)
		return ERR_FAIL;

	HxSTD_MemSet(&stFusingInfo, 0, sizeof(Sys_FusingInfo_t));

	if (PAL_FUSE_GetInfo(&stFusingInfo) != ERR_OK)
	{
		HxLOG_Info("[CAS_IR_SECURECHIP_UpdateFusingInfo] fail to get fuse statue\n");
		return ERR_FAIL;
	}

	*pucSCS = (HUINT8)stFusingInfo.ucSCS;
	*pucJTAG = (HUINT8)stFusingInfo.ucJTAG;
	*pucOTP = (HUINT8)stFusingInfo.ucOTP;
	*pucCWE = (HUINT8)stFusingInfo.ucCWE;

	return ERR_OK;
}

HERROR svc_cas_PalPortSysGetFusingInfoExt(HUINT8 *pucSCS, HUINT8 *pucJTAG, HUINT8 *pucOTP, HUINT8 *pucCWE, HUINT32* pulOTPField1, HUINT32* pulOTPField2)
{
	Sys_FusingInfo_t	stFusingInfo;

	if (NULL == pucSCS || NULL == pucJTAG || NULL == pucOTP || NULL == pucCWE || NULL == pulOTPField1 || NULL == pulOTPField2)
		return ERR_FAIL;

	HxSTD_MemSet(&stFusingInfo, 0, sizeof(Sys_FusingInfo_t));

	if (PAL_FUSE_GetInfoExt(&stFusingInfo) != ERR_OK)
	{
		HxLOG_Info("[CAS_IR_SECURECHIP_UpdateFusingInfo] fail to get fuse statue\n");
		return ERR_FAIL;
	}

	*pucSCS = (HUINT8)stFusingInfo.ucSCS;
	*pucJTAG = (HUINT8)stFusingInfo.ucJTAG;
	*pucOTP = (HUINT8)stFusingInfo.ucOTP;
	*pucCWE = (HUINT8)stFusingInfo.ucCWE;
	*pulOTPField1 = (HUINT32)stFusingInfo.ulOTPField1;
	*pulOTPField2 = (HUINT32)stFusingInfo.ulOTPField2;

	return ERR_OK;
}

#if defined(CONFIG_MW_CAS_CONAX_CPM)
HERROR svc_cas_PalPortSysGetFusingInfoForConax(HUINT8 *pucSCS, HUINT8 *pucJTAG, HUINT8 *pucOTP, HUINT8 *pucCWE, HUINT8 *pucRemux, HUINT8 *pucMarketID)
{
	return PAL_SYS_GetFusingInfoForConax(pucSCS, pucJTAG, pucOTP, pucCWE, pucRemux, pucMarketID);
}
#endif

HERROR svc_cas_PalPortSysGetChipId(HUINT32 *pulChipId)
{
	if (PalParam_Activated())
	{
		return PAL_PARAM_GetField(ePAL_PARAM_CHIP_ID, (void *)pulChipId, sizeof(HUINT32));
	}
	else
	{
		return PAL_SYS_GetChipId(pulChipId);
	}
}

HERROR svc_cas_PalPortSysGetChipRevision(HUINT32 *pulChipId)
{
	if (PalParam_Activated())
	{
		return PAL_PARAM_GetField(ePAL_PARAM_CHIP_REVISION, (void *)pulChipId, sizeof(HUINT8));
	}
	else
	{
		return PAL_SYS_GetChipRevision((HUINT8 *)pulChipId);
	}
}

#define _________AV_____________

HERROR svc_cas_PalPortVideoRegisterEventCallback(SvcCas_MgrVideoCb_t pfnCasMgrVideoCb)
{
	return PAL_VIDEO_RegisterEventCallbackForCas((SvcCas_PalVideoCb_t)pfnCasMgrVideoCb);
}

HERROR svc_cas_PalPortAudioRegisterEventCallback(SvcCas_MgrAudioCb_t pfnCasMgrAudioCb)
{
	return PAL_AUDIO_RegisterEventCallbackForCas((SvcCas_PalAudioCb_t)pfnCasMgrAudioCb);
}

HBOOL svc_cas_PalPortIsVideoRunning(HUINT32 ulDecoderId)
{
	return PAL_VIDEO_IsRunning(ulDecoderId);
}

#define __________FS___________

CASDEV_FS_BOOL svc_cas_PalPortIsDirExist(const char *dirname)
{
	return HLIB_DIR_IsExist(dirname);
}

int svc_cas_PalPortMkDir(const char *dirname)
{
	return HLIB_DIR_Make(dirname);
}

CASDEV_FS_BOOL svc_cas_PalPortIsFileExist(const char *pathname)
{
	return HLIB_FILE_Exist(pathname);
}

#define __________NVRAM___________
HERROR svc_cas_PalPortNVRAMWriteData(DxCAS_GroupId_e eCasGroupId, HUINT32 ulOffset, HUINT32 ulSize, HUINT8 *pucData)
{
	if (pucData == NULL)
	{
		HxLOG_Error("[svc_cas_PalPortNVRAMWriteData] Target is null\n");
		return ERR_FAIL;
	}

	if (local_CasMgrPalPort_nvram_WriteFile(eCasGroupId, ulOffset, ulSize, pucData) != ERR_OK)
	{
		HxLOG_Error("[local_CasMgrPalPort_nvram_WriteFile] Writing error...\n");
		return ERR_FAIL;
	}

	return ERR_OK;
	//return MW_EEP_ExtZoneWriteData(ulOffset, ulSize, pucData);
}

HERROR svc_cas_PalPortNVRAMReadData(DxCAS_GroupId_e eCasGroupId, HUINT32 ulOffset, HUINT32 ulSize, HUINT8 *pucData)
{
	if (pucData == NULL)
	{
		HxLOG_Error("[svc_cas_PalPortNVRAMReadData] Target is null\n");
		return ERR_FAIL;
	}

	if (local_CasMgrPalPort_nvram_ReadFile(eCasGroupId, ulOffset, ulSize, pucData) != ERR_OK)
	{
		HxLOG_Error("[local_CasMgrPalPort_nvram_ReadFile] Reading error...\n");
		return ERR_FAIL;
	}

	return ERR_OK;
	//return MW_EEP_ExtZoneReadData(ulOffset, ulSize, pucData);
}

#define __________CI___________

#if defined(CONFIG_PROD_DEVICE_FILE_SYSTEM)
#define __________FileSystem____________
/* FS */
int		svc_cas_PalPortFsOpen(int pttype, const char *pathname, CASDEV_FS_UINT32 flags, CASDEV_FS_UINT32 *filedes)
{
	int fd = -1;
	int oflags = 0;

	mode_t mode = (S_IRUSR | S_IWUSR | S_IRGRP | S_IROTH);

	(void)pttype;

	if (flags & PAL_FS_MODE_READ)
	{
		oflags |= O_RDONLY;
	}
	else if (flags & PAL_FS_MODE_WRITE)
	{
		oflags |= O_WRONLY;
		oflags |= O_CREAT;
	}
	else if (flags & PAL_FS_MODE_RDWR)
	{
		oflags |= O_RDWR;
		oflags |= O_CREAT;
	}
	else
	{
		HxLOG_Error("invalid flags \"%s\": %li\n", pathname, flags);
		return ERR_FAIL;
	}

	if (flags & PAL_FS_MODE_APPEND)		oflags |= O_APPEND;
	if (flags & PAL_FS_MODE_TRUNC)		oflags |= O_TRUNC;

	if (flags & PAL_FS_MODE_O_DIRECT)	oflags |= O_DIRECT;
	if (flags & PAL_FS_MODE_O_LARGEFILE)	oflags |= O_LARGEFILE;

	if (flags & PAL_FS_MODE_NONBLOCK)	oflags |= O_NONBLOCK;

	if (fd = open(pathname, oflags, mode), fd < 0)
	{
		*filedes = 0;
		HxLOG_Error("cannot open \"%s\": %s\n", pathname, strerror(errno));
		if (ENOENT == errno || ENOTDIR == errno || EACCES == errno)
			return PAL_FS_FOLDER_NOT_EXIST_ERR;
		else if (EIO == errno)
			return PAL_FS_ERROR_IO;
		else if (EROFS == errno)
			return PAL_FS_READ_ONLY_ERR;		// read-only filesystem

		return PAL_FS_ERROR_NO_MORE_RESOURCES;
	}

	*filedes = fd;

	return ERR_OK;
}

int		svc_cas_PalPortFsClose(CASDEV_FS_UINT32 filedes)
{
	fsync((int)filedes);
	close((int)filedes);
	return ERR_OK;
}


int		svc_cas_PalPortFsRead(CASDEV_FS_UINT32 filedes, CASDEV_FS_UINT8 *buf, CASDEV_FS_UINT32 size,   CASDEV_FS_UINT32 *sizep)
{
	ssize_t n;

	do {
		n = read((int)filedes, buf, size);
	} while (-1 == n && EINTR == errno);

	if (-1 == n) {
		*sizep = 0;
		HxLOG_Error("cannot read: %s\n", strerror(errno));
		return PAL_FS_ERROR_NO_MORE_RESOURCES;
	}

	*sizep = n;
	return ERR_OK;
}


int		svc_cas_PalPortFsWrite(CASDEV_FS_UINT32 filedes, const CASDEV_FS_UINT8 *buf, CASDEV_FS_UINT32 size,    CASDEV_FS_UINT32 *sizep)
{
	ssize_t n;

	HxLOG_Info("write file - handle:0x%x, size_to_write:%d\n", filedes, size);
	if ((buf == NULL) || (sizep == NULL) || (size == 0))
	{
		HxLOG_Error("invalid parameters : buffer(0x%x), written_size(0x%x), size_to_write(%d)\n", (HUINT32)buf, (HUINT32)sizep, size);
		return ERR_FAIL;
	}

	do {
		n = write((int)filedes, buf, size);
	} while (-1 == n && EINTR == errno);

	if (-1 == n) {
		*sizep = 0;
		HxLOG_Error("cannot write: %s\n", strerror(errno));
		return PAL_FS_ERROR_NO_MORE_RESOURCES;
	}

	*sizep = n;
	return ERR_OK;
}

int		svc_cas_PalPortFsLSeek(CASDEV_FS_UINT32 filedes, CASDEV_FS_INT64 offset, CASDEV_FS_UINT32 pos,     CASDEV_FS_INT64 *offsetp)
{
	off_t curr;
	int whence;

	if (pos == PAL_FS_POS_START)
		whence = SEEK_SET;
	else if (pos == PAL_FS_POS_END)
		whence = SEEK_END;
	else
		whence = SEEK_CUR;

	if (curr = lseek((int)filedes, offset, whence), -1 == curr)
	{
		*offsetp = 0;
		HxLOG_Error("cannot seek: %s\n", strerror(errno));
		return PAL_FS_ERROR_NO_MORE_RESOURCES;
	}

	*offsetp = curr;
	return ERR_OK;
}


int		svc_cas_PalPortFsDelete(int pttype, const char *pathname)
{
	return HLIB_FILE_Delete(pathname);
}

#if defined (CONFIG_MW_CAS_VIACCESS) && defined (CONFIG_MW_MM_PVR)
int 	svc_cas_PalPortFsGetHddSerialNumber(const char *dev, char **hddsn)
{
	return UAPI_FS_GetHddSerialNumber(dev, hddsn);
}
#endif // #if defined (CONFIG_MW_CAS_VIACCESS) && defined (CONFIG_MW_MM_PVR)


int 	svc_cas_PalPortFsGetFileInfo(const char *pathname, HUINT32 type, SvcCas_DevFsFileState_t *filestatp)
{
	struct stat st;

	if (NULL == filestatp)
		return ERR_FAIL;

	if (-1 == lstat(pathname, &st))
	{
		HxLOG_Error("cannot stat \"%s\": %s\n", pathname, strerror(errno));
		return ERR_FAIL;
	}

	switch (st.st_mode & S_IFMT) {
	case S_IFREG:
		filestatp->fileType = PAL_FS_FILE_TYPE_FILE;
		break;
	case S_IFDIR:
		filestatp->fileType = PAL_FS_FILE_TYPE_DIR;
		break;
	case S_IFIFO:
		filestatp->fileType = PAL_FS_FILE_TYPE_FIFO;
		break;
	case S_IFLNK:
		filestatp->fileType = PAL_FS_FILE_TYPE_LINK;
		break;
	default:
		filestatp->fileType = PAL_FS_FILE_TYPE_UNKNOWN;
		break;
	}

	/*
	 * when the named file is symbolic link,
	 * return size about the symbolic link,
	 * not the file referenced by the symbolic link.
	 */
	filestatp->ulFileSize = st.st_size;

	filestatp->aTime = st.st_atime; 	/* time of last access */
	filestatp->mTime = st.st_mtime; 	/* time of last modification */
	filestatp->cTime = st.st_ctime; 	/* time of last change */

	return ERR_OK;
}
#endif


#if defined(CONFIG_MW_CAS_NUM_SC_SLOT)
/* SC */

HERROR	local_svc_cas_PalPortScConvertSCIHandleRmToPal(SciHandle_t inType, PalSc_Handle_t *pCnvtType)
{
	if (NULL == pCnvtType)
	{
		return ERR_FAIL;
	}

	*pCnvtType = inType;
	return ERR_OK;
}

HERROR	local_svc_cas_PalPortScConvertSCIHandlePalToRm(PalSc_Handle_t inType, SciHandle_t *pCnvtType)
{
	if (NULL == pCnvtType)
	{
		return ERR_FAIL;
	}

	*pCnvtType = inType;
	return ERR_OK;
}

HERROR	local_svc_cas_PalPortScConvertResetTypeRmToPal(SvcCas_DevScmResetType_e inType, PalSc_ResetType_e *pCnvtType)
{
	if (NULL == pCnvtType)
	{
		return ERR_FAIL;
	}

	switch(inType)
	{
		case eCAS_SCM_RESET_TYPE_NONE:
			*pCnvtType = eCAS_SCM_RESET_TYPE_NONE;
			break;
		case eCAS_SCM_RESET_TYPE_COLD:
			*pCnvtType = eCAS_SCM_RESET_TYPE_COLD;
			break;
		case eCAS_SCM_RESET_TYPE_WARM:
			*pCnvtType = eCAS_SCM_RESET_TYPE_WARM;
			break;
		default:
			HxLOG_Critical("\n\n");
			return ERR_FAIL;
	}

	return ERR_OK;
}

HERROR	local_svc_cas_PalPortScConvertSciProtocolRmToPal(SvcCas_DevSciProtocol_e inType, PalSc_Protocol_e *pCnvtType)
{
	if (NULL == pCnvtType)
	{
		return ERR_FAIL;
	}

	switch(inType)
	{
		case eCAS_SCI_T0:
			*pCnvtType = ePAL_SC_PROTOCOL_T0;
			break;
		case eCAS_SCI_T1:
			*pCnvtType = ePAL_SC_PROTOCOL_T1;
			break;
		case eCAS_SCI_T_0_IR:
			*pCnvtType = ePAL_SC_PROTOCOL_T0_IR;
			break;
		case eCAS_SCI_T_0_CX:
			*pCnvtType = ePAL_SC_PROTOCOL_T0_CX;
			break;
		case eCAS_SCI_T_1_NA:
			*pCnvtType = ePAL_SC_PROTOCOL_T1_NA;
			break;
		case eCAS_SCI_T_1_ARIB:
			*pCnvtType = ePAL_SC_PROTOCOL_T1_ARIB;
			break;
		case eCAS_SCI_T14:
			*pCnvtType = ePAL_SC_PROTOCOL_T14;
			break;
		case eCAS_SCI_T_END:
			*pCnvtType = ePAL_SC_PROTOCOL_END;
			break;
		default:
			HxLOG_Critical("\n\n");
			return ERR_FAIL;
	}

	return ERR_OK;
}

HERROR	local_svc_cas_PalPortScConvertScParamRmToPal(SvcCas_DevSciScParam_t *pInType, PalSc_Param_t *pCnvtType)
{
	if (NULL == pCnvtType || NULL == pInType)
	{
		return ERR_FAIL;
	}

	if(local_svc_cas_PalPortScConvertSciProtocolRmToPal(pInType->eProtocol, &(pCnvtType->eProtocol)) != ERR_OK)
	{
		HxLOG_Critical("\n\n");
		return ERR_FAIL;
	}

	pCnvtType->ulClock = pInType->ulClock;
	pCnvtType->ulBaudrate = pInType->ulBaudrate;
	pCnvtType->ucFI = pInType->ucFI;
	pCnvtType->ucDI = pInType->ucDI;
	pCnvtType->ucN= pInType->ucN;
	pCnvtType->ucCWI = pInType->ucCWI;
	pCnvtType->ucBWI = pInType->ucBWI;

	return ERR_OK;
}

HERROR	local_svc_cas_PalPortScConvertSciProtocolPalToRM(PalSc_Protocol_e inType, SvcCas_DevSciProtocol_e *pCnvtType)
{
	if (NULL == pCnvtType)
	{
		return ERR_FAIL;
	}

	switch(inType)
	{
		case ePAL_SC_PROTOCOL_T0:
			*pCnvtType = eCAS_SCI_T0;
			break;
		case ePAL_SC_PROTOCOL_T1:
			*pCnvtType = eCAS_SCI_T1;
			break;
		case ePAL_SC_PROTOCOL_T0_IR:
			*pCnvtType = eCAS_SCI_T_0_IR;
			break;
		case ePAL_SC_PROTOCOL_T0_CX:
			*pCnvtType = eCAS_SCI_T_0_CX;
			break;
		case ePAL_SC_PROTOCOL_T1_NA:
			*pCnvtType = eCAS_SCI_T_1_NA;
			break;
		case ePAL_SC_PROTOCOL_T1_ARIB:
			*pCnvtType = eCAS_SCI_T_1_ARIB;
			break;
		case ePAL_SC_PROTOCOL_T14:
			*pCnvtType = eCAS_SCI_T14;
			break;
		case ePAL_SC_PROTOCOL_END:
			*pCnvtType = eCAS_SCI_T_END;
			break;
		default:
			HxLOG_Critical("\n\n");
			return ERR_FAIL;
	}

	return ERR_OK;
}

HERROR	local_svc_cas_PalPortScConvertScParamPalToRm(PalSc_Param_t *pInType, SvcCas_DevSciScParam_t *pCnvtType)
{
	if (NULL == pCnvtType || NULL == pInType)
	{
		return ERR_FAIL;
	}

	if(local_svc_cas_PalPortScConvertSciProtocolPalToRM(pInType->eProtocol, &(pCnvtType->eProtocol)) != ERR_OK)
	{
		HxLOG_Critical("\n\n");
		return ERR_FAIL;
	}

	pCnvtType->ulClock = pInType->ulClock;
	pCnvtType->ulBaudrate = pInType->ulBaudrate;
	pCnvtType->ucFI = pInType->ucFI;
	pCnvtType->ucDI = pInType->ucDI;
	pCnvtType->ucN= pInType->ucN;
	pCnvtType->ucCWI = pInType->ucCWI;
	pCnvtType->ucBWI = pInType->ucBWI;

	return ERR_OK;
}



HERROR	local_svc_cas_PalPortScConvertScFlagRmToPal(SvcCas_DevScmFlag_e inType, PalSc_Flag_e *pCnvtType)
{
	if (NULL == pCnvtType)
	{
		return ERR_FAIL;
	}

	if(inType == eCAS_SCM_FLAG_DISABLE)
	{
		*pCnvtType = ePAL_SC_FLAG_DISABLE;
	}
	else if(inType == eCAS_SCM_FLAG_ENABLE)
	{
		*pCnvtType = ePAL_SC_FLAG_ENABLE;
	}
	else
	{
		HxLOG_Critical("\n\n");
		return ERR_FAIL;
	}

	return ERR_OK;
}

HERROR	local_svc_cas_PalPortScConvertScStatusPalToRm(PalSc_CardStatus_e inType, SvcCas_DevSciScStatus_e *pCnvtType)
{
	if (NULL == pCnvtType)
	{
		return ERR_FAIL;
	}

	if(inType == ePAL_SC_CARDSTATUS_NOT_PRESENT)
	{
		*pCnvtType = eCAS_SCI_SC_NOT_PRESENT;
	}
	else if(inType == ePAL_SC_CARDSTATUS_PRESENT)
	{
		*pCnvtType = eCAS_SCI_SC_PRESENT;
	}
	else
	{
		HxLOG_Critical("\n\n");
		return ERR_FAIL;
	}

	return ERR_OK;
}

HERROR local_CasMgrPalPort_nvram_GetFileName(DxCAS_GroupId_e eCasGroupId, HUINT8 **ppszFileName)
{
	switch(eCasGroupId)
	{
		case eDxCAS_GROUPID_NA:
			*ppszFileName = (HUINT8 *)CARM_NAGRA_NVRAM_FILE_NAME;
			break;

		case eDxCAS_GROUPID_IR:
			*ppszFileName = (HUINT8 *)CARM_IRDETO_NVRAM_FILE_NAME;
			break;

		case eDxCAS_GROUPID_VA:
			*ppszFileName = (HUINT8 *)CARM_VIACCESS_NVRAM_FILE_NAME;
			break;

		case eDxCAS_GROUPID_CX:
			*ppszFileName = (HUINT8 *)CARM_CONAX_NVRAM_FILE_NAME;
			break;

		case eDxCAS_GROUPID_CAM:
			*ppszFileName = (HUINT8 *)CARM_CI_NVRAM_FILE_NAME;
			break;

		case eDxCAS_GROUPID_MG:
		case eDxCAS_GROUPID_CR:
		case eDxCAS_GROUPID_ND:
		case eDxCAS_GROUPID_SATSA:
		default:
			*ppszFileName = NULL;
			return ERR_FAIL;
	}

	return ERR_OK;
}

HERROR local_CasMgrPalPort_nvram_ReadFile(DxCAS_GroupId_e eCasGroupId, HUINT32 ulOffset, HUINT32 ulSize, HUINT8 *pucData)
{
	HERROR			hErr;
	HOffset_t 		nPosOffset;
	PAL_FS_UINT32	ulReadSize;
	HINT32			nErrCode;
	HUINT8			ucFileName[128], *pszFileName;
	HxFILE_t		hFile;

	if (pucData == NULL)
	{
		HxLOG_Error("target is null...\n");
		return ERR_FAIL;
	}

	HxLOG_Print("Offset[%d], Size[%d]\n", (int)ulOffset, (int)ulSize);
	if (HLIB_DIR_IsExist(CARM_JFFS2_PATH) != TRUE)
	{
		HxSTD_memset(pucData, 0xFF, ulSize);
		return ERR_OK;
	}

	hErr = local_CasMgrPalPort_nvram_GetFileName(eCasGroupId, &pszFileName);

	if(hErr != ERR_OK)
	{
		HxLOG_Error("error occurred in local_carm_nvram_GetFileName(Unknown CAS).\n");
		return ERR_FAIL;
	}

	HxSTD_snprintf(ucFileName, 128, "%s%s", CARM_JFFS2_PATH, pszFileName);

	if (HLIB_FILE_Exist(ucFileName) != TRUE)
	{
		HxSTD_memset(pucData, 0xFF, ulSize);
		return ERR_OK;
	}

	hFile = HLIB_FILE_Open(ucFileName, "rb");
	if (hFile == NULL)
	{
		HxLOG_Error("error occurred in HLIB_FILE_Open.\n");
		return ERR_FAIL;
	}

	nErrCode = HLIB_FILE_Seek(hFile, ulOffset, eHxFILE_SEEK_SET, &nPosOffset);
	if (nErrCode)
	{
		HxLOG_Error("error occurred in HLIB_FILE_Seek(%d).\n", (unsigned int)nErrCode);
		HLIB_FILE_Close(hFile);
		return ERR_FAIL;
	}

	ulReadSize = HLIB_FILE_Read(hFile, pucData, 1, ulSize);
	if (ulReadSize <= 0)
	{
		HxLOG_Error("error occurred in HLIB_FILE_Read.\n");
		HLIB_FILE_Close(hFile);
		return ERR_FAIL;
	}

	HLIB_FILE_Close(hFile);
	return ERR_OK;
}


HERROR local_CasMgrPalPort_nvram_WriteFile(DxCAS_GroupId_e eCasGroupId, HUINT32 ulOffset, HUINT32 ulSize, HUINT8 *pucData)
{
	HERROR			hErr;
	HOffset_t		nPosOffset;
	PAL_FS_UINT32	ulWrittenSize;
	HUINT32			nErrCode;
	HUINT8			ucFileName[128], *pszFileName;
	HxFILE_t		hFile;
	HUINT8			*pszFileMode;

	if (pucData == NULL)
	{
		HxLOG_Error("target is null...\n");
		return ERR_FAIL;
	}

	HxLOG_Print("Offset[%d], Size[%d]\n", (int)ulOffset, (int)ulSize);
#if 0 //SVC CAS init 시 이미 확인 했음.
	if (PAL_FS_IsDirExist(CARM_JFFS2_PATH) != TRUE)
	{
		if (PAL_FS_Mkdir(CARM_JFFS2_PATH) != ERR_OK)
		{
			HxLOG_Info("error occurred in PAL_FS_Mkdir\n");
			return ERR_FAIL;
		}
	}
#endif

	hErr = local_CasMgrPalPort_nvram_GetFileName(eCasGroupId, &pszFileName);

	if(hErr != ERR_OK)
	{
		HxLOG_Error("error occurred in local_CasMgrPalPort_nvram_GetFileName(Unknown CAS).\n");
		return ERR_FAIL;
	}

	HxSTD_snprintf(ucFileName, 128, "%s%s", CARM_JFFS2_PATH, pszFileName);

	pszFileMode = HLIB_FILE_Exist(ucFileName) ? "r+b" : "w+b";

	hFile = HLIB_FILE_Open(ucFileName, pszFileMode);
	if (hFile == NULL)
	{
		HxLOG_Error("error occurred in HLIB_FILE_Open(%s).\n", pszFileName);
		return ERR_FAIL;
	}

	nErrCode = HLIB_FILE_Seek(hFile, ulOffset, eHxFILE_SEEK_SET, &nPosOffset);
	if (nErrCode)
	{
		HxLOG_Error("error occurred in HLIB_FILE_Seek(%d).\n", (unsigned int)nErrCode);
		HLIB_FILE_Close(hFile);
		return ERR_FAIL;
	}

	ulWrittenSize = HLIB_FILE_Write(hFile, pucData, 1, ulSize);
	if (ulWrittenSize <= 0)
	{
		HxLOG_Error("error occurred in HLIB_FILE_Write.\n");
		HLIB_FILE_Close(hFile);
		return ERR_FAIL;
	}

	HLIB_FILE_Close(hFile);

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

/* SC */
HERROR svc_cas_PalPortScInit(void)
{
	return PAL_SC_Init();
}

HERROR svc_cas_PalPortScGetSlotId(SciHandle_t hSci, HUINT8 *pulSlotId)
{
	PalSc_Handle_t		hPalSci;

	if(local_svc_cas_PalPortScConvertSCIHandleRmToPal(hSci, &hPalSci) != ERR_OK)
	{
		HxLOG_Critical("\n\n");
		return ERR_FAIL;
	}

	return PAL_SC_GetSlotId(hPalSci, pulSlotId);
}

HERROR svc_cas_PalPortScDeactivate(SciHandle_t hSci)
{
	PalSc_Handle_t	hPalSci;

	if(local_svc_cas_PalPortScConvertSCIHandleRmToPal(hSci, &hPalSci) != ERR_OK)
	{
		HxLOG_Critical("\n\n");
		return ERR_FAIL;
	}

	return PAL_SC_Deactivate(hPalSci);
}

HERROR svc_cas_PalPortScReset(
						SciHandle_t hSci,
						HUINT8 *pucAtr,
						HUINT32 *pulLen,
						SvcCas_DevScmResetType_e eResetType,
						SvcCas_DevSciScParam_t *pstDefaultParam
						)
{
	PalSc_Handle_t		hPalSci;
	PalSc_ResetType_e ePalResetType;
	PalSc_Param_t	stPalDefaultParam;

	if(local_svc_cas_PalPortScConvertSCIHandleRmToPal(hSci, &hPalSci) != ERR_OK)
	{
		HxLOG_Critical("\n\n");
		return ERR_FAIL;
	}

	if(local_svc_cas_PalPortScConvertResetTypeRmToPal(eResetType, &ePalResetType) != ERR_OK)
	{
		HxLOG_Critical("\n\n");
		return ERR_FAIL;
	}

	if(local_svc_cas_PalPortScConvertScParamRmToPal(pstDefaultParam, &stPalDefaultParam) != ERR_OK)
	{
		HxLOG_Critical("\n\n");
		return ERR_FAIL;
	}

	return PAL_SC_Reset(hPalSci, pucAtr, pulLen, ePalResetType, &stPalDefaultParam);
}

HERROR svc_cas_PalPortScRegisterCbFunc(HUINT32 ulSlotId, DxCAS_GroupId_e eCARMId, SvcCas_SciCb_t pfnCb, SciHandle_t *hpSci)
{
	HERROR hErr;


	//1 Handle 값(hPalSci)을 바로 올려주지 않으면, 타이밍이 틀어지는 문제로 수정함.
	//1 (Callback에서 Handle을 사용하는 것과 Callback이 호출되어서 Handle값이 반영되는 타이밍)
#if 0	//타이밍 문제
	hErr = PAL_SC_RegisterCbFunc(ulSlotId, ePalCARMId, (PalSc_Callback_t)pfnCb, &hPalSci);
#endif
	hErr = PAL_SC_RegisterCbFunc(ulSlotId, eCARMId, (PalSc_Callback_t)pfnCb, (PalSc_Handle_t *)hpSci);

	if(hErr != ERR_OK)
	{
		return hErr;
	}

#if 0	//타이밍 문제
	if(local_svc_cas_PalPortScConvertSCIHandlePalToRm(hPalSci, hpSci) != ERR_OK)
	{
		HxLOG_Critical("\n\n");
		return ERR_FAIL;
	}
#endif

	return ERR_OK;
}


HERROR svc_cas_PalPortScTransfer(SciHandle_t hSci, HUINT8 *pucWriteBuf, HUINT32 ulNumWrite, HUINT8 *pucReadBuf, HUINT32 *pulNumRead, HUINT32 ulEpxectedNumRead)
{
	PalSc_Handle_t		hPalSci;

	if(local_svc_cas_PalPortScConvertSCIHandleRmToPal(hSci, &hPalSci) != ERR_OK)
	{
		HxLOG_Critical("\n\n");
		return ERR_FAIL;
	}

	return PAL_SC_Transfer(hPalSci, pucWriteBuf, ulNumWrite, pucReadBuf, pulNumRead, ulEpxectedNumRead);
}

HERROR svc_cas_PalPortScRead(SciHandle_t hSci, HUINT8 *pucWriteBuf, HUINT32 ulNumWrite, HUINT8 *pucReadBuf, HUINT32 *pulNumRead)
{
	PalSc_Handle_t		hPalSci;

	if(local_svc_cas_PalPortScConvertSCIHandleRmToPal(hSci, &hPalSci) != ERR_OK)
	{
		HxLOG_Critical("\n\n");
		return ERR_FAIL;
	}

	return PAL_SC_Read(hPalSci, pucWriteBuf, ulNumWrite, pucReadBuf, pulNumRead);
}

HERROR svc_cas_PalPortScWrite(SciHandle_t hSci, HUINT8 *pucWriteBuf, HUINT32 ulNumWrite, HUINT8 *pucReadBuf, HUINT32 *pulNumRead)
{
	PalSc_Handle_t		hPalSci;

	if(local_svc_cas_PalPortScConvertSCIHandleRmToPal(hSci, &hPalSci) != ERR_OK)
	{
		HxLOG_Critical("\n\n");
		return ERR_FAIL;
	}

	return PAL_SC_Write(hPalSci, pucWriteBuf, ulNumWrite, pucReadBuf, pulNumRead);
}


HERROR svc_cas_PalPortScSetParams(SciHandle_t hSci, SvcCas_DevSciScParam_t *pstParam)
{
	PalSc_Handle_t		hPalSci;
	PalSc_Param_t	stPalParam;

	if(local_svc_cas_PalPortScConvertSCIHandleRmToPal(hSci, &hPalSci) != ERR_OK)
	{
		HxLOG_Critical("\n\n");
		return ERR_FAIL;
	}

	if(local_svc_cas_PalPortScConvertScParamRmToPal(pstParam, &stPalParam) != ERR_OK)
	{
		HxLOG_Critical("\n\n");
		return ERR_FAIL;
	}

	return PAL_SC_SetParams(hPalSci, &stPalParam);
}

HERROR svc_cas_PalPortScGetParams(SciHandle_t hSci, SvcCas_DevSciScParam_t *pstParam)
{
	PalSc_Handle_t		hPalSci;
	PalSc_Param_t	stPalParam;

	if(local_svc_cas_PalPortScConvertSCIHandleRmToPal(hSci, &hPalSci) != ERR_OK)
	{
		HxLOG_Critical("\n\n");
		return ERR_FAIL;
	}

	if(PAL_SC_GetParams(hPalSci, &stPalParam) != ERR_OK)
	{
		return ERR_FAIL;
	}

	if(local_svc_cas_PalPortScConvertScParamPalToRm(&stPalParam, pstParam) != ERR_OK)
	{
		return ERR_FAIL;
	}

	return ERR_OK;
}

HERROR svc_cas_PalPortScNotifyScValidity(SciHandle_t hSci, SvcCas_DevScmFlag_e eMatched)										/* handle에 해당하는 CARM id 및 slot이 init 완료 됨을 의미 */
{
	PalSc_Handle_t	hPalSci ;
	PalSc_Flag_e	ePalMatched;

	if(local_svc_cas_PalPortScConvertSCIHandleRmToPal(hSci, &hPalSci) != ERR_OK)
	{
		HxLOG_Critical("\n\n");
		return ERR_FAIL;
	}

	if(local_svc_cas_PalPortScConvertScFlagRmToPal(eMatched, &ePalMatched) != ERR_OK)
	{
		HxLOG_Critical("\n\n");
		return ERR_FAIL;
	}

	return PAL_SC_NotifyScValidity(hPalSci, ePalMatched);
}


HERROR svc_cas_PalPortScNegotiatePTS(SciHandle_t hSci, HUINT8 *pucWriteBuf, HUINT8 ucNumToWrite, HUINT8 *pucReadBuf, HUINT8 *pucNumRead)
{
	PalSc_Handle_t	hPalSci;

	if(local_svc_cas_PalPortScConvertSCIHandleRmToPal(hSci, &hPalSci) != ERR_OK)
	{
		HxLOG_Critical("\n\n");
		return ERR_FAIL;
	}

	return PAL_SC_NegotiatePTS(hPalSci, pucWriteBuf, ucNumToWrite, pucReadBuf, pucNumRead);
}

HERROR svc_cas_PalPortScGetCardStatus(SciHandle_t hSci, SvcCas_DevSciScStatus_e *peStatus)
{
	HERROR hErr;
	PalSc_Handle_t		hPalSci;
	PalSc_CardStatus_e	ePalStatus;

	if(local_svc_cas_PalPortScConvertSCIHandleRmToPal(hSci, &hPalSci) != ERR_OK)
	{
		HxLOG_Critical("\n\n");
		return ERR_FAIL;
	}

	hErr = PAL_SC_GetCardStatus(hPalSci, &ePalStatus);

	if(hErr != ERR_OK)
	{
		HxLOG_Critical("\n\n");
		return hErr;
	}

	hErr = local_svc_cas_PalPortScConvertScStatusPalToRm(ePalStatus, peStatus);

	if(hErr != ERR_OK)
	{
		HxLOG_Critical("\n\n");
		return hErr;
	}

	return hErr;
}

HERROR svc_cas_PalPortScGetCardStatusBySlotID(HUINT32 ulSlotId, SvcCas_DevSciScStatus_e *peStatus)
{
	HERROR hErr;
	PalSc_CardStatus_e	ePalStatus;

	if(ulSlotId >= PAL_SC_GetNumberOfModule())
	{
		return ERR_FAIL;
	}

	hErr = PAL_SC_GetCardStatusBySlotID(ulSlotId, &ePalStatus);

	if(hErr != ERR_OK)
	{
		HxLOG_Error("PAL_SC_GetCardStatusBySlotID() error\n");
		return hErr;
	}

	hErr = local_svc_cas_PalPortScConvertScStatusPalToRm(ePalStatus, peStatus);

	if(hErr != ERR_OK)
	{
		HxLOG_Error("local_svc_cas_PalPortScConvertScStatusPalToRm() error\n");
		return hErr;
	}

	return hErr;
}

HUINT32 svc_cas_PalPortScGetNumberOfModule(void)
{
	return PAL_SC_GetNumberOfModule();
}

HERROR svc_cas_PalPortScActivate(SciHandle_t hSci)
{
	PalSc_Handle_t		hPalSc;

	if(local_svc_cas_PalPortScConvertSCIHandleRmToPal(hSci, &hPalSc) != ERR_OK)
	{
		HxLOG_Critical("\n\n");
		return ERR_FAIL;
	}

	return PAL_SC_Activate(hPalSc);
}
#endif


#if defined(CONFIG_MW_CI)
HUINT16 svc_cas_PalPortCiGetNumberOfModule(void)
{
	return PAL_CI_GetNumberOfModule();
}

HUINT16 svc_cas_PalPortCiGetMaxNumOfCISlot(void)
{
	return PAL_CI_GetMaxSlotNumber();
}

SvcCas_DevCiState_e svc_cas_PalPortCiCheckModule (CI_SLOT_ID usSlotId) /* 캠 삽입 유무를 리턴함 */
{
	SvcCas_DevCiState_e eState = eCAS_DEV_CI_EXTRACTED;
	PalCi_State_e ePalState;

	ePalState = PAL_CI_CheckModule (usSlotId);

	local_svc_cas_PalPortCiConvertCamStatePalToRm(ePalState, &eState);

	return eState;
}

HERROR svc_cas_PalPortCiResetModule (CI_SLOT_ID usSlotId) /* 캠 모듈을 리셋함 */
{
	return PAL_CI_ResetModule (usSlotId);
}

HERROR svc_cas_PalPortCiSetCamType(CI_SLOT_ID usSlotId, SvcCas_DevCiCamType_e eType)
{
	PalCi_CamType_e eCnvtType = ePAL_CI_CI_V1;

	if(local_svc_cas_PalPortCiConvertCamTypeRmToPal(eType, &eCnvtType) != ERR_OK)
	{
		HxLOG_Critical("\n\n");
		return ERR_FAIL;
	}

	return PAL_CI_SetCamType(usSlotId, eCnvtType);
}

HINT32 svc_cas_PalPortCiOpen (HUINT8 *device_name)
{
	return PAL_CI_Open(device_name);
}

HUINT8 *svc_cas_PalPortCiGetDeviceName(void)
{
	return (HUINT8 *)	DRV_DEV;
}

HINT32 svc_cas_PalPortCiClose (HUINT16 usSlotId)
{
	return PAL_CI_Close(usSlotId);
}

HERROR	svc_cas_PalPortCiPlugInCi (HUINT32 ulActionId, HINT32 nSlotId)
{
	return PAL_PIPE_SetCi (ulActionId, nSlotId);
}

HERROR svc_cas_PalPortCiGetPath(HUINT32 ulDemuxId, HINT32 *pnCI_Id)
{
	return PAL_CI_GetPath(ulDemuxId, pnCI_Id);
}

HERROR svc_cas_PalPortCiSetTsPath (CI_SLOT_ID usSlotId, SvcCas_DevCiPath_e path) /* bypass 또는 pass-thru를 지정하기 위한 함수 */
{
	PalCi_Path_e eCnvtPath = ePAL_CI_BYPASS;

	if(local_svc_cas_PalPortCiConvertPathRmToPal(path, &eCnvtPath) != ERR_OK)
	{
		HxLOG_Critical("\n\n");
		return ERR_FAIL;
	}

	return PAL_CI_SetTsPath (usSlotId, eCnvtPath);
}

HERROR svc_cas_PalPortCiRegisterCallback (void (*CiCallbackFunc)(CI_SLOT_ID usSlotId, unsigned short flag)) /* 캠의 상태에 따라 CI 드라이버로부터 호출되기를 원하는 콜백을 등록하기 위한 함수 */
{
	return PAL_CI_RegisterCallback(CiCallbackFunc);
}

HERROR svc_cas_PalPortCiGetStatusCI(unsigned short usSlotId, SvcCas_DevCiConnectStatue_t *pstCIStatus)
{
	HERROR						hErr;
	PalCi_ConnectStatus_e	stCIStatus;

	if(pstCIStatus == NULL)
	{
		HxLOG_Critical("\npstCIStatus is NULL\n");
		return ERR_FAIL;
	}

	hErr = PAL_CI_GetStatusCI(usSlotId, &stCIStatus);
	if(hErr != ERR_OK)
	{
		HxLOG_Critical("\n\n");
		return ERR_FAIL;
	}

	hErr = local_svc_cas_PalPortCiConvertCasMgrPalConnectState2CasCiConnectState(stCIStatus.ePalCiState, &pstCIStatus->eCiConnectState);
	if(hErr != ERR_OK)
	{
		HxLOG_Critical("\n\n");
		return ERR_FAIL;
	}

	hErr = local_svc_cas_PalPortCiConvertCasMgrPalConnectWay2CasCiConnectWay(stCIStatus.ePalConnectWay, &pstCIStatus->eCiConnectWay);
	if(hErr != ERR_OK)
	{
		HxLOG_Critical("\n\n");
		return ERR_FAIL;
	}

	return ERR_OK;
}

#if defined(CONFIG_MW_CI_USE_EXTERNAL_CONTROLLER)
HERROR svc_cas_PalPortCiGetCurrentPathWay(HUINT32 unSlotId, SvcCas_DevCiPathway_e *peCiPathWay)
{
	return PAL_CI_GetCurrentPathWay(unSlotId, (PalCi_PathWay_e *)peCiPathWay);
}

HERROR svc_cas_PalPortCiSetExtendedTsPath(HUINT32 unTunerId, HUINT32 unSlotId, HUINT32 unDemuxId, SvcCas_DevCiPathway_e eCarmCiPathWay)
{
	return PAL_CI_SetExtendedTsPath(unTunerId, unSlotId, unDemuxId, eCarmCiPathWay);
}
#endif

HINT32 svc_cas_PalPortCiWrite (HUINT16 usSlotId, HUINT16 len, HUINT8 *pbytes)
{
	return PAL_CI_Write(usSlotId, len, pbytes);
}

HINT32 svc_cas_PalPortCiRead (HUINT16 usSlotId, HUINT16 len, HUINT8 *pbytes)
{
	return PAL_CI_Read(usSlotId, len, pbytes);
}

HINT32 svc_cas_PalPortCiIoctl (HUINT16 usSlotId, HINT32 command, void *parg)
{
#if 0
	CARM_CI_stAddr *pstAddr;
	DRV_stAddr stAddr;
	CARM_CI_stMem*pstMem;
	DRV_stMem stMem;
	CARM_CI_stIO *pstIo;
	DRV_stIO stIo;
	CARM_CI_stSignal *pstSignal;
	DRV_stSignal stSignal;
	CARM_CI_ssSignal*pssSignal;
	DRV_ssSignal ssSignal;
	void *pOut;
	HINT32 nCnvtCommand;

	switch (command)
	{
		case CARM_CI_ADDR:
			pstAddr = (CARM_CI_stAddr *)parg;
			stAddr.addr = pstAddr->addr;
			pOut = (void *)&stAddr;
			break;

		case CARM_CI_READMEM:
		case CARM_CI_WRITEMEM:
			pstMem = (CARM_CI_stMem *)parg;
			stMem.addr = pstMem->addr;
			stMem.len = pstMem->len;
			stMem.pbytes = pstMem->pbytes;
			stMem.rlen = pstMem->rlen;
			pOut = (void *)&stMem;
			break;

		case CARM_CI_READIO:
		case CARM_CI_WRITEIO:
			pstIo= (CARM_CI_stIO *)parg;
			stIo.registr = pstIo->registr;
			stIo.pvalue = pstIo->pvalue;
			pOut = (void *)&stIo;
			break;

			break;
		case CARM_CI_TSIGNAL:
			pstSignal = (CARM_CI_stSignal *)parg;
			stSignal.sig = pstSignal->sig;
			stSignal.value = pstSignal->value;
			pOut = (void *)&stSignal;
			break;

		case CARM_CI_SSIGNAL:
			pssSignal = (CARM_CI_ssSignal *)parg;
			ssSignal.pin = pssSignal->pin;
			ssSignal.cs  = pssSignal->cs;
			pOut = (void *)&ssSignal;
			break;
		default:
			return ERR_FAIL;
	}

	local_carm_CI_ConvertCommandRmToPal(command, &nCnvtCommand);

	return PAL_CI_Ioctl(usSlotId, nCnvtCommand, pOut);
#else
	return PAL_CI_Ioctl(usSlotId, command, parg);
#endif
}


#endif

#define __________DSC___________

#if defined(CONFIG_MW_CAS) || defined(CONFIG_MW_CI_PLUS)
/****************************************************************************************
	functions to alternative PAL functions
****************************************************************************************/
#define _________Descrambler_____________
/* dsc */
HERROR svc_cas_PalPortDscInit(void)
{
	return PAL_DSC_Init();
}

HERROR svc_cas_PalPortDscCloseAll(void)
{
	return PAL_DSC_CloseAll();
}



HERROR svc_cas_PalPortDscResetPid(HUINT32 ulDscIndex, HUINT16 usPid)
{
	return PAL_DSC_ResetPid(ulDscIndex, usPid);
}

HERROR svc_cas_PalPortDscSetProtectionKey(HUINT32 ulDscIndex, HUINT8 *pucKeyData)
{
	return PAL_DSC_SetProtectionKey(ulDscIndex, pucKeyData);
}

HERROR svc_cas_PalPortDscClearProtectionKey(HUINT32 ulDscIndex)
{
	return PAL_DSC_ClearProtectionKey(ulDscIndex);
}

HERROR	local_svc_cas_PalPortDscDSC_ConvertDscKeyParityRmToPal(SvcCas_DevDscKeyParity_e inType, PalDsc_KeyParity_e *pCnvtType)
{
	if (NULL == pCnvtType)
	{
		return ERR_FAIL;
	}

	if(inType == eCAS_DSC_PARITY_ODD)
	{
		*pCnvtType = ePAL_DSC_KEYPARITY_ODD;
	}
	else if(inType == eCAS_DSC_PARITY_EVEN)
	{
		*pCnvtType = ePAL_DSC_KEYPARITY_EVEN;
	}
	else
	{
		HxLOG_Critical("\n\n");
		return ERR_FAIL;
	}

	return ERR_OK;
}

HERROR svc_cas_PalPortDscSetKey(HUINT32 ulDscIndex, SvcCas_DevDscKeyParity_e eParity, HUINT8 *pucKeyData)
{
	PalDsc_KeyParity_e ePalParity;

	if(local_svc_cas_PalPortDscDSC_ConvertDscKeyParityRmToPal(eParity, &ePalParity) != ERR_OK)
	{
		HxLOG_Critical("\n\n");
		return ERR_FAIL;
	}

	return PAL_DSC_SetKey(ulDscIndex, ePalParity, pucKeyData);
}


HERROR svc_cas_PalPortDscSetAesKey(HUINT32 ulDscIndex, HUINT8 *pucKeyData, HUINT32 ulKeySize)
{
	return PAL_DSC_SetAesKey(ulDscIndex, pucKeyData, ulKeySize);
}






HERROR svc_cas_PalPortDscOpen(HUINT32 ulDemuxId, SvcCas_DevDscType_e eDscType, SvcCas_DscDataType_e eDataType, HUINT32 *pulDscIndex)
{
	PalDsc_DscType_e		ePalDscType;
	PalDsc_DataType_e	ePalDataType;

	if(local_svc_cas_PalPortDscConvertDscTypeRmToPal(eDscType, &ePalDscType) != ERR_OK)
	{
		HxLOG_Critical("\n\n");
		return ERR_FAIL;
	}

	if(local_svc_cas_PalPortDscConvertDscDataTypeRmToPal(eDataType, &ePalDataType) != ERR_OK)
	{
		HxLOG_Critical("\n\n");
		return ERR_FAIL;
	}

	return PAL_DSC_Open(ulDemuxId, ePalDscType, ePalDataType, pulDscIndex);
}

HERROR svc_cas_PalPortDscClose(HUINT32 ulDscIndex)
{
	return PAL_DSC_Close(ulDscIndex);
}

#if defined(CONFIG_MW_CI) && defined(CONFIG_MW_CI_PLUS)
HERROR svc_cas_PalPortDscSetKeyForCiPlus(HUINT32 ulDscIndex, SvcCas_DevDscType_e eDscType, SvcCas_DevDscKeyParity_e eParity, HUINT8 *pucKeyData, HUINT8 *pucInitVec)
{
	PalDsc_DscType_e		ePalDscType;
	PalDsc_KeyParity_e 	ePalParity;

	if(local_svc_cas_PalPortDscConvertDscTypeRmToPal(eDscType, &ePalDscType) != ERR_OK)
	{
		HxLOG_Critical("\n\n");
		return ERR_FAIL;
	}

	if(local_svc_cas_PalPortDscConvertDscKeyParityRmToPal(eParity, &ePalParity) != ERR_OK)
	{
		HxLOG_Critical("\n\n");
		return ERR_FAIL;
	}

	return PAL_DSC_SetKeyForCiPlus(ulDscIndex, ePalDscType, ePalParity, pucKeyData, pucInitVec);
}
#endif

HERROR svc_cas_PalPortDscSetPid(HUINT32 ulDscIndex, HUINT16 usPid)
{
	return PAL_DSC_SetPid(ulDscIndex, usPid);
}

#if defined(CONFIG_MW_CAS_VERIMATRIX_IPTV)
HERROR svc_cas_PalPortDscSetVmxIptvDemuxId(HUINT32 ulDemuxId)
{
	return PAL_DSC_SetVmxIptvDemuxId(ulDemuxId);
}

HERROR svc_cas_PalPortDscSetVmxIptvEncryptionAlgorithm(HUINT8 ucSvcIdx, HUINT8 ucEncAlgo)
{
	return PAL_DSC_SetVmxIptvEncryptionAlgorithm(ucSvcIdx, ucEncAlgo);
}
#endif

#endif



#define __________DMX___________
static SvcCas_DevDmxTsState_e svc_cas_PalPortDmxConvertDmxStatePal2Cas(PalDmx_TsState_e ePalState)
{
	switch (ePalState)
	{
		case ePAL_DMX_TSSTATE_UNKNOWN:		return eCAS_DMX_TSSTATE_UNKNOWN;
		case ePAL_DMX_TSSTATE_CLEAN:		return eCAS_DMX_TSSTATE_CLEAN;
		case ePAL_DMX_TSSTATE_SCRAMBLED:	return eCAS_DMX_TSSTATE_SCRAMBLED;
		default:							break;
	}

	return eCAS_DMX_TSSTATE_UNKNOWN;
}

HBOOL svc_cas_PalPortDmxIsSupportScrambleBitChecking(HUINT32 ulDemuxId)
{
	return PAL_DMX_IsSupportScrambleBitChecking(ulDemuxId);
}

HERROR svc_cas_PalPortDmxCheckScrambleBitByPid(HUINT32 ulDemuxId, SvcCas_DevDmxTsState_e *peState)
{
	HERROR				hErr;
	PalDmx_TsState_e	ePalState;

	hErr = PAL_DMX_CheckScrambleBitByPid(ulDemuxId, &ePalState);
	if (ERR_OK == hErr)
	{
		*peState = svc_cas_PalPortDmxConvertDmxStatePal2Cas(ePalState);
	}

	return hErr;
}



#define __________CRYPT___________
HERROR svc_cas_PalPortSysEncrypt(HUINT8 *pucKey, HBOOL bBlock, HUINT8 *pucPlain, HUINT8 *pucEncrypted, HUINT32 ulSize)
{
	return PAL_SYS_Encrypt(pucKey, bBlock, pucPlain, pucEncrypted, ulSize);
}
HERROR svc_cas_PalPortSysDecrypt(HUINT8 *pucKey, HBOOL bBlock, HUINT8 *pucPlain, HUINT8 *pucDecrypted, HUINT32 ulSize)
{
	return PAL_SYS_Decrypt(pucKey, bBlock, pucPlain, pucDecrypted, ulSize);
}

HERROR svc_cas_PalPortSysEncryptData(SvcCas_Crypt_t *pstSvcCasCrypt)
{
	PAL_CRYPT_CRYPTMODE_t      	ePalCryptMode;
	PAL_CRYPT_CRYPTKEYTYPE_t   	ePalCryptKeyType;
	PAL_Crypt_t 	            stCrypt;

	if (pstSvcCasCrypt == NULL)
	{
    	HxLOG_Error("Param. is NULL \n");
   	 	return ERR_FAIL;
	}

	switch(pstSvcCasCrypt->etCryptMode)
	{
    	case eCAS_CRYPT_AES      	: ePalCryptMode = PAL_CRYPT_AES; break;
    	case eCAS_CRYPT_DES      	: ePalCryptMode = PAL_CRYPT_DES; break;
    	case eCAS_CRYPT_TDES     	: ePalCryptMode = PAL_CRYPT_TDES; break;
    	case eCAS_CRYPT_AES_CTR		: ePalCryptMode = PAL_CRYPT_AES_CTR; break;
    	case eCAS_CRYPT_AES_CBC_MAC	: ePalCryptMode = PAL_CRYPT_AES_CBC_MAC; break;
    	case eCAS_CRYPT_TDES_DTCPIP	: ePalCryptMode = PAL_CRYPT_TDES_DTCPIP; break;
    	default :
        	return ERR_FAIL;
	}

	switch(pstSvcCasCrypt->etCryptKey)
	{
    	case eCAS_CRYPT_DEFAULT_SYSTEM_KEY   	: ePalCryptKeyType = PAL_CRYPT_DEFAULT_SYSTEM_KEY; break;
   		case eCAS_CRYPT_INTERNAL_CAS_KEY     	: ePalCryptKeyType = PAL_CRYPT_INTERNAL_CAS_KEY; break;
    	case eCAS_CRYPT_USER_KEY             	: ePalCryptKeyType = PAL_CRYPT_USER_KEY; break;
    	default :
       		return ERR_FAIL;
	}

	stCrypt.etCryptMode = ePalCryptMode;
	stCrypt.etCryptKey  = ePalCryptKeyType;
	stCrypt.pKey        = pstSvcCasCrypt->pKey;
	stCrypt.ulKeySize   = pstSvcCasCrypt->ulKeySize;
	stCrypt.pSrcBuffer  = pstSvcCasCrypt->pSrcBuffer;
	stCrypt.pDstBuffer  = pstSvcCasCrypt->pDstBuffer;
	stCrypt.ulBuffSize  = pstSvcCasCrypt->ulBuffSize;

	if (PAL_SYS_EncryptData(&stCrypt) != ERR_OK)
	{
		HxLOG_Error("DI_CRYPT_Encrypt fail  (0x%X)\n");
		return ERR_FAIL;
	}

	return ERR_OK;
}

HERROR svc_cas_PalPortSysDecryptData(SvcCas_Crypt_t *pstSvcCasCrypt)
{
	PAL_CRYPT_CRYPTMODE_t      	ePalCryptMode;
	PAL_CRYPT_CRYPTKEYTYPE_t   	ePalCryptKeyType;
	PAL_Crypt_t 	            stCrypt;

	if (pstSvcCasCrypt == NULL)
	{
    	HxLOG_Error("Param. is NULL \n");
   	 	return ERR_FAIL;
	}

	switch(pstSvcCasCrypt->etCryptMode)
	{
    	case eCAS_CRYPT_AES      	: ePalCryptMode = PAL_CRYPT_AES; break;
    	case eCAS_CRYPT_DES      	: ePalCryptMode = PAL_CRYPT_DES; break;
    	case eCAS_CRYPT_TDES     	: ePalCryptMode = PAL_CRYPT_TDES; break;
    	case eCAS_CRYPT_AES_CTR		: ePalCryptMode = PAL_CRYPT_AES_CTR; break;
    	case eCAS_CRYPT_AES_CBC_MAC	: ePalCryptMode = PAL_CRYPT_AES_CBC_MAC; break;
    	case eCAS_CRYPT_TDES_DTCPIP	: ePalCryptMode = PAL_CRYPT_TDES_DTCPIP; break;
    	default :
        	return ERR_FAIL;
	}

	switch(pstSvcCasCrypt->etCryptKey)
	{
    	case eCAS_CRYPT_DEFAULT_SYSTEM_KEY   	: ePalCryptKeyType = PAL_CRYPT_DEFAULT_SYSTEM_KEY; break;
   		case eCAS_CRYPT_INTERNAL_CAS_KEY     	: ePalCryptKeyType = PAL_CRYPT_INTERNAL_CAS_KEY; break;
    	case eCAS_CRYPT_USER_KEY             	: ePalCryptKeyType = PAL_CRYPT_USER_KEY; break;
    	default :
       		return ERR_FAIL;
	}

	stCrypt.etCryptMode = ePalCryptMode;
	stCrypt.etCryptKey  = ePalCryptKeyType;
	stCrypt.pKey        = pstSvcCasCrypt->pKey;
	stCrypt.ulKeySize   = pstSvcCasCrypt->ulKeySize;
	stCrypt.pSrcBuffer  = pstSvcCasCrypt->pSrcBuffer;
	stCrypt.pDstBuffer  = pstSvcCasCrypt->pDstBuffer;
	stCrypt.ulBuffSize  = pstSvcCasCrypt->ulBuffSize;

	if (PAL_SYS_DecryptData(&stCrypt) != ERR_OK)
	{
		HxLOG_Error("DI_CRYPT_Encrypt fail  (0x%X)\n");
		return ERR_FAIL;
	}

	return ERR_OK;
}

HERROR svc_cas_PalPortChiperSha1(HUINT8* pucMsg, HUINT32 ulMsgSize, HUINT8* pucDigest)
{
	PAL_CIPHER_SHA_1(pucMsg, ulMsgSize, pucDigest);
	return ERR_OK;
}

HERROR svc_cas_PalPortChiperSha256(HUINT8* pucMsg, HUINT32 ulMsgSize, HUINT8* pucDigest)
{
	PAL_CIPHER_SHA_256(pucMsg, ulMsgSize, pucDigest);
	return ERR_OK;
}

HERROR svc_cas_PalPortChiperHmacSha1(HUINT8 *pucKey, HUINT8 ucKeySize, HUINT8 *pucMsg, HUINT32 ulMsgSize, HUINT8 *pucHmacResult)
{
	return PAL_CHIPER_HMAC_SHA1(pucKey, ucKeySize, pucMsg, ulMsgSize, pucHmacResult);
}

HERROR svc_cas_PalPortSysEncryptAesData(HUINT8 *pucData, HUINT8 *pucEncBuf, HUINT32 ulSize)
{
#if defined(CONFIG_3RD_CRYPT)
	HUINT8 *pucKey 		= NULL, *pucIv = NULL;
	if(pucData == NULL || pucEncBuf == NULL || ulSize == 0)
	{
		return ERR_FAIL;
	}

	pucKey = PAL_SYS_GetEncryptionKey();
	pucIv = PAL_SYS_GetEncryptionIV();

	PAL_ENC_AES_CBC(pucKey, pucData, pucEncBuf, ulSize, pucIv);
	return ERR_OK;
#else
	return ERR_FAIL;
#endif
}

HERROR svc_cas_PalPortSysDecryptAesData(HUINT8 *pucData, HUINT8 *pucDecBuf, HUINT32 ulSize)
{
#if defined(CONFIG_3RD_CRYPT)
	HUINT8 *pucKey 		= NULL, *pucIv = NULL;
	if(pucData == NULL || pucDecBuf == NULL || ulSize == 0)
	{
		return ERR_FAIL;
	}

	pucKey = PAL_SYS_GetEncryptionKey();
	pucIv = PAL_SYS_GetEncryptionIV();

	PAL_DEC_AES_CBC(pucKey, pucData, pucDecBuf, ulSize, pucIv);
	return ERR_OK;
#else
	return ERR_FAIL;
#endif
}

HUINT8* svc_cas_PalPortSysGetEncryptionKey(void)
{
	return PAL_SYS_GetEncryptionKey();
}

#define __________PANEL___________
HERROR svc_cas_PalPortSetColdBootPanelCondition (HBOOL bDisplayVideo, HBOOL bViewPanel)
{
	return PAL_PANEL_SetColdBootCondition (bDisplayVideo, bViewPanel);
}

#define __________POWER___________
HERROR svc_cas_PalPortPowerReboot(void)
{
	return PAL_PWR_Reboot();
}

#define __________FLASH___________
HERROR svc_cas_PalPortNandFlashRead(HUINT32 ulAddress, HUINT8 *pucBuffer, HUINT32 ulSize)
{
	return PAL_SYS_NANDFLASH_Read(ulAddress, pucBuffer, ulSize);
}

HERROR svc_cas_PalPortFlashRead(HUINT32 ulAddress, HUINT8 *pucBuffer, HUINT32 ulSize)
{
	return PAL_SYS_FLASH_Read(ulAddress, pucBuffer, ulSize);
}

/********************************************************************
* For Debugging
********************************************************************/
#define ___09_Debugging_APIs___


/*********************** End of File ******************************/

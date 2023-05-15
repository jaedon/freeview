/* $Header:   //BASE/archives/NEO-HEPVR/NEO/mw/cas/ir3/ir_vd/vd_macrovision.c-arc   1.9   Jun 19 2007 11:15:02   khwoo  $ */

/********************************************************************
*	author : hongkw
*
*	source file descriptions....
*		: Irdeto Virtual Driver - descrambler ( 710454 Virtual Driver API - chap.8 )
*
*	Copyright (c) 2002 by Humax Co., Ltd.
*	All right reserved
********************************************************************/
/********************************************************************
*	Including Headers
********************************************************************/
#include <pal_sc.h>

#include <_svc_cas_sub_dev_api.h>

#include "vd_humax.h"

//#include "ir_common.h"
#include "ir_svc.h"
#include "vd_humax.h"


#include "s3_vd.h"
#include "s3_vd_macrovision.h"
#include "s3_vd_copycontrol.h"

/********************************************************************
*	Definitions (for this source file)
********************************************************************/



#define	VdCCDump		HxLOG_Info



/********************************************************************
*	External variables
********************************************************************/

/********************************************************************
*	External function prototypes
********************************************************************/

/********************************************************************
*	Global variables
********************************************************************/


/********************************************************************
*	Static variables
********************************************************************/

//mode byte which is changed by Softcell.
#define IR_MV_RECORD_CLEAR				0
#define IR_MV_RECORD_DESCRAMBLE			1
#define IR_MV_RECORD_ENCRYPTION			2
#define IR_MV_RECORD_FORBIDDEN			3


#define MAX_CONFIG_LENGTH				32

//max number is the same with max play
#define MAX_MACROVISION_REF_NUM			IR_HUMAX_MAX_SERVICE

typedef struct
{
	vd_ref_id	refId;
//	Handle_t	hPlay;
	HUINT8		modeByte;
	HUINT32 	svcInfoIndex;
	HUINT16					usCCTypeMask;
	vd_copycontrol_cci_e	eDigitalCCi;
	vd_copycontrol_cci_e	eAnalogCCi;
	HUINT8 		version;
	HUINT16 	configLength;
	HUINT16		configData[MAX_CONFIG_LENGTH];
}MACROVISION_INFO;

MACROVISION_INFO	s_GlobalMacrovisionInfo;
MACROVISION_INFO	s_MacrovisionInfo[MAX_MACROVISION_REF_NUM];


/********************************************************************
*	Static functions Prototypes
*	The following functions will be used as inner functions
********************************************************************/
/********************************************************************
*	function name :	 HUMAX_VD_MACROVISION_Initialise
*	input : void(none)
*	output : result
*	description : ..
********************************************************************/
ia_result HUMAX_VD_MACROVISION_Initialise(void)
{
	int i;

	HxLOG_Info("\n");

	for(i=0; i< MAX_MACROVISION_REF_NUM; i++)
	{
		s_MacrovisionInfo[i].refId = VD_REF_ID_ERROR;
		s_MacrovisionInfo[i].svcInfoIndex = 0xffff;
		s_MacrovisionInfo[i].usCCTypeMask = 0;
	}
	return IA_SUCCESS;
}


HUINT32 HUMAX_VD_MACROVISION_GetResources(void **ppvList, ia_word16 *pwCount )
{
	vd_copycontrol_resource_st	*pstResources;

	HxLOG_Info("\n");

	// TODO: followings are wroten based 1 +1 macro resource.
	*pwCount = HMX_VD_COPYCONTROL_RSC_MAX;

	pstResources = (vd_copycontrol_resource_st *)OxCAS_Malloc( *pwCount * sizeof(vd_copycontrol_resource_st));

	//	이정보를 보고 MacroVision IO가 동작함.
	pstResources[0].eCopyControlType = VD_COPYCONTROL_MACROVISION;
	pstResources[1].eCopyControlType = VD_COPYCONTROL_DIGITAL;
	pstResources[2].eCopyControlType = VD_COPYCONTROL_ANALOG;

	HxLOG_Print("Resource count = %d\n", *pwCount );

	*ppvList = pstResources;
	return eHVD_OK;

}
/********************************************************************
*	function name :	 HUMAX_VD_MACROVISION_Open
*	input : wResourceID
*	output : None
*	description :
********************************************************************/
vd_ref_id HUMAX_VD_MACROVISION_Open( vd_rsc_id wResourceID, void *pvData )
{
	vd_copycontrol_open_st stCopyControlType;

	HxSTD_memcpy(&stCopyControlType, pvData, sizeof(vd_copycontrol_open_st));

	HxLOG_Info("wResourceID= 0x%08X\n", wResourceID);

	s_MacrovisionInfo[wResourceID].refId = wResourceID;
	s_MacrovisionInfo[wResourceID].svcInfoIndex = wResourceID;

	if(stCopyControlType.eCopyControlType == VD_COPYCONTROL_MACROVISION)
	{
		s_MacrovisionInfo[wResourceID].usCCTypeMask |= MSG_COPYCONTROL_MACROVISION;
	}

	if(stCopyControlType.eCopyControlType == VD_COPYCONTROL_DIGITAL)
	{
		s_MacrovisionInfo[wResourceID].usCCTypeMask |= MSG_COPYCONTROL_DIGITAL;
	}

	if(stCopyControlType.eCopyControlType == VD_COPYCONTROL_ANALOG)
	{
		s_MacrovisionInfo[wResourceID].usCCTypeMask |= MSG_COPYCONTROL_ANALOG;
	}

	return wResourceID;

}



/********************************************************************
*	function name :	 HUMAX_VD_MACROVISION_Close
*	input : wRefId
*	output : result
*	description : ..
********************************************************************/
ia_result HUMAX_VD_MACROVISION_Close( vd_ref_id wRefId )
{
	int i;

	HxLOG_Info("wRefId= 0x%08X\n", wRefId);

	for(i=0; i< MAX_MACROVISION_REF_NUM; i++)
	{
		if(s_MacrovisionInfo[i].refId == wRefId)
		{
			s_MacrovisionInfo[i].refId = VD_REF_ID_ERROR;
			s_MacrovisionInfo[i].usCCTypeMask = 0;
			break;
		}
	}


	return IA_SUCCESS;
}

/********************************************************************
*	function name :	 HUMAX_VD_MACROVISION_IO
*	input : wRefId
*	output : result
*	description :
********************************************************************/
ia_result HUMAX_VD_MACROVISION_IO( vd_ref_id wRefId, ia_word16 wOpCode, ia_word32 wDataWriteSize, void *pvDataWrite, ia_word32 *pwDataReadSize, void *pvDataRead )
{
	vd_macrovision_mode_st		MacroVisionMode;
	vd_macrovision_config_st	MacroVisionConfig;
	vd_copycontrol_digital_cci_st	CopyControlDigitalCCi;
	vd_copycontrol_analog_cci_st	CopyControlAnalogCCi;
	HUINT8						*p = (HUINT8 *)pvDataWrite;
	HUINT32						i;

	MACROVISION_INFO			*pInfo;

	HxLOG_Info("wRefId= 0x%08X, wOpCode= %d, wDataWriteSize= %d,pwDataReadSize= %d\n", wRefId, wOpCode, wDataWriteSize,pwDataReadSize );

	pInfo = NULL;
	if(wRefId == VD_REF_ID_GLOBAL)
	{
		pInfo = &s_GlobalMacrovisionInfo;
	}
	else
	{
		for(i=0; i< MAX_MACROVISION_REF_NUM; i++)
		{
			if(s_MacrovisionInfo[i].refId == wRefId)
			{
				pInfo = &s_MacrovisionInfo[i];
				break;
			}
		}
		if(i == MAX_MACROVISION_REF_NUM)
		{
			HxLOG_Print("[VD_MACROVISION]IO Invalid refId=0x%x wOpCode=0x%x\n", wRefId, wOpCode);
			return IA_FAIL;
		}
	}
	if (pInfo == NULL)
	{
		HxLOG_Print("[VD_MACROVISION]pInfo == NULL\n");
		return IA_FAIL;
	}

	switch(wOpCode)
	{
	case	VD_MACROVISION_SET_MODE:
		HxLOG_Print("\tVD_MACROVISION_SET_MODE");

		HxSTD_memcpy((HUINT8 *)&MacroVisionMode, p, sizeof(vd_macrovision_mode_st));
		pInfo->modeByte = MacroVisionMode.bMode;

		HxLOG_Print("  bMode(%d)\n", MacroVisionMode.bMode);
		break;

	case	VD_MACROVISION_SET_CONFIG:
		HxLOG_Print("\tVD_MACROVISION_SET_CONFIG\n");

		HxSTD_memcpy((HUINT8 *)&MacroVisionConfig, p, sizeof(vd_macrovision_config_st));
		p += sizeof(vd_macrovision_config_st);

		pInfo->version = MacroVisionConfig.bVersion;
		pInfo->configLength= MacroVisionConfig.wConfigLength;
		HxSTD_memcpy(pInfo->configData, p, pInfo->configLength);

		HxLOG_Print("\tbVersion(0x%X)\n", MacroVisionConfig.bVersion);
		HxLOG_Print("\twConfigLength(0x%X)\n", MacroVisionConfig.wConfigLength);
		VdCCDump("\t[");
		for( i = 0; i < MacroVisionConfig.wConfigLength; i++ )
		{
			VdCCDump("%02X ", p[i]);
			if( i % 16 == 15 )
				VdCCDump("\n\t");
		}
		VdCCDump("]\n");
		break;

	case	VD_COPYCONTROL_DIGITAL_SET_CCI:
		HxLOG_Print("\tVD_COPYCONTROL_DIGITAL_SET_CCI");

		HxSTD_memcpy((HUINT8 *)&CopyControlDigitalCCi, p, sizeof(vd_copycontrol_digital_cci_st));
		pInfo->eDigitalCCi = CopyControlDigitalCCi.eCopyControlInfo;

		HxLOG_Print("  Digital CCi(%d)\n", pInfo->eDigitalCCi);
		break;

	case	VD_COPYCONTROL_ANALOG_SET_CCI:
		HxLOG_Print("\tVD_COPYCONTROL_ANALOG_SET_CCI");

		HxSTD_memcpy((HUINT8 *)&CopyControlAnalogCCi, p, sizeof(vd_copycontrol_analog_cci_st));
		pInfo->eAnalogCCi = CopyControlAnalogCCi.eCopyControlInfo;

		HxLOG_Print("  Analog CCi(%d)\n", pInfo->eAnalogCCi);
		break;


	default :
		HxLOG_Print("HUMAX_VD_MACROVISION_IO: what OP?(OpCode = 0x%X)\n",wOpCode );
		break;
	}

	return IA_SUCCESS;
}

#if defined(IRPLAT_SYS_PVR)

#if defined(DEBUG_BUILD) || defined(CONFIG_DEBUG)
static HUINT8 s_ucForceEncryptFlag = 0;
#endif

ia_result IR_MACROVISION_GetRecordMode(HUINT16 svcInfoIndex, HUINT8* pRecordMode)
{
	HUINT8 modeByte;

	HxLOG_Info("svcInfoIndex= 0x%08X, RecordMode= %d\n", svcInfoIndex, *pRecordMode);

#ifdef DEBUG_BUILD
//test Encryption by forcing MV byte to encryption
	if(s_ucForceEncryptFlag == 1)
		modeByte = IR_MV_RECORD_ENCRYPTION;
	else if(s_ucForceEncryptFlag == 2)
		modeByte = IR_MV_RECORD_FORBIDDEN;
	else
		modeByte = s_MacrovisionInfo[svcInfoIndex].modeByte & 0x03;
#else
	modeByte =  s_MacrovisionInfo[svcInfoIndex].modeByte & 0x03;
#endif

	switch(modeByte)
	{
		case IR_MV_RECORD_DESCRAMBLE:				//	regarding the irdeto PVR specification, record is allowed without calling SCELL service
			HxLOG_Print("Check MacroVision mode : IR_MV_RECORD_DESCRAMBLE - recording is available without calling SCELL\n");
			*pRecordMode = IR_RECORD_TYPE_DESCRAMBLE;
			break;

		case IR_MV_RECORD_ENCRYPTION:				//	regarding the irdeto PVR specification, record is allowed with calling SCELL service
			HxLOG_Print("Check MacroVision mode : IR_MV_RECORD_ENCRYPTION - recording is available with call SCELL\n");
			*pRecordMode = IR_RECORD_TYPE_CHECKMSK;
			break;

		case IR_MV_RECORD_FORBIDDEN:				//	regarding the irdeto PVR specification, record is not allowed.
			HxLOG_Print("Check MacroVision mode : IR_MV_RECORD_FORBIDDEN - recording is not allowed!!!\n");
			*pRecordMode = IR_RECORD_TYPE_FORBIDDEN;
			break;

		default:	// case IR_MV_RECORD_CLEAR:		//	clear service (need not descramble and pvr service)
			HxLOG_Print("Check MacroVision mode : IR_MV_RECORD_CLEAR or etc - recording is allowed (FTA service!!!)\n");
			*pRecordMode = IR_RECORD_TYPE_DESCRAMBLE;
			break;

	}

	return IA_SUCCESS;
}

#if defined(CONFIG_DEBUG) || defined(DEBUG_BUILD)
HINT32 IR_MV_DBG_ForceEncryptionForAllChannel(HUINT8 ucEncryptFlag)
{
	HxLOG_Info("ucEncryptFlag= %d\n", ucEncryptFlag);

	if(ucEncryptFlag == 0)
		s_ucForceEncryptFlag = 0;
	else if(ucEncryptFlag == 1)
		s_ucForceEncryptFlag = 1;
	else if(ucEncryptFlag == 2)
		s_ucForceEncryptFlag = 2;
	else
		HxLOG_Print("[IR_MV]invalid flag\n");

	return ERR_OK;
}
#endif
#endif

#if defined(IRPLAT_FN_COPY_CONTROL)
ia_result IR_MACROVISION_GetCopyControlInfo(HUINT16 svcInfoIndex,
														HUINT16 *pusTypeMask,
														HUINT8 *pucMcMode,
														HUINT8 *pucDigitalCci,
														HUINT8 *pucAnologCci)
{
	HxLOG_Print("svcInfoIndex= 0x%08X\n", svcInfoIndex);

	if(svcInfoIndex >= MAX_MACROVISION_REF_NUM)
	{
		HxLOG_Critical("");


		return IA_FALSE;
	}

	if(pusTypeMask == NULL || pucMcMode == NULL || pucDigitalCci == NULL || pucAnologCci == NULL)
	{
		HxLOG_Critical("");


		return IA_FALSE;
	}

	*pusTypeMask = s_MacrovisionInfo[svcInfoIndex].usCCTypeMask;
	*pucMcMode = (HUINT8)(s_MacrovisionInfo[svcInfoIndex].modeByte >> 4) & 0x03;
	*pucDigitalCci = (HUINT8)s_MacrovisionInfo[svcInfoIndex].eDigitalCCi;
	*pucAnologCci = (HUINT8)s_MacrovisionInfo[svcInfoIndex].eAnalogCCi;

	HxLOG_Print("Mask[0x%04x], Mode[%d], Digital CCI[%d], Analog CCI[%d]\n", *pusTypeMask, *pucMcMode, *pucDigitalCci, *pucAnologCci);

	return IA_SUCCESS;
}
#endif

/* end of file */


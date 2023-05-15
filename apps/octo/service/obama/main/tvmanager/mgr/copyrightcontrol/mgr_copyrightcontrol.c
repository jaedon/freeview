/**
	@file     mgr_copyrightcontrol.c
	@brief    .

	Description:
	Module: MGR/CopyrightControl			 	\n

	Copyright (c) 2013 HUMAX Co., Ltd.				\n
	All rights reserved.							\n
*/

/*******************************************************************/
/********************      Header Files    *************************/
/*******************************************************************/
#include <octo_common.h>


#include <bus.h>
#include <mgr_common.h>

#include <mgr_copyrightcontrol.h>
#include <pmgr_copyrightcontrol.h>

#include <get_enum_str.h>

/*******************************************************************/
/********************      Defines         *************************/
/*******************************************************************/


// ============================================================================
// DRM Message Convert
// ============================================================================

// APS DRM Info :
#if 0
#define DRM_APS_TRICKMODECTRL_BIT			0x00000007				// 0~2 bit
#define DRM_APS_TRICKMODECTRL_SHIFT			0						// 0 bit shift

#define DRM_APS_SVCBLOCK_BIT				0x00000008				// 3bit
#define DRM_APS_SVCBLOCK_SHIFT				3

#define DRM_APS_DIGICOPYPROTECT_BIT			0x00000010				// 4bit
#define DRM_APS_DIGICOPYPROTECT_SHIFT		4

#define DRM_APS_ANACOPYPROTECT_BIT			0x00000020				// 5bit
#define DRM_APS_ANACOPYPROTECT_SHIFT		5

#define DRM_APS_STRGRIGHT_BIT				0x000001C0				// 6~8bit
#define DRM_APS_STRGRIGHT_SHIFT				6

#define DRM_APS_STRGENCYPT_BIT				0x00000200				// 9bit
#define DRM_APS_STRGENCYPT_SHIFT			9

#define DRM_APS_COMPVIDCTRL_BIT				0x00000C00				// 10~11bit
#define DRM_APS_COMPVIDCTRL_SHIFT			10

#define DRM_APS_TRANSCTRL_BIT				0x00001000				// 12bit
#define DRM_APS_TRANSCTRL_SHIFT				12

#define DRM_APS_BLUCOMCTRL_BIT				0x00002000				// 13bit
#define DRM_APS_BLUCOMCTRL_SHIFT			13


/* SKYD HDCP Related Bits */
#define DRM_SKYD_HDCP_BIT					0x00000001				// 0bit
#define DRM_SKYD_EIT_RCVED_BIT				0x00000002				// 1bit
#define DRM_SKYD_EIT_TIMEOUT_BIT			0x00000004				// 2bit
#define DRM_SKYD_SDT_RCVED_BIT				0x00000008				// 3bit
#define DRM_SKYD_SDT_TIMEOUT_BIT			0x00000010				// 4bit
#define DRM_SKYD_SDT_EIT_PF_BIT				0x00000020				// 5bit



/*******************************************************************/
/********************      Typedef         *************************/
/*******************************************************************/

/*******************************************************************/
/********************      Global Variables     ********************/
/*******************************************************************/

/*******************************************************************/
/********************      Static Variables     ********************/
/*******************************************************************/

/*******************************************************************/
/******************* Static Function Prototypes  *************************/
/*******************************************************************/


STATIC HERROR mgr_copyrightcontrol_ConvertCcInfoToCcMsgArg_Aps (DxCopyrightControl_NagraSes_t stDrmInfo, HUINT32 *p1, HUINT32 *p2, HUINT32 *p3)
{
	if (p1 == NULL || p2 == NULL || p3 == NULL)
	{
		return ERR_FAIL;
	}

	*p1 = eDrmModelType_ApsHd;
	*p2 = 0;
	*p3 = 0;

	// Trick Mode Control
	*p2 |= ((HUINT32)stDrmInfo.eTrickModeCtrl) & DRM_APS_TRICKMODECTRL_BIT;

	// Service Blocking
	if (stDrmInfo.bSvcBlock != FALSE)
	{
		*p2 |= DRM_APS_SVCBLOCK_BIT;
	}

	// Digital Copy Protection
	if (stDrmInfo.bDigiCpProtect != FALSE)
	{
		*p2 |= DRM_APS_DIGICOPYPROTECT_BIT;
	}

	// Analog Copy Protection
	if (stDrmInfo.bAnaCpProtect != FALSE)
	{
		*p2 |= DRM_APS_ANACOPYPROTECT_BIT;
	}

	// Storage Rights
	*p2 |= ((HUINT32)stDrmInfo.eStorageRight << DRM_APS_STRGRIGHT_SHIFT) & DRM_APS_STRGRIGHT_BIT;

	// Storage Encryption
	if (stDrmInfo.bStorageEncrypt != FALSE)
	{
		*p2 |= DRM_APS_STRGENCYPT_BIT;
	}

	// Component Video Control
	*p2 |= ((HUINT32)stDrmInfo.eCompVidCtrl << DRM_APS_COMPVIDCTRL_SHIFT) & DRM_APS_COMPVIDCTRL_BIT;

	// Transcoding Control
	if (stDrmInfo.bTransCtrl != FALSE)
	{
		*p2 |= DRM_APS_TRANSCTRL_BIT;
	}

	// Blucom Control
	if (stDrmInfo.bBlucomCtrl != FALSE)
	{
		*p2 |= DRM_APS_BLUCOMCTRL_BIT;
	}

	return ERR_OK;
}

STATIC HERROR mgr_copyrightcontrol_ConvertCcMsgArgToCcInfo_Aps (HUINT32 p1, HUINT32 p2, HUINT32 p3, DxCopyrightControl_NagraSes_t *pstDrmInfo)
{
	(void)p3;

	if (pstDrmInfo == NULL)
	{
		return ERR_FAIL;
	}

	if (p1 != eDrmModelType_ApsHd)
	{
		return ERR_FAIL;
	}

	// Trick Mode Control
	pstDrmInfo->eTrickModeCtrl = (HUINT8)(p2 & DRM_APS_TRICKMODECTRL_BIT);

	// Service Blocking
	pstDrmInfo->bSvcBlock = (p2 & DRM_APS_SVCBLOCK_BIT) ? TRUE : FALSE;

	// Digital Copy Protection
	pstDrmInfo->bDigiCpProtect = (p2 & DRM_APS_DIGICOPYPROTECT_BIT) ? TRUE : FALSE;

	// Analog Copy Protection
	pstDrmInfo->bAnaCpProtect = (p2 & DRM_APS_ANACOPYPROTECT_BIT) ? TRUE : FALSE;

	// Storage Rights
	pstDrmInfo->eStorageRight = (HUINT8)((p2 & DRM_APS_STRGRIGHT_BIT) >> DRM_APS_STRGRIGHT_SHIFT);

	// Storage Encryption
	pstDrmInfo->bStorageEncrypt = (p2 & DRM_APS_STRGENCYPT_BIT) ? TRUE : FALSE;

	// Component Video Control
	pstDrmInfo->eCompVidCtrl = (HUINT8)((p2 & DRM_APS_COMPVIDCTRL_BIT) >> DRM_APS_COMPVIDCTRL_SHIFT);

	// Transcoding Control
	pstDrmInfo->bTransCtrl = (p2 & DRM_APS_TRANSCTRL_BIT) ? TRUE : FALSE;

	// Blucom Control
	pstDrmInfo->bBlucomCtrl = (p2 & DRM_APS_BLUCOMCTRL_BIT) ? TRUE : FALSE;

	return ERR_OK;


}



STATIC HERROR mgr_copyrightcontrol_ConvertCcInfoToCcMsgArg_Ciplus (DxCopyrightControl_CiPlus_t stDrmInfo, HUINT32 *p1, HUINT32 *p2, HUINT32 *p3)
{
	if (p1 == NULL || p2 == NULL || p3 == NULL)
	{
		return ERR_FAIL;
	}

	*p1 = eDxCopyrightControl_CasCcType_CiPlus;
	*p2 = 0;
	*p3 = 0;

	if (stDrmInfo.eCiPlusDrmType == eDxCopyrightControl_CiPlusDrmType_Uri)
	{
		/*
			p2 : CiPlusDrmType(3bits) Mode(2bits) DOT(1bit) RCT(1bit) ICT(1bit) EMI(2bits) APS(2bits)
			p3 : RL(8bits)
		*/

		// CI+ DRM Type : 3 bits
		*p2 |= ((HUINT32)stDrmInfo.eCiPlusDrmType) & 0x07;

		// Analogue Protection System (APS) for Macrovision on/off : 2 bits
		*p2 |= ((HUINT32)stDrmInfo.ciplusDrmInfo.stCiPlusUriInfo.ucAps << 3) & 0x18;

		// Encryption Mode Indicator (EMI) or Copy Control Information (CCI) : 2 bits
		*p2 |= ((HUINT32)stDrmInfo.ciplusDrmInfo.stCiPlusUriInfo.ucEmi << 5) & 0x60;

		// Image Constrained Trigger (ICT) for HD downscale on/off : 1 bit
		*p2 |= ((HUINT32)stDrmInfo.ciplusDrmInfo.stCiPlusUriInfo.ucIct << 7) & 0x80;

		// Redistribution Control Trigger (RCT) for Encryption Plus Non-assert : 1 bit (for EMI 관련 추가 제어용)
		*p2 |= ((HUINT32)stDrmInfo.ciplusDrmInfo.stCiPlusUriInfo.ucRct << 8) & 0x100;

			// Digital Only Token (DOT) for analogue video output : 1 bit
		*p2 |= ((HUINT32)stDrmInfo.ciplusDrmInfo.stCiPlusUriInfo.ucDot << 9) & 0x200;

		// Modes
		*p2 |= ((HUINT32)stDrmInfo.ciplusDrmInfo.stCiPlusUriInfo.ucMode << 10) & 0x400;

		// Retention Limit (RL) : 0 means unlimited retention
		*p3 |= (HUINT32)stDrmInfo.ciplusDrmInfo.stCiPlusUriInfo.ulMinutesForRL;
	}
	else if (stDrmInfo.eCiPlusDrmType == eDxCopyrightControl_CiPlusDrmType_RecCtrl)
	{
		// CI+ DRM Type : 3 bits
		*p2 |= ((HUINT32)stDrmInfo.eCiPlusDrmType) & 0x07;

		// CI+ Record Protocol usage information by CC resource : 3 bits
		*p2 |= ((HUINT32)stDrmInfo.ciplusDrmInfo.stCiPlusRecCtrl.eCiPlusCcRecInfo << 3) & 0x38;
	}

	return ERR_OK;
}

STATIC HERROR mgr_copyrightcontrol_ConvertUriMsgArgToUriInfo_Ciplus (HUINT32 p1, HUINT32 p2, HUINT32 p3, DxCopyrightControl_CiPlus_t *pstDrmInfo)
{
	(void)p3;

	if (pstDrmInfo == NULL)
	{
		return ERR_FAIL;
	}

	if (p1 != eDxCopyrightControl_CasCcType_CiPlus)
	{
		return ERR_FAIL;
	}

	pstDrmInfo->eCiPlusDrmType = (eDxCopyrightControl_CiPlusDrmType_e)(p2 & 0x00000007);

	if(pstDrmInfo->eCiPlusDrmType == eDxCopyrightControl_CiPlusDrmType_Uri)
	{
		pstDrmInfo->ciplusDrmInfo.stCiPlusUriInfo.ucAps = (HUINT8)(p2 & 0x00000018) >> 3;
		pstDrmInfo->ciplusDrmInfo.stCiPlusUriInfo.ucEmi = (HUINT8)(p2 & 0x00000060) >> 5;
		pstDrmInfo->ciplusDrmInfo.stCiPlusUriInfo.ucIct = (HUINT8)(p2 & 0x00000080) >> 7;
		pstDrmInfo->ciplusDrmInfo.stCiPlusUriInfo.ucRct = (HUINT8)(p2 & 0x00000100) >> 8;
		pstDrmInfo->ciplusDrmInfo.stCiPlusUriInfo.ucDot = (HUINT8)(p2 & 0x00000200) >> 9;
		pstDrmInfo->ciplusDrmInfo.stCiPlusUriInfo.ucMode = (HUINT8)(p2 & 0x00000400) >> 10;
		pstDrmInfo->ciplusDrmInfo.stCiPlusUriInfo.ulMinutesForRL = (HUINT32)p3;
	}
	else if(pstDrmInfo->eCiPlusDrmType == eDxCopyrightControl_CiPlusDrmType_RecCtrl)
	{
		pstDrmInfo->ciplusDrmInfo.stCiPlusRecCtrl.eCiPlusCcRecInfo = (HUINT8)(p2 & 0x00000038) >> 3;
	}

	return ERR_OK;
}



STATIC HERROR mgr_copyrightcontrol_ConvertCcInfoToCcMsgArg_Nordig(DxCopyrightControl_Nordig_t stDrmInfo, HUINT32 *p1, HUINT32 *p2, HUINT32 *p3)
{
	if (p1 == NULL || p2 == NULL || p3 == NULL)
	{
		return ERR_FAIL;
	}

	*p1 = eDxCopyrightControl_SiCcType_Nordig;
	*p2 = (HUINT32)stDrmInfo.eContentProtectionLevel;

	return ERR_OK;
}

STATIC HERROR mgr_copyrightcontrol_ConvertCcMsgArgToCcInfo_Nordig (HUINT32 p1, HUINT32 p2, HUINT32 p3, DxCopyrightControl_Nordig_t *pstDrmInfo)
{
	(void)p3;

	if ( pstDrmInfo == NULL || p1 != eDxCopyrightControl_SiCcType_Nordig )
	{
		return ERR_FAIL;
	}

	pstDrmInfo->eContentProtectionLevel = p2;

	return ERR_OK;
}

STATIC HERROR mgr_copyrightcontrol_ConvertCcInfoToCcMsgArg_Viaccess(DxCopyrightControl_Viaccess_t stDrmInfo, HUINT32 *p1, HUINT32 *p2, HUINT32 *p3)
{
	if (p1 == NULL || p2 == NULL || p3 == NULL)
	{
		return ERR_FAIL;
	}

	*p1 = eDxCopyrightControl_CasCcType_Viaccess;
	*p2 = (HUINT32)stDrmInfo.bRecordForbidden;
	*p3 = (HUINT32)stDrmInfo.bTsrForbidden;

	return ERR_OK;
}

STATIC HERROR mgr_copyrightcontrol_ConvertCcMsgArgToCcInfo_Viaccess (HUINT32 p1, HUINT32 p2, HUINT32 p3, DxCopyrightControl_Viaccess_t *pstDrmInfo)
{
	(void)p3;

	if ( pstDrmInfo == NULL || p1 != eDxCopyrightControl_CasCcType_Viaccess )
	{
		return ERR_FAIL;
	}

	pstDrmInfo->bRecordForbidden = p2;
	pstDrmInfo->bTsrForbidden = p3;

	return ERR_OK;
}



STATIC HERROR mgr_copyrightcontrol_InitCcInfo_Aps (DxCopyrightControl_t *pstDrmInfo, DxCopyrightControl_RestrictLevel_e eDrmLevel)
{
	if (pstDrmInfo == NULL) 				{ return ERR_FAIL; }

	pstDrmInfo->eDrmType = eDrmModelType_ApsHd;

	switch (eDrmLevel)
	{
	case eDxCopyrightControl_RestrictLevel_Max:
	// 모든 것을 다 정지시킨다.
		pstDrmInfo->info.stNagraDrmInfo_Ses.eTrickModeCtrl  = eDxCopyrightControl_SesTrickModeCtrl_NoSkip_Jump_FF;
		pstDrmInfo->info.stNagraDrmInfo_Ses.bSvcBlock		 = TRUE;
		pstDrmInfo->info.stNagraDrmInfo_Ses.bDigiCpProtect  = TRUE;
		pstDrmInfo->info.stNagraDrmInfo_Ses.bAnaCpProtect	 = TRUE;
		pstDrmInfo->info.stNagraDrmInfo_Ses.eStorageRight	 = eDxCopyrightControl_SesStorageRight_NoRecording;
		pstDrmInfo->info.stNagraDrmInfo_Ses.bStorageEncrypt = TRUE;
		pstDrmInfo->info.stNagraDrmInfo_Ses.eCompVidCtrl	 = eDxCopyrightControl_SesComponentCtrl_DisableOutput;
		pstDrmInfo->info.stNagraDrmInfo_Ses.bTransCtrl 	 = TRUE;
		pstDrmInfo->info.stNagraDrmInfo_Ses.bBlucomCtrl	 = TRUE;
		break;

	case eDxCopyrightControl_RestrictLevel_None:
	default:
	// 모든 것을 다 허용한다.
	// Normal도 None으로 처리하도록 한다.
		pstDrmInfo->eDrmType = eDrmModelType_ApsHd;
		pstDrmInfo->info.stNagraDrmInfo_Ses.eTrickModeCtrl  = eDxCopyrightControl_SesTrickModeCtrl_NoRestriction;
		pstDrmInfo->info.stNagraDrmInfo_Ses.bSvcBlock		 = FALSE;
		pstDrmInfo->info.stNagraDrmInfo_Ses.bDigiCpProtect  = FALSE;
		pstDrmInfo->info.stNagraDrmInfo_Ses.bAnaCpProtect	 = FALSE;
		pstDrmInfo->info.stNagraDrmInfo_Ses.eStorageRight	 = eDxCopyrightControl_SesStorageRight_NoRestriction;
		pstDrmInfo->info.stNagraDrmInfo_Ses.bStorageEncrypt = FALSE;
		pstDrmInfo->info.stNagraDrmInfo_Ses.eCompVidCtrl	 = eDxCopyrightControl_SesComponentCtrl_NoRestriction;
		pstDrmInfo->info.stNagraDrmInfo_Ses.bTransCtrl 	 = FALSE;
		pstDrmInfo->info.stNagraDrmInfo_Ses.bBlucomCtrl	 = FALSE;
		break;
	}


	return ERR_OK;
}


STATIC HERROR mgr_copyrightcontrol_InitCcInfo_Nordig(DxCopyrightControl_t *pstDrmInfo, DxCopyrightControl_RestrictLevel_e eDrmLevel)
{
	if (pstDrmInfo == NULL) 				{ return ERR_FAIL; }

	pstDrmInfo->eDrmType = eDxCopyrightControl_SiCcType_Nordig;

	return ERR_OK;
}

//
// 향이 추가될 때마다 위에 추가하도록 한다.
//

#endif

/*******************************************************************/
/********************      API Functions   *************************/
/*******************************************************************/

//
// 향이 추가될 때마다 위에 추가하도록 한다.
//
#if 0
HERROR MGR_COPYRIGHTCONTROL_ConvertCcInfoToCcMsgArg (DxCopyrightControl_t stDrmInfo, HUINT32 *p1, HUINT32 *p2, HUINT32 *p3)
{

	if (p1 == NULL || p2 == NULL || p3 == NULL)
	{
		return ERR_FAIL;
	}

	switch (stDrmInfo.eDrmType)
	{
		case eDrmModelType_ApsHd:
			return mgr_copyrightcontrol_ConvertCcInfoToCcMsgArg_Aps (stDrmInfo.info.stNagraDrmInfo_Ses, p1, p2, p3);

		case eDxCopyrightControl_CasCcType_CiPlus:
			return mgr_copyrightcontrol_ConvertCcInfoToCcMsgArg_Ciplus(stDrmInfo.info.stCiPlusDrmInfo, p1, p2, p3);

		case eDxCopyrightControl_SiCcType_Nordig:
			return mgr_copyrightcontrol_ConvertCcInfoToCcMsgArg_Nordig(stDrmInfo.info.stNordigDrmInfo, p1, p2, p3);

		case eDxCopyrightControl_CasCcType_Viaccess:
			return mgr_copyrightcontrol_ConvertCcInfoToCcMsgArg_Viaccess(stDrmInfo.info.stViaccessDrmInfo, p1, p2, p3);

		default:
			break;
	}

	return ERR_FAIL;

}


HERROR MGR_COPYRIGHTCONTROL_ConvertCcMsgArgToCcInfo (HUINT32 p1, HUINT32 p2, HUINT32 p3, DxCopyrightControl_t *pstDrmInfo)
{

	if (pstDrmInfo == NULL)
	{
		return ERR_FAIL;
	}

	switch (p1)
	{
		case eDrmModelType_ApsHd:
			pstDrmInfo->eDrmType = eDrmModelType_ApsHd;
			return mgr_copyrightcontrol_ConvertCcMsgArgToCcInfo_Aps (p1, p2, p3, &(pstDrmInfo->info.stNagraDrmInfo_Ses));

		case eDxCopyrightControl_CasCcType_CiPlus:
			pstDrmInfo->eDrmType = eDxCopyrightControl_CasCcType_CiPlus;
			return mgr_copyrightcontrol_ConvertUriMsgArgToUriInfo_Ciplus (p1, p2, p3, &(pstDrmInfo->info.stCiPlusDrmInfo));

		case eDxCopyrightControl_SiCcType_Nordig:
			pstDrmInfo->eDrmType = eDxCopyrightControl_SiCcType_Nordig;
			return mgr_copyrightcontrol_ConvertCcMsgArgToCcInfo_Nordig (p1, p2, p3, &(pstDrmInfo->info.stNordigDrmInfo));

 		case eDxCopyrightControl_CasCcType_Viaccess:
			pstDrmInfo->eDrmType = eDxCopyrightControl_CasCcType_Viaccess;
			return mgr_copyrightcontrol_ConvertCcMsgArgToCcInfo_Viaccess (p1, p2, p3, &(pstDrmInfo->info.stViaccessDrmInfo));

	       	default:
				break;
	}

	return ERR_FAIL;
}


HERROR MGR_COPYRIGHTCONTROL_InitCcInfo (DxCopyrightControl_t *pstDrmInfo, DrmModelType_t eDrmType, DxCopyrightControl_RestrictLevel_e eDrmLevel)
{

	if (pstDrmInfo == NULL)					{ return ERR_FAIL; }

	switch (eDrmType)
	{
	case eDrmModelType_ApsHd:
		return mgr_copyrightcontrol_InitCcInfo_Aps (pstDrmInfo, eDrmLevel);
	case eDxCopyrightControl_SiCcType_Nordig:
		return mgr_copyrightcontrol_InitCcInfo_Nordig(pstDrmInfo, eDrmLevel);
	default:
		break;
	}

	return ERR_FAIL;
}

HBOOL	MGR_COPYRIGHTCONTROL_CheckIsCcValid(DxCopyrightControl_t *pDrmInfo)
{

	if(pDrmInfo == NULL)
	{
		return FALSE;
	}

	if((pDrmInfo->eDrmType == eDrmModelType_None)
		|| (pDrmInfo->eDrmType >= eDrmModelType_Max))
	{
		return FALSE;
	}

	return FALSE;
}
#endif


HERROR MGR_COPYRIGHTCONTROL_GetCopyProtectInfo(HBOOL bPlayback, const DxCopyrightControl_t *pstDrmInfo, MgrCopyrightControl_OutputControlInfo_t *pstCpInfo)
{
	HERROR					hErr;
	MgrCopyrightControl_OutputControlInfo_t	stSiCpInfo, stCasCpInfo;
	MgrCopyrightControl_Priority_e				ePriority;

	HxLOG_Debug("%s(bPlayback:%d, bDrmCc: %d, bCasCc: %d, bSiCc: %d)\n", __FUNCTION__, bPlayback, pstDrmInfo->bDrmCc, pstDrmInfo->bCasCc, pstDrmInfo->bSiCc);

	if(pstCpInfo == NULL)
	{
		HxLOG_Critical("\t%s() has NULL *pstCpInfo pointer\n",__FUNCTION__);
		return ERR_FAIL;
	}

	HxSTD_memset(&stSiCpInfo, 0, sizeof(MgrCopyrightControl_OutputControlInfo_t));
	HxSTD_memset(&stCasCpInfo, 0, sizeof(MgrCopyrightControl_OutputControlInfo_t));
	hErr = ERR_FAIL;


	ePriority = pmgr_copyrightcontrol_GetPriority();


	// TODO: pstDrmInfo->stDrmCcInfo 에 대한 처리.

	if(pstDrmInfo->bCasCc == TRUE)
	{
		hErr = pmgr_copyrightcontrol_GetCasCopyProtectInfo(bPlayback, pstDrmInfo, &stCasCpInfo);
		if(ePriority == eCopyrightControl_Priority_Cas && hErr == ERR_OK)
		{
			*pstCpInfo = stCasCpInfo;
			return ERR_OK;
		}
	}

	if(pstDrmInfo->bSiCc == TRUE)
	{
		hErr = pmgr_copyrightcontrol_GetSiCopyProtectInfo(bPlayback, pstDrmInfo, &stSiCpInfo);
		if(ePriority == eCopyrightControl_Priority_Si && hErr == ERR_OK)
		{
			*pstCpInfo = stSiCpInfo;
			return ERR_OK;
		}
	}


/* SI를 먼저 반영하고 */
	*pstCpInfo = stSiCpInfo;

/* CAS DRM이 더 강한 항목을 덮어 쓴다. */
	if( (stCasCpInfo.bSetMacrovision == TRUE)
		&& (stCasCpInfo.eMacrovision > pstCpInfo->eMacrovision)
	)
	{
		HxLOG_Debug("\tCAS override MACTOVISION(%s)\n", OTL_ENUM2STR_Macrovision(stCasCpInfo.eMacrovision));
		pstCpInfo->bSetMacrovision = TRUE;
		pstCpInfo->eMacrovision = stCasCpInfo.eMacrovision;
	}

	if( (stCasCpInfo.bSetHdcp == TRUE)
		&& (stCasCpInfo.bHdcp == TRUE)
	)
	{
		HxLOG_Debug("\tCAS override HDCP\n");
		pstCpInfo->bSetHdcp = TRUE;
		pstCpInfo->bHdcp = TRUE;
	}

	if( (stCasCpInfo.bSetScmsCopyRight == TRUE)
		&& (stCasCpInfo.eScmsCopyRight > pstCpInfo->eScmsCopyRight)
	)
	{
		HxLOG_Debug("\tCAS override SCMS(%s)\n", OTL_ENUM2STR_CopyRight(stCasCpInfo.eScmsCopyRight));
		pstCpInfo->bSetScmsCopyRight = TRUE;
		pstCpInfo->eScmsCopyRight = stCasCpInfo.eScmsCopyRight;
	}

	if( (stCasCpInfo.bSetCgmsCopyRight == TRUE)
		&& (stCasCpInfo.eCgmsCopyRight > pstCpInfo->eCgmsCopyRight)
	)
	{
		HxLOG_Debug("\tCAS override CGMS(%s)\n", OTL_ENUM2STR_CopyRight(stCasCpInfo.eCgmsCopyRight));
		pstCpInfo->bSetCgmsCopyRight = TRUE;
		pstCpInfo->eCgmsCopyRight = stCasCpInfo.eCgmsCopyRight;
	}

	if( (stCasCpInfo.bSetCgmsMacrovision == TRUE)
		&& (stCasCpInfo.eCgmsMacrovision > pstCpInfo->eCgmsMacrovision)
	)
	{
		HxLOG_Debug("\tCAS override CGMS MACTOVISION(%s)\n", OTL_ENUM2STR_Macrovision(stCasCpInfo.eCgmsMacrovision));
		pstCpInfo->bSetCgmsMacrovision = TRUE;
		pstCpInfo->eCgmsMacrovision = stCasCpInfo.eCgmsMacrovision;
	}

	if(stCasCpInfo.bOutputMute == TRUE)
	{
		pstCpInfo->bOutputMute = TRUE;
	}

	if(stCasCpInfo.bDigitalOnlyConstraint == TRUE)
	{
		HxLOG_Info("Digital Only Constraint is now enabled...\n");
		pstCpInfo->bDigitalOnlyConstraint = TRUE;
	}

	return ERR_OK;
}


HERROR MGR_COPYRIGHTCONTROL_GetMediaControlInfo(HBOOL bPlayback, const DxCopyrightControl_t *pstDrmInfo, MgrCopyrightControl_MediaControlInfo_t *pstCpInfo)
{
	HERROR								hErr = ERR_FAIL;
	MgrCopyrightControl_MediaControlInfo_t		stSiCpInfo;
	MgrCopyrightControl_MediaControlInfo_t		stCasCpInfo;
	MgrCopyrightControl_Priority_e				ePriority = eCopyrightControl_Priority_Component;
	HUINT32								eTrickModeCtrlSi = 0, eTrickModeCtrlCas = 0;

	HxLOG_Debug("%s(bPlayback:%d, bDrmCc: %d, bCasCc: %d, bSiCc: %d)\n", __FUNCTION__, bPlayback, pstDrmInfo->bDrmCc, pstDrmInfo->bCasCc, pstDrmInfo->bSiCc);

	if(pstCpInfo == NULL)
	{
		HxLOG_Critical("\t%s() has NULL *pstCpInfo pointer\n",__FUNCTION__);
		return ERR_FAIL;
	}

	HxSTD_memset(&stSiCpInfo, 0, sizeof(MgrCopyrightControl_MediaControlInfo_t));
	HxSTD_memset(&stCasCpInfo, 0, sizeof(MgrCopyrightControl_MediaControlInfo_t));

	ePriority = pmgr_copyrightcontrol_GetPriority();

	// TODO: pstDrmInfo->stDrmCcInfo, bSiCc 에 대한 처리.

	if(pstDrmInfo->bCasCc == TRUE)
	{
		hErr = pmgr_copyrightcontrol_GetCasMediaControlInfo(pstDrmInfo->stCasCcInfo.eCasCcType, pstDrmInfo, &stCasCpInfo);
		if(ePriority == eCopyrightControl_Priority_Cas && hErr == ERR_OK)
		{
			*pstCpInfo = stCasCpInfo;
			return ERR_OK;
		}

	}

	if(pstDrmInfo->bSiCc == TRUE)
	{
		hErr = pmgr_copyrightcontrol_GetSiMediaControlInfo(bPlayback, pstDrmInfo, &stSiCpInfo);
		if(ePriority == eCopyrightControl_Priority_Si && hErr == ERR_OK)
		{
			*pstCpInfo = stSiCpInfo;
			return ERR_OK;
		}
	}


	// eCopyrightControl_Priority_Component 일 경우.
	//혹은 eCopyrightControl_Priority_Cas/eCopyrightControl_Priority_Si 이지만 각 data 가 없는 경우는 혼합.

/* SI를 먼저 반영하고 */
	*pstCpInfo = stSiCpInfo;

/* CAS DRM이 더 강한 항목을 덮어 쓴다. */
	HxSTD_MemCopy( &(eTrickModeCtrlSi), &(pstCpInfo->eTrickModeCtrl), sizeof(DxCopyrightControl_TrickRestrictMode_u));
	HxSTD_MemCopy( &(eTrickModeCtrlCas), &(stCasCpInfo.eTrickModeCtrl), sizeof(DxCopyrightControl_TrickRestrictMode_u));
	if( eTrickModeCtrlCas > eTrickModeCtrlSi)
	{
		HxLOG_Debug("\tCAS override eTrickModeCtrl(%d)\n", stCasCpInfo.eTrickModeCtrl);
		HxSTD_MemCopy( &(pstCpInfo->eTrickModeCtrl), &(stCasCpInfo.eTrickModeCtrl), sizeof(DxCopyrightControl_TrickRestrictMode_u));
	}

	if( stCasCpInfo.bSvcBlock == TRUE)
	{
		HxLOG_Debug("\tCAS override bSvcBlock\n");
		pstCpInfo->bSvcBlock = TRUE;
	}

	if( stCasCpInfo.bDigiCpProtect == TRUE)
	{
		HxLOG_Debug("\tCAS override bDigiCpProtect\n");
		pstCpInfo->bDigiCpProtect = TRUE;
	}

	if( stCasCpInfo.bAnaCpProtect == TRUE)
	{
		HxLOG_Debug("\tCAS override bAnaCpProtect\n");
		pstCpInfo->bAnaCpProtect = TRUE;
	}

	if( stCasCpInfo.eStorageRight > pstCpInfo->eStorageRight)
	{
		HxLOG_Debug("\tCAS override eStorageRight(%s)\n", OTL_ENUM2STR_StorageRight(stCasCpInfo.eStorageRight));
		pstCpInfo->eStorageRight = stCasCpInfo.eStorageRight;
	}


	if( stCasCpInfo.bStorageEncrypt == TRUE)
	{
		HxLOG_Debug("\tCAS override bStorageEncrypt\n");
		pstCpInfo->bStorageEncrypt = TRUE;
	}


	if( stCasCpInfo.eCompVidCtrl > pstCpInfo->eCompVidCtrl)
	{
		HxLOG_Debug("\tCAS override eCompVidCtrl(%s)\n", OTL_ENUM2STR_CompVidCtrl(stCasCpInfo.eCompVidCtrl));
		pstCpInfo->eCompVidCtrl = stCasCpInfo.eCompVidCtrl;
	}

	if( stCasCpInfo.bTransCtrl == TRUE)
	{
		HxLOG_Debug("\tCAS override bTransCtrl\n");
		pstCpInfo->bTransCtrl = TRUE;
	}

	if( stCasCpInfo.bBlucomCtrl == TRUE)
	{
		HxLOG_Debug("\tCAS override bBlucomCtrl\n");
		pstCpInfo->bBlucomCtrl = TRUE;
	}





	return ERR_OK;
}


/********************************************************************/
/********************	Application Proc.	*************************/
/********************************************************************/



#include <octo_common.h>

#include <svc_cas.h>
#include <mgr_copyrightcontrol.h>


STATIC HERROR xmgr_copyrightcontrol_GetDrmInfoFromControlByte_NagraSes (HUINT8 ucCtrlByteA, HUINT8 ucCtrlByteB, MgrCopyrightControl_MediaControlInfo_t *pstDrmInfo)
{
	if (pstDrmInfo == NULL)				{ return ERR_FAIL; }


	// Byte A
	pstDrmInfo->eTrickModeCtrl.eTrickRestrictMode_Ses = (HUINT8)((ucCtrlByteA & 0x38) >> 3);
	pstDrmInfo->bSvcBlock		 = (ucCtrlByteA & 0x04) ? TRUE : FALSE;
	pstDrmInfo->bDigiCpProtect	 = (ucCtrlByteA & 0x02) ? TRUE : FALSE;
	pstDrmInfo->bAnaCpProtect	 = (ucCtrlByteA & 0x01) ? TRUE : FALSE;

	// Byte B
	pstDrmInfo->eStorageRight	 = (HUINT8)((ucCtrlByteB & 0xE0) >> 5);
	pstDrmInfo->bStorageEncrypt = (ucCtrlByteB & 0x10) ? TRUE : FALSE;
	pstDrmInfo->eCompVidCtrl	 = (HUINT8)((ucCtrlByteB & 0x0C) >> 2);
	pstDrmInfo->bTransCtrl		 =  (ucCtrlByteB & 0x02) ? TRUE : FALSE;
	pstDrmInfo->bBlucomCtrl	 =  (ucCtrlByteB & 0x01) ? TRUE : FALSE;

	return ERR_OK;
}

HERROR xmgr_copyrightcontrol_InitCasDrmInfo_NagraSes(DxCopyrightControl_t *pstCasDrmInfo, MgrCopyrightControl_MediaControlInfo_t *pstDrmInfo)
{
	if(pstCasDrmInfo)
	{
		pstCasDrmInfo->stCasCcInfo.info.stNagraDrmInfo_Ses.ucControlByteA = 0x02;	// 현재는 HDCP만 ON으로 함 추후 아래의 기본적용값이 변경되면 수정되어야한다.
		pstCasDrmInfo->stCasCcInfo.info.stNagraDrmInfo_Ses.ucControlByteB = 0x00; 	// 추후 아래의 기본적용값이 변경되면 수정되어야한다.
	}

	if(pstDrmInfo)
	{
		// Byte A
		pstDrmInfo->eTrickModeCtrl.eTrickRestrictMode_Ses = eDxCopyrightControl_SesTrickModeCtrl_NoRestriction;
		pstDrmInfo->bSvcBlock		 	= FALSE;
		pstDrmInfo->bDigiCpProtect	 	= TRUE; 	// hdcp는 항상 on이다.
		pstDrmInfo->bAnaCpProtect	 	= FALSE;

		// Byte B
		pstDrmInfo->eStorageRight	 	= eDxCopyrightControl_SesStorageRight_NoRestriction;
		pstDrmInfo->bStorageEncrypt 	= FALSE;
		pstDrmInfo->eCompVidCtrl	 	= eDxCopyrightControl_SesComponentCtrl_NoRestriction;
		pstDrmInfo->bTransCtrl		 	= FALSE;
		pstDrmInfo->bBlucomCtrl	 		= FALSE;
	}

	return ERR_OK;
}

HERROR xmgr_copyrightcontrol_InitSiDrmInfo_NagraSes(DxCopyrightControl_t *pstSiDrmInfo, MgrCopyrightControl_MediaControlInfo_t *pstDrmInfo)
{
	if(pstSiDrmInfo)
	{
		pstSiDrmInfo->stSiCcInfo.info.stSesDrmInfo.ucControlByteA = 0x02;	// 현재는 HDCP만 ON으로 함 추후 아래의 기본적용값이 변경되면 수정되어야한다.
		pstSiDrmInfo->stSiCcInfo.info.stSesDrmInfo.ucControlByteB = 0x00;	// 추후 아래의 기본적용값이 변경되면 수정되어야한다.
	}

	if(pstDrmInfo)
	{
		// Byte A
		pstDrmInfo->eTrickModeCtrl.eTrickRestrictMode_Ses = eDxCopyrightControl_SesTrickModeCtrl_NoRestriction;
		pstDrmInfo->bSvcBlock		 	= FALSE;
		pstDrmInfo->bDigiCpProtect	 	= TRUE;		// hdcp는 항상 on이다.
		pstDrmInfo->bAnaCpProtect	 	= FALSE;

		// Byte B
		pstDrmInfo->eStorageRight	 	= eDxCopyrightControl_SesStorageRight_NoRestriction;
		pstDrmInfo->bStorageEncrypt 	= FALSE;
		pstDrmInfo->eCompVidCtrl	 	= eDxCopyrightControl_SesComponentCtrl_NoRestriction;
		pstDrmInfo->bTransCtrl		 	= FALSE;
		pstDrmInfo->bBlucomCtrl	 		= FALSE;
	}

	return ERR_OK;
}

HERROR xmgr_copyrightcontrol_GetCasCopyProtectInfo_NagraSes(DxCopyrightControl_t *pstCasDrmInfo, MgrCopyrightControl_OutputControlInfo_t *pstCpInfo)
{
	MgrCopyrightControl_MediaControlInfo_t 			stDetailDrmInfo;
	HERROR					hErr = ERR_FAIL;

	if(pstCasDrmInfo == NULL || pstCpInfo == NULL)
	{
		HxLOG_Error("pstCasDrmInfo o pstCpInfo is NULL\n");
		return ERR_FAIL;
	}

	HxSTD_memset(&stDetailDrmInfo, 0, sizeof(MgrCopyrightControl_MediaControlInfo_t));
	hErr = xmgr_copyrightcontrol_GetDrmInfoFromControlByte_NagraSes(pstCasDrmInfo->stCasCcInfo.info.stNagraDrmInfo_Ses.ucControlByteA,
															pstCasDrmInfo->stCasCcInfo.info.stNagraDrmInfo_Ses.ucControlByteB,
															&stDetailDrmInfo);
	if(hErr == ERR_FAIL)
	{
		HxLOG_Error("xmgr_copyrightcontrol_GetDrmInfoFromControlByte_NagraSes() fail!\n");
		return ERR_FAIL;

	}

	HxSTD_memset(pstCpInfo, 0, sizeof(MgrCopyrightControl_OutputControlInfo_t));

	// HDCP
	pstCpInfo->bSetHdcp = TRUE;
	pstCpInfo->bHdcp = stDetailDrmInfo.bDigiCpProtect;

	// Macrovision
	pstCpInfo->bSetMacrovision = TRUE;
	pstCpInfo->eMacrovision = (stDetailDrmInfo.bAnaCpProtect == TRUE) ? eMACROVISION_AGC : eMACROVISION_OFF;

	// Component : iCordPro ( not exist )
#if 0 // 아래는 icord-mini 소스임, 추후 적용시 참고
	// Component
	pstCopyProtectFlag->bSetComCtrl = TRUE;
	if(pDrmInfo->info.stApsDrmInfo.eCompVidCtrl == eDRM_Comp_Disable_Output)
	{
		pstCopyProtectFlag->eCompCtrl = eComponentCtrl_DisableOutput;
	}
	else if(pDrmInfo->info.stApsDrmInfo.eCompVidCtrl == eDRM_Comp_Down_Scale)
	{
		pstCopyProtectFlag->eCompCtrl = eComponentCtrl_DownScale;
	}
	else
	{
		pstCopyProtectFlag->eCompCtrl = eComponentCtrl_None;
	}
#endif
	return ERR_OK;
}


HERROR xmgr_copyrightcontrol_GetSiCopyProtectInfo_Ses(DxCopyrightControl_t *pstSiDrmInfo, MgrCopyrightControl_OutputControlInfo_t *pstCpInfo)
{
	MgrCopyrightControl_MediaControlInfo_t 			stDetailDrmInfo;
	HERROR					hErr = ERR_FAIL;

	if(pstSiDrmInfo == NULL || pstCpInfo == NULL)
	{
		HxLOG_Error("pstCasDrmInfo o pstCpInfo is NULL\n");
		return ERR_FAIL;
	}

	HxSTD_memset(&stDetailDrmInfo, 0, sizeof(MgrCopyrightControl_MediaControlInfo_t));
	hErr = xmgr_copyrightcontrol_GetDrmInfoFromControlByte_NagraSes(pstSiDrmInfo->stSiCcInfo.info.stSesDrmInfo.ucControlByteA,
															pstSiDrmInfo->stSiCcInfo.info.stSesDrmInfo.ucControlByteB,
															&stDetailDrmInfo);
	if(hErr == ERR_FAIL)
	{
		HxLOG_Error("xmgr_copyrightcontrol_GetDrmInfoFromControlByte_NagraSes() fail!\n");
		return ERR_FAIL;

	}

	HxSTD_memset(pstCpInfo, 0, sizeof(MgrCopyrightControl_OutputControlInfo_t));

	// HDCP
	pstCpInfo->bSetHdcp = TRUE;
	pstCpInfo->bHdcp = stDetailDrmInfo.bDigiCpProtect;

	// Macrovision
	pstCpInfo->bSetMacrovision = TRUE;
	pstCpInfo->eMacrovision = (stDetailDrmInfo.bAnaCpProtect == TRUE) ? eMACROVISION_AGC : eMACROVISION_OFF;

	// Component : iCordPro ( not exist )

	return ERR_OK;

}



HERROR xmgr_copyrightcontrol_GetCasMediaControlInfo_NagraSes(DxCopyrightControl_t *pstCasDrmInfo, MgrCopyrightControl_MediaControlInfo_t *pstCpInfo)
{

	HERROR					hErr = ERR_FAIL;

	if(pstCasDrmInfo == NULL || pstCpInfo == NULL)
	{
		HxLOG_Error("pstCasDrmInfo o pstCpInfo is NULL\n");
		return ERR_FAIL;
	}

	HxSTD_memset(pstCpInfo, 0, sizeof(MgrCopyrightControl_MediaControlInfo_t));

	hErr = xmgr_copyrightcontrol_GetDrmInfoFromControlByte_NagraSes(pstCasDrmInfo->stCasCcInfo.info.stNagraDrmInfo_Ses.ucControlByteA,
															pstCasDrmInfo->stCasCcInfo.info.stNagraDrmInfo_Ses.ucControlByteB,
															pstCpInfo);

#if defined(CONFIG_MW_CAS_NAGRA_OP_SES_CAKTEST)
#if defined(CONFIG_DEBUG)
	// cak7 - CAK-MER-MetadataCopyCtrl test 시 보여주는 코드
	HxLOG_Print(">>> copy control is activated \n");
	HxLOG_Print("[METADATA] eTrickModeCtrl(%x)SvcBlock(%x) DigiCpProtect(%x)AnaCpProtect(%x) eStorageRight(%x) bStorageEncrypt(%x) eCompVidCtrl(%x)bTransCtrl(%x) bBlucomCtrl(%x)\n\n",
		pstCpInfo->eTrickModeCtrl.eTrickRestrictMode_Ses, pstCpInfo->bSvcBlock, pstCpInfo->bDigiCpProtect, pstCpInfo->bAnaCpProtect,
		pstCpInfo->eStorageRight, pstCpInfo->bStorageEncrypt, pstCpInfo->eCompVidCtrl, pstCpInfo->bTransCtrl,
		pstCpInfo->bBlucomCtrl);
#else
{
	HUINT8 ucTmp[256];
	HxSTD_memset(ucTmp, 0x00, 256);
	snprintf(ucTmp, 256, "[METADATA] eTrickModeCtrl(%x)SvcBlock(%x) DigiCpProtect(%x)AnaCpProtect(%x) eStorageRight(%x) bStorageEncrypt(%x) eCompVidCtrl(%x)bTransCtrl(%x) bBlucomCtrl(%x)\n\n",
		pstCpInfo->eTrickModeCtrl.eTrickRestrictMode_Ses, pstCpInfo->bSvcBlock, pstCpInfo->bDigiCpProtect, pstCpInfo->bAnaCpProtect,
		pstCpInfo->eStorageRight, pstCpInfo->bStorageEncrypt, pstCpInfo->eCompVidCtrl, pstCpInfo->bTransCtrl,
		pstCpInfo->bBlucomCtrl);

	// cak7 - CAK-MER-MetadataCopyCtrl test 시 보여주는 코드
	MW_CA_NA_PrintForRelease(">>> copy control is activated \n");
	MW_CA_NA_PrintForRelease(ucTmp);
}
#endif
#endif
	return hErr;

}

HERROR xmgr_copyrightcontrol_GetSiMediaControlInfo_NagraSes(DxCopyrightControl_t *pstSiDrmInfo, MgrCopyrightControl_MediaControlInfo_t *pstCpInfo)
{

	HERROR					hErr = ERR_FAIL;

	if(pstSiDrmInfo == NULL || pstCpInfo == NULL)
	{
		HxLOG_Error("pstSiDrmInfo or pstCpInfo is NULL\n");
		return ERR_FAIL;
	}

	HxSTD_memset(pstCpInfo, 0, sizeof(MgrCopyrightControl_MediaControlInfo_t));

	hErr = xmgr_copyrightcontrol_GetDrmInfoFromControlByte_NagraSes(pstSiDrmInfo->stSiCcInfo.info.stSesDrmInfo.ucControlByteA,
															pstSiDrmInfo->stSiCcInfo.info.stSesDrmInfo.ucControlByteB,
															pstCpInfo);
	return hErr;

}


#include <octo_common.h>

#include <svc_cas.h>
#include <mgr_copyrightcontrol.h>

HERROR xmgr_copyrightcontrol_GetCopyProtectInfo_Conax(HBOOL bPlayback, DxCopyrightControl_t *pstCasDrmInfo, MgrCopyrightControl_OutputControlInfo_t *pstCpInfo)
{
#if defined(CONFIG_MW_CAS_CONAX) && defined(CONFIG_MW_CAS_CONAX_URI)
	DxCopyrightControl_ConaxDrmInfo_t		*pstConaxDrmInfo;

	pstConaxDrmInfo = &pstCasDrmInfo->stCasCcInfo.info.stConaxDrmInfo;

	// 6.3.3.2	emi_copy_control
	// pstConaxDrmInfo->eEmiCopyCtrl�� eDxCopyrightControl_ConaxEmiCopyCtrl_Freely�̰�
	// pstConaxDrmInfo->eRedistributionCtrl�� eDxCopyrightControl_ConaxRedistributionCtrl_Off�̸� HDCP�� off �Ҽ��� ������
	// ������ HDCP on���� �����ϵ��� �Ѵ�.
	pstCpInfo->bSetHdcp = TRUE;
	pstCpInfo->bHdcp = TRUE;

	pstCpInfo->bSetScmsCopyRight = TRUE;
	pstCpInfo->bSetCgmsCopyRight = TRUE;
	pstCpInfo->bSetMacrovision = TRUE;

	// EMI & APS : Set Macrovision and SCMS
	switch(pstConaxDrmInfo->eApsCopyCtrl)
	{
	case eDxCopyrightControl_ConaxApsCopyCtrl_Off:
		pstCpInfo->eMacrovision = eMACROVISION_OFF;
		break;

	case eDxCopyrightControl_ConaxApsCopyCtrl_BurstOff:
		pstCpInfo->eMacrovision = eMACROVISION_AGC;
		break;

	case eDxCopyrightControl_ConaxApsCopyCtrl_Burst2Line:
		pstCpInfo->eMacrovision = eMACROVISION_2LINE;
		break;

	case eDxCopyrightControl_ConaxApsCopyCtrl_Burst4Line:
		pstCpInfo->eMacrovision = eMACROVISION_4LINE;
		break;

	default:
		pstCpInfo->eMacrovision = eMACROVISION_OFF;
		break;
	}

	switch (pstConaxDrmInfo->eEmiCopyCtrl)
	{
	case eDxCopyrightControl_ConaxEmiCopyCtrl_Freely :
		pstCpInfo->eMacrovision = eMACROVISION_OFF;
		pstCpInfo->eScmsCopyRight = eCOPYRIGHT_COPY_FREE;
		pstCpInfo->eCgmsCopyRight = eCOPYRIGHT_COPY_FREE;
		break;

	case eDxCopyrightControl_ConaxEmiCopyCtrl_NoMore :
		pstCpInfo->eScmsCopyRight = eCOPYRIGHT_COPY_NOMORE;
		pstCpInfo->eCgmsCopyRight = eCOPYRIGHT_COPY_NOMORE;
		break;

	case eDxCopyrightControl_ConaxEmiCopyCtrl_Once :
		if(bPlayback == TRUE)
		{
			pstCpInfo->eScmsCopyRight = eCOPYRIGHT_COPY_NOMORE;
			pstCpInfo->eCgmsCopyRight = eCOPYRIGHT_COPY_NOMORE;
		}
		else
		{
			pstCpInfo->eScmsCopyRight = eCOPYRIGHT_COPY_ONCE;
			pstCpInfo->eCgmsCopyRight = eCOPYRIGHT_COPY_ONCE;
		}
		break;

	case eDxCopyrightControl_ConaxEmiCopyCtrl_Never :
		pstCpInfo->eScmsCopyRight = eCOPYRIGHT_COPY_NEVER;
		pstCpInfo->eCgmsCopyRight = eCOPYRIGHT_COPY_NEVER;
		break;

	default:
		pstCpInfo->bSetScmsCopyRight = FALSE;
		pstCpInfo->bSetCgmsCopyRight = FALSE;
		pstCpInfo->bSetMacrovision = FALSE;
		HxLOG_Error("Invalid EMI bits 0x%02x !!\r\n", pstConaxDrmInfo->eEmiCopyCtrl);
		break;
	}

	// 6.3.3.3	image_constraint
	// pstConaxDrmInfo->eICT�� eDxCopyrightControl_ConaxICT_On�̸� Analog component video output�� 520K pixels ���Ϸ� ��µǾ�� �Ѵ�.
	// Analog component video output �������� �ʱ� ������ �������� �ʴ´�.

	// 6.3.3.8	disable_analogue_outputs
	if (pstConaxDrmInfo->eDisableAnalOutput == eCxDisableAnalOutput_Off)
	{
		pstCpInfo->bDigitalOnlyConstraint = FALSE;
	}
	else
	{
		pstCpInfo->bDigitalOnlyConstraint = TRUE;
	}
	return ERR_OK;
#else
	return ERR_FAIL;
#endif
}

HERROR xmgr_copyrightcontrol_GetCasMediaControlInfo_Conax(DxCopyrightControl_t *pstCasDrmInfo, MgrCopyrightControl_MediaControlInfo_t *pstCpInfo)
{
#if defined(CONFIG_MW_CAS_CONAX) && defined(CONFIG_MW_CAS_CONAX_URI)
	DxCopyrightControl_ConaxDrmInfo_t		*pstConaxDrmInfo;

	if(pstCasDrmInfo == NULL || pstCpInfo == NULL)
	{
		HxLOG_Error("pstCasDrmInfo o pstCpInfo is NULL\n");
		return ERR_FAIL;
	}

	HxSTD_memset(pstCpInfo, 0, sizeof(MgrCopyrightControl_MediaControlInfo_t));
	pstConaxDrmInfo = &pstCasDrmInfo->stCasCcInfo.info.stConaxDrmInfo;

#if 0
typedef struct
{
	DxCopyrightControl_TrickRestrictMode_u				eTrickModeCtrl;
	HBOOL		 				bSvcBlock;
	HBOOL		 				bDigiCpProtect, bAnaCpProtect;

	// TODO: ���� ���·� �ٲ���. union ����
	eDxCopyrightControl_SesStorageRight_e				eStorageRight;
	HBOOL		 				bStorageEncrypt;
	// TODO: ���� ���·� �ٲ���. union ����
	eDxCopyrightControl_SesComponentCtrl_e				eCompVidCtrl;
	HBOOL		 				bTransCtrl;
	HBOOL		 				bBlucomCtrl;

} MgrCopyrightControl_MediaControlInfo_t;
#endif


	pstCpInfo->eTrickModeCtrl.eTrickRestrictMode_Conax = pstConaxDrmInfo->eTrickPlayCtrl;
	pstCpInfo->bSvcBlock = FALSE;
	// 6.3.3.2	emi_copy_control
	// pstConaxDrmInfo->eEmiCopyCtrl�� eDxCopyrightControl_ConaxEmiCopyCtrl_Freely�̰�
	// pstConaxDrmInfo->eRedistributionCtrl�� eDxCopyrightControl_ConaxRedistributionCtrl_Off�̸� HDCP�� off �Ҽ��� ������
	// ������ HDCP on���� �����ϵ��� �Ѵ�.
	pstCpInfo->bDigiCpProtect = TRUE;

	switch(pstConaxDrmInfo->eApsCopyCtrl)
	{
	case eDxCopyrightControl_ConaxApsCopyCtrl_BurstOff:
	case eDxCopyrightControl_ConaxApsCopyCtrl_Burst2Line:
	case eDxCopyrightControl_ConaxApsCopyCtrl_Burst4Line:
		pstCpInfo->bAnaCpProtect = TRUE;
		break;
	case eDxCopyrightControl_ConaxApsCopyCtrl_Off:
	default:
		pstCpInfo->bAnaCpProtect = FALSE;
		break;
	}

	return ERR_OK;
#else
	return ERR_FAIL;
#endif
}



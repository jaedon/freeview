#include <octo_common.h>

#include <svc_cas.h>
#include <mgr_copyrightcontrol.h>

HERROR xmgr_copyrightcontrol_GetCasCopyProtectInfo_VmxIptv(DxCopyrightControl_t *pstCasDrmInfo, MgrCopyrightControl_OutputControlInfo_t *pstCpInfo)
{

	if(pstCasDrmInfo == NULL || pstCpInfo == NULL)
	{
		HxLOG_Error("pstCasDrmInfo o pstCpInfo is NULL\n");
		return ERR_FAIL;
	}

	HxSTD_memset(pstCpInfo, 0, sizeof(MgrCopyrightControl_OutputControlInfo_t));

	// HDCP
	pstCpInfo->bSetHdcp = TRUE;
	pstCpInfo->bHdcp = pstCasDrmInfo->stCasCcInfo.info.stVmxDrmInfo.bHdcp;

	pstCpInfo->bSetScmsCopyRight = TRUE;
	pstCpInfo->bSetCgmsCopyRight = TRUE;
	pstCpInfo->bSetMacrovision = TRUE;


	switch (pstCasDrmInfo->stCasCcInfo.info.stVmxDrmInfo.eEmiCopyCtrl)
	{
		case eDxCopyrightControl_VerimatrixxEmiCopyCtrl_Freely :
			pstCpInfo->eMacrovision = eMACROVISION_OFF;
			pstCpInfo->eScmsCopyRight = eCOPYRIGHT_COPY_FREE;
			pstCpInfo->eCgmsCopyRight = eCOPYRIGHT_COPY_FREE;
			break;

		case eDxCopyrightControl_VerimatrixEmiCopyCtrl_NoMore :
			switch(pstCasDrmInfo->stCasCcInfo.info.stVmxDrmInfo.eApsCopyCtrl)
			{
				case eDxCopyrightControl_VerimatrixApsCopyCtrl_Off:
					pstCpInfo->eMacrovision = eMACROVISION_OFF;
					break;

				case eDxCopyrightControl_VerimatrixApsCopyCtrl_BurstOff:
					pstCpInfo->eMacrovision = eMACROVISION_AGC;
					break;

				case eDxCopyrightControl_VerimatrixApsCopyCtrl_Burst2Line:
					pstCpInfo->eMacrovision = eMACROVISION_2LINE;
					break;

				case eDxCopyrightControl_VerimatrixApsCopyCtrl_Burst4Line:
					pstCpInfo->eMacrovision = eMACROVISION_4LINE;
					break;

				default:
					pstCpInfo->eMacrovision = eMACROVISION_OFF;
					break;
			}

			pstCpInfo->eScmsCopyRight = eCOPYRIGHT_COPY_NOMORE;
			pstCpInfo->eCgmsCopyRight = eCOPYRIGHT_COPY_NOMORE;
			break;

		case eDxCopyrightControl_VerimatrixEmiCopyCtrl_Once :
			pstCpInfo->eMacrovision = eMACROVISION_OFF;
			pstCpInfo->eScmsCopyRight = eCOPYRIGHT_COPY_ONCE;
			pstCpInfo->eCgmsCopyRight = eCOPYRIGHT_COPY_ONCE;
			break;

		case eDxCopyrightControl_VerimatrixEmiCopyCtrl_Never :
			switch(pstCasDrmInfo->stCasCcInfo.info.stVmxDrmInfo.eApsCopyCtrl)
			{
				case eDxCopyrightControl_VerimatrixApsCopyCtrl_Off:
					pstCpInfo->eMacrovision = eMACROVISION_OFF;
					break;

				case eDxCopyrightControl_VerimatrixApsCopyCtrl_BurstOff:
					pstCpInfo->eMacrovision = eMACROVISION_AGC;
					break;

				case eDxCopyrightControl_VerimatrixApsCopyCtrl_Burst2Line:
					pstCpInfo->eMacrovision = eMACROVISION_2LINE;
					break;

				case eDxCopyrightControl_VerimatrixApsCopyCtrl_Burst4Line:
					pstCpInfo->eMacrovision = eMACROVISION_4LINE;
					break;

				default:
					pstCpInfo->eMacrovision = eMACROVISION_OFF;
					break;
			}

			pstCpInfo->eScmsCopyRight = eCOPYRIGHT_COPY_NEVER;
			pstCpInfo->eCgmsCopyRight = eCOPYRIGHT_COPY_NEVER;
			break;

		default:
			pstCpInfo->bSetScmsCopyRight = FALSE;
			pstCpInfo->bSetCgmsCopyRight = FALSE;
			pstCpInfo->bSetMacrovision = FALSE;
			HxLOG_Error("Invalid ucEmi (0x%x) !!\n", pstCasDrmInfo->stCasCcInfo.info.stVmxDrmInfo.eEmiCopyCtrl);
			break;
	}

	HxLOG_Print("bHdcp(%d), eMacrovision(0x%x), eCgmsCopyRight(0x%x)\n",pstCpInfo->bHdcp, pstCpInfo->eMacrovision, pstCpInfo->eCgmsCopyRight);

	return ERR_OK;
}

HERROR xmgr_copyrightcontrol_GetCasMediaControlInfo_Verimatrix(DxCopyrightControl_t *pstCasDrmInfo, MgrCopyrightControl_MediaControlInfo_t *pstCpInfo)
{
	DxCopyrightControl_Verimatrix_t			*pstVmxDrmInfo;

	if(pstCasDrmInfo == NULL || pstCpInfo == NULL)
	{
		HxLOG_Error("pstCasDrmInfo o pstCpInfo is NULL\n");
		return ERR_FAIL;
	}

	if ((pstCasDrmInfo->bCasCc != TRUE) || (pstCasDrmInfo->stCasCcInfo.eCasCcType != eDxCopyrightControl_CasCcType_Verimatrix))
	{
		HxLOG_Print("Cas CopyControl disabled or not VMX CC\n");
		return ERR_OK;
	}

	HxSTD_MemSet(pstCpInfo, 0, sizeof(MgrCopyrightControl_MediaControlInfo_t));
	pstVmxDrmInfo = &pstCasDrmInfo->stCasCcInfo.info.stVmxDrmInfo;

	// 별다른 요청이 없다면 hdcp는 항상 enable시키자
	pstCpInfo->bDigiCpProtect = TRUE;

	// analogue는 aps를 보자
	switch(pstVmxDrmInfo->eApsCopyCtrl)
	{
		case eDxCopyrightControl_VerimatrixApsCopyCtrl_BurstOff:
		case eDxCopyrightControl_VerimatrixApsCopyCtrl_Burst2Line:
		case eDxCopyrightControl_VerimatrixApsCopyCtrl_Burst4Line:
			pstCpInfo->bAnaCpProtect = TRUE;
			break;

		case eDxCopyrightControl_VerimatrixApsCopyCtrl_Off:
		default:
			pstCpInfo->bAnaCpProtect = FALSE;
			break;
	}

	return ERR_OK;
}

/* end of file */

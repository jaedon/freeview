#include <octo_common.h>

#include <svc_cas.h>
#include <mgr_copyrightcontrol.h>

HERROR xmgr_copyrightcontrol_GetCopyProtectInfo_CiPlus(HBOOL bPlayback, DxCopyrightControl_t *pstCasDrmInfo, MgrCopyrightControl_OutputControlInfo_t *pstCpInfo)
{
#if defined(CONFIG_MW_CI_PLUS)
	DxCopyrightControl_CiPlusUriInfo_t		*pstCiPlusUriInfo;

	pstCiPlusUriInfo = &pstCasDrmInfo->stCasCcInfo.info.stCiPlusDrmInfo.ciplusDrmInfo.stCiPlusUriInfo;

	/* ci plus에서는 hdcp on, always */
	pstCpInfo->bSetHdcp = TRUE;
	pstCpInfo->bHdcp = TRUE;

	pstCpInfo->bSetScmsCopyRight = TRUE;
	pstCpInfo->bSetCgmsCopyRight = TRUE;
	pstCpInfo->bSetMacrovision = TRUE;

	// EMI & APS : Set Macrovision and SCMS
	switch (pstCiPlusUriInfo->ucEmi)
	{
		case CI_CC_URI_EMI_COPY_FREE :
			pstCpInfo->eMacrovision = eMACROVISION_OFF;
			pstCpInfo->eScmsCopyRight = eCOPYRIGHT_COPY_FREE;
			pstCpInfo->eCgmsCopyRight = eCOPYRIGHT_COPY_FREE;
			break;

		case CI_CC_URI_EMI_COPY_NO_MORE :
			switch(pstCiPlusUriInfo->ucAps)
			{
				case CI_CC_URI_APS_OFF:
					pstCpInfo->eMacrovision = eMACROVISION_OFF;
					break;

				case CI_CC_URI_APS_ON_1:
					pstCpInfo->eMacrovision = eMACROVISION_AGC;
					break;

				case CI_CC_URI_APS_ON_2:
					pstCpInfo->eMacrovision = eMACROVISION_2LINE;
					break;

				case CI_CC_URI_APS_ON_3:
					pstCpInfo->eMacrovision = eMACROVISION_4LINE;
					break;

				default:
					pstCpInfo->eMacrovision = eMACROVISION_OFF;
					break;
			}

			pstCpInfo->eScmsCopyRight = eCOPYRIGHT_COPY_NOMORE;
			pstCpInfo->eCgmsCopyRight = eCOPYRIGHT_COPY_NOMORE;
			break;

		case CI_CC_URI_EMI_COPY_ONCE :
			switch(pstCiPlusUriInfo->ucAps)
			{
				case CI_CC_URI_APS_OFF:
					pstCpInfo->eMacrovision = eMACROVISION_OFF;
					break;

				case CI_CC_URI_APS_ON_1:
					pstCpInfo->eMacrovision = eMACROVISION_AGC;
					break;

				case CI_CC_URI_APS_ON_2:
					pstCpInfo->eMacrovision = eMACROVISION_2LINE;
					break;

				case CI_CC_URI_APS_ON_3:
					pstCpInfo->eMacrovision = eMACROVISION_4LINE;
					break;

				default:
					pstCpInfo->eMacrovision = eMACROVISION_OFF;
					break;
			}

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

		case CI_CC_URI_EMI_COPY_NEVER :
			switch(pstCiPlusUriInfo->ucAps)
			{
				case CI_CC_URI_APS_OFF:
					pstCpInfo->eMacrovision = eMACROVISION_OFF;
					break;

				case CI_CC_URI_APS_ON_1:
					pstCpInfo->eMacrovision = eMACROVISION_AGC;
					break;

				case CI_CC_URI_APS_ON_2:
					pstCpInfo->eMacrovision = eMACROVISION_2LINE;
					break;

				case CI_CC_URI_APS_ON_3:
					pstCpInfo->eMacrovision = eMACROVISION_4LINE;
					break;

				default:
					pstCpInfo->eMacrovision = eMACROVISION_OFF;
					break;
			}

			pstCpInfo->eScmsCopyRight = eCOPYRIGHT_COPY_NEVER;
			pstCpInfo->eCgmsCopyRight = eCOPYRIGHT_COPY_NEVER;

			if(pstCiPlusUriInfo->ucDot)	// ucDot bit가 올라가 있는 것은  CI_CC_URI_DOT_ON의 의미로, digital 출력만 허용. 즉, analog output을 막겠다는 의미 임.
				pstCpInfo->bDigitalOnlyConstraint = TRUE;
			else
				pstCpInfo->bDigitalOnlyConstraint = FALSE;

			break;

		default:
			pstCpInfo->bSetScmsCopyRight = FALSE;
			pstCpInfo->bSetCgmsCopyRight = FALSE;
			pstCpInfo->bSetMacrovision = FALSE;
			HxLOG_Error("Invalid ucEmi (0x%x) !!\n", pstCiPlusUriInfo->ucEmi);
			break;
	}

	// RCT : Encryption Plus Non-assert :: 'CI_CC_URI_RCT_ON' 경우 암호화된 디지털 복사는 허용, OFF인 경우 어떠한 복사도 허용하지 않음. 본 내용은 Output control과는 관계 없음.
	/*
	if (pstCiPlusUriInfo->ucRct == CI_CC_URI_RCT_ON)
	{
	}
	else
	{
	}
	*/

	// RL : Retention Limit :: 해당 값(분단위)의 기간 사이에서만 해당 컨텐츠의 replay를 허용 함. output control 과는 관계 없음.
	/*
	if (pstCiPlusUriInfo->ulMinutesForRL == 0)
	{
	}
	else
	{
	}
	*/

	return ERR_OK;
#else
	return ERR_FAIL;
#endif
}


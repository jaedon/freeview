#include <octo_common.h>

#include <svc_cas.h>
#include <mgr_copyrightcontrol.h>

HERROR xmgr_copyrightcontrol_GetCopyProtectInfo_CiPlus(HBOOL bPlayback, DxCopyrightControl_t *pstCasDrmInfo, MgrCopyrightControl_OutputControlInfo_t *pstCpInfo)
{
#if defined(CONFIG_MW_CI_PLUS)
	DxCopyrightControl_CiPlusUriInfo_t		*pstCiPlusUriInfo;

	pstCiPlusUriInfo = &pstCasDrmInfo->stCasCcInfo.info.stCiPlusDrmInfo.ciplusDrmInfo.stCiPlusUriInfo;

	/* ci plus������ hdcp on, always */
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

			if(pstCiPlusUriInfo->ucDot)	// ucDot bit�� �ö� �ִ� ����  CI_CC_URI_DOT_ON�� �ǹ̷�, digital ��¸� ���. ��, analog output�� ���ڴٴ� �ǹ� ��.
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

	// RCT : Encryption Plus Non-assert :: 'CI_CC_URI_RCT_ON' ��� ��ȣȭ�� ������ ����� ���, OFF�� ��� ��� ���絵 ������� ����. �� ������ Output control���� ���� ����.
	/*
	if (pstCiPlusUriInfo->ucRct == CI_CC_URI_RCT_ON)
	{
	}
	else
	{
	}
	*/

	// RL : Retention Limit :: �ش� ��(�д���)�� �Ⱓ ���̿����� �ش� �������� replay�� ��� ��. output control ���� ���� ����.
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


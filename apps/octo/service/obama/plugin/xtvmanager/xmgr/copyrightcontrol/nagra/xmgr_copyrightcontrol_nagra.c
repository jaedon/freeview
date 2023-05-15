#include <octo_common.h>

#include <svc_cas.h>
#include <mgr_copyrightcontrol.h>

static HERROR _xmgr_copyrightcontrol_GetCopyProtectInfoByUri(DxCopyrightControl_t *pstCasDrmInfo, MgrCopyrightControl_OutputControlInfo_t *pstCpInfo)
{
	DxCopyrightControl_NagraUri_t *pstUriInfo = NULL;

	pstUriInfo = &pstCasDrmInfo->stCasCcInfo.info.stNagraDrmInfo.info.stUri;

	pstCpInfo->bSetHdcp = TRUE;
	pstCpInfo->bHdcp = TRUE;

	pstCpInfo->bSetScmsCopyRight = TRUE;
	pstCpInfo->bSetCgmsCopyRight = TRUE;
	pstCpInfo->bSetMacrovision = TRUE;

	// EMI & APS : Set Macrovision and SCMS
	switch (pstUriInfo->ucEmi)
	{
		case CI_CC_URI_EMI_COPY_FREE :
			pstCpInfo->eMacrovision = eMACROVISION_OFF;
			pstCpInfo->eScmsCopyRight = eCOPYRIGHT_COPY_FREE;
			pstCpInfo->eCgmsCopyRight = eCOPYRIGHT_COPY_FREE;
			break;

		case CI_CC_URI_EMI_COPY_NO_MORE :
			switch(pstUriInfo->ucAps)
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
			pstCpInfo->eMacrovision = eMACROVISION_OFF;
			pstCpInfo->eScmsCopyRight = eCOPYRIGHT_COPY_ONCE;
			pstCpInfo->eCgmsCopyRight = eCOPYRIGHT_COPY_ONCE;
			break;

		case CI_CC_URI_EMI_COPY_NEVER :
			switch(pstUriInfo->ucAps)
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
			break;

		default:
			pstCpInfo->bSetScmsCopyRight = FALSE;
			pstCpInfo->bSetCgmsCopyRight = FALSE;
			pstCpInfo->bSetMacrovision = FALSE;
			HxLOG_Error("Invalid ucEmi (0x%x) !!\n", pstUriInfo->ucEmi);
			break;
	}

	// RCT : Encryption Plus Non-assert :: 'CI_CC_URI_RCT_ON' ��� ��ȣȭ�� ������ ����� ���, OFF�� ��� ��� ���絵 ������� ����. �� ������ Output control���� ���� ����.
	/*
	if (pstUriInfo->ucRct == CI_CC_URI_RCT_ON)
	{
	}
	else
	{
	}
	*/

	// RL : Retention Limit :: �ش� ��(�д���)�� �Ⱓ ���̿����� �ش� �������� replay�� ��� ��. output control ���� ���� ����.
	/*
	if (pstUriInfo->ulMinutesForRL == 0)
	{
	}
	else
	{
	}
	*/

	return ERR_OK;
}

HERROR xmgr_copyrightcontrol_GetCopyProtectInfo_Nagra(DxCopyrightControl_t *pstCasDrmInfo, MgrCopyrightControl_OutputControlInfo_t *pstCpInfo)
{
	HERROR hError = ERR_FAIL;

	if(pstCasDrmInfo == NULL || pstCpInfo == NULL)
		return ERR_FAIL;

#if defined(CONFIG_MW_CAS_NAGRA)
	switch(pstCasDrmInfo->stCasCcInfo.info.stNagraDrmInfo.eType)
	{
		case eDxCopyrightControl_NagraDrmType_CopyControl:
			break;

		case eDxCopyrightControl_NagraDrmType_Uri:	//  CI+ spec�� �����ϴ�.
			hError = _xmgr_copyrightcontrol_GetCopyProtectInfoByUri(pstCasDrmInfo, pstCpInfo);
			break;

		default:
			break;
	}

	OxCAS_Free(pstCasDrmInfo);
#endif

	return hError;
}


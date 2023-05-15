/**
	@file     	pmgr_copyrightcontrol.c
	@brief

	Description:  			\n
	Module: 					 					\n
	Remarks : 										\n

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
#include "./local_include/_xmgr_copyrightcontrol.h"

/*******************************************************************/
/********************      Global Variables     ********************/
/*******************************************************************/

/*******************************************************************/
/********************      Defines         *************************/
/*******************************************************************/

/*******************************************************************/
/********************      Typedef         *************************/
/*******************************************************************/

/*******************************************************************/
/********************      Static Variables     ********************/
/*******************************************************************/

/*******************************************************************/
/********************      Functions       *************************/
/*******************************************************************/

HERROR pmgr_copyrightcontrol_GetSiCopyProtectInfo(HBOOL bPlayback, const DxCopyrightControl_t *pstSiDrmInfo, MgrCopyrightControl_OutputControlInfo_t *pstCpInfo)
{
	HERROR		hErr;

	if (NULL == pstSiDrmInfo)
	{
		HxLOG_Critical("\n\n");
		return ERR_FAIL;
	}

	if (NULL == pstCpInfo)
	{
		HxLOG_Critical("\n\n");
		return ERR_FAIL;
	}

	HxSTD_memset(pstCpInfo, 0, sizeof(MgrCopyrightControl_OutputControlInfo_t));
	hErr = ERR_FAIL;

	switch(pstSiDrmInfo->stSiCcInfo.eSiCcType)
	{
#if defined(CONFIG_MW_CAS_NAGRA_OP_SES) // DRM_TEST
		case eDxCopyrightControl_SiCcType_Ses:
			hErr = xmgr_copyrightcontrol_GetSiCopyProtectInfo_Ses((DxCopyrightControl_t *)pstSiDrmInfo, pstCpInfo);
			break;
#endif
		default:
			HxLOG_Error("\nError !!! in %s() Unrecognized DRM type [%d]\n",__FUNCTION__, pstSiDrmInfo->stSiCcInfo.eSiCcType);
			hErr = ERR_FAIL;
			break;
	}

	return hErr;
}

HERROR pmgr_copyrightcontrol_GetSiMediaControlInfo(HBOOL bPlayback, const DxCopyrightControl_t *pstSiDrmInfo, MgrCopyrightControl_MediaControlInfo_t *pstCpInfo)
{
	HERROR		hErr;

	if (NULL == pstSiDrmInfo)
	{
		HxLOG_Critical("\n\n");
		return ERR_FAIL;
	}

	if (NULL == pstCpInfo)
	{
		HxLOG_Critical("\n\n");
		return ERR_FAIL;
	}

	HxSTD_memset(pstCpInfo, 0, sizeof(MgrCopyrightControl_MediaControlInfo_t));
	hErr = ERR_FAIL;


	switch(pstSiDrmInfo->stSiCcInfo.eSiCcType)
	{
#if defined(CONFIG_MW_CAS_NAGRA_OP_SES) // DRM_TEST
		case eDxCopyrightControl_SiCcType_Ses:
			hErr = xmgr_copyrightcontrol_GetSiMediaControlInfo_NagraSes((DxCopyrightControl_t *)pstSiDrmInfo, pstCpInfo);
			break;
#endif
		default:
			HxLOG_Error("\nError !!! in %s() Unrecognized DRM type [%d]\n",__FUNCTION__, pstSiDrmInfo->stSiCcInfo.eSiCcType);
			hErr = ERR_FAIL;
			break;
	}

	return hErr;

}

HERROR pmgr_copyrightcontrol_InitCasCopyProtectInfo(DxCopyrightControl_t *pstCasDrmInfo, MgrCopyrightControl_MediaControlInfo_t *pstDrmInfo)
{
	HERROR		hErr = ERR_FAIL;

	if(NULL == pstCasDrmInfo || NULL == pstDrmInfo)
	{
		HxLOG_Critical("\n\n");
		return ERR_FAIL;
	}

	switch(pstCasDrmInfo->stCasCcInfo.eCasCcType)
	{
#if defined(CONFIG_MW_CAS_NAGRA_OP_SES)
		case eDxCopyrightControl_CasCcType_NagraSes:
			hErr = xmgr_copyrightcontrol_InitCasDrmInfo_NagraSes(pstCasDrmInfo, pstDrmInfo);
			break;
#endif
		default:
			HxLOG_Error("\nError !!! in %s() Unrecognized DRM type [%d]\n",__FUNCTION__, pstCasDrmInfo->stCasCcInfo.eCasCcType);
			hErr = ERR_FAIL;
			break;
	}

	return hErr;
}

HERROR pmgr_copyrightcontrol_InitSiCopyProtectInfo(DxCopyrightControl_t *pstSiDrmInfo, MgrCopyrightControl_MediaControlInfo_t *pstDrmInfo)
{
	HERROR		hErr = ERR_FAIL;

	if(NULL == pstSiDrmInfo)
	{
		HxLOG_Critical("\n\n");
		return ERR_FAIL;
	}

	switch(pstSiDrmInfo->stSiCcInfo.eSiCcType)
	{
#if defined(CONFIG_MW_CAS_NAGRA_OP_SES)
		case eDxCopyrightControl_SiCcType_Ses:
			hErr = xmgr_copyrightcontrol_InitSiDrmInfo_NagraSes(pstSiDrmInfo, pstDrmInfo);
			break;
#endif
		default:
			HxLOG_Error("\nError !!! in %s() Unrecognized DRM type [%d]\n",__FUNCTION__, pstSiDrmInfo->stSiCcInfo.eSiCcType);
			hErr = ERR_FAIL;
			break;
	}

	return hErr;
}

HERROR pmgr_copyrightcontrol_GetCasCopyProtectInfo(HBOOL bPlayback, const DxCopyrightControl_t *pstCasDrmInfo, MgrCopyrightControl_OutputControlInfo_t *pstCpInfo)
{
	HERROR		hErr;

	if (NULL == pstCasDrmInfo)
	{
		HxLOG_Critical("\n\n");
		return ERR_FAIL;
	}

	if (NULL == pstCpInfo)
	{
		HxLOG_Critical("\n\n");
		return ERR_FAIL;
	}

	HxSTD_memset(pstCpInfo, 0, sizeof(MgrCopyrightControl_OutputControlInfo_t));
	hErr = ERR_FAIL;


	switch(pstCasDrmInfo->stCasCcInfo.eCasCcType)
	{
#if defined(CONFIG_MW_CAS_NAGRA_OP_SES) // DRM_TEST
		case eDxCopyrightControl_CasCcType_NagraSes:
			hErr = xmgr_copyrightcontrol_GetCasCopyProtectInfo_NagraSes((DxCopyrightControl_t *)pstCasDrmInfo, pstCpInfo);
			break;
#endif
#if defined(CONFIG_MW_CI_PLUS)
		case eDxCopyrightControl_CasCcType_CiPlus:
			hErr = xmgr_copyrightcontrol_GetCopyProtectInfo_CiPlus(bPlayback, (DxCopyrightControl_t *)pstCasDrmInfo, pstCpInfo);
			break;
#endif
#if defined(CONFIG_MW_CAS_NAGRA)
		case eDxCopyrightControl_CasCcType_Nagra:
			hErr = xmgr_copyrightcontrol_GetCopyProtectInfo_Nagra((DxCopyrightControl_t *)pstCasDrmInfo, pstCpInfo);
			break;
#endif
#if defined(CONFIG_MW_CAS_VERIMATRIX_IPTV)
		case eDxCopyrightControl_CasCcType_Verimatrix:
			hErr = xmgr_copyrightcontrol_GetCasCopyProtectInfo_VmxIptv((DxCopyrightControl_t *)pstCasDrmInfo, pstCpInfo);
			break;
#endif
#if defined(CONFIG_MW_CAS_CONAX)
		case eDxCopyrightControl_CasCcType_Conax:
			hErr = xmgr_copyrightcontrol_GetCopyProtectInfo_Conax(bPlayback, (DxCopyrightControl_t *)pstCasDrmInfo, pstCpInfo);
			break;
#endif
#if 0 // TODO: MGR_CAS_XX_XXX 로 구현 예정.
			case eDxCopyrightControl_CasCcType_Irdeto:
			case eDxCopyrightControl_CasCcType_Viaccess:

				break;
#endif
		default:
			HxLOG_Error("\nError !!! in %s() Unrecognized DRM type [%d]\n",__FUNCTION__, pstCasDrmInfo->stCasCcInfo.eCasCcType);
			hErr = ERR_FAIL;
			break;
	}

	return hErr;

}

HERROR pmgr_copyrightcontrol_GetCasMediaControlInfo(DxCopyrightControl_CasCcType_e eDrmType, const DxCopyrightControl_t *pstCasDrmInfo, MgrCopyrightControl_MediaControlInfo_t *pstCpInfo)
{
	HERROR		hErr;

	if (NULL == pstCasDrmInfo)
	{
		HxLOG_Critical("\n\n");
		return ERR_FAIL;
	}

	if (NULL == pstCpInfo)
	{
		HxLOG_Critical("\n\n");
		return ERR_FAIL;
	}

	HxSTD_memset(pstCpInfo, 0, sizeof(MgrCopyrightControl_MediaControlInfo_t));
	hErr = ERR_FAIL;


	switch(pstCasDrmInfo->stCasCcInfo.eCasCcType)
	{
#if defined(CONFIG_MW_CAS_NAGRA_OP_SES) // DRM_TEST
		case eDxCopyrightControl_CasCcType_NagraSes:
			hErr = xmgr_copyrightcontrol_GetCasMediaControlInfo_NagraSes((DxCopyrightControl_t *)pstCasDrmInfo, pstCpInfo);
			break;
#endif

#if 0 // TODO: MGR_CAS_XX_XXX 로 구현 예정.
			case eDxCopyrightControl_CasCcType_CiPlus:
			case eDxCopyrightControl_CasCcType_Nagra:
			case eDxCopyrightControl_CasCcType_Irdeto:
			case eDxCopyrightControl_CasCcType_Viaccess:

				break;
#endif

#if defined(CONFIG_MW_CAS_CONAX)
		case eDxCopyrightControl_CasCcType_Conax:
			hErr = xmgr_copyrightcontrol_GetCasMediaControlInfo_Conax((DxCopyrightControl_t *)pstCasDrmInfo, pstCpInfo);
			break;
#endif

#if defined(CONFIG_MW_CAS_VERIMATRIX)
		case eDxCopyrightControl_CasCcType_Verimatrix:
			hErr = xmgr_copyrightcontrol_GetCasMediaControlInfo_Verimatrix((DxCopyrightControl_t *)pstCasDrmInfo, pstCpInfo);
			break;
#endif

			default:
				HxLOG_Error("\nError !!! in %s() Unrecognized DRM type [%d]\n",__FUNCTION__, pstCasDrmInfo->stCasCcInfo.eCasCcType);
				hErr = ERR_FAIL;
				break;
	}

	return hErr;

}

MgrCopyrightControl_Priority_e		pmgr_copyrightcontrol_GetPriority(void)
{
#if defined(CONFIG_MW_CAS_NAGRA_OP_SES)
	return eCopyrightControl_Priority_Cas;
#else
	return eCopyrightControl_Priority_Component;
#endif
}

/*********************** End of File ******************************/

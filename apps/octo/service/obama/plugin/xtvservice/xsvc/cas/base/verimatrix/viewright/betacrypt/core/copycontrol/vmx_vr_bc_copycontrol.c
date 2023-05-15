/* (c) 2011-2015 Humax Co., Ltd. 
 * This program is produced by Humax Co., Ltd. ("Humax") and 
 * the proprietary Software of Humax and its licensors. Humax provides you, as an Authorized Licensee, 
 * non-assignable, non-transferable and non-exclusive license to use this Software. 
 * You acknowledge that this Software contains valuable trade secrets of Humax and by using this Software 
 * you agree to the responsibility to take all reasonable efforts to protect the any information 
 * you receive from Humax. You are not permitted to duplicate, modify, distribute, sell or lease and 
 * reverse engineer or extract the source code of this Software unless you have Humax's written permission to do so.
 * If you have no authorized license, discontinue using this Software immediately. 
 * 
 * THE SOFTWARE IS PROVIDED "AS IS" AND HUMAX MAKES NO PROMISES, REPRESENTATIONS OR WARRANTIES, EITHER EXPRESS, 
 * IMPLIED OR STATUTORY, OR OTHERWISE, WITH RESPECT TO THE SOFTWARE. 
 * IN NO EVENT SHALL HUMAX BE LIABLE FOR LOST PROFITS, REVENUES, OR DATA, FINANCIAL LOSSES OR INDIRECT, SPECIAL,
 * CONSEQUENTIAL, EXEMPLARTY OR PUNITIVE DAMAGES WHATSOEVER RELATING TO YOUR USE OR INABILITY TO USE THE SOFTWARE.

 * This License is effective until terminated. You may terminate this License at any time by destroying all copies 
 * of the Software including all documentation. This License will terminate immediately without notice from Humax 
 * to you if you fail to comply with any provision of this License. Upon termination, you must destroy all copies 
 * of the Software and all documentation.  

 * The laws of the Republic of Korea will apply to any disputes arising out of or relating to this Copyright Notice. 
 * All claims arising out of or relating to this Copyright Notice will be litigated in the Seoul Central District Court, 
 * in the Republic of Korea.
*/
/**
	@file     vmx_vr_bc_copycontrol.c
	@brief    vmx_vr_bc_copycontrol.c (Verimatrix BC CC)

	Description:   \n
	Module: MW/CAS/ VERIMATRIX \n
	Remarks :\n

	Copyright (c) 2015 HUMAX Co., Ltd. \n
	All rights reserved. \n
*/

/*******************************************************************/
/********************      Header Files    *************************/
/*******************************************************************/
#include "mwc_drm.h"
#include "casmgr_mgr.h"
#include "_xsvc_cas_vmx_copycontrol.h"
#include "_xsvc_cas_vmx_os.h"
#include "_xsvc_cas_vmx_bc_main.h"

/*******************************************************************/
/********************      Defines         *************************/
/*******************************************************************/

/*******************************************************************/
/********************      Typedef         *************************/
/*******************************************************************/
/*
typedef enum
{
	eSvcXxx_Type,

} xsvcXxx_Type_e;

typedef  struct
{
	...
} xsvcXxx_Type_t;
*/

/*******************************************************************/
/********************      Global Variables     ********************/
/*******************************************************************/


/*******************************************************************/
/********************      Static Variables     ********************/
/*******************************************************************/


/*******************************************************************/
/********************      Functions     ********************/
/*******************************************************************/
/*	
// Local function
STATIC HERROR xsvc_xxx_DoSomething_Yyy( ... )
{
	...
}
*/

/*
// Global function
HERROR xsvc_xxx_DoSomething_Yyy( ... )
{
	...
}
*/

HERROR bc_cc_SetMacrovision(HUINT16 wMv, VmxBcDrmInfo_t *pstDrm)
{
	if(pstDrm == NULL)
	{
		return ERR_FAIL;
	}

	// TODO
	if(wMv > 255)
	{
		// MV is not support.
	}

	pstDrm->ucMacrovisionMode = wMv;

	return ERR_OK;
}

HERROR bc_cc_SetCgmsa(HUINT8 bCgmsa, VmxBcDrmInfo_t *pstDrm)
{
	if(pstDrm == NULL)
	{
		return ERR_FAIL;
	}

	if(bCgmsa == 0)
		pstDrm->eOutputInCgms = eVmxCgmsCopyFreely;
	else if(bCgmsa == 1)
		pstDrm->eOutputInCgms = eVmxCgmsCopyNoFurtherCopy;
	else if(bCgmsa == 3)
		pstDrm->eOutputInCgms = eVmxCgmsCopyCopyOnce;
	else if(bCgmsa == 4)
		pstDrm->eOutputInCgms = eVmxCgmsCopyCopyNever;
	else
	{
		HxLOG_Error(DBG_ASSERT, ("bCgmsa = %d\n", bCgmsa);
	}

	return ERR_OK;
}

HERROR bc_cc_SetHdcp(HUINT8 bHdcp, VmxBcDrmInfo_t *pstDrm)
{
	if(pstDrm == NULL)
	{
		return ERR_FAIL;
	}

	if(bHdcp == 0)
		pstDrm->ucHdcp = 0;
	else
		pstDrm->ucHdcp = 1;

	return ERR_OK;
}

HERROR bc_cc_SetHD2SD(HUINT8 bHd2Sd, VmxBcDrmInfo_t *pstDrm)
{
	if(pstDrm == NULL)
	{
		return ERR_FAIL;
	}

	if(bHd2Sd == 0)
		pstDrm->ucHD2SD = 0;
	else
		pstDrm->ucHD2SD = 1;

	return ERR_OK;
}

HERROR bc_cc_SetEmiCci(HUINT8 bEmiCci, VmxBcDrmInfo_t *pstDrm)
{
	if(pstDrm == NULL)
	{
		return ERR_FAIL;
	}

	if(bEmiCci == 0)
		pstDrm->eEmiCci = eVmxEmiCciCopyFreely;
	else if(bEmiCci == 1)
		pstDrm->eEmiCci = eVmxEmiCciCopyNoFurtherCopy;
	else if(bEmiCci == 2)
		pstDrm->eEmiCci = eVmxEmiCciCopyCopyOnce;
	else if(bEmiCci == 3)
		pstDrm->eEmiCci = eVmxEmiCciCopyCopyNever;
	else
	{
		HxLOG_Error(DBG_ASSERT, ("bEmiCci = %d\n", bEmiCci);
	}

	return ERR_OK;
}

HERROR vmx_vr_bc_copycontrol(HUINT8 usServiceIdx, HUINT16 usMv, HUINT8 ucCgmsa, HUINT8 ucHdcp, HUINT8 ucHd2Sd, HUINT8 ucEmiCci)
{
	VmxBcDrmInfo_t	stDrmInfo;
	Handle_t 			hAction = HANDLE_NULL;
	Handle_t 			ucScSlot = 0;

	// TODO_VMX : It is work using usServiceIdx
	HxLOG_Print(DBG_PRINT, ("usServiceIdx(%d), usMv(0x%x), ucCgmsa(0x%x), ucHdcp(0x%x), ucHd2Sd(0x%x), ucEmiCci(0x%x)\n", usServiceIdx, usMv, ucCgmsa, ucHdcp, ucHd2Sd, ucEmiCci);	

	VK_memset(&stDrmInfo, 0, sizeof(VmxBcDrmInfo_t));

	//VMX_TODO_CC  : hAction = bServiceIdx로부터 hAction을 얻어낸다.
	//VMX_TODO_CC : ucScSlot = 

	if (hAction == HANDLE_NULL)
	{
		HxLOG_Print(DBG_PRINT, ("hAction is failed!! ! \n");
		return ERR_FAIL;
	}

	bc_cc_SetMacrovision(usMv, &stDrmInfo);
	bc_cc_SetCgmsa(ucCgmsa, &stDrmInfo);
	bc_cc_SetHdcp(ucHdcp, &stDrmInfo);
	bc_cc_SetHD2SD(ucHd2Sd, &stDrmInfo);
	bc_cc_SetEmiCci(ucEmiCci, &stDrmInfo);

	return CASMGR_UpdateDrmInfo(hAction, ucScSlot, (void *)&stDrmInfo);
}


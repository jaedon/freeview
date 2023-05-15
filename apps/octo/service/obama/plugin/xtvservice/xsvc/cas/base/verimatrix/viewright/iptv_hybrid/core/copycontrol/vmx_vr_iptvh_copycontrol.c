/*******************************************************************
	File Description
********************************************************************/
/**
	@file	  vmx_vr_iptvh_copycontrol.c
	@brief

	Description:  									\n
	Module: 	OBAMA / TvService				\n

	Copyright (c) 2014 HUMAX Co., Ltd.				\n
	All rights reserved.							\n
*/

/*
* Character encoding.
*
* MS949
*
* This source file that uses MS949 encoding.
* MS949 encoding 을 사용하는 source file 입니다.
* MS949 encodingを使用して source fileです。
* Quelldatei, die MS949-Codierung verwendet.
*
*/
/*
 * (c) 2011-2013 Humax Co., Ltd.
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

/*******************************************************************/
/********************      Header Files    *************************/
/*******************************************************************/
#include "_xsvc_cas_vmx_iptv_main.h"
#include "vmx_vr_iptvh_mgr.h"
#include "_xsvc_cas_vmx_os.h"

/*******************************************************************/
/********************      Defines         *************************/
/*******************************************************************/

#define VMX_GET_EMI_VALUE(x)		(x & 0x03)
#define VMX_GET_APS_VALUE(x)		((x & 0x0c) >> 2)
#define VMX_GET_DC_VALUE(x)			(x & 0x01)
#define VMX_GET_HDCP_VALUE(x)		((x & 0x02) >> 1)

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

// see the ENG-208_ViewRight_STB_for_IPTV_3.5_API_Integration_Guide.pdf, page 64
void vmx_vr_iptvh_copycontrol_CopycontrolCallback(void *pUserParam, int dataLength, void *pCopyControlData)
{
	Vmx_Instance_t 			*pstInst = xsvc_vmx_main_GetInstance();
	DxCopyrightControl_t	 stCopyControl;
	Vmx_ActionTable_t 		*pstAction;
	Handle_t 				 hAction = HANDLE_NULL;
	HUINT32 				i;

	HUINT8 *pucCcData = NULL;
	HUINT8 ucCCI_Flag = 0;		// SCET41_CCI flags
	HUINT8 ucCCI2_Flag = 0;		// CCI2 flags

	HxLOG_Print("[Enter] dataLen(%d) \n", dataLength);

	if (pstInst == NULL)
	{
		HxLOG_Error("Error! NULL instance \n");
		return;
	}

	if ((pCopyControlData == NULL) || (dataLength == 0))
	{
		HxLOG_Error("Error!! pCopyControlData(%p) dataLength(%d) \n", pCopyControlData, dataLength);
		return;
	}

	if (pUserParam == NULL)
	{
		HxLOG_Error("Error! user param not exist!\n");
		return;
	}

	pstAction = (Vmx_ActionTable_t *)pUserParam;
	for (i = 0; i < VMX_CAS_MAX_NUM_OF_ACTION; i++)
	{
		if(pstAction == &pstInst->stActionList[i])
		{
			hAction = pstInst->stActionList[i].hAction;
			break;
		}
	}

	if (hAction == HANDLE_NULL)
	{
		HxLOG_Error("Error! Can't find hAction. \n");
		return;
	}

	HxSTD_MemSet(&stCopyControl, 0x00, sizeof(DxCopyrightControl_t));
	pucCcData = (HUINT8 *)pCopyControlData;

	switch (dataLength)
	{
		case 1: // 1 byte만 들어오는 경우도 있을까?? 일단 들어온 것이라도 적용해보자
			ucCCI_Flag = *pucCcData;
			HxLOG_Error("Check case !! dataLength(%d) ucCCI_Flag(0x%X) \n", dataLength, ucCCI_Flag);
			break;

		case 2: // normal case
			ucCCI_Flag = *pucCcData;
			ucCCI2_Flag = *(pucCcData + 1);
			break;

		default: // error
			HxLOG_Error("Error! not defined data length(%d) \n", dataLength);
			return;
	}

	stCopyControl.bCasCc = TRUE;
	stCopyControl.stCasCcInfo.eCasCcType = eDxCopyrightControl_CasCcType_Verimatrix;
	stCopyControl.stCasCcInfo.info.stVmxDrmInfo.usVmxType = eDxCopyrightControl_VerimatrixType_Iptv;

	// EMI, API
	if (ucCCI_Flag > 0)
	{
		stCopyControl.stCasCcInfo.info.stVmxDrmInfo.eEmiCopyCtrl = VMX_GET_EMI_VALUE(ucCCI_Flag);
		stCopyControl.stCasCcInfo.info.stVmxDrmInfo.eApsCopyCtrl = VMX_GET_APS_VALUE(ucCCI_Flag);
	}

	// DC, HDCP
	if (ucCCI2_Flag > 0)
	{
		stCopyControl.stCasCcInfo.info.stVmxDrmInfo.bDC 	= VMX_GET_DC_VALUE(ucCCI2_Flag);
		stCopyControl.stCasCcInfo.info.stVmxDrmInfo.bHdcp	= VMX_GET_HDCP_VALUE(ucCCI2_Flag);
	}

	if(svc_cas_MgrSubUpdateCasEtcEventData(hAction, (Handle_t)NULL, eDxCAS_GROUPID_VX, eSEVT_CAS_DRM_SIGNALLED, 0,0, (void*)&stCopyControl, sizeof(DxCopyrightControl_t)) != 0)
	{
		HxLOG_Error("svc_cas_MgrSubUpdateCasEtcEventData() error! \r\n");
	}

	HxLOG_Print("rcv CopyControl : eEmiCopyCtrl(%d) eApsCopyCtrl(%d) bDC(%d) bHdcp(%d) \n",
					stCopyControl.stCasCcInfo.info.stVmxDrmInfo.eEmiCopyCtrl, stCopyControl.stCasCcInfo.info.stVmxDrmInfo.eApsCopyCtrl,
					stCopyControl.stCasCcInfo.info.stVmxDrmInfo.bDC, stCopyControl.stCasCcInfo.info.stVmxDrmInfo.bHdcp);

	HxLOG_Print("[Exit]\n");

	return;
}

/* end of file */

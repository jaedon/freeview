/*
 * (c) 2011-2015 Humax Co., Ltd.
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
/********************************************************************
 * $Workfile:   _xsvc_cas_vmx_iptvh_ui.h  $
 * Project: 	 Verimatrix CAS
 * Author:		kwang-cheon, ahn
 * Description:	API for Verimatrix
 *
 *                              Copyright (c) 2015 by HUMAX Co., Ltd.
 *                                               All rights reserved.
 ********************************************************************/

#ifndef __INT_XSVC_VMXIPTVHUI_H__
#define __INT_XSVC_VMXIPTVHUI_H__

/*******************************************************************/
/********************      Header Files    *************************/
/*******************************************************************/
#include <octo_common.h>
#include "_svc_cas_mgr_common.h"
#include "VMCallbackTypes.h"

/************************************************************/
/********************      Defines         *************************/
/*******************************************************************/
#define VMX_FINGERPRINTING_LOCALDATA_MAX_NUM			(3) // ? 왜 3이지?

/*******************************************************************/
/********************      Typedef         *************************/
/*******************************************************************/
/*
typedef enum
{
	eSvcXxx_Type
} xsvcXxx_Type_e;

typedef struct	
{
} xsvcXxx_Type_t;
*/

typedef enum
{
	eVmx_Solution_Iptv_Hybrid = 0,
	eVmx_Solution_Dvb_Only
} Vmx_Solution_e;

typedef enum
{
	eVmxIptv_Display_FingerPrinting = 0,
	eVmxIptv_Display_OsdMessage,
	eVmxIptv_Display_ErrorMessage,
	eVmxIptv_Display_PinDialog,
	eVmxIptv_Display_Max
} VmxIptv_DisplayType_e;

typedef enum
{
	eVmxIptv_Mode_Always_On_Top = 0,
	eVmxIptv_Mode_User_Exit,
	eVmxIptv_Mode_Timeout,	
} VmxIptv_DisplayMode_e;

typedef enum
{
	// Decode Error Message
	eVmxIptv_ErrMsg_DecodeInit = 0,                   		// Initial state to detect first decode
	eVmxIptv_ErrMsg_DecodeOk,                     		// Decode proceeding
	eVmxIptv_ErrMsg_DecodeFailurePinRequired,    		// program Rating Level exceeds STB's
	eVmxIptv_ErrMsg_DecodeFailureKeyNotAvailable,     // Could be not authorized or network error getting key
	eVmxIptv_ErrMsg_DecodeFailureBlackout,             	// Service Class >= 0xFF0000 matched
	eVmxIptv_ErrMsg_DecodeFailureOther,                	// TBD
	eVmxIptv_ErrMsg_DecodeFailureTA,                		//  decryption fail due to trustzone applet error
	// Communication Error Message With VCAS
//	eVmxIptv_ErrMsg_Communication


	
	eVmxIptv_ErrMsg_Max,
} VmxIptv_ErrorMessage_e;

/* ---------- Finter Printing ---------- */
typedef enum
{
	eVmxIptv_Quadrant_C = 0,
	eVmxIptv_Quadrant_N,
	eVmxIptv_Quadrant_NE,	
	eVmxIptv_Quadrant_E,
	eVmxIptv_Quadrant_SE,
	eVmxIptv_Quadrant_S,
	eVmxIptv_Quadrant_SW,
	eVmxIptv_Quadrant_W,
	eVmxIptv_Quadrant_NW	
} VmxIptv_Quadrant_e;

typedef struct
{
	HUINT16 usType;
	HUINT8  ucDuration;
	VmxIptv_Quadrant_e  eQuadrant;
} VmxIptv_Fingerprinting_Format_t;

typedef struct
{
	HUINT16 usType;
	HUINT8  ucLength;
	HUINT8  *pData;
} VmxIptv_Fingerprinting_LocalData_t;

typedef struct
{
	HUINT16 usFormatLenght;
	VmxIptv_Fingerprinting_Format_t		stFormat;
	HUINT16 usLocalDataLength;
	VmxIptv_Fingerprinting_LocalData_t	stLocalData[VMX_FINGERPRINTING_LOCALDATA_MAX_NUM];
} VmxIptv_Fingerprinting_t;

typedef struct
{	
	HUINT16 usLenght;
	HUINT32 ulPeriod;
	HUINT8  *pucMsg;
} VmxIptv_PopupItem_t;

typedef struct
{
	VmxIptv_ErrorMessage_e eErrMsg;	
	HUINT32 ulChannel;
	HUINT32 ulValue;
	HUINT8  *pucMsg;
} VmxIptv_ErrorMsg_t;

typedef struct
{
	HUINT32		hAction;
	Vmx_Solution_e			eSolution;
	VmxIptv_DisplayType_e	eType;
	VmxIptv_DisplayMode_e   	eMode;

	union
	{
		VmxIptv_Fingerprinting_t		stFingrPrinting;	// finger print
		VmxIptv_PopupItem_t			stPopupItem;	// normal msg
		VmxIptv_ErrorMsg_t			stErrorMsg;
		// TODO : 필요시 아래에 추가함.
	} item;	
} VmxIptv_DisplayUi_t;

/*******************************************************************/
/********************      Global Variables     ********************/
/******************************************************************/


/*******************************************************************/
/********************      Functions     ********************/
/*******************************************************************/
// HERROR xsvc_xxx_yyy_DoSomething( ... );

HERROR vmx_vr_ui_Init(void);
HERROR vmx_vr_iptvh_ui_Init(void);
HERROR xsvc_cas_vmx_UpdateCasMessage(Handle_t hAction,SvcCas_MsgType_e eMsgType, DxCAS_GroupId_e eCasId, void *pMessage, SvcCas_ShowType_e eShowType);
#endif 	//#ifndef __INT_XSVC_VMXIPTVHUI_H__


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
 * $Workfile:   _xsvc_cas_vmx_bc_sc.h  $
 * Project: 	 Verimatrix CAS
 * Author:		ahn
 * Description:	API for Verimatrix
 *
 *                              Copyright (c) 2015 by HUMAX Co., Ltd.
 *                                               All rights reserved.
 ********************************************************************/

#ifndef __INT_XSVC_VMXBCSC_H__
#define __INT_XSVC_VMXBCSC_H__

#if defined(CONFIG_MW_CAS_VERIMATRIX_DVB)
/*******************************************************************/
/********************      Header Files    *************************/
/*******************************************************************/

/*******************************************************************/
/********************      Defines         *************************/
/*******************************************************************/
#define VMXBC_SC_SLOT_NUM						(1)
#define VMXBC_SC_INVALID_SLOT					(-1)
#define VMXBC_SC_INVALID_HANDLE					(-1)

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
	eVmxBc_MsgType_Init = 0,
	eVmxBc_MsgType_Reset, 
	eVmxBc_MsgType_ScExtracted,
	eVmxBc_MsgType_ScInserted,
	eVmxBc_MsgType_WriteToSc,
	eVmxBc_MsgType_Error
} VmxBc_SC_Msg_e;

typedef enum
{
	eVmxBc_Cmd_ConnectSc = 0,
	eVmxBc_Cmd_DisconnectSc,
	eVmxBc_Cmd_ResetSc,
	eVmxBc_Cmd_GetATRSc,
	eVmxBc_Cmd_CardDetectSc
} VmxBc_Cmd_e;

/*******************************************************************/
/********************      Global Variables     ********************/
/******************************************************************/


/*******************************************************************/
/********************      Functions     ********************/
/*******************************************************************/

// HERROR xsvc_xxx_yyy_DoSomething( ... );
HERROR xsvc_cas_vmx_bc_sc_Init(void);
HERROR xsvc_cas_vmx_bc_sc_Write(HINT32 nScSlot, HUINT8 *pucSndData, HUINT32 ulSndLen, HUINT8 *pucRcvData, HUINT32 *pulNumRead);
HERROR xsvc_cas_vmx_bc_sc_Reset(HINT32 nScSlot, HUINT8 *pucAtr, HUINT32 *pulAtrLen, HUINT8 *pucProtocol);

HERROR vmx_vr_bc_sc_Init(void);
HULONG vmx_vr_bc_sc_GetMsgQueueId(void);
HERROR vmx_vr_bc_sc_WriteToSC(HUINT8* pucBuffer, HUINT16 *pusLen, HUINT16 usTimeout);
HERROR vmx_vr_bc_sc_ReadFromSC(HUINT8* pucBuffer, HUINT16 *pusLen, HBOOL *pbTimeout);
HERROR  vmx_vr_bc_sc_IoCtrlWithSC(VmxBc_Cmd_e eCmd, void* pvParams, HUINT16* pusLen);
#endif
#endif 	//#ifndef __INT_XSVC_VMXBCSC_H__


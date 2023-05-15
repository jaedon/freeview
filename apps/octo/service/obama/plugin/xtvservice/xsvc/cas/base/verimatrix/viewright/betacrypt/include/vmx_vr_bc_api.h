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
 * $Workfile:   vmx_vr_bc_api.h  $
 * Project: 	 Verimatrix CAS
 * Author:		ahn
 * Description:	API for Verimatrix
 *
 *                              Copyright (c) 2015 by HUMAX Co., Ltd.
 *                                               All rights reserved.
 ********************************************************************/

#ifndef __INT_XSVC_VMXDVBVIEWRIGHT_H__
#define __INT_XSVC_VMXDVBVIEWRIGHT_H__

//#include "bc_consts.h"

/*******************************************************************/
/********************      Defines         *************************/
/*******************************************************************/
/* Define BetaCrypt features */
#if 1
#define BC_NSC_INCLUDED                     // Non SmartCard
//#define BC_CHAT_INCLUDED
//#define DOWNLOAD_CHECK
//#define MULTI_CA_SUPPORT
//#define BC_RC_INCLUDED
//#define BC_IP_INCLUDED
//#define BC_MS_INCLUDED
//#define BC_PVR_INCLUDED                     // PVR
//#define BC_DVR_INCLUDED
//#define BC_CIPLUS
#define BC_COPYCONTROL
#endif

#define VMX_SECBUF_SIZE         4096        // section buffer size

#define BC_OSM_TRIGGER						(0x06)

#define BC_OSM_TRIGGER_UPGRADE				(0x55)	// 'U'
#define BC_OSM_TRIGGER_RESCAN				(0x52)	// 'R'
#define BC_OSM_TRIGGER_SWITCH				(0x53)	// 'S'
#define BC_OSM_TRIGGER_WATERMARKING		(0x57)	// 'W'
#define BC_OSM_TRIGGER_PINNUMBERRESET		(0x50)	// 'P'
#define BC_OSM_TRIGGER_EAS					(0x45)	// 'E'

// In case of BC_OSM_TRIGGER_UPGRADE
#define OSM_U_FORCED_UPGRADE				(0x46)
#define OSM_U_UPGRADE_NOW					(0x4e)
#define OSM_U_UPGRADE_NOW_AFTER_USER_CONFIRMATION							(0x6e)
#define OSM_U_UPGRADE_POWER_DOWN			(0x44)
#define OSM_U_UPGRADE_ON_POWER_DOWN_AFTER_USER_CONFIRMATION			(0x64)
#define OSM_U_UPGRADE_POWER_UP			(0x55)
#define OSM_U_UPGRADE_ON_POWER_UP_AFTER_USERCONFIRMATION				(0x75)
#define OSM_U_UPGRADE_ON_NEXT_SERVICE_CHANGE								(0x53)
#define OSM_U_UPGRADE_ON_NEXT_NON_DESCRAMBLING_PERIOD					(0x43)

// In case of BC_OSM_TRIGGER_RESCAN
#define OSM_R_RESCAN_NOW					(0x4e)
#define OSM_R_RESCAN_NOW_AFTER_USER_CONFIRMATION							(0x6e)
#define OSM_R_RESCAN_ON_POWER_DOWN		(0x44)
#define OSM_R_RESCAN_ON_POWER_DOWN_AFTER_USER_CONFIRMATION				(0x64)
#define OSM_R_RESCAN_ON_POWER_UP			(0x55)
#define OSM_R_RESCAN_ON_POWER_UP_AFTER_USER_CONFIRMATION					(0x75)
#define OSM_R_RESCAN_ON_NEXT_SERVICE_CHANGE									(0x53)
#define OSM_R_RESCAN_ON_NEXT_NON_DESCRAMBLING_PERIOD						(0x43)

// In case of BC_OSM_TRIGGER_SWITCH
#define OSM_S_SWITCH_SERVICE_NOW			(0x4e)  // 'N'
#define OSM_S_SWITCH_AT_TIME					(0x41)  // 'A'
#define OSM_S_SWITCH_FOR_TIMED_PERIOD		(0x50)  // 'P'
#define OSM_S_SWITCH_AT_TIME_FOR_TIMED_PERIOD									(0x70)  // 'p'
#define OSM_S_SWITCH_IF_SERVICE_CANNOT_BE_DESCRAMBLED						(0x43)  // 'C'
#define OSM_S_SWITCH_UNTIL_TIME_REACHED	(0x55)  // 'U'
#define OSM_S_SWITCH_BACK_NOW				(0x42)  // 'B'
#define OSM_S_SWITCH_BACK_IN_X_SECONDS	(0x62)  // 'b'
#define OSM_S_SWITCH_BACK_AT_TIME			(0x61)  // 'a'



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




/*******************************************************************/
/********************      Global Variables     ********************/
/******************************************************************/


/*******************************************************************/
/********************      Functions     ********************/
/*******************************************************************/
// HERROR xsvc_xxx_yyy_DoSomething( ... );
HINT16 vmx_vr_bc_api_BcInit(void);
HINT16 vmx_vr_bc_api_CheckNSc( void );
#ifdef BC_NSC_INCLUDED
void vmx_vr_bc_api_ConnectScNotify(void* pvParams);
HERROR vmx_vr_bc_api_InitNSc(HUINT8* pabPtr, HUINT16* pwLen );
HERROR vmx_vr_bc_api_InitNScComm( HUINT8* pabPtr, HUINT16* pwLen, HUINT8* pabBoxSerNo, HUINT8* pabManuId, HUINT8* pabManuData, HUINT8* pabProvId, HUINT8* pabProvData );
#endif
void vmx_vr_bc_api_Task(HUINT32 argc, void *argv);
void vmx_vr_bc_api_GetVersion( HUINT8* pacVersion, HUINT8* pacDate, HUINT8* pacTime );
HERROR vmx_vr_bc_api_GetCASystemID (HUINT16 *usCaSystemId);
HERROR vmx_vr_bc_api_SetEmmPid(HUINT16 wEmmPid);
HUINT8 vmx_vr_bc_api_GetMacroVisionCtrl( HUINT16 wServiceId, HUINT8 bServiceIdx );
HERROR vmx_vr_bc_api_CheckPin( HUINT8 bPinLength, HUINT8* pabPin, HUINT8 bPinIndex, HUINT8 bReason, HUINT8 bServiceIdx );
HERROR vmx_vr_bc_api_ChangePin( HUINT8 bOldPinLength, HUINT8* pabOldPin, HUINT8 bNewPinLength, HUINT8* pabNewPin, HUINT8 bPinIndex ); 
HERROR vmx_vr_bc_api_SetIoctlNotify(void);
HERROR vmx_vr_bc_api_StartDescrambling( HUINT16 wServiceId, HUINT16 wIndex, HUINT16 * pawEcmPid, HUINT16 *pawStreamPid, HUINT8 bServiceIdx );
HERROR  vmx_vr_bc_api_StopDescrambling( HUINT16 wServiceId, HUINT8 bServiceIdx );
HERROR vmx_vr_bc_api_SetSTBFilterAddress( HUINT8 bAddressLen, HUINT8* pabAddress);
void vmx_vr_bc_api_InitWindow( HUINT16 wMaxWidth, HUINT16 wMaxHeight, HUINT8* pabUnblockString );
HERROR vmx_vr_bc_api_GetSCNo( HUINT8* pabSerNo, HUINT16 wLen );
void vmx_vr_bc_api_GetPurse( void * pvParams );
void vmx_vr_PrintfDump(char *title, void *buf, unsigned int len);
#endif		//#ifndef __INT_XSVC_VMXDVBVIEWRIGHT_H__


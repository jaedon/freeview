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

/******************************************************************************/
/**
 * @file	  		nx_core_network.h
 *
 * 	Description:  									\n
 *
 * @author											\n
 * @date											\n
 * @Attention		Copyright (c) 2013 HUMAX Co., Ltd. - All rights reserved.
 *
 **/
/******************************************************************************/

#ifndef __NX_CORE_NETWORK_H__
#define __NX_CORE_NETWORK_H__

/*******************************************************************/
/********************	Header Files		************************/
/*******************************************************************/
#include <ondk.h>
#include <hapi.h>
#include <papi.h>
#include <oapi.h>
#include <apk.h>

#include <nx_common.h>
#include <nx_port.h>


/*******************************************************************/
/********************	Extern variables	************************/
/*******************************************************************/


/*******************************************************************/
/********************	Macro Definition	 ***********************/
/*******************************************************************/


/*******************************************************************/
/********************		Typedef			************************/
/*******************************************************************/


/*******************************************************************/
/********************	Private Variables		********************/
/*******************************************************************/


/*******************************************************************/
/********************	Global Variables		********************/
/*******************************************************************/

HERROR  NX_NETWORK_GetLANMacAddr(HCHAR *pszMacAddr, HINT32 nMax);
HERROR  NX_NETWORK_GetWLANMacAddr(HCHAR *pszMacAddr, HINT32 nMax);
HERROR  NX_NETWORK_GetBluetoothMacAddr(HCHAR *pszMacAddr);

HERROR  NX_NETWORK_GetNetDevInfo(NX_NET_NetDevInfo_t **pstNetDevInfo, HINT32* nNetDevCount);
HERROR  NX_NETWORK_GetLANConnected(HBOOL *bLANConnected);
void    NX_NETWORK_RegisterNotifier(void);
void    NX_NETWORK_UnRegisterNotifier(void);
HERROR  NX_NETWORK_EnableNetInterface(NX_NET_NetIfType_e netIfType);
HERROR  NX_NETWORK_StartScan(void);
HERROR  NX_NETWORK_ConnectAP(APKS_NETAP_Info_t *pstApInfo, HCHAR *szKey );
HERROR  NX_NETWORK_GetLanInterface(NX_NET_NetIFADDR_t** pstNetIf);
HERROR  NX_NETWORK_GetWLanInterface(NX_NET_NetIFADDR_t** pstNetIf);
HERROR  NX_NETWORK_SetLANNetInterface(NX_NET_NetIFADDR_t *pstNetAddr);
HERROR  NX_NETWORK_CheckGateway(NX_NET_NetIfType_e netIfType);
HERROR  NX_NETWORK_CheckDNS(NX_NET_NetIfType_e netIfType);

#endif // __NX_CORE_NETWORK_H__


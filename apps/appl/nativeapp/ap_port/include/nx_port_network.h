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
 * @file	  		nx_port_network.h
 *
 * 	Description:  									\n
 *
 * @author											\n
 * @date											\n
 * @Attention		Copyright (c) 2013 HUMAX Co., Ltd. - All rights reserved.
 *
 **/
/******************************************************************************/

#ifndef __NX_PORT_NETWORK_H__
#define __NX_PORT_NETWORK_H__

/*******************************************************************/
/********************	Header Files		************************/
/*******************************************************************/

#include <ondk.h>
#include <hapi.h>
#include <papi.h>
#include <hlib.h>
#include <dlib.h>
#include <apk.h>

#include <nx_port_type.h>


/*******************************************************************/
/********************	Extern variables	************************/
/*******************************************************************/


/*******************************************************************/
/********************	Macro Definition	 ***********************/
/*******************************************************************/

#define NX_NET_MAX_MACADDR  (20)
#define	NX_NET_MAX_IP4ADDR	(16)
#define	NX_NET_MAX_MACADDR	(20)
#define NX_NET_MAX_IP6ADDR	(40)
#define NX_NET_FW_VERSION_LEN_MAX (128)
/*******************************************************************/
/********************		Typedef			************************/
/*******************************************************************/

typedef enum NX_NET_NetIfType{
    eNxNet_INTERFACE_TYPE_LAN = 0,
    eNxNet_INTERFACE_TYPE_WLAN,
    eNxNet_INTERFACE_TYPE_PPPOE,
    eNxNet_INTERFACE_TYPE_MAX,
} NX_NET_NetIfType_e;

typedef	struct
{
    NX_NET_NetIfType_e   eType;
    HBOOL                bConnected;
    HBOOL                bNetActive;
    HBOOL                bRunning;
    HCHAR                szMacAddr[NX_NET_MAX_MACADDR];
    HCHAR                szDev_name[16];
  	HCHAR 	szFwVer[NX_NET_FW_VERSION_LEN_MAX];
} NX_NET_NetDevInfo_t;

typedef struct {
	HBOOL	bDhcpOn;
	HBOOL	bDnsAuto;

	HULONG 	ulIpAddr;
	HULONG 	ulNetmask;
	HULONG 	ulGateway;
	HULONG 	ulDns1;
	HULONG 	ulDns2;

	HBOOL	bIpv6;                  /* whether using ipv6 address or not */
	HBOOL	bEnable;                /* Link is connected but IP address is not allocated. */
	HUINT32 bIpv6Mode;              /* whether using dhcp client or not */
	HBOOL	bDns6;                  /* whether static dns or not */
	HCHAR	szIp6_addr[NX_NET_MAX_IP6ADDR];          /* ipv6 address */
	HCHAR	szIp6_gateway[NX_NET_MAX_IP6ADDR];       /* ipv6 gateway address */
	HCHAR	szIp6_dns_addr_1[NX_NET_MAX_IP6ADDR];    /* ipv6 1st dns address */
	HCHAR	szIp6_dns_addr_2[NX_NET_MAX_IP6ADDR];    /* ipv6 2nd dns address */
	HUINT32	nPrefix;                /* ipv6 prefix */
	HULONG	ulTunnelStartAddr;      /* tunnel start ipv4 address ipv6 in ipv4 */
	HULONG	ulTunnelEndAddr;        /* tunnel end ipv4 address ipv6 in ipv4 */
	HBOOL	bActive;				/* IP Address is allocated */
} NX_NET_NetIFADDR_t;

/*******************************************************************/
/********************	Global Variables		********************/
/*******************************************************************/


/*******************************************************************/
/********************	global function prototype   ****************/
/*******************************************************************/

HERROR  NX_PORT_NETWORK_GetNumOfNetDev(HINT32 *pnCount);
HERROR  NX_PORT_NETWORK_GetDevInfo(HINT32 nIndex, NX_NET_NetDevInfo_t *pstDevInfo);
HERROR  NX_PORT_NETWORK_GetMacAddr(HINT32 nIndex, HCHAR *pszMacAddr, HINT32 nBufSize);
HERROR  NX_PORT_NETWORK_GetBluetoothMacAddr(HCHAR *szMacAddr);

HERROR  NX_PORT_NETWORK_GetAddrInfo(HINT32 nIndex,NX_NET_NetIFADDR_t *pstNetAddr);
HERROR  NX_PORT_NETWORK_SetAddrInfo(HINT32 nIndex,NX_NET_NetIFADDR_t *pstNetAddr);
void	NX_PORT_NETWORK_RegisterNotifier(void);
void    NX_PORT_NETWORK_UnRegisterNotifier(void);
HERROR  NX_PORT_NETWORK_StartScan(HINT32 nIndex);
HERROR  NX_PORT_NETWORK_ConnectAP( HINT32 nIndex, APKS_NETAP_Info_t *pstApInfo, HCHAR *szKey );
HERROR  NX_PORT_NETWORK_CheckGateway(HINT32 nIndex);
HERROR  NX_PORT_NETWORK_CheckDNS(HINT32 nIndex);

#endif // __NX_PORT_NETWORK_H__


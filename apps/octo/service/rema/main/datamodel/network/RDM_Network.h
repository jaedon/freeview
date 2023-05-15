/*******************************************************************
	File Description
********************************************************************/
/**
	@file	  re_init.h
	@brief

	Description:  									\n
	Module: HOMMA / API								\n

	Copyright (c) 2013 HUMAX Co., Ltd.				\n
	All rights reserved.							\n
*/

/*
* Character encoding.
*
* MS949
*
* This source file that uses MS949 encoding.
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



#ifndef	__REMA_DM_NETWORK_H__
#define	__REMA_DM_NETWORK_H__

/*******************************************************************/
/********************      Header Files    *************************/
/*******************************************************************/
#include <rema_int.h>

#ifdef __cplusplus
extern "C"
{
#endif

#define RDM_NETWORK_MAX_IP6ADDR	(40)
#define RDM_NETWORK_MAX_IP4ADDR	(16)
#define RDM_NETWORK_MAX_MACADDR	(20)

typedef struct {
	HBOOL	bDhcpOn;
	HBOOL	bDnsAuto;

	HULONG 	ulIpAddr;
	HULONG 	ulNetmask;
	HULONG 	ulGateway;
	HULONG 	ulDns1;
	HULONG 	ulDns2;

	HBOOL	bIpv6;                  /* whether using ipv6 address or not */
	HBOOL	bEnable;                /* temp */
	HUINT32 bIpv6Mode;              /* whether using dhcp client or not */
	HBOOL	bDns6;                  /* whether static dns or not */
	HCHAR	szIp6_addr[RDM_NETWORK_MAX_IP6ADDR];          /* ipv6 address */
	HCHAR	szIp6_gateway[RDM_NETWORK_MAX_IP6ADDR];       /* ipv6 gateway address */
	HCHAR	szIp6_dns_addr_1[RDM_NETWORK_MAX_IP6ADDR];    /* ipv6 1st dns address */
	HCHAR	szIp6_dns_addr_2[RDM_NETWORK_MAX_IP6ADDR];    /* ipv6 2nd dns address */
	HUINT32	nPrefix;                /* ipv6 prefix */
	HULONG	ulTunnelStartAddr;      /* tunnel start ipv4 address ipv6 in ipv4 */
	HULONG	ulTunnelEndAddr;        /* tunnel end ipv4 address ipv6 in ipv4 */
} RDM_NETIFADDR_t;


HERROR		RDM_NETWORK_CONF_IsSupportNetwork(HBOOL *bIsSupport );
HERROR		RDM_NETWORK_CONF_GetDefaultDev(HINT32 *pnIndex );
HERROR		RDM_NETWORK_CONF_SetAddrInfo( HINT32 nIndex, RDM_NETIFADDR_t* pstNetAddr  );


#ifdef __cplusplus
}
#endif

#endif

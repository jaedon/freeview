
/*******************************************************************/
/************************* File Description ************************/
/*******************************************************************/
// File Name:
// Original Author: Full Name!!
// File Description:
// Remarks:

/*******************************************************************/
/* Copyright (c) 2006 HUMAX Co., Ltd.                              */
/* All rights reserved.                                            */
/*******************************************************************/

#ifndef __DI_ETH_H__
#define __DI_ETH_H__

#ifdef __cplusplus
extern "C"
{
#endif

/*******************************************************************/
/**************************** Header Files**************************/
/*******************************************************************/
// Start Including Header Files
#include "di_err.h"

// End Including Headers

/*******************************************************************/
/************************ Extern variables *************************/
/*******************************************************************/
// Start Extern variablee

// End Extern variable


/*******************************************************************/
/************************ Macro Definition *************************/
/*******************************************************************/
// Start Macro definition
#define MAC_ADDR_LENGTH	6
#define ETH_ADDR_LENGTH		4
#define MY_PROTO 0x2020
#define ETH_LEN 6
#define ARP_REQUEST 0x0001
#define ARP_REPLAY 0x0002
#define ARP 0x0806
#define ETHERNET 0x0001
// End Macro definition


/*******************************************************************/
/****************************** typedef ****************************/
/*******************************************************************/
//20090517 :: POOH :: when you are going to manual setting, you must contain ip,broadcast,netmask address.
//				      gateway and dns address is option that only use local network .

// Start typedef
typedef struct
{
	int 			isDhcp;
	unsigned char	mac_addr[MAC_ADDR_LENGTH];
	unsigned char	ip_addr[ETH_ADDR_LENGTH];
	unsigned char	brd_addr[ETH_ADDR_LENGTH];
	unsigned char	net_addr[ETH_ADDR_LENGTH];
	unsigned char	gate_addr[ETH_ADDR_LENGTH];
	unsigned char	dns_addr[ETH_ADDR_LENGTH];
	int			mtu;
}DI_ETH_NETWORK_INFO;

#pragma pack(1)
typedef struct
{
	unsigned char dhost[ETH_LEN];
	unsigned char shost[ETH_LEN];
	unsigned short type;
} etherhdr_t;

typedef struct
{
	unsigned short hdr;
	unsigned short proto;
	unsigned char hlen;
	unsigned char plen;
	unsigned short op;
	unsigned char sha[ETH_LEN];
	unsigned int sip;
	unsigned char dha[ETH_LEN];
	unsigned int dip;
} arphdr_t;

typedef struct arp_packet
{
	etherhdr_t	ethhdr;
	arphdr_t	arphdr;
} arp_packet_t;
#pragma pack()


/*******************************************************************/
/******************** global function prototype ********************/
/*******************************************************************/
// Start global function prototypes

DI_ERR_CODE DI_ETH_Network_Up(int ethnum, DI_ETH_NETWORK_INFO* pNetworkInfo);
DI_ERR_CODE DI_ETH_Network_Down(int ethnum);

DI_ERR_CODE DI_ETH_Network_GetInfo(int ethnum, DI_ETH_NETWORK_INFO* pNetworkInfo);

DI_ERR_CODE DI_ETH_LinkState(int ethnum, int *pnLinkState);
DI_ERR_CODE DI_ETH_arp(int ethnum, char* strIP, int intIP, unsigned char* aucMAC, int waitSecond, int *retValue);

DI_ERR_CODE DI_ETH_InsertModule(void);
DI_ERR_CODE DI_ETH_RemoveModule(void);
DI_ERR_CODE DI_ETH_SetWakeOnLAN (HBOOL bOn_Off);

// End global function prototypes

#ifdef __cplusplus
}
#endif

#endif /* __DI_ETH_H__ */



/* $Header: $ */
/******************************************************************************
* $Workfile:$
* $Modtime:$
* Author :
* description :		 Factory Error Code Definition
*
* Copyright (c) 2008 by Humax Co., Ltd.
* All right reserved
******************************************************************************/
#ifndef _FDI_ETHERNET_H_
#define _FDI_ETHERNET_H_

/******************************************************************************
* header and definitions
******************************************************************************/
#include "fdi_err.h"

/******************************************************************************
* Definitions, typdef & const data
******************************************************************************/
#define FDI_IF_NAME_SIZE		16
#define FDI_MAC_ADDR_LEN		6
#define FDI_IPV4_ADDR_LEN		4
#define FDI_IPV6_ADDR_LEN		16

#define MAC_ADDR_LENGTH		6
#define IPV4_ADDR_LENGTH		4

#define MY_PROTO 0x2020
#define ETH_LEN 6
#define ARP_REQUEST 0x0001
#define ARP_REPLAY 0x0002
#define ARP 0x0806
#define ETHERNET 0x0001

typedef enum
{
	FDI_ETHERNET_DEV_LAN = 0, 
	FDI_ETHERNET_DEV_WAN, 
	FDI_ETHERNET_DEV_CABLEMODEM,
	FDI_ETHERNET_DEV_WIFI,
	FDI_ETHERNET_DEV_MAX
} FDI_ETHERNET_DEV_TYPE_e;

typedef struct
{
	unsigned char aucIpAddr[FDI_IPV4_ADDR_LEN];
	unsigned char aucBcastAddr[FDI_IPV4_ADDR_LEN];
	unsigned char aucMaskAddr[FDI_IPV4_ADDR_LEN];
} FDI_ETHERNET_IPV4_ADDR_t;

typedef struct
{
	unsigned char szName[FDI_IF_NAME_SIZE];
	FDI_ETHERNET_DEV_TYPE_e eDevType;
	unsigned char aucMacAddr[FDI_MAC_ADDR_LEN];
	BOOL bIsUp;
	BOOL bIsConnected;
	BOOL bIsDynamic;
	FDI_ETHERNET_IPV4_ADDR_t stIpv4Addr;
	unsigned char aucGwAddr[FDI_IPV4_ADDR_LEN];
} FDI_ETHERNET_INFO_t;

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

#define FDI_ETHERNET_FACTORY_DEV	0

/******************************************************************************
* Global variables and structures
******************************************************************************/
/* global variables and structures */

/******************************************************************************
* External variables and functions
******************************************************************************/
/* external variables  and functions */

/******************************************************************************
* Static variables and structures
******************************************************************************/
/* static variables and structures */

/******************************************************************************
* Static function prototypes
******************************************************************************/
/* static function prototypes */

/*******************************************************************************
* function : 
* description :
* input :
* output :
* return :
* 기타 참고자료 및 파일
*******************************************************************************/
FDI_ERR_CODE FDI_ETHERNET_GetDevCount(unsigned long *pulCount);

/*******************************************************************************
* function : 
* description :
* input :
* output :
* return :
* 기타 참고자료 및 파일
*******************************************************************************/
FDI_ERR_CODE FDI_ETHERNET_Open(unsigned long ulDevId);

/*******************************************************************************
* function : 
* description :
* input :
* output :
* return :
* 기타 참고자료 및 파일
*******************************************************************************/
FDI_ERR_CODE FDI_ETHERNET_ARPTest(unsigned long ulDevId, char *szIpAddr, unsigned char* pucMacAddr, int waitSecond);

/*******************************************************************************
* function : 
* description :
* input :
* output :
* return :
* 기타 참고자료 및 파일
*******************************************************************************/
BOOL FDI_ETHERNET_IsLinked(unsigned long ulDevId);

/*******************************************************************************
* function : 
* description :
* input :
* output :
* return :
* 기타 참고자료 및 파일
*******************************************************************************/
FDI_ERR_CODE FDI_ETHERNET_GetInfo(unsigned long ulDevId, FDI_ETHERNET_INFO_t *pEthInfo);

/*******************************************************************************
* function : 
* description :
* input :
* output :
* return :
* 기타 참고자료 및 파일
*******************************************************************************/
FDI_ERR_CODE FDI_ETHERNET_SetInfo(unsigned long ulDevId, FDI_ETHERNET_INFO_t *pEthInfo);

#endif


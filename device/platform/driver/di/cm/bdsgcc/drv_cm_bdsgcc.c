/* $Header: */
/*******************************************************************/
/************************* File Description ************************/
/*******************************************************************/
// File Name: drv_cm.c
// Original Author:
// File Description:
// Remarks:

/*******************************************************************/
/* Copyright (c) 2008 HUMAX Co., Ltd.								*/
/* All rights reserved. 												*/
/*******************************************************************/

/*******************************************************************/
/**************************** Header Files**************************/
/*******************************************************************/
#include "htype.h"
#include "vkernel.h"
#include "taskdef.h"

#if defined(CONFIG_VK_STDLIB)
#else
#include 	<stdio.h>
#include 	<stdlib.h>
#include 	<string.h>
#endif
#include <sys/ioctl.h>
#include <netinet/in.h>	/* sockaddr_in{} and other Internet defns */
#include <net/if.h>
#include <arpa/inet.h>	/* inet(3) functions */
#include <sys/socket.h>	/* basic socket definitions */
#include <errno.h>
#include <netdb.h>
#include <unistd.h>
#include <pthread.h>

/* di headers */
#include "di_channel_priv.h"
#include "drv_cm_bdsgcc.h"
#include "di_cm.h"
#include "drv_err.h"
#include "drv_gpio.h"
#include "di_snmp.h"
#include "drv_network.h"


/*******************************************************************/
/************************ Macro Definition *************************/
/*******************************************************************/
#define	SNMP_PORT	161	/* SNMP Port */

#define CMVERSIONLENGTH	11

#define CM_REBOOT_MONITOR	500
#define DRV_CM_TASK_PRIORITY		USER_PRIORITY5
#define DRV_CM_TASK_STACK_SIZE		BASIC_TASK_STACK_SIZE

#define INET_ADDR_STRLEN	15


/*******************************************************************/
/****************************** typedef ****************************/
/*******************************************************************/


/*******************************************************************/
/************************ Extern variables *************************/
/*******************************************************************/
extern int show_di_cm;
extern int g_nCmRebootNotify;

extern unsigned char TransId;
extern pthread_mutex_t LnxDsgApiMutex;
extern int EstbToEcmSocketSendRx( int tpkt_size, unsigned char *pBuf, bool waitreply, bool readBuffer );

extern DRV_NETWORK_DEV_INFO_t s_stDevList[];

extern DI_SNMP_Information_t g_stSnmpInfo;


/*******************************************************************/
/************************ Extern functions *************************/
/*******************************************************************/
extern int GetMac(int sockfd, const struct sockaddr_in *pservaddr, socklen_t servlen, char *reqMsg, int reqMsgLen, int offset, unsigned char mac[]);
extern unsigned long int GetCMIp(int sockfd, const struct sockaddr_in *pservaddr, socklen_t servlen);
extern int GetCmVer(int sockfd, const struct sockaddr_in *pservaddr, socklen_t servlen, char *reqMsg, int reqMsgLen, int offset, unsigned char output_string[], int *output_string_len);
extern int GetInt(int sockfd, const struct sockaddr_in *pservaddr, socklen_t servlen, char *reqMsg, int reqMsgLen, int offset);
extern unsigned int GetUInt(int sockfd, const struct sockaddr_in *pservaddr, socklen_t servlen, char *reqMsg, int reqMsgLen, int offset);
extern int GetString_P(int sockfd, const struct sockaddr_in *pservaddr, socklen_t servlen, char *reqMsg, int reqMsgLen, int offset, unsigned char output_string[], int *output_string_len);
extern int GetInt_P(int sockfd, const struct sockaddr_in *pservaddr, socklen_t servlen, char *reqMsg, int reqMsgLen, int offset);
extern int DsgSendScanDSFreq(unsigned int* pDsFreq);
extern unsigned int GetIpStackAddress(unsigned int* ipAddress, char * interfaceName);
extern void docsSwUgradebyCannedSNMP(void);
extern int DsgSendGetCmSavedTemp(int * pCmSavedTemp, int *pCurrentTemp);
extern int DsgSendGetCmMacAddress(unsigned char * pAddr);

extern int DsgSetDSGMode( int mode );
extern bool DsgScan ( bool enable );
extern int DsgSetFanControl( int mode );
extern int DsgSetDefaultTemp(int mode);
extern int DsgSetDefaultSnmp(int mode);
extern int DsgSetDefaultSavedFrq(int mode);
extern int DsgSetStandbyMode(int mode);
extern int DsgSendGetCmNetmask(unsigned int * pCmNetMask);
extern int DsgSendGetCmGateway( unsigned int *pCmGateway);

extern HUINT32 DRV_CFG_GetNetMaxDevice(void);




/******************************************************************************
 * Local(static) Variables
 ******************************************************************************/
char *psCmtargetAddr = {"192.168.100.1"};

static HBOOL s_bIsGetCmVer = FALSE;
static char s_aucCmVersion[CMVERSIONLENGTH]={""};

static unsigned char s_aucCmMacAddress[6]={""};
static HBOOL s_bIsGetMac = FALSE;
static unsigned int s_ulCmIpAddress=0;

static int s_nCmRebootNotifyDone;
static DRV_CM_CALLBACK s_pfCmCallback[DRV_CM_EVENT_END];

static HUINT8 s_ulSTBResetStatus = 0;
static HUINT8 s_ulFactoryResetStatus = 0;
static HUINT8 s_ulStbChangeState = 0;

static HINT32 s_nCmUpgradeOk = 0;

#if defined(CONFIG_PRODUCT_IMAGE_FACTORY)
/* 
cdMacAddress.1 (1.3.6.1.4.1.4413.2.99.1.1.2.1.4.1.2.1) 
SnmpRequestID : 0x43 0x65
*/
static unsigned char cdMacAddressGetMsg[] =
        {0x30, 0x31, 0x02, 0x01, 0x00, 0x04, 0x07, 0x70, 
         0x72, 0x69, 0x76, 0x61, 0x74, 0x65, 0xA0, 0x23,
         0x02, 0x02, 0x43, 0x65, 0x02, 0x01, 0x00, 0x02, 
         0x01, 0x00, 0x30, 0x17, 0x30, 0x15, 0x06, 0x11, 
         0x2B, 0x06, 0x01, 0x04, 0x01, 0xA2, 0x3D, 0x02, 
         0x63, 0x01, 0x01, 0x02, 0x01, 0x04, 0x01, 0x02, 
         0x01, 0x05, 0x00};

#define CDMACADDRESSGET_OFFSET 49
#else
/* 
ifPhysAddress (1.3.6.1.2.1.2.2.1.6.2) 
SnmpRequestID : 0x74 0xDB 0x92 0x68
*/
static unsigned char ifPhysAddressMsg[] =
        {0x30, 0x2B, 0x02, 0x01, 0x00, 0x04, 0x06, 0x70, 
         0x75, 0x62, 0x6C, 0x69, 0x63, 0xA0, 0x1E, 0x02, 
         0x04, 0x74, 0xDB, 0x92, 0x68, 0x02, 0x01, 0x00,
         0x02, 0x01, 0x00, 0x30, 0x10, 0x30, 0x0E, 0x06, 
         0x0A, 0x2B, 0x06, 0x01, 0x02, 0x01, 0x02, 0x02, 
         0x01, 0x06, 0x02, 0x05, 0x00};

#define  IFPHYSADDRESSMSG_OFFSET 43
#endif

/* 
docsDevSwCurrentVers (1.3.6.1.2.1.69.1.3.5.0) 
SnmpRequestID : 0x74 0xDB 0x92 0x6D
*/
static unsigned char docsDevSwCurrentVersMsg[] =
        {0x30, 0x2B, 0x02, 0x01, 0x00, 0x04, 0x06, 0x70,
         0x75, 0x62, 0x6C, 0x69, 0x63, 0xA0, 0x1E, 0x02,
         0x04, 0x74, 0xDB, 0x92, 0x6D, 0x02, 0x01, 0x00,
         0x02, 0x01, 0x00, 0x30, 0x10, 0x30, 0x0E, 0x06,
         0x0A, 0x2B, 0x06, 0x01, 0x02, 0x01, 0x45, 0x01,
         0x03, 0x05, 0x00, 0x05, 0x00};

#define  DOCSDEVSWCURRENTVERSMSG_OFFSET 43

/* 
docsDevSwOperStatus 1.3.6.1.2.1.69.1.3.4.0
SnmpRequestID : 0x74 0xDB 0x92 0x6E
*/
static unsigned char docsDevSwOperStatusMsg[] =
        {0x30, 0x2B, 0x02, 0x01, 0x00, 0x04, 0x06, 0x70,
         0x75, 0x62, 0x6C, 0x69, 0x63, 0xA0, 0x1E, 0x02,
         0x04, 0x74, 0xDB, 0x92, 0x6E, 0x02, 0x01, 0x00,
         0x02, 0x01, 0x00, 0x30, 0x10, 0x30, 0x0E, 0x06,
         0x0A, 0x2B, 0x06, 0x01, 0x02, 0x01, 0x45, 0x01,
         0x03, 0x04, 0x00, 0x05, 0x00};

#define  DOCSDEVSWOPERSTATUSMSG_OFFSET 43	/*Asn Int 02: 44th -1*/

/* 
docsDevSerialNumber (1.3.6.1.2.1.69.1.1.4.0) 
SnmpRequestID : 0x74, 0xDB, 0x92, 0x6F
*/
static unsigned char docsDevSerialNumberMsg[] =
        {0x30, 0x2B, 0x02, 0x01, 0x00, 0x04, 0x06, 0x70, 
         0x75, 0x62, 0x6C, 0x69, 0x63, 0xA0, 0x1E, 0x02, 
         0x04, 0x74, 0xDB, 0x92, 0x6F, 0x02, 0x01, 0x00,
         0x02, 0x01, 0x00, 0x30, 0x10, 0x30, 0x0E, 0x06, 
         0x0A, 0x2B, 0x06, 0x01, 0x02, 0x01, 0x45, 0x01, 
         0x01, 0x04, 0x00, 0x05, 0x00};

#define DOCSDEVSERIALNUMBERMSG_OFFSET 43

/* 
cdMacAddress.1 (1.3.6.1.4.1.4413.2.99.1.1.2.1.4.1.2.1) 
SnmpRequestID : 0x43 0x64
*/
static unsigned char cdMacAddressMsg[] =
        {0x30, 0x37, 0x02, 0x01, 0x00, 0x04, 0x07, 0x70,
         0x72, 0x69, 0x76, 0x61, 0x74, 0x65, 0xA3, 0x29,
         0x02, 0x02, 0x43, 0x64, 0x02, 0x01, 0x00, 0x02,
         0x01, 0x00, 0x30, 0x1D, 0x30, 0x1B, 0x06, 0x11,
         0x2B, 0x06, 0x01, 0x04, 0x01, 0xA2, 0x3D, 0x02,
         0x63, 0x01, 0x01, 0x02, 0x01, 0x04, 0x01, 0x02,
         0x01, 0x04, 0x06, 0x00, 0x03, 0x78, 0xFF, 0xFF,
         0xFF};  /* Value of 52th is Serial Num Index */

#define CDMACADDRESS_OFFSET 49 /*ASN Octet String 04: 50th -1 */

/* 
internet.mgmt.mib-2 (1.3.6.1.2.1.10.127.1.2.2.1.1,2) 
SnmpRequestID : 0x03 0xE7 0x00 0x01
*/
static unsigned char docsIfCmStatusValueMsg[] =
        {0x30, 0x2E, 0x02, 0x01, 0x00, 0x04, 0x06, 0x70,
         0x75, 0x62, 0x6C, 0x69, 0x63, 0xA0, 0x21, 0x02,
         0x04, 0x03, 0xE7, 0x00, 0x01, 0x02, 0x01, 0x00,
         0x02, 0x01, 0x00, 0x30, 0x13, 0x30, 0x11, 0x06,
         0x0D, 0x2B, 0x06, 0x01, 0x02, 0x01, 0x0A, 0x7F,
         0x01, 0x02, 0x02, 0x01, 0x01, 0x02, 0x05, 0x00};

#define  DOCSIFCMSTATUSVALUEMSG_OFFSET 46

/* 
docsIfDownstreamChannelFrequency - 1st (1.3.6.1.2.1.10.127.1.1.1.1.2.3)
SnmpRequestID : 0x74 0xDB 0x92 0x88
*/
static unsigned char docsIfDownChannelFrequency1stMsg[] =
        {0x30, 0x2E, 0x02, 0x01, 0x00, 0x04, 0x06, 0x70,
         0x75, 0x62, 0x6C, 0x69, 0x63, 0xA0, 0x21, 0x02,
         0x04, 0x74, 0xDB, 0x92, 0x88, 0x02, 0x01, 0x00,
         0x02, 0x01, 0x00, 0x30, 0x13, 0x30, 0x11, 0x06,
         0x0D, 0x2B, 0x06, 0x01, 0x02, 0x01, 0x0A, 0x7F,
         0x01, 0x01, 0x01, 0x01, 0x02, 0x03, 0x05, 0x00};


/* 
docsIfDownstreamChannelFrequency - 2nd (1.3.6.1.2.1.10.127.1.1.1.1.2.48)
SnmpRequestID : 0x74 0xDB 0x92 0x89
*/
static unsigned char docsIfDownChannelFrequency2ndMsg[] =
        {0x30, 0x2E, 0x02, 0x01, 0x00, 0x04, 0x06, 0x70,
         0x75, 0x62, 0x6C, 0x69, 0x63, 0xA0, 0x21, 0x02,
         0x04, 0x74, 0xDB, 0x92, 0x89, 0x02, 0x01, 0x00,
         0x02, 0x01, 0x00, 0x30, 0x13, 0x30, 0x11, 0x06,
         0x0D, 0x2B, 0x06, 0x01, 0x02, 0x01, 0x0A, 0x7F,
         0x01, 0x01, 0x01, 0x01, 0x02, 0x30, 0x05, 0x00};

/* 
docsIfDownstreamChannelFrequency - 3rd (1.3.6.1.2.1.10.127.1.1.1.1.2.49)
SnmpRequestID : 0x74 0xDB 0x92 0x8A
*/
static unsigned char docsIfDownChannelFrequency3rdMsg[] =
        {0x30, 0x2E, 0x02, 0x01, 0x00, 0x04, 0x06, 0x70,
         0x75, 0x62, 0x6C, 0x69, 0x63, 0xA0, 0x21, 0x02,
         0x04, 0x74, 0xDB, 0x92, 0x8A, 0x02, 0x01, 0x00,
         0x02, 0x01, 0x00, 0x30, 0x13, 0x30, 0x11, 0x06,
         0x0D, 0x2B, 0x06, 0x01, 0x02, 0x01, 0x0A, 0x7F,
         0x01, 0x01, 0x01, 0x01, 0x02, 0x31, 0x05, 0x00};

/* 
docsIfDownstreamChannelFrequency - 4th (1.3.6.1.2.1.10.127.1.1.1.1.2.50)
SnmpRequestID : 0x74 0xDB 0x92 0x8B
*/
static unsigned char docsIfDownChannelFrequency4thMsg[] =
        {0x30, 0x2E, 0x02, 0x01, 0x00, 0x04, 0x06, 0x70,
         0x75, 0x62, 0x6C, 0x69, 0x63, 0xA0, 0x21, 0x02,
         0x04, 0x74, 0xDB, 0x92, 0x8B, 0x02, 0x01, 0x00,
         0x02, 0x01, 0x00, 0x30, 0x13, 0x30, 0x11, 0x06,
         0x0D, 0x2B, 0x06, 0x01, 0x02, 0x01, 0x0A, 0x7F,
         0x01, 0x01, 0x01, 0x01, 0x02, 0x32, 0x05, 0x00};

/* 
docsIfDownstreamChannelFrequency - 5th (1.3.6.1.2.1.10.127.1.1.1.1.2.51)
SnmpRequestID : 0x74 0xDB 0x92 0x8C
*/
static unsigned char docsIfDownChannelFrequency5thMsg[] =
        {0x30, 0x2E, 0x02, 0x01, 0x00, 0x04, 0x06, 0x70,
         0x75, 0x62, 0x6C, 0x69, 0x63, 0xA0, 0x21, 0x02,
         0x04, 0x74, 0xDB, 0x92, 0x8C, 0x02, 0x01, 0x00,
         0x02, 0x01, 0x00, 0x30, 0x13, 0x30, 0x11, 0x06,
         0x0D, 0x2B, 0x06, 0x01, 0x02, 0x01, 0x0A, 0x7F,
         0x01, 0x01, 0x01, 0x01, 0x02, 0x33, 0x05, 0x00};

/* 
docsIfDownstreamChannelFrequency - 6th (1.3.6.1.2.1.10.127.1.1.1.1.2.52)
SnmpRequestID : 0x74 0xDB 0x92 0x8D
*/
static unsigned char docsIfDownChannelFrequency6thMsg[] =
        {0x30, 0x2E, 0x02, 0x01, 0x00, 0x04, 0x06, 0x70,
         0x75, 0x62, 0x6C, 0x69, 0x63, 0xA0, 0x21, 0x02,
         0x04, 0x74, 0xDB, 0x92, 0x8D, 0x02, 0x01, 0x00,
         0x02, 0x01, 0x00, 0x30, 0x13, 0x30, 0x11, 0x06,
         0x0D, 0x2B, 0x06, 0x01, 0x02, 0x01, 0x0A, 0x7F,
         0x01, 0x01, 0x01, 0x01, 0x02, 0x34, 0x05, 0x00};

/* 
docsIfDownstreamChannelFrequency - 7th (1.3.6.1.2.1.10.127.1.1.1.1.2.53)
SnmpRequestID : 0x74 0xDB 0x92 0x8E
*/
static unsigned char docsIfDownChannelFrequency7thMsg[] =
        {0x30, 0x2E, 0x02, 0x01, 0x00, 0x04, 0x06, 0x70,
         0x75, 0x62, 0x6C, 0x69, 0x63, 0xA0, 0x21, 0x02,
         0x04, 0x74, 0xDB, 0x92, 0x8E, 0x02, 0x01, 0x00,
         0x02, 0x01, 0x00, 0x30, 0x13, 0x30, 0x11, 0x06,
         0x0D, 0x2B, 0x06, 0x01, 0x02, 0x01, 0x0A, 0x7F,
         0x01, 0x01, 0x01, 0x01, 0x02, 0x35, 0x05, 0x00};

/* 
docsIfDownstreamChannelFrequency - 8th (1.3.6.1.2.1.10.127.1.1.1.1.2.54)
SnmpRequestID : 0x74 0xDB 0x92 0x8F
*/
static unsigned char docsIfDownChannelFrequency8thMsg[] =
        {0x30, 0x2E, 0x02, 0x01, 0x00, 0x04, 0x06, 0x70,
         0x75, 0x62, 0x6C, 0x69, 0x63, 0xA0, 0x21, 0x02,
         0x04, 0x74, 0xDB, 0x92, 0x8F, 0x02, 0x01, 0x00,
         0x02, 0x01, 0x00, 0x30, 0x13, 0x30, 0x11, 0x06,
         0x0D, 0x2B, 0x06, 0x01, 0x02, 0x01, 0x0A, 0x7F,
         0x01, 0x01, 0x01, 0x01, 0x02, 0x36, 0x05, 0x00};

#define  DOCSIFDOWNCHANNELFREQUENCY30MSG_OFFSET 46

/* 
docsIfDownChannelPower - 1st (1.3.6.1.2.1.10.127.1.1.1.1.6.3)
SnmpRequestID : 0x74 0xDB 0x92 0x98
*/
static unsigned char docsIfDownChannelPower1stMsg[] =
        {0x30, 0x2E, 0x02, 0x01, 0x00, 0x04, 0x06, 0x70,
         0x75, 0x62, 0x6C, 0x69, 0x63, 0xA0, 0x21, 0x02,
         0x04, 0x74, 0xDB, 0x92, 0x98, 0x02, 0x01, 0x00,
         0x02, 0x01, 0x00, 0x30, 0x13, 0x30, 0x11, 0x06,
         0x0D, 0x2B, 0x06, 0x01, 0x02, 0x01, 0x0A, 0x7F,
         0x01, 0x01, 0x01, 0x01, 0x06, 0x03, 0x05, 0x00};

/* 
docsIfDownChannelPower - 2nd (1.3.6.1.2.1.10.127.1.1.1.1.6.48)
SnmpRequestID : 0x74 0xDB 0x92 0x99
*/
static unsigned char docsIfDownChannelPower2ndMsg[] =
        {0x30, 0x2E, 0x02, 0x01, 0x00, 0x04, 0x06, 0x70,
         0x75, 0x62, 0x6C, 0x69, 0x63, 0xA0, 0x21, 0x02,
         0x04, 0x74, 0xDB, 0x92, 0x99, 0x02, 0x01, 0x00,
         0x02, 0x01, 0x00, 0x30, 0x13, 0x30, 0x11, 0x06,
         0x0D, 0x2B, 0x06, 0x01, 0x02, 0x01, 0x0A, 0x7F,
         0x01, 0x01, 0x01, 0x01, 0x06, 0x30, 0x05, 0x00};

/* 
docsIfDownChannelPower - 3rd (1.3.6.1.2.1.10.127.1.1.1.1.6.49)
SnmpRequestID : 0x74 0xDB 0x92 0x9A
*/
static unsigned char docsIfDownChannelPower3rdMsg[] =
        {0x30, 0x2E, 0x02, 0x01, 0x00, 0x04, 0x06, 0x70,
         0x75, 0x62, 0x6C, 0x69, 0x63, 0xA0, 0x21, 0x02,
         0x04, 0x74, 0xDB, 0x92, 0x9A, 0x02, 0x01, 0x00,
         0x02, 0x01, 0x00, 0x30, 0x13, 0x30, 0x11, 0x06,
         0x0D, 0x2B, 0x06, 0x01, 0x02, 0x01, 0x0A, 0x7F,
         0x01, 0x01, 0x01, 0x01, 0x06, 0x31, 0x05, 0x00};

/* 
docsIfDownChannelPower - 4th (1.3.6.1.2.1.10.127.1.1.1.1.6.50)
SnmpRequestID : 0x74 0xDB 0x92 0x9B
*/
static unsigned char docsIfDownChannelPower4thMsg[] =
        {0x30, 0x2E, 0x02, 0x01, 0x00, 0x04, 0x06, 0x70,
         0x75, 0x62, 0x6C, 0x69, 0x63, 0xA0, 0x21, 0x02,
         0x04, 0x74, 0xDB, 0x92, 0x9B, 0x02, 0x01, 0x00,
         0x02, 0x01, 0x00, 0x30, 0x13, 0x30, 0x11, 0x06,
         0x0D, 0x2B, 0x06, 0x01, 0x02, 0x01, 0x0A, 0x7F,
         0x01, 0x01, 0x01, 0x01, 0x06, 0x32, 0x05, 0x00};

/* 
docsIfDownChannelPower - 5th (1.3.6.1.2.1.10.127.1.1.1.1.6.51)
SnmpRequestID : 0x74 0xDB 0x92 0x9C
*/
static unsigned char docsIfDownChannelPower5thMsg[] =
        {0x30, 0x2E, 0x02, 0x01, 0x00, 0x04, 0x06, 0x70,
         0x75, 0x62, 0x6C, 0x69, 0x63, 0xA0, 0x21, 0x02,
         0x04, 0x74, 0xDB, 0x92, 0x9C, 0x02, 0x01, 0x00,
         0x02, 0x01, 0x00, 0x30, 0x13, 0x30, 0x11, 0x06,
         0x0D, 0x2B, 0x06, 0x01, 0x02, 0x01, 0x0A, 0x7F,
         0x01, 0x01, 0x01, 0x01, 0x06, 0x33, 0x05, 0x00};

/* 
docsIfDownChannelPower - 6th (1.3.6.1.2.1.10.127.1.1.1.1.6.52)
SnmpRequestID : 0x74 0xDB 0x92 0x9D
*/
static unsigned char docsIfDownChannelPower6thMsg[] =
        {0x30, 0x2E, 0x02, 0x01, 0x00, 0x04, 0x06, 0x70,
         0x75, 0x62, 0x6C, 0x69, 0x63, 0xA0, 0x21, 0x02,
         0x04, 0x74, 0xDB, 0x92, 0x9D, 0x02, 0x01, 0x00,
         0x02, 0x01, 0x00, 0x30, 0x13, 0x30, 0x11, 0x06,
         0x0D, 0x2B, 0x06, 0x01, 0x02, 0x01, 0x0A, 0x7F,
         0x01, 0x01, 0x01, 0x01, 0x06, 0x34, 0x05, 0x00};

/* 
docsIfDownChannelPower - 7th (1.3.6.1.2.1.10.127.1.1.1.1.6.53)
SnmpRequestID : 0x74 0xDB 0x92 0x9E
*/
static unsigned char docsIfDownChannelPower7thMsg[] =
        {0x30, 0x2E, 0x02, 0x01, 0x00, 0x04, 0x06, 0x70,
         0x75, 0x62, 0x6C, 0x69, 0x63, 0xA0, 0x21, 0x02,
         0x04, 0x74, 0xDB, 0x92, 0x9E, 0x02, 0x01, 0x00,
         0x02, 0x01, 0x00, 0x30, 0x13, 0x30, 0x11, 0x06,
         0x0D, 0x2B, 0x06, 0x01, 0x02, 0x01, 0x0A, 0x7F,
         0x01, 0x01, 0x01, 0x01, 0x06, 0x35, 0x05, 0x00};

/* 
docsIfDownChannelPower - 8th (1.3.6.1.2.1.10.127.1.1.1.1.6.54)
SnmpRequestID : 0x74 0xDB 0x92 0x9F
*/
static unsigned char docsIfDownChannelPower8thMsg[] =
        {0x30, 0x2E, 0x02, 0x01, 0x00, 0x04, 0x06, 0x70,
         0x75, 0x62, 0x6C, 0x69, 0x63, 0xA0, 0x21, 0x02,
         0x04, 0x74, 0xDB, 0x92, 0x9F, 0x02, 0x01, 0x00,
         0x02, 0x01, 0x00, 0x30, 0x13, 0x30, 0x11, 0x06,
         0x0D, 0x2B, 0x06, 0x01, 0x02, 0x01, 0x0A, 0x7F,
         0x01, 0x01, 0x01, 0x01, 0x06, 0x36, 0x05, 0x00};

#define  DOCSIFDOWNCHANNELPOWER30MSG_OFFSET 46

/* 
docsIfSigQSignalNoise - 1st (1.3.6.1.2.1.10.127.1.1.4.1.5.3)
SnmpRequestID : 0x74 0xDB 0x92 0xA0
*/
static unsigned char docsIfSigQSignalNoise1stMsg[] =
        {0x30, 0x2E, 0x02, 0x01, 0x00, 0x04, 0x06, 0x70,
         0x75, 0x62, 0x6C, 0x69, 0x63, 0xA0, 0x21, 0x02,
         0x04, 0x74, 0xDB, 0x92, 0xA0, 0x02, 0x01, 0x00,
         0x02, 0x01, 0x00, 0x30, 0x13, 0x30, 0x11, 0x06,
         0x0D, 0x2B, 0x06, 0x01, 0x02, 0x01, 0x0A, 0x7F,
         0x01, 0x01, 0x04, 0x01, 0x05, 0x03, 0x05, 0x00};

/* 
docsIfSigQSignalNoise - 2nd (1.3.6.1.2.1.10.127.1.1.4.1.5.48)
SnmpRequestID : 0x74 0xDB 0x92 0xA1
*/
static unsigned char docsIfSigQSignalNoise2ndMsg[] =
        {0x30, 0x2E, 0x02, 0x01, 0x00, 0x04, 0x06, 0x70,
         0x75, 0x62, 0x6C, 0x69, 0x63, 0xA0, 0x21, 0x02,
         0x04, 0x74, 0xDB, 0x92, 0xA1, 0x02, 0x01, 0x00,
         0x02, 0x01, 0x00, 0x30, 0x13, 0x30, 0x11, 0x06,
         0x0D, 0x2B, 0x06, 0x01, 0x02, 0x01, 0x0A, 0x7F,
         0x01, 0x01, 0x04, 0x01, 0x05, 0x30, 0x05, 0x00};

/* 
docsIfSigQSignalNoise - 3rd (1.3.6.1.2.1.10.127.1.1.4.1.5.49)
SnmpRequestID : 0x74 0xDB 0x92 0xA2
*/
static unsigned char docsIfSigQSignalNoise3rdMsg[] =
        {0x30, 0x2E, 0x02, 0x01, 0x00, 0x04, 0x06, 0x70,
         0x75, 0x62, 0x6C, 0x69, 0x63, 0xA0, 0x21, 0x02,
         0x04, 0x74, 0xDB, 0x92, 0xA2, 0x02, 0x01, 0x00,
         0x02, 0x01, 0x00, 0x30, 0x13, 0x30, 0x11, 0x06,
         0x0D, 0x2B, 0x06, 0x01, 0x02, 0x01, 0x0A, 0x7F,
         0x01, 0x01, 0x04, 0x01, 0x05, 0x31, 0x05, 0x00};

/* 
docsIfSigQSignalNoise - 4th (1.3.6.1.2.1.10.127.1.1.4.1.5.50)
SnmpRequestID : 0x74 0xDB 0x92 0xA3
*/
static unsigned char docsIfSigQSignalNoise4thMsg[] =
        {0x30, 0x2E, 0x02, 0x01, 0x00, 0x04, 0x06, 0x70,
         0x75, 0x62, 0x6C, 0x69, 0x63, 0xA0, 0x21, 0x02,
         0x04, 0x74, 0xDB, 0x92, 0xA3, 0x02, 0x01, 0x00,
         0x02, 0x01, 0x00, 0x30, 0x13, 0x30, 0x11, 0x06,
         0x0D, 0x2B, 0x06, 0x01, 0x02, 0x01, 0x0A, 0x7F,
         0x01, 0x01, 0x04, 0x01, 0x05, 0x32, 0x05, 0x00};

/* 
docsIfSigQSignalNoise - 5th (1.3.6.1.2.1.10.127.1.1.4.1.5.51)
SnmpRequestID : 0x74 0xDB 0x92 0xA4
*/
static unsigned char docsIfSigQSignalNoise5thMsg[] =
        {0x30, 0x2E, 0x02, 0x01, 0x00, 0x04, 0x06, 0x70,
         0x75, 0x62, 0x6C, 0x69, 0x63, 0xA0, 0x21, 0x02,
         0x04, 0x74, 0xDB, 0x92, 0xA4, 0x02, 0x01, 0x00,
         0x02, 0x01, 0x00, 0x30, 0x13, 0x30, 0x11, 0x06,
         0x0D, 0x2B, 0x06, 0x01, 0x02, 0x01, 0x0A, 0x7F,
         0x01, 0x01, 0x04, 0x01, 0x05, 0x33, 0x05, 0x00};

/* 
docsIfSigQSignalNoise - 6th (1.3.6.1.2.1.10.127.1.1.4.1.5.52)
SnmpRequestID : 0x74 0xDB 0x92 0xA5
*/
static unsigned char docsIfSigQSignalNoise6thMsg[] =
        {0x30, 0x2E, 0x02, 0x01, 0x00, 0x04, 0x06, 0x70,
         0x75, 0x62, 0x6C, 0x69, 0x63, 0xA0, 0x21, 0x02,
         0x04, 0x74, 0xDB, 0x92, 0xA5, 0x02, 0x01, 0x00,
         0x02, 0x01, 0x00, 0x30, 0x13, 0x30, 0x11, 0x06,
         0x0D, 0x2B, 0x06, 0x01, 0x02, 0x01, 0x0A, 0x7F,
         0x01, 0x01, 0x04, 0x01, 0x05, 0x34, 0x05, 0x00};

/* 
docsIfSigQSignalNoise - 7th (1.3.6.1.2.1.10.127.1.1.4.1.5.53)
SnmpRequestID : 0x74 0xDB 0x92 0xA6
*/
static unsigned char docsIfSigQSignalNoise7thMsg[] =
        {0x30, 0x2E, 0x02, 0x01, 0x00, 0x04, 0x06, 0x70,
         0x75, 0x62, 0x6C, 0x69, 0x63, 0xA0, 0x21, 0x02,
         0x04, 0x74, 0xDB, 0x92, 0xA6, 0x02, 0x01, 0x00,
         0x02, 0x01, 0x00, 0x30, 0x13, 0x30, 0x11, 0x06,
         0x0D, 0x2B, 0x06, 0x01, 0x02, 0x01, 0x0A, 0x7F,
         0x01, 0x01, 0x04, 0x01, 0x05, 0x35, 0x05, 0x00};

/* 
docsIfSigQSignalNoise - 8th (1.3.6.1.2.1.10.127.1.1.4.1.5.54)
SnmpRequestID : 0x74 0xDB 0x92 0xA7
*/
static unsigned char docsIfSigQSignalNoise8thMsg[] =
        {0x30, 0x2E, 0x02, 0x01, 0x00, 0x04, 0x06, 0x70,
         0x75, 0x62, 0x6C, 0x69, 0x63, 0xA0, 0x21, 0x02,
         0x04, 0x74, 0xDB, 0x92, 0xA7, 0x02, 0x01, 0x00,
         0x02, 0x01, 0x00, 0x30, 0x13, 0x30, 0x11, 0x06,
         0x0D, 0x2B, 0x06, 0x01, 0x02, 0x01, 0x0A, 0x7F,
         0x01, 0x01, 0x04, 0x01, 0x05, 0x36, 0x05, 0x00};

#define  DOCSIFSIGQSIGNALNOISE30MSG_OFFSET 46

/* 
docsIfDownstreamChannelId - 1st (1.3.6.1.2.1.10.127.1.1.1.1.1.3)
SnmpRequestID : 0x74 0xDB 0x92 0x80
*/
static unsigned char docsIfDownChannelId1stMsg[] =
        {0x30, 0x2E, 0x02, 0x01, 0x00, 0x04, 0x06, 0x70,
         0x75, 0x62, 0x6C, 0x69, 0x63, 0xA0, 0x21, 0x02,
         0x04, 0x74, 0xDB, 0x92, 0x80, 0x02, 0x01, 0x00,
         0x02, 0x01, 0x00, 0x30, 0x13, 0x30, 0x11, 0x06,
         0x0D, 0x2B, 0x06, 0x01, 0x02, 0x01, 0x0A, 0x7F,
         0x01, 0x01, 0x01, 0x01, 0x01, 0x03, 0x05, 0x00};

/* 
docsIfDownstreamChannelId - 2nd (1.3.6.1.2.1.10.127.1.1.1.1.1.48) 
SnmpRequestID : 0x74 0xDB 0x92 0x81
*/
static unsigned char docsIfDownChannelId2ndMsg[] =
        {0x30, 0x2E, 0x02, 0x01, 0x00, 0x04, 0x06, 0x70,
         0x75, 0x62, 0x6C, 0x69, 0x63, 0xA0, 0x21, 0x02,
         0x04, 0x74, 0xDB, 0x92, 0x81, 0x02, 0x01, 0x00,
         0x02, 0x01, 0x00, 0x30, 0x13, 0x30, 0x11, 0x06,
         0x0D, 0x2B, 0x06, 0x01, 0x02, 0x01, 0x0A, 0x7F,
         0x01, 0x01, 0x01, 0x01, 0x01, 0x30, 0x05, 0x00};

/* 
docsIfDownstreamChannelId - 3rd (1.3.6.1.2.1.10.127.1.1.1.1.1.49) 
SnmpRequestID : 0x74 0xDB 0x92 0x82
*/
static unsigned char docsIfDownChannelId3rdMsg[] =
        {0x30, 0x2E, 0x02, 0x01, 0x00, 0x04, 0x06, 0x70,
         0x75, 0x62, 0x6C, 0x69, 0x63, 0xA0, 0x21, 0x02,
         0x04, 0x74, 0xDB, 0x92, 0x82, 0x02, 0x01, 0x00,
         0x02, 0x01, 0x00, 0x30, 0x13, 0x30, 0x11, 0x06,
         0x0D, 0x2B, 0x06, 0x01, 0x02, 0x01, 0x0A, 0x7F,
         0x01, 0x01, 0x01, 0x01, 0x01, 0x31, 0x05, 0x00};

/* 
docsIfDownstreamChannelId - 4th (1.3.6.1.2.1.10.127.1.1.1.1.1.50) 
SnmpRequestID : 0x74 0xDB 0x92 0x83
*/
static unsigned char docsIfDownChannelId4thMsg[] =
        {0x30, 0x2E, 0x02, 0x01, 0x00, 0x04, 0x06, 0x70,
         0x75, 0x62, 0x6C, 0x69, 0x63, 0xA0, 0x21, 0x02,
         0x04, 0x74, 0xDB, 0x92, 0x83, 0x02, 0x01, 0x00,
         0x02, 0x01, 0x00, 0x30, 0x13, 0x30, 0x11, 0x06,
         0x0D, 0x2B, 0x06, 0x01, 0x02, 0x01, 0x0A, 0x7F,
         0x01, 0x01, 0x01, 0x01, 0x01, 0x32, 0x05, 0x00};

/* 
docsIfDownstreamChannelId - 5th (1.3.6.1.2.1.10.127.1.1.1.1.1.51) 
SnmpRequestID : 0x74 0xDB 0x92 0x84
*/
static unsigned char docsIfDownChannelId5thMsg[] =
        {0x30, 0x2E, 0x02, 0x01, 0x00, 0x04, 0x06, 0x70,
         0x75, 0x62, 0x6C, 0x69, 0x63, 0xA0, 0x21, 0x02,
         0x04, 0x74, 0xDB, 0x92, 0x84, 0x02, 0x01, 0x00,
         0x02, 0x01, 0x00, 0x30, 0x13, 0x30, 0x11, 0x06,
         0x0D, 0x2B, 0x06, 0x01, 0x02, 0x01, 0x0A, 0x7F,
         0x01, 0x01, 0x01, 0x01, 0x01, 0x33, 0x05, 0x00};

/* 
docsIfDownstreamChannelId - 6th (1.3.6.1.2.1.10.127.1.1.1.1.1.52) 
SnmpRequestID : 0x74 0xDB 0x92 0x85
*/
static unsigned char docsIfDownChannelId6thMsg[] =
        {0x30, 0x2E, 0x02, 0x01, 0x00, 0x04, 0x06, 0x70,
         0x75, 0x62, 0x6C, 0x69, 0x63, 0xA0, 0x21, 0x02,
         0x04, 0x74, 0xDB, 0x92, 0x85, 0x02, 0x01, 0x00,
         0x02, 0x01, 0x00, 0x30, 0x13, 0x30, 0x11, 0x06,
         0x0D, 0x2B, 0x06, 0x01, 0x02, 0x01, 0x0A, 0x7F,
         0x01, 0x01, 0x01, 0x01, 0x01, 0x34, 0x05, 0x00};

/* 
docsIfDownstreamChannelId - 7th (1.3.6.1.2.1.10.127.1.1.1.1.1.53) 
SnmpRequestID : 0x74 0xDB 0x92 0x86
*/
static unsigned char docsIfDownChannelId7thMsg[] =
        {0x30, 0x2E, 0x02, 0x01, 0x00, 0x04, 0x06, 0x70,
         0x75, 0x62, 0x6C, 0x69, 0x63, 0xA0, 0x21, 0x02,
         0x04, 0x74, 0xDB, 0x92, 0x86, 0x02, 0x01, 0x00,
         0x02, 0x01, 0x00, 0x30, 0x13, 0x30, 0x11, 0x06,
         0x0D, 0x2B, 0x06, 0x01, 0x02, 0x01, 0x0A, 0x7F,
         0x01, 0x01, 0x01, 0x01, 0x01, 0x35, 0x05, 0x00};

/* 
docsIfDownstreamChannelId - 8th (1.3.6.1.2.1.10.127.1.1.1.1.1.54) 
SnmpRequestID : 0x74 0xDB 0x92 0x87
*/
static unsigned char docsIfDownChannelId8thMsg[] =
        {0x30, 0x2E, 0x02, 0x01, 0x00, 0x04, 0x06, 0x70,
         0x75, 0x62, 0x6C, 0x69, 0x63, 0xA0, 0x21, 0x02,
         0x04, 0x74, 0xDB, 0x92, 0x87, 0x02, 0x01, 0x00,
         0x02, 0x01, 0x00, 0x30, 0x13, 0x30, 0x11, 0x06,
         0x0D, 0x2B, 0x06, 0x01, 0x02, 0x01, 0x0A, 0x7F,
         0x01, 0x01, 0x01, 0x01, 0x01, 0x36, 0x05, 0x00};

#define  DOCSIFDOWNCHANNELID30MSG_OFFSET 46

/* 
docsIfDownstreamChannelModulation - 1st (1.3.6.1.2.1.10.127.1.1.1.1.4.3)
SnmpRequestID : 0x74 0xDB 0x92 0x90
*/
static unsigned char docsIfDownChannelModulation1stMsg[] =
        {0x30, 0x2E, 0x02, 0x01, 0x00, 0x04, 0x06, 0x70,
         0x75, 0x62, 0x6C, 0x69, 0x63, 0xA0, 0x21, 0x02,
         0x04, 0x74, 0xDB, 0x92, 0x90, 0x02, 0x01, 0x00,
         0x02, 0x01, 0x00, 0x30, 0x13, 0x30, 0x11, 0x06,
         0x0D, 0x2B, 0x06, 0x01, 0x02, 0x01, 0x0A, 0x7F,
         0x01, 0x01, 0x01, 0x01, 0x04, 0x03, 0x05, 0x00};

/* 
docsIfDownstreamChannelModulation - 2nd (1.3.6.1.2.1.10.127.1.1.1.1.4.48)
SnmpRequestID : 0x74 0xDB 0x92 0x91
*/
static unsigned char docsIfDownChannelModulation2ndMsg[] =
        {0x30, 0x2E, 0x02, 0x01, 0x00, 0x04, 0x06, 0x70,
         0x75, 0x62, 0x6C, 0x69, 0x63, 0xA0, 0x21, 0x02,
         0x04, 0x74, 0xDB, 0x92, 0x91, 0x02, 0x01, 0x00,
         0x02, 0x01, 0x00, 0x30, 0x13, 0x30, 0x11, 0x06,
         0x0D, 0x2B, 0x06, 0x01, 0x02, 0x01, 0x0A, 0x7F,
         0x01, 0x01, 0x01, 0x01, 0x04, 0x30, 0x05, 0x00};

/* 
docsIfDownstreamChannelModulation - 3rd (1.3.6.1.2.1.10.127.1.1.1.1.4.49)
SnmpRequestID : 0x74 0xDB 0x92 0x92
*/
static unsigned char docsIfDownChannelModulation3rdMsg[] =
        {0x30, 0x2E, 0x02, 0x01, 0x00, 0x04, 0x06, 0x70,
         0x75, 0x62, 0x6C, 0x69, 0x63, 0xA0, 0x21, 0x02,
         0x04, 0x74, 0xDB, 0x92, 0x92, 0x02, 0x01, 0x00,
         0x02, 0x01, 0x00, 0x30, 0x13, 0x30, 0x11, 0x06,
         0x0D, 0x2B, 0x06, 0x01, 0x02, 0x01, 0x0A, 0x7F,
         0x01, 0x01, 0x01, 0x01, 0x04, 0x31, 0x05, 0x00};

/* 
docsIfDownstreamChannelModulation - 4th (1.3.6.1.2.1.10.127.1.1.1.1.4.50)
SnmpRequestID : 0x74 0xDB 0x92 0x93
*/
static unsigned char docsIfDownChannelModulation4thMsg[] =
        {0x30, 0x2E, 0x02, 0x01, 0x00, 0x04, 0x06, 0x70,
         0x75, 0x62, 0x6C, 0x69, 0x63, 0xA0, 0x21, 0x02,
         0x04, 0x74, 0xDB, 0x92, 0x93, 0x02, 0x01, 0x00,
         0x02, 0x01, 0x00, 0x30, 0x13, 0x30, 0x11, 0x06,
         0x0D, 0x2B, 0x06, 0x01, 0x02, 0x01, 0x0A, 0x7F,
         0x01, 0x01, 0x01, 0x01, 0x04, 0x32, 0x05, 0x00};

/* 
docsIfDownstreamChannelModulation - 5th (1.3.6.1.2.1.10.127.1.1.1.1.4.51)
SnmpRequestID : 0x74 0xDB 0x92 0x94
*/
static unsigned char docsIfDownChannelModulation5thMsg[] =
        {0x30, 0x2E, 0x02, 0x01, 0x00, 0x04, 0x06, 0x70,
         0x75, 0x62, 0x6C, 0x69, 0x63, 0xA0, 0x21, 0x02,
         0x04, 0x74, 0xDB, 0x92, 0x94, 0x02, 0x01, 0x00,
         0x02, 0x01, 0x00, 0x30, 0x13, 0x30, 0x11, 0x06,
         0x0D, 0x2B, 0x06, 0x01, 0x02, 0x01, 0x0A, 0x7F,
         0x01, 0x01, 0x01, 0x01, 0x04, 0x33, 0x05, 0x00};

/* 
docsIfDownstreamChannelModulation - 6th (1.3.6.1.2.1.10.127.1.1.1.1.4.52)
SnmpRequestID : 0x74 0xDB 0x92 0x95
*/
static unsigned char docsIfDownChannelModulation6thMsg[] =
        {0x30, 0x2E, 0x02, 0x01, 0x00, 0x04, 0x06, 0x70,
         0x75, 0x62, 0x6C, 0x69, 0x63, 0xA0, 0x21, 0x02,
         0x04, 0x74, 0xDB, 0x92, 0x95, 0x02, 0x01, 0x00,
         0x02, 0x01, 0x00, 0x30, 0x13, 0x30, 0x11, 0x06,
         0x0D, 0x2B, 0x06, 0x01, 0x02, 0x01, 0x0A, 0x7F,
         0x01, 0x01, 0x01, 0x01, 0x04, 0x34, 0x05, 0x00};

/* 
docsIfDownstreamChannelModulation - 7th (1.3.6.1.2.1.10.127.1.1.1.1.4.53)
SnmpRequestID : 0x74 0xDB 0x92 0x96
*/
static unsigned char docsIfDownChannelModulation7thMsg[] =
        {0x30, 0x2E, 0x02, 0x01, 0x00, 0x04, 0x06, 0x70,
         0x75, 0x62, 0x6C, 0x69, 0x63, 0xA0, 0x21, 0x02,
         0x04, 0x74, 0xDB, 0x92, 0x96, 0x02, 0x01, 0x00,
         0x02, 0x01, 0x00, 0x30, 0x13, 0x30, 0x11, 0x06,
         0x0D, 0x2B, 0x06, 0x01, 0x02, 0x01, 0x0A, 0x7F,
         0x01, 0x01, 0x01, 0x01, 0x04, 0x35, 0x05, 0x00};

/* 
docsIfDownstreamChannelModulation - 8th (1.3.6.1.2.1.10.127.1.1.1.1.4.54)
SnmpRequestID : 0x74 0xDB 0x92 0x97
*/
static unsigned char docsIfDownChannelModulation8thMsg[] =
        {0x30, 0x2E, 0x02, 0x01, 0x00, 0x04, 0x06, 0x70,
         0x75, 0x62, 0x6C, 0x69, 0x63, 0xA0, 0x21, 0x02,
         0x04, 0x74, 0xDB, 0x92, 0x97, 0x02, 0x01, 0x00,
         0x02, 0x01, 0x00, 0x30, 0x13, 0x30, 0x11, 0x06,
         0x0D, 0x2B, 0x06, 0x01, 0x02, 0x01, 0x0A, 0x7F,
         0x01, 0x01, 0x01, 0x01, 0x04, 0x36, 0x05, 0x00};

#define  DOCSIFDOWNCHANNELMODULATION30MSG_OFFSET 46

/* 
docsIfSigQCorrecteds - 1st (1.3.6.1.2.1.10.127.1.1.4.1.3.3) 
SnmpRequestID : 0x74 0xDB 0x92 0xA8
*/
static unsigned char docsIfSigQCorrecteds1stMsg[] =
        {0x30, 0x2E, 0x02, 0x01, 0x00, 0x04, 0x06, 0x70,
         0x75, 0x62, 0x6C, 0x69, 0x63, 0xA0, 0x21, 0x02,
         0x04, 0x74, 0xDB, 0x92, 0xA8, 0x02, 0x01, 0x00,
         0x02, 0x01, 0x00, 0x30, 0x13, 0x30, 0x11, 0x06,
         0x0D, 0x2B, 0x06, 0x01, 0x02, 0x01, 0x0A, 0x7F,
         0x01, 0x01, 0x04, 0x01, 0x03, 0x03, 0x05, 0x00};

/* 
docsIfSigQCorrecteds - 2nd (1.3.6.1.2.1.10.127.1.1.4.1.3.48) 
SnmpRequestID : 0x74 0xDB 0x92 0xA9
*/
static unsigned char docsIfSigQCorrecteds2ndMsg[] =
        {0x30, 0x2E, 0x02, 0x01, 0x00, 0x04, 0x06, 0x70,
         0x75, 0x62, 0x6C, 0x69, 0x63, 0xA0, 0x21, 0x02,
         0x04, 0x74, 0xDB, 0x92, 0xA9, 0x02, 0x01, 0x00,
         0x02, 0x01, 0x00, 0x30, 0x13, 0x30, 0x11, 0x06,
         0x0D, 0x2B, 0x06, 0x01, 0x02, 0x01, 0x0A, 0x7F,
         0x01, 0x01, 0x04, 0x01, 0x03, 0x30, 0x05, 0x00};

/* 
docsIfSigQCorrecteds - 3rd (1.3.6.1.2.1.10.127.1.1.4.1.3.49) 
SnmpRequestID : 0x74 0xDB 0x92 0xAA
*/
static unsigned char docsIfSigQCorrecteds3rdMsg[] =
        {0x30, 0x2E, 0x02, 0x01, 0x00, 0x04, 0x06, 0x70,
         0x75, 0x62, 0x6C, 0x69, 0x63, 0xA0, 0x21, 0x02,
         0x04, 0x74, 0xDB, 0x92, 0xAA, 0x02, 0x01, 0x00,
         0x02, 0x01, 0x00, 0x30, 0x13, 0x30, 0x11, 0x06,
         0x0D, 0x2B, 0x06, 0x01, 0x02, 0x01, 0x0A, 0x7F,
         0x01, 0x01, 0x04, 0x01, 0x03, 0x31, 0x05, 0x00};

/* 
docsIfSigQCorrecteds - 4th (1.3.6.1.2.1.10.127.1.1.4.1.3.50) 
SnmpRequestID : 0x74 0xDB 0x92 0xAB
*/
static unsigned char docsIfSigQCorrecteds4thMsg[] =
        {0x30, 0x2E, 0x02, 0x01, 0x00, 0x04, 0x06, 0x70,
         0x75, 0x62, 0x6C, 0x69, 0x63, 0xA0, 0x21, 0x02,
         0x04, 0x74, 0xDB, 0x92, 0xAB, 0x02, 0x01, 0x00,
         0x02, 0x01, 0x00, 0x30, 0x13, 0x30, 0x11, 0x06,
         0x0D, 0x2B, 0x06, 0x01, 0x02, 0x01, 0x0A, 0x7F,
         0x01, 0x01, 0x04, 0x01, 0x03, 0x32, 0x05, 0x00};

/* 
docsIfSigQCorrecteds - 5th (1.3.6.1.2.1.10.127.1.1.4.1.3.51) 
SnmpRequestID : 0x74 0xDB 0x92 0xAC
*/
static unsigned char docsIfSigQCorrecteds5thMsg[] =
        {0x30, 0x2E, 0x02, 0x01, 0x00, 0x04, 0x06, 0x70,
         0x75, 0x62, 0x6C, 0x69, 0x63, 0xA0, 0x21, 0x02,
         0x04, 0x74, 0xDB, 0x92, 0xAC, 0x02, 0x01, 0x00,
         0x02, 0x01, 0x00, 0x30, 0x13, 0x30, 0x11, 0x06,
         0x0D, 0x2B, 0x06, 0x01, 0x02, 0x01, 0x0A, 0x7F,
         0x01, 0x01, 0x04, 0x01, 0x03, 0x33, 0x05, 0x00};

/* 
docsIfSigQCorrecteds - 6th (1.3.6.1.2.1.10.127.1.1.4.1.3.52) 
SnmpRequestID : 0x74 0xDB 0x92 0xAD
*/
static unsigned char docsIfSigQCorrecteds6thMsg[] =
        {0x30, 0x2E, 0x02, 0x01, 0x00, 0x04, 0x06, 0x70,
         0x75, 0x62, 0x6C, 0x69, 0x63, 0xA0, 0x21, 0x02,
         0x04, 0x74, 0xDB, 0x92, 0xAD, 0x02, 0x01, 0x00,
         0x02, 0x01, 0x00, 0x30, 0x13, 0x30, 0x11, 0x06,
         0x0D, 0x2B, 0x06, 0x01, 0x02, 0x01, 0x0A, 0x7F,
         0x01, 0x01, 0x04, 0x01, 0x03, 0x34, 0x05, 0x00};

/* 
docsIfSigQCorrecteds - 7th (1.3.6.1.2.1.10.127.1.1.4.1.3.53) 
SnmpRequestID : 0x74 0xDB 0x92 0xAE
*/
static unsigned char docsIfSigQCorrecteds7thMsg[] =
        {0x30, 0x2E, 0x02, 0x01, 0x00, 0x04, 0x06, 0x70,
         0x75, 0x62, 0x6C, 0x69, 0x63, 0xA0, 0x21, 0x02,
         0x04, 0x74, 0xDB, 0x92, 0xAE, 0x02, 0x01, 0x00,
         0x02, 0x01, 0x00, 0x30, 0x13, 0x30, 0x11, 0x06,
         0x0D, 0x2B, 0x06, 0x01, 0x02, 0x01, 0x0A, 0x7F,
         0x01, 0x01, 0x04, 0x01, 0x03, 0x35, 0x05, 0x00};

/* 
docsIfSigQCorrecteds - 8th (1.3.6.1.2.1.10.127.1.1.4.1.3.54) 
SnmpRequestID : 0x74 0xDB 0x92 0xAF
*/
static unsigned char docsIfSigQCorrecteds8thMsg[] =
        {0x30, 0x2E, 0x02, 0x01, 0x00, 0x04, 0x06, 0x70,
         0x75, 0x62, 0x6C, 0x69, 0x63, 0xA0, 0x21, 0x02,
         0x04, 0x74, 0xDB, 0x92, 0xAF, 0x02, 0x01, 0x00,
         0x02, 0x01, 0x00, 0x30, 0x13, 0x30, 0x11, 0x06,
         0x0D, 0x2B, 0x06, 0x01, 0x02, 0x01, 0x0A, 0x7F,
         0x01, 0x01, 0x04, 0x01, 0x03, 0x36, 0x05, 0x00};

#define DOCSIFSIGQCORRECTEDS30MSG_OFFSET 46

/* 
docsIfSigQUncorrectables - 1st (1.3.6.1.2.1.10.127.1.1.4.1.4.3) 
SnmpRequestID : 0x74 0xDB 0x92 0xB0
*/
static unsigned char docsIfSigQUncorrectables1stMsg[] =
        {0x30, 0x2E, 0x02, 0x01, 0x00, 0x04, 0x06, 0x70,
         0x75, 0x62, 0x6C, 0x69, 0x63, 0xA0, 0x21, 0x02,
         0x04, 0x74, 0xDB, 0x92, 0xB0, 0x02, 0x01, 0x00,
         0x02, 0x01, 0x00, 0x30, 0x13, 0x30, 0x11, 0x06,
         0x0D, 0x2B, 0x06, 0x01, 0x02, 0x01, 0x0A, 0x7F,
         0x01, 0x01, 0x04, 0x01, 0x04, 0x03, 0x05, 0x00};

/* 
docsIfSigQUncorrectables - 2nd (1.3.6.1.2.1.10.127.1.1.4.1.4.48) 
SnmpRequestID : 0x74 0xDB 0x92 0xB1
*/
static unsigned char docsIfSigQUncorrectables2ndMsg[] =
        {0x30, 0x2E, 0x02, 0x01, 0x00, 0x04, 0x06, 0x70,
         0x75, 0x62, 0x6C, 0x69, 0x63, 0xA0, 0x21, 0x02,
         0x04, 0x74, 0xDB, 0x92, 0xB1, 0x02, 0x01, 0x00,
         0x02, 0x01, 0x00, 0x30, 0x13, 0x30, 0x11, 0x06,
         0x0D, 0x2B, 0x06, 0x01, 0x02, 0x01, 0x0A, 0x7F,
         0x01, 0x01, 0x04, 0x01, 0x04, 0x30, 0x05, 0x00};

/* 
docsIfSigQUncorrectables - 3rd (1.3.6.1.2.1.10.127.1.1.4.1.4.49) 
SnmpRequestID : 0x74 0xDB 0x92 0xB2
*/
static unsigned char docsIfSigQUncorrectables3rdMsg[] =
        {0x30, 0x2E, 0x02, 0x01, 0x00, 0x04, 0x06, 0x70,
         0x75, 0x62, 0x6C, 0x69, 0x63, 0xA0, 0x21, 0x02,
         0x04, 0x74, 0xDB, 0x92, 0xB2, 0x02, 0x01, 0x00,
         0x02, 0x01, 0x00, 0x30, 0x13, 0x30, 0x11, 0x06,
         0x0D, 0x2B, 0x06, 0x01, 0x02, 0x01, 0x0A, 0x7F,
         0x01, 0x01, 0x04, 0x01, 0x04, 0x31, 0x05, 0x00};

/* 
docsIfSigQUncorrectables - 4th (1.3.6.1.2.1.10.127.1.1.4.1.4.50) 
SnmpRequestID : 0x74 0xDB 0x92 0xB3
*/
static unsigned char docsIfSigQUncorrectables4thMsg[] =
        {0x30, 0x2E, 0x02, 0x01, 0x00, 0x04, 0x06, 0x70,
         0x75, 0x62, 0x6C, 0x69, 0x63, 0xA0, 0x21, 0x02,
         0x04, 0x74, 0xDB, 0x92, 0xB3, 0x02, 0x01, 0x00,
         0x02, 0x01, 0x00, 0x30, 0x13, 0x30, 0x11, 0x06,
         0x0D, 0x2B, 0x06, 0x01, 0x02, 0x01, 0x0A, 0x7F,
         0x01, 0x01, 0x04, 0x01, 0x04, 0x32, 0x05, 0x00};

/* 
docsIfSigQUncorrectables - 5th (1.3.6.1.2.1.10.127.1.1.4.1.4.51) 
SnmpRequestID : 0x74 0xDB 0x92 0xB4
*/
static unsigned char docsIfSigQUncorrectables5thMsg[] =
        {0x30, 0x2E, 0x02, 0x01, 0x00, 0x04, 0x06, 0x70,
         0x75, 0x62, 0x6C, 0x69, 0x63, 0xA0, 0x21, 0x02,
         0x04, 0x74, 0xDB, 0x92, 0xB4, 0x02, 0x01, 0x00,
         0x02, 0x01, 0x00, 0x30, 0x13, 0x30, 0x11, 0x06,
         0x0D, 0x2B, 0x06, 0x01, 0x02, 0x01, 0x0A, 0x7F,
         0x01, 0x01, 0x04, 0x01, 0x04, 0x33, 0x05, 0x00};

/* 
docsIfSigQUncorrectables - 6th (1.3.6.1.2.1.10.127.1.1.4.1.4.52) 
SnmpRequestID : 0x74 0xDB 0x92 0xB5
*/
static unsigned char docsIfSigQUncorrectables6thMsg[] =
        {0x30, 0x2E, 0x02, 0x01, 0x00, 0x04, 0x06, 0x70,
         0x75, 0x62, 0x6C, 0x69, 0x63, 0xA0, 0x21, 0x02,
         0x04, 0x74, 0xDB, 0x92, 0xB5, 0x02, 0x01, 0x00,
         0x02, 0x01, 0x00, 0x30, 0x13, 0x30, 0x11, 0x06,
         0x0D, 0x2B, 0x06, 0x01, 0x02, 0x01, 0x0A, 0x7F,
         0x01, 0x01, 0x04, 0x01, 0x04, 0x34, 0x05, 0x00};

/* 
docsIfSigQUncorrectables - 7th (1.3.6.1.2.1.10.127.1.1.4.1.4.53) 
SnmpRequestID : 0x74 0xDB 0x92 0xB6
*/
static unsigned char docsIfSigQUncorrectables7thMsg[] =
        {0x30, 0x2E, 0x02, 0x01, 0x00, 0x04, 0x06, 0x70,
         0x75, 0x62, 0x6C, 0x69, 0x63, 0xA0, 0x21, 0x02,
         0x04, 0x74, 0xDB, 0x92, 0xB6, 0x02, 0x01, 0x00,
         0x02, 0x01, 0x00, 0x30, 0x13, 0x30, 0x11, 0x06,
         0x0D, 0x2B, 0x06, 0x01, 0x02, 0x01, 0x0A, 0x7F,
         0x01, 0x01, 0x04, 0x01, 0x04, 0x35, 0x05, 0x00};

/* 
docsIfSigQUncorrectables - 8th (1.3.6.1.2.1.10.127.1.1.4.1.4.54) 
SnmpRequestID : 0x74 0xDB 0x92 0xB7
*/
static unsigned char docsIfSigQUncorrectables8thMsg[] =
        {0x30, 0x2E, 0x02, 0x01, 0x00, 0x04, 0x06, 0x70,
         0x75, 0x62, 0x6C, 0x69, 0x63, 0xA0, 0x21, 0x02,
         0x04, 0x74, 0xDB, 0x92, 0xB7, 0x02, 0x01, 0x00,
         0x02, 0x01, 0x00, 0x30, 0x13, 0x30, 0x11, 0x06,
         0x0D, 0x2B, 0x06, 0x01, 0x02, 0x01, 0x0A, 0x7F,
         0x01, 0x01, 0x04, 0x01, 0x04, 0x36, 0x05, 0x00};

#define DOCSIFSIGQUNCORRECTABLES30MSG_OFFSET 46

/* 
docsIfUpChannelFrequency - 1st (1.3.6.1.2.1.10.127.1.1.2.1.2.4)
SnmpRequestID : 0x74 0xDB 0x92 0xBC
*/
static unsigned char docsIfUpChannelFrequency1stMsg[] =
        {0x30, 0x2E, 0x02, 0x01, 0x00, 0x04, 0x06, 0x70,
         0x75, 0x62, 0x6C, 0x69, 0x63, 0xA0, 0x21, 0x02,
         0x04, 0x74, 0xDB, 0x92, 0xBC, 0x02, 0x01, 0x00,
         0x02, 0x01, 0x00, 0x30, 0x13, 0x30, 0x11, 0x06,
         0x0D, 0x2B, 0x06, 0x01, 0x02, 0x01, 0x0A, 0x7f,
         0x01, 0x01, 0x02, 0x01, 0x02, 0x04, 0x05, 0x00};

/* 
docsIfUpChannelFrequency - 2nd (1.3.6.1.2.1.10.127.1.1.2.1.2.80)
SnmpRequestID : 0x74 0xDB 0x92 0xBD
*/
static unsigned char docsIfUpChannelFrequency2ndMsg[] =
        {0x30, 0x2E, 0x02, 0x01, 0x00, 0x04, 0x06, 0x70,
         0x75, 0x62, 0x6C, 0x69, 0x63, 0xA0, 0x21, 0x02,
         0x04, 0x74, 0xDB, 0x92, 0xBD, 0x02, 0x01, 0x00,
         0x02, 0x01, 0x00, 0x30, 0x13, 0x30, 0x11, 0x06,
         0x0D, 0x2B, 0x06, 0x01, 0x02, 0x01, 0x0A, 0x7f,
         0x01, 0x01, 0x02, 0x01, 0x02, 0x50, 0x05, 0x00};

/* 
docsIfUpChannelFrequency - 3rd (1.3.6.1.2.1.10.127.1.1.2.1.2.81)
SnmpRequestID : 0x74 0xDB 0x92 0xBE
*/
static unsigned char docsIfUpChannelFrequency3rdMsg[] =
        {0x30, 0x2E, 0x02, 0x01, 0x00, 0x04, 0x06, 0x70,
         0x75, 0x62, 0x6C, 0x69, 0x63, 0xA0, 0x21, 0x02,
         0x04, 0x74, 0xDB, 0x92, 0xBE, 0x02, 0x01, 0x00,
         0x02, 0x01, 0x00, 0x30, 0x13, 0x30, 0x11, 0x06,
         0x0D, 0x2B, 0x06, 0x01, 0x02, 0x01, 0x0A, 0x7f,
         0x01, 0x01, 0x02, 0x01, 0x02, 0x51, 0x05, 0x00};

/* 
docsIfUpChannelFrequency - 4th (1.3.6.1.2.1.10.127.1.1.2.1.2.82)
SnmpRequestID : 0x74 0xDB 0x92 0xBF
*/
static unsigned char docsIfUpChannelFrequency4thMsg[] =
        {0x30, 0x2E, 0x02, 0x01, 0x00, 0x04, 0x06, 0x70,
         0x75, 0x62, 0x6C, 0x69, 0x63, 0xA0, 0x21, 0x02,
         0x04, 0x74, 0xDB, 0x92, 0xBF, 0x02, 0x01, 0x00,
         0x02, 0x01, 0x00, 0x30, 0x13, 0x30, 0x11, 0x06,
         0x0D, 0x2B, 0x06, 0x01, 0x02, 0x01, 0x0A, 0x7f,
         0x01, 0x01, 0x02, 0x01, 0x02, 0x52, 0x05, 0x00};

#define  DOCSIFUPCHANNELFREQUENCY30MSG_OFFSET 46

/* 
docsIf3CmStatusUsTxPower - 1st (1.3.6.1.4.1.4491.2.1.20.1.2.1.1.4) 
SnmpRequestID : 0x74 0xDB 0x92 0xC8
*/
static unsigned char docsIf3CmStatusUsTxPower1stMsg[] =
        {0x30, 0x30, 0x02, 0x01, 0x00, 0x04, 0x06, 0x70,
         0x75, 0x62, 0x6C, 0x69, 0x63, 0xA0, 0x23, 0x02,
         0x04, 0x74, 0xDB, 0x92, 0xC8, 0x02, 0x01, 0x00,
         0x02, 0x01, 0x00, 0x30, 0x15, 0x30, 0x13, 0x06,
         0x0F, 0x2B, 0x06, 0x01, 0x04, 0x01, 0xA3, 0x0B,
         0x02, 0x01, 0x14, 0x01, 0x02, 0x01, 0x01, 0x04,
         0x05, 0x00};


/* 
docsIf3CmStatusUsTxPower - 2nd (1.3.6.1.4.1.4491.2.1.20.1.2.1.1.80) 
SnmpRequestID : 0x74 0xDB 0x92 0xC9
*/
static unsigned char docsIf3CmStatusUsTxPower2ndMsg[] =
        {0x30, 0x30, 0x02, 0x01, 0x00, 0x04, 0x06, 0x70,
         0x75, 0x62, 0x6C, 0x69, 0x63, 0xA0, 0x23, 0x02,
         0x04, 0x74, 0xDB, 0x92, 0xC9, 0x02, 0x01, 0x00,
         0x02, 0x01, 0x00, 0x30, 0x15, 0x30, 0x13, 0x06,
         0x0F, 0x2B, 0x06, 0x01, 0x04, 0x01, 0xA3, 0x0B,
         0x02, 0x01, 0x14, 0x01, 0x02, 0x01, 0x01, 0x50,
         0x05, 0x00};


/* 
docsIf3CmStatusUsTxPower - 3rd (1.3.6.1.4.1.4491.2.1.20.1.2.1.1.81) 
SnmpRequestID : 0x74 0xDB 0x92 0xCA
*/
static unsigned char docsIf3CmStatusUsTxPower3rdMsg[] =
        {0x30, 0x30, 0x02, 0x01, 0x00, 0x04, 0x06, 0x70,
         0x75, 0x62, 0x6C, 0x69, 0x63, 0xA0, 0x23, 0x02,
         0x04, 0x74, 0xDB, 0x92, 0xCA, 0x02, 0x01, 0x00,
         0x02, 0x01, 0x00, 0x30, 0x15, 0x30, 0x13, 0x06,
         0x0F, 0x2B, 0x06, 0x01, 0x04, 0x01, 0xA3, 0x0B,
         0x02, 0x01, 0x14, 0x01, 0x02, 0x01, 0x01, 0x51,
         0x05, 0x00};


/* 
docsIf3CmStatusUsTxPower - 4th (1.3.6.1.4.1.4491.2.1.20.1.2.1.1.82) 
SnmpRequestID : 0x74 0xDB 0x92 0xCB
*/
static unsigned char docsIf3CmStatusUsTxPower4thMsg[] =
        {0x30, 0x30, 0x02, 0x01, 0x00, 0x04, 0x06, 0x70,
         0x75, 0x62, 0x6C, 0x69, 0x63, 0xA0, 0x23, 0x02,
         0x04, 0x74, 0xDB, 0x92, 0xCB, 0x02, 0x01, 0x00,
         0x02, 0x01, 0x00, 0x30, 0x15, 0x30, 0x13, 0x06,
         0x0F, 0x2B, 0x06, 0x01, 0x04, 0x01, 0xA3, 0x0B,
         0x02, 0x01, 0x14, 0x01, 0x02, 0x01, 0x01, 0x52,
         0x05, 0x00};

#define  DOCSIF3CMSTATUSUSTXPOWER30MSG_OFFSET 48

/* 
docsIf3CmStatusUsRangingStatus - 1st (1.3.6.1.4.1.4491.2.1.20.1.2.1.9.4) 
SnmpRequestID : 0x74 0xDB 0x92 0xCC
*/
static unsigned char docsIf3CmStatusUsRangingStatus1stMsg[] =
        {0x30, 0x30, 0x02, 0x01, 0x00, 0x04, 0x06, 0x70,
         0x75, 0x62, 0x6C, 0x69, 0x63, 0xA0, 0x23, 0x02,
         0x04, 0x74, 0xDB, 0x92, 0xCC, 0x02, 0x01, 0x00,
         0x02, 0x01, 0x00, 0x30, 0x15, 0x30, 0x13, 0x06,
         0x0F, 0x2B, 0x06, 0x01, 0x04, 0x01, 0xA3, 0x0B,
         0x02, 0x01, 0x14, 0x01, 0x02, 0x01, 0x09, 0x04,
         0x05, 0x00};

/* 
docsIf3CmStatusUsRangingStatus - 2nd (1.3.6.1.4.1.4491.2.1.20.1.2.1.9.80) 
SnmpRequestID : 0x74 0xDB 0x92 0xCD
*/
static unsigned char docsIf3CmStatusUsRangingStatus2ndMsg[] =
        {0x30, 0x30, 0x02, 0x01, 0x00, 0x04, 0x06, 0x70,
         0x75, 0x62, 0x6C, 0x69, 0x63, 0xA0, 0x23, 0x02,
         0x04, 0x74, 0xDB, 0x92, 0xCD, 0x02, 0x01, 0x00,
         0x02, 0x01, 0x00, 0x30, 0x15, 0x30, 0x13, 0x06,
         0x0F, 0x2B, 0x06, 0x01, 0x04, 0x01, 0xA3, 0x0B,
         0x02, 0x01, 0x14, 0x01, 0x02, 0x01, 0x09, 0x50,
         0x05, 0x00};

/* 
docsIf3CmStatusUsRangingStatus - 3rd (1.3.6.1.4.1.4491.2.1.20.1.2.1.9.81) 
SnmpRequestID : 0x74 0xDB 0x92 0xCE
*/
static unsigned char docsIf3CmStatusUsRangingStatus3rdMsg[] =
        {0x30, 0x30, 0x02, 0x01, 0x00, 0x04, 0x06, 0x70,
         0x75, 0x62, 0x6C, 0x69, 0x63, 0xA0, 0x23, 0x02,
         0x04, 0x74, 0xDB, 0x92, 0xCE, 0x02, 0x01, 0x00,
         0x02, 0x01, 0x00, 0x30, 0x15, 0x30, 0x13, 0x06,
         0x0F, 0x2B, 0x06, 0x01, 0x04, 0x01, 0xA3, 0x0B,
         0x02, 0x01, 0x14, 0x01, 0x02, 0x01, 0x09, 0x51,
         0x05, 0x00};

/* 
docsIf3CmStatusUsRangingStatus - 4th (1.3.6.1.4.1.4491.2.1.20.1.2.1.9.82) 
SnmpRequestID : 0x74 0xDB 0x92 0xCF
*/
static unsigned char docsIf3CmStatusUsRangingStatus4thMsg[] =
        {0x30, 0x30, 0x02, 0x01, 0x00, 0x04, 0x06, 0x70,
         0x75, 0x62, 0x6C, 0x69, 0x63, 0xA0, 0x23, 0x02,
         0x04, 0x74, 0xDB, 0x92, 0xCF, 0x02, 0x01, 0x00,
         0x02, 0x01, 0x00, 0x30, 0x15, 0x30, 0x13, 0x06,
         0x0F, 0x2B, 0x06, 0x01, 0x04, 0x01, 0xA3, 0x0B,
         0x02, 0x01, 0x14, 0x01, 0x02, 0x01, 0x09, 0x52,
         0x05, 0x00};

#define  DOCSIF3CMSTATUSUSRANGINGSTATUS30MSG_OFFSET 48

/* 
docsIfUpChannelModulation - 1st (1.3.6.1.2.1.10.127.1.1.2.1.1.4)
SnmpRequestID : 0x74 0xDB 0x92 0xB8
*/
static unsigned char docsIfUpChannelId1stMsg[] =
        {0x30, 0x2E, 0x02, 0x01, 0x00, 0x04, 0x06, 0x70,
         0x75, 0x62, 0x6C, 0x69, 0x63, 0xA0, 0x21, 0x02,
         0x04, 0x74, 0xDB, 0x92, 0xB8, 0x02, 0x01, 0x00,
         0x02, 0x01, 0x00, 0x30, 0x13, 0x30, 0x11, 0x06,
         0x0D, 0x2B, 0x06, 0x01, 0x02, 0x01, 0x0A, 0x7f,
         0x01, 0x01, 0x02, 0x01, 0x01, 0x04, 0x05, 0x00};

/* 
docsIfUpChannelModulation - 2nd (1.3.6.1.2.1.10.127.1.1.2.1.1.80)
SnmpRequestID : 0x74 0xDB 0x92 0xB9
*/
static unsigned char docsIfUpChannelId2ndMsg[] =
        {0x30, 0x2E, 0x02, 0x01, 0x00, 0x04, 0x06, 0x70,
         0x75, 0x62, 0x6C, 0x69, 0x63, 0xA0, 0x21, 0x02,
         0x04, 0x74, 0xDB, 0x92, 0xB9, 0x02, 0x01, 0x00,
         0x02, 0x01, 0x00, 0x30, 0x13, 0x30, 0x11, 0x06,
         0x0D, 0x2B, 0x06, 0x01, 0x02, 0x01, 0x0A, 0x7f,
         0x01, 0x01, 0x02, 0x01, 0x01, 0x50, 0x05, 0x00};

/* 
docsIfUpChannelModulation - 3rd (1.3.6.1.2.1.10.127.1.1.2.1.1.81)
SnmpRequestID : 0x74 0xDB 0x92 0xBA
*/
static unsigned char docsIfUpChannelId3rdMsg[] =
        {0x30, 0x2E, 0x02, 0x01, 0x00, 0x04, 0x06, 0x70,
         0x75, 0x62, 0x6C, 0x69, 0x63, 0xA0, 0x21, 0x02,
         0x04, 0x74, 0xDB, 0x92, 0xBA, 0x02, 0x01, 0x00,
         0x02, 0x01, 0x00, 0x30, 0x13, 0x30, 0x11, 0x06,
         0x0D, 0x2B, 0x06, 0x01, 0x02, 0x01, 0x0A, 0x7f,
         0x01, 0x01, 0x02, 0x01, 0x01, 0x51, 0x05, 0x00};

/* 
docsIfUpChannelModulation - 4th (1.3.6.1.2.1.10.127.1.1.2.1.1.82)
SnmpRequestID : 0x74 0xDB 0x92 0xBB
*/
static unsigned char docsIfUpChannelId4thMsg[] =
        {0x30, 0x2E, 0x02, 0x01, 0x00, 0x04, 0x06, 0x70,
         0x75, 0x62, 0x6C, 0x69, 0x63, 0xA0, 0x21, 0x02,
         0x04, 0x74, 0xDB, 0x92, 0xBB, 0x02, 0x01, 0x00,
         0x02, 0x01, 0x00, 0x30, 0x13, 0x30, 0x11, 0x06,
         0x0D, 0x2B, 0x06, 0x01, 0x02, 0x01, 0x0A, 0x7f,
         0x01, 0x01, 0x02, 0x01, 0x01, 0x52, 0x05, 0x00};

#define  DOCSIFUPCHANNELID30MSG_OFFSET 46

/* 
docsIfUpChannelType - 1st (1.3.6.1.2.1.10.127.1.1.2.1.15.4) 
SnmpRequestID : 0x74 0xDB 0x92 0xC4
*/
static unsigned char docsIfUpChannelType1stMsg[] =
        {0x30, 0x2E, 0x02, 0x01, 0x00, 0x04, 0x06, 0x70,
         0x75, 0x62, 0x6C, 0x69, 0x63, 0xA0, 0x21, 0x02,
         0x04, 0x74, 0xDB, 0x92, 0xC4, 0x02, 0x01, 0x00,
         0x02, 0x01, 0x00, 0x30, 0x13, 0x30, 0x11, 0x06,
         0x0D, 0x2B, 0x06, 0x01, 0x02, 0x01, 0x0A, 0x7f,
         0x01, 0x01, 0x02, 0x01, 0x0F, 0x04, 0x05, 0x00};

/* 
docsIfUpChannelType - 2nd (1.3.6.1.2.1.10.127.1.1.2.1.15.80) 
SnmpRequestID : 0x74 0xDB 0x92 0xC5
*/
static unsigned char docsIfUpChannelType2ndMsg[] =
        {0x30, 0x2E, 0x02, 0x01, 0x00, 0x04, 0x06, 0x70,
         0x75, 0x62, 0x6C, 0x69, 0x63, 0xA0, 0x21, 0x02,
         0x04, 0x74, 0xDB, 0x92, 0xC5, 0x02, 0x01, 0x00,
         0x02, 0x01, 0x00, 0x30, 0x13, 0x30, 0x11, 0x06,
         0x0D, 0x2B, 0x06, 0x01, 0x02, 0x01, 0x0A, 0x7f,
         0x01, 0x01, 0x02, 0x01, 0x0F, 0x50, 0x05, 0x00};

/* 
docsIfUpChannelType - 3rd (1.3.6.1.2.1.10.127.1.1.2.1.15.81) 
SnmpRequestID : 0x74 0xDB 0x92 0xC6
*/
static unsigned char docsIfUpChannelType3rdMsg[] =
        {0x30, 0x2E, 0x02, 0x01, 0x00, 0x04, 0x06, 0x70,
         0x75, 0x62, 0x6C, 0x69, 0x63, 0xA0, 0x21, 0x02,
         0x04, 0x74, 0xDB, 0x92, 0xC6, 0x02, 0x01, 0x00,
         0x02, 0x01, 0x00, 0x30, 0x13, 0x30, 0x11, 0x06,
         0x0D, 0x2B, 0x06, 0x01, 0x02, 0x01, 0x0A, 0x7f,
         0x01, 0x01, 0x02, 0x01, 0x0F, 0x51, 0x05, 0x00};

/* 
docsIfUpChannelType - 4th (1.3.6.1.2.1.10.127.1.1.2.1.15.82) 
SnmpRequestID : 0x74 0xDB 0x92 0xC7
*/
static unsigned char docsIfUpChannelType4thMsg[] =
        {0x30, 0x2E, 0x02, 0x01, 0x00, 0x04, 0x06, 0x70,
         0x75, 0x62, 0x6C, 0x69, 0x63, 0xA0, 0x21, 0x02,
         0x04, 0x74, 0xDB, 0x92, 0xC7, 0x02, 0x01, 0x00,
         0x02, 0x01, 0x00, 0x30, 0x13, 0x30, 0x11, 0x06,
         0x0D, 0x2B, 0x06, 0x01, 0x02, 0x01, 0x0A, 0x7f,
         0x01, 0x01, 0x02, 0x01, 0x0F, 0x52, 0x05, 0x00};

#define DOCSIFUPCHANNELTYPE30MSG_OFFSET 46

/* 
docsIfUpChannelWidth - 1st (1.3.6.1.2.1.10.127.1.1.2.1.3.4) 
SnmpRequestID : 0x74 0xDB 0x92 0xC0
*/
static unsigned char docsIfUpChannelWidth1stMsg[] =
        {0x30, 0x2E, 0x02, 0x01, 0x00, 0x04, 0x06, 0x70,
         0x75, 0x62, 0x6C, 0x69, 0x63, 0xA0, 0x21, 0x02,
         0x04, 0x74, 0xDB, 0x92, 0xC0, 0x02, 0x01, 0x00,
         0x02, 0x01, 0x00, 0x30, 0x13, 0x30, 0x11, 0x06,
         0x0D, 0x2B, 0x06, 0x01, 0x02, 0x01, 0x0A, 0x7f,
         0x01, 0x01, 0x02, 0x01, 0x03, 0x04, 0x05, 0x00};

/* 
docsIfUpChannelWidth - 2nd (1.3.6.1.2.1.10.127.1.1.2.1.3.80) 
SnmpRequestID : 0x74 0xDB 0x92 0xC1
*/
static unsigned char docsIfUpChannelWidth2ndMsg[] =
        {0x30, 0x2E, 0x02, 0x01, 0x00, 0x04, 0x06, 0x70,
         0x75, 0x62, 0x6C, 0x69, 0x63, 0xA0, 0x21, 0x02,
         0x04, 0x74, 0xDB, 0x92, 0xC1, 0x02, 0x01, 0x00,
         0x02, 0x01, 0x00, 0x30, 0x13, 0x30, 0x11, 0x06,
         0x0D, 0x2B, 0x06, 0x01, 0x02, 0x01, 0x0A, 0x7f,
         0x01, 0x01, 0x02, 0x01, 0x03, 0x50, 0x05, 0x00};

/* 
docsIfUpChannelWidth - 3rd (1.3.6.1.2.1.10.127.1.1.2.1.3.81) 
SnmpRequestID : 0x74 0xDB 0x92 0xC2
*/
static unsigned char docsIfUpChannelWidth3rdMsg[] =
        {0x30, 0x2E, 0x02, 0x01, 0x00, 0x04, 0x06, 0x70,
         0x75, 0x62, 0x6C, 0x69, 0x63, 0xA0, 0x21, 0x02,
         0x04, 0x74, 0xDB, 0x92, 0xC2, 0x02, 0x01, 0x00,
         0x02, 0x01, 0x00, 0x30, 0x13, 0x30, 0x11, 0x06,
         0x0D, 0x2B, 0x06, 0x01, 0x02, 0x01, 0x0A, 0x7f,
         0x01, 0x01, 0x02, 0x01, 0x03, 0x51, 0x05, 0x00};

/* 
docsIfUpChannelWidth - 4th (1.3.6.1.2.1.10.127.1.1.2.1.3.82) 
SnmpRequestID : 0x74 0xDB 0x92 0xC3
*/
static unsigned char docsIfUpChannelWidth4thMsg[] =
        {0x30, 0x2E, 0x02, 0x01, 0x00, 0x04, 0x06, 0x70,
         0x75, 0x62, 0x6C, 0x69, 0x63, 0xA0, 0x21, 0x02,
         0x04, 0x74, 0xDB, 0x92, 0xC3, 0x02, 0x01, 0x00,
         0x02, 0x01, 0x00, 0x30, 0x13, 0x30, 0x11, 0x06,
         0x0D, 0x2B, 0x06, 0x01, 0x02, 0x01, 0x0A, 0x7f,
         0x01, 0x01, 0x02, 0x01, 0x03, 0x52, 0x05, 0x00};

#define DOCSIFUPCHANNELWIDTH30MSG_OFFSET 46

/* TODO - Is below array needed ??? */
#if 0
/* ifOperStatus.1 (1.3.6.1.2.1.2.2.1.8.1) */
static unsigned char ifOperStatusMsg[] =
        {0x30, 0x2B, 0x02, 0x01, 0x00, 0x04, 0x06, 0x70,
         0x75, 0x62, 0x6C, 0x69, 0x63, 0xA0, 0x1E, 0x02,
         0x04, 0x74, 0xDB, 0x92, 0x69, 0x02, 0x01, 0x00,
         0x02, 0x01, 0x00, 0x30, 0x10, 0x30, 0x0E, 0x06,
         0x0A, 0x2B, 0x06, 0x01, 0x02, 0x01, 0x02, 0x02,
         0x01, 0x08, 0x01, 0x05, 0x00};

#define  IFOPERSTATUSMSG_OFFSET 43
#endif

/*
cmScanPushFrequency.0 (1.3.6.1.4.1.4413.2.2.99.4413.2.2.1.0) 
SnmpRequestID : 0x99 0x02 0x01 0x00
*/
static unsigned char cmScanPushFrequencyMsg[] =
        {0x30, 0x34, 0x02, 0x01, 0x00, 0x04, 0x07, 0x70,
         0x72, 0x69, 0x76, 0x61, 0x74, 0x65, 0xA3, 0x26,
         0x02, 0x02, 0x04, 0x99, 0x02, 0x01, 0x00, 0x02,
         0x01, 0x00, 0x30, 0x1A, 0x30, 0x18, 0x06, 0x10,
         0x2B, 0x06, 0x01, 0x04, 0x01, 0xA2, 0x3D, 0x02,
         0x02, 0x63, 0xA2, 0x3D, 0x02, 0x02, 0x01, 0x00,
         0x02, 0x04, 0x06, 0x9D, 0xB9, 0xC0};  /* Value of 51th is Frequency Num*/

#define CMSCANPUSHFREQUENCY_OFFSET 48 /*ASN Int 02: 49th -1 */

/* 
cmScanPushFrequency.0 (1.3.6.1.4.1.4413.2.2.99.4413.2.2.1.0) 
SnmpRequestID : 0x04 0x99
*/
static unsigned char cmScanPushFrequencyClearMsg[] =
        {0x30, 0x31, 0x02, 0x01, 0x00, 0x04, 0x07, 0x70,
         0x72, 0x69, 0x76, 0x61, 0x74, 0x65, 0xA3, 0x23,
         0x02, 0x02, 0x04, 0x99, 0x02, 0x01, 0x00, 0x02,
         0x01, 0x00, 0x30, 0x17, 0x30, 0x15, 0x06, 0x10,
         0x2B, 0x06, 0x01, 0x04, 0x01, 0xA2, 0x3D, 0x02,
         0x02, 0x63, 0xA2, 0x3D, 0x02, 0x02, 0x01, 0x00,
         0x02, 0x01, 0x00};  /* Value of 51th is Frequency Num*/

#define CMSCANPUSHFREQUENCYCLEAR_OFFSET 48 /*ASN Int 02: 49th -1 */

/* 
cmScanFrequency (1.3.6.1.4.1.4413.2.2.99.4413.2.2.2.1.2:1) 
SnmpRequestID : 0x27 0xC8
*/
static unsigned char cmScanFrequencyMsg[] =
        {0x30, 0x32, 0x02, 0x01, 0x00, 0x04, 0x07, 0x70,
         0x72, 0x69, 0x76, 0x61, 0x74, 0x65, 0xA0, 0x24,
         0x02, 0x02, 0x27, 0xC8, 0x02, 0x01, 0x00, 0x02,
         0x01, 0x00, 0x30, 0x18, 0x30, 0x16, 0x06, 0x12,
         0x2B, 0x06, 0x01, 0x04, 0x01, 0xA2, 0x3D, 0x02,
         0x02, 0x63, 0xA2, 0x3D, 0x02, 0x02, 0x02, 0x01,
         0x02, 0x01, 0x05, 0x00};  /* Value of 50th is Frequency Index */

#define CMSCANFREQUENCY_OFFSET 50 /*ASN Int 02: 51th -1 */


/* 
cdFactCommitSettings.0  (1.3.6.1.4.14413.2.99.1.1.2.1.1.0 )
SnmpRequestID : 0x76 0xD1
*/
static unsigned char cdFactCommitSettingsMsg[] =
        {0x30, 0x30, 0x02, 0x01, 0x00, 0x04, 0x07, 0x70,
         0x72, 0x69, 0x76, 0x61, 0x74, 0x65, 0xA3, 0x22,
         0x02, 0x02, 0x76, 0xD1, 0x02, 0x01, 0x00, 0x02,
         0x01, 0x00, 0x30, 0x16, 0x30, 0x14, 0x06, 0x0F,
         0x2B, 0x06, 0x01, 0x04, 0x01, 0xA2, 0x3D, 0x02,
         0x63, 0x01, 0x01, 0x02, 0x01, 0x01, 0x00, 0x02,
         0x01, 0x01};

#define  CDFACTCOMMITSETTINGS_OFFSET 47 /* Asn Int 02: 48th -1*/

/* 
cdPrivateMibEnable.0  (1.3.6.1.4.1.4413.2.99.1.1.1.1.0 )
SnmpRequestID : 0x13 0xF1
*/

static unsigned char cdPrivateMibEnableMsg[] =
        {0x30, 0x2F, 0x02, 0x01, 0x00, 0x04, 0x07, 0x70,
         0x72, 0x69, 0x76, 0x61, 0x74, 0x65, 0xA3, 0x21,
         0x02, 0x02, 0x13, 0xF1, 0x02, 0x01, 0x00, 0x02,
         0x01, 0x00, 0x30, 0x15, 0x30, 0x13, 0x06, 0x0E,
         0x2B, 0x06, 0x01, 0x04, 0x01, 0xA2, 0x3D, 0x02,
         0x63, 0x01, 0x01, 0x01, 0x01, 0x00, 0x02, 0x01,
         0x01};

#define  CDPRIVATEMIBENABLE_OFFSET 46 /* Asn Int 02: 47th -1*/

/* 
cdPvtMibEnableKeyValue.1 (1.3.6.1.4.1.4413.2.99.1.1.1.2.1.2.1 password) 
SnmpRequestID : 0x65 0x4F
*/
static unsigned char cdPvtMibEnableKeyValueMsg[] =
        {0x30, 0x38, 0x02, 0x01, 0x00, 0x04, 0x07, 0x70,
         0x72, 0x69, 0x76, 0x61, 0x74, 0x65, 0xA3, 0x2A,
         0x02, 0x02, 0x65, 0x4F, 0x02, 0x01, 0x00, 0x02,
         0x01, 0x00, 0x30, 0x1E, 0x30, 0x1C, 0x06, 0x10,
         0x2B, 0x06, 0x01, 0x04, 0x01, 0xA2, 0x3D, 0x02,
         0x63, 0x01, 0x01, 0x01, 0x02, 0x01, 0x02, 0x01,
         0x04, 0x08, 0x70, 0x61, 0x73, 0x73, 0x77, 0x6F,
         0x72, 0x64};

#define  CDPVTMIBENABLEKEYVALUE_OFFSET 48 /* Asn Oct 04: 49th -1*/

/* 
cdSerialNumber.5 (1.3.6.1.4.1.4413.2.99.1.1.2.1.3.1.2.5) 
SnmpRequestID : 0x39 0xB6
*/
static unsigned char cdSerialNumberMsg[] =
        {0x30, 0x3F, 0x02, 0x01, 0x00, 0x04, 0x07, 0x70,
         0x72, 0x69, 0x76, 0x61, 0x74, 0x65, 0xA3, 0x31,
         0x02, 0x02, 0x39, 0xB6, 0x02, 0x01, 0x00, 0x02,
         0x01, 0x00, 0x30, 0x25, 0x30, 0x23, 0x06, 0x11,
         0x2B, 0x06, 0x01, 0x04, 0x01, 0xA2, 0x3D, 0x02,
         0x63, 0x01, 0x01, 0x02, 0x01, 0x03, 0x01, 0x02,
         0x05, 0x04, 0x0E, 0x31, 0x32, 0x33, 0x34, 0x35,
         0x36, 0x37, 0x38, 0x39, 0x30, 0x31, 0x32, 0x33,
         0x34};  /* Value of 52th is Serial Num Index */

#define CDSERIALNUMBER_OFFSET 49 /*ASN Octet String 04: 50th -1 */




/*******************************************************************/
/******************** global function prototype ********************/
/*******************************************************************/
/* eSTB -> eCM SNMP Response */
/* Data format :				*/
/*	1 : TransID 				*/
/*	2 : SNMP Get Response		*/
/*	3 ~ 6 : msg_len 		*/
/*	7 : SNMP Response Event ID	*/
/*	8 ~ : PDU					*/
int DsgSendeCMSnmpResponse(unsigned char *pdata, unsigned int msg_len)
{
	unsigned char DsgConfig[64];
	unsigned char *pPduData = NULL;
	int ret_val;
	unsigned int ulmsgLen = msg_len-2;

	pPduData = DsgConfig;
	CM_DI_Print(1,("[%s][%d] ulmsgLen = %d\n", __FUNCTION__, __LINE__, ulmsgLen));
	pthread_mutex_lock(&LnxDsgApiMutex);

	//Reset memory.
	VK_memset( DsgConfig, 0, 64);

	DsgConfig[0] = ++TransId;
	DsgConfig[1] = Api_eCMSNMPGetResponse;	// KEventeCMSNMPGetResponse;

	pPduData++; // 1st is TransID
	pPduData++; // 2nd is Event SnmpResponse

	/* 3~6 : number of msg_len */
	*pPduData++ = (unsigned char)(ulmsgLen >> 24);
	*pPduData++ = (unsigned char)(ulmsgLen >> 16);
	*pPduData++ = (unsigned char)(ulmsgLen >> 8);
	*pPduData++ = (unsigned char)(ulmsgLen >> 0);

	while(msg_len--)
	{
		*pPduData++ = *pdata++;
	}
#if 0
	{
		unsigned int i = 0;

		for( i=0; i<ulmsgLen+8 ; i++)
		{
			VK_printf(" %02x ", DsgConfig[i]);
		}
		VK_printf("\n");
	}
#endif
	ret_val = EstbToEcmSocketSendRx( 64, DsgConfig, true, false);

	pthread_mutex_unlock(&LnxDsgApiMutex);


	return ret_val;
}

static void P_CM_SendSNMPGetResponse(HUINT16 ucSnmpEventId, HUINT32 msg_len, unsigned char *pdata)
{
	unsigned char ucGetResponse[64];
	unsigned char *pPduData = NULL;
	unsigned int ulSnmpMsgLen=0;

	pPduData = ucGetResponse;
	ucGetResponse[0] = ucSnmpEventId >> 8;
	ucGetResponse[1] = ucSnmpEventId >> 0;

	ulSnmpMsgLen = msg_len+2;	// add SNMP Event ID
	pPduData++;	// snmp event id
	pPduData++;	// snmp event id

	while(msg_len--)
	{
		*pPduData++ = *pdata++;
	}

#if 0
	{
		unsigned int i = 0;
		CM_DI_Print(0, ("[%s][%d] ucSnmpEventId(%d) ulSnmpMsgLen(%d)\n", __FUNCTION__, __LINE__, ucSnmpEventId, ulSnmpMsgLen));
		for( i=0; i<ulSnmpMsgLen ; i++)
		{
			CM_DI_Print (0, (" %02x ", ucGetResponse[i]));
		}
		CM_DI_Print(0, ("\n"));
	}
#endif

	// TBD
	DsgSendeCMSnmpResponse(ucGetResponse, ulSnmpMsgLen);
}

static void P_CM_RecvSNMPGetRequest(HUINT16 ucSnmpEventId, HUINT32 msg_len)
{
	msg_len = msg_len;
	switch(ucSnmpEventId)
	{
#if 0 /*cannot query to eCM */
		case SNMP_eCmMacInfo:
		{
			unsigned char cm_macaddr[6]={""};
			DI_CM_GetCmMac((unsigned char *)cm_macaddr);
			DBG_DDI_STBSTEATE( TraceInfo( TRACE_MODULE_SYSINFO, "CableModem mac addr \t[%02hX:%02hX:%02hX:%02hX:%02hX:%02hX]\n",
				cm_macaddr[0], cm_macaddr[1], cm_macaddr[2],
				cm_macaddr[3], cm_macaddr[4], cm_macaddr[5] ) );

			P_CM_SendSNMPGetResponse(ucSnmpEventId, 6, cm_macaddr);
			break;
		}
#endif
		case SNMP_eCmSerialNum:
		{
			unsigned char ucSysSerialNum[20]={""};
			//DI_CM_GetSerialNum((unsigned char *)ucSysSerialNum);

			P_CM_SendSNMPGetResponse(ucSnmpEventId, sizeof(ucSysSerialNum), ucSysSerialNum);
			break;
		}
		case SNMP_eCmGlueVer:
		{
			unsigned char ucGetGlueVer[20]={""};
			//DI_CM_GetGlueVer(ucGetGlueVer);

			P_CM_SendSNMPGetResponse(ucSnmpEventId, sizeof(ucGetGlueVer), ucGetGlueVer);
			break;
		}
		case SNMP_eCmLoaderVer:
		{
			unsigned char ucGetLoaderVer[20]={""};
			//DI_CM_GetLoaderVer(ucGetLoaderVer);

			P_CM_SendSNMPGetResponse(ucSnmpEventId, sizeof(ucGetLoaderVer), ucGetLoaderVer);
			break;
		}
		case SNMP_eCmCurrentFreq:
		{
			unsigned int ulCurrentFreq = 0;
			//ulCurrentFreq= DI_CHANNEL_GetFrequency();

			P_CM_SendSNMPGetResponse(ucSnmpEventId, sizeof(ulCurrentFreq), (unsigned char *)&ulCurrentFreq);
			break;
		}
		case SNMP_eCmModulation:
		{
#if 0
			DDI_CHANNEL_Mod_t ulModulation = DDI_CHANNEL_Mod_eNONE;
			DDI_CHANNEL_Qam_t ulQamMod = DDI_CHANNEL_Qam_eNotDefined;
			DDI_CHANNEL_Vsb_t ulVsbMod = DDI_CHANNEL_Vsb_eNotDefined;
			unsigned int ulRet = 0;

			ulModulation= DDI_CHANNEL_GetModulation();

			if(ulModulation == DDI_CHANNEL_Mod_eQAM)
			{
				ulQamMod = DDI_CHANNEL_GetQAMModulation();
				switch(ulQamMod)
				{
					case DDI_CHANNEL_Qam_e64:
						ulRet = 1;
						P_CM_SendSNMPGetResponse(ucSnmpEventId, 2, (unsigned char *)&ulRet);
						break;
					case DDI_CHANNEL_Qam_e256:
						ulRet = 2;
						P_CM_SendSNMPGetResponse(ucSnmpEventId, 2, (unsigned char *)&ulRet);
						break;
					default :
						break;
				}
			}
			else if(ulModulation == DDI_CHANNEL_Mod_eVSB)
			{
				ulVsbMod = DDI_CHANNEL_GetVSBModulation();
				switch(ulVsbMod)
				{
					case DDI_CHANNEL_Vsb_e8:
						ulRet = 3;
						P_CM_SendSNMPGetResponse(ucSnmpEventId, 2, (unsigned char *)&ulRet);
						break;
					default :
						break;
				}
			}
			else
#endif
			{
				return;
			}
			break;
		}
		case SNMP_eCmPowerLevel:
		{
			int ulSignalLevel = 0;
			//ulSignalLevel= DDI_CHANNEL_GetSignalLevel();

			P_CM_SendSNMPGetResponse(ucSnmpEventId, sizeof(ulSignalLevel), (unsigned char *)&ulSignalLevel);
			break;
		}
		case SNMP_eCmInbandSnr:
		{
			int ulInbandSnr = 0;
			//ulInbandSnr= DDI_CHANNEL_GetSNRPercent();

			P_CM_SendSNMPGetResponse(ucSnmpEventId, sizeof(ulInbandSnr), (unsigned char *)&ulInbandSnr);
			break;
		}
		case SNMP_eCmCurrentState:
		{
			char acCurrentState[10];
#if 0
			if(DI_CM_GetStbPowerState() == DI_CM_StbPowerState_eSTANDBY)
			{
				VK_sprintf(acCurrentState, "%s", "STANDBY");
			}
			else if(DI_CM_GetStbPowerState() == DI_CM_StbPowerState_eRUNNING)
			{
				VK_sprintf(acCurrentState, "%s", "RUNNING");
			}
			else
#endif
			{
				VK_snprintf(acCurrentState, sizeof(acCurrentState), "%s", "UNKNOWN");
			}
			P_CM_SendSNMPGetResponse(ucSnmpEventId, sizeof(acCurrentState), (unsigned char *)acCurrentState);
			break;
		}
		case SNMP_eCmTotalWorkingTime:
		{
			char  acTotalWorkingTime[20];
			//VKI_GetTimeStamp( acTotalWorkingTime, 20 );

			P_CM_SendSNMPGetResponse(ucSnmpEventId, sizeof(acTotalWorkingTime), (unsigned char *)acTotalWorkingTime);
			break;
		}
		case SNMP_eCmCcManufacturer:
		{
			HUINT8 acuCcManufacturer[30];
			HUINT8 ucCcManufacturer = FALSE;

			//ucCcManufacturer = DDI_POD_GetPODManufacturerId();

			switch(ucCcManufacturer)
			{
				case 0:
					VK_snprintf((char *)acuCcManufacturer, sizeof(acuCcManufacturer), "%s", "MOTOROLA\0");
					break;

				case 1:
					VK_snprintf((char *)acuCcManufacturer, sizeof(acuCcManufacturer),"%s", "SCIENTIFIC ATLANTA\0");
					break;

				case 2:
					VK_snprintf((char *)acuCcManufacturer, sizeof(acuCcManufacturer), "%s", "SCM MICROSYSTEMS\0");
					break;
				case 3:
					VK_snprintf((char *)acuCcManufacturer, sizeof(acuCcManufacturer),"%s", "MAKUS\0");
					break;
				default:
					VK_snprintf((char *)acuCcManufacturer, sizeof(acuCcManufacturer),"%s", "UNKNOWN MANUFACTURER\0");
					break;
			}
			P_CM_SendSNMPGetResponse(ucSnmpEventId, sizeof(acuCcManufacturer), acuCcManufacturer);
			break;
		}
		case SNMP_eCmCcMacAddress:
		{
			HUINT8 acuCCMacAddr[6];
			//DI_CM_GetCcMac(acuCCMacAddr);

			P_CM_SendSNMPGetResponse(ucSnmpEventId, sizeof(acuCCMacAddr), acuCCMacAddr);
			break;
		}
		case SNMP_eCmCcIpAddress:
		{
			HUINT32 ulCcIpAddr=0;
			//DI_CM_GetCcIp(&ulCcIpAddr);
			ulCcIpAddr = htonl(ulCcIpAddr);
			CM_DI_Print(5, ("[%s][%d] SNMP_eCmCcIpAddress(%x)", __FUNCTION__, __LINE__, ulCcIpAddr));

			P_CM_SendSNMPGetResponse(ucSnmpEventId, sizeof(ulCcIpAddr), (unsigned char *)&ulCcIpAddr);
			break;
		}

		case SNMP_eCmSysSystemIdInfo:
		{
			HUINT8 acuSystemId[10];
			//DI_CM_GetSystemId(acuSystemId);

			P_CM_SendSNMPGetResponse(ucSnmpEventId, sizeof(acuSystemId), acuSystemId);
			break;
		}
		case SNMP_eCmSysHostIdInfo:
		{
			//ac_sysinfo_getHostId();

			//P_CM_SendSNMPGetResponse(ucSnmpEventId, 10, ac_sysinfo_getHostId());
			break;
		}
		case SNMP_eCmSysHostMacInfo:
		{
			HUINT8 acuHostMacAddr[6];
			DRV_CM_Bdsgcc_GetEstbMac(acuHostMacAddr);

			P_CM_SendSNMPGetResponse(ucSnmpEventId, sizeof(acuHostMacAddr), acuHostMacAddr);
			break;
		}
		case SNMP_eCmSysHostIpInfo:
		{
			HUINT32 ulHostIpAddr=0;
			GetIpStackAddress(&ulHostIpAddr, "eth1");
			CM_DI_Print(5, ("[%s][%d] SNMP_eCmSysHostIpInfo(%x)", __FUNCTION__, __LINE__, ulHostIpAddr));
			P_CM_SendSNMPGetResponse(ucSnmpEventId, sizeof(ulHostIpAddr), (unsigned char *)&ulHostIpAddr);
			break;
		}
		case SNMP_eCmVideoCurrentChannelNum:
		{
			//CM_DI_Print(0, ("[%s][%d] SNMP_eCmVideoCurrentChannelNum(%s)\n", __FUNCTION__, __LINE__, g_acuVideoCurrentChannelNum));
			//P_CM_SendSNMPGetResponse(ucSnmpEventId, sizeof(g_acuVideoCurrentChannelNum), g_acuVideoCurrentChannelNum);
			break;
		}
		case SNMP_eCmAudioFormat:
		{
			int ulAudioFormat = 0;
#if 0
			switch( g_stAvInfo.ulAudioType )
			{
				case ddi_audio_format_mpeg:
					ulAudioFormat = 1;
					break;
				case ddi_audio_format_mp3:
					ulAudioFormat = 1;
					break;
				case ddi_audio_format_aac:
					ulAudioFormat = 1;
					break;
				case ddi_audio_format_aac_plus:
					ulAudioFormat = 1;
					break;
				case ddi_audio_format_ac3:
					ulAudioFormat = 3;
					break;
				case ddi_audio_format_ac3_plus:
					ulAudioFormat = 3;
					break;
				case ddi_audio_format_dts:
					ulAudioFormat = 1;
					break;
				case ddi_audio_format_lpcm_hddvd:
					ulAudioFormat = 1;
					break;
				case ddi_audio_format_lpcm_bluray:
					ulAudioFormat = 1;
					break;
				case ddi_audio_format_dts_hd:
					ulAudioFormat = 1;
					break;
				case ddi_audio_format_wma_std:
					ulAudioFormat = 1;
					break;
				default:
					ulAudioFormat = 1;
					break;
			}
#endif
			//CM_DI_Print(0, ("[%s][%d] SNMP_eCmAudioFormat(%d)\n", __FUNCTION__, __LINE__, ulAudioFormat));
			P_CM_SendSNMPGetResponse(ucSnmpEventId, sizeof(ulAudioFormat), (unsigned char *)&ulAudioFormat);
			break;
		}
		case SNMP_eCmStbReset:
		{
			s_ulSTBResetStatus = 1;
			P_CM_SendSNMPGetResponse(ucSnmpEventId, sizeof(s_ulSTBResetStatus), (unsigned char *)&s_ulSTBResetStatus);
			break;
		}
		case SNMP_eCmCcReset:
		{
			//s_ulCcResetStatus = 1;
			//P_CM_SendSNMPGetResponse(ucSnmpEventId, sizeof(s_ulCcResetStatus), (unsigned char *)&s_ulCcResetStatus);
			break;
		}
		case SNMP_eCmFactoryReset:
		{
			s_ulFactoryResetStatus = 1;
			P_CM_SendSNMPGetResponse(ucSnmpEventId, sizeof(s_ulFactoryResetStatus), (unsigned char *)&s_ulFactoryResetStatus);
			break;
		}
		case SNMP_eCmStbChangeState:
		{
#if 0
			if(DI_CM_GetStbPowerState() == DI_CM_StbPowerState_eSTANDBY)
			{
				s_ulStbChangeState = 2;
			}
			else if(DI_CM_GetStbPowerState() == DI_CM_StbPowerState_eRUNNING)
			{
				s_ulStbChangeState = 3;
			}
			else
			{
				s_ulStbChangeState = 0;
			}
#endif
			CM_DI_Print(5, ("[%s][%d] SNMP_eCmStbChangeState(%d)\n", __FUNCTION__, __LINE__, s_ulStbChangeState));
			P_CM_SendSNMPGetResponse(ucSnmpEventId, sizeof(s_ulStbChangeState), (unsigned char *)&s_ulStbChangeState);
			break;
		}
		case SNMP_eCmSystemId:
		{
			P_CM_SendSNMPGetResponse(ucSnmpEventId, sizeof(g_stSnmpInfo.stSnmpMWInfo.szSystemId), g_stSnmpInfo.stSnmpMWInfo.szSystemId);
			break;
		}
#if 1
		case SNMP_eCmAppVersion:
		{

			P_CM_SendSNMPGetResponse(ucSnmpEventId, sizeof(g_stSnmpInfo.stSnmpMWInfo.szApplicationVer), g_stSnmpInfo.stSnmpMWInfo.szApplicationVer);
			break;
		}
		case SNMP_eCmMwVersion:
		{
			P_CM_SendSNMPGetResponse(ucSnmpEventId, sizeof(g_stSnmpInfo.stSnmpMWInfo.szApplicationVer), g_stSnmpInfo.stSnmpMWInfo.szMiddleWareVer);
			break;
		}
		case SNMP_eCmBrowserVersion:
		{

			P_CM_SendSNMPGetResponse(ucSnmpEventId, sizeof(g_stSnmpInfo.stSnmpMWInfo.szSoftBrowserVer), g_stSnmpInfo.stSnmpMWInfo.szSoftBrowserVer);
			break;
		}
		case SNMP_eCmStbId:
		{

			P_CM_SendSNMPGetResponse(ucSnmpEventId, sizeof(g_stSnmpInfo.stSnmpMWInfo.szStbID), g_stSnmpInfo.stSnmpMWInfo.szStbID);
			break;
		}
		case SNMP_eCmBcasId:
		{

			P_CM_SendSNMPGetResponse(ucSnmpEventId, sizeof(g_stSnmpInfo.stSnmpMWInfo.szBcasCardId), g_stSnmpInfo.stSnmpMWInfo.szBcasCardId);
			break;
		}
		case SNMP_eCmCcasId:
		{

			P_CM_SendSNMPGetResponse(ucSnmpEventId, sizeof(g_stSnmpInfo.stSnmpMWInfo.szCcasCardId), g_stSnmpInfo.stSnmpMWInfo.szCcasCardId);
			break;
		}
		case SNMP_eCmMacAddress:
		{
			unsigned char acuCCMacAddr[6]={0x00, 0x03, 0x08, 0x12, 0x34, 0x56};
			DRV_CM_Bdsgcc_GetEstbMac(acuCCMacAddr);

			P_CM_SendSNMPGetResponse(ucSnmpEventId, sizeof(acuCCMacAddr), (unsigned char *)&acuCCMacAddr);
			break;
		}
		case SNMP_eCmIpaddress:
		{

			HUINT32 ulHostIpAddr=0;
			HUINT32 ulLoop;
			for(ulLoop = 0; ulLoop < DRV_CFG_GetNetMaxDevice(); ulLoop++)
			{
				if(s_stDevList[ulLoop].eDevType == DI_NETWORK_DEV_CABLEMODEM)
				{
					CM_DI_Print(4,("[%s][%d] DevId : %d IfName : %s \n",__FUNCTION__, __LINE__,s_stDevList[ulLoop].ulDevId,s_stDevList[ulLoop].szName));
					GetIpStackAddress(&ulHostIpAddr, s_stDevList[ulLoop].szName);
					break;
				}
			}

			P_CM_SendSNMPGetResponse(ucSnmpEventId, sizeof(ulHostIpAddr), (unsigned char *)&ulHostIpAddr);
			break;
		}
		case SNMP_eCmNetMask:
		{
			int s, rc;
			struct ifreq buffer;
			struct sockaddr_in *sin;
			unsigned char NetMaskAddr[4];
			HUINT32 ulLoop;

			s = socket(AF_INET, SOCK_DGRAM, 0);

			if (s >=0)
			{
				VK_memset(&buffer, 0x00, sizeof(buffer));

				for(ulLoop = 0; ulLoop < DRV_CFG_GetNetMaxDevice(); ulLoop++)
				{
					if(s_stDevList[ulLoop].eDevType == DI_NETWORK_DEV_CABLEMODEM)
					{
						CM_DI_Print(4,("[%s][%d] DevId : %d IfName : %s \n",__FUNCTION__, __LINE__,s_stDevList[ulLoop].ulDevId,s_stDevList[ulLoop].szName));
						VK_strncpy(buffer.ifr_name, s_stDevList[ulLoop].szName, IFNAMSIZ-1 );
						break;
					}
				}

				rc =	ioctl(s, SIOCGIFNETMASK, (char *)&buffer);

				close(s);

				if (rc !=0)
				{
					CM_DI_Print(0, ( "[%s][%d] SNMP_eCmNetMask:: ioctl error!",__FUNCTION__, __LINE__));
					break;
				}

				sin = (struct sockaddr_in *) &buffer.ifr_addr;
				VK_memcpy(NetMaskAddr, (unsigned char *)&sin->sin_addr, 4);

				P_CM_SendSNMPGetResponse(ucSnmpEventId, 4, NetMaskAddr);
			}
			else
			{
				CM_DI_Print(0, ( "[%s][%d] SNMP_eCmNetMask:: socket open error!",__FUNCTION__, __LINE__));
			}

			break;
		}
		case SNMP_eCmDsPower:
		{

			P_CM_SendSNMPGetResponse(ucSnmpEventId, sizeof(g_stSnmpInfo.stSnmpDIInfo.ulDownChannelPower), (unsigned char *)&g_stSnmpInfo.stSnmpDIInfo.ulDownChannelPower);
			break;
		}
		case SNMP_eCmDsFrq:
		{
			HUINT32 ulDownFrq;
			ulDownFrq = g_stSnmpInfo.stSnmpMWInfo.ulDownFrequency*1000;
			P_CM_SendSNMPGetResponse(ucSnmpEventId, sizeof(g_stSnmpInfo.stSnmpMWInfo.ulDownFrequency), (unsigned char *)&ulDownFrq);
			break;
		}
		case SNMP_eCmServiceId:
		{

			P_CM_SendSNMPGetResponse(ucSnmpEventId, sizeof(g_stSnmpInfo.stSnmpMWInfo.ulWatchingChannelId), (unsigned char *)&g_stSnmpInfo.stSnmpMWInfo.ulWatchingChannelId);
			break;
		}
		case SNMP_eCmNetworkId:
		{

			P_CM_SendSNMPGetResponse(ucSnmpEventId, sizeof(g_stSnmpInfo.stSnmpMWInfo.ulWatchingNetworkId), (unsigned char *)&g_stSnmpInfo.stSnmpMWInfo.ulWatchingNetworkId);
			break;
		}
		case SNMP_eCmUnerroredNo:
		{

			P_CM_SendSNMPGetResponse(ucSnmpEventId, sizeof(g_stSnmpInfo.stSnmpDIInfo.ulUnErroredNo), (unsigned char *)&g_stSnmpInfo.stSnmpDIInfo.ulUnErroredNo);
			break;
		}
		case SNMP_eCmCorrectedNo:
		{

			P_CM_SendSNMPGetResponse(ucSnmpEventId, sizeof(g_stSnmpInfo.stSnmpDIInfo.ulCorrectedNo), (unsigned char *)&g_stSnmpInfo.stSnmpDIInfo.ulCorrectedNo);
			break;
		}
		case SNMP_eCmUncorrectedNo:
		{

			P_CM_SendSNMPGetResponse(ucSnmpEventId, sizeof(g_stSnmpInfo.stSnmpDIInfo.ulUnCorrectedNo), (unsigned char *)&g_stSnmpInfo.stSnmpDIInfo.ulUnCorrectedNo);
			break;
		}
		case SNMP_eCmInbandBer:
		{

			P_CM_SendSNMPGetResponse(ucSnmpEventId, sizeof(g_stSnmpInfo.stSnmpDIInfo.ulInbandBer), (unsigned char *)&g_stSnmpInfo.stSnmpDIInfo.ulInbandBer);
			break;
		}
		case SNMP_eCmPowerStatus:
		{

			P_CM_SendSNMPGetResponse(ucSnmpEventId, sizeof(g_stSnmpInfo.stSnmpMWInfo.etPowerStatus),(unsigned char *)&g_stSnmpInfo.stSnmpMWInfo.etPowerStatus);
			break;
		}
		case SNMP_eCmPlayStatus:
		{

			P_CM_SendSNMPGetResponse(ucSnmpEventId, sizeof(g_stSnmpInfo.stSnmpMWInfo.etPlayStatus), (unsigned char *)&g_stSnmpInfo.stSnmpMWInfo.etPlayStatus);
			break;
		}
		case SNMP_eCmTuner1ChId:
		{

			P_CM_SendSNMPGetResponse(ucSnmpEventId, sizeof(g_stSnmpInfo.stSnmpMWInfo.ulTuner1ChannelId), (unsigned char *)&g_stSnmpInfo.stSnmpMWInfo.ulTuner1ChannelId);
			break;
		}
		case SNMP_eCmTuner1NetId:
		{

			P_CM_SendSNMPGetResponse(ucSnmpEventId, sizeof(g_stSnmpInfo.stSnmpMWInfo.ulTuner1NetworkId), (unsigned char *)&g_stSnmpInfo.stSnmpMWInfo.ulTuner1NetworkId);
			break;
		}
		case SNMP_eCmTuner1Frq:
		{
			HUINT32 ulDownFrq;
			ulDownFrq = g_stSnmpInfo.stSnmpMWInfo.ulTuner1Frequency*1000;
			P_CM_SendSNMPGetResponse(ucSnmpEventId, sizeof(g_stSnmpInfo.stSnmpMWInfo.ulTuner1Frequency), (unsigned char *)&ulDownFrq);
			break;
		}
		case SNMP_eCmTuner2ChId:
		{

			P_CM_SendSNMPGetResponse(ucSnmpEventId, sizeof(g_stSnmpInfo.stSnmpMWInfo.ulTuner2ChannelId), (unsigned char *)&g_stSnmpInfo.stSnmpMWInfo.ulTuner2ChannelId);
			break;
		}
		case SNMP_eCmTuner2NetId:
		{

			P_CM_SendSNMPGetResponse(ucSnmpEventId, sizeof(g_stSnmpInfo.stSnmpMWInfo.ulTuner2NetworkId), (unsigned char *)&g_stSnmpInfo.stSnmpMWInfo.ulTuner2NetworkId);
			break;
		}
		case SNMP_eCmTuner2Frq:
		{
			HUINT32 ulDownFrq;
			ulDownFrq = g_stSnmpInfo.stSnmpMWInfo.ulTuner2Frequency*1000;
			P_CM_SendSNMPGetResponse(ucSnmpEventId, sizeof(g_stSnmpInfo.stSnmpMWInfo.ulTuner2Frequency), (unsigned char *)&ulDownFrq);
			break;
		}
#endif
		default :
			break;
	}
}

static void P_CM_RecvSNMPSetRequest(HUINT16 ucSnmpEventId, HUINT32 msg_len, HUINT8 pduData)
{
	msg_len = msg_len;
	switch(ucSnmpEventId)
	{
		case SNMP_eCmStbReset:
		{
			if ((pduData != TRUE) && (pduData != FALSE))
			{
				return;
			}
			s_ulSTBResetStatus = pduData;
			CM_DI_Print(0, ("[%s][%d] STB will be reboot in 5 second!!(SNMP)(%d)\n", __FUNCTION__, __LINE__, s_ulSTBResetStatus));
			VK_TASK_Sleep(5000);
			//VKI_Reboot();
			break;
		}

		case SNMP_eCmCcReset:
		{
			if ((pduData != TRUE) && (pduData != FALSE))
			{
				return;
			}
#if 0
			if(DDI_POD_Detect() == 1)
			{
				s_ulCcResetStatus = pduData;
				CM_DI_Print(0, ("[%s][%d] CableCARD will be reboot in 5 second!!(SNMP)(%d)\n", __FUNCTION__, __LINE__, s_ulCcResetStatus));
				VKI_TASK_Sleep(5000);
				DDI_POD_DevReset();
				s_ulCcResetStatus = 0;
			}
			else
			{
				CM_DI_Print(0, ("[%s][%d] CableCARD is not inserted\n", __FUNCTION__, __LINE__));
			}
#endif
			break;
		}
		case SNMP_eCmFactoryReset:
		{
			if ((pduData != TRUE) && (pduData != FALSE))
			{
				return;
			}
			s_ulFactoryResetStatus = pduData;
			CM_DI_Print(0, ("[%s][%d] FactoryReset Command from SNMP. It will be excute in 5 second!!(SNMP)(%d)\n", __FUNCTION__, __LINE__, s_ulFactoryResetStatus));
			VK_TASK_Sleep(5000);
			//DDI_FLASH_FactoryReset(DDI_FLASH_Region_eMWFFS);
			//VKI_Reboot();
			break;
		}
		case SNMP_eCmStbChangeState:
		{
			//CM_DI_Print(0, ("[%s][%d] SNMP_eCmStbChangeState(%d)\n", pduData));
			if ((pduData != 1) && (pduData != 2) && (pduData != 3))
			{
				return;
			}
			s_ulFactoryResetStatus = pduData;
			CM_DI_Print(0, ("[%s][%d] SNMP_eCmStbChangeState Command from SNMP. It will be excute in 1 second!!(SNMP)(%d)\n", __FUNCTION__, __LINE__, s_ulFactoryResetStatus));
			VK_TASK_Sleep(1000);

			if(s_ulFactoryResetStatus == 1)		/* Offline */
			{
				return;
			}
			else if(s_ulFactoryResetStatus == 2)	/* StandBy */
			{
				//ac_stb_changeState(AC_STB_STATE_STANDBY);
			}
			else if(s_ulFactoryResetStatus == 3)	/* Active */
			{
				//ac_stb_changeState(AC_STB_STATE_RUNNING);
			}
			else	/* Nothing */
			{
				return;
			}
			break;
		}


		default :
			break;
	}
}

static void P_CM_RebootMonitorTask(void* param)
{
	param=param;
	VK_TASK_Sleep(CM_REBOOT_MONITOR);

	while(1)
	{
		VK_TASK_Sleep(CM_REBOOT_MONITOR);

		if ((g_nCmRebootNotify == 1) && (s_nCmRebootNotifyDone == 0))
		{
			if (s_pfCmCallback[DRV_CM_EVENT_REBOOT] != NULL)
			{
				CM_DI_Print(0, ("[%s][%d] s_pfCmCallback[DRV_CM_EVENT_REBOOT] have to be called!! \n", __FUNCTION__, __LINE__));
				(*s_pfCmCallback[DRV_CM_EVENT_REBOOT])(NULL);
				s_nCmRebootNotifyDone = 1;
			}
			else
			{
				CM_DI_Print(0, ("[%s][%d] s_pfCmCallback[DRV_CM_EVENT_REBOOT] is NULL!! \n", __FUNCTION__, __LINE__));
			}
		}
	}
}

static DRV_Error P_CM_Bdsg_SetCmVer(unsigned char *pucCmSwVer)
{

	if( pucCmSwVer == (unsigned char *)NULL )
	{
		return DRV_ERR_INVALID_PARAMETER;
	}

	VK_memcpy( s_aucCmVersion, pucCmSwVer, sizeof(s_aucCmVersion) );
	s_bIsGetCmVer = TRUE;

	return DRV_OK;
}

static DRV_Error P_CM_Bdsg_GetCmVer(unsigned char *pucCmSwVer)
{

	if( pucCmSwVer == (unsigned char *)NULL )
	{
		return DRV_ERR_INVALID_PARAMETER;
	}

	if(s_bIsGetCmVer != FALSE)
	{
		VK_memcpy(pucCmSwVer, s_aucCmVersion, sizeof(s_aucCmVersion));
		return DRV_OK;
	}
	else
	{
		return DRV_ERR;
	}
}

static DRV_Error P_CM_Bdsgcc_SetCmMac(unsigned char *pucEcmMac)
{
	if( pucEcmMac == (unsigned char *)NULL )
	{
		return DRV_ERR_INVALID_PARAMETER;
	}

	VK_memcpy( s_aucCmMacAddress, pucEcmMac, 6 );
	s_bIsGetMac = TRUE;

	return DRV_OK;
}

static DRV_Error P_CM_Bdsgcc_GetCmMac(unsigned char *pucEcmMac)
{
	if( pucEcmMac == (unsigned char *)NULL )
	{
		return DRV_ERR_INVALID_PARAMETER;
	}

	if(s_bIsGetMac != FALSE)
	{
		VK_memcpy(pucEcmMac, s_aucCmMacAddress, 6);
		return DRV_OK;
	}
	else
	{
		return DRV_ERR;
	}
}

static void P_CM_Bdsgcc_SetCmIp(unsigned int ulEcmIp)
{
	//CM_DI_Print(0,("[%s][%d] ulEcmIp = %8x\n", __FUNCTION__, __LINE__, ulEcmIp));

	s_ulCmIpAddress = ntohl(ulEcmIp);	/*  di network use littel endian for display network information.   */
}

static DRV_Error P_CM_Bdsgcc_GetCmIp(unsigned int *pulEcmIp)
{
	//CM_DI_Print(0, ("[%s][%d] pulEcmIp = %8x\n", __FUNCTION__, __LINE__, *pulEcmIp ));
	if( pulEcmIp == (unsigned int *)NULL )
	{
		return DRV_ERR_INVALID_PARAMETER;
	}

	if(s_ulCmIpAddress != 0)
	{
		*pulEcmIp = s_ulCmIpAddress;
		return DRV_OK;
	}
	else
	{
		return DRV_ERR;
	}
}


/*****************************************************************************
* function : DRV_CM_Bdsgcc_GetEcmIp
* description : Get eCM IP address
* input :
*   pulEcmIp : pointer for storing eCM IP address
* return :
*   DRV_OK / DRV_ERR
******************************************************************************/
DRV_Error DRV_CM_Bdsgcc_GetEcmIp(HUINT32 *pulEcmIp)
{
	unsigned int temp=0;

	struct in_addr cm_str;
	struct sockaddr_in s_sCMservaddr_t;
	int n, s_sCMSocketfd=0;
	struct timeval tv;

	VK_MEM_Memset(&cm_str, 0, sizeof(struct in_addr));
	VK_MEM_Memset(&s_sCMservaddr_t, 0, sizeof(s_sCMservaddr_t));

	s_sCMservaddr_t.sin_family = AF_INET;
	s_sCMservaddr_t.sin_port = htons(SNMP_PORT);
	inet_pton(AF_INET, psCmtargetAddr, &s_sCMservaddr_t.sin_addr);

	s_sCMSocketfd = socket(AF_INET, SOCK_DGRAM, 0);

	if (s_sCMSocketfd == -1)
	{
		CM_DI_Print(0, ("[%s][%d] socket error!\n",__FUNCTION__, __LINE__));
		return DRV_ERR;
	}

	tv.tv_sec = 2;
	tv.tv_usec = 0;
	n = setsockopt(s_sCMSocketfd, SOL_SOCKET, SO_SNDTIMEO, &tv, sizeof(tv));
	n = setsockopt(s_sCMSocketfd, SOL_SOCKET, SO_RCVTIMEO, &tv, sizeof(tv));

	temp = GetCMIp(s_sCMSocketfd, (struct sockaddr_in *) &s_sCMservaddr_t, sizeof(s_sCMservaddr_t));
	close(s_sCMSocketfd);

	if(temp != 100)
	{
		P_CM_Bdsgcc_SetCmIp(temp);
		CM_DI_Print(5, ("[%s][%d] got mgmtIpAddressMsg = %X\n", __FUNCTION__, __LINE__, temp));

		/* Transfer Cm Ip address */
		if(P_CM_Bdsgcc_GetCmIp(pulEcmIp) == DRV_OK)
		{
			cm_str.s_addr = *pulEcmIp;
			CM_DI_Print(1, ("[%s][%d] 1. got mgmtIpAddressMsg = %x (%s)\n", __FUNCTION__, __LINE__, *pulEcmIp, inet_ntoa(cm_str)));

			return DRV_OK;
		}
		else
		{
			return DRV_ERR;
		}
	}

	return DRV_ERR;
}

/*****************************************************************************
* function : DRV_CM_Bdsgcc_GetEcmMac
* description : Get eCM MAC address
* input :
*   pucEcmMac : pointer for storing eCM MAC address
* return :
*   DRV_OK / DRV_ERR
******************************************************************************/
DRV_Error DRV_CM_Bdsgcc_GetEcmMac(HUINT8 *pucEcmMac)
{
#if defined(CONFIG_PRODUCT_IMAGE_FACTORY)
	unsigned char cmMacAddr[6];

	int iRetcode = 0;
	struct sockaddr_in s_sCMservaddr_t;
	int n, s_sCMSocketfd=0;
	struct timeval tv;
	unsigned char cdPvtMibEnableKeyValue[8]={""};
	int cdPvtMibEnableKeyValueLen=0, cdPvtMibEnableKeyValueSize=0;
	int cmMacAddrLen;

	VK_MEM_Memset(&s_sCMservaddr_t, 0, sizeof(s_sCMservaddr_t));

	s_sCMservaddr_t.sin_family = AF_INET;
	s_sCMservaddr_t.sin_port = htons(SNMP_PORT);
	inet_pton(AF_INET, psCmtargetAddr, &s_sCMservaddr_t.sin_addr);

	s_sCMSocketfd = socket(AF_INET, SOCK_DGRAM, 0);

	if (s_sCMSocketfd == -1)
	{
		CM_DI_Print(0, ("[%s][%d] socket error!\n",__FUNCTION__, __LINE__));
		return DRV_ERR;
	}

	tv.tv_sec = 2;
	tv.tv_usec = 0;
	n = setsockopt(s_sCMSocketfd, SOL_SOCKET, SO_SNDTIMEO, &tv, sizeof(tv));
	n = setsockopt(s_sCMSocketfd, SOL_SOCKET, SO_RCVTIMEO, &tv, sizeof(tv));

	cdPvtMibEnableKeyValueSize = sizeof(cdPvtMibEnableKeyValue);
	cdPvtMibEnableKeyValueLen = GetString_P(s_sCMSocketfd, (struct sockaddr_in *) &s_sCMservaddr_t, sizeof(s_sCMservaddr_t),
										(char *)cdPvtMibEnableKeyValueMsg, sizeof(cdPvtMibEnableKeyValueMsg), CDPVTMIBENABLEKEYVALUE_OFFSET,
	cdPvtMibEnableKeyValue, &cdPvtMibEnableKeyValueSize);
	if (cdPvtMibEnableKeyValueLen == 0)
	{
		close(s_sCMSocketfd);

		CM_DI_Print(0, ("[%s][%d] Error cdPvtMibEnableKeyValueLen is %d \n",__FUNCTION__, __LINE__, cdPvtMibEnableKeyValueLen));
		return DRV_ERR;
	}

	cmMacAddrLen = GetMac(s_sCMSocketfd, &s_sCMservaddr_t, sizeof(s_sCMservaddr_t),
						(char *)cdMacAddressGetMsg, sizeof(cdMacAddressGetMsg),
						CDMACADDRESSGET_OFFSET, cmMacAddr);
	if (cdPvtMibEnableKeyValueLen == 0)
	{
		close(s_sCMSocketfd);

		CM_DI_Print(0, ("[%s][%d] Error cmMacAddrLen is %d \n",__FUNCTION__, __LINE__, cmMacAddrLen));
		return DRV_ERR;
	}

	CM_DI_Print(0, ("[%s][%d] Length=%d, CM_MAC=%02X:%02X:%02X:%02X:%02X:%02X\n", __FUNCTION__, __LINE__, cmMacAddrLen, cmMacAddr[0], cmMacAddr[1], cmMacAddr[2], cmMacAddr[3], cmMacAddr[4], cmMacAddr[5]));

	cdPrivateMibEnableMsg[48] = 0x00;	/* disable */
	iRetcode = GetInt_P(s_sCMSocketfd, (struct sockaddr_in *) &s_sCMservaddr_t, sizeof(s_sCMservaddr_t),
					(char *)cdPrivateMibEnableMsg, sizeof(cdPrivateMibEnableMsg), CDPRIVATEMIBENABLE_OFFSET);
	if (iRetcode == -1)
	{
		close(s_sCMSocketfd);

		CM_DI_Print(0, ("[%s][%d] Error iRetcode is %d \n",__FUNCTION__, __LINE__, iRetcode));
		return DRV_ERR;
	}

	/* cdFactCommitSetting.0 true */
	iRetcode = GetInt_P(s_sCMSocketfd, (struct sockaddr_in *) &s_sCMservaddr_t, sizeof(s_sCMservaddr_t),
					(char *)cdFactCommitSettingsMsg, sizeof(cdFactCommitSettingsMsg), CDFACTCOMMITSETTINGS_OFFSET);
	if (iRetcode == -1)
	{
		close(s_sCMSocketfd);

		CM_DI_Print(0, ("[%s][%d] Error iRetcode is %d \n",__FUNCTION__, __LINE__, iRetcode));
		return DRV_ERR;
	}

	close(s_sCMSocketfd);
#else
	unsigned char cmMacAddr[6];

	struct sockaddr_in s_sCMservaddr_t;
	int n, s_sCMSocketfd=0;
	struct timeval tv;
	int cmMacAddrLen;

	VK_MEM_Memset(&s_sCMservaddr_t, 0, sizeof(s_sCMservaddr_t));

	s_sCMservaddr_t.sin_family = AF_INET;
	s_sCMservaddr_t.sin_port = htons(SNMP_PORT);
	inet_pton(AF_INET, psCmtargetAddr, &s_sCMservaddr_t.sin_addr);

	s_sCMSocketfd = socket(AF_INET, SOCK_DGRAM, 0);

	if (s_sCMSocketfd == -1)
	{
		CM_DI_Print(0, ("[%s][%d] socket error!\n",__FUNCTION__, __LINE__));
		return DRV_ERR;
	}

	tv.tv_sec = 2;
	tv.tv_usec = 0;
	n = setsockopt(s_sCMSocketfd, SOL_SOCKET, SO_SNDTIMEO, &tv, sizeof(tv));
	n = setsockopt(s_sCMSocketfd, SOL_SOCKET, SO_RCVTIMEO, &tv, sizeof(tv));

	cmMacAddrLen = GetMac(s_sCMSocketfd, (struct sockaddr_in *) &s_sCMservaddr_t, sizeof(s_sCMservaddr_t),
						(char *)ifPhysAddressMsg, sizeof(ifPhysAddressMsg), IFPHYSADDRESSMSG_OFFSET, cmMacAddr);
	close(s_sCMSocketfd);
#endif

#if 0
	if (cmMacAddrLen == 6)
	{
			CM_DI_Print(0, ("[%s][%d] CM mac addr in DDI 1\t[%02hX:%02hX:%02hX:%02hX:%02hX:%02hX]\n",
				__FUNCTION__, __LINE__, *(cmMacAddr), *(cmMacAddr+1), *(cmMacAddr+2), *(cmMacAddr+3), *(cmMacAddr+4), *(cmMacAddr+5) ));
	}
	else
	{
			CM_DI_Print(0, ("[%s][%d] GetMac() got %d bytes!!!\n", __FUNCTION__, __LINE__, cmMacAddrLen));
	}
#endif

	if(cmMacAddr != NULL)
	{
		P_CM_Bdsgcc_SetCmMac(cmMacAddr);

		/* transfered cm mac address */
		if(P_CM_Bdsgcc_GetCmMac(pucEcmMac) == DRV_OK)
		{
			return DRV_OK;
		}
		else
		{
			return DRV_ERR;
		}
	}
	else
	{
		CM_DI_Print(0, ("[%s][%d] Get Cm Mac fail\n",__FUNCTION__, __LINE__));
		return DRV_ERR;
	}
}

/*****************************************************************************
* function : DRV_CM_Bdsgcc_SetEcmMac
* description : Set eCM MAC address
* input :
*   pucEcmMac : pointer for storing eCM MAC address
* return :
*   DRV_OK / DRV_ERR
******************************************************************************/
DRV_Error DRV_CM_Bdsgcc_SetEcmMac(HUINT8 *pucEcmMac)
{
	int iRetcode = 0;
	struct sockaddr_in s_sCMservaddr_t;
	int n, s_sCMSocketfd=0;
	struct timeval tv;
	unsigned char cdPvtMibEnableKeyValue[8]={""}, cdPvtMibCmMacValue[6]={""}, ucCmMac[6]={""};
	int cdPvtMibEnableKeyValueLen=0, cdPvtMibEnableKeyValueSize=0, cdPvtMibCmMacLen=0, cdPvtMibCmMacSize=0;

	VK_MEM_Memset(&s_sCMservaddr_t, 0, sizeof(s_sCMservaddr_t));

	s_sCMservaddr_t.sin_family = AF_INET;
	s_sCMservaddr_t.sin_port = htons(SNMP_PORT);
	inet_pton(AF_INET, psCmtargetAddr, &s_sCMservaddr_t.sin_addr);

	s_sCMSocketfd = socket(AF_INET, SOCK_DGRAM, 0);

	if (s_sCMSocketfd == -1)
	{
		CM_DI_Print(0, ("[%s][%d] socket error!\n",__FUNCTION__, __LINE__));
		return DRV_ERR;
	}

	tv.tv_sec = 2;
	tv.tv_usec = 0;
	n = setsockopt(s_sCMSocketfd, SOL_SOCKET, SO_SNDTIMEO, &tv, sizeof(tv));
	n = setsockopt(s_sCMSocketfd, SOL_SOCKET, SO_RCVTIMEO, &tv, sizeof(tv));

	cdPvtMibEnableKeyValueSize = sizeof(cdPvtMibEnableKeyValue);
	cdPvtMibEnableKeyValueLen = GetString_P(s_sCMSocketfd, (struct sockaddr_in *) &s_sCMservaddr_t, sizeof(s_sCMservaddr_t),
										(char *)cdPvtMibEnableKeyValueMsg, sizeof(cdPvtMibEnableKeyValueMsg), CDPVTMIBENABLEKEYVALUE_OFFSET,
										cdPvtMibEnableKeyValue, &cdPvtMibEnableKeyValueSize);
	if (cdPvtMibEnableKeyValueLen == 0)
	{
		close(s_sCMSocketfd);

		CM_DI_Print(0, ("[%s][%d] Error cdPvtMibEnableKeyValueLen is %d \n",__FUNCTION__, __LINE__, cdPvtMibEnableKeyValueLen));
		return DRV_ERR;
	}

	VK_MEM_Memset(ucCmMac, 0, 6);

	ucCmMac[0] = *(pucEcmMac);
	ucCmMac[1] = *(pucEcmMac+1);
	ucCmMac[2] = *(pucEcmMac+2);
	ucCmMac[3] = *(pucEcmMac+3);
	ucCmMac[4] = *(pucEcmMac+4);
	ucCmMac[5] = *(pucEcmMac+5);

	CM_DI_Print(5, ("[%s][%d] (%02x, %02x, %02x, %02x, %02x, %02x)\n", __FUNCTION__, __LINE__, ucCmMac[5], ucCmMac[4], ucCmMac[3], ucCmMac[2], ucCmMac[1], ucCmMac[0]));

	/*  7424에서 전달 받은 mac이 반대로 저장되는 문제가 있어  byte order를 변경함.*/
	cdMacAddressMsg[51] = ucCmMac[0];
	cdMacAddressMsg[52] = ucCmMac[1];
	cdMacAddressMsg[53] = ucCmMac[2];
	cdMacAddressMsg[54] = ucCmMac[3];
	cdMacAddressMsg[55] = ucCmMac[4];
	cdMacAddressMsg[56] = ucCmMac[5];

	cdPvtMibCmMacSize = sizeof(cdPvtMibCmMacValue);
	cdPvtMibCmMacLen = GetString_P(s_sCMSocketfd, &s_sCMservaddr_t, sizeof(s_sCMservaddr_t),
								(char *)cdMacAddressMsg, sizeof(cdMacAddressMsg), CDMACADDRESS_OFFSET, 
								cdPvtMibCmMacValue, &cdPvtMibCmMacSize);
	if (cdPvtMibCmMacLen == 0)
	{
		close(s_sCMSocketfd);

		CM_DI_Print(0, ("[%s][%d] Error cdPvtMibCmMacLen is %d \n",__FUNCTION__, __LINE__, cdPvtMibCmMacLen));
		return DRV_ERR;
	}

	cdPrivateMibEnableMsg[48] = 0x00;	/* disable */
	iRetcode = GetInt_P(s_sCMSocketfd, (struct sockaddr_in *) &s_sCMservaddr_t, sizeof(s_sCMservaddr_t),
					(char *)cdPrivateMibEnableMsg, sizeof(cdPrivateMibEnableMsg), CDPRIVATEMIBENABLE_OFFSET);
	if (iRetcode == -1)
	{
		close(s_sCMSocketfd);

		CM_DI_Print(0, ("[%s][%d] Error iRetcode is %d \n",__FUNCTION__, __LINE__, iRetcode));
		return DRV_ERR;
	}

	/* cdFactCommitSetting.0 true */
	iRetcode = GetInt_P(s_sCMSocketfd, (struct sockaddr_in *) &s_sCMservaddr_t, sizeof(s_sCMservaddr_t),
					(char *)cdFactCommitSettingsMsg, sizeof(cdFactCommitSettingsMsg), CDFACTCOMMITSETTINGS_OFFSET);
	if (iRetcode == -1)
	{
		close(s_sCMSocketfd);

		CM_DI_Print(0, ("[%s][%d] Error iRetcode is %d \n",__FUNCTION__, __LINE__, iRetcode));
		return DRV_ERR;
	}

	close(s_sCMSocketfd);

	return DRV_OK;
}

/*****************************************************************************
* function : DRV_CM_Bdsgcc_GetNetmask
* description : Get eCM subnet mask
* input :
*   pulCmNetmask : pointer for storing eCM subnet mask
* return :
*   DRV_OK / DRV_ERR
******************************************************************************/
DRV_Error DRV_CM_Bdsgcc_GetNetmask(HUINT32 *pulCmNetmask)
{
	int nRet = DRV_ERR;

	nRet = DsgSendGetCmNetmask(pulCmNetmask);

	if(nRet != TRUE)
	{
		CM_DI_Print(0, ("[%s][%d] Err! nRet = %d\n", __FUNCTION__, __LINE__, nRet));
		return DRV_ERR;
	}
#if 0
	else
	{
		CM_DI_Print(1, ("[%s][%d] Netmask[%8x] \n", __FUNCTION__, __LINE__, *pulCmNetmask));
	}
#endif

	return DRV_OK;
}

/*****************************************************************************
* function : DRV_CM_Bdsgcc_GetGateway
* description : Get eCM gateway IP address
* input :
*   pulCmGateway : pointer for storing eCM gateway IP address
* return :
*   DRV_OK / DRV_ERR
******************************************************************************/
DRV_Error DRV_CM_Bdsgcc_GetGateway(HUINT32 *pulCmGateway)
{
	int nRet = DRV_ERR;

	nRet = DsgSendGetCmGateway(pulCmGateway);

	if(nRet != TRUE)
	{
		CM_DI_Print(0, ("[%s][%d] Err! nRet = %d\n", __FUNCTION__, __LINE__, nRet));
		return DRV_ERR;
	}
#if 0
	else
	{
		CM_DI_Print(1, ("[%s][%d] [%8x] \n", __FUNCTION__, __LINE__, *pulCmGateway));
	}
#endif

	return DRV_OK;
}

/*****************************************************************************
* function : DRV_CM_Bdsgcc_GetSwVer
* description : Get eCM SW version
* input :
*   pucCmSwVer : pointer for storing eCM SW version
* return :
*   DRV_OK / DRV_ERR
******************************************************************************/
DRV_Error DRV_CM_Bdsgcc_GetSwVer(HUINT8 *pucCmSwVer)
{
	/* deal with Canned data */
//	unsigned char *cmMacAddr;
	unsigned char cmVer[CMVERSIONLENGTH] = {""};
	int cmVerLen, cmVerSize = 0;

	struct sockaddr_in s_sCMservaddr_t;
	int n, s_sCMSocketfd=0;
	struct timeval tv;

	VK_MEM_Memset(&s_sCMservaddr_t, 0, sizeof(s_sCMservaddr_t));

	s_sCMservaddr_t.sin_family = AF_INET;
	s_sCMservaddr_t.sin_port = htons(SNMP_PORT);
	inet_pton(AF_INET, psCmtargetAddr, &s_sCMservaddr_t.sin_addr);

	s_sCMSocketfd = socket(AF_INET, SOCK_DGRAM, 0);

	if (s_sCMSocketfd == -1)
	{
		CM_DI_Print(0, ("[%s][%d] socket error!\n",__FUNCTION__, __LINE__));
		return DRV_ERR;
	}

	tv.tv_sec = 2;
	tv.tv_usec = 0;
	n = setsockopt(s_sCMSocketfd, SOL_SOCKET, SO_SNDTIMEO, &tv, sizeof(tv));
	n = setsockopt(s_sCMSocketfd, SOL_SOCKET, SO_RCVTIMEO, &tv, sizeof(tv));

	cmVerSize = sizeof(cmVer);
	cmVerLen= GetCmVer(s_sCMSocketfd, (struct sockaddr_in *) &s_sCMservaddr_t, sizeof(s_sCMservaddr_t),
						(char *)docsDevSwCurrentVersMsg, sizeof(docsDevSwCurrentVersMsg), DOCSDEVSWCURRENTVERSMSG_OFFSET,
						cmVer, &cmVerSize);

	close(s_sCMSocketfd);

	CM_DI_Print(1, ("[%s][%d] CM Version %s\n", __FUNCTION__, __LINE__, cmVer));


		if(cmVer != NULL)
		{
			P_CM_Bdsg_SetCmVer(cmVer);

			/* transfered cm mac address */
			if(P_CM_Bdsg_GetCmVer(pucCmSwVer) == DRV_OK)
			{
				return DRV_OK;
			}
			else
			{
				return DRV_ERR;
			}
		}
		else
		{
			CM_DI_Print(0, ("[%s][%d] Get Cm Ver fail\n", __FUNCTION__, __LINE__));
			return DRV_ERR;
		}
}

/*****************************************************************************
* function : DRV_CM_Bdsgcc_GetCmRegStatus
* description : Get eCM registration status
* input :
*   pnRegStatus : pointer for storing eCM registration status
* return :
*   DRV_OK / DRV_ERR
******************************************************************************/
DRV_Error DRV_CM_Bdsgcc_GetCmRegStatus(HINT32 *pnRegStatus)
{
	struct sockaddr_in s_sCMservaddr_t;
	int n, s_sCMSocketfd=0;
	struct timeval tv;

	VK_MEM_Memset(&s_sCMservaddr_t, 0, sizeof(s_sCMservaddr_t));

	s_sCMservaddr_t.sin_family = AF_INET;
	s_sCMservaddr_t.sin_port = htons(SNMP_PORT);
	inet_pton(AF_INET, psCmtargetAddr, &s_sCMservaddr_t.sin_addr);

	s_sCMSocketfd = socket(AF_INET, SOCK_DGRAM, 0);

	if (s_sCMSocketfd == -1)
	{
		CM_DI_Print(0, ("[%s][%d] socket error!\n", __FUNCTION__, __LINE__));
		return DRV_ERR;
	}

	tv.tv_sec = 2;
	tv.tv_usec = 0;
	n = setsockopt(s_sCMSocketfd, SOL_SOCKET, SO_SNDTIMEO, &tv, sizeof(tv));
	n = setsockopt(s_sCMSocketfd, SOL_SOCKET, SO_RCVTIMEO, &tv, sizeof(tv));

	// Get the CM's Status
	*pnRegStatus = GetInt(s_sCMSocketfd, &s_sCMservaddr_t, sizeof(s_sCMservaddr_t),
			(char *)docsIfCmStatusValueMsg, sizeof(docsIfCmStatusValueMsg),
			DOCSIFCMSTATUSVALUEMSG_OFFSET);

	CM_DI_Print(5, ("[%s][%d] pRegStatus : %d\n", __FUNCTION__, __LINE__, *pnRegStatus));

	close(s_sCMSocketfd);

	return DRV_OK;
}

/*****************************************************************************
* function : DRV_CM_Bdsgcc_GetDsFreq
* description : Get eCM downstream frequency
* input :
*   pnDsFreq : pointer for storing eCM downstream frequency
* return :
*   DRV_OK / DRV_ERR
******************************************************************************/
DRV_Error DRV_CM_Bdsgcc_GetDsFreq(HINT32 nDsId, HINT32 *pnDsFreq)
{
	struct sockaddr_in s_sCMservaddr_t;
	int n, s_sCMSocketfd=0;
	struct timeval tv;

	VK_MEM_Memset(&s_sCMservaddr_t, 0, sizeof(s_sCMservaddr_t));

	s_sCMservaddr_t.sin_family = AF_INET;
	s_sCMservaddr_t.sin_port = htons(SNMP_PORT);
	inet_pton(AF_INET, psCmtargetAddr, &s_sCMservaddr_t.sin_addr);

	s_sCMSocketfd = socket(AF_INET, SOCK_DGRAM, 0);

	if (s_sCMSocketfd == -1)
	{
		CM_DI_Print(0, ("[%s][%d] socket error!\n", __FUNCTION__, __LINE__));
		return DRV_ERR;
	}

	tv.tv_sec = 2;
	tv.tv_usec = 0;
	n = setsockopt(s_sCMSocketfd, SOL_SOCKET, SO_SNDTIMEO, &tv, sizeof(tv));
	n = setsockopt(s_sCMSocketfd, SOL_SOCKET, SO_RCVTIMEO, &tv, sizeof(tv));

	switch (nDsId)
	{
		case 0 :
			*pnDsFreq = GetInt(s_sCMSocketfd, &s_sCMservaddr_t, sizeof(s_sCMservaddr_t),
								(char *)docsIfDownChannelFrequency1stMsg, sizeof(docsIfDownChannelFrequency1stMsg),
								DOCSIFDOWNCHANNELFREQUENCY30MSG_OFFSET);
			break;
		case 1 :
			*pnDsFreq = GetInt(s_sCMSocketfd, &s_sCMservaddr_t, sizeof(s_sCMservaddr_t),
								(char *)docsIfDownChannelFrequency2ndMsg, sizeof(docsIfDownChannelFrequency2ndMsg),
								DOCSIFDOWNCHANNELFREQUENCY30MSG_OFFSET);
			break;
		case 2 :
			*pnDsFreq = GetInt(s_sCMSocketfd, &s_sCMservaddr_t, sizeof(s_sCMservaddr_t),
								(char *)docsIfDownChannelFrequency3rdMsg, sizeof(docsIfDownChannelFrequency3rdMsg),
								DOCSIFDOWNCHANNELFREQUENCY30MSG_OFFSET);
			break;
		case 3 :
			*pnDsFreq = GetInt(s_sCMSocketfd, &s_sCMservaddr_t, sizeof(s_sCMservaddr_t),
								(char *)docsIfDownChannelFrequency4thMsg, sizeof(docsIfDownChannelFrequency4thMsg),
								DOCSIFDOWNCHANNELFREQUENCY30MSG_OFFSET);
			break;
		case 4 :
			*pnDsFreq = GetInt(s_sCMSocketfd, &s_sCMservaddr_t, sizeof(s_sCMservaddr_t),
								(char *)docsIfDownChannelFrequency5thMsg, sizeof(docsIfDownChannelFrequency5thMsg),
								DOCSIFDOWNCHANNELFREQUENCY30MSG_OFFSET);
			break;
		case 5 :
			*pnDsFreq = GetInt(s_sCMSocketfd, &s_sCMservaddr_t, sizeof(s_sCMservaddr_t),
								(char *)docsIfDownChannelFrequency6thMsg, sizeof(docsIfDownChannelFrequency6thMsg),
								DOCSIFDOWNCHANNELFREQUENCY30MSG_OFFSET);
			break;
		case 6 :
			*pnDsFreq = GetInt(s_sCMSocketfd, &s_sCMservaddr_t, sizeof(s_sCMservaddr_t),
								(char *)docsIfDownChannelFrequency7thMsg, sizeof(docsIfDownChannelFrequency7thMsg),
								DOCSIFDOWNCHANNELFREQUENCY30MSG_OFFSET);
			break;
		case 7 :
			*pnDsFreq = GetInt(s_sCMSocketfd, &s_sCMservaddr_t, sizeof(s_sCMservaddr_t),
								(char *)docsIfDownChannelFrequency8thMsg, sizeof(docsIfDownChannelFrequency8thMsg),
								DOCSIFDOWNCHANNELFREQUENCY30MSG_OFFSET);
			break;
		default :
			CM_DI_Print(0, ("[%s][%d] Invalid Down Channel ID (%d)\n", __FUNCTION__, __LINE__, nDsId));
			*pnDsFreq = 0;
			break;
	}

	close(s_sCMSocketfd);

	CM_DI_Print(5, ("[%s][%d] Downstream Freq (%d) is %d\n", __FUNCTION__, __LINE__, nDsId, *pnDsFreq));

	return DRV_OK;
}

/*****************************************************************************
* function : DRV_CM_Bdsgcc_GetDsPower
* description : Get eCM downstream RF power level
* input :
*   nDsId : downstream channel index
*   pnDsPower : pointer for storing eCM downstream RF power level
* return :
*   DRV_OK / DRV_ERR
******************************************************************************/
DRV_Error DRV_CM_Bdsgcc_GetDsPower(HINT32 nDsId, HINT32 *pnDsPower)
{
	struct sockaddr_in s_sCMservaddr_t;
	int n, s_sCMSocketfd=0;
	struct timeval tv;

	VK_MEM_Memset(&s_sCMservaddr_t, 0, sizeof(s_sCMservaddr_t));

	s_sCMservaddr_t.sin_family = AF_INET;
	s_sCMservaddr_t.sin_port = htons(SNMP_PORT);
	inet_pton(AF_INET, psCmtargetAddr, &s_sCMservaddr_t.sin_addr);

	s_sCMSocketfd = socket(AF_INET, SOCK_DGRAM, 0);

	if (s_sCMSocketfd == -1)
	{
		CM_DI_Print(0, ("[%s][%d] socket error!\n",__FUNCTION__, __LINE__));
		return DRV_ERR;
	}

	tv.tv_sec = 2;
	tv.tv_usec = 0;
	n = setsockopt(s_sCMSocketfd, SOL_SOCKET, SO_SNDTIMEO, &tv, sizeof(tv));
	n = setsockopt(s_sCMSocketfd, SOL_SOCKET, SO_RCVTIMEO, &tv, sizeof(tv));

	switch (nDsId)
	{
		case 0 :
			*pnDsPower = GetInt(s_sCMSocketfd, &s_sCMservaddr_t, sizeof(s_sCMservaddr_t),
								(char *)docsIfDownChannelPower1stMsg, sizeof(docsIfDownChannelPower1stMsg),
								DOCSIFDOWNCHANNELPOWER30MSG_OFFSET);
			break;
		case 1 :
			*pnDsPower = GetInt(s_sCMSocketfd, &s_sCMservaddr_t, sizeof(s_sCMservaddr_t),
								(char *)docsIfDownChannelPower2ndMsg, sizeof(docsIfDownChannelPower2ndMsg),
								DOCSIFDOWNCHANNELPOWER30MSG_OFFSET);
			break;
		case 2 :
			*pnDsPower = GetInt(s_sCMSocketfd, &s_sCMservaddr_t, sizeof(s_sCMservaddr_t),
								(char *)docsIfDownChannelPower3rdMsg, sizeof(docsIfDownChannelPower3rdMsg),
								DOCSIFDOWNCHANNELPOWER30MSG_OFFSET);
			break;
		case 3 :
			*pnDsPower = GetInt(s_sCMSocketfd, &s_sCMservaddr_t, sizeof(s_sCMservaddr_t),
								(char *)docsIfDownChannelPower4thMsg, sizeof(docsIfDownChannelPower4thMsg),
								DOCSIFDOWNCHANNELPOWER30MSG_OFFSET);
			break;
		case 4 :
			*pnDsPower = GetInt(s_sCMSocketfd, &s_sCMservaddr_t, sizeof(s_sCMservaddr_t),
								(char *)docsIfDownChannelPower5thMsg, sizeof(docsIfDownChannelPower5thMsg),
								DOCSIFDOWNCHANNELPOWER30MSG_OFFSET);
			break;
		case 5 :
			*pnDsPower = GetInt(s_sCMSocketfd, &s_sCMservaddr_t, sizeof(s_sCMservaddr_t),
								(char *)docsIfDownChannelPower6thMsg, sizeof(docsIfDownChannelPower6thMsg),
								DOCSIFDOWNCHANNELPOWER30MSG_OFFSET);
			break;
		case 6 :
			*pnDsPower = GetInt(s_sCMSocketfd, &s_sCMservaddr_t, sizeof(s_sCMservaddr_t),
								(char *)docsIfDownChannelPower7thMsg, sizeof(docsIfDownChannelPower7thMsg),
								DOCSIFDOWNCHANNELPOWER30MSG_OFFSET);
			break;
		case 7 :
			*pnDsPower = GetInt(s_sCMSocketfd, &s_sCMservaddr_t, sizeof(s_sCMservaddr_t),
								(char *)docsIfDownChannelPower8thMsg, sizeof(docsIfDownChannelPower8thMsg),
								DOCSIFDOWNCHANNELPOWER30MSG_OFFSET);
			break;
		default :
			CM_DI_Print(0, ("[%s][%d] Invalid Down Channel ID (%d)\n", __FUNCTION__, __LINE__, nDsId));
			*pnDsPower = 0;
			break;
	}

	close(s_sCMSocketfd);

	CM_DI_Print(5, ("[%s][%d] Downstream Power (%d) is %d\n", __FUNCTION__, __LINE__, nDsId, *pnDsPower));

	return DRV_OK;
}

/*****************************************************************************
* function : DRV_CM_Bdsgcc_GetDsSnr
* description : Get eCM downstream SNR
* input :
*   nDsId : downstream channel index
*   pnDsSnr : pointer for storing eCM downstream SNR
* return :
*   DRV_OK / DRV_ERR
******************************************************************************/
DRV_Error DRV_CM_Bdsgcc_GetDsSnr(HINT32 nDsId, HINT32 *pnDsSnr)
{
	struct sockaddr_in s_sCMservaddr_t;
	int n, s_sCMSocketfd=0;
	struct timeval tv;

	VK_MEM_Memset(&s_sCMservaddr_t, 0, sizeof(s_sCMservaddr_t));

	s_sCMservaddr_t.sin_family = AF_INET;
	s_sCMservaddr_t.sin_port = htons(SNMP_PORT);
	inet_pton(AF_INET, psCmtargetAddr, &s_sCMservaddr_t.sin_addr);

	s_sCMSocketfd = socket(AF_INET, SOCK_DGRAM, 0);

	if (s_sCMSocketfd == -1)
	{
		CM_DI_Print(0, ("[%s][%d] socket error!\n", __FUNCTION__, __LINE__));
		return DRV_ERR;
	}

	tv.tv_sec = 2;
	tv.tv_usec = 0;
	n = setsockopt(s_sCMSocketfd, SOL_SOCKET, SO_SNDTIMEO, &tv, sizeof(tv));
	n = setsockopt(s_sCMSocketfd, SOL_SOCKET, SO_RCVTIMEO, &tv, sizeof(tv));

	switch (nDsId)
	{
		case 0 :
			*pnDsSnr = GetInt(s_sCMSocketfd, &s_sCMservaddr_t, sizeof(s_sCMservaddr_t),
								(char *)docsIfSigQSignalNoise1stMsg, sizeof(docsIfSigQSignalNoise1stMsg),
								DOCSIFSIGQSIGNALNOISE30MSG_OFFSET);
			break;
		case 1 :
			*pnDsSnr = GetInt(s_sCMSocketfd, &s_sCMservaddr_t, sizeof(s_sCMservaddr_t),
								(char *)docsIfSigQSignalNoise2ndMsg, sizeof(docsIfSigQSignalNoise2ndMsg),
								DOCSIFSIGQSIGNALNOISE30MSG_OFFSET);
			break;
		case 2 :
			*pnDsSnr = GetInt(s_sCMSocketfd, &s_sCMservaddr_t, sizeof(s_sCMservaddr_t),
								(char *)docsIfSigQSignalNoise3rdMsg, sizeof(docsIfSigQSignalNoise3rdMsg),
								DOCSIFSIGQSIGNALNOISE30MSG_OFFSET);
			break;
		case 3 :
			*pnDsSnr = GetInt(s_sCMSocketfd, &s_sCMservaddr_t, sizeof(s_sCMservaddr_t),
								(char *)docsIfSigQSignalNoise4thMsg, sizeof(docsIfSigQSignalNoise4thMsg),
								DOCSIFSIGQSIGNALNOISE30MSG_OFFSET);
			break;
		case 4 :
			*pnDsSnr = GetInt(s_sCMSocketfd, &s_sCMservaddr_t, sizeof(s_sCMservaddr_t),
								(char *)docsIfSigQSignalNoise5thMsg, sizeof(docsIfSigQSignalNoise5thMsg),
								DOCSIFSIGQSIGNALNOISE30MSG_OFFSET);
			break;
		case 5 :
			*pnDsSnr = GetInt(s_sCMSocketfd, &s_sCMservaddr_t, sizeof(s_sCMservaddr_t),
								(char *)docsIfSigQSignalNoise6thMsg, sizeof(docsIfSigQSignalNoise6thMsg),
								DOCSIFSIGQSIGNALNOISE30MSG_OFFSET);
			break;
		case 6 :
			*pnDsSnr = GetInt(s_sCMSocketfd, &s_sCMservaddr_t, sizeof(s_sCMservaddr_t),
								(char *)docsIfSigQSignalNoise7thMsg, sizeof(docsIfSigQSignalNoise7thMsg),
								DOCSIFSIGQSIGNALNOISE30MSG_OFFSET);
			break;
		case 7 :
			*pnDsSnr = GetInt(s_sCMSocketfd, &s_sCMservaddr_t, sizeof(s_sCMservaddr_t),
								(char *)docsIfSigQSignalNoise8thMsg, sizeof(docsIfSigQSignalNoise8thMsg),
								DOCSIFSIGQSIGNALNOISE30MSG_OFFSET);
			break;
		default :
			CM_DI_Print(0, ("[%s][%d] Invalid Down Channel ID (%d)\n", __FUNCTION__, __LINE__, nDsId));
			*pnDsSnr = 0;
			break;
	}

	close(s_sCMSocketfd);

	CM_DI_Print(5, ("[%s][%d] Downstream SNR (%d) is %d\n", __FUNCTION__, __LINE__, nDsId, *pnDsSnr));

	return DRV_OK;
}

/*****************************************************************************
* function : DRV_CM_Bdsgcc_GetDsBer
* description : Get eCM downstream BER
* input :
*   nDsId : downstream channel index
*   pnDsBer : pointer for storing eCM downstream BER
* return :
*   DRV_OK / DRV_ERR
******************************************************************************/
DRV_Error DRV_CM_Bdsgcc_GetDsBer(HINT32 nDsId, HINT32 *pnDsBer)
{
	*pnDsBer = 0;

	CM_DI_Print(5, ("[%s][%d] Downstream BER (%d) is %d\n", __FUNCTION__, __LINE__, nDsId, *pnDsBer));

	return DRV_OK;
}

/*****************************************************************************
* function : DRV_CM_Bdsgcc_GetDsChannelId
* description : Get eCM downstream channel ID
* input :
*   nDsId : downstream channel index
*   pnDsChId : pointer for storing eCM downstream channel ID
* return :
*   DRV_OK / DRV_ERR
******************************************************************************/
DRV_Error DRV_CM_Bdsgcc_GetDsChannelId(HINT32 nDsId, HINT32 *pnDsChId)
{
	struct sockaddr_in s_sCMservaddr_t;
	int n, s_sCMSocketfd=0;
	struct timeval tv;

	VK_MEM_Memset(&s_sCMservaddr_t, 0, sizeof(s_sCMservaddr_t));

	s_sCMservaddr_t.sin_family = AF_INET;
	s_sCMservaddr_t.sin_port = htons(SNMP_PORT);
	inet_pton(AF_INET, psCmtargetAddr, &s_sCMservaddr_t.sin_addr);

	s_sCMSocketfd = socket(AF_INET, SOCK_DGRAM, 0);

	if (s_sCMSocketfd == -1)
	{
		CM_DI_Print(0, ("[%s][%d] socket error!\n", __FUNCTION__, __LINE__));
		return DRV_ERR;
	}

	tv.tv_sec = 2;
	tv.tv_usec = 0;
	n = setsockopt(s_sCMSocketfd, SOL_SOCKET, SO_SNDTIMEO, &tv, sizeof(tv));
	n = setsockopt(s_sCMSocketfd, SOL_SOCKET, SO_RCVTIMEO, &tv, sizeof(tv));

	switch (nDsId)
	{
		case 0 :
			*pnDsChId = GetInt(s_sCMSocketfd, &s_sCMservaddr_t, sizeof(s_sCMservaddr_t),
								(char *)docsIfDownChannelId1stMsg, sizeof(docsIfDownChannelId1stMsg),
								DOCSIFDOWNCHANNELID30MSG_OFFSET);
			break;
		case 1 :
			*pnDsChId = GetInt(s_sCMSocketfd, &s_sCMservaddr_t, sizeof(s_sCMservaddr_t),
								(char *)docsIfDownChannelId2ndMsg, sizeof(docsIfDownChannelId2ndMsg),
								DOCSIFDOWNCHANNELID30MSG_OFFSET);
			break;
		case 2 :
			*pnDsChId = GetInt(s_sCMSocketfd, &s_sCMservaddr_t, sizeof(s_sCMservaddr_t),
								(char *)docsIfDownChannelId3rdMsg, sizeof(docsIfDownChannelId3rdMsg),
								DOCSIFDOWNCHANNELID30MSG_OFFSET);
			break;
		case 3 :
			*pnDsChId = GetInt(s_sCMSocketfd, &s_sCMservaddr_t, sizeof(s_sCMservaddr_t),
								(char *)docsIfDownChannelId4thMsg, sizeof(docsIfDownChannelId4thMsg),
								DOCSIFDOWNCHANNELID30MSG_OFFSET);
			break;
		case 4 :
			*pnDsChId = GetInt(s_sCMSocketfd, &s_sCMservaddr_t, sizeof(s_sCMservaddr_t),
								(char *)docsIfDownChannelId5thMsg, sizeof(docsIfDownChannelId5thMsg),
								DOCSIFDOWNCHANNELID30MSG_OFFSET);
			break;
		case 5 :
			*pnDsChId = GetInt(s_sCMSocketfd, &s_sCMservaddr_t, sizeof(s_sCMservaddr_t),
								(char *)docsIfDownChannelId6thMsg, sizeof(docsIfDownChannelId6thMsg),
								DOCSIFDOWNCHANNELID30MSG_OFFSET);
			break;
		case 6 :
			*pnDsChId = GetInt(s_sCMSocketfd, &s_sCMservaddr_t, sizeof(s_sCMservaddr_t),
								(char *)docsIfDownChannelId7thMsg, sizeof(docsIfDownChannelId7thMsg),
								DOCSIFDOWNCHANNELID30MSG_OFFSET);
			break;
		case 7 :
			*pnDsChId = GetInt(s_sCMSocketfd, &s_sCMservaddr_t, sizeof(s_sCMservaddr_t),
								(char *)docsIfDownChannelId8thMsg, sizeof(docsIfDownChannelId8thMsg),
								DOCSIFDOWNCHANNELID30MSG_OFFSET);
			break;
		default :
			CM_DI_Print(0, ("[%s][%d] Invalid Down Channel ID (%d)\n", __FUNCTION__, __LINE__, nDsId));
			*pnDsChId = 0;
			break;
	}

	close(s_sCMSocketfd);

	CM_DI_Print(5, ("[%s][%d] Downstream CH ID (%d) is %d\n", __FUNCTION__, __LINE__, nDsId, *pnDsChId));

	return DRV_OK;
}

/*****************************************************************************
* function : DRV_CM_Bdsgcc_GetDsModulation
* description : Get eCM downstream modulation
* input :
*   nDsId : downstream channel index
*   pnDsMod : pointer for storing eCM downstream modulation
* return :
*   DRV_OK / DRV_ERR
******************************************************************************/
DRV_Error DRV_CM_Bdsgcc_GetDsModulation(HINT32 nDsId, HINT32 *pnDsMod)
{
	struct sockaddr_in s_sCMservaddr_t;
	int n, s_sCMSocketfd=0;
	struct timeval tv;

	VK_MEM_Memset(&s_sCMservaddr_t, 0, sizeof(s_sCMservaddr_t));

	s_sCMservaddr_t.sin_family = AF_INET;
	s_sCMservaddr_t.sin_port = htons(SNMP_PORT);
	inet_pton(AF_INET, psCmtargetAddr, &s_sCMservaddr_t.sin_addr);

	s_sCMSocketfd = socket(AF_INET, SOCK_DGRAM, 0);

	if (s_sCMSocketfd == -1)
	{
		CM_DI_Print(0, ("[%s][%d] socket error!\n", __FUNCTION__, __LINE__));
		return DRV_ERR;
	}

	tv.tv_sec = 2;
	tv.tv_usec = 0;
	n = setsockopt(s_sCMSocketfd, SOL_SOCKET, SO_SNDTIMEO, &tv, sizeof(tv));
	n = setsockopt(s_sCMSocketfd, SOL_SOCKET, SO_RCVTIMEO, &tv, sizeof(tv));

	switch (nDsId)
	{
		case 0 :
			*pnDsMod = GetInt(s_sCMSocketfd, &s_sCMservaddr_t, sizeof(s_sCMservaddr_t),
								(char *)docsIfDownChannelModulation1stMsg, sizeof(docsIfDownChannelModulation1stMsg),
								DOCSIFDOWNCHANNELMODULATION30MSG_OFFSET);
			break;
		case 1 :
			*pnDsMod = GetInt(s_sCMSocketfd, &s_sCMservaddr_t, sizeof(s_sCMservaddr_t),
								(char *)docsIfDownChannelModulation2ndMsg, sizeof(docsIfDownChannelModulation2ndMsg),
								DOCSIFDOWNCHANNELMODULATION30MSG_OFFSET);
			break;
		case 2 :
			*pnDsMod = GetInt(s_sCMSocketfd, &s_sCMservaddr_t, sizeof(s_sCMservaddr_t),
								(char *)docsIfDownChannelModulation3rdMsg, sizeof(docsIfDownChannelModulation3rdMsg),
								DOCSIFDOWNCHANNELMODULATION30MSG_OFFSET);
			break;
		case 3 :
			*pnDsMod = GetInt(s_sCMSocketfd, &s_sCMservaddr_t, sizeof(s_sCMservaddr_t),
								(char *)docsIfDownChannelModulation4thMsg, sizeof(docsIfDownChannelModulation4thMsg),
								DOCSIFDOWNCHANNELMODULATION30MSG_OFFSET);
			break;
		case 4 :
			*pnDsMod = GetInt(s_sCMSocketfd, &s_sCMservaddr_t, sizeof(s_sCMservaddr_t),
								(char *)docsIfDownChannelModulation5thMsg, sizeof(docsIfDownChannelModulation5thMsg),
								DOCSIFDOWNCHANNELMODULATION30MSG_OFFSET);
			break;
		case 5 :
			*pnDsMod = GetInt(s_sCMSocketfd, &s_sCMservaddr_t, sizeof(s_sCMservaddr_t),
								(char *)docsIfDownChannelModulation6thMsg, sizeof(docsIfDownChannelModulation6thMsg),
								DOCSIFDOWNCHANNELMODULATION30MSG_OFFSET);
			break;
		case 6 :
			*pnDsMod = GetInt(s_sCMSocketfd, &s_sCMservaddr_t, sizeof(s_sCMservaddr_t),
								(char *)docsIfDownChannelModulation7thMsg, sizeof(docsIfDownChannelModulation7thMsg),
								DOCSIFDOWNCHANNELMODULATION30MSG_OFFSET);
			break;
		case 7 :
			*pnDsMod = GetInt(s_sCMSocketfd, &s_sCMservaddr_t, sizeof(s_sCMservaddr_t),
								(char *)docsIfDownChannelModulation8thMsg, sizeof(docsIfDownChannelModulation8thMsg),
								DOCSIFDOWNCHANNELMODULATION30MSG_OFFSET);
			break;
		default :
			CM_DI_Print(0, ("[%s][%d] Invalid Down Channel ID (%d)\n", __FUNCTION__, __LINE__, nDsId));
			*pnDsMod = 0;
			break;
	}

	close(s_sCMSocketfd);

	CM_DI_Print(5, ("[%s][%d] Downstream Mod (%d) is %d\n", __FUNCTION__, __LINE__, nDsId, *pnDsMod));

	return DRV_OK;
}

/*****************************************************************************
* function : DRV_CM_Bdsgcc_GetDsCorrected
* description : Get eCM downstream corrected error count
* input :
*   nDsId : downstream channel index
*   pnDsCorrected : pointer for storing eCM downstream corrected error count
* return :
*   DRV_OK / DRV_ERR
******************************************************************************/
DRV_Error DRV_CM_Bdsgcc_GetDsCorrected(HINT32 nDsId, HINT32 *pnDsCorrected)
{
	struct sockaddr_in s_sCMservaddr_t;
	int n, s_sCMSocketfd=0;
	struct timeval tv;

	VK_MEM_Memset(&s_sCMservaddr_t, 0, sizeof(s_sCMservaddr_t));

	s_sCMservaddr_t.sin_family = AF_INET;
	s_sCMservaddr_t.sin_port = htons(SNMP_PORT);
	inet_pton(AF_INET, psCmtargetAddr, &s_sCMservaddr_t.sin_addr);

	s_sCMSocketfd = socket(AF_INET, SOCK_DGRAM, 0);

	if (s_sCMSocketfd == -1)
	{
		CM_DI_Print(0, ("[%s][%d] socket error!\n", __FUNCTION__, __LINE__));
		return DRV_ERR;
	}

	tv.tv_sec = 2;
	tv.tv_usec = 0;
	n = setsockopt(s_sCMSocketfd, SOL_SOCKET, SO_SNDTIMEO, &tv, sizeof(tv));
	n = setsockopt(s_sCMSocketfd, SOL_SOCKET, SO_RCVTIMEO, &tv, sizeof(tv));

	switch (nDsId)
	{
		case 0 :
			*pnDsCorrected = GetUInt(s_sCMSocketfd, &s_sCMservaddr_t, sizeof(s_sCMservaddr_t),
								(char *)docsIfSigQCorrecteds1stMsg, sizeof(docsIfSigQCorrecteds1stMsg),
								DOCSIFSIGQCORRECTEDS30MSG_OFFSET);
			break;
		case 1 :
			*pnDsCorrected = GetUInt(s_sCMSocketfd, &s_sCMservaddr_t, sizeof(s_sCMservaddr_t),
								(char *)docsIfSigQCorrecteds2ndMsg, sizeof(docsIfSigQCorrecteds2ndMsg),
								DOCSIFSIGQCORRECTEDS30MSG_OFFSET);
			break;
		case 2 :
			*pnDsCorrected = GetUInt(s_sCMSocketfd, &s_sCMservaddr_t, sizeof(s_sCMservaddr_t),
								(char *)docsIfSigQCorrecteds3rdMsg, sizeof(docsIfSigQCorrecteds3rdMsg),
								DOCSIFSIGQCORRECTEDS30MSG_OFFSET);
			break;
		case 3 :
			*pnDsCorrected = GetUInt(s_sCMSocketfd, &s_sCMservaddr_t, sizeof(s_sCMservaddr_t),
								(char *)docsIfSigQCorrecteds4thMsg, sizeof(docsIfSigQCorrecteds4thMsg),
								DOCSIFSIGQCORRECTEDS30MSG_OFFSET);
			break;
		case 4 :
			*pnDsCorrected = GetUInt(s_sCMSocketfd, &s_sCMservaddr_t, sizeof(s_sCMservaddr_t),
								(char *)docsIfSigQCorrecteds5thMsg, sizeof(docsIfSigQCorrecteds5thMsg),
								DOCSIFSIGQCORRECTEDS30MSG_OFFSET);
			break;
		case 5 :
			*pnDsCorrected = GetUInt(s_sCMSocketfd, &s_sCMservaddr_t, sizeof(s_sCMservaddr_t),
								(char *)docsIfSigQCorrecteds6thMsg, sizeof(docsIfSigQCorrecteds6thMsg),
								DOCSIFSIGQCORRECTEDS30MSG_OFFSET);
			break;
		case 6 :
			*pnDsCorrected = GetUInt(s_sCMSocketfd, &s_sCMservaddr_t, sizeof(s_sCMservaddr_t),
								(char *)docsIfSigQCorrecteds7thMsg, sizeof(docsIfSigQCorrecteds7thMsg),
								DOCSIFSIGQCORRECTEDS30MSG_OFFSET);
			break;
		case 7 :
			*pnDsCorrected = GetUInt(s_sCMSocketfd, &s_sCMservaddr_t, sizeof(s_sCMservaddr_t),
								(char *)docsIfSigQCorrecteds8thMsg, sizeof(docsIfSigQCorrecteds8thMsg),
								DOCSIFSIGQCORRECTEDS30MSG_OFFSET);
			break;
		default :
			CM_DI_Print(0, ("[%s][%d] Invalid Down Channel ID (%d)\n", __FUNCTION__, __LINE__, nDsId));
			*pnDsCorrected = 0;
			break;
	}

	close(s_sCMSocketfd);

	CM_DI_Print(5, ("[%s][%d] Downstream Corrected (%d) is %d\n", __FUNCTION__, __LINE__, nDsId, *pnDsCorrected));

	return DRV_OK;
}

/*****************************************************************************
* function : DRV_CM_Bdsgcc_GetDsUncorrected
* description : Get eCM downstream uncorrected error count
* input :
*   nDsId : downstream channel index
*   pnDsUncorrected : pointer for storing eCM downstream uncorrected error count
* return :
*   DRV_OK / DRV_ERR
******************************************************************************/
DRV_Error DRV_CM_Bdsgcc_GetDsUncorrected(HINT32 nDsId, HINT32 *pnDsUncorrected)
{
	struct sockaddr_in s_sCMservaddr_t;
	int n, s_sCMSocketfd=0;
	struct timeval tv;

	VK_MEM_Memset(&s_sCMservaddr_t, 0, sizeof(s_sCMservaddr_t));

	s_sCMservaddr_t.sin_family = AF_INET;
	s_sCMservaddr_t.sin_port = htons(SNMP_PORT);
	inet_pton(AF_INET, psCmtargetAddr, &s_sCMservaddr_t.sin_addr);

	s_sCMSocketfd = socket(AF_INET, SOCK_DGRAM, 0);

	if (s_sCMSocketfd == -1)
	{
		CM_DI_Print(0, ("[%s][%d] socket error!\n", __FUNCTION__, __LINE__));
		return DRV_ERR;
	}

	tv.tv_sec = 2;
	tv.tv_usec = 0;
	n = setsockopt(s_sCMSocketfd, SOL_SOCKET, SO_SNDTIMEO, &tv, sizeof(tv));
	n = setsockopt(s_sCMSocketfd, SOL_SOCKET, SO_RCVTIMEO, &tv, sizeof(tv));

	switch (nDsId)
	{
		case 0 :
			*pnDsUncorrected = GetUInt(s_sCMSocketfd, &s_sCMservaddr_t, sizeof(s_sCMservaddr_t),
								(char *)docsIfSigQUncorrectables1stMsg, sizeof(docsIfSigQUncorrectables1stMsg),
								DOCSIFSIGQUNCORRECTABLES30MSG_OFFSET);
			break;
		case 1 :
			*pnDsUncorrected = GetUInt(s_sCMSocketfd, &s_sCMservaddr_t, sizeof(s_sCMservaddr_t),
								(char *)docsIfSigQUncorrectables2ndMsg, sizeof(docsIfSigQUncorrectables2ndMsg),
								DOCSIFSIGQUNCORRECTABLES30MSG_OFFSET);
			break;
		case 2 :
			*pnDsUncorrected = GetUInt(s_sCMSocketfd, &s_sCMservaddr_t, sizeof(s_sCMservaddr_t),
								(char *)docsIfSigQUncorrectables3rdMsg, sizeof(docsIfSigQUncorrectables3rdMsg),
								DOCSIFSIGQUNCORRECTABLES30MSG_OFFSET);
			break;
		case 3 :
			*pnDsUncorrected = GetUInt(s_sCMSocketfd, &s_sCMservaddr_t, sizeof(s_sCMservaddr_t),
								(char *)docsIfSigQUncorrectables4thMsg, sizeof(docsIfSigQUncorrectables4thMsg),
								DOCSIFSIGQUNCORRECTABLES30MSG_OFFSET);
			break;
		case 4 :
			*pnDsUncorrected = GetUInt(s_sCMSocketfd, &s_sCMservaddr_t, sizeof(s_sCMservaddr_t),
								(char *)docsIfSigQUncorrectables5thMsg, sizeof(docsIfSigQUncorrectables5thMsg),
								DOCSIFSIGQUNCORRECTABLES30MSG_OFFSET);
			break;
		case 5 :
			*pnDsUncorrected = GetUInt(s_sCMSocketfd, &s_sCMservaddr_t, sizeof(s_sCMservaddr_t),
								(char *)docsIfSigQUncorrectables6thMsg, sizeof(docsIfSigQUncorrectables6thMsg),
								DOCSIFSIGQUNCORRECTABLES30MSG_OFFSET);
			break;
		case 6 :
			*pnDsUncorrected = GetUInt(s_sCMSocketfd, &s_sCMservaddr_t, sizeof(s_sCMservaddr_t),
								(char *)docsIfSigQUncorrectables7thMsg, sizeof(docsIfSigQUncorrectables7thMsg),
								DOCSIFSIGQUNCORRECTABLES30MSG_OFFSET);
			break;
		case 7 :
			*pnDsUncorrected = GetUInt(s_sCMSocketfd, &s_sCMservaddr_t, sizeof(s_sCMservaddr_t),
								(char *)docsIfSigQUncorrectables8thMsg, sizeof(docsIfSigQUncorrectables8thMsg),
								DOCSIFSIGQUNCORRECTABLES30MSG_OFFSET);
			break;
		default :
			CM_DI_Print(0, ("[%s][%d] Invalid Down Channel ID (%d)\n", __FUNCTION__, __LINE__, nDsId));
			*pnDsUncorrected = 0;
			break;
	}

	close(s_sCMSocketfd);

	CM_DI_Print(5, ("[%s][%d] Downstream Uncorrected (%d) is %d\n", __FUNCTION__, __LINE__, nDsId, *pnDsUncorrected));

	return DRV_OK;
}

/*****************************************************************************
* function : DRV_CM_Bdsgcc_GetDsSignalInfo
* description : Get eCM downstream signal information
* input :
*   nDsId : downstream channel index
*   pstDsInfo : pointer for storing eCM downstream signal information
* return :
*   DRV_OK / DRV_ERR
******************************************************************************/
DRV_Error DRV_CM_Bdsgcc_GetDsSignalInfo(HINT32 nDsId, DI_CM_DsSignalQuality_t *pstDsInfo)
{
	int nCmstatus = 0;
	HUINT32  ulUCB[8]={0,}, ulFDCBer[8]={0,};
	HINT32 lFDCPower[8]={0,}, lFDCMod[8]={0,}, lFDCSnr[8]={0,}, ulFDCFreq[8]={0,};
	HUINT8	FDCMod[10]={0,};

	struct sockaddr_in s_sCMservaddr_t;
	int n, s_sCMSocketfd=0;
	struct timeval tv;

	CM_DI_Print(6, ("[%s][%d] ---- \n ", __FUNCTION__, __LINE__));

	if ( pstDsInfo == NULL )
	{
		CM_DI_Print(0, ("[%s][%d] ERR! \n", __FUNCTION__, __LINE__));
		return DRV_ERR;
	}

	if (  nDsId >= 8 )
	{
		CM_DI_Print(0, ("[%s][%d] ERR! nDsId(%d) \n", __FUNCTION__, __LINE__, nDsId));
		return DRV_ERR;
	}

	VK_MEM_Memset(&s_sCMservaddr_t, 0, sizeof(s_sCMservaddr_t));

	s_sCMservaddr_t.sin_family = AF_INET;
	s_sCMservaddr_t.sin_port = htons(SNMP_PORT);
	inet_pton(AF_INET, psCmtargetAddr, &s_sCMservaddr_t.sin_addr);

	s_sCMSocketfd = socket(AF_INET, SOCK_DGRAM, 0);
	if (s_sCMSocketfd == -1)
	{
		CM_DI_Print(0, ("[%s][%d] socket error!\n", __FUNCTION__, __LINE__));
		return DRV_ERR;
	}

	tv.tv_sec = 2;
	tv.tv_usec = 0;
	n = setsockopt(s_sCMSocketfd, SOL_SOCKET, SO_SNDTIMEO, &tv, sizeof(tv));
	n = setsockopt(s_sCMSocketfd, SOL_SOCKET, SO_RCVTIMEO, &tv, sizeof(tv));

	nCmstatus = GetInt(s_sCMSocketfd, &s_sCMservaddr_t, sizeof(s_sCMservaddr_t),
				(char *)docsIfCmStatusValueMsg, sizeof(docsIfCmStatusValueMsg),
				DOCSIFCMSTATUSVALUEMSG_OFFSET);

	CM_DI_Print(5, ("[%s][%d] CM Status - %d\n", __FUNCTION__, __LINE__, nCmstatus));

	if(nCmstatus <= 4)
	{
		switch (nDsId)
		{
			case 0 :
				ulFDCFreq[nDsId] = GetInt(s_sCMSocketfd, &s_sCMservaddr_t, sizeof(s_sCMservaddr_t),
									(char *)docsIfDownChannelFrequency1stMsg, sizeof(docsIfDownChannelFrequency1stMsg),
									DOCSIFDOWNCHANNELFREQUENCY30MSG_OFFSET);
				break;

			case 1 :
				ulFDCFreq[nDsId] = GetInt(s_sCMSocketfd, &s_sCMservaddr_t, sizeof(s_sCMservaddr_t),
									(char *)docsIfDownChannelFrequency2ndMsg, sizeof(docsIfDownChannelFrequency2ndMsg),
									DOCSIFDOWNCHANNELFREQUENCY30MSG_OFFSET);
				break;

			case 2 :
				ulFDCFreq[nDsId] = GetInt(s_sCMSocketfd, &s_sCMservaddr_t, sizeof(s_sCMservaddr_t),
									(char *)docsIfDownChannelFrequency3rdMsg, sizeof(docsIfDownChannelFrequency3rdMsg),
									DOCSIFDOWNCHANNELFREQUENCY30MSG_OFFSET);
				break;

			case 3 :
				ulFDCFreq[nDsId] = GetInt(s_sCMSocketfd, &s_sCMservaddr_t, sizeof(s_sCMservaddr_t),
									(char *)docsIfDownChannelFrequency4thMsg, sizeof(docsIfDownChannelFrequency4thMsg),
									DOCSIFDOWNCHANNELFREQUENCY30MSG_OFFSET);
				break;

			case 4 :
				ulFDCFreq[nDsId] = GetInt(s_sCMSocketfd, &s_sCMservaddr_t, sizeof(s_sCMservaddr_t),
									(char *)docsIfDownChannelFrequency5thMsg, sizeof(docsIfDownChannelFrequency5thMsg),
									DOCSIFDOWNCHANNELFREQUENCY30MSG_OFFSET);
				break;

			case 5 :
				ulFDCFreq[nDsId] = GetInt(s_sCMSocketfd, &s_sCMservaddr_t, sizeof(s_sCMservaddr_t),
									(char *)docsIfDownChannelFrequency6thMsg, sizeof(docsIfDownChannelFrequency6thMsg),
									DOCSIFDOWNCHANNELFREQUENCY30MSG_OFFSET);
				break;

			case 6 :
				ulFDCFreq[nDsId] = GetInt(s_sCMSocketfd, &s_sCMservaddr_t, sizeof(s_sCMservaddr_t),
									(char *)docsIfDownChannelFrequency7thMsg, sizeof(docsIfDownChannelFrequency7thMsg),
									DOCSIFDOWNCHANNELFREQUENCY30MSG_OFFSET);
				break;

			case 7 :
				ulFDCFreq[nDsId] = GetInt(s_sCMSocketfd, &s_sCMservaddr_t, sizeof(s_sCMservaddr_t),
									(char *)docsIfDownChannelFrequency8thMsg, sizeof(docsIfDownChannelFrequency8thMsg),
									DOCSIFDOWNCHANNELFREQUENCY30MSG_OFFSET);
				break;

			default :
				CM_DI_Print(0, ("[%s][%d] Invalid Down Channel ID (%d)\n", __FUNCTION__, __LINE__, nDsId));
				ulFDCFreq[nDsId] = 0;
				break;
		}

		close(s_sCMSocketfd);

		pstDsInfo->ulDsFrequency = ulFDCFreq[nDsId];

		pstDsInfo->ulDsModulation = 0;
		pstDsInfo->fDsInputPower = 0.0;	/* dBuV*/
		pstDsInfo->fDsSnr = 0.0;
		pstDsInfo->fDsBer = 0;
		pstDsInfo->ulDsUnCorrectedNo = 0;
		pstDsInfo->ulDsStrength = 0;
	}
	else
	{
		switch (nDsId)
		{
			case 0 :
				ulFDCFreq[nDsId] = GetInt(s_sCMSocketfd, &s_sCMservaddr_t, sizeof(s_sCMservaddr_t),
									(char *)docsIfDownChannelFrequency1stMsg, sizeof(docsIfDownChannelFrequency1stMsg),
									DOCSIFDOWNCHANNELFREQUENCY30MSG_OFFSET);
				lFDCMod[nDsId] = GetInt(s_sCMSocketfd, &s_sCMservaddr_t, sizeof(s_sCMservaddr_t),
								(char *)docsIfDownChannelModulation1stMsg, sizeof(docsIfDownChannelModulation1stMsg),
								DOCSIFDOWNCHANNELMODULATION30MSG_OFFSET);
				lFDCPower[nDsId] = GetInt(s_sCMSocketfd, &s_sCMservaddr_t, sizeof(s_sCMservaddr_t),
								(char *)docsIfDownChannelPower1stMsg, sizeof(docsIfDownChannelPower1stMsg),
								DOCSIFDOWNCHANNELPOWER30MSG_OFFSET);
				lFDCSnr[nDsId] = GetInt(s_sCMSocketfd, &s_sCMservaddr_t, sizeof(s_sCMservaddr_t),
								(char *)docsIfSigQSignalNoise1stMsg, sizeof(docsIfSigQSignalNoise1stMsg),
								DOCSIFSIGQSIGNALNOISE30MSG_OFFSET);
				ulUCB[nDsId] = GetUInt(s_sCMSocketfd, &s_sCMservaddr_t, sizeof(s_sCMservaddr_t),
								(char *)docsIfSigQUncorrectables1stMsg, sizeof(docsIfSigQUncorrectables1stMsg),
								DOCSIFSIGQUNCORRECTABLES30MSG_OFFSET);
				break;

			case 1 :
				ulFDCFreq[nDsId] = GetInt(s_sCMSocketfd, &s_sCMservaddr_t, sizeof(s_sCMservaddr_t),
									(char *)docsIfDownChannelFrequency2ndMsg, sizeof(docsIfDownChannelFrequency2ndMsg),
									DOCSIFDOWNCHANNELFREQUENCY30MSG_OFFSET);
				lFDCMod[nDsId] = GetInt(s_sCMSocketfd, &s_sCMservaddr_t, sizeof(s_sCMservaddr_t),
								(char *)docsIfDownChannelModulation2ndMsg, sizeof(docsIfDownChannelModulation2ndMsg),
								DOCSIFDOWNCHANNELMODULATION30MSG_OFFSET);
				lFDCPower[nDsId] = GetInt(s_sCMSocketfd, &s_sCMservaddr_t, sizeof(s_sCMservaddr_t),
								(char *)docsIfDownChannelPower2ndMsg, sizeof(docsIfDownChannelPower2ndMsg),
								DOCSIFDOWNCHANNELPOWER30MSG_OFFSET);
				lFDCSnr[nDsId] = GetInt(s_sCMSocketfd, &s_sCMservaddr_t, sizeof(s_sCMservaddr_t),
								(char *)docsIfSigQSignalNoise2ndMsg, sizeof(docsIfSigQSignalNoise2ndMsg),
								DOCSIFSIGQSIGNALNOISE30MSG_OFFSET);
				ulUCB[nDsId] = GetUInt(s_sCMSocketfd, &s_sCMservaddr_t, sizeof(s_sCMservaddr_t),
								(char *)docsIfSigQUncorrectables2ndMsg, sizeof(docsIfSigQUncorrectables2ndMsg),
								DOCSIFSIGQUNCORRECTABLES30MSG_OFFSET);
				break;

			case 2 :
				ulFDCFreq[nDsId] = GetInt(s_sCMSocketfd, &s_sCMservaddr_t, sizeof(s_sCMservaddr_t),
									(char *)docsIfDownChannelFrequency3rdMsg, sizeof(docsIfDownChannelFrequency3rdMsg),
									DOCSIFDOWNCHANNELFREQUENCY30MSG_OFFSET);
				lFDCMod[nDsId] = GetInt(s_sCMSocketfd, &s_sCMservaddr_t, sizeof(s_sCMservaddr_t),
								(char *)docsIfDownChannelModulation3rdMsg, sizeof(docsIfDownChannelModulation3rdMsg),
								DOCSIFDOWNCHANNELMODULATION30MSG_OFFSET);
				lFDCPower[nDsId] = GetInt(s_sCMSocketfd, &s_sCMservaddr_t, sizeof(s_sCMservaddr_t),
								(char *)docsIfDownChannelPower3rdMsg, sizeof(docsIfDownChannelPower3rdMsg),
								DOCSIFDOWNCHANNELPOWER30MSG_OFFSET);
				lFDCSnr[nDsId] = GetInt(s_sCMSocketfd, &s_sCMservaddr_t, sizeof(s_sCMservaddr_t),
								(char *)docsIfSigQSignalNoise3rdMsg, sizeof(docsIfSigQSignalNoise3rdMsg),
								DOCSIFSIGQSIGNALNOISE30MSG_OFFSET);
				ulUCB[nDsId] = GetUInt(s_sCMSocketfd, &s_sCMservaddr_t, sizeof(s_sCMservaddr_t),
								(char *)docsIfSigQUncorrectables3rdMsg, sizeof(docsIfSigQUncorrectables3rdMsg),
								DOCSIFSIGQUNCORRECTABLES30MSG_OFFSET);
				break;

			case 3 :
				ulFDCFreq[nDsId] = GetInt(s_sCMSocketfd, &s_sCMservaddr_t, sizeof(s_sCMservaddr_t),
									(char *)docsIfDownChannelFrequency4thMsg, sizeof(docsIfDownChannelFrequency4thMsg),
									DOCSIFDOWNCHANNELFREQUENCY30MSG_OFFSET);
				lFDCMod[nDsId] = GetInt(s_sCMSocketfd, &s_sCMservaddr_t, sizeof(s_sCMservaddr_t),
								(char *)docsIfDownChannelModulation4thMsg, sizeof(docsIfDownChannelModulation4thMsg),
								DOCSIFDOWNCHANNELMODULATION30MSG_OFFSET);
				lFDCPower[nDsId] = GetInt(s_sCMSocketfd, &s_sCMservaddr_t, sizeof(s_sCMservaddr_t),
								(char *)docsIfDownChannelPower4thMsg, sizeof(docsIfDownChannelPower4thMsg),
								DOCSIFDOWNCHANNELPOWER30MSG_OFFSET);
				lFDCSnr[nDsId] = GetInt(s_sCMSocketfd, &s_sCMservaddr_t, sizeof(s_sCMservaddr_t),
								(char *)docsIfSigQSignalNoise4thMsg, sizeof(docsIfSigQSignalNoise4thMsg),
								DOCSIFSIGQSIGNALNOISE30MSG_OFFSET);
				ulUCB[nDsId] = GetUInt(s_sCMSocketfd, &s_sCMservaddr_t, sizeof(s_sCMservaddr_t),
								(char *)docsIfSigQUncorrectables4thMsg, sizeof(docsIfSigQUncorrectables4thMsg),
								DOCSIFSIGQUNCORRECTABLES30MSG_OFFSET);
				break;

			case 4 :
				ulFDCFreq[nDsId] = GetInt(s_sCMSocketfd, &s_sCMservaddr_t, sizeof(s_sCMservaddr_t),
									(char *)docsIfDownChannelFrequency5thMsg, sizeof(docsIfDownChannelFrequency5thMsg),
									DOCSIFDOWNCHANNELFREQUENCY30MSG_OFFSET);
				lFDCMod[nDsId] = GetInt(s_sCMSocketfd, &s_sCMservaddr_t, sizeof(s_sCMservaddr_t),
								(char *)docsIfDownChannelModulation5thMsg, sizeof(docsIfDownChannelModulation5thMsg),
								DOCSIFDOWNCHANNELMODULATION30MSG_OFFSET);
				lFDCPower[nDsId] = GetInt(s_sCMSocketfd, &s_sCMservaddr_t, sizeof(s_sCMservaddr_t),
								(char *)docsIfDownChannelPower5thMsg, sizeof(docsIfDownChannelPower5thMsg),
								DOCSIFDOWNCHANNELPOWER30MSG_OFFSET);
				lFDCSnr[nDsId] = GetInt(s_sCMSocketfd, &s_sCMservaddr_t, sizeof(s_sCMservaddr_t),
								(char *)docsIfSigQSignalNoise5thMsg, sizeof(docsIfSigQSignalNoise5thMsg),
								DOCSIFSIGQSIGNALNOISE30MSG_OFFSET);
				ulUCB[nDsId] = GetUInt(s_sCMSocketfd, &s_sCMservaddr_t, sizeof(s_sCMservaddr_t),
								(char *)docsIfSigQUncorrectables5thMsg, sizeof(docsIfSigQUncorrectables5thMsg),
								DOCSIFSIGQUNCORRECTABLES30MSG_OFFSET);
				break;

			case 5 :
				ulFDCFreq[nDsId] = GetInt(s_sCMSocketfd, &s_sCMservaddr_t, sizeof(s_sCMservaddr_t),
									(char *)docsIfDownChannelFrequency6thMsg, sizeof(docsIfDownChannelFrequency6thMsg),
									DOCSIFDOWNCHANNELFREQUENCY30MSG_OFFSET);
				lFDCMod[nDsId] = GetInt(s_sCMSocketfd, &s_sCMservaddr_t, sizeof(s_sCMservaddr_t),
								(char *)docsIfDownChannelModulation6thMsg, sizeof(docsIfDownChannelModulation6thMsg),
								DOCSIFDOWNCHANNELMODULATION30MSG_OFFSET);
				lFDCPower[nDsId] = GetInt(s_sCMSocketfd, &s_sCMservaddr_t, sizeof(s_sCMservaddr_t),
								(char *)docsIfDownChannelPower6thMsg, sizeof(docsIfDownChannelPower6thMsg),
								DOCSIFDOWNCHANNELPOWER30MSG_OFFSET);
				lFDCSnr[nDsId] = GetInt(s_sCMSocketfd, &s_sCMservaddr_t, sizeof(s_sCMservaddr_t),
								(char *)docsIfSigQSignalNoise6thMsg, sizeof(docsIfSigQSignalNoise6thMsg),
								DOCSIFSIGQSIGNALNOISE30MSG_OFFSET);
				ulUCB[nDsId] = GetUInt(s_sCMSocketfd, &s_sCMservaddr_t, sizeof(s_sCMservaddr_t),
								(char *)docsIfSigQUncorrectables6thMsg, sizeof(docsIfSigQUncorrectables6thMsg),
								DOCSIFSIGQUNCORRECTABLES30MSG_OFFSET);
				break;

			case 6 :
				ulFDCFreq[nDsId] = GetInt(s_sCMSocketfd, &s_sCMservaddr_t, sizeof(s_sCMservaddr_t),
									(char *)docsIfDownChannelFrequency7thMsg, sizeof(docsIfDownChannelFrequency7thMsg),
									DOCSIFDOWNCHANNELFREQUENCY30MSG_OFFSET);
				lFDCMod[nDsId] = GetInt(s_sCMSocketfd, &s_sCMservaddr_t, sizeof(s_sCMservaddr_t),
								(char *)docsIfDownChannelModulation7thMsg, sizeof(docsIfDownChannelModulation7thMsg),
								DOCSIFDOWNCHANNELMODULATION30MSG_OFFSET);
				lFDCPower[nDsId] = GetInt(s_sCMSocketfd, &s_sCMservaddr_t, sizeof(s_sCMservaddr_t),
								(char *)docsIfDownChannelPower7thMsg, sizeof(docsIfDownChannelPower7thMsg),
								DOCSIFDOWNCHANNELPOWER30MSG_OFFSET);
				lFDCSnr[nDsId] = GetInt(s_sCMSocketfd, &s_sCMservaddr_t, sizeof(s_sCMservaddr_t),
								(char *)docsIfSigQSignalNoise7thMsg, sizeof(docsIfSigQSignalNoise7thMsg),
								DOCSIFSIGQSIGNALNOISE30MSG_OFFSET);
				ulUCB[nDsId] = GetUInt(s_sCMSocketfd, &s_sCMservaddr_t, sizeof(s_sCMservaddr_t),
								(char *)docsIfSigQUncorrectables7thMsg, sizeof(docsIfSigQUncorrectables7thMsg),
								DOCSIFSIGQUNCORRECTABLES30MSG_OFFSET);
				break;

			case 7 :
				ulFDCFreq[nDsId] = GetInt(s_sCMSocketfd, &s_sCMservaddr_t, sizeof(s_sCMservaddr_t),
									(char *)docsIfDownChannelFrequency8thMsg, sizeof(docsIfDownChannelFrequency8thMsg),
									DOCSIFDOWNCHANNELFREQUENCY30MSG_OFFSET);
				lFDCMod[nDsId] = GetInt(s_sCMSocketfd, &s_sCMservaddr_t, sizeof(s_sCMservaddr_t),
								(char *)docsIfDownChannelModulation8thMsg, sizeof(docsIfDownChannelModulation8thMsg),
								DOCSIFDOWNCHANNELMODULATION30MSG_OFFSET);
				lFDCPower[nDsId] = GetInt(s_sCMSocketfd, &s_sCMservaddr_t, sizeof(s_sCMservaddr_t),
								(char *)docsIfDownChannelPower8thMsg, sizeof(docsIfDownChannelPower8thMsg),
								DOCSIFDOWNCHANNELPOWER30MSG_OFFSET);
				lFDCSnr[nDsId] = GetInt(s_sCMSocketfd, &s_sCMservaddr_t, sizeof(s_sCMservaddr_t),
								(char *)docsIfSigQSignalNoise8thMsg, sizeof(docsIfSigQSignalNoise8thMsg),
								DOCSIFSIGQSIGNALNOISE30MSG_OFFSET);
				ulUCB[nDsId] = GetUInt(s_sCMSocketfd, &s_sCMservaddr_t, sizeof(s_sCMservaddr_t),
								(char *)docsIfSigQUncorrectables8thMsg, sizeof(docsIfSigQUncorrectables8thMsg),
								DOCSIFSIGQUNCORRECTABLES30MSG_OFFSET);
				break;

			default :
				CM_DI_Print(0, ("[%s][%d] Invalid Down Channel ID (%d)\n", __FUNCTION__, __LINE__, nDsId));
				break;
		}

		close(s_sCMSocketfd);

		pstDsInfo->ulDsFrequency = ulFDCFreq[nDsId];
		pstDsInfo->ulDsModulation = lFDCMod[nDsId];
		pstDsInfo->fDsInputPower = (float)(lFDCPower[nDsId] / 10.0)+60.0;	/* dBuV*/
		pstDsInfo->fDsSnr = (float)(lFDCSnr[nDsId]/10.0);
		pstDsInfo->fDsBer = (float) (ulFDCBer[nDsId]/1000000);
		pstDsInfo->ulDsUnCorrectedNo = ulUCB[nDsId];

		if(pstDsInfo->fDsInputPower >= 75)
		{
			pstDsInfo->ulDsStrength = 100;
		}
		else if(pstDsInfo->fDsInputPower >= 65)
		{
			pstDsInfo->ulDsStrength =  90 + (HUINT32)(pstDsInfo->fDsInputPower - 65.0) ;
		}
		else if(pstDsInfo->fDsInputPower >= 50)
		{
			pstDsInfo->ulDsStrength = 60 + (HUINT32)((pstDsInfo->fDsInputPower - 50.0)*2.0) ;
		}
		else if(pstDsInfo->fDsInputPower >= 45)
		{
			pstDsInfo->ulDsStrength = 10 + (HUINT32)((pstDsInfo->fDsInputPower - 45.0)*10.0) ;
		}
		else
		{
			pstDsInfo->ulDsStrength = 10;
		}
	}

	switch (pstDsInfo->ulDsModulation)
	{
		case DI_CM_FDC_MOD_ERROR :
			VK_snprintf(FDCMod, sizeof(FDCMod), "Error");
			break;
		case DI_CM_FDC_MOD_UNKNOWN :
			VK_snprintf(FDCMod, sizeof(FDCMod), "Unknown");
			break;
		case DI_CM_FDC_MOD_OTHER :
			VK_snprintf(FDCMod, sizeof(FDCMod), "Other");
			break;
		case DI_CM_FDC_MOD_QAM64 :
			VK_snprintf(FDCMod, sizeof(FDCMod), "64-QAM");
			break;
		case DI_CM_FDC_MOD_QAM256 :
			VK_snprintf(FDCMod, sizeof(FDCMod), "256-QAM");
			break;
		default :
			VK_snprintf(FDCMod, sizeof(FDCMod), "Error");
			break;
	}

	CM_DI_Print(1,("[%s][%d] nDsId = %d\n", __FUNCTION__, __LINE__, nDsId));
	CM_DI_Print(1,("======================================================================\n"));
	CM_DI_Print(1,("  D/S Freq [Hz]     = %u\n", pstDsInfo->ulDsFrequency));
	CM_DI_Print(1,("  D/S Power [dBuV]  = %3.1f\n", pstDsInfo->fDsInputPower));
	CM_DI_Print(1,("  D/S Modulation    = %s\n",FDCMod));
	CM_DI_Print(1,("  D/S BER           = %f\n", pstDsInfo->fDsBer));
	CM_DI_Print(1,("  D/S SNR [dB]      = %3.1f\n", pstDsInfo->fDsSnr));
	CM_DI_Print(1,("  D/S UCB           = %u\n", pstDsInfo->ulDsUnCorrectedNo));
	CM_DI_Print(1,("  D/S Strength [%%]  = %u\n", pstDsInfo->ulDsStrength));
	CM_DI_Print(1,("======================================================================\n"));

	return DRV_OK;
}

/*****************************************************************************
* function : DRV_CM_Bdsgcc_GetUsFreq
* description : Get eCM upstream frequency
* input :
*   nUsId : upstream channel index
*   pnUsFreq : pointer for storing eCM upstream frequency
* return :
*   DRV_OK / DRV_ERR
******************************************************************************/
DRV_Error DRV_CM_Bdsgcc_GetUsFreq(HINT32 nUsId, HINT32 *pnUsFreq)
{
	struct sockaddr_in s_sCMservaddr_t;
	int n, s_sCMSocketfd=0;
	struct timeval tv;

	VK_MEM_Memset(&s_sCMservaddr_t, 0, sizeof(s_sCMservaddr_t));

	s_sCMservaddr_t.sin_family = AF_INET;
	s_sCMservaddr_t.sin_port = htons(SNMP_PORT);
	inet_pton(AF_INET, psCmtargetAddr, &s_sCMservaddr_t.sin_addr);

	s_sCMSocketfd = socket(AF_INET, SOCK_DGRAM, 0);

	if (s_sCMSocketfd == -1)
	{
		CM_DI_Print(0, ("[%s][%d] socket error!\n", __FUNCTION__, __LINE__));
		return DRV_ERR;
	}

	tv.tv_sec = 2;
	tv.tv_usec = 0;
	n = setsockopt(s_sCMSocketfd, SOL_SOCKET, SO_SNDTIMEO, &tv, sizeof(tv));
	n = setsockopt(s_sCMSocketfd, SOL_SOCKET, SO_RCVTIMEO, &tv, sizeof(tv));

	switch (nUsId)
	{
		case 0 :
			*pnUsFreq = GetInt(s_sCMSocketfd, &s_sCMservaddr_t, sizeof(s_sCMservaddr_t),
								(char *)docsIfUpChannelFrequency1stMsg, sizeof(docsIfUpChannelFrequency1stMsg),
								DOCSIFUPCHANNELFREQUENCY30MSG_OFFSET);
			break;
		case 1 :
			*pnUsFreq = GetInt(s_sCMSocketfd, &s_sCMservaddr_t, sizeof(s_sCMservaddr_t),
								(char *)docsIfUpChannelFrequency2ndMsg, sizeof(docsIfUpChannelFrequency2ndMsg),
								DOCSIFUPCHANNELFREQUENCY30MSG_OFFSET);
			break;
		case 2 :
			*pnUsFreq = GetInt(s_sCMSocketfd, &s_sCMservaddr_t, sizeof(s_sCMservaddr_t),
								(char *)docsIfUpChannelFrequency3rdMsg, sizeof(docsIfUpChannelFrequency3rdMsg),
								DOCSIFUPCHANNELFREQUENCY30MSG_OFFSET);
			break;
		case 3 :
			*pnUsFreq = GetInt(s_sCMSocketfd, &s_sCMservaddr_t, sizeof(s_sCMservaddr_t),
								(char *)docsIfUpChannelFrequency4thMsg, sizeof(docsIfUpChannelFrequency4thMsg),
								DOCSIFUPCHANNELFREQUENCY30MSG_OFFSET);
			break;
		default :
			CM_DI_Print(0, ("[%s][%d] Invalid Up Channel ID (%d)\n", __FUNCTION__, __LINE__, nUsId));
			*pnUsFreq = 0;
			break;
	}

	close(s_sCMSocketfd);

	CM_DI_Print(5, ("[%s][%d] Upstream Freq (%d) is %d\n", __FUNCTION__, __LINE__, nUsId, *pnUsFreq));

	return DRV_OK;
}

/*****************************************************************************
* function : DRV_CM_Bdsgcc_GetUsPower
* description : Get eCM upstream RF power level
* input :
*   nUsId : upstream channel index
*   pnUsPower : pointer for storing eCM upstream RF power level
* return :
*   DRV_OK / DRV_ERR
******************************************************************************/
DRV_Error DRV_CM_Bdsgcc_GetUsPower(HINT32 nUsId, HINT32 *pnUsPower)
{
	struct sockaddr_in s_sCMservaddr_t;
	int n, s_sCMSocketfd=0;
	struct timeval tv;

	VK_MEM_Memset(&s_sCMservaddr_t, 0, sizeof(s_sCMservaddr_t));

	s_sCMservaddr_t.sin_family = AF_INET;
	s_sCMservaddr_t.sin_port = htons(SNMP_PORT);
	inet_pton(AF_INET, psCmtargetAddr, &s_sCMservaddr_t.sin_addr);

	s_sCMSocketfd = socket(AF_INET, SOCK_DGRAM, 0);

	if (s_sCMSocketfd == -1)
	{
		CM_DI_Print(0, ("[%s][%d] socket error!\n", __FUNCTION__, __LINE__));
		return DRV_ERR;
	}

	tv.tv_sec = 2;
	tv.tv_usec = 0;
	n = setsockopt(s_sCMSocketfd, SOL_SOCKET, SO_SNDTIMEO, &tv, sizeof(tv));
	n = setsockopt(s_sCMSocketfd, SOL_SOCKET, SO_RCVTIMEO, &tv, sizeof(tv));

	switch (nUsId)
	{
		case 0 :
			*pnUsPower = GetInt(s_sCMSocketfd, &s_sCMservaddr_t, sizeof(s_sCMservaddr_t),
								(char *)docsIf3CmStatusUsTxPower1stMsg, sizeof(docsIf3CmStatusUsTxPower1stMsg),
								DOCSIF3CMSTATUSUSTXPOWER30MSG_OFFSET);
			break;
		case 1 :
			*pnUsPower = GetInt(s_sCMSocketfd, &s_sCMservaddr_t, sizeof(s_sCMservaddr_t),
								(char *)docsIf3CmStatusUsTxPower2ndMsg, sizeof(docsIf3CmStatusUsTxPower2ndMsg),
								DOCSIF3CMSTATUSUSTXPOWER30MSG_OFFSET);
			break;
		case 2 :
			*pnUsPower = GetInt(s_sCMSocketfd, &s_sCMservaddr_t, sizeof(s_sCMservaddr_t),
								(char *)docsIf3CmStatusUsTxPower3rdMsg, sizeof(docsIf3CmStatusUsTxPower3rdMsg),
								DOCSIF3CMSTATUSUSTXPOWER30MSG_OFFSET);
			break;
		case 3 :
			*pnUsPower = GetInt(s_sCMSocketfd, &s_sCMservaddr_t, sizeof(s_sCMservaddr_t),
								(char *)docsIf3CmStatusUsTxPower4thMsg, sizeof(docsIf3CmStatusUsTxPower4thMsg),
								DOCSIF3CMSTATUSUSTXPOWER30MSG_OFFSET);
			break;
		default :
			CM_DI_Print(0, ("[%s][%d] Invalid Up Channel ID (%d)\n", __FUNCTION__, __LINE__, nUsId));
			*pnUsPower = 0;
			break;
	}

	close(s_sCMSocketfd);

	CM_DI_Print(5, ("[%s][%d] Upstream Power (%d) is %d\n", __FUNCTION__, __LINE__, nUsId, *pnUsPower));

	return DRV_OK;
}

/*****************************************************************************
* function : DRV_CM_Bdsgcc_GetUsRangingStatus
* description : Get eCM upstream ranging status
* input :
*   nUsId : upstream channel index
*   pnUsRanging : pointer for storing eCM upstream ranging status
* return :
*   DRV_OK / DRV_ERR
******************************************************************************/
DRV_Error DRV_CM_Bdsgcc_GetUsRangingStatus(HINT32 nUsId, HINT32 *pnUsRanging)
{
	struct sockaddr_in s_sCMservaddr_t;
	int n, s_sCMSocketfd=0;
	struct timeval tv;

	VK_MEM_Memset(&s_sCMservaddr_t, 0, sizeof(s_sCMservaddr_t));

	s_sCMservaddr_t.sin_family = AF_INET;
	s_sCMservaddr_t.sin_port = htons(SNMP_PORT);
	inet_pton(AF_INET, psCmtargetAddr, &s_sCMservaddr_t.sin_addr);

	s_sCMSocketfd = socket(AF_INET, SOCK_DGRAM, 0);

	if (s_sCMSocketfd == -1)
	{
		CM_DI_Print(0, ("[%s][%d] socket error!\n", __FUNCTION__, __LINE__));
		return DRV_ERR;
	}

	tv.tv_sec = 2;
	tv.tv_usec = 0;
	n = setsockopt(s_sCMSocketfd, SOL_SOCKET, SO_SNDTIMEO, &tv, sizeof(tv));
	n = setsockopt(s_sCMSocketfd, SOL_SOCKET, SO_RCVTIMEO, &tv, sizeof(tv));

	switch (nUsId)
	{
		case 0 :
			*pnUsRanging = GetInt(s_sCMSocketfd, &s_sCMservaddr_t, sizeof(s_sCMservaddr_t),
								(char *)docsIf3CmStatusUsRangingStatus1stMsg, sizeof(docsIf3CmStatusUsRangingStatus1stMsg),
								DOCSIF3CMSTATUSUSRANGINGSTATUS30MSG_OFFSET);
			break;
		case 1 :
			*pnUsRanging = GetInt(s_sCMSocketfd, &s_sCMservaddr_t, sizeof(s_sCMservaddr_t),
								(char *)docsIf3CmStatusUsRangingStatus2ndMsg, sizeof(docsIf3CmStatusUsRangingStatus2ndMsg),
								DOCSIF3CMSTATUSUSRANGINGSTATUS30MSG_OFFSET);
			break;
		case 2 :
			*pnUsRanging = GetInt(s_sCMSocketfd, &s_sCMservaddr_t, sizeof(s_sCMservaddr_t),
								(char *)docsIf3CmStatusUsRangingStatus3rdMsg, sizeof(docsIf3CmStatusUsRangingStatus3rdMsg),
								DOCSIF3CMSTATUSUSRANGINGSTATUS30MSG_OFFSET);
			break;
		case 3 :
			*pnUsRanging = GetInt(s_sCMSocketfd, &s_sCMservaddr_t, sizeof(s_sCMservaddr_t),
								(char *)docsIf3CmStatusUsRangingStatus4thMsg, sizeof(docsIf3CmStatusUsRangingStatus4thMsg),
								DOCSIF3CMSTATUSUSRANGINGSTATUS30MSG_OFFSET);
			break;
		default :
			CM_DI_Print(0, ("[%s][%d] Invalid Up Channel ID (%d)\n", __FUNCTION__, __LINE__, nUsId));
			*pnUsRanging = DI_CM_RDC_RANGE_ERROR;
			break;
	}

	close(s_sCMSocketfd);

	CM_DI_Print(5, ("[%s][%d] Upstream Range (%d) is %d\n", __FUNCTION__, __LINE__, nUsId, *pnUsRanging));

	return DRV_OK;
}

/*****************************************************************************
* function : DRV_CM_Bdsgcc_GetUsChannelId
* description : Get eCM upstream channel ID
* input :
*   nUsId : upstream channel index
*   pnUsChId : pointer for storing eCM upstream channel ID
* return :
*   DRV_OK / DRV_ERR
******************************************************************************/
DRV_Error DRV_CM_Bdsgcc_GetUsChannelId(HINT32 nUsId, HINT32 *pnUsChId)
{
	struct sockaddr_in s_sCMservaddr_t;
	int n, s_sCMSocketfd=0;
	struct timeval tv;

	VK_MEM_Memset(&s_sCMservaddr_t, 0, sizeof(s_sCMservaddr_t));

	s_sCMservaddr_t.sin_family = AF_INET;
	s_sCMservaddr_t.sin_port = htons(SNMP_PORT);
	inet_pton(AF_INET, psCmtargetAddr, &s_sCMservaddr_t.sin_addr);

	s_sCMSocketfd = socket(AF_INET, SOCK_DGRAM, 0);

	if (s_sCMSocketfd == -1)
	{
		CM_DI_Print(0, ("[%s][%d] socket error!\n", __FUNCTION__, __LINE__));
		return DRV_ERR;
	}

	tv.tv_sec = 2;
	tv.tv_usec = 0;
	n = setsockopt(s_sCMSocketfd, SOL_SOCKET, SO_SNDTIMEO, &tv, sizeof(tv));
	n = setsockopt(s_sCMSocketfd, SOL_SOCKET, SO_RCVTIMEO, &tv, sizeof(tv));

	switch (nUsId)
	{
		case 0 :
			*pnUsChId = GetInt(s_sCMSocketfd, &s_sCMservaddr_t, sizeof(s_sCMservaddr_t),
								(char *)docsIfUpChannelId1stMsg, sizeof(docsIfUpChannelId1stMsg),
								DOCSIFUPCHANNELID30MSG_OFFSET);
			break;
		case 1 :
			*pnUsChId = GetInt(s_sCMSocketfd, &s_sCMservaddr_t, sizeof(s_sCMservaddr_t),
								(char *)docsIfUpChannelId2ndMsg, sizeof(docsIfUpChannelId2ndMsg),
								DOCSIFUPCHANNELID30MSG_OFFSET);
			break;
		case 2 :
			*pnUsChId = GetInt(s_sCMSocketfd, &s_sCMservaddr_t, sizeof(s_sCMservaddr_t),
								(char *)docsIfUpChannelId3rdMsg, sizeof(docsIfUpChannelId3rdMsg),
								DOCSIFUPCHANNELID30MSG_OFFSET);
			break;
		case 3 :
			*pnUsChId = GetInt(s_sCMSocketfd, &s_sCMservaddr_t, sizeof(s_sCMservaddr_t),
								(char *)docsIfUpChannelId4thMsg, sizeof(docsIfUpChannelId4thMsg),
								DOCSIFUPCHANNELID30MSG_OFFSET);
			break;
		default :
			CM_DI_Print(0, ("[%s][%d]Invalid Up Channel ID (%d)\n", __FUNCTION__, __LINE__, nUsId));
			break;
	}

	close(s_sCMSocketfd);

	CM_DI_Print(5, ("[%s][%d] Upstream CH ID (%d) is %d\n", __FUNCTION__, __LINE__, nUsId, *pnUsChId));

	return DRV_OK;
}

/*****************************************************************************
* function : DRV_CM_Bdsgcc_GetUsModulation
* description : Get eCM upstream modulation
* input :
*   nUsId : upstream channel index
*   pnUsMod : pointer for storing eCM upstream modulation
* return :
*   DRV_OK / DRV_ERR
******************************************************************************/
DRV_Error DRV_CM_Bdsgcc_GetUsModulation(HINT32 nUsId, HINT32 *pnUsMod)
{
	struct sockaddr_in s_sCMservaddr_t;
	int n, s_sCMSocketfd=0;
	struct timeval tv;

	VK_MEM_Memset(&s_sCMservaddr_t, 0, sizeof(s_sCMservaddr_t));

	s_sCMservaddr_t.sin_family = AF_INET;
	s_sCMservaddr_t.sin_port = htons(SNMP_PORT);
	inet_pton(AF_INET, psCmtargetAddr, &s_sCMservaddr_t.sin_addr);

	s_sCMSocketfd = socket(AF_INET, SOCK_DGRAM, 0);

	if (s_sCMSocketfd == -1)
	{
		CM_DI_Print(0, ("[%s][%d] socket error!\n", __FUNCTION__, __LINE__));
		return DRV_ERR;
	}

	tv.tv_sec = 2;
	tv.tv_usec = 0;
	n = setsockopt(s_sCMSocketfd, SOL_SOCKET, SO_SNDTIMEO, &tv, sizeof(tv));
	n = setsockopt(s_sCMSocketfd, SOL_SOCKET, SO_RCVTIMEO, &tv, sizeof(tv));

	switch (nUsId)
	{
		case 0 :
			*pnUsMod = GetInt(s_sCMSocketfd, &s_sCMservaddr_t, sizeof(s_sCMservaddr_t),
								(char *)docsIfUpChannelType1stMsg, sizeof(docsIfUpChannelType1stMsg),
								DOCSIFUPCHANNELTYPE30MSG_OFFSET);
			break;
		case 1 :
			*pnUsMod = GetInt(s_sCMSocketfd, &s_sCMservaddr_t, sizeof(s_sCMservaddr_t),
								(char *)docsIfUpChannelType2ndMsg, sizeof(docsIfUpChannelType2ndMsg),
								DOCSIFUPCHANNELTYPE30MSG_OFFSET);
			break;
		case 2 :
			*pnUsMod = GetInt(s_sCMSocketfd, &s_sCMservaddr_t, sizeof(s_sCMservaddr_t),
								(char *)docsIfUpChannelType3rdMsg, sizeof(docsIfUpChannelType3rdMsg),
								DOCSIFUPCHANNELTYPE30MSG_OFFSET);
			break;
		case 3 :
			*pnUsMod = GetInt(s_sCMSocketfd, &s_sCMservaddr_t, sizeof(s_sCMservaddr_t),
								(char *)docsIfUpChannelType4thMsg, sizeof(docsIfUpChannelType4thMsg),
								DOCSIFUPCHANNELTYPE30MSG_OFFSET);
			break;
		default :
			CM_DI_Print(0, ("[%s][%d] Invalid Up Channel ID (%d)\n", __FUNCTION__, __LINE__, nUsId));
			*pnUsMod = DI_CM_RDC_MOD_ERROR;
			break;
	}

	close(s_sCMSocketfd);

	CM_DI_Print(5, ("[%s][%d] Upstream Mode (%d) is %d\n", __FUNCTION__, __LINE__, nUsId, *pnUsMod));

	return DRV_OK;
}

/*****************************************************************************
* function : DRV_CM_Bdsgcc_GetUsBandwidth
* description : Get eCM upstream bandwidth
* input :
*   nUsId : upstream channel index
*   pnUsBandwith : pointer for storing eCM upstream bandwidth
* return :
*   DRV_OK / DRV_ERR
******************************************************************************/
DRV_Error DRV_CM_Bdsgcc_GetUsBandwidth(HINT32 nUsId, HINT32 *pnUsBandwith)
{
	struct sockaddr_in s_sCMservaddr_t;
	int n, s_sCMSocketfd=0;
	struct timeval tv;

	VK_MEM_Memset(&s_sCMservaddr_t, 0, sizeof(s_sCMservaddr_t));

	s_sCMservaddr_t.sin_family = AF_INET;
	s_sCMservaddr_t.sin_port = htons(SNMP_PORT);
	inet_pton(AF_INET, psCmtargetAddr, &s_sCMservaddr_t.sin_addr);

	s_sCMSocketfd = socket(AF_INET, SOCK_DGRAM, 0);

	if (s_sCMSocketfd == -1)
	{
		CM_DI_Print(0, ("[%s][%d] socket error!\n",__FUNCTION__, __LINE__));
		return DRV_ERR;
	}

	tv.tv_sec = 2;
	tv.tv_usec = 0;
	n = setsockopt(s_sCMSocketfd, SOL_SOCKET, SO_SNDTIMEO, &tv, sizeof(tv));
	n = setsockopt(s_sCMSocketfd, SOL_SOCKET, SO_RCVTIMEO, &tv, sizeof(tv));

	switch (nUsId)
	{
		case 0 :
			*pnUsBandwith = GetInt(s_sCMSocketfd, &s_sCMservaddr_t, sizeof(s_sCMservaddr_t),
								(char *)docsIfUpChannelWidth1stMsg, sizeof(docsIfUpChannelWidth1stMsg),
								DOCSIFUPCHANNELWIDTH30MSG_OFFSET);
			break;
		case 1 :
			*pnUsBandwith = GetInt(s_sCMSocketfd, &s_sCMservaddr_t, sizeof(s_sCMservaddr_t),
								(char *)docsIfUpChannelWidth2ndMsg, sizeof(docsIfUpChannelWidth2ndMsg),
								DOCSIFUPCHANNELWIDTH30MSG_OFFSET);
			break;
		case 2 :
			*pnUsBandwith = GetInt(s_sCMSocketfd, &s_sCMservaddr_t, sizeof(s_sCMservaddr_t),
								(char *)docsIfUpChannelWidth3rdMsg, sizeof(docsIfUpChannelWidth3rdMsg),
								DOCSIFUPCHANNELWIDTH30MSG_OFFSET);
			break;
		case 3 :
			*pnUsBandwith = GetInt(s_sCMSocketfd, &s_sCMservaddr_t, sizeof(s_sCMservaddr_t),
								(char *)docsIfUpChannelWidth4thMsg, sizeof(docsIfUpChannelWidth4thMsg),
								DOCSIFUPCHANNELWIDTH30MSG_OFFSET);
			break;
		default :
			CM_DI_Print(0, ("[%s][%d] Invalid Up Channel ID (%d)\n", __FUNCTION__, __LINE__, nUsId));
			*pnUsBandwith = 0;
			break;
	}

	close(s_sCMSocketfd);

	CM_DI_Print(5, ("[%s][%d] Upstream Bandwidth (%d) is %d\n", __FUNCTION__, __LINE__, nUsId, *pnUsBandwith));

	return DRV_OK;
}

/*****************************************************************************
* function : DRV_CM_Bdsgcc_GetUsSymbolrate
* description : Get eCM upstream symbol rate
* input :
*   nUsId : upstream channel index
*   pnUsSymbolrate : pointer for storing eCM upstream symbol rate
* return :
*   DRV_OK / DRV_ERR
******************************************************************************/
DRV_Error DRV_CM_Bdsgcc_GetUsSymbolrate(HINT32 nUsId, HINT32 *pnUsSymbolrate)
{
	struct sockaddr_in s_sCMservaddr_t;
	int n, s_sCMSocketfd=0;
	struct timeval tv;

	VK_MEM_Memset(&s_sCMservaddr_t, 0, sizeof(s_sCMservaddr_t));

	s_sCMservaddr_t.sin_family = AF_INET;
	s_sCMservaddr_t.sin_port = htons(SNMP_PORT);
	inet_pton(AF_INET, psCmtargetAddr, &s_sCMservaddr_t.sin_addr);

	s_sCMSocketfd = socket(AF_INET, SOCK_DGRAM, 0);

	if (s_sCMSocketfd == -1)
	{
		CM_DI_Print(0, ("[%s][%d] socket error!\n", __FUNCTION__, __LINE__));
		return DRV_ERR;
	}

	tv.tv_sec = 2;
	tv.tv_usec = 0;
	n = setsockopt(s_sCMSocketfd, SOL_SOCKET, SO_SNDTIMEO, &tv, sizeof(tv));
	n = setsockopt(s_sCMSocketfd, SOL_SOCKET, SO_RCVTIMEO, &tv, sizeof(tv));

	switch (nUsId)
	{
		case 0 :
			*pnUsSymbolrate = GetInt(s_sCMSocketfd, &s_sCMservaddr_t, sizeof(s_sCMservaddr_t),
								(char *)docsIfUpChannelWidth1stMsg, sizeof(docsIfUpChannelWidth1stMsg),
								DOCSIFUPCHANNELWIDTH30MSG_OFFSET);
			break;
		case 1 :
			*pnUsSymbolrate = GetInt(s_sCMSocketfd, &s_sCMservaddr_t, sizeof(s_sCMservaddr_t),
								(char *)docsIfUpChannelWidth2ndMsg, sizeof(docsIfUpChannelWidth2ndMsg),
								DOCSIFUPCHANNELWIDTH30MSG_OFFSET);
			break;
		case 2 :
			*pnUsSymbolrate = GetInt(s_sCMSocketfd, &s_sCMservaddr_t, sizeof(s_sCMservaddr_t),
								(char *)docsIfUpChannelWidth3rdMsg, sizeof(docsIfUpChannelWidth3rdMsg),
								DOCSIFUPCHANNELWIDTH30MSG_OFFSET);
			break;
		case 3 :
			*pnUsSymbolrate = GetInt(s_sCMSocketfd, &s_sCMservaddr_t, sizeof(s_sCMservaddr_t),
								(char *)docsIfUpChannelWidth4thMsg, sizeof(docsIfUpChannelWidth4thMsg),
								DOCSIFUPCHANNELWIDTH30MSG_OFFSET);
			break;
		default :
			CM_DI_Print(0, ("[%s][%d] Invalid Up Channel ID (%d)\n", __FUNCTION__, __LINE__, nUsId));
			*pnUsSymbolrate = 0;
			break;
	}

	close(s_sCMSocketfd);

	*pnUsSymbolrate = *pnUsSymbolrate/1250;

	return DRV_OK;
}

/*****************************************************************************
* function : DRV_CM_Bdsgcc_GetUsSignalInfo
* description : Get eCM upstream signal information
* input :
*   nUsId : upstream channel index
*   pstUsInfo : pointer for storing eCM upstream signal information
* return :
*   DRV_OK / DRV_ERR
******************************************************************************/
DRV_Error DRV_CM_Bdsgcc_GetUsSignalInfo (HINT32 nUsId, DI_CM_UsSignalQuality_t *pstUsInfo)
{
	int nCmstatus = 0;

	HUINT32 ulRDCChannelId[4]={0,}, ulRDCBw[4]={0,};
	HINT32	lRDCType[4]={0,}, lRDCPower[4]={0,}, ulRDCFreq[4]={0,} ;
	HUINT8	RDCMod[12]={0,};

	struct sockaddr_in s_sCMservaddr_t;
	int n, s_sCMSocketfd=0;
	struct timeval tv;

	CM_DI_Print(6, ("[%s][%d] ---- \n ", __FUNCTION__, __LINE__));

	if ( pstUsInfo == NULL )
	{
		CM_DI_Print(0, ("[%s][%d] ERR! \n", __FUNCTION__, __LINE__));
		return DRV_ERR;
	}

	if (  nUsId >= 4 )
	{
		CM_DI_Print(0, ("[%s][%d] ERR! nUsId(%d) \n", __FUNCTION__, __LINE__, nUsId));
		return DRV_ERR;
	}

	VK_MEM_Memset(&s_sCMservaddr_t, 0, sizeof(s_sCMservaddr_t));

	s_sCMservaddr_t.sin_family = AF_INET;
	s_sCMservaddr_t.sin_port = htons(SNMP_PORT);
	inet_pton(AF_INET, psCmtargetAddr, &s_sCMservaddr_t.sin_addr);

	s_sCMSocketfd = socket(AF_INET, SOCK_DGRAM, 0);
	if (s_sCMSocketfd == -1)
	{
		CM_DI_Print(0, ("[%s][%d] socket error!\n", __FUNCTION__, __LINE__));
		return DRV_ERR;
	}

	tv.tv_sec = 2;
	tv.tv_usec = 0;
	n = setsockopt(s_sCMSocketfd, SOL_SOCKET, SO_SNDTIMEO, &tv, sizeof(tv));
	n = setsockopt(s_sCMSocketfd, SOL_SOCKET, SO_RCVTIMEO, &tv, sizeof(tv));

	nCmstatus = GetInt(s_sCMSocketfd, &s_sCMservaddr_t, sizeof(s_sCMservaddr_t),
				(char *)docsIfCmStatusValueMsg, sizeof(docsIfCmStatusValueMsg),
				DOCSIFCMSTATUSVALUEMSG_OFFSET);

	CM_DI_Print(5, ("[%s][%d] CM Status - %d\n", __FUNCTION__, __LINE__, nCmstatus));

	if(nCmstatus <= 4)
	{
		pstUsInfo->ulUsFrequency = 0;
		pstUsInfo->fUsInputPower = 0.0; /* dBuV */
		pstUsInfo->ulUsModulation = 0;
		pstUsInfo->ulUsChannelId = 0;
		pstUsInfo->ulUsBandwidth = 0;
		pstUsInfo->ulUsStrength= 0;
	}
	else
	{
		switch (nUsId)
		{
			case 0 :
				ulRDCFreq[nUsId] = GetInt(s_sCMSocketfd, &s_sCMservaddr_t, sizeof(s_sCMservaddr_t),
									(char *)docsIfUpChannelFrequency1stMsg, sizeof(docsIfUpChannelFrequency1stMsg),
									DOCSIFUPCHANNELFREQUENCY30MSG_OFFSET);
				lRDCPower[nUsId] = GetInt(s_sCMSocketfd, &s_sCMservaddr_t, sizeof(s_sCMservaddr_t),
								(char *)docsIf3CmStatusUsTxPower1stMsg, sizeof(docsIf3CmStatusUsTxPower1stMsg),
								DOCSIF3CMSTATUSUSTXPOWER30MSG_OFFSET);
				lRDCType[nUsId] = GetInt(s_sCMSocketfd, &s_sCMservaddr_t, sizeof(s_sCMservaddr_t),
								(char *)docsIfUpChannelType1stMsg, sizeof(docsIfUpChannelType1stMsg),
								DOCSIFUPCHANNELTYPE30MSG_OFFSET);
				ulRDCChannelId[nUsId] = GetInt(s_sCMSocketfd, &s_sCMservaddr_t, sizeof(s_sCMservaddr_t),
								(char *)docsIfUpChannelId1stMsg, sizeof(docsIfUpChannelId1stMsg),
								DOCSIFUPCHANNELID30MSG_OFFSET);
				ulRDCBw[nUsId] = GetInt(s_sCMSocketfd, &s_sCMservaddr_t, sizeof(s_sCMservaddr_t),
								(char *)docsIfUpChannelWidth1stMsg, sizeof(docsIfUpChannelWidth1stMsg),
								DOCSIFUPCHANNELWIDTH30MSG_OFFSET);
				break;

			case 1 :
				ulRDCFreq[nUsId] = GetInt(s_sCMSocketfd, &s_sCMservaddr_t, sizeof(s_sCMservaddr_t),
									(char *)docsIfUpChannelFrequency2ndMsg, sizeof(docsIfUpChannelFrequency2ndMsg),
									DOCSIFUPCHANNELFREQUENCY30MSG_OFFSET);
				lRDCPower[nUsId] = GetInt(s_sCMSocketfd, &s_sCMservaddr_t, sizeof(s_sCMservaddr_t),
								(char *)docsIf3CmStatusUsTxPower2ndMsg, sizeof(docsIf3CmStatusUsTxPower2ndMsg),
								DOCSIF3CMSTATUSUSTXPOWER30MSG_OFFSET);
				lRDCType[nUsId] = GetInt(s_sCMSocketfd, &s_sCMservaddr_t, sizeof(s_sCMservaddr_t),
								(char *)docsIfUpChannelType2ndMsg, sizeof(docsIfUpChannelType2ndMsg),
								DOCSIFUPCHANNELTYPE30MSG_OFFSET);
				ulRDCChannelId[nUsId] = GetInt(s_sCMSocketfd, &s_sCMservaddr_t, sizeof(s_sCMservaddr_t),
								(char *)docsIfUpChannelId2ndMsg, sizeof(docsIfUpChannelId2ndMsg),
								DOCSIFUPCHANNELID30MSG_OFFSET);
				ulRDCBw[nUsId] = GetInt(s_sCMSocketfd, &s_sCMservaddr_t, sizeof(s_sCMservaddr_t),
								(char *)docsIfUpChannelWidth2ndMsg, sizeof(docsIfUpChannelWidth2ndMsg),
								DOCSIFUPCHANNELWIDTH30MSG_OFFSET);
				break;

			case 2 :
				ulRDCFreq[nUsId] = GetInt(s_sCMSocketfd, &s_sCMservaddr_t, sizeof(s_sCMservaddr_t),
									(char *)docsIfUpChannelFrequency3rdMsg, sizeof(docsIfUpChannelFrequency3rdMsg),
									DOCSIFUPCHANNELFREQUENCY30MSG_OFFSET);
				lRDCPower[nUsId] = GetInt(s_sCMSocketfd, &s_sCMservaddr_t, sizeof(s_sCMservaddr_t),
								(char *)docsIf3CmStatusUsTxPower3rdMsg, sizeof(docsIf3CmStatusUsTxPower3rdMsg),
								DOCSIF3CMSTATUSUSTXPOWER30MSG_OFFSET);
				lRDCType[nUsId] = GetInt(s_sCMSocketfd, &s_sCMservaddr_t, sizeof(s_sCMservaddr_t),
								(char *)docsIfUpChannelType3rdMsg, sizeof(docsIfUpChannelType3rdMsg),
								DOCSIFUPCHANNELTYPE30MSG_OFFSET);
				ulRDCChannelId[nUsId] = GetInt(s_sCMSocketfd, &s_sCMservaddr_t, sizeof(s_sCMservaddr_t),
								(char *)docsIfUpChannelId3rdMsg, sizeof(docsIfUpChannelId3rdMsg),
								DOCSIFUPCHANNELID30MSG_OFFSET);
				ulRDCBw[nUsId] = GetInt(s_sCMSocketfd, &s_sCMservaddr_t, sizeof(s_sCMservaddr_t),
								(char *)docsIfUpChannelWidth3rdMsg, sizeof(docsIfUpChannelWidth3rdMsg),
								DOCSIFUPCHANNELWIDTH30MSG_OFFSET);
				break;

			case 3 :
				ulRDCFreq[nUsId] = GetInt(s_sCMSocketfd, &s_sCMservaddr_t, sizeof(s_sCMservaddr_t),
									(char *)docsIfUpChannelFrequency4thMsg, sizeof(docsIfUpChannelFrequency4thMsg),
									DOCSIFUPCHANNELFREQUENCY30MSG_OFFSET);
				lRDCPower[nUsId] = GetInt(s_sCMSocketfd, &s_sCMservaddr_t, sizeof(s_sCMservaddr_t),
								(char *)docsIf3CmStatusUsTxPower4thMsg, sizeof(docsIf3CmStatusUsTxPower4thMsg),
								DOCSIF3CMSTATUSUSTXPOWER30MSG_OFFSET);
				lRDCType[nUsId] = GetInt(s_sCMSocketfd, &s_sCMservaddr_t, sizeof(s_sCMservaddr_t),
								(char *)docsIfUpChannelType4thMsg, sizeof(docsIfUpChannelType4thMsg),
								DOCSIFUPCHANNELTYPE30MSG_OFFSET);
				ulRDCChannelId[nUsId] = GetInt(s_sCMSocketfd, &s_sCMservaddr_t, sizeof(s_sCMservaddr_t),
								(char *)docsIfUpChannelId4thMsg, sizeof(docsIfUpChannelId4thMsg),
								DOCSIFUPCHANNELID30MSG_OFFSET);
				ulRDCBw[nUsId] = GetInt(s_sCMSocketfd, &s_sCMservaddr_t, sizeof(s_sCMservaddr_t),
								(char *)docsIfUpChannelWidth4thMsg, sizeof(docsIfUpChannelWidth4thMsg),
								DOCSIFUPCHANNELWIDTH30MSG_OFFSET);
				break;

			default :
				CM_DI_Print(0, ("[%s][%d] Invalid Up Channel ID (%d)\n", __FUNCTION__, __LINE__, nUsId));
				break;
		}

	}

	close(s_sCMSocketfd);

	pstUsInfo->ulUsFrequency = (HUINT32)ulRDCFreq[nUsId];
	pstUsInfo->fUsInputPower = (float)(lRDCPower[nUsId]/10.0)+60.0;	/* dBuV */
	pstUsInfo->ulUsModulation = (HUINT32)lRDCType[nUsId];
	pstUsInfo->ulUsChannelId = ulRDCChannelId[nUsId];
	pstUsInfo->ulUsBandwidth = ulRDCBw[nUsId];

	if (pstUsInfo->fUsInputPower >= 115.0)
	{
		pstUsInfo->ulUsStrength = 100;
	}
	else if (pstUsInfo->fUsInputPower >= 70.0)
	{
		pstUsInfo->ulUsStrength = (HUINT32)(((pstUsInfo->fUsInputPower-60) / 5.0 - 1.0)*10.0);
	}
	else
	{
		pstUsInfo->ulUsStrength = 10;
	}

	switch (pstUsInfo->ulUsModulation)
	{
		case DI_CM_RDC_MOD_ERROR :
			VK_snprintf(RDCMod, sizeof(RDCMod), "Error");
			break;
		case DI_CM_RDC_MOD_UNKNOWN :
			VK_snprintf(RDCMod, sizeof(RDCMod), "Unknown");
			break;
		case DI_CM_RDC_MOD_TDMA :
			VK_snprintf(RDCMod, sizeof(RDCMod), "TDMA");
			break;
		case DI_CM_RDC_MOD_ATDMA :
			VK_snprintf(RDCMod, sizeof(RDCMod), "ATDMA");
			break;
		case DI_CM_RDC_MOD_SCDMA :
			VK_snprintf(RDCMod, sizeof(RDCMod), "SCDMA");
			break;
		case DI_CM_RDC_MOD_TDMAANDATDMA :
			VK_snprintf(RDCMod, sizeof(RDCMod), "TDMA&ATDMA");
			break;
		default :
			VK_snprintf(RDCMod, sizeof(RDCMod), "Error");
			break;
	}

	CM_DI_Print(1,("[%s][%d] nUsId = %d \n", __FUNCTION__, __LINE__, nUsId));
	CM_DI_Print(1,("======================================================================\n"));
	CM_DI_Print(1,("  U/S Freq [Hz]      = %u\n", pstUsInfo->ulUsFrequency));
	CM_DI_Print(1,("  U/S Modulation     = %s\n", RDCMod));
	CM_DI_Print(1,("  U/S Channel ID     = %u\n", pstUsInfo->ulUsChannelId));
	CM_DI_Print(1,("  U/S Bandwidth [Hz] = %u\n", pstUsInfo->ulUsBandwidth));
	CM_DI_Print(1,("  U/S Power [dBuV]   = %3.1f\n", pstUsInfo->fUsInputPower));
	CM_DI_Print(1,("  U/S Strenght [%%]   = %u\n", pstUsInfo->ulUsStrength));
	CM_DI_Print(1,("======================================================================\n"));

	return DRV_OK;
}

/*****************************************************************************
* function : DRV_CM_Bdsgcc_GetScanDsFreq
* description : Get downstream frequency from list
* input :
*   pszDsFreq : pointer for storing downstream frequency
* return :
*   DRV_OK / DRV_ERR
******************************************************************************/
DRV_Error DRV_CM_Bdsgcc_GetScanDsFreq(HCHAR *pszDsFreq)
{
	int iRetcode = 0, i=0;
	struct sockaddr_in s_sCMservaddr_t;
	int n, s_sCMSocketfd=0;
	struct timeval tv;
	unsigned char cdPvtMibEnableKeyValue[8]={""};
	int cdPvtMibEnableKeyValueLen=0, cdPvtMibEnableKeyValueSize=0, asFreqList[8];

	VK_MEM_Memset(&s_sCMservaddr_t, 0, sizeof(s_sCMservaddr_t));

	s_sCMservaddr_t.sin_family = AF_INET;
	s_sCMservaddr_t.sin_port = htons(SNMP_PORT);
	inet_pton(AF_INET, psCmtargetAddr, &s_sCMservaddr_t.sin_addr);

	s_sCMSocketfd = socket(AF_INET, SOCK_DGRAM, 0);

	if (s_sCMSocketfd == -1)
	{
		CM_DI_Print(0, ("[%s][%d] socket error!\n", __FUNCTION__, __LINE__));
		return DRV_ERR;
	}

	tv.tv_sec = 2;
	tv.tv_usec = 0;
	n = setsockopt(s_sCMSocketfd, SOL_SOCKET, SO_SNDTIMEO, &tv, sizeof(tv));
	n = setsockopt(s_sCMSocketfd, SOL_SOCKET, SO_RCVTIMEO, &tv, sizeof(tv));

	cdPvtMibEnableKeyValueSize = sizeof(cdPvtMibEnableKeyValue);
	cdPvtMibEnableKeyValueLen = GetString_P(s_sCMSocketfd, (struct sockaddr_in *) &s_sCMservaddr_t, sizeof(s_sCMservaddr_t),
						(char *)cdPvtMibEnableKeyValueMsg, sizeof(cdPvtMibEnableKeyValueMsg), CDPVTMIBENABLEKEYVALUE_OFFSET,
						cdPvtMibEnableKeyValue, &cdPvtMibEnableKeyValueSize);
#if 0
	iRetcode = GetInt_P(s_sCMSocketfd, (struct sockaddr_in *) &s_sCMservaddr_t, sizeof(s_sCMservaddr_t),
						cdPrivateMibEnableMsg, sizeof(cdPrivateMibEnableMsg), CDPRIVATEMIBENABLE_OFFSET);
#endif


	VK_MEM_Memset(asFreqList, 0, sizeof(asFreqList));

	for(i =1; i<=8; i++)
	{

		cmScanFrequencyMsg[49] = i;

		asFreqList[i-1]= GetInt_P(s_sCMSocketfd, &s_sCMservaddr_t, sizeof(s_sCMservaddr_t),
						(char *)cmScanFrequencyMsg, sizeof(cmScanFrequencyMsg),
						CMSCANFREQUENCY_OFFSET);
		if(asFreqList[i-1] == -1)
		{

			break;
		}


	}

	cdPrivateMibEnableMsg[48] = 0x00;	/* disable */
	iRetcode = GetInt_P(s_sCMSocketfd, (struct sockaddr_in *) &s_sCMservaddr_t, sizeof(s_sCMservaddr_t),
						(char *)cdPrivateMibEnableMsg, sizeof(cdPrivateMibEnableMsg), CDPRIVATEMIBENABLE_OFFSET);

	close(s_sCMSocketfd);
#if 0
	VK_sprintf(pszDsFreq, "%d", iFreqList);
#else
	switch(i-1)
	{
		case 0:
			VK_snprintf(pszDsFreq, 20, "Not list");
			break;
		case 1:
			VK_snprintf(pszDsFreq, 20,"%d\n", asFreqList[0]);
			break;
		case 2:
			VK_snprintf(pszDsFreq, 30,"%d\n%d\n", asFreqList[0], asFreqList[1]);
			break;
		case 3:
			VK_snprintf(pszDsFreq, 40, "%d\n%d\n%d", asFreqList[0], asFreqList[1], asFreqList[2]);
			break;
		case 4:
			VK_snprintf(pszDsFreq, 50,"%d\n%d\n%d\n%d\n", asFreqList[0], asFreqList[1], asFreqList[2], asFreqList[3]);
			break;
		case 5:
			VK_snprintf(pszDsFreq, 60,"%d\n%d\n%d\n%d\n%d\n", asFreqList[0], asFreqList[1], asFreqList[2], asFreqList[3],
				asFreqList[4]);
			break;
		case 6:
			VK_snprintf(pszDsFreq, 70,"%d\n%d\n%d\n%d\n%d\n%d\n", asFreqList[0], asFreqList[1], asFreqList[2], asFreqList[3],
				asFreqList[4], asFreqList[5]);
			break;
		case 7:
			VK_snprintf(pszDsFreq, 80,"%d\n%d\n%d\n%d\n%d\n%d\n%d\n", asFreqList[0], asFreqList[1], asFreqList[2], asFreqList[3],
				asFreqList[4], asFreqList[5], asFreqList[6]);
			break;
		case 8:
			VK_snprintf(pszDsFreq, 90,"%d\n%d\n%d\n%d\n%d\n%d\n%d\n%d\n", asFreqList[0], asFreqList[1], asFreqList[2], asFreqList[3],
				asFreqList[4], asFreqList[5], asFreqList[6], asFreqList[7]);
			break;
	}
#endif

	CM_DI_Print(1, ("[%s][%d] Scan DS Freq List \n%s\n", __FUNCTION__, __LINE__, pszDsFreq));

	return DRV_OK;
}

/*****************************************************************************
* function : DRV_CM_Bdsgcc_SetScanDsFreq
* description : Set downstream frequency to list
* input :
*   pszDsFreq : pointer for storing downstream frequency
* return :
*   DRV_OK / DRV_ERR
******************************************************************************/
DRV_Error DRV_CM_Bdsgcc_SetScanDsFreq(HUINT32 *pulDsFreq)
{
	int iRetcode = 0;
	struct sockaddr_in s_sCMservaddr_t;
	int n, s_sCMSocketfd=0;
	struct timeval tv;
  	unsigned char cdPvtMibEnableKeyValue[8]={""}, ucScanFreq[4]={""};
	int cdPvtMibEnableKeyValueLen=0, cdPvtMibEnableKeyValueSize=0, iScanFreq = 0;

	iScanFreq = (int)(*pulDsFreq);

	VK_MEM_Memset(&s_sCMservaddr_t, 0, sizeof(s_sCMservaddr_t));

	s_sCMservaddr_t.sin_family = AF_INET;
	s_sCMservaddr_t.sin_port = htons(SNMP_PORT);
	inet_pton(AF_INET, psCmtargetAddr, &s_sCMservaddr_t.sin_addr);

	s_sCMSocketfd = socket(AF_INET, SOCK_DGRAM, 0);

	if (s_sCMSocketfd == -1)
	{
		CM_DI_Print(0, ("[%s][%d] socket error!\n", __FUNCTION__, __LINE__));
		return DRV_ERR;
	}

	tv.tv_sec = 2;
	tv.tv_usec = 0;
	n = setsockopt(s_sCMSocketfd, SOL_SOCKET, SO_SNDTIMEO, &tv, sizeof(tv));
	n = setsockopt(s_sCMSocketfd, SOL_SOCKET, SO_RCVTIMEO, &tv, sizeof(tv));

	cdPvtMibEnableKeyValueSize = sizeof(cdPvtMibEnableKeyValue);
	cdPvtMibEnableKeyValueLen = GetString_P(s_sCMSocketfd, (struct sockaddr_in *) &s_sCMservaddr_t, sizeof(s_sCMservaddr_t),
						(char *)cdPvtMibEnableKeyValueMsg, sizeof(cdPvtMibEnableKeyValueMsg), CDPVTMIBENABLEKEYVALUE_OFFSET,
						cdPvtMibEnableKeyValue, &cdPvtMibEnableKeyValueSize);

	/* Set Interface and real Mac address */
	VK_MEM_Memset(ucScanFreq, 0, sizeof(ucScanFreq));

	CM_DI_Print(5, ("[%s][%d] Scan freq %d ", __FUNCTION__, __LINE__, *pulDsFreq));

	if(*pulDsFreq == 0)
	{
		*pulDsFreq = GetInt_P(s_sCMSocketfd, &s_sCMservaddr_t, sizeof(s_sCMservaddr_t),
				        (char *)cmScanPushFrequencyClearMsg, sizeof(cmScanPushFrequencyClearMsg),
				        CMSCANPUSHFREQUENCYCLEAR_OFFSET);
	}
	else
	{
		ucScanFreq[0] = (*pulDsFreq) & 0xFF;
		ucScanFreq[1] = ((*pulDsFreq)>>8) & 0xFF;
		ucScanFreq[2] = ((*pulDsFreq)>>16) & 0xFF;
		ucScanFreq[3] = ((*pulDsFreq)>>24) & 0xFF;

		CM_DI_Print(5, ("[%s][%d] (%02x, %02x, %02x, %02x)\n", __FUNCTION__, __LINE__, ucScanFreq[0], ucScanFreq[1], ucScanFreq[2], ucScanFreq[3]));

		cmScanPushFrequencyMsg[50] = ucScanFreq[3];
		cmScanPushFrequencyMsg[51] = ucScanFreq[2];
		cmScanPushFrequencyMsg[52] = ucScanFreq[1];
		cmScanPushFrequencyMsg[53] = ucScanFreq[0];

		*pulDsFreq = GetInt_P(s_sCMSocketfd, &s_sCMservaddr_t, sizeof(s_sCMservaddr_t),
				        (char *)cmScanPushFrequencyMsg, sizeof(cmScanPushFrequencyMsg),
				        CMSCANPUSHFREQUENCY_OFFSET);

	}
	cdPrivateMibEnableMsg[48] = 0x00;	/* disable */
	iRetcode = GetInt_P(s_sCMSocketfd, (struct sockaddr_in *) &s_sCMservaddr_t, sizeof(s_sCMservaddr_t),
						(char *)cdPrivateMibEnableMsg, sizeof(cdPrivateMibEnableMsg), CDPRIVATEMIBENABLE_OFFSET);

	close(s_sCMSocketfd);

	/* Send DS frequency to scan directly downstream channel in CM */
	// TBD
	if(iScanFreq != 0) DsgSendScanDSFreq((unsigned int *)&iScanFreq);

	return DRV_OK;
}

/*****************************************************************************
* function : DRV_CM_Bdsgcc_GetTemperature
* description : Get saved and current temperature of eCM
* input :
*   pnCmSavedTemp : pointer for storing saved temperature
*   pnCmCurrentTemp : pointer for storing current temperature
* return :
*   DRV_OK / DRV_ERR
******************************************************************************/
DRV_Error DRV_CM_Bdsgcc_GetTemperature(HINT32 *pnCmSavedTemp, HINT32 *pnCmCurrentTemp)
{
	int nRet = FALSE;

	nRet = DsgSendGetCmSavedTemp(pnCmSavedTemp, pnCmCurrentTemp);
	if(nRet != TRUE)
	{
		CM_DI_Print(0, ("[%s][%d] Err! nRet = %d\n", __FUNCTION__, __LINE__, nRet));
		return DRV_ERR;
	}
#if 0
	else
	{
		CM_DI_Print(5, ("[%s][%d] pnCmSavedTemp[%d] pnCmCurrentTemp[%d]\n", __FUNCTION__, __LINE__, *pnCmSavedTemp, *pnCmCurrentTemp));
	}
#endif

	return DRV_OK;
}

/*****************************************************************************
* function : DRV_CM_Bdsgcc_SetTemperature
* description : Set saved temperature of eCM
* input :
*   nTempMode : temperature for saving eCM flash
* return :
*   DRV_OK / DRV_ERR
******************************************************************************/
DRV_Error DRV_CM_Bdsgcc_SetTemperature(HINT32 nTempMode)
{
	int nRet = FALSE;

	nRet = DsgSetDefaultTemp(nTempMode);
	if(nRet != TRUE)
	{
		CM_DI_Print(0, ("[%s][%d] Err! nRet = %d\n", __FUNCTION__, __LINE__, nRet));
		return DRV_ERR;

	}

	return DRV_OK;
}


/*****************************************************************************
* function : DRV_CM_Bdsgcc_SetStandbyMode
* description : Set eCM LED
* input :
*   nStandbyCmLed : 0 - LED off (standby) / 1 - LED on (operation)
* return :
*   DRV_OK / DRV_ERR
******************************************************************************/
DRV_Error DRV_CM_Bdsgcc_SetStandbyMode(HINT32 nStandbyCmLed)
{
	int nRet = DRV_ERR;

	CM_DI_Print(1, ("[%s][%d] nStandbyCmLed = %d\n", __FUNCTION__, __LINE__, nStandbyCmLed));
	if (nStandbyCmLed == DI_CM_LED_TURN_OFF)
	{
		nRet = DsgSetStandbyMode(DI_CM_LED_TURN_OFF);
	}
	else
	{
		nRet = DsgSetStandbyMode(DI_CM_LED_TURN_ON);
	}

	if(nRet != TRUE)
	{
		CM_DI_Print(0, ("[%s][%d] Err! nRet = %d\n", __FUNCTION__, __LINE__, nRet));
		return DRV_ERR;
	}

	return DRV_OK;
}

/*****************************************************************************
* function : DRV_CM_Bdsgcc_SetFanMode
* description : Set fan mode controlled by eCM
* input :
*   nFanMode : 0 - fan off / 1 - fan low speed / 2 - fan high speed
* return :
*   DRV_OK / DRV_ERR
******************************************************************************/
DRV_Error DRV_CM_Bdsgcc_SetFanMode(HINT32 nFanMode)
{
	int nRet = DRV_ERR;

	if (nFanMode == DI_CM_FAN_OFF )
	{
		nRet = DsgSetFanControl(DI_CM_FAN_OFF);
	}
	else if (nFanMode == DI_CM_FAN_LOW_SPEED)
	{
		nRet = DsgSetFanControl(DI_CM_FAN_LOW_SPEED);
	}
	else
	{
		nRet = DsgSetFanControl(DI_CM_FAN_HIGH_SPEED);
	}

	if(nRet != TRUE)
	{
		CM_DI_Print(0, ("[%s][%d] Err! nRet = %d\n", __FUNCTION__, __LINE__, nRet));
		return DRV_ERR;

	}

	return DRV_OK;
}

/*****************************************************************************
* function : DRV_CM_Bdsgcc_SetDefault
* description : Set eCM default settings
* input : none
* return :
*   DRV_OK / DRV_ERR
******************************************************************************/
DRV_Error DRV_CM_Bdsgcc_SetDefault(void)
{
	int nRet = DRV_ERR;

	nRet = DsgSetDefaultSavedFrq(2);
	if(nRet != TRUE)
	{
		CM_DI_Print(0, ("[%s][%d] Err! nRet = %d\n", __FUNCTION__, __LINE__, nRet));
		return DRV_ERR;

	}

	return DRV_OK;
}

/*****************************************************************************
* function : DRV_CM_Bdsgcc_GetSerialNumber
* description : Get eCM serial number
* input :
*   pszCmSerialNo : pointer for stored seriel number
* return :
*   DRV_OK / DRV_ERR
******************************************************************************/
DRV_Error DRV_CM_Bdsgcc_GetSerialNumber(HCHAR *pszCmSerialNo)
{
	HUINT8 cCmSerial[14] = {0,};
	HINT32 unCmSerialLen = 0, unCmSerialSize = 0;

	struct sockaddr_in s_sCMservaddr_t;
	int n, s_sCMSocketfd=0;
	struct timeval tv;

	VK_MEM_Memset(&s_sCMservaddr_t, 0, sizeof(s_sCMservaddr_t));

	s_sCMservaddr_t.sin_family = AF_INET;
	s_sCMservaddr_t.sin_port = htons(SNMP_PORT);
	inet_pton(AF_INET, psCmtargetAddr, &s_sCMservaddr_t.sin_addr);

	s_sCMSocketfd = socket(AF_INET, SOCK_DGRAM, 0);

	if (s_sCMSocketfd == -1)
	{
		CM_DI_Print(0, ("[%s][%d] socket error!\n",__FUNCTION__, __LINE__));
		return DRV_ERR;
	}

	tv.tv_sec = 2;
	tv.tv_usec = 0;
	n = setsockopt(s_sCMSocketfd, SOL_SOCKET, SO_SNDTIMEO, &tv, sizeof(tv));
	n = setsockopt(s_sCMSocketfd, SOL_SOCKET, SO_RCVTIMEO, &tv, sizeof(tv));

	unCmSerialSize = sizeof(cCmSerial);
	unCmSerialLen= GetCmVer(s_sCMSocketfd, (struct sockaddr_in *) &s_sCMservaddr_t, sizeof(s_sCMservaddr_t),
						(char *)docsDevSerialNumberMsg, sizeof(docsDevSerialNumberMsg), DOCSDEVSERIALNUMBERMSG_OFFSET,
						cCmSerial, &unCmSerialSize);

	close(s_sCMSocketfd);
	
	if (unCmSerialLen != unCmSerialSize)
	{
		CM_DI_Print(0, ("[%s][%d] unCmSerialLen[%d], unCmSerialSize[%d] \n", __FUNCTION__, __LINE__, unCmSerialLen, unCmSerialSize));
		return DRV_ERR;
	}

	VK_strncpy(pszCmSerialNo, cCmSerial, unCmSerialLen);
	
	CM_DI_Print(5, ("[%s][%d] CM Serial Number %s\n", __FUNCTION__, __LINE__, pszCmSerialNo));

	return DRV_OK;
}

/*****************************************************************************
* function : DRV_CM_Bdsgcc_SetSerialNumber
* description : Set eCM serial number
* input :
*   pszCmSerialNo : pointer for storing seriel number
* return :
*   DRV_OK / DRV_ERR
******************************************************************************/
DRV_Error DRV_CM_Bdsgcc_SetSerialNumber(HCHAR *pszCmSerialNo)
{
	int iRetcode = 0, i = 0, pduLen=0;
	struct sockaddr_in s_sCMservaddr_t;
	int n, s_sCMSocketfd=0;
	struct timeval tv;
	unsigned char cdPvtMibEnableKeyValue[8]={""}, cdPvtMibSerialValue[14]={""};
	int cdPvtMibEnableKeyValueLen=0, cdPvtMibEnableKeyValueSize=0, cdPvtMibSerialLen=0, cdPvtMibSerialSize=0;
	char cCmSerial[14]={""};

	VK_MEM_Memset(&s_sCMservaddr_t, 0, sizeof(s_sCMservaddr_t));

	s_sCMservaddr_t.sin_family = AF_INET;
	s_sCMservaddr_t.sin_port = htons(SNMP_PORT);
	inet_pton(AF_INET, psCmtargetAddr, &s_sCMservaddr_t.sin_addr);

	s_sCMSocketfd = socket(AF_INET, SOCK_DGRAM, 0);

	if (s_sCMSocketfd == -1)
	{
		CM_DI_Print(0, ("[%s][%d] socket error!\n", __FUNCTION__, __LINE__));
		return DRV_ERR;
	}

	tv.tv_sec = 2;
	tv.tv_usec = 0;
	n = setsockopt(s_sCMSocketfd, SOL_SOCKET, SO_SNDTIMEO, &tv, sizeof(tv));
	n = setsockopt(s_sCMSocketfd, SOL_SOCKET, SO_RCVTIMEO, &tv, sizeof(tv));

	cdPvtMibEnableKeyValueSize = sizeof(cdPvtMibEnableKeyValue);
	cdPvtMibEnableKeyValueLen = GetString_P(s_sCMSocketfd, (struct sockaddr_in *) &s_sCMservaddr_t, sizeof(s_sCMservaddr_t),
										(char *)cdPvtMibEnableKeyValueMsg, sizeof(cdPvtMibEnableKeyValueMsg), CDPVTMIBENABLEKEYVALUE_OFFSET,
										cdPvtMibEnableKeyValue, &cdPvtMibEnableKeyValueSize);
	if (cdPvtMibEnableKeyValueLen == 0)
	{
		close(s_sCMSocketfd);

		CM_DI_Print(0, ("[%s][%d] Error cdPvtMibEnableKeyValueLen is %d \n",__FUNCTION__, __LINE__, cdPvtMibEnableKeyValueLen));
		return DRV_ERR;
	}


	VK_MEM_Memset(cCmSerial, 0, 14);

	pduLen = VK_strlen(pszCmSerialNo);

	VK_snprintf(cCmSerial, pduLen+1, "%s", pszCmSerialNo);

	CM_DI_Print(5, ("[%s][%d] cCmSerial(%s) pduLen(%x)\n", __FUNCTION__, __LINE__, cCmSerial, pduLen));

	for (i=0; i<14; i++)
	{
		if(i < pduLen)
		{
			cdSerialNumberMsg[51+i] = (unsigned char)cCmSerial[i];
		}
		else
		{
			cdSerialNumberMsg[51+i] = 0x20;
		}
	}

	cdPvtMibSerialSize = sizeof(cdPvtMibSerialValue);
	cdPvtMibSerialLen = GetString_P(s_sCMSocketfd, &s_sCMservaddr_t, sizeof(s_sCMservaddr_t),
								(char *)cdSerialNumberMsg, sizeof(cdSerialNumberMsg), CDSERIALNUMBER_OFFSET, 
								cdPvtMibSerialValue, &cdPvtMibSerialSize);
	if (cdPvtMibSerialLen == 0)
	{
		close(s_sCMSocketfd);

		CM_DI_Print(0, ("[%s][%d] Error cdPvtMibSerialLen is %d \n",__FUNCTION__, __LINE__, cdPvtMibSerialLen));
		return DRV_ERR;
	}

	cdPrivateMibEnableMsg[48] = 0x00;	/* disable */
	iRetcode = GetInt_P(s_sCMSocketfd, (struct sockaddr_in *) &s_sCMservaddr_t, sizeof(s_sCMservaddr_t),
					(char *)cdPrivateMibEnableMsg, sizeof(cdPrivateMibEnableMsg), CDPRIVATEMIBENABLE_OFFSET);
	if (iRetcode == -1)
	{
		close(s_sCMSocketfd);

		CM_DI_Print(0, ("[%s][%d] Error iRetcode is %d \n",__FUNCTION__, __LINE__, iRetcode));
		return DRV_ERR;
	}

	/* cdFactCommitSetting.0 true */
	iRetcode = GetInt_P(s_sCMSocketfd, (struct sockaddr_in *) &s_sCMservaddr_t, sizeof(s_sCMservaddr_t),
					(char *)cdFactCommitSettingsMsg, sizeof(cdFactCommitSettingsMsg), CDFACTCOMMITSETTINGS_OFFSET);
	if (iRetcode == -1)
	{
		close(s_sCMSocketfd);

		CM_DI_Print(0, ("[%s][%d] Error iRetcode is %d \n",__FUNCTION__, __LINE__, iRetcode));
		return DRV_ERR;
	}

	close(s_sCMSocketfd);

	return DRV_OK;
}

/*****************************************************************************
* function : DRV_CM_Bdsgcc_SetDsgMode
* description : Set eCM DSG mode
* input :
*   mode : 0 - disable / 1 - basic / 2 - advanced
* return :
*   DRV_OK / DRV_ERR
******************************************************************************/
DRV_Error DRV_CM_Bdsgcc_SetDsgMode(HINT32 mode)
{
	int nRet = FALSE;

	nRet = DsgSetDSGMode(mode);
	if(nRet != TRUE)
	{
		CM_DI_Print(0, ("[%s][%d] Err! nRet = %d\n", __FUNCTION__, __LINE__, nRet));
		return DRV_ERR;

	}

	return DRV_OK;
}

/*****************************************************************************
* function : DRV_CM_Bdsgcc_SetDsgScan
* description : Set eCM scan
* input :
*   ucIsStart : 0 - scan stop / 1 - scan start
* return :
*   DRV_OK / DRV_ERR
******************************************************************************/
DRV_Error DRV_CM_Bdsgcc_SetDsgScan(HUINT8 ucIsStart)
{
	int nRet = FALSE;

	nRet = DsgScan(ucIsStart);
	if(nRet != TRUE)
	{
		CM_DI_Print(0, ("[%s][%d] Err! nRet = %d\n", __FUNCTION__, __LINE__, nRet));
		return DRV_ERR;

	}

	return DRV_OK;
}


/*****************************************************************************
* function : DRV_CM_Bdsgcc_SetAdminStatus
* description : Set eCM docsDevSwAdminStatus
* input :
*   nMode : 1 - allowProvisioningUpgrade / 2 - ignoreProvisioningUpgrade
* return :
*   DRV_OK / DRV_ERR
******************************************************************************/
DRV_Error DRV_CM_Bdsgcc_SetAdminStatus(HINT32 nMode)
{
	int nRet = DRV_OK;

	nRet = DsgSetDefaultSnmp(nMode);

	if(nRet != TRUE)
	{
		CM_DI_Print(0, ("[%s][%d] Err! nRet = %d\n", __FUNCTION__, __LINE__, nRet));
		return DRV_ERR;
	}

	return DRV_OK;
}


/*****************************************************************************
* function : DRV_CM_Bdsgcc_GetSwOperStatus
* description : Get eCM docsDevSwOperStatus
* input : none
* return :
*    docsDevSwOperStatus
******************************************************************************/
HINT32 DRV_CM_Bdsgcc_GetSwOperStatus(void)
{
	int iRetcode = 0;

	struct sockaddr_in s_sCMservaddr_t;
	int n, s_sCMSocketfd=0;
	struct timeval tv;

	VK_MEM_Memset(&s_sCMservaddr_t, 0, sizeof(s_sCMservaddr_t));

	s_sCMservaddr_t.sin_family = AF_INET;
	s_sCMservaddr_t.sin_port = htons(SNMP_PORT);
	inet_pton(AF_INET, psCmtargetAddr, &s_sCMservaddr_t.sin_addr);

	s_sCMSocketfd = socket(AF_INET, SOCK_DGRAM, 0);

	if (s_sCMSocketfd == -1)
	{
		CM_DI_Print(0, ("[%s][%d] socket error!\n", __FUNCTION__, __LINE__));
		return DRV_ERR;
	}

	tv.tv_sec = 2;
	tv.tv_usec = 0;
	n = setsockopt(s_sCMSocketfd, SOL_SOCKET, SO_SNDTIMEO, &tv, sizeof(tv));
	n = setsockopt(s_sCMSocketfd, SOL_SOCKET, SO_RCVTIMEO, &tv, sizeof(tv));

	// Get the CM's Status
	iRetcode = GetInt(s_sCMSocketfd, &s_sCMservaddr_t, sizeof(s_sCMservaddr_t),
			(char *)docsDevSwOperStatusMsg, sizeof(docsDevSwOperStatusMsg),
			DOCSDEVSWOPERSTATUSMSG_OFFSET);

	close(s_sCMSocketfd);

	return iRetcode;
}

/*****************************************************************************
* function : DRV_CM_Bdsgcc_SwUpgradeTrigger
* description : Trigger eCM SW upgrade with canned-SNMP
* input : none
* return : none
******************************************************************************/
void DRV_CM_Bdsgcc_SwUpgradeTrigger(void)
{
	docsSwUgradebyCannedSNMP();
}


/*****************************************************************************
* function : DRV_CM_Bdsgcc_SwUpgrade
* description : Start eCM SW upgrade
* input : none
* return :
*   DRV_OK / DRV_ERR
******************************************************************************/
DRV_Error DRV_CM_Bdsgcc_SwUpgrade(void)
{
	int nStat = 0;
	int nTimeOut = 1;
	int retVal = DRV_ERR;


	if (s_nCmUpgradeOk == 1)
	{
		CM_DI_Print(0, ("[%s][%d] Alreay CM Software upgraded. Rebooting STB!!!! \n", __FUNCTION__, __LINE__));
		return DRV_OK;
	}
	else
	{
		VK_SYSTEM_Command("/sbin/tftpd /media/drive1 &");
		DRV_CM_Bdsgcc_SetDsgMode(2);
		DRV_CM_Bdsgcc_SetDsgScan(0);
		DRV_CM_Bdsgcc_SwUpgradeTrigger();

		CM_DI_Print(0, ("[%s][%d] Start CM Software upgrade.... \n", __FUNCTION__, __LINE__));

		while(1)
		{
			VK_TASK_Sleep(1000);

			nStat = DRV_CM_Bdsgcc_GetSwOperStatus();
			if (nStat == 1)
			{

				CM_DI_Print(0,("[%s][%d] <<< CM S/W Upgrade in Progress....  [%d]\n", __FUNCTION__, __LINE__, nStat));

			}
			else if (nStat == 4)
			{
				CM_DI_Print(0,("[%s][%d] <<<  CM S/W Upgrade....  Fail!!!\n", __FUNCTION__, __LINE__));
				s_nCmUpgradeOk = 0;
				retVal = DRV_ERR;
				break;
			}
			else if( (nStat == 2) || (nStat == 3))
			{
				CM_DI_Print(0,("[%s][%d] <<<  CM S/W Upgrade....  Complete!!!\n", __FUNCTION__, __LINE__));
				s_nCmUpgradeOk = 1;
				retVal = DRV_OK;
				break;
			}
			else
			{
				CM_DI_Print(0,("[%s][%d] SwOpStatus Other[%d] \n", __FUNCTION__, __LINE__, nStat));
			}

			if (nTimeOut >= 20) /* 2분 */
			{
				CM_DI_Print(0,("[%s][%d] CM S/W Upgrade....  TIMEOUT!\n", __FUNCTION__, __LINE__));
				retVal = DRV_ERR;
				s_nCmUpgradeOk = 0;
				break;
			}

			nTimeOut++;
		}
		DRV_CM_Bdsgcc_SetAdminStatus(2);
		VK_TASK_Sleep(100);
		DRV_CM_Bdsgcc_SetDsgMode(0);
		DRV_CM_Bdsgcc_SetDsgScan(1);

		return retVal;
	}
}


/*****************************************************************************
* function : DRV_CM_Bdsgcc_GetEstbIp
* description : Get eSTB IP address
* input :
*   pcEstbIp : pointer for storing eSTB IP address
* return :
*   DRV_OK / DRV_ERR
******************************************************************************/
DRV_Error DRV_CM_Bdsgcc_GetEstbIp(HCHAR *pcEstbIp)
{
	//HINT32 retcode=0;
	HUINT32 cm0=0, host1=0;
	HUINT32 ulLoop;
	struct in_addr cm0_str;

	VK_MEM_Memset(&cm0_str, 0, sizeof(struct in_addr));

	for(ulLoop = 0; ulLoop < DRV_CFG_GetNetMaxDevice(); ulLoop++)
	{
		if(s_stDevList[ulLoop].eDevType == DI_NETWORK_DEV_CABLEMODEM)
		{
			CM_DI_Print(4,("[%s][%d] DevId : %d IfName : %s \n",__FUNCTION__, __LINE__,s_stDevList[ulLoop].ulDevId,s_stDevList[ulLoop].szName));
			GetIpStackAddress(&cm0, s_stDevList[ulLoop].szName);
			break;
		}
	}

	if(cm0 != 0)
	{
		cm0_str.s_addr = cm0;
	}

	host1 = cm0_str.s_addr;
	VK_strncpy(pcEstbIp, (char *)inet_ntoa(cm0_str), INET_ADDR_STRLEN);
	/*inet_ntoa_b( cm0_str, addr);*/

	CM_DI_Print(5, ("[%s][%d] Mpeg IP Addr : (0x%x) (%s)", __FUNCTION__, __LINE__, host1, pcEstbIp));

	return DRV_OK;
}

/*****************************************************************************
* function : DRV_CM_Bdsgcc_GetEstbMac
* description : Get eSTB MAC address
* input :
*   pucEstbMac : pointer for storing eSTB MAC address
* return :
*   DRV_OK / DRV_ERR
******************************************************************************/
DRV_Error DRV_CM_Bdsgcc_GetEstbMac(HUINT8 *pucEstbMac)
{
	int socketDescriptor;
	struct ifreq ifReq;
	int rc;
	HUINT8 ulLoop;
	//HUINT8	*host_macaddr;

	// Pre-set ifReq to 0's.
	VK_MEM_Memset(&ifReq, 0, sizeof(ifReq));

	for(ulLoop = 0; ulLoop < DRV_CFG_GetNetMaxDevice(); ulLoop++)
	{
		if(s_stDevList[ulLoop].eDevType == DI_NETWORK_DEV_CABLEMODEM)
		{
			CM_DI_Print(4,("[%s][%d] DevId : %d IfName : %s \n",__FUNCTION__, __LINE__,s_stDevList[ulLoop].ulDevId,s_stDevList[ulLoop].szName));
			VK_strncpy(ifReq.ifr_ifrn.ifrn_name, s_stDevList[ulLoop].szName, IFNAMSIZ-1);
			break;
		}
	}

	// Prime the interface for which I am making this request.
	// Create a socket to use for the network interface.
	socketDescriptor = socket(AF_INET, SOCK_DGRAM, 0);

	if (socketDescriptor >= 0)
	{
		// Get the physical address from the interface.
		rc = ioctl(socketDescriptor, SIOCGIFHWADDR, (char *) &ifReq);

		close(socketDescriptor);

		if (rc != 0)
		{
			CM_DI_Print(0, ("[%s][%d] Failed to get my MAC address\n", __FUNCTION__, __LINE__));
			return DRV_ERR;
		}

		VK_memcpy( pucEstbMac,  (HUINT8 *)&ifReq.ifr_hwaddr.sa_data, 6 );
		CM_DI_Print(1, ("[%s][%d] %s Mac addr [%02X:%02X:%02X:%02X:%02X:%02X]\n", __FUNCTION__, __LINE__, ifReq.ifr_ifrn.ifrn_name, pucEstbMac[0], pucEstbMac[1], pucEstbMac[2], pucEstbMac[3], pucEstbMac[4], pucEstbMac[5]));
	}
	else
	{
		CM_DI_Print(0, ( "[%s][%d] Failed to create my socket!", __FUNCTION__, __LINE__));
		return DRV_ERR;
	}

	return DRV_OK;
}


/*****************************************************************************
* function: DRV_CM_Dsgcc_GetCmIp
* description : Get saved and current temperature of CM
* input :
*   pucHmxAddedEvent : SNMP event ID
* return : none
******************************************************************************/
void humax_ProcessEcmNotifications(char * pucHmxAddedEvent)
{
	switch (pucHmxAddedEvent[0])
	{
#if 0
		case Event_CDFilename:
			{
				int data_len = 0;
				unsigned char *data_ptr = NULL;
				unsigned char *ptr = (unsigned char *)pucHmxAddedEvent;

				//init filename
				VK_memset(g_CDFileName, 0, 128);

				//skip the 1st byte
				ptr++;

				//get the packet length
				data_len  = ((int)*ptr++) << 24;
				data_len |= ((int)*ptr++) << 16;
				data_len |= ((int)*ptr++) << 8;
				data_len |= ((int)*ptr++) << 0;

				//get the data payload pointer
				data_ptr = ptr;

				if (data_len > 0 && data_ptr != NULL)
				{
					VK_memcpy(g_CDFileName, data_ptr, data_len);
					CM_DI_Print(0,("[%s][%d] Docsis Common Download FileName(%s)\n", __FUNCTION__, __LINE__, g_CDFileName));
				}
			}
			break;

		case Event_CDIpAddress:
			{
				int data_len = 0, ip_address = 0;
				unsigned char *ptr = (unsigned char *)pucHmxAddedEvent;

				//init IP address
				g_CDIpAddress = 0;

				//skip the 1st byte
				ptr++;

				// get the packet length
				data_len  = ((int)*ptr++) << 24;
				data_len |= ((int)*ptr++) << 16;
				data_len |= ((int)*ptr++) << 8;
				data_len |= ((int)*ptr++) << 0;

				//get the data payload pointer
				ip_address  = ((int)*ptr++) << 24;
				ip_address |= ((int)*ptr++) << 16;
				ip_address |= ((int)*ptr++) << 8;
				ip_address |= ((int)*ptr++) << 0;

				if (data_len > 0 && ip_address != 0)
				{
					g_CDIpAddress = htonl(ip_address);
					CM_DI_Print(0,("[%s][%d] Docsis Common Download IpAddress(%lx)\n", __FUNCTION__, __LINE__, g_CDIpAddress);
				}
			}
			break;

		case Event_CmSerialNum:
			{
				int data_len = 0;
				unsigned char *data_ptr = NULL;
				unsigned char *ptr = (unsigned char *)pucHmxAddedEvent;

				//init filename
				VK_memset(g_ReceivedCmSerialNum, 0, sizeof(g_ReceivedCmSerialNum));

				//skip the 1st byte
				ptr++;

				//get the packet length
				data_len  = ((int)*ptr++) << 24;
				data_len |= ((int)*ptr++) << 16;
				data_len |= ((int)*ptr++) << 8;
				data_len |= ((int)*ptr++) << 0;

				//get the data payload pointer
				data_ptr = ptr;

				if (data_len > 0 && data_ptr != NULL)
				{
					VK_memcpy(g_ReceivedCmSerialNum, data_ptr, data_len);
					g_ReceivedCmSerialNumLen = data_len;
					CM_DI_Print(0,("[%s][%d] CmSerialNumber(%s) len(%d)\n", __FUNCTION__, __LINE__, g_ReceivedCmSerialNum, g_ReceivedCmSerialNumLen);

				}
			}
			break;

		case Event_MpegMacAddr:
			{
				int data_len = 0;
				unsigned char *data_ptr = NULL;
				unsigned char *ptr = (unsigned char *)pucHmxAddedEvent;

				//init filename
				VK_memset(g_ReceivedMpegMacAddr, 0, sizeof(g_ReceivedMpegMacAddr));

				//skip the 1st byte
				ptr++;

				//get the packet length
				data_len  = ((int)*ptr++) << 24;
				data_len |= ((int)*ptr++) << 16;
				data_len |= ((int)*ptr++) << 8;
				data_len |= ((int)*ptr++) << 0;

				//get the data payload pointer
				data_ptr = ptr;

				if (data_len > 0 && data_ptr != NULL)
				{
					VK_memcpy(g_ReceivedMpegMacAddr, data_ptr, data_len);
					g_ReceivedMpegMacAddrLen = data_len;
#if 0
					VK_printf("\nNHLEE : MpegMacAddr[%x][%x][%x][%x][%x][%x] len(%d)\n"
					, g_ReceivedMpegMacAddr[0], g_ReceivedMpegMacAddr[1],g_ReceivedMpegMacAddr[2],
					g_ReceivedMpegMacAddr[3], g_ReceivedMpegMacAddr[4],g_ReceivedMpegMacAddr[5], g_ReceivedMpegMacAddrLen);
#endif
				}
			}
			break;
#endif
		case Event_eCMSNMPGetRequest: /* eCM -> eSTB */
			{
				unsigned int data_len = 0;
				unsigned short ucSnmpEventId;
				unsigned char *ptr = (unsigned char *)pucHmxAddedEvent;
#if 0
				VK_printf("\n NHLEE Receiving SNMP pkt\n");
				{
					unsigned int i = 0;
					for( i=0; i<16 ; i++)
					{
						VK_printf(" %02x ", ptr[i]);
					}
					VK_printf("\n");
				}
#endif
				//skip the 1st byte(Notification ID)
				ptr++;

				// get the packet length
				data_len  = ((int)*ptr++) << 24;
				data_len |= ((int)*ptr++) << 16;
				data_len |= ((int)*ptr++) << 8;
				data_len |= ((int)*ptr++) << 0;

				//get the data payload pointer
				ucSnmpEventId = ( (unsigned char) pucHmxAddedEvent[5] ) << 8;
				ucSnmpEventId |= ( (unsigned char) pucHmxAddedEvent[6] );

				if (data_len > 0 && ucSnmpEventId != 0)
				{
					CM_DI_Print(1,("[%s][%d] eCM SNMP GetRequest(%d)(%d)\n", __FUNCTION__, __LINE__, ucSnmpEventId, data_len));
					P_CM_RecvSNMPGetRequest(ucSnmpEventId, data_len);
				}
			}
			break;

		case Event_eCMSNMPSetRequest: /* eCM -> eSTB */
			{
				unsigned int data_len = 0;
				unsigned short ucSnmpEventId;
				unsigned char *ptr = (unsigned char *)pucHmxAddedEvent;
				unsigned char ucPduData = 0;
#if 0
				VK_printf("\n NHLEE Receiving SetRequest SNMP pkt\n");
				{
					unsigned int i = 0;
					for( i=0; i<16 ; i++)
					{
						VK_printf(" %02x ", ptr[i]);
					}
					VK_printf("\n");
				}
#endif
				//skip the 1st byte(Notification ID)
				ptr++;

				// get the packet length
				data_len  = ((int)*ptr++) << 24;
				data_len |= ((int)*ptr++) << 16;
				data_len |= ((int)*ptr++) << 8;
				data_len |= ((int)*ptr++) << 0;

				//get the data payload pointer
				ucSnmpEventId = ( (unsigned char) pucHmxAddedEvent[5] ) << 8;
				ucSnmpEventId |= ( (unsigned char) pucHmxAddedEvent[6] );

				ucPduData = (unsigned char) pucHmxAddedEvent[7];
				if (data_len > 0 && ucSnmpEventId != 0)
				{
					CM_DI_Print(1,("[%s][%d] eCM SNMP SetRequest(%d)(%d)\n", __FUNCTION__, __LINE__, ucSnmpEventId, data_len));
					P_CM_RecvSNMPSetRequest(ucSnmpEventId, data_len, ucPduData);
				}
			}
			break;

		default:
			CM_DI_Print(0,("[%s][%d] humax_ProcessEcmNotifications :: Invalid Notification from eCM %d\n", __FUNCTION__, __LINE__, pucHmxAddedEvent[0]));
			break;
	}
}


/* TODO - Are below APIs needed ??? */
#if 0
/*****************************************************************************
* function: DI_CM_GetCmSavedTemp
* description : Get saved and current temperature of CM
* input: none
*   pCmSavedTemp	: pointer for storing saved temperature
*   pCmCurrentTemp	: pointer for storing current temperature
* return:
*    DRV_Error : DRV_OK / DRV_ERR
******************************************************************************/
DRV_Error DRV_CM_Bdsgcc_GetEtherLink(void)
{
	int iRetcode = 0;

	struct sockaddr_in s_sCMservaddr_t;
	int n, s_sCMSocketfd=0;
	struct timeval tv;

	VK_MEM_Memset(&s_sCMservaddr_t, 0, sizeof(s_sCMservaddr_t));

	s_sCMservaddr_t.sin_family = AF_INET;
	s_sCMservaddr_t.sin_port = htons(SNMP_PORT);
	inet_pton(AF_INET, psCmtargetAddr, &s_sCMservaddr_t.sin_addr);

	s_sCMSocketfd = socket(AF_INET, SOCK_DGRAM, 0);
	if (s_sCMSocketfd == -1)
	{
		CM_DI_Print(0, ("[%s][%d] socket error!\n", __FUNCTION__, __LINE__));
		return DRV_ERR;
	}
	tv.tv_sec = 2;
	tv.tv_usec = 0;
	n = setsockopt(s_sCMSocketfd, SOL_SOCKET, SO_SNDTIMEO, &tv, sizeof(tv));
	n = setsockopt(s_sCMSocketfd, SOL_SOCKET, SO_RCVTIMEO, &tv, sizeof(tv));

	// Get the CPE Interface State
	iRetcode = GetInt(s_sCMSocketfd, &s_sCMservaddr_t, sizeof(s_sCMservaddr_t),
					(char *)ifOperStatusMsg, sizeof(ifOperStatusMsg),
					IFOPERSTATUSMSG_OFFSET);
	if (iRetcode == 1)
	{
		CM_DI_Print(1, ("[%s][%d] interface 1 is UP\n", __FUNCTION__, __LINE__));
	}
	else
	{
		CM_DI_Print(1, ("[%s][%d] interface 1 is NOT UP. ifOperStatus is %d\n", __FUNCTION__, __LINE__, iRetcode));
	}


	//iRetcode = GetEtherLink(s_sCMSocketfd, (struct sockaddr_in *) &s_sCMservaddr_t, sizeof(s_sCMservaddr_t));
	close(s_sCMSocketfd);

	return iRetcode;
}

// Calculate the 16 bit checksum over the buffer passed in for
// the number of BYTES passed in as a length.  Code will actually
// compute the checksum over (Length/2) USHORTS, but because we are
// little endian we'll take the buffer in as UCHAR's and convert.
// Also, this is necessary for odd Length...
unsigned short DI_CM_CalculateChecksum (unsigned char *pucData, int Length)
{
	int Index = 0;
	unsigned short usWord, usChecksum;
	unsigned long  ulTotal = 0;


	while (Index < Length)
	{
	usWord = (pucData[Index++] << 8) & 0xFF00;

	if (Index < Length) // handle possibility of odd # of bytes
	  usWord |= pucData[Index++];

	ulTotal += usWord;
	}

	// Don't understand what is being done here!!! But I found this
	// code fragment on the net and it works...
	ulTotal = (ulTotal & 0xffff) + (ulTotal>>16);
	ulTotal += (ulTotal >> 16);

	usChecksum = ~ulTotal & 0xFFFF;

	return usChecksum;
}

int DI_CM_Ping( char *ipAddress, unsigned int numberOfPings)
{
	unsigned long targetIpAddress = 0;
	int rc = 0;
	int sock, n;
	struct sockaddr_in name;
	unsigned char		*buf;

	typedef struct _DI_CM_ICMP_HDR
	{
	  unsigned char  ucType;
	  unsigned char  ucCode;
	  unsigned short usChecksum;
	  unsigned short usId;
	  unsigned short usSequence;
	} DI_CM_ICMP_HDR, *DI_CM_PICMP_HDR;

	typedef struct _DI_CM_ICMP_ECHO_PKT
	{
	  DI_CM_ICMP_HDR        IcmpHdr;
	  unsigned char           pucEchoData[128]; // maximum for our purposes. Really this can be as big as you like.
	} DI_CM_ICMP_ECHO_PKT, *DI_CM_PICMP_ECHO_PKT;

	DI_CM_ICMP_ECHO_PKT PingPkt;
	int PktSize;
	unsigned short usChecksum;
	unsigned char pucPingData[] = //"Broadcom DCT-5000 MAC stub is a-pingin' ya!";
	  // Data from captured ping packet, above.
	  {
		0x61, 0x62, 0x63, 0x64, 0x65, 0x66, 0x67, 0x68,
		0x69, 0x6A, 0x6B, 0x6C, 0x6D, 0x6E, 0x6F, 0x70,
		0x71, 0x72, 0x73, 0x74, 0x75, 0x76, 0x77, 0x61,
		0x62, 0x63, 0x64, 0x65, 0x66, 0x67, 0x68, 0x69
	  };

	numberOfPings = numberOfPings;
#if 1
	targetIpAddress = inet_network(ipAddress);
#else
	targetIpAddress = inet_addr(ipAddress);
#endif
	CM_DI_Print(5, ("[%s][%d] Ping : destIp(%s)(%x)\n", __FUNCTION__, __LINE__, ipAddress, targetIpAddress));
	PingPkt.IcmpHdr.ucType = 0x08; // ECHO=8
	PingPkt.IcmpHdr.ucCode = 0x00;
	PingPkt.IcmpHdr.usChecksum = 0x0000; // initially for cksm calculation
	PingPkt.IcmpHdr.usId = htons (0x0100); //(0xDADA); // arbitrary ID
	PingPkt.IcmpHdr.usSequence = htons (0x0C00); //(usSequence); // just count

	VK_memcpy (PingPkt.pucEchoData, pucPingData, sizeof (pucPingData));

	usChecksum = htons (DI_CM_CalculateChecksum ((unsigned char *) &PingPkt.IcmpHdr, sizeof (DI_CM_ICMP_HDR) + sizeof (pucPingData)));
	PingPkt.IcmpHdr.usChecksum = usChecksum;

	PktSize = sizeof (DI_CM_ICMP_HDR)+ sizeof (pucPingData);

	// Setup socket
	VK_memset(&name, 0, sizeof(name));

	// extract the addressing information into the sockaddr struct
	name.sin_addr.s_addr = htonl(targetIpAddress);
	name.sin_port = 1400;
	name.sin_family = AF_INET;

	sock = socket (AF_INET, SOCK_RAW, 1); // ICMP = 1
	if (sock < 0)
	{
	    return FALSE;
	}

	if( (buf = (unsigned char *)DD_MEM_Alloc(PktSize)) == NULL)
	{
		CM_DI_Print(0, ("[%s][%d] VK_malloc failure!\n", __FUNCTION__, __LINE__));
		close(sock);
		return FALSE;
	}


	rc = sendto(sock, (char*)&PingPkt, PktSize, 0,  (struct sockaddr*) &name, sizeof(name));


	if ( rc < 0 )
	{
		CM_DI_Print(0, ("[%s][%d] Socket sendto() error: %s\n", __FUNCTION__, __LINE__, strerror(errno)));
		if(buf != NULL)	DD_MEM_Free (buf);
		close(sock);
		return FALSE;
	}
	else
	{
		struct sockaddr_in	frominet;
		int					inetsize;
		int					cnt;

		bzero((char*)&frominet, sizeof(frominet));

		inetsize = sizeof(frominet);

		struct timeval  tv;
		bzero((char *) &tv, sizeof(struct timeval));
		tv.tv_sec = 1;
		n = setsockopt(sock, SOL_SOCKET, SO_RCVTIMEO, (char *)&tv, sizeof(tv));

		// Wait for response
		cnt = recvfrom(sock, (char*)buf, PktSize, 0, (struct sockaddr*)&frominet, (socklen_t*)&inetsize);
		if( cnt == PktSize )
		{
			if( frominet.sin_addr.s_addr == htonl(targetIpAddress ) )
				CM_DI_Print(0, ("[%s][%d] Rx response pkt from %s with len = %d\n",__FUNCTION__, __LINE__, (char *)inet_ntoa(frominet.sin_addr), cnt));

			DD_MEM_Free (buf);
			close(sock);
			return TRUE;
		}
		else
		{
			frominet.sin_addr.s_addr = htonl(targetIpAddress );

			CM_DI_Print(0, ("[%s][%d] No reply received. error:  (%s)%s\n", __FUNCTION__, __LINE__, (char *)inet_ntoa(frominet.sin_addr), strerror(errno)));
		}

	}

	close(sock);
	if(buf != NULL)	DD_MEM_Free (buf);

	return FALSE;

}
#endif


/*****************************************************************************
* function: DRV_CM_StartRebootMonitoringTask
* description : Start task for monitoring eCM rebooting
* input : none
* return:
*   DRV_Error : DRV_OK / DRV_ERR
******************************************************************************/
DRV_Error DRV_CM_StartRebootMonitoringTask(void)
{
	DRV_Error	nRet = DRV_ERR;
	HULONG		ulRebootMonitorTask = 0;

	nRet = VK_TASK_Create(  P_CM_RebootMonitorTask,
							DRV_CM_TASK_PRIORITY,
							BASIC_TASK_STACK_SIZE,
							"CM_Reboot_Monitor",
							NULL,
							&ulRebootMonitorTask,
							0 );

	if ((ulRebootMonitorTask == 0) || (nRet != VK_OK))
	{
		CM_DI_Print(0, ("[%s][%d] Error nRet(0x%x)", __FUNCTION__, __LINE__, nRet));
		return DRV_ERR;
	}
	else
	{
		VK_TASK_Start(ulRebootMonitorTask);
	}

	return DRV_OK;
}


/*****************************************************************************
* function: DRV_CM_RegisterEventNotify
* description : Register callback function for each event
* input :
*   eCmEvent : event for eCM
*   pfCmEventCallback : call-back function
* return: none
******************************************************************************/
void DRV_CM_RegisterEventNotify(DRV_CM_EVENT eCmEvent, DRV_CM_CALLBACK pfCmEventCallback)
{
	if (s_pfCmCallback[eCmEvent] != NULL)
	{
		CM_DI_Print(0, ("[%s][%d] CM callback function for event(%d) is already registered \n", __FUNCTION__, __LINE__, eCmEvent));
	}
	else
	{
		s_pfCmCallback[eCmEvent]= pfCmEventCallback;
		CM_DI_Print(0, ("[%s][%d] CM callback function for event(%d) is successfully registered \n", __FUNCTION__, __LINE__, eCmEvent));
	}

	return;
}


/*****************************************************************************
* function: DRV_CM_API_Install
* description : Install DRV_CM_xxx APIs
* input : none
* return: none
******************************************************************************/
void DRV_CM_API_Install(void)
{
	DRV_CM_GetEcmIp = &DRV_CM_Bdsgcc_GetEcmIp;
	DRV_CM_GetEcmMac = &DRV_CM_Bdsgcc_GetEcmMac;
	DRV_CM_SetEcmMac = &DRV_CM_Bdsgcc_SetEcmMac;
	DRV_CM_GetNetmask = &DRV_CM_Bdsgcc_GetNetmask;
	DRV_CM_GetGateway = &DRV_CM_Bdsgcc_GetGateway;
	DRV_CM_GetSwVer = &DRV_CM_Bdsgcc_GetSwVer;
	DRV_CM_GetCmRegStatus = &DRV_CM_Bdsgcc_GetCmRegStatus;
	DRV_CM_GetDsFreq = &DRV_CM_Bdsgcc_GetDsFreq;
	DRV_CM_GetDsPower = &DRV_CM_Bdsgcc_GetDsPower;
	DRV_CM_GetDsSnr = &DRV_CM_Bdsgcc_GetDsSnr;
	DRV_CM_GetDsBer = &DRV_CM_Bdsgcc_GetDsBer;
	DRV_CM_GetDsChannelId = &DRV_CM_Bdsgcc_GetDsChannelId;
	DRV_CM_GetDsModulation = &DRV_CM_Bdsgcc_GetDsModulation;
	DRV_CM_GetDsCorrected = &DRV_CM_Bdsgcc_GetDsCorrected;
	DRV_CM_GetDsUncorrected = &DRV_CM_Bdsgcc_GetDsUncorrected;
	DRV_CM_GetDsSignalInfo = &DRV_CM_Bdsgcc_GetDsSignalInfo;
	DRV_CM_GetUsFreq = &DRV_CM_Bdsgcc_GetUsFreq;
	DRV_CM_GetUsPower = &DRV_CM_Bdsgcc_GetUsPower;
	DRV_CM_GetUsRangingStatus = &DRV_CM_Bdsgcc_GetUsRangingStatus;
	DRV_CM_GetUsChannelId = &DRV_CM_Bdsgcc_GetUsChannelId;
	DRV_CM_GetUsModulation = &DRV_CM_Bdsgcc_GetUsModulation;
	DRV_CM_GetUsBandwidth = &DRV_CM_Bdsgcc_GetUsBandwidth;
	DRV_CM_GetUsSymbolrate = &DRV_CM_Bdsgcc_GetUsSymbolrate;
	DRV_CM_GetUsSignalInfo = &DRV_CM_Bdsgcc_GetUsSignalInfo;
	DRV_CM_GetScanDsFreq = &DRV_CM_Bdsgcc_GetScanDsFreq;
	DRV_CM_SetScanDsFreq = &DRV_CM_Bdsgcc_SetScanDsFreq;
	DRV_CM_GetTemperature = &DRV_CM_Bdsgcc_GetTemperature;
	DRV_CM_SetTemperature = &DRV_CM_Bdsgcc_SetTemperature;
	DRV_CM_SetStandbyMode = &DRV_CM_Bdsgcc_SetStandbyMode;
	DRV_CM_SetFanMode = &DRV_CM_Bdsgcc_SetFanMode;
	DRV_CM_SetDefault = &DRV_CM_Bdsgcc_SetDefault;
	DRV_CM_GetSerialNumber = &DRV_CM_Bdsgcc_GetSerialNumber;
	DRV_CM_SetSerialNumber = &DRV_CM_Bdsgcc_SetSerialNumber;
	DRV_CM_SetDsgMode = &DRV_CM_Bdsgcc_SetDsgMode;
	DRV_CM_SetDsgScan = &DRV_CM_Bdsgcc_SetDsgScan;
	DRV_CM_SetAdminStatus = &DRV_CM_Bdsgcc_SetAdminStatus;
	DRV_CM_GetSwOperStatus = &DRV_CM_Bdsgcc_GetSwOperStatus;
	DRV_CM_SwUpgradeTrigger = &DRV_CM_Bdsgcc_SwUpgradeTrigger;
	DRV_CM_SwUpgrade = &DRV_CM_Bdsgcc_SwUpgrade;
	DRV_CM_GetEstbIp = &DRV_CM_Bdsgcc_GetEstbIp;
	DRV_CM_GetEstbMac = &DRV_CM_Bdsgcc_GetEstbMac;
}


/* end of file */

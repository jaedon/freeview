/*******************************************************************************
* File name : fdi_wifi.h
* Author : D.K Lee
* description :
*
* Copyright (c)  by Humax Co., Ltd. Since 2007.
* All right reserved
*******************************************************************************/
 
#ifndef _FDI_WIFI_H_
#define _FDI_WIFI_H_

/*******************************************************************************
* Headers
*******************************************************************************/
#include "fdi_err.h"

/*******************************************************************************
* Definitions, typdef & const data
*******************************************************************************/
#define FDI_WIFI_MAX_NUM		2

#define MAC_ADDR_LENGTH		6
#define IPV4_ADDR_LENGTH		4
#define FDI_WIFI_ANT_MAX		4

typedef enum 
{
	FDI_WIFI_NOTIFY_EVT_INSERTED,
	FDI_WIFI_NOTIFY_EVT_EXTRACTED,
	FDI_WIFI_NOTIFY_EVT_CONNECT_SUCCESS,
	FDI_WIFI_NOTIFY_EVT_CONNECT_FAIL,
	FDI_WIFI_NOTIFY_EVT_MAX
} FDI_WIFI_EVENT_e;

typedef	struct
{
	unsigned long					ulDevId;
	FDI_WIFI_EVENT_e				eEvent;	
	unsigned long					ulNumOfData;
	void							*pvEventData;
}FDI_WIFI_CALLBACK_DATA_t;

typedef struct
{
	int 			isDhcp;
	unsigned char	mac_addr[MAC_ADDR_LENGTH];
	unsigned char	ip_addr[IPV4_ADDR_LENGTH];
	unsigned char	brd_addr[IPV4_ADDR_LENGTH];
	unsigned char	net_addr[IPV4_ADDR_LENGTH];
	unsigned char	gate_addr[IPV4_ADDR_LENGTH];
	unsigned char	dns_addr[IPV4_ADDR_LENGTH];
	int			mtu;
} FDI_WIFI_NETINFO;


typedef struct
{
	unsigned int	uiAntCount;
	int				iRssi;
	int				iAntRssi[FDI_WIFI_ANT_MAX];
} FDI_WIFI_RSSI_INFO_t;


typedef void (*pfnFDI_WIFI_NotifyFunc )(void  *pEventData);


/*******************************************************************************
* Global variables and structures
*******************************************************************************/


/*******************************************************************************
* External variables and functions
*******************************************************************************/


/*******************************************************************************
* Static variables and structures
*******************************************************************************/


/*******************************************************************************
* Static function prototypes
*******************************************************************************/


/*******************************************************************************
* function : 
* description :
* input :
* output :
* return :
* 기타 참고자료 및 파일
*******************************************************************************/
FDI_ERR_CODE FDI_WIFI_Open(void);

/*******************************************************************************
* function : 
* description :
* input :
* output :
* return :
* 기타 참고자료 및 파일
*******************************************************************************/
FDI_ERR_CODE FDI_WIFI_GetCapability(unsigned long *pulNum);

/*******************************************************************************
* function : 
* description :
* input :
* output :
* return :
* 기타 참고자료 및 파일
*******************************************************************************/
FDI_ERR_CODE FDI_WIFI_RegisterCallback(unsigned long ulIndex, FDI_WIFI_EVENT_e eEvent, pfnFDI_WIFI_NotifyFunc pfnCallback);

/*******************************************************************************
* function : 
* description : 
* input :
* output :
* return :
* 기타 참고자료 및 파일
*******************************************************************************/
FDI_ERR_CODE 	FDI_WIFI_Connect(unsigned long ulIndex, unsigned char *pucApName, unsigned long ulWaitSeconds);

/*******************************************************************************
* function : 
* description : 
* input :
* output :
* return :
* 기타 참고자료 및 파일
*******************************************************************************/
FDI_ERR_CODE 	FDI_WIFI_DisConnect(unsigned long ulIndex);

/*******************************************************************************
* function : 
* description : 
* input :
* output :
* return :
* 기타 참고자료 및 파일
*******************************************************************************/
FDI_ERR_CODE FDI_WIFI_SetAddr(unsigned long ulIndex, FDI_WIFI_NETINFO* pNetworkInfo);



FDI_ERR_CODE FDI_WIFI_GetEachAntRssi(unsigned long ulIndex, FDI_WIFI_RSSI_INFO_t *pRssiInfo);


FDI_ERR_CODE FDI_WIFI_GetApInfo(unsigned long ulIndex, unsigned char *pucSsid);


#endif /* _FDI_WIFI_H_ */


/* end of file */


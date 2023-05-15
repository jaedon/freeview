/******************************************************************************
* $Workfile:$
* $Modtime:$
* Author : 
* description :		 Factory Application
*
* Copyright (c) 2008 by Humax Co., Ltd.
* All right reserved
******************************************************************************/

/*******************************************************************************
* header and definitions
*******************************************************************************/
/* global header files */

#include <string.h>
#include <fcntl.h>



/* chip manufacture's  header files */

/* humax header files */
#include "fvk.h"
#include "fai_common.h"
#include "fai_testmenu.h"
#include "fai_tmd.h"
#include "fai_wifi.h"
#include "fvk.h"

#include "fdi_ethernet.h"
#include "fdi_wifi.h"
#include "di_wifi.h"
#include "di_network.h"
#include "di_err.h"
#include "drv_err.h"
#include "drv_network.h"

/* model specific configuration header */
#include "factorysw_config.h"

/*******************************************************************************
* Debug level
*******************************************************************************/

/*******************************************************************************
* Definitions, typdef & const data
*******************************************************************************/
//#define NFS_TEST

typedef struct FAI_WIFI_MSG
{
	int nCmd;
	unsigned long ulIndex;
} FAI_WIFI_MSG_t;

typedef enum
{
	FAI_WIFI_CONNECTED,
	FAI_WIFI_CONNECTED_FAIL,
	FAI_WIFI_CMD_THROUGHPUT,
	FAI_WIFI_CMD_RSSI,
	FAI_WIFI_CMD_SSID,
	FAI_WIFI_CMD_PASS_UI,
	FAI_WIFI_CMD_FAIL_UI
} FAI_WIFI_EVENT;

typedef struct
{
	unsigned long ulIndex;
	unsigned long ulTime;
}FAI_WIFI_THROUGHPUT_t;

#if defined(CONFIG_DI10)
typedef struct
{
	unsigned int	uiAntCount;
	int				iRssi;
	int				iAntRssi[FDI_WIFI_ANT_MAX];
} FAI_WIFI_RSSI_INFO_t;

FAI_ERR_CODE FAI_WIFI_GetEachAntRssi(unsigned long ulIndex, FAI_WIFI_RSSI_INFO_t *pRssiInfo);
#endif
FAI_ERR_CODE INTRN_FAI_WIFI_Get_QTN_State(int *uiState);
FVK_ERR_CODE INTRN_FAI_WIFI_Test_Timeout_Callback(unsigned long ulTimerId, void *pvParams);


/*******************************************************************************
* External variables and functions
*******************************************************************************/
/* external variables  and functions */

/*******************************************************************************
* Global variables and structures
*******************************************************************************/
/* global variables and structures */
/* global variables and structures */
static BOOL s_bWiFiInserted[FDI_WIFI_MAX_NUM];
static BOOL s_bWiFiConnected[FDI_WIFI_MAX_NUM];
static BOOL s_bWiFiSetupTest[FDI_WIFI_MAX_NUM];
static BOOL s_bWiFiThoughputest[FDI_WIFI_MAX_NUM];
static BOOL s_bWiFiSSID[FDI_WIFI_MAX_NUM];
static BOOL s_bWiFiRSSI[FDI_WIFI_MAX_NUM];
static BOOL s_bWiFiUI;

static unsigned long g_ulNumOfWiFi;
static unsigned long s_ulTestDuration[FDI_WIFI_MAX_NUM];

unsigned long g_uiWifiTimerId;
unsigned char	g_aucCurSSIDName[33];
unsigned char	g_aucPcIp[4];
unsigned char	g_aucRouterIp[4];

#define FAI_WIFI_TEST_TIMEOUT			60*1000
#define FAI_WIFI_UI_TEST_TIMEOUT			70*1000


#if defined(NFS_TEST)
#define AP_2_4_NAME	"3g3t"
#define AP_5_NAME	"ROUTE_5G"
unsigned char g_aucWiFiIpAddr[4] = {192, 168, 11, 112};
unsigned char g_aucWiFiNetAddr[4] = {255, 255, 255, 0};
#else
#define AP_2_4_NAME	"D_2G"
#define AP_5_NAME	"D_5G"
unsigned char g_aucWiFiIpAddr[4] = {192, 168, 200, 47};
unsigned char g_aucWiFiNetAddr[4] = {255, 255, 255, 0};
#endif
static unsigned long g_ulWIFIMsgId, g_ulWIFITaskId, g_ulMSGId;
char 	aucESSID[33];
unsigned char	ulConnectTimeOut = 0;
static BOOL s_bWIFIConnectTimeout=FALSE;

#if ( WIFI_MODULE_TYPE == WIFI_MODULE_QT3490)
#define QTN_RSSI_MAX_RETRY_NUM		20
extern const DRV_NETWORK_DEV_INFO_t s_stDevList[];
#endif
static void INTRN_FAI_WIFI_ThoughPutTimerCallback(unsigned long ulTid, void *arg)
{
	unsigned long 		nVkErr = 0;
	FAI_WIFI_MSG_t 	stWiFiMsg;
	FAI_WIFI_THROUGHPUT_t stThrouoghPut ;

	ulTid = ulTid;
	stThrouoghPut = *((FAI_WIFI_THROUGHPUT_t *)arg);
	
	stWiFiMsg.nCmd = FAI_WIFI_CMD_THROUGHPUT;
	stWiFiMsg.ulIndex = stThrouoghPut.ulIndex;

	nVkErr = VK_MSG_SendTimeout(g_ulWIFIMsgId, &stWiFiMsg, sizeof(FAI_WIFI_MSG_t),0);
	if( nVkErr != 0 )
	{
		FAI_PRINT(FAI_PRT_ERROR, ("[%s] : VK_MSG_SendTimeout Error=%d\n",  __func__, nVkErr));
	}
	
	return ;
}

static void INTRN_FAI_WIFI_ThoughPut(unsigned long ulIndex,unsigned long ulTime)
{
	unsigned long ulTimerId;
	FAI_WIFI_THROUGHPUT_t stThrouoghPut;

	stThrouoghPut.ulIndex = ulIndex;
	stThrouoghPut.ulTime = ulTime;
	VK_TIMER_EventAfter(ulTime,INTRN_FAI_WIFI_ThoughPutTimerCallback,(void *)&stThrouoghPut, sizeof(FAI_WIFI_THROUGHPUT_t), (unsigned long *)&(ulTimerId));
	return;
}

void INTRN_FAI_WIFI_NotifyFunc_Inserted(void *data)
{
	FDI_WIFI_CALLBACK_DATA_t *pstCallbackData;
	unsigned int ulIndex = 0;
	FAI_WIFI_MSG_t tWIFIMsg;

	pstCallbackData = (FDI_WIFI_CALLBACK_DATA_t *)data;
	ulIndex = pstCallbackData->ulDevId;
#if defined(CONFIG_DI10) /* fix the prevent insert&remove hotplug event , only using insert event*/
	if((pstCallbackData->eEvent==FDI_WIFI_NOTIFY_EVT_INSERTED)&&(s_bWiFiInserted[ulIndex] ==FALSE))
#else
if(pstCallbackData->eEvent==FDI_WIFI_NOTIFY_EVT_INSERTED)
#endif
	{
		s_bWiFiInserted[ulIndex] = TRUE;
		FAI_PRINT(0, ("[INTRN_FAI_WIFI_NotifyFunc] : FDI_WIFI_NOTIFY_EVT_INSERTED\n"));
#if defined(CONFIG_WIFI_AARDVARK)&&defined(CONFIG_DI10)
		VK_SYSTEM_Command("cp -rf /bcm_wifi_test /tmp");
		VK_SYSTEM_Command("chmod -R 777 /tmp/bcm_wifi_test");
		VK_SYSTEM_Command("ln -s /tmp/bcm_wifi_test/bin/wl /usr/bin/wl");		
		VK_SYSTEM_Command("/tmp/bcm_wifi_test/wifi_hmx.sh init mfg");
#endif		
	}
}
void INTRN_FAI_WIFI_NotifyFunc_Extracted(void *data)
{
	FDI_WIFI_CALLBACK_DATA_t *pstCallbackData;
	unsigned int ulIndex = 0;
	FAI_WIFI_MSG_t tWIFIMsg;

	pstCallbackData = (FDI_WIFI_CALLBACK_DATA_t *)data;
	ulIndex = pstCallbackData->ulDevId;

	if(pstCallbackData->eEvent==FDI_WIFI_NOTIFY_EVT_EXTRACTED)
	{
			s_bWiFiInserted[ulIndex] = FALSE;
			s_bWiFiConnected[ulIndex] = FALSE;
			FAI_PRINT(0, ("[INTRN_FAI_WIFI_NotifyFunc] : FDI_WIFI_NOTIFY_EVT_EXTRACTED\n"));
	}
}

void INTRN_FAI_WIFI_NotifyFunc_ConnectSuccess(void *data)
{
	FDI_WIFI_CALLBACK_DATA_t *pstCallbackData;
	unsigned int ulIndex = 0;
	FAI_WIFI_MSG_t tWIFIMsg;

	pstCallbackData = (FDI_WIFI_CALLBACK_DATA_t *)data;
	ulIndex = pstCallbackData->ulDevId;

	if(pstCallbackData->eEvent==FDI_WIFI_NOTIFY_EVT_CONNECT_SUCCESS)
	{
		s_bWiFiConnected[ulIndex] = TRUE;
		FAI_PRINT(0,("[INTRN_FAI_WIFI_NotifyFunc] : FDI_WIFI_NOTIFY_EVT_CONNECT_SUCCESS\n"));
		if(s_bWiFiSSID[0] == TRUE)
		{
			tWIFIMsg.nCmd = FAI_WIFI_CMD_SSID;
			FVK_MSG_Send(g_ulWIFIMsgId, &tWIFIMsg, sizeof(FAI_WIFI_MSG_t)); 	
		}
		else if(s_bWiFiRSSI[0]==TRUE)
		{
			tWIFIMsg.nCmd = FAI_WIFI_CMD_RSSI;
			FVK_MSG_Send(g_ulWIFIMsgId, &tWIFIMsg, sizeof(FAI_WIFI_MSG_t)); 	
		}
		else if(s_bWiFiUI==TRUE)
		{
			tWIFIMsg.nCmd = FAI_WIFI_CMD_PASS_UI;
			FVK_MSG_Send(g_ulWIFIMsgId, &tWIFIMsg, sizeof(FAI_WIFI_MSG_t)); 	
		}
		else
		{
			tWIFIMsg.nCmd = FAI_WIFI_CONNECTED;   /* 0x01 means DataCompareTest */
			tWIFIMsg.ulIndex = ulIndex;
			FVK_MSG_Send(g_ulWIFIMsgId, &tWIFIMsg, sizeof(FAI_WIFI_MSG_t));
		}
	}
}

void INTRN_FAI_WIFI_NotifyFunc_ConnectFail(void *data)
{
	FDI_WIFI_CALLBACK_DATA_t *pstCallbackData;
	unsigned int ulIndex = 0;
	FAI_WIFI_MSG_t tWIFIMsg;

	pstCallbackData = (FDI_WIFI_CALLBACK_DATA_t *)data;
	ulIndex = pstCallbackData->ulDevId;

	if(pstCallbackData->eEvent==FDI_WIFI_NOTIFY_EVT_CONNECT_FAIL)
	{
		s_bWiFiConnected[ulIndex] = FALSE;
			FAI_PRINT(0, ("[INTRN_FAI_WIFI_NotifyFunc] : FDI_WIFI_NOTIFY_EVT_CONNECT_FAIL\n"));
			if (s_bWiFiUI==TRUE)
			{
				tWIFIMsg.nCmd = FAI_WIFI_CMD_FAIL_UI;
				FVK_MSG_Send(g_ulWIFIMsgId, &tWIFIMsg, sizeof(FAI_WIFI_MSG_t)); 	
			}
			else
			{
				tWIFIMsg.nCmd = FAI_WIFI_CONNECTED_FAIL;   /* 0x01 means DataCompareTest */
				tWIFIMsg.ulIndex = ulIndex; /* fix the segment fault issue */
				FVK_MSG_Send(g_ulWIFIMsgId, &tWIFIMsg, sizeof(FAI_WIFI_MSG_t));
			}
	}
}


void INTRN_FAI_WIFI_Task(void *pData)
{
	FAI_WIFI_MSG_t tWIFIMsg;
	FVK_ERR_CODE eFvkErr;
	FAI_ERR_CODE eFaiErr = FAI_NO_ERR;
	unsigned char aucTmdResult[5];
   	FAI_TMD_RESULT_T tTMDResult;
	unsigned char aucAgcResult[2];
	unsigned char ucTmdErrCode[2];
	char szFileName[80];
	unsigned char aucResult;
	FDI_WIFI_NETINFO stNetworkInfo;
	FDI_ERR_CODE eFdiErr;
	FILE* fp;
	FDI_WIFI_RSSI_INFO_t pRssiInfo;
	unsigned long ulRSSI0 ,ulRSSI1, ulRSSI2, ulRSSI3;
	char ucRssi0[20];
	char ucRssi1[20];

	unsigned char ucTestStr[32];
//	unsigned char	ucCurSSIDName[64];
	unsigned int *ulState;
	int i=0,j=0,k=0,n=0, scanStatus, nAccess, qcsapiAPcounter, nReadSize;
	char qcsapiStatus[129];
	char cFilebuf[129];
	char cmdBuf[128];	
	bool bAPlive;
	
	DI_WIFI_INFO_t ApInfo;
	unsigned long ulDevId;

	DI_ERR_CODE eDiErr=DI_ERR_OK;
	char qcs_ap_name_SSID[128];
	unsigned char qcs_mac_addr_string[128];
	int qcs_ap_RSSI=0, qcs_ap_protocol=0, qcs_ap_authentication_mode=0, qcs_ap_encryption_modes=0, i_tmp=0, ulLutIdx=0, qcs_ap_channel=0;
	unsigned int qcs_ap_flags=0;
	HUINT32 	ulMac[6];
	int assocStatus=0;

	int nRssiValue=0,nRssiAveValue=0;

	int nSdFd;
	char szCheckStr1[20];
	char szCheckStr2[20];
	char szCheckStr3[20];
	char szCheckStr4[20];	
	unsigned char pb[50];
	unsigned char pucResult[4];

	while(1)
	{
		eFvkErr = FVK_MSG_Receive(g_ulWIFIMsgId, &tWIFIMsg, sizeof(FAI_WIFI_MSG_t));
		if (eFvkErr != FVK_NO_ERR)
		{
			FAI_PRINT(FAI_PRT_WARNING, ("[INTRN_FAI_WIFI_Task] : Error FVK_MSG_Receive() %d\n", eFvkErr));
			continue;
		}

        FVK_MEM_Set( (void *)&tTMDResult, 0x00, sizeof(tTMDResult));		

		switch(tWIFIMsg.nCmd)
		{			
			case FAI_WIFI_CONNECTED:
				VK_SYSTEM_Command("killall iperf");
				VK_SYSTEM_Command("rm -rf /tmp/iperf_ret.txt");
				VK_TASK_Sleep(100);
				if(s_bWiFiSetupTest[tWIFIMsg.ulIndex] == TRUE)
				{
					FAI_PRINT(0, ("[INTRN_FAI_WIFI_Task] : CONNECT_SUCCESS[%d]\n",tWIFIMsg.ulIndex));
					s_bWiFiSetupTest[tWIFIMsg.ulIndex] = FALSE;

					memcpy(stNetworkInfo.ip_addr,g_aucWiFiIpAddr,4);
					memcpy(stNetworkInfo.net_addr,g_aucWiFiNetAddr,4);
					
					FDI_WIFI_SetAddr(tWIFIMsg.ulIndex,&stNetworkInfo);
					
					aucResult = TRUE;						
					tTMDResult.eResult = FAI_TMD_ERR_OK;
					FAI_TMD_AddResultParam(&tTMDResult, &aucResult, sizeof(aucResult));
					FAI_TMD_SetResult(&tTMDResult);

					VK_SYSTEM_Command("iperf -s  > /tmp/iperf_ret.txt &"); 	
					s_bWiFiThoughputest[tWIFIMsg.ulIndex] = TRUE;
				}

				break;
			case FAI_WIFI_CONNECTED_FAIL:
				if(s_bWIFIConnectTimeout)
				{
					FAI_PRINT(0, ("[%s] : Already timeout!\n",__func__));
				}
				else
				{
					FAI_PRINT(0, ("[%s] : Connect Fail!\n",__func__));
					s_bWiFiSetupTest[tWIFIMsg.ulIndex] = FALSE;
					tTMDResult.usMsgId	= g_ulMSGId;
					tTMDResult.eResult = FAI_TMD_ERR_FAIL;
					ucTmdErrCode[0] = FAI_MODULE_TMD_ERR_WIFI;
					ucTmdErrCode[1] = FAI_WIFI_TMD_ERR_CONNECT;
					FAI_TMD_AddResultParam(&tTMDResult, &ucTmdErrCode[0], sizeof(ucTmdErrCode));
					FAI_TMD_SetResult(&tTMDResult);
					goto TEST_END;
				}
				break;
			case FAI_WIFI_CMD_THROUGHPUT:
				if(s_bWiFiThoughputest[tWIFIMsg.ulIndex] == TRUE)
				{
					char	cParsing_cmd1[128];
					char	cParsing_cmd2[128];
					int 	nFileSize=0;
					int 	nReadSize=0;
					FILE	*WifiTempFile = NULL;
					char	*pcFileBuffer=NULL;
					char	*pcTempBuffer=NULL;
					char	*pcTempBuffer2=NULL;	
					
					char	cBitSec[20];
					int 	i=0;
					unsigned long ulResultValue = 0;
					char	cTestResult = 0;
					
					FAI_PRINT(0, ("[INTRN_FAI_WIFI_Task] : FAI_WIFI_CMD_THROUGHPUT\n"));
					VK_SYSTEM_Command("killall iperf");

					s_bWiFiSetupTest[tWIFIMsg.ulIndex] = FALSE;					
				
					sprintf(cParsing_cmd1, "[SUM]  0.0");
					sprintf(cParsing_cmd2, "MBytes");
				
					/***********************************************/
					/* STEP1: scanresults operation Test */
					/***********************************************/
					WifiTempFile = fopen("/tmp/iperf_ret.txt", "rb");
					if(WifiTempFile == NULL)
					{
						FAI_PRINT(FAI_PRT_ERROR, ("[%s] Error: Can't create wifi_tmp.txt\n", __FUNCTION__));
						ucTmdErrCode[1] = FAI_WIFI_TMD_ERR_FILE_OPEN;
						goto TEST_END;
					}
				
					fseek(WifiTempFile, 0, SEEK_END);
				
					nFileSize = ftell(WifiTempFile);
				
					fseek(WifiTempFile, 0, SEEK_SET);
				
					pcFileBuffer = (char *)VK_MEM_Alloc(nFileSize+1);
					if(pcFileBuffer == NULL)
					{
						FAI_PRINT(FAI_PRT_ERROR, ("[%s] Error: Not alloc Memory\n", __FUNCTION__));
						fclose(WifiTempFile);
						ucTmdErrCode[1] = FAI_WIFI_TMD_ERR_FILE_OPEN;
						goto TEST_END;
					}
					
					VK_MEM_Memset(pcFileBuffer, 0x0, sizeof(nFileSize+1));
					nReadSize = fread(pcFileBuffer, 1, nFileSize, WifiTempFile);
					if(nReadSize == 0)
					{
						FAI_PRINT(FAI_PRT_ERROR, ("[%s] Error: Read Fail\n", __FUNCTION__));
						fclose(WifiTempFile);
						VK_MEM_Free(pcFileBuffer);
						ucTmdErrCode[1] = FAI_WIFI_TMD_ERR_FILE_READ;
						goto TEST_END;
					}
				
					fclose(WifiTempFile);
				
					pcTempBuffer = strstr((char *)pcFileBuffer, cParsing_cmd1);
					if(NULL != pcTempBuffer)
					{
						FAI_PRINT(0,("### Parsing start ###(%s)\n",pcTempBuffer));
						pcTempBuffer2 = strstr((char *)pcTempBuffer, cParsing_cmd2);
						FAI_PRINT(0,("### Parsing start ##(%s)\n",pcTempBuffer2));
						if(pcTempBuffer2 != NULL)
						{
							pcTempBuffer2 = pcTempBuffer2+8;
							
							/* Parsing Video Size */
							for(i = 0; i<strlen(pcTempBuffer2); i++)
							{
								if((pcTempBuffer2[i] != ' ')||(pcTempBuffer2[i] != '.'))
								{
									cBitSec[i] = pcTempBuffer2[i];
								}
								else
								{
									break;
								}
							}
							cTestResult = 1;
							ulResultValue = atoi(cBitSec);
						}
					}
					FAI_PRINT(0,("### Ret(%d) ###\n", ulResultValue));
				
					if(cTestResult !=1)
					{
						ucTmdErrCode[0] = FAI_WIFI_TMD_ERR_RECEIVE_DATA;
					}
					if(pcFileBuffer == NULL)
					{
						VK_MEM_Free(pcFileBuffer);
					}
TEST_END:
					if(cTestResult == 1)
					{
						int nTmp;
						unsigned long ulRSSI;
						
						/* TEST OK */
						FDI_WIFI_GetRSSI(0,&nTmp);
						ulRSSI = abs(nTmp);
						FAI_PRINT(0,("### RSSI (%d) ###\n", ulRSSI));
						aucResult = TRUE;						
						tTMDResult.eResult = FAI_TMD_ERR_OK;
						FAI_TMD_AddResultParam(&tTMDResult, &aucResult, sizeof(aucResult));
						FAI_TMD_AddResultParam(&tTMDResult, &ulResultValue, 1);
						FAI_TMD_AddResultParam(&tTMDResult, &ulRSSI, 1);
						FAI_TMD_SetResult(&tTMDResult);
					}
					else
					{
						/* TEST FAIL */
						tTMDResult.eResult = FAI_TMD_ERR_FAIL;
						ucTmdErrCode[0] = FAI_MODULE_TMD_ERR_WIFI;						
						FAI_TMD_AddResultParam(&tTMDResult, &ucTmdErrCode[0], sizeof(ucTmdErrCode));
						FAI_TMD_SetResult(&tTMDResult);
					}

				}
				else
				{
					tTMDResult.eResult = FAI_TMD_ERR_FAIL;
					ucTmdErrCode[0] = FAI_MODULE_TMD_ERR_WIFI;
					ucTmdErrCode[1] = FAI_WIFI_TMD_ERR_REQUEST;
					FAI_TMD_AddResultParam(&tTMDResult, &ucTmdErrCode[0], sizeof(ucTmdErrCode));
					FAI_TMD_SetResult(&tTMDResult);
				}
				break;
		case FAI_WIFI_CMD_RSSI:

#if (PRODUCT_YSR2000==YES)
	while(1)
	{
		VK_TASK_Sleep(1000);
		eFdiErr = INTRN_FAI_WIFI_Get_QTN_State(&ulState);
		if((eFdiErr==FAI_NO_ERR)&&(ulState>2))
		{
			FAI_PRINT(FAI_PRT_ERROR, ("===QTN wait finished===\n"));
			break;
		}
		else if(ulState==1)
		{
			FAI_PRINT(FAI_PRT_ERROR, ("%s():%d Error\n",__func__,__LINE__));
			goto TEST_CONN_FAIL;
		}
	}
	bAPlive=FALSE;
	VK_SYSTEM_Command("rm -rf /var/tmp/qv_scan_list");
	VK_SYSTEM_Command("rm -rf /var/tmp/qv_ap_counter");
	VK_SYSTEM_Command("rm -rf /var/tmp/qv_assoc_status");
	VK_SYSTEM_Command("rm -rf /var/tmp/qv_start_scan");
	VK_SYSTEM_Command("call_qcsapi_sockrpc --host 169.254.119.1 start_scan wifi0 > /var/tmp/qv_start_scan");
	VK_SYSTEM_Command("call_qcsapi_sockrpc --host 169.254.119.1 get_scanstatus wifi0 > /var/tmp/qv_scan_status");	// status 추가 monitor
	fp = fopen("/var/tmp/qv_start_scan", "r");
	if (fp != NULL)
	{
		fgets(cFilebuf, 128, fp);
		VK_sscanf(cFilebuf, "%s", &qcsapiStatus[0]);
		fclose(fp);

		if (VK_strcmp("complete", qcsapiStatus) != 0)
		{
			FAI_PRINT(FAI_PRT_ERROR, ("[%s] QCSAPI start scan error!\n", __FUNCTION__));
			//return FAI_NO_ERR;
		}
		// debug message
		//VK_DBG_Print("  %s(qcsapiStatus='%s')\n", __FUNCTION__, qcsapiStatus);
	}
	else
	{
		FAI_PRINT(FAI_PRT_ERROR, ("[%s] Err fopen\n", __FUNCTION__));
		//return FAI_NO_ERR;;
	}

	/* check the scan status */
	while (scanStatus != 1)
	{
		// debug message
		//VK_DBG_Print("  %s(scanStatus=%d)\n", __FUNCTION__, scanStatus);

		VK_SYSTEM_Command("call_qcsapi_sockrpc --host 169.254.119.1 get_scanstatus wifi0 > /var/tmp/qv_scan_status");
		fp = fopen("/var/tmp/qv_scan_status", "r");
		if (fp != NULL)
		{
			fgets(cFilebuf, 128, fp);
			VK_sscanf(cFilebuf, "%d", &scanStatus);
			fclose(fp);

			// debug message
			if(scanStatus==1)
			{
				FAI_PRINT(FAI_PRT_ERROR, ("%s AP scan finish!(scanStatus=%d)\n", __FUNCTION__, scanStatus));
				VK_TASK_Sleep(2000);
			}
		}
		else
		{
			FAI_PRINT(FAI_PRT_ERROR, ("[%s] Err fopen\n", __FUNCTION__));
			//return FAI_NO_ERR;
		}

		VK_TASK_Sleep(100);
	}
	scanStatus=0;
		
	VK_SYSTEM_Command("call_qcsapi_sockrpc --host 169.254.119.1 get_results_AP_scan wifi0 > /var/tmp/qv_ap_counter");
	fsync();
	while(1)
	{
		nAccess = access("/var/tmp/qv_ap_counter",0);
		if(nAccess == 0)
		{
			FAI_PRINT(FAI_PRT_ERROR, ("[%s] /var/tmp/qv_ap_counter found!\n", __FUNCTION__));
			break;
		}
	}
	fp = fopen("/var/tmp/qv_ap_counter", "r");
	if (fp != NULL)
	{
		fgets(cFilebuf, 128, fp);
		VK_sscanf(cFilebuf, "%d", &qcsapiAPcounter);
		fclose(fp);

		if (qcsapiAPcounter < 0)
		{
			FAI_PRINT(FAI_PRT_ERROR, ("[%s] QCSAPI scaned APs counter error!\n", __FUNCTION__));
			goto TEST_CONN_FAIL;
		}
		FAI_PRINT(FAI_PRT_ERROR, ("  %s(qcsapiAPcounter=%d)\n", __FUNCTION__, qcsapiAPcounter));
	}
	else
	{
		FAI_PRINT(FAI_PRT_ERROR, ("[%s] Err fopen\n", __FUNCTION__));
		goto TEST_CONN_FAIL;
	}

	/* get AP list */
	for (i = 0; i < qcsapiAPcounter; i++)
	{
		VK_snprintf(cmdBuf, sizeof(cmdBuf), "call_qcsapi_sockrpc --host 169.254.119.1 get_properties_AP wifi0 %d >> /var/tmp/qv_scan_list", i);
		VK_SYSTEM_Command(cmdBuf);
		fsync();
	}
	//AP가 존재하는지 확인하기 위한 코드
	fp = fopen("/var/tmp/qv_scan_list","r");
	if(fp != NULL)
	{
		for (i = 0; i < qcsapiAPcounter; i++)
		{
			VK_MEM_Memset(cFilebuf, 0x00, sizeof(cFilebuf));					
			if(fgets(cFilebuf, 1023, fp))
			{
				if(strstr(cFilebuf, g_aucCurSSIDName) != NULL)
				{
					fclose(fp);
					bAPlive=TRUE; /* SSID를 detect */
					FAI_PRINT(FAI_PRT_ERROR, ("AP Number  [%d/%d]\n",i,qcsapiAPcounter));
					VK_snprintf(cmdBuf, sizeof(cmdBuf), "call_qcsapi_sockrpc --host 169.254.119.1 get_properties_AP wifi0 %d > /var/tmp/qv_scan_list", i);
					VK_SYSTEM_Command(cmdBuf);
					VK_TASK_Sleep(5);
					break;
				}
			}
			VK_TASK_Sleep(5);
			if((i==(qcsapiAPcounter-1))&&(bAPlive!=TRUE))
			{
				FAI_PRINT(FAI_PRT_ERROR, ("AP [%s] not Found!!!!\n[SSID, MAC ADDRESS, CH, RSSI, Flags, Protocol, Auth_mode, Encrypt_mode]\n",g_aucCurSSIDName));
				VK_SYSTEM_Command("cat /var/tmp/qv_scan_list");
			}
			VK_TASK_Sleep(5);
		}
		VK_TASK_Sleep(5);
	}
	VK_TASK_Sleep(5);
	s_bWiFiConnected[0]=FALSE;
	eFdiErr = FDI_WIFI_DisConnect(0);
	if(eFdiErr) 
	{
		FAI_PRINT(FAI_PRT_ERROR, ("%s() FDI_WIFI_DisConnect err=0x%x\n",__func__,eFdiErr));
	}
	VK_TASK_Sleep(5);
	if(bAPlive==FALSE)
	{
		FAI_PRINT(FAI_PRT_ERROR, ("%s:%d apscan fail!\n",__func__,__LINE__));
		goto TEST_CONN_FAIL;
	}
#if 1
	n=0;
	while(n<=DI_NETWORK_DEV_MAX)
	{
		if(n==DI_NETWORK_DEV_MAX)
		{
			FAI_PRINT(FAI_PRT_ERROR, ("%s():%d handle Error!\n",__func__,__LINE__));
			goto TEST_CONN_FAIL;
		}
	
		if(s_stDevList[n].eDevType==DI_NETWORK_DEV_WIFI)
		{
			ulDevId=s_stDevList[n].ulDevId;
			FAI_PRINT(FAI_PRT_ERROR, ("%s() WIFI Dev detected!\n",__func__));
			break;
		}
		n++;		
	}
	VK_MEM_Memset(&ApInfo,0,sizeof(DI_WIFI_INFO_t));
	sprintf(ApInfo.aucESSID,"%s",g_aucCurSSIDName);

	/* 정보를 가져옴 */
	fp = fopen("/var/tmp/qv_scan_list", "r");
	if (fp != NULL)
	{
		while(fgets(cFilebuf, 128, fp) != NULL)
		{
			VK_sscanf(cFilebuf, "\"%s %x:%x:%x:%x:%x:%x %d %d %x %d %d %d",
				&qcs_ap_name_SSID[0],
				&ulMac[0], &ulMac[1], &ulMac[2], &ulMac[3], &ulMac[4], &ulMac[5],
				&qcs_ap_channel,
				&qcs_ap_RSSI,
				&qcs_ap_flags,
				&qcs_ap_protocol,
				&qcs_ap_authentication_mode,
				&qcs_ap_encryption_modes );

			VK_TASK_Sleep(100);

			if (qcs_ap_protocol == 0) ApInfo.eSecureType = DI_WIFI_SECURE_OPEN;
			else if (qcs_ap_protocol == 1) ApInfo.eSecureType = DI_WIFI_SECURE_WPA;
			else if (qcs_ap_protocol == 2) ApInfo.eSecureType = DI_WIFI_SECURE_WPA2;
			else if (qcs_ap_protocol == 3) ApInfo.eSecureType = DI_WIFI_SECURE_WPA_WPA2;
			// s_stWiFiInstance.stWiFiInfo[ulIndex].astAPList[i].eSecureType = DI_WIFI_SECURE_WEP; - Not support on QV940

			if (qcs_ap_authentication_mode == 1) ApInfo.eAuthenType = DI_WIFI_AUTHEN_PSK;
			else if (qcs_ap_authentication_mode == 2) ApInfo.eAuthenType = DI_WIFI_AUTHEN_OPEN;

			if (qcs_ap_encryption_modes == 0)
				ApInfo.eEncryptType = DI_WIFI_ENCRYPT_NONE;
			else if (qcs_ap_encryption_modes == 1)
				ApInfo.eEncryptType = DI_WIFI_ENCRYPT_TKIP;
			else if (qcs_ap_encryption_modes == 2)
				ApInfo.eEncryptType = DI_WIFI_ENCRYPT_AES;
			else if (qcs_ap_encryption_modes == 3)
				ApInfo.eEncryptType = DI_WIFI_ENCRYPT_TKIP_AES;
			else
				ApInfo.eEncryptType = DI_WIFI_ENCRYPT_UNKNOWN;
			//s_stWiFiInstance.stWiFiInfo[ulIndex].astAPList[i].eEncryptType = DI_WIFI_ENCRYPT_WEP_64; - Not support on QV940

			if((qcs_ap_encryption_modes)&&(qcs_ap_authentication_mode==1)&&(qcs_ap_protocol))
			{
				snprintf(ApInfo.aucKey,sizeof(ApInfo.aucKey),"%s","123456789a");/* 디폴트 AP Password */
			}
		}
	}
	else
	{
		FAI_PRINT(FAI_PRT_ERROR, ("%s() qv_scan_list not found!\n",__func__));
		goto TEST_CONN_FAIL;
	}

	FAI_PRINT(FAI_PRT_ERROR, ("%s() DI_WIFI_Connect(%d, %d, %d, TimeOut=%d) start\n",__func__,ApInfo.eAuthenType,ApInfo.eSecureType,ApInfo.eEncryptType,ulConnectTimeOut));
	eDiErr = DI_WIFI_Connect(ulDevId,DI_WIFI_WPS_NONE,&ApInfo,ulConnectTimeOut);
	if(eDiErr)
	{
		FAI_PRINT(FAI_PRT_ERROR, ("%s() DI_WIFI_Connect err=0x%x\n",__func__,eDiErr));
		goto TEST_CONN_FAIL;
	}	
#else
	eFdiErr = FDI_WIFI_Connect(0, curSSID, ulConnectTimeOut);
	if(eFdiErr)
	{
		DI_UART_Print("%s() FDI_WIFI_Connect err=0x%x\n",__func__,eFdiErr);
		goto TEST_FAIL;
	}
#endif
	i=0;
	assocStatus=0;
	while(1)
	{
		VK_TASK_Sleep(1000);
		VK_SYSTEM_Command("call_qcsapi_sockrpc --host 169.254.119.1 get_count_assoc  wifi0 > /var/tmp/qv_assoc_status");
		fp = fopen("/var/tmp/qv_assoc_status", "r");
		if (fp != NULL)
		{
//			FAI_PRINT(FAI_PRT_ERROR, ("[%s] opened!(assoc=%d)\n", __FUNCTION__,assocStatus));
			fgets(cFilebuf, 128, fp);
			VK_sscanf(cFilebuf, "%d", &assocStatus);
			fclose(fp);

			if(assocStatus!=1)
			{
				if(!s_bWIFIConnectTimeout)
				{
					FAI_PRINT(FAI_PRT_ERROR, ("[%s] not associate AP(status=%d), time=%d, bTimeout=%d\n",__FUNCTION__,assocStatus,i,s_bWIFIConnectTimeout));
				}
				else
				{
					FAI_PRINT(FAI_PRT_ERROR, ("[%s] Timeout~~~~~\n",__func__));
					goto TEST_CONN_FAIL_RETRY;
					//break;
				}
			}
		}
		else
		{
			FAI_PRINT(FAI_PRT_ERROR, ("[%s] Err fopen\n", __FUNCTION__));
			goto TEST_CONN_FAIL;
		}
		if(assocStatus==1)
		{
			FAI_PRINT(FAI_PRT_ERROR, ("%s() FDI_WIFI_Connect complete\n",__func__));
			VK_SYSTEM_Command("call_qcsapi_sockrpc --host 169.254.119.1 get_count_assoc  wifi0 > /var/tmp/qv_assoc_status");	
//			VK_SYSTEM_Command("echo -n QV_ASSOC_STATUS=&&cat /var/tmp/qv_assoc_status");
			//VK_TASK_Sleep(1000);
			//VK_SYSTEM_Command("echo -n RSSI0=&&call_qcsapi_sockrpc --host 169.254.119.1 get_rssi_by_chain wifi0 0");
			//VK_SYSTEM_Command("echo -n RSSI1=&&call_qcsapi_sockrpc --host 169.254.119.1 get_rssi_by_chain wifi0 1");
			//VK_SYSTEM_Command("echo -n RSSI2=&&call_qcsapi_sockrpc --host 169.254.119.1 get_rssi_by_chain wifi0 2");
			//VK_SYSTEM_Command("echo -n RSSI3=&&call_qcsapi_sockrpc --host 169.254.119.1 get_rssi_by_chain wifi0 3");			
			break;
		}
		i++;
	}
	FAI_PRINT(FAI_PRT_ERROR, ("AP [%s] Found!!!!\n[SSID, MAC ADDRESS, CH, RSSI, Flags, Protocol, Auth_mode, Encrypt_mode]\n",g_aucCurSSIDName));
	VK_SYSTEM_Command("cat /var/tmp/qv_scan_list");

	nAccess = access("/tmp/res",0);
	if(nAccess == 0)
	{
		VK_SYSTEM_Command("rm -rf /tmp/res");
	}
	k=0;
	for(j=0;j<WIFI_READ_SAMPLE;j++)
	{
		VK_SYSTEM_Command("call_qcsapi_sockrpc --host 169.254.119.1 get_rssi_by_chain wifi0 0 > /tmp/res");
		nSdFd = open("/tmp/res", O_RDONLY | O_NONBLOCK);
		if( nSdFd < 0 )
		{
			FAI_PRINT(FAI_PRT_ERROR, ("[FAI_WIFI_QT3940_RSSITest] : get_rssi_by_chain 0 open Error!!\n"));
			goto TEST_RSSI_FAIL;
		}
		/* read file */
		nReadSize = read(nSdFd, pb, 8);
		if( nReadSize < 0 )
		{
			FAI_PRINT(FAI_PRT_ERROR, ("[FAI_WIFI_QT3940_RSSITest] : get_rssi_by_chain 0 read Error!!\n"));
			goto TEST_RSSI_FAIL;
		}
		VK_SYSTEM_Command("rm -rf /tmp/res");
		close(nSdFd);
		nRssiValue = atoi(pb);
#if 1 /* RSSI zero check & retry*/		
		if((nRssiValue==0)&&(k<QTN_RSSI_MAX_RETRY_NUM))
		{
			j=0;
			k++;
			VK_TASK_Sleep(1000);
			FAI_PRINT(FAI_PRT_ERROR, ("Retry...Rssi1 check!!\n"));
			continue;
		}		
#endif		
		nRssiAveValue += nRssiValue;
		//printf(" nRssiValue = %d, nRssiAveValue = %d\n",nRssiValue,nRssiAveValue);
		
	}
	nRssiAveValue = nRssiAveValue/WIFI_READ_SAMPLE;
	FAI_PRINT(FAI_PRT_ERROR, (" Antenna 1 Final :	 nRssiAveValue : %d \n",nRssiAveValue));
	pucResult[0] = abs(nRssiAveValue);
	//printf("pucResult1 : %d\n", pucResult[0]);
	
	nRssiAveValue=0;
	k=0;
	for(j=0;j<WIFI_READ_SAMPLE;j++)
	{
		VK_SYSTEM_Command("call_qcsapi_sockrpc --host 169.254.119.1 get_rssi_by_chain wifi0 1 > /tmp/res");
		nSdFd = open("/tmp/res", O_RDONLY | O_NONBLOCK);
		if( nSdFd < 0 )
		{
			FAI_PRINT(FAI_PRT_ERROR, ("[FAI_WIFI_QT3940_RSSITest] : get_rssi_by_chain 1 open Error!!\n"));
			goto TEST_RSSI_FAIL;
		}
		/* read file */
		nReadSize = read(nSdFd, pb, 8);
		if( nReadSize < 0 )
		{
			FAI_PRINT(FAI_PRT_ERROR, ("[FAI_WIFI_QT3940_RSSITest] : get_rssi_by_chain 1 read Error!!\n"));
			goto TEST_RSSI_FAIL;
		}
		VK_SYSTEM_Command("rm -rf /tmp/res");
		close(nSdFd);
		nRssiValue = atoi(pb);
		nRssiAveValue += nRssiValue;
		//printf(" nRssiValue = %d, nRssiAveValue = %d\n",nRssiValue,nRssiAveValue);
		
	}
	nRssiAveValue = nRssiAveValue/WIFI_READ_SAMPLE;
	FAI_PRINT(FAI_PRT_ERROR, (" Antenna 2 Final :	 nRssiAveValue : %d \n",nRssiAveValue));
	pucResult[1] = abs(nRssiAveValue);
	//printf("pucResult2 : %d\n", pucResult[1]);

	nRssiAveValue=0;
	k=0;
	for(j=0;j<WIFI_READ_SAMPLE;j++)
	{
		VK_SYSTEM_Command("call_qcsapi_sockrpc --host 169.254.119.1 get_rssi_by_chain wifi0 2 > /tmp/res");
		nSdFd = open("/tmp/res", O_RDONLY | O_NONBLOCK);
		if( nSdFd < 0 )
		{
			FAI_PRINT(FAI_PRT_ERROR, ("[FAI_WIFI_QT3940_RSSITest] : get_rssi_by_chain 2 open Error!!\n"));
			goto TEST_RSSI_FAIL;

		}
		/* read file */
		nReadSize = read(nSdFd, pb, 8);
		if( nReadSize < 0 )
		{
			FAI_PRINT(FAI_PRT_ERROR, ("[FAI_WIFI_QT3940_RSSITest] : get_rssi_by_chain 2 read Error!!\n"));
			goto TEST_RSSI_FAIL;
		}
		VK_SYSTEM_Command("rm -rf /tmp/res");
		close(nSdFd);
		nRssiValue = atoi(pb);
		nRssiAveValue += nRssiValue;
		//printf(" nRssiValue = %d, nRssiAveValue = %d\n",nRssiValue,nRssiAveValue);
		
	}
	nRssiAveValue = nRssiAveValue/WIFI_READ_SAMPLE;
	FAI_PRINT(FAI_PRT_ERROR, (" Antenna 3 Final :	 nRssiAveValue : %d \n",nRssiAveValue));
	pucResult[2] = abs(nRssiAveValue);
	//printf("pucResult3 : %d\n", pucResult[2]);

	nRssiAveValue=0;
	k=0;
	for(j=0;j<WIFI_READ_SAMPLE;j++)
	{
		VK_SYSTEM_Command("call_qcsapi_sockrpc --host 169.254.119.1 get_rssi_by_chain wifi0 3 > /tmp/res");
		nSdFd = open("/tmp/res", O_RDONLY | O_NONBLOCK);
		if( nSdFd < 0 )
		{
			FAI_PRINT(FAI_PRT_ERROR, ("[FAI_WIFI_QT3940_RSSITest] : get_rssi_by_chain 3 open Error!!\n"));
			goto TEST_RSSI_FAIL;
		}
		/* read file */
		nReadSize = read(nSdFd, pb, 8);
		if( nReadSize < 0 )
		{
			FAI_PRINT(FAI_PRT_ERROR, ("[FAI_WIFI_QT3940_RSSITest] : get_rssi_by_chain 3 read Error!!\n"));
			goto TEST_RSSI_FAIL;
		}
		VK_SYSTEM_Command("rm -rf /tmp/res");
		close(nSdFd);
		nRssiValue = atoi(pb);
		nRssiAveValue += nRssiValue;
		//printf(" nRssiValue = %d, nRssiAveValue = %d\n",nRssiValue,nRssiAveValue);
		
	}
	nRssiAveValue = nRssiAveValue/WIFI_READ_SAMPLE;
	FAI_PRINT(FAI_PRT_ERROR, (" Antenna 4 Final :	 nRssiAveValue : %d \n",nRssiAveValue));
	pucResult[3] = abs(nRssiAveValue);
	//printf("pucResult4 : %d\n", pucResult[3]);


	nRssiAveValue=0;
	VK_SYSTEM_Command("rm -rf /tmp/res");
	if((pucResult[0]==0)&&(pucResult[1]==0)&&(pucResult[2]==0)&&(pucResult[3]==0))
	{
		FAI_PRINT(FAI_PRT_ERROR, ("[FAI_WIFI_QT3940_RSSITest] : Error!! RSSI All zero\n"));
		goto TEST_RSSI_FAIL;
	}

	tTMDResult.ucCategory = 0x10;
	tTMDResult.ucModule = 0x0F;
	tTMDResult.ucAction = 0x09;
	tTMDResult.eResult = FAI_TMD_ERR_OK;
	tTMDResult.ucNumOfParam = 4;
	tTMDResult.aucParams[0] = 0x01;
	tTMDResult.aucParams[1] = pucResult[0];
	tTMDResult.aucParams[2] = 0x01;
	tTMDResult.aucParams[3] = pucResult[1];
	tTMDResult.aucParams[4] = 0x01;
	tTMDResult.aucParams[5] = pucResult[2];
	tTMDResult.aucParams[6] = 0x01;
	tTMDResult.aucParams[7] = pucResult[3];
	FAI_TMD_SetResult(&tTMDResult);
	FVK_TIMER_Destory(g_uiWifiTimerId);
	FAI_PRINT(FAI_PRT_ERROR, ("%s OK!\n",__func__));
	break;

TEST_RSSI_FAIL:
	tTMDResult.ucCategory = 0x10;
	tTMDResult.ucModule = 0x0F;
	tTMDResult.ucAction = 0x09;
	tTMDResult.eResult = FAI_TMD_ERR_FAIL;
	tTMDResult.ucNumOfParam = 4;
	tTMDResult.aucParams[0] = 0x01;
	tTMDResult.aucParams[1] = pucResult[0];
	tTMDResult.aucParams[2] = 0x01;
	tTMDResult.aucParams[3] = pucResult[1];
	tTMDResult.aucParams[4] = 0x01;
	tTMDResult.aucParams[5] = pucResult[2];
	tTMDResult.aucParams[6] = 0x01;
	tTMDResult.aucParams[7] = pucResult[3]; 	
	ucTmdErrCode[0] = FAI_MODULE_TMD_ERR_COMMON;
	ucTmdErrCode[1] = FAI_WIFI_TMD_ERR_RSSI_READ;
	FAI_TMD_AddResultParam(&tTMDResult, &ucTmdErrCode[0], sizeof(ucTmdErrCode));
	FAI_TMD_SetResult(&tTMDResult);
	FVK_TIMER_Destory(g_uiWifiTimerId);
	FAI_PRINT(FAI_PRT_ERROR, ("%s RSSI FAIL\n",__func__));
	break;
	

TEST_CONN_FAIL:
	tTMDResult.ucCategory = 0x10;
	tTMDResult.ucModule = 0x0F;
	tTMDResult.ucAction = 0x09;
	//tTMDResult.usMsgId	= g_ulMSGId;
	tTMDResult.eResult = FAI_TMD_ERR_FAIL;
	ucTmdErrCode[0] = FAI_MODULE_TMD_ERR_WIFI;
	ucTmdErrCode[1] = FAI_WIFI_TMD_ERR_CONNECT;
	FAI_TMD_AddResultParam(&tTMDResult, &ucTmdErrCode[0], sizeof(ucTmdErrCode));
	FAI_TMD_SetResult(&tTMDResult);
	FVK_TIMER_Destory(g_uiWifiTimerId);
	FAI_PRINT(FAI_PRT_ERROR, ("%s CONNECT or AP NOT FOUND FAIL\n",__func__));	
	break;

TEST_TIMEOUT_FAIL:
	FAI_PRINT(FAI_PRT_ERROR, ("%s TIMEOUT FAIL\n",__func__));
	FVK_MEM_Set(&tTMDResult, 0x00, sizeof(tTMDResult));

	tTMDResult.ucCategory = 0x10;
	tTMDResult.ucModule 	= 0x0F;
	tTMDResult.ucAction 	= 0x09;

	tTMDResult.eResult = FAI_TMD_ERR_FAIL;
	ucTmdErrCode[0] = FAI_MODULE_TMD_ERR_WIFI;
	ucTmdErrCode[1] = FAI_WIFI_TMD_ERR_CONNECT_TIMEOUT;

	FAI_TMD_AddResultParam(&tTMDResult, &ucTmdErrCode[0], sizeof(ucTmdErrCode));
	FAI_TMD_SetResult(&tTMDResult);	
	break;

TEST_CONN_FAIL_RETRY:
	FAI_PRINT(FAI_PRT_ERROR, ("%s CONNECT FAIL RETRY\n",__func__));
	j=0;
	VK_MEM_Memset(&pRssiInfo,0x00,sizeof(pRssiInfo));
	while(1)
	{
		FDI_WIFI_GetEachAntRssi(0, &pRssiInfo);
		if((pRssiInfo.iAntRssi[0]!=0)||(j>20))
		{
			FAI_PRINT(FAI_PRT_ERROR, ("\n%s RSSI Read![j=%d], RSSI[0]=%d,RSSI[1]=%d,RSSI[2]=%d,RSSI[3]=%d,\n",__func__,j,pRssiInfo.iAntRssi[0],pRssiInfo.iAntRssi[1],pRssiInfo.iAntRssi[2],pRssiInfo.iAntRssi[3]));
			break;
		}
		VK_TASK_Sleep(500);
		j++;
	}
	if(j==21)
	{
		FAI_PRINT(FAI_PRT_ERROR, ("%s TIMEOUT FAIL2\n",__func__));
		goto TEST_TIMEOUT_FAIL;
	}
	FAI_PRINT(FAI_PRT_ERROR,("Rssi = %d \n", pRssiInfo.iRssi));
	FAI_PRINT(FAI_PRT_ERROR,("Antena count = %d \n", pRssiInfo.uiAntCount));
	for(i=0; i<pRssiInfo.uiAntCount; i++)
	{
		pucResult[i]=pRssiInfo.iAntRssi[i];
		FAI_PRINT(FAI_PRT_ERROR,("Antenna[%d] Rssi = %d \n", i, pRssiInfo.iAntRssi[i]));
	}	
	tTMDResult.ucCategory = 0x10;
	tTMDResult.ucModule = 0x0F;
	tTMDResult.ucAction = 0x09;
	tTMDResult.eResult = FAI_TMD_ERR_OK;
	tTMDResult.ucNumOfParam = 4;
	tTMDResult.aucParams[0] = 0x01;
	tTMDResult.aucParams[1] = pucResult[0];
	tTMDResult.aucParams[2] = 0x01;
	tTMDResult.aucParams[3] = pucResult[1];
	tTMDResult.aucParams[4] = 0x01;
	tTMDResult.aucParams[5] = pucResult[2];
	tTMDResult.aucParams[6] = 0x01;
	tTMDResult.aucParams[7] = pucResult[3];
	FAI_TMD_SetResult(&tTMDResult);
	FVK_TIMER_Destory(g_uiWifiTimerId);
	FAI_PRINT(FAI_PRT_ERROR, ("%s OK2!\n",__func__));	
	break;
#else

#if defined(CONFIG_DI10)
				eFdiErr = FAI_WIFI_GetEachAntRssi(0, &pRssiInfo);
#else
				eFdiErr = FDI_WIFI_GetEachAntRssi(0, &pRssiInfo);
#endif
				if(eFdiErr == FDI_NO_ERR)
				{
					ulRSSI0 = abs(pRssiInfo.iAntRssi[0]);
					ulRSSI1 = abs(pRssiInfo.iAntRssi[1]);
					FAI_PRINT(0, ("### RSSI (%d) (%d) ###\n", ulRSSI0, ulRSSI1));
					aucResult = TRUE;
					tTMDResult.ucCategory 	= 0x10;
					tTMDResult.ucModule 	= 0x0F;
					tTMDResult.ucAction 		= 0x09;
					tTMDResult.usMsgId		= g_ulMSGId;
#if defined(CONFIG_DI10)
					if((ulRSSI0==0)&&(ulRSSI1==0)) /* error 처리 */
					{
						tTMDResult.eResult = FAI_TMD_ERR_FAIL;
					}
					else
					{
						tTMDResult.eResult = FAI_TMD_ERR_OK;
					}
#else
					tTMDResult.eResult = FAI_TMD_ERR_OK;
#endif
					FAI_TMD_AddResultParam(&tTMDResult, &ulRSSI0, 1);
					FAI_TMD_AddResultParam(&tTMDResult, &ulRSSI1, 1);

					FAI_TMD_SetResult(&tTMDResult);
				}	
				else
				{
					FAI_PRINT(0, ("[INTRN_FAI_WIFI_Task] : FDI_WIFI_GetEachAntRssi FAIL\n"));
					tTMDResult.eResult = FAI_TMD_ERR_FAIL;
					ucTmdErrCode[0] = FAI_MODULE_TMD_ERR_WIFI;
					ucTmdErrCode[1] = FAI_WIFI_TMD_ERR_CONNECT;
					FAI_TMD_AddResultParam(&tTMDResult, &ucTmdErrCode[0], sizeof(ucTmdErrCode));
					FAI_TMD_SetResult(&tTMDResult);
				}
				s_bWiFiRSSI[0]=FALSE;
				s_bWiFiSetupTest[0]=FALSE;
#endif				
				break;

			case FAI_WIFI_CMD_SSID:
				eFdiErr = FDI_WIFI_GetApInfo(0, aucESSID);
				if(eFdiErr == FDI_NO_ERR)
				{
					aucResult = TRUE;
					tTMDResult.ucCategory = 0x10;
					tTMDResult.ucModule 	= 0x0F;
					tTMDResult.ucAction 	= 0x06;
					tTMDResult.usMsgId	= g_ulMSGId;
					tTMDResult.eResult = FAI_TMD_ERR_OK;
					FAI_TMD_AddResultParam(&tTMDResult, aucESSID, strlen(aucESSID));
					FAI_TMD_SetResult(&tTMDResult);
				}	
				else
				{
					FAI_PRINT(0, ("[FDI_WIFI_GetApInfo] : FAIL\n"));
					tTMDResult.ucCategory = 0x10;
					tTMDResult.ucModule 	= 0x0F;
					tTMDResult.ucAction 	= 0x06;
					tTMDResult.usMsgId	= g_ulMSGId;
					tTMDResult.eResult = FAI_TMD_ERR_FAIL;
					ucTmdErrCode[0] = FAI_MODULE_TMD_ERR_WIFI;
					ucTmdErrCode[1] = FAI_WIFI_TMD_ERR_CONNECT;
					FAI_TMD_AddResultParam(&tTMDResult, &ucTmdErrCode[0], sizeof(ucTmdErrCode));
					FAI_TMD_SetResult(&tTMDResult);
				}
				s_bWiFiSSID[0]=FALSE;
				s_bWiFiSetupTest[0]=FALSE;
				break;
			case FAI_WIFI_CMD_PASS_UI:
				
				eFdiErr = FDI_WIFI_GetEachAntRssi(0, &pRssiInfo);
				
				sprintf(ucRssi0, "Ant0 : %d", pRssiInfo.iAntRssi[0]);
				sprintf(ucRssi1, "Ant1 : %d", pRssiInfo.iAntRssi[1]);

				FAI_TESTMENU_ChangeCurrentMenuColor(RGB_COLOR_BLACK);
				if(GET_STEP()==1)
				{
					FAI_TESTMENU_ChangeCurrentMenu(TESTMENU_LINE_2, "WIFI(2_4G) Test", RGB_COLOR_BLACK);
				}
				else
				{
					FAI_TESTMENU_ChangeCurrentMenu(TESTMENU_LINE_2, "WIFI(5G) Test", RGB_COLOR_BLACK);
				}
				FAI_TESTMENU_ChangeCurrentMenu(TESTMENU_LINE_3, ucRssi0, RGB_COLOR_BLACK);
				FAI_TESTMENU_ChangeCurrentMenu(TESTMENU_LINE_4, ucRssi1, RGB_COLOR_BLACK);
				FAI_TESTMENU_ChangeCurrentRCUColor(RGB_COLOR_GREEN);
				break;
				
			case FAI_WIFI_CMD_FAIL_UI:
				FAI_TESTMENU_ChangeCurrentMenuColor(RGB_COLOR_RED);
				FAI_TESTMENU_ChangeCurrentMenu(TESTMENU_LINE_2, "WIFI Test", RGB_COLOR_RED);
				FAI_TESTMENU_ChangeCurrentMenu(TESTMENU_LINE_3, "FAIL", RGB_COLOR_RED);
				FDI_PANEL_Display(FAI_HDD_TEST_FAIL_STR);
				SET_STEP(0);
				break;

		}
	}
}


/*******************************************************************************
* function : 
* description : 
* input :
* output :
* return :
* 기타 참고자료 및 파일
*******************************************************************************/
FAI_ERR_CODE FAI_WIFI_Init(void)
{
	FDI_ERR_CODE eFdiErr;
	FAI_ERR_CODE eRet;
	int i;

	FVK_ERR_CODE eFvkErr;

	eFvkErr = FVK_MSG_Create(FVK_TASK_DEFAULT_QUEUE_SIZE, sizeof(FAI_WIFI_MSG_t), (char*)"WiFiMsg", &g_ulWIFIMsgId);
	if( eFvkErr	!= FVK_NO_ERR )
	{
		FAI_PRINT(FAI_PRT_ERROR, ("[FAI_WIFI_Init] : FVK_MSG_Create Error \n"));
		return FVK_ERR;
	}
	eFvkErr = FVK_TASK_Create(INTRN_FAI_WIFI_Task, FVK_TASK_DEFAULT_PRIORITY, FVK_TASK_DEFAULT_STACK_SIZE*100, "WiFi Task", NULL, &g_ulWIFITaskId);
	if( eFvkErr != FVK_NO_ERR )
	{
		FAI_PRINT(FAI_PRT_ERROR, ("[FAI_WIFI_Init] : FVK_TASK_Create Error!!\n"));
		return FVK_ERR;
	}
	eFvkErr = FVK_TASK_Start(g_ulWIFITaskId);
	if( eFvkErr != FVK_NO_ERR )
	{
		FAI_PRINT(FAI_PRT_ERROR, ("[FAI_WIFI_Init] : FVK_TASK_Start Error!!\n"));
		return FVK_ERR;
	}
	
	eRet = FDI_WIFI_GetCapability(&g_ulNumOfWiFi);
	
	for( i = 0; i < g_ulNumOfWiFi,i < FDI_WIFI_MAX_NUM ; i++ )
	{
		s_bWiFiInserted[i] = FALSE;
		s_bWiFiConnected[i] = FALSE;
		s_bWiFiSetupTest[i] = FALSE;
		s_bWiFiSSID[i] = FALSE;
		s_bWiFiUI=FALSE;
		eRet |= FDI_WIFI_RegisterCallback(i,FDI_WIFI_NOTIFY_EVT_INSERTED,(pfnFDI_WIFI_NotifyFunc)INTRN_FAI_WIFI_NotifyFunc_Inserted);
#if defined(CONFIG_WIFI_AARDVARK)&&defined(CONFIG_DI10)
#else
		eRet |= FDI_WIFI_RegisterCallback(i,FDI_WIFI_NOTIFY_EVT_EXTRACTED,(pfnFDI_WIFI_NotifyFunc)INTRN_FAI_WIFI_NotifyFunc_Extracted);
		eRet |= FDI_WIFI_RegisterCallback(i,FDI_WIFI_NOTIFY_EVT_CONNECT_SUCCESS,(pfnFDI_WIFI_NotifyFunc)INTRN_FAI_WIFI_NotifyFunc_ConnectSuccess);
		eRet |= FDI_WIFI_RegisterCallback(i,FDI_WIFI_NOTIFY_EVT_CONNECT_FAIL,(pfnFDI_WIFI_NotifyFunc)INTRN_FAI_WIFI_NotifyFunc_ConnectFail);
#endif
	}
	
	return FAI_NO_ERR;
}

/****************************************	***************************************
* function : 
* description : 
* input :
* output :
* return :
* 기타 참고자료 및 파일
*******************************************************************************/
FAI_ERR_CODE FAI_WIFI_SSID(unsigned short usMsgId,unsigned char ucArgc, unsigned char *pucArgList)
{
	FAI_TMD_RESULT_T tTMDResult;
	unsigned char ucTmdErrCode[2];
	FDI_ERR_CODE	eFdiErr = FDI_NO_ERR;
	FAI_WIFI_MSG_t tWIFIMsg;
		
	unsigned char 	ulBW = 0;/* 2.4 or 5 Mhz*/
	g_ulMSGId=usMsgId;
	
	if( ucArgc != 2 )
	{
		FAI_PRINT(FAI_PRT_ERROR, ("[%s] : Argument Count Error=%d\n",  __func__, ucArgc));
		tTMDResult.ucCategory = 0x10;
		tTMDResult.ucModule 	= 0x0F;
		tTMDResult.ucAction 	= 0x06;
		tTMDResult.usMsgId	= g_ulMSGId;
		tTMDResult.eResult = FAI_TMD_ERR_FAIL;
		ucTmdErrCode[0] = FAI_MODULE_TMD_ERR_ETHERNET;
		ucTmdErrCode[1] = FAI_COMMON_TMD_ERR_INVALID_ARGUMENT;
		FAI_TMD_AddResultParam(&tTMDResult, &ucTmdErrCode[0], sizeof(ucTmdErrCode));
		FAI_TMD_SetResult(&tTMDResult);
		return FAI_ERR;
	}
		
	FAI_TMD_GetParam((unsigned char*)&ulBW, pucArgList, 0);
	FAI_TMD_GetParam((unsigned char*)&aucESSID, pucArgList, 1);

	eFdiErr=FDI_WIFI_DisConnect(0);
	s_bWiFiConnected[0]=FALSE;
	eFdiErr=FDI_WIFI_Connect(0,aucESSID,20);
	
	if(eFdiErr==FDI_NO_ERR)
	{
		FAI_PRINT(0, ("[%s] : FDI_WIFI_Connect SUCCESS\n", __func__));
		s_bWiFiSSID[0] = TRUE;
		s_bWiFiSetupTest[0]=TRUE;
	}	
	else
	{
		FAI_PRINT(0, ("[%s] : FDI_WIFI_Connect FAIL\n", __func__));
		tTMDResult.ucCategory = 0x10;
		tTMDResult.ucModule 	= 0x0F;
		tTMDResult.ucAction 	= 0x06;
		tTMDResult.usMsgId	= g_ulMSGId;
		tTMDResult.eResult = FAI_TMD_ERR_FAIL;
		ucTmdErrCode[0] = FAI_MODULE_TMD_ERR_WIFI;
		ucTmdErrCode[1] = FAI_WIFI_TMD_ERR_CONNECT;
		FAI_TMD_AddResultParam(&tTMDResult, &ucTmdErrCode[0], sizeof(ucTmdErrCode));
		FAI_TMD_SetResult(&tTMDResult);
		return FAI_ERR;
	}
}
#if defined(CONFIG_DI10)
FAI_ERR_CODE FAI_WIFI_GetEachAntRssi(unsigned long ulIndex, FAI_WIFI_RSSI_INFO_t *pRssiInfo)
{
	unsigned int i,j = 0;
	unsigned long ulDevId;
	FILE *fp;
	char CmdStr[32];	
	char	*pLine;
	char acWlCmdBuf[384];
	char input[1024];
	char searchStr[32];
	FAI_ERR_CODE eFaiErr=FAI_ERR;

	VK_MEM_Memset(pRssiInfo, 0x0, sizeof(FAI_WIFI_RSSI_INFO_t));
		
	if((ulIndex >= FDI_WIFI_MAX_NUM)||(pRssiInfo == NULL))
	{
		FAI_PRINT(FAI_PRT_ERROR, ("[FDI_WIFI_GetEachAntRssi] Input Param ulDevId Err %d\n",ulIndex));
		return FAI_ERR;
	}

	/* init  for Client */
	VK_SYSTEM_Command("wl down");
	//VK_SYSTEM_Command("wl mimo_bw cap 1"); /* not supported */
	VK_SYSTEM_Command("wl obss_coex 1");
	VK_SYSTEM_Command("wl wme 1");
	VK_SYSTEM_Command("wl up");	
	VK_SYSTEM_Command("wl frameburst 1");	
	VK_SYSTEM_Command("wl auth 0");	
	VK_SYSTEM_Command("wl infra 1");	
	VK_SYSTEM_Command("wl wsec 0");	
	VK_SYSTEM_Command("wl wpa_auth 0");	
	VK_SYSTEM_Command("wl sup_wpa 1");
	VK_TASK_Sleep(100);

	/* Get RSSI of each antena & Antena count */

		for(i=0; i<FDI_WIFI_MAX_NUM; i++)
		{
			for(j=0; j<3; j++) /* scan 하지 못한 경우 retry 3 times */
			{
				VK_MEM_Memset(acWlCmdBuf, 0x00, sizeof(acWlCmdBuf));
				snprintf(acWlCmdBuf, sizeof(acWlCmdBuf), "wl rxchain %d && wl escanresults --ssid=%s> /tmp/scan_ap_list.txt", i+1, aucESSID);
				VK_SYSTEM_Command(acWlCmdBuf);

				FAI_PRINT(FAI_PRT_ERROR, ("[FAI_WIFI_GetEachAntRssi] Scanned = %d \n",j));

				//AP가 존재하는지 확인하기 위한 코드
				fp = fopen("/tmp/scan_ap_list.txt","r");
				if(fp != NULL)
				{
					VK_MEM_Memset(input, 0x00, 1024);
					
					pLine = fgets(input, 1023, fp);
					if(pLine != NULL)
					{
						pLine = NULL;
						VK_MEM_Memset(searchStr, 0x00, sizeof(searchStr));
						snprintf(searchStr, sizeof(searchStr), "\"%s\"", aucESSID);

						if((pLine = strstr(input, searchStr)) != NULL)
						{
							eFaiErr = FAI_NO_ERR; /* SSID를 detect 한 경우 no error처리 */
						}
					}
					fclose(fp);
				}
				if(eFaiErr == FAI_NO_ERR)
				{
					/* SSID detect 되면 빠져 나옴 */
					break;
				}
			}

			if(eFaiErr == FAI_NO_ERR)
			{
				fp = fopen("/tmp/scan_ap_list.txt","r");
				if(fp != NULL)
				{
					VK_MEM_Memset(input, 0x00, 1024);
					pLine = NULL;
					pLine = fgets(input, 1023, fp); /* 1st line */
					pLine = fgets(input, 1023, fp); /* 2nd line */
					if(pLine != NULL)
					{
						pLine = strstr(pLine, "RSSI:");
						if(pLine != NULL)
						{
							//FAI_PRINT(FAI_PRT_ERROR, ("[%s]\n",pLine+7));
							sscanf((pLine+7), "%d", &(pRssiInfo->iAntRssi[i]));
							FAI_PRINT(FAI_PRT_ERROR, ("[FAI_WIFI_GetEachAntRssi] Antena[%d] rssi = %d \n",i, pRssiInfo->iAntRssi[i]));
						}
					}
					fclose(fp);
				}
			}
		}
	return FAI_NO_ERR;
}
#endif

/****************************************	***************************************
* function : 
* description : 
* input :
* output :
* return :
* 기타 참고자료 및 파일
*******************************************************************************/
FAI_ERR_CODE FAI_WIFI_RSSI(unsigned short usMsgId,unsigned char ucArgc, unsigned char *pucArgList)
{
	FAI_TMD_RESULT_T tTMDResult;
	FDI_WIFI_RSSI_INFO_t *pRssiInfo;
	FAI_WIFI_MSG_t tWIFIMsg;

	unsigned char ucTmdErrCode[2];
	FDI_ERR_CODE	eFdiErr = FDI_NO_ERR;
	unsigned char	aulPcIp[4],ulPcIpSize = 0;
	unsigned char	aulRouterIp[4],ulRouterIpSize = 0; /* AP IP */
	char	  curSSID[33];
#if defined(CONFIG_DI10)
	char acWlCmdBuf[384];
	FILE *fp;
	char input[1024];
	char searchStr[32];
	char	*pLine;
	int i;
#endif
	g_ulMSGId = usMsgId;
	FVK_MEM_Set(&tTMDResult, 0x00, sizeof(tTMDResult)); /* result가 fail인 경우 result param이 추가되는 bug 수정 */

	if( ucArgc != 4 )
	{
		FAI_PRINT(FAI_PRT_ERROR, ("[%s] : Argument Count Error=%d\n",  __func__, ucArgc));
		tTMDResult.ucCategory = 0x10;
		tTMDResult.ucModule 	= 0x0F;
		tTMDResult.ucAction 	= 0x09;
		tTMDResult.usMsgId	= usMsgId;
		tTMDResult.eResult = FAI_TMD_ERR_FAIL;
		ucTmdErrCode[0] = FAI_MODULE_TMD_ERR_ETHERNET;
		ucTmdErrCode[1] = FAI_COMMON_TMD_ERR_INVALID_ARGUMENT;
		FAI_TMD_AddResultParam(&tTMDResult, &ucTmdErrCode[0], sizeof(ucTmdErrCode));
		FAI_TMD_SetResult(&tTMDResult);
		return FAI_ERR;
	}

	FAI_TMD_GetParam((unsigned char*)&aucESSID, pucArgList, 0);
	FAI_TMD_GetParam((unsigned char*)&ulConnectTimeOut, pucArgList, 1);
	FAI_TMD_GetParam((unsigned char*)&aulPcIp, pucArgList, 2);
	FAI_TMD_GetParam((unsigned char*)&aulRouterIp, pucArgList, 3);

	FAI_PRINT(FAI_PRT_DBGINFO, ("===============================================\n"));
	FAI_PRINT(FAI_PRT_DBGINFO, ("\tConnectTimeOut=%d\n", ulConnectTimeOut));
	FAI_PRINT(FAI_PRT_DBGINFO, ("\tPC IP %d.%d.%d.%d\n", aulPcIp[0],aulPcIp[1],aulPcIp[2],aulPcIp[3]));
	FAI_PRINT(FAI_PRT_DBGINFO, ("\tRouter(AP) IP %d.%d.%d.%d\n", aulRouterIp[0],aulRouterIp[1],aulRouterIp[2],aulRouterIp[3]));
	FAI_PRINT(FAI_PRT_DBGINFO, ("===============================================\n"));

#if 0
	if(s_bWiFiConnected[0]!=TRUE)
	{
		eFdiErr = FDI_WIFI_Connect(0,aucESSID,ulConnectTimeOut);		
		if(eFdiErr==FDI_NO_ERR)
		{
			FAI_PRINT(0, ("[%s] : FDI_WIFI_Connect SUCCESS\n", __func__));
			s_bWiFiRSSI[0] = TRUE;
			s_bWiFiSetupTest[0]=TRUE;
		}	
		else
		{
			FAI_PRINT(0, ("[%s] : FDI_WIFI_Connect FAIL\n", __func__));
			tTMDResult.ucCategory = 0x10;
			tTMDResult.ucModule 	= 0x0F;
			tTMDResult.ucAction 	= 0x09;
			tTMDResult.usMsgId	= g_ulMSGId;
			tTMDResult.eResult = FAI_TMD_ERR_FAIL;
			ucTmdErrCode[0] = FAI_MODULE_TMD_ERR_WIFI;
			ucTmdErrCode[1] = FAI_WIFI_TMD_ERR_CONNECT;
			FAI_TMD_AddResultParam(&tTMDResult, &ucTmdErrCode[0], sizeof(ucTmdErrCode));
			FAI_TMD_SetResult(&tTMDResult);
			return FAI_ERR;
		}				
	}
	else
	{
		eFdiErr = FDI_WIFI_GetApInfo(0, curSSID);
		FAI_PRINT(0, ("curSSID : %s aucESSID : %s\n", curSSID, aucESSID));
		if (&curSSID == &aucESSID)
		{
			FAI_PRINT(0, ("curSSID and aucESSID is same\n", curSSID, aucESSID));
			tWIFIMsg.nCmd = FAI_WIFI_CMD_RSSI;
			FVK_MSG_Send(g_ulWIFIMsgId, &tWIFIMsg, sizeof(FAI_WIFI_MSG_t)); 
		}
		else
		{
			eFdiErr=FDI_WIFI_DisConnect(0);
			s_bWiFiConnected[0]=FALSE;
			eFdiErr = FDI_WIFI_Connect(0,aucESSID,ulConnectTimeOut);		
			if(eFdiErr==FDI_NO_ERR)
			{
				FAI_PRINT(0, ("[%s] : SUCCESS\n", __func__));
				s_bWiFiRSSI[0] = TRUE;
				s_bWiFiSetupTest[0]=TRUE;
			}	
			else
			{
				FAI_PRINT(0, ("[%s] : FAIL\n", __func__));
				tTMDResult.ucCategory = 0x10;
				tTMDResult.ucModule 	= 0x0F;
				tTMDResult.ucAction 	= 0x09;
				tTMDResult.usMsgId	= g_ulMSGId;
				tTMDResult.eResult = FAI_TMD_ERR_FAIL;
				ucTmdErrCode[0] = FAI_MODULE_TMD_ERR_WIFI;
				ucTmdErrCode[1] = FAI_WIFI_TMD_ERR_CONNECT;
				FAI_TMD_AddResultParam(&tTMDResult, &ucTmdErrCode[0], sizeof(ucTmdErrCode));
				FAI_TMD_SetResult(&tTMDResult);
				return FAI_ERR;
			}				
		}
	}
#else
#if defined(CONFIG_DI20)
	eFdiErr=FDI_WIFI_DisConnect(0);
	eFdiErr = FDI_WIFI_Connect(0,aucESSID,ulConnectTimeOut);	
#endif	
	if(eFdiErr==FDI_NO_ERR)
	{
		FAI_PRINT(0, ("[%s] : FDI_WIFI_Connect SUCCESS\n", __func__));
		s_bWiFiRSSI[0] = TRUE;
		s_bWiFiSetupTest[0]=TRUE;
#if defined(CONFIG_DI10)
		s_bWiFiConnected[0] = TRUE;

		tWIFIMsg.nCmd = FAI_WIFI_CMD_RSSI;
		FVK_MSG_Send(g_ulWIFIMsgId, &tWIFIMsg, sizeof(FAI_WIFI_MSG_t)); 
		
		return FAI_NO_ERR;
#endif
	}	
	else
	{	
		/* DI 10은 script 기반의 DI라서 connect fail 판단 불가능. RSSI를 보고 판단해야됨 */
		FAI_PRINT(0, ("[%s] : FDI_WIFI_Connect FAIL\n", __func__));
		tTMDResult.ucCategory = 0x10;
		tTMDResult.ucModule 	= 0x0F;
		tTMDResult.ucAction 	= 0x09;
		tTMDResult.usMsgId	= g_ulMSGId;
		tTMDResult.eResult = FAI_TMD_ERR_FAIL;
		ucTmdErrCode[0] = FAI_MODULE_TMD_ERR_WIFI;
		ucTmdErrCode[1] = FAI_WIFI_TMD_ERR_CONNECT;
		FAI_TMD_AddResultParam(&tTMDResult, &ucTmdErrCode[0], sizeof(ucTmdErrCode));
		FAI_TMD_SetResult(&tTMDResult);
		return FAI_ERR;
	}	
#endif
}


/****************************************	***************************************
* function : 
* description : 
* input :
* output :
* return :
* 기타 참고자료 및 파일
*******************************************************************************/
#if (WIFI_THROUGHPUT_ENABLE==YES)
FAI_ERR_CODE FAI_WIFI_ThroughputSetup(unsigned short usMsgId,unsigned char ucArgc, unsigned char *pucArgList)
{
	FAI_TMD_RESULT_T tTMDResult;
	unsigned char ucTmdErrCode[2];
	FDI_ERR_CODE	eFdiErr = FDI_NO_ERR;
		
	unsigned char 	ulBW = 0;/* 2.4 or 5 Mhz*/
	unsigned char	ulMode = 0;/* RX,TX*/
	unsigned char	ulConnectTimeOut = 0;
	unsigned char	ulIperfDuration = 0;
	unsigned char	aulPcIp[4],ulPcIpSize = 0;
	unsigned char	aulRouterIp[4],ulRouterIpSize = 0; /* AP IP */

	FVK_MEM_Set( (void *)&tTMDResult, 0x00, sizeof(tTMDResult));
	s_ulTestDuration[0] = 0;

	if( ucArgc != 6 )
	{
		FAI_PRINT(FAI_PRT_ERROR, ("[%s] : Argument Count Error=%d\n",  __func__, ucArgc));
		tTMDResult.eResult = FAI_TMD_ERR_FAIL;
		ucTmdErrCode[0] = FAI_MODULE_TMD_ERR_ETHERNET;
		ucTmdErrCode[1] = FAI_COMMON_TMD_ERR_INVALID_ARGUMENT;
		FAI_TMD_AddResultParam(&tTMDResult, &ucTmdErrCode[0], sizeof(ucTmdErrCode));
		FAI_TMD_SetResult(&tTMDResult);
		return FAI_ERR;
	}
		
	FAI_TMD_GetParam((unsigned char*)&ulBW, pucArgList, 0);
	FAI_TMD_GetParam((unsigned char*)&ulMode, pucArgList, 1);
	FAI_TMD_GetParam((unsigned char*)&ulConnectTimeOut, pucArgList, 2);
	FAI_TMD_GetParam((unsigned char*)&ulIperfDuration, pucArgList, 3);
	ulPcIpSize = FAI_TMD_GetParam((unsigned char*)&aulPcIp, pucArgList, 4);
	ulRouterIpSize = FAI_TMD_GetParam((unsigned char*)&aulRouterIp, pucArgList, 5);
	s_ulTestDuration[0] = ulIperfDuration;

	//FAI_TMD_ArgToValue(&tChMsg.tCabTuneParam.ulFrequency, aucFreqeuncy, ucFreqLen);
	FAI_PRINT(FAI_PRT_DBGINFO, ("===============================================\n"));
	FAI_PRINT(FAI_PRT_DBGINFO, ("[%s]\n", __func__));
	FAI_PRINT(FAI_PRT_DBGINFO, ("\tBW=%d(0:2.4MHz, 1:5MHz)\n",ulBW));
	FAI_PRINT(FAI_PRT_DBGINFO, ("\tMODE=%d(0:RX, 1:TX)\n",ulMode));
	FAI_PRINT(FAI_PRT_DBGINFO, ("\tConnectTimeOut=%d\n", ulConnectTimeOut));
	FAI_PRINT(FAI_PRT_DBGINFO, ("\tIperf Duration=%d\n", ulIperfDuration));
	FAI_PRINT(FAI_PRT_DBGINFO, ("\tPC IP %d.%d.%d.%d\n", aulPcIp[0],aulPcIp[1],aulPcIp[2],aulPcIp[3]));
	FAI_PRINT(FAI_PRT_DBGINFO, ("\tRouter(AP) IP %d.%d.%d.%d\n", aulRouterIp[0],aulRouterIp[1],aulRouterIp[2],aulRouterIp[3]));
	FAI_PRINT(FAI_PRT_DBGINFO, ("===============================================\n"));

	FDI_WIFI_DisConnect(0);

	if(ulBW == 0)
	{
		eFdiErr = FDI_WIFI_Connect(0,AP_2_4_NAME,ulConnectTimeOut);		
	}
	else
	{
		eFdiErr = FDI_WIFI_Connect(0,AP_5_NAME,ulConnectTimeOut);
	}

	if(eFdiErr)
	{
		FAI_PRINT(FAI_PRT_ERROR, ("[%s] : FDI_WIFI_Connect Error =%d\n",  __func__, eFdiErr));
		tTMDResult.eResult = FAI_TMD_ERR_FAIL;
		ucTmdErrCode[0] = FAI_MODULE_TMD_ERR_ETHERNET;
		ucTmdErrCode[1] = FAI_COMMON_TMD_ERR_FAIL;
		FAI_TMD_AddResultParam(&tTMDResult, &ucTmdErrCode[0], sizeof(ucTmdErrCode));
		FAI_TMD_SetResult(&tTMDResult);
		return FAI_ERR;
	}
	s_bWiFiSetupTest[0] = TRUE;

	return FAI_ERR;
}

FAI_ERR_CODE FAI_WIFI_ThroughputTest(unsigned short usMsgId,unsigned char ucArgc, unsigned char *pucArgList)
{
	FAI_TMD_RESULT_T tTMDResult;
	unsigned char ucTmdErrCode[2];
	FDI_ERR_CODE	eFdiErr = FDI_NO_ERR;
		
	FVK_MEM_Set( (void *)&tTMDResult, 0x00, sizeof(tTMDResult));

	if( ucArgc != 0 )
	{
		FAI_PRINT(FAI_PRT_ERROR, ("[%s] : Argument Count Error=%d\n",  __func__, ucArgc));
		tTMDResult.eResult = FAI_TMD_ERR_FAIL;
		ucTmdErrCode[0] = FAI_MODULE_TMD_ERR_ETHERNET;
		ucTmdErrCode[1] = FAI_COMMON_TMD_ERR_INVALID_ARGUMENT;
		FAI_TMD_AddResultParam(&tTMDResult, &ucTmdErrCode[0], sizeof(ucTmdErrCode));
		FAI_TMD_SetResult(&tTMDResult);
		return FAI_ERR;
	}
	FAI_PRINT(FAI_PRT_DBGINFO, ("===============================================\n"));
	FAI_PRINT(FAI_PRT_DBGINFO, ("[%s]\n", __func__));
	FAI_PRINT(FAI_PRT_DBGINFO, ("===============================================\n"));

	INTRN_FAI_WIFI_ThoughPut(0,(s_ulTestDuration[0])*1000);
	
}
#endif


#if ( WIFI_MODULE_TYPE == WIFI_MODULE_QT3490)
extern int atoi( const char* );

FAI_ERR_CODE FAI_WIFI_QT3940_GetMacaddr(unsigned char *pucMacAddr)
{
	int nFileSize, nReadSize,nStrComp,nAccess;
	unsigned char pb[50];
	int nSdFd;
	char ucGetMacAddr[12]={0,};
	char ucGetMacAddrTemp[12]={0,};
	volatile int ucGetMacAddrTemp_return[12]={0,};
	unsigned char szSystemCommand[100]={0,};	
	unsigned int i = 0;

	FVK_MEM_Set(szSystemCommand, 0x00, sizeof(szSystemCommand));
	FVK_MEM_Set(ucGetMacAddr, 0x00, sizeof(ucGetMacAddr));
	FVK_MEM_Set(ucGetMacAddrTemp, 0x00, sizeof(ucGetMacAddrTemp));
	FVK_MEM_Set(ucGetMacAddrTemp_return, 0x00, sizeof(ucGetMacAddrTemp_return));

	FAI_PRINT(FAI_PRT_ERROR, ("[FAI_WIFI_QT3940_GetMacaddr] : start\n"));
    access("/tmp/res",0);
 	if(nAccess == 0)
 	{
 		VK_SYSTEM_Command("rm -rf /tmp/res");
 	}
#if 1
	sprintf(szSystemCommand, "call_qcsapi_sockrpc --host 169.254.119.1 get_mac_addr wifi0 > /tmp/res");
	printf("%s\n", szSystemCommand);
	VK_SYSTEM_Command(szSystemCommand);
	VK_SYSTEM_Command("cat /tmp/res");	
#else		
	VK_SYSTEM_Command("call_qcsapi_sockrpc --host 169.254.119.1 get_mac_addr wifi0 > /tmp/res");
#endif	
	nSdFd = open("/tmp/res", O_RDONLY | O_NONBLOCK);
	if( nSdFd < 0 )
	{
		FAI_PRINT(FAI_PRT_ERROR, ("[FAI_WIFI_QT3940_GetMacaddr] : get_mac_addr open Error!!\n"));
		VK_SYSTEM_Command("rm -rf /tmp/res");
		return FAI_ERR;
	}
	/* read file */
	nReadSize = read(nSdFd, pb, 17);
	if( nReadSize < 0 )
	{
		FAI_PRINT(FAI_PRT_ERROR, ("[FAI_WIFI_QT3940_RSSITest] : GetMacaddr read Error!!\n"));
		VK_SYSTEM_Command("rm -rf /tmp/res");
		return FAI_ERR;
	}



	ucGetMacAddr[0] = pb[0];
	ucGetMacAddr[1] = pb[1];

	ucGetMacAddr[2] = pb[3];
	ucGetMacAddr[3] = pb[4];

	ucGetMacAddr[4] = pb[6];
	ucGetMacAddr[5] = pb[7];

	ucGetMacAddr[6] = pb[9];
	ucGetMacAddr[7] = pb[10];

	ucGetMacAddr[8] = pb[12];
	ucGetMacAddr[9] = pb[13];

	ucGetMacAddr[10] = pb[15];
	ucGetMacAddr[11] = pb[16];
	
#if 0 //for debug
	for(i=0; i < 17; i++)
	{
		FAI_PRINT(FAI_PRT_ERROR, ("[FAI_WIFI_QT3940_RSSITest] : pb[%d] = 0x%02X!!\n",i,pb[i]));
	}
#endif

	for(i=0; i < 12; i++)
	{
		if(( '0' <= ucGetMacAddr[i] ) && (ucGetMacAddr[i] <= '9' ))
		{
			ucGetMacAddrTemp_return[i] = ucGetMacAddr[i] - '0';

		}
		else
		{
#if 0 //for debug
			FAI_PRINT(FAI_PRT_ERROR, ("[FAI_WIFI_QT3940_RSSITest] : 1. ucGetMacAddr[%d] = 0x%02X!!\n",i,ucGetMacAddr[i]));
			FAI_PRINT(FAI_PRT_ERROR, ("[FAI_WIFI_QT3940_RSSITest] : 2. 0x%02X!!\n",'A'));
			FAI_PRINT(FAI_PRT_ERROR, ("[FAI_WIFI_QT3940_RSSITest] : 3. 0x%02X!!\n",'E'));
			FAI_PRINT(FAI_PRT_ERROR, ("[FAI_WIFI_QT3940_RSSITest] : 4. 0x%02X!!\n",'B'));
#endif
			if(ucGetMacAddr[i] == 'A')
			{
				ucGetMacAddrTemp_return[i] = 0x0A;
			}
			else if(ucGetMacAddr[i] == 'B')
			{
				ucGetMacAddrTemp_return[i] = 0x0B;
			}
			else if(ucGetMacAddr[i] == 'C')
			{
				ucGetMacAddrTemp_return[i] = 0x0C;
			}
			else if(ucGetMacAddr[i] == 'D')
			{
				ucGetMacAddrTemp_return[i] = 0x0D;
			}
			else if(ucGetMacAddr[i] == 'E')
			{
				ucGetMacAddrTemp_return[i] = 0x0E;
			}
			else
			{
				ucGetMacAddrTemp_return[i] = 0x0F;
			}
		
		}

	}

#if 0 //for debug

	for(i=0; i < 12; i++)
		{
		FAI_PRINT(FAI_PRT_ERROR, ("[FAI_WIFI_QT3940_RSSITest] : ucGetMacAddrTemp_return[%d] = 0x%02X!!\n",i,ucGetMacAddrTemp_return[i]));
		}
#endif

	ucGetMacAddrTemp[0] = (unsigned char)(ucGetMacAddrTemp_return[0] << 4);
	ucGetMacAddrTemp[0] |= (unsigned char)ucGetMacAddrTemp_return[1];

	ucGetMacAddrTemp[1] = (unsigned char)(ucGetMacAddrTemp_return[2] << 4);
	ucGetMacAddrTemp[1] |= (unsigned char)ucGetMacAddrTemp_return[3];

	ucGetMacAddrTemp[2] = (unsigned char)(ucGetMacAddrTemp_return[4] << 4);
	ucGetMacAddrTemp[2] |= (unsigned char)ucGetMacAddrTemp_return[5];

	ucGetMacAddrTemp[3] = (unsigned char)(ucGetMacAddrTemp_return[6] << 4);
	ucGetMacAddrTemp[3] |= (unsigned char)ucGetMacAddrTemp_return[7];

	ucGetMacAddrTemp[4] = (unsigned char)(ucGetMacAddrTemp_return[8] << 4);
	ucGetMacAddrTemp[4] |= (unsigned char)ucGetMacAddrTemp_return[9];

	ucGetMacAddrTemp[5] = (unsigned char)(ucGetMacAddrTemp_return[10] << 4);
	ucGetMacAddrTemp[5] |= (unsigned char)ucGetMacAddrTemp_return[11];

#if 0 //for debug

	for(i=0; i < 6; i++)
		{
		FAI_PRINT(FAI_PRT_ERROR, ("[FAI_WIFI_QT3940_RSSITest] : ucGetMacAddrTemp[%d] = 0x%02X!!\n",i,ucGetMacAddrTemp[i]));
		}
#endif	
	

	FVK_MEM_Copy(pucMacAddr, ucGetMacAddrTemp, 6);
	FAI_PRINT(FAI_PRT_ERROR, ("[FAI_WIFI_QT3940_GetMacaddr] : end\n"));

	return FAI_NO_ERR; /*  20150918 by jmku - no return error */

}

FAI_ERR_CODE FAI_WIFI_QT3940_SetMacaddr(unsigned char *pucMacAddr)
{
	int nFileSize, nReadSize,nStrComp,nAccess;
	unsigned char pb[50]={0,};
	unsigned char szSystemCommand[100]={0,};
	int nSdFd;
	int Buff[6]={0,};

	FVK_MEM_Set(szSystemCommand, 0x00, 100);
	printf(" 0x%02x,0x%02x,0x%02x,0x%02x,0x%02x,0x%02x",pucMacAddr[0],pucMacAddr[1],pucMacAddr[2],pucMacAddr[3],pucMacAddr[4],pucMacAddr[5]); 
	/*
    Buff[0]=( pucMacAddr[0]-0x30)*16 + ( pucMacAddr[1]-0x30);
	Buff[1]=( pucMacAddr[2]-0x30)*16 + ( pucMacAddr[3]-0x30);
	Buff[2]=( pucMacAddr[4]-0x30)*16 + ( pucMacAddr[5]-0x30);
	Buff[3]=( pucMacAddr[6]-0x30)*16 + ( pucMacAddr[7]-0x30);
	Buff[4]=( pucMacAddr[8]-0x30)*16 + ( pucMacAddr[9]-0x30);
	Buff[5]=( pucMacAddr[10]-0x30)*16 + ( pucMacAddr[11]-0x30);
	*/
	
	FAI_PRINT(FAI_PRT_ERROR, ("[FAI_WIFI_QT3940_SetMacaddr] : start\n"));
	access("/tmp/res",0);
 	if(nAccess == 0)
 	{
 		VK_SYSTEM_Command("rm -rf /tmp/res");
 	}
	sprintf(szSystemCommand, "call_qcsapi_sockrpc --host 169.254.119.1 set_mac_addr wifi0 %02x:%02x:%02x:%02x:%02x:%02x > /tmp/res", pucMacAddr[0],pucMacAddr[1],pucMacAddr[2],pucMacAddr[3],pucMacAddr[4],pucMacAddr[5]);
	printf("%s\n", szSystemCommand);
	VK_SYSTEM_Command(szSystemCommand);
	nSdFd = open("/tmp/res", O_RDONLY | O_NONBLOCK);
	if( nSdFd < 0 )
	{
		FAI_PRINT(FAI_PRT_ERROR, ("[FAI_WIFI_QT3940_SetMacaddr] : set_mac_addr open Error!!\n"));
		VK_SYSTEM_Command("rm -rf /tmp/res");
		return FAI_ERR;
	}
	/* read file */
	nReadSize = read(nSdFd, pb, 17);
	if( nReadSize < 0 )
	{
		FAI_PRINT(FAI_PRT_ERROR, ("[FAI_WIFI_QT3940_SetMacaddr] : SetMacaddr read Error!!\n"));
		VK_SYSTEM_Command("rm -rf /tmp/res");
		return FAI_ERR;
	}
	nStrComp = strncmp(pb,"complete",8);
	
	if( nStrComp < 0 || nStrComp > 0 )
	{
		FAI_PRINT(FAI_PRT_ERROR, ("[FAI_WIFI_QT3940_SetMacaddr] : SetMacaddr strncmp Error!!\n"));
		return FAI_ERR;
	}

	FAI_PRINT(FAI_PRT_ERROR, ("[FAI_WIFI_QT3940_SetMacaddr] : end\n"));

	return FAI_NO_ERR;
}



FAI_ERR_CODE FAI_WIFI_QT3940_AssocSSID(unsigned short usMsgId,unsigned char ucArgc, unsigned char *pucArgList)
{
	FAI_TMD_RESULT_T tTMDResult;
	unsigned char ucTmdErrCode[2];
	FDI_ERR_CODE	eFdiErr = FDI_NO_ERR;

	int nFileSize, nReadSize,nStrComp,nAccess;
	unsigned char pb[50];
	int nSdFd;
	unsigned char pucResult[12]={0,};

	FVK_MEM_Set(&tTMDResult, 0x00, sizeof(tTMDResult));
	
	if( ucArgc != 2 )
	{
		FAI_PRINT(FAI_PRT_ERROR, ("[FAI_WIFI_QT3940_AssocSSID] : Argument Count Error=%d\n", ucArgc));
		tTMDResult.eResult = FAI_TMD_ERR_FAIL;
		ucTmdErrCode[0] = FAI_MODULE_TMD_ERR_COMMON;
		ucTmdErrCode[1] = FAI_COMMON_TMD_ERR_INVALID_ARGUMENT;
		FAI_TMD_AddResultParam(&tTMDResult, &ucTmdErrCode[0], sizeof(ucTmdErrCode));	
		FAI_TMD_SetResult(&tTMDResult);
		return FAI_ERR;
	}

	access("/tmp/res",0);
 	if(nAccess == 0)
 	{
 		VK_SYSTEM_Command("rm -rf /tmp/res");
 	}

	VK_SYSTEM_Command("call_qcsapi_sockrpc --host 169.254.119.1 create_SSID wifi0 D_5G > /tmp/res");
	nSdFd = open("/tmp/res", O_RDONLY | O_NONBLOCK);
	if( nSdFd < 0 )
	{
		FAI_PRINT(FAI_PRT_ERROR, ("[FAI_WIFI_QT3940_AssocSSID] : create_SSID open Error!!\n"));
		goto TEST_FAIL;
	}
	/* read file */
	nReadSize = read(nSdFd, pb, 8);
	if( nReadSize < 0 )
	{
		FAI_PRINT(FAI_PRT_ERROR, ("[FAI_WIFI_QT3940_AssocSSID] : create_SSID read Error!!\n"));
		goto TEST_FAIL;
	}	
	nStrComp = strncmp(pb,"complete",8);
	if( nStrComp < 0 || nStrComp > 0 )
	{
		FAI_PRINT(FAI_PRT_ERROR, ("[FAI_WIFI_QT3940_AssocSSID] : create_SSID strncmp Error!!\n"));
		goto TEST_FAIL;
	}
	printf("create_SSID Complete!!\n ");
	VK_SYSTEM_Command("rm -rf /tmp/res");
	close(nSdFd);

	VK_SYSTEM_Command("call_qcsapi_sockrpc --host 169.254.119.1 associate wifi0 D_5G> /tmp/res");
	nSdFd = open("/tmp/res", O_RDONLY | O_NONBLOCK);
	if( nSdFd < 0 )
	{
		FAI_PRINT(FAI_PRT_ERROR, ("[FAI_WIFI_QT3940_AssocSSID] : associate open Error!!\n"));
		return FAI_ERR;
	}
	/* read file */
	nReadSize = read(nSdFd, pb, 8);
	if( nReadSize < 0 )
	{
		FAI_PRINT(FAI_PRT_ERROR, ("[FAI_WIFI_QT3940_AssocSSID] : associate read Error!!\n"));
		goto TEST_FAIL;
	}	
	nStrComp = strncmp(pb,"complete",8);
	
	if( nStrComp < 0 || nStrComp > 0 )
	{
		FAI_PRINT(FAI_PRT_ERROR, ("[FAI_WIFI_QT3940_AssocSSID] : associate strncmp Error!!\n"));
		goto TEST_FAIL;
	}
	printf(" nStrComp : %d, %s \n",nStrComp,pb);
	VK_SYSTEM_Command("rm -rf /tmp/res");
	close(nSdFd);
#if 0
	pucResult[0] = 'Q';
	pucResult[1] = 'u';
	pucResult[2] = 'a';
	pucResult[3] = 'l';	
	pucResult[4] = 'i';
	pucResult[5] = 't';	
	pucResult[6] = 'y';
	pucResult[7] = '0';
	pucResult[8] = '1';
	pucResult[9] = '_';
	pucResult[10] = '5';
	pucResult[11] = 'G';
	
	tTMDResult.ucCategory = 0x10;
	tTMDResult.ucModule = 0x0F;
	tTMDResult.ucAction = 0x06;
	tTMDResult.usMsgId = usMsgId;
	tTMDResult.eResult = FAI_TMD_ERR_OK;
	FAI_TMD_AddResultParam(&tTMDResult,pucResult,sizeof(pucResult));
	FAI_TMD_SetResult(&tTMDResult);
#endif
	tTMDResult.ucCategory = 0x10;
	tTMDResult.ucModule = 0x0F;
	tTMDResult.ucAction = 0x06;
	tTMDResult.eResult = FAI_TMD_ERR_OK;
	tTMDResult.ucNumOfParam = 2;
	tTMDResult.aucParams[0] = 0x01;
	tTMDResult.aucParams[1] = 0x01;
	tTMDResult.aucParams[2] = 0x0B;
	tTMDResult.aucParams[3] = 'Q';
	tTMDResult.aucParams[4] = 'u';
	tTMDResult.aucParams[5] = 'a';
	tTMDResult.aucParams[6] = 'l';	
	tTMDResult.aucParams[7] = 'i';
	tTMDResult.aucParams[8] = 't';	
	tTMDResult.aucParams[9] = 'y';
	tTMDResult.aucParams[10] = '0';
	tTMDResult.aucParams[11] = '1';
	tTMDResult.aucParams[12] = '_';
	tTMDResult.aucParams[13] = '5';
	tTMDResult.aucParams[14] = 'G';

	FAI_TMD_SetResult(&tTMDResult);
	
	
	return FAI_NO_ERR;

TEST_FAIL:
	VK_SYSTEM_Command("call_qcsapi_sockrpc --host 169.254.119.1 remove_SSID wifi0 D_5G");
	VK_SYSTEM_Command("rm -rf /tmp/res");
	tTMDResult.ucCategory = 0x10;
	tTMDResult.ucModule = 0x0F;
	tTMDResult.ucAction = 0x06;
	tTMDResult.eResult = FAI_TMD_ERR_FAIL;
	
	ucTmdErrCode[0] = FAI_MODULE_TMD_ERR_COMMON;
	ucTmdErrCode[1] = FAI_COMMON_TMD_ERR_INVALID_PARAMETER;
	FAI_TMD_AddResultParam(&tTMDResult, &ucTmdErrCode[0], sizeof(ucTmdErrCode));
	FAI_TMD_SetResult(&tTMDResult);
	return FAI_ERR;


}
#if 0
FAI_ERR_CODE FAI_WIFI_QT3940_AssocSSID(void)
{
	FAI_TMD_RESULT_T tTMDResult;
	unsigned char ucTmdErrCode[2];
	FDI_ERR_CODE	eFdiErr = FDI_NO_ERR;

	int nFileSize, nReadSize,nStrComp,nAccess;
	unsigned char pb[50];
	int nSdFd;
	//unsigned char buf=50;

 /*
	if( ucArgc != 0 )
	{
		FAI_PRINT(FAI_PRT_ERROR, ("[%s] : Argument Count Error=%d\n",  __func__, ucArgc));
		tTMDResult.eResult = FAI_TMD_ERR_FAIL;
		ucTmdErrCode[0] = FAI_MODULE_TMD_ERR_ETHERNET;
		ucTmdErrCode[1] = FAI_COMMON_TMD_ERR_INVALID_ARGUMENT;
		FAI_TMD_AddResultParam(&tTMDResult, &ucTmdErrCode[0], sizeof(ucTmdErrCode));
		FAI_TMD_SetResult(&tTMDResult);
		return FAI_ERR;
	}
*/
	access("/tmp/res",0);
 	if(nAccess == 0)
 	{
 		VK_SYSTEM_Command("rm -rf /tmp/res");
 	}

	VK_SYSTEM_Command("call_qcsapi_sockrpc --host 192.168.1.200 associate wifi0 Quality01_5G > /tmp/res");
	nSdFd = open("/tmp/res", O_RDONLY | O_NONBLOCK);
	if( nSdFd < 0 )
	{
		FAI_PRINT(FAI_PRT_ERROR, ("[FAI_WIFI_QT3940_RSSITest] : associate open Error!!\n"));
		VK_SYSTEM_Command("call_qcsapi_sockrpc --host 192.168.1.200 remove_SSID wifi0 Quality01_5G");
		VK_SYSTEM_Command("rm -rf /tmp/res");
		return FAI_ERR;
	}
	/* read file */
	nReadSize = read(nSdFd, pb, 8);
	if( nReadSize < 0 )
	{
		FAI_PRINT(FAI_PRT_ERROR, ("[FAI_WIFI_QT3940_RSSITest] : associate read Error!!\n"));
		VK_SYSTEM_Command("call_qcsapi_sockrpc --host 192.168.1.200 remove_SSID wifi0 Quality01_5G");
		VK_SYSTEM_Command("rm -rf /tmp/res");
		return FAI_ERR;
	}	
	nStrComp = strncmp(pb,"complete",8);
	
	if( nStrComp < 0 || nStrComp > 0 )
	{
		FAI_PRINT(FAI_PRT_ERROR, ("[FAI_WIFI_QT3940_RSSITest] : associate strncmp Error!!\n"));
		VK_SYSTEM_Command("call_qcsapi_sockrpc --host 192.168.1.200 remove_SSID wifi0 Quality01_5G");
		VK_SYSTEM_Command("rm -rf /tmp/res");
		return FAI_ERR;
	}
	printf(" nStrComp : %d, %s \n",nStrComp,pb);
	VK_SYSTEM_Command("rm -rf /tmp/res");
	close(nSdFd);
	
	return FAI_NO_ERR;

}
#endif

FAI_ERR_CODE INTRN_FAI_WIFI_Get_QTN_State(int *uiState)
{
	FAI_ERR_CODE eFaiErr = FAI_ERR;
	unsigned char szSystemCommand[100];
	FILE *fp;
	unsigned char input[256];
	char	*pLine;
	int i=0;
	int nAccess;

	FAI_PRINT(FAI_PRT_ERROR,("[%s] Check for wifi connecting...\n",__func__));
	//while(1)
//	{
	 	if(access("/tmp/res",0) == 0)
	 	{
	 		VK_SYSTEM_Command("rm -rf /tmp/res");
	 	}
		FVK_MEM_Set(szSystemCommand, 0x00, sizeof(szSystemCommand));
		FVK_MEM_Set(input, 0x00, sizeof(input));
		sprintf(szSystemCommand, "call_qcsapi_sockrpc --host 169.254.119.1 get_bootcfg_param calstate > /tmp/res");
		VK_SYSTEM_Command(szSystemCommand);

		*uiState=0;

		//state or wifi connect가 가능한지 확인하기 위한 코드
		fp = fopen("/tmp/res","r");
		if(fp != NULL)
		{
			pLine = fgets(input, 255, fp);
			if(pLine != NULL)
			{			
				pLine = NULL;
				if((pLine = strstr(input,"RPC:")) != NULL)
				{
					eFaiErr = FAI_ERR;
					FAI_PRINT(FAI_PRT_ERROR,("[%s] wait for wifi connecting...[%d]\n",__func__,i));
				}
				else if(!strncmp(input,"0",1))
				{
					FAI_PRINT(FAI_PRT_ERROR,("[%s] invalid wifi calstate[0]...\n",__func__));
					*uiState=0;
					eFaiErr=FAI_NO_ERR; /* cal state !=3 한 경우 no error처리 */
				}			
				else if(!strncmp(input,"1",1))
				{
					FAI_PRINT(FAI_PRT_ERROR,("[%s] invalid wifi calstate[1]...\n",__func__));
					*uiState=1;
					eFaiErr=FAI_NO_ERR; /* cal state !=3 한 경우 no error처리 */
				}
				else if(!strncmp(input,"2",1))
				{
					FAI_PRINT(FAI_PRT_ERROR,("[%s] invalid wifi calstate[2]...\n",__func__));
					*uiState=2;
					eFaiErr=FAI_NO_ERR; /* cal state !=3 한 경우 no error처리 */
				}
				else if(!strncmp(input,"3",1))
				{
					FAI_PRINT(FAI_PRT_ERROR,("[%s] valid wifi calstate...[3]\n",__func__));
					*uiState=3;
					eFaiErr=FAI_NO_ERR; /* cal state !=3 한 경우 no error처리 */
				}						
				else
				{
					eFaiErr = FAI_NO_ERR;
					FAI_PRINT(FAI_PRT_ERROR,("[%s] invalid wifi calstate\n",__func__));
				}
			}
			fclose(fp);
		}

	return eFaiErr;
}

FAI_ERR_CODE FAI_WIFI_QT3940_ReadRSSI(unsigned short usMsgId,unsigned char ucArgc, unsigned char *pucArgList)
{
	FAI_WIFI_MSG_t tWIFIMsg;
	FAI_TMD_RESULT_T tTMDResult;
	unsigned char ucTmdErrCode[2];
	FDI_ERR_CODE	eFdiErr = FDI_NO_ERR;
	unsigned char ucESSIDSize=0;
	FVK_ERR_CODE eFvkErr;

	if( ucArgc != 4 )
	{
		FAI_PRINT(FAI_PRT_ERROR, ("[%s] : Argument Count Error=%d\n",  __func__, ucArgc));
		tTMDResult.ucCategory = 0x10;
		tTMDResult.ucModule 	= 0x0F;
		tTMDResult.ucAction 	= 0x09;
		tTMDResult.usMsgId	= usMsgId;
		tTMDResult.eResult = FAI_TMD_ERR_FAIL;
		ucTmdErrCode[0] = FAI_MODULE_TMD_ERR_WIFI;
		ucTmdErrCode[1] = FAI_COMMON_TMD_ERR_INVALID_ARGUMENT;
		FAI_TMD_AddResultParam(&tTMDResult, &ucTmdErrCode[0], sizeof(ucTmdErrCode));
		FAI_TMD_SetResult(&tTMDResult);
		return FAI_NO_ERR;
	}
	FVK_MEM_Set(g_aucCurSSIDName, 0x00, sizeof(g_aucCurSSIDName));
	FVK_MEM_Set(g_aucPcIp, 0x00, sizeof(g_aucPcIp));
	FVK_MEM_Set(g_aucRouterIp, 0x00, sizeof(g_aucRouterIp));
	
	ucESSIDSize = FAI_TMD_GetParam((unsigned char*)&g_aucCurSSIDName, pucArgList, 0);
	FAI_TMD_GetParam((unsigned char*)&ulConnectTimeOut, pucArgList, 1);
	FAI_TMD_GetParam((unsigned char*)&g_aucPcIp, pucArgList, 2);
	FAI_TMD_GetParam((unsigned char*)&g_aucRouterIp, pucArgList, 3);

	s_bWIFIConnectTimeout=FALSE;

#if (PRODUCT_YSR2000==YES)
//	strcpy(g_aucCurSSIDName,"TDC_5G");
	strcpy(g_aucCurSSIDName,MODEL_AP_NAME);/* YSR-2000과 YS-1000은 AP Name이 다름 */
	ucESSIDSize = strlen(g_aucCurSSIDName);
	FAI_PRINT(FAI_PRT_ERROR, ("\nInput WIFI ConnectTimeOut=%d\n", ulConnectTimeOut));
	if(ulConnectTimeOut<60)
	{
		ulConnectTimeOut=FAI_WIFI_TEST_TIMEOUT/1000;
	}
#endif
	g_aucCurSSIDName[ucESSIDSize] = '\0';

	tWIFIMsg.ulIndex = 0;
	tWIFIMsg.nCmd = FAI_WIFI_CMD_RSSI;
	
	eFvkErr = FVK_MSG_Send(g_ulWIFIMsgId, &tWIFIMsg, sizeof(FAI_WIFI_MSG_t)); 
	if (eFvkErr != FVK_NO_ERR)
	{
		FAI_PRINT(FAI_PRT_ERROR, ("[%s] : Error FVK_MSG_Send() : %d\n", __func__,eFvkErr));

		tTMDResult.ucCategory = 0x10;
		tTMDResult.ucModule 	= 0x0F;
		tTMDResult.ucAction 	= 0x09;
		tTMDResult.eResult = FAI_TMD_ERR_FAIL;
		ucTmdErrCode[0] = FAI_MODULE_TMD_ERR_COMMON;
		ucTmdErrCode[1] = FAI_COMMON_TMD_ERR_MESSAGE_SEND_FAIL;
		FAI_TMD_AddResultParam(&tTMDResult, &ucTmdErrCode[0], sizeof(ucTmdErrCode));
		FAI_TMD_SetResult(&tTMDResult);
		return FAI_ERR;
	}

	FAI_PRINT(FAI_PRT_ERROR, ("===============================================\n"));
	FAI_PRINT(FAI_PRT_ERROR, ("\tSSID=[%s]\n", g_aucCurSSIDName));	
	FAI_PRINT(FAI_PRT_ERROR, ("\tConnectTimeOut=%d\n", ulConnectTimeOut));
	FAI_PRINT(FAI_PRT_ERROR, ("\tPC IP %d.%d.%d.%d\n", g_aucPcIp[0],g_aucPcIp[1],g_aucPcIp[2],g_aucPcIp[3]));
	FAI_PRINT(FAI_PRT_ERROR, ("\tRouter(AP) IP %d.%d.%d.%d\n", g_aucRouterIp[0],g_aucRouterIp[1],g_aucRouterIp[2],g_aucRouterIp[3]));
	FAI_PRINT(FAI_PRT_ERROR, ("===============================================\n"));

	FVK_TIMER_CreateStart((ulConnectTimeOut*1000), INTRN_FAI_WIFI_Test_Timeout_Callback, NULL, 0, FVK_TIMER_ONCE, &g_uiWifiTimerId);

	/*	test 완료 후(success / fail) 후 반드시 FVK_TIMER_Destory(g_uiWifiTimerId); 가 호출되어야함. 	*/
	
	return FAI_NO_ERR;
}

FVK_ERR_CODE INTRN_FAI_WIFI_Test_Timeout_Callback(unsigned long ulTimerId, void *pvParams)
{
	s_bWIFIConnectTimeout=TRUE;

	return FVK_NO_ERR;
}

FAI_ERR_CODE FAI_WIFI_QT3940_RSSITest_UI(void)
{
	FAI_TMD_RESULT_T tTMDResult;
	unsigned char ucTmdErrCode[2];
	FDI_ERR_CODE	eFdiErr = FDI_NO_ERR;
	int nFileSize, nReadSize,nStrComp,nAccess;
	int nRssiValue=0,nRssiAveValue=0;
	unsigned char pb[50];
	int nSdFd;
	unsigned int i,j,k=0;
	//unsigned char buf=50;
	unsigned char ucWifiCmd[256];
	FILE *fp;
	char szCheckStr1[20];
	char szCheckStr2[20];
	char szCheckStr3[20];
	char szCheckStr4[20];	
	unsigned char ucTestMsg[32];
	unsigned char	aulPcIp[4],ulPcIpSize = 0;
	unsigned char	aulRouterIp[4],ulRouterIpSize = 0; /* AP IP */
	char	  curSSID[33];
	unsigned char ucESSIDSize=0;
	char cmdBuf[255], buf[129];

//	unsigned char input[129];
	unsigned char pCmd0[129];
	unsigned char* pTemp = 0;
	BOOL bAPlive=FALSE;
	unsigned char*pLine;
	char input[1024];
	char searchStr[32];
	int qcsapiAPcounter=0;
	char qcsapiStatus[129];
	int scanStatus = 0;
	unsigned int *ulState;
	DI_ERR_CODE eDiErr=DI_ERR_OK;	
	DI_WIFI_INFO_t ApInfo;
	unsigned long ulDevId;
	int assocStatus;
	char qcs_ap_name_SSID[128];
	unsigned char qcs_mac_addr_string[128];
	int qcs_ap_RSSI=0, qcs_ap_protocol=0, qcs_ap_authentication_mode=0, qcs_ap_encryption_modes=0, i_tmp=0, ulLutIdx=0, qcs_ap_channel=0;
	unsigned int qcs_ap_flags=0;
	HUINT32		ulMac[6];
	
	if( GET_STEP() == 0 )
	{

#if (PRODUCT_YSR2000==YES)
		strcpy(curSSID,MODEL_AP_NAME);
#else
		strcpy(curSSID,AP_5_NAME);
#endif
		FAI_PRINT(FAI_PRT_ERROR, ("===============================================\n"));
		FAI_PRINT(FAI_PRT_ERROR, ("\tSSID=[%s]\n", curSSID));	
		FAI_PRINT(FAI_PRT_ERROR, ("\tConnectTimeOut=%d\n", ((FAI_WIFI_UI_TEST_TIMEOUT)/1000))); /* default timeout */
		FAI_PRINT(FAI_PRT_ERROR, ("===============================================\n"));

		/* test time 개선을 위해 ap connect/disconnect 처리를 여기서 넣었음 */
		/* Check WIFI Booting */
		i=0;
		FVK_MEM_Set(ucTestMsg, 0x00, sizeof(ucTestMsg));
		FAI_PRINT(FAI_PRT_ERROR, ("===QTN Checking===\n"));
		while(1)
		{
			sprintf(ucTestMsg,"Wait %d sec",i);
			FAI_TESTMENU_ChangeCurrentMenuColor(RGB_COLOR_YELLOW);
			FAI_TESTMENU_ChangeCurrentMenu(TESTMENU_LINE_1, curSSID, RGB_COLOR_YELLOW);	
			FAI_TESTMENU_ChangeCurrentMenu(TESTMENU_LINE_2, "WIFI", RGB_COLOR_YELLOW);
			FAI_TESTMENU_ChangeCurrentMenu(TESTMENU_LINE_3, "booting", RGB_COLOR_YELLOW);
			FAI_TESTMENU_ChangeCurrentMenu(TESTMENU_LINE_4, ucTestMsg, RGB_COLOR_YELLOW);			
			FAI_TESTMENU_ChangeCurrentRCUColor(RGB_COLOR_BLACK);	
			VK_TASK_Sleep(1000);			
			eFdiErr = INTRN_FAI_WIFI_Get_QTN_State(&ulState);
			if((eFdiErr==FAI_NO_ERR)&&(ulState>2))
			{
				FAI_PRINT(FAI_PRT_ERROR, ("===QTN wait finished===\n"));
				break;
			}
			else if(ulState==1)
			{
				goto TEST_FAIL;
			}
			i++;
		}

		VK_SYSTEM_Command("rm -rf /var/tmp/qv_start_scan");
		VK_SYSTEM_Command("call_qcsapi_sockrpc --host 169.254.119.1 start_scan wifi0 > /var/tmp/qv_start_scan");
		VK_SYSTEM_Command("call_qcsapi_sockrpc --host 169.254.119.1 get_scanstatus wifi0 > /var/tmp/qv_scan_status");	// status 추가 monitor
		fp = fopen("/var/tmp/qv_start_scan", "r");
		if (fp != NULL)
		{
			fgets(buf, 128, fp);
			VK_sscanf(buf, "%s", &qcsapiStatus[0]);
			fclose(fp);
			if (VK_strcmp("complete", qcsapiStatus) != 0)
			{
				FAI_PRINT(FAI_PRT_ERROR, ("[%s] QCSAPI start scan error!\n", __FUNCTION__));
				return FAI_NO_ERR;
			}
		}
		else
		{
			FAI_PRINT(FAI_PRT_ERROR, ("[%s] Err fopen\n", __FUNCTION__));
			return FAI_NO_ERR;
		}

		/* check the scan status */
		while (scanStatus != 1)
		{
			VK_SYSTEM_Command("call_qcsapi_sockrpc --host 169.254.119.1 get_scanstatus wifi0 > /var/tmp/qv_scan_status");
			fp = fopen("/var/tmp/qv_scan_status", "r");
			if (fp != NULL)
			{
				fgets(buf, 128, fp);
				VK_sscanf(buf, "%d", &scanStatus);
				fclose(fp);

				// debug message
				if(scanStatus==1)
				{
					FAI_PRINT(FAI_PRT_ERROR, ("%s AP scan finish!(scanStatus=%d)\n", __FUNCTION__, scanStatus));
					VK_TASK_Sleep(2000);
				}
			}
			else
			{
				FAI_PRINT(FAI_PRT_ERROR, ("[%s] Err fopen\n", __FUNCTION__));
				return FAI_NO_ERR;
			}

			VK_TASK_Sleep(100);
		}
		scanStatus=0;
		VK_SYSTEM_Command("call_qcsapi_sockrpc --host 169.254.119.1 get_results_AP_scan wifi0 > /var/tmp/qv_ap_counter");
		fsync();
		while(1)
		{
			nAccess = access("/var/tmp/qv_ap_counter",0);
			if(nAccess == 0)
			{
				FAI_PRINT(FAI_PRT_ERROR, ("[%s] /var/tmp/qv_ap_counter found!\n", __FUNCTION__));
				break;
			}
		}
		fp = fopen("/var/tmp/qv_ap_counter", "r");
		if (fp != NULL)
		{
			fgets(buf, 128, fp);
			VK_sscanf(buf, "%d", &qcsapiAPcounter);
			fclose(fp);

			if (qcsapiAPcounter < 0)
			{
				FAI_PRINT(FAI_PRT_ERROR, ("[%s] QCSAPI scaned APs counter error!\n", __FUNCTION__));
				goto TEST_FAIL;
			}
			FAI_PRINT(FAI_PRT_ERROR, ("  %s(qcsapiAPcounter=%d)\n", __FUNCTION__, qcsapiAPcounter));
		}
		else
		{
			FAI_PRINT(FAI_PRT_ERROR, ("[%s] Err fopen\n", __FUNCTION__));
			goto TEST_FAIL;
		}

		/* get AP list */
		VK_SYSTEM_Command("rm -rf /var/tmp/qv_scan_list");
		for (i = 0; i < qcsapiAPcounter; i++)
		{
			VK_snprintf(cmdBuf, sizeof(cmdBuf), "call_qcsapi_sockrpc --host 169.254.119.1 get_properties_AP wifi0 %d >> /var/tmp/qv_scan_list", i);
			VK_SYSTEM_Command(cmdBuf);
			fsync();
		}
		//AP가 존재하는지 확인하기 위한 코드
		fp = fopen("/var/tmp/qv_scan_list","r");
		if(fp != NULL)
		{
			for (i = 0; i < qcsapiAPcounter; i++)
			{
				VK_MEM_Memset(input, 0x00, sizeof(input));
				if(fgets(input, 1023, fp))
				{
					//FAI_PRINT(FAI_PRT_ERROR, ("[%d]",i));
					if(strstr(input, curSSID) != NULL)
					{
						fclose(fp);
						FAI_PRINT(FAI_PRT_ERROR, ("\nFound! [%d/%d]\n",i,qcsapiAPcounter));
						bAPlive=TRUE; /* SSID를 detect */
						VK_snprintf(cmdBuf, sizeof(cmdBuf), "call_qcsapi_sockrpc --host 169.254.119.1 get_properties_AP wifi0 %d > /var/tmp/qv_scan_list", i);
						VK_SYSTEM_Command(cmdBuf);
						break;
					}
				}
				if((i==(qcsapiAPcounter-1))&&(bAPlive!=TRUE))
				{
					FAI_PRINT(FAI_PRT_ERROR, ("AP [%s] not Found!!!!\n[SSID, MAC ADDRESS, CH, RSSI, Flags, Protocol, Auth_mode, Encrypt_mode]\n",curSSID));
					VK_SYSTEM_Command("cat /var/tmp/qv_scan_list");
				}				
			}
		}
		else
		{
			FAI_PRINT(FAI_PRT_ERROR, ("Scan list file not found.\n"));
			goto TEST_CONN_FAIL;
		}
	s_bWiFiConnected[0]=FALSE;
	eFdiErr = FDI_WIFI_DisConnect(0);
	if(eFdiErr) 
	{
		FAI_PRINT(FAI_PRT_ERROR, ("%s() FDI_WIFI_DisConnect err=0x%x\n",__func__,eFdiErr));
	}
	if(bAPlive==FALSE)
	{
		FAI_PRINT(FAI_PRT_ERROR, ("AP[%s] not found. fail\n",curSSID));
		goto TEST_CONN_FAIL;
	}		
#if 1
	for(i=0; i<=DI_NETWORK_DEV_MAX; i++)
	{
		if(i==DI_NETWORK_DEV_MAX)
		{
			goto TEST_FAIL;
		}
	
		if(s_stDevList[i].eDevType==DI_NETWORK_DEV_WIFI)
		{
			ulDevId=s_stDevList[i].ulDevId;
			break;
		}
	}
	memset(&ApInfo,0,sizeof(DI_WIFI_INFO_t));
	sprintf(ApInfo.aucESSID,"%s",curSSID);

	/* 정보를 가져옴 */
	fp = fopen("/var/tmp/qv_scan_list", "r");
	if (fp != NULL)
	{
		while(fgets(buf, 128, fp) != NULL)
		{
			VK_sscanf(buf, "\"%s %x:%x:%x:%x:%x:%x %d %d %x %d %d %d",
				&qcs_ap_name_SSID[0],
				&ulMac[0], &ulMac[1], &ulMac[2], &ulMac[3], &ulMac[4], &ulMac[5],
				&qcs_ap_channel,
				&qcs_ap_RSSI,
				&qcs_ap_flags,
				&qcs_ap_protocol,
				&qcs_ap_authentication_mode,
				&qcs_ap_encryption_modes );

			VK_TASK_Sleep(100);

			if (qcs_ap_protocol == 0) ApInfo.eSecureType = DI_WIFI_SECURE_OPEN;
			else if (qcs_ap_protocol == 1) ApInfo.eSecureType = DI_WIFI_SECURE_WPA;
			else if (qcs_ap_protocol == 2) ApInfo.eSecureType = DI_WIFI_SECURE_WPA2;
			else if (qcs_ap_protocol == 3) ApInfo.eSecureType = DI_WIFI_SECURE_WPA_WPA2;
			// s_stWiFiInstance.stWiFiInfo[ulIndex].astAPList[i].eSecureType = DI_WIFI_SECURE_WEP; - Not support on QV940

			if (qcs_ap_authentication_mode == 1) ApInfo.eAuthenType = DI_WIFI_AUTHEN_PSK;
			else if (qcs_ap_authentication_mode == 2) ApInfo.eAuthenType = DI_WIFI_AUTHEN_OPEN;

			if (qcs_ap_encryption_modes == 0)
				ApInfo.eEncryptType = DI_WIFI_ENCRYPT_NONE;
			else if (qcs_ap_encryption_modes == 1)
				ApInfo.eEncryptType = DI_WIFI_ENCRYPT_TKIP;
			else if (qcs_ap_encryption_modes == 2)
				ApInfo.eEncryptType = DI_WIFI_ENCRYPT_AES;
			else if (qcs_ap_encryption_modes == 3)
				ApInfo.eEncryptType = DI_WIFI_ENCRYPT_TKIP_AES;
			else
				ApInfo.eEncryptType = DI_WIFI_ENCRYPT_UNKNOWN;
			//s_stWiFiInstance.stWiFiInfo[ulIndex].astAPList[i].eEncryptType = DI_WIFI_ENCRYPT_WEP_64; - Not support on QV940

			if((qcs_ap_encryption_modes)&&(qcs_ap_authentication_mode==1)&&(qcs_ap_protocol))
			{
				snprintf(ApInfo.aucKey,sizeof(ApInfo.aucKey),"%s","123456789a");/* 디폴트 AP Password */
			}
		}
	}

	FAI_PRINT(FAI_PRT_ERROR, ("%s() DI_WIFI_Connect(%d, %d, %d) start\n",__func__,ApInfo.eAuthenType,ApInfo.eSecureType,ApInfo.eEncryptType));
	eDiErr = DI_WIFI_Connect(ulDevId,DI_WIFI_WPS_NONE,&ApInfo,(FAI_WIFI_UI_TEST_TIMEOUT)/1000); /* default timeout */
	if(eDiErr)
	{
		FAI_PRINT(FAI_PRT_ERROR, ("%s() DI_WIFI_Connect err=0x%x\n",__func__,eDiErr));
		goto TEST_FAIL;
	}	
#else
	eFdiErr = FDI_WIFI_Connect(0, curSSID, ulConnectTimeOut);
	if(eFdiErr)
	{
		DI_UART_Print("%s() FDI_WIFI_Connect err=0x%x\n",__func__,eFdiErr);
		goto TEST_FAIL;
	}
#endif
	i=0;
	while(1)
	{
		FVK_MEM_Set(ucTestMsg, 0x00, sizeof(ucTestMsg));
		sprintf(ucTestMsg,"Wait %d sec",i);
		FAI_TESTMENU_ChangeCurrentMenuColor(RGB_COLOR_YELLOW);
		FAI_TESTMENU_ChangeCurrentMenu(TESTMENU_LINE_1, curSSID, RGB_COLOR_YELLOW);			
		FAI_TESTMENU_ChangeCurrentMenu(TESTMENU_LINE_2, "WIFI", RGB_COLOR_YELLOW);
		FAI_TESTMENU_ChangeCurrentMenu(TESTMENU_LINE_3, "Connecting", RGB_COLOR_YELLOW);
		FAI_TESTMENU_ChangeCurrentMenu(TESTMENU_LINE_4, ucTestMsg, RGB_COLOR_YELLOW);			
		FAI_TESTMENU_ChangeCurrentRCUColor(RGB_COLOR_BLACK);		

		VK_TASK_Sleep(1000);

		if((i>=((FAI_WIFI_UI_TEST_TIMEOUT)/1000))&&(s_bWiFiConnected[0]==FALSE))
		{
			FAI_PRINT(FAI_PRT_ERROR, ("error! connecting timeout %d sec.\n",i));
			FDI_WIFI_DisConnect(0);
			SET_STEP(3);
			i=0;
			break;
		}
		else if(s_bWiFiConnected[0]==TRUE)
		{
			VK_SYSTEM_Command("call_qcsapi_sockrpc --host 169.254.119.1 get_count_assoc  wifi0 > /var/tmp/qv_assoc_status");	
//			VK_SYSTEM_Command("echo -n QV_ASSOC_STATUS=&&cat /var/tmp/qv_assoc_status");
//			VK_SYSTEM_Command("echo -n RSSI0=&&call_qcsapi_sockrpc --host 169.254.119.1 get_rssi_by_chain wifi0 0");
//			VK_SYSTEM_Command("echo -n RSSI1=&&call_qcsapi_sockrpc --host 169.254.119.1 get_rssi_by_chain wifi0 1");
//			VK_SYSTEM_Command("echo -n RSSI2=&&call_qcsapi_sockrpc --host 169.254.119.1 get_rssi_by_chain wifi0 2");
//			VK_SYSTEM_Command("echo -n RSSI3=&&call_qcsapi_sockrpc --host 169.254.119.1 get_rssi_by_chain wifi0 3");				
			SET_STEP(1);
			break;
		}		
		VK_SYSTEM_Command("call_qcsapi_sockrpc --host 169.254.119.1 get_count_assoc  wifi0 > /var/tmp/qv_assoc_status");	
		fp = fopen("/var/tmp/qv_assoc_status", "r");
		if (fp != NULL)
		{
			fgets(buf, 128, fp);
			VK_sscanf(buf, "%d", &assocStatus);
			fclose(fp);
	
			if(assocStatus!=1)
			{
				FAI_PRINT(FAI_PRT_ERROR, ("[%s] not associate AP(status=%d)\n",__FUNCTION__,assocStatus));
				//i++;
				//continue;
			}
		}
		else
		{
			FAI_PRINT(FAI_PRT_ERROR, ("[%s] Err fopen\n", __FUNCTION__));
			continue;
		}

		i++;
	}

	}
	if( GET_STEP() == 3 )
	{
		goto TEST_CONN_FAIL; /* Fail */
	}	
	if( GET_STEP() == 2 )
	{
		goto TEST_FAIL; /* Fail */
	}	
	else if( GET_STEP() == 1 )
	{
		nAccess = access("/tmp/res",0);
	 	if(nAccess == 0)
	 	{
	 		VK_SYSTEM_Command("rm -rf /tmp/res");
	 	}
		k=0;
		nRssiAveValue=0;
		for(j=0;j<WIFI_READ_SAMPLE;j++)
		{
			//FAI_PRINT(FAI_PRT_ERROR, ("Read...Rssi (%d)\n",j));
			VK_SYSTEM_Command("call_qcsapi_sockrpc --host 169.254.119.1 get_rssi_by_chain wifi0 0 > /tmp/res");
			nSdFd = open("/tmp/res", O_RDONLY | O_NONBLOCK);
			if( nSdFd < 0 )
			{
				FAI_PRINT(FAI_PRT_ERROR, ("[FAI_WIFI_QT3940_RSSITest] : get_rssi_by_chain 0 open Error!!\n"));
				goto TEST_FAIL;
			}
			/* read file */
			nReadSize = read(nSdFd, pb, 8);
			if( nReadSize < 0 )
			{
				FAI_PRINT(FAI_PRT_ERROR, ("[FAI_WIFI_QT3940_RSSITest] : get_rssi_by_chain 0 read Error!!\n"));
				goto TEST_FAIL;
			}
			close(nSdFd);
			nRssiValue = atoi(pb);
			if((nRssiValue==0)&&(k<QTN_RSSI_MAX_RETRY_NUM))
			{
				j=0;
				k++;
				i++;
				VK_TASK_Sleep(1000);
				FVK_MEM_Set(ucTestMsg, 0x00, sizeof(ucTestMsg));
				sprintf(ucTestMsg,"Wait %d sec",i);
				FAI_TESTMENU_ChangeCurrentMenuColor(RGB_COLOR_YELLOW);
				FAI_TESTMENU_ChangeCurrentMenu(TESTMENU_LINE_1, curSSID, RGB_COLOR_YELLOW);			
				FAI_TESTMENU_ChangeCurrentMenu(TESTMENU_LINE_2, "WIFI", RGB_COLOR_YELLOW);
				FAI_TESTMENU_ChangeCurrentMenu(TESTMENU_LINE_3, "Connecting", RGB_COLOR_YELLOW);
				FAI_TESTMENU_ChangeCurrentMenu(TESTMENU_LINE_4, ucTestMsg, RGB_COLOR_YELLOW);			
				FAI_TESTMENU_ChangeCurrentRCUColor(RGB_COLOR_BLACK);						
				//FAI_PRINT(FAI_PRT_ERROR, ("Retry...Rssi1 check!! end\n"));
				continue;
			}
			else
			{
				nRssiAveValue += nRssiValue;
				//FAI_PRINT(FAI_PRT_ERROR, (" nRssiValue = %d, nRssiAveValue = %d\n",nRssiValue,nRssiAveValue));
			}
		}
		nRssiAveValue = nRssiAveValue/WIFI_READ_SAMPLE;
		FAI_PRINT(FAI_PRT_ERROR, (" Antenna 1 Final :   nRssiAveValue : %d \n",nRssiAveValue));
		sprintf(szCheckStr1, "Ant1 %d",nRssiAveValue);

		nRssiAveValue=0;
		for(j=0;j<WIFI_READ_SAMPLE;j++)
		{
			VK_SYSTEM_Command("call_qcsapi_sockrpc --host 169.254.119.1 get_rssi_by_chain wifi0 1 > /tmp/res");
			nSdFd = open("/tmp/res", O_RDONLY | O_NONBLOCK);
			if( nSdFd < 0 )
			{
				FAI_PRINT(FAI_PRT_ERROR, ("[FAI_WIFI_QT3940_RSSITest] : get_rssi_by_chain 1 open Error!!\n"));
				goto TEST_FAIL;
			}
			/* read file */
			nReadSize = read(nSdFd, pb, 8);
			if( nReadSize < 0 )
			{
				FAI_PRINT(FAI_PRT_ERROR, ("[FAI_WIFI_QT3940_RSSITest] : get_rssi_by_chain 1 read Error!!\n"));
				goto TEST_FAIL;
			}
			VK_SYSTEM_Command("rm -rf /tmp/res");
			close(nSdFd);
			nRssiValue = atoi(pb);
			nRssiAveValue += nRssiValue;

			//FAI_PRINT(FAI_PRT_ERROR, (" nRssiValue = %d, nRssiAveValue = %d\n",nRssiValue,nRssiAveValue));
			
		}
		nRssiAveValue = nRssiAveValue/WIFI_READ_SAMPLE;
		FAI_PRINT(FAI_PRT_ERROR, (" Antenna 2 Final :   nRssiAveValue : %d \n",nRssiAveValue));
		sprintf(szCheckStr2, "Ant2 %d",nRssiAveValue);

		nRssiAveValue=0;
		for(j=0;j<WIFI_READ_SAMPLE;j++)
		{
			VK_SYSTEM_Command("call_qcsapi_sockrpc --host 169.254.119.1 get_rssi_by_chain wifi0 2 > /tmp/res");
			nSdFd = open("/tmp/res", O_RDONLY | O_NONBLOCK);
			if( nSdFd < 0 )
			{
				FAI_PRINT(FAI_PRT_ERROR, ("[FAI_WIFI_QT3940_RSSITest] : get_rssi_by_chain 2 open Error!!\n"));
				goto TEST_FAIL;

			}
			/* read file */
			nReadSize = read(nSdFd, pb, 8);
			if( nReadSize < 0 )
			{
				FAI_PRINT(FAI_PRT_ERROR, ("[FAI_WIFI_QT3940_RSSITest] : get_rssi_by_chain 2 read Error!!\n"));
				goto TEST_FAIL;
			}
			VK_SYSTEM_Command("rm -rf /tmp/res");
			close(nSdFd);
			nRssiValue = atoi(pb);
			nRssiAveValue += nRssiValue;

			//FAI_PRINT(FAI_PRT_ERROR, (" nRssiValue = %d, nRssiAveValue = %d\n",nRssiValue,nRssiAveValue));
			
		}
		nRssiAveValue = nRssiAveValue/WIFI_READ_SAMPLE;
		FAI_PRINT(FAI_PRT_ERROR, (" Antenna 3 Final :   nRssiAveValue : %d \n",nRssiAveValue));
		sprintf(szCheckStr3, "Ant3 %d",nRssiAveValue);

		nRssiAveValue=0;
		for(j=0;j<WIFI_READ_SAMPLE;j++)
		{
			VK_SYSTEM_Command("call_qcsapi_sockrpc --host 169.254.119.1 get_rssi_by_chain wifi0 3 > /tmp/res");
			nSdFd = open("/tmp/res", O_RDONLY | O_NONBLOCK);
			if( nSdFd < 0 )
			{
				FAI_PRINT(FAI_PRT_ERROR, ("[FAI_WIFI_QT3940_RSSITest] : get_rssi_by_chain 3 open Error!!\n"));
				goto TEST_FAIL;
			}
			/* read file */
			nReadSize = read(nSdFd, pb, 8);
			if( nReadSize < 0 )
			{
				FAI_PRINT(FAI_PRT_ERROR, ("[FAI_WIFI_QT3940_RSSITest] : get_rssi_by_chain 3 read Error!!\n"));
				goto TEST_FAIL;
			}
			VK_SYSTEM_Command("rm -rf /tmp/res");
			close(nSdFd);
			nRssiValue = atoi(pb);
			nRssiAveValue += nRssiValue;

			//FAI_PRINT(FAI_PRT_ERROR, (" nRssiValue = %d, nRssiAveValue = %d\n",nRssiValue,nRssiAveValue));
			
		}
		nRssiAveValue = nRssiAveValue/WIFI_READ_SAMPLE;
		FAI_PRINT(FAI_PRT_ERROR, (" Antenna 4 Final :   nRssiAveValue : %d \n",nRssiAveValue));
		sprintf(szCheckStr4, "Ant4 %d",nRssiAveValue);
		FAI_TESTMENU_ChangeCurrentMenu(TESTMENU_LINE_1, szCheckStr1, RGB_COLOR_BLACK);
		FAI_TESTMENU_ChangeCurrentMenu(TESTMENU_LINE_2, szCheckStr2, RGB_COLOR_BLACK);
		FAI_TESTMENU_ChangeCurrentMenu(TESTMENU_LINE_3, szCheckStr3, RGB_COLOR_BLACK);
		FAI_TESTMENU_ChangeCurrentMenu(TESTMENU_LINE_4, szCheckStr4, RGB_COLOR_BLACK);
		nRssiAveValue=0;
		VK_SYSTEM_Command("rm -rf /tmp/res");
//		sprintf(ucWifiCmd,"call_qcsapi_sockrpc --host 169.254.119.1 remove_SSID wifi0 %s",curSSID);
//		VK_SYSTEM_Command(ucWifiCmd);		
		SET_STEP(0);
	}
	
	return FAI_NO_ERR;

TEST_FAIL:
	FAI_TESTMENU_ChangeCurrentMenuColor(RGB_COLOR_RED);
	FAI_TESTMENU_ChangeCurrentMenu(TESTMENU_LINE_1, curSSID, RGB_COLOR_RED);		
	FAI_TESTMENU_ChangeCurrentMenu(TESTMENU_LINE_2, "WIFI RSSI", RGB_COLOR_RED);
	FAI_TESTMENU_ChangeCurrentMenu(TESTMENU_LINE_3, "FAIL", RGB_COLOR_RED);
	SET_STEP(0);
	return FAI_NO_ERR;	

TEST_CONN_FAIL:
	FAI_TESTMENU_ChangeCurrentMenuColor(RGB_COLOR_RED);
	FAI_TESTMENU_ChangeCurrentMenu(TESTMENU_LINE_1, curSSID, RGB_COLOR_RED);		
	FAI_TESTMENU_ChangeCurrentMenu(TESTMENU_LINE_2, "WIFI Connect", RGB_COLOR_RED);
	FAI_TESTMENU_ChangeCurrentMenu(TESTMENU_LINE_3, "FAIL", RGB_COLOR_RED);
	SET_STEP(0);
	return FAI_NO_ERR;		
}

#endif

FAI_ERR_CODE FAI_WIFI_RSSITest_UI(void)
{
	FDI_ERR_CODE	eFdiErr = FDI_NO_ERR;
	s_bWiFiUI=TRUE;
	if( GET_STEP() == 0 )
	{
		FAI_TESTMENU_ChangeCurrentMenuColor(RGB_COLOR_YELLOW);
		FAI_TESTMENU_ChangeCurrentMenu(TESTMENU_LINE_2, "WIFI RSSI", RGB_COLOR_YELLOW);
		FAI_TESTMENU_ChangeCurrentMenu(TESTMENU_LINE_3, "2_4G", RGB_COLOR_YELLOW);
		FAI_TESTMENU_ChangeCurrentRCUColor(RGB_COLOR_GREEN);
		SET_STEP(1);

		eFdiErr=FDI_WIFI_DisConnect(0);
		eFdiErr=FDI_WIFI_Connect(0,AP_2_4_NAME,20);
		
	}	
	else if( GET_STEP() == 1 )
	{
		FDI_PANEL_Display(FAI_AUDIO_MUTE_OFF_STR);
		FAI_TESTMENU_ChangeCurrentMenuColor(RGB_COLOR_YELLOW);
		FAI_TESTMENU_ChangeCurrentMenu(TESTMENU_LINE_2, "WIFI RSSI", RGB_COLOR_YELLOW);
		FAI_TESTMENU_ChangeCurrentMenu(TESTMENU_LINE_3, "5G", RGB_COLOR_YELLOW);
		FAI_TESTMENU_ChangeCurrentRCUColor(RGB_COLOR_GREEN);
		SET_STEP(0);

		eFdiErr=FDI_WIFI_DisConnect(0);
		eFdiErr=FDI_WIFI_Connect(0,AP_5_NAME,20);
	}		
	return FAI_NO_ERR;			

TEST_FAIL:
	FAI_TESTMENU_ChangeCurrentMenuColor(RGB_COLOR_RED);
	FAI_TESTMENU_ChangeCurrentMenu(TESTMENU_LINE_2, "WIFI Test", RGB_COLOR_RED);
	FAI_TESTMENU_ChangeCurrentMenu(TESTMENU_LINE_3, "FAIL", RGB_COLOR_RED);
	FDI_PANEL_Display(FAI_HDD_TEST_FAIL_STR);
	SET_STEP(0);
	return FAI_ERR;	

}

#if(WIFI_CONDUCT_UI_ENABLE==YES)
/* added for WIFI Conducted Test */
FAI_ERR_CODE FAI_WIFI_Bridge_Setup_For_Conducted_Test_UI(void)
{
	FDI_ERR_CODE	eFdiErr = FDI_NO_ERR;

	if( GET_STEP() == 0 )
	{
		FAI_TESTMENU_ChangeCurrentMenuColor(RGB_COLOR_YELLOW);
		FAI_TESTMENU_ChangeCurrentMenu(TESTMENU_LINE_2, "WIFI", RGB_COLOR_YELLOW);
		FAI_TESTMENU_ChangeCurrentMenu(TESTMENU_LINE_3, "Conducted", RGB_COLOR_YELLOW);
		FAI_TESTMENU_ChangeCurrentMenu(TESTMENU_LINE_4, "Run", RGB_COLOR_YELLOW);
		FAI_TESTMENU_ChangeCurrentRCUColor(RGB_COLOR_GREEN);
		SET_STEP(1);
		VK_SYSTEM_Command("ifconfig eth0:1 down");
		VK_SYSTEM_Command("ifconfig eth1:1 down");
		VK_SYSTEM_Command("ifconfig eth1:2 down");
		VK_SYSTEM_Command("brctl addbr br0; brctl addif br0 eth0; brctl addif br0 eth1; ifconfig br0 192.168.1.50 up");
		FAI_PRINT(FAI_PRT_ERROR, ("[FAI_WIFI_Bridge_Setup_For_Conducted_Test_UI] : Run!!\n"));

	}	
	else if( GET_STEP() == 1 )
	{
		FAI_TESTMENU_ChangeCurrentMenuColor(RGB_COLOR_YELLOW);
		FAI_TESTMENU_ChangeCurrentMenu(TESTMENU_LINE_2, "WIFI", RGB_COLOR_YELLOW);
		FAI_TESTMENU_ChangeCurrentMenu(TESTMENU_LINE_3, "Conducted", RGB_COLOR_YELLOW);
		FAI_TESTMENU_ChangeCurrentMenu(TESTMENU_LINE_4, "Finish", RGB_COLOR_YELLOW);
		FAI_TESTMENU_ChangeCurrentRCUColor(RGB_COLOR_GREEN);
		SET_STEP(0);
		/* recovery 불가 */
		FAI_PRINT(FAI_PRT_ERROR, ("[FAI_WIFI_Bridge_Setup_For_Conducted_Test_UI] : Finish!!\n"));		
	}		
	return FAI_NO_ERR;			

}
#endif


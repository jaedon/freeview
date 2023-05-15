/**
 * @file	di_cm.c
 * @brief	Channel DI Layer C source
 * @version $Revision: 1.0.0 $
 * @date 	$Date: 2008/07/26 11:00:00 $
 * @author 	nhlee
 */

/** @defgroup DI_CM
 *  This is CM status module of DI layer.
 *  @{
 */

/******************************************************************************
 * Include Files
 ******************************************************************************/
#include "htype.h"
#include "vkernel.h"

#if defined(CONFIG_VK_STDLIB)
#else
#include 	<stdio.h>
#include 	<stdlib.h>
#include 	<string.h>
#endif

/* di headers */
#include "di_err.h"
#include "di_cm.h"
#if defined(CONFIG_CABLE_MODEM_BRCM_DSGCC)
#include "drv_cm_bdsgcc.h"
#endif
#if defined(CONFIG_CABLE_MODEM_BRCM_RPC)
#include "drv_cm_brpc.h"
#endif
#include "drv_err.h"
#include "di_channel_priv.h"
#include "drv_nvram.h"

#if defined(CONFIG_CABLE_MODEM_LED_STATUS)
#include "di_led.h"
#include "drv_micom.h"
#endif

#include "drv_network.h"

#include "drv_gpio.h"

/* NEW DI DEBUG PRINT */
HINT32 show_di_cm = 0;

#if defined(CONFIG_CABLE_MODEM)

/******************************************************************************
 * Symbol and Macro Definitions
 ******************************************************************************/
#define DSG_TASK_PRIORITY			61
#define DSG_TASK_STACK_SIZE			(1024*2)//(1024*2)

#define CM_HOST_DHCP_MONITOR	3000

HINT32	gStbIpGot = 0, gStbPrvIpGot = 0;

#if defined(CONFIG_CABLE_MODEM_LED_STATUS)
HUINT8 gucCmStatusChange = 0;
HINT32 gulPrvCmStatus = 0;
#endif

HINT32 gulCurrCmStatus = 0;

/******************************************************************************
 * Data Types
 ******************************************************************************/

/******************************************************************************
 * Local(static) Data Types
 ******************************************************************************/

/******************************************************************************
 * Extern Variables
 ******************************************************************************/
extern DRV_NETWORK_DEV_INFO_t s_stDevList[];

/******************************************************************************
 * Global Variables
 ******************************************************************************/


/******************************************************************************
 * Local(static) Variables
 ******************************************************************************/
typedef struct
{
	pfnDI_CM_NotifyLinkInfo 		CmLinkCallback;
}CM_LinkInfo_t;

CM_LinkInfo_t		sCmLinkInfo;

#if defined(CONFIG_EURO_DOCSIS)
static HUINT8 s_ulEchoTestStart = 0;
#endif

/******************************************************************************
 * Extern Function Prototypes
 ******************************************************************************/
extern int InitializeDsgCcApplication(void);

extern HUINT32 DRV_CFG_GetNetMaxDevice(void);

#if defined(CONFIG_CABLE_MODEM_LED_STATUS)
extern HUINT32 g_ulCmLedId;
#endif

#define NUM_OF_CM_DEFAULT_MAC 2
static HUINT8 s_aucCmDefaultMac[NUM_OF_CM_DEFAULT_MAC][6] = 
{
	{0x00, 0x10, 0x18, 0xde, 0xad, 0x01},
	{0x08, 0xeb, 0x74, 0xde, 0xad, 0x01},
};

/******************************************************************************
 * Local(static) Function Prototypes
 ******************************************************************************/
static void StartHostIpTask(void *param)
{
	HUINT32 ulLoop;
	HINT8 acCmdBuf[64]={0,};
	HINT8 acIfName[IF_NAME_SIZE] = {0,};

	param=param;

	VK_TASK_Sleep(CM_HOST_DHCP_MONITOR);

	for(ulLoop = 0; ulLoop < DRV_CFG_GetNetMaxDevice(); ulLoop++)
	{
		if(s_stDevList[ulLoop].eDevType == DI_NETWORK_DEV_CABLEMODEM)
		{
			CM_DI_Print(4,("[%s][%d] DevId : %d IfName : %s \n", __FUNCTION__, __LINE__, s_stDevList[ulLoop].ulDevId, s_stDevList[ulLoop].szName));
			VK_strncpy(acIfName, s_stDevList[ulLoop].szName, IF_NAME_SIZE-1 );
			break;
		}
	}

	while(1)
	{
		VK_TASK_Sleep(CM_HOST_DHCP_MONITOR);

		DRV_CM_GetCmRegStatus(&gulCurrCmStatus);

		if (sCmLinkInfo.CmLinkCallback != NULL)
		{
			if( gulCurrCmStatus == 12)
			{
				if(gStbIpGot == 0)
				{
					CM_DI_Print(1, ("[%s][%d] DI_CM_EVENT_ONLINE .......\n", __FUNCTION__, __LINE__));
					VK_snprintf(acCmdBuf,sizeof(acCmdBuf),"route add 192.168.100.1 %s",acIfName);
					VK_SYSTEM_Command(acCmdBuf);
					sCmLinkInfo.CmLinkCallback(DI_CM_EVENT_ONLINE );
					gStbIpGot = 1;
					gStbPrvIpGot = 1;
				}
			}
			else
			{
				if((gStbPrvIpGot == 1)&&( gulCurrCmStatus != 12 ) )
				{
					CM_DI_Print(1, ("[%s][%d] DI_CM_EVENT_OFF.......\n", __FUNCTION__, __LINE__));
					sCmLinkInfo.CmLinkCallback(DI_CM_EVENT_OFFLINE);
					gStbPrvIpGot = 0;
					gStbIpGot = 0;
				}
			}
		}

#if defined(CONFIG_CABLE_MODEM_LED_STATUS)
		CM_DI_Print(3, ("[%s][%d] CM Status Previous:Current = %d:%d\n", __FUNCTION__, __LINE__, gulPrvCmStatus, gulCurrCmStatus));
		if (gulCurrCmStatus != gulPrvCmStatus)
		{
			gucCmStatusChange = TRUE;
		}
		else
		{
			gucCmStatusChange = FALSE;
		}

		if (gucCmStatusChange == TRUE)
		{
			if (gulCurrCmStatus == 2)
			{
				DRV_MICOM_SetBlinkingTime(g_ulCmLedId,0);
			}
			else if (gulCurrCmStatus == 3)
			{
				DRV_MICOM_SetBlinkingTime(g_ulCmLedId,11);
			}
			else if ((gulCurrCmStatus >= 4)&&(gulCurrCmStatus <= 5))
			{
				DRV_MICOM_SetBlinkingTime(g_ulCmLedId,8);
			}
			else if ((gulCurrCmStatus >= 6)&&(gulCurrCmStatus <= 7))
			{
				DRV_MICOM_SetBlinkingTime(g_ulCmLedId,5);
			}
			else if (gulCurrCmStatus == 12)
			{
				DRV_MICOM_SetBlinkingTime(g_ulCmLedId,0xFF);
			}
			else
			{
				/* do nothing */
			}

			gulPrvCmStatus = gulCurrCmStatus;
		}
#endif
	}
}


static void StartDsgccTask(void* param)
{
	param = param;
	InitializeDsgCcApplication();
	return;
}

DRV_Error  DRV_CM_StartDsgCC(void)
{
	int nRet 	= DRV_OK;
	HULONG		ulDsgInitTaskID = 0;
        HULONG	ulHostIpTask = 0;
	HUINT8	ucSerialNo[14]={0,}, ucSerialNoByCm[14]={0,};
	HUINT8	ulIdx = 0, aucCmMac[6] = {0,}, aucCmMacByCm[14] = {0,};

	/* add DRV API install functions */
	DRV_CM_API_Install();

	nRet = VK_TASK_Create(	StartDsgccTask,
							DSG_TASK_PRIORITY,
							DSG_TASK_STACK_SIZE,
							"DSG-CC",
							NULL,
							&ulDsgInitTaskID,
							0 );

	if ((ulDsgInitTaskID == 0) || (nRet != VK_OK))
	{
		CM_DI_Print(0, ("[%s][%d] nRet(0x%x)\n", __FUNCTION__, __LINE__, nRet));
		return DRV_ERR;
	}
	else
	{
		VK_TASK_Start(ulDsgInitTaskID);
	}


	nRet = VK_TASK_Create(  StartHostIpTask,
							DSG_TASK_PRIORITY,
							DSG_TASK_STACK_SIZE,
							"HostIP",
							NULL,
							&ulHostIpTask,
							0 );

	if ((ulHostIpTask == 0) || (nRet != VK_OK))
	{
		CM_DI_Print(0, ("[%s][%d] nRet(0x%x)\n", __FUNCTION__, __LINE__, nRet));
		return DRV_ERR;
	}
	else
	{
		VK_TASK_Start(ulHostIpTask);
	}

	nRet = DRV_CM_StartRebootMonitoringTask();
	if (nRet != DRV_OK)
	{
		CM_DI_Print(0, ("[%s][%d] nRet(0x%x)\n", __FUNCTION__, __LINE__, nRet));
		return DRV_ERR;
	}

	/* Check whether CM serial number is ok on CM flash */
	nRet = DRV_NVRAM_GetField(DI_NVRAM_FIELD_SERIAL_NO, 0, &ucSerialNo[0], 14);
	CM_DI_Print(0, ("[%s][%d] SerialNo[%s]\n", __FUNCTION__, __LINE__, ucSerialNo));

	if (nRet != DRV_OK)
	{
		CM_DI_Print(0, ("[%s][%d] nRet(0x%x)\n", __FUNCTION__, __LINE__, nRet));
	}
	else
	{
		if (ucSerialNo[0] != 0)
		{
			nRet = DRV_CM_GetSerialNumber(ucSerialNoByCm);
			if (nRet != DRV_OK)
			{
					CM_DI_Print(0, ("[%s][%d] nRet(0x%x)\n", __FUNCTION__, __LINE__, nRet));
			}

			if (VK_strncmp(ucSerialNo, ucSerialNoByCm, 14) != 0)
			{
				CM_DI_Print(0, ("[%s][%d] eSTB_SN [%s] :: eCM_SN [%s]\n", __FUNCTION__, __LINE__, ucSerialNo, ucSerialNoByCm));
				CM_DI_Print(0, ("[%s][%d] Serial No. of eCM is not same with one of eSTB, Have to re-write it !! \n", __FUNCTION__, __LINE__));
				
				nRet = DRV_CM_SetSerialNumber(&ucSerialNo[0]);
				if (nRet != DRV_OK)
				{
					CM_DI_Print(0, ("[%s][%d] nRet(0x%x)\n", __FUNCTION__, __LINE__, nRet));
				}
			}
		}
		else
		{
			CM_DI_Print(0, ("[%s][%d] ucSerialNo is NULL!!\n", __FUNCTION__, __LINE__, nRet));
		}
	}
	
	/* Check whether CM MAC address is ok on CM flash */
	nRet = DRV_CM_GetEcmMac(aucCmMacByCm);
	if (nRet != DRV_OK)
	{
		CM_DI_Print(0, ("[%s][%d] nRet(0x%x)\n", __FUNCTION__, __LINE__, nRet));
	}
	else
	{
		for (ulIdx = 0; ulIdx < NUM_OF_CM_DEFAULT_MAC; ulIdx++)
		{
			if (((aucCmMacByCm[0] == s_aucCmDefaultMac[ulIdx][0]) && (aucCmMacByCm[1] == s_aucCmDefaultMac[ulIdx][1]) 
				&& (aucCmMacByCm[2] == s_aucCmDefaultMac[ulIdx][2]) && (aucCmMacByCm[3] == s_aucCmDefaultMac[ulIdx][3]) 
				&& (aucCmMacByCm[4] == s_aucCmDefaultMac[ulIdx][4]) && (aucCmMacByCm[5] == s_aucCmDefaultMac[ulIdx][5])))
			{
				CM_DI_Print(0, ("[%s][%d] CM MAC returned to default (%02x:%02x:%02x:%02x:%02x:%02x) !! \n", 
					__FUNCTION__, __LINE__, aucCmMacByCm[0], aucCmMacByCm[1], aucCmMacByCm[2], aucCmMacByCm[3], aucCmMacByCm[4], aucCmMacByCm[5]));

				nRet = DRV_NVRAM_GetField(DI_NVRAM_FIELD_CM_MAC_ADDR, 0, &aucCmMac[0], 6);
				CM_DI_Print(0, ("[%s][%d] Have to re-write it to (%02x:%02x:%02x:%02x:%02x:%02x) !!\n", 
					__FUNCTION__, __LINE__, aucCmMac[0], aucCmMac[1], aucCmMac[2], aucCmMac[3], aucCmMac[4], aucCmMac[5]));
				
				if (nRet != DRV_OK)
				{
					CM_DI_Print(0, ("[%s][%d] nRet(0x%x)\n", __FUNCTION__, __LINE__, nRet));
				}
				else
				{
					if ((aucCmMacByCm[0]+aucCmMacByCm[1]+aucCmMacByCm[2]+aucCmMacByCm[3]+aucCmMacByCm[4]+aucCmMacByCm[5] != 0))
					{
						nRet = DRV_CM_SetEcmMac(aucCmMac);
						if (nRet != DRV_OK)
						{
							CM_DI_Print(0, ("[%s][%d] nRet(0x%x)\n", __FUNCTION__, __LINE__, nRet));
						}
					}
					else
					{
						CM_DI_Print(0, ("[%s][%d] CM MAC in STB NVRAM is 00:00:00:00:00 !!\n", __FUNCTION__, __LINE__, nRet));
					}
				}
				break;
			}
		}
	}

	return DRV_OK;
}

DI_ERR_CODE DI_CM_Init(void)
{
#if defined(CONFIG_OS_UCOS)
	DI_CM_HwGpioReset();

	return DI_ERR_OK;
#else
	DRV_Error nRet = DRV_ERR;

	nRet = DRV_CM_StartDsgCC();
	if (nRet != DRV_OK)
	{
		return DI_ERR_ERROR;
	}

	return DI_ERR_OK;
#endif
}

DI_ERR_CODE DI_CM_GetCmIp(HUINT32 *pulCmIp)
{
	DRV_Error nRet = DRV_ERR;

	if (pulCmIp == NULL)
	{
		CM_DI_Print(0,("[%s][%d] Error! pulCmIp is NULL \n", __FUNCTION__, __LINE__));
		return DI_ERR_ERROR;
	}

	nRet = DRV_CM_GetEcmIp(pulCmIp);
	if (nRet != DRV_OK)
	{
		return DI_ERR_ERROR;
	}

	return DI_ERR_OK;
}


DI_ERR_CODE DI_CM_GetCmMac(HUINT8 *pucCmMac)
{
	DRV_Error nRet = DRV_ERR;

	if (pucCmMac == NULL)
	{
		CM_DI_Print(0,("[%s][%d] Error! pucCmMac is NULL \n", __FUNCTION__, __LINE__));
		return DI_ERR_ERROR;
	}

	nRet = DRV_CM_GetEcmMac(pucCmMac);
	if (nRet != DRV_OK)
	{
		return DI_ERR_ERROR;
	}

	return DI_ERR_OK;
}


DI_ERR_CODE DI_CM_SetCmMac(HUINT8 *pucCmMac)
{
	DRV_Error nRet = DRV_ERR;

	if (pucCmMac == NULL)
	{
		CM_DI_Print(0,("[%s][%d] Error! pucCmMac is NULL \n", __FUNCTION__, __LINE__));
		return DI_ERR_ERROR;
	}

	nRet = DRV_CM_SetEcmMac(pucCmMac);
	if (nRet != DRV_OK)
	{
		return DI_ERR_ERROR;
	}

	return DI_ERR_OK;
}


DI_ERR_CODE DI_CM_GetNetmask(HUINT32 *pulNetmask)
{
	DRV_Error nRet = DRV_ERR;

	if (pulNetmask == NULL)
	{
		CM_DI_Print(0,("[%s][%d] Error! pulNetmask is NULL \n", __FUNCTION__, __LINE__));
		return DI_ERR_ERROR;
	}

	nRet = DRV_CM_GetNetmask(pulNetmask);
	if (nRet != DRV_OK)
	{
		return DI_ERR_ERROR;
	}

	return DI_ERR_OK;
}


DI_ERR_CODE DI_CM_GetGateway(HUINT32 *pulGateway)
{
	DRV_Error nRet = DRV_ERR;

	if (pulGateway == NULL)
	{
		CM_DI_Print(0,("[%s][%d] Error! pulGateway is NULL \n", __FUNCTION__, __LINE__));
		return DI_ERR_ERROR;
	}

	nRet = DRV_CM_GetGateway(pulGateway);
	if (nRet != DRV_OK)
	{
		return DI_ERR_ERROR;
	}

	return DI_ERR_OK;
}


DI_ERR_CODE DI_CM_GetCmVer(HCHAR *pcCmVer)
{
	DRV_Error nRet = DRV_ERR;

	if (pcCmVer == NULL)
	{
		CM_DI_Print(0,("[%s][%d] Error! pcCmVer is NULL \n", __FUNCTION__, __LINE__));
		return DI_ERR_ERROR;
	}

	nRet = DRV_CM_GetSwVer((HUINT8 *)pcCmVer);
	if (nRet != DRV_OK)
	{
		return DI_ERR_ERROR;
	}

	return DI_ERR_OK;
}

/*
 *	Cm Online Status
             other(1),
            notReady(2),
            notSynchronized(3),
            phySynchronized(4),
            usParametersAcquired(5),
            rangingComplete(6),
            ipComplete(7),
            todEstablished(8),
            securityEstablished(9),
            paramTransferComplete(10),
            registrationComplete(11),
            operational(12),
            accessDenied(13)
 */
HINT32 DI_CM_CmStatus(void)
{
#if !defined(CONFIG_PRODUCT_IMAGE_FACTORY)
	CM_DI_Print(6, ("[%s][%d] gulCurrCmStatus = %d\n", __FUNCTION__, __LINE__, gulCurrCmStatus));

	return gulCurrCmStatus;
#else
	DRV_Error nRet = DRV_ERR;
	HINT32 nCmRegStatus = 0;

	nRet = DRV_CM_GetCmRegStatus(&nCmRegStatus);
	if (nRet != DRV_OK)
	{
		return 0;
	}

	return nCmRegStatus;
#endif
}


/* FDC Information */
DI_ERR_CODE DI_CM_GetFDCFreq(HINT32 *pnFdcFreq)
{
	DRV_Error nRet = DRV_ERR;

	if (pnFdcFreq == NULL)
	{
		CM_DI_Print(0,("[%s][%d] Error! pnFdcFreq is NULL \n", __FUNCTION__, __LINE__));
		return DI_ERR_ERROR;
	}

	nRet = DRV_CM_GetDsFreq(0,pnFdcFreq);
	if (nRet != DRV_OK)
	{
		return DI_ERR_ERROR;
	}

	return DI_ERR_OK;
}


DI_ERR_CODE DI_CM_GetFDCPower(HINT32 *pnFdcPwr)
{
	DRV_Error nRet = DRV_ERR;

	if (pnFdcPwr == NULL)
	{
		CM_DI_Print(0,("[%s][%d] Error! pnFdcPwr is NULL \n", __FUNCTION__, __LINE__));
		return DI_ERR_ERROR;
	}

	nRet = DRV_CM_GetDsPower(0,pnFdcPwr);
	if (nRet != DRV_OK)
	{
		return DI_ERR_ERROR;
	}

	return DI_ERR_OK;
}


DI_ERR_CODE DI_CM_GetFDCSnr(HINT32 *pnFdcSnr)
{
	DRV_Error nRet = DRV_ERR;

	if (pnFdcSnr == NULL)
	{
		CM_DI_Print(0,("[%s][%d] Error! pnFdcSnr is NULL \n", __FUNCTION__, __LINE__));
		return DI_ERR_ERROR;
	}

	nRet = DRV_CM_GetDsSnr(0,pnFdcSnr);
	if (nRet != DRV_OK)
	{
		return DI_ERR_ERROR;
	}

	return DI_ERR_OK;
}


DI_ERR_CODE DI_CM_GetFDCMod(HINT32 *pnFdcMod)
{
	DRV_Error nRet = DRV_ERR;

	if (pnFdcMod == NULL)
	{
		CM_DI_Print(0,("[%s][%d] Error! pnFdcMod is NULL \n", __FUNCTION__, __LINE__));
		return DI_ERR_ERROR;
	}

	nRet = DRV_CM_GetDsModulation(0,pnFdcMod);
	if (nRet != DRV_OK)
	{
		return DI_ERR_ERROR;
	}

	return DI_ERR_OK;
}


DI_ERR_CODE DI_CM_GetRDCFreq(HINT32 *pnRdcFreq)
{
	DRV_Error nRet = DRV_ERR;

	if (pnRdcFreq == NULL)
	{
		CM_DI_Print(0,("[%s][%d] Error! pnRdcFreq is NULL \n", __FUNCTION__, __LINE__));
		return DI_ERR_ERROR;
	}

	nRet = DRV_CM_GetUsFreq(0,pnRdcFreq);
	if (nRet != DRV_OK)
	{
		return DI_ERR_ERROR;
	}

	return DI_ERR_OK;
}


DI_ERR_CODE DI_CM_GetRDCPower(HUINT32 *pulRdcPwr)
{
	DRV_Error nRet = DRV_ERR;

	if (pulRdcPwr == NULL)
	{
		CM_DI_Print(0,("[%s][%d] Error! pulRdcPwr is NULL \n", __FUNCTION__, __LINE__));
		return DI_ERR_ERROR;
	}

	nRet = DRV_CM_GetUsPower(0,pulRdcPwr);
	if (nRet != DRV_OK)
	{
		return DI_ERR_ERROR;
	}

	return DI_ERR_OK;
}


DI_ERR_CODE DI_CM_GetRDCChannelId(HINT32 *pnRdcChId)
{
	DRV_Error nRet = DRV_ERR;

	if (pnRdcChId == NULL)
	{
		CM_DI_Print(0,("[%s][%d] Error! pnRdcChId is NULL \n", __FUNCTION__, __LINE__));
		return DI_ERR_ERROR;
	}

	nRet = DRV_CM_GetUsChannelId(0,pnRdcChId);
	if (nRet != DRV_OK)
	{
		return DI_ERR_ERROR;
	}

	return DI_ERR_OK;
}


DI_ERR_CODE DI_CM_GetRDCSnr(HUINT32 *ulRDCSNR)
{
	if (ulRDCSNR == NULL)
	{
		CM_DI_Print(0,("[%s][%d] Error! ulRDCSNR is NULL \n", __FUNCTION__, __LINE__));
		return DI_ERR_ERROR;
	}

	*ulRDCSNR = 0;

	return DI_ERR_OK;
}


/* FDC Information for DOCSIS30 */
DI_ERR_CODE DI_CM_GetFDCFreq30(HINT32 nFdcId, HINT32 *pnFdcFreq)
{
	DRV_Error nRet = DRV_ERR;

	if ((nFdcId < 0) && (nFdcId > NUM_OF_DS_CHANNEL))
	{
		CM_DI_Print(0,("[%s][%d] Error! Invalid downstream channel Id (%d) \n", __FUNCTION__, __LINE__, nFdcId));
		return DI_ERR_ERROR;
	}

	if (pnFdcFreq == NULL)
	{
		CM_DI_Print(0,("[%s][%d] Error! pnFdcFreq is NULL \n", __FUNCTION__, __LINE__));
		return DI_ERR_ERROR;
	}

	nRet = DRV_CM_GetDsFreq(nFdcId,pnFdcFreq);
	if (nRet != DRV_OK)
	{
		return DI_ERR_ERROR;
	}

	return DI_ERR_OK;
}


DI_ERR_CODE DI_CM_GetFDCPower30(HINT32 nFdcId, HINT32 *pnFdcPwr)
{
	DRV_Error nRet = DRV_ERR;

	if ((nFdcId < 0) && (nFdcId > NUM_OF_DS_CHANNEL))
	{
		CM_DI_Print(0,("[%s][%d] Error! Invalid downstream channel Id (%d) \n", __FUNCTION__, __LINE__, nFdcId));
		return DI_ERR_ERROR;
	}

	if (pnFdcPwr == NULL)
	{
		CM_DI_Print(0,("[%s][%d] Error! pnFdcPwr is NULL \n", __FUNCTION__, __LINE__));
		return DI_ERR_ERROR;
	}

	nRet = DRV_CM_GetDsPower(nFdcId,pnFdcPwr);
	if (nRet != DRV_OK)
	{
		return DI_ERR_ERROR;
	}

	return DI_ERR_OK;
}


DI_ERR_CODE DI_CM_GetFDCSnr30(HINT32 nFdcId, HINT32 *pnFdcSnr)
{
	DRV_Error nRet = DRV_ERR;

	if ((nFdcId < 0) && (nFdcId > NUM_OF_DS_CHANNEL))
	{
		CM_DI_Print(0,("[%s][%d] Error! Invalid downstream channel Id (%d) \n", __FUNCTION__, __LINE__, nFdcId));
		return DI_ERR_ERROR;
	}

	if (pnFdcSnr == NULL)
	{
		CM_DI_Print(0,("[%s][%d] Error! pnFdcSnr is NULL \n", __FUNCTION__, __LINE__));
		return DI_ERR_ERROR;
	}

	nRet = DRV_CM_GetDsSnr(nFdcId,pnFdcSnr);
	if (nRet != DRV_OK)
	{
		return DI_ERR_ERROR;
	}

	return DI_ERR_OK;
}


DI_ERR_CODE DI_CM_GetFDCBer30(HINT32 nFdcId, HUINT32 *pnFdcBer)
{
	if ((nFdcId < 0) && (nFdcId > NUM_OF_DS_CHANNEL))
	{
		CM_DI_Print(0,("[%s][%d] Error! Invalid downstream channel Id (%d) \n", __FUNCTION__, __LINE__, nFdcId));
		return DI_ERR_ERROR;
	}

	if (pnFdcBer == NULL)
	{
		CM_DI_Print(0,("[%s][%d] Error! pnFdcBer is NULL \n", __FUNCTION__, __LINE__));
		return DI_ERR_ERROR;
	}

	*pnFdcBer = 0;

	CM_DI_Print(5, ("Downstream BER (%d) is %d\n", nFdcId, *pnFdcBer));

	return DI_ERR_OK;
}


DI_ERR_CODE DI_CM_GetFDCChannelId30(HINT32 nFdcId, HINT32 *pnFdcChId)
{
	DRV_Error nRet = DRV_ERR;

	if ((nFdcId < 0) && (nFdcId > NUM_OF_DS_CHANNEL))
	{
		CM_DI_Print(0,("[%s][%d] Error! Invalid downstream channel Id (%d) \n", __FUNCTION__, __LINE__, nFdcId));
		return DI_ERR_ERROR;
	}

	if (pnFdcChId == NULL)
	{
		CM_DI_Print(0,("[%s][%d] Error! pnFdcChId is NULL \n", __FUNCTION__, __LINE__));
		return DI_ERR_ERROR;
	}

	nRet = DRV_CM_GetDsChannelId(nFdcId,pnFdcChId);
	if (nRet != DRV_OK)
	{
		return DI_ERR_ERROR;
	}

	return DI_ERR_OK;
}


DI_ERR_CODE DI_CM_GetFDCMod30(HINT32 nFdcId, HINT32 *pnFdcMod)
{
	DRV_Error nRet = DRV_ERR;

	if ((nFdcId < 0) && (nFdcId > NUM_OF_DS_CHANNEL))
	{
		CM_DI_Print(0,("[%s][%d] Error! Invalid downstream channel Id (%d) \n", __FUNCTION__, __LINE__, nFdcId));
		return DI_ERR_ERROR;
	}

	if (pnFdcMod == NULL)
	{
		CM_DI_Print(0,("[%s][%d] Error! pnFdcMod is NULL \n", __FUNCTION__, __LINE__));
		return DI_ERR_ERROR;
	}

	nRet = DRV_CM_GetDsModulation(nFdcId,pnFdcMod);
	if (nRet != DRV_OK)
	{
		return DI_ERR_ERROR;
	}

	return DI_ERR_OK;
}


DI_ERR_CODE DI_CM_GetFDCCorrected30(HINT32 nFdcId, HUINT32 *pulFdcCorr)
{
	DRV_Error nRet = DRV_ERR;

	if ((nFdcId < 0) && (nFdcId > NUM_OF_DS_CHANNEL))
	{
		CM_DI_Print(0,("[%s][%d] Error! Invalid downstream channel Id (%d) \n", __FUNCTION__, __LINE__, nFdcId));
		return DI_ERR_ERROR;
	}

	if (pulFdcCorr == NULL)
	{
		CM_DI_Print(0,("[%s][%d] Error! pulFdcCorr is NULL \n", __FUNCTION__, __LINE__));
		return DI_ERR_ERROR;
	}

	nRet = DRV_CM_GetDsCorrected(nFdcId,pulFdcCorr);
	if (nRet != DRV_OK)
	{
		return DI_ERR_ERROR;
	}

	return DI_ERR_OK;
}


DI_ERR_CODE DI_CM_GetFDCUncorrected30(HINT32 nFdcId, HUINT32 *pulFdcUncorr)
{
	DRV_Error nRet = DRV_ERR;

	if ((nFdcId < 0) && (nFdcId > NUM_OF_DS_CHANNEL))
	{
		CM_DI_Print(0,("[%s][%d] Error! Invalid downstream channel Id (%d) \n", __FUNCTION__, __LINE__, nFdcId));
		return DI_ERR_ERROR;
	}

	if (pulFdcUncorr == NULL)
	{
		CM_DI_Print(0,("[%s][%d] Error! pulFdcUncorr is NULL \n", __FUNCTION__, __LINE__));
		return DI_ERR_ERROR;
	}

	nRet = DRV_CM_GetDsUncorrected(nFdcId,pulFdcUncorr);
	if (nRet != DRV_OK)
	{
		return DI_ERR_ERROR;
	}

	return DI_ERR_OK;
}


DI_ERR_CODE DI_CM_GetDsSignalInfo (HUINT32 ulFdcId, DI_CM_DsSignalQuality_t *pstDsSignalInfo)
{
	DRV_Error nRet = DRV_ERR;

	if (ulFdcId > NUM_OF_DS_CHANNEL)
	{
		CM_DI_Print(0,("[%s][%d] Error! Invalid downstream channel Id (%d) \n", __FUNCTION__, __LINE__, ulFdcId));
		return DI_ERR_ERROR;
	}

	if (pstDsSignalInfo == NULL)
	{
		CM_DI_Print(0,("[%s][%d] Error! pstDsSignalInfo is NULL \n", __FUNCTION__, __LINE__));
		return DI_ERR_ERROR;
	}

	VK_MEM_Memset(pstDsSignalInfo, 0, sizeof(DI_CM_DsSignalQuality_t));

	nRet = DRV_CM_GetDsSignalInfo(ulFdcId,pstDsSignalInfo);
	if (nRet != DRV_OK)
	{
		return DI_ERR_ERROR;
	}

	return DI_ERR_OK;
}


/* RDC Information */
DI_ERR_CODE DI_CM_GetRDCFreq30(HINT32 nRdcId, HINT32 *pnRdcFreq)
{
	DRV_Error nRet = DRV_ERR;

	if ((nRdcId < 0) && (nRdcId > NUM_OF_US_CHANNEL))
	{
		CM_DI_Print(0,("[%s][%d] Error! Invalid upstream channel Id (%d) \n", __FUNCTION__, __LINE__, nRdcId));
		return DI_ERR_ERROR;
	}

	if (pnRdcFreq == NULL)
	{
		CM_DI_Print(0,("%s - Error! pnRdcFreq is NULL \n", __FUNCTION__));
		return DI_ERR_ERROR;
	}

	nRet = DRV_CM_GetUsFreq(nRdcId,pnRdcFreq);
	if (nRet != DRV_OK)
	{
		return DI_ERR_ERROR;
	}

	return DI_ERR_OK;
}


DI_ERR_CODE DI_CM_GetRDCPower30(HINT32 nRdcId, HUINT32 *pulRdcPower)
{
	DRV_Error nRet = DRV_ERR;

	if ((nRdcId < 0) && (nRdcId > NUM_OF_US_CHANNEL))
	{
		CM_DI_Print(0,("[%s][%d] Error! Invalid upstream channel Id (%d) \n", __FUNCTION__, __LINE__, nRdcId));
		return DI_ERR_ERROR;
	}

	if (pulRdcPower == NULL)
	{
		CM_DI_Print(0,("[%s][%d] Error! pulRdcPower is NULL \n", __FUNCTION__, __LINE__));
		return DI_ERR_ERROR;
	}

	nRet = DRV_CM_GetUsPower(nRdcId,pulRdcPower);
	if (nRet != DRV_OK)
	{
		return DI_ERR_ERROR;
	}

	return DI_ERR_OK;
}


DI_ERR_CODE DI_CM_GetRDCRange30(HINT32 nRdcId, HINT32 *pnRdcRange)
{
	DRV_Error nRet = DRV_ERR;

	if ((nRdcId < 0) && (nRdcId > NUM_OF_US_CHANNEL))
	{
		CM_DI_Print(0,("[%s][%d] Error! Invalid upstream channel Id (%d) \n", __FUNCTION__, __LINE__, nRdcId));
		return DI_ERR_ERROR;
	}

	if (pnRdcRange == NULL)
	{
		CM_DI_Print(0,("[%s][%d] Error! pnRdcRange is NULL \n", __FUNCTION__, __LINE__));
		return DI_ERR_ERROR;
	}

	nRet = DRV_CM_GetUsRangingStatus(nRdcId,pnRdcRange);
	if (nRet != DRV_OK)
	{
		return DI_ERR_ERROR;
	}

	return DI_ERR_OK;
}


DI_ERR_CODE DI_CM_GetRDCChannelId30(HINT32 nRdcId, HINT32 *pnRdcChId)
{
	DRV_Error nRet = DRV_ERR;

	if ((nRdcId < 0) && (nRdcId > NUM_OF_US_CHANNEL))
	{
		CM_DI_Print(0,("[%s][%d] Error! Invalid upstream channel Id (%d) \n", __FUNCTION__, __LINE__, nRdcId));
		return DI_ERR_ERROR;
	}

	if (pnRdcChId == NULL)
	{
		CM_DI_Print(0,("[%s][%d] Error! pnRdcChId is NULL \n", __FUNCTION__, __LINE__));
		return DI_ERR_ERROR;
	}

	nRet = DRV_CM_GetUsChannelId(nRdcId,pnRdcChId);
	if (nRet != DRV_OK)
	{
		return DI_ERR_ERROR;
	}

	return DI_ERR_OK;
}


DI_ERR_CODE DI_CM_GetRDCMod30(HINT32 nRdcId, HINT32 *pnRdcMod)
{
	DRV_Error nRet = DRV_ERR;

	if ((nRdcId < 0) && (nRdcId > NUM_OF_US_CHANNEL))
	{
		CM_DI_Print(0,("[%s][%d] Error! Invalid upstream channel Id (%d) \n", __FUNCTION__, __LINE__, nRdcId));
		return DI_ERR_ERROR;
	}

	if (pnRdcMod == NULL)
	{
		CM_DI_Print(0,("[%s][%d] Error! pnRdcMod is NULL \n", __FUNCTION__, __LINE__));
		return DI_ERR_ERROR;
	}

	nRet = DRV_CM_GetUsRangingStatus(nRdcId,pnRdcMod);
	if (nRet != DRV_OK)
	{
		return DI_ERR_ERROR;
	}

	return DI_ERR_OK;
}


DI_ERR_CODE DI_CM_GetRDCBandWidth30(HINT32 nRdcId, HINT32 *pnRdcBw)
{
	DRV_Error nRet = DRV_ERR;

	if ((nRdcId < 0) && (nRdcId > NUM_OF_US_CHANNEL))
	{
		CM_DI_Print(0,("[%s][%d] Error! Invalid upstream channel Id (%d) \n", __FUNCTION__, __LINE__, nRdcId));
		return DI_ERR_ERROR;
	}


	if (pnRdcBw == NULL)
	{
		CM_DI_Print(0,("[%s][%d] Error! pnRdcBw is NULL \n", __FUNCTION__, __LINE__));
		return DI_ERR_ERROR;
	}

	nRet = DRV_CM_GetUsBandwidth(nRdcId,pnRdcBw);
	if (nRet != DRV_OK)
	{
		return DI_ERR_ERROR;
	}

	return DI_ERR_OK;
}


DI_ERR_CODE DI_CM_GetRDCSymbolRate30(HINT32 nRdcId, HUINT32 *pulRdcSymbolRate)
{
	DRV_Error nRet = DRV_ERR;

	if ((nRdcId < 0) && (nRdcId > NUM_OF_US_CHANNEL))
	{
		CM_DI_Print(0,("[%s][%d] Error! Invalid upstream channel Id (%d) \n", __FUNCTION__, __LINE__, nRdcId));
		return DI_ERR_ERROR;
	}

	if (pulRdcSymbolRate == NULL)
	{
		CM_DI_Print(0,("[%s][%d] Error! pulRdcSymbolRate is NULL \n", __FUNCTION__, __LINE__));
		return DI_ERR_ERROR;
	}

	nRet = DRV_CM_GetUsSymbolrate(nRdcId,pulRdcSymbolRate);
	if (nRet != DRV_OK)
	{
		return DI_ERR_ERROR;
	}

	return DI_ERR_OK;
}


DI_ERR_CODE DI_CM_GetUsSignalInfo (HUINT32 ulRdcId, DI_CM_UsSignalQuality_t *pstUsSignalInfo)
{
	DRV_Error nRet = DRV_ERR;

	if (ulRdcId > NUM_OF_US_CHANNEL)
	{
		CM_DI_Print(0,("[%s][%d] Error! Invalid upstream channel Id (%d) \n", __FUNCTION__, __LINE__, ulRdcId));
		return DI_ERR_ERROR;
	}

	if (pstUsSignalInfo == NULL)
	{
		CM_DI_Print(0,("[%s][%d] Error! pstUsSignalInfo is NULL \n", __FUNCTION__, __LINE__));
		return DI_ERR_ERROR;
	}

	VK_MEM_Memset(pstUsSignalInfo, 0, sizeof(DI_CM_UsSignalQuality_t));

	nRet = DRV_CM_GetUsSignalInfo(ulRdcId,pstUsSignalInfo);
	if (nRet != DRV_OK)
	{
		return DI_ERR_ERROR;
	}

	return DI_ERR_OK;
}


DI_ERR_CODE DI_CM_GetScanDSFreq( char *pszValue)
{
	DRV_Error nRet = DRV_ERR;

	if (pszValue == NULL)
	{
		CM_DI_Print(0,("[%s][%d] Error! pszValue is NULL \n", __FUNCTION__, __LINE__));
		return DI_ERR_ERROR;
	}

	nRet = DRV_CM_GetScanDsFreq(pszValue);
	if (nRet != DRV_OK)
	{
		return DI_ERR_ERROR;
	}

	return DI_ERR_OK;
}


DI_ERR_CODE DI_CM_SetScanDSFreq( HUINT32 *ulDSFreq)
{
	DRV_Error nRet = DRV_ERR;

	if (ulDSFreq == NULL)
	{
		CM_DI_Print(0,("[%s][%d] Error! ulDSFreq is NULL \n", __FUNCTION__, __LINE__));
		return DI_ERR_ERROR;
	}

	nRet = DRV_CM_SetScanDsFreq(ulDSFreq);
	if (nRet != DRV_OK)
	{
		return DI_ERR_ERROR;
	}

	return DI_ERR_OK;
}


DI_ERR_CODE DI_CM_GetCmSavedTemp(int *pCmSavedTemp, int *pCmCurrentTemp)
{
	DRV_Error nRet = DRV_ERR;

	if ((pCmSavedTemp == NULL) || (pCmCurrentTemp == NULL))
	{
		CM_DI_Print(0,("[%s][%d] Error! pCmSavedTemp or pCmCurrentTemp is NULL \n", __FUNCTION__, __LINE__));
		return DI_ERR_ERROR;
	}

	nRet = DRV_CM_GetTemperature(pCmSavedTemp, pCmCurrentTemp);
	if (nRet != DRV_OK)
	{
		return DI_ERR_ERROR;
	}

	return DI_ERR_OK;
}


DI_ERR_CODE DI_CM_SetTemp(int nTempMode)
{
	DRV_Error nRet = DRV_ERR;

	nRet = DRV_CM_SetTemperature(nTempMode);
	if (nRet != DRV_OK)
	{
		return DI_ERR_ERROR;
	}

	return DI_ERR_OK;
}


DI_ERR_CODE DI_CM_SetStandbyMode(HINT32 nStandbyCmLed)
{
#if defined(CONFIG_CABLE_MODEM_LED_STATUS)
	DRV_Error unBlinkRet = DRV_OK;

	CM_DI_Print(1, ("[%s][%d] nStandbyCmLed = %d\n", __FUNCTION__, __LINE__, nStandbyCmLed));
	if (nStandbyCmLed == DI_CM_LED_TURN_OFF)
	{
		unBlinkRet = DRV_MICOM_SetBlinkingTime(g_ulCmLedId,0);
	}
	else
	{
		unBlinkRet = DRV_MICOM_SetBlinkingTime(g_ulCmLedId,0xFF);
	}

	if(unBlinkRet != DRV_OK)
	{
		CM_DI_Print(0, ("[%s][%d] Err! %d\n", __FUNCTION__, __LINE__, unBlinkRet));
		return DI_ERR_ERROR;
	}

	if (nStandbyCmLed == DI_CM_LED_TURN_ON)
	{
		gulPrvCmStatus = 0xFFFF;
	}

	return DI_ERR_OK;
#else
	DRV_Error nRet = DRV_ERR;

	nRet = DRV_CM_SetStandbyMode(nStandbyCmLed);
	if (nRet != DRV_OK)
	{
		return DI_ERR_ERROR;
	}

	return DI_ERR_OK;
#endif
}


DI_ERR_CODE DI_CM_SetFan(HINT32 nFanMode)
{
	DRV_Error nRet = DRV_ERR;

	nRet = DRV_CM_SetFanMode(nFanMode);
	if (nRet != DRV_OK)
	{
		return DI_ERR_ERROR;
	}

	return DI_ERR_OK;
}


DI_ERR_CODE DI_CM_SetCmDefaults(void)
{
	DRV_Error nRet = DRV_ERR;

	nRet = DRV_CM_SetDefault();
	if (nRet != DRV_OK)
	{
		return DI_ERR_ERROR;
	}

	return DI_ERR_OK;
}


DI_ERR_CODE DI_CM_GetCmSerial(HCHAR *pszCmSerialNo)
{
	DRV_Error nRet = DRV_ERR;

	nRet = DRV_CM_GetSerialNumber(pszCmSerialNo);
	if (nRet != DRV_OK)
	{
		return DI_ERR_ERROR;
	}

	return DI_ERR_OK;
}


DI_ERR_CODE DI_CM_SetCmSerial(HCHAR *pszCmSerialNo)
{
	DRV_Error nRet = DRV_ERR;

	nRet = DRV_CM_SetSerialNumber(pszCmSerialNo);
	if (nRet != DRV_OK)
	{
		return DI_ERR_ERROR;
	}

	return DI_ERR_OK;
}


DI_ERR_CODE DI_CM_SwUpgrade(void)
{
	DRV_Error nRet = DRV_ERR;

	nRet = DRV_CM_SwUpgrade();
	if (nRet != DRV_OK)
	{
		return DI_ERR_ERROR;
	}

	return DI_ERR_OK;
}


DI_ERR_CODE DI_CM_GetMpegIp(HCHAR *pcMpegIp)
{
	DRV_Error nRet = DRV_ERR;

	nRet = DRV_CM_GetEstbIp(pcMpegIp);
	if (nRet != DRV_OK)
	{
		return DI_ERR_ERROR;
	}

	return DI_ERR_OK;
}


DI_ERR_CODE DI_CM_RegisterCallback( pfnDI_CM_NotifyLinkInfo pfnNotify)
{
	int nResult = DI_CH_OK;

	CM_DI_Print(0, ("[%s][%d] ---- \n ", __FUNCTION__, __LINE__));


	if ( (sCmLinkInfo.CmLinkCallback == NULL) || (pfnNotify == NULL) )
	{
		CM_DI_Print(5, ("[%s][%d] Callback is registered...\n", __FUNCTION__, __LINE__));
		sCmLinkInfo.CmLinkCallback = pfnNotify;
	}
	else
	{
		/* callback is already regitered */
	}

	return nResult;
}


void DI_CM_HwGpioReset(void)
{
#if defined(CONFIG_CABLE_MODEM_RESET_BY_HOST)
	DRV_GPIO_Write(GPIO_ID_CM_RST_N, 1);

	VK_TASK_Sleep(10);

	DRV_GPIO_Write(GPIO_ID_CM_RST_N, 0);

	VK_TASK_Sleep(10);

	DRV_GPIO_Write(GPIO_ID_CM_RST_N, 1);

	CM_DI_Print(0, ("[%s] [&d] reset !!\n", __FUNCTION__, __LINE__));
#else
	CM_DI_Print(0, ("[%s] [&d] not support !!\n", __FUNCTION__, __LINE__));
#endif

	return ;
}


#if defined(CONFIG_EURO_DOCSIS)
static void StartEchoTestTask(void* param)
{
	param = param;
	EchoTest();
	return;
}

void DI_CM_eCMEchoTest(void)
{
	int nRet 	= DRV_OK;
	HULONG		ulEchoInitTaskID = 0;

	if(s_ulEchoTestStart == TRUE)
	{
		CM_DI_Print(0, ("[%s][%d] Alread started Echo Test\n", __FUNCTION__, __LINE__));
		return;
	}
	else
	{
		nRet = VK_TASK_Create(	StartEchoTestTask,
								DSG_TASK_PRIORITY,
								DSG_TASK_STACK_SIZE,
								"ECHO",
								NULL,
								&ulEchoInitTaskID,
								0 );

		if ((ulEchoInitTaskID == 0) || (nRet != VK_OK))
		{
			CM_DI_Print(0, ("[%s][%d] nRet(0x%x)", __FUNCTION__, __LINE__, nRet));
			return;
		}
		else
		{
			VK_TASK_Start(ulEchoInitTaskID);
		}
		s_ulEchoTestStart = TRUE;
	}
}
#endif

#endif


/** @} */ // end of DI_CM


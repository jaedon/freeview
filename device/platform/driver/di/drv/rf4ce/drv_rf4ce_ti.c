/*******************************************************************************
* File name : drv_rf4ce_ti.c
* @defgroup		rf4ce client: rf4ce client module
* @author		Jaekwang Lee(B2C SW4)
* @note
* @brief
* @file 			drv_rf4ce_ti.c
*
* Copyright (c)  by Humax Co., Ltd. Since 2007.
* All right reserved
*******************************************************************************/

/*******************************************************************************
* Header and definitions
*******************************************************************************/

/* global header files */
#if defined(CONFIG_VK_STDLIB)
#else
#include 	<stdio.h>
#include 	<string.h>
#endif
#include <sys/stat.h>
#include <fcntl.h>

/* chip manufacture's  header files */
#include "drv_rf4ce_ti.h"
#include "drv_rf4ce.h"

/* humax header files */
#include "bstd_defs.h"

#include "drv_i2c.h"
#include "drv_gpio.h"
#include "vkernel.h"
#include "taskdef.h"

#include "di_uart.h"
#include "di_key.h"
#include "di_rf4ce.h"
#include "di_rf4ce_priv.h"


/*******************************************************************************
* Debug level
*******************************************************************************/
//#define RF4CE_DEBUG
//#define RF4CE_DEBUG_ENTER_EXIT

#ifdef RF4CE_DEBUG
#define PrintInfo	VK_printf
#define PrintDebug	VK_printf
#define PrintError	VK_printf
#else
#define PrintData	while (0) ((int (*)(char *, ...)) 0)
#define PrintDebug	while (0) ((int (*)(char *, ...)) 0)
#define PrintError	DI_UART_Print
#define PrintInfo	DI_UART_Print
#endif

#ifdef RF4CE_DEBUG_ENTER_EXIT
#define PrintEnter	VK_printf("(+)%s\n", __FUNCTION__)
#define PrintExit		VK_printf("(-)%s\n", __FUNCTION__)
#else
#define PrintEnter
#define PrintExit
#endif



/*******************************************************************************
* Definitions, typdef & const data
*******************************************************************************/
#define RF4CE_I2C_SLAVE_ADDR		0x41

#define RF4CE_MESSAGE_QUEUE_SIZE	50
#define RF4CE_MESSAGE_BYTES			16

#define CLR	1
#define SET	0

#define RESET_INTERVAL	1500

#define MAX_RETRY_COUNT		3

#define LONG_INTERVAL	200
#define SHORT_INTERVAL	10

/*******************************************************************************
* Global variables and structures
*******************************************************************************/


/*******************************************************************************
* External variables and functions
*******************************************************************************/
extern RF4CE_INSTANCE_t g_tRF4CEInstance;

/*******************************************************************************
* Static variables and structures
*******************************************************************************/
static unsigned int s_ulIntrFlag = CLR;

static HULONG s_ulRf4ceReceiveTaskId = 0;
static HULONG s_ulRf4cePollingSrdyTaskId = 0;

static unsigned long s_ulRf4ceSemId;
static unsigned long s_ulRf4ceMsgQueueId;

static HBOOL	s_bSrdyPolling = FALSE;
static HBOOL	s_bInitialized = FALSE;

static HUINT8	s_ucDestIdx = 0xFF;
static HUINT8	s_ucDevType = 0xFF;

static RF4CE_TargetInfo_t	s_stTargetInfo;

static HUINT8	s_ucRtisBE;

static HUINT32	s_ulIntrCount = 0;

static HUINT32	s_ulSendSyncDataInterval = SHORT_INTERVAL;

static HUINT8 vendorName[RTI_VENDOR_STRING_LENGTH] = "HUMAX";

static HUINT32 s_Mode = 0;
static HUINT32 s_Channel = 0;
static HUINT32 s_TxPower = 0;

// RF4CE Key Callback
static pfnRF4CE_KeyCallback pfnRf4ceKeyCallback;


// List of implemented device types: maximum up to 3 device types.
static HUINT8 profileList[RTI_MAX_NUM_PROFILE_IDS] =
{
		RF4CE_PROFILE_ZRC, RF4CE_PROFILE_ZID
};



/*******************************************************************************
* Static function prototypes
*******************************************************************************/
static void P_RF4CE_ISR_SlaveReady(void *context, int param);
static void P_RF4CE_ReceiveTask(void *pvParam);
static void P_RF4CE_SrdyPollingTask(void *pvParam);
static void P_RF4CE_EventHandler(RF4CE_Msg_t *pMsg);
static void P_RF4CE_PrintCMD(RF4CE_Msg_t *pMsg);
static void P_RF4CE_PrintCMDHeader(RF4CE_Msg_t *pMsg);
static void P_RF4CE_EndianConversion( HUINT8 attrib, HUINT8 len, HUINT8 *pValue );
static HUINT8 P_RF4CE_CheckEndian( void );

static DRV_Error P_RF4CE_Reset(void);
static DRV_Error P_RF4CE_SetMasterReady (GPIO_STATE_t state);
static DRV_Error P_RF4CE_GetSlaveReady(GPIO_STATE_t* state);
static DRV_Error P_RF4CE_SendAsyncData (RF4CE_Msg_t *pMsg);
static DRV_Error P_RF4CE_SendSyncData (RF4CE_Msg_t *pMsg);


/**************************************************************************************************
 * @fn          P_RF4CE_CheckEndian
 *
 * @brief       This function determines endianness.
 *
 * input parameters
 *
 * None.
 *
 * output parameters
 *
 * None.
 *
 * @return      TRUE if the machine is a big-endian machine. FALSE, otherwise.
 **************************************************************************************************
 */
static HUINT8 P_RF4CE_CheckEndian( void )
{
	static const HUINT16 test = 1;
	HUINT8 *pTest = (HUINT8 *) &test;

	return (*pTest == 0);
}

/**************************************************************************************************
 * @fn          P_RF4CE_EndianConversion
 *
 * @brief       This function converts endianness of an attribute value if the current machine
 *              is a big endian machine.
 *              This function will not do anything if the machine is a little endian machine.
 *
 * input parameters
 *
 * @param       attrib - attribute identifier
 * @param       len    - length of the value buffer
 * @param       pValue - buffer where attribute value is stored
 *
 * output parameters
 *
 * @param       pValue - the buffer is rewritten with swapped endianness value.
 *
 * @return      None.
 **************************************************************************************************
 */
static void P_RF4CE_EndianConversion( HUINT8 attrib, HUINT8 len, HUINT8 *pValue )
{
	if (s_ucRtisBE)
	{
		if (attrib == RF4CE_SA_ITEM_PT_CURRENT_ENTRY)
		{
			if (len >= sizeof(RF4CE_PairingEntry_t))
			{
				// Note that endianness conversion will not occur if the retrieved attribute length is
				// smaller.
				RF4CE_PairingEntry_t *pEntry = (RF4CE_PairingEntry_t *) pValue;

				pEntry->srcNwkAddress = RF4CE_ENDIAN_CONV16(pEntry->srcNwkAddress);
				// Note that IEEE address is not converted. It is always supposed to be little endian
				pEntry->panId = RF4CE_ENDIAN_CONV16(pEntry->panId);
				pEntry->nwkAddress = RF4CE_ENDIAN_CONV16(pEntry->nwkAddress);
				pEntry->recFrameCounter = RF4CE_ENDIAN_CONV32(pEntry->recFrameCounter);
				pEntry->vendorIdentifier = RF4CE_ENDIAN_CONV16(pEntry->vendorIdentifier);
			}
		}
		else
		{
			// all other attributes are natural number
			HUINT8 i, j, buf;

			for (i = 0, j = len - 1; i < j; i++, j--)
			{
				buf = pValue[i];
				pValue[i] = pValue[j];
				pValue[j] = buf;
			}
		}
	}
}


static void P_RF4CE_ReceiveTask(void *pvParam)
{
	DRV_Error	eDrvError = DRV_OK;
	RF4CE_Msg_t Msg;
	GPIO_STATE_t srdy = GPIO_INVAILD;

	HUINT32 ulBuffer;
	HUINT32 ulReturnCode;

	pvParam = pvParam;

	while(1)
	{
		PrintDebug( "[%s] Waiting in VK_MSG_Receive...\n", __FUNCTION__);
		ulReturnCode = VK_MSG_Receive(s_ulRf4ceMsgQueueId, &ulBuffer, sizeof(ulBuffer));
		if(ulReturnCode != VK_OK)
		{
			PrintError( "[%s] VK_MSG_Receive Error", __FUNCTION__);
			continue;
		}

		do
		{
			// Set Msg
			VK_memset(&Msg, 0, sizeof(RF4CE_Msg_t));

			Msg.len = 0;
			Msg.subSys = RPC_CMD_POLL;
			Msg.cmdId = 0;

			eDrvError = P_RF4CE_SendSyncData(&Msg);
			if(eDrvError != DRV_OK)
			{
				PrintError("[%s] P_RF4CE_SendSyncData Error\n", __FUNCTION__);
				continue;
			}
			PrintDebug("[%s] SendSyncData Done\n", __FUNCTION__);

			//Check if polling was successful
			if((((HUINT8*)&Msg)[RPC_POS_CMD0] & RPC_CMD_TYPE_MASK) == RPC_CMD_AREQ)
			{
				(((HUINT8*)&Msg))[RPC_POS_CMD0] &=  RPC_SUBSYSTEM_MASK;
				P_RF4CE_EventHandler(&Msg);
			}
			PrintDebug("[%s] EventHandler Done\n", __FUNCTION__);

			eDrvError = P_RF4CE_GetSlaveReady(&srdy);
			if(eDrvError != DRV_OK)
			{
				PrintError("[%s] GetSlaveReady Error\n", __FUNCTION__);
				continue;
			}

			/* Check SRDY to confirm chip has no data to send */
			switch(srdy)
			{
				case GPIO_LOW:
					PrintDebug("[%s] SRDY is still LOW, Get data again\n", __FUNCTION__);
					break;
				case GPIO_HI:
					PrintInfo("[%s] SRDY is HIGH, Stop receiving data\n", __FUNCTION__);
					s_ulIntrFlag = CLR;
					break;
				case GPIO_INVAILD:
					PrintError("[%s] SRDY is INVALID, Return Error\n", __FUNCTION__);
				default:
					PrintError("[%s] SRDY is default, Return Error\n", __FUNCTION__);
					return;
			}
		}while(s_ulIntrFlag != CLR);
	}
}

//#define RF4CE_MSG_DEBUG
static void P_RF4CE_PrintCMD(RF4CE_Msg_t *pMsg)
{
#if defined(RF4CE_MSG_DEBUG)
	int i=0;

	P_RF4CE_PrintCMDHeader(pMsg);

	if(pMsg->len != 0)
	{
		for(i=0; i<pMsg->len; i++)
		{
			PrintDebug("pMsg->pData[%d] : 0x%x\n", i, pMsg->pData[i]);
		}
	}
#endif
}


static void P_RF4CE_PrintCMDHeader(RF4CE_Msg_t *pMsg)
{
#if defined(RF4CE_MSG_DEBUG)
	PrintInfo("pMsg->len : 0x%x\n", pMsg->len);
	PrintInfo("pMsg->subSys : 0x%x\n", pMsg->subSys);
	PrintInfo("pMsg->cmdId : 0x%x\n", pMsg->cmdId);
#endif
}

/********************************************************************
 Function	: P_RF4CE_ISR_SlaveReady
 Description :
 Input		: none
 Output 	: none
 Return 	: void
********************************************************************/
static void P_RF4CE_ISR_SlaveReady(void *context, int param)
{
	HUINT32 ulReturnCode = VK_OK;

	BSTD_UNUSED(context);
	BSTD_UNUSED(param);

	s_ulIntrCount++;

	// Fire Interrupt
	s_ulIntrFlag = SET;

	ulReturnCode = VK_MSG_SendTimeout(s_ulRf4ceMsgQueueId, &s_ulIntrCount, sizeof(s_ulIntrCount), 0);
	if(ulReturnCode != VK_OK)
	{
		PrintError( "[%s] VK_MSG_Send error : ulReturnCode is 0x%x\n", __FUNCTION__, ulReturnCode);
	}

	PrintInfo("[%s] Get Int from RF4CE, count = %d\n",__func__, s_ulIntrCount);
}

/********************************************************************
 Function	: P_RF4CE_EventHandler
 Description :
 Input		: none
 Output 	: none
 Return 	: void
********************************************************************/
static void P_RF4CE_EventHandler(RF4CE_Msg_t *pMsg)
{
	DRV_Error	eDrvError = DRV_OK;

	PrintDebug("[%s] Enter\n", __FUNCTION__);

	if (pMsg->subSys == RPC_SYS_RCAF)
	{
		PrintDebug("[%s:%d] RPC_SYS_RCAF, 0x%X\n", __FUNCTION__, __LINE__, RPC_SYS_RCAF);
		switch( pMsg->cmdId )
		{
			// confirmation to init request
			case RTIS_CMD_ID_RTI_INIT_CNF:
				PrintDebug("[%s] CMD INIT CNF\n", __FUNCTION__);
				eDrvError = DRV_RF4CE_TI_CNF_InitCnf( (HUINT8)pMsg->pData[0] );
				if(eDrvError != DRV_OK)
				{
					PrintError("[%s] InitCnf Error\n", __FUNCTION__);
				}
				break;

			// confirmation to allow pair request
			case RTIS_CMD_ID_RTI_ALLOW_PAIR_CNF:
				PrintDebug("[%s] CMD ALLOW PAIR CNF\n", __FUNCTION__);
				eDrvError = DRV_RF4CE_TI_CNF_AllowPairCnf( (HUINT8) pMsg->pData[0], pMsg->pData[1], pMsg->pData[2]);
				if(eDrvError != DRV_OK)
				{
					PrintError("[%s] AllowPairCnf Error\n", __FUNCTION__);
				}
				break;

			// indication of received data
			case RTIS_CMD_ID_RTI_REC_DATA_IND:
				PrintDebug("[%s] CMD REC DATA IND\n", __FUNCTION__);
				eDrvError = DRV_RF4CE_TI_IND_ReceiveDataInd( pMsg->pData[0], pMsg->pData[1],
									pMsg->pData[2] | (pMsg->pData[3] << 8), // vendor Id
									pMsg->pData[4],
									pMsg->pData[5],
									pMsg->pData[6],
									(HUINT8*)&(pMsg->pData[7]));
				if(eDrvError != DRV_OK)
				{
					PrintError("[%s] REC DATA IND Error\n", __FUNCTION__);
				}
				break;

			case RTIS_CMD_ID_RTI_UNPAIR_CNF:
				PrintDebug("[%s] CMD UNPAIR CNF\n", __FUNCTION__);
				eDrvError = DRV_RF4CE_CNF_UnpairCnf( (HUINT8) pMsg->pData[0],
										pMsg->pData[1] ); // dstIndex
				if(eDrvError != DRV_OK)
				{
					PrintError("[%s] UnpairCnf Error\n", __FUNCTION__);
				}
				break;

			case RTIS_CMD_ID_RTI_UNPAIR_IND:
				PrintDebug("[%s] CMD UNPAIR IND\n", __FUNCTION__);
				eDrvError = DRV_RF4CE_IND_UnpairInd( pMsg->pData[0] ); // dstIndex
				if(eDrvError != DRV_OK)
				{
					PrintError("[%s] UnpairInd Error\n", __FUNCTION__);
				}
				break;

			case RTIS_CMD_ID_RTI_RESET_IND:
				PrintInfo("[%s:%d] CMD RESET IND\n", __FUNCTION__, __LINE__);

				if(s_bInitialized == FALSE)
				{
					eDrvError = DRV_RF4CE_REQ_InitReq();
					if(eDrvError != DRV_OK)
					{
						PrintError("[%s:%d] InitReq Error\n", __FUNCTION__, __LINE__);
					}
				}
				break;

			case RTIS_HUMAX_CMD_ID_RTI_GET_RSSI_CNF:
				PrintInfo("[%s:%d] CE Get RSSI CNF\n", __FUNCTION__, __LINE__);
				eDrvError = DRV_RF4CE_CE_GetRssiCnf(pMsg);
				if(eDrvError != DRV_OK)
				{
					PrintError("[%s:%d] DRV_RF4CE_CE_GetRssiCnf Error\n", __FUNCTION__, __LINE__);
				}
				break;

			case RTIS_HUMAX_CMD_ID_RTI_GET_CONTEXT_CNF:
				PrintInfo("[%s:%d] CE Get Context CNF\n", __FUNCTION__, __LINE__);
				eDrvError = DRV_RF4CE_CE_GetContextCnf(pMsg);
				if(eDrvError != DRV_OK)
				{
					PrintError("[%s:%d] DRV_RF4CE_CE_GetContextCnf Error\n", __FUNCTION__, __LINE__);
				}
				break;

			default:
				// nothing can be done here!
				PrintError("[RPC_SYS_RCAF] DEFAULT, subSys : 0x%02X cmdId : 0x%02X, len : 0x%02X\n", pMsg->subSys,
																									pMsg->cmdId,
																									pMsg->len);
				break;
		}
	}
#if 0
	else if(pMsg->subSys == RPC_SYS_BOOT)
	{
		PrintInfo("[%s:%d] RPC_SYS_BOOT, 0x%X\n", __FUNCTION__, __LINE__, RPC_SYS_BOOT);
		switch( pMsg->cmdId )
		{
			// HANDSHAKE Confirm
			case (SBL_HANDSHAKE_CMD | SBL_RSP_MASK) :
				PrintInfo("[RPC_SYS_BOOT] HANDSHAKE, subSys : 0x%02X cmdId : 0x%02X, len : 0x%02X, status : 0x%X\n", pMsg->subSys,
																									pMsg->cmdId,
																									pMsg->len,
																									pMsg->pData[0]);
				break;

			// WRITE Confirm
			case (SBL_WRITE_CMD | SBL_RSP_MASK) :
				PrintInfo("[RPC_SYS_BOOT] WRITE, subSys : 0x%02X cmdId : 0x%02X, len : 0x%02X, status : 0x%X\n", pMsg->subSys,
																									pMsg->cmdId,
																									pMsg->len,
																									pMsg->pData[0]);
				break;

			// READ Confirm
			case (SBL_READ_CMD | SBL_RSP_MASK) :
				PrintInfo("[RPC_SYS_BOOT] READ, subSys : 0x%02X cmdId : 0x%02X, len : 0x%02X, status : 0x%X\n", pMsg->subSys,
																									pMsg->cmdId,
																									pMsg->len,
																									pMsg->pData[0]);
				break;

			case (SBL_ENABLE_CMD | SBL_RSP_MASK) :
				PrintInfo("[RPC_SYS_BOOT] ENABLE, subSys : 0x%02X cmdId : 0x%02X, len : 0x%02X, status : 0x%X\n", pMsg->subSys,
																									pMsg->cmdId,
																									pMsg->len,
																									pMsg->pData[0]);
				break;

			default:
				// nothing can be done here!
				PrintInfo("[RPC_SYS_BOOT] DEFAULT, subSys : 0x%02X cmdId : 0x%02X, len : 0x%02X, status : 0x%X\n", pMsg->subSys,
																									pMsg->cmdId,
																									pMsg->len,
																									pMsg->pData[0]);
				break;
		}
	}
#endif
}

static void P_RF4CE_SrdyPollingTask(void *pvParam)
{
	DRV_Error	nDrvErr = DRV_OK;
	GPIO_STATE_t state = GPIO_INVAILD;

	pvParam = pvParam;

	while(1)
	{
		if(s_bSrdyPolling == TRUE)
		{
			nDrvErr = P_RF4CE_GetSlaveReady(&state);
			if(nDrvErr != DRV_OK)
			{
				PrintError("GetSlaveReady FAIL\n");
			}
			else
			{
				if(state == GPIO_HI)
				{
					PrintInfo("[SRDY] HIGH \n");
				}
				else if(state == GPIO_LOW)
				{
					PrintInfo("[SRDY] LOW \n");
				}
				else if(state == GPIO_INVAILD)
				{
					PrintInfo("[SRDY] INVALID \n");
				}
			}

			VK_TASK_Sleep(1000);
		}
		else
		{
			VK_TASK_Sleep(5 * 1000);
		}
	}

}

/********************************************************************
Function   : P_RF4CE_SetMasterReady
Description : Set MRDY.
Input	   : state
Output	   : none
Return	   : DI_ERR_CODE
********************************************************************/
static DRV_Error P_RF4CE_SetMasterReady (GPIO_STATE_t state)
{
	DRV_Error 	eDrvError = DRV_OK;

	if(state == GPIO_LOW)
	{
		PrintDebug("[SetMasterReady] MRDY set to Low\n");
		eDrvError = DRV_GPIO_Write(GPIO_ID_RF4CE_MRDY, state);
		if (eDrvError != DRV_OK)
		{
			PrintError("[SetMasterReady] DRV_GPIO_Write Error\n");
			return DRV_ERR;
		}
	}
	else
	{
		PrintDebug("[SetMasterReady] MRDY set to High\n");
		eDrvError = DRV_GPIO_Write(GPIO_ID_RF4CE_MRDY, state);
		if (eDrvError != DRV_OK)
		{
			PrintError("[SetMasterReady] DRV_GPIO_Write Error\n");
			return DRV_ERR;
		}
	}

	return DRV_OK;
}



/********************************************************************
Function   : P_RF4CE_Reset
Description : Reset
Input	   : state
Output	   : none
Return	   : DI_ERR_CODE
********************************************************************/
static DRV_Error P_RF4CE_Reset(void)
{
	DRV_Error 	eDrvError = DRV_OK;

	PrintDebug("Set Reset High\n");
	eDrvError = DRV_GPIO_Write(GPIO_ID_RF4CE_RST, GPIO_HI);
	if (eDrvError != DRV_OK)
	{
		PrintError("[Reset] DRV_GPIO_Write(Set Reset High) Error\n");
		return DRV_ERR;
	}

	// Pause
	VK_TASK_Sleep(RESET_INTERVAL);

	PrintDebug("Set Reset Low\n");
	eDrvError = DRV_GPIO_Write(GPIO_ID_RF4CE_RST, GPIO_LOW);
	if (eDrvError != DRV_OK)
	{
		PrintError("[Reset] DRV_GPIO_Write(Set Reset Low) Error\n");
		return DRV_ERR;
	}

	// Pause
	VK_TASK_Sleep(RESET_INTERVAL);

	PrintDebug("Set Reset High\n");
	eDrvError = DRV_GPIO_Write(GPIO_ID_RF4CE_RST, GPIO_HI);
	if (eDrvError != DRV_OK)
	{
		PrintError("[Reset] DRV_GPIO_Write(Set Reset High) Error\n");
		return DRV_ERR;
	}

	// Wait until rf4ce init is done
	VK_TASK_Sleep(RESET_INTERVAL);

	return DRV_OK;
}

/********************************************************************
Function   : P_RF4CE_CheckSlaveReady
Description : Check SRDY Clear.
Input	   : none
Output	   : state
Return	   : DI_ERR_CODE
********************************************************************/
static DRV_Error P_RF4CE_GetSlaveReady(GPIO_STATE_t* state)
{
	DRV_Error 	eDrvError = DRV_OK;

	eDrvError = DRV_GPIO_Read(GPIO_ID_RF4CE_SRDY, state);
	if (eDrvError != DRV_OK)
	{
		PrintError("[CheckSlaveReady] DRV_GPIO_Read Error\n");
		return DRV_ERR;
	}

	return DRV_OK;
}

/********************************************************************
Function   : P_RF4CE_SendAsyncData
Description : Check SRDY Clear.
Input	   : none
Output	   : state
Return	   : DI_ERR_CODE
********************************************************************/
static DRV_Error P_RF4CE_SendAsyncData (RF4CE_Msg_t *pMsg)
{
	HUINT32 vResult = VK_ERROR;
	DRV_Error 	eDrvError = DRV_ERR;

	//AREQ : Async
	pMsg->subSys = ((pMsg->subSys & RPC_SUBSYSTEM_MASK) | RPC_CMD_AREQ);

	vResult = VK_SEM_Get(s_ulRf4ceSemId);
	if(vResult != VK_OK)
	{
		PrintError("[%s] VK_SEM_Get error \n", __FUNCTION__);
		return DRV_ERR;
	}

	eDrvError = P_RF4CE_SetMasterReady(GPIO_LOW);
	if(eDrvError != DRV_OK)
	{
		PrintError("[%s] SetMasterReady(LOW) Error\n", __FUNCTION__);
		VK_SEM_Release(s_ulRf4ceSemId);
		return DRV_ERR;
	}

#if 1
	eDrvError = P_RF4CE_SetMasterReady(GPIO_HI);
	if(eDrvError != DRV_OK)
	{
		PrintError("[RF4CE_SendSyncData] SetMasterReady(HIGH) Error\n");
		VK_SEM_Release(s_ulRf4ceSemId);
		return DRV_ERR;
	}
#endif

	VK_TASK_Sleep(10);
	PrintInfo("[RF4CE SendAsyncData] Sending CMD\n");
	P_RF4CE_PrintCMDHeader(pMsg);

	eDrvError = DRV_I2c_WriteNoAddr(I2C_CHANNEL_RF4CE, RF4CE_I2C_SLAVE_ADDR, (HUINT8*)pMsg, RPC_FRAME_HDR_SZ + (pMsg->len));
	if(eDrvError != DRV_OK)
	{
		PrintError("[%s] I2C WriteNoAddr Error\n", __FUNCTION__);
		VK_SEM_Release(s_ulRf4ceSemId);
		return DRV_ERR;
	}

	VK_TASK_Sleep(10);

	eDrvError = P_RF4CE_SetMasterReady(GPIO_HI);
	if(eDrvError != DRV_OK)
	{
		PrintError("[%s] SetMasterReady(HIGH) Error\n", __FUNCTION__);
		VK_SEM_Release(s_ulRf4ceSemId);
		return DRV_ERR;
	}

	vResult = VK_SEM_Release(s_ulRf4ceSemId);
	if(vResult != VK_OK)
	{
		PrintError("[%s] VK_SEM_Release error \n", __FUNCTION__);
		return DRV_ERR;
	}

	return DRV_OK;
}

/********************************************************************
Function   : P_RF4CE_SendSyncData
Description : Check SRDY Clear.
Input	   : none
Output	   : state
Return	   : DI_ERR_CODE
********************************************************************/
static DRV_Error P_RF4CE_SendSyncData (RF4CE_Msg_t *pMsg)
{
	HUINT32 vResult = VK_ERROR;
	DRV_Error 	eDrvError = DRV_ERR;

	//POLL : Polling
	pMsg->subSys = ((pMsg->subSys & RPC_SUBSYSTEM_MASK) | RPC_CMD_POLL);

	vResult = VK_SEM_Get(s_ulRf4ceSemId);
	if(vResult != VK_OK)
	{
		PrintError("[%s] VK_SEM_Get error \n", __FUNCTION__);
		return DRV_ERR;
	}

	eDrvError = P_RF4CE_SetMasterReady(GPIO_LOW);
	if(eDrvError != DRV_OK)
	{
		PrintError("[%s] SetMasterReady(LOW) Error\n", __FUNCTION__);
		VK_SEM_Release(s_ulRf4ceSemId);
		return DRV_ERR;
	}

#if 1
	eDrvError = P_RF4CE_SetMasterReady(GPIO_HI);
	if(eDrvError != DRV_OK)
	{
		PrintError("[RF4CE_SendSyncData] SetMasterReady(HIGH) Error\n");
		VK_SEM_Release(s_ulRf4ceSemId);
		return DRV_ERR;
	}
#endif

	//This wait is only valid if RNP mange MRDY line, if not SRDY will never be set low on SREQ.
	// To avoid any problem, just add a timeout, or ignore it.
	//P_RF4CE_WaitSlaveReadyClear();
	VK_TASK_Sleep(s_ulSendSyncDataInterval);

	PrintDebug("[%s] Sending POLL CMD\n", __FUNCTION__);
	P_RF4CE_PrintCMD(pMsg);

	//Send LEN, CMD0 and CMD1 (comand Header)
	eDrvError = DRV_I2c_WriteNoAddr(I2C_CHANNEL_RF4CE, RF4CE_I2C_SLAVE_ADDR, (HUINT8*)pMsg, RPC_FRAME_HDR_SZ + (pMsg->len));
	if(eDrvError != DRV_OK)
	{
		PrintError("[%s] I2C WriteNoAddr Error\n", __FUNCTION__);
		VK_SEM_Release(s_ulRf4ceSemId);
		return DRV_ERR;
	}

	VK_memset(pMsg, 0xff, sizeof(RF4CE_Msg_t));

	VK_TASK_Sleep(s_ulSendSyncDataInterval);

	// Receive 3Bytes
	PrintDebug("[%s] Receiving CMD Header...\n", __FUNCTION__);
	eDrvError = DRV_I2c_ReadNoAddr(I2C_CHANNEL_RF4CE, RF4CE_I2C_SLAVE_ADDR, (HUINT8*)pMsg, RPC_FRAME_HDR_SZ);
	if(eDrvError != DRV_OK)
	{
		PrintError("[%s] I2C ReadNoAddr - CMD Header Error\n", __FUNCTION__);
		VK_SEM_Release(s_ulRf4ceSemId);
		return DRV_ERR;
	}
	P_RF4CE_PrintCMDHeader(pMsg);

	VK_TASK_Sleep(s_ulSendSyncDataInterval);

	if(pMsg->len != 0)
	{

		//Read RSP Data
		PrintDebug("[%s] Receiving CMD Data...\n", __FUNCTION__);
		eDrvError = DRV_I2c_ReadNoAddr(I2C_CHANNEL_RF4CE, RF4CE_I2C_SLAVE_ADDR, (HUINT8*)&(pMsg->pData[0]), (pMsg->len));
		if(eDrvError != DRV_OK)
		{
			PrintError("[%s] I2C ReadNoAddr - CMD Error\n", __FUNCTION__);
			VK_SEM_Release(s_ulRf4ceSemId);
			return DRV_ERR;
		}
		P_RF4CE_PrintCMD(pMsg);
	}

	eDrvError = P_RF4CE_SetMasterReady(GPIO_HI);
	if(eDrvError != DRV_OK)
	{
		PrintError("[RF4CE_SendSyncData] SetMasterReady(HIGH) Error\n");
		VK_SEM_Release(s_ulRf4ceSemId);
		return DRV_ERR;
	}

	vResult = VK_SEM_Release(s_ulRf4ceSemId);
	if(vResult != VK_OK)
	{
		PrintError("[%s] VK_SEM_Release error \n", __FUNCTION__);
		return DRV_ERR;
	}

	return DRV_OK;
}

/*******************************************************************************
******************************** DRV Functions *********************************
*******************************************************************************/
DRV_Error DRV_RF4CE_SetPairing(HBOOL bPairing, HUINT32 ulTimeout)
{
	DRV_Error	eDrvError = DRV_ERR;

	ulTimeout = ulTimeout;

	if(bPairing == TRUE)
	{
		eDrvError = DRV_RF4CE_TI_REQ_AllowPairReq();
		if(eDrvError != DRV_OK)
		{
			PrintError("[%s] Error\n", __FUNCTION__);
			return DRV_ERR;
		}
	}
	else // bPairing == FALSE
	{

	}

	return DRV_OK;
}

DRV_Error DRV_RF4CE_TI_Reset(void)
{
	P_RF4CE_Reset();

	return DRV_OK;
}

DRV_Error DRV_RF4CE_SetSrdyPolling(HBOOL OnOff)
{
	s_bSrdyPolling = OnOff;

	return DRV_OK;
}

DRV_Error DRV_RF4CE_InterruptInit(void)
{
	// Register ISR
	DRV_GPIO_INTR_RegisterEventNotify(GPIO_ID_RF4CE_SRDY, P_RF4CE_ISR_SlaveReady, (void *)NULL, (int)NULL);
	DRV_GPIO_INTR_Enable(GPIO_ID_RF4CE_SRDY);

	return DRV_OK;
}

DRV_Error DRV_RF4CE_TI_Init(void)
{
	HUINT32 iResult = VK_ERROR;
	DRV_Error 	eDrvError = DRV_ERR;
	HUINT32 ucVersion = 0;

	RF4CE_Msg_t	Msg;

	static HUINT32	s_ulRetryCount = 0;

	// I2C Init
	eDrvError = DRV_I2c_SetClock( I2C_CHANNEL_RF4CE, DRV_I2C_CLOCK_100K);
	if(eDrvError != DRV_OK)
	{
		PrintError("[RF4CE] DRV_I2C_SetClock Error\n");
		return DRV_ERR;
	}

	// Semaphore
	iResult = VK_SEM_Create(&s_ulRf4ceSemId, "RF4CE", DEFAULT_SUSPEND_TYPE);
	if(iResult != VK_OK)
	{
		PrintError("[RF4CE] VK_SEM_Create Error\n");
		return DRV_ERR;
	}

	//MSG Queue
	iResult = VK_MSG_Create(RF4CE_MESSAGE_QUEUE_SIZE, RF4CE_MESSAGE_BYTES, "RF4CE_Key_Msg", &s_ulRf4ceMsgQueueId, VK_SUSPENDTYPE_FIFO);
	if(iResult != VK_OK)
	{
		PrintError("[DRV_MICOM_Init] VK_MSG_Create Error\n");
		return DRV_ERR;
	}

	// Tasks
	iResult = VK_TASK_Create(P_RF4CE_ReceiveTask, VIDEO_MSG_TASK_PRIORITY, VIDEO_MSG_TASK_STACK_SIZE, "RF4CE_RecTask", (void *)0, &s_ulRf4ceReceiveTaskId, FALSE);
	if(iResult != VK_OK)
	{
		PrintError("[RF4CE] P_RF4CE_ReceiveTask Create Error\n");
		return DRV_ERR;
	}

	iResult = VK_TASK_Start(s_ulRf4ceReceiveTaskId);
	if(iResult != VK_OK)
	{
		PrintError("[RF4CE] P_RF4CE_ReceiveTask Start Error\n");
		return DRV_ERR;
	}

	// debug
	iResult = VK_TASK_Create(P_RF4CE_SrdyPollingTask, VIDEO_MSG_TASK_PRIORITY, VIDEO_MSG_TASK_STACK_SIZE, "RF4CE_SrdyPollingTask", (void *)0, &s_ulRf4cePollingSrdyTaskId, FALSE);
	if(iResult != VK_OK)
	{
		PrintError("[RF4CE] P_RF4CE_SrdyPollingTask Create Error\n");
		return DRV_ERR;
	}

	iResult = VK_TASK_Start(s_ulRf4cePollingSrdyTaskId);
	if(iResult != VK_OK)
	{
		PrintError("[RF4CE] P_RF4CE_SrdyPollingTask Start Error\n");
		return DRV_ERR;
	}

	// Interrupt Init
	eDrvError = DRV_RF4CE_InterruptInit();
	if(eDrvError != DRV_OK)
	{
		PrintError("[RF4CE] DRV_RF4CE_InterruptInit Error\n");
		return DRV_ERR;
	}

	VK_memset(&Msg, 0, sizeof(RF4CE_Msg_t));

	s_ulSendSyncDataInterval = SHORT_INTERVAL;

	// determine endianness
	s_ucRtisBE = P_RF4CE_CheckEndian();

chipInit:

	PrintDebug("[RF4CE] ConfigParam\n");
	eDrvError = DRV_RF4CE_TI_ConfigParam();
	if(eDrvError != DRV_OK)
	{
		PrintError("[RF4CE] DRV_RF4CE_TI_ConfigParam Error\n");
		PrintError("[RF4CE] Reset Chip and go to chipInit\n");

		P_RF4CE_Reset();

		s_ulRetryCount++;
		PrintError("[RF4CE] Retry Count : %d\n", s_ulRetryCount);

		if(s_ulRetryCount < MAX_RETRY_COUNT)
		{
			goto chipInit;
		}
		else
		{
			PrintError("[RF4CE] No More Retry, Error Return\n");
			return DRV_ERR;
		}
	}

	PrintDebug("[RF4CE] GetSWVersion\n");
	eDrvError = DRV_RF4CE_GetSWVersion(&ucVersion);
	if(eDrvError != DRV_OK)
	{
		PrintError("[RF4CE] DRV_RF4CE_TI_GetSWVersion() Error\n");
		PrintError("[RF4CE] Reset Chip and go to chipInit\n");

		P_RF4CE_Reset();

		s_ulRetryCount++;
		PrintError("[RF4CE] Retry Count : %d\n", s_ulRetryCount);

		if(s_ulRetryCount < MAX_RETRY_COUNT)
		{
			goto chipInit;
		}
		else
		{
			PrintError("[RF4CE] No More Retry, Error Return\n");
			return DRV_ERR;
		}
	}

	// RF4CE Reset
	eDrvError = P_RF4CE_Reset();
	if(eDrvError != DRV_OK)
	{
		PrintError("[RF4CE] P_RF4CE_Reset Error\n");
		return DRV_ERR;
	}

	VK_memset(&s_stTargetInfo, 0, sizeof(RF4CE_TargetInfo_t));

	s_ulRetryCount = 0;

	return DRV_OK;
}

DRV_Error DRV_RF4CE_RegisterRf4ceEventCallback(pfnRF4CE_KeyCallback cbRf4ceKeyCallback)
{
	pfnRf4ceKeyCallback = cbRf4ceKeyCallback;

	return DRV_OK;
}

DRV_Error DRV_RF4CE_TI_ConfigParam(void)
{
	HUINT8 pValue[MAX_AVAIL_DEVICE_TYPES]; // space for largest number of bytes, not counting strings
	HUINT32 i =0;
	HUINT8 tmp;
	HUINT32 status = 0;
	DRV_Error eDrvError = DRV_OK;

	PrintInfo("[RF4CE] DRV_RF4CE_TI_ReadItem - RF4CE_CP_ITEM_STARTUP_CTRL\n");
	eDrvError = DRV_RF4CE_TI_ReadItem(RF4CE_CP_ITEM_STARTUP_CTRL, 1, &tmp, &status);
	if(eDrvError != DRV_OK)
	{
		PrintError("[RF4CE] DRV_RF4CE_TI_ReadItem(RF4CE_CP_ITEM_STARTUP_CTRL) Error\n");
		return DRV_ERR;
	}
	if(status != RF4CE_SUCCESS)
	{
		PrintError("[RF4CE][Error] DRV_RF4CE_TI_ReadItem(RF4CE_CP_ITEM_STARTUP_CTRL) returned status 0x%x\n", status);
		return DRV_ERR;
	}

	if(tmp != RESTORE_STATE)
	{
		PrintError("[RF4CE][Error] RF4CE_CP_ITEM_STARTUP_CTRL error, state : 0x%x\n", tmp);
		return DRV_ERR;
	}

	PrintInfo("[RF4CE] DRV_RF4CE_TI_ReadItem - RF4CE_CP_ITEM_NODE_SUPPORTED_TGT_TYPES\n");
	eDrvError = DRV_RF4CE_TI_ReadItem(RF4CE_CP_ITEM_NODE_SUPPORTED_TGT_TYPES, 1, pValue, &status);
	if(eDrvError != DRV_OK)
	{
		PrintError("[RF4CE] DRV_RF4CE_TI_ReadItem(RF4CE_CP_ITEM_NODE_SUPPORTED_TGT_TYPES) Error\n");
		return DRV_ERR;
	}
	if(status != RF4CE_SUCCESS)
	{
		PrintError("[RF4CE][Error] DRV_RF4CE_TI_ReadItem(RF4CE_CP_ITEM_NODE_SUPPORTED_TGT_TYPES) returned status 0x%x\n", status);
		return DRV_ERR;
	}
	if ( pValue[0] != RF4CE_DEVICE_REMOTE_CONTROL)
	{
		PrintError("[RF4CE][Error] RTI_CP_ITEM_NODE_SUPPORTED_TGT_TYPES error: 0x%x\n", pValue[0]);
		return DRV_ERR;
	}

	// No User String pairing; 1 Device (Television); 2 Profiles (ZRC & ZID)
	PrintInfo("[RF4CE] DRV_RF4CE_TI_ReadItem - RF4CE_CP_ITEM_APPL_CAPABILITIES\n");
	eDrvError = DRV_RF4CE_TI_ReadItem(RF4CE_CP_ITEM_APPL_CAPABILITIES, 1, pValue, &status);
	if(eDrvError != DRV_OK)
	{
		PrintError("[RF4CE] DRV_RF4CE_TI_ReadItem(RF4CE_CP_ITEM_APPL_CAPABILITIES) Error\n");
		return DRV_ERR;
	}
	if(status != RF4CE_SUCCESS)
	{
		PrintError("[RF4CE][Error] DRV_RF4CE_TI_ReadItem(RF4CE_CP_ITEM_APPL_CAPABILITIES) returned status 0x%x\n", status);
		return DRV_ERR;
	}
	if(pValue[0] != RTI_BUILD_APP_CAPABILITIES(0, 1, 2))
	{
		PrintError("[RF4CE][Error] RTI_CP_ITEM_APPL_CAPABILITIES error: 0x%x\n", pValue[0]);
		return DRV_ERR;
	}

	PrintInfo("[RF4CE] DRV_RF4CE_TI_ReadItem - RF4CE_CP_ITEM_APPL_DEV_TYPE_LIST\n");
	eDrvError = DRV_RF4CE_TI_ReadItem(RF4CE_CP_ITEM_APPL_DEV_TYPE_LIST, 1, pValue, &status);
	if(eDrvError != DRV_OK)
	{
		PrintError("[RF4CE] DRV_RF4CE_TI_ReadItem(RF4CE_CP_ITEM_APPL_DEV_TYPE_LIST) Error\n");
		return DRV_ERR;
	}
	if(status != RF4CE_SUCCESS)
	{
		PrintError("[RF4CE][Error] DRV_RF4CE_TI_ReadItem(RF4CE_CP_ITEM_APPL_DEV_TYPE_LIST) returned status 0x%x\n", status);
		return DRV_ERR;
	}
	if(pValue[0] != RF4CE_DEVICE_SET_TOP_BOX)
	{
		PrintError("[RF4CE][Error] RTI_CP_ITEM_APPL_DEV_TYPE_LIST error: 0x%x\n", pValue[0]);
		return DRV_ERR;
	}

	PrintInfo("[RF4CE] DRV_RF4CE_TI_ReadItem - RF4CE_CP_ITEM_APPL_PROFILE_ID_LIST\n");
	eDrvError = DRV_RF4CE_TI_ReadItem(RF4CE_CP_ITEM_APPL_PROFILE_ID_LIST, RTI_MAX_NUM_PROFILE_IDS, profileList, &status);
	if(eDrvError != DRV_OK)
	{
		PrintError("[RF4CE] DRV_RF4CE_TI_ReadItem(RF4CE_CP_ITEM_APPL_PROFILE_ID_LIST) Error\n");
		return DRV_ERR;
	}
	if(status != RF4CE_SUCCESS)
	{
		PrintError("[RF4CE][Error] DRV_RF4CE_TI_ReadItem(RF4CE_CP_ITEM_APPL_PROFILE_ID_LIST) returned status 0x%x\n", status);
		return DRV_ERR;
	}

	if( (profileList[0] != RF4CE_PROFILE_ZRC) || (profileList[1] != RF4CE_PROFILE_ZID))
	{
		for (i =0; i < RTI_MAX_NUM_PROFILE_IDS ; i++)
		{
			PrintError("[RF4CE][Error] RTI_CP_ITEM_APPL_PROFILE_ID_LIST error: 0x%x\n", profileList[i]);
		}
		return DRV_ERR;
	}

	PrintInfo("[RF4CE] DRV_RF4CE_TI_ReadItem - RF4CE_CP_ITEM_NODE_CAPABILITIES\n");
	eDrvError = DRV_RF4CE_TI_ReadItem(RF4CE_CP_ITEM_NODE_CAPABILITIES, 1, pValue, &status);
	if(eDrvError != DRV_OK)
	{
		PrintError("[RF4CE] DRV_RF4CE_TI_ReadItem(RF4CE_CP_ITEM_NODE_CAPABILITIES) Error\n");
		return DRV_ERR;
	}
	if(status != RF4CE_SUCCESS)
	{
		PrintError("[RF4CE][Error] DRV_RF4CE_TI_ReadItem(RF4CE_CP_ITEM_NODE_CAPABILITIES) returned status 0x%x\n", status);
		return DRV_ERR;
	}
	if( pValue[0] != RTI_BUILD_NODE_CAPABILITIES(1,0,1,0))
	{
		PrintError("[RF4CE][Error] RTI_CP_ITEM_NODE_CAPABILITIES error: 0x%x\n", pValue[0]);
		PrintError("[RF4CE][Error] RTI_BUILD_NODE_CAPABILITIES : 0x%x\n", RTI_BUILD_NODE_CAPABILITIES(1,0,1,0));

#if 0
		return DRV_ERR;
#endif
	}

	PrintInfo("[RF4CE] DRV_RF4CE_TI_ReadItem - RF4CE_CP_ITEM_VENDOR_NAME\n");
	eDrvError = DRV_RF4CE_TI_ReadItem(RF4CE_CP_ITEM_VENDOR_NAME, RTI_VENDOR_STRING_LENGTH, vendorName, &status);
	if(eDrvError != DRV_OK)
	{
		PrintError("[RF4CE] DRV_RF4CE_TI_ReadItem(RF4CE_CP_ITEM_VENDOR_NAME) Error\n");
		return DRV_ERR;
	}
	if(status != RF4CE_SUCCESS)
	{
		PrintError("[RF4CE][Error] DRV_RF4CE_TI_ReadItem(RF4CE_CP_ITEM_VENDOR_NAME) returned status 0x%x\n", status);
		return DRV_ERR;
	}

	for(i=0; i <RTI_VENDOR_STRING_LENGTH; i++)
	{
		PrintError("[RF4CE] RF4CE_CP_ITEM_VENDOR_NAME[%d] : %c\n", i, vendorName[i]);
	}

	PrintInfo("[RF4CE] DRV_RF4CE_TI_ReadItem - RF4CE_CP_ITEM_VENDOR_ID\n");
	eDrvError = DRV_RF4CE_TI_ReadItem(RF4CE_CP_ITEM_VENDOR_ID, 2, pValue, &status);
	if(eDrvError != DRV_OK)
	{
		PrintError("[RF4CE] DRV_RF4CE_TI_ReadItem(RF4CE_CP_ITEM_VENDOR_ID) Error\n");
		return DRV_ERR;
	}
	if(status != RF4CE_SUCCESS)
	{
		PrintError("[RF4CE][Error] DRV_RF4CE_TI_ReadItem(RF4CE_CP_ITEM_VENDOR_ID) returned status 0x%x\n", status);
		return DRV_ERR;
	}
	PrintError("[RF4CE] RF4CE_CP_ITEM_VENDOR_ID : 0x%x%2x\n", pValue[1],pValue[0]);

	return DRV_OK;
}

/*******************************************************************************
* function : DRV_RF4CE_GetSWVersion
* description :
* input :
* output :
* return :
* 기타 참고자료 및 파일
*******************************************************************************/
DRV_Error DRV_RF4CE_GetSWVersion(HUINT32 *ucVersion)
{
	DRV_Error eDrvError = DRV_OK;
	HUINT32 status = 0;

	eDrvError = DRV_RF4CE_TI_ReadItem(RF4CE_CONST_ITEM_SW_VERSION, 1, (HUINT8*)ucVersion, &status);
	if(eDrvError != DRV_OK)
	{
		PrintError("[%s] DRV_RF4CE_TI_ReadItem(RTI_CONST_ITEM_SW_VERSION) Error\n", __FUNCTION__);
		return DRV_ERR;
	}
	if(status != RF4CE_SUCCESS)
	{
		PrintError("[%s] DRV_RF4CE_TI_ReadItem(RTI_CONST_ITEM_SW_VERSION) returned status Error\n", __FUNCTION__);
		return DRV_ERR;
	}

	PrintInfo("[%s] RF4CE SW Version : 0x%X, Major : 0x%X, Minor : 0x%X\n",
															__FUNCTION__,
															*ucVersion,
															(*ucVersion & 0xF0) >> 4,
															*ucVersion & 0x0F);

	return DRV_OK;
}

/*******************************************************************************
* function : DRV_RF4CE_ClearPairingTable
* description :
* input :
* output :
* return :
* 기타 참고자료 및 파일
*******************************************************************************/

DRV_Error DRV_RF4CE_ClearPairingTable(void)
{
	RF4CE_PairingEntry_t stPairingTableEntry;

	HUINT32 i =0;
	HUINT32 result = 0;
	HUINT32 pairingTableSize = 0;
	HUINT32 status = 0;

	DRV_Error eDrvErr = DRV_OK;

	eDrvErr = DRV_RF4CE_TI_ReadItemEx(RF4CE_PROFILE_RTI, RF4CE_CONST_ITEM_MAX_PAIRING_TABLE_ENTRIES, 1, (HUINT8 *)&pairingTableSize, &status);
	if(eDrvErr != DRV_OK)
	{
		PrintError("[%s:%d] DRV_RF4CE_TI_ReadItemEx(RF4CE_CONST_ITEM_MAX_PAIRING_TABLE_ENTRIES) Error\n", __FUNCTION__, __LINE__);
		return DRV_ERR;
	}
	if(status != RF4CE_SUCCESS)
	{
		PrintError("[%s:%d] DRV_RF4CE_TI_ReadItemEx(RF4CE_CONST_ITEM_MAX_PAIRING_TABLE_ENTRIES) Returned Status Error, Status : 0x%X\n", __FUNCTION__, __LINE__, status);
		return DRV_ERR;
	}
	PrintInfo("[%s] Pairing Table Size = 0x%X\n", __FUNCTION__, pairingTableSize);

	VK_memset(&stPairingTableEntry, 0x00, sizeof(RF4CE_PairingEntry_t));

	for (i = 0; i < pairingTableSize; i++)
	{
		PrintInfo("[%s] Current Pairing Table Entry = 0x%X\n", __FUNCTION__, i);
		VK_memset(&stPairingTableEntry, 0x00, sizeof(RF4CE_PairingEntry_t));

		// Set current pairing entry
		eDrvErr = DRV_RF4CE_TI_WriteItemEx(RF4CE_PROFILE_RTI, RF4CE_SA_ITEM_PT_CURRENT_ENTRY_INDEX, 1, (HUINT8 *)&i, &status);
		if(eDrvErr != DRV_OK)
		{
			PrintError("[%s:%d] DRV_RF4CE_TI_WriteItemEx(RF4CE_SA_ITEM_PT_CURRENT_ENTRY_INDEX) Error\n", __FUNCTION__, __LINE__);
			return DRV_ERR;
		}
		if(status != RF4CE_SUCCESS)
		{
			PrintError("[%s:%d] DRV_RF4CE_TI_WriteItemEx(RF4CE_SA_ITEM_PT_CURRENT_ENTRY_INDEX) Returned Status Error, Status : 0x%X\n", __FUNCTION__, __LINE__, status);
			return DRV_ERR;
		}

		// Try to read out this entry
		eDrvErr = DRV_RF4CE_TI_ReadItemEx(RF4CE_PROFILE_RTI, RF4CE_SA_ITEM_PT_CURRENT_ENTRY, sizeof(RF4CE_PairingEntry_t),(HUINT8 *)&stPairingTableEntry, &status);
		if(eDrvErr != DRV_OK)
		{
			PrintError("[%s:%d] DRV_RF4CE_TI_ReadItemEx(RF4CE_SA_ITEM_PT_CURRENT_ENTRY) Error\n", __FUNCTION__, __LINE__);
			return DRV_ERR;
		}
		if(status == RF4CE_ERROR_NO_PAIRING_INDEX)
		{
			PrintInfo("[%s:%d] DRV_RF4CE_TI_WriteItemEx(RF4CE_SA_ITEM_PT_CURRENT_ENTRY) Returned Status : 0x%X\n", __FUNCTION__, __LINE__, status);
			PrintInfo("[%s:%d] There is No More Pairing Index\n", __FUNCTION__, __LINE__);
			return DRV_OK;
		}
		if(status != RF4CE_SUCCESS)
		{
			PrintError("[%s:%d] DRV_RF4CE_TI_ReadItemEx(RF4CE_SA_ITEM_PT_CURRENT_ENTRY) Returned Status Error, Status : 0x%X\n", __FUNCTION__, __LINE__, status);
			return DRV_ERR;
		}

		stPairingTableEntry.pairingRef = RTI_INVALID_PAIRING_REF;
		eDrvErr = DRV_RF4CE_TI_WriteItemEx(RF4CE_PROFILE_RTI, RF4CE_SA_ITEM_PT_CURRENT_ENTRY, sizeof(RF4CE_PairingEntry_t),	(HUINT8 *)&stPairingTableEntry, &status);
		if(eDrvErr != DRV_OK)
		{
			PrintError("[%s:%d] DRV_RF4CE_TI_WriteItemEx(RF4CE_SA_ITEM_PT_CURRENT_ENTRY) Error\n", __FUNCTION__, __LINE__);
			return DRV_ERR;
		}
		if(status != RF4CE_SUCCESS)
		{
			PrintError("[%s:%d] DRV_RF4CE_TI_WriteItemEx(RF4CE_SA_ITEM_PT_CURRENT_ENTRY) Returned Status Error, Status : 0x%X\n", __FUNCTION__, __LINE__, status);
			return DRV_ERR;
		}
	}

	return DRV_OK;
}

/**************************************************************************************************
 *
 * @fn          DRV_RF4CE_ReadItemEx
 *
 * @brief       This API is used to read an item from a Profile's Configuration Interface.
 *
 * input parameters
 *
 * @param       profileId - The Profile identifier.
 * @param       itemId - The Configuration Interface item identifier.
 * @param       len - The length in bytes of the item identifier's data.
 *
 * output parameters
 *
 * @param       *pValue - Pointer to buffer where read data is placed.
 *
 * @return      RF4CE_SUCCESS, RTI_ERROR_NOT_PERMITTED, RF4CE_ERROR_INVALID_INDEX,
 *              RF4CE_ERROR_INVALID_PARAMETER, RTI_ERROR_UNKNOWN_PARAMETER,
 *              RF4CE_ERROR_UNSUPPORTED_ATTRIBUTE, RTI_ERROR_OSAL_NV_OPER_FAILED,
 *              RTI_ERROR_OSAL_NV_ITEM_UNINIT, RTI_ERROR_OSAL_NV_BAD_ITEM_LEN
 *
 **************************************************************************************************/
DRV_Error DRV_RF4CE_TI_ReadItemEx( HUINT8 profileId, HUINT8 itemId, HUINT8 len, HUINT8 *pValue, HUINT32 *status)
{
	RF4CE_Msg_t pMsg;
	DRV_Error	eDrvError = DRV_OK;

	// prep Read Item request
	// Note: no need to send pValue over the NPI
	pMsg.subSys   = RPC_SYS_RCAF;
	pMsg.cmdId    = RTIS_CMD_ID_RTI_READ_ITEM_EX;
	pMsg.len      = 3;
	pMsg.pData[0] = profileId;
	pMsg.pData[1] = itemId;
	pMsg.pData[2] = len;

	// send Read Item request to NP RTIS synchronously
	eDrvError = P_RF4CE_SendSyncData( &pMsg );
	if(eDrvError != DRV_OK)
	{
		PrintError("[%s] SendSyncData Error\n", __FUNCTION__);
		return DRV_ERR;
	}

	// DEBUG
	if ( pMsg.pData[0] == RTI_ERROR_SYNCHRONOUS_NPI_TIMEOUT )
	{
		PrintError("[%s] RTI_ERROR_SYNCHRONOUS_NPI_TIMEOUT\n", __FUNCTION__);
		return DRV_ERR;
	}

	// copy the reply data to the client's buffer
	// Note: the first byte of the payload is reserved for the status
	VK_memcpy( pValue, &pMsg.pData[1], len );

	// perform endianness change
	P_RF4CE_EndianConversion( itemId, len, pValue );

	// return the status, which is stored is the first byte of the payload
	*status = ( (HUINT8)pMsg.pData[0] );

	return DRV_OK;
}

/**************************************************************************************************
 *
 * @fn          DRV_RF4CE_ReadItem
 *
 * @brief       This API is used to read the RTI Configuration Interface item
 *              from the Configuration Parameters table, the State Attributes
 *              table, or the Constants table.
 *
 * input parameters
 *
 * @param       itemId - The Configuration Interface item identifier.
 * @param       len - The length in bytes of the item identifier's data.
 *
 * output parameters
 *
 * @param       *pValue - Pointer to buffer where read data is placed.
 *
 * @return      RF4CE_SUCCESS, RTI_ERROR_NOT_PERMITTED, RF4CE_ERROR_INVALID_INDEX,
 *              RF4CE_ERROR_INVALID_PARAMETER, RTI_ERROR_UNKNOWN_PARAMETER,
 *              RF4CE_ERROR_UNSUPPORTED_ATTRIBUTE, RTI_ERROR_OSAL_NV_OPER_FAILED,
 *              RTI_ERROR_OSAL_NV_ITEM_UNINIT, RTI_ERROR_OSAL_NV_BAD_ITEM_LEN
 *
 **************************************************************************************************/
DRV_Error DRV_RF4CE_TI_ReadItem( HUINT8 itemId, HUINT8 len, HUINT8 *pValue, HUINT32 *status )
{
	RF4CE_Msg_t pMsg;
	DRV_Error	eDrvError = DRV_OK;

	// prep Read Item request
	// Note: no need to send pValue over the NPI
	pMsg.subSys   = RPC_SYS_RCAF;
	pMsg.cmdId    = RTIS_CMD_ID_RTI_READ_ITEM;
	pMsg.len      = 2;
	pMsg.pData[0] = itemId;
	pMsg.pData[1] = len;

	// send Read Item request to NP RTIS synchronously
	eDrvError = P_RF4CE_SendSyncData( &pMsg );
	if(eDrvError != DRV_OK)
	{
		PrintError("[%s] SendSyncData Error\n", __FUNCTION__);
		return DRV_ERR;
	}

	// DEBUG
	if ( pMsg.pData[0] == RTI_ERROR_SYNCHRONOUS_NPI_TIMEOUT )
	{
		PrintError("[%s] RTI_ERROR_SYNCHRONOUS_NPI_TIMEOUT\n", __FUNCTION__);
		return DRV_ERR;
	}

	// copy the reply data to the client's buffer
	// Note: the first byte of the payload is reserved for the status
	VK_memcpy( pValue, &pMsg.pData[1], len );

	// perform endianness change
	P_RF4CE_EndianConversion( itemId, len, pValue );

	// return the status, which is stored is the first byte of the payload
	*status = ( (HUINT8)pMsg.pData[0] );

	return DRV_OK;
}

/**************************************************************************************************
 *
 * @fn          DRV_RF4CE_WriteItemEx
 *
 * @brief       This API is used to write an item to a Profile's Configuration Interface.
 *
 * input parameters
 *
 * @param       profileId - The Profile identifier.
 * @param       itemId - The Configuration Interface item identifier.
 * @param       len - The length in bytes of the item identifier's data.
 * @param       *pValue - Pointer to buffer where write data is stored.
 *
 * input parameters
 *
 * None.
 *
 * @return      RF4CE_SUCCESS, RTI_ERROR_NOT_PERMITTED, RF4CE_ERROR_INVALID_INDEX,
 *              RF4CE_ERROR_INVALID_PARAMETER, RTI_ERROR_UNKNOWN_PARAMETER,
 *              RF4CE_ERROR_UNSUPPORTED_ATTRIBUTE, RTI_ERROR_OSAL_NV_OPER_FAILED,
 *              RTI_ERROR_OSAL_NV_ITEM_UNINIT, RTI_ERROR_OSAL_NV_BAD_ITEM_LEN
 *
 **************************************************************************************************/
DRV_Error DRV_RF4CE_TI_WriteItemEx( HUINT8 profileId, HUINT8 itemId, HUINT8 len, HUINT8 *pValue, HUINT32 *status )
{
	RF4CE_Msg_t pMsg;
	DRV_Error	eDrvError = DRV_OK;

	// prep Write Item request
	pMsg.subSys   = RPC_SYS_RCAF;
	pMsg.cmdId    = RTIS_CMD_ID_RTI_WRITE_ITEM_EX;
	pMsg.len      = 3+len;
	pMsg.pData[0] = profileId;
	pMsg.pData[1] = itemId;
	pMsg.pData[2] = len;

	// copy the client's data to be sent
	VK_memcpy( &pMsg.pData[3], pValue, len );

	// perform endianness change
	P_RF4CE_EndianConversion( itemId, len, &pMsg.pData[3] );

	// send Write Item request to NP RTIS synchronously
	eDrvError = P_RF4CE_SendSyncData( &pMsg );
	if(eDrvError != DRV_OK)
	{
		PrintError("[%s] SendSyncData Error\n", __FUNCTION__);
		return DRV_ERR;
	}

	// DEBUG
	if ( pMsg.pData[0] == RTI_ERROR_SYNCHRONOUS_NPI_TIMEOUT )
	{
		PrintError("[%s] RTI_ERROR_SYNCHRONOUS_NPI_TIMEOUT\n", __FUNCTION__);
		return DRV_ERR;
	}

	// return the status, which is stored is the first byte of the payload
	*status = ( (HUINT8)pMsg.pData[0] );

	return DRV_OK;
}

/**************************************************************************************************
 *
 * @fn          RTI_WriteItem
 *
 * @brief       This API is used to write RTI Configuration Interface parameters
 *              to the Configuration Parameters table, and permitted attributes
 *              to the State Attributes table.
 *
 * input parameters
 *
 * @param       itemId  - The Configuration Interface item identifier.
 * @param       len - The length in bytes of the item identifier's data.
 * @param       *pValue - Pointer to buffer where write data is stored.
 *
 * input parameters
 *
 * None.
 *
 * @return      RF4CE_SUCCESS, RTI_ERROR_NOT_PERMITTED, RF4CE_ERROR_INVALID_INDEX,
 *              RF4CE_ERROR_INVALID_PARAMETER, RTI_ERROR_UNKNOWN_PARAMETER,
 *              RF4CE_ERROR_UNSUPPORTED_ATTRIBUTE, RTI_ERROR_OSAL_NV_OPER_FAILED,
 *              RTI_ERROR_OSAL_NV_ITEM_UNINIT, RTI_ERROR_OSAL_NV_BAD_ITEM_LEN
 *
 **************************************************************************************************/
DRV_Error DRV_RF4CE_TI_WriteItem( HUINT8 itemId, HUINT8 len, HUINT8 *pValue, HUINT32 *status )
{
	RF4CE_Msg_t pMsg;
	DRV_Error	eDrvError = DRV_OK;

	// prep Write Item request
	pMsg.subSys   = RPC_SYS_RCAF;
	pMsg.cmdId    = RTIS_CMD_ID_RTI_WRITE_ITEM;
	pMsg.len      = 2+len;
	pMsg.pData[0] = itemId;
	pMsg.pData[1] = len;

	// copy the client's data to be sent
	VK_memcpy( &(pMsg.pData[2]), pValue, len );

	// perform endianness change
	P_RF4CE_EndianConversion( itemId, len, &(pMsg.pData[2]) );

	VK_TASK_Sleep(10);

	// send Write Item request to NP RTIS synchronously
	eDrvError = P_RF4CE_SendSyncData( &pMsg );
	if(eDrvError != DRV_OK)
	{
		PrintError("[%s] SendSyncData Error\n", __FUNCTION__);
		return DRV_ERR;
	}

	// DEBUG
	if ( pMsg.pData[0] == RTI_ERROR_SYNCHRONOUS_NPI_TIMEOUT )
	{
		PrintError("[%s] RTI_ERROR_SYNCHRONOUS_NPI_TIMEOUT\n", __FUNCTION__);
		return DRV_ERR;
	}
	/*
	// DEBUG, test if RNP not lock in boot mode
	if ( pMsg.subSys == RPC_SYS_BOOT )
	{
		return( 1 );
	}
	*/

	// return the status, which is stored is the first byte of the payload
	*status = ( (HUINT8)pMsg.pData[0] );

	return DRV_OK;
}

/*******************************************************************************
******************************** For SBL **************************************
*******************************************************************************/

DRV_Error DRV_RF4CE_REQ_EnterSBLReq(void)
{
	DRV_Error	eDrvError = DRV_ERR;
	RF4CE_Msg_t pMsg;

	// prep Ping request
	pMsg.subSys   = RPC_SYS_BOOT;
	pMsg.cmdId	  = SB_TGT_BOOTLOAD;
	pMsg.len	  = 0;

	PrintError("[%s] subSys : 0x%X\n", __FUNCTION__, pMsg.subSys);
	PrintError("[%s] cmdId : 0x%X\n", __FUNCTION__, pMsg.cmdId);
	PrintError("[%s] len : 0x%X\n", __FUNCTION__, pMsg.len);

	// send command to slave
	eDrvError = P_RF4CE_SendAsyncData(&pMsg);
	if(eDrvError != DRV_OK)
	{
		PrintError("[%s:%d] SendAsyncData Error\n", __FUNCTION__, __LINE__);
		return DRV_ERR;
	}

	return DRV_OK;
}

DRV_Error DRV_RF4CE_TI_SBL_HandShake(void)
{
	DRV_Error	eDrvError = DRV_ERR;
	RF4CE_Msg_t pMsg;

	// prep Ping request
	pMsg.subSys   = RPC_SYS_BOOT;
	pMsg.cmdId	  = SBL_HANDSHAKE_CMD;
	pMsg.len	  = 0;

	PrintError("[%s] subSys : 0x%X\n", __FUNCTION__, pMsg.subSys);
	PrintError("[%s] cmdId : 0x%X\n", __FUNCTION__, pMsg.cmdId);
	PrintError("[%s] len : 0x%X\n", __FUNCTION__, pMsg.len);

	// send command to slave
	eDrvError = P_RF4CE_SendAsyncData(&pMsg);
	if(eDrvError != DRV_OK)
	{
		PrintError("[%s:%d] P_RF4CE_SendAsyncData Error\n", __FUNCTION__, __LINE__);
		return DRV_ERR;
	}

	return DRV_OK;
}

static HUINT8 buf[64] = {0, };

DRV_Error DRV_RF4CE_TI_SBL_Write(void)
{
	DRV_Error	eDrvError = DRV_ERR;
	RF4CE_Msg_t pMsg;

	HUINT32 fd=0;
	HINT32 len = 0;

	HUINT32 i=0;
	HUINT32 ulAddrToWrite=0x0000;
	HUINT32 ulWrittenBytes = 0;

	fd = open("/new_version.bin", O_RDONLY);
	if(fd < 0)
	{
		PrintError("fd open Error\n");
		return DRV_ERR;
	}

	while(1)
	{
		len = read(fd, buf, 64);
		if(len < 0)
		{
			PrintError("[%s:%d] len is not valid\n", __FUNCTION__, __LINE__);
			break;
		}
		if(len == 0)
		{
			PrintError("[%s:%d] len is 0\n", __FUNCTION__, __LINE__);
			break;
		}
		PrintInfo("len : 0x%X\n", len);

		for(i=0; i<len; i++)
		{
			if((i % 16) == 0)
			{
				PrintInfo("\n");
			}
			PrintInfo("%X ", buf[i]);
		}
		PrintInfo("\n");

		// prep Ping request
		pMsg.subSys   = RPC_SYS_BOOT;
		pMsg.cmdId	  = SBL_WRITE_CMD;
		pMsg.len	  = len + 2;

		VK_printf("ulAddrToWrite = 0x%04X\n", ulAddrToWrite);

		pMsg.pData[0] = ulAddrToWrite & 0xFF;
		pMsg.pData[1] = (ulAddrToWrite >> 8) & 0xFF;

		VK_memcpy(&(pMsg.pData[2]), buf, len);

		PrintError("[%s] subSys : 0x%X\n", __FUNCTION__, pMsg.subSys);
		PrintError("[%s] cmdId : 0x%X\n", __FUNCTION__, pMsg.cmdId);
		PrintError("[%s] len : 0x%X\n", __FUNCTION__, pMsg.len);

		// send command to slave
		eDrvError = P_RF4CE_SendAsyncData(&pMsg);
		if(eDrvError != DRV_OK)
		{
			PrintError("[%s:%d] P_RF4CE_SendAsyncData Error\n", __FUNCTION__, __LINE__);
			return DRV_ERR;
		}
		ulAddrToWrite += (len / 4);	/* We Should Send Word Aligned Address	*/
		ulWrittenBytes += len;

		PrintInfo("Written Bytes = 0x%d\n", ulWrittenBytes);

		VK_TASK_Sleep(100);
	}

	close(fd);

	return DRV_OK;
}

DRV_Error DRV_RF4CE_TI_SBL_Read(void)
{
	DRV_Error	eDrvError = DRV_ERR;
	RF4CE_Msg_t pMsg;

	// prep Ping request
	pMsg.subSys   = RPC_SYS_BOOT;
	pMsg.cmdId	  = SBL_READ_CMD;
	pMsg.len	  = 0;

	PrintError("[%s] subSys : 0x%X\n", __FUNCTION__, pMsg.subSys);
	PrintError("[%s] cmdId : 0x%X\n", __FUNCTION__, pMsg.cmdId);
	PrintError("[%s] len : 0x%X\n", __FUNCTION__, pMsg.len);

	// send command to slave
	eDrvError = P_RF4CE_SendAsyncData(&pMsg);
	if(eDrvError != DRV_OK)
	{
		PrintError("[%s:%d] P_RF4CE_SendAsyncData Error\n", __FUNCTION__, __LINE__);
		return DRV_ERR;
	}

	return DRV_OK;
}

DRV_Error DRV_RF4CE_TI_SBL_Enable(void)
{
	DRV_Error	eDrvError = DRV_ERR;
	RF4CE_Msg_t pMsg;

	// prep Ping request
	pMsg.subSys   = RPC_SYS_BOOT;
	pMsg.cmdId	  = SBL_ENABLE_CMD;
	pMsg.len	  = 0;

	PrintError("[%s] subSys : 0x%X\n", __FUNCTION__, pMsg.subSys);
	PrintError("[%s] cmdId : 0x%X\n", __FUNCTION__, pMsg.cmdId);
	PrintError("[%s] len : 0x%X\n", __FUNCTION__, pMsg.len);

	// send command to slave
	eDrvError = P_RF4CE_SendAsyncData(&pMsg);
	if(eDrvError != DRV_OK)
	{
		PrintError("[%s:%d] P_RF4CE_SendAsyncData Error\n", __FUNCTION__, __LINE__);
		return DRV_ERR;
	}

	return DRV_OK;
}

/*******************************************************************************
******************************** Requests **************************************
*******************************************************************************/

DRV_Error DRV_RF4CE_REQ_PingReq(void)
{
	DRV_Error	eDrvError = DRV_ERR;
	RF4CE_Msg_t pMsg;

	// prep Ping request
	pMsg.subSys   = RPC_SYS_RCAF;
	pMsg.cmdId	  = RTIS_CMD_ID_TEST_PING_REQ;
	pMsg.len	  = 2;
	pMsg.pData[0] = 0xAA;
	pMsg.pData[1] = 0xCC;

	// send command to slave
	eDrvError = P_RF4CE_SendAsyncData(&pMsg);
	if(eDrvError != DRV_OK)
	{
		PrintError("[InitReq] SendAsyncData Error\n");
		return DRV_ERR;
	}

	return DRV_OK;
}

/**************************************************************************************************
 *
 * @fn          DRV_RF4CE_REQ_InitReq
 *
 * @brief       This API is used to initialize the RemoTI stack and begin
 *              network operation. A RemoTI confirmation callback is generated
 *              and handled by the client.
 *
 *              The first thing this function does is take a snapshot of the
 *              Configuration Parameters (CP) table stored in NV memory, and
 *              only the snapshot will be used by RTI until another call is made
 *              to this function (presumably due to a reset). Therefore, any
 *              changes to the CP table must be made prior to calling this
 *              function. Once the RTI is started, subsequent changes by the
 *              client to the CP table can be made, but they will have no affect
 *              on RTI operation. The CP table is stored in NV memory and will
 *              persist across a device reset. The client can restore the
 *              the CP table to its default settings by setting the Startup
 *              Option parameter accordingly.
 *
 *              The client's confirm callback will provide a status, which can
 *              be one of the following:
 *
 *              RF4CE_SUCCESS
 *              RTI_ERROR_INVALID_PARAMTER
 *              RF4CE_ERROR_UNSUPPORTED_ATTRIBUTE
 *              RF4CE_ERROR_INVALID_INDEX
 *
 * input parameters
 *
 * None.
 *
 * output parameters
 *
 * None.
 *
 * @return      None.
 *
 **************************************************************************************************/
DRV_Error DRV_RF4CE_REQ_InitReq(void)
{
	DRV_Error	eDrvError = DRV_ERR;
	RF4CE_Msg_t pMsg;

	// prep Init request
	pMsg.len	= 0;
	pMsg.subSys = RPC_SYS_RCAF;	//10 = 0x0a
	pMsg.cmdId	= RTIS_CMD_ID_RTI_INIT_REQ;

	s_ulSendSyncDataInterval = SHORT_INTERVAL;

	eDrvError = P_RF4CE_SendAsyncData(&pMsg);
	if(eDrvError != DRV_OK)
	{
		PrintError("[InitReq] SendAsyncData Error\n");
		return DRV_ERR;
	}

	return DRV_OK;
}

/**************************************************************************************************
 *
 * @fn          DRV_RF4CE_TI_REQ_AllowPairReq
 *
 * @brief       This function is used by the Target application to ready the
 *              node for a pairing request, and thereby allow this node to
 *              respond.
 *
 *              The client's confirm callback will provide a status, which can
 *              be one of the following:
 *
 *              RF4CE_SUCCESS
 *              RTI_ERROR_OSAL_NO_TIMER_AVAIL
 *              RF4CE_ERROR_ALLOW_PAIRING_TIMEOUT
 *
 * input parameters
 *
 * None.
 *
 * output parameters
 *
 * None.
 *
 * @return      None.
 *
 **************************************************************************************************/
DRV_Error DRV_RF4CE_TI_REQ_AllowPairReq( void )
{
	DRV_Error	eDrvError = DRV_ERR;
	RF4CE_Msg_t pMsg;

	// prep Pair request
	pMsg.subSys = RPC_SYS_RCAF;
	pMsg.cmdId  = RTIS_CMD_ID_RTI_ALLOW_PAIR_REQ;
	pMsg.len    = 0;

	// send Pair request to NP RTIS asynchronously as a confirm is due back
	eDrvError = P_RF4CE_SendAsyncData(&pMsg);
	if(eDrvError != DRV_OK)
	{
		PrintError("[AllowPairReq] SendData Error\n");
		return DRV_ERR;
	}

	return DRV_OK;
}

/**************************************************************************************************
 *
 * @fn          DRV_RF4CE_REQ_AllowPairAbortReq
 *
 * @brief       This API is used to attempt to abort an on-going allow-pairing process.
 *
 *              It is possible that allow pair is at a state of no return (no aborting).
 *              There is no callback associated to this function call.
 *
 * input parameters
 *
 * None.
 *
 * output parameters
 *
 * None.
 *
 * @return      None.
 *
 **************************************************************************************************/
DRV_Error DRV_RF4CE_REQ_AllowPairAbortReq( void )
{
	DRV_Error	eDrvError = DRV_ERR;
	RF4CE_Msg_t pMsg;

	// prep Pair request
	pMsg.subSys = RPC_SYS_RCAF;
	pMsg.cmdId  = RTIS_CMD_ID_RTI_ALLOW_PAIR_ABORT_REQ;
	pMsg.len    = 0;

	// send Pair request to NP RTIS asynchronously as a confirm is due back
	eDrvError = P_RF4CE_SendAsyncData(&pMsg);
	if(eDrvError != DRV_OK)
	{
		PrintError("[AllowPairAbortReq] SendData Error\n");
		return DRV_ERR;
	}

	return DRV_OK;
}

/**************************************************************************************************
 *
 * @fn          DRV_RF4CE_REQ_UnpairReq
 *
 * @brief       This API is used to trigger un-pairing of a pair entry
 *
 * input parameters
 *
 * @param      dstIndex - destination index
 *
 * output parameters
 *
 * None.
 *
 * @return      None.
 *
 **************************************************************************************************/
DRV_Error DRV_RF4CE_TI_REQ_UnpairReq( HUINT8 dstIndex )
{
	DRV_Error	eDrvError = DRV_ERR;
	RF4CE_Msg_t pMsg;

	// prep Pair request
	pMsg.subSys = RPC_SYS_RCAF;
	pMsg.cmdId  = RTIS_CMD_ID_RTI_UNPAIR_REQ;
	pMsg.len    = 1;
	pMsg.pData[0] = dstIndex;

	// send Pair request to NP RTIS asynchronously as a confirm is due back
	eDrvError = P_RF4CE_SendAsyncData(&pMsg);
	if(eDrvError != DRV_OK)
	{
		PrintError("[UnpairReq] SendData Error\n");
		return DRV_ERR;
	}

	return DRV_OK;
}

/**************************************************************************************************
 *
 * @fn          DRV_RF4CE_TI_REQ_TestModeReq
 *
 * @brief       This function is used to place the radio in test modes.
 *              Note that implementation is chip dependent. HAL is not used to reduce code
 *              size overhead.
 *
 * input parameters
 *
 * None.
 *
 * output parameters
 *
 * None.
 *
 * @return      None.
 *
 **************************************************************************************************/
DRV_Error DRV_RF4CE_TI_REQ_TestModeReq()
{
	DRV_Error	eDrvError = DRV_ERR;
	RF4CE_Msg_t pMsg;

	pMsg.subSys   = RPC_SYS_RCAF;
	pMsg.cmdId    = RTIS_CMD_ID_RTI_TEST_MODE_REQ;
	pMsg.len      = 3;

	pMsg.pData[0] = s_Mode;
	pMsg.pData[1] = s_TxPower;
	pMsg.pData[2] = s_Channel;

	PrintInfo("[%s:%d] mode : 0x%X\n", __FUNCTION__, __LINE__, pMsg.pData[0]);
	PrintInfo("[%s:%d] txPower : 0x%X\n", __FUNCTION__, __LINE__, pMsg.pData[1]);
	PrintInfo("[%s:%d] channel : 0x%X\n", __FUNCTION__, __LINE__, pMsg.pData[2]);

	// send Pair request to NP RTIS asynchronously as a confirm is due back
	eDrvError = P_RF4CE_SendAsyncData(&pMsg);
	if(eDrvError != DRV_OK)
	{
		PrintError("[%s] SendData Error\n", __FUNCTION__);
		return DRV_ERR;
	}

	return DRV_OK;
}

DRV_Error DRV_RF4CE_TI_SetChannel(HUINT32 channel)
{
	s_Channel = channel;
	PrintError("[%s:%d] s_Channel : 0x%X\n", __FUNCTION__, __LINE__, s_Channel);

	return DRV_OK;
}

DRV_Error DRV_RF4CE_TI_SetMode(HUINT32 mode)
{
	s_Mode = mode;
	PrintError("[%s:%d] s_Mode : 0x%X\n", __FUNCTION__, __LINE__, s_Mode);

	return DRV_OK;
}

DRV_Error DRV_RF4CE_TI_SetTxPower(HUINT32 txPower)
{
	s_TxPower = txPower;
	PrintError("[%s:%d] s_TxPower : 0x%X\n", __FUNCTION__, __LINE__, s_TxPower);

	return DRV_OK;
}

/*******************************************************************************
******************************** Confirms **************************************
*******************************************************************************/

/**************************************************************************************************
 *
 * @fn      DRV_RF4CE_CNF_InitCnf
 *
 * @brief   RTI confirmation callback initiated by client's RTI_InitReq API
 *          call. The client is expected to complete this function.
 *
 *          NOTE: It is possible that this call can be made to the RTI client
 *                before the call to RTI_InitReq has returned.
 *
 * @param   status - Result of RTI_InitReq API call.
 *
 * @return  DRV_Error
 */
DRV_Error DRV_RF4CE_TI_CNF_InitCnf(HUINT8 result)
{
	HUINT8 startupFlg;
	HUINT32	status = 0;
	DRV_Error	eDrvError	= DRV_OK;

#if defined(CONFIG_FACTORY)
	DI_RF4CE_CALLBACK_DATA_t data;
#endif

	if ( result != RF4CE_SUCCESS )
	{
		PrintError("ERROR: DRV_RF4CE_CNF_InitCnf called with status %u\n", (unsigned) result);

#if defined(CONFIG_FACTORY)
		if(pfnRf4ceEventCallback == NULL)
		{
			PrintError("[%s] EventCallback == NULL\n", __FUNCTION__);
		}
		else
		{
			data.eEvent = DI_RF4CE_NOTIFY_EVT_INIT_FAILED;
			data.pvEventData = NULL;

			PrintError("[%s] EventCallback != NULL\n", __FUNCTION__);
			pfnRf4ceEventCallback(&data);
		}
#endif
		return DRV_ERR;
	}

	startupFlg = RESTORE_STATE;
	eDrvError = DRV_RF4CE_TI_WriteItem(RF4CE_CP_ITEM_STARTUP_CTRL, 1, &startupFlg, &status );
	if(eDrvError != DRV_OK)
	{
		PrintError("[%s] WriteItem Error\n", __FUNCTION__);

#if defined(CONFIG_FACTORY)
		if(pfnRf4ceEventCallback == NULL)
		{
			PrintError("[%s] EventCallback == NULL\n", __FUNCTION__);
		}
		else
		{
			data.eEvent = DI_RF4CE_NOTIFY_EVT_INIT_FAILED;
			data.pvEventData = NULL;

			PrintError("[%s] EventCallback != NULL\n", __FUNCTION__);
			pfnRf4ceEventCallback(&data);
		}
#endif

		return DRV_ERR;
	}

	if(status != RF4CE_SUCCESS)
	{
		PrintError("[%s] WriteItem Failed... status = 0x%x\n", __FUNCTION__, status);

#if defined(CONFIG_FACTORY)
		if(pfnRf4ceEventCallback == NULL)
		{
			PrintError("[%s] EventCallback == NULL\n", __FUNCTION__);
		}
		else
		{
			data.eEvent = DI_RF4CE_NOTIFY_EVT_INIT_FAILED;
			data.pvEventData = NULL;

			PrintError("[%s] EventCallback != NULL\n", __FUNCTION__);
			pfnRf4ceEventCallback(&data);
		}
#endif

		return DRV_ERR;
	}

	s_ulSendSyncDataInterval = SHORT_INTERVAL;

	PrintInfo("[%s] InitReq SUCCESS: DRV_RF4CE_CNF_InitCnf called with status %u\n", __FUNCTION__, (unsigned) status);
	s_bInitialized = TRUE;

#if defined(CONFIG_FACTORY)
	if(pfnRf4ceEventCallback == NULL)
	{
		PrintError("[%s] EventCallback == NULL\n", __FUNCTION__);
	}
	else
	{
		data.eEvent = DI_RF4CE_NOTIFY_EVT_INIT_SUCCESS;
		data.pvEventData = NULL;

		PrintError("[%s] EventCallback != NULL\n", __FUNCTION__);
		pfnRf4ceEventCallback(&data);
	}
#endif

	return DRV_OK;
}

/**************************************************************************************************
 *
 * @fn      RTI_AllowPairCnf
 *
 * @brief   RTI confirmation callback initiated by client's RTI_AllowPairReq API
 *          call. The client is expected to complete this function.
 *
 *          NOTE: It is possible that this call can be made to the RTI client
 *                before the call to RTI_AllowPairReq has returned.
 * @param   status   - Result of RTI_PairReq API call.
 * @param   dstIndex - Pairing table entry of paired device, or invalid
 * @param   devType  - Pairing table index device type, or invalid
 *
 * @return  void
 */
DRV_Error DRV_RF4CE_TI_CNF_AllowPairCnf( HUINT8 status, HUINT8 dstIndex, HUINT8 devType )
{
	// set paring reference (destination index)
	s_ucDestIdx = dstIndex;
	s_ucDevType = devType;

	s_stTargetInfo.dstIndex = dstIndex;
	s_stTargetInfo.devType = devType;

	DI_RF4CE_CALLBACK_DATA_t data;

	PrintDebug("RTI_AllowpairCnf called with status 0x%.2X\n", (unsigned) status);
	if (status == 0)
	{
		PrintInfo("[%s] Paired !!!\n", __FUNCTION__);
		PrintInfo("[%s] dstIndex : 0x%x\n", __FUNCTION__, dstIndex);
		PrintInfo("[%s] devType : 0x%x\n", __FUNCTION__, devType);

		data.eEvent = DI_RF4CE_NOTIFY_EVT_PAIRING_SUCCESS;
		data.pvEventData = NULL;

		if(g_tRF4CEInstance.tCallbackInfo[DI_RF4CE_NOTIFY_EVT_PAIRING_SUCCESS].pfnCallback == NULL)
		{
			PrintError("[%s] EventCallback == NULL\n", __FUNCTION__);
		}
		else
		{
			PrintError("[%s] EventCallback != NULL\n", __FUNCTION__);
			g_tRF4CEInstance.tCallbackInfo[DI_RF4CE_NOTIFY_EVT_PAIRING_SUCCESS].pfnCallback(&data);
		}
	}
	else
	{
		PrintError("[%s] Failed... status : 0x%x\n", __FUNCTION__, status);

		data.eEvent = DI_RF4CE_NOTIFY_EVT_PAIRING_FAILED;
		data.pvEventData = NULL;

		if(g_tRF4CEInstance.tCallbackInfo[DI_RF4CE_NOTIFY_EVT_PAIRING_SUCCESS].pfnCallback == NULL)
		{
			PrintError("[%s] EventCallback == NULL\n", __FUNCTION__);
		}
		else
		{
			PrintError("[%s] EventCallback != NULL\n", __FUNCTION__);
			g_tRF4CEInstance.tCallbackInfo[DI_RF4CE_NOTIFY_EVT_PAIRING_SUCCESS].pfnCallback(&data);
		}
	}

	s_ulSendSyncDataInterval = SHORT_INTERVAL;

	return DRV_OK;
}

/**************************************************************************************************
 *
 * @fn      RTI_UnpairCnf
 *
 * @brief   RTI confirmation callback initiated by client's RTI_UnpairReq API
 *          call. The client is expected to complete this function.
 *
 *          NOTE: It is possible that this call can be made to the RTI client
 *                before the call to RTI_UnpairReq has returned.
 *
 * @param   status   - Result of RTI_PairReq API call.
 * @param   dstIndex - Pairing table index of paired device, or invalid.
 *
 * @return  void
 */
DRV_Error DRV_RF4CE_CNF_UnpairCnf( HUINT8 status, HUINT8 dstIndex )
{
	// This function is introduced in RemoTI 1.1
	status = status;
	dstIndex = dstIndex;

	return DRV_OK;
}

/*******************************************************************************
******************************** Indications ***********************************
*******************************************************************************/

/**************************************************************************************************
 *
 * @fn      DRV_RF4CE_TI_IND__ReceiveDataInd
 *
 * @brief   RTI receive data indication callback asynchronously initiated by
 *          another node. The client is expected to complete this function.
 *
 * input parameters
 *
 * @param   srcIndex:  Pairing table index.
 * @param   profileId: Profile identifier.
 * @param   vendorId:  Vendor identifier.
 * @param   rxLQI:     Link Quality Indication.
 * @param   rxFlags:   Receive flags.
 * @param   len:       Number of bytes to send.
 * @param   *pData:    Pointer to data to be sent.
 *
 * output parameters
 *
 * None.
 *
 * @return      DRV_Error.
 */
DRV_Error DRV_RF4CE_TI_IND_ReceiveDataInd( HUINT8 srcIndex, HUINT8 profileId, HUINT16 vendorId, HUINT8 rxLQI, HUINT8 rxFlags, HUINT8 len, HUINT8 *pData )
{
	int i = FALSE;

	DI_KEY_TYPE eKeyType = DI_KEY_TYPE_MAX;

	PrintInfo("*************************************************\n");
	PrintInfo("DRV_RF4CE_TI_IND_ReceiveDataInd @ \n");
	PrintInfo("Source Idx: 0x%x, profileId 0x%x , vendorId: 0x%4x , rxLQI 0x%x , rxFlags 0x%x , len 0x%x\n",
			srcIndex,
			profileId,
			vendorId,
			rxLQI,
			rxFlags,
			len);

	if(profileId == 0x01)	// ZRC Profile
	{
		PrintInfo("Profile : ZRC\n");
		PrintInfo("KEY TYPE : %x\n", pData[0]);
		PrintInfo("KEY VALUE(1) : %x\n", pData[1]);
		PrintInfo("KEY VALUE(2) : %x\n", pData[2]);

		if(pfnRf4ceKeyCallback == NULL)
		{
			PrintError("[%s] Callback is NULL\n", __FUNCTION__);
		}
		else
		{
			pfnRf4ceKeyCallback(pData[0], pData[1]);
		}
	}
	else if(profileId == 0x02)	// ZID Profile
	{
		PrintInfo("Profile : ZID\n");
		PrintInfo("Raw Data : ");
		for(i=0; i<len; i++)
			PrintInfo(" %x ", pData[i]);
		PrintInfo("\n");
	}
	PrintInfo("*************************************************\n");

	return DRV_OK;
}

/**************************************************************************************************
 *
 * @fn      RTI_UnpairInd
 *
 * @brief   RTI indication callback initiated by receiving unpair request command.
 *
 * @param   dstIndex - Pairing table index of paired device.
 *
 * @return  void
 */
DRV_Error DRV_RF4CE_IND_UnpairInd( HUINT8 dstIndex )
{
	// This function is introduced in RemoTI 1.
	PrintInfo("[%s] dstIndex : 0x%x\n", __FUNCTION__, dstIndex);
	return DRV_OK;
}

/*******************************************************************************
******************************** Certifications ********************************
*******************************************************************************/

DRV_Error DRV_RF4CE_CE_SetTxPowerReq(HUINT32 ulTxPower)
{
	DRV_Error	eDrvError = DRV_ERR;
	RF4CE_Msg_t pMsg;

	// prep Pair request
	pMsg.subSys = RPC_SYS_RCAF;
	pMsg.cmdId  = RTIS_HUMAX_CMD_ID_RTI_SET_TX_POWER_REQ;
	pMsg.len    = 1;

	pMsg.pData[0] = ulTxPower;

	// send Pair request to NP RTIS asynchronously as a confirm is due back
	eDrvError = P_RF4CE_SendAsyncData(&pMsg);
	if(eDrvError != DRV_OK)
	{
		PrintError("[%s] SendData Error\n", __FUNCTION__);
		return DRV_ERR;
	}

	return DRV_OK;
}

DRV_Error DRV_RF4CE_CE_SetChannelReq(HUINT32 ulChannel)
{
	DRV_Error	eDrvError = DRV_ERR;
	RF4CE_Msg_t pMsg;

	// prep Pair request
	pMsg.subSys = RPC_SYS_RCAF;
	pMsg.cmdId  = RTIS_HUMAX_CMD_ID_RTI_SET_CHANNEL_REQ;
	pMsg.len    = 1;

	pMsg.pData[0] = ulChannel;

	// send Pair request to NP RTIS asynchronously as a confirm is due back
	eDrvError = P_RF4CE_SendAsyncData(&pMsg);
	if(eDrvError != DRV_OK)
	{
		PrintError("[%s] SendData Error\n", __FUNCTION__);
		return DRV_ERR;
	}

	return DRV_OK;
}

DRV_Error DRV_RF4CE_CE_SetTxModeReq(RF4CE_Tx_Mode_e e_TxMode)
{
	DRV_Error	eDrvError = DRV_ERR;
	RF4CE_Msg_t pMsg;

	// prep Pair request
	pMsg.subSys = RPC_SYS_RCAF;
	pMsg.cmdId  = RTIS_HUMAX_CMD_ID_RTI_SET_TX_MODE_REQ;
	pMsg.len    = 1;

	PrintInfo("[%s:%d] pMsg.subSys : 0x%X\n", __FUNCTION__, __LINE__, pMsg.subSys);
	PrintInfo("[%s:%d] pMsg.cmdId : 0x%X\n", __FUNCTION__, __LINE__, pMsg.subSys);
	PrintInfo("[%s:%d] pMsg.len : 0x%X\n", __FUNCTION__, __LINE__, pMsg.subSys);

	switch(e_TxMode)
	{
	case RF4CE_TX_MODE_UNMODULATED :
		PrintInfo("[%s:%d] Tx Mode : UnModulated\n", __FUNCTION__, __LINE__);
		break;
	case RF4CE_TX_MODE_MODULATED :
		PrintInfo("[%s:%d] Tx Mode : Modulated\n", __FUNCTION__, __LINE__);
		break;
	default :
		PrintInfo("[%s:%d] DEFAULT, value : 0x%X\n", __FUNCTION__, __LINE__, e_TxMode);
		break;
	}
	pMsg.pData[0] = e_TxMode;

	// send Pair request to NP RTIS asynchronously as a confirm is due back
	eDrvError = P_RF4CE_SendAsyncData(&pMsg);
	if(eDrvError != DRV_OK)
	{
		PrintError("[%s] SendData Error\n", __FUNCTION__);
		return DRV_ERR;
	}

	return DRV_OK;
}

DRV_Error DRV_RF4CE_CE_SetRxModeReq(RF4CE_Rx_Mode_e e_RxMode)
{
	DRV_Error	eDrvError = DRV_ERR;
	RF4CE_Msg_t pMsg;

	// prep Pair request
	pMsg.subSys = RPC_SYS_RCAF;
	pMsg.cmdId  = RTIS_HUMAX_CMD_ID_RTI_SET_RX_MODE_REQ;
	pMsg.len    = 1;

	switch(e_RxMode)
	{
	case RF4CE_RX_MODE_UNMODULATED :
		PrintInfo("[%s:%d] Rx Mode : UnModulated\n", __FUNCTION__, __LINE__);
		break;
	case RF4CE_RX_MODE_MODULATED :
		PrintInfo("[%s:%d] Rx Mode : Modulated\n", __FUNCTION__, __LINE__);
		break;
	default :
		PrintInfo("[%s:%d] DEFAULT, value : 0x%X\n", __FUNCTION__, __LINE__, e_RxMode);
		break;
	}
	pMsg.pData[0] = e_RxMode;

	// send Pair request to NP RTIS asynchronously as a confirm is due back
	eDrvError = P_RF4CE_SendAsyncData(&pMsg);
	if(eDrvError != DRV_OK)
	{
		PrintError("[%s] SendData Error\n", __FUNCTION__);
		return DRV_ERR;
	}

	return DRV_OK;
}

DRV_Error DRV_RF4CE_CE_SetRxEnableReq(RF4CE_RxEnable_e e_RxEnable)
{
	DRV_Error	eDrvError = DRV_ERR;
	RF4CE_Msg_t pMsg;

	// prep Pair request
	pMsg.subSys = RPC_SYS_RCAF;
	pMsg.cmdId  = RTIS_HUMAX_CMD_ID_RTI_SET_RX_ENABLE_REQ;
	pMsg.len    = 1;

	switch(e_RxEnable)
	{
	case RF4CE_RX_ENABLE_STXON :
		PrintInfo("[%s:%d] RxEnable : STXON\n", __FUNCTION__, __LINE__);
		break;
	case RF4CE_RX_ENABLE_SRXON :
		PrintInfo("[%s:%d] RxEnable : SRXON\n", __FUNCTION__, __LINE__);
		break;
	case RF4CE_RX_ENABLE_SRFOFF :
		PrintInfo("[%s:%d] RxEnable : SRFOFF\n", __FUNCTION__, __LINE__);
		break;
	case RF4CE_RX_ENABLE_SRXMASKBITSET :
		PrintInfo("[%s:%d] RxEnable : SRXMASKBITSET\n", __FUNCTION__, __LINE__);
		break;
	case RF4CE_RX_ENABLE_SRXMASKBITCLR :
		PrintInfo("[%s:%d] RxEnable : SRXMASKBITCLR\n", __FUNCTION__, __LINE__);
		break;
	default :
		PrintInfo("[%s:%d] DEFAULT, value : 0x%X\n", __FUNCTION__, __LINE__, e_RxEnable);
		break;
	}
	pMsg.pData[0] = e_RxEnable;

	// send Pair request to NP RTIS asynchronously as a confirm is due back
	eDrvError = P_RF4CE_SendAsyncData(&pMsg);
	if(eDrvError != DRV_OK)
	{
		PrintError("[%s] SendData Error\n", __FUNCTION__);
		return DRV_ERR;
	}

	return DRV_OK;
}

DRV_Error DRV_RF4CE_CE_SetTestModeReq(RF4CE_TestMode_e e_TestMode)
{
	DRV_Error	eDrvError = DRV_ERR;
	RF4CE_Msg_t pMsg;

	// prep Pair request
	pMsg.subSys = RPC_SYS_RCAF;
	pMsg.cmdId  = RTIS_HUMAX_CMD_ID_RTI_SET_TEST_MODE_REQ;
	pMsg.len    = 1;

	switch(e_TestMode)
	{
	case RF4CE_TEST_MODE_TX_UNMODULATED :
		PrintInfo("[%s:%d] Tx Mode : UnModulated\n", __FUNCTION__, __LINE__);
		break;
	case RF4CE_TEST_MODE_TX_MODULATED :
		PrintInfo("[%s:%d] Tx Mode : Modulated\n", __FUNCTION__, __LINE__);
		break;
	case RF4CE_TEST_MODE_RX_UNMODULATED :
		PrintInfo("[%s:%d] Rx Mode : Unmodulated\n", __FUNCTION__, __LINE__);
		break;
	case RF4CE_TEST_MODE_RX_MODULATED :
		PrintInfo("[%s:%d] Rx Mode : Modulated\n", __FUNCTION__, __LINE__);
		break;
	case RF4CE_TEST_MODE_PACKET_TX :
		PrintInfo("[%s:%d] Tx Mode : Packet\n", __FUNCTION__, __LINE__);
		break;
	case RF4CE_TEST_MODE_PACKET_RX :
		PrintInfo("[%s:%d] Rx Mode : Packet\n", __FUNCTION__, __LINE__);
		break;
	default :
		PrintInfo("[%s:%d] DEFAULT, value : 0x%X\n", __FUNCTION__, __LINE__, e_TestMode);
		break;
	}
	pMsg.pData[0] = e_TestMode;

	// send Pair request to NP RTIS asynchronously as a confirm is due back
	eDrvError = P_RF4CE_SendAsyncData(&pMsg);
	if(eDrvError != DRV_OK)
	{
		PrintError("[%s] SendData Error\n", __FUNCTION__);
		return DRV_ERR;
	}

	return DRV_OK;
}

DRV_Error DRV_RF4CE_CE_GetContextReq()
{
	DRV_Error	eDrvError = DRV_ERR;
	RF4CE_Msg_t pMsg;

	// prep Pair request
	pMsg.subSys = RPC_SYS_RCAF;
	pMsg.cmdId  = RTIS_HUMAX_CMD_ID_RTI_GET_CONTEXT_REQ;
	pMsg.len    = 0;

	// send Pair request to NP RTIS asynchronously as a confirm is due back
	eDrvError = P_RF4CE_SendAsyncData(&pMsg);
	if(eDrvError != DRV_OK)
	{
		PrintError("[%s] SendData Error\n", __FUNCTION__);
		return DRV_ERR;
	}

	return DRV_OK;
}

DRV_Error DRV_RF4CE_CE_GetRssiReq( void )
{
	DRV_Error	eDrvError = DRV_ERR;
	RF4CE_Msg_t pMsg;

	// prep Pair request
	pMsg.subSys = RPC_SYS_RCAF;
	pMsg.cmdId  = RTIS_HUMAX_CMD_ID_RTI_GET_RSSI_REQ;
	pMsg.len    = 0;

	// send Pair request to NP RTIS asynchronously as a confirm is due back
	eDrvError = P_RF4CE_SendAsyncData(&pMsg);
	if(eDrvError != DRV_OK)
	{
		PrintError("[%s] SendData Error\n", __FUNCTION__);
		return DRV_ERR;
	}

	return DRV_OK;
}

#if 0 /* Not Implemented */
DRV_Error DRV_RF4CE_CE_SetTxFIFOReq()
{
	DRV_Error	eDrvError = DRV_ERR;
	RF4CE_Msg_t pMsg;

	// prep Pair request
	pMsg.subSys = RPC_SYS_RCAF;
	pMsg.cmdId  = RTIS_HUMAX_CMD_ID_RTI_SET_TX_FIFO_REQ;
	pMsg.len    = 6;

	pMsg.pData[0] = 'A';
	pMsg.pData[1] = 'B';
	pMsg.pData[2] = 'C';
	pMsg.pData[3] = 'D';
	pMsg.pData[4] = 'E';
	pMsg.pData[5] = 'F';

	// send Pair request to NP RTIS asynchronously as a confirm is due back
	eDrvError = P_RF4CE_SendAsyncData(&pMsg);
	if(eDrvError != DRV_OK)
	{
		PrintError("[%s] SendData Error\n", __FUNCTION__);
		return DRV_ERR;
	}

	return DRV_OK;
}

DRV_Error DRV_RF4CE_CE_GetRxFIFOReq()
{
	DRV_Error	eDrvError = DRV_ERR;
	RF4CE_Msg_t pMsg;

	// prep Pair request
	pMsg.subSys = RPC_SYS_RCAF;
	pMsg.cmdId  = RTIS_HUMAX_CMD_ID_RTI_GET_RX_FIFO_REQ;
	pMsg.len    = 0;

	// send Pair request to NP RTIS asynchronously as a confirm is due back
	eDrvError = P_RF4CE_SendAsyncData(&pMsg);
	if(eDrvError != DRV_OK)
	{
		PrintError("[%s] SendData Error\n", __FUNCTION__);
		return DRV_ERR;
	}

	return DRV_OK;
}
#endif

DRV_Error DRV_RF4CE_CE_SetTxPowerCnf(RF4CE_Msg_t* pMsg)
{
	HUINT32 i=0;

	for(i=0; i<pMsg->len; i++)
	{
		PrintInfo("[%s:%d] pMsg->pData[%d] = 0x%X\n", __FUNCTION__, __LINE__, i, pMsg->pData[i]);
	}

	return DRV_OK;
}

DRV_Error DRV_RF4CE_CE_SetChannelCnf(RF4CE_Msg_t* pMsg)
{
	HUINT32 i=0;

	for(i=0; i<pMsg->len; i++)
	{
		PrintInfo("[%s:%d] pMsg->pData[%d] = 0x%X\n", __FUNCTION__, __LINE__, i, pMsg->pData[i]);
	}

	return DRV_OK;
}

DRV_Error DRV_RF4CE_CE_SetTxModeCnf(RF4CE_Msg_t* pMsg)
{
	HUINT32 i=0;

	for(i=0; i<pMsg->len; i++)
	{
		PrintInfo("[%s:%d] pMsg->pData[%d] = 0x%X\n", __FUNCTION__, __LINE__, i, pMsg->pData[i]);
	}

	return DRV_OK;
}

DRV_Error DRV_RF4CE_CE_SetRxModeCnf(RF4CE_Msg_t* pMsg)
{
	HUINT32 i=0;

	for(i=0; i<pMsg->len; i++)
	{
		PrintInfo("[%s:%d] pMsg->pData[%d] = 0x%X\n", __FUNCTION__, __LINE__, i, pMsg->pData[i]);
	}

	return DRV_OK;
}

DRV_Error DRV_RF4CE_CE_SetRxEnableCnf(RF4CE_Msg_t* pMsg)
{
	HUINT32 i=0;

	for(i=0; i<pMsg->len; i++)
	{
		PrintInfo("[%s:%d] pMsg->pData[%d] = 0x%X\n", __FUNCTION__, __LINE__, i, pMsg->pData[i]);
	}

	return DRV_OK;
}

DRV_Error DRV_RF4CE_CE_SetTestModeCnf(RF4CE_Msg_t* pMsg)
{
	HUINT32 i=0;

	for(i=0; i<pMsg->len; i++)
	{
		PrintInfo("[%s:%d] pMsg->pData[%d] = 0x%X\n", __FUNCTION__, __LINE__, i, pMsg->pData[i]);
	}

	return DRV_OK;
}

DRV_Error DRV_RF4CE_CE_GetRssiCnf(RF4CE_Msg_t* pMsg)
{
	HUINT32 i=0;

	for(i=0; i<pMsg->len; i++)
	{
		PrintInfo("[%s:%d] pMsg->pData[%d] = 0x%X\n", __FUNCTION__, __LINE__, i, pMsg->pData[i]);
	}

	return DRV_OK;
}

DRV_Error DRV_RF4CE_CE_GetContextCnf(RF4CE_Msg_t* pMsg)
{
	PrintInfo("[%s:%d] FRMFILT0 = 0x%02X\n", __FUNCTION__, __LINE__, pMsg->pData[0]);
	PrintInfo("[%s:%d] FRMCTRL0 = 0x%02X\n", __FUNCTION__, __LINE__, pMsg->pData[1]);
	PrintInfo("[%s:%d] FRMCTRL1 = 0x%02X\n", __FUNCTION__, __LINE__, pMsg->pData[2]);
	PrintInfo("[%s:%d] MDMTEST1 = 0x%02X\n", __FUNCTION__, __LINE__, pMsg->pData[3]);
	PrintInfo("[%s:%d] TXPOWER = 0x%02X\n", __FUNCTION__, __LINE__, pMsg->pData[4]);
	PrintInfo("[%s:%d] TXCTRL = 0x%02X\n", __FUNCTION__, __LINE__, pMsg->pData[5]);
	PrintInfo("[%s:%d] FSCTRL = 0x%02X\n", __FUNCTION__, __LINE__, pMsg->pData[6]);
	PrintInfo("[%s:%d] FREQCTRL = 0x%02X\n", __FUNCTION__, __LINE__, pMsg->pData[7]);
/*
	PrintInfo("[%s:%d] FSCAL1 = 0x%02X\n", __FUNCTION__, __LINE__, pMsg->pData[8]);
	PrintInfo("[%s:%d] AGCCTRL1 = 0x%02X\n", __FUNCTION__, __LINE__, pMsg->pData[9]);
	PrintInfo("[%s:%d] TXFILTCFG = 0x%02X\n", __FUNCTION__, __LINE__, pMsg->pData[10]);
	PrintInfo("[%s:%d] IVCTRL = 0x%02X\n", __FUNCTION__, __LINE__, pMsg->pData[11]);
*/
	return DRV_OK;
}

/* end of file */


/*******************************************************************/
/************************* File Description ************************/
/*******************************************************************/
// File Name: drv_cimax.c
// Original Author:
// File Description:
// Module:
// Remarks:

/*******************************************************************/
/* Copyright (c) 2011 HUMAX Co., Ltd.								*/
/* All rights reserved. 												*/
/*******************************************************************/


/*******************************************************************/
/* INCLUDE FILES													*/
/*******************************************************************/
#if defined(CONFIG_VK_STDLIB)
#else
#include 	<stdio.h>
#include 	<string.h>
#endif
#include <fcntl.h>
#include <unistd.h>
#include <sys/mman.h>
#include <ctype.h>

#include "nexus_types.h"
#include "nexus_pid_channel.h"

/* humax di layer */
#include "linden_trace.h"

#include "bstd_defs.h"
#include "berr.h"
#include "bdbg.h"
#include "breg_mem.h"
#include "vkernel.h"
#include "taskdef.h"
#include "di_err.h"
#include "drv_cimax.h"
#include "di_ci.h"
#include "di_uart.h"
#include "pdriver_cimax.h"
#include "drv_gpio.h"
#include "drv_spi.h"
#include "drv_demux.h"

/*******************************************************************/
/* LOCAL DEFINE 													*/
/*******************************************************************/
#define DEBUG_MODULE						TRACE_MODULE_DI_CI
#define UNREFENCED_PARAM(x) 				(x=x)
#define DRV_CI_USING_MMAP					(1)
#define CI_TASK_SLEEP_TIME					500
#define DI_CI_TASK_STACKSIZE				(1024 * 8)
#define MAX_NUM_CIMSG						16
#define CAM_RESET_TIME						20
#define MAX_RESET_COUNT						300 // org is 100
#define GPIO_PIN_5_CI_PWR					5

#define CI_PWR_OFF							(0)
#define CI_PWR_ON							(1)

#define CAM_RESET_OFF						(0)
#define CAM_RESET_ON						(1)
#if defined(CONFIG_TASKBASE_FW_DOWNLOAD)
#define SPI_WAIT_TIMEOUT					3000
#else
#define SPI_WAIT_TIMEOUT					1000
#endif
#define CIMAX_RETRY_NUM						10
#define CIMAX_RESET_RETRY					35
#define CIMAX_POWER_RETRY					20

#define ECOOMMAND_EXTRACT	0
#define ECOOMMAND_HWRESET	1

#define CIMAX_SLEEP_MAX_TIME				100
#define CIMAX_SLEEP_MIDDLE_TIME				50
#define CIMAX_SLEEP_SMALL_TIME				10

#define	NOT_USED_VALUE		0xff
#define	USED_LIVE		1
#define	USED_PLAYBACK		1
#define	USED_SLOTOUTPUT		1

#include "cimaxplus_firmware.h"

#if 0 /* org.. 우선 막아 놓음.. 더 검증 해야 함  */
#define USE_GAP_REMOVER //use GAP_REMOVER by default.
#endif
//#define CAM_SLOT_POWER_ALWAYS_ON /*if disabled, slot power on after cam detected*/

/*******************************************************************/
/* GLOBAL VARIABLES 												*/
/*******************************************************************/
CAMINFO_t caminfo;

/*******************************************************************/
/* STATIC VARIABLES 												*/
/*******************************************************************/
static HULONG		s_ulCiMaxEventId;
static HULONG		s_ulCiTaskId;
#if defined(CONFIG_TASKBASE_FW_DOWNLOAD)
static HULONG		s_ulCiInitTaskId;
#endif
static HULONG		s_ulCiSemId, s_ulCiConnSemId;
static HULONG		s_ulCiMaxMsgId;
HUINT8				s_cimaxBufferOut[CIMAX_CAM_HEADER_SIZE+CIMAX_CAM_MAX_PAYLOAD_SIZE] = {0,};
HUINT8				s_cimaxBufferIn[CIMAX_CAM_HEADER_SIZE+CIMAX_CAM_MAX_PAYLOAD_SIZE] = {0,};

static HULONG		s_cimaxNormalCommandErrorCount = 0;
static HULONG		s_cimaxAcessCommandSPIErrorCount = 0;

DRV_CI_PATH_STATUS_t	sCIPathStatus[CAM_MAXSLOTID];

static HUINT32		s_ulCiOnceFifoResetByInsert[CAM_MAXSLOTID] = {0,};
/*******************************************************************/
/* TYPE DEFINE													*/
/*******************************************************************/
/*******************************************************************/
typedef enum CI_TASK_STATE
{
	CI_TASK_IDLE = 0,
	CI_TASK_CAM_EXTRACTED,
	CI_TASK_CAM_INSERTED,
	CI_TASK_CAM_POWER_ON,
	CI_TASK_CAM_RESET,
	CI_TASK_CAM_ACCESSIBLE
}CI_TASK_STATE_t;

void (*_pfnCiCallbackFuncforDemux)(HUINT16, HUINT16);
HINT32 CIMAX_CheckCommandStatus(void);
HINT32 CIMAX_GetEventCommand(HUINT8 *slotId, HUINT8 *evtStatus );
DRV_Error DRV_CIMAX_SetCLK(HUINT32 slotId,HUINT32 flag );

int P_CIMAX_GetIntPinLevel(void)
{
	DRV_Error drvError;
	GPIO_STATE_t state;
	drvError = DRV_GPIO_Read(GPIO_ID_CIMAX_INT, &state);
	if(drvError != DRV_OK)
		return -1;
	return (int)state;
}
#if defined(CONFIG_EXT_CI_CTRL_INT_MODE)
HINT32 CIMAX_GetResponse(HUINT32 ulSpeed)
{
	DRV_Error	drvErr = 0;
	HUINT8		jcount1 = 0, jcount2 = 0;
	HUINT32 	totalLen = 0;
	HUINT8		transcmd = 0,transmode = 0;
	HUINT16 	numPayload = 0;
	HUINT16		dataLen = 0;
	HUINT8		invalidflag = 0;
	HUINT32		index = 0;

	VK_MEM_Memset(s_cimaxBufferIn, 0, sizeof(s_cimaxBufferIn));

	/* check a interrupt signal from a CI external controller. */
	while (P_CIMAX_GetIntPinLevel() != 0);

AGAIN:
	if(ulSpeed == GET_CI_EXT_CTRL_RESP)
	{
		totalLen = CIMAX_REGISTER_HEADER_SIZE;
		transmode = 0;

		drvErr = DRV_SPI_ReceivePacket(0, transmode, s_cimaxBufferIn, totalLen);
		if(drvErr != 0)
		{
			jcount1++;

			if(jcount1 < CIMAX_RETRY_NUM)
			{
				VK_TASK_Sleep(CIMAX_SLEEP_MAX_TIME);
				PrintError("== NC SPR AG(%d:%d)(%x) ==\r\n",drvErr,jcount1,transcmd);
				return CIMAX_CAM_RESPONSE_ERROR;
			}
			else
			{
				s_cimaxNormalCommandErrorCount++;
				jcount1 = 0;
				PrintError("== NC SPR E(%d:%d)(%x)(%d) ==\r\n",drvErr,totalLen,transcmd,s_cimaxNormalCommandErrorCount);
				return CIMAX_SPI_COMM_ERROR;
			}
		}
	}
	else
	{
		return INT_SIG;
	}

	jcount1 = 0;

	switch(s_cimaxBufferIn[CIMAX_REGISTER_CMD_POS] & CIMAX_CAM_TYP_CMD_MASK)
	{
		case CIMAX_REGISTER_INIT_RESP_OK:	/* Fall through. */
		case CIMAX_REGISTER_INIT_RESP_NOK:	/* Fall through. */
		case CIMAX_REGISTER_WRITE_RESP_OK:	/* Fall through. */
		case CIMAX_REGISTER_WRITE_RESP_NOK: /* Fall through. */
		case CIMAX_REGISTER_READ_RESP_OK:	/* Fall through. */
		case CIMAX_REGISTER_READ_RESP_NOK:	/* Fall through. */
		case CIMAX_REGISTER_CMD_PARSE_ERROR:
			/* Read register response payload if needed. */
			numPayload = s_cimaxBufferIn[CIMAX_CAM_DATA_LEN_MSB_POS];
			if ((s_cimaxBufferIn[CIMAX_REGISTER_NUM_OF_REGS_POS] > 0)&&(s_cimaxBufferIn[CIMAX_REGISTER_NUM_OF_REGS_POS] < 0xff))
			{
				drvErr	= DRV_SPI_ReceivePacket(0,transmode,&s_cimaxBufferIn[CIMAX_REGISTER_HEADER_SIZE],s_cimaxBufferIn[CIMAX_REGISTER_NUM_OF_REGS_POS]);
				if(drvErr != 0)
				{
					s_cimaxNormalCommandErrorCount++;
					PrintError("== NC R1 (%x)(%x:%x:%x:%x:%x)(%d)####==\r\n",drvErr,s_cimaxBufferIn[0],s_cimaxBufferIn[1],s_cimaxBufferIn[2],s_cimaxBufferIn[3],s_cimaxBufferIn[4],s_cimaxNormalCommandErrorCount);
					return CIMAX_SPI_COMM_ERROR;
				}

			}
			break;

		case CIMAX_CAM_A_RESET_OK:			   /* or CIMAX_CAM_B_RESET_OK */
			PrintDebug("%s(%d)[RESET_OK]  check GPIO_INT! \n",__func__, __LINE__);
		case CIMAX_CAM_A_GET_CIS_OK:		   /* or CIMAX_CAM_B_GET_CIS_OK */
		case CIMAX_CAM_A_WRITE_COR_OK:		   /* or CIMAX_CAM_B_WRITE_COR_OK */
		case CIMAX_CAM_A_NEGOTIATE_OK:		   /* or CIMAX_CAM_B_NEGOTIATE_OK */
		case CIMAX_CAM_A_WRITE_LPDU_OK: 	   /* or CIMAX_CAM_B_WRITE_LPDU_OK */
		case CIMAX_CAM_A_READ_LPDU_OK:		   /* or CIMAX_CAM_B_READ_LPDU_OK */
		case CIMAX_CAM_A_CAM_ERROR: 		   /* or CIMAX_CAM_B_CAM_ERROR */
		case CIMAX_CAM_A_WRITE_BUSY:		   /* or CIMAX_CAM_B_WRITE_BUSY */
		case CIMAX_CAM_A_DET_OK:			   /* or CIMAX_CAM_B_DET_OK */
		case CIMAX_CAM_A_DATA_READY:		   /* or CIMAX_CAM_B_DATA_READY */
		case CIMAX_CAM_A_CMD_PENDING:		   /* or CIMAX_CAM_B_CMD_PENDING */
		case CIMAX_CAM_A_REG_STATUS_OK: 	   /* or CIMAX_CAM_B_REG_STATUS_OK */
		case CIMAX_CAM_NO_EVT:
		case CIMAX_CAM_A_NO_CAM:			   /* or CIMAX_CAM_B_NO_CAM */
			/* Get data length for CAM response. */
			dataLen = s_cimaxBufferIn[CIMAX_CAM_DATA_LEN_MSB_POS];
			dataLen = (dataLen << 8);
			dataLen = dataLen | s_cimaxBufferIn[CIMAX_CAM_DATA_LEN_LSB_POS];

			/* Read CAM response payload if needed. */
			if((dataLen > 0)&&(dataLen < 65535))
			{
				drvErr = DRV_SPI_ReceivePacket(0,transmode,&s_cimaxBufferIn[CIMAX_CAM_HEADER_SIZE], dataLen);
				if(drvErr != 0)
				{
					s_cimaxNormalCommandErrorCount++;
					PrintError("== NC R2 (%x)(%x:%x:%x:%x:%x)(%d)####==\r\n",drvErr,s_cimaxBufferIn[0],s_cimaxBufferIn[1],s_cimaxBufferIn[2],s_cimaxBufferIn[3],s_cimaxBufferIn[4],s_cimaxNormalCommandErrorCount);
					return CIMAX_SPI_COMM_ERROR;
				}
			}
			numPayload = dataLen;
			break;

		default:
			PrintDebug("==== Command :: Transmit Data Start (%d) =======\r\n",totalLen);
			for( index = 0; index < totalLen; index++)
			{
				PrintDebug("  [0x%02x]",s_cimaxBufferOut[index]);
			}
			PrintDebug("\r\n");
			PrintDebug("=================================================\r\n");
			PrintDebug( "%s: ***** invalid response!:0x%02x 0x%02x 0x%02x 0x%02x. *****\n",
			__FUNCTION__,s_cimaxBufferIn[CIMAX_CAM_COMMAND_OR_STATUS_POS],
			s_cimaxBufferIn[CIMAX_CAM_PACKET_COUNTER_POS],
			s_cimaxBufferIn[CIMAX_CAM_DATA_LEN_MSB_POS],
			s_cimaxBufferIn[CIMAX_CAM_DATA_LEN_LSB_POS]);

			invalidflag = 1;
			break;

	}  /* switch (responseBuff[CIMAX_REGISTER_CMD_POS] & CIMAX_CAM_TYP_CMD_MASK) */
	if(invalidflag == 1)
	{
		jcount2++;
		if(jcount2 < CIMAX_RESET_RETRY)
		{
			invalidflag = 0;
			VK_TASK_Sleep(CIMAX_SLEEP_MAX_TIME);
			PrintDebug("== NC SPR E AG(%d:%d)(%x) ==\r\n",drvErr,jcount2,transcmd);
			goto AGAIN;
		}
		else
		{
			invalidflag = 0;
			jcount2 = 0;
			PrintDebug("== NC SPR E End(%d:%d)(%x) ==\r\n",drvErr,totalLen,transcmd);
		}
	}

	jcount2 = 0;
	invalidflag = 0;

	return numPayload;

}
#endif

HINT32 CIMAX_AcessRegisterCommand(HUINT8 cmd, HUINT16 addr, HUINT8 *buffer, HUINT8 numOfBytes)
{
	HINT32		vkErr = 0;
	DRV_Error	drvErr = 0;
	HUINT8		bufferOut[CIMAX_REGISTER_HEADER_SIZE+CIMAX_REGISTER_MAX_PAYLOAD_SIZE] = {0,};
	HUINT32		totalLen = 0;
	HUINT8		transmode = 0;
	HUINT8		transfailcnt = 0;
#if defined(CONFIG_EXT_CI_CTRL_INT_MODE)
	HINT32		retVal = 0;
#else
	HUINT16		dataLen = 0;
	HULONG		ulEvent = 0;
	HUINT8		transfailcnt1 = 0,transfailcnt2 = 0;
	HUINT8		invalidflag = 0,jcount = 0;
	HUINT8		evtSlotId, evtStatus;
	HINT32			evttrycnt=0;
#endif

	if(((cmd != CIMAX_REGISTER_WRITE_REQ)&&(cmd != CIMAX_REGISTER_READ_REQ))||(buffer == NULL))
	{
		PrintError("== ARC S1 (%x)####==\r\n",drvErr);
		return CIMAX_ERROR;
	}

	VK_MEM_Memset(bufferOut,0,sizeof(bufferOut));

	vkErr = VK_SEM_Get(s_ulCiSemId);
#if defined(CONFIG_EXT_CI_CTRL_INT_MODE)
#else
	while(P_CIMAX_GetIntPinLevel() == 0)
	{
		PrintDebug("%s() read Event (%dth)\n", __func__, evttrycnt);
		CIMAX_GetEventCommand( &evtSlotId, &evtStatus);
		PrintDebug("event received. slotd=%d evt=%d\n", evtSlotId, evtStatus);
		evttrycnt++;
	}
#endif
TRANSAGAIN:
	bufferOut[CIMAX_REGISTER_CMD_POS]			= cmd;
	bufferOut[CIMAX_REGISTER_REG_ADDR_MSB_POS]	= (addr>>8) & 0xFF;
	bufferOut[CIMAX_REGISTER_REG_ADDR_LSB_POS]	= addr & 0xFF;
	bufferOut[CIMAX_REGISTER_NUM_OF_REGS_POS]	= numOfBytes;

	if(cmd == CIMAX_REGISTER_WRITE_REQ)
	totalLen = CIMAX_REGISTER_HEADER_SIZE + numOfBytes;
	else
		totalLen = CIMAX_REGISTER_HEADER_SIZE;

	if(cmd == CIMAX_REGISTER_WRITE_REQ)
	{
		VK_memcpy(&bufferOut[CIMAX_REGISTER_HEADER_SIZE], buffer, numOfBytes);
	}

	transmode = 0;

	drvErr = DRV_SPI_TransmitPacket(0,transmode, bufferOut, totalLen);
	if(drvErr != 0)
	{
		PrintError("== ARC SPI T R1 (%x)####==\r\n",drvErr);
		transfailcnt++;
		if(transfailcnt < CIMAX_RETRY_NUM)
		{
			VK_TASK_Sleep(CIMAX_SLEEP_MAX_TIME);
			PrintDebug("== ARC SPI T AG1 (%x:%d)(%x)####==\r\n",drvErr,transfailcnt,cmd);
			goto TRANSAGAIN;
		}
		else
		{
			transfailcnt = 0;
			s_cimaxAcessCommandSPIErrorCount++;
			VK_SEM_Release(s_ulCiSemId);
			PrintError("== ARC SPI T AG1 E(%x:%d)(%x) (%d)####==\r\n",drvErr,transfailcnt,cmd,s_cimaxAcessCommandSPIErrorCount);
			return CIMAX_SPI_COMM_ERROR;
		}
	}
#if defined(CONFIG_EXT_CI_CTRL_INT_MODE)
	retVal = CIMAX_GetResponse(GET_CI_EXT_CTRL_RESP);
	if (retVal < 0)
	{
		VK_SEM_Release(s_ulCiSemId);
		PrintError("[%s] CIMAX_GetResponse Error! \n", __func__);
		return retVal;
	}
#else
	transfailcnt = 0;

	vkErr = VK_EVENT_ReceiveTimeout(s_ulCiMaxEventId, &ulEvent,SPI_WAIT_TIMEOUT);
	if(vkErr != 0)
	{
		transfailcnt1++;
		if(transfailcnt1 < CIMAX_RETRY_NUM)
		{
			VK_TASK_Sleep(CIMAX_SLEEP_MAX_TIME);
			PrintDebug("== ARC SPI TO AG2 (%x:%d)(%x)####==\r\n",drvErr,transfailcnt1,cmd);
			goto TRANSAGAIN;
		}
		else
		{

			transfailcnt1 = 0;
			s_cimaxAcessCommandSPIErrorCount++;
			VK_SEM_Release(s_ulCiSemId);
			PrintError("== ARC SPI TO AG2 E(%x:%d)(%x:%x)(%d)####==\r\n",drvErr,transfailcnt1,cmd,addr,s_cimaxAcessCommandSPIErrorCount);
			return CIMAX_TIMEOUT_ERROR;
		}
	}

	transfailcnt1 = 0;

	totalLen = CIMAX_REGISTER_HEADER_SIZE;

	transmode = 0;

	VK_TASK_Sleep(CIMAX_SLEEP_MIDDLE_TIME);

	VK_MEM_Memset(s_cimaxBufferIn, 0, sizeof(s_cimaxBufferIn));

AGAIN:
	drvErr	= DRV_SPI_ReceivePacket(0,transmode, s_cimaxBufferIn,totalLen);
	if(drvErr != 0)
	{
		PrintError("== ARC R3 (%x)####==\r\n",drvErr);
		transfailcnt2++;
		if(transfailcnt2 < CIMAX_RETRY_NUM)
		{
			VK_TASK_Sleep(CIMAX_SLEEP_MAX_TIME);
			PrintDebug("== ARC SPI R AG (%x:%d)(%x)####==\r\n",drvErr,transfailcnt2,cmd);
			goto AGAIN;
		}
		else
		{

			transfailcnt2 = 0;
			s_cimaxAcessCommandSPIErrorCount++;
			VK_SEM_Release(s_ulCiSemId);
			PrintError("== ARC SPI R AG E(%x:%d)(%x)####==\r\n",drvErr,transfailcnt2,cmd);
			return CIMAX_SPI_COMM_ERROR;
		}


	}

	switch(s_cimaxBufferIn[CIMAX_REGISTER_CMD_POS] & CIMAX_CAM_TYP_CMD_MASK)
	{
		case CIMAX_REGISTER_INIT_RESP_OK:   /* Fall through. */
		case CIMAX_REGISTER_INIT_RESP_NOK:  /* Fall through. */
		case CIMAX_REGISTER_WRITE_RESP_OK:  /* Fall through. */
		case CIMAX_REGISTER_WRITE_RESP_NOK: /* Fall through. */
		case CIMAX_REGISTER_READ_RESP_OK:   /* Fall through. */
		case CIMAX_REGISTER_READ_RESP_NOK:  /* Fall through. */
		case CIMAX_REGISTER_CMD_PARSE_ERROR:
			/* Read register response payload if needed. */
			if ((s_cimaxBufferIn[CIMAX_REGISTER_NUM_OF_REGS_POS] > 0)&&(s_cimaxBufferIn[CIMAX_REGISTER_NUM_OF_REGS_POS] < 0xff))
			{
				drvErr	= DRV_SPI_ReceivePacket(0,transmode,&s_cimaxBufferIn[CIMAX_REGISTER_HEADER_SIZE],s_cimaxBufferIn[CIMAX_REGISTER_NUM_OF_REGS_POS]);
				if(drvErr != 0)
				{
					PrintError("== ARC R4 (%x)(%x:%x:%x:%x:%x)####==\r\n",drvErr,s_cimaxBufferIn[0],s_cimaxBufferIn[1],s_cimaxBufferIn[2],s_cimaxBufferIn[3],s_cimaxBufferIn[4]);
					s_cimaxAcessCommandSPIErrorCount++;
					VK_SEM_Release(s_ulCiSemId);
					return CIMAX_SPI_COMM_ERROR;
				}

			}
			break;

		case CIMAX_CAM_A_RESET_OK:             /* or CIMAX_CAM_B_RESET_OK */
		case CIMAX_CAM_A_GET_CIS_OK:           /* or CIMAX_CAM_B_GET_CIS_OK */
		case CIMAX_CAM_A_WRITE_COR_OK:         /* or CIMAX_CAM_B_WRITE_COR_OK */
		case CIMAX_CAM_A_NEGOTIATE_OK:         /* or CIMAX_CAM_B_NEGOTIATE_OK */
		case CIMAX_CAM_A_WRITE_LPDU_OK:        /* or CIMAX_CAM_B_WRITE_LPDU_OK */
		case CIMAX_CAM_A_READ_LPDU_OK:         /* or CIMAX_CAM_B_READ_LPDU_OK */
		case CIMAX_CAM_A_NO_CAM:               /* or CIMAX_CAM_B_NO_CAM */
		case CIMAX_CAM_A_CAM_ERROR:            /* or CIMAX_CAM_B_CAM_ERROR */
		case CIMAX_CAM_A_WRITE_BUSY:           /* or CIMAX_CAM_B_WRITE_BUSY */
		case CIMAX_CAM_A_DET_OK:               /* or CIMAX_CAM_B_DET_OK */
		case CIMAX_CAM_A_DATA_READY:           /* or CIMAX_CAM_B_DATA_READY */
		case CIMAX_CAM_A_CMD_PENDING:          /* or CIMAX_CAM_B_CMD_PENDING */
		case CIMAX_CAM_A_REG_STATUS_OK:        /* or CIMAX_CAM_B_REG_STATUS_OK */
		case CIMAX_CAM_NO_EVT:
			/* Get data length for CAM response. */
			dataLen = s_cimaxBufferIn[CIMAX_CAM_DATA_LEN_MSB_POS];
			dataLen = (dataLen << 8);
			dataLen = dataLen | s_cimaxBufferIn[CIMAX_CAM_DATA_LEN_LSB_POS];

			/* Read CAM response payload if needed. */
			if((dataLen > 0)&&(dataLen < CIMAX_CAM_MAX_PAYLOAD_SIZE))
			{
				drvErr = DRV_SPI_ReceivePacket(0,transmode,&s_cimaxBufferIn[CIMAX_CAM_HEADER_SIZE], dataLen);
				if(drvErr != 0)
				{
					PrintError("== ARC R5 (%x)(%x:%x:%x:%x:%x)####==\r\n",drvErr,s_cimaxBufferIn[0],s_cimaxBufferIn[1],s_cimaxBufferIn[2],s_cimaxBufferIn[3],s_cimaxBufferIn[4]);
					s_cimaxAcessCommandSPIErrorCount++;
					VK_SEM_Release(s_ulCiSemId);
					return CIMAX_SPI_COMM_ERROR;
				}
			}
			break;

		default:
			PrintDebug( "%s: ***** invalid response!:0x%02x 0x%02x 0x%02x 0x%02x. *****\n",
			__FUNCTION__,s_cimaxBufferIn[CIMAX_CAM_COMMAND_OR_STATUS_POS],
			s_cimaxBufferIn[CIMAX_CAM_PACKET_COUNTER_POS],
			s_cimaxBufferIn[CIMAX_CAM_DATA_LEN_MSB_POS],
			s_cimaxBufferIn[CIMAX_CAM_DATA_LEN_LSB_POS]);
			invalidflag = 1;
			break;
	}  /* switch (responseBuff[CIMAX_REGISTER_CMD_POS] & CIMAX_CAM_TYP_CMD_MASK) */

	if(invalidflag == 1)
	{
		jcount++;
		if(jcount < CIMAX_RESET_RETRY)
		{
			invalidflag = 0;
			VK_TASK_Sleep(CIMAX_SLEEP_MAX_TIME);
			PrintDebug("== ARC SPR Err AG(%d:%d) ==\r\n",drvErr,jcount);
			goto AGAIN;
		}
		else
		{
			invalidflag = 0;
			jcount = 0;
			PrintDebug("== ARC SPR Err End(%d:%d) ==\r\n",drvErr,totalLen);
		}
	}

	jcount = 0;
#endif
	if(cmd == CIMAX_REGISTER_WRITE_REQ)
	{
		totalLen = CIMAX_REGISTER_HEADER_SIZE;
	}
	else
	{
		totalLen = CIMAX_REGISTER_HEADER_SIZE + numOfBytes;
	}

	if ((s_cimaxBufferIn[CIMAX_REGISTER_REG_ADDR_MSB_POS]	!= bufferOut[CIMAX_REGISTER_REG_ADDR_MSB_POS]) ||
	(s_cimaxBufferIn[CIMAX_REGISTER_REG_ADDR_LSB_POS]	!= bufferOut[CIMAX_REGISTER_REG_ADDR_LSB_POS]))
	{
		if(cmd == CIMAX_REGISTER_WRITE_REQ)
		{
			if((s_cimaxBufferIn[CIMAX_REGISTER_CMD_POS] != CIMAX_REGISTER_WRITE_RESP_OK )||(s_cimaxBufferIn[CIMAX_REGISTER_NUM_OF_REGS_POS] != 0x00))
			{
				PrintError("== ARC R6 (%x:%x) ####==\r\n",s_cimaxBufferIn[CIMAX_REGISTER_CMD_POS],s_cimaxBufferIn[CIMAX_REGISTER_NUM_OF_REGS_POS]);
				s_cimaxAcessCommandSPIErrorCount++;
				VK_SEM_Release(s_ulCiSemId);
				return CIMAX_SPI_COMM_ERROR;
			}
		}
		else
		{
			if((s_cimaxBufferIn[CIMAX_REGISTER_CMD_POS] != CIMAX_REGISTER_READ_RESP_OK )||(s_cimaxBufferIn[CIMAX_REGISTER_NUM_OF_REGS_POS] != totalLen))
			{
				PrintError("== ARC R7 (%x:%x:%d) ####==\r\n",s_cimaxBufferIn[CIMAX_REGISTER_CMD_POS],s_cimaxBufferIn[CIMAX_REGISTER_NUM_OF_REGS_POS],totalLen);
				s_cimaxAcessCommandSPIErrorCount++;
				VK_SEM_Release(s_ulCiSemId);
				return CIMAX_SPI_COMM_ERROR;
			}
		}
	}
	else
	{
		if(cmd == CIMAX_REGISTER_READ_REQ)
		{
			VK_MEM_Memcpy(buffer,&s_cimaxBufferIn[CIMAX_REGISTER_HEADER_SIZE],s_cimaxBufferIn[CIMAX_REGISTER_NUM_OF_REGS_POS]);
		}
	}

	VK_SEM_Release(s_ulCiSemId);

	return CIMAX_NO_ERROR;
}


HINT32 CIMAX_WriteRegister(HUINT16 addr, HUINT8 *buffer, HUINT8 numOfBytes)
{
	HINT32 result = 0;

	result  = CIMAX_AcessRegisterCommand(CIMAX_REGISTER_WRITE_REQ,addr,buffer,numOfBytes);

	return result;
}

HINT32 CIMAX_ReadRegister(HUINT16 addr, HUINT8 *buffer, HUINT8 numOfBytes)
{
	HINT32 result = 0;

	result  = CIMAX_AcessRegisterCommand(CIMAX_REGISTER_READ_REQ,addr,buffer,numOfBytes);

	return result;
}


HINT32	CIMAX_CheckSlotStatus(HUINT8 slotId, HUINT8 *result)
{
	HUINT8 rdata[2];
	HINT32 ret = 0;

	if(slotId >= CIMAX_CAM_MAX_SLOT_NUM)
		return -1;

	ret = CIMAX_CheckCommandStatus();
	if(ret != CIMAX_NO_ERROR)
	{
		return CIMAX_ERROR;
	}

	ret  = CIMAX_ReadRegister(MOD_CTRL_A,rdata,2);
	if(ret != 0)
		return ret;

	if(slotId == 0)
		*result = rdata[0];
	else
		*result = rdata[1];
	return 0;

}

#if defined(CAM_SLOT_POWER_ALWAYS_ON)
HINT32 CIMAX_PowerOnCam(void)
{
	HINT32 result  = 0;
	HUINT8 wdata = 0, rdata = 0,count = 0,count1 = 0;

	rdata = 0;
	result	= CIMAX_ReadRegister(GPIO0_DATA_OUT,&rdata,1);
	if(result != 0 )
	{
		PrintError("== CIMAX_PowerOnCam Return Error 1(%d) ==\r\n",result);
		return result;
	}

	rdata |= (EN_SLOT0|EN_SLOT1);

	wdata = rdata;
	result	= CIMAX_WriteRegister(GPIO0_DATA_OUT,&wdata,1);
	if(result != 0 )
		return -5;

	do
	{
		rdata = 0;
		result	= CIMAX_ReadRegister(CFG_1,&rdata,1);
		if(result != 0 )
		{
			PrintError("== CIMAX_PowerOnCam Return Error 2(%d) ==\r\n",result);
			return result;
		}
		else
		{
			VK_TASK_Sleep(CIMAX_SLEEP_MAX_TIME);
			count++;
			if(count == CIMAX_POWER_RETRY)
			{
				PrintError("== CIMAX_PowerOnCam Return Error 3(%d:%x) ==\r\n",result,rdata);
				return result;
			}
		}

	}while((rdata&EN_VCC) != EN_VCC);

	rdata = 0;
	result	= CIMAX_ReadRegister(CFG_1,&rdata,1);
	if(result != 0 )
	{
		PrintError("== CIMAX_PowerOnCam Return Error 4(%d) ==\r\n",result);
		return result;
	}

	VK_TASK_Sleep(CIMAX_SLEEP_SMALL_TIME);

	wdata = rdata;
	wdata |= EN_PCM_OUT;

	result	= CIMAX_WriteRegister(CFG_1,&wdata,1);
	if(result != 0 )
	{
		PrintError("== CIMAX_PowerOnCam Return Error 5(%d) ==\r\n",result);
		return result;
	}

	count = 0;
	do
	{
		rdata = 0;
		result	= CIMAX_ReadRegister(CFG_1,&rdata,1);
		if(result != 0 )
		{
			PrintError("== CIMAX_PowerOnCam Return Error 6(%d) ==\r\n",result);
			return result;
		}
		else
		{
			VK_TASK_Sleep(CIMAX_SLEEP_MAX_TIME);
			count1++;
			if(count1 == CIMAX_RETRY_NUM)
			{
				PrintError("== CIMAX_PowerOnCam Return Error 7(%d) ==\r\n",result);
				return result;
			}
		}

	}while((rdata&EN_PCM_OUT) != EN_PCM_OUT);

	return 0;

}
#else
HINT32 CIMAX_PowerOnCam(HUINT8 slotId)
{
	HINT32 result  = 0;
	HUINT8 wdata = 0, rdata = 0,count = 0,count1 = 0;

	if(slotId >= CIMAX_CAM_MAX_SLOT_NUM)
		return -1;

	rdata = 0;
	result	= CIMAX_ReadRegister(GPIO0_DATA_OUT,&rdata,1);
	if(result != 0 )
	{
		PrintError("== CIMAX_PowerOnCam Return Error 1(%d) ==\r\n",result);
		return result;
	}

	VK_TASK_Sleep(CIMAX_SLEEP_SMALL_TIME);

	if(slotId == 0)
		rdata |= EN_SLOT0;
	else
		rdata |= EN_SLOT1;

	wdata = rdata;
	result	= CIMAX_WriteRegister(GPIO0_DATA_OUT,&wdata,1);
	if(result != 0 )
		return -5;

	VK_TASK_Sleep(CIMAX_SLEEP_SMALL_TIME);

	do
	{
		rdata = 0;
		result	= CIMAX_ReadRegister(CFG_1,&rdata,1);
		if(result != 0 )
		{
			PrintError("== CIMAX_PowerOnCam Return Error 2(%d) ==\r\n",result);
			return result;
		}
		else
		{
			VK_TASK_Sleep(CIMAX_SLEEP_MAX_TIME);
			count++;
			if(count == CIMAX_POWER_RETRY)
			{
				PrintError("== CIMAX_PowerOnCam Return Error 3(%d:%x) ==\r\n",result,rdata);
				return result;
			}
		}

	}while((rdata&EN_VCC) != EN_VCC);

	rdata = 0;
	result	= CIMAX_ReadRegister(CFG_1,&rdata,1);
	if(result != 0 )
	{
		PrintError("== CIMAX_PowerOnCam Return Error 4(%d) ==\r\n",result);
		return result;
	}

	VK_TASK_Sleep(CIMAX_SLEEP_SMALL_TIME);

	wdata = rdata;
	wdata |= EN_PCM_OUT;

	result	= CIMAX_WriteRegister(CFG_1,&wdata,1);
	if(result != 0 )
	{
		PrintError("== CIMAX_PowerOnCam Return Error 5(%d) ==\r\n",result);
		return result;
	}

	count = 0;
	do
	{
		rdata = 0;
		result	= CIMAX_ReadRegister(CFG_1,&rdata,1);
		if(result != 0 )
		{
			PrintError("== CIMAX_PowerOnCam Return Error 6(%d) ==\r\n",result);
			return result;
		}
		else
		{
			VK_TASK_Sleep(CIMAX_SLEEP_MAX_TIME);
			count1++;
			if(count1 == CIMAX_RETRY_NUM)
			{
				PrintError("== CIMAX_PowerOnCam Return Error 7(%d) ==\r\n",result);
				return result;
			}
		}

	}while((rdata&EN_PCM_OUT) != EN_PCM_OUT);

	return 0;

}
#endif

HINT32 CIMAX_PowerOffCam(HUINT8 slotId)
{
	HINT32 result  = 0;
	HUINT8 wdata = 0;
	HUINT8 rdata = 0;

	if(slotId >= CIMAX_CAM_MAX_SLOT_NUM)
	{
		return -1;
	}

	rdata = 0;
	result  = CIMAX_ReadRegister(GPIO0_DATA_OUT,&rdata,1);
	if(result != 0 )
	{
		PrintError("== CIMAX_PowerOffCam Return Error 1(%d) ==\r\n",result);
		return result;
	}

	wdata = rdata;
	if(slotId == 0)
		wdata &= ~(EN_SLOT0);
	else
		wdata &= ~(EN_SLOT1);

	result  = CIMAX_WriteRegister(GPIO0_DATA_OUT,&wdata,1);
	if(result != 0 )
	{
		PrintError("== CIMAX_PowerOffCam Return Error 2(%d) ==\r\n",result);
		return result;
	}

	VK_TASK_Sleep(CIMAX_SLEEP_SMALL_TIME);

	wdata = PCMCIA_LOCK;
	result  = CIMAX_WriteRegister(CFG_2,&wdata,1);
	if(result != 0 )
	{
		PrintError("== CIMAX_PowerOffCam Return Error 3(%d) ==\r\n",result);
		return result;
	}


 	return 0;
}


HINT32 CIMAX_SetCLK(HUINT8 slotId, HUINT32 flagOn)
{
	HUINT8	rdata = 0, wdata = 0;
	HINT32 result = 0;

	if(slotId >= CIMAX_CAM_MAX_SLOT_NUM)
	{
		return -1;
	}

	if( flagOn == 1)
	{
		rdata = 0;
		wdata = 0;
		result	= CIMAX_ReadRegister(CkMan_Config,&rdata,1);
		if(result != 0 )
		{
			PrintError("== CIMAX_SetCLK Return Error 1(%d) ==\r\n",result);
			return result;
		}

		wdata = rdata;

		if(slotId == 0)
		{
			if(rdata & EN_SER0_CLK)
			{
				return result;
			}
			else
			{
			wdata |= EN_SER0_CLK;
			}

		}
		else
		{
			if(rdata & EN_SER1_CLK)
			{
				return result;
			}
			else
			{
				wdata |= EN_SER1_CLK;
			}

		}

		result	= CIMAX_WriteRegister(CkMan_Config,&wdata,1);
		if(result != 0 )
		{
			PrintError("== CIMAX ClK Return Error 2(%d) ==\r\n",result);
			return result;
		}
	}
	else
	{
		rdata = 0;
		wdata = 0;
		result	= CIMAX_ReadRegister(CkMan_Config,&rdata,1);
		if(result != 0 )
		{
			PrintError("== CIMAX_PowerOffCam Return Error 3(%d) ==\r\n",result);
			return result;
		}

		wdata = rdata;

		if(slotId == 0)
		{
			if(rdata & EN_SER0_CLK)
			{
				wdata &= ~(EN_SER0_CLK);
			}
			else
			{
				return result;
			}
		}
		else
		{
			if(rdata & EN_SER1_CLK)
			{
				wdata &= ~(EN_SER1_CLK);
			}
			else
			{
				return result;
			}
		}

		result  = CIMAX_WriteRegister(CkMan_Config,&wdata,1);
		if(result != 0 )
		{
			PrintError("== CIMAX_SetCLK Return Error 4(%d) ==\r\n",result);
			return result;
		}

	}

	return result;

}

HINT32 CIMAX_ResetFIFO(HUINT8 outputId)
{
    HINT32 result = 0;
	HUINT8    rdata = 0, wdata = 0,tmpin = 0,savein = 0,cmp = 0;

    if(outputId >= CIMAX_CAM_MAX_SLOT_NUM)
    {
        PrintError("== CIMAX_ResetFIFO Slot is out of range (outputID=%d)\n", outputId);
        return -1;
    }

	if(outputId == 0)
	{
//		wdata = 0x40;
		wdata = FIFO_RESET_VALUE;
		result = CIMAX_WriteRegister(USB2TS0_RDL,&wdata, 1);
        if(result != 0)
        {
            PrintError("== CIMAX_ResetFIFO Reg. read error at line:%ld (%d) ==\r\n",__LINE__,result);
            return result;
        }

//		wdata = 0x03;
		wdata = (USBTS2_ENABLE|USBTS2_RESET);
		result = CIMAX_WriteRegister(USB2TS_CTRL,&wdata, 1);
        if(result != 0)
        {
            PrintError("== CIMAX_ResetFIFO Reg. read error at line:%ld (%d) ==\r\n",__LINE__,result);
            return result;
        }
	}
	else
	{
//		wdata = 0x40;
		wdata = FIFO_RESET_VALUE;
		result = CIMAX_WriteRegister(USB2TS1_RDL,&wdata, 1);
        if(result != 0)
        {
            PrintError("== CIMAX_ResetFIFO Reg. read error at line:%ld (%d) ==\r\n",__LINE__,result);
            return result;
        }
//		wdata = 0x0C;
		wdata = (USBTS2_ENABLE|USBTS2_RESET);
		wdata <<= SHIFT_IN_OF_BLOCK;
		result = CIMAX_WriteRegister(USB2TS_CTRL,&wdata, 1);
        if(result != 0)
        {
            PrintError("== CIMAX_ResetFIFO Reg. read error at line:%ld (%d) ==\r\n",__LINE__,result);
            return result;
        }
	}

    //Disconnect Input Source
    rdata = 0;
    result    = CIMAX_ReadRegister(IN_SEL,&rdata,1);
    if(result != 0 )
    {
        PrintError("== CIMAX_ResetFIFO Return Error 2(%d) ==\r\n",result);
        return result;
    }

    tmpin = rdata;
    savein = rdata;

    if(outputId == 0)
    {
        tmpin &= ~(CH0_SRC_MASK);
//		tmpin |= 0x02; //Use data from USB input instead setting NULL.
		tmpin |= (CH_IN_USBENDPOINT); //Use data from USB input instead setting NULL.
    }
    else
    {
        tmpin &= ~(CH1_SRC_MASK);
//		tmpin |= 0x20;
		cmp = CH_IN_USBENDPOINT;
		cmp <<= CH_IN_SHIFT;
		tmpin |= cmp;
    }

    result    = CIMAX_WriteRegister(IN_SEL,&tmpin,1);
    if(result != 0 )
    {
        PrintError("== CIMAX_ResetFIFO Return Error 3(%d) ==\r\n",result);
        return result;
    }

    //Reset P2S, S2P, Sync-Retriever
    if(outputId == 0)
    {
        rdata = 0;
        result    = CIMAX_ReadRegister(S2P_CH0_CTRL,&rdata,1);
        if(result != 0)
        {
            PrintError("== CIMAX_ResetFIFO Reg. read error at line:%ld (%d) ==\r\n",__LINE__,result);
            return result;
        }
        rdata |= 0x01;
        result    = CIMAX_WriteRegister(S2P_CH0_CTRL,&rdata,1);
        if(result != 0 )
        {
            PrintError("== CIMAX_ResetFIFO Reg. write error at line:%ld (%d) ==\r\n",__LINE__,result);
            return result;
        }
        result    = CIMAX_ReadRegister(SYNC_RTV_CTRL,&rdata,1);
        if(result != 0)
        {
            PrintError("== CIMAX_ResetFIFO Reg. read error at line:%ld (%d) ==\r\n",__LINE__,result);
            return result;
        }
        rdata |= 0x01;
        result    = CIMAX_WriteRegister(SYNC_RTV_CTRL,&rdata,1);
        if(result != 0 )
        {
            PrintError("== CIMAX_ResetFIFO Reg. write error at line:%ld (%d) ==\r\n",__LINE__,result);
            return result;
        }

    }
    else
    {
        rdata = 0;
        result    = CIMAX_ReadRegister(S2P_CH1_CTRL,&rdata,1);
        if(result != 0)
        {
            PrintError("== CIMAX_ResetFIFO Reg. read error at line:%ld (%d) ==\r\n",__LINE__,result);
            return result;
        }
        rdata |= 0x01;
        result    = CIMAX_WriteRegister(S2P_CH1_CTRL,&rdata,1);
        if(result != 0 )
        {
            PrintError("== CIMAX_ResetFIFO Reg. write error at line:%ld (%d) ==\r\n",__LINE__,result);
            return result;
        }
        result    = CIMAX_ReadRegister(SYNC_RTV_CTRL,&rdata,1);
        if(result != 0)
        {
            PrintError("== CIMAX_ResetFIFO Reg. read error at line:%ld (%d) ==\r\n",__LINE__,result);
            return result;
        }
        rdata |= 0x04;
        result    = CIMAX_WriteRegister(SYNC_RTV_CTRL,&rdata,1);
        if(result != 0 )
        {
            PrintError("== CIMAX_ResetFIFO Reg. write error at line:%ld (%d) ==\r\n",__LINE__,result);
            return result;
        }
    }

    //Reset FIFO
    rdata = 0;
    result    = CIMAX_ReadRegister(FIFO_CTRL,&rdata,1);
    if(result != 0)
    {
        PrintError("== CIMAX_ResetFIFO Return Error 6(%d) ==\r\n",result);
        return result;
    }


    if(outputId == 0)
    {
        rdata |= (RSTF_3|RSTF_1);

    }
    else
    {
        rdata |= (RSTF_4|RSTF_2);
    }

    wdata = rdata;

    result    = CIMAX_WriteRegister(FIFO_CTRL,&wdata,1);
    if(result != 0 )
    {
        PrintError("== CIMAX_ResetFIFO Return Error 7(%d) ==\r\n",result);
        return result;
    }


    //Reset P2S, S2P, Sync-Retriever
    if(outputId == 0)
    {
        rdata = 0;
#if defined(USE_GAP_REMOVER)
        result    = CIMAX_ReadRegister(GAP_REMOVER_CH0_CTRL,&rdata,1);
        if(result != 0)
        {
            PrintError("== CIMAX_ResetFIFO Reg. read error at line:%ld (%d) ==\r\n",__LINE__,result);
            return result;
        }
//        rdata |= 0x01;
        rdata |= (RST);
        result    = CIMAX_WriteRegister(GAP_REMOVER_CH0_CTRL,&rdata,1);
        if(result != 0 )
        {
            PrintError("== CIMAX_ResetFIFO Reg. write error at line:%ld (%d) ==\r\n",__LINE__,result);
            return result;
        }

        rdata = 0;
#endif
        result    = CIMAX_ReadRegister(P2S_CH0_CTRL,&rdata,1);
        if(result != 0)
        {
            PrintError("== CIMAX_ResetFIFO Reg. read error at line:%ld (%d) ==\r\n",__LINE__,result);
            return result;
        }
//        rdata |= 0x01;
        rdata |= (RST);
        result    = CIMAX_WriteRegister(P2S_CH0_CTRL,&rdata,1);
        if(result != 0 )
        {
            PrintError("== CIMAX_ResetFIFO Reg. write error at line:%ld (%d) ==\r\n",__LINE__,result);
            return result;
        }
    }
    else
    {
#if defined(USE_GAP_REMOVER)
        rdata = 0;
        result    = CIMAX_ReadRegister(GAP_REMOVER_CH1_CTRL,&rdata,1);
        if(result != 0)
        {
            PrintError("== CIMAX_ResetFIFO Reg. read error at line:%ld (%d) ==\r\n",__LINE__,result);
            return result;
        }
//        rdata |= 0x01;
        rdata |= (RST);
        result    = CIMAX_WriteRegister(GAP_REMOVER_CH1_CTRL,&rdata,1);
        if(result != 0 )
        {
            PrintError("== CIMAX_ResetFIFO Reg. write error at line:%ld (%d) ==\r\n",__LINE__,result);
            return result;
        }
#endif

        rdata = 0;
        result    = CIMAX_ReadRegister(P2S_CH1_CTRL,&rdata,1);
        if(result != 0)
        {
            PrintError("== CIMAX_ResetFIFO Reg. read error at line:%ld (%d) ==\r\n",__LINE__,result);
            return result;
        }
//        rdata |= 0x01;
        rdata |= (RST);
        result    = CIMAX_WriteRegister(P2S_CH1_CTRL,&rdata,1);
        if(result != 0 )
        {
            PrintError("== CIMAX_ResetFIFO Reg. write error at line:%ld (%d) ==\r\n",__LINE__,result);
            return result;
        }
    }

    //Reset FIFO
    rdata = 0;
    result    = CIMAX_ReadRegister(FIFO_CTRL,&rdata,1);
    if(result != 0)
    {
        PrintError("== CIMAX_ResetFIFO Return Error 6(%d) ==\r\n",result);
        return result;
    }


    if(outputId == 0)
    {
        rdata |= (RSTF_3|RSTF_1);

    }
    else
    {
        rdata |= (RSTF_4|RSTF_2);
    }

    wdata = rdata;

    result    = CIMAX_WriteRegister(FIFO_CTRL,&wdata,1);
    if(result != 0 )
    {
        PrintError("== CIMAX_ResetFIFO Return Error 7(%d) ==\r\n",result);
        return result;
    }


    if(outputId == 0)
    {
//		wdata = 0x48;
		wdata = (STRT|EN_P2S);
		result = CIMAX_WriteRegister(P2S_CH0_CTRL,&wdata, 1);
        if(result != 0)
        {
            PrintError("== CIMAX_ResetFIFO Reg. read error at line:%ld (%d) ==\r\n",__LINE__,result);
            return result;
        }
    }
	else
    {
//		wdata = 0x48;
		wdata = (STRT|EN_P2S);
		result = CIMAX_WriteRegister(P2S_CH1_CTRL,&wdata, 1);
        if(result != 0)
        {
            PrintError("== CIMAX_ResetFIFO Reg. read error at line:%ld (%d) ==\r\n",__LINE__,result);
            return result;
        }
    }

#if defined(USE_GAP_REMOVER)

	wdata = DIV_VALUE;
	wdata <<= 2;
	wdata |= ENABLE;
    if(outputId == 0)
    {
//		wdata = 0x26;
		result = CIMAX_WriteRegister(GAP_REMOVER_CH0_CTRL,&wdata, 1);
        if(result != 0)
        {
            PrintError("== CIMAX_ResetFIFO Reg. read error at line:%ld (%d) ==\r\n",__LINE__,result);
            return result;
        }
    }
	else
    {
//		wdata = 0x26;
		result = CIMAX_WriteRegister(GAP_REMOVER_CH1_CTRL,&wdata, 1);
        if(result != 0)
        {
            PrintError("== CIMAX_ResetFIFO Reg. read error at line:%ld (%d) ==\r\n",__LINE__,result);
            return result;
        }
    }
#endif
    rdata = 0;
    result    = CIMAX_ReadRegister(SYNC_RTV_CTRL,&rdata,1);
    if(result != 0)
    {
        PrintError("== CIMAX_ResetFIFO Reg. read error at line:%ld (%d) ==\r\n",__LINE__,result);
        return result;
    }

    if(outputId == 0)
    {
//        rdata |= 0x02;
        rdata |= (EN_SYNC_0);

    }
    else
    {
//        rdata |= 0x08;
        rdata |= (EN_SYNC_1);
    }

    wdata = rdata;

    result    = CIMAX_WriteRegister(SYNC_RTV_CTRL,&wdata,1);
    if(result != 0 )
    {
        PrintError("== CIMAX_ResetFIFO Reg. write error at line:%ld (%d) ==\r\n",__LINE__,result);
        return result;
    }


//	wdata = 0x22;
	wdata = (SER_EN_CH|ICE);
    if(outputId == 0)
    {
		result = CIMAX_WriteRegister(S2P_CH0_CTRL,&wdata, 1);
        if(result != 0)
        {
            PrintError("== CIMAX_ResetFIFO Reg. read error at line:%ld (%d) ==\r\n",__LINE__,result);
            return result;
        }
    }
	else
    {
		result = CIMAX_WriteRegister(S2P_CH1_CTRL,&wdata, 1);
        if(result != 0)
        {
            PrintError("== CIMAX_ResetFIFO Reg. read error at line:%ld (%d) ==\r\n",__LINE__,result);
            return result;
        }
    }


    wdata = 0;
    if(outputId == 0)
    {
        wdata |= P2S0_CLK | RDF3_CLK | WRF3_CLK;
    }
    else
    {
        wdata |= P2S1_CLK | RDF4_CLK | WRF4_CLK;
    }

    result = CIMAX_WriteRegister(RESET_CLK_SWITCH_0,&wdata,1);
    if(result != 0)
    {
        PrintError("== CIMAX_ResetFIFO Return Error 4(%d) ==\r\n",result);
        return result;
    }

    wdata = 0;
    if(outputId == 0)
    {
        wdata |= MICLK0 | SER0_CLK;
    }
    else
    {
        wdata |= MICLK1 | SER1_CLK;
    }

    result = CIMAX_WriteRegister(RESET_CLK_SWITCH_1,&wdata,1);
    if(result != 0)
    {
        PrintError("== CIMAX_ResetFIFO Return Error 5(%d) ==\r\n",result);
        return result;
    }

	if(outputId == 0)
	{
//		wdata = 0x01;
		wdata = USBTS2_RESET;
		result = CIMAX_WriteRegister(USB2TS_CTRL,&wdata, 1);
        if(result != 0)
        {
            PrintError("== CIMAX_ResetFIFO Reg. read error at line:%ld (%d) ==\r\n",__LINE__,result);
            return result;
        }
	}
	else
	{
//		wdata = 0x04;
		wdata = USBTS2_RESET;
		wdata <<= SHIFT_IN_OF_BLOCK;
		result = CIMAX_WriteRegister(USB2TS_CTRL,&wdata, 1);
        if(result != 0)
        {
            PrintError("== CIMAX_ResetFIFO Reg. read error at line:%ld (%d) ==\r\n",__LINE__,result);
            return result;
        }
	}

    result    = CIMAX_WriteRegister(IN_SEL,&savein,1);
    if(result != 0)
    {
        PrintError("== CIMAX_ResetFIFO Return Error 11(%d) ==\r\n",result);
        return result;
    }

	PrintDebug("==== ResetFIFO END (%d) ====\r\n",outputId);

	return result;

}

DRV_Error P_GetICInputInfo(HUINT32 *pInputLogicalLimit, HUINT32 *pInputPhisicalLimit)
{

	if((pInputLogicalLimit == NULL)||(pInputPhisicalLimit == NULL))
	{
		return DRV_ERR_INVALID_PARAMETER;
	}

	*pInputLogicalLimit = (CAM_MAXSLOTID * USED_LIVE +  CAM_MAXSLOTID * USED_PLAYBACK + CAM_MAXSLOTID * USED_SLOTOUTPUT);
	*pInputPhisicalLimit = (CAM_MAXSLOTID * USED_LIVE +  CAM_MAXSLOTID * USED_PLAYBACK );

	return DRV_OK;
}

HUINT8 CIMAX_ProcessNormalResponse( HUINT8 *data, HUINT16 numPayload, HUINT8 *buffer )
{
	HUINT8 result  = 0,resCmd = 0;

	if(data == NULL)
	{
		return result;
	}

	resCmd = (data[CIMAX_REGISTER_CMD_POS]&CIMAX_CAM_TYP_CMD_MASK);

	switch(resCmd)
	{
		case CIMAX_CAM_A_RESET_OK:
			if((data[CIMAX_CAM_PACKET_COUNTER_POS]		!= CIMAX_CAM_PACKET_COUNTER_VALUE) ||
				(data[CIMAX_CAM_DATA_LEN_MSB_POS]		!= 0x00) ||
				(data[CIMAX_CAM_DATA_LEN_LSB_POS]		!= 0x00))
			{
				result = 1 ;
			}
			else
			{
				result  = 0; 	/* no error */
			}
			break;
		case CIMAX_CAM_A_GET_CIS_OK:
			if(data[CIMAX_CAM_PACKET_COUNTER_POS] != CIMAX_CAM_PACKET_COUNTER_VALUE)
			{
				result = 1 ;
			}
			else
			{
				if((numPayload > 0)&&(buffer != NULL))
				{
					result  = 0; 	/* no error */
					VK_memcpy(buffer,&data[CIMAX_CAM_HEADER_SIZE],numPayload);
				}
				else
				{
					result  = 1;
				}
			}
			break;
		case CIMAX_CAM_A_WRITE_COR_OK:
			if((data[CIMAX_CAM_PACKET_COUNTER_POS]	   == CIMAX_CAM_PACKET_COUNTER_VALUE) &&
			(data[CIMAX_CAM_DATA_LEN_MSB_POS]	   == 0x00) &&
			(data[CIMAX_CAM_DATA_LEN_LSB_POS]	   == 0x00))
			{
				result	= 0;	/* no error */
			}
			else
			{
				result = 1 ;
			}
			break;
		case CIMAX_CAM_A_NEGOTIATE_OK:
			if((data[CIMAX_CAM_PACKET_COUNTER_POS]	   == CIMAX_CAM_PACKET_COUNTER_VALUE) &&
			(data[CIMAX_CAM_DATA_LEN_MSB_POS]	   == 0x00) &&
			(data[CIMAX_CAM_DATA_LEN_LSB_POS]	   == 0x02))
			{
				if((numPayload > 0)&&(buffer != NULL))
				{
					result	= 0;	/* no error */
					VK_memcpy(buffer,&data[CIMAX_CAM_FIRST_DATA_BYTE_POS],2);
				}
				else
				{
					result  = 1;
				}

			}
			else
			{
				result = 1 ;
			}
			break;
		case CIMAX_CAM_A_WRITE_LPDU_OK:
			if((data[CIMAX_CAM_PACKET_COUNTER_POS]	   == CIMAX_CAM_PACKET_COUNTER_VALUE) &&
			(data[CIMAX_CAM_DATA_LEN_MSB_POS]	   == 0x00) &&
			(data[CIMAX_CAM_DATA_LEN_LSB_POS]	   == 0x00))
			{
				result  = 0; 	/* no error */
			}
			else
			{
				result = 1 ;
			}
			break;
		case CIMAX_CAM_A_READ_LPDU_OK:
			if(data[CIMAX_CAM_PACKET_COUNTER_POS]		!= CIMAX_CAM_PACKET_COUNTER_VALUE)
			{
				result = 1 ;
			}
			else
			{
				if(buffer != NULL)
				{
					if(numPayload > 0)	VK_memcpy(buffer,&data[CIMAX_CAM_HEADER_SIZE],numPayload);
					result  = 0; 	/* no error */
				}
				else
				{
					result  = 1;
				}
			}
			break;
		case CIMAX_CAM_A_REG_STATUS_OK:
			if((data[CIMAX_CAM_PACKET_COUNTER_POS]		!= CIMAX_CAM_PACKET_COUNTER_VALUE) ||
			(data[CIMAX_CAM_DATA_LEN_MSB_POS]		!= 0x00) ||
			(data[CIMAX_CAM_DATA_LEN_LSB_POS]		!= 0x01))
			{
				result = 1 ;
			}
			else
			{
				if((numPayload > 0)&&(buffer != NULL))
				{
					VK_memcpy(buffer,&data[CIMAX_CAM_HEADER_SIZE],numPayload);
					result  = 0; 	/* no error */
				}
				else
				{
					result  = 1;
				}
			}
			break;
		case 0x4b:
			if((data[CIMAX_CAM_PACKET_COUNTER_POS]		== 0x00) &&
			(data[CIMAX_CAM_DATA_LEN_MSB_POS]		== 0x00) &&
			(data[CIMAX_CAM_DATA_LEN_LSB_POS]		== 0x00))
			{
				result = 0;	/* no error */
			}
			else
			{
				result = 1;
			}
			break;

		case CIMAX_CAM_A_DATA_READY:
			result = 0; /* no error */
			break;

		default:
			result = 1 ;
			break;
	}

	return result;
}

/**
* @ingroup     CIMaX_Statics
*
* @brief       Checks if CIMaX+ response is CAMNoCam message.
*
* @param       slotId   CAM slot id.
*
* @return      TRUE if response is CAMNoCam, otherwise FALSE.
*/
HUINT32 responseCamNoCam( HUINT8 slotId, HUINT8 *buffer)
{
	HUINT8	camStatus = CIMAX_CAM_A_NO_CAM;

	/* Check if response is valid. */
	if(slotId != CIMAX_CAM_SLOT_A_ID)
	{
		camStatus = CIMAX_CAM_B_NO_CAM;
	}

	if ((buffer[CIMAX_CAM_COMMAND_OR_STATUS_POS] == camStatus) &&
	(buffer[CIMAX_CAM_PACKET_COUNTER_POS]     == CIMAX_CAM_PACKET_COUNTER_VALUE) &&
	(buffer[CIMAX_CAM_DATA_LEN_MSB_POS]       == 0x00) &&
	(buffer[CIMAX_CAM_DATA_LEN_LSB_POS]       == 0x00))
	{
		return TRUE;
	}

	return FALSE;
}


/**
* @ingroup     CIMaX_Statics
*
* @brief       Checks if CIMaX+ response is CAMError message.
*
* @param       slotId   CAM slot id.
*
* @return      TRUE if response is CAMError, otherwise FALSE.
*/
HUINT32 responseCamError( HUINT8 slotId, HUINT8 *buffer)
{
	HUINT8    camStatus = CIMAX_CAM_A_CAM_ERROR;
	HUINT16   receivedErrorNumber = 0;

	/* Check if response is valid. */
	if(slotId != CIMAX_CAM_SLOT_A_ID)
	{
		camStatus = CIMAX_CAM_B_CAM_ERROR;
	}

	if ((buffer[CIMAX_CAM_COMMAND_OR_STATUS_POS] == camStatus) &&
	(buffer[CIMAX_CAM_PACKET_COUNTER_POS]     == CIMAX_CAM_PACKET_COUNTER_VALUE) &&
	(buffer[CIMAX_CAM_DATA_LEN_MSB_POS]       == 0x00) &&
	((buffer[CIMAX_CAM_DATA_LEN_LSB_POS] 	  == 0x02)||(buffer[CIMAX_CAM_DATA_LEN_LSB_POS] == 0x01))
	)
	{
		receivedErrorNumber = buffer[CIMAX_CAM_FIRST_DATA_BYTE_POS] & 0x0F;
		receivedErrorNumber = receivedErrorNumber << 8;
		receivedErrorNumber |= buffer[CIMAX_CAM_SECOND_DATA_BYTE_POS];

		PrintDebug("%s: CAMError %d received: ", __FUNCTION__, slotId);

		switch(receivedErrorNumber)
		{
			case CAMERROR_RESET:
				PrintError("CAM not ready Slot(%d)!!!!\n",slotId);
			break;

			case CAMERROR_CIS_BUFFER:
				PrintError("CIS buffer not allocated Slot(%d)!!!!\n",slotId);
			break;

			case CAMERROR_CIS_BAD_SIZE:
				PrintError("Bad CIS size (> 512 bytes) Slot(%d)!!!!\n",slotId);
			break;

			case CAMERROR_CIS_CAM_NOT_ACTIVATED:
				PrintError("CAM not activated! Slot(%d)!!!!\n",slotId);
			break;

			case CAMERROR_COR_CAM_NOT_READY:
				PrintError("CAM not ready after software reset during write COR process Slot(%d)!!!!\n",slotId);
			break;

			case CAMERROR_COR_VALUE_CHECKING:
				PrintError("Checking of COR value failed Slot(%d)!!!!\n",slotId);
			break;

			case CAMERROR_NEGOTIATE_NOT_RESPONDING:
				PrintError("CAM not responding after SR bit is raised during buffer negociation phase Slot(%d)!!!!\n",slotId);
			break;

			case CAMERROR_NEGOTIATE_SIZE_LENGTH:
				PrintError("CAM buffer size length <> 2 Slot(%d)!!!!\n",slotId);
			break;

			case CAMERROR_NEGOTIATE_NOT_READY:
				PrintError("CAM not ready to accept new buffer size negotiate during buffer negociation phase Slot(%d)!!!!\n",slotId);
			break;

			case CAMERROR_LPDU_NOT_AVAILABLE:
				PrintError("LPDU not available  Slot(%d)!!!!\n",slotId);
			break;

			default:
				PrintError("unknown error 0x%04x! (%x) Slot(%d)=== \n", receivedErrorNumber,buffer[CIMAX_CAM_DATA_LEN_LSB_POS],slotId);
			break;
		}

		return TRUE;
	}

	return FALSE;

}

/**
* @ingroup     CIMaX_Statics
*
* @brief       Checks if CIMaX+ response is CAMWriteBusy message.
*
* @param       slotId   CAM slot id.
*
* @return      TRUE if response is CAMWriteBusy, otherwise FALSE.
*/
HUINT32 responseCamWriteBusy( HUINT8 slotId, HUINT8 *buffer)
{
	HUINT8    camStatus = CIMAX_CAM_A_WRITE_BUSY;

	/* Check if response is valid. */
	if(slotId != CIMAX_CAM_SLOT_A_ID)
	{
		camStatus = CIMAX_CAM_B_WRITE_BUSY;
	}

	if ((buffer[CIMAX_CAM_COMMAND_OR_STATUS_POS] == camStatus) &&
	(buffer[CIMAX_CAM_PACKET_COUNTER_POS]     == CIMAX_CAM_PACKET_COUNTER_VALUE) &&
	(buffer[CIMAX_CAM_DATA_LEN_MSB_POS]       == 0x00) &&
	(buffer[CIMAX_CAM_DATA_LEN_LSB_POS]       == 0x00))
	{
		return TRUE;
	}

	return FALSE;
}

/**
* @ingroup     CIMaX_Statics
*
* @brief       Checks if CIMaX+ response is CAMCmdPending message.
*
* @param       slotId   CAM slot id.
*
* @return      TRUE if response is CAMCmdPending, otherwise FALSE.
*/
HUINT32 responseCamCmdPending( HUINT8 slotId, HUINT8 *buffer)
{
	HUINT8    camStatus = CIMAX_CAM_A_CMD_PENDING;

	/* Check if response is valid. */
	if(slotId != CIMAX_CAM_SLOT_A_ID)
	{
		camStatus = CIMAX_CAM_B_CMD_PENDING;
	}

	if ((buffer[CIMAX_CAM_COMMAND_OR_STATUS_POS] == camStatus) &&
	(buffer[CIMAX_CAM_PACKET_COUNTER_POS]     == CIMAX_CAM_PACKET_COUNTER_VALUE) &&
	(buffer[CIMAX_CAM_DATA_LEN_MSB_POS]       == 0x00) &&
	(buffer[CIMAX_CAM_DATA_LEN_LSB_POS]       == 0x01))
	{
		return TRUE;
	}

	return FALSE;
}


HINT32 CIMAX_NormalCommand( HUINT8 slotId, HUINT8 cmd, HUINT8 *buffer, HUINT16 numOfBytes,HUINT16 *pNumOfBytes )
{
	HINT32 vkErr = 0;
	DRV_Error	drvErr = 0;
	HUINT32		totalLen = 0;
	HUINT8		transcmd = 0,transmode = 0,checkres = 0;
	HUINT16		numPayload = 0;
	HUINT8 	msgData[4] = {0,};
	HUINT8 jcount = 0;

#if defined(CONFIG_EXT_CI_CTRL_INT_MODE)
	HINT32 retVal = 0;
#else
	HUINT8 jcount1 = 0,jcount2 = 0,jcount3 = 0;
	HULONG		ulEvent = 0;
	HUINT16	dataLen = 0;
	HUINT8	evtSlotId, evtStatus;
	int 	evttrycnt=0;

	HUINT8	invalidflag = 0;
	HUINT32		index = 0;
#endif
	if (slotId >= CIMAX_CAM_MAX_SLOT_NUM)
	{
		PrintError("== NC Return Error (%d) ==\r\n",slotId);
		return CIMAX_ERROR;
	}

	VK_SEM_Get(s_ulCiSemId);

#if defined(CONFIG_EXT_CI_CTRL_INT_MODE)
#else
	while(P_CIMAX_GetIntPinLevel() == 0)
	{
		VK_printf("%s() read Event (%dth)\n", __func__, evttrycnt);
		CIMAX_GetEventCommand( &evtSlotId, &evtStatus);
		VK_printf("event received. slotd=%d evt=%d\n", evtSlotId, evtStatus);
		evttrycnt++;
	}
#endif
ASEND:
	if(slotId != CIMAX_CAM_SLOT_A_ID)
	{
		transcmd = cmd |CIMAX_CAM_SLOT_MASK;
	}
	else
	{
		transcmd = cmd;
	}

	VK_MEM_Memset(s_cimaxBufferOut, 0, sizeof(s_cimaxBufferOut));
#if defined(CONFIG_EXT_CI_CTRL_INT_MODE)
#else
	VK_MEM_Memset(s_cimaxBufferIn, 0, sizeof(s_cimaxBufferIn));
#endif
	s_cimaxBufferOut[CIMAX_REGISTER_CMD_POS]			= transcmd;
	if((transcmd == CIMAX_CAM_INIT_CMD)||(transcmd == (CIMAX_CAM_INIT_CMD|CIMAX_CAM_SLOT_MASK)))
		s_cimaxBufferOut[CIMAX_CAM_PACKET_COUNTER_POS]    = 0x00;
	else
		s_cimaxBufferOut[CIMAX_CAM_PACKET_COUNTER_POS]    = CIMAX_CAM_PACKET_COUNTER_VALUE;
	s_cimaxBufferOut[CIMAX_CAM_DATA_LEN_MSB_POS]	= (numOfBytes>>8) & 0xFF;
	s_cimaxBufferOut[CIMAX_CAM_DATA_LEN_LSB_POS]	= (numOfBytes & 0xFF);


	totalLen = CIMAX_REGISTER_HEADER_SIZE + numOfBytes;

	if((numOfBytes != 0)||(buffer != NULL))
	{
		VK_memcpy(&s_cimaxBufferOut[CIMAX_REGISTER_HEADER_SIZE], buffer, numOfBytes);
	}

	transmode = 0;

	drvErr = DRV_SPI_TransmitPacket(0,transmode, s_cimaxBufferOut, totalLen);
	if(drvErr != 0)
	{
		PrintError("== NC SPT 1(%d)(%x) ==\r\n",drvErr,transcmd);

		jcount++;

		if(jcount < CIMAX_RETRY_NUM)
		{
			VK_TASK_Sleep(CIMAX_SLEEP_MAX_TIME);
			PrintError("== NC SPT AG(%d:%d)(%x) ==\r\n",drvErr,jcount,transcmd);

			goto ASEND;
		}
		else
		{
			jcount = 0;
			s_cimaxNormalCommandErrorCount++;
			PrintError("== NC SPT E(%d :%d) ==\r\n",drvErr,s_cimaxNormalCommandErrorCount);
			VK_SEM_Release(s_ulCiSemId);
			return CIMAX_SPI_COMM_ERROR;
		}

	}

	jcount = 0;
#if defined(CONFIG_EXT_CI_CTRL_INT_MODE)
	retVal = CIMAX_GetResponse(GET_CI_EXT_CTRL_RESP);
	if (retVal < 0)
	{
		VK_SEM_Release(s_ulCiSemId);
		PrintError("[%s] CIMAX_GetResponse Error! \n", __func__);
		return retVal;
	}
	else if(retVal == CIMAX_CAM_RESPONSE_ERROR)
	{
		PrintError("[%s] ERROR! retry! \n", __func__);
		goto ASEND;
	}
	else
	{
		numPayload = retVal;
	}
#else
WAIT:
	vkErr = VK_EVENT_ReceiveTimeout(s_ulCiMaxEventId, &ulEvent,SPI_WAIT_TIMEOUT);
	if(vkErr != 0)
	{
		jcount1++;

		while(P_CIMAX_GetIntPinLevel() != 0)
		{
		if(jcount1 < CIMAX_RETRY_NUM )
		{
			VK_TASK_Sleep(CIMAX_SLEEP_MAX_TIME);
			PrintError("== NC TO AG(%d:%d)(%x) ==\r\n",drvErr,jcount1,transcmd);

				goto WAIT;
		}
		else
		{
			jcount1 = 0;
			s_cimaxNormalCommandErrorCount++;
			PrintError("== NC TO E(%d:%d)(%x)(%d) ==\r\n",drvErr,jcount1,transcmd,s_cimaxNormalCommandErrorCount);
			VK_SEM_Release(s_ulCiSemId);
			return CIMAX_SPI_COMM_ERROR;
		}
	}

	}

	jcount1 = 0;


AGAIN:
	totalLen = CIMAX_REGISTER_HEADER_SIZE;

	transmode = 0;

	drvErr	= DRV_SPI_ReceivePacket(0,transmode,s_cimaxBufferIn,totalLen);
	if(drvErr != 0)
	{
		jcount2++;

		if(jcount2 < CIMAX_RETRY_NUM)
		{
			VK_TASK_Sleep(CIMAX_SLEEP_MAX_TIME);
			PrintError("== NC SPR AG(%d:%d)(%x) ==\r\n",drvErr,jcount2,transcmd);
			goto ASEND;
		}
		else
		{
			jcount2 = 0;
			s_cimaxNormalCommandErrorCount++;
			PrintError("== NC SPR E(%d:%d)(%x)(%d) ==\r\n",drvErr,totalLen,transcmd,s_cimaxNormalCommandErrorCount);
			VK_SEM_Release(s_ulCiSemId);
			return CIMAX_SPI_COMM_ERROR;
		}
	}

	jcount2 = 0;

	switch(s_cimaxBufferIn[CIMAX_REGISTER_CMD_POS] & CIMAX_CAM_TYP_CMD_MASK)
	{
		case CIMAX_REGISTER_INIT_RESP_OK:   /* Fall through. */
		case CIMAX_REGISTER_INIT_RESP_NOK:  /* Fall through. */
		case CIMAX_REGISTER_WRITE_RESP_OK:  /* Fall through. */
		case CIMAX_REGISTER_WRITE_RESP_NOK: /* Fall through. */
		case CIMAX_REGISTER_READ_RESP_OK:   /* Fall through. */
		case CIMAX_REGISTER_READ_RESP_NOK:  /* Fall through. */
		case CIMAX_REGISTER_CMD_PARSE_ERROR:
			/* Read register response payload if needed. */
			numPayload = s_cimaxBufferIn[CIMAX_CAM_DATA_LEN_MSB_POS];
			if ((s_cimaxBufferIn[CIMAX_REGISTER_NUM_OF_REGS_POS] > 0)&&(s_cimaxBufferIn[CIMAX_REGISTER_NUM_OF_REGS_POS] < 0xff))
			{
				drvErr	= DRV_SPI_ReceivePacket(0,transmode,&s_cimaxBufferIn[CIMAX_REGISTER_HEADER_SIZE],s_cimaxBufferIn[CIMAX_REGISTER_NUM_OF_REGS_POS]);
				if(drvErr != 0)
				{
					s_cimaxNormalCommandErrorCount++;
					PrintError("== NC R1 (%x)(%x:%x:%x:%x:%x)(%d)####==\r\n",drvErr,s_cimaxBufferIn[0],s_cimaxBufferIn[1],s_cimaxBufferIn[2],s_cimaxBufferIn[3],s_cimaxBufferIn[4],s_cimaxNormalCommandErrorCount);
					VK_SEM_Release(s_ulCiSemId);
					return CIMAX_SPI_COMM_ERROR;
				}

			}
			break;

		case CIMAX_CAM_A_RESET_OK:             /* or CIMAX_CAM_B_RESET_OK */
			PrintDebug("%s(%d)[RESET_OK]  check GPIO_INT = %d\n",__func__, __LINE__, P_CIMAX_GetIntPinLevel());
		case CIMAX_CAM_A_GET_CIS_OK:           /* or CIMAX_CAM_B_GET_CIS_OK */
		case CIMAX_CAM_A_WRITE_COR_OK:         /* or CIMAX_CAM_B_WRITE_COR_OK */
		case CIMAX_CAM_A_NEGOTIATE_OK:         /* or CIMAX_CAM_B_NEGOTIATE_OK */
		case CIMAX_CAM_A_WRITE_LPDU_OK:        /* or CIMAX_CAM_B_WRITE_LPDU_OK */
		case CIMAX_CAM_A_READ_LPDU_OK:         /* or CIMAX_CAM_B_READ_LPDU_OK */
		case CIMAX_CAM_A_CAM_ERROR:            /* or CIMAX_CAM_B_CAM_ERROR */
		case CIMAX_CAM_A_WRITE_BUSY:           /* or CIMAX_CAM_B_WRITE_BUSY */
		case CIMAX_CAM_A_DET_OK:               /* or CIMAX_CAM_B_DET_OK */
		case CIMAX_CAM_A_DATA_READY:           /* or CIMAX_CAM_B_DATA_READY */
		case CIMAX_CAM_A_CMD_PENDING:          /* or CIMAX_CAM_B_CMD_PENDING */
		case CIMAX_CAM_A_REG_STATUS_OK:        /* or CIMAX_CAM_B_REG_STATUS_OK */
		case CIMAX_CAM_NO_EVT:
		case CIMAX_CAM_A_NO_CAM:			   /* or CIMAX_CAM_B_NO_CAM */
			/* Get data length for CAM response. */
			dataLen = s_cimaxBufferIn[CIMAX_CAM_DATA_LEN_MSB_POS];
			dataLen = (dataLen << 8);
			dataLen = dataLen | s_cimaxBufferIn[CIMAX_CAM_DATA_LEN_LSB_POS];

			/* Read CAM response payload if needed. */
			if((dataLen > 0)&&(dataLen < 65535))
			{
				drvErr = DRV_SPI_ReceivePacket(0,transmode,&s_cimaxBufferIn[CIMAX_CAM_HEADER_SIZE], dataLen);
				if(drvErr != 0)
				{
					s_cimaxNormalCommandErrorCount++;
					PrintError("== NC R2 (%x)(%x:%x:%x:%x:%x)(%d)####==\r\n",drvErr,s_cimaxBufferIn[0],s_cimaxBufferIn[1],s_cimaxBufferIn[2],s_cimaxBufferIn[3],s_cimaxBufferIn[4],s_cimaxNormalCommandErrorCount);
					VK_SEM_Release(s_ulCiSemId);
					return CIMAX_SPI_COMM_ERROR;
				}
			}
			numPayload = dataLen;
			break;
		default:
			PrintDebug("==== Command :: Transmit Data Start (%d) =======\r\n",totalLen);
			for( index = 0; index < totalLen; index++)
			{
				PrintDebug("  [0x%02x]",s_cimaxBufferOut[index]);
			}
			PrintDebug("\r\n");
			PrintDebug("=================================================\r\n");

			PrintDebug( "%s: ***** invalid response!:0x%02x 0x%02x 0x%02x 0x%02x. *****\n",
			__FUNCTION__,s_cimaxBufferIn[CIMAX_CAM_COMMAND_OR_STATUS_POS],
			s_cimaxBufferIn[CIMAX_CAM_PACKET_COUNTER_POS],
			s_cimaxBufferIn[CIMAX_CAM_DATA_LEN_MSB_POS],
			s_cimaxBufferIn[CIMAX_CAM_DATA_LEN_LSB_POS]);
			invalidflag = 1;
			break;
	}  /* switch (responseBuff[CIMAX_REGISTER_CMD_POS] & CIMAX_CAM_TYP_CMD_MASK) */


	if(invalidflag == 1)
	{
		jcount3++;
		if(jcount3 < CIMAX_RESET_RETRY)
		{
			invalidflag = 0;
			VK_TASK_Sleep(CIMAX_SLEEP_MAX_TIME);
			PrintDebug("== NC SPR E AG(%d:%d)(%x) ==\r\n",drvErr,jcount3,transcmd);
			goto AGAIN;
		}
		else
		{
			invalidflag = 0;
			jcount3 = 0;
			PrintDebug("== NC SPR E End(%d:%d)(%x) ==\r\n",drvErr,totalLen,transcmd);
		}
	}

	jcount3 = 0;
	invalidflag = 0;
#endif
	if ((slotId << CIMAX_CAM_SLOT_BIT_POSITION) != (s_cimaxBufferIn[CIMAX_CAM_COMMAND_OR_STATUS_POS] & CIMAX_CAM_SLOT_MASK))
	{
	   PrintDebug("%s: ***** slot MISMATCH (%d instead of %d)! *****\n", __FUNCTION__,
			 (s_cimaxBufferIn[CIMAX_CAM_COMMAND_OR_STATUS_POS] >> CIMAX_CAM_SLOT_BIT_POSITION), slotId);
	}


	/* Check response from CIMax+. */
	if (responseCamNoCam(slotId,s_cimaxBufferIn) == TRUE)
	{
		PrintError("%s: CAMNoCAM %d received!\n", __FUNCTION__, slotId);

		caminfo.usStatus[slotId] = DI_CI_EXTRACTED;
		caminfo.ucDetect[slotId] = CAM_NO_DETECT;

		msgData[0] = slotId;
		msgData[1] = ECOOMMAND_EXTRACT;
		vkErr = VK_MSG_Send(s_ulCiMaxMsgId, msgData, 4);

		VK_SEM_Release(s_ulCiSemId);
		return CIMAX_NO_CAM_ERROR;
	}

	if (responseCamError(slotId,s_cimaxBufferIn) == TRUE)
	{
		PrintError("%s: CAMError %d received!\n", __FUNCTION__, slotId);
		PrintDebug( "### Response ::0x%02x 0x%02x 0x%02x 0x%02x. (Slot = %d :: Commad (%x))*****\n",
		s_cimaxBufferIn[CIMAX_CAM_COMMAND_OR_STATUS_POS],
		s_cimaxBufferIn[CIMAX_CAM_PACKET_COUNTER_POS],
		s_cimaxBufferIn[CIMAX_CAM_DATA_LEN_MSB_POS],
		s_cimaxBufferIn[CIMAX_CAM_DATA_LEN_LSB_POS],slotId,transcmd);

		VK_SEM_Release(s_ulCiSemId);
		return CIMAX_CAM_ERROR;
	}

	if (responseCamWriteBusy(slotId,s_cimaxBufferIn) == TRUE)
	{
		PrintError("%s: CAMWriteBusy %d received!\n", __FUNCTION__, slotId);
		PrintDebug( "### Response ::0x%02x 0x%02x 0x%02x 0x%02x. (Slot = %d :: Commad (%x))*****\n",
		s_cimaxBufferIn[CIMAX_CAM_COMMAND_OR_STATUS_POS],
		s_cimaxBufferIn[CIMAX_CAM_PACKET_COUNTER_POS],
		s_cimaxBufferIn[CIMAX_CAM_DATA_LEN_MSB_POS],
		s_cimaxBufferIn[CIMAX_CAM_DATA_LEN_LSB_POS],slotId,transcmd);

		VK_SEM_Release(s_ulCiSemId);
		return CIMAX_CAM_WRITE_BUSY_ERROR;
	}

	if (responseCamCmdPending(slotId,s_cimaxBufferIn) == TRUE)
	{
		PrintError("%s: CAMCmdPending %d received!\n", __FUNCTION__, slotId);
		PrintDebug( "### Response ::0x%02x 0x%02x 0x%02x 0x%02x. (Slot = %d :: Commad (%x))*****\n",
		s_cimaxBufferIn[CIMAX_CAM_COMMAND_OR_STATUS_POS],
		s_cimaxBufferIn[CIMAX_CAM_PACKET_COUNTER_POS],
		s_cimaxBufferIn[CIMAX_CAM_DATA_LEN_MSB_POS],
		s_cimaxBufferIn[CIMAX_CAM_DATA_LEN_LSB_POS],slotId,transcmd);

		VK_SEM_Release(s_ulCiSemId);
		return CIMAX_CAM_CMD_PENDING;
	}

	checkres = CIMAX_ProcessNormalResponse(s_cimaxBufferIn,numPayload,buffer);
	if(checkres != 0)
	{
		s_cimaxNormalCommandErrorCount++;
		PrintError("== NC 7(%x)(%x)(%x:%x:%x:%x) (%d)####==\r\n",transcmd,checkres,s_cimaxBufferIn[0],s_cimaxBufferIn[1],s_cimaxBufferIn[2],s_cimaxBufferIn[3],s_cimaxNormalCommandErrorCount);
		VK_SEM_Release(s_ulCiSemId);
		return CIMAX_SPI_COMM_ERROR;
	}

	*pNumOfBytes = numPayload;

	VK_SEM_Release(s_ulCiSemId);

	jcount = 0;

	return CIMAX_NO_ERROR;
}

HINT32 CIMAX_GetEventCommand( HUINT8 *slotId, HUINT8 *evtStatus )
{
	HINT32 vkErr = 0;
	DRV_Error	drvErr = 0;

	HUINT8		bufferOut[CIMAX_CAM_HEADER_SIZE];
	HUINT32		totalLen = 0;
	HUINT8		transmode = 0,slot = 0,status = 0;
	HUINT16 	readNum = 0;
	HULONG		ulEvent = 0;


	PrintDebug("%s()\n", __func__);
	bufferOut[CIMAX_REGISTER_CMD_POS]			= CIMAX_CAM_GET_EVT_CMD;
	bufferOut[CIMAX_REGISTER_REG_ADDR_MSB_POS]	= 0x01;
	bufferOut[CIMAX_REGISTER_REG_ADDR_LSB_POS]	= 0x00;
	bufferOut[CIMAX_REGISTER_NUM_OF_REGS_POS]	= 00;

	totalLen = CIMAX_REGISTER_HEADER_SIZE;

	transmode  = 0;

	drvErr = DRV_SPI_TransmitPacket(0,transmode, bufferOut, totalLen);
	if(drvErr != 0)
	{
		PrintError("=== CIMAX_GetEventCommand error (%d) ==\r\n",drvErr);
		return -1;
	}

	vkErr = VK_EVENT_ReceiveTimeout(s_ulCiMaxEventId, &ulEvent,SPI_WAIT_TIMEOUT);
	if(vkErr != 0)
	{
		PrintError("=== CIMAX_GetEventCommand E1 (%d) ==\r\n",vkErr);
		return -1;
	}

	VK_TASK_Sleep(CIMAX_SLEEP_MIDDLE_TIME);

	VK_MEM_Memset(s_cimaxBufferIn, 0, sizeof(s_cimaxBufferIn));

	totalLen = CIMAX_REGISTER_HEADER_SIZE;

#if 0
	//block out to remove prevent issue
	if(!(totalLen%2))
		transmode = 16;
	else
		transmode = 8;
#endif

	transmode  = 0;

	drvErr = DRV_SPI_ReceivePacket(0,transmode, s_cimaxBufferIn, totalLen );
	if(drvErr != 0)
	{
		PrintError("=== CIMAX_GetEventCommand E2 (%d) ==\r\n",drvErr);
		return -1;
	}

	readNum = s_cimaxBufferIn[CIMAX_CAM_DATA_LEN_MSB_POS];
	readNum = (readNum << 8);
	readNum = readNum | s_cimaxBufferIn[CIMAX_CAM_DATA_LEN_LSB_POS];

	if(readNum > 0)
	{
		drvErr	= DRV_SPI_ReceivePacket(0,8,&s_cimaxBufferIn[CIMAX_CAM_HEADER_SIZE], readNum);
		if(drvErr != 0)
		{
			PrintError("=== CIMAX_GetEventCommand E3 (%d) ==\r\n",drvErr);
			return -1;
		}
	}

	status = s_cimaxBufferIn[CIMAX_REGISTER_CMD_POS] & CIMAX_CAM_TYP_CMD_MASK;

	switch(status)
	{
		case CIMAX_CAM_A_DET_OK: /* or CIMAX_CAM_B_DET_OK */
			slot = CIMAX_CAM_SLOT_A_ID;
			if (s_cimaxBufferIn[CIMAX_REGISTER_CMD_POS] & CIMAX_CAM_SLOT_MASK)
			{
			   slot ++;/* = CIMAX_CAM_SLOT_B_ID */
			}
			PrintDebug("[CIMAX_EVT] Insert/Remove event.\n");
			break;
		case CIMAX_CAM_A_DATA_READY: /* or CIMAX_CAM_B_DATA_READY */
			slot = CIMAX_CAM_SLOT_A_ID;
			if (s_cimaxBufferIn[CIMAX_REGISTER_CMD_POS] & CIMAX_CAM_SLOT_MASK)
			{
			   slot ++;/* = CIMAX_CAM_SLOT_B_ID */
			}
			PrintDebug("[CIMAX_EVT] DATA_RDY event.\n");
			break;
		case CIMAX_CAM_NO_EVT:
			PrintDebug("[CIMAX_EVT] no Event\n");
			break;
		default:
			PrintDebug("[CIMAX_EVT] default: status=%d\n", status); //GPIO change...
			break;
	}

	*evtStatus = s_cimaxBufferIn[CIMAX_REGISTER_CMD_POS];
	*slotId = slot;


	return 0;
}

HINT32 CIMAX_CheckCommandStatus(void)
{
	if((s_cimaxAcessCommandSPIErrorCount != 0 )||(s_cimaxNormalCommandErrorCount != 0))
	{
		PrintError("== CCS (%d:%d)####==\r\n",s_cimaxAcessCommandSPIErrorCount,s_cimaxNormalCommandErrorCount);
		return CIMAX_ERROR;
	}
	else
	{
		return CIMAX_NO_ERROR;

	}
}

void CIMAX_InitCommandStatus(void)
{
	s_cimaxAcessCommandSPIErrorCount = 0;
	s_cimaxNormalCommandErrorCount = 0;
	return;
}

HINT32 DRV_CIMAX_CAMReset(HUINT8 slotId)
{
	HINT32 ret = 0;
	HUINT16 retBytes = 0;

	if (slotId >= CIMAX_CAM_MAX_SLOT_NUM)
	{
		return CIMAX_INVALID_SLOT_ID_ERROR;
	}

	ret = CIMAX_CheckCommandStatus();
	if(ret != CIMAX_NO_ERROR)
	{
		return CIMAX_ERROR;
	}

	ret  = CIMAX_NormalCommand(slotId,CIMAX_CAM_RESET_CMD,NULL,0,&retBytes);

	return ret;
}

HINT32 DRV_CIMAX_GetCIS(HUINT8 slotId, HUINT8 *cisBuff, HUINT16 *cisLen)
{
	HINT32 ret = 0;
	HUINT16 retBytes = 0;

	if (slotId >= CIMAX_CAM_MAX_SLOT_NUM)
	{
		return CIMAX_INVALID_SLOT_ID_ERROR;
	}

	if(cisBuff == NULL)
	{
		return CIMAX_ERROR;
	}

	ret = CIMAX_CheckCommandStatus();
	if(ret != CIMAX_NO_ERROR)
	{
		return CIMAX_ERROR;
	}

	ret  = CIMAX_NormalCommand(slotId,CIMAX_CAM_GET_CIS_CMD,cisBuff,0,&retBytes);
	if(ret == 0)
	{
		*cisLen = retBytes;
	}
	else
	{
		*cisLen = 0;
	}

	return ret;
}


HINT32 DRV_CIMAX_WriteCOR(HUINT8 slotId, HUINT16 coraddr, HUINT8 cordata)
{

	HINT32 ret = 0;
	HUINT16	retBytes = 0;
	HUINT8	tmp[CIMAX_CAM_HEADER_SIZE+CIMAX_CAM_COR_PAYLOAD_SIZE+CIMAX_CAM_COR_ADD_PAYLOAD_SIZE];
	HUINT8	payloadSize = 0;

	if (slotId >= CIMAX_CAM_MAX_SLOT_NUM)
	{
		return CIMAX_INVALID_SLOT_ID_ERROR;
	}

	ret = CIMAX_CheckCommandStatus();
	if(ret != CIMAX_NO_ERROR)
	{
		return CIMAX_ERROR;
	}

	payloadSize = CIMAX_CAM_COR_PAYLOAD_SIZE;

	tmp[CIMAX_CAM_COR_ADDR_MSB_POS]	= coraddr >> 8;
	tmp[CIMAX_CAM_COR_ADDR_LSB_POS]	= coraddr & 0x00FF;
	tmp[CIMAX_CAM_COR_VALUE_POS]	= cordata;
	tmp[CIMAX_CAM_COR_LAST_MSB_POS]	= 0x00;
	tmp[CIMAX_CAM_COR_LAST_LSB_POS]	= 0x00;
	tmp[5]			= 0x00;
#if defined(CONFIG_EXT_CI_CTRL_INT_MODE)
	tmp[6]			= 0x41;							/* CIMAX interrupt mode	*/
#else
	tmp[6]			= 0x40;							/* CIMAX polling mode		*/
#endif
	PrintDebug("=== DRV_CIMAX_WriteCOR (%x:%x:%x:%x:%x:%x:%x) ===\r\n",tmp[0],tmp[1],tmp[2],tmp[3],tmp[4],tmp[5],tmp[6]);

	ret  = CIMAX_NormalCommand(slotId,CIMAX_CAM_WRITE_COR_CMD,tmp,payloadSize+2,&retBytes);
	return ret;

}

HINT32 DRV_CIMAX_DataAvailable(HUINT8  slotId, HINT32 *result)
{
	HINT32 ret = 0;
	HUINT8 tmp = 0;
	HUINT16 retBytes = 0;
	HUINT32 detected = CAM_NO_DETECT;

	if ((slotId >= CIMAX_CAM_MAX_SLOT_NUM)||(result == NULL))
	{
		return CIMAX_INVALID_SLOT_ID_ERROR;
	}

	DRV_CIMAX_CAMDetected(slotId, &detected);
	if(detected != CAM_DETECT)
	{
		return CIMAX_ERROR;
	}

	ret = CIMAX_CheckCommandStatus();
	if(ret != CIMAX_NO_ERROR)
	{
		return CIMAX_ERROR;
	}

	*result  = 0;
	ret  = CIMAX_NormalCommand(slotId,CIMAX_CAM_REG_STATUS,&tmp,0,&retBytes);
	if(ret == 0)
	{
		*result = tmp;
	}

	return ret;
}

HINT32 DRV_CIMAX_Negotiate(HUINT8 slotId, HUINT16 bufferSize,HUINT16 *negociatedbufferSize)
{
	HINT32 ret = 0;
	HUINT8 tmp[2] = {0,};
	HUINT16 retBytes = 0;

	if (slotId >= CIMAX_CAM_MAX_SLOT_NUM)
	{
		return CIMAX_INVALID_SLOT_ID_ERROR;
	}

	if(negociatedbufferSize == NULL)
	{
		return CIMAX_ERROR;
	}

	ret = CIMAX_CheckCommandStatus();
	if(ret != CIMAX_NO_ERROR)
	{
		return CIMAX_ERROR;
	}

	tmp[0] = bufferSize >> 8;
	tmp[1] = bufferSize & 0x00FF;

	ret  = CIMAX_NormalCommand(slotId,CIMAX_CAM_NEGOCIATE_CMD,tmp,CIMAX_CAM_NEGOTIATE_PAYLOAD_SIZE,&retBytes);
	if((ret == 0)&&(retBytes > 0))
	{
		*negociatedbufferSize = tmp[0];
		*negociatedbufferSize <<= 8;
		*negociatedbufferSize |= tmp[1];
	}
	else
	{
		*negociatedbufferSize = 0;
		ret = -1;
	}


	return ret;
}

HINT32 DRV_CIMAX_WriteLPDU(HUINT8 slotId, HUINT8 *buffer, HUINT16 numOfBytes)
{
	HINT32 ret = 0;
	HUINT16 retBytes = 0;

	if (slotId >= CIMAX_CAM_MAX_SLOT_NUM)
	{
		return CIMAX_INVALID_SLOT_ID_ERROR;
	}

	if(buffer == NULL)
	{
		return CIMAX_ERROR;
	}

	ret = CIMAX_CheckCommandStatus();
	if(ret != CIMAX_NO_ERROR)
	{
		return CIMAX_ERROR;
	}

	ret  = CIMAX_NormalCommand(slotId,CIMAX_CAM_WRITE_LPDU_CMD,buffer,numOfBytes,&retBytes);

	return ret;
}

HINT32 DRV_CIMAX_ReadLPDU(HUINT8 slotId, HUINT8 *buffer, HUINT16 *numOfBytes)
{
	HINT32 ret = 0;
	HUINT16 retBytes = 0;

	if (slotId >= CIMAX_CAM_MAX_SLOT_NUM)
	{
		return CIMAX_INVALID_SLOT_ID_ERROR;
	}

	if(buffer == NULL)
	{
		return CIMAX_ERROR;
	}

	ret = CIMAX_CheckCommandStatus();
	if(ret != CIMAX_NO_ERROR)
	{
		return CIMAX_ERROR;
	}

	ret  = CIMAX_NormalCommand(slotId,CIMAX_CAM_READ_LPDU_CMD,buffer,0,&retBytes);
	if(ret == 0)
	{
		*numOfBytes = retBytes;
	}

	return ret;
}

HINT32 DRV_CIMAX_CAMDetected(HUINT16 slotId, HUINT32  *result)
{

	if (slotId >= CIMAX_CAM_MAX_SLOT_NUM)
	{
		return CIMAX_INVALID_SLOT_ID_ERROR;
	}

	VK_SEM_Get(s_ulCiConnSemId);
	if((caminfo.ucDetect[slotId])&&(caminfo.usStatus[slotId] == DI_CI_ENABLED))
	{

		*result = caminfo.ucDetect[slotId];
	}
	else
	{
		*result = 0;
	}
	VK_SEM_Release(s_ulCiConnSemId);

	return CIMAX_NO_ERROR;
}

/**
 * * @ingroup  CIMAX+
 * * @brief    This function represents the procedure for calculating signature.
 * *
 * */
HUINT32 CIMAX_ComputeBistRom(HUINT8 *tab, HINT32 size, HUINT16 FWSign)
{
	HINT32 k = 0, i = 0;
	HUINT16 mySign = 0;

	for ( k=0; k<size; k++ )
	{
		mySign = tab[k]&0x01;

		for (i=0; i<16; i++)
			if ( 0x88B7&(1<<i) )
				mySign ^= (FWSign>>i)&0x01;

		mySign |= ((FWSign<<1)^(tab[ k ]))&0x00FE;
		mySign |= (FWSign<<1) & 0x00FF00;

		FWSign = mySign;
	}

	return FWSign;
}

HINT32 CIMAX_InterruptInit(void)
{
	DRV_Error eDrvError = DRV_OK;

	HUINT8	bufferOut[10]={0,};
#if defined(CONFIG_EXT_CI_CTRL_INT_MODE)
	HINT32 retVal = 0;
#else
	HUINT8	bufferIn[10]={0,};
	HULONG	ulEvent = 0;
#endif
	HUINT8	transmode = 0;

	HUINT32 totalLength = 0;
	HINT32 vkErr = 0;


	vkErr = VK_SEM_Get(s_ulCiSemId);

	VK_MEM_Memset(bufferOut,0,sizeof(bufferOut));
#if defined(CONFIG_EXT_CI_CTRL_INT_MODE)
#else
	VK_MEM_Memset(bufferIn,0,sizeof(bufferIn));
#endif

	bufferOut[CIMAX_REGISTER_CMD_POS]			= CIMAX_REGISTER_WRITE_REQ;
	bufferOut[CIMAX_REGISTER_REG_ADDR_MSB_POS]	= (IT_HOST_PIN_CFG>>8) & 0xFF;
	bufferOut[CIMAX_REGISTER_REG_ADDR_LSB_POS]	= IT_HOST_PIN_CFG & 0xFF;
	bufferOut[CIMAX_REGISTER_NUM_OF_REGS_POS]	= 1;
	bufferOut[CIMAX_REGISTER_NUM_OF_REGS_POS+1]	= 0x02;

	totalLength = CIMAX_REGISTER_HEADER_SIZE + 1;


	eDrvError = DRV_SPI_TransmitPacket(0,transmode, bufferOut, totalLength);
	if(eDrvError != 0)
	{
		PrintError("== CIMAX_InterruptInit R1 (%x)####==\r\n",eDrvError);
		VK_SEM_Release(s_ulCiSemId);
		return -1;
	}
#if defined(CONFIG_EXT_CI_CTRL_INT_MODE)
	retVal = CIMAX_GetResponse(GET_CI_EXT_CTRL_RESP);
	if (retVal < 0)
	{
		VK_SEM_Release(s_ulCiSemId);
		PrintError("[%s] CIMAX_GetResponse Error! \n", __func__);
		return retVal;
	}

	if ((s_cimaxBufferIn[CIMAX_REGISTER_REG_ADDR_MSB_POS]	!= bufferOut[CIMAX_REGISTER_REG_ADDR_MSB_POS]) ||
	(s_cimaxBufferIn[CIMAX_REGISTER_REG_ADDR_LSB_POS]	!= bufferOut[CIMAX_REGISTER_REG_ADDR_LSB_POS]))
	{
		if((s_cimaxBufferIn[CIMAX_REGISTER_CMD_POS] != CIMAX_REGISTER_WRITE_RESP_OK )||(s_cimaxBufferIn[CIMAX_REGISTER_NUM_OF_REGS_POS] != 0x00))
		{
			PrintError("== CIMAX_InterruptInit R4 (%x:%x) ####==\r\n",s_cimaxBufferIn[CIMAX_REGISTER_CMD_POS],s_cimaxBufferIn[CIMAX_REGISTER_NUM_OF_REGS_POS]);
			VK_SEM_Release(s_ulCiSemId);
			return -1;
		}
	}

#else
	vkErr = VK_EVENT_ReceiveTimeout(s_ulCiMaxEventId, &ulEvent,SPI_WAIT_TIMEOUT);
	if(vkErr != 0)
	{
		PrintError("== CIMAX_InterruptInit R2 (%x)####==\r\n",vkErr);
		VK_SEM_Release(s_ulCiSemId);
		return -1;
	}

	totalLength = CIMAX_REGISTER_HEADER_SIZE;

	eDrvError	= DRV_SPI_ReceivePacket(0,transmode, bufferIn,totalLength);
	if(eDrvError != 0)
	{
		PrintError("== CIMAX_InterruptInit R3 (%x)####==\r\n",eDrvError);
		VK_SEM_Release(s_ulCiSemId);
		return -1;
	}


	if ((bufferIn[CIMAX_REGISTER_REG_ADDR_MSB_POS]	!= bufferOut[CIMAX_REGISTER_REG_ADDR_MSB_POS]) ||
	(bufferIn[CIMAX_REGISTER_REG_ADDR_LSB_POS]	!= bufferOut[CIMAX_REGISTER_REG_ADDR_LSB_POS]))
	{
		if((bufferIn[CIMAX_REGISTER_CMD_POS] != CIMAX_REGISTER_WRITE_RESP_OK )||(bufferIn[CIMAX_REGISTER_NUM_OF_REGS_POS] != 0x00))
		{
			PrintError("== CIMAX_InterruptInit R4 (%x:%x) ####==\r\n",bufferIn[CIMAX_REGISTER_CMD_POS],bufferIn[CIMAX_REGISTER_NUM_OF_REGS_POS]);
			VK_SEM_Release(s_ulCiSemId);
			return -1;
		}
	}
#endif
	VK_SEM_Release(s_ulCiSemId);
	return 0;
}


/**
 * * @ingroup  CIMAX+
 * * @brief    This function represents the procedure for sending INIT command used to indicate the end of th boot sequence.
 * *
 * *
 * * @param    none.
 * * @return   int             Returns 4 on success or -1 on failure.
 * *
 * */

HINT32 CIMAX_DownloadFW(void)
{
	DRV_Error eDrvError = DRV_OK;

	HUINT32		currAddr = FW_START_ADDRESS;
	HUINT8		bufOut[FW_PACKET_SIZE+CIMAX_REGISTER_HEADER_SIZE]={0,};
	HUINT8		rdata[FW_PACKET_SIZE+CIMAX_REGISTER_HEADER_SIZE]={0,};
	HUINT8		bufferOut[FW_PACKET_SIZE]={0,};
	HUINT32		packetSize = 0;
	HUINT32		FWSign = 0;
	HUINT8		transmode = 8;

	HUINT32		totalLength = 0;
	HUINT8		Val[2] = {0};
	HINT32		startAddr = 0,endAddr = 0,signature = 0;
	HINT32		vkErr = 0;
#if defined(CONFIG_EXT_CI_CTRL_INT_MODE)
	HINT32		retVal = 0;
#else
	HULONG		ulEvent = 0;
#endif

	vkErr = VK_SEM_Get(s_ulCiSemId);
	do
	{

	/* Calculate packet size for sending to CIMaX+. */
		packetSize = (unsigned char) (( currAddr <= (FW_END_ADDRESS+1-FW_PACKET_SIZE) ) ? (FW_PACKET_SIZE) : (FW_END_ADDRESS+1-currAddr));

		VK_MEM_Memset(bufferOut,0,FW_PACKET_SIZE);
		VK_MEM_Memset(bufOut,0,FW_PACKET_SIZE+CIMAX_REGISTER_HEADER_SIZE);

		/* Fill output buffer with data from CIMaX+ FW file. */
		VK_MEM_Memcpy((void *)bufferOut, (void *)&cimax_firmware[currAddr], packetSize);

		/* Compute BistRom. */
		FWSign = CIMAX_ComputeBistRom( bufferOut, packetSize, FWSign );

		bufOut[CIMAX_REGISTER_CMD_POS]			= CIMAX_REGISTER_WRITE_REQ;
		bufOut[CIMAX_REGISTER_REG_ADDR_MSB_POS] = (currAddr>>8) & 0xFF;
		bufOut[CIMAX_REGISTER_REG_ADDR_LSB_POS] = currAddr & 0xFF;
		bufOut[CIMAX_REGISTER_NUM_OF_REGS_POS]	= packetSize;

		VK_MEM_Memcpy(&bufOut[CIMAX_REGISTER_HEADER_SIZE], bufferOut, packetSize);

		totalLength = CIMAX_REGISTER_HEADER_SIZE + packetSize;

		if(!(totalLength%2))
			transmode = 16;
		else
			transmode = 8;

		eDrvError = DRV_SPI_TransmitPacket(0,transmode,bufOut,totalLength);
		if(eDrvError != DRV_OK)
		{
			VK_SEM_Release(s_ulCiSemId);
			PrintError("=== DRV_SPI_TransmitPacket Error(%d) === \r\n",eDrvError);
			return -1;
		}
#if defined(CONFIG_EXT_CI_CTRL_INT_MODE)
		retVal = CIMAX_GetResponse(GET_INT_PIN);
		if (retVal != INT_SIG)
		{
			VK_SEM_Release(s_ulCiSemId);
			PrintError("[%s] CIMAX_GetResponse Error! \n", __func__);
			return -1;
		}
#else
		vkErr = VK_EVENT_ReceiveTimeout(s_ulCiMaxEventId, &ulEvent,SPI_WAIT_TIMEOUT);
		if(vkErr != 0)
		{
			VK_SEM_Release(s_ulCiSemId);
			PrintError("== CIMAX_DownloadFW R1 (%x)####==\r\n",vkErr);
			return -1;
		}
#endif
		totalLength = CIMAX_REGISTER_HEADER_SIZE;
		VK_MEM_Memset(rdata, 0,FW_PACKET_SIZE+CIMAX_REGISTER_HEADER_SIZE);

		if(!(totalLength%2))
			transmode = 16;
		else
			transmode = 8;

		VK_TASK_Sleep(CIMAX_SLEEP_SMALL_TIME);

		eDrvError = DRV_SPI_ReceivePacket(0,transmode,rdata,totalLength);
		/* Write data to CIMaX+. */
		if (eDrvError != 0)
		{
			VK_SEM_Release(s_ulCiSemId);
			/* CIMaX+ write error. */
			PrintError("=== DWF R1 ===\r\n");
			return -1;
		}
		else
		{
			if ((rdata[CIMAX_REGISTER_REG_ADDR_MSB_POS] != bufOut[CIMAX_REGISTER_REG_ADDR_MSB_POS]) ||
			(rdata[CIMAX_REGISTER_REG_ADDR_LSB_POS] != bufOut[CIMAX_REGISTER_REG_ADDR_LSB_POS]))
			{
				if((rdata[CIMAX_REGISTER_CMD_POS] != CIMAX_REGISTER_WRITE_RESP_OK )||(rdata[CIMAX_REGISTER_NUM_OF_REGS_POS] != 0x00))
				{
					PrintError("== CIMAX_DownloadFW R4 (%x:%x:%x:%x) ####==\r\n",rdata[0],rdata[1],rdata[2],rdata[3]);
					VK_SEM_Release(s_ulCiSemId);
					return -1;
				}
			}

		}

		currAddr += packetSize;

	} while ( currAddr < (FW_END_ADDRESS+1) );


	VK_MEM_Memset(bufOut,0,FW_PACKET_SIZE+CIMAX_REGISTER_HEADER_SIZE);
	VK_MEM_Memset(bufferOut,0,FW_PACKET_SIZE);
	/* Set current address. */
	currAddr = FW_VECTOR_TABLE_START_ADDRESS;

	/* Set packet size for vector table. */
	packetSize = FW_VECTOR_TABLE_SIZE;

	/* Fill output buffer with data from CIMaX+ FW file */
	VK_MEM_Memcpy((void *)bufferOut, (void *)&cimax_firmware[currAddr], packetSize);

	/* Compute BistRom. */
	FWSign = CIMAX_ComputeBistRom(bufferOut, packetSize, FWSign);
	bufOut[CIMAX_REGISTER_CMD_POS]			= CIMAX_REGISTER_WRITE_REQ;
	bufOut[CIMAX_REGISTER_REG_ADDR_MSB_POS] = (currAddr>>8) & 0xFF;
	bufOut[CIMAX_REGISTER_REG_ADDR_LSB_POS] = currAddr & 0xFF;
	bufOut[CIMAX_REGISTER_NUM_OF_REGS_POS]	= packetSize;

	VK_MEM_Memcpy(&bufOut[CIMAX_REGISTER_HEADER_SIZE], bufferOut, packetSize);

	totalLength = CIMAX_REGISTER_HEADER_SIZE + packetSize;

	if(!(totalLength%2))
		transmode = 16;
	else
		transmode = 8;

	eDrvError = DRV_SPI_TransmitPacket(0,transmode,bufOut,totalLength);

	if (eDrvError != 0)
	{
		VK_SEM_Release(s_ulCiSemId);
		PrintError("=== DWF R3 ===\r\n");
		return -1;
	}
#if defined(CONFIG_EXT_CI_CTRL_INT_MODE)
	retVal = CIMAX_GetResponse(GET_INT_PIN);
	if (retVal != INT_SIG)
	{
		VK_SEM_Release(s_ulCiSemId);
		PrintError("[%s] CIMAX_GetResponse Error! \n", __func__);
		return -1;
	}
#else
	vkErr = VK_EVENT_ReceiveTimeout(s_ulCiMaxEventId, &ulEvent,SPI_WAIT_TIMEOUT);
	if(vkErr != 0)
	{
		VK_SEM_Release(s_ulCiSemId);
		PrintError("== CIMAX_DownloadFW R2 (%x)####==\r\n",vkErr);
		return -1;
	}
#endif
	totalLength = CIMAX_REGISTER_HEADER_SIZE;
	VK_MEM_Memset(rdata, 0,CIMAX_REGISTER_HEADER_SIZE);

	if(!(totalLength%2))
		transmode = 16;
	else
		transmode = 8;

	eDrvError = DRV_SPI_ReceivePacket(0,transmode,rdata,totalLength);
	if (eDrvError != 0)
	{
		PrintError("=== DWF R4 ===\r\n");
		VK_SEM_Release(s_ulCiSemId);
		return -1;
	}

	startAddr = FW_START_ADDRESS;
	endAddr = FW_END_ADDRESS;
	currAddr = 0x008D;

	/* Write "Flash" Size */
	Val[0] = (0xD000-startAddr)&0x00ff;
	Val[1] = (0xD000-startAddr)>>8;
	packetSize = 2;
	totalLength = CIMAX_REGISTER_HEADER_SIZE + packetSize;

	bufOut[CIMAX_REGISTER_CMD_POS]			= CIMAX_REGISTER_WRITE_REQ;
	bufOut[CIMAX_REGISTER_REG_ADDR_MSB_POS] = (currAddr>>8) & 0xFF;
	bufOut[CIMAX_REGISTER_REG_ADDR_LSB_POS] = currAddr & 0xFF;
	bufOut[CIMAX_REGISTER_NUM_OF_REGS_POS]	= packetSize;
	VK_MEM_Memcpy(&bufOut[CIMAX_REGISTER_HEADER_SIZE], Val, packetSize);

	if(!(totalLength%2))
		transmode = 16;
	else
		transmode = 8;

	eDrvError = DRV_SPI_TransmitPacket(0,transmode,bufOut,totalLength);
	if (eDrvError != 0)
	{
		VK_SEM_Release(s_ulCiSemId);
		PrintError("=== DWF R5 ===\r\n");
		return -1;
	}
#if defined(CONFIG_EXT_CI_CTRL_INT_MODE)
	retVal = CIMAX_GetResponse(GET_INT_PIN);
	if (retVal != INT_SIG)
	{
		VK_SEM_Release(s_ulCiSemId);
		PrintError("[%s] CIMAX_GetResponse Error! \n", __func__);
		return -1;
	}
#else
	vkErr = VK_EVENT_ReceiveTimeout(s_ulCiMaxEventId, &ulEvent,SPI_WAIT_TIMEOUT);
	if(vkErr != 0)
	{
		VK_SEM_Release(s_ulCiSemId);
		PrintError("== CIMAX_DownloadFW R3 (%x)####==\r\n",vkErr);
		return -1;
	}
#endif
	totalLength = CIMAX_REGISTER_HEADER_SIZE;
	VK_MEM_Memset(rdata, 0,CIMAX_REGISTER_HEADER_SIZE);
	if(!(totalLength%2))
		transmode = 16;
	else
		transmode = 8;

	eDrvError = DRV_SPI_ReceivePacket(0,transmode,rdata,totalLength);
	if (eDrvError != 0)
	{
		VK_SEM_Release(s_ulCiSemId);
		PrintError("=== DWF R6 ===\r\n");
		return -1;
	}

	/* Write Signature */
	signature = FWSign;
	Val[0] = signature&0x00ff;
	Val[1] = signature>>8;
	packetSize = 2;
	currAddr = 0x0080;
	totalLength = CIMAX_REGISTER_HEADER_SIZE + packetSize;

	bufOut[CIMAX_REGISTER_CMD_POS]			= CIMAX_REGISTER_WRITE_REQ;
	bufOut[CIMAX_REGISTER_REG_ADDR_MSB_POS] = (currAddr>>8) & 0xFF;
	bufOut[CIMAX_REGISTER_REG_ADDR_LSB_POS] = currAddr & 0xFF;
	bufOut[CIMAX_REGISTER_NUM_OF_REGS_POS]	= packetSize;
	VK_MEM_Memcpy(&bufOut[CIMAX_REGISTER_HEADER_SIZE], Val, packetSize);

	if(!(totalLength%2))
		transmode = 16;
	else
		transmode = 8;

	eDrvError = DRV_SPI_TransmitPacket(0,transmode,bufOut,totalLength);
	if (eDrvError != 0)
	{
		VK_SEM_Release(s_ulCiSemId);
		PrintError("=== DWF R7 ===\r\n");
		return -1;
	}
#if defined(CONFIG_EXT_CI_CTRL_INT_MODE)
	retVal = CIMAX_GetResponse(GET_INT_PIN);
	if (retVal != INT_SIG)
	{
		VK_SEM_Release(s_ulCiSemId);
		PrintError("[%s] CIMAX_GetResponse Error! \n", __func__);
		return -1;
	}
#else
	vkErr = VK_EVENT_ReceiveTimeout(s_ulCiMaxEventId, &ulEvent,SPI_WAIT_TIMEOUT);
	if(vkErr != 0)
	{
		VK_SEM_Release(s_ulCiSemId);
		PrintError("== CIMAX_DownloadFW R4 (%x)####==\r\n",vkErr);
		return -1;
	}
#endif
	totalLength = CIMAX_REGISTER_HEADER_SIZE;
	VK_MEM_Memset(rdata, 0,CIMAX_REGISTER_HEADER_SIZE);
	if(!(totalLength%2))
		transmode = 16;
	else
		transmode = 8;

	eDrvError = DRV_SPI_ReceivePacket(0,transmode,rdata,totalLength);

	if (eDrvError != 0)
	{
		VK_SEM_Release(s_ulCiSemId);
		PrintError("=== DWF R8 ===\r\n");
		return -1;
	}

	/* Launch BistRom [(D000-flashSize)..CFF9]+[FFFA..FFFF] computation */
	Val[0] = 0x0F;
	packetSize = 1;
	currAddr = 0x0082;
	totalLength = CIMAX_REGISTER_HEADER_SIZE + packetSize;
	bufOut[CIMAX_REGISTER_CMD_POS]			= CIMAX_REGISTER_WRITE_REQ;
	bufOut[CIMAX_REGISTER_REG_ADDR_MSB_POS] = (currAddr>>8) & 0xFF;
	bufOut[CIMAX_REGISTER_REG_ADDR_LSB_POS] = currAddr & 0xFF;
	bufOut[CIMAX_REGISTER_NUM_OF_REGS_POS]	= packetSize;
	VK_MEM_Memcpy(&bufOut[CIMAX_REGISTER_HEADER_SIZE], Val, packetSize);

	if(!(totalLength%2))
		transmode = 16;
	else
		transmode = 8;

	eDrvError = DRV_SPI_TransmitPacket(0,transmode,bufOut,totalLength);
	if (eDrvError != 0)
	{
		VK_SEM_Release(s_ulCiSemId);
		PrintError("=== DWF R10 ===\r\n");
		return -1;
	}
#if defined(CONFIG_EXT_CI_CTRL_INT_MODE)
	retVal = CIMAX_GetResponse(GET_INT_PIN);
	if (retVal != INT_SIG)
	{
		VK_SEM_Release(s_ulCiSemId);
		PrintError("[%s] CIMAX_GetResponse Error! \n", __func__);
		return -1;
	}
#else
	vkErr = VK_EVENT_ReceiveTimeout(s_ulCiMaxEventId, &ulEvent,SPI_WAIT_TIMEOUT);
	if(vkErr != 0)
	{
		VK_SEM_Release(s_ulCiSemId);
		PrintError("== CIMAX_DownloadFW R5 (%x)####==\r\n",vkErr);
		return -1;
	}
#endif
	totalLength = CIMAX_REGISTER_HEADER_SIZE;
	VK_MEM_Memset(rdata, 0,CIMAX_REGISTER_HEADER_SIZE);
	if(!(totalLength%2))
		transmode = 16;
	else
		transmode = 8;
	eDrvError = DRV_SPI_ReceivePacket(0,transmode,rdata,totalLength);

	if (eDrvError != 0)
	{
		VK_SEM_Release(s_ulCiSemId);
		PrintError("=== DWF R11 ===\r\n");
		return -1;
	}

	VK_TASK_Sleep (CIMAX_SLEEP_SMALL_TIME);

	packetSize = 2;
	currAddr = 0x0041;

	bufOut[CIMAX_REGISTER_CMD_POS]			= CIMAX_REGISTER_READ_REQ;
	bufOut[CIMAX_REGISTER_REG_ADDR_MSB_POS] = (currAddr>>8) & 0xFF;
	bufOut[CIMAX_REGISTER_REG_ADDR_LSB_POS] = currAddr & 0xFF;
	bufOut[CIMAX_REGISTER_NUM_OF_REGS_POS]	= packetSize;

	totalLength = CIMAX_REGISTER_HEADER_SIZE;
	transmode = 16;
	eDrvError = DRV_SPI_TransmitPacket(0,transmode,bufOut,CIMAX_REGISTER_HEADER_SIZE);
	if (eDrvError != 0)
	{
		VK_SEM_Release(s_ulCiSemId);
		PrintError("=== DWF R12 ===\r\n");
		return -1;
	}
#if defined(CONFIG_EXT_CI_CTRL_INT_MODE)
	retVal = CIMAX_GetResponse(GET_INT_PIN);
	if (retVal != INT_SIG)
	{
		VK_SEM_Release(s_ulCiSemId);
		PrintError("[%s] CIMAX_GetResponse Error! \n", __func__);
		return -1;
	}
#else
	vkErr = VK_EVENT_ReceiveTimeout(s_ulCiMaxEventId, &ulEvent,SPI_WAIT_TIMEOUT);
	if(vkErr != 0)
	{
		VK_SEM_Release(s_ulCiSemId);
		PrintError("== CIMAX_DownloadFW R6 (%x)####==\r\n",vkErr);
		return -1;
	}
#endif
	totalLength = CIMAX_REGISTER_HEADER_SIZE + packetSize;
	VK_MEM_Memset(rdata, 0, totalLength);
	if(!(totalLength%2))
		transmode = 16;
	else
		transmode = 8;

	eDrvError = DRV_SPI_ReceivePacket(0,transmode,rdata,totalLength);
	if (eDrvError != 0)
	{
		VK_SEM_Release(s_ulCiSemId);
		PrintError("=== DWF R13 ===\r\n");
		return -1;
	}

	packetSize = 1;
	currAddr = BOOT_Status;
	totalLength = CIMAX_REGISTER_HEADER_SIZE;

	bufOut[CIMAX_REGISTER_CMD_POS]			= CIMAX_REGISTER_READ_REQ;
	bufOut[CIMAX_REGISTER_REG_ADDR_MSB_POS] = (currAddr>>8) & 0xFF;
	bufOut[CIMAX_REGISTER_REG_ADDR_LSB_POS] = currAddr & 0xFF;
	bufOut[CIMAX_REGISTER_NUM_OF_REGS_POS]	= packetSize;


	if(!(totalLength%2))
		transmode = 16;
	else
		transmode = 8;
	eDrvError = DRV_SPI_TransmitPacket(0,transmode,bufOut,CIMAX_REGISTER_HEADER_SIZE);
	if (eDrvError != 0)
	{
		VK_SEM_Release(s_ulCiSemId);
		PrintError("=== DWF R14 ===\r\n");
		return -1;
	}
#if defined(CONFIG_EXT_CI_CTRL_INT_MODE)
	retVal = CIMAX_GetResponse(GET_INT_PIN);
	if (retVal != INT_SIG)
	{
		VK_SEM_Release(s_ulCiSemId);
		PrintError("[%s] CIMAX_GetResponse Error! \n", __func__);
		return -1;
	}
#else
	vkErr = VK_EVENT_ReceiveTimeout(s_ulCiMaxEventId, &ulEvent,SPI_WAIT_TIMEOUT);
	if(vkErr != 0)
	{
		VK_SEM_Release(s_ulCiSemId);
		PrintError("== CIMAX_DownloadFW R7 (%x)####==\r\n",vkErr);
		return -1;
	}
#endif
	totalLength = CIMAX_REGISTER_HEADER_SIZE + packetSize;
	VK_MEM_Memset(rdata, 0, totalLength);
	if(!(totalLength%2))
		transmode = 16;
	else
		transmode = 8;

	eDrvError = DRV_SPI_ReceivePacket(0,transmode,rdata,totalLength);
	if (eDrvError != 0)
	{
		VK_SEM_Release(s_ulCiSemId);
		PrintError("=== DWF R15 ===\r\n");
		return -1;
	}

	if(rdata[4] != 0x02)
	{
		VK_SEM_Release(s_ulCiSemId);
		PrintError("=== DWF R16 ===\r\n");
		return -1;
	}

	totalLength = CIMAX_REGISTER_HEADER_SIZE;
	VK_MEM_Memset(bufOut, 0, totalLength);
	if(!(totalLength%2))
		transmode = 16;
	else
		transmode = 8;

	eDrvError = DRV_SPI_TransmitPacket(0,transmode,bufOut,totalLength);
	if (eDrvError != 0)
	{
		VK_SEM_Release(s_ulCiSemId);
		PrintError("=== DWF R17 ===\r\n");
		return -1;
	}
#if defined(CONFIG_EXT_CI_CTRL_INT_MODE)
	retVal = CIMAX_GetResponse(GET_INT_PIN);
	if (retVal != INT_SIG)
	{
		VK_SEM_Release(s_ulCiSemId);
		PrintError("[%s] CIMAX_GetResponse Error! \n", __func__);
		return -1;
	}
#else
	vkErr = VK_EVENT_ReceiveTimeout(s_ulCiMaxEventId, &ulEvent,SPI_WAIT_TIMEOUT);
	if(vkErr != 0)
	{
		VK_SEM_Release(s_ulCiSemId);
		PrintError("== CIMAX_DownloadFW R8 (%x)####==\r\n",vkErr);
		return -1;
	}
#endif
	totalLength = CIMAX_REGISTER_HEADER_SIZE;
	VK_MEM_Memset(rdata, 0,CIMAX_REGISTER_HEADER_SIZE);
	if(!(totalLength%2))
		transmode = 16;
	else
		transmode = 8;

	eDrvError = DRV_SPI_ReceivePacket(0,eDrvError,rdata,totalLength);
	if (eDrvError != 0)
	{
		VK_SEM_Release(s_ulCiSemId);
		PrintError("=== DWF R18 ===\r\n");
		return -1;
	}

	VK_SEM_Release(s_ulCiSemId);

	return 0;
}

HINT32 CIMAX_Config(void)
{
	HINT32 result  = 0;
	HUINT8 wdata[2] = {0,};

	wdata[0] = DEFAULT_VALUE;
	/** Close TS input. */
	result  = CIMAX_WriteRegister(IN_SEL,wdata,1);
	if(result != 0 )
		return result;

	/** Close TS output. */
	result  = CIMAX_WriteRegister(OUT_SEL,wdata,1);
	if(result != 0 )
		return result;

	wdata[0] = DEFAULT_VALUE;
	wdata[0] = (RSTF_4|RSTF_3|RSTF_2|RSTF_1);
	/** Reset TS FIFO.  */
	result  = CIMAX_WriteRegister(FIFO_CTRL,wdata,1);
	if(result != 0 )
		return result;

	wdata[0] = DEFAULT_VALUE;
	wdata[0] = (EN_SYNC_1|EN_SYNC_0);
	result  = CIMAX_WriteRegister(SYNC_RTV_CTRL,wdata,1);
	if(result != 0 )
		return result;

	wdata[0] = DEFAULT_VALUE;
	wdata[0] = (DESYNC_COUNT << 4) | SYNC_COUNT;
	result  = CIMAX_WriteRegister(SYNC_RTV_CH0_SYNC_NB,wdata,1);
	if(result != 0 )
		return result;

	wdata[0] = DEFAULT_VALUE;
	wdata[0] = (DESYNC_COUNT << 4) | SYNC_COUNT;
	result  = CIMAX_WriteRegister(SYNC_RTV_CH1_SYNC_NB,wdata,1);
	if(result != 0 )
		return result;

	wdata[0] = DEFAULT_VALUE;
#if defined(USE_GAP_REMOVER)
	wdata[0] = DEFAULT_VALUE;
//	wdata[0] = 0x27;
	wdata[0] = DIV_VALUE;
	wdata[0] <<= 2;
	wdata[0] |= (ENABLE|RESET);

	result  = CIMAX_WriteRegister(GAP_REMOVER_CH0_CTRL,wdata,1);
	if(result != 0 )
		return result;

	wdata[0] = DEFAULT_VALUE;
//	wdata[0] = 0x27;
	wdata[0] = DIV_VALUE;
	wdata[0] <<= 2;
	wdata[0] |= (ENABLE|RESET);
	result  = CIMAX_WriteRegister(GAP_REMOVER_CH1_CTRL,wdata,1);
	if(result != 0 )
		return result;
#endif
	wdata[0] = DEFAULT_VALUE;
#if defined(USE_GAP_REMOVER)
	wdata[0] = (EN_SER0_CLK|EN_SER1_CLK|EN_TS_CLK|EN_108_CLK|EN_72_CLK|EN_54_CLK|EN_27_CLK);
#else
	wdata[0] = (EN_SER0_CLK|EN_SER1_CLK|EN_108_CLK|EN_72_CLK|EN_54_CLK|EN_27_CLK);
#endif
	result  = CIMAX_WriteRegister(CkMan_Config,wdata,1);
	if(result != 0 )
		return result;

	wdata[0] = DEFAULT_VALUE;
	/* ser0_clk ==> 54 Mhz, ser1_clk ==> 54 Mhz ,TSCKSL ==>54MHz*/
	wdata[0] = TS_SER_CLK_72MHZ;
	wdata[0] <<= SER_CLK_SHIFT;
	wdata[0] |= TS_SER_CLK_72MHZ;
	wdata[0] <<= SER_CLK_SHIFT;
#if defined(USE_GAP_REMOVER)
	wdata[0] |= TS_SER_CLK_54MHZ;
#else
	wdata[0] |= TS_SER_CLK_72MHZ;
#endif
	result	= CIMAX_WriteRegister(CkMan_Select,wdata,1);
	if(result != 0 )
		return result;

	wdata[0] = DEFAULT_VALUE;
	wdata[0] = CH_IN_TSSERIAL2;		/* CH1 input을 serial input2 설정 */
	wdata[0] <<= CH_IN_SHIFT;
	wdata[0] |= CH_IN_TSSERIAL1;	/* CH0 input을 serial input1 설정 */
	result	= CIMAX_WriteRegister(IN_SEL,wdata,1);
	if(result != 0 )
		return result;

	wdata[0] = DEFAULT_VALUE;
	wdata[1] = DEFAULT_VALUE;
	wdata[0] = (SER_EN_CH|ICE|RST);
	wdata[1] = wdata[0];
	result  = CIMAX_WriteRegister(S2P_CH0_CTRL,wdata,2);
	if(result != 0 )
		return result;

	wdata[0] = DEFAULT_VALUE;
	wdata[1] = DEFAULT_VALUE;
	wdata[0] = (STRT|OSCS|EN_P2S|OCE|RST);
	wdata[1] = wdata[0];
	result  = CIMAX_WriteRegister(P2S_CH0_CTRL,wdata,2);
	if(result != 0 )
		return result;

	wdata[0] = DEFAULT_VALUE;
	wdata[0] = (CH_OUT_TSSERIAL2|CH_OUT_TSSERIAL1);
	result  = CIMAX_WriteRegister(OUT_SEL,wdata,1);
	if(result != 0 )
		return result;

	/* cam config */
	wdata[0] = DEFAULT_VALUE;
	result	= CIMAX_WriteRegister(GPIO0_DATA_OUT,wdata,1);
	if(result != 0 )
		return result;

	wdata[0] = DEFAULT_VALUE;
	wdata[1] = DEFAULT_VALUE;

	wdata[1] = PCMCIA_LOCK;
	result  = CIMAX_WriteRegister(CFG_1,wdata,2);
	if(result != 0 )
		return result;

	wdata[0] = DEFAULT_VALUE;
	result  = CIMAX_WriteRegister(GPIO0_DFT,wdata,1);
	if(result != 0 )
		return result;

	wdata[0] = DEFAULT_VALUE;
	wdata[0] = (EN_SLOT1|EN_SLOT0);
	result  = CIMAX_WriteRegister(GPIO0_MASK_DATA,wdata,1);
	if(result != 0 )
		return result;

	wdata[0] = DEFAULT_VALUE;
	wdata[0] = (EN_SLOT1|EN_SLOT0);
	result  = CIMAX_WriteRegister(GPIO0_DIR,wdata,1);
	if(result != 0 )
		return result;
#if defined(CAM_SLOT_POWER_ALWAYS_ON)
	result  = CIMAX_PowerOnCam();
	if(result != 0 )
	{
		return result;
	}
#endif
	/* CAM0,1의 attribute/com memory cycle time 설정 */
	wdata[0] = DEFAULT_VALUE;
	wdata[1] = DEFAULT_VALUE;
	wdata[0] = MEM_CYCLLE_250NS;
	wdata[0] <<= SHIFT_MEM_CYCLLE_TIME;
	wdata[0] |= MEM_CYCLLE_250NS;
	wdata[1] = wdata[0];
	result  = CIMAX_WriteRegister(MEM_ACC_TIME_A,wdata,2);
	if(result != 0 )
		return result;

	wdata[0] = DEFAULT_VALUE;
	wdata[0] = (PDC5|PDC4|PDC3|PDC2);
	result  = CIMAX_WriteRegister(PAD_PUPD_CTRL,wdata,1);
	if(result != 0 )
		return result;

	return result;
}

void DRV_CIMAX_IntrCallback(void *context, HINT32 param)
{

	BSTD_UNUSED(param);

	VK_EVENT_Send(*(unsigned long *)context, 0);
}


/*****************************************************************
 * Function	: DRV_CI_InitPathStatus
 * Description
 * Inputs
 * Outputs
 * Returns
 *****************************************************************/
void DRV_CI_InitPathStatus(void)
{
	HUINT8 index = 0;

	for(index = 0; index < CAM_MAXSLOTID; index++)
	{
		VK_MEM_Memset(&(sCIPathStatus[index]), 0xff, sizeof(DRV_CI_PATH_STATUS_t));
		sCIPathStatus[index].used = 0;
	}

	return;
}

/*****************************************************************
 * Function	: DRV_CI_SetPathStatus
 * Description
 * Inputs
 * Outputs
 * Returns
 *****************************************************************/
DRV_Error DRV_CI_SetPathStatus( HUINT32 slotId , DRV_CI_PATH_STATUS_t *pCiPathStatus )
{
	if((pCiPathStatus == NULL)||(slotId >= CIMAX_CAM_MAX_SLOT_NUM))
		return DRV_ERR;

	VK_memcpy(&(sCIPathStatus[slotId]),pCiPathStatus,sizeof(DRV_CI_PATH_STATUS_t));

	return DRV_OK;

}

/*****************************************************************
 * Function	: DRV_CI_SetPathStatus
 * Description
 * Inputs
 * Outputs
 * Returns
 *****************************************************************/
DRV_Error DRV_CI_SavePathWay( HUINT32 slotId , HUINT32 savepathWay )
{

	if(slotId >= CIMAX_CAM_MAX_SLOT_NUM)
		return DRV_ERR;

	if((sCIPathStatus[slotId].used == 1)&&(sCIPathStatus[slotId].connectStatus == 1))
	{
		sCIPathStatus[slotId].connectWay = savepathWay;
	}
	else
	{
		return DRV_ERR;
	}

	return DRV_OK;

}

/*****************************************************************
 * Function	: DRV_CI_GetPathStatus
 * Description
 * Inputs
 * Outputs
 * Returns
 *****************************************************************/
DRV_Error DRV_CI_GetPathStatus( HUINT32 slotId , DRV_CI_PATH_STATUS_t *pCiPathStatus )
{
	if((pCiPathStatus == NULL)||(slotId >= CIMAX_CAM_MAX_SLOT_NUM))
		return DRV_ERR;

	VK_memcpy(pCiPathStatus,&(sCIPathStatus[slotId]),sizeof(DRV_CI_PATH_STATUS_t));

	return DRV_OK;

}

DRV_Error DRV_CIMAX_HWReset(void)
{
	DRV_Error eDrvError = DRV_OK;

	PrintEnter();
	/* CAM ReSeT Signal ACTIVE */
	eDrvError = DRV_GPIO_Write(GPIO_ID_CIMAX_RESET, CAM_RESET_ON);
	if(eDrvError != DRV_OK)
	{
		PrintError("DRV_CIMAX_HWReset() Active - GPIO_ID_CIMAX_RESET Error (%d)\n",eDrvError);
		eDrvError = DRV_ERR;
		return eDrvError;
	}

	VK_TASK_Sleep(CAM_RESET_TIME);

	/* CAM ReSeT Signal INACTIVE */
	eDrvError = DRV_GPIO_Write(GPIO_ID_CIMAX_RESET, CAM_RESET_OFF);
	if(eDrvError != DRV_OK)
	{
		PrintError("DRV_CIMAX_HWReset() Inactive - GPIO_ID_CIMAX_RESET Error(%d)\n",eDrvError);
		eDrvError = DRV_ERR;
		return eDrvError;
	}

	PrintExit();

	return eDrvError;
}


HINT32	P_CIMAX_ReInitialize(void)
{
	DRV_Error drvErr = 0;
	HINT32	err = 0;

	PrintEnter();

	drvErr = DRV_CIMAX_HWReset();
	if(drvErr != DRV_OK)
	{
		PrintError("=== P_CIMAX_ReInitialize 1(%d) ===\r\n",err);
		PrintExit();
		return -1;
	}

	VK_TASK_Sleep (CIMAX_SLEEP_MAX_TIME);

	err = CIMAX_InterruptInit();
	if(err != CIMAX_NO_ERROR)
	{
		PrintError("=== P_CIMAX_ReInitialize 2(%d) ===\r\n",err);
		PrintExit();
		return -2;
	}

	VK_TASK_Sleep (CIMAX_SLEEP_SMALL_TIME);

	/* Firmware Download */
	err = CIMAX_DownloadFW();
	if(err != CIMAX_NO_ERROR)
	{
		PrintError("=== P_CIMAX_ReInitialize 3(%d) ===\r\n",err);
		PrintExit();
		return -3;
	}

	VK_TASK_Sleep (CIMAX_SLEEP_SMALL_TIME);

	err = CIMAX_Config();
	if(err != CIMAX_NO_ERROR)
	{
		PrintError("=== P_CIMAX_ReInitialize 4(%d) ===\r\n",err);
		PrintExit();
		return -4;
	}

	PrintExit();
	return 0;

}

static void P_CIMAX_Task (void *param)
{
	HINT32 result = 0,ret = 0,oresult  = 0, commandStatus = 0;
	static CI_TASK_STATE_t s_CiTaskState[CIMAX_CAM_MAX_SLOT_NUM] = {CI_TASK_IDLE,CI_TASK_IDLE};
	HUINT8 index = 0,retval[CIMAX_CAM_MAX_SLOT_NUM] = {0,0},checkval[CIMAX_CAM_MAX_SLOT_NUM] = {0,0};
	HUINT8 msgData[12] = {0,};
	HUINT8 rSlotId = 0,rCommand = 0xff;
	DRV_CI_PATH_STATUS_t	ciConnectStatus;

	HINT32	reinit = 0;

	UNREFENCED_PARAM(param);
	while (1)
	{
		if (_pfnCiCallbackFunc != NULL)
		{
			commandStatus = CIMAX_CheckCommandStatus();

			oresult  = VK_MSG_ReceiveTimeout(s_ulCiMaxMsgId, msgData, 4, 100);
			if(oresult == 0)
			{
				if(commandStatus == CIMAX_NO_ERROR)
				{
					rSlotId = msgData[0];
					rCommand = msgData[1];

					switch(rCommand)
					{
						case ECOOMMAND_EXTRACT:
#if defined(CAM_SLOT_POWER_ALWAYS_ON)
							/*Don't turn off slot power*/
#else
							VK_SEM_Get(s_ulCiConnSemId);
							ret = CIMAX_PowerOffCam (rSlotId);
							if(ret == 0 )
							{
#endif
							DRV_CI_GetPathStatus( rSlotId, &ciConnectStatus);
							if((ciConnectStatus.used == 1)&& (ciConnectStatus.connectStatus == CI_INSERTED))
							{
								ciConnectStatus.connectStatus = CI_EXTRACTED;
							}

							DRV_CI_SetPathStatus(rSlotId , &ciConnectStatus);

							caminfo.usStatus[rSlotId] = DI_CI_EXTRACTED;
							caminfo.ucDetect[rSlotId] = CAM_NO_DETECT;

							_pfnCiCallbackFunc (rSlotId, caminfo.usStatus[rSlotId]);

							if(_pfnCiCallbackFuncforDemux != NULL)
							{
								_pfnCiCallbackFuncforDemux(rSlotId, caminfo.usStatus[rSlotId]);
							}

							s_CiTaskState[rSlotId] = CI_TASK_IDLE;
#if defined(CAM_SLOT_POWER_ALWAYS_ON)
							/*Nothing to do*/
#else
							}
							else
							{
								s_CiTaskState[rSlotId] = CI_TASK_CAM_EXTRACTED;
							}
							VK_SEM_Release(s_ulCiConnSemId);
#endif
						break;
					}
				}

			}
			else
			{
				if(commandStatus != CIMAX_NO_ERROR)
				{
					reinit = P_CIMAX_ReInitialize();
					if(reinit == 0)
					{
						for( index = 0; index < CIMAX_CAM_MAX_SLOT_NUM; index++)
						{
							if(caminfo.usStatus[index] != DI_CI_EXTRACTED)
							{
								caminfo.usStatus[index] = DI_CI_EXTRACTED;
								caminfo.ucDetect[index] = CAM_NO_DETECT;
								_pfnCiCallbackFunc (index, caminfo.usStatus[index]);
							}

							s_CiTaskState[index] = CI_TASK_IDLE;
						}

						CIMAX_InitCommandStatus();
					}

				}
				else
				{
					for( index = 0; index < CIMAX_CAM_MAX_SLOT_NUM; index++)
					{
						if(s_CiTaskState[index] == CI_TASK_IDLE)
						{
							result = CIMAX_CheckSlotStatus (index,&retval[index]);
						}

						if(result == 0)
						{
							checkval[index] = retval[index];
							checkval[index] &= 0x01;

							if (checkval[index] != caminfo.ucDetect[index])
							{
								caminfo.ucDetect[index] = checkval[index];

								if (checkval[index] == CAM_DETECT)
									s_CiTaskState[index] = CI_TASK_CAM_INSERTED;
								else
									s_CiTaskState[index] = CI_TASK_CAM_EXTRACTED;
							}

							switch (s_CiTaskState[index])
							{
								case CI_TASK_IDLE:
									/* idle state : nothing to do */
									break;

								case CI_TASK_CAM_EXTRACTED:
#if defined(CAM_SLOT_POWER_ALWAYS_ON)
									DI_UART_Print("[%s] CI_TASK_CAM_EXTRACTED (%d)\n", __func__,index);
#else
									ret = CIMAX_PowerOffCam (index);
									DI_UART_Print("[%s] CI_TASK_CAM_EXTRACTED (%d : %d)\n", __func__,index,ret);
#endif
									caminfo.usStatus[index] = DI_CI_EXTRACTED;

									DRV_CI_GetPathStatus( index, &ciConnectStatus);
									if((ciConnectStatus.used == 1)&& (ciConnectStatus.connectStatus == CI_INSERTED))
									{
										ciConnectStatus.connectStatus = CI_EXTRACTED;
									}

									DRV_CI_SetPathStatus(index , &ciConnectStatus);

									_pfnCiCallbackFunc (index, caminfo.usStatus[index]);

									if(_pfnCiCallbackFuncforDemux != NULL)
									{
										_pfnCiCallbackFuncforDemux(index, caminfo.usStatus[index]);
									}

									caminfo.ucDetect[index] = 0;
									s_CiTaskState[index] = CI_TASK_IDLE;
									break;

								case CI_TASK_CAM_INSERTED:
									DI_UART_Print("[%s] CI_TASK_CAM_INSERTED (%d)\n", __func__,index);
									caminfo.usStatus[index] = DI_CI_INSERTED;
									_pfnCiCallbackFunc (index, caminfo.usStatus[index]);
									s_CiTaskState[index] = CI_TASK_CAM_POWER_ON;
									break;

								case CI_TASK_CAM_POWER_ON:
#if defined(CAM_SLOT_POWER_ALWAYS_ON)
									DI_UART_Print("[%s] CI_TASK_CAM_POWER_ON (%d)\n", __func__,index);
#else
									ret = CIMAX_PowerOnCam(index);
									DI_UART_Print("[%s] CI_TASK_CAM_POWER_ON (%d : %d)\n", __func__,index,ret);
									if(ret == 0)
									{
#endif
									s_CiTaskState[index] = CI_TASK_CAM_RESET;
#if defined(CAM_SLOT_POWER_ALWAYS_ON)
									/*Nothing to do*/
#else
									}
									else
									{
										s_CiTaskState[index] = CI_TASK_CAM_EXTRACTED;
									}
#endif
									break;

								case CI_TASK_CAM_RESET:
									ret = DRV_CIMAX_CAMReset(index);
									VK_printf("[%s] CI_TASK_CAM_RESET (%d:%d)\n", __func__,index,ret);
									if(ret == 0)
									{
										s_CiTaskState[index] = CI_TASK_CAM_ACCESSIBLE;
									}
									else
									{
										s_CiTaskState[index] = CI_TASK_CAM_EXTRACTED;
									}
									break;

								case CI_TASK_CAM_ACCESSIBLE:
									VK_printf("[%s] CI_TASK_CAM_ACCESSIBLE (%d)\n", __func__,index);
									caminfo.usStatus[index] = DI_CI_ENABLED;
									s_ulCiOnceFifoResetByInsert[index] = CAM_DETECT;
									_pfnCiCallbackFunc (index, caminfo.usStatus[index]);
									if(_pfnCiCallbackFuncforDemux != NULL)
									{
										_pfnCiCallbackFuncforDemux(index, caminfo.usStatus[index]);
									}

									DRV_CI_GetPathStatus( index, &ciConnectStatus);
									if((ciConnectStatus.used == 1)&& (ciConnectStatus.connectStatus == CI_EXTRACTED))
									{
										ciConnectStatus.connectStatus = CI_INSERTED;
									}

									DRV_CI_SetPathStatus(index , &ciConnectStatus);

									s_CiTaskState[index] = CI_TASK_IDLE;
									break;

								default:
									PrintError("[%s] Unknown CI TASK STATE!!!\n", __func__);
									break;
							}
						}

						if(s_CiTaskState[index] == CI_TASK_IDLE)
							VK_TASK_Sleep (CI_TASK_SLEEP_TIME);
						else
							VK_TASK_Sleep (CIMAX_SLEEP_MAX_TIME);

					}
				}
			}

			VK_TASK_Sleep (CIMAX_SLEEP_MAX_TIME);

		}
		else
		{
			VK_TASK_Sleep (CIMAX_SLEEP_MAX_TIME);
		}
	}
}

#if defined(CONFIG_TASKBASE_FW_DOWNLOAD)
void InitCiTask(void* pData)
{
	HINT32 err = 0;
	HULONG *pstCiInitTaskId;
	HULONG ulCurrentTaskId;

	pstCiInitTaskId = (HULONG*)pData;
	ulCurrentTaskId = *pstCiInitTaskId;

	PrintDebug("[InitCiTask] >>>>>>>>>>>Start...\n");
	/* Firmware Download */
	PrintDebug("[%s] CIMAX_DownloadFW Start!\n", __FUNCTION__);
	err = CIMAX_DownloadFW();
	if(err != 0)
	{
		PrintError("=== CIMAX_DownloadFW &&&(%d) ===\r\n",err);
		return;
	}
	PrintDebug("[%s] CIMAX_DownloadFW Done!\n", __FUNCTION__);

	VK_TASK_Sleep (CIMAX_SLEEP_SMALL_TIME);

	err = CIMAX_Config();
	if(err != 0)
	{
		PrintError("=== CIMAX_Config (%d) ===\r\n",err);
		return;
	}

	PrintDebug("[InitCiTask] >>>>>>>>>>>Done...\n");

	PrintDebug("+++VK_TASK_Destroy(s_ulCiInitTaskId) ulCurrentTaskId = %d\n", ulCurrentTaskId);
	err = VK_TASK_Destroy(ulCurrentTaskId);
	if(err != 0)
	{
		PrintError("[%s] === VK_TASK_Destroy error(%d) ===\r\n", __FUNCTION__, err);
		return;
	}
	PrintDebug("---VK_TASK_Destroy(s_ulCiInitTaskId)\n");

	while(1)	/* wait until self task detroy */
	{
		VK_TASK_Sleep(0xFFFFFFFF);
	}
}
#endif


/************************************************************************************************************************************/

static DRV_Error DRV_CIMAX_Init(void)
{

	DRV_Error eDrvError = DRV_OK;
	HINT32 vkErr = 0,err = 0;

	DRV_CI_InitPathStatus();
#if defined(CONFIG_EXT_CI_CTRL_INT_MODE)
#else
	vkErr = VK_EVENT_Create(&s_ulCiMaxEventId, "cimax_evt");
	if(vkErr != VK_OK)
	{
		PrintError("Error in VK_EVENT_Create()\n");
		return DRV_ERR;
	}

	eDrvError = DRV_GPIO_INTR_RegisterEventNotify(GPIO_ID_CIMAX_INT, DRV_CIMAX_IntrCallback, (void *)&s_ulCiMaxEventId, 0);
	if(eDrvError != DRV_OK)
	{
		PrintError("DRV_GPIO_INTR_RegisterEventNotify() - GPIO_ID_CIMAX_INT Error\n");
		goto Fail_S1;
	}
#endif
	eDrvError = DRV_GPIO_INTR_Enable(GPIO_ID_CIMAX_INT);
	if(eDrvError != DRV_OK)
	{
		PrintError("DRV_GPIO_INTR_Enable() - GPIO_ID_CIMAX_INT Error\n");
		goto Fail_S1;
	}

	eDrvError = DRV_SPI_SetSpiMode(TRUE, TRUE);
	if(eDrvError != DRV_OK)
	{
		PrintError("DRV_SPI_SetSpiMode() - DRV_SPI_SetSpiMode Error\n");
		goto Fail_S1;
	}

	vkErr = VK_SEM_Create(&s_ulCiSemId, "CIMAX_SEM", VK_SUSPENDTYPE_PRIORITY);
	if(vkErr != VK_OK)
	{
		PrintError("Error in VK_SEM_Create()\n");
		goto Fail_S1;
	}

	vkErr = VK_SEM_Create(&s_ulCiConnSemId, "CICON_SEM", VK_SUSPENDTYPE_PRIORITY);
	if(vkErr != VK_OK)
	{
		PrintError("Error in VK_SEM_Create()\n");
		goto Fail_S2;
	}

	vkErr = VK_MSG_Create(256, 4,"CIMsg",(void *)&s_ulCiMaxMsgId,VK_SUSPENDTYPE_FIFO);
	if(vkErr != VK_OK)
	{
		PrintError("Error in VK_MSG_Create()\n");
		goto Fail_S2_1;
	}

	CIMAX_InitCommandStatus();
	VK_MEM_Memset(s_cimaxBufferOut, 0, sizeof(s_cimaxBufferOut));
	VK_MEM_Memset(s_cimaxBufferIn, 0, sizeof(s_cimaxBufferIn));

	VK_TASK_Sleep (CIMAX_SLEEP_SMALL_TIME);

	eDrvError = DRV_CIMAX_HWReset();
	if(eDrvError != DRV_OK)
	{
		PrintError("DRV_CIMAX_HWReset() - DRV_CIMAX_HWReset Error\n");
		goto Fail_S3;
	}

	VK_TASK_Sleep (CIMAX_SLEEP_SMALL_TIME);

	err = CIMAX_InterruptInit();
	if(err != 0)
	{
		PrintError("=== CIMAX_InterruptInit &&&(%d) ===\r\n",err);
		goto Fail_S3;
	}

	VK_TASK_Sleep (CIMAX_SLEEP_SMALL_TIME);

#if defined(CONFIG_TASKBASE_FW_DOWNLOAD)
	if (VK_TASK_Create ( InitCiTask,
						(unsigned long)CI_TASK_PRIORITY,
						(unsigned long)DI_CI_TASK_STACKSIZE,
						(char *)"DRV_CIMAX_FWD",
						&s_ulCiInitTaskId,
						&s_ulCiInitTaskId,
						TRUE) != VK_OK)
	{
		PrintError("[%s] Error::ci_init - VK_TASK_Create \n", __func__);
		goto Fail_S3;
	}

	if (VK_TASK_Start (s_ulCiInitTaskId) != VK_OK)
	{
		PrintError("[%s] s_ulCiTaskId task start error \n\r", __func__);
		goto Fail_S4;
	}
#else
	/* Firmware Download */
	PrintDebug("[%s] CIMAX_DownloadFW Start!\n", __FUNCTION__);
	err = CIMAX_DownloadFW();
	if(err != 0)
	{
		PrintError("=== CIMAX_DownloadFW &&&(%d) ===\r\n",err);
		goto Fail_S3;
	}
	PrintDebug("[%s] CIMAX_DownloadFW Done!\n", __FUNCTION__);

	VK_TASK_Sleep (CIMAX_SLEEP_SMALL_TIME);

	err = CIMAX_Config();
	if(err != 0)
	{
		PrintError("=== CIMAX_Config (%d) ===\r\n",err);
		goto Fail_S3;
	}
#endif
	VK_TASK_Sleep (CIMAX_SLEEP_MAX_TIME);

	if (VK_TASK_Create ( P_CIMAX_Task,
						(unsigned long)CI_TASK_PRIORITY,
						(unsigned long)DI_CI_TASK_STACKSIZE,
						(char *)"DRV_CIMAX",
						(void *)NULL,
						&s_ulCiTaskId,
						FALSE) != VK_OK)
	{
		PrintError("[%s] Error::ci_init - VK_TASK_Create \n", __func__);
		goto Fail_S3;
	}

	if (VK_TASK_Start (s_ulCiTaskId) != VK_OK)
	{
		PrintError("[%s] s_ulCiTaskId task start error \n\r", __func__);
		goto Fail_S4;
	}

	return DRV_OK;

Fail_S4:
	VK_TASK_Destroy(s_ulCiTaskId);

Fail_S3:
	VK_MSG_Destroy(s_ulCiMaxMsgId);

Fail_S2_1:
	VK_SEM_Destroy(s_ulCiConnSemId);

Fail_S2:
	VK_SEM_Destroy(s_ulCiSemId);

Fail_S1:
#if defined(CONFIG_EXT_CI_CTRL_INT_MODE)
#else
	VK_EVENT_Destroy(s_ulCiMaxEventId);
#endif

	PrintError("[%s] Initizlie error \n\r", __func__);

	return DRV_ERR;

}

/*****************************************************************
 * Function	: DRV_CI_Init
 * Description
 * Inputs
 * Outputs
 * Returns
 *****************************************************************/
DRV_Error DRV_CI_Init (void)
{
	HUINT8 i = 0;
	DRV_Error ret = 0;

	PrintEnter();

	for ( i=0; i<CIMAX_CAM_MAX_SLOT_NUM; i++ )
	{
		caminfo.ucOpen[i]	= FALSE;
		caminfo.nDevId[i]	= -1;
		caminfo.usSlotId[i]	= 0;
		caminfo.ucDetect[i]	= CAM_NO_DETECT;
		caminfo.usStatus[i]	= DI_CI_EXTRACTED;
	}

	ret = DRV_CIMAX_Init();
	if( ret != DRV_OK)
	{
		PrintError("DRV_CIMAX_Init() Error\n");
		return DRV_ERR;
	}

	PrintExit();

	return DRV_OK;
}

/*****************************************************************
 * Function	: DRV_CI_CheckSlotId
 * Description
 * Inputs
 * Outputs
 * Returns
 *****************************************************************/
HINT32 DRV_CI_CheckSlotId (HUINT16 usSlotId)
{
	// 일단 슬롯 하나인 경우만 고려하여 코딩함.
	if ((caminfo.ucOpen[usSlotId] == TRUE) && (caminfo.usSlotId[usSlotId] == usSlotId))
	{
		return caminfo.nDevId[usSlotId];
	}
	else
	{
		return -1;
	}
}

/*****************************************************************
 * Function	: DRV_CI_GetCAMDetect
 * Description
 * Inputs
 * Outputs
 * Returns
 *****************************************************************/
HUINT8 DRV_CI_GetCAMDetect (HINT32 nDevId, HUINT16 slot)
{
	HINT32  ret = 0;
	HUINT32 result = 0;

	UNREFENCED_PARAM(nDevId);
	PrintEnter();

	ret = DRV_CIMAX_CAMDetected(slot,&result);

	if(ret == 0)
	{
		if(result == 1)
			return CAM_DETECT;
	}

	PrintExit();

	return	CAM_NO_DETECT;

}

/*****************************************************************
 * Function	: DRV_CI_GetCAMStatus
 * Description
 * Inputs
 * Outputs
 * Returns
 *****************************************************************/
HUINT16 DRV_CI_GetCAMStatus(HINT32 nDevId)
{
	HUINT16 retVal = 0;

	retVal = caminfo.usStatus[nDevId];

	return retVal;
}

/*****************************************************************
 * Function	: DRV_CI_GetALLCAMDetect
 * Description
 * Inputs
 * Outputs
 * Returns
 *****************************************************************/
DRV_Error DRV_CI_GetAllCAMDetect (HUINT32 *pAllCamDetect)
{
	HUINT32 index = 0,result = 0;
	HINT32 ret = 0;
	HUINT8 errflag = 0;

	for(index = 0; index < CIMAX_CAM_MAX_SLOT_NUM; index++)
	{
		ret = DRV_CIMAX_CAMDetected(index,&result);
		if(ret != 0)
		{
			errflag = 1;
			break;
		}
		else
		{
			if(result == 0)
			{
				errflag = 1;
				break;
			}
		}
	}

	if(errflag == 1)
	{
		*pAllCamDetect = 0;
	}
	else
	{
		*pAllCamDetect = 1;
	}

	return DRV_OK;

}


/*****************************************************************
 * Function	: DRV_CI_SWReset_CAM
 * Description
 * Inputs
 * Outputs
 * Returns
 *****************************************************************/
DRV_Error DRV_CI_SWReset_CAM( HUINT8 slotId )
{
	DRV_Error	Ret = DRV_OK;
	HINT32 ret = 0;
	HUINT8	msgData[4] = {0,};
	HUINT32 result = 0;

	ret = DRV_CIMAX_CAMDetected(slotId,&result);
	if((ret == 0)&&(result == CAM_DETECT))
	{
		VK_SEM_Get(s_ulCiSemId);
		msgData[0] = slotId;
		msgData[1] = ECOOMMAND_EXTRACT;
		result = VK_MSG_Send(s_ulCiMaxMsgId, msgData, 4);
		VK_SEM_Release(s_ulCiSemId);
	}
	else
	{
		Ret = DRV_ERR;
	}

	return Ret;

}

void DRV_CI_RegisterCallback (void (*CiCallbackFuncforDemux)(HUINT16 usSlotId, HUINT16 flag))
{
	_pfnCiCallbackFuncforDemux = CiCallbackFuncforDemux;
	return;
}

/**********************************************************************************************************************************/


/*****************************************************************
 * Function	: DRV_CI_GetInputValue
 * Description
 * Inputs
 * Outputs
 * Returns
 *****************************************************************/
DRV_Error DRV_CI_GetInputValue(HUINT32 slotId, HUINT32 *pInputValue)
{
	HUINT32 inputlogical = 0,inputphisical = 0, tmpInputValue = 0;
	DRV_Error	Ret = DRV_OK;

	if(pInputValue == NULL)
	{
		return DRV_ERR_INVALID_PARAMETER;
	}

	Ret = P_GetICInputInfo(&inputlogical, &inputphisical);

	tmpInputValue = inputphisical + slotId + 1;

	if(tmpInputValue > inputlogical)
	{
		return DRV_ERR_INVALID_PARAMETER;
	}

	*pInputValue = tmpInputValue;

	return DRV_OK;

}

DRV_Error P_GetICIEntranceInfo(DRV_CI_ENTRANCE_INFO_t *pEntranceInfo)
{

	if(pEntranceInfo == NULL)
	{
		return DRV_ERR_INVALID_PARAMETER;
	}

	pEntranceInfo->inputLogicalLimit  	= (CAM_MAXSLOTID * USED_LIVE +  CAM_MAXSLOTID * USED_PLAYBACK + CAM_MAXSLOTID * USED_SLOTOUTPUT);
	pEntranceInfo->inputPhysicalLimit 	= (CAM_MAXSLOTID * USED_LIVE +  CAM_MAXSLOTID * USED_PLAYBACK );
	pEntranceInfo->outputLogicalLimit	= (CAM_MAXSLOTID + CAM_MAXSLOTID * USED_SLOTOUTPUT);
	pEntranceInfo->outputPhysicalLimit 	= CAM_MAXSLOTID;

	return DRV_OK;
}

/*****************************************************************
 * Function	: DRV_CI_MakePath
 * Description
 * Inputs
 * Outputs
 * Returns
 *****************************************************************/
DRV_Error	DRV_CI_MakePath( HUINT32 tunerId, HUINT32 inputId, HUINT32 input, HUINT32 slotId, HUINT32 output, HUINT32 channelId, HUINT32 connectWay, HUINT32 *pResetFIFO )
{
	DRV_Error	Ret = DRV_OK;
	DRV_CI_PATH_STATUS_t tmpCIPathStatus;
	HINT32	result = 0;
	HUINT8	tmp = 0,cmp = 0,rdata = 0,wdata = 0,inputvalue = 0;
	DRV_CI_ENTRANCE_INFO_t tmpCIEntranceInfo;

	HUINT8 sameinput = 0;

	if(pResetFIFO == NULL)
	{
		goto FAIL;
	}

	VK_MEM_Memset(&tmpCIEntranceInfo,0x00,sizeof(DRV_CI_ENTRANCE_INFO_t));

	Ret = P_GetICIEntranceInfo(&tmpCIEntranceInfo);
	if(Ret != DRV_OK)
	{
		goto FAIL;
	}

	if(input > tmpCIEntranceInfo.inputLogicalLimit)
	{
		return DRV_ERR_INVALID_PARAMETER;
	}

	VK_MEM_Memset(&tmpCIPathStatus,0x00,sizeof(DRV_CI_PATH_STATUS_t));

	result = CIMAX_ReadRegister(ROUTER_CAM_MOD,&tmp,1);
	if(result != 0)
	{
		return result;
	}

	/* cam passthrough mode*/
	if(slotId != NOT_USED_VALUE)
	{
		if(slotId == 0)
		{
			tmp &= ~(IN_OF_CAM0);

			if(input > tmpCIEntranceInfo.inputPhysicalLimit)
			{
				tmp |= (IN_OTHER_CAMOUT);
			}
			else
			{
				if(channelId == 0)
				{
#if defined(USE_GAP_REMOVER)
					tmp |= IN_GAP_REMOVER0;
#else
					tmp |= (IN_CH0);
#endif
				}
				else
				{
#if defined(USE_GAP_REMOVER)
					tmp |= IN_GAP_REMOVER1;
#else
					tmp |= (IN_CH1);
#endif
				}
			}
		}
		else
		{
			tmp &= ~(IN_OF_CAM1);

			if(input > tmpCIEntranceInfo.inputPhysicalLimit)
			{
				cmp = (IN_OTHER_CAMOUT);
			}
			else
			{
				if(channelId == 0)
				{
#if defined(USE_GAP_REMOVER)
					cmp = IN_GAP_REMOVER0;
#else
					cmp = (IN_CH0);
#endif
				}
				else
				{
#if defined(USE_GAP_REMOVER)
					cmp = IN_GAP_REMOVER1;
#else
					cmp = (IN_CH1);
#endif
				}
			}
			cmp <<= SHIFT_IN_OF_SLOT;
			tmp |= cmp;
		}

		result = CIMAX_WriteRegister(ROUTER_CAM_MOD,&tmp,1);
		if(result != 0)
		{
			PrintError("== DRV_CI_MakePath1 Return Error 3(%d) ==\r\n",result);
			return result;
		}

		tmp = 0;
		result = CIMAX_ReadRegister(ROUTER_CAM_CH,&tmp,1);
		if(result != 0)
		{
			return result;
		}

		if(output < tmpCIEntranceInfo.outputPhysicalLimit)
		{
			if(slotId == 0)
			{
				if(channelId == 0)
				{
					tmp &= ~(OUT_OF_CH0);
					tmp |= IN_SELECT_CAM;
				}
				else
				{
					tmp &= ~(OUT_OF_CH1);
					cmp = IN_OTHER_CAM;
					cmp <<= SHIFT_OUT_OF_CH;
					tmp |= cmp;
				}
			}
			else
			{
				if(channelId == 0)
				{
					tmp &= ~(OUT_OF_CH0);
					tmp |= IN_OTHER_CAM;
				}
				else
				{
					tmp &= ~(OUT_OF_CH1);
					cmp = IN_SELECT_CAM;
					cmp <<= SHIFT_OUT_OF_CH;
					tmp |= cmp;
				}
			}

		}

		result = CIMAX_WriteRegister(ROUTER_CAM_CH,&tmp,1);
		if(result != 0)
		{
			PrintError("== DRV_CI_MakePath1 Return Error 4(%d) ==\r\n",result);
			return result;
		}

	}
	/* cam bypass mode */
	else
	{
		tmp = 0;
		cmp = 0;
		result = CIMAX_ReadRegister(ROUTER_CAM_CH,&tmp,1);
		if(result != 0)
		{
			PrintError("== DRV_CI_MakePath1 Return Error 5(%d) ==\r\n",result);
			return result;
		}

		if(output == NOT_USED_VALUE)
		{
			return -1;
		}
		else
		{
			if(channelId == 0)
		{
				tmp &= ~(OUT_OF_CH0);
				tmp |= IN_SELECT_CH0;
			}
			else
			{
				tmp &= ~(OUT_OF_CH1);
				cmp = IN_SELECT_CH1;
				cmp <<= SHIFT_OUT_OF_CH;
				tmp |= cmp;
			}

			result = CIMAX_WriteRegister(ROUTER_CAM_CH,&tmp,1);
			if(result != 0)
			{
				PrintError("== DRV_CI_MakePath1 Return Error 6(%d) ==\r\n",result);
				return result;
			}
		}

	}


	rdata = 0;
	result	= CIMAX_ReadRegister(IN_SEL,&rdata,1);
	if(result != 0)
	{
		PrintError("== DRV_CI_MakePath1 Return Error 7(%d) ==\r\n",result);
		return result;
	}

	cmp = rdata;
	tmp = rdata;
	wdata = rdata;

	if(input <= tmpCIEntranceInfo.inputPhysicalLimit )
	{
		if(input == CH_SRC_NULL)
			inputvalue = input;
		else
			inputvalue	= input + 3;

		if(inputvalue > CH0_SRC_MASK)
		{
			PrintError("== DRV_CI_MakePath1 Return Error 8 ==\r\n");
			return -1;
		}

		sameinput = 0;

		if(channelId == 0)
		{
			wdata &= ~(CH0_SRC_MASK);
			cmp &= ~(CH0_SRC_MASK);
			tmp &= (CH0_SRC_MASK);

			if(tmp == inputvalue)
			{
				sameinput = 1;
			}

			wdata |= inputvalue;
		}
		else
		{
			wdata &= ~(CH1_SRC_MASK);
			cmp &= ~(CH1_SRC_MASK);
			tmp &= (CH1_SRC_MASK);
			inputvalue <<= CH_IN_SHIFT;

			if(tmp == inputvalue)
			{
				sameinput = 1;
			}

			wdata |= inputvalue;

		}
	}
	else
	{
		sameinput = 1;
	}

	if((input == CI_INPUT_LIVE0)||(input == CI_INPUT_LIVE1))
	{

		Ret = DRV_SetTs2CIInput(tunerId,(inputId - 1));
		if(Ret != DRV_OK)
		{
			PrintError("== DRV_CI_MakePath1 Return Error 9 (%d) ==\r\n",Ret);
			goto FAIL;
		}
	}

	if(sameinput == 0)
	{
		result	= CIMAX_WriteRegister(IN_SEL,&wdata,1);
		if(result != 0 )
		{
			PrintError("== DRV_CI_MakePath1 Return Error 10 (%d) ==\r\n",result);
			return result;
		}

	}

	tmpCIPathStatus.used = 1;
	tmpCIPathStatus.usedTunerId = tunerId;
	tmpCIPathStatus.inputId = inputId;
	tmpCIPathStatus.inputSrc = input;
	tmpCIPathStatus.slotId = slotId;
	tmpCIPathStatus.outputSrc = output;
	tmpCIPathStatus.outputId = channelId;
	tmpCIPathStatus.connectWay = connectWay;
	tmpCIPathStatus.connectStatus = 1;

	if(connectWay != DI_CI_NORMAL_BYPASS)
	{
		Ret =  DRV_CI_SetPathStatus(slotId,&tmpCIPathStatus);
		if(Ret != DRV_OK)
		{
			PrintError("== DRV_CI_MakePath1 Return Error 11 (%d) ==\r\n",Ret);
			goto FAIL;
		}
	}
	else
	{
		Ret =  DRV_CI_SetPathStatus(channelId,&tmpCIPathStatus);
		if(Ret != DRV_OK)
		{
			PrintError("== DRV_CI_MakePath1 Return Error 12 (%d) ==\r\n",Ret);
			goto FAIL;
		}

	}

	if(sameinput == 0)
	{
		PrintDebug("===== @@@@@@ Have to Reset FIFO @@@@@@====\r\n");
		*pResetFIFO = 1;
	}
	else
	{
		*pResetFIFO = 0;
	}

	return DRV_OK;

FAIL:
	return DRV_ERR;

}


/*****************************************************************
 * Function	: DRV_CIMAX_ResetFIFO
 * Description
 * Inputs
 * Outputs
 * Returns
*****************************************************************/
DRV_Error DRV_CIMAX_ResetFIFO( HUINT32 slotId )
{
	DRV_CI_PATH_STATUS_t tmpCIPathStatus;
	DRV_Error drvErr = 0;
	HINT32 result = 0;

	if (slotId >= CIMAX_CAM_MAX_SLOT_NUM)
	{
		return DRV_ERR;
	}

	VK_MEM_Memset(&tmpCIPathStatus, 0x00, sizeof(DRV_CI_PATH_STATUS_t));

	drvErr = DRV_CI_GetPathStatus(slotId ,&tmpCIPathStatus);
	if(drvErr != 0)
	{
		return DRV_ERR;
	}

	if((tmpCIPathStatus.used == 1)&&(tmpCIPathStatus.connectStatus == CI_INSERTED)&&((tmpCIPathStatus.outputSrc == 0)||(tmpCIPathStatus.outputSrc == 1)))
	{
		result = CIMAX_ResetFIFO(tmpCIPathStatus.outputId);
		if(result != 0)
		{
			return DRV_ERR;
		}
	}

	return DRV_OK;
}

/*****************************************************************
 * Function	: DRV_CIMAX_SetCLK
 * Description
 * Inputs
 * Outputs
 * Returns
*****************************************************************/
DRV_Error DRV_CIMAX_SetCLK( HUINT32 slotId,HUINT32 flag )
{
	DRV_CI_PATH_STATUS_t tmpCIPathStatus;
	DRV_Error drvErr = 0;
	HINT32 result = 0;

	if (slotId >= CIMAX_CAM_MAX_SLOT_NUM)
	{
		return DRV_ERR;
	}

	VK_MEM_Memset(&tmpCIPathStatus, 0x00, sizeof(DRV_CI_PATH_STATUS_t));

	drvErr = DRV_CI_GetPathStatus(slotId ,&tmpCIPathStatus);
	if(drvErr != 0)
	{
		return DRV_ERR;
	}

	if((tmpCIPathStatus.used == 1)&&(tmpCIPathStatus.connectStatus == CI_INSERTED)&&((tmpCIPathStatus.outputSrc == 0)||(tmpCIPathStatus.outputSrc == 1)))
	{
		result = CIMAX_SetCLK(tmpCIPathStatus.outputId,flag);
		if(result != 0)
		{
			return DRV_ERR;
		}
	}

	return DRV_OK;
}

/*****************************************************************
 * Function	: DRV_CIMAX_ResetFIFOBySynchro
 * Description
 * Inputs
 * Outputs
 * Returns
*****************************************************************/
DRV_Error DRV_CIMAX_ResetFIFOBySynchro( HUINT32 slotId )
{
	DRV_CI_PATH_STATUS_t tmpCIPathStatus;
	DRV_Error drvErr = 0;
	HINT32 result = 0;
	HUINT8 rdata=0;
	HUINT8 desync=0;

	if (slotId >= CIMAX_CAM_MAX_SLOT_NUM)
	{
		return DRV_ERR;
	}

	VK_MEM_Memset(&tmpCIPathStatus, 0x00, sizeof(DRV_CI_PATH_STATUS_t));

	drvErr = DRV_CI_GetPathStatus(slotId ,&tmpCIPathStatus);
	if(drvErr != 0)
	{
		return DRV_ERR;
	}

	if((tmpCIPathStatus.used == 1)&&(tmpCIPathStatus.connectStatus == CI_INSERTED)&&((tmpCIPathStatus.outputSrc == 0)||(tmpCIPathStatus.outputSrc == 1)))
	{
		rdata = 0;
		result  = CIMAX_ReadRegister(TS_IT_STATUS,&rdata,1);

		if( tmpCIPathStatus.outputId == 0 )
			desync = DSYNC_0;
		else
			desync = DSYNC_1;


		if( (rdata & desync) || s_ulCiOnceFifoResetByInsert[slotId] == CAM_DETECT)
		{
			PrintError("[%d]%s slot %d is desynchroniztion (TP change) !!\n",__LINE__,__func__, slotId);
			result = CIMAX_ResetFIFO(tmpCIPathStatus.outputId);
			if(result != 0)
			{
				return DRV_ERR;
			}

			result  = CIMAX_WriteRegister(TS_IT_STATUS,&desync,1);
			if(result != 0)
			{
				return DRV_ERR;
			}

			if(s_ulCiOnceFifoResetByInsert[slotId] == CAM_DETECT)
				s_ulCiOnceFifoResetByInsert[slotId] = CAM_NO_DETECT;
		}

	}

	return DRV_OK;
}

////////////////////CI TEST CODE//////////////////////////////////
/* CIS management ***********************************************************/

/*--- tuples ---*/
#define TPL_VERS_1 0x15
#define TPL_CONFIG 0x1a
#define TPL_CFTABLE_ENTRY 0x1b
#define TPL_END 0xff
#define TPL_NO_LINK 0x14

#if 0 // org : Common Interface Minimum Tuples Set
#define CISTPL_DEVICE_0A		0x1d
#define CISTPL_DEVICE_0C		0x1c
#define CISTPL_VERS_1			0x15
#define CISTPL_MANFID			0x20
#define CISTPL_CONFIG			0x1a
#define CISTPL_CFTABLE_ENTRY	0x1b
#define CISTPL_NO_LINK			0x14
#define CISTPL_END				0xff
#else // 추가
/* Layer 1 - Basic Compatability Tuples */
#define	CISTPL_NULL				0x00	/* null tuple - ignore */
#define	CISTPL_DEVICE			0x01	/* device information */
#define	CISTPL_LONGLINK_CB		0x02	/* longlink to next tuple chain */
#define	CISTPL_CONFIG_CB		0x04	/* configuration tuple */
#define	CISTPL_CFTABLE_ENTRY_CB	0x05	/* configuration table entry */
#define	CISTPL_LONGLINK_MFC		0x06	/* multi-function tuple */
#define	CISTPL_BAR				0x07	/* Base Address Register definition */
#define	CISTPL_CHECKSUM			0x10	/* checksum control */
#define	CISTPL_LONGLINK_A		0x11	/* long-link to AM */
#define	CISTPL_LONGLINK_C		0x12	/* long-link to CM */
#define	CISTPL_LINKTARGET		0x13	/* link-target control */
#define	CISTPL_NO_LINK			0x14	/* no-link control */
#define CISTPL_VERS_1			0x15	/* level 1 version information */
#define	CISTPL_ALTSTR			0x16	/* alternate language string */
#define	CISTPL_DEVICE_A			0x17	/* AM device information */
#define	CISTPL_JEDEC_C			0x18	/* JEDEC programming info for CM */
#define	CISTPL_JEDEC_A			0x19	/* JEDEC programming info for AM */
#define CISTPL_CONFIG			0x1a	/* configuration */
#define CISTPL_CFTABLE_ENTRY	0x1b	/* configuration-table-entry */
#define CISTPL_DEVICE_0C		0x1c	/* other op conditions CM device info */
#define CISTPL_DEVICE_0A		0x1d	/* other op conditions AM device info */
#define	CISTPL_DEVICEGEO		0x1e	/* Common Memory device geometry */
#define	CISTPL_DEVICEGEO_A		0x1f	/* Attribute Memory device geometry */
#define CISTPL_MANFID			0x20	/* manufacturer identification */
#define	CISTPL_FUNCID			0x21	/* function identification */
#define	CISTPL_FUNCE			0x22	/* function extension */
/* Layer 2 - Data Recording Format Tuples */
#define	CISTPL_SWIL				0x23	/* software interleave */
#define	CISTPL_VERS_2			0x40	/* level 2 version information */
#define	CISTPL_FORMAT			0x41	/* Common Memory recording format */
#define	CISTPL_GEOMETRY			0x42	/* geometry */
#define	CISTPL_BYTEORDER		0x43	/* byte order */
#define	CISTPL_DATE				0x44	/* card initialization date */
#define	CISTPL_BATTERY			0x45	/* battery replacement date */
#define	CISTPL_FORMAT_A			0x47	/* Attribute Memory recording format */
/* Layer 3 - Data Organization Tuples */
#define	CISTPL_ORG				0x46	/* organization */
/* Layer 4 - System Specific Standard Tuples */
#define	CISTPL_SPCL				0x90	/* special-purpose tuple */
#define CISTPL_END				0xff	/* end-of-list tuple */
#endif

/*--- subtuples ---*/

#define	CCST_CIF		0xc0
#define	STCE_EV			0xc0
#define	STCE_PD			0xc1

/*--- offsets in tuple parameters (without the 2 first bytes tuple and link) ---*/

/* CISTPL_VERS_1 */
#define TPLLV1_MAJOR		0
#define TPLLV1_MINOR		1
#define TPLLV1_INFO			2

/* CISTPL_CONFIG */
#define TPCC_SZ				0
#define TPCC_LAST			1
#define TPCC_RADR			2

/* CISTPL_CFTABLE_ENTRY */
#define TPCE_INDX			0
#define TPCE_IF				1
#define TPCE_FS				2
#define TPCE_PD				3

/*--- bit masks ---*/

/* CISTPL_CONFIG */
#define TPCC_RASZ			0x03
#define TPCC_RMSZ			0x3c

/* CISTPL_CFTABLE_ENTRY */
#define TPCE_INDX_MASK		0xc0
#define TPCE_INTFACE		0x80
#define TPCE_FS_MASK		0x0b
#define TPCE_FS_POWER		0x03
#define TPCE_PD_MASK		0x01
#define TPCE_EXT			0x80
#define TPCE_TIMING			0x04
#define TPCE_TIMING_WAIT	0x03
#define TPCE_TIMING_READY	0x1c
#define TPCE_IO				0x08
#define TPCE_IO_RANGE		0x80
#define TPCE_IO_SLN			0xc0
#define TPCE_IO_SLNR		0x06
#define TPCE_IO_SAN			0x30
#define TPCE_IO_SANR		0x04
#define TPCE_IR				0x10
#define TPCE_IR_MASK		0x10
#define TPCE_MEM			0x60
#define TPCE_MEM_SL			0x18
#define TPCE_MEM_SLR		0x03
#define TPCE_MEM_SCA		0x60
#define TPCE_MEM_SCAR		0x05
#define TPCE_MEM_HPA		0x80

/*--- values ---*/

/* CISTPL_VERS_1 */
#define TPLLV1_MAJOR_VAL	0x05
#define TPLLV1_MINOR_VAL	0x00

/* CISTPL_CONFIG */
#define TPCC_RMSK			0x01
#define NBRBYTES			0x01
#define MAXADDR				0xffe

/* CISTPL_CFTABLE_ENTRY */
#define TPCE_IF_VAL			0x04
#define DVB_INDEX			0x0f

/* CISTPL_NO_LINK */
#define	CISTPL_NOLINKLEN	0x00

/* CCST_CIF */
#define CCST_CIFLEN			0x0e
#define CCST_CIF1			0x41
#define CCST_CIF2			0x02
#define	STCI_STR			"DVB_CI_V"
#define	STCI_STRP			0x2e

/* STCE_EV */
#define STCE_EV_VAL		"DVB_HOST"

/* STCE_PD */
#define STCE_PD_VAL		"DVB_CI_MODULE"



#define NEXUS_DVB_CI_MANUFACTURER_NAME_MAX 128
#define NEXUS_DVB_CI_PRODUCT_NAME_MAX 128
#define NEXUS_DVB_CI_ADDITIONAL_INFO_MAX 128

#define INCREMENT_INDEX(i,val,next) do { if ( (i)+(val) >= (1024*1024) || (i)+(val)>(next) ) goto overflow; (i) += (val); } while (0)

HCHAR manufacturerName[NEXUS_DVB_CI_MANUFACTURER_NAME_MAX];
HCHAR productName[NEXUS_DVB_CI_PRODUCT_NAME_MAX];
HCHAR additionalInfo[NEXUS_DVB_CI_ADDITIONAL_INFO_MAX];
HUINT8 corValue;
HUINT32  corAddr;
HBOOL validCis;
HBOOL vpp5v;

HINT32 DRV_CI_ParseCis(HUINT8 *paramCIS)
{
	HUINT8 tuple, cislink;
	HUINT8 noLinkFlag=0;
    HINT32 i, j, k, nextLink, cc=0;
	HUINT8 byte;
    bool foundCor=false, foundCen=false;

#if 1
    DI_UART_Print("CIS DUMP\n");
    for ( i = 16; i < 256; i+=16 )
    {
    	k=0;
	DI_UART_Print("[0x%03x] ", (i-16));
	while(k<2)
	{
		if(k==0)
		{
			for(j=(i-16); j<i;j++)
			{
				DI_UART_Print("0x%02x ",paramCIS[j]);
			}
			k++;
		}
		else if(k==1)
		{
			DI_UART_Print("| ");
			for(j=(i-16); j<i;j++)
			{
				if(paramCIS[j] >= 0x20)
					DI_UART_Print("%c",paramCIS[j]);
				else
					DI_UART_Print(".");
			}
			k++;
		}
	}
	DI_UART_Print("\n");
    }
#endif

    for ( i = 0;; )
    {
        DI_UART_Print("CIS Parsing - offset %d\n", i);
        tuple = paramCIS[i];
        i++;
        cislink = paramCIS[i];
        i++;
        nextLink = i + cislink;

        switch ( tuple )
        {
        case TPL_VERS_1:
            DI_UART_Print("TPL_VERS_1: major %x, minor %x.", paramCIS[i], paramCIS[i+1]);
            INCREMENT_INDEX(i,2,nextLink);
            j=0;
            while ( (byte = paramCIS[i]) )
            {
                if (byte == 0xff)
                    break;
                if ( j < NEXUS_DVB_CI_MANUFACTURER_NAME_MAX-1 )
                {
                    manufacturerName[j++] = byte;
                }
                INCREMENT_INDEX(i,1,nextLink);
            }
            /* Skip NUL byte */
            INCREMENT_INDEX(i,1,nextLink);
            DI_UART_Print("Manufacturer Name: '%s'", manufacturerName);
            j=0;
            while ( (byte = paramCIS[i]) )
            {
                if (byte == 0xff)
                    break;
                if ( j < NEXUS_DVB_CI_PRODUCT_NAME_MAX-1 )
                {
                    productName[j++] = byte;
                }
                INCREMENT_INDEX(i,1,nextLink);
            }
            /* Skip NUL byte */
            INCREMENT_INDEX(i,1,nextLink);
            DI_UART_Print("Product Name: '%s'", productName);
            j=0;
            while ( (byte = paramCIS[i]) )
            {
                if (byte == 0xff)
                    break;
                if ( j < NEXUS_DVB_CI_ADDITIONAL_INFO_MAX-1 )
                {
                    additionalInfo[j++] = byte;
                }
                INCREMENT_INDEX(i,1,nextLink);
            }
            DI_UART_Print("Additional Info: '%s'", additionalInfo);
            break;

        case TPL_CONFIG:
        {
            unsigned char rmsz, rasz, last;
            unsigned int  corVal;

            rmsz = ((paramCIS[i]&0x3c)>>2);
            rasz = (paramCIS[i]&0x03);
            INCREMENT_INDEX(i,1,nextLink);
            last = (paramCIS[i]&0x3f);
            INCREMENT_INDEX(i,1,nextLink);

            DI_UART_Print("rmsz = %x, rasz = %x, last = %x ", rmsz, rasz, last);

            corVal = 0;
            for (j=0; j<=rasz; j++)
                corVal |= ((paramCIS[i+j])<<(j*8));
            corVal &= 0x03ffffff;
            DI_UART_Print("cor address offset 0x%x ", corVal);
            corAddr = corVal;
            INCREMENT_INDEX(i,j,nextLink);

            if ( (paramCIS[i] & 0x01) == 0 )
            {
                DI_UART_Print(("COR not present "));
            }
            foundCor = true;
            INCREMENT_INDEX(i,(rmsz+1),nextLink);
            if ( paramCIS[i] != CCST_CIF )
            {
                DI_UART_Print("CCST_CIF not present ");
                return -1;
            }
            INCREMENT_INDEX(i,2,nextLink);
            #if 0
            /* CableCard - useful for HPNx testing */
            if ( handle->pBaseAddress[i] != 0x41 || handle->pBaseAddress[i+2] != 0x03 )
            #else
            /* DVB-CI */
            if ( paramCIS[i] != 0x41 || paramCIS[i+1] != 0x02 )
            #endif
            {
                DI_UART_Print("CCST_CIF IFN not correct %x %x ", paramCIS[i], paramCIS[i+1]);
                return -1;
            }

            #if 0
            /* Useful for debug */
            INCREMENT_INDEX(i,4,nextLink);
            BDBG_MSG(("CCST_CIF: Interface Description: "));
            while ( handle->pBaseAddress[i] && i < nextLink)
            {
                BDBG_MSG(("   %c", handle->pBaseAddress[i]));
                INCREMENT_INDEX(i,2,nextLink);
            }
            #endif
        }
        break;
        case TPL_CFTABLE_ENTRY:
        {
            unsigned char fs, pdParSel, normv;

            corValue = 0x40 | (paramCIS[i] & 0x3f);
            foundCen = true;
            DI_UART_Print("CISTPL_CFTABLE_ENTRY: TPCE_INDX %x ", corValue);
            INCREMENT_INDEX(i,1,nextLink);  /* Points to TPCE_IF */
            DI_UART_Print("CISTPL_CFTABLE_ENTRY: TPCE_IF %x ", paramCIS[i]);
            INCREMENT_INDEX(i,1,nextLink);  /* Points to TPCE_FS */
            fs = paramCIS[i];
            DI_UART_Print("CISTPL_CFTABLE_ENTRY: TPCE_FS %x ", fs);

            fs &= 0x03; /* extract PD. */
            switch (fs)
            {
            case 0x02: /* VCC and VPP PD. */
                INCREMENT_INDEX(i,1,nextLink);  /* points to VCC PD par sel */
                pdParSel = paramCIS[i];
                DI_UART_Print("CISTPL_CFTABLE_ENTRY: VCC TPCE_PD Par Sel %x ", pdParSel);
                INCREMENT_INDEX(i,1,nextLink);  /* points to VCC PD par */
                for (j=0; j<8; j++)
                    if ((1<<j) & pdParSel)
                    {
                        while (paramCIS[i] & 0x80)
                            INCREMENT_INDEX(i,1,nextLink);
                        INCREMENT_INDEX(i,1,nextLink);
                    }
                pdParSel = paramCIS[i];
                INCREMENT_INDEX(i,1,nextLink);
                DI_UART_Print("CISTPL_CFTABLE_ENTRY: VPP TPCE_PD Par Sel %x ", pdParSel);
                if (pdParSel & 1)
                {
                    normv = paramCIS[i];
                    DI_UART_Print("CISTPL_CFTABLE_ENTRY: VPP TPCE_PD normV %x ", normv);
                    if ((normv & 0x07) == 5)
                    {
                        /* exponent is 1V. */
                        normv >>= 3;
                        normv &= 0x0f;
                        if (normv >= 6 && normv < 8) /* 6:3, 7:3.5. */
                        {
                            DI_UART_Print("CISTPL_CFTABLE_ENTRY: VPP norminal 3.3V ");
                            vpp5v = FALSE;
                        }
                        else if(normv >= 0xa && normv < 0xc) /* a:5, b:5.5 */
                        {
                            DI_UART_Print("CISTPL_CFTABLE_ENTRY: VPP norminal 5V ");
                        }
                        else
                            DI_UART_Print("CISTPL_CFTABLE_ENTRY: VPP TPCE_PD normV Mantissa out of range! use 5V for Vpp by default. ");
                    }
                    else
                        DI_UART_Print("CISTPL_CFTABLE_ENTRY: VPP TPCE_PD normV exponent not 1V! use 5V for Vpp by default. ");
                }
                else
                    DI_UART_Print("CISTPL_CFTABLE_ENTRY: NO VPP TPCE_PD normV, use 5V for Vpp by default. ");

                break;

            default:
                DI_UART_Print("CISTPL_CFTABLE_ENTRY: NO TPCE_PD for VPP, use 5V for Vpp by default. ");
                break;
            }
        }
		break;
        case TPL_NO_LINK:
            noLinkFlag = 1;
            break;
        case TPL_END:
            if ( noLinkFlag == 0 )
                DI_UART_Print("have not rcved NO_LINK before END ");
            goto done;
        default:
            if (cc++ < 50)
            {
                DI_UART_Print("CIS Tuple %x received. ", tuple);
            }
            else
            {
                DI_UART_Print("Too many tuples ");
                return -1;
            }
            break;
        }

        i = nextLink;
    }

done:
    if ( !foundCor )
    {
        DI_UART_Print("Did not find CISTPL_CONFIG ");
        return -1;
    }
    if ( !foundCen )
    {
        DI_UART_Print("Did not find CISTPL_CFTABLE_ENTRY ");
        return -1;
    }

    validCis = true;

    return 0;

overflow:
    DI_UART_Print("Overflow processing CIS ");
    return -1;
}

HINT32 DRV_CI_PHYS_Tuple(unsigned mtuple, HUINT8 tuple, HUINT8 **pcis, HINT32 *len, HINT32 *first, HINT32 *itpcc_radr, HINT32 *dvb, HINT32 *last)
{
	HINT32 ocis;						/* current offset in the CIS */
	HINT32 link;						/* tuple link				 */
	HINT32 found;						/* 1 if the current tuple is found, else 0 */
	HUINT8 ctuple;			/* current tuple			 */
	HINT32 sublen; 					/* intermediate length		 */
	HUINT8 *subpcis; 		/* pointer to subtuple		 */
	HINT32 nb, nbt0 = 0, nbt1 = 0, i;	/* intermediate variables	 */
	HINT32 ctpce_number = 0;			/* Configuration Index Value */
	HCHAR ctpcc_sz;					/* tuple's elementary item   */
	HCHAR lastentry; 				/* 1 if the last entry is found */
	HCHAR ctpcc_rasz;
	HCHAR ctpcc_rmsz;
	HCHAR cpce_pd;
#if 1 /* #ifdef CI_PLUS_ENABLE */ /* CI+ 지원 모델이 아니더라도 CI+ 캠이 삽입될 수 있고 이에 대한 핸들링이 필요할 수도 있으므로 범용적으로 정의/처리해 둔다 (스펙에도 부합됨) */
	HINT32 ciplus = 0;
#endif

	ocis  = 0;
	found = 0;

	do
	{

		ctuple = **pcis;
		link   = *(*pcis+1);
		*pcis += 2;
		ocis  += 2;


		if (tuple == ctuple)
		{
			found = 1;
			/* tuple found ; control it */

			switch (tuple) {
			case CISTPL_VERS_1 :
				if (*(*pcis + TPLLV1_MAJOR) != TPLLV1_MAJOR_VAL)	/* Control */
				{
					PrintError("PHYS_Tuple : not found or wrong TPLLV1_MAJOR\n");
					return -1;	/* wrong TPLLV1_MAJOR */			/* ======= */
				}
				if (*(*pcis + TPLLV1_MINOR) != TPLLV1_MINOR_VAL)	/* Control */
				{
					PrintError("PHYS_Tuple : not found or wrong TPLLV1_MINOR\n");
					return -1;	/* wrong TPLLV1_MINOR */			/* ======= */
				}
#if 1 /* #ifdef CI_PLUS_ENABLE */ /* CI+ 지원 모델이 아니더라도 CI+ 캠이 삽입될 수 있고 이에 대한 핸들링이 필요할 수도 있으므로 범용적으로 정의/처리해 둔다 (스펙에도 부합됨) */
				{
					/* check TPLLV1_INFO's "Additional Product Information" includes "ciplus=1" with case insensitive parsing */

					unsigned char info_str[256]; // maybe max tuple data length is 254
					int info_len;

					info_len = link - TPLLV1_INFO; // info_len 가 254 를 넘을 일이 없으므로 이에 대한 에러 핸들링은 할 필요가 없지...
					if (info_len > 0)
					{
						for (i = 0; i < info_len; i++)
						{
							if (*(*pcis + TPLLV1_INFO + i) == CISTPL_NULL)
								info_str[i] = ' ';
							else if (*(*pcis + TPLLV1_INFO + i) == CISTPL_END)
								info_str[i] = 0x00;
							else
								info_str[i] = *(*pcis + TPLLV1_INFO + i);
						}
						for (i = 0; i < info_len; i++)
						{
							info_str[i] = VK_tolower(info_str[i]);
						}
						if (VK_strcmp((char *)info_str, "ciplus=1") == 0)
						{
							ciplus = 1;
						}
						else
						{
							PrintDebug("[CIS] CIv1 or CI+ CAM but doesn't meet CI+ spec - not found ciplus=1\n");
						}
					}
					else
					{
						PrintDebug("[CIS] CIv1 or CI+ CAM but doesn't meet CI+ spec - not found or wrong TPLLV1_INFO\n");
					}
				}
#endif // #ifdef CI_PLUS_ENABLE
				break;

			case CISTPL_CONFIG :
				ctpcc_sz = *(*pcis + TPCC_SZ);
				ctpcc_rasz = ctpcc_sz & TPCC_RASZ;		/* number of bytes of Configuration Register Base Address */
				if (ctpcc_rasz > NBRBYTES)				/* Control : number = 0 or 1 means 1 or 2 bytes --> address between 0 and 0xfff */
				{
					PrintError("PHYS_Tuple : wrong Configuration Register Base Address (ctpcc_rasz = 0x%x)\n", ctpcc_rasz);
					return -1;							/* ======= wrong Configuration Register Base Address */
				}

				ctpcc_rmsz = ctpcc_sz & TPCC_RMSZ;
				*last  = *(*pcis + TPCC_LAST);			/* index number of last entry in Card configuration Table */
				if (ctpcc_rasz == 0)
					*itpcc_radr = *(*pcis + TPCC_RADR);
				else
					*itpcc_radr = *(*pcis + TPCC_RADR) + (*(*pcis + TPCC_RADR + 1) << 8);
				if (*itpcc_radr > MAXADDR)				/* Control */
				{
					PrintError("PHYS_Tuple : wrong Configuration Register Base Address (*itpcc_radr = 0x%x)\n", *itpcc_radr);
					return -1;							/* ======= wrong Configuration Register Base Address */
				}
				if (! ((*(*pcis + TPCC_RADR + ctpcc_rasz + 1)) & TPCC_RMSK))	/* Control */
				{
					PrintError("PHYS_Tuple : COR not present\n");
					return -1;							/* COR not present		   ======= */
				}

				nb = TPCC_RADR + ctpcc_rasz + 1 + ctpcc_rmsz + 1;
				subpcis = *pcis + nb;
				sublen = *len - ocis - (link - nb);

				/* control CCST_CIF subtuple */
				PrintDebug("PHYS_Tuple : CCST_CIF\n");
				if (DRV_CI_PHYS_Tuple(tuple, CCST_CIF, &subpcis, &sublen, first, itpcc_radr, dvb, last) < 0)
				{
					PrintError("PHYS_Tuple : not found or wrong CCST_CIF\n");
					return -1;		/* Control not found or wrong CCST_CIF */
				}
				break;				/* =======							   */

			case CISTPL_CFTABLE_ENTRY :
				if (*first & !((*(*pcis + TPCE_INDX)) & TPCE_INDX_MASK))	/* Intface bit and Default bit must be present in the first entry */
				{
					PrintError("PHYS_Tuple : not found or wrong CISTPL_CFTABLE_ENTRY\n");
					return -1;		/* Control wrong first CISTPL_CFTABLE_ENTRY tuple */
				}
									/* =======										  */
				ctpce_number = *(*pcis + TPCE_INDX) & ~TPCE_INDX_MASK;	/* configuration Entry Number to write to the COR */

				*first = 0;

				if (*(*pcis + TPCE_INDX) & TPCE_INTFACE)
				{
					if (*(*pcis + TPCE_IF) != TPCE_IF_VAL)				/* Control : TPCE_IF must be equal to TPCE_IF_VAL */
					{													/* =======										  */
						PrintError("PHYS_Tuple : not found or wrong TPCE_IF_VAL\n"); // CISTPL_CFTABLE_ENTRY 가 2개 이상인 경우 통상 이 상황이 발생하므로 PrintDbg 로 프린트 한다.
						lastentry = (char)-2; 							/* wrong CISTPL_CFTABLE_ENTRY tuple */
						break;
					}
				}
				else
				{
					PrintError("PHYS_Tuple : not found or wrong TPCE_IF\n");
					lastentry = (char)-2; 	/* Control : wrong CISTPL_CFTABLE_ENTRY tuple */
					break;					/* =======									  */
				}

				if (!(*(*pcis + TPCE_FS) & TPCE_FS_MASK))	/* Control : I/O Space and power must be present */
				{											/* =======										 */
					PrintError("PHYS_Tuple : not found or wrong TPCE_FS_MASK\n");
					lastentry = (char)-2; 					/* wrong CISTPL_CFTABLE_ENTRY tuple */
					break;
				}

				nb = 0;

				if (*(*pcis + TPCE_FS) & TPCE_FS_POWER)
				{
					cpce_pd = *(*pcis + TPCE_PD);
					/* find the number of bytes coding the TPCE_PD (power)	*/
					for (i=0; i < 8; i++)
					{
						if (cpce_pd & TPCE_PD_MASK) /* one set bit means one following byte */
							nb++;
						cpce_pd = cpce_pd >> 1;
					}
					/* skip the TPCE_PD ; if the bit 7 of one power byte is set then a additional byte is present */
					for (i=TPCE_PD + 1; i <= TPCE_PD + nb; i++)
					{
						if (*(*pcis + i) & TPCE_EXT)
							nb++;
					}	/* end of power configuration skipping */
					nb++;		/* count the Power Description Structure Parameter Selection Byte */
				}

				if (*(*pcis + TPCE_FS) & TPCE_TIMING)
				{
					/* skip the timing configuration */
					if ((*(*pcis + TPCE_PD + nb) & TPCE_TIMING_WAIT) != TPCE_TIMING_WAIT)
					{
						nbt0 = 0;
						while (*(*pcis + TPCE_PD + nb + nbt0 + 1) & TPCE_EXT)
							nbt0++;
						nbt0++;
					}
					if ((*(*pcis + TPCE_PD + nb) & TPCE_TIMING_READY) != TPCE_TIMING_READY)
					{
						nbt1 = 0;
						while (*(*pcis + TPCE_PD + nb + nbt0 + nbt1 + 1) & TPCE_EXT)
							nbt1++;
						nbt1++;
					}
					nb = nb + nbt0 + nbt1 + 1;
				}	/* end of timing configuration skipping */

				/* skip the I/O Space */
				if (*(*pcis + TPCE_FS) & TPCE_IO)
				{
					if (*(*pcis + TPCE_PD + nb) & TPCE_IO_RANGE)
					{
						nbt0 = (*(*pcis + TPCE_PD + nb + 1) & TPCE_IO_SLN) >> TPCE_IO_SLNR;
						nbt1 = (*(*pcis + TPCE_PD + nb + 1) & TPCE_IO_SAN) >> TPCE_IO_SANR;
						nb = nb + nbt0 + nbt1 + 1;	/* + 1 to skip the I/O Range Descriptor */
					}
					nb++;	/* skip the I/O Space descriptor */
				}	/* end of I/O space configuration skipping */

				/* skip the IR Configuration */
				if (*(*pcis + TPCE_FS) & TPCE_IR)
				{
					if (*(*pcis + TPCE_PD + nb) & TPCE_IR_MASK)
						nb += 2;
					nb++;
				}	/* end of IR configuration skipping */

				/* skip the Mem space Configuration */
				if (*(*pcis + TPCE_FS) & TPCE_MEM)
				{
					nbt0 = (*(*pcis + TPCE_PD + nb) & TPCE_MEM_SL) >> TPCE_MEM_SLR;
					nbt1 = (*(*pcis + TPCE_PD + nb) & TPCE_MEM_SCA) >> TPCE_MEM_SCAR;
					if (*(*pcis + TPCE_PD + nb) & TPCE_MEM_HPA)
						nbt1 *= 2;
					nb = nb + nbt0 + nbt1 + 1;
				}	/* end of Mem space configuration skipping */

				/* Control the STCE_EV subtuple */
				nb = TPCE_PD + nb;
				subpcis = *pcis + nb;
				sublen = *len - ocis - nb;
				PrintDebug("PHYS_Tuple : STCE_EV\n");
				if (DRV_CI_PHYS_Tuple(tuple, STCE_EV, &subpcis, &sublen, first, itpcc_radr, dvb, last) < 0)
				{						/* Control */
					lastentry = (char)-2; 	/* ======= not found or wrong STCE_EV */
					break;
				}

				/* Control the STCE_PD subtuple */
				PrintDebug("PHYS_Tuple : STCE_PD\n");
				if (DRV_CI_PHYS_Tuple(tuple, STCE_PD, &subpcis, &sublen, first, itpcc_radr, dvb, last) < 0)
				{						/* Control */
					lastentry = (char)-2; 	/* ======= not found or wrong STCE_PD */
					break;
				}

				*dvb = ctpce_number;		/* the DVB entry is present */

				break;

			case CISTPL_NO_LINK :
				if (link != CISTPL_NOLINKLEN)	/* Control */
				{
					PrintError("PHYS_Tuple : not found or wrong CISTPL_NOLINKLEN\n");
					return -1;					/* ======= wrong CISTPL_NO_LINK tuple */
				}
				break;

			/* subtuples */
			case CCST_CIF : 		/* if mtuple = CISTPL_CFTABLE_ENTRY then this tuple is STCE_EV */
				if (mtuple == CISTPL_CFTABLE_ENTRY)
				{	/* STCE_EV */
					if (VK_strcmp((char*)*pcis, STCE_EV_VAL)) 	/* Control */
					{
						PrintError("PHYS_Tuple : not found or wrong STCE_EV_VAL\n");
						return -1;						/* ======= wrong STCE_EV subtuple */
					}
				}
				else
				{
					if (**pcis != CCST_CIF1)	/* Control */
					{
						PrintError("PHYS_Tuple : not found or wrong CCST_CIF1\n");
						return -1;				/* ======= wrong CCST_CIF subtuple */
					}
					if (*(*pcis + 1) != CCST_CIF2)	/* Control */
					{
						PrintError("PHYS_Tuple : not found or wrong CCST_CIF2\n");
						return -1;					/* ======= wrong CCST_CIF subtuple */
					}

					/* STCI_STR must be "DVB_CI_Vx.xx" where x is digit */
					nb = VK_strlen(STCI_STR);
					if (VK_strncmp((char*)(*pcis + 2), STCI_STR, nb))	/* Control */
					{
						PrintError("PHYS_Tuple : not found or wrong STCI_STR\n");
						return -1;							/* ======= wrong CCST_CIF subtuple */
					}
					if (!VK_isdigit(*(*pcis + 2 + nb)))		/* Control */
					{
						PrintError("PHYS_Tuple : not found or wrong CCST_CIF subtuple - %d line\n", __LINE__);
						return -1;							/* ======= wrong CCST_CIF subtuple */
					}
					if (*(*pcis + 2 + nb + 1) != STCI_STRP) /* Control */
					{
						PrintError("PHYS_Tuple : not found or wrong STCI_STRP\n");
						return -1;							/* ======= wrong CCST_CIF subtuple */
					}
					if (!VK_isdigit(*(*pcis + 2 + nb + 2)))	/* Control */
					{
						PrintError("PHYS_Tuple : not found or wrong CCST_CIF subtuple - %d line\n", __LINE__);
						return -1;							/* ======= wrong CCST_CIF subtuple */
					}
					if (!VK_isdigit(*(*pcis + 2 + nb + 3)))	/* Control */
					{
						PrintError("PHYS_Tuple : not found or wrong CCST_CIF subtuple - %d line\n", __LINE__);
						return -1;							/* ====== wrong CCST_CIF subtuple */
					}
				}
				break;

			case STCE_PD :
				if (VK_strcmp((char*)(*pcis), STCE_PD_VAL)) /* Control */
				{
					PrintError("PHYS_Tuple : not found or wrong STCE_PD_VAL\n");
					return -1;					/* ======= wrong STCE_PD subtuple */
				}
				break;

			/* end-of-list tuple */
			case CISTPL_END :
				return 0;			/* good CISTPL_END tuple */

			default :
				/* other tuples : no control */
				break;
			}
		}

		/* skip the current tuple */
		*pcis += link;
		ocis += link;
		*len -= link;
		if (*len <= 0)	/* Control */
		{
			PrintError("PHYS_Tuple : tuple not found (ocis=%d, len=%d)\n", ocis, *len);
			return -1;		/* ======= tuple not found */
		}
		else
		{
			PrintDebug("PHYS_Tuple : next tuple=%02X following %d bytes from ctuple=%02X\n", **pcis, ocis, ctuple);
		}
	}
	while (!found);

	if (tuple == CISTPL_CFTABLE_ENTRY)
	{
		if (ctpce_number == *last || **pcis == CISTPL_NO_LINK) // if (ctpce_number == *last)
		{
			lastentry = 1;		/* end of one entry analysis, it's the last */
		}
		else
		{
			lastentry = 0;		/* not the last entry */
		}
		return lastentry;
	}
#if 1 /* #ifdef CI_PLUS_ENABLE */ /* CI+ 지원 모델이 아니더라도 CI+ 캠이 삽입될 수 있고 이에 대한 핸들링이 필요할 수도 있으므로 범용적으로 정의/처리해 둔다 (스펙에도 부합됨) */
	else if (tuple == CISTPL_VERS_1)
	{
		if (ciplus == 1)
			return 2;
		else
			return 1;
	}
#endif
	else
		return 1;
}

HINT32 DRV_CI_PHYS_CheckCIS(HUINT8 *cis, HINT32 len, HUINT16 *addrcor, HUINT8 *cor, HUINT8 *ciplus)
{
	HUINT8 *pcis;	/* pointer to the remainding CIS	*/
	HINT32 lcis;				/* length of the remainding CIS 	*/
	HINT32 first;				/* 1 for the first configuration table entry, else 0 */
	HINT32 addr;				/* configuration registers base address */
	HINT32 dvb;				/* index number of the DVB entry	*/
	HINT32 last;				/* index number of the last entry	*/
	HINT32 iRet;				/* return code						*/

	dvb = 0;

	pcis = cis;
	lcis = len;

	PrintDebug("PHYS_CheckCIS : CISTPL_DEVICE_0A\n");
	if (DRV_CI_PHYS_Tuple(0, CISTPL_DEVICE_0A, &pcis, &lcis, &first, &addr, &dvb, &last) < 0)
	{
		PrintError("PHYS_CheckCIS : not found or wrong CISTPL_DEVICE_0A\n");
		return -1;		/* not found or wrong CISTPL_DEVICE_0A		*/
	}

	PrintDebug("PHYS_CheckCIS : CISTPL_DEVICE_0C\n");
	if (DRV_CI_PHYS_Tuple(0, CISTPL_DEVICE_0C, &pcis, &lcis, &first, &addr, &dvb, &last) < 0)
	{
		PrintError("PHYS_CheckCIS : not found or wrong CISTPL_DEVICE_0C\n");
		return -1;		/* not found or wrong CISTPL_DEVICE_0C		*/
	}

	PrintDebug("PHYS_CheckCIS : CISTPL_VERS_1\n");
	if ((iRet = DRV_CI_PHYS_Tuple(0, CISTPL_VERS_1, &pcis, &lcis, &first, &addr, &dvb, &last)) < 0)
	{
		PrintError("PHYS_CheckCIS : not found or wrong CISTPL_VERS_1\n");
		return -1;		/* not found or wrong CISTPL_VERS_1 		*/
	}

	#if 1 /* #ifdef CI_PLUS_ENABLE */ /* CI+ 지원 모델이 아니더라도 CI+ 캠이 삽입될 수 있고 이에 대한 핸들링이 필요할 수도 있으므로 범용적으로 정의/처리해 둔다 (스펙에도 부합됨) */
	if (iRet == 2)
		*ciplus = 1;
	else
		*ciplus = 0;
	PrintDebug("PHYS_CheckCIS : ciplus=%d\n", *ciplus);
	#endif

	PrintDebug("PHYS_CheckCIS : CISTPL_MANFID\n");
	if (DRV_CI_PHYS_Tuple(0, CISTPL_MANFID, &pcis, &lcis, &first, &addr, &dvb, &last) < 0)
	{
		PrintError(("PHYS_CheckCIS : not found or wrong CISTPL_MANFID\n"));
		return -1;		/* not found or wrong CISTPL_MANFID 		*/
	}

	PrintDebug("PHYS_CheckCIS : CISTPL_CONFIG\n");
	if (DRV_CI_PHYS_Tuple(0, CISTPL_CONFIG, &pcis, &lcis, &first, &addr, &dvb, &last) < 0)
	{
		PrintError("PHYS_CheckCIS : not found or wrong CISTPL_CONFIG\n");
		return -1;		/* not found or wrong CISTPL_CONFIG 		*/
	}

	*addrcor = addr;
	first = 1;
	do
	{
		PrintDebug("PHYS_CheckCIS : CISTPL_CFTABLE_ENTRY\n");
		iRet = DRV_CI_PHYS_Tuple(0, CISTPL_CFTABLE_ENTRY, &pcis, &lcis, &first, &addr, &dvb, &last);
	}
	while ((iRet == 0) || (iRet == -2));		/* one more entry */
	if (iRet < 0)
	{
		PrintError("PHYS_CheckCIS : not found or wrong CISTPL_CFTABLE_ENTRY\n");
		return -1;		/* not found or wrong CISTPL_CFTABLE_ENTRY	*/
	}

	PrintDebug("PHYS_CheckCIS : addr=%x, dvb=%x, last=%x\n", addr, dvb, last);

	PrintDebug("PHYS_CheckCIS : CISTPL_NO_LINK\n");
	if (DRV_CI_PHYS_Tuple(0, CISTPL_NO_LINK, &pcis, &lcis, &first, &addr, &dvb, &last) < 0)
	{
		PrintError("PHYS_CheckCIS : not found or wrong CISTPL_NO_LINK\n");
		return -1;		/* not found or wrong CISTPL_NO_LINK 		*/
	}

	PrintDebug("PHYS_CheckCIS : CISTPL_END\n");
	if (DRV_CI_PHYS_Tuple(0, CISTPL_END, &pcis, &lcis, &first, &addr, &dvb, &last) < 0)
	{
		PrintError("PHYS_CheckCIS : not found or wrong CISTPL_END\n");
		return -1;		/* not found or wrong CISTPL_END			*/
	}

	/* the index of the DVB entry is the COR value */
	if (dvb != last)
	{
		PrintError("PHYS_CheckCIS : DVB entry found but... (dvb=%x, last=%x)\n", dvb, last);
	}

	*cor = dvb;

	return 1;			/* good CIS */
}

DRV_Error DRV_CI_InitForTest(HINT32 index)
{
	HUINT8 *cis; 			/* CIS buffer		 */
	HUINT16 addrcor = 0; 		/* COR Address		 */
	HUINT8 cor = 0;				/* COR value		 */
	HUINT8 ciplus;			/* CI+ CAM detected  */
	HINT32 iRet;						/* return code		 */
	HUINT16 cisLen;
	HUINT16 rcisLen = 0;
	HUINT8 cnt = 0;
	HUINT16 negociatedbufferSize = 0;

	cisLen = 1024; /* <TODO_CI> : 체크 요함 */ /* see CI_CIS_SIZE at the DRV_ci_drv.h */
	cis = (unsigned char *)VK_MEM_Alloc(cisLen);
	if(!cis)
	{
		DI_UART_Print("DRV_CI_InitForTest : not enought memory (%d) !!!\n", cisLen);
		return DRV_ERR;
	}

	iRet = DRV_CI_SWReset_CAM (index);
	if (iRet != 0)
	{
		VK_MEM_Free(cis);
		DI_UART_Print("DRV_CI_InitForTest : DRV_CI_ResetCAM error (%d) !!!\n", iRet);
		return DRV_ERR;
	}


	VK_MEM_Memset(cis, 0, cisLen);
	iRet = DRV_CIMAX_GetCIS(index,cis ,&rcisLen);
	if (iRet != 0)
	{
		VK_MEM_Free(cis);
		DI_UART_Print("DRV_CI_InitForTest : DRV_CI_ReadCIS error (%d) !!!\n", iRet);
		return DRV_ERR;
	}

	/* Check the CIS and find the COR Address */
	iRet = DRV_CI_PHYS_CheckCIS(cis, rcisLen, &addrcor, &cor, &ciplus);
	if ( iRet< 0)
	{
		DI_UART_Print("DRV_CI_InitForTest : PHYS_CheckCIS reports wrong CIS !!!\n");
		VK_MEM_Free(cis);
		return DRV_ERR;
	}
	VK_MEM_Free(cis);


	VK_TASK_Sleep(CIMAX_SLEEP_MAX_TIME*3);

AGAIN:
	iRet = DRV_CIMAX_WriteCOR(index,addrcor,cor);
	if (iRet != 0)
	{
		cnt++;

		if(cnt > 20)
		{
			return DRV_ERR;
		}
		else
		{
			VK_TASK_Sleep(CIMAX_SLEEP_MAX_TIME*6);
			goto AGAIN;

		}

	}

	iRet = DRV_CIMAX_Negotiate(index, 1024,&negociatedbufferSize);
	if (iRet == 0)
	{
		return DRV_OK;
	}
	else
		return DRV_ERR;
}


/*********************** End of File **********************************/




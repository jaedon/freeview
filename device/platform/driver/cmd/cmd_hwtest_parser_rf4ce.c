#if defined(CONFIG_VK_STDLIB)
#else
#include 	<stdio.h>
#include 	<stdlib.h>
#include 	<string.h>
#endif

#include "vkernel.h"
#include "htype.h"
#include "taskdef.h"
#include "cmdtool.h"

#include "di_err.h"
#include "di_uart.h"
#include "drv_err.h"

#include "drv_rf4ce.h"

#if defined(CONFIG_RF4CE_MICOM)
#include "drv_micom.h"
#include "di_rf4ce.h"
#if defined(CONFIG_RF4CE_MOTION_TOUCH_UEI_HILCREST)
#include "callbacks.h"
#include "uapi.h"
#include "drv_hilcrest.h"
#endif
#endif
/*******************************************************************/
/****************************** define *****************************/
/*******************************************************************/

#define CMD_RF4CE_DEBUG

#ifdef CMD_RF4CE_DEBUG
#define PrintDebug			DI_UART_Print
#define PrintError			DI_UART_Print
#else
#define PrintDebug			while (0) ((int (*)(char *, ...)) 0)
#define PrintError			DI_UART_Print
#endif

#ifdef CMD_RF4CE_ENTER_LEAVE_DEBUG
#define PrintEnter			DI_UART_Print("(+)%s\n", __FUNCTION__)
#define PrintExit				DI_UART_Print("(-)%s\n", __FUNCTION__)
#else
#define PrintEnter			while (0) ((int (*)(char *, ...)) 0)
#define PrintExit				while (0) ((int (*)(char *, ...)) 0)
#endif

#define AssertDI(arg1, arg2) if(arg1 != DI_ERR_OK) {DI_UART_Print(\
					"ERROR code : 0x%x, At %s() function, FILE: %s, LINE: %d\n",arg1, __func__, __FILE__,__LINE__); return arg2;}

#define AssertDRV(arg1, arg2) if(arg1 != DRV_OK) {DI_UART_Print(\
					"ERROR code : 0x%x, At %s() function, FILE: %s, LINE: %d\n",arg1, __func__, __FILE__,__LINE__); return arg2;}


#define HWTEST_CMD		szCmd
#define HWTEST_PARAM	szParam
#define HWTEST_PARAM1	szParam1
#define GET_ARGS		int iResult=CMD_ERR; \
						char *HWTEST_CMD=CMD_NextArg((char **)&szArgs); \
						char *HWTEST_PARAM=CMD_NextArg((char **)&szArgs); \
						char *HWTEST_PARAM1=CMD_NextArg((char **)&szArgs); \
						char *HWTEST_PARAM2=CMD_NextArg((char **)&szArgs);
#define CMD_IS(x)		(HWTEST_CMD!=NULL && VK_strcmp(HWTEST_CMD, x)==0)
#define PARAM_IS(x)		(HWTEST_PARAM!=NULL && VK_strcmp(HWTEST_PARAM, x)==0)
#define PARAM1_IS(x)	(HWTEST_PARAM1!=NULL && VK_strcmp(HWTEST_PARAM1, x)==0)
#define READABLE_IN_HEX(sz,val) CMD_ReadAsHex(sz, &val)
#define READABLE_IN_DEC(sz,val) CMD_ReadAsDec(sz, &val)
#define GET_NEXT_ARG(variable)		char *variable = CMD_NextArg((char **)&szArgs)

#define UNREFENCED_PARAM(x) (x=x)

#if defined(CONFIG_RF4CE_EXTERNAL)

extern DRV_Error DRV_RF4CE_TI_SBL_HandShake(void);
extern DRV_Error DRV_RF4CE_TI_SBL_Write(void);
extern DRV_Error DRV_RF4CE_TI_SBL_Read(void);
extern DRV_Error DRV_RF4CE_TI_SBL_Enable(void);
extern DRV_Error DRV_RF4CE_ClearPairingTable(void);

int CMD_HwTest_Parser_Rf4ce(void *szArgs)
{
	DRV_Error	nDrvErr = DRV_OK;
	int i=0;

	GET_ARGS;

	if CMD_IS("pair")
	{
		if PARAM_IS("on")
		{
			DI_RF4CE_SetPairing(TRUE, 0);
			CMD_Printf("SetPairing On\n");
		}
		else if PARAM_IS("off")
		{
			DI_RF4CE_SetPairing(FALSE, 0);
			CMD_Printf("SetPairing Off\n");
		}
		else if PARAM_IS("clear")
		{
			nDrvErr = DRV_RF4CE_ClearPairingTable();
			if(nDrvErr != DRV_OK)
			{
				CMD_Printf("Clear Pairing Table Error\n");
			}
			else
			{
				CMD_Printf("Clear Pairing Table Done\n");
			}
		}
		else
		{
			CMD_Printf("Enter On/Off/Clear\n");
		}
	}
	else if CMD_IS("reset")
	{
		nDrvErr = DRV_RF4CE_TI_Reset();
		CMD_Printf("RF4CE Reset!\n");
	}
	else if CMD_IS("init")
	{
		nDrvErr = DRV_RF4CE_Init();
		CMD_Printf("RF4CE Init!\n");
	}
	else if CMD_IS("initreq")
	{
		nDrvErr = DRV_RF4CE_REQ_InitReq();
		if(nDrvErr != DRV_OK)
		{
			CMD_Printf("DRV_RF4CE_TI_REQ_InitReq() Error\n");
		}
		CMD_Printf("RF4CE InitReq Done!\n");
	}
	else if CMD_IS("version")
	{
		HUINT32 ulVersion = 0;
		nDrvErr = DRV_RF4CE_GetSWVersion(&ulVersion);
		if(nDrvErr != DI_ERR_OK)
		{
			CMD_Printf("RF4CE GetSWVersion ERROR!\n");
		}
	}
	else if CMD_IS("sbl")
	{
		if PARAM_IS("enter")
		{
			nDrvErr = DRV_RF4CE_REQ_EnterSBLReq();
			if(nDrvErr != DI_ERR_OK)
			{
				CMD_Printf("RF4CE EnterSBLReq ERROR!\n");
			}
			CMD_Printf("Enter the SBL\n");
		}
		else if PARAM_IS("handshake")
		{
			nDrvErr = DRV_RF4CE_TI_SBL_HandShake();
			if(nDrvErr != DI_ERR_OK)
			{
				CMD_Printf("RF4CE SBL Handshake ERROR!\n");
			}
			CMD_Printf("SBL Handshake Done\n");
		}
		else if PARAM_IS("write")
		{
			nDrvErr = DRV_RF4CE_TI_SBL_Write();
			if(nDrvErr != DI_ERR_OK)
			{
				CMD_Printf("RF4CE SBL Write ERROR!\n");
			}
			CMD_Printf("SBL Write Done\n");
		}
		else if PARAM_IS("read")
		{
			nDrvErr = DRV_RF4CE_TI_SBL_Read();
			if(nDrvErr != DI_ERR_OK)
			{
				CMD_Printf("RF4CE SBL Read ERROR!\n");
			}
			CMD_Printf("SBL Read Done\n");
		}
		else if PARAM_IS("enable")
		{
			nDrvErr = DRV_RF4CE_TI_SBL_Enable();
			if(nDrvErr != DI_ERR_OK)
			{
				CMD_Printf("RF4CE SBL Enable ERROR!\n");
			}
			CMD_Printf("SBL Enable Done\n");
		}
		else
		{
			CMD_Printf("Enter More Word\n");
		}
	}
#if 0 /* OLD IMPLEMENTATION */
	else if CMD_IS("tx")
	{
		if PARAM_IS("unmod")
		{
			nDrvErr = DRV_RF4CE_CE_SetTxModeReq(RF4CE_TX_MODE_UNMODULATED);
			if(nDrvErr != DI_ERR_OK)
			{
				CMD_Printf("Set TX(Unmodulated Wave) ERROR!\n");
			}
			CMD_Printf("Set TX(Unmodulated Wave) Done\n");
		}
		else if PARAM_IS("mod")
		{
			nDrvErr = DRV_RF4CE_CE_SetTxModeReq(RF4CE_TX_MODE_MODULATED);
			if(nDrvErr != DI_ERR_OK)
			{
				CMD_Printf("Set TX(Modulated) ERROR!\n");
			}
			CMD_Printf("Set TX(Modulated) Done\n");
		}
		else
		{
			CMD_Printf("Enter Tx Mode\n");
		}
	}
	else if CMD_IS("rx")
	{
		nDrvErr = DRV_RF4CE_CE_SetRxModeReq(RF4CE_RX_MODE_UNMODULATED);
		if(nDrvErr != DI_ERR_OK)
		{
			CMD_Printf("Set RX Mode ERROR!\n");
		}
		CMD_Printf("Set RX Mode Done\n");
	}
#endif /* OLD IMPLEMENTATION */
	else if CMD_IS("status")
	{
		nDrvErr = DRV_RF4CE_CE_GetContextReq();
		if(nDrvErr != DRV_OK)
		{
			CMD_Printf("Get Context ERROR!\n");
		}
		CMD_Printf("Get Context Done\n");
	}
	else if CMD_IS("rssi")
	{
		nDrvErr = DRV_RF4CE_CE_GetRssiReq();
		if(nDrvErr != DRV_OK)
		{
			CMD_Printf("Get Context ERROR!\n");
		}
		CMD_Printf("Get Context Done\n");
	}
	else if CMD_IS("test")
	{
		nDrvErr = DRV_RF4CE_TI_REQ_TestModeReq();
		if(nDrvErr != DRV_OK)
		{
			CMD_Printf("Test Mode Req ERROR!\n");
		}
		CMD_Printf("Test Mode Req Done\n");
	}
	else if CMD_IS("setmode")
	{
		HUINT32 ulMode = 0;
		if (READABLE_IN_DEC(HWTEST_PARAM, ulMode))
		{
			CMD_Printf("Set Mode %d\n", ulMode);
			nDrvErr = DRV_RF4CE_TI_SetMode(ulMode);
			if(nDrvErr != DI_ERR_OK)
			{
				CMD_Printf("Set Mode %d ERROR!\n", ulMode);
			}
			CMD_Printf("Set Mode %d Done\n", ulMode);
		}
		else
		{
			CMD_Printf("Enter Mode\n");
		}
	}
	else if CMD_IS("setch")
	{
		HUINT32 ulChannel = 0;
		if (READABLE_IN_DEC(HWTEST_PARAM, ulChannel))
		{
			CMD_Printf("Set Channel %d\n", ulChannel);
			nDrvErr = DRV_RF4CE_TI_SetChannel(ulChannel);
			if(nDrvErr != DI_ERR_OK)
			{
				CMD_Printf("Set Channel %d ERROR!\n", ulChannel);
			}
			CMD_Printf("Set Channel %d Done\n", ulChannel);
		}
		else
		{
			CMD_Printf("Enter Channel\n");
		}
	}
	else if CMD_IS("setpw")
	{
		HUINT32 ulTxPower = 0;
		if (READABLE_IN_DEC(HWTEST_PARAM, ulTxPower))
		{
			CMD_Printf("Set Tx Power %d\n", ulTxPower);
			nDrvErr = DRV_RF4CE_TI_SetTxPower(ulTxPower);
			if(nDrvErr != DI_ERR_OK)
			{
				CMD_Printf("Set TxPower %d ERROR!\n", ulTxPower);
			}
			CMD_Printf("Set Channel %d Done\n", ulTxPower);
		}
		else
		{
			CMD_Printf("Enter Channel\n");
		}
	}
#if 0 /* OLD IMPLEMENTATION */
	else if CMD_IS("ch")
	{
		HUINT32 ulChannel = 0;
		if (READABLE_IN_DEC(HWTEST_PARAM, ulChannel))
		{
			CMD_Printf("Set Channel %d\n", ulChannel);
			nDrvErr = DRV_RF4CE_CE_SetChannelReq(ulChannel);
			if(nDrvErr != DI_ERR_OK)
			{
				CMD_Printf("Set Channel %d ERROR!\n", ulChannel);
			}
			CMD_Printf("Set Channel %d Done\n", ulChannel);
		}
		else
		{
			CMD_Printf("Enter Channel\n");
		}
	}
	else if CMD_IS("pw")
	{
#if 0
		if PARAM_IS("1")
		{
			nDrvErr = DRV_RF4CE_CE_SetTxPowerReq(RF4CE_TX_POWER_MINUS_4P1);
			if(nDrvErr != DI_ERR_OK)
			{
				CMD_Printf("Set TX Power -4.1dBm ERROR!\n");
			}
			CMD_Printf("Set TX Power -4.1dBm Done\n");
		}
		else if PARAM_IS("2")
		{
			nDrvErr = DRV_RF4CE_CE_SetTxPowerReq(RF4CE_TX_POWER_0P3);
			if(nDrvErr != DI_ERR_OK)
			{
				CMD_Printf("Set TX Power 0.3dBm ERROR!\n");
			}
			CMD_Printf("Set TX Power 0.3dBm Done\n");
		}
		else if PARAM_IS("3")
		{
			nDrvErr = DRV_RF4CE_CE_SetTxPowerReq(RF4CE_TX_POWER_4P5);
			if(nDrvErr != DI_ERR_OK)
			{
				CMD_Printf("Set TX Power 4.5dBm ERROR!\n");
			}
			CMD_Printf("Set TX Power 4.5dBm Done\n");
		}
		else if PARAM_IS("4")
		{
			nDrvErr = DRV_RF4CE_CE_SetTxPowerReq(RF4CE_TX_POWER_7P0);
			if(nDrvErr != DI_ERR_OK)
			{
				CMD_Printf("Set TX Power 7dBm ERROR!\n");
			}
			CMD_Printf("Set TX Power 7dBm Done\n");
		}
#endif
		HUINT32 ulTxPower = 0;
		if (READABLE_IN_DEC(HWTEST_PARAM, ulTxPower))
		{
			CMD_Printf("Tx Power : %d\n", ulTxPower);
			nDrvErr = DRV_RF4CE_CE_SetTxPowerReq(ulTxPower);
			if(nDrvErr != DI_ERR_OK)
			{
				CMD_Printf("Set TX Power %d ERROR!\n", ulTxPower);
			}
			CMD_Printf("Set TX Power %d Done\n", ulTxPower);
		}
		else
		{
			CMD_Printf("Enter TX Power\n");
		}
	}
#endif	/* OLD IMPLEMENTATION */
	else
	{
		HWTEST_PARAM = HWTEST_PARAM;
		HWTEST_PARAM1 = HWTEST_PARAM1;

		CMD_Printf("Command : rf4ce ?? -> pair/reset\n");
	}

	return CMD_OK;
}
#elif defined(CONFIG_RF4CE_MICOM)

#if defined(CONFIG_RF4CE_MOTION_TOUCH_UEI_HILCREST)
extern void DRV_HILCREST_EnableRelativeMotion (unsigned char ucEnable);
extern void DRV_HILCREST_EnableRawSensorData (unsigned char ucEnable);
extern void DRV_HILCREST_ChangeRawSensorDataSpeed (unsigned char ucMultiPlier);
extern unsigned char DRV_RF4CE_GetCurrentPairingTableIndex (void);
#endif/*CONFIG_RF4CE_MOTION_TOUCH_UEI_HILCREST*/

void CMD_HwTest_Parser_Rf4ceEventCallback(void *data)
{
	DI_RF4CE_CALLBACK_DATA_t *pstCallbackData;
	unsigned int i = 0;

	pstCallbackData = (DI_RF4CE_CALLBACK_DATA_t *)data;

	switch(pstCallbackData->eEvent)
	{
		case DI_RF4CE_NOTIFY_EVT_PAIRING_SUCCESS:
			CMD_Printf("CMD_HwTest_Parser_Rf4ceEventCallback] RF4CE Connect Ok!!!! \n");
			break;
		case DI_RF4CE_NOTIFY_EVT_PAIRING_FAILED:
			CMD_Printf("CMD_HwTest_Parser_Rf4ceEventCallback] RF4CE Connect Fail!!!! \n");
			break;
		default:
			break;
	}

}

int CMD_HwTest_Parser_Rf4ce(void *szArgs)
{
	unsigned char ucCurrnetPairingID =0;
	DI_ERR_CODE	nErr = DI_ERR_OK;
	int i=0;
	HUINT8 aucOkCount[2]={0,}, aucErrCount[2]={0,};
	static unsigned char s_ucChannelNum=0;
	static unsigned char s_ucModeNum=0;
	static unsigned char s_ucRxMode=0;
	static unsigned char s_ucTxMode=0;

	GET_ARGS;

	DI_RF4CE_RegisterEventCallback(DI_RF4CE_NOTIFY_EVT_PAIRING_SUCCESS,CMD_HwTest_Parser_Rf4ceEventCallback);
	DI_RF4CE_RegisterEventCallback(DI_RF4CE_NOTIFY_EVT_PAIRING_FAILED,CMD_HwTest_Parser_Rf4ceEventCallback);

	if CMD_IS("init")
	{

	}
	else if CMD_IS("reset")
	{
	}
	else if CMD_IS("ping")
	{
	}
	else if CMD_IS("set_pair")
	{
		nErr = DI_RF4CE_SetPairing(TRUE,30);
		if(nErr)
		{
			CMD_Printf("[CMD_HwTest_Parser_Rf4ce] ERROR: DI_RF4CE_SetPairing TRUE \r\n");
			return CMD_ERR;
		}
	}
	else if CMD_IS("get_pair")
	{
		HUINT32 ulCnt = 0;

		nErr = DI_RF4CE_GetPairedDevCount(&ulCnt);
		if(nErr)
		{
			CMD_Printf("[CMD_HwTest_Parser_Rf4ce] ERROR: DI_RF4CE_GetPairedDevCount FALSE\r\n");
			return CMD_ERR;
		}
		CMD_Printf("########### Paired Device Count : %d ###########\n",ulCnt);
	}
	else if CMD_IS("pair_abort")
	{
		nErr = DI_RF4CE_SetPairing(FALSE,0);
		if(nErr)
		{
			CMD_Printf("[CMD_HwTest_Parser_Rf4ce] ERROR: DI_RF4CE_SetPairing FALSE\r\n");
			return CMD_ERR;
		}
	}
	else if CMD_IS("get_ieee")
	{
		HUINT32 ulCnt = 0;
		HINT32 nIndex = 0;
		HUINT8 aucIEEE[8];
		nErr = DI_RF4CE_GetPairedDevCount(&ulCnt);
		if(nErr)
		{
			CMD_Printf("[CMD_HwTest_Parser_Rf4ce] ERROR: DI_RF4CE_GetPairedDevCount FALSE\r\n");
			return CMD_ERR;
		}
		CMD_Printf("########### Paired Device Count : %d ###########\n",ulCnt);
		for(nIndex == 0; nIndex < ulCnt ; nIndex++)
		{
			DI_RF4CE_GetIEEEAddr(nIndex,aucIEEE);
			CMD_Printf("\t [%d] : %02x:%02x:%02x:%02x:%02x:%02x:%02x:%02x \n",nIndex
				,aucIEEE[0],aucIEEE[1],aucIEEE[2],aucIEEE[3],aucIEEE[4],aucIEEE[5],aucIEEE[6],aucIEEE[7]);
		}
		DRV_MICOM_GetIEEEAddr(aucIEEE);
		CMD_Printf("\t [STB] : %02x:%02x:%02x:%02x:%02x:%02x:%02x:%02x \n",aucIEEE[0],aucIEEE[1],aucIEEE[2],aucIEEE[3],aucIEEE[4],aucIEEE[5],aucIEEE[6],aucIEEE[7]);

		CMD_Printf("################################################\n",ulCnt);
	}
	else if (CMD_IS("inout"))
	{
		if PARAM_IS("cwoutput")
		{
			DRV_MICOM_SetRf4ceCerti_InOutMode(0);
		}
		else if PARAM_IS("cmoutput")
		{
			DRV_MICOM_SetRf4ceCerti_InOutMode(1);
		}
		else if PARAM_IS("input")
		{
			DRV_MICOM_SetRf4ceCerti_InOutMode(2);
		}
		else
		{
			CMD_Printf("Wrong Command~! right command : rf4ce inout\r\n");
			return CMD_ERR;
		}
	}
	else if (CMD_IS("setchan"))
	{
		int nChannelNum=0;

		READABLE_IN_DEC(HWTEST_PARAM, nChannelNum);
		if (DRV_MICOM_SetRf4ceCerti_ChangeFreq((unsigned char)nChannelNum))
		{
			CMD_Printf("Wrong Command~! right command : rf4ce setchan\r\n");
			return CMD_ERR;
		}
	}
	else if (CMD_IS("cw"))
	{
		unsigned char ucModeDisable = 0;
		
		if(s_ucTxMode == TRUE)
		{
			s_ucTxMode = FALSE;
			DRV_MICOM_SetRf4ceCerti_Iqview_Tx(0);
			CMD_Printf("Disable Tx Done!!\n");
		}
		if(s_ucRxMode == TRUE)
		{
			s_ucRxMode = FALSE;
			int nChannelNum=0, nOkCount=0, nErrCount=0;
			nErr = DRV_MICOM_SetRf4ceCerti_Iqview_Rx(0);
			if(nErr != DI_ERR_OK)
			{
				CMD_Printf("Set Rx Mode (%d) ERROR!\n", nChannelNum);
				return CMD_ERR;
			}

			VK_TASK_Sleep(1000);
			
			nErr = DRV_MICOM_SetRf4ceCerti_Iqview_Rx_Result(aucOkCount, aucErrCount);
			if(nErr != DI_ERR_OK)
			{
				CMD_Printf("Get Rx PER ERROR!\n", nChannelNum);
				return CMD_ERR;
			}
			
			nOkCount = aucOkCount[0];
			nOkCount <<= 8;
			nOkCount = nOkCount + aucOkCount[1];

			nErrCount = aucErrCount[0];
			nErrCount <<= 8;
			nErrCount = nErrCount + aucErrCount[1];
				
			CMD_Printf("Disable Rx Mode (%d)!!\n", nChannelNum);
			CMD_Printf(" RxCount CRC OK = (%d)!!\n", nOkCount);
			CMD_Printf(" RxCount CRC FAIL = (%d)!!\n", nErrCount);
			CMD_Printf("Disable Rx Done!!\n");
		}
		
		if PARAM_IS("u")
		{
			if(READABLE_IN_DEC(HWTEST_PARAM1, ucModeDisable))
			{
				if (ucModeDisable == 0)
				{
					if(s_ucModeNum == 1)
					{
						DRV_MICOM_SetRf4ceCerti_InOutMode(0);
						CMD_Printf("Disable CW Wait!!\n");
					}
					
					s_ucModeNum = 2;
					DRV_MICOM_SetRf4ceCerti_InOutMode(s_ucModeNum);
					CMD_Printf("Disable CW Done!!\n");
					return CMD_OK;
				}
			}

			s_ucModeNum=0;
			DRV_MICOM_SetRf4ceCerti_InOutMode(s_ucModeNum);
			CMD_Printf("Set CW Done!!\n");
		}
		else if PARAM_IS("m")
		{
			if(READABLE_IN_DEC(HWTEST_PARAM1, ucModeDisable))
			{
				if (ucModeDisable == 0)
				{
					if(s_ucModeNum == 1)
					{
						DRV_MICOM_SetRf4ceCerti_InOutMode(0);
						CMD_Printf("Disable CMWait!!\n");
					}
					
					s_ucModeNum = 2;
					DRV_MICOM_SetRf4ceCerti_InOutMode(s_ucModeNum);
					CMD_Printf("Disable CM Done!!\n");
					return CMD_OK;
				}
			}
			s_ucModeNum = 1;
			DRV_MICOM_SetRf4ceCerti_InOutMode(s_ucModeNum);
			CMD_Printf("Set CM Done!!\n");
		}
		else
		{
			CMD_Printf("Wrong Command~! right command : rf4ce cw [m/u]\r\n");
			return CMD_ERR;
		}
	}
	else if (CMD_IS("ch")) //ch = 12~25
	{
		int nChannelNum = 0 ;

		if (READABLE_IN_DEC(HWTEST_PARAM, nChannelNum))
		{
			s_ucChannelNum = nChannelNum;
			
			if (DRV_MICOM_SetRf4ceCerti_ChangeFreq((unsigned char)s_ucChannelNum))
			{
				CMD_Printf("Wrong Command (%d)~! right command : rf4ce ch [Channel Num]\r\n", s_ucChannelNum);
				s_ucChannelNum = 0;
				return CMD_ERR;
			}
			CMD_Printf("Set Tx Channel number (%d) Done!!\n", s_ucChannelNum);
		}
		else
		{
			CMD_Printf("Wrong Command ~! right command : rf4ce ch [Channel Num]\r\n");
		}
	}
	else if CMD_IS("w")
	{
		int nPowerLevel = 0;
		
		if (READABLE_IN_DEC(HWTEST_PARAM, nPowerLevel))
		{
			nErr = DRV_MICOM_SetRf4ceCerti_ChangePower(nPowerLevel);
			if(nErr != DI_ERR_OK)
			{
				CMD_Printf("Set Power level (%d) ERROR!\n", nPowerLevel);
				return CMD_ERR;
			}
			CMD_Printf("Set Power level (%d) Done!!\n", nPowerLevel);
		}
		else
		{
			CMD_Printf("Wrong Command ~! right command : rf4ce pw [Power Level]\r\n");
		}
	}
	else if CMD_IS("rx") 
	{
		unsigned char ucRxEnable=0;
		int nChannelNum=0, nOkCount=0, nErrCount=0;

		if (s_ucModeNum != 2)	// Function Check & Disable
		{
			if(s_ucModeNum == 1)
			{
				DRV_MICOM_SetRf4ceCerti_InOutMode(0);
				CMD_Printf("Disable CW/CM Wait!!\n");
			}

			s_ucModeNum = 2;
			DRV_MICOM_SetRf4ceCerti_InOutMode(s_ucModeNum);
			CMD_Printf("Disable CW/CM Done!!\n");
		}
		if(s_ucTxMode == TRUE)
		{
			s_ucTxMode = FALSE;
			DRV_MICOM_SetRf4ceCerti_Iqview_Tx(0);
			CMD_Printf("Disable Tx Done!!\n");
		}
		if(s_ucChannelNum==0)
		{
			CMD_Printf("Please Set Rx Channel Number!!\n");
			return CMD_ERR;
		}

		nChannelNum = s_ucChannelNum;
		s_ucRxMode = TRUE;
		
		nErr = DRV_MICOM_SetRf4ceCerti_Iqview_Rx(nChannelNum);
		if(nErr != DI_ERR_OK)
		{
			CMD_Printf("Set Rx Mode (%d) ERROR!\n", nChannelNum);
			return CMD_ERR;
		}

		CMD_Printf("Set Rx (%d) Done!!\n", nChannelNum);			
	}
	else if CMD_IS("p") // Check Rx PER Result
	{
		int nChannelNum=0, nOkCount=0, nErrCount=0;

		if (s_ucModeNum != 2)	// Function Check & Disable
		{
			if(s_ucModeNum == 1)
			{
				DRV_MICOM_SetRf4ceCerti_InOutMode(0);
				CMD_Printf("Disable CW/CM Wait!!\n");
			}

			s_ucModeNum = 2;
			DRV_MICOM_SetRf4ceCerti_InOutMode(s_ucModeNum);
			CMD_Printf("Disable CW/CM Done!!\n");
		}
		if(s_ucTxMode == TRUE)
		{
			s_ucTxMode = FALSE;
			DRV_MICOM_SetRf4ceCerti_Iqview_Tx(0);
			CMD_Printf("Disable Tx Done!!\n");
		}

		s_ucRxMode = FALSE;
		
		nErr = DRV_MICOM_SetRf4ceCerti_Iqview_Rx(0);
		if(nErr != DI_ERR_OK)
		{
			CMD_Printf("Set Rx Mode (%d) ERROR!\n", nChannelNum);
			return CMD_ERR;
		}

		VK_TASK_Sleep(1000);
		
		nErr = DRV_MICOM_SetRf4ceCerti_Iqview_Rx_Result(aucOkCount, aucErrCount);
		if(nErr != DI_ERR_OK)
		{
			CMD_Printf("Get Rx PER ERROR!\n", nChannelNum);
			return CMD_ERR;
		}

		nOkCount = aucOkCount[0];
		nOkCount <<= 8;
		nOkCount = nOkCount + aucOkCount[1];

		nErrCount = aucErrCount[0];
		nErrCount <<= 8;
		nErrCount = nErrCount + aucErrCount[1];
			
		CMD_Printf("Disable Rx Mode (%d)!!\n", nChannelNum);
		CMD_Printf(" RxCount CRC OK = (%d)!!\n", nOkCount);
		CMD_Printf(" RxCount CRC FAIL = (%d)!!\n", nErrCount);			
	}
	else if CMD_IS("tx") // 0 = Tx disable, 1 = Tx enable
	{
		unsigned char ucTxEnable=0;
		int nChannelNum=0;

		if (s_ucModeNum != 2) // Function Check & Disable
		{
			if(s_ucModeNum == 1)
			{
				DRV_MICOM_SetRf4ceCerti_InOutMode(0);
				CMD_Printf("Disable CW/CM Wait!!\n");
			}

			s_ucModeNum = 2;
			DRV_MICOM_SetRf4ceCerti_InOutMode(s_ucModeNum);
			CMD_Printf("Disable CW/CM Done!!\n");
		}
		if(s_ucRxMode == TRUE)
		{
			s_ucRxMode = FALSE;
			int nChannelNum=0, nOkCount=0, nErrCount=0;
			nErr = DRV_MICOM_SetRf4ceCerti_Iqview_Rx(0);
			if(nErr != DI_ERR_OK)
			{
				CMD_Printf("Set Rx Mode (%d) ERROR!\n", nChannelNum);
				return CMD_ERR;
			}

			VK_TASK_Sleep(1000);
			
			nErr = DRV_MICOM_SetRf4ceCerti_Iqview_Rx_Result(aucOkCount, aucErrCount);
			if(nErr != DI_ERR_OK)
			{
				CMD_Printf("Get Rx PER ERROR!\n", nChannelNum);
				return CMD_ERR;
			}
			
			nOkCount = aucOkCount[0];
			nOkCount <<= 8;
			nOkCount = nOkCount + aucOkCount[1];

			nErrCount = aucErrCount[0];
			nErrCount <<= 8;
			nErrCount = nErrCount + aucErrCount[1];
				
			CMD_Printf("Disable Rx Mode (%d)!!\n", nChannelNum);
			CMD_Printf(" RxCount CRC OK = (%d)!!\n", nOkCount);
			CMD_Printf(" RxCount CRC FAIL = (%d)!!\n", nErrCount);
			CMD_Printf("Disable Rx Done!!\n");
		}

		if(READABLE_IN_DEC(HWTEST_PARAM, ucTxEnable))
		{
			if (ucTxEnable == 0)
			{
				nChannelNum = 0;
				s_ucTxMode = FALSE;
			}
			else
			{
				nChannelNum = s_ucChannelNum;
				s_ucTxMode = TRUE;
			}
		}
		else
		{
			nChannelNum = s_ucChannelNum;
			s_ucTxMode = TRUE;
		}
		
		nErr = DRV_MICOM_SetRf4ceCerti_Iqview_Tx(nChannelNum);
		if(nErr != DI_ERR_OK)
		{
			CMD_Printf("Set Tx Mode (%d) ERROR!\n", nChannelNum);
			return CMD_ERR;
		}

		if(nChannelNum == 0)
		{
			CMD_Printf("Disable Tx Mode (%d) \n", nChannelNum);			
		}
		else
		{
			CMD_Printf("Set Tx (%d) Done!!\n", nChannelNum);			
		}
	}	
#if defined(CONFIG_RF4CE_MOTION_TOUCH_UEI_HILCREST)
	else if (CMD_IS("battery"))
	{
		if PARAM_IS("status")
		{
	       	ucCurrnetPairingID = DRV_RF4CE_GetCurrentPairingTableIndex();
	       	UAPIBatteryStatusRequest(UAPI_RF4CE_NIF, ucCurrnetPairingID);
		}
		else if PARAM_IS("param")
	       {
	       	ucCurrnetPairingID = DRV_RF4CE_GetCurrentPairingTableIndex();
			UAPIGetInfo(UAPIFeature_BatteryStatus, UAPI_RF4CE_NIF, ucCurrnetPairingID);
		}
		else
		{
			CMD_Printf("Wrong Command~! right command : rf4ce battery\r\n");
			return CMD_ERR;
		}
	}
	else if (CMD_IS("feature"))
	{
		ucCurrnetPairingID = DRV_RF4CE_GetCurrentPairingTableIndex();
		UAPIGetRemoteFeatures(UAPI_RF4CE_NIF, ucCurrnetPairingID);
	}
	else if (CMD_IS("remote"))
	{
		ucCurrnetPairingID = DRV_RF4CE_GetCurrentPairingTableIndex();
		UAPIGetInfo(UAPIFeature_Core, UAPI_RF4CE_NIF, ucCurrnetPairingID);
	}
	else if (CMD_IS("txdata"))
	{

		unsigned char ucZrcUserData[251]={'a', 'b', 'c', 'd', 'e', 'f', 'g', 'h', 'i', 'j', 'k', 'l', 'm', 'n', 'o', 'p', 'q', 'r', 's', 't', 'u', 'v', 'w', 'x', 'y', 'z', '1', '2', '3', '4', '5', '6', '7', '8', '9','0',
		                                                          'A', 'B', 'C', 'D', 'E', 'F', 'G', 'H', 'I', 'J', 'K', 'L', 'M', 'N', 'O', 'P', 'Q', 'R', 'S', 'T', 'U', 'V', 'W', 'X', 'Y','Z'};
		HUINT32 ulTxDataSize;
		ucCurrnetPairingID = DRV_RF4CE_GetCurrentPairingTableIndex();

		if ( READABLE_IN_DEC(HWTEST_PARAM, ulTxDataSize) )
		{
			DRV_MICOM_TransmitZrcUserData(ucCurrnetPairingID, &ucZrcUserData[0], ulTxDataSize);
		}
	}
#if (SUPPORT_HILCREST_ALLFUNCS)
	else if (CMD_IS("relmotion"))
	{
		if PARAM_IS("enable")
		{
			DRV_HILCREST_EnableRelativeMotion(TRUE);
		}
		else if PARAM_IS("disable")
		{
			DRV_HILCREST_EnableRelativeMotion(FALSE);
		}
		else
		{
			CMD_Printf("Wrong Command~! right command : rf4ce relmotion\r\n");
			return CMD_ERR;
		}
	}
	else if (CMD_IS("rawsensor"))
	{
		if PARAM_IS("enable")
		{
			DRV_HILCREST_EnableRawSensorData(TRUE);
		}
		else if PARAM_IS("disable")
		{
			DRV_HILCREST_EnableRawSensorData(FALSE);
		}
		else if PARAM_IS("speed")
		{
			HUINT32 ulMultiPlier;

			if (READABLE_IN_DEC(HWTEST_PARAM1, ulMultiPlier))
			{
				DRV_HILCREST_ChangeRawSensorDataSpeed((unsigned char)ulMultiPlier);
			}
		}
		else
		{
			CMD_Printf("Wrong Command~! right command : rf4ce rawsensor\r\n");
			return CMD_ERR;
		}
	}
#endif
#endif
	else
	{
		CMD_Printf("Wrong rf4ce test command!\r\n");
		return CMD_ERR;
	}

	return CMD_OK;
}
#endif/*CONFIG_RF4CE_MICOM*/

#undef HWTEST_CMD
#undef HWTEST_PARAM
#undef HWTEST_PARAM1
#undef GET_ARGS
#undef CMD_IS
#undef PARAM_IS
#undef PARAM1_IS
#undef READABLE_IN_HEX
#undef READABLE_IN_DEC
#undef GET_NEXT_ARG


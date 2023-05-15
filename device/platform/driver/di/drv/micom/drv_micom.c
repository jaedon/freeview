/**
 * drv_micom.c
 */

/**
 * @defgroup		DRV_MICOM DRV_KPD : DRV_MICOM apis
 * @author			Kyu-young Cho
 * @note			DRV_MICOM APIs
 * @brief			Define DRV_MICOM APIs
 * @file 			drv_micom.c
 */


/*******************************************************************/
/**************************** Header Files *************************/
/*******************************************************************/
/* include  common utils headers */
#include "linden_trace.h"

#if defined(CONFIG_VK_STDLIB)
#else
#include 	<stdio.h>
#include 	<stdlib.h>
#include 	<string.h>
#endif
#include <unistd.h>
#include <termios.h>
#include <fcntl.h>
#include <sys/times.h>
#include <sys/time.h>
#include <time.h>
#include "bstd.h"

#include "vkernel.h"
#include "taskdef.h"
#include "di_uart.h"
#include "drv_key.h"
#include "drv_micom.h"

#if defined(CONFIG_OS_UCOS)
#include <humax_bsu.h>
#endif

/*******************************************************************/
/****************************** define *****************************/
/*******************************************************************/
#ifdef CONFIG_MICOM_ALIVE_TASK_ENABLE
#include "drv_gpio.h"
#endif

#define DEBUG_MODULE	TRACE_MODULE_DRV_MICOM

/*******************************************************************/
/****************************** typedef ****************************/
/*******************************************************************/

/*******************************************************************/
/************************ constant & gloval variables *************************/
/*******************************************************************/
#if defined(CONFIG_PANEL_7SEG)
static unsigned char	s_ucaPanelCharMap[0x80] =
{
	0x0,  0x0,  0x0,  0x0,  0x0,  0x0,  0x0,  0x0,		// 0x00 - 0x07
		0x0,  0x0,  0x0,  0x0,  0x0,  0x0,  0x0,  0x0,		// 0x08 - 0x0F
		0x0,  0x0,  0x0,  0x0,  0x0,  0x0,  0x0,  0x0,		// 0x10 - 0x17
		0x0,  0x0,  0x0,  0x0,  0x0,  0x0,  0x0,  0x0,		// 0x18 - 0x1F
		0x0,  0x0,  0x0,  0xff,  0x0,  0x0,  0x0,  0x0,		// 0x20 - 0x27
		0x0,  0x0,  0x0,  0x0,  0x0,  0x40, 0x80, 0x0,		// 0x28 - 0x2F
		0x3f, 0x06, 0x5b, 0x4f, 0x66, 0x6d, 0x7d, 0x27,		// 0x30 - 0x37
		0x7f, 0x6f, 0x0,  0x0,  0x0,  0x0,  0x0,  0x0,		// 0x38 - 0x3F
		0x0,  0x77, 0x7c, 0x39, 0x5e, 0x79, 0x71, 0x3d,		// 0x40 - 0x47
		0x76, 0x06, 0x0e, 0x74, 0x38, 0x49, 0x54, 0x3f,		// 0x48 - 0x4F
		0x73, 0x0,  0x50, 0x6d, 0x78, 0x3e, 0x3e, 0x49,		// 0x50 - 0x57
		0x76,  0x6e, 0x0,  0x0,  0x0,  0x0,  0x0,  0x0,		// 0x58 - 0x5F
		0x0,  0x77, 0x7c, 0x39, 0x5e, 0x79, 0x71, 0x3d,		// 0x60 - 0x67
		0x76, 0x06, 0x0e, 0x74, 0x38, 0x49, 0x54, 0x3f,		// 0x68 - 0x6F
		0x73, 0x0,  0x50, 0x6d, 0x78, 0x3e, 0x3e, 0x49,		// 0x70 - 0x77
		0x76,  0x66, 0x0,  0x0,  0x0,  0x0,  0x0,  0xff,		// 0x78 - 0x7F
};
#endif

unsigned int ulMicomVersion = 0;

#if defined(CONFIG_MICOM_UPGRADE)
unsigned long s_ulMicomTxSema;
unsigned long s_ulMicomRxSema;
#endif

#include "drv_rcu_type.h"
extern const RCU_INFO s_asKeyCodeTable[IR_NUMBER_OF_KEY];

/*******************************************************************/
/************************ static variables *************************/
/*******************************************************************/
#if defined(CONFIG_OS_UCOS)
#else
static int fd_rd, fd_wr;
#endif
static unsigned long MessageId;
static unsigned long s_ulMicomTxRxSema;

#if defined(CONFIG_MICOM_ALIVE_TASK_ENABLE)
static unsigned long s_ulMicomAliveTaskID = NULL;
static HBOOL s_bIsMicomAliveTaskEnabled = TRUE;
#endif

#if defined(CONFIG_OS_UCOS)
static HINT32 s_isRunMicomTask = FALSE;
static unsigned long s_ulUcosMicomKeyTaskID = 0;
#endif	/* CONFIG_OS_UCOS */

#define	GET_MICOM_SEMA 		VK_SEM_Get(s_ulMicomTxRxSema)
#define	REL_MICOM_SEMA 		VK_SEM_Release(s_ulMicomTxRxSema)

/*******************************************************************/
/*********************** Function Prototypes ***********************/
/*******************************************************************/
#if defined(CONFIG_OS_UCOS)
#else
static void DRV_MICOM_EventTask( void *p );
#endif

#if defined(CONFIG_MICOM_ALIVE_TASK_ENABLE)
static void DRV_MICOM_Alive_Task( void *p );
static DI_ERR_CODE DRV_MICOM_SetSysMonitoring(HBOOL bOn_nOff, unsigned char ucTimeOutSec);
static DRV_Error DRV_MICOM_SetAppReady(void);
#endif

static void MakeCheckSum(unsigned char *pCheckSum, int CheckNum);
static DRV_Error FrameCheckSum(unsigned char *pbuffer, int buffer_cnt);

#if defined(CONFIG_OS_UCOS)
static DRV_Error   DRV_uMICOM_FlushUart(void);
static void DRV_uMICOM_Key_Task( void *p );
static DRV_Error DRV_uMICOM_GetMessage(int argRxNum, unsigned char *pDataBuffer, unsigned long timeout);

#undef 	MICOM_KEY_TASK_PRIORITY
#define MICOM_KEY_TASK_PRIORITY 	51
#endif /* CONFIG_OS_UCOS */

/********************************************************************
  Function   : DRV_MICOM_Init
  Description :
  Input      : none
  Output     : none
  Return     : DRV_Error
 ********************************************************************/
DRV_Error DRV_MICOM_Init( void )
{
#if defined(CONFIG_OS_UCOS)
	HUINT32	result=DRV_ERR;
	result = DRV_uMICOM_FlushUart();
	if(result != VK_OK)
	{
		PrintError("DRV_uMICOM_FlushUart error\n");
		return DRV_ERR;
	}
	result = VK_SEM_Create(&s_ulMicomTxRxSema,"uMicom_rxtx", DEFAULT_SUSPEND_TYPE);
	if(result != VK_OK)
	{
		PrintError("uMicom_rxtx VK_SEM_Create Create Error\n");
		return DRV_ERR;
	}
	result = VK_MSG_Create(MICOM_MESSAGE_QUEUE_SIZE,MICOM_MESSAGE_BYTES, "Micom_Key_Msg", &MessageId, VK_SUSPENDTYPE_FIFO);
	if(result != VK_OK)
	{
		PrintError("[DRV_MICOM_Init] Micom_Key_Msg Create Error\n");
			return DRV_ERR;
	}
	s_isRunMicomTask = TRUE;
	result = VK_TASK_Create(DRV_uMICOM_Key_Task, MICOM_KEY_TASK_PRIORITY, MICOM_RX_TASK_STACK_SIZE, "uMICOM_KEY_Event_Task", (void *)0, &s_ulUcosMicomKeyTaskID, FALSE);
	if(result != VK_OK)
	{
		s_isRunMicomTask = FALSE;
		PrintError("[DRV_MICOM_Init] DRV_MICOM_Event_Task Create Error\n");
		return DRV_ERR;
	}
	VK_TASK_Start(s_ulUcosMicomKeyTaskID);

	return DRV_OK;
#else	/* CONFIG_OS_UCOS */

		struct termios tio;
		unsigned long TaskID;
		HUINT32 retval = DRV_OK;

		PrintDebug("\n DRV_MICOM_Init \n");

		/*	´Ù¸¥ µÎ task (RunAppl/MICOM_KEY_Event_Task) ¿¡¼­ °°Àº file pointer¸¦ »ç¿ëÇÏ¸é */
		/*	µ¿½Ã¿¡ accessÇÒ °æ¿ì tty driver ³» refcnt °ª¿¡ Áõ°¡.											 */
		/*	write°¡ Á¾·á µÇ±â Àü data°¡ µé¾î ¿À¸é read¿¡¼­ blockingÀ» Ç®Áö ¸øÇÔ.					*/
		/*	read/write ¸ðµå·Î µû·Î openÇÏ¿© file pointer¸¦ µû·Î °ü¸®											*/
		fd_rd = open(DRV_MICOM_DEVICE, O_RDONLY|O_NOCTTY|O_NONBLOCK);
		if(fd_rd < 0)
		{
			PrintError("[DRV_MICOM_Init] Read Open Error-(%d)\n", fd_rd);
				return DRV_ERR;
		}

		fd_wr = open(DRV_MICOM_DEVICE, O_WRONLY|O_NOCTTY);
		if(fd_wr < 0)
		{
			PrintError("[DRV_MICOM_Init] Write Open Error-(%d)\n", fd_wr);
				return DRV_ERR;
		}

		VK_memset(&tio, 0, sizeof(struct termios));

		/* Set termio */
		tio.c_cflag = B115200|CS8|CREAD|CLOCAL;
		tio.c_cflag &= ~HUPCL;
		tio.c_lflag = 0;
		tio.c_iflag = IGNPAR;
		tio.c_oflag = 0;

		tcsetattr(fd_rd,TCSANOW, &tio);
		tcflush(fd_rd,TCIFLUSH);
		tcsetattr(fd_wr,TCSANOW, &tio);
		tcflush(fd_wr,TCIFLUSH);

		retval = VK_SEM_Create(&s_ulMicomTxRxSema,"micom_rxtx", DEFAULT_SUSPEND_TYPE);
		if(retval != VK_OK)
		{
			PrintError("[DRV_MICOM_Init] VK_SEM_Create Create Error\n");
				return DRV_ERR;
		}

		retval = VK_MSG_Create(MICOM_MESSAGE_QUEUE_SIZE,MICOM_MESSAGE_BYTES, "Micom_Key_Msg", &MessageId, VK_SUSPENDTYPE_FIFO);
		if(retval != VK_OK)
		{
			PrintError("[DRV_MICOM_Init] Micom_Key_Msg Create Error\n");
				return DRV_ERR;
		}

		retval = VK_TASK_Create(DRV_MICOM_EventTask, MICOM_KEY_TASK_PRIORITY, MICOM_RX_TASK_STACK_SIZE, "MICOM_KEY_Event_Task", (void *)0, &TaskID, FALSE);
		if(retval != VK_OK)
		{
			PrintError("[DRV_MICOM_Init] DRV_MICOM_EventTask Create Error\n");
				return DRV_ERR;
		}

		VK_TASK_Start(TaskID);
#if defined(CONFIG_MICOM_ALIVE_TASK_ENABLE)
		retval = VK_TASK_Create(DRV_MICOM_Alive_Task, MICOM_KEY_TASK_PRIORITY, MICOM_RX_TASK_STACK_SIZE, "MICOM_ALIVE_TASK_ENABLE_Task", (void *)0, &s_ulMicomAliveTaskID, FALSE);
		if(retval != VK_OK)
		{
			PrintError("[DRV_MICOM_Init] DRV_MICOM_Alive_Task Create Error\n");
				return DRV_ERR;
		}
	VK_TASK_Start(s_ulMicomAliveTaskID);
#endif
		retval = DRV_MICOM_GetVersion(&ulMicomVersion);
		if (retval != DRV_OK)
		{
			PrintError("Error(0x%x) In DRV_MICOM_GetVersion() \n", retval);
		}

	DI_UART_Print("\033[01;34m\nMicomVersion : %x.%2x(major.minor)\n\n\033[00m", (ulMicomVersion&0xFF00)>>8, ulMicomVersion&0x00FF);

#if	defined(CONFIG_PANEL_7SEG)
		/* APPL ºÎÆÃ ½Ã (Áï, DI_INIT ½Ã) 7segment animation Ç×»ó ²û */
		retval = DRV_MICOM_SetDisplayAni(FALSE);
		if(retval != DRV_OK)
		{
			PrintError("DRV_MICOM_SetDisplayAni (FALSE) Error\n");
		}
#endif

		retval = DRV_MICOM_SetFastBoot(FALSE);
		if(retval != DRV_OK)
		{
			PrintError("DRV_MICOM_SetFastBoot (FALSE) Error\n");
		}

		/* rescue reset (3ÃÊ ÀÌ»ó standby button ´©¸¦ °æ¿ì reset) enable */
 		retval = DRV_MICOM_EnableRescueReset(TRUE);
		if(retval != DRV_OK)
		{
			PrintError("DRV_MICOM_EnableRescueReset (TRUE) Error\n");
		}

#if defined(CONFIG_MICOM_ALIVE_TASK_ENABLE)
		retval = DRV_MICOM_SetSysMonitoring(TRUE, MICOM_MONITOR_SEND_TIME*5);
		if(retval != DRV_OK)
		{
			PrintError("Error(0x%x) In DRV_MICOM_SetSysMonitoring() \n", retval);
		}

		retval = DRV_MICOM_SetAppReady();
		if(retval != DRV_OK)
		{
			PrintError("Error(0x%x) In DRV_MICOM_SetAppReady() \n", retval);
		}
#endif
		return DRV_OK;
#endif	/* CONFIG_OS_UCOS */
}

/*******************************************************************
  Function   : DRV_MICOM_TX
  Description :
  Input      : ucTxNum, pDataBuffer
  Output     : none
  Return     : DRV_Error
 ********************************************************************/
static DRV_Error DRV_MICOM_TX( int ucTxNum, unsigned char *pDataBuffer )
{
#if defined(CONFIG_OS_UCOS)
	while(ucTxNum--)
	{
		BSU_UART1_PutChar(*pDataBuffer++);
	}

	return DRV_OK;
#else	/* CONFIG_OS_UCOS */
	int retval;

		retval = write(fd_wr, pDataBuffer, ucTxNum);
		if(retval < 0) {
			PrintError("[DRV_MICOM_TX] TX Error : \n");

				return DRV_ERR;
		}
	return DRV_OK;
#endif	/* CONFIG_OS_UCOS */
}

/*******************************************************************
  Function   : DRV_MICOM_RX
  Description :
  Input	   : ucTxNum, pDataBuffer
  Output	   : none
  Return	   : DRV_Error
 ********************************************************************/
static DRV_Error DRV_MICOM_RX(int ucRxNum, unsigned char *pDataBuffer)
{
#if defined(CONFIG_OS_UCOS)
	while(ucRxNum--)
	{
		*pDataBuffer++ = BSU_UART1_GetChar();
	}
	return DRV_OK;
#else	/* CONFIG_OS_UCOS */
	int retval = -1;

		/*
		  file pointer¸¦ µû·Î °ü¸® ÇÏ´õ¶óµµ device°¡ °°Àº °æ¿ì private_data Æ÷ÀÎÅÍ°¡ °°À½.
		  ±×·¡¼­ file pointer¸¦ µû·Î ½áµµ read/write µ¿½Ã¿¡ access ÇÒ °æ¿ì
		  read°¡ blocking ÀÌ ¾È Ç®¸®´Â ¹®Á¦´Â ¿ÏÀüÈ÷ ÇØ°á µÇ´Â °ÍÀÌ ¾Æ´Ô.
		  ÇöÀç ±¸Á¶¿¡¼­´Â read ¸¦ O_NONBLOCKÀ¸·Î »ç¿ë. valid ÇÑ data°¡ ¿Ã °æ¿ì¿¡¸¸ return ÇÏ´Â °ÍÀÌ ÃÖ¼±.
		 */
		while(retval < 0){
			retval = read(fd_rd, pDataBuffer, ucRxNum);
				VK_TASK_Sleep(10);
		}

		return DRV_OK;
#endif	/* CONFIG_OS_UCOS */
}

#if defined(CONFIG_MICOM_UPGRADE)
DRV_Error DRV_MICOM_RX_BYTE(HUINT8 *pucData)
{
	return DRV_MICOM_RX(MICOM_FRAME_LENGTH_ONE, pucData);
}

DRV_Error DRV_MICOM_TX_BYTE(HUINT8 ucData)
{
	return DRV_MICOM_TX(MICOM_FRAME_LENGTH_ONE, &ucData);
}
#endif
#if defined(CONFIG_MICOM_ALIVE_TASK_ENABLE)
/********************************************************************
  Function	 : DRV_MICOM_SetSysMonitoring
  Description :
  Input 	 : none
  Output	 : none
  Return	 : DI_ERR_OK
 ********************************************************************/
static DI_ERR_CODE DRV_MICOM_SetSysMonitoring(HBOOL bOn_nOff, unsigned char ucTimeOutSec)
{
	unsigned char ucSysMonitoringData[DRV_MICOM_MESSAGE_BYTES];
		int result = DI_ERR_OK;

		PrintEnter();

		ucSysMonitoringData[MICOM_HEADER] = MICOM_FRAME_HEADER;
		ucSysMonitoringData[MICOM_LENGTH] = MICOM_FRAME_LENGTH_THREE;
		ucSysMonitoringData[MICOM_COMMAND] = MICOM_FRAME_CMD_SET_SYSTEM_MONITOR;
		ucSysMonitoringData[MICOM_DATA_ONE] = (unsigned char)bOn_nOff;
		ucSysMonitoringData[MICOM_DATA_TWO] = ucTimeOutSec;

		MakeCheckSum(ucSysMonitoringData,CHECK_NUM_FIVE);

		GET_MICOM_SEMA;
		result = DRV_MICOM_TX( MICOM_FRAME_LENGTH_SIX, ucSysMonitoringData );
		if( result != DI_ERR_OK )
		{
			PrintError( "[%s] DRV_MICOM_TX Error \n", __func__ );
				REL_MICOM_SEMA;
				return DRV_ERR;
		}

		result=VK_MSG_ReceiveTimeout(MessageId,ucSysMonitoringData,MICOM_FRAME_LENGTH_FOUR,(unsigned long)MICOM_TIMEOUT);
		REL_MICOM_SEMA;
		if(result == DI_ERR_OK) {
			if(ucSysMonitoringData[MICOM_COMMAND] == MICOM_FRAME_CMD_SET_SYSTEM_MONITOR)
				return DI_ERR_OK;
			else
				return DRV_ERR;
		} else {
			return DRV_ERR;
		}
}

/********************************************************************
  Function	: DRV_MICOM_SetAppReady
  Description :
  Input		: none
  Output 	: none
  Return 	: DRV_OK
 ********************************************************************/
static DRV_Error DRV_MICOM_SetAppReady(void)
{
	unsigned char ucAppReadyData[DRV_MICOM_MESSAGE_BYTES];
		int result=DI_ERR_OK;

		PrintEnter();

		ucAppReadyData[MICOM_HEADER] = MICOM_FRAME_HEADER;
		ucAppReadyData[MICOM_LENGTH] = MICOM_FRAME_LENGTH_ONE;
		ucAppReadyData[MICOM_COMMAND] = MICOM_FRAME_CMD_SET_APP_READY;
		MakeCheckSum(ucAppReadyData,CHECK_NUM_THREE);

		GET_MICOM_SEMA;
		result = DRV_MICOM_TX( MICOM_FRAME_LENGTH_FOUR, ucAppReadyData );
		if(result != DI_ERR_OK) {
			PrintError( "[DRV_MICOM_RequestLocalTime] MICOM_TX Error \n" );
				REL_MICOM_SEMA;
				return DRV_ERR;
		}

		result=VK_MSG_ReceiveTimeout(MessageId,ucAppReadyData,MICOM_FRAME_LENGTH_FOUR,(unsigned long)MICOM_TIMEOUT);
		REL_MICOM_SEMA;
		if(result == DI_ERR_OK)
		{
			if (ucAppReadyData[MICOM_COMMAND] == MICOM_FRAME_CMD_SET_APP_READY)
			{
				return DI_ERR_OK;
			}
			else
			{
				return DRV_ERR;
			}
		}
		else
		{
			return DRV_ERR;
		}
}
#endif

#if defined(CONFIG_RF4CE_MICOM)
/*******************************************************************
  Function	  : DRV_MICOM_SetRf4ceCerti_InOutMode
  Description :
  Input   : none
  Output  : *ulVersion
  Return  : DRV_Error
 ********************************************************************/
DRV_Error DRV_MICOM_SetRf4ceCerti_InOutMode (MICOM_RF4CE_INOUT_MODE eRF4CEInOutMode)
{

		unsigned char	ucSetRf4ceInOutSetting[DRV_MICOM_MESSAGE_BYTES];
		int 			result=DRV_OK;

		PrintEnter();

		if ((eRF4CEInOutMode != OUTPUT_MODE_WITH_CW) && (eRF4CEInOutMode != OUTPUT_MODE_WITH_CM) && (eRF4CEInOutMode != INPUT_MODE))
		{
			PrintError( "[DRV_MICOM_SetRf4ceCerti_InOutMode] Invalid RF4CE mode(%d) \n", eRF4CEInOutMode );
				return DRV_ERR;
		}

		ucSetRf4ceInOutSetting[MICOM_HEADER] = MICOM_FRAME_HEADER;
		ucSetRf4ceInOutSetting[MICOM_LENGTH] = MICOM_FRAME_LENGTH_TWO;
		ucSetRf4ceInOutSetting[MICOM_COMMAND] = MICOM_FRAME_CMD_SET_RF4CE_CERTI_INOUT_MODE;
		ucSetRf4ceInOutSetting[MICOM_DATA_ONE] = eRF4CEInOutMode;
		MakeCheckSum(ucSetRf4ceInOutSetting,CHECK_NUM_FOUR);

		GET_MICOM_SEMA;
		result = DRV_MICOM_TX( MICOM_FRAME_LENGTH_FIVE, ucSetRf4ceInOutSetting );
		if( result != DRV_OK )
		{
			PrintError( "[DRV_MICOM_SetRf4ceCerti_InOutMode] DRV_MICOM_TX Error \n" );
				REL_MICOM_SEMA;
				return DRV_ERR;
		}

		result=VK_MSG_ReceiveTimeout(MessageId,ucSetRf4ceInOutSetting,MICOM_FRAME_LENGTH_FOUR,(unsigned long)MICOM_TIMEOUT);
		REL_MICOM_SEMA;
		if(result == DRV_OK) {
			if(ucSetRf4ceInOutSetting[MICOM_COMMAND] == MICOM_FRAME_CMD_SET_RF4CE_CERTI_INOUT_MODE)
				return DRV_OK;
			else
				return DRV_ERR;
		} else {
			return DRV_ERR;
		}
}


/*******************************************************************
  Function	  : DRV_MICOM_SetRf4ceCerti_ChangeFreq
  Description : 2.405GHz(Channel 11, 0x0B) ~ 2.480GHz(Channel 26, 0x1A)
  Input   : none
  Output  : *ulVersion
  Return  : DRV_Error
 ********************************************************************/
DRV_Error DRV_MICOM_SetRf4ceCerti_ChangeFreq (unsigned char ucChannelNumber)
{

		unsigned char	ucSetRf4ceChannelSetting[DRV_MICOM_MESSAGE_BYTES];
		int 			result=DRV_OK;

		PrintEnter();

		if ((ucChannelNumber < 11) || (ucChannelNumber > 26))
		{
			PrintError( "[DRV_MICOM_SetRf4ceCerti_ChangeFreq] Invalid channel number(%d) \n", ucChannelNumber );
				return DRV_ERR;
		}

		ucSetRf4ceChannelSetting[MICOM_HEADER] = MICOM_FRAME_HEADER;
		ucSetRf4ceChannelSetting[MICOM_LENGTH] = MICOM_FRAME_LENGTH_TWO;
		ucSetRf4ceChannelSetting[MICOM_COMMAND] = MICOM_FRAME_CMD_SET_RF4CE_CERTI_CHANGE_FREQ;
		ucSetRf4ceChannelSetting[MICOM_DATA_ONE] = ucChannelNumber;
		MakeCheckSum(ucSetRf4ceChannelSetting,CHECK_NUM_FOUR);

		GET_MICOM_SEMA;
		result = DRV_MICOM_TX( MICOM_FRAME_LENGTH_FIVE, ucSetRf4ceChannelSetting );
		if( result != DRV_OK )
		{
			PrintError( "[DRV_MICOM_SetRf4ceCerti_ChangeFreq] DRV_MICOM_TX Error \n" );
				REL_MICOM_SEMA;
				return DRV_ERR;
		}

		result=VK_MSG_ReceiveTimeout(MessageId,ucSetRf4ceChannelSetting,MICOM_FRAME_LENGTH_FOUR,(unsigned long)MICOM_TIMEOUT);
		REL_MICOM_SEMA;
		if(result == DRV_OK) {
			if(ucSetRf4ceChannelSetting[MICOM_COMMAND] == MICOM_FRAME_CMD_SET_RF4CE_CERTI_CHANGE_FREQ)
				return DRV_OK;
			else
				return DRV_ERR;
		} else {
			return DRV_ERR;
		}
}
#endif

/*******************************************************************
  Function	  : DRV_MICOM_GetVersion
  Description :
  Input   : none
  Output  : *ulVersion
  Return  : DRV_Error
 ********************************************************************/
DRV_Error DRV_MICOM_GetVersion( unsigned int *pulVersion )
{
	unsigned char ucModelData[DRV_MICOM_MESSAGE_BYTES];
		int			result=DRV_OK;

		PrintEnter();

		ucModelData[MICOM_HEADER]=MICOM_FRAME_HEADER;
		ucModelData[MICOM_LENGTH]=MICOM_FRAME_LENGTH_ONE;
		ucModelData[MICOM_COMMAND]=MICOM_FRAME_CMD_GET_VER;
		MakeCheckSum(ucModelData,CHECK_NUM_THREE);

		GET_MICOM_SEMA;
		result=DRV_MICOM_TX((int)MICOM_FRAME_LENGTH_FOUR,ucModelData);
		if(result != DRV_OK) {
			PrintError( "[DRV_MICOM_GetVersion] MICOM_TX Error \n" );
				REL_MICOM_SEMA;
				return DRV_ERR;
		}

	result=VK_MSG_ReceiveTimeout(MessageId,ucModelData,MICOM_FRAME_LENGTH_FIVE,(unsigned long)MICOM_TIMEOUT);
		REL_MICOM_SEMA;
		if(result == DRV_OK) {
			if (MICOM_FRAME_CMD_GET_VER == ucModelData[MICOM_COMMAND]) {
				*pulVersion = (ucModelData[MICOM_DATA_TWO] << 8) + (ucModelData[MICOM_DATA_ONE]);
					return DRV_OK;
			} else {
				return DRV_ERR;
			}
		} else {
			return DRV_ERR;
		}
}

/*******************************************************************
  Function	  : DRV_MICOM_GetStrVersion
  Description :
  Input   : none
  Output  : *pucStrVersion
  Return  : DRV_Error
 ********************************************************************/
DRV_Error DRV_MICOM_GetStrVersion( unsigned char *pucStrVersion )
{
	unsigned char ucModelData[DRV_MICOM_MESSAGE_BYTES];
		int			result=DRV_OK;

		PrintEnter();

		ucModelData[MICOM_HEADER]=MICOM_FRAME_HEADER;
		ucModelData[MICOM_LENGTH]=MICOM_FRAME_LENGTH_ONE;
		ucModelData[MICOM_COMMAND]=MICOM_FRAME_CMD_GET_STRVER;
		MakeCheckSum(ucModelData,CHECK_NUM_THREE);

		GET_MICOM_SEMA;
		result=DRV_MICOM_TX((int)MICOM_FRAME_LENGTH_FOUR,ucModelData);
		if(result != DRV_OK) {
			PrintError( "[DRV_MICOM_GetStrVersion] MICOM_TX Error \n" );
				REL_MICOM_SEMA;
				return DRV_ERR;
		}

		result=VK_MSG_ReceiveTimeout(MessageId,ucModelData,14,(unsigned long)MICOM_TIMEOUT);
		REL_MICOM_SEMA;
		if(result == DRV_OK) {
			if (MICOM_FRAME_CMD_GET_STRVER == ucModelData[MICOM_COMMAND]) {
				VK_memcpy(pucStrVersion, &(ucModelData[3]), 10);
					return DRV_OK;
			} else {
				return DRV_ERR;
			}
		} else {
			return DRV_ERR;
		}
}


/*******************************************************************
  Function	  : DRV_MICOM_FrontLedControl
  Description :
  Input   : blue_red(blue:1, red:0)
  Output  :
  Return  : DRV_Error
 ********************************************************************/
DRV_Error DRV_MICOM_FrontLedControl( unsigned char blue_red)
{

		unsigned char ucSetLedControlData[DRV_MICOM_MESSAGE_BYTES];
		int			result=DRV_OK;

		PrintEnter();

		ucSetLedControlData[MICOM_HEADER] = MICOM_FRAME_HEADER;
		ucSetLedControlData[MICOM_LENGTH] = MICOM_FRAME_LENGTH_TWO;
		ucSetLedControlData[MICOM_COMMAND] = MICOM_FRAME_CMD_SET_LED;
		if (blue_red == DRV_MICOM_BLUE_LED_ON)
			ucSetLedControlData[MICOM_DATA_ONE] = DRV_MICOM_BLUE_LED_ON;
		else if (blue_red == DRV_MICOM_LED_OFF)
			ucSetLedControlData[MICOM_DATA_ONE] = DRV_MICOM_LED_OFF;
                else
			ucSetLedControlData[MICOM_DATA_ONE] = DRV_MICOM_RED_LED_ON;
                MakeCheckSum(ucSetLedControlData, CHECK_NUM_FOUR);

                GET_MICOM_SEMA;
                result = DRV_MICOM_TX(MICOM_FRAME_LENGTH_FIVE, ucSetLedControlData);
                if(result != DRV_OK)
                {
                        PrintError ("[DRV_MICOM_SetFrontPowerLedControl] MICOM_TX Error\n");
                                REL_MICOM_SEMA;
                                return DRV_ERR;
                }

		result = VK_MSG_ReceiveTimeout(MessageId, ucSetLedControlData, MICOM_FRAME_LENGTH_FOUR, (unsigned long)MICOM_TIMEOUT);
		REL_MICOM_SEMA;
		if(result == DRV_OK)
		{
			if(ucSetLedControlData[MICOM_COMMAND] == MICOM_FRAME_CMD_SET_LED)
				return DRV_OK;
			else
				return DRV_ERR;
		}
		else
		{
			return DRV_ERR;
		}
}

/********************************************************************
  Function   : DRV_MICOM_SetLocalTime
  Description :
  Input      : stDateTime
  Output     : none
  Return     : DRV_OK
 ********************************************************************/
DRV_Error DRV_MICOM_SetLocalTime(unsigned int unixTime)
{
	unsigned char 	ucTimeData[DRV_MICOM_MESSAGE_BYTES];
		int				result=DRV_OK;

		PrintEnter();

		ucTimeData[MICOM_HEADER] = MICOM_FRAME_HEADER;
		ucTimeData[MICOM_LENGTH] = MICOM_FRAME_LENGTH_FIVE;
		ucTimeData[MICOM_COMMAND] = MICOM_FRAME_CMD_SET_TIME;
		ucTimeData[MICOM_DATA_ONE] = (unsigned char)((unixTime >> 24) & UNIXTIME_BYTE_MASK);
		ucTimeData[MICOM_DATA_TWO] = (unsigned char)((unixTime >> 16) & UNIXTIME_BYTE_MASK);
		ucTimeData[MICOM_DATA_THREE] = (unsigned char)((unixTime >> 8) & UNIXTIME_BYTE_MASK);
		ucTimeData[MICOM_DATA_FOUR] = (unsigned char)(unixTime & UNIXTIME_BYTE_MASK);
		MakeCheckSum(ucTimeData,CHECK_NUM_SEVEN);

		PrintDebug("[DRV_MICOM_SetLocalTime] after convert unixtime %x, (%x:%x:%x:%x)\n",
				unixTime, ucTimeData[MICOM_DATA_ONE], ucTimeData[MICOM_DATA_TWO], ucTimeData[MICOM_DATA_THREE], ucTimeData[MICOM_DATA_FOUR]);

		GET_MICOM_SEMA;
		result = DRV_MICOM_TX( MICOM_FRAME_LENGTH_EIGHT, ucTimeData );
		if(result != DRV_OK)
		{
			PrintError( "[DRV_MICOM_SetLocalTime] MICOM_TX Error \n" );
				REL_MICOM_SEMA;
				return DRV_ERR;
		}

		result=VK_MSG_ReceiveTimeout(MessageId,ucTimeData,MICOM_FRAME_LENGTH_FOUR,(unsigned long)MICOM_TIMEOUT);
		REL_MICOM_SEMA;
		if(result == DRV_OK) {
			if(ucTimeData[MICOM_COMMAND] == MICOM_FRAME_CMD_SET_TIME) {
				return DRV_OK;
			} else {
				return DRV_ERR;
			}

		} else {
			return DRV_ERR;
		}

}

/********************************************************************
  Function   : DRV_MICOM_GetLocalTime
  Description :
  Input      : pstDateTime
  Output     : pstDateTime
  Return     : DRV_Error
 ********************************************************************/
DRV_Error  DRV_MICOM_GetLocalTime(unsigned int *punixTime)
{
	unsigned char 	ucTimeData[DRV_MICOM_MESSAGE_BYTES];
		int				result=DRV_OK;

		PrintEnter();

		ucTimeData[MICOM_HEADER] = MICOM_FRAME_HEADER;
		ucTimeData[MICOM_LENGTH] = MICOM_FRAME_LENGTH_ONE;
		ucTimeData[MICOM_COMMAND] = MICOM_FRAME_CMD_GET_TIME;
		MakeCheckSum(ucTimeData,CHECK_NUM_THREE);

		GET_MICOM_SEMA;
		result = DRV_MICOM_TX( MICOM_FRAME_LENGTH_FOUR, ucTimeData );
		if(result != DRV_OK) {
			PrintError( "[DRV_MICOM_RequestLocalTime] MICOM_TX Error \n" );
				REL_MICOM_SEMA;
				return DRV_ERR;
		}
	result=VK_MSG_ReceiveTimeout(MessageId,ucTimeData,MICOM_FRAME_LENGTH_EIGHT,(unsigned long)MICOM_TIMEOUT);
		REL_MICOM_SEMA;

		if(result == DRV_OK) {

				if (ucTimeData[MICOM_COMMAND] == MICOM_FRAME_CMD_GET_TIME) {

						*punixTime = (unsigned int) (ucTimeData[MICOM_DATA_ONE] << 24 | ucTimeData[MICOM_DATA_TWO] << 16 |
								ucTimeData[MICOM_DATA_THREE] << 8 | ucTimeData[MICOM_DATA_FOUR] );

						if(*punixTime < VALID_UNIXTIME)
						{
							*punixTime = VALID_UNIXTIME;
						}
					PrintDebug("[DRV_MICOM_GetLocalTime] received unix time : %x\n", *punixTime);
						return DRV_OK;
				}
				else{
					return DRV_ERR;
				}
		}
		else{
			return DRV_ERR;
		}
}

/********************************************************************
  Function   : DRV_MICOM_SetWakeupTime
  Description :
  Input      : stDateTime
  Output     : none
  Return     : DRV_OK
 ********************************************************************/
DRV_Error  DRV_MICOM_SetWakeupTime(unsigned int unixTime)
{
	unsigned char 	ucTimeData[DRV_MICOM_MESSAGE_BYTES];
		int				result=DRV_OK;

		PrintEnter();

		ucTimeData[MICOM_HEADER] = MICOM_FRAME_HEADER;
		ucTimeData[MICOM_LENGTH] = MICOM_FRAME_LENGTH_SIX;
		ucTimeData[MICOM_COMMAND] = MICOM_FRAME_CMD_SET_WTIME;
		ucTimeData[MICOM_DATA_ONE] = MICOM_FRAME_CMD_SET_WTIME_ON;
		ucTimeData[MICOM_DATA_TWO] = (unsigned char)((unixTime >> 24) & UNIXTIME_BYTE_MASK);
		ucTimeData[MICOM_DATA_THREE] = (unsigned char)((unixTime >> 16) & UNIXTIME_BYTE_MASK);
		ucTimeData[MICOM_DATA_FOUR] = (unsigned char)((unixTime >> 8) & UNIXTIME_BYTE_MASK);
		ucTimeData[MICOM_DATA_FIVE] = (unsigned char)(unixTime & UNIXTIME_BYTE_MASK);

		MakeCheckSum(ucTimeData,CHECK_NUM_EIGHT);

		GET_MICOM_SEMA;
		result = DRV_MICOM_TX( MICOM_FRAME_LENGTH_NINE, ucTimeData );
		if(result != DRV_OK) {
			PrintError( "[DRV_MICOM_SetWakeupTime] DRV_MICOM_TX Error \n" );
				REL_MICOM_SEMA;
				return DRV_ERR;
		}
	result=VK_MSG_ReceiveTimeout(MessageId,ucTimeData,MICOM_FRAME_LENGTH_FOUR,(unsigned long)MICOM_TIMEOUT);
		REL_MICOM_SEMA;
		if(result == DRV_OK) {
			if (ucTimeData[MICOM_COMMAND] == MICOM_FRAME_CMD_SET_WTIME) {
				return DRV_OK;
			} else {
				return DRV_ERR;
			}

		} else {
			return DRV_ERR;
		}

}
/********************************************************************
Function   : DRV_MICOM_RequestWakeupTime
Description :
Input      : none
Output     : none
Return     : DRV_OK
 ********************************************************************/
DRV_Error  DRV_MICOM_RequestWakeupTime(unsigned int *punixTime , unsigned char *OnOff)
{
	unsigned char		ucTimeData[DRV_MICOM_MESSAGE_BYTES];
		int				result=DRV_OK;

		PrintEnter();

		ucTimeData[MICOM_HEADER] = MICOM_FRAME_HEADER;
		ucTimeData[MICOM_LENGTH] = MICOM_FRAME_LENGTH_ONE;
		ucTimeData[MICOM_COMMAND] = MICOM_FRAME_CMD_GET_WAKEUPTIME;
		MakeCheckSum(ucTimeData,CHECK_NUM_THREE);

		GET_MICOM_SEMA;
		result = DRV_MICOM_TX( MICOM_FRAME_LENGTH_FOUR, ucTimeData );
		if(result != DRV_OK) {
			PrintError( "[DRV_MICOM_RequestWakeupTime] DRV_MICOM_TX Error \n" );
				REL_MICOM_SEMA;
				return DRV_ERR;
		}
	result=VK_MSG_ReceiveTimeout(MessageId,ucTimeData, MICOM_FRAME_LENGTH_NINE,(unsigned long)MICOM_TIMEOUT);
		REL_MICOM_SEMA;
		if(result == DRV_OK) {

				if (ucTimeData[MICOM_COMMAND] == MICOM_FRAME_CMD_GET_WAKEUPTIME) {

						*OnOff = ucTimeData[MICOM_DATA_ONE];
						*punixTime = (unsigned int) (ucTimeData[MICOM_DATA_TWO] << 24 | ucTimeData[MICOM_DATA_THREE] << 16 |
								ucTimeData[MICOM_DATA_FOUR] << 8 | ucTimeData[MICOM_DATA_FIVE] );

						return DRV_OK;
				}
				else{
					return DRV_ERR;
				}
		}
		else{
			PrintError( "[DRV_MICOM_RequestWakeupTime] MSG ReceiveTimeout Error \n" );//tp.
			return DRV_ERR;
		}
}

/********************************************************************
  Function   : DRV_MICOM_DelWakeupTime
  Description :
  Input      : none
  Output     : none
  Return     : DRV_OK
 ********************************************************************/
DRV_Error  DRV_MICOM_DelWakeupTime( void )
{
	unsigned char 	ucDelWakeUpData[DRV_MICOM_MESSAGE_BYTES];
		int				result=DRV_OK;

		PrintEnter();

		ucDelWakeUpData[MICOM_HEADER] = MICOM_FRAME_HEADER;
		ucDelWakeUpData[MICOM_LENGTH] = MICOM_FRAME_LENGTH_TWO;
		ucDelWakeUpData[MICOM_COMMAND] = MICOM_FRAME_CMD_SET_WTIME;
		ucDelWakeUpData[MICOM_DATA_ONE] = MICOM_FRAME_CMD_SET_WTIME_OFF;
		MakeCheckSum(ucDelWakeUpData,CHECK_NUM_FOUR);

		GET_MICOM_SEMA;
		result = DRV_MICOM_TX( MICOM_FRAME_LENGTH_FIVE, ucDelWakeUpData );
		if( result != DRV_OK )
		{
			PrintError( "[DRV_MICOM_DelWakeUpTime] DRV_MICOM_TX Error \n" );
				REL_MICOM_SEMA;
				return DRV_ERR;
		}
	result=VK_MSG_ReceiveTimeout(MessageId,ucDelWakeUpData,MICOM_FRAME_LENGTH_FOUR,(unsigned long)MICOM_TIMEOUT);
		REL_MICOM_SEMA;
		if(result == DRV_OK) {
			if(ucDelWakeUpData[MICOM_COMMAND] == MICOM_FRAME_CMD_SET_WTIME) {
				return DRV_OK;
			} else {
				return DRV_ERR;
			}
		} else {
			return DRV_ERR;
		}
}

/********************************************************************
  Function   : DRV_MICOM_SetStandby
  Description :
  Input      : none
  Output     : none
  Return     : DRV_OK
 ********************************************************************/
DRV_Error DRV_MICOM_SetStandby( void )
{
	unsigned char 	ucSetStandbyData[DRV_MICOM_MESSAGE_BYTES];
		int				result=DRV_OK;

		PrintEnter();

		ucSetStandbyData[MICOM_HEADER] = MICOM_FRAME_HEADER;
		ucSetStandbyData[MICOM_LENGTH] = MICOM_FRAME_LENGTH_TWO;
		ucSetStandbyData[MICOM_COMMAND] = MICOM_FRAME_CMD_SET_STANDBY;
		ucSetStandbyData[MICOM_DATA_ONE] = MICOM_FRAME_CMD_SET_STANDBY_ON;
		MakeCheckSum(ucSetStandbyData,CHECK_NUM_FOUR);

		GET_MICOM_SEMA;
		result = DRV_MICOM_TX( MICOM_FRAME_LENGTH_FIVE, ucSetStandbyData );
		if( result != DRV_OK )
		{
			PrintError( "[DRV_MICOM_SetStandby] DRV_MICOM_TX Error \n" );
				REL_MICOM_SEMA;
				return DRV_ERR;
		}

		result=VK_MSG_ReceiveTimeout(MessageId,ucSetStandbyData,MICOM_FRAME_LENGTH_FOUR,(unsigned long)MICOM_TIMEOUT);
		REL_MICOM_SEMA;
		if(result == DRV_OK) {
			if(ucSetStandbyData[MICOM_COMMAND] == MICOM_FRAME_CMD_SET_STANDBY)
				return DRV_OK;
			else
				return DRV_ERR;
		} else {
			return DRV_ERR;
		}

}

/********************************************************************
  Function   : DRV_MICOM_STBPowerOn
  Description :
  Input      : none
  Output     : none
  Return     : DRV_OK
 ********************************************************************/
DRV_Error DRV_MICOM_STBPowerOn( void )
{
	unsigned char 	ucSTBPowerData[DRV_MICOM_MESSAGE_BYTES];
		int				result=DRV_OK;

		PrintEnter();

		ucSTBPowerData[MICOM_HEADER] = MICOM_FRAME_HEADER;
		ucSTBPowerData[MICOM_LENGTH] = MICOM_FRAME_LENGTH_ONE;
		ucSTBPowerData[MICOM_COMMAND] = MICOM_FRAME_CMD_SET_WAKEUP;
		MakeCheckSum(ucSTBPowerData,CHECK_NUM_THREE);

		GET_MICOM_SEMA;
		result = DRV_MICOM_TX( MICOM_FRAME_LENGTH_FOUR, ucSTBPowerData );
		if( result != DRV_OK )
		{
			PrintError( "[DRV_MICOM_STBPowerOn] DRV_MICOM_TX Error \n" );
				REL_MICOM_SEMA;
				return DRV_ERR;
		}

		result=VK_MSG_ReceiveTimeout(MessageId,ucSTBPowerData,MICOM_FRAME_LENGTH_FOUR,(unsigned long)MICOM_TIMEOUT);
		REL_MICOM_SEMA;
		if(result == DRV_OK) {
			if(ucSTBPowerData[MICOM_COMMAND] == MICOM_FRAME_CMD_SET_WAKEUP)
				return DRV_OK;
			else
				return DRV_ERR;
		} else {
			return DRV_ERR;
		}
}

/********************************************************************
  Function   : DRV_MICOM_STBPowerReboot
  Description :
  Input	  : none
  Output	  : none
  Return	  : DRV_OK
 ********************************************************************/
DRV_Error DRV_MICOM_STBPowerReboot( void )
{
	unsigned char	ucSTBPowerData[DRV_MICOM_MESSAGE_BYTES];
		int				result=DRV_OK;

		PrintEnter();

		ucSTBPowerData[MICOM_HEADER] = MICOM_FRAME_HEADER;
		ucSTBPowerData[MICOM_LENGTH] = MICOM_FRAME_LENGTH_ONE;
		ucSTBPowerData[MICOM_COMMAND] = MICOM_FRAME_CMD_POWER_REBOOT;
		MakeCheckSum(ucSTBPowerData,CHECK_NUM_THREE);

		GET_MICOM_SEMA;
		result = DRV_MICOM_TX( MICOM_FRAME_LENGTH_FOUR, ucSTBPowerData );
		if( result != DRV_OK )
		{
			PrintError( "[DRV_MICOM_STBPowerReboot] DRV_MICOM_TX Error \n" );
				REL_MICOM_SEMA;
				return DRV_ERR;
		}

	result=VK_MSG_ReceiveTimeout(MessageId,ucSTBPowerData,MICOM_FRAME_LENGTH_FOUR,(unsigned long)MICOM_TIMEOUT);
	REL_MICOM_SEMA;
	if(result == DRV_OK) {
		if(ucSTBPowerData[MICOM_COMMAND] == MICOM_FRAME_CMD_POWER_REBOOT)
			return DRV_OK;
		else
			return DRV_ERR;
	} else {
		return DRV_ERR;
	}
}

/********************************************************************
  Function   : DRV_MICOM_RequestPowerState
  Description :
  Input      : none
  Output     : none
  Return     : DRV_OK
 ********************************************************************/
DRV_Error  DRV_MICOM_RequestPowerState(unsigned char *powerState)
{
	unsigned char 	ucPowerStateData[DRV_MICOM_MESSAGE_BYTES];
		int				result=DRV_OK;

		PrintEnter();

		ucPowerStateData[MICOM_HEADER] = MICOM_FRAME_HEADER;
		ucPowerStateData[MICOM_LENGTH] = MICOM_FRAME_LENGTH_ONE;
		ucPowerStateData[MICOM_COMMAND] = MICOM_FRAME_CMD_GET_POWERSTATE;
		MakeCheckSum(ucPowerStateData,CHECK_NUM_THREE);

		GET_MICOM_SEMA;
		result = DRV_MICOM_TX( MICOM_FRAME_LENGTH_FOUR, ucPowerStateData );
		if( result != DRV_OK )
		{
			PrintError( "[DRV_MICOM_SetStandby] DRV_MICOM_TX Error \n" );
				REL_MICOM_SEMA;
				return DRV_ERR;
		}
	result=VK_MSG_ReceiveTimeout(MessageId,ucPowerStateData,MICOM_FRAME_LENGTH_FIVE,(unsigned long)MICOM_TIMEOUT);
		REL_MICOM_SEMA;
		if(result == DRV_OK) {
			if(ucPowerStateData[MICOM_COMMAND] == MICOM_FRAME_CMD_GET_POWERSTATE) {
				*powerState = ucPowerStateData[MICOM_DATA_ONE];
					return DRV_OK;
			} else {
				return DRV_ERR;
			}

		} else {
			return DRV_ERR;
		}
}

/********************************************************************
  Function   : DRV_MICOM_SetWatchdog
  Description :
  Input      : none
  Output     : none
  Return     : DRV_OK
 ********************************************************************/
DRV_Error DRV_MICOM_SetWatchdog(HBOOL bOn_nOff, unsigned char ucTimeOut)
{
	unsigned char	 ucWatchdogData[DRV_MICOM_MESSAGE_BYTES];
		int			 result = DRV_OK;

		PrintEnter();

		ucWatchdogData[MICOM_HEADER] = MICOM_FRAME_HEADER;
		ucWatchdogData[MICOM_LENGTH] = MICOM_FRAME_LENGTH_THREE;
		ucWatchdogData[MICOM_COMMAND] = MICOM_FRAME_CMD_WATCHDOG;
		ucWatchdogData[MICOM_DATA_ONE] = (unsigned char)bOn_nOff;
		ucWatchdogData[MICOM_DATA_TWO] = ucTimeOut;

		MakeCheckSum(ucWatchdogData,CHECK_NUM_FIVE);

		GET_MICOM_SEMA;
		result = DRV_MICOM_TX( MICOM_FRAME_LENGTH_SIX, ucWatchdogData );
		if( result != DRV_OK )
		{
			PrintError( "[%s] DRV_MICOM_TX Error \n", __func__ );
				REL_MICOM_SEMA;
				return DRV_ERR;
		}

		result=VK_MSG_ReceiveTimeout(MessageId,ucWatchdogData,MICOM_FRAME_LENGTH_FOUR,(unsigned long)MICOM_TIMEOUT);
		REL_MICOM_SEMA;
		if(result == DRV_OK) {
			if(ucWatchdogData[MICOM_COMMAND] == MICOM_FRAME_CMD_WATCHDOG)
				return DRV_OK;
			else
				return DRV_ERR;
		} else {
			return DRV_ERR;
		}

}


/********************************************************************
  Function   : DRV_MICOM_DisplayTime
  Description :
  Input      : none
  Output     : none
  Return     : DRV_OK
 ********************************************************************/
DRV_Error  DRV_MICOM_DisplayTime(unsigned char on_off)
{

		unsigned char 	ucDisplay[DRV_MICOM_MESSAGE_BYTES];
		int				result=DRV_OK;

		PrintEnter();

		ucDisplay[MICOM_HEADER] = MICOM_FRAME_HEADER;
		ucDisplay[MICOM_LENGTH] = MICOM_FRAME_LENGTH_TWO;
		ucDisplay[MICOM_COMMAND] = MICOM_FRAME_CMD_SET_DISPLAY_TIME;

		if(on_off)
			ucDisplay[MICOM_DATA_ONE] = 1;
		else
			ucDisplay[MICOM_DATA_ONE] = 0;

				MakeCheckSum(ucDisplay,CHECK_NUM_FOUR);

				GET_MICOM_SEMA;
				result = DRV_MICOM_TX( MICOM_FRAME_LENGTH_FIVE, ucDisplay );
				if( result != DRV_OK )
				{
					PrintError( "[DRV_MICOM_DisplayTime] DRV_MICOM_TX Error \n" );
						REL_MICOM_SEMA;
						return DRV_ERR;
				}

		result=VK_MSG_ReceiveTimeout(MessageId,ucDisplay,MICOM_FRAME_LENGTH_FIVE,(unsigned long)MICOM_TIMEOUT);
		REL_MICOM_SEMA;
		if(result == DRV_OK) {
			if(ucDisplay[MICOM_COMMAND] == MICOM_FRAME_CMD_SET_DISPLAY_TIME) {
				return DRV_OK;
			} else {
				return DRV_ERR;
			}

		} else {
			return DRV_ERR;
		}
}

/********************************************************************
  Function   : DRV_MICOM_GetWakeupSrc
  Description :
  Input      : none
  Output     : none
  Return     : DRV_OK
 ********************************************************************/
DRV_Error  DRV_MICOM_GetWakeupSrc(DRV_MICOM_WAKEUP_SRC *peWakeupSrc)
{
	unsigned char 	ucGetWakeUpSrcData[DRV_MICOM_MESSAGE_BYTES];
		int				result=DRV_OK;

		PrintEnter();

		ucGetWakeUpSrcData[MICOM_HEADER] = MICOM_FRAME_HEADER;
		ucGetWakeUpSrcData[MICOM_LENGTH] = MICOM_FRAME_LENGTH_ONE;
		ucGetWakeUpSrcData[MICOM_COMMAND] = MICOM_FRAME_CMD_GET_WAKEUPSRC;
		MakeCheckSum(ucGetWakeUpSrcData,CHECK_NUM_THREE);

		GET_MICOM_SEMA;
		result = DRV_MICOM_TX( MICOM_FRAME_LENGTH_FOUR, ucGetWakeUpSrcData );
		if( result != DRV_OK )
		{
			PrintError( "[DRV_MICOM_GetWakeUpSrc] DRV_MICOM_TX Error \n" );
				REL_MICOM_SEMA;
				return DRV_ERR;
		}

		result=VK_MSG_ReceiveTimeout(MessageId,ucGetWakeUpSrcData,MICOM_FRAME_LENGTH_FIVE,(unsigned long)MICOM_TIMEOUT);
		REL_MICOM_SEMA;
		if(result == DRV_OK) {
			if (ucGetWakeUpSrcData[MICOM_COMMAND] == MICOM_FRAME_CMD_GET_WAKEUPSRC) {
				switch(ucGetWakeUpSrcData[MICOM_DATA_ONE])
				{
					case 1:
						   *peWakeupSrc = DRV_MICOM_WAKEUP_BY_KEY;
							   break;

					case 2:
						   *peWakeupSrc = DRV_MICOM_WAKEUP_BY_RCU;
							   break;

					case 3:
						   *peWakeupSrc = DRV_MICOM_WAKEUP_BY_TIMER;
							   break;

					case 5:
						   *peWakeupSrc = DRV_MICOM_WAKEUP_BY_REBOOT;
							   break;
					case 9:
						   *peWakeupSrc = DRV_MICOM_WAKEUP_BY_WOL;
							   break;
					case 4:
					default:
							*peWakeupSrc = DRV_MICOM_WAKEUP_BY_ACPOWER;
								break;
				}
				return DRV_OK;
			} else {
				return DRV_ERR;
			}
		} else {
			return DRV_ERR;
		}
}

DRV_Error DRV_MICOM_SetWakeOnLAN(HBOOL bOn_nOff)
{
	unsigned char 	ucWakeOnLanData[DRV_MICOM_MESSAGE_BYTES];
		int				result = DRV_OK;

		PrintEnter();

		ucWakeOnLanData[MICOM_HEADER] = MICOM_FRAME_HEADER;
		ucWakeOnLanData[MICOM_LENGTH] = MICOM_FRAME_LENGTH_TWO;
		ucWakeOnLanData[MICOM_COMMAND] = MICOM_FRAME_CMD_SET_WOL;
		ucWakeOnLanData[MICOM_DATA_ONE] = (unsigned char)bOn_nOff;
		MakeCheckSum(ucWakeOnLanData,CHECK_NUM_FOUR);

		GET_MICOM_SEMA;
		result = DRV_MICOM_TX( MICOM_FRAME_LENGTH_FIVE, ucWakeOnLanData );
		if( result != DRV_OK )
		{
			PrintError( "[DRV_MICOM_SetWakeOnLAN] MICOM_TX_Bytes Error \n" );
				REL_MICOM_SEMA;
				return DRV_ERR;
		}

		result=VK_MSG_ReceiveTimeout(MessageId,ucWakeOnLanData,MICOM_FRAME_LENGTH_FOUR,(unsigned long)MICOM_TIMEOUT);
		REL_MICOM_SEMA;
		if(result == DRV_OK) {
			if(ucWakeOnLanData[MICOM_COMMAND] == MICOM_FRAME_CMD_SET_WOL)
				return DRV_OK;
			else
				return DRV_ERR;
		} else {
			return DRV_ERR;
		}
}

DRV_Error DRV_MICOM_SetFastBoot(HBOOL bOn_nOff)
{
	unsigned char 	ucWakeOnLanData[DRV_MICOM_MESSAGE_BYTES];
	int				result = DRV_OK;

	PrintEnter();

	ucWakeOnLanData[MICOM_HEADER] = MICOM_FRAME_HEADER;
	ucWakeOnLanData[MICOM_LENGTH] = MICOM_FRAME_LENGTH_TWO;
	ucWakeOnLanData[MICOM_COMMAND] = MICOM_FRAME_CMD_SET_FAST_BOOT;
	ucWakeOnLanData[MICOM_DATA_ONE] = (unsigned char)bOn_nOff;
	MakeCheckSum(ucWakeOnLanData,CHECK_NUM_FOUR);

	GET_MICOM_SEMA;
	result = DRV_MICOM_TX( MICOM_FRAME_LENGTH_FIVE, ucWakeOnLanData );
	if( result != DRV_OK )
	{
		PrintError( "[DRV_MICOM_SetFastBoot] MICOM_TX_Bytes Error \n" );
		REL_MICOM_SEMA;
		return DRV_ERR;
	}

	result=VK_MSG_ReceiveTimeout(MessageId,ucWakeOnLanData,MICOM_FRAME_LENGTH_FOUR,(unsigned long)MICOM_TIMEOUT);
	REL_MICOM_SEMA;
	if(result == DRV_OK) {
		if(ucWakeOnLanData[MICOM_COMMAND] == MICOM_FRAME_CMD_SET_FAST_BOOT)
			return DRV_OK;
		else
			return DRV_ERR;
	} else {
		return DRV_ERR;
	}
}

#if defined(CONFIG_FRONT_MICOM_TOUCH)
DRV_Error  DRV_MICOM_GetTouchSensitive(unsigned char *pSensitiveData, unsigned char *pKeyNum)
{
	unsigned char 	ucGetTouchSense[MICOM_MESSAGE_BYTES];
		int				result=DRV_OK;

		PrintEnter();

		if((pSensitiveData==NULL) || (pKeyNum==NULL))
		{
			PrintError( "[DRV_MICOM_GetTouchSensitive] Invalid Parm!!\n" );
				return DRV_ERR;
		}

		ucGetTouchSense[MICOM_HEADER] = MICOM_FRAME_HEADER;
		ucGetTouchSense[MICOM_LENGTH] = MICOM_FRAME_LENGTH_ONE;
		ucGetTouchSense[MICOM_COMMAND] = MICOM_FRAME_CMD_GET_TOUCH_KEYINFO;
		MakeCheckSum(ucGetTouchSense,CHECK_NUM_THREE);

		GET_MICOM_SEMA;
		result = DRV_MICOM_TX(MICOM_FRAME_LENGTH_FOUR, ucGetTouchSense );
		if( result != DRV_OK )
		{
			PrintError( "[DRV_MICOM_GetTouchSensitive] DRV_MICOM_TX Error \n" );
				REL_MICOM_SEMA;
				return DRV_ERR;
		}

		result=VK_MSG_ReceiveTimeout(MessageId,ucGetTouchSense,MICOM_MESSAGE_BYTES,(unsigned long)MICOM_TIMEOUT);
		REL_MICOM_SEMA;
		if(result == DRV_OK)
		{
			*pKeyNum = ucGetTouchSense[MICOM_LENGTH]-1;
				VK_MEM_Memcpy(pSensitiveData, &ucGetTouchSense[MICOM_DATA_ONE], *pKeyNum);
				return DRV_OK;
		}
		else
		{
			return DRV_ERR;
		}
}

DRV_Error  DRV_MICOM_GetTouchSignal(unsigned short *pSignalData, unsigned char *pKeyNum)
{
	unsigned char 	ucGetTouchSense[MICOM_MESSAGE_BYTES];
		int				i, result=DRV_OK;

		PrintEnter();

		if((pSignalData==NULL) || (pKeyNum==NULL))
		{
			PrintError( "[DRV_MICOM_GetTouchSignal] Invalid Parm!!\n" );
				return DRV_ERR;
		}

		ucGetTouchSense[MICOM_HEADER] = MICOM_FRAME_HEADER;
		ucGetTouchSense[MICOM_LENGTH] = MICOM_FRAME_LENGTH_ONE;
		ucGetTouchSense[MICOM_COMMAND] = MICOM_FRAME_CMD_GET_TOUCH_KEYSIGNAL;
		MakeCheckSum(ucGetTouchSense,CHECK_NUM_THREE);

		GET_MICOM_SEMA;
		result = DRV_MICOM_TX(MICOM_FRAME_LENGTH_FOUR, ucGetTouchSense );
		if( result != DRV_OK )
		{
			PrintError( "[DRV_MICOM_GetTouchSignal] DRV_MICOM_TX Error \n" );
				REL_MICOM_SEMA;
				return DRV_ERR;
		}

		result=VK_MSG_ReceiveTimeout(MessageId,ucGetTouchSense,MICOM_MESSAGE_BYTES,(unsigned long)MICOM_TIMEOUT);
		REL_MICOM_SEMA;
		if(result == DRV_OK)
		{
			unsigned char index = MICOM_DATA_ONE;
				*pKeyNum = (ucGetTouchSense[MICOM_LENGTH]-1)/2;
				for(i=0; i<*pKeyNum; i++)
				{
					pSignalData[i] = (ucGetTouchSense[index]<<8) | ucGetTouchSense[index+1];
						index += 2;
				}
			return DRV_OK;
		}
		else
		{
			return DRV_ERR;
		}
}

DRV_Error  DRV_MICOM_GetTouchReference(unsigned short *pRefrenceData, unsigned char *pKeyNum)
{
	unsigned char 	ucGetTouchSense[MICOM_MESSAGE_BYTES];
		int				i, result=DRV_OK;

		PrintEnter();

		if((pRefrenceData==NULL) || (pKeyNum==NULL))
		{
			PrintError( "[DRV_MICOM_GetTouchReference] Invalid Parm!!\n" );
				return DRV_ERR;
		}

		ucGetTouchSense[MICOM_HEADER] = MICOM_FRAME_HEADER;
		ucGetTouchSense[MICOM_LENGTH] = MICOM_FRAME_LENGTH_ONE;
		ucGetTouchSense[MICOM_COMMAND] = MICOM_FRAME_CMD_GET_TOUCH_KEYREFERENCE;
		MakeCheckSum(ucGetTouchSense,CHECK_NUM_THREE);

		GET_MICOM_SEMA;
		result = DRV_MICOM_TX(MICOM_FRAME_LENGTH_FOUR, ucGetTouchSense );
		if( result != DRV_OK )
		{
			PrintError( "[DRV_MICOM_GetTouchReference] DRV_MICOM_TX Error \n" );
				REL_MICOM_SEMA;
				return DRV_ERR;
		}

		result=VK_MSG_ReceiveTimeout(MessageId,ucGetTouchSense,MICOM_MESSAGE_BYTES,(unsigned long)MICOM_TIMEOUT);
		REL_MICOM_SEMA;
		if(result == DRV_OK)
		{
			unsigned char index = MICOM_DATA_ONE;
				*pKeyNum = (ucGetTouchSense[MICOM_LENGTH]-1)/2;
				for(i=0; i<*pKeyNum; i++)
				{
					pRefrenceData[i] = (ucGetTouchSense[index]<<8) | ucGetTouchSense[index+1];
						index += 2;
				}
			return DRV_OK;
		}
		else
		{
			return DRV_ERR;
		}
}

DRV_Error DRV_MICOM_SetTouchThreshold(unsigned char *pucThreshold, unsigned char ucTouchNum)
{
	unsigned char 	ucTouchThreshold[DRV_MICOM_MESSAGE_BYTES];
	int				result = DRV_OK, i;

	PrintEnter();

	ucTouchThreshold[MICOM_HEADER] = MICOM_FRAME_HEADER;
	ucTouchThreshold[MICOM_LENGTH] = MICOM_FRAME_LENGTH_ONE + ucTouchNum;
	ucTouchThreshold[MICOM_COMMAND] = MICOM_FRAME_CMD_SET_TOUCH_SENSITIVITY;

	for(i=0; i<ucTouchNum; i++)
	{
		ucTouchThreshold[MICOM_DATA_ONE+i] = pucThreshold[i];
	}

	MakeCheckSum(ucTouchThreshold, (CHECK_NUM_THREE + ucTouchNum));

	GET_MICOM_SEMA;
	result = DRV_MICOM_TX( (CHECK_NUM_FOUR+ ucTouchNum), ucTouchThreshold );
	if( result != DRV_OK )
	{
		PrintError(" MICOM_TX_Bytes Error \n");
		REL_MICOM_SEMA;
		return DRV_ERR;
	}

	result=VK_MSG_ReceiveTimeout(MessageId,ucTouchThreshold,MICOM_FRAME_LENGTH_FOUR,(unsigned long)MICOM_TIMEOUT);
	REL_MICOM_SEMA;
	if(result == DRV_OK)
	{
		if(ucTouchThreshold[MICOM_COMMAND] == MICOM_FRAME_CMD_SET_TOUCH_SENSITIVITY)
		{
			return DRV_OK;
		}
		else
		{
			PrintError(" received MICOM_COMMAND does not match! \n");
			return DRV_ERR;
		}
	}
	else
	{
		PrintError(" VK_MSG_ReceiveTimeout error!\n");
		return DRV_ERR;
	}
}

DRV_Error DRV_MICOM_GetTouchThreshold(unsigned char *pucThreshold, unsigned char *pKeyNum)
{
	unsigned char 	ucTouchThreshold[MICOM_MESSAGE_BYTES];
	int				result = DRV_OK;
	int i;

	PrintEnter();

	if((pucThreshold==NULL) || (pKeyNum==NULL))
	{
		PrintError(" Invalid Parm!!\n");
			return DRV_ERR;
	}

	ucTouchThreshold[MICOM_HEADER] = MICOM_FRAME_HEADER;
	ucTouchThreshold[MICOM_LENGTH] = MICOM_FRAME_LENGTH_ONE;
	ucTouchThreshold[MICOM_COMMAND] = MICOM_FRAME_CMD_GET_TOUCH_SENSITIVITY;
	MakeCheckSum(ucTouchThreshold,CHECK_NUM_THREE);

	GET_MICOM_SEMA;
	result = DRV_MICOM_TX( MICOM_FRAME_LENGTH_FOUR, ucTouchThreshold );
	if( result != DRV_OK )
	{
		PrintError( " MICOM_TX_Bytes Error \n" );
		REL_MICOM_SEMA;
		return DRV_ERR;
	}

	result=VK_MSG_ReceiveTimeout(MessageId,ucTouchThreshold,MICOM_MESSAGE_BYTES,(unsigned long)MICOM_TIMEOUT);
	REL_MICOM_SEMA;
	if(result == DRV_OK)
	{
		unsigned char index = MICOM_DATA_ONE;
		*pKeyNum = (ucTouchThreshold[MICOM_LENGTH]-1);
		for(i=0; i<*pKeyNum; i++)
		{
			pucThreshold[i] = ucTouchThreshold[index];
			index++;
		}
		return DRV_OK;
	}
	else
	{
		PrintError(" VK_MSG_ReceiveTimeout error!\n");
		return DRV_ERR;
	}
}
#endif

DRV_Error DRV_MICOM_EnableRescueReset(HBOOL bOn_nOff)
{
	unsigned char 	ucRescueResetData[DRV_MICOM_MESSAGE_BYTES];
		int				result = DRV_OK;

		PrintEnter();

		ucRescueResetData[MICOM_HEADER] = MICOM_FRAME_HEADER;
		ucRescueResetData[MICOM_LENGTH] = MICOM_FRAME_LENGTH_THREE;
		ucRescueResetData[MICOM_COMMAND] = MICOM_FRAME_CMD_SET_RESCUE_RESET;
		ucRescueResetData[MICOM_DATA_ONE] = bOn_nOff;
		ucRescueResetData[MICOM_DATA_TWO] = 15;	/* Need to be modified to use function parameter */
		MakeCheckSum(ucRescueResetData, CHECK_NUM_FIVE);

		GET_MICOM_SEMA;
		result = DRV_MICOM_TX( MICOM_FRAME_LENGTH_SIX, ucRescueResetData );
		if( result != DRV_OK )
		{
			PrintError( "[DRV_MICOM_EnableRescueReset] MICOM_TX_Bytes Error \n" );
				REL_MICOM_SEMA;
				return DRV_ERR;
		}

		result=VK_MSG_ReceiveTimeout(MessageId,ucRescueResetData,MICOM_FRAME_LENGTH_FOUR,(unsigned long)MICOM_TIMEOUT);
		REL_MICOM_SEMA;
		if(result == DRV_OK) {
			if(ucRescueResetData[MICOM_COMMAND] == MICOM_FRAME_CMD_SET_RESCUE_RESET)
				return DRV_OK;
			else
				return DRV_ERR;
		} else {
			return DRV_ERR;
		}
}

/********************************************************************
  Function   : DRV_MICOM_SET_Icon
  Description :
  Input      : none
  Output     : none
  Return     : DRV_OK
 ********************************************************************/
DRV_Error  DRV_MICOM_SET_Icon(unsigned char position, unsigned char value)
{
	unsigned char 	icon_data[DRV_MICOM_MESSAGE_BYTES];
		int				result=DRV_OK;

		PrintEnter();

		icon_data[MICOM_HEADER] = MICOM_FRAME_HEADER;
		icon_data[MICOM_LENGTH] = MICOM_FRAME_LENGTH_THREE;
		icon_data[MICOM_COMMAND] = MICOM_FRAME_CMD_SET_ICON;
		icon_data[MICOM_DATA_ONE] = position;
		icon_data[MICOM_DATA_TWO] = value;

		MakeCheckSum(icon_data,CHECK_NUM_FIVE);

		GET_MICOM_SEMA;
		result = DRV_MICOM_TX(MICOM_FRAME_LENGTH_SIX, icon_data);
		if( result != DRV_OK )
		{
			PrintError( "[DRV_MICOM_SET_Icon] DRV_MICOM_TX Error \n" );
				REL_MICOM_SEMA;
				return DRV_ERR;
		}

		result=VK_MSG_ReceiveTimeout(MessageId,icon_data,MICOM_FRAME_LENGTH_FOUR,(unsigned long)MICOM_TIMEOUT);
		REL_MICOM_SEMA;
		if(result == DRV_OK) {
			if(icon_data[MICOM_COMMAND] == MICOM_FRAME_CMD_SET_ICON) {
				return DRV_OK;
			} else {
				return DRV_ERR;
			}

		} else {
			return DRV_ERR;
		}
}

/********************************************************************
  Function   : DRV_MICOM_SET_Icon
  Description :
  Input      : none
  Output     : none
  Return     : DRV_OK
 ********************************************************************/
DRV_Error  DRV_MICOM_SetLedControl(HUINT32 ulLedMask, HBOOL bOn_nOff)
{
	unsigned char 	icon_data[DRV_MICOM_MESSAGE_BYTES];
		int				result=DRV_OK;

		PrintEnter();

		icon_data[MICOM_HEADER] = MICOM_FRAME_HEADER;
		icon_data[MICOM_LENGTH] = MICOM_FRAME_LENGTH_FIVE;
		icon_data[MICOM_COMMAND] = MICOM_FRAME_CMD_SET_ICON;
		icon_data[MICOM_DATA_ONE] = (unsigned char)(ulLedMask >> 16);
		icon_data[MICOM_DATA_TWO] = (unsigned char)(ulLedMask >> 8);
		icon_data[MICOM_DATA_THREE] = (unsigned char)ulLedMask;
		icon_data[MICOM_DATA_FOUR] = (unsigned char)bOn_nOff;

		MakeCheckSum(icon_data, CHECK_NUM_SEVEN);

		GET_MICOM_SEMA;
		result = DRV_MICOM_TX(MICOM_FRAME_LENGTH_EIGHT, icon_data);
		if( result != DRV_OK )
		{
			PrintError( "[DRV_MICOM_SET_Icon] DRV_MICOM_TX Error \n" );
				REL_MICOM_SEMA;
				return DRV_ERR;
		}

		result=VK_MSG_ReceiveTimeout(MessageId,icon_data,MICOM_FRAME_LENGTH_FOUR,(unsigned long)MICOM_TIMEOUT);
		REL_MICOM_SEMA;
		if(result == DRV_OK)
		{
			if(icon_data[MICOM_COMMAND] == MICOM_FRAME_CMD_SET_ICON){
				return DRV_OK;
			} else {
				return DRV_ERR;
			}

		} else {
			return DRV_ERR;
		}
}


/********************************************************************
  Function   : DRV_MICOM_WriteString
  Description :
  Input      : none
  Output     : none
  Return     : DRV_OK
 ********************************************************************/
DRV_Error  DRV_MICOM_WriteString(unsigned char *string, unsigned char len)
{
	unsigned char ucDisplay[DRV_MICOM_MESSAGE_BYTES], i;
		DRV_Error result;
		unsigned char ucMICOM_CMD;

		PrintEnter();

#if	defined(CONFIG_PANEL_7SEG)
		ucMICOM_CMD = MICOM_FRAME_CMD_DISPLAY_7SEG;
		len = 4;
#else
		ucMICOM_CMD = MICOM_FRAME_CMD_WRITE_STR;
		if(len > 16)
			len = 16;
#endif

				ucDisplay[MICOM_HEADER] = MICOM_FRAME_HEADER;
				ucDisplay[MICOM_LENGTH] = MICOM_FRAME_LENGTH_ONE+len;
				ucDisplay[MICOM_COMMAND] = ucMICOM_CMD;

				for(i=0; i<len; i++)
				{
#if	defined(CONFIG_PANEL_7SEG)
					/* 7segment ÀÇ °æ¿ì, Áö¿ø°¡´É ¹®ÀÚ¿¡ Á¦ÇÑÀÌ ÀÖÀ¸¹Ç·Î,  7segment ¿¡ ¸Â°Ô ÀüÈ¯ ÇÊ¿ä */
						ucDisplay[MICOM_DATA_ONE+i] = s_ucaPanelCharMap[ (string[i]&0x7F) ];
#else
						ucDisplay[MICOM_DATA_ONE+i] = string[i];
#endif
				}

		PrintDebug("[DRV_MICOM_WriteStrLcd] len = %d, ucDisplay[MICOM_LENGTH] = %d\n", len, ucDisplay[MICOM_LENGTH]);

		MakeCheckSum(ucDisplay,CHECK_NUM_THREE+len);

		GET_MICOM_SEMA;
		result = DRV_MICOM_TX( ucDisplay[MICOM_LENGTH] + MICOM_FRAME_LENGTH_THREE, ucDisplay );
		if( result != DRV_OK )
		{
			PrintError( "[DRV_MICOM_WriteStrLcd] DRV_MICOM_TX Error \n" );
				REL_MICOM_SEMA;
				return DRV_ERR;
		}

		result=VK_MSG_ReceiveTimeout(MessageId,ucDisplay,MICOM_FRAME_LENGTH_FOUR,(unsigned long)MICOM_TIMEOUT);
		REL_MICOM_SEMA;
		if(result == DRV_OK) {
			if(ucDisplay[MICOM_COMMAND] == ucMICOM_CMD) {
				return DRV_OK;
			} else {
				return DRV_ERR;
			}

		} else {
			return DRV_ERR;
		}
}

DRV_Error DRV_MICOM_SetPowerSavingMode(HBOOL bOn_nOff)
{
	unsigned char 	ucPowerSavingData[DRV_MICOM_MESSAGE_BYTES];
		int				result = DRV_OK;
			PrintError( "[%s][%d] \n", __func__,__LINE__ );

		PrintEnter();

		ucPowerSavingData[MICOM_HEADER] = MICOM_FRAME_HEADER;
		ucPowerSavingData[MICOM_LENGTH] = MICOM_FRAME_LENGTH_TWO;
		ucPowerSavingData[MICOM_COMMAND] = MICOM_FRAME_CMD_SET_PWR_SAV;
		ucPowerSavingData[MICOM_DATA_ONE] = (unsigned char)bOn_nOff;
		MakeCheckSum(ucPowerSavingData,CHECK_NUM_FOUR);

		GET_MICOM_SEMA;
		result = DRV_MICOM_TX( MICOM_FRAME_LENGTH_FIVE, ucPowerSavingData );
		if( result != DRV_OK )
		{
			PrintError( "[%s] DRV_MICOM_TX Error \n", __func__ );
				REL_MICOM_SEMA;
				return DRV_ERR;
		}

		result=VK_MSG_ReceiveTimeout(MessageId,ucPowerSavingData,MICOM_FRAME_LENGTH_FOUR,(unsigned long)MICOM_TIMEOUT);
		REL_MICOM_SEMA;
		if(result == DRV_OK) {
			if(ucPowerSavingData[MICOM_COMMAND] == MICOM_FRAME_CMD_SET_PWR_SAV)
				return DRV_OK;
			else
				return DRV_ERR;
		} else {
			return DRV_ERR;
		}

}

DRV_Error DRV_MICOM_SetDisplayAni(HBOOL bOn_nOff)
{
	unsigned char 	ucDisplayAniData[DRV_MICOM_MESSAGE_BYTES];
		int				result = DRV_OK;

		PrintEnter();

		ucDisplayAniData[MICOM_HEADER] = MICOM_FRAME_HEADER;
		ucDisplayAniData[MICOM_LENGTH] = MICOM_FRAME_LENGTH_TWO;
		ucDisplayAniData[MICOM_COMMAND] = MICOM_FRAME_CMD_SET_DIS_ANI;
		ucDisplayAniData[MICOM_DATA_ONE] = (unsigned char)bOn_nOff;
		MakeCheckSum(ucDisplayAniData,CHECK_NUM_FOUR);

		GET_MICOM_SEMA;
		result = DRV_MICOM_TX( MICOM_FRAME_LENGTH_FIVE, ucDisplayAniData );
		if( result != DRV_OK )
		{
			PrintError( "[DDI_MICOM_SetDisplayAni] MICOM_TX_Bytes Error \n" );
				REL_MICOM_SEMA;
				return DRV_ERR;
		}

		result=VK_MSG_ReceiveTimeout(MessageId,ucDisplayAniData,MICOM_FRAME_LENGTH_FOUR,(unsigned long)MICOM_TIMEOUT);
		REL_MICOM_SEMA;
		if(result == DRV_OK) {
			if(ucDisplayAniData[MICOM_COMMAND] == MICOM_FRAME_CMD_SET_DIS_ANI)
				return DRV_OK;
			else
				return DRV_ERR;
		} else {
			return DRV_ERR;
		}
}

DRV_Error DRV_MICOM_SetLCDPower(HBOOL bOn_nOff)
{
	unsigned char 	ucLCDPowerData[DRV_MICOM_MESSAGE_BYTES];
		int				result = DRV_OK;

		PrintEnter();

		ucLCDPowerData[MICOM_HEADER] = MICOM_FRAME_HEADER;
		ucLCDPowerData[MICOM_LENGTH] = MICOM_FRAME_LENGTH_TWO;
		ucLCDPowerData[MICOM_COMMAND] = MICOM_FRAME_CMD_SET_LCD_PWR;
		ucLCDPowerData[MICOM_DATA_ONE] = (unsigned char)bOn_nOff;
		MakeCheckSum(ucLCDPowerData,CHECK_NUM_FOUR);

		GET_MICOM_SEMA;
		result = DRV_MICOM_TX( MICOM_FRAME_LENGTH_FIVE, ucLCDPowerData );
		if( result != DRV_OK )
		{
			PrintError( "[%s] DRV_MICOM_TX Error \n", __func__ );
				REL_MICOM_SEMA;
				return DRV_ERR;
		}

		result=VK_MSG_ReceiveTimeout(MessageId,ucLCDPowerData,MICOM_FRAME_LENGTH_FOUR,(unsigned long)MICOM_TIMEOUT);
		REL_MICOM_SEMA;
		if(result == DRV_OK) {
			if(ucLCDPowerData[MICOM_COMMAND] == MICOM_FRAME_CMD_SET_LCD_PWR)
				return DRV_OK;
			else
				return DRV_ERR;
		} else {
			return DRV_ERR;
		}

}

DRV_Error DRV_MICOM_SetLcdDimmingLevel(unsigned char ucLcdDimmingLevel)
{
	unsigned char 	ucLCDPowerData[DRV_MICOM_MESSAGE_BYTES];
		int				result = DRV_OK;

		PrintEnter();

		ucLCDPowerData[MICOM_HEADER] = MICOM_FRAME_HEADER;
		ucLCDPowerData[MICOM_LENGTH] = MICOM_FRAME_LENGTH_TWO;
		ucLCDPowerData[MICOM_COMMAND] = MICOM_FRAME_CMD_SET_LCD_DIMMING_LEVEL;
		ucLCDPowerData[MICOM_DATA_ONE] = ucLcdDimmingLevel;
		MakeCheckSum(ucLCDPowerData,CHECK_NUM_FOUR);

		GET_MICOM_SEMA;
		result = DRV_MICOM_TX( MICOM_FRAME_LENGTH_FIVE, ucLCDPowerData );
		if( result != DRV_OK )
		{
			PrintError( "[%s] DRV_MICOM_TX Error \n", __func__ );
				REL_MICOM_SEMA;
				return DRV_ERR;
		}

		result=VK_MSG_ReceiveTimeout(MessageId,ucLCDPowerData,MICOM_FRAME_LENGTH_FOUR,(unsigned long)MICOM_TIMEOUT);
		REL_MICOM_SEMA;
		if(result == DRV_OK) {
			if(ucLCDPowerData[MICOM_COMMAND] == MICOM_FRAME_CMD_SET_LCD_DIMMING_LEVEL)
				return DRV_OK;
			else
				return DRV_ERR;
		} else {
			return DRV_ERR;
		}

}

DRV_Error DRV_MICOM_SetLEDDimmingLevel(unsigned char ucLcdDimmingLevel)
{
	unsigned char 	ucLCDPowerData[DRV_MICOM_MESSAGE_BYTES];
		int				result = DRV_OK;

		PrintEnter();

		ucLCDPowerData[MICOM_HEADER] = MICOM_FRAME_HEADER;
		ucLCDPowerData[MICOM_LENGTH] = MICOM_FRAME_LENGTH_TWO;
		ucLCDPowerData[MICOM_COMMAND] = MICOM_FRAME_CMD_SET_LED_DIMMING_LEVEL;
		ucLCDPowerData[MICOM_DATA_ONE] = ucLcdDimmingLevel;
		MakeCheckSum(ucLCDPowerData,CHECK_NUM_FOUR);

		GET_MICOM_SEMA;
		result = DRV_MICOM_TX( MICOM_FRAME_LENGTH_FIVE, ucLCDPowerData );
		if( result != DRV_OK )
		{
			PrintError( "[%s] DRV_MICOM_TX Error \n", __func__ );
				REL_MICOM_SEMA;
				return DRV_ERR;
		}

		result=VK_MSG_ReceiveTimeout(MessageId,ucLCDPowerData,MICOM_FRAME_LENGTH_FOUR,(unsigned long)MICOM_TIMEOUT);
		REL_MICOM_SEMA;
		if(result == DRV_OK) {
			if(ucLCDPowerData[MICOM_COMMAND] == MICOM_FRAME_CMD_SET_LED_DIMMING_LEVEL)
				return DRV_OK;
			else
				return DRV_ERR;
		} else {
			return DRV_ERR;
		}

}

DRV_Error DRV_MICOM_SetCustomCode(unsigned short usCCandCCB)	/*CustomCode and CustomCodeBar*/
{
	unsigned char 	ucCustomCodeData[DRV_MICOM_MESSAGE_BYTES];
		int				result = DRV_OK;

		PrintEnter();

		ucCustomCodeData[MICOM_HEADER] = MICOM_FRAME_HEADER;
		ucCustomCodeData[MICOM_LENGTH] = MICOM_FRAME_LENGTH_THREE;
		ucCustomCodeData[MICOM_COMMAND] = MICOM_FRAME_CMD_SET_CUSTOMCODE;
#if	0
		ucCustomCodeData[MICOM_DATA_ONE] = (unsigned char)(usCCandCCB & 0x00FF);
		ucCustomCodeData[MICOM_DATA_TWO] = (unsigned char)((usCCandCCB & 0xFF00) >> 8);
#else
		/* HUMAX Custom Code´Â ½ÇÁ¦ RCU¿¡¼­ ¿À´Â IR Signal°ú ¹Ý´ë·Î µÇ¾î ÀÖ¾î */
		/* ½ÇÁ¦ IR SignalÀ» ÀÎ½ÄÇÏ±â À§ÇØ¼­´Â ¾Æ·¡¿Í °°ÀÌ inverse¸¦ ÇØÁÖ¾î¾ß ÇÑ´Ù.  */
		ucCustomCodeData[MICOM_DATA_ONE] = (unsigned char)((usCCandCCB & 0xFF00) >> 8);
		ucCustomCodeData[MICOM_DATA_TWO] = (unsigned char)(usCCandCCB & 0x00FF);
#endif
		MakeCheckSum(ucCustomCodeData,CHECK_NUM_FIVE);

		GET_MICOM_SEMA;
		result = DRV_MICOM_TX( MICOM_FRAME_LENGTH_SIX, ucCustomCodeData );
		if( result != DRV_OK )
		{
			PrintError( "[MICOM_CUSTOM_CODE_SET] MICOM_TX_Bytes Error \n" );
				REL_MICOM_SEMA;
				return DRV_ERR;
		}

		result=VK_MSG_ReceiveTimeout(MessageId,ucCustomCodeData,MICOM_FRAME_LENGTH_FOUR,(unsigned long)MICOM_TIMEOUT);
		REL_MICOM_SEMA;
		if(result == DRV_OK) {
			if(ucCustomCodeData[MICOM_COMMAND] == MICOM_FRAME_CMD_SET_CUSTOMCODE)
				return DRV_OK;
			else
				return DRV_ERR;
		} else {
			return DRV_ERR;
		}
}
/********************************************************************
Function   : DRV_MICOM_SetUnitTest
Description : This APi is testing micom functions
Input      : none
Output     : none
Return	 : DDI_MICOM_OK

 ********************************************************************/
DRV_Error DRV_MICOM_SetUnitTest(unsigned char ucFuncNum, HBOOL bOn_nOff)
{
	unsigned char 	ucUnitTestData[DRV_MICOM_MESSAGE_BYTES];
		int				result = DRV_OK;

		PrintEnter();

		ucUnitTestData[MICOM_HEADER] = MICOM_FRAME_HEADER;
		ucUnitTestData[MICOM_LENGTH] = MICOM_FRAME_LENGTH_THREE;
		ucUnitTestData[MICOM_COMMAND] = MICOM_FRAME_CMD_SET_UNIT_TEST;
		ucUnitTestData[MICOM_DATA_ONE] = ucFuncNum;
		ucUnitTestData[MICOM_DATA_TWO] = (unsigned char)bOn_nOff;
		MakeCheckSum(ucUnitTestData,CHECK_NUM_FIVE);

		GET_MICOM_SEMA;
		result = DRV_MICOM_TX( MICOM_FRAME_LENGTH_SIX, ucUnitTestData );
		if( result != DRV_OK )
		{
			PrintError( "[DDI_MICOM_SetUnitTest] MICOM_TX_Bytes Error \n" );
				REL_MICOM_SEMA;
				return DRV_ERR;
		}

		result=VK_MSG_ReceiveTimeout(MessageId,ucUnitTestData,MICOM_FRAME_LENGTH_FOUR,(unsigned long)MICOM_TIMEOUT);
		REL_MICOM_SEMA;
		if(result == DRV_OK) {
			if(ucUnitTestData[MICOM_COMMAND] == MICOM_FRAME_CMD_SET_UNIT_TEST)
				return DRV_OK;
			else
				return DRV_ERR;
		} else {
			return DRV_ERR;
		}
}

/********************************************************************
  Function   : DRV_MICOM_SetWakeupRcuKey
  Description :
  Input      : none
  Output     : none
  Return     : DRV_OK
 ********************************************************************/
DRV_Error DRV_MICOM_SetWakeupRcuKey(DRV_MICOM_SET_WAKEUPKEY_TYPE nSetType, unsigned char *pucKey, unsigned char ucKeyNum)
{
	unsigned char ucWakeupkey[DRV_MICOM_MESSAGE_BYTES], i;
		DRV_Error result;

		PrintEnter();

		ucWakeupkey[MICOM_HEADER] = MICOM_FRAME_HEADER;
		ucWakeupkey[MICOM_LENGTH] = MICOM_FRAME_LENGTH_ONE+ucKeyNum+1;		// Typebyte + KeyNum
	ucWakeupkey[MICOM_COMMAND] = MICOM_FRAME_CMD_SET_WAKEUP_KEY;
		ucWakeupkey[MICOM_DATA_ONE] = nSetType;

		for(i=0; i<ucKeyNum; i++)
		{
			ucWakeupkey[MICOM_DATA_TWO+i] = pucKey[i];
		}

		PrintDebug("[DRV_MICOM_SetWakeupRcuKey] nSetType(%d), ucKeyNum(%d), ucWakeupkey[MICOM_LENGTH] = %d\n", nSetType, ucKeyNum, ucWakeupkey[MICOM_LENGTH]);

		MakeCheckSum(ucWakeupkey,CHECK_NUM_THREE+ucKeyNum+1);

		GET_MICOM_SEMA;
		result = DRV_MICOM_TX( ucWakeupkey[MICOM_LENGTH] + MICOM_FRAME_LENGTH_THREE, ucWakeupkey );
		if( result != DRV_OK )
		{
			PrintError( "[DRV_MICOM_WriteStrLcd] DRV_MICOM_TX Error \n" );
				REL_MICOM_SEMA;
				return DRV_ERR;
		}

		result=VK_MSG_ReceiveTimeout(MessageId,ucWakeupkey,MICOM_FRAME_LENGTH_FOUR,(unsigned long)MICOM_TIMEOUT);
		REL_MICOM_SEMA;
		if(result == DRV_OK) {
			if(ucWakeupkey[MICOM_COMMAND] == MICOM_FRAME_CMD_SET_WAKEUP_KEY) {
				return DRV_OK;
			} else {
				return DRV_ERR;
			}

		} else {
			return DRV_ERR;
		}
}

/********************************************************************
  Function   : DRV_MICOM_GetWakeupRcuKey
  Description :
  Input	   : none
  Output	   : none
  Return	   : DRV_OK
 ********************************************************************/
DRV_Error DRV_MICOM_GetWakeupRcuKey(unsigned char *pucRcuKeyCode)
{
	unsigned char	   ucWakeupData[DRV_MICOM_MESSAGE_BYTES];
		int 			   result=DRV_OK;

		PrintEnter();

		ucWakeupData[MICOM_HEADER] = MICOM_FRAME_HEADER;
		ucWakeupData[MICOM_LENGTH] = MICOM_FRAME_LENGTH_ONE;
		ucWakeupData[MICOM_COMMAND] = MICOM_FRAME_CMD_GET_WAKEUP_KEY;
		MakeCheckSum(ucWakeupData,CHECK_NUM_THREE);

		GET_MICOM_SEMA;
		result = DRV_MICOM_TX( MICOM_FRAME_LENGTH_FOUR, ucWakeupData );
		if(result != DRV_OK) {
			PrintError( "[DRV_MICOM_GetWakeupRcuKey] DRV_MICOM_TX Error \n" );
				REL_MICOM_SEMA;
				return DRV_ERR;
		}
	result=VK_MSG_ReceiveTimeout(MessageId,ucWakeupData, MICOM_FRAME_LENGTH_FIVE,(unsigned long)MICOM_TIMEOUT);
		REL_MICOM_SEMA;
		if(result == DRV_OK) {
			if (ucWakeupData[MICOM_COMMAND] == MICOM_FRAME_CMD_GET_WAKEUP_KEY) {
				*pucRcuKeyCode = ucWakeupData[MICOM_DATA_ONE];
					return DRV_OK;
			}
			else{
				return DRV_ERR;
			}
		}
		else{
			PrintError( "[DRV_MICOM_GetWakeupRcuKey] MSG ReceiveTimeout Error \n" );//tp.
			return DRV_ERR;
		}
}

DRV_Error DRV_MICOM_SetRCUInfo(HBOOL bOn_nOff)
{
	unsigned char 	ucLCDPowerData[DRV_MICOM_MESSAGE_BYTES];
		int				result = DRV_OK;

		PrintEnter();

		ucLCDPowerData[MICOM_HEADER] = MICOM_FRAME_HEADER;
		ucLCDPowerData[MICOM_LENGTH] = MICOM_FRAME_LENGTH_TWO;
		ucLCDPowerData[MICOM_COMMAND] = MICOM_FRAME_CMD_SET_RCUINFO;
		ucLCDPowerData[MICOM_DATA_ONE] = (unsigned char)bOn_nOff;
		MakeCheckSum(ucLCDPowerData,CHECK_NUM_FOUR);

		GET_MICOM_SEMA;
		result = DRV_MICOM_TX( MICOM_FRAME_LENGTH_FIVE, ucLCDPowerData );
		if( result != DRV_OK )
		{
			PrintError( "[%s] DRV_MICOM_TX Error \n", __func__ );
				REL_MICOM_SEMA;
				return DRV_ERR;
		}

		result=VK_MSG_ReceiveTimeout(MessageId,ucLCDPowerData,MICOM_FRAME_LENGTH_FOUR,(unsigned long)MICOM_TIMEOUT);
		REL_MICOM_SEMA;
		if(result == DRV_OK) {
			if(ucLCDPowerData[MICOM_COMMAND] == MICOM_FRAME_CMD_SET_RCUINFO)
				return DRV_OK;
			else
				return DRV_ERR;
		} else {
			return DRV_ERR;
		}

}

#if defined(CONFIG_OS_UCOS)
#else
static DRV_Error DRV_MICOM_ParseCommand(unsigned char* buf)
{
	DRV_Error result;
		HUINT8 frame = 0;

		/* Get Header */
		result = DRV_MICOM_RX(MICOM_FRAME_LENGTH_ONE, &frame);
		if(result != DRV_OK)
		{
			PrintError("DRV_MICOM_RX(Receiving 1st Byte) Error\r\n");
				VK_TASK_Sleep(100);
				return DRV_ERR;
		}

		/* Check Header */
		if((frame != MICOM_FRAME_REC_HEADER) &&
				(frame != MICOM_FRAME_HEADER))
		{
			/*
			  PrintError("Header is not valid\r\n");
			 */
			VK_memset( buf, 0x0, DRV_MICOM_MESSAGE_BYTES);
				return DRV_ERR;
		}

		/* Save */
		PrintDebug("[01;36m\nMICOM_GET_HEADER : %s\n[00m",frame == 6 ? "ACK" : "ENQ");
		buf[MICOM_HEADER] = frame;

		/* Get Length */
		result=DRV_MICOM_RX(MICOM_FRAME_LENGTH_ONE, &frame);
		if(result != DRV_OK)
		{
			PrintError("DRV_MICOM_RX(Receiving 2nd Byte) Error\r\n");
				VK_TASK_Sleep(100);
				return DRV_ERR;
		}

		/* Check */
		if (frame > DRV_MICOM_MAX_LENGTH)
		{
			PrintError("Length is Over\r\n");
				VK_TASK_Sleep(100);
				return DRV_ERR;
		}

		/* Save */
		buf[MICOM_LENGTH] = frame;
		PrintDebug("MICOM_GET_LENGTH 0x%x\n", frame);

		/* Get Rest of Data */
		/* Receiving length plus one more byte for checksum */
		result = DRV_MICOM_RX(frame + 1, &buf[MICOM_COMMAND]);
		if(result != DRV_OK)
		{
			PrintError("DRV_MICOM_RX(Receiving Rest of Data) Error\r\n");
				VK_memset(buf, 0x0, DRV_MICOM_MESSAGE_BYTES);
				VK_TASK_Sleep(100);
				return DRV_ERR;
		}

		PrintDebug("[01;36m\nMICOM_GET_COMMAND(%x)\n[00m", buf[2]);

		/* Check Validity of Received Data */
		/* length plus two more byte for header byte and command byte */
		result = FrameCheckSum(buf, buf[MICOM_LENGTH] + 2);
		if(result != DRV_OK)
		{
			PrintError("FrameCheckSum Error\r\n");
				VK_memset(buf, 0x0, DRV_MICOM_MESSAGE_BYTES);
				VK_TASK_Sleep(100);
				return DRV_ERR;
		}

		return DRV_OK;
}
#endif


/********************************************************************
  Function   : DRV_MICOM_EventTask
  Description :
  Input      : void *p(argument)
  Output     : none
  Return     : void
 ********************************************************************/
#if defined(CONFIG_OS_UCOS)
#else
static void DRV_MICOM_EventTask( void *p )
{
	unsigned long multi_key;
	unsigned char buf[DRV_MICOM_MESSAGE_BYTES]={0,};
	DRV_Error result;

	BSTD_UNUSED(p);

	while(1)
	{
#if defined (CONFIG_MICOM_UPGRADE)
		if(upgradeStatus == 1)
		{
			VK_TASK_Sleep(100);
			continue;
		}
#endif
		result = DRV_MICOM_ParseCommand(buf);
		if(result != DRV_OK)
		{
			continue;
		}

		switch(buf[2])
		{
		case MICOM_FRAME_CMD_SENDRCUINFO:
			 PrintError("\n MICOM_FRAME_CMD_SENDRCUINFO :  KeyCode(%d): %s\n", buf[3], s_asKeyCodeTable[buf[3]].aucKeyName);
			 break;
		case MICOM_FRAME_CMD_SET_MULTI_KEYINFO:
			multi_key = (buf[3]<<24) | (buf[4]<<16) | (buf[5]<<8) | (buf[6]);
			PrintDebug("\n MICOM_FRAME_CMD_SET_MULTI_KEYINFO :  KeyID(0x%x), KeyType(%d)\n", multi_key, buf[7]);
			DRV_KEY_SendMultiKeyMessage(multi_key, buf[7]);
			break;
		case MICOM_FRAME_CMD_SENDKEYINFO:
			/*
			  configure\##CHIP-##MODEL_REVx.x (ex. configure\7335-icordhd2_rev0.0) ¾È¿¡
			  board_cfg.c file ÀÇ const DI_KEY_FRONT s_BoardFrontKeyMapping[] ¸¦
			  Board Front silk ¹× micom ¿¡ ¸Â°Ô ¼öÁ¤ÇØÁÖ¾î¾ß ÇÑ´Ù.
			  °¢°¢ÀÇ Front key ¿¡ ´ëÇØ micom ¿¡¼­ ¾î¶² KeyId ¸¦ ¿Ã·ÁÁÖ´ÂÁö,
			  ±×¸®°í ±×°ÍÀ» DI_KEY_FRONT ÀÇ ¾î¶² enum À¸·Î mapping ÇØ¾ß ÇÏ´ÂÁö È®ÀÎÇÏÀÚ.
			*/
			PrintDebug("\n MICOM_FRAME_CMD_SENDKEYINFO :  KeyID(%d), KeyType(%d)\n", buf[3], buf[4]);
#if defined(CONFIG_FRONT_MICOM_TOUCH) && defined(CONFIG_PRODUCT_IMAGE_FACTORY)
			DI_UART_Print("\n TOUCH : KeyID(%d), Delta(%d)\n", buf[3], buf[5]);
#endif
			DRV_KEY_SendFrontKeyMessage(buf[3], buf[4]);
			break;
		case MICOM_FRAME_CMD_GET_VER:
			PrintDebug("MICOM_FRAME_CMD_GET_VER_MESSAGE\n");
			VK_MSG_Send(MessageId, buf, MICOM_FRAME_LENGTH_SIX);
			break;
		case MICOM_FRAME_CMD_SET_CUSTOMCODE:
			PrintDebug("MICOM_FRAME_CMD_SET_CUSTOMCODE_MESSAGE\n");
			VK_MSG_Send(MessageId, buf, MICOM_FRAME_LENGTH_FOUR);
			break;
		case MICOM_FRAME_CMD_SET_TIME:
			PrintDebug("MICOM_FRAME_CMD_SET_TIME_MESSAGE\n");
			VK_MSG_Send(MessageId, buf, MICOM_FRAME_LENGTH_FOUR);
			break;
		case MICOM_FRAME_CMD_GET_TIME:
			PrintDebug("MICOM_FRAME_CMD_GET_TIME_MESSAGE\n");
			VK_MSG_Send(MessageId, buf, MICOM_FRAME_LENGTH_EIGHT);
			break;
		case MICOM_FRAME_CMD_SET_WTIME:
			PrintDebug("MICOM_FRAME_CMD_SET_WTIME_MESSAGE\n");
			VK_MSG_Send(MessageId, buf, MICOM_FRAME_LENGTH_FOUR);
			break;
		case MICOM_FRAME_CMD_SET_WAKEUP:
			PrintDebug("MICOM_FRAME_CMD_SET_WAKEUP_MESSAGE\n");
			VK_MSG_Send(MessageId, buf, MICOM_FRAME_LENGTH_FOUR);
			break;
		case MICOM_FRAME_CMD_GET_WAKEUPSRC:
			PrintDebug("MICOM_FRAME_CMD_GET_WAKEUPSRC_MESSAGE\n");
			VK_MSG_Send(MessageId, buf, MICOM_FRAME_LENGTH_FIVE);
			break;
		case MICOM_FRAME_CMD_GET_POWERSTATE:
			PrintDebug("MICOM_FRAME_CMD_GET_POWERSTATE\n");
			VK_MSG_Send(MessageId, buf,MICOM_FRAME_LENGTH_FIVE);
			break;
		case MICOM_FRAME_CMD_WATCHDOG:
			PrintDebug("MICOM_FRAME_CMD_WATCHDOG\n");
			VK_MSG_Send(MessageId, buf, MICOM_FRAME_LENGTH_FOUR);
			break;
		case MICOM_FRAME_CMD_GET_WAKEUPTIME:
			PrintDebug("MICOM_FRAME_CMD_GET_WATCHDOG_MESSAGE\n");
			VK_MSG_Send(MessageId, buf,MICOM_FRAME_LENGTH_NINE);
			break;
		case MICOM_FRAME_CMD_SET_LED:
			PrintDebug("MICOM_FRAME_CMD_SET_SETLED\n");
			VK_MSG_Send(MessageId, buf,MICOM_FRAME_LENGTH_FIVE);
			break;
		case MICOM_FRAME_CMD_DISPLAY_7SEG:
			PrintDebug("MICOM_FRAME_CMD_DISPLAY_7SEG\n");
			VK_MSG_Send(MessageId, buf,MICOM_FRAME_LENGTH_FOUR);
			break;
		case MICOM_FRAME_CMD_SET_ICON:
			PrintDebug("MICOM_FRAME_CMD_SET_ICON\n");
			VK_MSG_Send(MessageId, buf,MICOM_FRAME_LENGTH_FOUR);
			break;
		case MICOM_FRAME_CMD_WRITE_STR:
			PrintDebug("MICOM_FRAME_CMD_WRITE_STR\n");
			VK_MSG_Send(MessageId, buf,MICOM_FRAME_LENGTH_FOUR);
			break;
		case MICOM_FRAME_CMD_SET_DISPLAY_TIME:
			PrintDebug("MICOM_FRAME_CMD_SET_DISPLAY_TIME\n");
			VK_MSG_Send(MessageId, buf,MICOM_FRAME_LENGTH_FIVE);
			break;
		case MICOM_FRAME_CMD_SET_PWR_SAV:
			PrintDebug("MICOM_FRAME_CMD_SET_PWR_SAV\n");
			VK_MSG_Send(MessageId, buf,MICOM_FRAME_LENGTH_FOUR);
			break;
		case MICOM_FRAME_CMD_SET_DIS_ANI:
			PrintDebug("MICOM_FRAME_CMD_SET_DIS_ANI\n");
			VK_MSG_Send(MessageId, buf,MICOM_FRAME_LENGTH_FOUR);
			break;
		case MICOM_FRAME_CMD_SET_LCD_PWR:
			PrintDebug("MICOM_FRAME_CMD_SET_LCD_PWR\n");
			VK_MSG_Send(MessageId, buf,MICOM_FRAME_LENGTH_FOUR);
			break;
		case MICOM_FRAME_CMD_SET_WAKEUP_KEY:
			PrintDebug("MICOM_FRAME_CMD_SET_WAKEUP_KEY\n");
			VK_MSG_Send(MessageId, buf,MICOM_FRAME_LENGTH_FOUR);
			break;
		case MICOM_FRAME_CMD_GET_WAKEUP_KEY:
			PrintDebug("MICOM_FRAME_CMD_GET_WAKEUP_KEY\n");
			VK_MSG_Send(MessageId, buf,MICOM_FRAME_LENGTH_FIVE);
			break;
		case MICOM_FRAME_CMD_SET_LCD_DIMMING_LEVEL:
			PrintDebug("MICOM_FRAME_CMD_SET_LCD_DIMMING_LEVEL\n");
			VK_MSG_Send(MessageId, buf,MICOM_FRAME_LENGTH_FOUR);
			break;
		case MICOM_FRAME_CMD_SET_LED_DIMMING_LEVEL:
			PrintDebug("MICOM_FRAME_CMD_SET_LED_DIMMING_LEVEL\n");
			VK_MSG_Send(MessageId, buf,MICOM_FRAME_LENGTH_FOUR);
			break;
		case MICOM_FRAME_CMD_SET_UNIT_TEST:
			PrintDebug("MICOM_FRAME_CMD_SET_UNIT_TEST\n");
			VK_MSG_Send(MessageId, buf,MICOM_FRAME_LENGTH_FOUR);
			break;
		case MICOM_FRAME_CMD_SET_RCUINFO:
			PrintDebug("MICOM_FRAME_CMD_SET_RCUINFO\n");
			VK_MSG_Send(MessageId, buf,MICOM_FRAME_LENGTH_FOUR);
			break;
		case MICOM_FRAME_CMD_SET_RESCUE_RESET:
			PrintDebug("MICOM_FRAME_CMD_SET_RESCUE_RESET\n");
			VK_MSG_Send(MessageId, buf, MICOM_FRAME_LENGTH_FOUR);
			break;
		case MICOM_FRAME_CMD_GET_STRVER:
			PrintDebug("MICOM_FRAME_CMD_GET_STRVER\n");
			VK_MSG_Send(MessageId, buf, 14);
			break;
		case MICOM_FRAME_CMD_SET_WOL:
			PrintDebug("MICOM_FRAME_CMD_SET_WOL\n");
			VK_MSG_Send(MessageId, buf, MICOM_FRAME_LENGTH_FOUR);
			break;
		case MICOM_FRAME_CMD_SET_FAST_BOOT:
			PrintDebug("MICOM_FRAME_CMD_SET_FAST_BOOT\n");
			VK_MSG_Send(MessageId, buf, MICOM_FRAME_LENGTH_FOUR);
			break;
		case MICOM_FRAME_CMD_POWER_REBOOT:
			PrintDebug("MICOM_FRAME_CMD_POWER_REBOOT\n");
			VK_MSG_Send(MessageId, buf, MICOM_FRAME_LENGTH_FOUR);
			break;
		case MICOM_FRAME_CMD_SET_STANDBY:
			PrintDebug("MICOM_FRAME_CMD_SET_STANDBY\n");
			VK_MSG_Send(MessageId, buf, MICOM_FRAME_LENGTH_FOUR);
			break;
#if defined(CONFIG_RF4CE_MICOM)
		case MICOM_FRAME_CMD_SET_RF4CE_PAIRING:
			PrintDebug("MICOM_FRAME_CMD_SET_RF4CE_PAIRING\n");
			VK_MSG_Send(MessageId, buf, MICOM_FRAME_LENGTH_FOUR);
			break;
		case MICOM_FRAME_CMD_GET_RF4CE_PAIRED_DEVS:
			PrintDebug("MICOM_FRAME_CMD_GET_RF4CE_PAIRED_DEVS\n");
			VK_MSG_Send(MessageId, buf, MICOM_FRAME_LENGTH_FIVE);
			break;
		case MICOM_FRAME_CMD_GET_RF4CE_PAIRED_DEV_IEEE_ADDR:
			PrintDebug("MICOM_FRAME_CMD_GET_RF4CE_PAIRED_DEV_IEEE_ADDR\n");
			VK_MSG_Send(MessageId, buf, 19);
			break;
		case MICOM_FRAME_CMD_GET_RF4CE_NWK_INFO:
			PrintDebug("MICOM_FRAME_CMD_GET_RF4CE_NWK_INFO\n");
			VK_MSG_Send(MessageId, buf, 16);
			break;
		case MICOM_FRAME_CMD_SET_RF4CE_IEEE_ADDR:
			PrintDebug("MICOM_FRAME_CMD_SET_RF4CE_IEEE_ADDR\n");
			VK_MSG_Send(MessageId, buf, MICOM_FRAME_LENGTH_FOUR);
			break;
		case MICOM_FRAME_CMD_GET_RF4CE_IEEE_ADDR:
			PrintDebug("MICOM_FRAME_CMD_GET_RF4CE_IEEE_ADDR\n");
			VK_MSG_Send(MessageId, buf, MICOM_FRAME_LENGTH_TWELVE);
			break;
		case MICOM_FRAME_CMD_SET_RF4CE_FACTORY_MODE:
			PrintDebug("MICOM_FRAME_CMD_SET_RF4CE_FACTORY_MODE\n");
			VK_MSG_Send(MessageId, buf, MICOM_FRAME_LENGTH_FOUR);
			break;
		case MICOM_FRAME_CMD_SET_RF4CE_CERTI_INOUT_MODE:
			PrintDebug("MICOM_FRAME_CMD_SET_RF4CE_CERTI_INOUT_MODE\n");
			VK_MSG_Send(MessageId, buf, MICOM_FRAME_LENGTH_FOUR);
			break;
		case MICOM_FRAME_CMD_SET_RF4CE_CERTI_CHANGE_FREQ:
			PrintDebug("MICOM_FRAME_CMD_SET_RF4CE_CERTI_CHANGE_FREQ\n");
			VK_MSG_Send(MessageId, buf, MICOM_FRAME_LENGTH_FOUR);
			break;
		case MICOM_FRAME_CMD_TRANS_RF4CE_ZRC_USER_DATA:
			PrintDebug("MICOM_FRAME_CMD_TRANS_RF4CE_ZRC_USER_DATA\n");
			VK_MSG_Send(MessageId, buf, MICOM_FRAME_LENGTH_FIVE);
			break;
#endif

#if defined(CONFIG_FRONT_MICOM_TOUCH)
		case MICOM_FRAME_CMD_GET_TOUCH_KEYINFO:
			PrintDebug("MICOM_FRAME_CMD_GET_TOUCH_KEYINFO\n");
			VK_MSG_Send(MessageId, buf, MICOM_MESSAGE_BYTES);
			break;
		case MICOM_FRAME_CMD_GET_TOUCH_KEYSIGNAL:
			PrintDebug("MICOM_FRAME_CMD_GET_TOUCH_KEYSIGNAL\n");
			VK_MSG_Send(MessageId, buf, MICOM_MESSAGE_BYTES);
			break;
		case MICOM_FRAME_CMD_GET_TOUCH_KEYREFERENCE:
			PrintDebug("MICOM_FRAME_CMD_GET_TOUCH_KEYREFERENCE\n");
			VK_MSG_Send(MessageId, buf, MICOM_MESSAGE_BYTES);
			break;
		case MICOM_FRAME_CMD_SET_TOUCH_SENSITIVITY:
			PrintDebug("MICOM_FRAME_CMD_SET_TOUCH_SENSITIVITY\n");
			VK_MSG_Send(MessageId, buf, MICOM_FRAME_LENGTH_FOUR);
			break;
		case MICOM_FRAME_CMD_GET_TOUCH_SENSITIVITY:
			PrintDebug("MICOM_FRAME_CMD_GET_TOUCH_SENSITIVITY\n");
			VK_MSG_Send(MessageId, buf, MICOM_MESSAGE_BYTES);
			break;
#endif

#if defined(CONFIG_MICOM_ALIVE_TASK_ENABLE)
		case MICOM_FRAME_CMD_SET_SYSTEM_MONITOR:
			PrintDebug("MICOM_FRAME_CMD_SET_MONITOR\n");
			VK_MSG_Send(MessageId, buf, MICOM_FRAME_LENGTH_FOUR);
			break;
		case MICOM_FRAME_CMD_SET_APP_READY:
			PrintDebug("MICOM_FRAME_CMD_SET_APP_READY\n");
			VK_MSG_Send(MessageId, buf, MICOM_FRAME_LENGTH_FOUR);
			break;
#endif
		default:
			PrintError("Unknown command(0x%x) received !\n\n", buf[2]);
			break;
		}
		VK_memset( buf, 0x0, sizeof(buf));
		VK_TASK_Sleep(100);
	}
}
#endif

#if defined(CONFIG_MICOM_ALIVE_TASK_ENABLE)
/********************************************************************
  Function   : DRV_MICOM_Alive_Task
  Description : this task toggle GPIO and MICOM detects it. HIGH(1sec) and LOW(1sec)
  Input      : void *p(argument)
  Output     : none
  Return     : void
 ********************************************************************/
static void DRV_MICOM_Alive_Task( void *p )
{
	BSTD_UNUSED(p);

		unsigned char ucToggleCnt=0;

		PrintDebug("[DRV_MICOM_Alive_Task] GStarts~~~\n");

		while(1)
		{
			if(s_bIsMicomAliveTaskEnabled == TRUE)
			{
				ucToggleCnt = ucToggleCnt % 2;
					if (ucToggleCnt)
					{
						PrintDebug("[DRV_MICOM_Alive_Task] GPIO	LOW!!\n");
							DRV_GPIO_Write(GPIO_ID_ALIVE, GPIO_LOW);
					}
					else
					{
						PrintDebug("[DRV_MICOM_Alive_Task] GPIO	HIGH!!\n");
							DRV_GPIO_Write(GPIO_ID_ALIVE, GPIO_HI);
					}
				ucToggleCnt++;
			}

				VK_TASK_Sleep(MICOM_MONITOR_SEND_TIME*1000); /* Toggle Duration is 1Sec */
		}
}

/********************************************************************
  Function   : DRV_MICOM_SetMicomAliveTask
  Description :
  Input      :
  Output     :
  Return     :
 ********************************************************************/
DRV_Error DRV_MICOM_SetMicomAliveTask(HBOOL bEnable)
{
	s_bIsMicomAliveTaskEnabled = bEnable;
		return DRV_OK;
}

/********************************************************************
  Function   : DRV_MICOM_SetMicomAliveCheck
  Description :
  Input      :
  Output     :
  Return     :
 ********************************************************************/
DRV_Error DRV_MICOM_SetMicomAliveCheck(HBOOL bEnable)
{
	DRV_Error eDrvRet = DRV_OK;

		if(bEnable == TRUE)
		{
			eDrvRet = DRV_MICOM_SetSysMonitoring(TRUE, MICOM_MONITOR_SEND_TIME*5);
				if(eDrvRet != DRV_OK)
				{
					PrintError("DRV_MICOM_SetSysMonitoring err! ret=%d\n\n", eDrvRet);
				}
		}
		else
		{
			eDrvRet = DRV_MICOM_SetSysMonitoring(FALSE, 0);
				if(eDrvRet != DRV_OK)
				{
					PrintError("DRV_MICOM_SetSysMonitoring err! ret=%d\n\n", eDrvRet);
				}
		}

		return eDrvRet;
}
#endif

/********************************************************************
  Function   : MakeCheckSum
  Description :
  Input      : pCheckSum, int CheckNum
  Output     : pCheckSum
  Return     : void
 ********************************************************************/
void MakeCheckSum(unsigned char *pCheckSum, int CheckNum)
{
	int i=0;
		pCheckSum[CheckNum]=0;
		for(i=1;i<CheckNum;i++){
			pCheckSum[CheckNum]+=pCheckSum[i];
		}
}

/********************************************************************
  Function   : FrameCheckSum
  Description :
  Input      : pbuffer, buffer_cnt
  Output     : pbuffer
  Return     : void
 ********************************************************************/
DRV_Error FrameCheckSum(unsigned char *pbuffer, int buffer_cnt)
{

		int i = 0;
		unsigned char checksum=0;

		for(i=1;i<buffer_cnt;i++) {
			checksum += pbuffer[i];
		}

		if(checksum == pbuffer[buffer_cnt])
			return DRV_OK;
		else
			return DRV_ERR;
}

#if defined(CONFIG_OS_UCOS)
/********************************************************************
Function	 : DRV_uMICOM_FlushUart
Description :
Input 	 : none
Output	 : none
Return	 : DRV_OK
********************************************************************/
static DRV_Error   DRV_uMICOM_FlushUart(void)
{
	HUINT8 ch;

	while(BSU_UART1_RX_inpstate())
	{
		ch = BSU_UART1_GetCh();
	}
	return DRV_OK;
}


/********************************************************************
Function	 : DRV_uMICOM_PARSE_Command
Description :
Input 	 : none
Output	 : none
Return	 : DRV_OK
********************************************************************/
static DRV_Error DRV_uMICOM_PARSE_Command(unsigned char* buf)
{
	DRV_Error result;
	HUINT8 frame=0, complete_flag = 0;

	result = DRV_uMICOM_GetMessage(MICOM_FRAME_LENGTH_ONE, &frame, 10);
	if (result != DRV_OK)
	{
		return DRV_ERR;
	}

	switch(frame)
	{
	case MICOM_FRAME_REC_HEADER:
	case MICOM_FRAME_HEADER:
		break;

	default:
		PrintError("[01;36m\nMICOM_GET_KEYDATA 0x%x\n[00m",frame);
		return DRV_ERR;
	}
	PrintDebug("[01;36m\nMICOM_GET_HEADER : %s\n[00m",frame == 6 ? "ACK" : "ENQ");

	buf[MICOM_HEADER] = frame;

	result=DRV_MICOM_RX(MICOM_FRAME_LENGTH_ONE, &frame);
	if(result != DRV_OK)
	{
		PrintError("[MICOM_KEY_Parse_Command] Error RX_Byte2.\r\n");
		return DRV_ERR;
	}

	if (frame > DRV_MICOM_MAX_LENGTH)
	{
		PrintError("Error uMicom data length.\n");
		return DRV_ERR;
	}

	buf[MICOM_LENGTH] = frame;
	result = DRV_MICOM_RX(frame+MICOM_FRAME_LENGTH_ONE, &buf[MICOM_COMMAND]);
	if(result != DRV_OK)
	{
		PrintError("[MICOM_KEY_Parse_Command] Error RX_Byte3.\r\n");
		return DRV_ERR;
	}
	complete_flag=1;

	result=FrameCheckSum(buf,buf[MICOM_LENGTH] + MICOM_FRAME_LENGTH_TWO);
	if(result!=DRV_OK){
		PrintError("FrameCheckSum Fail\n");
		return DRV_ERR;
	}

	return DRV_OK;
}

/********************************************************************
 Function   : DRV_uMICOM_Key_Task
 Description :
 Input      : void *p(argument)
 Output     : none
 Return     : void
********************************************************************/
static void DRV_uMICOM_Key_Task( void *p )
{
	HUINT8 buf[DRV_MICOM_MESSAGE_BYTES]={0,};
	unsigned long multi_key;
	DRV_Error result;

	BSTD_UNUSED(p);

	DI_UART_Print("[DRV_uMICOM_Key_Task] Starts~~~\n");

	VK_TASK_Sleep(1);

	VK_MEM_Memset( buf, 0x0, DRV_MICOM_MESSAGE_BYTES);

	while(s_isRunMicomTask)
	{
		result = DRV_uMICOM_PARSE_Command(buf);
		if(result != DRV_OK)
		{
			VK_TASK_Sleep(100);
			continue;
		}
		else
		{
			switch(buf[MICOM_FRAME_LENGTH_TWO])
			{
				case MICOM_FRAME_CMD_SENDRCUINFO:
					 PrintError("\n MICOM_FRAME_CMD_SENDRCUINFO :  KeyCode(%d): %s\n", buf[3], s_asKeyCodeTable[buf[3]].aucKeyName);
					 break;
				case MICOM_FRAME_CMD_SET_MULTI_KEYINFO:
					multi_key = (buf[MICOM_DATA_ONE]<<8) | (buf[MICOM_DATA_TWO]);
					PrintDebug("\n uMicom multi key info:  KeyID(%d), KeyType(%d)\n", multi_key, buf[MICOM_DATA_THREE]);
					DRV_KEY_SendMultiKeyMessage(multi_key, buf[MICOM_DATA_THREE]);
					break;
				case MICOM_FRAME_CMD_SENDKEYINFO:
					PrintDebug("\n uMicom key info:  KeyID(%d), KeyType(%d)\n", buf[MICOM_DATA_ONE], buf[MICOM_DATA_TWO]);
					DRV_KEY_SendFrontKeyMessage(buf[MICOM_DATA_ONE], buf[MICOM_DATA_TWO]);
					break;
				case MICOM_FRAME_CMD_GET_VER:
					PrintDebug("MICOM_FRAME_CMD_GET_VER_MESSAGE\n");
					VK_MSG_Send(MessageId, buf, MICOM_FRAME_LENGTH_SIX);
					break;
				case MICOM_FRAME_CMD_SET_CUSTOMCODE:
					PrintDebug("MICOM_FRAME_CMD_SET_CUSTOMCODE_MESSAGE\n");
					VK_MSG_Send(MessageId, buf, MICOM_FRAME_LENGTH_FOUR);
					break;
				case MICOM_FRAME_CMD_SET_TIME:
					PrintDebug("MICOM_FRAME_CMD_SET_TIME_MESSAGE\n");
					VK_MSG_Send(MessageId, buf, MICOM_FRAME_LENGTH_FOUR);
					break;
				case MICOM_FRAME_CMD_GET_TIME:
					PrintDebug("MICOM_FRAME_CMD_GET_TIME_MESSAGE\n");
					VK_MSG_Send(MessageId, buf, MICOM_FRAME_LENGTH_EIGHT);
					break;
				case MICOM_FRAME_CMD_SET_WTIME:
					PrintDebug("MICOM_FRAME_CMD_SET_WTIME_MESSAGE\n");
					VK_MSG_Send(MessageId, buf, MICOM_FRAME_LENGTH_FOUR);
					break;
				case MICOM_FRAME_CMD_SET_WAKEUP:
					PrintDebug("MICOM_FRAME_CMD_SET_WAKEUP_MESSAGE\n");
					VK_MSG_Send(MessageId, buf, MICOM_FRAME_LENGTH_FOUR);
					break;
				case MICOM_FRAME_CMD_GET_WAKEUPSRC:
					PrintDebug("MICOM_FRAME_CMD_GET_WAKEUPSRC_MESSAGE\n");
					VK_MSG_Send(MessageId, buf, MICOM_FRAME_LENGTH_FIVE);
					break;
				case MICOM_FRAME_CMD_GET_POWERSTATE:
					PrintDebug("MICOM_FRAME_CMD_GET_POWERSTATE\n");
					VK_MSG_Send(MessageId, buf,MICOM_FRAME_LENGTH_FIVE);
					break;
				case MICOM_FRAME_CMD_WATCHDOG:
					PrintDebug("MICOM_FRAME_CMD_WATCHDOG\n");
					VK_MSG_Send(MessageId, buf, MICOM_FRAME_LENGTH_FOUR);
					break;
				case MICOM_FRAME_CMD_GET_WAKEUPTIME:
					PrintDebug("MICOM_FRAME_CMD_GET_WATCHDOG_MESSAGE\n");
					VK_MSG_Send(MessageId, buf,MICOM_FRAME_LENGTH_NINE);
					break;
				case MICOM_FRAME_CMD_SET_LED:
					PrintDebug("MICOM_FRAME_CMD_SET_SETLED\n");
					VK_MSG_Send(MessageId, buf,MICOM_FRAME_LENGTH_FIVE);
					break;
				case MICOM_FRAME_CMD_DISPLAY_7SEG:
					PrintDebug("MICOM_FRAME_CMD_DISPLAY_7SEG\n");
					VK_MSG_Send(MessageId, buf,MICOM_FRAME_LENGTH_FOUR);
					break;
				case MICOM_FRAME_CMD_SET_ICON:
					PrintDebug("MICOM_FRAME_CMD_SET_ICON\n");
					VK_MSG_Send(MessageId, buf,MICOM_FRAME_LENGTH_FOUR);
					break;
				case MICOM_FRAME_CMD_WRITE_STR:
					PrintDebug("MICOM_FRAME_CMD_WRITE_STR\n");
					VK_MSG_Send(MessageId, buf,MICOM_FRAME_LENGTH_FOUR);
					break;
				case MICOM_FRAME_CMD_SET_DISPLAY_TIME:
					PrintDebug("MICOM_FRAME_CMD_SET_DISPLAY_TIME\n");
					VK_MSG_Send(MessageId, buf,MICOM_FRAME_LENGTH_FIVE);
					break;
				case MICOM_FRAME_CMD_SET_PWR_SAV:
					PrintDebug("MICOM_FRAME_CMD_SET_PWR_SAV\n");
					VK_MSG_Send(MessageId, buf,MICOM_FRAME_LENGTH_FOUR);
					break;
				case MICOM_FRAME_CMD_SET_DIS_ANI:
					PrintDebug("MICOM_FRAME_CMD_SET_DIS_ANI\n");
					VK_MSG_Send(MessageId, buf,MICOM_FRAME_LENGTH_FOUR);
					break;
				case MICOM_FRAME_CMD_SET_LCD_PWR:
					PrintDebug("MICOM_FRAME_CMD_SET_LCD_PWR\n");
					VK_MSG_Send(MessageId, buf,MICOM_FRAME_LENGTH_FOUR);
					break;
				case MICOM_FRAME_CMD_SET_WAKEUP_KEY:
					PrintDebug("MICOM_FRAME_CMD_SET_WAKEUP_KEY\n");
					VK_MSG_Send(MessageId, buf,MICOM_FRAME_LENGTH_FOUR);
					break;
				case MICOM_FRAME_CMD_GET_WAKEUP_KEY:
					PrintDebug("MICOM_FRAME_CMD_GET_WAKEUP_KEY\n");
					VK_MSG_Send(MessageId, buf,MICOM_FRAME_LENGTH_FIVE);
					break;
				case MICOM_FRAME_CMD_SET_LCD_DIMMING_LEVEL:
					PrintDebug("MICOM_FRAME_CMD_SET_LCD_DIMMING_LEVEL\n");
					VK_MSG_Send(MessageId, buf,MICOM_FRAME_LENGTH_FOUR);
					break;
				case MICOM_FRAME_CMD_SET_LED_DIMMING_LEVEL:
					PrintDebug("MICOM_FRAME_CMD_SET_LED_DIMMING_LEVEL\n");
					VK_MSG_Send(MessageId, buf,MICOM_FRAME_LENGTH_FOUR);
					break;
				case MICOM_FRAME_CMD_SET_UNIT_TEST:
					PrintDebug("MICOM_FRAME_CMD_SET_UNIT_TEST\n");
					VK_MSG_Send(MessageId, buf,MICOM_FRAME_LENGTH_FOUR);
					break;
				case MICOM_FRAME_CMD_SET_RCUINFO:
					PrintDebug("MICOM_FRAME_CMD_SET_RCUINFO\n");
					VK_MSG_Send(MessageId, buf,MICOM_FRAME_LENGTH_FOUR);
					break;
				case MICOM_FRAME_CMD_SET_RESCUE_RESET:
					PrintDebug("MICOM_FRAME_CMD_SET_RESCUE_RESET\n");
					VK_MSG_Send(MessageId, buf, MICOM_FRAME_LENGTH_FOUR);
					break;
				case MICOM_FRAME_CMD_GET_STRVER:
					PrintDebug("MICOM_FRAME_CMD_GET_STRVER\n");
					VK_MSG_Send(MessageId, buf, 14);
					break;
				case MICOM_FRAME_CMD_SET_WOL:
					PrintDebug("MICOM_FRAME_CMD_SET_WOL\n");
					VK_MSG_Send(MessageId, buf, MICOM_FRAME_LENGTH_FOUR);
					break;
				case MICOM_FRAME_CMD_SET_FAST_BOOT:
					PrintDebug("MICOM_FRAME_CMD_SET_FAST_BOOT\n");
					VK_MSG_Send(MessageId, buf, MICOM_FRAME_LENGTH_FOUR);
					break;
				case MICOM_FRAME_CMD_POWER_REBOOT:
					PrintDebug("MICOM_FRAME_CMD_POWER_REBOOT\n");
					VK_MSG_Send(MessageId, buf, MICOM_FRAME_LENGTH_FOUR);
					break;
				case MICOM_FRAME_CMD_SET_STANDBY:
					PrintDebug("MICOM_FRAME_CMD_SET_STANDBY\n");
					VK_MSG_Send(MessageId, buf, MICOM_FRAME_LENGTH_FOUR);
					break;
#if defined(CONFIG_RF4CE_MICOM)
				case MICOM_FRAME_CMD_SET_RF4CE_PAIRING:
					PrintDebug("MICOM_FRAME_CMD_SET_RF4CE_PAIRING\n");
					VK_MSG_Send(MessageId, buf, MICOM_FRAME_LENGTH_FOUR);
					break;
				case MICOM_FRAME_CMD_GET_RF4CE_PAIRED_DEVS:
					PrintDebug("MICOM_FRAME_CMD_GET_RF4CE_PAIRED_DEVS\n");
					VK_MSG_Send(MessageId, buf, MICOM_FRAME_LENGTH_FIVE);
					break;
				case MICOM_FRAME_CMD_GET_RF4CE_PAIRED_DEV_IEEE_ADDR:
					PrintDebug("MICOM_FRAME_CMD_GET_RF4CE_PAIRED_DEV_IEEE_ADDR\n");
					VK_MSG_Send(MessageId, buf, 19);
					break;
				case MICOM_FRAME_CMD_GET_RF4CE_NWK_INFO:
					PrintDebug("MICOM_FRAME_CMD_GET_RF4CE_NWK_INFO\n");
					VK_MSG_Send(MessageId, buf, 16);
					break;
				case MICOM_FRAME_CMD_SET_RF4CE_IEEE_ADDR:
					PrintDebug("MICOM_FRAME_CMD_SET_RF4CE_IEEE_ADDR\n");
					VK_MSG_Send(MessageId, buf, MICOM_FRAME_LENGTH_FOUR);
					break;
				case MICOM_FRAME_CMD_GET_RF4CE_IEEE_ADDR:
					PrintDebug("MICOM_FRAME_CMD_GET_RF4CE_IEEE_ADDR\n");
					VK_MSG_Send(MessageId, buf, MICOM_FRAME_LENGTH_TWELVE);
					break;
				case MICOM_FRAME_CMD_SET_RF4CE_FACTORY_MODE:
					PrintDebug("MICOM_FRAME_CMD_SET_RF4CE_FACTORY_MODE\n");
					VK_MSG_Send(MessageId, buf, MICOM_FRAME_LENGTH_FOUR);
					break;
				case MICOM_FRAME_CMD_SET_RF4CE_CERTI_INOUT_MODE:
					PrintDebug("MICOM_FRAME_CMD_SET_RF4CE_CERTI_INOUT_MODE\n");
					VK_MSG_Send(MessageId, buf, MICOM_FRAME_LENGTH_FOUR);
					break;
				case MICOM_FRAME_CMD_SET_RF4CE_CERTI_CHANGE_FREQ:
					PrintDebug("MICOM_FRAME_CMD_SET_RF4CE_CERTI_CHANGE_FREQ\n");
					VK_MSG_Send(MessageId, buf, MICOM_FRAME_LENGTH_FOUR);
					break;
				case MICOM_FRAME_CMD_TRANS_RF4CE_ZRC_USER_DATA:
					PrintDebug("MICOM_FRAME_CMD_TRANS_RF4CE_ZRC_USER_DATA\n");
					VK_MSG_Send(MessageId, buf, MICOM_FRAME_LENGTH_FIVE);
					break;
#endif

#if defined(CONFIG_FRONT_MICOM_TOUCH)
				case MICOM_FRAME_CMD_GET_TOUCH_KEYINFO:
					PrintDebug("MICOM_FRAME_CMD_GET_TOUCH_KEYINFO\n");
					VK_MSG_Send(MessageId, buf, MICOM_MESSAGE_BYTES);
					break;
				case MICOM_FRAME_CMD_GET_TOUCH_KEYSIGNAL:
					PrintDebug("MICOM_FRAME_CMD_GET_TOUCH_KEYSIGNAL\n");
					VK_MSG_Send(MessageId, buf, MICOM_MESSAGE_BYTES);
					break;
				case MICOM_FRAME_CMD_GET_TOUCH_KEYREFERENCE:
					PrintDebug("MICOM_FRAME_CMD_GET_TOUCH_KEYREFERENCE\n");
					VK_MSG_Send(MessageId, buf, MICOM_MESSAGE_BYTES);
					break;
				case MICOM_FRAME_CMD_SET_TOUCH_SENSITIVITY:
					PrintDebug("MICOM_FRAME_CMD_SET_TOUCH_SENSITIVITY\n");
					VK_MSG_Send(MessageId, buf, MICOM_FRAME_LENGTH_FOUR);
					break;
				case MICOM_FRAME_CMD_GET_TOUCH_SENSITIVITY:
					PrintDebug("MICOM_FRAME_CMD_GET_TOUCH_SENSITIVITY\n");
					VK_MSG_Send(MessageId, buf, MICOM_MESSAGE_BYTES);
					break;
#endif

#if defined(CONFIG_MICOM_ALIVE_TASK_ENABLE)
				case MICOM_FRAME_CMD_SET_SYSTEM_MONITOR:
					PrintDebug("MICOM_FRAME_CMD_SET_MONITOR\n");
					VK_MSG_Send(MessageId, buf, MICOM_FRAME_LENGTH_FOUR);
					break;
				case MICOM_FRAME_CMD_SET_APP_READY:
					PrintDebug("MICOM_FRAME_CMD_SET_APP_READY\n");
					VK_MSG_Send(MessageId, buf, MICOM_FRAME_LENGTH_FOUR);
					break;
#endif
				default:
					PrintError("Unknown command(0x%x) received !\n\n", buf[2]);
					break;
			}
		}

		VK_MEM_Memset( buf, 0x0, DRV_MICOM_MESSAGE_BYTES);
		VK_TASK_Sleep(100);
	}
}


/********************************************************************
Function	 : DRV_uMICOM_GetMessage
Description :
Input 	 : none
Output	 : none
Return	 : DRV_OK
********************************************************************/
static DRV_Error DRV_uMICOM_GetMessage(int argRxNum, unsigned char *pDataBuffer, unsigned long timeout)
{
	HUINT32	ulTargetTick = 50;
	HUINT32 ulCurTick = VK_TIMER_GetSystemTick();
	HUINT8	*buf = pDataBuffer;
	DRV_Error result = DRV_OK;

	VK_MEM_Memset(buf, 0x00, argRxNum);

	ulTargetTick = ulCurTick + timeout;
	while(argRxNum > 0)
	{
		ulCurTick = VK_TIMER_GetSystemTick();
		if(ulCurTick > ulTargetTick)
		{
			break;
		}

		if(!BSU_UART1_RX_inpstate())
		{
			continue;
		}

		*buf++ = BSU_UART1_GetCh();
		argRxNum--;
	}

	if(argRxNum > 0)
	{
		return DRV_ERR;
	}

	if(argRxNum > (MICOM_LENGTH+MICOM_FRAME_LENGTH_TWO))
	{
		result=FrameCheckSum(pDataBuffer,pDataBuffer[MICOM_LENGTH] + MICOM_FRAME_LENGTH_TWO);
		if(result!=DRV_OK){
			PrintError("FrameCheckSum Fail\n");
		}
	}

	#if defined(CONFIG_DEBUG)
	if(result!=DRV_OK)
	{
		HINT32 i = 0;

		DI_UART_Print("error RecvData : [");
		for(i=0; i < 128; i++)
		{
			if(pDataBuffer[i] == 0 && i > 10)
			{
				break;
			}
			DI_UART_Print("0x%02X, ", pDataBuffer[i]);
		}
		DI_UART_Print("]\n");
	}
	#endif

	return result;
}
#endif	/* CONFIG_OS_UCOS */

#if defined(CONFIG_MICOM_UPGRADE)
DRV_Error  DRV_MICOM_GetReadyForDownload(void)
{
	int			 result	 = (int)DRV_OK;
	unsigned char	 ucaBuff[DRV_MICOM_MESSAGE_BYTES];


	PrintEnter();

	ucaBuff[MICOM_HEADER] = MICOM_FRAME_HEADER;
	ucaBuff[MICOM_LENGTH] = MICOM_FRAME_LENGTH_ONE;
	ucaBuff[MICOM_COMMAND] = MICOM_FRAME_CMD_GET_READY_FOR_DOWNLOAD;
	MakeCheckSum(ucaBuff,CHECK_NUM_THREE);

	result = DRV_MICOM_TX( MICOM_FRAME_LENGTH_FOUR, ucaBuff );
	if( result != DRV_OK )
	{
		PrintError( "[DRV_MICOM_GetReadyForDownload MICOM_TX_Bytes Error\n" );
		return DRV_ERR;
	}
	return DRV_OK;
}
#endif

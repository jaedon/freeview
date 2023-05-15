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
#include "di_power.h"
#include "di_rtc.h"
#include "di_front_panel.h"
#include "di_led.h"
#include "di_err.h"
#include "di_uart.h"
#include "drv_err.h"
#include "drv_micom.h"
#if defined(CONFIG_STLED)
#include "drv_stled.h"
#endif
#include "drv_fan.h"
#include "drv_smart.h"
#if !defined(CONFIG_USE_EXTERNAL_CI_CONTROLLER)
#include "drv_ci.h"
#else
#include "di_ci.h"
#include "drv_cimax.h"
#endif
#include "pdriver.h"
#include "drv_demux.h"
#include "di_demux.h"
#include "drv_rcu_type.h"
#include "di_key.h"
#include "di_system.h"
#include "di_scart.h"
#include "di_nvram.h"
#include "di_ethernet.h"
#include "di_hdd.h"
#include "drv_key.h"

#if defined(SUPPORT_IR_MULTI_CUSTOM_CODE)
#include "drv_remocon_multi_custom_code.h"
#endif
#include "bstd_defs.h"
#include "breg_mem.h"

#if (BCHP_CHIP == 7250 || BCHP_CHIP == 7439 || BCHP_CHIP == 7252) || ((NEXUS_VERSION >= 1502) && (BCHP_CHIP == 7241))
#else
#include "bchp_usb_ehci.h"
#endif
#if (CONFIG_USB_HOSTCONTROLLER_CNT > 1)
#if ((BCHP_CHIP == 7439) && (BCHP_VER == BCHP_VER_B0)) || ((BCHP_CHIP == 7252) && (BCHP_VER == BCHP_VER_B0)) || ((NEXUS_VERSION >= 1502) && (BCHP_CHIP == 7241))
#else
#include "bchp_usb1_ehci.h"
#endif
#endif

#if defined(CONFIG_PRODUCT_IMAGE_FACTORY)
#include "cmd_hwtest_parser_ci_ext.h"
#endif

#if defined(CONFIG_RECORD)
#include "cmd_hwtest_interface.h"
#endif

#if defined(BDBG_DEBUG_BUILD)
#include "linden_trace.h"
#endif

#include <fcntl.h>
#include <sys/ioctl.h>

#if defined(CONFIG_CAS_NA)
#if defined(CONFIG_CAS_NA_NOCS_3X)
#include "nocs_csd.h"
#include "nocs_csd_impl.h"
#else
#include "nocs1x_csd.h"
#include "nocs1x_csd_impl.h"
#endif
#include <stdlib.h>
#include <assert.h>
#include <unistd.h>
#include <errno.h>
#include <fcntl.h>
#include <sys/mman.h>
#endif

#if defined(CONFIG_BRCM_MOD_NEXUS_POWER_MANAGEMENT)
#include <signal.h>
#include "drv_pm.h"
#endif

#if defined(CONFIG_ETHERNET)
#include <netdb.h>
#endif

#if defined(CONFIG_TS_MATRIX)
extern DRV_Error DRV_SetTsMatrix(HUINT32 ulTs0_0, HUINT32 ulTs0_1, HUINT32 ulTs1_0, HUINT32 ulTs1_1, HUINT32 ulTs1_2, HUINT32 ulTs1_3);
#endif

/*******************************************************************/
/****************************** define *****************************/
/*******************************************************************/
#define TEST_EXT_DEBUG
#define TEST_EXT_ENTER_LEAVE_DEBUG

#ifdef TEST_EXT_DEBUG
#define PrintDebug			DI_UART_Print
#define PrintError			DI_UART_Print
#else
#define PrintDebug			while (0) ((int (*)(char *, ...)) 0)
#define PrintError			DI_UART_Print
#endif

#ifdef TEST_EXT_ENTER_LEAVE_DEBUG
#define PrintEnter				DI_UART_Print("(+)%s\n", __FUNCTION__)
#define PrintExit				DI_UART_Print("(-)%s\n", __FUNCTION__)
#else
#define PrintEnter				while (0) ((int (*)(char *, ...)) 0)
#define PrintExit				while (0) ((int (*)(char *, ...)) 0)
#endif

#define AssertDI(arg1, arg2) if(arg1 != DI_ERR_OK) {DI_UART_Print(\
					"ERROR code : 0x%x, At %s() function, FILE: %s, LINE: %d\n",arg1, __func__, __FILE__,__LINE__); return arg2;}

#define AssertDRV(arg1, arg2) if(arg1 != DRV_OK) {DI_UART_Print(\
					"ERROR code : 0x%x, At %s() function, FILE: %s, LINE: %d\n",arg1, __func__, __FILE__,__LINE__); return arg2;}


#define HWTEST_CMD		szCmd
#define HWTEST_PARAM	szParam
#define HWTEST_PARAM1	szParam1
#define HWTEST_PARAM2	szParam2
#define HWTEST_PARAM3	szParam3
#define HWTEST_PARAM4	szParam4
#define HWTEST_PARAM5	szParam5
#define HWTEST_PARAM6	szParam6
#define GET_ARGS		int iResult=CMD_ERR; \
						char *HWTEST_CMD=CMD_NextArg((char **)&szArgs); \
						char *HWTEST_PARAM=CMD_NextArg((char **)&szArgs); \
						char *HWTEST_PARAM1=CMD_NextArg((char **)&szArgs); \
						char *HWTEST_PARAM2=CMD_NextArg((char **)&szArgs); \
						char *HWTEST_PARAM3=CMD_NextArg((char **)&szArgs); \
						char *HWTEST_PARAM4=CMD_NextArg((char **)&szArgs); \
						char *HWTEST_PARAM5=CMD_NextArg((char **)&szArgs); \
						char *HWTEST_PARAM6=CMD_NextArg((char **)&szArgs);

#define CMD_IS(x)		(HWTEST_CMD!=NULL && VK_strcmp(HWTEST_CMD, x)==0)
#define PARAM_IS(x)		(HWTEST_PARAM!=NULL && VK_strcmp(HWTEST_PARAM, x)==0)
#define PARAM1_IS(x)	(HWTEST_PARAM1!=NULL && VK_strcmp(HWTEST_PARAM1, x)==0)

#define READABLE_IN_HEX(sz,val) CMD_ReadAsHex(sz, &val)
#define READABLE_IN_DEC(sz,val) CMD_ReadAsDec(sz, &val)
#define GET_NEXT_ARG(variable)		char *variable = CMD_NextArg((char **)&szArgs)

#define	IR_CUSTOM_CODE_RMF01 				(0x1000)
#define	IR_CUSTOM_CODE_RMF02 				(0x1700)

/* kernel driver 관련 임시 코드..   */
//#include "autoconf.h"
#define UNREFENCED_PARAM(x) (x=x)

#define CONFIG_BCM_KERNEL_MODE
#ifdef CONFIG_BCM_KERNEL_MODE
#define FS_REG_ACCESS	0
#define FS_MEM_ACCESS	1
#define FS_RD_MODE		0
#define FS_WR_MODE		1

unsigned long FSREG_Read(void *RegHandle, unsigned long ofs)
{
	UNREFENCED_PARAM(RegHandle);
	unsigned long rdValue;
#if defined(CONFIG_DIRECTFB_MASTER)
	NEXUS_CI_Read(FS_REG_ACCESS, ofs, &rdValue);
#else
	NEXUS_CI_Access(FS_REG_ACCESS, FS_RD_MODE, ofs, &rdValue);
#endif
	return rdValue;
}

void FSREG_Write(void *RegHandle, unsigned int ofs, unsigned int data)
{
	UNREFENCED_PARAM(RegHandle);
	VK_printf("[hee.usermode] write at 0x%u value=0x%u\n", ofs, data);
#if defined(CONFIG_DIRECTFB_MASTER)
	NEXUS_CI_Write(FS_REG_ACCESS, ofs, data);
#else
	NEXUS_CI_Access(FS_REG_ACCESS, FS_WR_MODE, ofs, data);
#endif
}

unsigned char FSMEM_Read(void *RegHandle, unsigned int ofs)
{
	UNREFENCED_PARAM(RegHandle);
	unsigned long rdValue;
#if defined(CONFIG_DIRECTFB_MASTER)
	NEXUS_CI_Read(FS_MEM_ACCESS, ofs, &rdValue);
#else
	NEXUS_CI_Access(FS_MEM_ACCESS, FS_RD_MODE, ofs, &rdValue);
#endif
	return (rdValue & 0x000000FF);
}

void FSMEM_Write(void *RegHandle, unsigned int ofs, unsigned char data)
{
	UNREFENCED_PARAM(RegHandle);
	unsigned long wrValue;
	wrValue = (unsigned long)data;
#if defined(CONFIG_DIRECTFB_MASTER)
	NEXUS_CI_Write(FS_MEM_ACCESS, ofs, wrValue);
#else
	NEXUS_CI_Access(FS_MEM_ACCESS, FS_WR_MODE, ofs, wrValue);
#endif
}
#else
#define FSREG_Read	BREG_Read32
#define FSREG_Write	BREG_Write32
#define FSMEM_Read	BREG_Read8
#define FSMEM_Write	BREG_Write8
#endif

#if	defined(CONFIG_CI)
#define MAX_CASYSID_CNT 100
typedef struct _camInfo
{
	unsigned char ucIsInitialized;
	unsigned char ModuleType;
	unsigned char ModuleManufacturer[41];
	unsigned char ucCaSysIdcount;
	unsigned short usCaSysId[MAX_CASYSID_CNT];
	unsigned char ucCaSysIdName[MAX_CASYSID_CNT][20];
}CAMINFO;
#endif

extern HBOOL	g_bFanDispInfo;

extern const RCU_INFO s_asKeyCodeTable[IR_NUMBER_OF_KEY];

#if defined(CONFIG_FRONT_MICOM_TOUCH) && defined(CONFIG_DEBUG)
extern HUINT32 g_LastKeyInfo;
extern HBOOL g_FrontTestFlag;
#endif

#if defined(CONFIG_USE_EXTERNAL_CI_CONTROLLER)
#if defined(CONFIG_DEBUG)
#define CIMAX_NUM_CHIP_INPUT_MAX 8
#define CIMAX_NUM_MODULE_INPUT_MAX	8
#define CIMAX_NUM_OUTPUT_CHANNEL_MAX 16
#define CIMAX_CH0_OUTPUT_MAX 4
#define CIMAX_CH1_OUTPUT_MAX 2

static char *CIMAX_ChipInput[CIMAX_NUM_CHIP_INPUT_MAX] = {
	"None\0",
	"TS_P\0",
	"USB_Endp\0",
	"Err\0",
	"TS_Live1\0",
	"TS_Live2\0",
	"TS_Pb1\0",
	"TS_PB2\0",
};

static char *CIMAX_ModuleInput[CIMAX_NUM_MODULE_INPUT_MAX] = {
	"None\0",
	"InCh_0\0",
	"InCh_1\0",
	"Remapper\0",
	"PH_Adder\0",
	"D-Chained\0",
	"GAP_RMV0\0",
	"GAP_RMV1\0",
};

static char *CIMAX_OutputChannel[CIMAX_NUM_OUTPUT_CHANNEL_MAX] = {
	"SameSlot\0",
	"InCh_0\0",
	"InCh_1\0",
	"Remapper\0",
	"Adder\0",
	"OtherSlot\0",
	"GAP_RMV0\0",
	"GAP_RMV1\0",
	"None\0",
	"OUT_Err\0",
	"OUT_Err\0",
	"OUT_Err\0",
	"OUT_Err\0",
	"OUT_Err\0",
	"OUT_Err\0",
	"OUT_Err\0",
};

static char *CIMAX_ChipOutput0[CIMAX_CH0_OUTPUT_MAX] = {
	"Disabled\0",
	"OutCh0_P\0",
	"OutCh0_S\0",
	"InCh0_S\0",
};

static char *CIMAX_ChipOutput1[CIMAX_CH1_OUTPUT_MAX] = {
	"Disabled\0",
	"OutCh1_S\0",
};
#endif
extern HINT32 CIMAX_WriteRegister(HUINT16, HUINT8 *, HUINT8);
extern HINT32 CIMAX_ReadRegister(HUINT16, HUINT8 *, HUINT8);
#endif

#if defined(CONFIG_RECORD)
#define SMART_BUF_SIZE 516
#define SMART_CMD (0x031F)

#define SMART_SHORT_TEST 1
#define SMART_LONG_TEST 2

static int curDstTest = 0;
static HBOOL bIsHddDstTesting = FALSE;
static int hddfd=-1;
unsigned long HddDstTimerId;
static pfnHI_HDD_Dst_NotifyFUNC sHddDstCallback = NULL;
#endif


#if defined(CONFIG_CAS_NA)
#if defined(CONFIG_RAM_ROOTFS)
#define NAGRA_PK_RAM_PHY_ADDR		(0x20000000)
#define NAGRA_PK_RAM_CACHED_ADDR	(0x60000000)
#else
#define NAGRA_PK_RAM_PHY_ADDR		(0x00A00000)
#define NAGRA_PK_RAM_CACHED_ADDR	(0x00A00000)
#endif
#define NAGRA_PK_SIZE				(0x3FC) /* 1020 bytes */
/*PK_LENGTH_SIZE(4) + STB_CA_SN(4) + PK(N) + PK_SIGN(128) + CRC(2)*/
#define PK_LENGTH_SIZE				(4)
#define STB_CA_SN_SIZE				(4)
#endif

static unsigned long ulFrontAgingTaskId = 0;
static HBOOL g_FrontAgingTestFlag = FALSE;

static unsigned long ulFrontStressTaskId = 0;
static HBOOL g_FrontStressTestFlag = FALSE;

#define MAX_USER_SEQUENCE 150
#define USER_AGING_CODE_FILE "/user_aging"
static unsigned long ulIrAgingTaskId = 0;
static HUINT32 s_ulKeyRepeatInterval = 0;
static HUINT32 s_ulKeyRepeatCode = 0;
static HBOOL s_bUserAgingStart = FALSE;
static HUINT32 s_ulNumberOfUserAgingSequence = 0;
static HUINT32 s_ulUserKeyTable[MAX_USER_SEQUENCE][2];
static HUINT32 s_ulTestCount = 0;

static int INT_HwTest_Front_Convert_Input_To_Unixtime(char *szDate, char *szTime, HUINT32 *pulUnixTime)
{
	unsigned short wMJD;
	char *sep;
	DRV_CLOCK_DATE_T date_t;
	DRV_DATE_TIME current_time;

	if (szDate!= NULL && szTime!=NULL)
	{
		if(VK_strlen(szDate) > 10){
			CMD_Printf("ERROR! Parameter Parsing : HWTEST_PARAM2(%s)\n", szDate);
			CMD_Printf("Right Date Type : YYYY/MM/DD(1999/09/09)\n");
			return CMD_ERR;
		}

		if(VK_strlen(szTime) > 8){
			CMD_Printf("ERROR! Parameter Parsing : HWTEST_PARAM3(%s)\n",szTime);
			CMD_Printf("Right Time Type : HH:MM:SS(03:03:09)\n");
			return CMD_ERR;
		}

		if(!(sep = strsep(&szDate, "/"))){
			CMD_Printf("ERROR! Please use seprator token as '/' \n");
			return CMD_ERR;
		}
		date_t.usYear = (unsigned short)(VK_atoi(sep));
		if(date_t.usYear > 2009){
			CMD_Printf("ERROR! Year %d is not available\n",date_t.usYear);
			return CMD_ERR;
		}

		if(!(sep = strsep(&szDate, "/"))){
			CMD_Printf("ERROR! Please use seprator token as '/' \n");
			return CMD_ERR;
		}
		date_t.ucMonth = (unsigned char)(VK_atoi(sep));
		if(date_t.ucMonth > 12){
			CMD_Printf("ERROR! Month %d is not available\n",date_t.ucMonth);
			return CMD_ERR;
		}

		date_t.ucDay = (unsigned char)(VK_atoi(szDate));
		if(date_t.ucMonth > 31){
			CMD_Printf("ERROR! Day %d is not available\n",date_t.ucDay);
			return CMD_ERR;
		}

		DRV_RTC_YMD2MJD(date_t, &wMJD);

		CMD_Printf("User Input : Local Date(%d/%d/%d), wMJD=%d\n", date_t.usYear, date_t.ucMonth, date_t.ucDay, wMJD);

		current_time.usMjd = wMJD;

		if(!(sep = strsep(&szTime, ":"))){
			CMD_Printf("ERROR!Please use seprator token as ':' \n");
			return CMD_ERR;
		}
		current_time.ucHour = (unsigned short)(VK_atoi(sep));
		if(current_time.ucHour > 23){
			CMD_Printf("ERROR!Hour %d is not available\n",current_time.ucHour);
			return CMD_ERR;
		}

		if(!(sep = strsep(&szTime, ":"))){
			CMD_Printf("ERROR!Please use seprator token as ':' \n");
			return CMD_ERR;
		}
		current_time.ucMin = (unsigned char)(VK_atoi(sep));
		if(current_time.ucMin > 59){
			CMD_Printf("ERROR! Minute %d is not available\n",current_time.ucMin);
			return CMD_ERR;
		}

		current_time.ucSec = date_t.ucDay = (unsigned char)(VK_atoi(szTime));
		if(current_time.ucSec > 59){
			CMD_Printf("ERROR! Second %d is not available\n",current_time.ucSec);
			return CMD_ERR;
		}

		CMD_Printf("User Input : Local Time(%d:%d:%d)\n", current_time.ucHour, current_time.ucMin, current_time.ucSec);

		DRV_RTC_ConvertDateTimeToUnixTime(current_time, (unsigned long *)pulUnixTime);

		CMD_Printf("User Input : Unix Time(%d)\n", *pulUnixTime);

	}
	else
	{
		CMD_Printf("ERROR! Parameter is NULL -> Right Date Type : YYYY/MM/DD(1999/09/09) HH:MM:SS(03:03:09)\n");
		return CMD_ERR;
	}

	return CMD_OK;
}

static void P_Front_AgingTask(void *p)
{
	int loop_count = 0;
	int iResult=CMD_ERR;
	char string[5];
	p = p;


	VK_memset(string, 0, sizeof(string));

	while(1)
	{
		if( g_FrontAgingTestFlag == TRUE )
		{
			switch( loop_count )
			{
				/* Display All ON */
				case 0 :
					iResult = DRV_MICOM_SetUnitTest(0, TRUE);
					break;

				/* Display All Off */
				case 1 :
					iResult = DRV_MICOM_SetUnitTest(0, FALSE);

				/* Display "xxxx" */
				default :
					string[0] = string[1] = string[2] = string[3] = '0' + ((loop_count)%10);
					DI_FRONT_PANEL_Display((HUINT8 *)string);
#if defined(CONFIG_STLED)
					(void)DRV_STLED_WriteString(string, 4, FALSE);
#endif

					break;
			}

			if( (++loop_count) > 10)
				loop_count = 0;

		}

		VK_TASK_Sleep(10000);
	}

}


static void P_Front_StressTask(void *p)
{
	int loop_count = 0;
	DI_ERR_CODE etDiRet = DI_ERR_OK;
	unsigned char aucMicomVersion[10];
	DI_POWER_REASON nPowrReason;
	HUINT8 szString[5];
	p = p;

	while(1)
	{
		if( g_FrontStressTestFlag == TRUE )
		{
			loop_count++;
			if(loop_count > 9999) loop_count = 0;
			etDiRet = DI_SYSTEM_GetSystemInfo(DI_SYS_MICOM_STRVER, aucMicomVersion, 10);
			if (etDiRet != DI_ERR_OK)
			{
				PrintError("Error(0x%x) In DI_SYSTEM_GetSystemInfo() \n", etDiRet);
				break;
			}

			DI_UART_Print("\033[01;34m\nMicomModel : %c%c%c%c%c%c\n\n\033[00m", aucMicomVersion[0],
				aucMicomVersion[1],aucMicomVersion[2],aucMicomVersion[3],aucMicomVersion[4],aucMicomVersion[5]);
			DI_UART_Print("\033[01;34m\nMicomType : %x\n\n\033[00m", aucMicomVersion[7]);
			DI_UART_Print("\033[01;34m\nMicomVersion : %x.%02x\n\n\033[00m", aucMicomVersion[8], aucMicomVersion[9]);

			etDiRet = DI_POWER_GetWakeupReason(&nPowrReason);
			if(etDiRet != DI_ERR_OK)
			{
				PrintError("Error(0x%x) In DI_POWER_GetWakeupReason() \n", etDiRet);
				break;
			}

			etDiRet = DI_FRONT_PANEL_SetDimmingLevel(loop_count % 0xff);
			if(etDiRet != DI_ERR_OK)
			{
				PrintError("Error(0x%x) In DI_FRONT_PANEL_SetDimmingLevel() \n", etDiRet);
				break;
			}

			etDiRet = DI_LED_SetOn(DI_LED_NUM0);
			if(etDiRet != DI_ERR_OK)
			{
				PrintError("Error(0x%x) In DI_LED_SetOn() \n", etDiRet);
				break;
			}

			VK_MEM_Memset(szString, 0, sizeof(szString));
			VK_sprintf(szString,"%04d", loop_count);

			DI_FRONT_PANEL_Display(szString);
		}
	}


	return;
}

static int P_Front_Test(HBOOL bOn_nOff)
{
	int i;
	DI_LED_ID	nled_id;
	DI_ERR_CODE	nErr = DI_ERR_OK;
	DRV_Error dResult = DRV_OK;
	HUINT32 ulLedCapa = 0;
	DI_FRONT_PANEL_CAP stPanelCapa;

	CMD_Printf("[%s] Front pannel & LED Auto Test %s\r\n", __func__, bOn_nOff?"ON":"OFF");

	dResult = DRV_MICOM_SetUnitTest(0, FALSE);
	AssertDRV(dResult, CMD_ERR);

	if (bOn_nOff==FALSE)
	{
		nErr = DI_LED_ClearAll();
		AssertDI(nErr, CMD_ERR);

		nErr = DI_FRONT_PANEL_Display((HUINT8 *)" ");
		AssertDI(nErr, CMD_ERR);

		return CMD_OK;
	}

	DI_LED_GetCapability(&ulLedCapa);

	DI_FRONT_PANEL_GetCapability(&stPanelCapa);

	CMD_Printf("[%s] LED Capability is  0x%8x\r\n", __func__, ulLedCapa);

	/* LED Test */
	if ( (stPanelCapa.type == DI_FRONT_PANEL_TYPE_7SEG) || (stPanelCapa.type == DI_FRONT_PANEL_TYPE_11SEG) )
		nErr = DI_FRONT_PANEL_Display((HUINT8 *)"LEDT");
	else
		nErr = DI_FRONT_PANEL_Display((HUINT8 *)"LED Test");
	AssertDI(nErr, CMD_ERR);
	VK_TASK_Sleep(1000);

	for (i=0; i<32; i++)
	{
		nled_id = 0;
		nled_id = 0x1<<i;

		if ( (ulLedCapa & nled_id) == nled_id)	// if supported LED
		{
			switch(nled_id)
			{
				case DI_LED_NUM0:
					DI_FRONT_PANEL_Display((HUINT8 *)"TV");
                    if ( CMD_GetMsgUI() != NULL )
                    {
					    (int *)(CMD_GetMsgUI())("Front LED : TV\n");
                    }
					break;
				case DI_LED_NUM1:
					DI_FRONT_PANEL_Display((HUINT8 *)"RADIO");
                    if ( CMD_GetMsgUI() != NULL )
                    {
					    (int *)(CMD_GetMsgUI())("Front LED : RADIO\n");
                    }
					break;
				case DI_LED_NUM2:
					DI_FRONT_PANEL_Display((HUINT8 *)"REC");
                    if ( CMD_GetMsgUI() != NULL )
                    {
					    (int *)(CMD_GetMsgUI())("Front LED : REC\n");
                    }
					break;
				case DI_LED_NUM3:
					DI_FRONT_PANEL_Display((HUINT8 *)"PLAY");
                    if ( CMD_GetMsgUI() != NULL )
                    {
					    (int *)(CMD_GetMsgUI())("Front LED : PLAY\n");
                    }
					break;
				case DI_LED_NUM4:
					DI_FRONT_PANEL_Display((HUINT8 *)"PAUSE");
                    if ( CMD_GetMsgUI() != NULL )
                    {
					    (int *)(CMD_GetMsgUI())("Front LED : PAUSE\n");
                    }
					break;
				case DI_LED_NUM5:
					DI_FRONT_PANEL_Display((HUINT8 *)"TIME");
                    if ( CMD_GetMsgUI() != NULL )
                    {
					    (int *)(CMD_GetMsgUI())("Front LED : TIME\n");
                    }
					break;
				case DI_LED_NUM6:
					DI_FRONT_PANEL_Display((HUINT8 *)"DTS");
                    if ( CMD_GetMsgUI() != NULL )
                    {
					    (int *)(CMD_GetMsgUI())("Front LED : DTS\n");
                    }
					break;
				case DI_LED_NUM9:
					DI_FRONT_PANEL_Display((HUINT8 *)"HD");
                    if ( CMD_GetMsgUI() != NULL )
                    {
					    (int *)(CMD_GetMsgUI())("Front LED : HD\n");
                    }
					break;
				case DI_LED_NUM10:
					DI_FRONT_PANEL_Display((HUINT8 *)"DOLBY");
                    if ( CMD_GetMsgUI() != NULL )
                    {
					    (int *)(CMD_GetMsgUI())("Front LED : DOLBY\n");
                    }
					break;
				case DI_LED_NUM11:
					DI_FRONT_PANEL_Display((HUINT8 *)"1080P");
                    if ( CMD_GetMsgUI() != NULL )
                    {
					    (int *)(CMD_GetMsgUI())("Front LED : 1080P\n");
                    }
					break;
				case DI_LED_NUM12:
					DI_FRONT_PANEL_Display((HUINT8 *)"720P");
                    if ( CMD_GetMsgUI() != NULL )
                    {
					    (int *)(CMD_GetMsgUI())("Front LED : 720P\n");
                    }
					break;
				case DI_LED_NUM13:
					DI_FRONT_PANEL_Display((HUINT8 *)"576P");
                    if ( CMD_GetMsgUI() != NULL )
                    {
					    (int *)(CMD_GetMsgUI())("Front LED : 576P\n");
                    }
					break;
				case DI_LED_NUM14:
					DI_FRONT_PANEL_Display((HUINT8 *)"480P");
                    if ( CMD_GetMsgUI() != NULL )
                    {
					    (int *)(CMD_GetMsgUI())("Front LED : 480P\n");
                    }
					break;
				case DI_LED_NUM15:
					DI_FRONT_PANEL_Display((HUINT8 *)"1080I");
                    if ( CMD_GetMsgUI() != NULL )
                    {
					    (int *)(CMD_GetMsgUI())("Front LED : 1080I\n");
                    }
					break;
				case DI_LED_NUM16:
					DI_FRONT_PANEL_Display((HUINT8 *)"480I");
                    if ( CMD_GetMsgUI() != NULL )
                    {
					    (int *)(CMD_GetMsgUI())("Front LED : 480I\n");
                    }
					break;
				case DI_LED_NUM17:
					DI_FRONT_PANEL_Display((HUINT8 *)"576I");
                    if ( CMD_GetMsgUI() != NULL )
                    {
					    (int *)(CMD_GetMsgUI())("Front LED : 576I\n");
                    }
					break;
				case DI_LED_NUM18:
					DI_FRONT_PANEL_Display((HUINT8 *)"MAIL");
                    if ( CMD_GetMsgUI() != NULL )
                    {
					    (int *)(CMD_GetMsgUI())("Front LED : MAIL\n");
                    }
					break;
				default :
					DI_FRONT_PANEL_Display((HUINT8 *)"NOT SUPPORT");
                    if ( CMD_GetMsgUI() != NULL )
                    {
					    (int *)(CMD_GetMsgUI())("Front LED : NOT Supported\n");
                    }
					break;
			}

			VK_TASK_Sleep(10);
			nErr = DI_LED_SetOn(nled_id);
			AssertDI(nErr, CMD_ERR);

			CMD_Printf("LED_ID(0x%x) i(%d) ON for 1 sec\r\n", nled_id, i);
			VK_TASK_Sleep(1000);

			nErr = DI_LED_SetOff(nled_id);
			AssertDI(nErr, CMD_ERR);

			CMD_Printf("LED_ID(0x%x) i(%d) OFF... \r\n\n", nled_id, i);
		}
	}

	/* Power LED Test */
	if ( (stPanelCapa.type == DI_FRONT_PANEL_TYPE_7SEG) || (stPanelCapa.type == DI_FRONT_PANEL_TYPE_11SEG) )
		nErr = DI_FRONT_PANEL_Display((HUINT8 *)"PLED");
	else
		nErr = DI_FRONT_PANEL_Display((HUINT8 *)"Power LED Test");

    if ( CMD_GetMsgUI() != NULL )
    {
        (int *)(CMD_GetMsgUI())("Power LED Test\n");
    }

	AssertDI(nErr, CMD_ERR);
	VK_TASK_Sleep(1000);

	if ( (stPanelCapa.type == DI_FRONT_PANEL_TYPE_7SEG) || (stPanelCapa.type == DI_FRONT_PANEL_TYPE_11SEG) )
		nErr = DI_FRONT_PANEL_Display((HUINT8 *)"STBY");
	else
		nErr = DI_FRONT_PANEL_Display((HUINT8 *)"Standby LED:10s");
	AssertDI(nErr, CMD_ERR);
	VK_TASK_Sleep(10);
	CMD_Printf("Front Power LED set RED (STANDBY)\r\n");
    if ( CMD_GetMsgUI() != NULL )
    {
	    (int *)(CMD_GetMsgUI())("Power LED Test : RED\n");
    }
	nErr = DI_LED_SetPowerLedState(DI_LED_STANDBY);
	AssertDI(nErr, CMD_ERR);
	VK_TASK_Sleep(10000);

	if ( (stPanelCapa.type == DI_FRONT_PANEL_TYPE_7SEG) || (stPanelCapa.type == DI_FRONT_PANEL_TYPE_11SEG) )
		nErr = DI_FRONT_PANEL_Display((HUINT8 *)"NORM");
	else
		nErr = DI_FRONT_PANEL_Display((HUINT8 *)"Normal LED:10s");
	AssertDI(nErr, CMD_ERR);
	VK_TASK_Sleep(10);
	CMD_Printf("Front Power LED set BLUE (WATCHING)\r\n");

    if ( CMD_GetMsgUI() != NULL )
    {
	    (int *)(CMD_GetMsgUI())("Power LED Test : BLUE\n");
    }
	nErr = DI_LED_SetPowerLedState(DI_LED_WATCHING);
	AssertDI(nErr, CMD_ERR);
	VK_TASK_Sleep(10000);

	/* UNIT Test */
	nErr = DI_FRONT_PANEL_Display((HUINT8 *)"UNIT Test");
    if ( CMD_GetMsgUI() != NULL )
    {
	    (int *)(CMD_GetMsgUI())("Unit Test\n");
    }

	AssertDI(nErr, CMD_ERR);
	VK_TASK_Sleep(1000);

	dResult = DRV_MICOM_SetUnitTest(0, TRUE);

    if ( CMD_GetMsgUI() != NULL )
    {
        (int *)(CMD_GetMsgUI())("Front Test is done\n");
    }
    if ( CMD_GetMsgUI() != NULL )
    {
	    (int *)(CMD_GetMsgUI())("Front Test is done\n");
    }

	AssertDRV(dResult, CMD_ERR);

	return CMD_OK;

}

extern void DRV_POWER_SetAgingTestMode(HBOOL bOn, HUINT32 ulOnTimer);

DRV_Error P_Key_AgingSquenceOpen(void)
{
	FILE* fp = NULL;

	char *pStr;
	char *pStrInterval;
	char strTemp[20];
	int i=0;

	HUINT32 keycode, interval;

	keycode = 0;
	interval = 0;
	s_ulNumberOfUserAgingSequence = 0;

	fp = fopen( USER_AGING_CODE_FILE, "rb" );
	if( NULL == fp )
	{
		fclose( fp );
		CMD_Printf("Reading User Aging Code file(%s) error\n", USER_AGING_CODE_FILE);
		return DRV_ERR;
	}

	while( !feof( fp ) )
    {
        pStr = fgets(strTemp, sizeof(strTemp), fp );
		if(pStr != NULL)
		{
			for(i=0; i<IR_NUMBER_OF_KEY;i++)
			{

	  			if(0== VK_strncasecmp(strTemp, s_asKeyCodeTable[i].aucKeyName, VK_strlen(s_asKeyCodeTable[i].aucKeyName)))
  				{
  					//CMD_Printf("[%d] %s \n", i, strTemp);
					break;
  				}
			}

			if(i < IR_NUMBER_OF_KEY)
			{
				keycode = s_asKeyCodeTable[i].ulKeyCode;
				pStrInterval =	strTemp + VK_strlen(s_asKeyCodeTable[i].aucKeyName) + 1;
				interval = VK_atoi(pStrInterval);
				if(interval == 0) interval = 4;

				s_ulUserKeyTable[s_ulNumberOfUserAgingSequence][0] = keycode;
				s_ulUserKeyTable[s_ulNumberOfUserAgingSequence++][1] = interval;

				//CMD_Printf("  Key Get From File [%d] code : %d, term : %d \n", s_ulNumberOfUserAgingSequence, keycode, interval);
			}
			else
			{
				CMD_Printf("  Cannot identify key : %s", strTemp);
			}
		}

		//CMD_Printf("Aiging Sequence Read : %d lines\n",s_ulNumberOfUserAgingSequence );
    }
    fclose( fp );

	return DRV_OK;
}



void P_Key_Repeat_Task(void)
{
	HUINT32 IntervalCount = 0;
	HUINT32 UserKeyCount = 0;
	HUINT32 ulKeyCode = 0;
	HUINT32 ulKeyInterval = 0;
	HBOOL	bJustWakeup = FALSE;

	HUINT8 cKeyName[IR_LENGTH_OF_KEY_NAME];

	while(1)
	{
		if(s_ulKeyRepeatInterval == 0 && s_bUserAgingStart == FALSE)
		{
			VK_TASK_Sleep(1000);
			continue;
		}

		#if defined(CONFIG_BRCM_MOD_NEXUS_POWER_MANAGEMENT)
		if(DRV_PM_GetStandbyStatus() == TRUE)
		{
			bJustWakeup = TRUE;
			continue;
		}
		#endif

		if(UserKeyCount > s_ulNumberOfUserAgingSequence) UserKeyCount = 0;

		if(s_bUserAgingStart == TRUE)
		{
			ulKeyInterval = s_ulUserKeyTable[UserKeyCount][1];
			ulKeyCode = s_ulUserKeyTable[UserKeyCount][0];
			UserKeyCount++;
		}
		else
		{
			ulKeyInterval = s_ulKeyRepeatInterval;
			ulKeyCode = s_ulKeyRepeatCode;
		}

		DRV_KEY_GetKeyName(ulKeyCode, cKeyName);

		CMD_Printf("\n\t\t @@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@\n");
		CMD_Printf("\t\t @@@@@ Key Repeat Test(%dth) (key : %s , code : 0x%x, Interval : %d) \n", s_ulTestCount++, cKeyName, ulKeyCode, ulKeyInterval);
		CMD_Printf("\t\t @@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@\n");




		if(ulKeyCode == DI_KEY_REMOCON_STANDBY)
		{
			if(bJustWakeup == FALSE)
			{
				DRV_POWER_SetAgingTestMode(TRUE, ulKeyInterval);
				Drv_KeyEmulate(ulKeyCode);
			}
			else
			{
				bJustWakeup = FALSE;
				CMD_Printf("Skip First Standby Key after on\n");
			}

		}
		else
		{
		Drv_KeyEmulate(ulKeyCode);
		}

		IntervalCount = 0;
		while(IntervalCount < ulKeyInterval)
		{
			VK_TASK_Sleep(1000);
			IntervalCount ++;
		}
	}
}



#if defined(CONFIG_BRCM_MOD_NEXUS_POWER_MANAGEMENT)
void Cmd_hwtest_enterS3mode(void)
{
#if defined(CONFIG_DI20)
	Pid_Info pidInfo;
	DRV_PM_STBY_Settings stbySettings;

	pidInfo.pidBitmask = PID_BITMASK_AUDIO_MAIN | PID_BITMASK_VIDEO | PID_BITMASK_PCR;

	stbySettings.bIR = TRUE;
	stbySettings.bFrontKey = FALSE;
	stbySettings.bWOL = FALSE;
	stbySettings.bTimer = FALSE;

	DI_AUDIO_Stop(0);
	DI_VIDEO_Stop(0);
	DI_DEMUX_StopPID(0,pidInfo.pidBitmask);
	DI_DEMUX_ReleasePath(0);
	//DRV_VIDEO_UnInit(0);
	VK_TASK_SendPMMsg(SIGUSR1);
#if defined(CONFIG_PLAYBACK)
	DRV_PVR_PLAY_Uninit();
#endif
#if defined(CONFIG_RECORD)
	DRV_PVR_REC_Uninit();
#endif
	//DRV_MEDIA_Uninit();
	//DRV_REMOCON_UnInit();
	VK_TASK_Sleep(1000);
	DRV_PM_StandbyStates3(0, stbySettings);
	MAIN_CH_ReInitChannel();
	VK_TASK_SendPMMsg(SIGCONT);
	P_AvStartOnly();
#endif
	return;
}

#endif
int CMD_HwTest_Parser_Front(void *szArgs)
{
	GET_ARGS;

	if CMD_IS("strver")
	{
		unsigned char StrVer[10];

		CMD_Printf("Command : front strver -> Display Front Micom Ver(String)\r\n");

		iResult = DI_SYSTEM_GetSystemInfo(DI_SYS_MICOM_STRVER, StrVer, 10);
		AssertDI(iResult, CMD_ERR);

		CMD_Printf("Micom Version(String) : %c%c%c%c%c%c\n", StrVer[0], StrVer[1], StrVer[2],
								StrVer[3], StrVer[4], StrVer[5]);
		CMD_Printf("Micom Version(Number) : %u.%u.%u\r\n", StrVer[7], StrVer[8], StrVer[9]);
	}
	else if CMD_IS("ver")
	{
		HUINT16	usMicomVersion = 0;

		CMD_Printf("Command : front ver -> Display Front Micom Ver\r\n");

		iResult = DI_SYSTEM_GetSystemInfo(DI_SYS_MICOM_VER, &usMicomVersion, sizeof(HUINT16));
		AssertDI(iResult, CMD_ERR);

		CMD_Printf("uiMicomVersion: (major.minor) %x.%02x\n", (usMicomVersion & 0xFF00) >> 8, usMicomVersion & 0x00FF);
	}
	else if CMD_IS("reboot")
	{
		CMD_Printf("Command : front reboot -> STB Reboot By Front Micom \r\n");

		CMD_Printf("NOW STB Rebooting~!!! 3.. 2.. 1.. \r\n\n\n");

		DI_NANDFLASH_UmountPartitions();

		iResult = DI_POWER_SetMainChipReset();
		AssertDI(iResult, CMD_ERR);
	}
#if defined(CONFIG_WATCHDOG)
	else if CMD_IS("watchdog")
	{
		if PARAM_IS("on")
		{
			CMD_Printf("Command : front watchdog on-> front micom watchdog enable\r\n");

			iResult = DI_WATCHDOG_OnOff(TRUE);
			AssertDI(iResult, CMD_ERR);
		}
		else if PARAM_IS("off")
		{
			CMD_Printf("Command : front watchdog off-> front micom watchdog disable\r\n");

			iResult = DI_WATCHDOG_OnOff(FALSE);
			AssertDI(iResult, CMD_ERR);
		}
		else if PARAM_IS("timeout")
		{
			HUINT32	ulWDTimeOut;

			if ( READABLE_IN_DEC(HWTEST_PARAM1, ulWDTimeOut) )
			{
				CMD_Printf("input value is %d (sec. not ms)\n", ulWDTimeOut);
			}
			else
			{
				CMD_Printf("input value is NULL. so fix to default value 5 sec\n");
				ulWDTimeOut = 5;
			}

			CMD_Printf("Command : front watchdog timeout %d-> Front micom watchdog timeout value (%d) \r\n", ulWDTimeOut, ulWDTimeOut);

			iResult = DI_WATCHDOG_SetTimeOut((HUINT16)ulWDTimeOut);
			AssertDI(iResult, CMD_ERR);
		}
		else if PARAM_IS("reset")
		{
			CMD_Printf("Command : front watchdog reset-> front micom watchdog reset : watchdog clear~!\r\n");

			iResult = DI_WATCHDOG_Reset();
			AssertDI(iResult, CMD_ERR);
		}
		else if PARAM_IS("period")
		{
			HUINT32 ulWatchdogPeriod = 1000;

			if ( READABLE_IN_DEC(HWTEST_PARAM1, ulWatchdogPeriod) )
			{
				CMD_Printf("input value is %d\n", ulWatchdogPeriod);
			}
			else
			{
				CMD_Printf("input value is NULL. so fix to default value 1000ms\n");
			}

			CMD_Printf("Command : front watchdog period-> front micom watchdog set reset period (%d) \r\n", ulWatchdogPeriod);
			iResult = DI_WATCHDOG_SetResetPeriod(ulWatchdogPeriod);
			AssertDI(iResult, CMD_ERR);
		}
		else
		{
			CMD_Printf("Wrong Command~! right command : front watchdog on/off/reset/timeout/period \r\n");
			return CMD_ERR;
		}
	}
#endif
#if defined(CONFIG_BRCM_MOD_NEXUS_POWER_MANAGEMENT)
#if defined(CONFIG_HDD)
	else if CMD_IS("hddoff")
	{
		int rc;
#if defined(CONFIG_FS)
		DI_FS_UnmountPartition(0,1,3);
#endif
		VK_TASK_Sleep(1000);
		rc = VK_SYSTEM_Command("echo 1 > /sys/block/sda/device/delete");
		if (rc != VK_OK) {
			PrintError("[%s] Error. can't excute command\n", __FUNCTION__);
		}
		VK_TASK_Sleep(5000);
		//HI_HDD_PowerControl(1, HI_HDD_PWR_OFF);
		DI_HDD_ControlPower(1, 0);
		iResult = CMD_OK;

	}
	else if CMD_IS("hddon")
	{
		DI_HDD_ControlPower(1, 1);
		iResult = CMD_OK;
	}
#endif
#if defined(CONFIG_DI20)
	else if CMD_IS("s3")
	{
		Pid_Info pidInfo;
		DRV_PM_STBY_Settings stbySettings;

		pidInfo.pidBitmask = PID_BITMASK_AUDIO_MAIN | PID_BITMASK_VIDEO | PID_BITMASK_PCR;

		stbySettings.bIR = TRUE;
		stbySettings.bFrontKey = FALSE;
		stbySettings.bWOL = FALSE;
		stbySettings.bTimer = FALSE;

		DI_AUDIO_Stop(0);
		DI_VIDEO_Stop(0);
		DI_DEMUX_StopPID(0,pidInfo.pidBitmask);
		DI_DEMUX_ReleasePath(0);
		//DRV_VIDEO_UnInit(0);
		VK_TASK_SendPMMsg(SIGUSR1);
#if defined(CONFIG_PLAYBACK)
		DRV_PVR_PLAY_Uninit();
#endif
#if defined(CONFIG_RECORD)
		DRV_PVR_REC_Uninit();
#endif
		//DRV_MEDIA_Uninit();
		//DRV_REMOCON_UnInit();
		VK_TASK_Sleep(1000);
		DRV_PM_StandbyStates3(0, stbySettings);
		//MAIN_CH_ReInitChannel();
		VK_TASK_SendPMMsg(SIGCONT);
		//P_AvStartOnly();
		iResult = CMD_OK;
	}
	else if CMD_IS("s2")
	{
		Pid_Info pidInfo;
		DRV_PM_STBY_Settings stbySettings;

		pidInfo.pidBitmask = PID_BITMASK_AUDIO_MAIN | PID_BITMASK_VIDEO | PID_BITMASK_PCR;

		stbySettings.bIR = TRUE;
		stbySettings.bFrontKey = FALSE;
		stbySettings.bWOL = FALSE;
		stbySettings.bTimer = FALSE;
		DI_AUDIO_Stop(0);
		DI_VIDEO_Stop(0);
		DI_DEMUX_StopPID(0,pidInfo.pidBitmask);
		DI_DEMUX_ReleasePath(0);
		//DRV_VIDEO_UnInit(0);
		VK_TASK_SendPMMsg(SIGUSR1);
#if defined(CONFIG_PLAYBACK)
		DRV_PVR_PLAY_Uninit();
#endif
#if defined(CONFIG_RECORD)
		DRV_PVR_REC_Uninit();
#endif
		//DRV_MEDIA_Uninit();
		//DRV_REMOCON_UnInit();
		VK_TASK_Sleep(1000);
		DRV_PM_StandbyStates2(stbySettings);
		//MAIN_CH_ReInitChannel();
		VK_TASK_SendPMMsg(SIGCONT);
		//P_AvStartOnly();
		iResult = CMD_OK;
	}
#endif


#endif
#if defined(CONFIG_MICOM_NONE)
	else if CMD_IS("poweron")
	{
		char led_data[4] = {0x7f, 0x7f, 0x7f, 0x7f};

		DI_LED_SetPowerLedState(DI_LED_WATCHING);
		DI_FRONT_PANEL_Display(led_data);

		CMD_Printf("[%s] Active standby Off (no micom)\n", __FUNCTION__);
		iResult = CMD_OK;
	}
#if defined(CONFIG_LED_GPIO_RGB_FULL_COLOR	)
	else if CMD_IS("poweron2")
	{
		DI_LED_SetPowerLedState(DI_LED_WATCHING_LEDLOW);

		CMD_Printf("[%s] Active standby Off2 (no micom)\n", __FUNCTION__);
		iResult = CMD_OK;
	}
#endif
	else if CMD_IS("poweroff")
	{
		DI_LED_ClearAll();
		DI_LED_SetPowerLedState(DI_LED_STANDBY);
		DI_FRONT_PANEL_SetTimeDisplay(TRUE);

		CMD_Printf("[%s] Active standby On (no micom)\n", __FUNCTION__);
		iResult = CMD_OK;
	}
	else if CMD_IS("poweroff2")
	{
		DI_LED_ClearAll();
		DI_LED_SetPowerLedState(DI_LED_STANDBY_LEDLOW);
		DI_FRONT_PANEL_SetTimeDisplay(TRUE);
		CMD_Printf("[%s] Active standby2 On (no micom)\n", __FUNCTION__);
		iResult = CMD_OK;
	}
#else
	else if CMD_IS("poweroff")
	{
		iResult= DI_KEY_SetWakeupRcuKey(DI_KEY_WAKEUP_KEY_TYPE_NUMBER, DI_KEY_REMOCON_UNKNOWN);
		AssertDI(iResult, CMD_ERR);

		CMD_Printf("Set Wakeup Key : DI_KEY_WAKEUP_KEY_TYPE_NUMBER\n");

		iResult= DI_KEY_SetWakeupRcuKey(DI_KEY_WAKEUP_KEY_TYPE_CH_UP_DOWN, DI_KEY_REMOCON_UNKNOWN);
		AssertDI(iResult, CMD_ERR);

		CMD_Printf("Set Wakeup Key : DI_KEY_WAKEUP_KEY_TYPE_CH_UP_DOWN\n");

		iResult= DI_KEY_SetWakeupRcuKey(DI_KEY_WAKEUP_KEY_TYPE_ARROW, DI_KEY_REMOCON_UNKNOWN);
		AssertDI(iResult, CMD_ERR);

		CMD_Printf("Set Wakeup Key : DI_KEY_WAKEUP_KEY_TYPE_ARROW\n");

		iResult= DI_KEY_SetWakeupRcuKey(DI_KEY_WAKEUP_KEY_TYPE_SINGLE, DI_KEY_REMOCON_MENU);
		AssertDI(iResult, CMD_ERR);

		CMD_Printf("Set Wakeup Key : Single - DI_KEY_REMOCON_MENU\n");

		iResult= DI_KEY_SetWakeupRcuKey(DI_KEY_WAKEUP_KEY_TYPE_SINGLE, DI_KEY_REMOCON_CHLIST);
		AssertDI(iResult, CMD_ERR);

		CMD_Printf("Set Wakeup Key : Single - DI_KEY_REMOCON_CHLIST\n");

		iResult= DI_LED_ClearAll();
		AssertDI(iResult, CMD_ERR);

		iResult= DI_LED_SetPowerLedState(DI_LED_STANDBY);

		AssertDI(iResult, CMD_ERR);

		CMD_Printf("All LED (or icon) Clear & POWER LED STANDBY\n");

		iResult= DI_FRONT_PANEL_Display((HUINT8 *)" ");
		AssertDI(iResult, CMD_ERR);

		if(g_FrontAgingTestFlag == TRUE)
		{
			g_FrontAgingTestFlag = FALSE;
			CMD_Printf("7Segment Stress test OFF - Aging OFF\r\n");
		}

		if(g_FrontStressTestFlag == TRUE)
		{
			g_FrontStressTestFlag = FALSE;
			CMD_Printf("Front Stress test OFF - Stress OFF\r\n");
		}

		CMD_Printf("Front Display Clear~!\n");

		/*
		Power Saving in Standby -> Disable
			MAIN_ON 신호(Form Micom): Low
			UNDER_1W 신호(Form Micom): High
			STV6417: Auto startup mode로 진입
			Front LCD: ON
			LED_POWER: OFF
			LED_STBY: ON

		Power Saving in Standby -> Enable
			MAIN_ON 신호(Form Micom): Low
			UNDER_1W 신호(Form Micom): High
			STV6417: Auto startup mode로 진입
			Front LCD: OFF
			LED_POWER: OFF
			LED_STBY: ON
		*/

		if PARAM_IS("save")
		{
			CMD_Printf("Command : front poweroff save-> STB POWER OFF with power saviing on\n");

			CMD_Printf("POWER Saving ON :: Front LCD Power OFF~\n");

			iResult= DI_FRONT_PANEL_SetPanelPower(FALSE);
			AssertDI(iResult, CMD_ERR);

			VK_TASK_Sleep(300);
			DI_NANDFLASH_UmountPartitions();
			iResult = DI_POWER_SetPowerOff(DI_POWER_OFFMODE_STANDBY_POWERSAVING);
			AssertDI(iResult, CMD_ERR);
		}
		else if PARAM_IS("under")
		{
			CMD_Printf("Command : front poweroff under-> STB POWER OFF with Micom PowerSaving GPIO OFF control\n");

			CMD_Printf("POWER Saving & UNDER_1W Control ON :: Front LCD Power OFF~\n");

			iResult= DI_FRONT_PANEL_SetPanelPower(FALSE);
			AssertDI(iResult, CMD_ERR);

			VK_TASK_Sleep(300);

			DI_NANDFLASH_UmountPartitions();

			iResult = DI_POWER_SetPowerOff(DI_POWER_OFFMODE_STANDBY_POWERSAVING);
			AssertDI(iResult, CMD_ERR);
		}
		else
		{
			iResult= DI_FRONT_PANEL_SetTimeDisplay(TRUE);
			AssertDI(iResult, CMD_ERR);

			CMD_Printf("Front Time Display On\n");

			CMD_Printf("Command : front poweroff normal(standby)-> STB POWER OFF By Front Micom power control gpio\n");

			CMD_Printf("Normal Standby :: Front Clock Display ON~\n");

			VK_TASK_Sleep(300);

			DI_NANDFLASH_UmountPartitions();

			iResult = DI_POWER_SetPowerOff(DI_POWER_OFFMODE_STANDBY_NORMAL);
			AssertDI(iResult, CMD_ERR);
		}
	}
#endif
	else if CMD_IS("lcdpower")
	{
		if PARAM_IS("off")
		{
			CMD_Printf("Command : front lcdpower off-> Front LCD Power OFF\n");

			iResult = DI_FRONT_PANEL_SetPanelPower(FALSE);
			AssertDI(iResult, CMD_ERR);
		}
		else
		{
			CMD_Printf("Command : front lcdpower on-> Front LCD Power ON\n");

			iResult = DI_FRONT_PANEL_SetPanelPower(TRUE);
			AssertDI(iResult, CMD_ERR);
		}
	}
	else if CMD_IS("dim")
	{
		if PARAM_IS("test")
		{
			HUINT8 ucLevel = 0;

			if PARAM1_IS("down")	// dimming level down test
			{
				CMD_Printf("Command : front dim test down-> Front LCD Panel backlight dimming level-down Test \n");

				for(ucLevel=0xf; ucLevel>0; ucLevel--)
				{

					iResult = DI_FRONT_PANEL_SetDimmingLevel((ucLevel<<4));
					AssertDI(iResult, CMD_ERR);

					CMD_Printf("dim level (0x%x) \n", ucLevel<<4);

					VK_TASK_Sleep(500);
				}
			}
			else	// dimming level up test
			{
				CMD_Printf("Command : front dim test-> Front LCD Panel backlight dimming level-up Test \n");

				for(ucLevel=0; ucLevel<0xf; ucLevel++)
				{
					iResult = DI_FRONT_PANEL_SetDimmingLevel((ucLevel<<4));
					AssertDI(iResult, CMD_ERR);

					CMD_Printf("dim level (0x%x) \n", ucLevel<<4);

					VK_TASK_Sleep(500);
				}
			}
			CMD_Printf("front dim test End~ \n");
		}

		else if PARAM_IS("led") // dimming level down test
		{
			int ulLevel=0;
			DI_LED_DIMMING_LEVEL dim_table[] = {DI_LED_DIMMING_LOW, DI_LED_DIMMING_MID, DI_LED_DIMMING_HIGH};


			if (READABLE_IN_DEC(HWTEST_PARAM1, ulLevel))
			{

	#if defined(CONFIG_LED_GPIO_PWM_AND_THREE_STATE)
				CMD_Printf("Command : front dim test led-> Front LED dimming level Test(0 - 100) : level %d\n", ulLevel);
				iResult = DI_LED_SetPowerLedDimmingLevel(ulLevel);
				AssertDI(iResult, CMD_ERR);
	#else
				CMD_Printf("Command : front dim test led-> Front LED dimming level Test : level %d\n", ulLevel);
				if( ulLevel < 3 )
				{
					iResult = DI_LED_SetPowerLedDimmingLevel(dim_table[ulLevel]);
					AssertDI(iResult, CMD_ERR);
				}
				else
				{
					iResult = DRV_MICOM_SetLEDDimmingLevel(ulLevel);
					AssertDI(iResult, CMD_ERR);
				}
	#endif
				return CMD_OK;
			}
		}
		else if PARAM_IS("fixed")
		{
			DI_LED_ID nLedID=0xFF;
			int ulLevel=100;

			CMD_Printf("\tFixed GPIO LED Dimming Test~~~  \r\n");

			if (READABLE_IN_DEC(HWTEST_PARAM1, nLedID))
			{
				if (READABLE_IN_DEC(HWTEST_PARAM2, ulLevel))
				{
					CMD_Printf("Fixed Dimming LED Test Dimming Level(%d)", ulLevel);
					if(nLedID == DI_LED_NUM0)
					{
						iResult = DI_LED_SetStandbyLedDimmingLevel(ulLevel);
						if (iResult == DI_ERR_OK)
						{
							return CMD_OK;
						}

					}
					else if(nLedID == DI_LED_NUM1)
					{
						iResult = DI_LED_SetPowerLedDimmingLevel(ulLevel);
						if (iResult == DI_ERR_OK)
						{
							return CMD_OK;
						}

					}
					else
					{
						iResult = DI_LED_SetLedsDimmingLevel(nLedID, ulLevel);
						if (iResult == DI_ERR_OK)
						{
							return CMD_OK;
						}
					}
				}
			}

			CMD_Printf("Invalid paramters nLedID=%d, ulLevel=%d\n", nLedID, ulLevel);
			return CMD_ERR;

		}
		else
		{
			int ulLevel=0;
			if (READABLE_IN_DEC(HWTEST_PARAM, ulLevel))
			{
				CMD_Printf("Command : front dim %d -> Set Front LCD Panel backlight dimming level %d \n", ulLevel);

				iResult = DI_FRONT_PANEL_SetDimmingLevel((HUINT8)ulLevel);
				AssertDI(iResult, CMD_ERR);
			}
			else
			{
				CMD_Printf("Wrong Command~! right command : front dim [val(0~255)] \n");
				return CMD_ERR;
			}
		}
	}
	else if CMD_IS("rcu")
	{
		if PARAM_IS("off")
		{
			CMD_Printf("Command : front rcu off-> Front Send RCU Info OFF\r\n");

			iResult = DRV_MICOM_SetRCUInfo(FALSE);
			AssertDRV(iResult, CMD_ERR);
		}
		else
		{
			CMD_Printf("Command : front rcu on-> Front Send RCU Info ON\r\n");

			iResult = DRV_MICOM_SetRCUInfo(TRUE);
			AssertDRV(iResult, CMD_ERR);
		}
	}
	else if CMD_IS("unittest")
	{
		if PARAM_IS("on")
		{
			CMD_Printf("Command : front unittest on -> Display Module (LCD) Unit Test On By Micom\r\n");

			iResult = DRV_MICOM_SetUnitTest(0, TRUE);
			AssertDRV(iResult, CMD_ERR);
		}
		else
		{
			CMD_Printf("Command : front unittest off -> Display Module (LCD) Unit Test Off\r\n");

			iResult = DRV_MICOM_SetUnitTest(0, FALSE);
			AssertDRV(iResult, CMD_ERR);
		}
	}
	else if CMD_IS("reason")
	{
		DI_POWER_REASON nPowrReason;
		CMD_Printf("Command : front reason -> Get STB Wakeup Reason From Front Micom \r\n");

		iResult = DI_POWER_GetWakeupReason(&nPowrReason);
		AssertDI(iResult, CMD_ERR);

		CMD_Printf("nPowrReason (0x%x) : AC_ON(0x1), FRONT_KEY(0x2), IR(0x04 or 0x08), REAL_TIME_CLOCK(0x10), WATCH_DOG(0x20), \
SERIAL_COMMAND(0x40), HDMI_CEC(0x80), SYSTEM_REBOOT(0x100), RESERVED(0x00)\r\n", nPowrReason);

		if (nPowrReason==IR_NEC_TYPE || nPowrReason==IR_JVC_TYPE)
		{
			HUINT8	ucKeyCode;

			iResult = DI_KEY_GetWakeupRcuKey(&ucKeyCode);
			AssertDI(iResult, CMD_ERR);

			CMD_Printf("Wakeup RCU Key Source is 0x%x(%s)\n", ucKeyCode, s_asKeyCodeTable[ucKeyCode].aucKeyName);
		}
	}
#if defined(SUPPORT_IR_MULTI_CUSTOM_CODE)
	else if CMD_IS("code")
	{
		unsigned int ulCustomCode;

		if (HWTEST_PARAM != NULL)
		{
			if (VK_sscanf(HWTEST_PARAM, "%x", &ulCustomCode) == 0)
			{
				CMD_Printf("Wrong Command~! right command : front code [CustomCode(hex)]\r\n");
				return CMD_ERR;
			}

		}
		else
		{
			ulCustomCode = IR_MULTI_CUSTOM_CODE_CUSTOM_CODE_DEFAULT;
			CMD_Printf("No CustomCode Input... Set Default Custom Code(0x%x) \r\n", ulCustomCode);
		}

		CMD_Printf("Command : front code 0x%x -> Set RCU Custom Code to Front Micom\r\n", ulCustomCode);

		iResult = DRV_MICOM_SetCustomCode((HUINT16)ulCustomCode);
		AssertDRV(iResult, CMD_ERR);
	}
#endif
	else if CMD_IS("test")
	{
		if PARAM_IS("on")
		{
			CMD_Printf("Command : front test on -> Front Module Test On\r\n");

			iResult = P_Front_Test(TRUE);
#if defined(CONFIG_STLED)
			iResult |= DRV_STLED_7SegmentTest(TRUE);
#endif
			AssertDRV(iResult, CMD_ERR);
		}
		else
		{
			CMD_Printf("Command : front test off -> Front Module Test Off\r\n");

			iResult = P_Front_Test(FALSE);
#if defined(CONFIG_STLED)
			iResult |= DRV_STLED_7SegmentTest(FALSE);
#endif
			AssertDRV(iResult, CMD_ERR);
		}
	}
	else if CMD_IS("disp")
	{
		if (HWTEST_PARAM != NULL)
		{
			CMD_Printf("Command : front disp %s -> Front Display (LCD, VFD, 7Segment) String function Test \r\n", HWTEST_PARAM);

			iResult = DI_FRONT_PANEL_Display((HUINT8 *)HWTEST_PARAM);
#if defined(CONFIG_STLED)
			iResult |= DRV_STLED_WriteString(HWTEST_PARAM, 4, FALSE);
#endif
			AssertDI(iResult, CMD_ERR);
		}
		else
		{
			CMD_Printf("Wrong Command~! right command : front disp [string(<16)]  \r\n");
			return CMD_ERR;
		}
	}
	else if CMD_IS("led")
	{
		if PARAM_IS("power")
		{
			if PARAM1_IS("red")
			{
				CMD_Printf("Command : front led power red -> Front Power LED set RED (STANDBY)\r\n");

				iResult = DI_LED_SetPowerLedState(DI_LED_STANDBY);
				AssertDI(iResult, CMD_ERR);
			}
                        else if PARAM1_IS("off")
			{
				CMD_Printf("Command : front led power off -> Front Power LED OFF (OFF)\r\n");

				iResult = DI_LED_SetPowerLedState(DI_LED_OFF);
				AssertDI(iResult, CMD_ERR);
			}
			else	// blue (others)
			{
				CMD_Printf("Command : front led power others -> Front Power LED set BLUE (WATCHING)\r\n");

				iResult = DI_LED_SetPowerLedState(DI_LED_WATCHING);
				AssertDI(iResult, CMD_ERR);
			}
		}
		else if PARAM_IS("on")
		{
			int		ulLed;
			DI_LED_ID	nled_id;

			if ( READABLE_IN_DEC(HWTEST_PARAM1, ulLed) )
			{
				if (ulLed>31)
					ulLed = 31;
				CMD_Printf("Command : front led on %d-> Front LED On test\r\n", ulLed);
				nled_id = 0x1<<ulLed;
				iResult = DI_LED_SetOn(nled_id);
				AssertDI(iResult, CMD_ERR);
			}
			else
			{
				CMD_Printf("Wrong Command~! right command : front led on [led_number]  \r\n");
				return CMD_ERR;
			}
		}
		else if PARAM_IS("rgb")
		{
			int nRed=0xFF, nGreen=0xFF, nBlue=0xFF;
			DI_LED_ID nLedID=0xFF;

			CMD_Printf("\tGPIO LED RGB Test~~~  \r\n");

			if (READABLE_IN_DEC(HWTEST_PARAM1, nLedID))
			{
				if (READABLE_IN_DEC(HWTEST_PARAM2, nRed))
				{
					if (READABLE_IN_DEC(HWTEST_PARAM3, nGreen))
					{
						if (READABLE_IN_DEC(HWTEST_PARAM4, nBlue))
						{
							iResult = DI_LED_SetRGBOn(nLedID, nRed, nGreen, nBlue);
							if (iResult == DI_ERR_OK)
							{
								return CMD_OK;
							}
						}
					}
				}
			}

			CMD_Printf("Invalid paramters nLedID=%d, nRed=%d, nGreen=%d, nBlue=%d\n", nLedID, nRed, nGreen, nBlue);
			return CMD_ERR;
		}
		else if PARAM_IS("off")
		{
			int		ulLed;
			DI_LED_ID	nled_id;

			if ( READABLE_IN_DEC(HWTEST_PARAM1, ulLed) )
			{
				if (ulLed>31)
					ulLed = 31;
				CMD_Printf("Command : front led off %d-> Front LED Off test\r\n", ulLed);
				nled_id = 0x1<<ulLed;
				iResult = DI_LED_SetOff(nled_id);
				AssertDI(iResult, CMD_ERR);
			}
			else
			{
				CMD_Printf("Wrong Command~! right command : front led on [led_number]  \r\n");
				return CMD_ERR;
			}
		}
		//added by dsshin(01347)
		else if PARAM_IS("all")
		{
			int			i;
			DI_LED_ID	led_id = 0;
			HUINT32		led_capa;

			CMD_Printf("Command : front led all -> Turn on all leds\r\n");
			(void)DI_LED_GetCapability(&led_capa);
			for (i=0; i<32 && led_capa; i++) {
				led_id = led_capa & (1<<i);
				if (led_id != 0) {
					(void)DI_LED_SetOn(led_id);
				}
			}
			(void)DI_LED_SetPowerLedState(DI_LED_STANDBY);

			iResult = CMD_OK;
		}
		else if PARAM_IS("clear")
		{
			int			i;
			DI_LED_ID	led_id = 0;
			HUINT32		led_capa;

			CMD_Printf("Command : front led clear -> Turn off all leds\r\n");
			(void)DI_LED_GetCapability(&led_capa);
			for (i=0; i<32 && led_capa; i++) {
				led_id = led_capa & (1<<i);
				if (led_id != 0) {
					(void)DI_LED_SetOff(led_id);
				}
			}
			(void)DI_LED_SetPowerLedState(DI_LED_WATCHING);

			iResult = CMD_OK;
		}
		else
		{
			CMD_Printf("Wrong Command~! right command : front led power/on/off/clear \r\n");
			return CMD_ERR;
		}
	}
	else if CMD_IS("clock")
	{
		HUINT32		ulUnixTime = 0x00000000;

		if PARAM_IS("local")
		{
			if PARAM1_IS("set")
			{
				GET_NEXT_ARG(szdate);
				GET_NEXT_ARG(sztime);

				if (szdate != NULL && sztime != NULL)
				{
					CMD_Printf("Command : front clock local set %s %s (unixtime : %d)-> Set Front Micom Local Clock \r\n",
								szdate, sztime, ulUnixTime);

					iResult = INT_HwTest_Front_Convert_Input_To_Unixtime(szdate, sztime, &ulUnixTime);
					if (iResult != CMD_OK)
					{
						CMD_Printf("Wrong Command~! right command : front clock local set  YYYY/MM/DD HH:MM:SS (ex. front clock local set 2009/02/16 14:59:03)\r\n");
						return CMD_ERR;
					}
				}
				else
				{
					CMD_Printf("Wrong Command~! right command : front clock local set  YYYY/MM/DD HH:MM:SS (ex. front clock local set 2009/02/16 14:59:03)\r\n");
				}

				iResult = DI_RTC_SetCurrentTime(ulUnixTime);
				AssertDI(iResult, CMD_ERR);
			}
			else	// get
			{
				DRV_CLOCK_DATE_T		stDate;
				DRV_CLOCK_TIME_T		stTime;

				CMD_Printf("Command : front clock local -> Get Front Micom Local Clock\r\n");

				iResult = DI_RTC_GetCurrentTime(&ulUnixTime);
				AssertDI(iResult, CMD_ERR);

				iResult = DRV_RTC_ConvertUnixTimeToTime(ulUnixTime, &stTime);
				AssertDRV(iResult, CMD_ERR);

				iResult = DRV_RTC_ConvertUnixTimeToDate(ulUnixTime, &stDate);
				AssertDRV(iResult, CMD_ERR);

				CMD_Printf("Now Local Unixtime is %d(0x%x) \r\n", ulUnixTime, ulUnixTime);
				CMD_Printf("Now Local Date & Time is %04d/%02d/%02d %02d:%02d:%02d (YYYY/MM/DD HH:MM:SS)\r\n",
					stDate.usYear, stDate.ucMonth, stDate.ucDay, stTime.ucHour, stTime.ucMinute, stTime.ucSecond);

			}
		}
		else if PARAM_IS("disp")
		{
			if PARAM1_IS("off")
			{
				CMD_Printf("Command : front clock disp off ->  Front Clock Display Off~!\r\n");

				iResult = DI_FRONT_PANEL_SetTimeDisplay(FALSE);
				AssertDI(iResult, CMD_ERR);
			}
			else	// get
			{
				CMD_Printf("Command : front clock disp on ->  Front Clock Display ON~!\r\n");

				iResult = DI_FRONT_PANEL_SetTimeDisplay(TRUE);
				AssertDI(iResult, CMD_ERR);
			}
		}
		else if PARAM_IS("wakeup")
		{
			if PARAM1_IS("set")
			{
				GET_NEXT_ARG(szdate);
				GET_NEXT_ARG(sztime);

				if (szdate != NULL && sztime != NULL)
				{
					CMD_Printf("Command : front clock wakeup set %s %s (unixtime : %d)-> Set Front Micom Wakeup Clock \r\n",
								szdate, sztime, ulUnixTime);

					iResult = INT_HwTest_Front_Convert_Input_To_Unixtime(szdate, sztime, &ulUnixTime);
					if (iResult != CMD_OK)
					{
						CMD_Printf("Wrong Command~! right command : front clock wakeup set  YYYY/MM/DD HH:MM:SS (ex. front clock wakeup set 2009/02/16 14:59:03)\r\n");
						return CMD_ERR;
					}
				}
				else
				{
					CMD_Printf("Wrong Command~! right command : front clock wakeup set  YYYY/MM/DD HH:MM:SS (ex. front clock wakeup set 2009/02/16 14:59:03)\r\n");
				}

				iResult = DI_RTC_SetOnTime(ulUnixTime);
				AssertDI(iResult, CMD_ERR);
			}
			else if PARAM1_IS("on")
			{
				CMD_Printf("Command : front clock wakeup on -> Enable Front Micom Wakeup Clock setting\r\n");

				iResult = DI_RTC_OnTimerOnOff(TRUE);
				AssertDI(iResult, CMD_ERR);
			}
			else if PARAM1_IS("off")
			{
				CMD_Printf("Command : front clock wakeup off -> Diaable Front Micom Wakeup Clock setting\r\n");

				iResult = DI_RTC_OnTimerOnOff(FALSE);
				AssertDI(iResult, CMD_ERR);
			}
			else	// get
			{
				DRV_CLOCK_DATE_T		stDate;
				DRV_CLOCK_TIME_T		stTime;

				CMD_Printf("Command : front clock wakeup -> Get Front Micom Wakeup Clock\r\n");

				iResult = DI_RTC_GetOnTime(&ulUnixTime);
				AssertDI(iResult, CMD_ERR);

				iResult = DRV_RTC_ConvertUnixTimeToTime(ulUnixTime, &stTime);
				AssertDRV(iResult, CMD_ERR);

				iResult = DRV_RTC_ConvertUnixTimeToDate(ulUnixTime, &stDate);
				AssertDRV(iResult, CMD_ERR);

				CMD_Printf("Now Wakeup Unixtime is %d(0x%x) \r\n", ulUnixTime, ulUnixTime);
				CMD_Printf("Now Wakeup Date & Time is %04d/%02d/%02d %02d:%02d:%02d (YYYY/MM/DD HH:MM:SS)\r\n",
					stDate.usYear, stDate.ucMonth, stDate.ucDay, stTime.ucHour, stTime.ucMinute, stTime.ucSecond);
			}
		}
		else
		{
			CMD_Printf("Wrong Command~! right command : front clock local/disp/wakeup \r\n");
			return CMD_ERR;
		}
	}
	else if CMD_IS("aging")
	{
		if PARAM_IS("on")
		{
			CMD_Printf("Command : front aging -> ON\r\n");
			g_FrontAgingTestFlag = TRUE;

			if (ulFrontAgingTaskId==0)
			{
				iResult = VK_TASK_Create(P_Front_AgingTask, FAST_BOOT_TASK_PRIORITY, MICOM_RX_TASK_STACK_SIZE, "Front_Aging_Task", (void *)0, &ulFrontAgingTaskId, FALSE);
				if(iResult != VK_OK)
				{
					PrintError("[front aging on] P_Front_AgingTask Create Error\n");
					return CMD_ERR;
				}

	    		VK_TASK_Start(ulFrontAgingTaskId);
			}
			return CMD_OK;
		}
		else if PARAM_IS("off")
		{
			CMD_Printf("Command : front aging -> OFF\r\n");
			g_FrontAgingTestFlag = FALSE;
			return CMD_OK;
		}
		else
		{
			CMD_Printf("Wrong Command~! right command : front aging on/off\r\n");
			return CMD_ERR;
		}
 	}
	else if CMD_IS("stress")
	{
		if PARAM_IS("on")
		{
			CMD_Printf("Command : front stress -> ON\r\n");
			g_FrontStressTestFlag = TRUE;

			if(ulFrontStressTaskId == 0)
			{
				iResult = VK_TASK_Create(P_Front_StressTask, FAST_BOOT_TASK_PRIORITY, MICOM_RX_TASK_STACK_SIZE, "Front_Stress_Task", (void *)0, &ulFrontStressTaskId, FALSE);
				if(iResult != VK_OK)
				{
					PrintError("[front aging on] P_Front_StressTask Create Error\n");
					return CMD_ERR;
				}

	    		VK_TASK_Start(ulFrontStressTaskId);
			}
			return CMD_OK;
		}
		else if PARAM_IS("off")
		{
			CMD_Printf("Command : front stress -> OFF\r\n");
			g_FrontStressTestFlag = FALSE;
			return CMD_OK;
		}
		else
		{
			CMD_Printf("Wrong Command~! right command : front stress on/off\r\n");
			return CMD_ERR;
		}
 	}
#if defined(CONFIG_FRONT_MICOM_TOUCH) && defined(CONFIG_DEBUG)
	else if CMD_IS("touch")
	{
		unsigned char len;
		HUINT32 s_PrevKeyInfo = 0;
		unsigned int unThreshold[10];
		unsigned char ucThresholdChannel[10];
		int i;

		if PARAM_IS("set")
		{
			unsigned char ucChannelNum=0;

			char* szParm2=CMD_NextArg((char **)&szArgs);
			char* szParm3=CMD_NextArg((char **)&szArgs);
			char* szParm4=CMD_NextArg((char **)&szArgs);
			char* szParm5=CMD_NextArg((char **)&szArgs);
			char* szParm6=CMD_NextArg((char **)&szArgs);
			char* szParm7=CMD_NextArg((char **)&szArgs);
			char* szParm8=CMD_NextArg((char **)&szArgs);

			DRV_MICOM_GetTouchThreshold(ucThresholdChannel, &ucChannelNum);

			READABLE_IN_DEC(HWTEST_PARAM1, unThreshold[0]);
			READABLE_IN_DEC(szParm2, unThreshold[1]);
			READABLE_IN_DEC(szParm3, unThreshold[2]);
			READABLE_IN_DEC(szParm4, unThreshold[3]);
			READABLE_IN_DEC(szParm5, unThreshold[4]);

			if(ucChannelNum == 8)
			{
				READABLE_IN_DEC(szParm6, unThreshold[5]);
				READABLE_IN_DEC(szParm7, unThreshold[6]);
				READABLE_IN_DEC(szParm8, unThreshold[7]);
			}

			for(i=0; i<ucChannelNum; i++)
			{
				if(unThreshold[i] != 0)
				{
					ucThresholdChannel[i] = (unsigned char) unThreshold[i];
				}
			}

			CMD_Printf("[Set touch threshold test]\n");
			CMD_Printf("[0]Power, [1]Ch+, [2]Ch-, [3]Vol+, [4]Vol- : ");
			for(i=0; i<ucChannelNum; i++)
			{
				CMD_Printf("[%d] %d ", i, ucThresholdChannel[i]);
			}
			CMD_Printf("\n");

			DRV_MICOM_SetTouchThreshold(ucThresholdChannel, ucChannelNum);
		}
		else if PARAM_IS("get")
		{
			unsigned char ucThreshold[8];
			DRV_MICOM_GetTouchThreshold(ucThreshold, &len);

			CMD_Printf("[Get touch threshold test]\n");
			CMD_Printf("[0]Power, [1]Ch+, [2]Ch-, [3]Vol+, [4]Vol- : ");
			for(i=0; i<len; i++)
			{
				CMD_Printf("[%d] %d ", i, ucThreshold[i]);
			}
			CMD_Printf("\n");
		}
		else /* Touch test aging */
		{
		g_FrontTestFlag = TRUE;

		while(1)
		{
			unsigned char sens_data[16], len0, len1, len2, i;
			unsigned short signal_data[16], ref_data[16];

			VK_memset(sens_data,0x00,16);
			VK_memset(signal_data,0x00,16);
			VK_memset(ref_data,0x00,16);

			iResult = DRV_MICOM_GetTouchSensitive(sens_data, &len0);
			if( iResult != DRV_OK)
			{
				VK_TASK_Sleep(200);
				continue;
			}

			iResult = DRV_MICOM_GetTouchSignal(signal_data, &len1);
			if( iResult != DRV_OK)
			{
				VK_TASK_Sleep(200);
				continue;
			}

			iResult = DRV_MICOM_GetTouchReference(ref_data, &len2);
			if( iResult != DRV_OK)
			{
				VK_TASK_Sleep(200);
				continue;
			}
				CMD_Printf("[TOUCH] : Power|Ch+  |Ch-  |Vol+ |Vol- |\n");
			CMD_Printf("[TOUCH] : ");
			for(i=0; i<len0; i++)
			{
					CMD_Printf("%04d |", sens_data[i]);
			}
			CMD_Printf("\n");

			CMD_Printf("[SIGN ] : ");
			for(i=0; i<len1; i++)
			{
					CMD_Printf("%04d |", signal_data[i]);
			}
			CMD_Printf("\n");

			CMD_Printf("[ REF ] : ");
			for(i=0; i<len2; i++)
			{
					CMD_Printf("%04d |", ref_data[i]);
			}
			CMD_Printf("\n\n");

			if((g_LastKeyInfo!=0) && (s_PrevKeyInfo!=g_LastKeyInfo))
			{
				DI_KEY_DEVICE KeyDevice;
				DI_KEY_TYPE KeyType;
				HUINT32 keyCode;
				char str_buff[10];

				KeyDevice = (g_LastKeyInfo>>31) & 0x01;
				KeyType =   (g_LastKeyInfo>>30) & 0x01;
				keyCode =   (g_LastKeyInfo&0x3fffffff);

				s_PrevKeyInfo = g_LastKeyInfo;
				VK_sprintf(str_buff, "[%s]%c-%x", ((KeyDevice==DI_FRONT_KEY)?"FRONT":"RCU"), ((KeyType==DI_KEY_PRESSED)?'P':'R'), keyCode);
				DRV_MICOM_WriteString(str_buff, VK_strlen(str_buff));
			}

			VK_TASK_Sleep(400);
		}
	}
		return CMD_OK;
	}
#endif
#if defined(CONFIG_STLED)
	else if CMD_IS("stled")
	{
		if PARAM_IS("on")
		{
			(void)DRV_STLED_7SegmentOnOff(TRUE);
		}
		else if PARAM_IS("off")
		{
			(void)DRV_STLED_7SegmentOnOff(FALSE);
		}
		else if PARAM_IS("dim")
		{
			(void)DRV_STLED_Brightness(VK_atoi((char *)szParam1));
		}
		else if PARAM_IS("led")
		{
			if PARAM1_IS("on")
			{
				unsigned char ucData[2] = {0x00, 0xff};
				(void)DRV_STLED_DisplayLED(ucData);
			}
			else if PARAM1_IS("off")
			{
				unsigned char ucData[2] = {0x00, 0x00};
				(void)DRV_STLED_DisplayLED(ucData);
			}
		}
		else if PARAM_IS("segtest")
		{
			(void)DRV_STLED_7SegmentTest(TRUE);
		}
		return CMD_OK;
	}
#endif
#if defined(CONFIG_MICOM_ALIVE_TASK_ENABLE)
	else if CMD_IS("micom_alive_task")
	{
		if PARAM_IS("on")
		{
			CMD_Printf("Command : Micom alive task control ==> ON \r\n");
			DRV_MICOM_SetMicomAliveTask(TRUE);
		}
		else if PARAM_IS("off")
		{
			CMD_Printf("Command : Micom alive task control ==> OFF \r\n");
			DRV_MICOM_SetMicomAliveTask(FALSE);
		}
		else
		{
			CMD_Printf("Wrong Command~! right command : micom_alive_task on/off\r\n");
			return CMD_ERR;
		}

		return CMD_OK;
	}
	else if CMD_IS("micom_alive_check")
	{
		if PARAM_IS("on")
		{
			CMD_Printf("Command : Micom alive check control ==> ON \r\n");
			DRV_MICOM_SetMicomAliveCheck(TRUE);
		}
		else if PARAM_IS("off")
		{
			CMD_Printf("Command : Micom alive check control ==> OFF \r\n");
			DRV_MICOM_SetMicomAliveCheck(FALSE);
		}
	else
	{
			CMD_Printf("Wrong Command~! right command : micom_alive_check on/off\r\n");
			return CMD_ERR;
		}
		return CMD_OK;
	}
#endif
#if defined(CONFIG_MICOM_JP)
	else if CMD_IS("blink")
	{
		int		ulIconNum;
		int		nBlinkTime;
		DI_LED_ID	nIconId;

		if ( READABLE_IN_DEC(HWTEST_PARAM, ulIconNum) )
		{
			if (ulIconNum>24)
			{
				ulIconNum = 24;
			}
			nIconId = 0x1<<ulIconNum;

			if ( READABLE_IN_DEC(HWTEST_PARAM1, nBlinkTime) )
			{
				CMD_Printf("Command : Blink [Icon:%d], [Time:%d] ==> Front Icon Blinking test\r\n", ulIconNum, nBlinkTime);
				iResult = DRV_MICOM_SetBlinkingTime( nIconId, (unsigned char)nBlinkTime);
				AssertDI(iResult, CMD_ERR);
			}
		}
		else
		{
			CMD_Printf("Wrong Command~! right command : front blink [Icon_number] [Blink_time] \r\n");
			return CMD_ERR;
		}
	}
#endif
#if defined(CONFIG_RF4CE_MICOM)
	else if CMD_IS("rf4ce")
	{
		if PARAM_IS("inout")
		{
			if PARAM1_IS("cwoutput")
			{
				DRV_MICOM_SetRf4ceCerti_InOutMode(0);
			}
			else if PARAM1_IS("cmoutput")
			{
				DRV_MICOM_SetRf4ceCerti_InOutMode(1);
			}
			else if PARAM1_IS("input")
			{
				DRV_MICOM_SetRf4ceCerti_InOutMode(2);
			}
			else
			{
				CMD_Printf("Wrong Command~! right command : rf4ce inout\r\n");
				return CMD_ERR;
			}
		}
		else if PARAM_IS("setchan")
		{
			int nChannelNum=0;

			READABLE_IN_DEC(HWTEST_PARAM1, nChannelNum);
			if (DRV_MICOM_SetRf4ceCerti_ChangeFreq((unsigned char)nChannelNum))
			{
				CMD_Printf("Wrong Command~! right command : rf4ce setchan\r\n");
				return CMD_ERR;
			}
		}
		else
		{
			CMD_Printf("Wrong Command~! right command : rf4ce pair/status\r\n");
			return CMD_ERR;
		}

		return CMD_OK;
	}
#endif
#if 0
	else if CMD_IS("ipled")
	{
		if PARAM_IS("on")
		{
			CMD_Printf("IP LED TEST : ON \r\n");
			DI_LED_SetIPLED(TRUE);
		}
		else
		{
			CMD_Printf("IP LED TEST : OFF \r\n");
			DI_LED_SetIPLED(FALSE);
		}

		return CMD_OK;
	}
#endif
	else
	{
		CMD_Printf("Command : front ?? -> front ver/reboot/wd/ poweroff/ lcdpower/dim/ rcu/unittest/reason/code/test/disp/led/clock/micom_alive_task/micom_alive_check/blink\r\n");
	}

	return iResult;
}

#if defined(CONFIG_RECORD)
int P_Hdd_RegisterCallback(pfnHI_HDD_Dst_NotifyFUNC NotifyFUNC)
{
    sHddDstCallback = NotifyFUNC;
}

static int P_Hdd_AtaCmd(int fd, unsigned char *buf, AtaCmd eCmd)
{
    unsigned char *pbuf = buf;
    /* Read Identify Device info from HDD as used by S.M.A.R.T
       Check 82th-byte whether S.M.A.R.T Set Support or not */

    if(eCmd >= ATA_CMD_MAX)
    {
        CMD_Printf("[%d]%s No Command \n",__LINE__,__func__);
        return 1;
    }

    /* 0 = ATA Command
    * 1 = Sector Count
    * 2 = Features
    * 3 = Sector Number
    * 4 = CylinderLow
    * 5 = CylinderHigh
    * 6 = Device/Head */

    switch (eCmd)
    {
        case ATA_IDENTIFY_DEVICE:
             pbuf[0] = 0xec;
             pbuf[1] = 0;
             pbuf[2] = 0;
             pbuf[3] = 1;
             pbuf[4] = 0;
             pbuf[5] = 0;
             break;
        case ATA_SMART_READ_ATTR_VALUE:
             pbuf[0] = 0xb0;
             pbuf[1] = pbuf[3] = 1;
             pbuf[2] = 0xd0;
             pbuf[4] = 0x4f;
             pbuf[5] = 0xc2;
             break;
        case ATA_SMART_READ_ATTR_THRE_VALUE:
             pbuf[0] = 0xb0;
             pbuf[1] = pbuf[3] = 1;
             pbuf[2] = 0xd1;
             pbuf[4] = 0x4f;
             pbuf[5] = 0xc2;
             break;
        case ATA_SMART_ENABLE:
             pbuf[0] = 0xb0;
             pbuf[1] = 0;
             pbuf[2] = 0xd8;
             pbuf[3] = 0;
             pbuf[4] = 0x4f;
             pbuf[5] = 0xc2;
             break;
        case ATA_SMART_EXECUTE_OFFLINE_IMMEDIATE:
             pbuf[0] = 0xb0;
             pbuf[2] = 0xd4;
             pbuf[3] = 0;
             pbuf[4] = 0x4f;
             pbuf[5] = 0xc2;
             break;
        default:
             break;
    }

    if(ioctl(fd,SMART_CMD,pbuf)!= 0)
    {
        CMD_Printf("[%d]%s ioctl error!!! \n",__LINE__,__func__);
        return 1;
    }

    if((pbuf[4] == 0xf4) && (pbuf[5] == 0x2c))
    {
        CMD_Printf("[%d]%s read attribute data failed\n",__LINE__,__func__);
        return 1;
    }

    return 0;
}

static int P_Hdd_DstStatus(int fd)
{
    int ret=0;
    unsigned char buf[SMART_BUF_SIZE];
    unsigned char status=0;

    VK_memset(buf,0x00,SMART_BUF_SIZE);
    ret=P_Hdd_AtaCmd(fd, buf, ATA_SMART_READ_ATTR_VALUE);
    if(ret < 0)
    {
        CMD_Printf("[%d]%s Error to read Attr values \n",__LINE__,__func__);
        return -1;
    }
    status = ((buf[367] >> 4) & 0x0F);
    switch(status) {
        case 15:
       	    break;
        case 0:
	    CMD_Printf("Diagnostic Test : Complete No Error\n");
	    break;
        case 3:
	    CMD_Printf("Diagnostic Test : Fatal error\n");
	    break;
        case 4:
	    CMD_Printf("Diagnostic Test : Test element failed\n");
	    break;
        case 5:
	    CMD_Printf("Diagnostic Test : Electrical element failed\n");
	    break;
        case 6:
	    CMD_Printf("Diagnostic Test : Servo(seek) element failed\n");
	    break;
        case 7:
	    CMD_Printf("Diagnostic Test : Read element failed\n");
	    break;
        case 8:
	    CMD_Printf("Diagnostic Test : Handling Damage\n");
	    break;
        default:
	    status = -1;
	    break;
    }
    return status;
}

static void P_HddDst_TimeNotify(unsigned long tid, void *arg)
{
	int ret;

	ret=P_Hdd_DstStatus(hddfd);
	if(bIsHddDstTesting != TRUE)
		return;

	if(sHddDstCallback != NULL)
		(int *)(sHddDstCallback)(ret);

}

static int P_Hdd_Dst_Stop(void)
{
	if(HddDstTimerId != NULL)
		VK_TIMER_Cancel(HddDstTimerId);

	bIsHddDstTesting = FALSE;
	close(hddfd);
}

static int P_Hdd_Dst(char *str,int dst)
{
    int ret=0;
    unsigned char buf[SMART_BUF_SIZE];
    char devpath[10];

    VK_sprintf(devpath,"%s",str);
    if(bIsHddDstTesting != TRUE)
    {
	    hddfd=open(devpath, O_RDONLY | O_NONBLOCK);
	    if(hddfd < 0)
	    {
		CMD_Printf("[%d]%s Error to open %s \n",__LINE__,__func__,devpath);
		return 1;
	    }
    }

    ret=P_Hdd_DstStatus(hddfd);
    if(ret < 0)
    {
        CMD_Printf("[%d]%s Error to read DstStatus\n",__LINE__,__func__);
	goto err;
    }

    if(ret == 15)
    {
	goto doing;
    }
    curDstTest = 0;

    VK_memset(buf,0x00,SMART_BUF_SIZE);
    ret=P_Hdd_AtaCmd(hddfd, buf, ATA_SMART_ENABLE);
    if(ret < 0)
    {
        CMD_Printf("[%d]%s Error to read Attr values \n",__LINE__,__func__);
	goto err;
    }
    VK_memset(buf,0x00,SMART_BUF_SIZE);
    buf[1] = dst;
    ret=P_Hdd_AtaCmd(hddfd, buf, ATA_SMART_EXECUTE_OFFLINE_IMMEDIATE);
    if(ret < 0)
    {
        CMD_Printf("[%d]%s Error to do SMART EXECUTE OFFLINE IMMEDIATE \n",__LINE__,__func__);
	goto err;
    }

    VK_memset(buf,0x00,SMART_BUF_SIZE);
    ret=P_Hdd_AtaCmd(hddfd, buf, ATA_SMART_READ_ATTR_VALUE);
    if(ret < 0)
    {
        CMD_Printf("[%d]%s Error to read Attr values \n",__LINE__,__func__);
	goto err;
    }

    if(!(buf[367] & 0xF0))
    {
        CMD_Printf("[%d]%s Error to do LongDST/ShortDST or not Start\n",__LINE__,__func__);
	goto err;
    }
    CMD_Printf("Start %s DST!\n",(dst == 1) ? "Short" : "Long");

    curDstTest = dst;
    bIsHddDstTesting = TRUE;
    VK_TIMER_EventEvery(1000, P_HddDst_TimeNotify,NULL,0,&HddDstTimerId);

doing:
    return 0;

err:
    close(hddfd);
    return 1;

}

#if defined(CONFIG_HDD)
int CMD_HwTest_Parser_Hdd(void *szArgs)
{
	GET_ARGS;
	int portnum;

	if CMD_IS("long")
	{
		if(HWTEST_PARAM != NULL)
		{
			iResult=P_Hdd_Dst(HWTEST_PARAM,SMART_LONG_TEST);
			if(iResult == CMD_ERR)
				CMD_Printf("Error HDD Long DST!\n");
		}
		else
		{
			CMD_Printf("No dev name\n");
			iResult = CMD_ERR;
		}

	}
	else if CMD_IS("short")
	{
		if(HWTEST_PARAM != NULL)
		{
			iResult=P_Hdd_Dst(HWTEST_PARAM,SMART_SHORT_TEST);
			if(iResult == CMD_ERR)
				CMD_Printf("Error HDD Short DST!\n");
		}
		else
		{
			CMD_Printf("No dev name\n");
			iResult = CMD_ERR;
		}
	}
	else if CMD_IS("stop")
	{
		iResult=P_Hdd_Dst_Stop();
		if(iResult == CMD_ERR)
			CMD_Printf("Error HDD Short DST!\n");

	}
#if defined(CONFIG_DI_SHARED_LIB)
#if defined(CONFIG_PRODUCT_IMAGE_HWTEST)
	else if CMD_IS("spinup")
	{
		if (!HWTEST_PARAM) return CMD_ERR;

		READABLE_IN_DEC(HWTEST_PARAM, portnum);
		iResult=DRV_SMART_ATA_SpinUp(portnum);
		if (iResult==CMD_ERR)
			CMD_Printf("Error HDD Spin Up!!\n");
	}
	else if CMD_IS("spindown")
	{
		if (!HWTEST_PARAM) return CMD_ERR;

		READABLE_IN_DEC(HWTEST_PARAM, portnum);
		iResult=DRV_SMART_ATA_SpinDown(portnum);
		if (iResult==CMD_ERR)
			CMD_Printf("Error HDD Spin Down!!\n");
	}
#endif
#else
#if defined (CONFIG_HDD_SMART)
	else if CMD_IS("spinup")
	{
		if (!HWTEST_PARAM) return CMD_ERR;

		READABLE_IN_DEC(HWTEST_PARAM, portnum);
		iResult = DRV_SMART_ATA_SpinUp(portnum);
		if (iResult == CMD_ERR)
			CMD_Printf("Error HDD Spin Up!!\n");
	}
	else if CMD_IS("spindown")
	{
		if (!HWTEST_PARAM) return CMD_ERR;

		READABLE_IN_DEC(HWTEST_PARAM, portnum);
		iResult = DRV_SMART_ATA_SpinDown(portnum);
		if (iResult == CMD_ERR)
			CMD_Printf("Error HDD Spin Down!!\n");
	}
#endif
#endif
#if defined(CONFIG_DI_SHARED_LIB)
#if defined(CONFIG_PRODUCT_IMAGE_HWTEST)
	else if CMD_IS("power") {
		if (!HWTEST_PARAM1) return CMD_ERR;

		READABLE_IN_DEC(HWTEST_PARAM1, portnum);

		if (PARAM_IS("on")) {
			DI_HDD_ControlPower(portnum, 1);
		}
		else if(PARAM_IS("off")) {
			DI_HDD_ControlPower(portnum, 0);
		}
		iResult = CMD_OK;
	}
#endif
#else
	else if CMD_IS("power") {
		if (!HWTEST_PARAM1) return CMD_ERR;

		READABLE_IN_DEC(HWTEST_PARAM1, portnum);

		if (PARAM_IS("on")) {
			DI_HDD_ControlPower(portnum, 1);
		}
		else if(PARAM_IS("off")) {
			DI_HDD_ControlPower(portnum, 0);
		}
		iResult = CMD_OK;
	}
#endif
#if defined(CONFIG_DI_SHARED_LIB)
#if defined(CONFIG_PRODUCT_IMAGE_HWTEST)
	else if CMD_IS("dst")
	{
		int portnum;

		if (!HWTEST_PARAM) return CMD_ERR;

		READABLE_IN_DEC(HWTEST_PARAM, portnum);
		DI_HDD_StartDst(portnum);

		iResult = CMD_OK;
	}
#endif
#else
	else if CMD_IS("dst")
	{
		int portnum;

		if (!HWTEST_PARAM) return CMD_ERR;

		READABLE_IN_DEC(HWTEST_PARAM, portnum);
		DI_HDD_StartDst(portnum);

		iResult = CMD_OK;
	}
#endif
#if defined(CONFIG_DI_SHARED_LIB)
#if defined(CONFIG_PRODUCT_IMAGE_HWTEST)
	else if CMD_IS("info")
	{
		DRV_SMART_SATA_INFO_t	hddInfo;
		char					encr_key[32] = {0,};

		if (!HWTEST_PARAM) return CMD_ERR;

		READABLE_IN_DEC(HWTEST_PARAM, portnum);
		DRV_SMART_IdentifyDevice(portnum, &hddInfo);
#if defined(CONFIG_CRYPTSETUP)
		DI_HDD_GetEncryptionKey(portnum, encr_key);
#endif

		CMD_Printf("[SIZE] %ldMB\n", hddInfo.ulSizeMB);
		CMD_Printf("[MODEL] %s\n", hddInfo.ucModelName);
		CMD_Printf("[FIRMWARE] %s\n", hddInfo.ucSerialNo);
		CMD_Printf("[SERIAL] %s\n", hddInfo.ucFirmwareVer);
		CMD_Printf("[ENCR-KEY] %s\n", encr_key);

		iResult = CMD_OK;
	}
#endif
#else
#if defined (CONFIG_HDD_SMART)
	else if CMD_IS("info")
	{
		DRV_SMART_SATA_INFO_t	hddInfo;
		char					encr_key[32] = {0,};

		if (!HWTEST_PARAM) return CMD_ERR;

		READABLE_IN_DEC(HWTEST_PARAM, portnum);
		DRV_SMART_IdentifyDevice(portnum, &hddInfo);
#if defined(CONFIG_CRYPTSETUP)
		DI_HDD_GetEncryptionKey(portnum, encr_key);
#endif

		CMD_Printf("[SIZE] %ldMB\n", hddInfo.ulSizeMB);
		CMD_Printf("[MODEL] %s\n", hddInfo.ucModelName);
		CMD_Printf("[FIRMWARE] %s\n", hddInfo.ucSerialNo);
		CMD_Printf("[SERIAL] %s\n", hddInfo.ucFirmwareVer);
		CMD_Printf("[ENCR-KEY] %s\n", encr_key);

		iResult = CMD_OK;
	}
#endif
#endif
	else
	{
		CMD_Printf("Command : hdd [long/short] \n");
		iResult = CMD_ERR;
	}
	return iResult;
}
#endif
#endif

#if defined(CONFIG_ETHERNET)
#if defined(SUPPORT_ETHERNET)

int CMD_HwTest_Parser_SetFtpOtaInfo(void *szArgs)
{
	DI_NVRAM_FTP_OTA_SETTING_T *pFtpOtaSet;
	DI_NVRAM_OTA_FLAG_E g_eOtaFlag;
	DI_NVRAM_OTA_TYPE_E g_eOtaType;
	DI_NVRAM_OTA_CHANNEL_E g_eOtaChannel;

	HUINT8 aUser[]={"stbclient"};
	HUINT8 aPasswd[]={"humax"};
	HUINT8 aDir[]={"ota_files/804e7500"};
	HUINT8 aFileName[]={"804e7500.1.00.00.hdf"};

	pFtpOtaSet = VK_MEM_Alloc(sizeof(DI_NVRAM_FTP_OTA_SETTING_T));
	VK_MEM_Memset(pFtpOtaSet, 0x0, sizeof(DI_NVRAM_FTP_OTA_SETTING_T));


	pFtpOtaSet->aServerIp[0]= 91;
	pFtpOtaSet->aServerIp[1]= 202;
	pFtpOtaSet->aServerIp[2]= 202;
	pFtpOtaSet->aServerIp[3]= 60;

	VK_strncpy(pFtpOtaSet->aUserName, aUser, VK_strlen(aUser));
	VK_strncpy(pFtpOtaSet->aPasswd, aPasswd, VK_strlen(aPasswd));
	VK_strncpy(pFtpOtaSet->aDirPath, aDir, VK_strlen(aDir));
	VK_strncpy(pFtpOtaSet->aFileName, aFileName, VK_strlen(aFileName));

	DI_UART_Print("[CMD_HwTest_Parser_Ethernet] %d %d %d %d : %s : %s : %s : %s \n",
		pFtpOtaSet->aServerIp[0],
		pFtpOtaSet->aServerIp[1],
		pFtpOtaSet->aServerIp[2],
		pFtpOtaSet->aServerIp[3],
		pFtpOtaSet->aUserName,
		pFtpOtaSet->aPasswd,
		pFtpOtaSet->aDirPath,
		pFtpOtaSet->aFileName);

	g_eOtaFlag = DI_NVRAM_OTA_FLAG_DETECTED;
	g_eOtaType = DI_NVRAM_OTA_FILE;
	g_eOtaChannel = DI_NVRAM_OTA_CHANNEL_FTP;

	DI_NVRAM_SetField(DI_NVRAM_FIELD_OTA_FTP_INFO, 0, (void *)pFtpOtaSet, sizeof(DI_NVRAM_FTP_OTA_SETTING_T));
	DI_NVRAM_SetField(DI_NVRAM_FIELD_OTA_FLAG, 0, &g_eOtaFlag, sizeof(DI_NVRAM_OTA_FLAG_E));
	DI_NVRAM_SetField(DI_NVRAM_FIELD_OTA_TYPE, 0, &g_eOtaType, sizeof(DI_NVRAM_OTA_TYPE_E));
	DI_NVRAM_SetField(DI_NVRAM_FIELD_CH_TYPE, 0, &g_eOtaChannel, sizeof(DI_NVRAM_OTA_CHANNEL_E));


	return 0;
}
#endif

extern DRV_Error DRV_ETH_Network_GetInfo(DI_ETH_NETWORK_INFO* pNetworkInfo);
extern DRV_Error DRV_ETH_Network_SetInfo(DI_ETH_NETWORK_INFO* pNetworkInfo);

int CMD_HwTest_Parser_Ethernet(void *szArgs)
{
	GET_ARGS;

	if CMD_IS("info")
	{
		DI_ETH_NETWORK_INFO EthInfo;

		DRV_ETH_Network_GetInfo(&EthInfo);
		VK_printf("MAC : %02x:%02x:%02x:%02x:%02x:%02x\n", EthInfo.mac_addr[0], EthInfo.mac_addr[1],EthInfo.mac_addr[2],EthInfo.mac_addr[3], EthInfo.mac_addr[4], EthInfo.mac_addr[5]);
		VK_printf("IP : %d.%d.%d.%d\n", EthInfo.ip_addr[0], EthInfo.ip_addr[1],EthInfo.ip_addr[2],EthInfo.ip_addr[3]);
		VK_printf("Bcast : %d.%d.%d.%d\n", EthInfo.brd_addr[0],EthInfo.brd_addr[1],EthInfo.brd_addr[2],EthInfo.brd_addr[3]);
		VK_printf("Mask : %d.%d.%d.%d\n", EthInfo.net_addr[0],EthInfo.net_addr[1],EthInfo.net_addr[2],EthInfo.net_addr[3]);


		static char parser_eth_caMsg[100];
		VK_memset(parser_eth_caMsg, 0x00, 100);
		// 한번에 UI로 출력하려고 하면 화면에 전부 표시되지 않는다. 각각 한줄씩 표시하도록 수정
        if ( CMD_GetMsgUI() != NULL )
        {
			VK_sprintf(parser_eth_caMsg, "MAC  : %02x:%02x:%02x:%02x:%02x:%02x  \n", EthInfo.mac_addr[0], EthInfo.mac_addr[1],EthInfo.mac_addr[2],EthInfo.mac_addr[3], EthInfo.mac_addr[4], EthInfo.mac_addr[5]);
            (int *)(CMD_GetMsgUI())(parser_eth_caMsg);
			VK_sprintf(parser_eth_caMsg, "IP       : %d.%d.%d.%d  \n", EthInfo.ip_addr[0], EthInfo.ip_addr[1],EthInfo.ip_addr[2],EthInfo.ip_addr[3]);
            (int *)(CMD_GetMsgUI())(parser_eth_caMsg);
			VK_sprintf(parser_eth_caMsg, "Bcast : %d.%d.%d.%d  \n", EthInfo.brd_addr[0],EthInfo.brd_addr[1],EthInfo.brd_addr[2],EthInfo.brd_addr[3]);
            (int *)(CMD_GetMsgUI())(parser_eth_caMsg);
			VK_sprintf(parser_eth_caMsg, "Mask  : %d.%d.%d.%d  \n", EthInfo.net_addr[0],EthInfo.net_addr[1],EthInfo.net_addr[2],EthInfo.net_addr[3]);
            (int *)(CMD_GetMsgUI())(parser_eth_caMsg);
        }
	}
	else if CMD_IS("ip")
	{
		DI_ETH_NETWORK_INFO EthInfo;
		DRV_ETH_Network_GetInfo(&EthInfo);

		if (HWTEST_PARAM!=NULL)
		{
			unsigned int iIndex;
			int iStartPos;
			int iIpIndex=0;
			unsigned int iVal;
			char caData[10];

			for (iIndex=0, iStartPos=0 ; iIndex<VK_strlen(HWTEST_PARAM) ; iIndex++)
			{
				if (HWTEST_PARAM[iIndex]=='.' || iIndex==VK_strlen(HWTEST_PARAM)-1)
				{
					VK_memset(caData, 0, 10);
					if (iIndex==VK_strlen(HWTEST_PARAM)-1)
						VK_memcpy(caData, &HWTEST_PARAM[iStartPos], iIndex-iStartPos+1);
					else
						VK_memcpy(caData, &HWTEST_PARAM[iStartPos], iIndex-iStartPos);

					VK_sscanf(caData, "%u", &iVal);
					EthInfo.ip_addr[iIpIndex++]=iVal;
					iStartPos=iIndex+1;
				}
			}

			DRV_ETH_Network_SetInfo(&EthInfo);
		}
	}
	else if CMD_IS("dhcp")
	{
		HUINT8 msg[100];

		if PARAM_IS("start")
		{
			VK_sprintf(msg, "DHCP Deamon is starting...");
			(int *)(CMD_GetMsgUI())(msg);
			VK_printf("%s ", __func__,msg);

			VK_SYSTEM_Command("ifconfig eth0 up");
			VK_SYSTEM_Command("udhcpc -t 3 -T 10 -i eth0 -n -q &");
		}
		else if PARAM_IS("stop")
		{
			VK_SYSTEM_Command("killall udhcpc");
			VK_SYSTEM_Command("killall zcip");

			VK_sprintf(msg, "DHCP Deamon is stopped!");
			(int *)(CMD_GetMsgUI())(msg);
			VK_printf("%s ", __func__,msg);
		}
		else
		{
			HWTEST_PARAM1 = HWTEST_PARAM1;

			CMD_Printf("command : dhcp start/stop");

			return iResult;
		}
	}
	else
	{
		HWTEST_PARAM1 = HWTEST_PARAM1;

		CMD_Printf("Command : ethernet ?? -> ethernet info/ip \n");

		return iResult;
	}
	return 0;
}
#endif

#if defined(CONFIG_FAN)
#if defined(CONFIG_DI_SHARED_LIB)
#if defined(CONFIG_PRODUCT_IMAGE_HWTEST)
static void P_FAN_CbCheckTemp(void *pvParam)
{
	DI_SYS_FAN_INFO_t	*pstFanInfo;

	if(pvParam == NULL)
	{
		CMD_Printf("[%s]pvParam is NULL\n", __func__);
		return;
	}

	pstFanInfo = (DI_SYS_FAN_INFO_t *)pvParam;

	CMD_Printf("[%s] is CALLED :  ucTemp(%d)v ulFanCtrlVoltage(%d), ulFanFeedback_Hz(%d)\n",
					__func__, pstFanInfo->ucTemp, pstFanInfo->ulFanCtrlVoltage, pstFanInfo->ulFanFeedback_Hz);

	return;
}

static void P_FAN_CbCheckOverTemp(void *pvParam)
{
	DI_SYS_EVENT	eEvent;

	if(pvParam == NULL)
	{
		CMD_Printf("[%s]pvParam is NULL\n", __func__);
		return;
	}

	eEvent = *(DI_SYS_EVENT *)pvParam;

	CMD_Printf("[%s] is CALLED eEvent(%d):  \n",
					__func__,eEvent);

	return;
}
static char parser_fan_caMsg[200];
int CMD_HwTest_Parser_Fan(void *szArgs)
{
	GET_ARGS;

	if CMD_IS("info")
	{
		if PARAM_IS("off")
		{
			CMD_Printf("Command : fan info off -> PANNEL FAN Info Display OFF\n");

			g_bFanDispInfo = FALSE;

			iResult = DI_FRONT_PANEL_Display((HUINT8 *)" ");
			AssertDI(iResult, CMD_ERR);
		}
		else
		{


			CMD_Printf("Command : fan info on -> PANNEL FAN Info Display ON\n");

			g_bFanDispInfo = TRUE;
            VK_MEM_Memset(parser_fan_caMsg, 0x00, 200);
			iResult = DRV_FAN_DisplayInfo(parser_fan_caMsg);
            if ( CMD_GetMsgUI() != NULL )
            {
                (int *)(CMD_GetMsgUI())(parser_fan_caMsg);
            }
			AssertDRV(iResult, CMD_ERR);
		}
	}
	else if CMD_IS("mode")
	{
		if PARAM_IS("0")
		{
			CMD_Printf("Command : fan mode 0 -> AUTOMATIC FAN control by task\n");
            if ( CMD_GetMsgUI() != NULL )
            {
                (int *)(CMD_GetMsgUI())("Automatic FAN Control by task\n");
            }

			iResult = DRV_FAN_SetTestMode(FALSE);
			AssertDRV(iResult, CMD_ERR);
		}
		else
		{
			CMD_Printf("Command : fan mode x -> Manual FAN control by user\n");
            if ( CMD_GetMsgUI() != NULL )
            {
                (int *)(CMD_GetMsgUI())("Manual FAN Control by user\n");
            }

			iResult = DRV_FAN_SetTestMode(TRUE);
			AssertDRV(iResult, CMD_ERR);
		}
	}
	else if CMD_IS("volt")
	{
		int ulVoltage;
		if (READABLE_IN_DEC(HWTEST_PARAM, ulVoltage))
		{

			CMD_Printf("Command : fan volt %d -> FAN RUNNING In %d V\n", ulVoltage,ulVoltage);
            VK_MEM_Memset(parser_fan_caMsg, 0x00, 200);
			VK_sprintf(parser_fan_caMsg, "FAN is running in %d V\n", ulVoltage);
            if ( CMD_GetMsgUI() != NULL )
            {
                (int *)(CMD_GetMsgUI())(parser_fan_caMsg);
            }

			iResult = DRV_FAN_SetCtrlDetail(ulVoltage);
			AssertDRV(iResult, CMD_ERR);
		}
		else
		{
			CMD_Printf("Wrong Command~! right command : fan volt [val(<%d)] \n", FAN_VOLTAGE_MAX);
			return CMD_ERR;
		}
	}
	else if CMD_IS("cw")
	{
		int ulvalue;

		if (READABLE_IN_DEC(HWTEST_PARAM, ulvalue))
		{
			CMD_Printf("Command : fan cw 0x%04x -> SET PWM ControlWord %d\n", ulvalue);

			iResult = DRV_FAN_SetControlWord((HUINT16)ulvalue);
			AssertDRV(iResult, CMD_ERR);
		}
		else
		{
			CMD_Printf("Wrong Command~! right command : fan cw [val(0~32768))] \n");
			return CMD_ERR;
		}
	}
	else if CMD_IS("period")
	{
		int ulvalue;

		if (READABLE_IN_DEC(HWTEST_PARAM, ulvalue))
		{
			CMD_Printf("Command : fan period 0x%04x -> SET PWM period %d\n", ulvalue);

			iResult = DRV_FAN_SetPeriodInterval((HUINT16)ulvalue);
			AssertDRV(iResult, CMD_ERR);
		}
		else
		{
			CMD_Printf("Wrong Command~! right command : fan period [val(0~255)] \n");
			return CMD_ERR;
		}
	}
	else if CMD_IS("duty")
	{
		int ulvalue;

		if (READABLE_IN_DEC(HWTEST_PARAM, ulvalue))
		{
			CMD_Printf("Command : fan duty 0x%04x -> SET PWM duty %d\n", ulvalue);
            VK_MEM_Memset(parser_fan_caMsg, 0x00, 200);
			VK_sprintf(parser_fan_caMsg, "FAN PWM duty : %d\n", ulvalue);
            if ( CMD_GetMsgUI() != NULL )
            {
                (int *)(CMD_GetMsgUI())(parser_fan_caMsg);
            }
			iResult = DRV_FAN_SetOnInterval((HUINT16)ulvalue);
			AssertDRV(iResult, CMD_ERR);
		}
		else
		{
			CMD_Printf("Wrong Command~! right command : fan duty [val(0~255)] \n");
			return CMD_ERR;
		}
	}
	else if CMD_IS("reg")
	{
		int ucSysTemp = 0;

		CMD_Printf("Command : fan reg -> register fan callback func.\n");

		iResult = DI_SYSTEM_RegisterEventNotify(DI_SYS_EVENT_CHECK_TEMP, P_FAN_CbCheckTemp);
		AssertDI(iResult, CMD_ERR);

		iResult = DI_SYSTEM_GetSystemInfo(DI_SYS_SMART_TEMP, &ucSysTemp, sizeof(ucSysTemp));
		AssertDI(iResult, CMD_ERR);

		CMD_Printf("ucSysTemp : %d by DI_SYSTEM_GetSystemInfo\n", ucSysTemp);
	}
	else if CMD_IS("reg_overtemp")
	{
		CMD_Printf("Command : overtemp reg -> register over temperature callback func.\n");

		iResult = DI_SYSTEM_RegisterEventNotify(DI_SYS_EVENT_OVER_TEMPERATURE, P_FAN_CbCheckOverTemp);
		AssertDI(iResult, CMD_ERR);

	}
	else if CMD_IS("temp")
	{
		int ucSysTemp = 0;

		CMD_Printf("Command : fan temp -> get hdd temperature by HDD SMART\n");

		iResult = DI_SYSTEM_GetSystemInfo(DI_SYS_SMART_TEMP, &ucSysTemp, sizeof(ucSysTemp));
		AssertDI(iResult, CMD_ERR);

		CMD_Printf("ucSysTemp : %d by DI_SYSTEM_GetSystemInfo\n", ucSysTemp);
	}
	else if CMD_IS("init")
	{
		CMD_Printf("Command : fan init -> init fan module.\n");

		DRV_FAN_Init();

		CMD_Printf("Fan Init Command Done.\n");
	}
	else if CMD_IS("uninit")
	{
		CMD_Printf("Command : fan uninit -> uninit fan module.\n");

		DRV_FAN_Uninit();

		CMD_Printf("Fan Uninit Command Done.\n");
	}
	else
	{
		HWTEST_PARAM1 = HWTEST_PARAM1;

		CMD_Printf("Command : fan ?? -> fan info/mode/volt/cw/period/duty/reg/temp/init/uninit \n");
	}

	return iResult;
}
#endif
#else
static void P_FAN_CbCheckTemp(void *pvParam)
{
	DI_SYS_FAN_INFO_t	*pstFanInfo;

	if(pvParam == NULL)
	{
		CMD_Printf("[%s]pvParam is NULL\n", __func__);
		return;
	}

	pstFanInfo = (DI_SYS_FAN_INFO_t *)pvParam;

	CMD_Printf("[%s] is CALLED :  ucTemp(%d)v ulFanCtrlVoltage(%d), ulFanFeedback_Hz(%d)\n",
					__func__, pstFanInfo->ucTemp, pstFanInfo->ulFanCtrlVoltage, pstFanInfo->ulFanFeedback_Hz);

	return;
}
static void P_FAN_CbCheckOverTemp(void *pvParam)
{
	DI_SYS_EVENT	eEvent;

	if(pvParam == NULL)
	{
		CMD_Printf("[%s]pvParam is NULL\n", __func__);
		return;
	}

	eEvent = *(DI_SYS_EVENT *)pvParam;

	CMD_Printf("[%s] is CALLED eEvent(%d):  \n",
					__func__,eEvent);

	return;
}
static char parser_fan_caMsg[200];
int CMD_HwTest_Parser_Fan(void *szArgs)
{
	GET_ARGS;

	if CMD_IS("info")
	{
		if PARAM_IS("off")
		{
			CMD_Printf("Command : fan info off -> PANNEL FAN Info Display OFF\n");

			g_bFanDispInfo = FALSE;

			iResult = DI_FRONT_PANEL_Display((HUINT8 *)" ");
			AssertDI(iResult, CMD_ERR);
		}
		else
		{


			CMD_Printf("Command : fan info on -> PANNEL FAN Info Display ON\n");

			g_bFanDispInfo = TRUE;
            VK_MEM_Memset(parser_fan_caMsg, 0x00, 200);
			iResult = DRV_FAN_DisplayInfo(parser_fan_caMsg);
            if ( CMD_GetMsgUI() != NULL )
            {
                (int *)(CMD_GetMsgUI())(parser_fan_caMsg);
            }
			AssertDRV(iResult, CMD_ERR);
		}
	}
	else if CMD_IS("mode")
	{
		if PARAM_IS("0")
		{
			CMD_Printf("Command : fan mode 0 -> AUTOMATIC FAN control by task\n");
            if ( CMD_GetMsgUI() != NULL )
            {
                (int *)(CMD_GetMsgUI())("Automatic FAN Control by task\n");
            }

			iResult = DRV_FAN_SetTestMode(FALSE);
			AssertDRV(iResult, CMD_ERR);
		}
		else
		{
			CMD_Printf("Command : fan mode x -> Manual FAN control by user\n");
            if ( CMD_GetMsgUI() != NULL )
            {
                (int *)(CMD_GetMsgUI())("Manual FAN Control by user\n");
            }

			iResult = DRV_FAN_SetTestMode(TRUE);
			AssertDRV(iResult, CMD_ERR);
		}
	}
	else if CMD_IS("volt")
	{
		int ulVoltage;
		if (READABLE_IN_DEC(HWTEST_PARAM, ulVoltage))
		{

			CMD_Printf("Command : fan volt %d -> FAN RUNNING In %d V\n", ulVoltage,ulVoltage);
            VK_MEM_Memset(parser_fan_caMsg, 0x00, 200);
			VK_sprintf(parser_fan_caMsg, "FAN is running in %d V\n", ulVoltage);
            if ( CMD_GetMsgUI() != NULL )
            {
                (int *)(CMD_GetMsgUI())(parser_fan_caMsg);
            }

			iResult = DRV_FAN_SetCtrlDetail(ulVoltage);
			AssertDRV(iResult, CMD_ERR);
		}
		else
		{
			CMD_Printf("Wrong Command~! right command : fan volt [val(<%d)] \n", FAN_VOLTAGE_MAX);
			return CMD_ERR;
		}
	}
	else if CMD_IS("cw")
	{
		int ulvalue;

		if (READABLE_IN_DEC(HWTEST_PARAM, ulvalue))
		{
			CMD_Printf("Command : fan cw 0x%04x -> SET PWM ControlWord %d\n", ulvalue);

			iResult = DRV_FAN_SetControlWord((HUINT16)ulvalue);
			AssertDRV(iResult, CMD_ERR);
		}
		else
		{
			CMD_Printf("Wrong Command~! right command : fan cw [val(0~32768))] \n");
			return CMD_ERR;
		}
	}
	else if CMD_IS("period")
	{
		int ulvalue;

		if (READABLE_IN_DEC(HWTEST_PARAM, ulvalue))
		{
			CMD_Printf("Command : fan period 0x%04x -> SET PWM period %d\n", ulvalue);

			iResult = DRV_FAN_SetPeriodInterval((HUINT16)ulvalue);
			AssertDRV(iResult, CMD_ERR);
		}
		else
		{
			CMD_Printf("Wrong Command~! right command : fan period [val(0~255)] \n");
			return CMD_ERR;
		}
	}
	else if CMD_IS("duty")
	{
		int ulvalue;

		if (READABLE_IN_DEC(HWTEST_PARAM, ulvalue))
		{
			CMD_Printf("Command : fan duty 0x%04x -> SET PWM duty %d\n", ulvalue);
            VK_MEM_Memset(parser_fan_caMsg, 0x00, 200);
			VK_sprintf(parser_fan_caMsg, "FAN PWM duty : %d\n", ulvalue);
            if ( CMD_GetMsgUI() != NULL )
            {
                (int *)(CMD_GetMsgUI())(parser_fan_caMsg);
            }
			iResult = DRV_FAN_SetOnInterval((HUINT16)ulvalue);
			AssertDRV(iResult, CMD_ERR);
		}
		else
		{
			CMD_Printf("Wrong Command~! right command : fan duty [val(0~255)] \n");
			return CMD_ERR;
		}
	}
	else if CMD_IS("reg")
	{
		int ucSysTemp = 0;

		CMD_Printf("Command : fan reg -> register fan callback func.\n");

		iResult = DI_SYSTEM_RegisterEventNotify(DI_SYS_EVENT_CHECK_TEMP, P_FAN_CbCheckTemp);
		AssertDI(iResult, CMD_ERR);

		iResult = DI_SYSTEM_GetSystemInfo(DI_SYS_SMART_TEMP, &ucSysTemp, sizeof(ucSysTemp));
		AssertDI(iResult, CMD_ERR);

		CMD_Printf("ucSysTemp : %d by DI_SYSTEM_GetSystemInfo\n", ucSysTemp);
	}
	else if CMD_IS("reg_overtemp")
	{
		CMD_Printf("Command : overtemp reg -> register over temperature callback func.\n");

		iResult = DI_SYSTEM_RegisterEventNotify(DI_SYS_EVENT_OVER_TEMPERATURE, P_FAN_CbCheckOverTemp);
		AssertDI(iResult, CMD_ERR);

	}
	else if CMD_IS("temp")
	{
		int ucSysTemp = 0;

		CMD_Printf("Command : fan temp -> get hdd temperature by HDD SMART\n");

		iResult = DI_SYSTEM_GetSystemInfo(DI_SYS_SMART_TEMP, &ucSysTemp, sizeof(ucSysTemp));
		AssertDI(iResult, CMD_ERR);

		CMD_Printf("ucSysTemp : %d by DI_SYSTEM_GetSystemInfo\n", ucSysTemp);
	}
	else if CMD_IS("init")
	{
		CMD_Printf("Command : fan init -> init fan module.\n");

		DRV_FAN_Init();

		CMD_Printf("Fan Init Command Done.\n");
	}
	else if CMD_IS("uninit")
	{
		CMD_Printf("Command : fan uninit -> uninit fan module.\n");

		DRV_FAN_Uninit();

		CMD_Printf("Fan Uninit Command Done.\n");
	}
	else
	{
		HWTEST_PARAM1 = HWTEST_PARAM1;

		CMD_Printf("Command : fan ?? -> fan info/mode/volt/cw/period/duty/reg/temp/init/uninit \n");
	}

	return iResult;
}
#endif
#endif

#if 0
extern BREG_Handle g_hReg;
int CMD_HwTest_Parser_Usb(void *szArgs)
{
	GET_ARGS;

	if CMD_IS("overcurrent")
	{
		if PARAM_IS("stop")
		{
			DI_UART_Print("Watching USB_OVERRENT stops\n");
			//g_ulHwTestTaskJob&=~TJ_USB_OVERCURRENT;
		}
		else
		{
			DI_UART_Print("Watching USB_OVERRENT starts\n");
			//g_ulHwTestTaskJob|=TJ_USB_OVERCURRENT;
		}
		iResult=CMD_OK;
	}
	else if CMD_IS("test")
	{
		if PARAM_IS("port")
		{
			int ulPortIndex;
			if (READABLE_IN_DEC(HWTEST_PARAM1, ulPortIndex))
			{
				unsigned long ulVal;
				CMD_Printf("BCIP : %d Port = %d \n", BCHP_CHIP, ulPortIndex);

#if (BCHP_CHIP == 7325)
				ulVal=BREG_Read32(g_hReg, BCHP_USB_EHCI_PORTSC_0);
				ulVal|=(1<<BCHP_USB_EHCI_PORTSC_0_SUSPEND_SHIFT);
				BREG_Write32(g_hReg, BCHP_USB_EHCI_PORTSC_0, ulVal);
				VK_TASK_Sleep(100);

				ulVal=BREG_Read32(g_hReg, BCHP_USB_EHCI_USBCMD);
				ulVal&=~BCHP_USB_EHCI_USBCMD_RUN_STOP_MASK;
				BREG_Write32(g_hReg, BCHP_USB_EHCI_USBCMD, ulVal);
				VK_TASK_Sleep(100);

				ulVal=BREG_Read32(g_hReg, BCHP_USB_EHCI_PORTSC_0);
				ulVal&=~BCHP_USB_EHCI_PORTSC_0_PORT_TEST_CONTROL_MASK;
				// pattern 4
				ulVal|=(4<<BCHP_USB_EHCI_PORTSC_0_PORT_TEST_CONTROL_SHIFT);
				BREG_Write32(g_hReg, BCHP_USB_EHCI_PORTSC_0, ulVal);

				iResult=CMD_OK;
#elif (BCHP_CHIP == 7335)
				if (ulPortIndex==0)
				{
					ulVal=BREG_Read32(g_hReg, BCHP_USB_EHCI_PORTSC_0);
					ulVal|=(1<<BCHP_USB_EHCI_PORTSC_0_SUSPEND_SHIFT);
					BREG_Write32(g_hReg, BCHP_USB_EHCI_PORTSC_0, ulVal);
					VK_TASK_Sleep(100);

					ulVal=BREG_Read32(g_hReg, BCHP_USB_EHCI_USBCMD);
					ulVal&=~BCHP_USB_EHCI_USBCMD_RUN_STOP_MASK;
					BREG_Write32(g_hReg, BCHP_USB_EHCI_USBCMD, ulVal);
					VK_TASK_Sleep(100);

					ulVal=BREG_Read32(g_hReg, BCHP_USB_EHCI_PORTSC_0);
					ulVal&=~BCHP_USB_EHCI_PORTSC_0_PORT_TEST_CONTROL_MASK;
					// pattern 4
					ulVal|=(4<<BCHP_USB_EHCI_PORTSC_1_PORT_TEST_CONTROL_SHIFT);
					BREG_Write32(g_hReg, BCHP_USB_EHCI_PORTSC_0, ulVal);

					iResult=CMD_OK;
				}
				else if (ulPortIndex==1)
				{
					ulVal=BREG_Read32(g_hReg, BCHP_USB_EHCI_PORTSC_1);
					ulVal|=(1<<BCHP_USB_EHCI_PORTSC_1_SUSPEND_SHIFT);
					BREG_Write32(g_hReg, BCHP_USB_EHCI_PORTSC_1, ulVal);
					VK_TASK_Sleep(100);

					ulVal=BREG_Read32(g_hReg, BCHP_USB_EHCI_USBCMD);
					ulVal&=~BCHP_USB_EHCI_USBCMD_RUN_STOP_MASK;
					BREG_Write32(g_hReg, BCHP_USB_EHCI_USBCMD, ulVal);
					VK_TASK_Sleep(100);

					ulVal=BREG_Read32(g_hReg, BCHP_USB_EHCI_PORTSC_1);
					ulVal&=~BCHP_USB_EHCI_PORTSC_1_PORT_TEST_CONTROL_MASK;
					// pattern 4
					ulVal|=(4<<BCHP_USB_EHCI_PORTSC_1_PORT_TEST_CONTROL_SHIFT);
					BREG_Write32(g_hReg, BCHP_USB_EHCI_PORTSC_1, ulVal);

					iResult=CMD_OK;
				}
#elif (BCHP_CHIP == 7346)
				if (ulPortIndex==0)
				{
					ulVal=FSREG_Read(g_hReg, BCHP_USB_EHCI_PORTSC_0);
					ulVal|=(1<<BCHP_USB_EHCI_PORTSC_0_SUSPEND_SHIFT);
					FSREG_Write(g_hReg, BCHP_USB_EHCI_PORTSC_0, ulVal);
					VK_TASK_Sleep(100);

					ulVal=FSREG_Read(g_hReg, BCHP_USB_EHCI_USBCMD);
					ulVal&=~BCHP_USB_EHCI_USBCMD_RUN_STOP_MASK;
					FSREG_Write(g_hReg, BCHP_USB_EHCI_USBCMD, ulVal);
					VK_TASK_Sleep(100);

					ulVal=FSREG_Read(g_hReg, BCHP_USB_EHCI_PORTSC_0);
					ulVal&=~BCHP_USB_EHCI_PORTSC_0_PORT_TEST_CONTROL_MASK;
					// pattern 4
					ulVal|=(4<<BCHP_USB_EHCI_PORTSC_0_PORT_TEST_CONTROL_SHIFT);
					FSREG_Write(g_hReg, BCHP_USB_EHCI_PORTSC_0, ulVal);

					iResult=CMD_OK;
				}
				else if (ulPortIndex==1)
				{
					ulVal=FSREG_Read(g_hReg, BCHP_USB1_EHCI_PORTSC_0);
					ulVal|=(1<<BCHP_USB_EHCI_PORTSC_0_PORT_TEST_CONTROL_SHIFT);
					FSREG_Write(g_hReg, BCHP_USB1_EHCI_PORTSC_0, ulVal);
					VK_TASK_Sleep(100);

					ulVal=FSREG_Read(g_hReg, BCHP_USB_EHCI_USBCMD);
					ulVal&=~BCHP_USB_EHCI_USBCMD_RUN_STOP_MASK;
					FSREG_Write(g_hReg, BCHP_USB_EHCI_USBCMD, ulVal);
					VK_TASK_Sleep(100);

					ulVal=FSREG_Read(g_hReg, BCHP_USB1_EHCI_PORTSC_0);
					ulVal&=~BCHP_USB_EHCI_PORTSC_0_PORT_TEST_CONTROL_MASK;
					// pattern 4
					ulVal|=(4<<BCHP_USB_EHCI_PORTSC_0_PORT_TEST_CONTROL_SHIFT);
					FSREG_Write(g_hReg, BCHP_USB1_EHCI_PORTSC_0, ulVal);

					iResult=CMD_OK;
				}
#elif (BCHP_CHIP == 7405)
				if (ulPortIndex==0)
				{
					ulVal=BREG_Read32(g_hReg, BCHP_USB_EHCI_PORTSC_0);
					ulVal|=(1<<BCHP_USB_EHCI_PORTSC_0_SUSPEND_SHIFT);
					BREG_Write32(g_hReg, BCHP_USB_EHCI_PORTSC_0, ulVal);
					VK_TASK_Sleep(100);

					ulVal=BREG_Read32(g_hReg, BCHP_USB_EHCI_USBCMD);
					ulVal&=~BCHP_USB_EHCI_USBCMD_RUN_STOP_MASK;
					BREG_Write32(g_hReg, BCHP_USB_EHCI_USBCMD, ulVal);
					VK_TASK_Sleep(100);

					ulVal=BREG_Read32(g_hReg, BCHP_USB_EHCI_PORTSC_0);
					ulVal&=~BCHP_USB_EHCI_PORTSC_0_PORT_TEST_CONTROL_MASK;
					// pattern 4
					ulVal|=(4<<BCHP_USB_EHCI_PORTSC_0_PORT_TEST_CONTROL_SHIFT);
					BREG_Write32(g_hReg, BCHP_USB_EHCI_PORTSC_0, ulVal);

					iResult=CMD_OK;
				}
				else if (ulPortIndex==1)
				{
					ulVal=BREG_Read32(g_hReg, BCHP_USB_EHCI_PORTSC_1);
					ulVal|=(1<<BCHP_USB_EHCI_PORTSC_1_SUSPEND_SHIFT);
					BREG_Write32(g_hReg, BCHP_USB_EHCI_PORTSC_1, ulVal);
					VK_TASK_Sleep(100);

					ulVal=BREG_Read32(g_hReg, BCHP_USB_EHCI_USBCMD);
					ulVal&=~BCHP_USB_EHCI_USBCMD_RUN_STOP_MASK;
					BREG_Write32(g_hReg, BCHP_USB_EHCI_USBCMD, ulVal);
					VK_TASK_Sleep(100);

					ulVal=BREG_Read32(g_hReg, BCHP_USB_EHCI_PORTSC_1);
					ulVal&=~BCHP_USB_EHCI_PORTSC_1_PORT_TEST_CONTROL_MASK;
					// pattern 4
					ulVal|=(4<<BCHP_USB_EHCI_PORTSC_1_PORT_TEST_CONTROL_SHIFT);
					BREG_Write32(g_hReg, BCHP_USB_EHCI_PORTSC_1, ulVal);

					iResult=CMD_OK;
				}
#else
				CMD_Printf("Command : usb, invalid chip.\n");
#endif
			}
		}
	}

	return iResult;
}
#endif

#if	defined(CONFIG_CI)
CAMINFO	camInfo;

#if defined(CONFIG_PRODUCT_IMAGE_FACTORY) && !defined(CONFIG_USE_EXTERNAL_CI_CONTROLLER)
static volatile unsigned char  Command_Register=0;
static volatile unsigned char  Status_Register=0;
static volatile unsigned char  Size_LS_Register=0;
static volatile unsigned char  Size_MS_Register=0;
static DRVPhys DRVTab[2];

int GetCisLen(unsigned short slotId, unsigned short *addrCcrs)
{
	int i;
	int iRet;							/* return code	*/
	DRV_stMem mem;					/* driver parameters */
	unsigned char cis[8];
	unsigned char radrSize;

	mem.len    = 8;
	mem.pbytes = cis;

	for(i = DRV_MEMADDR; i < MAXADDR; )
	{
		mem.addr = i;
		iRet = DRV_CI_Ioctl(slotId, DRV_READMEM, &mem);
		if (iRet < 0)
			return -1;		/* error while reading CIS */
		if(cis[0] == CISTPL_CONFIG)
		{
			radrSize = cis[2] & TPCC_RASZ;			/* number of bytes of Configuration Register Base Address */
			if (radrSize > NBRBYTES)				/* Control : number = 0 or 1 means 1 or 2 bytes --> address between 0 and 0xfff */
			{
				return -1;						/* ======= wrong Configuration Register Base Address */
			}
			if (radrSize == 0)
				*addrCcrs = cis[4];
			else
				*addrCcrs = cis[4] | (cis[5] << 8);
			if (*addrCcrs > MAXADDR)				/* Control */
			{
				return -1;						/* ======= wrong Configuration Register Base Address */
			}
			return 0;
		}
		else
		{
			if(cis[0] == 0xff || !cis[1] || cis[1] == 0xff)
				return -1;
			i += cis[1] + 2;
		}

	}
	return -1;
}

int P_HwTest_SearchTuple(unsigned mtuple, unsigned char tuple, unsigned char **pcis, int *len, int *first, int *itpcc_radr, int *dvb, int *last)
{
	int ocis;						/* current offset in the CIS */
	int link;						/* tuple link				 */
	int found;						/* 1 if the current tuple is found, else 0 */
	unsigned char ctuple;			/* current tuple			 */
	int sublen; 					/* intermediate length		 */
	unsigned char *subpcis; 		/* pointer to subtuple		 */
	int nb, nbt0 = 0, nbt1 = 0, i;	/* intermediate variables	 */
	int ctpce_number = 0;			/* Configuration Index Value */
	char ctpcc_sz;					/* tuple's elementary item   */
	char lastentry; 				/* 1 if the last entry is found */
	char ctpcc_rasz;
	char ctpcc_rmsz;
	char cpce_pd;
	int ciplus = 0;

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
					CMD_Printf("PHYS_Tuple : not found or wrong TPLLV1_MAJOR\n");
					return -1;	/* wrong TPLLV1_MAJOR */			/* ======= */
				}
				if (*(*pcis + TPLLV1_MINOR) != TPLLV1_MINOR_VAL)	/* Control */
				{
					CMD_Printf("PHYS_Tuple : not found or wrong TPLLV1_MINOR\n");
					return -1;	/* wrong TPLLV1_MINOR */			/* ======= */
				}
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
						if (VK_strstr(info_str, "ciplus=1"))
						{
							ciplus = 1;
						}
					}
				}
				break;

			case CISTPL_CONFIG :
				ctpcc_sz = *(*pcis + TPCC_SZ);
				ctpcc_rasz = ctpcc_sz & TPCC_RASZ;		/* number of bytes of Configuration Register Base Address */
				if (ctpcc_rasz > NBRBYTES)				/* Control : number = 0 or 1 means 1 or 2 bytes --> address between 0 and 0xfff */
				{
					CMD_Printf("PHYS_Tuple : wrong Configuration Register Base Address (ctpcc_rasz = 0x%x)\n", ctpcc_rasz);
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
					CMD_Printf("PHYS_Tuple : wrong Configuration Register Base Address (*itpcc_radr = 0x%x)\n", *itpcc_radr);
					return -1;							/* ======= wrong Configuration Register Base Address */
				}
				if (! ((*(*pcis + TPCC_RADR + ctpcc_rasz + 1)) & TPCC_RMSK))	/* Control */
				{
					CMD_Printf("PHYS_Tuple : COR not present\n");
					return -1;							/* COR not present		   ======= */
				}

				nb = TPCC_RADR + ctpcc_rasz + 1 + ctpcc_rmsz + 1;
				subpcis = *pcis + nb;
				sublen = *len - ocis - (link - nb);

				/* control CCST_CIF subtuple */
				if (P_HwTest_SearchTuple(tuple, CCST_CIF, &subpcis, &sublen, first, itpcc_radr, dvb, last) < 0)
				{
					CMD_Printf("PHYS_Tuple : not found or wrong CCST_CIF\n");
					return -1;		/* Control not found or wrong CCST_CIF */
				}
				break;				/* =======							   */

			case CISTPL_CFTABLE_ENTRY :
				if (*first & !((*(*pcis + TPCE_INDX)) & TPCE_INDX_MASK))	/* Intface bit and Default bit must be present in the first entry */
				{
					CMD_Printf("PHYS_Tuple : not found or wrong CISTPL_CFTABLE_ENTRY\n");
					return -1;		/* Control wrong first CISTPL_CFTABLE_ENTRY tuple */
				}
									/* =======										  */
				ctpce_number = *(*pcis + TPCE_INDX) & ~TPCE_INDX_MASK;	/* configuration Entry Number to write to the COR */

				*first = 0;

				if (*(*pcis + TPCE_INDX) & TPCE_INTFACE)
				{
					if (*(*pcis + TPCE_IF) != TPCE_IF_VAL)				/* Control : TPCE_IF must be equal to TPCE_IF_VAL */
					{													/* =======										  */
						lastentry = (char)-2; 							/* wrong CISTPL_CFTABLE_ENTRY tuple */
						break;
					}
				}
				else
				{
					CMD_Printf("PHYS_Tuple : not found or wrong TPCE_IF\n");
					lastentry = (char)-2; 	/* Control : wrong CISTPL_CFTABLE_ENTRY tuple */
					break;					/* =======									  */
				}

				if (!(*(*pcis + TPCE_FS) & TPCE_FS_MASK))	/* Control : I/O Space and power must be present */
				{											/* =======										 */
					CMD_Printf("PHYS_Tuple : not found or wrong TPCE_FS_MASK\n");
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
				if (P_HwTest_SearchTuple(tuple, STCE_EV, &subpcis, &sublen, first, itpcc_radr, dvb, last) < 0)
				{						/* Control */
					lastentry = (char)-2; 	/* ======= not found or wrong STCE_EV */
					break;
				}

				/* Control the STCE_PD subtuple */
				if (P_HwTest_SearchTuple(tuple, STCE_PD, &subpcis, &sublen, first, itpcc_radr, dvb, last) < 0)
				{						/* Control */
					lastentry = (char)-2; 	/* ======= not found or wrong STCE_PD */
					break;
				}

				*dvb = ctpce_number;		/* the DVB entry is present */

				break;

			case CISTPL_NO_LINK :
				if (link != CISTPL_NOLINKLEN)	/* Control */
				{
					CMD_Printf("PHYS_Tuple : not found or wrong CISTPL_NOLINKLEN\n");
					return -1;					/* ======= wrong CISTPL_NO_LINK tuple */
				}
				break;

			/* subtuples */
			case CCST_CIF : 		/* if mtuple = CISTPL_CFTABLE_ENTRY then this tuple is STCE_EV */
				if (mtuple == CISTPL_CFTABLE_ENTRY)
				{	/* STCE_EV */
					if (VK_strcmp((char*)*pcis, STCE_EV_VAL)) 	/* Control */
					{
						CMD_Printf("PHYS_Tuple : not found or wrong STCE_EV_VAL\n");
						return -1;						/* ======= wrong STCE_EV subtuple */
					}
				}
				else
				{	/* CCST_CIF */
					if (**pcis != CCST_CIF1)	/* Control */
					{
						CMD_Printf("PHYS_Tuple : not found or wrong CCST_CIF1\n");
						return -1;				/* ======= wrong CCST_CIF subtuple */
					}
					if (*(*pcis + 1) != CCST_CIF2)	/* Control */
					{
						CMD_Printf("PHYS_Tuple : not found or wrong CCST_CIF2\n");
						return -1;					/* ======= wrong CCST_CIF subtuple */
					}

					/* STCI_STR must be "DVB_CI_Vx.xx" where x is digit */
					nb = VK_strlen(STCI_STR);
					if (VK_strncmp((char*)(*pcis + 2), STCI_STR, nb))	/* Control */
					{
						CMD_Printf("PHYS_Tuple : not found or wrong STCI_STR\n");
						return -1;							/* ======= wrong CCST_CIF subtuple */
					}
					if (!VK_isdigit(*(*pcis + 2 + nb)))		/* Control */
					{
						CMD_Printf("PHYS_Tuple : not found or wrong CCST_CIF subtuple - %d line\n", __LINE__);
						return -1;							/* ======= wrong CCST_CIF subtuple */
					}
					if (*(*pcis + 2 + nb + 1) != STCI_STRP) /* Control */
					{
						CMD_Printf("PHYS_Tuple : not found or wrong STCI_STRP\n");
						return -1;							/* ======= wrong CCST_CIF subtuple */
					}
					if (!VK_isdigit(*(*pcis + 2 + nb + 2)))	/* Control */
					{
						CMD_Printf("PHYS_Tuple : not found or wrong CCST_CIF subtuple - %d line\n", __LINE__);
						return -1;							/* ======= wrong CCST_CIF subtuple */
					}
					if (!VK_isdigit(*(*pcis + 2 + nb + 3)))	/* Control */
					{
						CMD_Printf("PHYS_Tuple : not found or wrong CCST_CIF subtuple - %d line\n", __LINE__);
						return -1;							/* ====== wrong CCST_CIF subtuple */
					}
				}
				break;

			case STCE_PD :
				if (VK_strcmp((char*)(*pcis), STCE_PD_VAL)) /* Control */
				{
					CMD_Printf("PHYS_Tuple : not found or wrong STCE_PD_VAL\n");
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
			CMD_Printf("PHYS_Tuple : tuple not found (ocis=%d, len=%d)\n", ocis, *len);
			return -1;		/* ======= tuple not found */
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
	else if (tuple == CISTPL_VERS_1)
	{
		if (ciplus == 1)
			return 2;
		else
			return 1;
	}
	else
		return 1;
}


int P_HwTest_CheckCIS(unsigned char *cis, int len, unsigned short *addrcor, unsigned char *cor, unsigned char *ciplus)
{
	unsigned char *pcis;	/* pointer to the remainding CIS	*/
	int lcis;				/* length of the remainding CIS 	*/
	int first;				/* 1 for the first configuration table entry, else 0 */
	int addr;				/* configuration registers base address */
	int dvb;				/* index number of the DVB entry	*/
	int last;				/* index number of the last entry	*/
	int iRet;				/* return code						*/

	dvb = 0;

	pcis = cis;
	lcis = len;

	if (P_HwTest_SearchTuple(0, CISTPL_DEVICE_0A, &pcis, &lcis, &first, &addr, &dvb, &last) < 0)
	{
		CMD_Printf("PHYS_CheckCIS : not found or wrong CISTPL_DEVICE_0A\n");
		return -1;		/* not found or wrong CISTPL_DEVICE_0A		*/
	}

	if (P_HwTest_SearchTuple(0, CISTPL_DEVICE_0C, &pcis, &lcis, &first, &addr, &dvb, &last) < 0)
	{
		CMD_Printf("PHYS_CheckCIS : not found or wrong CISTPL_DEVICE_0C\n");
		return -1;		/* not found or wrong CISTPL_DEVICE_0C		*/
	}

	if ((iRet = P_HwTest_SearchTuple(0, CISTPL_VERS_1, &pcis, &lcis, &first, &addr, &dvb, &last)) < 0)
	{
		CMD_Printf("PHYS_CheckCIS : not found or wrong CISTPL_VERS_1\n");
		return -1;		/* not found or wrong CISTPL_VERS_1 		*/
	}

	if (iRet == 2)
		*ciplus = 1;
	else
		*ciplus = 0;

	if (P_HwTest_SearchTuple(0, CISTPL_MANFID, &pcis, &lcis, &first, &addr, &dvb, &last) < 0)
	{
		CMD_Printf("PHYS_CheckCIS : not found or wrong CISTPL_MANFID\n");
		return -1;		/* not found or wrong CISTPL_MANFID 		*/
	}

	if (P_HwTest_SearchTuple(0, CISTPL_CONFIG, &pcis, &lcis, &first, &addr, &dvb, &last) < 0)
	{
		CMD_Printf("PHYS_CheckCIS : not found or wrong CISTPL_CONFIG\n");
		return -1;		/* not found or wrong CISTPL_CONFIG 		*/
	}

	*addrcor = addr;
	first = 1;
	do
	{
		iRet = P_HwTest_SearchTuple(0, CISTPL_CFTABLE_ENTRY, &pcis, &lcis, &first, &addr, &dvb, &last);
	}
	while ((iRet == 0) || (iRet == -2));		/* one more entry */
	if (iRet < 0)
	{
		CMD_Printf("PHYS_CheckCIS : not found or wrong CISTPL_CFTABLE_ENTRY\n");
		return -1;		/* not found or wrong CISTPL_CFTABLE_ENTRY	*/
	}

	if (P_HwTest_SearchTuple(0, CISTPL_NO_LINK, &pcis, &lcis, &first, &addr, &dvb, &last) < 0)
	{
		CMD_Printf("PHYS_CheckCIS : not found or wrong CISTPL_NO_LINK\n");
		return -1;		/* not found or wrong CISTPL_NO_LINK 		*/
	}

	if (P_HwTest_SearchTuple(0, CISTPL_END, &pcis, &lcis, &first, &addr, &dvb, &last) < 0)
	{
		CMD_Printf("PHYS_CheckCIS : not found or wrong CISTPL_END\n");
		return -1;		/* not found or wrong CISTPL_END			*/
	}

	if (dvb != last)
	{
		CMD_Printf("PHYS_CheckCIS : DVB entry found but... (dvb=%x, last=%x)\n", dvb, last);
	}

	*cor = dvb;

	return 1;			/* good CIS */
}

int CamMemoryInitializeTest(unsigned short slotId)
{
	unsigned char *cis; 			/* CIS buffer		 */
	unsigned short addrcor; 		/* COR Address		 */
	unsigned char cor;				/* COR value		 */
	unsigned char ciplus;			/* CI+ CAM detected  */
	int iRet;						/* return code		 */
	DRV_stMem mem;					/* driver parameters */
	unsigned short cisLen;

	unsigned short addrccrs;
	if (GetCisLen(slotId, &addrccrs) < 0)
	{
		CMD_Printf("Wrong CIS\n");
		return -1;		/* wrong CIS */
	}
	cisLen = addrccrs >> 1;

	cis = (unsigned char *)DD_MEM_Alloc(cisLen);
	if(!cis)
	{
		CMD_Printf("PHYS_CIS : not enought memory (%d) !!!\n", cisLen);
		return -1;
	}

	mem.addr   = DRV_MEMADDR;
	mem.len    = cisLen;
	mem.pbytes = cis;
	iRet = DRV_CI_Ioctl(slotId, DRV_READMEM, &mem);
	if (iRet < 0)
	{
		DD_MEM_Free(cis);
		CMD_Printf("PHYS_CIS : DRV_Ioctl error (%d) !!!\n", iRet);
		return -1; 	/* error while reading CIS */
	}

	if (P_HwTest_CheckCIS(cis, mem.rlen, &addrcor, &cor, &ciplus) < 0)
	{
		CMD_Printf(("PHYS_CIS : PHYS_CheckCIS reports wrong CIS !!!\n"));
		DD_MEM_Free(cis);
		return -1;		/* wrong CIS */
	}
	DD_MEM_Free(cis);

	/* the CIS is OK, addrcor contains the COR Address and cor the COR value */
	mem.addr   = addrcor >> 1;
	mem.len    = 1; 	/* one byte */
	mem.pbytes = &cor;

	iRet = DRV_CI_Ioctl(slotId, DRV_WRITEMEM, &mem);
	if ((iRet >= 0) && (mem.rlen == 1))
		return 0;
	else
		return -1; /* error while writing the COR */
}

unsigned char P_HwTest_CI_ReadByte(int index, int Reg, unsigned char *data)
{
	DRV_stSignal signal;/* DRV_Ioctl argument */
	int Ret;			/* return code*/
	DRV_stIO reg;	/* Ioctl parameter */

	signal.sig = DRV_CARD_DETECT;
	Ret = DRV_CI_Ioctl(DRVTab[index].Slot_id, DRV_TSIGNAL, &signal);
	if (!((Ret > 0) && (signal.value)))
	{
		CMD_Printf("PHYS_ReadByte : PHYS_CD error !!!\n");
		return W_SEACH;
	}
	reg.registr = Reg;
	reg.pvalue  = data;
	if (DRV_CI_Ioctl(DRVTab[index].Slot_id, DRV_READIO, &reg) < 0)
	{
		CMD_Printf("PHYS_ReadByte : DRV_CI_Ioctl error !!!\n");
		DRVTab[index].State = DRV_IDLE;
		return(W_SEACH);
	}
	else
	{
		return(OK);
	}
}

unsigned char P_HwTest_CI_WriteByte(int index, int Reg, unsigned char data)
{
	DRV_stSignal signal;/* DRV_Ioctl argument */
	int Ret;			/* return code*/
	DRV_stIO reg;	/* Ioctl parameter */

	signal.sig = DRV_CARD_DETECT;
	Ret = DRV_CI_Ioctl(DRVTab[index].Slot_id, DRV_TSIGNAL, &signal);
	if (!((Ret > 0) && (signal.value)))
	{
		CMD_Printf("PHYS_ReadByte : PHYS_CD error !!!\n");
		return W_SEACH;
	}
	reg.registr = Reg;
	reg.pvalue  = &data;
	if (DRV_CI_Ioctl(DRVTab[index].Slot_id, DRV_WRITEIO, &reg) < 0)
	{
		CMD_Printf("PHYS_WriteByte : DRV_CI_Ioctl error !!!\n");
		DRVTab[index].State = DRV_IDLE;
		return(W_SEACH);
	}
	else
	{
		return(OK);
	}
}

static unsigned char uc_Expected_Tsb[6] = {0x01, 0x00, 0x80, 0x02, 0x01, 0x00};

unsigned char P_HwTest_CI_SendData(int index, unsigned char *pData, unsigned short Size_in)
{
	if(DRVTab[index].State == DRV_IDLE) return W_SEACH;

	Status_Register = 0x00;
	if(P_HwTest_CI_ReadByte(index, OFF_STA_REG, (unsigned char *)(&Status_Register))==W_SEACH)
	{
		return(W_SEACH);
	}
#if 0
	if (Status_Register & DA)
	{
		PHYS_ReceiveIdle(index);
	}
#endif
	Command_Register = HC;

	if(P_HwTest_CI_WriteByte(index, OFF_COM_REG, Command_Register)==W_SEACH) return(W_SEACH);

	/* Test FR */
	Status_Register = 0x00;
	if(P_HwTest_CI_ReadByte(index, OFF_STA_REG, (unsigned char *)(&Status_Register))==W_SEACH)
	{
		Command_Register = 0x00;
		if(P_HwTest_CI_WriteByte(index, OFF_COM_REG, Command_Register)==W_SEACH)
		{
			return(W_SEACH);
		}
		return(W_SEACH);
	}

	if(!(Status_Register & FR)) /* if dvb module is busy */
	{
		Command_Register = 0x00;
		if(P_HwTest_CI_WriteByte(index, OFF_COM_REG, Command_Register)==W_SEACH)
		{
			return(W_SEACH);
		}
		return(M_BUSY);
	}

	/* write data size */
	if(P_HwTest_CI_WriteByte(index, OFF_SIZE_LS_REG, ( unsigned char)Size_in)==W_SEACH)
	{
		return(W_SEACH);
	}
	if(P_HwTest_CI_WriteByte(index, OFF_SIZE_MS_REG, ( unsigned char)(Size_in>>8))==W_SEACH)
	{
		return(W_SEACH);
	}

	/* write data */
	/* ========== */
	if (DRV_CI_Write(DRVTab[index].Slot_id, Size_in, pData) != (int)Size_in)
	{
		return(W_SEACH);
	}

	/* Verify if WE is down */
	Status_Register = 0x00;
	if(P_HwTest_CI_ReadByte(index, OFF_STA_REG, (unsigned char *)(&Status_Register))==W_SEACH)
	{
		return(W_SEACH);
	}

	if(Status_Register & WE)
	{
		Command_Register = 0x00;
		if(P_HwTest_CI_WriteByte(index, OFF_COM_REG, Command_Register)==W_SEACH)
		{
			return(W_SEACH);
		}
		return(T_ERR);
	}

	Command_Register = 0x00;
	if(P_HwTest_CI_WriteByte(index, OFF_COM_REG, Command_Register)==W_SEACH)
	{
		return(W_SEACH);
	}

	uc_Expected_Tsb[0] = uc_Expected_Tsb[4] = pData[0];

	return(OK);
}

unsigned char P_HwTest_CI_ReceiveData(int index, unsigned char *pData, unsigned short *Size_out, int flag)
{
	SLPDU *p;

	if(DRVTab[index].State == DRV_IDLE)
	{
		return W_SEACH;
	}

	if (flag && DRVTab[index].firstSLPDU)
	{
		/* Received data while sending previous data */
		VK_memcpy(pData, DRVTab[index].firstSLPDU->pLPDU, DRVTab[index].firstSLPDU->length);
		*Size_out = DRVTab[index].firstSLPDU->length;

		/* free the received buffer into the linked list and update the head */
		p = DRVTab[index].firstSLPDU;
		DRVTab[index].firstSLPDU = (SLPDU *)(DRVTab[index].firstSLPDU->next);
		DD_MEM_Free((unsigned char *)p);
		return(OK);
	}

	Status_Register = 0x00;
	if(P_HwTest_CI_ReadByte(index, OFF_STA_REG, (unsigned char *)(&Status_Register))==W_SEACH)
	{
		return(W_SEACH);
	}

	if(!(Status_Register & DA))
	{
		return(M_DATA); /* if no data available */
	}

	/* data size */
	Size_LS_Register = Size_MS_Register = 0;
	if(P_HwTest_CI_ReadByte(index, OFF_SIZE_LS_REG, (unsigned char *)(&Size_LS_Register))==W_SEACH)
	{
		return(W_SEACH);
	}
	if(P_HwTest_CI_ReadByte(index, OFF_SIZE_MS_REG, (unsigned char *)(&Size_MS_Register))==W_SEACH)
	{
		return(W_SEACH);
	}

	*Size_out = ((unsigned short)Size_MS_Register<<8) & 0xff00;
	*Size_out |= (unsigned short)(Size_LS_Register & 0x00ff);
	if((DRVTab[index].State == DRV_CONNECT) && (*Size_out > DRVTab[index].Negociated_size))
	{
		CMD_Printf("[CI_RW_ERROR] PHYS_ReceiveData : read size invalid %d > %d -> PAL_CI_ResetModule(%d)...\n",
			*Size_out, DRVTab[index].Negociated_size, DRVTab[index].Slot_id);


		if (DI_CI_ResetModule(DRVTab[index].Slot_id) != 0x0)
		{
			CMD_Printf("[CI_RW_ERROR] PHYS_ReceiveData : fail to reset slot %d !!!\n", DRVTab[index].Slot_id);
		}
		return(W_SEACH);
	}

	if (*Size_out == 0xffff)
	{
		CMD_Printf("[CI_RW_ERROR] PHYS_ReceiveData : read size invalid 0xffff !!!\n");
		return(W_SEACH);
	}
	if (*Size_out > MAXLNSIZE)
	{
		CMD_Printf("[CI_RW_ERROR] PHYS_ReceiveData : read size invalid %d > %d !!!\n", *Size_out, MAXLNSIZE);
		return(W_SEACH);
	}

	/* read data */
	/* ========= */
	if (DRV_CI_Read(DRVTab[index].Slot_id, *Size_out, pData) != (int)*Size_out)
	{
		return(W_SEACH);
	}

	Status_Register = 0x00;
	if(P_HwTest_CI_ReadByte(index, OFF_STA_REG, (unsigned char *)(&Status_Register))==W_SEACH)
	{
		return(W_SEACH);
	}

	if (Status_Register & DA)
	{
		CMD_Printf("PHYS_ReceiveData : DA\n");
//		return(M_DATA); /* <TODO_CI> : 이런 경우에 이 플로우에서 데이터를 다시 가져오도록 하는 것이 좋을 것 같은데... 좀더 고려 필요... */
	}

	if (Status_Register & RE)
	{
		CMD_Printf("[CI_RW_ERROR] PHYS_ReceiveData : RE !!!\n");
//		return(T_ERR); /* <TODO_CI> : 이런 경우에 대한 처리에 대해선 좀더 고려 필요... */
	}

	if (*Size_out == 6 // length 가 6 으로써 캠에서 보내는 TPDU 는 따져보면 Tsb 밖에 없다.
		&&
			((pData[0] != 0x01 && pData[0] != 0x02)
			|| (pData[1] != 0x00 && pData[1] != 0x01)
			|| (pData[2] != 0x80 /* && pData[2] != 0x83 && pData[2] != 0x84 && pData[2] != 0x85 && pData[2] != 0x86 && pData[2] != 0xA0 && pData[2] != 0xA1 */)
			|| (pData[3] != 0x02)
			|| (pData[4] != pData[0])
			|| (pData[5] != 0x00 && pData[5] != 0x80)
			)
		)
	{
		// 일단 에러 찍어 주고...
		CMD_Printf("[CI_RW_ERROR] Read I/O mem failed : wrong Tsb = [ %02x %02x %02x %02x %02x %02x ] -> expected [ %02x %02x %02x %02x %02x (00 or 80) ] !!!\n",
			pData[0], pData[1], pData[2], pData[3], pData[4], pData[5],
			uc_Expected_Tsb[0], uc_Expected_Tsb[1], uc_Expected_Tsb[2], uc_Expected_Tsb[3], uc_Expected_Tsb[4]);

		// 기대되는 T_SB 값으로 강제 변경
		VK_memcpy(pData, uc_Expected_Tsb, 6);
	}

	return(OK);
}

int CamIOInitializeTest(int index)
{
	unsigned char	buff[MAXLNSIZE];
	unsigned short	Size;
	unsigned char	status;
	int 			count;
	DRV_ssSignal	signal;	/* DRV_Ioctl argument */
	int				Ret;
	unsigned int Host_Size = MAXLNSIZE;
	unsigned short NegociatedBufferSize;
	unsigned short *Negociate_Size = &NegociatedBufferSize;

	/* CLEAR the EMSTREAM pin */
	signal.cs  = 0;
	signal.pin = DRV_EMSTREAM;
	Ret = DRV_CI_Ioctl(DRVTab[index].Slot_id, DRV_SSIGNAL, &signal);
	if (Ret < 0)
	{
		CMD_Printf("PHYS_ResetModule : Error at %d lines !!!\n", __LINE__);
		return -1;
	}

	Command_Register = RS;
	if(P_HwTest_CI_WriteByte(index, OFF_COM_REG, Command_Register) == W_SEACH)
	{
		CMD_Printf("PHYS_ResetModule : Error at %d lines !!!\n", __LINE__);
		return -1;
	}

	Command_Register = 0x00;
	if(P_HwTest_CI_WriteByte(index, OFF_COM_REG, Command_Register) == W_SEACH)
	{
		CMD_Printf("PHYS_ResetModule : Error at %d lines !!!\n", __LINE__);
		return -1;
	}

	count=0;
	while(1)
	{
		Status_Register = 0x00;
		if(P_HwTest_CI_ReadByte(index, OFF_STA_REG, (unsigned char *)(&Status_Register)) == W_SEACH)
		{
			CMD_Printf("PHYS_ResetModule : Error at %d lines !!!\n", __LINE__);
			return -1;
		}

		if(Status_Register & FR)
			break;

		VK_TASK_Sleep(PHYS_DELAY);
		count++;
		if(count == PHYS_TIMEOUT)
		{
			CMD_Printf("PHYS_ResetModule : Error at %d lines !!!\n", __LINE__);
			return -1;
		}
	}

	Command_Register = SR;
	if(P_HwTest_CI_WriteByte(index, OFF_COM_REG, Command_Register) == W_SEACH)
	{
		CMD_Printf("PHYS_ResetModule : Error at %d lines !!!\n", __LINE__);
		return -1;
	}

	count=0;
	while(1)
	{
		buff[0] = buff[1] = 0; // VK_memset(buff, 0, MAXLNSIZE);
		if((status=P_HwTest_CI_ReceiveData(index, buff, &Size, 1)) == W_SEACH)
		{
			CMD_Printf("PHYS_ResetModule : Error at %d lines !!!\n", __LINE__);
			return -1;
		}

		if(status != M_DATA && status != M_BUSY)
			break;

		VK_TASK_Sleep(PHYS_DELAY);
		count++;
		if(count == PHYS_TIMEOUT)
		{
			CMD_Printf("PHYS_ResetModule : Error at %d lines !!!\n", __LINE__);
			return -1;
		}
	}

	(*Negociate_Size) = buff[0] * 256 + buff[1];

	VK_printf("PHYS_ResetModule : Size = %d, Negociate_Size = %d, Host_Size = %d\n", Size, *Negociate_Size, Host_Size);

	if (Size != 2)
	{
		CMD_Printf("[CI_RW_ERROR] PHYS_ResetModule : Wrong Size %d !!!\n", Size);
	}

	if((*Negociate_Size) > Host_Size)
	{
		CMD_Printf("[CI_RW_ERROR] PHYS_ResetModule : Negociate_Size = %d > Host_Size = %d !!!\n", *Negociate_Size, Host_Size); // 이 경우 지원 가능하다면 Host_Size (MAXLNSIZE) 를 늘려주는 것이 좋다.
		buff[0] = Host_Size >> 8;
		buff[1] = Host_Size;
		(*Negociate_Size) = Host_Size;
	}

	Command_Register = 0x00;
	if(P_HwTest_CI_WriteByte(index, OFF_COM_REG, Command_Register) == W_SEACH)
	{
		CMD_Printf("PHYS_ResetModule : Error at %d lines !!!\n", __LINE__);
		return -1;
	}

	Command_Register = SW;
	if(P_HwTest_CI_WriteByte(index, OFF_COM_REG, Command_Register) == W_SEACH)
	{
		CMD_Printf("PHYS_ResetModule : Error at %d lines !!!\n", __LINE__);
		return -1;
	}

	/* send the buffer size to the module */
	count=0;
	do
	{
		if((status=P_HwTest_CI_SendData(index, buff, 2)) == W_SEACH)
		{
			CMD_Printf("PHYS_ResetModule : Error at %d lines !!!\n", __LINE__);
			return -1;
		}

		VK_TASK_Sleep(PHYS_DELAY);
		count++;
		if(count == PHYS_TIMEOUT)
		{
			CMD_Printf("PHYS_ResetModule : Error at %d lines !!!\n", __LINE__);
			return -1;
		}

	} while(status == M_BUSY);

	Command_Register = 0x00;
	if(P_HwTest_CI_WriteByte(index, OFF_COM_REG, Command_Register) == W_SEACH)
	{
		CMD_Printf("PHYS_ResetModule : Error at %d lines !!!\n", __LINE__);
		return -1;
	}

	count=0;
	while(1)
	{
		Status_Register = 0x00;
		if(P_HwTest_CI_ReadByte(index, OFF_STA_REG, (unsigned char *)(&Status_Register)) == W_SEACH)
		{
			CMD_Printf("PHYS_ResetModule : Error at %d lines !!!\n", __LINE__);
			return -1;
		}

		if(Status_Register & FR)
			break;

		VK_TASK_Sleep(PHYS_DELAY);
		count++;
		if(count == PHYS_TIMEOUT)
		{
			CMD_Printf("PHYS_ResetModule : Error at %d lines !!!\n", __LINE__);
			return -1;
		}
	}

	return 0;
}

unsigned char IsCamAvailable(unsigned short slotId)
{
	int count;
	DRV_stSignal signal;
	int Ret;

	signal.sig = DRV_CARD_DETECT;
	Ret = DRV_CI_Ioctl(slotId, DRV_TSIGNAL, &signal);
	if ((Ret > 0) && (signal.value))
	{
		count = 0;
		signal.sig = DRV_READY_BUSY;

		do
		{
			Ret = DRV_CI_Ioctl(slotId, DRV_TSIGNAL, &signal);
			if (Ret < 0)
			{
				CMD_Printf("ERR3\n");
				return -1;
			}
			VK_TASK_Sleep(PHYS_DELAY);
			count++;
			if(count==PHYS_TIMEOUT)
			{
				CMD_Printf("ERR2\n");
				return -1;
			}
		}while (! signal.value);

		return 0;
	}
	else
	{
		CMD_Printf("ERR1\n");
		return -1;
	}
}

int CamTestStart(unsigned short slotId)
{
	int i;
	int slot;

	DRVTab[slotId].State 	 = DRV_IDLE;
	DRVTab[slotId].firstSLPDU = NULL;
	slot = DRV_CI_Open((unsigned char *)DRV_DEV);
	if (slot < 0)
	{
		CMD_Printf("DRV_CI_Open : wrong slot %d !!!\n", slot);
		DRVTab[i].Slot_id = INVALID_CI_SLOT_ID;
	}
	DRVTab[slotId].Slot_id = (unsigned short)slot;
	DRVTab[slotId].Ci_plus = 0;

	return 0;
}

int CamTestStop(unsigned short slotId)
{
	unsigned char *p;
	unsigned char *free_p;

	if (DRVTab[slotId].Slot_id != INVALID_CI_SLOT_ID)
	{
		DRV_CI_Close(DRVTab[slotId].Slot_id);
		DRVTab[slotId].Slot_id = INVALID_CI_SLOT_ID;
	}
	if (DRVTab[slotId].firstSLPDU)
	{
		p = (unsigned char *)DRVTab[slotId].firstSLPDU;
		while (((SLPDU*)p)->next)
		{
			free_p = p;
			p = ((SLPDU*)p)->next;
			DD_MEM_Free(free_p);
		}
		DD_MEM_Free(p);
	}
	DRVTab[slotId].State = DRV_DISCONNECT;

	return 0;
}

int CamMemTest(int index)
{
	int Ret;

	DRVTab[index].State = DRV_DISCONNECT;

	Ret = IsCamAvailable(0);
	if (Ret != 0)
	{
		DRVTab[index].State = DRV_IDLE;
		CMD_Printf("CAM NOT AVAILABLE!\n");
		return -1;
	}
 	VK_TASK_Sleep(1000);

	Ret = CamMemoryInitializeTest(index);
	return Ret;
}

int CamIOTest(int index)
{
	int Ret;

	Ret = IsCamAvailable(0);
	if (Ret != 0)
	{
		DRVTab[index].State = DRV_IDLE;
		CMD_Printf("CAM NOT AVAILABLE!\n");
		return -1;
	}
	Ret = CamIOInitializeTest(index);
	if(Ret != 0)
		DI_CI_ResetModule(0);
	return Ret;
}
#endif
static char parser_ci_caStr[500];
int CMD_HwTest_Parser_Ci(void *szArgs)
{
#if defined(CONFIG_TS_MATRIX)
	HUINT32 ulTs0_0 = 0, ulTs0_1 = 0, ulTs1_0 = 0, ulTs1_1 = 0, ulTs1_2 = 0, ulTs1_3 = 0;
	unsigned char result = 0;
#endif

	GET_ARGS;
	if CMD_IS("readcis")
	{
#if !defined(CONFIG_USE_EXTERNAL_CI_CONTROLLER)
		unsigned int offset;
		unsigned char *pCISInfo;

		pCISInfo = DD_MEM_Alloc(0x200);
		VK_memset(pCISInfo,0x0,0x200);
		for(offset =0; offset < 0x200; offset++)
		{
			DRV_CI_ReadMem(0,offset,&pCISInfo[offset]);
		}
		DRV_CI_ParseCis(pCISInfo);
		DD_MEM_Free(pCISInfo);
#endif
		iResult = CMD_OK;
	}
	else if CMD_IS("info")
	{
#if !defined(CONFIG_USE_EXTERNAL_CI_CONTROLLER)
		int i;
		int maxlist = 1;
		if(camInfo.ucIsInitialized)
		{

			CMD_Printf("%s detected.\n", (camInfo.ModuleType == 1)?"Conditional Access Module\0":"Eletronic Program Guide\0");
			CMD_Printf(" Manufacturer: %s\n", camInfo.ModuleManufacturer);
			CMD_Printf(" Supported CA_SYSTEM_ID\n");
			for(i=0;i<camInfo.ucCaSysIdcount; i++)
			{
				CMD_Printf("%3d: 0x%04x (%s)\n", i, camInfo.usCaSysId[i], camInfo.ucCaSysIdName[i]);
			}

			if(camInfo.ucCaSysIdcount >= 1)
			{
				VK_sprintf(parser_ci_caStr, " 0x%04x (%s)\n", camInfo.usCaSysId[0], camInfo.ucCaSysIdName[0]);
				if ( CMD_GetMsgUI() != NULL )
				{
					(int *)(CMD_GetMsgUI())(parser_ci_caStr);
	           			VK_MEM_Memset(parser_ci_caStr, 0x00, 500);
				}
				VK_sprintf(parser_ci_caStr," Supported CA_SYSTEM_ID\n");
				if ( CMD_GetMsgUI() != NULL )
				{
					(int *)(CMD_GetMsgUI())(parser_ci_caStr);
	           			VK_MEM_Memset(parser_ci_caStr, 0x00, 500);
				}
			}

			VK_sprintf(parser_ci_caStr, "Manufacturer: %s\n", camInfo.ModuleManufacturer);
			if ( CMD_GetMsgUI() != NULL )
			{
				(int *)(CMD_GetMsgUI())(parser_ci_caStr);
           			VK_MEM_Memset(parser_ci_caStr, 0x00, 500);
			}

			VK_sprintf(parser_ci_caStr, "%s detected.\n", (camInfo.ModuleType == 1)?"Conditional Access Module\0":"Eletronic Program Guide\0");
			if ( CMD_GetMsgUI() != NULL )
			{
				(int *)(CMD_GetMsgUI())(parser_ci_caStr);
           			VK_MEM_Memset(parser_ci_caStr, 0x00, 500);
			}
		}
		else
		{
			CMD_Printf("No CAM detected.\n");
            if ( CMD_GetMsgUI() != NULL )
            {
                (int *)(CMD_GetMsgUI())("No CAM detected\n");
            }
		}
#endif

		iResult = CMD_OK;
	}
	else if CMD_IS("test")
	{
#if !defined(CONFIG_USE_EXTERNAL_CI_CONTROLLER)
		if PARAM_IS("rmxfeedback")
		{
			//register callbacks for PES filtering
			DI_DEMUX_SetPath(0,DI_DEMUX_INPUT_CURRENT,0,DI_DEMUX_OUTPUT_CURRENT,0,0);
			iResult = CMD_OK;
		}
		else if PARAM_IS("bp") /*bypass*/
		{
			int slotId = 0;
			if (READABLE_IN_DEC(HWTEST_PARAM1, slotId))
			{
				DRV_DEMUX_CamBypass(slotId);
				iResult = CMD_OK;
			}
		}
		else if PARAM_IS("pt") /*passthrough*/
		{
			int slotId = 0;
			if (READABLE_IN_DEC(HWTEST_PARAM1, slotId))
			{
				DRV_DEMUX_CamPassthrough(slotId);
				iResult = CMD_OK;
			}
		}
#if defined(EBI_CONFLICT_TEST)
		else if PARAM_IS("ebitraffic")
		{
			//start ebi traffic test.
			DRV_CI_StartEBITrafficTask();
			iResult = CMD_OK;
		}
#endif
#else
		if PARAM_IS("rmxfeedback")
		{
			iResult = CMD_OK;
		}
		else if PARAM_IS("bp") /*bypass*/
		{
			int slotId = 0;
			int tunerId = 0;
			int ret = 0,ret1 = 0;
			DI_ERR_CODE	diErr = 0;
			GET_NEXT_ARG(szSlotId);

			ret = READABLE_IN_DEC(HWTEST_PARAM1, tunerId);
			ret1 = READABLE_IN_DEC(szSlotId, slotId);

			CMD_Printf("=== BYPASS TEST (%d:%d:%d:%d)==\r\n",ret,ret1,tunerId,slotId);

			if(ret && ret1&&((slotId == 0)||(slotId == 1))&&((tunerId >= 0)&&( tunerId < 4 )))
			{

				DI_DEMUX_SetPath(0,DI_DEMUX_INPUT_CURRENT,0,DI_DEMUX_OUTPUT_CURRENT,0,-1);
				if(slotId == 0)
			{
					diErr = DI_CI_SetExtendedTsPath(tunerId, DI_CI_LIVE0,0, DI_CI_OUTPUT0, DI_CI_NORMAL_BYPASS);
					CMD_Printf("=== BYPASS Slot 0 (%d:%d:%d)==\r\n",diErr,tunerId,slotId);

				}
				else
				{
					diErr = DI_CI_SetExtendedTsPath(tunerId, DI_CI_LIVE1,1, DI_CI_OUTPUT1, DI_CI_NORMAL_BYPASS);
					CMD_Printf("=== BYPASS Slot 1 (%d:%d:%d)==\r\n",diErr,tunerId,slotId);
				}

				DI_DEMUX_SetPath(0,DI_DEMUX_INPUT_CURRENT,0,DI_DEMUX_OUTPUT_CURRENT,0,0);

			}
		}
		else if PARAM_IS("pt") /*passthrough*/
		{
			unsigned int slotId = 0;
			unsigned int tunerId = 0;
			int ret = 0,ret1 = 0;
			DI_ERR_CODE	diErr = 0;
			GET_NEXT_ARG(szSlotId);

			ret = READABLE_IN_DEC(HWTEST_PARAM1, tunerId);
			ret1 = READABLE_IN_DEC(szSlotId, slotId);

			if(ret && ret1&&((slotId == 0)||(slotId == 1))&&((tunerId >= 0)&&( tunerId < 4 )))
			{
				if (DRV_CI_GetCAMDetect (0,slotId) == CAM_DETECT)
				{
					if(slotId == 0)
					{
						diErr = DI_CI_SetExtendedTsPath(tunerId, DI_CI_LIVE0,slotId, DI_CI_OUTPUT0, DI_CI_NORMAL_PASSTHROUGH);
						diErr = DI_DEMUX_SetPath(0,2,tunerId,3,0,0);
						CMD_Printf("=== PASSTHROUGH Slot 0 (%d:%d:%d)==\r\n",diErr,tunerId,slotId);
					}
					else
					{
						diErr = DI_CI_SetExtendedTsPath(tunerId, DI_CI_LIVE1,slotId, DI_CI_OUTPUT1, DI_CI_NORMAL_PASSTHROUGH);
						diErr = DI_DEMUX_SetPath(0,2,tunerId,3,0,1);
						CMD_Printf("=== PASSTHROUGH Slot 1 (%d:%d:%d)==\r\n",diErr,tunerId,slotId);
					}
				}
			}
		}
		else if PARAM_IS("ptr") /*remux passthrough*/
		{
			unsigned int slotId = 0;
			unsigned int tunerId = 0;
			int ret = 0,ret1 = 0;
			DI_ERR_CODE	diErr = 0;
			GET_NEXT_ARG(szSlotId);

			ret = READABLE_IN_DEC(HWTEST_PARAM1, tunerId);
			ret1 = READABLE_IN_DEC(szSlotId, slotId);

			if(ret && ret1&&((slotId == 0)||(slotId == 1))&&((tunerId >= 0)&&( tunerId < 4 )))
			{
				if (DRV_CI_GetCAMDetect (0,slotId) == CAM_DETECT)
				{
					if(slotId == 0)
					{
						diErr = DI_CI_SetExtendedTsPath(tunerId, DI_CI_PLAYBACK0,slotId, DI_CI_OUTPUT0, DI_CI_NORMAL_PASSTHROUGH);
						diErr = DI_DEMUX_SetPath(0,2,tunerId,3,0,0);
						CMD_Printf("=== Remux PASSTHROUGH Slot 0 (%d:%d:%d)==\r\n",diErr,tunerId,slotId);
					}
					else
					{
						diErr = DI_CI_SetExtendedTsPath(tunerId, DI_CI_PLAYBACK1,slotId, DI_CI_OUTPUT1, DI_CI_NORMAL_PASSTHROUGH);
						diErr = DI_DEMUX_SetPath(0,2,tunerId,3,0,1);
						CMD_Printf("=== Remux PASSTHROUGH Slot 1 (%d:%d:%d)==\r\n",diErr,tunerId,slotId);
					}

				}
			}
		}
		else if PARAM_IS("clko")
		{
			int clkresult = 0;
			unsigned int slotId = 0;
			int ret = 0;
			ret = READABLE_IN_DEC(HWTEST_PARAM1, slotId);
			if(ret&&((slotId == 0)||(slotId == 1)))
			{
				clkresult  = CIMAX_SetCLK(0, 1);
				CMD_Printf("=== CM CLK ON(%d) ==\r\n",clkresult);
			}

		}
		else if PARAM_IS("clkf")
		{
			int clkresult = 0;
			unsigned int slotId = 0;
			int ret = 0;
			ret = READABLE_IN_DEC(HWTEST_PARAM1, slotId);
			if(ret&&((slotId == 0)||(slotId == 1)))
			{
				clkresult  = CIMAX_SetCLK(0, 0);
				CMD_Printf("=== CM CLK OFF(%d) ==\r\n",clkresult);
			}

		}
		else if PARAM_IS("clf")
		{
			int oret = 0;
			unsigned int slotId = 0;
			int ret = 0;
			ret = READABLE_IN_DEC(HWTEST_PARAM1, slotId);
			if(ret&&((slotId == 0)||(slotId == 1)))
			{
				oret = DRV_CIMAX_ResetFIFO(slotId);
				CMD_Printf("=== CIMAX_ResetFO(%d) ==\r\n",oret);
			}
		}

		iResult = CMD_OK;

#endif
	}
#if defined(CONFIG_USE_EXTERNAL_CI_CONTROLLER)
	else if CMD_IS("cimax")
	{

		HINT32 chkReturn;
		HUINT32 regAddr;
		HUINT32	regValuefromUser;
		HUINT8	regValue;
		GET_NEXT_ARG(szRegValuefromUser);

		iResult = CMD_ERR;

		if PARAM_IS("regr")
		{
			if (READABLE_IN_HEX(HWTEST_PARAM1, regAddr))
			{
				chkReturn = CIMAX_ReadRegister(regAddr, &regValue, 1);
				if(chkReturn == CIMAX_NO_ERROR)
				{
					CMD_Printf("Read_Register: [0x%04x]: 0x%02x\n",regAddr, regValue);
					iResult = CMD_OK;
				}
				else
				{
					CMD_Printf("Faild to read from register 0x%lx\n", regAddr);
				}
			}
			else
			{
				CMD_Printf("Error!! failed to get 1st paramter.\n");

			}
		}
		else if PARAM_IS("regw")
		{
			if (READABLE_IN_HEX(HWTEST_PARAM1, regAddr))
			{
				if(READABLE_IN_HEX(szRegValuefromUser, regValuefromUser))
				{
					regValue = (HUINT8)regValuefromUser;
					chkReturn = CIMAX_WriteRegister(regAddr, &regValue, 1);
					if(chkReturn == CIMAX_NO_ERROR)
					{
						CMD_Printf("Write_Register [0x%04x]: 0x%02x\n",regAddr, regValue);
						iResult = CMD_OK;
					}
					else
					{
						CMD_Printf("Faild to write to register 0x%lx\n", regAddr);
					}
				}
				else
				{
					CMD_Printf("Error!! failed to get 2nd paramter.\n");

				}
			}
			else
			{
				CMD_Printf("Error!! failed to get 1st paramter.\n");

			}
		}
		else if PARAM_IS("pathinfo")
		{
#if defined(CONFIG_DEBUG)
			chkReturn = CIMAX_ReadRegister(IN_SEL, &regValue, 1);
			if(chkReturn == CIMAX_NO_ERROR)
			{
				CMD_Printf("CHIP_IN : 0x%02x [InCh_0:   %10s][InCh_1:   %10s]\n", regValue, CIMAX_ChipInput[(regValue&0x07)], CIMAX_ChipInput[(regValue&0x70)>>4]);
				chkReturn = CIMAX_ReadRegister(ROUTER_CAM_MOD, &regValue, 1);
				if(chkReturn == CIMAX_NO_ERROR)
				{
					CMD_Printf("CAM_IN  : 0x%02x [Slot0:    %10s][Slot1:    %10s]\n", regValue, CIMAX_ModuleInput[(regValue&0x07)], CIMAX_ModuleInput[(regValue&0x70)>>4]);
					chkReturn = CIMAX_ReadRegister(ROUTER_CAM_CH, &regValue, 1);
					if(chkReturn == CIMAX_NO_ERROR)
					{
						CMD_Printf("CH_OUT  : 0x%02x [OutCh0:   %10s][Outch1:   %10s]\n", regValue, CIMAX_OutputChannel[(regValue&0x0F)], CIMAX_OutputChannel[(regValue&0xF0)>>4]);
						chkReturn = CIMAX_ReadRegister(OUT_SEL, &regValue, 1);
						if(chkReturn == CIMAX_NO_ERROR)
						{
							CMD_Printf("CHIP_OUT: 0x%02x [ChipOut0: %10s][ChipOut1: %10s]\n", regValue, CIMAX_ChipOutput0[(regValue&0x03)],CIMAX_ChipOutput1[(regValue&0x04)>>2]);
							iResult = CMD_OK;
						}
					}
				}
			}
#else
	  		chkReturn = CIMAX_ReadRegister(IN_SEL, &regValue, 1);
			if(chkReturn == CIMAX_NO_ERROR)
			{
				CMD_Printf("CHIP_IN : 0x%02x\n", regValue);
				chkReturn = CIMAX_ReadRegister(ROUTER_CAM_MOD, &regValue, 1);
				if(chkReturn == CIMAX_NO_ERROR)
				{
					CMD_Printf("CAM_IN  : 0x%02x\n", regValue);
					chkReturn = CIMAX_ReadRegister(ROUTER_CAM_CH, &regValue, 1);
					if(chkReturn == CIMAX_NO_ERROR)
					{
						CMD_Printf("CH_OUT  : 0x%02x\n", regValue);
						chkReturn = CIMAX_ReadRegister(OUT_SEL, &regValue, 1);
						if(chkReturn == CIMAX_NO_ERROR)
						{
							CMD_Printf("CHIP_OUT: 0x%02x\n", regValue);
							iResult = CMD_OK;
						}
					}
				}
			}
#endif
		}
	}
#endif
#if defined(BDBG_DEBUG_BUILD)
	else if CMD_IS("tracelevel")
	{
		int traceLevel;
		iResult=CMD_ERR;
		if(READABLE_IN_DEC(HWTEST_PARAM, traceLevel))
		{
			if(traceLevel >= TRACE_LEVEL_NONE || traceLevel <= TRACE_LEVEL_ALL)
			{
				CMD_Printf("Set CI tracelevel to %d\n",traceLevel);
				TraceSetLevel(TRACE_MODULE_DI_CI, traceLevel);
				iResult = CMD_OK;
			}
			else
			{
				CMD_Printf("Trace level %d is out of range. (0<=level<= 6)\n", traceLevel);
				iResult=CMD_ERR;
			}
		}
		else
		{
			CMD_Printf("Failed to get tracelevel value.\n");
			iResult=CMD_ERR;
		}
	}
#endif
	else
	{
		HWTEST_PARAM1 = HWTEST_PARAM1;

		CMD_Printf("Wrong ci Command : ci ?? -> ci readcis/info/test\n");
	}
	return iResult;
}
#endif

int CMD_HwTest_Parser_Scard(void *szArgs)
{
	szArgs = szArgs;

	return 0;
}

int CMD_HwTest_Parser_Rcu(void *szArgs)
{
	int Interval, keycode, count;
	HUINT8 cKeyName[IR_LENGTH_OF_KEY_NAME];

	GET_ARGS;

	if CMD_IS("repeat")
	{
		if(READABLE_IN_HEX(HWTEST_PARAM, keycode) && READABLE_IN_DEC(HWTEST_PARAM1, Interval))
		{
			s_ulKeyRepeatInterval = Interval;
			s_ulKeyRepeatCode = keycode;
			s_bUserAgingStart = FALSE;

		}
		else
		{
			s_ulKeyRepeatInterval = 0;
			s_ulKeyRepeatCode = 0;
		}

		if(s_ulKeyRepeatInterval != 0 && s_ulKeyRepeatCode != 0)
		{
			CMD_Printf("Key Repeat Test - Code : 0x%x, Interval : %dSec\n", s_ulKeyRepeatCode, s_ulKeyRepeatInterval);

			if(ulIrAgingTaskId == 0)
			{
				if(0 == VK_TASK_Create(P_Key_Repeat_Task, USER_PRIORITY2, NORMAL_TASK_STACK_SIZE, "key repeat task", (void *)0, &ulIrAgingTaskId, FALSE))
				{
					VK_TASK_Start(ulIrAgingTaskId);
					CMD_Printf("Key Repeat Task Start.\n");
					iResult=CMD_OK;
				}
				else
				{
					CMD_Printf("Key Repeat Task Start Fail.\n");
					iResult=CMD_ERR;
				}
			}
		}
		else
		{
			s_ulKeyRepeatInterval = 0;
			s_ulKeyRepeatCode = 0;

			CMD_Printf("rcu repeat stop \n");
		}

		iResult=CMD_OK;
	}
	else if CMD_IS("useraging")
	{
		if PARAM_IS("load")
		{
			iResult=CMD_OK;
			s_ulNumberOfUserAgingSequence = 0;
			if(DRV_OK == P_Key_AgingSquenceOpen())
			{
				CMD_Printf("Check the missed key \n");

				for(count=0; count<s_ulNumberOfUserAgingSequence ; count++)
				{
					DRV_KEY_GetKeyName((HUINT8)s_ulUserKeyTable[count][0], cKeyName);
					CMD_Printf("[%d] Key : %s(0x%x), Interval: %d\n", count, cKeyName,s_ulUserKeyTable[count][0], s_ulUserKeyTable[count][1] );
				}
			}
			else
			{
				CMD_Printf("Loading UserAging Sequence Fail.\n");
				s_ulNumberOfUserAgingSequence= 0;
				iResult=CMD_ERR;
			}
		}
		else if PARAM_IS("start")
		{
			if(s_ulNumberOfUserAgingSequence != 0)
			{
				s_bUserAgingStart = TRUE;
				s_ulKeyRepeatInterval = 0;
				s_ulKeyRepeatCode = 0;

		iResult=CMD_OK;

				if(ulIrAgingTaskId == 0)
				{
					if(0 == VK_TASK_Create(P_Key_Repeat_Task, USER_PRIORITY2, NORMAL_TASK_STACK_SIZE, "key repeat task", (void *)0, &ulIrAgingTaskId, FALSE))
					{
						VK_TASK_Start(ulIrAgingTaskId);
						CMD_Printf("User Aging Task Start.\n");
						iResult=CMD_OK;
					}
					else
					{
						CMD_Printf("User Aging Start Fail.\n");
						iResult=CMD_ERR;
					}
				}
			}
		}
		else if PARAM_IS("stop")
		{
			s_bUserAgingStart = FALSE;
			iResult=CMD_OK;
		}
		else
		{
			CMD_Printf("first \"rcu useraging load\" and then \"rcu useraging start\" \n");
			iResult=CMD_ERR;
		}
	}
#if defined(SUPPORT_IR_MULTI_CUSTOM_CODE)
	else if CMD_IS("defaultset")
	{
		HUINT16 pusCustomCode;
		pusCustomCode = DRV_REMOCON_MultiCusomCode_GetCurrentCustomCode();
		CMD_Printf("Current custom code :: 0x%04x  \n",pusCustomCode);
		DRV_REMOCON_MultiCustomCode_WriteDefaultCustomCodeToE2P();
		pusCustomCode = DRV_REMOCON_MultiCusomCode_GetCurrentCustomCode();
		CMD_Printf("Writed custom code :: 0x%04x \n",pusCustomCode);
	}
	else if CMD_IS("info")
	{
		HUINT16 pusCustomCode;
		pusCustomCode = DRV_REMOCON_MultiCusomCode_GetCurrentCustomCode();
		CMD_Printf("Current custom code :: 0x%04x  \n",pusCustomCode);
	}
	else
	{
		HWTEST_PARAM = HWTEST_PARAM;
		HWTEST_PARAM1 = HWTEST_PARAM1;

		CMD_Printf("Command : rcu ?? -> defaultset/info \n");
	}

#else
	else
	{
	CMD_Printf("Command : Not support command \n");
		iResult=CMD_ERR;
	}
#endif

	return iResult;

}

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


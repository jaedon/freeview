/**
 * di_scart.c
*/

/**
 * @defgroup		DI_SCART : DI_SCART module
 * @author			Chung Dong Chul
 * @note			DI_SCART APIs
 * @brief			Define DI_SCART APIs
 * @file 			di_scart.c
*/



/*******************************************************************/
/**************************** Header Files *************************/
/*******************************************************************/
#if defined(CONFIG_VK_STDLIB)
#else
#include 	<stdio.h>
#include 	<stdlib.h>
#include 	<string.h>
#endif

#include "di_err.h"
#include "drv_err.h"
#include "drv_gpio.h"
#include "di_scart.h"
#include "di_uart.h"
#include "drv_scart.h"
#include "vkernel.h"
#include "taskdef.h"
#include "di_nvram.h"
#include "drv_micom.h"
#include "drv_nvram.h"


/*******************************************************************/
/****************************** define *****************************/
/*******************************************************************/

//#define DI_SCART_SEE_PARAMETERS
//#define DI_SCART_DEBUG


#define PARAM_COLOR_ON		"\033[01;32m"
#define PARAM_COLOR_OFF	"\033[00m"

#ifdef DI_SCART_DEBUG
#define PrintData		DI_UART_Print
#define PrintDebug	DI_UART_Print
#define PrintError	DI_UART_Print
#else
#define PrintData		while (0) ((int (*)(char *, ...)) 0)
#define PrintDebug	while (0) ((int (*)(char *, ...)) 0)
#define PrintError	DI_UART_Print
#endif

#define DI_SCART_CHECK_COUNT		10
#define SCART_MSGQ_COUNT			32

/*******************************************************************/
/****************************** typedef ****************************/
/*******************************************************************/
typedef	struct {
	HINT32					bInit;
	HUINT8					ucSoundLevel;
	HUINT32					uiCheckCount;
	HBOOL					IsTvConnected;
	HBOOL					IsVcrConnected;
	DI_SCART_SETTINGS		Settings;
} DI_SCART_INFO;

typedef enum SCART_MSG_e
{
	P_SCART_MSG_TYPE_INTERRUPT = 0,
	P_SCART_MSG_TYPE_DUMMY
} SCART_MSG_E;

typedef struct SCART_MSG_t
{
	SCART_MSG_E		msgClass;
} SCART_MSG_T;

/*******************************************************************/
/************************ global variables *************************/
/*******************************************************************/

/*******************************************************************/
/************************ static variables *************************/
/*******************************************************************/
static DRV_SCART_OPERATION s_ScartOp;
static DI_SCART_INFO s_ScartInfo;
#if defined(CONFIG_SCART_NULL)
#else
static unsigned long   s_ulScartTaskId = 0;
static unsigned long   s_ulScartQueueId = 0;
#endif
static DI_SCART_CALLBACK s_pfCallback[DS_EVENT_MAX]={NULL,NULL};

/*******************************************************************/
/************************ extern           *************************/
/*******************************************************************/
extern void Scart_Init_ChipDriver(void);
extern int DRV_POWER_GetPowerSavingMode(void);

#if defined(CONFIG_SCART_NULL)
#else
static void P_SCART_InterruptEnable(void)
{
	DRV_GPIO_INTR_Enable(GPIO_ID_SCART_IT);
	(*s_ScartOp.pfInterrupt_Disable)();
	/* SCART-STV6417 Status Register를 먼저 read (Interrup가 clear 됨) 하고 Scart Interrupt를 Enable해야 된다.*/
	(*s_ScartOp.pfStatusRead)();
	(*s_ScartOp.pfInterrupt_Enable)();
}


// Disabled to remove warnings.
static void P_SCART_InterruptDisable(void)
{
	DRV_GPIO_INTR_Disable(GPIO_ID_SCART_IT);
	(*s_ScartOp.pfInterrupt_Disable)();
}
#endif

#if defined(CONFIG_SCART_6417)
static void P_SCART_InterruptHandler(void *context, int param)
{
	SCART_MSG_T	scartMsg;
	int		rc=0;
	BSTD_UNUSED(context);
	BSTD_UNUSED(param);

	DI_UART_Print("[P_SCART_InterruptHandler]\n");
	s_ScartInfo.uiCheckCount=DI_SCART_CHECK_COUNT;
 	scartMsg.msgClass = P_SCART_MSG_TYPE_INTERRUPT;
 	rc = VK_MSG_SendTimeout(s_ulScartQueueId, &scartMsg, sizeof(SCART_MSG_T), 0);
	if (rc != VK_OK)
	{
		DI_UART_Print("[P_SCART_InterruptHandler] error(%08X) VK_MSG_SendTimeout\n", rc);
		return;
	}

	return;
}

static void P_SCART_InterruptInit(void)
{
	DRV_GPIO_INTR_RegisterEventNotify(GPIO_ID_SCART_IT, P_SCART_InterruptHandler, (void*)NULL, (int)NULL);
	P_SCART_InterruptEnable();
}
#endif

static DI_ERR_CODE P_SCART_SetSettings(DI_SCART_SETTINGS* pSettings)
{//Sunwoo
	if(pSettings->eMode == DI_SCART_MODE_NORMAL)
	{
		if(s_ScartInfo.Settings.eMode!=DI_SCART_MODE_NORMAL)
		{
			(*s_ScartOp.pfSet_Normal_Mode)();
			(*s_ScartOp.pfReset_VCRbypass_Mode)();
			(*s_ScartOp.pfReset_VCR_Output_Off)();
			(*s_ScartOp.pfSet_Sound_Level)(s_ScartInfo.ucSoundLevel);

/*************************
*	NOTICE!!
*	After turning back from bypass-mode to normal-mode, with stv6417,
*	it seems that the "slow blanking" register value is removed unexpectedly.
*	(There was an scart signal missing issue...)
*	It doesn't make any problem with GPIO scart.
*************************/
			if(pSettings->stNormalMode.eTvAspectRatioOutput == DI_SCART_NO_SIGNAL)
			{
				(*s_ScartOp.pfReset_FunctionPin_On)();
				(*s_ScartOp.pfSet_TV_CVBS_Mode)();
			}
			else if(pSettings->stNormalMode.eTvAspectRatioOutput == DI_SCART_AR_4_3)
			{
				(*s_ScartOp.pfReset_Wide_Mode)();
			}
			else
			{
				(*s_ScartOp.pfSet_Wide_Mode)();
			}
		}

/*************************
*	1. TV Fast Blanking
*************************/
		if(s_ScartInfo.Settings.stNormalMode.eTvVideoOutput != pSettings->stNormalMode.eTvVideoOutput)
		{
			if(pSettings->stNormalMode.eTvVideoOutput == DI_SCART_OUTPUT_CVBS)
			{
				(*s_ScartOp.pfReset_TV_SVHS_Mode)();
				(*s_ScartOp.pfSet_TV_CVBS_Mode)();
			}
			else if(pSettings->stNormalMode.eTvVideoOutput == DI_SCART_OUTPUT_RGB)
			{
				(*s_ScartOp.pfReset_TV_SVHS_Mode)();
				(*s_ScartOp.pfSet_TV_RGB_Mode)();
			}
			else if(pSettings->stNormalMode.eTvVideoOutput == DI_SCART_OUTPUT_SVIDEO)
			{
				(*s_ScartOp.pfSet_TV_SVHS_Mode)();
				(*s_ScartOp.pfSet_TV_CVBS_Mode)();
			}
			else
			{
				/*	Nothing to do
				*/
			}
		}
/*************************
*	2. TV Slow Blanking
*************************/
		if(s_ScartInfo.Settings.stNormalMode.eTvAspectRatioOutput != pSettings->stNormalMode.eTvAspectRatioOutput)
		{
			if(pSettings->stNormalMode.eTvAspectRatioOutput == DI_SCART_NO_SIGNAL)
			{
				(*s_ScartOp.pfReset_FunctionPin_On)();
				(*s_ScartOp.pfSet_TV_CVBS_Mode)();
			}
			else if(pSettings->stNormalMode.eTvAspectRatioOutput == DI_SCART_AR_4_3)
			{
				(*s_ScartOp.pfSet_FunctionPin_On)();
				(*s_ScartOp.pfReset_Wide_Mode)();
			}
			else if(pSettings->stNormalMode.eTvAspectRatioOutput == DI_SCART_AR_16_9)
			{
				(*s_ScartOp.pfSet_FunctionPin_On)();
				(*s_ScartOp.pfSet_Wide_Mode)();
			}
			else
			{
				/*	Nothing to do
				*/
			}
		}
/*************************
*	3. VCR Fast Blanking
*************************/
		if(s_ScartInfo.Settings.stNormalMode.eVcrVideoOutput != pSettings->stNormalMode.eVcrVideoOutput)
		{
			if(pSettings->stNormalMode.eVcrVideoOutput == DI_SCART_OUTPUT_CVBS)
			{
				(*s_ScartOp.pfSet_VCR_CVBS_Mode)();
			}
			else if(pSettings->stNormalMode.eVcrVideoOutput == DI_SCART_OUTPUT_RGB)
			{
				/*	Nothing to do
				*/
			}
			else if(pSettings->stNormalMode.eVcrVideoOutput == DI_SCART_OUTPUT_SVIDEO)
			{
				(*s_ScartOp.pfSet_VCR_SVHS_Mode)();
			}
			else
			{
				/*	Nothing to do
				*/
			}
		}
/*************************
*	4. VCR Slow Blanking
*************************/
		if(s_ScartInfo.Settings.stNormalMode.eVcrVideoOutput != pSettings->stNormalMode.eVcrVideoOutput)
		{
			if(pSettings->stNormalMode.eVcrAspectRatioOutput == DI_SCART_NO_SIGNAL)
			{
				(*s_ScartOp.pfReset_FunctionPin_On)();
				(*s_ScartOp.pfReset_VCR_Output_Off)();
			}
			else if(pSettings->stNormalMode.eVcrAspectRatioOutput == DI_SCART_AR_4_3)
			{
				/*	Nothing to do
				*/
			}
			else if(pSettings->stNormalMode.eVcrAspectRatioOutput == DI_SCART_AR_16_9)
			{
				/*	Nothing to do
				*/
			}
			else
			{
				/*	Nothing to do
				*/
			}
		}
	}
	else if(pSettings->eMode == DI_SCART_MODE_BYPASS)
	{
		(*s_ScartOp.pfSet_VCR_Output_Off)();
		(*s_ScartOp.pfSet_VCRbypass_Mode)();

/*************************
*	Modification : By Nara, 2010-JAN-08
*	HD_FOX_T2, Issue Number 72, MEDIA, Sub Number 3
*	VCR Bypass의 경우, 현재 STB의 볼륨을 셋팅하지 않게 한다
*************************/
#if 0
		(*s_ScartOp.pfSet_Sound_Level)(s_ScartInfo.ucSoundLevel);
#endif
	}
	else if(pSettings->eMode == DI_SCART_MODE_HW_BYPASS)
	{
		(*s_ScartOp.pfSet_HW_Bypass)(HW_BYPASS_NO_DETECT_SIG_MODE);
	}
	else if(pSettings->eMode == DI_SCART_MODE_HW_BYPASS_NO_DET)
	{
		(*s_ScartOp.pfSet_HW_Bypass)(HW_BYPASS_NO_DETECT_SIG_MODE);
	}
	else
	{
		/*	Nothing to do
		*/
	}

	VK_MEM_Memcpy(&s_ScartInfo.Settings, pSettings, sizeof(DI_SCART_SETTINGS));
	return DI_ERR_OK;
}

#if defined(CONFIG_SCART_NULL)
#else
static void P_SCART_ScartTask(void *param)
{
	unsigned int uiIsConnected=0xaaaaaaaa;
	unsigned char ucStatus;
	SCART_MSG_T		scartMsg;
	int				rc;

	BSTD_UNUSED(param);

	while (1)
	{
		rc = VK_MSG_ReceiveTimeout(s_ulScartQueueId, &scartMsg, sizeof(SCART_MSG_T), 100);
		if (rc != VK_OK)
		{
			if (s_ScartInfo.uiCheckCount>0)
			{
				ucStatus = (*s_ScartOp.pfCheck_VCR_Input)();

				uiIsConnected<<=1;
				if (ucStatus==1)
					uiIsConnected|=1;

				if ((uiIsConnected&0xf)==0 && s_ScartInfo.IsVcrConnected!=FALSE)
				{
#if defined(DI_SCART_SEE_PARAMETERS)
					DI_UART_Print(PARAM_COLOR_ON"VCR Disconnected\n"PARAM_COLOR_OFF);
#endif
					 s_ScartInfo.IsVcrConnected=FALSE;
					if (s_pfCallback[DS_EVENT_VCR_PWR_CHANGE]!=NULL)
						(*s_pfCallback[DS_EVENT_VCR_PWR_CHANGE])(&s_ScartInfo.IsVcrConnected);
					s_ScartInfo.uiCheckCount--;

				}
				else if ((uiIsConnected&0xf)==0xf && s_ScartInfo.IsVcrConnected!=TRUE)
				{
#if defined(DI_SCART_SEE_PARAMETERS)
					DI_UART_Print(PARAM_COLOR_ON"VCR Connected\n"PARAM_COLOR_OFF);
#endif
					 s_ScartInfo.IsVcrConnected=TRUE;
					if (s_pfCallback[DS_EVENT_VCR_PWR_CHANGE]!=NULL)
						(*s_pfCallback[DS_EVENT_VCR_PWR_CHANGE])(&s_ScartInfo.IsVcrConnected);
					s_ScartInfo.uiCheckCount--;
				}
				else if (((uiIsConnected&0xf)==0 && s_ScartInfo.IsVcrConnected==FALSE)
						||
						((uiIsConnected&0xf)==0xf && s_ScartInfo.IsVcrConnected==TRUE))
				{
					s_ScartInfo.uiCheckCount--;
				}
			}
		}
		else
		{
			switch (scartMsg.msgClass)
			{
				case P_SCART_MSG_TYPE_INTERRUPT :
					VK_TASK_Sleep(250);
					P_SCART_InterruptDisable();
					(*s_ScartOp.pfStatusRead)();
					P_SCART_InterruptEnable();
					break;
				default:
					break;
			}
		}
	}
}
#endif

DRV_Error DRV_SCART_Init(void)
{
#if defined(CONFIG_SCART_NULL)
#else
	DRV_Error rc=DRV_OK;
#endif

	DRV_MICOM_WAKEUP_SRC eWakeupSrc = DRV_MICOM_WAKEUP_BY_ACPOWER;
	DI_NVRAM_OSD_DISPLAY_E eVideo=DI_NVRAM_OSD_ON;
	DI_SCART_SETTINGS Settings;

	VK_memset(&s_ScartInfo, 0, sizeof(DI_SCART_INFO));
	s_ScartInfo.IsTvConnected=TRUE+1; // unknown
	s_ScartInfo.IsVcrConnected=TRUE+1; // unknown
	s_ScartInfo.ucSoundLevel= 0;
	s_ScartInfo.uiCheckCount=DI_SCART_CHECK_COUNT;
	s_ScartInfo.Settings.eMode=DI_SCART_NO_SIGNAL;
	s_ScartInfo.Settings.stNormalMode.eTvAspectRatioOutput=DI_SCART_NO_SIGNAL;
	s_ScartInfo.Settings.stNormalMode.eTvVideoOutput	=DI_SCART_NO_SIGNAL;
	s_ScartInfo.Settings.stNormalMode.eVcrAspectRatioOutput=DI_SCART_NO_SIGNAL;
	s_ScartInfo.Settings.stNormalMode.eVcrVideoOutput=DI_SCART_NO_SIGNAL;

	Scart_Init_ChipDriver();
#if defined(CONFIG_SCART_NULL)
	// Do nothing
#else
	rc=VK_MSG_Create(SCART_MSGQ_COUNT, sizeof(SCART_MSG_T), "ScartMsg", &s_ulScartQueueId, VK_SUSPENDTYPE_FIFO);
	if(VK_OK != rc)
	{
		return DRV_ERR;
	}

	rc=VK_TASK_Create( P_SCART_ScartTask, SCART_CHK_TASK_PRIORITY, SCART_CHK_TASK_STACK_SIZE, "ScartTask", NULL, &s_ulScartTaskId, FALSE);
	if(VK_OK != rc)
	{
		return DRV_ERR;
	}

	rc=VK_TASK_Start(s_ulScartTaskId);
	if(VK_OK != rc)
	{
		return DRV_ERR;
	}

	rc=DRV_MICOM_GetWakeupSrc(&eWakeupSrc);
	if(rc != DRV_OK)
	{
		DI_UART_Print("Can't Read Wake Up Src from MICOM, so Scart ON: \n");
		eWakeupSrc=DRV_MICOM_WAKEUP_BY_ACPOWER;
	}

	rc=DRV_NVRAM_GetField(DI_NVRAM_FIELD_OSD_DISPLAY, 0, &eVideo, sizeof(DI_NVRAM_OSD_DISPLAY_E));
	if(rc != DRV_OK)
	{
		DI_UART_Print("Can't Read NVRAM data, so Scart ON: \n");
		eVideo=DI_NVRAM_OSD_ON;
	}

	if((eVideo == DI_NVRAM_OSD_OFF) && (eWakeupSrc == DRV_MICOM_WAKEUP_BY_TIMER))
	{
		Settings.eMode = DI_SCART_MAX;
		Settings.stNormalMode.eTvAspectRatioOutput=DI_SCART_NO_SIGNAL;
	}
	else
	{
		Settings.eMode = DI_SCART_MODE_NORMAL;
		Settings.stNormalMode.eTvAspectRatioOutput=DI_SCART_AR_4_3;
	}
	Settings.stNormalMode.eTvVideoOutput=DI_SCART_OUTPUT_CVBS;
	Settings.stNormalMode.eVcrAspectRatioOutput=DI_SCART_AR_4_3;
	Settings.stNormalMode.eVcrVideoOutput=DI_SCART_OUTPUT_CVBS;
	P_SCART_SetSettings(&Settings);

#if defined(CONFIG_SCART_6417)
	P_SCART_InterruptInit();
#endif
#endif
	s_ScartInfo.bInit=TRUE;

	return DRV_OK;
}


DRV_Error DRV_SCART_SetVolume(unsigned char ucVolume)
{
    /* Modification : By Nara, 2010-JAN-08 */
    /* HD_FOX_T2, Issue Number 72, MEDIA, Sub Number 3 */
    /* Bypass의 경우, 현재 STB의 볼륨을 셋팅하지 않게 한다 */
#if 1
    if ( (s_ScartInfo.Settings.eMode != DI_SCART_MODE_BYPASS) &&
         (s_ScartInfo.Settings.eMode != DI_SCART_MODE_HW_BYPASS) )
    {
	s_ScartOp.pfSet_Sound_Level(ucVolume);
    }
#else
	    s_ScartOp.pfSet_Sound_Level(ucVolume);
#endif
	s_ScartInfo.ucSoundLevel=ucVolume;
	return DRV_OK;
}


DRV_Error DRV_SCART_SetTVMute(unsigned char ucMute)
{
	if(ucMute)
	{
		s_ScartOp.pfSet_TV_Mute();
	}
	else
	{
		s_ScartOp.pfReset_TV_Mute();
	}

	return DRV_OK;
}


DRV_Error DRV_SCART_SetVCRMute(unsigned char ucMute)
{
	if(ucMute)
	{
		s_ScartOp.pfSet_VCR_Mute();
	}
	else
	{
		s_ScartOp.pfReset_VCR_Mute();
	}

	return DRV_OK;
}


DRV_SCART_OPERATION* DRV_SCART_GetOpHandle(void)
{
	return &s_ScartOp;
}


DRV_Error DRV_SCART_Standby(void)
{
	s_ScartOp.pfStandbyMode();
	return DRV_OK;
}

void DRV_SCART_GetSettingStr(HUINT32 ulIndex, char* pStr)
{
	char* strSettingValue[DI_SCART_MAX] = {	"DI_SCART_NO_SIGNAL",
											"DI_SCART_AR_4_3",
											"DI_SCART_AR_16_9",
											"DI_SCART_OUTPUT_CVBS",
											"DI_SCART_OUTPUT_RGB",
											"DI_SCART_OUTPUT_SVIDEO",
											"DI_SCART_MODE_NORMAL",
											"DI_SCART_MODE_BYPASS",
											"DI_SCART_MODE_HW_BYPASS"};
	/*
	 * "pStr" should be an array that is big enough.
	 * dcchung 2009.5.12
	 */
	if (ulIndex<DI_SCART_MAX)
		VK_strncpy(pStr, strSettingValue[ulIndex], 32);
	else
		VK_snprintf(pStr, 32, "undefined index : %d", ulIndex);
}


void DRV_SCART_PrintStatus(void)
{
	char strTemp[100];

	DI_UART_Print("Connected: %s   %s\n",  (s_ScartInfo.IsTvConnected==TRUE)? "TV":"", (s_ScartInfo.IsVcrConnected==TRUE)? "VCR":"");
	DI_UART_Print("Vol      : %d\n", s_ScartInfo.ucSoundLevel);

	DRV_SCART_GetSettingStr(s_ScartInfo.Settings.eMode, strTemp);
	DI_UART_Print("Mode     : %s\n", strTemp);

	if (s_ScartInfo.Settings.eMode==DI_SCART_MODE_NORMAL)
	{
		DRV_SCART_GetSettingStr(s_ScartInfo.Settings.stNormalMode.eTvAspectRatioOutput, strTemp);
		DI_UART_Print("TV AR    : %s\n", strTemp);

		DRV_SCART_GetSettingStr(s_ScartInfo.Settings.stNormalMode.eTvVideoOutput, strTemp);
		DI_UART_Print("TV Out   : %s\n", strTemp);

		DRV_SCART_GetSettingStr(s_ScartInfo.Settings.stNormalMode.eVcrAspectRatioOutput, strTemp);
		DI_UART_Print("VCR AR   : %s\n", strTemp);

		DRV_SCART_GetSettingStr(s_ScartInfo.Settings.stNormalMode.eVcrVideoOutput, strTemp);
		DI_UART_Print("VCR Out  : %s\n", strTemp);
	}
}


DI_ERR_CODE DI_SCART_RegisterEventNotify(DI_SCART_EVENT eScartEvent, DI_SCART_CALLBACK pfScartEventCallback)
{
	DI_ERR_CODE rc=DI_ERR_OK;

	if (eScartEvent<DS_EVENT_MAX)
	{
		s_pfCallback[eScartEvent]= pfScartEventCallback;
	}
	else
	{
		rc=DI_ERR_ERROR;
	}

#if defined(CONFIG_PRODUCT_IMAGE_FACTORY)
	/* update check count */
	s_ScartInfo.uiCheckCount = DI_SCART_CHECK_COUNT;
	s_ScartInfo.IsVcrConnected = FALSE;
#endif
	return rc;
}


void DI_SCART_GetSettings(DI_SCART_SETTINGS* pSettings)
{//Sunwoo
	VK_MEM_Memcpy(pSettings, &s_ScartInfo.Settings, sizeof(DI_SCART_SETTINGS));
}


DI_ERR_CODE DI_SCART_SetSettings(DI_SCART_SETTINGS* pSettings)
{//Sunwoo
	PrintDebug("eMode: 0x%x\n", pSettings->eMode);

/*************************
*	1. Fast Blanking
*************************/
	PrintDebug("eTvVideoOutput: 0x%x\n", pSettings->stNormalMode.eTvVideoOutput);
	PrintDebug("eVcrVideoOutput: 0x%x\n", pSettings->stNormalMode.eVcrVideoOutput);
	PrintDebug("eTvVideoInput: 0x%x\n", pSettings->stNormalMode.eTvVideoInput);


/*************************
*	2. Slow Blanking
*************************/
	PrintDebug("eTvAspectRatioOutput: 0x%x\n", pSettings->stNormalMode.eTvAspectRatioOutput);
	PrintDebug("eVcrAspectRatioOutput: 0x%x\n", pSettings->stNormalMode.eVcrAspectRatioOutput);
	PrintDebug("eTvAspectRatioInput: 0x%x\n", pSettings->stNormalMode.eTvAspectRatioInput);

	return P_SCART_SetSettings(pSettings);
}

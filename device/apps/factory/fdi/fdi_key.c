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

/* chip manufacture's  header files */

/* humax header files */
#include "fdi_key.h"

#include "di_key.h"
#include "fdi_common.h" 

#include "vkernel.h"

/*******************************************************************************
* Debug level
*******************************************************************************/

/*******************************************************************************
* Definitions, typdef & const data
*******************************************************************************/
#if defined(FDI_IR_TYPE) && (FDI_IR_TYPE == FDI_IR_TYPE_RC5) 
//RC5는 Repeat key가 구별되지 않는데, DD에서  걸러지지 않으므로 app에서 처리해야 함.
//(20120705) factory2.0에서는 RCU Test가 필수 사항이 아니므로 간단한 방법으로 repeat 처리.
//아래 define이 coding convetion에 맞는지 확인 필요
#define USE_REPEAT_FILTER
#define REPEAT_FILTER_TIMEOUT 400
#endif

#ifdef USE_REPEAT_FILTER
typedef struct FDI_IR_INFO
{
	FDI_RCU_TYPE 	usLastKeyCode;	
	HUINT32     ulPreviousTick;    
} sFDI_IR_INFO;
#endif

/*******************************************************************************
* Static variables and functions

*******************************************************************************/

#ifdef USE_REPEAT_FILTER
static sFDI_IR_INFO sFdiIrInfo;
#endif




/*******************************************************************************
* External variables and functions
*******************************************************************************/
/* external variables  and functions */

/*******************************************************************************
* Global variables and structures
*******************************************************************************/
/* global variables and structures */
FDI_KEY_EVENT_CALLBACK pfnFdiKeyEventCallback;

HINT32 INTRN_FDI_KEY_EventCallBack(DI_KEY_DEVICE eDevice, HUINT32 ulKeyCode, DI_KEY_TYPE eKeyType);

HUINT32 local_key_GetFrontKeyCode(HUINT32 ulDiKeyCode)
{
	HUINT32 nRetKey = FRONT_KEY_MAX;
	switch(ulDiKeyCode)
	{
		case DI_KEY_FRONT_VOL_DOWN:
			nRetKey = FRONT_KEY_VOLDOWN;
			break;
		case DI_KEY_FRONT_VOL_UP:
			nRetKey = FRONT_KEY_VOLUP;
			break;
		case DI_KEY_FRONT_CH_DOWN:
			nRetKey = FRONT_KEY_CHDOWN;
			break;
		case DI_KEY_FRONT_CH_UP:
			nRetKey = FRONT_KEY_CHUP;
			break;
		case DI_KEY_FRONT_MENU:
			nRetKey = FRONT_KEY_MENU;
			break;
		case DI_KEY_FRONT_INPUT:
			nRetKey = FRONT_KEY_INPUT;
			break;
		case DI_KEY_FRONT_PWR:
			nRetKey = FRONT_KEY_POWER;
			break;
		case DI_KEY_FRONT_OK:
			nRetKey = FRONT_KEY_OK;
			break;
		case DI_KEY_FRONT_DVD_PLAY_PAUSE:
			nRetKey = FRONT_KEY_PLAY;
			break;
		case DI_KEY_FRONT_DVD_STOP:
			nRetKey = FRONT_KEY_STOP;
			break;
		case DI_KEY_FRONT_DVD_OPEN_CLOSE:
			nRetKey = FRONT_KEY_EJECT;
			break;
		case DI_KEY_FRONT_TV_RADIO:
			nRetKey = FRONT_KEY_TVRADIO;
			break;
		case DI_KEY_FRONT_UP:
			nRetKey = FRONT_KEY_UP;
			break;
		case DI_KEY_FRONT_DOWN:
			nRetKey = FRONT_KEY_DOWN;
			break;
		case DI_KEY_FRONT_LEFT:
			nRetKey = FRONT_KEY_LEFT;
			break;
		case DI_KEY_FRONT_RIGHT:
			nRetKey = FRONT_KEY_RIGHT;
			break;
		case DI_KEY_FRONT_GUIDE:
			nRetKey = FRONT_KEY_GUIDE;
			break;
		case DI_KEY_FRONT_BACK:
			nRetKey = FRONT_KEY_BACK;
			break;
		case DI_KEY_FRONT_DETAIL_INFO:
			nRetKey = FRONT_KEY_INFO;
			break;
#if defined(CONFIG_DI10)
		case DI_KEY_FRONT_RECORD:
			nRetKey = FRONT_KEY_REC;
			break;
		case DI_KEY_FRONT_HDD_DISC:
			nRetKey = FRONT_KEY_HDD_DISC;
			break;
		case DI_KEY_FRONT_STOP:
			nRetKey = FRONT_KEY_STOP;
			break;
		case DI_KEY_FRONT_PLAY_LIST:
			nRetKey = FRONT_KEY_LIST;
			break;
#endif
#if defined(CONFIG_DI20)
		case DI_KEY_FRONT_WPS:
			nRetKey = FRONT_KEY_WPS;
			break;
		case  DI_KEY_FRONT_NETWORK:
			nRetKey = FRONT_KEY_NETWORK;
			break;
		case  DI_KEY_FRONT_SW_REBOOT:
			nRetKey = FRONT_KEY_SW_REBOOT;
			break;			
#endif
		default:
			break;
	}
	return nRetKey;
}

HUINT32 local_key_GetRCUKeyCode(HUINT32 ulDiKeyCode)
{
	HUINT32 nRetKey = RCU_KEY_NONE;
	switch(ulDiKeyCode)
	{
		case DI_KEY_REMOCON_STANDBY:
			nRetKey = RCU_KEY_STANDBY;
			break;
		case DI_KEY_REMOCON_0:
			nRetKey = RCU_KEY_0;
			break;
		case DI_KEY_REMOCON_1:
			nRetKey = RCU_KEY_1;
			break;
		case DI_KEY_REMOCON_2:
			nRetKey = RCU_KEY_2;
			break;
		case DI_KEY_REMOCON_3:
			nRetKey = RCU_KEY_3;
			break;
		case DI_KEY_REMOCON_4:
			nRetKey = RCU_KEY_4;
			break;
		case DI_KEY_REMOCON_5:
			nRetKey = RCU_KEY_5;
			break;
		case DI_KEY_REMOCON_6:
			nRetKey = RCU_KEY_6;
			break;
		case DI_KEY_REMOCON_7:
			nRetKey = RCU_KEY_7;
			break;
		case DI_KEY_REMOCON_8:
			nRetKey = RCU_KEY_8;
			break;
		case DI_KEY_REMOCON_9:
			nRetKey = RCU_KEY_9;
			break;
		case DI_KEY_REMOCON_11:
			nRetKey = RCU_KEY_11;
			break;
		case DI_KEY_REMOCON_12:
			nRetKey = RCU_KEY_12;
			break;
		case DI_KEY_REMOCON_RED:
			nRetKey = RCU_KEY_RED;
			break;
		case DI_KEY_REMOCON_GREEN:
			nRetKey = RCU_KEY_GREEN;
			break;
		case DI_KEY_REMOCON_YELLOW:
			nRetKey = RCU_KEY_YELLOW;
			break;
		case DI_KEY_REMOCON_BLUE:
			nRetKey = RCU_KEY_BLUE;
			break;
		case DI_KEY_REMOCON_INFO:
			nRetKey = RCU_KEY_INFO;
			break;
		case DI_KEY_REMOCON_GUIDE:
			nRetKey = RCU_KEY_GUIDE;
			break;
		case DI_KEY_REMOCON_MENU:
			nRetKey = RCU_KEY_MENU;
			break;
		case DI_KEY_REMOCON_CHLIST:
			nRetKey = RCU_KEY_LIST;
			break;
		case DI_KEY_REMOCON_VFORMAT:
			nRetKey = RCU_KEY_VFORMAT;
			break;
		case DI_KEY_REMOCON_ARROWUP:
			nRetKey = RCU_KEY_UP;
			break;
		case DI_KEY_REMOCON_ARROWDOWN:
			nRetKey = RCU_KEY_DOWN;
			break;
		case DI_KEY_REMOCON_ARROWLEFT:
			nRetKey = RCU_KEY_LEFT;
			break;
		case DI_KEY_REMOCON_ARROWRIGHT:
			nRetKey = RCU_KEY_RIGHT;
			break;
		case DI_KEY_REMOCON_BACK:
			nRetKey = RCU_KEY_BACK;
			break;
		case DI_KEY_REMOCON_EXIT:
			nRetKey = RCU_KEY_EXIT;
			break;
		case DI_KEY_REMOCON_OK:
			nRetKey = RCU_KEY_OK;
			break;
		case DI_KEY_REMOCON_CH_PLUS:
			nRetKey = RCU_KEY_CHUP;
			break;
		case DI_KEY_REMOCON_CH_MINUS:
			nRetKey = RCU_KEY_CHDOWN;
			break;
		case DI_KEY_REMOCON_LAST:
			nRetKey = RCU_KEY_LAST;
			break;			
		case DI_KEY_REMOCON_TVRADIO:
			nRetKey = RCU_KEY_TVRADIO;
			break;
		case DI_KEY_REMOCON_VOLUMEUP:
			nRetKey = RCU_KEY_VOLUP;
			break;
		case DI_KEY_REMOCON_VOLUMEDOWN:
			nRetKey = RCU_KEY_VOLDOWN;
			break;
		case DI_KEY_REMOCON_MUTE:
			nRetKey = RCU_KEY_MUTE;
			break;
		case DI_KEY_REMOCON_SOURCE:
			nRetKey = RCU_KEY_SOURCE;
			break;			
		case DI_KEY_REMOCON_PAUSE:
			nRetKey = RCU_KEY_PAUSE;
			break;
		case DI_KEY_REMOCON_PREV:
			nRetKey = RCU_KEY_PREV;
			break;
		case DI_KEY_REMOCON_DT:
			nRetKey = RCU_KEY_DT;
			break;
		case DI_KEY_REMOCON_BS:
			nRetKey = RCU_KEY_BS;
			break;
		case DI_KEY_REMOCON_CATV:
			nRetKey = RCU_KEY_CATV;
			break;
		case DI_KEY_REMOCON_DATA:
			nRetKey = RCU_KEY_DATA;
			break;			
		default:
			break;
	}
	return nRetKey;
}

/*******************************************************************************
* function : FDI_KEY_Init
* description :
* input :
* output :
* return :
* 기타 참고자료 및 파일
*******************************************************************************/
FDI_ERR_CODE FDI_KEY_Open(void)
{
	#ifdef USE_REPEAT_FILTER
	sFdiIrInfo.usLastKeyCode = RCU_KEY_MAX;
	sFdiIrInfo.ulPreviousTick = 0;
	#endif
	
	return FDI_NO_ERR;
}

/*******************************************************************************
* function : FDI_KEY_RegisterEventCallback
* description :
* input :
* output :
* return :
* 기타 참고자료 및 파일
*******************************************************************************/
FDI_ERR_CODE FDI_KEY_RegisterEventCallback(FDI_KEY_EVENT_CALLBACK pfnCallback) /* wrapping */
{
	pfnFdiKeyEventCallback = pfnCallback;
	
	DI_KEY_RegisterKeyEventCallback((pfnDI_KEY_Notify)INTRN_FDI_KEY_EventCallBack);

	return FDI_NO_ERR;
}

/*******************************************************************************
* function : 
* description :
* input :
* output :
* return :
* 기타 참고자료 및 파일
*******************************************************************************/
HINT32 INTRN_FDI_KEY_EventCallBack(DI_KEY_DEVICE eDevice, HUINT32 ulKeyCode, DI_KEY_TYPE eKeyType)
{
	FDI_KEY_DEVICE	eDev;
	FDI_KEY_TYPE	eType;
	FDI_FKEY_TYPE	eFkey;
	
	#ifdef USE_REPEAT_FILTER
	HUINT32 ulCurrentTick;
	HUINT32 ulTickInterval;
	#endif
	
	switch( eDevice )
	{
		case DI_FRONT_KEY :
			eDev = FDI_FRONT_KEY;
			eFkey = local_key_GetFrontKeyCode(ulKeyCode);
			break;
		case DI_RCU_KEY :
		default :
			eDev = FDI_RCU_KEY;
			eFkey = local_key_GetRCUKeyCode(ulKeyCode);
			break;
	}
	
	switch( eKeyType )
	{
		case DI_KEY_RELEASED :
			eKeyType = FDI_KEY_RELEASED;
			break;
		case DI_KEY_REPEATED :
			eKeyType = FDI_KEY_REPEATED;
			break;
		case DI_KEY_PRESSED :
		default :
			eKeyType = FDI_KEY_PRESSED;
			break;
	}

	#ifdef USE_REPEAT_FILTER
	if(eDev == DI_RCU_KEY)
	{
		ulCurrentTick = VK_TIMER_GetSystemTick();
		ulTickInterval = ulCurrentTick - sFdiIrInfo.ulPreviousTick;
		FDI_PRINT(FDI_PRT_DBGINFO, ("RC5 key(%x) currtick(%d) lasttick(%d)  \n", eFkey, ulCurrentTick, sFdiIrInfo.ulPreviousTick));
		sFdiIrInfo.ulPreviousTick = ulCurrentTick;
		sFdiIrInfo.usLastKeyCode = eFkey;

		
		if(ulTickInterval < REPEAT_FILTER_TIMEOUT)
		{
			FDI_PRINT(FDI_PRT_DBGINFO, ("RC5 Repeated Input filtered out (%x)(%d ms)  \n", eFkey, ulTickInterval));
			//return 0;
			eKeyType = FDI_KEY_REPEATED;
		}
	}
	#endif
		
	if( pfnFdiKeyEventCallback != NULL )
		pfnFdiKeyEventCallback(eDev, eFkey, eKeyType);
	return 0;
}


/**
 * drv_scart.h
*/

/**
 * @defgroup		DRV_SCART : DRV_SCART module
 * @author		Chung Dong Chul
 * @note			scart DD
 * @file 			drv_scart.h
 * @remarks		Copyright (c) 2008 HUMAX Co., Ltd. All rights reserved.
*/
#ifndef __DRV_SCART_H__
#define __DRV_SCART_H__

#include "htype.h"
#include "drv_err.h"

/* For HW Bypass Mode */
#define HW_BYPASS_NORMAL_MODE			0
#define HW_BYPASS_AUTOSTARTUP_MODE		1
#define HW_BYPASS_NO_DETECT_SIG_MODE	2

typedef struct {
	void (*pfSet_Normal_Mode)(void);
	void (*pfSet_VCR_Output_Off)(void);
	void (*pfReset_VCR_Output_Off)(void);
	void (*pfSet_TV_Output_Off)(void);
	void (*pfReset_TV_Output_Off)(void);
	void (*pfSet_RF_Output_Off)(void);
	void (*pfReset_RF_Output_Off)(void);
	void (*pfSet_Cinch_Output_Off)(void);
	void (*pfReset_Cinch_Output_Off)(void);
	void (*pfSet_TV_SVHS_Mode)(void);
	void (*pfReset_TV_SVHS_Mode)(void);
	void (*pfSet_TV_RGB_Mode)(void);
	void (*pfSet_TV_CVBS_Mode)(void);
	void (*pfSet_VCR_CVBS_Mode)(void);
	void (*pfSet_VCR_SVHS_Mode)(void);
	void (*pfSet_Wide_Mode)(void);
	void (*pfReset_Wide_Mode)(void);
	void (*pfSet_TVMono_Mode)(void);
	void (*pfReset_TVMono_Mode)(void);
	void (*pfSet_VCRMono_Mode)(void);
	void (*pfReset_VCRMono_Mode)(void);
	void (*pfSet_VCRbypass_Mode)(void);
	void (*pfReset_VCRbypass_Mode)(void);
	int	(*pfCheck_TV_Input)(void);
	int	(*pfCheck_VCR_Input)(void);
	int	(*pfCheck_VCR_ScreenFormat)(void);
	void (*pfSet_TV_Mute)(void);
	void (*pfReset_TV_Mute)(void);
	void (*pfSet_VCR_Mute)(void);
	void (*pfReset_VCR_Mute)(void);
	void (*pfSet_FunctionPin_On)(void);
	void (*pfReset_FunctionPin_On)(void);
	void (*pfSet_HW_Bypass)(unsigned char ucAutoDetection);
	void (*pfSet_Sound_Level)(unsigned char);
	void (*pfSet_HW_Mute)(void);
	void (*pfReset_HW_Mute)(void);
	void (*pfDump_Reg)(void);
	void (*pfWrite_Reg)(unsigned char data, unsigned char subaddr);
	void (*pfInterrupt_Enable)(void);
	void (*pfInterrupt_Disable)(void);
	void (*pfStandbyMode)(void);
	void (*pfStatusRead)(void);
} DRV_SCART_OPERATION;


DRV_Error DRV_SCART_Init(void);
DRV_Error DRV_SCART_SetTVMute(unsigned char ucMute);
DRV_Error DRV_SCART_SetVCRMute(unsigned char ucMute);
DRV_Error DRV_SCART_SetVolume(unsigned char ucVolume);

DRV_SCART_OPERATION* DRV_SCART_GetOpHandle(void);
void DRV_SCART_GetSettingStr(HUINT32 ulIndex, char* pStr);
void DRV_SCART_PrintStatus(void);

#endif /* !__DRV_SCART_H__ */



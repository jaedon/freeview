#ifndef _SCART_GPIO_H_
#define _SCART_GPIO_H_

#include "htype.h"
#include "drv_err.h"

void P_SCART_GPIO_I2C_Write_For_Debug(unsigned char data, unsigned char subaddr);
void P_SCART_GPIO_REG_Dump(void);
void P_SCART_GPIO_Normal_Mode(void);
void P_SCART_GPIO_VCR_Output_Off(void);	
void P_SCART_GPIO_VCR_Output_On(void);
void P_SCART_GPIO_TV_Output_Off(void);
void P_SCART_GPIO_TV_Output_On(void);
void P_SCART_GPIO_Wide_Mode_On(void);
void P_SCART_GPIO_Wide_Mode_Off(void);
void P_SCART_GPIO_VCRbypass_Mode_On(void);
void P_SCART_GPIO_VCRbypass_Mode_Off(void);
int P_SCART_GPIO_Check_TV_Input(void);
int P_SCART_GPIO_Check_VCR_Input(void);
int P_SCART_GPIO_Check_VCR_ScreenFormat(void);
int P_SCART_GPIO_Check_TV_ScreenFormat(void);
void P_SCART_GPIO_TV_Audio_Mute_On(void);
void P_SCART_GPIO_TV_Audio_Mute_Off(void);
void P_SCART_GPIO_VCR_Mute_On(void);
void P_SCART_GPIO_VCR_Mute_Off(void);
void P_SCART_GPIO_TV_RGB_Mode_On(void);
void P_SCART_GPIO_TV_CVBS_Mode_On(void);
void P_SCART_GPIO_VCR_CVBS_Mode_On(void);
void P_SCART_GPIO_HW_Bypass(unsigned char ucAutoDetection);
void P_SCART_GPIO_Sound_Level(unsigned char level);
void P_SCART_GPIO_HW_Mute_On(void);
void P_SCART_GPIO_HW_Mute_Off(void);
void P_SCART_GPIO_Cinch_Output_Off	(void);
void P_SCART_GPIO_Cinch_Output_On(void);
void P_SCART_GPIO_TV_SVHS_Mode_On(void);
void P_SCART_GPIO_TV_SVHS_Mode_Off(void);
void P_SCART_GPIO_RF_Output_Off(void);
void P_SCART_GPIO_RF_Output_On(void); 
void P_SCART_GPIO_TVMono_Mode_On(void);
void P_SCART_GPIO_TVMono_Mode_Off(void);
void P_SCART_GPIO_VCRMono_Mode_On(void);
void P_SCART_GPIO_VCRMono_Mode_Off(void);
void P_SCART_GPIO_FunctionPin_On(void);
void P_SCART_GPIO_FunctionPin_Off(void);
void P_SCART_GPIO_VCR_SVHS_Mode_On(void);

void Scart_Init_ChipDriver(void);

#endif


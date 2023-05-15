#ifndef _SCART_NULL_H_
#define _SCART_NULL_H_

void Scart_Normal_Mode(void);
void Scart_VCR_Output_Off(void);	
void Scart_VCR_Output_On(void);
void Scart_TV_Output_Off(void);
void Scart_TV_Output_On(void);
void Scart_Wide_Mode_On(void);
void Scart_Wide_Mode_Off(void);
void Scart_VCRbypass_Mode_On(void);
void Scart_VCRbypass_Mode_Off(void);
int Scart_Check_TV_Input(void);
int Scart_Check_VCR_Input(void);
int Scart_Check_VCR_ScreenFormat(void);
int Scart_Check_TV_ScreenFormat(void);
void Scart_TV_Audio_Mute_On(void);
void Scart_TV_Audio_Mute_Off(void);
void Scart_VCR_Mute_On(void);
void Scart_VCR_Mute_Off(void);
void Scart_TV_RGB_Mode_On(void);
void Scart_TV_CVBS_Mode_On(void);
void Scart_VCR_CVBS_Mode_On(void);
void Scart_HW_Bypass(unsigned char ucAutoDetection);
void Scart_Sound_Level(unsigned char level);
void Scart_HW_Mute_On(void);
void Scart_HW_Mute_Off(void);
void Scart_Cinch_Output_Off	(void);
void Scart_Cinch_Output_On(void);
void Scart_TV_SVHS_Mode_On(void);
void Scart_TV_SVHS_Mode_Off(void);
void Scart_RF_Output_Off(void);
void Scart_RF_Output_On(void);
void Scart_TVMono_Mode_On(void);
void Scart_TVMono_Mode_Off(void);
void Scart_VCRMono_Mode_On(void);
void Scart_VCRMono_Mode_Off(void);
void Scart_FunctionPin_On(void);
void Scart_FunctionPin_Off(void);
void Scart_VCR_SVHS_Mode_On(void);

void Scart_Init_ChipDriver(void);

#endif


#ifndef _STV6417_H_
#define _STV6417_H_

/*******************************************************************/
/**************************** Header Files**************************/
/*******************************************************************/
#include "htype.h"
#include "drv_err.h"

//WRITE ONLY
#define ADDR6417_AUD00					0x00
#define ADDR6417_AUD01					0x01
#define ADDR6417_VID02					0x02
#define ADDR6417_VID03					0x03
#define ADDR6417_MISC04					0x04
#define ADDR6417_INPUT05				0x05
#define ADDR6417_OUTCON06				0x06
#define ADDR6417_AUTOSTARTUP07			0x07
#define ADDR6417_STBY08					0x08
#define ADDR6417_AUTODETECT09			0x09

//READ ONLY
#define ADDR6412_READ00					0x00

/*****************************/
/* Register 00h: TV_Audio_Output */
/*****************************/
#define AUD00_TV_AUD_MODE_STEREO		0x00
#define AUD00_TV_AUD_MODE_MONO			0x80

#define AUD00_6DB_EXTGAIN_ON			0x40
#define AUD00_6DB_EXTGAIN_OFF			0x00

#define AUD00_LEVEL_ADJUST				0x3E

#define AUD00_VOL_CHANGE_ACT			0x00
#define AUD00_VOL_CHANGE_DIS			0x01

//MASK
#define AUD00_TV_AUD_MODE_MASK			0x80
#define AUD00_6DB_EXTGAIN_MASK			0x40
#define AUD00_LEVEL_ADJUST_MASK			0x3E
#define AUD00_VOL_CHANGE_MASK			0x01


/***********************************************/
/* Register 01h:Audio selection & TV/VCR Audio Output */
/***********************************************/
#define AUD01_VCR_AUD_CON_ACT			0x00
#define AUD01_VCR_AUD_CON_HiZ			0x40

#define AUD01_VCR_AUD_MODE_STEREO		0x00
#define AUD01_VCR_AUD_MODE_MONO			0x20

#define AUD01_VCR_AUD_OUTSEL_MUTE		0x00
#define AUD01_VCR_AUD_OUTSEL_ENC		    0x08
#define AUD01_VCR_AUD_OUTSEL_TV			0x10
#define AUD01_VCR_AUD_OUTSEL_NOTALL      0x18


#define AUD01_TV_AUD_CON_ACT			0x00
#define AUD01_TV_AUD_CON_HiZ			0x04

#define AUD01_TV_AUD_OUTSEL_MUTE		0x00
#define AUD01_TV_AUD_OUTSEL_ENC			0x01
#define AUD01_TV_AUD_OUTSEL_VCR			0x02
#define AUD01_TV_AUD_OUTSEL_TV			0x03


//MASK
#define AUD01_VCR_AUD_CON_MASK			0x40
#define AUD01_VCR_AUD_MODE_MASK			0x20
#define AUD01_VCR_AUD_OUTSEL_MASK		0x18
#define AUD01_TV_AUD_CON_MASK			0x04
#define AUD01_TV_AUD_OUTSEL_MASK		0x03



/************************************/
/* Register 02h: TV/VCR Video Selectionl  */
/************************************/
#define VID02_VCR_CHROMA_MUTE				0x80
#define VID02_VCR_CHROMA_d6d5d4				0x00

#define VID02_VCR_VIDOUT_SEL_MUTE			0x00
#define VID02_VCR_VIDOUT_SEL_CVBS_R			0x10
#define VID02_VCR_VIDOUT_SEL_Y_C			0x20
#define VID02_VCR_VIDOUT_SEL_R_G			0x30
#define VID02_VCR_VIDOUT_SEL_CVBS_C_TV		0x40

#define VID02_TV_CHROMA_MUTE				0x08
#define VID02_TV_CHROMA_d2d1d0				0x00

#define VID02_TV_VIDOUT_SEL_MUTE				0x00
#define VID02_TV_VIDOUT_SEL_CVBS_R				0x01
#define VID02_TV_VIDOUT_SEL_Y_C					0x02
#define VID02_TV_VIDOUT_SEL_YCVBS_RC_VCR		0x03
#define VID02_TV_VIDOUT_SEL_R_G					0x04

//MASK
#define VID02_VCR_CHROMA_MASK				0x80
#define VID02_VCR_VIDOUT_SEL_MASK			0x70
#define VID02_TV_CHROMA_MASK				0x08
#define VID02_TV_VIDOUT_SEL_MASK			0x07

/***************************************/
/* Register 03h: RGB & fast blanking OUTPUT  */
/***************************************/
#define VID03_RGB_CON_ACT				0x80
#define VID03_RGB_CON_HiZ				0x00

#define VID03_RGB_EXTRA_GAIN_0DB		0x00	/* 0dB Extra gain */
#define VID03_RGB_EXTRA_GAIN_3DB		0x40	/* +3dB for weak input signals */

#define VID03_RGB_GAIN_6DB				0x00
#define VID03_RGB_GAIN_5DB				0x10
#define VID03_RGB_GAIN_4DB				0x20
#define VID03_RGB_GAIN_3DB				0x30

#define VID03_RGB_SEL_MUTE				0x00
#define VID03_RGB_SEL_ENC				0x04
#define VID03_RGB_SEL_VCR				0x08

/* Fast Blanking is CVBS or RGB selection pin : SCART pin 16*/
#define VID03_FB_CON_LOW_CVBS			0x00	/* <0.5V ; CVBS sel */
#define VID03_FB_CON_HIGH_RGB			0x01	/* 3.0V< "3.4V" < 3.8V ; RGB sel */
#define VID03_FB_CON_VCR				0x02
#define VID03_FB_CON_ENC				0x03


//MASK
#define VID03_RGB_CON_MASK				0x80
#define VID03_RGB_EXTRA_GAIN_MASK		0x40
#define VID03_RGB_GAIN_MASK				0x30
#define VID03_RGB_SEL_MASK				0x0C
#define VID03_FB_CON_MASK				0x03


/*****************************************/
/* Register 04h: RF, LPF & miscellaneous control  */
/*****************************************/
#define MISC04_B_TV_OUT_GND			0x00
#define MISC04_B_TV_OUT_HiZ			0x40
#define MISC04_B_TV_OUT_ACT			0x80

#define MISC04_C_VCR_OUT_GND		0x00
#define MISC04_C_VCR_OUT_HiZ		0x10
#define MISC04_C_VCR_OUT_ACT		0x20

#define MISC04_LPF_ENABLE			0x00
#define MISC04_LPF_BYPASS			0x08

#define MISC04_RF_OUT_CVBS			0x00
#define MISC04_RF_OUT_YC_ADDER		0x04

//MASK
#define MISC04_B_TV_OUT_MASK		0xC0
#define MISC04_C_VCR_OUT_MASK		0x30
#define MISC04_LPF_MASK				0x08
#define MISC04_RF_OUT_MASK			0x04


/**************************/
/* Register 05h: Input control  */
/**************************/
#define INPUT05_RC_TV_OUTSEL_RED			0x00
#define INPUT05_RC_TV_OUTSEL_CHROMA			0x80

#define INPUT05_ENC_AUD_LEVEL_0DB			0x00
#define INPUT05_ENC_AUD_LEVEL_6DB			0x20
#define INPUT05_ENC_AUD_LEVEL_9DB			0x40

#define INPUT05_VCR_CLAMP_SEL_BOT			0x00	//RGB
#define INPUT05_VCR_CLAMP_SEL_AVR_BOT		0x08	//C, Y, Y
#define INPUT05_VCR_CLAMP_SEL_SYNC			0x10	//YPbPr

#define INPUT05_ENC_CLAMP_SEL_BOT			0x00	//RGB
#define INPUT05_ENC_CLAMP_SEL_AVR_BOT		0x02	//C, Y, Y
#define INPUT05_ENC_CLAMP_SEL_SYNC			0x04	//YPbPr

#define INPUT05_ENC_CLAMP_CON_EN			0x00	//AC Coupled
#define INPUT05_ENC_CLAMP_CON_DIS			0x01	//DC Coupled

//MASK
#define INPUT05_RC_TV_OUTSEL_MASK			0x80
#define INPUT05_ENC_AUD_LEVEL_MASK			0x60
#define INPUT05_VCR_CLAMP_SEL_MASK			0x18
#define INPUT05_ENC_CLAMP_SEL_MASK			0x06
#define INPUT05_ENC_CLAMP_CON_MASK			0x01


/*****************************************/
/* Register 06h: Slow Blanking & Output Control */
/****************************************/
#define OUTCON06_TV_YCVBS_CON_ACT			0x00
#define OUTCON06_TV_YCVBS_CON_HiZ			0x80

#define OUTCON06_VCR_YCVBS_CON_ACT			0x00
#define OUTCON06_VCR_YCVBS_CON_HiZ			0x40

/* Slow Blanking is Input source, 16:9, 4:3 selection pin : SCART pin 8*/
#define OUTCON06_VCR_SB_CON_INPUT			0x00	/* */
#define OUTCON06_VCR_SB_CON_LOW				0x10	/* <1.5V */
#define OUTCON06_VCR_SB_CON_MED_16_9		0x20	/* 5V< "5.75V" < 6.5V */
#define OUTCON06_VCR_SB_CON_HIGH_4_3		0x30	/* 10V< "11V" < 12V */

#define OUTCON06_TV_SB_CON_INPUT			0x00	/* */
#define OUTCON06_TV_SB_CON_LOW				0x04	/* <1.5V */
#define OUTCON06_TV_SB_CON_MED_16_9			0x08	/* 5V< "5.75V" < 6.5V */
#define OUTCON06_TV_SB_CON_HIGH_4_3			0x0C	/* 10V< "11V" < 12V */

#define OUTCON06_IT_DISABLE			0x00
#define OUTCON06_IT_ENABLE			0x02

#define OUTCON06_TV_SB_MODE_NORMAL			0x00
#define OUTCON06_TV_SB_MODE_BY_VCR			0x01

//MASK
#define OUTCON06_TV_YCVBS_CON_MASK			0x80
#define OUTCON06_VCR_YCVBS_CON_MASK			0x40
#define OUTCON06_VCR_SB_CON_MASK			0x30
#define OUTCON06_TV_SB_CON_MASK				0x0C
#define OUTCON06_IT_MASK					0x02
#define OUTCON06_TV_SB_MODE_MASK			0x01

/*******************************/
/* Register 07h: Auto-startup mode */
/*******************************/
#define AUTOSTARTUP07_DET_DIS					0x00
#define AUTOSTARTUP07_TV_DET_EN					0x08
#define AUTOSTARTUP07_VCR_DET_EN				0x10
#define AUTOSTARTUP07_TV_VCR_DET_EN				0x18
#define AUTOSTARTUP07_DET_DIS_SB_EN				0x20
#define AUTOSTARTUP07_TV_DET_EN_SB_EN			0x28
#define AUTOSTARTUP07_VCR_DET_EN_SB_EN			0x30
#define AUTOSTARTUP07_TV_VCR_DET_EN_SB_EN		0x38

//MASK
#define AUTOSTARTUP07_MASK						0x38

/***************************/
/*Register 08h: Standby modes */
/***************************/
#define STBY08_FULL_STOP			0xFF	/* only IIC bus and slow blanking detectionparts are supplied */

#define STBY08_CVBS_VCR_DET_DIS		0x00
#define STBY08_CVBS_VCR_DET_EN		0x80

#define STBY08_CVBS_TV_DET_DIS		0x00
#define STBY08_CVBS_TV_DET_EN		0x40

#define STBY08_RF_OUTPUT_ON			0x00
#define STBY08_RF_OUTPUT_OFF		0x20

#define STBY08_TV_OUTPUT_ON			0x00
#define STBY08_TV_OUTPUT_OFF		0x10

#define STBY08_VCR_OUTPUT_ON		0x00
#define STBY08_VCR_OUTPUT_OFF		0x08

#define STBY08_TV_INPUT_ACT			0x00
#define STBY08_TV_INPUT_DIS			0x04

#define STBY08_VCR_INPUT_ACT		0x00
#define STBY08_VCR_INPUT_DIS		0x02

#define STBY08_ENC_INPUT_ACT		0x00
#define STBY08_ENC_INPUT_DIS		0x01

/****************************************/
/*Register 09h: Test Pattern for auto-detection  */
/****************************************/
/*
In order to avoid this issue, a simple and safe software workaround can be applied by
asserting d7d6d5 bits to 100 or 010, instead of 000. However, setting d7d6d5 bits to 010 is
recommended as it provides a good increase of sensitivity while keeping some margin to
not get triggered by any parasitic signals.
Asserting d7d6d5 bits to 010 affects only the ¡° turn-on time ¡±. Therefore, there is no side
effect to be expected.
*/
#define AUTODETECT09_WORKAROUND			0x80


/******************************/
/* 00h : Output signal (Read Mode) */
/******************************/
#define READ00_SB_TV_LOW					0x01	/* Input < 2V */
#define READ00_SB_TV_MED_16_9				0x02	/* Input 16/9 format */
#define READ00_SB_TV_HIGH_4_3				0x03	/* Input 4/3 format */

#define READ00_SB_VCR_LOW					0x04	/* Input < 2V */
#define READ00_SB_VCR_MED_16_9				0x08	/* Input 16/9 format */
#define READ00_SB_VCR_HIGH_4_3				0x0C	/* Input 4/3 format */

#define READ00_VCR_DET_NO					0x00
#define READ00_VCR_DET_Y_CVBS				0x10	

#define READ00_TV_DET_NO					0x00
#define READ00_TV_DET_Y_CVBS				0x20	

#define READ00_IT_FLAG_NO_CHANGE			0x00
#define READ00_IT_FLAG_DET_CHANGE			0x40	

//MASK
#define READ00_IT_FLAG_MASK			0x40
#define READ00_TV_DET_MASK			0x20
#define READ00_VCR_DET_MASK			0x10
#define READ00_SB_VCR_MASK			0x0C
#define READ00_SB_TV_MASK			0x03


/* Switch YPbPr or RGB Select */
#define YPBPR_RGB_PIN_NUM					13
#define RGB_SELECTION						1
#define YPBPR_SELECTION						0

void P_STV6417_I2C_Write_For_Debug(unsigned char data, unsigned char subaddr);
void P_STV6417_REG_Dump(void);
void P_STV6417_Normal_Mode(void);
void P_STV6417_VCR_Output_Off(void);	
void P_STV6417_VCR_Output_On(void);
void P_STV6417_TV_Output_Off(void);
void P_STV6417_TV_Output_On(void);
void P_STV6417_Wide_Mode_On(void);
void P_STV6417_Wide_Mode_Off(void);
void P_STV6417_VCRbypass_Mode_On(void);
void P_STV6417_VCRbypass_Mode_Off(void);
int P_STV6417_Check_TV_Input(void);
int P_STV6417_Check_VCR_Input(void);
int P_STV6417_Check_VCR_ScreenFormat(void);
int P_STV6417_Check_TV_ScreenFormat(void);
void P_STV6417_TV_Audio_Mute_On(void);
void P_STV6417_TV_Audio_Mute_Off(void);
void P_STV6417_VCR_Mute_On(void);
void P_STV6417_VCR_Mute_Off(void);
void P_STV6417_TV_RGB_Mode_On(void);
void P_STV6417_TV_CVBS_Mode_On(void);
void P_STV6417_VCR_CVBS_Mode_On(void);
void P_STV6417_HW_Bypass(unsigned char ucAutoDetection);
void P_STV6417_Sound_Level(unsigned char level);
void P_STV6417_HW_Mute_On(void);
void P_STV6417_HW_Mute_Off(void);
void P_STV6417_Cinch_Output_Off	(void);
void P_STV6417_Cinch_Output_On(void);
void P_STV6417_TV_SVHS_Mode_On(void);
void P_STV6417_TV_SVHS_Mode_Off(void);
void P_STV6417_RF_Output_Off(void);
void P_STV6417_RF_Output_On(void); 
void P_STV6417_TVMono_Mode_On(void);
void P_STV6417_TVMono_Mode_Off(void);
void P_STV6417_VCRMono_Mode_On(void);
void P_STV6417_VCRMono_Mode_Off(void);
void P_STV6417_FunctionPin_On(void);
void P_STV6417_FunctionPin_Off(void);
void P_STV6417_VCR_SVHS_Mode_On(void);

void Scart_Init_ChipDriver(void);

#endif

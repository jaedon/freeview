/**
 * stv6417.c
*/

/**
 * @defgroup		stv6417 : stv6417 module
 * @author		Chung Dong Chul
 * @note
 * @brief
 * @file 			stv6417.c
*/


/*******************************************************************/
/**************************** Header Files *************************/
/*******************************************************************/

#include "bstd_defs.h"

/* driver include */
#include "drv_i2c.h"
#include "di_scart.h"
#include "drv_scart.h"
#include "stv6417.h"

#if defined(CONFIG_VK_STDLIB)
#else
#include 	<stdio.h>
#include 	<stdlib.h>
#include 	<string.h>
#endif

/*******************************************************************/
/****************************** define *****************************/
/*******************************************************************/

//#define STV6417_DEBUG
//#define STV6417_DEBUG_ENTER_EXIT

#ifdef STV6417_DEBUG
#define PrintData		VK_printf
#define PrintDebug	VK_printf
#define PrintError	VK_printf
#else
#define PrintData		while (0) ((int (*)(char *, ...)) 0)
#define PrintDebug	while (0) ((int (*)(char *, ...)) 0)
#define PrintError	VK_printf
#endif

#ifdef STV6417_DEBUG_ENTER_EXIT
#define PrintEnter	VK_printf("(+)%s\n", __FUNCTION__)
#define PrintExit		VK_printf("(-)%s\n", __FUNCTION__)
#else
#define PrintEnter
#define PrintExit
#endif

/* Scart I2C Sub_address */
#define SCART_CTRL_READ	    (0x97>>1)
#define SCART_CTRL_WRITE	    (0x96>>1)

#define DDI_ERR_CODE DRV_Error
#define DDI_NO_ERR DRV_OK

/* Scart I2C Channel of BCM7403 */
#define SCART_I2C_CHANNEL_NUM			(I2C_CHANNEL_SCART)

/* Scart I2C Sub_address */
#define SCART_I2C_ADDR 					0x4B	//STV6417	1001 011x

#define PIN_DISABLE 	0
#define PIN_ENABLE 		1

#define STV6417_AUDIO_MUTE_OFF	0
#define STV6417_AUDIO_MUTE_ON	  1

#define CONFIG_STV6417_INTERRUPT_ENABLE


#define uint8_t HUINT8



/*******************************************************************/
/****************************** typedef ****************************/
/*******************************************************************/


/*******************************************************************/
/************************ global variables *************************/
/*******************************************************************/
extern int g_ScartAutoStartUpMode;


/*******************************************************************/
/************************ static variables *************************/
/*******************************************************************/
static DRV_SCART_OPERATION* s_pScartOp;



// s_STV6417_REG[] shows the current status of the scart register values.
/*
 * 1. SCART Controller IC (���⼭�� STV6417) �� register �ʱⰪ
 * 2.SCART Controller IC (���⼭�� STV6417) register���� ���� �� ���� ������ local�ϰ� �����ϱ� ����.
 *    I2C bus�� ���� STV6417�� register�� ���� ������ ������ �� ������
 *    ���� ������ �����ϰ� �� ������ ���� ������ �־�� ��
 */
static unsigned char s_STV6417_REG[10] =
	/* 0x0 : TV Audio Output */
	{ 0x3E,
	/* 0x1 : Audio selection & TV/VCR Audio Output */
	  0x09,
	/* 0x2 : TV/VCR Video Selectionl */
	  0x11,
	/* 0x3 : RGB & fast blanking OUTPUT */
	  0x84,
	/* 0x4 : RF, LPF & miscellaneous control */
	  0x80,
	/* 0x5 : Input control */
	  0x0A,
	/* 0x6 : Slow Blanking & Output Control */
	  0x0C,
	/* 0x7 : Auto-startup mode */
	  0x00,
	/* 0x8 : Standby modes */
	  0x00,
	/* 0x9 : Auto detection */
	  0x00	  } ;

static unsigned char s_Level2DB[21] =
{
	//0x00,
	0x1F, /* maybe mute -62dB, min level */
	0x13, 0x12, 0x11, 0x10, 0x0F,
	0x0E, 0x0D, 0x0C, 0x0B, 0x0A,
	0x09, 0x08, 0x07, 0x06, 0x05,
	0x04, 0x03, 0x02, 0x01, 0x00 /* max 0dB */
} ;

static unsigned char _v3iPinFlag;

static unsigned char s_ucCurrent_TV_Audio_Mute_State = STV6417_AUDIO_MUTE_OFF;
static unsigned char s_ucPrevious_TV_Audio_Mute_State = STV6417_AUDIO_MUTE_OFF;
static unsigned char s_ucCurrent_VCR_Audio_Mute_State = STV6417_AUDIO_MUTE_OFF;
static unsigned char s_ucPrevious_VCR_Audio_Mute_State = STV6417_AUDIO_MUTE_OFF;

static unsigned char s_ucVCR_Bypass = FALSE;

unsigned char P_STV6417_GetPinStatus(void)
{
	PrintData("SCART PIN GET (%d)\n\r", _v3iPinFlag);
	return _v3iPinFlag;
}


void P_STV6417_SetPinStatus(unsigned char flag)
{
	PrintData("SCART PIN SET (%d)\n\r", flag);
	_v3iPinFlag = flag;
}

static uint8_t scart_write_data;

void P_STV6417_I2C_Write(uint8_t data, uint8_t subaddr)
{
	DDI_ERR_CODE	returnCode = DDI_NO_ERR;

	scart_write_data = data;

	returnCode = DRV_I2c_WriteA8(SCART_I2C_CHANNEL_NUM, SCART_I2C_ADDR, subaddr, &scart_write_data, 1);
	if (returnCode != DDI_NO_ERR)
	{
		PrintError("[P_STV6417_I2C_Write]SCART_I2C_CHANNEL_NUM(0x%x), SCART_I2C_ADDR(0x%x), subaddr(0x%x), data(0x%x).... \n",
													SCART_I2C_CHANNEL_NUM, SCART_I2C_ADDR, subaddr, scart_write_data);
	}
}

void P_STV6417_I2C_Write_For_Debug(unsigned char data, unsigned char subaddr)
{
	s_STV6417_REG[subaddr] = data;
	P_STV6417_I2C_Write(s_STV6417_REG[subaddr], subaddr);
}


static uint8_t scart_read_data;

/* �ٸ� ������ I2C �� ���� READ �� �� ����. �׷��Ƿ�, s_STV6417_REG[7] �� ���� �����Ѵ�. */
uint8_t P_STV6417_Status_Read(void)
{
	DDI_ERR_CODE	returnCode = DDI_NO_ERR;

	//DDI_I2C_read(SCART_I2C_CHANNEL_NUM, SCART_I2C_ADDR, ADDR6412_READ00 , &data, 1);
	/* SCART STV6417 Read ��, I2C subaddress ���� SLAVE ADDRESS �� �̿��ؼ� read �ؾ� �Ѵ�. */
	/* �׷��Ƿ� DDI_I2C_read �� �ƴ϶�, DDI_I2C_noaddrread �� ����ؾ߸� �Ѵ�. */

	returnCode = DRV_I2c_ReadNoAddr(SCART_I2C_CHANNEL_NUM, SCART_I2C_ADDR, &scart_read_data, 1);
	if (returnCode != DDI_NO_ERR)
	{
		PrintError("[P_STV6417_Status_Read]DDI_I2C_noaddrread ERROR ~~!! : SCART_I2C_CHANNEL_NUM(0x%x), SCART_I2C_ADDR(0x%x), data(0x%x)\n",
													SCART_I2C_CHANNEL_NUM, SCART_I2C_ADDR,  scart_read_data);
	}

	return scart_read_data;
}

/**************************************
 *	Register Value Dump
 **************************************/
void s_STV6417_REG_Dump(void)
{
	int i ;

	PrintError("STV6417_REG_Dump\n") ;

	for(i = 0 ; i < (ADDR6417_AUTODETECT09+1) ; i++)
	{
		if (i == ADDR6417_AUD00)
			PrintError("REG[0x00:TV Audio Output] -(0x%02X)\n", s_STV6417_REG[ADDR6417_AUD00]);
		else if (i== ADDR6417_AUD01)
			PrintError("REG[0x01:Audio Selection and TV/VCR Audio Output] -(0x%02X)\n", s_STV6417_REG[ADDR6417_AUD01]);
		else if (i== ADDR6417_VID02)
			PrintError("REG[0x02: TV/VCR Video Selection] -(0x%02X)\n", s_STV6417_REG[ADDR6417_VID02]);
		else if (i== ADDR6417_VID03)
			PrintError("REG[0x03: RGB & Fast Blanking Outputs] -(0x%02X)\n", s_STV6417_REG[ADDR6417_VID03]);
		else if (i== ADDR6417_MISC04)
			PrintError("REG[0x04: RF, LPF & Miscellaneous Control] -(0x%02X)\n", s_STV6417_REG[ADDR6417_MISC04]);
		else if (i== ADDR6417_INPUT05)
			PrintError("REG[0x05: Inputs Control] -(0x%02X)\n", s_STV6417_REG[ADDR6417_INPUT05]);
		else if (i== ADDR6417_OUTCON06)
			PrintError("REG[0x06: Slow blanking, Interrupt & Outputs Control] -(0x%02X)\n", s_STV6417_REG[ADDR6417_OUTCON06]);
		else if (i== ADDR6417_AUTOSTARTUP07)
			PrintError("REG[0x07: Auto-startup Mode] -(0x%02X)\n", s_STV6417_REG[ADDR6417_AUTOSTARTUP07]);
		else if (i== ADDR6417_STBY08)
			PrintError("REG[0x08: Standby Modes] -(0x%02X)\n", s_STV6417_REG[ADDR6417_STBY08]);
		else if (i== ADDR6417_AUTODETECT09)
			PrintError("REG[0x09: Auto Detection] -(0x%02X)\n", s_STV6417_REG[ADDR6417_AUTODETECT09]);
	}
	PrintError("READ Only -> REG[0x00: Output Signals] -(0x%02X)\n", P_STV6417_Status_Read());

}


//GPIO�� Pin�� ���ؼ� Interrupt�� �ٸ� VCR���� �˷��ִ� ����� �Ѵ�.
void Switch_RGB_Mode(int mode)
{
#if 1
    mode = mode;
#else
	DDI_ERR_CODE ret=DDI_NO_ERR;

	PrintEnter;

	PrintData("[DRV_SCART : Switch_RGB_Mode]mode = 0x%x\n",mode);
	//ret=hgio_write(YPBPR_RGB_PIN_NUM, mode);
///////////////////////////////////////	PIN Type Ȯ�����ּ���.
	ret = DDI_GPIO_Write(GPIO_VIDEO_SW, mode);
	if(ret != DDI_NO_ERR) {
		PrintError("[Switch_RGB_Mode] DDI_GPIO_write error ret=0x%x\n",ret);
	}

	PrintExit;
#endif
}


/*******************/
/*	Set Normal Mode*/
/*******************/
void P_STV6417_Normal_Mode(void)
{

	// R:00h
	// TV Audio Mode : stereo
	// Extra Gain : 6dB	->	HDMI volume �� ���̰� ����, �켱 extra 6dB gain 'ON'  ���ش�.
	// TV Audio Level  : 0dB
	// Soft Volume Change : Active
	s_STV6417_REG[ADDR6417_AUD00] = AUD00_TV_AUD_MODE_STEREO| AUD00_6DB_EXTGAIN_OFF|((s_Level2DB[20])<<1) | AUD00_VOL_CHANGE_ACT;
	P_STV6417_I2C_Write(s_STV6417_REG[ADDR6417_AUD00], ADDR6417_AUD00);

	// R:01h
	// Input Source for TV Audio : Encoder Audio
	// Input Source for VCR Audio : Encoder Audio
	// TV & VCR Audio output : Active
	// VCR Audio Mode : stereo
	s_STV6417_REG[ADDR6417_AUD01] = AUD01_VCR_AUD_CON_ACT | AUD01_VCR_AUD_MODE_STEREO | AUD01_VCR_AUD_OUTSEL_ENC | AUD01_TV_AUD_CON_ACT | AUD01_TV_AUD_OUTSEL_ENC;
	P_STV6417_I2C_Write(s_STV6417_REG[ADDR6417_AUD01], ADDR6417_AUD01);

	//R:02h
	//TV Video CVBS/Y & R/C: Y/CVBS_ENC & R/C_ENC
	//VCR Video CVBS/Y & R/C: Y/CVBS_ENC & R/C_ENC
	s_STV6417_REG[ADDR6417_VID02] = VID02_VCR_CHROMA_d6d5d4 |VID02_VCR_VIDOUT_SEL_CVBS_R |VID02_TV_CHROMA_d2d1d0 | VID02_TV_VIDOUT_SEL_CVBS_R;
	P_STV6417_I2C_Write(s_STV6417_REG[ADDR6417_VID02], ADDR6417_VID02);

 	//R:03h
 	// RGB & fast blanking : Active
 	// RGB Extra gain : 0dB
 	// RGB gain : 6dB
 	// RGB output : RGB_ENC
 	// Fast Blanking signal : LOW (CVBS sel)
	s_STV6417_REG[ADDR6417_VID03] = VID03_RGB_CON_ACT| VID03_RGB_EXTRA_GAIN_0DB | VID03_RGB_GAIN_6DB | VID03_RGB_SEL_ENC | VID03_FB_CON_LOW_CVBS;
	P_STV6417_I2C_Write(s_STV6417_REG[ADDR6417_VID03], ADDR6417_VID03);

 	//R:04h
	//B_TV : active
	//C_VCR : GND
	//LPF : Enable
	//RF Out : CVBS
	s_STV6417_REG[ADDR6417_MISC04] = MISC04_B_TV_OUT_ACT | MISC04_C_VCR_OUT_GND | MISC04_LPF_ENABLE | MISC04_RF_OUT_CVBS;
	P_STV6417_I2C_Write(s_STV6417_REG[ADDR6417_MISC04], ADDR6417_MISC04);

	//R:05h
	//R/C TV : Red signal
	//Encoder input level of audio : 0dB (Normal Audio)
	//VCR Clamp : average + bottom clamp
	//Encoder Clamp : average + bottom clamp
	//vider encoder clamp : Enable
	s_STV6417_REG[ADDR6417_INPUT05] = INPUT05_RC_TV_OUTSEL_RED | INPUT05_ENC_AUD_LEVEL_9DB | INPUT05_VCR_CLAMP_SEL_AVR_BOT | INPUT05_ENC_CLAMP_SEL_AVR_BOT | INPUT05_ENC_CLAMP_CON_DIS;
	P_STV6417_I2C_Write(s_STV6417_REG[ADDR6417_INPUT05], ADDR6417_INPUT05);


 	//R:06h
 	//Y/CVBS_TV Out : Active
 	//Y/CVBS_VCR Out : Active
 	//VCR Slow Blanking : Input mode only
 	//TV Slow Blanking : High (4:3 format)
 	//Interrupt : Disable
 	//Slow Blanking Mode : Normal
	s_STV6417_REG[ADDR6417_OUTCON06] = OUTCON06_TV_YCVBS_CON_ACT| OUTCON06_VCR_YCVBS_CON_ACT |
									OUTCON06_VCR_SB_CON_INPUT | OUTCON06_TV_SB_CON_HIGH_4_3 | OUTCON06_IT_DISABLE | OUTCON06_TV_SB_MODE_NORMAL;
#ifdef CONFIG_STV6417_INTERRUPT_ENABLE
    /* Interrupt�� enable ��Ų��. */
	s_STV6417_REG[ADDR6417_OUTCON06] |= OUTCON06_IT_ENABLE;
#endif
	P_STV6417_I2C_Write(s_STV6417_REG[ADDR6417_OUTCON06], ADDR6417_OUTCON06);

 	//R:07h
 	//Auto Startup : SLB Enable & Detection Disable
	s_STV6417_REG[ADDR6417_AUTOSTARTUP07] = AUTOSTARTUP07_DET_DIS_SB_EN;
	P_STV6417_I2C_Write(s_STV6417_REG[ADDR6417_AUTOSTARTUP07], ADDR6417_AUTOSTARTUP07);

	//R:08h
	//CVBS TV, VCR Detectinon : Disable
	// TV/VCR/RF Output : ON
	// ENC/TV/VCR Input : Active
	s_STV6417_REG[ADDR6417_STBY08] = STBY08_CVBS_VCR_DET_DIS | STBY08_CVBS_TV_DET_DIS | STBY08_RF_OUTPUT_ON |STBY08_TV_OUTPUT_ON |STBY08_VCR_OUTPUT_ON |\
									STBY08_TV_INPUT_ACT | STBY08_VCR_INPUT_ACT | STBY08_ENC_INPUT_ACT;
#ifdef CONFIG_STV6417_INTERRUPT_ENABLE
	/* VCR Detection (Insert/Remove)�� enable ��Ų��. */
	/* Interrupt�� Enable �� ��� VCR Insert/Remove �� Interrupt�� �߻��Ѵ�. */
	s_STV6417_REG[ADDR6417_STBY08] |= STBY08_CVBS_VCR_DET_EN;
#endif
	P_STV6417_I2C_Write(s_STV6417_REG[ADDR6417_STBY08], ADDR6417_STBY08);

	// VCR_CVBS/Y detect bug ����  SCART_IT �̻��� workaround
	P_STV6417_I2C_Write(AUTODETECT09_WORKAROUND, ADDR6417_AUTODETECT09);
	s_STV6417_REG[ADDR6417_AUTODETECT09]|=AUTODETECT09_WORKAROUND;

	/* Audio Mute State Update */
	s_ucCurrent_TV_Audio_Mute_State = STV6417_AUDIO_MUTE_OFF;
	s_ucCurrent_VCR_Audio_Mute_State = STV6417_AUDIO_MUTE_OFF;
}


/*********************************/
/*   Change VCR Output enable status  */
/*********************************/
void P_STV6417_VCR_Output_Off(void)
{
	/* VCR AV Output ON(Output enabled) -> OFF(Output disabled); bit set */
	s_STV6417_REG[ADDR6417_STBY08] |= STBY08_VCR_OUTPUT_OFF;
	P_STV6417_I2C_Write(s_STV6417_REG[ADDR6417_STBY08], ADDR6417_STBY08);
}
void P_STV6417_VCR_Output_On(void)
{
	/* VCR AV Output OFF -> ON ; bit clear */
	s_STV6417_REG[ADDR6417_STBY08] &= ~(STBY08_VCR_OUTPUT_OFF);
	P_STV6417_I2C_Write(s_STV6417_REG[ADDR6417_STBY08], ADDR6417_STBY08);
}

/*******************************/
/*	Change TV Output enable status */
/*******************************/
void P_STV6417_TV_Output_Off(void)
{
	/* TV AV Output ON(Output enabled) -> OFF(Output disabled) ; bit set*/
	s_STV6417_REG[ADDR6417_STBY08] |= STBY08_TV_OUTPUT_OFF;
	P_STV6417_I2C_Write(s_STV6417_REG[ADDR6417_STBY08], ADDR6417_STBY08);
}
void P_STV6417_TV_Output_On(void)
{
	/* TV AV Output OFF -> ON ; bit clear */
	s_STV6417_REG[ADDR6417_STBY08] &= ~(STBY08_TV_OUTPUT_OFF);
	P_STV6417_I2C_Write(s_STV6417_REG[ADDR6417_STBY08], ADDR6417_STBY08);
}

/****************************************/
/*	Change RF Modulator Output enable status  */
/****************************************/
//STV6417 ���� RF_VOUT �� CVBS �� ����ȴ�. (��, �׻� ON �� ��Ģ�̴�.)

void P_STV6417_RF_Output_Off(void)
{
	/* RF AV Output ON(Output enabled) -> OFF(Output disabled) ; bit set*/
	s_STV6417_REG[ADDR6417_STBY08] |= STBY08_RF_OUTPUT_OFF;
	P_STV6417_I2C_Write(s_STV6417_REG[ADDR6417_STBY08], ADDR6417_STBY08);
}
void P_STV6417_RF_Output_On(void)
{
	/* RF AV Output OFF -> ON ; bit clear */
	s_STV6417_REG[ADDR6417_STBY08] &= ~(STBY08_RF_OUTPUT_OFF);
	P_STV6417_I2C_Write(s_STV6417_REG[ADDR6417_STBY08], ADDR6417_STBY08);
}

/*********************************/
/* Change Cinch Output enable status  */
/*********************************/
void P_STV6417_Cinch_Output_Off	(void)
{
	//STV6417 ���� RF_VOUT �� CVBS �� ����ȴ�. (��, �׻� ON �� ��Ģ�̴�.)
	//��, CVBS_Cinch == RF
	P_STV6417_RF_Output_Off();

	//CINCH Audio OFF(Mute) �ϸ�, SCART TV Audio �� mute �ǹǷ�, ���⼭�� �������� �ʴ´�.
	//�ʿ��ϴٸ� �ٸ� �Լ� call ����.
}
void P_STV6417_Cinch_Output_On(void)
{

	//STV6417 ���� RF_VOUT �� CVBS �� ����ȴ�. (��, �׻� ON �� ��Ģ�̴�.)
	//��, CVBS_Cinch == RF
	P_STV6417_RF_Output_On();

	//CINCH Audio ON �ϸ�, SCART TV Audio �� On �ǹǷ�, ���⼭�� �������� �ʴ´�.
	//�ʿ��ϴٸ� �ٸ� �Լ� call ����.
}

/***************************/
/* Change Video Output Format */
/***************************/
// 4:3 ratio -> 16:9 ratio On
void P_STV6417_Wide_Mode_On(void)
{

 	//Set TV Function Switching : 16:9 ratio
	s_STV6417_REG[ADDR6417_OUTCON06] &= ~(OUTCON06_TV_SB_CON_MASK);			//clear TV function (slow blanking) bits
	s_STV6417_REG[ADDR6417_OUTCON06] |= OUTCON06_TV_SB_CON_MED_16_9;		//set TV function bits to 16:9 Aspect Ratio

	if (P_STV6417_GetPinStatus())
	{
		P_STV6417_I2C_Write(s_STV6417_REG[ADDR6417_OUTCON06], ADDR6417_OUTCON06);
	}

}


//16:9 ratio -> 4:3 ratio On
void P_STV6417_Wide_Mode_Off(void)
{
	//Set TV Function Switching : 4:3 ratio
	s_STV6417_REG[ADDR6417_OUTCON06] &= ~(OUTCON06_TV_SB_CON_MASK);			//clear TV function (slow blanking) bits
	s_STV6417_REG[ADDR6417_OUTCON06] |= OUTCON06_TV_SB_CON_HIGH_4_3;		//set TV function bits to 4:3 Aspect Ratio

	if (P_STV6417_GetPinStatus())	//
	{
		P_STV6417_I2C_Write(s_STV6417_REG[ADDR6417_OUTCON06], ADDR6417_OUTCON06);
	}

}

// STV6412_TVMono_Mode_On
void P_STV6417_TVMono_Mode_On(void)
{
	/* Stereo -> Mono */
	s_STV6417_REG[ADDR6417_AUD00] |= AUD00_TV_AUD_MODE_MONO;
	P_STV6417_I2C_Write(s_STV6417_REG[ADDR6417_AUD00], ADDR6417_AUD00);
}


// STV6412_TVMono_Mode_Off
void P_STV6417_TVMono_Mode_Off(void)
{
	/* Mono -> Stereo */
	s_STV6417_REG[ADDR6417_AUD00] &= ~AUD00_TV_AUD_MODE_MASK;
	s_STV6417_REG[ADDR6417_AUD00] |= AUD00_TV_AUD_MODE_STEREO;
	P_STV6417_I2C_Write(s_STV6417_REG[ADDR6417_AUD00], ADDR6417_AUD00);
}

// STV6412_VCRMono_Mode_On
void P_STV6417_VCRMono_Mode_On(void)
{
	unsigned char ucScartRegValue = 0;

	/* Stereo -> Mono */
	s_STV6417_REG[ADDR6417_AUD01] |= AUD01_VCR_AUD_MODE_MONO;

	ucScartRegValue = s_STV6417_REG[ADDR6417_AUD01];

	if(s_ucCurrent_VCR_Audio_Mute_State == STV6417_AUDIO_MUTE_ON)
	{
		/* VCR Audio�� Mute On ���¶�� Vcr Audio Output Selection�� mute�� */
		ucScartRegValue = (ucScartRegValue & (~AUD01_VCR_AUD_OUTSEL_MASK));
	}

	if(s_ucCurrent_TV_Audio_Mute_State == STV6417_AUDIO_MUTE_ON)
	{
		/* TV Audio�� Mute On ���¶�� TV Audio Output Selection�� mute�� */
		ucScartRegValue = (ucScartRegValue & (~AUD01_TV_AUD_OUTSEL_MASK));
	}

	P_STV6417_I2C_Write(ucScartRegValue, ADDR6417_AUD01);
}

// STV6412_VCRMono_Mode_Off
void P_STV6417_VCRMono_Mode_Off(void)
{
	unsigned char ucScartRegValue = 0;

	/* Mono -> Stereo */
	s_STV6417_REG[ADDR6417_AUD01] &= ~AUD01_VCR_AUD_MODE_MASK;
	s_STV6417_REG[ADDR6417_AUD01] |= AUD01_VCR_AUD_MODE_STEREO;

	ucScartRegValue = s_STV6417_REG[ADDR6417_AUD01];

	if(s_ucCurrent_VCR_Audio_Mute_State == STV6417_AUDIO_MUTE_ON)
	{
		/* VCR Audio�� Mute On ���¶�� Vcr Audio Output Selection�� mute�� */
		ucScartRegValue = (ucScartRegValue & (~AUD01_VCR_AUD_OUTSEL_MASK));
	}

	if(s_ucCurrent_TV_Audio_Mute_State == STV6417_AUDIO_MUTE_ON)
	{
		/* TV Audio�� Mute On ���¶�� TV Audio Output Selection�� mute�� */
		ucScartRegValue = (ucScartRegValue & (~AUD01_TV_AUD_OUTSEL_MASK));
	}

	P_STV6417_I2C_Write(ucScartRegValue, ADDR6417_AUD01);
}

void P_STV6417_VCRbypass_Mode_On(void)
{
	unsigned char temp_MISC04_reg, temp_INPUT05_reg;

	PrintEnter;
	if (s_ucVCR_Bypass == TRUE)
	{
		PrintError("[P_STV6417_VCRbypass_Mode_Off] already VCR bypass on\n") ;
		return;
	}

	/* Normal -> VCR bypass */

	s_STV6417_REG[ADDR6417_AUD00]  &= ~(AUD00_6DB_EXTGAIN_MASK | AUD00_LEVEL_ADJUST_MASK);
	s_STV6417_REG[ADDR6417_AUD00] |= (AUD00_6DB_EXTGAIN_OFF|(s_Level2DB[20] <<1));
	P_STV6417_I2C_Write(s_STV6417_REG[ADDR6417_AUD00], ADDR6417_AUD00);

	s_STV6417_REG[ADDR6417_AUD01] &= ~(AUD01_TV_AUD_CON_MASK | AUD01_TV_AUD_OUTSEL_MASK);
	s_STV6417_REG[ADDR6417_AUD01] |= (AUD01_TV_AUD_CON_ACT | AUD01_TV_AUD_OUTSEL_VCR);
	P_STV6417_I2C_Write(s_STV6417_REG[ADDR6417_AUD01], ADDR6417_AUD01);

	s_STV6417_REG[ADDR6417_VID02] &= ~VID02_TV_VIDOUT_SEL_MASK;
	s_STV6417_REG[ADDR6417_VID02] |= VID02_TV_VIDOUT_SEL_YCVBS_RC_VCR;
	P_STV6417_I2C_Write(s_STV6417_REG[ADDR6417_VID02], ADDR6417_VID02);

	s_STV6417_REG[ADDR6417_VID03] &= ~(VID03_RGB_SEL_MASK | VID03_FB_CON_MASK);
	s_STV6417_REG[ADDR6417_VID03] |= (VID03_RGB_SEL_VCR | VID03_FB_CON_VCR);
	P_STV6417_I2C_Write(s_STV6417_REG[ADDR6417_VID03], ADDR6417_VID03);

	//if s-vhs, disable
	temp_MISC04_reg = s_STV6417_REG[ADDR6417_MISC04];
	temp_INPUT05_reg = s_STV6417_REG[ADDR6417_INPUT05];

	temp_MISC04_reg &= ~MISC04_C_VCR_OUT_MASK;
	temp_MISC04_reg |= MISC04_C_VCR_OUT_GND;

	temp_INPUT05_reg &= ~INPUT05_RC_TV_OUTSEL_MASK;
	temp_INPUT05_reg |= INPUT05_RC_TV_OUTSEL_RED;

	P_STV6417_I2C_Write(temp_MISC04_reg, ADDR6417_MISC04);
	P_STV6417_I2C_Write(temp_INPUT05_reg, ADDR6417_INPUT05);

	/* VCR�� Video Input Format ������ ��� TV�� Video Output Format ���� */
	s_STV6417_REG[ADDR6417_OUTCON06] &= ~OUTCON06_TV_SB_CON_MASK;
	s_STV6417_REG[ADDR6417_OUTCON06] |= OUTCON06_TV_SB_MODE_BY_VCR;
	P_STV6417_I2C_Write(s_STV6417_REG[ADDR6417_OUTCON06], ADDR6417_OUTCON06);

	//VCR output disable
	s_STV6417_REG[ADDR6417_STBY08] |= STBY08_VCR_OUTPUT_OFF;
	P_STV6417_I2C_Write(s_STV6417_REG[ADDR6417_STBY08], ADDR6417_STBY08);

	// VCR_CVBS/Y detect bug ����  SCART_IT �̻��� workaround
	P_STV6417_I2C_Write(AUTODETECT09_WORKAROUND, ADDR6417_AUTODETECT09);
	s_STV6417_REG[ADDR6417_AUTODETECT09]|=AUTODETECT09_WORKAROUND;

	/* Update Audio Mute State */
	s_ucPrevious_VCR_Audio_Mute_State = s_ucCurrent_VCR_Audio_Mute_State;
	s_ucPrevious_TV_Audio_Mute_State = s_ucCurrent_TV_Audio_Mute_State;
	s_ucCurrent_VCR_Audio_Mute_State = STV6417_AUDIO_MUTE_OFF;
	s_ucCurrent_TV_Audio_Mute_State = STV6417_AUDIO_MUTE_OFF;

	s_ucVCR_Bypass = TRUE;
}

void P_STV6417_VCRbypass_Mode_Off(void)
{
	unsigned char ucScartRegValue = 0;

	PrintEnter;
	if (s_ucVCR_Bypass == FALSE)
	{
		PrintError("[P_STV6417_VCRbypass_Mode_Off] already VCR bypass off\n") ;
		return;
	}

	/* VCR bypass -> Normal */

	/* volume don't care and +6dB gain off=>on */
	//s_STV6417_REG[ADDR6417_AUD00]  &= ~AUD00_6DB_EXTGAIN_MASK;
	//s_STV6417_REG[ADDR6417_AUD00] |= AUD00_6DB_EXTGAIN_ON;
	//P_STV6417_I2C_Write(s_STV6417_REG[ADDR6417_AUD00], ADDR6417_AUD00);

	/* set TV & VCR Audio source from ENCoder */
	s_STV6417_REG[ADDR6417_AUD01] &= ~(AUD01_VCR_AUD_OUTSEL_MASK | AUD01_TV_AUD_OUTSEL_MASK);
	s_STV6417_REG[ADDR6417_AUD01] |= (AUD01_VCR_AUD_OUTSEL_ENC | AUD01_TV_AUD_OUTSEL_ENC);

	/* Update Audio Mute State */
	s_ucCurrent_VCR_Audio_Mute_State = s_ucPrevious_VCR_Audio_Mute_State;
	s_ucCurrent_TV_Audio_Mute_State = s_ucPrevious_TV_Audio_Mute_State;
	s_ucPrevious_VCR_Audio_Mute_State = STV6417_AUDIO_MUTE_OFF;
	s_ucPrevious_TV_Audio_Mute_State = STV6417_AUDIO_MUTE_OFF;

	ucScartRegValue = s_STV6417_REG[ADDR6417_AUD01];

	if(s_ucCurrent_VCR_Audio_Mute_State == STV6417_AUDIO_MUTE_ON)
	{
		/* VCR Audio�� Mute On ���¶�� Vcr Audio Output Selection�� mute�� */
		ucScartRegValue = (ucScartRegValue & (~AUD01_VCR_AUD_OUTSEL_MASK));
	}

	if(s_ucCurrent_TV_Audio_Mute_State == STV6417_AUDIO_MUTE_ON)
	{
		/* TV Audio�� Mute On ���¶�� TV Audio Output Selection�� mute�� */
		ucScartRegValue = (ucScartRegValue & (~AUD01_TV_AUD_OUTSEL_MASK));
	}

	P_STV6417_I2C_Write(ucScartRegValue, ADDR6417_AUD01);

	s_STV6417_REG[ADDR6417_VID02] &= ~(VID02_VCR_VIDOUT_SEL_MASK | VID02_TV_VIDOUT_SEL_MASK);
	s_STV6417_REG[ADDR6417_VID02] |= (VID02_VCR_VIDOUT_SEL_CVBS_R | VID02_TV_VIDOUT_SEL_CVBS_R);
	P_STV6417_I2C_Write(s_STV6417_REG[ADDR6417_VID02], ADDR6417_VID02);


	s_STV6417_REG[ADDR6417_VID03] &= ~(VID03_RGB_SEL_MASK | VID03_FB_CON_MASK);
	s_STV6417_REG[ADDR6417_VID03] |= (VID03_RGB_SEL_ENC | VID03_FB_CON_LOW_CVBS);
	P_STV6417_I2C_Write(s_STV6417_REG[ADDR6417_VID03], ADDR6417_VID03);

	//if s-vhs, recover
	P_STV6417_I2C_Write(s_STV6417_REG[ADDR6417_MISC04], ADDR6417_MISC04);
	P_STV6417_I2C_Write(s_STV6417_REG[ADDR6417_INPUT05], ADDR6417_INPUT05);

	s_STV6417_REG[ADDR6417_OUTCON06] &= ~OUTCON06_TV_SB_CON_MASK;
	P_STV6417_I2C_Write(s_STV6417_REG[ADDR6417_OUTCON06], ADDR6417_OUTCON06);

	//VCR output enaable
	s_STV6417_REG[ADDR6417_STBY08] &= ~STBY08_VCR_OUTPUT_OFF;
	P_STV6417_I2C_Write(s_STV6417_REG[ADDR6417_STBY08], ADDR6417_STBY08);

	// VCR_CVBS/Y detect bug ����  SCART_IT �̻��� workaround
	P_STV6417_I2C_Write(AUTODETECT09_WORKAROUND, ADDR6417_AUTODETECT09);
	s_STV6417_REG[ADDR6417_AUTODETECT09]|=AUTODETECT09_WORKAROUND;

	s_ucVCR_Bypass = FALSE;

}

// STV6412_Check_TV_Input || TV Input�� ratio�� Ȯ���Ѵ�.
// 1�� bit �� '1' �̸�(&0x02), TV slow blanking input �� �����Ѵٶ�� �ǹ���.
int P_STV6417_Check_TV_Input(void)
{

	if(P_STV6417_Status_Read() & 0x02)
		return 1; /* TV Input is available  */
	else
		return 0; /* TV Input is not available */

}


// STV6412_Check_VCR_Input || VCR Input�� ratio�� Ȯ���Ѵ�.
// 3�� bit �� '1' �̸�(&0x08), VCR slow blanking input �� �����Ѵٶ�� �ǹ���.
// STV6417�� ��� VCR detect bit�� ������.. (bit 4)�� ����ؾ� ��Ȯ�� detect �Ҽ� ����.
int P_STV6417_Check_VCR_Input(void)
{
	if(P_STV6417_Status_Read() & 0x08)
		return 1; /* VCR Input is available  */
	else
		return 0; /* VCR Input is not available */
}


// STV6412_Check_VCR_ScreenFormat ||
int P_STV6417_Check_VCR_ScreenFormat(void)
{

	if(P_STV6417_Status_Read() & 0x04)
		return 1 ;	/* 4 : 3 */
	else
		return 0 ;	/* 16 : 9 */
}

int P_STV6417_Check_TV_ScreenFormat(void)
{
	if(P_STV6417_Status_Read() & 0x01)
		return 1 ;	/* 4 : 3 */
	else
		return 0 ;	/* 16 : 9 */

}

void P_STV6417_HW_Mute_On(void)
{
	unsigned char ucScartRegValue = 0;

	/*************************/
	/* TV VCR Audio Mute On      */
	/*************************/
	ucScartRegValue = (s_STV6417_REG[ADDR6417_AUD01] & (~(AUD01_TV_AUD_OUTSEL_MASK|AUD01_VCR_AUD_OUTSEL_MASK)));

	if (s_ucVCR_Bypass == FALSE)
		P_STV6417_I2C_Write(ucScartRegValue, ADDR6417_AUD01);

	s_ucCurrent_TV_Audio_Mute_State = STV6417_AUDIO_MUTE_ON;
	s_ucCurrent_VCR_Audio_Mute_State = STV6417_AUDIO_MUTE_ON;
}

void P_STV6417_HW_Mute_Off(void)
{
	/*************************/
	/* TV VCR Audio Mute Off      */
	/*************************/
	if (s_ucVCR_Bypass == FALSE)
		P_STV6417_I2C_Write(s_STV6417_REG[ADDR6417_AUD01], ADDR6417_AUD01);

	s_ucCurrent_TV_Audio_Mute_State = STV6417_AUDIO_MUTE_OFF;
	s_ucCurrent_VCR_Audio_Mute_State = STV6417_AUDIO_MUTE_OFF;
}

void P_STV6417_TV_Audio_Mute_On(void)
{
	unsigned char ucScartRegValue = 0;

	/***************************/
	/* TV Audio Output Mute On      */
	/***************************/
	if(s_ucCurrent_VCR_Audio_Mute_State == STV6417_AUDIO_MUTE_ON)
	{
		/* ���� VCR Audio�� Mute �����̸� VCR / TV Audio�� Mute On ���·� �Ѵ�. */
		ucScartRegValue = (s_STV6417_REG[ADDR6417_AUD01] & (~(AUD01_VCR_AUD_OUTSEL_MASK|AUD01_TV_AUD_OUTSEL_MASK)));
	}
	else
	{
		/* TV Audio�� Mute On ���·� �Ѵ�. */
		ucScartRegValue = (s_STV6417_REG[ADDR6417_AUD01] & (~AUD01_TV_AUD_OUTSEL_MASK));
	}

	if (s_ucVCR_Bypass == FALSE)
	{
		P_STV6417_I2C_Write(ucScartRegValue, ADDR6417_AUD01);
	}

	s_ucCurrent_TV_Audio_Mute_State = STV6417_AUDIO_MUTE_ON;
}

void P_STV6417_TV_Audio_Mute_Off(void)
{
	unsigned char ucScartRegValue = 0;

	/***************************/
	/* TV Audio Output Mute Off      */
	/***************************/
	if(s_ucCurrent_VCR_Audio_Mute_State == STV6417_AUDIO_MUTE_ON)
	{
		/* ���� VCR Audio�� Mute �����̸� VCR Audio�� Mute On ���·� �Ѵ�. */
		ucScartRegValue = (s_STV6417_REG[ADDR6417_AUD01] & (~AUD01_VCR_AUD_OUTSEL_MASK));
	}
	else
	{
		/* TV Audio Output�� Mute �� Selection���� ���� */
		ucScartRegValue = s_STV6417_REG[ADDR6417_AUD01];
	}

	if (s_ucVCR_Bypass == FALSE)
	{
		P_STV6417_I2C_Write(ucScartRegValue, ADDR6417_AUD01);
	}

	s_ucCurrent_TV_Audio_Mute_State = STV6417_AUDIO_MUTE_OFF;
}

void P_STV6417_VCR_Mute_On(void)
{
	unsigned char ucScartRegValue = 0;

	/***************************/
	/* VCR audio Output Mute On    */
	/***************************/
	if(s_ucCurrent_TV_Audio_Mute_State == STV6417_AUDIO_MUTE_ON)
	{
		/* ���� TV Audio�� Mute �����̸� VCR / TV Audio�� Mute On ���·� �Ѵ�. */
		ucScartRegValue = (s_STV6417_REG[ADDR6417_AUD01] & (~(AUD01_VCR_AUD_OUTSEL_MASK|AUD01_TV_AUD_OUTSEL_MASK)));
	}
	else
	{
		/* VCR Audio�� Mute On ���·� �Ѵ�. */
		ucScartRegValue = (s_STV6417_REG[ADDR6417_AUD01] & (~AUD01_VCR_AUD_OUTSEL_MASK));
	}

	if (s_ucVCR_Bypass == FALSE)
	{
		P_STV6417_I2C_Write(ucScartRegValue, ADDR6417_AUD01);
	}

	s_ucCurrent_VCR_Audio_Mute_State = STV6417_AUDIO_MUTE_ON;
}

void P_STV6417_VCR_Mute_Off(void)
{
	unsigned char ucScartRegValue = 0;

	/***************************/
	/* VCR Audio Output Mute Off    */
	/***************************/
	if(s_ucCurrent_TV_Audio_Mute_State == STV6417_AUDIO_MUTE_ON)
	{
		/* ���� TV Audio�� Mute On �����̸� TV Audio�� Mute On ���·� �Ѵ�. */
		ucScartRegValue = (s_STV6417_REG[ADDR6417_AUD01] & (~AUD01_TV_AUD_OUTSEL_MASK));
	}
	else
	{
		/* VCR Audio Output�� Mute �� Selection���� ���� */
		ucScartRegValue = s_STV6417_REG[ADDR6417_AUD01];
	}

	if (s_ucVCR_Bypass == FALSE)
	{
		P_STV6417_I2C_Write(ucScartRegValue, ADDR6417_AUD01);
	}

	s_ucCurrent_VCR_Audio_Mute_State = STV6417_AUDIO_MUTE_OFF;
}

void P_STV6417_TV_RGB_Mode_On(void)
{
	Switch_RGB_Mode(RGB_SELECTION);
	PrintData("RGB Mode ON : RGB/YPbPr select to RGB(H='1')\n\r ");

	/* TV Video Out : CVBS -> RGB */
	s_STV6417_REG[ADDR6417_VID03]  &= ~VID03_RGB_SEL_MASK;
	s_STV6417_REG[ADDR6417_VID03]  |= VID03_RGB_SEL_ENC;
	P_STV6417_I2C_Write(s_STV6417_REG[ADDR6417_VID03], ADDR6417_VID03);

	/* Bottom Level Clamp */
	//s_STV6417_REG[ADDR6417_INPUT05]  &= ~INPUT05_ENC_CLAMP_SEL_MASK;
	//s_STV6417_REG[ADDR6417_INPUT05]  |= INPUT05_ENC_CLAMP_SEL_BOT;
	//P_STV6417_I2C_Write(s_STV6417_REG[ADDR6417_INPUT05], ADDR6417_INPUT05);

	//VK_TASK_Sleep(40);

	/* fast blanking setting  to HIGH (RGB) */
	s_STV6417_REG[ADDR6417_VID03]  &= ~VID03_FB_CON_MASK;
	s_STV6417_REG[ADDR6417_VID03]  |= VID03_FB_CON_HIGH_RGB;
	P_STV6417_I2C_Write(s_STV6417_REG[ADDR6417_VID03], ADDR6417_VID03);

}

void P_STV6417_TV_CVBS_Mode_On(void)
{
	Switch_RGB_Mode(YPBPR_SELECTION);

	/* TV Video Out : RGB -> CVBS */
	s_STV6417_REG[ADDR6417_VID03]  &= ~(VID03_RGB_SEL_MASK | VID03_FB_CON_MASK);
	s_STV6417_REG[ADDR6417_VID03]  |= (VID03_RGB_SEL_MUTE | VID03_FB_CON_LOW_CVBS);
	P_STV6417_I2C_Write(s_STV6417_REG[ADDR6417_VID03], ADDR6417_VID03);

	/* Bottom Level Clamp */
	//s_STV6417_REG[ADDR6417_INPUT05]  &= ~INPUT05_ENC_CLAMP_SEL_MASK;
	//s_STV6417_REG[ADDR6417_INPUT05]  |= INPUT05_ENC_CLAMP_SEL_BOT;
	//P_STV6417_I2C_Write(s_STV6417_REG[ADDR6417_INPUT05], ADDR6417_INPUT05);
}

/*********************************/
/* Change SVHS Output enable status  */
/*********************************/
void P_STV6417_TV_SVHS_Mode_On(void)
{

	//Switch_RGB_Mode(YPBPR_SELECTION);

	//R/C_TV output selection "Chroma"
	s_STV6417_REG[ADDR6417_INPUT05] |= INPUT05_RC_TV_OUTSEL_CHROMA;
	P_STV6417_I2C_Write(s_STV6417_REG[ADDR6417_INPUT05], ADDR6417_INPUT05);

	s_STV6417_REG[ADDR6417_VID02]   &= ~(VID02_TV_CHROMA_MASK|VID02_TV_VIDOUT_SEL_MASK);		//clear chroma control & TV video out bits
	s_STV6417_REG[ADDR6417_VID02]  |= (VID02_TV_CHROMA_d2d1d0 | VID02_TV_VIDOUT_SEL_Y_C);		// TV SVHS(S-video) ��� �� Y_ENC, C_ENC �� set
	P_STV6417_I2C_Write(s_STV6417_REG[ADDR6417_VID02], ADDR6417_VID02);

	// CVBS/Y_TV , R/C_TV �� ���� ENC_Y, ENC_C  �� ������ ���õǹǷ�,
	// STV6417 ���� CVBS_OUT ���� ���Ǵ� RF_CVBS_OUT �� Y/C adder �� ���� �����Ǿ�� �Ѵ�.
	s_STV6417_REG[ADDR6417_MISC04] &= ~MISC04_RF_OUT_MASK;
	s_STV6417_REG[ADDR6417_MISC04] |= MISC04_RF_OUT_YC_ADDER;
	P_STV6417_I2C_Write(s_STV6417_REG[ADDR6417_MISC04], ADDR6417_MISC04);
}

void P_STV6417_TV_SVHS_Mode_Off(void)
{
	//R/C_TV output selection "Chroma"
	s_STV6417_REG[ADDR6417_INPUT05] &= ~INPUT05_RC_TV_OUTSEL_MASK;		//CLEAR R/C_TV sel bit
	s_STV6417_REG[ADDR6417_INPUT05] |= INPUT05_RC_TV_OUTSEL_RED;		//set Red signal (for RGB)
	P_STV6417_I2C_Write(s_STV6417_REG[ADDR6417_INPUT05], ADDR6417_INPUT05);

	s_STV6417_REG[ADDR6417_VID02]  &= ~VID02_TV_VIDOUT_SEL_MASK;		//clear chroma control & TV video out bits
	s_STV6417_REG[ADDR6417_VID02]  |= VID02_TV_VIDOUT_SEL_CVBS_R;		// default ��� �� CVBS_ENC, R_ENC �� set
	P_STV6417_I2C_Write(s_STV6417_REG[ADDR6417_VID02], ADDR6417_VID02);

	// CVBS/Y_TV , R/C_TV �� ���� ENC_CVBS, ENC_R  �� ������ ���õǹǷ�,
	// STV6417 ���� CVBS_OUT ���� ���Ǵ� RF_CVBS_OUT �� CVBS �� �������� set �Ѵ�.
	s_STV6417_REG[ADDR6417_MISC04] &= ~MISC04_RF_OUT_MASK;
	s_STV6417_REG[ADDR6417_MISC04] |= MISC04_RF_OUT_CVBS;
	P_STV6417_I2C_Write(s_STV6417_REG[ADDR6417_MISC04], ADDR6417_MISC04);
}

void P_STV6417_VCR_CVBS_Mode_On(void)
{
	//Switch_RGB_Mode(YPBPR_SELECTION);

	/* if STV6412, C_GATE Output -> High level, C_VCR Output -> High-Z */
	/* Use RED/C_VCR Input (pull-down to GND<COUT_VCR>*/
	s_STV6417_REG[ADDR6417_MISC04]  &= ~MISC04_C_VCR_OUT_MASK;
	s_STV6417_REG[ADDR6417_MISC04]  |= MISC04_C_VCR_OUT_GND;
	P_STV6417_I2C_Write(s_STV6417_REG[ADDR6417_MISC04], ADDR6417_MISC04);

	/* VCR Video Out as CVBS */
	/* Y/CVBS_ENC & R/C_ENC In Vcr Output Mode */
	s_STV6417_REG[ADDR6417_VID02]  &= ~VID02_VCR_VIDOUT_SEL_MASK;
	s_STV6417_REG[ADDR6417_VID02]  |= VID02_VCR_VIDOUT_SEL_CVBS_R;
	P_STV6417_I2C_Write(s_STV6417_REG[ADDR6417_VID02], ADDR6417_VID02);
}

void P_STV6417_VCR_SVHS_Mode_On(void)
{
	/* if STV6412, C_GATE Output -> Low level, C_VCR Output -> Active */
	/* Use COUT_VCR Output */
	s_STV6417_REG[ADDR6417_MISC04]  &= ~MISC04_C_VCR_OUT_MASK;
	s_STV6417_REG[ADDR6417_MISC04]  |= MISC04_C_VCR_OUT_ACT;
	P_STV6417_I2C_Write(s_STV6417_REG[ADDR6417_MISC04], ADDR6417_MISC04);

	/* VCR Video Out as SVHS */
	s_STV6417_REG[ADDR6417_VID02]  &= ~(VID02_VCR_CHROMA_MASK|VID02_VCR_VIDOUT_SEL_MASK);		//clear chroma control & VCR video out bits
	s_STV6417_REG[ADDR6417_VID02]  |= (VID02_VCR_CHROMA_d6d5d4 | VID02_VCR_VIDOUT_SEL_Y_C);		//VCR SVHS(S-video) ��� �� Y_ENC, C_ENC �� set
	P_STV6417_I2C_Write(s_STV6417_REG[ADDR6417_VID02], ADDR6417_VID02);
}

// STV6412_Set_HW_Bypass
void P_STV6417_HW_Bypass(unsigned char ucAutoDetection)
{
    PrintEnter;
	s_STV6417_REG[ADDR6417_AUD00] = AUD00_TV_AUD_MODE_STEREO|AUD00_6DB_EXTGAIN_OFF|(s_Level2DB[20] <<1)|AUD00_VOL_CHANGE_DIS;
	s_STV6417_REG[ADDR6417_AUD01] = AUD01_VCR_AUD_CON_ACT|AUD01_VCR_AUD_MODE_STEREO|AUD01_VCR_AUD_OUTSEL_TV|\
								AUD01_TV_AUD_CON_ACT|AUD01_TV_AUD_OUTSEL_VCR;
	s_STV6417_REG[ADDR6417_VID02] = VID02_VCR_CHROMA_d6d5d4 | VID02_VCR_VIDOUT_SEL_CVBS_C_TV|\
								VID02_TV_CHROMA_d2d1d0 | VID02_TV_VIDOUT_SEL_YCVBS_RC_VCR;
	s_STV6417_REG[ADDR6417_VID03] = VID03_RGB_CON_ACT | VID03_RGB_EXTRA_GAIN_0DB | VID03_RGB_GAIN_6DB |\
								VID03_RGB_SEL_VCR | VID03_FB_CON_VCR;
	s_STV6417_REG[ADDR6417_MISC04] = MISC04_B_TV_OUT_ACT | MISC04_C_VCR_OUT_GND | MISC04_LPF_ENABLE | MISC04_RF_OUT_CVBS;
	s_STV6417_REG[ADDR6417_INPUT05] = INPUT05_RC_TV_OUTSEL_RED | INPUT05_ENC_AUD_LEVEL_9DB | INPUT05_VCR_CLAMP_SEL_BOT |\
								INPUT05_ENC_CLAMP_SEL_BOT | INPUT05_ENC_CLAMP_CON_DIS;
	s_STV6417_REG[ADDR6417_OUTCON06] = OUTCON06_TV_YCVBS_CON_ACT | OUTCON06_VCR_YCVBS_CON_ACT | OUTCON06_VCR_SB_CON_INPUT | OUTCON06_TV_SB_CON_INPUT |\
								OUTCON06_IT_DISABLE | OUTCON06_TV_SB_MODE_BY_VCR;
#if 1
	// 2007.08.21. HW ���Կ� ����� ��û�� ���� standby �� VCR DET �� enable ��Ų��. (pop-noise ���� but power 0.09W up)
	//s_STV6417_REG[ADDR6417_AUTOSTARTUP07] = AUTOSTARTUP07_VCR_DET_EN_SB_EN;
	// 2007. 10. 09. Rev0.5 B/D ���� GPIO38 �� ���� HW_MUTE control �ϹǷ�, AUTO START-UP mode �� ����.

	switch( ucAutoDetection )
	{
		case HW_BYPASS_AUTOSTARTUP_MODE :
			s_STV6417_REG[ADDR6417_AUTOSTARTUP07] = AUTOSTARTUP07_TV_VCR_DET_EN_SB_EN;
			s_STV6417_REG[ADDR6417_STBY08] = STBY08_CVBS_VCR_DET_EN | STBY08_CVBS_TV_DET_EN | STBY08_RF_OUTPUT_OFF | STBY08_TV_OUTPUT_ON |\
										STBY08_VCR_OUTPUT_ON | STBY08_TV_INPUT_ACT | STBY08_VCR_INPUT_ACT | STBY08_ENC_INPUT_DIS;
			break;

		case HW_BYPASS_NO_DETECT_SIG_MODE :
			s_STV6417_REG[ADDR6417_AUTOSTARTUP07]=0;
			s_STV6417_REG[ADDR6417_STBY08]=0;
			break;

		case HW_BYPASS_NORMAL_MODE :
		default :
			// Standby power < 2W �� �����ʿ� ���� Pop-noise ���Ÿ� ���� AUTO Startup Mode ����. (Standby power 0.5W up)  2007. 10. 16. kycho
			s_STV6417_REG[ADDR6417_AUTOSTARTUP07] = AUTOSTARTUP07_DET_DIS_SB_EN;
			s_STV6417_REG[ADDR6417_STBY08] = STBY08_CVBS_VCR_DET_DIS | STBY08_CVBS_TV_DET_DIS | STBY08_RF_OUTPUT_OFF | STBY08_TV_OUTPUT_ON |\
										STBY08_VCR_OUTPUT_ON | STBY08_TV_INPUT_ACT | STBY08_VCR_INPUT_ACT | STBY08_ENC_INPUT_DIS;
	}
#else	// AUTO STARTUP �����ϰ� POP Noise ��� ���ؼ�...
	// Standby power < 2W �� �����ʿ� ���� Pop-noise ���Ÿ� ���� AUTO Startup Mode ����. (Standby power 0.5W up)  2007. 10. 16. kycho
	s_STV6417_REG[ADDR6417_AUTOSTARTUP07] = AUTOSTARTUP07_DET_DIS_SB_EN;
	s_STV6417_REG[ADDR6417_STBY08] = STBY08_CVBS_VCR_DET_DIS | STBY08_CVBS_TV_DET_DIS | STBY08_RF_OUTPUT_OFF | STBY08_TV_OUTPUT_ON |\
								STBY08_VCR_OUTPUT_ON | STBY08_TV_INPUT_ACT | STBY08_VCR_INPUT_ACT | STBY08_ENC_INPUT_DIS;
#endif
	P_STV6417_I2C_Write(s_STV6417_REG[ADDR6417_AUD00], ADDR6417_AUD00);
	P_STV6417_I2C_Write(s_STV6417_REG[ADDR6417_AUD01], ADDR6417_AUD01);
	P_STV6417_I2C_Write(s_STV6417_REG[ADDR6417_VID02], ADDR6417_VID02);
	P_STV6417_I2C_Write(s_STV6417_REG[ADDR6417_VID03], ADDR6417_VID03);
	P_STV6417_I2C_Write(s_STV6417_REG[ADDR6417_MISC04], ADDR6417_MISC04);
	P_STV6417_I2C_Write(s_STV6417_REG[ADDR6417_INPUT05], ADDR6417_INPUT05);
	P_STV6417_I2C_Write(s_STV6417_REG[ADDR6417_OUTCON06], ADDR6417_OUTCON06);
	/*
	auto-start up ���� �� ������, audio signal �� by-pass ���� �ʰ�,
	audio output �� (-) �� drive �ϸ� �̰��� pop noise �� ������ �ȴ�.
	�켱�� ���� �Ҹ� �켱�� �Ͽ�, pop-noise �� �����ϴ� �� ���·� ���� �ϸ�,
	���� hw ���� solution �� ã�ƺ���. ->  2007. 10. 09. Rev0.5 B/D ���� GPIO38 �� ���� HW_MUTE control.
	*/
	P_STV6417_I2C_Write(s_STV6417_REG[ADDR6417_AUTOSTARTUP07], ADDR6417_AUTOSTARTUP07);
	P_STV6417_I2C_Write(s_STV6417_REG[ADDR6417_STBY08], ADDR6417_STBY08);

	// VCR_CVBS/Y detect bug ����  SCART_IT �̻��� workaround
	P_STV6417_I2C_Write(AUTODETECT09_WORKAROUND, ADDR6417_AUTODETECT09);
	s_STV6417_REG[ADDR6417_AUTODETECT09]|=AUTODETECT09_WORKAROUND;

}

//level is +0dB~--62dB
void P_STV6417_Sound_Level(unsigned char level)
{
	PrintData("[P_STV6417_Sound_Level] level %d s_Level2DB[level]=0x%X\n", level, s_Level2DB[level]) ;

	s_STV6417_REG[ADDR6417_AUD00] &= ~AUD00_LEVEL_ADJUST_MASK;
	s_STV6417_REG[ADDR6417_AUD00] |= (s_Level2DB[level] << 1);
	P_STV6417_I2C_Write(s_STV6417_REG[ADDR6417_AUD00], ADDR6417_AUD00);
}

//STV6412_FunctionPin_On
void P_STV6417_FunctionPin_On(void)
{
	PrintEnter;

	P_STV6417_SetPinStatus(PIN_ENABLE);
	/* Function Pin Off -> On */
	P_STV6417_I2C_Write(s_STV6417_REG[ADDR6417_OUTCON06], ADDR6417_OUTCON06);
}

//STV6412_FunctionPin_Off
void P_STV6417_FunctionPin_Off(void)
{
	unsigned char	SB_Bit_clear;

	PrintEnter;

	P_STV6417_SetPinStatus(PIN_DISABLE);
	/* Function Pin On -> Off */
	SB_Bit_clear = ~(OUTCON06_VCR_SB_CON_MASK|OUTCON06_TV_SB_CON_MASK);
	P_STV6417_I2C_Write( (s_STV6417_REG[ADDR6417_OUTCON06]& SB_Bit_clear)|OUTCON06_TV_SB_CON_LOW, ADDR6417_OUTCON06);
}

//STV6417_Interrupt_Enable
void P_STV6417_Interrupt_Enable(void)
{
	PrintEnter;
	/* STV6417�� Interrupt�� enable ��Ų��. */
	s_STV6417_REG[ADDR6417_OUTCON06] |=  OUTCON06_IT_ENABLE;
	P_STV6417_I2C_Write(s_STV6417_REG[ADDR6417_OUTCON06], ADDR6417_OUTCON06);
}

//STV6417_Interrupt_Disable
void P_STV6417_Interrupt_Disable(void)
{
	PrintEnter;
	/* STV6417�� Interrupt�� disable ��Ų��. */
	s_STV6417_REG[ADDR6417_OUTCON06] &= (~ OUTCON06_IT_ENABLE);
	P_STV6417_I2C_Write(s_STV6417_REG[ADDR6417_OUTCON06], ADDR6417_OUTCON06);
}

void P_STV6417_Read(void)
{
	uint8_t	status_data;

	PrintEnter;

	/* SCART-STV6417 Status Register�� ���� read (Interrup�� clear ��) �ϰ� Scart Interrupt�� Enable�ؾ� �ȴ�.*/
	status_data = P_STV6417_Status_Read();

	PrintData("[P_STV6417_Read] Output Signals(0x%02X)\n", status_data);
}

void P_STV6417_StandbyMode(void)
{
	PrintEnter;

	/* STV6417�� Full Stop Mode On */
	s_STV6417_REG[ADDR6417_STBY08] = 0xff;
	P_STV6417_I2C_Write(s_STV6417_REG[ADDR6417_STBY08], ADDR6417_STBY08);
}


/*****************************/
/* Register 00h: TV_Audio_Control */
/*****************************/
void Scart_Init_ChipDriver(void)
{
	s_pScartOp=DRV_SCART_GetOpHandle();
	if (s_pScartOp!=NULL)
	{
		s_pScartOp->pfDump_Reg				= s_STV6417_REG_Dump;
		s_pScartOp->pfWrite_Reg				= P_STV6417_I2C_Write_For_Debug;
		s_pScartOp->pfSet_Normal_Mode		= P_STV6417_Normal_Mode ;
		s_pScartOp->pfSet_VCR_Output_Off		= P_STV6417_VCR_Output_Off ;
		s_pScartOp->pfReset_VCR_Output_Off	= P_STV6417_VCR_Output_On ;
		s_pScartOp->pfSet_TV_Output_Off		= P_STV6417_TV_Output_Off ;
		s_pScartOp->pfReset_TV_Output_Off		= P_STV6417_TV_Output_On ;
		s_pScartOp->pfSet_RF_Output_Off		= P_STV6417_RF_Output_Off ;
		s_pScartOp->pfReset_RF_Output_Off		= P_STV6417_RF_Output_On ;
		s_pScartOp->pfSet_Cinch_Output_Off	= P_STV6417_Cinch_Output_Off ;
		s_pScartOp->pfReset_Cinch_Output_Off	= P_STV6417_Cinch_Output_On ;
		s_pScartOp->pfSet_TV_SVHS_Mode		= P_STV6417_TV_SVHS_Mode_On;
		s_pScartOp->pfReset_TV_SVHS_Mode	= P_STV6417_TV_SVHS_Mode_Off;
		s_pScartOp->pfSet_Wide_Mode			= P_STV6417_Wide_Mode_On ;
		s_pScartOp->pfReset_Wide_Mode		= P_STV6417_Wide_Mode_Off;
		s_pScartOp->pfSet_TVMono_Mode		= P_STV6417_TVMono_Mode_On;
		s_pScartOp->pfReset_TVMono_Mode		= P_STV6417_TVMono_Mode_Off ;
		s_pScartOp->pfSet_VCRMono_Mode		= P_STV6417_VCRMono_Mode_On ;
		s_pScartOp->pfReset_VCRMono_Mode	= P_STV6417_VCRMono_Mode_Off ;
		s_pScartOp->pfSet_VCRbypass_Mode	= P_STV6417_VCRbypass_Mode_On ;
		s_pScartOp->pfReset_VCRbypass_Mode	= P_STV6417_VCRbypass_Mode_Off ;
		s_pScartOp->pfCheck_TV_Input			= P_STV6417_Check_TV_Input ;
		s_pScartOp->pfCheck_VCR_Input		= P_STV6417_Check_VCR_Input ;
		s_pScartOp->pfCheck_VCR_ScreenFormat	= P_STV6417_Check_VCR_ScreenFormat ;
		s_pScartOp->pfSet_TV_Mute			= P_STV6417_TV_Audio_Mute_On ;
		s_pScartOp->pfReset_TV_Mute			= P_STV6417_TV_Audio_Mute_Off ;
		s_pScartOp->pfSet_VCR_Mute			= P_STV6417_VCR_Mute_On ;
		s_pScartOp->pfReset_VCR_Mute		= P_STV6417_VCR_Mute_Off ;
		s_pScartOp->pfSet_FunctionPin_On		= P_STV6417_FunctionPin_On ;
		s_pScartOp->pfReset_FunctionPin_On	= P_STV6417_FunctionPin_Off ;
		//s_pScartOp->pfSet_RGB_Mode		= STV6412_Set_RGB_Mode ;
		//s_pScartOp->pfSet_CVBS_Mode		= STV6412_Set_CVBS_Mode ;
		s_pScartOp->pfSet_TV_RGB_Mode		= P_STV6417_TV_RGB_Mode_On;
		s_pScartOp->pfSet_TV_CVBS_Mode		= P_STV6417_TV_CVBS_Mode_On;
		s_pScartOp->pfSet_VCR_CVBS_Mode 	= P_STV6417_VCR_CVBS_Mode_On;
		s_pScartOp->pfSet_VCR_SVHS_Mode 	= P_STV6417_VCR_SVHS_Mode_On;
		s_pScartOp->pfSet_HW_Bypass			= P_STV6417_HW_Bypass;
		s_pScartOp->pfSet_Sound_Level		= P_STV6417_Sound_Level;
		s_pScartOp->pfSet_HW_Mute			= P_STV6417_HW_Mute_On;
		s_pScartOp->pfReset_HW_Mute			= P_STV6417_HW_Mute_Off;
		s_pScartOp->pfInterrupt_Enable        	= P_STV6417_Interrupt_Enable;
		s_pScartOp->pfInterrupt_Disable       	= P_STV6417_Interrupt_Disable;
		s_pScartOp->pfStatusRead 			= P_STV6417_Read;
		s_pScartOp->pfStandbyMode			= P_STV6417_StandbyMode;
	}
}


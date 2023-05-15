/*
 * $Header:   $
 */
/*********************************************************************
 * $Workfile:   drv_stled_mc2003.c  $
 * $Modtime:   April 4 2013 01:07:04  $
 *
 * Author:
 * Description:
 *
 *                                 Copyright (c) 2008 HUMAX Co., Ltd.
 *                                               All rights reserved.
 ********************************************************************/

/*---------------------------------------------------------------------------
**	Header Files
**-------------------------------------------------------------------------*/
#if defined(CONFIG_VK_STDLIB)
#else
#include 	<stdio.h>
#include 	<stdlib.h>
#include 	<string.h>
#endif

#include "vkernel.h"
#include "taskdef.h"
#include "drv_stled.h"
#include "drv_err.h"
#include "drv_key.h"
#include "drv_pm.h"

#include "nexus_spi.h"

/*---------------------------------------------------------------------------
**	Definitions
**-------------------------------------------------------------------------*/
//#define STLED_DEBUG
#if defined(CONFIG_DI_SHARED_LIB)
#ifdef STLED_DEBUG
#define PrintDebug(fmt, ...) 		VK_DBG_Print (fmt, ## __VA_ARGS__)
#define PrintError(fmt, ...)    	VK_DBG_Print (fmt, ## __VA_ARGS__)
#else
#define PrintDebug(...)
#define PrintError(...)				VK_DBG_Print (__VA_ARGS__)
#endif
#else
#ifdef STLED_DEBUG
#define PrintDebug(fmt, ...)		VK_DBG_Print (fmt , ## __VA_ARGS__)
#define PrintError(fmt, ...)		VK_DBG_Print (fmt , ## __VA_ARGS__)
#else
#define PrintDebug(fmt, ...)
#define PrintError(fmt, ...)		VK_DBG_Print (fmt , ## __VA_ARGS__)
#endif
#endif

/* Define Number of GRID and SEGMENT */
#define MC2003_GRID_NUM			NUMBER_OF_SEGMENTS
#define MAX_DATA_SIZE			MC2003_GRID_NUM*2
#define MAX_WRITE_BUFFER_SIZE	(MAX_DATA_SIZE + 1) // 1-byte cmd and 10-byte data

/* MC2003 Command Set */
#define MC2003_CMD1		0x00 //DISPLAY MODE SETTING Command
#define MC2003_CMD2		0x40 //DATA SETTING Command
#define MC2003_CMD3		0xC0 //ADDRESS SETTING Command
#define MC2003_CMD4		0x80 //DISPLAY CONTROL Command

/* Parameters for each Commands */
#define CMD1_PARAM_9SEG_4GRID		0x00
#define CMD1_PARAM_8SEG_5GRID		0x01

#define CMD2_PARAM_INCR_ADDR			0x00
#define CMD2_PARAM_FIXED_ADDR			0x04
#define CMD2_PARAM_WRITE_DATA_TO_DISP	0x00
#define CMD2_PARAM_IGNORE_WRITE_DATA	0x01
#define CMD2_PARAM_READ_KEY_SCAN_DATA	0x02
#define CMD2_PARAM_IGNORE_READ_KEY_DATA	0x03

#define CMD4_PARAM_PW_01_16	0x00
#define CMD4_PARAM_PW_02_16	0x01
#define CMD4_PARAM_PW_04_16	0x02
#define CMD4_PARAM_PW_10_16	0x03
#define CMD4_PARAM_PW_11_16	0x04
#define CMD4_PARAM_PW_12_16	0x05
#define CMD4_PARAM_PW_13_16	0x06
#define CMD4_PARAM_PW_14_16	0x07
#define CMD4_PARAM_DISP_OFF	0x00
#define CMD4_PARAM_DISP_ON	0x08

#if defined(CONFIG_STLED_MC2003_NEXUS_SPI)
#define MSB2LSB(x)	(((x)<<7)&0x80) | (((x)<<5)&0x40) | (((x)<<3)&0x20) | (((x)<<1)&0x10) | (((x)>>7)&0x01) | (((x)>>5)&0x02) | (((x)>>3)&0x04) | (((x)>>1)&0x08)
#elif defined(CONFIG_STLED_MC2003_NEXUS_GPIO)
#define MSB2LSB(x)	(x)
#endif

#if defined(CONFIG_STLED_MC2003_KEY_SCAN)
#define KEY_FRONT_B0K1		0x80
#define KEY_FRONT_B1K2		0x40
#define KEY_FRONT_B3K1		0x10
#define KEY_FRONT_B4K2		0x08

#define KEYSCAN_ZERO_MASK	0x02
#define KEYSCAN_VALID_MASK	0xD8

#define KEYSCAN_BYTE			4	//spec 상 최대 6byte 까지 가능. 1바이트는 cmd byte

#define KEYSCAN_POLLING_INTERVAL	250 /* ms */
typedef struct {
	unsigned int	frontkeyid;
} KEYSCAN_MSG_T;

#endif


/*---------------------------------------------------------------------------
**	Variables
**-------------------------------------------------------------------------*/
static NEXUS_SpiHandle 		SpiHandle;
static unsigned char		bInitialized;

static unsigned char	Char2SegmentMap[0x80] =
{
	0x00,	0x00,	0x00,	0x00,	0x00,	0x00,	0x00,	0x00,	// ASCII 0x00 - 0x07
	0x00,	0x00,	0x00,	0x00,	0x00,	0x00,	0x00,	0x00,	// ASCII 0x08 - 0x0F
	0x00,	0x00,	0x00,	0x00,	0x00,	0x00,	0x00,	0x00,	// ASCII 0x10 - 0x17
	0x00,	0x00,	0x00,	0x00,	0x00,	0x00,	0x00,	0x00,	// ASCII 0x18 - 0x1F
	0x00,	0x00,	0x00,	0xff,	0x00,	0x00,	0x00,	0x00, 	// ASCII 0x20 - 0x27
	0x00,	0x00,	0x00,	0x00,	0x00,	0x40,	0x80,	0x00,	// ASCII 0x28 - 0x2F
	0x3f,	0x06,	0x5b,	0x4f,	0x66,	0x6d,	0x7d,	0x27,	// ASCII 0x30 - 0x37
	0x7f,	0x6f,	0x00,	0x00,	0x00,	0x00,	0x00,	0x00,	// ASCII 0x38 - 0x3F
	0x00,	0x77,	0x7c,	0x39,	0x5e,	0x79,	0x71,	0x3d,	// ASCII 0x40 - 0x47
	0x76,	0x06,	0x0e,	0x74,	0x38,	0x49,	0x54,	0x3f,	// ASCII 0x48 - 0x4F
	0x73,	0x00,	0x50,	0x6d,	0x78,	0x3e,	0x3e,	0x49,	// ASCII 0x50 - 0x57
	0x76,	0x6e,	0x00,	0x00,	0x00,	0x00,	0x00,	0x00,	// ASCII 0x58 - 0x5F
	0x00,	0x77,	0x7c,	0x39,	0x5e,	0x79,	0x71,	0x3d,	// ASCII 0x60 - 0x67
	0x76,	0x06,	0x0e,	0x74,	0x38,	0x49,	0x54,	0x3f,	// ASCII 0x68 - 0x6F
	0x73,	0x00,	0x50,	0x6d,	0x78,	0x3e,	0x3e,	0x49, 	// ASCII 0x70 - 0x77
	0x76,	0x66,	0x00,	0x00,	0x00,	0x00,	0x00,	0xff,	// ASCII 0x78 - 0x7F
};

static unsigned char 	SEG_DIM_VAL = CMD4_PARAM_PW_12_16;
static unsigned char 	SEG_DISP_VAL = CMD4_PARAM_DISP_ON;
static unsigned long	Sema;
#if defined(CONFIG_STLED_MC2003_KEY_SCAN)
static unsigned long	KeyScanEvtId;
static unsigned long	KeyScanTaskId;
static unsigned char	Poll;
static unsigned char	Intr;
#endif


/*---------------------------------------------------------------------------
**	Local Functions
**-------------------------------------------------------------------------*/
#if defined(CONFIG_STLED_MC2003_NEXUS_GPIO)
/********************************************************************
Function	: P_MC2003_Write
Description :
Input		: none
Output 	: none
Return 	: int
********************************************************************/
static int P_MC2003_Write(unsigned char ucData)
{
	unsigned char ucMask = 0x01;
	int i = 0;

	for(i = 0; i < 8; i++)
	{
		(void) DRV_GPIO_Write(FRONT_CLK, GPIO_LOW);
		if((ucData&ucMask) == 0)	/* data on */
		{
			(void) DRV_GPIO_Write(FRONT_DO, GPIO_LOW);
		}
		else						/* data off */
		{
			(void) DRV_GPIO_Write(FRONT_DO, GPIO_HI);
		}
		(void) DRV_GPIO_Write(FRONT_CLK, GPIO_HI);

		ucMask <<= 1;
	}

	return DRV_OK;
}
#endif
/********************************************************************
Function	: P_MC2003_WriteCmd
Description : Send 1-byte command data
Input		: none
Output 	: none
Return 	: int
********************************************************************/
static int	P_MC2003_WriteCmd(unsigned char wdata)
{
#if defined(CONFIG_STLED_MC2003_NEXUS_SPI)
	NEXUS_Error		err;

	if (!SpiHandle) {
		PrintDebug("[%s] spi handle is null. ignore this call\n", __FUNCTION__);
		return 0;
	}

	VK_SEM_Get(Sema);
	wdata = MSB2LSB(wdata);
#if defined(CONFIG_STLED_MC2003_NEXUS_SPI_USING_GPIO_FOR_STROBE)
	DRV_GPIO_Write (FRONT_STB, 0);
#endif
	err = NEXUS_Spi_Write(SpiHandle, (const unsigned char*)&wdata, (size_t)1);
	if (err != NEXUS_SUCCESS) {
		PrintError("[%s] Error! Can't send command data on SPI\n", __FUNCTION__);
		VK_SEM_Release(Sema);
		return -1;
	}
#if defined(CONFIG_STLED_MC2003_NEXUS_SPI_USING_GPIO_FOR_STROBE)
	DRV_GPIO_Write (FRONT_STB, 1);
#endif
	VK_SEM_Release(Sema);

#elif defined(CONFIG_STLED_MC2003_NEXUS_GPIO)
	int	err;

	VK_SEM_Get(Sema);

	(void) DRV_GPIO_Write(FRONT_STB, GPIO_LOW);
	err = P_MC2003_Write(wdata);
	if(err != DRV_OK) {
		VK_SEM_Release(Sema);
		PrintError("[%s] Error! Can't send command data on GPIO\n", __FUNCTION__);
		return DRV_ERR;
	}
	(void) DRV_GPIO_Write(FRONT_STB, GPIO_HI);

	VK_SEM_Release(Sema);
#endif

	return 0;
}

/********************************************************************
Function	: P_MC2003_WriteData
Description : send datas
Input		: none
Output 	: none
Return 	: int
********************************************************************/
static int	P_MC2003_WriteData(unsigned char *wdata, int wlen)
{
#if defined(CONFIG_STLED_MC2003_NEXUS_SPI)
	NEXUS_Error		err;
	unsigned char	cmd;
	unsigned char	senddata[MAX_WRITE_BUFFER_SIZE];
	int				i;

	if (!SpiHandle) {
		PrintDebug("[%s] spi handle is null. ignore this call\n", __FUNCTION__);
		return 0;
	}

	VK_SEM_Get(Sema);
	cmd = MSB2LSB(MC2003_CMD2 | CMD2_PARAM_INCR_ADDR | CMD2_PARAM_WRITE_DATA_TO_DISP);
#if defined(CONFIG_STLED_MC2003_NEXUS_SPI_USING_GPIO_FOR_STROBE)
	DRV_GPIO_Write (FRONT_STB, 0);
#endif
	err = NEXUS_Spi_Write(SpiHandle, (const unsigned char*)&cmd, (size_t)1);
	if (err != NEXUS_SUCCESS) {
		PrintError("[%s] Error! Can't send command data on SPI\n", __FUNCTION__);
		VK_SEM_Release(Sema);
		return -1;
	}
#if defined(CONFIG_STLED_MC2003_NEXUS_SPI_USING_GPIO_FOR_STROBE)
	DRV_GPIO_Write (FRONT_STB, 1);
#endif

	VK_MEM_Memset(senddata, 0, sizeof(senddata));
	senddata[0] = MSB2LSB(MC2003_CMD3 | 0x00);
	for(i=1; i<=wlen && i<MAX_WRITE_BUFFER_SIZE; i++) {
		senddata[i] = MSB2LSB(wdata[i-1]);
	}
#if defined(CONFIG_STLED_MC2003_NEXUS_SPI_USING_GPIO_FOR_STROBE)
	DRV_GPIO_Write (FRONT_STB, 0);
#endif
	err = NEXUS_Spi_Write(SpiHandle, (const unsigned char*)senddata, (size_t)sizeof(senddata));
	if (err != NEXUS_SUCCESS) {
		PrintError("[%s] Error! Can't send command data on SPI\n", __FUNCTION__);
		VK_SEM_Release(Sema);
		return -1;
	}
#if defined(CONFIG_STLED_MC2003_NEXUS_SPI_USING_GPIO_FOR_STROBE)
	DRV_GPIO_Write (FRONT_STB, 1);
#endif

	VK_SEM_Release(Sema);

#elif defined(CONFIG_STLED_MC2003_NEXUS_GPIO)
	unsigned char 	ucCmd=0x0;
	int				err;
	int				i = 0;
	unsigned char	aucSendData[MAX_WRITE_BUFFER_SIZE];

	VK_SEM_Get(Sema);

	(void) DRV_GPIO_Write(FRONT_STB, GPIO_LOW);
	ucCmd = (MC2003_CMD2 | CMD2_PARAM_INCR_ADDR | CMD2_PARAM_WRITE_DATA_TO_DISP);
	err = P_MC2003_Write(ucCmd);
	if(err != DRV_OK) {
		VK_SEM_Release(Sema);
		PrintError("[%s] Error! Can't send command data on GPIO\n", __FUNCTION__);
		return DRV_ERR;
	}
	(void) DRV_GPIO_Write(FRONT_STB, GPIO_HI);

	VK_MEM_Memset(aucSendData, 0, sizeof(aucSendData));
	aucSendData[0]=(MC2003_CMD3 | 0x00);
	for(i=1; i<=wlen && i<MAX_WRITE_BUFFER_SIZE; i++) {
		aucSendData[i] = wdata[i-1];
	}

	(void) DRV_GPIO_Write(FRONT_STB, GPIO_LOW);
	for(i = 0; i <= wlen; i++)
	{
		err = P_MC2003_Write( aucSendData[i] );
		if(err != DRV_OK) {
		VK_SEM_Release(Sema);
		PrintError("[%s] Error! Can't send command data on GPIO\n", __FUNCTION__);
		return DRV_ERR;
		}
	}
	(void) DRV_GPIO_Write(FRONT_STB, GPIO_HI);

	VK_SEM_Release(Sema);
#endif

	return 0;
}

#if defined(CONFIG_STLED_MC2003_KEY_SCAN)
/********************************************************************
Function	: P_MC2003_ReadData
Description : Read datas from mc2003 device. usually it is key datas.
Input		: none
Output 	: none
Return 	: int
********************************************************************/
static int	P_MC2003_ReadData(unsigned char *wdata, unsigned char *rdata, int rlen)
{
#if defined(CONFIG_STLED_MC2003_NEXUS_SPI)
	NEXUS_Error		err;

	if (!SpiHandle) {
		PrintDebug("[%s] spi handle is null. ignore this call\n", __FUNCTION__);
		return 0;
	}

	VK_SEM_Get(Sema);
#if defined(CONFIG_STLED_MC2003_NEXUS_SPI_USING_GPIO_FOR_STROBE)
	DRV_GPIO_Write (FRONT_STB, 0);
#endif
	err = NEXUS_Spi_Read(SpiHandle, (const unsigned char*)wdata, (unsigned char*)rdata, (size_t)rlen);
	if (err != NEXUS_SUCCESS) {
		PrintError("[%s] Error! Can't read data on SPI\n", __FUNCTION__);
		VK_SEM_Release(Sema);
		return -1;
	}
#if defined(CONFIG_STLED_MC2003_NEXUS_SPI_USING_GPIO_FOR_STROBE)
	DRV_GPIO_Write (FRONT_STB, 1);
#endif

	VK_SEM_Release(Sema);

#elif defined(CONFIG_STLED_MC2003_NEXUS_GPIO)
	int				err;
	int				i=0, k=0;
	GPIO_STATE_t		eGpioState=NEXUS_GpioValue_eLow;

	VK_SEM_Get(Sema);

	(void) DRV_GPIO_Write(FRONT_STB, GPIO_LOW);
	err = P_MC2003_Write( wdata[0] );
	if(err != DRV_OK) {
		VK_SEM_Release(Sema);
		PrintError("[%s] Error! Can't send command data on GPIO\n", __FUNCTION__);
		return DRV_ERR;
	}

	/* set gpio direction as INPUT */
	(void) DRV_GPIO_ChangeMode(FRONT_DO, GPIO_MODE_READ, GPIO_HI);

	rdata[0] = wdata[0];

	for(k=1; k<rlen; k++)
	{
		rdata[k] = 0;

		for(i=7; i>=0; i--)
		{
			(void) DRV_GPIO_Write(FRONT_CLK, GPIO_LOW);
			(void) DRV_GPIO_Read(FRONT_DO, &eGpioState);
			rdata[k] |= (unsigned char)(eGpioState<<i);
			(void) DRV_GPIO_Write(FRONT_CLK, GPIO_HI);
		}
	}

	/* set gpio direction as OUTPUT */
	(void) DRV_GPIO_ChangeMode(FRONT_DO, GPIO_MODE_WRITE, GPIO_HI);
	(void) DRV_GPIO_Write(FRONT_STB, GPIO_HI);

	VK_SEM_Release(Sema);
#endif

	return 0;
}

#if defined(CONFIG_STLED_MC2003_KEY_SCAN_INTERRUPT)
/********************************************************************
Function	: P_MC2003_KeyISR
Description : Key ISR
Input		: none
Output 	: none
Return 	: int
********************************************************************/
static void P_MC2003_KeyISR(void *context, int param)
{
	(void)context;
	(void)param;

	if (Intr) {
		PrintDebug("[%s] ignore key isr\n", __FUNCTION__);
		return;
	}
	Intr = 1;

	PrintDebug("[%s] key isr called\n", __FUNCTION__);

	/* disable isr */
	(void)DRV_GPIO_INTR_Disable(GPIO_ID_IRQ_FRONT);
	(void)DRV_GPIO_INTR_Disable(GPIO_ID_IRQ_FRONT2);

	/* clear interrupt before sending event to task */
	(void)DRV_GPIO_INTR_Clear(GPIO_ID_IRQ_FRONT);
	(void)DRV_GPIO_INTR_Clear(GPIO_ID_IRQ_FRONT2);

	if (VK_EVENT_Send(KeyScanEvtId, 0) == VK_OK) {
		return;
	}

	Intr = 0;
	/* enable isr */
	(void)DRV_GPIO_INTR_Enable(GPIO_ID_IRQ_FRONT);
	(void)DRV_GPIO_INTR_Enable(GPIO_ID_IRQ_FRONT2);

	return;
}

static void P_MC2003_KeyISR2(void *context, int param)
{
	P_MC2003_KeyISR(context, param);
	return;
}

#endif

/********************************************************************
Function	: P_MC2003_KeyScanTask
Description : Scan which key is pressed and send message to key driver
Input		: none
Output 	: none
Return 	: void
********************************************************************/
static void P_MC2003_KeyScanTask(void *param)
{
	unsigned char	wdata[KEYSCAN_BYTE];
	unsigned char	rdata[KEYSCAN_BYTE];
	int				err;
	unsigned long	frontKey = 0;
	unsigned long	keyBackup = 0;
	unsigned long	keyRepeat = 0;
	unsigned long	evt;
	int			i, keybit = 0;

	(void)param;

#if defined(CONFIG_STLED_MC2003_KEY_SCAN_POLLING)
	/* In polling mode, we set Polling variable to TRUE forcibly.
	    If we use interrupt mode not polling, set this variable when interrupt occured */
	Poll = TRUE;
#endif

	VK_MEM_Memset(wdata, 0xff, sizeof(wdata));

	while(1) {
		if (!Poll) {
			if (VK_EVENT_Receive(KeyScanEvtId, &evt) != VK_OK)
				continue;

			Poll = 1;

			/* wait for key-data becomes stable */
			VK_TASK_Sleep(20);
		}

		if(Poll) {
			/* When we read data from mc2003, wdata need read-command in buffer[0] and 0xff in the other buffers.
			    wdata buffer size must be same with rdata buffer size */
			wdata[0] = MSB2LSB(MC2003_CMD2 | CMD2_PARAM_READ_KEY_SCAN_DATA);
			err = P_MC2003_ReadData(wdata, rdata, sizeof(wdata));

			if (err < 0) {
				PrintError("[%s] Error! Can't read data from mc2003\n", __FUNCTION__);
				goto keyscan_end;
			}
			else {
				/* If success, rdata[0] has command data that sended */
				if (rdata[0] != wdata[0]) {
					PrintError("[%s] Req/Res Command Mismatch!\n", __FUNCTION__);
					goto keyscan_end;
				}

				PrintDebug("[%s] rdata[1]=0x%x, rdata[2]=0x%x\n", __FUNCTION__, rdata[1], rdata[2]);

				frontKey = 0;
				if ((rdata[1] & KEYSCAN_VALID_MASK) && ((rdata[1] & KEYSCAN_ZERO_MASK ) == 0)) {// KS1/KS2
					/* key pressed */
					if (rdata[1] & KEY_FRONT_B0K1) {
						frontKey |= (1<<7); /* KS1 KEY_COL1 <<s_BoardFrontKeyMapping KeyID 7 */
					}
					if (rdata[1] & KEY_FRONT_B1K2) {
						frontKey |= (1<<5); /* KS1 KEY_COL2 <<s_BoardFrontKeyMapping KeyID 5 */
					}
					if (rdata[1] & KEY_FRONT_B3K1) {
						frontKey |= (1<<3); /* KS2 KEY_COL1 <<s_BoardFrontKeyMapping KeyID 3 */
					}
					if (rdata[1] & KEY_FRONT_B4K2) {
						frontKey |= (1<<1); /* KS2 KEY_COL2 <<s_BoardFrontKeyMapping KeyID 1 */
					}
				}
				if ((rdata[2] & KEYSCAN_VALID_MASK) && ((rdata[2] & KEYSCAN_ZERO_MASK ) == 0)) {// KS3/KS4
					/* key pressed */
					if (rdata[2] & KEY_FRONT_B0K1) {
						frontKey |= (1<<2); /* KS3 KEY_COL1 <<s_BoardFrontKeyMapping KeyID 2 */
					}
					if (rdata[2] & KEY_FRONT_B1K2) {
						frontKey |= (1<<8); /* KS3 KEY_COL2 <<s_BoardFrontKeyMapping KeyID 8 */
					}
					if (rdata[2] & KEY_FRONT_B3K1) {
						frontKey |= (1<<6); /* KS4 KEY_COL1 <<s_BoardFrontKeyMapping KeyID 6 */
					}
					if (rdata[2] & KEY_FRONT_B4K2) {
						frontKey |= (1<<4); /* KS4 KEY_COL2 <<s_BoardFrontKeyMapping KeyID 4 */
					}
				}
				if ((rdata[3] & KEYSCAN_VALID_MASK) && ((rdata[3] & KEYSCAN_ZERO_MASK ) == 0)) {// KS3/KS4
					/* key pressed */
					if (rdata[3] & KEY_FRONT_B0K1) {
						frontKey |= (1<<9); /* KS5 KEY_COL1 <<s_BoardFrontKeyMapping KeyID 9 */
					}
					if (rdata[3] & KEY_FRONT_B1K2) {
						frontKey |= (1<<10); /* KS5 KEY_COL2 <<s_BoardFrontKeyMapping KeyID 10 */
					}
					if (rdata[3] & KEY_FRONT_B3K1) {
						frontKey |= (1<<11); /* KS6 KEY_COL1 <<s_BoardFrontKeyMapping KeyID 11 */
					}
					if (rdata[3] & KEY_FRONT_B4K2) {
						frontKey |= (1<<12);  /* KS6 KEY_COL2 <<s_BoardFrontKeyMapping KeyID 12 */
					}
				}
				if (frontKey != 0) {
					keyRepeat = keyBackup & frontKey;
					keyBackup = frontKey;

					if (keyRepeat) {
						PrintDebug("[%s] Key Repeated(0x%02x)\n", __FUNCTION__, keyRepeat);
						(void)DRV_KEY_SendMultiKeyMessage(keyRepeat, 1);
						frontKey = frontKey & ~keyRepeat;
					}
					if (frontKey) {
						PrintDebug("[%s] Key Pressed(0x%02x)\n", __FUNCTION__, frontKey);
						(void)DRV_KEY_SendMultiKeyMessage(frontKey, 0);
					}
				}
				else {
					/* no key pressed */
					keyBackup = 0;
#if defined(CONFIG_STLED_MC2003_KEY_SCAN_INTERRUPT)
					Poll = 0;
					Intr = 0;

					/* Clear interrupt befor enabling isr */
					(void)DRV_GPIO_INTR_Clear(GPIO_ID_IRQ_FRONT);
					(void)DRV_GPIO_INTR_Clear(GPIO_ID_IRQ_FRONT2);

					/* Enable interrupt on gpio */
					(void)DRV_GPIO_INTR_Enable(GPIO_ID_IRQ_FRONT);
					(void)DRV_GPIO_INTR_Enable(GPIO_ID_IRQ_FRONT2);

					continue;
#endif
				}
			}
		}

keyscan_end:
		VK_TASK_Sleep(KEYSCAN_POLLING_INTERVAL);
	}
}
#endif

/*---------------------------------------------------------------------------
**	Global Functions
**-------------------------------------------------------------------------*/
#if defined(CONFIG_BRCM_MOD_NEXUS_POWER_MANAGEMENT)
/********************************************************************
Function	: DRV_STLED_Resume
Description :
Input		:
Output	: none
Return	: DRV_OK
*********************************************************************/
void DRV_STLED_Resume(void)
{
	unsigned char		wdata[MAX_DATA_SIZE];
	unsigned char		cmd;
	int					err;
#if defined(CONFIG_STLED_MC2003_NEXUS_SPI)
	NEXUS_SpiSettings	SpiSettings;
#endif

	PrintDebug("[%s] Resuming...\n", __FUNCTION__);

#if defined(CONFIG_STLED_MC2003_NEXUS_SPI)
	if (SpiHandle)
		return;

	NEXUS_Spi_GetDefaultSettings(&SpiSettings);

    /* These settings need to be vendor specific. Please populate spiSettings accordingly */
    SpiSettings.clockActiveLow = false;
	SpiSettings.bitsPerTransfer = 8;
    SpiSettings.baud = 333000;      /* 333KHz */
    SpiSettings.interruptMode = false;
    SpiSettings.lastByteContinueEnable = false;
    SpiSettings.txLeadingCapFalling = false;
    SpiSettings.useUserDtlAndDsclk = false;
    //SpiSettings.dtl = 100;

	SpiHandle = NEXUS_Spi_Open(0, &SpiSettings);
	if (!SpiHandle)
	{
		PrintError("[%s] Error! can't resume\n", __FUNCTION__);
		return;
	}
#endif

	/* Initialize MC2003 device */
	cmd = (MC2003_CMD1 | CMD1_PARAM_8SEG_5GRID);
	err = P_MC2003_WriteCmd(cmd);
	if (err < 0)
	{
		PrintError("[%s] Error! fail to initialize MC2003(CMD1/2)\n", __FUNCTION__);
		return;
	}

	cmd = (MC2003_CMD2 | CMD2_PARAM_INCR_ADDR | CMD2_PARAM_WRITE_DATA_TO_DISP);
	err = P_MC2003_WriteCmd(cmd);
	if (err < 0)
	{
		PrintError("[%s] Error! fail to initialize MC2003(CMD1/2)\n", __FUNCTION__);
		return;
	}

	/* Initialize MC2003 Memory buffer */
#if defined(CONFIG_PRODUCT_IMAGE_DEBUG) || defined(CONFIG_PRODUCT_IMAGE_HWTEST)
	VK_MEM_Memset(wdata, 0xff, sizeof(wdata));
	err = P_MC2003_WriteData(wdata, sizeof(wdata));
	if (err < 0)
	{
		PrintError("[%s] Error! fail to initialize MC2003(CMD3)\n", __FUNCTION__);
		return;
	}
#else
	VK_MEM_Memset(wdata, 0x00, sizeof(wdata));
	err = P_MC2003_WriteData(wdata, sizeof(wdata));
	if (err < 0)
	{
		PrintError("[%s] Error! fail to initialize MC2003(CMD3)\n", __FUNCTION__);
		return;
	}
#endif

	/* Turn on segments and set dimming value*/
	cmd = (MC2003_CMD4 | SEG_DISP_VAL | SEG_DIM_VAL);
	err = P_MC2003_WriteCmd(cmd);
	if (err < 0)
	{
		PrintError("[%s] Error! fail to initialize MC2003(CMD4)\n", __FUNCTION__);
		return;
	}
}

/********************************************************************
Function	: DRV_STLED_Suspend
Description :
Input		:
Output	: none
Return	: DRV_OK
*********************************************************************/
void DRV_STLED_Suspend(void)
{
	PrintDebug("[%s] Suspending...\n", __FUNCTION__);

#if defined(CONFIG_STLED_MC2003_NEXUS_SPI)
	if (!SpiHandle)
		return;

	NEXUS_Spi_Close(SpiHandle);
	SpiHandle = 0;
#endif
#if defined(CONFIG_STLED_MC2003_NEXUS_SPI_USING_GPIO_FOR_STROBE)
	DRV_GPIO_Write (FRONT_STB, 0);
#endif
}

#endif


/********************************************************************
Function	: DRV_STLED_WriteString
Description :
Input		:
Output	: none
Return	: DRV_OK
*********************************************************************/
DRV_Error DRV_STLED_WriteString(unsigned char *string, unsigned char len, int isTime)
{
	unsigned char 	wdata[MAX_DATA_SIZE];
	int				i;
	int				err;
	unsigned char	sdata;

	if (!string) {
		return DRV_ERR;
	}

	/* We support 4-grid 7-segment */
	if (len > MC2003_GRID_NUM) {
		len = MC2003_GRID_NUM;
	}

	VK_MEM_Memset(wdata, 0x00, sizeof(wdata));
	for(i=0; i<len; i++) {
		sdata = Char2SegmentMap[(string[i]&0x7F)];
		if (isTime) {
			sdata |= 0x80;
		}
		wdata[i*2] = sdata; /*data*/
	}
	err = P_MC2003_WriteData(wdata, sizeof(wdata));
	if (err < 0) {
		PrintError("[%s] Error! Fail to write string data\n", __FUNCTION__);
		return DRV_ERR;
	}

	return DRV_OK;
}

/********************************************************************
Function	: DRV_STLED_Brightness
Description :
Input		: brightness value (1~4) integer
Output	: none
Return	: DRV_OK
*********************************************************************/
DRV_Error DRV_STLED_Brightness(int value)
{
	unsigned char 	cmd;
	unsigned char	sdata;
	int				err;

	switch(value) {
	case 1:
		sdata = CMD4_PARAM_PW_01_16;
		break;
	case 2:
		sdata = CMD4_PARAM_PW_04_16;
		break;
	case 3:
		sdata = CMD4_PARAM_PW_12_16;
		break;
	case 4:
		sdata = CMD4_PARAM_PW_14_16;
		break;
	default:
		PrintError("[%s] Error! Not supported(invaild value)\n", __FUNCTION__);
		return DRV_ERR;
	}

	cmd = (MC2003_CMD4 | SEG_DISP_VAL | sdata);
	err = P_MC2003_WriteCmd(cmd);
	if (err < 0) {
		PrintError("[%s] Error! Fail to write brightness value\n", __FUNCTION__);
		return DRV_ERR;
	}

	SEG_DIM_VAL = sdata;

	return DRV_OK;
}

/********************************************************************
Function	: DRV_STLED_DisplayLED
Description :
Input		: const char *
Output	: none
Return	: DRV_OK
*********************************************************************/
DRV_Error DRV_STLED_DisplayLED (unsigned char *ucData)
{
	//do implement
	(void)ucData;

	return DRV_OK;
}

/********************************************************************
Function	: DRV_STLED_7SegmentOnOff
Description :
Input		: ON?OFF
Output	: none
Return	: DRV_OK
*********************************************************************/
DRV_Error DRV_STLED_7SegmentOnOff (bool OnOff)
{
	unsigned char 	cmd;
	int				err;
	unsigned char	param_on = CMD4_PARAM_DISP_OFF;

	if (OnOff) {
		param_on = CMD4_PARAM_DISP_ON;
	}

	cmd = (MC2003_CMD4 | param_on | SEG_DIM_VAL);
	err = P_MC2003_WriteCmd(cmd);
	if (err < 0) {
		PrintError("[%s] Error! Fail to write display on/off data\n", __FUNCTION__);
		return DRV_ERR;
	}

	SEG_DISP_VAL = param_on;
	return DRV_OK;
}

/********************************************************************
Function	: DRV_STLED_7SegmentTest
Description :
Input		: ON/OFF
Output	: none
Return	: DRV_OK
*********************************************************************/
DRV_Error DRV_STLED_7SegmentTest(int Onoff)
{
	unsigned char 	wdata[MAX_DATA_SIZE];
	int				i, j;

	if (Onoff) {
		// draw 8 turn by turn
		VK_MEM_Memset(wdata, 0x00, sizeof(wdata));
		for(i=0; i<MC2003_GRID_NUM; i++) {
			for(j=0; j<7; j++) {
				wdata[i*2] |= (1 << j);
				(void)P_MC2003_WriteData(wdata, sizeof(wdata));
				VK_TASK_Sleep(100);
			}
		}

		//clock
		wdata[2] |= (0x80);
		(void)P_MC2003_WriteData(wdata, sizeof(wdata));
		VK_TASK_Sleep(100);

		wdata[6] |= (0x80);
		(void)P_MC2003_WriteData(wdata, sizeof(wdata));
		VK_TASK_Sleep(100);

		// turn on led, turn by turn
		// not supported
	}
	else {
		VK_MEM_Memset(wdata, 0x00, sizeof(wdata));
		(void)P_MC2003_WriteData(wdata, sizeof(wdata));
	}

	return DRV_OK;
}

/********************************************************************
Function	: DRV_STLED_Init
Description :
Input		: none
Output	: none
Return	: DRV_OK
*********************************************************************/
DRV_Error DRV_STLED_Init(void)
{
#if defined(CONFIG_STLED_MC2003_NEXUS_SPI)
	NEXUS_SpiSettings	SpiSettings;
#endif
	unsigned char		wdata[MAX_DATA_SIZE];
	unsigned char		cmd;
	int					err;

	if (bInitialized) {
		PrintError("[%s] STLED already initialized!\n", __FUNCTION__);
		return DRV_OK;
	}

	if (VK_SEM_Create(&Sema,"mc2003-sema", DEFAULT_SUSPEND_TYPE) != VK_OK) {
		return DRV_ERR;
	}

#if defined(CONFIG_STLED_MC2003_NEXUS_SPI)
	NEXUS_Spi_GetDefaultSettings(&SpiSettings);

    /* These settings need to be vendor specific. Please populate spiSettings accordingly */
    SpiSettings.clockActiveLow = false;
	SpiSettings.bitsPerTransfer = 8;
    SpiSettings.baud = 333000;      /* 333KHz */
    SpiSettings.interruptMode = false;
    SpiSettings.lastByteContinueEnable = false;
    SpiSettings.txLeadingCapFalling = false;
    SpiSettings.useUserDtlAndDsclk = false;
    //SpiSettings.dtl = 100;

	SpiHandle = NEXUS_Spi_Open(0, &SpiSettings);
	if (!SpiHandle) {
		PrintError("[%s] Error! can't open nexus spi\n", __FUNCTION__);
		return DRV_ERR;
	}
#elif defined(CONFIG_STLED_MC2003_NEXUS_GPIO)
	/* set gpio direction as OUTPUT */
	(void) DRV_GPIO_ChangeMode(FRONT_STB, GPIO_MODE_WRITE, GPIO_HI);
	(void) DRV_GPIO_ChangeMode(FRONT_CLK, GPIO_MODE_WRITE, GPIO_HI);
	(void) DRV_GPIO_ChangeMode(FRONT_DO, GPIO_MODE_WRITE, GPIO_HI);

	/* set initial value */
	(void) DRV_GPIO_Write(FRONT_STB, GPIO_HI);
	(void) DRV_GPIO_Write(FRONT_CLK, GPIO_HI);
	(void) DRV_GPIO_Write(FRONT_DO, GPIO_HI);
#endif

	/* Initialize MC2003 device */
	cmd = (MC2003_CMD1 | CMD1_PARAM_8SEG_5GRID);
	err = P_MC2003_WriteCmd(cmd);
	if (err < 0) {
		PrintError("[%s] Error! fail to initialize MC2003(CMD1/2)\n", __FUNCTION__);
		return DRV_ERR;
	}

	cmd = (MC2003_CMD2 | CMD2_PARAM_INCR_ADDR | CMD2_PARAM_WRITE_DATA_TO_DISP);
	err = P_MC2003_WriteCmd(cmd);
	if (err < 0) {
		PrintError("[%s] Error! fail to initialize MC2003(CMD1/2)\n", __FUNCTION__);
		return DRV_ERR;
	}

	/* Initialize MC2003 Memory buffer */
#if defined(CONFIG_PRODUCT_IMAGE_DEBUG) || defined(CONFIG_PRODUCT_IMAGE_HWTEST)
	VK_MEM_Memset(wdata, 0xff, sizeof(wdata));
	err = P_MC2003_WriteData(wdata, sizeof(wdata));
	if (err < 0) {
		PrintError("[%s] Error! fail to initialize MC2003(CMD3)\n", __FUNCTION__);
		return DRV_ERR;
	}
#else
	VK_MEM_Memset(wdata, 0x00, sizeof(wdata));
	err = P_MC2003_WriteData(wdata, sizeof(wdata));
	if (err < 0) {
		PrintError("[%s] Error! fail to initialize MC2003(CMD3)\n", __FUNCTION__);
		return DRV_ERR;
	}
#endif

	/* Turn on segments and set dimming value*/
	cmd = (MC2003_CMD4 | SEG_DISP_VAL | SEG_DIM_VAL);
	err = P_MC2003_WriteCmd(cmd);
	if (err < 0) {
		PrintError("[%s] Error! fail to initialize MC2003(CMD4)\n", __FUNCTION__);
		return DRV_ERR;
	}

#if defined(CONFIG_STLED_MC2003_KEY_SCAN)
	if (VK_EVENT_Create(&KeyScanEvtId, "keyscan_evt") != VK_OK) {
		return DRV_ERR;
	}

	/* If we use key which connected with mc2003 device, create task for scanning key data */
	if (VK_TASK_Create(P_MC2003_KeyScanTask, USB_TASK_PRIORITY, USB_TASK_STACK_SIZE, "Mc2003KeyScanTask", NULL, &KeyScanTaskId, FALSE) != VK_OK) {
		return DRV_ERR;
	}

	VK_TASK_Start(KeyScanTaskId);

#if defined(CONFIG_STLED_MC2003_KEY_SCAN_INTERRUPT)
	if (DRV_GPIO_INTR_RegisterEventNotify(GPIO_ID_IRQ_FRONT, P_MC2003_KeyISR, NULL, GPIO_ID_IRQ_FRONT) != DRV_OK) {
		PrintError("[%s] Error! Can't init interrupt on GPIO\n", __FUNCTION__);
		return DRV_ERR;
	}
	if (DRV_GPIO_INTR_Enable(GPIO_ID_IRQ_FRONT) != DRV_OK) {
		PrintError("[%s] Error! Can't enable interrupt on GPIO\n", __FUNCTION__);
		return DRV_ERR;
	}
	if (DRV_GPIO_INTR_RegisterEventNotify(GPIO_ID_IRQ_FRONT2, P_MC2003_KeyISR2, NULL, GPIO_ID_IRQ_FRONT2) != DRV_OK) {
		PrintError("[%s] Error! Can't init interrupt2 on GPIO\n", __FUNCTION__);
		return DRV_ERR;
	}
	if (DRV_GPIO_INTR_Enable(GPIO_ID_IRQ_FRONT2) != DRV_OK) {
		PrintError("[%s] Error! Can't enable interrupt2 on GPIO\n", __FUNCTION__);
		return DRV_ERR;
	}
#endif
#endif

#if defined(CONFIG_BRCM_MOD_NEXUS_POWER_MANAGEMENT)
	DRV_PM_RegisterCallback(DRV_STLED_Suspend, DRV_STLED_Resume);
#endif

	bInitialized = TRUE;
	PrintError("[%s] STLED Init Success!\n", __FUNCTION__);

	return DRV_OK;
}

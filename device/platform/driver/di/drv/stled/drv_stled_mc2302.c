/*
 * $Header:   $
 */
/*********************************************************************
 * $Workfile:   drv_stled_mc2302.c  $
 * $Modtime:   August 21 2015 15:51:13  $
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
#define PrintDebug(fmt, ...) 	VK_DBG_Print (fmt, ## __VA_ARGS__)
#define PrintError(fmt, ...)    	VK_DBG_Print (fmt, ## __VA_ARGS__)
#else
#define PrintDebug(...)
#define PrintError(...)			VK_DBG_Print (__VA_ARGS__)
#endif
#else
#ifdef STLED_DEBUG
#define PrintDebug(fmt, ...)	VK_DBG_Print (fmt , ## __VA_ARGS__)
#define PrintError(fmt, ...)		VK_DBG_Print (fmt , ## __VA_ARGS__)
#else
#define PrintDebug(fmt, ...)
#define PrintError(fmt, ...)		VK_DBG_Print (fmt , ## __VA_ARGS__)
#endif
#endif

/* Define Number of GRID and SEGMENT */
#define MC2302_GRID_NUM		NUMBER_OF_SEGMENTS
#define MAX_DATA_SIZE			(MC2302_GRID_NUM+1)*2	/* add 1 grid (GR5) for LED and DP */
#define MAX_WRITE_BUFFER_SIZE	(MAX_DATA_SIZE + 1)		/* add 1-byte for command */

/* MC2302 Command Set */
#define MC2302_CMD1		0x00	/* DISPLAY MODE SETTING Command */
#define MC2302_CMD2		0x40	/* DATA SETTING Command */
#define MC2302_CMD3		0xC0	/* ADDRESS SETTING Command */
#define MC2302_CMD4		0x80	/* DISPLAY CONTROL Command */

/* Parameters for each Commands */
#define CMD1_PARAM_14SEG_4GRID		0x00
#define CMD1_PARAM_13SEG_5GRID		0x01
#define CMD1_PARAM_12SEG_6GRID		0x10
#define CMD1_PARAM_11SEG_7GRID		0x11

#define CMD2_PARAM_INCR_ADDR				0x00
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

/* LED grid address */
#define MC2302_LED_GRID_FIXED_ADDRESS	0x08

#if defined(CONFIG_STLED_MC2302_NEXUS_SPI)
#define MSB2LSB(x)	(((x)<<7)&0x80) | (((x)<<5)&0x40) | (((x)<<3)&0x20) | (((x)<<1)&0x10) | (((x)>>7)&0x01) | (((x)>>5)&0x02) | (((x)>>3)&0x04) | (((x)>>1)&0x08)
#elif defined(CONFIG_STLED_MC2302_NEXUS_GPIO)
#define MSB2LSB(x)	(x)
#endif

#if defined(CONFIG_STLED_MC2302_KEY_SCAN)
#define KEY_FRONT_B0K1			0x80
#define KEY_FRONT_B1K2			0x40
#define KEY_FRONT_B2K3			0x20
#define KEY_FRONT_B3K1			0x10
#define KEY_FRONT_B4K2			0x08
#define KEY_FRONT_B5K3			0x04

#define KEYSCAN_ZERO_MASK		0x02
#define KEYSCAN_VALID_MASK	0xD8

#define KEYSCAN_BYTE			4	/* spec 상 최대 6byte 까지 가능. 1바이트는 cmd byte */

#define KEYSCAN_POLLING_INTERVAL	250 /* ms */
typedef struct {
	unsigned int	frontkeyid;
} KEYSCAN_MSG_T;
#endif


/*---------------------------------------------------------------------------
**	Variables
**-------------------------------------------------------------------------*/
#if defined(CONFIG_STLED_MC2302_NEXUS_SPI)
static NEXUS_SpiHandle	SpiHandle;
#endif
static unsigned char	bInitialized;

static unsigned char	Char2SegmentMap[128][2] =
{
	{0x00, 0x00}, {0x00, 0x00}, {0x00, 0x00}, {0x00, 0x00}, {0x00, 0x00},	/* ASCII 0 - 4 */
	{0x00, 0x00}, {0x00, 0x00}, {0x00, 0x00}, {0x00, 0x00}, {0x00, 0x00},	/* ASCII 5 - 9 */
	{0x00, 0x00}, {0x00, 0x00}, {0x00, 0x00}, {0x00, 0x00}, {0x00, 0x00},	/* ASCII 10 - 14 */
	{0x00, 0x00}, {0x00, 0x00}, {0x00, 0x00}, {0x00, 0x00}, {0x00, 0x00},	/* ASCII 15 - 19 */
	{0x00, 0x00}, {0x00, 0x00}, {0x00, 0x00}, {0x00, 0x00}, {0x00, 0x00},	/* ASCII 20 - 24 */
	{0x00, 0x00}, {0x00, 0x00}, {0x00, 0x00}, {0x00, 0x00}, {0x00, 0x00},	/* ASCII 25 - 29 */
	{0x00, 0x00}, {0x00, 0x00}, {0x00, 0x00}, {0x00, 0x00}, {0x00, 0x00},	/* ASCII 30 - 34 */
	{0x00, 0x00}, {0x00, 0x00}, {0x00, 0x00}, {0x00, 0x00}, {0x00, 0x00},	/* ASCII 35 - 39 */
	{0x00, 0x00}, {0x00, 0x00}, {0x00, 0x00}, {0x00, 0x00}, {0x00, 0x00},	/* ASCII 40 - 44 */
	{0x00, 0x00}, {0x00, 0x00}, {0x00, 0x00}, {0x3f, 0x00}, {0x06, 0x00},	/* ASCII 45 - 49 (0 - 1) */
	{0x5b, 0x00}, {0x4f, 0x00}, {0x66, 0x00}, {0x6d, 0x00}, {0x7d, 0x00},	/* ASCII 50 - 54 (2 - 6) */
	{0x27, 0x00}, {0x7f, 0x00}, {0x6f, 0x00}, {0x00, 0x00}, {0x00, 0x00},	/* ASCII 55 - 59 (7 - 9) */
	{0x00, 0x00}, {0x00, 0x00}, {0x00, 0x00}, {0x00, 0x00}, {0x00, 0x00},	/* ASCII 60 - 64 */
	{0x77, 0x00}, {0x7c, 0x00}, {0x39, 0x00}, {0x5e, 0x00}, {0x79, 0x00},	/* ASCII 65 - 69 (A - E) */
	{0x71, 0x00}, {0x3d, 0x00}, {0x76, 0x00}, {0x04, 0x00}, {0x1e, 0x00},	/* ASCII 70 - 74 (F - J) */
	{0x70, 0x05}, {0x38, 0x00}, {0xb6, 0x01}, {0xb6, 0x04}, {0x5c, 0x00},	/* ASCII 75 - 79 (K - O) */
	{0x73, 0x00}, {0x3f, 0x04}, {0x73, 0x04}, {0x6d, 0x00}, {0x78, 0x00},	/* ASCII 80 - 84 (P - T) */
	{0x3e, 0x00}, {0x30, 0x03}, {0x36, 0x06}, {0x80, 0x07}, {0x6e, 0x00},	/* ASCII 85 - 89 (U - Y) */
	{0x09, 0x03}, {0x00, 0x00}, {0x00, 0x00}, {0x00, 0x00}, {0x00, 0x00},	/* ASCII 90 - 94 (Z) */
	{0x00, 0x00}, {0x00, 0x00}, {0x5c, 0x04}, {0x7c, 0x00}, {0x58, 0x00},	/* ASCII 95 - 99 (a - c) */
	{0x5e, 0x00}, {0x79, 0x00}, {0x71, 0x00}, {0x6f, 0x02}, {0x74, 0x00},	/* ASCII 100 - 104 (d - h) */
	{0x04, 0x00}, {0x0e, 0x00}, {0x70, 0x05}, {0x38, 0x00}, {0xb6, 0x01},	/* ASCII 105 - 109 (i - m) */
	{0xb6, 0x04}, {0x5c, 0x00}, {0x73, 0x00}, {0x67, 0x00}, {0x73, 0x04},	/* ASCII 110 - 114 (n - r) */
	{0x6d, 0x00}, {0x78, 0x00}, {0x1c, 0x00}, {0x30, 0x03}, {0x36, 0x06},	/* ASCII 115 - 119 (s - w) */
	{0x80, 0x07}, {0x6e, 0x00}, {0x09, 0x03}, {0x00, 0x00}, {0x00, 0x00},	/* ASCII 120 - 124 (x - z) */
	{0x00, 0x00}, {0x00, 0x00}, {0xff, 0x07},							/* ASCII 125 - 127 */
	/* turn on all segments when incoming character is 127(0x7F). */
};

static unsigned char 	SEG_DIM_VAL = CMD4_PARAM_PW_12_16;
static unsigned char 	SEG_DISP_VAL = CMD4_PARAM_DISP_ON;
static unsigned long	Sema;
static unsigned char	ucSavedLED = 0x00;	/* save LED values for use with DP at the same time. */

#if defined(CONFIG_STLED_MC2302_KEY_SCAN)
static unsigned long	KeyScanEvtId;
static unsigned long	KeyScanTaskId;
static unsigned char	Poll;
#if defined(CONFIG_STLED_MC2302_KEY_SCAN_INTERRUPT)
static unsigned char	Intr;
#endif
#endif


/*---------------------------------------------------------------------------
**	Local Functions
**-------------------------------------------------------------------------*/

/********************************************************************
Function	: P_MC2302_STB_Low
Description : 
Input		: none
Output	: none
Return	: int
********************************************************************/
static void P_MC2302_STB_Low(void)
{
#if defined(CONFIG_STLED_MC2302_NEXUS_GPIO) || defined(CONFIG_STLED_MC2302_NEXUS_SPI_USING_GPIO_FOR_STROBE)
	(void) DRV_GPIO_Write(GPIO_ID_SPI_M_SS0, GPIO_LOW);
#endif
	return;
}

/********************************************************************
Function	: P_MC2302_STB_High
Description : 
Input		: none
Output	: none
Return	: int
********************************************************************/
static void P_MC2302_STB_High(void)
{
#if defined(CONFIG_STLED_MC2302_NEXUS_GPIO) || defined(CONFIG_STLED_MC2302_NEXUS_SPI_USING_GPIO_FOR_STROBE)
	(void) DRV_GPIO_Write(GPIO_ID_SPI_M_SS0, GPIO_HI);
#endif
	return;
}

/********************************************************************
Function	: P_MC2302_Write
Description : 
Input		: none
Output	: none
Return	: int
********************************************************************/
static int P_MC2302_Write(unsigned char ucData)
{
#if defined(CONFIG_STLED_MC2302_NEXUS_GPIO)
	unsigned char ucMask = 0x01;
	int i = 0;

	for ( i=0; i<8; i++ )
	{
		(void) DRV_GPIO_Write(GPIO_ID_SPI_M_CLK, GPIO_LOW);

		if ( (ucData&ucMask) == 0 )	/* data on */
		{
			(void) DRV_GPIO_Write(GPIO_ID_SPI_M_MOSI, GPIO_LOW);
		}
		else							/* data off */
		{
			(void) DRV_GPIO_Write(GPIO_ID_SPI_M_MOSI, GPIO_HI);
		}

		(void) DRV_GPIO_Write(GPIO_ID_SPI_M_CLK, GPIO_HI);

		ucMask <<= 1;
	}
#endif

	return DRV_OK;
}

/********************************************************************
Function	: P_MC2302_WriteCmd
Description : Send 1-byte command data
Input		: none
Output 	: none
Return 	: int
********************************************************************/
static int	P_MC2302_WriteCmd(unsigned char wdata)
{
#if defined(CONFIG_STLED_MC2302_NEXUS_SPI)
	unsigned char cmd = 0x00;

	if ( !SpiHandle )
	{
		PrintDebug("[%s] spi handle is null. ignore this call\n", __FUNCTION__);
		return DRV_ERR;
	}

	VK_SEM_Get(Sema);

	cmd = MSB2LSB(wdata);

	P_MC2302_STB_Low();
	if ( NEXUS_Spi_Write(SpiHandle, (const unsigned char*)&cmd, (size_t)1) != NEXUS_SUCCESS )
	{
		PrintError("[%s] Error! Can't send command data on SPI\n", __FUNCTION__);
		P_MC2302_STB_High();
		VK_SEM_Release(Sema);
		return DRV_ERR;
	}
	P_MC2302_STB_High();

	VK_SEM_Release(Sema);

#elif defined(CONFIG_STLED_MC2302_NEXUS_GPIO)
	VK_SEM_Get(Sema);

	P_MC2302_STB_Low();
	if ( P_MC2302_Write(wdata) != DRV_OK )
	{
		PrintError("[%s] Error! Can't send command data on GPIO\n", __FUNCTION__);
		P_MC2302_STB_High();
		VK_SEM_Release(Sema);
		return DRV_ERR;
	}
	P_MC2302_STB_High();

	VK_SEM_Release(Sema);
#endif

	return DRV_OK;
}

/********************************************************************
Function	: P_MC2302_WriteData
Description : send datas
Input		: none
Output 	: none
Return 	: int
********************************************************************/
static int	P_MC2302_WriteData(unsigned char *wdata, int wlen)
{
	unsigned char cmd = 0x00;
	unsigned char senddata[MAX_WRITE_BUFFER_SIZE];
	int i = 0x00;

#if defined(CONFIG_STLED_MC2302_NEXUS_SPI)	
	if ( !SpiHandle )
	{
		PrintDebug("[%s] spi handle is null. ignore this call\n", __FUNCTION__);
		return DRV_ERR;
	}

	VK_SEM_Get(Sema);

	cmd = MSB2LSB(MC2302_CMD2 | CMD2_PARAM_INCR_ADDR | CMD2_PARAM_WRITE_DATA_TO_DISP);

	P_MC2302_STB_Low();
	if ( NEXUS_Spi_Write(SpiHandle, (const unsigned char*)&cmd, (size_t)1) != NEXUS_SUCCESS )
	{
		PrintError("[%s] Error! Can't send command data on SPI\n", __FUNCTION__);
		P_MC2302_STB_High();
		VK_SEM_Release(Sema);
		return DRV_ERR;
	}
	P_MC2302_STB_High();

	VK_MEM_Memset(senddata, 0, sizeof(senddata));
	senddata[0] = MSB2LSB(MC2302_CMD3 | 0x00);
	for ( i=1; i<=wlen && i<MAX_WRITE_BUFFER_SIZE; i++ )
	{
		senddata[i] = MSB2LSB(wdata[i-1]);
	}

	P_MC2302_STB_Low();
	if ( NEXUS_Spi_Write(SpiHandle, (const unsigned char*)senddata, (size_t)sizeof(senddata)) != NEXUS_SUCCESS )
	{
		PrintError("[%s] Error! Can't send data on SPI\n", __FUNCTION__);
		P_MC2302_STB_High();
		VK_SEM_Release(Sema);
		return DRV_ERR;
	}
	P_MC2302_STB_High();

	VK_SEM_Release(Sema);

#elif defined(CONFIG_STLED_MC2302_NEXUS_GPIO)

	VK_SEM_Get(Sema);

	cmd = (MC2302_CMD2 | CMD2_PARAM_INCR_ADDR | CMD2_PARAM_WRITE_DATA_TO_DISP);

	P_MC2302_STB_Low();
	if ( P_MC2302_Write(cmd) != DRV_OK )
	{
		PrintError("[%s] Error! Can't send command data on GPIO\n", __FUNCTION__);
		P_MC2302_STB_High();
		VK_SEM_Release(Sema);
		return DRV_ERR;
	}
	P_MC2302_STB_High();

	VK_MEM_Memset(senddata, 0, sizeof(senddata));
	senddata[0] = (MC2302_CMD3 | 0x00);
	for ( i=1; i<=wlen && i<MAX_WRITE_BUFFER_SIZE; i++ )
	{
		senddata[i] = wdata[i-1];
	}

	P_MC2302_STB_Low();
	for ( i=0; i<=wlen; i++ )
	{
		if ( P_MC2302_Write(senddata[i]) != DRV_OK )
		{
			PrintError("[%s] Error! Can't send data on GPIO\n", __FUNCTION__);
			P_MC2302_STB_High();
			VK_SEM_Release(Sema);
			return DRV_ERR;
		}
	}
	P_MC2302_STB_High();

	VK_SEM_Release(Sema);
#endif

	return DRV_OK;
}

static int	P_MC2302_WriteDataToFixedAddr(unsigned char *wdata, int wlen, unsigned int addr)
{
	
	unsigned char cmd = 0x00;
	unsigned char senddata[MAX_WRITE_BUFFER_SIZE];
	int i = 0x00;

#if defined(CONFIG_STLED_MC2302_NEXUS_SPI)
	if ( !SpiHandle )
	{
		PrintDebug("[%s] spi handle is null. ignore this call\n", __FUNCTION__);
		return DRV_ERR;
	}

	VK_SEM_Get(Sema);

	cmd = MSB2LSB(MC2302_CMD2 | CMD2_PARAM_FIXED_ADDR | CMD2_PARAM_WRITE_DATA_TO_DISP);

	P_MC2302_STB_Low();
	if ( NEXUS_Spi_Write(SpiHandle, (const unsigned char*)&cmd, (size_t)1) != NEXUS_SUCCESS )
	{
		PrintError("[%s] Error! Can't send command data on SPI\n", __FUNCTION__);
		P_MC2302_STB_High();
		VK_SEM_Release(Sema);
		return DRV_ERR;
	}
	P_MC2302_STB_High();

	VK_MEM_Memset(senddata, 0, sizeof(senddata));
	senddata[0] = MSB2LSB(MC2302_CMD3 | addr);
	for ( i=1; i<=wlen && i<MAX_WRITE_BUFFER_SIZE; i++ )
	{
		senddata[i] = MSB2LSB(wdata[i-1]);
	}

	P_MC2302_STB_Low();
	if ( NEXUS_Spi_Write(SpiHandle, (const unsigned char*)senddata, (size_t)wlen) != NEXUS_SUCCESS )
	{
		PrintError("[%s] Error! Can't send data on SPI\n", __FUNCTION__);
		P_MC2302_STB_High();
		VK_SEM_Release(Sema);
		return DRV_ERR;
	}
	P_MC2302_STB_High();

	VK_SEM_Release(Sema);

#elif defined(CONFIG_STLED_MC2302_NEXUS_GPIO)

	VK_SEM_Get(Sema);

	cmd = (MC2302_CMD2 | CMD2_PARAM_INCR_ADDR | CMD2_PARAM_WRITE_DATA_TO_DISP);

	P_MC2302_STB_Low();
	if ( P_MC2302_Write(cmd) != DRV_OK )
	{
		PrintError("[%s] Error! Can't send command data on GPIO\n", __FUNCTION__);
		P_MC2302_STB_High();
		VK_SEM_Release(Sema);
		return DRV_ERR;
	}
	P_MC2302_STB_High();

	VK_MEM_Memset(senddata, 0, sizeof(senddata));
	senddata[0] = (MC2302_CMD3 | addr);
	for ( i=1; i<=wlen && i<MAX_WRITE_BUFFER_SIZE; i++ )
	{
		senddata[i] = wdata[i-1];
	}

	P_MC2302_STB_Low();
	for ( i=0; i<=wlen; i++ )
	{
		if ( P_MC2302_Write(senddata[i]) != DRV_OK )
		{
			PrintError("[%s] Error! Can't send data on GPIO\n", __FUNCTION__);
			P_MC2302_STB_High();
			VK_SEM_Release(Sema);
			return DRV_ERR;
		}
	}
	P_MC2302_STB_High();

	VK_SEM_Release(Sema);
#endif

	return DRV_OK;
}


#if defined(CONFIG_STLED_MC2302_KEY_SCAN)
/********************************************************************
Function	: P_MC2302_ReadData
Description : Read datas from mc2302 device. usually it is key datas.
Input		: none
Output 	: none
Return 	: int
********************************************************************/
static int	P_MC2302_ReadData(unsigned char *wdata, unsigned char *rdata, int rlen)
{
#if defined(CONFIG_STLED_MC2302_NEXUS_SPI)
	if ( !SpiHandle )
	{
		PrintDebug("[%s] spi handle is null. ignore this call\n", __FUNCTION__);
		return DRV_ERR;
	}

	VK_SEM_Get(Sema);
	
	P_MC2302_STB_Low();
	if ( NEXUS_Spi_Read(SpiHandle, (const unsigned char*)wdata, (unsigned char*)rdata, (size_t)rlen) != NEXUS_SUCCESS )
	{
		PrintError("[%s] Error! Can't read data on SPI\n", __FUNCTION__);
		P_MC2302_STB_High();
		VK_SEM_Release(Sema);
		return DRV_ERR;
	}
	P_MC2302_STB_High();

	VK_SEM_Release(Sema);

#elif defined(CONFIG_STLED_MC2302_NEXUS_GPIO)
	int i = 0, k = 0;
	GPIO_STATE_t eGpioState = NEXUS_GpioValue_eLow;

	VK_SEM_Get(Sema);

	P_MC2302_STB_Low();
	if ( P_MC2302_Write(wdata[0]) != DRV_OK )
	{
		PrintError("[%s] Error! Can't send command data on GPIO\n", __FUNCTION__);
		P_MC2302_STB_High();
		VK_SEM_Release(Sema);
		return DRV_ERR;
	}

	rdata[0] = wdata[0];

	for (k=1; k<rlen; k++)
	{
		rdata[k] = 0;
		
		for (i=0; i<8; i++)
		{
			(void) DRV_GPIO_Write(GPIO_ID_SPI_M_CLK, GPIO_LOW);
			(void) DRV_GPIO_Read(GPIO_ID_SPI_M_MISO, &eGpioState);
			rdata[k] |= (unsigned char)(eGpioState<<(7-i));	/* swap MSB and LSB */
			(void) DRV_GPIO_Write(GPIO_ID_SPI_M_CLK, GPIO_HI);
		}
	}
	P_MC2302_STB_High();

	VK_SEM_Release(Sema);
#endif

	return DRV_OK;
}

#if defined(CONFIG_STLED_MC2302_KEY_SCAN_INTERRUPT)
/********************************************************************
Function	: P_MC2302_KeyISR
Description : Key ISR
Input		: none
Output 	: none
Return 	: int
********************************************************************/
static void P_MC2302_KeyISR(void *context, int param)
{
	(void)context;
	(void)param;

	if (Intr)
	{
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

	if (VK_EVENT_Send(KeyScanEvtId, 0) == VK_OK)
	{
		return;
	}

	Intr = 0;
	/* enable isr */
	(void)DRV_GPIO_INTR_Enable(GPIO_ID_IRQ_FRONT);
	(void)DRV_GPIO_INTR_Enable(GPIO_ID_IRQ_FRONT2);

	return;
}

static void P_MC2302_KeyISR2(void *context, int param)
{
	P_MC2302_KeyISR(context, param);
	return;
}
#endif

/********************************************************************
Function	: P_MC2302_KeyScanTask
Description : Scan which key is pressed and send message to key driver
Input		: none
Output 	: none
Return 	: void
********************************************************************/
static void P_MC2302_KeyScanTask(void *param)
{
	unsigned char	wdata[KEYSCAN_BYTE];
	unsigned char	rdata[KEYSCAN_BYTE];
	unsigned long	frontKey = 0;
	unsigned long	keyBackup = 0;
	unsigned long	keyRepeat = 0;
	unsigned long	evt;

	(void)param;

#if defined(CONFIG_STLED_MC2302_KEY_SCAN_POLLING)
	/* In polling mode, we set Polling variable to TRUE forcibly.
	    If we use interrupt mode not polling, set this variable when interrupt occured */
	Poll = TRUE;
#endif

	VK_MEM_Memset(wdata, 0xff, sizeof(wdata));

	while (1)
	{
		if ( !Poll )
		{
			if ( VK_EVENT_Receive(KeyScanEvtId, &evt) != VK_OK )
				continue;

			Poll = 1;

			/* wait for key-data becomes stable */
			VK_TASK_Sleep(20);
		}

		if ( Poll )
		{
			/* When we read data from mc2302, wdata need read-command in buffer[0] and 0xff in the other buffers.
			    wdata buffer size must be same with rdata buffer size */
			wdata[0] = MSB2LSB(MC2302_CMD2 | CMD2_PARAM_READ_KEY_SCAN_DATA);
			if ( P_MC2302_ReadData(wdata, rdata, sizeof(wdata)) != DRV_OK )
			{
				PrintError("[%s] Error! Can't read data from mc2302\n", __FUNCTION__);
				goto keyscan_end;
			}
			else
			{
				PrintDebug("[%s] rdata[1]=0x%x, rdata[2]=0x%x\n", __FUNCTION__, rdata[1], rdata[2]);

				frontKey = 0;

				if ((rdata[1] & KEYSCAN_VALID_MASK) && ((rdata[1] & KEYSCAN_ZERO_MASK ) == 0))	/* 1st byte */
				{
					/* key pressed */
					if (rdata[1] & KEY_FRONT_B0K1)
					{
						frontKey |= (1<<1);	/* SG1 / KEY_COL1 >> s_BoardFrontKeyMapping KeyID 1 */
					}
					if (rdata[1] & KEY_FRONT_B1K2)
					{
						frontKey |= (1<<2);	/* SG1 / KEY_COL2 >> s_BoardFrontKeyMapping KeyID 2 */
					}
					if (rdata[1] & KEY_FRONT_B2K3)
					{
						frontKey |= (1<<3);	/* SG1 / KEY_COL3 >> s_BoardFrontKeyMapping KeyID 3 */
					}
					if (rdata[1] & KEY_FRONT_B3K1)
					{
						frontKey |= (1<<4);	/* SG2 / KEY_COL1 >> s_BoardFrontKeyMapping KeyID 4 */
					}
					if (rdata[1] & KEY_FRONT_B4K2)
					{
						frontKey |= (1<<5);	/* SG2 / KEY_COL2 >> s_BoardFrontKeyMapping KeyID 5 */
					}
					if (rdata[1] & KEY_FRONT_B5K3)
					{
						frontKey |= (1<<6);	/* SG2 / KEY_COL3 >> s_BoardFrontKeyMapping KeyID 6 */
					}
				}

				if ((rdata[2] & KEYSCAN_VALID_MASK) && ((rdata[2] & KEYSCAN_ZERO_MASK ) == 0))	/* 2nd byte */
				{
					/* key pressed */
					if (rdata[2] & KEY_FRONT_B0K1)
					{
						frontKey |= (1<<7);	/* SG3 / KEY_COL1 >> s_BoardFrontKeyMapping KeyID 7 */
					}
					if (rdata[2] & KEY_FRONT_B1K2)
					{
						frontKey |= (1<<8);	/* SG3 / KEY_COL2 >> s_BoardFrontKeyMapping KeyID 8 */
					}
					if (rdata[2] & KEY_FRONT_B2K3)
					{
						frontKey |= (1<<9);	/* SG3 / KEY_COL3 >> s_BoardFrontKeyMapping KeyID 9 */
					}
					if (rdata[2] & KEY_FRONT_B3K1)
					{
						frontKey |= (1<<10);	/* SG4 / KEY_COL1 >> s_BoardFrontKeyMapping KeyID 10 */
					}
					if (rdata[2] & KEY_FRONT_B4K2)
					{
						frontKey |= (1<<11);	/* SG4 / KEY_COL2 >> s_BoardFrontKeyMapping KeyID 11 */
					}
					if (rdata[2] & KEY_FRONT_B5K3)
					{
						frontKey |= (1<<12);	/* SG4 / KEY_COL3 >> s_BoardFrontKeyMapping KeyID 12 */
					}
				}

				if (frontKey != 0)
				{
					keyRepeat = keyBackup & frontKey;
					keyBackup = frontKey;

					if (keyRepeat)
					{
						PrintDebug("[%s] Key Repeated(0x%02x)\n", __FUNCTION__, keyRepeat);
						(void)DRV_KEY_SendMultiKeyMessage(keyRepeat, 1);
						frontKey = frontKey & ~keyRepeat;
					}
					if (frontKey)
					{
						PrintDebug("[%s] Key Pressed(0x%02x)\n", __FUNCTION__, frontKey);
						(void)DRV_KEY_SendMultiKeyMessage(frontKey, 0);
					}
				}
				else
				{
					/* no key pressed */
					keyBackup = 0;
#if defined(CONFIG_STLED_MC2302_KEY_SCAN_INTERRUPT)
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
#endif	/* if defined(CONFIG_STLED_MC2302_KEY_SCAN) */

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
#if defined(CONFIG_STLED_MC2302_NEXUS_SPI)
	NEXUS_SpiSettings	SpiSettings;
#endif

	PrintDebug("[%s] Resuming...\n", __FUNCTION__);

#if defined(CONFIG_STLED_MC2302_NEXUS_SPI)
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
	// SpiSettings.dtl = 100;

	SpiHandle = NEXUS_Spi_Open(0, &SpiSettings);
	if (!SpiHandle)
	{
		PrintError("[%s] Error! can't resume\n", __FUNCTION__);
		return;
	}
#endif

	/* Initialize MC2302 device */
	cmd = (MC2302_CMD1 | CMD1_PARAM_11SEG_7GRID);
	if ( P_MC2302_WriteCmd(cmd) != DRV_OK )
	{
		PrintError("[%s] Error! fail to initialize MC2302(CMD1/2)\n", __FUNCTION__);
		return;
	}

	cmd = (MC2302_CMD2 | CMD2_PARAM_INCR_ADDR | CMD2_PARAM_WRITE_DATA_TO_DISP);
	if ( P_MC2302_WriteCmd(cmd) != DRV_OK )
	{
		PrintError("[%s] Error! fail to initialize MC2302(CMD1/2)\n", __FUNCTION__);
		return;
	}

	/* Initialize MC2302 Memory buffer */
#if defined(CONFIG_PRODUCT_IMAGE_DEBUG) || defined(CONFIG_PRODUCT_IMAGE_HWTEST)
	VK_MEM_Memset(wdata, 0xff, sizeof(wdata));
	if ( P_MC2302_WriteData(wdata, sizeof(wdata)) != DRV_OK )
	{
		PrintError("[%s] Error! fail to initialize MC2302(CMD3)\n", __FUNCTION__);
		return;
	}
#else
	VK_MEM_Memset(wdata, 0x00, sizeof(wdata));
	if ( P_MC2302_WriteData(wdata, sizeof(wdata)) != DRV_OK )
	{
		PrintError("[%s] Error! fail to initialize MC2302(CMD3)\n", __FUNCTION__);
		return;
	}
#endif

	/* Turn on segments and set dimming value*/
	cmd = (MC2302_CMD4 | SEG_DISP_VAL | SEG_DIM_VAL);
	if ( P_MC2302_WriteCmd(cmd) != DRV_OK )
	{
		PrintError("[%s] Error! fail to initialize MC2302(CMD4)\n", __FUNCTION__);
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

#if defined(CONFIG_STLED_MC2302_NEXUS_SPI)
	if (!SpiHandle)
		return;

	NEXUS_Spi_Close(SpiHandle);
	SpiHandle = 0;
#endif
}

#endif	/* if defined(CONFIG_BRCM_MOD_NEXUS_POWER_MANAGEMENT) */


/********************************************************************
Function	: DRV_STLED_WriteString
Description :
Input		:
Output	: none
Return	: DRV_OK
*********************************************************************/
DRV_Error DRV_STLED_WriteString(unsigned char *string, unsigned char len, int isTime)
{
	unsigned char wdata[MAX_DATA_SIZE];
	unsigned char sdata[2] = {0,};
	int i = 0x00;

	if ( !string )
	{
		PrintError("[%s] Error! string is NULL\n", __FUNCTION__);
		return DRV_ERR;
	}

	VK_MEM_Memset(wdata, 0x00, sizeof(wdata));

	/* We support 4-grid 11-segment */
	if ( len > MC2302_GRID_NUM )
	{
		len = MC2302_GRID_NUM;

		/* add ":" handling */
		if ( string[MC2302_GRID_NUM] == 0x3A )
		{
			wdata[MAX_DATA_SIZE-2] = 0x18;
		}
	}

	for ( i=0; i<len; i++ )
	{
		sdata[0] = Char2SegmentMap[(string[i]&0x7F)][0];
		sdata[1] = Char2SegmentMap[(string[i]&0x7F)][1];

		wdata[i*2] = sdata[0];
		wdata[i*2+1] = sdata[1];
	}

	/* clock */
	if ( isTime )
	{
		wdata[MAX_DATA_SIZE-2] = 0x18;
	}

	/* restore LED values */
	wdata[MAX_DATA_SIZE-2] |= ucSavedLED;

	if ( P_MC2302_WriteData(wdata, sizeof(wdata)) != DRV_OK )
	{
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
	unsigned char cmd = 0x00;
	unsigned char sdata = 0x00;

	switch(value)
	{
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

	cmd = (MC2302_CMD4 | SEG_DISP_VAL | sdata);
	if ( P_MC2302_WriteCmd(cmd) != DRV_OK )
	{
		PrintError("[%s] Error! Fail to write brightness value\n", __FUNCTION__);
		return DRV_ERR;
	}

	SEG_DIM_VAL = sdata;

	return DRV_OK;
}

/********************************************************************
Function	: DRV_STLED_DisplayLED
Description : 	ucData[0] = 0x01 : M1 (Power Green)
			ucData[0] = 0x02 : M2 (Modem Green)
			ucData[0] = 0x04 : M3 (RemoteControl Green)
			ucData[0] = 0x20 : R1 (Power Red)
Input	: unsigned char *
Output	: none
Return	: DRV_OK
*********************************************************************/
DRV_Error DRV_STLED_DisplayLED (unsigned char *ucData)
{
	unsigned char wdata[2];

	if ( !ucData )
	{
		PrintError("[%s] Error! LED is NULL\n", __FUNCTION__);
		return DRV_ERR;
	}

	VK_MEM_Memset(wdata, 0x00, sizeof(wdata));

	wdata[0] = ucData[0];
	wdata[1] = ucData[1];

	if ( P_MC2302_WriteDataToFixedAddr(wdata, sizeof(wdata), MC2302_LED_GRID_FIXED_ADDRESS) != DRV_OK )
	{
		PrintError("[%s] Error! Fail to write led data\n", __FUNCTION__);
		return DRV_ERR;
	}

	ucSavedLED = ucData[0];

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
	unsigned char cmd = 0x00;
	unsigned char	param_on = CMD4_PARAM_DISP_OFF;

	if ( OnOff )
	{
		param_on = CMD4_PARAM_DISP_ON;
	}

	cmd = (MC2302_CMD4 | param_on | SEG_DIM_VAL);
	if ( P_MC2302_WriteCmd(cmd) != DRV_OK )
	{
		PrintError("[%s] Error! Fail to write display on/off data\n", __FUNCTION__);
		return DRV_ERR;
	}

	SEG_DISP_VAL = param_on;

	return DRV_OK;
}

/********************************************************************
Function	: DRV_STLED_7SegmentTest
Description : test 11-segments
Input		: ON/OFF
Output	: none
Return	: DRV_OK
*********************************************************************/
DRV_Error DRV_STLED_7SegmentTest(int Onoff)
{
	unsigned char wdata[MAX_DATA_SIZE];
	int i = 0, j = 0;

	if ( Onoff )
	{
		VK_MEM_Memset(wdata, 0x00, sizeof(wdata));

		/* turn on 11-segments, turn by turn */
		for ( i=0; i<MC2302_GRID_NUM; i++ )
		{
			for ( j=0; j<11; j++ )
			{
				wdata[i*2+(j/8)] |= (1 << (j%8));
				(void) P_MC2302_WriteData(wdata, sizeof(wdata));
				VK_TASK_Sleep(100);
			}
		}

		/* turn on LED and DP, turn by turn */
		for ( j=0; j<6; j++ )
		{
			wdata[i*2] |= (1 << j);
			(void) P_MC2302_WriteData(wdata, sizeof(wdata));
			VK_TASK_Sleep(100);
		}
	}
	else
	{
		VK_MEM_Memset(wdata, 0x00, sizeof(wdata));
		(void)P_MC2302_WriteData(wdata, sizeof(wdata));
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
#if defined(CONFIG_STLED_MC2302_NEXUS_SPI)
	NEXUS_SpiSettings SpiSettings;
#endif
	unsigned char wdata[MAX_DATA_SIZE];
	unsigned char cmd = 0x00;

	if ( bInitialized )
	{
		PrintError("[%s] STLED already initialized!\n", __FUNCTION__);
		return DRV_OK;
	}

	if ( VK_SEM_Create(&Sema,"mc2302-sema", DEFAULT_SUSPEND_TYPE) != VK_OK )
	{
		PrintError("[%s] failed to create sema!\n", __FUNCTION__);
		return DRV_ERR;
	}

#if defined(CONFIG_STLED_MC2302_NEXUS_SPI)
	NEXUS_Spi_GetDefaultSettings(&SpiSettings);

	/* These settings need to be vendor specific. Please populate spiSettings accordingly */
	SpiSettings.clockActiveLow = false;
	SpiSettings.bitsPerTransfer = 8;
	SpiSettings.baud = 333000;      /* 333KHz */
	SpiSettings.interruptMode = false;
	SpiSettings.lastByteContinueEnable = false;
	SpiSettings.txLeadingCapFalling = false;
	SpiSettings.useUserDtlAndDsclk = false;
	// SpiSettings.dtl = 100;

	SpiHandle = NEXUS_Spi_Open(0, &SpiSettings);
	if ( !SpiHandle )
	{
		PrintError("[%s] Error! can't open nexus spi\n", __FUNCTION__);
		return DRV_ERR;
	}
#elif defined(CONFIG_STLED_MC2302_NEXUS_GPIO)
	/* set gpio mode */
	DRV_GPIO_ChangeMode(GPIO_ID_SPI_M_CLK, GPIO_MODE_WRITE, GPIO_HI);
	DRV_GPIO_ChangeMode(GPIO_ID_SPI_M_MOSI, GPIO_MODE_WRITE, GPIO_HI);
	DRV_GPIO_ChangeMode(GPIO_ID_SPI_M_MISO, GPIO_MODE_READ, GPIO_HI);
	DRV_GPIO_ChangeMode(GPIO_ID_SPI_M_SS0, GPIO_MODE_WRITE, GPIO_HI);

	/* set initial value */
	DRV_GPIO_Write(GPIO_ID_SPI_M_CLK, GPIO_HI);
	DRV_GPIO_Write(GPIO_ID_SPI_M_MOSI, GPIO_HI);
	DRV_GPIO_Write(GPIO_ID_SPI_M_MISO, GPIO_HI);
	DRV_GPIO_Write(GPIO_ID_SPI_M_SS0, GPIO_HI);
#endif

	/* Initialize MC2302 device */
	cmd = (MC2302_CMD1 | CMD1_PARAM_13SEG_5GRID);
	if ( P_MC2302_WriteCmd(cmd) != DRV_OK )
	{
		PrintError("[%s] Error! fail to initialize MC2302(CMD1)\n", __FUNCTION__);
		return DRV_ERR;
	}

	cmd = (MC2302_CMD2 | CMD2_PARAM_INCR_ADDR | CMD2_PARAM_WRITE_DATA_TO_DISP);
	if ( P_MC2302_WriteCmd(cmd) != DRV_OK )
	{
		PrintError("[%s] Error! fail to initialize MC2302(CMD2)\n", __FUNCTION__);
		return DRV_ERR;
	}

	/* Initialize MC2302 Memory buffer */
	VK_MEM_Memset(wdata, 0x00, sizeof(wdata));
	if ( P_MC2302_WriteData(wdata, sizeof(wdata)) != DRV_OK )
	{
		PrintError("[%s] Error! fail to initialize MC2302(CMD3)\n", __FUNCTION__);
		return DRV_ERR;
	}

	/* Turn on segments and set dimming value*/
	cmd = (MC2302_CMD4 | SEG_DISP_VAL | SEG_DIM_VAL);
	if ( P_MC2302_WriteCmd(cmd) != DRV_OK )
	{
		PrintError("[%s] Error! fail to initialize MC2302(CMD4)\n", __FUNCTION__);
		return DRV_ERR;
	}

#if defined(CONFIG_STLED_MC2302_KEY_SCAN)
	if ( VK_EVENT_Create(&KeyScanEvtId, "keyscan_evt") != VK_OK )
	{
		return DRV_ERR;
	}

	/* If we use key which connected with mc2302 device, create task for scanning key data */
	if ( VK_TASK_Create(P_MC2302_KeyScanTask, USB_TASK_PRIORITY, USB_TASK_STACK_SIZE, "MC2302KeyScanTask", NULL, &KeyScanTaskId, FALSE) != VK_OK )
	{
		return DRV_ERR;
	}

	VK_TASK_Start(KeyScanTaskId);

#if defined(CONFIG_STLED_MC2302_KEY_SCAN_INTERRUPT)
	if ( DRV_GPIO_INTR_RegisterEventNotify(GPIO_ID_IRQ_FRONT, P_MC2302_KeyISR, NULL, GPIO_ID_IRQ_FRONT) != DRV_OK )
	{
		PrintError("[%s] Error! Can't init interrupt on GPIO\n", __FUNCTION__);
		return DRV_ERR;
	}
	if ( DRV_GPIO_INTR_Enable(GPIO_ID_IRQ_FRONT) != DRV_OK )
	{
		PrintError("[%s] Error! Can't enable interrupt on GPIO\n", __FUNCTION__);
		return DRV_ERR;
	}
	if ( DRV_GPIO_INTR_RegisterEventNotify(GPIO_ID_IRQ_FRONT2, P_MC2302_KeyISR2, NULL, GPIO_ID_IRQ_FRONT2) != DRV_OK )
	{
		PrintError("[%s] Error! Can't init interrupt2 on GPIO\n", __FUNCTION__);
		return DRV_ERR;
	}
	if (DRV_GPIO_INTR_Enable(GPIO_ID_IRQ_FRONT2) != DRV_OK)
	{
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

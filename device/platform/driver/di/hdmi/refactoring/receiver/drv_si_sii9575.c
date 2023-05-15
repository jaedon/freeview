/*
 * $Header:   $
 */
/*********************************************************************
 * $Workfile:   drv_sil9575.c  $
 * $Modtime:   $
 *
 * Author:
 * Description:
 *
 *                                 Copyright (c) 2008 HUMAX Co., Ltd.
 *                                               All rights reserved.
 ********************************************************************/



/*******************************************************************/
/**************************** Header Files *************************/
/*******************************************************************/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "drv_err.h"
#include "drv_i2c.h"
#include "vkernel.h"
#include "taskdef.h"


/*******************************************************************/
/****************************** define *****************************/
/*******************************************************************/
#define SIL9575_DEBUG
#if defined(CONFIG_DI_SHARED_LIB)
#ifdef SIL9575_DEBUG
#define PrintDebug(fmt, ...) 		VK_DBG_Print (fmt, ## __VA_ARGS__)
#define PrintError(fmt, ...)    	VK_DBG_Print (fmt, ## __VA_ARGS__)
#else
#define PrintDebug(...)
#define PrintError(...)				VK_DBG_Print (__VA_ARGS__)
#endif
#else
#ifdef SIL9575_DEBUG
#define PrintDebug(fmt, ...)		VK_DBG_Print (fmt , ## __VA_ARGS__)
#define PrintError(fmt, ...)		VK_DBG_Print (fmt , ## __VA_ARGS__)
#else
#define PrintDebug(fmt, ...)
#define PrintError(fmt, ...)		VK_DBG_Print (fmt , ## __VA_ARGS__)
#endif
#endif

#define DONE_ERR(ret) {						\
		if(ret) {						\
			PrintError("ERROR %s,%d\n", __FUNCTION__, __LINE__); \
			goto done; \
		}							\
	}

#define SIL9575_NEXUS_I2C_INDEX	2

enum {
	DEV_PAGE_PP_0 = (0xB0>>1),
	DEV_PAGE_PP_5 = (0x50>>1),
	DEV_PAGE_PP_6 = (0x52>>1),
	DEV_PAGE_PP_7 = (0x54>>1),
	DEV_PAGE_PP_9 = (0xE0>>1),
	DEV_PAGE_PP_A = (0x64>>1),
	DEV_PAGE_PP_B = (0x90>>1),
	DEV_PAGE_PP_F = (0xF8>>1),
	DEV_PAGE_TX_0 = (0x72>>1),
	DEV_PAGE_TX_1 = (0x70>>1)
};

#define PP_0_RX_PORT_SEL 0x0A
	#define PP_0_RX_PORT_SEL_MAXTRIX_SWITCH_EN_MASK 0x80
	#define PP_0_RX_PORT_SEL_RP_PORT_SEL_MASK 0x38
	#define PP_0_RX_PORT_SEL_RP_PORT_SEL_SHIFT 3
	#define PP_0_RX_PORT_SEL_MP_PORT_SEL_MASK 0x07
	#define PP_0_RX_PORT_SEL_MP_PORT_SEL_SHIFT 0

/*******************************************************************/
/************************ global variables *************************/
/*******************************************************************/


/*******************************************************************/
/************************ static variables *************************/
/*******************************************************************/
static const unsigned char Sil9575_EDID[] =
{
	0x00, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0x00, 0x08, 0x6D, 0x25, 0x74, 0x67, 0x45, 0x23, 0x01,
	0x18, 0x18, 0x01, 0x03, 0x80, 0x80, 0x48, 0x78, 0x0A, 0xDA, 0xFF, 0xA3, 0x58, 0x4A, 0xA2, 0x29,
	0x17, 0x49, 0x4B, 0x20, 0x00, 0x00, 0x31, 0x40, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01,
	0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x02, 0x3A, 0x80, 0x18, 0x71, 0x38, 0x2D, 0x40, 0x58, 0x2C,
	0x45, 0x00, 0x20, 0xC2, 0x31, 0x00, 0x00, 0x1E, 0x04, 0x74, 0x00, 0x30, 0xF2, 0x70, 0x5A, 0x80,
	0xB0, 0x58, 0x8A, 0x00, 0x20, 0xC2, 0x31, 0x00, 0x00, 0x1E, 0x00, 0x00, 0x00, 0xFC, 0x00, 0x42,
	0x72, 0x6F, 0x61, 0x64, 0x63, 0x6F, 0x6D, 0x20, 0x31, 0x2E, 0x30, 0x0A, 0x00, 0x00, 0x00, 0xFD,
	0x00, 0x17, 0x3D, 0x0F, 0x88, 0x1E, 0x00, 0x0A, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x01, 0xBF,

	0x02, 0x03, 0x2F, 0xF0, 0x4B, 0x10, 0x05, 0x20, 0x22, 0x04, 0x03, 0x02, 0x07, 0x06, 0x5E, 0x5F,
	0x23, 0x09, 0x07, 0x01, 0x77, 0x03, 0x0C, 0x00, 0x10, 0x00, 0xB8, 0x3C, 0x21, 0xC8, 0x6A, 0x01,
	0x03, 0x04, 0x81, 0x41, 0x00, 0x16, 0x06, 0x08, 0x00, 0x56, 0x58, 0x00, 0xE2, 0x00, 0x4B, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x8D
};

/*******************************************************************/
/****************** Static Function Prototypes *********************/
/*******************************************************************/
static void SiiRegWrite(unsigned char usChipAddr, unsigned char usSubAddr, unsigned char ucData)
{
	(void)DRV_I2c_WriteA8(SIL9575_NEXUS_I2C_INDEX, usChipAddr, usSubAddr, &ucData, sizeof(unsigned char));
}

static unsigned char SiiRegRead(unsigned char usChipAddr, unsigned char usSubAddr)
{
	unsigned char ucBuffer = 0;

	(void)DRV_I2c_ReadA8(SIL9575_NEXUS_I2C_INDEX, usChipAddr, usSubAddr, &ucBuffer, sizeof(unsigned char));

	return ucBuffer;
}

static void SiiRegBitsSet(unsigned char usChipAddr, unsigned char usSubAddr, unsigned char bitMask, unsigned char setBits )
{
    unsigned char aByte;

    aByte = SiiRegRead(usChipAddr, usSubAddr);
    aByte = (setBits) ? (aByte | bitMask) : (aByte & ~bitMask);
    SiiRegWrite(usChipAddr, usSubAddr, aByte);
}

static void SiiRegModify(unsigned char usChipAddr, unsigned char usSubAddr, unsigned char mask, unsigned char value)
{
    unsigned char aByte;

    aByte = SiiRegRead(usChipAddr, usSubAddr);
    aByte &= (~mask);
    aByte |= (mask & value);
    SiiRegWrite(usChipAddr, usSubAddr, aByte );
}

static void SiiRegWriteBlock(unsigned char usChipAddr, unsigned char usSubAddr, unsigned char *pBuffer, unsigned short count )
{
	unsigned int i;

	for (i=0; i<count; i++)
	{
		(void)DRV_I2c_WriteA8(SIL9575_NEXUS_I2C_INDEX, usChipAddr, usSubAddr, &pBuffer[i], 1);
	}
}

/*******************************************************************/
/****************** Global Function Prototypes *********************/
/*******************************************************************/
DRV_Error DRV_Si_Sii9575_Init(void)
{
	unsigned char val;
	unsigned int ramSelect;

	/* Hard Reset */
	SiiRegWrite(DEV_PAGE_PP_9, 0xFF, 0x80);
	(void)SiiRegRead(DEV_PAGE_PP_9, 0xFF);  /*Dummy read to clear write interrupted by reset*/

	/* Boot */
	/*SiiRegWrite(DEV_PAGE_PP_9, 0x03, 0x10);
	SiiRegWrite(DEV_PAGE_PP_9, 0x05, 0x06);
	SiiRegWrite(DEV_PAGE_PP_9, 0x08, 0x01);*/

	/* Download EDID to EDID SDRAM */
	for (ramSelect=0; ramSelect<6; ramSelect++)
	{
		SiiRegModify(DEV_PAGE_PP_9, 0x04, 0x07, ramSelect);
		SiiRegWrite(DEV_PAGE_PP_9, 0x02, 0);
		SiiRegWriteBlock(DEV_PAGE_PP_9, 0x03, Sil9575_EDID, sizeof(Sil9575_EDID));
	}

	/* Termination Control */
	SiiRegWrite(DEV_PAGE_PP_0, 0x81, 0x00);
	SiiRegWrite(DEV_PAGE_PP_0, 0x82, 0x00);

	/* Enable HDCP DDC */
	SiiRegWrite(DEV_PAGE_PP_0, 0x09, 0xFC);

	/* Enable EDID DDC */
	SiiRegWrite(DEV_PAGE_PP_9, 0x01, 0x7F);

	/* Enable Manual HPD control */
	SiiRegWrite(DEV_PAGE_PP_9, 0x14, 0x3F);

	/* HPD High */
	SiiRegWrite(DEV_PAGE_PP_0, 0x0F, 0x05);
	SiiRegWrite(DEV_PAGE_PP_0, 0x10, 0x55);

	/* Ready for the ARC and Disable in default */
	/*
	val = SiiRegRead(DEV_PAGE_PP_B, 0xB0);
	val = val & 0xc7;
	SiiRegWrite (DEV_PAGE_PP_B, 0xB0, val);

	val = SiiRegRead(DEV_PAGE_PP_B, 0xB1);
	val = val & 0x01;
	SiiRegWrite (DEV_PAGE_PP_B, 0xB1, val);
	*/

	/* Enable packet bypass mode */
	SiiRegWrite(DEV_PAGE_PP_F, 0xF1, 0x18);
	SiiRegWrite(DEV_PAGE_PP_F, 0xF9, 0x18);

	/* Enable Tx0/Tx1 */
	SiiRegWrite(DEV_PAGE_PP_0, 0x50, 0xC0);

	SiiRegWrite(DEV_PAGE_TX_0, 0x82, 0x00);
	SiiRegWrite(DEV_PAGE_TX_0, 0x85, 0xDB);
	SiiRegWrite(DEV_PAGE_TX_0, 0x84, 0x04);
	SiiRegWrite(DEV_PAGE_TX_0, 0xC7, 0x00);
	SiiRegWrite(DEV_PAGE_TX_0, 0x1E, 0x00);
	SiiRegWrite(DEV_PAGE_TX_0, 0x0A, 0x02);
	SiiRegWrite(DEV_PAGE_TX_0, 0x1A, 0x01);

	SiiRegWrite(DEV_PAGE_TX_1, 0x82, 0x00);
	SiiRegWrite(DEV_PAGE_TX_1, 0x85, 0xDB);
	SiiRegWrite(DEV_PAGE_TX_1, 0x84, 0x04);
	SiiRegWrite(DEV_PAGE_TX_1, 0x0A, 0x02);
	SiiRegWrite(DEV_PAGE_TX_1, 0xC7, 0x00);
	SiiRegWrite(DEV_PAGE_TX_1, 0x1E, 0x00);
	SiiRegWrite(DEV_PAGE_TX_1, 0x1A, 0x01);

	/* TDRET TDsSII9575HdmiSwitch::Connect(TDSource_k eSrc) */
	/* Set up 9575 special registers and mute whilst doing the rest. */
	val = SiiRegRead(DEV_PAGE_PP_5, 0x63);
	val = val & 0xcf;
	val = val | 0x30;
	SiiRegWrite(DEV_PAGE_PP_5, 0x63, val);

	/* Enable Matrix Switch Mode and Select input0 to main/sub pipe */
	SiiRegWrite(DEV_PAGE_PP_0, PP_0_RX_PORT_SEL, PP_0_RX_PORT_SEL_MAXTRIX_SWITCH_EN_MASK);

	val = SiiRegRead(DEV_PAGE_PP_5, 0x63);
	val = val & 0xcf;
	SiiRegWrite(DEV_PAGE_PP_5, 0x63, val);

	return DRV_OK;
}

DRV_Error DRV_Si_Sii9575_ChangePortMap(HUINT8 InputPortNum, HUINT8 OutputPortNum)
{
	DRV_Error eDrvRet = DRV_ERR;
	unsigned char val;
	unsigned char mask = (OutputPortNum == 1)?PP_0_RX_PORT_SEL_RP_PORT_SEL_MASK:PP_0_RX_PORT_SEL_MP_PORT_SEL_MASK;
	unsigned char shift = (OutputPortNum == 1)?PP_0_RX_PORT_SEL_RP_PORT_SEL_SHIFT:PP_0_RX_PORT_SEL_MP_PORT_SEL_SHIFT;
	PrintError("InputPortNum = %d, 0x%02x, OutputPortNum = %d, 0x%02x\n",InputPortNum,InputPortNum, OutputPortNum, OutputPortNum);

	DONE_ERR((InputPortNum>5));
	DONE_ERR((OutputPortNum>1));

	val = SiiRegRead(DEV_PAGE_PP_0, PP_0_RX_PORT_SEL);
	val = val & ~mask;
	val = val | (InputPortNum<<shift);
	SiiRegWrite(DEV_PAGE_PP_0, PP_0_RX_PORT_SEL, val);

	eDrvRet = DRV_OK;
done:
	return eDrvRet;
}

/************************************************************************
 *
 *	TC90507.h
 *
 *	Copyright (c) 2007 by Humax	Corp.
 *
************************************************************************/
#if defined(CONFIG_VK_STDLIB)
#else
#include 	<stdio.h>
#include 	<stdlib.h>
#include 	<string.h>
#endif

#include "di_channel.h"
#include "di_channel_priv.h"
#include "di_channel_t.h"
#include "di_channel_attr.h"

#include "drv_err.h"
#include "drv_i2c.h"
#include "drv_channel_t.h"
#include "drv_gpio.h"

#include "drv_channel.h"
#include "drv_channel_mxl683.h"


/****************************************************
	Define
******************************************************/
#define ISDBT_LOCK_TRIAL	2
#define ISDBT_LAYER_NUM	3
#define MAX_NUM_TER_UNITS		4


/****************************************************
	Extern  Variable
******************************************************/
extern HINT32 show_di_ch;

/****************************************************
	Groval Variable
******************************************************/

/****************************************************
	Static Variable
******************************************************/
static CH_TER_AntStatus_t s_stAntState[MAX_NUM_TER_UNITS];

/****************************************************
	Static Function
******************************************************/
int MXL683_I2C_Read(int nChannelId , HUINT16 ucRegAddr, HUINT16 *pucBuffer);
int MXL683_I2C_Write(int nChannelId , HUINT16 ucRegAddr, HUINT16 pucBuffer);
int MXL683_I2C_ReadBlock(int nChannelId,  HUINT16 ucRegAddr, HUINT16 length, HUINT8 *pucBuffer);
int MXL683_I2C_WriteBlock(int nChannelId,  HUINT16 ucRegAddr, HUINT16 length, HUINT8 *pucBuffer);

MXL_STATUS_E Ctrl_DownloadFirmwareSegment(HUINT8 devId, HUINT32 address, const HUINT8* dataPtr, HUINT32 dataLen);
MXL_STATUS_E Ctrl_DownloadFirmware(int devId, MBIN_FILE_T* mbinPtr, HUINT8 enableRun);
MXL_STATUS_E Ctrl_SendHostCommand(int devId, PHOST_COMMAND_T commandPtr);
/****************************************************
	Extern Function
******************************************************/
extern void DRV_CFG_SetTSlineConfiguration(int nChannelId);


const HUINT8 MxLWareDrvVersion[] = {1, 2, 3, 6};
const HUINT8 BuildNumber = 0;

HUINT32 MxL68x_I2cMaxWriteLen = MAX_BLOCK_WRITE_LENGTH;

MXL68x_DEV_CONTEXT_T  MxL68x_Dev[3];

MXL683_Signal_Info MxL68x_Signal_Information[3];


// Analog NTSC application mode setting
REG_CTRL_INFO_T MxL68x_AnalogNtsc[] =
{
  {0x0C, 0xFF, 0x08},
  {0x13, 0xFF, 0x00},
  {0x53, 0xFF, 0x7E},
  {0x57, 0xFF, 0x91},
  {0x5C, 0xFF, 0xB1},
  {0x62, 0xFF, 0xC2},
  {0x6E, 0xFF, 0x01},
  {0x6F, 0xFF, 0x51},
  {0x87, 0xFF, 0x56},
  {0x88, 0xFF, 0x44},
  {0x93, 0xFF, 0x33},
  {0x97, 0xFF, 0xA3},
  {0xBA, 0xFF, 0x40},
  {0x98, 0xFF, 0xA5},
  {0x9B, 0xFF, 0x20},
  {0x9C, 0xFF, 0x20},
  {0xA0, 0xFF, 0x10},
  {0xA5, 0xFF, 0x0F},
  {0xC2, 0xFF, 0xAB},
  {0xC5, 0xFF, 0x7C},
  {0xD5, 0xFF, 0x05},
  {0xD9, 0xFF, 0x00},
  {0xEA, 0xFF, 0x00},
  {0xDC, 0xFF, 0x1C},
  {0,    0,    0}
};

// DFE_RFLUT_DIV_IN_SWP for NTSC HIF mode
REG_CTRL_INFO_T MxL68x_Ntsc_RfLutSwpHIF[] =
{
  {0x4A, 0xFF, 0x1B},
  {0x4B, 0xFF, 0x1A},
  {0x4C, 0xFF, 0x19},
  {0x4D, 0xFF, 0x16},
  {0x4E, 0xFF, 0x15},
  {0x4F, 0xFF, 0x14},
  {0x50, 0xFF, 0x13},
  {0x51, 0xFF, 0x12},
  {0x52, 0xFF, 0x17},
  {0,    0,    0}
};

// DFE_RFLUT_DIV_IN_SWP for NTSC - 16 MHz XTAL LIF mode
REG_CTRL_INFO_T MxL68x_Ntsc_16MHzRfLutSwpLIF[] =
{
  {0x4A, 0xFF, 0x1B},
  {0x4B, 0xFF, 0x1A},
  {0x4C, 0xFF, 0x19},
  {0x4D, 0xFF, 0x17},
  {0x4E, 0xFF, 0x16},
  {0x4F, 0xFF, 0x15},
  {0x50, 0xFF, 0x14},
  {0x51, 0xFF, 0x13},
  {0x52, 0xFF, 0x12},
  {0,    0,    0}
};

// DFE_RFLUT_DIV_IN_SWP for NTSC - 24 MHz XTAL LIF mode
REG_CTRL_INFO_T MxL68x_Ntsc_24MHzRfLutSwpLIF[] =
{
  {0x4A, 0xFF, 0x1C},
  {0x4B, 0xFF, 0x1B},
  {0x4C, 0xFF, 0x1A},
  {0x4D, 0xFF, 0x19},
  {0x4E, 0xFF, 0x17},
  {0x4F, 0xFF, 0x16},
  {0x50, 0xFF, 0x15},
  {0x51, 0xFF, 0x14},
  {0x52, 0xFF, 0x13},
  {0,    0,    0}
};

// DFE_RFLUT_DIV_IN_SWP for NTSC - 24 MHz XTAL LIF mode
REG_CTRL_INFO_T MxL68x_Ntsc_HRCRfLutSwpLIF[] =
{
  {0x4A, 0xFF, 0x1C},
  {0x4B, 0xFF, 0x1B},
  {0x4C, 0xFF, 0x1A},
  {0x4D, 0xFF, 0x19},
  {0x4E, 0xFF, 0x17},
  {0x4F, 0xFF, 0x16},
  {0x50, 0xFF, 0x15},
  {0x51, 0xFF, 0x14},
  {0x52, 0xFF, 0x13},
  {0xDD, 0xFF, 0x01},
  {0,    0,    0}
};

// DFE_RFLUT_DIV_IN_SWP settings mode PAL LIF
REG_CTRL_INFO_T MxL68x_Pal_RfLutSwpLIF[] =
{
  {0x4B, 0xFF, 0x1B},
  {0x4C, 0xFF, 0x1A},
  {0x4D, 0xFF, 0x19},
  {0x4E, 0xFF, 0x17},
  {0x4F, 0xFF, 0x16},
  {0x50, 0xFF, 0x15},
  {0x51, 0xFF, 0x14},
  {0x52, 0xFF, 0x13},
  {0,    0,    0}
};

// DFE_RFLUT_DIV_IN_SWP settings mode PAL-D HIF
REG_CTRL_INFO_T MxL68x_PalD_RfLutSwpHIF[] =
{
  {0x4B, 0xFF, 0x14},
  {0x4C, 0xFF, 0x13},
  {0x4D, 0xFF, 0x15},
  {0x4E, 0xFF, 0x1B},
  {0x4F, 0xFF, 0x1E},
  {0x50, 0xFF, 0x16},
  {0x51, 0xFF, 0x1A},
  {0x52, 0xFF, 0x19},
  {0,    0,    0}
};

// DFE_RFLUT_DIV_IN_SWP settings mode PAL-I HIF
REG_CTRL_INFO_T MxL68x_PalI_RfLutSwpHIF[] =
{
  {0x4B, 0xFF, 0x16},
  {0x4C, 0xFF, 0x15},
  {0x4D, 0xFF, 0x14},
  {0x4E, 0xFF, 0x13},
  {0x4F, 0xFF, 0x1B},
  {0x50, 0xFF, 0x19},
  {0x51, 0xFF, 0x1A},
  {0x52, 0xFF, 0x17},
  {0,    0,    0}
};

// DFE_RFLUT_DIV_IN_SWP settings mode PAL-BG 8 MHz BW HIF
REG_CTRL_INFO_T MxL68x_PalBG_8MHzBW_RfLutSwpHIF[] =
{
  {0x4B, 0xFF, 0x1B},
  {0x4C, 0xFF, 0x1A},
  {0x4D, 0xFF, 0x19},
  {0x4E, 0xFF, 0x17},
  {0x4F, 0xFF, 0x16},
  {0x50, 0xFF, 0x15},
  {0x51, 0xFF, 0x14},
  {0x52, 0xFF, 0x13},
  {0,    0,    0}
};

// DFE_RFLUT_DIV_IN_SWP settings mode PAL-BG 7 MHz BW HIF
REG_CTRL_INFO_T MxL68x_PalBG_7MHzBW_RfLutSwpHIF[] =
{
  {0x4B, 0xFF, 0x1B},
  {0x4C, 0xFF, 0x1A},
  {0x4D, 0xFF, 0x15},
  {0x4E, 0xFF, 0x14},
  {0x4F, 0xFF, 0x13},
  {0x50, 0xFF, 0x16},
  {0x51, 0xFF, 0x17},
  {0x52, 0xFF, 0x19},
  {0,    0,    0}
};

// Analog PAL application mode setting
REG_CTRL_INFO_T MxL68x_AnalogPal[] =
{
  //{0x0C, 0xFF, 0x0C}, // Not need according to Pegasus
  {0x0F, 0xFF, 0x00},
  {0x13, 0xFF, 0x00},
  {0x53, 0xFF, 0x7E},
  {0x57, 0xFF, 0x91},
  {0x5C, 0xFF, 0xB1},
  {0x62, 0xFF, 0xC2},
  {0x6E, 0xFF, 0x01},
  {0x6F, 0xFF, 0x51},
  {0x87, 0xFF, 0x56},
  {0x88, 0xFF, 0x44},
  {0x93, 0xFF, 0x33},
  {0x97, 0xFF, 0xA3},
  {0x98, 0xFF, 0xA5},
  {0x9B, 0xFF, 0x20},
  {0x9C, 0xFF, 0x20},
  {0xA0, 0xFF, 0x00},
  {0xA5, 0xFF, 0x0F},
  {0xC2, 0xFF, 0xAB},
  {0xC5, 0xFF, 0x7C},
  {0xD5, 0xFF, 0x05},
  {0xD9, 0xFF, 0x00},
  {0xEA, 0xFF, 0x00},
  {0x49, 0xFF, 0x1D},
  {0x4A, 0xFF, 0x1C},
  {0xDD, 0xFF, 0x06},
  {0xDC, 0xFF, 0x1C},
  {0,    0,    0}
};

// Analog SECAM application mode setting
REG_CTRL_INFO_T MxL68x_AnalogSecam[] =
{
  //{0x0C, 0xFF, 0x0B}, // Not need according to Pegasus
  {0x0F, 0xFF, 0x00},
  {0x13, 0xFF, 0x00},
  {0x53, 0xFF, 0x7E},
  {0x57, 0xFF, 0x91},
  {0x5C, 0xFF, 0xB1},
  {0x62, 0xFF, 0xC2},
  {0x6E, 0xFF, 0x01},
  {0x6F, 0xFF, 0x51},
  {0x87, 0xFF, 0x56},
  {0x88, 0xFF, 0x44},
  {0x93, 0xFF, 0x33},
  {0x97, 0xFF, 0xA3},
  {0xBA, 0xFF, 0x40},
  {0x98, 0xFF, 0x95},
  {0x9B, 0xFF, 0x66},
  {0x9C, 0xFF, 0x1F},
  {0xA0, 0xFF, 0x10},
  {0xA5, 0xFF, 0x0F},
  {0xC2, 0xFF, 0xA8},
  {0xC5, 0xFF, 0xBD},
  {0xD5, 0xFF, 0x05},
  {0xD9, 0xFF, 0x00},
  {0xEA, 0xFF, 0x00},
  {0x49, 0xFF, 0x1D},
  {0x4A, 0xFF, 0x1C},
  {0x4B, 0xFF, 0x1B},
  {0x4C, 0xFF, 0x1A},
  {0x4D, 0xFF, 0x19},
  {0x4E, 0xFF, 0x17},
  {0x4F, 0xFF, 0x16},
  {0x50, 0xFF, 0x15},
  {0x51, 0xFF, 0x14},
  {0x52, 0xFF, 0x13},
  {0xDD, 0xFF, 0x06},
  {0xDC, 0xFF, 0x1C},
  {0,    0,    0}
};

// Digital DVB-C application mode setting
REG_CTRL_INFO_T MxL68x_DigitalDvbc[] =
{
  {0x0F, 0xFF, 0x20},
  {0x0C, 0xFF, 0x00},
  {0x13, 0xFF, 0x04},
  {0x53, 0xFF, 0x7E},
  {0x57, 0xFF, 0x91},
  {0x5C, 0xFF, 0xB1},
  {0x62, 0xFF, 0xF2},
  {0x6E, 0xFF, 0x03},
  {0x6F, 0xFF, 0xD1},
  {0x87, 0xFF, 0x77},
  {0x88, 0xFF, 0x55},
  {0x93, 0xFF, 0x33},
  {0x97, 0xFF, 0x03},
  {0xBA, 0xFF, 0x40},
  {0x98, 0xFF, 0xAF},
  {0x9B, 0xFF, 0x20},
  {0x9C, 0xFF, 0x1E},
  {0xA0, 0xFF, 0x18},
  {0xA5, 0xFF, 0x09},
  {0xC2, 0xFF, 0xA9},
  {0xC5, 0xFF, 0x7C},
  {0xD5, 0xFF, 0x05},
  {0xEA, 0xFF, 0x00},
  {0xDC, 0xFF, 0x1C},
  {0,    0,    0}
};

// Digital ISDBT & ATSC application mode setting
REG_CTRL_INFO_T MxL68x_DigitalIsdbtAtsc[] =
{
  {0x0F, 0xFF, 0x20},
  {0x0C, 0xFF, 0x00},
  {0x13, 0xFF, 0x04},
  {0x53, 0xFF, 0xFE},
  {0x57, 0xFF, 0x90},
  {0x62, 0xFF, 0xC2},
  {0x6E, 0xFF, 0x01},
  {0x6F, 0xFF, 0x51},
  {0x87, 0xFF, 0x77},
#ifdef CUSTOMER_SPECIFIC_SETTING_1
  {0x88, 0xFF, 0x77},
#else
  {0x88, 0xFF, 0x55},
#endif
  {0x93, 0xFF, 0x22},
  {0x97, 0xFF, 0x02},
  {0xBA, 0xFF, 0x30},
  {0x98, 0xFF, 0xAF},
  {0x9B, 0xFF, 0x20},
  {0x9C, 0xFF, 0x1E},
  {0xA0, 0xFF, 0x18},
  {0xA5, 0xFF, 0x09},
  {0xC2, 0xFF, 0xA9},
  {0xC5, 0xFF, 0x7C},
  {0xD5, 0xFF, 0x03},
  {0xD9, 0xFF, 0x04},
  {0,    0,    0}
};

// Digital DVB-T 6MHz application mode setting
REG_CTRL_INFO_T MxL68x_DigitalDvbt[] =
{
  {0x0F, 0xFF, 0x20},
  {0x13, 0xFF, 0x04},
  {0x0C, 0xFF, 0x00},
  {0x53, 0xFF, 0xFE},
  {0x57, 0xFF, 0x90},
  {0x62, 0xFF, 0xC2},
  {0x6E, 0xFF, 0x01},
  {0x6F, 0xFF, 0x51},
  {0x87, 0xFF, 0x77},
  {0x88, 0xFF, 0x55},
  {0x93, 0xFF, 0x22},
  {0x97, 0xFF, 0x02},
  {0xBA, 0xFF, 0x30},
  {0x98, 0xFF, 0xAF},
  {0x9B, 0xFF, 0x20},
  {0x9C, 0xFF, 0x1E},
  {0xA0, 0xFF, 0x18},
  {0xA5, 0xFF, 0x09},
  {0xC2, 0xFF, 0xA9},
  {0xC5, 0xFF, 0x7C},
  {0xD5, 0xFF, 0x03},
  {0xD9, 0xFF, 0x04},
  {0,    0,    0}
};



// Table : 24
// NTSC XTAL=16MHz LIF
MXL68x_CHAN_DEP_FREQ_TABLE_T MXL68x_NTSC_LUT_XTAL_16MHZ_LIF[] =
{
  {1, 0x1C, 0x17, 0x00, 0xD8},
  { 57000000, 0x19, 0x39, 0x00, 0xD8},
  {111000000, 0x1C, 0x17, 0x20, 0xD7},
  {177000000, 0x1C, 0x17, 0x20, 0xDB},
  {201000000, 0x1C, 0x17, 0x20, 0xDC},
  {219000000, 0x1C, 0x17, 0x20, 0xD7},
  {285000000, 0x1F, 0x3F, 0x20, 0xD7},
  {363000000, 0x1B, 0x3B, 0x20, 0xD8},
  {651000000, 0x1C, 0x17, 0x20, 0xD4},
  {663000000, 0x1C, 0x17, 0x20, 0xD7},
  {675000000, 0x1C, 0x17, 0x20, 0xD5},
  {705000000, 0x1C, 0x17, 0x20, 0xD5},
  {813000000, 0x1C, 0x17, 0x20, 0xD5},
  {861000000, 0x1C, 0x17, 0x20, 0xD5},
  {867000000, 0x1C, 0x17, 0x20, 0xD7},
  {879000000, 0x1C, 0x17, 0x20, 0xD3},
  {0, 0, 0, 0, 0},
};

// Table : 25
// NTSC XTAL=24MHz
MXL68x_CHAN_DEP_FREQ_TABLE_T MXL68x_NTSC_LUT_XTAL_24MHZ[] =
{
  {1, 0x1D, 0x17, 0x00, 0x90},
  {123000000, 0x19, 0x39, 0x20, 0x8D},
  {177000000, 0x1F, 0x3F, 0x20, 0x90},
  {231000000, 0x1D, 0x3D, 0x20, 0x90},
  {255000000, 0x19, 0x39, 0x20, 0x90},
  {699000000, 0x1A, 0x3A, 0x20, 0x90},
  {705000000, 0x19, 0x39, 0x20, 0x8F},
  {0, 0, 0, 0, 0},
};

// Table : 26
// NTSC HRC XTAL=16MHz
MXL68x_CHAN_DEP_FREQ_TABLE_T MXL68x_NTSC_LUT_HRC_16MHZ[] =
{
  {1, 0x1D, 0x17, 0x00, 0xD8},
  {55750000, 0x19, 0x39, 0x20, 0xD8},
  {283765625, 0x1E, 0x3E, 0x20, 0xD8},
  {649781250, 0x1A, 0x3A, 0x20, 0xD8},
  {883794100, 0x1C, 0x17, 0x20, 0xD7},
  {0, 0, 0, 0, 0},
};

// Table : 27
// NTSC HRC XTAL=24MHz
MXL68x_CHAN_DEP_FREQ_TABLE_T MXL68x_NTSC_LUT_HRC_24MHZ[] =
{
  {1, 0x1D, 0x17, 0x00, 0x90},
  {271765625, 0x1E, 0x3E, 0x20, 0x8F},
  {283765625, 0x1E, 0x3E, 0x20, 0x90},
  {0, 0, 0, 0, 0},
};

// Table : 28
// NTSC IRC XTAL=16MHz
MXL68x_CHAN_DEP_FREQ_TABLE_T MXL68x_NTSC_LUT_IRC_16MHZ[] =
{
  {1, 0x1C, 0x17, 0x00, 0xD8},
  {111012500, 0x1C, 0x17, 0x20, 0xD7},
  {177012500, 0x1F, 0x3F, 0x20, 0xD8},
  {219012500, 0x1C, 0x17, 0x20, 0xD7},
  {285012500, 0x1F, 0x3F, 0x20, 0xD8},
  {363012500, 0x1B, 0x3B, 0x20, 0xD8},
  {663012500, 0x1C, 0x17, 0x20, 0xD7},
  {759012500, 0x1C, 0x17, 0x20, 0xD6},
  {813012500, 0x1C, 0x17, 0x20, 0xD5},
  {867012500, 0x1C, 0x17, 0x20, 0xD7},
  {0, 0, 0, 0, 0},
};

// Table : 29
// NTSC IRC XTAL=24MHz
MXL68x_CHAN_DEP_FREQ_TABLE_T MXL68x_NTSC_LUT_IRC_24MHZ[] =
{
  {1, 0x1D, 0x17, 0x00, 0x90},
  {123015625, 0x19, 0x39, 0x20, 0x8D},
  {231015625, 0x15, 0x35, 0x20, 0x8E},
  {0, 0, 0, 0, 0},
};

// Table : new, updated on May.31
// PAL-MN, either 16MHz or 24MHz
MXL68x_CHAN_DEP_FREQ_TABLE_T MXL68x_PAL_MN_LUT[] =
{
  {1, 0x1C, 0x17, 0x00, 0xD8},
  { 63143000, 0x1C, 0x17, 0x20, 0xD7},
  {177000000, 0x1C, 0x17, 0x20, 0xDB},
  {405143000, 0x1C, 0x17, 0x20, 0xD5},
  {621143000, 0x1C, 0x17, 0x20, 0xD9},
  {0, 0, 0, 0, 0},
};

// Table : 31
// PAL-B/G 7MHz XTAL=16MHz LIF
MXL68x_CHAN_DEP_FREQ_TABLE_T MXL68x_PAL_BG_7MHZ_LUT_XTAL_16MHZ_LIF[] =
{
  {1, 0x1D, 0x17, 0x00, 0xD8},
  { 50500000, 0x1C, 0x17, 0x20, 0xDB},
  { 57500000, 0x1C, 0x17, 0x20, 0xD5},
  {114500000, 0x1C, 0x17, 0x20, 0xDB},
  {219500000, 0x1C, 0x17, 0x20, 0xD7},
  {0, 0, 0, 0, 0},
};

// Table : 32
// PAL-B/G 7MHz XTAL=24MHz
MXL68x_CHAN_DEP_FREQ_TABLE_T MXL68x_PAL_BG_7MHZ_LUT_XTAL_24MHZ[] =
{
  {1, 0x1D, 0x17, 0x00, 0x90},
  {128500000, 0x1F, 0x3F, 0x20, 0x91},
  {149500000, 0x1B, 0x3B, 0x20, 0x8F},
  {0, 0, 0, 0, 0},
};

// Table : 33
// PAL-B/G 8MHz XTAL=16MHz LIF
MXL68x_CHAN_DEP_FREQ_TABLE_T MXL68x_PAL_BG_8MHZ_LUT_XTAL_16MHZ_LIF[] =
{
  {1, 0x1D, 0x17, 0x00, 0xD8},
  {378000000, 0x1A, 0x3A, 0x20, 0xD5},
  {418000000, 0x1E, 0x3E, 0x20, 0xD4},
  {594000000, 0x19, 0x39, 0x20, 0xDA},
  {786000000, 0x1C, 0x17, 0x20, 0xD5},
  {834000000, 0x1A, 0x3A, 0x20, 0xD5},
  {0, 0, 0, 0, 0},
};

// Table : 34
// PAL-B/G 8MHz XTAL=24MHz
MXL68x_CHAN_DEP_FREQ_TABLE_T MXL68x_PAL_BG_8MHZ_LUT_XTAL_24MHZ[] =
{
  {1, 0x1D, 0x17, 0x00, 0x90},
  {314000000, 0x1F, 0x3F, 0x20, 0x93},
  {378000000, 0x1E, 0x3E, 0x20, 0x91},
  {386000000, 0x18, 0x38, 0x20, 0x8C},
  {418000000, 0x1B, 0x3B, 0x20, 0x91},
  {450000000, 0x1C, 0x3C, 0x20, 0x8E},
  {602000000, 0x19, 0x39, 0x20, 0x8F},
  {0, 0, 0, 0, 0},
};

// Table : 35
// PAL-D XTAL=16MHz LIF
MXL68x_CHAN_DEP_FREQ_TABLE_T MXL68x_PAL_D_LUT_XTAL_16MHZ_LIF[] =
{
  {1, 0x1D, 0x17, 0x00, 0xD8},
  { 65000000, 0x1D, 0x17, 0x20, 0xDE},
  {116000000, 0x1D, 0x17, 0x20, 0xD6},
  {227000000, 0x1A, 0x3A, 0x20, 0xDA},
  {243000000, 0x1D, 0x17, 0x00, 0xD8},
  {275000000, 0x1D, 0x17, 0x20, 0xD9},
  {365000000, 0x1D, 0x17, 0x20, 0xD7},
  {898000000, 0x1D, 0x3D, 0x20, 0xD5},
  {0, 0, 0, 0, 0},
};

// Table : 36
// PAL-D XTAL=24MHz
MXL68x_CHAN_DEP_FREQ_TABLE_T MXL68x_PAL_D_LUT_XTAL_24MHZ[] =
{
  {1, 0x1D, 0x17, 0x00, 0x90},
  {147000000, 0x1A, 0x3A, 0x20, 0x8E},
  {259000000, 0x1B, 0x3B, 0x20, 0x8D},
  {275000000, 0x1A, 0x3A, 0x20, 0x8E},
  {602000000, 0x1C, 0x3C, 0x20, 0x8E},
  {834000000, 0x1B, 0x3B, 0x20, 0x8E},
  {898000000, 0x1D, 0x3D, 0x20, 0x8E},
  {0, 0, 0, 0, 0},
};

// Table : 37
// PAL-I XTAL=16MHz LIF
MXL68x_CHAN_DEP_FREQ_TABLE_T MXL68x_PAL_I_LUT_XTAL_16MHZ_LIF[] =
{
  {1, 0x1D, 0x17, 0x00, 0xD8},
  {55000000, 0x1D, 0x17, 0x20, 0xCB},
  {80500000, 0x1A, 0x3A, 0x20, 0xE3},
  {186000000, 0x1D, 0x17, 0x0, 0xD8},
  {418000000, 0x1E, 0x3E, 0x20, 0xD5},
  {594000000, 0x19, 0x39, 0x20, 0xDA},
  {874000000, 0x1D, 0x17, 0x20, 0xD6},
  {898000000, 0x1D, 0x3D, 0x20, 0xD5},
  {0, 0, 0, 0, 0},
};

// Table : 38
// PAL-I XTAL=24MHz
MXL68x_CHAN_DEP_FREQ_TABLE_T MXL68x_PAL_I_LUT_XTAL_24MHZ[] =
{
  {1, 0x1D, 0x17, 0x00, 0x90},
  {152500000, 0x19, 0x39, 0x20, 0x90},
  {418000000, 0x1B, 0x3B, 0x20, 0x91},
  {458000000, 0x1A, 0x3A, 0x20, 0x90},
  {898000000, 0x1D, 0x3D, 0x20, 0x8E},
  {0, 0, 0, 0, 0},
};

// Table : 39
// SECAM L XTAL=16MHz
MXL68x_CHAN_DEP_FREQ_TABLE_T MXL68x_SECAM_L_LUT_XTAL_16MHZ[] =
{
  {1, 0x1D, 0x17, 0x00, 0xD8},
  {242750000, 0x1D, 0x17, 0x0, 0xD8},
  {346750000, 0x1B, 0x3B, 0x20, 0xD9},
  {0, 0, 0, 0, 0},
};

// Table : 40
// SECAM-L XTAL=24MHz
MXL68x_CHAN_DEP_FREQ_TABLE_T MXL68x_SECAM_L_LUT_XTAL_24MHZ[] =
{
  {1, 0x1D, 0x17, 0x00, 0x90},
  {226750000, 0x19, 0x39, 0x20, 0x8F},
  {346750000, 0x1D, 0x3D, 0x20, 0x8F},
  {0, 0, 0, 0, 0},
};

// Table : 50
// PAL-D XTAL=16MHz HIF
MXL68x_CHAN_DEP_FREQ_TABLE_T MXL68x_PAL_D_LUT_XTAL_16MHZ_HIF[] =
{
  {1, 0x1D, 0x17, 0x00, 0xD8},
  {171000000, 0x1D, 0x17, 0x20, 0xD9},
  {179000000, 0x1D, 0x17, 0x20, 0xD3},
  {187000000, 0x1D, 0x17, 0x20, 0xDD},
  {227000000, 0x1D, 0x17, 0x20, 0xDB},
  {315000000, 0x1D, 0x17, 0x20, 0xD7},
  {339000000, 0x1D, 0x17, 0x20, 0xD5},
  {363000000, 0x1D, 0x17, 0x20, 0xDB},
  {387000000, 0x1D, 0x17, 0x20, 0xD6},
  {443000000, 0x1D, 0x17, 0x20, 0xDA},
  {522000000, 0x1D, 0x17, 0x20, 0xD7},
  {666000000, 0x1D, 0x17, 0x20, 0xD7},
  {738000000, 0x1D, 0x17, 0x20, 0xD7},
  {834000000, 0x1D, 0x17, 0x20, 0xDA},
  {0, 0, 0, 0, 0},
};

// Table : 51
// PAL-I XTAL=16MHz HIF
MXL68x_CHAN_DEP_FREQ_TABLE_T MXL68x_PAL_I_LUT_XTAL_16MHZ_HIF[] =
{
  {1, 0x1D, 0x17, 0x00, 0xD8},
  {48500000, 0x1D, 0x17, 0x20, 0xD7},
  {55000000, 0x1D, 0x17, 0x20, 0xC9},
  {64500000, 0x1D, 0x17, 0x20, 0xDE},
  {96500000, 0x1D, 0x17, 0x20, 0xD1},
  {120500000, 0x1D, 0x17, 0x20, 0xE0},
  {144500000, 0x1D, 0x17, 0x20, 0xDA},
  {178000000, 0x1D, 0x17, 0x20, 0xD6},
  {186000000, 0x1D, 0x17, 0x20, 0xDC},
  {226000000, 0x1D, 0x17, 0x20, 0xDB},
  {234000000, 0x1D, 0x17, 0x20, 0xD7},
  {306000000, 0x1D, 0x17, 0x20, 0xD7},
  {394000000, 0x1D, 0x17, 0x20, 0xD9},
  {410000000, 0x1D, 0x17, 0x20, 0xD7},
  {450000000, 0x1D, 0x17, 0x20, 0xD6},
  {522000000, 0x1D, 0x17, 0x20, 0xD7},
  {666000000, 0x1D, 0x17, 0x20, 0xD7},
  {738000000, 0x1D, 0x17, 0x20, 0xD7},
  {874000000, 0x1D, 0x17, 0x20, 0xD7},
  {882000000, 0x1D, 0x17, 0x20, 0xD7},
  {0, 0, 0, 0, 0},
};

// Table : 52
// PAL-BG XTAL=16MHz HIF
MXL68x_CHAN_DEP_FREQ_TABLE_T MXL68x_PAL_BG_7MHZ_LUT_XTAL_16MHZ_HIF[] =
{
  {1, 0x1D, 0x17, 0x00, 0xD8},
  {57500000, 0x1D, 0x17, 0x20, 0xD7},
  {191500000, 0x1D, 0x17, 0x20, 0xD5},
  {219500000, 0x1D, 0x17, 0x20, 0xD7},
  {240500000, 0x1D, 0x17, 0x20, 0xDB},
  {282500000, 0x1D, 0x17, 0x20, 0xDC},
  {296500000, 0x1D, 0x17, 0x20, 0xDC},
  {0, 0, 0, 0, 0},
};

// Table : 53
// PAL-BG XTAL=16MHz HIF
MXL68x_CHAN_DEP_FREQ_TABLE_T MXL68x_PAL_BG_8MHZ_LUT_XTAL_16MHZ_HIF[] =
{
  {1, 0x1D, 0x17, 0x00, 0xD8},
  {306000000, 0x1D, 0x17, 0x20, 0xD9},
  {314000000, 0x1D, 0x17, 0x20, 0xD6},
  {450000000, 0x1D, 0x17, 0x20, 0xD7},
  {522000000, 0x1D, 0x17, 0x20, 0xD7},
  {666000000, 0x1D, 0x17, 0x20, 0xD7},
  {730000000, 0x1D, 0x17, 0x20, 0xD3},
  {738000000, 0x1D, 0x17, 0x20, 0xD7},
  {0, 0, 0, 0, 0},
};

// Table : 54
// NTSC XTAL=16MHz HIF
MXL68x_CHAN_DEP_FREQ_TABLE_T MXL68x_NTSC_LUT_XTAL_16MHZ_HIF[] =
{
  {1, 0x1C, 0x17, 0x00, 0xD8},
  {99000000, 0x1C, 0x17, 0x20, 0xD7},
  {111000000, 0x1C, 0x17, 0x20, 0xD6},
  {219000000, 0x1C, 0x17, 0x20, 0xD7},
  {261000000, 0x1C, 0x17, 0x20, 0xD9},
  {333000000, 0x1C, 0x17, 0x20, 0xD9},
  {363000000, 0x1C, 0x17, 0x20, 0xDB},
  {387000000, 0x1C, 0x17, 0x20, 0xD9},
  {441000000, 0x1C, 0x17, 0x20, 0xD9},
  {447000000, 0x1C, 0x17, 0x20, 0xD6},
  {495000000, 0x1C, 0x17, 0x20, 0xD7},
  {549000000, 0x1C, 0x17, 0x20, 0xD7},
  {603000000, 0x1C, 0x17, 0x20, 0xD7},
  {663000000, 0x1C, 0x17, 0x20, 0xD6},
  {705000000, 0x1C, 0x17, 0x20, 0xD7},
  {735000000, 0x1C, 0x17, 0x20, 0xD7},
  {867000000, 0x1C, 0x17, 0x20, 0xD7},
  {0, 0, 0, 0, 0},
};

// Table : 60
// Digital Cable 6MHz
MXL68x_CHAN_DEP_FREQ_TABLE_T MXL68x_DIG_CABLE_LUT_BW_6MHZ[] =
{
  {1, 0x1C, 0x17, 0x00, 0xD8},
  {0, 0, 0, 0, 0},
};

// Table : 61
// Digital Cable 8MHz
MXL68x_CHAN_DEP_FREQ_TABLE_T MXL68x_DIG_CABLE_LUT_BW_8MHZ[] =
{
  {1, 0x1C, 0x17, 0x00, 0xD8},
  {0, 0, 0, 0, 0},
};

// Table : 62
// Digital Terestrial 6MHz
MXL68x_CHAN_DEP_FREQ_TABLE_T MXL68x_DIG_TERR_LUT_BW_6MHZ[] =
{
  {1, 0x1C, 0x17, 0x00, 0xD8},
  {0, 0, 0, 0, 0},
};

// Table : 63
// Digital Terestrial 7MHz
MXL68x_CHAN_DEP_FREQ_TABLE_T MXL68x_DIG_TERR_LUT_BW_7MHZ[] =
{
  {1, 0x1C, 0x17, 0x00, 0xD8},
  {0, 0, 0, 0, 0},
};

// Table : 64
// Digital Terestrial 8MHz
MXL68x_CHAN_DEP_FREQ_TABLE_T MXL68x_DIG_TERR_LUT_BW_8MHZ[] =
{
  {1, 0x1C, 0x17, 0x00, 0xD8},
  {0, 0, 0, 0, 0},
};




PMXL68x_DEV_CONTEXT_T MxL68x_Ctrl_GetDeviceContext(int devId)
{
	return (devId < MXL68X_MAX_DEVICES) ? (&MxL68x_Dev[devId]) : NULL;
}


/*******************************************************************************
* function: MXL683_I2C_Read
* input:
*	- nChannelId : 버스 번호
* output: No
* return: No
*******************************************************************************/
int MXL683_I2C_Read(int nChannelId , HUINT16 ucRegAddr, HUINT16 *pucBuffer)
{
	int nResult = DI_CH_OK;
	HUINT8 read_buf[4];
	HUINT32	I2C_Channel;
	HUINT16	I2C_Addr;
	HUINT16 pageNumber;
	HINT32	nUnitId;

	nUnitId = GetDeviceUnitId( nChannelId );

	PMXL68x_DEV_CONTEXT_T devContextPtr = MxL68x_Ctrl_GetDeviceContext(nUnitId);
#if defined(CONFIG_I2C_MAP_FOR_2TUNER)
	I2C_Channel = I2C_CHANNEL_CHANNEL2;
	I2C_Addr= MXL683_I2C_ADDRESS1;
#else
	if (nUnitId == 0)
	{
		I2C_Channel = I2C_CHANNEL_CHANNEL;
		I2C_Addr= MXL683_I2C_ADDRESS1;
	}
	else if (nUnitId == 1)
	{
		I2C_Channel = I2C_CHANNEL_CHANNEL2;
		I2C_Addr= MXL683_I2C_ADDRESS2;
	}
	else if (nUnitId == 2)
	{
		I2C_Channel = I2C_CHANNEL_CHANNEL3;
		I2C_Addr= MXL683_I2C_ADDRESS3;
	}
	else
		return DI_CH_ERR;
#endif

	if (ucRegAddr < GINJO_DFE_REGISTER_BOUNDARY)
	{
		if (devContextPtr->firmwareStatus != MXL68x_FW_STATUS_LOADED)
		{
			// Bit<9> and Bit<8> for page number
			pageNumber = (ucRegAddr & GINJO_DFE_REG_PAGE_MASK) >> GINJO_DFE_REG_WIDTH;

			// change to respective register page bank
			read_buf[0] = (HUINT8)(pageNumber >> 8);
			read_buf[1] = (HUINT8)(pageNumber & 0x00FF);
			nResult |= DRV_I2c_WriteA16(I2C_Channel, I2C_Addr, DFE_PAGE_CTRL_REG, read_buf,2);

			ucRegAddr &= GINJO_DFE_REG_ADDR_MASK;
		}
		read_buf[0] = 0xff;
		read_buf[1] = 0xfb;
		read_buf[2] =  (HUINT8)(ucRegAddr >> 8);
		read_buf[3] =  (HUINT8)(ucRegAddr & 0x00FF);
		nResult = DRV_I2c_WriteNoAddr( I2C_Channel, I2C_Addr, read_buf, 4 );
		VK_TASK_Sleep(1);
		nResult = DRV_I2c_ReadNoAddr( I2C_Channel, I2C_Addr, read_buf, 2 );


	}
	else
	{

		read_buf[0] = 0xff;
		read_buf[1] = 0xfb;
		read_buf[2] =  (HUINT8)(ucRegAddr >> 8);
		read_buf[3] =  (HUINT8)(ucRegAddr & 0x00FF);
		//CH_DI_Print(0, (" MXL683...111 I2C_read_buf[2] = 0x%x, read_buf[3] = 0x%x  \n", read_buf[2], read_buf[3]));
		nResult = DRV_I2c_WriteNoAddr( I2C_Channel, I2C_Addr, read_buf, 4 );
		VK_TASK_Sleep(1);
		nResult = DRV_I2c_ReadNoAddr( I2C_Channel, I2C_Addr, read_buf, 2 );
	}

	if ( nResult != DI_CH_OK )
	{
		CH_DI_Print(CH_LOG_0, (" MXL683...I2C_Read...Error[0x%x]  \n", nResult));
		return nResult;
	}
	else
	{
		//CH_DI_Print(0, (" MXL683...I2C_read_buf[0] = 0x%x, read_buf[1] = 0x%x  \n", read_buf[0], read_buf[1]));
		*pucBuffer = (HUINT16)((read_buf[0]<<8)|read_buf[1]);
		return nResult;
	}



}
int MXL683_I2C_Write(int nChannelId,  HUINT16 ucRegAddr, HUINT16 pucBuffer)
{
	int nResult = DI_CH_OK;
	HUINT8	write_buf[4];
	HUINT32	I2C_Channel;
	HUINT16	I2C_Addr;
	HUINT16 pageNumber;
	HINT32	nUnitId;

	nUnitId = GetDeviceUnitId( nChannelId );

	PMXL68x_DEV_CONTEXT_T devContextPtr = MxL68x_Ctrl_GetDeviceContext(nUnitId);
#if defined(CONFIG_I2C_MAP_FOR_2TUNER)
		I2C_Channel = I2C_CHANNEL_CHANNEL2;
		I2C_Addr= MXL683_I2C_ADDRESS1;
#else

	if (nUnitId == 0)
	{
		I2C_Channel = I2C_CHANNEL_CHANNEL;
		I2C_Addr= MXL683_I2C_ADDRESS1;
	}
	else if (nUnitId == 1)
	{
		I2C_Channel = I2C_CHANNEL_CHANNEL2;
		I2C_Addr= MXL683_I2C_ADDRESS2;
	}
	else if (nUnitId == 2)
	{
		I2C_Channel = I2C_CHANNEL_CHANNEL3;
		I2C_Addr= MXL683_I2C_ADDRESS3;
	}
	else
		return DI_CH_ERR;
#endif

	if (ucRegAddr < GINJO_DFE_REGISTER_BOUNDARY)
	{
		if (devContextPtr->firmwareStatus != MXL68x_FW_STATUS_LOADED)
		{
			// Bit<9> and Bit<8> for page number
			pageNumber = (ucRegAddr & GINJO_DFE_REG_PAGE_MASK) >> GINJO_DFE_REG_WIDTH;

			// change to respective register page bank
			write_buf[0] = (HUINT8)(pageNumber >> 8);
			write_buf[1] = (HUINT8)(pageNumber & 0x00FF);
			nResult |= DRV_I2c_WriteA16(I2C_Channel, I2C_Addr, DFE_PAGE_CTRL_REG, write_buf,2);
			ucRegAddr = ucRegAddr & GINJO_DFE_REG_ADDR_MASK; // Bit<7:0> total 8 bit
		}
		write_buf[0] = (HUINT8)(pucBuffer >> 8);
		write_buf[1] = (HUINT8)(pucBuffer & 0x00FF);
		nResult |= DRV_I2c_WriteA16(I2C_Channel, I2C_Addr, ucRegAddr, write_buf,2);
	}
	else
	{
		write_buf[0] = (HUINT8)(pucBuffer >> 8);
		write_buf[1] = (HUINT8)(pucBuffer & 0x00FF);
		//CH_DI_Print(0, (" MXL683...MXL683_I2C_Write...writebuf[0] = 0x%x, writebuf[1] = 0x%x  \n", write_buf[0], write_buf[1]));
		nResult = DRV_I2c_WriteA16( I2C_Channel, I2C_Addr, ucRegAddr, write_buf, 2 );
	}

	if ( nResult != DI_CH_OK )
	{
		CH_DI_Print(CH_LOG_0, (" MXL683...I2C_Write...Error[0x%x]  \n", nResult));
		return nResult;
	}
	else
	{
		return nResult;
	}

}

int MXL683_I2C_ReadBlock(int nChannelId,  HUINT16 ucRegAddr, HUINT16 length, HUINT8 *pucBuffer)
{
	int nResult;
	HUINT32	I2C_Channel;
	HUINT16	I2C_Addr;
	HUINT8 readBuffer[4];
	HINT32	nUnitId;

	nUnitId = GetDeviceUnitId( nChannelId );

#if defined(CONFIG_I2C_MAP_FOR_2TUNER)
	I2C_Channel = I2C_CHANNEL_CHANNEL2;
	I2C_Addr= MXL683_I2C_ADDRESS1;
#else
	if (nUnitId == 0)
	{
		I2C_Channel = I2C_CHANNEL_CHANNEL;
		I2C_Addr= MXL683_I2C_ADDRESS1;
	}
	else if (nUnitId == 1)
	{
		I2C_Channel = I2C_CHANNEL_CHANNEL2;
		I2C_Addr= MXL683_I2C_ADDRESS2;
	}
	else if (nUnitId == 2)
	{
		I2C_Channel = I2C_CHANNEL_CHANNEL3;
		I2C_Addr= MXL683_I2C_ADDRESS3;
	}
	else
		return DI_CH_ERR;
#endif

	readBuffer[0] = 0xff;
	readBuffer[1] = 0xfd;
	readBuffer[2] =  (HUINT8)(ucRegAddr >> 8);
	readBuffer[3] =  (HUINT8)(ucRegAddr & 0x00FF);
	//CH_DI_Print(0, (" MXL683...111 MXL683_I2C_ReadBlock ...read_buf= 0x%x, read_buf[3] = 0x%x  \n", readBuffer[2], readBuffer[3]));
	nResult = DRV_I2c_WriteNoAddr( I2C_Channel, I2C_Addr, readBuffer, 4 );

	VK_TASK_Sleep(1);
	nResult = DRV_I2c_ReadNoAddr( I2C_Channel, I2C_Addr, pucBuffer, length );


	if ( nResult != DI_CH_OK )
	{
		CH_DI_Print(CH_LOG_0, (" MXL683...MXL683_I2C_ReadBlock...Error[0x%x]  \n", nResult));
		return nResult;
	}
	else
	{
		return nResult;
	}
}

 int MXL683_I2C_WriteBlock(int nChannelId,  HUINT16 ucRegAddr, HUINT16 length, HUINT8 *pucBuffer)
{
	 int nResult = DI_CH_OK;
	 HUINT32 I2C_Channel;
	 HUINT16 I2C_Addr;
	HINT32	nUnitId;

	nUnitId = GetDeviceUnitId( nChannelId );
#if defined(CONFIG_I2C_MAP_FOR_2TUNER)
		I2C_Channel = I2C_CHANNEL_CHANNEL2;
		I2C_Addr= MXL683_I2C_ADDRESS1;
#else
	 if (nUnitId == 0)
	 {
		 I2C_Channel = I2C_CHANNEL_CHANNEL;
		 I2C_Addr= MXL683_I2C_ADDRESS1;
	 }
	 else if (nUnitId == 1)
	 {
		 I2C_Channel = I2C_CHANNEL_CHANNEL2;
		 I2C_Addr= MXL683_I2C_ADDRESS2;
	 }
	 else if (nUnitId == 2)
	 {
		 I2C_Channel = I2C_CHANNEL_CHANNEL3;
		 I2C_Addr= MXL683_I2C_ADDRESS3;
	 }
	 else
		 return DI_CH_ERR;
#endif

	 nResult = DRV_I2c_WriteA16( I2C_Channel, I2C_Addr, ucRegAddr, pucBuffer, length );
	 if ( nResult != DI_CH_OK )
	 {
		 CH_DI_Print(CH_LOG_0, (" MXL683...MXL683_I2C_WriteBlock...Error[0x%x]  \n", nResult));
		 return nResult;
	 }
	 else
	 {
		 return nResult;
	 }
}

 //JAMES
 int MXL683_DumpRegister(int nChannelId)
 {
	 HUINT8 status = MXL_SUCCESS;
	 HUINT16 index, regData;

	 for (index = 0x1000; index < 0x11D4; index++)
	 {
		 status |= MXL683_I2C_Read(nChannelId, index, &regData);
		 CH_DI_Print(CH_LOG_0, ("[MXL683] MXL683_DumpRegister()  Addr = %x, Data = %x \n",index, regData));
	 }

	 CH_DI_Print(CH_LOG_0, ("[MXL683] MXL683_DumpRegister()  status = %d! \n",status));
	 return status;
 }

 int MXL683_READ(int nChannelId, int nAddr)
 {
	 HUINT8 status = MXL_SUCCESS;
	 HUINT16 regAddr = (HUINT16)nAddr;
	 HUINT16 regData = 0;


	 status |= MXL683_I2C_Read(nChannelId, regAddr, &regData);
	 CH_DI_Print(CH_LOG_0, ("[MXL683] MXL683_READ()  Addr = %x, regData = %x \n",regAddr, regData));
	 CH_DI_Print(CH_LOG_0, ("[MXL683] MXL683_READ()  Addr = %x, regData = %x \n",regAddr, regData));
	 CH_DI_Print(CH_LOG_0, ("[MXL683] MXL683_READ()  Addr = %x, regData = %x \n",regAddr, regData));


	 CH_DI_Print(CH_LOG_0, ("[MXL683] MXL683_READ()  status = %d! \n",status));
	 return status;
 }

 int MXL683_WRITE(int nChannelId, int nAddr, int nData)
 {
	 HUINT8 status = MXL_SUCCESS;
	 HUINT16 regAddr = (HUINT16)nAddr;
	 HUINT16 regData = (HUINT16)nData;


	 status |= MXL683_I2C_Write(nChannelId, regAddr, regData);
	 CH_DI_Print(CH_LOG_0, ("[MXL683] MXL683_WRITE()  Addr = %x, regData = %x \n",regAddr, regData));
	 CH_DI_Print(CH_LOG_0, ("[MXL683] MXL683_WRITE()  Addr = %x, regData = %x \n",regAddr, regData));
	 CH_DI_Print(CH_LOG_0, ("[MXL683] MXL683_WRITE()  Addr = %x, regData = %x \n",regAddr, regData));

	 CH_DI_Print(CH_LOG_0, ("[MXL683] MXL683_WRITE()  status = %d! \n",status));
	 return status;
 }

 //JAMES...140731...Sometimes values of 0x101c, 0x1122 registers are changed to 0 for some reason
 //workaound is to insert checking routine
 int MXL683_Check_TS_OUTPUT(int nChannelId)
 {
	 HUINT8 status = MXL_SUCCESS;
	 HUINT16 _VAL_101C=0;
	 HUINT16 _VAL_1122=0;

	 status |= MXL683_I2C_Read(nChannelId, 0x101c, &_VAL_101C);
	 status |= MXL683_I2C_Read(nChannelId, 0x1122, &_VAL_1122);

	 if ( (_VAL_101C != 0x04) || (_VAL_1122 != 0x20) )
	 {
		 CH_DI_Print(CH_LOG_0, ("[MXL683]  JAMES...MXL683_Check_TS_OUTPUT()  status = %d... _VAL_101C  = %x, _VAL_1122 = %x \n",status,_VAL_101C,_VAL_1122));
		 status |= MXL683_I2C_Write(nChannelId, 0x101c, 0x0004);
		 status |= MXL683_I2C_Write(nChannelId, 0x1122, 0x0020);
	 }

	 CH_DI_Print(CH_LOG_4, ("[MXL683] MXL683_Check_TS_OUTPUT()	status = %d \n",status));
	 return status;
 }


 void DRV_T_InstallApi( void )
 {
	 TER_InitDevice 		 	= &MXL683_InitDevice;
	 TER_SetTune			 = &MXL683_SetTune;
	 TER_CheckLock			 = &MXL683_CheckLock;
	 TER_GetSignalInfo  		= &MXL683_GetSignalInfo;
	 TER_SetPowerState		 = &MXL683_SetPowerState;
	 TER_SetTsOutputMode 	= &MXL683_SetTsOutputMode;
	 TER_EnableTsOutput 	 = &MXL683_EnableTsOutput;
	 TER_DisableTsOutput	 = &MXL683_DisableTsOutput;
	 TER_CheckAntState	 	= &MXL683_CheckAntState;
	 TER_SetAntState	 	= &MXL683_SetAntState;
	 TER_GetAntState	 	= &MXL683_GetAntState;
	 TER_ProcessRecoverAntenna = &MXL683_ProcessRecoverAntenna;
	 TER_ResetGpio			 = &MXL683_ResetGpio;
	 TER_DumpRegister			 = &MXL683_DumpRegister; //JAMES
	 TER_READ			 = &MXL683_READ; //JAMES
	 TER_WRITE			 = &MXL683_WRITE; //JAMES

 	CH_DI_Print(CH_LOG_4, ("[MXL683] DRV_T_InstallApi()  OK! \n"));

	 return;
 }

HUINT8 Ctrl_HighBit(HUINT32 In)
{
	HUINT8 k,NonZeroP = 31;
	HUINT32 Mask = 0x80000000;

	for (k = 0; k < 32; k++)
	{
		if ((In & Mask) == 0x00)
		{
			Mask = Mask >> 1;
			NonZeroP = NonZeroP - 1;
		}
		else
			break;
	}
	return(NonZeroP);
}

MXL_STATUS_E Ctrl_SubMulDiv(HUINT16 b1,HUINT16 b2,HUINT16 b3,HINT16 p1,HINT16 p2,HINT16 p3,HUINT32 *RetVal)
{
	HINT16 p4;
	HUINT32 b4,b5;

	p4 = p1 + p2 - p3;

	if ((p4 >= 32) && (p3 < 0))
	{
		p4 = p1 + p2; // Because of p3 <0, hence force p3 = 0
		b5 = b3 >> (-p3);
	}
	else if ((p4 <= -32) && (p3 > 0))
	{
		p4 = p1 + p2; // Because of p3 >0, force p3 = 0
		b5 = (HUINT32)b3 << p3;
	}
	else
		b5 = b3;

	if (p4 > 0)
	{
		if (Ctrl_HighBit(b5) > p4)
		{
			b5 = (HUINT32)(b5 >> p4);
			*RetVal = (HUINT32)b1 * b2 / b5;
		}
		else
		{
			b4 = (HUINT32)b1 * b2 / b5;
			*RetVal = (HUINT32)(b4 << p4);
		}
	}
	else
	{
		b4 = (HUINT32)b1 * b2 / b5;
		p4 = - p4;
		// Round operation
		if ( Ctrl_HighBit(b4)+1 == p4 && p4 >1 )
			b4 = b4 + (b4 >> 1);

		*RetVal = (HUINT32)(b4 >> p4);
	}
	return(MXL_SUCCESS);
}


MXL_STATUS_E Ctrl_Decomp32Bit(HUINT32 In, HUINT16* b, HINT16* p)
{
	HUINT16 Bas;
	HINT16 Pwr;
	HUINT8 k,NonZeroP=31;
	HUINT32 Mask = 0x80000000;

	for(k=0;k<32;k++)
	{
		if ((In & Mask) == 0x00)
		{
			Mask = Mask >> 1;
			NonZeroP = NonZeroP - 1;
		}
		else
			break;
	}

	if (NonZeroP > 15)
	{
		Pwr = NonZeroP - 15;
		Bas = (HUINT16)((In >> Pwr) & 0x0000FFFF);
	}
	else if (NonZeroP <15)
	{
		Pwr =  NonZeroP -15;
		Bas = (HUINT16)((In << (15-NonZeroP)) & 0x0000FFFF);
	}
	else
	{
		Pwr = 0;
		Bas = (HUINT16)In;
	}

	* b = Bas;
	* p = Pwr;

	return(MXL_SUCCESS);
}

MXL_STATUS_E Ctrl_MulDiv32Bit(HUINT32 MulData1, HUINT32 MulData2, HUINT32 DivData, HUINT32* RtnVal)
{
	HUINT32 RetVal1,RetVal2,RetVal3;
	HUINT16 b1,b2,b3;
	HINT16 p1,p2,p3;
	HUINT16 r1=0,r2=0;

	Ctrl_Decomp32Bit(MulData1,&b1,&p1);
	if (p1 > 0) r1 = (HUINT16)MulData1 - (b1 << p1);

	Ctrl_Decomp32Bit(MulData2,&b2,&p2);
	if (p2 > 0) r2 = (HUINT16)MulData2 - (b2 << p2);

	Ctrl_Decomp32Bit(DivData,&b3,&p3);

	Ctrl_SubMulDiv(b1,b2,b3,p1,p2,p3,&RetVal1);
	Ctrl_SubMulDiv(b1,r2,b3,p1,0,p3,&RetVal2);
	Ctrl_SubMulDiv(r1,b2,b3,0,p2,p3,&RetVal3);

	*RtnVal = RetVal1 + RetVal2 + RetVal3;

	return (MXL_SUCCESS);
}

MXL_STATUS_E Ctrl_CheckIntegration(HUINT8 readCheckSum, HUINT8 readDataLen, HUINT8 *readDataPtr)
{
	HUINT16 sum = 0;
	HUINT8 len;

	if (readDataLen == 0)
	return(MXL_SUCCESS);  // Do not need to check integrate

	for (len = 0; len < readDataLen; len ++)
	sum += *readDataPtr++;

	if ((sum & 0xff) == readCheckSum)
	return(MXL_SUCCESS);  // Data is complete
	else
	return(MXL_FAILURE); // Data is not complete
}



HUINT32 Ctrl_GetBigEndian24(const HUINT8 buf[])
{
  return (((HUINT32)buf[0]) << 16) | (((HUINT32)buf[1]) << 8) | buf[2];
}

void Ctrl_SetBigEndian24(HUINT8 buf[], HUINT32 val)
{
  buf[2] = val&0xFF;  val >>= 8;
  buf[1] = val&0xFF;  val >>= 8;
  buf[0] = val&0xFF;

  return;
}

HUINT32 Ctrl_GetBigEndian32(const HUINT8 buf[])
{
  return (((HUINT32)buf[0]) << 24) | (((HUINT32)buf[1]) << 16) | (((HUINT32)buf[2]) << 8) | buf[3];
}

void Ctrl_SetBigEndian32(HUINT8 buf[], HUINT32 val)
{
	buf[3] = val&0xFF;  val >>= 8;
	buf[2] = val&0xFF;  val >>= 8;
	buf[1] = val&0xFF;  val >>= 8;
	buf[0] = val&0xFF;

	return;
}

MXL_STATUS_E Ctrl_CalCheckSum(HUINT8 inDataLen, HUINT8 *inDataPtr, HUINT8 *checkSum)
{
	HUINT16 sum = 0;

	if (inDataLen == 0)
	{
		*checkSum = 0;
		return(MXL_SUCCESS);
	}
	else
	{
		while (inDataLen--)
			sum += *inDataPtr++;

		*checkSum = (HUINT8)(sum & 0xFF);
	}

	return(MXL_SUCCESS);
}


MXL_STATUS_E Ctrl_DownloadFirmwareSegment(HUINT8 devId, HUINT32 address, const HUINT8* dataPtr, HUINT32 dataLen)
{
	HUINT32 nSend;       // Bytes that send to device
	HUINT32 segmentLength;  // Save original segment length
	HUINT16 seqNum = 0;  // Record the firmware block sequence number that have been downloaded
	HINT16 PreviousFwDlPercent;
	HUINT8  payload[4];
	HUINT8 status = MXL_SUCCESS;
	MXL_STATUS_E tmpStatus = MXL_SUCCESS;

	int nUnitId = GetDeviceUnitId( devId );
	PMXL68x_DEV_CONTEXT_T devContextPtr = MxL68x_Ctrl_GetDeviceContext(nUnitId);
	if (devContextPtr)
	{
		segmentLength = dataLen;
		PreviousFwDlPercent = devContextPtr->fwDownloadPercent;

		Ctrl_SetBigEndian32(payload, address);
		// Send HOST_CMD_SET_ADDR command firstly

		tmpStatus = Ctrl_SendDownloadCommand(devId, HOST_CMD_SET_ADDR_CFG, payload, 4, 0);

		if (MXL_SUCCESS == tmpStatus)
		{
			//MxL_DLL_DEBUG0("Ctrl_DownloadFirmwareSegment: HOST_CMD_SET_ADDR_CFG success. Segment data length = %d\n",dataLen);

			while (dataLen)
			{
				// Calculate firmware download percent
				devContextPtr->fwDownloadPercent = PreviousFwDlPercent +  (HUINT16)((segmentLength - dataLen) * 100 / segmentLength);

				// Send segment data, the block length is nSend
				if (dataLen > (MxL68x_I2cMaxWriteLen - COMMAND_HEADER_LENGTH))
				{
					nSend = MxL68x_I2cMaxWriteLen - COMMAND_HEADER_LENGTH;
				}
				else
				{
					nSend = dataLen;
				}

				tmpStatus = Ctrl_SendDownloadCommand(devId, HOST_CMD_DOWNLOAD_SEGMENT_CFG, (void*)dataPtr, nSend, seqNum);

				if (MXL_SUCCESS == tmpStatus)
				{
					// Sending data pointer is moved to next segment
					seqNum ++;
					dataPtr += nSend;
					dataLen -= nSend;
					address += nSend;
				}
			}
		}
	}
	else
	{
		status = MXL_INVALID_PARAMETER;
	}
	return (MXL_STATUS_E)status;
}

MXL_STATUS_E Ctrl_DownloadFirmware(int devId, MBIN_FILE_T* mbinPtr, HUINT8 enableRun)
{
	MBIN_SEGMENT_T *segmentPtr ;
	HUINT32 index;
	HUINT32 dummy0 = 0;
	HUINT32 segLength;
	HUINT32 segAddress;
	HUINT8 status = MXL_SUCCESS;

	int nUnitId = GetDeviceUnitId( devId );
	PMXL68x_DEV_CONTEXT_T devContextPtr = MxL68x_Ctrl_GetDeviceContext(nUnitId);

	if (devContextPtr)
	{
		segmentPtr = (MBIN_SEGMENT_T*)(&mbinPtr->data[0]);

		CH_DI_Print(CH_LOG_4, ("[MXL683] Ctrl_DownloadFirmware() Header ID = %c,  Segments number = %d \n",mbinPtr->header.id, mbinPtr->header.numSegments));
		/* Check for proper file format */
		if (mbinPtr->header.id != MBIN_FILE_HEADER_ID )
		{
			CH_DI_Print(CH_LOG_0, ("[MXL683] Ctrl_DownloadFirmware() Invalid file header ID (%c)\n", mbinPtr->header.id));
			return MXL_FAILURE; // If the file is not MBIN format, then exit with MXL_FAILURE
		}

		/* Process each segment */
		for (index = 0; index < mbinPtr->header.numSegments; index++)
		{

			// Calculate firmware download percent
			devContextPtr->fwDownloadPercent = (HUINT16)(index + 1) * 100 / mbinPtr->header.numSegments;

			/* Validate segment header ID */
			if ( segmentPtr->header.id != MBIN_SEGMENT_HEADER_ID)
			{
				CH_DI_Print(CH_LOG_0, ("[MXL683] Ctrl_DownloadFirmware() Invalid file header ID (%c)\n", mbinPtr->header.id));
				return MXL_FAILURE; // If the segment format is wrong, then exit with MXL_FAILURE
			}

			/* Get segment data length and start address */

			segLength  = Ctrl_GetBigEndian24(&(segmentPtr->header.len24[0]));
			segAddress = Ctrl_GetBigEndian32(&(segmentPtr->header.address[0]));

			CH_DI_Print(CH_LOG_4, ("[MXL683] Ctrl_DownloadFirmware(),,,Segment_Header_ID = %c  segLength = %d, segAddress = %x\n",segmentPtr->header.id,segLength,segAddress));
			//MxL_DLL_DEBUG0("Segment%d: length: %02x %02x %02x, address: %02x %02x %02x %02x\n", index,
			//  segmentPtr->header.len24[0], segmentPtr->header.len24[1], segmentPtr->header.len24[2],
			//segmentPtr->header.address[0], segmentPtr->header.address[1], segmentPtr->header.address[2], segmentPtr->header.address[3]);

			/* Send segment */
			if (MXL_SUCCESS != Ctrl_DownloadFirmwareSegment(devId, segAddress, segmentPtr->data, segLength))
			{
				return MXL_FAILURE;
			}

			/* Next segment */
			segmentPtr = (MBIN_SEGMENT_T*)&(segmentPtr->data[((segLength + 3)/4)*4]);

		}

		CH_DI_Print(CH_LOG_4, ("[MXL683] Ctrl_DownloadFirmware() enableRun = %d \n",enableRun));
		if ((MXL_BOOL_E)enableRun == MXL_ENABLE)
		{
			// Run firmware after download
			status |= Ctrl_SendDownloadCommand(devId, HOST_CMD_DOWNLOAD_DONE_CFG, &(mbinPtr->header.entryAddress[0]), 4, 0);
		}
		else
		{
			// Do not run firmware after download
			status |= Ctrl_SendDownloadCommand(devId, HOST_CMD_DOWNLOAD_DONE_CFG, &dummy0, 4, 0);
		}

	}
	else
	{
		status = MXL_INVALID_PARAMETER;
	}

	return (MXL_STATUS_E)status;
}




MXL_STATUS_E MxLWare68x_API_CfgDrvInit(int nChannelId)
{
	MXL_STATUS_E status = MXL_SUCCESS;
	HINT32	nUnitId;

	nUnitId = GetDeviceUnitId( nChannelId );

	PMXL68x_DEV_CONTEXT_T devContextPtr = MxL68x_Ctrl_GetDeviceContext(nUnitId);

	if (devContextPtr)
	{
		if (devContextPtr->driverStatus == MXL68x_DRV_UNINITIALISED)
		{
			//MxL_DLL_DEBUG0("MxLWare68x_API_CfgDrvInit, Init Context [%d]\n",devId);
			// Driver is not initialised
			//MxL68x_OEM_DataPtr[devId] = oemDataPtr;
			devContextPtr->driverStatus = MXL68x_DRV_INITIALISED;    // change to enum
			devContextPtr->ifOutFreqInKHz = MXL68x_IF_FREQ_4000_KHZ;  // ISDB-T default setting
			devContextPtr->xtalFreqInMHz = MXL68x_XTAL_16MHz;    // Default XTAL freq
			devContextPtr->ifGainLevel = 13;   // default value
			devContextPtr->devVersion = MXL68x_VER_NA;
			devContextPtr->devOpMode = MXL68x_TUNER_DEMOD;
			devContextPtr->pllClkReady = MXL_FALSE;
			devContextPtr->firmwareStatus = MXL68x_FW_STATUS_UNKNOWN;
			devContextPtr->mxl68xType = UNKNOWN_DEVICE;
			devContextPtr->rfLoopThrough = MXL_DISABLE;
			devContextPtr->fwDownloadPercent = 0;
		}
		else
			status = MXL_ALREADY_INITIALIZED;
	}
	else
		status = MXL_INVALID_PARAMETER;

	CH_DI_Print(CH_LOG_4, ("[MXL683] MxLWare68x_API_CfgDrvInit()  FAIL (chid : %d, status : %d)! \n",nChannelId,status));

	return status;
}



MXL_STATUS_E MxLWare68x_API_CfgDrvDeInit(int devId)
{
	MXL_STATUS_E status = MXL_SUCCESS;
	HINT32	nUnitId;

	nUnitId = GetDeviceUnitId( devId );

	PMXL68x_DEV_CONTEXT_T devContextPtr = MxL68x_Ctrl_GetDeviceContext(nUnitId);

	if (devContextPtr)
	{
		CH_DI_Print(CH_LOG_4, ("[MXL683] MxLWare68x_API_CfgDrvDeInit() chid : %d)! \n",devId));
		// Un-register the interrupt handler call back function.
#if 0
		if (devContextPtr->intSrcId != MXL68x_INT_NONE)
		status = Ctrl_UnRegisterIntHandlerFunc(devId);
#endif
		devContextPtr->driverStatus = MXL68x_DRV_UNINITIALISED;
		devContextPtr->pllClkReady = MXL_FALSE;
		devContextPtr->firmwareStatus = MXL68x_FW_STATUS_UNKNOWN;
		devContextPtr->mxl68xType = UNKNOWN_DEVICE;
	}
	else
		status = MXL_INVALID_PARAMETER;

	CH_DI_Print(CH_LOG_4, ("[MXL683] MxLWare68x_API_CfgDrvDeInit() chid : %d, status = %d \n",devId,status));

	return status;
}


MXL_STATUS_E MxLWare68x_API_CfgDevPowerSupply(
                int devId,
                MXL68x_PWR_SUPPLY_E devPwrSupplyCfg)
{
	HUINT16 regData1, regData = 0;
	HUINT8 status = MXL_SUCCESS;
	HINT32	nUnitId;

	nUnitId = GetDeviceUnitId( devId );

	PMXL68x_DEV_CONTEXT_T devContextPtr = MxL68x_Ctrl_GetDeviceContext(nUnitId);


	if (devContextPtr)
	{
		if (devContextPtr->driverStatus)
		{
		// Enable Schmitt Trigger for I2C bus signals
		status = MXL683_I2C_Read(devId, MPEG_CLK_PHASE_REG - 1, &regData1);  // High 16 bit
		status |= MXL683_I2C_Read(devId, MPEG_CLK_PHASE_REG, &regData);   // Low 16 bit
		regData |= 0x03;  // Set Bit<1> and Bit<0> with 1
		status |= MXL683_I2C_Write(devId, MPEG_CLK_PHASE_REG - 1, regData1);   // High 16 bit
		status |= MXL683_I2C_Write(devId, MPEG_CLK_PHASE_REG, regData);   // Low 16 bit

		// Perform Demod soft reset
		status |= Ctrl_CfgDevReset(devId, MXL68x_DEMOD_RESET);

		// Inquire MxL68x device type
		status |= MXL683_I2C_Read(devId, DMD_ID_VERSION_REG, &regData);
		CH_DI_Print(CH_LOG_4, ("[MXL683] MxLWare68x_API_CfgDevPowerSupply() regData : %x, status = %d \n",regData,status));


		if (MXL_SUCCESS == status)
		{
			if ((regData & 0x03) == MXL681_DEVICE_ID)
				devContextPtr->mxl68xType = MXL681_DEVICE;
			else if ((regData & 0x03) == MXL683_DEVICE_ID)
				devContextPtr->mxl68xType = MXL683_DEVICE;
			else
				devContextPtr->mxl68xType = UNKNOWN_DEVICE;
		}
		CH_DI_Print(CH_LOG_4, ("[MXL683] MxLWare68x_API_CfgDevPowerSupply() regData : 0x%x, status = %d mxl68xType = %d\n",regData,status,devContextPtr->mxl68xType));

		// Configure single or dual power supply
		regData = (MXL68x_SINGLE_SUPPLY == devPwrSupplyCfg ) ? 0x14 : 0x10;

		status |= MXL683_I2C_Write(devId, POWER_SUPPLY_REG, regData);
		}
		else
		status = MXL_NOT_INITIALIZED;
	}
	else
		status = MXL_INVALID_PARAMETER;

	CH_DI_Print(CH_LOG_4, ("[MXL683] MxLWare68x_API_CfgDevPowerSupply() chid : %d, status = %d \n",devId,status));

	return(MXL_STATUS_E)status;
}

MXL_STATUS_E MxLWare68x_API_CfgDevXtal(
                int devId,
                MXL68x_XTAL_FREQ_E xtalFreqSel,
                HUINT8 xtalCap,
                MXL_BOOL_E clkOutEnable,
                MXL_BOOL_E clkOutDiv,
                MXL_BOOL_E enableXtalSharing,
		            MXL_BOOL_E enableXtalCal)
{
	HUINT16 regData = 0;
	HUINT8 status = MXL_SUCCESS;
	HINT32	nUnitId;

	nUnitId = GetDeviceUnitId( devId );

	PMXL68x_DEV_CONTEXT_T devContextPtr = MxL68x_Ctrl_GetDeviceContext(nUnitId);


	// Check for possible invalid parameters
	if ((devContextPtr) && (xtalCap > 0) && (xtalCap < 32))
	{
		if (devContextPtr->driverStatus)
		{
			status |= MXL683_I2C_Read(devId, XTAL_CFG_REG, &regData);


			//Removing this to solve issues where slace cap needs to be changed to 5pf
			//if(MXL_ENABLE == enableXtalCal)
			{
			// Set XTAL cap value bits <4:0>
			regData = regData & 0xE0;
			regData |= (xtalCap & 0x1F);
			}

			// Set XTAL frequency bit <5>
			regData = (xtalFreqSel == MXL68x_XTAL_16MHz) ? (regData & 0x00DF) : (regData | 0x0020);

			// Set CLK OUT bit <7>
			regData = (clkOutEnable == MXL_ENABLE) ? (regData | 0x0080) : (regData & 0x007F);

			// Program XTAL freq, clk out & xtal cap
			status |= MXL683_I2C_Write(devId, XTAL_CFG_REG, regData);
			regData = 0;

			if(MXL_ENABLE == enableXtalCal) // For Master Only
			{
				//Program Xtal Aplitude to a higher value
				status |= MXL683_I2C_Write(devId, DFE_DIG_ANA_XTAL_TH, 0x0003);
			}

			status |= MXL683_I2C_Read(devId, XTAL_SHARING_REG, &regData);

			// Set CLK OUT DIV <0>
			regData = (clkOutDiv == MXL_ENABLE) ? (regData | 0x0001) : (regData & 0x00FE);

			// Set XTAL sharing
			regData = (enableXtalSharing == MXL_ENABLE) ? (regData & 0x01) : (regData | 0x00);

			// Program XTAL sharing & clk out div
			status |= MXL683_I2C_Write(devId, XTAL_SHARING_REG, regData);

			if(MXL_ENABLE == enableXtalCal)
			{
				// Program Xtal external bias register
				regData = 0;
				status |= MXL683_I2C_Read(devId, DFE_XTAL_EXT_BIAS_REG, &regData);
				regData = ((regData & 0x00C0) | 0x000A);  // Program Bit<5:0> with 0x0A
				status |= MXL683_I2C_Write(devId, DFE_XTAL_EXT_BIAS_REG, regData);
				regData |= 0x0080;  // set bit<7>  to 1
				status |= MXL683_I2C_Write(devId, DFE_XTAL_EXT_BIAS_REG, regData);
				regData &= 0x007F;  // set bit<7> to 0
				status |= MXL683_I2C_Write(devId, DFE_XTAL_EXT_BIAS_REG, regData);
			}
			else
			{
				//Changed as per RFIC requirement to drive the voltage swing on slaves up
				status |= MXL683_I2C_Write(devId, DFE_XTAL_EXT_BIAS_REG, 0x008A);
			}

			// Add 1 ms waiting time
			VK_TASK_Sleep(1);

			// XTAL calibration
			status |= MXL683_I2C_Write(devId, XTAL_CALI_SET_REG, 0x0000);
			status |= MXL683_I2C_Write(devId, XTAL_CALI_SET_REG, 0x0001);
			// Add 1 ms waiting time
			VK_TASK_Sleep(1);
			status |= MXL683_I2C_Write(devId, XTAL_CALI_SET_REG, 0x0000);

			// Add 70 ms waiting time
			VK_TASK_Sleep(70);

			// Update Device Context
			devContextPtr->xtalFreqInMHz = xtalFreqSel;
		}
		else
			status = MXL_NOT_INITIALIZED;
	}
	else
		status = MXL_INVALID_PARAMETER;

	CH_DI_Print(CH_LOG_4, ("[MXL683] MxLWare68x_API_CfgDevXtal() chid : %d, status = %d \n",devId,status));

	return(MXL_STATUS_E)status;
}

MXL_STATUS_E MxLWare68x_API_CfgDevOperationalMode(
                int devId,
                MXL68x_DEV_MODE_E deviceOpMode)
{
	HUINT16 msbRegData = 0;
	HUINT16 lsbRegData = 0;
	HUINT8 status = MXL_SUCCESS;
	HINT32	nUnitId;

	nUnitId = GetDeviceUnitId( devId );

	PMXL68x_DEV_CONTEXT_T devContextPtr = MxL68x_Ctrl_GetDeviceContext(nUnitId);

	if (devContextPtr)
	{
		if (devContextPtr->driverStatus)
		{
			switch(deviceOpMode)
			{
				case MXL68x_TUNER_ONLY:
				// Disable Demod
				// status |= Ctrl_CfgDemodEnable(devId, MXL_DISABLE);
				// Switch SoC to XTAL clock
				status |= Ctrl_CfgDevSwitchClock(devId, MXL68x_XTAL_CLOCK);
				// Enable Tuner
				status |= MXL683_I2C_Write(devId, TUNER_ENABLE_REG, MXL_ENABLE);
				// START_TUNE = 0
				status |= MXL683_I2C_Write(devId, START_TUNE_REG, MXL_DISABLE);
				// update Device Context
				devContextPtr->devOpMode = MXL68x_TUNER_ONLY;
				break;

				case MXL68x_TUNER_DEMOD:
				// unclock gate CPU
				// Read DMD_GPIO_FUNC_MODE_REG high 16 bit
				status |= MXL683_I2C_Read(devId, DMD_GPIO_FUNC_MODE_REG - 1, &msbRegData);
				status |= MXL683_I2C_Read(devId, DMD_GPIO_FUNC_MODE_REG, &lsbRegData);
				lsbRegData &= 0xFFFD; // Set Bit<1> as 0
				// Write high 16 bit
				status |= MXL683_I2C_Write(devId, DMD_GPIO_FUNC_MODE_REG - 1, msbRegData);
				// Write low 16 bit
				status |= MXL683_I2C_Write(devId, DMD_GPIO_FUNC_MODE_REG, lsbRegData);
				// Enable demodulator clock to access demodulator registers, 0x3003 write 0xF89
				// Read high 16 bit
				// Read DMD_CCTL_RSTRQ_N_REG, read MSB
				status |= MXL683_I2C_Read(devId, DMD_CCTL_RSTRQ_N_REG - 1, &msbRegData);
				// Write high 16 bit
				status |= MXL683_I2C_Write(devId, DMD_CCTL_RSTRQ_N_REG - 1, msbRegData);
				// Write low 16 bit
				status |= MXL683_I2C_Write(devId, DMD_CCTL_RSTRQ_N_REG, 0x0F89);
				// Switch SoC to XTAL clock
				status |= Ctrl_CfgDevSwitchClock(devId, MXL68x_XTAL_CLOCK);
				// START_TUNE = 0
				status |= MXL683_I2C_Write(devId, START_TUNE_REG, MXL_DISABLE);
				// configure Tuner defaults overwrite
				// Set Tuner Application mode to ISDB-T
				status |= MxLWare68x_API_CfgTunerApplicationMode(devId, MXL68x_DIG_ISDBT);
				// Enable Tuner
				status |= MXL683_I2C_Write(devId, TUNER_ENABLE_REG, MXL_ENABLE);
				//Update device context OP mode
				devContextPtr->devOpMode = MXL68x_TUNER_DEMOD;
				break;

				default: status = MXL_INVALID_PARAMETER; break;
			}
		}
		else
			status = MXL_NOT_INITIALIZED;
	}
	else
		status = MXL_INVALID_PARAMETER;

	CH_DI_Print(CH_LOG_4, ("[MXL683] MxLWare68x_API_CfgDevOperationalMode() chid : %d, status = %d \n",devId,status));

	return(MXL_STATUS_E)status;
}

MXL_STATUS_E Ctrl_CfgDevSwitchClock(
                       int devId,
                       MXL68x_CLK_SWITCH_E clkType)
{
	HUINT16 k = 0;
	HUINT16 msbRegData = 0;
	HUINT16 lsbRegData = 0;
	HUINT8 status = MXL_SUCCESS;
	HINT32	nUnitId;

	nUnitId = GetDeviceUnitId( devId );

	PMXL68x_DEV_CONTEXT_T devContextPtr = MxL68x_Ctrl_GetDeviceContext(nUnitId);

	if (devContextPtr)
	{
		if (clkType == MXL68x_PLL_CLOCK)
		{
			while (k < SEQ_STATE_RB_POLLING_TIMES)
			{
				// Check if SEQ_STATE_RB bit<1:0> equals 0x0A3
				status |= MXL683_I2C_Read(devId, DFE_SEQ_STATE_RB_REG, &lsbRegData);
				if (((lsbRegData & 0x0F) >= DFE_SEQ_STATE_VALUE) && (status == MXL_SUCCESS))
				{
					CH_DI_Print(CH_LOG_4, ("[MXL683] Ctrl_CfgDevSwitchClock() Switch to PLL clock = %dx \n",lsbRegData));
					break;
				}
				k++;
				// Wait for 5ms
				VK_TASK_Sleep(10);
			}

			if ((k >= SEQ_STATE_RB_POLLING_TIMES) && ((lsbRegData & 0x0F) != DFE_SEQ_STATE_VALUE))
			{
				CH_DI_Print(CH_LOG_4, ("[MXL683] Ctrl_CfgDevSwitchClock() Err: Switching to PLL clock, SEQ_STATE_RB not equals 0x0A!\n"));
				status = MXL_FAILURE;
				devContextPtr->pllClkReady = MXL_FALSE;
			}
		}

		if (MXL_SUCCESS == status)
		{
			// Read high 16 bits
			status |= MXL683_I2C_Read(devId, DMD_CCTL_RSTRQ_N_REG - 1, &msbRegData);
			// Read low 16 bits
			status |= MXL683_I2C_Read(devId, DMD_CCTL_RSTRQ_N_REG, &lsbRegData);

			switch (clkType)
			{
				// Switch to PLL clock, set bit<13> to 1
				case MXL68x_PLL_CLOCK:
				lsbRegData |= 0x2000;
				devContextPtr->pllClkReady = MXL_TRUE;
				break;

				// Switch to XTAL clock, set bit<13> to 0
				case MXL68x_XTAL_CLOCK:
				lsbRegData &= 0xDFFF;
				devContextPtr->pllClkReady = MXL_FALSE;
				break;

				default: status = MXL_INVALID_PARAMETER; break;
			}

			// Write low 16 bit
			status |= MXL683_I2C_Write(devId, DMD_CCTL_RSTRQ_N_REG - 1, msbRegData);
			status |= MXL683_I2C_Write(devId, DMD_CCTL_RSTRQ_N_REG, (lsbRegData|0x0008));
		}

		// Wait for 15 ms
		VK_TASK_Sleep(15);
		CH_DI_Print(CH_LOG_4, ("[MXL683] Ctrl_CfgDevSwitchClock() chid : %d, status = %d \n",devId,status));
	}
	else
	{
		status = MXL_NOT_INITIALIZED;
	}
	return (MXL_STATUS_E)status;
}


MXL_STATUS_E MxLWare68x_API_CfgDevInit(
                int devId,
                MXL68x_IF_SRC_E ifSrc,
                MXL_BOOL_E enableRfLoopThru)
{
	HUINT16 regData = 0;
	HUINT8 status = MXL_SUCCESS;
	HINT32	nUnitId;

	nUnitId = GetDeviceUnitId( devId );

	PMXL68x_DEV_CONTEXT_T devContextPtr = MxL68x_Ctrl_GetDeviceContext(nUnitId);

	if (devContextPtr)
	{
		CH_DI_Print(CH_LOG_4, ("[MXL683] MxLWare68x_API_CfgDevInit() devContextPtr->driverStatus: %x, devContextPtr->firmwareStatus= %x devContextPtr->mxl68xType= %x \n",devContextPtr->driverStatus,devContextPtr->firmwareStatus,devContextPtr->mxl68xType));
		if (devContextPtr->driverStatus)
		{
			if (devContextPtr->firmwareStatus == MXL68x_FW_STATUS_UNKNOWN)
			{
				// Enable Tuner
				status = MXL683_I2C_Write(devId, TUNER_ENABLE_REG, 0x0001);

				// Start tune = 1
				status |= MXL683_I2C_Write(devId, START_TUNE_REG, 0x0001);

				// Switch to PLL Clock
				status |= Ctrl_CfgDevSwitchClock(devId, MXL68x_PLL_CLOCK);
			}

			if (devContextPtr->firmwareStatus != MXL68x_FW_STATUS_UNKNOWN)
			{
				if ((devContextPtr->mxl68xType == MXL681_DEVICE) && (ifSrc != MXL68x_IF_NA))
				{
					// Select IF source
					status |= MXL683_I2C_Read(devId, IF_FREQ_SEL_REG, &regData);
					regData = (ifSrc == MXL68x_IF_EXTERNAL)? (regData | 0xC0) : (regData & 0x3F);  // Bit<7:6>
					status |= MXL683_I2C_Write(devId, IF_FREQ_SEL_REG, regData);

					status |= MXL683_I2C_Read(devId, DIG_ANA_IF_CFG_0, &regData);
					regData = (ifSrc == MXL68x_IF_EXTERNAL)? (regData & 0xBF) : (regData | 0x40);  // Bit<6> = 0
					status |= MXL683_I2C_Write(devId, DIG_ANA_IF_CFG_0, regData);

					status |= MXL683_I2C_Read(devId, DIG_INT_AAF_EN_REG, &regData);
					regData = (ifSrc == MXL68x_IF_EXTERNAL)? (regData & 0xBF) : (regData | 0x40);  // Bit<6> = 0
					status |= MXL683_I2C_Write(devId, DIG_INT_AAF_EN_REG, regData);
				}

				if(devContextPtr->mxl68xType == MXL683_DEVICE)
				{
					//Loop-through is enabled only for MxL683 Removed PORT_HIGH/LOW logic

					CH_DI_Print(CH_LOG_4, ("[MXL683] MxLWare68x_API_CfgDevInit() MxL683 LT = %d \n",enableRfLoopThru));
					// Configure RF Loop Through option
					status |= MXL683_I2C_Read(devId, DFE_LT_CONFIG_REG, &regData);

					// For MxL683 RF LT EN default is high
					// Bit<4> = 1, turn on loop through : 0, turn on loop through
					//If Loop Through
					if (MXL_FALSE == enableRfLoopThru)
						regData &= 0xFFEF;
					else
						regData |= 0x10;

					status |= MXL683_I2C_Write(devId, DFE_LT_CONFIG_REG, regData);
					devContextPtr->rfLoopThrough = enableRfLoopThru;
				}
				else
				{
					CH_DI_Print(CH_LOG_4, ("[MXL683] MxLWare68x_API_CfgDevInit() MxLWare68x_API_CfgDevInit: MxL681 LT Disabled \n"));

					// Configure RF Loop Through option
					status |= MXL683_I2C_Read(devId, DFE_LT_CONFIG_REG, &regData);

					// For MxL681 disable RF LT
					// Bit<4> = 0, turn off loop through
					regData &= 0xFFEF;

					status |= MXL683_I2C_Write(devId, DFE_LT_CONFIG_REG, regData);
					devContextPtr->rfLoopThrough = MXL_FALSE;
				}
			}
		}
		else
			status = MXL_NOT_INITIALIZED;
	}
	else
		status = MXL_INVALID_PARAMETER;

	CH_DI_Print(CH_LOG_4, ("[MXL683] MxLWare68x_API_CfgDevInit() chid : %d, status = %d \n",devId,status));

	return(MXL_STATUS_E)status;
}


MXL_STATUS_E MxLWare68x_API_CfgDevFirmwareDownload(
                int devId,
                HUINT32 mbinBufferSize,
                HUINT8 *mbinBufferPtr)
{
	HUINT8 status = MXL_SUCCESS;
	HUINT16 mailbox = 0;
	HINT32	nUnitId;

	nUnitId = GetDeviceUnitId( devId );

	PMXL68x_DEV_CONTEXT_T devContextPtr = MxL68x_Ctrl_GetDeviceContext(nUnitId);

	// Check for invalid parameters
	if ((devContextPtr) && (mbinBufferPtr) && (mbinBufferSize))
	{
		CH_DI_Print(CH_LOG_4, ("[MXL683] MxLWare68x_API_CfgDevFirmwareDownload() driverStatus : %d, firmwareStatus = %d pllClkReady = %d\n",devContextPtr->driverStatus,devContextPtr->firmwareStatus,devContextPtr->pllClkReady));
		if (devContextPtr->driverStatus)
		{
			// Check if FW is already loaded or not
			if (devContextPtr->firmwareStatus == MXL68x_FW_STATUS_LOADED)
			{
				status = MXL_SUCCESS;
			}
			else
			{
				// Download firmware only if PLL clock is ready
				if (devContextPtr->pllClkReady == MXL_TRUE)
				{
					// Download Firmware
					status |= Ctrl_DownloadFirmware(devId, (MBIN_FILE_T*)mbinBufferPtr, (HUINT8)MXL_ENABLE);
					if (status == MXL_SUCCESS)
					{
						devContextPtr->firmwareStatus = MXL68x_FW_STATUS_LOADED;
						devContextPtr->fwDownloadPercent = FIRMWARE_DOWNLOAD_DONE_PERCENT;
					}
					else
					{
						devContextPtr->firmwareStatus = MXL68x_FW_STATUS_UNKNOWN;
						devContextPtr->fwDownloadPercent = FIRMWARE_DOWNLOAD_FAIL_PERCENT;
					}
				}
				else
					status = MXL_FAILURE;
			}

			// Wait for 100 ms
			VK_TASK_Sleep(100);
#if 0
{
		HUINT8 i;
		HUINT16 read_1;
		while(1)
		{
			VK_TASK_Sleep(100);
			MXL683_I2C_Read(devId, 0xA8CC, &mailbox);
			CH_DI_Print(0, ("[MXL683] MxLWare68x_API_CfgDevFirmwareDownload() mailbox [%x] \n",mailbox));
			if(mailbox==0xABCD)
				break;
			MXL683_I2C_Read(devId, FIRMWARE_LOADED_CHECK_REG, &read_1);
			CH_DI_Print(0, ("[MXL683] read_1 = %x, read_2 = %x \n",read_1));
		}

}
#else
			MXL683_I2C_Read(devId, 0xA8CC, &mailbox);
			CH_DI_Print(CH_LOG_0, ("[MXL683] MxLWare68x_API_CfgDevFirmwareDownload() mailbox [%x] \n",mailbox));
#endif
			if(mailbox!=0xABCD)
			{
				CH_DI_Print(CH_LOG_0, ("[MXL683] MxLWare68x_API_CfgDevFirmwareDownload() FW Download Error Mailbox 0x%x [%d] \n",mailbox,devId));
				status = MXL_FAILURE;
			}
			else
			{
				CH_DI_Print(CH_LOG_0, ("[MXL683] MxLWare68x_API_CfgDevFirmwareDownload() FW Download Success Mailbox 0x%x [%d] \n",mailbox,devId));
			}
		}
		else
		{
			status = MXL_NOT_INITIALIZED;
			CH_DI_Print(CH_LOG_0, ("[MXL683] MxLWare68x_API_CfgDevFirmwareDownload() FW Download Error status [%d] \n",status));
		}
	}
	else
		status = MXL_INVALID_PARAMETER;

	CH_DI_Print(CH_LOG_4, ("[MXL683] MxLWare68x_API_CfgDevFirmwareDownload() chid : %d, status = %d \n",devId,status));
	return(MXL_STATUS_E)status;
}

MXL_STATUS_E Ctrl_SendHostCommand(int devId, PHOST_COMMAND_T commandPtr)
{
	HUINT16 bufferSize,i;
	MXL_STATUS_E retStatus = MXL_SUCCESS;
	HUINT16 RegAddr;
	HUINT8 writeBuffer[sizeof(HOST_COMMAND_T)];

	// If there is command data, copy it into writeBuffer
	if (commandPtr->payloadLen > 0)
	{
		for (i = 0; i < commandPtr->payloadLen; i++)  // Copy data from commandPtr->data to writeBuffer
			writeBuffer[2 + i] = commandPtr->data[i];

		bufferSize = commandPtr->payloadLen + 2;
	}
	else
		bufferSize = 2;

	RegAddr = commandPtr->syncChar;
	RegAddr = (RegAddr << 8) + (HUINT16)(commandPtr->commandId);
	writeBuffer[0] = commandPtr->payloadCheckSum;
	writeBuffer[1] = commandPtr->payloadLen;

	// Send host command to MxL68x
	retStatus = MXL683_I2C_WriteBlock(devId, RegAddr, bufferSize, writeBuffer);

	return retStatus;
}


MXL_STATUS_E Ctrl_ProgramRegisters(int devId, PREG_CTRL_INFO_T ctrlRegInfoPtr)
{
	MXL_STATUS_E status = MXL_SUCCESS;
	HUINT16 i = 0;
	HUINT16 tmp = 0;

	i = 0;
	while (1)
	{
		if ((ctrlRegInfoPtr[i].regAddr == 0) && (ctrlRegInfoPtr[i].mask == 0) && (ctrlRegInfoPtr[i].data == 0))
			break;

		// Check if partial bits of register were updated
		if (ctrlRegInfoPtr[i].mask != 0xFFFF)
		{
			status = MXL683_I2C_Read(devId,ctrlRegInfoPtr[i].regAddr, &tmp);
			if (status != MXL_SUCCESS) break;
		}

		tmp &= ~ctrlRegInfoPtr[i].mask;
		tmp |= ctrlRegInfoPtr[i].data;

		status = MXL683_I2C_Write(devId,ctrlRegInfoPtr[i].regAddr, tmp);
		if (status != MXL_SUCCESS) break;

		i++;
	}


	return status;
}

MXL_STATUS_E Ctrl_CfgDevReset(int devId, MXL68x_RESET_TYPE_E resetType)
{
	HUINT8 status = MXL_SUCCESS;
	HUINT16 msbRegData = 0;
	HUINT16 lsbRegData = 0;
	HOST_COMMAND_T tunerReset;
	HINT32	nUnitId;

	nUnitId = GetDeviceUnitId( devId );

	PMXL68x_DEV_CONTEXT_T devContextPtr = MxL68x_Ctrl_GetDeviceContext(nUnitId);

	if (devContextPtr)
	{
		if (devContextPtr->driverStatus)
		{
			// Read DMD_CCTL_RSTRQ_N_REG, read MSB
			status |= MXL683_I2C_Read(devId, DMD_CCTL_RSTRQ_N_REG - 1, &msbRegData);

			// Read DMD_CCTL_RSTRQ_N_REG, read LSB
			status |= MXL683_I2C_Read(devId, DMD_CCTL_RSTRQ_N_REG, &lsbRegData);

			switch (resetType)
			{
				// Bit<31:16>, Set bit<25> as 1
				case MXL68x_DEMOD_DATAPATH_RESET: msbRegData |= 0x0200; break;

				// Bit<31:16>, Set bit<24> as 1
				case MXL68x_DEMOD_RESET: msbRegData |= 0x0100; break;

				case MXL68x_TUNER_RESET:
				FORM_CMD_HEADER(tunerReset, HOST_CMD_TUNER_DFE_RESET_CFG, 0, 0);
				status |= Ctrl_SendHostCommand(devId, &tunerReset);
				break;

				default:
				break;
			}

			if(MXL68x_TUNER_RESET != resetType)
			{
				// Program MSB part of the reset register
				status |= MXL683_I2C_Write(devId, DMD_CCTL_RSTRQ_N_REG - 1, msbRegData);

				// Write LSB part of the reset register
				status |= MXL683_I2C_Write(devId, DMD_CCTL_RSTRQ_N_REG, (lsbRegData|0x0008));

				// Tuner & Demod blocks are active reset, clear reset bits to start reset function
				// REVIST for TUNER RESET - MK
				switch (resetType)
				{
					// Bit<31:16>, Set bit<25> as 0
					case MXL68x_DEMOD_DATAPATH_RESET: msbRegData &= 0xFDFF; break;

					// Bit<31:16>, Set bit<24> as 0
					case MXL68x_DEMOD_RESET: msbRegData &= 0xFEFF; break;

					default:
						break;
				}

				// Program MSB part of the reset register
				status |= MXL683_I2C_Write(devId, DMD_CCTL_RSTRQ_N_REG - 1, msbRegData);

				// Write LSB part of the reset register
				status |= MXL683_I2C_Write(devId, DMD_CCTL_RSTRQ_N_REG, (lsbRegData|0x0008));
			}
		}
		else
			status = MXL_NOT_INITIALIZED;
	}
	else
		status = MXL_INVALID_PARAMETER;

	return(MXL_STATUS_E)status;
}


MXL_STATUS_E Ctrl_SendDownloadCommand(int devId, MXL_CMD_ID_E commandId, void* dataPtr, HUINT32 dataLen, HUINT16 downloadBlockCnt)
{
	HOST_COMMAND_T downloadFwSeg;
	FW_RESPONSE_T  responsePtr;
	HUINT32 i;
	HUINT8 status = MXL_SUCCESS;

	// Check if the sending data length exceeds the limitation
	if (dataLen > (MxL68x_I2cMaxWriteLen - COMMAND_HEADER_LENGTH))
	{
		CH_DI_Print(CH_LOG_0, ("[MXL683] Ctrl_SendDownloadCommand() Invalid command length (%d)\n", dataLen));
		return MXL_FAILURE;
	}

	switch (commandId)
	{
		case HOST_CMD_SET_ADDR_CFG:
		//Ctrl_GetBigEndian32((HUINT8*)dataPtr);
		break;

		case HOST_CMD_DOWNLOAD_SEGMENT_CFG:
		break;

		case HOST_CMD_DOWNLOAD_DONE_CFG:
		//CH_DI_Print(CH_LOG_4, ("[MXL683] Ctrl_SendDownloadCommand() COMMAND_DOWNLOAD_DONE_CFG (0x%08x)\n", Ctrl_GetBigEndian32((HUINT8*)dataPtr)));
		break;

		default: break;
	}

	// Populate command header
	FORM_CMD_HEADER(downloadFwSeg, commandId, downloadBlockCnt, dataLen);

	// Copy data from dataPtr to downloadFwSeg.data
	for (i = 0; i < dataLen; i++)  // Copy data from dataPtr to downloadFwSeg.data
		downloadFwSeg.data[i] = (HUINT8)*((HUINT8*)dataPtr+i);

	// Calculate the CheckSum
	status |= Ctrl_CalCheckSum(downloadFwSeg.payloadLen, (HUINT8 *)dataPtr, &(downloadFwSeg.payloadCheckSum));

	if (commandId == HOST_CMD_DOWNLOAD_DONE_CFG)
	{
		i = 0;
		do
		{
			VK_TASK_Sleep(100);
			status |= Ctrl_GetDeviceResp(devId, HOST_CMD_DOWNLOAD_DONE_CFG, &responsePtr);
		}
		while ((status != MXL_SUCCESS) && (i++ < 5));

		if (status != MXL_SUCCESS)  // Means there is error either in Command transfering or response receiving
		{
			CH_DI_Print(CH_LOG_0, ("[MXL683] Ctrl_SendDownloadCommand() 111 FW Download Error ! \n"));
			//return MXL_FAILURE;
		}
	}

	status = Ctrl_SendHostCommand(devId, &downloadFwSeg);

	if (status != MXL_SUCCESS)  // Means there is error either in Command transfering or response receiving
	{
		i = 0;
		do
		{
			VK_TASK_Sleep(100);
			status |= Ctrl_SendHostCommand(devId, &downloadFwSeg);
		}
		while ((status != MXL_SUCCESS) && (i++ < 5));

		if (status != MXL_SUCCESS)
		{
			CH_DI_Print(CH_LOG_0, ("[MXL683] Ctrl_SendDownloadCommand() Calling Ctrl_SendHostCommand error ! \n"));
			return MXL_FAILURE;
		}

	}
	//else
	//  MxL_DLL_DEBUG0("No.%d block is downloaded success. \n", downloadBlockCnt);

	if (commandId == HOST_CMD_DOWNLOAD_DONE_CFG)
	{
		i = 0;
		do
		{
			VK_TASK_Sleep(100);
			status |= Ctrl_GetDeviceResp(devId, commandId, &responsePtr);
		}
		while ((status != MXL_SUCCESS) && (i++ < 5));

		if (status != MXL_SUCCESS)  // Means there is error either in Command transfering or response receiving
		{
			CH_DI_Print(CH_LOG_0, ("[MXL683] Ctrl_SendDownloadCommand() 222 FW Download Error ! \n"));
			return MXL_FAILURE;
		}
	}

	return (MXL_STATUS_E)status;
}

MXL_STATUS_E Ctrl_DfeRegRead(int devId, HUINT16 regAddr, HUINT8 numRegs, HUINT32* regDataPtr )
{

	HUINT8 status = MXL_SUCCESS;
	HOST_COMMAND_T dfeRegRead;
	HUINT16 tmp = 0;
	HUINT8 checksum = 0;

	FORM_CMD_HEADER(dfeRegRead, HOST_CMD_DEV_DFE_REG_READ, 0, 3);

	dfeRegRead.data[0] = (regAddr >> 8) & 0xFF;
	checksum +=  dfeRegRead.data[0];

	dfeRegRead.data[1] = regAddr & 0xFF;
	checksum +=  dfeRegRead.data[1];

	dfeRegRead.data[2] = numRegs;
	checksum +=  dfeRegRead.data[2];

	dfeRegRead.payloadCheckSum = checksum;

	status |= Ctrl_SendHostCommand(devId, &dfeRegRead);

	// Add 1 ms waiting time
	VK_TASK_Sleep(1);

	status |= MXL683_I2C_Read(devId,DFE_FW_REG_BUFFER_MSB, &tmp);
	*regDataPtr = tmp;

	status |= MXL683_I2C_Read(devId,DFE_FW_REG_BUFFER_LSB, &tmp);
	*regDataPtr = (*regDataPtr << 16) | tmp;

	return (MXL_STATUS_E)status;
}


MXL_STATUS_E Ctrl_DfeRegWrite(int devId, HUINT16 regAddr, HUINT8 numRegs, HUINT32 regData )
{

	HUINT8 status = MXL_SUCCESS;
	HOST_COMMAND_T dfeRegWrite;
	HUINT8 checksum = 0;

	FORM_CMD_HEADER(dfeRegWrite, HOST_CMD_DEV_DFE_REG_WRITE, 0, 7);

	dfeRegWrite.data[0] = (regAddr >> 8) & 0xFF;
	checksum +=  dfeRegWrite.data[0];

	dfeRegWrite.data[1] = regAddr & 0xFF;
	checksum +=  dfeRegWrite.data[1];

	// Send the 32 bits of Reg Data
	dfeRegWrite.data[2] = numRegs;
	checksum +=  dfeRegWrite.data[2];

	dfeRegWrite.data[3] = (regData >> 24) & 0xFF;
	checksum +=  dfeRegWrite.data[3];

	dfeRegWrite.data[4] = (regData >> 16) & 0xFF;
	checksum +=  dfeRegWrite.data[4];

	dfeRegWrite.data[5] = (regData >> 8) & 0xFF;
	checksum +=  dfeRegWrite.data[5];

	dfeRegWrite.data[6] = regData & 0xFF;
	checksum +=  dfeRegWrite.data[6];

	dfeRegWrite.payloadCheckSum = checksum;

	status |= Ctrl_SendHostCommand(devId, &dfeRegWrite);

	// Add 1 ms waiting time
	VK_TASK_Sleep(1);

	return (MXL_STATUS_E)status;
}


MXL_STATUS_E Ctrl_GetDeviceResp(int devId, HUINT16 subAddr, PFW_RESPONSE_T responsePtr)
{
	HUINT16 bufferSize,i;
	HUINT8 status = MXL_SUCCESS;
	HUINT8 readBuffer[sizeof(HOST_COMMAND_T)];

	// Read response header firstly
	status |= MXL683_I2C_ReadBlock(devId, subAddr, COMMAND_HEADER_LENGTH, readBuffer);

	CH_DI_Print(CH_LOG_4, ("[MXL683] Ctrl_GetDeviceResp() Block read back : SYNC=0x%02X, errorCode=0x%02X, checkSum=0x%02X, dataLen=0x%02X. \n",
	readBuffer[0],readBuffer[1],readBuffer[2],readBuffer[3]));

	if (status == MXL_SUCCESS)  // Read block success
	{
		responsePtr->syncChar = readBuffer[0];
		responsePtr->errorCode = (MXL_RSP_ID_E)readBuffer[1];
		responsePtr->payloadCheckSum = readBuffer[2];
		responsePtr->payloadLen = readBuffer[3];

		// Check if previous command execution success or not
		if (responsePtr->errorCode == RESPONSE_ID_SUCCESS)
		{
			// Check if need to read response data
			if (responsePtr->payloadLen > 0)
			{
				bufferSize = responsePtr->payloadLen;

				// Read Response data
				status |= MXL683_I2C_ReadBlock(devId, subAddr, bufferSize, readBuffer);

				// Check if received response data is complete or not
				Ctrl_CheckIntegration(responsePtr->payloadCheckSum, (HUINT8)bufferSize, readBuffer);

				if (status == MXL_SUCCESS) // Read response data success
				{
					for (i = 0; i < bufferSize; i++)  // Copy data from readBuffer to responsePtr->data
						responsePtr->data[i] = readBuffer[i];
					/*
					MxL_DLL_DEBUG0("Block read back : SYNC=0x%02X, errorCode=0x%02X, checkSum=0x%02X, dataLen=0x%02X. \n",
					readBuffer[0],readBuffer[1],readBuffer[2],readBuffer[3]);
					*/
				}
			}
		}
		else
			status = MXL_FAILURE;
	}
	return((MXL_STATUS_E)status);
}

MXL_STATUS_E Ctrl_SetRfFreqLutReg(int devId, HUINT32 FreqInHz, PMXL68x_CHAN_DEP_FREQ_TABLE_T freqLutPtr)
{
	HUINT8 status = MXL_SUCCESS;
	HUINT8 idx = 0;
	HUINT16 readBack, regSetData[MAX_SPUR_REG_NUM] = {0, 0, 0, 0};
	CHAN_DEP_SPUR_REG_T MxL68x_SPUR_REGISTER;

	MxL68x_SPUR_REGISTER.SpurRegNum = MAX_SPUR_REG_NUM;
	MxL68x_SPUR_REGISTER.SpurRegAddr[0] =  DFE_RFLUT_SWP1_REG;
	MxL68x_SPUR_REGISTER.SpurRegAddr[1] =  DFE_RFLUT_DIV_MOD_REG;
	MxL68x_SPUR_REGISTER.SpurRegAddr[2] =  DFE_REFLUT_BYP_REG;
	MxL68x_SPUR_REGISTER.SpurRegAddr[3] =  DFE_REFSX_INT_MOD_REG;

	if (freqLutPtr)
	{
		// Find and get default value firstly.
		for (idx = 0; 0 != freqLutPtr->centerFreqHz; idx++, freqLutPtr++)
		{
			if (freqLutPtr->centerFreqHz == 1)
			{
				// When center frequency is 1 means corresponding data is default value
				regSetData[0] = freqLutPtr->rfLutSwp1Reg;
				regSetData[1] = freqLutPtr->rfLutDivInBypReg;
				regSetData[2] = freqLutPtr->refLutBypReg;
				regSetData[3] = freqLutPtr->refIntModReg;

				break;
			} // end of if ((freqLutPtr->centerFreqHz -
		} // end of for (idx = 0;

		// Check in LUT
		for (idx = 0; 0 != freqLutPtr->centerFreqHz; idx++, freqLutPtr++)
		{
			if ((freqLutPtr->centerFreqHz - SPUR_SHIFT_FREQ_WINDOW) <= FreqInHz &&
			(freqLutPtr->centerFreqHz + SPUR_SHIFT_FREQ_WINDOW) >= FreqInHz)
			{
				regSetData[0] = freqLutPtr->rfLutSwp1Reg;
				regSetData[1] = freqLutPtr->rfLutDivInBypReg;
				regSetData[2] = freqLutPtr->refLutBypReg;
				regSetData[3] = freqLutPtr->refIntModReg;

				break;
			} // end of if ((freqLutPtr->centerFreqHz -
		} // end of for (idx = 0;
	}

	// Program registers
	for (idx = 0; idx < MxL68x_SPUR_REGISTER.SpurRegNum; idx++)
	{
		if (MxL68x_SPUR_REGISTER.SpurRegAddr[idx] == DFE_REFLUT_BYP_REG)
		{
			status |= MXL683_I2C_Read(devId, MxL68x_SPUR_REGISTER.SpurRegAddr[idx], &readBack);
			regSetData[idx] |= (readBack & 0x00DF);  // Protect bit <4:0>, clear bit<5>, 1101 1111 = 0xDF
		}
		status |= MXL683_I2C_Write(devId, MxL68x_SPUR_REGISTER.SpurRegAddr[idx], regSetData[idx]);
	}

	return(MXL_STATUS_E)status;
}

MXL68x_FW_STATUS_E Ctrl_CheckFirmwareStatus(int devId)
{
	MXL68x_FW_STATUS_E fwStatus = MXL68x_FW_STATUS_UNKNOWN;
	HUINT8 mxlStatus = MXL_SUCCESS;
	HUINT16 fwDlStatus = 0;
	HUINT16 fwClkStatus = 0;

	// Readk back 0x302B bit<0> to know if firmware has been downloaded
	mxlStatus |= MXL683_I2C_Read(devId, FIRMWARE_LOADED_CHECK_REG - 1, &fwDlStatus);
	mxlStatus |= MXL683_I2C_Read(devId, FIRMWARE_LOADED_CHECK_REG, &fwDlStatus);

	// Readk back 0x300B bit<1> to know if CPU is clock gated
	mxlStatus |= MXL683_I2C_Read(devId, DMD_GPIO_FUNC_MODE_REG - 1, &fwClkStatus);
	mxlStatus |= MXL683_I2C_Read(devId, DMD_GPIO_FUNC_MODE_REG, &fwClkStatus);

	// Bit<0> = 1 means firmware has been downloaded successfully
	if ((mxlStatus == MXL_SUCCESS) && ((fwDlStatus & 0x01) == 0x01) && ((fwClkStatus & 0x02) == 0x00))
	{
		fwStatus = MXL68x_FW_STATUS_LOADED;
	}
	else if ((mxlStatus == MXL_SUCCESS) && ((fwDlStatus & 0x01) == 0x01) && ((fwClkStatus & 0x02) == 0x02))
	{
		fwStatus = MXL68x_FW_STATUS_CLKGATED;
	}
	else if ((mxlStatus == MXL_SUCCESS) && ((fwDlStatus & 0x01) == 0x00) && ((fwClkStatus & 0x02) == 0x00))
	{
		fwStatus = MXL68x_FW_STATUS_BOOTLOADER;
	}
	else
	{
		fwStatus = MXL68x_FW_STATUS_UNKNOWN;
	}

	return fwStatus;
}

MXL_STATUS_E Ctrl_AssignNumDenCR(HUINT8 *num, HUINT8 *den,  HUINT8 cr)
{
	HUINT8 status = MXL_SUCCESS;

	switch (cr)
	{
		case MXL68x_CR_1_2:
		*num = 1;
		*den = 2;
		break;

		case MXL68x_CR_2_3:
		*num = 2;
		*den = 3;
		break;

		case MXL68x_CR_3_4:
		*num = 3;
		*den = 4;
		break;

		case MXL68x_CR_5_6:
		*num = 5;
		*den = 6;
		break;

		case MXL68x_CR_7_8:
		*num = 7;
		*den = 8;
		break;

		default:
		// Assign 1 by default to avoid division by 0
		*num = 1;
		*den = 1;
		status = MXL_FAILURE;
		break;
	}
	return (MXL_STATUS_E)status;
}








MXL_STATUS_E MxLWare68x_API_CfgDevPowerMode(
                int devId,
                MXL68x_PWR_MODE_E powerMode)
{
	HUINT8 status = MXL_SUCCESS;
	HOST_COMMAND_T pwrmodeSet;
	HINT32	nUnitId;

	nUnitId = GetDeviceUnitId( devId );

	PMXL68x_DEV_CONTEXT_T devContextPtr = MxL68x_Ctrl_GetDeviceContext(nUnitId);

	if (devContextPtr)
	{
		if (devContextPtr->driverStatus)
		{
			switch(powerMode)
			{
				case MXL68x_PWR_MODE_ACTIVE:
				// Populate command header
				FORM_CMD_HEADER(pwrmodeSet, HOST_CMD_DEV_POWER_MODE_ON_CFG, 0, 0);
				status |= Ctrl_SendHostCommand(devId, &pwrmodeSet);

				// Program tuner related registers
				status |= MXL683_I2C_Write(devId, TUNER_ENABLE_REG, MXL_ENABLE);
				status |= MXL683_I2C_Write(devId, START_TUNE_REG, MXL_ENABLE);

				break;

				case MXL68x_PWR_MODE_STANDBY:
				// Populate command header
				FORM_CMD_HEADER(pwrmodeSet, HOST_CMD_DEV_POWER_MODE_STANDBY_CFG, 0, 0);
				status |= Ctrl_SendHostCommand(devId, &pwrmodeSet);
				break;

				case MXL68x_PWR_MODE_EWS:
				// Populate command header
				FORM_CMD_HEADER(pwrmodeSet, HOST_CMD_DEV_POWER_MODE_AUTO_EWS_CFG, 0, 0);
				status |= Ctrl_SendHostCommand(devId, &pwrmodeSet);
				break;

				default:
				status = MXL_INVALID_PARAMETER;
				break;
			}
		}
		else
			status = MXL_NOT_INITIALIZED;
	}
	else
		status = MXL_INVALID_PARAMETER;

	CH_DI_Print(CH_LOG_4, ("[MXL683] MxLWare68x_API_CfgDevPowerMode() chid : %d, status = %d \n",devId,status));

	return(MXL_STATUS_E)status;
}


MXL_STATUS_E Ctrl_CfgDemodEnable(int devId, MXL_BOOL_E demodEnable)
{
	HUINT16 msbRegData = 0;
	HUINT16 lsbRegData = 0;
	HUINT8 status = MXL_SUCCESS;

	CH_DI_Print(CH_LOG_4, ("[MXL683] Ctrl_CfgDemodEnable() chid : %d, status = %d set = %d (0 = disable, 1 = enable) \n",devId,demodEnable));

	status |= MXL683_I2C_Read(devId, DMD_CCTL_DEMRQ_REG - 1, &msbRegData);
	status |= MXL683_I2C_Read(devId, DMD_CCTL_DEMRQ_REG, &lsbRegData);

	// Enable or Disable demod
	lsbRegData = (demodEnable == MXL_ENABLE) ? 0x0001 : 0x0000;

	status |= MXL683_I2C_Write(devId, DMD_CCTL_DEMRQ_REG - 1, 0x0000);
	status |= MXL683_I2C_Write(devId, DMD_CCTL_DEMRQ_REG, lsbRegData);

	return (MXL_STATUS_E)status;
}

MXL_STATUS_E MxLWare68x_API_CfgDevGPIO (
                int devId,
                MXL68x_GPIO_ID_E gpioId,
                MXL68x_GPIO_TYPE_E  gpioType,
                MXL68x_GPIO_STATE_E gpioState)
{
	HUINT32 gpioRegData = 0;
	HUINT16 regData = 0;
	HUINT8 status = MXL_SUCCESS;
	HINT32	nUnitId;

	nUnitId = GetDeviceUnitId( devId );

	PMXL68x_DEV_CONTEXT_T devContextPtr = MxL68x_Ctrl_GetDeviceContext(nUnitId);

	if (devContextPtr)
	{
		if (devContextPtr->driverStatus)
		{
			// Read MSB
			status |= MXL683_I2C_Read(devId, DMD_GPIO_FUNC_MODE_REG - 1, &regData);
			gpioRegData = (regData << 16);

			// Read LSB
			status |= MXL683_I2C_Read(devId, DMD_GPIO_FUNC_MODE_REG, &regData);
			gpioRegData |= regData;

			// Set corresponding GPIO enable bit, from bit<20> (0 is input, 1 is output)
			if (gpioType == MXL68x_GPIO_OUTPUT)
			{
				// Enable GPIO as Output
				gpioRegData |= (0x00100000 << (gpioId - 1));

				// Set GPO as HIGH or LOW
				if (gpioState == MXL68x_GPIO_HIGH)
					gpioRegData |= (0x00002000 << (gpioId - 1));
				else
					gpioRegData &= ~(0x00002000 << (gpioId - 1));
			}
			else
			{
				// Disable GPIO output
				gpioRegData &= ~(0x00100000 << (gpioId - 1));
			}

			// Write high 16 bit
			regData = (HUINT16)((gpioRegData & 0xFFFF0000) >> 16);
			status |= MXL683_I2C_Write(devId, DMD_GPIO_FUNC_MODE_REG - 1, regData);

			// Write low 16 bit
			regData = (HUINT16)(gpioRegData & 0x0FFFF);
			status |= MXL683_I2C_Write(devId, DMD_GPIO_FUNC_MODE_REG, regData);

		}
		else
			status = MXL_NOT_INITIALIZED;

	}
	else
		status = MXL_INVALID_PARAMETER;

	CH_DI_Print(CH_LOG_4, ("[MXL683] MxLWare68x_API_CfgDevGPIO() chid : %d, status = %d \n",devId,status));

	return(MXL_STATUS_E)status;
}


MXL_STATUS_E MxLWare68x_API_ReqDevVersionInfo(
                int devId,
                HUINT8* chipIdPtr,
                HUINT8* chipVersionPtr,
                HUINT8 mxlWareVer[MXL68X_VERSION_SIZE],
                HUINT8 firmwareVer[MXL68X_VERSION_SIZE],
                HUINT8 bootloaderVer[MXL68X_VERSION_SIZE])
{
	HUINT16 lsbRegData = 0;
	HUINT8 status = MXL_SUCCESS;
	HUINT8 k = 0;
	FW_RESPONSE_T devVerInfoRsp;
	HINT32	nUnitId;

	nUnitId = GetDeviceUnitId( devId );

	PMXL68x_DEV_CONTEXT_T devContextPtr = MxL68x_Ctrl_GetDeviceContext(nUnitId);

	if (devContextPtr)
	{
		if (devContextPtr->driverStatus)
		{
			for (k = 0; k < MXL68X_VERSION_SIZE; k++)
			{
				mxlWareVer[k] = MxLWareDrvVersion[k];
				firmwareVer[k] = 0;
				bootloaderVer[k] = 0;
			}


			CH_DI_Print(CH_LOG_4, ("[MXL683] MxLWare68x_API_ReqDevVersionInfo() MxLWare version: 0x%x.0x%x.0x%x.0x%x\n",
			                      mxlWareVer[0],
			                      mxlWareVer[1],
			                      mxlWareVer[2],
			                      mxlWareVer[3]));

			// We need read both 32 bit data at one time.
			status = MXL683_I2C_Read(devId, DMD_ID_VERSION_REG - 1, &lsbRegData);
			// Read low 16 bit , bit<15-12>: chip version, Bit<1:0>: Chip ID
			status |= MXL683_I2C_Read(devId, DMD_ID_VERSION_REG, &lsbRegData);

			if (MXL_SUCCESS == status)
			{
				// 0x02 is MxL683, 0x03 is MxL681
				*chipIdPtr = (lsbRegData & 0x03);
				*chipVersionPtr = (lsbRegData >> 12) & 0x0F;

				if (devContextPtr->firmwareStatus == MXL68x_FW_STATUS_LOADED)
				{
					// Means firmware has been loaded success before, get version information by FW communication
					status |= Ctrl_GetDeviceResp(devId, HOST_CMD_DEV_VERSION_INFO_REQ, &devVerInfoRsp);

					if ((status == MXL_SUCCESS) &&
					(devVerInfoRsp.errorCode == RESPONSE_ID_SUCCESS) &&
					(devVerInfoRsp.payloadLen == MXL68X_VERSION_SIZE * 2))
					{
						CH_DI_Print(CH_LOG_4, ("[MXL683] MxLWare68x_API_ReqDevVersionInfo() Received FW version info from Firmware successfully \n"));

						// Response Data[0:3] : Firmware version
						for (k = 0; k < MXL68X_VERSION_SIZE; k++)
							firmwareVer[k] = devVerInfoRsp.data[k];

						// Response Data[4:7] : Boot loader version
						for (k = 0; k < MXL68X_VERSION_SIZE; k++)
							bootloaderVer[k] = devVerInfoRsp.data[k + MXL68X_VERSION_SIZE];

						status |= MXL683_I2C_Read(devId, MAILBOX_REG_FIRMWARE_RC_VER, &lsbRegData);
						//firmwareVer[MXL68X_VERSION_SIZE-1] = (lsbRegData & 0xFF);
						CH_DI_Print(CH_LOG_0, ("[MXL683] MxLWare68x_API_ReqDevVersionInfo() FW version : %u.%u.%u.%u-RC%d \n", firmwareVer[0], firmwareVer[1], firmwareVer[2], firmwareVer[3], lsbRegData));
						CH_DI_Print(CH_LOG_4, ("[MXL683] MxLWare68x_API_ReqDevVersionInfo() BootLoader version: 0x%x.0x%x.0x%x.0x%x\n",
						bootloaderVer[0], bootloaderVer[1],bootloaderVer[2], bootloaderVer[3]));
					}
				}
			}
		}
		else
		{
			status = MXL_NOT_INITIALIZED;
			CH_DI_Print(CH_LOG_0, ("[MXL683] MxLWare68x_API_ReqDevVersionInfo() MXL_NOT_INITIALIZED \n"));
		}
	}
	else
	{
		status = MXL_INVALID_PARAMETER;
		CH_DI_Print(CH_LOG_0, ("[MXL683] MxLWare68x_API_ReqDevVersionInfo() MXL_INVALID_PARAMETER \n"));
	}

	CH_DI_Print(CH_LOG_4, ("[MXL683] MxLWare68x_API_ReqDevVersionInfo() chid : %d, status = %d \n",devId,status));

	return(MXL_STATUS_E)status;
}

MXL_STATUS_E MxLWare68x_API_ReqDevGPIOStatus(
                int devId,
                MXL68x_GPIO_ID_E gpioId,
                MXL68x_GPIO_TYPE_E  *gpioTypePtr,
                MXL68x_GPIO_STATE_E *gpioStatePtr)
{
	HUINT32 regValue, gpioPos;
	HUINT16 regData = 0;
	HUINT8 status = MXL_SUCCESS;
	HINT32	nUnitId;

	nUnitId = GetDeviceUnitId( devId );

	PMXL68x_DEV_CONTEXT_T devContextPtr = MxL68x_Ctrl_GetDeviceContext(nUnitId);

	if (devContextPtr)
	{
		if (devContextPtr->driverStatus)
		{
			// Read high 16 bit firstly
			status |= MXL683_I2C_Read(devId, DMD_GPIO_FUNC_MODE_REG - 1, &regData);
			regValue = (HUINT32)regData << 16;

			// Read low 16 bit secondly
			status |= MXL683_I2C_Read(devId, DMD_GPIO_FUNC_MODE_REG, &regData);
			regValue = regValue | regData;

			// Direction bit location begin from bit<20> to bit<26>
			gpioPos = gpioId + 20 - 1;
			*gpioTypePtr = (MXL68x_GPIO_TYPE_E)((regValue >> gpioPos) & 0x01);

			gpioPos = gpioId + 13 - 1;
			*gpioStatePtr = (MXL68x_GPIO_STATE_E)((regValue >> gpioPos) & 0x01);
		}
		else
		{
			status = MXL_NOT_INITIALIZED;
			CH_DI_Print(CH_LOG_0, ("[MXL683] MxLWare68x_API_ReqDevGPIOStatus() MXL_NOT_INITIALIZED \n"));
		}
	}
	else
	{
		status = MXL_INVALID_PARAMETER;
		CH_DI_Print(CH_LOG_0, ("[MXL683] MxLWare68x_API_ReqDevGPIOStatus() MXL_INVALID_PARAMETER \n"));
	}

	CH_DI_Print(CH_LOG_4, ("[MXL683] MxLWare68x_API_ReqDevGPIOStatus() chid : %d, status = %d \n",devId,status));

	return(MXL_STATUS_E)status;
}


MXL_STATUS_E MxLWare68x_API_ReqDevIntrStatus(
                int devId,
                HUINT8* intrStatusPtr)
{
	HUINT16 readBack = 0;
	HUINT8 status = MXL_SUCCESS;
	HINT32	nUnitId;

	nUnitId = GetDeviceUnitId( devId );

	PMXL68x_DEV_CONTEXT_T devContextPtr = MxL68x_Ctrl_GetDeviceContext(nUnitId);


	if (devContextPtr)
	{
		if (devContextPtr->driverStatus)
		{
			status = MXL683_I2C_Read(devId, DMD_INTERRUPT_MAILBOX_REG, &readBack);
			*intrStatusPtr = (HUINT8)readBack;
		}
		else
		{
			status = MXL_NOT_INITIALIZED;
			CH_DI_Print(CH_LOG_0, ("[MXL683] MxLWare68x_API_ReqDevIntrStatus() MXL_NOT_INITIALIZED \n"));
		}
	}
	else
	{
		status = MXL_INVALID_PARAMETER;
		CH_DI_Print(CH_LOG_0, ("[MXL683] MxLWare68x_API_ReqDevIntrStatus() MXL_INVALID_PARAMETER \n"));
	}

	CH_DI_Print(CH_LOG_4, ("[MXL683] MxLWare68x_API_ReqDevIntrStatus() chid : %d, status = %d \n",devId,status));
	return(MXL_STATUS_E)status;
}


MXL_STATUS_E MxLWare68x_API_CfgDemodBerPer(
                int devId,
                HUINT8 numFrames)
{
	HUINT16 regData = 0;
	HUINT8 status = MXL_SUCCESS;
	HINT32	nUnitId;

	nUnitId = GetDeviceUnitId( devId );

	PMXL68x_DEV_CONTEXT_T devContextPtr = MxL68x_Ctrl_GetDeviceContext(nUnitId);

	if (devContextPtr)
	{
		if (devContextPtr->driverStatus)
		{
			// Set BER measurement period, bit<8:0> of DMD_RS_BER_PER_FRAME
			status = MXL683_I2C_Read(devId, DMD_RS_BER_PER_FRAME, &regData);
			regData = (regData & 0xFF00) | (numFrames & 0x00FF);
			status |= MXL683_I2C_Write(devId, DMD_RS_BER_PER_FRAME, regData);
			status |= MXL683_I2C_Write(devId, DMD_RS_BER_PER_FRAME_MAILBOX_REG, regData);
			CH_DI_Print(CH_LOG_4, ("\n[MXL683] JAMES...MxLWare68x_API_CfgDemodBerPer()  DMD_RS_BER_PER_FRAME_MAILBOX_REG !!! \n"));

			regData = 0;

			// Start Ber stat is moved from BER measurement (toggle 0 to 1)
			status |= MXL683_I2C_Read(devId, DMD_RS_BER_PER_START, &regData);
			regData &= 0xFFFE; // Bit<0> = 0
			status |= MXL683_I2C_Write(devId, DMD_RS_BER_PER_START, regData);
			regData |= 0x01; // Bit<0> = 1
			status |= MXL683_I2C_Write(devId, DMD_RS_BER_PER_START, regData);
		}
		else
		{
			status = MXL_NOT_INITIALIZED;
			CH_DI_Print(CH_LOG_0, ("[MXL683] MxLWare68x_API_CfgDemodBerPer() MXL_NOT_INITIALIZED \n"));
		}
	}
	else
	{
		status = MXL_INVALID_PARAMETER;
		CH_DI_Print(CH_LOG_0, ("[MXL683] MxLWare68x_API_CfgDemodBerPer() MXL_INVALID_PARAMETER \n"));
	}

	CH_DI_Print(CH_LOG_4, ("[MXL683] MxLWare68x_API_CfgDemodBerPer() chid : %d, status = %d \n",devId,status));
	return(MXL_STATUS_E)status;
}


MXL_STATUS_E MxLWare68x_API_ReqDemodBerPerEndFlag(
                int devId,
                HUINT8 *numFramesPtr,
                MXL_BOOL_E *berEndFlagPtr)
{
	HUINT16 regData = 0;
	HUINT8 status = MXL_SUCCESS;
	HINT32	nUnitId;

	nUnitId = GetDeviceUnitId( devId );

	PMXL68x_DEV_CONTEXT_T devContextPtr = MxL68x_Ctrl_GetDeviceContext(nUnitId);

	if (devContextPtr)
	{
		if (devContextPtr->driverStatus)
		{
			status |= MXL683_I2C_Read(devId, DMD_RS_BER_PER_END_FLG, &regData);
			if(0x0001 & regData)//Bit<0>
			{
				*berEndFlagPtr = MXL_TRUE;
			}
			else
			{
				*berEndFlagPtr = MXL_FALSE;
			}


			regData = 0;

			// Get BER measurement period, bit<8:0> of DMD_RS_BER_PER_FRAME
			status |= MXL683_I2C_Read(devId, DMD_RS_BER_PER_FRAME, &regData);
			*numFramesPtr = 0x00FF & regData;
		}
		else
		{
			status = MXL_NOT_INITIALIZED;
			CH_DI_Print(CH_LOG_0, ("[MXL683] MxLWare68x_API_ReqDemodBerPerEndFlag() MXL_NOT_INITIALIZED \n"));
		}
	}
	else
	{
		status = MXL_INVALID_PARAMETER;
		CH_DI_Print(CH_LOG_0, ("[MXL683] MxLWare68x_API_ReqDemodBerPerEndFlag() MXL_INVALID_PARAMETER \n"));
	}

	CH_DI_Print(CH_LOG_4, ("[MXL683] MxLWare68x_API_ReqDemodBerPerEndFlag() chid : %d, status = %d \n",devId,status));
	return(MXL_STATUS_E)status;
}

MXL_STATUS_E MxLWare68x_API_CfgDemodClearBerPerEndFlag(
                int devId)
{
	HUINT16 regData = 0;
	HUINT8 status = MXL_SUCCESS;
	HINT32	nUnitId;

	nUnitId = GetDeviceUnitId( devId );

	PMXL68x_DEV_CONTEXT_T devContextPtr = MxL68x_Ctrl_GetDeviceContext(nUnitId);

	if (devContextPtr)
	{
		if (devContextPtr->driverStatus)
		{
			status |= MXL683_I2C_Read(devId, DMD_RS_BER_PER_CLR, &regData);
			regData = regData | 0xFF00; // To clear set bit<15:8> to 1
			//Clear the BER PER end flag to restart another iteration of BER / PER calculation
			status |= MXL683_I2C_Write(devId, DMD_RS_BER_PER_CLR, regData);
		}
		else
		{
			status = MXL_NOT_INITIALIZED;
			CH_DI_Print(CH_LOG_0, ("[MXL683] MxLWare68x_API_CfgDemodClearBerPerEndFlag() MXL_NOT_INITIALIZED \n"));
		}
	}
	else
	{
		status = MXL_INVALID_PARAMETER;
		CH_DI_Print(CH_LOG_0, ("[MXL683] MxLWare68x_API_CfgDemodClearBerPerEndFlag() MXL_INVALID_PARAMETER \n"));
	}

	CH_DI_Print(CH_LOG_4, ("[MXL683] MxLWare68x_API_CfgDemodClearBerPerEndFlag() chid : %d, status = %d \n",devId,status));
	return(MXL_STATUS_E)status;
}


MXL_STATUS_E MxLWare68x_API_CfgDemodNullPacketFilter(
	               int devId,
	               MXL_BOOL_E enableLayerA,
												    MXL_BOOL_E enableLayerB,
												    MXL_BOOL_E enableLayerC,
												    MXL_BOOL_E enableSkipNullPacket)
{
	HUINT16 readBack, control;
	HUINT8 status = MXL_SUCCESS;
	HINT32	nUnitId;

	nUnitId = GetDeviceUnitId( devId );

	PMXL68x_DEV_CONTEXT_T devContextPtr = MxL68x_Ctrl_GetDeviceContext(nUnitId);

	if (devContextPtr)
	{
		if (devContextPtr->driverStatus)
		{
			// Read DMD_CNST_CTRL register, TS_Out setting field is on the bit<15:8>.
			status |= MXL683_I2C_Read(devId, DMD_CNST_CTRL, &readBack);
			control = (readBack >> 8) & 0x00FF; // Shift high 8 bit to low 8 bit position.

			if (enableSkipNullPacket == MXL_DISABLE) control |= 0x08; // Bit<3> = 1
			else control &= 0xF7; // Bit<3> = 0

			if (enableLayerA == MXL_DISABLE) control |= 0x04; // Bit<2> = 1
			else control &= 0xFB; // Bit<2> = 0

			if (enableLayerB == MXL_DISABLE) control |= 0x02; // Bit<1> = 1
			else control &= 0xFD; // Bit<1> = 0

			if (enableLayerC == MXL_DISABLE) control |= 0x01; // Bit<0> = 1
			else control &= 0xFE; // Bit<0> = 0

			control = ((control << 8) & 0xFF00) | (readBack & 0x00FF); // Move TS_Out related parameter setting to high 8 bit position
			status |= MXL683_I2C_Write(devId, DMD_CNST_CTRL, control);
		}
		else
		{
			status = MXL_NOT_INITIALIZED;
			CH_DI_Print(CH_LOG_0, ("[MXL683] MxLWare68x_API_CfgDemodNullPacketFilter() MXL_NOT_INITIALIZED \n"));
		}
	}
	else
	{
		status = MXL_INVALID_PARAMETER;
		CH_DI_Print(CH_LOG_0, ("[MXL683] MxLWare68x_API_CfgDemodNullPacketFilter() MXL_INVALID_PARAMETER \n"));
	}

	CH_DI_Print(CH_LOG_4, ("[MXL683] MxLWare68x_API_CfgDemodNullPacketFilter() chid : %d, status = %d \n",devId,status));
	return (MXL_STATUS_E)status;
}

MXL_STATUS_E MxLWare68x_API_CfgDemodRSDecIterDec(
	               int devId,
                MXL_BOOL_E enableIterativeDecoder,
                MXL_BOOL_E enableRsDecoder)
{
	HUINT16 regData = 0;
	HUINT8 status = MXL_SUCCESS;
	HINT32	nUnitId;

	nUnitId = GetDeviceUnitId( devId );

	PMXL68x_DEV_CONTEXT_T devContextPtr = MxL68x_Ctrl_GetDeviceContext(nUnitId);

	if (devContextPtr)
	{
		if (devContextPtr->driverStatus)
		{
			status |= MXL683_I2C_Read(devId, DMD_FEC_OUT_CTRL, &regData);

			// Bit<4> = 1, means before RS
			if (enableRsDecoder == MXL_DISABLE) regData |= 0x0010;
			else regData &= 0xFFEF; // Bit<4> = 0, default is with RS

			// DMD_FEC_ITER_CTRL is same address as DMD_FEC_OUT_CTRL
			// Bit<9:8> = 11B, means without interative decoding
			if (enableIterativeDecoder == MXL_DISABLE)  regData |= 0x0300;
			else regData &=  0xFCFF; // Bit<9:8> = 00B, default is has iterative decoding

			status |= MXL683_I2C_Write(devId, DMD_FEC_OUT_CTRL, regData);

		}
		else
		{
			status = MXL_NOT_INITIALIZED;
			CH_DI_Print(CH_LOG_0, ("[MXL683] MxLWare68x_API_CfgDemodRSDecIterDec() MXL_NOT_INITIALIZED \n"));
		}
	}
	else
	{
		status = MXL_INVALID_PARAMETER;
		CH_DI_Print(CH_LOG_0, ("[MXL683] MxLWare68x_API_CfgDemodRSDecIterDec() MXL_INVALID_PARAMETER \n"));
	}

	CH_DI_Print(CH_LOG_4, ("[MXL683] MxLWare68x_API_CfgDemodRSDecIterDec() chid : %d, status = %d \n",devId,status));
	return(MXL_STATUS_E)status;
}


MXL_STATUS_E MxLWare68x_API_CfgDemodMPEGOut(
                int devId,
                MXL68x_MPEG_MODE_E mpegMode,
                MXL68x_MPEG_CLK_FREQ_E mpegClkFreq,
                MXL68x_MPEG_DATA_FMT_E mpegDataFmt,
                MXL68x_MPEG_CLK_PHASE_E mpegClkPhase,
                MXL68x_MPEG_POLARITY_E mpegSyncPol,
                MXL68x_MPEG_POLARITY_E mpegValidPol,
                MXL68x_MPEG_SYNC_PULSE_E mpegSyncPulseWidth,
                MXL68x_MPEG_ERROR_IND_E mpegErrIndicator,
		MXL68x_TS_PKT_SIZE_E tsPktSize,
		MXL68x_TS_DRV_STR_E tsDriveStrength)
{
	HUINT8 status = MXL_SUCCESS;
	HUINT16 regData = 0;
	HUINT16 tmpData = 0;
	HINT32	nUnitId;

	nUnitId = GetDeviceUnitId( devId );

	PMXL68x_DEV_CONTEXT_T devContextPtr = MxL68x_Ctrl_GetDeviceContext(nUnitId);

	if (devContextPtr)
	{
		if (devContextPtr->driverStatus)
		{
			status |= MXL683_I2C_Read(devId, MPEG_OUT_CFG_REG, &regData);

			// Configure Normal or 3-Wire mode
			if (mpegMode == MXL68x_MPEG_MODE_NORMAL) regData &= 0xFFFB; // Bit<2> = 0
			else if (mpegMode == MXL68x_MPEG_MODE_3WIRE) regData |= 0x0004; // Bit<2> = 1

			// Configure MPEG clock freq & CLK rate configuration
			status |= MXL683_I2C_Read(devId, MPEG_CLK_CFG_REG, &tmpData);

			if (mpegClkFreq == MXL68x_MPEG_CLK_64MHZ) tmpData &= 0xFFFE; // Bit<0> = 0
			else if ((mpegClkFreq == MXL68x_MPEG_CLK_32MHZ) || (mpegClkFreq == MXL68x_MPEG_CLK_16MHZ))
			tmpData |= 0x0001; // Bit<0> = 1

			status |= MXL683_I2C_Write(devId, MPEG_CLK_CFG_REG, tmpData);

			// Set Clock frequency in bit<1:0>
			regData &= 0xFFFC; // Clear bit<1:0>
			if (mpegClkFreq != MXL68x_MPEG_CLK_NA) regData |= (HUINT16)(mpegClkFreq);

			// Config MPEG VALID polarity <8>
			if (mpegValidPol == MXL68x_POLARITY_NORMAL) regData &= 0xFEFF;
			else if (mpegValidPol == MXL68x_POLARITY_INVERTED) regData |= 0x0100;

			// Config MPEG SYNC polarity <6>
			if (mpegSyncPol == MXL68x_POLARITY_NORMAL) regData &= 0xFFBF; // Bit<6> = 0
			else if (mpegSyncPol == MXL68x_POLARITY_INVERTED) regData |= 0x0040;  // Bit<4> = 1

			// Config MPEG Data format <4>
			if (mpegDataFmt == MXL68x_MPEG_DATA_MSB_FIRST) regData &= 0xFFEF; // Bit<4> = 0
			else if (mpegDataFmt == MXL68x_MPEG_DATA_LSB_FIRST) regData |= 0x0010;  // Bit<4> = 1

			// Config TS Packet size <5>
			if (tsPktSize == MXL68x_TS_PKT_204_BYTES) regData &= 0xFFDF; // Bit<5> = 0
			else if(tsPktSize == MXL68x_TS_PKT_188_BYTES) regData |= 0x0020;  // Bit<5> = 1

			// Config MPEG SYNC pulse width <7>
			if (mpegSyncPulseWidth == MXL68x_MPEG_SYNC_PULSE_1BYTE) regData &= 0xFF7F; // Bit<7> = 0
			else if (mpegSyncPulseWidth == MXL68x_MPEG_SYNC_PULSE_1BIT) regData |= 0x0080;  // Bit<7> = 1

			// Config MPEG ERR Indicator <9>
			if (mpegErrIndicator == MXL68x_MPEG_ERROR_IND_ENABLE) regData &= 0xFDFF;  // Bit<9> = 0
			else if (mpegErrIndicator == MXL68x_MPEG_ERROR_IND_DISABLE) regData |= 0x0200; // Bit<9> = 1

			// Config MPEG clock polarity
			switch(mpegClkPhase)
			{
				case MXL68x_MPEG_CLK_PHASE_SHIFT_90_DEG:
				case MXL68x_MPEG_CLK_PHASE_SHIFT_180_DEG:
				regData &= 0xFFF7; // Bit<3> = 0
				break;

				case MXL68x_MPEG_CLK_PHASE_SHIFT_270_DEG:
				case MXL68x_MPEG_CLK_PHASE_SHIFT_0_DEG:
				regData |= 0x0008; // Bit<3> = 1
				break;

				default:
				break;
			}

			status |= MXL683_I2C_Write(devId, MPEG_OUT_CFG_REG, regData);
			status |= MXL683_I2C_Write(devId, MPEG_OUT_CFG_MAILBOX_REG, regData);
			CH_DI_Print(CH_LOG_4, ("\n[MXL683] JAMES...MxLWare68x_API_CfgDemodMPEGOut()  MPEG_OUT_CFG_MAILBOX_REG !!! \n"));

			// Config MPEG CLK Phase <7>
			status |= MXL683_I2C_Read(devId, MPEG_CLK_PHASE_REG-1, &regData);
			status |= MXL683_I2C_Read(devId, MPEG_CLK_PHASE_REG, &tmpData);

			switch(mpegClkPhase)
			{
				case MXL68x_MPEG_CLK_PHASE_SHIFT_0_DEG:
				case MXL68x_MPEG_CLK_PHASE_SHIFT_180_DEG:
				tmpData &= 0xFF7F; // Bit<7> = 0
				break;

				case MXL68x_MPEG_CLK_PHASE_SHIFT_270_DEG:
				case MXL68x_MPEG_CLK_PHASE_SHIFT_90_DEG:
				tmpData |= 0x0080; // Bit<7> = 1
				break;

				default:
				break;
			}

			status |= MXL683_I2C_Write(devId, MPEG_CLK_PHASE_REG - 1, regData);
			status |= MXL683_I2C_Write(devId, MPEG_CLK_PHASE_REG, tmpData);


			// Config MPEG TS Drive Strength
			status |= MXL683_I2C_Read(devId, MPEG_TS_DRV_STR_REG - 2, &regData);
			status |= MXL683_I2C_Read(devId, MPEG_TS_DRV_STR_REG, &tmpData);

			switch(tsDriveStrength)
			{
			  case MXL68x_TS_DRV_STR_1X:
				tmpData &= 0xFFC3; // 0x3009<5:2> = 0
				regData &= 0xFFC3; // 0x3007<5:2> = 0
				break;
			  case MXL68x_TS_DRV_STR_3X:
				tmpData &= 0xFFC3; // 0x3009<5:2> = 0
				regData |= 0x003C; // 0x3007<5:2> = 0xf
				break;
			  case MXL68x_TS_DRV_STR_5X:
				tmpData |= 0x003C; // 0x3009<5:2> = 0xf
				regData &= 0xFFC3; // 0x3007<5:2> = 0
				break;
			  case MXL68x_TS_DRV_STR_7X:
				tmpData |= 0x003C; // 0x3009<5:2> = 0xf
				regData |= 0x003C; // 0x3007<5:2> = 0xf
				break;
			  default:
				break;
			}

			status |= MXL683_I2C_Write(devId, MPEG_TS_DRV_STR_REG - 2, regData);
			status |= MXL683_I2C_Write(devId, MPEG_TS_DRV_STR_REG, tmpData);
			// Config MPEG TS Drive Strength end.
		}
		else
		{
			status = MXL_NOT_INITIALIZED;
			CH_DI_Print(CH_LOG_0, ("[MXL683] MxLWare68x_API_CfgDemodMPEGOut() MXL_NOT_INITIALIZED \n"));
		}
	}
	else
	{
		status = MXL_INVALID_PARAMETER;
		CH_DI_Print(CH_LOG_0, ("[MXL683] MxLWare68x_API_CfgDemodMPEGOut() MXL_INVALID_PARAMETER \n"));
	}

	CH_DI_Print(CH_LOG_4, ("[MXL683] MxLWare68x_API_CfgDemodMPEGOut() chid : %d, status = %d \n",devId,status));
	return(MXL_STATUS_E)status;
}


MXL_STATUS_E MxLWare68x_API_ReqDemodCNR(
                int devId,
                HUINT32* cnrRawPtr,
                HINT32* cnrPtr)
{
	HUINT8 status = MXL_SUCCESS;
	HUINT16 regData = 0;
	HUINT32 tmp = 0;
	HUINT32 cnrRaw = 0;
	HUINT32 cnY = 0;
	HUINT8 numOfBitsUsed = 0;
	HUINT32 cnR = 0;
	HOST_COMMAND_T dmdCnrRead;
	HINT32	nUnitId;

	nUnitId = GetDeviceUnitId( devId );

	PMXL68x_DEV_CONTEXT_T devContextPtr = MxL68x_Ctrl_GetDeviceContext(nUnitId);

	// LUT used while calculating CNR value
	HUINT32 cnrLut[32] =
	{    0,     0,  3010,  4771,  6021,  6990,  7782,  8451, \
	9031,  9542, 10000, 10414, 10792, 11139, 11461, 11761, \
	12041, 12304, 12553, 12788, 13010, 13222, 13424, 13617, \
	13802, 13979, 14150, 14314, 14472, 14624, 14771, 14914 \
	};

	if (devContextPtr)
	{
		if (devContextPtr->driverStatus)
		{
			*cnrPtr = 0 ;

			//Send Host Command to copy BER PER to mailbox registers
			FORM_CMD_HEADER(dmdCnrRead, HOST_CMD_DEMOD_CNR_RB, 0, 0);

			status |= Ctrl_SendHostCommand(devId, &dmdCnrRead);

			// Add 2 ms waiting time
			VK_TASK_Sleep(2);

			// Read bit error count before Iterative decoding
			status |= MXL683_I2C_Read(devId, DMD_CNR_RB_LO_MAILBOX, &regData);
			cnrRaw = regData; // Low 16 bit

			// High 16 bit, total 32 bits
			status |= MXL683_I2C_Read(devId, DMD_CNR_RB_HI_MAILBOX, &regData);
			cnrRaw += ((regData) << 16 );

			*cnrRawPtr = cnrRaw;

			if(cnrRaw == 0)
			{
				*cnrPtr = 0;

				status = MXL_FAILURE; // Means has issue !
			}
			else
			{
				cnrRaw = (cnrRaw < 340) ? 340 : cnrRaw;

				// Calculate number of bits used
				tmp = cnrRaw - 270;

				while (tmp)
				{
					numOfBitsUsed++;
					tmp = tmp >> 1;
				}

				// Cacluate cnR
				cnR = (numOfBitsUsed >= 5) ?  (numOfBitsUsed - 5) : 0;
				cnY = (cnrRaw - 270) >> cnR;
				cnY = (cnY < 1)? 1 : cnY;
				cnY = (cnY > 31)? 31: cnY;

				*cnrPtr = (3010 * (23 - cnR)) - 11500 - cnrLut[cnY];
			}
		}
		else
		{
			status = MXL_NOT_INITIALIZED;
			CH_DI_Print(CH_LOG_0, ("[MXL683] MxLWare68x_API_ReqDemodCNR() MXL_NOT_INITIALIZED \n"));
		}
	}
	else
	{
		status = MXL_INVALID_PARAMETER;
		CH_DI_Print(CH_LOG_0, ("[MXL683] MxLWare68x_API_ReqDemodCNR() MXL_INVALID_PARAMETER \n"));
	}

	CH_DI_Print(CH_LOG_4, ("[MXL683] MxLWare68x_API_ReqDemodCNR() chid : %d, status = %d \n",devId,status));
	return(MXL_STATUS_E)status;
}


MXL_STATUS_E MxLWare68x_API_ReqDemodLockStatus(
                int devId,
                MXL_BOOL_E *tmccLockPtr,
                MXL_BOOL_E *layerARSLockPtr,
                MXL_BOOL_E *layerBRSLockPtr,
                MXL_BOOL_E *layerCRSLockPtr)
{
	HUINT16 regData = 0;
	HUINT8 status = MXL_SUCCESS;
	HINT32	nUnitId;

	nUnitId = GetDeviceUnitId( devId );

	PMXL68x_DEV_CONTEXT_T devContextPtr = MxL68x_Ctrl_GetDeviceContext(nUnitId);

	if (devContextPtr)
	{
		if (devContextPtr->driverStatus)
		{
			// TMCC lock status
			if (tmccLockPtr)
			{
				status |= MXL683_I2C_Read(devId, DMD_STATUS, &regData);
				if (tmccLockPtr) *tmccLockPtr = (MXL_BOOL_E)((regData >> 2) & 0x01); // Bit <2>
			}

			// Layer A & B RS lock status
			if ((layerARSLockPtr) || (layerBRSLockPtr))
			{
				status |= MXL683_I2C_Read(devId, DMD_RS_LOCK_A_B, &regData);

				if (layerARSLockPtr) *layerARSLockPtr = (MXL_BOOL_E)((regData >> 14) & 0x01); // Bit<14>
				if (layerBRSLockPtr) *layerBRSLockPtr = (MXL_BOOL_E)((regData >> 15) & 0x01); // Bit<15>
			}

			// Layer C RS lock status
			if (layerCRSLockPtr)
			{
				status |= MXL683_I2C_Read(devId, DMD_RS_LOCK_C, &regData);
				*layerCRSLockPtr = (MXL_BOOL_E)(regData & 0x01); // Bit<0>
			}
		}
		else
		{
			status = MXL_NOT_INITIALIZED;
			CH_DI_Print(CH_LOG_0, ("[MXL683] MxLWare68x_API_ReqDemodLockStatus() MXL_NOT_INITIALIZED \n"));
		}
	}
	else
	{
		status = MXL_INVALID_PARAMETER;
		CH_DI_Print(CH_LOG_0, ("[MXL683] MxLWare68x_API_ReqDemodLockStatus() MXL_INVALID_PARAMETER \n"));
	}

	CH_DI_Print(CH_LOG_4, ("[MXL683] MxLWare68x_API_ReqDemodLockStatus() chid : %d, status = %d \n",devId,status));
	return(MXL_STATUS_E)status;
}


MXL_STATUS_E MxLWare68x_API_ReqDemodNullPacketFilter(
                int devId,
											 				MXL_BOOL_E* enableLayerAPtr,
												 			MXL_BOOL_E* enableLayerBPtr,
																MXL_BOOL_E* enableLayerCPtr,
																MXL_BOOL_E* enableSkipNullPacketPtr)
{
	HUINT16 readBack;
	HUINT8 status = MXL_SUCCESS;
	HINT32	nUnitId;

	nUnitId = GetDeviceUnitId( devId );

	PMXL68x_DEV_CONTEXT_T devContextPtr = MxL68x_Ctrl_GetDeviceContext(nUnitId);


	if (devContextPtr)
	{
		if (devContextPtr->driverStatus)
		{
			// Read DMD_CNST_CTRL firstly.
			status = MXL683_I2C_Read(devId, DMD_CNST_CTRL, &readBack);
			if (MXL_SUCCESS == status)
			{
				    readBack = (readBack >> 8) & 0x00FF;
				    *enableLayerCPtr = (readBack & 0x01)? MXL_DISABLE : MXL_ENABLE;   // Bit<0>
				    *enableLayerBPtr = ((readBack & 0x02) >> 1)? MXL_DISABLE : MXL_ENABLE; // Bit<1>
				    *enableLayerAPtr = ((readBack & 0x04) >> 2)? MXL_DISABLE : MXL_ENABLE; // Bit<2>
				    *enableSkipNullPacketPtr = ((readBack & 0x08) >> 3)? MXL_DISABLE : MXL_ENABLE; // Bit<3>
			}
			else
			{
				    *enableSkipNullPacketPtr = MXL_DISABLE;
				    *enableLayerAPtr = MXL_DISABLE;
				    *enableLayerBPtr = MXL_DISABLE;
				    *enableLayerCPtr = MXL_DISABLE;
			}
			CH_DI_Print(CH_LOG_4, ("[MXL683] MxLWare68x_API_ReqDemodNullPacketFilter() Skip NULL packet enable = %d, Layer A = %d, Layer B = %d, Layer C = %d \n",
					    *enableSkipNullPacketPtr, *enableLayerAPtr, *enableLayerBPtr, *enableLayerCPtr));
		}
		else
		{
			status = MXL_NOT_INITIALIZED;
			CH_DI_Print(CH_LOG_0, ("[MXL683] MxLWare68x_API_ReqDemodNullPacketFilter() MXL_NOT_INITIALIZED \n"));
		}
	}
	else
	{
		status = MXL_INVALID_PARAMETER;
		CH_DI_Print(CH_LOG_0, ("[MXL683] MxLWare68x_API_ReqDemodNullPacketFilter() MXL_INVALID_PARAMETER \n"));
	}

	return (MXL_STATUS_E)status;
}


MXL_STATUS_E MxLWare68x_API_ReqDemodRsIterSetting(
                int devId,
																MXL_BOOL_E* enableIterativeDecoder,
																MXL_BOOL_E* enableRsDecoder)
{
	HUINT16 readBack;
	HUINT8 status = MXL_SUCCESS;
	HINT32	nUnitId;

	nUnitId = GetDeviceUnitId( devId );

	PMXL68x_DEV_CONTEXT_T devContextPtr = MxL68x_Ctrl_GetDeviceContext(nUnitId);

	if (devContextPtr)
	{
		if (devContextPtr->driverStatus)
		{
			status = MXL683_I2C_Read(devId, DMD_FEC_OUT_CTRL, &readBack);

			// Check Bit<4> of DMD_FEC_OUT_CTRL register for RS decoder enable or disable setting
			if ((readBack & 0x10) == 0x10) *enableRsDecoder = MXL_DISABLE; // Without RS decoder
			else *enableRsDecoder = MXL_ENABLE; // With RS decoder

			// Check Bit<9:8> of DMD_FEC_OUT_CTRL for Iterative decoding
			if ((readBack & 0x0300) == 0x0300)
				*enableIterativeDecoder = MXL_DISABLE; // Without interative
			else if ((readBack & 0x0300) == 0)
				*enableIterativeDecoder = MXL_ENABLE;
			else
				*enableIterativeDecoder = MXL_DISABLE;

			CH_DI_Print(CH_LOG_4, ("[MXL683] MxLWare68x_API_ReqDemodRsIterSetting() RS decoder enable = %d, Iterative decoder enable = %d \n",
			*enableRsDecoder, *enableIterativeDecoder));
		}
		else
		{
			status = MXL_NOT_INITIALIZED;
			CH_DI_Print(CH_LOG_0, ("[MXL683] MxLWare68x_API_ReqDemodRsIterSetting() MXL_NOT_INITIALIZED \n"));
		}
	}
	else
	{
		status = MXL_INVALID_PARAMETER;
		CH_DI_Print(CH_LOG_0, ("[MXL683] MxLWare68x_API_ReqDemodRsIterSetting() MXL_INVALID_PARAMETER \n"));
	}

	return (MXL_STATUS_E)status;
}


MXL_STATUS_E MxLWare68x_API_ReqDemodMpegOutSet(
                int devId,
                MXL68x_MPEG_MODE_E *mpegModePtr,
                MXL68x_MPEG_CLK_FREQ_E *mpegClkFreqPtr,
                MXL68x_MPEG_DATA_FMT_E *mpegDataFmtPtr,
                MXL68x_MPEG_CLK_PHASE_E *mpegClkPhasePtr,
                MXL68x_MPEG_POLARITY_E *mpegSyncPolPtr,
                MXL68x_MPEG_POLARITY_E *mpegValidPolPtr,
                MXL68x_MPEG_SYNC_PULSE_E *mpegSyncPulseWidthPtr,
                MXL68x_MPEG_ERROR_IND_E *mpegErrIndicatorPtr,
		MXL68x_TS_PKT_SIZE_E *tsPktSizePtr,
		MXL68x_TS_DRV_STR_E *tsDriveStrengthPtr)
{
	HUINT16 readBack, clkRotate, clkInvert, tsDrvStr0, tsDrvStr1;
	HUINT8 status = MXL_SUCCESS;
	HINT32	nUnitId;

	nUnitId = GetDeviceUnitId( devId );

	PMXL68x_DEV_CONTEXT_T devContextPtr = MxL68x_Ctrl_GetDeviceContext(nUnitId);

	if (devContextPtr)
	{
		if (devContextPtr->driverStatus)
		{
			status = MXL683_I2C_Read(devId, MPEG_OUT_CFG_REG, &readBack);
			readBack = (readBack & 0x03FF);

			*mpegClkFreqPtr = (MXL68x_MPEG_CLK_FREQ_E)(readBack & 0x03); // Bit<1:0>
			*mpegModePtr = (MXL68x_MPEG_MODE_E)((readBack & 0x04) >> 2); // Bit<2>
			clkInvert = (readBack & 0x08) >> 3; // Bit<3>
			*mpegDataFmtPtr = (MXL68x_MPEG_DATA_FMT_E)((readBack & 0x10) >> 4); // Bit<4>
			*tsPktSizePtr = (MXL68x_TS_PKT_SIZE_E)((readBack & 0x20) >> 5); // Bit<5>
			*mpegSyncPolPtr = (MXL68x_MPEG_POLARITY_E)((readBack & 0x40) >> 6); // Bit<6>
			*mpegSyncPulseWidthPtr = (MXL68x_MPEG_SYNC_PULSE_E)((readBack & 0x80) >> 7); // Bit<7>
			*mpegValidPolPtr = (MXL68x_MPEG_POLARITY_E)((readBack & 0x100) >> 8); // Bit<8>
			*mpegErrIndicatorPtr = (MXL68x_MPEG_ERROR_IND_E)((readBack & 0x200) >> 9); // Bit<9>

			status |= MXL683_I2C_Read(devId, MPEG_CLK_PHASE_REG - 1, &readBack);
			status |= MXL683_I2C_Read(devId, MPEG_CLK_PHASE_REG, &readBack);
			clkRotate = (readBack & 0x0080) >> 7;

			if ((clkRotate == 0) && (clkInvert == 1))
				*mpegClkPhasePtr = MXL68x_MPEG_CLK_PHASE_SHIFT_0_DEG;
			else if ((clkRotate == 1) && (clkInvert == 0))
				*mpegClkPhasePtr = MXL68x_MPEG_CLK_PHASE_SHIFT_90_DEG;
			else if ((clkRotate == 0) && (clkInvert == 0))
				*mpegClkPhasePtr = MXL68x_MPEG_CLK_PHASE_SHIFT_180_DEG;
			else if ((clkRotate == 1) && (clkInvert == 1))
				*mpegClkPhasePtr = MXL68x_MPEG_CLK_PHASE_SHIFT_270_DEG;


			status |= MXL683_I2C_Read(devId, MPEG_TS_DRV_STR_REG - 2, &readBack);
			tsDrvStr1 = (readBack & 0x003c) >> 2;
			status |= MXL683_I2C_Read(devId, MPEG_TS_DRV_STR_REG, &readBack);
			tsDrvStr0 = (readBack & 0x003c) >> 2;

			if ((tsDrvStr0 == 0) && (tsDrvStr1 == 0))
			  *tsDriveStrengthPtr = MXL68x_TS_DRV_STR_1X;
			else if ((tsDrvStr0 == 0) && (tsDrvStr1 == 0xf))
			  *tsDriveStrengthPtr = MXL68x_TS_DRV_STR_3X;
			else if ((tsDrvStr0 == 0xf) && (tsDrvStr1 == 0))
			  *tsDriveStrengthPtr = MXL68x_TS_DRV_STR_5X;
			else if ((tsDrvStr0 == 0xf) && (tsDrvStr1 == 0xf))
			  *tsDriveStrengthPtr = MXL68x_TS_DRV_STR_7X;
			else  *tsDriveStrengthPtr = MXL68x_TS_DRV_STR_NA;

		}
		else
		{
			status = MXL_NOT_INITIALIZED;
			CH_DI_Print(CH_LOG_0, ("[MXL683] MxLWare68x_API_ReqDemodMpegOutSet() MXL_NOT_INITIALIZED \n"));
		}
	}
	else
	{
		status = MXL_INVALID_PARAMETER;
		CH_DI_Print(CH_LOG_0, ("[MXL683] MxLWare68x_API_ReqDemodMpegOutSet() MXL_INVALID_PARAMETER \n"));
	}

	CH_DI_Print(CH_LOG_4, ("[MXL683] MxLWare68x_API_ReqDemodMpegOutSet() chid : %d, status = %d \n",devId,status));

  return (MXL_STATUS_E)status;
}


MXL_STATUS_E MxLWare68x_API_ReqDemodBER(
                int devId,
                MXL68x_BER_T *BerPtr)
{
	HUINT8 status = MXL_SUCCESS;
	HUINT16 regData = 0;
	MXL_BOOL_E tmccLock;
	HUINT8 mode = 0;
	HUINT8 seg = 0;
	HUINT8 cr = 0;
	HUINT8 numCR = 0;
	HUINT8 denCR = 1;
	HUINT8 mod = 0;
	HUINT8 i = 0;
	HUINT32 ber_temp=0;
	HUINT8 selected_layer=0xff;
	MXL68x_ISDBT_MODE_E isdbtModeInfo;
	MXL68x_TMCC_INFO_T tmccInfo[MXL68x_ISDBT_LAYER_ALL];
	MXL68x_BER_T berInfoPtr[MXL68x_ISDBT_LAYER_ALL];
	HOST_COMMAND_T dmdBerperRead;
	HINT32	nUnitId;

	nUnitId = GetDeviceUnitId( devId );

	PMXL68x_DEV_CONTEXT_T devContextPtr = MxL68x_Ctrl_GetDeviceContext(nUnitId);

	for (i = 0; i < MXL68x_ISDBT_LAYER_ALL; i++)
		VK_memset(&berInfoPtr[i], 0x00, sizeof(MXL68x_BER_T));

	if (devContextPtr)
	{
		if (devContextPtr->driverStatus)
		{
			// validate input pointers
			if (BerPtr)
			{
				// Check TMCC lock status
				status |= MxLWare68x_API_ReqDemodLockStatus(devId, &tmccLock, NULL, NULL, NULL);

				CH_DI_Print(CH_LOG_4, ("[MXL683] MxLWare68x_API_ReqDemodBER() chid : %d, Check Demod Lock = %d \n",devId,tmccLock));

				if (tmccLock == MXL_ON)
				{
					// Check if BER cal done flag else BER value will be invalid
					status |= MXL683_I2C_Read(devId, DMD_RS_BER_PER_END_FLG, &regData);

					if (regData == 1)
					{

						// Get Modulation, CR, Mode & segment info for all layers
						status |= MxLWare68x_API_ReqDemodTMCCParams(devId,
						        NULL,
						        NULL,
						        &isdbtModeInfo,
						        &tmccInfo[MXL68x_ISDBT_LAYER_A],
						        &tmccInfo[MXL68x_ISDBT_LAYER_B],
						        &tmccInfo[MXL68x_ISDBT_LAYER_C]);


						if (status == MXL_SUCCESS)
						{
							for (i = 0; i < MXL68x_ISDBT_LAYER_ALL; i++)
							{
									// Get mode info
									if (isdbtModeInfo <= MXL68x_ISDBT_MODE3)
									{
										mode = (((HUINT8)isdbtModeInfo) << 1);
										mode = (mode==0) ? 1 : mode;
									}
									else
									{
										status = MXL_FAILURE;
									}

									// Get CR
									if (tmccInfo[i].codeRateInfo <= MXL68x_CR_7_8)
									{
										cr = (HUINT8)tmccInfo[i].codeRateInfo;
										Ctrl_AssignNumDenCR(&numCR, &denCR, cr);
									}
									else
									{
										status = MXL_FAILURE;
									}

									// Get modulation
									if (tmccInfo[i].modulationInfo <= MXL68x_MODULATION_QAM64) mod = (((HUINT8)tmccInfo[i].modulationInfo)<< 1);
									else status = MXL_FAILURE;

									// Get number of segments
									if (tmccInfo[i].numOfSegments) seg = tmccInfo[i].numOfSegments;
									else status = MXL_FAILURE;

									status |= MXL683_I2C_Read(devId, DMD_RS_BER_PER_FRAME, &regData);

									CH_DI_Print(CH_LOG_4, ("[MXL683] MxLWare68x_API_ReqDemodBER() chid : %d, layter%d: index %d, mode %d, seg %d, mod %d, numCR %d, denCR %d, frm %d\n",devId, i, mode, seg, mod, numCR, denCR, (regData & 0x0F)));

									if (seg >= 255)  // Means there is no signal on this layer
									{
										berInfoPtr[i].numOfBits = 0;
										berInfoPtr[i].bitErrCount_WO_Iter = 0;
										berInfoPtr[i].bitErrCount_W_Iter = 0;
									}
									else
									{
										// Calculate Number of Bits used for BER calculation
										berInfoPtr[i].numOfBits = (HUINT32)((96 * (mode) * (seg) * numCR * mod * 204 * (regData & 0xFF))/denCR);
										berInfoPtr[i].numOfBits = berInfoPtr[i].numOfBits;

										if(MXL68x_ISDBT_LAYER_A == i)
										{
											//Send Host Command to copy BER PER to mailbox registers
											FORM_CMD_HEADER(dmdBerperRead, HOST_CMD_DEMOD_LAYER_A_BER_RB, 0, 0);
										}
										else if(MXL68x_ISDBT_LAYER_B == i)
										{
											//Send Host Command to copy BER PER to mailbox registers
											FORM_CMD_HEADER(dmdBerperRead, HOST_CMD_DEMOD_LAYER_B_BER_RB, 0, 0);
										}
										else if(MXL68x_ISDBT_LAYER_C == i)
										{
											//Send Host Command to copy BER PER to mailbox registers
											FORM_CMD_HEADER(dmdBerperRead, HOST_CMD_DEMOD_LAYER_C_BER_RB, 0, 0);
										}

										status |= Ctrl_SendHostCommand(devId, &dmdBerperRead);

										// Add 1 ms waiting time
										VK_TASK_Sleep(2);

										// Read bit error count before Iterative decoding
										status |= MXL683_I2C_Read(devId, DMD_RS_BER_PER_DATA_LO_MAILBOX, &regData);
										berInfoPtr[i].bitErrCount_WO_Iter = regData; // Low 16 bit

										// High 8 bit, total 32 bits
										status |= MXL683_I2C_Read(devId, DMD_RS_BER_PER_DATA_HI_MAILBOX, &regData);
										berInfoPtr[i].bitErrCount_WO_Iter += ((regData) << 16 );

										// Read bit error count after Iterative decoding
										status |= MXL683_I2C_Read(devId, DMD_RS_BER_PER_ITER_DATA_LO_MAILBOX, &regData);
										berInfoPtr[i].bitErrCount_W_Iter = regData; // Low 16 bit

										status |= MXL683_I2C_Read(devId, DMD_RS_BER_PER_ITER_DATA_HI_MAILBOX, &regData);
										berInfoPtr[i].bitErrCount_W_Iter += ((regData) << 16 );

									}

									CH_DI_Print(CH_LOG_4, ("[MXL683] MxLWare68x_API_ReqDemodBER() chid : %d, Layer %d, total bit = %d,  BER Cnt_WO_Iter = %d, BER Cnt_W_Iter = %d , status = %d \n",
									devId,i, berInfoPtr[i].numOfBits, berInfoPtr[i].bitErrCount_WO_Iter, berInfoPtr[i].bitErrCount_W_Iter, status));
							}
						}
					}
					else
					{
						for (i = 0; i < MXL68x_ISDBT_LAYER_ALL; i++)
						{
							berInfoPtr[i].numOfBits = 0;
							berInfoPtr[i].bitErrCount_WO_Iter = 0;
							berInfoPtr[i].bitErrCount_W_Iter = 0;
						}
						status = MXL_NOT_READY;
						CH_DI_Print(CH_LOG_0, ("[MXL683] MxLWare68x_API_ReqDemodBER() 111 MXL_NOT_READY \n"));
					}
				}
				else
				{
					status = MXL_NOT_READY;
					CH_DI_Print(CH_LOG_0, ("[MXL683] MxLWare68x_API_ReqDemodBER() 222 MXL_NOT_READY \n"));
				}
			}
			else
			{
				status = MXL_INVALID_PARAMETER;
				CH_DI_Print(CH_LOG_0, ("[MXL683] MxLWare68x_API_ReqDemodBER() 111 MXL_INVALID_PARAMETER \n"));
			}
		}
		else
		{
			status = MXL_NOT_INITIALIZED;
			CH_DI_Print(CH_LOG_0, ("[MXL683] MxLWare68x_API_ReqDemodBER() MXL_NOT_INITIALIZED \n"));
		}
	}
	else
	{
		status = MXL_INVALID_PARAMETER;
		CH_DI_Print(CH_LOG_0, ("[MXL683] MxLWare68x_API_ReqDemodBER() 222 MXL_INVALID_PARAMETER \n"));
	}

//JAMES...calculating BER
	for (i = 0; i < MXL68x_ISDBT_LAYER_ALL; i++)
	{
		if (berInfoPtr[i].numOfBits != 0)
		{
			selected_layer = i;
			break;
		}
	}

	if (i == MXL68x_ISDBT_LAYER_ALL)
	{
		BerPtr->bitErrCount_WO_Iter = 0;
		BerPtr->bitErrCount_W_Iter = 0;
		BerPtr->numOfBits = 0;
	}
	else
	{
		for (i = 0; i < MXL68x_ISDBT_LAYER_ALL; i++)
		{
			if (berInfoPtr[i].numOfBits != 0)
			{
				if (ber_temp <= berInfoPtr[i].bitErrCount_WO_Iter)
				{
					selected_layer = i;
					ber_temp = berInfoPtr[i].bitErrCount_WO_Iter;
				}
			}
		}

		if (selected_layer != 0xff)
		{
			BerPtr->bitErrCount_WO_Iter = berInfoPtr[selected_layer].bitErrCount_WO_Iter;
			BerPtr->bitErrCount_W_Iter = berInfoPtr[selected_layer].bitErrCount_W_Iter;
			BerPtr->numOfBits = berInfoPtr[selected_layer].numOfBits;
			status = MXL_SUCCESS;
		}
	}

	CH_DI_Print(CH_LOG_4, ("[MXL683] MxLWare68x_API_ReqDemodBER() chid : %d, status = %d \n",devId,status));
	return(MXL_STATUS_E)status;
}


MXL_STATUS_E MxLWare68x_API_ReqDemodPER(
                int devId,
                MXL68x_PER_T *layerAPktErrCountPtr,
                MXL68x_PER_T *layerBPktErrCountPtr,
                MXL68x_PER_T *layerCPktErrCountPtr)
{
	HUINT8 status = MXL_SUCCESS;
	HUINT16 regData = 0;
	MXL_BOOL_E tmccLock;
	HUINT8 mode = 0;
	HUINT8 seg = 0;
	HUINT8 cr = 0;
	HUINT8 numCR = 0;
	HUINT8 denCR = 1;
	HUINT8 mod = 0;
	HUINT8 i = 0;
	MXL68x_ISDBT_MODE_E isdbtModeInfo;
	MXL68x_TMCC_INFO_T tmccInfo[MXL68x_ISDBT_LAYER_ALL];
	MXL68x_PER_T       *perInfoPtr[MXL68x_ISDBT_LAYER_ALL] ={NULL, NULL,NULL};
	HOST_COMMAND_T dmdBerperRead;
	HINT32	nUnitId;

	nUnitId = GetDeviceUnitId( devId );

	PMXL68x_DEV_CONTEXT_T devContextPtr = MxL68x_Ctrl_GetDeviceContext(nUnitId);

	if (devContextPtr)
	{
		if (devContextPtr->driverStatus)
		{
			// validate input pointers
			if ((layerAPktErrCountPtr) || (layerBPktErrCountPtr) || (layerCPktErrCountPtr))
			{
				// Check TMCC lock status
				status |= MxLWare68x_API_ReqDemodLockStatus(devId, &tmccLock, NULL, NULL, NULL);
				CH_DI_Print(CH_LOG_4, ("[MXL683] MxLWare68x_API_ReqDemodPER() chid : %d, Demod Lock = %d \n",devId,tmccLock));

				if (tmccLock == MXL_ON)
				{
					// Check if PER cal done flag else PER value will be invalid
					status |= MXL683_I2C_Read(devId, DMD_RS_BER_PER_END_FLG, &regData);

					if (regData == 1)
					{

						// Get Modulation, CR, Mode & segment info for all layers
						status |= MxLWare68x_API_ReqDemodTMCCParams(devId,
						                NULL,
						                NULL,
						                &isdbtModeInfo,
						                &tmccInfo[MXL68x_ISDBT_LAYER_A],
						                &tmccInfo[MXL68x_ISDBT_LAYER_B],
						                &tmccInfo[MXL68x_ISDBT_LAYER_C]);


						if (status == MXL_SUCCESS)
						{
							perInfoPtr[MXL68x_ISDBT_LAYER_A] = layerAPktErrCountPtr;
							perInfoPtr[MXL68x_ISDBT_LAYER_B] = layerBPktErrCountPtr;
							perInfoPtr[MXL68x_ISDBT_LAYER_C] = layerCPktErrCountPtr;

							for (i = 0; i < MXL68x_ISDBT_LAYER_ALL; i++)
							{
								if (perInfoPtr[i])
								{
									// Get mode info
									if (isdbtModeInfo <= MXL68x_ISDBT_MODE3)
									{
										mode = (((HUINT8)isdbtModeInfo) << 1);
										mode = (mode==0) ? 1 : mode;
									}
									else
									{
										status = MXL_FAILURE;
									}

									// Get CR
									if (tmccInfo[i].codeRateInfo <= MXL68x_CR_7_8)
									{
										cr = (HUINT8)tmccInfo[i].codeRateInfo;
										Ctrl_AssignNumDenCR(&numCR, &denCR, cr);
									}
									else
									{
										status = MXL_FAILURE;
									}

									// Get modulation
									if (tmccInfo[i].modulationInfo <= MXL68x_MODULATION_QAM64) mod = (((HUINT8)tmccInfo[i].modulationInfo) << 1);
									else status = MXL_FAILURE;

									// Get segment
									if (tmccInfo[i].numOfSegments) seg = tmccInfo[i].numOfSegments;
									else status = MXL_FAILURE;

									if (seg >= 255)  // Means there is no signal on this layer
									{
										perInfoPtr[i]->numOfPkts = 0;
										perInfoPtr[i]->pktErrCount_WO_Iter = 0;
										perInfoPtr[i]->pktErrCount_W_Iter = 0;
									}
									else
									{
										// retrieve number of frames
										status |= MXL683_I2C_Read(devId, DMD_RS_BER_PER_FRAME, &regData);

										// Calculate Number of Frames used for PER calculation
										perInfoPtr[i]->numOfPkts = (HUINT32)(96 * (mode) * (seg) * numCR * mod * (regData & 0xFF));
										perInfoPtr[i]->numOfPkts = (HUINT32)(perInfoPtr[i]->numOfPkts/(denCR*8));

										if(MXL68x_ISDBT_LAYER_A == i)
										{
										//Send Host Command to copy BER PER to mailbox registers
										FORM_CMD_HEADER(dmdBerperRead, HOST_CMD_DEMOD_LAYER_A_PER_RB, 0, 0);
										}
										if(MXL68x_ISDBT_LAYER_B == i)
										{
										//Send Host Command to copy BER PER to mailbox registers
										FORM_CMD_HEADER(dmdBerperRead, HOST_CMD_DEMOD_LAYER_B_PER_RB, 0, 0);
										}
										if(MXL68x_ISDBT_LAYER_C == i)
										{
										//Send Host Command to copy BER PER to mailbox registers
										FORM_CMD_HEADER(dmdBerperRead, HOST_CMD_DEMOD_LAYER_C_PER_RB, 0, 0);
										}

										status |= Ctrl_SendHostCommand(devId, &dmdBerperRead);

										// Add 1 ms waiting time
										VK_TASK_Sleep(2);

										// Read bit error count before Iterative decoding
										status |= MXL683_I2C_Read(devId, DMD_RS_BER_PER_DATA_LO_MAILBOX, &regData);
										perInfoPtr[i]->pktErrCount_WO_Iter = regData; // Low 16 bit

										// High 8 bit, total 32 bits
										status |= MXL683_I2C_Read(devId, DMD_RS_BER_PER_DATA_HI_MAILBOX, &regData);
										perInfoPtr[i]->pktErrCount_WO_Iter += ((regData & 0xFF) << 16 );

										// Read bit error count after Iterative decoding
										status |= MXL683_I2C_Read(devId, DMD_RS_BER_PER_ITER_DATA_LO_MAILBOX, &regData);
										perInfoPtr[i]->pktErrCount_W_Iter = regData; // Low 16 bit

										status |= MXL683_I2C_Read(devId, DMD_RS_BER_PER_ITER_DATA_HI_MAILBOX, &regData);
										perInfoPtr[i]->pktErrCount_W_Iter += ((regData & 0xFF) << 16 );
									}

									CH_DI_Print(CH_LOG_4, ("[MXL683] MxLWare68x_API_ReqDemodPER() chid : %d, Layer %d, Total PKT = %d, PER Cnt_WO_Iter = %d, PER Cnt_W_Iter = %d \n",
									devId,i, perInfoPtr[i]->numOfPkts, perInfoPtr[i]->pktErrCount_WO_Iter, perInfoPtr[i]->pktErrCount_W_Iter));
								}
							}
						}
					}
					else
					{
						for (i = 0; i < MXL68x_ISDBT_LAYER_ALL; i++)
						{
							if (perInfoPtr[i])
							{
								perInfoPtr[i]->numOfPkts = 0;
								perInfoPtr[i]->pktErrCount_WO_Iter = 0;
								perInfoPtr[i]->pktErrCount_W_Iter = 0;
							}
						}
						status = MXL_FAILURE;
						CH_DI_Print(CH_LOG_0, ("[MXL683] MxLWare68x_API_ReqDemodPER() MXL_FAILURE \n"));
					}
				}
				else
				{
					status = MXL_FAILURE;
					CH_DI_Print(CH_LOG_0, ("[MXL683] MxLWare68x_API_ReqDemodPER() MXL_FAILURE \n"));
				}
			}
			else
			{
				status = MXL_INVALID_PARAMETER;
				CH_DI_Print(CH_LOG_0, ("[MXL683] MxLWare68x_API_ReqDemodPER() MXL_INVALID_PARAMETER \n"));
			}

		}
		else
		{
			status = MXL_NOT_INITIALIZED;
			CH_DI_Print(CH_LOG_0, ("[MXL683] MxLWare68x_API_ReqDemodPER() MXL_NOT_INITIALIZED \n"));
		}
	}
	else
	{
		status = MXL_INVALID_PARAMETER;
		CH_DI_Print(CH_LOG_0, ("[MXL683] MxLWare68x_API_ReqDemodPER() MXL_INVALID_PARAMETER \n"));
	}

	CH_DI_Print(CH_LOG_4, ("[MXL683] MxLWare68x_API_ReqDemodPER() chid : %d, status = %d \n",devId,status));
	return(MXL_STATUS_E)status;
}


MXL_STATUS_E MxLWare68x_API_ReqDemodTMCCParams(
                int devId,
                MXL_BOOL_E *ewsAlarmPtr,
                MXL68x_GUARD_INTERVAL_E *guardIntervalInfoPtr,
                MXL68x_ISDBT_MODE_E *isdbtModeInfoPtr,
                MXL68x_TMCC_INFO_T *tmccInfoLayerAPtr,
                MXL68x_TMCC_INFO_T *tmccInfoLayerBPtr,
                MXL68x_TMCC_INFO_T *tmccInfoLayerCPtr)
{
	HUINT8 status = MXL_SUCCESS;
	HUINT16 demodStatusReg0 = 0;
	HUINT16 demodStatusReg1 = 0;
	HUINT16 demodStatusReg2 = 0;
	HUINT16 demodStatusReg3 = 0;
	HUINT16 tmp = 0;
	MXL68x_TMCC_INFO_T *tmccInfo[MXL68x_ISDBT_LAYER_ALL];
	HUINT8 i = 0;
	HINT32	nUnitId;

	nUnitId = GetDeviceUnitId( devId );

	PMXL68x_DEV_CONTEXT_T devContextPtr = MxL68x_Ctrl_GetDeviceContext(nUnitId);

	MXL68x_TMCC_REG_MASK tmccRegMask[MXL68x_ISDBT_LAYER_ALL] =
	{
		{
			LAYER_A_MODULATION_MASK, LAYER_A_CR_MASK, LAYER_A_IL_MASK, LAYER_A_SEG_MASK,
			LAYER_A_MODULATION_BIT_SHIFT, LAYER_A_CR_BIT_SHIFT, LAYER_A_IL_BIT_SHIFT,
			LAYER_A_SEG_BIT_SHIFT
		},
		{
			LAYER_B_MODULATION_MASK, LAYER_B_CR_MASK, LAYER_B_IL_MASK, LAYER_B_SEG_MASK,
			LAYER_B_MODULATION_BIT_SHIFT, LAYER_B_CR_BIT_SHIFT, LAYER_B_IL_BIT_SHIFT,
			LAYER_B_SEG_BIT_SHIFT
		},
		{
			LAYER_C_MODULATION_MASK, LAYER_C_CR_MASK, LAYER_C_IL_MASK, LAYER_C_SEG_MASK,
			LAYER_C_MODULATION_BIT_SHIFT, LAYER_C_CR_BIT_SHIFT, LAYER_C_IL_BIT_SHIFT,
			LAYER_C_SEG_BIT_SHIFT
		}
	};

	if (devContextPtr)
	{
		if (devContextPtr->driverStatus)
		{
			status |= MXL683_I2C_Read(devId, DMD_TMCC_INFO_REG_0, &demodStatusReg0);

			// Emergency Alaram Signal status <2>
			if (ewsAlarmPtr)
			{
				if (demodStatusReg0 & 0x0004) *ewsAlarmPtr = MXL_ON;
				else *ewsAlarmPtr = MXL_OFF;
			}

			status |= MXL683_I2C_Read(devId, DMD_STATUS_INFO_REG, &tmp);

			// Guard Interval info <1:0>
			if (guardIntervalInfoPtr)
				*guardIntervalInfoPtr = (MXL68x_GUARD_INTERVAL_E)(tmp & 0x0003);

			// ISDB-T mode
			if (isdbtModeInfoPtr)
				*isdbtModeInfoPtr = (MXL68x_ISDBT_MODE_E)(((tmp & 0x000C) >> 2)-1);

			// TMCC information
			if ((tmccInfoLayerAPtr) || (tmccInfoLayerBPtr) || (tmccInfoLayerCPtr))
			{
				// Readback TMCC status registers
				status |= MXL683_I2C_Read(devId, DMD_TMCC_INFO_REG_1, &demodStatusReg1);
				status |= MXL683_I2C_Read(devId, DMD_TMCC_INFO_REG_2, &demodStatusReg2);
				status |= MXL683_I2C_Read(devId, DMD_TMCC_INFO_REG_3, &demodStatusReg3);

				tmccInfo[0] = tmccInfoLayerAPtr;
				tmccInfo[1] = tmccInfoLayerBPtr;
				tmccInfo[2] = tmccInfoLayerCPtr;

				for (i = 0; i <= MXL68x_ISDBT_LAYER_C; i++)
				{
					if (tmccInfo[i])
					{
						// Modulation information
						tmp = (demodStatusReg0 & tmccRegMask[i].modulationMask) >> tmccRegMask[i].modulationShift;
						if (tmp <= MXL68x_MODULATION_QAM64) tmccInfo[i]->modulationInfo = (MXL68x_MODULATION_E)tmp;
						else tmccInfo[i]->modulationInfo = MXL68x_MODULATION_UNKNOWN;

						// Code Rate information
						tmp = (demodStatusReg1 & tmccRegMask[i].codeRateMask) >> tmccRegMask[i].codeRateShift;
						if (tmp <= MXL68x_CR_7_8) tmccInfo[i]->codeRateInfo = (MXL68x_CODE_RATE_E)tmp;
						else tmccInfo[i]->codeRateInfo = MXL68x_CR_UNKNOWN;

						// Interleaver information
						if (i == MXL68x_ISDBT_LAYER_A)
							tmp = (demodStatusReg1 & tmccRegMask[i].interleaverMask) >> tmccRegMask[i].interleaverShift;
						else
							tmp = (demodStatusReg2 & tmccRegMask[i].interleaverMask) >> tmccRegMask[i].interleaverShift;

						if (tmp <= MXL68x_INTERLEAVER_INDEX3) tmccInfo[i]->interleaverLenInfo = (MXL68x_INTERLEAVER_LEN_E)tmp;
						else tmccInfo[i]->interleaverLenInfo = MXL68x_INTERLEAVER_UNKNOWN;

						// Number of Segments
						if (i == MXL68x_ISDBT_LAYER_C)
							tmp = (demodStatusReg3 & tmccRegMask[i].segmentMask) >> tmccRegMask[i].segmentShift;
						else
							tmp = (demodStatusReg2 & tmccRegMask[i].segmentMask) >> tmccRegMask[i].segmentShift;

						if ((tmp) && (tmp < 14)) tmccInfo[i]->numOfSegments = (HUINT8)tmp;
						else tmccInfo[i]->numOfSegments = 0xFF;

						CH_DI_Print(CH_LOG_4, ("[MXL683] MxLWare68x_API_ReqDemodTMCCParams() chid : %d, Layer %d : mod = %d, CR = %d, Interleave = %d, Segment = %d \n",
						devId,i, tmccInfo[i]->modulationInfo,tmccInfo[i]->codeRateInfo,tmccInfo[i]->interleaverLenInfo,tmccInfo[i]->numOfSegments));
					}
				}
			}
		}
		else
		{
			status = MXL_NOT_INITIALIZED;
			CH_DI_Print(CH_LOG_0, ("[MXL683] MxLWare68x_API_ReqDemodTMCCParams() MXL_NOT_INITIALIZED \n"));
		}
	}
	else
	{
		status = MXL_INVALID_PARAMETER;
		CH_DI_Print(CH_LOG_0, ("[MXL683] MxLWare68x_API_ReqDemodTMCCParams() MXL_INVALID_PARAMETER \n"));
	}

	CH_DI_Print(CH_LOG_4, ("[MXL683] MxLWare68x_API_ReqDemodTMCCParams() chid : %d, status = %d \n",devId,status));

	return(MXL_STATUS_E)status;
}


MXL_STATUS_E MxLWare68x_API_ReqDemodAcData(
	               int devId,
                HUINT8 *acDataPtr)
{
	HUINT16 k;
	HUINT8 status = MXL_SUCCESS;
	HINT32	nUnitId;

	nUnitId = GetDeviceUnitId( devId );

	PMXL68x_DEV_CONTEXT_T devContextPtr = MxL68x_Ctrl_GetDeviceContext(nUnitId);

	if (devContextPtr)
	{
		if (devContextPtr->driverStatus)
		{
			if (devContextPtr->fwIntRetData.ValidFlag == MXL_VALID)
			{
				// Read saved AC datsa to return parameter structure
				for (k = 0; k < devContextPtr->fwIntRetData.DataLength ; k++)
					acDataPtr[k] = devContextPtr->fwIntRetData.Data[k];

				devContextPtr->fwIntRetData.ValidFlag = MXL_INVALID;
			}
			else
			{
				status = MXL_FAILURE;
			}
		}
		else
		{
			status = MXL_NOT_INITIALIZED;
			CH_DI_Print(CH_LOG_0, ("[MXL683] MxLWare68x_API_ReqDemodAcData() MXL_NOT_INITIALIZED \n"));
		}
	}
	else
	{
		status = MXL_INVALID_PARAMETER;
		CH_DI_Print(CH_LOG_0, ("[MXL683] MxLWare68x_API_ReqDemodAcData() MXL_INVALID_PARAMETER \n"));
	}

	CH_DI_Print(CH_LOG_4, ("[MXL683] MxLWare68x_API_ReqDemodAcData() chid : %d, status = %d \n",devId,status));

	return (MXL_STATUS_E)status;
}


MXL_STATUS_E MxLWare68x_API_ReqDemodFreqOffset(
	               int devId,
	               HINT32 *carrierFreqOff,
	               HINT32 *samplingFreqOff)
{
	HUINT8 status = MXL_SUCCESS;
	HUINT16 regData = 0;
	HINT32	nUnitId;

	nUnitId = GetDeviceUnitId( devId );

	PMXL68x_DEV_CONTEXT_T devContextPtr = MxL68x_Ctrl_GetDeviceContext(nUnitId);

	if (devContextPtr)
	{
		if (devContextPtr->driverStatus)
		{
			status |= MXL683_I2C_Read(devId, DMD_CARRIER_FREQ_OFF_LO_MAILBOX, &regData);
			*carrierFreqOff = regData; // Low 16 bit

			status |= MXL683_I2C_Read(devId, DMD_CARRIER_FREQ_OFF_HI_MAILBOX, &regData);
			*carrierFreqOff += ((regData & 0xFFFF) << 16 );

			status |= MXL683_I2C_Read(devId, DMD_SAMP_FREQ_OFF_LO_MAILBOX, &regData);
			*samplingFreqOff = regData; // Low 16 bit

			status |= MXL683_I2C_Read(devId, DMD_SAMP_FREQ_OFF_HI_MAILBOX, &regData);
			*samplingFreqOff += ((regData & 0xFFFF) << 16 );
		}
		else
		{
			status = MXL_NOT_INITIALIZED;
			CH_DI_Print(CH_LOG_0, ("[MXL683] MxLWare68x_API_ReqDemodFreqOffset() MXL_NOT_INITIALIZED \n"));
		}
	}
	else
	{
		status = MXL_INVALID_PARAMETER;
		CH_DI_Print(CH_LOG_0, ("[MXL683] MxLWare68x_API_ReqDemodFreqOffset() MXL_INVALID_PARAMETER \n"));
	}

	CH_DI_Print(CH_LOG_4, ("[MXL683] MxLWare68x_API_ReqDemodAcData() chid : %d, status = %d \n",devId,status));

	return (MXL_STATUS_E) status;
}


MXL_STATUS_E MxLWare68x_API_ReqDemodFreqOffsetInKHzAndPPM(
	               int devId,
                HINT32 *carrierFreqOffInKHz,
                HINT32 *samplingFreqOffInPPM)
{
	HINT64 temp = 0;
	HINT32 carrierFreqOff,samplingFreqOff;
	HUINT16 regData = 0;
	HUINT8 status = MXL_SUCCESS;
	HINT32	nUnitId;

	nUnitId = GetDeviceUnitId( devId );

	PMXL68x_DEV_CONTEXT_T devContextPtr = MxL68x_Ctrl_GetDeviceContext(nUnitId);

	if (devContextPtr)
	{
		if (devContextPtr->driverStatus)
		{
			status |= MXL683_I2C_Read(devId, DMD_CARRIER_FREQ_OFF_LO_MAILBOX, &regData);
			carrierFreqOff = regData; // Low 16 bit

			status |= MXL683_I2C_Read(devId, DMD_CARRIER_FREQ_OFF_HI_MAILBOX, &regData);
			carrierFreqOff |= ((regData & 0xFFFF) << 16 );

			status |= MXL683_I2C_Read(devId, DMD_SAMP_FREQ_OFF_LO_MAILBOX, &regData);
			samplingFreqOff = regData; // Low 16 bit

			status |= MXL683_I2C_Read(devId, DMD_SAMP_FREQ_OFF_HI_MAILBOX, &regData);
			samplingFreqOff |= ((regData & 0xFFFF) << 16 );

			temp = (HINT64)(carrierFreqOff)*16000LL;
			temp = temp >> 28;
			*carrierFreqOffInKHz = (HINT32)(temp);

			temp = (HINT64)(samplingFreqOff & 0x3FFFFFFF); // Take only 30 bits
			temp = (temp * -507937LL) >> 30;
			*samplingFreqOffInPPM = (HINT32)(temp+492064LL);
		}
		else
		{
			status = MXL_NOT_INITIALIZED;
			CH_DI_Print(CH_LOG_0, ("[MXL683] MxLWare68x_API_ReqDemodFreqOffsetInKHzAndPPM() MXL_NOT_INITIALIZED \n"));
		}
	}
	else
	{
		status = MXL_INVALID_PARAMETER;
		CH_DI_Print(CH_LOG_0, ("[MXL683] MxLWare68x_API_ReqDemodFreqOffsetInKHzAndPPM() MXL_INVALID_PARAMETER \n"));
	}

	CH_DI_Print(CH_LOG_4, ("[MXL683] MxLWare68x_API_ReqDemodFreqOffsetInKHzAndPPM() chid : %d, status = %d \n",devId,status));
	return (MXL_STATUS_E) status;
}


MXL_STATUS_E MxLWare68x_API_ReqDemodSpectrumInversion(
                int devId,
                MXL_BOOL_E *isSpectrumInverted)
{
	HUINT8 status = MXL_SUCCESS;
	HUINT16 regData = 0;
	HINT32	nUnitId;

	nUnitId = GetDeviceUnitId( devId );

	PMXL68x_DEV_CONTEXT_T devContextPtr = MxL68x_Ctrl_GetDeviceContext(nUnitId);


	//Make Default false
	*isSpectrumInverted = MXL_FALSE;

	if (devContextPtr)
	{
		if (devContextPtr->driverStatus)
		{
			status |= MXL683_I2C_Read(devId, DFE_GCOMP_IQ_SWITCH, &regData);

			if(regData & (1<<7))//Bit 7 is IQ Swicth Indicator
			{
				*isSpectrumInverted = MXL_TRUE;
			}
		}
		else
		{
			status = MXL_NOT_INITIALIZED;
			CH_DI_Print(CH_LOG_0, ("[MXL683] MxLWare68x_API_ReqDemodSpectrumInversion() MXL_NOT_INITIALIZED \n"));
		}
	}
	else
	{
		status = MXL_INVALID_PARAMETER;
		CH_DI_Print(CH_LOG_0, ("[MXL683] MxLWare68x_API_ReqDemodSpectrumInversion() MXL_INVALID_PARAMETER \n"));
	}


	CH_DI_Print(CH_LOG_4, ("[MXL683] MxLWare68x_API_ReqDemodSpectrumInversion() chid : %d, status = %d \n",devId,status));
	return (MXL_STATUS_E) status;
}


MXL_STATUS_E MxLWare68x_API_CfgTunerIFOutParams(
                int devId,
                HUINT16 ifOutFreqInKHz,
                MXL_BOOL_E invertIFPolarity,
                HUINT8 gainLevel)
{
	HUINT8 status = MXL_SUCCESS;
	HUINT16 regData = 0;
	HUINT16 ifManualFreq;
	HUINT8 i = 0;
	HINT32	nUnitId;

	nUnitId = GetDeviceUnitId( devId );

	PMXL68x_DEV_CONTEXT_T devContextPtr = MxL68x_Ctrl_GetDeviceContext(nUnitId);

	HUINT16 presetIfFreq[MAX_PRESET_IF_FREQ][2] =
	{
		{ MXL68x_IF_FREQ_3650_KHZ, 0x00 },
		{ MXL68x_IF_FREQ_4000_KHZ, 0x01 },
		{ MXL68x_IF_FREQ_4100_KHZ, 0x02 },
		{ MXL68x_IF_FREQ_4150_KHZ, 0x03 },
		{ MXL68x_IF_FREQ_4500_KHZ, 0x04 },
		{ MXL68x_IF_FREQ_4570_KHZ, 0x05 },
		{ MXL68x_IF_FREQ_5000_KHZ, 0x06 },
		{ MXL68x_IF_FREQ_5380_KHZ, 0x07 },
		{ MXL68x_IF_FREQ_6000_KHZ, 0x08 },
		{ MXL68x_IF_FREQ_6280_KHZ, 0x09 },
		{ MXL68x_IF_FREQ_7200_KHZ, 0x0A },
		{ MXL68x_IF_FREQ_82500_KHZ, 0x0B },
		{ MXL68x_IF_FREQ_35250_KHZ, 0x0C },
		{ MXL68x_IF_FREQ_36000_KHZ, 0x0D },
		{ MXL68x_IF_FREQ_36150_KHZ, 0x0E },
		{ MXL68x_IF_FREQ_36650_KHZ, 0x0F },
		{ MXL68x_IF_FREQ_44000_KHZ, 0x10 }
	};

	CH_DI_Print(CH_LOG_4, ("[MXL683] MxLWare68x_API_CfgTunerIFOutParams() chid : %d, IF Out Freq = %d, Gain = %d, Invert = %d \n",
	ifOutFreqInKHz, gainLevel, invertIFPolarity));

	if (devContextPtr)
	{
		if (devContextPtr->driverStatus)
		{
			// Read back register for manual IF Out
			status = MXL683_I2C_Read(devId, IF_FREQ_SEL_REG, &regData);

			for (i = 0; i < MAX_PRESET_IF_FREQ; i++)
			{
				if (ifOutFreqInKHz == presetIfFreq[i][0]) break;
			}

			if (i == MAX_PRESET_IF_FREQ)
			{
				// IF out manual setting : bit<5>
				regData |= 0x0020;

				// Manual IF freq set
				ifManualFreq = (HUINT16)(ifOutFreqInKHz * 8192 / 216000);
				status |= MXL683_I2C_Write(devId, IF_FCW_LOW_REG, (ifManualFreq & 0xFF));
				status |= MXL683_I2C_Write(devId, IF_FCW_HIGH_REG, ((ifManualFreq >> 8) & 0x0F));

			}
			else
			{
				// bit<5> = 0, use IF frequency from IF frequency table
				regData &= 0xFFC0;

				// update register setting for preset IF freq
				regData |= presetIfFreq[i][1];
			}

			if (status == MXL_SUCCESS)
			{
				// Program pre-defined IF freq
				status |= MXL683_I2C_Write(devId, IF_FREQ_SEL_REG, regData);

				// Set spectrum invert, gain level and IF path
				// Spectrum invert indication is bit<7:6>
				if (MXL_ENABLE == invertIFPolarity) regData = 0x3 << 6;
				else regData = 0;

				// Gain level is bit<3:0> and set IF Path1 as output
				regData |= (gainLevel & 0x0F) | (0x01 << 4);
				status |= MXL683_I2C_Write(devId, IF_PATH_GAIN_REG, regData);

				// update Device context
				devContextPtr->ifGainLevel = gainLevel;
				devContextPtr->ifOutFreqInKHz = ifOutFreqInKHz;
			}
		}
		else
		{
			status = MXL_NOT_INITIALIZED;
			CH_DI_Print(CH_LOG_0, ("[MXL683] MxLWare68x_API_CfgTunerIFOutParams() MXL_NOT_INITIALIZED \n"));
		}
	}
	else
	{
		status = MXL_INVALID_PARAMETER;
		CH_DI_Print(CH_LOG_0, ("[MXL683] MxLWare68x_API_CfgTunerIFOutParams() MXL_INVALID_PARAMETER \n"));
	}

	CH_DI_Print(CH_LOG_4, ("[MXL683] MxLWare68x_API_CfgTunerIFOutParams() chid : %d, status = %d \n",devId,status));

	return(MXL_STATUS_E)status;
}


MXL_STATUS_E MxLWare68x_API_CfgTunerAGC(
                int devId,
                MXL68x_AGC_TYPE_E agcType,
                HUINT8 setPoint,
                MXL_BOOL_E agcPolarityInversion)
{
	HUINT8 status = MXL_SUCCESS;
	HUINT16 regData = 0;
	HINT32	nUnitId;

	nUnitId = GetDeviceUnitId( devId );

	PMXL68x_DEV_CONTEXT_T devContextPtr = MxL68x_Ctrl_GetDeviceContext(nUnitId);

	CH_DI_Print(CH_LOG_4, ("[MXL683] MxLWare68x_API_CfgTunerAGC() chid : %d, AGC type = %d, set point = %d, polarity = %d \n",
	devId,agcType, setPoint, agcPolarityInversion));

	if (devContextPtr)
	{
		if (devContextPtr->driverStatus)
		{
			// AGC selecton <3:2> and mode setting <0>
			status = MXL683_I2C_Read(devId, AGC_CONFIG_REG, &regData);
			regData &= 0xFFF2; // Clear bit<3:2> and bit<0>
			regData |= (HUINT16)((agcType << 2) | 0x01);  // Only has AGC1
			status |= MXL683_I2C_Write(devId, AGC_CONFIG_REG, regData);

			// AGC set point <6:0>
			status |= MXL683_I2C_Read(devId, AGC_SET_POINT_REG, &regData);
			regData &= 0xFF80; // Clear bit <6:0>
			regData |= setPoint;
			status |= MXL683_I2C_Write(devId, AGC_SET_POINT_REG, regData);

			// AGC Polarity <4>
			status |= MXL683_I2C_Read(devId, AGC_FLIP_REG, &regData);
			regData &= 0xFFEF; // Clear bit <4>
			regData |= (agcPolarityInversion << 4);
			status |= MXL683_I2C_Write(devId, AGC_FLIP_REG, regData);
		}
		else
		{
			status = MXL_NOT_INITIALIZED;
			CH_DI_Print(CH_LOG_0, ("[MXL683] MxLWare68x_API_CfgTunerAGC() MXL_NOT_INITIALIZED \n"));
		}
	}
	else
	{
		status = MXL_INVALID_PARAMETER;
		CH_DI_Print(CH_LOG_0, ("[MXL683] MxLWare68x_API_CfgTunerAGC() MXL_INVALID_PARAMETER \n"));
	}

	CH_DI_Print(CH_LOG_4, ("[MXL683] MxLWare68x_API_CfgTunerAGC() chid : %d, status = %d \n",devId,status));

	return(MXL_STATUS_E)status;
}


MXL_STATUS_E MxLWare68x_API_CfgTunerApplicationMode(
                int devId,
                MXL68x_SIGNAL_MODE_E signalMode)
{
	HUINT16 dfeRegData = 0;
	HUINT16 control1 = 0;
	HUINT16 control2 = 0;
	HUINT8 IfFreqSel = 1;  // Default is 4MHz IF for ISDB-T mode
	HUINT8 status = MXL_SUCCESS;
	REG_CTRL_INFO_T* tmpRegTable;
	HOST_COMMAND_T HostCommand;
	HINT32	nUnitId;

	nUnitId = GetDeviceUnitId( devId );

	PMXL68x_DEV_CONTEXT_T devContextPtr = MxL68x_Ctrl_GetDeviceContext(nUnitId);

	if (devContextPtr)
	{
		if (devContextPtr->driverStatus)
		{
			CH_DI_Print(CH_LOG_4, ("[MXL683] MxLWare68x_API_CfgTunerApplicationMode() chid : %d, Tuner Signal mode = %d \n",devId,signalMode));

			//UnclockGate CPU here
			status |= MXL683_I2C_Read(devId, DMD_GPIO_FUNC_MODE_REG-1, &control1);
			status |= MXL683_I2C_Read(devId, DMD_GPIO_FUNC_MODE_REG, &control2);
			status |= MXL683_I2C_Write(devId, DMD_GPIO_FUNC_MODE_REG-1, control1);
			status |= MXL683_I2C_Write(devId, DMD_GPIO_FUNC_MODE_REG, (control2 & 0xFFFD));

			// Change to XTAL clock before configuring signal type (App mode)
			// Switch to XTAL Clock
			// clkType = MXL68x_XTAL_CLOCK;
			status |= Ctrl_CfgDevSwitchClock(devId, MXL68x_XTAL_CLOCK);

			// Start Tune = 0
			status |= MXL683_I2C_Write(devId, START_TUNE_REG, MXL_DISABLE);

			// If firmware is active, then use FW host command for application mode settings
			//     Check if firmware has been downloaded and actived or not.
			devContextPtr->firmwareStatus = Ctrl_CheckFirmwareStatus(devId);
			if ((MXL68x_FW_STATUS_LOADED == devContextPtr->firmwareStatus))
			{
				switch(devContextPtr->ifOutFreqInKHz)
				{
					case MXL68x_IF_FREQ_3650_KHZ:   IfFreqSel = 0x00; break;
					case MXL68x_IF_FREQ_4000_KHZ:   IfFreqSel = 0x01; break;
					case MXL68x_IF_FREQ_4100_KHZ:   IfFreqSel = 0x02; break;
					case MXL68x_IF_FREQ_4150_KHZ:   IfFreqSel = 0x03; break;
					case MXL68x_IF_FREQ_4500_KHZ:   IfFreqSel = 0x04; break;
					case MXL68x_IF_FREQ_4570_KHZ:   IfFreqSel = 0x05; break;
					case MXL68x_IF_FREQ_5000_KHZ:   IfFreqSel = 0x06; break;
					case MXL68x_IF_FREQ_5380_KHZ:   IfFreqSel = 0x07; break;
					case MXL68x_IF_FREQ_6000_KHZ:   IfFreqSel = 0x08; break;
					case MXL68x_IF_FREQ_6280_KHZ:   IfFreqSel = 0x09; break;
					case MXL68x_IF_FREQ_7200_KHZ:   IfFreqSel = 0x0A; break;
					case MXL68x_IF_FREQ_82500_KHZ:  IfFreqSel = 0x0B; break;
					case MXL68x_IF_FREQ_35250_KHZ:  IfFreqSel = 0x0C; break;
					case MXL68x_IF_FREQ_36000_KHZ:  IfFreqSel = 0x0D; break;
					case MXL68x_IF_FREQ_36150_KHZ:  IfFreqSel = 0x0E; break;
					case MXL68x_IF_FREQ_36650_KHZ:  IfFreqSel = 0x0F; break;
					case MXL68x_IF_FREQ_44000_KHZ:  IfFreqSel = 0x10; break;
					default: status |= MXL_INVALID_PARAMETER; break;
				}

				dfeRegData = ((devContextPtr->xtalFreqInMHz & 0x01) << 13) + \
				((IfFreqSel & 0x1F) << 8) + \
				((devContextPtr->ifGainLevel & 0x0F) << 4) + \
				(signalMode & 0x0F);
				status |= MXL683_I2C_Write(devId, MXL68X_FW_MAIL_BOX_REG, dfeRegData);

				// Send command to FW
				FORM_CMD_HEADER(HostCommand, HOST_CMD_TUNER_APP_DEP_SETTING, 0, 0);
				HostCommand.payloadCheckSum = 0;
				if (MXL_FAILURE == (MXL_STATUS_E)Ctrl_SendHostCommand(devId, &HostCommand))
				status |= MXL_FAILURE;

				// Wait for 100 ms
				VK_TASK_Sleep(100);

				// check if firmware implementation is success or not
				status |= MXL683_I2C_Read(devId, MXL68X_FW_MAIL_BOX_REG, &dfeRegData);

				if (dfeRegData == 0xFFFF)
				{
					CH_DI_Print(CH_LOG_4, ("[MXL683] MxLWare68x_API_CfgTunerApplicationMode() chid : %d, FW Programs App Mode Settings \n",devId));
					// Clear the FW read Back register
					status |= MXL683_I2C_Write(devId, MXL68X_FW_MAIL_BOX_REG, 0x00);

					switch (signalMode)
					{
						case MXL68x_DIG_DVB_C:
						case MXL68x_DIG_J83B:
						//Disable TT Spur Fix  by writing into mailbox reg
						status |= MXL683_I2C_Write(devId, DFE_TT_SPUR_ENABLE, 0);

						//Make sure the CPU is Clock Gated for cable modes
						status |= MXL683_I2C_Read(devId, DMD_GPIO_FUNC_MODE_REG-1, &control1);
						status |= MXL683_I2C_Read(devId, DMD_GPIO_FUNC_MODE_REG, &control2);
						status |= MXL683_I2C_Write(devId, DMD_GPIO_FUNC_MODE_REG-1, control1);
						status |= MXL683_I2C_Write(devId, DMD_GPIO_FUNC_MODE_REG, (control2 | 0x0002));
						break;

						default:
						//Do Nothing as we just Un-Clock Gated the CPU
						break;
					}
				}
				else
				{
					CH_DI_Print(CH_LOG_4, ("[MXL683] MxLWare68x_API_CfgTunerApplicationMode() chid : %d, FW Fails to Program App Mode Settings  \n", devId));
				}
				// Update device context Signal mode
				devContextPtr->signalType = signalMode;
				devContextPtr->firmwareStatus = Ctrl_CheckFirmwareStatus(devId);

			}
			else  // Implement tuner application setting in host side
			{
				CH_DI_Print(CH_LOG_4, ("[MXL683] MxLWare68x_API_CfgTunerApplicationMode() chid : %d, FW is Either Clock Gated or Not Loaded  \n", devId));

				switch(signalMode)
				{
					case MXL68x_ANA_NTSC_MODE:

					status = Ctrl_ProgramRegisters(devId, MxL68x_AnalogNtsc);

					if (devContextPtr->ifOutFreqInKHz < MXL68x_IF_FREQ_35250_KHZ)
					{
						// Low power
#ifdef CUSTOMER_SPECIFIC_SETTING_1
						status |= MXL683_I2C_Write(devId, DIG_ANA_IF_CFG_0, 0xFE);
#else
						status |= MXL683_I2C_Write(devId, DIG_ANA_IF_CFG_0, 0x7E);
#endif
						status |= MXL683_I2C_Write(devId, DIG_ANA_IF_CFG_1, 0x10);

						if (devContextPtr->xtalFreqInMHz == MXL68x_XTAL_16MHz)
						{
							tmpRegTable = MxL68x_Ntsc_16MHzRfLutSwpLIF;
							dfeRegData = 0x00;
						}
						else
						{
							tmpRegTable = MxL68x_Ntsc_24MHzRfLutSwpLIF;
							dfeRegData = 0x01;
						}

						status |= MXL683_I2C_Write(devId, 0xDD, dfeRegData);
					}
					else
					{
						// High power
						status |= MXL683_I2C_Write(devId, DIG_ANA_IF_CFG_0, 0xD9);
						status |= MXL683_I2C_Write(devId, DIG_ANA_IF_CFG_1, 0x16);
						tmpRegTable = MxL68x_Ntsc_RfLutSwpHIF;
					}

					status |= Ctrl_ProgramRegisters(devId, tmpRegTable);
					break;

					case MXL68x_ANA_PAL_BG:
					tmpRegTable = MxL68x_AnalogPal;
					status = Ctrl_ProgramRegisters(devId, tmpRegTable);

					if (devContextPtr->ifOutFreqInKHz < MXL68x_IF_FREQ_35250_KHZ)
					{
						// Low power if settings
#ifdef CUSTOMER_SPECIFIC_SETTING_1
						status |= MXL683_I2C_Write(devId, DIG_ANA_IF_CFG_0, 0xFE);
#else
						status |= MXL683_I2C_Write(devId, DIG_ANA_IF_CFG_0, 0x7E);
#endif
						status |= MXL683_I2C_Write(devId, DIG_ANA_IF_CFG_1, 0x10);
						status |= Ctrl_ProgramRegisters(devId, MxL68x_Pal_RfLutSwpLIF);
					}
					else
					{
						// High power if settings
						status |= MXL683_I2C_Write(devId, DIG_ANA_IF_CFG_0, 0xD9);
						status |= MXL683_I2C_Write(devId, DIG_ANA_IF_CFG_1, 0x16);
					}
					break;

					case MXL68x_ANA_PAL_I:
					status |= MXL683_I2C_Write(devId, 0x0C, 0x0B);

					tmpRegTable = MxL68x_AnalogPal;
					status |= Ctrl_ProgramRegisters(devId, tmpRegTable);

					if (devContextPtr->ifOutFreqInKHz < MXL68x_IF_FREQ_35250_KHZ)
					{
						// Low power if settings
#ifdef CUSTOMER_SPECIFIC_SETTING_1
						status |= MXL683_I2C_Write(devId, DIG_ANA_IF_CFG_0, 0xFE);
#else
						status |= MXL683_I2C_Write(devId, DIG_ANA_IF_CFG_0, 0x7E);
#endif
						status |= MXL683_I2C_Write(devId, DIG_ANA_IF_CFG_1, 0x10);
						status |= Ctrl_ProgramRegisters(devId, MxL68x_Pal_RfLutSwpLIF);
					}
					else
					{
						// High power if settings
						status |= MXL683_I2C_Write(devId, DIG_ANA_IF_CFG_0, 0xD9);
						status |= MXL683_I2C_Write(devId, DIG_ANA_IF_CFG_1, 0x16);
						status |= Ctrl_ProgramRegisters(devId, MxL68x_PalI_RfLutSwpHIF);
					}
					break;

					case MXL68x_ANA_PAL_D:
					status |= MXL683_I2C_Write(devId, 0x0C, 0x0C);

					tmpRegTable = MxL68x_AnalogPal;
					status |= Ctrl_ProgramRegisters(devId, tmpRegTable);

					if (devContextPtr->ifOutFreqInKHz < MXL68x_IF_FREQ_35250_KHZ)
					{
						// Low power if settings
#ifdef CUSTOMER_SPECIFIC_SETTING_1
						status |= MXL683_I2C_Write(devId, DIG_ANA_IF_CFG_0, 0xFE);
#else
						status |= MXL683_I2C_Write(devId, DIG_ANA_IF_CFG_0, 0x7E);
#endif
						status |= MXL683_I2C_Write(devId, DIG_ANA_IF_CFG_1, 0x10);
						status |= Ctrl_ProgramRegisters(devId, MxL68x_Pal_RfLutSwpLIF);
					}
					else
					{
						// High power if settings
						status |= MXL683_I2C_Write(devId, DIG_ANA_IF_CFG_0, 0xD9);
						status |= MXL683_I2C_Write(devId, DIG_ANA_IF_CFG_1, 0x16);
						status |= Ctrl_ProgramRegisters(devId, MxL68x_PalD_RfLutSwpHIF);
					}
					break;

					case MXL68x_ANA_SECAM_I:

					status |= MXL683_I2C_Write(devId, 0x0C, 0x0B);
					tmpRegTable = MxL68x_AnalogSecam;
					status |= Ctrl_ProgramRegisters(devId, tmpRegTable);

					if (devContextPtr->ifOutFreqInKHz < MXL68x_IF_FREQ_35250_KHZ)
					{
						// Low power if settings
#ifdef CUSTOMER_SPECIFIC_SETTING_1
						status |= MXL683_I2C_Write(devId, DIG_ANA_IF_CFG_0, 0xFE);
#else
						status |= MXL683_I2C_Write(devId, DIG_ANA_IF_CFG_0, 0x7E);
#endif
						status |= MXL683_I2C_Write(devId, DIG_ANA_IF_CFG_1, 0x10);
					}
					else
					{
						// High power if settings
						status |= MXL683_I2C_Write(devId, DIG_ANA_IF_CFG_0, 0xD9);
						status |= MXL683_I2C_Write(devId, DIG_ANA_IF_CFG_1, 0x16);
					}
					break;

					case MXL68x_ANA_SECAM_L:
					status |= MXL683_I2C_Write(devId, 0x0C, 0x0C);

					tmpRegTable = MxL68x_AnalogSecam;
					status |= Ctrl_ProgramRegisters(devId, tmpRegTable);

					if (devContextPtr->ifOutFreqInKHz < MXL68x_IF_FREQ_35250_KHZ)
					{
						// Low power if settings
#ifdef CUSTOMER_SPECIFIC_SETTING_1
						status |= MXL683_I2C_Write(devId, DIG_ANA_IF_CFG_0, 0xFE);
#else
						status |= MXL683_I2C_Write(devId, DIG_ANA_IF_CFG_0, 0x7E);
#endif
						status |= MXL683_I2C_Write(devId, DIG_ANA_IF_CFG_1, 0x10);
					}
					else
					{
						// High power if settings
						status |= MXL683_I2C_Write(devId, DIG_ANA_IF_CFG_0, 0xD9);
						status |= MXL683_I2C_Write(devId, DIG_ANA_IF_CFG_1, 0x16);
					}
					break;

					case MXL68x_DIG_DVB_C:
					case MXL68x_DIG_J83B:
					tmpRegTable = MxL68x_DigitalDvbc;
					status |= Ctrl_ProgramRegisters(devId, tmpRegTable);

					if (devContextPtr->ifOutFreqInKHz < MXL68x_IF_FREQ_35250_KHZ)
					{
						// Low power if settings
						status |= MXL683_I2C_Write(devId, DIG_ANA_IF_CFG_0, 0xFE);
						status |= MXL683_I2C_Write(devId, DIG_ANA_IF_CFG_1, 0x10);
					}
					else
					{
						// High power if settings
						status |= MXL683_I2C_Write(devId, DIG_ANA_IF_CFG_0, 0xD9);
						status |= MXL683_I2C_Write(devId, DIG_ANA_IF_CFG_1, 0x16);
					}

					if (devContextPtr->xtalFreqInMHz == MXL68x_XTAL_16MHz) dfeRegData = 0x0D;
					else dfeRegData = 0x0E;

					status |= MXL683_I2C_Write(devId, DFE_CSF_SS_SEL, dfeRegData);

					break;

					case MXL68x_DIG_ATSC:
					tmpRegTable = MxL68x_DigitalIsdbtAtsc;
					status |= Ctrl_ProgramRegisters(devId, tmpRegTable);

					if (devContextPtr->ifOutFreqInKHz < MXL68x_IF_FREQ_35250_KHZ)
					{
						// Low power if settings
						status |= MXL683_I2C_Write(devId, DIG_ANA_IF_CFG_0, 0xF9);
						status |= MXL683_I2C_Write(devId, DIG_ANA_IF_CFG_1, 0x18);
						status |= MXL683_I2C_Write(devId, DIG_ANA_IF_PWR, 0xF1);
					}
					else
					{
						// High power if settings
						status |= MXL683_I2C_Write(devId, DIG_ANA_IF_CFG_0, 0xD9);
						status |= MXL683_I2C_Write(devId, DIG_ANA_IF_CFG_1, 0x16);
						status |= MXL683_I2C_Write(devId, DIG_ANA_IF_PWR, 0xB1);
					}

					if (MXL68x_XTAL_16MHz == devContextPtr->xtalFreqInMHz) dfeRegData = 0x0D;
					else if (MXL68x_XTAL_24MHz == devContextPtr->xtalFreqInMHz) dfeRegData = 0x0E;

					status |= MXL683_I2C_Write(devId, DFE_CSF_SS_SEL, dfeRegData);

					dfeRegData = 0;
					// IF gain based settings
					switch(devContextPtr->ifGainLevel)
					{
						case 0x09: dfeRegData = 0x44; break;
						case 0x08: dfeRegData = 0x43; break;
						case 0x07: dfeRegData = 0x42; break;
						case 0x06: dfeRegData = 0x41; break;
						case 0x05: dfeRegData = 0x40; break;
					}
					status |= MXL683_I2C_Write(devId, DFE_DACIF_GAIN, dfeRegData);

					break;

					case MXL68x_DIG_DVB_T:
					tmpRegTable = MxL68x_DigitalDvbt;
					status |= Ctrl_ProgramRegisters(devId, tmpRegTable);

					if (devContextPtr->ifOutFreqInKHz < MXL68x_IF_FREQ_35250_KHZ)
					{
						// Low power if settings
						status |= MXL683_I2C_Write(devId, DIG_ANA_IF_CFG_0, 0xFE);
						status |= MXL683_I2C_Write(devId, DIG_ANA_IF_CFG_1, 0x18);
						status |= MXL683_I2C_Write(devId, DIG_ANA_IF_PWR, 0xF1);
					}
					else
					{
						// High power if settings
						status |= MXL683_I2C_Write(devId, DIG_ANA_IF_CFG_0, 0xD9);
						status |= MXL683_I2C_Write(devId, DIG_ANA_IF_CFG_1, 0x16);
						status |= MXL683_I2C_Write(devId, DIG_ANA_IF_PWR, 0xB1);
					}

					if (MXL68x_XTAL_16MHz == devContextPtr->xtalFreqInMHz) dfeRegData = 0x0D;
					else if (MXL68x_XTAL_24MHz == devContextPtr->xtalFreqInMHz) dfeRegData = 0x0E;

					status |= MXL683_I2C_Write(devId, DFE_CSF_SS_SEL, dfeRegData);

					dfeRegData = 0;

					// IF gain based settings
					switch(devContextPtr->ifGainLevel)
					{
						case 0x09: dfeRegData = 0x44; break;
						case 0x08: dfeRegData = 0x43; break;
						case 0x07: dfeRegData = 0x42; break;
						case 0x06: dfeRegData = 0x41; break;
						case 0x05: dfeRegData = 0x40; break;
					}

					status |= MXL683_I2C_Write(devId, DFE_DACIF_GAIN, dfeRegData);

					break;

					default:
						status = MXL_INVALID_PARAMETER;
					break;
					}

				if (MXL_SUCCESS == status)
				{
					// XTAL calibration
					status |= MXL683_I2C_Write(devId, XTAL_CALI_SET_REG, 0x00);
					status |= MXL683_I2C_Write(devId, XTAL_CALI_SET_REG, 0x01);
					status |= MXL683_I2C_Write(devId, XTAL_CALI_SET_REG, 0x00);

					// 50 ms sleep after XTAL calibration
					VK_TASK_Sleep(50);

					// Update device context Signal mode
					devContextPtr->signalType = signalMode;
				}
			} // end of else (host implement tuner application mode setting)
		}
		else
			status = MXL_NOT_INITIALIZED;
	}
	else
		status = MXL_INVALID_PARAMETER;

	CH_DI_Print(CH_LOG_4, ("[MXL683] MxLWare68x_API_CfgTunerApplicationMode() chid : %d, status = %d \n",devId,status));
	return(MXL_STATUS_E)status;
}


MXL_STATUS_E MxLWare68x_API_CfgTunerChanTune(
                int devId,
                MXL68x_TUNE_TYPE_E tuneType,
                MXL68x_BW_E bandWidth,
                HUINT32 freqInHz,
                MXL68x_AUTO_SPEC_INV autoSpecInv,
                MXL_BOOL_E useChanTuneInt)
{
	HUINT32 frequency;
	HUINT32 freq = 0;
	HUINT16 regData = 0, fwXtalBwSettings = 0, tuneDone = 0, control = 0, ampRB = 0, ampLinRB = 0;
	HINT16 tempBias = 0;
	HUINT8 chanFound = 0, index_i = 0;
	HUINT16 tuneTimeOut = 0;
	HUINT8 status = MXL_SUCCESS;
	HOST_COMMAND_T chanTune;
	FW_RESPONSE_T getChanDataRsp;
	MXL68x_RESET_TYPE_E devReset;
	MXL68x_CHAN_DEP_FREQ_TABLE_T *freqLutPtr = NULL;
	MXL68x_NTSC_CARRIERS_E ntscCarrier = MXL68x_NTSC_CARRIER_NA;
	HINT32	nUnitId;

	nUnitId = GetDeviceUnitId( devId );

	PMXL68x_DEV_CONTEXT_T devContextPtr = MxL68x_Ctrl_GetDeviceContext(nUnitId);

	if (devContextPtr)
	{
		if (devContextPtr->driverStatus)
		{
			CH_DI_Print(CH_LOG_4, ("[MXL683] MxLWare68x_API_CfgTunerChanTune() chid : %d,ignal type = %d, Freq = %d, BW = %d, Xtal = %d, NTSC Carrier =%d  useChanTuneInt = %d\n",
			devId,devContextPtr->signalType, freqInHz, bandWidth, devContextPtr->xtalFreqInMHz, ntscCarrier, useChanTuneInt));
			// Disable Demod
			if (devContextPtr->devOpMode == MXL68x_TUNER_DEMOD)
				status |= Ctrl_CfgDemodEnable(devId, MXL_DISABLE);

			// Switch to XTAL Clock
			status |= Ctrl_CfgDevSwitchClock(devId, MXL68x_XTAL_CLOCK);

			// START_TUNE = 0
			status |= MXL683_I2C_Write(devId, START_TUNE_REG, MXL_DISABLE);

			status |= MXL683_I2C_Write(devId, DMD_CHAN_SCAN_TUNE_XTAL_REG, 0x0000); // set all bits to 0 (including AutoSpecInv)
			status |= MXL683_I2C_Write(devId, DMD_CHAN_SCAN_TUNE_FREQ_REG, 0x0000);

			if (freqInHz < 700000000)
			{
				// Program cable mode settings - IF related
				status |= MXL683_I2C_Write(devId, 0x7C, 0x1F);
				if ((devContextPtr->signalType == MXL68x_DIG_DVB_C) ||
				(devContextPtr->signalType == MXL68x_DIG_J83B))
				{
					regData = 0xC1;
				}
				else
				{
					regData = 0x81;
				}
			}
			else
			{
				// Program cable mode settings - IF related
				status |= MXL683_I2C_Write(devId, 0x7C, 0x9F);

				if ((devContextPtr->signalType == MXL68x_DIG_DVB_C) ||
				(devContextPtr->signalType == MXL68x_DIG_J83B))
				{
					regData = 0xD1;
				}
				else
				{
					regData = 0x91;
				}
			}
			// Program IF register
			status |= MXL683_I2C_Write(devId, 0x131, regData);

			// frequency dependent XTAL Bias Settings for all analog modes
			if(devContextPtr->signalType <= MXL68x_ANA_SECAM_L)
			{
				status |= MXL683_I2C_Read(devId, ANA_DIG_REFSX_XTAL_AMP_RB_REG, &ampRB);
				status |= MXL683_I2C_Read(devId, ANA_DIG_REFSX_XTAL_AMP_LIN_RB_REG, &ampLinRB);
				ampLinRB = (ampLinRB & 0x30) >> 4; //using only bits <5:4>



				if (freqInHz < 300000000)
				{
					tempBias = (ampRB + ampLinRB - 5);
					regData = (tempBias < 0) ? 0 : tempBias;
				}
				else
				{
					regData = (ampRB + ampLinRB);
				}
				status |= MXL683_I2C_Write(devId, DIG_ANA_XTAL_XTAL_EXT_BIAS, regData);

				CH_DI_Print(CH_LOG_4, ("[MXL683] MxLWare68x_API_CfgTunerChanTune() chid : %d, ampRB %d ampLinRB %d regData %d \n",devId,ampRB,ampLinRB,regData));

				regData = 0;
			}

			switch(devContextPtr->signalType)
			{
				// Signal type is NTSC mode
				case MXL68x_ANA_NTSC_MODE:

					// If 16MHz XTAL is used then need to program registers based on frequency value
					if (MXL68x_XTAL_16MHz == devContextPtr->xtalFreqInMHz)
					{
						if (devContextPtr->ifOutFreqInKHz < MXL68x_IF_FREQ_35250_KHZ)
							status |= Ctrl_ProgramRegisters(devId, MxL68x_Ntsc_16MHzRfLutSwpLIF);
						else
							status |= Ctrl_ProgramRegisters(devId, MxL68x_Ntsc_RfLutSwpHIF);

						if (MXL68x_NTSC_CARRIER_NA == ntscCarrier)
						{
							// Select spur shifting LUT to be used based NTSC, HRC or IRC frequency
							if ((freqInHz % 1000000) == 0)
							{
								// SPUR LUT based on IF freq
								if (devContextPtr->ifOutFreqInKHz < MXL68x_IF_FREQ_35250_KHZ)
									freqLutPtr = MXL68x_NTSC_LUT_XTAL_16MHZ_LIF;
								else
									freqLutPtr = MXL68x_NTSC_LUT_XTAL_16MHZ_HIF;
							}
							else if (((freqInHz - 12500) % 1000000) == 0) freqLutPtr = MXL68x_NTSC_LUT_IRC_16MHZ;
							else if (((freqInHz - 143000) % 1000000) == 0) freqLutPtr = MXL68x_PAL_MN_LUT;
							else
							{
								freqLutPtr = MXL68x_NTSC_LUT_HRC_16MHZ;
								status |= Ctrl_ProgramRegisters(devId, MxL68x_Ntsc_HRCRfLutSwpLIF);
							}
						}
					} // end of if (16 MHz XTAL
					else if (MXL68x_XTAL_24MHz == devContextPtr->xtalFreqInMHz)
					{
						if (devContextPtr->ifOutFreqInKHz < MXL68x_IF_FREQ_35250_KHZ)
							status |= Ctrl_ProgramRegisters(devId, MxL68x_Ntsc_24MHzRfLutSwpLIF);
						else
							status |= Ctrl_ProgramRegisters(devId, MxL68x_Ntsc_RfLutSwpHIF);

						if (MXL68x_NTSC_CARRIER_NA == ntscCarrier)
						{
							// Select spur shifting LUT to be used based NTSC, HRC or IRC frequency
							if ((freqInHz % 1000000) == 0) freqLutPtr = MXL68x_NTSC_LUT_XTAL_24MHZ;
							else if (((freqInHz - 12500) % 1000000) == 0) freqLutPtr = MXL68x_NTSC_LUT_IRC_24MHZ;
							else if (((freqInHz - 143000) % 1000000) == 0) freqLutPtr = MXL68x_PAL_MN_LUT;
							else
							{
								freqLutPtr = MXL68x_NTSC_LUT_HRC_24MHZ;
								status |= Ctrl_ProgramRegisters(devId, MxL68x_Ntsc_HRCRfLutSwpLIF);
							}
						}
					} // end of if (24 MHz XTAL

					// program spur lut
					if (freqLutPtr)
						status |= Ctrl_SetRfFreqLutReg(devId, freqInHz, freqLutPtr);

				break;

				// Signal type is PAL D
				case MXL68x_ANA_PAL_D:

					// If 16MHz XTAL is used then need to program registers based on frequency value
					if (MXL68x_XTAL_16MHz == devContextPtr->xtalFreqInMHz)
					{
					if (devContextPtr->ifOutFreqInKHz < MXL68x_IF_FREQ_35250_KHZ)
					freqLutPtr = MXL68x_PAL_D_LUT_XTAL_16MHZ_LIF;
					else freqLutPtr = MXL68x_PAL_D_LUT_XTAL_16MHZ_HIF;

					} // end of if (16 MHz XTAL
					else if (MXL68x_XTAL_24MHz == devContextPtr->xtalFreqInMHz)
					{
					freqLutPtr = MXL68x_PAL_D_LUT_XTAL_24MHZ;
					} // end of if (24 MHz XTAL

					// program spur lut
					if (freqLutPtr)
					status |= Ctrl_SetRfFreqLutReg(devId, freqInHz, freqLutPtr);

				break;

				case MXL68x_ANA_PAL_BG:

					switch(bandWidth)
					{
						case MXL68x_BW_7MHz:
						regData = 0x09;
						break;

						case MXL68x_BW_8MHz:
						regData = 0x0A;
						break;

						default:
						break;
					}
					// program dfe ana mode for 7 & 8 MHz BW
					if ((bandWidth == MXL68x_BW_7MHz) || (bandWidth == MXL68x_BW_8MHz))
						status |= MXL683_I2C_Write(devId, 0x0C, regData);

					// If 16MHz XTAL is used then need to program registers based on frequency value
					if (MXL68x_XTAL_16MHz == devContextPtr->xtalFreqInMHz)
					{
						// PAL - BG 7 MHz Frequency range
						if (MXL68x_BW_7MHz == bandWidth)
						{
							if (devContextPtr->ifOutFreqInKHz < MXL68x_IF_FREQ_35250_KHZ)
							{
								freqLutPtr = MXL68x_PAL_BG_7MHZ_LUT_XTAL_16MHZ_LIF;
								status |= Ctrl_ProgramRegisters(devId, MxL68x_Pal_RfLutSwpLIF);
							}
							else
							{
								freqLutPtr = MXL68x_PAL_BG_7MHZ_LUT_XTAL_16MHZ_HIF;
								status |= Ctrl_ProgramRegisters(devId, MxL68x_PalBG_7MHzBW_RfLutSwpHIF);
							}
						}
						else
						{
							if (devContextPtr->ifOutFreqInKHz < MXL68x_IF_FREQ_35250_KHZ)
							{
								freqLutPtr = MXL68x_PAL_BG_8MHZ_LUT_XTAL_16MHZ_LIF;
								status |= Ctrl_ProgramRegisters(devId, MxL68x_Pal_RfLutSwpLIF);
							}
							else
							{
								freqLutPtr = MXL68x_PAL_BG_8MHZ_LUT_XTAL_16MHZ_HIF;
								status |= Ctrl_ProgramRegisters(devId, MxL68x_PalBG_8MHzBW_RfLutSwpHIF);
							}
						}
					} // end of if (XTAL_16MHz
					else if (MXL68x_XTAL_24MHz == devContextPtr->xtalFreqInMHz)
					{
						// If 16MHz XTAL is used then need to program registers based on frequency value
						// PAL - BG 7 MHz Frequency range
						if (MXL68x_BW_7MHz == bandWidth)
						{
							freqLutPtr = MXL68x_PAL_BG_7MHZ_LUT_XTAL_24MHZ;

							if (devContextPtr->ifOutFreqInKHz < MXL68x_IF_FREQ_35250_KHZ)
								status |= Ctrl_ProgramRegisters(devId, MxL68x_Pal_RfLutSwpLIF);
							else
								status |= Ctrl_ProgramRegisters(devId, MxL68x_PalBG_7MHzBW_RfLutSwpHIF);
						}
						else
						{
							freqLutPtr = MXL68x_PAL_BG_8MHZ_LUT_XTAL_24MHZ;

							if (devContextPtr->ifOutFreqInKHz < MXL68x_IF_FREQ_35250_KHZ)
								status |= Ctrl_ProgramRegisters(devId, MxL68x_Pal_RfLutSwpLIF);
							else
								status |= Ctrl_ProgramRegisters(devId, MxL68x_PalBG_8MHzBW_RfLutSwpHIF);
						}
					} // end of if (XTAL_24MHz

					// program spur lut
					if (freqLutPtr)
						status |= Ctrl_SetRfFreqLutReg(devId, freqInHz, freqLutPtr);

				break;

				case MXL68x_ANA_PAL_I:

					// If 16MHz XTAL is used then need to program registers based on frequency value
					if (MXL68x_XTAL_16MHz == devContextPtr->xtalFreqInMHz)
					{
						if (devContextPtr->ifOutFreqInKHz < MXL68x_IF_FREQ_35250_KHZ)
							freqLutPtr = MXL68x_PAL_I_LUT_XTAL_16MHZ_LIF;
						else
							freqLutPtr = MXL68x_PAL_I_LUT_XTAL_16MHZ_HIF;
					}
					else
						freqLutPtr = MXL68x_PAL_I_LUT_XTAL_24MHZ;

					// program spur lut
					if (freqLutPtr)
						status |= Ctrl_SetRfFreqLutReg(devId, freqInHz, freqLutPtr);

				break;

				case MXL68x_ANA_SECAM_L:
					// If 16MHz XTAL is used then need to program registers based on frequency value
					if (MXL68x_XTAL_16MHz == devContextPtr->xtalFreqInMHz) freqLutPtr = MXL68x_SECAM_L_LUT_XTAL_16MHZ;
					else freqLutPtr = MXL68x_SECAM_L_LUT_XTAL_24MHZ;

					// program spur lut
					if (freqLutPtr)
						status |= Ctrl_SetRfFreqLutReg(devId, freqInHz, freqLutPtr);

				break;

				case MXL68x_DIG_DVB_C:
				case MXL68x_DIG_J83B:

					if ((bandWidth == MXL68x_BW_6MHz) || (bandWidth == MXL68x_BW_8MHz))
					{
						if (bandWidth == MXL68x_BW_6MHz) freqLutPtr = MXL68x_DIG_CABLE_LUT_BW_6MHZ;
						else freqLutPtr = MXL68x_DIG_CABLE_LUT_BW_8MHZ;

						// program dig mode related spur lut
						status |= Ctrl_SetRfFreqLutReg(devId, freqInHz, freqLutPtr);
					}
				break;

				case MXL68x_DIG_ATSC:
				case MXL68x_DIG_ISDBT:
				case MXL68x_DIG_DVB_T:
					if (bandWidth == MXL68x_BW_6MHz) freqLutPtr = MXL68x_DIG_TERR_LUT_BW_6MHZ;
					else if (bandWidth == MXL68x_BW_7MHz) freqLutPtr = MXL68x_DIG_TERR_LUT_BW_7MHZ;
					else freqLutPtr = MXL68x_DIG_TERR_LUT_BW_8MHZ;

					// program dig mode related spur lut
					if (freqLutPtr)
						status |= Ctrl_SetRfFreqLutReg(devId, freqInHz, freqLutPtr);

				break;

				default:
				break;
			}

			/* Calculate RF Channel = DIV(64*RF(Hz), 1E6) */
			frequency = freqInHz / 1000;
			frequency = frequency * 64;
			freq = (HUINT32)((frequency + 500) / 1000);

			// According to firmware status choose the channel tune is done by firmware or host SW
			// Firmware is loaded and active, process channel tune by firmware
			if ((MXL68x_FW_STATUS_LOADED == devContextPtr->firmwareStatus) &&
			(MXL68x_DIG_ISDBT == devContextPtr->signalType))
			{
				//Loop Through Configs need to be done for every channel change in SoC Mode
				if (MXL_ENABLE == devContextPtr->rfLoopThrough)
				{
					status |= MXL683_I2C_Read(devId, DFE_AGC_AUTO_REG, &regData);
					control = (regData & 0x80) | 0x0E;  // Bit<6> = 0, Bit<5:0> = 14
					status |= MXL683_I2C_Write(devId, DFE_AGC_AUTO_REG, control);

					status |= MXL683_I2C_Read(devId, DFE_SEQ_TUNE_RF1_BO_REG, &regData);
					control = (regData & 0xC0) | 0x0E;  // Bit<5:0> = 14
					status |= MXL683_I2C_Write(devId, DFE_SEQ_TUNE_RF1_BO_REG, control);

					status |= MXL683_I2C_Read(devId, DFE_SEQ_CDC_RF1_BO_REG, &regData);
					control = (regData & 0xC0) | 0x0E;  // Bit<5:0> = 14
					status |= MXL683_I2C_Write(devId, DFE_SEQ_CDC_RF1_BO_REG, control);
				}
				else
				{
					status |= MXL683_I2C_Read(devId, DFE_AGC_AUTO_REG, &regData);
					control = (regData & 0x80) | 0x40;  // Bit<6> = 1, Bit<5:0> = 0
					status |= MXL683_I2C_Write(devId, DFE_AGC_AUTO_REG, control);

					status |= MXL683_I2C_Read(devId, DFE_SEQ_TUNE_RF1_BO_REG, &regData);
					control = (regData & 0xC0) | 0x37;  // Bit<5:0> = 55
					status |= MXL683_I2C_Write(devId, DFE_SEQ_TUNE_RF1_BO_REG, control);

					status |= MXL683_I2C_Read(devId, DFE_SEQ_CDC_RF1_BO_REG, &regData);
					control = (regData & 0xC0) | 0x37;  // Bit<5:0> = 55
					status |= MXL683_I2C_Write(devId, DFE_SEQ_CDC_RF1_BO_REG, control);
				}

				CH_DI_Print(CH_LOG_4, ("[MXL683] MxLWare68x_API_CfgTunerChanTune() chid : %d, Firmware is active, program channel tune by FW \n",devId));
				//Program frequency value
				status |= MXL683_I2C_Write(devId, DMD_CHAN_SCAN_TUNE_FREQ_REG, (HUINT16)freq);

				switch(bandWidth)
				{
					case MXL68x_BW_6MHz: fwXtalBwSettings = fwXtalBwSettings | (0x00 << 2); break;
					case MXL68x_BW_7MHz: fwXtalBwSettings = fwXtalBwSettings | (0x01 << 2); break;
					case MXL68x_BW_8MHz: fwXtalBwSettings = fwXtalBwSettings | (0x02 << 2); break;
					default: break;
				}

				status |= MXL683_I2C_Read(devId, DMD_CHAN_SCAN_TUNE_XTAL_REG, &regData);

				fwXtalBwSettings = fwXtalBwSettings | ((devContextPtr->xtalFreqInMHz & 0x01) << 1);  // Set Xtal type , bit<1>
				fwXtalBwSettings = fwXtalBwSettings | (tuneType & 0x01);  // Set Tune type , bit<0>

				if(MXL68x_AUTO_SPEC_INV_ENABLE == autoSpecInv)
				{
					fwXtalBwSettings = fwXtalBwSettings | 0x10;  // Enable Auto Spec Inv bit <4> = 1
				}
				else
				{
					fwXtalBwSettings = fwXtalBwSettings & 0xEF;  // Disable Auto Spec Inv bit <4> = 0
				}

				status |= MXL683_I2C_Write(devId, DMD_CHAN_SCAN_TUNE_XTAL_REG, fwXtalBwSettings);

				//Send FW Host Command for channel tune
				FORM_CMD_HEADER(chanTune, HOST_CMD_TUNER_CHAN_TUNE_CFG, 0, 0);
				status |= Ctrl_SendHostCommand(devId, &chanTune);

				if(useChanTuneInt == MXL_FALSE)
				{
					//time out here is 3 sec
					while( (tuneDone != 1) && (tuneTimeOut <= 300) )
					{
						// Add 10 ms waiting time - poll every 10ms
						VK_TASK_Sleep(10);

						//reacd back for checking tune done
						status |= MXL683_I2C_Read(devId, DMD_CHAN_SCAN_TUNE_XTAL_REG, &tuneDone);
						regData = tuneDone;  // Save reading data of 0x3028

						//Channel Found Indication is Bit 7
						chanFound  = (tuneDone & 0x80) >> 7;
						//Tune Done Indication is Bit 6
						tuneDone = (tuneDone & 0x40) >> 6;

						tuneTimeOut++;
					}

					CH_DI_Print(CH_LOG_4, ("[MXL683] MxLWare68x_API_CfgTunerChanTune() chid : %d, FTuneDone = %d, ChanFound = %d, Time = %dms \n",devId,tuneDone,chanFound,(tuneTimeOut*10)));

					if(tuneDone == 0)
					{
						status |= MXL_FAILURE;
					}
					else
					{
						if(chanFound == 1)
						{
							// Means firmware has been loaded success before, get version information by FW communication
							status |= Ctrl_GetDeviceResp(devId, HOST_CMD_TUNER_TUNE_RESULT_REQ, &getChanDataRsp);

							if ((getChanDataRsp.errorCode == RESPONSE_ID_SUCCESS) && (status == MXL_SUCCESS)
							&& (getChanDataRsp.payloadLen == FW_CHAN_TUNE_DATA_LENGTH))
							{
								for(index_i=0; index_i<FW_CHAN_TUNE_DATA_LENGTH; index_i++)
								{
									CH_DI_Print(CH_LOG_4, ("[MXL683] MxLWare68x_API_CfgTunerChanTune() chid : %d, ChData[%d] = 0x%02X \n",devId,getChanDataRsp.data[index_i]));
								}
							}
						}
					}
					//Removing this as part of the cable cut issue. To facilitate the FW to Auto re-tune when this problem is detected, we do not reset the values here
					//it is reset at the start of this API
					//status |= MXL683_I2C_Write(devId, DMD_CHAN_SCAN_TUNE_XTAL_REG, 0x0000); // set all bits to 0 (including AutoSpecInv)
					//status |= MXL683_I2C_Write(devId, DMD_CHAN_SCAN_TUNE_FREQ_REG, 0x0000);
				}
			}
			else   // Analog Modes, perform channel tune by host
			{
				CH_DI_Print(CH_LOG_4, ("[MXL683] MxLWare68x_API_CfgTunerChanTune() chid : %d, Firmware is inactive, program channel tune by host SW \n",devId));

				// Tune mode scan or view <0>
				status |= MXL683_I2C_Read(devId, TUNE_MODE_REG, &regData);
				regData &= 0xFE;

				if (MXL68x_SCAN_MODE == tuneType) regData |= 0x01;

				status |= MXL683_I2C_Write(devId, TUNE_MODE_REG, regData);

				// Bandwidth <1:0>
				status |= MXL683_I2C_Read(devId, CHAN_TUNE_BW_REG, &regData);
				regData = (HUINT16)((regData & 0xFFFC) | bandWidth);

				status |= MXL683_I2C_Write(devId, CHAN_TUNE_BW_REG, regData);

				// Set RF
				status |= MXL683_I2C_Write(devId, CHAN_TUNE_LOW_REG, (HUINT8)(freq & 0xFF));
				status |= MXL683_I2C_Write(devId, CHAN_TUNE_HI_REG, (HUINT8)((freq >> 8 ) & 0xFF));

				if (devContextPtr->signalType <= MXL68x_ANA_SECAM_L)
				{
					status |= MxLWare68x_API_CfgTunerEnableFineTune(devId, MXL_DISABLE);
					status |= MXL683_I2C_Write(devId, FINE_TUNE_CTRL_REG_1, 0);
					status |= MXL683_I2C_Write(devId, FINE_TUNE_CTRL_REG_1, 0x20);
				}

				// Start Tune  = 1
				status |= MXL683_I2C_Write(devId, START_TUNE_REG, 0x0001);

				// Switch to PLL Clock
				// clkType = MXL68x_PLL_CLOCK;
				status |= Ctrl_CfgDevSwitchClock(devId, MXL68x_PLL_CLOCK);

				if (devContextPtr->devOpMode == MXL68x_TUNER_DEMOD)
				{
					// Enable Demod
					status |= Ctrl_CfgDemodEnable(devId, MXL_ENABLE);

					// Demod data path reset
					devReset = MXL68x_DEMOD_DATAPATH_RESET;
					status |= Ctrl_CfgDevReset(devId, devReset);
				}
				if ((devContextPtr->signalType != MXL68x_DIG_DVB_C) && (devContextPtr->signalType != MXL68x_DIG_J83B))
				{
					CH_DI_Print(CH_LOG_4, ("[MXL683] MxLWare68x_API_CfgTunerChanTune() chid : %d, TT Spur Mitigation Host Command to FW \n",devId));
					//Send FW Host Command to enable TeleText Spur Mitigation
					FORM_CMD_HEADER(chanTune, HOST_CMD_TUNER_TT_SPUR_MIT_EN_CFG, 0, 0);
					status |= Ctrl_SendHostCommand(devId, &chanTune);
				}
			}
		}
		else
		{
			status = MXL_NOT_INITIALIZED;
			CH_DI_Print(CH_LOG_0, ("[MXL683] MxLWare68x_API_CfgTunerChanTune() MXL_NOT_INITIALIZED \n"));
		}
	}
	else
	{
		status = MXL_INVALID_PARAMETER;
		CH_DI_Print(CH_LOG_0, ("[MXL683] MxLWare68x_API_CfgTunerChanTune() MXL_INVALID_PARAMETER \n"));
	}

	CH_DI_Print(CH_LOG_4, ("[MXL683] MxLWare68x_API_CfgTunerChanTune() chid : %d, status = %d \n",devId,status));

	return(MXL_STATUS_E)status;
}


MXL_STATUS_E MxLWare68x_API_CfgTunerEnableFineTune(
                int devId,
                MXL_BOOL_E enableFineTune)
{
	HUINT8 status = MXL_SUCCESS;
	HUINT16 fineTuneReg0 = 0;
	HUINT16 fineTuneReg1 = 0;
	HINT32	nUnitId;

	nUnitId = GetDeviceUnitId( devId );

	PMXL68x_DEV_CONTEXT_T devContextPtr = MxL68x_Ctrl_GetDeviceContext(nUnitId);

	if (devContextPtr)
	{
		if (devContextPtr->driverStatus)
		{
			status = MXL683_I2C_Read(devId, FINE_TUNE_CTRL_REG_0, &fineTuneReg0);
			status |= MXL683_I2C_Read(devId, FINE_TUNE_CTRL_REG_1, &fineTuneReg1);

			// Bit <2> set to 0 (disable) and 1(enable)
			fineTuneReg0 &= 0xFB;

			// Bit <5> set to 0 (disable) and 1(enable)
			fineTuneReg1 &= 0xDF;

			if (MXL_ENABLE == enableFineTune)
			{
				fineTuneReg0 |= 0x04;
				fineTuneReg1 |= 0x20;
			}
			else
			{
				// Bit <1:0> set to 0 (disable) and 1(enable)
				fineTuneReg0 &= 0xFC;
			}

			status |= MXL683_I2C_Write(devId, FINE_TUNE_CTRL_REG_0, fineTuneReg0);
			status |= MXL683_I2C_Write(devId, FINE_TUNE_CTRL_REG_1, fineTuneReg1);
		}
		else
		{
			status = MXL_NOT_INITIALIZED;
			CH_DI_Print(CH_LOG_0, ("[MXL683] MxLWare68x_API_CfgTunerEnableFineTune() MXL_NOT_INITIALIZED \n"));
		}
	}
	else
	{
		status = MXL_INVALID_PARAMETER;
		CH_DI_Print(CH_LOG_0, ("[MXL683] MxLWare68x_API_CfgTunerEnableFineTune() MXL_INVALID_PARAMETER \n"));
	}

	CH_DI_Print(CH_LOG_4, ("[MXL683] MxLWare68x_API_CfgTunerEnableFineTune() chid : %d, status = %d \n",devId,status));
	return(MXL_STATUS_E)status;
}


MXL_STATUS_E MxLWare68x_API_CfgTunerFineTune(
                int devId,
                MXL68x_FINE_TUNE_STEP_E fineTuneStep)
{
	HUINT8 status = MXL_SUCCESS;
	HUINT16 regData = 0;
	HINT32	nUnitId;

	nUnitId = GetDeviceUnitId( devId );

	PMXL68x_DEV_CONTEXT_T devContextPtr = MxL68x_Ctrl_GetDeviceContext(nUnitId);


	if (devContextPtr)
	{
		if (devContextPtr->driverStatus)
		{
			status = MXL683_I2C_Read(devId, FINE_TUNE_SET_REG, &regData);

			// Fine tune <1:0>
			regData &= 0xFFFC;
			status |= MXL683_I2C_Write(devId, FINE_TUNE_SET_REG, regData);

			if (MXL68x_FINE_TUNE_STEP_UP == fineTuneStep) regData |= TUNE_INCREASE;
			else regData |= TUNE_DECREASE;

			// Program fine tune step
			status |= MXL683_I2C_Write(devId, FINE_TUNE_SET_REG, regData);

		}
		else
		{
			status = MXL_NOT_INITIALIZED;
			CH_DI_Print(CH_LOG_0, ("[MXL683] MxLWare68x_API_CfgTunerFineTune() MXL_NOT_INITIALIZED \n"));
		}
	}
	else
	{
		status = MXL_INVALID_PARAMETER;
		CH_DI_Print(CH_LOG_0, ("[MXL683] MxLWare68x_API_CfgTunerFineTune() MXL_INVALID_PARAMETER \n"));
	}

	CH_DI_Print(CH_LOG_4, ("[MXL683] MxLWare68x_API_CfgTunerFineTune() chid : %d, status = %d \n",devId,status));
	return(MXL_STATUS_E)status;
}


MXL_STATUS_E MxLWare68x_API_ReqTunerAGCStatus(
                int devId,
                MXL_BOOL_E* agcLockStatusPtr,
                HUINT16* rawAgcGainPtr)
{
	HUINT8 status = MXL_SUCCESS;
	HUINT16 regData = 0;
	HUINT16 tempData = 0;
	HINT32	nUnitId;

	nUnitId = GetDeviceUnitId( devId );

	PMXL68x_DEV_CONTEXT_T devContextPtr = MxL68x_Ctrl_GetDeviceContext(nUnitId);

	if (devContextPtr)
	{
		if ((devContextPtr->driverStatus) && (rawAgcGainPtr != NULL))
		{
			*rawAgcGainPtr = 0;

			// Read low 8 bit
			status = MXL683_I2C_Read(devId, RF_TOTAL_GAIN_RB_REG, &regData);
			tempData = regData & 0xFF;

			// Read high 4 bit
			status |= MXL683_I2C_Read(devId, RF_TOTAL_GAIN_RB_REG + 1, &regData);
			regData = ((regData & 0x0F) << 8) + tempData;

			if (MXL_SUCCESS == status) *rawAgcGainPtr = regData;

			// Readback AGC lock status
			status |= MXL683_I2C_Read(devId, AGC_SAGCLOCK_STATUS_REG, &regData);

			if ((regData & 0x08) == 0x08) *agcLockStatusPtr = MXL_ON;
			else *agcLockStatusPtr = MXL_OFF;

		}
		else
		{
			status = MXL_NOT_INITIALIZED;
			CH_DI_Print(CH_LOG_0, ("[MXL683] MxLWare68x_API_ReqTunerAGCStatus() MXL_NOT_INITIALIZED \n"));
		}
	}
	else
	{
		status = MXL_INVALID_PARAMETER;
		CH_DI_Print(CH_LOG_0, ("[MXL683] MxLWare68x_API_ReqTunerAGCStatus() MXL_INVALID_PARAMETER \n"));
	}

	CH_DI_Print(CH_LOG_4, ("[MXL683] MxLWare68x_API_ReqTunerAGCStatus() chid : %d, status = %d \n",devId,status));
	return(MXL_STATUS_E)status;
}


MXL_STATUS_E MxLWare68x_API_ReqTunerLockStatus(
                int devId,
                MXL_BOOL_E* rfSynthLockPtr,
                MXL_BOOL_E* refSynthLockPtr)
{
	HUINT8 status = MXL_SUCCESS;
	HUINT16 regData = 0;
	HINT32	nUnitId;

	nUnitId = GetDeviceUnitId( devId );

	PMXL68x_DEV_CONTEXT_T devContextPtr = MxL68x_Ctrl_GetDeviceContext(nUnitId);

	if (devContextPtr)
	{
		if (devContextPtr->driverStatus)
		{
			// Readback tuner lock status
			status = MXL683_I2C_Read(devId, RF_REF_STATUS_REG, &regData);

			// Bit<0>
			if ((regData & 0x01) == 0x01) *refSynthLockPtr = MXL_YES;
			else *refSynthLockPtr = MXL_NO;

			// Bit<1>
			if ((regData & 0x02) == 0x02) *rfSynthLockPtr = MXL_YES;
			else *rfSynthLockPtr = MXL_NO;

			CH_DI_Print(CH_LOG_4, ("[MXL683] MxLWare68x_API_ReqTunerLockStatus() chid : %d,  RfSynthStatus = %d, RefSynthStatus = %d \n",devId,(HUINT8)*rfSynthLockPtr,(HUINT8)*refSynthLockPtr));

		}
		else
		{
			status = MXL_NOT_INITIALIZED;
			CH_DI_Print(CH_LOG_0, ("[MXL683] MxLWare68x_API_ReqTunerLockStatus() MXL_NOT_INITIALIZED \n"));
		}
	}
	else
	{
		status = MXL_INVALID_PARAMETER;
		CH_DI_Print(CH_LOG_0, ("[MXL683] MxLWare68x_API_ReqTunerLockStatus() MXL_INVALID_PARAMETER \n"));
	}

	CH_DI_Print(CH_LOG_4, ("[MXL683] MxLWare68x_API_ReqTunerLockStatus() chid : %d, status = %d \n",devId,status));
	return(MXL_STATUS_E)status;
}


MXL_STATUS_E MxLWare68x_API_ReqTunerRxPower(
                int devId,
                HINT32* rxPwrPtr)
{
	HUINT32 dfeRegData = 0;
	HUINT16 regData = 0, tmpData = 0;
	HUINT8 status = MXL_SUCCESS;
	HINT32	nUnitId;

	nUnitId = GetDeviceUnitId( devId );

	PMXL68x_DEV_CONTEXT_T devContextPtr = MxL68x_Ctrl_GetDeviceContext(nUnitId);

	if (devContextPtr)
	{
		if (devContextPtr->driverStatus)
		{
			if ((MXL68x_FW_STATUS_LOADED == devContextPtr->firmwareStatus))
			{
				Ctrl_DfeRegRead(devId, RFPIN_RB_LOW_REG, 2, &dfeRegData);
				tmpData = (HUINT16)(dfeRegData & 0x3FF);
			}
			else
			{
				// RF input power low <7:0>
				status = MXL683_I2C_Read(devId, RFPIN_RB_LOW_REG, &regData);
				tmpData = regData;

				// RF input power high <1:0>
				status |= MXL683_I2C_Read(devId, RFPIN_RB_HIGH_REG, &regData);
				tmpData |= (regData & 0x03) << 8;
			}

			// Fractional last 2 bits
			*rxPwrPtr= (tmpData & 0x01FF) * 25; // Change the unit to 0.01 dBm

			if (tmpData & 0x02) *rxPwrPtr += 50;
			if (tmpData & 0x01) *rxPwrPtr += 25;
			if (tmpData & 0x0200) *rxPwrPtr -= 128*100;

			CH_DI_Print(CH_LOG_4, ("[MXL683] MxLWare68x_API_ReqTunerRxPower() chid : %d,Rx power = %d *0.01 dBm \n",devId,*rxPwrPtr));
		}
		else
		{
			status = MXL_NOT_INITIALIZED;
			CH_DI_Print(CH_LOG_0, ("[MXL683] MxLWare68x_API_ReqTunerRxPower() MXL_NOT_INITIALIZED \n"));
		}
	}
	else
	{
		status = MXL_INVALID_PARAMETER;
		CH_DI_Print(CH_LOG_0, ("[MXL683] MxLWare68x_API_ReqTunerRxPower() MXL_INVALID_PARAMETER \n"));
	}

	CH_DI_Print(CH_LOG_4, ("[MXL683] MxLWare68x_API_ReqTunerRxPower() chid : %d, status = %d \n",devId,status));
	return (MXL_STATUS_E)status;
}



MXL_STATUS_E MxLWare68x_API_CfgDevCustSpecificSettings(
                int devId,
                HUINT16 mask)
{
	HUINT8 status = MXL_SUCCESS;
	HINT32	nUnitId;

	nUnitId = GetDeviceUnitId( devId );

	PMXL68x_DEV_CONTEXT_T devContextPtr = MxL68x_Ctrl_GetDeviceContext(nUnitId);


	if (devContextPtr)
	{
		if (devContextPtr->driverStatus)
		{
			status |= MXL683_I2C_Write(devId, CUST_SET_REG, mask);
		}
		else
		{
			status = MXL_NOT_INITIALIZED;
			CH_DI_Print(CH_LOG_0, ("[MXL683] MxLWare68x_API_CfgDevCustSpecificSettings() MXL_NOT_INITIALIZED \n"));
		}
	}
	else
	{
		status = MXL_INVALID_PARAMETER;
		CH_DI_Print(CH_LOG_0, ("[MXL683] MxLWare68x_API_CfgDevCustSpecificSettings() MXL_INVALID_PARAMETER \n"));
	}

	CH_DI_Print(CH_LOG_4, ("[MXL683] MxLWare68x_API_CfgDevCustSpecificSettings() chid : %d, status = %d \n",devId,status));
	return (MXL_STATUS_E)status;
}

MXL_STATUS_E downloadFirmware(int DEV_ID)
{
	MXL_STATUS_E status = MXL_SUCCESS;
	HUINT32 fwBuffSize;
	HUINT8 *fwBuffPtr = NULL;
#if 0
	long fileSize = 0;
	FILE * fw=NULL;
	/*
	Download the firmware by either opening the file and reading the contents as shown in the demo code here,
	or by including the header files provided in the source code release, as shown below:

	const unsigned char firmware[] =
	{
#ifdef MXL68X_FIRMWARE_V1
#include "MxL68x_FW_V1.h"
#else
#include "MxL68x_FW_V1_200.h"
#endif
	};

	fwBuffSize = sizeof(firmware);
	fwBuffPtr = firmware;

	MxL68x_FW_V1.h contains FW for V1 version of the chip and
	MxL68x_FW_V1_200.h contains FW for the V1_200 version of the chip
	*/
	fw = fopen(".\\Firmware\\ISDBT_FW_RAM.mbin", "rb");

	fseek(fw, 0, SEEK_END);
	fileSize = ftell(fw);
	fseek(fw, 0, SEEK_SET);

	if (fileSize)
	{
		fwBuffSize = fileSize;

		fwBuffPtr = (HUINT8 *)VK_malloc(1 + (sizeof(HUINT8) * fwBuffSize));
		fileSize = fread(fwBuffPtr, sizeof(HUINT8), fwBuffSize, fw);

		if (fileSize == (long)fwBuffSize)
		{
			status = MxLWare68x_API_CfgDevFirmwareDownload(DEV_ID,
			                         (HUINT32)fwBuffSize,
			                         fwBuffPtr);

			if (status != MXL_SUCCESS)
				VK_printf ("Err : Firmware Download %d\n", status);
		}
		else
		{
			VK_printf ("Err : File Read Size = %d FW size = %d \n", fileSize, fwBuffSize);
		}
	}
	else
	{
		VK_printf ("Err : FW file size %d\n", fileSize);
	}

	if (fw) fclose(fw);

	if (fwBuffPtr) VK_free (fwBuffPtr);
#else
	const unsigned char firmware[] =
	{
#include "MxL68x_FW_V1_200.h"
	};

	fwBuffSize = sizeof(firmware);
//	fwBuffPtr = malloc(fwBuffSize);
//	memset(fwBuffPtr,0x00,fwBuffSize);
//	memset(fwBuffPtr,0x00,fwBuffSize);
	fwBuffPtr = firmware;

	CH_DI_Print(CH_LOG_4, ("[MXL683] MxLWare68x_API_CfgDevFirmwareDownload()...size : %d,  fwBuffPtr0: %x, fwBuffPtr1= %x fwBuffPtr2= %x \n",fwBuffSize,fwBuffPtr[0],fwBuffPtr[1],fwBuffPtr[2]));
	if (fwBuffSize)
	{
		status = MxLWare68x_API_CfgDevFirmwareDownload(DEV_ID,
								 (HUINT32)fwBuffSize,
								 fwBuffPtr);
		if (status != MXL_SUCCESS)
			CH_DI_Print(CH_LOG_0, ("[MXL683] MxLWare68x_API_CfgDevFirmwareDownload() 111 FAILED chid : %d, status = %d \n",DEV_ID,status));
	}

{
	HUINT8 chipIdPtr;
	HUINT8 chipVersionPtr;
	HUINT8 mxlWareVer[MXL68X_VERSION_SIZE];
	HUINT8 firmwareVer[MXL68X_VERSION_SIZE];
	HUINT8 bootloaderVer[MXL68X_VERSION_SIZE];

	MxLWare68x_API_ReqDevVersionInfo(DEV_ID,
					&chipIdPtr,
					&chipVersionPtr,
					mxlWareVer,
					firmwareVer,
					bootloaderVer);
}
#endif
	return status;
}






int MXL683_InitDevice(int nChannelId)
{
	MXL_STATUS_E status = MXL_SUCCESS;
	MXL68x_FW_STATUS_E firmwareDownloadStatus;
	HINT32	nUnitId;

	nUnitId = GetDeviceUnitId( nChannelId );

	DRV_CFG_SetTSlineConfiguration(nChannelId);

	MXL683_ResetGpio(nChannelId);
#if 0
{
	HUINT16	regData;
	status |= MXL683_I2C_Read(nChannelId, DMD_GPIO_FUNC_MODE_REG, &regData);
	CH_DI_Print(CH_LOG_4, ("[MXL683] MXL683_InitDevice()  DMD_GPIO_FUNC_MODE_REG (%x)! \n",regData));
}
#endif
	VK_memset(&MxL68x_Dev[nUnitId], 0x00, sizeof(MXL68x_DEV_CONTEXT_T));
	VK_memset(&MxL68x_Signal_Information[nUnitId], 0x00, sizeof(MXL683_Signal_Info));

	// Init driver context
	status |= MxLWare68x_API_CfgDrvInit(nChannelId);
	if (status != MXL_SUCCESS)
	{
		CH_DI_Print(CH_LOG_0, ("[MXL683] MxLWare68x_API_CfgDrvInit()  FAIL (%d)! \n",status));
		return DI_CH_ERR;
	}

	// Config Power Supply
	status |= MxLWare68x_API_CfgDevPowerSupply(nChannelId, MXL68x_SINGLE_SUPPLY);
	if (status != MXL_SUCCESS)
	{
		CH_DI_Print(CH_LOG_0, ("[MXL683] MxLWare68x_API_CfgDevPowerSupply()  FAIL (%d)! \n",status));
		return DI_CH_ERR;
	}


#if 0 //I2C test
	{
		while(1)
			{
				HUINT16 regData1;
				regData1 = 0x1617;
				status |= MXL683_I2C_Write(nChannelId, DMD_CHAN_SCAN_TUNE_FREQ_REG, regData1);
				VK_TASK_Sleep(100);
				status |= MXL683_I2C_Read(nChannelId,DMD_CHAN_SCAN_TUNE_FREQ_REG, &regData1);  // High 16 bit
				VK_TASK_Sleep(100);
				CH_DI_Print(0, ("[MXL683] JAMES... regData1 : 0x%x, status = %d \n",regData1,status));
			}
	}
#endif

//JAMES...141022...sharing X-tal setting
#if defined (CONFIG_XTAL_CAP_23)
	// Config XTAL
	// Xtal Cap has been changed cuz of the vendor of Xtal was replaced.
	// WA8500 : Xtal Cap : 25
	// UHD2500 : Xtal Cap : 23
	CH_DI_Print(CH_LOG_0, ("\n\n\n[MXL683] CONFIG_XTAL_CAP_23()  ! \n\n\n\n"));
	if (nChannelId == 4)//Master
	{
		status |= MxLWare68x_API_CfgDevXtal(nChannelId,
							   MXL68x_XTAL_16MHz,	// 16 MHz
							   23,							// 25 pF
							   MXL_ENABLE,		  // Enable CLK out
							   MXL_FALSE,			// Disable CLK DIV
							   MXL_FALSE,		   // Disable XTAL Sharing
							   MXL_ENABLE); 		 // Enable XTAL Cal for Master, Disable for Slave
	}
	else if (nChannelId == 6)
	{
		status |= MxLWare68x_API_CfgDevXtal(nChannelId,
							   MXL68x_XTAL_16MHz,	// 16 MHz
							   23,							// 25 pF
							   MXL_ENABLE,		  // Enable CLK out
							   MXL_FALSE,			// Disable CLK DIV
							   MXL_FALSE,		   // Disable XTAL Sharing
							   MXL_ENABLE); 		 // Enable XTAL Cal for Master, Disable for Slave
	}
	else//Slave
	{
		status |= MxLWare68x_API_CfgDevXtal(nChannelId,
							   MXL68x_XTAL_16MHz,	// 16 MHz
							   5,							// 25 pF
							   MXL_FALSE,		  // Enable CLK out
							   MXL_FALSE,			// Disable CLK DIV
							   MXL_ENABLE,		   // Disable XTAL Sharing
							   MXL_FALSE); 		 // Enable XTAL Cal for Master, Disable for Slave
	}
#else
	CH_DI_Print(CH_LOG_0, ("\n\n\n[MXL683] CONFIG_XTAL_CAP_25()  ! \n\n\n\n"));
	if (nChannelId == 4)//Master
	{
		status |= MxLWare68x_API_CfgDevXtal(nChannelId,
							   MXL68x_XTAL_16MHz,	// 16 MHz
							   25,							// 25 pF
							   MXL_ENABLE,		  // Enable CLK out
							   MXL_FALSE,			// Disable CLK DIV
							   MXL_FALSE,		   // Disable XTAL Sharing
							   MXL_ENABLE); 		 // Enable XTAL Cal for Master, Disable for Slave
	}
	else//Slave
	{
		status |= MxLWare68x_API_CfgDevXtal(nChannelId,
							   MXL68x_XTAL_16MHz,	// 16 MHz
							   5,							// 25 pF
							   MXL_FALSE,		  // Enable CLK out
							   MXL_FALSE,			// Disable CLK DIV
							   MXL_ENABLE,		   // Disable XTAL Sharing
							   MXL_FALSE); 		 // Enable XTAL Cal for Master, Disable for Slave
	}
#endif

	if (status != MXL_SUCCESS)
	{
		CH_DI_Print(CH_LOG_0, ("[MXL683] MxLWare68x_API_CfgDevXtal()  FAIL (%d)! \n",status));
		return DI_CH_ERR;
	}

	// Config Dev Init, switch clock to PLL clock
	status |= MxLWare68x_API_CfgDevInit(nChannelId,
									   MXL68x_IF_NA,		  // Do not set IF mux
									   MXL_FALSE);			  // Disable RF Loop-Thru
	if (status != MXL_SUCCESS)
	{
		CH_DI_Print(CH_LOG_0, ("[MXL683] MxLWare68x_API_CfgDevInit()  FAIL (%d)! \n",status));
		return DI_CH_ERR;
	}

	VK_TASK_Sleep(100); // wait for 100 ms

	firmwareDownloadStatus = Ctrl_CheckFirmwareStatus(nChannelId);
	CH_DI_Print(CH_LOG_4, ("[MXL683] MxLWare68x_API_CfgDevInit()  firmwareDownloadStatus (%d)! \n",firmwareDownloadStatus));

	if (firmwareDownloadStatus != MXL68x_FW_STATUS_LOADED)
	{
		// Download firmware
		status |= downloadFirmware(nChannelId);
		if (status != MXL_SUCCESS)
		{
			CH_DI_Print(CH_LOG_0, ("[MXL683] Ctrl_CheckFirmwareStatus()  FAIL (%d)! \n",status));
			return DI_CH_ERR;
		}
		else
		{
			CH_DI_Print(CH_LOG_4, ("[MXL683] Ctrl_CheckFirmwareStatus()  SUCCESS (%d)! \n",status));
		}

		VK_TASK_Sleep(100); // wait for 100 ms
	}
	else
		CH_DI_Print(CH_LOG_0, ("[MXL683] Ctrl_CheckFirmwareStatus()  FW has been already loaded  (%d)!!! \n",firmwareDownloadStatus));

	// Change Device Operational Mode to Tuner+Demod
	status |= MxLWare68x_API_CfgDevOperationalMode(nChannelId, MXL68x_TUNER_DEMOD);
	if (status != MXL_SUCCESS)
	{
		CH_DI_Print(CH_LOG_0, ("[MXL683] MxLWare68x_API_CfgDevOperationalMode()  FAIL (%d)! \n",status));
		return DI_CH_ERR;
	}

    // configure MxL68x MPEG TS out interface
    status = MxLWare68x_API_CfgDemodMPEGOut(
                nChannelId, // OK
                MXL68x_MPEG_MODE_NORMAL, // OK 4-wired IF
                MXL68x_MPEG_CLK_64MHZ,	    // ??
                MXL68x_MPEG_DATA_MSB_FIRST, // OK
                MXL68x_MPEG_CLK_PHASE_SHIFT_180_DEG,  // ??
                MXL68x_POLARITY_NORMAL,	// ??
                MXL68x_POLARITY_NORMAL,	// ??
                MXL68x_MPEG_SYNC_PULSE_1BYTE,	// ??
                MXL68x_MPEG_ERROR_IND_ENABLE,	// OK
                MXL68x_TS_PKT_188_BYTES,
                MXL68x_TS_DRV_STR_1X);    // ??

	if (status != MXL_SUCCESS)
	{
		CH_DI_Print(CH_LOG_0, ("[MXL683] MxLWare68x_API_CfgDemodMPEGOut()  FAIL (%d)! \n",status));
		return DI_CH_ERR;
	}

	// Config IF Mux and RF loop through
	status |= MxLWare68x_API_CfgDevInit(nChannelId,
									   MXL68x_IF_INTERNAL,	  // Initernal IF
									   MXL_FALSE);			  // Disable RF Loop-Thru
	if (status != MXL_SUCCESS)
	{
		CH_DI_Print(CH_LOG_0, ("[MXL683] MxLWare68x_API_CfgDevInit()  FAIL (%d)! \n",status));
		return DI_CH_ERR;
	}

	status |= MxLWare68x_API_CfgDemodBerPer(nChannelId, 4);		    // numFrames 8 -> approximately 1600ms

	if (status != MXL_SUCCESS)
	{
		CH_DI_Print(CH_LOG_0, ("[MXL683] MxLWare68x_API_CfgDemodBerPer()  FAIL (%d)! \n",status));
		return DI_CH_ERR;
	}

	if (status != MXL_SUCCESS)
		return DI_CH_ERR;
	else
		return DI_CH_OK;

}


int MXL683_SetTune( int nChannelId, HUINT8 *bChecklock , HUINT32 unWorkingId )
{

	 int				 nResult = DI_CH_OK;
	 int 				i=0;
	 HUINT16			regData;
	 HUINT32	 		frequency;
	 unsigned char		 lock_ofdm = 0;
	 MXL_BOOL_E rfSynthLock, refSynthLock;
	 MXL_BOOL_E rsLockA, rsLockB, rsLockC, tmccLock;
	 MXL_STATUS_E status = MXL_SUCCESS;
	 CH_TER_TuneParam_t *pstTerTuneParam;
	 MXL68x_TMCC_INFO_T tmccInfoLayerA, tmccInfoLayerB, tmccInfoLayerC;
	HINT32	nUnitId;

	nUnitId = GetDeviceUnitId( nChannelId );

	CH_UNUSED(unWorkingId);

	pstTerTuneParam = (CH_TER_TuneParam_t *)GetWorkingTuneInfo( nChannelId );

	frequency = pstTerTuneParam->ulFrequency * KHZ_TO_HZ ;

	if(((frequency -143000) / 1000000) != (frequency /1000000)) /* 만약 143KHz가 안붙어서 오는 경우 */
		frequency += 143000;

	MxL68x_Signal_Information[nUnitId].ulCorrectedNo = 0;
	MxL68x_Signal_Information[nUnitId].ulUnCorrectedNo = 0;
	MxL68x_Signal_Information[nUnitId].ulUnErroredNo = 0 ;
	MxL68x_Signal_Information[nUnitId].fBer = 0;

	CH_DI_Print(CH_LOG_1, ("[MXL683] MXL683_SetTune[CHID : %d]....FREQ = %d, BW = %d  ! \n",nChannelId, frequency, pstTerTuneParam->etBandWidth));
	// Set or Change Channel
	status = MxLWare68x_API_CfgTunerChanTune(nChannelId,
											 MXL68x_VIEW_MODE,
								 MXL68x_BW_6MHz,
								 frequency,
								 MXL68x_AUTO_SPEC_INV_ENABLE,
								 MXL_FALSE);
	if (status != MXL_SUCCESS)
		CH_DI_Print(CH_LOG_0, ("[MXL683] MXL683_SetTune....MxLWare68x_API_CfgTunerChanTune()  FAIL (%d)! \n",status));

	// Check Tuner lock status
	status = MxLWare68x_API_ReqTunerLockStatus(nChannelId, &rfSynthLock, &refSynthLock);
	if (status != MXL_SUCCESS)
		CH_DI_Print(CH_LOG_0, ("[MXL683] MXL683_SetTune...MxLWare68x_API_ReqTunerLockStatus()  FAIL (%d)! \n",status));
	else
		CH_DI_Print(CH_LOG_4, ("[MXL683] MXL683_SetTune...MxLWare68x_API_ReqTunerLockStatus()...  (RFsync = %d, REFsync = %d)! \n",rfSynthLock, refSynthLock));

	// Wait for 1 seconds
	VK_TASK_Sleep(1000);

	// Check Demod lock status
	status = MxLWare68x_API_ReqDemodLockStatus(nChannelId, &tmccLock, &rsLockA, &rsLockB, &rsLockC);
	if (status != MXL_SUCCESS)
		CH_DI_Print(CH_LOG_0, ("[MXL683] MXL683_SetTune...MxLWare68x_API_ReqDemodLockStatus()  FAIL (%d)! \n",status));
	else
		CH_DI_Print(CH_LOG_4, ("[MXL683] MXL683_SetTune...MxLWare68x_API_ReqTunerLockStatus()...  (tmccLock = %d, rsLockA = %d, rsLockB = %d, rsLockC = %d)! \n",tmccLock, rsLockA,rsLockB, rsLockC));


	if (tmccLock)
	{
		lock_ofdm = 0;
		MxLWare68x_API_ReqDemodTMCCParams(nChannelId, NULL, NULL, NULL, &tmccInfoLayerA, &tmccInfoLayerB, &tmccInfoLayerC);
		if( ((tmccInfoLayerA.numOfSegments > 0) && (tmccInfoLayerA.numOfSegments < 14))
			& (tmccInfoLayerA.codeRateInfo <= MXL68x_CR_7_8)
			&(tmccInfoLayerA.modulationInfo <= MXL68x_MODULATION_QAM64)
			& (tmccInfoLayerA.interleaverLenInfo <= MXL68x_INTERLEAVER_INDEX3))
		{
			if (rsLockA == MXL_FALSE)
			{
				for (i=0;i<20;i++)
				{
					status |= MXL683_I2C_Read(nChannelId, DMD_RS_LOCK_A_B, &regData);
					if( ((regData >> 14) & 0x01) == 0x01)
					{
						rsLockA = MXL_TRUE;
						break;
					}
					VK_TASK_Sleep(20);
				}
			}
		CH_DI_Print(CH_LOG_2, ("[MXL683] Layer A LOCK_Check(%d) count (%d)! \n",rsLockA,i));
			lock_ofdm |= rsLockA;
		}
		if( ((tmccInfoLayerB.numOfSegments > 0) && (tmccInfoLayerB.numOfSegments < 14))
			& (tmccInfoLayerB.codeRateInfo <= MXL68x_CR_7_8)
			&(tmccInfoLayerB.modulationInfo <= MXL68x_MODULATION_QAM64)
			& (tmccInfoLayerB.interleaverLenInfo <= MXL68x_INTERLEAVER_INDEX3))
		{
			if (rsLockB == MXL_FALSE)
			{
				for (i=0;i<20;i++)
				{
					status |= MXL683_I2C_Read(nChannelId, DMD_RS_LOCK_A_B, &regData);
					if( ( (regData >> 15) & 0x01) == 0x01)
					{
						rsLockB = MXL_TRUE;
						break;
					}
					VK_TASK_Sleep(20);
				}
			}
		CH_DI_Print(CH_LOG_2, ("[MXL683] Layer B LOCK_Check(%d) count (%d)! \n",rsLockB,i));
			lock_ofdm |= rsLockB;
		}
		if( ((tmccInfoLayerC.numOfSegments > 0) && (tmccInfoLayerC.numOfSegments < 14))
			& (tmccInfoLayerC.codeRateInfo <= MXL68x_CR_7_8)
			&(tmccInfoLayerC.modulationInfo <= MXL68x_MODULATION_QAM64)
			& (tmccInfoLayerC.interleaverLenInfo <= MXL68x_INTERLEAVER_INDEX3))
		{
			if (rsLockC == MXL_FALSE)
			{
				for (i=0;i<20;i++)
				{
					status |= MXL683_I2C_Read(nChannelId, DMD_RS_LOCK_C, &regData);
					if( (regData & 0x01) == 0x01)
					{
						rsLockC = MXL_TRUE;
						break;
					}
					VK_TASK_Sleep(20);
				}
			}
		CH_DI_Print(CH_LOG_2, ("[MXL683] Layer C LOCK_Check(%d) count (%d)! \n",rsLockC,i));
			lock_ofdm |= rsLockC;
		}
	}

	if( lock_ofdm == TRUE )
	{
		CH_DI_Print(CH_LOG_4, (">>> TER CH_STATUS_LOCKED\n"));
		SetTuneStatus( nChannelId, CH_STATUS_LOCKED );
		//JAMES...140616... channel status DB values should be set before MXL683_GetSignalInfo() is called when the first time locking is tried */
		MXL683_CheckLock_for_tune(nChannelId, 0);
		DRV_CH_CallNotifyCallback( nChannelId, unWorkingId, DI_CH_SIGNAL_LOCKED );
	}
	else
	{
		CH_DI_Print(CH_LOG_4, (">>> TER CH_STATUS_UNLOCK [%d] \n", nChannelId));
		SetTuneStatus( nChannelId, CH_STATUS_UNLOCK );
		DRV_CH_CallNotifyCallback( nChannelId, unWorkingId, DI_CH_SIGNAL_UNLOCK );
	}

	*bChecklock = lock_ofdm;

	return nResult;
 }

 HBOOL MXL683_CheckLock_for_tune ( int nChannelId, HUINT32 unWorkingID )
{
	int	 nResult = DI_CH_OK;
	MXL_BOOL_E rsLockA, rsLockB, rsLockC, tmccLock;
	MXL68x_TMCC_INFO_T tmccInfoLayerA, tmccInfoLayerB, tmccInfoLayerC;
	HUINT8 LOCK_683 = FALSE;
	HUINT32 cnrRawPtr;
	HINT32 cnrPtr;
	HINT32 RF_POWER;
	MXL68x_BER_T BerPtr;
	HUINT8 numFramesPtr;
	MXL_BOOL_E berEndFlag;
	CH_TER_TuneParam_t *pstTerTuneParam;
	HUINT32 frequency;
	HINT32	nUnitId;

	nUnitId = GetDeviceUnitId( nChannelId );

	 CH_UNUSED(unWorkingID);

	 pstTerTuneParam = (CH_TER_TuneParam_t *)GetWorkingTuneInfo( nChannelId );
	 frequency = pstTerTuneParam->ulFrequency;
	 if(((frequency -143) / 1000) != (frequency /1000)) /* 만약 143KHz가 안붙어서 오는 경우 */
		 frequency += 143;

	 // Check Demod lock status
	 nResult = MxLWare68x_API_ReqDemodLockStatus(nChannelId, &tmccLock, &rsLockA, &rsLockB, &rsLockC);
	 if (nResult != MXL_SUCCESS)
		 CH_DI_Print(CH_LOG_0, ("[MXL683] MXL683_CheckLock_for_tune...MxLWare68x_API_ReqDemodLockStatus()  FAIL (%d)! \n",nResult));
	 else
		 CH_DI_Print(CH_LOG_4, ("[MXL683] MXL683_CheckLock_for_tune...MxLWare68x_API_ReqTunerLockStatus()...	(tmccLock = %d, rsLockA = %d, rsLockB = %d, rsLockC = %d)! \n",tmccLock, rsLockA, rsLockB, rsLockC));


	 if (tmccLock)
	 {
		 LOCK_683 = FALSE;
		 MxLWare68x_API_ReqDemodTMCCParams(nChannelId, NULL, NULL, NULL, &tmccInfoLayerA, &tmccInfoLayerB, &tmccInfoLayerC);
		 if( ((tmccInfoLayerA.numOfSegments > 0) && (tmccInfoLayerA.numOfSegments < 14))
			 & (tmccInfoLayerA.codeRateInfo <= MXL68x_CR_7_8)
			 &(tmccInfoLayerA.modulationInfo <= MXL68x_MODULATION_QAM64)
			 & (tmccInfoLayerA.interleaverLenInfo <= MXL68x_INTERLEAVER_INDEX3))
		 {
			 CH_DI_Print(CH_LOG_2, ("MXL683_CheckLock_for_tune[]:: LrsLockA ~~~~~ \n"));
			 LOCK_683 |= rsLockA;
		 }
		 if( ((tmccInfoLayerB.numOfSegments > 0) && (tmccInfoLayerB.numOfSegments < 14))
			 & (tmccInfoLayerB.codeRateInfo <= MXL68x_CR_7_8)
			 &(tmccInfoLayerB.modulationInfo <= MXL68x_MODULATION_QAM64)
			 & (tmccInfoLayerB.interleaverLenInfo <= MXL68x_INTERLEAVER_INDEX3))
		 {
			 CH_DI_Print(CH_LOG_2, ("MXL683_CheckLock_for_tune[]:: rsLockB ~~~~~ \n"));
			 LOCK_683 |= rsLockB;
		 }
		 if( ((tmccInfoLayerC.numOfSegments > 0) && (tmccInfoLayerC.numOfSegments < 14))
			 & (tmccInfoLayerC.codeRateInfo <= MXL68x_CR_7_8)
			 &(tmccInfoLayerC.modulationInfo <= MXL68x_MODULATION_QAM64)
			 & (tmccInfoLayerC.interleaverLenInfo <= MXL68x_INTERLEAVER_INDEX3))
		 {
			 CH_DI_Print(CH_LOG_2, ("MXL683_CheckLock_for_tune[]:: rsLockC ~~~~~ \n"));
			 LOCK_683 |= rsLockC;
		 }
	 }
	 CH_DI_Print(CH_LOG_2, ("[MXL683] MXL683_CheckLock_for_tune...MxLWare68x_API_ReqTunerLockStatus()...	(LOCK_683 = %d)! \n",LOCK_683));


	 if (LOCK_683 != TRUE)
	 {
		 CH_DI_Print(CH_LOG_2, ("MXL683_CheckLock_for_tune[%d]:: LOCK FAIL ~~~~~ \n", nChannelId));
		 MxL68x_Signal_Information[nUnitId].fSignalInputPower = 0; //dBuV
		 MxL68x_Signal_Information[nUnitId].fSnr = 0;
		 //Converting AGC Gain value to RF strength
		 MxL68x_Signal_Information[nUnitId].ulAgc = 0;
		 MxL68x_Signal_Information[nUnitId].ulStrength= 0;
		 MxL68x_Signal_Information[nUnitId].ulQuality= 0;

		 MxL68x_Signal_Information[nUnitId].ulCorrectedNo= 0;
		 MxL68x_Signal_Information[nUnitId].ulUnCorrectedNo= 0;
		 MxL68x_Signal_Information[nUnitId].ulUnErroredNo= 0 ;
		 MxL68x_Signal_Information[nUnitId].ulUnCorrectedFlag= 0;

		 MxL68x_Signal_Information[nUnitId].fBer = 0;
		 return FALSE;
	 }
	 else
	 {
		 MxL68x_Signal_Information[nUnitId].fBer = 0;
		 MxLWare68x_API_ReqDemodCNR(nChannelId, &cnrRawPtr, &cnrPtr);
		 MxLWare68x_API_ReqTunerRxPower(nChannelId, &RF_POWER);
#if 0
		 MxLWare68x_API_ReqDemodBerPerEndFlag(nChannelId, &numFramesPtr, &berEndFlag);
		 //CH_DI_Print(0, ("[MXL683] MXL683_GetSignalInfo[%d]:: numFramesPtr =%d , berEndFlag = %d,  \n ",nChannelId,numFramesPtr,berEndFlag));

		nResult = MxLWare68x_API_ReqDemodBER(nChannelId, &BerPtr);
		if (nResult == MXL_SUCCESS)
		{
			if (BerPtr.bitErrCount_WO_Iter < 300)
			{
				BerPtr.bitErrCount_WO_Iter = 0;
				BerPtr.bitErrCount_W_Iter = 0;
				BerPtr.numOfBits = 0;
			}

		}

		MxLWare68x_API_CfgDemodClearBerPerEndFlag(nChannelId);
#endif

#ifdef CONFIG_LONG_RF_TRACE //WA-7500
		if (frequency <= 423143)
			MxL68x_Signal_Information[nUnitId].fSignalInputPower = RF_POWER/100+104; //dBuV
		else if (frequency <= 665143)
			MxL68x_Signal_Information[nUnitId].fSignalInputPower = RF_POWER/100+106; //dBuV
		else if (frequency <= 767143)
			MxL68x_Signal_Information[nUnitId].fSignalInputPower = RF_POWER/100+107; //dBuV
#else	//WA-8500
		if (frequency <= 497143)
			MxL68x_Signal_Information[nUnitId].fSignalInputPower = RF_POWER/100+103; //dBuV
		else if (frequency <=767143)
			MxL68x_Signal_Information[nUnitId].fSignalInputPower = RF_POWER/100+107; //dBuV
#endif
		MxL68x_Signal_Information[nUnitId].fSnr = (float)cnrPtr/1000;
		//Converting AGC Gain value to RF strength
		MxL68x_Signal_Information[nUnitId].ulAgc = 100;
		MxL68x_Signal_Information[nUnitId].ulStrength= 100;
		MxL68x_Signal_Information[nUnitId].ulQuality= 100;

		MxL68x_Signal_Information[nUnitId].ulCorrectedNo += BerPtr.bitErrCount_WO_Iter -BerPtr.bitErrCount_W_Iter;
		MxL68x_Signal_Information[nUnitId].ulUnCorrectedNo += BerPtr.bitErrCount_W_Iter;
		MxL68x_Signal_Information[nUnitId].ulUnErroredNo += BerPtr.numOfBits -BerPtr.bitErrCount_WO_Iter ;
		if (BerPtr.bitErrCount_W_Iter > 0)
			MxL68x_Signal_Information[nUnitId].ulUnCorrectedFlag= 1;
		else
			MxL68x_Signal_Information[nUnitId].ulUnCorrectedFlag= 0;

		if (BerPtr.numOfBits !=0)
			MxL68x_Signal_Information[nUnitId].fBer = (float)(BerPtr.bitErrCount_WO_Iter*1000000/BerPtr.numOfBits);

		CH_DI_Print(CH_LOG_2, ("[MXL683] MXL683_CheckLock_for_tune[%d]:: RF_POWER =%f, CNR = %f \n ",nChannelId,MxL68x_Signal_Information[nUnitId].fSignalInputPower,MxL68x_Signal_Information[nUnitId].fSnr));
		//CH_DI_Print(0, ("[MXL683] MXL683_GetSignalInfo[%d]:: rawAgcGainPtr =%d , cnrRawPtr = %d, cnrPtr = %d \n ",nChannelId,rawAgcGainPtr,cnrRawPtr, cnrPtr));
		CH_DI_Print(CH_LOG_2, ("[MXL683] MXL683_CheckLock_for_tune[%d]:: Previous Error Bit =%d \n ", nChannelId,BerPtr.bitErrCount_WO_Iter));
		CH_DI_Print(CH_LOG_2, ("[MXL683] MXL683_CheckLock_for_tune[%d]:: UnCorrecctable Error Bit =%d \n ", nChannelId,BerPtr.bitErrCount_W_Iter));
		CH_DI_Print(CH_LOG_2, ("[MXL683] MXL683_CheckLock_for_tune[%d]:: Total Bits =%d \n ", nChannelId,BerPtr.numOfBits));
		CH_DI_Print(CH_LOG_2, ("[MXL683] MXL683_CheckLock_for_tune[%d]:: fBer =%f \n ", nChannelId,MxL68x_Signal_Information[nUnitId].fBer));
		//CH_DI_Print(1, ("[MXL683] MXL683_GetSignalInfo[%d]:: BerPtr.numOfBits =%d \n ", nChannelId,BerPtr.numOfBits));

		return TRUE;
	 }

  }

 HBOOL MXL683_CheckLock ( int nChannelId, HUINT32 unWorkingID )
 {
	int	 nResult = DI_CH_OK;
	MXL_BOOL_E rsLockA, rsLockB, rsLockC, tmccLock;
	MXL68x_TMCC_INFO_T tmccInfoLayerA, tmccInfoLayerB, tmccInfoLayerC;
	HUINT8 LOCK_683 = FALSE;
	HUINT32 cnrRawPtr;
	HINT32 cnrPtr;
	HINT32 RF_POWER;
	MXL68x_BER_T BerPtr;
	HUINT8 numFramesPtr;
	MXL_BOOL_E berEndFlag;
	CH_TER_TuneParam_t *pstTerTuneParam;
	HUINT32 frequency;
	HINT32	nUnitId;

	nUnitId = GetDeviceUnitId( nChannelId );

	 CH_UNUSED(unWorkingID);

	 pstTerTuneParam = (CH_TER_TuneParam_t *)GetWorkingTuneInfo( nChannelId );
	 frequency = pstTerTuneParam->ulFrequency;
	 if(((frequency -143) / 1000) != (frequency /1000)) /* 만약 143KHz가 안붙어서 오는 경우 */
		 frequency += 143;

	 // Check Demod lock status
	 nResult = MxLWare68x_API_ReqDemodLockStatus(nChannelId, &tmccLock, &rsLockA, &rsLockB, &rsLockC);
	 if (nResult != MXL_SUCCESS)
		 CH_DI_Print(CH_LOG_0, ("[MXL683] MXL683_CheckLock...MxLWare68x_API_ReqDemodLockStatus()  FAIL (%d)! \n",nResult));
	 else
		 CH_DI_Print(CH_LOG_4, ("[MXL683] MXL683_CheckLock...MxLWare68x_API_ReqTunerLockStatus()...	(tmccLock = %d, rsLockA = %d, rsLockB = %d, rsLockC = %d)! \n",tmccLock, rsLockA, rsLockB, rsLockC));


	 if (tmccLock)
	 {
		 LOCK_683 = FALSE;
		 MxLWare68x_API_ReqDemodTMCCParams(nChannelId, NULL, NULL, NULL, &tmccInfoLayerA, &tmccInfoLayerB, &tmccInfoLayerC);
		 if( ((tmccInfoLayerA.numOfSegments > 0) && (tmccInfoLayerA.numOfSegments < 14))
			 & (tmccInfoLayerA.codeRateInfo <= MXL68x_CR_7_8)
			 &(tmccInfoLayerA.modulationInfo <= MXL68x_MODULATION_QAM64)
			 & (tmccInfoLayerA.interleaverLenInfo <= MXL68x_INTERLEAVER_INDEX3))
		 {
			 CH_DI_Print(CH_LOG_2, ("MXL683_CheckLock[]:: LrsLockA ~~~~~ \n"));
			 LOCK_683 |= rsLockA;
		 }
		 if( ((tmccInfoLayerB.numOfSegments > 0) && (tmccInfoLayerB.numOfSegments < 14))
			 & (tmccInfoLayerB.codeRateInfo <= MXL68x_CR_7_8)
			 &(tmccInfoLayerB.modulationInfo <= MXL68x_MODULATION_QAM64)
			 & (tmccInfoLayerB.interleaverLenInfo <= MXL68x_INTERLEAVER_INDEX3))
		 {
			 CH_DI_Print(CH_LOG_2, ("MXL683_CheckLock[]:: rsLockB ~~~~~ \n"));
			 LOCK_683 |= rsLockB;
		 }
		 if( ((tmccInfoLayerC.numOfSegments > 0) && (tmccInfoLayerC.numOfSegments < 14))
			 & (tmccInfoLayerC.codeRateInfo <= MXL68x_CR_7_8)
			 &(tmccInfoLayerC.modulationInfo <= MXL68x_MODULATION_QAM64)
			 & (tmccInfoLayerC.interleaverLenInfo <= MXL68x_INTERLEAVER_INDEX3))
		 {
			 CH_DI_Print(CH_LOG_2, ("MXL683_CheckLock[]:: rsLockC ~~~~~ \n"));
			 LOCK_683 |= rsLockC;
		 }
	 }
	 CH_DI_Print(CH_LOG_2, ("[MXL683] MXL683_CheckLock...MxLWare68x_API_ReqTunerLockStatus()...	(LOCK_683 = %d)! \n",LOCK_683));


	 if (LOCK_683 != TRUE)
	 {
		 CH_DI_Print(CH_LOG_2, ("MXL683_CheckLock[%d]:: LOCK FAIL ~~~~~ \n", nChannelId));
		 MxL68x_Signal_Information[nUnitId].fSignalInputPower = 0; //dBuV
		 MxL68x_Signal_Information[nUnitId].fSnr = 0;
		 //Converting AGC Gain value to RF strength
		 MxL68x_Signal_Information[nUnitId].ulAgc = 0;
		 MxL68x_Signal_Information[nUnitId].ulStrength= 0;
		 MxL68x_Signal_Information[nUnitId].ulQuality= 0;

		 MxL68x_Signal_Information[nUnitId].ulCorrectedNo= 0;
		 MxL68x_Signal_Information[nUnitId].ulUnCorrectedNo= 0;
		 MxL68x_Signal_Information[nUnitId].ulUnErroredNo= 0 ;
		 MxL68x_Signal_Information[nUnitId].ulUnCorrectedFlag= 0;

		 MxL68x_Signal_Information[nUnitId].fBer = 0;
		 return FALSE;
	 }
	 else
	 {
		 MxL68x_Signal_Information[nUnitId].fBer = 0;
		 MxLWare68x_API_ReqDemodCNR(nChannelId, &cnrRawPtr, &cnrPtr);
		 MxLWare68x_API_ReqTunerRxPower(nChannelId, &RF_POWER);

		 MxLWare68x_API_ReqDemodBerPerEndFlag(nChannelId, &numFramesPtr, &berEndFlag);
		 //CH_DI_Print(0, ("[MXL683] MXL683_GetSignalInfo[%d]:: numFramesPtr =%d , berEndFlag = %d,  \n ",nChannelId,numFramesPtr,berEndFlag));

		nResult = MxLWare68x_API_ReqDemodBER(nChannelId, &BerPtr);
		if (nResult == MXL_SUCCESS)
		{
			if (BerPtr.bitErrCount_WO_Iter < 300)
			{
				BerPtr.bitErrCount_WO_Iter = 0;
				BerPtr.bitErrCount_W_Iter = 0;
				BerPtr.numOfBits = 0;
			}

		}

		MxLWare68x_API_CfgDemodClearBerPerEndFlag(nChannelId);

#ifdef CONFIG_LONG_RF_TRACE //WA-7500
		if (frequency <= 423143)
			MxL68x_Signal_Information[nUnitId].fSignalInputPower = RF_POWER/100+104; //dBuV
		else if (frequency <= 665143)
			MxL68x_Signal_Information[nUnitId].fSignalInputPower = RF_POWER/100+106; //dBuV
		else if (frequency <= 767143)
			MxL68x_Signal_Information[nUnitId].fSignalInputPower = RF_POWER/100+107; //dBuV
#else	//WA-8500
		if (frequency <= 497143)
			MxL68x_Signal_Information[nUnitId].fSignalInputPower = RF_POWER/100+103; //dBuV
		else if (frequency <=767143)
			MxL68x_Signal_Information[nUnitId].fSignalInputPower = RF_POWER/100+107; //dBuV
#endif
		MxL68x_Signal_Information[nUnitId].fSnr = (float)cnrPtr/1000;
		//Converting AGC Gain value to RF strength
		MxL68x_Signal_Information[nUnitId].ulAgc = 100;
		MxL68x_Signal_Information[nUnitId].ulStrength= 100;
		MxL68x_Signal_Information[nUnitId].ulQuality= 100;

		MxL68x_Signal_Information[nUnitId].ulCorrectedNo += BerPtr.bitErrCount_WO_Iter -BerPtr.bitErrCount_W_Iter;
		MxL68x_Signal_Information[nUnitId].ulUnCorrectedNo += BerPtr.bitErrCount_W_Iter;
		MxL68x_Signal_Information[nUnitId].ulUnErroredNo += BerPtr.numOfBits -BerPtr.bitErrCount_WO_Iter ;
		if (BerPtr.bitErrCount_W_Iter > 0)
			MxL68x_Signal_Information[nUnitId].ulUnCorrectedFlag= 1;
		else
			MxL68x_Signal_Information[nUnitId].ulUnCorrectedFlag= 0;

		if (BerPtr.numOfBits !=0)
			MxL68x_Signal_Information[nUnitId].fBer = (float)(BerPtr.bitErrCount_WO_Iter*1000000/BerPtr.numOfBits);

		CH_DI_Print(CH_LOG_2, ("[MXL683] MXL683_CheckLock[%d]:: RF_POWER =%f, CNR = %f \n ",nChannelId,MxL68x_Signal_Information[nUnitId].fSignalInputPower,MxL68x_Signal_Information[nUnitId].fSnr));
		//CH_DI_Print(0, ("[MXL683] MXL683_GetSignalInfo[%d]:: rawAgcGainPtr =%d , cnrRawPtr = %d, cnrPtr = %d \n ",nChannelId,rawAgcGainPtr,cnrRawPtr, cnrPtr));
		CH_DI_Print(CH_LOG_2, ("[MXL683] MXL683_CheckLock[%d]:: Previous Error Bit =%d \n ", nChannelId,BerPtr.bitErrCount_WO_Iter));
		CH_DI_Print(CH_LOG_2, ("[MXL683] MXL683_CheckLock[%d]:: UnCorrecctable Error Bit =%d \n ", nChannelId,BerPtr.bitErrCount_W_Iter));
		CH_DI_Print(CH_LOG_2, ("[MXL683] MXL683_CheckLock[%d]:: Total Bits =%d \n ", nChannelId,BerPtr.numOfBits));
		CH_DI_Print(CH_LOG_2, ("[MXL683] MXL683_CheckLock[%d]:: fBer =%f \n ", nChannelId,MxL68x_Signal_Information[nUnitId].fBer));
		//CH_DI_Print(1, ("[MXL683] MXL683_GetSignalInfo[%d]:: BerPtr.numOfBits =%d \n ", nChannelId,BerPtr.numOfBits));

		return TRUE;
	 }

  }

void MXL683_ResetGpio(int nChannelId)
 {
	 int nGpioNum = 0;
	HINT32	nUnitId;

	nUnitId = GetDeviceUnitId( nChannelId );

	PMXL68x_DEV_CONTEXT_T devContextPtr = MxL68x_Ctrl_GetDeviceContext(nUnitId);
#if 1
	 switch(nUnitId)
	 {
		 case 0:
			 nGpioNum = GPIO_ID_nCH0_RST;
			 break;
		 case 1:
			 nGpioNum = GPIO_ID_nCH1_RST;
			 break;
		 case 2:
			 nGpioNum = GPIO_ID_nCH2_RST;
			 break;
		 default:
			 nGpioNum = GPIO_ID_nCH0_RST;
			 CH_DI_Print(CH_LOG_0, (" Error : MXL683_HW_Reset... nChannelId[%d]... \n", nChannelId));
			 break;
	 }


	 DRV_GPIO_Write(nGpioNum, 0);
	 VK_TASK_Sleep(30);
	 DRV_GPIO_Write(nGpioNum, 1);
	 VK_TASK_Sleep(100);
#else

#if 1
	 while (1)
		 {
			 DRV_GPIO_Write(GPIO_ID_nCH0_RST, 1);
			 VK_TASK_Sleep(100);
			 DRV_GPIO_Write(GPIO_ID_nCH0_RST, 0);
			 VK_TASK_Sleep(100);
			 DRV_GPIO_Write(GPIO_ID_nCH0_RST, 1);
			 VK_TASK_Sleep(100);
		 }
#else
 DRV_GPIO_Write(GPIO_ID_nCH1_RST, 0);
 VK_TASK_Sleep(100);
 DRV_GPIO_Write(GPIO_ID_nCH1_RST, 1);
 VK_TASK_Sleep(100);
#endif
#endif
	 CH_DI_Print(CH_LOG_4, (" MXL683_HW_Reset...GPIO_ID_nCH0[%d]_RST reset !!\n", nChannelId));


 } /* end of ResetHwTda10048 */


 int  MXL683_GetSignalInfo(int nChannelId, DI_CH_SignalQuality_t *pstSignalInfo)
 {
	HINT32	nUnitId;

	nUnitId = GetDeviceUnitId( nChannelId );

#if defined(CONFIG_PRODUCT_IMAGE_FACTORY)
		 MXL683_CheckLock(nChannelId, 0);
#endif
	 //CH_DI_Print(0, ("[MXL683] MXL683_GetSignalInfo[%d]:: rawAgcGainPtr =%d , cnrRawPtr = %d, cnrPtr = %d \n ",nChannelId,rawAgcGainPtr,cnrRawPtr, cnrPtr));
	// CH_DI_Print(0, ("[MXL683] MXL683_GetSignalInfo[%d]:: layerABerPtr.bitErrCount_W_Iter =%d \n ", nChannelId,layerABerPtr.bitErrCount_W_Iter));
	 //CH_DI_Print(0, ("[MXL683] MXL683_GetSignalInfo[%d]:: layerABerPtr.bitErrCount_WO_Iter =%d \n ", nChannelId,layerABerPtr.bitErrCount_WO_Iter));
	 //CH_DI_Print(0, ("[MXL683] MXL683_GetSignalInfo[%d]:: layerABerPtr.numOfBits =%d \n ", nChannelId,layerABerPtr.numOfBits));
	 pstSignalInfo->ulStrength	 			= MxL68x_Signal_Information[nUnitId].ulStrength;
	 pstSignalInfo->ulAgc		 			= MxL68x_Signal_Information[nUnitId].ulAgc;
	 pstSignalInfo->ulQuality	 			= MxL68x_Signal_Information[nUnitId].ulQuality;
	 pstSignalInfo->fSnr 		 			= MxL68x_Signal_Information[nUnitId].fSnr;
	 pstSignalInfo->fSignalInputPower 		= MxL68x_Signal_Information[nUnitId].fSignalInputPower;
	 pstSignalInfo->ulUnCorrectedNo 		= MxL68x_Signal_Information[nUnitId].ulUnCorrectedNo;
	 pstSignalInfo->fBer 					= MxL68x_Signal_Information[nUnitId].fBer;
	 pstSignalInfo->ulUnCorrectedFlag 		= MxL68x_Signal_Information[nUnitId].ulUnCorrectedFlag;
	 pstSignalInfo->fBer					= MxL68x_Signal_Information[nUnitId].fBer;
	 pstSignalInfo->ulCorrectedNo 			= MxL68x_Signal_Information[nUnitId].ulCorrectedNo;
	 pstSignalInfo->ulUnErroredNo			= MxL68x_Signal_Information[nUnitId].ulUnErroredNo;


	 CH_DI_Print(CH_LOG_2, ("[MXL683] GetStrengthQuality[%d]:: Strength =%d , ulAgc = %d, ulQuality = %d, fSnr = %f , fBer = %f \n ",
					 nChannelId,pstSignalInfo->ulStrength,pstSignalInfo->ulAgc, pstSignalInfo->ulQuality, pstSignalInfo->fSnr,pstSignalInfo->fBer));


	 return DI_CH_OK;

 }

 int  MXL683_SetPowerState( int nChannelId, CH_Power_e etPower )
 {
	 /*----------------------*/
	 /* Implementation*/
	 /*----------------------*/
	 switch (etPower)
	 {
		 case CH_POWER_OFF:  //tmPowerStandby = 1

			 SetActiveStatus( nChannelId, CH_ACTION_OFF );

			 CH_DI_Print(CH_LOG_4, ("\n ------------- Sleep mode[%d] -------------\n", nChannelId));

			 MxLWare68x_API_CfgDevPowerMode(nChannelId,MXL68x_PWR_MODE_STANDBY);

			 break;


		 case CH_POWER_ON: //tmPowerOn = 0
			 default:

			 SetActiveStatus( nChannelId, CH_ACTION_ON );

			 CH_DI_Print(CH_LOG_4, ("\n ------------- WakeUp mode[%d] -------------\n",nChannelId));
			 //MXL683_ResetGpio(nChannelId);

			 MxLWare68x_API_CfgDevPowerMode(nChannelId,MXL68x_PWR_MODE_ACTIVE );
			 break;
	 }


	 return DI_CH_OK;
 }

 int MXL683_EnableTsOutput ( int nChannelId )
 {
#if 0
	CH_UNUSED(nChannelId);
	return DI_CH_OK;
#else
	MXL_STATUS_E status = MXL_SUCCESS;

	 CH_DI_Print(CH_LOG_4,("[MXL683] TS Enable[%d] \n",nChannelId));

	status = MxLWare68x_API_CfgDemodMPEGOut(
                nChannelId, // OK
                MXL68x_MPEG_MODE_NORMAL, // OK 4-wired IF
                MXL68x_MPEG_CLK_64MHZ,	    // ??
                MXL68x_MPEG_DATA_MSB_FIRST, // OK
                MXL68x_MPEG_CLK_PHASE_SHIFT_180_DEG,  // ??
                MXL68x_POLARITY_NORMAL,	// ??
                MXL68x_POLARITY_NORMAL,	// ??
                MXL68x_MPEG_SYNC_PULSE_1BYTE,	// ??
                MXL68x_MPEG_ERROR_IND_ENABLE,	// OK
                MXL68x_TS_PKT_188_BYTES,
                MXL68x_TS_DRV_STR_1X);    // ??

	if (status != MXL_SUCCESS)
		return DI_CH_ERR;
	else
		return DI_CH_OK;
#endif

 }

 int MXL683_DisableTsOutput ( int nChannelId )
 {
#if 0
	CH_UNUSED(nChannelId);
	return DI_CH_OK;
#else
	MXL_STATUS_E status = MXL_SUCCESS;

	 CH_DI_Print(CH_LOG_4,("[MXL683] TS Disable[%d] \n",nChannelId));

	status = MxLWare68x_API_CfgDemodMPEGOut(
                nChannelId, // OK
                MXL68x_MPEG_MODE_NORMAL, // OK 4-wired IF
                MXL68x_MPEG_CLK_64MHZ,	    // ??
                MXL68x_MPEG_DATA_MSB_FIRST, // OK
                MXL68x_MPEG_CLK_PHASE_SHIFT_180_DEG,  // ??
                MXL68x_POLARITY_NORMAL,	// ??
                MXL68x_POLARITY_INVERTED,	// ??
                MXL68x_MPEG_SYNC_PULSE_1BYTE,	// ??
                MXL68x_MPEG_ERROR_IND_ENABLE,	// OK
                MXL68x_TS_PKT_188_BYTES,
                MXL68x_TS_DRV_STR_1X);    // ??

	if (status != MXL_SUCCESS)
		return DI_CH_ERR;
	else
		return DI_CH_OK;
#endif
 }

 int MXL683_SetTsOutputMode( int nChannelId, CH_TsOutMode_e etTsMode )
 {
	 CH_UNUSED(nChannelId);
	CH_UNUSED(etTsMode);
	 CH_DI_Print(CH_LOG_4,("[MXL683] TS Disable[%d] \n",nChannelId));

	 return DI_CH_OK;
 }


 void MXL683_ProcessRecoverAntenna( int nChannelId, HUINT32 unWorkingId )
 {

	 CH_UNUSED(nChannelId);
	 CH_UNUSED(unWorkingId);

 }


 int MXL683_SetAntState( int nChannelId, CH_TER_AntStatus_e etAntState )
 {
	 int nUnitId;
	 int nRet=0;
	 HUINT32 unWorkingId;
	 int nActiveState;

	CH_UNUSED(etAntState);

	 nUnitId = GetDeviceUnitId( nChannelId );
	 unWorkingId = GetRequestedId( nChannelId );

	 s_stAntState[nUnitId].etDesiredAntState = etAntState;

	 if( s_stAntState[nUnitId].etCurrentAntState != DI_CH_ANTENNA_FAULT )
	 {
		 if(etAntState < DI_CH_ANTENNA_FAULT)
		 {
#if 0 // nhlee delete temporary
			 nRet = DRV_CH_SetGpio( GPIO_ANT_PWR_EN, etAntState );
#endif
			 if( nRet < 0 )
			 {
				 CH_DI_Print(CH_LOG_4, (""));
				 return DI_CH_ERR;
			 }
		 }
		 else
		 {
			 CH_DI_Print(CH_LOG_4, (""));
		 }
		 s_stAntState[nUnitId].etCurrentAntState = etAntState;

	 }
	 else if( s_stAntState[nUnitId].etCurrentAntState == DI_CH_ANTENNA_FAULT )
	 {

		 if( etAntState == DI_CH_ANTENNA_OFF )
		 {
#if 0 // nhlee delete temporary
			 nRet = DRV_CH_SetGpio( GPIO_ANT_PWR_EN, etAntState );
#endif
			 if( nRet < 0 )
			 {
				 CH_DI_Print(CH_LOG_4, (""));
				 return DI_CH_ERR;
			 }

			 s_stAntState[nUnitId].etCurrentAntState = etAntState;

			 DRV_CH_CallNotifyCallback( nChannelId, unWorkingId, DI_CH_SIGNAL_ANTENNA_OK );

			 nActiveState = GetActiveStatus( nChannelId );
			 if( nActiveState == CH_ACTION_OFF )
			 {
				 SetTuneStatus( nChannelId, CH_STATUS_IDLE );
			 }
			 else if( nActiveState == CH_ACTION_ON )
			 {
				 SetTuneStatus( nChannelId, CH_STATUS_UNLOCK );
			 }
			 else
			 {
				 //Do nothing
			 }

		 }
		 else
		 {
			 return DI_CH_HW_RESTRICTION;
		 }
	 }

	 return DI_CH_OK;

 }


 CH_TER_AntStatus_e MXL683_GetAntState( int nChannelId )
 {
	 int nUnitId;

	 nUnitId = GetDeviceUnitId( nChannelId );

	 return s_stAntState[nUnitId].etCurrentAntState;

 }

 int MXL683_CheckAntState( int nChannelId )
 {

	 CH_UNUSED(nChannelId);

	 return DI_CH_OK;

 }






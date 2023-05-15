/*******************************************************************************
 *
 * FILE NAME          : MxL241SF_OEM_Drv.c
 *
 * AUTHOR             : Brenndon Lee
 * DATE CREATED       : 7/30/2009
 *
 * DESCRIPTION        : This file contains I2C driver functins that OEM should
 *                      implement for MxL241SF APIs
 *
 *******************************************************************************
 *                Copyright (c) 2006, MaxLinear, Inc.
 ******************************************************************************/

#include "MxL241SF_OEM_Drv.h"
#include "htype.h"
#include "di_channel_priv.h"
#include "drv_i2c.h"
#include "drv_channel_mxl241sf.h"

//#define JD_DEBUG_I2C
#define	MASTER_MXL241SF_I2C_BUS	2
#define	SLAVE_MXL241SF_I2C_BUS	0


extern HINT32 show_di_ch;

/*------------------------------------------------------------------------------
--| FUNCTION NAME : Ctrl_WriteRegister
--|
--| AUTHOR        : Brenndon Lee
--|
--| DATE CREATED  : 7/30/2009
--|
--| DESCRIPTION   : This function does I2C write operation.
--|
--| RETURN VALUE  : True or False
--|
--|---------------------------------------------------------------------------*/

MXL_STATUS Ctrl_WriteRegister(UINT8 I2cSlaveAddr, UINT16 RegAddr, UINT16 RegData)
{
	MXL_STATUS	status = MXL_TRUE;
	UINT8		ucI2CBus		= MASTER_MXL241SF_I2C_BUS;
	UINT8		ucRet			= DRV_OK;
	UINT8		ucDataTmp[2]	= {0,0};
	SINT32		nChannelId		= MASTER_MXL241SF;

	if (I2cSlaveAddr == MASTER_MXL241SF_I2C_ADDR)
	{
		ucI2CBus = I2C_CHANNEL_CHANNEL;
		nChannelId = MASTER_MXL241SF;
	}
	else
	{
#if defined(CONFIG_I2C_MAP_FOR_2TUNER)		
		ucI2CBus = I2C_CHANNEL_CHANNEL2;
#else
		ucI2CBus = I2C_CHANNEL_CHANNEL;
#endif
		nChannelId = SLAVE_MXL241SF;
	}
#if defined(JD_DEBUG_I2C)
	CH_DI_Print(0,(">>>>> I2C_Write[%d] :: I2cSlaveAddr = 0x%x, RegAddr = 0x%x, RegData = 0x%4x \n",nChannelId, I2cSlaveAddr, RegAddr, RegData));
#endif

	ucDataTmp[0] = (UINT8)(RegData >> 8);
	ucDataTmp[1] = (UINT8)(RegData & 0x00FF);

#if defined(JD_DEBUG_I2C)
	CH_DI_Print(0,("----- I2C_Write[%d] :: I2cSlaveAddr = 0x%x, RegAddr = 0x%x, RegData = 0x%2x%2x \n",nChannelId, I2cSlaveAddr, RegAddr, ucDataTmp[0], ucDataTmp[1]));
#endif

	ucRet = DRV_I2c_WriteA16(ucI2CBus, I2cSlaveAddr, RegAddr, &ucDataTmp[0], 2);
	if (ucRet != DRV_OK)
	{
		CH_DI_Print(0,("Ctrl_WriteRegister[%d] :: Write Fail.. ucRet(0x%x) \n", nChannelId, ucRet));
		status = MXL_FALSE;
	}

	return status;
}

/*------------------------------------------------------------------------------
--| FUNCTION NAME : Ctrl_ReadRegister
--|
--| AUTHOR        : Brenndon Lee
--|
--| DATE CREATED  : 7/30/2009
--|
--| DESCRIPTION   : This function does I2C read operation.
--|
--| RETURN VALUE  : True or False
--|
--|---------------------------------------------------------------------------*/

MXL_STATUS Ctrl_ReadRegister(UINT8 I2cSlaveAddr, UINT16 RegAddr, UINT16 *DataPtr)
{
	MXL_STATUS	status			= MXL_TRUE;
	UINT8		ucI2CBus		= MASTER_MXL241SF_I2C_BUS;
	UINT8		ucRet			= DRV_OK;
	UINT8		ucDataTmp[2]	= {0,};
	UINT8		ucAddrTmp[4]	= {0,};
	SINT32		nChannelId		= MASTER_MXL241SF;

	if (I2cSlaveAddr == MASTER_MXL241SF_I2C_ADDR)
	{
		ucI2CBus = I2C_CHANNEL_CHANNEL;
		nChannelId = MASTER_MXL241SF;
	}
	else
	{
#if defined(CONFIG_I2C_MAP_FOR_2TUNER)		
		ucI2CBus = I2C_CHANNEL_CHANNEL2;
#else
		ucI2CBus = I2C_CHANNEL_CHANNEL;
#endif
		nChannelId = SLAVE_MXL241SF;
	}

#if defined(JD_DEBUG_I2C)
	CH_DI_Print(0,(">>>>> I2C_Read[%d]  :: I2cSlaveAddr = 0x%x, RegAddr = 0x%x \n",nChannelId, I2cSlaveAddr, RegAddr));
#endif

	ucAddrTmp[0] = 0xFF;
	ucAddrTmp[1] = 0xFB;
	ucAddrTmp[2] = (UINT8)(RegAddr >> 8);
	ucAddrTmp[3] = (UINT8)(RegAddr & 0x00FF);

	ucRet = DRV_I2c_WriteNoAddr(ucI2CBus, I2cSlaveAddr, &ucAddrTmp[0], 4);
	if (ucRet == DRV_OK)
	{
		ucRet = DRV_I2c_ReadNoAddr(ucI2CBus, I2cSlaveAddr, &ucDataTmp[0], 2);
		if (ucRet == DRV_OK)
		{
			*DataPtr = (UINT16)((ucDataTmp[0]<<8) | ucDataTmp[1]);
#if defined(JD_DEBUG_I2C)
			CH_DI_Print(0,("----- I2C_Read[%d]  :: I2cSlaveAddr = 0x%x, RegAddr = 0x%x RegData = 0x%4x\n",nChannelId, I2cSlaveAddr, RegAddr, *DataPtr));
#endif
			status = MXL_TRUE;
		}
		else
		{
			CH_DI_Print(0,("Ctrl_ReadRegister[%d] :: Read Fail.. ucRet(0x%x) \n", nChannelId, ucRet));
			status = MXL_FALSE;
		}
	}
	else
	{
		CH_DI_Print(0,("Ctrl_ReadRegister[%d] :: Write Fail.. ucRet(0x%x) \n", nChannelId, ucRet));
		status = MXL_FALSE;
	}

	return status;
}


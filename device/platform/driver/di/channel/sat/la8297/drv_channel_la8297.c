/* $Header$ */
/*******************************************************************/
/************************* File Description ************************/
/*******************************************************************/
// File Name:
// Original Author: Full Name!!
// File Description:
// Module:
// Remarks:

/*******************************************************************/
/* Copyright (c) 2006 HUMAX Co., Ltd. 										         */
/* All rights reserved.																	           */
/*******************************************************************/


/*******************************************************************/
/**************************** Header Files *************************/
/*******************************************************************/
// Start Including Header Files
/* chip headers */
#include "htype.h"
#include "vkernel.h"

/* di headers */
#include "di_channel.h"
#include "di_channel_priv.h"
#include "di_channel_attr.h"

#include "drv_channel.h"
#include "drv_channel_lnb.h"
#include "drv_i2c.h"
#include "drv_gpio.h"
// End Including Headers


/*******************************************************************/
/****************************** define *****************************/
/*******************************************************************/
// Start #define
#define A8297_I2C_ADDR (0x10>>1)
#define A8297_CTRL_REG (0x0)
#define A8297_STATUS_REG (0x0)
#define CONST_RW_LENGTH (1)

#define NUM_OF_A8297 	(1)
#define FIRST_A8297		(0)

#define MASTER_CHANNEL	(0)

// M : master unit, S : slave unit
#define CTRL_VSEL0_M 	(0x01)
#define CTRL_VSEL1_M 	(0x02)
#define CTRL_VSEL2_M 	(0x04)
#define CTRL_ENB_M 		(0x08)
#define CTRL_VSEL0_S 	(0x10)
#define CTRL_VSEL1_S 	(0x20)
#define CTRL_VSEL2_S 	(0x40)
#define CTRL_ENB_S 		(0x80)

#define CTRL_OCP_M 		(0x04)
#define CTRL_OCP_S 		(0x08)
// End #define


/*******************************************************************/
/****************************** typedef ****************************/
/*******************************************************************/
// Start typedef

// End typedef


/*******************************************************************/
/************************ global variables *************************/
/*******************************************************************/
// Start global variablee
extern HINT32 show_di_ch;
HUINT8 g_hwtest_lnb_off = 0;//It will bi deprecated...
// End global variable


/*******************************************************************/
/************************ static variables *************************/
/*******************************************************************/
// Start static variablee
static HUINT8 s_aucCtrData[NUM_OF_A8297] = {0,};

static int WriteLnbReg(
		int	nUnitId,
		HUINT8 * pucWrData
		)
{

	int ErrCode = DI_CH_OK;

#if defined(CONFIG_PLATFORM_DB4506)
	DRV_CH_LockI2c(nUnitId);
#endif

	ErrCode = DRV_I2c_WriteA8(I2C_CHANNEL_CHANNEL,A8297_I2C_ADDR,A8297_CTRL_REG, pucWrData,CONST_RW_LENGTH);
	if( ErrCode != DI_CH_OK )
	{
		CH_DI_Print(0, ( "[A8297] WriteLnbReg nUnitId(%d) Error(0x%x)\n", nUnitId, ErrCode));
		ErrCode = DI_CH_ERR;
	}
	else
	{
		s_aucCtrData[FIRST_A8297] = *pucWrData;
		CH_DI_Print(7, ( "\t\t>>>>> WriteLnbReg nUnitId(%d) s_aucCtrData[0](0x%x)\n", nUnitId, s_aucCtrData[FIRST_A8297]));
	}

#if defined(CONFIG_PLATFORM_DB4506)
	DRV_CH_UnLockI2c(nUnitId);
#endif

	return ErrCode;
} /* end of WriteLnbReg */


/********************************************************************************
 *	Function	: ReadLnbReg
 *	Description :
 *	Input		:
 *	Output		:
 *	Return		:
 ********************************************************************************/

static int ReadLnbReg(
		int nUnitId,
		HUINT8 * pucRdData
		)
{

	int ErrCode = DI_CH_OK;
#if 0
	DRV_CH_LockI2c(nUnitId);
	//ErrCode = DRV_I2c_WriteA8(I2C_CHANNEL_CHANNEL,A8297_I2C_ADDR,A8297_CTRL_REG, pucWrData,CONST_RW_LENGTH);
	//ErrCode = DRV_I2c_ReadA8(I2C_CHANNEL_CHANNEL,A8297_I2C_ADDR,A8297_STATUS_REG, pucRdData,CONST_RW_LENGTH);
	ErrCode = DRV_I2c_ReadA16(I2C_CHANNEL_CHANNEL,A8297_I2C_ADDR,A8297_STATUS_REG, pucRdData,CONST_RW_LENGTH);
	if ( ErrCode != DI_CH_OK )
	{
		CH_DI_Print(0, ( "[A8297] ReadLnbReg nUnitId(%d) Error(0x%x)\n", nUnitId, ErrCode));
		ErrCode = DI_CH_ERR;
	}
#else
	HUINT8 ucRdData = 0;

#if defined(CONFIG_PLATFORM_DB4506)
	DRV_CH_LockI2c(nUnitId);
#endif

	ErrCode = DRV_I2c_WriteNoAddr(I2C_CHANNEL_CHANNEL,A8297_I2C_ADDR, &ucRdData,CONST_RW_LENGTH);
	if ( ErrCode != DI_CH_OK )
	{
		CH_DI_Print(0, ( "[A8297] ReadLnbReg nUnitId(%d) Error(0x%x)\n", nUnitId, ErrCode));
		return DI_CH_ERR;
	}

	ErrCode = DRV_I2c_ReadNoAddr(I2C_CHANNEL_CHANNEL,A8297_I2C_ADDR, pucRdData,CONST_RW_LENGTH);
	if ( ErrCode != DI_CH_OK )
	{
		CH_DI_Print(0, ( "[A8297] ReadLnbReg nUnitId(%d) Error(0x%x)\n", nUnitId, ErrCode));
		ErrCode = DI_CH_ERR;
	}
	else
	{
		//CH_DI_Print(0, ( "ReadLnbReg (nUnitId(%d)) *pucRdDatas = (0x%x)\n", nUnitId, *pucRdData));
	}
#endif

#if defined(CONFIG_PLATFORM_DB4506)
	DRV_CH_UnLockI2c(nUnitId);
#endif

	return ErrCode;

}/* end of ReadLnbReg */


/********************************************************************************
 *	Function	: ResetGpioLnb
 *	Description :
 *	Input		:
 *	Output		:
 *	Return		:
 ********************************************************************************/
static int ResetGpioLnb(int nUnitId)
{
	HINT32 nRet = DI_CH_OK;

	if (nUnitId == MASTER_CHANNEL)
	{
		nRet = DRV_GPIO_Write(GPIO_ID_nLNBP0_RST, GPIO_HI);
		if (nRet != DI_CH_OK)
		{
			CH_DI_Print(0,("ResetGpioLnb[%d] :: DRV_GPIO_Write to GPIO_HI Error [%d]\n",nUnitId,nRet));
			return DI_CH_ERR;
		}
		VK_TASK_Sleep(20);

		nRet = DRV_GPIO_Write(GPIO_ID_nLNBP0_RST, GPIO_LOW);
		if (nRet != DI_CH_OK)
		{
			CH_DI_Print(0,("ResetGpioLnb[%d] :: DRV_GPIO_Write to GPIO_LOW Error [%d]\n",nUnitId,nRet));
			return DI_CH_ERR;
		}
		VK_TASK_Sleep(20);

		nRet = DRV_GPIO_Write(GPIO_ID_nLNBP0_RST, GPIO_HI);
		if (nRet != DI_CH_OK)
		{
			CH_DI_Print(0,("ResetGpioLnb[%d] :: DRV_GPIO_Write to GPIO_HI Error [%d]\n",nUnitId,nRet));
			return DI_CH_ERR;
		}
		VK_TASK_Sleep(20);
	}
	else
	{
		CH_DI_Print(6,("ResetGpioLnb[%d] :: No Need to Control Reset GPIO for LNBP\n",nUnitId));
	}

	return DI_CH_OK;
}
// End static variable


/****************** Function 시작점에 필히 기록 ********************/
/*******************************************************************/
/*********************** Function Description***********************/
/*******************************************************************/
// Function Name:
// Function Description:
// Parameter:
// Return Value:
int LA8297_Init(int nUnitId)
{
	int nRet = DI_CH_OK;
	HUINT8 ucRdData = 0;
	HUINT8 ucWrData = 0;

	nRet = ResetGpioLnb(nUnitId);
	if( nRet != DI_CH_OK )
	{
		CH_DI_Print(0, ( "Error : LA8297_Init nUnitId(%d) nRet(0x%x)\n", nUnitId, nRet));
	}

	ucWrData = s_aucCtrData[FIRST_A8297];

	if(nUnitId == MASTER_CHANNEL)
	{
		ucWrData |= (CTRL_ENB_M | CTRL_VSEL1_M | CTRL_VSEL0_M);
	}
	else
	{
		ucWrData |= (CTRL_ENB_S | CTRL_VSEL1_S | CTRL_VSEL0_S);
	}

	/* !!!Caution!!! LNB status must be cleared */
	nRet = ReadLnbReg(nUnitId, &ucRdData);
	if( nRet != DI_CH_OK )
	{
		CH_DI_Print(0, ( "Error : LA8297_Init nUnitId(%d) nRet(0x%x)\n", nUnitId, nRet));
	}

	nRet = WriteLnbReg(nUnitId, &ucWrData);
	if( nRet != DI_CH_OK )
	{
		CH_DI_Print(0, ( "Error : LA8297_Init nUnitId(%d) Error(0x%x)\n", nUnitId, nRet));
	}

	nRet = ReadLnbReg(nUnitId, &ucRdData);
	if( nRet != DI_CH_OK )
	{
		CH_DI_Print(0, ( "Error : LA8297_Init nUnitId(%d) nRet(0x%x)\n", nUnitId, nRet));
	}
	else
	{
		CH_DI_Print(3, ( "LA8297_Init (nUnitId(%d)) ucRdData = (0x%x)\n", nUnitId, ucRdData));
	}

	SetActiveStatus( nUnitId, CH_ACTION_ON );// TODO:

	return nRet;
}



int LA8297_SetOutput(int  nUnitId, HBOOL bEnable)
{
	unsigned char	 ucData = 0;
	int ErrCode = DI_CH_OK;

	ucData = s_aucCtrData[FIRST_A8297];

	if(nUnitId == MASTER_CHANNEL)
	{
		if(bEnable == TRUE)
		{
			CH_DI_Print(3, ( ">>>>>>>>>>>LA8297_EnableLnbOut() : Enable output nUnitId(%d) \n", nUnitId));
			ucData = ( ucData | CTRL_ENB_M );
		}
		else
		{
			CH_DI_Print(3, ( ">>>>>>>>>>>LA8297_EnableLnbOut() : Disable output nUnitId(%d) \n", nUnitId));
			ucData = ( ucData & (~CTRL_ENB_M) );
		}
	}
	else //slave channel
	{
		if(bEnable == TRUE)
		{
			CH_DI_Print(3, ( ">>>>>>>>>>>LA8297_EnableLnbOut() : Enable output nUnitId(%d) \n", nUnitId));
			ucData = ( ucData | CTRL_ENB_S );
		}
		else
		{
			CH_DI_Print(3, ( ">>>>>>>>>>>LA8297_EnableLnbOut() : Disable output nUnitId(%d) \n", nUnitId));
			ucData = ( ucData & (~CTRL_ENB_S) );
		}

	}

	ErrCode = WriteLnbReg(nUnitId, &ucData);
	if( ErrCode != DI_CH_OK )
	{
		CH_DI_Print(0, ( "Error : LA8297_EnableLnbOut nUnitId(%d) Error(0x%x)\n", nUnitId, ErrCode));
	}

	return ErrCode;
}



int LA8297_SetPolar(int  nUnitId, CH_SAT_Polarization_e etPolar, CH_SAT_LnbVoltage_e etVoltage)
{

	unsigned char	 ucData = 0;
	int ErrCode = DI_CH_OK;

	ucData = s_aucCtrData[FIRST_A8297];

	if(nUnitId == MASTER_CHANNEL)
	{
		ucData = ( ucData & (~0x07) );//init master voltage

		if( (etPolar == DI_CH_POLAR_HOR)||(etPolar == DI_CH_POLAR_LEFT) )
		{
			CH_DI_Print(3, ( ">>>>>>>>>>>LA8297_SetPolar(nUnitId(%d)) : DI_CH_POLAR_HOR \n", nUnitId));
			if( etVoltage == DI_CH_LNB_VOLT_STD )
			{
				ucData = (ucData | CTRL_VSEL2_M | CTRL_VSEL0_M );
			}
			else
			{
				ucData = (ucData | CTRL_VSEL2_M | CTRL_VSEL1_M );
			}
		}
		else
		{
			CH_DI_Print(3, ( ">>>>>>>>>>>LA8297_SetPolar(nUnitId(%d)) : DI_CH_POLAR_VER \n", nUnitId));
			if( etVoltage == DI_CH_LNB_VOLT_STD )
			{
				ucData = (ucData | CTRL_VSEL1_M | CTRL_VSEL0_M );
			}
			else
			{
				ucData = (ucData | CTRL_VSEL1_M | CTRL_VSEL0_M );
			}
		}
	}
	else //slave channel
	{
		ucData = ( ucData & (~0x70) );//init slave voltage

		if( (etPolar == DI_CH_POLAR_HOR)||(etPolar == DI_CH_POLAR_LEFT) )
		{
			CH_DI_Print(3, ( ">>>>>>>>>>>LA8297_SetPolar(nUnitId(%d)) : DI_CH_POLAR_HOR \n", nUnitId));
			if( etVoltage == DI_CH_LNB_VOLT_STD )
			{
				ucData = (ucData | CTRL_VSEL2_S | CTRL_VSEL0_S );
			}
			else
			{
				ucData = (ucData | CTRL_VSEL2_S | CTRL_VSEL1_S );
			}
		}
		else
		{
			CH_DI_Print(3, ( ">>>>>>>>>>>LA8297_SetPolar(nUnitId(%d)) : DI_CH_POLAR_VER \n", nUnitId));
			if( etVoltage == DI_CH_LNB_VOLT_STD )
			{
				ucData = (ucData | CTRL_VSEL1_S | CTRL_VSEL0_S );
			}
			else
			{
				ucData = (ucData | CTRL_VSEL1_S | CTRL_VSEL0_S );
			}
		}
	}

	ErrCode = WriteLnbReg(nUnitId, &ucData);
	if( ErrCode != DI_CH_OK )
	{
		CH_DI_Print(0, ( "Error : LA8297_SetPolar nUnitId(%d) Error(0x%x)\n", nUnitId, ErrCode));
	}
	else
	{
		CH_DI_Print(5, ( "LA8297_SetPolar nUnitId(%d) ucData(0x%x)\n", nUnitId, ucData));
	}

	return ErrCode;

}



int LA8297_CheckAntState(int nUnitId)
{
	int nRet = DI_CH_OK;
	HUINT8 ucRdStatus = 0;

	nRet = ReadLnbReg(nUnitId, &ucRdStatus);
	if( nRet != DI_CH_OK )
	{
		CH_DI_Print(0, ( "Error : LA8297_SetPolar nUnitId(%d) Error(0x%x)\n", nUnitId, nRet));
		nRet = DI_CH_ERR;
	}
	else
	{
		if(nUnitId == MASTER_CHANNEL)
		{
			if( ucRdStatus & CTRL_OCP_M )
			{
				nRet = DI_CH_ANTENNA_FAULT;
			}
			else
			{
				//OK
			}
		}
		else
		{
			if( ucRdStatus & CTRL_OCP_S )
			{
				nRet = DI_CH_ANTENNA_FAULT;
			}
			else
			{
				//OK
			}

		}

		CH_DI_Print(6, ( "LA8297_CheckAntState nUnitId(%d) ucRdStatus(0x%x)\n", nUnitId, ucRdStatus));

	}

	return nRet;
}



int LA8297_ReadStatus(
		int	nUnitId,
		HUINT8 *pucData
		)
{
	int nRet = DI_CH_OK;

	nRet = ReadLnbReg(nUnitId, pucData);
	if( nRet != DI_CH_OK )
	{
		CH_DI_Print(0, ( "Error : LA8297_SetPolar nUnitId(%d) Error(0x%x)\n", nUnitId, nRet));
	}
	else
	{
		CH_DI_Print(0, ( "LA8297_ReadStatus (nUnitId(%d)) *pucData = (0x%x)\n", nUnitId, *pucData));
	}
	return nRet;
}



/*********************** End of File ******************************/

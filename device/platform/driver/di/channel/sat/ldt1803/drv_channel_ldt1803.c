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
#define DT1803_I2C_ADDR0 (0x10>>1)
#define DT1803_I2C_ADDR1 (0x10>>1)
#define DT1803_CTRL_REG (0x0)
#define DT1803_STATUS_REG (0x0)
#define CONST_RW_LENGTH (1)

#if defined (CONFIG_PLATFORM_LA8297) && defined (CONFIG_PLATFORM_LDT1803)
#define NUM_OF_DT1803 	(2)
#else
#define NUM_OF_DT1803 	(1)
#endif
#define FIRST_DT1803		(0)

#define MASTER_CHANNEL	(0)

// M : master unit, S : slave unit
#define CTRL_VSEL0 	(0x01)
#define CTRL_VSEL1 	(0x02)
#define CTRL_VSEL2 	(0x04)
#define CTRL_ENB 		(0x08)
#define CTRL_OCP 		(0x04)
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
#if defined (CONFIG_PLATFORM_LA8297) && defined (CONFIG_PLATFORM_LDT1803)
#else
HUINT8 g_hwtest_lnb_off = 0;//It will bi deprecated...
#endif
// End global variable


/*******************************************************************/
/************************ static variables *************************/
/*******************************************************************/
// Start static variablee
static HUINT8 s_aucCtrData[NUM_OF_DT1803] = {0,};

static int LDT1803_WriteLnbReg(
		int	nUnitId,
		HUINT8 * pucWrData
		)
{

	int ErrCode = DI_CH_OK;
	HUINT32	_i2c_channel_;
	HUINT16	_i2c_addr_;

	
#if defined(CONFIG_I2C_MAP_FOR_2TUNER)
	if (nUnitId == 1)
	{
		_i2c_channel_ = I2C_CHANNEL_CHANNEL2;
		_i2c_addr_ = DT1803_I2C_ADDR1;
	}
	else 
#endif		
	{
		_i2c_channel_ = I2C_CHANNEL_CHANNEL;
		_i2c_addr_ = DT1803_I2C_ADDR0;
	}

	DRV_CH_LockI2c(nUnitId);
	ErrCode = DRV_I2c_WriteA8(_i2c_channel_,_i2c_addr_,DT1803_CTRL_REG, pucWrData,CONST_RW_LENGTH);
	if( ErrCode != DI_CH_OK )
	{
		CH_DI_Print(0, ( "[DT1803] LDT_1803_WriteLnbReg nUnitId(%d) Error(0x%x)\n", nUnitId, ErrCode));
		ErrCode = DI_CH_ERR;
	}
	else
	{
		s_aucCtrData[FIRST_DT1803] = *pucWrData;
		CH_DI_Print(7, ( "\t\t>>>>> LDT_1803_WriteLnbReg nUnitId(%d) s_aucCtrData[0](0x%x)\n", nUnitId, s_aucCtrData[FIRST_DT1803]));
	}
	DRV_CH_UnLockI2c(nUnitId);

	return ErrCode;
} /* end of LDT_1803_WriteLnbReg */


/********************************************************************************
 *	Function	: ReadLnbReg
 *	Description :
 *	Input		:
 *	Output		:
 *	Return		:
 ********************************************************************************/

static int LDT1803_ReadLnbReg(
		int nUnitId,
		HUINT8 * pucRdData
		)
{

	int ErrCode = DI_CH_OK;
	HUINT8 ucRdData = 0;
	HUINT32	_i2c_channel_;
	HUINT16	_i2c_addr_;

#if defined(CONFIG_I2C_MAP_FOR_2TUNER)
	if (nUnitId == 1)
	{
		_i2c_channel_ = I2C_CHANNEL_CHANNEL2;
		_i2c_addr_ = DT1803_I2C_ADDR1;
	}
	else 
#endif		
	{
		_i2c_channel_ = I2C_CHANNEL_CHANNEL;
		_i2c_addr_ = DT1803_I2C_ADDR0;
	}

	DRV_CH_LockI2c(nUnitId);
	ErrCode = DRV_I2c_WriteNoAddr(_i2c_channel_,_i2c_addr_, &ucRdData,CONST_RW_LENGTH);
	if ( ErrCode != DI_CH_OK )
	{
		CH_DI_Print(0, ( "[DT1803] LDT_1803_ReadLnbReg nUnitId(%d) Error(0x%x)\n", nUnitId, ErrCode));
		DRV_CH_UnLockI2c(nUnitId);
		return DI_CH_ERR;
	}

	ErrCode = DRV_I2c_ReadNoAddr(_i2c_channel_,_i2c_addr_, pucRdData,CONST_RW_LENGTH);
	if ( ErrCode != DI_CH_OK )
	{
		CH_DI_Print(0, ( "[DT1803] LDT_1803_ReadLnbReg nUnitId(%d) Error(0x%x)\n", nUnitId, ErrCode));
		ErrCode = DI_CH_ERR;
	}
	else
	{
		//CH_DI_Print(0, ( "ReadLnbReg (nUnitId(%d)) *pucRdDatas = (0x%x)\n", nUnitId, *pucRdData));
	}
	DRV_CH_UnLockI2c(nUnitId);


	return ErrCode;

}/* end of ReadLnbReg */


/****************** Function 시작점에 필히 기록 ********************/
/*******************************************************************/
/*********************** Function Description***********************/
/*******************************************************************/
// Function Name:
// Function Description:
// Parameter:
// Return Value:
int LDT1803_Init(int nUnitId)
{
	int nRet = DI_CH_OK;
	HUINT8 ucRdData = 0;
	HUINT8 ucWrData = 0;

	if( nRet != DI_CH_OK )
	{
		CH_DI_Print(0, ( "Error : LDT1803_Init nUnitId(%d) nRet(0x%x)\n", nUnitId, nRet));
	}

	ucWrData = s_aucCtrData[FIRST_DT1803];

//JAMES...150130...H/W channel team wants Vertical voltage to lower that before.
#if 1
	ucWrData |= (CTRL_ENB |  CTRL_VSEL0);
#else
	ucWrData |= (CTRL_ENB | CTRL_VSEL1 | CTRL_VSEL0);
#endif

	/* !!!Caution!!! LNB status must be cleared */
	nRet = LDT1803_ReadLnbReg(nUnitId, &ucRdData);
	if( nRet != DI_CH_OK )
	{
		CH_DI_Print(0, ( "Error : LDT1803_Init nUnitId(%d) nRet(0x%x)\n", nUnitId, nRet));
	}

	nRet = LDT1803_WriteLnbReg(nUnitId, &ucWrData);
	if( nRet != DI_CH_OK )
	{
		CH_DI_Print(0, ( "Error : LDT1803_Init nUnitId(%d) Error(0x%x)\n", nUnitId, nRet));
	}

	nRet = LDT1803_ReadLnbReg(nUnitId, &ucRdData);
	if( nRet != DI_CH_OK )
	{
		CH_DI_Print(0, ( "Error : LDT1803_Init nUnitId(%d) nRet(0x%x)\n", nUnitId, nRet));
	}
	else
	{
		CH_DI_Print(3, ( "LDT1803_Init (nUnitId(%d)) ucRdData = (0x%x)\n", nUnitId, ucRdData));
	}

	SetActiveStatus( nUnitId, CH_ACTION_ON );// TODO:

	return nRet;
}



int LDT1803_SetOutput(int  nUnitId, HBOOL bEnable)
{
	unsigned char	 ucData = 0;
	int ErrCode = DI_CH_OK;

	ucData = s_aucCtrData[FIRST_DT1803];

	if(bEnable == TRUE)
	{
		CH_DI_Print(3, ( ">>>>>>>>>>>LDT1803_EnableLnbOut() : Enable output nUnitId(%d) \n", nUnitId));
		ucData = ( ucData | CTRL_ENB );
	}
	else
	{
		CH_DI_Print(3, ( ">>>>>>>>>>>LDT1803_EnableLnbOut() : Disable output nUnitId(%d) \n", nUnitId));
		ucData = ( ucData & (~CTRL_ENB) );
	}
	

	ErrCode = LDT1803_WriteLnbReg(nUnitId, &ucData);
	if( ErrCode != DI_CH_OK )
	{
		CH_DI_Print(0, ( "Error : LDT1803_EnableLnbOut nUnitId(%d) Error(0x%x)\n", nUnitId, ErrCode));
	}

	return ErrCode;
}



int LDT1803_SetPolar(int  nUnitId, CH_SAT_Polarization_e etPolar, CH_SAT_LnbVoltage_e etVoltage)
{

	unsigned char	 ucData = 0;
	int ErrCode = DI_CH_OK;

	ucData = s_aucCtrData[FIRST_DT1803];

	ucData = ( ucData & (~0x07) );//init master voltage

	if( (etPolar == DI_CH_POLAR_HOR)||(etPolar == DI_CH_POLAR_LEFT) )
	{
		CH_DI_Print(3, ( ">>>>>>>>>>>LDT1803_SetPolar(nUnitId(%d)) : DI_CH_POLAR_HOR \n", nUnitId));
		if( etVoltage == DI_CH_LNB_VOLT_STD )
		{
#if defined(CONFIG_SUPPORT_SURGE_PROTECTION)
			CH_DI_Print(3, ( "LDT1803_SetPolar(nUnitId(%d)): [SUPPORT_SURGE_PROTECTION] \n", nUnitId));
			ucData = (ucData | CTRL_VSEL2 | CTRL_VSEL0 );
#else
			ucData = (ucData | CTRL_VSEL2 );
#endif
		}
		else
		{
			ucData = (ucData | CTRL_VSEL2 | CTRL_VSEL0 );
		}
	}
	else
	{
		CH_DI_Print(3, ( ">>>>>>>>>>>LDT1803_SetPolar(nUnitId(%d)) : DI_CH_POLAR_VER \n", nUnitId));
		if( etVoltage == DI_CH_LNB_VOLT_STD )
		{
#if defined(CONFIG_SUPPORT_SURGE_PROTECTION)
			CH_DI_Print(3, ( "LDT1803_SetPolar(nUnitId(%d)): [SUPPORT_SURGE_PROTECTION] \n", nUnitId));
			ucData = (ucData | CTRL_VSEL1 );
#else
			ucData = (ucData | CTRL_VSEL0 );
#endif
		}
		else
		{
			ucData = (ucData | CTRL_VSEL1 );
		}
	}

	ErrCode = LDT1803_WriteLnbReg(nUnitId, &ucData);
	if( ErrCode != DI_CH_OK )
	{
		CH_DI_Print(0, ( "Error : LDT1803_SetPolar nUnitId(%d) Error(0x%x)\n", nUnitId, ErrCode));
	}
	else
	{
		CH_DI_Print(5, ( "LDT1803_SetPolar nUnitId(%d) ucData(0x%x)\n", nUnitId, ucData));
	}

	return ErrCode;

}



int LDT1803_CheckAntState(int nUnitId)
{
	int nRet = DI_CH_OK;
	HUINT8 ucRdStatus = 0;

	nRet = LDT1803_ReadLnbReg(nUnitId, &ucRdStatus);
	if( nRet != DI_CH_OK )
	{
		CH_DI_Print(0, ( "Error : LDT1803_CheckAntState nUnitId(%d) Error(0x%x)\n", nUnitId, nRet));
		nRet = DI_CH_ERR;
	}
	else
	{
		if( ucRdStatus & CTRL_OCP )
		{
			nRet = DI_CH_ANTENNA_FAULT;
		}

		CH_DI_Print(6, ( "LDT1803_CheckAntState nUnitId(%d) ucRdStatus(0x%x)\n", nUnitId, ucRdStatus));

	}

	return nRet;
}



int LDT1803_ReadStatus(
		int	nUnitId,
		HUINT8 *pucData
		)
{
	int nRet = DI_CH_OK;

	nRet = LDT1803_ReadLnbReg(nUnitId, pucData);
	if( nRet != DI_CH_OK )
	{
		CH_DI_Print(0, ( "Error : LDT1803_SetPolar nUnitId(%d) Error(0x%x)\n", nUnitId, nRet));
	}
	else
	{
		CH_DI_Print(0, ( "LDT1803_ReadStatus (nUnitId(%d)) *pucData = (0x%x)\n", nUnitId, *pucData));
	}
	return nRet;
}



/*********************** End of File ******************************/

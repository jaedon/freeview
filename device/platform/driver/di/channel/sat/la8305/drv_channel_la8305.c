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

#include "drv_channel_lnb.h"
#include "drv_i2c.h"
// End Including Headers


/*******************************************************************/
/****************************** define *****************************/
/*******************************************************************/
// Start #define
#define A8305_I2C_ADDR (0x10>>1)
#define A8305_CTRL_REG (0x0)
#define A8305_STATUS_REG (0x0)
#define CONST_RW_LENGTH (1)

#define NUM_OF_A8305 (1)

#define CTRL_VSEL0 (0x1)
#define CTRL_VSEL1 (0x2)
#define CTRL_VSEL2 (0x4)
#define CTRL_ENB (0x8)

#define CTRL_OCP (0x4)
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
static HUINT8 s_aucCtrData[NUM_OF_A8305] = {0,};

static int WriteLnbReg(
		int	nUnitId,
		HUINT8 * pucWrData
		)
{

	int ErrCode = DI_CH_OK;

	ErrCode = DRV_I2c_WriteA8(I2C_CHANNEL_CHANNEL,A8305_I2C_ADDR,A8305_CTRL_REG, pucWrData,CONST_RW_LENGTH);
	if( ErrCode != DI_CH_OK )
	{
		CH_DI_Print(0, ( "[A8305] WriteLnbReg nUnitId(%d) Error(0x%x)\n", nUnitId, ErrCode));
		ErrCode = DI_CH_ERR;
	}
	else
	{
		s_aucCtrData[nUnitId] = *pucWrData;
	}

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
	HAPPY(nUnitId);
#if 0
	//ErrCode = DRV_I2c_WriteA8(I2C_CHANNEL_CHANNEL,A8305_I2C_ADDR,A8305_CTRL_REG, pucWrData,CONST_RW_LENGTH);
	//ErrCode = DRV_I2c_ReadA8(I2C_CHANNEL_CHANNEL,A8305_I2C_ADDR,A8305_STATUS_REG, pucRdData,CONST_RW_LENGTH);
	ErrCode = DRV_I2c_ReadA16(I2C_CHANNEL_CHANNEL,A8305_I2C_ADDR,A8305_STATUS_REG, pucRdData,CONST_RW_LENGTH);
	if ( ErrCode != DI_CH_OK )
	{
		CH_DI_Print(0, ( "[A8305] ReadLnbReg nUnitId(%d) Error(0x%x)\n", nUnitId, ErrCode));
		ErrCode = DI_CH_ERR;
	}
#else
	HUINT8 ucRdData = 0;

	ErrCode = DRV_I2c_WriteNoAddr(I2C_CHANNEL_CHANNEL,A8305_I2C_ADDR, &ucRdData,CONST_RW_LENGTH);
	if ( ErrCode != DI_CH_OK )
	{
		CH_DI_Print(0, ( "[A8305] ReadLnbReg nUnitId(%d) Error(0x%x)\n", nUnitId, ErrCode));
		ErrCode = DI_CH_ERR;
	}

	ErrCode = DRV_I2c_ReadNoAddr(I2C_CHANNEL_CHANNEL,A8305_I2C_ADDR, pucRdData,CONST_RW_LENGTH);
	if ( ErrCode != DI_CH_OK )
	{
		CH_DI_Print(0, ( "[A8305] ReadLnbReg nUnitId(%d) Error(0x%x)\n", nUnitId, ErrCode));
		ErrCode = DI_CH_ERR;
	}
	else
	{
		//CH_DI_Print(0, ( "ReadLnbReg (nUnitId(%d)) *pucRdDatas = (0x%x)\n", nUnitId, *pucRdData));
	}
#endif
	return ErrCode;

}/* end of ReadLnbReg */
// End static variable


/****************** Function �������� ���� ��� ********************/
/*******************************************************************/
/*********************** Function Description***********************/
/*******************************************************************/
// Function Name:
// Function Description:
// Parameter:
// Return Value:
int LA8305_Init(int nUnitId)
{
	int nRet = DI_CH_OK;
	HUINT8 ucRdData = 0;
	HUINT8 ucWrData = 0;

	ucWrData = CTRL_ENB | CTRL_VSEL1 | CTRL_VSEL0;

	/* !!!Caution!!! LNB status must be cleared */
	nRet = ReadLnbReg(nUnitId, &ucRdData);
	if( nRet != DI_CH_OK )
	{
		CH_DI_Print(0, ( "Error : LA8305_Init nUnitId(%d) nRet(0x%x)\n", nUnitId, nRet));
	}

	nRet = WriteLnbReg(nUnitId, &ucWrData);
	if( nRet != DI_CH_OK )
	{
		CH_DI_Print(0, ( "Error : LA8305_SetPolar nUnitId(%d) Error(0x%x)\n", nUnitId, nRet));
	}

	nRet = ReadLnbReg(nUnitId, &ucRdData);
	if( nRet != DI_CH_OK )
	{
		CH_DI_Print(0, ( "Error : LA8305_Init nUnitId(%d) nRet(0x%x)\n", nUnitId, nRet));
	}
	else
	{
		CH_DI_Print(3, ( "LA8305_Init (nUnitId(%d)) ucRdData = (0x%x)\n", nUnitId, ucRdData));
	}

	SetActiveStatus( nUnitId, CH_ACTION_ON );// TODO:

	return nRet;
}


int LA8305_SetOutput(int  nUnitId, HBOOL bEnable)
{
	unsigned char	 ucData = 0;
	int ErrCode = DI_CH_OK;

	ucData = s_aucCtrData[nUnitId];

	if(bEnable == TRUE)
	{
		CH_DI_Print(3, ( ">>>>>>>>>>>LA8305_EnableLnbOut() : Enable output nUnitId(%d) \n", nUnitId));
		ucData = ( ucData | CTRL_ENB );
	}
	else
	{
		CH_DI_Print(3, ( ">>>>>>>>>>>LA8305_EnableLnbOut() : Disable output nUnitId(%d) \n", nUnitId));
		ucData = ( ucData & (~CTRL_ENB) );
	}

	ErrCode = WriteLnbReg(nUnitId, &ucData);
	if( ErrCode != DI_CH_OK )
	{
		CH_DI_Print(0, ( "Error : LA8305_EnableLnbOut nUnitId(%d) Error(0x%x)\n", nUnitId, ErrCode));
	}

	return ErrCode;
}



int LA8305_SetPolar(int  nUnitId, CH_SAT_Polarization_e etPolar, CH_SAT_LnbVoltage_e etVoltage)
{

	unsigned char	 ucData = 0;
	int ErrCode = DI_CH_OK;

	ucData = s_aucCtrData[nUnitId];
	ucData = ( ucData & (~0x07) );

	if( (etPolar == DI_CH_POLAR_HOR)||(etPolar == DI_CH_POLAR_LEFT) )
	{
		CH_DI_Print(3, ( ">>>>>>>>>>>LA8305_SetPolar(nUnitId(%d)) : DI_CH_POLAR_HOR \n", nUnitId));
		if( etVoltage == DI_CH_LNB_VOLT_STD )
		{
			ucData = (ucData | CTRL_VSEL2 | CTRL_VSEL0 );
#if defined(CONFIG_PLATFORM_MXL683)
			/* workaround for A8304 for HTB-100S */
			ucData = 0x0b; /* 18.667 Volt */
#endif
		}
		else
		{
			ucData = (ucData | CTRL_VSEL2 | CTRL_VSEL1 );
		}
	}
	else
	{
		CH_DI_Print(3, ( ">>>>>>>>>>>LA8305_SetPolar(nUnitId(%d)) : DI_CH_POLAR_VER \n", nUnitId));
		if( etVoltage == DI_CH_LNB_VOLT_STD )
		{
			ucData = (ucData  | CTRL_VSEL0 );
#if defined(CONFIG_PLATFORM_MXL683)
			/* workaround for A8304 for HTB-100S */
			ucData = 0x03; /* 13.667 Volt */
#endif
		}
		else
		{
			ucData = (ucData | CTRL_VSEL1 | CTRL_VSEL0 );
		}
	}
#if defined(CONFIG_PLATFORM_MXL683)
	/* workaround for A8304 for HTB-100S */
	ucData |= 0x10; /* LNB enable bit of A8304 is different from A8305.  TODO_ILEE */
	CH_DI_Print(3, ( "########## LA8304_SetPolar nUnitId(%d) ucData(0x%x)\n", nUnitId, ucData));
#endif

	ErrCode = WriteLnbReg(nUnitId, &ucData);
	if( ErrCode != DI_CH_OK )
	{
		CH_DI_Print(0, ( "Error : LA8305_SetPolar nUnitId(%d) Error(0x%x)\n", nUnitId, ErrCode));
	}

	return ErrCode;

}



int LA8305_CheckAntState(int nUnitId)
{
	int nRet = DI_CH_OK;
	HUINT8 ucRdStatus = 0;

	nRet = ReadLnbReg(nUnitId, &ucRdStatus);
	if( nRet != DI_CH_OK )
	{
		CH_DI_Print(0, ( "Error : LA8305_SetPolar nUnitId(%d) Error(0x%x)\n", nUnitId, nRet));
		nRet = DI_CH_ERR;
	}
	else
	{
		if( ucRdStatus & CTRL_OCP )
		{
			nRet = DI_CH_ANTENNA_FAULT;
		}
		else
		{
			//OK
		}
	}

	return nRet;
}



int LA8305_ReadStatus(
		int	nUnitId,
		HUINT8 *pucData
		)
{
	int nRet = DI_CH_OK;

	nRet = ReadLnbReg(nUnitId, pucData);
	if( nRet != DI_CH_OK )
	{
		CH_DI_Print(0, ( "Error : LA8305_SetPolar nUnitId(%d) Error(0x%x)\n", nUnitId, nRet));
	}
	else
	{
		CH_DI_Print(0, ( "LA8305_ReadStatus (nUnitId(%d)) *pucData = (0x%x)\n", nUnitId, *pucData));
	}
	return nRet;
}



/*********************** End of File ******************************/
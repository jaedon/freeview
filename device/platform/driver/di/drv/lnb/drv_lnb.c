/* $Header: */
/*******************************************************************/
/************************* File Description ************************/
/*******************************************************************/
// File Name:			dev_lnbp.c
// Original Author: 	Kim Sung Yong
// File Description:
// Module:				DDI
// Remarks:				Jinhong Kim, modified

/*******************************************************************/
/* Copyright (c) 2007 HUMAX Co., Ltd. 							   */
/* All rights reserved.											   */
/*******************************************************************/


/*******************************************************************/
/**************************** Header Files *************************/
/*******************************************************************/
/* include  common utils */
#include "linden_trace.h"
#include "htype.h"
#include "di_err.h"

/* include  drv headers */
#include "drv_lnb.h"
#include "drv_i2c.h"

/* include  di headers */
#if defined(CONFIG_VK_STDLIB)
#else
#include 	<stdio.h>
#endif

#include "vkernel.h"
#include "taskdef.h"

/* Humax Native */


/*******************************************************************/
/****************************** define *****************************/
/*******************************************************************/
#define A8290_CTRL_VSEL0  		0x01
#define A8290_CTRL_VSEL1  		0x02
#define A8290_CTRL_VSEL2  		0x04
#define A8290_CTRL_VSEL3  		0x08
#define A8290_CTRL_ODT  		0x10
#define A8290_CTRL_ENB  		0x20
#define A8290_CTRL_TMODE  		0x01
#define A8290_CTRL_TGATE  		0x02
#define A8290_CTRL_CADT  		0x04
#define A8290_CTRL_BFC2			0x20
#define A8290_CTRL_ADDR0  		0x00
#define A8290_CTRL_ADDR1  		0x80

#define A8290_STATUS_OCP  		0x04
//#define CHANNEL_LNBP_ADDRESS	0x08
#define CONST_DRV_TN_A8290_ADDRESS		0x08



#define DRV_TUNER_LOCK()				(VK_SEM_Get(s_ulLNBSemID))
#define DRV_TUNER_UNLOCK()				(VK_SEM_Release(s_ulLNBSemID))


/*******************************************************************/
/****************************** typedef ****************************/
/*******************************************************************/




/*******************************************************************/
/************************ global variables *************************/
/*******************************************************************/



/*******************************************************************/
/************************ static variables *************************/
/*******************************************************************/
static HUINT8 lnb_pol=0;
static DRV_CH_LNB_POWER g_stPower;

HUINT32 s_ulLNBSemID = 0;

/*******************************************************************/
/************************ Function Prototypes *************************/
/*******************************************************************/

DRV_Error DRV_LNB_I2cWrite( HUINT8 ucData );
DRV_Error DRV_LNB_I2cRead( HUINT8 *buf );




/*******************************************************************/
/*********************** Function Description***********************/
/*******************************************************************/

/*******************************************************************/
// Function Name: DRV_LNB_Init
// Function Description:
// Parameter:
// Return Value:
/*******************************************************************/

DRV_Error DRV_LNB_Init(void)
{
	HUINT16 ErrCode = DI_ERR_OK;
	VK_ERR_CODE etVErrorCode = VK_OK;
	unsigned char lbnp_status[2];

	if(etVErrorCode != VK_OK)
	{
		TraceError(TRACE_MODULE_DRV_LNB, "%s->VKI_SEM_Create(): Error Code(%d), Line(%d), %s\n",
			__FUNCTION__, etVErrorCode, __LINE__, __FILE__);
		return DI_ERR_ERROR;
	}

	etVErrorCode = VK_SEM_Create((unsigned long *)&s_ulLNBSemID, "DRV_LNB", DEFAULT_SUSPEND_TYPE);
	if(etVErrorCode != VK_OK)
	{
		TraceError(TRACE_MODULE_DRV_LNB, "%s->VKI_SEM_Create(): Error Code(%d), Line(%d), %s\n",
			__FUNCTION__, etVErrorCode, __LINE__, __FILE__);
		return DI_ERR_ERROR;
	}

	lnb_pol = 0;
	ErrCode = DRV_LNB_I2cRead(lbnp_status);
	//LNB_POWER_ON
	ErrCode |= DRV_LNB_I2cWrite( 0x70/*A8290_CTRL_ADDR1|A8290_CTRL_TMODE| A8290_CTRL_TGATE */);
	VK_TASK_Sleep(100);

	ErrCode |= DRV_LNB_I2cWrite( 0x30/*(A8290_CTRL_ENB|A8290_CTRL_ODT|lnb_pol)*/);
	VK_TASK_Sleep(100);

	return ErrCode;


}/*end of Drv_Tuner_Lnbp_Init */


/*******************************************************************/
// Function Name:		DRV_LNB_I2cWrite
// Function Description:
// Parameter:
// Return Value:
/*******************************************************************/
DRV_Error DRV_LNB_I2cWrite( HUINT8 ucData )
{
	DRV_Error etDrvErr = DRV_OK;

	DRV_TUNER_LOCK();

	etDrvErr = DRV_I2c_WriteA8(I2C_CHANNEL_CHANNEL, CONST_DRV_TN_A8290_ADDRESS, 0, &ucData, 1);
	if (DRV_OK != etDrvErr)
	{
		TraceError(TRACE_MODULE_DRV_LNB, "%s->DRV_LNB_I2cWrite(): Error Code(0x%x), Line(%d), %s\n",
			__FUNCTION__, etDrvErr, __LINE__, __FILE__);
		DRV_TUNER_UNLOCK();
		return DI_ERR_ERROR;
	}

	TraceInfo(TRACE_MODULE_DRV_LNB, "%s->DRV_LNB_I2cWrite(): Done (%d), Line(%d), %s\n",
			__FUNCTION__, etDrvErr, __LINE__, __FILE__);
	DRV_TUNER_UNLOCK();
	return DI_ERR_OK;
} /* end of DRV_LNB_I2cWrite */


/*******************************************************************/
// Function Name: drv_ch_lnbhRead
// Function Description:
// Parameter:
// Return Value:
/*******************************************************************/
DRV_Error DRV_LNB_I2cRead( HUINT8 *buf )
{
	DRV_Error etDrvErr = DRV_OK;

	DRV_TUNER_LOCK();
	etDrvErr = DRV_I2c_ReadNoAddr(I2C_CHANNEL_CHANNEL, CONST_DRV_TN_A8290_ADDRESS, buf, 1);

	if (DRV_OK != etDrvErr)
	{
		TraceError(TRACE_MODULE_DRV_LNB, "%s->DRV_LNB_I2cRead() Error Code(0x%x), Line(%d), %s\n",
			__FUNCTION__, etDrvErr, __LINE__, __FILE__);
		DRV_TUNER_UNLOCK();
		return DI_ERR_ERROR;
	}

	TraceInfo(TRACE_MODULE_DRV_LNB, "%s->DRV_LNB_I2cRead() Done(%d), Line(%d), %s\n",
			__FUNCTION__, etDrvErr, __LINE__, __FILE__);
	DRV_TUNER_UNLOCK();
	return DI_ERR_OK;
} /* end of DRV_LNB_I2cRead */

/*******************************************************************/
/****************************** define *****************************/
/*******************************************************************/

 /*******************************************************************/
 // Function Name: LNBP_SetPolarization
 // Function Description:
 // Parameter:
 // Return Value:	NONE
 /*******************************************************************/

 DRV_Error DRV_LNB_SetPolarization(DRV_CH_LNB_POL ucPolar, DRV_CH_LNB_VOLTAGE etVoltage)
{
	 HUINT16 RetCode = DI_ERR_OK;
	 unsigned char ucData = 0;

	if(ucPolar == LNB_POL_LINEAR_HOR) // 18V
	{
		if( etVoltage == LNB_VOLTAGE_HIGH) // 19V
			lnb_pol = A8290_CTRL_VSEL3 | 0x04;
		else
			lnb_pol = A8290_CTRL_VSEL3 | 0x01;
	}
	else if(ucPolar == LNB_POL_LINEAR_VER) // 13V
	{
		if( etVoltage == LNB_VOLTAGE_HIGH) // 14V
			lnb_pol = A8290_CTRL_VSEL3 | 0x04;
		else
			lnb_pol = 0x05;
	}

	RetCode = DRV_LNB_I2cWrite( A8290_CTRL_ENB|A8290_CTRL_ODT|lnb_pol );
	VK_TASK_Sleep(40);

	return RetCode;
}


/*******************************************************************/
// Function Name: LNBP_SetLNBPower
// Function Description:
// Parameter:
// Return Value:   NONE
/*******************************************************************/
DRV_Error DRV_LNB_SetLNBPower(DRV_CH_LNB_POWER etPower)
{
	HUINT8 ucData;
	HUINT16 RetCode = DI_ERR_OK;

	g_stPower = etPower;

	TraceInfo(TRACE_MODULE_DRV_LNB, "Drv_Tuner_SetLNBPower (%d) \n",etPower);

	if (etPower == LNB_POWER_ON)
	{
		RetCode |= DRV_LNB_I2cWrite( A8290_CTRL_ENB|A8290_CTRL_ODT|lnb_pol);
		RetCode |= DRV_LNB_I2cWrite( A8290_CTRL_ADDR1| A8290_CTRL_TMODE | A8290_CTRL_TGATE ); //external tone, tone gated on
		VK_TASK_Sleep(100);
	}
	else if ((etPower == LNB_POWER_OFF)
			|| (etPower == LNB_POWER_TEST))
	{
		RetCode |= DRV_LNB_I2cWrite( A8290_CTRL_ODT|lnb_pol);
		RetCode |= DRV_LNB_I2cWrite( A8290_CTRL_ADDR1|A8290_CTRL_TMODE|A8290_CTRL_TGATE );
	}
	else if(etPower == LNB_POWER_SHORT)
	{
		// LNB_POWER_OFF //
		RetCode |= DRV_LNB_I2cWrite( A8290_CTRL_ODT|lnb_pol);
		RetCode |= DRV_LNB_I2cWrite( A8290_CTRL_ADDR1|A8290_CTRL_TMODE|A8290_CTRL_TGATE );
		VK_TASK_Sleep(50);

		// LNB_POWER_ON //
		RetCode |= DRV_LNB_I2cWrite( A8290_CTRL_ENB|A8290_CTRL_ODT|lnb_pol);
		RetCode |= DRV_LNB_I2cWrite( A8290_CTRL_ADDR1| A8290_CTRL_TMODE | A8290_CTRL_TGATE ); //external tone, tone gated on
		VK_TASK_Sleep(100);

	}
	return RetCode;
}

/*******************************************************************/
// Function Name: LNBP_CheckOverload
// Function Description:
// Parameter:
// Return Value:   LNB_OVERLOADED or LNB_NOT_OVERLOADED
/*******************************************************************/

DRV_Error DRV_LNB_CheckOverload(HUINT8 *usReadReg)
{
	HUINT8  lnbp_status[2];
	HUINT16 RetCode = DI_ERR_OK;


	if( g_stPower == LNB_POWER_ON )
	{
		RetCode = DRV_LNB_I2cRead( lnbp_status );

		TraceInfo(TRACE_MODULE_DRV_LNB, "%s->LNBH_CheckOverload]  lnbp_status(0x%x), RetCode(%d), Line(%d), %s\n",
					__FUNCTION__, lnbp_status[0], RetCode, __LINE__, __FILE__);

		if((lnbp_status[0] &= A8290_STATUS_OCP) != 0)
			*usReadReg = LNB_OVERLOADED;
		else
			*usReadReg = LNB_NOT_OVERLOADED;
	}
	return RetCode;
}


/* EOF */



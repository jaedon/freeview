// File Name	     :		drv_channel_lnb.c
// Original Author: 	Kim Sung Yong
// File Description:
// Module:				DDI
// Remarks:				Jinhong Kim, modified

/*******************************************************************/
/* Copyright (c) 2007 HUMAX Co., Ltd. 							   */
/* All rights reserved.											   */
/*******************************************************************/


/*******************************************************************/
/**************************** Header Files ****************************/
/*******************************************************************/
/* include  common utils */
#include "vkernel.h"

/* include  drv headers */
#include "drv_i2c.h"
#include "drv_channel_lnb.h"
#include "drv_channel_s.h"

/* include  di headers */
#include "di_channel_priv.h"
#include "di_channel_attr.h"

/*******************************************************************/
/****************************** define *******************************/
/*******************************************************************/
#define	LNB_REG_NUM 	2

// Over Current Mask
#define A8286_STATUS_OCP1  		0x04
#define A8286_STATUS_OCP2  		0x08

// Control register 0
#define A8286_CTRL_ADDR_CH1  		0x00
#define A8286_CTRL_ADDR_CH2  		0x40

#define A8286_CTRL_VSEL0  			0x01
#define A8286_CTRL_VSEL1  			0x02
#define A8286_CTRL_VSEL2  			0x04
#define A8286_CTRL_VSEL3  			0x08
#define A8286_CTRL_ODT  			0x10
#define A8286_CTRL_ENB  			0x20

// Control register 1
#define A8286_CTRL_ADDR1_CH1  		0x80
#define A8286_CTRL_ADDR1_CH2  		0xC0

#define A8286_CTRL_TMODE  			0x01
#define A8286_CTRL_TGATE  			0x02
#define A8286_CTRL_CADT  			0x04
#define A8286_CTRL_BFC2			0x2

/* control0/control1 registers */
#define	LNB_REG_0		0
#define 	LNB_REG_1		1

#define DRV_TUNER_LOCK()				(VK_SEM_Get(s_ulLNBSemID))
#define DRV_TUNER_UNLOCK()				(VK_SEM_Release(s_ulLNBSemID))

#define DRV_CHECKOVERLOAD_LOCK()		(VK_SEM_Get(s_ulCheckOverloadSemID))
#define DRV_CHECKOVERLOAD_UNLOCK()	(VK_SEM_Release(s_ulCheckOverloadSemID))

#define LNB_NOT_OVERLOADED	0
#define LNB_OVERLOADED			1

extern HINT32 show_di_ch;

HUINT8 g_hwtest_lnb_off = 0;//It will bi deprecated...

static HUINT32 s_ulLNBSemID = 0;
static HUINT32 s_ulCheckOverloadSemID = 0;

static HUINT8 CHANNEL_LNBP_ADDRESS1 = 0x08;
static HUINT8 CHANNEL_LNBP_ADDRESS2 = 0x0B;
static HUINT8 s_ucLNBData[CONFIG_NUM_OF_SAT_CHANNEL][LNB_REG_NUM];
static HUINT8 s_ucLnbShortFlag[CONFIG_NUM_OF_SAT_CHANNEL];

typedef enum
{
	LNB_VOL_14_375,
	LNB_VOL_15_042,
	LNB_VOL_19_375,
	LNB_VOL_20_375,
	LNB_VOL_NONE
} LA8286_OUTPUT_VOLTAGEPOL;

typedef enum {
	LNB_POWER_ON,
	LNB_POWER_OFF,
	LNB_POWER_TEST,
	LNB_POWER_SHORT
} DRV_CH_LNB_POWER;

typedef enum {
	LNB_VOLTAGE_STD,
	LNB_VOLTAGE_HIGH,
	LNB_VOLTAGE_NONE
}DRV_CH_LNB_VOLTAGE;

typedef enum
{
	LNB_POL_NOT_DEFINED = 0,
	LNB_POL_LINEAR_HOR,
	LNB_POL_LINEAR_VER,
	LNB_POL_CIRCUL_LEFT,
	LNB_POL_CIRCUL_RIGHT
} DRV_CH_LNB_POL;
/********************************************************************************
 *							Static variables									*
 ********************************************************************************/



/********************************************************************************
 *							Static functions 									*
 ********************************************************************************/
/********************************************************************************
 *	Function	: WriteLnbReg
 *	Description :
 *	Input		:
 *	Output		:
 *	Return		:
 ********************************************************************************/

static int WriteLnbReg(
		int	nUnitId,
		HUINT8 ucData
		)
{
	int ErrCode = DRV_OK;

	HUINT16	usChipAddr = 0;

#if !defined(__LNB_DEBUG__) || !defined(__CH_RELEASE_MODE_DEBUG__)
	CH_UNUSED(nUnitId);
#endif

	CH_DI_Print(6,("[WriteLnbReg] Tuner Id : %d, ucData : 0x%x \n", nUnitId, ucData));

	DRV_TUNER_LOCK();

	if((nUnitId == 0) || (nUnitId == 1))
	{
		usChipAddr = CHANNEL_LNBP_ADDRESS1;
	}
	else
	{
		usChipAddr = CHANNEL_LNBP_ADDRESS2;
	}

	ErrCode = DRV_I2c_WriteNoAddr(I2C_CHANNEL_CHANNEL, usChipAddr, &ucData, 1);
	if (ErrCode != DRV_OK)
	{
		CH_DI_Print(0,("[WriteLnbReg] Tuner Id : %d, Error from BREG_I2C_WriteNoAddr (0x%x)\n", nUnitId, usChipAddr));
		ErrCode = DRV_ERR;
	}

	 DRV_TUNER_UNLOCK();

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
		int  nUnitId,
		HUINT8 *ucData
		)
{
	int  ErrCode = DRV_OK;

	HUINT16	usChipAddr = 0;

#if !defined(__LNB_DEBUG__) || !defined(__CH_RELEASE_MODE_DEBUG__)
	CH_UNUSED(nUnitId);
#endif

	DRV_TUNER_LOCK();

	if((nUnitId == 0) || (nUnitId == 1))
	{
		usChipAddr = CHANNEL_LNBP_ADDRESS1;
	}
	else
	{
		usChipAddr = CHANNEL_LNBP_ADDRESS2;
	}

	ErrCode = DRV_I2c_ReadNoAddr(I2C_CHANNEL_CHANNEL, usChipAddr, ucData, 2);
	if (ErrCode != DRV_OK)
	{
		CH_DI_Print(0,("[ReadLnbReg] Tuner Id : %d, Error [0x%x] \n\r", nUnitId, ErrCode));
		ErrCode = DRV_ERR;
	}

	 DRV_TUNER_UNLOCK();

	return ErrCode;
}/* end of ReadLnbReg */



/********************************************************************************
 *	Function	: Drv_Ch_LnbRegRead
 *	Description :
 *	Input		:
 *	Output		:
 *	Return		:
 ********************************************************************************/

static DRV_Error GetControlData(
		HUINT32 UnitId,
		HUINT8 *buf
		)
{
	DRV_Error ErrCode = DRV_OK;

	HUINT8 ucData[LNB_REG_NUM] = {0xff, 0xff};

#if !defined(__LNB_DEBUG__) || !defined(__CH_RELEASE_MODE_DEBUG__)
		CH_UNUSED(UnitId);
#endif

	if( (UnitId%2) == 0 )
	{
		ucData[0] = (A8286_CTRL_ADDR_CH1|A8286_CTRL_ENB|A8286_CTRL_ODT);
		ucData[1] = (A8286_CTRL_ADDR1_CH1 |A8286_CTRL_TMODE|A8286_CTRL_TGATE );
	}
	else
	{
		ucData[0]  = (A8286_CTRL_ADDR_CH2|A8286_CTRL_ENB|A8286_CTRL_ODT);
		ucData[1] = (A8286_CTRL_ADDR1_CH2 |A8286_CTRL_TMODE|A8286_CTRL_TGATE );
	}

	*buf = ucData[0];

	*(buf +1) = ucData[1];

	CH_DI_Print(3,("[GetControlData] Tuner Id : %d, Data_0: 0x%x, Data_1: 0x%x \n", UnitId, *buf, *(buf +1)));

	return ErrCode;
}/* GetControlData */



/********************************************************************************
 *	Function	: Drv_Ch_LnbSetPolar
 *	Description :
 *	Input		:
 *	Output		:
 *	Return		:
 ********************************************************************************/

static DRV_Error GetPolaData(
		HUINT32 UnitId,
		DRV_CH_LNB_POL etPolar,
		DRV_CH_LNB_VOLTAGE etVoltage,
		HUINT8  ucData,
		HUINT8 *buf
		)

{
	DRV_Error ErrCode = DRV_OK;

	LA8286_OUTPUT_VOLTAGEPOL	  output_voltage = LNB_VOL_NONE;

#if !defined(__LNB_DEBUG__) || !defined(__CH_RELEASE_MODE_DEBUG__)
			CH_UNUSED(UnitId);
#endif

	/* HIGH VOL */
	if(etVoltage == LNB_VOLTAGE_HIGH)
	{
		if(etPolar == LNB_POL_LINEAR_HOR)
			output_voltage = LNB_VOL_20_375;
		else
			output_voltage = LNB_VOL_15_042;
	}
	/* STANDARD VOL */
	else
	{
		if(etPolar == LNB_POL_LINEAR_HOR)
			output_voltage = LNB_VOL_19_375;
		else
			output_voltage = LNB_VOL_14_375;
	}

	CH_DI_Print(3,("[GetPolaData] Tuner Id : %d, etVoltage(%x), etPolar(%x), Set  Output_voltage : %d \n", UnitId, etVoltage, etPolar, output_voltage));

	CH_DI_Print(3,("[GetPolaData] Tuner Id : %d, ucData: 0x%x \n", UnitId, ucData));

	switch (output_voltage)
	{

		case LNB_VOL_14_375:
			ucData |= (A8286_CTRL_VSEL2 |A8286_CTRL_VSEL0 );
			break;

		case LNB_VOL_15_042:
			ucData |= (A8286_CTRL_VSEL2 |A8286_CTRL_VSEL1| A8286_CTRL_VSEL0);
			break;

		case LNB_VOL_19_375:
			ucData |=  (A8286_CTRL_VSEL3 | A8286_CTRL_VSEL2 );
			break;

		case LNB_VOL_20_375:
			ucData |=  (A8286_CTRL_VSEL3 | A8286_CTRL_VSEL2 |A8286_CTRL_VSEL1 |A8286_CTRL_VSEL0 );
			break;

		case LNB_VOL_NONE:
		default:
			ucData |= (A8286_CTRL_VSEL2 |A8286_CTRL_VSEL0 );
			break;


	}

	*buf = ucData;

	CH_DI_Print(3,("[GetPolaData] Tuner Id : %d, RetData: 0x%x \n", UnitId, *buf));

	return ErrCode;
}

/********************************************************************************
 *	Function	: Drv_Ch_LnbSetPower
 *	Description :
 *	Input		:
 *	Output		:
 *	Return		:
 ********************************************************************************/

static DRV_Error GetPowerData(
		DRV_CH_LNB_POWER etPower,
		HUINT8  ucData,
		HUINT8 *buf
		)
{
	DRV_Error ErrCode = DRV_OK;


	if(etPower == LNB_POWER_ON)
	{

		CH_DI_Print(5,("[GetPowerData]  LNB_POWER_ON \n"));
		ucData |= A8286_CTRL_ENB;
	}
	else
	{
		CH_DI_Print(5,("[GetPowerData]  LNB_POWER_OFF  \n"));
		ucData &= ~A8286_CTRL_ENB;
	}

	*buf = ucData;

	CH_DI_Print(3,("[GetPowerData]  ucData: 0x%x \n", ucData));

	return ErrCode;
}

static DRV_Error GetShortFlag(
		HUINT32 UnitId,
		HUINT8	ucData,
		HUINT8 *buf
		)
{
	HUINT8 ucLnbShortFlag;
	DRV_Error ErrCode = DRV_OK;

	if( (UnitId%2) == 0 )
	{
		if((ucData & A8286_STATUS_OCP1)== A8286_STATUS_OCP1)
		{
			ucLnbShortFlag = 1;
		}
		else
		{
			ucLnbShortFlag = 0;
		}
	}
	else
	{
		if((ucData & A8286_STATUS_OCP2)== A8286_STATUS_OCP2)
		{
			ucLnbShortFlag = 1;
		}
		else
		{
			ucLnbShortFlag = 0;
		}
	}

	*buf = ucLnbShortFlag;

	CH_DI_Print(3,("[GetShortFlag] Tuner Id : %d, ucData: 0x%x \n", UnitId, *buf));
	return ErrCode;
}


static int ShortProtection(
		int  nUnitId,
		DRV_CH_LNB_POWER etPower
		)
{
	DRV_Error ErrCode = DRV_OK;
	HUINT8 ucWData;
	HUINT8 ucGetData;

	if( etPower == LNB_POWER_SHORT )
	{
		ucWData =s_ucLNBData[nUnitId][LNB_REG_0];

		WriteLnbReg( nUnitId, ucWData);

		CH_DI_Print(0,("[ShortProtection] Tuner Id : %d, LNB_POWER_SHORT ! Data: 0x%x \n", nUnitId, ucWData));
	}
	else if( etPower == LNB_POWER_OFF )
	{
		ucWData = s_ucLNBData[nUnitId][LNB_REG_0];

		ErrCode = GetPowerData(LNB_POWER_OFF, ucWData, &ucGetData);

		WriteLnbReg( nUnitId, ucGetData);

		CH_DI_Print(0,("[ShortProtection] Tuner Id : %d, LNB_POWER_OFF ! Data: 0x%x \n", nUnitId, ucWData));
	}

	return ErrCode;
}


/********************************************************************************
 *	Function	: Drv_Ch_LnbSetPower
 *	Description :
 *	Input		:
 *	Output		:
 *	Return		:
 ********************************************************************************/

static int  SetPower(
		int  nUnitId,
		DRV_CH_LNB_POWER etPower
		)
{
	HUINT8  ucLNBData, ucData;
	DRV_Error ErrCode = DRV_OK;

	if( nUnitId >= CONFIG_NUM_OF_SAT_CHANNEL)
	{
		CH_DI_Print(0,( "[SetPower] Tuner Id : %d, ERR! \n", nUnitId));
		return DRV_ERR;
	}

	ucLNBData = s_ucLNBData[nUnitId][LNB_REG_0];

	ErrCode = GetPowerData(etPower, ucLNBData, &ucData);

	s_ucLNBData[nUnitId][LNB_REG_0] = ucData;

	CH_DI_Print(3,("[SetPower] Tuner Id : %d, Data: 0x%x \n", nUnitId, ucData));


	ErrCode = WriteLnbReg(nUnitId, ucData);
	if( ErrCode != DRV_OK )
	{
		CH_DI_Print(0,("[SetPower] Tuner Id : %d, WriteLnbReg Error(0x%x) \n", nUnitId, ErrCode));
	}

	VK_TASK_Sleep(10);

	return ErrCode;
}
/********************************************************************************
 *							Global functions 									*
 ********************************************************************************/


/********************************************************************************
 *							Global functions 									*
 ********************************************************************************/



/********************************************************************************
 *	Function	: Drv_Ch_Lnb_Init
 *	Description :
 *	Input		:
 *	Output		:
 *	Return		:
 ********************************************************************************/

int LA8286_Init(
		int nUnitId
		)
{
	DRV_Error ErrCode = DRV_OK;
	HUINT8  ucRData[2], i;
	HUINT8 ucData[LNB_REG_NUM];
	int nRet = DRV_OK;

	if( nUnitId >= CONFIG_NUM_OF_SAT_CHANNEL)
	{
		CH_DI_Print(0,("[LA8286_Init] Tuner Id : %d \n", nUnitId));
		return DRV_ERR;
	}

	CH_DI_Print(1,("[LA8286_Init] Tuner Id : %d \n", nUnitId));

	if( nUnitId == 0 )
	{
		nRet = VK_SEM_Create((unsigned long *)&s_ulLNBSemID, "DRV_LNB", VK_SUSPENDTYPE_MAX);
		if( nRet != DRV_OK )
		{
			CH_DI_Print(0, ("[LA8286_Init] ERR! Create Semaphore! nRet(0x%x)", nRet));
			//return DRV_ERR;
		}
		nRet = VK_SEM_Create((unsigned long *)&s_ulCheckOverloadSemID, "DRV_LNB_SHORT", VK_SUSPENDTYPE_MAX);
		if( nRet != DRV_OK )
		{
			CH_DI_Print(0, ("[LA8286_Init] ERR! Create Semaphore! nRet(0x%x)", nRet));
			//return DRV_ERR;
		}
	}

	if( LNB_REG_NUM > 1)
	{
		/* lnb register initialize */
		for(i=0; i< LNB_REG_NUM; i++)
		{
			s_ucLNBData[nUnitId][i] = 0xFF;
			s_ucLnbShortFlag[nUnitId] = 0;
		}
	}


	ReadLnbReg(nUnitId, &ucRData[0]);

	CH_DI_Print(2,("[LA8286_Init] Tuner Id : %d, ucRData(0x%x) \n", nUnitId, ucRData[0]));

	ErrCode = GetControlData(nUnitId, &ucData[0]);

	CH_DI_Print(2,("[LA8286_Init] Tuner Id : %d, REG_0 (0x%x)  REG_1 (0x%x)\n", nUnitId, ucData[0], ucData[1]));

	s_ucLNBData[nUnitId][LNB_REG_0] = ucData[LNB_REG_0];

	ErrCode = WriteLnbReg( nUnitId,  ucData[LNB_REG_0]);
	if( ErrCode != DRV_OK )
	{
		CH_DI_Print(0,("[LA8286_Init] Tuner Id : %d, WriteLnbReg Error(0x%x) LINE(%d) \n", nUnitId, ErrCode, __LINE__));
		ErrCode = DRV_ERR;
	}

	s_ucLNBData[nUnitId][LNB_REG_1] = ucData[LNB_REG_1];

	ErrCode = WriteLnbReg( nUnitId,  ucData[LNB_REG_1]);
	if( ErrCode != DRV_OK )
	{
		CH_DI_Print(0,("[LA8286_Init] Tuner Id : %d, WriteLnbReg Error(0x%x) LINE(%d) \n", nUnitId, ErrCode, __LINE__));
		ErrCode = DRV_ERR;
	}

	SetActiveStatus( nUnitId, CH_ACTION_ON );

	return ErrCode;
} /* end of drv_ch_lnbh_Init */



int LA8286_SetOutput(int  nUnitId, HBOOL bEnable)
{
	int ErrCode = DI_CH_OK;

	if(bEnable == TRUE)
	{
		CH_DI_Print(2, ( "[LA8286_SetOutput] Tuner Id : %d, Enable output \n", nUnitId));
		ErrCode = SetPower( nUnitId, LNB_POWER_ON);
	}
	else
	{
		CH_DI_Print(2, ( "[LA8286_SetOutput] Tuner Id : %d, Disable output \n", nUnitId));
		ErrCode = SetPower( nUnitId, LNB_POWER_OFF);
	}

	return ErrCode;
}



/********************************************************************************
 *	Function	: Drv_Ch_LnbSetPolar
 *	Description :
 *	Input		:
 *	Output		:
 *	Return		:
 ********************************************************************************/

int LA8286_SetPolar(
		int  nUnitId,
		CH_SAT_Polarization_e etPolar,
		CH_SAT_LnbVoltage_e etVoltage )

{
	int ErrCode = DRV_OK;
	HUINT8 	  ucData = 0;
	HUINT8	  ucLNBData;

	if( nUnitId >= CONFIG_NUM_OF_SAT_CHANNEL)
	{
		CH_DI_Print(0,("[LA8286_SetPolar] Tuner Id : %d ERR! \n", nUnitId));
		return DRV_ERR;
	}

	CH_DI_Print(1,("[LA8286_SetPolar] Tuner Id : %d \n", nUnitId));

	ucLNBData = (s_ucLNBData[nUnitId][LNB_REG_0] &= ~0x0f);

	ErrCode = GetPolaData(nUnitId, etPolar, etVoltage, ucLNBData, &ucData);

	s_ucLNBData[nUnitId][LNB_REG_0] = ucData;

	CH_DI_Print(2,("[LA8286_SetPolar] Tuner Id : %d, Data: 0x%x \n", nUnitId, s_ucLNBData[nUnitId][LNB_REG_0]));

	ErrCode = WriteLnbReg(nUnitId, ucData);
	if( ErrCode != DRV_OK )
	{
		CH_DI_Print(0,("[LA8286_SetPolar] Tuner Id : %d, WriteLnbReg Error(0x%x) \n", nUnitId, ErrCode));
	}
	//VK_TASK_Sleep(10);
	VK_TASK_Sleep(5);

	return ErrCode;

}





/********************************************************************************
 *	Function	: LA8286_CheckAntState
 *	Description :
 *	Input		:
 *	Output		:
 *	Return		:
 ********************************************************************************/

int LA8286_CheckAntState(int  nUnitId)
{
	int nRet = DI_CH_OK;
	DRV_Error ErrCode = DRV_OK;
	HUINT8   ucData[2];
	HUINT8   ucLnbShortFlag[2];
	HUINT8 ucOverload = 0;
	HBOOL bEnable;


	if( nUnitId >= CONFIG_NUM_OF_SAT_CHANNEL)
	{
		CH_DI_Print(0,("[LA8286_CheckAntState] Tuner Id : %d ERR! \n", nUnitId));
		return DRV_ERR;
	}

	bEnable = DRV_CH_LNB_GetOutput(nUnitId);

	/* for  HW test sw : lnb output off */
	if((g_hwtest_lnb_off == 1)||(bEnable==FALSE))
	{
		CH_DI_Print(1,("[LA8286_CheckAntState] Tuner Id : %d, Not CheckOverload (hwtest_lnb_off) \n", nUnitId));
		return DRV_OK;
	}

        DRV_CHECKOVERLOAD_LOCK();

	ucOverload  = LNB_NOT_OVERLOADED;

	if( s_ucLNBData[nUnitId][LNB_REG_0] != 0xFF)
	{
		if(s_ucLnbShortFlag[nUnitId])
		{
			ShortProtection(nUnitId, LNB_POWER_SHORT);
		}
		else
		{
	 		SetPower( nUnitId, LNB_POWER_ON);
		}

		VK_TASK_Sleep(70); // min 48ms

		ErrCode = ReadLnbReg(nUnitId, &ucData[0]);

		CH_DI_Print(2,("[LA8286_CheckAntState] Tuner Id : %d, Data : 0x%x \n", nUnitId, ucData[0]));

		if(ErrCode != DRV_OK)
		{
            VK_TASK_Sleep(30);

			ErrCode = ReadLnbReg(nUnitId, &ucData[0]);
        }

		ErrCode = GetShortFlag(nUnitId, ucData[0],  &ucLnbShortFlag[nUnitId]);

		CH_DI_Print(2,("[LA8286_CheckAntState] Tuner Id : %d, ucLnbShortFlag : 0x%x \n", nUnitId, ucLnbShortFlag[nUnitId]));

		if(ErrCode != DRV_OK)
		{
            VK_TASK_Sleep(30);

			ErrCode = ReadLnbReg(nUnitId, &ucData[0]);
        }

		s_ucLnbShortFlag[nUnitId] = ucLnbShortFlag[nUnitId];

		if(s_ucLnbShortFlag[nUnitId])
		{
			ShortProtection(nUnitId, LNB_POWER_OFF);
		}

		if( s_ucLnbShortFlag[nUnitId])
		{
			ucOverload = LNB_OVERLOADED;
			nRet = DI_CH_ANTENNA_FAULT;
		}
		else
		{
			ucOverload = LNB_NOT_OVERLOADED;
		}
	}

	if( ErrCode != DRV_OK )
	{
		nRet = DI_CH_ERR;
	}
        DRV_CHECKOVERLOAD_UNLOCK();

	return nRet;
}


int LA8286_ReadStatus(
		int	nUnitId,
		HUINT8 *pucData
		)
{
	int nRet = DI_CH_OK;

	nRet = ReadLnbReg(nUnitId, pucData);
	if( nRet != DI_CH_OK )
	{
		CH_DI_Print(0, ( "[LA8286_ReadStatus] Tuner Id : %d, Error(0x%x)\n", nUnitId, nRet));
	}
	else
	{
		CH_DI_Print(0, ( "[LA8286_ReadStatus] Tuner Id : %d, *pucData = (0x%x)\n", nUnitId, *pucData));
	}
	return nRet;
}
/* EOF */

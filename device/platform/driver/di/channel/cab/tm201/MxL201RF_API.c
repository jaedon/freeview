/*

 Driver APIs for MxL201RF Tuner

 Copyright, Maxlinear, Inc.
 All Rights Reserved

 File Name:      MxL201RF_API.c

 Version:    6.2.5
*/


//#include "StdAfx.h"
#include "MxL201RF_API.h"
//#include "MxL_User_Define.h"
#include "MxL201RF.h"

#if defined(PLATFORM_TM201)
#include "drv_i2c.h"
#include "di_channel_priv.h"

#define MAST_TUNER_I2C_ADDR	0x60		//0xC0
extern HINT32 show_di_ch;

/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
                  I2C Functions (implement by customer)
  +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/

/******************************************************************************
**
**  Name: MxL_I2C_Write
**
**  Description:    I2C write operations
**
**  Parameters:
**					DeviceAddr	- MxL201RF Device address
**					pArray		- Write data array pointer
**					count		- total number of array
**
**  Returns:        0 if success
**
**  Revision History:
**
**   SCR      Date      Author  Description
**  -------------------------------------------------------------------------
**   N/A   12-16-2007   khuang initial release.
**
******************************************************************************/
UINT32  MxL_I2C_Write(UINT8 DeviceAddr, UINT8* pArray, UINT32 count)
{

	int nRet = 0, nAck=0;
	UINT32 i=0;

	//CH_DI_Print(6, ("MxL_I2C_Write[i2c %d][0x%x]\n", DeviceAddr, count));

	while(i<(count))
	{
  #if defined(CONFIG_I2C_MAP_FOR_2TUNER)
		nAck = DRV_I2c_WriteA8((DeviceAddr == MAST_TUNER_I2C_ADDR) ? I2C_CHANNEL_CHANNEL : I2C_CHANNEL_CHANNEL2, DeviceAddr, pArray[i], &pArray[i+1], 1);
  #else
        nAck = DRV_I2c_WriteA8(I2C_CHANNEL_CHANNEL, DeviceAddr, pArray[i], &pArray[i+1], 1);
  #endif
		if(nAck != DRV_OK)
		{
			CH_DI_Print(0, ("Mxl_I2C_Write Error(%d)\n", nAck));
		}
		i=i+2;
	}
	return nRet;
}

/******************************************************************************
**
**  Name: MxL_I2C_Read
**
**  Description:    I2C read operations
**
**  Parameters:
**					DeviceAddr	- MxL201RF Device address
**					Addr		- register address for read
**					*Data		- data return
**
**  Returns:        0 if success
**
**  Revision History:
**
**   SCR      Date      Author  Description
**  -------------------------------------------------------------------------
**   N/A   12-16-2007   khuang initial release.
**
******************************************************************************/
UINT32 MxL_I2C_Read(UINT8 DeviceAddr, UINT32 Addr, UINT8 * mData)
{

	int nRet = 0;
	UINT8	addr_data[2];

	//CH_DI_Print(6, ("MxL_I2C_Read[i2c %d][0x%x]\n", DeviceAddr, Addr));

	addr_data[0] = 0xFB;
	addr_data[1] = (UINT8)(Addr);
  #if defined(CONFIG_I2C_MAP_FOR_2TUNER)
	DRV_I2c_WriteA8((DeviceAddr == MAST_TUNER_I2C_ADDR) ? I2C_CHANNEL_CHANNEL : I2C_CHANNEL_CHANNEL2, DeviceAddr, addr_data[0], &addr_data[1], 1);

	nRet = DRV_I2c_ReadNoAddr((DeviceAddr == MAST_TUNER_I2C_ADDR) ? I2C_CHANNEL_CHANNEL : I2C_CHANNEL_CHANNEL2, DeviceAddr, mData, 1);
  #else
    DRV_I2c_WriteA8(I2C_CHANNEL_CHANNEL, DeviceAddr, addr_data[0], &addr_data[1], 1);

	nRet = DRV_I2c_ReadNoAddr( I2C_CHANNEL_CHANNEL, DeviceAddr, mData, 1);
  #endif
	if ( nRet != 0 )
	{
		CH_DI_Print(0, (" Error[0x%x]", nRet));
	}
	else
	{
		//CH_DI_Print(0, (" ucRegValue[0x%x]\n", ucRegValue));
	}

	return nRet;
}

/******************************************************************************
**
**  Name: MxL_Delay
**
**  Description:    Delay function in milli-second
**
**  Parameters:
**					mSec		- milli-second to delay
**
**  Returns:        0
**
**  Revision History:
**
**   SCR      Date      Author  Description
**  -------------------------------------------------------------------------
**   N/A   12-16-2007   khuang initial release.
**
******************************************************************************/
void MxL_Delay(UINT32 mSec)
{
	VK_TASK_Sleep(mSec);
}

const ShiftLUT SHIFT_LUT8[]=
{
	{331000000, 0x18, 0x04},
	{338000000, 0x2C, 0x04},
	{339000000, 0xB4, 0x07},
	{346000000, 0xD4, 0x07},
	{363000000, 0xD0, 0x07},
	{394000000, 0x2C, 0x04},
	{403000000, 0x62, 0x04},
	{410000000, 0x2C, 0x04},
	{411000000, 0xD4, 0x07},
	{418000000, 0xD4, 0x07},
	{434000000, 0xD4, 0x07},
	{435000000, 0xD4, 0x07},
	{466000000, 0x2C, 0x04},
	{506000000, 0xD4, 0x07},
	{538000000, 0x2C, 0x04},
	{578000000, 0xD4, 0x07},
	{610000000, 0x2C, 0x04},
	{650000000, 0xD4, 0x07},
	{682000000, 0x3C, 0x04},
	{722000000, 0xCA, 0x07},
	{754000000, 0x34, 0x04},
	{794000000, 0xCC, 0x07},
	{826000000, 0x34, 0x04},
	{866000000, 0xCC, 0x07},
	{898000000, 0x34, 0x04},
	{938000000, 0xCC, 0x07},
	{970000000, 0x34, 0x04},
	{-1, 0, 0}
};

const ShiftLUT SHIFT_LUT6[]=
{
	{339000000, 0xD4, 0x07},
	{345000000, 0x2C, 0x04},
	{357000000, 0x2C, 0x04},
	{363000000, 0xD2, 0x07},
	{375000000, 0xD4, 0x07},
	{381000000, 0x2C, 0x04},
	{393000000, 0x2C, 0x04},
	{399000000, 0xD4, 0x07},
	{411000000, 0xD4, 0x07},
	{417000000, 0x2C, 0x04},
	{429000000, 0x2C, 0x04},
	{435000000, 0xD4, 0x07},
	{447000000, 0xD4, 0x07},
	{453000000, 0x2C, 0x04},
	{465000000, 0x2C, 0x04},
	{471000000, 0xD4, 0x07},
	{501000000, 0x2C, 0x04},
	{507000000, 0xD4, 0x07},
	{537000000, 0x2C, 0x04},
	{543000000, 0xD4, 0x07},
	{573000000, 0x2C, 0x04},
	{579000000, 0xD4, 0x07},
	{609000000, 0x2C, 0x04},
	{615000000, 0xD4, 0x07},
	{645000000, 0x37, 0x04},
	{651000000, 0xCA, 0x07},
	{681000000, 0x34, 0x04},
	{687000000, 0xCC, 0x07},
	{717000000, 0x2C, 0x04},
	{723000000, 0xD4, 0x07},
	{753000000, 0x2C, 0x04},
	{759000000, 0xD4, 0x07},
	{789000000, 0x2C, 0x04},
	{795000000, 0xD4, 0x07},
	{825000000, 0x34, 0x04},
	{831000000, 0xCC, 0x07},
	{861000000, 0x34, 0x04},
	{867000000, 0xCC, 0x07},
	{897000000, 0x34, 0x04},
	{903000000, 0xCC, 0x07},
	{933000000, 0x34, 0x04},
	{939000000, 0xCC, 0x07},
	{969000000, 0x34, 0x04},
	{975000000, 0xCC, 0x07},
	{-1, 0, 0}
};

UINT32 MxL_Dump_Register(MxL201RF_TunerConfigS* myTuner)
{
	int i=0;
	UINT8 ucData=0;
	UINT32 Status=0;

	CH_DI_Print(5, ("MxL_Dump_Register\n"));
        MxL_Check_ChipVersion(myTuner,&Status);
	CH_DI_Print(5, ("\tMxL_Check_ChipVersion [0x%x]\n", Status));

	for(i=0; i<=0xff; i++)
	{
		MxL_Get_Register(myTuner, i, &ucData);
		CH_DI_Print(0, ("\t[0x%x] : [0x%x]\n", i, ucData));
	}

	return MxL_OK;
}

#endif


/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++//
//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++//
//																		   //
//							Tuner Functions								   //
//																		   //
//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++//
//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
MxL_ERR_MSG MxL_Set_Register(MxL201RF_TunerConfigS* myTuner, UINT8 RegAddr, UINT8 RegData)
{
	UINT32 Status=0;
	UINT8 pArray[2];
	pArray[0] = RegAddr;
	pArray[1] = RegData;
	Status = MxL_I2C_Write((UINT8)myTuner->I2C_Addr, pArray, 2);
	if(Status) return MxL_ERR_SET_REG;

	return MxL_OK;
}

MxL_ERR_MSG MxL_Get_Register(MxL201RF_TunerConfigS* myTuner, UINT8 RegAddr, UINT8 *RegData)
{
	if(MxL_I2C_Read((UINT8)myTuner->I2C_Addr, RegAddr, RegData))
		return MxL_ERR_GET_REG;
	return MxL_OK;

}

MxL_ERR_MSG MxL_Soft_Reset(MxL201RF_TunerConfigS* myTuner)
{
	UINT8 reg_reset[2];
	reg_reset[0] = 0xFF;
	reg_reset[1] = 0xFF;
	if(MxL_I2C_Write((UINT8)myTuner->I2C_Addr, reg_reset, 2))
		return MxL_ERR_OTHERS;

	return MxL_OK;
}


MxL_ERR_MSG MxL_Stand_By(MxL201RF_TunerConfigS* myTuner)
{
	UINT8 pArray[4];	/* a array pointer that store the addr and data pairs for I2C write	*/

	pArray[0] = 0x01;
	pArray[1] = 0x0;
	pArray[2] = 0x10;
	pArray[3] = 0x0;

	if(MxL_I2C_Write((UINT8)myTuner->I2C_Addr, pArray, 4))
		return MxL_ERR_OTHERS;

	return MxL_OK;
}

MxL_ERR_MSG MxL_Wake_Up(MxL201RF_TunerConfigS* myTuner)
{
	UINT8 pArray[2];	/* a array pointer that store the addr and data pairs for I2C write	*/

	pArray[0] = 0x01;
	pArray[1] = 0x01;

	if(MxL_I2C_Write((UINT8)myTuner->I2C_Addr, pArray, 2))
		return MxL_ERR_OTHERS;

	if(MxL_Tuner_RFTune(myTuner, myTuner->RF_Freq_Hz, myTuner->BW_MHz))
		return MxL_ERR_RFTUNE;

	return MxL_OK;
}

MxL_ERR_MSG MxL_Tuner_Init(MxL201RF_TunerConfigS* myTuner)
{
	UINT8 pArray[MAX_ARRAY_SIZE];	/* a array pointer that store the addr and data pairs for I2C write */
	UINT32 Array_Size;				/* a integer pointer that store the number of element in above array */

	/* Soft reset tuner */
	if(MxL_Soft_Reset(myTuner))
		return MxL_ERR_INIT;

	/* perform initialization calculation */
	MxL201RF_Init(pArray, &Array_Size, (UINT8)myTuner->Mode, (UINT32)myTuner->Xtal_Freq,
				(UINT32)myTuner->IF_Freq, (UINT8)myTuner->IF_Spectrum, (UINT8)myTuner->ClkOut_Setting, (UINT8)myTuner->ClkOut_Amp,
				(UINT8)myTuner->Xtal_Cap);

	/* perform I2C write here */
	if(MxL_I2C_Write((UINT8)myTuner->I2C_Addr, pArray, Array_Size))
		return MxL_ERR_INIT;

	return MxL_OK;
}


MxL_ERR_MSG MxL_Tuner_RFTune(MxL201RF_TunerConfigS* myTuner, UINT32 RF_Freq_Hz, MxL201RF_BW_MHz BWMHz)
{
	UINT8 pArray[MAX_ARRAY_SIZE];	/* a array pointer that store the addr and data pairs for I2C write */
	UINT32 Array_Size;				/* a integer pointer that store the number of element in above array */

	/* Register Address, Mask, and Value for CHANGE_CHAN_SQ */
	UINT8 Addr = 0x3E;
	UINT8 Mask = 0x20;
	UINT8 Val = 0;

	UINT32 counter = 0;

	/* Store information into struc	 */
	myTuner->RF_Freq_Hz = RF_Freq_Hz;
	myTuner->BW_MHz = BWMHz;

	/* Set CHANGE_CHAN_SQ = 0*/
	MxL_Get_Register(myTuner, Addr, &Val);
	Val = Val & ~Mask;
	MxL_Set_Register(myTuner, Addr, Val);

	/* perform Channel Change calculation */
	MxL201RF_RFTune(pArray,&Array_Size,RF_Freq_Hz,BWMHz, myTuner->Mode);

	/* perform I2C write here */
	if(MxL_I2C_Write((UINT8)myTuner->I2C_Addr, pArray, Array_Size))
		return MxL_ERR_RFTUNE;

	/* SHFLUT HERE */

	/* wait for 1ms */
	MxL_Delay(1);

	/* Set CHANGE_CHAN_SQ = 1*/
	Val = Val | Mask;
	MxL_Set_Register(myTuner, Addr, Val);

	MxL_Delay(2);

	/* SHFLUT */
	MxL_Set_Register(myTuner,0x5F,0x00);
	if(myTuner->Xtal_Freq == MxL_XTAL_24_MHZ)
	{
		counter = 0;
		if(myTuner->BW_MHz == MxL_BW_8MHz)
		{
			while(SHIFT_LUT8[counter].Ch_Freq_Hz != -1)
			{
				if(SHIFT_LUT8[counter].Ch_Freq_Hz == RF_Freq_Hz)
				{
					MxL_Set_Register(myTuner,0x5E,SHIFT_LUT8[counter].Reg1_Val);
					MxL_Set_Register(myTuner,0x5F,SHIFT_LUT8[counter].Reg2_Val);
					break;
				}
				counter++;
			}
		}
		else if(myTuner->BW_MHz == MxL_BW_6MHz)
		{
			while(SHIFT_LUT6[counter].Ch_Freq_Hz != -1)
			{
				if(SHIFT_LUT6[counter].Ch_Freq_Hz == RF_Freq_Hz)
				{
					MxL_Set_Register(myTuner,0x5E,SHIFT_LUT6[counter].Reg1_Val);
					MxL_Set_Register(myTuner,0x5F,SHIFT_LUT6[counter].Reg2_Val);
					break;
				}
				counter++;
			}
		}
	}

	/* Start Tune */
	MxL_Set_Register(myTuner, 0x10, 0x01);

	MxL_Delay(30);

	return MxL_OK;
}

MxL_ERR_MSG MxL_Enable_LT(MxL201RF_TunerConfigS* myTuner, UINT8 En_LT)
{
	UINT8 Status;
	if(En_LT)
		Status = MxL_Set_Register(myTuner, 0x0C, 0x00);
	else
		Status = MxL_Set_Register(myTuner, 0x0C, 0x01);

	if(Status)
		return MxL_ERR_SET_REG;

	return MxL_OK;
}


MxL_ERR_MSG MxL_Check_ChipVersion(MxL201RF_TunerConfigS* myTuner, MxL201RF_ChipVersion* myChipVersion)
{
	UINT8 Data;
	if(MxL_I2C_Read((UINT8)myTuner->I2C_Addr, 0x15, &Data))
		return MxL_GET_ID_FAIL;

	switch(Data & 0x0F)
	{
	case 0x06: *myChipVersion=MxL_201RF_ES4; break;
	default:
		*myChipVersion=MxL_UNKNOWN_ID;
	}

	return MxL_OK;
}

MxL_ERR_MSG MxL_RFSynth_Lock_Status(MxL201RF_TunerConfigS* myTuner, BOOL* isLock)
{
	UINT8 Data;
	*isLock = FALSE;
	if(MxL_I2C_Read((UINT8)myTuner->I2C_Addr, 0x14, &Data))
		return MxL_ERR_OTHERS;
	Data &= 0x0C;
	if (Data == 0x0C)
		*isLock = TRUE;  /* RF Synthesizer is Lock */
	return MxL_OK;
}

MxL_ERR_MSG MxL_REFSynth_Lock_Status(MxL201RF_TunerConfigS* myTuner, BOOL* isLock)
{
	UINT8 Data;
	*isLock = FALSE;
	if(MxL_I2C_Read((UINT8)myTuner->I2C_Addr, 0x14, &Data))
		return MxL_ERR_OTHERS;
	Data &= 0x03;
	if (Data == 0x03)
		*isLock = TRUE;   /*REF Synthesizer is Lock */
	return MxL_OK;
}

MxL_ERR_MSG MxL_Check_RF_Input_Power(MxL201RF_TunerConfigS* myTuner, REAL32* RF_Input_Level)
{
	REAL32 temp;

	UINT8 Data1, Data2, whole, dec;
	if(MxL_I2C_Read((UINT8)myTuner->I2C_Addr, 0x16, &Data1))  /* LSBs */
		return MxL_ERR_OTHERS;
	if(MxL_I2C_Read((UINT8)myTuner->I2C_Addr, 0x17, &Data2))  /* MSBs */
		return MxL_ERR_OTHERS;

	/* Determine whole and fractional portions of the power */
	whole = (Data1 >> 3) | (Data2 << 5);
	dec = Data1 & 0x07;

	temp = (REAL32)(whole) + (REAL32)(dec*0.125);

	*RF_Input_Level= temp - 120;

	return MxL_OK;
}

MxL_ERR_MSG MxL_Input_Power_LUT_Reg_Write(MxL201RF_TunerConfigS* myTuner, UINT8 LUT_Index, UINT8 LUT_Val)
{
	if(LUT_Index > 15)
		return MxL_ERR_OTHERS;

	MxL_Set_Register(myTuner, 0x6A, LUT_Index);
	MxL_Set_Register(myTuner, 0x6B, LUT_Val);

	return MxL_OK;
}
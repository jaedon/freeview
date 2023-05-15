/*

 Driver APIs for MxL201RF Tuner

 Copyright, Maxlinear, Inc.
 All Rights Reserved

 File Name:      MxL_User_Define.c / MxL_User_Define.h

 Version:    6.2.5
*/


#include "sembsl_mxl201.h"
#include "htype.h"
#include "di_channel_priv.h"
#include "drv_i2c.h"

#if defined(CONFIG_PLATFORM_TM201COMBO)				// JD - Add ifdef
#define MASTER_TUNER_CHTYPE_TER	0
#define MASTER_TUNER_CHTYPE_CAB	1
#define SLAVE_TUNER_CHTYPE_TER	2
#define SLAVE_TUNER_CHTYPE_CAB	3
#define MASTER_TUNER_I2C_ADDR	0x60		//0xC0
#define SLAVE_TUNER_I2C_ADDR	0x63		//0xC6
#define MASTER_TC90517_I2C_ADDR		0x1E	//0x3C
#define SLAVE_TC90517_I2C_ADDR		0x18	//0x30
#define MASTER_TC90517_THRU_READ	0xFEC1
#define MASTER_TC90517_THRU_WRITE	0xFEC0
#define SLAVE_TC90517_THRU_READ		0xFEC7
#define SLAVE_TC90517_THRU_WRITE	0xFEC6
#elif defined(CONFIG_EVIAN)
#define	I2C_TUNER_ADDR		((0xC0)>>1)
#endif

extern HINT32 show_di_ch;

/* SHFLUT */
typedef struct
{
	UINT32	Ch_Freq_Hz;
	UINT8	Reg1_Val;
	UINT8	Reg2_Val;
} ShiftLUT;

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


//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++//
//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++//
//																		   //
//					I2C Functions (implement by customer)				   //
//																		   //
//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++//
//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++//

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
UINT32 MxL_I2C_Write(MxL201RF_TunerConfigS* p_obj, UINT8* pArray, UINT32 count)
{
#if defined(CONFIG_PLATFORM_TM201COMBO)
	HINT32	nRet = 0, nAck = 0;
	HUINT32 i = 0;
	HUINT8	ucI2CBus = 0;
	HUINT8	ucDevAddr = 0;
	HUINT16 ucThruRegAddr = 0;

	/* Select Channel I2C Bus */
	if((p_obj->nChannelId == MASTER_TUNER_CHTYPE_TER) || (p_obj->nChannelId == MASTER_TUNER_CHTYPE_CAB))
	{
		ucI2CBus = MASTER_I2C_BUS;
	}
	else if((p_obj->nChannelId == SLAVE_TUNER_CHTYPE_TER) || (p_obj->nChannelId == SLAVE_TUNER_CHTYPE_CAB))
	{
		ucI2CBus = SLAVE_I2C_BUS;
	}
	else
	{
		CH_DI_Print(0,("MxL_I2C_Write Error! Wrong Channel ID(%d).. \n", p_obj->nChannelId));
	}

	/* Select device addr & reg addr & I2C writing method depending on channel type */
	if((p_obj->nChannelId == MASTER_TUNER_CHTYPE_TER) || (p_obj->nChannelId == SLAVE_TUNER_CHTYPE_TER)) 	/* Channel type is TER using TC90517 */
	{
		if(p_obj->nChannelId == MASTER_TUNER_CHTYPE_TER)
		{
			ucDevAddr = MASTER_TC90517_I2C_ADDR;
			ucThruRegAddr = MASTER_TC90517_THRU_WRITE;
		}
		else
		{
			ucDevAddr = SLAVE_TC90517_I2C_ADDR;
			ucThruRegAddr = SLAVE_TC90517_THRU_WRITE;
		}

		CH_DI_Print(6, ("MxL_I2C_Write[0x%x]\n", count));
		if(count == 1)
		{
			nAck = DRV_I2c_WriteA16(ucI2CBus, ucDevAddr, ucThruRegAddr, &pArray[0], 1);
			if(nAck != DRV_OK)
			{
				CH_DI_Print(0, ("Mxl_I2C_Write Error(%d)\n", nAck));
			}
		}
		else
		{
			while(i<(count))
			{
				nAck = DRV_I2c_WriteA16(ucI2CBus, ucDevAddr, ucThruRegAddr, &pArray[i], 2);
				if(nAck != DRV_OK)
				{
					CH_DI_Print(0, ("Mxl_I2C_Write Error(%d)\n", nAck));
				}
				i=i+2;
			}
		}
	}
	else if((p_obj->nChannelId == MASTER_TUNER_CHTYPE_CAB) || (p_obj->nChannelId == SLAVE_TUNER_CHTYPE_CAB))	/* Channel type is CAB using TDA10023 */
	{
		CH_UNUSED(ucThruRegAddr);

		if(p_obj->nChannelId == MASTER_TUNER_CHTYPE_CAB)
		{
			ucDevAddr = MASTER_TUNER_I2C_ADDR;
		}
		else
		{
			ucDevAddr = SLAVE_TUNER_I2C_ADDR;
		}

		CH_DI_Print(6, ("MxL_I2C_Write[0x%x]\n", count));
		while(i<(count))
		{
			nAck = DRV_I2c_WriteA8(ucI2CBus, ucDevAddr, pArray[i], &pArray[i+1], 1);
			if(nAck != DRV_OK)
			{
				CH_DI_Print(0, ("Mxl_I2C_Write Error(%d)\n", nAck));
			}
			i=i+2;
		}
	}
	else
	{
		CH_DI_Print(0,("MxL_I2C_Write Error! Wrong Channel ID(%d).. \n", p_obj->nChannelId));
	}

	return nRet;

#elif defined(CONFIG_EVIAN)
	int nRet = 0, nAck=0;
	U32 i=0;
	U8	ucI2CBus	= 0;
	U8	ucDevAddr	= 0;

	ucI2CBus	= I2C_CHANNEL_CHANNEL;
	ucDevAddr	= I2C_TUNER_ADDR;
	CH_UNUSED(p_obj);

//	nRet = Sec_1411xRepeaterEnable();
	if( nRet != 0 )
	{
		CH_DI_Print(0, ("nRet(%x)\n", nRet));
	}

	CH_DI_Print(6, ("MxL_I2C_Write[0x%x]\n", count));

	while(i<(count))
	{
		nAck = DRV_I2c_WriteA8(ucI2CBus, ucDevAddr, pArray[i], &pArray[i+1], 1);
		if(nAck != DRV_OK)
		{
			CH_DI_Print(0, ("Mxl_I2C_Write Error(%d)\n", nAck));
		}
		i=i+2;
	}

//	nRet = Sec_1411xRepeaterDisable();
	if( nRet != 0 )
	{
		CH_DI_Print(0, ("nRet(%x)\n", nRet));
	}

	return nRet;
#else
	int res;

	res=SEMOSAL_I2C_LOCK(&p_obj->i2c);
	if(res) goto err_exit;

	res|=SEMOSAL_I2C_WRITE(&p_obj->i2c,pArray,count);

	res|=SEMOSAL_I2C_UNLOCK(&p_obj->i2c);
err_exit:;
	return res;
#endif
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
UINT32 MxL_I2C_Read(MxL201RF_TunerConfigS* p_obj, UINT8 Addr, UINT8 * mData)
{
#if defined(CONFIG_PLATFORM_TM201COMBO)
	HINT32	nRet = 0;
	HUINT8	ucI2CBus = 0;
	HUINT8	ucDevAddr = 0;
	HUINT8	addr_data[2];
	HUINT16 ucThruRegReadAddr = 0;
	HUINT16 ucThruRegWriteAddr = 0;

	/* Select Channel I2C Bus */
	if((p_obj->nChannelId == MASTER_TUNER_CHTYPE_TER) || (p_obj->nChannelId == MASTER_TUNER_CHTYPE_CAB))
	{
		ucI2CBus = MASTER_I2C_BUS;
	}
	else if((p_obj->nChannelId == SLAVE_TUNER_CHTYPE_TER) || (p_obj->nChannelId == SLAVE_TUNER_CHTYPE_CAB))
	{
		ucI2CBus = SLAVE_I2C_BUS;
	}
	else
	{
		CH_DI_Print(0,("MxL_I2C_Reaad Error! Wrong Channel ID(%d).. \n", p_obj->nChannelId));
	}

	/* Select device addr & reg addr & I2C reading method depending on channel type */
	if((p_obj->nChannelId == MASTER_TUNER_CHTYPE_TER) || (p_obj->nChannelId == SLAVE_TUNER_CHTYPE_TER)) 	/* Channel type is TER using TC90517 */
	{
		if(p_obj->nChannelId == MASTER_TUNER_CHTYPE_TER)
		{
			ucDevAddr = MASTER_TC90517_I2C_ADDR;
			ucThruRegWriteAddr = MASTER_TC90517_THRU_WRITE;
			ucThruRegReadAddr = MASTER_TC90517_THRU_READ;
		}
		else
		{
			ucDevAddr = SLAVE_TC90517_I2C_ADDR;
			ucThruRegWriteAddr = SLAVE_TC90517_THRU_WRITE;
			ucThruRegReadAddr = SLAVE_TC90517_THRU_READ;
		}


		CH_DI_Print(4, ("MxL_I2C_Read[%d] : Addr = [0x%x]\n", p_obj->nChannelId, Addr));

		addr_data[0] = 0xFB;
		addr_data[1] = (HUINT8) (Addr);

		nRet = DRV_I2c_WriteA16(ucI2CBus, ucDevAddr, ucThruRegWriteAddr, &addr_data[0], 2);
		nRet = DRV_I2c_ReadA16(ucI2CBus, ucDevAddr, ucThruRegReadAddr, mData, 1 );

		if ( nRet != 0 )
		{
			CH_DI_Print(0, ("MxL_I2C_Read Error[0x%x]", nRet));
		}
		else
		{
			CH_DI_Print(4, ("MxL_I2C_Read[%d] : Read Val[0] = 0x%x\n", p_obj->nChannelId, *mData));
		}
	}
	else if((p_obj->nChannelId == MASTER_TUNER_CHTYPE_CAB) || (p_obj->nChannelId == SLAVE_TUNER_CHTYPE_CAB))	/* Channel type is CAB using TDA10023 */
	{
		CH_UNUSED(ucThruRegWriteAddr);
		CH_UNUSED(ucThruRegReadAddr);

		if(p_obj->nChannelId == MASTER_TUNER_CHTYPE_CAB)
		{
			ucDevAddr = MASTER_TUNER_I2C_ADDR;
		}
		else
		{
			ucDevAddr = SLAVE_TUNER_I2C_ADDR;
		}

		addr_data[0] = 0xFB;
		addr_data[1] = (UINT8)(Addr);

		nRet = DRV_I2c_WriteA8(ucI2CBus, ucDevAddr, addr_data[0], &addr_data[1], 1);
		nRet = DRV_I2c_ReadNoAddr(ucI2CBus, ucDevAddr, mData, 1);
		if ( nRet != 0 )
		{
			CH_DI_Print(0, ("MxL_I2C_Read Error[0x%x]", nRet));
		}
		else
		{
			CH_DI_Print(4, ("MxL_I2C_Read[%d] : Read Val[0] = 0x%x\n", p_obj->nChannelId, *mData));
		}
	}
	else
	{
		CH_DI_Print(0,("MxL_I2C_Read Error! Wrong Channel ID(%d).. \n", p_obj->nChannelId));
	}

	return nRet;

#elif defined(CONFIG_EVIAN)
	int nRet = 0;//, i;
	U8	addr_data[2];
	U8	ucI2CBus	= 0;
	U8	ucDevAddr	= 0;

	ucI2CBus	= I2C_CHANNEL_CHANNEL;
	ucDevAddr	= I2C_TUNER_ADDR;
	CH_UNUSED(p_obj);

	CH_DI_Print(6, ("MxL_I2C_Read[0x%x]\n", Addr));

//	nRet = Sec_1411xRepeaterEnable();
	if( nRet != 0 )
	{
		CH_DI_Print(0, ("nRet(%x)\n", nRet));
	}

	addr_data[0] = 0xFB;
	addr_data[1] = (U8)(Addr);
	DRV_I2c_WriteA8(ucI2CBus, ucDevAddr, addr_data[0], &addr_data[1], 1);

	nRet = DRV_I2c_ReadNoAddr(ucI2CBus, ucDevAddr, mData, 1);
	if ( nRet != 0 )
	{
		CH_DI_Print(0, (" Error[0x%x]", nRet));
	}
	else
	{
		//CH_DI_Print(0, (" ucRegValue[0x%x]\n", ucRegValue));
	}

//	nRet = Sec_1411xRepeaterDisable();
	if( nRet != 0 )
	{
		CH_DI_Print(0, ("nRet(%x)\n", nRet));
	}
	return nRet;
#else
	int 			res;
	U8				addr_data[2];

	res=SEMOSAL_I2C_LOCK(&p_obj->i2c);
	if(res) goto err_exit;

	addr_data[0]=0xFB;
	addr_data[1]=(U8)(Addr);
	res|=SEMOSAL_I2C_WRITE(&p_obj->i2c, addr_data, 2);
	res|=SEMOSAL_I2C_READ(&p_obj->i2c,mData,1);

	res|=SEMOSAL_I2C_UNLOCK(&p_obj->i2c);
err_exit:;
	return res;
#endif

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
#if defined(CONFIG_EVIAN) || defined(CONFIG_PLATFORM_TM201COMBO)
		VK_TASK_Sleep(mSec);
#else
		semosal_wait_msec( mSec);
#endif
}


/*
 MxL201RF Source Code : V6.2.5.0

 Copyright, Maxlinear, Inc.
 All Rights Reserved

 File Name:      MxL201RF.c / Mxl201RF.h

 Description: The source code is for MxL201RF user to quickly integrate MxL201RF into their software.
	There are two functions the user can call to generate a array of I2C command that's require to
	program the MxL201RF tuner. The user should pass an array pointer and an integer pointer in to the
	function. The funciton will fill up the array with format like follow:

		addr1
		data1
		addr2
		data2
		...

	The user can then pass this array to their I2C function to perform progromming the tuner.
*/

typedef struct
{
	UINT8 Num;	/*Register number */
	UINT8 Val;	/*Register value */
} IRVType, *PIRVType;

/* ========================== Local functions called by Init and RFTune ============================ */
UINT32 SetIRVBit(PIRVType pIRV, UINT8 Num, UINT8 Mask, UINT8 Val)
{
	while (pIRV->Num || pIRV->Val)
	{
		if (pIRV->Num==Num)
		{
			pIRV->Val&=~Mask;
			pIRV->Val|=Val;
		}
		pIRV++;

	}
	return MxL_OK;
}

/* Initializes the registers of the chip */
UINT32 MxL201RF_Init(UINT8* pArray,				/* a array pointer that store the addr and data pairs for I2C write */
					UINT32* Array_Size,			/* a integer pointer that store the number of element in above array */
					UINT8 Mode,
					UINT32 Xtal_Freq_Hz,
					UINT32 IF_Freq_Hz,
					UINT8 Invert_IF,
					UINT8 Clk_Out_Enable,
					UINT8 Clk_Out_Amp,
					UINT8 Xtal_Cap
					)
{

	UINT32 Reg_Index=0;
	UINT32 Array_Index=0;

	/* Terrestial register settings */
	IRVType IRV_Init[]=
	{
		/* { Addr, Data} */
		{ 0x02, 0x06},
		{ 0x03, 0x1A},
		{ 0x04, 0x14},
		{ 0x05, 0x0E},
		{ 0x0C, 0x00},
		{ 0x07, 0x01},
		{ 0x93, 0xD7},
		{ 0x95, 0x61},
		{ 0xA2, 0x00},
		{ 0x2F, 0x00},
		{ 0x60, 0x60},
		{ 0x70, 0x00},
		{ 0xB9, 0x10},
		{ 0x8E, 0x57},
		{ 0x58, 0x08},
		{ 0x5C, 0x00},
		{ 0x01, 0x01}, /*TOP_MASTER_ENABLE=1 */
		{ 0, 0}
	};

	IRVType IRV_Init_Cable[]=
	{
		/*{ Addr, Data}	 */
		{ 0x02, 0x06},
		{ 0x03, 0x1A},
		{ 0x04, 0x14},
		{ 0x05, 0x0E},
		{ 0x0C, 0x00},
		{ 0x07, 0x14},
		{ 0x29, 0x03},
		{ 0x45, 0x01},
		{ 0x7A, 0xCF},
		{ 0x7C, 0x7C},
		{ 0x7E, 0x27},
		{ 0x93, 0xD7},
		{ 0x99, 0x40},
		{ 0x2F, 0x00},
		{ 0x60, 0x60},
		{ 0x70, 0x00},
		{ 0xB9, 0x10},
		{ 0x8E, 0x57},
		{ 0x58, 0x08},
		{ 0x5C, 0x00},
		{ 0x48, 0x37},
		{ 0x55, 0x06},
		{ 0x01, 0x01}, /*TOP_MASTER_ENABLE=1 */
		{ 0, 0}
	};
	/*edit Init setting here */

	PIRVType myIRV = NULL;

	switch(Mode)
	{

	case MxL_MODE_DVBT: /*DVBT Mode	*/
		myIRV = IRV_Init;
		SetIRVBit(myIRV, 0x07, 0x1F, 0x01);
		SetIRVBit(myIRV, 0xA2, 0xFF, 0x02);
		break;
	case MxL_MODE_ATSC: /*ATSC Mode	*/
		myIRV = IRV_Init;
		SetIRVBit(myIRV, 0x07, 0x1F, 0x02);
		break;
	case MxL_MODE_CAB_OPT1: /*Cable Option 1 Mode */
		myIRV = IRV_Init_Cable;
		SetIRVBit(myIRV, 0x29, 0xFF, 0x09);
		SetIRVBit(myIRV, 0x45, 0xFF, 0x01);
		SetIRVBit(myIRV, 0x7A, 0xFF, 0x6F);
		SetIRVBit(myIRV, 0x7C, 0xFF, 0x1C);
		SetIRVBit(myIRV, 0x7E, 0xFF, 0x5A);
		SetIRVBit(myIRV, 0x93, 0xFF, 0xF7);
		break;
	case MxL_MODE_CAB_STD: /*Cable Standard Mode */
		myIRV = IRV_Init_Cable;
		SetIRVBit(myIRV, 0x45, 0xFF, 0x01);
		SetIRVBit(myIRV, 0x7A, 0xFF, 0x6F);
		SetIRVBit(myIRV, 0x7C, 0xFF, 0x1C);
		SetIRVBit(myIRV, 0x7E, 0xFF, 0x7C);
		SetIRVBit(myIRV, 0x93, 0xFF, 0xE7);
		break;
	default:
		return MxL_ERR_INIT;
	}

	switch(IF_Freq_Hz)
	{
	case MxL_IF_4_MHZ:
		SetIRVBit(myIRV, 0x02, 0x0F, 0x01);
		break;
	case MxL_IF_4_5_MHZ:
		SetIRVBit(myIRV, 0x02, 0x0F, 0x02);
		break;
	case MxL_IF_4_57_MHZ:
		SetIRVBit(myIRV, 0x02, 0x0F, 0x03);
		break;
	case MxL_IF_5_MHZ:
		SetIRVBit(myIRV, 0x02, 0x0F, 0x04);
		break;
	case MxL_IF_5_38_MHZ:
		SetIRVBit(myIRV, 0x02, 0x0F, 0x05);
		break;
	case MxL_IF_6_MHZ:
		SetIRVBit(myIRV, 0x02, 0x0F, 0x06);
		break;
	case MxL_IF_6_28_MHZ:
		SetIRVBit(myIRV, 0x02, 0x0F, 0x07);
		break;
	case MxL_IF_7_2_MHZ:
		SetIRVBit(myIRV, 0x02, 0x0F, 0x08);
		break;
	case MxL_IF_35_25_MHZ:
		SetIRVBit(myIRV, 0x02, 0x0F, 0x09);
		break;
	case MxL_IF_36_MHZ:
		SetIRVBit(myIRV, 0x02, 0x0F, 0x0A);
		break;
	case MxL_IF_36_15_MHZ:
		SetIRVBit(myIRV, 0x02, 0x0F, 0x0B);
		break;
	case MxL_IF_44_MHZ:
		SetIRVBit(myIRV, 0x02, 0x0F, 0x0C);
		break;
	default:
		return MxL_ERR_IF_FREQ;
	}


	if(Invert_IF)
		SetIRVBit(myIRV, 0x02, 0x10, 0x10);   /*Invert IF*/
	else
		SetIRVBit(myIRV, 0x02, 0x10, 0x00);   /*Normal IF*/


	switch(Xtal_Freq_Hz)
	{
	case MxL_XTAL_16_MHZ:
		SetIRVBit(myIRV, 0x04, 0x0F, 0x00);
		SetIRVBit(myIRV, 0x58, 0x03, 0x03);
		SetIRVBit(myIRV, 0x5C, 0xFF, 0x36);
		break;
	case MxL_XTAL_20_MHZ:
		SetIRVBit(myIRV, 0x04, 0x0F, 0x01);
		SetIRVBit(myIRV, 0x58, 0x03, 0x03);
		SetIRVBit(myIRV, 0x5C, 0xFF, 0x2B);
		break;
	case MxL_XTAL_20_25_MHZ:
		SetIRVBit(myIRV, 0x04, 0x0F, 0x02);
		SetIRVBit(myIRV, 0x58, 0x03, 0x03);
		SetIRVBit(myIRV, 0x5C, 0xFF, 0x2A);
		break;
	case MxL_XTAL_20_48_MHZ:
		SetIRVBit(myIRV, 0x04, 0x0F, 0x03);
		SetIRVBit(myIRV, 0x58, 0x03, 0x03);
		SetIRVBit(myIRV, 0x5C, 0xFF, 0x2A);
		break;
	case MxL_XTAL_24_MHZ:
		SetIRVBit(myIRV, 0x04, 0x0F, 0x04);
		SetIRVBit(myIRV, 0x58, 0x03, 0x00);
		SetIRVBit(myIRV, 0x5C, 0xFF, 0x48);
		break;
	case MxL_XTAL_25_MHZ:
		SetIRVBit(myIRV, 0x04, 0x0F, 0x05);
		SetIRVBit(myIRV, 0x58, 0x03, 0x00);
		SetIRVBit(myIRV, 0x5C, 0xFF, 0x45);
		break;
	case MxL_XTAL_25_14_MHZ:
		SetIRVBit(myIRV, 0x04, 0x0F, 0x06);
		SetIRVBit(myIRV, 0x58, 0x03, 0x00);
		SetIRVBit(myIRV, 0x5C, 0xFF, 0x44);
		break;
	case MxL_XTAL_28_8_MHZ:
		SetIRVBit(myIRV, 0x04, 0x0F, 0x08);
		SetIRVBit(myIRV, 0x58, 0x03, 0x00);
		SetIRVBit(myIRV, 0x5C, 0xFF, 0x3C);
		break;
	case MxL_XTAL_32_MHZ:
		SetIRVBit(myIRV, 0x04, 0x0F, 0x09);
		SetIRVBit(myIRV, 0x58, 0x03, 0x00);
		SetIRVBit(myIRV, 0x5C, 0xFF, 0x36);
		break;
	case MxL_XTAL_40_MHZ:
		SetIRVBit(myIRV, 0x04, 0x0F, 0x0A);
		SetIRVBit(myIRV, 0x58, 0x03, 0x00);
		SetIRVBit(myIRV, 0x5C, 0xFF, 0x2B);
		break;
	case MxL_XTAL_44_MHZ:
		SetIRVBit(myIRV, 0x04, 0x0F, 0x0B);
		SetIRVBit(myIRV, 0x58, 0x03, 0x02);
		SetIRVBit(myIRV, 0x5C, 0xFF, 0x4E);
		break;
	case MxL_XTAL_48_MHZ:
		SetIRVBit(myIRV, 0x04, 0x0F, 0x0C);
		SetIRVBit(myIRV, 0x58, 0x03, 0x02);
		SetIRVBit(myIRV, 0x5C, 0xFF, 0x48);
		break;
	case MxL_XTAL_49_3811_MHZ:
		SetIRVBit(myIRV, 0x04, 0x0F, 0x0D);
		SetIRVBit(myIRV, 0x58, 0x03, 0x02);
		SetIRVBit(myIRV, 0x5C, 0xFF, 0x45);
		break;
	default:
		return MxL_ERR_XTAL_FREQ;
	}

	if(!Clk_Out_Enable) /*default is enable  */
		SetIRVBit(myIRV, 0x03, 0x10, 0x00);

	/* Clk_Out_Amp */
	SetIRVBit(myIRV, 0x03, 0x0F, Clk_Out_Amp);

	/* Xtal Capacitor */
	if (Xtal_Cap >0 && Xtal_Cap <=25)
		SetIRVBit(myIRV, 0x05, 0xFF, Xtal_Cap);
	else if (Xtal_Cap == 0)
		SetIRVBit(myIRV, 0x05, 0xFF, 0x3F);
	else
		return MxL_ERR_INIT;


	/* Generate one Array that Contain Data, Address */
	while (myIRV[Reg_Index].Num || myIRV[Reg_Index].Val)
	{
		pArray[Array_Index++] = myIRV[Reg_Index].Num;
		pArray[Array_Index++] = myIRV[Reg_Index].Val;
		Reg_Index++;
	}

	*Array_Size=Array_Index;
	return MxL_OK;
}


/* Sets registers of the tuner based on RF freq, BW, etc. */
UINT32 MxL201RF_RFTune(UINT8* pArray,
					   UINT32* Array_Size,
					   UINT32 RF_Freq,
					   UINT8 BWMHz,
					   UINT8 Mode
					   )

{
	IRVType IRV_RFTune[]=
	{
	/*{ Addr, Data} */
		{ 0x10, 0x00},  /*abort tune*/
		{ 0x0D, 0x15},
		{ 0x0E, 0x40},
		{ 0x0F, 0x0E},
		{ 0xAB, 0x10},
		{ 0x91, 0x00},
		{ 0, 0}
	};

	UINT32 dig_rf_freq=0;
	UINT32 rf_freq_MHz = 0;
	UINT32 temp = 0 ;
	UINT32 Reg_Index=0;
	UINT32 Array_Index=0;
	UINT32 i = 0;
	UINT32 frac_divider = 1000000;

	rf_freq_MHz = RF_Freq/MHz;

	switch(Mode)
	{
		case MxL_MODE_CAB_OPT1: /* CABLE */
		case MxL_MODE_DVBT:		/* DVB-T */
			switch(BWMHz)
			{
				case MxL_BW_6MHz:
					SetIRVBit(IRV_RFTune, 0x0D, 0xFF, 0x95);
				break;
				case MxL_BW_7MHz:
					SetIRVBit(IRV_RFTune, 0x0D, 0xFF, 0xAA);
				break;
				case MxL_BW_8MHz:
					SetIRVBit(IRV_RFTune, 0x0D, 0xFF, 0xBF);
				break;
				default:
					return MxL_ERR_RFTUNE;
			}
		break;

		case MxL_MODE_ATSC: /*ATSC */
			SetIRVBit(IRV_RFTune, 0x0D, 0xFF, 0x99);
		break;

		case MxL_MODE_CAB_STD:	/*CABLE */
			switch(BWMHz)
			{
				case MxL_BW_6MHz:
					SetIRVBit(IRV_RFTune, 0x0D, 0xFF, 0x49);
				break;
				case MxL_BW_7MHz:
					SetIRVBit(IRV_RFTune, 0x0D, 0xFF, 0x5A);
				break;
				case MxL_BW_8MHz:
					SetIRVBit(IRV_RFTune, 0x0D, 0xFF, 0x6F);
				break;
				default:
					return MxL_ERR_RFTUNE;
			}
		break;

		default:
			return MxL_ERR_RFTUNE;
	}

	/*Convert RF frequency into 16 bits => 10 bit integer (MHz) + 6 bit fraction */
	dig_rf_freq = RF_Freq / MHz; /*Whole number portion of RF freq (in MHz) */
	temp = RF_Freq % MHz; /*Decimal portion of RF freq (in MHz) */
	for(i=0; i<6; i++)
	{
		dig_rf_freq <<= 1;
		frac_divider /=2;
		if(temp > frac_divider) /* Carryover from decimal */
		{
			temp -= frac_divider;
			dig_rf_freq++;
		}
	}

	/*add to have shift center point by 7.8124 kHz */
	if(temp > 7812)
		dig_rf_freq ++;

	SetIRVBit(IRV_RFTune, 0x0E, 0xFF, (UINT8)dig_rf_freq);
	SetIRVBit(IRV_RFTune, 0x0F, 0xFF, (UINT8)(dig_rf_freq>>8));

	/* Updated on 2010-0508 for V6.2.5*/
	if(Mode == MxL_MODE_CAB_STD || Mode == MxL_MODE_CAB_OPT1)
	{
		if(RF_Freq < 333000000)
			SetIRVBit(IRV_RFTune, 0xAB, 0xFF, 0x70);
		else if(RF_Freq < 667000000)
			SetIRVBit(IRV_RFTune, 0xAB, 0xFF, 0x20);
		else
			SetIRVBit(IRV_RFTune, 0xAB, 0xFF, 0x10);
	}
	else	/* If Terrestrial modes ... */
	{
		if(RF_Freq < 444000000)
			SetIRVBit(IRV_RFTune, 0xAB, 0xFF, 0x70);
		else if(RF_Freq < 667000000)
			SetIRVBit(IRV_RFTune, 0xAB, 0xFF, 0x20);
		else
			SetIRVBit(IRV_RFTune, 0xAB, 0xFF, 0x10);
	}


	if (RF_Freq <= 334000000)
		SetIRVBit(IRV_RFTune, 0x91, 0x40, 0x40);
	else
		SetIRVBit(IRV_RFTune, 0x91, 0x40, 0x00);


	/*Generate one Array that Contain Data, Address  */
	while (IRV_RFTune[Reg_Index].Num || IRV_RFTune[Reg_Index].Val)
	{
		pArray[Array_Index++] = IRV_RFTune[Reg_Index].Num;
		pArray[Array_Index++] = IRV_RFTune[Reg_Index].Val;
		Reg_Index++;
	}

	*Array_Size=Array_Index;

	return MxL_OK;
}


/*

 Driver APIs for MxL201RF Tuner

 Copyright, Maxlinear, Inc.
 All Rights Reserved

 File Name:      MxL201RF_API.c

 Version:    6.2.5
*/


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
	Status = MxL_I2C_Write(myTuner, pArray, 2);
	if(Status) return MxL_ERR_SET_REG;

	return MxL_OK;
}

MxL_ERR_MSG MxL_Get_Register(MxL201RF_TunerConfigS* myTuner, UINT8 RegAddr, UINT8 *RegData)
{
	if(MxL_I2C_Read(myTuner, RegAddr, RegData))
		return MxL_ERR_GET_REG;
	return MxL_OK;

}

MxL_ERR_MSG MxL_Soft_Reset(MxL201RF_TunerConfigS* myTuner)
{
	UINT8 reg_reset;
	reg_reset = 0xFF;
	if(MxL_I2C_Write(myTuner, &reg_reset, 1))
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

	if(MxL_I2C_Write(myTuner, pArray, 4))
		return MxL_ERR_OTHERS;

	return MxL_OK;
}

MxL_ERR_MSG MxL_Wake_Up(MxL201RF_TunerConfigS* myTuner)
{
	UINT8 pArray[2];	/* a array pointer that store the addr and data pairs for I2C write	*/

	pArray[0] = 0x01;
	pArray[1] = 0x01;

	if(MxL_I2C_Write(myTuner, pArray, 2))
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
	if(MxL_I2C_Write(myTuner, pArray, Array_Size))
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
	if(MxL_I2C_Write(myTuner, pArray, Array_Size))
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
	if(MxL_I2C_Read(myTuner, 0x15, &Data))
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
	if(MxL_I2C_Read(myTuner, 0x14, &Data))
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
	if(MxL_I2C_Read(myTuner, 0x14, &Data))
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
	if(MxL_I2C_Read(myTuner, 0x16, &Data1))  /* LSBs */
		return MxL_ERR_OTHERS;
	if(MxL_I2C_Read(myTuner, 0x17, &Data2))  /* MSBs */
		return MxL_ERR_OTHERS;

	/* Determine whole and fractional portions of the power */
	whole = (Data1 >> 3) | (Data2 << 5);
	dec = Data1 & 0x07;

	temp = (REAL32)(whole) + (REAL32)(dec*0.125);

	temp = temp - 120;

#if defined(CONFIG_PLATFORM_TM201COMBO)
	CH_DI_Print(5, ("MxL_Check_Rf_Input_Power:: TunerPower : [%f dBm] [%f dBuV]\n", temp, temp +  108.75));
	*RF_Input_Level = temp + 108.75;
#else
	*RF_Input_Level = temp;
#endif

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


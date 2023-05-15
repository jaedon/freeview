/*
 * (c) 2011-2012 Humax Co., Ltd.
 * This program is produced by Humax Co., Ltd. ("Humax") and
 * the proprietary Software of Humax and its licensors. Humax provides you, as an Authorized Licensee,
 * non-assignable, non-transferable and non-exclusive license to use this Software.
 * You acknowledge that this Software contains valuable trade secrets of Humax and by using this Software
 * you agree to the responsibility to take all reasonable efforts to protect the any information
 * you receive from Humax. You are not permitted to duplicate, modify, distribute, sell or lease and
 * reverse engineer or extract the source code of this Software unless you have Humax's written permission to do so.
 * If you have no authorized license, discontinue using this Software immediately.
 *
 * THE SOFTWARE IS PROVIDED "AS IS" AND HUMAX MAKES NO PROMISES, REPRESENTATIONS OR WARRANTIES, EITHER EXPRESS,
 * IMPLIED OR STATUTORY, OR OTHERWISE, WITH RESPECT TO THE SOFTWARE.
 * IN NO EVENT SHALL HUMAX BE LIABLE FOR LOST PROFITS, REVENUES, OR DATA, FINANCIAL LOSSES OR INDIRECT, SPECIAL,
 * CONSEQUENTIAL, EXEMPLARTY OR PUNITIVE DAMAGES WHATSOEVER RELATING TO YOUR USE OR INABILITY TO USE THE SOFTWARE.

 * This License is effective until terminated. You may terminate this License at any time by destroying all copies
 * of the Software including all documentation. This License will terminate immediately without notice from Humax
 * to you if you fail to comply with any provision of this License. Upon termination, you must destroy all copies
 * of the Software and all documentation.

 * The laws of the Republic of Korea will apply to any disputes arising out of or relating to this Copyright Notice.
 * All claims arising out of or relating to this Copyright Notice will be litigated in the Seoul Central District Court,
 * in the Republic of Korea.
*/

/******************************************************************************
File Name   : drv_channel_dp10048.c
Original Author: Full Name!!
Description : Channel driver
Remarks:
******************************************************************************/


/******************************************************************************
 *   Header Files
 ******************************************************************************/
#define _______________________________________________________________________
#define ____HEADER_FILES____

#include "htype.h"
#include "vkernel.h"

#include "di_channel.h"
#include "di_channel_priv.h"
#include "di_channel_attr.h"
#include "drv_i2c.h"
#include "drv_channel.h"
#include "drv_gpio.h"

#include "dsp48_ver5.0.h"

#include "drv_channel_dp10048.h"

#if defined(CONFIG_PLATFORM_TN18273)
#include "drv_channel_tn18273.h"
#elif defined(CONFIG_PLATFORM_TN18275)
#include "drv_channel_tn18275.h"
#endif


/******************************************************************************
 *   Defines
 ******************************************************************************/
#define _______________________________________________________________________
#define ____MACRO_DEFINITION____

#define SERIAL_INTERFACE_MODE

#define 	OFDM_UNLOCKED			0
#define 	OFDM_LOCKED				1
#define 	OFDM_CHECK_CANCELED		2

#define	WAIT_LOCK_TOTAL				50 		/*150 */
#define	LOCK_TRIAL					1
#define	ADC_POWER_ON_OFF_MASK	  	0x10

#define M_DEMOD	0
#define S_DEMOD		1


/***** Sampling frequency configuration ******/
#define TMBSLTDA10048_CFG_XTALL             16000000 /* Xtal frequency=16MHz or 4MHz */
#define TMBSLTDA10048_CFG_PLLMFACTOR        7//5
#define TMBSLTDA10048_CFG_PLLNFACTOR        3        /* 0: with Xtal=4MHz , 3: with Xtal=16MHz */
#define TMBSLTDA10048_CFG_PLLPFACTOR        0


#define TDA10048_DEMOD_0_ADDRESS	(0x10>>1)
#define TDA10048_DEMOD_1_ADDRESS	(0x12>>1)

#define CONST_I2C_REG_LENGTH	1
//#define SUPPORT_MANUAL_CODERATE
#if defined(CONFIG_PLATFORM_TN18275)
#define FW_DIVIDED_DOWNLOAD
#endif

/******************************************************************************
 *   Debug Option
 ******************************************************************************/
#define _______________________________________________________________________
#define ____TYPE_DEBUG_OPTION____

/*#define AGC_BER_DEBUG_ON*/
/*#define TUNER_WRITE_DATA*/

/******************************************************************************
 * Data Types
 ******************************************************************************/
#define _______________________________________________________________________
#define ____TYPE_DEFINITION____

typedef enum
{
	TDA10048_MOD_QPSK,
	TDA10048_MOD_16QAM,
	TDA10048_MOD_64QAM
} TDA10048_Moudulation_e;

typedef enum
{
	TDA10048_HIERARCHY_NONE,
	TDA10048_HIERARCHY_1,
	TDA10048_HIERARCHY_2,
	TDA10048_HIERARCHY_4
} TDA10048_Hierachy_e;

typedef enum
{
	TDA10048_CODERATE_1_2,
	TDA10048_CODERATE_2_3,
	TDA10048_CODERATE_3_4,
	TDA10048_CODERATE_5_6,
	TDA10048_CODERATE_7_8
} TDA10048_CodeRate_e;

typedef enum
{
	TDA10048_TRANSMODE_2K,
	TDA10048_TRANSMODE_8K,
} TDA10048_TransMode_e;

typedef enum
{
	TDA10048_GUARDINTERVAL_1_32,
	TDA10048_GUARDINTERVAL_1_16,
	TDA10048_GUARDINTERVAL_1_8,
	TDA10048_GUARDINTERVAL_1_4
} TDA10048_GuardInterval_e;

typedef enum
{
	TDA10048_OFFSET_NONE,
	TDA10048_OFFSET_1_6_P,
	TDA10048_OFFSET_1_6_N,
	TDA10048_OFFSET_2_6_P,
	TDA10048_OFFSET_2_6_N,
	TDA10048_OFFSET_3_6_P,
	TDA10048_OFFSET_3_6_N
} TDA10048_Offset_e;

typedef enum
{
	TUNER_TDA18273 	= 0,
	TUNER_MOPLL 	= 1
} CH_TER_Tuner_e;


/******************************************************************************
 * Extern Variables
 ******************************************************************************/
#define _______________________________________________________________________
#define ____EXTERNAL_VARIABLE_DEFINITION____

extern CH_InfoChannel_t g_stChannelConfig[NUM_OF_CHANNEL];
extern HINT32 show_di_ch;

/******************************************************************************
 * Global Variables
 ******************************************************************************/


/******************************************************************************
 * Extern Function Prototypes
 ******************************************************************************/


/******************************************************************************
 * Local(static) Function Prototypes
 ******************************************************************************/
#define _______________________________________________________________________
#define ____STATIC_FUNCTION_PROTOTYPES____

/******************************************************************************
 * Local(static) Variables
 ******************************************************************************/
#define _______________________________________________________________________
#define ____STATIC_VARIABLE_DEFINITION____

static CH_TER_AntStatus_t s_stAntState[CONFIG_NUM_OF_TER_CHANNEL];
static CH_TER_CodeRate_e                s_etRealCodeRate=0;
static HBOOL s_bTuned = FALSE;


/********************************************************************************
* Static functions
********************************************************************************/
#define _______________________________________________________________________
#define ____CH_T_PRIVATE_FUNCITONS____


static int ConvertTdaModulation( CH_TER_Moudulation_e etModulation )
{
    switch (etModulation)
    {
        case DI_CH_TMOD_QPSK:   return TDA10048_MOD_QPSK;
        case DI_CH_TMOD_16QAM:  return TDA10048_MOD_16QAM;
        case DI_CH_TMOD_64QAM:  return TDA10048_MOD_64QAM;
        default:
            CH_DI_Print(0, (" "));
            return -1;
    }
}

static int ConvertTdaHierarchy( CH_TER_Hierarchy_e etHierarchy )
{
    switch (etHierarchy)
    {
        case DI_CH_HIERARCHY_NONE:  return TDA10048_HIERARCHY_NONE;
        case DI_CH_HIERARCHY_1: return TDA10048_HIERARCHY_1;
        case DI_CH_HIERARCHY_2: return TDA10048_HIERARCHY_2;
        case DI_CH_HIERARCHY_4: return TDA10048_HIERARCHY_4;
        default:
            CH_DI_Print(0, (" "));
            return -1;
    }
}


static int ConvertTdaCodeRate( CH_TER_CodeRate_e etCodeRate )
{
    switch (etCodeRate)
    {
        case DI_CH_TCODERATE_1_2:   return TDA10048_CODERATE_1_2;
        case DI_CH_TCODERATE_2_3:   return TDA10048_CODERATE_2_3;
        case DI_CH_TCODERATE_3_4:   return TDA10048_CODERATE_3_4;
        case DI_CH_TCODERATE_5_6:   return TDA10048_CODERATE_5_6;
        case DI_CH_TCODERATE_7_8:   return TDA10048_CODERATE_7_8;
        default:
            CH_DI_Print(0, (" "));
            return -1;
    }
}


static int ConvertTdaFftSize( CH_TER_TransMode_e etTransMode )
{
    switch (etTransMode)
    {
        case DI_CH_TRANSMODE_2K:    return TDA10048_TRANSMODE_2K;
        case DI_CH_TRANSMODE_8K:    return TDA10048_TRANSMODE_8K;
        default:
            CH_DI_Print(0, (" "));
            return -1;
    }
}


static int ConvertTdaGuardInterval( CH_TER_GuardInterval_e etGuardInterval )
{
    switch (etGuardInterval)
    {
        case DI_CH_GUARDINTERVAL_1_32:  return TDA10048_GUARDINTERVAL_1_32;
        case DI_CH_GUARDINTERVAL_1_16:  return TDA10048_GUARDINTERVAL_1_16;
        case DI_CH_GUARDINTERVAL_1_8:   return TDA10048_GUARDINTERVAL_1_8;
        case DI_CH_GUARDINTERVAL_1_4:   return TDA10048_GUARDINTERVAL_1_4;
        default:
            CH_DI_Print(0, (" "));
            return -1;
    }
}

static int ConvertDiModulation( TDA10048_Moudulation_e etModulation )
{
    switch (etModulation)
    {
        case TDA10048_MOD_QPSK:     return DI_CH_TMOD_QPSK;
        case TDA10048_MOD_16QAM:    return DI_CH_TMOD_16QAM;
        case TDA10048_MOD_64QAM:    return DI_CH_TMOD_64QAM;
        default:
            CH_DI_Print(0, (" "));
            return -1;
    }
}

static int ConvertDiHierarchy( TDA10048_Hierachy_e etHierarchy )
{
    switch (etHierarchy)
    {
        case TDA10048_HIERARCHY_NONE:   return DI_CH_HIERARCHY_NONE;
        case TDA10048_HIERARCHY_1:      return DI_CH_HIERARCHY_1;
        case TDA10048_HIERARCHY_2:      return DI_CH_HIERARCHY_2;
        case TDA10048_HIERARCHY_4:      return DI_CH_HIERARCHY_4;
        default:
            CH_DI_Print(0, (" "));
            return -1;
    }
}

static int ConvertDiCodeRate( TDA10048_CodeRate_e etCodeRate )
{
    switch (etCodeRate)
    {
        case TDA10048_CODERATE_1_2:     return DI_CH_TCODERATE_1_2;
        case TDA10048_CODERATE_2_3:     return DI_CH_TCODERATE_2_3;
        case TDA10048_CODERATE_3_4:     return DI_CH_TCODERATE_3_4;
        case TDA10048_CODERATE_5_6:     return DI_CH_TCODERATE_5_6;
        case TDA10048_CODERATE_7_8:     return DI_CH_TCODERATE_7_8;
        default:
            CH_DI_Print(0, (" "));
            return -1;
    }
}

static int ConvertDiFftSize( TDA10048_TransMode_e etTransMode )
{
    switch (etTransMode)
    {
        case TDA10048_TRANSMODE_2K:     return DI_CH_TRANSMODE_2K;
        case TDA10048_TRANSMODE_8K:     return DI_CH_TRANSMODE_8K;
        default:
            CH_DI_Print(0, (" "));
            return -1;
    }
}


static int ConvertDiGuardInterval( TDA10048_GuardInterval_e etGuardInterval )
{
    switch (etGuardInterval)
    {
        case TDA10048_GUARDINTERVAL_1_32:   return DI_CH_GUARDINTERVAL_1_32;
        case TDA10048_GUARDINTERVAL_1_16:   return DI_CH_GUARDINTERVAL_1_16;
        case TDA10048_GUARDINTERVAL_1_8:    return DI_CH_GUARDINTERVAL_1_8;
        case TDA10048_GUARDINTERVAL_1_4:    return DI_CH_GUARDINTERVAL_1_4;
        default:
            CH_DI_Print(0, (" "));
            return -1;
    }
}


static int ConvertDiOffset( TDA10048_Offset_e etOffset )
{
    switch (etOffset)
    {
        case TDA10048_OFFSET_NONE:      return DI_CH_OFFSET_NONE;
        case TDA10048_OFFSET_1_6_P:     return DI_CH_OFFSET_1_6_P;
        case TDA10048_OFFSET_1_6_N:     return DI_CH_OFFSET_1_6_N;
        case TDA10048_OFFSET_2_6_P:     return DI_CH_OFFSET_2_6_P;
        case TDA10048_OFFSET_2_6_N:     return DI_CH_OFFSET_2_6_N;
        case TDA10048_OFFSET_3_6_P:     return DI_CH_OFFSET_3_6_P;
        case TDA10048_OFFSET_3_6_N:     return DI_CH_OFFSET_3_6_N;
        default:
            CH_DI_Print(0, (" "));
            return -1;
    }
}


/********************************************************************************
*	Function	: GetChI2CBus
*	Description	:
*	Input		:	physical unit id
*	Return	:
********************************************************************************/
#if defined(CONFIG_I2C_MAP_FOR_4TUNER)
static void GetChI2CBus(HUINT8 unitId, HUINT32 *ulI2CBus)
{

	if(unitId == 0)
	{
		*ulI2CBus = I2C_CHANNEL_CHANNEL;
	}
	else if(unitId == 1)
	{
		*ulI2CBus = I2C_CHANNEL_CHANNEL2;
	}
	else if(unitId == 2)
	{
		*ulI2CBus = I2C_CHANNEL_CHANNEL3;
	}
	else if(unitId == 3)
	{
		*ulI2CBus = I2C_CHANNEL_CHANNEL4;
	}
	else
	{
		*ulI2CBus = I2C_CHANNEL_CHANNEL;
	}
}
#else
static void GetChI2CBus(HUINT8 unitId, HUINT32 *ulI2CBus)
{
#if defined(CONFIG_PLATFORM_TN18275)
	if(unitId>=2)
	{
		unitId = S_DEMOD;
	}
	else
	{
		unitId = M_DEMOD;
	}
#endif

	if(unitId == M_DEMOD)
	{
		*ulI2CBus = I2C_CHANNEL_CHANNEL;
	}
#if defined(CONFIG_I2C_MAP_FOR_2TUNER)
	else if(unitId == S_DEMOD)
	{
		*ulI2CBus = I2C_CHANNEL_CHANNEL2;
	}
#endif
	else
	{
		*ulI2CBus = I2C_CHANNEL_CHANNEL;
	}
}

#endif
/********************************************************************************
*	Function	: GetDemodI2CAddr
*	Description	:
*	Input		:	physical unit id
*	Return	:
********************************************************************************/

static void GetDemodI2CAddr(HUINT8 unitId, HUINT16 *usChipAddr)
{
	if(unitId>=2)
	{
		unitId = unitId % 2;
	}

	if(unitId == M_DEMOD)
	{
		*usChipAddr = TDA10048_DEMOD_0_ADDRESS;
	}
#if defined(CONFIG_I2C_MAP_FOR_2TUNER)||defined(CONFIG_I2C_MAP_FOR_4TUNER)
	else if(unitId == S_DEMOD)
	{
		*usChipAddr = TDA10048_DEMOD_1_ADDRESS;
	}
#endif
	else
	{
		*usChipAddr = TDA10048_DEMOD_0_ADDRESS;
	}
}


/********************************************************************************
*   Function    : drv_demod_i2c_Read
*   Description :
*   Input       :
*   Return  :
********************************************************************************/

int drv_demod_i2c_Read(HUINT8 unitId, HUINT8 ui8RegAddress, HUINT8 *lpRegData)
{
	HUINT32 ulI2CBus = I2C_CHANNEL_CHANNEL;
	HUINT16 usChipAddr	= TDA10048_DEMOD_0_ADDRESS;
	int nResult = DI_CH_OK;
	int nRet = 0;


	if( lpRegData == NULL)
	{
		CH_DI_Print(0, (" Invalid Parameter(Data  NULL) "));
		return DI_CH_PARAM_ERR;
	}

	GetChI2CBus(unitId, &ulI2CBus);
	GetDemodI2CAddr(unitId, &usChipAddr);

	CH_DI_Print(6, (" [drv_demod_i2c_Read][%d] [%d] [0x%x]\n ",unitId, ulI2CBus, usChipAddr ));

	nRet = DRV_I2c_ReadA8( ulI2CBus, usChipAddr, ui8RegAddress, lpRegData, CONST_I2C_REG_LENGTH );
	if ( nRet != DI_CH_OK )
	{
		CH_DI_Print(0, (" [demod_i2c_Read] Error[0x%x]", nRet));
		nResult = DI_CH_ERR;
	}
	else
	{
		CH_DI_Print(6, (" [%s] Success \n", __FUNCTION__));
	}

	return nResult;

}


/********************************************************************************
*   Function    : drv_demod_i2c_Write
*   Description :
*   Input       :
*   Return  :
********************************************************************************/

int drv_demod_i2c_Write(HUINT8 unitId, HUINT8 ui8RegAddress, HUINT8 ui8RegData)
{
	HUINT32 ulI2CBus = I2C_CHANNEL_CHANNEL;
	HUINT16 usChipAddr	= TDA10048_DEMOD_0_ADDRESS;
	int nResult = DI_CH_OK;
    int nRet = 0;

	GetChI2CBus(unitId, &ulI2CBus);
	GetDemodI2CAddr(unitId, &usChipAddr);

	CH_DI_Print(6, (" [drv_demod_i2c_Write][%d] [%d] [0x%x]\n ",unitId, ulI2CBus, usChipAddr ));

	nRet = DRV_I2c_WriteA8( ulI2CBus, usChipAddr, ui8RegAddress, &ui8RegData, CONST_I2C_REG_LENGTH );
	if ( nRet != DI_CH_OK )
	{
		CH_DI_Print(0, (" Error[0x%x]", nRet));
		nResult = DI_CH_ERR;
	}
	else
	{
		CH_DI_Print(6, (" [%s] Success \n", __FUNCTION__));
	}

	return nResult;
}


/********************************************************************************
*   Function    : drv_demod_i2c_Write
*   Description :
*   Input       :
*   Return  :
********************************************************************************/

int drv_demod_i2c_WriteNum(HUINT8 unitId, HUINT8 ui8RegAddress, HUINT8 ui8RegData[], int nNumberToWrite)
{
	HUINT32 ulI2CBus = I2C_CHANNEL_CHANNEL;
	HUINT16 usChipAddr	= TDA10048_DEMOD_0_ADDRESS;
	int nResult = DI_CH_OK;
	int nRet = 0;

	if( ui8RegData == NULL)
	{
		CH_DI_Print(0, (" Invalid Parameter(Data  NULL) "));
		return DI_CH_PARAM_ERR;
	}

	GetChI2CBus(unitId, &ulI2CBus);
	GetDemodI2CAddr(unitId, &usChipAddr);

	CH_DI_Print(6, (" [drv_demod_i2c_WriteNum][%d] [%d] [0x%x]\n ",unitId, ulI2CBus, usChipAddr ));

	nRet = DRV_I2c_WriteA8( ulI2CBus, usChipAddr, ui8RegAddress, ui8RegData, nNumberToWrite );
	if ( nRet != DI_CH_OK )
	{
		CH_DI_Print(0, (" Error[0x%x]", nRet));
		nResult = DI_CH_ERR;
	}
	else
	{
		CH_DI_Print(6, (" [%s] Success!! \n", __FUNCTION__));
	}

	return nResult;
}


/********************************************************************************
*   Function    : DRV_CH_EnableI2c
*   Description :   Enable tuner I2C in Demod
*   Input       :
*   Return  :
********************************************************************************/

void DRV_CH_EnableI2c (HUINT8 unitId)
{
	HUINT8		oldval,newval;

	DRV_CH_LockI2c(unitId);

	/*(BP_I2CTUN == 1) The SCL_TUN, SDA_TUN I2C bus is connected to the master(SCL,SDA)I2C bus.*/
	drv_demod_i2c_Read(unitId, DEMOD_10048_REG_CONF_C4_1, &oldval);
	newval = oldval | 0x02 ;
	drv_demod_i2c_Write(unitId, DEMOD_10048_REG_CONF_C4_1, newval);

	drv_demod_i2c_Read(unitId, DEMOD_10048_REG_CONF_C4_1, &oldval);
	CH_DI_Print(6, (">>>>>>>>>>>>>>oldval(%x)\n", oldval));

}


/********************************************************************************
*   Function    : DRV_CH_DisableI2c
*   Description : Disable tuner I2C in Demod
*   Input       :
*   Return  :
********************************************************************************/

void DRV_CH_DisableI2c (HUINT8 unitId)
{
	HUINT8		oldval,newval;

	drv_demod_i2c_Read(unitId, DEMOD_10048_REG_CONF_C4_1, &oldval);
	newval = oldval & 0xFD ; /* (BP_I2CTUN == 0 ) The SCL_TUN and SDA_TUN are tri-stated. */
	drv_demod_i2c_Write(unitId, DEMOD_10048_REG_CONF_C4_1, newval);

	DRV_CH_UnLockI2c(unitId);
}


/********************************************************************************
*	Function	: ResetHwTda10048
*	Description	: The DSP could be reset to load DSP code.
*	Input		:	physical unit id
*	Return		:
********************************************************************************/

static int ResetHwTda10048(HUINT8 unitId)
{
	int nGpioNum = 0;

	switch(unitId)
	{
		case 0:
			nGpioNum = GPIO_ID_nCH0_RST;
			break;
#if (CONFIG_NUM_OF_TER_CHANNEL >= 2)
		case 1:
			nGpioNum = GPIO_ID_nCH1_RST;
			break;
#endif
#if (CONFIG_NUM_OF_TER_CHANNEL >= 3)
		case 2:
			nGpioNum = GPIO_ID_nCH2_RST;
			break;
		case 3:
			nGpioNum = GPIO_ID_nCH3_RST;
			break;
#endif
		default:
			nGpioNum = GPIO_ID_nCH0_RST;
			CH_DI_Print(0, (" Error : ResetHwTda10048 unitId[%d]... \n", unitId));
			break;
	}


	DRV_GPIO_Write(nGpioNum, 1);
	VK_TASK_Sleep(20);
	DRV_GPIO_Write(nGpioNum, 0);
	VK_TASK_Sleep(20);
	DRV_GPIO_Write(nGpioNum, 1);
	CH_DI_Print(2, (" GPIO_ID_nCH0[%d]_RST reset !!\n", unitId));

	return 0;

} /* end of ResetHwTda10048 */


/********************************************************************************
*	Function	: GetOFDMChipID
*	Description	:
*	Input		:	physical unit id
*	Return	:
********************************************************************************/

static void GetOFDMChipID(HUINT8 unitId)
{
	HUINT8	val;

	drv_demod_i2c_Read(unitId, DEMOD_10048_REG_IDENTITY,&val);

	if(val == 0x48 )
	{
		CH_DI_Print(1, ("############################### \n"));
		CH_DI_Print(1, ("[GetOFDMChipID][%d] val = 0x%x...\n\r", unitId, val));
		CH_DI_Print(1, ("############################### \n"));
	}
}


/********************************************************************************
*	Function	: set_MNP_PLL
*	Description :  Setting PLL values
*	Input		:	physical unit id
*	Return	:
********************************************************************************/

static void set_MNP_PLL(HUINT8 unitId)
{
	HUINT8 ival;

	drv_demod_i2c_Read(unitId, DEMOD_10048_REG_CONF_XO, &ival);
	CH_DI_Print(5, ("DEMOD_10048_REG_CONF_XO [%d][0x%x] \n", unitId, ival));
	ival = 0x0F;	drv_demod_i2c_Write(unitId, DEMOD_10048_REG_CONF_PLL1, ival);
	ival = 0x07;	drv_demod_i2c_Write(unitId, DEMOD_10048_REG_CONF_PLL2, ival);
	ival = 0x43;	drv_demod_i2c_Write(unitId, DEMOD_10048_REG_CONF_PLL3, ival);

	/*Direct IF config*/
	ival = 0xd9;	drv_demod_i2c_Write(unitId, DEMOD_10048_REG_FREQ_PHY2_MSB, ival);
	ival = 0x07;	drv_demod_i2c_Write(unitId, DEMOD_10048_REG_FREQ_PHY2_LSB, ival);
}


/********************************************************************************
*	Function	: set_NCO_PLL_parameterT
*	Description :
*	Input		:	physical unit id
*	Return	:
********************************************************************************/

static void set_NCO_PLL_parameterT(HUINT8 unitId, CH_TER_BandWidth_e etBandWidth)
{
	HUINT8 ival;

	if( etBandWidth == DI_CH_BW_8MHZ )
	{
		CH_DI_Print(5, ("CHANNEL BANDWIDTH == 8mhZ \n"));
		ival = 0x03;	drv_demod_i2c_Write(unitId, DEMOD_10048_REG_TIME_WREF_LSB, ival);
		ival = 0x2D;	drv_demod_i2c_Write(unitId, DEMOD_10048_REG_TIME_WREF_MID1, ival);
		ival = 0xD0;	drv_demod_i2c_Write(unitId, DEMOD_10048_REG_TIME_WREF_MID2, ival);
		ival = 0x02;	drv_demod_i2c_Write(unitId, DEMOD_10048_REG_TIME_WREF_MSB, ival);
		ival = 0xB0;	drv_demod_i2c_Write(unitId, DEMOD_10048_REG_TIME_INVWREF_LSB, ival);
		ival = 0x05;	drv_demod_i2c_Write(unitId, DEMOD_10048_REG_TIME_INVWREF_MSB, ival);
	}
	else if( etBandWidth == DI_CH_BW_7MHZ )
	{
		CH_DI_Print(5, ("CHANNEL BANDWIDTH == 7mhZ \n"));
		ival = 0x62;	drv_demod_i2c_Write(unitId, DEMOD_10048_REG_TIME_WREF_LSB, ival);
		ival = 0x27;	drv_demod_i2c_Write(unitId, DEMOD_10048_REG_TIME_WREF_MID1, ival);
		ival = 0x76;	drv_demod_i2c_Write(unitId, DEMOD_10048_REG_TIME_WREF_MID2, ival);
		ival = 0x02;	drv_demod_i2c_Write(unitId, DEMOD_10048_REG_TIME_WREF_MSB, ival);
		ival = 0x80;	drv_demod_i2c_Write(unitId, DEMOD_10048_REG_TIME_INVWREF_LSB, ival);
		ival = 0x06;	drv_demod_i2c_Write(unitId, DEMOD_10048_REG_TIME_INVWREF_MSB, ival);
	}
	else if( etBandWidth == DI_CH_BW_6MHZ )
	{
		CH_DI_Print(5, ("CHANNEL BANDWIDTH == 6mhZ \n"));
		ival = 0xC2;	drv_demod_i2c_Write(unitId, DEMOD_10048_REG_TIME_WREF_LSB, ival);
		ival = 0x21;	drv_demod_i2c_Write(unitId, DEMOD_10048_REG_TIME_WREF_MID1, ival);
		ival = 0x1C;	drv_demod_i2c_Write(unitId, DEMOD_10048_REG_TIME_WREF_MID2, ival);
		ival = 0x02;	drv_demod_i2c_Write(unitId, DEMOD_10048_REG_TIME_WREF_MSB, ival);
		ival = 0x95;	drv_demod_i2c_Write(unitId, DEMOD_10048_REG_TIME_INVWREF_LSB, ival);
		ival = 0x07;	drv_demod_i2c_Write(unitId, DEMOD_10048_REG_TIME_INVWREF_MSB, ival);
	}
	else
	{
		CH_DI_Print(0, ("CHANNEL BANDWIDTH ERROR(%d)", etBandWidth));
	}

}


/********************************************************************************
*	Function	: LoadDSPCode
*	Description :  Load DSP code for TDA10046 OFDM IC
*	Input		:	physical unit id
*	Return	:
********************************************************************************/

static void LoadDSPCode(HUINT8 unitId)
{
	HUINT8	ival;
#if defined(FW_DIVIDED_DOWNLOAD)
	int i=0, nLoop=0, nDldSize=0, nWrSize=0;
	int nFixSize = 2048;
	int nRet=0, nRetry=0;
	int nCodeSize=0;
	nCodeSize = sizeof(dspdata)/sizeof(HUINT8);
#endif
	/* The Host has to first set to 1 the register BOOT_HOST to initiate the boot.*/
	drv_demod_i2c_Read(unitId, DEMOD_10048_REG_CONF_C4_1,&ival);
	CH_DI_Print(5, ("DEMOD BOOT_HOST REGISTER,  %x \n",ival));

	/* SET THE BOOT_EEP_HOST TO 0 : boot from HOST */
	ival &= 0xfb;
	drv_demod_i2c_Write(unitId, DEMOD_10048_REG_CONF_C4_1,ival);

	/* Set START_BOOT to 1 */
	ival |= 0x08;
	drv_demod_i2c_Write(unitId, DEMOD_10048_REG_CONF_C4_1,ival);

	/* The Host has to write once in register DSPCPT in order to reset the code counter to 0( any value ) */
	ival = 0x00;
	drv_demod_i2c_Write(unitId, DEMOD_10048_REG_DSP_CODE_CPT, ival);

	VK_TASK_Sleep(10);

#if defined(FW_DIVIDED_DOWNLOAD)
		CH_DI_Print(3, ("######### LoadDSPCode() unitId[%d] : nCodeSize  (%d) #########\n",unitId, nCodeSize));
	nLoop = nCodeSize/nFixSize + 1;
	nWrSize = nFixSize;
		CH_DI_Print(6, ("######### nLoop  (%d)                                #########\n",nLoop));
	for(i =0;i<nLoop;i++)
	{
		CH_DI_Print(6, ("######### nDldSize(%d):(%d)nWrSize                   #########\n",nDldSize, nWrSize));

		for (nRetry = 0; nRetry < 3; nRetry++)
		{
			nRet = drv_demod_i2c_WriteNum( unitId, DEMOD_10048_REG_DSP_CODE_IN, &dspdata[nDldSize], nWrSize);
			if (nRet == DI_CH_OK)
			{
				break;
			}
			else
			{
				CH_DI_Print(0, (" Error(0x%x) from drv_demod_i2c_WriteNum()", nRet));
			}
		}
		//VK_TASK_Sleep(100);
		nDldSize+= nWrSize;
		nCodeSize-= nWrSize;
		if(nCodeSize < nFixSize)
		{
			nWrSize = nCodeSize;
		}
		CH_DI_Print(6, ("######### nCodeSize  (%d)                            #########\n",nCodeSize));
	}
#else
	/* then write the code in register DSPCODEIN */
	/*  kjeom replaced this code at 061013 to prevent infinite loop...*/
	while( 0 != drv_demod_i2c_WriteNum( unitId, DEMOD_10048_REG_DSP_CODE_IN, dspdata, sizeof(dspdata)/sizeof(HUINT8)))
	{
		CH_DI_Print(5, ("CH:I2C DSP code send error\n"));
		VK_TASK_Sleep(1000);
	}
#endif

}


/********************************************************************************
*	Function	: drv_channel_loadDSPcode
*	Description :  Load DSP code for TDA10046 OFDM IC
*	Input		:	physical unit id
*	Return	:
********************************************************************************/

static HUINT8 drv_channel_loadDSPcode(HUINT8 unitId)
{
	HUINT8	ival;

	CH_DI_Print(3, ("drv_channel_loadDSPcode:  %s \n",dspcode_version));
	LoadDSPCode(unitId);
	VK_TASK_Sleep(100);

	/* Check the transfer has been correct */
	drv_demod_i2c_Read(unitId, DEMOD_10048_REG_SYNC_STATUS, &ival);
	CH_DI_Print(5, ("DEMOD SYNC_STATUS REGISTER,  %x \n",ival));

	ival &= 0x40;
	if(ival == 0x40)
	{
		CH_DI_Print(5, ("DSP Download is O.K -- The DSPready bit SET \n"));
		return TRUE;
	}
	else
	{
		CH_DI_Print(4, ("DSP Download is not O.K. -- The DSPready bit is not SET, require retransfer \n"));
		return FALSE ;
	}
}


/********************************************************************************
*	Function	: drv_ADC_Reset
*	Description :  Reset ADC
*	Input		:	physical unit id
*	Return	:
********************************************************************************/

void drv_ADC_Reset(HUINT8 unitId)
{
	HUINT8 ival;
	int ret;

	/* ADC Power Down Mode */
	ret = drv_demod_i2c_Read(unitId, DEMOD_10048_REG_CONF_ADC2, &ival);
	if (ret != 0)
	{
		CH_DI_Print(0, (" Error(0x%x) from iicWriteIndexedReg()", ret));
	}

	ival |= ADC_POWER_ON_OFF_MASK;
	drv_demod_i2c_Write(unitId, DEMOD_10048_REG_CONF_ADC2, ival);

	VK_TASK_Sleep(50);

	/* ADC Normal Mode */
	ret = drv_demod_i2c_Read(unitId, DEMOD_10048_REG_CONF_ADC2, &ival);
	if (ret != 0)
	{
		CH_DI_Print(0, ("Error(0x%x) from iicWriteIndexedReg()", ret));
	}

	ival &= ~ADC_POWER_ON_OFF_MASK;
	drv_demod_i2c_Write(unitId, DEMOD_10048_REG_CONF_ADC2, ival);
}/* end of drv_ADC_Reset */


/********************************************************************************
*	Function	: Soft Reset
*	Description :  The DSP could be reset to start its application software.
*	Input		:	physical unit id
*	Return	:
********************************************************************************/

static void soft_resetT(HUINT8 unitId)
{
	HUINT8 newval;

	drv_demod_i2c_Read(unitId, DEMOD_10048_REG_AUTO, &newval);
	newval = newval | 0x40; /* DSP reset bit set to 1 */
	drv_demod_i2c_Write(unitId, DEMOD_10048_REG_AUTO, newval);
}


/********************************************************************************
*	Function	: config_demod_pio
*	Description :  THIS FUNCTION is for the ANTENNA POWER DETECTION
*	Input		:	physical unit id
*	Return	:
********************************************************************************/

void config_demod_pio(HUINT8 unitId)
{
	HUINT8	ival;

	CH_DI_Print(5, ("[config_demod_pio] \n" ));
	if( unitId == 0 )
	{
	    /* ----------------------------------
		  GPIO (ST5202)
		  GPIO0 used for CH_INT
		  GPIO1 NOT USED (X)
		  GPIO2 used for ANT_PW_EN output
		  GPIO3 used for 5V_ANT_CHK input

		  select IO
		  00: pull-up drive output (for 3.3V output level),
		  01: input,
		  10: open-drain output,
	  	  11: forced to 3-state mode (default value)
	    --------------------------------------- */
		/*  select IO */
		ival = 0x40;	drv_demod_i2c_Write(unitId, DEMOD_10048_REG_CONF_TRISTATE2, ival);
		/*  polarity IO */
		ival = 0x00;	drv_demod_i2c_Write(unitId, DEMOD_10048_REG_CONF_POLARITY, ival);
		/* configure IO */
		/* GPIO3 output:11 - GPIO2 output :11 - GPIO1 not used :11 - GPIO0 don't care:00 */
		ival = 0xFE;	drv_demod_i2c_Write(unitId, DEMOD_10048_REG_GPIO_OUT_SEL, ival); /* 11111110: 0xFE */
		/* enable write by I2C */
	/*	ival = 0x03;	drv_demod_i2c_Write(unitId, DEMOD_10048_REG_GPIO_SELECT, ival); */
		ival = 0x05;	drv_demod_i2c_Write(unitId, DEMOD_10048_REG_GPIO_SELECT, ival); /* change GPIO0 as UNCOR bit */

	/* ANT_PW_EN :	High Output */
	drv_demod_i2c_Write(unitId, DEMOD_10048_REG_GPIO_SP_DS2, 0xFF );
	}
#ifdef DUAL_TER_TUNER
	else	/*Unit #1*/
	{
		/*-------------------------------
		// GPIO
		// GPIO0 input for the comparator
		// GPIO1 not used
		// GPIO2 not used
		// GPIO3 not used
		//-------------------------------
		// select IO
		// GPIO3 tristate:11 - GPIO2 tristate:11 - GPIO1 tristate:11 - GPIO0 input:01
		// 11111101: 0xFD
		*/
		ival = 0x02;	drv_demod_i2c_Write(unitId, DEMOD_10048_REG_CONF_TRISTATE2, ival);
		ival = 0x00;	drv_demod_i2c_Write(unitId, DEMOD_10048_REG_CONF_POLARITY, ival);
		/* configure IO
		// GPIO3 output:11 - GPIO2 output :11 - GPIO1 not used :11 - GPIO0 don't care:00
		// 11111100: 0xFC
		*/
		ival = 0xdC;	drv_demod_i2c_Write(unitId, DEMOD_10048_REG_GPIO_OUT_SEL, ival);

			/* enable write by I2C */
			ival = 0x28;	drv_demod_i2c_Write(unitId, DEMOD_10048_REG_GPIO_SELECT, ival);
	}
#endif  /* DUAL_TER_TUNER */
}


/********************************************************************************
*	Function	: check_fec_lock
*	Description :  	Configure registers as the initial value
				"0xffff" is used to mark the followed registers
				- READ-OLNY register
				- TPS and DDFS_OFFSET register
				- registers written at other routine
<< Caution>>
1) Internal ADC format = 2's complement and uPC2798 as a down-converter
2) AGC polarity bit = 1: AGC from L64781 increases,the gain of IF AGC is increased

*	Input		:	physical unit id
*	Return	:
********************************************************************************/

static void set_demod_registersT(HUINT8 unitId, CH_TER_TuneParam_t * pstTerTuneParam )
{
	HUINT8	ival;

    if ( pstTerTuneParam == NULL )
    {
		CH_DI_Print(0, ("[set_demod_registersT] pstTerTuneParam is NULL \n"));
		return;
    }

	/* Level and Polarity of output pads */
	//ival = 0x21;	drv_demod_i2c_Write(unitId, DEMOD_10048_REG_CONF_TRISTATE1, ival);
	// Remain Disable TS until Lock Event
	ival = 0x61;	drv_demod_i2c_Write(unitId, DEMOD_10048_REG_CONF_TRISTATE1, ival);

	/* 0x21: 0010 0001
		[7] - 0
		[6] - 0 (Z_TS: the signals are normal push-pull drive outputs
		[5] - 1 (Reservecd set 1)
		[3:4] -00(Z_AGC_TUN[1:0] push-pull drive output)
		[1:2]- 00(Z_AGC_IF[1:0] push-pul drive output)
		[0] - 1(CLR_I2C_N: Reset not active)  */

    /*  FOR SETTING PARALLEL MODE A/B
	IF WE KNOW THE ALL PARAMETER AND CAN CALCULATE THE EXACT DATA RATE AT THIS POINT,  SETTING PARALLEL MODE B.
	OTHER CASE, THAT IS, WE CAN NOT CALCULATE THE EXACET DATA RATE SETTING PARALLEL MODE A. */

	/* Configure ADC */
	ival = 0x60;	drv_demod_i2c_Write(unitId, DEMOD_10048_REG_CONF_ADC, ival);
		/* 1 	two's complement
		// 1000	current consumption of ADC : 52 MHz Fs used
		// 0	AC-coupled
		// 0	Vrange = 2V, Gain = 0
		*/
	/* ADC Normal Mode */
	ival = 0x00;	drv_demod_i2c_Write(unitId, DEMOD_10048_REG_CONF_ADC2, ival);

	/* Configure Enable or bypass frontend functions */
	ival = 0xA8;	drv_demod_i2c_Write(unitId, DEMOD_10048_REG_CONF_C1_1, ival);
	ival = 0x00;	drv_demod_i2c_Write(unitId, DEMOD_10048_REG_CONF_C3_1, ival);
	ival = 0x00;	drv_demod_i2c_Write(unitId, DEMOD_10048_REG_CONF_C4_1, ival);/* boot HOST/EEP */

	/* use Pulse Killer and no inversion spectrum */
	ival = 0x0b;	drv_demod_i2c_Write(unitId, DEMOD_10048_REG_CONF_C1_3, ival);  /* IF AGC Polarity --> Invert mode */

	/* Configure AGC  */
	ival = 0x10 |TDA10048_AGCCONF_INIT_BIT; /* reg[70]D[5] '0':single IF AGC loop, '1':Double AGC loop, D[4]:AUTOAGC */
	drv_demod_i2c_Write(unitId, DEMOD_10048_REG_AGC_CONF, ival);

	ival = 0xCC;	drv_demod_i2c_Write(unitId, DEMOD_10048_REG_AGC_THRESHOLD, ival);
	ival = 0x00;	drv_demod_i2c_Write(unitId, DEMOD_10048_REG_AGC_THRESHOLD_MSB, ival);
	ival = 0x08;	drv_demod_i2c_Write(unitId, DEMOD_10048_REG_AGC_RENORM, ival);
	/* ival = 0x00;	drv_demod_i2c_Write(unitId, DEMOD_10048_REG_AGC_GAINS, ival); */
    ival = 0x12;    drv_demod_i2c_Write(unitId, DEMOD_10048_REG_AGC_GAINS, ival); /* same tda10046 , 0x01->0x12 */
	ival = 0x00;	drv_demod_i2c_Write(unitId, DEMOD_10048_REG_AGC_TUN_MIN, ival);
	ival = 0xFF;	drv_demod_i2c_Write(unitId, DEMOD_10048_REG_AGC_TUN_MAX, ival);
	ival = 0x00;	drv_demod_i2c_Write(unitId, DEMOD_10048_REG_AGC_IF_MIN, ival);
	ival = 0xFF;	drv_demod_i2c_Write(unitId, DEMOD_10048_REG_AGC_IF_MAX, ival);

	/* Interrupt Disable */
	ival = 0x00;	drv_demod_i2c_Write(unitId, DEMOD_10048_REG_IT_SEL, ival);

	/* Digital AGC Level :Gain : 29dB	 */
	ival = 0x7b;	drv_demod_i2c_Write(unitId, DEMOD_10048_REG_DIG_AGC_LEVEL, ival);

	/* BER */
	/* ival = 0x3b;	drv_demod_i2c_Write(unitId, DEMOD_10048_REG_CVBER_CTL, ival); */
	 ival = 0x38;    drv_demod_i2c_Write(unitId, DEMOD_10048_REG_CVBER_CTL, ival);

	/* Enable Outside Guard Inteval : 0x04 */
	/* ival = 0x14;	drv_demod_i2c_Write(unitId, DEMOD_10048_REG_CONF_C4_2, ival); */
	ival = 0x00;	drv_demod_i2c_Write(unitId, DEMOD_10048_REG_CONF_C4_2, ival);

#if 0
	/* Configure Enable or bypass backend functions  */
	//ival = 0x86;	drv_demod_i2c_Write(unitId, DEMOD_10048_REG_CONF_C3_1, ival);
	ival = 0x84;	drv_demod_i2c_Write(unitId, DEMOD_10048_REG_CONF_C3_1, ival);
	ival = 0x00;	drv_demod_i2c_Write(unitId, DEMOD_10048_REG_DSP_CODE_CPT, ival);

#endif

}


/********************************************************************************
*	Function	: check_fec_lock
*	Description :  	FEC lock:
				Check the BER value after Viterbi decoding
				if BER < 10e-4, it is lock
				the threshold is 2 X 10 -4,
*	Input		:	physical unit id
*	Return	:
********************************************************************************/

static HUINT8 check_fec_lock(HUINT8 unitId)
{
    HUINT8   ival, jval,count=0;
    HUINT32    iuncor_cpt=0,juncor_cpt=0;

	/* check uncorrectable blocks counter */
	ival = 0x80;
	drv_demod_i2c_Write(unitId, DEMOD_10048_REG_UNCOR_CTRL, ival );
	drv_demod_i2c_Read(unitId, DEMOD_10048_REG_UNCOR_CPT_LSB, &ival );
	drv_demod_i2c_Read(unitId, DEMOD_10048_REG_UNCOR_CPT_MSB, &jval );
	iuncor_cpt =(HUINT32) ival |((HUINT32)jval<<8);

	CH_DI_Print(5, ("[UnitId [%d] UNCORRECTED BLOCK = %d \n",unitId,ival));

	if (iuncor_cpt)
	{
		do{
		ival=0x80; /*UNCOR_CTRL_CLEAR */
		drv_demod_i2c_Write(unitId, DEMOD_10048_REG_UNCOR_CTRL, ival);
		VK_TASK_Sleep(WAIT_LOCK_TOTAL);
		drv_demod_i2c_Read(unitId, DEMOD_10048_REG_UNCOR_CPT_LSB, &ival);
		drv_demod_i2c_Read(unitId, DEMOD_10048_REG_UNCOR_CPT_MSB, &jval);
		juncor_cpt =(HUINT32) ival |((HUINT32)jval<<8);
		count ++;
		}  while (juncor_cpt >= iuncor_cpt && count < 5);
	}

	if( juncor_cpt < 0x10 )
		return TRUE;
	else
		return FALSE;
}


/********************************************************************************
*	Function	:  check_ofdm_lock
*	Description :  	OFDM lock: clock, freq, FFT start window and frame
				FFT Window position lock
				Timing Lock
				Frequency Lock
				TPS lock
*	Input		:
*	Return	:
********************************************************************************/

static HUINT8 check_ofdm_lock(HUINT8 unitId)
{
    HUINT8 ival = 0,testout = 0;
	HUINT8 loop=0,loop_fec=0;
	HUINT8 LOCK_OUT_TIME ;

	LOCK_OUT_TIME = 60;

	for(loop=0;loop < LOCK_OUT_TIME; loop++)
	{
		/* check counter */
		drv_demod_i2c_Read(unitId, DEMOD_10048_REG_SCAN_CPT, &testout);
		CH_DI_Print(3, ("[check_ofdm_lock]  UnitId [%d] -- TESTOUT = %x \n", unitId,testout));

		if( testout >= 0x10 )
			return OFDM_UNLOCKED; /* 0x20 -> maximum threshold */

		/* check Front End Lock( FEL) */
		drv_demod_i2c_Read(unitId, DEMOD_10048_REG_SYNC_STATUS, &ival);

		/*mask of the sync register */
		ival &= 0x0f ;
		if( ival == 0x0f )
		{
			/* 0x0f : FEL Lock, Freq_Lock, Time_Lock, TPS_Lock */
			break;
		}
		else
		{
			VK_TASK_Sleep(WAIT_LOCK_TOTAL);
		}
	}

    /* stable Lock state 인지 체크 */
	for(loop_fec=loop;loop_fec< LOCK_OUT_TIME; loop_fec++)
	{
		if(check_fec_lock(unitId)==TRUE)
			return OFDM_LOCKED;
		else
			VK_TASK_Sleep(WAIT_LOCK_TOTAL);
	}
	return OFDM_UNLOCKED;
}


/********************************************************************************
*	Function	:  set_TPS_value
*	Description :  	nitialise channel parameters
*	Input		:
*	Return	:
********************************************************************************/

static void set_TPS_valueT(HUINT8 unitId, CH_TER_TuneParam_t * pstTerTuneParam)
{
	HUINT8	ival;
	HUINT8 		ucTdaModulation, ucTdaGuardInterval, ucTdaTransMode, ucTdaHierarchy, ucTdaCodeRate;
	/* Default value
         Bit 7 : 1   AUTO4K  '0' -> 4K 는 찾지 않음
		Bit 6 : 1 	START_LOCK, Reset DSP process
		Bit 5 : 0	ZAP/SCAN	Scan mode
		Bit 4 : 1	AUTOOFFSET mode, auto detect the offset */
	ival = 0x50;
	/* Offset configuration always auto-offset  */
	if (pstTerTuneParam->etStream == DI_CH_STREAM_LP ) 					ival = ival | 0x08; /* in case LP stream  */
	if (pstTerTuneParam->etTransMode == DI_CH_TRANSMODE_AUTO ) 			ival = ival | 0x04; /* in case AUTO2k8k */
	if (pstTerTuneParam->etGuardInterval == DI_CH_GUARDINTERVAL_AUTO )  	ival = ival | 0x02; /* in case AUTOGI */

	/* followings are AUTOTPS case */
	if ( pstTerTuneParam->etHierarchy == DI_CH_HIERARCHY_AUTO ) 			ival = ival | 0x01;
	if ( pstTerTuneParam->etModulation == DI_CH_TMOD_AUTO ) 				ival = ival | 0x01;
	if ( pstTerTuneParam->etCodeRate == DI_CH_TCODERATE_AUTO ) 			ival = ival | 0x01;

	drv_demod_i2c_Write(unitId, DEMOD_10048_REG_AUTO, ival);
	CH_DI_Print(5, ("[set_TPS_value] Register AUTO(0x18) = 0x%02x\n",ival));

	ival = 0x70; /* OFFSET set to 111 , +/- 3/6 MHz searching */
	if ( pstTerTuneParam->etTransMode != DI_CH_TRANSMODE_AUTO )
	{
		ucTdaTransMode = (HUINT8)ConvertTdaFftSize( pstTerTuneParam->etTransMode );
		ival = ival | ucTdaTransMode ;
	}
	if ( pstTerTuneParam->etGuardInterval != DI_CH_GUARDINTERVAL_AUTO )
	{
		ucTdaGuardInterval = (HUINT8)ConvertTdaGuardInterval( pstTerTuneParam->etGuardInterval );
		ival = ival | (ucTdaGuardInterval << 2) ;
	}

	drv_demod_i2c_Write(unitId, DEMOD_10048_REG_IN_CONF1, ival);
	CH_DI_Print(5, ("[set_TPS_value] Register IN CONF1 = 0x%02x\n",ival));

	ival = 0x00;
	if ( pstTerTuneParam->etHierarchy != DI_CH_HIERARCHY_AUTO )
	{
		ucTdaHierarchy = (HUINT8)ConvertTdaHierarchy( pstTerTuneParam->etHierarchy );
		ival = (ucTdaHierarchy << 3) | ival;
	}

	if ( pstTerTuneParam->etModulation != DI_CH_TMOD_AUTO )
	{
		ucTdaModulation = (HUINT8)ConvertTdaModulation( pstTerTuneParam->etModulation );
		ival = (ucTdaModulation << 5) | ival;
	}

	drv_demod_i2c_Write(unitId, DEMOD_10048_REG_IN_CONF2, ival);

	if ( pstTerTuneParam->etStream == DI_CH_STREAM_HP )
	{
		drv_demod_i2c_Read(unitId, DEMOD_10048_REG_IN_CONF2, &ival);
		if ( pstTerTuneParam->etCodeRate != DI_CH_TCODERATE_AUTO )
		{
			ival &= 0xF8;
			ucTdaCodeRate = (HUINT8)ConvertTdaCodeRate( pstTerTuneParam->etCodeRate );
			ival = ival | ucTdaCodeRate;
			drv_demod_i2c_Write(unitId, DEMOD_10048_REG_IN_CONF2, ival);
		}
	}
	CH_DI_Print(5, ("[set_TPS_value] Register IN_CONF2 = 0x%02x\n",ival));

	if ( pstTerTuneParam->etStream == DI_CH_STREAM_LP )
	{
		drv_demod_i2c_Read(unitId, DEMOD_10048_REG_IN_CONF3, &ival);
		if ( pstTerTuneParam->etCodeRate != DI_CH_TCODERATE_AUTO )
		{
           	 	ival &= 0xF8;
			ucTdaCodeRate = (HUINT8)ConvertTdaCodeRate( pstTerTuneParam->etCodeRate );
			ival = ival | ucTdaCodeRate;
			drv_demod_i2c_Write(unitId, DEMOD_10048_REG_IN_CONF3, ival);
			CH_DI_Print(5, ("[set_TPS_value] Register IN_CONF3 = 0x%02x\n",ival));
		}
	}
}


/********************************************************************************
*	Function	:  AddOffsetFreq
*	Description :
*	Input		:
*	Return	:
********************************************************************************/

static HUINT32 AddOffsetFreq ( int nOffsetIndex, HUINT32 ulFrequency)
{

	if ( nOffsetIndex == DI_CH_OFFSET_1_6_P ) ulFrequency += 166666 ;
	else if ( nOffsetIndex == DI_CH_OFFSET_1_6_N ) ulFrequency -= 166666 ;
	else if ( nOffsetIndex == DI_CH_OFFSET_2_6_P ) ulFrequency += 166666 * 2;
	else if ( nOffsetIndex == DI_CH_OFFSET_2_6_N ) ulFrequency -= 166666 * 2;
	else if ( nOffsetIndex == DI_CH_OFFSET_3_6_P ) ulFrequency += 166666 * 3;
	else if ( nOffsetIndex == DI_CH_OFFSET_3_6_N ) ulFrequency -= 166666 * 3;

	return ulFrequency;
}


/********************************************************************************
*   Function    :  SetIFFrequency
*   Description :
*   Input       :
*   Return  :
********************************************************************************/
static int SetIFFrequency(int nUnitId,  HUINT32  uIF )
{
    DI_CHANNEL_ERR      err = DI_CH_OK;
    HUINT8               puByte[2] = {0,0};
    HUINT32              uByte = 0;
    HFLOAT32             dValue = 0.0;
    HUINT32				uSampClk = 0;

    uSampClk  = TMBSLTDA10048_CFG_XTALL * (TMBSLTDA10048_CFG_PLLMFACTOR + 45);
    uSampClk /= TMBSLTDA10048_CFG_PLLNFACTOR + 1;
    uSampClk /= TMBSLTDA10048_CFG_PLLPFACTOR + 4;

    CH_DI_Print(5, ("SetIFFrequency[%d], IF: [%d] , uSampClk = (%d)\n", nUnitId, uIF, uSampClk));

    if (uIF  > uSampClk/2) /* logically it's pObj->uSampClk/2 + BW/2) */
    {
        dValue = ((HFLOAT32)uIF - (HFLOAT32)uSampClk)/(HFLOAT32)uSampClk;
        dValue *= 32768;

        CH_DI_Print(5, ("SetIFFrequency[%d], dValue = (%f)\n", nUnitId, dValue));

        uByte = (HINT16)(((dValue*100)-50)/100); /* rounding to the next integer*/

        CH_DI_Print(5, ("SetIFFrequency[%d], uIF=(%d)>uSampClk, uByte = (0x%x)\n", nUnitId, uIF, uByte));

        puByte[1] = (HUINT8)(uByte >> 8);
        puByte[0] = (HUINT8)(uByte);

        drv_demod_i2c_Write(nUnitId, DEMOD_10048_REG_FREQ_PHY2_MSB, puByte[1]);
        drv_demod_i2c_Write(nUnitId, DEMOD_10048_REG_FREQ_PHY2_LSB, puByte[0]);

    }
    else if (uIF  < uSampClk/2) /* logically it's pObj->uSampClk/2 - BW/2) */
    {
        dValue = (HFLOAT32)((HINT32)uIF);
        dValue *= 32768;
        dValue /= (HFLOAT32)uSampClk;
        uByte = (HUINT32)(((dValue*100)-50)/100); /* rounding to the next integer*/

        CH_DI_Print(5, ("SetIFFrequency[%d], uIF=(%d)<uSampClk, uByte = (0x%x)\n", nUnitId, uIF, uByte));

        puByte[1] = (HUINT8)(uByte >> 8);
        puByte[0] = (HUINT8)uByte;

        drv_demod_i2c_Write(nUnitId, DEMOD_10048_REG_FREQ_PHY2_MSB, puByte[1]);
        drv_demod_i2c_Write(nUnitId, DEMOD_10048_REG_FREQ_PHY2_LSB, puByte[0]);
     }

    CH_DI_Print(5, ("SetIFFrequency[%d], puByte[0]=(0x%x):(0x%x)=puByte[1]\n", nUnitId, puByte[0], puByte[1]));

    return err;
}


/********************************************************************************
*   Function    :  DRV_CH_ConfigTunerPll
*   Description :   Writing the tuning frequency into tuner & Program the tuner's PLL registers
*   Input       :
*   Return  :
********************************************************************************/

void DRV_CH_ConfigTunerPll( int nChannelId, HUINT32 ulFreqHz, HUINT8 FreqOffset, HUINT8 ucBW )
{
    HUINT8 unitId;
    HUINT32 nfreq = 0;
	int nBandWidth = 0;
#if defined(CONFIG_PLATFORM_TN18275)
	HUINT32 ulIfFreq = 0;
#endif
    unitId = (HUINT8)GetDeviceUnitId( nChannelId );

    CH_DI_Print(5, ("DRV_CH_ConfigTunerPll][%d],  FreqHz(%d), BW(%d) \n", unitId, ulFreqHz, ucBW));

    nfreq = AddOffsetFreq(FreqOffset, ulFreqHz);

#if !defined(CONFIG_PLATFORM_TN18275)
    if( ucBW == DI_CH_BW_8MHZ )
    {
    	SetIFFrequency(unitId, 4000000); /* IF freq = 4MHz */
        TDA18273_SetTune(unitId, nfreq, 8);
    }
    else if( ucBW == DI_CH_BW_7MHZ )
    {
        SetIFFrequency(unitId, 3500000); /* IF freq = 3.5MHz */
        TDA18273_SetTune(unitId, nfreq, 7);
    }
    else
    {
		SetIFFrequency(unitId, 4000000); /* IF freq = 4MHz */
        TDA18273_SetTune(unitId, nfreq, 8);
    }
#else
    if( ucBW == DI_CH_BW_7MHZ )
    {
		nBandWidth = 7;
    }
    else
    {
		nBandWidth = 8;
    }

	ulIfFreq = TDA18275_GetIfFreq( unitId, nfreq, nBandWidth);
	if(ulIfFreq == 0)
	{
		CH_DI_Print(0, ("Error : [DRV_CH_ConfigTunerPll][%d], IF freq is invalid... \n", unitId));
	}
	else
	{
		SetIFFrequency(unitId, ulIfFreq);
	}

	TDA18273_SetTune(unitId, nfreq, nBandWidth);
#endif

    return;
}

/********************************************************************************
*   Function    :  core_reset
*   Description :   The DSP could be reset to start its application software.
*   Input       :
*   Return  :
********************************************************************************/
static void core_reset(unsigned char unitId)/*bwwhang070502  bit doesn’t comes back to “1”automatically */
{
    unsigned char newval;

    drv_demod_i2c_Read(unitId, DEMOD_10048_REG_CONF_TRISTATE1, &newval);
    newval = newval & 0xfe; /* D0[CLR_I2C_N]: '0':reset active, '1': reset NOT active */
    drv_demod_i2c_Write(unitId, DEMOD_10048_REG_AGC_CONF, newval);
    newval = newval | 0x01;
    drv_demod_i2c_Write(unitId, DEMOD_10048_REG_AGC_CONF, newval);
}


/********************************************************************************
*   Function    :  drv_channel_Initialise
*   Description :   reset demod chip & DSP code download & common parameter setting
*   Input       :
*   Return  :
********************************************************************************/

static void drv_channel_Initialise(HUINT8 unitId)
{
	HUINT8 val;
	int timeout = 0;

	do  	/* NCO PLL parameter setting & Loading DSP code util it is O.K. */
	{
		timeout++;
		if (timeout >= 10)
		{
			CH_DI_Print(5, ("drv_channel_Initialise(): Cannot load DSP code for %d loop\n", timeout));
			break;
		}
        /* Initialise HW */
    	ResetHwTda10048(unitId);
        /* Reset ADC */
        drv_ADC_Reset(unitId);
		set_MNP_PLL(unitId);
		set_NCO_PLL_parameterT(unitId, DI_CH_BW_8MHZ);/* TODO_ILEE */
		val = drv_channel_loadDSPcode(unitId);
	} while(val != TRUE);
}


/********************************************************************************
*   Function    :  GetCellId
*   Description :
*   Input       :
*   Return  :
********************************************************************************/

static int GetCellId(int nUnitId, HUINT16 *pusCellId)
{
    HUINT8 ucCellIdMsb = 0;
    HUINT8 ucCellIdLsb = 0;
    HUINT16 ustmpCellId = 0;

    drv_demod_i2c_Read(nUnitId, DEMOD_10048_REG_CELL_ID_MSB, &ucCellIdMsb);
    drv_demod_i2c_Read(nUnitId, DEMOD_10048_REG_CELL_ID_LSB, &ucCellIdLsb);

    ustmpCellId = ucCellIdMsb<<8|ucCellIdLsb;
    *pusCellId = ustmpCellId;
    CH_DI_Print(4, ("&&&&&&&&& Cell Id (0x%x)\n",*pusCellId));

    return DI_CH_OK;
}


/********************************************************************************
*   Function    :  TDA10048_LockInfo
*   Description :
*   Input       :
*   Return  :
********************************************************************************/

void 	TDA10048_LockInfo(int nChannelId)
{
	HUINT8 unitId;
	CH_TER_TuneParam_t *pstTerTuneParam;

	unitId = (HUINT8)GetDeviceUnitId( nChannelId );
	pstTerTuneParam = (CH_TER_TuneParam_t *)GetWorkingTuneInfo( nChannelId );


	CH_DI_Print(3, ("===================================================\n"));
	CH_DI_Print(3, ("   TER TUNE Unit [%d] Status(%s)\n", unitId, (DP10048_CheckLock(nChannelId, nChannelId)==TRUE)?"Lock":"Unlock"));
	CH_DI_Print(3, ("===================================================\n"));
	CH_DI_Print(3, ("	  Local Frequency : %d [KHz]\r\n", pstTerTuneParam->ulFrequency));
	CH_DI_Print(3, ("	  Band Width  :  "));
	switch(pstTerTuneParam->etBandWidth)
	{
		case DI_CH_BW_8MHZ:
			CH_DI_Print(3, ("8 [MHz]\r\n"));
			break;
		case DI_CH_BW_7MHZ:
			CH_DI_Print(3, ("7 [MHz]\r\n"));
			break;
		case DI_CH_BW_6MHZ:
			CH_DI_Print(3, ("6 [MHz]\r\n"));
			break;
		default:
			CH_DI_Print(3, ("%d [MHz]\r\n", pstTerTuneParam->etBandWidth));
			break;
	}

	CH_DI_Print(3, ("	  Modulation : "));
	switch(pstTerTuneParam->etModulation)
	{
		case DI_CH_TMOD_AUTO:
			CH_DI_Print(3, ("AUTO\r\n"));
			break;
		case DI_CH_TMOD_QPSK:
			CH_DI_Print(3, ("QPSK\r\n"));
			break;
		case DI_CH_TMOD_16QAM:
			CH_DI_Print(3, ("16QAM\r\n"));
			break;
		case DI_CH_TMOD_64QAM:
			CH_DI_Print(3, ("64QAM\r\n"));
			break;
		default:
			CH_DI_Print(3, ("256QAM\r\n"));
			break;
	}

	CH_DI_Print(3, ("	  Hierarchy : "));
	switch(pstTerTuneParam->etHierarchy)
	{
		case DI_CH_HIERARCHY_AUTO:
			CH_DI_Print(3, ("AUTO\r\n"));
			break;
		case DI_CH_HIERARCHY_NONE:
			CH_DI_Print(3, ("NONE\r\n"));
			break;
		case DI_CH_HIERARCHY_1:
			CH_DI_Print(3, ("1\r\n"));
			break;
		case DI_CH_HIERARCHY_2:
			CH_DI_Print(3, ("2\r\n"));
			break;
		case DI_CH_HIERARCHY_4:
			CH_DI_Print(3, ("4\r\n"));
			break;
	}

	CH_DI_Print(3, ("	  Code Rate : "));
	switch(pstTerTuneParam->etCodeRate)
	{
		case DI_CH_TCODERATE_AUTO:
			CH_DI_Print(3, ("AUTO\r\n"));
			break;
		case DI_CH_TCODERATE_1_2:
			CH_DI_Print(3, ("1/2\r\n"));
			break;
		case DI_CH_TCODERATE_2_3:
			CH_DI_Print(3, ("2/3\r\n"));
			break;
		case DI_CH_TCODERATE_3_4:  /* 3 */
			CH_DI_Print(3, ("3/4\r\n"));
			break;
		case DI_CH_TCODERATE_5_6:
			CH_DI_Print(3, ("5/6\r\n"));
			break;
		case DI_CH_TCODERATE_7_8:
			CH_DI_Print(3, ("7/8\r\n"));
			break;
		default:
			CH_DI_Print(3, ("Other coderate\r\n"));
	}


	CH_DI_Print(3, ("	  Transmode : "));
	switch(pstTerTuneParam->etTransMode)
	{
		case DI_CH_TRANSMODE_AUTO:
			CH_DI_Print(3, ("AUTO\r\n"));
			break;
		case DI_CH_TRANSMODE_2K:
			CH_DI_Print(3, ("2K\r\n"));
			break;
		case DI_CH_TRANSMODE_8K:
			CH_DI_Print(3, ("8K\r\n"));
			break;
		default:
			CH_DI_Print(3, ("Other Transmode\r\n"));
			break;
	}


	CH_DI_Print(3, ("	  Guard Interval : "));
	switch(pstTerTuneParam->etGuardInterval)
	{
		case DI_CH_GUARDINTERVAL_AUTO:
			CH_DI_Print(3, ("AUTO\r\n"));
			break;
		case DI_CH_GUARDINTERVAL_1_32:
			CH_DI_Print(3, ("1/32\r\n"));
			break;
		case DI_CH_GUARDINTERVAL_1_16:
			CH_DI_Print(3, ("1/16\r\n"));
			break;
		case DI_CH_GUARDINTERVAL_1_8:
			CH_DI_Print(3, ("1/8\r\n"));
			break;
		case DI_CH_GUARDINTERVAL_1_4:
			CH_DI_Print(3, ("1/4\r\n"));
			break;
		default:
			CH_DI_Print(3, ("Other GuardInterval\r\n"));
			break;
	}

	CH_DI_Print(3, ("	  Stream : "));
	switch(pstTerTuneParam->etStream)
	{
		case DI_CH_STREAM_HP:
			CH_DI_Print(3, ("HP\r\n"));
			break;
		case DI_CH_STREAM_LP:
			CH_DI_Print(3, ("LP\r\n"));
			break;
	}

	CH_DI_Print(3, ("	  Offset : "));
	switch(pstTerTuneParam->etOffset)
	{
		case DI_CH_OFFSET_AUTO:
			CH_DI_Print(3, ("AUTO\r\n"));
			break;
		case DI_CH_OFFSET_NONE:
			CH_DI_Print(3, ("NONE\r\n"));
			break;
		case DI_CH_OFFSET_1_6_P:
			CH_DI_Print(3, ("1.6P\r\n"));
			break;
		case DI_CH_OFFSET_1_6_N:
			CH_DI_Print(3, ("1.6N\r\n"));
			break;
		case DI_CH_OFFSET_2_6_P:
			CH_DI_Print(3, ("2.6P\r\n"));
			break;
		case DI_CH_OFFSET_2_6_N:
			CH_DI_Print(3, ("2.6N\r\n"));
			break;
		case DI_CH_OFFSET_3_6_P:
			CH_DI_Print(3, ("3.6P\r\n"));
			break;
		case DI_CH_OFFSET_3_6_N:
			CH_DI_Print(3, ("3.6N\r\n"));
			break;
 	}
#if 0
	CH_DI_Print(0, ("   UnErr \t[%ld]\n", ulUnErr));
	CH_DI_Print(0, ("   Corr \t[%ld]\n", ulCorr));
	CH_DI_Print(0, ("   UnCorr \t[%ld]\n", ulUnCorr));
	CH_DI_Print(0, ("   If Agc \t[%d]\n", ucIfAgc));
	CH_DI_Print(0, ("   Rf Agc \t[%d]\n", ulRfAgc));
	CH_DI_Print(0, ("   Signal Level \t[%d.%d]dBmV\n", lSignalLevel/10, abs(lSignalLevel%10)));
#endif
	CH_DI_Print(3, ("===================================================\n"));
}


/********************************************************************************
*   Function    :  TDA10048_CN_Info
*   Description :
*   Input       :
*   Return  :
********************************************************************************/

static HUINT8 TDA10048_CN_Info(HUINT8 unitId)
{
    HUINT8  ival,val;

    drv_demod_i2c_Read(unitId, DEMOD_10048_REG_NP_OUT, &ival);

    if(ival >= 228)     val = 0;
    else if(ival >= 181 )   val = 1;
    else if(ival >= 144 )   val = 2;
    else if(ival >= 114 )   val = 3;
    else if(ival >= 91  )   val = 4;
    else if(ival >= 72  )   val = 5;
    else if(ival >= 58  )   val = 6;
    else if(ival >= 46  )   val = 7;
    else if(ival >= 37  )   val = 8;
    else if(ival >= 29  )   val = 9;
    else if(ival >= 23  )   val = 10;
    else if(ival >= 19  )   val = 11;
    else if(ival >= 15  )   val = 12;
    else if(ival >= 12  )   val = 13;
    else if(ival >= 10  )   val = 14;
    else if(ival >= 8   )   val = 15;
    else if(ival >= 6   )   val = 16;
    else if(ival >= 5   )   val = 17;
    else if(ival >= 4   )   val = 18;
    else if(ival >= 3   )   val = 19;
    else if(ival >= 2   )   val = 21;
    else                    val = 24;

    CH_DI_Print(5, ("[CN_Info] UnitId [%d]C/N VALUE = %d dB \n",unitId,val));
    return val;

}


/********************************************************************************
*   Function    :  GetStrengthTda18273
*   Description :
*   Input       :
*   Return  :
********************************************************************************/
 static unsigned char GetStrengthTda18273(unsigned short ucReceivedDbuv)
 {
                                                     /*   0      1      2      3      4      5       6      7     8   */
                                                    /* 100    90    80     70    60     50     40     30   20  */
        unsigned short reflevel[9] = {6000,5750,5450,5250,4950,4400,3950,3450,3250};
        unsigned char   ucStrength = 0;

        if( ucReceivedDbuv >= reflevel[0] ) ucStrength = 100;
        else if( ucReceivedDbuv >= reflevel[1] ) ucStrength = 90 + ((ucReceivedDbuv-reflevel[1])*10)/(reflevel[0]-reflevel[1]);
        else if( ucReceivedDbuv >= reflevel[2] ) ucStrength = 80 + ((ucReceivedDbuv-reflevel[2])*10)/(reflevel[1]-reflevel[2]);
        else if( ucReceivedDbuv >= reflevel[3] ) ucStrength = 70 + ((ucReceivedDbuv-reflevel[3])*10)/(reflevel[2]-reflevel[3]);
        else if( ucReceivedDbuv >= reflevel[4] ) ucStrength = 60 + ((ucReceivedDbuv-reflevel[4])*10)/(reflevel[3]-reflevel[4]);
        else if( ucReceivedDbuv >= reflevel[5] ) ucStrength = 50 + ((ucReceivedDbuv-reflevel[5])*10)/(reflevel[4]-reflevel[5]);
        else if( ucReceivedDbuv >= reflevel[6] ) ucStrength = 40 + ((ucReceivedDbuv-reflevel[6])*10)/(reflevel[5]-reflevel[6]);
        else if( ucReceivedDbuv >= reflevel[7] ) ucStrength = 30 + ((ucReceivedDbuv-reflevel[7])*10)/(reflevel[6]-reflevel[7]);
        else if( ucReceivedDbuv >= reflevel[8] ) ucStrength = 20 + ((ucReceivedDbuv-reflevel[8])*10)/(reflevel[7]-reflevel[8]);
        else ucStrength = 10;

        CH_DI_Print(3, (">>>>>GetStrengthTda18273() = dDbuv(%d):(%d %%)Strength\n",ucReceivedDbuv,ucStrength));
        return ucStrength;
 }


/********************************************************************************
*   Function    :  TDA10048_GetAGCLevel
*   Description :
*   Input       :
*   Return  :
********************************************************************************/
HUINT8 TDA10048_GetAGCLevel(HUINT8 unitId, DI_CH_SignalQuality_t *pstSignalInfo)
{
	HUINT8	 uIfAGC, retAGC, strength = 0;
	int	dLevel, percent;

	/*
	Formula = RF AGC + IF AGC - BB GAIN + 70

	Real dBm	Strength(%)	Strength(ret)	RF_AGC	IF_AGC	BB_GAIN		Formula
	0			100			255 			151		194		1			415
	-5			100			255 			150		188		1			408
	-10			100			255 			149		179		1			398
	-15			100			255 			150		163		1			383
	-20			95			242 			149		143		1			362
	-25			90			230 			149		127		1			346
	-30			85			217 			149		114		1			333
	-35			80			204 			149		103		2			321
	-40			75			191 			149		96		1			315
	-45			70			179 			149		85		1			304
	-50			65			166 			149		46		1			265
	-55			60			153 			143		0		0			214
	-60			50			128 			134		0		1			204
	-65			40			102 			122		0		1			192
	-70			30			77 				105		0		1			175
	-75			25			64 				58		0		1			128
	-80			20			51 				0		0		17			54
	-85			15			26 				0		0		37			34
	-90			10->8		26 				0		0		55			16
	-95			10->8		26 				0		0		71			0

	*/
	/* using IF AGC only ( BB GAIN is not used ) optimized for FQD1116AME/BH

	Real dBm	Strength(%)	new value ( without rf agc )
	0			100
	-5			100
	-10			100			old value ( using rf agc )
	-15			100
	-20			100 		95
	-25			100 		90
	-30			100 		85
	-35			100 		80
	-40			100 		75
	-45			100 		70
	-50			100 		65
	-55			80  		60
	-60			60  		50
	-65			50  		40
	-70			40  		30
	-75			30			25
	-80			20  		20
	-85			10			15
	-90			8			8
	-95			8			8

	*/
	/* reference level for 100%,90%,80%.....10% */
	unsigned long reflevel[11] = {142,141,139,135,132,123,114,106,96,88,80};

   	/*----------------------
   	// Implementation
   	//---------------------- */
   	drv_demod_i2c_Read(unitId, DEMOD_10048_REG_AGC_IF_LEVEL, &uIfAGC);

	dLevel = (int)uIfAGC ;
	CH_DI_Print(5, ("[drv_channel_GetAGCLevel] uIfAGC = (%d) \n",uIfAGC));

	if( dLevel<0) dLevel = 0; /* modified by sykim */

	if ( dLevel > (int)reflevel[0] ) retAGC = 255;
	else if( dLevel > (int)reflevel[1] ) retAGC = (HUINT8)((dLevel - (int)reflevel[1]) * 25 / (reflevel[0] - reflevel[1]) + 230);
	else if( dLevel > (int)reflevel[2] ) retAGC = (HUINT8)((dLevel - (int)reflevel[2]) * 25 / (reflevel[1] - reflevel[2]) + 204);
	else if( dLevel > (int)reflevel[3] ) retAGC = (HUINT8)((dLevel - (int)reflevel[3]) * 25 / (reflevel[2] - reflevel[3]) + 179);
	else if( dLevel > (int)reflevel[4] ) retAGC = (HUINT8)((dLevel - (int)reflevel[4]) * 25 / (reflevel[3] - reflevel[4]) + 153);
	else if( dLevel > (int)reflevel[5] ) retAGC = (HUINT8)((dLevel - (int)reflevel[5]) * 25 / (reflevel[4] - reflevel[5]) + 128);
	else if( dLevel > (int)reflevel[6] ) retAGC = (HUINT8)((dLevel - (int)reflevel[6]) * 25 / (reflevel[5] - reflevel[6]) + 102);
	else if( dLevel > (int)reflevel[7] ) retAGC = (HUINT8)((dLevel - (int)reflevel[7]) * 25 / (reflevel[6] - reflevel[7]) + 77);
	else if( dLevel > (int)reflevel[8] ) retAGC = (HUINT8)((dLevel - (int)reflevel[8]) * 25 / (reflevel[7] - reflevel[8]) + 51);
	else if( dLevel > (int)reflevel[9] ) retAGC = (HUINT8)((dLevel - (int)reflevel[9]) * 25 / (reflevel[8] - reflevel[9]) + 38);
	else if( dLevel > (int)reflevel[10]) retAGC =(HUINT8)((dLevel - (int)reflevel[10]) * 25 / (reflevel[9] - reflevel[10]) + 21); /* 26->21 */
	else retAGC = 21;

	percent = (int) retAGC * 100 / 255 ;
	strength = (HUINT8)percent;

	CH_DI_Print(5, ("AGC VALUE = %d/255 ( %d %% )\n",retAGC,strength));

	pstSignalInfo->ulAgc = (HUINT32)retAGC;
	pstSignalInfo->ulStrength = (HUINT32)strength;
	pstSignalInfo->ulIfAgc = (HUINT32)uIfAGC;
	pstSignalInfo->ulRfAgc = 0;
	pstSignalInfo->fSignalInputPower = 0;

	return strength;
}


/********************************************************************************
*   Function    :  TDA10048_GetBER
*   Description :   Convert the real BER to IRD's signal quality factor.
*               BER after Viterbi decoding      Return Value
*               Unlock              0
*               1.0e-2 <= BER < 5.0e-2      1
*               7.5e-3 <= BER < 1.0e-2      2
*               5.0e-3 <= BER < 7.5e-3      3
*               2.5e-3 <= BER < 5.0e-3      4
*               1.0e-3 <= BER < 2.5e-3      5
*               7.5e-4 <= BER < 1.0e-3      6
*               5.0e-4 <= BER < 7.5e-4      7
*               2.5e-4 <= BER < 5.0e-4      8
*               1.0e-4 <= BER < 2.5e-4      9
*                     BER < 1.0e-4      10
*   Input       :
*   Return  :
********************************************************************************/
HUINT8 TDA10048_GetBER(HUINT8 unitId, DI_CH_SignalQuality_t *pstSignalInfo)
{
	HUINT8	ival,jval,quality;
	unsigned long	ber;
#ifdef AGC_BER_DEBUG_ON
	unsigned long	integer,remainder;
#endif

/* PVBER is set to 10 : that means 10^6 bits computation
	so the multiplicant is 10^-6  */

	/* reset of the VBER counting procedure */
/*	ival = 0x2a; drv_demod_i2c_Write(unitId, DEMOD_10046_REG_CVBER_CTL, ival); */

/******************************************************
Calculation of the time for BER checking
1 symbol duration = 0.109375 us ( SPEC 744 Page 26 참조 )
10^6 symbol duration = 109.375 ms
10^6 bit duration = 10^6 symbol duration / ( BIT PER SYMBOL )
	BIT PER SYMBOL = 6 for 64-QAM	=> 18.2 ms
			 4 for 16-QAM	=> 27.3 ms
			 2 for QPSK	=> 54.6 ms
*******************************************************/
	drv_demod_i2c_Read(unitId, DEMOD_10048_REG_SOFT_IT_C3, &jval);
	if (!(jval & 0x02)){
		/* CH_DI_Print(5, ("BER IS NOT SET so give delay\n")); */
		VK_TASK_Sleep(122); /* maximum period = 121.36ms (GI=1/4, QPSK) */
		/* False Lock */
		drv_demod_i2c_Read(unitId, DEMOD_10048_REG_SOFT_IT_C3, &jval);
		if (!(jval & 0x02)){
			return 1;
	        }
	}

	drv_demod_i2c_Read(unitId, DEMOD_10048_REG_VBER_LSB, &ival);
	ber = ival ;

	drv_demod_i2c_Read(unitId, DEMOD_10048_REG_VBER_MID, &ival);
	ber = ber + ival * 256;

	drv_demod_i2c_Read(unitId, DEMOD_10048_REG_VBER_MSB, &ival);
	ival = ival & 0x0f;
	ber = ber + ival * 256 * 256 ;
	ber = ber * 999584 / 1000000 ;     /* correction */

	pstSignalInfo->fBer = (float)ber;

	if( ber >= 50000 )  		quality = 0;
	else if( ber >= 10000 ) 	quality = 10;
	else if( ber >= 7500 ) 		quality = 20;
	else if( ber >= 5000 ) 		quality = 30;
	else if( ber >= 2500 ) 		quality = 40;
	else if( ber >= 1000 ) 		quality = 50;
	else if( ber >= 750 ) 		quality = 60;
	else if( ber >= 500 ) 		quality = 70;
	else if( ber >= 250 ) 		quality = 80;
	else if( ber >= 100 ) 		quality = 90;   /* Threshold for QEF */
	else 						quality = 100;

	pstSignalInfo->ulQuality = (HUINT32)quality;

#ifdef AGC_BER_DEBUG_ON
	if(ber < 10) {
		CH_DI_Print(6, ("BER = %lde-6, %d\n", ber,quality));
	}
	else if (ber<100) {
		integer = ber / 10;
		remainder = ber - integer * 10;
		CH_DI_Print(6, ("BER = %ld.%lde-5, %d\n", integer,remainder,quality));
		}
	else if (ber<1000) {
		integer = ber / 100;
		remainder = ber - integer * 100;
		CH_DI_Print(6, ("BER = %ld.%lde-4, %d\n", integer,remainder,quality));
		}
	else if (ber<10000) {
		integer = ber / 1000;
		remainder = ber - integer * 1000;
		CH_DI_Print(6, ("BER = %ld.%lde-3, %d\n", integer,remainder,quality));
		}
	else {
		integer = ber / 10000;
		remainder = ber - integer * 10000;
		CH_DI_Print(6, ("BER = %ld.%lde-2, %d\n", integer,remainder,quality));
		}
#endif

	/* Now Calculating Uncorrected Packet */
	ival = 0x80;
	drv_demod_i2c_Write(unitId, DEMOD_10048_REG_UNCOR_CTRL, ival );
	VK_TASK_Sleep(122);
	drv_demod_i2c_Read(unitId, DEMOD_10048_REG_UNCOR_CPT_LSB, &ival );
	drv_demod_i2c_Read(unitId, DEMOD_10048_REG_UNCOR_CPT_MSB, &jval );
	ber = jval * 256 + ival ;

	pstSignalInfo->ulUnCorrectedNo = (HUINT32)ber;

#ifdef AGC_BER_DEBUG_ON
	CH_DI_Print(5, ("UNCORRECTED BLOCK = %ld \n", ber));
#endif

	return quality;
}


/********************************************************************************
*   Function    :  slow_lockT
*   Description :
*   Input       :
*   Return  :
********************************************************************************/

static HUINT8 slow_lockT( int nChannelId, HUINT32 unFreq, HUINT8 FreqOffset, HUINT8 ucBW  )
{
	HUINT8  lock_ofdm = 0, lock_fec = 0; /* 1: locked, 0: unlocked */
	HUINT8  count = 0;
	HUINT32   frequency;
	HUINT8 unitId;
	DI_CH_SignalQuality_t pstSignalInfo;

	count = 0;

 	lock_ofdm		= FALSE;
   	lock_fec			= FALSE;
	unitId = (HUINT8)GetDeviceUnitId( nChannelId );

	CH_DI_Print(5, ("[slow_lock][%d]------------ \n", nChannelId));

	TDA10048_CN_Info(unitId);
	TDA10048_GetAGCLevel(unitId, &pstSignalInfo);

	while (!(lock_ofdm && lock_fec))
	{
	    	++count;
		if(count > 1 )
		{
			frequency = unFreq;
			DRV_CH_ConfigTunerPll( nChannelId, frequency, FreqOffset,  ucBW);
		}

		soft_resetT(unitId);

		/* OFDM lock: TPS frame, CLK, Carrier and FFT window */
		lock_ofdm = check_ofdm_lock(unitId);

		if( lock_ofdm == OFDM_UNLOCKED )
		{
			CH_DI_Print(1, ("[UnitId [%d] OFDM UNLOCKED \n", unitId));
	   	}
		else if( lock_ofdm == OFDM_LOCKED )
		{
			CH_DI_Print(1, ("[UnitId [%d] OFDM & FEC LOCKED\n", unitId));
			lock_fec = TRUE;
			return OFDM_LOCKED;
		}
		else
		{
			/* OFDM Cancelled */
			CH_DI_Print(3, ("[UnitId [%d] slow_lock canceled \n", unitId));
			return OFDM_CHECK_CANCELED;
		}

		if (count == LOCK_TRIAL)
		{
			return FALSE;
		}

	} /*  end of while */

	return TRUE;
}


/********************************************************************************
*   Function    :  TDA10048_UpdateTunedParam
*   Description :
*   Input       :
*   Return  :
********************************************************************************/

static void TDA10048_UpdateTunedParam( int nChannelId, CH_TER_TuneParam_t *pstTuneParam )
{
    HUINT8   ival1, ival2, ival3;
    int nUnitId;
    HUINT8 		ucTdaModulation, ucTdaGuardInterval, ucTdaTransMode, ucTdaHierarchy;
    HUINT8 	ucTdaCodeRate;

    nUnitId = (HUINT8)GetDeviceUnitId( nChannelId );

    drv_demod_i2c_Read( nUnitId, DEMOD_10048_REG_OUT_CONF1, &ival1 );
    drv_demod_i2c_Read( nUnitId, DEMOD_10048_REG_OUT_CONF2, &ival2 );
    drv_demod_i2c_Read( nUnitId, DEMOD_10048_REG_OUT_CONF3, &ival3 );

    ucTdaModulation 	= (ival2 & 0x60)>>5; /*D[6:5]   ival = ival >> 2 */
    ucTdaGuardInterval 	= (ival1 & 0x0C)>>2; /* D[3:2]   ival = ival >> 2 */
    ucTdaTransMode 		= (ival1 & 0x03); /* D[1:0]    ival = ival >> 1 */
    ucTdaHierarchy 		= (ival2 & 0x18)>>3; /* D[4:3] */

	pstTuneParam->etModulation		= ConvertDiModulation((TDA10048_Moudulation_e) ucTdaModulation);
	pstTuneParam->etGuardInterval	= ConvertDiGuardInterval((TDA10048_GuardInterval_e) ucTdaGuardInterval);
	pstTuneParam->etTransMode		= ConvertDiFftSize((TDA10048_TransMode_e) ucTdaTransMode);
	pstTuneParam->etHierarchy		= ConvertDiHierarchy((TDA10048_Hierachy_e) ucTdaHierarchy);

#ifdef SUPPORT_MANUAL_CODERATE
    if( pstTuneParam->etStream == DI_CH_STREAM_HP )
    {
    	ucTdaCodeRate = (ival2 & 0x07);//D[2:0]
    	pstTuneParam->etCodeRate = ConvertDiCodeRate((TDA10048_CodeRate_e) ucTdaCodeRate);
    }
    else if( pstTuneParam->etStream == DI_CH_STREAM_LP )
    {
    	ucTdaCodeRate = (ival3 & 0x07);//D[2:0]
    	pstTuneParam->etCodeRate = ConvertDiCodeRate((TDA10048_CodeRate_e) ucTdaCodeRate);
    }
#else

    if( pstTuneParam->etStream == DI_CH_STREAM_HP )
    {
    	ucTdaCodeRate = (ival2 & 0x07);
    	s_etRealCodeRate = ConvertDiCodeRate((TDA10048_CodeRate_e) ucTdaCodeRate);
    }
    else if( pstTuneParam->etStream == DI_CH_STREAM_LP )
    {
    	ucTdaCodeRate = (ival3 & 0x07);
    	s_etRealCodeRate = ConvertDiCodeRate((TDA10048_CodeRate_e) ucTdaCodeRate);
    }

	pstTuneParam->etCodeRate = DI_CH_TCODERATE_AUTO;
#endif

}


/********************************************************************************
*   Function    :  TDA10048_GetRealCodeRate
*   Description :
*   Input       :
*   Return  :
********************************************************************************/

CH_TER_CodeRate_e TDA10048_GetRealCodeRate( void )
{
	return s_etRealCodeRate;
}


/********************************************************************************
* Public functions
********************************************************************************/

#define _______________________________________________________________________
#define ____CH_T_PUBLIC_FUNCITONS____



/********************************************************************************
*	Function	: DP10048_InitDevice
*	Description	:  initialize Terrestrial Front-end Devices.
*	Input		:	Device ID
*	Return		: DI_CH_OK on success, or other DI_CHANNEL_ERR on failure
********************************************************************************/

DI_CHANNEL_ERR DP10048_InitDevice( int nChannelId )
{
	int nResult = DI_CH_OK;
	HUINT8 unitId;
	HUINT32	ulI2CBus = I2C_CHANNEL_CHANNEL;

	CH_DI_Print(3, ("-----------TDA10048_Initialise[%d] -------------\n\n", nChannelId));

	unitId = (HUINT8)GetDeviceUnitId( nChannelId );
	GetChI2CBus(unitId, &ulI2CBus);

	CH_DI_Print(3, ("-----------ulI2CBus[%d] -------------\n\n", ulI2CBus));

	DRV_I2c_SetClock( ulI2CBus, DRV_I2C_CLOCK_400K );
	drv_channel_Initialise(unitId);

    /* change I2C speed after DSP code download */
    DRV_I2c_SetClock( ulI2CBus, DRV_I2C_CLOCK_100K );
	TDA18273_Initialize((int)unitId);
    SetIFFrequency(unitId, 4000000); /* default IF freq = 4MHz */
	GetOFDMChipID(unitId);
	config_demod_pio(unitId);

	return nResult;
}


/********************************************************************************
*   Function    :  DP10048_SetTune
*   Description :
*   Input       :
*   Return  :
********************************************************************************/

int DP10048_SetTune( int nChannelId, HUINT8 * bChecklock , HUINT32 unWorkingId )
{
	int 				nResult = DI_CH_OK;
	HUINT32		frequency;
	HUINT8		ival;
	HUINT8 		lock_ofdm = 0;
	HUINT8				unitId;
	CH_TER_TuneParam_t *pstTerTuneParam;
	HUINT8 				ucTdaOffset, ucDiOffset;

	CH_DI_Print(5, ("Unit ID[%d], [%s]  \n",nChannelId, __FUNCTION__));

	unitId = (HUINT8)GetDeviceUnitId( nChannelId );
	pstTerTuneParam = (CH_TER_TuneParam_t *)GetWorkingTuneInfo( nChannelId );
	/* PrintTerTuneParam( pstTerTuneParam ); */
	core_reset(unitId);
	set_NCO_PLL_parameterT( unitId , pstTerTuneParam->etBandWidth );
	set_demod_registersT( unitId, pstTerTuneParam );
	set_TPS_valueT( unitId, pstTerTuneParam );  /* in case CX22700, if autotps is 1, use "get TPS parameters" */

	s_bTuned = TRUE;

	/* Get the tuning data loaded from application to channel device */
	frequency = pstTerTuneParam->ulFrequency * KHZ_TO_HZ ;
	CH_DI_Print(5, ("unitId(%d), frequency(%dL)\n", unitId, frequency));

	/* set the tuner's PLL register */
	if( pstTerTuneParam->etOffset == DI_CH_OFFSET_AUTO )
	{
		CH_DI_Print(3, ("unitId(%d), DI_CH_OFFSET_AUTO!! \n", unitId ));
		pstTerTuneParam->etOffset = DI_CH_OFFSET_NONE;

		DRV_CH_ConfigTunerPll( unitId, frequency, pstTerTuneParam->etOffset, pstTerTuneParam->etBandWidth);
		lock_ofdm = slow_lockT( unitId, frequency, pstTerTuneParam->etOffset, pstTerTuneParam->etBandWidth);

		if( lock_ofdm == OFDM_LOCKED )
		{
			CH_DI_Print(1, ("OFDM_LOCKED [case:1]\n"));
		}
		else if(lock_ofdm == OFDM_UNLOCKED)
		{
			drv_demod_i2c_Read( unitId, DEMOD_10048_REG_OUT_CONF1, &ival );
			ucTdaOffset = (ival & 0x70) >> 4;
			ucDiOffset = ConvertDiOffset((TDA10048_Offset_e) ucTdaOffset);

			CH_DI_Print(3, ("ucTdaOffset(%d)", ucTdaOffset));
			if ( ucTdaOffset == TDA10048_OFFSET_NONE )
			{
				pstTerTuneParam->etOffset = DI_CH_OFFSET_AUTO;
			}
			else
			{
				pstTerTuneParam->etOffset = ucDiOffset;

				DRV_CH_ConfigTunerPll( unitId, frequency, pstTerTuneParam->etOffset, pstTerTuneParam->etBandWidth);
				lock_ofdm = slow_lockT( unitId, frequency, pstTerTuneParam->etOffset, pstTerTuneParam->etBandWidth);

				if (lock_ofdm == OFDM_LOCKED )
				{
					CH_DI_Print(3, ("OFDM_LOCKED [case:2]\n"));
				}
				else if(lock_ofdm == OFDM_UNLOCKED)
				{
					pstTerTuneParam->etOffset = DI_CH_OFFSET_AUTO;
				}
				else
				{
					CH_DI_Print(5, (" [UnitId [%d] <1>PRE_LOCKProcess() skipped because of new TP Setting \n", unitId));
				}
			}
		}
		else
		{
			/* OFDM Canceled  */
			CH_DI_Print(5, ("  [UnitId [%d] <2>PRE_LOCKProcess() skipped because of new TP Setting \n", unitId));
		}
	}
	else if ( pstTerTuneParam->etOffset != DI_CH_OFFSET_AUTO )
	{
		CH_DI_Print(5, ("unitId(%d), DI_CH_OFFSET(%d) \n", unitId, pstTerTuneParam->etOffset ));
		DRV_CH_ConfigTunerPll( unitId, frequency, pstTerTuneParam->etOffset, pstTerTuneParam->etBandWidth);
		lock_ofdm = slow_lockT( unitId, frequency, pstTerTuneParam->etOffset, pstTerTuneParam->etBandWidth);

		if( lock_ofdm == OFDM_LOCKED )
		{
			CH_DI_Print(3, ("OFDM_LOCKED [case:3]\n"));
		}
		else if( lock_ofdm == OFDM_UNLOCKED )
		{
			pstTerTuneParam->etOffset = DI_CH_OFFSET_AUTO;
		}
		else
		{
			/* OFDM_Canceled */
			CH_DI_Print(5, (" [UnitId [%d] <3>PRE_LOCKProcess() skipped because of new TP Setting \n", unitId));
		}
	}

	if( lock_ofdm == OFDM_LOCKED )
	{
		CH_DI_Print(1, (">>> TER CH_STATUS_LOCKED\n"));
		TDA10048_UpdateTunedParam( unitId, pstTerTuneParam );
		SetTuneStatus( unitId, CH_STATUS_LOCKED );
		DRV_CH_CallNotifyCallback( unitId, unWorkingId, DI_CH_SIGNAL_LOCKED );
	}
	else
	{
		CH_DI_Print(1, (">>> TER CH_STATUS_UNLOCK [%d] \n", unitId));
		SetTuneStatus( unitId, CH_STATUS_UNLOCK );
		DRV_CH_CallNotifyCallback( unitId, unWorkingId, DI_CH_SIGNAL_UNLOCK );
	}

	*bChecklock = lock_ofdm;

	return nResult;
}


/********************************************************************************
*   Function    :  DP10048_CheckLock
*   Description :
*   Input       :
*   Return  :
********************************************************************************/

HBOOL DP10048_CheckLock ( int nChannelId, HUINT32 unWorkingID )
{
	HUINT8 unitId;
	HUINT16 usCellId;
 	HUINT8 lock_ofdm = 0;

	CH_UNUSED(unWorkingID);

	unitId = (HUINT8)GetDeviceUnitId( nChannelId );

	CH_DI_Print(2, (" CheckLock : (%d), (%d)\n", nChannelId, unitId));

	/* OFDM lock: TPS frame, CLK, Carrier and FFT window */
	lock_ofdm = check_ofdm_lock(unitId);
	if( lock_ofdm == OFDM_LOCKED )
	{
		GetCellId( unitId, &usCellId);
		return TRUE;
	}
	else
	{
		return FALSE;
	}
}


/********************************************************************************
*   Function    :  DP10048_GetSignalInfo
*   Description :
*   Input       :
*   Return  :
********************************************************************************/

int   DP10048_GetSignalInfo(int nChannelId, DI_CH_SignalQuality_t *pstSignalInfo)
{
	HUINT8 unitId;
	HUINT8 ucStrength, ucQaulity;
	float fPower = 0.0;
	HUINT16 usReceivedDbuv = 0;

	unitId = (HUINT8)GetDeviceUnitId( nChannelId );

	TDA10048_LockInfo(unitId);

	pstSignalInfo->fSnr = (float)TDA10048_CN_Info(unitId);

	TDA18273_GetPowerLevel(unitId, &fPower);
	usReceivedDbuv = (HUINT16)(fPower*100);
	ucStrength = GetStrengthTda18273(usReceivedDbuv);
	pstSignalInfo->ulStrength = (HUINT32)ucStrength;

	ucQaulity = TDA10048_GetBER(unitId, pstSignalInfo);

	CH_DI_Print(4, ("DP10048_GetSignalInfo [%d] strength = %d, quality = %d \n", unitId, ucStrength , ucQaulity));
	return DI_CH_OK;
}


/********************************************************************************
*   Function    :  DP10048_SetPowerState
*   Description :
*   Input       :
*   Return  :
********************************************************************************/

int  DP10048_SetPowerState( int nChannelId, CH_Power_e etPower )
{
	HUINT8  uByte,uByte1,uByte2,uByte3;
	HUINT8  val,val1,val2,val3;
	HUINT8 	unitId;

	/* DRV_CH_Lock( nChannelId ); */

	unitId = (HUINT8)GetDeviceUnitId( nChannelId );
	/*----------------------*/
	/* Implementation*/
	/*----------------------*/
	switch (etPower)
	{
		case CH_POWER_OFF:  /* tmPowerStandby = 1 */
			uByte = 0x01;  /* TDA10048_CONFC4_ICSTDBY_BIT; */
			uByte1 = 0;
			uByte2 = 0x10;
			uByte3 = 0x06;

			SetActiveStatus( nChannelId, CH_ACTION_OFF );
			CH_DI_Print(5, ("\n -------------CH_T_SetPowerState standby mode -------------[%d] \n", unitId));
			break;

		case CH_POWER_ON: /* tmPowerOn = 0 */
	        default:
			uByte = 0;
			uByte1 =0x08; /* TDA10048_CONF_PLL1_AUTOPLL_BIT; */
			uByte2 = 0;
			uByte3 = 0;

			SetActiveStatus( nChannelId, CH_ACTION_ON );
			CH_DI_Print(5, ("\n -------------CH_T_SetPowerState Power on mode -------------[%d] \n", unitId) );
			break;
	}

	/* program the chip via I2c bus*/
  	val1 = uByte1 & 0x08;
	drv_demod_i2c_Write(unitId, DEMOD_10048_REG_CONF_PLL1, val1);

	/* ADC Power down */
	val2 = uByte2 & 0x10;
	drv_demod_i2c_Write(unitId, DEMOD_10048_REG_CONF_ADC2, val2);

	/* IC STDBY */
	 val = uByte & 0x01;
	drv_demod_i2c_Write(unitId, DEMOD_10048_REG_CONF_C4_1, val);

	/* 44h : Conf_tristate1 */
	/* [2:1] : Z_AGC_IF */
	val3 = uByte3 & 0x06;
	drv_demod_i2c_Write(unitId, DEMOD_10048_REG_CONF_TRISTATE1, val3);

	/* DRV_CH_UnLock( nChannelId ); */
	return DI_CH_OK;
}


/********************************************************************************
*   Function    :  DP10048_SetTsOutputMode
*   Description :
*   Input       :
*   Return  :
********************************************************************************/

int DP10048_SetTsOutputMode (int nChannelId, CH_TsOutMode_e OutputMode )
{
	int nResult = DI_CH_OK;
	HUINT8 ival;
	HUINT8 unitId;

	unitId = (HUINT8)GetDeviceUnitId( nChannelId );

	if( OutputMode == CH_TS_MODE_SERIAL )
	{
		/* IC functionality */
		CH_DI_Print(5, ("CH_TS_MODE_SERIAL\n"));
		/*MUX_CMD == 0100, Serial TS output */
		ival = 0x20;	drv_demod_i2c_Write( unitId, DEMOD_10048_REG_IC_MODE, ival );
		VK_TASK_Sleep(10);

/*  for change TS clock mode to continuous mode */
#if 0
		/* 0	SOCLK not present during redundancy
		// 0	PSYNC is active on all bit of the first byte
		// 0	MSB first
		// 1	Serial Stream
		// 0	PSYNC is HIGH
		// 0	UNCOR is HIGH for error
		// 0	DEN is HIGH
		// 1	rising edge for serial TS		*/
		ival = 0x11;	drv_demod_i2c_Write(UnitId, DEMOD_10048_REG_CONF_TS2, ival);
#else
		/* 1	SOCLK present during redundancy
		// 0	PSYNC is active on all bit of the first byte
		// 0	MSB first
		// 1	Serial Stream
		// 0	PSYNC is HIGH
		// 0	UNCOR is HIGH for error
		// 0	DEN is HIGH
		// 1	rising edge for serial TS		*/
		ival = 0x91;	drv_demod_i2c_Write(unitId, DEMOD_10048_REG_CONF_TS2, ival);
#endif
	}
	else if( OutputMode == CH_TS_MODE_PARALLEL )
	{
		CH_DI_Print(5, ("CH_TS_MODE_PARALLEL\n"));
		/*MUX_CMD == 0000, Parallel TS output */
		ival = 0x00;	drv_demod_i2c_Write(unitId, DEMOD_10048_REG_IC_MODE, ival);
		VK_TASK_Sleep(10);

#if defined(MPEG2_TS_CLK_CONTINUOUS)
        /* Conf_TS2 register (address E0h) */
        /* [7] REDUND_CLK = 1:continuous, =0:gapped  */
		ival = 0xc1;
#elif defined(MPEG2_TS_CLK_GAPPED)
		ival = 0x01;
#else
		ival = 0xc4;
#endif
		drv_demod_i2c_Write(unitId, DEMOD_10048_REG_CONF_TS2, ival); /* set PUNCOR = 1 for CI model */
	}
	else
	{
		CH_DI_Print(0, (" "));
		nResult = DI_CH_PARAM_ERR;
	}

	return nResult;

}



/********************************************************************************
*   Function    :  DP10048_EnableTsOutput
*   Description :
*   Input       :
*   Return  :
********************************************************************************/

int DP10048_EnableTsOutput ( int nChannelId )
{
	int nResult = DI_CH_OK;
	HUINT8 unitId, val;

	unitId = (HUINT8)GetDeviceUnitId( nChannelId );

	drv_demod_i2c_Read(unitId, DEMOD_10048_REG_CONF_TRISTATE1, &val);

	/* 44h : Conf_tristate1 */
	/* [6] : Z_TS */
	val = val & 0xbf;
	CH_DI_Print(5, (" TS Enable: CONF_TRISTATE1 = 0x%02x \n", val ));
	drv_demod_i2c_Write(unitId, DEMOD_10048_REG_CONF_TRISTATE1, val);
	SetTsStatus( nChannelId, CH_TS_PATH_ON );

	return nResult;
}


/********************************************************************************
*   Function    :  DP10048_EnableTsOutput
*   Description :
*   Input       :
*   Return  :
********************************************************************************/

int DP10048_DisableTsOutput ( int nChannelId )
{
	int nResult = DI_CH_OK;
	HUINT8 unitId, val;

	unitId = (HUINT8)GetDeviceUnitId( nChannelId );

	drv_demod_i2c_Read(unitId, DEMOD_10048_REG_CONF_TRISTATE1, &val);

	/* 44h : Conf_tristate1 */
	/* [6] : Z_TS */
	val = val |0x40;
	CH_DI_Print(5, (" TS Disable: CONF_TRISTATE1 = 0x%02x \n", val ));
	drv_demod_i2c_Write(unitId, DEMOD_10048_REG_CONF_TRISTATE1, val);
	SetTsStatus( nChannelId, CH_TS_PATH_OFF );

    return nResult;
	}


/********************************************************************************
*   Function    :  DP10048_GetAntState
*   Description :
*   Input       :
*   Return  :
********************************************************************************/

CH_TER_AntStatus_e DP10048_GetAntState( int nChannelId )
{
	int nUnitId;

	nUnitId = GetDeviceUnitId( nChannelId );

	return s_stAntState[nUnitId].etCurrentAntState;

}


/********************************************************************************
*   Function    :  DP10048_SetAntState
*   Description :
*   Input       :
*   Return  :
********************************************************************************/

int DP10048_SetAntState( int nChannelId, CH_TER_AntStatus_e etAntState )
{
	int nUnitId;
	int nRet=0;
	HUINT32 unWorkingId;
	int nActiveState;
	HUINT8 ucVal = 0;
	CH_Active_e	etChannelAction;

	nUnitId = GetDeviceUnitId( nChannelId );
	unWorkingId = GetRequestedId( nChannelId );

	s_stAntState[nUnitId].etDesiredAntState = etAntState;

	etChannelAction = GetActiveStatus(nChannelId);

	if( etChannelAction == CH_ACTION_OFF )
	{
		DP10048_SetPowerState( nChannelId, CH_POWER_ON);  /* demod pwr */
	}

	if(s_bTuned == FALSE)
	{
		drv_demod_i2c_Read(nUnitId, DEMOD_10048_REG_CONF_TRISTATE1, &ucVal);
		ucVal |= 0x1;	drv_demod_i2c_Write(nUnitId, DEMOD_10048_REG_CONF_TRISTATE1, ucVal);
	}

	if( s_stAntState[nUnitId].etCurrentAntState != DI_CH_ANT_POWER_FAULT )
	{
		if(etAntState < DI_CH_ANT_POWER_FAULT)
		{
			if((etAntState != DI_CH_ANT_POWER_OFF) && s_stAntState[nUnitId].etCurrentAntState != DI_CH_ANT_POWER_ON)
			{
				CH_DI_Print(5, ("DP10048_SetAntState ON (%d)\n", nUnitId));

				/* Power On */
                /* Menu Power ON 시 ANT_PW_EN: LOW,     ANT_EN : 한번만 3ms 동안 High 유지  */
				drv_demod_i2c_Write(nUnitId, DEMOD_10048_REG_GPIO_SP_DS2, 0xff );
				VK_TASK_Sleep(2);
				drv_demod_i2c_Write(nUnitId, DEMOD_10048_REG_GPIO_SP_DS1, 0xff );
				VK_TASK_Sleep(1);
				drv_demod_i2c_Write(nUnitId, DEMOD_10048_REG_GPIO_SP_DS1, 0x00 );
			}
			else
			{
				CH_DI_Print(5, ("DP10048_SetAntState OFF (%d)\n", nUnitId));
				/* Power Off */
                /* Menu Power Off 시 ANT_PW_EN: High  ,  ANT_EN : Low */
				drv_demod_i2c_Write(nUnitId, DEMOD_10048_REG_GPIO_SP_DS2, 0x00 );
				drv_demod_i2c_Write(nUnitId, DEMOD_10048_REG_GPIO_SP_DS1, 0x00 );
			}
			if( nRet < 0 )
			{
				CH_DI_Print(0, ("DP10048_SetAntState Write fail\n"));
				return DI_CH_ERR;
			}
		}
		else
		{
			CH_DI_Print(0, ("DP10048_SetAntState Unknown State\n"));
		}
		s_stAntState[nUnitId].etCurrentAntState = etAntState;

	}
	else if( s_stAntState[nUnitId].etCurrentAntState == DI_CH_ANT_POWER_FAULT )
	{
		if( etAntState == DI_CH_ANT_POWER_OFF )
		{
			/* Power Off */
            /* Menu Power Off 시 ANT_PW_EN: High  ,  ANT_EN : Low */
			drv_demod_i2c_Write(nUnitId, DEMOD_10048_REG_GPIO_SP_DS2, 0x00 );
			drv_demod_i2c_Write(nUnitId, DEMOD_10048_REG_GPIO_SP_DS1, 0x00 );

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
			/* Do nothing */
			}
		}
		else
		{
			return DI_CH_HW_RESTRICTION;
		}
	}

	return DI_CH_OK;

}


/********************************************************************************
*   Function    :  DP10048_CheckAntState
*   Description :
*   Input       :
*   Return  :
********************************************************************************/

static int DP10048_CheckAntState( int nChannelId )
{
	int nUnitId;
	HUINT8 ucReadVal = 0, ucTmpVal = 0;

	nUnitId = GetDeviceUnitId( nChannelId );

	if( nUnitId != M_DEMOD )
	{
		return DI_CH_HW_RESTRICTION;
	}

	if( s_stAntState[nUnitId].etDesiredAntState == DI_CH_ANT_POWER_ON )
	{
		drv_demod_i2c_Read(nUnitId, DEMOD_10048_REG_GPIO_SELECT, &ucReadVal );
		CH_DI_Print(5, (">>>>>>>>>>>>DP10048_CheckAntState :: nUnitId (%d):(0x%x) ucReadVal \n", nUnitId, ucReadVal));
		ucTmpVal = ucReadVal & 0x40;

		if( ucTmpVal == 0 )  /* active low */
		{
			CH_DI_Print(0, (">>>>>>>>>>>>Antenna short circuit \n"));
			s_stAntState[nUnitId].etCurrentAntState = DI_CH_ANT_POWER_FAULT;
			return DI_CH_ANT_POWER_FAULT;
		}
		else
		{
			if( s_stAntState[nUnitId].etCurrentAntState == DI_CH_ANT_POWER_FAULT )
			{
				CH_DI_Print(0, ("DP10048_CheckAntState CHK(HIGH), Curr(FAULT)\n"));
				/* Power On */
				drv_demod_i2c_Write(nUnitId, DEMOD_10048_REG_GPIO_SP_DS2, 0xff );
				VK_TASK_Sleep(2);
				drv_demod_i2c_Write(nUnitId, DEMOD_10048_REG_GPIO_SP_DS1, 0xff );
				VK_TASK_Sleep(1);
				drv_demod_i2c_Write(nUnitId, DEMOD_10048_REG_GPIO_SP_DS1, 0x00 );
			}
			return DI_CH_OK;
		}
	}
	else /* DI_CH_ANT_POWER_OFF, DI_CH_ANT_POWER_FAULT */
	{
		return DI_CH_OK;
	}

}


/********************************************************************************
*   Function    :  DP10048_ProcessRecoverAntenna
*   Description :
*   Input       :
*   Return  :
********************************************************************************/

static void DP10048_ProcessRecoverAntenna( int nChannelId, HUINT32 unWorkingId )
{
	int nRet=0;
	int nUnitId;
	int nActiveState;
	HUINT8 ucReadVal = 0, ucTmpVal = 0;

	nUnitId = GetDeviceUnitId( nChannelId );

	CH_DI_Print(5, ("DP10048_ProcessRecoverAntenna\n"));
	/* nRet = DRV_GPIO_Write(GPIO_ID_ANT_EN, GPIO_HI); */
	drv_demod_i2c_Write(nUnitId, DEMOD_10048_REG_GPIO_SP_DS1, 0xff );
	/* VK_TASK_Sleep(1);  duty 4.58ms, there may be damage to Tuner and TR. so modifications to the minimum.*/
	VK_TASK_Sleep(0);
	/* nRet = DRV_GPIO_Write(GPIO_ID_ANT_EN, GPIO_LOW); */
	drv_demod_i2c_Write(nUnitId, DEMOD_10048_REG_GPIO_SP_DS1, 0x00 );
	VK_TASK_Sleep(100);// wait 100ms

	/* nRet = DRV_GPIO_Read(GPIO_ID_ANT_PWR_CHK, &gpioStat_t); */
	drv_demod_i2c_Read(nUnitId, DEMOD_10048_REG_GPIO_SELECT, &ucReadVal );

	ucTmpVal = ucReadVal & 0x40;

	if( ucTmpVal == 0 )
	{
		CH_DI_Print(5, ("Antenna continue fault... Turn 5 Volt Off\n"));
		/* Power Off */
		/* nRet = DRV_GPIO_Write(GPIO_ID_ANT_EN, GPIO_LOW); */
		drv_demod_i2c_Write(nUnitId, DEMOD_10048_REG_GPIO_SP_DS1, 0x00 );
		if( nRet < 0 )
		{
			CH_DI_Print(0, ("DP10048_ProcessRecoverAntenna Write LOW fail\n"));
		}
		DRV_CH_CallNotifyCallback( nChannelId, unWorkingId, DI_CH_SIGNAL_ANTENNA_FAULT);
	}
	else
	{
		CH_DI_Print(5, ("Antenna recovered...\n"));
		s_stAntState[nUnitId].etCurrentAntState = DI_CH_ANTENNA_ON;
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
			/* Do nothing */
		}
	}

	return;

}


/********************************************************************************
*   Function    :  DP10048_GetPlpCount
*   Description :   DVB-T2
*   Input       :
*   Return  :
********************************************************************************/

int DP10048_GetPlpCount( int nChannelId, HUINT8 * pucNumPlps )
{
	CH_DI_Print(0, (" TDA10048 isn't supported.. \n"));

	CH_UNUSED(nChannelId);
	CH_UNUSED(pucNumPlps);
	return DI_CH_OK;
}


/********************************************************************************
*   Function    :  DP10048_GetPlpInfo
*   Description :   DVB-T2
*   Input       :
*   Return  :
********************************************************************************/

int DP10048_GetPlpInfo( int nChannelId, HUINT8 * pucPlpIds, HUINT8 * pucNumPlps )
{
	CH_DI_Print(0, (" TDA10048 isn't supported.. \n"));

	CH_UNUSED(nChannelId);
	CH_UNUSED(pucPlpIds);
	CH_UNUSED(pucNumPlps);
	return DI_CH_OK;
}


/********************************************************************************
*   Function    :  DP10048_GetCellId
*   Description :   DVB-T2
*   Input       :
*   Return  :
********************************************************************************/

static int DP10048_GetCellId(int nChannelId, HUINT16 *pusCellId)
{
	int			nUnitId = 0;
	unsigned char ucCellIdMsb = 0;
	unsigned char ucCellIdLsb = 0;
	unsigned short usTmpCellId = 0;


	nUnitId = GetDeviceUnitId( nChannelId );

	drv_demod_i2c_Read(nUnitId, DEMOD_10048_REG_CELL_ID_MSB, &ucCellIdMsb);
	drv_demod_i2c_Read(nUnitId, DEMOD_10048_REG_CELL_ID_LSB, &ucCellIdLsb);

	usTmpCellId = ucCellIdMsb<<8|ucCellIdLsb;
	*pusCellId = usTmpCellId;

	CH_DI_Print(0, ("&&&&&&&&& unitId(%d) : Cell Id (0x%x)\n", nUnitId, *pusCellId));

	return DI_CH_OK;
}



/********************************************************************************
*	Function	: DP10048_ReInitDevice
*	Description	:  Reinitialize Terrestrial Front-end Devices.
*	Input		:	Device ID
*	Return		: DI_CH_OK on success, or other DI_CHANNEL_ERR on failure
********************************************************************************/

int DP10048_ReInitDevice( int nChannelId )
{
	int nResult = DI_CH_OK;
	HUINT8 unitId;
	HUINT32	ulI2CBus = I2C_CHANNEL_CHANNEL;

	CH_DI_Print(3, ("-----------DP10048_ReInitDevice[%d] -------------\n\n", nChannelId));

	unitId = (HUINT8)GetDeviceUnitId( nChannelId );
	GetChI2CBus(unitId, &ulI2CBus);

	CH_DI_Print(3, ("-----------ulI2CBus[%d] -------------\n\n", ulI2CBus));

	TDA18273_Close((int)unitId);

	DRV_I2c_SetClock( ulI2CBus, DRV_I2C_CLOCK_400K );
	drv_channel_Initialise(unitId);

    /* change I2C speed after DSP code download */
    DRV_I2c_SetClock( ulI2CBus, DRV_I2C_CLOCK_100K );

	TDA18273_Initialize((int)unitId);
    SetIFFrequency(unitId, 4000000); /* default IF freq = 4MHz */
	GetOFDMChipID(unitId);
	config_demod_pio(unitId);

	DP10048_SetTsOutputMode(nChannelId,CH_TS_MODE_SERIAL);

	return nResult;
}


int DP10048_UnInitDevice( int nChannelId )
{
    int nResult = DI_CH_OK;

    CH_DI_Print(6,("DP10048_UnInitDevice Enter ...\n"));

    /* TODO : to be implemented ... */
    CH_UNUSED(nChannelId);

    return nResult;
}



#if 0  /* ANT_USED_HW_GPIO */

int DP10048_SetAntState( int nChannelId, CH_TER_AntStatus_e etAntState )
{
	int nUnitId;
	int nRet=0;
	HUINT32 unWorkingId;
	int nActiveState;
	HUINT8 ucVal = 0;
	CH_Active_e	etChannelAction;

	nUnitId = GetDeviceUnitId( nChannelId );
	unWorkingId = GetRequestedId( nChannelId );

	s_stAntState[nUnitId].etDesiredAntState = etAntState;

	etChannelAction = GetActiveStatus(nChannelId);

	if( etChannelAction == CH_ACTION_OFF )
	{
		DP10048_SetPowerState( nChannelId, CH_POWER_ON);  // demod pwr
	}

	if(s_bTuned == FALSE)
	{
		drv_demod_i2c_Read(nUnitId, DEMOD_10048_REG_CONF_TRISTATE1, &ucVal);
		ucVal |= 0x1;	drv_demod_i2c_Write(nUnitId, DEMOD_10048_REG_CONF_TRISTATE1, ucVal);
	}

	if( s_stAntState[nUnitId].etCurrentAntState != DI_CH_ANT_POWER_FAULT )
	{
		if(etAntState < DI_CH_ANT_POWER_FAULT)
		{
			if((etAntState != DI_CH_ANT_POWER_OFF) && s_stAntState[nUnitId].etCurrentAntState != DI_CH_ANT_POWER_ON)
			{
				CH_DI_Print(0, ("DP10048_SetAntState ON (%d)\n", nUnitId));

				/* Power On */
                /* Menu Power ON 시 ANT_PW_EN: LOW,     ANT_EN : 한번만 3ms 동안 High 유지  */
				nRet = DRV_GPIO_Write(GPIO_ID_ANT_PWR_EN, GPIO_HI);
				VK_TASK_Sleep(2);
				nRet = DRV_GPIO_Write(GPIO_ID_ANT_EN, GPIO_HI);
				VK_TASK_Sleep(1);
				nRet = DRV_GPIO_Write(GPIO_ID_ANT_EN, GPIO_LOW);
			}
			else
			{
				CH_DI_Print(0, ("DP10048_SetAntState OFF (%d)\n", nUnitId));
				/* Power Off */
                 /* Menu Power Off 시 ANT_PW_EN: High  ,  ANT_EN : Low */
				nRet = DRV_GPIO_Write(GPIO_ID_ANT_PWR_EN, GPIO_LOW);
				nRet = DRV_GPIO_Write(GPIO_ID_ANT_EN, GPIO_LOW);
			}
			if( nRet < 0 )
			{
				CH_DI_Print(0, ("DP10048_SetAntState Write fail\n"));
				return DI_CH_ERR;
			}
		}
		else
		{
			CH_DI_Print(0, ("DP10048_SetAntState Unknown State\n"));
		}
		s_stAntState[nUnitId].etCurrentAntState = etAntState;

	}
	else if( s_stAntState[nUnitId].etCurrentAntState == DI_CH_ANT_POWER_FAULT )
	{

		if( etAntState == DI_CH_ANT_POWER_OFF )
		{
			/* Power Off */
            /* Menu Power Off 시 ANT_PW_EN: High  ,  ANT_EN : Low */
			nRet = DRV_GPIO_Write(GPIO_ID_ANT_PWR_EN, GPIO_LOW);
			nRet = DRV_GPIO_Write(GPIO_ID_ANT_EN, GPIO_LOW);

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


static int DP10048_CheckAntState( int nChannelId )
{
	int nUnitId;
	int nRet=0;
	GPIO_STATE_t	gpioStat_t = GPIO_INVAILD;
	HUINT8 ucReadVal = 0, ucTmpVal = 0;

	nUnitId = GetDeviceUnitId( nChannelId );

	if( nUnitId != M_DEMOD )
	{
		return DI_CH_HW_RESTRICTION;
	}

	if( s_stAntState[nUnitId].etDesiredAntState == DI_CH_ANT_POWER_ON )
	{
		if( ucTmpVal == 0 )
		nRet = DRV_GPIO_Read(GPIO_ID_ANT_PWR_CHK, &gpioStat_t);
		CH_DI_Print(0, (">>>>>>>>>>>>DP10048_CheckAntState :: nUnitId (%d):(0x%x) gpioStat_t \n", nUnitId, gpioStat_t));
		if( gpioStat_t == GPIO_LOW )
		{
			CH_DI_Print(0, (">>>>>>>>>>>>Antenna short circuit \n"));
			s_stAntState[nUnitId].etCurrentAntState = DI_CH_ANT_POWER_FAULT;
			return DI_CH_ANT_POWER_FAULT;
		}
		else
		{
			if( s_stAntState[nUnitId].etCurrentAntState == DI_CH_ANT_POWER_FAULT )
			{
				CH_DI_Print(0, ("DP10048_CheckAntState CHK(HIGH), Curr(FAULT)\n"));

				/* Power On */
				nRet = DRV_GPIO_Write(GPIO_ID_ANT_PWR_EN, GPIO_HI);
			}
			return DI_CH_OK;
		}
	}
	else // DI_CH_ANT_POWER_OFF, DI_CH_ANT_POWER_FAULT
	{
		return DI_CH_OK;
	}

}


static void DP10048_ProcessRecoverAntenna( int nChannelId, HUINT32 unWorkingId )
{
	int nRet=0;
	int nUnitId;
	int nActiveState;
	GPIO_STATE_t	gpioStat_t = GPIO_INVAILD;
	HUINT8 ucReadVal = 0, ucTmpVal = 0;

	nUnitId = GetDeviceUnitId( nChannelId );


	nRet = DRV_GPIO_Write(GPIO_ID_ANT_PWR_EN, GPIO_HI);
	if( nRet < 0 )
	{
		CH_DI_Print(0, ("DP10048_ProcessRecoverAntenna Write HI fail\n"));
	}

	VK_TASK_Sleep(100);// wait 100ms

	nRet = DRV_GPIO_Read(GPIO_ID_ANT_PWR_CHK, &gpioStat_t);
	if( nRet < 0 )
	{
		CH_DI_Print(0, ("DP10048_ProcessRecoverAntenna Read fail\n"));
		return;
	}

	if( gpioStat_t == GPIO_LOW )
	{
		CH_DI_Print(5, ("Antenna continue fault... Turn 5 Volt Off\n"));
		nRet = DRV_GPIO_Write(GPIO_ID_ANT_PWR_EN, GPIO_LOW);
		if( nRet < 0 )
		{
			CH_DI_Print(0, ("DP10048_ProcessRecoverAntenna Write LOW fail\n"));
		}

		DRV_CH_CallNotifyCallback( nChannelId, unWorkingId, DI_CH_SIGNAL_ANTENNA_FAULT);
		//s_stAntState[nUnitId].etCurrentAntState = DI_CH_ANT_POWER_OFF;

	}
	else if( gpioStat_t == GPIO_HI )
	{
		CH_DI_Print(5, ("Antenna recovered...\n"));

		s_stAntState[nUnitId].etCurrentAntState = DI_CH_ANT_POWER_ON;

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
		CH_DI_Print(0, ("DP10048_ProcessRecoverAntenna Read Unknown (%d)\n", gpioStat_t));
	}

	return;

}
#endif


/********************************************************************************
*	Function	: DRV_T_InstallApi
*	Description	:
*	Input		:	Device ID
*	Return		: DI_CH_OK on success, or other DI_CHANNEL_ERR on failure
********************************************************************************/

void DRV_T_InstallApi( void )
{
	TER_InitDevice			= &DP10048_InitDevice;
	TER_SetTune 			= &DP10048_SetTune;
	TER_CheckLock			= &DP10048_CheckLock;
	TER_GetSignalInfo		= &DP10048_GetSignalInfo;
	TER_SetPowerState		= &DP10048_SetPowerState;
	TER_SetTsOutputMode	= &DP10048_SetTsOutputMode;
	TER_EnableTsOutput		= &DP10048_EnableTsOutput;
	TER_DisableTsOutput 	= &DP10048_DisableTsOutput;
	TER_CheckAntState		= &DP10048_CheckAntState;
	TER_SetAntState			= &DP10048_SetAntState;
	TER_GetAntState			= &DP10048_GetAntState;
	TER_ProcessRecoverAntenna = &DP10048_ProcessRecoverAntenna;
	TER_GetDataPlpCount		= &DP10048_GetPlpCount;
	TER_GetDataPlpInfo		= &DP10048_GetPlpInfo;
	TER_GetCellId			= &DP10048_GetCellId;
	TER_ReInitDevice		= &DP10048_ReInitDevice;
    TER_UnInitDevice        = &DP10048_UnInitDevice;
       
	CH_DI_Print(5, ("[DP10048] DRV_T_InstallApi()  OK! \n"));

	return;
}



 /********************************************************************************
 *   Function    : DP10048_DumpRegister
 *   Description :  for debugging demod register.
 *   Input       :
 *   Return      :
 ********************************************************************************/

 int DP10048_DumpRegister( int nChannelId)
{
	int i=0, nUnitId;
	HUINT8 ucdata;

	nUnitId = GetDeviceUnitId( nChannelId );

	CH_DI_Print(0, ("===================================================\n"));
	CH_DI_Print(0, ("   TDA10048 Demod [%d] Dump\n", nChannelId));
	CH_DI_Print(0, ("===================================================\n"));

	for(i=0; i<= DEMOD_10048_REG_CONF_TS1; i++)
	{
		drv_demod_i2c_Read(nUnitId,  i, &ucdata);
		CH_DI_Print(0, ("[0x%02X] \t[0x%02x]\n", i, ucdata));
	}
	CH_DI_Print(0, ("===================================================\n"));

	return DI_CH_OK;

}

/** @} */


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
File Name   : drv_channel_tn18275.c
Original Author: J.D.CHO
Description : Channel driver
Remarks:
******************************************************************************/


/******************************************************************************
 *   Header Files
 ******************************************************************************/
#define _______________________________________________________________________
#define ____HEADER_FILES____


#if defined(CONFIG_VK_STDLIB)
#else
#include 	<stdio.h>
#endif
#include <stdarg.h>

#include "tmNxTypes.h"
#include "tmCompId.h"
#include "tmFrontEnd.h"
#include "tmbslFrontEndTypes.h"
#include "tmbslTDA18275.h"

#include "htype.h"
#include "di_channel.h"
#include "di_channel_priv.h"
#include "drv_i2c.h"

#if defined(CONFIG_PLATFORM_DP10048)
#include "drv_channel_dp10048.h"
#endif


/******************************************************************************
 *   Defines
 ******************************************************************************/
#define _______________________________________________________________________
#define ____MACRO_DEFINITION____

#define M_TDA18275_TUNER    0
#define S_TDA18275_TUNER    1

#if defined(CONFIG_PLATFORM_DS2820)||defined(CONFIG_PLATFORM_DS2834)
#define M_CXD2820_I2CADDR	(0xD8>>1)
#define S_CXD2820_I2CADDR	(0xDA>>1)
#define M_TDA18275_I2CADDR	(0xC0)  //To write I2C through CXD2820
#define S_TDA18275_I2CADDR	(0xC6)  //To write I2C through CXD2820
#define CONST_CXD2820_GWSUBADDR	(0x09)
#elif defined(CONFIG_PLATFORM_DP10048)
#define M_TDA18275_I2CADDR  (0xC0>>1)
#define S_TDA18275_I2CADDR  (0xC6>>1)
#endif

#define PRINT_BUFFER_SIZE (1040)
#define MAX_WRITE_BYTE      (255)
#define NUM_OF_REG          0x44

#define CONST_ADDR_LENGTH       (1)

/******************************************************************************
 *   Debug Option
 ******************************************************************************/
#define _______________________________________________________________________
#define ____TYPE_DEBUG_OPTION____

//#define DEBUG_TUNER_I2C

/******************************************************************************
 * Extern Variables
 ******************************************************************************/
#define _______________________________________________________________________
#define ____EXTERNAL_VARIABLE_DEFINITION____

extern HINT32 show_di_ch;

/********************************************************************************
* Static functions
********************************************************************************/
#define _______________________________________________________________________
#define ____CH_T_PRIVATE_FUNCITONS____


/********************************************************************************
*   Function    : UserWrittenI2CRead
*   Description :
*   Input       :   tmUnitSelect_t tUnit
*               UInt32 AddrSize,
*               UInt8* pAddr,
*               UInt32 ReadLen,
*               UInt8* pData
*   Return  :   tmErrorCode_t.
********************************************************************************/
#if defined(CONFIG_PLATFORM_DS2820)||defined(CONFIG_PLATFORM_DS2834)
tmErrorCode_t UserWrittenI2CRead(tmUnitSelect_t tUnit,	UInt32 AddrSize, UInt8* pAddr,
UInt32 ReadLen, UInt8* pData)
{
	/* Variable declarations */
	tmErrorCode_t err = TM_OK;
	int nRet = 0;
	HUINT32	ulI2cChannel = I2C_CHANNEL_CHANNEL;
	HUINT8	ucDemodAddr = M_CXD2820_I2CADDR;
	HUINT8	ucTunerAddr = M_TDA18275_I2CADDR;
	HUINT8 aucWriteData[2] = {0,0};
	HUINT16 usSubAddr = 0;

#if defined(DEBUG_TUNER_I2C)
	int i;
#endif

	if( pAddr == NULL )
	{
		return DI_CH_PARAM_ERR;
	}
	if( pData == NULL )
	{
		return DI_CH_PARAM_ERR;
	}

	if( AddrSize >= 2 || AddrSize == 0 )
	{
		CH_DI_Print(0, (">>> Error : UserWrittenI2CRead()... AddrSize[%d] \n", AddrSize));
		return DI_CH_PARAM_ERR;
	}

	if(tUnit == M_TDA18275_TUNER)
	{
		ulI2cChannel = I2C_CHANNEL_CHANNEL;
		ucDemodAddr = M_CXD2820_I2CADDR;
		ucTunerAddr = M_TDA18275_I2CADDR;
	}
	else if(tUnit == S_TDA18275_TUNER )
	{
#if defined(CONFIG_I2C_MAP_FOR_2TUNER)
		ulI2cChannel = I2C_CHANNEL_CHANNEL2;
#else
		ulI2cChannel = I2C_CHANNEL_CHANNEL;
#endif
		ucDemodAddr = S_CXD2820_I2CADDR;
		ucTunerAddr = S_TDA18275_I2CADDR;
	}
	else
	{
		ulI2cChannel = I2C_CHANNEL_CHANNEL;
		ucDemodAddr = M_CXD2820_I2CADDR;
		ucTunerAddr = M_TDA18275_I2CADDR;
	}

//////////////////////////////////////////////////////////////////////////////////////
	aucWriteData[0] = ucTunerAddr;//(CONST_TDA18275_I2CADDR);//(CONST_TDA18275_I2CADDR+1);
	aucWriteData[1] = *pAddr;//0x0;//*pAddr; //TDA18275 sub-address

	nRet = DRV_I2c_WriteA8( ulI2cChannel, ucDemodAddr, CONST_CXD2820_GWSUBADDR,\
							aucWriteData, 2 );
    if ( nRet != 0 )
    {
    	CH_DI_Print(0, (">>> Error : DRV_I2c_WriteA8()... in UserWrittenI2CWrite()... \n"));
        return TM_ERR_WRITE;
    }


	usSubAddr = CONST_CXD2820_GWSUBADDR<<8 | (ucTunerAddr + 1);//<<8 | *pAddr;   //??????

	nRet = DRV_I2c_ReadA16( ulI2cChannel, ucDemodAddr, usSubAddr, pData, ReadLen);//OOOOOOOO
    if ( nRet != 0 )
    {
    	CH_DI_Print(0, (">>> Error : DRV_I2c_ReadA16()... in UserWrittenI2CRead()... \n"));
        return TM_ERR_READ;
    }
	else
	{
#if defined(DEBUG_TUNER_I2C)
		CH_DI_Print(0, ("\n<<< Read : Sub-address, AddrSize[%d]\n", AddrSize));
		for(i=0; i<AddrSize; i++)
		{
			CH_DI_Print(0, ("pAddr[%d]=[0x%x]\n", i, pAddr[i]));
		}
		CH_DI_Print(0, ("<<< Read : Data, ReadLen[%d]\n", ReadLen));
		for(i=0; i<ReadLen; i++)
		{
			CH_DI_Print(0, ("pData[%d]=[0x%x]\n", i, pData[i]));
		}
#endif
	}


	nRet = DRV_I2c_WriteA8( ulI2cChannel, ucDemodAddr, CONST_CXD2820_GWSUBADDR,\
							aucWriteData, 2 );
    if ( nRet != 0 )
    {
    	CH_DI_Print(0, (">>> Error : DRV_I2c_WriteA8()... in UserWrittenI2CWrite()... \n"));
        return TM_ERR_WRITE;
    }

//////////////////////////////////////////////////////////////////////////////////////

	return err;
}

#elif defined(CONFIG_PLATFORM_DP10048)
tmErrorCode_t UserWrittenI2CRead(tmUnitSelect_t tUnit,  UInt32 AddrSize, UInt8* pAddr,
UInt32 ReadLen, UInt8* pData)
{
    /* Variable declarations */
    tmErrorCode_t err = TM_OK;
    int nRet = 0;
    HUINT32 ulI2cChannel = I2C_CHANNEL_CHANNEL;
    HUINT8  ucTunerAddr = M_TDA18275_I2CADDR;
    HUINT8 ucSubAddr = 0;
	int nSelectI2cBus = 0;
#if defined(DEBUG_TUNER_I2C)
    UInt32 i;
#endif


    if( pAddr == NULL )
    {
        return DI_CH_PARAM_ERR;
    }
    if( pData == NULL )
    {
        return DI_CH_PARAM_ERR;
    }

    if( AddrSize >= 2 || AddrSize == 0 )
    {
        CH_DI_Print(0, (">>> Error : UserWrittenI2CRead()... AddrSize[%d] \n", AddrSize));
        return DI_CH_PARAM_ERR;
    }

	nSelectI2cBus = tUnit%2;
	CH_DI_Print(7, ("UserWrittenI2CRead()... nSelectI2cBus[%d] \n", nSelectI2cBus));
#if defined(CONFIG_I2C_MAP_FOR_4TUNER)
	if(tUnit == 0)
	{
		ulI2cChannel = I2C_CHANNEL_CHANNEL;
	}
	else if(tUnit == 1)
	{
		ulI2cChannel = I2C_CHANNEL_CHANNEL2;
	}
	else if(tUnit == 2)
	{
		ulI2cChannel = I2C_CHANNEL_CHANNEL3;
	}
	else if(tUnit == 3)
	{
		ulI2cChannel = I2C_CHANNEL_CHANNEL4;
	}
	else
	{
		ulI2cChannel = I2C_CHANNEL_CHANNEL;
	}
#else
	if(tUnit>=2)
	{
		ulI2cChannel = I2C_CHANNEL_CHANNEL2;
	}
	else
	{
		ulI2cChannel = I2C_CHANNEL_CHANNEL;
	}
#endif

	if(tUnit%2)
	{
		ucTunerAddr = S_TDA18275_I2CADDR;
	}
	else
	{
		ucTunerAddr = M_TDA18275_I2CADDR;
	}
#if 0
	CH_DI_Print(0, ("######################################################\n"));
	CH_DI_Print(0, ("UserWrittenI2CRead() tUnit[%d] : ulI2cChannel[%d]::[0x%x]ucTunerAddr\n",tUnit, ulI2cChannel,ucTunerAddr));
	CH_DI_Print(0, ("######################################################\n"));
#endif

    ucSubAddr = *pAddr;

    DRV_CH_EnableI2c( tUnit );

    nRet = DRV_I2c_ReadA8( ulI2cChannel, ucTunerAddr, ucSubAddr, pData, ReadLen);
    if ( nRet != 0 )
    {
        CH_DI_Print(0, (">>> Error : DRV_I2c_ReadA8()... in UserWrittenI2CRead()... \n"));
        DRV_CH_DisableI2c( tUnit );
        return TM_ERR_READ;
    }
    else
    {
#if defined(DEBUG_TUNER_I2C)
        CH_DI_Print(0, ("\n<<< Read : Sub-address, AddrSize[%d]\n", AddrSize));
        for(i=0; i<AddrSize; i++)
        {
            CH_DI_Print(0, ("pAddr[%d]=[0x%x]\n", i, pAddr[i]));
        }
        CH_DI_Print(0, ("<<< Read : Data, ReadLen[%d]\n", ReadLen));
        for(i=0; i<ReadLen; i++)
        {
            CH_DI_Print(0, ("pData[%d]=[0x%x]\n", i, pData[i]));
        }
#endif
    }

    DRV_CH_DisableI2c( tUnit );

    return err;
}
#endif


/********************************************************************************
*   Function    : UserWrittenI2CWrite
*   Description :
*   Input       :   tmUnitSelect_t tUnit
*               UInt32 AddrSize,
*               UInt8* pAddr,
*               UInt32 WriteLen,
*               UInt8* pData
*   Return  :   tmErrorCode_t.
********************************************************************************/
#if defined(CONFIG_PLATFORM_DS2820)||defined(CONFIG_PLATFORM_DS2834)
tmErrorCode_t UserWrittenI2CWrite (tmUnitSelect_t tUnit, 	UInt32 AddrSize, UInt8* pAddr,
UInt32 WriteLen, UInt8* pData)
{
	/* Variable declarations */
	tmErrorCode_t err = TM_OK;
	int nRet = 0;
	HUINT32	ulI2cChannel = I2C_CHANNEL_CHANNEL;
	HUINT8	ucDemodAddr = M_CXD2820_I2CADDR;
	HUINT8	ucTunerAddr = M_TDA18275_I2CADDR;
	int nTotalLength = 0;
	HUINT8 aucWriteData[MAX_WRITE_BYTE];
#if defined(DEBUG_TUNER_I2C)
	int i;
#endif

	if( pAddr == NULL )
	{
		return DI_CH_PARAM_ERR;
	}
	if( pData == NULL )
	{
		return DI_CH_PARAM_ERR;
	}

	if( AddrSize >= 2 || AddrSize == 0 )
	{
		CH_DI_Print(0, (">>> Error : UserWrittenI2CRead()... AddrSize[%d] \n", AddrSize));
		return DI_CH_PARAM_ERR;
	}

	if(tUnit == M_TDA18275_TUNER )
	{
		ulI2cChannel = I2C_CHANNEL_CHANNEL;
		ucDemodAddr = M_CXD2820_I2CADDR;
		ucTunerAddr = M_TDA18275_I2CADDR;
	}
	else if(tUnit == S_TDA18275_TUNER )
	{
#if defined(CONFIG_I2C_MAP_FOR_2TUNER)
		ulI2cChannel = I2C_CHANNEL_CHANNEL2;
#else
		ulI2cChannel = I2C_CHANNEL_CHANNEL;
#endif
		ucDemodAddr = S_CXD2820_I2CADDR;
		ucTunerAddr = S_TDA18275_I2CADDR;
	}
	else
	{
		ulI2cChannel = I2C_CHANNEL_CHANNEL;
		ucDemodAddr = M_CXD2820_I2CADDR;
		ucTunerAddr = M_TDA18275_I2CADDR;
	}

//////////////////////////////////////////////////////////////////////////////////////
	nTotalLength = AddrSize + WriteLen + CONST_ADDR_LENGTH;

	if( nTotalLength > MAX_WRITE_BYTE )
	{
		CH_DI_Print(0, (">>> Error : UserWrittenI2CWrite()... nTotalLength[%d] \n", nTotalLength));
		return DI_CH_PARAM_ERR;
	}

	aucWriteData[0] = ucTunerAddr;
	VK_MEM_Memcpy( &aucWriteData[CONST_ADDR_LENGTH], pAddr, AddrSize);
	VK_MEM_Memcpy( &aucWriteData[AddrSize+CONST_ADDR_LENGTH], pData, WriteLen);

	nRet = DRV_I2c_WriteA8( ulI2cChannel, ucDemodAddr, CONST_CXD2820_GWSUBADDR,\
							aucWriteData, nTotalLength );
    if ( nRet != 0 )
    {
    	CH_DI_Print(0, (">>> Error : DRV_I2c_WriteA8()... in UserWrittenI2CWrite()... \n"));
        return TM_ERR_WRITE;
    }
	else
	{
#if defined(DEBUG_TUNER_I2C)
		CH_DI_Print(0, ("\n>>> Write : Sub-address, AddrSize[%d]\n", AddrSize));
		for(i=0; i<AddrSize; i++)
		{
			CH_DI_Print(0, ("[%d]=[0x%x]\n", i, pAddr[i]));
		}
		CH_DI_Print(0, (">>> Write : Data, WriteLen[%d]\n", WriteLen));
		for(i=0; i<WriteLen; i++)
		{
			CH_DI_Print(0, ("[%d]=[0x%x]\n", i, pData[i]));
		}
#endif
	}
//////////////////////////////////////////////////////////////////////////////////////

	return err;
}
#elif defined(CONFIG_PLATFORM_DP10048)
tmErrorCode_t UserWrittenI2CWrite (tmUnitSelect_t tUnit,    UInt32 AddrSize, UInt8* pAddr,
UInt32 WriteLen, UInt8* pData)
{
    /* Variable declarations */
    tmErrorCode_t err = TM_OK;
    int nRet = 0;
    HUINT8 ucSubAddr = 0;
    HUINT32 ulI2cChannel = I2C_CHANNEL_CHANNEL;
    HUINT8  ucTunerAddr = M_TDA18275_I2CADDR;
	int nSelectI2cBus = 0;
#if defined(DEBUG_TUNER_I2C)
    UInt32 i;
#endif

    if( pAddr == NULL )
    {
        return DI_CH_PARAM_ERR;
    }
    if( pData == NULL )
    {
        return DI_CH_PARAM_ERR;
    }

    if( AddrSize >= 2 || AddrSize == 0 )
    {
        CH_DI_Print(0, (">>> Error : UserWrittenI2CRead()... AddrSize[%d] \n", AddrSize));
        return DI_CH_PARAM_ERR;
    }

	nSelectI2cBus = tUnit%2;
	CH_DI_Print(7, ("UserWrittenI2CRead()... nSelectI2cBus[%d] \n", nSelectI2cBus));
#if defined(CONFIG_I2C_MAP_FOR_4TUNER)
	if(tUnit == 0)
	{
		ulI2cChannel = I2C_CHANNEL_CHANNEL;
	}
	else if(tUnit == 1)
	{
		ulI2cChannel = I2C_CHANNEL_CHANNEL2;
	}
	else if(tUnit == 2)
	{
		ulI2cChannel = I2C_CHANNEL_CHANNEL3;
	}
	else if(tUnit == 3)
	{
		ulI2cChannel = I2C_CHANNEL_CHANNEL4;
	}
	else
	{
		ulI2cChannel = I2C_CHANNEL_CHANNEL;
	}
#else
	if(tUnit>=2)
	{
		ulI2cChannel = I2C_CHANNEL_CHANNEL2;
	}
	else
	{
		ulI2cChannel = I2C_CHANNEL_CHANNEL;
	}
#endif
	if(tUnit%2)
	{
		ucTunerAddr = S_TDA18275_I2CADDR;
	}
	else
	{
		ucTunerAddr = M_TDA18275_I2CADDR;
	}
#if 0
	CH_DI_Print(0, ("######################################################\n"));
	CH_DI_Print(0, ("UserWrittenI2CWrite() tUnit[%d] : ulI2cChannel[%d]::[0x%x]ucTunerAddr\n",tUnit, ulI2cChannel,ucTunerAddr));
	CH_DI_Print(0, ("######################################################\n"));
#endif


    ucSubAddr = *pAddr;

    DRV_CH_EnableI2c( tUnit );

    nRet = DRV_I2c_WriteA8( ulI2cChannel, ucTunerAddr, ucSubAddr,\
                            pData, WriteLen );
    if ( nRet != 0 )
    {
    DRV_CH_DisableI2c( tUnit );
        return TM_ERR_WRITE;
    }
    else
    {
#if defined(DEBUG_TUNER_I2C)
        CH_DI_Print(0, ("\n>>> Write : Sub-address, AddrSize[%d]\n", AddrSize));
        for(i=0; i<AddrSize; i++)
        {
            CH_DI_Print(0, ("[%d]=[0x%x]\n", i, pAddr[i]));
        }
        CH_DI_Print(0, (">>> Write : Data, WriteLen[%d]\n", WriteLen));
        for(i=0; i<WriteLen; i++)
        {
            CH_DI_Print(0, ("[%d]=[0x%x]\n", i, pData[i]));
        }
#endif
    }

    DRV_CH_DisableI2c( tUnit );

    return err;
}


/********************************************************************************
*   Function    : UserWrittenI2CWriteNoAck
*   Description :
*   Input       :   tmUnitSelect_t tUnit
*               UInt32 AddrSize,
*               UInt8* pAddr,
*               UInt32 WriteLen,
*               UInt8* pData
*   Return  :   tmErrorCode_t.
********************************************************************************/
tmErrorCode_t UserWrittenI2CWriteNoAck (tmUnitSelect_t tUnit,    UInt32 AddrSize, UInt8* pAddr,
UInt32 WriteLen, UInt8* pData)
{
    /* Variable declarations */
    tmErrorCode_t err = TM_OK;
    int nRet = 0;
    HUINT8 ucSubAddr = 0;
    HUINT32 ulI2cChannel = I2C_CHANNEL_CHANNEL;
    HUINT8  ucTunerAddr = M_TDA18275_I2CADDR;
	int nSelectI2cBus = 0;
#if defined(DEBUG_TUNER_I2C)
    UInt32 i;
#endif

    if( pAddr == NULL )
    {
        return DI_CH_PARAM_ERR;
    }
    if( pData == NULL )
    {
        return DI_CH_PARAM_ERR;
    }

    if( AddrSize >= 2 || AddrSize == 0 )
    {
        CH_DI_Print(0, (">>> Error : UserWrittenI2CRead()... AddrSize[%d] \n", AddrSize));
        return DI_CH_PARAM_ERR;
    }

	nSelectI2cBus = tUnit%2;
	CH_DI_Print(7, ("UserWrittenI2CRead()... nSelectI2cBus[%d] \n", nSelectI2cBus));
#if defined(CONFIG_I2C_MAP_FOR_4TUNER)
	if(tUnit == 0)
	{
		ulI2cChannel = I2C_CHANNEL_CHANNEL;
	}
	else if(tUnit == 1)
	{
		ulI2cChannel = I2C_CHANNEL_CHANNEL2;
	}
	else if(tUnit == 2)
	{
		ulI2cChannel = I2C_CHANNEL_CHANNEL3;
	}
	else if(tUnit == 3)
	{
		ulI2cChannel = I2C_CHANNEL_CHANNEL4;
	}
	else
	{
		ulI2cChannel = I2C_CHANNEL_CHANNEL;
	}
#else
	if(tUnit>=2)
	{
		ulI2cChannel = I2C_CHANNEL_CHANNEL2;
	}
	else
	{
		ulI2cChannel = I2C_CHANNEL_CHANNEL;
	}
#endif
	if(tUnit%2)
	{
		ucTunerAddr = S_TDA18275_I2CADDR;
	}
	else
	{
		ucTunerAddr = M_TDA18275_I2CADDR;
	}
#if 0
	CH_DI_Print(0, ("######################################################\n"));
	CH_DI_Print(0, ("UserWrittenI2CWriteNoAck() tUnit[%d] : ucTunerAddr[%d]::[0x%x]ucTunerAddr\n",tUnit, ulI2cChannel,ucTunerAddr));
	CH_DI_Print(0, ("######################################################\n"));
#endif


    ucSubAddr = *pAddr;

    DRV_CH_EnableI2c( tUnit );

    nRet = DRV_I2c_WriteA8NoAck( ulI2cChannel, ucTunerAddr, ucSubAddr,\
                            pData, WriteLen );
    if ( nRet != 0 )
    {
    	DRV_CH_DisableI2c( tUnit );
		CH_DI_Print(0, ("##### Error : DRV_I2c_WriteA8NoAck(0x%x) ######\n", nRet));
        return TM_ERR_WRITE;
    }
    else
    {
#if defined(DEBUG_TUNER_I2C)
        CH_DI_Print(0, ("\n>>> Write : Sub-address, AddrSize[%d]\n", AddrSize));
        for(i=0; i<AddrSize; i++)
        {
            CH_DI_Print(0, ("[%d]=[0x%x]\n", i, pAddr[i]));
        }
        CH_DI_Print(0, (">>> Write : Data, WriteLen[%d]\n", WriteLen));
        for(i=0; i<WriteLen; i++)
        {
            CH_DI_Print(0, ("[%d]=[0x%x]\n", i, pData[i]));
        }
#endif
    }

    DRV_CH_DisableI2c( tUnit );

    return err;
}
#endif


/********************************************************************************
*   Function    : UserWrittenWait
*   Description :
*   Input       :   tmUnitSelect_t tUnit
*               UInt32 tms
*   Return  :   tmErrorCode_t.
********************************************************************************/

tmErrorCode_t UserWrittenWait(tmUnitSelect_t tUnit, UInt32 tms)
{
    /* Variable declarations */
    tmErrorCode_t err = TM_OK;
    CH_UNUSED(tUnit);
    VK_TASK_Sleep(tms);

   return err;
}


/********************************************************************************
*   Function    : UserWrittenPrint
*   Description :
*   Input       :   UInt32 level, const char* format, ...
*   Return  :   tmErrorCode_t.
********************************************************************************/

tmErrorCode_t           UserWrittenPrint(UInt32 level, const char* format, ...)
{
   /* Variable declarations */
   tmErrorCode_t err = TM_OK;

    static char     buf[PRINT_BUFFER_SIZE] = "\0";
    va_list         ap;

    //DEBUGLVL_VERBOSE , DEBUGLVL_MAX
    if( level<DEBUGLVL_MAX )
    {
        va_start(ap, format);
        (void)vsnprintf(buf, PRINT_BUFFER_SIZE, format, ap);
        va_end(ap);

        DI_UART_Print(buf);
        DI_UART_Print("\n");
    }

   return err;
}


/********************************************************************************
*   Function    : UserWrittenMutexInit
*   Description :
*   Input       :   ptmbslFrontEndMutexHandle *ppMutexHandle
*   Return  :   tmErrorCode_t.
********************************************************************************/

tmErrorCode_t  UserWrittenMutexInit(ptmbslFrontEndMutexHandle *ppMutexHandle)
{
    /* Variable declarations */
    tmErrorCode_t err = TM_OK;
    CH_UNUSED(ppMutexHandle);

/* Customer code here */
/* ...*/

/* ...*/
/* End of Customer code here */

   return err;
}


/********************************************************************************
*   Function    : UserWrittenMutexDeInit
*   Description :
*   Input       :   ptmbslFrontEndMutexHandle pMutex
*   Return  :   tmErrorCode_t.
********************************************************************************/

tmErrorCode_t  UserWrittenMutexDeInit( ptmbslFrontEndMutexHandle pMutex)
{
    /* Variable declarations */
    tmErrorCode_t err = TM_OK;
    CH_UNUSED(pMutex);
/* Customer code here */
/* ...*/

/* ...*/
/* End of Customer code here */

   return err;
}


/********************************************************************************
*   Function    : UserWrittenMutexAcquire
*   Description :
*   Input       :   ptmbslFrontEndMutexHandle pMutex, UInt32 timeOut
*   Return  :   tmErrorCode_t.
********************************************************************************/

tmErrorCode_t  UserWrittenMutexAcquire(ptmbslFrontEndMutexHandle pMutex, UInt32 timeOut)
{
    /* Variable declarations */
    tmErrorCode_t err = TM_OK;
    CH_UNUSED(pMutex);
    CH_UNUSED(timeOut);
/* Customer code here */
/* ...*/

/* ...*/
/* End of Customer code here */

   return err;
}


/********************************************************************************
*   Function    : UserWrittenMutexRelease
*   Description :
*   Input       :   ptmbslFrontEndMutexHandle pMutex
*   Return  :   tmErrorCode_t.
********************************************************************************/

tmErrorCode_t  UserWrittenMutexRelease(ptmbslFrontEndMutexHandle pMutex)
{
    /* Variable declarations */
    tmErrorCode_t err = TM_OK;
    CH_UNUSED(pMutex);
/* Customer code here */
/* ...*/

/* ...*/
/* End of Customer code here */

   return err;
}

/********************************************************************************
* Public functions
********************************************************************************/

#define _______________________________________________________________________
#define ____CH_T_PUBLIC_FUNCITONS____

/********************************************************************************
*   Function    : TDA18275_Initialize
*   Description :
*   Input       :
*   Return  :
********************************************************************************/

int TDA18273_Initialize( int nUnitId )
{
    tmErrorCode_t err = TM_OK;
    tmbslFrontEndDependency_t sSrvTunerFunc;

    sSrvTunerFunc.sIo.Write             = UserWrittenI2CWrite;
	sSrvTunerFunc.sIo.WriteNoAck             = UserWrittenI2CWriteNoAck;
    sSrvTunerFunc.sIo.Read              = UserWrittenI2CRead;
    sSrvTunerFunc.sTime.Get             = Null;
    sSrvTunerFunc.sTime.Wait            = UserWrittenWait;
    sSrvTunerFunc.sDebug.Print          = UserWrittenPrint;
    sSrvTunerFunc.sMutex.Init           = UserWrittenMutexInit;
    sSrvTunerFunc.sMutex.DeInit         = UserWrittenMutexDeInit;
    sSrvTunerFunc.sMutex.Acquire        = UserWrittenMutexAcquire;
    sSrvTunerFunc.sMutex.Release        = UserWrittenMutexRelease;
    sSrvTunerFunc.dwAdditionalDataSize  = 0;
    sSrvTunerFunc.pAdditionalData       = Null;

    CH_DI_Print(3, ("+++ TDA18275_Initialize()...nUnitId(%d)\n", nUnitId));

    /* TDA18275 Master Driver low layer setup */
    err = tmbslTDA18275_Open(nUnitId, &sSrvTunerFunc);
    if(err != TM_OK)
    {
        CH_DI_Print(0, (">>> Error(0x%x) : tmbslTDA18275_Open()... in TDA18275_Initialize()... \n", err));
        return err;
    }

#if 0//TODO_ILEE : TUNER IIC CHECK

        err = tmbslTDA18275_CheckHWVersion(nUnitId);
        if(err != TM_OK)
        {
            CH_DI_Print(0, (">>> Error : tmbslTDA18275_CheckHWVersion()... in TDA18275_Initialize()... \n"));
            return TDA18275_ERR_BAD_VERSION;
        }

        tmbslTDA18275_SetIRQWait(nUnitId, TRUE);
#endif

    /* TDA18275 Master Hardware initialization */
    err = tmbslTDA18275_HwInit(nUnitId);
    if(err != TM_OK)
    {
        CH_DI_Print(0, (">>> Error : tmbslTDA18275_HwInit()... in TDA18275_Initialize(): nUnitId(%d)... \n", nUnitId));
        return err;
    }

    CH_DI_Print(3, ("--- TDA18275_Initialize()...nUnitId(%d)\n", nUnitId));

    return err;
}


/********************************************************************************
*   Function    : TDA18275_SetTune
*   Description :
*   Input       :
*   Return  :
********************************************************************************/

int TDA18273_SetTune( int nUnitId, HUINT32 ulRfFreqHz, int bandWidth )
{
    tmErrorCode_t err = TM_OK;
    tmTDA18275StandardMode_t TDA18275StdMode = tmTDA18275_DVBT_8MHz;
    //UInt32 uRF = 770000000;
    //HUINT32 ulIfFreq = 0;
    tmbslFrontEndState_t etPllLockStatus = tmbslFrontEndStateUnknown;
	CH_UNUSED(ulRfFreqHz);
    if( bandWidth == 8 )
    {
        TDA18275StdMode = tmTDA18275_DVBT_8MHz;
    }
    else if( bandWidth == 7 )
    {
        TDA18275StdMode = tmTDA18275_DVBT_7MHz;
    }
    else
    {
        TDA18275StdMode = tmTDA18275_DVBT_8MHz;
    }

    CH_DI_Print(3, (">>> tmbslTDA18275SetStandardMode(%d) :: bandWidth(%d):(%d)TDA18275StdMode \n", nUnitId, bandWidth, TDA18275StdMode));
#if 0
    /* TDA18275 Master standard mode */
    err = tmbslTDA18275_SetStandardMode(nUnitId, TDA18275StdMode);
    if(err != TM_OK)
    {
        CH_DI_Print(0, (">>> Error : tmbslTDA18275SetStandardMode()... in TDA18275_SetTune()... \n"));
        return err;
    }
    /* TDA18275 Master RF frequency */
    err = tmbslTDA18275_SetRF(nUnitId, ulRfFreqHz);
    if(err != TM_OK)
    {
        CH_DI_Print(0, (">>> Error : tmbslTDA18275SetRf()... in TDA18275_SetTune()... \n"));
        return err;
    }
#endif
    /* Get TDA18275 PLL Lock status */
    err = tmbslTDA18275_GetLockStatus(nUnitId, &etPllLockStatus);
    if(err != TM_OK)
    {
        CH_DI_Print(0, (">>> Error : TDA18275_SetTune()... err(0x%x)... \n", err));
    }
    else
    {
        if( etPllLockStatus == tmbslFrontEndStateLocked )
        {
            CH_DI_Print(3, ("<<<<<<<<<<<<<<<<<<<<<<<<< tmbslFrontEndStateLocked!!! \n"));
        }
        else
        {
            CH_DI_Print(0, (">>>>>>>>>>>>>>>>>>>>>>>>> etPllLockStatus(%d)!!! \n", etPllLockStatus));
        }
    }
#if 0
    err = tmbslTDA18275_GetIF(nUnitId, (UInt32 *)&ulIfFreq);
    if(err != TM_OK)
    {
        CH_DI_Print(0, (">>> Error : TDA18275_SetTune()... err(0x%x)... \n", err));
    }
    else
    {
        CH_DI_Print(4, (">>>>>>>>>>>>>>>>>>>>>>>>> ulIfFreq(%d)!!! \n", ulIfFreq));
    }
#endif
    CH_DI_Print(4, ("&&&&&&&&&&&&&&&&&&&&&&&&&  TDA18275_SetTune()... \n"));

    return err;
}


/********************************************************************************
*   Function    : TDA18275_GetPowerLevel
*   Description :
*   Input       :
*   Return  :
********************************************************************************/

#if 0
int TDA18275_GetPowerLevel(int nUnitId, float *pfPower)
{
    tmErrorCode_t err = TM_OK;
    int nRfLevel = 0;
    HUINT32 ulIfFreq = 5000000;
    HUINT8 ucRssiValue = 0;//TODO_ILEE
    CH_UNUSED(ulIfFreq);

    CH_DI_Print(5, ("TDA18275_GetPowerLevel()... Unit(0x%x)... \n", nUnitId));
    err = tmbslTDA18275_GetPowerLevel(nUnitId, (UInt8 *)&nRfLevel);
    if(err != TM_OK)
    {
        CH_DI_Print(0, (">>> Error : TDA18275GetPower_Level()... err(0x%x)... \n", err));
    }
    else
    {
        *pfPower = (float)nRfLevel*0.5;
        CH_DI_Print(4, (">>>>>>>>>>>> nUnitId(%d):(%f)dBuV pfPower \n", nUnitId, *pfPower));
        //CH_DI_Print(4, (">>>>>>>>>>>> nUnitId(%d):(%f)dBmV pfPower \n", nUnitId, *pfPower-60));
        //CH_DI_Print(4, (">>>>>>>>>>>> nUnitId(%d):(%f)dBm pfPower \n", nUnitId, *pfPower-47-60));
    }

    if(*pfPower >= 55)  ucRssiValue = 100;
    else if(*pfPower >= 53) ucRssiValue = 90;
    else if(*pfPower >= 51) ucRssiValue = 80;
    else if(*pfPower >= 49) ucRssiValue = 70;
    else if(*pfPower >= 47) ucRssiValue = 60;
    else if(*pfPower >= 45) ucRssiValue = 50;
    else if(*pfPower >= 43) ucRssiValue = 40;
    else if(*pfPower >= 41) ucRssiValue = 30;
    else if(*pfPower >= 39) ucRssiValue = 20;
    else    ucRssiValue = 10;

    return ucRssiValue;
}
#else
int TDA18273_GetPowerLevel(int nUnitId, float *pfPower)
{
    tmErrorCode_t err = TM_OK;
    int nRfLevel = 0;

    CH_DI_Print(5, ("TDA18275_GetPowerLevel()... Unit(0x%x)... \n", nUnitId));
    err = tmbslTDA18275_GetPowerLevel(nUnitId, (UInt8 *)&nRfLevel);
    if(err != TM_OK)
    {
        CH_DI_Print(0, (">>> Error : TDA18275GetPower_Level()... err(0x%x)... \n", err));
    }
    else
    {
        *pfPower = (float)nRfLevel*0.5;
        CH_DI_Print(4, (">>>>>>>>>>>> nUnitId(%d):(%f)dBuV pfPower \n", nUnitId, *pfPower));
        //CH_DI_Print(4, (">>>>>>>>>>>> nUnitId(%d):(%f)dBmV pfPower \n", nUnitId, *pfPower-60));
        //CH_DI_Print(4, (">>>>>>>>>>>> nUnitId(%d):(%f)dBm pfPower \n", nUnitId, *pfPower-47-60));
    }

    return err;
}
#endif


int TDA18273_Close( int nUnitId )
{
    tmErrorCode_t err = TM_OK;

    CH_DI_Print(3, ("+++ TDA18273_Close()...nUnitId(%d)\n", nUnitId));

    /* TDA18275 Master Driver low layer setup */
    err = tmbslTDA18275_Close(nUnitId);
    if(err != TM_OK)
    {
        CH_DI_Print(0, (">>> Error(0x%x) : tmbslTDA18275_Close()... in TDA18273_Close()... \n", err));
    }

	return err;
}
/********************************************************************************
*   Function    : TDA18275_DumpRegister
*   Description : for debugging tuner chip register
*   Input       :
*   Return  :
********************************************************************************/

void TDA18275_DumpRegister(int nUnitId)
{
    tmErrorCode_t   err = TM_OK;
    HUINT8          ucAddr=0x00;
    HUINT8          ucBuffer[NUM_OF_REG];
    HUINT8          i;

    err = UserWrittenI2CRead(nUnitId, 1, &ucAddr, NUM_OF_REG, ucBuffer);

    CH_DI_Print(0, ("===================================================\n"));
    CH_DI_Print(0, ("   TDA18275_DumpRegister [%d] Dump\n", nUnitId));
    CH_DI_Print(0, ("===================================================\n"));

    if (err == TM_OK)
    {
        for(i=0; i<NUM_OF_REG; i++)
        {
            CH_DI_Print(0, ("[0x%02X] \t[0x%02x]\n", i, ucBuffer[i]));
        }
    }
    else
    {
        CH_DI_Print(0, ("TDA18275_DumpRegister[%d] :: Fail!! Error[0x%0X]\n", nUnitId, err));
    }

    CH_DI_Print(0, ("===================================================\n"));
}



HUINT32 TDA18275_GetIfFreq( int nUnitId, HUINT32 ulRfFreqHz, int nBandWidth )
{
    tmErrorCode_t err = TM_OK;
    tmTDA18275StandardMode_t TDA18275StdMode = tmTDA18275_DVBT_8MHz;
    HUINT32 ulIfFreq = 0;

    if( nBandWidth == 8 )
    {
        TDA18275StdMode = tmTDA18275_DVBT_8MHz;
    }
    else if( nBandWidth == 7 )
    {
        TDA18275StdMode = tmTDA18275_DVBT_7MHz;
    }
    else
    {
        TDA18275StdMode = tmTDA18275_DVBT_8MHz;
    }

    CH_DI_Print(3, (">>> tmbslTDA18275SetStandardMode(%d) :: nBandWidth(%d):(%d)TDA18275StdMode \n", nUnitId, nBandWidth, TDA18275StdMode));

    /* TDA18275 Master standard mode */
    err = tmbslTDA18275_SetStandardMode(nUnitId, TDA18275StdMode);
    if(err != TM_OK)
    {
        CH_DI_Print(0, (">>> Error : tmbslTDA18275SetStandardMode()... in TDA18275_SetTune()... \n"));
        return 0;
    }

    /* TDA18275 Master RF frequency */
    err = tmbslTDA18275_SetRF(nUnitId, ulRfFreqHz);
    if(err != TM_OK)
    {
        CH_DI_Print(0, (">>> Error : tmbslTDA18275SetRf()... in TDA18275_SetTune()... \n"));
        return 0;
    }

    err = tmbslTDA18275_GetIF(nUnitId, (UInt32 *)&ulIfFreq);
    if(err != TM_OK)
    {
        CH_DI_Print(0, (">>> Error : tmbslTDA18275_GetIF()... err(0x%x)... \n", err));
		return 0;
    }
    else
    {
        CH_DI_Print(3, (">>>>>>>>>>>>>>>>>>>>>>>>> ulIfFreq(%d)!!! \n", ulIfFreq));
    }

    return ulIfFreq;
}

/*********************** End of File ******************************/

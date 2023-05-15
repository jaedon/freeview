/*
 * $Header:   $
 */
/*********************************************************************
 * $Workfile:   di_eeprom.c  $
 * $Modtime:   Jul 30 2007 01:07:04  $
 *
 * Author:
 * Description:
 *
 *                                 Copyright (c) 2008 HUMAX Co., Ltd.
 *                                               All rights reserved.
 ********************************************************************/

/*******************************************************************/
/**************************** Header Files *************************/
/*******************************************************************/
#if defined(CONFIG_VK_STDLIB)
#else
#include 	<stdio.h>
#include 	<stdlib.h>
#include 	<string.h>
#endif

#include "linden_trace.h"
#include "htype.h"

#include "taskdef.h"

#include "di_err.h"
#include "vkernel.h"
#include "di_eeprom.h"
#include "drv_eeprom.h"
#include "di_eeprom_ota.h"
#include "di_nvram.h"
#include "drv_nvram.h"

/*******************************************************************/
/****************************** define *****************************/
/*******************************************************************/

/* eeprom size */
#if defined (CONFIG_EEPROM_SIZE_32KB)
#define DRV_EEPROM_DEVICE_SIZE		(0x8000)
#else
#define DRV_EEPROM_DEVICE_SIZE		(0x2000)
#endif

/*********************************************************************
 * static Function Prototypes
 *********************************************************************/


/*********************************************************************
 * Local Function
 *********************************************************************/
DI_ERR_CODE DRV_EEPROM_Write(HUINT32 usAddress, HUINT8* pucBuffer, HUINT32 ulSize);
DI_ERR_CODE DRV_EEPROM_Read(HUINT32 usAddress, HUINT8* pucBuffer, HUINT32 ulSize);
DI_ERR_CODE DRV_EEPROM_GetCapabilities(unsigned long *pulEepromSize);


/*********************************************************************
 *
 * Function 	DRV_EEPROM_Init
 * Description
 * Inputs
 * Outputs
 * Returns
 *
 *********************************************************************/


/*********************************************************************
 *
 * Function 	DI_EEPROM_Read
 * Description
 * Inputs
 * Outputs
 * Returns
 *
 *********************************************************************/
DI_ERR_CODE DI_EEPROM_Read(unsigned long address, unsigned char *data, unsigned long numToRead)
{
    DI_ERR_CODE errCode = DI_ERR_OK;

    errCode= DRV_EEPROM_Read(address, data, numToRead);
    if(DI_ERR_OK != errCode)
    {
    	TraceError(TRACE_MODULE_DI_EEPROM, "%s(%d) : DRV_EEPROM_Read Error!!.\n", __func__, __LINE__);
    	return DI_ERR_ERROR;
    }

    return DI_ERR_OK;
}

/*********************************************************************
 *
 * Function 	DI_EEPROM_Write
 * Description
 * Inputs
 * Outputs
 * Returns
 *
 *********************************************************************/
DI_ERR_CODE DI_EEPROM_Write(unsigned long address, unsigned char *data, unsigned long numToWrite)
{
    DI_ERR_CODE errCode = DI_ERR_OK;

    errCode = DRV_EEPROM_Write(address, data, numToWrite);
    if(DI_ERR_OK != errCode)
    {
    	TraceError(TRACE_MODULE_DI_EEPROM, "%s(%d) : DRV_EEPROM_Write Error!!.\n", __func__, __LINE__);
    	return DI_ERR_ERROR;
    }
    return DI_ERR_OK;
}


/*********************************************************************
 *
 * Function 	DI_EEPROM_EraseAll
 * Description
 * Inputs
 * Outputs
 * Returns
 *
 *********************************************************************/
DI_ERR_CODE DI_EEPROM_EraseAll(void)
{
    DI_ERR_CODE errCode;
    unsigned char *data;
    data = DD_MEM_Alloc(DRV_EEPROM_DEVICE_SIZE);
    if( data == NULL)
    {
        TraceError(TRACE_MODULE_DI_EEPROM, "%s(%d) : DD_MEM_Alloc Error!!.\n", __func__, __LINE__);
        return DI_ERR_NO_RESOURCE;
    }
    VK_memset(data, 0, DRV_EEPROM_DEVICE_SIZE);
    errCode = DRV_EEPROM_Write(0, data, DRV_EEPROM_DEVICE_SIZE);
    DD_MEM_Free(data);
    if(DI_ERR_OK != errCode)
    {
        TraceError(TRACE_MODULE_DI_EEPROM, "%s(%d) : DRV_EEPROM_Write Error!!.\n", __func__, __LINE__);
        return DI_ERR_ERROR;
    }
    return DI_ERR_OK;
}

/*********************************************************************
 * Function 	DI_EEPROM_GetCapabilities
 * Description
 * Inputs
 * Outputs
 * Returns
 *********************************************************************/
DI_ERR_CODE DI_EEPROM_GetCapabilities(unsigned long *pulEepromSize)
{
    DI_ERR_CODE errCode = DI_ERR_OK;
    unsigned long drv_eeprom_size = 0;

    if(pulEepromSize == NULL)
    {
        TraceError(TRACE_MODULE_DI_EEPROM, "%s(%d) : pulEepromSize == NULL Error!!.\n", __func__, __LINE__);
        return DI_ERR_INVALID_PARAM;
    }

    *pulEepromSize = 0;

    errCode = DRV_EEPROM_GetCapabilities(&drv_eeprom_size);
    if(DI_ERR_OK != errCode)
    {
        TraceError(TRACE_MODULE_DI_EEPROM, "%s(%d) : DRV_EEPROM_GetCapabilities  Error!!.\n", __func__, __LINE__);
        return DI_ERR_INVALID_PARAM;
    }

    *pulEepromSize = (drv_eeprom_size);

    return DI_ERR_OK;
}

DI_ERR_CODE DRV_EEPROM_Write  ( HUINT32  usAddress,
                                        HUINT8 *  pucBuffer,
                                        HUINT32  ulSize
                                      )

{
    DI_ERR_CODE errCode = DI_ERR_OK;

	if( (pucBuffer == NULL) || (ulSize == 0) || ((usAddress+ulSize) > DRV_EEPROM_DEVICE_SIZE) )
	{
		TraceError(TRACE_MODULE_DI_EEPROM, "%s : Line(%d), %s\n", __FUNCTION__, __LINE__, __FILE__);
		return DI_ERR_ERROR;
	}

	errCode = DRV_NVRAM_SetField(DI_NVRAM_FIELD_BIN8K, usAddress, pucBuffer, ulSize);
    if(DI_ERR_OK != errCode)
    {
        TraceError(TRACE_MODULE_DI_EEPROM, "%s(%d) : DRV_NVRAM_SetField  Error!!.\n", __func__, __LINE__);
        return DI_ERR_INVALID_PARAM;
    }
    return DI_ERR_OK;
}


DI_ERR_CODE DRV_EEPROM_Read  ( HUINT32  usAddress,
                                       HUINT8 *  pucBuffer,
                                       HUINT32  ulSize
                                     )

{
    DI_ERR_CODE errCode = DI_ERR_OK;

    if( (pucBuffer == NULL) || (ulSize == 0) || ((usAddress+ulSize) > DRV_EEPROM_DEVICE_SIZE) )
    {
        TraceError(TRACE_MODULE_DI_EEPROM, "%s : Line(%d), %s\n", __FUNCTION__, __LINE__, __FILE__);
        return DI_ERR_ERROR;
    }

	errCode = DRV_NVRAM_GetField(DI_NVRAM_FIELD_BIN8K, usAddress, pucBuffer, ulSize);
    if(DI_ERR_OK != errCode)
    {
        TraceError(TRACE_MODULE_DI_EEPROM, "%s(%d) : DRV_NVRAM_GetField  Error!!.\n", __func__, __LINE__);
        return DI_ERR_INVALID_PARAM;
    }

    return DI_ERR_OK;
}


DI_ERR_CODE DRV_EEPROM_GetCapabilities(unsigned long *pulEepromSize)
{
    if(pulEepromSize == NULL)
    {
        TraceError(TRACE_MODULE_DI_EEPROM, "%s(%d) : pulEepromSize == NULL Error!!.\n", __func__, __LINE__);
        return DI_ERR_INVALID_PARAM;
    }

    *pulEepromSize = DRV_EEPROM_DEVICE_SIZE;

    return DI_ERR_OK;
}




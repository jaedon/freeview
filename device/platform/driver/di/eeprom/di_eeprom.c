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

#include "drv_i2c.h"
#include "drv_gpio.h"

/*******************************************************************/
/****************************** define *****************************/
/*******************************************************************/

/* eeprom size */
#define DRV_EEPROM_NUM_CHIP     		(1)
#if defined (CONFIG_EEPROM_SIZE_32KB)
#define DRV_EEPROM_ONE_CHIP_SIZE		(0x8000) // size - (32KB, 24LC256)
#else
#define DRV_EEPROM_ONE_CHIP_SIZE		(0x2000) //size - (8KB, 24LC64[64kbit])
#endif

#define DRV_EEPROM_DEVICE_SIZE (DRV_EEPROM_NUM_CHIP * DRV_EEPROM_ONE_CHIP_SIZE)
#define DRV_EEPROM_SYSTEM_SIZE		        (0x800)			/* 2K */

/* eeprom i2c channel */
#define DRV_I2C_CHANNEL_EEPROM	I2C_CHANNEL_EEPROM

/* eeprom read/write address */
#define DRV_EEPROM_CHIP_ADDR_READ	       (0xa1>>1)
#define DRV_EEPROM_CHIP_ADDR_WRITE	(0xa0>>1)

/* eeprom pase size */
#define DRV_EEPROM_PAGE_SIZE_BYTES	    32

#define DRV_EEPROM_UNCACHED_VAL 0
#define DRV_EEPROM_CACHED_VAL 1

#define DRV_I2C_ACCESS_SLEEP_TIME_MIN 5

/*********************************************************************
 * static Function Prototypes
 *********************************************************************/

HUINT8	*s_pEepromCache;
HUINT8	*s_pulEepromCacheMask;
unsigned long	s_eepromSema;
static HUINT32	 s_uleepromWriteTick;

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
DRV_Error DRV_EEPROM_Init(void)
{
	int err;

	err = VK_SEM_Create((unsigned long*)&s_eepromSema, (const char*)"DI_EEPROM_SEM", VK_SUSPENDTYPE_FIFO);
	if (err != VK_OK)
	{
    	TraceError(TRACE_MODULE_DI_EEPROM, "%s(%d) : VK_SEM_Create Error!!.\n", __func__, __LINE__);
		return DRV_ERR;
	}
	s_pEepromCache = DD_MEM_Alloc(DRV_EEPROM_ONE_CHIP_SIZE);
	if (s_pEepromCache == NULL)
	{
    	TraceError(TRACE_MODULE_DI_EEPROM, "%s(%d) : DD_MEM_Alloc Error!!.\n", __func__, __LINE__);
		return DRV_ERR_OUTOFMEMORY;
	}

	s_pulEepromCacheMask = DD_MEM_Alloc(DRV_EEPROM_ONE_CHIP_SIZE/DRV_EEPROM_PAGE_SIZE_BYTES);
	if (s_pulEepromCacheMask == NULL)
	{
		TraceError(TRACE_MODULE_DI_EEPROM, "%s(%d) : DD_MEM_Alloc Error!!.\n", __func__, __LINE__);
		return DRV_ERR_OUTOFMEMORY;
	}
	VK_memset(s_pulEepromCacheMask, DRV_EEPROM_UNCACHED_VAL, DRV_EEPROM_ONE_CHIP_SIZE/DRV_EEPROM_PAGE_SIZE_BYTES);

	/* Disable EEP_WP */
	DRV_GPIO_Write(GPIO_ID_NVRAM_WP, GPIO_LOW);

	DRV_EEPROM_MapInit();

	return DRV_OK;
}

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

    errCode= DRV_EEPROM_Read(address+DRV_EEPROM_SYSTEM_SIZE, data, numToRead);
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

    errCode = DRV_EEPROM_Write(address+DRV_EEPROM_SYSTEM_SIZE, data, numToWrite);
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
    data = DD_MEM_Alloc(DRV_EEPROM_DEVICE_SIZE - DRV_EEPROM_SYSTEM_SIZE);
    if( data == NULL)
    {
        TraceError(TRACE_MODULE_DI_EEPROM, "%s(%d) : DD_MEM_Alloc Error!!.\n", __func__, __LINE__);
        return DI_ERR_NO_RESOURCE;
    }
    VK_memset(data, 0, DRV_EEPROM_DEVICE_SIZE - DRV_EEPROM_SYSTEM_SIZE);
    errCode = DRV_EEPROM_Write(DRV_EEPROM_SYSTEM_SIZE, data, DRV_EEPROM_DEVICE_SIZE - DRV_EEPROM_SYSTEM_SIZE);
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

    *pulEepromSize = (drv_eeprom_size - DRV_EEPROM_SYSTEM_SIZE);

    return DI_ERR_OK;
}

static HBOOL P_EEPROM_IsCached(HUINT32 usAddress, HUINT32 ulSize)
{
	HUINT32		i;

#if defined(CONFIG_PRODUCT_IMAGE_HWTEST) || defined(CONFIG_PRODUCT_IMAGE_FACTORY)
	return FALSE;
#endif
	if (ulSize == 0)
		return FALSE;

	ulSize += (usAddress%DRV_EEPROM_PAGE_SIZE_BYTES + 31);
	ulSize /= DRV_EEPROM_PAGE_SIZE_BYTES;
	usAddress /= DRV_EEPROM_PAGE_SIZE_BYTES;
  	for (i=0; i<ulSize; i++)
  	{
  		if (s_pulEepromCacheMask[usAddress+i] == DRV_EEPROM_UNCACHED_VAL)
  		{
			return FALSE;
  		}
  	}
  	return TRUE;
}

DI_ERR_CODE DRV_EEPROM_Write  ( HUINT32  usAddress,
                                        HUINT8 *  pucBuffer,
                                        HUINT32  ulSize
                                      )

{
    DI_ERR_CODE errCode = DI_ERR_OK;
    HUINT32 count = 0;
    HBOOL	cached;

	if( (pucBuffer == NULL) || (ulSize == 0) || ((usAddress+ulSize) > DRV_EEPROM_DEVICE_SIZE) )
	{
		TraceError(TRACE_MODULE_DI_EEPROM, "%s : Line(%d), %s\n", __FUNCTION__, __LINE__, __FILE__);
		return DI_ERR_ERROR;
	}

	VK_SEM_Get(s_eepromSema);
	while(ulSize)
    {
        if( ulSize > DRV_EEPROM_PAGE_SIZE_BYTES )
        {
            count = DRV_EEPROM_PAGE_SIZE_BYTES;
        }
        else
        {
            count = ulSize;
        }

        if( ((usAddress+count)/DRV_EEPROM_PAGE_SIZE_BYTES) > (usAddress/DRV_EEPROM_PAGE_SIZE_BYTES) )
        {
            count = DRV_EEPROM_PAGE_SIZE_BYTES - (usAddress%DRV_EEPROM_PAGE_SIZE_BYTES);
        }

		cached = P_EEPROM_IsCached(usAddress, count);
		if (cached == FALSE || VK_memcmp(s_pEepromCache+usAddress, pucBuffer, count))
		{
			HUINT32	tickNow, tickDiff;

			tickNow = VK_TIMER_GetSystemTick();
			if (VK_TIMER_TimeAfter(s_uleepromWriteTick+DRV_I2C_ACCESS_SLEEP_TIME_MIN, tickNow) == VK_TIMER_RUNNING)
			{
				tickDiff = DRV_I2C_ACCESS_SLEEP_TIME_MIN - (tickNow - s_uleepromWriteTick);
				if (tickDiff > DRV_I2C_ACCESS_SLEEP_TIME_MIN)
					tickDiff = DRV_I2C_ACCESS_SLEEP_TIME_MIN;
				VK_TASK_Sleep(tickDiff);
			}
			TraceInfo(TRACE_MODULE_DI_EEPROM, "[DRV_EEPROM_Write] addr(%04X) size(%04X)\n", usAddress, ulSize);
			errCode = DRV_I2c_WriteA16(DRV_I2C_CHANNEL_EEPROM, DRV_EEPROM_CHIP_ADDR_WRITE, (HUINT16)usAddress, pucBuffer, count);
			if(errCode != DI_ERR_OK)
			{
				TraceError(TRACE_MODULE_DI_EEPROM, "%s->DRV_I2c_WriteA16(): Error Code(%d), Line(%d), %s\n", __FUNCTION__, errCode, __LINE__, __FILE__);
				VK_SEM_Release(s_eepromSema);
				return DI_ERR_ERROR;
			}
			s_uleepromWriteTick = VK_TIMER_GetSystemTick();

			if ((usAddress%DRV_EEPROM_PAGE_SIZE_BYTES == 0) && (count == DRV_EEPROM_PAGE_SIZE_BYTES))
			{
				VK_memcpy(s_pEepromCache+usAddress, pucBuffer, count);
				s_pulEepromCacheMask[usAddress/DRV_EEPROM_PAGE_SIZE_BYTES] = DRV_EEPROM_CACHED_VAL;
				TraceInfo(TRACE_MODULE_DI_EEPROM, "[DRV_EEPROM_Write] addr(%04X) size(%04X) <<= CACHED\n", usAddress, ulSize);
			}
			else if (cached != FALSE)
			{
				VK_memcpy(s_pEepromCache+usAddress, pucBuffer, count);
			}
	    }
        TraceInfo(TRACE_MODULE_DI_EEPROM, "count=%d, usAddress=0x%x, pucBuffer=0x%x\n", count, usAddress, *pucBuffer);
        ulSize -= count;
        usAddress += count;
        pucBuffer += count;
    }
   	VK_SEM_Release(s_eepromSema);
    return DI_ERR_OK;
}


DI_ERR_CODE DRV_EEPROM_Read  ( HUINT32  usAddress,
                                       HUINT8 *  pucBuffer,
                                       HUINT32  ulSize
                                     )

{
    DI_ERR_CODE errCode = DI_ERR_OK;
	HBOOL		cached;
	HUINT16		startAddrByPage;
	HUINT32		lengthByPage;

    if( (pucBuffer == NULL) || (ulSize == 0) || ((usAddress+ulSize) > DRV_EEPROM_DEVICE_SIZE) )
    {
        TraceError(TRACE_MODULE_DI_EEPROM, "%s : Line(%d), %s\n", __FUNCTION__, __LINE__, __FILE__);
        return DI_ERR_ERROR;
    }

  	VK_SEM_Get(s_eepromSema);
  	cached = P_EEPROM_IsCached(usAddress, ulSize);
  	if (cached == FALSE)
  	{
		HUINT32 tickNow, tickDiff;

		tickNow = VK_TIMER_GetSystemTick();
		if (VK_TIMER_TimeAfter(s_uleepromWriteTick+DRV_I2C_ACCESS_SLEEP_TIME_MIN, tickNow) == VK_TIMER_RUNNING)
		{
			tickDiff = DRV_I2C_ACCESS_SLEEP_TIME_MIN - (tickNow - s_uleepromWriteTick);
			if (tickDiff > DRV_I2C_ACCESS_SLEEP_TIME_MIN)
				tickDiff = DRV_I2C_ACCESS_SLEEP_TIME_MIN;
			VK_TASK_Sleep(tickDiff);
		}
		startAddrByPage = (HUINT16)(usAddress/DRV_EEPROM_PAGE_SIZE_BYTES)*DRV_EEPROM_PAGE_SIZE_BYTES;
  		lengthByPage = ((usAddress%DRV_EEPROM_PAGE_SIZE_BYTES + ulSize + 31)/DRV_EEPROM_PAGE_SIZE_BYTES)*DRV_EEPROM_PAGE_SIZE_BYTES;
		TraceInfo(TRACE_MODULE_DI_EEPROM, "[DRV_EEPROM_Read] addr(%04X) size(%04X) --> actual(%04X) size(%04X)\n", usAddress, ulSize, startAddrByPage, lengthByPage);
	    errCode = DRV_I2c_ReadA16(DRV_I2C_CHANNEL_EEPROM, DRV_EEPROM_CHIP_ADDR_READ, startAddrByPage, s_pEepromCache+startAddrByPage, lengthByPage);
	    if(errCode != DI_ERR_OK)
	    {
	        TraceError(TRACE_MODULE_DI_EEPROM, "%s->DRV_I2c_ReadA16(): Error Code(%d), Line(%d), %s\n", __FUNCTION__, errCode, __LINE__, __FILE__);
			VK_SEM_Release(s_eepromSema);
	        return DI_ERR_ERROR;
	    }
		VK_MEM_Memset(s_pulEepromCacheMask+usAddress/DRV_EEPROM_PAGE_SIZE_BYTES, DRV_EEPROM_CACHED_VAL, lengthByPage/DRV_EEPROM_PAGE_SIZE_BYTES);
    }
	TraceInfo(TRACE_MODULE_DI_EEPROM, "[DRV_EEPROM_Read] addr(%04X) size(%04X) --> CAHCED\n", usAddress, ulSize);
	VK_MEM_Memcpy(pucBuffer, s_pEepromCache+usAddress, ulSize);
   	VK_SEM_Release(s_eepromSema);

    return DI_ERR_OK;
}


DI_ERR_CODE DRV_EEPROM_WriteUncached  ( HUINT32  usAddress,
                                        HUINT8 *  pucBuffer,
                                        HUINT32  ulSize
                                      )

{
    DI_ERR_CODE errCode = DI_ERR_OK;
    HUINT32 count = 0;

	if( (pucBuffer == NULL) || (ulSize == 0) || ((usAddress+ulSize) > DRV_EEPROM_DEVICE_SIZE) )
	{
		TraceError(TRACE_MODULE_DI_EEPROM, "%s : Line(%d), %s\n", __FUNCTION__, __LINE__, __FILE__);
		return DI_ERR_ERROR;
	}

	VK_SEM_Get(s_eepromSema);
	while(ulSize)
    {
        if( ulSize > DRV_EEPROM_PAGE_SIZE_BYTES )
        {
            count = DRV_EEPROM_PAGE_SIZE_BYTES;
        }
        else
        {
            count = ulSize;
        }

        if( ((usAddress+count)/DRV_EEPROM_PAGE_SIZE_BYTES) > (usAddress/DRV_EEPROM_PAGE_SIZE_BYTES) )
        {
            count = DRV_EEPROM_PAGE_SIZE_BYTES - (usAddress%DRV_EEPROM_PAGE_SIZE_BYTES);
        }

		HUINT32	tickNow, tickDiff;

		tickNow = VK_TIMER_GetSystemTick();
		if (VK_TIMER_TimeAfter(s_uleepromWriteTick+DRV_I2C_ACCESS_SLEEP_TIME_MIN, tickNow) == VK_TIMER_RUNNING)
		{
			tickDiff = DRV_I2C_ACCESS_SLEEP_TIME_MIN - (tickNow - s_uleepromWriteTick);
			if (tickDiff > DRV_I2C_ACCESS_SLEEP_TIME_MIN)
				tickDiff = DRV_I2C_ACCESS_SLEEP_TIME_MIN;
			VK_TASK_Sleep(tickDiff);
		}
		TraceInfo(TRACE_MODULE_DI_EEPROM, "[DRV_EEPROM_WriteUncached] addr(%04X) size(%04X)\n", usAddress, ulSize);
		errCode = DRV_I2c_WriteA16(DRV_I2C_CHANNEL_EEPROM, DRV_EEPROM_CHIP_ADDR_WRITE, (HUINT16)usAddress, pucBuffer, count);
		if(errCode != DI_ERR_OK)
		{
			TraceError(TRACE_MODULE_DI_EEPROM, "%s->DRV_I2c_WriteA16(): Error Code(%d), Line(%d), %s\n", __FUNCTION__, errCode, __LINE__, __FILE__);
			VK_SEM_Release(s_eepromSema);
			return DI_ERR_ERROR;
		}
		s_uleepromWriteTick = VK_TIMER_GetSystemTick();

        TraceInfo(TRACE_MODULE_DI_EEPROM, "count=%d, usAddress=0x%x, pucBuffer=0x%x\n", count, usAddress, *pucBuffer);
        ulSize -= count;
        usAddress += count;
        pucBuffer += count;
    }
   	VK_SEM_Release(s_eepromSema);
    return DI_ERR_OK;
}


DI_ERR_CODE DRV_EEPROM_ReadUncached( HUINT32  usAddress,
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

  	VK_SEM_Get(s_eepromSema);

	HUINT32 tickNow, tickDiff;

	tickNow = VK_TIMER_GetSystemTick();
	if (VK_TIMER_TimeAfter(s_uleepromWriteTick+DRV_I2C_ACCESS_SLEEP_TIME_MIN, tickNow) == VK_TIMER_RUNNING)
	{
		tickDiff = DRV_I2C_ACCESS_SLEEP_TIME_MIN - (tickNow - s_uleepromWriteTick);
		if (tickDiff > DRV_I2C_ACCESS_SLEEP_TIME_MIN)
			tickDiff = DRV_I2C_ACCESS_SLEEP_TIME_MIN;
		VK_TASK_Sleep(tickDiff);
	}

	TraceInfo(TRACE_MODULE_DI_EEPROM, "[DRV_EEPROM_ReadUncached] addr(%04X) size(%04X) \n", usAddress, ulSize);
    errCode = DRV_I2c_ReadA16(DRV_I2C_CHANNEL_EEPROM, DRV_EEPROM_CHIP_ADDR_READ, (HUINT16)usAddress, pucBuffer, ulSize);
    if(errCode != DI_ERR_OK)
    {
        TraceError(TRACE_MODULE_DI_EEPROM, "%s->DRV_I2c_ReadA16(): Error Code(%d), Line(%d), %s\n", __FUNCTION__, errCode, __LINE__, __FILE__);
		VK_SEM_Release(s_eepromSema);
        return DI_ERR_ERROR;
    }

   	VK_SEM_Release(s_eepromSema);

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




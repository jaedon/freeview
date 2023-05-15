/**
 * drv_i2c.c
*/

/**
 * @defgroup		DRV_I2C DRV_I2C : DRV_I2C apis
 * @author		Jin-hong Kim
 * @note			DRV_I2C APIs
 * @brief			Define DRV_I2C APIs
 * @file 			drv_i2c.c
*/

/*******************************************************************/
/**************************** Header Files *************************/
/*******************************************************************/
/* os lib */
/*#include "b_os_lib.h" */

/* include  common utils headers */
#if defined(CONFIG_VK_STDLIB)
#else
#include 	<stdio.h>
#include 	<stdlib.h>
#include 	<string.h>
#endif
#include "linden_trace.h"

/* include  drv headers */
#include "drv_i2c.h"
#include "vkernel.h"
#include "taskdef.h"

/* include nexus api */
#include "nexus_platform.h"


/*******************************************************************/
/****************************** define *****************************/
/*******************************************************************/
#define DRV_I2C_RETRY_COUNT 10
#define DRV_I2C_ACCESS_SLEEP_TIME 5

/*******************************************************************/
/****************************** typedef ****************************/
/*******************************************************************/


/*******************************************************************/
/************************ global variables *************************/
/*******************************************************************/
static unsigned long i2c_sem[NEXUS_NUM_I2C_CHANNELS];

static NEXUS_I2cHandle i2cHandle[NEXUS_NUM_I2C_CHANNELS];

/*******************************************************************/
/************************ static variables *************************/
/*******************************************************************/


/*******************************************************************/
/*********************** Function Prototypes ***********************/
/*******************************************************************/


/*******************************************************************/
/*********************** Function Prototypes ***********************/
/*******************************************************************/

/*
    DRV_I2c_Init
    I2C를 초기화 한다.
*/
DRV_Error DRV_I2c_Init(void)
{
	NEXUS_PlatformConfiguration platformConfig;
	HUINT32						i;
	char						szSemaName[32];
	int							result;

	NEXUS_Platform_GetConfiguration(&platformConfig);

	VK_memcpy(i2cHandle, platformConfig.i2c, sizeof(NEXUS_I2cHandle)*NEXUS_NUM_I2C_CHANNELS );
	for (i=0; i<NEXUS_NUM_I2C_CHANNELS; i++)
	{
		VK_snprintf(szSemaName, 31, "i2c_sem_%d", i);
		result = VK_SEM_Create(&i2c_sem[i], szSemaName, DEFAULT_SUSPEND_TYPE);
		if (result != 0)
		{
			TraceError(TRACE_MODULE_DRV_I2C, "[DRV_I2c_Init] error in VK_SEM_Create\n");
		}
	}

	return DRV_OK;
}

DRV_Error DRV_I2c_ReadA24( HUINT32  ulI2cChannel,
                                        HUINT16 usChipAddr,
                                        HUINT32 usSubAddr,
                                        HUINT8 *  ucBuffer,
                                        HUINT32  ulLength
                                        )
{
	NEXUS_Error eNError = NEXUS_SUCCESS;
	DRV_Error	eDrvError = DRV_ERR;
	HINT32		i;

	VK_SEM_Get(i2c_sem[ulI2cChannel]);

	for (i=0; i<DRV_I2C_RETRY_COUNT; i++)
	{
		eNError = NEXUS_I2c_ReadA24(i2cHandle[ulI2cChannel], usChipAddr, usSubAddr, ucBuffer, ulLength);
		if(eNError == NEXUS_SUCCESS)
		{
			eDrvError = DRV_OK;
			break;
		}
		//TraceError(TRACE_MODULE_DRV_I2C, "%s->DRV_I2c_ReadA24(): ulI2cChannel(%d) I2C_Addr(%x) eNError(0x%08x), Line(%d), %s\n", __FUNCTION__, ulI2cChannel,usChipAddr , eNError, __LINE__, __FILE__);
		VK_TASK_Sleep(DRV_I2C_ACCESS_SLEEP_TIME);
	}

	VK_SEM_Release(i2c_sem[ulI2cChannel]);

	return eDrvError;
}

DRV_Error DRV_I2c_WriteA24( HUINT32  ulI2cChannel,
                                        HUINT16 usChipAddr,
                                        HUINT16 usSubAddr,
                                        HUINT8 *  ucBuffer,
                                        HUINT32  ulLength
                                        )
{
	NEXUS_Error eNError = NEXUS_SUCCESS;
	DRV_Error	eDrvError = DRV_ERR;
	HINT32		i;

	VK_SEM_Get(i2c_sem[ulI2cChannel]);

	for (i=0; i<DRV_I2C_RETRY_COUNT; i++)
	{
		eNError = NEXUS_I2c_WriteA16(i2cHandle[ulI2cChannel], usChipAddr, usSubAddr, ucBuffer, ulLength);
		if(eNError == NEXUS_SUCCESS)
		{
			eDrvError = DRV_OK;
			break;
		}
		TraceError(TRACE_MODULE_DRV_I2C, "%s->NEXUS_I2c_WriteA24(): ulI2cChannel(%d) eNError(0x%08x), Line(%d), %s\n", __FUNCTION__, ulI2cChannel, eNError, __LINE__, __FILE__);
		VK_TASK_Sleep(DRV_I2C_ACCESS_SLEEP_TIME);
	}

	VK_SEM_Release(i2c_sem[ulI2cChannel]);

	return eDrvError;
}


DRV_Error DRV_I2c_ReadA16( HUINT32  ulI2cChannel,
                                        HUINT16 usChipAddr,
                                        HUINT16 usSubAddr,
                                        HUINT8 *  ucBuffer,
                                        HUINT32  ulLength
                                        )
{
	NEXUS_Error eNError = NEXUS_SUCCESS;
	DRV_Error	eDrvError = DRV_ERR;
	HINT32		i;

	VK_SEM_Get(i2c_sem[ulI2cChannel]);

	for (i=0; i<DRV_I2C_RETRY_COUNT; i++)
	{
		eNError = NEXUS_I2c_ReadA16(i2cHandle[ulI2cChannel], usChipAddr, usSubAddr, ucBuffer, ulLength);
		if(eNError == NEXUS_SUCCESS)
		{
			eDrvError = DRV_OK;
			break;
		}
		TraceError(TRACE_MODULE_DRV_I2C, "%s->NEXUS_I2c_ReadA16(): ulI2cChannel(%d) eNError(0x%08x), Line(%d), %s\n", __FUNCTION__, ulI2cChannel, eNError, __LINE__, __FILE__);
		VK_TASK_Sleep(DRV_I2C_ACCESS_SLEEP_TIME);
	}

	VK_SEM_Release(i2c_sem[ulI2cChannel]);

	return eDrvError;
}

DRV_Error DRV_I2c_WriteA16( HUINT32  ulI2cChannel,
                                        HUINT16 usChipAddr,
                                        HUINT16 usSubAddr,
                                        HUINT8 *  ucBuffer,
                                        HUINT32  ulLength
                                        )
{
	NEXUS_Error eNError = NEXUS_SUCCESS;
	DRV_Error	eDrvError = DRV_ERR;
	HINT32		i;

	VK_SEM_Get(i2c_sem[ulI2cChannel]);

	for (i=0; i<DRV_I2C_RETRY_COUNT; i++)
	{
		eNError = NEXUS_I2c_WriteA16(i2cHandle[ulI2cChannel], usChipAddr, usSubAddr, ucBuffer, ulLength);
		if(eNError == NEXUS_SUCCESS)
		{
			eDrvError = DRV_OK;
			break;
		}
		TraceError(TRACE_MODULE_DRV_I2C, "%s->NEXUS_I2c_WriteA16(): ulI2cChannel(%d) eNError(0x%08x), Line(%d), %s\n", __FUNCTION__, ulI2cChannel, eNError, __LINE__, __FILE__);
		VK_TASK_Sleep(DRV_I2C_ACCESS_SLEEP_TIME);
	}

	VK_SEM_Release(i2c_sem[ulI2cChannel]);

	return eDrvError;
}

DRV_Error DRV_I2c_ReadA8_NoRetry( HUINT32  ulI2cChannel,
                                        HUINT16 usChipAddr,
                                        HUINT8 usSubAddr,
                                        HUINT8 *  ucBuffer,
                                        HUINT32  ulLength
                                        )
{
	NEXUS_Error eNError = NEXUS_SUCCESS;
	DRV_Error 	eDrvError = DRV_OK;

	VK_SEM_Get(i2c_sem[ulI2cChannel]);

	eNError = NEXUS_I2c_Read(i2cHandle[ulI2cChannel], usChipAddr, usSubAddr, ucBuffer, ulLength);
	if(eNError != NEXUS_SUCCESS)
	{
		eDrvError = DRV_ERR;
		TraceError(TRACE_MODULE_DRV_I2C, "%s->NEXUS_I2c_Read(): ulI2cChannel(%d) eNError(0x%08x), Line(%d), %s\n", __FUNCTION__, ulI2cChannel, eNError, __LINE__, __FILE__);
	}
	else
	{
		eDrvError = DRV_OK;
	}

	VK_SEM_Release(i2c_sem[ulI2cChannel]);

	return eDrvError;
}

DRV_Error DRV_I2c_ReadA8( HUINT32  ulI2cChannel,
                                        HUINT16 usChipAddr,
                                        HUINT8 usSubAddr,
                                        HUINT8 *  ucBuffer,
                                        HUINT32  ulLength
                                        )
{
	NEXUS_Error eNError = NEXUS_SUCCESS;
	DRV_Error	eDrvError = DRV_ERR;
	HINT32		i;

	VK_SEM_Get(i2c_sem[ulI2cChannel]);

	for (i=0; i<DRV_I2C_RETRY_COUNT; i++)
	{
		eNError = NEXUS_I2c_Read(i2cHandle[ulI2cChannel], usChipAddr, usSubAddr, ucBuffer, ulLength);
		if(eNError == NEXUS_SUCCESS)
		{
			eDrvError = DRV_OK;
			break;
		}
		TraceError(TRACE_MODULE_DRV_I2C, "%s->NEXUS_I2c_Read(): ulI2cChannel(%d) eNError(0x%08x), Line(%d), %s\n", __FUNCTION__, ulI2cChannel, eNError, __LINE__, __FILE__);
		VK_TASK_Sleep(DRV_I2C_ACCESS_SLEEP_TIME);
	}

	VK_SEM_Release(i2c_sem[ulI2cChannel]);

	return eDrvError;
}

DRV_Error DRV_I2c_WriteA8( HUINT32  ulI2cChannel,
                                    HUINT16 usChipAddr,
                                    HUINT8 usSubAddr,
                                    HUINT8 *  ucBuffer,
                                    HUINT32  ulLength
                                    )
{
	NEXUS_Error eNError = NEXUS_SUCCESS;
	DRV_Error	eDrvError = DRV_ERR;
	HINT32		i;

	VK_SEM_Get(i2c_sem[ulI2cChannel]);

	for (i=0; i<DRV_I2C_RETRY_COUNT; i++)
	{
		eNError = NEXUS_I2c_Write(i2cHandle[ulI2cChannel], usChipAddr, usSubAddr, ucBuffer, ulLength);
		if(eNError == NEXUS_SUCCESS)
		{
			eDrvError = DRV_OK;
			break;
		}
		TraceError(TRACE_MODULE_DRV_I2C, "%s->NEXUS_I2c_Write(): ulI2cChannel(%d) eNError(0x%08x), Line(%d), %s\n", __FUNCTION__, ulI2cChannel, eNError, __LINE__, __FILE__);
		VK_TASK_Sleep(DRV_I2C_ACCESS_SLEEP_TIME);
	}

	VK_SEM_Release(i2c_sem[ulI2cChannel]);

	return eDrvError;
}

DRV_Error DRV_I2c_WriteA8NoAck( HUINT32  ulI2cChannel,
                                    HUINT16 usChipAddr,
                                    HUINT8 usSubAddr,
                                    HUINT8 *  ucBuffer,
                                    HUINT32  ulLength
                                    )
{
	NEXUS_Error eNError = NEXUS_SUCCESS;
	DRV_Error	eDrvError = DRV_ERR;
	HINT32		i;

	VK_SEM_Get(i2c_sem[ulI2cChannel]);

	for (i=0; i<DRV_I2C_RETRY_COUNT; i++)
	{
		eNError = NEXUS_I2c_WriteNoAck(i2cHandle[ulI2cChannel], usChipAddr, usSubAddr, ucBuffer, ulLength);
		if(eNError == NEXUS_SUCCESS)
		{
			eDrvError = DRV_OK;
			break;
		}
		TraceError(TRACE_MODULE_DRV_I2C, "%s->NEXUS_I2c_Write(): ulI2cChannel(%d) eNError(0x%08x), Line(%d), %s\n", __FUNCTION__, ulI2cChannel, eNError, __LINE__, __FILE__);
		VK_TASK_Sleep(DRV_I2C_ACCESS_SLEEP_TIME);
	}

	VK_SEM_Release(i2c_sem[ulI2cChannel]);

	return eDrvError;
}


DRV_Error DRV_I2c_ReadNoAddr( HUINT32  ulI2cChannel,
                                        HUINT16 usChipAddr,
                                        HUINT8 *  ucBuffer,
                                        HUINT32  ulLength
                                        )
{
	NEXUS_Error eNError = NEXUS_SUCCESS;
	DRV_Error	eDrvError = DRV_ERR;
	HINT32		i;

	VK_SEM_Get(i2c_sem[ulI2cChannel]);

	for (i=0; i<DRV_I2C_RETRY_COUNT; i++)
	{
		eNError = NEXUS_I2c_ReadNoAddr(i2cHandle[ulI2cChannel], usChipAddr, ucBuffer, ulLength);
		if(eNError == NEXUS_SUCCESS)
		{
			eDrvError = DRV_OK;
			break;
		}
		TraceError(TRACE_MODULE_DRV_I2C, "%s->NEXUS_I2c_ReadNoAddr(): ulI2cChannel(%d) eNError(0x%08x), Line(%d), %s\n", __FUNCTION__, ulI2cChannel, eNError, __LINE__, __FILE__);
		VK_TASK_Sleep(DRV_I2C_ACCESS_SLEEP_TIME);
	}

	VK_SEM_Release(i2c_sem[ulI2cChannel]);

	return eDrvError;
}

DRV_Error DRV_I2c_WriteNoAddr( HUINT32  ulI2cChannel,
                                        HUINT16 usChipAddr,
                                        HUINT8 *  ucBuffer,
                                        HUINT32  ulLength
                                        )
{
	NEXUS_Error eNError = NEXUS_SUCCESS;
	DRV_Error	eDrvError = DRV_ERR;
	HINT32		i;

	VK_SEM_Get(i2c_sem[ulI2cChannel]);

	for (i=0; i<DRV_I2C_RETRY_COUNT; i++)
	{
		eNError = NEXUS_I2c_WriteNoAddr(i2cHandle[ulI2cChannel], usChipAddr, ucBuffer, ulLength);
		if(eNError == NEXUS_SUCCESS)
		{
			eDrvError = DRV_OK;
			break;
		}
		TraceError(TRACE_MODULE_DRV_I2C, "%s->NEXUS_I2c_WriteNoAddr(): eNError(0x%08x), Line(%d), %s\n", __FUNCTION__, eNError, __LINE__, __FILE__);
		VK_TASK_Sleep(DRV_I2C_ACCESS_SLEEP_TIME);
	}

	VK_SEM_Release(i2c_sem[ulI2cChannel]);

	return eDrvError;
}

DRV_Error DRV_I2c_SetClock( HUINT32  ulI2cChannel, DRV_I2C_CLOCK_E eClock)
{
	NEXUS_Error	nexus_error;
	NEXUS_I2cSettings I2cSettings;

	VK_SEM_Get(i2c_sem[ulI2cChannel]);

	NEXUS_I2c_GetSettings(i2cHandle[ulI2cChannel], &I2cSettings);

	/* convert DRV_I2C_CLOCK_E to NEXUS_I2cClockRate */
	switch( eClock )
	{
		case DRV_I2C_CLOCK_47K :
			I2cSettings.clockRate = NEXUS_I2cClockRate_e47Khz;
			break;
		case DRV_I2C_CLOCK_50K :
			I2cSettings.clockRate = NEXUS_I2cClockRate_e50Khz;
			break;
		case DRV_I2C_CLOCK_93K :
			I2cSettings.clockRate = NEXUS_I2cClockRate_e93Khz;
			break;
		case DRV_I2C_CLOCK_100K :
			I2cSettings.clockRate = NEXUS_I2cClockRate_e100Khz;
			break;
		case DRV_I2C_CLOCK_187K :
			I2cSettings.clockRate = NEXUS_I2cClockRate_e187Khz;
			break;
		case DRV_I2C_CLOCK_200K :
			I2cSettings.clockRate = NEXUS_I2cClockRate_e200Khz;
			break;
		case DRV_I2C_CLOCK_375K :
			I2cSettings.clockRate = NEXUS_I2cClockRate_e375Khz;
			break;
		case DRV_I2C_CLOCK_400K :
			I2cSettings.clockRate = NEXUS_I2cClockRate_e400Khz;
			break;
		default :
			I2cSettings.clockRate = NEXUS_I2cClockRate_e100Khz;
	}

	nexus_error = NEXUS_I2c_SetSettings(i2cHandle[ulI2cChannel], &I2cSettings);

	VK_SEM_Release(i2c_sem[ulI2cChannel]);

	if (nexus_error != NEXUS_SUCCESS)
	{
		TraceError(TRACE_MODULE_DRV_I2C, "[DRV_I2c_SetClock] error %08X : NEXUS_I2c_SetSettings\n", nexus_error);
		return (DRV_Error)nexus_error;
	}

	return DRV_OK;
}



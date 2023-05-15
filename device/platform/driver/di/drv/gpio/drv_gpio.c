/*
 * $Header:   $
 */
/*********************************************************************
 * $Workfile:   drv_gpio.c  $
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
/* include  common utils headers */
#include <string.h>
#include "linden_trace.h"
#include "vkernel.h"

/* include  drv headers */
#include "drv_gpio.h"
#include "drv_pm.h"
#include "nexus_platform_features.h"

/*******************************************************************/
/****************************** define *****************************/
/*******************************************************************/
#define DEBUG_MODULE			TRACE_MODULE_DRV_GPIO


/*******************************************************************/
/************************ global variables *************************/
/*******************************************************************/
extern GPIO_ConfigInfo_t GPIO_ConfigInfoTable[];
//extern GPIO_TSConfigInfo_t GPIO_TSConfigInfoTable[];
extern HUINT32 DRV_CFG_GetGPIOMaxDevice(void);
//extern HUINT32 DRV_CFG_Get_Max_TS_OUTPUT_Number(void);

extern void NEXUS_Platform_ChangePinmux(unsigned char ucPioNum, unsigned char ucFuncType);

/*******************************************************************/
/************************ static variables *************************/
/*******************************************************************/
static HINT32 s_gpio_max_num = 0;
static GPIO_ConfigInfo_t	*pgpio_config;

/* drv.h 에서 conditional define 사용 하지 않는 대신 enum값과 ID 사이의 map table을 사용 한다. */
static int s_gio_maptable[GPIO_ID_MAX];

/*******************************************************************/
/****************** Static Function Prototypes *********************/
/*******************************************************************/

/*******************************************************************/
/****************** Global Function Prototypes *********************/
/*******************************************************************/
DRV_Error DRV_GPIO_Init(void)
{
	HINT32	returnCode;
	HUINT32 gpio_id;
	HINT32 table_index;
	NEXUS_GpioSettings gpioSettings;


	for(table_index=0; table_index<GPIO_ID_MAX; table_index++)
	{
		s_gio_maptable[ table_index ] = -1; /*Use -1 as meaning of unused gpio id*/
	}

	s_gpio_max_num = DRV_CFG_GetGPIOMaxDevice();
	pgpio_config = (GPIO_ConfigInfo_t *)DD_MEM_Alloc(s_gpio_max_num*sizeof(GPIO_ConfigInfo_t));
	VK_memset(pgpio_config, 0, s_gpio_max_num*sizeof(GPIO_ConfigInfo_t));

	for(table_index=0; table_index<s_gpio_max_num; table_index++)
	{
		/*  drv_gio.h 에 정의 된  GPIO_ID_t에는 conditional define 사용하지 않음. */
#if 0
		if(gpio_id == GPIO_ConfigInfoTable[gpio_id].gpio_id)
		{
			pgpio_config[gpio_id] = GPIO_ConfigInfoTable[gpio_id];
		}
		else
		{
			PrintError("gpio_id = %d,  GPIO_ConfigInfoTable[%d].gpio_pinId = %d,  %s(%d)\n", gpio_id, gpio_id, GPIO_ConfigInfoTable[gpio_id].gpio_id);
			return DRV_ERR;
		}
#endif
		pgpio_config[table_index] = GPIO_ConfigInfoTable[table_index];

		gpio_id = pgpio_config[table_index].gpio_id;
		if( gpio_id >= GPIO_ID_MAX)
		{
			PrintError("gpio_id is over range (%d > %d)!!!\n", gpio_id, GPIO_ID_MAX);
			return DRV_ERR;
		}

		s_gio_maptable[ gpio_id ] = table_index;

		if( pgpio_config[table_index].gpio_Mode != GPIO_MODE_INVALID )
		{
			NEXUS_Gpio_GetDefaultSettings(pgpio_config[table_index].gpio_type, &gpioSettings);
			gpioSettings.mode = pgpio_config[table_index].gpio_Mode;
			gpioSettings.interruptMode = pgpio_config[table_index].gpio_IntrMode;
#if defined(CONFIG_TEMP_YSR2000)
			if((pgpio_config[table_index].gpio_init_state != GPIO_INVAILD)&&(pgpio_config[table_index].gpio_Mode == GPIO_MODE_WRITE))
			{
				gpioSettings.value = pgpio_config[table_index].gpio_init_state;
			}
#endif			
			pgpio_config[table_index].gpio_handle = NEXUS_Gpio_Open(pgpio_config[table_index].gpio_type, pgpio_config[table_index].gpio_number, &gpioSettings);
			if(NULL == pgpio_config[table_index].gpio_handle)
			{
				PrintError("NEXUS_Gpio_Open is NULL!!!\n");
				return DRV_ERR;
			}

			if(pgpio_config[table_index].gpio_init_state != GPIO_INVAILD)
			{
				returnCode = DRV_GPIO_Write(gpio_id, pgpio_config[table_index].gpio_init_state);
				if(DRV_OK != returnCode)
				{
					PrintError("DRV_GPIO_Write Error!!!\n");
				}
			}
		}
	}

	return DRV_OK;
}

#if defined(CONFIG_BRCM_MOD_NEXUS_POWER_MANAGEMENT)
static DRV_Error P_GPIO_Reinit(void)
{
	HUINT32 gpio_id;
	HINT32 idx;
	NEXUS_GpioSettings gpioSettings;
	NEXUS_Error eNexusErr;
	DRV_Error eDrvErr = DRV_ERR;

	for(idx=0; idx<s_gpio_max_num; idx++)
	{
		gpio_id = pgpio_config[idx].gpio_id;
		DONE_ERR((gpio_id>=GPIO_ID_MAX));

		if(pgpio_config[idx].gpio_type == NEXUS_GpioType_eAonStandard)
		{
			/*GPIO in AON block don't need  reinitialization. skip it.*/
			continue;
		}

		if( pgpio_config[idx].gpio_Mode != GPIO_MODE_INVALID )
		{
			NEXUS_Gpio_GetSettings(pgpio_config[idx].gpio_handle, &gpioSettings);
			gpioSettings.mode = pgpio_config[idx].gpio_Mode;
			gpioSettings.interruptMode = pgpio_config[idx].gpio_IntrMode;
			gpioSettings.value = (pgpio_config[idx].gpio_init_state==GPIO_INVAILD)?GPIO_LOW:pgpio_config[idx].gpio_init_state;

			eNexusErr = NEXUS_Gpio_SetSettings(pgpio_config[idx].gpio_handle, &gpioSettings);
			DONE_ERR((eNexusErr!=NEXUS_SUCCESS));
		}
	}

	eDrvErr = DRV_OK;
done:
	return eDrvErr;
}

DRV_Error DRV_GPIO_PM_Suspend(DRV_GPIO_STANDBY_STATE_E state)
{
	DRV_Error eDrvErr = DRV_ERR;
	NEXUS_Error eNexusErr;
	HINT32 idx;
	NEXUS_GpioSettings gpioSettings;
	
	switch(state) {
	case DRV_GPIO_STANDBY_STATE_LIGHT_SLEEP:
		/* clear interrupt mode for standard gpio.
		   in S2 mode, gpio block is alive. so chip will be awaken by interrupt of standard gpio
		*/
		for (idx=0; idx<s_gpio_max_num; idx++)
		{
			if ((pgpio_config[idx].gpio_Mode != GPIO_MODE_INVALID)
				&& (pgpio_config[idx].gpio_type == NEXUS_GpioType_eStandard)
			)
			{
				NEXUS_Gpio_GetSettings(pgpio_config[idx].gpio_handle, &gpioSettings);
				gpioSettings.mode = GPIO_MODE_READ;
				gpioSettings.interruptMode = GPIO_INT_MODE_DISABLED;
				eNexusErr = NEXUS_Gpio_SetSettings(pgpio_config[idx].gpio_handle, &gpioSettings);
				DONE_ERR((eNexusErr!=NEXUS_SUCCESS));
			}
		}
		break;
	default:
		break;
	}

	eDrvErr = DRV_OK;
done:
	return eDrvErr;
}

DRV_Error DRV_GPIO_PM_Resume(void)
{
	DRV_Error eDrvErr = DRV_ERR;

	eDrvErr = P_GPIO_Reinit();
	DONE_ERR((eDrvErr!=DRV_OK));
	
	eDrvErr = DRV_OK;
done:
	return eDrvErr;
}
#endif

DRV_Error DRV_GPIO_Read(GPIO_ID_t gpio_id, GPIO_STATE_t *val)
{
	HINT32 table_index;
	NEXUS_GpioStatus gpio_status;
	NEXUS_Error errCode = NEXUS_SUCCESS;

	if (NULL == val)
	{
		PrintError("DRV_GPIO_Read Error!!!\n");
		return DRV_ERR;
	}

	if (gpio_id >= GPIO_ID_MAX)
	{
		PrintError("DRV_GPIO_Read Error!!!\n");
		return DRV_ERR;
	}

	table_index = s_gio_maptable[ gpio_id ];
	if (table_index == -1 )
	{
		PrintError("gpid id(%d) in not initialized!!!\n", gpio_id);
		return DRV_ERR_INITIALIZATION;
	}
	else if (table_index >= s_gpio_max_num )
	{
		PrintError("table_index over range!!!\n");
		return DRV_ERR;
	}

	errCode = NEXUS_Gpio_GetStatus(pgpio_config[table_index].gpio_handle, &gpio_status);
	if(NEXUS_SUCCESS != errCode)
	{
		PrintError("NEXUS_Gpio_GetStatus Error!!!\n");
		return DRV_ERR;
	}

	*val = gpio_status.value;

	return DRV_OK;
}

DRV_Error DRV_GPIO_Write(GPIO_ID_t gpio_id, GPIO_STATE_t val)
{
	HINT32 table_index;
	NEXUS_Error ErrCode = NEXUS_SUCCESS;
	NEXUS_GpioSettings gpioSettings;

	if (gpio_id >= GPIO_ID_MAX)
	{
		PrintError("DRV_GPIO_Write failure!!!, gpio_id = %d\n", gpio_id);
		return DRV_ERR;
	}

	table_index = s_gio_maptable[ gpio_id ];

	if (table_index == -1 )
	{
		PrintError("gpid id(%d) in not initialized!!!\n", gpio_id);
		return DRV_ERR_INITIALIZATION;
	}
	else if (table_index >= s_gpio_max_num)
	{
		PrintError("table_index over range!!!\n");
		return DRV_ERR;
	}

	if (pgpio_config[table_index].gpio_Mode == GPIO_MODE_WRITE)
	{
		NEXUS_Gpio_GetSettings(pgpio_config[table_index].gpio_handle, &gpioSettings);
		gpioSettings.value = val;
		ErrCode = NEXUS_Gpio_SetSettings(pgpio_config[table_index].gpio_handle, &gpioSettings);
		if(NEXUS_SUCCESS != ErrCode)
		{
			PrintError("NEXUS_Gpio_SetSettings Error!! ErrCode = %d\n", ErrCode);
			return DRV_ERR;
		}
	}
	else
	{
		PrintError("Gpio Mode is not Write Mode!!! Gpio Id = %d\n", table_index);
		return DRV_ERR;
	}

	return DRV_OK;
}

DRV_Error DRV_GPIO_ChangePinmux(GPIO_ID_t gpio_id, GPIO_PINMUX_FUNCTION eType)
{
	HINT32 table_index;


	if (gpio_id >= GPIO_ID_MAX)
	{
		PrintError("DRV_GPIO_ChangePinmux failure!!!, gpio_id = %d\n", gpio_id);
		return DRV_ERR;
	}

	if( eType >= GPIO_PINMUX_MAX )
	{
		PrintError("Invalid pinmux type(%d)\n", eType);
		return DRV_ERR;
	}

	table_index = s_gio_maptable[ gpio_id ];

	if (table_index == -1 )
	{
		PrintError("gpid id(%d) in not initialized!!!\n", gpio_id);
		return DRV_ERR_INITIALIZATION;
	}
	else if (table_index >= s_gpio_max_num)
	{
		PrintError("table_index over range!!!\n");
		return DRV_ERR;
	}

	PrintDebug("[DRV_GPIO_ChangePinmux] pin %d, type %d\n", pgpio_config[table_index].gpio_number, (unsigned char)eType);

/* Di 2.0 does not use this function. - avoid compile error. - hykim2
	NEXUS_Platform_ChangePinmux(pgpio_config[table_index].gpio_number, (unsigned char)eType);
*/
	return DRV_OK;
}

DRV_Error DRV_GPIO_INTR_Enable(GPIO_ID_t gpio_id)
{
	HINT32 table_index;
	NEXUS_Error ErrCode = NEXUS_SUCCESS;
	NEXUS_GpioSettings gpioSettings;

	if (gpio_id >= GPIO_ID_MAX)
	{
		PrintError("DDI_GPIO_read failure!!!)\n");
		return DRV_ERR;
	}

	table_index = s_gio_maptable[ gpio_id ];

	if (table_index == -1 )
	{
		PrintError("gpid id(%d) in not initialized!!!\n", gpio_id);
		return DRV_ERR_INITIALIZATION;
	}
	else if (table_index >= s_gpio_max_num)
	{
		PrintError("table_index over range!!!\n");
		return DRV_ERR;
	}

	if (pgpio_config[table_index].gpio_Mode == GPIO_MODE_INTERRUPT)
	{
		NEXUS_Gpio_GetSettings(pgpio_config[table_index].gpio_handle, &gpioSettings);
		gpioSettings.interruptMode = pgpio_config[table_index].gpio_IntrMode;
		ErrCode = NEXUS_Gpio_SetSettings(pgpio_config[table_index].gpio_handle, &gpioSettings);
		if(NEXUS_SUCCESS != ErrCode)
		{
			PrintError("NEXUS_Gpio_SetSettings Error!! ErrCode = %d\n", ErrCode);
			return DRV_ERR;
		}
	}
	else
	{
		PrintError("Gpio Mode is not interrupt mode\n");
		return DRV_ERR;
	}

	return DRV_OK;
}

DRV_Error DRV_GPIO_INTR_Disable(GPIO_ID_t gpio_id)
{
	HINT32 table_index;
	NEXUS_Error ErrCode = NEXUS_SUCCESS;
	NEXUS_GpioSettings gpioSettings;

	if (gpio_id >= GPIO_ID_MAX)
	{
		PrintError("DDI_GPIO_read failure!!!)\n");
		return DRV_ERR;
	}

	table_index = s_gio_maptable[ gpio_id ];

	if (table_index == -1 )
	{
		PrintError("gpid id(%d) in not initialized!!!\n", gpio_id);
		return DRV_ERR_INITIALIZATION;
	}
	else if (table_index >= s_gpio_max_num)
	{
		PrintError("table_index over range!!!\n");
		return DRV_ERR;
	}

	if (pgpio_config[table_index].gpio_Mode == GPIO_MODE_INTERRUPT)
	{
		NEXUS_Gpio_GetSettings(pgpio_config[table_index].gpio_handle, &gpioSettings);
		gpioSettings.interruptMode = GPIO_INT_MODE_DISABLED;
		ErrCode = NEXUS_Gpio_SetSettings(pgpio_config[table_index].gpio_handle, &gpioSettings);
		if(NEXUS_SUCCESS != ErrCode)
		{
			PrintError("NEXUS_Gpio_SetSettings Error!! ErrCode = %d\n");
			return DRV_ERR;
		}
	}
	else
	{
		PrintError("Gpio Mode is not interrupt mode\n");
		return DRV_ERR;
	}

	return DRV_OK;
}

DRV_Error DRV_GPIO_INTR_Clear(GPIO_ID_t gpio_id)
{
	HINT32 table_index;
	NEXUS_Error ErrCode = NEXUS_SUCCESS;

	if (gpio_id >= GPIO_ID_MAX)
	{
		PrintError("DDI_GPIO_read failure!!!)\n");
		return DRV_ERR;
	}

	table_index = s_gio_maptable[ gpio_id ];

	if (table_index == -1 )
	{
		PrintError("gpid id(%d) in not initialized!!!\n", gpio_id);
		return DRV_ERR_INITIALIZATION;
	}
	else if (table_index >= s_gpio_max_num)
	{
		PrintError("table_index over range!!!\n");
		return DRV_ERR;
	}

	ErrCode = NEXUS_Gpio_ClearInterrupt(pgpio_config[table_index].gpio_handle);
	if(NEXUS_SUCCESS != ErrCode)
	{
		PrintError("NEXUS_Gpio_SetSettings Error!! ErrCode = %d\n");
		return DRV_ERR;
	}

	return DRV_OK;
}

DRV_Error DRV_GPIO_INTR_RegisterEventNotify(
						GPIO_ID_t gpio_id,
						GPIO_CALLBACK_t pfnGpioEventCallback,
						void *pParam1, int pParam2)
{
	HINT32 table_index;
	NEXUS_Error ErrCode = NEXUS_SUCCESS;
	NEXUS_GpioSettings gpioSettings;

	if (gpio_id >= GPIO_ID_MAX)
	{
		PrintError("DDI_GPIO_read failure!!!)\n");
		return DRV_ERR;
	}

	table_index = s_gio_maptable[ gpio_id ];

	if (table_index == -1 )
	{
		PrintError("gpid id(%d) in not initialized!!!\n", gpio_id);
		return DRV_ERR_INITIALIZATION;
	}
	else if (table_index >= s_gpio_max_num)
	{
		PrintError("table_index over range!!!\n");
		return DRV_ERR;
	}

	if (pgpio_config[table_index].gpio_Mode == GPIO_MODE_INTERRUPT)
	{
		NEXUS_Gpio_GetSettings(pgpio_config[table_index].gpio_handle, &gpioSettings);
		gpioSettings.interrupt.callback = pfnGpioEventCallback;
		gpioSettings.interrupt.context = pParam1;
		gpioSettings.interrupt.param = pParam2;
		ErrCode = NEXUS_Gpio_SetSettings(pgpio_config[table_index].gpio_handle, &gpioSettings);
		if(NEXUS_SUCCESS != ErrCode)
		{
			PrintError("NEXUS_Gpio_SetSettings Error!! ErrCode = %d\n", ErrCode);
			return DRV_ERR;
		}
	}
	else
	{
		PrintError("Gpio Mode is not interrupt mode\n");
		return DRV_ERR;
	}

	return DRV_OK;
}

DRV_Error DRV_GPIO_ChangeMode(GPIO_ID_t gpio_id, GPIO_MODE_t eMode, GPIO_STATE_t eState)
{
	HINT32 table_index;
	NEXUS_Error ErrCode = NEXUS_SUCCESS;
	NEXUS_GpioSettings gpioSettings;

	if (gpio_id >= GPIO_ID_MAX)
	{
		PrintError("DRV_GPIO_Write failure!!!, gpio_id = %d\n", gpio_id);
		return DRV_ERR;
	}

	table_index = s_gio_maptable[ gpio_id ];

	if (table_index == -1 )
	{
		PrintError("gpid id(%d) in not initialized!!!\n", gpio_id);
		return DRV_ERR_INITIALIZATION;
	}
	else if (table_index >= s_gpio_max_num)
	{
		PrintError("table_index over range!!!\n");
		return DRV_ERR;
	}

	NEXUS_Gpio_GetSettings(pgpio_config[table_index].gpio_handle, &gpioSettings);
	gpioSettings.mode = eMode;
	gpioSettings.value = eState;
	ErrCode = NEXUS_Gpio_SetSettings(pgpio_config[table_index].gpio_handle, &gpioSettings);
	if(NEXUS_SUCCESS != ErrCode)
	{
		PrintError("NEXUS_Gpio_SetSettings Error!! ErrCode = %d\n", ErrCode);
		return DRV_ERR;
	}

	pgpio_config[table_index].gpio_Mode = eMode;

	return DRV_OK;
}

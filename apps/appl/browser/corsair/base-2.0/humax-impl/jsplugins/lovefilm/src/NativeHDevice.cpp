/*************************************************************************************************************
	File 		: NativeHDevice.cpp
	author 		: STB-Component (tstbcomp@humaxdigital.com)
	comment		: 
	date    	: 2013/10/21
	attention 	: 

	(c) 2011-2013 Humax Co., Ltd. 
	This program is produced by Humax Co., Ltd. (¡°Humax¡±) and 
	the proprietary Software of Humax and its licensors. Humax provides you, as an Authorized Licensee, 
	non-assignable, non-transferable and non-exclusive license to use this Software. 
	You acknowledge that this Software contains valuable trade secrets of Humax and by using this Software 
	you agree to the responsibility to take all reasonable efforts to protect the any information 
	you receive from Humax. You are not permitted to duplicate, modify, distribute, sell or lease and 
	reverse engineer or extract the source code of this Software unless you have Humax's written permission 
	to do so.
	If you have no authorized license, discontinue using this Software immediately. 
	
	THE SOFTWARE IS PROVIDED ¡°AS IS¡± AND HUMAX MAKES NO PROMISES, REPRESENTATIONS OR WARRANTIES, 
	EITHER EXPRESS, IMPLIED OR STATUTORY, OR OTHERWISE, WITH RESPECT TO THE SOFTWARE. 
	IN NO EVENT SHALL HUMAX BE LIABLE FOR LOST PROFITS, REVENUES, OR DATA, FINANCIAL LOSSES OR 
	INDIRECT, SPECIAL, CONSEQUENTIAL, EXEMPLARTY OR PUNITIVE DAMAGES WHATSOEVER RELATING TO YOUR USE OR 
	INABILITY TO USE THE SOFTWARE.
	
	This License is effective until terminated. You may terminate this License at any time 
	by destroying all copies of the Software including all documentation. This License will terminate 
	immediately without notice from Humax to you if you fail to comply with any provision of this License. 
	Upon termination, you must destroy all copies of the Software and all documentation.  
	
	The laws of the Republic of Korea will apply to any disputes arising out of or relating to 
	this Copyright Notice. All claims arising out of or relating to this Copyright Notice will be litigated
	in the Seoul Central District Court, in the Republic of Korea.
*************************************************************************************************************/


#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include "NativeHDevice.h"
#include "macros.h"

/* Begin of Temporary */
//#include "OplDevice.h"
typedef void*	OplDeviceEventData;

typedef enum 
{
	eOPL_DEVICE_CONN_CHANGED,
	eOPL_DEVICE_ERROR,
	eOPL_DEVICE_FOCUS_CHANGED,
	eOPL_DEVICE_APPWILL_EXIT,
	eOPL_DEVICE_MEM_WARNING,
} OPL_DEVICE_EVENT_e;
/* End of Temporary */

typedef struct __DEVICE_ERROR__
{
	char category[128];
	char type[128];
	char module[128];
	char code[32];
	char description[128];
	char stackTrace[128];	
} DEVICE_ERROR_st;

static DEVICE_ERROR_st	   g_lastError = {"","","","","",""};
static DeviceEventListener g_callback;
/*****************************************************************************
 * HDevice
 ***************************************************************************** */
/* Event Callback */
void		   native_Device_Listener(int event, OplDeviceEventData data)
{
	switch(event)
	{	
		/* Convert OPL to Native */
		// OplDeviceEventData -> DeviceEventHandle
		case eOPL_DEVICE_CONN_CHANGED:
			break;
		case eOPL_DEVICE_ERROR:
			// @@TT:KTH Cache LastError
			// Convert DeviceEventHandle -> DEVICE_ERROR_st
			
			break;
		case eOPL_DEVICE_FOCUS_CHANGED:
			break;
		case eOPL_DEVICE_APPWILL_EXIT:
			break;
		case eOPL_DEVICE_MEM_WARNING:
			break;
		default:
			break;
	}
	g_callback(event, data);
}

NJSEReturnCode native_Device_RegEventListener(DeviceEventListener listener)
{
	g_callback = listener;
	return NJSE_RETURN_OK;	
}

NJSEReturnCode native_Device_UnRegEventListener( void )
{
	g_callback = NULL;
	return NJSE_RETURN_OK;		
}

/* Method */
NJSEReturnCode native_Device_SetTime(const NJSENumber timeStamp)
{
	NJSEReturnCode eRet = NJSE_RETURN_OK;

	return eRet;
}
NJSEReturnCode native_Device_SetMemoryWarningLevel( const NJSENumber levelBytes )
{
	NJSEReturnCode eRet = NJSE_RETURN_OK;

	return eRet;
}

NJSEReturnCode native_Device_Gc( void )
{
	NJSEReturnCode eRet = NJSE_RETURN_OK;

	return eRet;
}

NJSEReturnCode native_Device_Exit( void )
{
	NJSEReturnCode eRet = NJSE_RETURN_OK;

	return eRet;
}

/* Properties */
NJSEReturnCode native_Device_InterfaceVersion( const char **interface_version )
{
	NJSEReturnCode eRet = NJSE_RETURN_OK;

	return eRet;
}

NJSEReturnCode native_Device_Get_FirmwareVersion( const char **firmware_version )
{
	NJSEReturnCode eRet = NJSE_RETURN_OK;

	return eRet;
}

NJSEReturnCode native_Device_Get_OperatingSystem( const char **operating_system )
{
	NJSEReturnCode eRet = NJSE_RETURN_OK;

	return eRet;
}

NJSEReturnCode native_Device_Get_Manufacturer( const char **manufacturer )
{
	NJSEReturnCode eRet = NJSE_RETURN_OK;

	return eRet;
}

NJSEReturnCode natvie_Device_Get_ModelName( const char **model_name )
{
	NJSEReturnCode eRet = NJSE_RETURN_OK;

	return eRet;
}

NJSEReturnCode natvie_Device_Get_Chipset( const char **chipset )
{
	NJSEReturnCode eRet = NJSE_RETURN_OK;

	return eRet;
}

NJSEReturnCode natvie_Device_Get_Uuid( const char **uuid )
{
	NJSEReturnCode eRet = NJSE_RETURN_OK;

	return eRet;
}

NJSEReturnCode natvie_Device_Get_ClientIp( const char **client_ip)
{
	NJSEReturnCode eRet = NJSE_RETURN_OK;

	return eRet;
}

NJSEReturnCode natvie_Device_Get_ClientTime( const char **client_time )
{
	NJSEReturnCode eRet = NJSE_RETURN_OK;

	return eRet;
}

NJSEReturnCode natvie_Device_Get_ConnectionType( const char **connection_type )
{
	NJSEReturnCode eRet = NJSE_RETURN_OK;

	return eRet;
}

NJSEReturnCode natvie_Device_Get_ConnectionStrength( int *connection_strength )
{
	NJSEReturnCode eRet = NJSE_RETURN_OK;

	return eRet;
}


NJSEReturnCode natvie_Device_Get_AudioCapability( NJSENumber *size,  const char **audio_capability )
{
	NJSEReturnCode eRet = NJSE_RETURN_OK;

	return eRet;
}


NJSEReturnCode natvie_Device_Get_CpuLoad( NJSENumber *cpu_load )
{
	NJSEReturnCode eRet = NJSE_RETURN_OK;

	return eRet;
}

NJSEReturnCode natvie_Device_Get_MemoryTotal( NJSENumber *memory_total )
{
	NJSEReturnCode eRet = NJSE_RETURN_OK;

	return eRet;
}
	
NJSEReturnCode natvie_Device_Get_MemoryUsed( NJSENumber *memory_used )
{
	NJSEReturnCode eRet = NJSE_RETURN_OK;

	return eRet;
}

NJSEReturnCode native_Device_Get_MemoryAvailable( NJSENumber *memory_available )
{
	NJSEReturnCode eRet = NJSE_RETURN_OK;
	/* Available = Total - Used */
	return eRet;
}

NJSEReturnCode natvie_Device_Get_DiskStorageTotal( NJSENumber *disk_storage__total )
{
	NJSEReturnCode eRet = NJSE_RETURN_OK;

	return eRet;
}
	
NJSEReturnCode natvie_Device_Get_DiskStorageUsed( NJSENumber *disk_storage_used )
{
	NJSEReturnCode eRet = NJSE_RETURN_OK;

	return eRet;
}

NJSEReturnCode natvie_Device_Get_AppWidth( NJSENumber *app_width )
{
	NJSEReturnCode eRet = NJSE_RETURN_OK;

	return eRet;
}

NJSEReturnCode natvie_Device_Get_AppHeight( NJSENumber *app_height )
{
	NJSEReturnCode eRet = NJSE_RETURN_OK;

	return eRet;
}

NJSEReturnCode natvie_Device_Get_ScreenWidth( NJSENumber *screen_width )
{
	NJSEReturnCode eRet = NJSE_RETURN_OK;

	return eRet;
}

NJSEReturnCode natvie_Device_Get_ScreenHeight( NJSENumber *screen_height )
{
	NJSEReturnCode eRet = NJSE_RETURN_OK;

	return eRet;
}

NJSEReturnCode natvie_Device_Get_LastError( DeviceEventError *error )
{
	NJSEReturnCode eRet = NJSE_RETURN_OK;
	*error = (DeviceEventError)&g_lastError;
	return eRet;
}

/*****************************************************************************
 * HError
 ***************************************************************************** */
/* Properties */
NJSEReturnCode native_Device_GetHasFocus( DeviceEventHandle identifier, NJSEBoolean *retval )
{
	NJSEReturnCode eRet = NJSE_RETURN_OK;
	if(identifier)
	{
	
	}
	else
	{
		eRet = NJSE_RETURN_VALUE_NULL;
	}	
	return eRet;
}

NJSEReturnCode native_Device_GetAppName( DeviceEventHandle identifier, const char **retval )
{
	NJSEReturnCode eRet = NJSE_RETURN_OK;
	if(identifier)
	{
	
	}
	else
	{
		eRet = NJSE_RETURN_VALUE_NULL;
	}	
	return eRet;
}

NJSEReturnCode native_Device_GetCategory( DeviceEventError identifier, const char **retval )
{
	NJSEReturnCode eRet = NJSE_RETURN_OK;
	if(identifier)
	{
	
	}
	else
	{
		eRet = NJSE_RETURN_VALUE_NULL;
	}	
	return eRet;
}
NJSEReturnCode native_Device_GetType( DeviceEventError identifier, const char **retval )
{
	NJSEReturnCode eRet = NJSE_RETURN_OK;
	if(identifier)
	{
	
	}
	else
	{
		eRet = NJSE_RETURN_VALUE_NULL;
	}	
	return eRet;
}
NJSEReturnCode native_Device_GetModule( DeviceEventError identifier, const char **retval )
{
	NJSEReturnCode eRet = NJSE_RETURN_OK;

	return eRet;
}
NJSEReturnCode native_Device_GetCode( DeviceEventError identifier, const char **retval )
{
	NJSEReturnCode eRet = NJSE_RETURN_OK;
	if(identifier)
	{
	
	}
	else
	{
		eRet = NJSE_RETURN_VALUE_NULL;
	}	
	return eRet;
}
NJSEReturnCode native_Device_GetDescription( DeviceEventError identifier, const char **retval )
{
	NJSEReturnCode eRet = NJSE_RETURN_OK;

	return eRet;
}
NJSEReturnCode native_Device_GetStackTrace( DeviceEventError identifier, const char **retval )
{
	NJSEReturnCode eRet = NJSE_RETURN_OK;
	if(identifier)
	{
	
	}
	else
	{
		eRet = NJSE_RETURN_VALUE_NULL;
	}	
	return eRet;
}

NJSEReturnCode native_Device_ReleaseErrorHandle( DeviceEventError id )
{
	NJSEReturnCode eRet = NJSE_RETURN_OK;
	if(id)
	{
	
	}
	else
	{
		eRet = NJSE_RETURN_VALUE_NULL;
	}	
	return eRet;
}


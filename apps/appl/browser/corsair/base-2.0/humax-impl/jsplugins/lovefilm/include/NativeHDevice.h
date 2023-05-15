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


#ifndef __NATIVE_HDEVICE__
#define __NATIVE_HDEVICE__

#include "NativeNJSETypes.h"

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

/*****************************************************************************
 * HDevice
 ***************************************************************************** */
typedef void*	DeviceEventHandle;
typedef void*	DeviceEventError;
typedef void 	(*DeviceEventListener)(unsigned event, DeviceEventHandle data);

void		   native_Device_Listener(int event, void *data);
NJSEReturnCode native_Device_RegEventListener(DeviceEventListener listener);
NJSEReturnCode native_Device_UnRegEventListener( void );

typedef enum
{
	eNATIVE_DEVICE_CONN_CHANGED,
	eNATIVE_DEVICE_ERROR,
	eNATIVE_DEVICE_FOCUS_CHANGED,
	eNATIVE_DEVICE_APPWILL_EXIT,
	eNATIVE_DEVICE_MEM_WARNING,
} NATIVE_DEVICE_EVENT_e;

/* Properties */

NJSEReturnCode native_Device_Get_Player( void *player);
NJSEReturnCode native_Device_Get_InterfaceVersion( const char **interface_version );
NJSEReturnCode native_Device_Get_FirmwareVersion( const char **firmware_version );
NJSEReturnCode native_Device_Get_OperatingSystem( const char **operating_system );
NJSEReturnCode native_Device_Get_Manufacturer( const char **manufacturer );
NJSEReturnCode native_Device_Get_ModelName( const char **model_name );
NJSEReturnCode native_Device_Get_Chipset( const char **chipset );
NJSEReturnCode native_Device_Get_Uuid( const char **uuid );
NJSEReturnCode native_Device_Get_ClientIp( const char **client_ip);
NJSEReturnCode native_Device_Get_ClientTime( const char **client_time );
NJSEReturnCode native_Device_Get_ConnectionType( const char **connection_type );
NJSEReturnCode native_Device_Get_ConnectionStrength( NJSENumber *connection_strength );

NJSEReturnCode native_Device_Get_AudioCapability( NJSENumber *size,  char ***audio_capability );

NJSEReturnCode native_Device_Get_CpuLoad( NJSENumber *cpu_load );
NJSEReturnCode native_Device_Get_MemoryTotal( NJSENumber *memory_total );
NJSEReturnCode native_Device_Get_MemoryAvailable( NJSENumber *memory_available );

NJSEReturnCode native_Device_Get_MemoryUsed( NJSENumber *memory_used );
NJSEReturnCode native_Device_Get_DiskStorageTotal( NJSENumber *disk_storage__total );
NJSEReturnCode native_Device_Get_DiskStorageUsed( NJSENumber *disk_storage_used );
NJSEReturnCode native_Device_Get_AppWidth( NJSENumber *app_width );
NJSEReturnCode native_Device_Get_AppHeight( NJSENumber *app_height );
NJSEReturnCode native_Device_Get_ScreenWidth( NJSENumber *screen_width );
NJSEReturnCode native_Device_Get_ScreenHeight( NJSENumber *screen_height );

NJSEReturnCode native_Device_Get_LastError( DeviceEventError *error );

NJSEReturnCode native_Device_GetHasFocus( DeviceEventHandle identifier, NJSEBoolean *retval );
NJSEReturnCode native_Device_GetAppName( DeviceEventHandle identifier, const char **retval );

NJSEReturnCode native_Device_GetCategory( DeviceEventError identifier, const char **retval );
NJSEReturnCode native_Device_GetType( DeviceEventError identifier, const char **retval );
NJSEReturnCode native_Device_GetModule( DeviceEventError identifier, const char **retval );
NJSEReturnCode native_Device_GetCode( DeviceEventError identifier, const char **retval );
NJSEReturnCode native_Device_GetDescription( DeviceEventError identifier, const char **retval );
NJSEReturnCode native_Device_GetStackTrace( DeviceEventError identifier, const char **retval );

NJSEReturnCode native_Device_ReleaseErrorHandle( DeviceEventError id );






/* Methods */
NJSEReturnCode native_Device_SetTime( const NJSENumber timeStamp, NJSEBoolean *retval );  
NJSEReturnCode native_Device_SetMemoryWarningLevel( const NJSENumber levelBytes, NJSEBoolean *retval );	
NJSEReturnCode native_Device_Gc( NJSEBoolean *retval );
NJSEReturnCode native_Device_Exit( NJSEBoolean *retval );


#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* __NATIVE_HDEVICE__*/

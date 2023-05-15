/*************************************************************
 * @file		
 * @date		
 * @author		
 * @brief		

 * http://www.humaxdigital.com
 *************************************************************/

#define ___HEADER________________________________________________________________________________________
#ifndef __DI_SERVER_CLIENT_POWER_H__
#define __DI_SERVER_CLIENT_POWER_H__ 

#include "di_err.h"

#define ___DEFINE________________________________________________________________________________________

#define RPC_METHOD_NAME_POWER_SETSETTING    "rpc.power.setsetting"
#define RPC_METHOD_NAME_POWER_SETSYSFS      "rpc.power.setsysfs"
#define RPC_METHOD_NAME_POWER_GETSTANDBYSTATUS    "rpc.power.getstandbystatus"


#define ___TYPEDEF_______________________________________________________________________________________

#define ___STATIC_VARIABLES______________________________________________________________________________
 
#define ___PRIVATE_FUNCTION_PROTOTYPE____________________________________________________________________
 
#define	___PRIVATE_API___________________________________________________________________________________
 
#define	___PUBLIC_API____________________________________________________________________________________

DI_ERR_CODE DI_POWER_CLIENT_SetStandbySetting(void *pNexusStandbySettings, unsigned long size);
DI_ERR_CODE DI_POWER_CLIENT_GetStandbyStatus(void *pNexusStandbyStatus);
DI_ERR_CODE DI_POWER_CLIENT_SetSysFs(HCHAR *pCmd);
DI_ERR_CODE DI_POWER_CLIENT_Init(void);

DI_ERR_CODE DI_POWER_SERVER_Init(void);
#endif


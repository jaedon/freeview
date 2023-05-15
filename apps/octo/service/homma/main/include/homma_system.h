
#ifndef	__HOMMA_SERVER_H__
#define	__HOMMA_SERVER_H__

#include "homma_int.h"
#include "common.h"

void				HOMMA_SYSTEM_Run( void );
HCHAR*				HOMMA_SYSTEM_GetDeviceTypeStringWithMounthPath(HCHAR* MountPath, HBOOL IsFullMatched);
DxStorage_DevType_e HOMMA_SYSTEM_GetDeviceTypeWithMounthPath(HCHAR* MountPath, HBOOL IsFullMatched);

HBOOL				HOMMA_SYSTEM_AddDevice(HUINT32 DeviceID);
HBOOL				HOMMA_SYSTEM_RemoveDevice(HUINT32 DeviceID);
HCHAR*				HOMMA_SYSTEM_GetMountPath(char* media_path);

#endif	//__HOMMA_SERVER_H__


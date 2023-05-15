#ifndef __NATIVEHBLUETOOTHMANAGER_H__
#define __NATIVEHBLUETOOTHMANAGER_H__

#include "NativeOOIFTypes.h"

#include <list>
#include <oplbluetoothmanager.h>

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

#ifdef OIPF

using namespace std;

typedef enum
{
	BLUETOOTH_EVENT_LIST_UPDATED,
	BLUETOOTH_EVENT_LIST_CONNECTED,
	BLUETOOTH_EVENT_LIST_FAILED,
	BLUETOOTH_EVENT_PIN_REQUESTED,
	BLUETOOTH_EVENT_PIN_PASSKEY,
	BLUETOOTH_EVENT_DISCONNECTED,
	BLUETOOTH_EVENT_PROFILE_STARTED,
	BLUETOOTH_EVENT_PROFILE_STOPPED,
	BLUETOOTH_EVENT_AUDIO_MUTE
} eNativeBluetoothManagerEvent; 

typedef void *BluetoothHandle;
typedef void (*BluetoothStateEventListener)(int event, int param1, int param2, int param3);

void native_BluetoothManager_StateEventListener(int event, int param1, int param2, int param3);

OOIFReturnCode native_BluetoothManager_RegStateEventListener(BluetoothStateEventListener listener);
OOIFReturnCode native_BluetoothManager_UnregStateEventListener();

OOIFReturnCode native_BluetoothManager_SetEnabled(OOIFBoolean enabled);
OOIFReturnCode native_BluetoothManager_GetEnabled(OOIFBoolean *retval);
OOIFReturnCode native_BluetoothManager_StartScan();
OOIFReturnCode native_BluetoothManager_StopScan();
OOIFReturnCode native_BluetoothManager_Connect(char* address);
OOIFReturnCode native_BluetoothManager_Disconnect(char* address);
OOIFReturnCode native_BluetoothManager_Remove(char* address);
OOIFReturnCode native_BluetoothManager_GetDeviceList(list<OplBluetoothDevice*> *retval);
OOIFReturnCode native_BluetoothManager_SendPincode(char* address, char* pincode);
OOIFReturnCode native_BluetoothManager_GetMacAddress(const char **retval);

#endif /* OIPF */
#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif // __NATIVEHBLUETOOTHMANAGER_H__

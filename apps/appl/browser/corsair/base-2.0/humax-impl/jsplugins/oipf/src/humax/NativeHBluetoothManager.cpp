#include "NativeHBluetoothManager.h"

#include "macros.h"

#include <oplbluetoothmanager.h>

#define BLUETOOTHMANAGER_TRACE OOIF_LOG_DEBUG("\tBluetoothManager::%s(%d)\n", __FUNCTION__, __LINE__)

BluetoothStateEventListener g_onStateEventListener = NULL;

typedef void (*BluetoothStateEventListener)(int event, int param1, int param2, int param3);

void native_BluetoothManager_StateEventListener(int event, int param1, int param2, int param3)
{
	BLUETOOTHMANAGER_TRACE;

	if (g_onStateEventListener != NULL)
	{
		g_onStateEventListener(event, param1, param2, param3);
	}
}

OOIFReturnCode native_BluetoothManager_RegStateEventListener(BluetoothStateEventListener listener)
{
	BLUETOOTHMANAGER_TRACE;

	g_onStateEventListener = listener;
    OplBluetoothManager::getInstance()->addEventListener(native_BluetoothManager_StateEventListener);
	return OOIF_RETURN_OK;
}

OOIFReturnCode native_BluetoothManager_UnregStateEventListener()
{
	BLUETOOTHMANAGER_TRACE;

	g_onStateEventListener = NULL;
	return OOIF_RETURN_OK;
}

OOIFReturnCode native_BluetoothManager_SetEnabled(OOIFBoolean enabled)
{
    BLUETOOTHMANAGER_TRACE;
    OplBluetoothManager::getInstance()->setEnabled((bool)enabled);
	
	return OOIF_RETURN_OK;
}

OOIFReturnCode native_BluetoothManager_GetEnabled(OOIFBoolean *retval)
{
	BLUETOOTHMANAGER_TRACE;

    bool enabled = OplBluetoothManager::getInstance()->getEnabled();
	*retval = (OOIFBoolean)enabled;
	
	return OOIF_RETURN_OK;
}

OOIFReturnCode native_BluetoothManager_StartScan()
{
	BLUETOOTHMANAGER_TRACE;

    OplBluetoothManager::getInstance()->startScan();
	
	return OOIF_RETURN_OK;
}

OOIFReturnCode native_BluetoothManager_StopScan()
{
	BLUETOOTHMANAGER_TRACE;
	
    OplBluetoothManager::getInstance()->stopScan();
	
	return OOIF_RETURN_OK;
}

OOIFReturnCode native_BluetoothManager_Connect(char* address)
{
    BLUETOOTHMANAGER_TRACE;

    OplBluetoothManager::getInstance()->connect(address);

    return OOIF_RETURN_OK;
}

OOIFReturnCode native_BluetoothManager_Disconnect(char* address)
{
    BLUETOOTHMANAGER_TRACE;

    OplBluetoothManager::getInstance()->disconnect(address);

    return OOIF_RETURN_OK;
}

OOIFReturnCode native_BluetoothManager_Remove(char* address)
{
    BLUETOOTHMANAGER_TRACE;

    OplBluetoothManager::getInstance()->remove(address);

    return OOIF_RETURN_OK;
}

OOIFReturnCode native_BluetoothManager_GetDeviceList(list<OplBluetoothDevice*> *retval)
{
    BLUETOOTHMANAGER_TRACE;

    *retval = OplBluetoothManager::getInstance()->getDeviceList();

    return OOIF_RETURN_OK;
}

OOIFReturnCode native_BluetoothManager_SendPincode(char* address, char* pincode)
{
    BLUETOOTHMANAGER_TRACE;

    OplBluetoothManager::getInstance()->sendPincode(address, pincode);

    return OOIF_RETURN_OK;
}

OOIFReturnCode native_BluetoothManager_GetMacAddress(const char **retval)
{
    BLUETOOTHMANAGER_TRACE;

    *retval = OplBluetoothManager::getInstance()->getMacAddress();

    if (*retval == NULL)
        return OOIF_RETURN_VALUE_UNDEFINED;

    return OOIF_RETURN_OK;
}


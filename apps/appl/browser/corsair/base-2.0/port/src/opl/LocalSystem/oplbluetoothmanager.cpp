#include <apk.h>

#include "oplbluetoothmanager.h"

typedef	enum
{
	eOPL_BT_STATUS_LIST_UPDATED,
	eOPL_BT_STATUS_CONNECTED,
	eOPL_BT_STATUS_CONNECT_FAILED,
	eOPL_BT_STATUS_PIN_REQUESTED,
	eOPL_BT_STATUS_PIN_PASSKEY,
	eOPL_BT_STATUS_DISCONNECTED,
	eOPL_BT_STATUS_PROFILE_STARTED,
	eOPL_BT_STATUS_PROFILE_STOPPED,
	eOPL_BT_STATUS_AUDIO_MUTE,
} OplBluetoothStatus_e;

typedef enum
{
	eOplBroadcastMctrlMode_None				= 0,

	eOplBroadcastMctrlMode_Live,
	eOplBroadcastMctrlMode_TsrPb,

	eOplBroadcastMctrlMode_EndOfCase
} xOplBroadcastMctrlMode_e;

bool OplBluetoothManager::instanceFlag = false;
OplBluetoothManager* OplBluetoothManager::manager = NULL;
StateEventListener OplBluetoothManager::listener = NULL;

OplBluetoothManager* OplBluetoothManager::getInstance() {
    if (!instanceFlag) {
        manager = new OplBluetoothManager();
        instanceFlag = true;
        return manager;
    }
    else {
        return manager;
    }
}

OplBluetoothManager::~OplBluetoothManager()
{
	APK_NETWORK_UnRegisterNotifier((APKS_NETWORK_CbHandler)OplBluetoothManager::StateEventCallback);
    instanceFlag = false;
}

void OplBluetoothManager::StateEventCallback(int event, int param1, int param2, int param3)
{

	int opl_event = -1;

	switch( event )
	{
		case eNETWORK_STATUS_BT_LIST_UPDATED:
			opl_event = eOPL_BT_STATUS_LIST_UPDATED; break;
		case eNETWORK_STATUS_BT_CONNECTED:
			opl_event = eOPL_BT_STATUS_CONNECTED; break;
		case eNETWORK_STATUS_BT_CONNECT_FAILED:
			opl_event = eOPL_BT_STATUS_CONNECT_FAILED; break;
		case eNETWORK_STATUS_BT_PIN_REQUESTED:
			opl_event = eOPL_BT_STATUS_PIN_REQUESTED; break;
		case eNETWORK_STATUS_BT_PASSKEY:
			opl_event = eOPL_BT_STATUS_PIN_PASSKEY; break;
		case eNETWORK_STATUS_BT_DISCONNECTED:
			opl_event = eOPL_BT_STATUS_DISCONNECTED; break;
		case eNETWORK_STATUS_BT_PROFILE_STARTED:
			opl_event = eOPL_BT_STATUS_PROFILE_STARTED; break;
		case eNETWORK_STATUS_BT_PROFILE_STOPPED:
			opl_event = eOPL_BT_STATUS_PROFILE_STOPPED; break;
		case eNETWORK_STATUS_BT_AUDIO_MUTE:
			opl_event = eOPL_BT_STATUS_AUDIO_MUTE; break;
		default: break;
	}

	if( opl_event == -1 ) return;

	if (listener != NULL)
	{
		listener(opl_event, param1, param2, param3);
	}
}

void OplBluetoothManager::addEventListener(StateEventListener listener)
{
	if (listener)
	{
		OplBluetoothManager::listener = listener;
        APK_NETWORK_RegisterNotifier((APKS_NETWORK_CbHandler)OplBluetoothManager::StateEventCallback);
	}
}

void OplBluetoothManager::setEnabled(bool enabled)
{
	 APK_NETWORK_BT_SetEnable((HBOOL)enabled);
}

bool OplBluetoothManager::getEnabled()
{
	HBOOL retval = FALSE;

    APK_NETWORK_BT_GetEnable(&retval);

	return (int)retval;
}

void OplBluetoothManager::startScan()
{
	APK_NETWORK_BT_Scan();
}

void OplBluetoothManager::stopScan()
{
}

void OplBluetoothManager::connect(int index)
{
	APK_NETWORK_BT_Connect(index);
}

void OplBluetoothManager::connect(char* address)
{
    APK_NETWORK_BT_ConnectByBdAddr(address);
}

void OplBluetoothManager::disconnect(int index)
{
	APK_NETWORK_BT_Disconnect(index);
}

void OplBluetoothManager::disconnect(char* address)
{
    APK_NETWORK_BT_DisconnectByBdAddr(address);
}

void OplBluetoothManager::remove(char* address)
{
    APK_NETWORK_BT_RemoveByBdAddr(address);
}

void OplBluetoothManager::removeAt(int index)
{
	APK_NETWORK_BT_Remove(index);
}

void OplBluetoothManager::sendPincode(char* address, char* pincode)
{
    APK_NETWORK_BT_SendPinCodeByBdAddr(address, pincode);
}

const char*  OplBluetoothManager::getMacAddress()
{
    const char *retval = (char*)HLIB_STD_MemAlloc(APK_NETWORK_BT_GetMacAddrInfoSize());
    APK_NETWORK_BT_GetMacAddrInfo((HCHAR*)retval);
    return retval;
}

list<OplBluetoothDevice*> OplBluetoothManager::getDeviceList()
{
    int count = 0;
    int size = sizeof(APKS_BT_DEVICEINFO_t);
	int requestCount = 0;
    int totalSize = 0;
    list<OplBluetoothDevice*> devices;
    OplBluetoothDevice* device;

	requestCount = APK_NETWORK_BT_GetDeviceNum();
	totalSize = size * requestCount;

	APKS_BT_DEVICEINFO_t* origin = (APKS_BT_DEVICEINFO_t*)HLIB_STD_MemAlloc(totalSize);
    count = (int)APK_NETWORK_BT_GetDevInfoList((HINT32)totalSize, origin);

    for (int i = 0; i < count; i++)
    {
        APKS_BT_DEVICEINFO_t info(origin[i]);
        device = new OplBluetoothDevice((OplBluetoothHandle)&info);
        device->index = i;
        devices.push_back(device);
    }

    HLIB_STD_MemFree(origin);

    return devices;
}

OplBluetoothDevice::OplBluetoothDevice(OplBluetoothHandle handle)
{
	HCHAR tmp[3];

    APKS_BT_DEVICEINFO_t* source = (APKS_BT_DEVICEINFO_t*)handle;
    id = source->ulDevId;
    status = source->eStatus;
    supportedProfile = source->eSupportedProfile;
	snprintf(tmp, sizeof(tmp), "%X", source->dev_class[2]);
	devClass = HLIB_STD_HexToInt(tmp);
    HLIB_STD_StrNCpySafe(name, (char*)source->szDevName, sizeof(name));
    HLIB_STD_StrNCpySafe(address, (char*)source->aucBdAddr, sizeof(address));
}



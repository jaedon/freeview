#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "jsplugin.h"

#include "macros.h"
#include "Exception.h"

#include "HBluetoothManager.h"
#include <oplbluetoothmanager.h>

using namespace Ooif;

/**
 * @brief The HBluetoothDevice class
 */
HBluetoothDevice::HBluetoothDevice(OplBluetoothDevice* origin) :
    WrapperObject<HBluetoothDevice>(origin),
    info(origin)
{
    classType = CLASS_HMX_BLUETOOTHDEVICE;
}

HBluetoothDevice::~HBluetoothDevice()
{

}

int HBluetoothDevice::getter(jsplugin_obj *obj, const char *name, jsplugin_value *result)
{
    GET_NUMBER("id", info->id, JSP_GET_VALUE);
    GET_STRING("name", info->name, JSP_GET_VALUE);
    GET_NUMBER("status", info->status, JSP_GET_VALUE);
    GET_NUMBER("index", info->index, JSP_GET_VALUE);
    GET_STRING("address", info->address, JSP_GET_VALUE);
	GET_NUMBER("devClass", info->devClass, JSP_GET_VALUE);
    GET_NUMBER("supportedProfile", info->supportedProfile, JSP_GET_VALUE);

    GET_FUNCTION(connect, obj, "", JSP_GET_VALUE_CACHE);
    GET_FUNCTION(disconnect, obj, "", JSP_GET_VALUE_CACHE);
	GET_FUNCTION(remove, obj, "", JSP_GET_VALUE_CACHE);
    GET_FUNCTION(sendPincode, obj, "s", JSP_GET_VALUE_CACHE);

    return JSP_GET_NOTFOUND;
}

int HBluetoothDevice::setter(jsplugin_obj *obj, const char *name, jsplugin_value *result)
{
    return JSP_GET_NOTFOUND;
}

int HBluetoothDevice::connect(jsplugin_obj *this_obj, jsplugin_obj *function_obj, int argc, jsplugin_value *argv, jsplugin_value *result)
{
    ENTRY;
    CHECK_ARGUMENT_COUNT(0);
    HBluetoothDevice *instance = EXTRACT_OBJECT_OR_THROW(instance, this_obj, HBluetoothDevice);
    // NATIVE_CALL(native_BluetoothManager_Connect(instance->info->index));
    NATIVE_CALL(native_BluetoothManager_Connect(instance->info->address));

    return JSP_CALL_NO_VALUE;
}

int HBluetoothDevice::disconnect(jsplugin_obj *this_obj, jsplugin_obj *function_obj, int argc, jsplugin_value *argv, jsplugin_value *result)
{
    ENTRY;
    CHECK_ARGUMENT_COUNT(0);
    HBluetoothDevice *instance = EXTRACT_OBJECT_OR_THROW(instance, this_obj, HBluetoothDevice);
    NATIVE_CALL(native_BluetoothManager_Disconnect(instance->info->address));

    return JSP_CALL_NO_VALUE;
}

int HBluetoothDevice::remove(jsplugin_obj *this_obj, jsplugin_obj *function_obj, int argc, jsplugin_value *argv, jsplugin_value *result)
{
    ENTRY;
    CHECK_ARGUMENT_COUNT(0);
    HBluetoothDevice *instance = EXTRACT_OBJECT_OR_THROW(instance, this_obj, HBluetoothDevice);
    NATIVE_CALL(native_BluetoothManager_Remove(instance->info->address));

    return JSP_CALL_NO_VALUE;
}

int HBluetoothDevice::sendPincode(jsplugin_obj *this_obj, jsplugin_obj *function_obj, int argc, jsplugin_value *argv, jsplugin_value *result)
{
    ENTRY;
    CHECK_ARGUMENT_COUNT(1);

    HBluetoothDevice *instance = EXTRACT_OBJECT_OR_THROW(instance, this_obj, HBluetoothDevice);
    char* pincode = (char*)STRING_VALUE(argv[0]);
    NATIVE_CALL(native_BluetoothManager_SendPincode(instance->info->address, pincode));

    return JSP_CALL_NO_VALUE;
}

void HBluetoothDevice::releaseNative(void* id)
{
    OplBluetoothDevice* oplData = reinterpret_cast<OplBluetoothDevice*>(id);
    delete oplData;
}

/**
 * @brief The HBluetoothManager class
 */
HBluetoothManager *HBluetoothManager::manager;

HBluetoothManager::HBluetoothManager() : EventTarget()
{
    ENTRY;

    classType = CLASS_HMX_BLUETOOTHMANAGER;
	manager = this;
	native_BluetoothManager_RegStateEventListener(HBluetoothManager::StateEventHandler);
}

HBluetoothManager::~HBluetoothManager()
{
	native_BluetoothManager_UnregStateEventListener();
}

int HBluetoothManager::getter(jsplugin_obj *obj, const char *name, jsplugin_value *result)
{
    ENTRY;

    NATIVE_GET_BOOLEAN("enabled", native_BluetoothManager_GetEnabled(&RETVAL), JSP_GET_VALUE);

	GET_FUNCTION(startScan, obj, "", JSP_GET_VALUE_CACHE);
	GET_FUNCTION(stopScan, obj, "", JSP_GET_VALUE_CACHE);
	GET_FUNCTION(connect, obj, "s", JSP_GET_VALUE_CACHE);
    GET_FUNCTION(disconnect, obj, "s", JSP_GET_VALUE_CACHE);
	GET_FUNCTION(remove, obj, "s", JSP_GET_VALUE_CACHE);
    GET_FUNCTION(getDeviceList, obj, "", JSP_GET_VALUE_CACHE);
    GET_FUNCTION(sendPincode, obj, "ss", JSP_GET_VALUE_CACHE);
    GET_FUNCTION(getMacAddress, obj, "", JSP_GET_VALUE_CACHE);

    GET_FUNCTION(addEventListener, obj, "sob", JSP_GET_VALUE_CACHE);
    GET_FUNCTION(removeEventListener, obj, "sos", JSP_GET_VALUE_CACHE);
    INTRINSIC_EVENT_GET(StateUpdate);

    return JSP_GET_NOTFOUND;
    // return EventTarget::getter(obj, name, result);
}

int HBluetoothManager::setter(jsplugin_obj *obj, const char *name, jsplugin_value *value)
{
    ENTRY;

    bool enabled = BOOLEAN_VALUE(value[0]);
    NATIVE_SET("enabled", native_BluetoothManager_SetEnabled(enabled));
    INTRINSIC_EVENT(StateUpdate);

    return JSP_PUT_NOTFOUND;
}

int HBluetoothManager::startScan(jsplugin_obj *this_obj, jsplugin_obj *function_obj, int argc, jsplugin_value *argv, jsplugin_value *result)
{
    ENTRY;
	CHECK_ARGUMENT_COUNT(0);

    NATIVE_CALL(native_BluetoothManager_StartScan());

	return JSP_CALL_NO_VALUE;
}

int HBluetoothManager::stopScan(jsplugin_obj *this_obj, jsplugin_obj *function_obj, int argc, jsplugin_value *argv, jsplugin_value *result)
{
    ENTRY;
	CHECK_ARGUMENT_COUNT(0);

	native_BluetoothManager_StopScan();

	return JSP_CALL_NO_VALUE;
}

int HBluetoothManager::connect(jsplugin_obj *this_obj, jsplugin_obj *function_obj, int argc, jsplugin_value *argv, jsplugin_value *result)
{
    ENTRY;
	CHECK_ARGUMENT_COUNT(1);

    char* address = (char*)STRING_VALUE(argv[0]);
    NATIVE_CALL(native_BluetoothManager_Connect(address));

	return JSP_CALL_NO_VALUE;
}

int HBluetoothManager::disconnect(jsplugin_obj *this_obj, jsplugin_obj *function_obj, int argc, jsplugin_value *argv, jsplugin_value *result)
{
    ENTRY;
	CHECK_ARGUMENT_COUNT(1);

    char* address = (char*)STRING_VALUE(argv[0]);
    NATIVE_CALL(native_BluetoothManager_Disconnect(address));

	return JSP_CALL_NO_VALUE;
}

int HBluetoothManager::remove(jsplugin_obj *this_obj, jsplugin_obj *function_obj, int argc, jsplugin_value *argv, jsplugin_value *result)
{
    ENTRY;
	CHECK_ARGUMENT_COUNT(1);

    char* address = (char*)STRING_VALUE(argv[0]);
    NATIVE_CALL(native_BluetoothManager_Remove(address));

	return JSP_CALL_NO_VALUE;
}

int HBluetoothManager::getDeviceList(jsplugin_obj *this_obj, jsplugin_obj *function_obj, int argc, jsplugin_value *argv, jsplugin_value *result)
{
    ENTRY;

    HBluetoothManager* instance = EXTRACT_OBJECT_OR_THROW(instance, this_obj, HBluetoothManager);
    list<OplBluetoothDevice*> devices;
    NATIVE_CALL(native_BluetoothManager_GetDeviceList(&devices));

    HBluetoothDeviceCollection* collection = new HBluetoothDeviceCollection();
    list<OplBluetoothDevice*>::iterator i(devices.begin());
    while (i != devices.end()) {
        collection->addItem(new HBluetoothDevice(*i++));
    }

    RETURN_OBJECT(this_obj, collection, result, JSP_CALL_VALUE);
    return JSP_CALL_ERROR;
}

int HBluetoothManager::sendPincode(jsplugin_obj *this_obj, jsplugin_obj *function_obj, int argc, jsplugin_value *argv, jsplugin_value *result)
{
    ENTRY;
    CHECK_ARGUMENT_COUNT(2);

    //int index = NUMBER_VALUE(argv[0]);
	char* address = (char*)STRING_VALUE(argv[0]);
    char* pincode = (char*)STRING_VALUE(argv[1]);
    NATIVE_CALL(native_BluetoothManager_SendPincode(address, pincode));

    return JSP_CALL_NO_VALUE;
}

int HBluetoothManager::getMacAddress(jsplugin_obj *this_obj, jsplugin_obj *function_obj, int argc, jsplugin_value *argv, jsplugin_value *result)
{
    ENTRY;
    CHECK_ARGUMENT_COUNT(0);

    const char* retval;
    NATIVE_CALL(native_BluetoothManager_GetMacAddress(&retval));
    RETURN_STRING(retval, JSP_GET_VALUE);

    return JSP_CALL_NO_VALUE;
}

void HBluetoothManager::StateEventHandler(int event, int param1, int param2, int param3)
{
    ENTRY;

	Event *e = NULL;
	jsplugin_obj *context = NULL;
    HBluetoothManager *obj = manager;

    if (NULL == obj)
	{
		OOIF_LOG_DEBUG("HBluetoothManager has Null instance !!!! \n");
		return ;
	}

    context = obj->getHost();

	if(context)
	{
		e = new HBluetoothManagerStateEvent(context, event, param1, param2, param3);
		if( NULL != e)
		{
			OOIF_LOG_DEBUG("dispatchEvent(%p)'s ----> (%p)", manager, e);
            obj->dispatchEvent(e);
		}
	}

	OOIF_LOG_DEBUG("StateEventHandler - event(%d), param1(%d), param2(%d), param3(%d) \n", event, param1, param2, param3);
}

/**
 * @brief The HBluetoothManagerStateEvent class
 */
HBluetoothManagerStateEvent::HBluetoothManagerStateEvent(jsplugin_obj *c, int _event, int _param1, int _param2, int _param3)
    : Event(c, "StateUpdate"), event(_event), param1(_param1), param2(_param2), param3(_param3)
{

}

HBluetoothManagerStateEvent:: ~HBluetoothManagerStateEvent()
{
    ENTRY;
}

int HBluetoothManagerStateEvent::getter(jsplugin_obj *obj, const char *name, jsplugin_value *result)
{
	ENTRY;

    GET_NUMBER("event", event, JSP_GET_VALUE);
    GET_NUMBER("param1", param1, JSP_GET_VALUE);
    GET_NUMBER("param2", param2, JSP_GET_VALUE);
    GET_NUMBER("param3", param3, JSP_GET_VALUE);

	return JSP_GET_NOTFOUND;
}

int HBluetoothManagerStateEvent::setter(jsplugin_obj *obj, const char *name, jsplugin_value *value)
{
    ENTRY;

    return JSP_PUT_NOTFOUND;
}

int HBluetoothManagerStateEvent::serialize(jsplugin_value **argv)
{
	ENTRY;

	const int RETURN_PARAM_COUNT = 4;
	*argv = new jsplugin_value[RETURN_PARAM_COUNT];
	jsplugin_value *result = *argv;

	result[0].type = JSP_TYPE_NUMBER;
	result[0].u.number = event;

	result[1].type = JSP_TYPE_NUMBER;
	result[1].u.number = param1;

	result[2].type = JSP_TYPE_NUMBER;
	result[2].u.number = param2;

	result[3].type = JSP_TYPE_NUMBER;
	result[3].u.number = param3;

	return RETURN_PARAM_COUNT;
}

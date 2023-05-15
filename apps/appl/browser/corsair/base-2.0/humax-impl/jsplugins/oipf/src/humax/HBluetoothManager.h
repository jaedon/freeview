#ifndef __HBLUETOOTHMANAGER_H__
#define __HBLUETOOTHMANAGER_H__

#include <list>
#include "EventTarget.h"
#include "WrapperObject.h"
#include "Collection.h"
#include "oplbluetoothmanager.h"

#include "NativeHBluetoothManager.h"

using namespace std;

class OplBluetoothDevice;

namespace Ooif {

/**
 * @brief The HBluetoothDevice class
 */
class HBluetoothDevice : public EventTarget, public WrapperObject<HBluetoothDevice>
{
private:
    OplBluetoothDevice* info;
public:
    HBluetoothDevice(OplBluetoothDevice* origin);
    virtual ~HBluetoothDevice();
    virtual int getter(jsplugin_obj *obj, const char *name, jsplugin_value *result);
    virtual int setter(jsplugin_obj *obj, const char *name, jsplugin_value *value);

    static int connect(jsplugin_obj *this_obj, jsplugin_obj *function_obj, int argc, jsplugin_value *argv, jsplugin_value *result);
    static int disconnect(jsplugin_obj *this_obj, jsplugin_obj *function_obj, int argc, jsplugin_value *argv, jsplugin_value *result);
	static int remove(jsplugin_obj *this_obj, jsplugin_obj *function_obj, int argc, jsplugin_value *argv, jsplugin_value *result);
    static int sendPincode(jsplugin_obj *this_obj, jsplugin_obj *function_obj, int argc, jsplugin_value *argv, jsplugin_value *result);
    static void releaseNative(void* id);
    static const ClassType class_name = CLASS_HMX_BLUETOOTHDEVICE;
};

/**
 * @brief The HBluetoothManager class
 */
class HBluetoothManager : public EventTarget
{
private:
	static HBluetoothManager *manager;
	static void StateEventHandler(int event, int param1, int param2, int param3);	
public:
    HBluetoothManager();
    virtual ~HBluetoothManager();
    virtual int getter(jsplugin_obj *obj, const char *name, jsplugin_value *result);
    virtual int setter(jsplugin_obj *obj, const char *name, jsplugin_value *value);

	static int startScan(jsplugin_obj *this_obj, jsplugin_obj *function_obj, int argc, jsplugin_value *argv, jsplugin_value *result);
	static int stopScan(jsplugin_obj *this_obj, jsplugin_obj *function_obj, int argc, jsplugin_value *argv, jsplugin_value *result);
	static int connect(jsplugin_obj *this_obj, jsplugin_obj *function_obj, int argc, jsplugin_value *argv, jsplugin_value *result);
	static int disconnect(jsplugin_obj *this_obj, jsplugin_obj *function_obj, int argc, jsplugin_value *argv, jsplugin_value *result);
    static int remove(jsplugin_obj *this_obj, jsplugin_obj *function_obj, int argc, jsplugin_value *argv, jsplugin_value *result);
    static int getDeviceList(jsplugin_obj *this_obj, jsplugin_obj *function_obj, int argc, jsplugin_value *argv, jsplugin_value *result);
    static int sendPincode(jsplugin_obj *this_obj, jsplugin_obj *function_obj, int argc, jsplugin_value *argv, jsplugin_value *result);
    static int getMacAddress(jsplugin_obj *this_obj, jsplugin_obj *function_obj, int argc, jsplugin_value *argv, jsplugin_value *result);
	
    static const ClassType class_name = CLASS_HMX_BLUETOOTHMANAGER;
};

/**
 * @brief The HBluetoothManagerStateEvent class
 */
class HBluetoothManagerStateEvent : public Event
{
private:
	int event;
	int param1;
	int param2;
	int param3;
public:
	HBluetoothManagerStateEvent(jsplugin_obj *c, int _event, int _param1, int _param2, int _param3);
    virtual ~HBluetoothManagerStateEvent();
	int getter(jsplugin_obj *obj, const char *name, jsplugin_value *result);
    int setter(jsplugin_obj *obj, const char *name, jsplugin_value *value);
	virtual int serialize(jsplugin_value **argv);
};

} /* namespace Ooif */

#endif // __HBLUETOOTHMANAGER_H__

#ifndef __OPLBLUETOOTHMANAGER_H__
#define __OPLBLUETOOTHMANAGER_H__

#include <list>

using namespace std;

typedef void* OplBluetoothHandle;
typedef void (*StateEventListener)(int event, int param1, int param2, int param3);

class OplBluetoothDevice
{
public:
    OplBluetoothDevice(OplBluetoothHandle source);
    int index;
    int id;
    int status;
    int supportedProfile;
	int devClass;
    char address[64];
    char name[64];
};

class OplBluetoothManager
{
private:
    static bool instanceFlag;
    static OplBluetoothManager* manager;
    static StateEventListener listener;
    static void StateEventCallback(int event, int param1, int param2, int param3);
public:
    virtual ~OplBluetoothManager();

    static OplBluetoothManager* getInstance();
    void addEventListener(StateEventListener listener);
    void setEnabled(bool enabled);
    bool getEnabled();
    void startScan();
    void stopScan();
    void connect(int index);
    void connect(char* address);
    void disconnect(int index);
    void disconnect(char* address);
    void remove(char* address);
    void removeAt(int index);
    void sendPincode(char* address, char* pincode);
    const char* getMacAddress();
    list<OplBluetoothDevice*> getDeviceList();
};

#endif // __OPLBLUETOOTHMANAGER_H__


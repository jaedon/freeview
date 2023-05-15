#ifndef __DBUS_SERVER_H__
#define __DBUS_SERVER_H__

#include <dbus-c++/dbus.h>

typedef enum 
{
	DBUS_CONNECT_NONE		= 0,
		
	DBUS_CONNECT_SYSTEM,
	DBUS_CONNECT_SESSION,	
}eDbusConnect_t;

typedef enum
{
	DBUS_SERVER_NONE	= 0,
		
	DBUS_SERVER_CONNECT,
}eDbusState_t;

class cDbusServer
{
public:
	cDbusServer(void);
	~cDbusServer(void);

	void ConnectBus(eDbusConnect_t eType);
	void RegisterModule(const char *pModuleUri);
	void Start(void);
	void Stop(void);

	DBus::Connection& GetConnectHandle(void);

protected:
	bool IsConnect(void);
		
private:

	eDbusState_t	mState;
	eDbusConnect_t	mConnectType;	

	static DBus::BusDispatcher mDispatcher;
	static DBus::Connection mConnectHandle;
};

#endif

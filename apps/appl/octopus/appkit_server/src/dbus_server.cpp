#include <dbus_server.h>
#include <stdio.h>
DBus::Connection cDbusServer::mConnectHandle = DBus::Connection::SystemBus();
DBus::BusDispatcher cDbusServer::mDispatcher;


#define _____PROTECTED_FUNC_____
bool cDbusServer::IsConnect(void)
{
	return (mState >= DBUS_SERVER_CONNECT);
}

#define _____PUBLIC_FUNC_____
cDbusServer::cDbusServer(void) : mState(DBUS_SERVER_NONE), mConnectType(DBUS_CONNECT_NONE)
{
	DBus::default_dispatcher = &mDispatcher;
	return ;
}

cDbusServer::~cDbusServer(void)
{
	mState = DBUS_SERVER_NONE;
	mConnectType = DBUS_CONNECT_NONE;

	return ;
}

void cDbusServer::ConnectBus(eDbusConnect_t eType)
{
	if (DBUS_SERVER_NONE != mState)
	{
		/* already connect to busdaemon */
		return ;
	}

/*
	//connect
	switch (eType)
	{ 
		case DBUS_CONNECT_SYSTEM:
			mConnectHandle = DBus::Connection::SystemBus();
			break;
			
		case DBUS_CONNECT_SESSION:
			mConnectHandle = DBus::Connection::SessionBus();
			break;

		default:
			return;
	}
*/
	//update member-value
	mState = DBUS_SERVER_CONNECT;
	mConnectType = eType;

	printf("ConnectBus");
	return ;
}

void cDbusServer::RegisterModule(const char *pModuleUri)
{
	if ( !IsConnect() )
		return ;

	if (NULL == pModuleUri)
		return ;

	//request 3rd module
	mConnectHandle.request_name (pModuleUri);

	printf("RegisterModule");
	return ;	
}

void cDbusServer::Start(void)
{
	if ( !IsConnect() )
		return ;

	mDispatcher.enter();

	printf("Start");
	return ;
}

void cDbusServer::Stop(void)
{
	mDispatcher.leave();

	printf("Stop");
	return;
}

DBus::Connection& cDbusServer::GetConnectHandle(void)
{
	printf("GetConnectHandle");
	return mConnectHandle;
}

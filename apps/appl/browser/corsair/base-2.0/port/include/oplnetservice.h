#ifndef __OPLNETSERVICE_H__
#define __OPLNETSERVICE_H__

#include "bpltype.h"

#ifdef __cplusplus
#include <list>
#endif

#ifdef __cplusplus
extern "C" {
#endif

#ifdef __cplusplus
};

#if defined(CONFIG_OP_JAPAN)
typedef enum {
	eNetService_WiFiScanning,
	eNetService_WiFiConnect,
	eNetService_WiFiWPS,
	eNetService_DHCPServer,
//	eNetService_FTP,
//	eNetService_DMS,
	eNetService_MAX,
} NetSvc_e;

typedef enum {
	eSVC_WIFI_SCAN_FAIL = 0,
	eSVC_WIFI_SCAN_SUCCESS, 
	eSVC_WIFI_SCAN_MAX 
} SvcWiFiScanEvent_e;

typedef enum {
	eSVC_WIFI_CONNECT_FAIL = 0,
	eSVC_WIFI_CONNECT_SUCCESS,
	eSVC_WIFI_CONNECT_MAX
} SvcWiFiConnectEvent_e;

typedef void (OPL_NETWORKSERVICE_EventCallback)( int nSvcIndex, int nEvent, int nParam1, int nParam2 );
class NetService
{
	public : 
		NetService( OPL_NETWORKSERVICE_EventCallback callback );
		 
		virtual ~NetService() {} 

		virtual bool 	Set(char *szName, char *szValue ) { return true; }
		virtual char *	Get(char *szName) { return NULL; }
		virtual int		Start() { return 0; }
		virtual int		Stop() { return 0; }

		char *	GetName() { return m_szName; }
		char *	GetTarget() { return m_szTarget; }
		bool	SetTarget( const char *pszTarget );
		int		GetIfIndex() { return m_nIfIndex;	}

	protected :
		int	GetIndexOfIf( const char *szIfName );

	protected :
		char * 	m_szName;
		char  	m_szTarget[32];
		int		m_nIfIndex;
};

class NetServiceFactory
{
	public : 
	NetServiceFactory();
	virtual ~NetServiceFactory();

	static NetService * CreateService( NetSvc_e svcType, OPL_NETWORKSERVICE_EventCallback callback );
	private :
};

class WiFiScanningService : public NetService 
{
	public :
		WiFiScanningService( OPL_NETWORKSERVICE_EventCallback callback ); 
		virtual ~WiFiScanningService(); 

		bool 	Set(char *szName, char *szValue );
		char *	Get(char *szName);
		int		Start();
		int		Stop() { return 0; }

		static  int		GetApCount()	{ return m_sApCount; }
		static	void*	GetApList();
	private :
		static void NotifyEvent( int eEvent, unsigned int ulParam1, unsigned int ulParam2, unsigned int ulParam3 );
	private : 
		static OPL_NETWORKSERVICE_EventCallback *m_fnCallback;
		static std::list<WiFiScanningService *> m_sInstanceList;

		static void *	m_sApList;
		static int		m_sApCount;
};

class WiFiConnectService : public NetService 
{
	public :
		WiFiConnectService( OPL_NETWORKSERVICE_EventCallback callback );
		virtual ~WiFiConnectService();

		bool 	Set(char *szName, char *szValue );
		char *	Get(char *szName);
		int		Start();
		int		Stop() { return 0; }

		bool 	UpdateApInfo();	
	private :
		static	bool m_bFailed;
		static void NotifyEvent( int eEvent, unsigned int ulParam1, unsigned int ulParam2, unsigned int ulParam3 );

		bool	setSecurity( const char *szValue );
		bool	setAuthen( const char *szValue );
		bool	setEncrypt( const char *szValue );

		char *	getSecurity( int type );
		char *	getAuthen( int type );
		char *	getEncrypt( int type );
		char *	getWaveStrength( int nBandWidth );
		char *	getConnectSpeed( int nSpeed );
	private : 
		static OPL_NETWORKSERVICE_EventCallback *m_fnCallback;
		static std::list<WiFiConnectService *> m_sInstanceList;
		char 	m_szSSID[48];
		char 	m_szKey[128];
		int		m_nSecureType;
		int 	m_nAuthenType;
		int		m_nEncryptType;
		
		char	m_szWave[32];
		char	m_szSpeed[32];
		int 	m_nWaveStrength;
		int		m_nConnectSpeed;
};

class WiFiWPSService : public NetService 
{
	public :
		WiFiWPSService( OPL_NETWORKSERVICE_EventCallback callback ); 
		virtual ~WiFiWPSService(); 

		bool 	Set(char *szName, char *szValue );
		char *	Get(char *szName);
		int		Start();
		int		Stop() { return 0; }

	private :
		static void NotifyEvent( int eEvent, unsigned int ulParam1, unsigned int ulParam2, unsigned int ulParam3 );
		bool	setConnectType( const char *szValue );
		bool	initPincodeByMac( const char *szMac );
	private : 
		static OPL_NETWORKSERVICE_EventCallback *m_fnCallback;
		static std::list<WiFiWPSService *> m_sInstanceList;

		int		m_nConnectType;
		char	m_szPinCode[16]; // 8-digit PINs

};

class DHCPServerService : public NetService 
{
	public :
		DHCPServerService( OPL_NETWORKSERVICE_EventCallback callback ); 
		virtual ~DHCPServerService(); 

		bool 	Set(char *szName, char *szValue );
		char *	Get(char *szName);
		int		Start();
		int		Stop(); 

		void	ClearTarget();

	private :
		static void NotifyEvent( int eEvent, unsigned int ulParam1, unsigned int ulParam2, unsigned int ulParam3 );
	private : 
		static OPL_NETWORKSERVICE_EventCallback *m_fnCallback;
		static std::list<DHCPServerService *> m_sInstanceList;
};

/******************************************************************
	class OplNetworkService
******************************************************************/
class OplNetworkService
{
	public:
		OplNetworkService(int nServiceIndex);
		virtual ~OplNetworkService();

		static	int	getNumOfSupportService();

		static  bool registerEventListener( OPL_NETWORKSERVICE_EventCallback callbackFunc );
		static  bool unregisterEventListener( OPL_NETWORKSERVICE_EventCallback callbackFunc ); 
		
		static	void notifyEventToCallback( int nSvcIndex, int nEvent, int nParam1, int nParam2  );

		char *	getServiceName(); 
		char *	getTargetName(); 
		int		getServiceIndex()
		{	return m_nSvcIndex;		}
		int		getTargetIndex();

		bool setTargetName(char *p);

		bool setCmd(char *szCmd, char *szParam);
		char *getCmd(char *p1);
		int  restart();
		int  stop();

	private :
		NetService *m_pService;
		int		m_nSvcIndex;

		static OPL_NETWORKSERVICE_EventCallback *s_pfnNetServiceCallback;
};

#endif
#endif
#endif

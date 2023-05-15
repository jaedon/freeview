/**************************************************************************************/
/**
 * @file oplnetwork.h
 *
 * Network Interfaces
 *
 **************************************************************************************
 **/

#ifndef __OPLNETWORK_H__
#define __OPLNETWORK_H__

#include "bpltype.h"

#ifdef __cplusplus
#include <list>
#endif
/******************************************************************
	Define
******************************************************************/
#define OPLNET_MAX_IPADDR_LEN		16
#define OPLNET_MAX_MACADDR_LEN		20
#define OPLNET_MAX_NAME_LEN			16
#define OPLNET_MAX_IPV6ADDR_LEN		40
#define	OPLNET_MAX_ID_LEN			64
#define OPLNET_MAX_PW_LEN			128

/******************************************************************
	Enum
 ******************************************************************/
typedef enum {
	OPLNET_TYPE_LAN = 0,			/** the wired LAN */
	OPLNET_TYPE_WLAN = 1,			/** the wireless LAN */
	OPLNET_TYPE_PPPOE = 2,			/** the PPPoE */
	OPLNET_TYPE_NONE,
} eOplNetType;

typedef enum {
	eNET_CONNECT_TRY_FAIL  = 0,
	eNET_CONNECT_TRY_SUCCESS,

	eNET_CABLE_LINK_DICONNECTED,
	eNET_CABLE_LINK_CONNECTD,

	eNET_WIFI_USB_EXTRACTED,
	eNET_WIFI_USB_INSERTED,

	eNET_CONNECT_AUTOIP_FAIL,
	eNET_CONNECT_AUTOIP_SUCCESS,

	eNET_ACCESS_GW_FAIL,
	eNET_ACCESS_GW_SUCCESS,
	eNET_ACCESS_DNS_FAIL,
	eNET_ACCESS_DNS_SUCCESS,
	eNET_ACCESS_INTERNET_FAIL,
	eNET_ACCESS_INTERNET_SUCCESS

} OplNetLinkStatus_e;

typedef enum {
	eNET_WIFI_SCANNING_OUTPUT = 0,
	eNET_WIFI_SCANNING_END,
	eNET_WIFI_SCANNING_FAIL,
	eNET_WIFI_CONNECT_SUCCESS,
	eNET_WIFI_CONNECT_FAIL,
	eNET_WIFI_KEY_INVALID,
	eNET_WIFI_WPS_SUCCESS,
	eNET_WIFI_WPS_FAIL,
} OplNetWifiStatus_e;

typedef enum {
	eNET_WOWL_NONE				= 0x00,		/* Do not wake up on any event. */
	eNET_WOWL_MAGIC_PATTERN		= 0x01,		/* Wake up on magic pattern. */
	eNET_WOWL_NET_PATTERN		= 0x02,		/* Wake up on a special net pattern. */
	eNET_WOWL_DISASSOC_DEAUTH	= 0x04,		/* Wake up on disassociate from AP. */
	eNET_WOWL_RETROGRADE_TSF	= 0x08,		/* Wake up on retrograde tsf. */
	eNET_WOWL_BCN_LOSS			= 0x10,		/* Wake up on loss of beacon. */
} OplNetWoWlan_Mode_t;

typedef enum {
	eNET_PPPOE_CONNECTED = 0,
	eNET_PPPOE_CONNECT_FAILED,
	eNET_PPPOE_DISCONNECTED,
	eNET_PPPOE_IN_PROGRESS,
} OplNetPPPoEStatus_e;

/******************************************************************
	Typedef
******************************************************************/
typedef void * OplNetworkHandle_t;
typedef void * OplNetApHandle_t;

typedef struct {
	char	szIpAddr[OPLNET_MAX_IPADDR_LEN];		/** the IP address */
	char	szNetmask[OPLNET_MAX_IPADDR_LEN];		/** the netmask */
	char	szGateway[OPLNET_MAX_IPADDR_LEN];		/** the Gateway address */
	char	szDns1[OPLNET_MAX_IPADDR_LEN];			/** the primary DNS address */
	char	szDns2[OPLNET_MAX_IPADDR_LEN];			/** the secondary DNS address */
	char	szMacAddress[OPLNET_MAX_MACADDR_LEN];	/** the MAC address */
	BPL_BOOL		bConnected;					/** if the network link is connected */
	BPL_BOOL		bEnabled;					/** if your box has the network interface */
	eOplNetType		eNetType;					/** wired or wirelessa LAN */
} OplNetworkInterface_t;

//typedef void (OPL_NETWORK_EventCallback)(void * id, int nDevIndex, unsigned int state);
typedef void (OPL_NETWORK_EventCallback)( int event, OplNetworkHandle_t handleId, int nDevIndex );

typedef struct {
	HCHAR	szESSID[48];
	HCHAR	szMacAddr[OPLNET_MAX_MACADDR_LEN];
	HINT32	nOpMode;
	HINT32	nSecureType;
	HINT32	nAuthenType;
	HINT32	nEncryptType;
	HINT32	nWaveStrength;
	HINT32	nConnectSpeed;
	HINT32	bIsConnected;
} OplNetworkAccessPoint_t;

typedef OplNetworkAccessPoint_t  OplNetworkAccessPoint;

typedef enum {
	eNET_Property_Type_String = 0,
	eNET_Property_Type_Interger,
	eNET_Property_Type_Bool,
} OplNetPropertyType;
#if defined(CONFIG_OP_JAPAN)
#define OPL_NETWORK_COMMAND_MAX	21
typedef struct {
	HCHAR 				szCmd[32];
	OplNetPropertyType	eType;
	HUINT32				ValAddr;
	HUINT32				ulValSize;
} OplNetworkCmd_t;

typedef OplNetworkCmd_t OplNetworkCmd;
#endif

/******************************************************************
	Interfaces
******************************************************************/

#ifdef __cplusplus
extern "C" {
#endif

BPLStatus	OPL_Network_GetDefaultDevIndex( int *pnIndex);
BPLStatus	OPL_Network_GetLinkStatus( int *pnStatus);
BPLStatus   OPL_Network_GetDhcpEnabled(int *pnEnabled );
BPLStatus	OPL_NetworkManager_RegisterListener( OPL_NETWORK_EventCallback callback );
BPLStatus	OPL_NetworkManager_UnRegisterListener( void );

BPLStatus	OPL_NetworkWiFi_ApScanning( int nDevIndex );
void	OPL_NetworkWiFi_RegisterListener( OPL_NETWORK_EventCallback callback );
void	OPL_NetworkWiFi_UnRegisterListener( void );

BPLStatus	OPL_NetworkWiFi_ConnectToAP( int nDevIndex, OplNetworkAccessPoint * ap, const char *szKey );
BPLStatus	OPL_NetworkWiFi_GetConnectedInfo( int nDevIndex, OplNetworkAccessPoint * ap );
BPLStatus	OPL_NetworkWiFi_ConnectToAPbyWPS( int nDevIndex, int nType, const char *szPincode );
BPLStatus	OPL_NetworkWiFi_GetPincodeByWPS( int nDevIndex, HCHAR *szPincode );
BPLStatus   OPL_NetworkWiFi_GetFWVersion(int nDevIndex, char * wifiFWVersion, unsigned int max_len);
BPLStatus   OPL_NetworkWiFi_SetWakeupOnWlan(int nDevIndex, OplNetWoWlan_Mode_t etMode);

HINT32	OPL_NetworkWiFi_ConvertToSecurityInt( int nSecureType );
HINT32	OPL_NetworkWiFi_ConvertToAuthenInt( int nAuthenType );
HINT32	OPL_NetworkWiFi_ConvertToEncryptInt( int nEncryptType );
HINT32  OPL_NetworkWiFi_ConvertToModeInt( int nModeType );

const char * OPL_NetworkWiFi_ConvertToSecuritySTR( int nSecureType );
const char * OPL_NetworkWiFi_ConvertToAuthenSTR( int nAuthenType );
const char * OPL_NetworkWiFi_ConvertToEncryptSTR( int nEncryptType );

void	OPL_NetworkPPPoE_RegisterListener( OPL_NETWORK_EventCallback callback );
void	OPL_NetworkPPPoE_UnRegisterListener( void );
BPLStatus	OPL_NetworkPPPoE_GetInfo( int nDevIndex, char **szId, char **szPassword );
BPLStatus	OPL_NetworkPPPoE_SetInfo( int nDevIndex, const char *szId, const char *szPassword );
BPLStatus	OPL_NetworkPPPoE_GetOnOff( int nDevIndex, BPL_BOOL *bOn );
BPLStatus	OPL_NetworkPPPoE_SetOnOff( int nDevIndex, BPL_BOOL bOn );
BPLStatus	OPL_NetworkPPPoE_Connect( int nDevIndex, const char *szId, const char *szPassword );
BPLStatus	OPL_NetworkPPPoE_Disconnect( int nDevIndex );

//#ifdef CONFIG_MW_INET_FTP
#if 1
BPLStatus OPL_Network_GetFTPStatus( int *pnStatus);
BPLStatus OPL_Network_SetFTPStatus( int nStatus);
#endif

BPLStatus OPL_Network_SetInternetConnection( HINT32 eEvent );
BPLStatus OPL_Network_GetInternetConnection( int *status );


#ifdef __cplusplus
};

/******************************************************************
	class OplNetworkInterface
******************************************************************/

class OplNetworkInterface
{
private :
	typedef enum {
		eADDR_IPv4,
		eADDR_NETMASK,
		eADDR_GATEWAY,
		eADDR_DNS1,
		eADDR_DNS2,

		eADDR_IPV6_TUNNEL_START,
		eADDR_IPV6_TUNNEL_END,

		eADDR_IPV6,
		eAddr_IPV6_GATEWAY,
		eADDR_IPV6_DNS1,
		eADDR_IPV6_DNS2,
	} ADDRESS_Type_e;

	BPL_BOOL	setupByAddressType( ADDRESS_Type_e eAddrType, const char * address );
	static void 	onEventNotifier( HINT32 eEvent, HUINT32 ulParam1, HUINT32 ulParam2, HUINT32 ulParam3  );

public :
	OplNetworkInterface( int nIndex );

	virtual ~OplNetworkInterface();
	BPL_BOOL	setupByIndex( int nIndex );

	BPL_BOOL	refreshData();
	char * 		getIpAddress()	{	return m_szIpAddr; 		}
	char *		getNetmask()	{	return m_szNetmask;		}
	char *		getGateway()	{	return m_szGateway; 	}
	char *		getDNS1()		{	return m_szDns1;		}
	char *		getDNS2()		{	return m_szDns2;		}
	BPL_BOOL	getDHCPStatus()	{	return m_bDhcpOn;		}
	BPL_BOOL	getDNSAutoStatus()	{	return m_bDnsAuto;	}

 	BPL_BOOL	getIpv6Type()	{	return m_bIpv6Type;		}
	char *		getIpv6Address(){	return m_szIpv6Addr;	}
	int	 		getIpv6Prefix()	{	return m_nIpv6Prefix;	}
	char *		getIpv6Gateway(){	return m_szIpv6Gateway;	}
	char *		getIpv6DNS1()	{	return m_szIpv6Dns1;	}
	char *		getIpv6DNS2()	{	return m_szIpv6Dns2;	}
	BPL_BOOL	getIpv6DhcpOn() {	return m_bIpv6DhcpOn;	}
	BPL_BOOL	getIpv6DnsAuto(){	return m_bIpv6DnsAuto;	}
	char *		getIpv6TunnelStart(){	return m_szIpv6TnlStr;	}
	char *		getIpv6TunnelEnd()	{	return m_szIpv6TnlEnd;	}

	char *		getInterfaceName()		{	return m_szInterfaceName;	}
	char *		getMacAddress()			{	return m_szMacAddress;		}
	BPL_BOOL	getConnected();
	BPL_BOOL	getEnabled();
	BPL_BOOL	getIpv6StatelessMode()	{	return m_bIpv6StatelessMode;}

	eOplNetType	getNetworkType(){	return m_eNetType;		}



	BPL_BOOL	setEnabled( BPL_BOOL bOn );

	BPL_BOOL	setIpAddress( const char * pszAddress )
	{	return setupByAddressType( eADDR_IPv4, pszAddress );	}

	BPL_BOOL	setNetmask( const char * pszNetmask )
	{	return setupByAddressType( eADDR_NETMASK, pszNetmask );	}

	BPL_BOOL	setGateway( const char * pszGateway )
	{	return setupByAddressType( eADDR_GATEWAY, pszGateway );	}

	BPL_BOOL	setDNS1( const char * pszDns1 )
	{	return setupByAddressType( eADDR_DNS1, pszDns1 );	}

	BPL_BOOL	setDNS2( const char * pszDns2 )
	{	return setupByAddressType( eADDR_DNS2, pszDns2 );	}

	BPL_BOOL	setDHCPStatus( BPL_BOOL bOn )
	{	m_bDhcpOn = bOn; return TRUE;	}

	BPL_BOOL	setDNSAutoStatus( BPL_BOOL bOn )
	{	m_bDnsAuto = bOn; return TRUE;	}

	BPL_BOOL	setIpv6Type( BPL_BOOL bType )
	{	m_bIpv6Type = bType; return TRUE;	}

	BPL_BOOL	setIpv6Address( const char * pszAddress )
	{	return setupByAddressType( eADDR_IPV6, pszAddress );	}

	BPL_BOOL	setIpv6Prefix( const int nPrefix)
	{	m_nIpv6Prefix = nPrefix; return TRUE;	}

	BPL_BOOL	setIpv6Gateway( const char * pszGateway )
	{ 	return setupByAddressType( eAddr_IPV6_GATEWAY, pszGateway );	}

	BPL_BOOL	setIpv6DNS1( const char * pszDns1 )
	{	return setupByAddressType( eADDR_IPV6_DNS1, pszDns1 );	}

	BPL_BOOL	setIpv6DNS2( const char * pszDns2 )
	{	return setupByAddressType( eADDR_IPV6_DNS2, pszDns2 );	}

	BPL_BOOL	setIpv6TunnelStartAddr( const char * pszStartAddr)
	{	return setupByAddressType( eADDR_IPV6_TUNNEL_START, pszStartAddr);	}

	BPL_BOOL	setIpv6TunnalEndAddr( const char * pszEndAddr)
	{	return setupByAddressType( eADDR_IPV6_TUNNEL_END, pszEndAddr);	}

	BPL_BOOL	setInterfaceName( const char * pszName);

	BPL_BOOL	setIpv6DhcpOn( BPL_BOOL bOn )
	{	m_bIpv6DhcpOn = bOn; return TRUE;	}

	BPL_BOOL	setIpv6DnsSAuto( BPL_BOOL bOn )
	{	m_bIpv6DnsAuto = bOn; return TRUE;	}

	BPL_BOOL	setIpv6StatelessMode( BPL_BOOL bOn )
	{	m_bIpv6StatelessMode = bOn; return TRUE;	}

	BPL_BOOL	applyConfigure( void );
	BPL_BOOL	stopConnecting( void );

	int	checkGatewayAccess( void );
	int	checkDnsAccess( void );
	int	checkInternetConnection( void );
#if defined(CONFIG_OP_JAPAN)
	BPL_BOOL	get(char *szCmd, char **szRetVal);
	BPL_BOOL	set(char *szCmd, char *szParam);
#endif
	int			getIndex()
	{	return m_nIndex; 	}

	static void 	registerEventListener( OplNetworkInterface *netInterface, OPL_NETWORK_EventCallback callback );
	static void		unRegisterEventListener( OplNetworkInterface *netInterface );
	static void		refreshDataAll();
	static void		refreshDataAll(HINT32 nIndex);

private :

	int				m_nIndex;

	char			m_szIpAddr[OPLNET_MAX_IPADDR_LEN];		/** the IP address */
	char			m_szNetmask[OPLNET_MAX_IPADDR_LEN];		/** the netmask */
	char			m_szGateway[OPLNET_MAX_IPADDR_LEN];		/** the Gateway address */
	char			m_szDns1[OPLNET_MAX_IPADDR_LEN];		/** the primary DNS address */
	char			m_szDns2[OPLNET_MAX_IPADDR_LEN];		/** the secondary DNS address */

	char			m_szIpv6TnlStr[OPLNET_MAX_IPADDR_LEN];		/** the ipv6-in-ipv4 tunnel start address*/
	char			m_szIpv6TnlEnd[OPLNET_MAX_IPADDR_LEN];		/** the ipv6-in-ipv4 tunnel end address */

	char			m_szIpv6Addr[OPLNET_MAX_IPV6ADDR_LEN];		/** the ipv6 address */
	char			m_szIpv6Gateway[OPLNET_MAX_IPV6ADDR_LEN];	/** the ipv6 Gateway address */
	char			m_szIpv6Dns1[OPLNET_MAX_IPV6ADDR_LEN];		/** the ipv6 primary DNS address */
	char			m_szIpv6Dns2[OPLNET_MAX_IPV6ADDR_LEN];		/** the ipv6 secondary DNS address */

	char			m_szMacAddress[OPLNET_MAX_MACADDR_LEN];		/** the MAC address */
	char			m_szInterfaceName[OPLNET_MAX_NAME_LEN];		/** the Interface Name */

	int				m_nIpv6Prefix;								/** the ipv6 prefix*/

	BPL_BOOL		m_bDhcpOn;
	BPL_BOOL		m_bDnsAuto;

	BPL_BOOL		m_bIpv6Type;
	BPL_BOOL		m_bIpv6DhcpOn;
	BPL_BOOL		m_bIpv6DnsAuto;
	BPL_BOOL		m_bIpv6StatelessMode;

	BPL_BOOL		m_bConnected;				/** if the network link is connected */
	BPL_BOOL		m_bIpSetup;				/** if the network link is connected */
	BPL_BOOL		m_bEnabled;					/** if your box has the network interface */
	eOplNetType		m_eNetType;					/** wired or wirelessa LAN */

#if defined(CONFIG_OP_JAPAN)
	OplNetworkCmd Cmds[21];
#endif

	int				m_gwReqId;
	int				m_dnsReqId;

	static OPL_NETWORK_EventCallback *m_sfnNetworkEventCallback;
	static std::list<OplNetworkInterface *> m_sInstanceList;

};
#endif

#endif


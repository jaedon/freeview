/**************************************************************
 *	@file		apk_network_netconfig.c
 *	appkit for network
 *
 *	http://www.humaxdigital.com
 *
 *	@data			2013-07-15
 *	@author			humax
 *	@brief			appkit for network
 **************************************************************/
#include 	<apk.h>
#include 	<oapi.h>
#include 	<napi.h>

#if defined(CONFIG_SUPPORT_NIMA)

HERROR	APK_NETWORK_DHCPSVR_Init(void)
{
	// just because of using corsair not prism,
	// do not create 2 task by corsair, prism.
#if defined(CONFIG_OP_TDC_NETWORK_LED)
	APK_NETWORK_InitTask();
#endif

	return ERR_OK;
}

HERROR	APK_NETWORK_DHCPSVR_DeInit(void)
{
	return ERR_OK;
}

HERROR		APK_NETWORK_DHCPSVR_UpdateInfo(void)
{
	/* empty */
	return ERR_OK;
}

HERROR		APK_NETWORK_DHCPSVR_IsRunning(HBOOL *pbRunnig)
{

	NAPIDHCPServerInfo_t dhcpserver_info;

	NAPI_NetConf_GetDHCPServerInfo(&dhcpserver_info);

	*pbRunnig = dhcpserver_info.bActive;

	return ERR_OK;
}


HERROR		APK_NETWORK_DHCPSVR_GetDevIndex(HINT32 *pnDevIndex)
{
	NAPIDHCPServerInfo_t dhcpserver_info;

	NAPI_NetConf_GetDHCPServerInfo(&dhcpserver_info);

	*pnDevIndex = dhcpserver_info.id;

	return ERR_OK;
}

HERROR		APK_NETWORK_DHCPSVR_Start( HINT32 nIndex )
{

	NAPI_NetConf_StartDHCPServer();

	return ERR_OK;
}

HERROR		APK_NETWORK_DHCPSVR_Stop( HINT32 nIndex )
{
	NAPI_NetConf_StopDHCPServer();

	return ERR_OK;
}

#else



HERROR	APK_NETWORK_DHCPSVR_Init(void)
{
	return ERR_FAIL;
}

HERROR	APK_NETWORK_DHCPSVR_DeInit(void)
{
	return ERR_FAIL;
}

HERROR		APK_NETWORK_DHCPSVR_UpdateInfo(void)
{
	return ERR_FAIL;
}

HERROR		APK_NETWORK_DHCPSVR_IsRunning(HBOOL *pbRunnig)
{
	return ERR_FAIL;
}


HERROR		APK_NETWORK_DHCPSVR_GetDevIndex(HINT32 *pnDevIndex)
{
	return ERR_FAIL;
}

HERROR		APK_NETWORK_DHCPSVR_Start( HINT32 nIndex )
{
	return ERR_FAIL;
}

HERROR		APK_NETWORK_DHCPSVR_Stop( HINT32 nIndex )
{
	return ERR_FAIL;
}


#endif

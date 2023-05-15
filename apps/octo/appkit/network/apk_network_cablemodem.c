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

HERROR		APK_NETWORK_CM_Init(void)
{
	return ERR_OK;
}

HERROR		APK_NETWORK_CM_DeInit(void)
{
	return ERR_OK;
}

HERROR		APK_NETWORK_CM_GetStatus( APKE_NETCM_Status_e *peStatus )
{
	/* empty */
	return ERR_OK;
}

HERROR		APK_NETWORK_CM_GetDownStreamInfo( APKE_NETCM_DownInfo_e eDownInfo, HINT32 *pnValue )
{
	/* empty */
	return ERR_OK;
}

HERROR		APK_NETWORK_CM_SetDownStreamInfo( APKE_NETCM_DownInfo_e eDownInfo, HINT32 nValue )
{

	/* empty */
	return ERR_OK;
}
HERROR		APK_NETWORK_CM_GetUpStreamInfo( APKE_NETCM_UpInfo_e eUpInfo, HINT32 *pnValue )
{

	/* empty */
	return ERR_OK;
}

#else



HERROR		APK_NETWORK_CM_Init(void)
{
	return ERR_FAIL;
}

HERROR		APK_NETWORK_CM_DeInit(void)
{
	return ERR_FAIL;
}

HERROR		APK_NETWORK_CM_GetStatus( APKE_NETCM_Status_e *peStatus )
{
	return ERR_FAIL;
}

HERROR		APK_NETWORK_CM_GetDownStreamInfo( APKE_NETCM_DownInfo_e eDownInfo, HINT32 *pnValue )
{
	return ERR_FAIL;
}

HERROR		APK_NETWORK_CM_SetDownStreamInfo( APKE_NETCM_DownInfo_e eDownInfo, HINT32 nValue )
{
	return ERR_FAIL;
}
HERROR		APK_NETWORK_CM_GetUpStreamInfo( APKE_NETCM_UpInfo_e eUpInfo, HINT32 *pnValue )
{
	return ERR_FAIL;
}


#endif

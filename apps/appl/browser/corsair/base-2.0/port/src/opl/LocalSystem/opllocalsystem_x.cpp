/**************************************************************************************/
/**
 * @file bpllocalsystem.cpp
 *
 * Local System Information
 *
 **************************************************************************************
 **/

#include "opllocalsystem.h"

#include "oapi.h"
#include "hapi.h"

#include <apk.h>
#include <hlib.h>
#include <map>

using namespace std;

/******************************************************************
 class OplLocalSystem
******************************************************************/

int OplLocalSystem::getAVOutputsCount(void)
{
	return (int)0;
}

OplAVOutput *OplLocalSystem::getAVOutputByIndex(int nIndex)
{
	OplAVOutput *pOutput = new OplAVOutput();
	if ( !pOutput )
	{
		return NULL;
	}

	/* TODO: */

	return pOutput;
}

int OplLocalSystem::getNetworkInterfacesCount(void)
{
	int nCount;

	if (APK_NETWORK_CONF_GetNumOfNetDev(&nCount) != ERR_OK)
	{
		HxLOG_Error("Getting the number of Network Interfaces is failed.\n");
		return -1;
	}

	return nCount;
}

OplNetworkInterface *OplLocalSystem::getNetworkInterfaceByIndex(int nIndex)
{
	OplNetworkInterface *pNetif = new OplNetworkInterface( nIndex );
	if ( !pNetif )
	{
		return NULL;
	}
	//pNetif->setupByIndex( nIndex );

	return pNetif;
}

#if defined(CONFIG_OP_JAPAN)
int OplLocalSystem::getNetworkServiceCount(void)
{
	return OplNetworkService::getNumOfSupportService();
}

OplNetworkService *OplLocalSystem::getNetworkService(int index)
{
	OplNetworkService *pNetSrc = new OplNetworkService(index);
	if ( !pNetSrc)
	{
		return NULL;
	}
	return pNetSrc;
}

int OplLocalSystem::getWiFiAccessPointCount(void)
{
	return WiFiScanningService::GetApCount();
}

OplNetworkAccessPoint *OplLocalSystem::getWiFiAccessPoint(int index)
{
	int apCount = WiFiScanningService::GetApCount();
	OplNetworkAccessPoint *pApList = (OplNetworkAccessPoint *)WiFiScanningService::GetApList();
	if (!pApList || index >= apCount )
	{
		return NULL;
	}
	OplNetworkAccessPoint *pApInfo = (OplNetworkAccessPoint *)HLIB_STD_MemAlloc(sizeof(OplNetworkAccessPoint));
	if ( !pApInfo)
	{
		return NULL;
	}
	HxSTD_MemCopy( pApInfo, &pApList[index], sizeof(OplNetworkAccessPoint) );

	return pApInfo;
}

#endif


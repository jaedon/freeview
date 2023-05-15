/**************************************************************************************/
/**
 * @file NativeJlabsDTTChannel.cpp
 *
 * jlabsDTTChanne module
 *
 * @author  Seo-Lim Park(slpark@humaxdigital.com)
 * @date    2012/03/15
 * @attention   Copyright (c) 2011 Humax - All rights reserved.
 *
 **************************************************************************************
 **/

#if defined OIPF && defined JLABS

#undef	OPL_jlabsChannel

#include "NativeJlabsChannel.h"
#include "macros.h"

#include "oplchannel.h"
#include "oplbroadcast.h"
#include <hlib.h>

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define	__TRACE()					do{OOIF_LOG_DUMMY("[%s:%d] trace\n", __FUNCTION__, __LINE__);}while(0)
#define	__RETURN_ERROR_IF(cond)		do{if(cond){OOIF_LOG_ERROR("[%s:%d] error \'%s\'\n", __FUNCTION__, __LINE__, #cond);return OOIF_RETURN_ERROR;}}while(0)
#define	__RETURN_NULL_IF(cond)		do{if(cond){OOIF_LOG_WARNING("[%s:%d] null \'%s\'\n", __FUNCTION__, __LINE__, #cond);return OOIF_RETURN_VALUE_NULL;}}while(0)
#define	__RETURN_UNDEFINED_IF(cond)	do{if(cond){OOIF_LOG_WARNING("[%s:%d] undefined \'%s\'\n", __FUNCTION__, __LINE__, #cond);return OOIF_RETURN_VALUE_UNDEFINED;}}while(0)
#define	__RETURN_OK_IF(cond)		do{if(cond)return OOIF_RETURN_ERROR;}while(0)
#define	MAX_BUF_LEN 32

resultCreateMaskListener *g_onResultCreateMask =NULL;


OOIFReturnCode native_jlabsChannelConfigGetChannelListsByNetType(jlabsChannelListNetwork network, int *pNumOfNetwork, int ** ppNumOfChannelsByNetwork, jlabsChannelListHandle **handles)
{
	OOIF_LOG_DUMMY("channel list of (%d)", (int)network);

	OPL_ChannelManager_t	manager;
	jlabsChannelListHandle 	*pChannelLists = NULL;
	jlabsChannelHandle		*pChannels = NULL;
	int					*pNetwork = NULL, *pNumOfChannelByNetwork = NULL;
	int					i, numOfNetwork = 0, numOfChannels = 0;
	OPL_NetworkType_e	netType;
	__TRACE();
	if((pNumOfNetwork == NULL) || (handles == NULL) || (ppNumOfChannelsByNetwork == NULL))
	{
		OOIF_LOG_ERROR("params are NULL [%s : %d]\n",__FUNCTION__,__LINE__);
		return OOIF_RETURN_VALUE_NULL;
	}
	*pNumOfNetwork = 0;
	*handles = NULL;
	*ppNumOfChannelsByNetwork = NULL;

	switch(network)
	{
		case JLABS_DTT_NETWORK:
			netType = eOPL_DTT_NETWORK;
			break;
		case JLABS_BS_NETWORK:
			netType = eOPL_BS_NETWORK;
			break;
		case JLABS_CATV_NETWORK:
			netType = eOPL_CATV_NETWORK;
			break;
		case JLABS_CS1_NETWORK:
			netType = eOPL_CS1_NETWORK;
			break;
		case JLABS_CS2_NETWORK:
			netType = eOPL_CS2_NETWORK;
			break;
		case JLABS_JCHITS_NETWORK:
			netType = eOPL_JCHITS_NETWORK;
			break;
		default:
			OOIF_LOG_ERROR("network(%d) is out of bound [%s : %d]\n",network,__FUNCTION__,__LINE__);
			return OOIF_RETURN_VALUE_NULL;
	}

	manager = OPL_Channel_GetManager();
	__RETURN_NULL_IF(!manager);

	numOfNetwork = OPL_Channel_GetNetworkIDList(netType, manager, &pNetwork);
	if((numOfNetwork == 0) || (pNetwork == NULL))
	{
		OOIF_LOG_ERROR("numOfNetwork (%d), pNetwork (%p) [%s : %d]\n",numOfNetwork,pNetwork, __FUNCTION__,__LINE__);
		goto funcExit;
	}
	pChannelLists = (jlabsChannelListHandle*)HLIB_STD_MemAlloc((unsigned int)(numOfNetwork*sizeof(jlabsChannelListHandle)));
	pNumOfChannelByNetwork = (int*)HLIB_STD_MemAlloc((unsigned int)(numOfNetwork*sizeof(int)));

	if((pChannelLists == NULL) || (pNumOfChannelByNetwork == NULL))
	{
		OOIF_LOG_ERROR("out of memory!![%s : %d]\n",__FUNCTION__,__LINE__);
		goto funcExit;
	}
	*pNumOfNetwork= numOfNetwork;
	for(i = 0; i <numOfNetwork; i++)
	{
		numOfChannels = OPL_Channel_GetChannelListByOnID(pNetwork[i], manager, (OPL_Channel_t**)&pChannels);
		pNumOfChannelByNetwork[i] = numOfChannels;
		pChannelLists[i] = (jlabsChannelListHandle)pChannels;
	}
	*ppNumOfChannelsByNetwork = pNumOfChannelByNetwork;
	*handles = pChannelLists;

	if(pNetwork != NULL)
	{
		OPL_Channel_FreeNetworkIDList(pNetwork);
	}
	return OOIF_RETURN_OK;
funcExit:

	if(pNetwork != NULL)
	{
		OPL_Channel_FreeNetworkIDList(pNetwork);
	}
	if(pChannelLists != NULL)
	{
		HLIB_STD_MemFree(pChannelLists);
	}
	if(pNumOfChannelByNetwork != NULL)
	{
		HLIB_STD_MemFree(pNumOfChannelByNetwork);
	}
	return OOIF_RETURN_VALUE_NULL;
}

OOIFReturnCode native_jlabsChannelConfigFreeChList(jlabsChannelListHandle pHandle)
{
	OOIF_LOG_DUMMY("Releasing channel with handle=%d", (int)pHandle);
	if(pHandle != NULL)
	{
		HLIB_STD_MemFree(pHandle);
	}
	return OOIF_RETURN_OK;
}

OOIFReturnCode native_jlabsChannelConfigFreeNumOfChannel(int *pHandle)
{
	OOIF_LOG_DUMMY("Releasing numOfChannel handle=%d", (int)pHandle);
	if(pHandle != NULL)
	{
		HLIB_STD_MemFree(pHandle);
	}
	return OOIF_RETURN_OK;
}

OOIFReturnCode native_jlabsChannelReleaseHandle(jlabsChannelHandle id)
{
	OOIF_LOG_DUMMY("Releasing channel with handle=%d", (int)id);
	OPL_Channel_Delete(id);
	return OOIF_RETURN_OK;
}

OOIFReturnCode native_jlabsChannelGetField(jlabsChannelHandle identifier, const char *field, const char **retval)
{
#ifdef OPL_jlabsChannel
	OPL_jlabsChannel_t	channel = OPL_jlabsChannel_GetChannel(NULL, (int)identifier);
	char *value = OPL_jlabsChannel_GetField(channel, field, NULL, 0);
#endif

	__TRACE();
#ifdef OPL_jlabsChannel
	__RETURN_ERROR_IF(channel == NULL || field == NULL);
	__RETURN_NULL_IF(value == NULL);
#endif

#ifdef OPL_jlabsChannel
	*retval = (const char *)value;
#else
	*retval = "jlabsChannelGetField";
#endif
	return OOIF_RETURN_OK;
}

OOIFReturnCode native_jlabsChannelGetLogo(jlabsChannelHandle identifier, OOIFNumber width, OOIFNumber height, const char **retval)
{
	OPL_Channel_t	channel = (OPL_Channel_t)identifier;

	__TRACE();
	__RETURN_ERROR_IF(channel == NULL);

	*retval = OPL_Channel_GetLogo(channel, width, height);

	return OOIF_RETURN_OK;
}

OOIFReturnCode native_jlabsChannelGetProviderName(jlabsChannelHandle identifier, const char **retval)
{
	OPL_Channel_t	channel = (OPL_Channel_t)identifier;
	static char		ProvName[256];

	__TRACE();
	__RETURN_ERROR_IF(channel == NULL);
	__RETURN_ERROR_IF(retval == NULL);

	memset( ProvName , 0x00 , 256 );
	if((OPL_Channel_GetProviderName(channel, ProvName) == BPL_STATUS_OK) && (strlen(ProvName) > 0))
	{
		*retval = ProvName;
		return OOIF_RETURN_OK;
	}

	*retval = "";
	return OOIF_RETURN_VALUE_NULL;
}

OOIFReturnCode native_jlabsChannelGetInvisible(jlabsChannelHandle identifier, OOIFBoolean *retval)
{
	OPL_Channel_t	channel = (OPL_Channel_t)identifier;

	__TRACE();
	__RETURN_ERROR_IF(channel == NULL);

	*retval = OPL_Channel_IsSkipChannel(channel) ? TRUE : FALSE;
	return OOIF_RETURN_OK;
}

OOIFReturnCode native_jlabsChannelSetInvisible(jlabsChannelHandle identifier, OOIFBoolean invisible)
{
	OPL_Channel_t	channel = (OPL_Channel_t)identifier;

	__TRACE();
	__RETURN_ERROR_IF(channel == NULL);

	OPL_Channel_SetSkipChannel(channel, invisible ? OPL_TRUE : OPL_FALSE);
	return OOIF_RETURN_OK;
}

OOIFReturnCode native_jlabsChannelGetOneTouch(jlabsChannelHandle identifier, OOIFNumber *retval)
{
	OPL_Channel_t	channel = (OPL_Channel_t)identifier;
	__TRACE();
	__RETURN_ERROR_IF(channel == NULL);
	__RETURN_ERROR_IF(retval == NULL);
	*retval = (OOIFNumber)OPL_Channel_GetOneTouch(channel);
	return OOIF_RETURN_OK;
}

OOIFReturnCode native_jlabsChannelSetOneTouch(jlabsChannelHandle identifier, OOIFNumber oneTouch)
{
	OPL_Channel_t	channel = (OPL_Channel_t)identifier;
	__TRACE();
	__RETURN_ERROR_IF(channel == NULL);

	OPL_Channel_SetOneTouch(channel, (int)oneTouch);
	return OOIF_RETURN_OK;
}

/* TS name */
OOIFReturnCode native_jlabsChannelListGetName(jlabsChannelListHandle identifier, int length, const char **retval)
{
	OPL_Channel_t	*pChannels = (OPL_Channel_t*)identifier;
	OPL_Channel_t	channel;
	char		acTsName[MAX_BUF_LEN];
	__TRACE();
	__RETURN_ERROR_IF(pChannels == NULL);
	__RETURN_ERROR_IF(retval == NULL);
	if(length <=0)
	{
		goto funcErr;
	}
	channel = OPL_Channel_GetRepresentativeChannelInChannelList(pChannels, length);
	if(channel != NULL)
	{
		HxSTD_MemSet(acTsName, 0x00, MAX_BUF_LEN);
		if(OPL_Channel_GetTsName(channel, acTsName, MAX_BUF_LEN) == BPL_STATUS_OK)
		{
			*retval = acTsName;
			return OOIF_RETURN_OK;
		}
	}
funcErr:
	*retval = "";
	return OOIF_RETURN_VALUE_NULL;

}

OOIFReturnCode native_jlabsChannelListGetAreaCode(jlabsChannelListHandle identifier, int length, OOIFNumber *retval)
{
	OPL_Channel_t	*pChannels = (OPL_Channel_t*)identifier;
	OPL_Channel_t	channel;
	__TRACE();
	__RETURN_ERROR_IF(pChannels == NULL);
	__RETURN_ERROR_IF(retval == NULL);
	if(length <=0)
	{
		*retval= -1;
		return OOIF_RETURN_OK;
	}
	channel = OPL_Channel_GetRepresentativeChannelInChannelList(pChannels, length);
	if(channel != NULL)
	{
		*retval = static_cast<OOIFNumber>(OPL_Channel_GetAreacode(channel));
	}
	else
	{
		*retval = -1;
	}
	return OOIF_RETURN_OK;
}

OOIFReturnCode native_jlabsChannelListGetSignalLevel(jlabsChannelListHandle identifier, int length, const char **retval)
{
	OPL_Channel_t	*pChannels = (OPL_Channel_t*)identifier;
	OPL_Channel_t	channel;
	unsigned int		ulAgc = 0, ulBer = 0,ulSnr = 0;
	int				nStrength = 0;
	static char		s_szSignalLevel[MAX_BUF_LEN];

	__TRACE();
	__RETURN_ERROR_IF(pChannels == NULL);
	__RETURN_ERROR_IF(retval == NULL);

	if(length <=0)
	{
		goto funcErr;
	}
	HxSTD_MemSet(s_szSignalLevel, 0x00, MAX_BUF_LEN);

	channel = OPL_Channel_GetRepresentativeChannelInChannelList(pChannels, length);
	if(channel != NULL)
	{
		if((OPL_Channel_GetSignalLevel(channel, &ulAgc, &ulBer, &ulSnr, &nStrength) == BPL_STATUS_OK))
		{
			HxSTD_snprintf(s_szSignalLevel, MAX_BUF_LEN, "%d.%d",nStrength,0);
			s_szSignalLevel[MAX_BUF_LEN-1] = '\0';
			*retval = s_szSignalLevel;
			return OOIF_RETURN_OK;
		}
	}
funcErr:
	*retval = "";
	return OOIF_RETURN_VALUE_NULL;
}


OOIFReturnCode native_jlabsChannelListGetAgc(jlabsChannelListHandle identifier, int length, OOIFNumber *retval)
{
	OPL_Channel_t	*pChannels = (OPL_Channel_t*)identifier;
	OPL_Channel_t	channel;
	unsigned int		ulAgc = 0, ulBer = 0,ulSnr = 0;
	int				nStrength = 0;

	__TRACE();
	__RETURN_ERROR_IF(pChannels == NULL);
	__RETURN_ERROR_IF(retval == NULL);

	if(length <=0)
	{
		goto funcErr;
	}

	channel = OPL_Channel_GetRepresentativeChannelInChannelList(pChannels, length);
	if(channel != NULL)
	{
		if((OPL_Channel_GetSignalLevel(channel, &ulAgc, &ulBer, &ulSnr, &nStrength) == BPL_STATUS_OK))
		{
			*retval = static_cast<OOIFNumber>(ulAgc);
			return OOIF_RETURN_OK;
		}
	}
funcErr:
	*retval = 0;
	return OOIF_RETURN_ERROR;
}

OOIFReturnCode native_jlabsChannelListGetBer(jlabsChannelListHandle identifier, int length, OOIFNumber *retval)
{
	OPL_Channel_t	*pChannels = (OPL_Channel_t*)identifier;
	OPL_Channel_t	channel;
	unsigned int		ulAgc = 0, ulBer = 0,ulSnr = 0;
	int				nStrength = 0;

	__TRACE();
	__RETURN_ERROR_IF(pChannels == NULL);
	__RETURN_ERROR_IF(retval == NULL);

	if(length <=0)
	{
		goto funcErr;
	}

	channel = OPL_Channel_GetRepresentativeChannelInChannelList(pChannels, length);
	if(channel != NULL)
	{
		if((OPL_Channel_GetSignalLevel(channel, &ulAgc, &ulBer, &ulSnr, &nStrength) == BPL_STATUS_OK))
		{
			*retval = static_cast<OOIFNumber>(ulBer);
			return OOIF_RETURN_OK;
		}
	}
funcErr:
	*retval = 0;
	return OOIF_RETURN_ERROR;
}

OOIFReturnCode native_jlabsChannelListGetSnr(jlabsChannelListHandle identifier, int length, OOIFNumber *retval)
{
	OPL_Channel_t	*pChannels = (OPL_Channel_t*)identifier;
	OPL_Channel_t	channel;
	unsigned int		ulAgc = 0, ulBer = 0,ulSnr = 0;
	int				nStrength = 0;

	__TRACE();
	__RETURN_ERROR_IF(pChannels == NULL);
	__RETURN_ERROR_IF(retval == NULL);

	if(length <=0)
	{
		goto funcErr;
	}

	channel = OPL_Channel_GetRepresentativeChannelInChannelList(pChannels, length);
	if(channel != NULL)
	{
		if((OPL_Channel_GetSignalLevel(channel, &ulAgc, &ulBer, &ulSnr, &nStrength) == BPL_STATUS_OK))
		{
			*retval = ulSnr;
			return OOIF_RETURN_OK;
		}
	}
funcErr:
	*retval = 0;
	return OOIF_RETURN_ERROR;
}

OOIFReturnCode native_jlabsChannelListGetBranchNumber(jlabsChannelListHandle identifier, int length, OOIFNumber *retval)
{
	OPL_Channel_t	*pChannels = (OPL_Channel_t*)identifier;
	OPL_Channel_t	channel;
	__TRACE();
	__RETURN_ERROR_IF(pChannels == NULL);
	__RETURN_ERROR_IF(retval == NULL);
	if(length <=0)
	{
		*retval= -1;
		return OOIF_RETURN_OK;
	}
	channel = OPL_Channel_GetRepresentativeChannelInChannelList(pChannels, length);
	if(channel != NULL)
	{
		*retval = (OOIFNumber)OPL_Channel_GetBranchNum(channel);
	}
	else
	{
		*retval = -1;
	}
	return OOIF_RETURN_OK;
}

OOIFReturnCode native_jlabsChannelListGetOneTouch(jlabsChannelListHandle identifier, int length, OOIFNumber *retval)
{
	OPL_Channel_t	*pChannels = (OPL_Channel_t*)identifier;
	OPL_Channel_t	channel;
	__TRACE();
	__RETURN_ERROR_IF(pChannels == NULL);
	__RETURN_ERROR_IF(retval == NULL);
	if(length <=0)
	{
		*retval= -1;
		return OOIF_RETURN_OK;
	}
	channel = OPL_Channel_GetRepresentativeChannelInChannelList(pChannels, length);
	if(channel != NULL)
	{
		*retval = (OOIFNumber)OPL_Channel_GetOneTouch(channel);
	}
	else
	{
		*retval = -1;
	}
	return OOIF_RETURN_OK;
}

OOIFReturnCode native_jlabsChannelListSetOneTouch(jlabsChannelListHandle identifier, int length, OOIFNumber value)
{
	OPL_Channel_t	*pChannels = (OPL_Channel_t*)identifier;
	OPL_Channel_t	channel;
	__TRACE();
	__RETURN_ERROR_IF(pChannels == NULL);
	if(length <=0)
	{
		return OOIF_RETURN_OK;
	}
	channel = OPL_Channel_GetRepresentativeChannelInChannelList(pChannels, length);
	if(channel != NULL)
	{
		OPL_Channel_SetOneTouch(channel,  (int)value);
	}
	return OOIF_RETURN_OK;
}

OOIFReturnCode native_jlabsChannelListReleaseChListHandle(jlabsChannelHandle *pHandle)
{
	OPL_Channel_FreeChannelList((OPL_Channel_t*)pHandle);
	return OOIF_RETURN_OK;
}

OOIFReturnCode native_jlabsChannelListCollectionGetNetworkTypeName(jlabsChannelListNetwork eNetworkTypeName, const char **retval)
{
	static char		s_szNetworkTypeName[MAX_BUF_LEN];
	__TRACE();
	__RETURN_ERROR_IF(retval == NULL);
	HxSTD_MemSet(s_szNetworkTypeName, 0x00, MAX_BUF_LEN);
	switch(eNetworkTypeName)
	{
		case JLABS_DTT_NETWORK:
			HxSTD_snprintf(s_szNetworkTypeName, MAX_BUF_LEN, "%d: %s",JLABS_DTT_NETWORK,"JLABS_DTT_NETWORK");
			break;
		case JLABS_BS_NETWORK:
			HxSTD_snprintf(s_szNetworkTypeName, MAX_BUF_LEN, "%d: %s",JLABS_BS_NETWORK,"JLABS_BS_NETWORK");
			break;
		case JLABS_CATV_NETWORK:
			HxSTD_snprintf(s_szNetworkTypeName, MAX_BUF_LEN, "%d: %s",JLABS_CATV_NETWORK,"JLABS_CATV_NETWORK");
			break;
		case JLABS_ALL_NETWORK:
			HxSTD_snprintf(s_szNetworkTypeName, MAX_BUF_LEN, "%d: %s",JLABS_ALL_NETWORK,"JLABS_ALL_NETWORK");
			break;
		case JLABS_CS1_NETWORK:
			HxSTD_snprintf(s_szNetworkTypeName, MAX_BUF_LEN, "%d: %s",JLABS_CS1_NETWORK,"JLABS_CS1_NETWORK");
			break;
		case JLABS_CS2_NETWORK:
			HxSTD_snprintf(s_szNetworkTypeName, MAX_BUF_LEN, "%d: %s",JLABS_CS2_NETWORK,"JLABS_CS2_NETWORK");
			break;
		case JLABS_JCHITS_NETWORK:
			HxSTD_snprintf(s_szNetworkTypeName, MAX_BUF_LEN, "%d: %s",JLABS_JCHITS_NETWORK,"JLABS_JCHITS_NETWORK");
			break;
		default:
			OOIF_LOG_ERROR("network(%d) is out of bound [%s : %d]\n",eNetworkTypeName,__FUNCTION__,__LINE__);
			return OOIF_RETURN_VALUE_NULL;
	}
	*retval= s_szNetworkTypeName;
	return OOIF_RETURN_OK;
}


OOIFReturnCode native_jlabsChannelListCollectionGetLastUpdateDate(jlabsChannelListNetwork networkTypeName, OOIFNumber *retval)
{
	OPL_ChannelManager_t	manager;
	OPL_NetworkType_e netType = eOPL_NETWORK_END;
	jlabsChannelHandle	*pChannels = NULL;
	int				*pNetwork = NULL;
	int				numOfNetwork = 0, numOfChannels = 0, i = 0;
	int				lastUpdateDate = 0, latestUpdateDate = 0;
	__TRACE();
	switch(networkTypeName)
	{
		case JLABS_DTT_NETWORK:
			netType = eOPL_DTT_NETWORK;
			break;
		case JLABS_BS_NETWORK:
			netType = eOPL_BS_NETWORK;
			break;
		case JLABS_CATV_NETWORK:
			netType = eOPL_CATV_NETWORK;
			break;
		case JLABS_CS1_NETWORK:
			netType = eOPL_CS1_NETWORK;
			break;
		case JLABS_CS2_NETWORK:
			netType = eOPL_CS2_NETWORK;
			break;
		case JLABS_JCHITS_NETWORK:
			netType = eOPL_JCHITS_NETWORK;
			break;
		default:
			OOIF_LOG_ERROR("network(%d) is out of bound [%s : %d]\n",networkTypeName,__FUNCTION__,__LINE__);
			return OOIF_RETURN_VALUE_NULL;
	}
	manager = OPL_Channel_GetManager();
	__RETURN_NULL_IF(!manager);

	numOfNetwork = OPL_Channel_GetNetworkIDList(netType, manager, &pNetwork);
	if((numOfNetwork == 0) || (pNetwork == NULL))
	{
		OOIF_LOG_ERROR("numOfNetwork (%d), pNetwork (%p) [%s : %d]\n",numOfNetwork,pNetwork, __FUNCTION__,__LINE__);
		goto funcExit;
	}
	for(i =0; i < numOfNetwork; i++)
	{
		numOfChannels = OPL_Channel_GetChannelListByOnID(pNetwork[i], manager, (OPL_Channel_t**)&pChannels);
		if((numOfChannels == 0) || (pChannels == NULL))
		{
			OOIF_LOG_ERROR("numOfNetwork (%d), pNetwork (%p) [%s : %d]\n",numOfNetwork,pNetwork, __FUNCTION__,__LINE__);
			goto funcExit;
		}
		OPL_Channel_GetLastUpdateDate(pChannels[0], &lastUpdateDate);
		if(lastUpdateDate > latestUpdateDate)
		{
			latestUpdateDate = lastUpdateDate;
		}
	}
	*retval = static_cast<OOIFNumber>(latestUpdateDate);
funcExit:

	if(pNetwork != NULL)
	{
		OPL_Channel_FreeNetworkIDList(pNetwork);
	}
	OPL_Channel_FreeChannelList((OPL_Channel_t*)pChannels);
	return OOIF_RETURN_OK;
}

#endif /* OIPF && JLABS */

#ifdef JLABS_JCOM

void chmask_listener(int result)
{
	g_onResultCreateMask(result);
}

OOIFReturnCode native_jlabsChannelConfigCreateChannelMask(OOIFBoolean *retval)
{
	HBOOL result;

	if (NULL == retval)
		return OOIF_RETURN_VALUE_UNDEFINED;

	if (!OplJlabsLocalSystem::doChannelMask(&result))
	{
		OOIF_LOG_ERROR("[%s:%d] doChannelMask ret Error!!!\n", __PRETTY_FUNCTION__, __LINE__);
		return OOIF_RETURN_ERROR;
	}

	//OOIF_LOG_PRINT("native_jlabsChannelConfigCreateChannelMask: result:%d\n", result);

	if(result == true)
	{
		OPL_Channel_SetResultCreateMaskListener(chmask_listener);
	}

	*retval = result;
	return OOIF_RETURN_OK;
}

OOIFReturnCode native_jlabsChannelConfigSetListener(resultCreateMaskListener listener)
{
	g_onResultCreateMask = listener;

	return OOIF_RETURN_OK;
}

#endif


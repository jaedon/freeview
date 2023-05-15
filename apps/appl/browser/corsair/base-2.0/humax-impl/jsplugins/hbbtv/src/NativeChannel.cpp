// -*- Mode: c++; tab-width: 4; indent-tabs-mode: t; c-basic-offset: 4 -*-
/*
 * Copyright (C) 1995-2013 Opera Software ASA. All rights reserved.
 *
 * It is prohibited for you to distribute this file to anybody without Operas
 * separate written consent.
 */

#include "NativeChannel.h"

#include "macros.h"
#include "oplchannel.h"
#include "oplbroadcast.h"
#ifdef USE_NEW_FAV
#include "oplfavouritehandle.h"
#else
#include "oplfavourite.h"
#endif
#include "opltuner.h"
#include <hlib.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define	__TRACE()					do{OOIF_LOG_DEBUG("[%s:%d] trace\n", __FUNCTION__, __LINE__);}while(0)
#define	__RETURN_ERROR_IF(cond)		do{if(cond){OOIF_LOG_ERROR("[%s:%d] error \'%s\'\n", __FUNCTION__, __LINE__, #cond);return OOIF_RETURN_ERROR;}}while(0)
#define	__RETURN_NULL_IF(cond)		do{if(cond){OOIF_LOG_WARNING("[%s:%d] null \'%s\'\n", __FUNCTION__, __LINE__, #cond);return OOIF_RETURN_VALUE_NULL;}}while(0)
#define	__RETURN_UNDEFINED_IF(cond)	do{if(cond){OOIF_LOG_WARNING("[%s:%d] undefined \'%s\'\n", __FUNCTION__, __LINE__, #cond);return OOIF_RETURN_VALUE_UNDEFINED;}}while(0)
#define	__RETURN_OK_IF(cond)		do{if(cond)return OOIF_RETURN_ERROR;}while(0)

#define	MAX_BUF_LEN 32
#ifdef OIPF
channelConfigListener *g_channelConfigListener=NULL;
#endif //OIPF

OPL_STATIC OPL_INLINE
ChannelType	__getChannelType (OPL_ChannelType_e channelType)
{
	switch (channelType)
	{
	case eOPL_ChannelType_TV:
		return CHANNEL_TYPE_TV;

	case eOPL_ChannelType_RADIO:
		return CHANNEL_TYPE_RADIO;

	case eOPL_ChannelType_OTHER:
	default:
		break;
	}
	return CHANNEL_TYPE_OTHER;
}

static const struct idtypemap {
	OPL_ChannelID_e	opl;
	OOIFNumber		oipf;
} s_idtypemap[] = {
      {eOPL_ChannelID_DVB_S, CHANNEL_ID_DVB_S}
    , {eOPL_ChannelID_DVB_S2, CHANNEL_ID_DVB_S2}
	, {eOPL_ChannelID_DVB_C, CHANNEL_ID_DVB_C}
	, {eOPL_ChannelID_DVB_C2, CHANNEL_ID_DVB_C2}
    , {eOPL_ChannelID_DVB_T, CHANNEL_ID_DVB_T}
	, {eOPL_ChannelID_DVB_T2, CHANNEL_ID_DVB_T2}
    , {eOPL_ChannelID_ANALOG, CHANNEL_ID_ANALOG}
    , {eOPL_ChannelID_DVB_SI_DIRECT, CHANNEL_ID_DVB_SI_DIRECT}
	, {eOPL_ChannelID_ISDB_C, CHANNEL_ID_ISDB_C}
	, {eOPL_ChannelID_ISDB_S, CHANNEL_ID_ISDB_S}
	, {eOPL_ChannelID_ISDB_T, CHANNEL_ID_ISDB_T}
	, {eOPL_ChannelID_ATSC_T, CHANNEL_ID_ATSC_T}
	, {eOPL_ChannelID_IPTV_SDS, CHANNEL_ID_IPTV_SDS}
	, {eOPL_ChannelID_IPTV_URI, CHANNEL_ID_IPTV_URI}
};

OPL_STATIC OPL_INLINE
OOIFNumber	__toOipfIdType (OPL_ChannelID_e idType)
{
	int	i, n = sizeof(s_idtypemap) / sizeof(s_idtypemap[0]);
	for (i = 0 ; i < n ; i++)
	{
		if (s_idtypemap[i].opl == idType)
			return s_idtypemap[i].oipf;
	}
	OOIF_LOG_WARNING("%s(%d) Unknown idtype\n", __FUNCTION__, (int)idType);
	return 0;
}

OPL_STATIC OPL_INLINE
OPL_ChannelID_e	__toOplIdType (OOIFNumber idType)
{
	int	i, n = sizeof(s_idtypemap) / sizeof(s_idtypemap[0]);
	for (i = 0 ; i < n ; i++)
	{
		if (s_idtypemap[i].oipf == idType)
			return s_idtypemap[i].opl;
	}
	OOIF_LOG_WARNING("%s(%d) Unknown idtype\n", __FUNCTION__, (int)idType);
	return (OPL_ChannelID_e)0;
}


OPL_STATIC OPL_INLINE
char *		__strDup (const char *str)
{
	if (str == NULL)
		return NULL;
	return HLIB_STD_StrDup(str);
}


OOIFReturnCode native_ChannelInitialize(ChannelHandle identifier, ChannelData *data)
{
	char	buf[128];

	__TRACE();

	OPL_Channel_t	channel = (OPL_Channel_t)identifier;

	__RETURN_ERROR_IF(channel == NULL);

	data->channelType = __getChannelType(OPL_Channel_GetChannelType(channel));
#ifdef OIPF
	data->idType      = __toOipfIdType(OPL_Channel_GetIdType(channel));
#endif
	data->ccid        = __strDup(OPL_Channel_GetCCID(channel, buf, 128));
#ifdef OIPF
	data->tunerID     = __strDup(OPL_Channel_GetTunerID(channel, buf, 128));
#endif
	data->onid = OPL_Channel_GetOrgNetworkID(channel);
	data->tsid = OPL_Channel_GetTransponderID(channel);
	data->sid  = OPL_Channel_GetServiceID(channel);
	data->has_onid    = (data->onid) ? TRUE : FALSE;
#ifdef OIPF
	data->sourceID = OPL_Channel_GetSourceID(channel);
	data->freq = OPL_Channel_GetFrequency(channel);
	data->cni  = OPL_Channel_GetCNI(channel);
#endif

	data->name = __strDup(OPL_Channel_GetName(channel));
#ifdef HBBTV_1_2_1
	if ( data->name != NULL )
		data->has_name = true;
#endif
#ifdef OIPF
	data->majorChannel = static_cast<OOIFNumber>(OPL_Channel_GetMajorChannel(channel));
	data->minorChannel = static_cast<OOIFNumber>(OPL_Channel_GetMinorChannel(channel));
#endif
	data->dsd_len   = OPL_Channel_GetDeliverySystemDescriptor(channel, &data->dsd);

#ifdef OIPF
	data->locked      = OPL_Channel_IsLocked(channel) ? TRUE : FALSE;
	data->manualBlock = OPL_Channel_IsManualBlocked(channel) ? TRUE : FALSE;
	data->ipBroadcastID = __strDup(OPL_Channel_GetIPBroadcastID(channel, buf, 128));
	data->channelMaxBitRate = static_cast<OOIFNumber>(OPL_Channel_GetChannelMaxBitRate(channel));
	data->channelTTR        = static_cast<OOIFNumber>(OPL_Channel_GetChannelTTR(channel));
	data->longName    = __strDup(OPL_Channel_GetLongName(channel));
	data->description = __strDup(OPL_Channel_GetDescription(channel));

	data->authorised  = OPL_Channel_IsAuthorised(channel) ? TRUE : FALSE;
	data->hidden      = OPL_Channel_IsHidden(channel) ? TRUE : FALSE;
	data->logoURL     = NULL; //(char *)OPL_Channel_GetLogoURL(channel);

#endif
	return OOIF_RETURN_OK;
}

OOIFReturnCode native_ChannelReleaseHandle(ChannelHandle id)
{
	__TRACE();

	//OOIF_LOG_DEBUG("[%s] uid:%d\n", __FUNCTION__, (int)id);
	OPL_Channel_Delete((OPL_Channel_t)id);

	return OOIF_RETURN_OK;
}

#ifdef OIPF
OOIFReturnCode native_ChannelGetFavIDs(ChannelHandle identifier, int *count, char ***favids)
{
	OPL_Channel_t	channel = (OPL_Channel_t)identifier;

	__TRACE();
	__RETURN_ERROR_IF(channel == NULL);

	*count = OPL_Channel_GetFavouriteIDs(channel, favids);
	if (*count == 0)
	{
		*favids = (char **)HLIB_STD_StrDup("will be freed");
	}
	return OOIF_RETURN_OK;
}

OOIFReturnCode native_ChannelGetGenre(ChannelHandle identifier, int *count, char ***genres)
{
	OPL_Channel_t	channel = (OPL_Channel_t)identifier;

	__TRACE();
	__RETURN_ERROR_IF(channel == NULL);

	*count = OPL_Channel_GetGenre(channel, genres);
	if (*count == 0)
	{
		*genres = (char **)HLIB_STD_StrDup("will be freed");
	}
	return OOIF_RETURN_OK;
}

OOIFReturnCode native_ChannelGetFavourite(ChannelHandle identifier, OOIFBoolean *pbFavourite)
{
	OPL_Channel_t	channel = (OPL_Channel_t)identifier;

	__TRACE();
	__RETURN_ERROR_IF(channel == NULL);
	 *pbFavourite = OPL_Channel_GetFavouriteIDs(channel, NULL) ? TRUE : FALSE;
	 return OOIF_RETURN_OK;
}

OOIFReturnCode native_ChannelGetOnID(ChannelHandle identifier, int *pOnId)
{
	OPL_Channel_t	channel = (OPL_Channel_t)identifier;

	__TRACE();
	__RETURN_ERROR_IF(channel == NULL);

	*pOnId = OPL_Channel_GetOrgNetworkID(channel);
	return OOIF_RETURN_OK;
}

OOIFReturnCode native_ChannelGetField(ChannelHandle identifier, const char *field, const char **retval)
{
	#define	__POOL_SIZE	3
	static char *s_pool[__POOL_SIZE];
	static int	s_poolIndex;

	OPL_Channel_t	channel = (OPL_Channel_t)identifier;

	__TRACE();
	__RETURN_ERROR_IF(channel == NULL || field == NULL);

	char *value = OPL_Channel_GetField(channel, field);
	if ( value == NULL )
	{
		return OOIF_RETURN_VALUE_NULL;
	}

	__RETURN_NULL_IF(value == NULL);

	if (s_pool[s_poolIndex])
		HLIB_STD_MemFree(s_pool[s_poolIndex]);
	s_pool[s_poolIndex] = value;
	s_poolIndex = (s_poolIndex + 1) % __POOL_SIZE;

	*retval = (const char *)value;
	return OOIF_RETURN_OK;
}

OOIFReturnCode native_ChannelGetLogo(ChannelHandle identifier, OOIFNumber width, OOIFNumber height, const char **retval)
{
	OPL_Channel_t	channel = (OPL_Channel_t)identifier;

	__TRACE();
	__RETURN_ERROR_IF(channel == NULL);

	*retval = OPL_Channel_GetLogo(channel, width, height);
	if ( *retval == NULL )
	{
		return OOIF_RETURN_VALUE_NULL;
	}

	return OOIF_RETURN_OK;
}

OOIFReturnCode native_ChannelGetByCCID(const char *ccid, ChannelHandle *retval)
{
	OPL_ChannelManager_t	manager;

	__TRACE();

	manager = OPL_Channel_GetManager();
	__RETURN_NULL_IF(!manager);

	OPL_Channel_t channel = OPL_Channel_FindChannel(manager, ccid);
	__RETURN_NULL_IF(!channel);

	*retval = (ChannelHandle)OPL_Channel_Clone(channel);
	if ( *retval == NULL )
	{
		return OOIF_RETURN_VALUE_NULL;
	}

	return OOIF_RETURN_OK;
}
#endif // OIPF

OOIFReturnCode native_ChannelConfigIsChannelListManaged(OOIFBoolean *retval)
{
	OPL_ChannelManager_t	manager;

	__TRACE();

	manager = OPL_Channel_GetManager();
	if (manager)
	{
		*retval = TRUE;
	}
	else
	{
		*retval = FALSE;
	}
	return OOIF_RETURN_OK;
}

// Populate list with all channels
OOIFReturnCode native_ChannelConfigGetChannelList(int *count, ChannelHandle **handles)
{
	OPL_ChannelManager_t	manager;

	__TRACE();

	manager = OPL_Channel_GetManager();
	__RETURN_ERROR_IF(!manager);

	*count = OPL_Channel_GetChannelList(manager, (OPL_Channel_t **)handles);
	/*	// 얘는 또... 그냥 count만 0면 위에서 알아서 하네..  구려
	if (*count == 0)
	{
		*handles = (ChannelHandle *)HLIB_STD_MemAlloc(strlen("will be freed"));
	}
	*/
	return OOIF_RETURN_OK;
}

#ifdef OIPF
static void		__ChannelListener (OPL_ChannelManager_t manager, OPL_ChannelState_e state, OPL_Collection_t *updated)
{
	(void)manager;

	if (g_channelConfigListener)
	{
		switch (state)
		{
		case eOPL_CHANNEL_STATE_UPDATED:
            g_channelConfigListener(CHANNEL_CONFIG_EVENT_CHANNEL_LIST_UPDATE, NULL);
			break;

		default:
			break;
		}
	}
}

OOIFReturnCode native_ChannelConfigSetListener(channelConfigListener listener)
{
	OOIF_LOG_DEBUG("ChannelConfig: Setting Channel Config Listener");
	g_channelConfigListener = listener;

	return OOIF_RETURN_OK;
}

OOIFReturnCode native_ChannelConfigIsValidBDR(const char* bdr, OOIFBoolean* retval)
{
	// TODO: not implemented
	*retval = FALSE;
	return OOIF_RETURN_NOT_SUPPORTED;
}

OOIFReturnCode native_ChannelCreateChannelList(const char* bdr, int *count, ChannelHandle **handles)
{
	// TODO: not implemnted
	*count = 0;
	*handles = NULL;
	return OOIF_RETURN_NOT_SUPPORTED;
}

OOIFReturnCode native_ChannelConfigCreateChannelObject(OOIFNumber idType, OOIFBoolean has_onid, OOIFNumber onid, OOIFBoolean has_tsid, OOIFNumber tsid, OOIFBoolean has_sid, OOIFNumber sid, OOIFBoolean has_sourceID, OOIFNumber sourceID, const char *ipBroadcastID, ChannelHandle *retval)
{
	__RETURN_NULL_IF(!OPL_Channel_IsSupportedChannelID(__toOplIdType(idType)));

	__RETURN_NULL_IF(!has_onid && !has_tsid && !has_sid);

	/**
	 * 왜 이렇게 채널을 찾아서 생성하지 않고, 그냥 create하는지는...
	 * 현재 OCTO가 채널 없이 triplet만으로 튜닝할 수 없어서
	 * 그냥 빈 채널을 주는 것이 무의미 함.
	 * createChannelObject를 하는 이유는 난중에 setChannel()을 하기 위해서 인데,
	 * 어짜피 튜닝못하는거, 그냥 채널이 없는 경우에는 null 리턴해서 채널 못만든다고 하면 될듯
	 */
	OPL_Channel_t channel = OPL_Channel_FindChannelByTriplet(OPL_Channel_GetManager(), (int)onid, (int)tsid, (int)sid);
	__RETURN_NULL_IF(!channel);

	*retval = (ChannelHandle)OPL_Channel_Clone(channel);
	__RETURN_NULL_IF((*retval == (ChannelHandle)0));

	(void)sourceID;
	(void)ipBroadcastID;

	return OOIF_RETURN_OK;
}

OOIFReturnCode native_ChannelConfigIsFavouriteListManaged(OOIFBoolean *retval)
{
	__TRACE();

	*retval = TRUE;

	return OOIF_RETURN_OK;
}

OOIFReturnCode native_ChannelConfigGetFavouriteLists(int *count, FavouriteListHandle **handles)
{
	__TRACE();
#ifdef USE_NEW_FAV
    std::list<OplFavouriteListHandle*> favouriteHandles = OplFavouriteManager::createHandles();
    int size = favouriteHandles.size();
    if(size > 0)
    {
        *handles = (FavouriteListHandle*)HLIB_STD_MemAlloc(sizeof(FavouriteListHandle) * size);
        int index = 0;
        std::list<OplFavouriteListHandle*>::iterator i(favouriteHandles.begin());
        while (i != favouriteHandles.end()) {
            OplFavouriteListHandle* h = (*i++);
            (*handles)[index] = (FavouriteListHandle*)h;
            index++;
        }
    }
    *count = size;

    if (*count == 0)
    {
        *handles = (FavouriteListHandle *)HLIB_STD_StrDup("will be freed");
    }

    return OOIF_RETURN_OK;
#else
	*count = (int)OPL_Favourite_GetCollection((OPL_Favourite_t **)handles);
	if (*count == 0)
	{
		*handles = (FavouriteListHandle *)HLIB_STD_StrDup("will be freed");
	}
	return OOIF_RETURN_OK;
#endif
}

OOIFReturnCode native_ChannelConfigGetCurrentFavouriteList(FavouriteListHandle *retval)
{
	__TRACE();
#ifdef USE_NEW_FAV
    return OOIF_RETURN_VALUE_UNDEFINED;
#else
	*retval = (FavouriteListHandle)OPL_Favourite_GetCurrent();
	if (*retval)
	{
	return OOIF_RETURN_OK;
}
    return OOIF_RETURN_VALUE_UNDEFINED;
#endif
}

OOIFReturnCode native_ChannelConfigGetCurrentChannel(ChannelHandle *retval)
{
	/*
	The current channel of the OITF if the user has given permission to share this information, possibly
	through a mechanism outside the scope of this specification. If no channel is being presented, or if this
	information is not visible to the caller, the value of this property SHALL be null.
	In an OITF where exactly one video/broadcast object is in any state other than Unrealized and the
	channel being presented by that video/broadcast object is the only broadcast channel being presented
	by the OITF then changes to the channel presented by that video/broadcast object SHALL result in
	changes to the current channel of the OITF.
	In an OITF which is presenting more than one broadcast channel at the same time, the current channel
	of the OITF is the channel whose audio is being presented (as defined in the
	bindToCurrentChannel() method). If that current channel is under the control of a DAE application
	via a video/broadcast object then changes to the channel presented by that video/broadcast object
	*/
#if defined(USE_VIDEOHANDLE)
    OPL_Channel_t channel = NULL;
    if(OPL_Channel_GetCurrentChannel(&channel) == OPL_TRUE)
    {
        if ( !channel )
        {
            return OOIF_RETURN_VALUE_NULL;
        }
        *retval = (ChannelHandle)OPL_Channel_Clone(channel);
        if ( *retval == NULL )
        	return OOIF_RETURN_ERROR;
        return OOIF_RETURN_OK;
    }
#endif
    return OOIF_RETURN_VALUE_NULL;
}

OOIFReturnCode native_ChannelConfigStartScan(const ScanOptions* options, ScanParametersType type, const ScanParameters* parameters, OOIFNumber* retval)
{
	OOIF_LOG_DEBUG("ChannelConfig: Start Searching for channels");
	OOIF_LOG_DEBUG("==Channel Scan Options==)");
	OOIF_LOG_DEBUG("channelType=%d", (int)options->channelType);
	OOIF_LOG_DEBUG("replaceExisting=%d", (int)options->replaceExisting);
	switch (type)
	{
	case SCAN_PARAMETERS_DVBT:
	{
		OOIF_LOG_DEBUG("==Channel Scan Parameters==)");
		OOIF_LOG_DEBUG("startFrequency=%d", (int)parameters->scan_parameters_dvbt.startFrequency);
		OOIF_LOG_DEBUG("endFrequency=%d", (int)parameters->scan_parameters_dvbt.endFrequency);
		OOIF_LOG_DEBUG("raster=%d", (int)parameters->scan_parameters_dvbt.raster);
		OOIF_LOG_DEBUG("ofdm=%s", parameters->scan_parameters_dvbt.ofdm);
		OOIF_LOG_DEBUG("modulationModes=%d", (int)parameters->scan_parameters_dvbt.modulationModes);
		OOIF_LOG_DEBUG("bandwidth=%s", parameters->scan_parameters_dvbt.bandwidth);

		ChannelConfigEventInfo info;
		info.channel_scan_data.type = (OOIFNumber)4; //a channel scan has completed
		info.channel_scan_data.progress = (OOIFNumber)100; //100%
		info.channel_scan_data.frequency = parameters->scan_parameters_dvbt.endFrequency; //10kHz
		info.channel_scan_data.signalStrength = (OOIFNumber)50; //valid values between 0 and 100
		info.channel_scan_data.channelNumber = (OOIFNumber)10; //logical channel number
		info.channel_scan_data.channelType = (OOIFNumber)0; //0 == TYPE_TV
		info.channel_scan_data.channelCount = (OOIFNumber)1; //total number of Channels found so far
		info.channel_scan_data.transponderCount = (OOIFNumber)1; //one transponder found

		g_channelConfigListener(CHANNEL_CONFIG_EVENT_CHANNEL_LIST_UPDATE, NULL);
		g_channelConfigListener(CHANNEL_CONFIG_EVENT_CHANNEL_SCAN, &info);

		*retval = info.channel_scan_data.channelCount;
		break;
	}
	case SCAN_PARAMETERS_DVBS:
	{
		OOIF_LOG_DEBUG("==Channel Scan Parameters==)");
		OOIF_LOG_DEBUG("startFrequency=%d", (int)parameters->scan_parameters_dvbs.startFrequency);
		OOIF_LOG_DEBUG("endFrequency=%d", (int)parameters->scan_parameters_dvbs.endFrequency);
		OOIF_LOG_DEBUG("polarisation=%d", (int)parameters->scan_parameters_dvbs.polarisation);
		OOIF_LOG_DEBUG("codeRate=%s", parameters->scan_parameters_dvbs.codeRate);
		OOIF_LOG_DEBUG("modulationModes=%d", (int)parameters->scan_parameters_dvbs.modulationModes);
		OOIF_LOG_DEBUG("symbolRate=%s", parameters->scan_parameters_dvbs.symbolRate);
		OOIF_LOG_DEBUG("orbitalPosition=%d", (int)parameters->scan_parameters_dvbs.orbitalPosition);
		OOIF_LOG_DEBUG("networkId=%d", (int)parameters->scan_parameters_dvbs.networkId);

		ChannelConfigEventInfo info;
		info.channel_scan_data.type = (OOIFNumber)4; //a channel scan has completed
		info.channel_scan_data.progress = (OOIFNumber)100; //100%
		info.channel_scan_data.frequency = parameters->scan_parameters_dvbs.endFrequency; //10kHz
		info.channel_scan_data.signalStrength = (OOIFNumber)50; //valid values between 0 and 100
		info.channel_scan_data.channelNumber = (OOIFNumber)11; //logical channel number
		info.channel_scan_data.channelType = (OOIFNumber)0; //0 == TYPE_TV
		info.channel_scan_data.channelCount = (OOIFNumber)1; //total number of Channels found so far
		info.channel_scan_data.transponderCount = (OOIFNumber)1; //one transponder found

		g_channelConfigListener(CHANNEL_CONFIG_EVENT_CHANNEL_LIST_UPDATE, NULL);
		g_channelConfigListener(CHANNEL_CONFIG_EVENT_CHANNEL_SCAN, &info);

		*retval = info.channel_scan_data.channelCount;
		break;
	}
	case SCAN_PARAMETERS_DVBC:
	{
		OOIF_LOG_DEBUG("==Channel Scan Parameters==)");
		OOIF_LOG_DEBUG("startFrequency=%d", (int)parameters->scan_parameters_dvbc.startFrequency);
		OOIF_LOG_DEBUG("endFrequency=%d", (int)parameters->scan_parameters_dvbc.endFrequency);
		OOIF_LOG_DEBUG("raster=%d", (int)parameters->scan_parameters_dvbc.raster);
		OOIF_LOG_DEBUG("symbolRate=%s", parameters->scan_parameters_dvbc.symbolRate);
		OOIF_LOG_DEBUG("modulationModes=%d", (int)parameters->scan_parameters_dvbc.modulationModes);
                OOIF_LOG_DEBUG("startNoetworkScanOnNIT=%d", (int)parameters->scan_parameters_dvbc.startNetworkScanOnNIT);
		OOIF_LOG_DEBUG("networkId=%d", (int)parameters->scan_parameters_dvbc.networkId);

		ChannelConfigEventInfo info;
		info.channel_scan_data.type = (OOIFNumber)4; //a channel scan has completed
		info.channel_scan_data.progress = (OOIFNumber)100; //100%
		info.channel_scan_data.frequency = parameters->scan_parameters_dvbc.endFrequency; //10kHz
		info.channel_scan_data.signalStrength =  (OOIFNumber)50; //valid values between 0 and 100
		info.channel_scan_data.channelNumber =  (OOIFNumber)10; //logical channel number
		info.channel_scan_data.channelType =  (OOIFNumber)0; //0 == TYPE_TV
		info.channel_scan_data.channelCount =  (OOIFNumber)1; //total number of Channels found so far
		info.channel_scan_data.transponderCount =  (OOIFNumber)1; //one transponder found

		g_channelConfigListener(CHANNEL_CONFIG_EVENT_CHANNEL_LIST_UPDATE, NULL);
		g_channelConfigListener(CHANNEL_CONFIG_EVENT_CHANNEL_SCAN, &info);

		*retval = info.channel_scan_data.channelCount;
		break;
	}
	}
	return OOIF_RETURN_OK;
}

OOIFReturnCode native_ChannelConfigStopScan()
{
	OOIF_LOG_DEBUG("ChannelConfig: Stop Scanning");
	ChannelConfigEventInfo info;
	info.channel_scan_data.type = (OOIFNumber)5; // a channel scan has been aborted
	g_channelConfigListener(CHANNEL_CONFIG_EVENT_CHANNEL_SCAN, &info);
	return OOIF_RETURN_OK;
}

// Populate list with channels with recording capability
OOIFReturnCode native_FavouriteListGetChannels(FavouriteListHandle identifier, int *count, ChannelHandle **handles)
{
	__TRACE();
#ifdef USE_NEW_FAV
    OplFavouriteListHandle* handle = static_cast<OplFavouriteListHandle*>(identifier);
    std::list<OPL_Channel_t> channelList = handle->createChannelList();
    int size = channelList.size();
    if(size > 0)
    {
        *handles = (ChannelHandle*)HLIB_STD_MemAlloc(sizeof(ChannelHandle) * size);
        int index = 0;
        std::list<OPL_Channel_t>::const_iterator i(channelList.begin());
        while(i != channelList.end()) {
            OPL_Channel_t c = (*i++);
            (*handles)[index] = (ChannelHandle*)c;
            index++;
        }
    }
    *count = size;
    if (*count == 0)
    {
        *handles = (ChannelHandle *)HLIB_STD_StrDup("will be freed");
    }

    return OOIF_RETURN_OK;
#else
	*count = (int)OPL_Favourite_GetChannelList((OPL_Favourite_t)identifier, (OPL_Channel_t **)handles);
	if (*count == 0)
	{
		*handles = (ChannelHandle *)HLIB_STD_StrDup("will be freed");
	}
	return OOIF_RETURN_OK;
#endif
}

OOIFReturnCode native_FavouriteListGetChannel(FavouriteListHandle identifier, const char* channelID, ChannelHandle* retval)
{
	// TODO: What we have to do!!!!????
	*retval = (void*)1;
	return OOIF_RETURN_OK;
}

OOIFReturnCode native_FavouriteListGetChannelByTriplet(FavouriteListHandle identifier, OOIFNumber onid, OOIFBoolean has_tsid, OOIFNumber tsid, OOIFNumber sid, ChannelHandle* retval)
{
	*retval = (void*)1;
	return OOIF_RETURN_OK;
}

OOIFReturnCode native_FavouriteListGetChannelBySourceID(FavouriteListHandle identifier, OOIFNumber sourceID, ChannelHandle* retval)
{
	*retval = (void*)1;
	return OOIF_RETURN_OK;
}

OOIFReturnCode native_FavouriteListGetFavID(FavouriteListHandle identifier, const char **retval)
{
	__TRACE();
#ifdef USE_NEW_FAV
    OplFavouriteListHandle *handle = static_cast<OplFavouriteListHandle*>(identifier);
	static char ret[30];
	// const char can not be freed..
    //char* ret = (char*)HLIB_STD_MemAlloc(sizeof(char*) * 4);
    sprintf(ret, "%d", (int)handle->getFavID());
    *retval = ret;
    return OOIF_RETURN_OK;
#else
	*retval = OPL_Favourite_GetID((OPL_Favourite_t)identifier);
	__RETURN_NULL_IF(*retval == NULL);

	return OOIF_RETURN_OK;
#endif
}

OOIFReturnCode native_FavouriteListGetName(FavouriteListHandle identifier, const char **retval)
{
	__TRACE();
#ifdef USE_NEW_FAV
    OplFavouriteListHandle *handle = static_cast<OplFavouriteListHandle*>(identifier);
    *retval = handle->getName();
    return ( *retval == NULL ) ? OOIF_RETURN_VALUE_NULL : OOIF_RETURN_OK;
#else
	*retval = OPL_Favourite_GetName((OPL_Favourite_t)identifier);
	__RETURN_NULL_IF(*retval == NULL);

	return OOIF_RETURN_OK;
#endif
}

OOIFReturnCode native_FavouriteListCommit(FavouriteListHandle identifier, char **ccids, int channelCount)
{
    __TRACE();
#ifdef USE_NEW_FAV
    OplFavouriteListHandle *handle = static_cast<OplFavouriteListHandle*>(identifier);
    std::list<const char*> aCcids;
    int i;
    for(i=0; i<channelCount; i++)
    {
        aCcids.push_back(ccids[i]);
    }
    handle->save(aCcids);
    return OOIF_RETURN_OK;
#else
    OPL_BOOL ret = OPL_Favourite_Commit(
                      (OPL_Favourite_t)identifier
                    , (const char **)ccids
                    , (size_t)channelCount
                );
    (void)ret;
    // TODO: ret 값 전달, 지금은 그냥 무조건 TRUE 리턴함 .. 오페라도 이런건 대책 없음~
    return OOIF_RETURN_OK;
#endif
}

OOIFReturnCode native_FavouriteListCreate(FavouriteListHandle *retval)
{
	__TRACE();
#ifdef USE_NEW_FAV
    OplFavouriteListHandle* favouriteHandle = OplFavouriteManager::createFavouriteList(name);
    *retval = (FavouriteListHandle*)favouriteHandle;
    return OOIF_RETURN_OK;
#else
    OPL_Favourite_t	hFav = OPL_Favourite_New(name, extension, NULL);
    __RETURN_NULL_IF(!hFav);

    *retval = (FavouriteListHandle)hFav;
    return OOIF_RETURN_OK;
#endif
}

OOIFReturnCode native_FavouriteListReleaseHandle(FavouriteListHandle id)
{
	__TRACE();

#ifdef USE_NEW_FAV
	STEP;
    OplFavouriteListHandle* handle = static_cast<OplFavouriteListHandle*>(id);
	delete handle;
#else
	// do nothing! (FavouriteListHandle은 그냥 HINT32 id)
#endif
	return OOIF_RETURN_OK;
}
#endif // OIPF

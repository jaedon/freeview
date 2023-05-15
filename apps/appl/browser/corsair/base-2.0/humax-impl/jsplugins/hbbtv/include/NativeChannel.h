/* -*- Mode: c; tab-width: 4; indent-tabs-mode: t; c-basic-offset: 4 -*- */
/*
 * Copyright (C) 1995-2013 Opera Software ASA. All rights reserved.
 *
 * It is prohibited for you to distribute this file to anybody without Operas
 * separate written consent.
 */

#ifndef __NATIVE_CHANNEL
#define __NATIVE_CHANNEL

#include "NativeOOIFTypes.h"

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

/**
  \defgroup Channel Scheduled Content
  @{

   This is the native interface for the The Channel related classes
   as defined in DAE1.1 7.13.9-14 / DAE2.1 7.13.10-19.
 */

/**
   Opaque reference to a Channel in the native platform.
 */
typedef void* ChannelHandle;

#ifdef OIPF
/**
   Opaque reference to a FavouriteList in the native platform.
   Note that a FavouriteListHandle is not (automatically) the same as
   the favourite list id (favID). But nothing prevents it from
   actually being a char* disguised as a void*
 */
typedef void* FavouriteListHandle;
#endif /* OIPF */

/*------------------------------------------
  Channel
  ------------------------------------------*/
/** @name Channel
	The Channel class as defined in DAE1.1 7.13.11 / DAE2.1 7.13.12.
	@{*/

/**
  The constants defined in DAE1.1 7.13.12.1 / DAE2.1 7.13.12.1
 */
typedef enum {
	CHANNEL_TYPE_TV = 0,
	CHANNEL_TYPE_RADIO = 1,
	CHANNEL_TYPE_OTHER = 2,
	CHANNEL_ID_ANALOG = 0,
	CHANNEL_ID_DVB_C = 10,
	CHANNEL_ID_DVB_S = 11,
	CHANNEL_ID_DVB_T = 12,
	CHANNEL_ID_DVB_SI_DIRECT = 13,
	CHANNEL_ID_DVB_C2 = 14,
	CHANNEL_ID_DVB_S2 = 15,
	CHANNEL_ID_DVB_T2 = 16,
	CHANNEL_ID_ISDB_C = 20,
	CHANNEL_ID_ISDB_S = 21,
	CHANNEL_ID_ISDB_T = 22,
	CHANNEL_ID_ATSC_T = 30,
	CHANNEL_ID_IPTV_SDS = 40,
	CHANNEL_ID_IPTV_URI = 41
} ChannelType;

/**
   This data struct holds the static properties of a Channel as
   defined in DAE1.1 7.13.11.2 / DAE2.1 7.13.12.2. The OOIF plugin
   will take over ownership of all strings in the data struct, strdup()
   or malloc should be used in the native code to allocate memory for them.
*/
typedef struct {
	ChannelType channelType;
#ifdef OIPF
	OOIFNumber idType;
#endif /* OIPF */
	char *ccid;
#ifdef OIPF
	char *tunerID;
#endif /* OIPF */
	OOIFBoolean has_onid;
	OOIFNumber onid;
	OOIFNumber tsid;
	OOIFNumber sid;
#ifdef OIPF
	OOIFNumber sourceID;
	OOIFNumber freq;
	OOIFNumber cni;
#endif /* OIPF */
#ifdef HBBTV_1_2_1
	OOIFBoolean has_nid;
	OOIFNumber nid;
	OOIFBoolean has_name;
#endif /* HBBTV_1_2_1 */
	char *name;
#ifdef OIPF
	OOIFNumber majorChannel;
	OOIFNumber minorChannel;
#endif /* OIPF */
	unsigned char *dsd;		/**< The raw dsd data (null termination not required) */
	int dsd_len;			/**< The length of the raw dsd data, in bytes */
#ifdef OIPF
	OOIFBoolean favourite;
	OOIFBoolean locked;
	OOIFBoolean manualBlock;
	char *ipBroadcastID;
	OOIFNumber channelMaxBitRate;
	OOIFNumber channelTTR;
	char *longName;
	char *description;
	OOIFBoolean authorised;
	OOIFBoolean hidden;
	char *logoURL;
#endif /* OIPF */
} ChannelData;

/**
   Called when the Channel instance is destroyed in the OOIF plugin.
   \param id Channel instance identifier
   \return OOIF_RETURN_OK
 */
OOIFReturnCode native_ChannelReleaseHandle(ChannelHandle id);

/**
   Populate the ChannelData struct with the value of the Channel properties
   defined in DAE1.1 7.13.11.3.1 / DAE2.1 7.13.12.2. The OOIF plugin will handle all getter calls on
   these parameter.
   \param identifier The instance identifier of the Channel to get properties of
   \param data property struct to populate
   \return OOIF_RETURN_OK
*/
OOIFReturnCode native_ChannelInitialize(ChannelHandle identifier, ChannelData *data);

#ifdef OIPF
/**
   Gets OIPF property favIDs as defined in DAE 7.13.12.2
   \param identifier The instance identifier of the Channel
   \param count Number of elements in the collection
   \param favids Use this to return the array.
   The OOIF framework takes ownership of the returned array
   and the strings it contains, and will free the memory when appropriate.
   malloc(), strdup(), or similar should be used in the native code to allocate
   memory for them.
   \return OOIF_RETURN_OK
*/
OOIFReturnCode native_ChannelGetFavIDs(ChannelHandle identifier, int *count, char ***favids);

/**
   Gets OIPF property genre as defined in DAE 7.13.12.3.1
   \param identifier The instance identifier of the Channel
   \param count Number of elements in the collection
   \param genres Use this to return the array.
   The OOIF framework takes ownership of the returned array
   and the strings it contains, and will free the memory when appropriate.
   malloc(), strdup(), or similar should be used in the native code to allocate
   memory for them.
   \return OOIF_RETURN_OK
*/
OOIFReturnCode native_ChannelGetGenre(ChannelHandle identifier, int *count, char ***genres);

/**
   This function corresponds to the OIPF function getField as defined in DAE 7.13.12.3.2.
   \param identifier The instance identifier of the Channel
   \param field The name of the field whose value SHALL be retrieved.
   \param retval field referred
   \return OOIF_RETURN_OK or OOIF_RETURN_VALUE_UNDEFINED
*/
OOIFReturnCode native_ChannelGetField(ChannelHandle identifier, const char *field, const char **retval);

/**
   This function corresponds to the OIPF function getLogo as defined in DAE 7.13.12.3.2.
   \param identifier The instance identifier of the Channel
   \param The desired width of the image
   \param The desired height of the image
   \param retval the desired URI
   \return OOIF_RETURN_OK or OOIF_RETURN_VALUE_NULL
*/
OOIFReturnCode native_ChannelGetLogo(ChannelHandle identifier, OOIFNumber width, OOIFNumber height, const char **retval);

/**
   Gets a channel handle from a CCID.
   \param ccid The CCID of the channel to return
   \param retval Return the channel handle here
   \return OOIF_RETURN_OK
*/
OOIFReturnCode native_ChannelGetByCCID(const char *ccid, ChannelHandle *retval);
#endif /* OIPF */

/*@}*/
/*------------------------------------------
  Channel Config
  ------------------------------------------*/
/** @name ChannelConfig
	The ChannelConfig class as defined in DAE1.1 7.13.9 / DAE2.1 7.13.10.
	@{*/

#ifdef OIPF

/**
	ChannelConfig event types
*/
typedef enum
{
	/** This event corresponds to the onChannelScan event defined in 7.13.10.3 */
	CHANNEL_CONFIG_EVENT_CHANNEL_SCAN = 0,
	/** This event corresponds to the onChannelListUpdate event defined in DAE 7.13.10.3 */
	CHANNEL_CONFIG_EVENT_CHANNEL_LIST_UPDATE = 1
} ChannelConfigEvent;

/**
	Union used to pass event context info in an event callback. What
	attribute to use is decided by the event type defined by the
	#ChannelConfigEvent
*/
typedef union
{
	/** used by CHANNEL_CONFIG_EVENT_CHANNEL_SCAN as described in DAE 7.13.10.3 */
	struct {
		OOIFNumber type;
		OOIFNumber progress;
		OOIFNumber frequency;
		OOIFNumber signalStrength;
		OOIFNumber channelNumber;
		OOIFNumber channelType;
		OOIFNumber channelCount;
		OOIFNumber transponderCount;
	} channel_scan_data;
} ChannelConfigEventInfo;

/**
   The callback function that the native platform should call to
   dispatch a ChannelConfig related event. The OOIF plugin will call the
   registered listeners on the ChannelConfig instance.
   \param event The event type
   \param info Event context info, or NULL if no context info exists
   @see native_ChannelConfigSetListener
*/
typedef void (channelConfigListener)(ChannelConfigEvent event, ChannelConfigEventInfo *info);

/**
   Registers new listener on ChannelConfig related events.
   \param listener The listener
   \return OOIF_RETURN_OK
*/
OOIFReturnCode native_ChannelConfigSetListener(channelConfigListener listener);

/**
   Enumeration which points to channel scan parameters
   specific to certain types of broadcast network.
*/
typedef enum
{
	SCAN_PARAMETERS_DVBT = 0,
	SCAN_PARAMETERS_DVBS = 1,
	SCAN_PARAMETERS_DVBC = 2
} ScanParametersType;

/**
   This union holds the static properties of a ChannelScanParameters as
   defined in DAE 7.13.17.
*/
typedef union
{
	/** used by SCAN_PARAMETERS_DVBT, see DAE 7.13.18 */
	struct
	{
		OOIFNumber startFrequency;
		OOIFNumber endFrequency;
		OOIFNumber raster;
		const char* ofdm;
		OOIFNumber modulationModes;
		const char* bandwidth;
	} scan_parameters_dvbt;
	/** used by SCAN_PARAMETERS_DVBS, see DAE 7.13.19 */
	struct
	{
		OOIFNumber startFrequency;
		OOIFNumber endFrequency;
		OOIFNumber modulationModes;
		const char* symbolRate;
		OOIFNumber polarisation;
		const char* codeRate;
		OOIFNumber orbitalPosition;
		OOIFBoolean has_networkId;
		OOIFNumber networkId;
	} scan_parameters_dvbs;
	/** used by SCAN_PARAMETERS_DVBC, see DAE 7.13.20 */
	struct
	{
		OOIFNumber startFrequency;
		OOIFNumber endFrequency;
		OOIFNumber raster;
		OOIFBoolean startNetworkScanOnNIT;
		OOIFNumber modulationModes;
		const char* symbolRate;
		OOIFBoolean has_networkId;
		OOIFNumber networkId;
	} scan_parameters_dvbc;
} ScanParameters;

/**
   This data struct holds the static properties of a ChannelScanOptions as
   defined in DAE 7.13.14.
*/
typedef struct {
	OOIFNumber channelType;
	OOIFBoolean replaceExisting;
} ScanOptions;

#endif /* OIPF */

/**
   Checks if the channel list is not (partially) managed by the OITF
   (i.e. if the channel list information is managed entirely in the
   network). If 0, the channelList property will be NULL as defined in
   DAE1.1 7.13.9.1 / DAE2.1 7.13.10.1.
   \param retval Return TRUE if channel list is managed, FALSE if not.
   \retval OOIF_RETURN_OK
*/
OOIFReturnCode native_ChannelConfigIsChannelListManaged(OOIFBoolean *retval);

/**
   Gets the OIPF property channelList as defined in DAE1.1 7.13.9.1 / DAE2.1 7.13.10.1.
   \param count The number of returned channel handles
   \param handles Return handles to the the channels here. The OOIF
   plugin will take over ownership of the array and will later call
   free() to de-allocate the memory
   \return OOIF_RETURN_OK
*/
OOIFReturnCode native_ChannelConfigGetChannelList(int *count, ChannelHandle **handles);

#ifdef OIPF

/**
   Checks if the favourite lists are not (partially) managed by the
   OITF (i.e., if the favourite lists information is managed entirely
   in the network).  If 0, the favouriteList and currentFavouritList
   properties will be NULL as defined in DAE 7.13.9.1.
   \param retval Return TRUE if favourite ist is managed, FALSE if not.
   \return OOIF_RETURN_OK
*/
OOIFReturnCode native_ChannelConfigIsFavouriteListManaged(OOIFBoolean *retval);

/**
   Gets the OIPF property favouritLists as defined in DAE 7.13.10.1
   \param count The number of returned handles
   \param handles Return the favourite list handles here. The OOIF
   plugin will take over ownership of the array and will later call
   free() to de-allocate the memory
   \return OOIF_RETURN_OK
 */
OOIFReturnCode native_ChannelConfigGetFavouriteLists(int *count, FavouriteListHandle **handles);

/**
   Gets the OIPF property currentFavouritList as defined in DAE 7.13.10.1
   \param retval Return value as defined in DAE 7.13.10.1
   \return OOIF_RETURN_OK
 */
OOIFReturnCode native_ChannelConfigGetCurrentFavouriteList(FavouriteListHandle *retval);

/**
   Gets the OIPF property currentChannel as defined in DAE 7.13.10.1
   \param retval Return value as defined in DAE 7.13.10.1
   \return OOIF_RETURN_OK
 */
OOIFReturnCode native_ChannelConfigGetCurrentChannel(ChannelHandle *retval);

/**
   Checks if bdr is a valid XML-encoded string containing an SD&S Broadcast Discovery Record
   as defined in DAE 7.13.10.2. This function will be called every time before
   native_ChannelCreateChannelList
   \param bdr XML-encoded string containing an SD&S Broadcast Discovery Record
   \param retval should be set to true if bdr is valid
   \return OOIF_RETURN_OK
*/
OOIFReturnCode native_ChannelConfigIsValidBDR(const char* bdr, OOIFBoolean* retval);

/**
   This function corresponds to the OIPF function createChannelList as defined in DAE 7.13.10.2
   \param bdr An XML-encoded string containing an SD&S Broadcast Discovery Record
   \param count The number of returned channel handles
   \param handles  Return handles to the the channels here. The OOIF
   plugin will take over ownership of the array and will later call
   free() to de-allocate the memory.
   \return OOIF_RETURN_OK
   \see native_ChannelConfigIsValidBDR
*/
OOIFReturnCode native_ChannelCreateChannelList(const char* bdr, int *count, ChannelHandle **handles);

/**
   This function corresponds to the OIPF function createChannelObject as defined in DAE 7.13.10.2
   \param idType As defined in DAE 7.13.10.2
   \param has_onid is FALSE when the optional onid parameter isn't set.
   \param onid As defined in DAE 7.13.10.2
   \param has_tsid is FALSE when the optional tsid parameter isn't set.
   \param tsid As defined in DAE 7.13.10.2
   \param has_sid is FALSE when the optional sid parameter isn't set.
   \param sid As defined in DAE 7.13.10.2
   \param has_sourceID is FALSE when the optional sourceID parameter isn't set.
   \param sourceID As defined in DAE 7.13.10.2
   \param ipBroadcastID As defined in DAE 7.13.10.2, or NULL when not set.
   \param retval Retval as defined in DAE 7.13.10.2
   \return OOIF return code
*/
OOIFReturnCode native_ChannelConfigCreateChannelObject(OOIFNumber idType, OOIFBoolean has_onid, OOIFNumber onid, OOIFBoolean has_tsid, OOIFNumber tsid, OOIFBoolean has_sid, OOIFNumber sid, OOIFBoolean has_sourceID, OOIFNumber sourceID, const char *ipBroadcastID, ChannelHandle *retval);

/**
   This function corresponds to the OIPF function startScan as defined in DAE 7.13.10.2
   \param options ChannelScanOptions as defined in DAE 7.13.10.2
   \param type Describes a kind of scan to perform (DVB-T, DVB-S or DVB-C)
   \param parameters ChannelScanParameters as defined in DAE 7.13.10.2
   \param retval Retun valie as defined in DAE 7.13.10.2
   \return OOIF_RETURN_OK
*/
OOIFReturnCode native_ChannelConfigStartScan(const ScanOptions* options, ScanParametersType type, const ScanParameters* parameters, OOIFNumber* retval);

/**
   This function corresponds to the OIPF function stopScan as defined in DAE 7.13.10.2
   \return OOIF_RETURN_OK
*/
OOIFReturnCode native_ChannelConfigStopScan();

#endif /* OIPF */
/*@}*/
#ifdef OIPF
/*------------------------------------------
  Favourites List
  ------------------------------------------*/
/** @name FavouritesList
	The FavouritesList class as defined in DAE 7.13.14.
	@{*/
/**
   Gets all the channels in a favourite list.
   \param identifier The identifier of the favourite list instance.
   \param count The number of returned channels
   \param handles Return the channels here. The OOIF plugin will take
   over ownership of the array and will later call free() to de-allocate the memory
   \return OOIF_RETURN_OK
*/
OOIFReturnCode native_FavouriteListGetChannels(FavouriteListHandle identifier, int *count, ChannelHandle **handles);

/**
   Gets OIPF property favID as defined in DAE 7.13.14.1
   \param identifier The identifier of the favourite list instance.
   \param Retval as defined in DAE 7.13.14.1
   \return OOIF_RETURN_OK
*/
OOIFReturnCode native_FavouriteListGetFavID(FavouriteListHandle identifier, const char **retval);

/**
   Gets OIPF property name as defined in DAE 7.13.14.1
   \param identifier The identifier of the favourite list instance.
   \param retval Retval as defined in DAE 7.13.14.1
   \return OOIF_RETURN_OK
*/
OOIFReturnCode native_FavouriteListGetName(FavouriteListHandle identifier, const char **retval);

/**
   This function corresponds to the OIPF function getChannel as defined in DAE 7.13.14.2
   \param identifier The identifier of the favourite list instance.
   \param channelID As defined in DAE 7.13.14.2
   \param retval Retval as defined in DAE 7.13.14.2
   \return OOIF return code
*/
OOIFReturnCode native_FavouriteListGetChannel(FavouriteListHandle identifier, const char* channelID, ChannelHandle* retval);

/**
   This function corresponds to the OIPF function getChannelByTriplet as defined in DAE 7.13.14.2
   \param identifier The identifier of the favourite list instance.
   \param onid As defined in DAE 7.13.14.2
   \param has_tsid is FALSE when the optional tsid parameter isn't set.
   \param tsid As defined in DAE 7.13.14.2
   \param sid As defined in DAE 7.13.14.2
   \param retval Retval as defined in DAE 7.13.14.2
   \return OOIF return code
*/
OOIFReturnCode native_FavouriteListGetChannelByTriplet(FavouriteListHandle identifier, OOIFNumber onid, OOIFBoolean has_tsid, OOIFNumber tsid, OOIFNumber sid, ChannelHandle* retval);

/**
   This function corresponds to the OIPF function getChannelBySourceID as defined in DAE 7.13.14.2
   \param identifier The identifier of the favourite list instance.
   \param sourceID As defined in DAE 7.13.14.2
   \param retval Retval as defined in DAE 7.13.14.2
   \return OOIF return code
*/
OOIFReturnCode native_FavouriteListGetChannelBySourceID(FavouriteListHandle identifier, OOIFNumber sourceID, ChannelHandle* retval);

/**
   This function corresponds to the OIPF function commit as defined in DAE 7.13.14.2
   \param identifier The identifier of the favourite list instance
   \param ccids The CCIDs of the channels in the favourite list.
   The OOIF owns the array and will de-allocate the memory upon function return.
   \param channelCount Number of CCIDs in the ccids array
   \return OOIF return code
*/
OOIFReturnCode native_FavouriteListCommit(FavouriteListHandle identifier, char **ccids, int channelCount);

/**
   Creates a new Favourite List.
   \param retval Return the handle to the new list instance here
   \return OOIF_RETURN_OK
*/
OOIFReturnCode native_FavouriteListCreate(FavouriteListHandle *retval);

/**
   Called when a Favourite List instance is destroyed in the OOIF plugin.
   \param id Favourite List instance identifier
   \return OOIF_RETURN_OK
*/
OOIFReturnCode native_FavouriteListReleaseHandle(FavouriteListHandle id);
/*@}*/

#endif /* OIPF */

/*@}*/

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* __NATIVE_CHANNEL */

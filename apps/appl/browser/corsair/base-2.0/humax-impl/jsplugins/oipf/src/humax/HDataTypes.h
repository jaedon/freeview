
#ifndef __HDataTypes_H__
#define __HDataTypes_H__

namespace humax
{
	typedef	enum
	{
		ChannelConfigEvent_ChannelScan,
		ChannelConfigEvent_ListUpdated,
		ChannelConfigEvent_Connection
	} ChannelConfigEvent;

	typedef	struct
	{
		int		dummy;
	} ListUpdated_Data;

	#define	MAX_SEARCHEDINFO_NAME_LEN	128

	typedef	struct
	{
		int		onId;
		int		tsId;
		int		svcId;
		int		channelType;
		int		casType;
		int		lcn;
		char	name[MAX_SEARCHEDINFO_NAME_LEN];
	} SearchedInfo_Data;

	#define MAX_DATAS_LEN		64
	#define MAX_SYMBOL_LEN	(MAX_DATAS_LEN * 2)
	#define MAX_INFOS_LEN		(MAX_DATAS_LEN * 2)

	typedef struct
	{
		int		antennaId;
		char	antennaName[MAX_DATAS_LEN];
		int		startFreq;
		int		endFreq;
		int		modulationModes;
		char	symbolRate[MAX_SYMBOL_LEN];
		int		polarisation;
		char	codeRate[MAX_DATAS_LEN];
		float	orbitalPosition;
		int		networkId;
	} DVBS_TunedData;

	typedef struct
	{
		int		startFreq;
		int		endFreq;
		int		raster;
		bool	startNoetworkScanOnNIT;
		int		modulationModes;
		char	symbolRate[MAX_SYMBOL_LEN];
		int		networkId;
	} DVBC_TunedData;

	typedef struct
	{
		int		startFreq;
		int		endFreq;
		int		raster;
		char	transmission[MAX_DATAS_LEN];
		char	ofdm[MAX_DATAS_LEN];
		int		modulationModes;
		char	bandwidth[MAX_DATAS_LEN];
	} DVBT_TunedData;

	typedef struct
	{
		int		tunerType;
		union
		{
			DVBS_TunedData	stDVBSTunedData;
			DVBC_TunedData	stDVBCTunedData;
			DVBT_TunedData	stDVBTTunedData;
		} data;
	} ChannelTuned_Data;

	typedef	struct
	{
		int		type;
		int		progress;
		int		frequency;
		int		signalStrength;
		int		channelNumber;
		int		channelType;
		int		channelCount;
		int		transponderCount;

		int					infoCount;
		SearchedInfo_Data	*searchedInfos;
		ChannelTuned_Data	*tunedInfo;
	} ChannelScan_Data;

	typedef	struct
	{
		int		type;
		int		progress;
		int		connType;
	} ConnectionTest_Data;

	typedef	union
	{
		ListUpdated_Data	uListUpdateData;
		ChannelScan_Data	uChannelScanData;
		ConnectionTest_Data	uConnectionData;
	} ChannelConfigEventInfo;

	#define	MAX_LCNUPDATEDINFO_NAME_LEN		64
	#define	MAX_DBSVC_NAME_LEN				128

	typedef	enum
	{
		LcnUpdateEvent_Start_OK,
		LcnUpdateEvent_Start_FAIL,
		LcnUpdateEvent_Save_OK,
		LcnUpdateEvent_Save_FAIL,
		LcnUpdateEvent_Stop_OK,
		LcnUpdateEvent_Stop_FAIL,
		LcnUpdateEvent_Get_Info
	} LcnUpdateEvent;

	typedef struct
	{
		int		index;
		char	name[MAX_LCNUPDATEDINFO_NAME_LEN];
	} LcnUpdateSvcList_Data;

	typedef struct
	{
		int		lcn;
		char	name[MAX_DBSVC_NAME_LEN];
		char	ccid[MAX_DATAS_LEN];
	} LcnUpdateSvc_Data;

	typedef struct
	{
		int							numOfList;
		LcnUpdateSvcList_Data		*svcList;
	} LcnUpdateStart_Data;

	typedef struct
	{
		int							numOfNewSvc;
		LcnUpdateSvc_Data			*newSvc;

		int							numOfReplacedSvc;
		LcnUpdateSvc_Data			*replacedSvc;
	} LcnUpdateSave_Data;

	typedef struct
	{
		char		svcListName[MAX_LCNUPDATEDINFO_NAME_LEN];
		int			tvNum;
		int			radioNum;
		char		lastUpdatedTime[20];
		char		lastCheckedTime[20];
		bool		displayInfo;
		int			coldbootResult;
	} LncUpdateInfo_Data;

	typedef	union
	{
		LcnUpdateStart_Data	uStartData;
		LcnUpdateSave_Data	uSaveData;
		LncUpdateInfo_Data	uInfoData;
	} LcnUpdateEventInfo;

	typedef enum
	{
		eNCDEVENT_NONE =	0,
		eNCDEVENT_GET_INFO, 
		eNCDEVENT_END ,
	}NcdInfoEventType_e;
	typedef struct
	{
		char				ncdnetworkMsg[512];
		
	} NcdNetworkInfo_t;


	typedef enum
	{
		eTRDCONFLICT_UNKNOWN =	0,
		eTRDCONFLICT_GET_INFO, 
		eTRDCONFLICT_SET_INFO ,
	}TrdConflictEventType_e;

	typedef struct
	{
		int				 ulLen;
		char				szTrdConflictedSvcName[128];
	} TrdConflictSvc_t;
	typedef struct
	{
		int				 ulArrayNum;
		TrdConflictSvc_t	 *pstTrdConflictedArray;
	} TrdConflictList_t;

	typedef struct
	{
		TrdConflictEventType_e			eventType;
		unsigned int					trdconflictCount;

		// Using for get info
		TrdConflictList_t				trdconflictInfo;
		char				trdconflictStr[2048];
		
	} TrdConflictData_t;



}// humax

#endif // __HDataTypes_H__

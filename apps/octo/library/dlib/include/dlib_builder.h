/* $Header: $ */

#ifndef DLIB_BUILDER_H__
#define DLIB_BUILDER_H__

#ifdef __cplusplus
extern "C" {
#endif

#include <hlib.h>


/*******************************************************************/
/********************      Defines         *************************/
/*******************************************************************/

#define DxBUILDER_SATELLITE_NAME_LEN				(32)
#define DxBUILDER_SVC_NAME_LEN						(128)



/*******************************************************************/
/********************      Typedef         *************************/
/*******************************************************************/

typedef enum
{
	eDxBUILDER_STATE_NONE				= 0,

	eDxBUILDER_STATE_IDLE,
	eDxBUILDER_STATE_REQUESTED,
	eDxBUILDER_STATE_RUNNING,
	eDxBUILDER_STATE_PAUSED,
	eDxBUILDER_STATE_STOPPED,
	eDxBUILDER_STATE_FINISHED,

	eDxBUILDER_STATE_ENDOFCASE
} DxBuilder_State_e;

typedef enum
{
	eDxBUILDER_EVENT_NONE				= 0,

	eDxBUILDER_SEARCH_STATUS_CHANGED,

	eDxBUILDER_EVENT_ENDOFCASE
} DxBuilder_Event_e;

typedef enum
{
	eDxBUILDER_BUILDINFO_NONE,

	eDxBUILDER_BUILDINFO_TP,

	eDxBUILDER_BUILDINFO_ENDOFCASE
} DxBuilder_BuildInfoType_e;

typedef struct
{
	HUINT32						 ulTotalTpNum;
	HUINT32						 ulFinishedTpNum;
	HUINT32						 ulSearchedSvcNum;
	DxBuilder_State_e			 eState;
	DxTuneParam_t				 stTuneParam;
} DxBuilder_SearchStatus_t;

typedef union
{
	DxBuilder_SearchStatus_t	 stStatusChanged;
} DxBuilder_NotifierData_t;

typedef struct
{
	DxBuilder_BuildInfoType_e	 eType;
	union
	{
		DxTuneParam_t			 stTuneParam;
	} unArgs;
} DxBuilder_BuildInfo_t;

typedef	struct
{
	HINT32				onId;
	HINT32				tsId;
	HINT32				svcId;
	HUINT32				ulSvcType;		// DbSvc_Type_e
	HUINT32				ulCasType;		// DbSvc_CasType_e
	HUINT32				ulLcn;
	HUINT8				szSvcName[DxBUILDER_SVC_NAME_LEN];
} DxBuilder_SearchedInfo_t;

typedef	struct
{
	HUINT32		ulAntId;
	HCHAR		szSatelliteName[DxBUILDER_SATELLITE_NAME_LEN];
} DxBuilder_SatelliteInfo_t;

typedef	struct
{
	DxSvcType_e			eSvcType;
	DxCasType_e			eCasType;
	HBOOL				bNetworkSearch;
	HBOOL				bWithNetworkId;
	HUINT32				ulNetworkId;
	HBOOL				bReplaceChannel;
} DxBuilder_Option_t;


#ifdef __cplusplus
}
#endif

#endif // DLIB_BUILDER_H__

/* end of file */


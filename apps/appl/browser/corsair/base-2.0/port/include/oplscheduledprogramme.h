/**************************************************************************************/
/**
 * @file oplscheduledprogramme.h
 *
 *
 *
 * @attention   Copyright (c) 2011 Humax - All rights reserved.
 *
 **************************************************************************************
 **/

#ifndef __OPLSCHEDULEDPROGRAMME_H__
#define __OPLSCHEDULEDPROGRAMME_H__

#include	"opltype.h"
#include	"oplprogramme.h"

#ifdef __cplusplus
extern "C" {
#endif	//__cplusplus

enum OPL_ScheduledItemType
{
	eOPL_SCHEDULED_PRO_RECORD,
	eOPL_SCHEDULED_PRO_WATCH,
	eOPL_SCHEDULED_PRO_POWER_ON_TIMER,
	eOPL_SCHEDULED_PRO_DLNA,
	eOPL_SCHEDULED_PRO_DUBBING,
	eOPL_SCHEDULED_PRO_NETSTREAMING_LIVE,
	eOPL_SCHEDULED_PRO_NETSTREAMING_FILE,
	eOPL_SCHEDULED_PRO_SAT_RECORD,
	eOPL_SCHEDULED_PRO_REGULAR_OTA,
	eOPL_SCHEDULED_PRO_ASO_NOTIFY,
	eOPL_SCHEDULED_PRO_UNKNOWN
};

enum OPL_ScheduledItemIDType
{
	eOPL_SCHEDULED_PRO_ID_TVA_CRID,
	eOPL_SCHEDULED_PRO_ID_TVA_EVENT
};


enum OPL_ScheduledItemRepeat
{
	eOPL_SCHEDULED_PRO_SUNDAY		= 0x01,
	eOPL_SCHEDULED_PRO_MONDAY		= 0x02,
	eOPL_SCHEDULED_PRO_TUESDAY		= 0x04,
	eOPL_SCHEDULED_PRO_WEDNESDAY	= 0x08,
	eOPL_SCHEDULED_PRO_THURSDAY		= 0x10,
	eOPL_SCHEDULED_PRO_FRIDAY		= 0x20,
	eOPL_SCHEDULED_PRO_SATURDAY		= 0x40
};

#define	OPL_SCHEDULED_CRID_AUTH_LEN							32
#define	OPL_SCHEDULED_CRID_DATA_LEN							29
#define	OPL_SCHEDULED_CRID_IMI_LEN							3
#define	OPL_SCHEDULED_CRID_LEN								(OPL_SCHEDULED_CRID_AUTH_LEN + OPL_SCHEDULED_CRID_DATA_LEN + OPL_SCHEDULED_CRID_IMI_LEN)
#define	OPL_SCHEDULED_NUM_RECORDED_PROG_SAVE				5


typedef struct{
	HUINT8 		ucCRIDType;
	HCHAR 		szCRID[OPL_SCHEDULED_CRID_LEN +1];
}OPL_ScheduledTvAnyTimetCRID_t;

typedef	struct{
	OPL_ScheduledTvAnyTimetCRID_t  stCRID[OPL_SCHEDULED_NUM_RECORDED_PROG_SAVE];
}OPL_ScheduledAusDttItem_t;


typedef struct
{
	unsigned int sizeOf;				///< @see macro OPL_ScheduledProgramme_InstanceOf()
	int					uid;
	int					type;				///< @see enum OPL_ScheduledItemType
	unsigned long		startTime;
	unsigned long		duration;
	unsigned long		startPadding;
	unsigned long		endPadding;
	int					repeatDays;			///< @see enum OPL_ScheduledProgrammeRepeat
	int					targetStorage;

	OPL_BOOL			isScheduledAsSeries;

	char				*name;
	char				*longName;
	char				*description;
	char				*longDescription;
	char				*channelID;
	char				*programmeID;
	char				*seriesID;
	int					episode;
	int					season;
	int					totalEpisodes;
	char				*storageID;
	int					recMode;
	int					volume;
	int					status;		//DxRsvStatus_e
	int					lcn;
	OPL_BOOL			fromDap;
	int					epgType;
	OPL_BOOL			freeCaMode;
	int					Rating;
	OPL_BOOL			eventbased;
	int					groupIndex;
	char				*userID;
	char				*deviceHandle;
	char				*destinationId;
	char				*dubbingId;				///< @use to dlna dubbing

	/* to display tuning info for SAT>IP */
	int				diseqcInput;
	int				frequency;
	int				symbolRate;
	int				polarisation;
	char				*satType;

	/* Factory Reset후 Ch DB가 없는 상태로 record list가 만들어진 상황에서 Ch가 검색되어 record list가 처리될 때의 예외 처리 */
	int 				serviceId;		   	/* service id */
	int 				transportStreamId; 	/* transport stream id */
	int 				originalNetworkId; 	/* original network id */

	OPL_Collection_t	*crid;				/* DxEventCRID_t */
} OPL_ScheduledItem_t;

typedef void *	OPL_ScheduledHandle;
typedef void *	OPL_RecordingHandle;

/**
 * USAGE:
 * if (OPL_ScheduledItem_InstanceOf(obj, OPL_Recording_t))
 *     do something...
 */
#define	OPL_ScheduledItem_InstanceOf(instance, type)	\
	(((OPL_ScheduledItem_t *)(instance))->sizeOf == sizeof(type))

OPL_ScheduledItem_t *
	OPL_ScheduledItem_New (const char *signature, ...);

OPL_ScheduledItem_t *
	OPL_ScheduledItem_NewWithDummy (void);

OPL_ScheduledItem_t *
	OPL_ScheduledItem_NewWithProgramme (int type, OPL_ProgrammeHandle programme, const char *storageID);

OPL_ScheduledItem_t *
	OPL_ScheduledItem_NewWithTime (int type, unsigned long startTime, unsigned long duration, int repeatDays, const char *channelID, const char *storageID, int volume);

OPL_ScheduledItem_t *
	OPL_ScheduledItem_NewWithCrid (int recType, int isScheduledAsSeries, const char *Crid, const char *title, const char *channelID, const char *storageID);

OPL_ScheduledItem_t *
	OPL_ScheduledItem_Clone (const OPL_ScheduledItem_t *self);

void
	OPL_ScheduledItem_Delete (OPL_ScheduledItem_t *self);

int
	OPL_ScheduledItem_GetParentalRating (OPL_ScheduledItem_t *self);

void *
	OPL_ScheduledItem_CloneCrid (void* data);

void
	OPL_ScheduledItem_FreeCrid (void* data);

OPL_ScheduledItem_t *
	OPL_ScheduledItem_jlabsNewWithProgramme (int type, OPL_ProgrammeHandle programme, int recmode, const char *storageID, const char *devicehandle, const char *destinationId);

OPL_ScheduledItem_t *
	OPL_ScheduledItem_jlabsNewWithTime (int type, unsigned long startTime, unsigned long duration, int repeatDays, const char *channelID, int recmode, const char *storageID, const char *devicehandle, const char *destinationId);

OPL_ScheduledItem_t *
	OPL_ScheduledItem_jlabsNewWithRecording (int type, unsigned long startTime, OPL_RecordingHandle identifier, const char *devicehandle, const char *destinationId);

#ifdef __cplusplus
};
#endif	//__cplusplus

#endif	//__OPLSCHEDULEDPROGRAMME_H__


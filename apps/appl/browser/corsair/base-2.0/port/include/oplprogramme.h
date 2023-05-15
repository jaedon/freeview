/**************************************************************************************/
/**
 * @file oplprogramme.h
 *
 * Programme
 *
 **************************************************************************************
 **/

#ifndef __OPLPROGRAMME_H__
#define __OPLPROGRAMME_H__

#include "bpltype.h"
#include "oplcollection.h"
#include "oplpcmanager.h"
#include "oplchannel.h"
#include "oplavcomponent.h"
#include <hlib.h>

#ifdef __cplusplus
extern "C" {
#endif

/**
 * Programme.programmeIDType property
 *
 * ID_TVA_CRID			0
 *	  - dvb://<original_network_id>.[<transport_stream>].<service_id>;[<event_id>];<TVA_id>[;<service_unique_id>]
 *
 * ID_DVB_EVENT 		1
 *	  - dvb://<original_network_id>.[<transport_stream>].<service_id>;<event_id>[;;<service_unique_id>]
 *
 * ID_TVA_GROUP_CRID	2
 * ID_FSAT_EVENT		16	//'F'
 *	  - fsat://<service_id-digit>;<event_id-hex>
 */
enum OPL_ProgrammeIDType
{
	eOPL_ProgrammeID_TVA_CRID	= 0,
	eOPL_ProgrammeID_DVB_EVENT	= 1,
	eOPL_ProgrammeID_TVA_GROUP_CRID	= 2,

	eOPL_ProgrammeID_FSAT_EVENT	= 10,

	eOPL_ProgrammeID_Unknown	= -1
};

typedef enum
{
	ANALOG_COPY_TYPE = 0,
	DIGITAL_COPY_TYPE =1,

	MAX_COPY_TYPE
}eCopyControlType;

typedef enum
{
	OUTPUT_COPY_FREE = 0,
	OUTPUT_COPY_1_OR_10 = 2,
	OUTPUT_COPY_NEVER =3,
	OUTPUT_NEVER =4,
}eOoutputCopy;

typedef struct
{
	HUINT8							ucGuidanceType;
	HUINT8							ucGuidanceMode;
	HUINT8  						szIso639LangCode[4];
	HUINT8  						szGuidance[256];
} Opl_Guidance_t;

typedef void *	OPL_ProgrammeHandle;

OPL_ProgrammeHandle	OPL_Programme_New (const char *signature, ...);
void			OPL_Programme_Delete (OPL_ProgrammeHandle self);
OPL_ProgrammeHandle	OPL_Programme_Clone (OPL_ProgrammeHandle self);
OPL_BOOL		OPL_Programme_Equals (OPL_ProgrammeHandle self, OPL_ProgrammeHandle other);
HINT32			OPL_Programme_GetEventId(OPL_ProgrammeHandle self);
const char *	OPL_Programme_GetName(OPL_ProgrammeHandle self);
const char *	OPL_Programme_GetDescription(OPL_ProgrammeHandle self);
unsigned long	OPL_Programme_GetStartTime(OPL_ProgrammeHandle self);
unsigned long	OPL_Programme_GetDuration(OPL_ProgrammeHandle self);
const char *	OPL_Programme_GetChannelID(OPL_ProgrammeHandle self);
//OPL_Collection_t *	OPL_Programme_GetParentalRating(OPL_ProgrammeHandle self);
int				OPL_Programme_GetParentalRating(OPL_ProgrammeHandle self);
void			OPL_Programme_SetName(OPL_ProgrammeHandle self, const char* name);
void			OPL_Programme_SetDescription(OPL_ProgrammeHandle self, const char* description);
void			OPL_Programme_SetLongDescription(OPL_ProgrammeHandle self, const char* longDescription);
void			OPL_Programme_SetStartTime(OPL_ProgrammeHandle self, unsigned long startTime);
void			OPL_Programme_SetDuration(OPL_ProgrammeHandle self, unsigned long duration);
void			OPL_Programme_SetChannelID(OPL_ProgrammeHandle self, const char* channelID);
void			OPL_Programme_SetEpisode(OPL_ProgrammeHandle self, int episode);
void			OPL_Programme_SetTotalEpisodes(OPL_ProgrammeHandle self, int totalEpisodes);
void			OPL_Programme_SetProgrammeID(OPL_ProgrammeHandle self, const char* programmeID);
void			OPL_Programme_SetProgrammeIDType(OPL_ProgrammeHandle self, int programmeIDType);
const char *	OPL_Programme_GetLongName(OPL_ProgrammeHandle self);
const char *	OPL_Programme_GetLongDescription(OPL_ProgrammeHandle self);
const char *	OPL_Programme_GetProgrammeID(OPL_ProgrammeHandle self);
int				OPL_Programme_GetEpisode(OPL_ProgrammeHandle self);
int				OPL_Programme_GetSeason(OPL_ProgrammeHandle self);
int				OPL_Programme_GetTotalEpisodes(OPL_ProgrammeHandle self);
int				OPL_Programme_GetProgrammeIDType(OPL_ProgrammeHandle self);
int				OPL_Programme_GetShowType(OPL_ProgrammeHandle self);
int				OPL_Programme_GetAudioType(OPL_ProgrammeHandle self);
int				OPL_Programme_GetSubtitleType(OPL_ProgrammeHandle self);
OPL_BOOL		OPL_Programme_IsBlocked(OPL_ProgrammeHandle self);
OPL_BOOL		OPL_Programme_HasSubtitle(OPL_ProgrammeHandle self);
OPL_Collection_t		*OPL_Programme_GetSubtitleTypes(OPL_ProgrammeHandle self);
OPL_BOOL		OPL_Programme_IsHD(OPL_ProgrammeHandle self);
OPL_BOOL		OPL_Programme_IsMultilingual(OPL_ProgrammeHandle self);
OPL_BOOL		OPL_Programme_IsLocked(OPL_ProgrammeHandle self);
OPL_BOOL		OPL_Programme_HasRecording(OPL_ProgrammeHandle self);
OPL_Collection_t	*OPL_Programme_GetGenre(OPL_ProgrammeHandle self);
OPL_Collection_t	*OPL_Programme_GetAudioLanguages(OPL_ProgrammeHandle self);
OPL_Collection_t	*OPL_Programme_GetAudioComponent(OPL_ProgrammeHandle self);
OPL_Collection_t	*OPL_Programme_GetSubtitleLanguages(OPL_ProgrammeHandle self);
OPL_Channel_t	OPL_Programme_GetChannel(OPL_ProgrammeHandle self);
OPL_HANDLE		OPL_Programme_GetScheduledRecording(OPL_ProgrammeHandle self);
OPL_HANDLE		OPL_Programme_GetRecording(OPL_ProgrammeHandle self);
OPL_Collection_t*	OPL_Programme_GetRecordings(OPL_ProgrammeHandle self);
OPL_Collection_t	*OPL_Programme_GetGroupCRIDs(OPL_ProgrammeHandle self);
OPL_BOOL		OPL_Programme_GetCRIDSearchFlag(OPL_ProgrammeHandle self);
const char *	OPL_Programme_GetProgrammeCRID(OPL_ProgrammeHandle self);
const char *	OPL_Programme_GetRecommendationCRID(OPL_ProgrammeHandle self);
OPL_Collection_t	*OPL_Programme_GetGroupCRIDsAndType(OPL_ProgrammeHandle self);
const char *	OPL_Programme_GetIMI(OPL_ProgrammeHandle self);
const char *	OPL_Programme_GetField(OPL_ProgrammeHandle self, const char *field);
OPL_Collection_t	*OPL_Programme_GetSiDescription(OPL_ProgrammeHandle self, int tag, int tagExtention);

int				OPL_Programme_GetEventIDFromProgrammeID (const char *programmeID);
int				OPL_Programme_GetSvcUIDFromProgrammeID (const char *programmeID);
#ifndef CONFIG_FUNC_EPG_USE_SVCUID
void			OPL_Programme_MakeProgrammeID (char *buf, unsigned int n, int onid, int tsid, int svcid, int eventId);
#else
void			OPL_Programme_MakeProgrammeID (char *buf, unsigned int n, int svcuid, int eventId);
#endif

int				OPL_Programme_GetIDTypeFromProgrammeID (const char *programmeID);
OPL_BOOL		OPL_Programme_GetUniqueIDsFromProgrammeID (const char *programmeID, int *svcuid, int *eventid);
OPL_BOOL		OPL_Programme_GetTrippleIDFromProgrammeID (const char *programmeID, int *onid, int *tsid, int *svcid);


OPL_Collection_t	*OPL_Programme_GetComponents (OPL_ProgrammeHandle self, eOplAVComponentType eType );
HERROR		OPL_Programme_GetMultiViewComponent( OPL_ProgrammeHandle self , HxVector_t *descList );
int				OPL_Programme_GetCopyControl(OPL_ProgrammeHandle self, eCopyControlType eType );
OPL_BOOL		OPL_Programme_GetIs3D(OPL_ProgrammeHandle self);
OPL_BOOL		OPL_Programme_GetIsCharged(OPL_ProgrammeHandle self);
const char		*OPL_Programme_GetContractVerificationInfo(OPL_ProgrammeHandle self);
int				OPL_Programme_GetRecordingStatus(OPL_ProgrammeHandle self);
unsigned long		OPL_Programme_GetEpgType(OPL_ProgrammeHandle self);
const char *	OPL_Programme_GetProgrammeIDSubtype(OPL_ProgrammeHandle self);
OPL_Collection_t	*OPL_Programme_GetSharedProgrammeIds(OPL_ProgrammeHandle self);
OPL_ProgrammeHandle	OPL_Programme_GetSharedProgrammeInfo(const char* pszSharedProgrammeId);
HCHAR 		*OPL_PROGRAMME_GetGenre (HUINT16 content);

// releated in MediaDeeplink
unsigned long	OPL_Programme_GetMediaDeeplinkExpireDate(OPL_ProgrammeHandle self);
const char *	OPL_Programme_GetMediaDeeplinkUrl(OPL_ProgrammeHandle self);
const char *	OPL_Programme_GetMediaDeeplinkID(OPL_ProgrammeHandle self);

// Freeview Connected OnDemand
unsigned long	OPL_Programme_GetOnDemandStartOfAvailability(OPL_ProgrammeHandle self);
unsigned long	OPL_Programme_GetOnDemandEndOfAvailability(OPL_ProgrammeHandle self);
unsigned long	OPL_Programme_GetOnDemandPublishedDuration(OPL_ProgrammeHandle self);
OPL_BOOL		OPL_Programme_GetOnDemandIsFree(OPL_ProgrammeHandle self);
OPL_BOOL		OPL_Programme_GetOnDemandIsHD(OPL_ProgrammeHandle self);
OPL_BOOL		OPL_Programme_GetOnDemandIsMediaAvailable(OPL_ProgrammeHandle self);
OPL_BOOL		OPL_Programme_GetOnDemandIsFEPGAvailable(OPL_ProgrammeHandle self);
const char *	OPL_Programme_GetOnDemandProgramUrl(OPL_ProgrammeHandle self);
const char *	OPL_Programme_GetOnDemandAuxiliaryUrl(OPL_ProgrammeHandle self);
const char *	OPL_Programme_GetOnDemandDeliveryMode(OPL_ProgrammeHandle self);


#ifdef __cplusplus
};
#endif

#endif


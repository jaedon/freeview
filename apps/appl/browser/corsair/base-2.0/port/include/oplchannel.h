/**************************************************************************************/
/**
 * @file oplchannel.h
 *
 * Defines the channel structure.
 *
 **************************************************************************************
 **/

#ifndef __OPLCHANNEL_H__
#define __OPLCHANNEL_H__

#include	"opltype.h"
#include 	"bpltype.h"
#include	"oplcollection.h"

#ifdef __cplusplus
extern "C" {
#endif

#define USE_NEW_FAV
#define STEP fflush(NULL);/*printf("######shshshshsh########[%s:%d]\n",__PRETTY_FUNCTION__,__LINE__);*/fflush(NULL);


#define 	OPL_MACRO_IS_JAPAN_DT_NETWORK(onID)		( (onID & 0xF000) == 0x7000 )
#define 	OPL_MACRO_IS_JAPAN_BS_NETWORK(onID)		(onID == 0x0004)
#define 	OPL_MACRO_IS_JAPAN_CATV_NETWORK(onID)	((onID == 0xFFFE) || (onID == 0xFFFA)) 	/* 0xFFFE (REMUX/iHITS), 0xFFFA (Advanced REMUX/Advanced iHITS) */
#define	OPL_MACRO_IS_JAPAN_CS1_NETWORK(onID)		(onID == 0x0006)
#define	OPL_MACRO_IS_JAPAN_CS2_NETWORK(onID)		(onID == 0x0007)
#define	OPL_MACRO_IS_JAPAN_JCHITS_NETWORK(onID)	((onID == 0xFFFD) || (onID == 0xFFF9)) 	/* 0xFFFD (JC-HITS), 0xFFF9 (Advanced JC-HITS) */

typedef enum
{
	eOPL_ChannelType_TV		= 0,
	eOPL_ChannelType_RADIO	= 1,
	eOPL_ChannelType_OTHER	= 2
} OPL_ChannelType_e;

typedef enum
{
	eOPL_ChannelID_ANALOG			= 0,
	eOPL_ChannelID_DVB_C			= 10,
	eOPL_ChannelID_DVB_S			= 11,
	eOPL_ChannelID_DVB_T			= 12,
	eOPL_ChannelID_DVB_SI_DIRECT	= 13,
	eOPL_ChannelID_DVB_C2 			= 14,
	eOPL_ChannelID_DVB_S2 			= 15,
	eOPL_ChannelID_DVB_T2 			= 16,
	eOPL_ChannelID_ISDB_C 			= 20,
	eOPL_ChannelID_ISDB_S 			= 21,
	eOPL_ChannelID_ISDB_T 			= 22,
	eOPL_ChannelID_ATSC_T 			= 30,
	eOPL_ChannelID_IPTV_SDS			= 40,
	eOPL_ChannelID_IPTV_URI			= 41
} OPL_ChannelID_e;

typedef enum
{
	eOPL_ChannelCasID_FTA			= 0,
	eOPL_ChannelCasID_NAGRA			= 1,
	eOPL_ChannelCasID_IRDETO		= 2,
	eOPL_ChannelCasID_NDS			= 3,
	eOPL_ChannelCasID_VIACCESS		= 4,
	eOPL_ChannelCasID_CONAX			= 5,
	eOPL_ChannelCasID_MEDIAGUARD	= 6,
	eOPL_ChannelCasID_CRYPTOWORKS	= 7,
	eOPL_ChannelCasID_JPBCAS		= 10,
	eOPL_ChannelCasID_JPCCAS		= 11,
	eOPL_ChannelCasID_ALLCAS		= 12,
	eOPL_ChannelCasID_VERIMATRIX	= 13
} OPL_ChannelCasID_e;

typedef enum
{
	eOPL_CHANNEL_STATE_UPDATED,
	eOPL_CHANNEL_STATE_INVALID,

	eOPL_CHANNEL_STATE_UNKNOWN
} OPL_ChannelState_e;

typedef enum{
	eOPL_NETWORK_START = 0,
	eOPL_DTT_NETWORK = 1,
	eOPL_BS_NETWORK = 2,
	eOPL_CATV_NETWORK = 3,
	eOPL_CS1_NETWORK = 4,
	eOPL_CS2_NETWORK = 5,
	eOPL_JCHITS_NETWORK = 6,
	eOPL_NETWORK_END,
}OPL_NetworkType_e;

typedef enum
{
	eOPL_AribLogo_HD_LARGE = 0,	/* 64x36 */
	eOPL_AribLogo_HD_SMALL,		/* 48x27 */
	eOPL_AribLogo_SD43_LARGE,	/* 72x36 */
	eOPL_AribLogo_SD43_SMALL,	/* 48x24 */
	eOPL_AribLogo_SD169_LARGE,	/* 54x36 */
	eOPL_AribLogo_SD169_SMALL,	/* 36x24 */
	eOPL_AribLogo_Unkown,
}OPL_AribLogoType_e;

typedef void *	OPL_Channel_t;
typedef void *	OPL_ChannelManager_t;

typedef void	(* OPL_ChannelListener_t) (OPL_ChannelManager_t, OPL_ChannelState_e, OPL_Collection_t *);
typedef void	(* OPL_ResultCreateMaskListener_t) (int result);

OPL_ChannelManager_t	OPL_Channel_GetManager (void);
void			OPL_Channel_AddListener (OPL_ChannelManager_t manager, OPL_ChannelListener_t listener);

OPL_ChannelManager_t	OPL_Channel_LoadList (OPL_ChannelManager_t manager);

int				OPL_Channel_GetFreesatGenre (OPL_ChannelManager_t manager, char ***genre);
int		OPL_Channel_GetFreesatGenreExt (OPL_ChannelManager_t manager, char ***genreExt);
OPL_Channel_t	OPL_Channel_FindChannelByUID (OPL_ChannelManager_t manager, int uid);
int				OPL_Channel_GetChannelList (OPL_ChannelManager_t manager, OPL_Channel_t **channelList);
int				OPL_Channel_GetFilteredChannelList (OPL_ChannelManager_t manager, OPL_Channel_t **channelList, OPL_BOOL (*filter) (OPL_Channel_t, void *), void *userdata);
int				OPL_Channel_ForEach (OPL_ChannelManager_t manager, OPL_BOOL (*filter) (OPL_Channel_t, void *), void *userdata);
int				OPL_Channel_IndexOf (OPL_ChannelManager_t manager, OPL_Channel_t channel);
OPL_Channel_t	OPL_Channel_FindChannel (OPL_ChannelManager_t manager, const char *ccid);
OPL_Channel_t	OPL_Channel_FindChannelByTriplet (OPL_ChannelManager_t manager, int onid, int tsid, int sid);
OPL_Channel_t	OPL_Channel_FindChannelByNumber (OPL_ChannelManager_t manager, int major, int minor);
OPL_Channel_t	OPL_Channel_FindChannelAt (OPL_ChannelManager_t manager, int index);
OPL_BOOL 		OPL_Channel_IsPossibleToTuneByUpDown(OPL_Channel_t channel, int tuningRange);
int				OPL_Channel_Count (OPL_ChannelManager_t manager);
void			OPL_Channel_RemoveChannel (OPL_ChannelManager_t manager, OPL_Channel_t channel);
void		      OPL_Channel_RemoveChannelList (OPL_ChannelManager_t manager);
int			OPL_Channel_GetNetworkIDList(OPL_NetworkType_e netType, OPL_ChannelManager_t manager, int ** ppDttNetworkList);
void			OPL_Channel_FreeNetworkIDList(int *pNetworkList);
OPL_NetworkType_e	OPL_Channel_GetNetTypeByOnID(int onId);
int			OPL_Channel_GetChannelListByOnID (int onId, OPL_ChannelManager_t manager, OPL_Channel_t **channelList);
void			OPL_Channel_FreeChannelList(OPL_Channel_t *channelList);

void    		OPL_Channel_AddChannel (OPL_ChannelManager_t manager, OPL_Channel_t channel);
OPL_Channel_t	OPL_Channel_Clone (OPL_Channel_t channel);
void			OPL_Channel_Delete (OPL_Channel_t channel);
int				OPL_Channel_GetUniqueID (OPL_Channel_t channel);
char *			OPL_Channel_MakeCCID (char *buf, unsigned int n, int uid, int onid, int tsid, int sid);
int				OPL_Channel_CCID2UniqueID (const char *ccid);
OPL_BOOL		OPL_Channel_IsSupportedChannelID (OPL_ChannelID_e id);
OPL_ChannelType_e	OPL_Channel_GetChannelType (OPL_Channel_t channel);
int				OPL_Channel_GetOrgSvcType(OPL_Channel_t channel);
OPL_ChannelID_e		OPL_Channel_GetIdType (OPL_Channel_t channel);
OPL_BOOL		OPL_Channel_HasCasID (OPL_Channel_t channel, OPL_ChannelCasID_e id);
int				OPL_Channel_GetCasIDs (OPL_Channel_t channel, OPL_ChannelCasID_e **ids);
char *			OPL_Channel_GetCCID (OPL_Channel_t channel, char *buf, unsigned int n);
BPLStatus			OPL_Channel_GetIntTunerID (OPL_Channel_t channel , HINT32 *tunerId);
char *			OPL_Channel_GetTunerID (OPL_Channel_t channel, char *buf, unsigned int n);
char *	        OPL_Channel_GetTuningInfo (OPL_Channel_t channel, char *buf, unsigned int n);
int				OPL_Channel_GetOrgNetworkID (OPL_Channel_t channel);
int				OPL_Channel_GetTransponderID (OPL_Channel_t channel);
int				OPL_Channel_GetServiceID (OPL_Channel_t channel);
int				OPL_Channel_GetSourceID (OPL_Channel_t channel);
int				OPL_Channel_GetFrequency (OPL_Channel_t channel);
int				OPL_Channel_GetCNI (OPL_Channel_t channel);
const char *	OPL_Channel_GetName (OPL_Channel_t channel);
int				OPL_Channel_GetMajorChannel (OPL_Channel_t channel);
int				OPL_Channel_GetMinorChannel (OPL_Channel_t channel);
int				OPL_Channel_GetDeliverySystemDescriptor (OPL_Channel_t channel, unsigned char **buf);
int				OPL_Channel_GetFavouriteIDs (OPL_Channel_t channel, char ***favIDs);
int				OPL_Channel_GetSvcSection (OPL_Channel_t channel);
OPL_BOOL		OPL_Channel_IsHD (OPL_Channel_t channel);
OPL_BOOL		OPL_Channel_IsLocked (OPL_Channel_t channel);
OPL_BOOL		OPL_Channel_HasTeletext (OPL_Channel_t channel);
OPL_BOOL		OPL_Channel_IsManualBlocked (OPL_Channel_t channel);
void			OPL_Channel_SetManualBlock (OPL_Channel_t channel, OPL_BOOL block);
const char *	OPL_Channel_GetName (OPL_Channel_t channel);
void			OPL_Channel_SetName (OPL_Channel_t channel, const char* chname);
void			OPL_Channel_SetMajorChannel (OPL_Channel_t channel, int majorChannel);
OPL_BOOL		OPL_Channel_IsRemoved (OPL_Channel_t channel);
void			OPL_Channel_SetRemovedFlag (OPL_Channel_t channel, OPL_BOOL removed);
OPL_BOOL		OPL_Channel_IsNewChannel(OPL_Channel_t channel);
void			OPL_Channel_SetNewChannelFlag(OPL_Channel_t channel, OPL_BOOL newChannel);
char *			OPL_Channel_GetIPBroadcastID (OPL_Channel_t channel, char *buf, unsigned int n);
int				OPL_Channel_GetChannelMaxBitRate (OPL_Channel_t channel);
int				OPL_Channel_GetChannelTTR (OPL_Channel_t channel);
const char *	OPL_Channel_GetLongName (OPL_Channel_t channel);
const char *	OPL_Channel_GetDescription (OPL_Channel_t channel);
OPL_BOOL		OPL_Channel_IsAuthorised (OPL_Channel_t channel);
OPL_BOOL		OPL_Channel_HasGenre (OPL_Channel_t channel, const char *genre);
int				OPL_Channel_GetGenre (OPL_Channel_t channel, char ***genre);
OPL_BOOL		OPL_Channel_IsHidden (OPL_Channel_t channel);
const char *	OPL_Channel_GetLogoURL (OPL_Channel_t channel);
const char *	OPL_Channel_GetSvcOpTypeString (OPL_Channel_t channel);
char *			OPL_Channel_GetField (OPL_Channel_t channel, const char *field);
int				OPL_Channel_GetFreesatServiceID (OPL_Channel_t channel);
int				OPL_Channel_GetFsatSvcIdByUID (OPL_ChannelManager_t manager, int uid);
OPL_Channel_t 	OPL_Channel_GetRepresentativeChannelInChannelList(OPL_Channel_t *pChannels, int length);
int				OPL_Channel_GetOneTouch(OPL_Channel_t channel);
void				OPL_Channel_SetOneTouch(OPL_Channel_t channel, int oneTouch);
int				OPL_Channel_GetAreacode(OPL_Channel_t channel);
BPLStatus		OPL_Channel_GetSignalLevel(OPL_Channel_t channel,unsigned int *pulAgc, unsigned int *pulBer, unsigned int*pulSnr, int*pnStrength);
int				OPL_Channel_GetBranchNum(OPL_Channel_t channel);
BPLStatus		OPL_Channel_GetTsName(OPL_Channel_t channel, char *tsName, unsigned int length);
OPL_BOOL		OPL_Channel_IsSkipChannel (OPL_Channel_t channel);
void				OPL_Channel_SetSkipChannel (OPL_Channel_t channel, OPL_BOOL bSkip);
BPLStatus		OPL_Channel_GetLastUpdateDate(OPL_Channel_t channel, int *lastUpdateDate);
char *			OPL_Channel_GetLogo (OPL_Channel_t channel, int width, int height);
OPL_BOOL		OPL_Channel_IsSameNetwork(OPL_Channel_t hChannel, OPL_Channel_t hCmpChannel);
BPLStatus			OPL_Channel_GetProviderName(OPL_Channel_t channel, char *provName);
int			OPL_Channel_GetFreeCAMode(OPL_Channel_t channel);
OPL_BOOL 	OPL_Channel_GetExistingSdtCAD(OPL_Channel_t channel);
char * 		OPL_Channel_GetSubscriptionResponse(OPL_Channel_t channel);
void	OPL_Channel_SetResultCreateMaskListener (OPL_ResultCreateMaskListener_t listener);
OPL_BOOL 	OPL_Channel_GetCurrentChannel(OPL_Channel_t *pChannel);
void OPL_Channel_GetdefaultAuth(OPL_Channel_t Channel, char *pucDefautAuth);
#define	OPL_CHANNEL_GET_STRING(retval,channel,func)	\
	do {\
		char __buf[512];\
		if (func(channel,__buf,512))\
			retval = HLIB_STD_StrDup(__buf);\
		else retval = NULL;\
	} while (0)
/******************************************************************
	Define
******************************************************************/
#define OPLCHANNEL_MAX_NAME_LEN 		   	(64)
#define OPLCHANNEL_MAX_ID_LEN 			   	(64)


#ifdef __cplusplus
};
#endif



#endif


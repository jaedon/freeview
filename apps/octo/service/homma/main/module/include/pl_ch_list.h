#ifndef _PL_CHANNEL_LIST_H_
#define _PL_CHANNEL_LIST_H_

#include <common.h>
#include <pl_common.h>

typedef void (*ON_CHANNEL_CHANGED) ( void );

/*  channel list  */
extern HERROR	PL_COMMON_InitChannelList(void);

extern HERROR	PL_COMMON_GetChannelCount(HUINT32 *pCount);

extern HERROR	PL_COMMON_GetChannel(HINT32 uid, CO_CHANNEL_t *pChannel);

extern HERROR	PL_COMMON_GetLastVersion(HUINT32 *pnLastVersion);

extern HERROR	PL_COMMON_GetChannelList(HxList_t **list);

extern HERROR	PL_COMMON_ReleaseChannelList(HxList_t *list);

extern HERROR	PL_COMMON_UpdateChannelList(void);

extern HERROR	PL_COMMON_GetAntenaList(HxList_t **list);

extern void		PL_COMMON_ReleseAntenaList(HxList_t *list);

extern HERROR	PL_COMMON_GetSatIpSignalLevel(HINT32 nIndex, HINT32 *pnLevel);
extern HERROR	PL_COMMON_GetSatIpSignalQuality(HINT32 nIndex, HINT32 *pnQuality);
extern HERROR	PL_COMMON_IsTunerLocked(HINT32 nIndex, HBOOL *pbLocked);

extern HERROR	PL_COMMON_GetCurrentServiceInfo( CO_CHANNEL_t **ppCoChannel, HUINT32 *ppulSvcUID);
extern HERROR	PL_COMMON_ReleaseCurrentServiceInfo( CO_CHANNEL_t *pCoChannel);

extern HERROR 	PL_COMMON_GetAntennaConnection(DxAntennaConnection_e *peConnectionType);

extern HERROR	PL_COMMON_GetTuningInfoChannelName(HUID	uUID, DxTuneParam_t	*pstTuningParam, HxSTR_t **ppChNameStr);

extern HERROR	PL_COMMON_SetChannelNotifier(ON_CHANNEL_CHANGED onChanged);



#endif  // _PL_CHANNEL_LIST_H_

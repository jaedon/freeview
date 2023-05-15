/**
	@file     pal_inet.h
	@brief

	Description: Applicatuion for searching Services\n
	Module: Application(Layer), Menu(module) \n
	Remarks : \n
	 Date			Modification				Name\n
	-----------	-------------------------------------	------------------\n
	 2012/09/26		initiated					mhpark3\n

	Copyright (c) 2009 HUMAX Co., Ltd.				\n
	All rights reserved.							\n
*/

#ifndef	_PAL_CM_H_
#define	_PAL_CM_H_

typedef struct {
	HULONG	ipAddr;
	HCHAR 	HWaddr[6];
	HULONG	netmask; 
	HULONG	gateway; 
} PAL_CM_Interface_t;

typedef enum
{
	PAL_CM_NOTIFY_OFFLINE = 0,
	PAL_CM_NOTIFY_ONLINE,	
	PAL_CM_NOTIFY_UNKNOWN,	
	PAL_CM_NOTIFY_MAX,	
} PAL_CM_Event_e;

/* interface */

typedef void (*CM_PostEvtToMwCallback_t)(HINT32 p1, HINT32 p2, HINT32 p3);

extern HERROR		PAL_CM_Init(void);
extern HERROR		PAL_CM_Deinit(void);
extern HERROR		PAL_CM_RegisterCallback(const CM_PostEvtToMwCallback_t pfnPostEvt);

extern HERROR		PAL_CM_GetLinkState( HBOOL *pbLinkOn );
extern HERROR		PAL_CM_GetINetInformation(PAL_CM_Interface_t *pIfInfo);

extern HERROR		PAL_CM_GetStatus( INET_CM_Status_t *peStatus );

extern HERROR		PAL_CM_GetDownStreamFreq( HINT32 *pnFreq );
extern HERROR		PAL_CM_SetDownStreamFreq( HUINT32 ulFreq );
extern HERROR		PAL_CM_GetDownStreamModType( INET_CM_DownMod_t *peMod );
extern HERROR		PAL_CM_GetDownStreamSNR( HINT32 *pnSnr );
extern HERROR		PAL_CM_GetDownStreamSignalLevel( HINT32 *pnLevel );
extern HERROR		PAL_CM_GetDownStreamBer( HINT32 *pnBer );

extern HERROR		PAL_CM_GetUpStreamChannelId( HINT32 *pnChId );
extern HERROR		PAL_CM_GetUptreamModType( INET_CM_DownMod_t *peMod );
extern HERROR		PAL_CM_GetUpStreamFreq( HINT32 *pnFreq );
extern HERROR		PAL_CM_GetUpStreamBandWidth( HINT32 *pnBwd );
extern HERROR		PAL_CM_GetUpStreamSignalLevel( HINT32 *pnLevel );
extern HERROR		PAL_CM_GetCMTemporature(int *pCmSavedTemp, int *pCmCurrentTemp);

extern HERROR		PAL_CM_GetDownStreamFreq30( HUINT32 ulChannelId, HINT32 *pnFreq );
extern HERROR		PAL_CM_GetDownStreamMod30Type( HUINT32 ulChannelId, INET_CM_DownMod_t *peMod );
extern HERROR		PAL_CM_GetDownStreamSNR30( HUINT32 ulChannelId, HINT32 *pnSnr );
extern HERROR		PAL_CM_GetDownStreamSignalLevel30( HUINT32 ulChannelId, HINT32 *pnLevel );
extern HERROR		PAL_CM_GetDownStreamUnCorrected30( HUINT32 ulChannelId, HINT32 *pnUnCorr );
extern HERROR		PAL_CM_GetDownStreamBer30( HUINT32 ulChannelId, HINT32 *pnBer );

extern HERROR		PAL_CM_GetUpStreamFreq30( HUINT32 ulChannelId, HINT32 *pnFreq );
extern HERROR		PAL_CM_GetUpStreamSignalLevel30( HUINT32 ulChannelId, HINT32 *pnLevel );
extern HERROR		PAL_CM_GetUpStreamSymbolRate30( HUINT32 ulChannelId, HINT32 *pnRate );
extern HERROR		PAL_CM_GetUpStreamMod30( HUINT32 ulChannelId, INET_CM_UpMod_t *peMod);
extern HERROR		PAL_CM_GetUpStreamBandWidth30( HUINT32 ulChannelId, HINT32 *pnBwd );
extern HERROR		PAL_CM_GetUpStreamChannelId30( HUINT32 ulChannelId, HINT32 *psRDCChannelId );
extern HERROR		PAL_CM_SetCmDefaults( void );


#endif	/* _PAL_CM_H_*/
/* end of file */


/**************************************************************************************/
/**
 * @file opltuner.h
 *
 * Tuner Interfaces
 *
 * @author  HUMAX
 * @date    2011/11/20
 * @attention   Copyright (c) 2011 Humax - All rights reserved.
 *
**************************************************************************************
**/

#ifndef __OPLBUILDER_H__
#define __OPLBUILDER_H__

#include "bpltype.h"

/******************************************************************
	Define
******************************************************************/
#define	MAX_PARAMS_LEN		64
#define	MAX_SYMBOLRATE_LEN	(MAX_PARAMS_LEN * 2)
#define	MAX_INFO_LEN		128
/******************************************************************
	Enum
 ******************************************************************/
typedef	enum
{
	OPL_TUNER_SAT,
	OPL_TUNER_CAB,
	OPL_TUNER_TER
} OPL_TUNERTYPE_e;

typedef	enum
{
	eBUILDER_STARTED = 0,
	eBUILDER_INDICATE_PROGRESS,
	eBUILDER_NEW_CHANNEL_FOUND,
	eBUILDER_NEW_TP_FOUND,
	eBUILDER_COMPLETE,
	eBUILDER_ABORTED
} OPL_BUILDER_Status_e;

/*
	It's good to use enum types with oplbuilder modulationModes.

	Prism Webapp uses like that
	    TCONSTELLATION : {
        EAUTO : 0,
        EQAM16 : 4,
        EQAM32 : 8,
        EQAM64 : 16,
        EQAM128 : 32,
        EQAM256 : 64,
    },
*/
typedef enum
{
	eOplModulationMode_Auto    = 0,
	eOplModulationMode_Qam16   = 4,
	eOplModulationMode_Qam32   = 8,
	eOplModulationMode_Qam64   = 16,
	eOplModulationMode_Qam128  = 32,
	eOplModulationMode_Qam256  = 64,
	eOplModulationMode_Qam1024 = 128,
	eOplModulationMode_Qam4096 = 256
} OPL_QAM_MODULATION_MODE_e;

/******************************************************************
	Typedef
******************************************************************/
typedef	struct
{
	HINT32		antennaId;
	HCHAR		antennaName[MAX_PARAMS_LEN];
	HINT32		startFreq;
	HINT32		endFreq;
	HINT32		modulationModes;
	HCHAR		symbolRate[MAX_SYMBOLRATE_LEN];
	HINT32		polarisation;
	HCHAR		codeRate[MAX_PARAMS_LEN];
	float		orbitalPosition;
	HINT32		networkId;
} DVBS_SCANPARAMS_t;

typedef	struct
{
	HINT32						startFreq;
	HINT32						endFreq;
	HINT32						raster;
	HBOOL						startNoetworkScanOnNIT;
	OPL_QAM_MODULATION_MODE_e	modulationModes;
	HCHAR						symbolRate[MAX_SYMBOLRATE_LEN];
	HINT32						networkId;
} DVBC_SCANPARAMS_t;

typedef	struct
{
	HINT32		startFreq;
	HINT32		endFreq;
	HCHAR		transmission[MAX_PARAMS_LEN];
	HINT32		raster;

	HCHAR		ofdm[MAX_PARAMS_LEN];
	HINT32		modulationModes;
	HCHAR		bandwidth[MAX_PARAMS_LEN];
} DVBT_SCANPARAMS_t;

typedef	struct
{
	HINT32		tunerType;
	union
	{
		DVBS_SCANPARAMS_t	stDVBSScanParams;
		DVBC_SCANPARAMS_t	stDVBCScanParams;
		DVBT_SCANPARAMS_t	stDVBTScanParams;
	} data;
} OPL_SCANPARAMS_t;

typedef	struct
{
	HINT32		channelType;
	HINT32		casType;
	HBOOL		isReplaceChannel;
	HBOOL		enableNetworkScan;
} OPL_SCANOPTIONS_t;

typedef	struct
{
	HINT32		onId;
	HINT32		tsId;
	HINT32		svcId;
	HINT32		channelType;
	HINT32		casType;
	HINT32		lcn;
	HCHAR		name[MAX_INFO_LEN];
} OPL_SEARCHEDINFO_t;

typedef	struct
{
	HINT32		type;
	HINT32		signalStrength, signalQuality;
	HUINT32		totalTpNum, completeTpNum, searchedSvcNum;
	HUINT32		frequency;

	HINT32				infoCount;
	OPL_SEARCHEDINFO_t	*searchedInfos;
	OPL_SCANPARAMS_t	*pstTunedParam;
} OPL_SCANEVENT_t;

typedef	struct
{
	HINT32		type;
	HINT32		progress;
	HUINT32		connType;
} OPL_CONNECTIONEVENT_t;

typedef	enum
{
	eBUILDEREVENT_SCAN,
	eBUILDEREVENT_CONNECTION
} OPL_SCANEVENT_e;

typedef	struct
{
	OPL_SCANEVENT_e		eventType;
	union
	{
		OPL_SCANEVENT_t			scanEvent;
		OPL_CONNECTIONEVENT_t	connectionEvent;
	} u;
} OPL_BUILDEREVENT_t;

typedef enum
{
	eOPLSCANTYPE_NORMAL,
	eOPLSCANTYPE_ANTENNACONNECTION,
	eOPLSCANTYPE_DISEQCDETECTION,
	eOPLSCANTYPE_FSatUser,

	eOPLSCANTYPE_MAX
} OPL_SCANTYPE_e;

#define	OPLSCANTYPE_NORMAL				"normal"
#define	OPLSCANTYPE_ANTENNACONNECTION	"antennaconnection"
#define	OPLSCANTYPE_DISEQCDETECTION		"diseqcdetection"
#define	OPLSCANTYPE_FSATUSERSCAN		"fsatuserscan"
#define	OPLSCANTYPE_TUNEONLY			"tuneonly"
#define	OPLSCANTYPE_FSATPOSTCODESKIP	"fsatpostcodeskip"
#define	OPLSCANTYPE_FSATTUNETWIN		"tunetwin"
#define	OPLSCANTYPE_SESLCNUPDATE		"seslcnupdate"
/*********************************************************************/
#define	OPLSCANTYPE_ERTSEARCH			"ertsearch"
#define	OPLSCANTYPE_OPTPLISTSEARCH		"optplistsearch"
/*********************************************************************/

#ifdef __cplusplus
extern "C" {
#endif

extern	BPLStatus	OPL_Builder_StartScan(HCHAR *scanType, OPL_SCANOPTIONS_t *pstOptions, HINT32 num, OPL_SCANPARAMS_t *pstParams);
extern	BPLStatus	OPL_Builder_StopScan(void);
extern	BPLStatus	OPL_Builder_ControlScan(HCHAR *controlScan);
extern	BPLStatus	OPL_Builder_RegisterListener(int (*cbFunc)(void *pstData));
extern	BPLStatus	OPL_Builder_UnRegisterListener(void);

extern	BPLStatus	OPL_Builder_Motor_GotoPosition(HINT32 position);
extern	BPLStatus	OPL_Builder_Motor_StorePosition(HINT32 position);
extern	BPLStatus	OPL_Builder_Motor_ResetPosition(void);  // MOON_motor
extern	BPLStatus	OPL_Builder_Motor_Recalculate(HINT32 position);
extern	BPLStatus	OPL_Builder_Motor_Move(HINT32 direction, HINT32 mode, HINT32 step);
extern	BPLStatus	OPL_Builder_Motor_Stop(void);
extern	BPLStatus	OPL_Builder_Motor_SetLimit(HINT32 direction);
extern	BPLStatus	OPL_Builder_Motor_EnableLimit(void);
extern	BPLStatus	OPL_Builder_Motor_DisableLimit(void);
extern	BPLStatus	OPL_Builder_Motor_GotoDegree(HINT32 degree);
extern	BPLStatus	OPL_Builder_Motor_GotoSatellite(HINT32 satellite);
extern	BPLStatus	OPL_Builder_Motor_SetMyLatitude(HINT32 mylatitude);
extern	BPLStatus	OPL_Builder_Motor_SetMyLongitude(HINT32 mylongitude);
extern	HINT32	OPL_Builder_Motor_GetMyLatitude(void);
extern	HINT32	OPL_Builder_Motor_GetMyLongitude(void);
extern	HINT32	OPL_Builder_SetSelectedConflictLcn(OPL_SEARCHEDINFO_t *searchedInfos, int count);

#ifdef __cplusplus
};
#endif

/******************************************************************
	Interfaces
******************************************************************/
#ifdef __cplusplus
extern "C" {
#endif	/* __cplusplus */

#ifdef __cplusplus
};

/******************************************************************
	class OplAVOutput
******************************************************************/
//typedef OplAVOutput_t	OplAVOutput;

#endif	/* __cplusplus */

#endif	/* __OPLAVOUTPUT_H__ */





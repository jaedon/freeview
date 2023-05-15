/**************************************************************************************/
/**
 * @file oplantinfo.h
 *
 * Defines the favourite structure.
 *
 * @attention   Copyright (c) 2011 Humax - All rights reserved.
 *
 **************************************************************************************
 **/

#ifndef __OPLANTINFO_H__
#define __OPLANTINFO_H__

#ifdef __cplusplus
	extern "C" {
#endif
#include	"htype.h"

typedef	enum
{
	eAntType_LNB = 0,
	eAntType_Diseqc,
	eAntType_SCD,
	eAntType_SMATV,
	eAntType_MonoBlock
}	OPL_AntennaType_e;

#define	OPL_ANTINFO_NAME_LEN		24
typedef	struct
{
	OPL_AntennaType_e	eAntennaType;
	char			satelliteType[OPL_ANTINFO_NAME_LEN];
	char			satelliteName[OPL_ANTINFO_NAME_LEN];

	int				lnb_freq;
	int				lnb_voltage;		//	0 : standard, 1 : high

	int				diseqc_version;		//	0, 1, 2, 3, 4 (1.0, 1.1, 1.2, 1.3, 2.0)
	int				switch_input;		//	diseqc 1.0, 1.1 : 0 : none, 1 : a, 2 : b, 3 : c, 4 : d
										//	scd             : 0 : none, 1 : a, 2 : b
	HBOOL			enable_22khztone;
	int				tone_burst;			//	0 : none, 1 : tone_burst_a, 2 : tone_burst_b
	int				position;			//	diseqc 1.2 motor position
	int				longitude;			//	diseqc 1.3 satellite longitude

	int				scd_band_num[4];	//	basically 2 tuner model. MOON_SCD
	int				scd_band_freq[4];
} OPL_AntennaInfo_t;

extern	HUINT32		OPL_ANTINFO_Count(void);
extern	HINT32		OPL_ANTINFO_GetId(HINT32 nIndex);
extern	HINT32		OPL_ANTINFO_New(void);
extern	HERROR		OPL_ANTINFO_RemoveAll(void);
extern	HINT32		OPL_ANTINFO_GetConnectionType(void);
extern	HERROR		OPL_ANTINFO_SetConnectionType(HINT32 n);
extern	HERROR		OPL_ANTINFO_GetInfo(HINT32 nIndex, OPL_AntennaInfo_t *pstAntInfo);
extern	HERROR		OPL_ANTINFO_SetInfo(HINT32 nIndex, OPL_AntennaInfo_t *pstAntInfo);
extern	HERROR		OPL_ANTINFO_Sync(void);
extern	HERROR		OPL_ANTINFO_Refresh(void);

#ifdef __cplusplus
	};
#endif

#endif	// __OPLANTINFO_H__


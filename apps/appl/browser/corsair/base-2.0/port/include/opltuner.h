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

#ifndef __OPLTUNER_H__
#define __OPLTUNER_H__

#include "bpltype.h"

/******************************************************************
	Define
******************************************************************/

/******************************************************************
	Enum
 ******************************************************************/

/******************************************************************
	Typedef
******************************************************************/

#ifdef __cplusplus
extern "C" {
#endif

BPLStatus	OPL_Tuner_GetNumOfDev(unsigned int *pnNumOfDev);
BPLStatus	OPL_Tuner_GetTunerId(unsigned int ulDevIdx, int *pnTunerId);
BPLStatus	OPL_Tuner_GetPowerState(unsigned int ulDevIdx, BPL_BOOL *pbPowerState);
BPLStatus	OPL_Tuner_GetIdTypes(unsigned int ulDevIdx, int *count, char ***pszIdTypes);
BPLStatus	OPL_Tuner_GetTunerIdByTripleId(int onId, int tsId, int svcId, int *pnTunerId);

#ifdef __cplusplus
};

class	OplTuner {
	private:
		unsigned int		ulDevIdx;

	public:
		OplTuner(unsigned int ulDevIdx)
		{
			ulDevIdx = ulDevIdx;
		}
		virtual	~OplTuner()
		{
		}
		static	bool	getNumOfDev(unsigned int *pnNumOfDev)
		{
			return (OPL_Tuner_GetNumOfDev(pnNumOfDev) == BPL_STATUS_OK);
		}

		static	bool	getTunerId(unsigned int ulDevIdx, int *pnTunerId)
		{	return (OPL_Tuner_GetTunerId(ulDevIdx, pnTunerId) == BPL_STATUS_OK);	}

		static	bool	isTunerEnabled(unsigned int ulDevIdx)
		{	return	true;			}

		static	bool	isExternalPower(unsigned int ulDevIdx)
		{
			BPL_BOOL		power_state;

			OPL_Tuner_GetPowerState(ulDevIdx, &power_state);

			return power_state;
		}

		static	bool	getIdTypes(unsigned int ulDevIdx, int *count, char ***pszIdTypes)
		{	return (OPL_Tuner_GetIdTypes(ulDevIdx, count, pszIdTypes) == BPL_STATUS_OK);	}

		static	bool getTunerIdByTripleId(int onId, int tsId, int svcId, int *pnTunerId)
		{	return (OPL_Tuner_GetTunerIdByTripleId(onId, tsId, svcId, pnTunerId) == BPL_STATUS_OK);}

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





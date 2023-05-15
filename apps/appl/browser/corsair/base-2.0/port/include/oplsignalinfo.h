/**************************************************************************************/
/**
 * @file oplsignalinfo.h
 *
 * SignalInfo Interfaces
 *
 * @author  HUMAX
 * @date    2011/11/20
 * @attention   Copyright (c) 2011 Humax - All rights reserved.
 *
**************************************************************************************
**/

#ifndef __OPLSIGNALINFO_H__
#define __OPLSIGNALINFO_H__

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

BPLStatus	OPL_SignalInfo_GetStrength(unsigned int ulDevIdx, int *pnStrength);
BPLStatus	OPL_SignalInfo_GetQuality (unsigned int ulDevIdx, int *pnQuality);
BPLStatus	OPL_SignalInfo_GetBer(unsigned int ulDevIdx, float *pfBer);
BPLStatus	OPL_SignalInfo_GetSnr(unsigned int ulDevIdx, float *pfSnr);
BPLStatus	OPL_SignalInfo_GetAgc(unsigned int ulDevIdx, unsigned int *pulAgc);
BPLStatus	OPL_SignalInfo_GetLocked(unsigned int ulDevIdx, BPL_BOOL *pbLocked);
BPLStatus	OPL_SignalInfo_GetTunedParamStr(unsigned int ulDevIdx, char *pszTuneParam, int maxStrLen);


#ifdef __cplusplus
};

class	OplSignalInfo {
	private:
		unsigned int		ulDevIdx;

	public:
		OplSignalInfo(unsigned int ulDevIdx)
		{
			this->ulDevIdx = ulDevIdx;
		}
		virtual	~OplSignalInfo()
		{
		}
		static	bool	getSignalInfoStrength(unsigned int ulDevIdx, int *pnStrength)
		{	return (OPL_SignalInfo_GetStrength(ulDevIdx, pnStrength) == BPL_STATUS_OK);}
		static	bool	getSignalInfoQuality(unsigned int ulDevIdx, int *pnQuality)
		{	return (OPL_SignalInfo_GetQuality(ulDevIdx, pnQuality) == BPL_STATUS_OK);}
		static	bool	getSignalInfoBer(unsigned int ulDevIdx, float *pfBer)
		{	return (OPL_SignalInfo_GetBer(ulDevIdx, pfBer) == BPL_STATUS_OK);}
		static	bool	getSignalInfoSnr(unsigned int ulDevIdx, float *pfSnr)
		{	return (OPL_SignalInfo_GetSnr(ulDevIdx, pfSnr) == BPL_STATUS_OK);}
		static	bool getSignalInfoAgc(unsigned int ulDevIdx, unsigned int *pulAgc)
		{	return (OPL_SignalInfo_GetAgc(ulDevIdx, pulAgc) == BPL_STATUS_OK);}
		static	bool	getSignalInfoLocked(unsigned int ulDevIdx, bool *pbLocked)
		{
			BPL_BOOL	flag;

			if (OPL_SignalInfo_GetLocked(ulDevIdx, &flag) != BPL_STATUS_OK)
				return false;

			*pbLocked = flag;

			return true;
		}
		static bool getTunedParamStr(unsigned int ulDevIdx, char *pszTuneParam, int maxStrLen)
		{ return OPL_SignalInfo_GetTunedParamStr(ulDevIdx, pszTuneParam, maxStrLen) == BPL_STATUS_OK; }
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





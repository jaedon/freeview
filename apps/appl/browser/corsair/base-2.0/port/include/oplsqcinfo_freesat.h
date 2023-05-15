/**************************************************************************************/
/**
 * @file oplsqcinfo_freesat.h
 *
 * System information
 *
 * @author
 * @date
 * @attention   Copyright (c) 2011 Humax - All rights reserved.
 *
 **************************************************************************************
 **/

#ifndef __OPLSQCINFO_FREESAT
#define __OPLSQCINFO_FREESAT

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
typedef void OplSqcInfo_Freesat_t;

/******************************************************************
	Interfaces
******************************************************************/
#ifdef __cplusplus
extern "C" {
#endif
#define POSTCODE_LEN_MAX 64
#define BOUQUETNAME_LEN_MAX 64

OplSqcInfo_Freesat_t *OPL_SqcInfo_Freesat_Create(void);

void OPL_SqcInfo_Freesat_Destroy(OplSqcInfo_Freesat_t *hSqcInfo_Freesat);

#ifdef CONFIG_OP_FREESAT
BPLStatus OPL_SqcInfo_Freesat_GetBouquetName(OplSqcInfo_Freesat_t *hSqcInfo_Freesat, char *pszBouqName,unsigned int ulLen);
BPLStatus OPL_SqcInfo_Freesat_SetBouquetName(OplSqcInfo_Freesat_t *hSqcInfo_Freesat, const char *pszBouqName);

BPLStatus OPL_SqcInfo_Freesat_SetPostcode(OplSqcInfo_Freesat_t *hSqcInfo_Freesat, const char *pszPostcode);
BPLStatus OPL_SqcInfo_Freesat_GetPostcode(OplSqcInfo_Freesat_t *hSqcInfo_Freesat, char *pszPostcode, unsigned int ulLen);

BPLStatus OPL_SqcInfo_Freesat_SetSiMode(OplSqcInfo_Freesat_t *hSqcInfo_Freesat, int siMode);
BPLStatus OPL_SqcInfo_Freesat_GetSiMode(OplSqcInfo_Freesat_t *hSqcInfo_Freesat, int *siMode);

BPLStatus OPL_SqcInfo_Freesat_GetBouquetId(OplSqcInfo_Freesat_t *hSqcInfo_Freesat, int *bouquetId);
BPLStatus OPL_SqcInfo_Freesat_GetRegionCode(OplSqcInfo_Freesat_t *hSqcInfo_Freesat, int *regionCode);

BPLStatus OPL_SqcInfo_Freesat_SetAdultChannels(OplSqcInfo_Freesat_t *hSqcInfo_Freesat, BPL_BOOL adultChannels);
BPLStatus OPL_SqcInfo_Freesat_GetAdultChannels(OplSqcInfo_Freesat_t *hSqcInfo_Freesat, BPL_BOOL *adultChannels);

BPLStatus OPL_SqcInfo_Freesat_SetDeepLinkForITV(OplSqcInfo_Freesat_t *hSqcInfo_Freesat, const char *url, int lcn);
BPLStatus OPL_SqcInfo_Freesat_SetMHEGOperationControl(OplSqcInfo_Freesat_t * hSqcInfo_Freesat, BPL_BOOL bControl);
#else
BPLStatus OPL_SqcInfo_Freesat_GetBouquetName(OplSqcInfo_Freesat_t *hSqcInfo_Freesat, char *pszBouqName,unsigned int ulLen) { return BPL_STATUS_OK; }
BPLStatus OPL_SqcInfo_Freesat_SetBouquetName(OplSqcInfo_Freesat_t *hSqcInfo_Freesat, const char *pszBouqName) { return BPL_STATUS_OK; }

BPLStatus OPL_SqcInfo_Freesat_SetPostcode(OplSqcInfo_Freesat_t *hSqcInfo_Freesat, const char *pszPostcode) { return BPL_STATUS_OK; }
BPLStatus OPL_SqcInfo_Freesat_GetPostcode(OplSqcInfo_Freesat_t *hSqcInfo_Freesat, char *pszPostcode, unsigned int ulLen) { return BPL_STATUS_OK; }

BPLStatus OPL_SqcInfo_Freesat_SetSiMode(OplSqcInfo_Freesat_t *hSqcInfo_Freesat, int siMode) { return BPL_STATUS_OK; }
BPLStatus OPL_SqcInfo_Freesat_GetSiMode(OplSqcInfo_Freesat_t *hSqcInfo_Freesat, int *siMode) { return BPL_STATUS_OK; }

BPLStatus OPL_SqcInfo_Freesat_GetBouquetId(OplSqcInfo_Freesat_t *hSqcInfo_Freesat, int *bouquetId) { return BPL_STATUS_OK; }
BPLStatus OPL_SqcInfo_Freesat_GetRegionCode(OplSqcInfo_Freesat_t *hSqcInfo_Freesat, int *regionCode) { return BPL_STATUS_OK; }

BPLStatus OPL_SqcInfo_Freesat_SetAdultChannels(OplSqcInfo_Freesat_t *hSqcInfo_Freesat, BPL_BOOL adultChannels) { return BPL_STATUS_OK; }
BPLStatus OPL_SqcInfo_Freesat_GetAdultChannels(OplSqcInfo_Freesat_t *hSqcInfo_Freesat, BPL_BOOL *adultChannels) { return BPL_STATUS_OK; }

BPLStatus OPL_SqcInfo_Freesat_SetDeepLinkForITV(OplSqcInfo_Freesat_t *hSqcInfo_Freesat, const char *url, int lcn) { return BPL_STATUS_OK; }
BPLStatus OPL_SqcInfo_Freesat_SetMHEGOperationControl(OplSqcInfo_Freesat_t * hSqcInfo_Freesat, BPL_BOOL bControl) { return BPL_STATUS_OK; }
#endif //CONFIG_OP_FREESAT

#ifdef __cplusplus
};

/******************************************************************
	class OplSqcInfo_Freesat
******************************************************************/
class OplSqcInfo_Freesat {

private:
	static OplSqcInfo_Freesat_t *m_sqcinfoHandle;


public:
	OplSqcInfo_Freesat()
	{
	}
	virtual ~OplSqcInfo_Freesat()
	{
	}


	static bool getBouquetName(char *pszName, unsigned int ulLen)
	{ return OPL_SqcInfo_Freesat_GetBouquetName(NULL, pszName,ulLen) == BPL_STATUS_OK; }
	static bool setBouquetName(const char *pszName)
	{ return OPL_SqcInfo_Freesat_SetBouquetName(NULL, pszName) == BPL_STATUS_OK; }

	static bool getPostcode(char *pszPostcode, unsigned int ulLen)
	{ return OPL_SqcInfo_Freesat_GetPostcode(NULL, pszPostcode, ulLen) == BPL_STATUS_OK; }
	static bool setPostcode(const char *pszPostcode)
	{ return OPL_SqcInfo_Freesat_SetPostcode(NULL, pszPostcode) == BPL_STATUS_OK; }
	static bool getSiMode(int *siMode)
	{ return OPL_SqcInfo_Freesat_GetSiMode(NULL, siMode) == BPL_STATUS_OK; }
	static bool setSiMode(int siMode)
	{ return OPL_SqcInfo_Freesat_SetSiMode(NULL, siMode) == BPL_STATUS_OK; }
	static bool getBouquetId(int *bouquetId)
	{ return OPL_SqcInfo_Freesat_GetBouquetId(NULL, bouquetId) == BPL_STATUS_OK; }
	static bool getRegionCode(int *regionCode)
	{ return OPL_SqcInfo_Freesat_GetRegionCode(NULL, regionCode) == BPL_STATUS_OK; }
	static bool getAdultChannels(BPL_BOOL *badultChannels)
	{ return OPL_SqcInfo_Freesat_GetAdultChannels(NULL, badultChannels) == BPL_STATUS_OK; }
	static bool setAdultChannels(BPL_BOOL badultChannels)
	{ return OPL_SqcInfo_Freesat_SetAdultChannels(NULL, badultChannels) == BPL_STATUS_OK; }
	static bool setMHEGOperationControl(BPL_BOOL bControl)
	{ return OPL_SqcInfo_Freesat_SetMHEGOperationControl(NULL, bControl) == BPL_STATUS_OK; }
};

#endif

#endif




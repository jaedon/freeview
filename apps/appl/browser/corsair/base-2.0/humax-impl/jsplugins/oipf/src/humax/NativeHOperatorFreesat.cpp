/**************************************************************************************/
/**
 * @file NativeConfiguration.cpp
 *
 * Configuration module
 *
 * @author  Jongwon Kim(jwkim@humaxdigital.com), Inseon Son(isson@humaxdigital.com)
 * @date    2011/04/07
 * @attention   Copyright (c) 2011 Humax - All rights reserved.
 *
 **************************************************************************************
 **/
#ifdef	CONFIG_OP_FREESAT

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <hlib.h>
#include "NativeHOperatorFreesat.h"
#include "macros.h"


#include "oplsqcinfo_freesat.h"


#ifdef OIPF


OOIFReturnCode native_HGetPostCode(const char **retval)
{
	static char szPostcode[POSTCODE_LEN_MAX];
	OplSqcInfo_Freesat::getPostcode(szPostcode,POSTCODE_LEN_MAX-1);

	*retval = szPostcode;

	return OOIF_RETURN_OK;
}
OOIFReturnCode native_HSetPostCode(const char *retval)
{
	OplSqcInfo_Freesat::setPostcode(retval);

	return OOIF_RETURN_OK;
}

OOIFReturnCode native_HGetBouquetName(const char **retval)
{
	static char szBouquetName[BOUQUETNAME_LEN_MAX];
	OplSqcInfo_Freesat::getBouquetName(szBouquetName,BOUQUETNAME_LEN_MAX-1);

	*retval = szBouquetName;

	return OOIF_RETURN_OK;
}

OOIFReturnCode native_HSetBouquetName(const char *retval)
{
	OplSqcInfo_Freesat::setBouquetName(retval);

	return OOIF_RETURN_OK;
}

OOIFReturnCode native_HGetSiMode(OOIFNumber *retval)
{
	int siMode;

	OplSqcInfo_Freesat::getSiMode(&siMode);

	*retval = siMode;
#if 0
{
	OOIFNumber tmp;
	native_HGetBouquetId(&tmp);
	OOIF_LOG_PRINT(">>>>>>>>>>>>>>   native_HGetBouquetId = %d\n", (int)tmp);

	native_HGetRegionCode(&tmp);
	OOIF_LOG_PRINT(">>>>>>>>>>>>>>   native_HGetRegionCode = %d\n", (int)tmp);

}
#endif
	return OOIF_RETURN_OK;
}

OOIFReturnCode native_HSetSiMode(OOIFNumber retval)
{
	OplSqcInfo_Freesat::setSiMode(retval);

	return OOIF_RETURN_OK;
}

OOIFReturnCode native_HGetBouquetId(OOIFNumber *retval)
{
	int bouquetId;

	OplSqcInfo_Freesat::getBouquetId(&bouquetId);

	*retval = bouquetId;


	return OOIF_RETURN_OK;
}

OOIFReturnCode native_HGetRegionCode(OOIFNumber *retval)
{
	int regionCode;

	OplSqcInfo_Freesat::getRegionCode(&regionCode);

	*retval = regionCode;

	return OOIF_RETURN_OK;
}

OOIFReturnCode native_HGetAdultChannels(OOIFBoolean *retval)
{
	BPL_BOOL badultChannels;

	if (NULL == retval)
	{
		return OOIF_RETURN_VALUE_UNDEFINED;
	}

	OplSqcInfo_Freesat::getAdultChannels(&badultChannels);

	*retval = badultChannels;

	return OOIF_RETURN_OK;
}

OOIFReturnCode native_HSetAdultChannels(OOIFBoolean retval)
{
	OplSqcInfo_Freesat::setAdultChannels((BPL_BOOL)retval);

	return OOIF_RETURN_OK;
}

OOIFReturnCode native_HSetMHEGOperationControl(OOIFBoolean retval)
{
	OplSqcInfo_Freesat::setMHEGOperationControl((BPL_BOOL)retval);

	return OOIF_RETURN_OK;
}

#endif // #ifdef OIPF
#endif


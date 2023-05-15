// -*- Mode: c++; tab-width: 4; indent-tabs-mode: t; c-basic-offset: 4 -*-
/*
 * Copyright (C) 1995-2013 Opera Software ASA. All rights reserved.
 *
 * It is prohibited for you to distribute this file to anybody without Operas
 * separate written consent.
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "NativeParentalControlManager.h"

#include "macros.h"

#ifdef HMX_WEBUI
#include "oplpcmanager.h"
#include "oplnagrapcmanager.h"
static	OPL_PCMAN_t		*s_pcManager = NULL;
#endif

OOIFReturnCode native_ParentalControlManagerOnCreate(long int window, const char *host)
{
	if (s_pcManager)
		return OOIF_RETURN_OK;

	s_pcManager = OPL_PCMAN_CreateHandle();

#if (defined(JLABS_JCOM) || defined(HMX_WEBUI))
/*
 * This function originate from JLABS, But HUMAX WEB UI will be use, also.
 * and more, below condition is also same.
 *
 * jlabs에서는 addParentalRating을 사용하지 않는다. 따라서 default로 dvb-si가 등록 되어야 함.
 */
	OPL_PCMAN_AddParentalRatingScheme(s_pcManager, OPL_PARENTAL_SCHEM_NAME_DVB_SI, NULL);
 #endif
	return OOIF_RETURN_OK;
}

OOIFReturnCode native_ParentalControlGetParentalRatingSchemes(int *count,
        ParentalRatingSchemeHandle **ids)
{
	int	num;

	if (s_pcManager == NULL)
		return OOIF_RETURN_ERROR;

	num = OPL_PCMAN_GetParentalRatingSchemesCount(s_pcManager);
	*count = num;
    *ids = (ParentalRatingSchemeHandle*)HLIB_STD_MemAlloc(sizeof(ParentalRatingSchemeHandle)*num);
    for (int handleIndex = 0; handleIndex < num; ++handleIndex)
    {
        (*ids)[handleIndex] = OPL_PCMAN_GetParentalRatingSchemeByIndex(s_pcManager, handleIndex);
    }

	return OOIF_RETURN_OK;
}

OOIFReturnCode native_ParentalRatingSchemeReleaseHandle(ParentalRatingSchemeHandle id)
{
	return OOIF_RETURN_OK;
}

#ifdef OIPF
/** 7.9.1.2 */
OOIFReturnCode native_ParentalControlGetIsPINEntryLocked(OOIFBoolean *retval)
{
	if (s_pcManager == NULL)
		return OOIF_RETURN_ERROR;

	*retval = OPL_PCMAN_IsEntryLocked(s_pcManager);

    return OOIF_RETURN_OK;
}

OOIFReturnCode native_ParentalControlSetParentalControlStatus(const char *pcPIN, OOIFBoolean enable,
        OOIFNumber *retval)
{
	if (s_pcManager == NULL)
		return OOIF_RETURN_ERROR;
#if defined(JLABS)
	/*jlabs 023 그림B-7에 setParentalControlStatus의 2번째 인수는 항상 false만 줄 수 있다고 명시 되어 있음*/
	if(enable == TRUE)
	{
		return OOIF_RETURN_ERROR;
	}
#endif
	*retval = OPL_PCMAN_SetParentalControlStatus(s_pcManager, pcPIN, (enable == TRUE) ? OPL_TRUE : OPL_FALSE);

	return OOIF_RETURN_OK;
}

OOIFReturnCode native_ParentalControlGetParentalControlStatus(OOIFBoolean *retval)
{
	if (s_pcManager == NULL)
		return OOIF_RETURN_ERROR;

	*retval = OPL_PCMAN_GetParentalControlStatus(s_pcManager);

	return OOIF_RETURN_OK;
}

OOIFReturnCode native_ParentalControlGetBlockUnrated(OOIFBoolean *retval)
{
	if (s_pcManager == NULL)
		return OOIF_RETURN_ERROR;

	*retval = OPL_PCMAN_GetBlockUnrated(s_pcManager);

	return OOIF_RETURN_OK;
}

OOIFReturnCode native_ParentalControlSetParentalControlPIN(const char *oldPcPIN, const char *newPcPIN, OOIFNumber *retval)
{
	if (s_pcManager == NULL)
		return OOIF_RETURN_ERROR;

	*retval = OPL_PCMAN_SetParentalControlPIN(s_pcManager, oldPcPIN, newPcPIN);

    return OOIF_RETURN_OK;
}

#ifdef HMX_WEBUI
OOIFReturnCode native_ParentalControlUnlockWithParentalControlPIN(const char *pcPIN, int obj_type, void *target, OOIFNumber *retval)
#else
OOIFReturnCode native_ParentalControlUnlockWithParentalControlPIN(const char *pcPIN, void *target, OOIFNumber *retval)
#endif
{
	if (0 == obj_type) { // CHANNEL

		if (s_pcManager == NULL)
			return OOIF_RETURN_ERROR;

		*retval = OPL_PCMAN_UnlockChannelWithParentalControlPIN(s_pcManager, pcPIN, target);
//		*retval = OPL_PCMAN_UnlockAVObjectWithParentalControlPIN(s_pcManager, pcPIN, target);

		return OOIF_RETURN_OK;
	} else if (1 == obj_type) { // VIDEO ON DEMAND
		// TODO: we have to add!!!!!!
	} else if (2 == obj_type) { // VIDEO BROADCAST
		if (s_pcManager == NULL)
			return OOIF_RETURN_ERROR;

		*retval = OPL_PCMAN_UnlockVBObjectWithParentalControlPIN(s_pcManager, pcPIN, target);

    	return OOIF_RETURN_OK;
	} else {
		// This case is not support!!!
	}

	return OOIF_RETURN_ERROR;
}

OOIFReturnCode native_ParentalControlVerifyParentalControlPIN(const char* pcPIN, OOIFNumber *retval)
{
	if (s_pcManager == NULL)
		return OOIF_RETURN_ERROR;

	*retval = OPL_PCMAN_VerifyParentalControlPIN(s_pcManager, pcPIN);

    return OOIF_RETURN_OK;
}

OOIFReturnCode native_ParentalControlSetBlockUnrated(const char *pcPIN, OOIFBoolean block, OOIFNumber *retval)
{
	if (s_pcManager == NULL)
		return OOIF_RETURN_ERROR;

	*retval = OPL_PCMAN_SetBlockUnrated(s_pcManager, pcPIN, (block == TRUE) ? OPL_TRUE : OPL_FALSE);

    return OOIF_RETURN_OK;
}

#endif // OIPF

int rating_size = 99;

OOIFReturnCode native_ParentalRatingSchemeGetRatings(ParentalRatingSchemeHandle id, int *count, const char ***handles)
{
	const char** ratings = (const char**)HLIB_STD_MemAlloc(sizeof(const char*)*rating_size);
	for (int i=0; i < rating_size; ++i)
	{
		ratings[i] = HLIB_STD_StrDup("Dummy Scheme Item Name");
	}
	*count = rating_size;
	*handles = ratings;
	return OOIF_RETURN_OK;
}

OOIFReturnCode native_ParentalRatingSchemeGetName(ParentalRatingSchemeHandle id, const char **retval)
{
	if (s_pcManager == NULL)
		return OOIF_RETURN_ERROR;

	*retval = OPL_PCMAN_SCHEME_GetName((OPL_ParentalRatingScheme_t*)id);

	return OOIF_RETURN_OK;
}

OOIFReturnCode native_ParentalRatingSchemeGetLength(ParentalRatingSchemeHandle id, OOIFNumber *retval)
{
	if (s_pcManager == NULL)
		return OOIF_RETURN_ERROR;

	*retval = OPL_PCMAN_SCHEME_Length((OPL_ParentalRatingScheme_t*)id);

	return OOIF_RETURN_OK;
}

OOIFReturnCode native_ParentalRatingSchemeGetThreshold(ParentalRatingSchemeHandle id, ParentalRatingHandle *retval)
{
	if (s_pcManager == NULL)
		return OOIF_RETURN_ERROR;

	*retval = (ParentalRatingHandle)OPL_PCMAN_SCHEME_GetThreshold((OPL_ParentalRatingScheme_t*)id);

	return OOIF_RETURN_OK;
}

OOIFReturnCode native_ParentalRatingSchemeIndexOf(ParentalRatingSchemeHandle id,
        const char *ratingValue, OOIFNumber *retval)
{
	if (s_pcManager == NULL)
		return OOIF_RETURN_ERROR;

	*retval = OPL_PCMAN_SCHEME_IndexOf((OPL_ParentalRatingScheme_t*)id, ratingValue);

	return OOIF_RETURN_OK;
}

OOIFReturnCode native_ParentalRatingSchemeItem(ParentalRatingSchemeHandle id, OOIFNumber index,
        const char **retval)
{
	//	ParentalRating이 Collection도 아닌데 왠 아이템???
	return OOIF_RETURN_ERROR;
}

OOIFReturnCode native_ParentalRatingSchemeIconUri(ParentalRatingSchemeHandle id, OOIFNumber index, const char **retval)
{
	//	iconUri개념 아직 없음..

	return OOIF_RETURN_VALUE_NULL;
}

#ifdef OIPF
OOIFReturnCode native_PRSCollectionAddParentalRatingScheme(const char *name, const char *values, ParentalRatingSchemeHandle *retval)
{
	if (s_pcManager == NULL)
		return OOIF_RETURN_ERROR;

	*retval = (ParentalRatingSchemeHandle)OPL_PCMAN_AddParentalRatingScheme(s_pcManager, name, values);

	return OOIF_RETURN_OK;
}
#endif /* OIPF */

OOIFReturnCode native_ParentalRatingGetName(ParentalRatingHandle id, const char **retval)
{
	if (s_pcManager == NULL)
		return OOIF_RETURN_ERROR;

	*retval = OPL_PCMAN_PR_GetName((OPL_ParentalRating_t*)id);

	return OOIF_RETURN_OK;
}

OOIFReturnCode native_ParentalRatingGetScheme(ParentalRatingHandle id, const char **retval)
{
	if (s_pcManager == NULL)
		return OOIF_RETURN_ERROR;

	*retval = OPL_PCMAN_PR_GetScheme((OPL_ParentalRating_t*)id);

	return OOIF_RETURN_OK;
}

OOIFReturnCode native_ParentalRatingGetValue(ParentalRatingHandle id, OOIFNumber *retval)
{
	if (s_pcManager == NULL)
		return OOIF_RETURN_ERROR;

	*retval = OPL_PCMAN_PR_GetValue((OPL_ParentalRating_t*)id);

	return OOIF_RETURN_OK;
}

OOIFReturnCode native_ParentalRatingGetLabels(ParentalRatingHandle id, OOIFNumber *retval)
{
	if (s_pcManager == NULL)
		return OOIF_RETURN_ERROR;

	*retval = OPL_PCMAN_PR_GetLabels((OPL_ParentalRating_t*)id);

	return OOIF_RETURN_OK;
}

OOIFReturnCode native_ParentalRatingGetRegion(ParentalRatingHandle id, const char **retval)
{
	if (s_pcManager == NULL)
		return OOIF_RETURN_ERROR;

	*retval = OPL_PCMAN_PR_GetRegion((OPL_ParentalRating_t*)id);

	return OOIF_RETURN_OK;
}

OOIFReturnCode native_PRCollectionAddParentalRating(void *id, const char *scheme,
        const char *name, OOIFBoolean has_value,
        OOIFNumber value, OOIFNumber labels,
        const char *region,
        long int window_identifier, const char *host,
        ParentalRatingHandle *retval)
{
	*retval = (void*)OPL_PCMAN_PRC_AddParentalRating(id, scheme, name, (int)value, (int)labels, region);

	return OOIF_RETURN_OK;
}

OOIFReturnCode native_ParentalRatingReleaseHandle(ParentalRatingHandle id)
{
	OPL_PCMAN_PRC_RemoveParentalRating((OPL_ParentalRating_t*)id);

	return OOIF_RETURN_OK;
}

#if (defined(JLABS_JCOM) || defined(HMX_WEBUI))
/*
 * This function originate from JLABS, But HUMAX WEB UI will be use, also.
 */
OOIFReturnCode native_ParentalRatingSchemeChangeThreshold(ParentalRatingSchemeHandle id, unsigned int ratingValue, OOIFNumber *retval)
{
	if (s_pcManager == NULL)
		return OOIF_RETURN_ERROR;

	*retval = OPL_PCMAN_SCHEME_ChangeThreshold((OPL_ParentalRatingScheme_t*)id, ratingValue);
    	return OOIF_RETURN_OK;
}
#endif

#if defined(HMX_WEBUI)
OOIFReturnCode native_ParentalControlResetParentalControlPIN(const char *newPcPIN, OOIFNumber *retval)
{
	if (s_pcManager == NULL)
		return OOIF_RETURN_ERROR;

	*retval = OPL_PCMAN_ResetParentalControlPIN(s_pcManager, newPcPIN);

    return OOIF_RETURN_OK;
}

OOIFReturnCode native_ParentalControlIsNagraSmartCardInserted(OOIFBoolean *retval)
{
	*retval = OPL_NAGRA_PCMAN_IsSmartCardInserted();
	return OOIF_RETURN_OK;
}

OOIFReturnCode native_ParentalControlSetNargaSmartCardPIN(const char *oldPIN, const char *newPIN, OOIFBoolean *retval)
{
	*retval = OPL_NAGRA_PCMAN_SetSmartCardPIN(oldPIN, newPIN);
	return OOIF_RETURN_OK;
}

OOIFReturnCode native_ParentalControlGetNargaSmartCardPINAccessibleState(OOIFNumber *retval)
{
	int accessibleState;

	OPL_NAGRA_PCMAN_GetSmartCardPINAccessibleState(&accessibleState);
	*retval = accessibleState;

	return OOIF_RETURN_OK;
}

OOIFReturnCode native_ParentalControlGetNargaSmartCardPINRetryCount(OOIFNumber *retval)
{
	int retryCnt;

	OPL_NAGRA_PCMAN_GetSmartCardPINRetryCount(&retryCnt);
	*retval = retryCnt;

	return OOIF_RETURN_OK;
}

OOIFReturnCode native_ParentalControlGetNargaSmartCardPINRemainTime(OOIFNumber *retval)
{
	int remainTime;

	OPL_NAGRA_PCMAN_GetSmartCardPINRemainTime(&remainTime);
	*retval = remainTime;

	return OOIF_RETURN_OK;
}

OOIFReturnCode native_ParentalControlVerifyNargaSmartCardPIN(const char *inputPIN, OOIFNumber *retval)
{
	int retryCnt;

	OPL_NAGRA_PCMAN_VerifySmartCardPIN(inputPIN, &retryCnt);
	*retval = retryCnt;

	return OOIF_RETURN_OK;
}

OOIFReturnCode native_ParentalControlUnlockAV(OOIFNumber *retval)
{
	*retval = OPL_NAGRA_PCMAN_UnlockAV();
	return OOIF_RETURN_OK;
}

#endif

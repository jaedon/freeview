/**************************************************************************************/
/**
 * @file oplpcmanager.c
 *
 * Defines the Parental Control Manager structure.
 *
 * @attention   Copyright (c) 2011 Humax - All rights reserved.
 *
 **************************************************************************************
 **/

#include	"oplpcmanager.h"

#include	"hlib.h"
#ifdef	CONFIG_OP_FREESAT
#include <appkit_freesat.h>
#endif
#include	"apk.h"



/**************************************************************
	internal structures and variables
 **************************************************************/
#define	_PARENTAL_SCHEME_NAME_LEN			1024
#define	_PARENTAL_NAME_LEN					32
#define	_PARENTAL_SCHEME_VALUE_LEN			32
#define	_PARENTAL_SCHEME_VALUE_NUM			32
#define	_PARENTAL_REGION_NAME_LEN			8
typedef struct _OPL_ParentalRating_t
{
	char	scheme[_PARENTAL_SCHEME_NAME_LEN];
	char	name[_PARENTAL_NAME_LEN];
	int		value;
	int		labels;
	char	region[_PARENTAL_REGION_NAME_LEN];
} __OPL_ParentalRating_t;

typedef	struct _OPL_PRCOLLECTION_t
{
	void		*collectionOwner;
	HxList_t	*parentalRatings;
} __OPL_PRCOLLECTION_t;

typedef struct _OPL_ParentalRatingScheme_t
{
	char		name[_PARENTAL_SCHEME_NAME_LEN];
	char		values[_PARENTAL_SCHEME_VALUE_NUM][_PARENTAL_SCHEME_VALUE_LEN];
	int			valuecount;

	OPL_ParentalRating_t	threshold;
} __OPL_ParentalRatingScheme_t;

typedef struct	_OPL_PCMAN_t
{
	int		isPINEntryLocked;
	int		isTemporaryUnlocked;
	int		isServiceLocked;
	HxList_t	*parentalRatingSchemes;
} __OPLPCMAN_t;

/**************************************************************
	private functions
 **************************************************************/

/**************************************************************
	public functions
 **************************************************************/
OPL_PCMAN_t*	OPL_PCMAN_CreateHandle(void)
{
	OPL_PCMAN_t		*pcMan;



	HxLOG_Trace();

	pcMan = (OPL_PCMAN_t*)HLIB_STD_MemCalloc(sizeof(OPL_PCMAN_t));

	return pcMan;
}

void			OPL_PCMAN_DestroyHandle(OPL_PCMAN_t *handle)
{
	HxLOG_Trace();
	HxLOG_Assert(handle);

	if (handle->parentalRatingSchemes)
	{
		HxList_t	*list = handle->parentalRatingSchemes;

		while (list)
		{
			OPL_ParentalRatingScheme_t	*scheme;

			scheme = (OPL_ParentalRatingScheme_t*)HLIB_LIST_Data(list);
			HLIB_STD_MemFree(scheme);

			list = list->next;
		}
		HLIB_LIST_RemoveAll(handle->parentalRatingSchemes);
	}
}

/**************************************************************
	ParentalControlManager functions
 **************************************************************/
#define	____PARENTAL_CONTROL_MANAGER_FUNCTIONS_______________

OPL_BOOL	OPL_PCMAN_IsEntryLocked(OPL_PCMAN_t *handle)
{
	HxLOG_Trace();
	HxLOG_Assert(handle);

	return handle->isPINEntryLocked;
}

//	Temporary Unlock.
int		OPL_PCMAN_SetParentalControlStatus(OPL_PCMAN_t *handle, const char *pinCode, OPL_BOOL enable)
{
	HxLOG_Trace();
	HxLOG_Assert(handle);

	if (APK_SCENARIO_PINCODE_SetTemporaryBlock((HCHAR *)pinCode, enable))
	{
		handle->isTemporaryUnlocked = enable;

		return OPL_PCMAN_PIN_ERROR_CORRECT;
	}

	return OPL_PCMAN_PIN_ERROR_INCORRECT;
}

//	Temporary Unlock Status
OPL_BOOL	OPL_PCMAN_GetParentalControlStatus(OPL_PCMAN_t *handle)
{
	HxLOG_Trace();
	HxLOG_Assert(handle);
#if 1
	return (APK_SCENARIO_PINCODE_GetTemporaryBlock() ? OPL_TRUE: OPL_FALSE);
#else
	return handle->isTemporaryUnlocked;
#endif
}

//	Get service block mode if parental rating is not coming.
OPL_BOOL	OPL_PCMAN_GetBlockUnrated(OPL_PCMAN_t *handle)
{
	HxLOG_Trace();
	HxLOG_Assert(handle);

	return handle->isServiceLocked;
}

//	Set Service which doesn't have parental rating information block mode
int		OPL_PCMAN_SetBlockUnrated(OPL_PCMAN_t *handle, const char *pinCode, OPL_BOOL block)
{
	HxLOG_Trace();
	HxLOG_Assert(handle);

	if (APK_SCENARIO_PINCODE_Check((const HCHAR *)pinCode))
	{
		handle->isServiceLocked = block;

		return OPL_PCMAN_PIN_ERROR_CORRECT;
	}

	return OPL_PCMAN_PIN_ERROR_INCORRECT;
}

//	Change PIN code
int		OPL_PCMAN_SetParentalControlPIN(OPL_PCMAN_t *handle, const char *oldPinCode, const char *newPinCode)
{
	HxLOG_Trace();
	HxLOG_Assert(handle);

	if (APK_SCENARIO_PINCODE_Set((const HCHAR *)oldPinCode, (const HCHAR *)newPinCode))
		return OPL_PCMAN_PIN_ERROR_CORRECT;

	return OPL_PCMAN_PIN_ERROR_INCORRECT;
}

int		OPL_PCMAN_VerifyParentalControlPIN(OPL_PCMAN_t *handle, const char *pinCode)
{
	HxLOG_Trace();
	HxLOG_Assert(handle);

	if (APK_SCENARIO_PINCODE_Verify((const HCHAR *)pinCode))
		return OPL_PCMAN_PIN_ERROR_CORRECT;

	return OPL_PCMAN_PIN_ERROR_INCORRECT;
}

//	Unlock Specific Object with Parental PIN
int		OPL_PCMAN_UnlockChannelWithParentalControlPIN(OPL_PCMAN_t *handle, const char *pinCode, OPL_Channel_t channel)
{
	HxLOG_Trace();
	HxLOG_Assert(handle);

	if (APK_SCENARIO_PINCODE_Check((const HCHAR *)pinCode))
	{
		return OPL_PCMAN_PIN_ERROR_CORRECT;
	}
	return OPL_PCMAN_PIN_ERROR_INCORRECT;
}

int		OPL_PCMAN_UnlockVBObjectWithParentalControlPIN(OPL_PCMAN_t *handle, const char *pinCode, void *vbHandle)
{
	HxLOG_Trace();
	HxLOG_Assert(handle);

	if (APK_SCENARIO_PINCODE_Check((const HCHAR *)pinCode))
	{
		return OPL_PCMAN_PIN_ERROR_CORRECT;
	}
	return OPL_PCMAN_PIN_ERROR_INCORRECT;
}

int		OPL_PCMAN_UnlockAVObjectWithParentalControlPIN(OPL_PCMAN_t *handle, const char *pinCode, void *avObjectHandle)
{
	HxLOG_Trace();
	HxLOG_Assert(handle);

	if (APK_SCENARIO_PINCODE_Check((const HCHAR *)pinCode))
	{
		return OPL_PCMAN_PIN_ERROR_CORRECT;
	}
	return OPL_PCMAN_PIN_ERROR_INCORRECT;

}

OPL_BOOL OPL_PCMAN_Prompt_For_Pin(OPL_BOOL adult, OPL_BOOL ip, OPL_BOOL pay)
{
	HUINT32 ulGuidancePolicy = 0;
	HERROR	hError;

	/* adult : Guidance Policy Check*/
	if(adult)
	{
		hError = APK_SCENARIO_GetGuidancePolicy(&ulGuidancePolicy);
		if(hError == ERR_OK && ulGuidancePolicy == 0)
			return FALSE;
	}

	return TRUE;
}

OPL_BOOL OPL_PCMAN_Validate_Pin(char *pin, OPL_BOOL adult, OPL_BOOL ip, OPL_BOOL pay)
{
	/* adult, ip, pay */
	if(pin)
		return APK_SCENARIO_PINCODE_Check(pin);

	return FALSE;
}

/**************************************************************
	ParentalRatingScheme functions
 **************************************************************/
#define	____PARENTAL_RATING_SCHEME_FUNCTIONS_______________
static HBOOL	_pcman_compare_scheme_name(void *userdata, void *listdata)
{
	char	*user_name = (char*)userdata;
	OPL_ParentalRatingScheme_t	*scheme = (OPL_ParentalRatingScheme_t*)listdata;

	if (HLIB_STD_StrCaseCmp(user_name, scheme->name) == 0)
		return TRUE;

	return FALSE;
}

int		OPL_PCMAN_SCHEME_Length(OPL_ParentalRatingScheme_t *handle)
{
	HxLOG_Trace();
	HxLOG_Assert(handle);

	return handle->valuecount;
}

int		OPL_PCMAN_SCHEME_IndexOf(OPL_ParentalRatingScheme_t *handle, const char *ratingValue)
{
	int			i;

	HxLOG_Trace();
	HxLOG_Assert(handle);

	for (i = 0; i < handle->valuecount; i++)
	{
		if (HLIB_STD_StrCaseCmp(handle->values[i], ratingValue) == 0)
			return i;
	}

	return -1;
}

const OPL_ParentalRating_t* OPL_PCMAN_SCHEME_GetThreshold(OPL_ParentalRatingScheme_t *handle)
{
	HxLOG_Trace();
	HxLOG_Assert(handle);

	return &handle->threshold;
}

int 		OPL_PCMAN_SCHEME_ChangeThreshold(OPL_ParentalRatingScheme_t *handle, unsigned int ratingValue)
{

	HxLOG_Trace();
	HxLOG_Assert(handle);
	// TODO : we have to consider combine below routine, because it can be useless in API level.
	HxLOG_Print("[%s] ratingValue is %d\n",__FUNCTION__,ratingValue);
	if(ratingValue <= eDxRATING_AGE_20)
	{
		if(APK_SCENARIO_SetMaturityRating((DxRatingAge_e)ratingValue) == ERR_OK)
		{
			handle->threshold.value = ratingValue;
			HxSTD_MemSet(handle->threshold.name, 0x00, _PARENTAL_NAME_LEN);
			HxSTD_snprintf(handle->threshold.name, _PARENTAL_NAME_LEN, "%d", handle->threshold.value);
			HxLOG_Print("[%s] handle->threshold.name is %s\n",__FUNCTION__,handle->threshold.name);
			return OPL_PCMAN_CHANGE_THRESHOLD_SUCCESS;
		}
	} else if (eDxRATING_AGE_LOCK_ALL == ratingValue)
	{
		// for supporting Lock All.
		if(APK_SCENARIO_SetMaturityRating((int)ratingValue) == ERR_OK)
		{
			handle->threshold.value = ratingValue;
			HxSTD_MemSet(handle->threshold.name, 0x00, _PARENTAL_NAME_LEN);
			HxSTD_snprintf(handle->threshold.name, _PARENTAL_NAME_LEN, "%d", handle->threshold.value);
			HxLOG_Print("[%s] handle->threshold.name is %s\n",__FUNCTION__,handle->threshold.name);
			return OPL_PCMAN_CHANGE_THRESHOLD_SUCCESS;
		}
	}
	HxLOG_Print("[%s] fail to change Threshold \n",__FUNCTION__);
	return OPL_PCMAN_CHANGE_THRESHOLD_FAIL;
}

const char*	OPL_PCMAN_SCHEME_GetName(OPL_ParentalRatingScheme_t *scheme)
{
	HxLOG_Trace();
	HxLOG_Assert(scheme);

	return (const char*)scheme->name;
}

const char*	OPL_PCMAN_SCHEME_IconUri(OPL_PCMAN_t *handle, int index)
{
	int			i;
	HxList_t	*list;

	HxLOG_Trace();
	HxLOG_Assert(handle);

	i = 0;
	list = handle->parentalRatingSchemes;
	while (list)
	{
		if (i++ == index)
			break;
		list = list->next;
	}

	return NULL;
/*
	//	Where do i get the uri icon???
	if (list)
	{
		OPL_ParentalRatingScheme_t	*scheme = (OPL_ParentalRatingScheme_t*)HLIB_LIST_Data(list);

		return scheme->
	}
*/
	return NULL;
}

const char*	OPL_PCMAN_PR_GetName(OPL_ParentalRating_t *handle)
{
	HxLOG_Trace();
	HxLOG_Assert(handle);

	return handle->name;
}

const char*	OPL_PCMAN_PR_GetScheme(OPL_ParentalRating_t *handle)
{
	HxLOG_Trace();
	HxLOG_Assert(handle);

	return handle->scheme;
}

const int	OPL_PCMAN_PR_GetValue(OPL_ParentalRating_t *handle)
{
	HxLOG_Trace();
	HxLOG_Assert(handle);

	return handle->value;
}

const int	OPL_PCMAN_PR_GetLabels(OPL_ParentalRating_t *handle)
{
	HxLOG_Trace();
	HxLOG_Assert(handle);

	return handle->labels;
}

const char*	OPL_PCMAN_PR_GetRegion(OPL_ParentalRating_t *handle)
{
	HxLOG_Trace();
	HxLOG_Assert(handle);

	return handle->region;
}

OPL_ParentalRatingScheme_t*	OPL_PCMAN_AddParentalRatingScheme(OPL_PCMAN_t *handle, const char *name, const char *values)
{
	HxList_t	*list;
	OPL_ParentalRatingScheme_t	*scheme;

	HxLOG_Trace();
	/*dvb-siï¿½ï¿½ ï¿½ï¿½ï¿?valueï¿½ï¿½ï¿½ï¿½ nullï¿½ï¿½ ï¿½ï¿½ ï¿½ï¿½ï¿½ï¿½*/
	HxLOG_Assert(handle && name);
	HxLOG_Debug("name : [%s]\n", name);
	if(HxSTD_StrCmp(name, OPL_PARENTAL_SCHEM_NAME_DVB_SI) != 0)
	{
		HxLOG_Assert(values);
	}
	list = HLIB_LIST_FindEx(handle->parentalRatingSchemes, name, _pcman_compare_scheme_name);
	if (list)
		return (void*)HLIB_LIST_Data(list);

	scheme = (OPL_ParentalRatingScheme_t*)HLIB_STD_MemCalloc(sizeof(OPL_ParentalRatingScheme_t));
	HxSTD_StrNCpy(scheme->name, name, _PARENTAL_SCHEME_NAME_LEN - 1);

	scheme->valuecount = 0;

	if (values)
	{
		HxLOG_Debug("values : [%s]\n", values);
		char	*valuePtr = HLIB_STD_StrDup(values);
		char	*token, *save;

		token = HxSTD_StrTok(valuePtr, ",", &save);
		while (token)
		{
			HxLOG_Debug("\tToken(%d) : [%s]\n", scheme->valuecount, token);
			HxSTD_StrNCpy(scheme->values[scheme->valuecount++], token, _PARENTAL_SCHEME_VALUE_LEN - 1);
			token = HxSTD_StrTok(NULL, ",", &save);
		}
		HLIB_STD_MemFree(valuePtr);
	}
	HxSTD_StrNCpy(scheme->threshold.scheme, scheme->name, _PARENTAL_SCHEME_NAME_LEN -1);
	if(HxSTD_StrCmp(name, OPL_PARENTAL_SCHEM_NAME_DVB_SI) == 0)
	{
		DxRatingAge_e	ratingValue;

		if(APK_SCENARIO_GetMaturityRating(&ratingValue) == ERR_OK)
		{
			HxSTD_snprintf(scheme->threshold.name, _PARENTAL_NAME_LEN -1, "%d", ratingValue);
			scheme->threshold.value = ratingValue;
		}
	}

	handle->parentalRatingSchemes = HLIB_LIST_Append(handle->parentalRatingSchemes, (void*)scheme);

	return (OPL_ParentalRatingScheme_t*)scheme;
}

OPL_ParentalRatingScheme_t*	OPL_PCMAN_GetParentalRatingScheme(OPL_PCMAN_t *handle, const char *name)
{
	HxList_t	*list;

	HxLOG_Trace();
	HxLOG_Assert(handle && name);

	list = HLIB_LIST_FindEx(handle->parentalRatingSchemes, name, _pcman_compare_scheme_name);
	if (list)
		return HLIB_LIST_Data(list);

	return NULL;
}

int		OPL_PCMAN_GetParentalRatingSchemesCount(OPL_PCMAN_t *handle)
{
	HxLOG_Trace();
	HxLOG_Assert(handle);

	return HLIB_LIST_Length(handle->parentalRatingSchemes);
}

OPL_ParentalRatingScheme_t*	OPL_PCMAN_GetParentalRatingSchemeByIndex(OPL_PCMAN_t *handle, int index)
{
	int		i;
	HxList_t	*list;

	HxLOG_Trace();
	HxLOG_Assert(handle);

	i = 0;
	list = handle->parentalRatingSchemes;
	while (list)
	{
		if (i == index)
			break;
		i++;
		list = list->next;
	}

	return (OPL_ParentalRatingScheme_t*)HLIB_LIST_Data(list);
}

/**************************************************************
	ParentalRatingCollection functions
 **************************************************************/
OPL_ParentalRating_t*	OPL_PCMAN_PRC_AddParentalRating(void *collectionOwner, const char *scheme, const char *name, int value, int labels, const char *region)
{
	OPL_ParentalRating_t	*parentalRating;

	parentalRating = (OPL_ParentalRating_t*)HLIB_STD_MemCalloc(sizeof(OPL_ParentalRating_t));
	if (scheme)
		HxSTD_StrNCpy(parentalRating->scheme, scheme, _PARENTAL_SCHEME_NAME_LEN - 1);
	else
		HxSTD_StrNCpy(parentalRating->scheme, "dvb-si", _PARENTAL_SCHEME_NAME_LEN - 1);

	if (name)
		HxSTD_StrNCpy(parentalRating->name, name, _PARENTAL_NAME_LEN - 1);
	else
		HxSTD_snprintf(parentalRating->name, _PARENTAL_NAME_LEN - 1, "%d", value);

	parentalRating->value = value;
	parentalRating->labels = labels;

	if (region)
		HxSTD_StrNCpy(parentalRating->region, region, _PARENTAL_REGION_NAME_LEN - 1);

	return parentalRating;
}

void	OPL_PCMAN_PRC_RemoveParentalRating(OPL_ParentalRating_t *id)
{
	HLIB_STD_MemFree(id);
}

//	Reset PIN code
int		OPL_PCMAN_ResetParentalControlPIN(OPL_PCMAN_t *handle, const char *newPinCode)
{
	HxLOG_Trace();
	HxLOG_Assert(handle);

	if (APK_SCENARIO_PINCODE_Reset((const HCHAR *)newPinCode))
		return OPL_PCMAN_PIN_ERROR_CORRECT;

	return OPL_PCMAN_PIN_ERROR_INCORRECT;
}

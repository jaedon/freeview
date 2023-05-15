

#include "htype.h"
#include "octo_common.h"

#include "../local_include/_svc_epg.h"
#include "../local_include/_svc_epg_query.h"



/*******************************************************************/
/********************      Functions       *************************/
/*******************************************************************/
#define ____GLOBAL_FUNC____

void		svc_epgquery_Traverse (const SvcEpg_Query_t *q, void (* each) (const SvcEpg_Query_t *q, void *), void *userdata)
{
	if (q)
	{
		svc_epgquery_Traverse(q->left , each, userdata);
		svc_epgquery_Traverse(q->right, each, userdata);

		if (each)
			each(q, userdata);
	}
}

SvcEpg_Query_t *	svc_epgquery_New (SvcEpg_Field_e field, SvcEpg_Comparison_e comparison, const HCHAR *value)
{
	SvcEpg_Query_t	*q;

	q = (SvcEpg_Query_t *)HLIB_STD_MemCalloc(sizeof(SvcEpg_Query_t));
	if (q)
	{
		HERROR		hErr;

		q->field = field;
		q->comparison = comparison;
		hErr = DB_PARAM_GetItem(eDB_PARAM_ITEM_LANG_MAIN, (HUINT32 *)&q->language, 0);
		if (ERR_OK != hErr)
		{
			HxLOG_Warning("DB_PARAM_GetItem(eDB_PARAM_ITEM_LANG_MAIN) Error!!\n");
		}

		{	// 이걸 여기서 해야하나... ?? 일단 넣어보자
//			HCHAR	iso3166[4];
			HxCountry_e	eCountryId;
			hErr = DB_PARAM_GetItem(eDB_PARAM_ITEM_COUNTRY_ID, (HUINT32 *)&eCountryId, 0);
			if (ERR_OK != hErr)
			{
				HxLOG_Warning("DB_PARAM_GetItem(eDB_PARAM_ITEM_LANG_MAIN) Error!!\n");
			}

#if 0
			if (MWC_UTIL_GetIso3166CodeByCountryId(eCountryId, iso3166) == ERR_OK)
			{
				q->country = HLIB_COUNTRY_3166_Alpha3ToId(iso3166);
			}
#endif
			q->country = eCountryId;
		}

		if (value)
		{
			if (HLIB_STD_IsInteger(value))
				q->valueInt = HLIB_STD_StrToINT32(value);
			q->value = HLIB_STD_StrDup(value);
			W_GOTO_IF(q->value == NULL, FAILED);
		}
	}
	return q;
FAILED:
	svc_epgquery_Delete(q);
	return NULL;
}

void		svc_epgquery_Delete (SvcEpg_Query_t *q)
{
	if (q)
	{
		if (q->left)
			svc_epgquery_Delete(q->left);
		if (q->right)
			svc_epgquery_Delete(q->right);

		if (q->value)
			HLIB_STD_MemFree(q->value);
		HLIB_STD_MemFree(q);
	}
}

SvcEpg_Query_t *	svc_epgquery_NewWithInteger (SvcEpg_Field_e field, SvcEpg_Comparison_e comparison, HUINT32 value)
{
	HCHAR	valueStr[32];

	HxSTD_PrintToStrN(valueStr, 32, "%u", (unsigned int)value);
	return svc_epgquery_New(field, comparison, valueStr);
}




/**************************************************************************************/
/**
 * @file oplquery.c
 *
 * Defines the query structure.
 *
 * @attention   Copyright (c) 2011 Humax - All rights reserved.
 *
 **************************************************************************************
 **/

#include "oplquery.h"
#include "oplprogramme.h"

#if defined( CONFIG_OP_JAPAN)
#include "oplprogramme_japan.h"
#endif

#include <hlib.h>
#include <apk.h>




#define	____OPL_QUERY_PRIVATE_____________________________________________________________________

static const char *	query_OptimizeValue (const char *field, int comparison, const char *value)
{
	static char	s_valueInt[64];
	DxOperator_e op;

	APK_SCENARIO_GetOperatorType(&op);

	if (HLIB_STD_StrCaseCmp(field, "programmeID") == 0
		&& comparison == eOPL_QUERY_EQ
		&& value
	) {
		int	svcuid, eventid;

		if (OPL_Programme_GetUniqueIDsFromProgrammeID(value, &svcuid, &eventid))
		{
			HxSTD_PrintToStrN(s_valueInt, 64, "%d.%d", svcuid, eventid);
			return s_valueInt;
		}
	}

	if (HLIB_STD_StrCaseCmp(field, "genre") == 0
		&& comparison == eOPL_QUERY_EQ
		&& value
	) {

		switch(op)
		{
			case eDxOPERATOR_JCOM:
#if defined( CONFIG_OP_JAPAN)
				return OPL_PROGRAMME_JAPAN_GetGenreValue((HCHAR*) value );
#endif
				break;

			default:
				break;
		}
	}
	return value;
}

#define	____OPL_QUERY_PUBLIC______________________________________________________________________

OPL_HANDLE	OPL_QUERY_New (const char *field, int comparison, const char *value)
{
	const HCHAR	*comp;

	HxLOG_Trace();
	HxLOG_Assert(field);

	switch(comparison)
	{
	case eOPL_QUERY_EQ:
		comp = "==";
		break;
	case eOPL_QUERY_NOT_EQ:
		comp = "!=";
		break;
	case eOPL_QUERY_GT:
		comp = ">";
		break;
	case eOPL_QUERY_GT_OR_EQ:
		comp = ">=";
		break;
	case eOPL_QUERY_LT:
		comp = "<";
		break;
	case eOPL_QUERY_LT_OR_EQ:
		comp = "<=";
		break;
	case eOPL_QUERY_CONTAINS:
		comp = "contains";
		break;
	case eOPL_QUERY_EXIST:
		comp = "exist";
		break;
	default:
		return (OPL_HANDLE)NULL;
	}
	return (OPL_HANDLE)APK_META_QUERY_NewQuery(
						  (const HCHAR *)field
						, comp
						, (const HCHAR *)query_OptimizeValue(field, comparison, value)
					);
}

void		OPL_QUERY_Delete (OPL_HANDLE q)
{
	HxLOG_Trace();

	APK_META_QUERY_DeleteQuery((APK_QUERY_t)q);
}

OPL_HANDLE	OPL_QUERY_Clone (const OPL_HANDLE q)
{
	HxLOG_Trace();

	return (OPL_HANDLE)APK_META_QUERY_CloneQuery((const APK_QUERY_t)q);
}

OPL_HANDLE	OPL_QUERY_And (const OPL_HANDLE q1, const OPL_HANDLE q2)
{
	HxLOG_Trace();
	HxLOG_Assert(q1 && q2);

	return (OPL_HANDLE)APK_META_QUERY_And((const APK_QUERY_t)q1, (const APK_QUERY_t)q2);
}

OPL_HANDLE	OPL_QUERY_Or (const OPL_HANDLE q1, const OPL_HANDLE q2)
{
	HxLOG_Trace();
	HxLOG_Assert(q1 && q2);

	return (OPL_HANDLE)APK_META_QUERY_Or((const APK_QUERY_t)q1, (const APK_QUERY_t)q2);
}

OPL_HANDLE	OPL_QUERY_Not (const OPL_HANDLE q)
{
	HxLOG_Trace();
	HxLOG_Assert(q);

	return (OPL_HANDLE)APK_META_QUERY_Not((const APK_QUERY_t)q);
}




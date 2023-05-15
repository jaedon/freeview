/*******************************************************************
	File Description
********************************************************************/
/**
	@file	 rest_datatype_sysenv.c
	@brief

	Description:  									\n
	Module: node/binding/datatype_convert		\n

	Copyright (c) 2013 HUMAX Co., Ltd.				\n
	All rights reserved.							\n
*/

/*
* Character encoding.
*
* MS949
*
* This source file that uses MS949 encoding.
* MS949 encoding À» »ç¿ëÇÏ´Â source file ÀÔ´Ï´Ù.
* MS949 encodingªòÞÅéÄª·ªÆ source fileªÇª¹¡£
* Quelldatei, die MS949-Codierung verwendet.
*
*/
/*
 * (c) 2011-2013 Humax Co., Ltd.
 * This program is produced by Humax Co., Ltd. ("Humax") and
 * the proprietary Software of Humax and its licensors. Humax provides you, as an Authorized Licensee,
 * non-assignable, non-transferable and non-exclusive license to use this Software.
 * You acknowledge that this Software contains valuable trade secrets of Humax and by using this Software
 * you agree to the responsibility to take all reasonable efforts to protect the any information
 * you receive from Humax. You are not permitted to duplicate, modify, distribute, sell or lease and
 * reverse engineer or extract the source code of this Software unless you have Humax's written permission to do so.
 * If you have no authorized license, discontinue using this Software immediately.
 *
 * THE SOFTWARE IS PROVIDED "AS IS" AND HUMAX MAKES NO PROMISES, REPRESENTATIONS OR WARRANTIES, EITHER EXPRESS,
 * IMPLIED OR STATUTORY, OR OTHERWISE, WITH RESPECT TO THE SOFTWARE.
 * IN NO EVENT SHALL HUMAX BE LIABLE FOR LOST PROFITS, REVENUES, OR DATA, FINANCIAL LOSSES OR INDIRECT, SPECIAL,
 * CONSEQUENTIAL, EXEMPLARTY OR PUNITIVE DAMAGES WHATSOEVER RELATING TO YOUR USE OR INABILITY TO USE THE SOFTWARE.

 * This License is effective until terminated. You may terminate this License at any time by destroying all copies
 * of the Software including all documentation. This License will terminate immediately without notice from Humax
 * to you if you fail to comply with any provision of this License. Upon termination, you must destroy all copies
 * of the Software and all documentation.

 * The laws of the Republic of Korea will apply to any disputes arising out of or relating to this Copyright Notice.
 * All claims arising out of or relating to this Copyright Notice will be litigated in the Seoul Central District Court,
 * in the Republic of Korea.
*/

/*******************************************************************/
/********************      Header Files    *************************/
/*******************************************************************/
#include <hlib.h>
#include <dlib.h>
#include <apk.h>
#include <rest_bind_api.h>
#include <rest_datatype.h>
#include <rest_datatype_base.h>

/*******************************************************************/
/********************      Local Definition         *************************/
/*******************************************************************/

/*******************************************************************/
/********************      Typedef         *************************/
/*******************************************************************/

/*******************************************************************/
/********************      Global Variables     ********************/
/*******************************************************************/

/*******************************************************************/
/********************      Static Variables     ********************/
/*******************************************************************/
// Converting Types:
STATIC RestEnumStringTable_t	 s_stRestDataType_MaturityRating =
{
	(HCHAR*)DxNAMEOF(DxRatingAge_e),
		
	{
		{	"view_all",		eDxRATING_VIEW_ALL		},
		{	"4",			eDxRATING_AGE_4			},
		{	"5",			eDxRATING_AGE_5			},
		{	"6",			eDxRATING_AGE_6			},
		{	"7",			eDxRATING_AGE_7 		},
		{	"8",			eDxRATING_AGE_8 		},
		{	"9",			eDxRATING_AGE_9 		},
		{	"10",			eDxRATING_AGE_10 		},
		{	"11",			eDxRATING_AGE_11		},
		{	"12",			eDxRATING_AGE_12		},
		{	"13",			eDxRATING_AGE_13		},
		{	"14",			eDxRATING_AGE_14		},
		{	"15",			eDxRATING_AGE_15		},
		{	"16",			eDxRATING_AGE_16		},
		{	"17",			eDxRATING_AGE_17		},
		{	"18",			eDxRATING_AGE_18		},
		{	"19",			eDxRATING_AGE_19		},
		{	"20",			eDxRATING_AGE_20		},
		{	"all_age", 		eDxRATING_AGE_ALL_AGE	},
		{	"children",		eDxRATING_AGE_CHILDREN	},
		{	"x_rated", 		eDxRATING_AGE_X_RATED	},
		{	"lock_all",		eDxRATING_AGE_LOCK_ALL	},

		{	NULL, 			0						}
	}
};

STATIC RestEnumStringTable_t	 s_stRestDataType_Operator =
{
	(HCHAR*)DxNAMEOF(DxOperator_e),
		
	{
		{	"none",			eDxOPERATOR_None		},
		{	"ziggo",		eDxOPERATOR_Ziggo		},
		{	"comhem",		eDxOPERATOR_Comhem		},
		{	"kdg",			eDxOPERATOR_KDG			},
		{	"skyd",			eDxOPERATOR_SkyD 		},
		{	"ses",			eDxOPERATOR_Ses 		},
		{	"ficom",		eDxOPERATOR_FiCom		},
		{	"freesat",		eDxOPERATOR_Freesat		},
		{	"canalready",	eDxOPERATOR_CanalReady 	},
		{	"rcs", 			eDxOPERATOR_Rcs			},
		{	"ntvplus",		eDxOPERATOR_NtvPlus 	},
		{	"bbc",			eDxOPERATOR_BBC			},
		{	"upc",			eDxOPERATOR_UPC			},
		{	"digiturk",		eDxOPERATOR_Digiturk	},
		{	"adtv",			eDxOPERATOR_ADTV 		},
		{	"aljazeera",	eDxOPERATOR_Aljazeera 	},
		{	"cne",			eDxOPERATOR_CNE			},
		{	"spaintdt",		eDxOPERATOR_SpainTdt	},
		{	"truevisions",	eDxOPERATOR_Truevisions	},
		{	"boxer", 		eDxOPERATOR_Boxer		},
		{	"tivu",			eDxOPERATOR_TIVU 		},
		{	"jcon",			eDxOPERATOR_JCOM		},
		{	"skapa",		eDxOPERATOR_SKAPA		},
		{	"mostelcom",	eDxOPERATOR_Mostelecom	},
		{	"swisscanal",	eDxOPERATOR_SwissCanal 	},
		{	"russiatvt",	eDxOPERATOR_RussiaTvt 	},
		{	"almajd",		eDxOPERATOR_Almajd		},
		{	"nova",			eDxOPERATOR_NOVA		},
		{	"skycable",		eDxOPERATOR_SkyCable	},
		{	"germancab", 	eDxOPERATOR_GermanCab	},
		{	"primacom",		eDxOPERATOR_Primacom 	},
		{	"vtc",			eDxOPERATOR_VTC			},
		{	"ertelecom", 	eDxOPERATOR_ERtelecom	},
		{	"middleeast",	eDxOPERATOR_MiddleEast 	},

		{	NULL,			0 	}
	}
};

STATIC RestEnumStringTable_t	 s_stRestDataType_AntConnect =
{
	(HCHAR*)DxNAMEOF(DxAntennaConnection_e),
		
	{
		{	"one_cable",	eDxANTCONN_ONECABLE			},
		{	"two_same",		eDxANTCONN_TWOSAME			},
		{	"two_diff",		eDxANTCONN_TWODIFF			},
		{	"one_input",	eDxANTCONN_NONSATONECABLE	},
		{	"two_input",	eDxANTCONN_NONSATTWOCABLE	},

		{	NULL,			0 	}
	}
};

#define ___LOCAL_FUNCTIONS___

#define ___MEMBER_FUNCTIONS___
#define _____SYSTEM_ENVIRONMENT_____
HERROR rest_datatype_CleanSysEnv (RestSysEnv_t *pstSysEnv)
{
	if (NULL == pstSysEnv)
	{
		HxLOG_Error("pstSvc NULL\n");
		return ERR_FAIL;
	}

	HxSTD_MemSet(pstSysEnv, 0, sizeof(RestSysEnv_t));

	pstSysEnv->nHardOfHearing		= -1;
	pstSysEnv->nMaturityRating		= -1;
	pstSysEnv->eCountry				= eCountryID_NONE;
	pstSysEnv->eOperater			= eDxOPERATOR_NOT_DEFINED;
	pstSysEnv->nAntConnectType		= -1;
	pstSysEnv->nStartPaddingTime	= -1;
	pstSysEnv->nEndPaddingTime		= -1;
	pstSysEnv->nAutoDelete			= -1;
	pstSysEnv->nGmtOffset			= -255;

	return ERR_OK;
}

HINT32 rest_datatype_JsonizeSysEnv (HINT32 nJsonWriter, RestSysEnv_t *pstSysEnv, HCHAR *szObjectName, HCHAR *szSelect)
{
	HINT32		 nWriter;

	if (NULL == pstSysEnv)
	{
		HxLOG_Error("pstSvc NULL\n");
		return nJsonWriter;
	}

	nWriter = (0 == nJsonWriter) ? HLIB_JSONWRITER_Open() : nJsonWriter;
	if (0 == nWriter)
	{
		HxLOG_Error(">> HLIB_JSONWRITER_Open failed:\n");
		return 0;
	}

	if (NULL == szObjectName)
	{
		// No Object: Nothing to do
	}
	else if ('\0' == *szObjectName)
	{
		// Make the object, but no name
		HxJSONWRITER_ObjectBegin(nWriter);
	}
	else
	{
		// Make the object
		HxJSONWRITER_Object(nWriter, szObjectName);
	}

	// Contents:
	{
		(void)rest_datatype_base_JsonizeSysEnv(nWriter, pstSysEnv, szSelect);
	}

	if (NULL != szObjectName)
	{
		HxJSONWRITER_ObjectEnd(nWriter);
	}

	return nWriter;	
}

HERROR rest_datatype_DejsonizeSysEnv (HxJSON_t *pstJson, RestSysEnv_t *pstSysEnv, HBOOL bClearObj)
{

	if (NULL == pstSysEnv)
	{
		HxLOG_Error("pstSysEnv NULL\n");
		return ERR_FAIL;
	}

	if (NULL == pstJson)
	{
		HxLOG_Error("pstJson NULL\n");
		return ERR_FAIL;
	}

	if (TRUE == bClearObj)
	{
		rest_datatype_CleanSysEnv(pstSysEnv);
	}

	// Contents:
	{
		(void)rest_datatype_base_DejsonizeSysEnv(pstJson, pstSysEnv);
	}

	return ERR_OK;	
}

#define _____INITIALIZATION_____
HERROR rest_datatype_InitSysEnv (void)
{
	(void)rest_datatype_RegisterEnumTable(&s_stRestDataType_MaturityRating);
	(void)rest_datatype_RegisterEnumTable(&s_stRestDataType_Operator);
	(void)rest_datatype_RegisterEnumTable(&s_stRestDataType_AntConnect);

	return ERR_OK;
}


#define ___GLOBAL_FUNCTIONS___


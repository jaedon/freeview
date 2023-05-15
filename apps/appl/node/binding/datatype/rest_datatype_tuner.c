/*******************************************************************
	File Description
********************************************************************/
/**
	@file	 rest_datatype_tuner.c
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
STATIC RestEnumStringTable_t	 s_stRestDataType_DeliveryType =
{
	(HCHAR*)DxNAMEOF(DxDeliveryType_b),
		
	{
		{	"all",			eDxDELIVERY_TYPE_ALL			},
		{	"sat",			eDxDELIVERY_TYPE_SAT			},
		{	"ter",			eDxDELIVERY_TYPE_TER			},
		{	"cab",			eDxDELIVERY_TYPE_CAB			},
		{	"multicast",	eDxDELIVERY_TYPE_MULTICAST 		},

		{	NULL, 			0						}
	}
};

#if 0
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
#endif
#define ___LOCAL_FUNCTIONS___

#define ___MEMBER_FUNCTIONS___
#define _____SYSTEM_ENVIRONMENT_____

HINT32 rest_datatype_JsonizeTunerStatus(HINT32 nJsonWriter, RestTunerInst_t *pstStatus, HCHAR *szSelect)
{
	HINT32		 nWriter;

	nWriter = (0 == nJsonWriter) ? HLIB_JSONWRITER_Open() : nJsonWriter;
	if (0 == nWriter)
	{
		HxLOG_Error(">> HLIB_JSONWRITER_Open failed:\n");
		return 0;
	}

	HxJSONWRITER_ObjectBegin(nWriter);
	{
		(void)rest_datatype_SetJsonInteger(nWriter, "tunerId", pstStatus->nTunerId, szSelect);
		(void)rest_datatype_SetJsonEnum(nWriter, "tunerType", DxNAMEOF(DxDeliveryType_b), (HINT32)pstStatus->eTunerType, szSelect);
		(void)rest_datatype_SetJsonInteger(nWriter, "locked", pstStatus->bLocked, szSelect);

		if(pstStatus->bLocked)
		{
			(void)rest_datatype_SetJsonInteger(nWriter, "signalLevel", pstStatus->nSignalLevel, szSelect);
			(void)rest_datatype_SetJsonInteger(nWriter, "signalQuality", pstStatus->nSignalQuality, szSelect);
			(void)rest_datatype_SetJsonInteger(nWriter, "agc", pstStatus->nAgc, szSelect);
			(void)rest_datatype_SetJsonInteger(nWriter, "ber", pstStatus->fBer, szSelect);
			(void)rest_datatype_SetJsonInteger(nWriter, "snr", pstStatus->fSnr, szSelect);
			(void)rest_datatype_SetJsonInteger(nWriter, "inputPower", pstStatus->fInputPower, szSelect);			

			(void)HxJSONWRITER_StringValue(nJsonWriter, "tunedTransponder");
			rest_datatype_JsonizeTuningParam(nJsonWriter, &pstStatus->stTuneParam, szSelect);
	
		}

	}
	HxJSONWRITER_ObjectEnd(nWriter);

	return nWriter;
}



HINT32 rest_datatype_JsonizeTunerOverview(HINT32 nJsonWriter, HINT32 nTunerNum, DxAntennaConnection_e eConType, HCHAR *szSelect)
{
	HINT32		 nWriter;

	nWriter = (0 == nJsonWriter) ? HLIB_JSONWRITER_Open() : nJsonWriter;
	if (0 == nWriter)
	{
		HxLOG_Error(">> HLIB_JSONWRITER_Open failed:\n");
		return 0;
	}

	HxJSONWRITER_ObjectBegin(nWriter);
	{
		(void)rest_datatype_SetJsonInteger(nWriter, "tunerNum", nTunerNum, szSelect);
		(void)rest_datatype_SetJsonEnum(nWriter, "antConnectType", DxNAMEOF(DxAntennaConnection_e), (HINT32)eConType, szSelect);
	}
	HxJSONWRITER_ObjectEnd(nWriter);

	return nWriter;
}



#define _____INITIALIZATION_____
HERROR rest_datatype_InitTuner (void)
{
	(void)rest_datatype_RegisterEnumTable(&s_stRestDataType_DeliveryType);
//	(void)rest_datatype_RegisterEnumTable(&s_stRestDataType_AntConnect);	

	return ERR_OK;
}


#define ___GLOBAL_FUNCTIONS___


/*******************************************************************
	File Description
********************************************************************/
/**
	@file	  oapi_scenario.h
	@brief

	Description:  									\n
	Module: OAPI / API					\n

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

#ifndef __OAPI_SCENARIO_H__
#define __OAPI_SCENARIO_H__

/*******************************************************************/
/********************      Header Files    *************************/
/*******************************************************************/

/*******************************************************************/
/********************      Defines         *************************/
/*******************************************************************/
#define	RPC_OAPI_SCENARIO_SET						"oapi.scenario.set"

#define	OAPI_SCENARIO_MAINLANGUAGE					"mainlang"
// Audio Language Base
#define	OAPI_SCENARIO_AUDIOLANGUAGE					"audiolang"
#define	OAPI_SCENARIO_AUDIOLANGUAGE0				"audiolang0"
#define	OAPI_SCENARIO_AUDIOLANGUAGE1				"audiolang1"
#define	OAPI_SCENARIO_AUDIOLANGUAGE2				"audiolang2"
#define	OAPI_SCENARIO_AUDIOLANGUAGE3				"audiolang3"
#define	OAPI_SCENARIO_AUDIOLANGUAGE4				"audiolang4"
#define	OAPI_SCENARIO_AUDIOLANGUAGE5				"audiolang5"
#define	OAPI_SCENARIO_AUDIOLANGUAGE6				"audiolang6"
#define	OAPI_SCENARIO_AUDIOLANGUAGE7				"audiolang7"
#define	OAPI_SCENARIO_AUDIOLANGUAGE_UNDEFFINED	"audioangund"
#define	OAPI_SCENARIO_AUDIOLANGUAGE_ORIGINAL		"audiolangorg"
#define	OAPI_SCENARIO_AUDIOLANGUAGE_DEFAULT		"audiolangdef"
// Subtitle Language Base
#define	OAPI_SCENARIO_SUBTITLELANGUAGE				"subtitlelang"
#define	OAPI_SCENARIO_SUBTITLELANGUAGE0				"subtitlelang0"
#define	OAPI_SCENARIO_SUBTITLELANGUAGE1				"subtitlelang1"
#define	OAPI_SCENARIO_SUBTITLELANGUAGE2				"subtitlelang2"
#define	OAPI_SCENARIO_SUBTITLELANGUAGE3				"subtitlelang3"
#define	OAPI_SCENARIO_SUBTITLELANGUAGE4				"subtitlelang4"
#define	OAPI_SCENARIO_SUBTITLELANGUAGE5				"subtitlelang5"
#define	OAPI_SCENARIO_SUBTITLELANGUAGE6				"subtitlelang6"
#define	OAPI_SCENARIO_SUBTITLELANGUAGE7				"subtitlelang7"
#define	OAPI_SCENARIO_SUBTITLELANGUAGE_UNDEFFINED	"subtitlelangund"
#define	OAPI_SCENARIO_SUBTITLELANGUAGE_ORIGINAL		"subtitlelangorg"
#define	OAPI_SCENARIO_SUBTITLELANGUAGE_DEFAULT		"subtitlelangdef"
#define	OAPI_SCENARIO_SUBTITLESETTING				"subtitlesetting"
#define	OAPI_SCENARIO_HARDOFHEARING					"hardofhearing"
#define OAPI_SCENARIO_MATURITYRATING				"maturiyrating"
#define	OAPI_SCENARIO_COUNTRYCODE					"countrycode"
#define	OAPI_SCENARIO_OPERATORTYPE					"operatortype"
#define	OAPI_SCENARIO_ANTENNACONNECTION				"antennaconnection"
#define	OAPI_SCENARIO_STARTPADDING					"startpadding"
#define	OAPI_SCENARIO_ENDPADDING					"endpadding"
#define	OAPI_SCENARIO_AUTODELETE					"autodelete"
#define	OAPI_SCENARIO_POSTCODE						"postcode"
#define	OAPI_SCENARIO_GMTOFFSET						"gmtoffset"
#define	OAPI_SCENARIO_CHANNELSETUPPIN				"channelsetuppin"
#define	OAPI_SCENARIO_GUIDANCEPOLICY				"guidancepolicy"
#define	OAPI_SCENARIO_DAYLIGHTSAVING				"daylightsaving"
#define	OAPI_SCENARIO_TIMEOFFSET					"timeoffset"
#define	OAPI_SCENARIO_TIMEUPDATE					"timeupdate"
#define	OAPI_SCENARIO_SWUPDATEVERSION				"swupdateversion"
#define OAPI_SCENARIO_IPSWUPDATEVERSION             "ipswupdateversion"
#define	OAPI_SCENARIO_BOUQUETNAME					"bouquetname"
#define	OAPI_SCENARIO_SIMODE						"simode"
#define	OAPI_SCENARIO_BOUQUETID						"bouquetid"
#define	OAPI_SCENARIO_REGIONCODE					"regioncode"
#define	OAPI_SCENARIO_ADULTCHANNEL					"adultchannel"

#define	OAPI_SCENARIO_SATREC_RECEIVERID				"receiverid"
#define	OAPI_SCENARIO_SATREC_INTERVAL				"updateinterval"
#define	OAPI_SCENARIO_TVTV_ENABLE					"tvtvenable"
#define	OAPI_SCENARIO_TVTV_DOWNTYPE					"tvtvdowntype"
#define	OAPI_SCENARIO_FAVCHLISTNUMBERING			"favchlistnumbering"
#define	OAPI_SCENARIO_DESCRAMBLEONOFF				"descrambleonoff"
#define	OAPI_SCENARIO_NETWORK_MSG_INFO				"networkmsginfo"
#define	OAPI_SCENARIO_LIVESTREAM_PRIORITY			"livestreampriority"
#define	OAPI_SCENARIO_AUTOMATIC_POWERDOWN			"automaticpowerdown"
#define OAPI_SCENARIO_PRIORITY_HDMIOUTPUT			"priorityhdmioutput"
#define OAPI_SCENARIO_BATUPDATEONOFF				"setbatupdateonoff"
#define OAPI_SCENARIO_NITLCNUPDATEONOFF				"setnitlcnupdateonoff"
#define	OAPI_SCENARIO_LAST_SVC_GROUP				"lastsvcgroup"



#if 0		//	for JAPAN <-- NEXT TIME!!!
#define	OAPI_SCENARIO_LOCALAREA						"localarea"
#define	OAPI_SCENARIO_SUBTITLEMODE					"subtitlemode"
#define	OAPI_SCENARIO_SUPERIMPOSE					"superimpose"
#define	OAPI_SCENARIO_AUTOSWDOWNLOAD				"autoswdownload"
#define	OAPI_SCENARIO_CHANNELMASK					"channelmask"
#define	OAPI_SCENARIO_SEAMLESS						"seamless"
#define	OAPI_SCENARIO_AUTOPPVPURCHASE				"autoppvpurchase"
#define	OAPI_SCENARIO_LAUNCHMODE					"launchmode"
#define	OAPI_SCENARIO_DISPLAY3DMODE					"display3d"
#define	OAPI_SCENARIO_3DMODE						"3dmode"
#define	OAPI_SCENARIO_LIMITPERPPV					"limitperppv"
#define	OAPI_SCENARIO_LIMITPERPPM					"limitperppm"
#define	OAPI_SCENARIO_NAVIURL						"naviurl"
#define	OAPI_SCENARIO_INTERACTVURL					"interacttvurl"
#define	OAPI_SCENARIO_INTERNETBROWSERURL			"browserurl"
#define	OAPI_SCENARIO_VODHOMEURL					"vodhomeurl"
#define	OAPI_SCENARIO_ACSURL						"acsurl"
#define	OAPI_SCENARIO_PINUNLOCKEXPIREOPTION			"pinunlockexpireopt"
#define	OAPI_SCENARIO_AMPERMISSIONLEVEL				"ampermissionlevel"
#define	OAPI_SCENARIO_STANDBYMODE					"standbymode"
#define	OAPI_SCENARIO_BMLINFODISP					"bmlinfodisp"
#define	OAPI_SCENARIO_FRONTDISP						"frontdisp"
#define	OAPI_SCENARIO_DEFAULTDMS					"defdms"
#define	OAPI_SCENARIO_DEFAULTDMSDESTID				"defdmsdstid"
#define	OAPI_SCENARIO_DMSDELIVER					"dmsdeliver"
#define	OAPI_SCENARIO_CHANNELMASKPATTERN			"channelmaskpattern"
#define	OAPI_SCENARIO_DEFAULTFACTORYDAP				"deffactorydap"
#define OAPI_SCENARIO_DEFAULTDISKDAP				"defdiskdap"
#define	OAPI_SCENARIO_USERRESET						"userreset"
#define	OAPI_SCENARIO_STOPRFOTA						"stoprfota"
#define	OAPI_SCENARIO_FINALIZEIPOTA					"finalizeipota"
#define	OAPI_SCENARIO_DOCHANNELMASK					"dochannelmask"
#define	OAPI_SCENARIO_UPLOADPPVLOG					"ppvlogupdate"			//	0 : cancel, 1 : goon
#define	OAPI_SCENARIO_GETUPDATETIME					"systemupdatetime"
#define	OAPI_SCENARIO_SWUPDATESTATE					"swupdatestate"
#define	OAPI_SCENARIO_STARTSETTING					"startsetting"
#define	OAPI_SCENARIO_INSERTSETUP					"insertsetup"
#endif
/*******************************************************************/
/********************      Typedef         *************************/
/*******************************************************************/

/*******************************************************************/
/********************      Global Variables     ********************/
/*******************************************************************/

/*******************************************************************/
/********************      Functions     ********************/
/*******************************************************************/
HERROR	OAPI_SCENARIO_Init(void);
HERROR	OAPI_SCENARIO_DeInit(void);
HERROR	OAPI_SCENARIO_Set(const HCHAR *pszKey, const HCHAR *pszSignature, ...);

#endif // __OAPI_SCENARIO_H__



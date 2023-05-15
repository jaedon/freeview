/*******************************************************************
	File Description
********************************************************************/
/**
	@file	  _xsvc_si_spec_ukfsat.h
	@brief

	Description:  									\n
	Module: Porting Layer/TV Service/SI /OTA			\n

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

#ifndef ___INT_XSVC_SI_UKFSAT_H___
#define ___INT_XSVC_SI_UKFSAT_H___

/*******************************************************************/
/********************      Header Files    *************************/
/*******************************************************************/
#include <_svc_si.h>

/*******************************************************************/
/********************      Defines         *************************/
/*******************************************************************/
/* Freesat SPEC */
#define PRIVATE_DATA_SPECIFIER_FOR_UK_FSAT				(0x46534154)	/* FSAT */

/* Freesat SPEC */
enum
{
	FSAT_T_DATA_TYPE_EITPF = 0x00,
	FSAT_T_DATA_TYPE_EITSC,
	FSAT_T_DATA_TYPE_EITPFPP,
	FSAT_T_DATA_TYPE_SDT,
	FSAT_T_DATA_TYPE_BAT,
	FSAT_T_DATA_TYPE_TDT,
	FSAT_T_DATA_TYPE_TOT,
	FSAT_T_DATA_TYPE_NIT = 0x07,
	// Reserved(shall not be used) = 0x08,
	FSAT_T_DATA_TYPE_REC_VIDEO = 0x09,
	FSAT_T_DATA_TYPE_REC_AUDIO = 0x0A,
	FSAT_T_DATA_TYPE_POSTCODE_DB = 0x0B,

	// FREESAT GEN2 AddFilters
	FSAT_T_DATA_TYPE_RNT = 0x0C,
	FSAT_T_DATA_TYPE_EITCC = 0x0D,
	FSAT_T_DATA_TYPE_EITSCEXT = 0x0E,
	FSAT_T_DATA_TYPE_EITPFPPEXT = 0x0F,
//	Reserved(shall not be used) = 0x10 - 0x1F,

	FSAT_T_DATA_TYPE_END
};

#define FSAT_DEFAULT_POSTCODE_STR		"RH1 1XX"	/* ÀÎÁõ SuiteÀÇ ´ëºÎºÐ¿¡¼­ ÀÌ³ðÀÌ RegionId 1·Î ¸ÊÇÎµÊ */
#define FSAT_DEFAULT_HD_BOUQUET_ID		(272)
#define FSAT_DEFAULT_HD_REGION_ID		(50)
enum
{
	FSAT_SD_BOUQUET_ID = 0,
	FSAT_SD_REGION_ID,
	FSAT_HD_BOUQUET_ID,
	FSAT_HD_REGION_ID,
};

/*******************************************************************/
/********************      Typedef         *************************/
/*******************************************************************/

/*******************************************************************/
/********************      Global Variables     ********************/
/*******************************************************************/

/*******************************************************************/
/********************      Static Variables     ********************/
/*******************************************************************/

#endif // ___INT_XSVC_SI_UKFSAT_H___

/* end of file */

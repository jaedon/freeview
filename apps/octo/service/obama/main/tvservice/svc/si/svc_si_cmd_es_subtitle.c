/*******************************************************************
	File Description
********************************************************************/
/**
	@file	  svc_si_cmd_es_subtitle.c
	@brief

	Description:  									\n
	Module: SVC / SI_NEW					\n

	Copyright (c) 2013 HUMAX Co., Ltd.				\n
	All rights reserved.							\n
*/

/*
* Character encoding.
*
* MS949
*
* This source file that uses MS949 encoding.
* MS949 encoding 을 사용하는 source file 입니다.
* MS949 encodingを使用して source fileです。
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
#include <silib.h>
#include <vkernel.h>
#include <octo_common.h>
#include <isosvc.h>

#include <pal_sirawpool.h>
#include <pal_pipe.h>
//#include <mwc_util.h>
#include <svc_si.h>

#include <_svc_si.h>
#include <_svc_si_lib.h>
#include <_svc_si_filtering.h>
#include <_svc_si_search.h>
#include <_svc_si_ut.h>

#include "cutest.h"
#include "_svc_si_cmd.h"

/*******************************************************************/
/********************      Defines         *************************/
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

STATIC svcSi_SubtitleEs_t s_astSubtitleEsList[] =
{
	[0] = {
		.usPid					= 0x1000,
		.ucComponentTag			= 0,
		.ucType					= 0x01,		/* EBU, HOH:no, AR:n/a, HD:n/a */
		.aucLangCode			= { 'e', 'n', 'g' },
		.usCompositionPageId	= 0,
		.usAncillaryPageId		= 0,
		.ucMagazineNumber		= 0,
		.ucPageNumber			= 0,
		.bCaDesFound			= FALSE,
	},
	[1] = {
		.usPid					= 0x1001,
		.ucComponentTag 		= 0,
		.ucType 				= 0x02, 	/* EBU, HOH:no, AR:n/a, HD:n/a */
		.aucLangCode			= { 'e', 'n', 'g' },		/* eng */
		.usCompositionPageId	= 0,
		.usAncillaryPageId		= 0,
		.ucMagazineNumber		= 0,
		.ucPageNumber			= 0,
		.bCaDesFound			= FALSE,
	},
	[2] = {
		.usPid					= 0x1002,
		.ucComponentTag 		= 0,
		.ucType 				= 0x03, 	/* EBU, HOH:no, AR:n/a, HD:n/a */
		.aucLangCode			= { 'e', 'n', 'g' },		/* eng */
		.usCompositionPageId	= 0,
		.usAncillaryPageId		= 0,
		.ucMagazineNumber		= 0,
		.ucPageNumber			= 0,
		.bCaDesFound			= FALSE,
	},
	[3] = {
		.usPid					= 0x1003,
		.ucComponentTag 		= 0,
		.ucType 				= 0x05, 	/* EBU, HOH:yes, AR:n/a, HD:n/a */
		.aucLangCode			= { 'e', 'n', 'g' },		/* eng */
		.usCompositionPageId	= 0,
		.usAncillaryPageId		= 0,
		.ucMagazineNumber		= 0,
		.ucPageNumber			= 0,
		.bCaDesFound			= FALSE,
	},
	[4] = {
		.usPid					= 0x1004,
		.ucComponentTag 		= 0,
		.ucType 				= 0x10, 	/* DVB, HOH:no, AR:no, HD:no */
		.aucLangCode			= { 'e', 'n', 'g' },		/* eng */
		.usCompositionPageId	= 0,
		.usAncillaryPageId		= 0,
		.ucMagazineNumber		= 0,
		.ucPageNumber			= 0,
		.bCaDesFound			= FALSE,
	},
	[5] = {
		.usPid					= 0x1005,
		.ucComponentTag 		= 0,
		.ucType 				= 0x11, 	/* DVB, HOH:no, AR:4x3, HD:no */
		.aucLangCode			= { 'e', 'n', 'g' },		/* eng */
		.usCompositionPageId	= 0,
		.usAncillaryPageId		= 0,
		.ucMagazineNumber		= 0,
		.ucPageNumber			= 0,
		.bCaDesFound			= FALSE,
	},
	[6] = {
		.usPid					= 0x1006,
		.ucComponentTag 		= 0,
		.ucType 				= 0x12, 	/* DVB, HOH:no, AR:16x9, HD:no */
		.aucLangCode			= { 'e', 'n', 'g' },		/* eng */
		.usCompositionPageId	= 0,
		.usAncillaryPageId		= 0,
		.ucMagazineNumber		= 0,
		.ucPageNumber			= 0,
		.bCaDesFound			= FALSE,
	},
	[7] = {
		.usPid					= 0x1007,
		.ucComponentTag 		= 0,
		.ucType 				= 0x13, 	/* DVB, HOH:no, AR:2.21x1, HD:no */
		.aucLangCode			= { 'e', 'n', 'g' },		/* eng */
		.usCompositionPageId	= 0,
		.usAncillaryPageId		= 0,
		.ucMagazineNumber		= 0,
		.ucPageNumber			= 0,
		.bCaDesFound			= FALSE,
	},
	[8] = {
		.usPid					= 0x1008,
		.ucComponentTag 		= 0,
		.ucType 				= 0x14, 	/* DVB, HOH:no, AR:n/a, HD:yes */
		.aucLangCode			= { 'e', 'n', 'g' },		/* eng */
		.usCompositionPageId	= 0,
		.usAncillaryPageId		= 0,
		.ucMagazineNumber		= 0,
		.ucPageNumber			= 0,
		.bCaDesFound			= FALSE,
	},
	[9] = {
		.usPid					= 0x1009,
		.ucComponentTag 		= 0,
		.ucType 				= 0x20, 	/* DVB, HOH:yes, AR:no, HD:no */
		.aucLangCode			= { 'e', 'n', 'g' },		/* eng */
		.usCompositionPageId	= 0,
		.usAncillaryPageId		= 0,
		.ucMagazineNumber		= 0,
		.ucPageNumber			= 0,
		.bCaDesFound			= FALSE,
	},
	[10] = {
		.usPid					= 0x100A,
		.ucComponentTag 		= 0,
		.ucType 				= 0x21, 	/* DVB, HOH:yes, AR:4x3, HD:no */
		.aucLangCode			= { 'e', 'n', 'g' },		/* eng */
		.usCompositionPageId	= 0,
		.usAncillaryPageId		= 0,
		.ucMagazineNumber		= 0,
		.ucPageNumber			= 0,
		.bCaDesFound			= FALSE,
	},
	[11] = {
		.usPid					= 0x100B,
		.ucComponentTag 		= 0,
		.ucType 				= 0x22, 	/* DVB, HOH:yes, AR:16x9, HD:no */
		.aucLangCode			= { 'e', 'n', 'g' },		/* eng */
		.usCompositionPageId	= 0,
		.usAncillaryPageId		= 0,
		.ucMagazineNumber		= 0,
		.ucPageNumber			= 0,
		.bCaDesFound			= FALSE,
	},
	[12] = {
		.usPid					= 0x100C,
		.ucComponentTag 		= 0,
		.ucType 				= 0x23, 	/* DVB, HOH:yes, AR:2.21x1, HD:no */
		.aucLangCode			= { 'e', 'n', 'g' },		/* eng */
		.usCompositionPageId	= 0,
		.usAncillaryPageId		= 0,
		.ucMagazineNumber		= 0,
		.ucPageNumber			= 0,
		.bCaDesFound			= FALSE,
	},
	[13] = {
		.usPid					= 0x100D,
		.ucComponentTag 		= 0,
		.ucType 				= 0x24, 	/* DVB, HOH:yes, AR:n/a, HD:yes */
		.aucLangCode			= { 'e', 'n', 'g' },		/* eng */
		.usCompositionPageId	= 0,
		.usAncillaryPageId		= 0,
		.ucMagazineNumber		= 0,
		.ucPageNumber			= 0,
		.bCaDesFound			= FALSE,
	},
	[14] = {
		.usPid					= 0x100E,
		.ucComponentTag 		= 0,
		.ucType 				= 0x02, 	/* EBU, HOH:no, AR:n/a, HD:n/a */
		.aucLangCode			= { 'n', 'o', 'r' },		/* nor */
		.usCompositionPageId	= 0,
		.usAncillaryPageId		= 0,
		.ucMagazineNumber		= 0,
		.ucPageNumber			= 0,
		.bCaDesFound			= FALSE,
	},
	[15] = {
		.usPid					= 0x100F,
		.ucComponentTag 		= 0,
		.ucType 				= 0x10, 	/* DVB, HOH:no, AR:no, HD:no */
		.aucLangCode			= { 'n', 'o', 'r' },		/* nor */
		.usCompositionPageId	= 0,
		.usAncillaryPageId		= 0,
		.ucMagazineNumber		= 0,
		.ucPageNumber			= 0,
		.bCaDesFound			= FALSE,
	},
	[16] = {
		.usPid					= 0x1010,
		.ucComponentTag 		= 0,
		.ucType 				= 0x11, 	/* DVB, HOH:no, AR:4x3, HD:no */
		.aucLangCode			= { 'n', 'o', 'r' },		/* nor */
		.usCompositionPageId	= 0,
		.usAncillaryPageId		= 0,
		.ucMagazineNumber		= 0,
		.ucPageNumber			= 0,
		.bCaDesFound			= FALSE,
	},
	[17] = {
		.usPid					= 0x1011,
		.ucComponentTag 		= 0,
		.ucType 				= 0x20, 	/* DVB, HOH:yes, AR:no, HD:no */
		.aucLangCode			= { 'n', 'o', 'r' },		/* nor */
		.usCompositionPageId	= 0,
		.usAncillaryPageId		= 0,
		.ucMagazineNumber		= 0,
		.ucPageNumber			= 0,
		.bCaDesFound			= FALSE,
	},
	[18] = {
		.usPid					= 0x1012,
		.ucComponentTag 		= 0,
		.ucType 				= 0x21, 	/* DVB, HOH:yes, AR:4x3, HD:no */
		.aucLangCode			= { 'n', 'o', 'r' },		/* nor */
		.usCompositionPageId	= 0,
		.usAncillaryPageId		= 0,
		.ucMagazineNumber		= 0,
		.ucPageNumber			= 0,
		.bCaDesFound			= FALSE,
	},
	[19] = {
		.usPid					= 0x1013,
		.ucComponentTag 		= 0,
		.ucType 				= 0x02, 	/* EBU, HOH:no, AR:n/a, HD:n/a */
		.aucLangCode			= { 'd', 'a', 'n' },		/* dan */
		.usCompositionPageId	= 0,
		.usAncillaryPageId		= 0,
		.ucMagazineNumber		= 0,
		.ucPageNumber			= 0,
		.bCaDesFound			= FALSE,
	},
	[20] = {
		.usPid					= 0x1014,
		.ucComponentTag 		= 0,
		.ucType 				= 0x10, 	/* DVB, HOH:no, AR:no, HD:no */
		.aucLangCode			= { 'd', 'a', 'n' },		/* dan */
		.usCompositionPageId	= 0,
		.usAncillaryPageId		= 0,
		.ucMagazineNumber		= 0,
		.ucPageNumber			= 0,
		.bCaDesFound			= FALSE,
	},
	[21] = {
		.usPid					= 0x1015,
		.ucComponentTag 		= 0,
		.ucType 				= 0x11, 	/* DVB, HOH:no, AR:4x3, HD:no */
		.aucLangCode			= { 'd', 'a', 'n' },		/* dan */
		.usCompositionPageId	= 0,
		.usAncillaryPageId		= 0,
		.ucMagazineNumber		= 0,
		.ucPageNumber			= 0,
		.bCaDesFound			= FALSE,
	},
	[22] = {
		.usPid					= 0x1016,
		.ucComponentTag 		= 0,
		.ucType 				= 0x20, 	/* DVB, HOH:yes, AR:no, HD:no */
		.aucLangCode			= { 'd', 'a', 'n' },		/* dan */
		.usCompositionPageId	= 0,
		.usAncillaryPageId		= 0,
		.ucMagazineNumber		= 0,
		.ucPageNumber			= 0,
		.bCaDesFound			= FALSE,
	},
	[23] = {
		.usPid					= 0x1017,
		.ucComponentTag 		= 0,
		.ucType 				= 0x21, 	/* DVB, HOH:yes, AR:4x3, HD:no */
		.aucLangCode			= { 'd', 'a', 'n' },		/* dan */
		.usCompositionPageId	= 0,
		.usAncillaryPageId		= 0,
		.ucMagazineNumber		= 0,
		.ucPageNumber			= 0,
		.bCaDesFound			= FALSE,
	},
	[24] = {
		.usPid					= 0x1018,
		.ucComponentTag 		= 0,
		.ucType 				= 0x02, 	/* EBU, HOH:no, AR:n/a, HD:n/a */
		.aucLangCode			= { 's', 'w', 'e' },		/* swe */
		.usCompositionPageId	= 0,
		.usAncillaryPageId		= 0,
		.ucMagazineNumber		= 0,
		.ucPageNumber			= 0,
		.bCaDesFound			= FALSE,
	},
	[25] = {
		.usPid					= 0x1019,
		.ucComponentTag 		= 0,
		.ucType 				= 0x10, 	/* DVB, HOH:no, AR:no, HD:no */
		.aucLangCode			= { 's', 'w', 'e' },		/* swe */
		.usCompositionPageId	= 0,
		.usAncillaryPageId		= 0,
		.ucMagazineNumber		= 0,
		.ucPageNumber			= 0,
		.bCaDesFound			= FALSE,
	},
	[26] = {
		.usPid					= 0x101A,
		.ucComponentTag 		= 0,
		.ucType 				= 0x11, 	/* DVB, HOH:no, AR:4x3, HD:no */
		.aucLangCode			= { 's', 'w', 'e' },		/* swe */
		.usCompositionPageId	= 0,
		.usAncillaryPageId		= 0,
		.ucMagazineNumber		= 0,
		.ucPageNumber			= 0,
		.bCaDesFound			= FALSE,
	},
	[27] = {
		.usPid					= 0x101B,
		.ucComponentTag 		= 0,
		.ucType 				= 0x20, 	/* DVB, HOH:yes, AR:no, HD:no */
		.aucLangCode			= { 's', 'w', 'e' },		/* swe */
		.usCompositionPageId	= 0,
		.usAncillaryPageId		= 0,
		.ucMagazineNumber		= 0,
		.ucPageNumber			= 0,
		.bCaDesFound			= FALSE,
	},
	[28] = {
		.usPid					= 0x101C,
		.ucComponentTag 		= 0,
		.ucType 				= 0x21, 	/* DVB, HOH:yes, AR:4x3, HD:no */
		.aucLangCode			= { 's', 'w', 'e' },		/* swe */
		.usCompositionPageId	= 0,
		.usAncillaryPageId		= 0,
		.ucMagazineNumber		= 0,
		.ucPageNumber			= 0,
		.bCaDesFound			= FALSE,
	},
	[29] = {
		.usPid					= 0x101D,
		.ucComponentTag 		= 0,
		.ucType 				= 0x02, 	/* EBU, HOH:no, AR:n/a, HD:n/a */
		.aucLangCode			= { 'f', 'i', 'n' },		/* fin */
		.usCompositionPageId	= 0,
		.usAncillaryPageId		= 0,
		.ucMagazineNumber		= 0,
		.ucPageNumber			= 0,
		.bCaDesFound			= FALSE,
	},
	[30] = {
		.usPid					= 0x101E,
		.ucComponentTag 		= 0,
		.ucType 				= 0x10, 	/* DVB, HOH:no, AR:no, HD:no */
		.aucLangCode			= { 'f', 'i', 'n' },		/* fin */
		.usCompositionPageId	= 0,
		.usAncillaryPageId		= 0,
		.ucMagazineNumber		= 0,
		.ucPageNumber			= 0,
		.bCaDesFound			= FALSE,
	},
	[31] = {
		.usPid					= 0x101F,
		.ucComponentTag 		= 0,
		.ucType 				= 0x11, 	/* DVB, HOH:no, AR:4x3, HD:no */
		.aucLangCode			= { 'f', 'i', 'n' },		/* fin */
		.usCompositionPageId	= 0,
		.usAncillaryPageId		= 0,
		.ucMagazineNumber		= 0,
		.ucPageNumber			= 0,
		.bCaDesFound			= FALSE,
	},
	[32] = {
		.usPid					= 0x1020,
		.ucComponentTag 		= 0,
		.ucType 				= 0x20, 	/* DVB, HOH:yes, AR:no, HD:no */
		.aucLangCode			= { 'f', 'i', 'n' },		/* fin */
		.usCompositionPageId	= 0,
		.usAncillaryPageId		= 0,
		.ucMagazineNumber		= 0,
		.ucPageNumber			= 0,
		.bCaDesFound			= FALSE,
	},
	[33] = {
		.usPid					= 0x1021,
		.ucComponentTag 		= 0,
		.ucType 				= 0x21, 	/* DVB, HOH:yes, AR:4x3, HD:no */
		.aucLangCode			= { 'f', 'i', 'n' },		/* fin */
		.usCompositionPageId	= 0,
		.usAncillaryPageId		= 0,
		.ucMagazineNumber		= 0,
		.ucPageNumber			= 0,
		.bCaDesFound			= FALSE,
	},
	[34] = {
		.usPid					= 0x1022,
		.ucComponentTag 		= 0,
		.ucType 				= 0x02, 	/* EBU, HOH:no, AR:n/a, HD:n/a */
		.aucLangCode			= { 'r', 'u', 's' },		/* rus */
		.usCompositionPageId	= 0,
		.usAncillaryPageId		= 0,
		.ucMagazineNumber		= 0,
		.ucPageNumber			= 0,
		.bCaDesFound			= FALSE,
	},
	[35] = {
		.usPid					= 0x1023,
		.ucComponentTag 		= 0,
		.ucType 				= 0x10, 	/* DVB, HOH:no, AR:no, HD:no */
		.aucLangCode			= { 'r', 'u', 's' },		/* rus */
		.usCompositionPageId	= 0,
		.usAncillaryPageId		= 0,
		.ucMagazineNumber		= 0,
		.ucPageNumber			= 0,
		.bCaDesFound			= FALSE,
	},
	[36] = {
		.usPid					= 0x1024,
		.ucComponentTag 		= 0,
		.ucType 				= 0x11, 	/* DVB, HOH:no, AR:4x3, HD:no */
		.aucLangCode			= { 'r', 'u', 's' },		/* rus */
		.usCompositionPageId	= 0,
		.usAncillaryPageId		= 0,
		.ucMagazineNumber		= 0,
		.ucPageNumber			= 0,
		.bCaDesFound			= FALSE,
	},
	[37] = {
		.usPid					= 0x1025,
		.ucComponentTag 		= 0,
		.ucType 				= 0x20, 	/* DVB, HOH:yes, AR:no, HD:no */
		.aucLangCode			= { 'r', 'u', 's' },		/* rus */
		.usCompositionPageId	= 0,
		.usAncillaryPageId		= 0,
		.ucMagazineNumber		= 0,
		.ucPageNumber			= 0,
		.bCaDesFound			= FALSE,
	},
	[38] = {
		.usPid					= 0x1026,
		.ucComponentTag 		= 0,
		.ucType 				= 0x21, 	/* DVB, HOH:yes, AR:4x3, HD:no */
		.aucLangCode			= { 'r', 'u', 's' },		/* rus */
		.usCompositionPageId	= 0,
		.usAncillaryPageId		= 0,
		.ucMagazineNumber		= 0,
		.ucPageNumber			= 0,
		.bCaDesFound			= FALSE,
	},
	[39] = {
		.usPid					= 0x1027,
		.ucComponentTag 		= 0,
		.ucType 				= 0x02, 	/* EBU, HOH:no, AR:n/a, HD:n/a */
		.aucLangCode			= { 'q', 'a', 'a' },		/* qaa */
		.usCompositionPageId	= 0,
		.usAncillaryPageId		= 0,
		.ucMagazineNumber		= 0,
		.ucPageNumber			= 0,
		.bCaDesFound			= FALSE,
	},
	[40] = {
		.usPid					= 0x1028,
		.ucComponentTag 		= 0,
		.ucType 				= 0x10, 	/* DVB, HOH:no, AR:no, HD:no */
		.aucLangCode			= { 'q', 'a', 'a' },		/* qaa */
		.usCompositionPageId	= 0,
		.usAncillaryPageId		= 0,
		.ucMagazineNumber		= 0,
		.ucPageNumber			= 0,
		.bCaDesFound			= FALSE,
	},
	[41] = {
		.usPid					= 0x1029,
		.ucComponentTag 		= 0,
		.ucType 				= 0x11, 	/* DVB, HOH:no, AR:4x3, HD:no */
		.aucLangCode			= { 'q', 'a', 'a' },		/* qaa */
		.usCompositionPageId	= 0,
		.usAncillaryPageId		= 0,
		.ucMagazineNumber		= 0,
		.ucPageNumber			= 0,
		.bCaDesFound			= FALSE,
	},
	[42] = {
		.usPid					= 0x102A,
		.ucComponentTag 		= 0,
		.ucType 				= 0x20, 	/* DVB, HOH:yes, AR:no, HD:no */
		.aucLangCode			= { 'q', 'a', 'a' },		/* qaa */
		.usCompositionPageId	= 0,
		.usAncillaryPageId		= 0,
		.ucMagazineNumber		= 0,
		.ucPageNumber			= 0,
		.bCaDesFound			= FALSE,
	},
	[43] = {
		.usPid					= 0x102B,
		.ucComponentTag 		= 0,
		.ucType 				= 0x21, 	/* DVB, HOH:yes, AR:4x3, HD:no */
		.aucLangCode			= { 'q', 'a', 'a' },		/* qaa */
		.usCompositionPageId	= 0,
		.usAncillaryPageId		= 0,
		.ucMagazineNumber		= 0,
		.ucPageNumber			= 0,
		.bCaDesFound			= FALSE,
	},
	[44] = {
		.usPid					= 0x102C,
		.ucComponentTag 		= 0,
		.ucType 				= 0x10, 	/* DVB, HOH:no, AR:no, HD:no */
		.aucLangCode			= { 'u', 'n', 'd' },		/* und */
		.usCompositionPageId	= 0,
		.usAncillaryPageId		= 0,
		.ucMagazineNumber		= 0,
		.ucPageNumber			= 0,
		.bCaDesFound			= FALSE,
	},
	[45] = {
		.usPid					= 0x102D,
		.ucComponentTag 		= 0,
		.ucType 				= 0x11, 	/* DVB, HOH:no, AR:4x3, HD:no */
		.aucLangCode			= { 'u', 'n', 'd' },		/* und */
		.usCompositionPageId	= 0,
		.usAncillaryPageId		= 0,
		.ucMagazineNumber		= 0,
		.ucPageNumber			= 0,
		.bCaDesFound			= FALSE,
	},
	[46] = {
		.usPid					= 0x102E,
		.ucComponentTag 		= 0,
		.ucType 				= 0x20, 	/* DVB, HOH:yes, AR:no, HD:no */
		.aucLangCode			= { 'u', 'n', 'd' },		/* und */
		.usCompositionPageId	= 0,
		.usAncillaryPageId		= 0,
		.ucMagazineNumber		= 0,
		.ucPageNumber			= 0,
		.bCaDesFound			= FALSE,
	},
	[47] = {
		.usPid					= 0x102F,
		.ucComponentTag 		= 0,
		.ucType 				= 0x21, 	/* DVB, HOH:yes, AR:4x3, HD:no */
		.aucLangCode			= { 'u', 'n', 'd' },		/* und */
		.usCompositionPageId	= 0,
		.usAncillaryPageId		= 0,
		.ucMagazineNumber		= 0,
		.ucPageNumber			= 0,
		.bCaDesFound			= FALSE,
	},
	[48] = {
		.usPid					= 0x1030,
		.ucComponentTag 		= 0,
		.ucType 				= 0x02, 	/* EBU, HOH:no, AR:n/a, HD:n/a */
		.aucLangCode			= { 0, },		/*	*/
		.usCompositionPageId	= 0,
		.usAncillaryPageId		= 0,
		.ucMagazineNumber		= 0,
		.ucPageNumber			= 0,
		.bCaDesFound			= FALSE,
	},
	[49] = {
		.usPid					= 0x1031,
		.ucComponentTag 		= 0,
		.ucType 				= 0x10, 	/* DVB, HOH:no, AR:no, HD:no */
		.aucLangCode			= { 0, },		/*	*/
		.usCompositionPageId	= 0,
		.usAncillaryPageId		= 0,
		.ucMagazineNumber		= 0,
		.ucPageNumber			= 0,
		.bCaDesFound			= FALSE,
	},
	[50] = {
		.usPid					= 0x1032,
		.ucComponentTag 		= 0,
		.ucType 				= 0x11, 	/* DVB, HOH:no, AR:4x3, HD:no */
		.aucLangCode			= { 0, },		/*	*/
		.usCompositionPageId	= 0,
		.usAncillaryPageId		= 0,
		.ucMagazineNumber		= 0,
		.ucPageNumber			= 0,
		.bCaDesFound			= FALSE,
	},
	[51] = {
		.usPid					= 0x1033,
		.ucComponentTag 		= 0,
		.ucType 				= 0x20, 	/* DVB, HOH:yes, AR:no, HD:no */
		.aucLangCode			= { 0, },		/*	*/
		.usCompositionPageId	= 0,
		.usAncillaryPageId		= 0,
		.ucMagazineNumber		= 0,
		.ucPageNumber			= 0,
		.bCaDesFound			= FALSE,
	},
	[52] = {
		.usPid					= 0x1034,
		.ucComponentTag 		= 0,
		.ucType 				= 0x21, 	/* DVB, HOH:yes, AR:4x3, HD:no */
		.aucLangCode			= { 0, },		/*	*/
		.usCompositionPageId	= 0,
		.usAncillaryPageId		= 0,
		.ucMagazineNumber		= 0,
		.ucPageNumber			= 0,
		.bCaDesFound			= FALSE,
	}
};


#define ___LOCAL_FUNCTIONS___

/**********************************************
	Command 수행 함수를 등록하는 통합 등록 함수.
***********************************************/

STATIC HUINT32 svc_si_UtSubtitleEsGetEsTableItemNum(void)
{
	return sizeof(s_astSubtitleEsList) / sizeof(svcSi_SubtitleEs_t);
}

STATIC svcSi_SubtitleEs_t* svc_si_UtSubtitleEsGetEsTableItem(HUINT32 ulIdx)
{
	if (svc_si_UtSubtitleEsGetEsTableItemNum() <= ulIdx)
		return NULL;

	return &s_astSubtitleEsList[ulIdx];
}

STATIC void svc_si_UtSubtitleEsPrintSimple(svcSi_SubtitleEs_t *pstEs, HUINT32 ulIndex)
{
#if defined(CONFIG_DEBUG)
	HxLOG_Print("  [%02d] : Pid(0x%04x), Type(0x%02x), Lang([%3s]), CompTag(0x%02x), CompPageId(0x%04x), AncPageId(0x%04x), MagNum(0x%02x), PageNum(0x%02x), CaDesFound(%s)\n",
				ulIndex,
				pstEs->usPid,					pstEs->ucType,
				pstEs->aucLangCode,				pstEs->ucComponentTag,
				pstEs->usCompositionPageId,		pstEs->usAncillaryPageId,
				pstEs->ucMagazineNumber,		pstEs->ucPageNumber,
				pstEs->bCaDesFound ? "TRUE" : "FALSE");
#endif
}

STATIC HBOOL svc_si_UtSubtitleEsPrintInfo(void *pvData, void *pvUserData)
{
	svcSi_SubtitleEs_t 	*pstEs = (svcSi_SubtitleEs_t *)pvData;
	HUINT32				*pulNum = (HUINT32 *)pvUserData;

	if (NULL == pstEs)
		return FALSE;

	if (NULL == pulNum)
		return FALSE;

	svc_si_UtSubtitleEsPrintSimple(pstEs, (*pulNum)++);

	return FALSE;
}

STATIC void svc_si_UtSubtitleEsPrintList(HxList_t *pstEsList)
{
#if defined(CONFIG_DEBUG) //&& 0
	if (pstEsList)
	{
		HUINT32 ulCount;

		HxLOG_Print("=================================================================\n");
		HxLOG_Print("== ES List (%02d) =================================================\n", HLIB_LIST_Length(pstEsList));
		HxLOG_Print("=================================================================\n");

		ulCount = 0;
		HLIB_LIST_ForeachEx(pstEsList, svc_si_UtSubtitleEsPrintInfo, (void *)&ulCount);

		HxLOG_Print("=================================================================\n");
	}
#endif
}


STATIC HERROR svc_si_UtSubtitleEsSetPriorityTable(void)
{
	STATIC SvcSi_EsTypePriority_t s_astPriorityTable_Ut[] =
	{
		{	eSvcSi_SUBTTL_PRIORITY_LANG_BASE,					eSvcSi_SUBTTL_SEL_TYPE_NUM,							10000	},
		{	eSvcSi_SUBTTL_PRIORITY_LANG_USER,					eSvcSi_SUBTTL_SEL_LANG_USER,						80000	},
		{	eSvcSi_SUBTTL_PRIORITY_LANG_USER_WITH_IDX,			eSvcSi_SUBTTL_SEL_LANG_USER_WITH_IDX,				90000	},
		{	eSvcSi_SUBTTL_PRIORITY_LANG_MENU_0,					eSvcSi_SUBTTL_SEL_LANG_0,							70000	},
		{	eSvcSi_SUBTTL_PRIORITY_LANG_MENU_1,					eSvcSi_SUBTTL_SEL_LANG_1,							60000	},
		{	eSvcSi_SUBTTL_PRIORITY_LANG_OSD,					eSvcSi_SUBTTL_SEL_LANG_OSD,							50000	},
		{	eSvcSi_SUBTTL_PRIORITY_LANG_UNDEFINED,				eSvcSi_SUBTTL_SEL_LANG_UNDEFINED,					40000	},
		{	eSvcSi_SUBTTL_PRIORITY_LANG_ORIGINAL,				eSvcSi_SUBTTL_SEL_LANG_ORIGINAL,					30000	},
		{	eSvcSi_SUBTTL_PRIORITY_LANG_DEFAULT,				eSvcSi_SUBTTL_SEL_LANG_DEFAULT,						20000	},
		{	eSvcSi_SUBTTL_PRIORITY_TYPE_BASE,					eSvcSi_SUBTTL_SEL_TYPE_NUM,							1000	},
		{	eSvcSi_SUBTTL_PRIORITY_TYPE_DVB_MATCHED_USER,		eSvcSi_SUBTTL_SEL_TYPE_DVB_USER, 					7000	},
		{	eSvcSi_SUBTTL_PRIORITY_TYPE_DVB_MATCHED_MENU,		eSvcSi_SUBTTL_SEL_TYPE_DVB_MENU, 					6000	},
		{	eSvcSi_SUBTTL_PRIORITY_TYPE_DVB_MATCHED_AUTO,		eSvcSi_SUBTTL_SEL_TYPE_DVB_AUTO, 					5000	},
		{	eSvcSi_SUBTTL_PRIORITY_TYPE_EBU_MATCHED_USER,		eSvcSi_SUBTTL_SEL_TYPE_EBU_USER, 					4000	},
		{	eSvcSi_SUBTTL_PRIORITY_TYPE_EBU_MATCHED_MENU,		eSvcSi_SUBTTL_SEL_TYPE_EBU_MENU, 					3000	},
		{	eSvcSi_SUBTTL_PRIORITY_TYPE_EBU_MATCHED_AUTO,		eSvcSi_SUBTTL_SEL_TYPE_EBU_AUTO, 					2000	},
		{	eSvcSi_SUBTTL_PRIORITY_HOH_BASE,					eSvcSi_SUBTTL_SEL_TYPE_NUM,							100000	},
		{	eSvcSi_SUBTTL_PRIORITY_HOH_MATCHED_USER,			eSvcSi_SUBTTL_SEL_HOH_USER,							400000	},
		{	eSvcSi_SUBTTL_PRIORITY_HOH_MATCHED_MENU,			eSvcSi_SUBTTL_SEL_HOH_MENU,							300000	},
		{	eSvcSi_SUBTTL_PRIORITY_HOH_OFF_ES_YES,				eSvcSi_SUBTTL_SEL_HOH_OFF_ES_YES,					100000	},
		{	eSvcSi_SUBTTL_PRIORITY_HOH_OFF_ES_NO,				eSvcSi_SUBTTL_SEL_HOH_OFF_ES_NO,					200000	},
		{	eSvcSi_SUBTTL_PRIORITY_HOH_ON_ES_NO,				eSvcSi_SUBTTL_SEL_HOH_ON_ES_NO,						200000	},
		{	eSvcSi_SUBTTL_PRIORITY_ASPECTRATIO_BASE,			eSvcSi_SUBTTL_SEL_TYPE_NUM,							10		},
		{	eSvcSi_SUBTTL_PRIORITY_ASPECTRATIO_NA,				eSvcSi_SUBTTL_SEL_ASPECTRATIO_NA,					20		},
		{	eSvcSi_SUBTTL_PRIORITY_ASPECTRATIO_MATCHED_YES,		eSvcSi_SUBTTL_SEL_ASPECTRATIO_YES,					30		},
		{	eSvcSi_SUBTTL_PRIORITY_HD_BASE,						eSvcSi_SUBTTL_SEL_TYPE_NUM,							100		},
		{	eSvcSi_SUBTTL_PRIORITY_HD_NA,						eSvcSi_SUBTTL_SEL_HD_NA,							300		},
		{	eSvcSi_SUBTTL_PRIORITY_HD_MATCHED_USER,				eSvcSi_SUBTTL_SEL_HD_USER,							400 	},
		{	eSvcSi_SUBTTL_PRIORITY_HD_MATCHED_MENU,				eSvcSi_SUBTTL_SEL_HD_MENU,							500 	},
		{	eSvcSi_SUBTTL_PRIORITY_FIRST_TRACK_BASE,			eSvcSi_SUBTTL_SEL_TYPE_NUM,							0		},
		{	eSvcSi_SUBTTL_PRIORITY_FIRST_TRACK_YES,				eSvcSi_SUBTTL_SEL_TRACK,							1		},
		{	eSvcSi_SUBTTL_PRIORITY_FIRST_TRACK_NO, 				eSvcSi_SUBTTL_SEL_TYPE_NUM,							0		},
		{	eSvcSi_SUBTTL_PRIORITY_TYPE_NUM,					-1,													0		}
	};
	HUINT32	 ulNum;

	ulNum = sizeof(s_astPriorityTable_Ut) / sizeof(SvcSi_EsTypePriority_t);

	return SVC_SI_SetSubtitlePriorityTable (s_astPriorityTable_Ut, ulNum);
}

STATIC void svc_si_UtSubtitleEsSetScenario(Cutest_TestCase_t *pstTc)
{
	HERROR	hErr;
	HUINT8	szPrintBuf[svcSi_CMD_STR_BUFF_SIZE] = {0, };

	hErr = svc_si_UtSubtitleEsSetPriorityTable();
	if(hErr != ERR_OK)
	{
		HxSTD_snprintf((char *)&szPrintBuf, svcSi_CMD_STR_BUFF_SIZE, (const char *)"Fail to Get Subtitle Select Priority Table for this Test : err[0x%x]", hErr);
		CutestFail(pstTc, (const char *)&szPrintBuf);
	}
}

const HxList_t *svc_si_UtSubtitleEsGetInfo(HUINT32 ulEsNum, ...)
{
	va_list				 ap;
	HUINT32 			 aulTrackList[32] = { 0, };
	HxList_t			*pstList = NULL;
	svcSi_SubtitleEs_t	*pstEs, *pstEsSource;
	HUINT32				 ulEsTableItemNum;
	HUINT32 			 ulCount, ulTrack;
	HUINT32 			 ulItemNum;

    va_start(ap, ulEsNum);
	ulItemNum = svc_si_UtSetTrackIdx(aulTrackList, ulEsNum, svc_si_UtSubtitleEsGetEsTableItemNum()-1, ap);
	va_end(ap);

	if (32 <= ulItemNum)
	{
		HxLOG_Warning("32 <= ulItemNum\n");
		return NULL;
	}

	if (ulEsNum < ulItemNum)
		ulItemNum = ulEsNum;

	if (ulItemNum > 0)
	{
		ulEsTableItemNum = svc_si_UtSubtitleEsGetEsTableItemNum();
		for (ulCount=0; ulCount<ulItemNum; ulCount++)
		{
			ulTrack = aulTrackList[ulCount];
			if (ulTrack >= ulEsTableItemNum)	continue;

			pstEsSource = svc_si_UtSubtitleEsGetEsTableItem(ulTrack);
			if (pstEsSource)
			{
				pstEs = OxSI_MemDup(pstEsSource, sizeof(svcSi_SubtitleEs_t));
				if (pstEs)
				{
					pstList = HLIB_LIST_Append(pstList, (void *)pstEs);
				}
			}
		}

		svc_si_UtSubtitleEsPrintList(pstList);
	}

	return (const HxList_t *)pstList;
}

STATIC const svcSi_PmtEsInfo_t *svc_si_UtSubtitleEsSetList(svcSi_PmtEsInfo_t *pstPmtEsInfo, const HxList_t *pstSubtitleList)
{
	return svc_si_UtSetEsListToPmtEsInfo(pstPmtEsInfo, eEsType_Subtitle, pstSubtitleList);
}

STATIC void svc_si_UtSubtitleEsVeritySelectOutput(Cutest_TestCase_t		*pstTc,
													HERROR				 hErr,
													SvcSi_SelectedEs_t	*pstSelectedEs,
													HINT32				 nExpectedTrackIdx)
{
	svc_si_UtVeritySelectSubtitleOutput(pstTc, hErr,
									pstSelectedEs, nExpectedTrackIdx,
									"svc_si_LibSvcEsSelectSubtitle() fail",
									0, 0, 0);
}

STATIC void	svc_si_UtSubtitleEsVeritySelectResult(Cutest_TestCase_t		*pstTc,
													const HCHAR			*pszFuncName,
													const HCHAR			*pszResultType,
													HINT32				 nActualResult,
													HINT32				 nExpectedResult)
{
	svc_si_UtVeritySelectEsResult(pstTc, pszFuncName, pszResultType,
									nActualResult, nExpectedResult);
}

STATIC const SvcSi_Config_t *svc_si_UtSubtitleEsGetConfig(const HUINT8 szLang0[], const HUINT8 szLang1[], const HUINT8 szOsdLang[],
															const HUINT8 szLangDefault[],
															DxAspectRatio_e eAspectRatio,
															HBOOL bDvbMenu, HBOOL bHohMenu,
															const HUINT8 szLangUser[], HBOOL bHohUser)
{
	HERROR			 hErr;
	SvcSi_Config_t	*pstConfig;

	hErr = svc_si_UtSubtitleEsSetPriorityTable();
	if(hErr != ERR_OK)
	{
		HxLOG_Warning("svc_si_UtSubtitleEsSetPriorityTable() Error!\n");
		return NULL;
	}

	pstConfig = OxSI_Malloc(sizeof(SvcSi_Config_t));
	if (pstConfig)
	{
		HxSTD_MemSet(pstConfig, 0x00, sizeof(SvcSi_Config_t));

		pstConfig->stSystem.eOperator		= eDxOPERATOR_None;
		pstConfig->stSystem.eCountry		= eCountryID_GBR;
		pstConfig->stSystem.eAspectRatio	= eAspectRatio;
		pstConfig->stSystem.eOsdLang		= svcSi_UtGetLangId(szOsdLang);

		pstConfig->stSubtitleLangauge.aeLangId[eSvcSi_CFG_SUBTTL_LANG_User]			= svcSi_UtGetLangId(szLangUser);
		pstConfig->stSubtitleLangauge.aeLangId[eSvcSi_CFG_SUBTTL_LANG_User_With_Idx]= svcSi_UtGetLangId(szLangUser);
		pstConfig->stSubtitleLangauge.aeLangId[eSvcSi_CFG_SUBTTL_LANG_0]			= svcSi_UtGetLangId(szLang0);
		pstConfig->stSubtitleLangauge.aeLangId[eSvcSi_CFG_SUBTTL_LANG_1]			= svcSi_UtGetLangId(szLang1);
		pstConfig->stSubtitleLangauge.aeLangId[eSvcSi_CFG_SUBTTL_LANG_Undefined]	= eLangID_Undefined;
		pstConfig->stSubtitleLangauge.aeLangId[eSvcSi_CFG_SUBTTL_LANG_Original]		= eLangID_OriginalAudio;
		pstConfig->stSubtitleLangauge.aeLangId[eSvcSi_CFG_SUBTTL_LANG_Default]		= svcSi_UtGetLangId(szLangDefault);

		pstConfig->stSubtitleType.abDvb[eSvcSi_CFG_SUBTTL_DVB_User]	= FALSE;
		pstConfig->stSubtitleType.abDvb[eSvcSi_CFG_SUBTTL_DVB_Menu]	= bDvbMenu;

		pstConfig->stSubtitleType.abHoh[eSvcSi_CFG_SUBTTL_HOH_User]	= bHohUser;
		pstConfig->stSubtitleType.abHoh[eSvcSi_CFG_SUBTTL_HOH_Menu]	= bHohMenu;

		pstConfig->stSubtitleType.abHD[eSvcSi_CFG_SUBTTL_HD_User] = FALSE;
		pstConfig->stSubtitleType.abHD[eSvcSi_CFG_SUBTTL_HD_Menu] = TRUE;
	}

	svc_si_UtPrintConfig((const SvcSi_Config_t *)pstConfig, FALSE, TRUE);

	return (const SvcSi_Config_t *)pstConfig;
}

STATIC HERROR svc_si_UtSubtitleEsCheckParam(Cutest_TestCase_t *pstTc, const svcSi_PmtEsInfo_t *pstPmtEsInfo, const SvcSi_Config_t *pstConfig, SvcSi_SelectedEs_t *pstSelected)
{
	HUINT8	szPrintBuf[svcSi_CMD_STR_BUFF_SIZE] = {0, };

	if (NULL == pstPmtEsInfo)
	{
		HxSTD_snprintf((char *)&szPrintBuf, svcSi_CMD_STR_BUFF_SIZE, (const char *)"SOURCE NULL : (NULL == pstPmtEsInfo)");
		CutestFail(pstTc, (const char *)&szPrintBuf);
		return ERR_FAIL;
	}

	if (NULL == pstConfig)
	{
		HxSTD_snprintf((char *)&szPrintBuf, svcSi_CMD_STR_BUFF_SIZE, (const char *)"SOURCE NULL : (NULL == pstConfig)");
		CutestFail(pstTc, (const char *)&szPrintBuf);
		return ERR_FAIL;
	}

	if (NULL == pstSelected)
	{
		HxSTD_snprintf((char *)&szPrintBuf, svcSi_CMD_STR_BUFF_SIZE, (const char *)"TARGET NULL : (NULL == pstSelected)");
		CutestFail(pstTc, (const char *)&szPrintBuf);
		return ERR_FAIL;
	}

	return ERR_OK;
}

STATIC HERROR svc_si_UtSubtitleEsSelect (Cutest_TestCase_t *pstTc, HBOOL bCheckParam, const svcSi_PmtEsInfo_t *pstPmtEsInfo, const SvcSi_Config_t *pstConfig, SvcSi_SelectedEs_t *pstSelected)
{
	HERROR	hErr = ERR_OK;

	if (bCheckParam)
	{
		hErr = svc_si_UtSubtitleEsCheckParam (pstTc, pstPmtEsInfo, pstConfig, pstSelected);
	}

	if (ERR_OK != hErr)
	{
		return hErr;
	}

	return svc_si_LibSvcEsSelectSubtitle(pstPmtEsInfo, pstConfig, pstSelected);
}

#define ___ES_SUBTITLE_TEST_CASE___

/*	SUT 			: svc_si_UtSubtitleEsSelect
 *	Expected Result : ERR_FAIL
 *	Type			: SOURCE NULL (pstPmtEsInfo)
 */
void svc_si_TcSubtitleSelectSourceNull1(Cutest_TestCase_t *pstTc)
{
	HERROR					 hErr;
	SvcSi_SelectedEs_t		 stSelectedSubtitle;
	const svcSi_PmtEsInfo_t	*pstPmtEsInfo;
	const SvcSi_Config_t	*pstConfig;

	// 1. Setup Fixture
	svc_si_UtSubtitleEsSetScenario (pstTc);		//	Set Up Test Stub
	pstPmtEsInfo	= NULL;
	pstConfig		= svc_si_UtSubtitleEsGetConfig (	"rus",	"dan",	"fra",	// szLang0[], szLang1[], szOsdLang[],
														"",						// szLangDefault[],
														eDxASPECTRATIO_16X9,	// eAspectRatio,
														FALSE,	FALSE,			// bDvbMenu, bHohMenu,
														"", 	FALSE); 		// szLangUser, bHohUser
	svc_si_UtInitSelectedEs (&stSelectedSubtitle);

	// 2. Exercise
	hErr = svc_si_UtSubtitleEsSelect (pstTc, FALSE, pstPmtEsInfo, pstConfig, &stSelectedSubtitle);

	// 3. Verify
	svc_si_UtSubtitleEsVeritySelectResult(pstTc, __FUNCTION__, HxNAMEOF(HERROR), hErr, ERR_FAIL);

	// 4. Teardown
	svc_si_UtFreeConfig ((SvcSi_Config_t *)pstConfig);
	svc_si_UtFreeEsInfo ((svcSi_PmtEsInfo_t *)pstPmtEsInfo);

	return;
}

/*	SUT 			: svc_si_UtSubtitleEsSelect
 *	Expected Result : ERR_FAIL
 *	Type			: SOURCE NULL (pstConfig)
 */
void svc_si_TcSubtitleSelectSourceNull2(Cutest_TestCase_t *pstTc)
{
	HERROR					 hErr;
	SvcSi_SelectedEs_t		 stSelectedSubtitle;
	const HxList_t			*pstList;
	const svcSi_PmtEsInfo_t	*pstPmtEsInfo;
	const SvcSi_Config_t	*pstConfig;

	// 1. Setup Fixture
	svc_si_UtSubtitleEsSetScenario (pstTc);		//	Set Up Test Stub
	pstList	= svc_si_UtSubtitleEsGetInfo (	4,		//	ItemNum
											1,  // [ Lang : "eng", MainType : "EBU", HOH : FALSE, AR : "n/a", HD : "n/a" ]
											14, // [ Lang : "nor", MainType : "EBU", HOH : FALSE, AR : "n/a", HD : "n/a" ]
											24, // [ Lang : "swe", MainType : "EBU", HOH : FALSE, AR : "n/a", HD : "n/a" ]
											25, // [ Lang : "swe", MainType : "DVB", HOH : FALSE, AR : "no", HD : "no" ]
											(HUINT32)-1);
	pstPmtEsInfo	= svc_si_UtSubtitleEsSetList (NULL, pstList);
	pstConfig		= NULL;
	svc_si_UtInitSelectedEs (&stSelectedSubtitle);

	// 2. Exercise
	hErr = svc_si_UtSubtitleEsSelect (pstTc, FALSE, pstPmtEsInfo, pstConfig, &stSelectedSubtitle);

	// 3. Verify
	svc_si_UtSubtitleEsVeritySelectResult(pstTc, __FUNCTION__, HxNAMEOF(HERROR), hErr, ERR_FAIL);

	// 4. Teardown
	svc_si_UtFreeConfig ((SvcSi_Config_t *)pstConfig);
	svc_si_UtFreeEsInfo ((svcSi_PmtEsInfo_t *)pstPmtEsInfo);

	return;
}

/*	SUT 			: svc_si_UtSubtitleEsSelect
 *	Expected Result : ERR_FAIL
 *	Type			: TARGET NULL (SelectedEs)
 */
void svc_si_TcSubtitleSelectTargetNull(Cutest_TestCase_t *pstTc)
{
	HERROR					 hErr;
	const HxList_t			*pstList;
	const svcSi_PmtEsInfo_t	*pstPmtEsInfo;
	const SvcSi_Config_t	*pstConfig;

	// 1. Setup Fixture
	svc_si_UtSubtitleEsSetScenario (pstTc);		//	Set Up Test Stub
	pstList	= svc_si_UtSubtitleEsGetInfo (	4,		//	ItemNum
											1,  // [ Lang : "eng", MainType : "EBU", HOH : FALSE, AR : "n/a", HD : "n/a" ]
											14, // [ Lang : "nor", MainType : "EBU", HOH : FALSE, AR : "n/a", HD : "n/a" ]
											24, // [ Lang : "swe", MainType : "EBU", HOH : FALSE, AR : "n/a", HD : "n/a" ]
											25, // [ Lang : "swe", MainType : "DVB", HOH : FALSE, AR : "no", HD : "no" ]
											(HUINT32)-1);
	pstPmtEsInfo	= svc_si_UtSubtitleEsSetList (NULL, pstList);
	pstConfig		= svc_si_UtSubtitleEsGetConfig (	"rus",	"dan",	"fra",	// szLang0[], szLang1[], szOsdLang[],
														"", 					// szLangDefault[],
														eDxASPECTRATIO_16X9,	// eAspectRatio,
														FALSE,	FALSE,			// bDvbMenu, bHohMenu,
														"", 	FALSE); 		// szLangUser, bHohUser

	// 2. Exercise
	hErr = svc_si_UtSubtitleEsSelect (pstTc, FALSE, pstPmtEsInfo, pstConfig, NULL);

	// 3. Verify
	svc_si_UtSubtitleEsVeritySelectResult(pstTc, __FUNCTION__, HxNAMEOF(HERROR), hErr, ERR_FAIL);

	// 4. Teardown
	svc_si_UtFreeConfig ((SvcSi_Config_t *)pstConfig);
	svc_si_UtFreeEsInfo ((svcSi_PmtEsInfo_t *)pstPmtEsInfo);

	return;
}

/*	SUT 			: svc_si_UtSubtitleEsSelect
 *	Expected Result : ERR_FAIL
 *	Type			: SOURCE INVALID (pstPmtEsInfo)
 *	Comments		: Number of Subtitle ES Item in pmtEsInfo is "0"
 */
void svc_si_TcSubtitleSelectSourceInvalid(Cutest_TestCase_t *pstTc)
{
	HERROR					 hErr;
	SvcSi_SelectedEs_t		 stSelectedSubtitle;
	const HxList_t			*pstList;
	const svcSi_PmtEsInfo_t	*pstPmtEsInfo;
	const SvcSi_Config_t	*pstConfig;

	// 1. Setup Fixture
	svc_si_UtSubtitleEsSetScenario (pstTc);			//	Set Up Test Stub
	pstList	= svc_si_UtSubtitleEsGetInfo (	0,		//	ItemNum
											(HUINT32)-1);
	pstPmtEsInfo	= svc_si_UtSubtitleEsSetList (NULL, pstList);
	pstConfig		= svc_si_UtSubtitleEsGetConfig (	"rus",	"dan",	"fra",	// szLang0[], szLang1[], szOsdLang[],
														"", 					// szLangDefault[],
														eDxASPECTRATIO_16X9,	// eAspectRatio,
														FALSE,	FALSE,			// bDvbMenu, bHohMenu,
														"", 	FALSE); 		// szLangUser, bHohUser
	svc_si_UtInitSelectedEs (&stSelectedSubtitle);

	// 2. Exercise
	hErr = svc_si_UtSubtitleEsSelect (pstTc, FALSE, pstPmtEsInfo, pstConfig, &stSelectedSubtitle);

	// 3. Verify
	svc_si_UtSubtitleEsVeritySelectResult(pstTc, __FUNCTION__, HxNAMEOF(HERROR), hErr, ERR_FAIL);

	// 4. Teardown
	svc_si_UtFreeConfig ((SvcSi_Config_t *)pstConfig);
	svc_si_UtFreeEsInfo ((svcSi_PmtEsInfo_t *)pstPmtEsInfo);

	return;
}

/*	SUT 			: svc_si_UtSubtitleEsSelect
 *	Expected Result : ERR_OK
 *	Type			: Check 1st Track Has top priority
 *	Comments		: No Language, Main Type, HOH, AR and HD Matched
 *					  In this case 1st Track Has Top Priority
 */
void svc_si_TcSubtitleSelectNoMatchFirstTrack(Cutest_TestCase_t *pstTc)
{
	HERROR					 hErr;
	SvcSi_SelectedEs_t		 stSelectedSubtitle;
	const HxList_t			*pstList;
	const svcSi_PmtEsInfo_t	*pstPmtEsInfo;
	const SvcSi_Config_t	*pstConfig;

	// 1. Setup Fixture
	svc_si_UtSubtitleEsSetScenario (pstTc);		//	Set Up Test Stub
	pstList	= svc_si_UtSubtitleEsGetInfo (	3,  // ItemNum
											29, // [ Lang : "fin", MainType : "EBU", HOH : FALSE, AR : "n/a", HD : "n/a" ]
											14, // [ Lang : "nor", MainType : "EBU", HOH : FALSE, AR : "n/a", HD : "n/a" ]
											24, // [ Lang : "swe", MainType : "EBU", HOH : FALSE, AR : "n/a", HD : "n/a" ]
//											25, // [ Lang : "swe", MainType : "DVB", HOH : FALSE, AR : "no", HD : "no" ]
											(HUINT32)-1);
	pstPmtEsInfo	= svc_si_UtSubtitleEsSetList (NULL, pstList);

	pstConfig		= svc_si_UtSubtitleEsGetConfig (	"rus",	"dan",	"fra",	// szLang0[], szLang1[], szOsdLang[],
														"", 					// szLangDefault[],
														eDxASPECTRATIO_16X9,	// eAspectRatio,
														FALSE, 	FALSE,			// bDvbMenu, bHohMenu,
														"",		FALSE);			// szLangUser, bHohUser
	svc_si_UtInitSelectedEs (&stSelectedSubtitle);

	// 2. Exercise
	hErr = svc_si_UtSubtitleEsSelect (pstTc, TRUE, pstPmtEsInfo, pstConfig, &stSelectedSubtitle);

	// 3. Verify
	svc_si_UtSubtitleEsVeritySelectOutput(pstTc, hErr, &stSelectedSubtitle, 0);

	// 4. Teardown
	svc_si_UtFreeConfig ((SvcSi_Config_t *)pstConfig);
	svc_si_UtFreeEsInfo ((svcSi_PmtEsInfo_t *)pstPmtEsInfo);

	return;
}

#define ___ES_SUBTITLE_TEST_CASE_LANGUAGE___

/*	SUT 			: svc_si_UtSubtitleEsSelect
 *	Expected Result : ERR_OK
 *	Type			: Lang Subtitle0
 *	Comments		: Find Lang Subtitle0 Matched Track
 */
void svc_si_TcSubtitleSelectLang0(Cutest_TestCase_t *pstTc)
{
	HERROR					 hErr;
	SvcSi_SelectedEs_t		 stSelectedSubtitle;
	const HxList_t			*pstList;
	const svcSi_PmtEsInfo_t	*pstPmtEsInfo;
	const SvcSi_Config_t	*pstConfig;

	// 1. Setup Fixture
	svc_si_UtSubtitleEsSetScenario (pstTc);		//	Set Up Test Stub
	pstList	= svc_si_UtSubtitleEsGetInfo (	3,		//	ItemNum
											29, // [ Lang : "fin", MainType : "EBU", HOH : FALSE, AR : "n/a", HD : "n/a" ]
											14, // [ Lang : "nor", MainType : "EBU", HOH : FALSE, AR : "n/a", HD : "n/a" ]
											24, // [ Lang : "swe", MainType : "EBU", HOH : FALSE, AR : "n/a", HD : "n/a" ]
											(HUINT32)-1);
	pstPmtEsInfo	= svc_si_UtSubtitleEsSetList (NULL, pstList);
	pstConfig		= svc_si_UtSubtitleEsGetConfig (	"swe",	"dan",	"fra",	// szLang0[], szLang1[], szOsdLang[],
														"", 					// szLangDefault[],
														eDxASPECTRATIO_16X9,	// eAspectRatio,
														FALSE,	FALSE,			// bDvbMenu, bHohMenu,
														"", 	FALSE); 		// szLangUser, bHohUser
	svc_si_UtInitSelectedEs (&stSelectedSubtitle);

	// 2. Exercise
	hErr = svc_si_UtSubtitleEsSelect (pstTc, TRUE, pstPmtEsInfo, pstConfig, &stSelectedSubtitle);

	// 3. Verify
	svc_si_UtSubtitleEsVeritySelectOutput(pstTc, hErr, &stSelectedSubtitle, 2);

	// 4. Teardown
	svc_si_UtFreeConfig ((SvcSi_Config_t *)pstConfig);
	svc_si_UtFreeEsInfo ((svcSi_PmtEsInfo_t *)pstPmtEsInfo);

	return;
}

/*	SUT 			: svc_si_UtSubtitleEsSelect
 *	Expected Result : ERR_OK
 *	Type			: Lang Audio1
 *	Comments		: Find Lang Audio1 Matched Track
 */
void svc_si_TcSubtitleSelectLang1(Cutest_TestCase_t *pstTc)
{
	HERROR					 hErr;
	SvcSi_SelectedEs_t		 stSelectedSubtitle;
	const HxList_t			*pstList;
	const svcSi_PmtEsInfo_t	*pstPmtEsInfo;
	const SvcSi_Config_t	*pstConfig;

	// 1. Setup Fixture
	svc_si_UtSubtitleEsSetScenario (pstTc);		//	Set Up Test Stub
	pstList	= svc_si_UtSubtitleEsGetInfo (	4,		//	ItemNum
											29, // [ Lang : "fin", MainType : "EBU", HOH : FALSE, AR : "n/a", HD : "n/a" ]
											14, // [ Lang : "nor", MainType : "EBU", HOH : FALSE, AR : "n/a", HD : "n/a" ]
											19, // [ Lang : "dan", MainType : "EBU", HOH : FALSE, AR : "n/a", HD : "n/a" ]
											24, // [ Lang : "swe", MainType : "EBU", HOH : FALSE, AR : "n/a", HD : "n/a" ]
											(HUINT32)-1);
	pstPmtEsInfo	= svc_si_UtSubtitleEsSetList (NULL, pstList);
	pstConfig		= svc_si_UtSubtitleEsGetConfig (	"rus",	"dan",	"fra",	// szLang0[], szLang1[], szOsdLang[],
														"", 					// szLangDefault[],
														eDxASPECTRATIO_16X9,	// eAspectRatio,
														FALSE,	FALSE,			// bDvbMenu, bHohMenu,
														"", 	FALSE); 		// szLangUser, bHohUser
	svc_si_UtInitSelectedEs (&stSelectedSubtitle);

	// 2. Exercise
	hErr = svc_si_UtSubtitleEsSelect (pstTc, TRUE, pstPmtEsInfo, pstConfig, &stSelectedSubtitle);

	// 3. Verify
	svc_si_UtSubtitleEsVeritySelectOutput(pstTc, hErr, &stSelectedSubtitle, 2);

	// 4. Teardown
	svc_si_UtFreeConfig ((SvcSi_Config_t *)pstConfig);
	svc_si_UtFreeEsInfo ((svcSi_PmtEsInfo_t *)pstPmtEsInfo);

	return;
}

/*	SUT 			: svc_si_UtSubtitleEsSelect
 *	Expected Result : ERR_OK
 *	Type			: Lang OSD
 *	Comments		: Find Lang Menu Matched Track
 */
void svc_si_TcSubtitleSelectLangMenu(Cutest_TestCase_t *pstTc)
{
	HERROR					 hErr;
	SvcSi_SelectedEs_t		 stSelectedSubtitle;
	const HxList_t			*pstList;
	const svcSi_PmtEsInfo_t	*pstPmtEsInfo;
	const SvcSi_Config_t	*pstConfig;

	// 1. Setup Fixture
	svc_si_UtSubtitleEsSetScenario (pstTc);		//	Set Up Test Stub
	pstList	= svc_si_UtSubtitleEsGetInfo (	4,		//	ItemNum
											29, // [ Lang : "fin", MainType : "EBU", HOH : FALSE, AR : "n/a", HD : "n/a" ]
											14, // [ Lang : "nor", MainType : "EBU", HOH : FALSE, AR : "n/a", HD : "n/a" ]
											19, // [ Lang : "dan", MainType : "EBU", HOH : FALSE, AR : "n/a", HD : "n/a" ]
											24, // [ Lang : "swe", MainType : "EBU", HOH : FALSE, AR : "n/a", HD : "n/a" ]
											(HUINT32)-1);
	pstPmtEsInfo	= svc_si_UtSubtitleEsSetList (NULL, pstList);
	pstConfig		= svc_si_UtSubtitleEsGetConfig (	"rus",	"ger",	"swe",	// szLang0[], szLang1[], szOsdLang[],
														"", 					// szLangDefault[],
														eDxASPECTRATIO_16X9,	// eAspectRatio,
														FALSE,	FALSE,			// bDvbMenu, bHohMenu,
														"", 	FALSE); 		// szLangUser, bHohUser
	svc_si_UtInitSelectedEs (&stSelectedSubtitle);

	// 2. Exercise
	hErr = svc_si_UtSubtitleEsSelect (pstTc, TRUE, pstPmtEsInfo, pstConfig, &stSelectedSubtitle);

	// 3. Verify
	svc_si_UtSubtitleEsVeritySelectOutput(pstTc, hErr, &stSelectedSubtitle, 3);

	// 4. Teardown
	svc_si_UtFreeConfig ((SvcSi_Config_t *)pstConfig);
	svc_si_UtFreeEsInfo ((svcSi_PmtEsInfo_t *)pstPmtEsInfo);

	return;
}

/*	SUT 			: svc_si_UtSubtitleEsSelect
 *	Expected Result : ERR_OK
 *	Type			: Lang User
 *	Comments		: Find Lang User Matched Track
 */
void svc_si_TcSubtitleSelectLangUser(Cutest_TestCase_t *pstTc)
{
	HERROR					 hErr;
	SvcSi_SelectedEs_t 		 stSelectedSubtitle;
	const HxList_t			*pstList;
	const svcSi_PmtEsInfo_t	*pstPmtEsInfo;
	const SvcSi_Config_t	*pstConfig;

	// 1. Setup Fixture
	svc_si_UtSubtitleEsSetScenario (pstTc);		//	Set Up Test Stub
	pstList	= svc_si_UtSubtitleEsGetInfo (	4,	//	ItemNum
											29, // [ Lang : "fin", MainType : "EBU", HOH : FALSE, AR : "n/a", HD : "n/a" ]
											14, // [ Lang : "nor", MainType : "EBU", HOH : FALSE, AR : "n/a", HD : "n/a" ]
											19, // [ Lang : "dan", MainType : "EBU", HOH : FALSE, AR : "n/a", HD : "n/a" ]
											24, // [ Lang : "swe", MainType : "EBU", HOH : FALSE, AR : "n/a", HD : "n/a" ]
											(HUINT32)-1);
	pstPmtEsInfo	= svc_si_UtSubtitleEsSetList (NULL, pstList);
	pstConfig		= svc_si_UtSubtitleEsGetConfig (	"rus",	"ger",	"fra",	// szLang0[], szLang1[], szOsdLang[],
														"", 					// szLangDefault[],
														eDxASPECTRATIO_16X9,	// eAspectRatio,
														FALSE,	FALSE,			// bDvbMenu, bHohMenu,
														"swe", 	FALSE); 		// szLangUser, bHohUser
	svc_si_UtInitSelectedEs (&stSelectedSubtitle);

	// 2. Exercise
	hErr = svc_si_UtSubtitleEsSelect (pstTc, TRUE, pstPmtEsInfo, pstConfig, &stSelectedSubtitle);

	// 3. Verify
	svc_si_UtSubtitleEsVeritySelectOutput(pstTc, hErr, &stSelectedSubtitle, 3);

	// 4. Teardown
	svc_si_UtFreeConfig ((SvcSi_Config_t *)pstConfig);
	svc_si_UtFreeEsInfo ((svcSi_PmtEsInfo_t *)pstPmtEsInfo);

	return;
}

/*	SUT 			: svc_si_UtSubtitleEsSelect
 *	Expected Result : ERR_OK
 *	Type			: Lang Undefined
 *	Comments		: Find Lang Undefined Matched Track
 */
void svc_si_TcSubtitleSelectLangUndefined(Cutest_TestCase_t *pstTc)
{
	HERROR					 hErr;
	SvcSi_SelectedEs_t 		 stSelectedSubtitle;
	const HxList_t			*pstList;
	const svcSi_PmtEsInfo_t	*pstPmtEsInfo;
	const SvcSi_Config_t	*pstConfig;

	// 1. Setup Fixture
	svc_si_UtSubtitleEsSetScenario (pstTc);		//	Set Up Test Stub
	pstList	= svc_si_UtSubtitleEsGetInfo (	4,		//	ItemNum
											29, // [ Lang : "fin", MainType : "EBU", HOH : FALSE, AR : "n/a", HD : "n/a" ]
											14, // [ Lang : "nor", MainType : "EBU", HOH : FALSE, AR : "n/a", HD : "n/a" ]
											44, // [ Lang : "und", MainType : "DVB", HOH : FALSE, AR : "no", HD : "no" ]
											24, // [ Lang : "swe", MainType : "EBU", HOH : FALSE, AR : "n/a", HD : "n/a" ]
											(HUINT32)-1);
	pstPmtEsInfo	= svc_si_UtSubtitleEsSetList (NULL, pstList);
	pstConfig		= svc_si_UtSubtitleEsGetConfig (	"rus",	"dan",	"fra",	// szLang0[], szLang1[], szOsdLang[],
														"", 					// szLangDefault[],
														eDxASPECTRATIO_16X9,	// eAspectRatio,
														FALSE,	FALSE,			// bDvbMenu, bHohMenu,
														"", 	FALSE); 		// szLangUser, bHohUser
	svc_si_UtInitSelectedEs (&stSelectedSubtitle);

	// 2. Exercise
	hErr = svc_si_UtSubtitleEsSelect (pstTc, TRUE, pstPmtEsInfo, pstConfig, &stSelectedSubtitle);

	// 3. Verify
	svc_si_UtSubtitleEsVeritySelectOutput(pstTc, hErr, &stSelectedSubtitle, 2);

	// 4. Teardown
	svc_si_UtFreeConfig ((SvcSi_Config_t *)pstConfig);
	svc_si_UtFreeEsInfo ((svcSi_PmtEsInfo_t *)pstPmtEsInfo);

	return;
}

/*	SUT 			: svc_si_UtSubtitleEsSelect
 *	Expected Result : ERR_OK
 *	Type			: Lang Original
 *	Comments		: Find Lang Original Matched Track
 */
void svc_si_TcSubtitleSelectLangOriginal(Cutest_TestCase_t *pstTc)
{
	HERROR					 hErr;
	SvcSi_SelectedEs_t 		 stSelectedSubtitle;
	const HxList_t			*pstList;
	const svcSi_PmtEsInfo_t	*pstPmtEsInfo;
	const SvcSi_Config_t	*pstConfig;

	// 1. Setup Fixture
	svc_si_UtSubtitleEsSetScenario (pstTc);		//	Set Up Test Stub
	pstList	= svc_si_UtSubtitleEsGetInfo (	4,		//	ItemNum
											29, // [ Lang : "fin", MainType : "EBU", HOH : FALSE, AR : "n/a", HD : "n/a" ]
											14, // [ Lang : "nor", MainType : "EBU", HOH : FALSE, AR : "n/a", HD : "n/a" ]
											39, // [ Lang : "qaa", MainType : "EBU", HOH : FALSE, AR : "n/a", HD : "n/a" ]
											24, // [ Lang : "swe", MainType : "EBU", HOH : FALSE, AR : "n/a", HD : "n/a" ]
											(HUINT32)-1);
	pstPmtEsInfo	= svc_si_UtSubtitleEsSetList (NULL, pstList);
	pstConfig		= svc_si_UtSubtitleEsGetConfig (	"rus",	"dan",	"fra",	// szLang0[], szLang1[], szOsdLang[],
														"", 					// szLangDefault[],
														eDxASPECTRATIO_16X9,	// eAspectRatio,
														FALSE,	FALSE,			// bDvbMenu, bHohMenu,
														"", 	FALSE); 		// szLangUser, bHohUser
	svc_si_UtInitSelectedEs (&stSelectedSubtitle);

	// 2. Exercise
	hErr = svc_si_UtSubtitleEsSelect (pstTc, TRUE, pstPmtEsInfo, pstConfig, &stSelectedSubtitle);

	// 3. Verify
	svc_si_UtSubtitleEsVeritySelectOutput(pstTc, hErr, &stSelectedSubtitle, 2);

	// 4. Teardown
	svc_si_UtFreeConfig ((SvcSi_Config_t *)pstConfig);
	svc_si_UtFreeEsInfo ((svcSi_PmtEsInfo_t *)pstPmtEsInfo);

	return;
}

/*	SUT 			: svc_si_UtSubtitleEsSelect
 *	Expected Result : ERR_OK
 *	Type			: Lang Default
 *	Comments		: Find Lang Default Matched Track
 */
void svc_si_TcSubtitleSelectLangDefault(Cutest_TestCase_t *pstTc)
{
	HERROR					 hErr;
	SvcSi_SelectedEs_t 		 stSelectedSubtitle;
	const HxList_t			*pstList;
	const svcSi_PmtEsInfo_t	*pstPmtEsInfo;
	const SvcSi_Config_t	*pstConfig;

	// 1. Setup Fixture
	svc_si_UtSubtitleEsSetScenario (pstTc);		//	Set Up Test Stub
	pstList	= svc_si_UtSubtitleEsGetInfo (	4,		//	ItemNum
											29, // [ Lang : "fin", MainType : "EBU", HOH : FALSE, AR : "n/a", HD : "n/a" ]
											14, // [ Lang : "nor", MainType : "EBU", HOH : FALSE, AR : "n/a", HD : "n/a" ]
											01, // [ Lang : "eng", MainType : "EBU", HOH : FALSE, AR : "n/a", HD : "n/a" ]
											24, // [ Lang : "swe", MainType : "EBU", HOH : FALSE, AR : "n/a", HD : "n/a" ]
											(HUINT32)-1);
	pstPmtEsInfo	= svc_si_UtSubtitleEsSetList (NULL, pstList);
	pstConfig		= svc_si_UtSubtitleEsGetConfig (	"rus",	"dan",	"fra",	// szLang0[], szLang1[], szOsdLang[],
														"eng", 					// szLangDefault[],
														eDxASPECTRATIO_16X9,	// eAspectRatio,
														FALSE,	FALSE,			// bDvbMenu, bHohMenu,
														"", 	FALSE); 		// szLangUser, bHohUser
	svc_si_UtInitSelectedEs (&stSelectedSubtitle);

	// 2. Exercise
	hErr = svc_si_UtSubtitleEsSelect (pstTc, TRUE, pstPmtEsInfo, pstConfig, &stSelectedSubtitle);

	// 3. Verify
	svc_si_UtSubtitleEsVeritySelectOutput(pstTc, hErr, &stSelectedSubtitle, 2);

	// 4. Teardown
	svc_si_UtFreeConfig ((SvcSi_Config_t *)pstConfig);
	svc_si_UtFreeEsInfo ((svcSi_PmtEsInfo_t *)pstPmtEsInfo);

	return;
}

/*	SUT 			: svc_si_UtSubtitleEsSelect
 *	Expected Result : ERR_OK
 *	Type			: Lang Default, Original
 *	Comments		: Lang Undefined > Lang Original
 */
void svc_si_TcSubtitleSelectLangUndefinedAndOriginal(Cutest_TestCase_t *pstTc)
{
	HERROR					 hErr;
	SvcSi_SelectedEs_t 		 stSelectedSubtitle;
	const HxList_t			*pstList;
	const svcSi_PmtEsInfo_t	*pstPmtEsInfo;
	const SvcSi_Config_t	*pstConfig;

	// 1. Setup Fixture
	svc_si_UtSubtitleEsSetScenario (pstTc);		//	Set Up Test Stub
	pstList	= svc_si_UtSubtitleEsGetInfo (	4,		//	ItemNum
											29, // [ Lang : "fin", MainType : "EBU", HOH : FALSE, AR : "n/a", HD : "n/a" ]
											40, // [ Lang : "qaa", MainType : "DVB", HOH : FALSE, AR : "no", HD : "no" ]
											44, // [ Lang : "und", MainType : "DVB", HOH : FALSE, AR : "no", HD : "no" ]
											24, // [ Lang : "swe", MainType : "EBU", HOH : FALSE, AR : "n/a", HD : "n/a" ]
											(HUINT32)-1);
	pstPmtEsInfo	= svc_si_UtSubtitleEsSetList (NULL, pstList);
	pstConfig		= svc_si_UtSubtitleEsGetConfig (	"rus",	"dan",	"fra",	// szLang0[], szLang1[], szOsdLang[],
														"", 					// szLangDefault[],
														eDxASPECTRATIO_16X9,	// eAspectRatio,
														FALSE,	FALSE,			// bDvbMenu, bHohMenu,
														"", 	FALSE); 		// szLangUser, bHohUser
	svc_si_UtInitSelectedEs (&stSelectedSubtitle);

	// 2. Exercise
	hErr = svc_si_UtSubtitleEsSelect (pstTc, TRUE, pstPmtEsInfo, pstConfig, &stSelectedSubtitle);

	// 3. Verify
	svc_si_UtSubtitleEsVeritySelectOutput(pstTc, hErr, &stSelectedSubtitle, 2);

	// 4. Teardown
	svc_si_UtFreeConfig ((SvcSi_Config_t *)pstConfig);
	svc_si_UtFreeEsInfo ((svcSi_PmtEsInfo_t *)pstPmtEsInfo);

	return;
}

/*	SUT 			: svc_si_UtSubtitleEsSelect
 *	Expected Result : ERR_OK
 *	Type			: Lang Original and Default
 *	Comments		: Lang Original > Lang Default
 */
void svc_si_TcSubtitleSelectLangOriginalAndDefault(Cutest_TestCase_t *pstTc)
{
	HERROR					 hErr;
	SvcSi_SelectedEs_t 		 stSelectedSubtitle;
	const HxList_t			*pstList;
	const svcSi_PmtEsInfo_t	*pstPmtEsInfo;
	const SvcSi_Config_t	*pstConfig;

	// 1. Setup Fixture
	svc_si_UtSubtitleEsSetScenario (pstTc);		//	Set Up Test Stub
	pstList	= svc_si_UtSubtitleEsGetInfo (	4,		//	ItemNum
											29, // [ Lang : "fin", MainType : "EBU", HOH : FALSE, AR : "n/a", HD : "n/a" ]
											40, // [ Lang : "qaa", MainType : "DVB", HOH : FALSE, AR : "no", HD : "no" ]
											00, // [ Lang : "eng", MainType : "EBU", HOH : FALSE, AR : "n/a", HD : "n/a" ]
											24, // [ Lang : "swe", MainType : "EBU", HOH : FALSE, AR : "n/a", HD : "n/a" ]
											(HUINT32)-1);
	pstPmtEsInfo	= svc_si_UtSubtitleEsSetList (NULL, pstList);
	pstConfig		= svc_si_UtSubtitleEsGetConfig (	"rus",	"dan",	"fra",	// szLang0[], szLang1[], szOsdLang[],
														"eng", 					// szLangDefault[],
														eDxASPECTRATIO_16X9,	// eAspectRatio,
														FALSE,	FALSE,			// bDvbMenu, bHohMenu,
														"", 	FALSE); 		// szLangUser, bHohUser
	svc_si_UtInitSelectedEs (&stSelectedSubtitle);

	// 2. Exercise
	hErr = svc_si_UtSubtitleEsSelect (pstTc, TRUE, pstPmtEsInfo, pstConfig, &stSelectedSubtitle);

	// 3. Verify
	svc_si_UtSubtitleEsVeritySelectOutput(pstTc, hErr, &stSelectedSubtitle, 1);

	// 4. Teardown
	svc_si_UtFreeConfig ((SvcSi_Config_t *)pstConfig);
	svc_si_UtFreeEsInfo ((svcSi_PmtEsInfo_t *)pstPmtEsInfo);

	return;
}

/*	SUT 			: svc_si_UtSubtitleEsSelect
 *	Expected Result : ERR_OK
 *	Type			: Lang Priority
 *	Comments		: Lang0 > Lang1
 */
void svc_si_TcSubtitleSelectLangPriority1(Cutest_TestCase_t *pstTc)
{
	HERROR					 hErr;
	SvcSi_SelectedEs_t		 stSelectedSubtitle;
	const HxList_t			*pstList;
	const svcSi_PmtEsInfo_t	*pstPmtEsInfo;
	const SvcSi_Config_t	*pstConfig;

	// 1. Setup Fixture
	svc_si_UtSubtitleEsSetScenario (pstTc);		//	Set Up Test Stub
	pstList	= svc_si_UtSubtitleEsGetInfo (	4,		//	ItemNum
											29, // [ Lang : "fin", MainType : "EBU", HOH : FALSE, AR : "n/a", HD : "n/a" ]
											14, // [ Lang : "nor", MainType : "EBU", HOH : FALSE, AR : "n/a", HD : "n/a" ]
											19, // [ Lang : "dan", MainType : "EBU", HOH : FALSE, AR : "n/a", HD : "n/a" ]
											24, // [ Lang : "swe", MainType : "EBU", HOH : FALSE, AR : "n/a", HD : "n/a" ]
											(HUINT32)-1);
	pstPmtEsInfo	= svc_si_UtSubtitleEsSetList (NULL, pstList);
	pstConfig		= svc_si_UtSubtitleEsGetConfig (	"swe",	"dan",	"fra",	// szLang0[], szLang1[], szOsdLang[],
														"", 					// szLangDefault[],
														eDxASPECTRATIO_16X9,	// eAspectRatio,
														FALSE,	FALSE,			// bDvbMenu, bHohMenu,
														"", 	FALSE); 		// szLangUser, bHohUser
	svc_si_UtInitSelectedEs (&stSelectedSubtitle);

	// 2. Exercise
	hErr = svc_si_UtSubtitleEsSelect (pstTc, TRUE, pstPmtEsInfo, pstConfig, &stSelectedSubtitle);

	// 3. Verify
	svc_si_UtSubtitleEsVeritySelectOutput(pstTc, hErr, &stSelectedSubtitle, 3);

	// 4. Teardown
	svc_si_UtFreeConfig ((SvcSi_Config_t *)pstConfig);
	svc_si_UtFreeEsInfo ((svcSi_PmtEsInfo_t *)pstPmtEsInfo);

	return;
}

/*	SUT 			: svc_si_UtSubtitleEsSelect
 *	Expected Result : ERR_OK
 *	Type			: Lang Priority
 *	Comments		: Lang1 > LangOSD
 */
void svc_si_TcSubtitleSelectLangPriority2(Cutest_TestCase_t *pstTc)
{
	HERROR					 hErr;
	SvcSi_SelectedEs_t		 stSelectedSubtitle;
	const HxList_t			*pstList;
	const svcSi_PmtEsInfo_t	*pstPmtEsInfo;
	const SvcSi_Config_t	*pstConfig;

	// 1. Setup Fixture
	svc_si_UtSubtitleEsSetScenario (pstTc);		//	Set Up Test Stub
	pstList	= svc_si_UtSubtitleEsGetInfo (	4,		//	ItemNum
											29, // [ Lang : "fin", MainType : "EBU", HOH : FALSE, AR : "n/a", HD : "n/a" ]
											14, // [ Lang : "nor", MainType : "EBU", HOH : FALSE, AR : "n/a", HD : "n/a" ]
											19, // [ Lang : "dan", MainType : "EBU", HOH : FALSE, AR : "n/a", HD : "n/a" ]
											24, // [ Lang : "swe", MainType : "EBU", HOH : FALSE, AR : "n/a", HD : "n/a" ]
											(HUINT32)-1);
	pstPmtEsInfo	= svc_si_UtSubtitleEsSetList (NULL, pstList);
	pstConfig		= svc_si_UtSubtitleEsGetConfig (	"ger",	"dan",	"fin",	// szLang0[], szLang1[], szOsdLang[],
														"", 					// szLangDefault[],
														eDxASPECTRATIO_16X9,	// eAspectRatio,
														FALSE,	FALSE,			// bDvbMenu, bHohMenu,
														"", 	FALSE); 		// szLangUser, bHohUser
	svc_si_UtInitSelectedEs (&stSelectedSubtitle);

	// 2. Exercise
	hErr = svc_si_UtSubtitleEsSelect (pstTc, TRUE, pstPmtEsInfo, pstConfig, &stSelectedSubtitle);

	// 3. Verify
	svc_si_UtSubtitleEsVeritySelectOutput(pstTc, hErr, &stSelectedSubtitle, 2);

	// 4. Teardown
	svc_si_UtFreeConfig ((SvcSi_Config_t *)pstConfig);
	svc_si_UtFreeEsInfo ((svcSi_PmtEsInfo_t *)pstPmtEsInfo);

	return;
}

/*	SUT 			: svc_si_UtSubtitleEsSelect
 *	Expected Result : ERR_OK
 *	Type			: Lang Priority
 *	Comments		: LangUser > Lang0
 */
void svc_si_TcSubtitleSelectLangPriority3(Cutest_TestCase_t *pstTc)
{
	HERROR					 hErr;
	SvcSi_SelectedEs_t		 stSelectedSubtitle;
	const HxList_t			*pstList;
	const svcSi_PmtEsInfo_t	*pstPmtEsInfo;
	const SvcSi_Config_t	*pstConfig;

	// 1. Setup Fixture
	svc_si_UtSubtitleEsSetScenario (pstTc);		//	Set Up Test Stub
	pstList	= svc_si_UtSubtitleEsGetInfo (	4,		//	ItemNum
											29, // [ Lang : "fin", MainType : "EBU", HOH : FALSE, AR : "n/a", HD : "n/a" ]
											14, // [ Lang : "nor", MainType : "EBU", HOH : FALSE, AR : "n/a", HD : "n/a" ]
											19, // [ Lang : "dan", MainType : "EBU", HOH : FALSE, AR : "n/a", HD : "n/a" ]
											24, // [ Lang : "swe", MainType : "EBU", HOH : FALSE, AR : "n/a", HD : "n/a" ]
											(HUINT32)-1);
	pstPmtEsInfo	= svc_si_UtSubtitleEsSetList (NULL, pstList);
	pstConfig		= svc_si_UtSubtitleEsGetConfig (	"fin",	"dan",	"ger",	// szLang0[], szLang1[], szOsdLang[],
														"", 					// szLangDefault[],
														eDxASPECTRATIO_16X9,	// eAspectRatio,
														FALSE,	FALSE,			// bDvbMenu, bHohMenu,
														"swe", 	FALSE); 		// szLangUser, bHohUser
	svc_si_UtInitSelectedEs (&stSelectedSubtitle);

	// 2. Exercise
	hErr = svc_si_UtSubtitleEsSelect (pstTc, TRUE, pstPmtEsInfo, pstConfig, &stSelectedSubtitle);

	// 3. Verify
	svc_si_UtSubtitleEsVeritySelectOutput(pstTc, hErr, &stSelectedSubtitle, 3);

	// 4. Teardown
	svc_si_UtFreeConfig ((SvcSi_Config_t *)pstConfig);
	svc_si_UtFreeEsInfo ((svcSi_PmtEsInfo_t *)pstPmtEsInfo);

	return;
}

#define ___ES_SUBTITLE_TEST_CASE_MAIN_TYPE___

/*	SUT 			: svc_si_UtSubtitleEsSelect
 *	Expected Result : ERR_OK
 *	Type			: Main Type (DVB)
 *	Comments		: DVB > EBU
 */
void svc_si_TcSubtitleSelectTypeDvb(Cutest_TestCase_t *pstTc)
{
	HERROR					 hErr;
	SvcSi_SelectedEs_t		 stSelectedSubtitle;
	const HxList_t			*pstList;
	const svcSi_PmtEsInfo_t	*pstPmtEsInfo;
	const SvcSi_Config_t	*pstConfig;

	// 1. Setup Fixture
	svc_si_UtSubtitleEsSetScenario (pstTc);		//	Set Up Test Stub
	pstList	= svc_si_UtSubtitleEsGetInfo (	4,		//	ItemNum
											29, // [ Lang : "fin", MainType : "EBU", HOH : FALSE, AR : "n/a", HD : "n/a" ]
											0,  // [ Lang : "eng", MainType : "EBU", HOH : FALSE, AR : "n/a", HD : "n/a" ]
											4,  // [ Lang : "eng", MainType : "DVB", HOH : FALSE, AR : "no", HD : "no" ]
											24, // [ Lang : "swe", MainType : "EBU", HOH : FALSE, AR : "n/a", HD : "n/a" ]
											(HUINT32)-1);
	pstPmtEsInfo	= svc_si_UtSubtitleEsSetList (NULL, pstList);
	pstConfig		= svc_si_UtSubtitleEsGetConfig (	"eng",	"dan",	"ger",	// szLang0[], szLang1[], szOsdLang[],
														"", 					// szLangDefault[],
														eDxASPECTRATIO_16X9,	// eAspectRatio,
														TRUE,	FALSE,			// bDvbMenu, bHohMenu,
														"", 	FALSE); 		// szLangUser, bHohUser
	svc_si_UtInitSelectedEs (&stSelectedSubtitle);

	// 2. Exercise
	hErr = svc_si_UtSubtitleEsSelect (pstTc, TRUE, pstPmtEsInfo, pstConfig, &stSelectedSubtitle);

	// 3. Verify
	svc_si_UtSubtitleEsVeritySelectOutput(pstTc, hErr, &stSelectedSubtitle, 2);

	// 4. Teardown
	svc_si_UtFreeConfig ((SvcSi_Config_t *)pstConfig);
	svc_si_UtFreeEsInfo ((svcSi_PmtEsInfo_t *)pstPmtEsInfo);

	return;
}

/*	SUT 			: svc_si_UtSubtitleEsSelect
 *	Expected Result : ERR_OK
 *	Type			: Main Type (DVB)
 *	Comments		: Lang has higher priority than main type
 */
void svc_si_TcSubtitleSelectTypeDvbAndLang1(Cutest_TestCase_t *pstTc)
{
	HERROR					 hErr;
	SvcSi_SelectedEs_t		 stSelectedSubtitle;
	const HxList_t			*pstList;
	const svcSi_PmtEsInfo_t	*pstPmtEsInfo;
	const SvcSi_Config_t	*pstConfig;

	// 1. Setup Fixture
	svc_si_UtSubtitleEsSetScenario (pstTc);		//	Set Up Test Stub
	pstList	= svc_si_UtSubtitleEsGetInfo (	4,		//	ItemNum
											29, // [ Lang : "fin", MainType : "EBU", HOH : FALSE, AR : "n/a", HD : "n/a" ]
											16, // [ Lang : "nor", MainType : "DVB", HOH : FALSE, AR : "4x3", HD : "no" ]
											0,  // [ Lang : "eng", MainType : "EBU", HOH : FALSE, AR : "n/a", HD : "n/a" ]
											24, // [ Lang : "swe", MainType : "EBU", HOH : FALSE, AR : "n/a", HD : "n/a" ]
											(HUINT32)-1);
	pstPmtEsInfo	= svc_si_UtSubtitleEsSetList (NULL, pstList);
	pstConfig		= svc_si_UtSubtitleEsGetConfig (	"eng",	"dan",	"ger",	// szLang0[], szLang1[], szOsdLang[],
														"", 					// szLangDefault[],
														eDxASPECTRATIO_16X9,	// eAspectRatio,
														TRUE,	FALSE,			// bDvbMenu, bHohMenu,
														"", 	FALSE); 		// szLangUser, bHohUser
	svc_si_UtInitSelectedEs (&stSelectedSubtitle);

	// 2. Exercise
	hErr = svc_si_UtSubtitleEsSelect (pstTc, TRUE, pstPmtEsInfo, pstConfig, &stSelectedSubtitle);

	// 3. Verify
	svc_si_UtSubtitleEsVeritySelectOutput(pstTc, hErr, &stSelectedSubtitle, 2);

	// 4. Teardown
	svc_si_UtFreeConfig ((SvcSi_Config_t *)pstConfig);
	svc_si_UtFreeEsInfo ((svcSi_PmtEsInfo_t *)pstPmtEsInfo);

	return;
}

/*	SUT 			: svc_si_UtSubtitleEsSelect
 *	Expected Result : ERR_OK
 *	Type			: Main Type (DVB)
 *	Comments		: Lang has higher priority than main type
 */
void svc_si_TcSubtitleSelectTypeDvbAndLang2(Cutest_TestCase_t *pstTc)
{
	HERROR					 hErr;
	SvcSi_SelectedEs_t		 stSelectedSubtitle;
	const HxList_t			*pstList;
	const svcSi_PmtEsInfo_t	*pstPmtEsInfo;
	const SvcSi_Config_t	*pstConfig;

	// 1. Setup Fixture
	svc_si_UtSubtitleEsSetScenario (pstTc);		//	Set Up Test Stub
	pstList	= svc_si_UtSubtitleEsGetInfo (	4,		//	ItemNum
											29, // [ Lang : "fin", MainType : "EBU", HOH : FALSE, AR : "n/a", HD : "n/a" ]
											16, // [ Lang : "nor", MainType : "DVB", HOH : FALSE, AR : "4x3", HD : "no" ]
											4,  // [ Lang : "eng", MainType : "DVB", HOH : FALSE, AR : "no", HD : "no" ]
											24, // [ Lang : "swe", MainType : "EBU", HOH : FALSE, AR : "n/a", HD : "n/a" ]
											(HUINT32)-1);
	pstPmtEsInfo	= svc_si_UtSubtitleEsSetList (NULL, pstList);
	pstConfig		= svc_si_UtSubtitleEsGetConfig (	"eng",	"dan",	"ger",	// szLang0[], szLang1[], szOsdLang[],
														"", 					// szLangDefault[],
														eDxASPECTRATIO_16X9,	// eAspectRatio,
														TRUE,	FALSE,			// bDvbMenu, bHohMenu,
														"nor", 	FALSE); 		// szLangUser, bHohUser
	svc_si_UtInitSelectedEs (&stSelectedSubtitle);

	// 2. Exercise
	hErr = svc_si_UtSubtitleEsSelect (pstTc, TRUE, pstPmtEsInfo, pstConfig, &stSelectedSubtitle);

	// 3. Verify
	svc_si_UtSubtitleEsVeritySelectOutput(pstTc, hErr, &stSelectedSubtitle, 1);

	// 4. Teardown
	svc_si_UtFreeConfig ((SvcSi_Config_t *)pstConfig);
	svc_si_UtFreeEsInfo ((svcSi_PmtEsInfo_t *)pstPmtEsInfo);

	return;
}

/*	SUT 			: svc_si_UtSubtitleEsSelect
 *	Expected Result : ERR_OK
 *	Type			: Main Type (DVB)
 *	Comments		: DVB Type and AR
 */
void svc_si_TcSubtitleSelectTypeDvbAndAr1(Cutest_TestCase_t *pstTc)
{
	HERROR					 hErr;
	SvcSi_SelectedEs_t		 stSelectedSubtitle;
	const HxList_t			*pstList;
	const svcSi_PmtEsInfo_t	*pstPmtEsInfo;
	const SvcSi_Config_t	*pstConfig;

	// 1. Setup Fixture
	svc_si_UtSubtitleEsSetScenario (pstTc);		//	Set Up Test Stub
	pstList	= svc_si_UtSubtitleEsGetInfo (	5,		//	ItemNum
											29, // [ Lang : "fin", MainType : "EBU", HOH : FALSE, AR : "n/a", HD : "n/a" ]
											4,  // [ Lang : "eng", MainType : "DVB", HOH : FALSE, AR : "no", HD : "no" ]
											5,  // [ Lang : "eng", MainType : "DVB", HOH : FALSE, AR : "4x3", HD : "no" ]
											6,  // [ Lang : "eng", MainType : "DVB", HOH : FALSE, AR : "16x9", HD : "no" ]
											11, // [ Lang : "eng", MainType : "DVB", HOH : TRUE, AR : "16x9", HD : "no" ]
											(HUINT32)-1);
	pstPmtEsInfo	= svc_si_UtSubtitleEsSetList (NULL, pstList);
	pstConfig		= svc_si_UtSubtitleEsGetConfig (	"eng",	"dan",	"ger",	// szLang0[], szLang1[], szOsdLang[],
														"", 					// szLangDefault[],
														eDxASPECTRATIO_16X9,	// eAspectRatio,
														TRUE,	FALSE,			// bDvbMenu, bHohMenu,
														"", 	FALSE); 		// szLangUser, bHohUser
	svc_si_UtInitSelectedEs (&stSelectedSubtitle);

	// 2. Exercise
	hErr = svc_si_UtSubtitleEsSelect (pstTc, TRUE, pstPmtEsInfo, pstConfig, &stSelectedSubtitle);

	// 3. Verify
	svc_si_UtSubtitleEsVeritySelectOutput(pstTc, hErr, &stSelectedSubtitle, 3);

	// 4. Teardown
	svc_si_UtFreeConfig ((SvcSi_Config_t *)pstConfig);
	svc_si_UtFreeEsInfo ((svcSi_PmtEsInfo_t *)pstPmtEsInfo);

	return;
}

/*	SUT 			: svc_si_UtSubtitleEsSelect
 *	Expected Result : ERR_OK
 *	Type			: Main Type (DVB)
 *	Comments		: DVB Type and AR
 *					: In same condition (Lang, MainType, AR) HOH has priority
 *					: HOH > Lang > MainType > AR
 */
void svc_si_TcSubtitleSelectTypeDvbAndAr2(Cutest_TestCase_t *pstTc)
{
	HERROR					 hErr;
	SvcSi_SelectedEs_t		 stSelectedSubtitle;
	const HxList_t			*pstList;
	const svcSi_PmtEsInfo_t	*pstPmtEsInfo;
	const SvcSi_Config_t	*pstConfig;

	// 1. Setup Fixture
	svc_si_UtSubtitleEsSetScenario (pstTc);		//	Set Up Test Stub
	pstList	= svc_si_UtSubtitleEsGetInfo (	5,		//	ItemNum
											29, // [ Lang : "fin", MainType : "EBU", HOH : FALSE, AR : "n/a", HD : "n/a" ]
											4,  // [ Lang : "eng", MainType : "DVB", HOH : FALSE, AR : "no", HD : "no" ]
											5,  // [ Lang : "eng", MainType : "DVB", HOH : FALSE, AR : "4x3", HD : "no" ]
											6,  // [ Lang : "eng", MainType : "DVB", HOH : FALSE, AR : "16x9", HD : "no" ]
											11, // [ Lang : "eng", MainType : "DVB", HOH : TRUE, AR : "16x9", HD : "no" ]
											(HUINT32)-1);
	pstPmtEsInfo	= svc_si_UtSubtitleEsSetList (NULL, pstList);
	pstConfig		= svc_si_UtSubtitleEsGetConfig (	"eng",	"dan",	"ger",	// szLang0[], szLang1[], szOsdLang[],
														"", 					// szLangDefault[],
														eDxASPECTRATIO_16X9,	// eAspectRatio,
														TRUE,	TRUE,			// bDvbMenu, bHohMenu,
														"", 	FALSE); 		// szLangUser, bHohUser
	svc_si_UtInitSelectedEs (&stSelectedSubtitle);

	// 2. Exercise
	hErr = svc_si_UtSubtitleEsSelect (pstTc, TRUE, pstPmtEsInfo, pstConfig, &stSelectedSubtitle);

	// 3. Verify
	svc_si_UtSubtitleEsVeritySelectOutput(pstTc, hErr, &stSelectedSubtitle, 4);

	// 4. Teardown
	svc_si_UtFreeConfig ((SvcSi_Config_t *)pstConfig);
	svc_si_UtFreeEsInfo ((svcSi_PmtEsInfo_t *)pstPmtEsInfo);

	return;
}

#define ___ES_SUBTITLE_TEST_CASE_AR___

/*	SUT 			: svc_si_UtSubtitleEsSelect
 *	Expected Result : ERR_OK
 *	Type			: AR Type
 *	Comments		: Find Matching AR
 */
void svc_si_TcSubtitleSelectAr1(Cutest_TestCase_t *pstTc)
{
	HERROR					 hErr;
	SvcSi_SelectedEs_t		 stSelectedSubtitle;
	const HxList_t			*pstList;
	const svcSi_PmtEsInfo_t	*pstPmtEsInfo;
	const SvcSi_Config_t	*pstConfig;

	// 1. Setup Fixture
	svc_si_UtSubtitleEsSetScenario (pstTc);		//	Set Up Test Stub
	pstList	= svc_si_UtSubtitleEsGetInfo (	4,		//	ItemNum
											29, // [ Lang : "fin", MainType : "EBU", HOH : FALSE, AR : "n/a", HD : "n/a" ]
											5,  // [ Lang : "eng", MainType : "DVB", HOH : FALSE, AR : "4x3", HD : "no" ]
											6,  // [ Lang : "eng", MainType : "DVB", HOH : FALSE, AR : "16x9", HD : "no" ]
											7,  // [ Lang : "eng", MainType : "DVB", HOH : FALSE, AR : "2.21x1", HD : "no" ]
											(HUINT32)-1);
	pstPmtEsInfo	= svc_si_UtSubtitleEsSetList (NULL, pstList);
	pstConfig		= svc_si_UtSubtitleEsGetConfig (	"eng",	"dan",	"ger",	// szLang0[], szLang1[], szOsdLang[],
														"", 					// szLangDefault[],
														eDxASPECTRATIO_16X9,	// eAspectRatio,
														FALSE,	FALSE,			// bDvbMenu, bHohMenu,
														"", 	FALSE); 		// szLangUser, bHohUser
	svc_si_UtInitSelectedEs (&stSelectedSubtitle);

	// 2. Exercise
	hErr = svc_si_UtSubtitleEsSelect (pstTc, TRUE, pstPmtEsInfo, pstConfig, &stSelectedSubtitle);

	// 3. Verify
	svc_si_UtSubtitleEsVeritySelectOutput(pstTc, hErr, &stSelectedSubtitle, 2);

	// 4. Teardown
	svc_si_UtFreeConfig ((SvcSi_Config_t *)pstConfig);
	svc_si_UtFreeEsInfo ((svcSi_PmtEsInfo_t *)pstPmtEsInfo);

	return;
}

/*	SUT 			: svc_si_UtSubtitleEsSelect
 *	Expected Result : ERR_OK
 *	Type			: AR Type
 *	Comments		: Find Matching AR
 */
void svc_si_TcSubtitleSelectAr2(Cutest_TestCase_t *pstTc)
{
	HERROR					 hErr;
	SvcSi_SelectedEs_t		 stSelectedSubtitle;
	const HxList_t			*pstList;
	const svcSi_PmtEsInfo_t	*pstPmtEsInfo;
	const SvcSi_Config_t	*pstConfig;

	// 1. Setup Fixture
	svc_si_UtSubtitleEsSetScenario (pstTc);		//	Set Up Test Stub
	pstList	= svc_si_UtSubtitleEsGetInfo (	4,		//	ItemNum
											29, // [ Lang : "fin", MainType : "EBU", HOH : FALSE, AR : "n/a", HD : "n/a" ]
											5,  // [ Lang : "eng", MainType : "DVB", HOH : FALSE, AR : "4x3", HD : "no" ]
											6,  // [ Lang : "eng", MainType : "DVB", HOH : FALSE, AR : "16x9", HD : "no" ]
											7,  // [ Lang : "eng", MainType : "DVB", HOH : FALSE, AR : "2.21x1", HD : "no" ]
											(HUINT32)-1);
	pstPmtEsInfo	= svc_si_UtSubtitleEsSetList (NULL, pstList);
	pstConfig		= svc_si_UtSubtitleEsGetConfig (	"eng",	"dan",	"ger",	// szLang0[], szLang1[], szOsdLang[],
														"", 					// szLangDefault[],
														eDxASPECTRATIO_4X3,		// eAspectRatio,
														FALSE,	FALSE,			// bDvbMenu, bHohMenu,
														"", 	FALSE); 		// szLangUser, bHohUser
	svc_si_UtInitSelectedEs (&stSelectedSubtitle);

	// 2. Exercise
	hErr = svc_si_UtSubtitleEsSelect (pstTc, TRUE, pstPmtEsInfo, pstConfig, &stSelectedSubtitle);

	// 3. Verify
	svc_si_UtSubtitleEsVeritySelectOutput(pstTc, hErr, &stSelectedSubtitle, 1);

	// 4. Teardown
	svc_si_UtFreeConfig ((SvcSi_Config_t *)pstConfig);
	svc_si_UtFreeEsInfo ((svcSi_PmtEsInfo_t *)pstPmtEsInfo);

	return;
}

/*	SUT 			: svc_si_UtSubtitleEsSelect
 *	Expected Result : ERR_OK
 *	Type			: AR Type and Lang
 *	Comments		: Lang > AR
 */
void svc_si_TcSubtitleSelectAr3(Cutest_TestCase_t *pstTc)
{
	HERROR					 hErr;
	SvcSi_SelectedEs_t		 stSelectedSubtitle;
	const HxList_t			*pstList;
	const svcSi_PmtEsInfo_t	*pstPmtEsInfo;
	const SvcSi_Config_t	*pstConfig;

	// 1. Setup Fixture
	svc_si_UtSubtitleEsSetScenario (pstTc);		//	Set Up Test Stub
	pstList	= svc_si_UtSubtitleEsGetInfo (	4,		//	ItemNum
											29, // [ Lang : "fin", MainType : "EBU", HOH : FALSE, AR : "n/a", HD : "n/a" ]
											5,  // [ Lang : "eng", MainType : "DVB", HOH : FALSE, AR : "4x3", HD : "no" ]
											6,  // [ Lang : "eng", MainType : "DVB", HOH : FALSE, AR : "16x9", HD : "no" ]
											16, // [ Lang : "nor", MainType : "DVB", HOH : TRUE, AR : "4x3", HD : "no" ]
											(HUINT32)-1);
	pstPmtEsInfo	= svc_si_UtSubtitleEsSetList (NULL, pstList);
	pstConfig		= svc_si_UtSubtitleEsGetConfig (	"rus",	"dan",	"fra",	// szLang0[], szLang1[], szOsdLang[],
														"", 					// szLangDefault[],
														eDxASPECTRATIO_16X9,	// eAspectRatio,
														FALSE,	FALSE,			// bDvbMenu, bHohMenu,
														"nor", 	FALSE); 		// szLangUser, bHohUser
	svc_si_UtInitSelectedEs (&stSelectedSubtitle);

	// 2. Exercise
	hErr = svc_si_UtSubtitleEsSelect (pstTc, TRUE, pstPmtEsInfo, pstConfig, &stSelectedSubtitle);

	// 3. Verify
	svc_si_UtSubtitleEsVeritySelectOutput(pstTc, hErr, &stSelectedSubtitle, 3);

	// 4. Teardown
	svc_si_UtFreeConfig ((SvcSi_Config_t *)pstConfig);
	svc_si_UtFreeEsInfo ((svcSi_PmtEsInfo_t *)pstPmtEsInfo);

	return;
}

#define ___ES_SUBTITLE_TEST_CASE_HD___

/*	SUT 			: svc_si_UtSubtitleEsSelect
 *	Expected Result : ERR_OK
 *	Type			: HD Type
 *	Comments		: Find HD Type
 */
void svc_si_TcSubtitleSelectHd1(Cutest_TestCase_t *pstTc)
{
	HERROR					 hErr;
	SvcSi_SelectedEs_t		 stSelectedSubtitle;
	const HxList_t			*pstList;
	const svcSi_PmtEsInfo_t	*pstPmtEsInfo;
	const SvcSi_Config_t	*pstConfig;

	// 1. Setup Fixture
	svc_si_UtSubtitleEsSetScenario (pstTc);		//	Set Up Test Stub
	pstList	= svc_si_UtSubtitleEsGetInfo (	4,		//	ItemNum
											29, // [ Lang : "fin", MainType : "EBU", HOH : FALSE, AR : "n/a", HD : "n/a" ]
											8,  // [ Lang : "eng", MainType : "DVB", HOH : FALSE, AR : "n/a", HD : "yes" ]
											6,  // [ Lang : "eng", MainType : "DVB", HOH : FALSE, AR : "16x9", HD : "no" ]
											18, // [ Lang : "nor", MainType : "DVB", HOH : TRUE, AR : "4x3", HD : "no" ]
											(HUINT32)-1);
	pstPmtEsInfo	= svc_si_UtSubtitleEsSetList (NULL, pstList);
	pstConfig		= svc_si_UtSubtitleEsGetConfig (	"rus",	"dan",	"fra",	// szLang0[], szLang1[], szOsdLang[],
														"", 					// szLangDefault[],
														eDxASPECTRATIO_16X9,	// eAspectRatio,
														FALSE,	FALSE,			// bDvbMenu, bHohMenu,
														"", 	FALSE); 		// szLangUser, bHohUser
	svc_si_UtInitSelectedEs (&stSelectedSubtitle);

	// 2. Exercise
	hErr = svc_si_UtSubtitleEsSelect (pstTc, TRUE, pstPmtEsInfo, pstConfig, &stSelectedSubtitle);

	// 3. Verify
	svc_si_UtSubtitleEsVeritySelectOutput(pstTc, hErr, &stSelectedSubtitle, 1);

	// 4. Teardown
	svc_si_UtFreeConfig ((SvcSi_Config_t *)pstConfig);
	svc_si_UtFreeEsInfo ((svcSi_PmtEsInfo_t *)pstPmtEsInfo);

	return;
}


/*	SUT 			: svc_si_UtSubtitleEsSelect
 *	Expected Result : ERR_OK
 *	Type			: HD Type and Proper AR
 *	Comments		: Find HD Type and AR
 *					: HD on and No AR
 *					: HD > AR
 */
void svc_si_TcSubtitleSelectHd2(Cutest_TestCase_t *pstTc)
{
	HERROR					 hErr;
	SvcSi_SelectedEs_t		 stSelectedSubtitle;
	const HxList_t			*pstList;
	const svcSi_PmtEsInfo_t	*pstPmtEsInfo;
	const SvcSi_Config_t	*pstConfig;

	// 1. Setup Fixture
	svc_si_UtSubtitleEsSetScenario (pstTc);		//	Set Up Test Stub
	pstList	= svc_si_UtSubtitleEsGetInfo (	4,		//	ItemNum
											29, // [ Lang : "fin", MainType : "EBU", HOH : FALSE, AR : "n/a", HD : "n/a" ]
											8,  // [ Lang : "eng", MainType : "DVB", HOH : FALSE, AR : "n/a", HD : "yes" ]
											11, // [ Lang : "eng", MainType : "DVB", HOH : TRUE, AR : "16x9", HD : "no" ]
											13, // [ Lang : "eng", MainType : "DVB", HOH : TRUE, AR : "n/a", HD : "yes" ]
											(HUINT32)-1);
	pstPmtEsInfo	= svc_si_UtSubtitleEsSetList (NULL, pstList);
	pstConfig		= svc_si_UtSubtitleEsGetConfig (	"eng",	"dan",	"fra",	// szLang0[], szLang1[], szOsdLang[],
														"", 					// szLangDefault[],
														eDxASPECTRATIO_16X9,	// eAspectRatio,
														FALSE,	TRUE,			// bDvbMenu, bHohMenu,
														"", 	FALSE); 		// szLangUser, bHohUser
	svc_si_UtInitSelectedEs (&stSelectedSubtitle);

	// 2. Exercise
	hErr = svc_si_UtSubtitleEsSelect (pstTc, TRUE, pstPmtEsInfo, pstConfig, &stSelectedSubtitle);

	// 3. Verify
	svc_si_UtSubtitleEsVeritySelectOutput(pstTc, hErr, &stSelectedSubtitle, 3);

	// 4. Teardown
	svc_si_UtFreeConfig ((SvcSi_Config_t *)pstConfig);
	svc_si_UtFreeEsInfo ((svcSi_PmtEsInfo_t *)pstPmtEsInfo);

	return;
}

#define ___ES_SUBTITLE_TEST_CASE_HOH___

/*	SUT 			: svc_si_UtSubtitleEsSelect
 *	Expected Result : ERR_OK
 *	Type			: Hoh Type
 *	Comments		: Find Hoh Type
 *					: HoH > Language
 */
void svc_si_TcSubtitleSelectHohOn1(Cutest_TestCase_t *pstTc)
{
	HERROR					 hErr;
	SvcSi_SelectedEs_t		 stSelectedSubtitle;
	const HxList_t			*pstList;
	const svcSi_PmtEsInfo_t	*pstPmtEsInfo;
	const SvcSi_Config_t	*pstConfig;

	// 1. Setup Fixture
	svc_si_UtSubtitleEsSetScenario (pstTc);		//	Set Up Test Stub
	pstList	= svc_si_UtSubtitleEsGetInfo (	3,		//	ItemNum
											29, // [ Lang : "fin", MainType : "EBU", HOH : FALSE, AR : "n/a", HD : "n/a" ]
											8,  // [ Lang : "eng", MainType : "DVB", HOH : FALSE, AR : "n/a", HD : "yes" ]
											18, // [ Lang : "nor", MainType : "DVB", HOH : TRUE, AR : "4x3", HD : "no" ]
											(HUINT32)-1);
	pstPmtEsInfo	= svc_si_UtSubtitleEsSetList (NULL, pstList);
	pstConfig		= svc_si_UtSubtitleEsGetConfig (	"rus",	"dan",	"fra",	// szLang0[], szLang1[], szOsdLang[],
														"", 					// szLangDefault[],
														eDxASPECTRATIO_16X9,	// eAspectRatio,
														TRUE,	TRUE,			// bDvbMenu, bHohMenu,
														"eng", 	FALSE); 		// szLangUser, bHohUser
	svc_si_UtInitSelectedEs (&stSelectedSubtitle);

	// 2. Exercise
	hErr = svc_si_UtSubtitleEsSelect (pstTc, TRUE, pstPmtEsInfo, pstConfig, &stSelectedSubtitle);

	// 3. Verify
	svc_si_UtSubtitleEsVeritySelectOutput(pstTc, hErr, &stSelectedSubtitle, 2);

	// 4. Teardown
	svc_si_UtFreeConfig ((SvcSi_Config_t *)pstConfig);
	svc_si_UtFreeEsInfo ((svcSi_PmtEsInfo_t *)pstPmtEsInfo);

	return;
}

/*	SUT 			: svc_si_UtSubtitleEsSelect
 *	Expected Result : ERR_OK
 *	Type			: Hoh Type
 *	Comments		: Find Hoh Type
 *					:
 */
void svc_si_TcSubtitleSelectHohOn2(Cutest_TestCase_t *pstTc)
{
	HERROR					 hErr;
	SvcSi_SelectedEs_t		 stSelectedSubtitle;
	const HxList_t			*pstList;
	const svcSi_PmtEsInfo_t	*pstPmtEsInfo;
	const SvcSi_Config_t	*pstConfig;

	// 1. Setup Fixture
	svc_si_UtSubtitleEsSetScenario (pstTc);		//	Set Up Test Stub
	pstList	= svc_si_UtSubtitleEsGetInfo (	4,		//	ItemNum
											29, // [ Lang : "fin", MainType : "EBU", HOH : FALSE, AR : "n/a", HD : "n/a" ]
											8,  // [ Lang : "eng", MainType : "DVB", HOH : FALSE, AR : "n/a", HD : "yes" ]
											13, // [ Lang : "eng", MainType : "DVB", HOH : TRUE, AR : "n/a", HD : "yes" ]
											18, // [ Lang : "nor", MainType : "DVB", HOH : TRUE, AR : "4x3", HD : "no" ]
											(HUINT32)-1);
	pstPmtEsInfo	= svc_si_UtSubtitleEsSetList (NULL, pstList);
	pstConfig		= svc_si_UtSubtitleEsGetConfig (	"rus",	"dan",	"fra",	// szLang0[], szLang1[], szOsdLang[],
														"", 					// szLangDefault[],
														eDxASPECTRATIO_16X9,	// eAspectRatio,
														TRUE,	TRUE,			// bDvbMenu, bHohMenu,
														"", 	FALSE); 		// szLangUser, bHohUser
	svc_si_UtInitSelectedEs (&stSelectedSubtitle);

	// 2. Exercise
	hErr = svc_si_UtSubtitleEsSelect (pstTc, TRUE, pstPmtEsInfo, pstConfig, &stSelectedSubtitle);

	// 3. Verify
	svc_si_UtSubtitleEsVeritySelectOutput(pstTc, hErr, &stSelectedSubtitle, 2);

	// 4. Teardown
	svc_si_UtFreeConfig ((SvcSi_Config_t *)pstConfig);
	svc_si_UtFreeEsInfo ((svcSi_PmtEsInfo_t *)pstPmtEsInfo);

	return;
}

/*	SUT 			: svc_si_UtSubtitleEsSelect
 *	Expected Result : ERR_OK
 *	Type			: Hoh Type
 *	Comments		: Find Hoh Type but HoH is not exists
 *					: HoH > Lang
 */
void svc_si_TcSubtitleSelectHohOn3(Cutest_TestCase_t *pstTc)
{
	HERROR					 hErr;
	SvcSi_SelectedEs_t		 stSelectedSubtitle;
	const HxList_t			*pstList;
	const svcSi_PmtEsInfo_t	*pstPmtEsInfo;
	const SvcSi_Config_t	*pstConfig;

	// 1. Setup Fixture
	svc_si_UtSubtitleEsSetScenario (pstTc);		//	Set Up Test Stub
	pstList	= svc_si_UtSubtitleEsGetInfo (	4,		//	ItemNum
											29, // [ Lang : "fin", MainType : "EBU", HOH : FALSE, AR : "n/a", HD : "n/a" ]
											8,  // [ Lang : "eng", MainType : "DVB", HOH : FALSE, AR : "n/a", HD : "yes" ]
											24, // [ Lang : "swe", MainType : "EBU", HOH : FALSE, AR : "n/a", HD : "n/a" ]
											16, // [ Lang : "nor", MainType : "DVB", HOH : FALSE, AR : "4x3", HD : "no" ]
											(HUINT32)-1);
	pstPmtEsInfo	= svc_si_UtSubtitleEsSetList (NULL, pstList);
	pstConfig		= svc_si_UtSubtitleEsGetConfig (	"rus",	"dan",	"fra",	// szLang0[], szLang1[], szOsdLang[],
														"", 					// szLangDefault[],
														eDxASPECTRATIO_16X9,	// eAspectRatio,
														TRUE,	TRUE,			// bDvbMenu, bHohMenu,
														"", 	FALSE); 		// szLangUser, bHohUser
	svc_si_UtInitSelectedEs (&stSelectedSubtitle);

	// 2. Exercise
	hErr = svc_si_UtSubtitleEsSelect (pstTc, TRUE, pstPmtEsInfo, pstConfig, &stSelectedSubtitle);

	// 3. Verify
	svc_si_UtSubtitleEsVeritySelectOutput(pstTc, hErr, &stSelectedSubtitle, 1);

	// 4. Teardown
	svc_si_UtFreeConfig ((SvcSi_Config_t *)pstConfig);
	svc_si_UtFreeEsInfo ((svcSi_PmtEsInfo_t *)pstPmtEsInfo);

	return;
}

/*	SUT 			: svc_si_UtSubtitleEsSelect
 *	Expected Result : ERR_OK
 *	Type			: Hoh Type
 *	Comments		: Ignore Hoh Type
 *					: With HoH menu item Off HoH Type has lower priorty then normal type
 */
void svc_si_TcSubtitleSelectHohOff1(Cutest_TestCase_t *pstTc)
{
	HERROR					 hErr;
	SvcSi_SelectedEs_t		 stSelectedSubtitle;
	const HxList_t			*pstList;
	const svcSi_PmtEsInfo_t	*pstPmtEsInfo;
	const SvcSi_Config_t	*pstConfig;

	// 1. Setup Fixture
	svc_si_UtSubtitleEsSetScenario (pstTc);		//	Set Up Test Stub
	pstList	= svc_si_UtSubtitleEsGetInfo (	4,		//	ItemNum
											29, // [ Lang : "fin", MainType : "EBU", HOH : FALSE, AR : "n/a", HD : "n/a" ]
											8,  // [ Lang : "eng", MainType : "DVB", HOH : FALSE, AR : "n/a", HD : "yes" ]
											13, // [ Lang : "eng", MainType : "DVB", HOH : TRUE, AR : "n/a", HD : "yes" ]
											18, // [ Lang : "nor", MainType : "DVB", HOH : TRUE, AR : "4x3", HD : "no" ]
											(HUINT32)-1);
	pstPmtEsInfo	= svc_si_UtSubtitleEsSetList (NULL, pstList);
	pstConfig		= svc_si_UtSubtitleEsGetConfig (	"rus",	"dan",	"fra",	// szLang0[], szLang1[], szOsdLang[],
														"eng", 					// szLangDefault[],
														eDxASPECTRATIO_16X9,	// eAspectRatio,
														TRUE,	FALSE,			// bDvbMenu, bHohMenu,
														"", 	FALSE); 		// szLangUser, bHohUser
	svc_si_UtInitSelectedEs (&stSelectedSubtitle);

	// 2. Exercise
	hErr = svc_si_UtSubtitleEsSelect (pstTc, TRUE, pstPmtEsInfo, pstConfig, &stSelectedSubtitle);

	// 3. Verify
	svc_si_UtSubtitleEsVeritySelectOutput(pstTc, hErr, &stSelectedSubtitle, 1);

	// 4. Teardown
	svc_si_UtFreeConfig ((SvcSi_Config_t *)pstConfig);
	svc_si_UtFreeEsInfo ((svcSi_PmtEsInfo_t *)pstPmtEsInfo);

	return;
}

/*	SUT 			: svc_si_UtSubtitleEsSelect
 *	Expected Result : ERR_OK
 *	Type			: Hoh Type
 *	Comments		: Ignore Hoh Type
 *					: With HoH menu item Off HoH Type has lower priorty then normal type
 */
void svc_si_TcSubtitleSelectHohOff2(Cutest_TestCase_t *pstTc)
{
	HERROR					 hErr;
	SvcSi_SelectedEs_t		 stSelectedSubtitle;
	const HxList_t			*pstList;
	const svcSi_PmtEsInfo_t	*pstPmtEsInfo;
	const SvcSi_Config_t	*pstConfig;

	// 1. Setup Fixture
	svc_si_UtSubtitleEsSetScenario (pstTc);		//	Set Up Test Stub
	pstList	= svc_si_UtSubtitleEsGetInfo (	3,		//	ItemNum
											29, // [ Lang : "fin", MainType : "EBU", HOH : FALSE, AR : "n/a", HD : "n/a" ]
											13, // [ Lang : "eng", MainType : "DVB", HOH : TRUE, AR : "n/a", HD : "yes" ]
											18, // [ Lang : "nor", MainType : "DVB", HOH : TRUE, AR : "4x3", HD : "no" ]
											(HUINT32)-1);
	pstPmtEsInfo	= svc_si_UtSubtitleEsSetList (NULL, pstList);
	pstConfig		= svc_si_UtSubtitleEsGetConfig (	"rus",	"dan",	"fra",	// szLang0[], szLang1[], szOsdLang[],
														"eng", 					// szLangDefault[],
														eDxASPECTRATIO_16X9,	// eAspectRatio,
														TRUE,	FALSE,			// bDvbMenu, bHohMenu,
														"eng", 	FALSE); 		// szLangUser, bHohUser
	svc_si_UtInitSelectedEs (&stSelectedSubtitle);

	// 2. Exercise
	hErr = svc_si_UtSubtitleEsSelect (pstTc, TRUE, pstPmtEsInfo, pstConfig, &stSelectedSubtitle);

	// 3. Verify
	svc_si_UtSubtitleEsVeritySelectOutput(pstTc, hErr, &stSelectedSubtitle, 0);

	// 4. Teardown
	svc_si_UtFreeConfig ((SvcSi_Config_t *)pstConfig);
	svc_si_UtFreeEsInfo ((svcSi_PmtEsInfo_t *)pstPmtEsInfo);

	return;
}

/*	SUT 			: svc_si_UtSubtitleEsSelect
 *	Expected Result : ERR_OK
 *	Type			: Hoh Type
 *	Comments		: Ignore Hoh Type
 *					: HoH > Lang > MainType(DVB, EBU)
 */
void svc_si_TcSubtitleSelectHohOff3(Cutest_TestCase_t *pstTc)
{
	HERROR					 hErr;
	SvcSi_SelectedEs_t		 stSelectedSubtitle;
	const HxList_t			*pstList;
	const svcSi_PmtEsInfo_t	*pstPmtEsInfo;
	const SvcSi_Config_t	*pstConfig;

	// 1. Setup Fixture
	svc_si_UtSubtitleEsSetScenario (pstTc);		//	Set Up Test Stub
	pstList	= svc_si_UtSubtitleEsGetInfo (	4,		//	ItemNum
											29, // [ Lang : "fin", MainType : "EBU", HOH : FALSE, AR : "n/a", HD : "n/a" ]
											0,  // [ Lang : "eng", MainType : "EBU", HOH : FALSE, AR : "n/a", HD : "n/a" ]
											4,  // [ Lang : "eng", MainType : "DVB", HOH : FALSE, AR : "no", HD : "no" ]
											18, // [ Lang : "nor", MainType : "DVB", HOH : TRUE, AR : "4x3", HD : "no" ]
											(HUINT32)-1);
	pstPmtEsInfo	= svc_si_UtSubtitleEsSetList (NULL, pstList);
	pstConfig		= svc_si_UtSubtitleEsGetConfig (	"rus",	"dan",	"fra",	// szLang0[], szLang1[], szOsdLang[],
														"eng", 					// szLangDefault[],
														eDxASPECTRATIO_16X9,	// eAspectRatio,
														TRUE,	FALSE,			// bDvbMenu, bHohMenu,
														"", 	FALSE); 		// szLangUser, bHohUser
	svc_si_UtInitSelectedEs (&stSelectedSubtitle);

	// 2. Exercise
	hErr = svc_si_UtSubtitleEsSelect (pstTc, TRUE, pstPmtEsInfo, pstConfig, &stSelectedSubtitle);

	// 3. Verify
	svc_si_UtSubtitleEsVeritySelectOutput(pstTc, hErr, &stSelectedSubtitle, 2);

	// 4. Teardown
	svc_si_UtFreeConfig ((SvcSi_Config_t *)pstConfig);
	svc_si_UtFreeEsInfo ((svcSi_PmtEsInfo_t *)pstPmtEsInfo);

	return;
}

#define ___ES_SUBTITLE_TEST_CASE_HOH_AND_AR___

/*	SUT 			: svc_si_UtSubtitleEsSelect
 *	Expected Result : ERR_OK
 *	Type			: Hoh On and AR 4x3
 *	Comments		: Find Hoh Type and Proper AR
 */
void svc_si_TcSubtitleSelectHohAndAr1(Cutest_TestCase_t *pstTc)
{
	HERROR					 hErr;
	SvcSi_SelectedEs_t		 stSelectedSubtitle;
	const HxList_t			*pstList;
	const svcSi_PmtEsInfo_t	*pstPmtEsInfo;
	const SvcSi_Config_t	*pstConfig;

	// 1. Setup Fixture
	svc_si_UtSubtitleEsSetScenario (pstTc);		//	Set Up Test Stub
	pstList	= svc_si_UtSubtitleEsGetInfo (	5,		//	ItemNum
											29, // [ Lang : "fin", MainType : "EBU", HOH : FALSE, AR : "n/a", HD : "n/a" ]
											3,  // [ Lang : "eng", MainType : "EBU", HOH : TRUE, AR : "n/a", HD : "n/a" ]
											5,  // [ Lang : "eng", MainType : "DVB", HOH : FALSE, AR : "4x3", HD : "no" ]
											10, // [ Lang : "eng", MainType : "DVB", HOH : TRUE, AR : "4x3", HD : "no" ]
											11, // [ Lang : "eng", MainType : "DVB", HOH : TRUE, AR : "16x9", HD : "no" ]
											(HUINT32)-1);
	pstPmtEsInfo	= svc_si_UtSubtitleEsSetList (NULL, pstList);
	pstConfig		= svc_si_UtSubtitleEsGetConfig (	"eng",	"dan",	"fra",	// szLang0[], szLang1[], szOsdLang[],
														"", 					// szLangDefault[],
														eDxASPECTRATIO_4X3,		// eAspectRatio,
														TRUE,	TRUE,			// bDvbMenu, bHohMenu,
														"", 	FALSE); 		// szLangUser, bHohUser
	svc_si_UtInitSelectedEs (&stSelectedSubtitle);

	// 2. Exercise
	hErr = svc_si_UtSubtitleEsSelect (pstTc, TRUE, pstPmtEsInfo, pstConfig, &stSelectedSubtitle);

	// 3. Verify
	svc_si_UtSubtitleEsVeritySelectOutput(pstTc, hErr, &stSelectedSubtitle, 3);

	// 4. Teardown
	svc_si_UtFreeConfig ((SvcSi_Config_t *)pstConfig);
	svc_si_UtFreeEsInfo ((svcSi_PmtEsInfo_t *)pstPmtEsInfo);

	return;
}


/*	SUT 			: svc_si_UtSubtitleEsSelect
 *	Expected Result : ERR_OK
 *	Type			: Hoh Off and AR 4x3
 *	Comments		: Find None Hoh Type and Proper AR
 */
void svc_si_TcSubtitleSelectHohAndAr2(Cutest_TestCase_t *pstTc)
{
	HERROR					 hErr;
	SvcSi_SelectedEs_t		 stSelectedSubtitle;
	const HxList_t			*pstList;
	const svcSi_PmtEsInfo_t	*pstPmtEsInfo;
	const SvcSi_Config_t	*pstConfig;

	// 1. Setup Fixture
	svc_si_UtSubtitleEsSetScenario (pstTc);		//	Set Up Test Stub
	pstList	= svc_si_UtSubtitleEsGetInfo (	5,		//	ItemNum
											29, // [ Lang : "fin", MainType : "EBU", HOH : FALSE, AR : "n/a", HD : "n/a" ]
											3,  // [ Lang : "eng", MainType : "EBU", HOH : TRUE, AR : "n/a", HD : "n/a" ]
											5,  // [ Lang : "eng", MainType : "DVB", HOH : FALSE, AR : "4x3", HD : "no" ]
											10, // [ Lang : "eng", MainType : "DVB", HOH : TRUE, AR : "4x3", HD : "no" ]
											11, // [ Lang : "eng", MainType : "DVB", HOH : TRUE, AR : "16x9", HD : "no" ]
											(HUINT32)-1);
	pstPmtEsInfo	= svc_si_UtSubtitleEsSetList (NULL, pstList);
	pstConfig		= svc_si_UtSubtitleEsGetConfig (	"eng",	"dan",	"fra",	// szLang0[], szLang1[], szOsdLang[],
														"", 					// szLangDefault[],
														eDxASPECTRATIO_4X3,		// eAspectRatio,
														TRUE,	FALSE,			// bDvbMenu, bHohMenu,
														"", 	FALSE); 		// szLangUser, bHohUser
	svc_si_UtInitSelectedEs (&stSelectedSubtitle);

	// 2. Exercise
	hErr = svc_si_UtSubtitleEsSelect (pstTc, TRUE, pstPmtEsInfo, pstConfig, &stSelectedSubtitle);

	// 3. Verify
	svc_si_UtSubtitleEsVeritySelectOutput(pstTc, hErr, &stSelectedSubtitle, 2);

	// 4. Teardown
	svc_si_UtFreeConfig ((SvcSi_Config_t *)pstConfig);
	svc_si_UtFreeEsInfo ((svcSi_PmtEsInfo_t *)pstPmtEsInfo);

	return;
}


/*	SUT 			: svc_si_UtSubtitleEsSelect
 *	Expected Result : ERR_OK
 *	Type			: Hoh On and AR 16x9
 *	Comments		: Find Hoh Type and AR 16x9
 *					: Where is no HOH On And AR 16x9 ES
 *					: HOH > AR
 */
void svc_si_TcSubtitleSelectHohAndAr3(Cutest_TestCase_t *pstTc)
{
	HERROR					 hErr;
	SvcSi_SelectedEs_t		 stSelectedSubtitle;
	const HxList_t			*pstList;
	const svcSi_PmtEsInfo_t	*pstPmtEsInfo;
	const SvcSi_Config_t	*pstConfig;

	// 1. Setup Fixture
	svc_si_UtSubtitleEsSetScenario (pstTc);		//	Set Up Test Stub
	pstList	= svc_si_UtSubtitleEsGetInfo (	5,		//	ItemNum
											29, // [ Lang : "fin", MainType : "EBU", HOH : FALSE, AR : "n/a", HD : "n/a" ]
											3,  // [ Lang : "eng", MainType : "EBU", HOH : TRUE, AR : "n/a", HD : "n/a" ]
											5,  // [ Lang : "eng", MainType : "DVB", HOH : FALSE, AR : "4x3", HD : "no" ]
											10, // [ Lang : "eng", MainType : "DVB", HOH : TRUE, AR : "4x3", HD : "no" ]
											11, // [ Lang : "eng", MainType : "DVB", HOH : TRUE, AR : "16x9", HD : "no" ]
											(HUINT32)-1);
	pstPmtEsInfo	= svc_si_UtSubtitleEsSetList (NULL, pstList);
	pstConfig		= svc_si_UtSubtitleEsGetConfig (	"eng",	"dan",	"fra",	// szLang0[], szLang1[], szOsdLang[],
														"", 					// szLangDefault[],
														eDxASPECTRATIO_16X9,	// eAspectRatio,
														TRUE,	FALSE,			// bDvbMenu, bHohMenu,
														"", 	FALSE); 		// szLangUser, bHohUser
	svc_si_UtInitSelectedEs (&stSelectedSubtitle);

	// 2. Exercise
	hErr = svc_si_UtSubtitleEsSelect (pstTc, TRUE, pstPmtEsInfo, pstConfig, &stSelectedSubtitle);

	// 3. Verify
	svc_si_UtSubtitleEsVeritySelectOutput(pstTc, hErr, &stSelectedSubtitle, 2);

	// 4. Teardown
	svc_si_UtFreeConfig ((SvcSi_Config_t *)pstConfig);
	svc_si_UtFreeEsInfo ((svcSi_PmtEsInfo_t *)pstPmtEsInfo);

	return;
}

/* end of file */

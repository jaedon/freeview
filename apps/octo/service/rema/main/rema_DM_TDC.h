/*******************************************************************
	File Description
********************************************************************/
/**
	@file	  rema_util.h
	@brief

	Description:  									\n
	Module: REMA								\n

	Copyright (c) 2016 HUMAX Co., Ltd.				\n
	All rights reserved.							\n
*/

/*
* Character encoding.
*
* MS949
*
* This source file that uses MS949 encoding.
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



#ifndef	__REMA_DM_TDC_H__
#define	__REMA_DM_TDC_H__

/*******************************************************************/
/********************      Header Files    *************************/
/*******************************************************************/
#include <hlib.h>

#ifdef __cplusplus
extern "C"
{
#endif

/*******************************************************************/
/********************      Global Variables     ********************/
/*******************************************************************/

/*******************************************************************/
/********************      Macro Definition     ********************/
/*******************************************************************/
#define	CONNREQ_PORT			9090
#define XMPP_SERVER				"1.2.3.4"

#define TEST_ACS_SERVER			"https://87.51.34.73:7548/ACS-server/ACS"
//#define TEST_ACS_SERVER			"http://10.0.12.145:8080/openacs/acs"				// for openACS	
//#define TEST_ACS_SERVER			"http://192.168.1.5:8080/openacs/acs"				// for emulator open ACS
//#define TEST_ACS_SERVER			"http://192.168.1.2:8000/testacs"					// for testACS

#define PRODUCT_ACS_SERVER		"https://akilleus.nms.tele.dk:7548/ACS-server/ACS"

/*******************************************************************/
/********************      Typedef         *************************/
/*******************************************************************/


/*******************************************************************/
/********************      Public Functions     ********************/
/*******************************************************************/


/*******************************************************************/
/********************      extern Functions     ********************/
/*******************************************************************/

#ifdef __cplusplus
}
#endif



#endif //__REMA_CORE_H__

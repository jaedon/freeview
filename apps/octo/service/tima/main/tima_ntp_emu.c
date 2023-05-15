/*******************************************************************
	File Description
********************************************************************/
/**
	@file	  tima_ntp.c
	@brief

	Description:  									\n
	Module: TiMa NTP 								\n

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
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <vkernel.h>
/*
NTP Protocol 
Discription is in src dir.

0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1 2
+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
| LI| VN  | MODE| STRATUM       | POLL          | PRECISION     |
+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+

32 bits		|LI|VN |MODE|STRATUM|POLL|PRECISION|
32 bits		ROOT DELAY
32 bits		ROOT DISPERSION 
32 bits		REFERENCE IDENTIFIER 
64 bits		REFERENCE TIMESTAMP 
64 bits		ORIGINATE TIMESTAMP 
64 bits		RECEIVE TIMESTAMP 
64 bits		TRANSMIT TIMESTAMP 
96 bits		AUTHENTICATOR (optional)
*/

#define NTP_POS_LI 			30
#define NTP_POS_VN 			27
#define NTP_POS_MODE 		24
#define NTP_POS_STRATUM 	16
#define NTP_POS_POLL 		8
#define NTP_POS_PREC 		0

#define NTP_LI 				0
#define NTP_VN 				3
#define NTP_MODE 			3
#define NTP_STRATUM 		0
#define NTP_POLL 			4
#define NTP_PREC 			-6

#define __TIMA_NTP_PORT (123)
#define __TIMA_URL_MAX	(256)

#define __TIMA_NTP_MAX_TRY_CNT	(5)

#define LINUX_DEFAULT_SECOND  0x83AA7E80      /* 1970 - 1900 in seconds */


#if defined(CONFIG_PROD_OS_EMULATOR)
#define sockaddr_in emu_sockaddr_in
#define hostent		emu_hostent
#define timeval		emu_timeval
#endif

static HERROR tima_SetNtpTime(HUINT32 ulUnixTime)
{
	return ERR_OK;
}

static HUINT32 tima_SendOrgTime(int uSockfd)
{
	return ERR_OK;
}

static int tima_GetNtpTimeFromServer(HUINT32 *data, HUINT32 ulArrivalTime, HUINT32 ulOrgTime)
{
	return ERR_OK;
}

static void tima_FinishedNtpThread(void)
{
	return ERR_OK;
}

static void tima_NtpThread(void *arg)
{
	return ERR_OK;
}


HERROR TIMA_CreateNtp(HCHAR * ulURL)
{
	return ERR_OK;
}



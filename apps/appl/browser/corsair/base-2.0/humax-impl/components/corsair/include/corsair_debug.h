/** **********************************************************************************************************
	@file 		corsair_debug.h

	@date		2012/10/11
	@author		IPM-SW1T (tipmsw1@humaxdigital.com)
	@breif

	(c) 2011-2012 Humax Co., Ltd.
	This program is produced by Humax Co., Ltd. ("Humax") and
	the proprietary Software of Humax and its licensors. Humax provides you, as an Authorized Licensee,
	non-assignable, non-transferable and non-exclusive license to use this Software.
	You acknowledge that this Software contains valuable trade secrets of Humax and by using this Software
	you agree to the responsibility to take all reasonable efforts to protect the any information
	you receive from Humax. You are not permitted to duplicate, modify, distribute, sell or lease and
	reverse engineer or extract the source code of this Software unless you have Humax's written permission
	to do so.
	If you have no authorized license, discontinue using this Software immediately.

	THE SOFTWARE IS PROVIDED "AS IS" AND HUMAX MAKES NO PROMISES, REPRESENTATIONS OR WARRANTIES,
	EITHER EXPRESS, IMPLIED OR STATUTORY, OR OTHERWISE, WITH RESPECT TO THE SOFTWARE.
	IN NO EVENT SHALL HUMAX BE LIABLE FOR LOST PROFITS, REVENUES, OR DATA, FINANCIAL LOSSES OR
	INDIRECT, SPECIAL, CONSEQUENTIAL, EXEMPLARTY OR PUNITIVE DAMAGES WHATSOEVER RELATING TO YOUR USE OR
	INABILITY TO USE THE SOFTWARE.

	This License is effective until terminated. You may terminate this License at any tdebug
	by destroying all copies of the Software including all documentation. This License will terminate
	immediately without notice from Humax to you if you fail to comply with any provision of this License.
	Upon termination, you must destroy all copies of the Software and all documentation.

	The laws of the Republic of Korea will apply to any disputes arising out of or relating to
	this Copyright Notice. All claims arising out of or relating to this Copyright Notice will be litigated
	in the Seoul Central District Court, in the Republic of Korea.
*********************************************************************************************************** */

#ifndef __CORSAIR_DEBUG_H__
#define __CORSAIR_DEBUG_H__

/** **************************************************************************************
	@class	CorsairDebug
 	@brief 	this class is for printing log/setting var the Opera functions.
****************************************************************************************** */
#include "corsair_hbbtv_surface.h"
#include "oipf_surface.h"

class CorsairDebug {
public:
	CorsairDebug();
	virtual ~CorsairDebug();

	static void debugCoutMemInfo(void);
	static void debugCookieInfo(void);
	static void debugCookieDelete(void);
	static void debugStartDragonfly(const char *ip_addr, unsigned short port);
	static void debugStopDragonfly(void);
	static void debugGetUrl(void);
	static void debugSetUrl(const char* url);
	static void debugOpenUrl(const char* url);
	static void debugGetCertificate(void);
	static void debugOnDSMCC();
	static void debugOffDSMCC();
	static void debugReloadMainAppFromUSB();

private :
    static int getRSS(void);
};

/** **************************************************************************************
	@class	CorsairDebugThread
 	@brief 	this class is for debug command demon.
 ****************************************************************************************** */
class CorsairDebugThread {
public:
	static bool start();
	static bool stop();
};

#endif
/** **********************************************************************************************************
	@file 		humax_custom_key.h

	@date		2012/09/13
	@author		IPM-SW1T(tipmsw1@humaxdigital.com)
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

	This License is effective until terminated. You may terminate this License at any time
	by destroying all copies of the Software including all documentation. This License will terminate
	immediately without notice from Humax to you if you fail to comply with any provision of this License.
	Upon termination, you must destroy all copies of the Software and all documentation.

	The laws of the Republic of Korea will apply to any disputes arising out of or relating to
	this Copyright Notice. All claims arising out of or relating to this Copyright Notice will be litigated
	in the Seoul Central District Court, in the Republic of Korea.
*********************************************************************************************************** */

#ifndef __HUMAX_CUSTOM_KEY_H__
#define __HUMAX_CUSTOM_KEY_H__

#include "gogi_opera_api.h"

enum {
	GOGI_KEY_TVRADIO_TOGGLE = GOGI_KEY_NUMPAD2,
	GOGI_KEY_LIST,
	GOGI_KEY_SETTING,
	GOGI_KEY_OPTION,
	GOGI_KEY_PLAYER,
	GOGI_KEY_WIDE,
	GOGI_KEY_SEARCH,
	GOGI_KEY_RECORDINGS,
	//JAPAN
	GOGI_KEY_11,
	GOGI_KEY_12,
	GOGI_KEY_PROGRAM,
	GOGI_KEY_DATA_BROADCAST,
	GOGI_KEY_DIGITS,
	GOGI_KEY_VOD,
	GOGI_KEY_BROWSER,
	GOGI_KEY_DTT,
	GOGI_KEY_BS,
	GOGI_KEY_CATV,
	GOGI_KEY_LAST,
	GOGI_KEY_RECOMMAND,
	GOGI_KEY_ETC,
	GOGI_KEY_BROADCAST_NETWORK,
	GOGI_KEY_WPS,
	GOGI_KEY_HIDDEN_MENU,
	// MULTI-CUSTOM KEY
	GOGI_KEY_MULTI_CUSTOM,
	GOGI_KEY_MULTI_CUSTOM_SUCCESS,
	GOGI_KEY_MULTI_CUSTOM_FAIL,
	// Newly defined JAPAN
	GOGI_KEY_REPLAY,
	GOGI_KEY_SKIP,
	// front key
	GOGI_KEY_FRONT_UP ,    //GOGI_KEY_F16
	GOGI_KEY_FRONT_DOWN,
	GOGI_KEY_FRONT_LEFT,
	GOGI_KEY_FRONT_RIGHT,
	GOGI_KEY_FRONT_SW_REBOOT,   //
	GOGI_KEY_NETFLIX,

	//GOGI_KEY_HUMAX_CUSTOM_LAST =GOGI_KEY_RED  // GOGI_FIRST_KEY + 403
};
#endif

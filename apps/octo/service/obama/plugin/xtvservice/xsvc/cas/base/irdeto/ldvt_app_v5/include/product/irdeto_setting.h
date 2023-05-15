/*
 * (c) 2011-2012 Humax Co., Ltd.
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
#ifndef __IR_PRODUCT_SETTING_H__
#define __IR_PRODUCT_SETTING_H__

#if defined (CONFIG_PROD_ICORDHDPLUSIR)
	#include "icordhdplusir_setting.h"
#elif defined (CONFIG_PROD_IRHD5100C)
	#include "irhd5100c_setting.h"
#elif defined (CONFIG_PROD_IHDR5200C)
	#include "ihdr5200c_setting.h"
#elif defined (CONFIG_PROD_HD5500T)
	#include "hd5500t_setting.h"
#elif defined (CONFIG_PROD_IRUC)
	#include "iruc_setting.h"
#elif defined (CONFIG_PROD_IRUDTT)
	#include "irudtt_setting.h"
#elif defined (CONFIG_PROD_IRHD5100CTVT)
	#include "irhd5100ctvt_setting.h"
#elif defined (CONFIG_PROD_IR2020HD) || defined (CONFIG_PROD_IR2000HD)
	#include "ir2020hd_setting.h"
#elif defined (CONFIG_PROD_IRHD5100S)
	#include "irhd5100s_setting.h"
#elif defined (CONFIG_PROD_ICORDHDPLUSME) || defined(CONFIG_PROD_ICORDHDPLUSMEADM)
	#include "icordhdplusme_setting.h"
#elif defined (CONFIG_PROD_IRHD5200S)
	#include "irhd5200s_setting.h"
#elif defined (CONFIG_PROD_HDH100S)
	#include "hdh100s_setting.h"
#elif defined (CONFIG_PROD_HM9502HD)
	#include "hm9502hd_setting.h"
#elif defined (CONFIG_PROD_IR2000SD)
	#include "ir2000sd_setting.h"
#elif defined (CONFIG_PROD_SDHDR)
	#include "sdhdr_setting.h"
#elif defined (CONFIG_PROD_IRHD5300C)
	#include "irhd5300c_setting.h"
#elif defined (CONFIG_PROD_IRUSAT)
	#include "irusat_setting.h"
#elif defined (CONFIG_PROD_VTCHD06)
	#include "vtchd06_setting.h"
#elif defined (CONFIG_PROD_HDH100CPH)
	#include "hdh100cph_setting.h"
#elif defined (CONFIG_PROD_UDFOXHD)
	#include "udfoxhd_setting.h"
#elif defined (CONFIG_PROD_HDFREE)
	#include "hdfree_setting.h"
#elif defined (CONFIG_PROD_IR3020HD) || defined (CONFIG_PROD_IR3000HD)
	#include "ir3020hd_setting.h"
#elif defined (CONFIG_PROD_HDR1003S)
	#include "hdr1003s_setting.h"
#elif defined (CONFIG_PROD_HM9504HD)
	#include "hm9504hd_setting.h"
#elif defined (CONFIG_PROD_HDFREECCA)
	#include "hdfreecca_setting.h"
#elif defined (CONFIG_PROD_IR4000HD)   // TODO: shko just for build
	#include "ir4000hd_setting.h"
#elif defined (CONFIG_PROD_HMS1000S)   // TODO: shko just for build
	#include "ir4000hd_setting.h"  // TODO: nex change to ir4000hd hw . now ir3020hd_settinig.h
#elif defined (CONFIG_PROD_IHDR6100C)   // TODO: syji just for build
	#include "ihdr6100c_setting.h"
#elif defined (CONFIG_PROD_M1)
	#include "m1_setting.h"
#else
	#error "irdeto setting is not defined yet! See irdeto_setting.h"
#endif

#endif

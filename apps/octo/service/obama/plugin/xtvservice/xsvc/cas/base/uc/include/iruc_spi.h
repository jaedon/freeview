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

#ifndef __IRUC_SPI_H__
#define	__IRUC_SPI_H__

#include "ir_platform.h"

//For Stream SPI definition
#define IRUC_STREAM_MAX_NUM				(1)  //일단 1개

#if 1 // ciyoon  CCA- Pre DVT 테스트에서 stream이 부족하여 증가 시킴.   Review 필요
#define IRUC_STREAM_MAX_FILTER_PER_CHANNEL		(16)//(8)
#define IRUC_STREAM_MAX_CHANNEL_PER_DEMUX		(8)
#else
#define IRUC_STREAM_MAX_FILTER_PER_CHANNEL		(8)  // mutilple vendor id. * global, unique, shared, specifiec.. the num 8 is enough.
#define IRUC_STREAM_MAX_CHANNEL_PER_DEMUX		(8)  // uc 에서는 1 emm * vendor channel + ecm * x , 8 is enough.
#endif
#define IRUC_STREAM_MAX_TOTAL_CHANNEL		(IRUC_STREAM_MAX_CHANNEL_PER_DEMUX * IRUC_STREAM_MAX_NUM)
#define IRUC_STREAM_MAX_TOTAL_FILTER			(128) // 대략..  // 128
#define IRUC_STREAM_MAX_CONNECTION  			(1) //(8)

#define IRUC_STREAM_INVALID_REQUESET_ID  		(0xFFFF) //octo request id size is word.
#define IRUC_STREAM_INVALID_PALDSC_ID 		(0xFFFFFFFF)
#define IRUC_STREAM_MAX_COMPONENT  			(8)
#define IRUC_STREAM_FILTER_MAX_LEN 			(16)

#define IRUC_INVALID_FILTER_REFID 				(UC_INVALID_HANDLE_VALUE)

#if defined(CONFIG_MW_CAS_IRDETO_SECURE)
#define	IRUC_SECURE_CSSK_NUM		16
#endif

/*******************************************************************/
/********************      Functions     ********************************/
/*******************************************************************/
void 		CAS_IRUC_SPI_OS_Init(void);
void			CAS_IRUC_SPI_STREAM_Init(void);
void			CAS_IRUC_SPI_PS_Init(void);
uc_result		CAS_IRUC_SPI_TIMER_Init(void);
uc_result		CAS_IRUC_SPI_SC_Init(void);

#if defined(CONFIG_MW_CAS_IRDETO_SECURE)
HUINT8 *	CAS_IRUC_SPI_GetCSSK(void);
HBOOL	CAS_IRUC_SPI_GetEnabledCSSK(void);
void		CAS_IRUC_SPI_ClearCSSK(void);
#endif

#endif

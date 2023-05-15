/*******************************************************************
7	File Description
********************************************************************/
/**
	@file
	@brief

	Description:  									\n
	Module: PL/PROFILE					\n

	Copyright (c) 2012 HUMAX Co., Ltd.				\n
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
* ⓒ 2011-2012 Humax Co., Ltd.
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
*  CONSEQUENTIAL, EXEMPLARTY OR PUNITIVE DAMAGES WHATSOEVER RELATING TO YOUR USE OR INABILITY TO USE THE SOFTWARE.
*
* This License is effective until terminated. You may terminate this License at any time by destroying all copies
* of the Software including all documentation. This License will terminate immediately without notice from Humax
* to you if you fail to comply with any provision of this License. Upon termination, you must destroy all copies
* of the Software and all documentation.
*
* The laws of the Republic of Korea will apply to any disputes arising out of or relating to this Copyright Notice.
* All claims arising out of or relating to this Copyright Notice will be litigated in the Seoul Central District Court,
* in the Republic of Korea.
*/

#ifndef	___INT_STREAMING_H___
#define	___INT_STREAMING_H___

#include <vkernel.h>
#include <hlib.h>
#include <dlib.h>
#include <pl_common.h>
#include <pl_ch_list.h>
#include <pl_streaming.h>


/* *************************************************************************************************** */
#define ___LOCAL_INCLUDE________________________________________________________________________________________
/* *************************************************************************************************** */

/* format */
#include "int_include/int_pl_mediaprofile.h"
#include "int_include/int_pl_m3u8.h"
#include "int_include/int_pl_streamingkey.h"
#include "int_include/int_pl_ringbuf_context.h"

/* spec */
#include "int_include/int_pl_streaming_spec.h"



/* streaming */
#include "int_include/int_pl_streaming_context.h"
#include "int_include/int_pl_streaming_callback.h"
#include "int_include/int_pl_streaming_receive.h"
#include "int_include/int_pl_streaming_resource.h"
#include "int_include/int_pl_streaming_uds.h"


/* mode */
#include "int_include/int_pl_mode_satip.h"
#include "int_include/int_pl_mode_hls.h"
#include "int_include/int_pl_mode_bypass.h"


/* streaming context util */
#include "int_include/int_pl_streaming_context_util.h"


/* *************************************************************************************************** */
#define ___DEFINE________________________________________________________________________________________
/* *************************************************************************************************** */

#define PL_STREAMING_ALL_PID	0x8206
#define PL_STREAMING_NONE_PID	0x2000

#define SAMASLOT_INITVAL	0xFF820610

// 2014/11/13 mhkang: refer to http://svn.humaxdigital.com:3000/issues/98576
// Streaming Type에 따라 SAMA schedule duration을 달리가져감.
#define PL_STREAMING_SCHEDULE_HLS_DURATION         60

/* 2014/11/14 mhkang:
  hls에 비해서 client connection 상태를 일찍 알 수 있기 때문에 hls 60초보다는 작게 주었고
  모바일 device를 들고다니면서 wifi signal 범위 밖으로 왔다갔다할 수 있는데 이런 상황에서도
  어느정도는 연속해서 재생할 수 있는 여유를 주기 위해서 40초로 일단 정함. 변경할 수 있는 값임. 최소 10보다는 커야함.
*/
#define PL_STREAMING_SCHEDULE_NTLS_DURATION        40
#define PL_STREAMING_SCHEDULE_SATIP_DURATION       60
#define PL_STREAMING_SCHEDULE_DEFAULT_DURATION     60
#define PL_STREAMING_SCHEDULE_LOWER_BOUND          50

#define ENTER_FUNCTION	HxLOG_Info("(+)\r\n")
#define LEAVE_FUNCTION	HxLOG_Debug("(-)\r\n")

// 2015/3/1 mhkang: When you test PVR HLS.
// #define SUPPORT_PVR_HLS_SEEK

/* *************************************************************************************************** */
#define ___TYPEDEF_______________________________________________________________________________________
/* *************************************************************************************************** */



#if 0
#define PL_Streaming_Debug_Print PL_Streaming_Debug_FilePrint
#else
#define PL_Streaming_Debug_Print
#endif
/* *************************************************************************************************** */
#define ___FUNCTION_HEADER_______________________________________________________________________________
/* *************************************************************************************************** */



/* *************************************************************************************************** */
#define ___DEBUG_FUNCTION_HEADER_________________________________________________________________________
/* *************************************************************************************************** */

extern 	void	HOMMA_CMDRegister_Streaming(HCHAR *hCmdHandle);


extern	void 			PL_Streaming_Debug_PrintScheduleInfo(const DxSchedule_t	*pstSchedule, const HCHAR *pszFunction, const HINT32 nLine);
extern	void 			PL_Streaming_Debug_PrintTuningParam(const DxTuneParam_t *pstTuningParam, const HCHAR *pszFunction, const HINT32 nLine);
extern 	void			PL_Streaming_Debug_PrintPids(HINT32 nPIDsCount, HUINT16 usPIDs[], const HCHAR *pszFunction, const HINT32 nLine);


extern	HCHAR			*PL_Streaming_Debug_GetSetupStatusStr(PLStreaming_SetupStatus_e	eSetupStatus);
extern	HCHAR			*PL_Streaming_Debug_GetHomResultStr(HOM_Result_e	eHomResult);


extern	HOM_Result_e 	PL_Streaming_Debug_Init(void);
extern	void 			PL_Streaming_Debug_DeInit(void);
extern	void 			PL_Streaming_Debug_FilePrint(const HCHAR *format, ...);
extern	void			PL_Streaming_Debug_PrintMem(HINT32 nPid);

#endif	//___INT_STREAMING_H___

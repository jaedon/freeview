
/*******************************************************************
	File Description
********************************************************************/
/**
	@file	  layer_module.h File Name을 적습니다.
	@brief	  file_name & simple comment.

	Description: File에 대한 설명을 적습니다.		\n
	Module: Layer와 모듈 명을 적습니다.			 	\n
	Remarks : 										\n

	Copyright (c) 2008 HUMAX Co., Ltd.				\n
	All rights reserved.							\n
*/
#ifndef __TTX_DRCS_H__
#define	__TTX_DRCS_H__


/*******************************************************************/
/********************      Header Files    *************************/
/*******************************************************************/
//#include <octo_common.h>
#include <hlib.h>

#include "ttx_decoder.h"

/*******************************************************************/
/********************      Macro Definition     ********************/
/*******************************************************************/

/*******************************************************************/
/********************      Typedef         *************************/
/*******************************************************************/

typedef enum
{
	eTTX_DRCS_MODE_12x10x1,
	eTTX_DRCS_MODE_12x10x2,
	eTTX_DRCS_MODE_12x10x4,
	eTTX_DRCS_MODE_6x5x4,
	eTTX_DRCS_MODE_LAST
} TTX_DrcsMode_t;

typedef enum
{
	eTTX_USERDEF_BMP_ROTATE_L,
	eTTX_USERDEF_BMP_ROTATE_R,	
	eTTX_USERDEF_BMP_TTX_L,
	eTTX_USERDEF_BMP_TTX_R,
	eTTX_USERDEF_BMP_MIX_R,
	eTTX_USERDEF_BMP_INDEX_R,
	eTTX_USERDEF_BMP_TIME_R,
	eTTX_USERDEF_BMP_CANCEL_R,
	eTTX_USERDEF_BMP_DOUBLE_R,
	eTTX_USERDEF_BMP_HOLD_R,
	eTTX_USERDEF_BMP_REVEAL_R,
	eTTX_USERDEF_BMP_MODE_R,
	eTTX_USERDEF_BMP_LAST
} TTX_UserDefinedBmp_t;

/*******************************************************************/
/********************      Public Functions     ********************/
/*******************************************************************/


Handle_t		TTX_DRCS_Create (void);
void			TTX_DRCS_Destroy(Handle_t hDrcs);
TTX_Result_t	TTX_DRCS_GetPTU (Handle_t hDrcs, const TTX_Cell_t *pCell, HUINT8 *pucPTU);

// TODO: 실제 DRCS 기능 구현... 

#endif	// __TTX_DRCS_H__


/*******************************************************************************
*
* File Name    : xmgr_common.h
*
* Description  : 각종 core application 의 함수 Proto type 및 data type들을 정의한 헤더 파일
*
*
* Copyright (C) 2013, Humax Co., Ltd. All rights reserved.
*
* Revision History :
*
* Date        Modification                                               Name
* ----        ------------                                             --------
* 2013/05/03	createded											dymoon
*
*******************************************************************************/

#ifndef	__XMGR_COMMON_H__
#define	__XMGR_COMMON_H__

#include <octo_common.h>
#include <bus.h>
#include <mgr_common.h>

/*
	Manager 들의 Priority
*/


#define	eMEVT_XMGR_CAS_START				(eMEVT_XMGR_START | 0x00001000)
#define	eMEVT_XMGR_END				(eMEVT_XMGR_START | 0x000FF000)



#if ((eMEVT_XMGR_END) >= (eMEVT_MANAGER_END))
#error "MSG_XMGR_XXX Maxmum Value exceeded"
#endif


#endif // __XMGR_COMMON_H__


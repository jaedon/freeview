/*******************************************************************************
*
* File Name    : xmgr_common.h
*
* Description  : ���� core application �� �Լ� Proto type �� data type���� ������ ��� ����
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
	Manager ���� Priority
*/


#define	eMEVT_XMGR_CAS_START				(eMEVT_XMGR_START | 0x00001000)
#define	eMEVT_XMGR_END				(eMEVT_XMGR_START | 0x000FF000)



#if ((eMEVT_XMGR_END) >= (eMEVT_MANAGER_END))
#error "MSG_XMGR_XXX Maxmum Value exceeded"
#endif


#endif // __XMGR_COMMON_H__


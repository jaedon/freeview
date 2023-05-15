#ifndef _OCTO_CONFIG_H_
#define _OCTO_CONFIG_H_

/*******************************************************************
	File Description
********************************************************************/
/**
	@file	  octo_config.h
	@Remard
		Octo �������� configuration�� �����ϴ� ��� ���Ϸ� octo_common.h�� include�Ǿ� ��ü ���� �����ȴ�.
\n
	Copyright (c) 2008 HUMAX Co., Ltd.				\n
	All rights reserved.							\n
*/
#ifndef AUTOCONF_INCLUDED
#include "autoconf.h"
#endif

/*******************************************************************/
/********************      Macro Definition     ********************/
/*******************************************************************/

/*******************************************************************/
/********************      Typedef         *************************/
/*******************************************************************/

/************* ���� : �Ʒ� NUM_?_ACTION�� ActionId_t ���� ��... **************/
#if defined(CONFIG_PROD_DEVICE_NUM_FULLSCREEN)
#define FULLSCREEN_NUM				CONFIG_PROD_DEVICE_NUM_FULLSCREEN
#else
#define FULLSCREEN_NUM				1
#endif

#if defined(CONFIG_PROD_DEVICE_NUM_PIP)
#define PIP_NUM						CONFIG_PROD_DEVICE_NUM_PIP
#else
#define PIP_NUM						0
#endif

#define NUM_DISPLAY					(FULLSCREEN_NUM + PIP_NUM)


#if defined(CONFIG_SUPPORT_FCC)
#define	NUM_VIEW_ACTION			4
#else
#define	NUM_VIEW_ACTION			2
#endif

#define	NUM_BG_ACTION			8
#if defined(CONFIG_MW_MM_PVR)
#define	NUM_REC_ACTION			CONFIG_MW_MM_MAX_REC_NUM
#define	NUM_TSRREC_ACTION		CONFIG_MW_MM_MAX_TSRREC_NUM
#else
#define	NUM_REC_ACTION			0	
#define	NUM_TSRREC_ACTION		0	
#endif

#define	NUM_SEARCH_ACTION		NUM_VIEW_ACTION

#define	NUM_HLS_ACTION			0

#define NUM_MAX_REC_ACTION		(NUM_REC_ACTION + NUM_TSRREC_ACTION + NUM_HLS_ACTION)

#define	NUM_MAX_ACTION			((NUM_VIEW_ACTION) + (NUM_MAX_REC_ACTION) + (NUM_BG_ACTION))



/****************************************************************************************
	���� action ID�� ���ҽ��� ���� �����ϸ� ���ÿ� ������ �� �ִ� �������� �з��� ���̴�.
	���� ���ҽ��� ���ڶ�� � action ID�� �켱�� ���� �ó������� ������.
*****************************************************************************************/
typedef enum
{
	eActionId_VIEW_FIRST		= 0,
	eActionId_VIEW_0			= eActionId_VIEW_FIRST,
	eActionId_VIEW_1,

	eActionId_VIEW_LAST			= (eActionId_VIEW_FIRST + NUM_VIEW_ACTION - 1),

	// Record Action //
	eActionId_REC_FIRST			= (eActionId_VIEW_FIRST + NUM_VIEW_ACTION),
		// !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
		// Do not make any enum like eActionId_REC_0, 1, ... !!!!!!
		// !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
	eActionId_REC_LAST			= (eActionId_REC_FIRST + NUM_MAX_REC_ACTION - 1),

	// Background Action //
	eActionId_BG_FIRST			= (eActionId_REC_FIRST + NUM_MAX_REC_ACTION),
		// !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
		// Do not make any enum like eActionId_BG_0, 1, ... !!!!!!
		// !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
	eActionId_BG_LAST			= (eActionId_BG_FIRST + NUM_BG_ACTION - 1),

	eActionId_MAX				= (eActionId_BG_FIRST + NUM_BG_ACTION),
	eActionId_Unknown			= 0xffffffff /* ���� �������� ���� Action Id, �ʱⰪ���� Ȱ��, HUINT32 ulActionId�� ���Ƽ� 0xfffffff�� ��� */
} ActionId_t;

#endif /* _OCTO_CONFIG_H_ */


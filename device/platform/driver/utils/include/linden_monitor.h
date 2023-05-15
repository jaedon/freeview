/********************************************************************************
 * Copyright 2001- by HUMAX Co., Ltd.
 *
 * R&D Center
 * Humax Venture Tower 271-2
 * Seohyeon-Dong, Bundang-Gu, Seongnam-Si,
 * Gyeonggi-Do, Korea(ROK)
 *
 * This source code is confidential, proprietary information of HUMAX and
 * is an unpublished work of authorship protected by the copyright laws of
 * the Korea(ROK). Unauthorized copying or use of this document or the program
 * contained herein, in original or modified form, is a violation of Korea law.
 *-------------------------------------------------------------------------------
 * {PROJECT}
 *-------------------------------------------------------------------------------
 * OC-2000 (LINDEN)
 *-------------------------------------------------------------------------------
 * {FILENAME}
 *-------------------------------------------------------------------------------
 * linden_monitor.h
 *
 * $Id: $
 *-------------------------------------------------------------------------------
 * {DESCRIPTION}
 *-------------------------------------------------------------------------------
 * This file contains interfaces of the linden monitor.
 *-------------------------------------------------------------------------------
 * {INITIAL AUTHOR}
 *-------------------------------------------------------------------------------
 * Jinny Youn, Humax. Contacts at jyyoun@humaxdigital.com
 *-------------------------------------------------------------------------------
 * {ANKNOWLEDGMENTS}
 *-------------------------------------------------------------------------------
 *
 *-------------------------------------------------------------------------------
 * {REVISION HISTORY}
 *-------------------------------------------------------------------------------
 * $Log: $
 *
 ********************************************************************************/


/**
 * linden_monitor.h
*/

/**
 * @defgroup		MONITOR MONITOR : menu_monitor module
 * @author		Jinny Youn
 * @note			MenuMonitor APIs
 * @brief			Define MenuMonitor APIs
 * @file			linden_monitor.h
*/


#ifndef __LINDEN_MONITOR_H__
#define __LINDEN_MONITOR_H__

/********************************************************************************
 * H E A D E R S
 ********************************************************************************/

/********************************************************************************
 * D A T A T Y P E S
 ********************************************************************************/
typedef void (*TMonitorFunction)( void );

typedef enum
{
	eMenuTypeNull,
	eMenuTypeNode,
	eMenuTypeLeaf
} etMenuCategory;


typedef struct _stMenu	stMenu;
typedef union
{
	TMonitorFunction	m_pxMonitorFunction;
	stMenu*				m_pstSubMenu;
} suContents;
struct _stMenu
{
	etMenuCategory			m_nCategory;
	const char*				m_szDesc;
#if 1
	suContents 				m_uContents;
/*
	union
	{
		TMonitorFunction	m_pxMonitorFunction;
		stMenu*				m_pstSubMenu;
	}m_uContents;
*/
#else
	suContents*				m_pContents;
#endif

};

typedef struct
{
	int					m_nInt;
	const char*			m_szName;
} stLMNameTable;

/********************************************************************************
 * I N T E R F A C E S
 ********************************************************************************/

/*
 *#ifdef DEBUG
 */
void lmInit_Monitor
(
	void
);

void lmRegister_Monitor
(
	const char*			szDesc,
	stMenu*		pMenus
);

void lmStart_Monitor
(
	void
);

void lmEnd_Monitor
(
	void
);


const char* lmFind_String
(
	stLMNameTable*		pxTable,
	int					nInt
);

void lmStart_Monitor
(
	void
);

void lmEnd_Monitor
(
	void
);

#if 0
/*
 *#else
 */
#define lmInit_Monitor()		TRUE
#define lmRegister_Monitor		if (0) ((int (*)(const char*,const stMenu*)) 0)
#define lmStart_Monitor()		TRUE
#define lmFind_String			if (0) ((int (*)(stLMNameTable*,int)) 0)
/*
 *#endif
 */
#endif

extern stMenu	gaThreaadMonitorMenu[];
extern stMenu	gaTimerMonitorMenu[];
extern stMenu	gaSemMonitorMenu[];
extern stMenu	gaOsMonitorMenu[];
extern stMenu	gaAcTestkitMenu[];
extern stMenu	gaPodMonitorMenu[];
extern stMenu	gaTraceMonitorMenu[];
extern stMenu	gaModuleTestMonitorMenu[];

#endif	/* ! __LINDEN_MONITOR_H__ */

/******************************* end of file **************************************/


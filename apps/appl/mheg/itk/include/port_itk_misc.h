/**
	@file     port_itk_misc.h
	@brief    ITK Host Misc 모듈 헤더파일.

	Description: Porinting Layer 함수 선언을 포함 함.						\n
	Module: mw/mheg/itk												\n

	Copyright (c) 2008 HUMAX Co., Ltd.									\n
	All rights reserved.													\n
*/
#ifndef _PORT_ITK_MISC_H_
#define _PORT_ITK_MISC_H_

/*******************************************************************/
/********************      Header Files    *************************/
/*******************************************************************/
#include "itk.h"
#include "itk_misc.h"

/*******************************************************************/
/********************      Defines         *************************/
/*******************************************************************/


/*******************************************************************/
/********************      Typedef         *************************/
/*******************************************************************/

/** @brief WatchTV application state enumerates */

/*******************************************************************/
/********************      Global Variables     ********************/
/*******************************************************************/

/*******************************************************************/
/********************      Static Variables     ********************/
/*******************************************************************/

/*******************************************************************/
/********************      Public Functions     ********************/
/*******************************************************************/
extern void PORT_ITK_MISC_CreateObject(redkey_profile_t eItkProfile);
void	PORT_ITK_MISC_NotifySubtitleChangedEvent(void);
void	PORT_ITK_MISC_NotifyAudioDescriptionChangedEvent(void);


extern itk_date_t 	mheg_itk_port_miscGetDate(struct itk_misc_t* thiz);
extern itk_misc_t*	mheg_itk_port_miscGetObject(void);
extern void 		mheg_itk_port_miscClosePincodeForPFG(HBOOL bPFG);


#endif	/* _PORT_ITK_MISC_H_ */





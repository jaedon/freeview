/**
	@file     port_itk_audioclip.h
	@brief    ITK Host audioclip 모듈 헤더파일.

	Description: Porinting Layer 함수 선언을 포함 함.
	Module: mw/mheg/itk												\n

	Copyright (c) 2008 HUMAX Co., Ltd.									\n
	All rights reserved.													\n
*/

#ifndef _PORT_ITK_AUDIOCLIP_H_
#define _PORT_ITK_AUDIOCLIP_H_

#include <hlib.h>

#include "itk_audioclip.h"

/*******************************************************************/
/********************      Defines         *************************/
/*******************************************************************/

/*******************************************************************/
/********************      Typedef         *************************/
/*******************************************************************/
typedef struct ITK_AudioclipHandle_t
{
	itk_audioclip_t vtbl;
	void *content;
	size_t content_size;
	itk_int32_t presentation_count;
	itk_audioclip_callback_t callback;
	void *callback_context;
	//HUINT8 ucAudioId;
	HUINT8 ucAudioEv;
} ITK_AudioclipHandle_t;

/*******************************************************************/
/********************      Global Variables     ********************/
/*******************************************************************/

/*******************************************************************/
/********************      Static Variables     ********************/
/*******************************************************************/

/*******************************************************************/
/********************      Public Functions     ********************/
/*******************************************************************/
extern itk_audioclip_t*	port_itk_audioclip_NewAudioClip(struct itk_display_t *thiz, const void *content, size_t content_size,
												 itk_int32_t presentation_count, itk_audioclip_callback_t callback, void *callback_context);
extern void	*PORT_ITK_AUDIOCLIP_getCurHandle(void);
extern void	PORT_ITK_AUDIOCLIP_setCurHandle(void *pCurAudioClip);
extern void	*PORT_ITK_AUDIOCLIP_getStoppedHandle(void);
extern void	PORT_ITK_AUDIOCLIP_setStoppedHandle(void *pStoppedAudioClip);
extern void	PORT_ITK_AUDIOCLIP_Restore(void);
#endif	/* _PORT_ITK_AUDIOCLIP_H_ */





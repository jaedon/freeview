/*******************************************************************
	File Description
********************************************************************/
/**
	@file	  arabic_support.h
	@brief	  related arabic combination

	Description: File�� ���� ������ �����ϴ�.		\n
	Module: Layer�� ��� ���� �����ϴ�.			 	\n

	Copyright (c) 2008 HUMAX Co., Ltd.				\n
	All rights reserved.							\n
*/

#ifndef	__ARABIC_SUPPORT_H__
#define	__ARABIC_SUPPORT_H__

/*******************************************************************/
/********************      Header Files    *************************/
/*******************************************************************/



/*******************************************************************/
/********************      Global Variables     ********************/
/*******************************************************************/


/*******************************************************************/
/********************      Macro Definition     ********************/
/*******************************************************************/


/*******************************************************************/
/********************      Typedef         *************************/
/*******************************************************************/


/*******************************************************************/
/********************      Public Functions     ********************/
/*******************************************************************/
extern HBOOL 	ARABIC_SUPPORT_CheckUnicodeArabicCharacterInString(HUINT16 *pStr);
extern HxSTR_t *ARABIC_SUPPORT_ConvertLogicalAraToVisualAra(HxSTR_t *pStr);

#endif /* !__ARABIC_SUPPORT_H__ */


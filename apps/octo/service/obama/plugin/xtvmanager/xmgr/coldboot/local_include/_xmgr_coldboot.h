/*******************************************************************
	File Description
********************************************************************/
/**
	@file	  _xmgr_coldboot.h
	@brief	  ColdBoot ��� ��� ����

	Description: ���� API �� ������ ������ ������ ��� ����.
	Module: xmgr / coldboot			 	\n

	Copyright (c) 2013 HUMAX Co., Ltd.				\n
	All rights reserved.							\n
*/

#ifndef __XMGR_COLDBOOT_INT_H__
#define __XMGR_COLDBOOT_INT_H__

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
BUS_Result_t xproc_coldboot_Base(HINT32 message, Handle_t hAct, HINT32 p1, HINT32 p2, HINT32 p3);
HERROR	xmgr_coldboot_Start_Base(void);

#if defined(CONFIG_OP_MIDDLE_EAST)
BUS_Result_t xproc_coldboot_MeSat(HINT32 message, Handle_t hAct, HINT32 p1, HINT32 p2, HINT32 p3);
HERROR	xmgr_coldboot_Start_MeSat(void);
#endif
#if defined(CONFIG_OP_SES)
BUS_Result_t xproc_coldboot_Ses(HINT32 message, Handle_t hAct, HINT32 p1, HINT32 p2, HINT32 p3);
BUS_Result_t xproc_coldboot_SesBoot(HINT32 message, Handle_t hAction, HINT32 p1, HINT32 p2, HINT32 p3);
HERROR	xmgr_coldboot_Start_Ses(void);
#endif
#endif /* __XMGR_COLDBOOT_INT_H__ */


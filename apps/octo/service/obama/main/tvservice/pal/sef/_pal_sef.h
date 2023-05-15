/*******************************************************************
	File Description
********************************************************************/
/**
	@file	  _pal_sef.h
	@brief	  pal sef local header

	Description: internal header in pal_sef module \n
	Module: PAL/Sef \n
	Remarks : octo base								\n

	Copyright (c) 2008 HUMAX Co., Ltd.				\n
	All rights reserved.							\n
*/
#ifndef	__PAL_SEF_INT_H__
#define	__PAL_SEF_INT_H__

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

/**
  @remark   pal_sef 전용 section free 함수. SI와 구분하는 게 맞다. debugging을 위해서도 사용함
  @remark   type : Inline
*/
extern void 		pal_sef_FreeDemuxSection(HUINT8 *pucSec);

/**
  @remark   pal_sef 전용 PalSef_RawData_t free 함수.
  @remark   type : Inline
*/
extern void 		pal_sef_FreePackageData(PalSef_RawData_t *pstRawPackage);

#if defined(CONFIG_DEBUG)
void 				pal_sef_PrintContext(HINT32 nPrintLevel);
#endif


/**
  @brief
  Function Name        : pal_sef_SiCreateFilterThread
  Function Description : 부팅 시 초기화 할 때 한 번 불림,
  Parameter            : nothing
  type                 : inline
  Return Value         : HERROR
*/
extern HERROR		pal_sef_SiCreateFilterThread(void);
extern HUINT32		pal_sef_SiRequestFilterStopByDemux(HUINT8 ucDemuxId);
extern HERROR		pal_sef_GetDemuxNum(HUINT8 *pucNumDev);

#endif /* !__PAL_SEF_INT_H__ */


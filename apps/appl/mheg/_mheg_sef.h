/*******************************************************************
	File Description
********************************************************************/
/**
	@file	  _mheg_sef.h
	@brief	  pal sef local header

	Description: internal header in mheg_sef module \n
	Module: PAL/Sef \n
	Remarks : octo base								\n

	Copyright (c) 2008 HUMAX Co., Ltd.				\n
	All rights reserved.							\n
*/
#ifndef	__MHEG_SEF_INT_H__
#define	__MHEG_SEF_INT_H__

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
  @remark   mheg_sef ���� section free �Լ�. SI�� �����ϴ� �� �´�. debugging�� ���ؼ��� �����
  @remark   type : Inline
*/
extern void 		mheg_sef_FreeDemuxSection(HUINT8 *pucSec);

/**
  @remark   mheg_sef ���� PalSef_RawData_t free �Լ�.
  @remark   type : Inline
*/
extern void 		mheg_sef_FreePackageData(MhegSef_RawData_t *pstRawPackage);

#if defined(CONFIG_DEBUG)
void 				mheg_sef_PrintContext(void);
#endif


/**
  @brief
  Function Name        : mheg_sef_SiCreateFilterThread
  Function Description : ���� �� �ʱ�ȭ �� �� �� �� �Ҹ�,
  Parameter            : nothing
  type                 : inline
  Return Value         : HERROR
*/
extern HERROR		mheg_sef_SiCreateFilterThread(void);
extern HUINT32		mheg_sef_SiRequestFilterStopByDemux(HUINT8 ucDemuxId);
extern HERROR		mheg_sef_GetDemuxNum(HUINT8 *pucNumDev);

#endif /* !__MHEG_SEF_INT_H__ */


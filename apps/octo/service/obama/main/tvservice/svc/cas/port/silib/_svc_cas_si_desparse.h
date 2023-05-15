/*******************************************************************
	File Description
********************************************************************/
/**
	@file	  layer_module.h File Name을 적습니다.
	@brief	  file_name & simple comment.

	Description: File에 대한 설명을 적습니다.		\n
	Module: Layer와 모듈 명을 적습니다.			 	\n
	Remarks : 										\n

	Copyright (c) 2008 HUMAX Co., Ltd.				\n
	All rights reserved.							\n
*/
#ifndef __DESPARSE_H__
#define __DESPARSE_H__

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

/* Public Function 마다 필히 기재하도록 함 */
/**
  @brief     LAYER_MODULE_DoSomething 함수.

  Function Name:
  Function Description:
  Parameter:
  type: (Sync or Async)
  Return Value:
*/
HERROR svc_cas_SiParseDes(SIxTextEncoding_e eTextEnc, HUINT8 *p, HUINT8 *pDes);
HERROR parseDummyDes(HUINT8 ucDesTag, HUINT8 *pDes);

HERROR	parseUntDes (HUINT8 *p, HUINT8 *pDes);
HERROR	parseExtDes (HUINT8 *p, HUINT8 ucExtDesTag, HUINT8 ucExtDesLen, HUINT8 *pDes);

#endif	/* __DESPARSE_H__ */

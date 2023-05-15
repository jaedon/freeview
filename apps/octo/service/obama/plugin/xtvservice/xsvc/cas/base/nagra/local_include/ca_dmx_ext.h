/*
@file				ca_dmx_ext.h
@brief				ca_dmx_ext.h (Nagra DMX Service)

Description:  \n
				\n
Module: MW/CAS \n
Remarks :\n

Copyright (c) 2008 HUMAX Co., Ltd. \n
All rights reserved. \n
*/

#ifndef CA_DMX_EXT_H
#define CA_DMX_EXT_H

// 디바이스 드라이버에 섹션 Event 요구 방법
#define SI_EVENT_ONETIME_MODE		0
#define SI_EVENT_CONT_MODE			1

#define CONFIG_ENABLE_FILTER_CONTINUE_MODE		1

/********************************************************************
  Extern Function declaration.
 ********************************************************************/
 // for dmx api
extern void 	CA_NA_DmxCallbackTable(TUnsignedInt32 ulTableIdAndResult, TUnsignedInt32 ulFilterAndUniqId, TUnsignedInt32 ulParam, TUnsignedInt32 hAction);
extern HERROR 	CA_NA_DmxSetEmmProcess(TUnsignedInt32 ulTsId, TBoolean bEmmStop);

// for dmx extension api
extern HERROR 	CA_NA_DmxStartFilter(TUnsignedInt32 hAction, TUnsignedInt16 usPid, TUnsignedInt32 ulTimeOut, TUnsignedInt8 ucEvtMode, TUnsignedInt8 ucFilterValue[], TUnsignedInt8 ucMaskValue[], TUnsignedInt16 *pusFilterId);
extern HERROR 	CA_NA_DmxStopFilter(TUnsignedInt16 usFilterId);

#endif /* CA_DMX_EXT_H */


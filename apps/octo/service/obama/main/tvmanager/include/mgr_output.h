/**
	@file     mgr_output.h
	@brief    file_name & simple comment.

	Description: \n
	Module:  \n
	Remarks : \n
	 Date			Modification				Name\n
	-----------	-------------------------------------	------------------\n
	 2013/03/27		initiated					chmin\n

	Copyright (c) 2013 HUMAX Co., Ltd.				\n
	All rights reserved.							\n
*/

#ifndef	__MGR_OUTPUT_H__
#define	__MGR_OUTPUT_H__

/*******************************************************************/
/********************      Header Files    *************************/
/*******************************************************************/


#include <mgr_common.h>
#include <mgr_copyrightcontrol.h>
/*******************************************************************/
/********************      Macro Definition     ********************/
/*******************************************************************/

/*******************************************************************/
/********************      Typedef         *************************/
/*******************************************************************/

enum
{
	eMEVT_OUTPUT			= eMEVT_OUTPUT_START,

	/* - Description : Action application이 AV 시청 상태로 진입하였음. Subtitle, teletext, data 방송등이 이 이벤트를 기준으로 start한다.
	   - Parameters Usage :
		   handle : Action handle
		   p1 : TBD
		   p2 : TBD
		   p3 : TBD */
	eMEVT_OUTPUT_AV_STARTED,

	/* - Description : Action application이 AV 시청 상태에서 벗어남. Subtitle, teletext, data 방송등이 이 이벤트를 기준으로 stop한다.
	   - Parameters Usage :
		   handle : Action handle
		   p1 : TBD
		   p2 : TBD
		   p3 : TBD */
	eMEVT_OUTPUT_AV_STOPPED,
};

/*
	A/V Mute 관련 Flags
*/
typedef enum
{
	eAvMuteFlag_None			= 0x00000000,

	eAvMuteFlag_View			= 0x00000001,
	eAvMuteFlag_UserLock		= 0x00000002,
	eAvMuteFlag_PcLock			= 0x00000004,
	eAvMuteFlag_Cas				= 0x00000008,
	eAvMuteFlag_Drm				= 0x00000010,
	eAvMuteFlag_Appkit			= 0x00000020,
	eAvMuteFlag_Fcc				= 0x00000040,
	eAvMuteFlag_PlaybackSpeed	= 0x00000080,

	eAvMuteFlag_All				= 0xFFFFFFFF
} MgrOutput_AvMuteFlag_e;





/*******************************************************************/
/********************      Public Functions     ********************/
/*******************************************************************/

HERROR MGR_OUTPUT_Init(void);

HERROR MGR_OUTPUT_SetCopyProtect(HUINT32 ulActionId, MgrCopyrightControl_OutputControlInfo_t stCpInfo);
HERROR MGR_OUTPUT_ResetCopyProtect(HUINT32 ulActionId);
HBOOL MGR_OUTPUT_SupportTvScart(void);
HBOOL MGR_OUTPUT_SupportVcrScart(void);
HBOOL MGR_OUTPUT_SupportSpdif(void);
HBOOL MGR_OUTPUT_SupportCec(void);
DxScartFormat_b MGR_OUTPUT_GetSupportTvScartFormat(void);
DxScartFormat_b MGR_OUTPUT_GetSupportVcrScartFormat(void);

#endif /* __MGR_OUTPUT_H__ */


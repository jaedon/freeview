/**
	@file     xmgr_dynamicupdate_base.c
	@brief    file_name & simple comment.

	Description: \n
	Module: \n
	Remarks : \n
	 Date			Modification				Name\n
	-----------	-------------------------------------	------------------\n


	Copyright (c) 2009 HUMAX Co., Ltd.				\n
	All rights reserved.							\n
*/

/*******************************************************************/
/********************      Header Files    *************************/
/*******************************************************************/
#include <octo_common.h>

#include <svc_pipe.h>

#include <bus.h>


#include <mgr_action.h>
#include <mgr_live.h>
#include <mgr_common.h>

#include <svc_si.h>


#include "../local_include/_xmgr_dynamicupdate.h"
#include <_mgr_dynamicupdate.h>

/*******************************************************************/
/********************      Defines         *************************/
/*******************************************************************/

/*******************************************************************/
/********************      Typedef         *************************/
/*******************************************************************/
typedef struct tagDynamicUpdateContent_Base
{
	DynamicUpdate_Content_t		stBaseContents;

	//1 향별 추가되는 filed는 여기에. 밑으로.
}DynamicUpdate_Content_Base_t;


/*******************************************************************/
/********************      Global Variables     ********************/
/*******************************************************************/


/*******************************************************************/
/********************      Static Variables     ********************/
/*******************************************************************/



/*******************************************************************/
/******************* Static Function Prototypes  *************************/
/*******************************************************************/
STATIC BUS_Result_t	proc_dynamicupdate_Base(HINT32 lMessage, Handle_t hAction, HINT32 lParam1, HINT32 lParam2, HINT32 lParam3);


#define	________________Static_Function_________________________

/*******************************************************************/
/********************      	Static Function 	********************/
/*******************************************************************/




STATIC BUS_Result_t	proc_dynamicupdate_Base(HINT32 lMessage, Handle_t hAction, HINT32 lParam1, HINT32 lParam2, HINT32 lParam3)
{
//	DynamicUpdate_Content_Base_t		*pstContents = NULL;

	switch(lMessage)
	{
		case eMEVT_BUS_CREATE :
			HxLOG_Print("eMEVT_BUS_CREATE\n");
			break;

		case eMEVT_BUS_DESTROY :
			HxLOG_Print("eMEVT_BUS_DESTROY !!\n");
			break;

		//1 공통 msg에 대한 처리는 xmgr_dynamicupdate.c 에 구현해서 모든 향에서 같이 사용.
		// action releated.
		case eMEVT_ACTION_SETUP_REQUESTED:
			HxLOG_Print("eMEVT_ACTION_SETUP_REQUESTED !!\n");
			break;

		case eMEVT_ACTION_SETUP_SUCCEEDED:
			HxLOG_Print("eMEVT_ACTION_SETUP_SUCCEEDED !!\n");
			break;

		// channel event.
		case eSEVT_CH_LOCKED :
			HxLOG_Print("eSEVT_CH_LOCKED !!\n");
			break;


		//2 향별 service compose에 대한 처리는 xmgr_dynamicupdate_xxx.c 에서 구현해서 각자 사용.
		// si dynamic update event.
		case eSEVT_SI_DYNAMIC_UPDATE_DONE:
			HxLOG_Info("\t[eSEVT_SI_DYNAMIC_UPDATE_DONE]\n\n");
			break;

		case eSEVT_SI_DYNAMIC_UPDATE_FAIL:
			HxLOG_Info("\t[eSEVT_SI_DYNAMIC_UPDATE_FAIL]\n\n");
			break;


	}

	return BUS_ProcessMessageDefault(lMessage, hAction, lParam1, lParam2, lParam3);
}


#define	________________Global_Function_________________________

/*******************************************************************/
/********************      	Global Function 	********************/
/*******************************************************************/


HERROR	xmgr_dynamicupdate_Start_Base(void)
{
	BUS_Result_t		eRes = ERR_BUS_SOURCE_NULL;

	eRes = BUS_MGR_Create((char*)"proc_dynamicupdate_Base", APP_ACTION_PRIORITY, proc_dynamicupdate_Base, HANDLE_NULL, 0, 0, 0);

	return (eRes != ERR_BUS_NO_ERROR) ? ERR_FAIL : ERR_OK;
}

HERROR	xmgr_dynamicupdate_Stop_Base(void)
{
	BUS_Result_t		eRes = ERR_BUS_SOURCE_NULL;

	eRes = BUS_MGR_DestroyWithReason(proc_dynamicupdate_Base, eDESTROY_BUS_MYSELF, 0);

	return (eRes != ERR_BUS_NO_ERROR) ? ERR_FAIL : ERR_OK;
}


/*********************** End of File ******************************/


/*******************************************************************/
/********************      Header Files    *************************/
/*******************************************************************/

#include <octo_common.h>

#include <svc_pb.h>

#include <mgr_common.h>
#include <mgr_output.h>
#include <mgr_playback.h>

#include <_xmgr_playback.h>

#include <mgr_copyrightcontrol.h>
/*******************************************************************/
/********************      Typedef         *************************/
/*******************************************************************/

/*******************************************************************/
/********************      Global Variables     ********************/
/*******************************************************************/


/*******************************************************************/
/********************      Static Variables     ********************/
/*******************************************************************/

/*******************************************************************/
/********************      Functions       *************************/
/*******************************************************************/


HUINT32 xmgr_playback_GetContextIndexFromActionId (HUINT32 ulActionId)
{
	if ((ulActionId >= eActionId_VIEW_FIRST) && (ulActionId <= eActionId_VIEW_LAST))
	{
		return (ulActionId - eActionId_VIEW_FIRST);
	}
	else if ((ulActionId >= eActionId_BG_FIRST) && (ulActionId <= eActionId_BG_LAST))
	{
		return (ulActionId - eActionId_BG_FIRST) + NUM_VIEW_ACTION;
	}

	return (HUINT32)-1;
}

void xmgr_playback_SetCopyProtection(xmgrPbContext_t *pstContext)
{
	MgrCopyrightControl_OutputControlInfo_t	stCpInfo;
	HERROR					hError;

	if(pstContext->ePbState == eXmgrPbState_AV)
	{
 		pstContext->stDrmInfo.bCasCc = pstContext->bDrmValid;

		hError = MGR_COPYRIGHTCONTROL_GetCopyProtectInfo(TRUE, (const DxCopyrightControl_t *)&pstContext->stDrmInfo, &stCpInfo);
		if (hError != ERR_OK)
		{
			HxLOG_Critical("Critical Eror!!!\n");
			return;
		}

		MGR_OUTPUT_SetCopyProtect(pstContext->ulActionId, stCpInfo);
	}
	else
	{
		/* 다른 state에서는 no action */
	}
}



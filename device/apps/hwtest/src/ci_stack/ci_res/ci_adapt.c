/***************************************************************
* $Workfile:   ci_adapt.c  $
* $Modtime:   13 Dec 2004 09:27:30  $
* Auther : Jaehee Cho
*
* Desc :
*
* Copyright (c) 2002 by Humax Co., Ltd.
* All right reserved
****************************************************************/

/***************************************************************
* #ifdef statements and include
***************************************************************/
#include "cb_list.h"

#include <di_ci.h>
#include "ci.h"
#include "pal_ci.h"
/***************************************************************
* definition
***************************************************************/


/***************************************************************
* typedef
***************************************************************/


/***************************************************************
* external variables and structures
***************************************************************/
extern CALLBACK_HANDLE				g_hCbNotifyCamState;


/***************************************************************
* global variables and structures
***************************************************************/


/***************************************************************
* static variables and structures
***************************************************************/


/***************************************************************
* static function prototypes
***************************************************************/
static void CiAdapt_NotifyCamState(CI_SLOT_ID usSlotId, unsigned short usState /* DI_CI_STATE : PAL_CI_STATE */);


/***************************************************************
* function bodies
***************************************************************/

/********************************************************************
 Function   :	CiAdapt_Init
 Description :	Initialize CI Adaptation Layer
 Input      :
 Output     :
 Return     : 	0 for OK, -1 for Failure
 ********************************************************************/
int CiAdapt_Init(void)
{
	PAL_CI_RegisterCallback(CiAdapt_NotifyCamState);
	return ERR_OK;
}

#ifdef CI_WORKAROUND_FOR_DOUGLAS
static unsigned long s_ulDouglasWorkAroundTimerId = 0;
static unsigned long s_ulDouglasWorkAroundResetCnt = 0;
static void CiAdapt_DouglasWorkAroundTimerCallback(unsigned long ulTimerId, void *pvParam)
{
	CI_SLOT_ID usSlotId = *((CI_SLOT_ID *)pvParam);

	VK_Print("[CI_RW_ERROR] CiAdapt_DouglasWorkAroundTimerCallback called (%ld) !!!\n", s_ulDouglasWorkAroundResetCnt);

	// todo : check slot id is valid or not...

	s_ulDouglasWorkAroundTimerId = 0;

	if (PAL_CI_ResetModule(usSlotId) != ERR_OK)
	{
		PrintErr(("Error : fail to reset slot %d !!!\n", usSlotId));
	}

	s_ulDouglasWorkAroundResetCnt++;

	return;
}
void CiAdapt_DouglasWorkAroundTimerCallbackCancel(void)
{
	if (s_ulDouglasWorkAroundTimerId)
	{
		VK_TIMER_Cancel(s_ulDouglasWorkAroundTimerId);
		s_ulDouglasWorkAroundTimerId = 0;
	}
}
#endif

/********************************************************************
 Function   :	CiAdapt_NotifyCamState
 Description :
 Input      :
 Output     :
 Return     : 	0 for OK, -1 for Failure
 ********************************************************************/
static void CiAdapt_NotifyCamState(CI_SLOT_ID usSlotId, unsigned short usState /* DI_CI_STATE : PAL_CI_STATE */)
{
	CI_CAM_STATE	nCamState;
	CI_NOTIFY_CAM_STATE_FUNC	pfnNotifyCamState;

	/* DI로부터는 DI_CI_EXTRACTED, DI_CI_INSERTED 밖에 notify되지 않도록 한다 */

	switch (usState)
	{
		case DI_CI_EXTRACTED:

#ifdef CI_WORKAROUND_FOR_DOUGLAS
			CiAdapt_DouglasWorkAroundTimerCallbackCancel(); // CiAdapt_DouglasWorkAroundTimerCallback() 타이머 기동 중이면 캔슬한다.
#endif

			nCamState = CI_CAM_EXTRACTED;
			break;

		case DI_CI_INSERTED:

#ifdef CI_WORKAROUND_FOR_DOUGLAS
			CiAdapt_DouglasWorkAroundTimerCallbackCancel(); // CiAdapt_DouglasWorkAroundTimerCallback() 타이머 기동 중이면 캔슬한다.
#endif

			nCamState = CI_CAM_INSERTED;
			break;

		case DI_CI_ENABLED:

#ifdef CI_WORKAROUND_FOR_DOUGLAS
			if(s_ulDouglasWorkAroundTimerId == 0 && s_ulDouglasWorkAroundResetCnt < CI_WORKAROUND_FOR_DOUGLAS_LIMIT)
			{
				VK_TIMER_EventAfter(CI_WORKAROUND_FOR_DOUGLAS_TIMEOUT, CiAdapt_DouglasWorkAroundTimerCallback, (void *)&usSlotId, sizeof(CI_SLOT_ID), (unsigned long *)&s_ulDouglasWorkAroundTimerId);
			}
#endif

			PrintErr(("[CI] DI_CI_ENABLED !!\n")); // 에러 아니나 주목할 만한 로그라서 넣어둠.
			return;

		default:

			PrintErr(("[CI] unknown state !!\n"));
			return;
	}

	/* get notify function list and notify CAM state */
	Callback_Reset(g_hCbNotifyCamState);
    while ((pfnNotifyCamState = (CI_NOTIFY_CAM_STATE_FUNC)Callback_GetNextFunc(g_hCbNotifyCamState)) != NULL)
    {
    	(*pfnNotifyCamState)(usSlotId, nCamState);
    }
}



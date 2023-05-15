/****************************************************************************
*
* File Name   : ap_coldboot_none.c
*
* Description : AP cold boot processing application. No cold boot operation.
*
*
* Revision History	:
*
* Date			Modification				Name
* -----------	-------------------------------------	------------------
*
****************************************************************************/


/* ======================================================================== */
/* Include. 																*/
/* ======================================================================== */
/** @brief global header file  */
#include <octo_common.h>

#include <svc_output.h>
#include <svc_sys.h>

#include <bus.h>

#include <mgr_common.h>
#include <mgr_time.h>
#include <mgr_coldboot.h>

/* ======================================================================== */
/* Private Macros. 															*/
/* ======================================================================== */




/* ======================================================================== */
/* Global/Extern Variables.													*/
/* ======================================================================== */

/* ======================================================================== */
/* Private Types.															*/
/* ======================================================================== */

/* ======================================================================== */
/* Private Constants. 														*/
/* ======================================================================== */

/* ======================================================================== */
/* Private Variables. 														*/
/* ======================================================================== */
static HINT32	s_nPamaKeepMessage = 0;

/* ======================================================================== */
/* Private Function prototypes. 											*/
/* ======================================================================== */

/* ======================================================================== */
/* Functions. 																*/
/* ======================================================================== */

BUS_Result_t xproc_coldboot_Base(HINT32 message, Handle_t hAct, HINT32 p1, HINT32 p2, HINT32 p3)
{
	switch (message)
	{
		case eMEVT_BUS_CREATE:
			{
#if defined(CONFIG_MW_CH_SATELLITE)
				HUINT32 ulUnixTime;
#endif
				//HxLOG_Print("\n\n\t############ Cold Boot Processing Start !!! ##############\n");
				// AV Enable
				//CHECK_BOOT_TIME("Create Done - XPROC_ColdBoot - Create  - Start");
				BUS_MGR_SetMgrName(BUS_MGR_GetThis(), "XPROC_ColdBoot");

#if defined(CONFIG_MW_CH_SATELLITE)
				{
					HERROR	hErr;

					hErr = VK_CLOCK_GetTime((unsigned long*)&ulUnixTime) ;
					if ( VK_OK != hErr	|| HLIB_DATETIME_IsValidUnixTime(ulUnixTime) != ERR_OK )
					{
						MGR_TIME_ClockRecoverStart();
						break;
					}
				}
#endif

//	PAMA or MainAppl control
//				SVC_OUT_SetAvEnable(TRUE);

#if 0
				// Cold Boot 시 필요한 UI를 실행시킨다.
				// Cold Boot UI를 띄운다.
				BUS_SendMessage (NULL, MSG_APPC2U_CREATE_BOOTUI,
								HANDLE_NULL,
								eBootUi_ColdBoot, 0, 0);
				CHECK_BOOT_TIME("BUS_SendMessage (NULL, MSG_APPC2U_CREATE_BOOTUI, HANDLE_NULL, eBootUi_ColdBoot, 0, 0)");
#endif

				BUS_MGR_Destroy(0);
				BUS_PostMessage(NULL, eMEVT_COLDBOOT_NOTIFY_DONE, 0, 0, 0, 0);
				//CHECK_BOOT_TIME("BUS_PostMessage(NULL, eMEVT_COLDBOOT_NOTIFY_DONE, 0, 0, 0, 0)");
			}
			break;

		case eMEVT_TIME_CLOCKRECOVER_SETUP_DONE:
//	PAMA or MainAppl control
//			SVC_OUT_SetAvEnable(TRUE);

			BUS_MGR_Destroy(0);
			BUS_PostMessage(NULL, eMEVT_COLDBOOT_NOTIFY_DONE, 0, 0, 0, 0);
			//CHECK_BOOT_TIME("BUS_PostMessage(NULL, eMEVT_COLDBOOT_NOTIFY_DONE, 0, 0, 0, 0)");
			break;

		case eMEVT_BUS_DESTROY:
			if (s_nPamaKeepMessage)
			{
				HxLOG_Warning("Posting PAMA Message again...\n");
				BUS_PostMessage(NULL, s_nPamaKeepMessage, (Handle_t)0, 0, 0, 0);

				s_nPamaKeepMessage = 0;
			}
			return MESSAGE_BREAK;

		//	In coldboot mode, all pama message will be here if coldboot proc is existed.
		//	Keep pama message, and posting message when coldboot is finished.
		case eMEVT_BUS_GO_OPERATION:
		case eMEVT_BUS_READY_SHUTDOWN:
			HxLOG_Warning("PAMA Message received in COLDBOOT!\n");
			s_nPamaKeepMessage = message;
			if(message == eMEVT_BUS_READY_SHUTDOWN)
			{
				MGR_ACTION_WaitShutDown(6000);
			}
			return MESSAGE_BREAK;

		default:
			break;
	}

	return BUS_ProcessMessageDefault(message, hAct, p1, p2, p3);
}

HERROR	xmgr_coldboot_Start_Base(void)
{
	BUS_Result_t	eRes;

	eRes = BUS_MGR_Create("XPROC_ColdBoot", APP_ACTION_MGR_PRIORITY, xproc_coldboot_Base, 0, 0, 0, 0);

	return (eRes != ERR_BUS_NO_ERROR) ? ERR_FAIL : ERR_OK;

}

/* End of File. ---------------------------------------------------------- */



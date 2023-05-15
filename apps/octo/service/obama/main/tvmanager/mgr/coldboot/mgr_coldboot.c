/****************************************************************************
*
* File Name   : mgr_coldboot.c
*
* Description : AP module initialization application
*
*
* Revision History	:
*
* Date			Modification							Name
* -----------	-------------------------------------	------------------
* 2013			created									wjmoh
*
****************************************************************************/


/* ======================================================================== */
/* Include. 																*/
/* ======================================================================== */

#include <htype.h>
#include <hlib.h>
#include <octo_common.h>
#include <mgr_coldboot.h>
#include <pmgr_coldboot.h>

/* ======================================================================== */
/* Private Macros. 															*/
/* ======================================================================== */

/* ======================================================================== */
/* Global/Extern Variables.													*/
/* ======================================================================== */

/* ======================================================================== */
/* Private Constants. 														*/
/* ======================================================================== */


/* ======================================================================== */
/* Private Types.															*/
/* ======================================================================== */



/* ======================================================================== */
/* Private Variables. 														*/
/* ======================================================================== */


/* ======================================================================== */
/* Private Function prototypes. 											*/
/* ======================================================================== */


/* ======================================================================== */
/* Functions. 																*/
/* ======================================================================== */
BUS_Callback_t	MGR_ColdBoot_GetProc(void)
{
	return pmgr_ColdBoot_GetProc();
}

HERROR	MGR_ColdBoot_Start(void)
{
	return pmgr_ColdBoot_Start();
}

HERROR	MGR_ColdBoot_Stop(BUS_DestroyReason_t eDestroyReason)
{
	BUS_Result_t	hErr = ERR_BUS_ACTION_FAILED;

	if (BUS_MGR_Get(MGR_ColdBoot_GetProc()) != NULL)
	{
		hErr = BUS_MGR_DestroyWithReason(MGR_ColdBoot_GetProc(), eDestroyReason, 0);
		if (hErr != ERR_OK)
		{
			HxLOG_Error("BUS_MGR_DestroyWithReason fail to destroy MGR_ColdBoot_GetProc.\n");
		}
	}

	return hErr;
}

BUS_Callback_t	MGR_ColdBoot_InitColdBootBatch(void **ppvUserData, HUINT32 *pulUserDataLength, DxBatchType_e eBatchType)
{
	BUS_Callback_t		pfnSrchProc = NULL;
	HERROR				hErr = ERR_FAIL;

	HxLOG_Trace();

	pfnSrchProc = MGR_ColdBoot_GetProc();
	if (pfnSrchProc == NULL)
	{
		HxLOG_Error("no coldboot proc \n");
		return NULL;
	}

	hErr = MGR_ColdBoot_Start();
	if (hErr == ERR_FAIL)
	{
		HxLOG_Error("cannot start coldboot \n");
	}

	return pfnSrchProc;

}

HERROR	MGR_ColdBoot_DeinitColdBootBatch(void *pvUserData, HUINT32 ulUserDataLength, DxBatchType_e eBatchType)
{
	HERROR		hErr = ERR_FAIL;

	hErr = MGR_ColdBoot_Stop(eDESTROY_BUS_MYSELF);
	if (hErr != ERR_OK)
	{
		HxLOG_Error("stop coldboot fail because it's already destroyed.\n");
	}
	return hErr;

}

HERROR	MGR_ColdBoot_RescheduleColdBootBatch(DxBatchType_e eBatchType)
{
	HxLOG_Trace();
	return ERR_OK;
}

HERROR	MGR_ColdBoot_GetBatchColdBootCBList(MgrAction_BatchProcCBList_t * fnBatchProcCBList)
{
	HxLOG_Trace();

	fnBatchProcCBList->fnAppInit = MGR_ColdBoot_InitColdBootBatch;
	fnBatchProcCBList->fnAppDeinit = MGR_ColdBoot_DeinitColdBootBatch;
	fnBatchProcCBList->fnAppReSchedule = MGR_ColdBoot_RescheduleColdBootBatch;

	return ERR_OK;
}

HERROR	MGR_ColdBoot_GetBatchColdBootTime(UNIXTIME *ulStartTime, HUINT32 *ulDuration)
{
	HxLOG_Trace();
	return ERR_OK;
}

/* End of File. ---------------------------------------------------------- */



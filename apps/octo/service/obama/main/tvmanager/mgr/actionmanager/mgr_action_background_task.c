/****************************************************************************
*
* File Name   : app_stockimg_init.c
*
* Description : AP module initialization application
*
*
* Revision History	:
*
* Date			Modification							Name
* -----------	-------------------------------------	------------------
* 2009/01/14	created									wjmoh
*
****************************************************************************/


/* ======================================================================== */
/* Include. 																*/
/* ======================================================================== */

#include <htype.h>
#include <hlib.h>
#include <octo_common.h>

#include <mgr_common.h>
#include <mgr_action.h>


/* ======================================================================== */
/* Private Macros. 															*/
/* ======================================================================== */

/* ======================================================================== */
/* Global/Extern Variables.													*/
/* ======================================================================== */

/* ======================================================================== */
/* Private Constants. 														*/
/* ======================================================================== */




#define	LOG								HxLOG_Print
#define	ERR_LOG							HxLOG_Error


#define BGTASK_QUEUE_SIZE				16


/* ======================================================================== */
/* Private Types.															*/
/* ======================================================================== */

typedef	HERROR (*_mgrBgJobCallback_t) (Handle_t hAction, HUINT32 ulParam1, HUINT32 ulParam2, HUINT32 ulParam3);

typedef enum
{
	eBGJOBMSG_Init = 0,

	eBGJOBMSG_ExecuteJob,

	eBGJOBMSG_END
} _mgrBgJobMsgType_e;

typedef	struct
{
	_mgrBgJobMsgType_e		eMsgClass;
	_mgrBgJobCallback_t		pfnCallBack;
	Handle_t				hAction;			/**< action handle */
	HUINT32					ulParam1;			/**< arg 1 */
	HUINT32					ulParam2;			/**< arg 2 */
	HUINT32					ulParam3;			/**< arg 3 */
} _mgrBgJobMsg_t;


/* ======================================================================== */
/* Private Variables. 														*/
/* ======================================================================== */

STATIC unsigned long		 s_ulBgJob_QueueId;
STATIC unsigned long		 s_ulBgJob_TaskId;


/* ======================================================================== */
/* Private Function prototypes. 											*/
/* ======================================================================== */


/* ======================================================================== */
/* Functions. 																*/
/* ======================================================================== */

STATIC void mgr_BGJOB_Task (void *arg1)
{
	_mgrBgJobMsg_t		 stMsg;
	HERROR				 hErr;

	while (1)
	{
		HxSTD_memset (&stMsg, 0, sizeof(_mgrBgJobMsg_t));

		hErr = VK_MSG_Receive (s_ulBgJob_QueueId, &stMsg, sizeof(_mgrBgJobMsg_t));
		if (ERR_OK == hErr)
		{
			if (stMsg.pfnCallBack != NULL)
			{
				hErr = stMsg.pfnCallBack (stMsg.hAction, stMsg.ulParam1, stMsg.ulParam2, stMsg.ulParam3);
				BUS_PostMessage (NULL, eMEVT_ACTION_NOTIFY_BGJOB_FINISHED, stMsg.hAction, (HINT32)stMsg.pfnCallBack, (HINT32)hErr, 0);
			}
		}
	}
}


#define _____BACKGROUND_JOBS_____

HERROR MGR_ACTION_BgJobInit (void)
{
	HERROR			 hErr;

	hErr = VK_MSG_Create (BGTASK_QUEUE_SIZE, sizeof(_mgrBgJobMsg_t), "bgjob_q", &s_ulBgJob_QueueId, VK_SUSPENDTYPE_FIFO);
	if (VK_OK != hErr)
	{
		ERR_LOG ("[MGR_ACTION_BgJobInit] VK_MSG_Create err:0x%08x\n", hErr);
		return ERR_FAIL;
	}

	hErr = VK_TASK_Create (mgr_BGJOB_Task, BACKGROUND_TASK_PRIORITY, GWM_TASK_STACK_SIZE,
							"tMgrBgJob", NULL, &s_ulBgJob_TaskId, 0);
	if (VK_OK != hErr)
	{
		ERR_LOG ("[MGR_ACTION_BgJobInit] VK_TASK_Create err:0x%08x\n", hErr);
		return ERR_FAIL;
	}

	hErr = VK_TASK_Start (s_ulBgJob_TaskId);
	if (VK_OK != hErr)
	{
		ERR_LOG ("[MGR_ACTION_BgJobInit] VK_TASK_Start err:0x%08x\n", hErr);
		return ERR_FAIL;
	}

	return ERR_OK;
}

HERROR MGR_ACTION_BgJobExecuteFunction (HERROR (*pfnFunc)(Handle_t hAction, HUINT32 ulParam1, HUINT32 ulParam2, HUINT32 ulParam3), Handle_t hAction, HUINT32 ulParam1, HUINT32 ulParam2, HUINT32 ulParam3)
{
	_mgrBgJobMsg_t	 stMsg;
	HERROR			 hErr;

	if (NULL == pfnFunc)
	{
		ERR_LOG ("[MGR_ACTION_BgJobExecuteFunction] Callback functio NULL\n");
		return ERR_FAIL;
	}

	stMsg.eMsgClass		= eBGJOBMSG_ExecuteJob;
	stMsg.pfnCallBack	= pfnFunc;
	stMsg.hAction		= hAction;
	stMsg.ulParam1		= ulParam1;
	stMsg.ulParam2		= ulParam2;
	stMsg.ulParam3		= ulParam3;

	hErr = VK_MSG_Send (s_ulBgJob_QueueId,(void *)&stMsg, sizeof(_mgrBgJobMsg_t));
	if (VK_OK != hErr)
	{
		ERR_LOG ("[MGR_ACTION_BgJobExecuteFunction] VK_MSG_Send err:0x%08x\n", hErr);
		return ERR_FAIL;
	}

	return ERR_OK;
}



/* End of File. ---------------------------------------------------------- */



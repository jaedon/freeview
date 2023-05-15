/**************************************************************
 *	@file		imma.c
 *	????? Media Manager Application for OCTO 2.0
 *
 *	http://www.humaxdigital.com
 *
 *	@data			2032/04/23
 *	@author			humax
 *	@brief			????? Media Manager Application
 **************************************************************/

#include	<stdio.h>
#include	<stdlib.h>
#include	<sys/types.h>
#include	<pwd.h>
#include	<grp.h>

#include	<directfb.h>
#define CONFIG_USE_PAMA_NOTIFICATION
#ifdef CONFIG_USE_PAMA_NOTIFICATION
#include <hapi.h>
#include <papi.h>
#include <vkernel.h>
#include "cso.h"
#endif
#include	"cso_ipc.h"
#include	"cso_def.h"

/**************************************************************
	internal structures and variables
 **************************************************************/
#ifdef CONFIG_USE_PAMA_NOTIFICATION
unsigned long s_ulSystemEventMsgQId;
#endif

/**************************************************************
	global functions
 **************************************************************/
int DI_MI_Init(void);
/**************************************************************
	private functions
 **************************************************************/

#ifdef CONFIG_USE_PAMA_NOTIFICATION
/*-----------------------------------------------------------------------------
 *  HAMA Event
 *-----------------------------------------------------------------------------*/
static HERROR	__imma_system_bus_signal_onSystemStateChanged(HINT32 nArgc, const HxObject_t *apArgv[], HINT32 nRequestId, void *pvUser)
{
	PxSTATUS_e			ePapiStatus;
	HUINT32				nResult;

	HxLOG_Assert(nArgc == 1);

	ePapiStatus = HxOBJECT_INT(apArgv[0]);

	nResult = (HUINT32)VK_MSG_SendTimeout(s_ulSystemEventMsgQId, &ePapiStatus, sizeof(PxSTATUS_e), 500);
	if(nResult != ERR_OK)
	{
		HxLOG_Error("\n");
	}

	return ERR_OK;
}


static	void 	__imma_system_ext_event_listen( void )
{
	/* HAMA Signal Listen */
	HAPI_ConnectSignal("signal:onSystemStateChanged", NULL, __imma_system_bus_signal_onSystemStateChanged);
	/* HAMA Event Listen */
}
#endif

/***********************************************************************************/
/***************************  For Ready Check Task *********************************/

/**************************************************************
	public functions
 **************************************************************/
static void _apk_imma_task(void *arg)
{
	int					ret = 0;
    PxSTATUS_e			ePapiStatusData;
    CSO_Status_t 		tCsoStatus;

	while (TRUE)
	{
		ret = (HUINT32)VK_MSG_Receive(s_ulSystemEventMsgQId, &ePapiStatusData, sizeof(PxSTATUS_e));
		if(ret == ERR_OK)
		{
			switch(ePapiStatusData)
			{
				case ePAMA_STATUS_Reboot:
					HxLOG_Error("[%s:%d] IMMA --------- ePAMA_STATUS_Reboot \n", __FUNCTION__, __LINE__);
					break;

				case ePAMA_STATUS_Shutdown:
					HxLOG_Error("[%s:%d] IMMA --------- ePAMA_STATUS_Shutdown\n", __FUNCTION__, __LINE__);

					VK_MEM_Memset(&tCsoStatus, 0 , sizeof(CSO_Status_t));

					/* Check if Calypso Status is ready to Shutdown... */
					(void)CSO_Status(0, &tCsoStatus);

					if ( tCsoStatus.nOpened && tCsoStatus.nPlaying )
					{
						HxLOG_Error("[%s:%d] IMMA --------- Calypso session is opened and playing....\n", __FUNCTION__, __LINE__ );
						PAPI_WaitForShutDown(3000);
					}
					else
					{
						HxLOG_Error("[%s:%d] IMMA --------- Calypso session is ready to SHUTDOWN.... \n", __FUNCTION__, __LINE__ );
					}
					break;

				case ePAMA_STATUS_Operation:
					HxLOG_Error("[%s:%d] IMMA --------- ePAMA_STATUS_Operation\n", __FUNCTION__, __LINE__);
					break;

				default:
						break;
			}
		}
		else /* this shouldn't be happen... */
		{
			sleep(1);
		}
	}
}

int		imma_main(int argc, char **argv)
{
	unsigned long	ulTaskId;
#if 1
    int ret = 0;

#ifdef CONFIG_USE_PAMA_NOTIFICATION
    VK_Init();
	/* Message Q onSystemStateChanged event callback */
	if(VK_MSG_Create(64, sizeof(PxSTATUS_e), "onSystemStateChangedEventMsgQ", &s_ulSystemEventMsgQId, VK_SUSPENDTYPE_PRIORITY) != ERR_OK)
	{
		HxLOG_Critical(" \n");
		exit(-1);
	}

	__imma_system_ext_event_listen();
#endif

//    (void)VK_Init();
    ret = DI_MI_Init();
	if(ret != 0)
	{
		exit(-1);
	}

	//	Drop Root Previleges --> if CONFIG_DROP_ROOT_PRIVILEGES is ON,
	HLIB_EXTRA_DropRootPrivileges("imma");

    ret = CSO_IPC_Create(48629);
	if(ret != 0)
	{
		exit(-1);
	}
#endif

#ifdef CONFIG_USE_PAMA_NOTIFICATION
	VK_TASK_Create(_apk_imma_task, 40, SIZE_16K, "immaApk", NULL, (unsigned long *)&ulTaskId, 0);
	VK_TASK_Start(ulTaskId);
#endif

	return 0;
}

int		main(int argc, char **argv)
{
	imma_main(argc, argv);

	while (1)
	{
		HLIB_STD_TaskSleep(100000);
	}

    exit(EXIT_SUCCESS);
    return 0;
}


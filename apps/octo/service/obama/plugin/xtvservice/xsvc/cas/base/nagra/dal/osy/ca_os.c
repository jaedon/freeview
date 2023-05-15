/**************************************************************************************************/
#define ________CA_OS_Private_Include_____________________________________________________
/**************************************************************************************************/
#include <stdio.h>
#include <stdarg.h>
#include "vkernel.h"
#include "na_glob.h"

//#include "di_uart.h"
//#include "pal_power.h"
//#include "pal_panel.h"
//#include "pal_sys.h"
#include <_svc_cas_mgr_common.h>

//#define ENABLE_TERMINAL_FOR_RELEASE	/* CAS Library가 직접 Uart를 통해 통신하는 경우 */

#if defined(CONFIG_MW_CAS_NAGRA_CA_PRODUCT_ICME) || defined(ENABLE_TERMINAL_FOR_RELEASE)
#if !defined(CONFIG_DEBUG)

/*
	DLK (cardless) 의 경우 Production SW에도 DLK library가 Uart를 사용할 수 있게 해야 함.
	Nagra에서 생산된 물건을 가지고 debugging을 할 때 Card모델의 경우 Card interface를 통해 하지만
	cardless 모델의 경우 Card interface가 없으므로 Uart를 통해 직접 Library와 연결하여 통신해야 한다.
	실제 나그라에서 자기들 분석 툴을 통해 library와 통신하여 정상 동작하는 지를 확인 하고 있음.
*/
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>
#include <termios.h>
#endif
#endif

/**************************************************************************************************/
#define ________CA_OS_Golbal_Value________________________________________________________
/**************************************************************************************************/


/**************************************************************************************************/
#define ________CA_OS_Private_Define_______________________________________________________
/**************************************************************************************************/
#if defined(CONFIG_MW_CAS_NAGRA_DALTEST)
#define OsyLogTrace(message) 	//(HLIB_CMD_Printf(C_CODE_F_PURPLE"[CA OSY](%s:%d)"C_CODE_RESET, __FUNCTION__, __LINE__), (HLIB_CMD_Printf message))
#define OsyLogError(message) 	//(HLIB_CMD_Printf(C_CODE_F_RED"[CA OSY](%s:%d)"C_CODE_RESET, __FUNCTION__, __LINE__), (HLIB_CMD_Printf message))
#define OsyLogWarning(message) 	//(HLIB_CMD_Printf(C_CODE_F_RED_B_BLACK"[CA OSY](%s:%d)"C_CODE_RESET, __FUNCTION__, __LINE__), (HLIB_CMD_Printf message))
#define OsyLogInfo(message)		//((HLIB_CMD_Printf message))


#if defined (HxLOG_Print)
#undef	HxLOG_Print
#define HxLOG_Print(...)	OsyLogTrace((__VA_ARGS__))
#endif

#if defined (HxLOG_Error)
#undef	HxLOG_Error
#define HxLOG_Error(...)	OsyLogError((__VA_ARGS__))
#endif

#if defined (HxLOG_Warning)
#undef	HxLOG_Warning
#define HxLOG_Warning(...)	OsyLogWarning((__VA_ARGS__))
#endif

#if defined (HxLOG_Info)
#undef	HxLOG_Info
#define HxLOG_Info(...)		OsyLogInfo((__VA_ARGS__))
#endif

#endif

//#define _CA_OS_CAPTURE_OS_PRINT_

#define CONFIG_NA_REMOVE_CSCD						(0)
#define CONFIG_NA_PERSISTENT_MEM					(1)

/*priority*/
#if !defined(CONFIG_PROD_PROFILE_MINIOCTO)
#define OS_TASK_PRIORITY_NORMAL					(VK_TASK_PRIORITY_MW_REF+2)
#define OS_TASK_PRIORITY_VERY_LOW 				(OS_TASK_PRIORITY_NORMAL-1)
#define OS_TASK_PRIORITY_LOW						(OS_TASK_PRIORITY_NORMAL-2)
#define OS_TASK_PRIORITY_HIGH 						(OS_TASK_PRIORITY_NORMAL+1)
#define OS_TASK_PRIORITY_VERY_HIGH				(OS_TASK_PRIORITY_NORMAL+2)
#else
#define OS_TASK_PRIORITY_NORMAL					(VK_TASK_PRIORITY_MW_REF)
#define OS_TASK_PRIORITY_VERY_LOW 				(OS_TASK_PRIORITY_NORMAL-2)
#define OS_TASK_PRIORITY_LOW						(OS_TASK_PRIORITY_NORMAL-1)
#define OS_TASK_PRIORITY_HIGH 						(OS_TASK_PRIORITY_NORMAL+1)
#define OS_TASK_PRIORITY_VERY_HIGH				(OS_TASK_PRIORITY_NORMAL+2)
#endif
/* etc */
#define OS_PERSISTENT_MEMORY_UNITS				(3)
#if (CONFIG_PROTECT_CW == 1)
#if defined(CONFIG_MW_CAS_NAGRA_PRM)
#define OS_PERSISTENT_MEMORY_NUMS				(OS_PERSISTENT_MEMORY_UNITS + 3)
#else
#define OS_PERSISTENT_MEMORY_NUMS				(OS_PERSISTENT_MEMORY_UNITS + 2)
#endif
#else
#if defined(CONFIG_MW_CAS_NAGRA_PRM)
#define OS_PERSISTENT_MEMORY_NUMS				(OS_PERSISTENT_MEMORY_UNITS + 1)
#else
#define OS_PERSISTENT_MEMORY_NUMS				(OS_PERSISTENT_MEMORY_UNITS)
#endif
#endif
#define OS_PERSISTENT_SECURITY_EEPROM_SIZE		(512)
#define OS_PERSISTENT_SECURITY_FLASH_SIZE		(16*1024)
#define OS_PERSISTENT_STANDARD_FLASH_SIZE		(32*1024)
#if (CONFIG_PROTECT_CW == 1)
#if defined(CONFIG_MW_CAS_NAGRA_DALAPI_7_1_ABOVE)
#if defined(CONFIG_PROD_HGS1000S)
#define OS_PERSISTENT_CSC_FLASH_ADDRESS				(0x140000)
#else
#define OS_PERSISTENT_CSC_FLASH_ADDRESS				(0x1C0000)
#endif
#else
#define OS_PERSISTENT_CSC_FLASH_ADDRESS				(0x2C0000)
#endif
#if defined (CONFIG_MW_CAS_NAGRA_NOCS_3_X)
// a CSC data record shall be probrammed in NVM during the STB personallization. 
// The maximux size of this record depends on the type of chipset integrated and is defined as folows:
// NOCS1 : 256 byte
// NOCS3 : 3000 byte
#define OS_PERSISTENT_CSC_FLASH_SIZE				(3000)
#else
#define OS_PERSISTENT_CSC_FLASH_SIZE				(256)
#endif
#endif
#if defined(CONFIG_MW_CAS_NAGRA_PRM)
#define OS_PERSISTENT_DVL_FLASH_SIZE				(4*1024)
#endif

#define WAIT_FOR_SEM(x)							VK_SEM_Get(x);
#define RELEASE_SEM(x)								VK_SEM_Release(x);
#define Milli2Tick(x)									(x)

#define IPC_BUFFER_MAX							(1100)


/**************************************************************************************************/
#define ________CA_OS_Private_Type_Define_________________________________________________
/**************************************************************************************************/
typedef void (*CaOsThreadFunc)(void* param);

typedef enum
{
	eTask_Dormant,
	eTask_Running,
	eTask_Susupended,
	eTask_State_Max
} TaskState_t;

typedef enum
{
	eTimer_Dormant,
	eTimer_Running,
	eTimer_Stopped,
	eTimer_State_Max
} TimerState_t;

#if (CONFIG_NA_PERSISTENT_MEM == 1)
typedef enum
{
	eNa_Standard_Block = 0,
	eNa_Security_Block,
	eNa_Block_Max
} FlashNagraBlock_t;
#endif

/* task */
struct SOsTaskId
{
	TUnsignedInt32	ulId;
	TOsEntryPoint       	pfnEntryPoint;
	TOsEvent            	usAllocatedEvents;
	TaskState_t		eTaskState;
	Event_Handle_t	hEvent;
	TUnsignedInt16	usCurrentEvent;
};

typedef struct SPsosTaskList
{
	struct SOsTaskId		*pstTaskId;
	struct SPsosTaskList	*pNext;
} TPsosTaskList;

/* semaphore */
struct SOsSemaphoreId
{
	TUnsignedInt32	ulId;
};

typedef struct SPsosSemaphoreList
{
	struct SOsSemaphoreId	*pstSemaphoreId;
	struct SPsosSemaphoreList *pNext;
} TPsosSemaphoreList;

/* queue */
struct SOsQueueId
{
	TUnsignedInt32	ulId;
	TOsCount		usCount;
	TUnsignedInt32	ulMsg[4];
};

typedef struct SPsosQueueList
{
	struct SOsQueueId	*pstQueueId;
	struct SPsosQueueList	*pNext;
} TPsosQueueList;

/* timer */
struct SOsTimerId
{
	TUnsignedInt32	ulId;
	TUnsignedInt32	ulTimerTaskId;
	TOsTaskId		stCreatorTaskId;
	TOsEvent			usEventToSend;
	TOsTimerMode	eTimerMode;
	TimerState_t		eTimerState;
	volatile TOsTime	ulTimeout;
};

typedef struct SPsosTimerList
{
	struct SOsTimerId 	*pstTimerId;
	struct SPsosTimerList	*pNext;
} TPsosTimerList;


/**************************************************************************************************/
#define ________CA_OS_Private_Static_Value_________________________________________________
/**************************************************************************************************/
static volatile TUnsignedInt32		s_ulTaskObjSemId = 0;
static volatile TUnsignedInt32		s_ulSemaphoreObjSemId = 0;
static volatile TUnsignedInt32		s_ulQueueObjSemId = 0;
static volatile TUnsignedInt32		s_ulTimerObjSemId = 0;

static TPsosTaskList				*s_pstTaskList;
static TPsosSemaphoreList			*s_pstSemaphoreList;
static TPsosQueueList				*s_pstQueueList;
static TPsosTimerList				*s_pstTimerList;

static TUnsignedInt32				s_ulRefSysTime;

#if (CONFIG_NA_PERSISTENT_MEM == 1)
static TOsPersistentMemoryStorage	s_stStorageTable[OS_PERSISTENT_MEMORY_NUMS];
#endif

#if defined(CONFIG_MW_CAS_NAGRA_CA_PRODUCT_ICME) || defined(ENABLE_TERMINAL_FOR_RELEASE)
#if !defined(CONFIG_DEBUG)
static HINT32	s_ulTtyFd = 0;
#endif
#endif

#if defined(CONFIG_DEBUG)
static TBoolean						s_bCakMessageOn = FALSE;
#endif


/**************************************************************************************************/
#define ________CA_OS_Private_Static_Prototype______________________________________________
/**************************************************************************************************/
static TOsStatus 	local_caOs_AllocateTaskObject(struct SOsTaskId **ppData);
static TOsStatus 	local_caOs_FreeTaskObject(struct SOsTaskId *pData);
static TOsStatus 	local_caOs_AllocateSemaphoreObject(struct SOsSemaphoreId **ppData, TUnsignedInt32	ulCount);
static TOsStatus 	local_caOs_FreeSemaphoreObject(struct SOsSemaphoreId *pData);
static TOsStatus 	local_caOs_CheckSemaphoreObject(struct SOsSemaphoreId*	pData);
static TOsStatus 	local_caOs_AllocateQueueObject(struct SOsQueueId **ppData);
static TOsStatus 	local_caOs_FreeQueueObject(struct SOsQueueId *pData);
static TOsStatus 	local_caOs_AllocateTimerObject(struct SOsTimerId **ppData);
static TOsStatus 	local_caOs_FreeTimerObject(struct SOsTimerId *pData);
static TOsStatus 	local_caOs_FindTimerObject(struct SOsTimerId *pData);
static void 		local_caOs_TimerTaskOneShot(unsigned long size,  void *p);
static void 		local_caOs_TimerTaskContinuous(unsigned long size,  void *p);


/**************************************************************************************************/
#define ________CA_OS_Private_Static_Prototype_Body_________________________________________
/**************************************************************************************************/
#if defined(CONFIG_MW_CAS_NAGRA_CA_PRODUCT_ICME) || defined(ENABLE_TERMINAL_FOR_RELEASE) 
#if !defined(CONFIG_DEBUG)
/*
	DLK (cardless) 의 경우 Production SW에도 DLK library가 Uart를 사용할 수 있게 해야 함.
	Nagra에서 생산된 물건을 가지고 debugging을 할 때 Card모델의 경우 Card interface를 통해 하지만
	cardless 모델의 경우 Card interface가 없으므로 Uart를 통해 직접 Library와 연결하여 통신해야 한다.
	실제 나그라에서 자기들 분석 툴을 통해 library와 통신하여 정상 동작하는 지를 확인 하고 있음.
*/
void local_CaOs_InitTerminalForRelease(void)
{
	struct termios tio;
	struct termios stdio;

	memset(&stdio, 0, sizeof(stdio));
	stdio.c_iflag = 0;
	stdio.c_oflag = 0;
	stdio.c_cflag = 0;
	stdio.c_lflag = 0;
	stdio.c_cc[VMIN] = 1;
	stdio.c_cc[VTIME] = 0;
	tcsetattr(STDOUT_FILENO, TCSANOW, &stdio);
	tcsetattr(STDOUT_FILENO, TCSAFLUSH, &stdio);
	if(fcntl(STDIN_FILENO, F_SETFL, O_NONBLOCK) == -1)       // make the reads non-blocking
	{
		HxLOG_Error("fcntl error\n");
	}

	memset(&tio, 0, sizeof(tio));
	tio.c_iflag = 0 /*ICRNL*/;
	tio.c_oflag = 0 /*ONLCR*/;
	tio.c_cflag = CS8|CREAD|CLOCAL;           // 8n1, see termios.h for more information
	tio.c_lflag = 0;
	tio.c_cc[VMIN] = 1;
	tio.c_cc[VTIME] = 5;

	s_ulTtyFd = open("/dev/ttyS0", O_RDWR | O_NONBLOCK);
	cfsetospeed(&tio, B115200);            // 115200 baud
	cfsetispeed(&tio, B115200);            // 115200 baud

	tcsetattr(s_ulTtyFd, TCSANOW, &tio);

	//close(s_ulTtyFd);
}

HUINT8 local_caOs_GetCharForRelease(void)
{
	HUINT8 ucChar;

	while(1)
	{
		if (read(s_ulTtyFd,&ucChar, 1) > 0)
			break;
	}

#if defined (CONFIG_PROD_OS_LINUX)
	if(ucChar == 0x0A) /* DALTS Library에서 LF대신 CR을 원하고 있다,  왠지 Linux여서 그런 듯..*/
		ucChar = 0x0D;
#endif

	return ucChar;
}

void local_caOs_PrintForRelease(const TChar *pxString)
{
	HUINT32	ulLen = 0, ulCnt = 0;
	HUINT8	ucChar = 0, ucPreChar = 0, ucCharCr = '\r';

	if(pxString != NULL)
	{
		ulLen = HxSTD_StrLen(pxString);
		ulCnt = 0;
		while(1)
		{
			ucChar = pxString[ulCnt++];

			if(ucChar == '\n' && ucPreChar != ucCharCr)	/* LF만 올경우 CR/LF로 처리해 주자! */
			{
				write(s_ulTtyFd, (void *)&ucCharCr, 1);
			}
			write(s_ulTtyFd, (void *)&ucChar, 1);

			ucPreChar = ucChar;

			if(ulCnt >= ulLen)
				break;
		}
	}
}
#endif
#endif


/********************************************************************
 Function	: local_caOs_AllocateTaskObject
 Description:
 Input		:
 Output 	:
 Return 	:
 ********************************************************************/
static TOsStatus local_caOs_AllocateTaskObject(struct SOsTaskId **ppData)
{
	TOsStatus eStatus;
	TPsosTaskList *pstTaskList = (TPsosTaskList*)NULL;

	if (ppData == NULL)
		return OS_ERROR;

	WAIT_FOR_SEM(s_ulTaskObjSemId)
	*ppData = (struct SOsTaskId*)NULL;
	eStatus = OS_ERROR;
	pstTaskList = (TPsosTaskList*)OxCAS_Malloc(sizeof(TPsosTaskList));
	if (pstTaskList != (TPsosTaskList*)NULL)
	{
		HxSTD_memset(pstTaskList, 0x00, sizeof (TPsosTaskList));

		*ppData = (struct SOsTaskId*)OxCAS_Malloc(sizeof(struct SOsTaskId));
		if (*ppData)
		{
			(*ppData)->ulId = 0;
			(*ppData)->pfnEntryPoint = NULL;
			(*ppData)->usAllocatedEvents = 0;
			(*ppData)->usCurrentEvent = 0;

			eStatus = OS_NO_ERROR;
			pstTaskList->pstTaskId = *ppData;
			pstTaskList->pNext = s_pstTaskList;
			s_pstTaskList = pstTaskList;
		}
		else
		{
			if(pstTaskList != NULL)
				OxCAS_Free(pstTaskList);
		}
	}
	RELEASE_SEM(s_ulTaskObjSemId)
	return eStatus;
}


/********************************************************************
 Function	: local_caOs_FreeTaskObject
 Description:
 Input		:
 Output 	:
 Return 	:
 ********************************************************************/
static TOsStatus local_caOs_FreeTaskObject(struct SOsTaskId *pData)
{
	TOsStatus eStatus;
	TPsosTaskList *pstCurr, *pstPrev;

	if (pData == NULL)
		return OS_NO_ERROR;

	WAIT_FOR_SEM(s_ulTaskObjSemId)
	eStatus = OS_ERROR;
	pstPrev = NULL;
	for (pstCurr = s_pstTaskList; pstCurr != NULL; pstCurr = pstCurr->pNext)
	{
		if (pstCurr->pstTaskId == pData)
		{
			if (pstPrev != NULL)
			{
				pstPrev->pNext = pstCurr->pNext;
			}
			else
			{
				s_pstTaskList = s_pstTaskList->pNext;
			}
			OxCAS_Free(pData);
			if(pstCurr != NULL)
				OxCAS_Free(pstCurr);
			eStatus = OS_NO_ERROR;
			break;
		}
		pstPrev = pstCurr;
	}
	RELEASE_SEM(s_ulTaskObjSemId)
	return eStatus;
}


/********************************************************************
 Function	: local_caOs_AllocateSemaphoreObject
 Description:
 Input		:
 Output 	:
 Return 	:
 ********************************************************************/
static TOsStatus local_caOs_AllocateSemaphoreObject(struct SOsSemaphoreId **ppData, TUnsignedInt32	ulCount)
{
	TOsStatus eStatus;
	TPsosSemaphoreList *pstSemList = (TPsosSemaphoreList*)NULL;

	if (ppData == NULL)
		return OS_ERROR;

	WAIT_FOR_SEM(s_ulSemaphoreObjSemId)
	eStatus = OS_ERROR;
	*ppData = (struct SOsSemaphoreId*)NULL;
	pstSemList = (TPsosSemaphoreList*)OxCAS_Malloc(sizeof(TPsosSemaphoreList));
	if (pstSemList)
	{
		*ppData = (struct SOsSemaphoreId*)OxCAS_Malloc(sizeof(struct SOsSemaphoreId));
		if (pstSemList != NULL && *ppData != NULL)
		{
			eStatus = OS_NO_ERROR;

			(*ppData)->ulId = 0;
			pstSemList->pstSemaphoreId = *ppData;
			pstSemList->pNext = s_pstSemaphoreList;
			s_pstSemaphoreList = pstSemList;
		}
		else
		{
			if(pstSemList != NULL)
				OxCAS_Free(pstSemList);
		}
	}
	RELEASE_SEM(s_ulSemaphoreObjSemId)
	return eStatus;
}


/********************************************************************
 Function	: local_caOs_FreeSemaphoreObject
 Description:
 Input		:
 Output 	:
 Return 	:
 ********************************************************************/
static TOsStatus local_caOs_FreeSemaphoreObject(struct SOsSemaphoreId *pData)
{
	TOsStatus eStatus;
	TPsosSemaphoreList *pstCurr, *pstPrev;

	if (pData == NULL)
		return OS_NO_ERROR;

	WAIT_FOR_SEM(s_ulSemaphoreObjSemId)
	eStatus = OS_ERROR;
	pstPrev = NULL;
	for (pstCurr = s_pstSemaphoreList; pstCurr != NULL; pstCurr = pstCurr->pNext)
	{
		if (pstCurr->pstSemaphoreId == pData)
		{
			if (pstPrev != NULL)
			{
				pstPrev->pNext = pstCurr->pNext;
			}
			else
			{
				s_pstSemaphoreList = s_pstSemaphoreList->pNext;
			}
			OxCAS_Free(pData);
			if(pstCurr != NULL)
				OxCAS_Free(pstCurr);
			eStatus = OS_NO_ERROR;
			break;
		}
		pstPrev = pstCurr;
	}
	RELEASE_SEM(s_ulSemaphoreObjSemId)
	return eStatus;
}


/********************************************************************
 Function	: local_caOs_CheckSemaphoreObject
 Description:
 Input		:
 Output 	:
 Return 	:
 ********************************************************************/
TOsStatus local_caOs_CheckSemaphoreObject(  struct SOsSemaphoreId *pData)
{
	TOsStatus eStatus;
	TPsosSemaphoreList *pstCurr, *pstPrev;

	if (pData == NULL)
		return OS_ERROR;

	WAIT_FOR_SEM(s_ulSemaphoreObjSemId)
	eStatus = OS_ERROR;
	pstPrev = NULL;
	for (pstCurr = s_pstSemaphoreList; pstCurr != NULL; pstCurr = pstCurr->pNext)
	{
		if (pstCurr->pstSemaphoreId == pData)
		{
			eStatus = OS_NO_ERROR;
			break;
		}
	}
	RELEASE_SEM(s_ulSemaphoreObjSemId)
	return eStatus;
}


/********************************************************************
 Function	: local_caOs_AllocateQueueObject
 Description:
 Input		:
 Output 	:
 Return 	:
 ********************************************************************/
static TOsStatus local_caOs_AllocateQueueObject(struct SOsQueueId **ppData)
{
	TOsStatus 			eStatus;
	TPsosQueueList*		pstQueueList = (TPsosQueueList*)NULL;
	struct SOsQueueId*	pstQueue = (struct SOsQueueId*)NULL;

	if (ppData == NULL)
		return OS_ERROR;

	WAIT_FOR_SEM(s_ulQueueObjSemId)
	eStatus = OS_ERROR;
	pstQueueList = (TPsosQueueList*)OxCAS_Malloc(sizeof(TPsosQueueList));
	if (pstQueueList)
	{
		HxSTD_memset(pstQueueList, 0x00, sizeof(TPsosQueueList));

		pstQueue = (struct SOsQueueId*)OxCAS_Malloc(sizeof(struct SOsQueueId));
		if (pstQueue)
		{
			HxSTD_memset(pstQueue, 0x00, sizeof(struct SOsQueueId));

			eStatus = OS_NO_ERROR;
			*ppData = pstQueue;

			pstQueueList->pstQueueId = *ppData;
			pstQueueList->pNext = s_pstQueueList;
			s_pstQueueList = pstQueueList;
		}
		else
		{
			if(pstQueueList != NULL)
				OxCAS_Free(pstQueueList);
		}
	}
	RELEASE_SEM(s_ulQueueObjSemId)
	return eStatus;
}


/********************************************************************
 Function	: local_caOs_FreeQueueObject
 Description:
 Input		:
 Output 	:
 Return 	:
 ********************************************************************/
static TOsStatus local_caOs_FreeQueueObject(struct SOsQueueId *pData)
{
	TOsStatus eStatus;
	TPsosQueueList *pstCurr, *pstPrev;

	if (pData == NULL)
		return OS_NO_ERROR;

	WAIT_FOR_SEM(s_ulQueueObjSemId)
	eStatus = OS_ERROR;
	pstPrev = NULL;
	for (pstCurr = s_pstQueueList; pstCurr != NULL; pstCurr = pstCurr->pNext)
	{
		if (pstCurr->pstQueueId == pData)
		{
			if (pstPrev != NULL)
			{
				pstPrev->pNext = pstCurr->pNext;
			}
			else
			{
				s_pstQueueList = s_pstQueueList->pNext;
			}
			OxCAS_Free(pData);
			if(pstCurr != NULL)
				OxCAS_Free(pstCurr);
			eStatus = OS_NO_ERROR;
			break;
		}
		pstPrev = pstCurr;
	}
	RELEASE_SEM(s_ulQueueObjSemId)
	return eStatus;
}


/********************************************************************
 Function	: local_caOs_AllocateTimerObject
 Description:
 Input		:
 Output 	:
 Return 	:
 ********************************************************************/
static TOsStatus local_caOs_AllocateTimerObject(struct SOsTimerId **ppData)
{
	TOsStatus eStatus;
	TPsosTimerList *pstTimerList = (TPsosTimerList*)NULL;
	struct SOsTimerId *pstTimer = (struct SOsTimerId*)NULL;

	if (ppData == NULL)
		return OS_ERROR;

	WAIT_FOR_SEM(s_ulTimerObjSemId)

	eStatus = OS_ERROR;
	pstTimerList = (TPsosTimerList*)OxCAS_Malloc(sizeof(TPsosTimerList));
	if (pstTimerList)
	{
		HxSTD_memset(pstTimerList, 0x00, sizeof(TPsosTimerList));

		pstTimer = (struct SOsTimerId*)OxCAS_Malloc(sizeof(struct SOsTimerId));
		if (pstTimer)
		{
			HxSTD_memset(pstTimer, 0x00, sizeof(struct SOsTimerId));

			eStatus = OS_NO_ERROR;
			*ppData = pstTimer;
			pstTimerList->pstTimerId = *ppData;
			pstTimerList->pNext = s_pstTimerList;
			s_pstTimerList = pstTimerList;
		}
		else
		{
			if(pstTimerList != NULL)
				OxCAS_Free(pstTimerList);
		}
	}
	RELEASE_SEM(s_ulTimerObjSemId)
	return eStatus;
}


/********************************************************************
 Function	: local_caOs_FreeTimerObject
 Description:
 Input		:
 Output 	:
 Return 	:
 ********************************************************************/
static TOsStatus local_caOs_FreeTimerObject(struct SOsTimerId *pData)
{
	TOsStatus eStatus;
	TPsosTimerList *pstCurr, *pstPrev;

	if (pData == NULL)
		return OS_NO_ERROR;

	WAIT_FOR_SEM(s_ulTimerObjSemId)
	eStatus = OS_ERROR;
	pstPrev = NULL;
	for (pstCurr = s_pstTimerList; pstCurr != NULL; pstCurr = pstCurr->pNext)
	{
		if (pstCurr->pstTimerId == pData)
		{
			if (pstPrev != NULL)
			{
				pstPrev->pNext = pstCurr->pNext;
			}
			else
			{
				s_pstTimerList = s_pstTimerList->pNext;
			}
			OxCAS_Free(pData);
			if(pstCurr != NULL)
				OxCAS_Free(pstCurr);
			eStatus = OS_NO_ERROR;
			break;
		}
		pstPrev = pstCurr;
	}
	RELEASE_SEM(s_ulTimerObjSemId)
	return eStatus;
}


/********************************************************************
 Function	: local_caOs_FreeTimerObject
 Description:
 Input		:
 Output 	:
 Return 	:
 ********************************************************************/
static TOsStatus local_caOs_FindTimerObject(struct SOsTimerId *pData)
{
	TOsStatus eStatus;
	TPsosTimerList *pstCurr, *pstPrev;

	if (pData == NULL)
		return OS_ERROR;

	WAIT_FOR_SEM(s_ulTimerObjSemId)
	eStatus = OS_ERROR;
	pstPrev = NULL;
	for (pstCurr = s_pstTimerList; pstCurr != NULL; pstCurr = pstCurr->pNext)
	{
		if (pstCurr->pstTimerId == pData)
		{
			eStatus = OS_NO_ERROR;
			break;
		}
		pstPrev = pstCurr;
	}
	RELEASE_SEM(s_ulTimerObjSemId)
	return eStatus;
}


/********************************************************************
 Function	: local_caOs_TimerTaskOneShot
 Description:
 Input		:
 Output 	:
 Return 	:
 ********************************************************************/
static void local_caOs_TimerTaskOneShot(unsigned long size, void *p)
{
	TOsTimerId     	xTimerId;
  	TUnsignedInt32 	ulTimeout;
   	TUnsignedInt32 	*pulArg;
	TUnsignedInt32	ulEvent_s;
	TSignedInt32		nErrCode;

	if (p == NULL)
		return;

	pulArg = (TUnsignedInt32*)p;
	xTimerId = (TOsTimerId)pulArg[0];
	ulTimeout = pulArg[1];

//	WAIT_FOR_SEM(s_ulTimerObjSemId)
	ulEvent_s = (TUnsignedInt32)xTimerId->usEventToSend;
	nErrCode = CA_NA_OsEventSend(xTimerId->stCreatorTaskId->hEvent, ulEvent_s);
	if (nErrCode != ERR_OK)
	{
		HxLOG_Error("[local_caOs_TimerTaskOneShot] CA_NA_OsEventSend Error\n");
	}
	xTimerId->eTimerState = eTimer_Stopped;
//	RELEASE_SEM(s_ulTimerObjSemId)
}


/********************************************************************
 Function	: local_caOs_TimerTaskContinuous
 Description:
 Input		:
 Output 	:
 Return 	:
 ********************************************************************/
static void local_caOs_TimerTaskContinuous(unsigned long size, void *p)
{
	TOsTimerId     	xTimerId;
  	TUnsignedInt32 	ulTimeout;
  	TUnsignedInt32 	*pulArg;
	TUnsignedInt32	ulEvent_s;
	TSignedInt32		nErrCode;

	if (p == NULL)
		return;

	pulArg = (TUnsignedInt32*) p;
	xTimerId = (TOsTimerId)pulArg[0];
	ulTimeout = pulArg[1];

//	WAIT_FOR_SEM(s_ulTimerObjSemId)
	ulEvent_s = (TUnsignedInt32)xTimerId->usEventToSend;
	nErrCode = CA_NA_OsEventSend(xTimerId->stCreatorTaskId->hEvent, ulEvent_s);
	if (nErrCode != ERR_OK)
	{
		HxLOG_Error("[local_caOs_TimerTaskContinuous] CA_NA_OsEventSend Error\n");
	}
//	RELEASE_SEM(s_ulTimerObjSemId)
}


/**************************************************************************************************/
#define ________CA_OS_Public_Functions_Body________________________________________________
/**************************************************************************************************/
/********************************************************************
 Function	: osTaskCreate
 Description: Task create
 Input		:
 Output 	:
 Return 	:
 ********************************************************************/
TOsStatus osTaskCreate(TOsEntryPoint xEntryPoint, TOsStackSize xStackSize, TOsPriority xPriority, TOsTaskId *pxTaskId)
{
	TSignedInt32		nErrCode;
	TUnsignedInt32	ulPrio = OS_TASK_PRIORITY_VERY_LOW;
	TUnsignedInt32 	ulTaskId;
	TOsStackSize   	ulStackSize = xStackSize;
	TUnsignedInt8		ucTaskName[16];
	static TUnsignedInt32 sulNaTaskNum = 0;

	HxLOG_Print("stacksize = %d, priority = %d\n", (int)xStackSize, (int)xPriority);

	if ( (xEntryPoint == NULL)
		|| (pxTaskId == NULL) )
	{
		return OS_ERROR;
	}

	if ( (xStackSize == 0)
		|| (xPriority < OS_PRIORITY_VERY_LOW)
		|| (xPriority > OS_PRIORITY_VERY_HIGH) )
	{
		*pxTaskId = NULL;
		return OS_ERROR;
	}

	switch (xPriority)
	{
		case OS_PRIORITY_VERY_LOW:
			ulPrio = OS_TASK_PRIORITY_VERY_LOW;
			break;
		case OS_PRIORITY_LOW:
			ulPrio = OS_TASK_PRIORITY_LOW;
			break;
		case OS_PRIORITY_NORMAL:
			ulPrio = OS_TASK_PRIORITY_NORMAL;
			break;
		case OS_PRIORITY_HIGH:
			ulPrio = OS_TASK_PRIORITY_HIGH;
			break;
		case OS_PRIORITY_VERY_HIGH:
			ulPrio = OS_TASK_PRIORITY_VERY_HIGH;
			break;
	}

	if (local_caOs_AllocateTaskObject(pxTaskId) == OS_NO_ERROR)
	{
		if (xStackSize < 1024)
			ulStackSize = 1024;

		HxSTD_memset((TChar*)ucTaskName, 0x00, 16);
		snprintf((TChar*)ucTaskName, 16, "%3s%4d", "TSK", (int)sulNaTaskNum++);

		nErrCode = VK_TASK_Create((CaOsThreadFunc)xEntryPoint, ulPrio, ulStackSize, (TChar *)ucTaskName, (void *)NULL, (unsigned long*)&ulTaskId, 1);
		if (nErrCode == ERR_OK)
		{
			Event_Handle_t hEvent;

			HxLOG_Print("pxTaskId = 0x%x, taskId=0x%x\n", (unsigned int)*pxTaskId, (unsigned int)ulTaskId);
			hEvent = CA_NA_OsEventCreate();
			if (hEvent)
			{
				WAIT_FOR_SEM(s_ulTaskObjSemId)
				(*pxTaskId)->ulId 				= ulTaskId;
				(*pxTaskId)->pfnEntryPoint		= xEntryPoint;
				(*pxTaskId)->usAllocatedEvents	= 0;
				(*pxTaskId)->eTaskState 		= eTask_Dormant;
				(*pxTaskId)->hEvent 			= hEvent;
				RELEASE_SEM(s_ulTaskObjSemId)

				nErrCode = VK_TASK_Start(ulTaskId);
				if (nErrCode == ERR_OK)
				{
					(*pxTaskId)->eTaskState = eTask_Running;
					return OS_NO_ERROR;
				}
				else
				{
					HxLOG_Error("VK_TASK_Start ERROR(0x%x)\n", (unsigned int)nErrCode);
				}
			}
			else
			{
				HxLOG_Error("CA_NA_OsEventCreate ERROR\n");
				VK_TASK_Destroy(ulTaskId);
			}
		}
		else
		{
			HxLOG_Error("VK_TASK_Create ERROR(0x%x)\n", (unsigned int)nErrCode);
		}
		local_caOs_FreeTaskObject(*pxTaskId);
	}
	*pxTaskId = NULL;
	return OS_ERROR;
}


/********************************************************************
 Function	: osTaskIdentify
 Description: Task identify
 Input		:
 Output 	:
 Return 	:
 ********************************************************************/
TOsStatus osTaskIdentify(TOsTaskId *pxTaskId)
{
	TOsStatus 		eStatus;
	TSignedInt32 		nErrCode;
	TUnsignedInt32 	ulTaskId;
	TPsosTaskList		*pstTask;

	if (pxTaskId == NULL)
	{
		return OS_ERROR;
	}

	WAIT_FOR_SEM(s_ulTaskObjSemId)
	*pxTaskId = NULL;
	eStatus = OS_ERROR;

	nErrCode = VK_TASK_GetCurrentId((unsigned long*)&ulTaskId);
	if (nErrCode == ERR_OK)
	{
		for (pstTask = s_pstTaskList; pstTask != NULL; pstTask = pstTask->pNext)
		{
			if (pstTask->pstTaskId->ulId == 0)
			{
				HxLOG_Error("error occurred but I did workaround...\n");
				continue;
			}

			HxLOG_Print("TaskID(0x%x)(0x%x)\n", (unsigned int)(pstTask->pstTaskId), (unsigned int)ulTaskId);
			if (((TOsTaskId)(pstTask->pstTaskId))->ulId == ulTaskId)
			{
				*pxTaskId = pstTask->pstTaskId;
				eStatus = OS_NO_ERROR;
				break;
			}
		}
	}
	else
	{
		HxLOG_Print("ERROR(0x%x)\n", (unsigned int)nErrCode);
	}
	RELEASE_SEM(s_ulTaskObjSemId)
	return eStatus;
}


/********************************************************************
 Function	: osTaskWait
 Description:
 Input		:
 Output 	:
 Return 	:
 ********************************************************************/
TOsStatus osTaskWait(TOsTime xTime)
{
	TSignedInt32 		nErrCode;
	TUnsignedInt32 	ulTaskId;
	TPsosTaskList		*pstTask = NULL;

	WAIT_FOR_SEM(s_ulTaskObjSemId)
	nErrCode = VK_TASK_GetCurrentId((unsigned long*)&ulTaskId);
	if (nErrCode == ERR_OK)
	{
		for (pstTask = s_pstTaskList; pstTask != NULL; pstTask = pstTask->pNext)
		{
			if (pstTask->pstTaskId->ulId == 0)
			{
				HxLOG_Error("[osTaskWait] Error Occurred but I did workaround...\n");
				continue;
			}

			if (((TOsTaskId)(pstTask->pstTaskId))->ulId == ulTaskId)
			{
				pstTask->pstTaskId->eTaskState = eTask_Susupended;
				break;
			}
		}
	}
	RELEASE_SEM(s_ulTaskObjSemId)
	if(xTime != 0)
	{
              VK_TASK_Sleep((unsigned long)xTime);
	}
	WAIT_FOR_SEM(s_ulTaskObjSemId)

	if (pstTask != NULL && pstTask->pstTaskId != NULL)
	{
		pstTask->pstTaskId->eTaskState = eTask_Running;
	}

	RELEASE_SEM(s_ulTaskObjSemId)
	return OS_NO_ERROR;
}


/********************************************************************
 Function	: osTaskDelete
 Description:
 Input		:
 Output 	:
 Return 	:
 ********************************************************************/
TOsStatus osTaskDelete(TOsTaskId xTaskId)
{
	TSignedInt32		nErrCode;
  	TOsTaskId 		pstTaskId;
	TOsStatus 		eStatus;
	TPsosTaskList		*pstTask;
	TBoolean			bIsInTaskList = FALSE;

	if (xTaskId == NULL)
	{
		return OS_ERROR;
	}

	HxLOG_Print("xTaskId = 0x%x, taskId = 0x%x\n", (unsigned int)xTaskId, (unsigned int)xTaskId->ulId);

	for (pstTask = s_pstTaskList; pstTask != NULL; pstTask = pstTask->pNext)
	{
		if ((TOsTaskId)(pstTask->pstTaskId) == xTaskId)
		{
			eStatus = OS_NO_ERROR;
			bIsInTaskList = TRUE;
			break;
		}
	}

	if (!bIsInTaskList)
	{
		HxLOG_Error("input pTaskId is already deleted error\n");
		return OS_ERROR;
	}

	if (xTaskId->eTaskState != eTask_Running)
	{
		HxLOG_Error("Wrong command to delete waiting task, So I returned it\n");
		VK_TASK_Sleep(5);
		return OS_ERROR;
	}

	eStatus = osTaskIdentify(&pstTaskId);
	if (eStatus == OS_NO_ERROR )
	{
		if (pstTaskId == xTaskId)
		{
			HxLOG_Error("Self deletion\n");
			return OS_ERROR;
		}
	}
	else
	{
		HxLOG_Error("ERROR - cannot identify the current task\n");
	}

	nErrCode = CA_NA_OsEventDelete(xTaskId->hEvent);
	if (nErrCode != ERR_OK)
	{
		HxLOG_Error("CA_NA_OsEventDelete ERROR(0x%x)\n", (unsigned int)nErrCode);
		return OS_ERROR;
	}
	xTaskId->hEvent = NULL;

	nErrCode = VK_TASK_Destroy(xTaskId->ulId);
	if (nErrCode == ERR_OK)
	{
		return local_caOs_FreeTaskObject(xTaskId);
	}
	else
	{
		HxLOG_Error("ERROR(0x%x)\n", (unsigned int)nErrCode);
		return OS_ERROR;
	}
}


/********************************************************************
 Function	: osEventAllocate
 Description:
 Input		:
 Output 	:
 Return 	:
 ********************************************************************/
TOsStatus osEventAllocate(TOsEvent *pxEvent)
{
	TUnsignedInt8		i;
  	TOsTaskId 		pstTaskId;
	TOsStatus 		eStatus;

	if (pxEvent == NULL)
	{
		return OS_ERROR;
	}

	eStatus = osTaskIdentify(&pstTaskId);
	if (eStatus == OS_NO_ERROR)
	{
		WAIT_FOR_SEM(s_ulTaskObjSemId)
		for (i=0; i<16; i++)
		{
			if ((((pstTaskId->usAllocatedEvents) >> i) & 0x0001) == 0)
			{
				break;
			}
		}

		if (i < 16)
		{
			*pxEvent = (0x0001 << i);
			pstTaskId->usAllocatedEvents |= *pxEvent;
			RELEASE_SEM(s_ulTaskObjSemId)
			return OS_NO_ERROR;
		}
		RELEASE_SEM(s_ulTaskObjSemId)
	}

	*pxEvent = 0;
	return OS_ERROR;
}


/********************************************************************
 Function	: osEventFree
 Description:
 Input		:
 Output 	:
 Return 	:
 ********************************************************************/
TOsStatus osEventFree(TOsEvent	xEvents)
{
  	TOsTaskId 	pstTaskId;
	TOsStatus 	eStatus;

	eStatus = osTaskIdentify(&pstTaskId);
	if (eStatus == OS_NO_ERROR)
	{
		WAIT_FOR_SEM(s_ulTaskObjSemId)
		pstTaskId->usAllocatedEvents &= ~xEvents;
#if defined(CONFIG_MW_CAS_NAGRA_DALTEST)
		pstTaskId->hEvent->ulEvent = 0;
#endif
		RELEASE_SEM(s_ulTaskObjSemId)
		return OS_NO_ERROR;
	}
	return OS_ERROR;
}


/********************************************************************
 Function	: osEventReceive
 Description:
 Input		:
 Output 	:
 Return 	:
 ********************************************************************/
 TOsStatus osEventReceive(TOsEvent	xExpectedEvents, TOsEventFlags xFlags, TOsTime xTimeout, TOsEvent *pxReceivedEvents)
{
	TSignedInt32 		nErrCode;
	TUnsignedInt32 	ulEvtOpt;
	TOsEvent  		usReceivedEvents = 0;
	TUnsignedInt32 	ulReceiveEvents = 0;
  	TOsTaskId 		pstTaskId;
	TOsStatus 		eStatus;
	TOsEvent			usCurEvent;

	if (pxReceivedEvents == NULL)
	{
		return OS_ERROR;
	}

	eStatus = osTaskIdentify(&pstTaskId);
	*pxReceivedEvents = 0;

	if (eStatus == OS_NO_ERROR)
	{
		WAIT_FOR_SEM(s_ulTaskObjSemId)
		usCurEvent = pstTaskId->usAllocatedEvents;
		RELEASE_SEM(s_ulTaskObjSemId)

		if ((xExpectedEvents & (usCurEvent)) == xExpectedEvents)
		{
			if (xFlags == OS_EVENT_NO_WAIT)
			{
				ulEvtOpt = eWait_Any_Event | eEvent_No_Wait;
			}
			else
			{
				ulEvtOpt = eWait_Any_Event | eEvent_Wait;
			}
#if defined(CONFIG_MW_CAS_NAGRA_DALTEST)
			VK_TASK_Sleep(1);
#endif
			nErrCode = (TSignedInt32)CA_NA_OsEventReceive(pstTaskId->hEvent, (TUnsignedInt32)xExpectedEvents, (TUnsignedInt32*)&ulReceiveEvents, ulEvtOpt, (TUnsignedInt32)xTimeout);
			//if (errCode == ERR_OK || errCode == ERR_TIMEOUT || errCode == ERR_NOEVS)
			{
				usReceivedEvents = (TUnsignedInt16)ulReceiveEvents;
#if 0//defined(CONFIG_MW_CAS_NAGRA_DALTEST)
				if(nErrCode == ERR_OK || usReceivedEvents == (TUnsignedInt16)xExpectedEvents)
#endif
					*pxReceivedEvents = (usReceivedEvents & xExpectedEvents);

				HxLOG_Print("xExpectedEvents [0x%x], usReceivedEvents[0x%x] *pxReceivedEvents[0x%x] nErrCode[%d]\n", xExpectedEvents, usReceivedEvents, *pxReceivedEvents, nErrCode);
				return OS_NO_ERROR;
			}
		}
	}
	else
	{
		HxLOG_Error("Unidentified task(0x%x)\n", (unsigned int)pstTaskId);
	}
	return OS_ERROR;
}


/********************************************************************
 Function	: osEventSend
 Description:
 Input		:
 Output 	:
 Return 	:
 ********************************************************************/
 TOsStatus osEventSend(TOsTaskId xTaskId, TOsEvent xSentEvents)
{
	TOsEvent			usCurEvent;
	TUnsignedInt32 	ulSendEvents = 0;
	TPsosTaskList		*pstTask;
	TBoolean			bIsInTaskList = FALSE;

	if (xTaskId == NULL)
	{
		return OS_ERROR;
	}

	WAIT_FOR_SEM(s_ulTaskObjSemId)
	for (pstTask = s_pstTaskList; pstTask != NULL; pstTask = pstTask->pNext)
	{
		if ((TOsTaskId)(pstTask->pstTaskId) == xTaskId)
		{
			bIsInTaskList = TRUE;
			break;
		}
	}

	if (bIsInTaskList == FALSE)
	{
		RELEASE_SEM(s_ulTaskObjSemId)
		return OS_ERROR;
	}

	usCurEvent = xTaskId->usAllocatedEvents;
	RELEASE_SEM(s_ulTaskObjSemId)

	if (((usCurEvent) & xSentEvents) == xSentEvents)
	{
		ulSendEvents = (TUnsignedInt32)xSentEvents;
		if (CA_NA_OsEventSend(xTaskId->hEvent, ulSendEvents) == 0)
		{
			return OS_NO_ERROR;
		}
	}
	return OS_ERROR;
}


/********************************************************************
 Function	: osSemaphoreCreate
 Description:
 Input		:
 Output 	:
 Return 	:
 ********************************************************************/
TOsStatus osSemaphoreCreate(TOsCount xCount, TOsSemaphoreId *pxSemaphoreId)
{
	TSignedInt32 		nErrCode;
	TUnsignedInt32 	ulSmid;
	TUnsignedInt32 	ulCount;
	TUnsignedInt8		ucSemName[16];
	static TSignedInt32 s_ulSemCount = 0;

	if (pxSemaphoreId == NULL)
		return OS_ERROR;

	ulCount = (TUnsignedInt32)xCount;
	if (local_caOs_AllocateSemaphoreObject(pxSemaphoreId, ulCount) == OS_NO_ERROR)
	{
		if (*pxSemaphoreId)
		{
			HxLOG_Print("smid = 0x%x\n", (unsigned int)*pxSemaphoreId);
		}
		else
		{
			HxLOG_Critical("Semaphore ID is 0\n");
			local_caOs_FreeSemaphoreObject(*pxSemaphoreId);
			return OS_ERROR;
		}

		HxSTD_memset((TChar*)ucSemName, 0x00, 16);
		snprintf((TChar*)ucSemName, 16, "%s%4d", "SEM", (int)s_ulSemCount++);
		nErrCode = VK_SEM_CreateWithCount((unsigned long*)&ulSmid, ulCount, (const char *)ucSemName, VK_SUSPENDTYPE_PRIORITY);
		if (nErrCode == ERR_OK)
		{
			WAIT_FOR_SEM(s_ulSemaphoreObjSemId)
			(*pxSemaphoreId)->ulId = ulSmid;
			RELEASE_SEM(s_ulSemaphoreObjSemId)

			HxLOG_Print("[%08x]\n", (unsigned int)ulSmid);
			return OS_NO_ERROR;
		}
		else
		{
			HxLOG_Error("ERROR(0x%x)\n", (unsigned int)nErrCode);
		}
		local_caOs_FreeSemaphoreObject(*pxSemaphoreId);
	}
	return OS_ERROR;
}


/********************************************************************
 Function	: osSemaphoreGet
 Description:
 Input		:
 Output 	:
 Return 	:
 ********************************************************************/
TOsStatus osSemaphoreGet(TOsSemaphoreId	xSemaphoreId)
{
	TSignedInt32 		nErrCode;
	TUnsignedInt32 	ulSmid;

	if (xSemaphoreId == NULL)
	{
		return OS_ERROR;
	}

	if (local_caOs_CheckSemaphoreObject(xSemaphoreId) == OS_ERROR)
	{
		return OS_ERROR;
	}

	WAIT_FOR_SEM(s_ulSemaphoreObjSemId)
	HxLOG_Print("xSemaphoreId = 0x%x\n", (unsigned int)xSemaphoreId);
	ulSmid = xSemaphoreId->ulId;
	RELEASE_SEM(s_ulSemaphoreObjSemId)

	nErrCode = VK_SEM_Get(ulSmid);
	if (nErrCode == ERR_OK)
	{
		return OS_NO_ERROR;
	}
	else
	{
		HxLOG_Error("ERROR(0x%x)\n", (unsigned int)nErrCode);
	}
	return OS_ERROR;
}


/********************************************************************
 Function	: osSemaphoreRelease
 Description:
 Input		:
 Output 	:
 Return 	:
 ********************************************************************/
TOsStatus osSemaphoreRelease(TOsSemaphoreId xSemaphoreId)
{
	TSignedInt32 		nErrCode;
	TUnsignedInt32 	ulSmid;

	if (xSemaphoreId == NULL)
	{
		HxLOG_Error("xSemaphoreId null\n");
		return OS_ERROR;
	}

	if (local_caOs_CheckSemaphoreObject(xSemaphoreId) == OS_ERROR)
	{
		return OS_ERROR;
	}

	WAIT_FOR_SEM(s_ulSemaphoreObjSemId)
	HxLOG_Print("xSemaphoreId = 0x%x\n", (unsigned int)xSemaphoreId);
	ulSmid = xSemaphoreId->ulId;
	RELEASE_SEM(s_ulSemaphoreObjSemId)

	nErrCode = VK_SEM_Release(ulSmid);
	if (nErrCode == ERR_OK)
	{
		return OS_NO_ERROR;
	}
	else
	{
		HxLOG_Error("ERROR(0x%x)\n", (unsigned int)nErrCode);
	}
	return OS_ERROR;
}


/********************************************************************
 Function	: osSemaphoreDelete
 Description:
 Input		:
 Output 	:
 Return 	:
 ********************************************************************/
TOsStatus osSemaphoreDelete(TOsSemaphoreId xSemaphoreId)
{
	TSignedInt32 			nErrCode;
	TUnsignedInt32 		ulSmid;
	TPsosSemaphoreList 	*pstSmCurr;
	TBoolean				bIsInSmList = FALSE;

	WAIT_FOR_SEM(s_ulSemaphoreObjSemId)

	if (xSemaphoreId == NULL)
	{
		RELEASE_SEM(s_ulSemaphoreObjSemId)
		HxLOG_Error("xSemaphoreId null\n");
		return OS_ERROR;
	}

	for (pstSmCurr = s_pstSemaphoreList; pstSmCurr != NULL; pstSmCurr = pstSmCurr->pNext)
	{
		if (pstSmCurr->pstSemaphoreId->ulId == xSemaphoreId->ulId)
		{
			bIsInSmList = TRUE;
			break;
		}
	}

	if (!bIsInSmList)
	{
		RELEASE_SEM(s_ulSemaphoreObjSemId)
		HxLOG_Error("Sema_list null\n");
		return OS_ERROR;
	}

	ulSmid = xSemaphoreId->ulId;
	nErrCode = VK_SEM_Destroy(ulSmid);
	RELEASE_SEM(s_ulSemaphoreObjSemId)
	if (nErrCode == ERR_OK)
	{
		if (local_caOs_FreeSemaphoreObject(xSemaphoreId) == OS_NO_ERROR)
		{
			return OS_NO_ERROR;
		}
	}
	return OS_ERROR;
}


/********************************************************************
 Function	: osTimerCreate
 Description:
 Input		:
 Output 	:
 Return 	:
 ********************************************************************/
TOsStatus osTimerCreate(TOsEvent xEventToSend, TOsTimerMode xTimerMode, TOsTimerId *pxTimerId)
{
  	TOsTaskId pstTaskId;

	if (pxTimerId == NULL)
		return OS_ERROR;

	if (osTaskIdentify(&pstTaskId) == OS_NO_ERROR)
	{
		if (xEventToSend == 0)
		{
			HxLOG_Error("No event to send specified\n");
			return OS_ERROR;
		}

		if ((xEventToSend & (pstTaskId->usAllocatedEvents)) != xEventToSend)
		{
			HxLOG_Error("Event not allocated in the thread\n");
			return OS_ERROR;
		}

		if (local_caOs_AllocateTimerObject(pxTimerId) == OS_NO_ERROR)
		{
			WAIT_FOR_SEM(s_ulTimerObjSemId)
			(*pxTimerId)->ulId = 0;
			(*pxTimerId)->ulTimerTaskId = 0;
			(*pxTimerId)->stCreatorTaskId = pstTaskId;
			(*pxTimerId)->usEventToSend = xEventToSend;
			(*pxTimerId)->eTimerState = eTimer_Dormant;
			(*pxTimerId)->eTimerMode = xTimerMode;
			RELEASE_SEM(s_ulTimerObjSemId)
			return OS_NO_ERROR;
		}
	}
	return OS_ERROR;
}


/********************************************************************
 Function	: osTimerStart
 Description:
 Input		:
 Output 	:
 Return 	:
 ********************************************************************/
TOsStatus osTimerStart(TOsTimerId xTimerId, TOsTime xTimeout)
{
	TSignedInt32 		nErrCode = ERR_OK;
	TUnsignedInt32 	ulArg[4];
	TUnsignedInt32	ulTid;
	TimerState_t		eTimerState;
	TOsTimerMode	eTimerMode;
	TOsTime			ulTimeout;

	if (local_caOs_FindTimerObject(xTimerId) == OS_ERROR)
		return OS_ERROR;

	WAIT_FOR_SEM(s_ulTimerObjSemId)
	WAIT_FOR_SEM(s_ulTaskObjSemId)
	if ((xTimerId->usEventToSend & xTimerId->stCreatorTaskId->usAllocatedEvents) != xTimerId->usEventToSend)
	{
		RELEASE_SEM(s_ulTaskObjSemId)
		RELEASE_SEM(s_ulTimerObjSemId)
		return OS_ERROR;
	}
	RELEASE_SEM(s_ulTaskObjSemId)

	if (xTimeout == 0)
	{
		nErrCode = CA_NA_OsEventSend(xTimerId->stCreatorTaskId->hEvent, (TUnsignedInt32)xTimerId->usEventToSend);
		if (nErrCode != ERR_OK)
		{
			HxLOG_Error("CA_NA_OsEventSend ERROR[%x]\n", (unsigned int)nErrCode);
			RELEASE_SEM(s_ulTimerObjSemId)
			return OS_ERROR;
		}
		RELEASE_SEM(s_ulTimerObjSemId)
		return OS_NO_ERROR;
	}

	ulArg[0] = (TUnsignedInt32)xTimerId;
	ulArg[1] = xTimeout;

	eTimerState = xTimerId->eTimerState;
	eTimerMode = xTimerId->eTimerMode;
	ulTimeout = xTimerId->ulTimeout;

	RELEASE_SEM(s_ulTimerObjSemId)

	if (eTimerState == eTimer_Dormant)
	{
		if (eTimerMode == OS_TIMER_ONE_SHOT)
	    {
			nErrCode = VK_TIMER_EventAfter((TUnsignedInt32)xTimeout, local_caOs_TimerTaskOneShot, (void *)ulArg, (TSignedInt32)8, (unsigned long*)&ulTid);
			if (nErrCode != ERR_OK)
			{
				HxLOG_Error("VK_TIMER_EventAfter() return error\n");
			}
		}
		else if (eTimerMode == OS_TIMER_CONTINUOUS)
		{
			nErrCode = VK_TIMER_EventEvery((TUnsignedInt32)xTimeout, local_caOs_TimerTaskContinuous, (void *)ulArg, (TSignedInt32)8, (unsigned long*)&ulTid);
			if (nErrCode != ERR_OK)
			{
				HxLOG_Error("VK_TIMER_EventEvery() return error\n");
			}
		}
		WAIT_FOR_SEM(s_ulTimerObjSemId)
		xTimerId->ulTimerTaskId = ulTid;
		xTimerId->ulTimeout = xTimeout;
	}
	else if (eTimerState == eTimer_Stopped)
	{
		if (eTimerMode == OS_TIMER_ONE_SHOT)
		{
			nErrCode = VK_TIMER_EventAfter((TUnsignedInt32)xTimeout, local_caOs_TimerTaskOneShot, (void *)ulArg, (TSignedInt32)8, (unsigned long*)&ulTid);
			if (nErrCode != ERR_OK)
			{
				HxLOG_Error("VK_TIMER_EventAfter() return error\n");
			}
		}
		else if (eTimerMode == OS_TIMER_CONTINUOUS)
		{
			nErrCode = VK_TIMER_EventEvery((TUnsignedInt32)xTimeout, local_caOs_TimerTaskContinuous, (void *)ulArg, (TSignedInt32)8, (unsigned long*)&ulTid);
			if (nErrCode != ERR_OK)
			{
				HxLOG_Error("VK_TIMER_EventAfter() return error\n");
			}
		}
		WAIT_FOR_SEM(s_ulTimerObjSemId)
		xTimerId->ulTimerTaskId = ulTid;
		xTimerId->ulTimeout = xTimeout;
	}
	else if (eTimerState == eTimer_Running)
	{
//		if (ulTimeout != xTimeout)
		{
			if (eTimerMode == OS_TIMER_ONE_SHOT)
			{
				nErrCode = VK_TIMER_Cancel(xTimerId->ulTimerTaskId);
				if (nErrCode != ERR_OK)
				{
					HxLOG_Error("VK_TIMER_Cancel() return error\n");
				}
				nErrCode = VK_TIMER_EventAfter((TUnsignedInt32)xTimeout, local_caOs_TimerTaskOneShot, (void *)ulArg, (TSignedInt32)8, (unsigned long*)&ulTid);
				if (nErrCode != ERR_OK)
				{
					HxLOG_Error("VK_TIMER_EventAfter() return error\n");
				}
			}
			else
			{
				nErrCode = VK_TIMER_Cancel(xTimerId->ulTimerTaskId);
				if (nErrCode != ERR_OK)
				{
					HxLOG_Error("VK_TIMER_Cancel() return error\n");
				}
				nErrCode = VK_TIMER_EventEvery((TUnsignedInt32)xTimeout, local_caOs_TimerTaskContinuous, (void *)ulArg, (TSignedInt32)8, (unsigned long*)&ulTid);
				if (nErrCode != ERR_OK)
				{
					HxLOG_Error("VK_TIMER_EventAfter() return error\n");
				}
			}
			WAIT_FOR_SEM(s_ulTimerObjSemId)
			xTimerId->ulTimerTaskId = ulTid;
			xTimerId->ulTimeout = xTimeout;
		}
	}
	else
	{
		HxLOG_Error("Invalid Timer State(0x%x)\n", eTimerState);
		return OS_ERROR;
	}

	if (nErrCode == ERR_OK)
	{
		xTimerId->eTimerState = eTimer_Running;
		xTimerId->ulTimeout = xTimeout;
		RELEASE_SEM(s_ulTimerObjSemId)
		return OS_NO_ERROR;
	}
	else
	{
		xTimerId->ulTimeout = 0;
		HxLOG_Error("ERROR(0x%x)\n", (unsigned int)nErrCode);
	}
	RELEASE_SEM(s_ulTimerObjSemId)
	return OS_ERROR;
}


/********************************************************************
 Function	: osTimerStop
 Description:
 Input		:
 Output 	:
 Return 	:
 ********************************************************************/
TOsStatus osTimerStop(TOsTimerId xTimerId)
{
	TSignedInt32 nErrCode;

	if (local_caOs_FindTimerObject( xTimerId ) == OS_ERROR)
	{
		return OS_ERROR;
	}

	WAIT_FOR_SEM(s_ulTimerObjSemId)
	if ( (xTimerId->eTimerState == eTimer_Stopped)
		|| (xTimerId->eTimerState == eTimer_Dormant) )
	{
		RELEASE_SEM(s_ulTimerObjSemId)
		return OS_NO_ERROR;
	}
	RELEASE_SEM(s_ulTimerObjSemId)

	nErrCode = VK_TIMER_Cancel(xTimerId->ulTimerTaskId);

	WAIT_FOR_SEM(s_ulTimerObjSemId)
	if (nErrCode == ERR_OK)
	{
		xTimerId->eTimerState = eTimer_Stopped;
		RELEASE_SEM(s_ulTimerObjSemId)
		return OS_NO_ERROR;
	}
	else
	{
		HxLOG_Error("ERROR(0x%x)(task id = 0x%x)\n", (unsigned int)nErrCode, (unsigned int)xTimerId->ulTimerTaskId);
	}
	RELEASE_SEM(s_ulTimerObjSemId)
	return OS_ERROR;
}


/********************************************************************
 Function	: osTimerDelete
 Description:
 Input		:
 Output 	:
 Return 	:
 ********************************************************************/
TOsStatus osTimerDelete(TOsTimerId xTimerId)
{
	if (local_caOs_FindTimerObject(xTimerId) == OS_ERROR)
	{
		return OS_ERROR;
	}

	WAIT_FOR_SEM(s_ulTimerObjSemId)
	if (xTimerId->eTimerState == eTimer_Running)
	{
		RELEASE_SEM(s_ulTimerObjSemId)
		return OS_ERROR;
	}
	RELEASE_SEM(s_ulTimerObjSemId)

	if (local_caOs_FreeTimerObject(xTimerId) == OS_NO_ERROR)
	{
		return OS_NO_ERROR;
	}
	else
	{
		HxLOG_Error("Could not free timer object(ID 0x%x)...\n", (unsigned int)xTimerId);
		return OS_ERROR;
	}
}


/********************************************************************
 Function	: osMemoryAllocate
 Description:
 Input		:
 Output 	:
 Return 	:
 ********************************************************************/
void* osMemoryAllocate(size_t xSize)
{
	void *pAddr;

	if (xSize == 0)
	{
		HxLOG_Error("error in allocating memory(size %d)\n", xSize);
		return NULL;
	}

	if (xSize < 32)
	{
		xSize = 32;
	}

	pAddr = (void *)OxCAS_Malloc((int)xSize);
	if (pAddr == NULL)
	{
		HxLOG_Error("error in allocating memory(size %d)\n", xSize);
	}

	return pAddr;
}


/********************************************************************
 Function	: osMemoryFree
 Description:
 Input		:
 Output 	:
 Return 	:
 ********************************************************************/
void osMemoryFree(  void *pxMemBlock)
{
	if (pxMemBlock != NULL)
	{
		OxCAS_Free(pxMemBlock);
	}
	else
	{
		HxLOG_Error("error in freeing memory (NULL)\n");
	}
}


/********************************************************************
 Function	: osAbort
 Description:
 Input		:
 Output 	:
 Return 	:
 ********************************************************************/
void osAbort(const TChar *pxString, const TChar *pxFile, const int xLine)
{
	HxLOG_Print("%s file(%s) line(%d)\n\n", pxString, pxFile, xLine);

	// Edited by Hyun Chin
	// CAS가 Reboot 시킨 경우, Loader의 AV가 가려져서는 안된다.
	svc_cas_DevPortSetColdBootPanelCondition(TRUE, TRUE);
	VK_TASK_Sleep(100);

	svc_cas_DevPortPowerReboot();
}

#if defined(_CA_OS_CAPTURE_OS_PRINT_)
void filelogTest(HCHAR *ps)
{
	static FILE *pF = NULL;

	if(pF == NULL) {
#if defined(CONFIG_USES_NFSROOT)
		pF = fopen("/var/lib/humaxtv/osPrint.txt", "w");
#else
		pF = fopen("/media/drive1/osPrint.txt", "w");
#endif
	}

	if(pF != NULL) {
		int n = fwrite(ps, strlen(ps), 1, pF);
		fflush(pF);
	}
}
#endif

/********************************************************************
 Function	: osPrint
 Description:
 Input		:
 Output 	:
 Return 	:
 ********************************************************************/
void osPrint(const TChar *pxString)
{
	if (pxString == NULL)
		return;

#if defined(_CA_OS_CAPTURE_OS_PRINT_)
	filelogTest((HCHAR *)pxString);
#endif

#if defined (CONFIG_MW_CAS_NAGRA_CA_PRODUCT_ICME) && !defined(CONFIG_DEBUG)
	local_caOs_PrintForRelease(pxString);
#elif defined (ENABLE_TERMINAL_FOR_RELEASE) && !defined(CONFIG_DEBUG)
	local_caOs_PrintForRelease(pxString);
#elif defined (CONFIG_MW_CAS_NAGRA_CA_PRODUCT_ALME) && !defined(CONFIG_DEBUG)
	// nothing to do
#else
#if defined(CONFIG_MW_CAS_NAGRA_DALTEST)
	HLIB_CMD_Printf(pxString);
#else	//CONFIG_MW_CAS_NAGRA_DALTEST
#if defined(CONFIG_USES_NFSROOT)
	HxLOG_Message(HxANSI_COLOR_PURPLE("%s"), pxString);
#else
	#if defined(CONFIG_DEBUG)
	s_bCakMessageOn ? Hx_Print(HxANSI_COLOR_PURPLE("%s"), pxString) : ((void)0);
	#endif
#endif	//CONFIG_DEBUG
#endif	//CONFIG_MW_CAS_NAGRA_DALTEST
#endif	//CONFIG_MW_CAS_NAGRA_CA_PRODUCT_ICME
}


/********************************************************************
 Function	: osCharacterGet
 Description:
 Input		:
 Output 	:
 Return 	:
 ********************************************************************/
TChar osCharacterGet(  void)
{
	TUnsignedInt8 ucChar = 0;

#if !defined(CONFIG_DEBUG) && (defined(CONFIG_MW_CAS_NAGRA_CA_PRODUCT_ICME) || defined(ENABLE_TERMINAL_FOR_RELEASE))
	return (TChar) local_caOs_GetCharForRelease();
#elif defined(CONFIG_MW_CAS_NAGRA_DALTEST)
	while(1)
	{
		if(HLIB_CMD_RecvGetChar((char *)&ucChar) == ERR_OK)
			break;
	}

#if defined (CONFIG_PROD_OS_LINUX)
	if(ucChar == 0x0A) /* DALTS Library에서 LF대신 CR을 원하고 있다,  왠지 Linux여서 그런 듯..*/
		ucChar = 0x0D;
#endif
#endif

	return (TChar)ucChar;
}


/********************************************************************
 Function	: osTimeGet
 Description:
 Input		:
 Output 	:
 Return 	:
 ********************************************************************/
TOsTime osTimeGet(  void)
{
	static TOsTime	ulCurSysTime = 0;
	TOsTime 			ulTime = 0;

	ulCurSysTime = VK_TIMER_GetSystemTick();
	if (ulCurSysTime >= s_ulRefSysTime)
	{
		ulTime = ulCurSysTime - s_ulRefSysTime;
	}
	else
	{
		ulTime = 0x7fffffff - s_ulRefSysTime + ulCurSysTime + 1;
	}
	return ulTime;
}


#if (CONFIG_NA_PERSISTENT_MEM == 1)
/********************************************************************
 Function	: osPersistentMemoryGetStorageTable
 Description:
 Input		:
 Output 	:
 Return 	:
 ********************************************************************/
TOsStatus osPersistentMemoryGetStorageTable(TUnsignedInt8 *pxNumStorages, TOsPersistentMemoryStorage **ppxStorageTable)
{
	if (pxNumStorages == NULL || ppxStorageTable == NULL)
	{
		return OS_ERROR;
	}

	*pxNumStorages = OS_PERSISTENT_MEMORY_NUMS;
	*ppxStorageTable = &s_stStorageTable[0];
	return OS_NO_ERROR;
}


/********************************************************************
 Function	: osPersistentMemoryRead
 Description:
 Input		:
 Output 	:
 Return 	:
 ********************************************************************/
TOsStatus osPersistentMemoryRead(TOsPersistentStorageId xStorageId, TSize xFirstByte, TSize xNumBytes, TUnsignedInt8 *pxBuffer)
{
	TSignedInt32 i;
	TSignedInt32 nErrCode = ERR_OK;

	if (pxBuffer == NULL)
	{
		return OS_ERROR;
	}

	//0xFFFFFFFF 인 경우 error 처리를 못해서.
	if ((TSignedInt32)xStorageId >= OS_PERSISTENT_MEMORY_NUMS || (TSignedInt32)xStorageId < 0)
	{
		return OS_ERROR;
	}

	if (xNumBytes <= 0)
	{
		return OS_NO_ERROR;
	}

	switch ((TSignedInt32)xStorageId)
	{
		case STORAGE_ID_0: // eeprom security storage
			nErrCode = CA_NA_OsEepromRead((TSignedInt32)xStorageId, xFirstByte, xNumBytes, pxBuffer);
			for (i=0; i<xNumBytes; i++)
			{
				pxBuffer[i] = ~pxBuffer[i];
			}
			break;

#if (CONFIG_PROTECT_CW == 1)
		case STORAGE_ID_4: // flash CSC
#if defined (CONFIG_MW_CAS_NAGRA_NOCS_3_X)
			nErrCode = CA_NA_OsFlashRead((TSignedInt32)xStorageId, xFirstByte, xNumBytes, pxBuffer);
#else
			nErrCode = CA_NA_OsEepromRead((TSignedInt32)xStorageId, xFirstByte, xNumBytes, pxBuffer);
#endif
			for (i=0; i<xNumBytes; i++)
			{
				pxBuffer[i] = ~pxBuffer[i];
			}
			break;
#endif			
		case STORAGE_ID_1: // flash security storage
		case STORAGE_ID_2: // flash standard storage
#if defined(CONFIG_MW_CAS_NAGRA_PRM)
#if (CONFIG_PROTECT_CW == 0)
		case STORAGE_ID_3:
#else
		case STORAGE_ID_5:
#endif
#endif
			nErrCode = CA_NA_OsFlashRead((TSignedInt32)xStorageId, xFirstByte, xNumBytes, pxBuffer);
			break;

#if (CONFIG_PROTECT_CW == 1)
		case STORAGE_ID_3: // flash CSC Data storage
			{
				static TUnsignedInt8 ucCsc[OS_PERSISTENT_CSC_FLASH_SIZE];
#if (CONFIG_NA_REMOVE_CSCD == 0)
				//nErrCode = svc_cas_DevSysGetField(eCAS_DEV_FLASH_FIELD_CSC_KEY, 0, (void*)ucCsc, OS_PERSISTENT_CSC_FLASH_SIZE);
	#if defined(CONFIG_MW_SYS_NAND_FLASH_ONLY)
				nErrCode = svc_cas_DevPortNandFlashRead(OS_PERSISTENT_CSC_FLASH_ADDRESS, ucCsc, OS_PERSISTENT_CSC_FLASH_SIZE);
	#else
				nErrCode = svc_cas_DevPortFlashRead(OS_PERSISTENT_CSC_FLASH_ADDRESS, ucCsc, OS_PERSISTENT_CSC_FLASH_SIZE);
	#endif
#else
				nErrCode = ERR_OK;
				HxSTD_memset(ucCsc, 0xFF, OS_PERSISTENT_CSC_FLASH_SIZE);
#endif
				HxLOG_Info(" xStorageId:%d, nErrCode:0x%X, xFirstByte:%d, xNumBytes:%d\r\n", xStorageId, nErrCode, xFirstByte, xNumBytes);

				if (nErrCode == ERR_OK)
				{
					nErrCode = ERR_FAIL;
					if ( (xFirstByte + xNumBytes) <= OS_PERSISTENT_CSC_FLASH_SIZE)
					{
						nErrCode = ERR_OK;
						HxSTD_memcpy(pxBuffer, ucCsc+xFirstByte, xNumBytes);

#if defined(CONFIG_DEBUG)
						for( i=xFirstByte; i<xFirstByte+xNumBytes; i++ )
						{
							if (i%20 == 0) HxLOG_Info("\r\n");
							HxLOG_Info(" %02X", ucCsc[i]);
						}
						HxLOG_Info("\r\n");
#endif
					}
				}
			}
			break;
#endif

		default :
			break;
	}

	if (nErrCode == ERR_OK)
	{
		return OS_NO_ERROR;
	}
	else
	{
		return OS_ERROR;
	}
}


/********************************************************************
 Function	: osPersistentMemoryWrite
 Description:
 Input		:
 Output 	:
 Return 	:
 ********************************************************************/
TOsStatus osPersistentMemoryWrite(TOsPersistentStorageId xStorageId, TOsPersistentMemoryFlags xFlags, TSize xFirstByte, TSize xNumBytes, const TUnsignedInt8 *pxBuffer)
{
	TSignedInt32 	i;
	TSignedInt32 	nErrCode = ERR_OK;
	TUnsignedInt8 *pucChar = NULL;

	if (pxBuffer == NULL)
	{
		return OS_ERROR;
	}

	//0xFFFFFFFF 인 경우 error 처리를 못해서.
	if ((TSignedInt32)xStorageId >= OS_PERSISTENT_MEMORY_NUMS || (TSignedInt32)xStorageId < 0)
	{
		return OS_ERROR;
	}

	if (xNumBytes <= 0)
	{
		return OS_NO_ERROR;
	}

	switch ((TSignedInt32)xStorageId)
	{
		case STORAGE_ID_0: // eeprom security storage
			if (xFirstByte + xNumBytes > s_stStorageTable[(TSignedInt32)xStorageId].size)
			{
				return OS_ERROR;
			}
			pucChar = (TUnsignedInt8*)OxCAS_Malloc(xNumBytes);
			if (pucChar == NULL)
			{
				return OS_ERROR;
			}
			for (i=0; i<xNumBytes; i++)
			{
				pucChar[i] = ~pxBuffer[i];
			}
			nErrCode = CA_NA_OsEepromWrite((TSignedInt32)xStorageId, xFirstByte, xNumBytes, pucChar);
			break;
#if (CONFIG_PROTECT_CW == 1)
		case STORAGE_ID_4: // flash CSC
			if (xFirstByte + xNumBytes > s_stStorageTable[(TSignedInt32)xStorageId].size)
			{
				return OS_ERROR;
			}
			pucChar = (TUnsignedInt8*)OxCAS_Malloc(xNumBytes);
			if (pucChar == NULL)
			{
				return OS_ERROR;
			}
			for (i=0; i<xNumBytes; i++)
			{
				pucChar[i] = ~pxBuffer[i];
			}
#if defined (CONFIG_MW_CAS_NAGRA_NOCS_3_X)
			/*
			*	Currently, the CSC size has been increased since the NOCS 3.X version. 
			*	So, It's necessary to move the changable CSC data space from NVRAM to Flash.
			*/
			nErrCode = CA_NA_OsFlashWrite((TSignedInt32)xStorageId, xFirstByte, xNumBytes, pucChar);
#else
			nErrCode = CA_NA_OsEepromWrite((TSignedInt32)xStorageId, xFirstByte, xNumBytes, pucChar);
#endif
			break;
#endif

		case STORAGE_ID_1: // flash security storage
		case STORAGE_ID_2: // flash standard storage
#if defined(CONFIG_MW_CAS_NAGRA_PRM)
#if (CONFIG_PROTECT_CW == 0)
		case STORAGE_ID_3:
#else
		case STORAGE_ID_5:
#endif
#endif
			pucChar = (TUnsignedInt8*)OxCAS_Malloc(xNumBytes);
			if (pucChar == NULL)
			{
				return OS_ERROR;
			}
			HxSTD_memset(pucChar, 0x00, xNumBytes);
			HxSTD_memcpy(pucChar, pxBuffer, xNumBytes);
			nErrCode = CA_NA_OsFlashWrite((TSignedInt32)xStorageId, xFirstByte, xNumBytes, pucChar);
			break;

#if (CONFIG_PROTECT_CW == 1)
		case STORAGE_ID_3: // flash CSC Data storage, only read
			nErrCode = ERR_FAIL;
			break;
#endif

		default:
			break;
	}

	if (pucChar != NULL)
	{
		OxCAS_Free(pucChar);
	}

	if (nErrCode == ERR_OK)
	{
		return OS_NO_ERROR;
	}
	else
	{
		return OS_ERROR;
	}
}


/********************************************************************
 Function	: osPersistentMemoryGetStorageUsage
 Description:
 Input		:
 Output 	:
 Return 	:
 ********************************************************************/
TOsStatus osPersistentMemoryGetStorageUsage(TOsPersistentStorageId xStorageId, TOsPersistentMemoryUsage* pxUsage)
{
	if (pxUsage == NULL)
		return OS_ERROR;

	//0xFFFFFFFF 인 경우 error 처리를 못해서.
	if ((TSignedInt32)xStorageId >= OS_PERSISTENT_MEMORY_NUMS || (TSignedInt32)xStorageId < 0)
	{
		return OS_ERROR;
	}

	HxLOG_Print("osPersistentMemoryGetStorageUsage : xStorageId = %d\n", xStorageId);

	switch((TSignedInt32)xStorageId)
	{
		case STORAGE_ID_0: // eeprom security storage
			*pxUsage = OS_PERSISTENT_MEMORY_USAGE_SECURITY;
			break;
		case STORAGE_ID_1: // flash security storage
			*pxUsage = OS_PERSISTENT_MEMORY_USAGE_SECURITY;
			break;
		case STORAGE_ID_2: // flash standard storage
			*pxUsage = OS_PERSISTENT_MEMORY_USAGE_STANDARD;
			break;
#if (CONFIG_PROTECT_CW == 1)
		case STORAGE_ID_3: // flash CSC Data storage
			*pxUsage = OS_PERSISTENT_MEMORY_USAGE_CSC;
			break;
		case STORAGE_ID_4: // flash CSC Data storage
			*pxUsage = OS_PERSISTENT_MEMORY_USAGE_CSC;
			break;
#if defined(CONFIG_MW_CAS_NAGRA_PRM)
		case STORAGE_ID_5: // flash DVL Data storage
			*pxUsage = OS_PERSISTENT_MEMORY_USAGE_DVL;
			break;
#endif
#else
#if defined(CONFIG_MW_CAS_NAGRA_PRM)
		case STORAGE_ID_3: // flash DVL Data storage
			*pxUsage = OS_PERSISTENT_MEMORY_USAGE_DVL;
			break;
#endif
#endif
	}

	return OS_NO_ERROR;
}
#endif /*CONFIG_NA_PERSISTENT_MEM*/


/**************************************************************************************************/
#define ________CA_OS_Externel_Functions_Body________________________________________________
/**************************************************************************************************/
HERROR CA_NA_OsInitialization(void)
{
	TSignedInt32		nErrCode;
	TUnsignedInt32 	ulSmid;

#if !defined(CONFIG_DEBUG) && (defined(CONFIG_MW_CAS_NAGRA_CA_PRODUCT_ICME) || defined(ENABLE_TERMINAL_FOR_RELEASE))
	local_CaOs_InitTerminalForRelease();
#elif defined(CONFIG_MW_CAS_NAGRA_DALTEST)
	HLIB_CMD_InitGetChar();
#endif

	nErrCode = VK_SEM_Create((unsigned long*)&ulSmid, "ca_os_task", VK_SUSPENDTYPE_PRIORITY);
	if (nErrCode != ERR_OK)
	{
		HxLOG_Error("VK_SEM_Create() return error\n");
		return ERR_FAIL;
	}
	else
	{
		s_ulTaskObjSemId = ulSmid;
	}

	nErrCode = VK_SEM_Create((unsigned long*)&ulSmid, "ca_os_sem", VK_SUSPENDTYPE_PRIORITY);
	if (nErrCode != ERR_OK)
	{
		HxLOG_Error("VK_SEM_Create() return error\n");
		return ERR_FAIL;
	}
	else
	{
		s_ulSemaphoreObjSemId = ulSmid;
	}

	nErrCode = VK_SEM_Create((unsigned long*)&ulSmid, "ca_os_queue", VK_SUSPENDTYPE_PRIORITY);
	if (nErrCode != ERR_OK)
	{
		HxLOG_Error("VK_SEM_Create() return error\n");
		return ERR_FAIL;
	}
	else
	{
		s_ulQueueObjSemId = ulSmid;
	}

	nErrCode = VK_SEM_Create((unsigned long*)&ulSmid, "ca_os_timer", VK_SUSPENDTYPE_PRIORITY);
	if (nErrCode != ERR_OK)
	{
		HxLOG_Error("VK_SEM_Create() return error\n");
		return ERR_FAIL;
	}
	else
	{
		s_ulTimerObjSemId = ulSmid;
	}

	s_pstTaskList = NULL;
	s_pstSemaphoreList = NULL;
	s_pstQueueList = NULL;
	s_pstTimerList = NULL;

#if (CONFIG_NA_PERSISTENT_MEM == 1)
	s_stStorageTable[0].storageId = (TOsPersistentStorageId)STORAGE_ID_0;
	s_stStorageTable[0].type = OS_PERSISTENT_MEMORY_EEPROM;
	s_stStorageTable[0].flags = 0x00;
	s_stStorageTable[0].size = OS_PERSISTENT_SECURITY_EEPROM_SIZE;

	s_stStorageTable[1].storageId = (TOsPersistentStorageId)STORAGE_ID_1;
	s_stStorageTable[1].type = OS_PERSISTENT_MEMORY_FLASH;
	s_stStorageTable[1].flags = 0x00;
	s_stStorageTable[1].size = OS_PERSISTENT_SECURITY_FLASH_SIZE;

	s_stStorageTable[2].storageId = (TOsPersistentStorageId)STORAGE_ID_2;
	s_stStorageTable[2].type = OS_PERSISTENT_MEMORY_FLASH;
	s_stStorageTable[2].flags = 0x00;
	s_stStorageTable[2].size = OS_PERSISTENT_STANDARD_FLASH_SIZE;

#if (CONFIG_PROTECT_CW == 1)
	s_stStorageTable[3].storageId = (TOsPersistentStorageId)STORAGE_ID_3;
	s_stStorageTable[3].type = OS_PERSISTENT_MEMORY_OTP;
	s_stStorageTable[3].flags = 0x00;
	s_stStorageTable[3].size = OS_PERSISTENT_CSC_FLASH_SIZE;

	s_stStorageTable[4].storageId = (TOsPersistentStorageId)STORAGE_ID_4;
	s_stStorageTable[4].type = OS_PERSISTENT_MEMORY_NVRAM;
	s_stStorageTable[4].flags = 0x00;
	s_stStorageTable[4].size = OS_PERSISTENT_CSC_FLASH_SIZE;

#if defined(CONFIG_MW_CAS_NAGRA_PRM)
	s_stStorageTable[5].storageId = (TOsPersistentStorageId)STORAGE_ID_5;
	s_stStorageTable[5].type = OS_PERSISTENT_MEMORY_FLASH;
	s_stStorageTable[5].flags = 0x00;
	s_stStorageTable[5].size = OS_PERSISTENT_DVL_FLASH_SIZE;
#endif
#else
#if defined(CONFIG_MW_CAS_NAGRA_PRM)
	s_stStorageTable[3].storageId = (TOsPersistentStorageId)STORAGE_ID_3;
	s_stStorageTable[3].type = OS_PERSISTENT_MEMORY_FLASH;
	s_stStorageTable[3].flags = 0x00;
	s_stStorageTable[3].size = OS_PERSISTENT_DVL_FLASH_SIZE;
#endif
#endif
#endif

	s_ulRefSysTime = VK_TIMER_GetSystemTick();

	nErrCode = CA_NA_OsCheckEepromVersion();
	if (nErrCode != ERR_OK)
	{
		HxLOG_Error("Failed to convert NVRAM format to current version\n");
		return ERR_FAIL;
	}
	
	return ERR_OK;
}

/**************************************************************************************************/
#define ________CA_OS_Debug_Functions_Body________________________________________________
/**************************************************************************************************/
#if defined(CONFIG_DEBUG)
void CA_NA_OsCakMessage(void);
void CA_NA_OsCakMessage()
{
	s_bCakMessageOn = !s_bCakMessageOn;
	Hx_Print(HxANSI_COLOR_GREEN("[%s - %s]\n"), __FUNCTION__, s_bCakMessageOn ? "ON" : "OFF");
}
#endif


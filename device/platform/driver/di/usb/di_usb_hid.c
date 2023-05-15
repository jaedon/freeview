/**
 * di_usb_hid.c
*/

/**
 * @defgroup		DI_USB_HID_MODULE DI_USB_HID_MODULE : extra key moudle device interface
 * @author		Moonhoen Lee
 * @note			Extra Key Event module API
 * @brief			Define extra key event
 * @file 			di_exkey.c -> di_usb_hid.c
*/

/*
 * Comment : 2009.03.25 Moonhoen Lee
 * since DEBUG_MODULE is undeclared, we temporarily use raw VK_printf function instead of
 * predefined di's debugging function(ex. PrintError()).
 */

/*******************************************************************/
/**************************** Header Files *************************/
/*******************************************************************/
/* include for standard headers */
#if defined(CONFIG_VK_STDLIB)
#else
#include 	<stdio.h>
#include 	<stdlib.h>
#include 	<string.h>
#endif
#include <unistd.h>
#include <ctype.h>
#include <fcntl.h> /* for O_RDWR */
#include <sys/fcntl.h>
#include <sys/ioctl.h> /* for ioctl */
#include <sys/time.h>
#include <sys/types.h>
#include <errno.h>
#include <pthread.h>

/* include common utils first! */
#if defined(CONFIG_DI_SHARED_LIB) || defined(CONFIG_SHARED_COMPILE)
//NULL
#else
#include "linden_trace.h"
#endif

#include "vkernel.h"
#include "taskdef.h"

#include "drv_usb.h"
/* include di headers */
#include "di_usb_hid.h"

/*******************************************************************/
/****************************** define *****************************/
/*******************************************************************/
#if defined(CONFIG_DI_SHARED_LIB) || defined(CONFIG_SHARED_COMPILE)
//#define DI_USB_HID_DEBUG
#ifdef DI_USB_HID_DEBUG
#define PrintDebug(fmt, ...) 		VK_DBG_Print (fmt, ## __VA_ARGS__)
#define PrintError(fmt, ...)    	VK_DBG_Print (fmt, ## __VA_ARGS__)
#define PrintEnter(...)				VK_printf("(+)%s\n", __FUNCTION__)
#define PrintExit(...)				VK_printf("(-)%s\n", __FUNCTION__)
#else
#define PrintDebug(...)
#define PrintError(...)				VK_DBG_Print (__VA_ARGS__)
#define PrintEnter(...)
#define PrintExit(...)
#endif
#else
/* Since there is not enough space for the input module, we use DI_KEY debug module temporarily */
#define DEBUG_MODULE	TRACE_MODULE_DI_KEY

#ifndef DEBUG_MODULE
#ifdef DI_USB_HID_DEBUG
#define PrintDebug(fmt, ...) 		VK_DBG_Print (fmt, ## __VA_ARGS__)
#define PrintError(fmt, ...)    	VK_DBG_Print (fmt, ## __VA_ARGS__)
#define PrintEnter()				VK_printf("(+)%s\n", __FUNCTION__)
#define PrintExit()				VK_printf("(-)%s\n", __FUNCTION__)
#else
#define PrintDebug(...)
#define PrintError(...)		VK_DBG_Print (__VA_ARGS__)
#define PrintEnter()
#define PrintExit()
#endif
#endif
#endif


/** max device count of event input module */
#define DI_USB_HID_MAX_DEVICE_CNT							(8)	/* should be less than 10 */
/** event input proc file system device path */
#define DI_USB_HID_PROCFS_DEVICE_PATH						("/proc/bus/input/devices")
/** event input proc file system handler path */
#define DI_USB_HID_PROCFS_HANDLER_PATH						("/proc/bus/input/handlers")
/** prerfix of the path of the input devices */
#define DI_USB_HID_DEVICE_PATH_PREFIX						("/dev/input/event")

/** event input monitor's select timeout(msec) */
#define DI_USB_HID_MONITOR_SELECT_TIMEOUT_MSEC			(500)
/** event input monitor's select timeout(sec) */
#define DI_USB_HID_MONITOR_SELECT_TIMEOUT_SEC				(0)

/** checking interval of not opend event input devices */
#define DI_USB_HID_CHECK_NOT_OPENED_DEVICE_INTERVAL		(10)

/** buffer size for parsing procfs of event input */
#define DI_USB_HID_BUF_SIZE_OF_PARSING_PROCFS				(256)

/** max size of handler name */
#define DI_USB_HID_MAX_SIZE_OF_PROCFS_HANDLER_NAME		(10)

enum
{
	/** usb keyboard */
	DI_USB_HID_HANDLER_USB_KEYBOARD = 0,
	/** usb mouse */
	DI_USB_HID_HANDLER_USB_MOUSE,
	/** usb joystick */
	DI_USB_HID_HANDLER_USB_JOYSTICK,
	/** num of handler */
	DI_USB_HID_HANDLER_NUM
};

/** get semaphore macro */
#define	GET_USB_HID_SEMA 		VK_SEM_Get(s_ulHIDSema)
/** release semaphore macro */
#define	REL_USB_HID_SEMA 		VK_SEM_Release(s_ulHIDSema)
/** not used parameter macro */
#define DI_USB_HID_PARAM_UNUSED(x) ((void)x)

/*******************************************************************/
/****************************** typedef ****************************/
/*******************************************************************/

/*******************************************************************/
/************************ global variables *************************/
/*******************************************************************/


/*******************************************************************/
/************************ static variables *************************/
/*******************************************************************/
static pfnDI_USB_HID_Notify 		s_fnHIDUSBMouseCallback = NULL;
static pfnDI_USB_HID_Notify 		s_fnHIDUSBKeyboardCallback = NULL;
static pfnDI_USB_HID_Notify 		s_fnHIDUSBJoystickCallback = NULL;
static HUINT32 				s_ulHIDTaskId;
static unsigned long 			s_ulHIDSema;

/** event input FDs */
static int s_aunHIDFDs[DI_USB_HID_MAX_DEVICE_CNT];

/** a flag which indicates whether the event input monitor is running or not */
static int s_nIsHIDMonitorRunning = 0;

/** handler name */
static char s_szHandlerName[DI_USB_HID_HANDLER_NUM][DI_USB_HID_MAX_SIZE_OF_PROCFS_HANDLER_NAME] = {"kbd", "mouse", "js"};

/** event node's prefix */
static char s_szEventNodeName[] = {"event"};


/*******************************************************************/
/*********************** Function Prototypes ***********************/
/*******************************************************************/

/**
 *  usb mouse event monitor task function
 *
 * @param[in] tInputEvent input event data
 */
static void DRV_USB_HID_DispatchUSBMouseEvent(const DI_USB_HID_DATA tInputEvent)
{
	PrintEnter();

	if(s_fnHIDUSBMouseCallback == NULL)
	{
		PrintError("<USB Mouse> type=%x code=%x value=%x\n",
				tInputEvent.type,tInputEvent.code,tInputEvent.value);
	}
	else
	{
		s_fnHIDUSBMouseCallback(DI_USB_HID_DEVICE_USBMOUSE, tInputEvent);
	}
	PrintExit();
}

/**
 *  usb keyboard event monitor task function
 *
 * @param[in] tInputEvent input event data
 */
static void DRV_USB_HID_DispatchUSBKbdEvent(const DI_USB_HID_DATA tInputEvent)
{
	PrintEnter();
	if(s_fnHIDUSBKeyboardCallback == NULL)
	{
		PrintError("<USB Keyboard> type=%x code=%x value=%x\n",
				tInputEvent.type,tInputEvent.code,tInputEvent.value);
	}
	else
	{
		s_fnHIDUSBKeyboardCallback(DI_USB_HID_DEVICE_USBKEYBOARD, tInputEvent);
	}
	PrintExit();
}

/**
 *  joystick event monitor task function
 *
 * @param[in] tInputEvent input event data
 */
static void DRV_USB_HID_DispatchUSBJoystickEvent(const DI_USB_HID_DATA tInputEvent)
{
	PrintEnter();
	if(s_fnHIDUSBJoystickCallback == NULL)
	{
		PrintError("<USB Keyboard> type=%x code=%x value=%x\n",
				tInputEvent.type,tInputEvent.code,tInputEvent.value);
	}
	else
	{
		s_fnHIDUSBJoystickCallback(DI_USB_HID_DEVICE_USBJOYSTICK, tInputEvent);
	}
	PrintExit();
}

/**
 *  event monitor task function
 *
 * @param[in] nFD FD
 * @param[in] nType event device type
 * @return 0 if success
 */
static HINT32 DRV_USB_HID_DispatchEvent(
	const HINT32 nFD,
	const HINT32 nType)
{
	DI_USB_HID_DATA 	tInputEvent;
	HINT32			nRead = 0;

	PrintEnter();

	nRead = read(nFD, &tInputEvent, sizeof(DI_USB_HID_DATA));
	if(nRead <= 0)
	{
		/* maybe input device is disconnected. */
		PrintError("can not read data from fd[%d].\n", nFD);
		return -1;
	}
	/* else { continue } */

	switch(nType)
	{
		case DI_USB_HID_HANDLER_USB_KEYBOARD:
			DRV_USB_HID_DispatchUSBKbdEvent(tInputEvent);
			break;
		case DI_USB_HID_HANDLER_USB_MOUSE:
			DRV_USB_HID_DispatchUSBMouseEvent(tInputEvent);
			break;
		case DI_USB_HID_HANDLER_USB_JOYSTICK:
			DRV_USB_HID_DispatchUSBJoystickEvent(tInputEvent);
			break;
		default:
			/* unknown device event */
			break;
	}

	PrintExit();

	return 0;
}

/**
 *  get event number from buffer
 *
 * @param[in] szBuf buffer
 * @param[in] nBufferSize buffer size
 * @return 0 if success
 */
static HINT32 DRV_USB_HID_GetEventNodeNum(
	const HINT8 *szBuf,
	const HINT32 nBufSize)
{
	HINT32 		i = 0;
	HINT32 		j = 0;
	HINT32 		nEventNodeLegnth;

	PrintEnter();

	nEventNodeLegnth = VK_strlen(s_szEventNodeName);

	for(i = 0; i < nBufSize; ++i)
	{
		if(szBuf[i] == s_szEventNodeName[j])
		{
			j++;
			if(j == nEventNodeLegnth)
			{
				if((i < nBufSize - 1) &&
					(szBuf[i+1] >= '0') &&
					(szBuf[i+1] <= '9'))
				{
					PrintExit();
					return (int)szBuf[i+1] - (int)'0';
				}
				else
				{
					j = 0;
				}
			}
			/* else { continue } */
		}
		else
		{
			j = 0;
		}
	}

	return -1;
}

/**
 *  find hander name from buffer
 *
 * @param[in] szBuf buffer
 * @param[in] nBufferSize buffer size
 * @return 0 if success
 */
static HINT32 DRV_USB_HID_FindHandlerName(
	const HINT8 *szBuf,
	const HINT32 nBufSize)
{
	HINT32 		i = 0;

	PrintEnter();

	for(i = 0; i < DI_USB_HID_HANDLER_NUM; ++i)
	{
		int 		j = 0;
		int 		k = 0;
		int 		nPatternLegnth = 0;

		nPatternLegnth = VK_strlen(s_szHandlerName[i]);

		for(j = 0; j < nBufSize; j++)
		{
			if(szBuf[j] == s_szHandlerName[i][k])
			{
				k++;
				if(k == nPatternLegnth)
				{
					PrintExit();
					return i;
				}
				/* else { continue } */
			}
			else
			{
				k = 0;
			}
		}
	}
	return -1;
}

/**
 *  find hander name from buffer
 *
 * @param[in] szBuf buffer
 * @param[in] nBufferSize buffer size
 * @param[out] pnHandlerType handler type (ex. USB Mouse)
 * @param[out] pnEventNodeNum event node's num (ex. event3)
 * @return 0 if success
 */
static HINT32 DRV_USB_HID_ParseEventInfo(
	const HINT8 *szBuf,
	const HINT32 nBufSize,
	HINT32 *pnHandlerType,
	HINT32 *pnEventNodeNum)
{
	/*
	* Note :
	*   pattern should be "H: Handlers=kbd event1"
	*/
	PrintEnter();

	/* 1. check parameters */
	if(nBufSize <= 0)
	{
		return -1;
	}
	/* else {continue } */

	if(szBuf == NULL)
	{
		return -1;
	}
	/* else { continue } */

	/* 2. check whether szBuf has handler information or not */
	if(szBuf[0] != 'H')
	{
		return -1;
	}
	/* else { continue } */

	/* 3. get handler type & node num */
	(*pnHandlerType) = DRV_USB_HID_FindHandlerName(szBuf, nBufSize);
	(*pnEventNodeNum) = DRV_USB_HID_GetEventNodeNum(szBuf, nBufSize);

	PrintExit();
	return 0;
}

/**
 *  read line of the fd
 *
 * @param[in] fd file descriptor
 * @param[in] strBuf buffer
 * @param[in] nMaxLength max length of the strBuf
 * @return read count
 */
static HINT32 DRV_USB_HID_ReadLine(
	const HINT32 fd,
	HINT8 *strBuf,
	const HINT32 nMaxLength)
{
	int count = 0;
	int i = 0;
	int nRet = 0;
	char c;

	PrintEnter();

	/* 1. check parameters */
	if(nMaxLength <= 0)
	{
		return 0;
	}
	/* else { continue } */

	if(strBuf == NULL)
	{
		return 0;
	}
	/* else { continue } */

	/* 2. get characters until '\n' */
	for(i = 0; i < nMaxLength; ++i)
	{
		nRet = read(fd, &c, sizeof(char));
		if(nRet <= 0)
		{
			return -1;
		}
		else if (c == '\n')
		{
			break;
		}
		else
		{
			strBuf[i] = c;
			count++;
		}
	}

	PrintExit();

	return count;
}

/**
 *  get device info from proc file system
 *
 * @param[out] aunFDSet FD sets
 * @param[in] nFDSetSize max size of FD sets
 */
static void DRV_USB_HID_GetDevInfoFromProcFS(
	HINT32 *aunFDSet,
	const HINT32 nFDSetSize)
{
	int 			fd = 0;
	int 			i = 0;

	PrintEnter();
	/* 0. needs some time update proc file system */
	VK_TASK_Sleep(100);

	/* 1. open */
	fd = open(DI_USB_HID_PROCFS_DEVICE_PATH, O_RDONLY);
	if(fd < 0)
	{
		PrintError("can not open %s\n", DI_USB_HID_PROCFS_DEVICE_PATH);
		return;
	}
	/* else { continue } */

	/* 2. initialzie aunFDSet */
	for(i = 0; i < nFDSetSize; ++i)
	{
		aunFDSet[i] = -1;
	}

	/* 3. save current procfs info into aunFDset */
	while(1)
	{
		HINT8 		szBuf[DI_USB_HID_BUF_SIZE_OF_PARSING_PROCFS];
		HINT32 		nHandlerType = DI_USB_HID_HANDLER_USB_KEYBOARD;
		HINT32 		nEventNodeNum = 0;
		HINT32 		nRead = 0;
		HINT32		nRet = 0;

		/* 3.1 read procfs */
		nRead = DRV_USB_HID_ReadLine(fd, szBuf, DI_USB_HID_BUF_SIZE_OF_PARSING_PROCFS);
		if(nRead == -1)
		{
			/* eof or unknown internal error */
			break;
		}
		else if(nRead == 0)	/* '\n' comes */
		{
			continue;
		}
		/* else { continue } */

		/* 3.2 parse event info */
		nRet = DRV_USB_HID_ParseEventInfo(szBuf, nRead, &nHandlerType, &nEventNodeNum);

		if((nRet != 0) ||
			(nEventNodeNum < 0) ||
			(nEventNodeNum >= DI_USB_HID_MAX_DEVICE_CNT) ||
			(nFDSetSize <= nEventNodeNum))
		{
			/* range error */
			continue;
		}
		else if((nHandlerType < 0) ||
			(nHandlerType >= DI_USB_HID_HANDLER_NUM))
		{
			/* unknown event handler */
			aunFDSet[nEventNodeNum] = -1;
		}
		else
		{
			/* success case */
			aunFDSet[nEventNodeNum] = nHandlerType;
		}
	}

	{
		HINT32 loopCnt = 0;
		PrintDebug("<debug> : aunFDSet\n");
		for(loopCnt = 0; loopCnt < DI_USB_HID_MAX_DEVICE_CNT; ++loopCnt)
		{
			PrintDebug("[%d]:%d    ", loopCnt, aunFDSet[loopCnt]);
		}
		PrintDebug("\n");
	}
	PrintExit();
	/* 4. close */
	(void)close(fd);
}

/**
 *  event input monitor task function
 *
 * @param[in] pvParm not used parameter.
 */
static void P_USB_HID_EventMonitorTask(
	void *pvParm)
{
	HINT32 				nFdmax = 0;
	HINT32 				i = 0;
	HINT32 				nCheckIntervalOfNotOpenedDevice = 0;
	HINT32 				nIsChangedInputEventSet = 0;
	HINT32 				nIsNewDataArrived = 0;
	fd_set				fdReadFDSet;
	HINT32				nRet = 0;
	struct timeval		tTimeval;
	HINT32 				fd = -1;
	HINT32				aunAvailableEventInputFDs[DI_USB_HID_MAX_DEVICE_CNT];
	HINT32				aunHandlersOfEventInputFD[DI_USB_HID_MAX_DEVICE_CNT];

	/* pvParam not used */
	DI_USB_HID_PARAM_UNUSED(pvParm);

	while(1)
	{
		GET_USB_HID_SEMA;
		if(!s_nIsHIDMonitorRunning)
		{
			/* exit thread loop */
			REL_USB_HID_SEMA;
			VK_TASK_Sleep(1000);
			break;
		}
		/* else { continue } */
		REL_USB_HID_SEMA;

		/* 0. initialize data */
		(void*)VK_memset(aunAvailableEventInputFDs, 0x0, sizeof(int) * DI_USB_HID_MAX_DEVICE_CNT);
		nIsNewDataArrived = 0;

		/* 1. scan all input devices which are not opened */
		if(nCheckIntervalOfNotOpenedDevice == 0)
		{
			/* 1-a. find file descriptors which have not been opened */
			nIsChangedInputEventSet = 0;
			for(i = 0; i < DI_USB_HID_MAX_DEVICE_CNT; ++i)
			{
				HINT8 		szDeviceName[256];
				if(s_aunHIDFDs[i] > 0)
				{
					continue;
				}
				/* else { continue } */

				(void)VK_snprintf((char*)szDeviceName, 255, "%s%d", DI_USB_HID_DEVICE_PATH_PREFIX, i);

				fd = open((char*)szDeviceName, O_RDONLY);
				/*it's not an error to get 0 as a return value (although it's almost impossible), so this case shall be considered.*/
				if(fd >= 0)
				{
					s_aunHIDFDs[i] = fd;
					PrintDebug( "[P_USB_HID_EventMonitorTask] szDeviceName= %s opened\n",szDeviceName);
					nIsChangedInputEventSet= 1;
				}
				/* else { continue } */
			}

			/* 1-b. find file descriptors which have not been opened */
			if(nIsChangedInputEventSet)
			{
				/* fill aunHandlersOfEventInputFD */
				DRV_USB_HID_GetDevInfoFromProcFS(aunHandlersOfEventInputFD, DI_USB_HID_MAX_DEVICE_CNT);
			}
			/* else { continue } */

			nIsChangedInputEventSet = 0;
		}
		/* else { continue } */

		/* 2. count up nCheckIntervalOfNotOpenedDevice */
		nCheckIntervalOfNotOpenedDevice =
			(nCheckIntervalOfNotOpenedDevice + 1) % DI_USB_HID_CHECK_NOT_OPENED_DEVICE_INTERVAL;

		/* 3. prepare fdsets */
		nFdmax = 0;
		FD_ZERO (&fdReadFDSet);
		for(i = 0; i < DI_USB_HID_MAX_DEVICE_CNT; ++i)
		{
			if(s_aunHIDFDs[i] > 0)
			{
				FD_SET (s_aunHIDFDs[i], &fdReadFDSet);
				if(nFdmax < s_aunHIDFDs[i])
				{
					nFdmax = s_aunHIDFDs[i];
				}
			}
			/* else { continue  } */
		}
		nFdmax += 1;

		/* 4. do select() */
		tTimeval.tv_sec = DI_USB_HID_MONITOR_SELECT_TIMEOUT_SEC;
		tTimeval.tv_usec = DI_USB_HID_MONITOR_SELECT_TIMEOUT_MSEC*1000;

		nRet = select(nFdmax, &fdReadFDSet, NULL, NULL, &tTimeval);

		if(nRet <= 0)
		{
			continue;
		}
		/* else { continue } */

		/* 5. get FDs which read data */
		for(i = 0; i < DI_USB_HID_MAX_DEVICE_CNT; ++i)
		{
			if(s_aunHIDFDs[i] > 0)
			{
				if (FD_ISSET(s_aunHIDFDs[i], &fdReadFDSet))
				{
					aunAvailableEventInputFDs[i] += 1;
					nIsNewDataArrived = 1;
				}
				/* else { continue } */
			}
			/* else { continue } */
		}

		/* 6. dispatch data */
		if(nIsNewDataArrived)
		{
			for(i = 0; i< DI_USB_HID_MAX_DEVICE_CNT; ++i)
			{
				if(aunAvailableEventInputFDs[i])
				{
					nRet = DRV_USB_HID_DispatchEvent(s_aunHIDFDs[i], aunHandlersOfEventInputFD[i]);
					if(nRet < 0)
					{
						/* close FD */
						(void)close(s_aunHIDFDs[i]);
						/* set not available FD */
						s_aunHIDFDs[i] = -1;
						/* set unknown handler */
						aunHandlersOfEventInputFD[i] = -1;
						/* in order to check device forcely */
						nIsChangedInputEventSet = 1;
					}
					/* else { continue } */
				}
				/* else { continue } */
			}
		}
		/* else { continue } */

		/* 7. update FDsets when some device is disconnected */
		if(nIsChangedInputEventSet)
		{
			DRV_USB_HID_GetDevInfoFromProcFS(aunHandlersOfEventInputFD, DI_USB_HID_MAX_DEVICE_CNT);
		}
		/* else { continue } */
//		VK_TASK_Sleep(100);
	}

	/* close all fds */
	for(i = 0; i < DI_USB_HID_MAX_DEVICE_CNT; ++i)
	{
		if(s_aunHIDFDs[i] > 0)
		{
			(void)close(s_aunHIDFDs[i]);
		}
		/* else { continue } */
	}
}

/**
 *  initilalize HID module
 *
 * @return DRV_OK if success
 */
DRV_Error DRV_USB_HID_Init(void)
{
	HINT32 			uVKReturnCode = VK_OK;
	HINT32			i = 0;
	HINT32			nRet = 0;

	PrintEnter();

	/*  1. initialize s_aunHIDFDs  */
	for(i = 0; i < DI_USB_HID_MAX_DEVICE_CNT; ++i)
	{
		s_aunHIDFDs[i] = -1;	/* not opened */
	}

	/* 2. initialzie static variables */
	s_nIsHIDMonitorRunning = 1;

	/* 3. create semaphore for the input module */
	nRet = VK_SEM_Create(&s_ulHIDSema,"input_module_sema", DEFAULT_SUSPEND_TYPE);
	if(nRet != VK_OK)
	{
		PrintError("[DI_USB_HID_Init] VK_SEM_Create Create Error\n");
		return DRV_ERR;
	}

	/* 4. DI_INPUT_MODULE_Task로 태스크 생성 */
	uVKReturnCode = VK_TASK_Create(P_USB_HID_EventMonitorTask,
		61, 2048, "EventMonTask", (void *)0, (unsigned long*)&s_ulHIDTaskId, 0);
	if(uVKReturnCode != VK_OK)
	{
		PrintError( "[DI_USB_HID_Init] VK_TASK_Create error : uVKReturnCode is 0x%x\n", uVKReturnCode);
		return DRV_ERR;
	}

	/* 5. DI_KEY_Task 시작 */
	uVKReturnCode = VK_TASK_Start(s_ulHIDTaskId);
	if(uVKReturnCode != VK_OK)
	{
		PrintError( "[DI_USB_HID_Init] VK_TASK_Start error : uVKReturnCode is 0x%x\n", uVKReturnCode);
		return DRV_ERR;
	}
	PrintExit();

	return DRV_OK;
}

/**
 *  register a callback fucntion for ex-key event
 *
 * @param[in] eDevices devices which are registed to pfnNotify callback function
 * @param[in] pfnNotify callback function
 * @return DI_ERR_OK, if it success. DI_ERR_NO_RESOURCE, if callback function is already registered
 */
DI_ERR_CODE DI_USB_HID_RegisterHIDEventCallback (
	const DI_USB_HID_DEVICE eDevices,
	pfnDI_USB_HID_Notify pfnNotify)
{
	DI_ERR_CODE eRet = DI_ERR_OK;

	/* 1. register USB Keyboard if it is required */
	if(eDevices & DI_USB_HID_DEVICE_USBKEYBOARD)
	{
		if(s_fnHIDUSBKeyboardCallback != NULL)
		{
			PrintDebug( "[DI_USB_HID_RegisterHIDEventCallback] USB Keyboard is already registered.\n");
			eRet = DI_ERR_NO_RESOURCE;
		}
		else
		{
			s_fnHIDUSBKeyboardCallback = pfnNotify;
		}
	}
	/* else { continue } */

	/* 2. register USB Mouse if it is required */
	if(eDevices & DI_USB_HID_DEVICE_USBMOUSE)
	{
		if(s_fnHIDUSBMouseCallback != NULL)
		{
			PrintDebug( "[DI_USB_HID_RegisterHIDEventCallback] USB Mouse is already registered.\n");
			eRet = DI_ERR_NO_RESOURCE;
		}
		else
		{
			s_fnHIDUSBMouseCallback = pfnNotify;
		}
	}
	/* else { continue } */

	/* 3. register USB Joystick if it is required */
	if(eDevices & DI_USB_HID_DEVICE_USBJOYSTICK)
	{
		if(s_fnHIDUSBJoystickCallback != NULL)
		{
			PrintDebug( "[DI_USB_HID_RegisterHIDEventCallback] USB Joystick is already registered.\n");
			eRet = DI_ERR_NO_RESOURCE;
		}
		else
		{
			s_fnHIDUSBJoystickCallback = pfnNotify;
		}
	}
	/* else { continue } */

	return eRet;
}

DI_ERR_CODE DI_USB_HID_Init(void)
{
	DRV_Error err;

	err = DRV_USB_HID_Init();
	if (err != DRV_OK) {
		return DI_ERR_ERROR;
	}

	return DI_ERR_OK;
}

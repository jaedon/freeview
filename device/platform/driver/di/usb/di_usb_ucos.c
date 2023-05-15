/**
 * di_usb.c
*/

/**
 * @defgroup		DI_USB : DI_USB module
 * @author
 * @note			DI_USB APIs
 * @brief			Define DI_USB APIs
 * @file 			di_usb.c
*/

/*******************************************************************
**  Include
********************************************************************/
#if defined(CONFIG_VK_STDLIB)
#else
#include 	<stdio.h>
#include 	<stdlib.h>
#include 	<string.h>
#endif

#include "nexus_types.h"
#include "di_err.h"
#include "drv_err.h"
#include "di_uart.h"
#include "vkernel.h"
#include "taskdef.h"
#include "htype.h"
#include "di_usb.h"
#include "drv_usb.h"

#include "drv_bootloader.h"
#include "humax_bsu.h"

/*******************************************************************
**  Defines
********************************************************************/
//#define USB_DEBUG
#ifdef USB_DEBUG
#define PrintDebug(fmt, ...) 		VK_DBG_Print (fmt, ## __VA_ARGS__)
#define PrintError(fmt, ...)    	VK_DBG_Print (fmt, ## __VA_ARGS__)
#define PrintInfo(...)              VK_DBG_Print (fmt, ## __VA_ARGS__)
#define PrintEnter(...)				VK_printf("(+)%s\n", __FUNCTION__)
#define PrintExit(...)				VK_printf("(-)%s\n", __FUNCTION__)
#else
#define PrintDebug(...)
#define PrintError(...)				DI_UART_Print (__VA_ARGS__)
#define PrintInfo(...)              DI_UART_Print (__VA_ARGS__)
#define PrintEnter(...)
#define PrintExit(...)
#endif

#define MAX_NUM_USB_STORAGE 		8//CONFIG_USB_PORT_CNT


/*******************************************************************
**  Type define
********************************************************************/

/*******************************************************************
**  static variables
********************************************************************/

static pfnDI_USB_NotifyFUNC 		CB_USBNotify = NULL;
static HBOOL						s_bIsUsbUp[MAX_NUM_USB_STORAGE] = {0,};


#define UNUSED(x) (void)(x)

/*******************************************************************
**  Functions
********************************************************************/
static HBOOL P_USB_CheckUsbDisk(HUINT32 ulUsbIndex)
{
	int usbdisk;
	char usbname[12];

	VK_MEM_Memset(usbname, 0, sizeof(usbname));
	VK_snprintf(usbname, sizeof(usbname)-1, "%s%d", "usbdisk", ulUsbIndex);
	usbdisk = BSU_open(usbname);
	if (usbdisk < 0)
	{
		return FALSE;
	}
	else
	{
		BSU_close(usbdisk);
		PrintDebug("ulUsbIndex (%d) Detected\n", ulUsbIndex);
		return TRUE;
	}
}

static HINT32 P_USB_CheckUSB(void)
{
	HINT32 	cnt = 0;
	HINT32	i = 0;
	HBOOL	bIsInsert;
	HCHAR	localDevName[64] = {0, };

	usb_info_t		usbInfo;
	DI_USB_Event_t	eEvent;

	if(CB_USBNotify == NULL)
	{
		PrintError("[%s:%d] CB_USBNotify == NULL\r\n", __FUNCTION__, __LINE__);
		return -1;
	}

	VK_memset(&usbInfo, 0, sizeof(usb_info_t));

	/*
	 * ucos에서는 한번에 1개의 usb를 callback을 통해 알려준다.
	 */
	for(i = 0; i < MAX_NUM_USB_STORAGE; i++)
	{
		bIsInsert = P_USB_CheckUsbDisk(i);
		if(bIsInsert)
		{
			cnt++;
		}

		if(s_bIsUsbUp[i] == bIsInsert)
		{
			continue;
		}

		s_bIsUsbUp[i] = bIsInsert;
		if(bIsInsert)
		{
			eEvent = DI_USB_Event_Insert;
		}
		else
		{
			cnt--;
			eEvent = DI_USB_Event_Extract;
		}
		VK_sprintf(localDevName, "usbdisk%d", (i));

		usbInfo.ePortNum = (DI_USB_PORT_Num_t)(i+1);
		usbInfo.devCount = 1;
		usbInfo.devNameSize = 4;
		usbInfo.devName = (char **)DD_MEM_Alloc( sizeof(char *)*usbInfo.devCount );
		usbInfo.devName[0] = localDevName;

		CB_USBNotify (eEvent, (void *)&usbInfo);

		DD_MEM_Free(usbInfo.devName);
	}

	return cnt;
}


DI_ERR_CODE DI_USB_Check(HBOOL isClear)
{
	if(isClear)
	{
		VK_memset(s_bIsUsbUp, 0, sizeof(HBOOL) * MAX_NUM_USB_STORAGE);
	}
	P_USB_CheckUSB();

	return ERR_OK;
}

DRV_Error DRV_USB_Init(void)
{
	/*
	 * not implement
	 */

	return 0;
}

/*---------------------------------------------------------------------------
** DI API Lists
**-------------------------------------------------------------------------*/
void DI_USB_RegisterCallback(pfnDI_USB_NotifyFUNC pfnNotify)
{
	CB_USBNotify  = pfnNotify;
}

DI_ERR_CODE DI_USB_Stop(void)
{
	/*
	 * not implement
	 */

	return 0;
}

DI_ERR_CODE DI_USB_PWR_Ctrl(DI_USB_PWR_t OnOff) /* PORT */
{
	/*
	 * not implement
	 */
	UNUSED(OnOff);
	return 0;
}

DI_ERR_CODE DI_USB_HOST_CONTROLLER_PWR_Ctrl(DI_USB_PWR_t OnOff) /* HOST CONTROLLER */
{
	/*
	 * not implement
	 */
	UNUSED(OnOff);
	return 0;
}

/*******************************************************************************
* function : DI_USB_GetUsbHostCount
* description :
* input :
* output :
* return :
* 기타 참고자료 및 파일
*******************************************************************************/
DI_ERR_CODE DI_USB_GetUsbHostCount(HUINT32 *pulCount)
{
	/*
	 * not implement
	 */
	UNUSED(pulCount);
	return 0;
}

/*******************************************************************************
* function : DI_USB_GetUsbHostInfo
* description :
* input :
* output :
* return :
* 기타 참고자료 및 파일
*******************************************************************************/
DI_ERR_CODE DI_USB_GetUsbHostInfo(HUINT32 ulHostId, DI_USB_HOSTCONTROLLER_INFO_t *pstUsbHostInfo)
{
	/*
	 * not implement
	 */
	UNUSED(ulHostId);
	UNUSED(pstUsbHostInfo);
	return 0;
}

/*******************************************************************************
* function : DI_USB_GetUsbDeviceInfo
* description :
* input :
* output :
* return :
* 기타 참고자료 및 파일
*******************************************************************************/
DI_ERR_CODE DI_USB_GetUsbDeviceInfo(HUINT32 ulHostId, HUINT32 ulDevId, DI_USB_DEVICE_INFO_t *pstUsbDevInfo)
{
	/*
	 * not implement
	 */
	UNUSED(ulHostId);
	UNUSED(ulDevId);
	UNUSED(pstUsbDevInfo);
	return 0;
}

/*******************************************************************************
* function : DI_USB_GetUsbDevConfigurationsInfo
* description :
* input :
* output :
* return :
* 기타 참고자료 및 파일
*******************************************************************************/
DI_ERR_CODE DI_USB_GetUsbDevConfigurationsInfo(HUINT32 ulHostId, HUINT32 ulDevId, DI_USB_DEVICE_CONFIGURATION_t *pstUsbDevConfigusInfo)
{
	/*
	 * not implement
	 */
	UNUSED(ulHostId);
	UNUSED(ulDevId);
	UNUSED(pstUsbDevConfigusInfo);
	return 0;}

/*******************************************************************************
* function : DI_USB_GetUsbDevInterfacesInfo
* description :
* input :
* output :
* return :
* 기타 참고자료 및 파일
*******************************************************************************/
DI_ERR_CODE DI_USB_GetUsbDevInterfacesInfo(HUINT32 ulHostId, HUINT32 ulDevId, DI_USB_DEVICE_INTERFACE_t *pstUsbDevInterfacesInfo)
{
	/*
	 * not implement
	 */
	UNUSED(ulHostId);
	UNUSED(ulDevId);
	UNUSED(pstUsbDevInterfacesInfo);
	return 0;
}

/*******************************************************************************
* function : DI_USB_RegisterErrHandleCallback
* description :
* input :
* output :
* return :
* 기타 참고자료 및 파일
*******************************************************************************/
void DI_USB_RegisterErrHandleCallback(pfnDI_USB_ErrNotifyFunc pfnNotify)
{
	/*
	 * not implement
	 */
	UNUSED(pfnNotify);
	return ;
}

/*******************************************************************************
* function : DI_USB_RestorePendingSector
* description :
* input :
* output :
* return :
* 기타 참고자료 및 파일
*******************************************************************************/
DI_ERR_CODE DI_USB_RestorePendingSector(const char *devname, unsigned long long sector)
{
	/*
	 * not implement
	 */
	UNUSED(devname);
	UNUSED(sector);
	return 0;
}

DI_ERR_CODE DI_USB_Init(void)
{
	DRV_Error err;

	err = DRV_USB_Init();
	if (err != DRV_OK) {
		return DI_ERR_ERROR;
	}

	return DI_ERR_OK;
}

/*******************************************************************************
* function : DI_USB_ConnectUSB2Serial
* description :
* input :
* output :
* return :
* 기타 참고자료 및 파일
*******************************************************************************/
#if defined(CONFIG_USB2SERIAL)
static int s_uart_handle = -1;

DI_ERR_CODE DI_USB_ConnectUSB2Serial(void)
{
	int uart_handle;

	uart_handle = BSU_open("usb2serial0");
	if (uart_handle < 0)
		return DI_ERR_ERROR;

	s_uart_handle = uart_handle;

	return DI_ERR_OK;
}

/*******************************************************************************
* function : DI_USB_DisconnectUSB2Serial
* description :
* input :
* output :
* return :
* 기타 참고자료 및 파일
*******************************************************************************/
DI_ERR_CODE DI_USB_DisconnectUSB2Serial(void)
{
	if (s_uart_handle < 0)
		return DI_ERR_ERROR;
	BSU_close(s_uart_handle);
	s_uart_handle = -1;

	return DI_ERR_OK;
}


/*******************************************************************************
* function : DI_USB_WriteUSB2Serial
* description :
* input :
* output :
* return :
* 기타 참고자료 및 파일
*******************************************************************************/
DI_ERR_CODE DI_USB_WriteUSB2Serial(HUINT8 *buffer, HINT32 length)
{
	int res;

    if (s_uart_handle == -1) return DI_ERR_ERROR;

    for (;;)
    {
		res = BSU_write(s_uart_handle, buffer, length);
		if (res < 0)
			break;
		buffer += res;
		length -= res;
		if (length == 0)
			break;
	}

    if (res < 0)
    	return DI_ERR_ERROR;

	return DI_ERR_OK;
}

/*******************************************************************************
* function : DI_USB_ReadUSB2Serial
* description :
* input :
* output :
* return :
* 기타 참고자료 및 파일
*******************************************************************************/
DI_ERR_CODE DI_USB_ReadUSB2Serial(HUINT8 *buffer, HINT32 length)
{
	int res;
	
	if (s_uart_handle == -1) return DI_ERR_ERROR;

	res = BSU_inpstat(s_uart_handle);
	if (res == 0)
		return DI_ERR_ERROR;
		
	res = BSU_read(s_uart_handle, buffer, length);
	if (res < 0)
		return DI_ERR_ERROR;
	
	return DI_ERR_OK;
}

/*******************************************************************************
* function : DI_USB_ClearUSB2Serial
* description :
* input :
* output :
* return :
* 기타 참고자료 및 파일
*******************************************************************************/
DI_ERR_CODE  DI_USB_ClearUSB2Serial(void)
{
	int res;
	unsigned char data;
	
	if (s_uart_handle == -1) return DI_ERR_ERROR;
	
	res = BSU_inpstat(s_uart_handle);
	if (res < 0)
		return DI_ERR_ERROR;

	/*  RX buffer에 데이터가 빌때까지 read 한다. */
	while(res != 0) {
		BSU_read(s_uart_handle, &data, 1);	
		res = BSU_inpstat(s_uart_handle);
	}

	return DI_ERR_OK;
}
#endif
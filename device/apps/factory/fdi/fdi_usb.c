/******************************************************************************
* $Workfile:$
* $Modtime:$
* Author :
* description :		 Factory Application
*
* Copyright (c) 2008 by Humax Co., Ltd.
* All right reserved
******************************************************************************/

/*******************************************************************************
* header and definitions
*******************************************************************************/
/* global header files */
#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#if !defined(CONFIG_OS_RTOS)
#include <termio.h>
#include <termios.h>
#include <poll.h>
#endif

/* chip manufacture's  header files */

/* humax header files */
#include "fdi_common.h"
#include "fdi_usb.h"

#include "htype.h"
#include "di_err.h"
#include "di_usb.h"

/*******************************************************************************
* Debug level
*******************************************************************************/

/*******************************************************************************
* Definitions, typdef & const data
*******************************************************************************/

/*******************************************************************************
* External variables and functions
*******************************************************************************/
/* external variables  and functions */

/*******************************************************************************
* Global variables and structures
*******************************************************************************/
/* global variables and structures */
pfnFDI_USB_NotifyFunc g_pfnUsbNofifyFunc;
static FDI_USB_Info_t s_tFdiUsbInfo[FDI_NUMBER_OF_USBS];
int fd_u2s_rd = 0, fd_u2s_wr = 0;		/* USB to Serial read/write handle */
BOOL usb2SerialInitialized = FALSE;

DI_ERR_CODE INTRN_FDI_USB_NotifyFunc(DI_USB_Event_t etUsbEventType,void *pucUsbInfo);

/*******************************************************************************
* function :
* description :
* input :
* output :
* return :
* 기타 참고자료 및 파일
*******************************************************************************/
FDI_ERR_CODE FDI_USB_Open(void)
{
	return FDI_NO_ERR;
}

/*******************************************************************************
* function :
* description :
* input :
* output :
* return :
* 기타 참고자료 및 파일
*******************************************************************************/
FDI_ERR_CODE FDI_USB_HOST_CTRL_SetPower (unsigned char ucPowerOn)
{
	DI_ERR_CODE eRet;

	eRet = DI_USB_PWR_Ctrl(DI_USB_PWR_ON);
	return eRet;
}


/*******************************************************************************
* function :
* description :
* input :
* output :
* return :
* 기타 참고자료 및 파일
*******************************************************************************/
FDI_ERR_CODE FDI_USB_GetCapability(unsigned long *pulNumOfUsb)
{
	*pulNumOfUsb = FDI_NUMBER_OF_USBS;

	return FDI_NO_ERR;
}

/*******************************************************************************
* function :
* description :
* input :
* output :
* return :
* 기타 참고자료 및 파일
*******************************************************************************/
FDI_ERR_CODE FDI_USB_RegisterCallback(pfnFDI_USB_NotifyFunc pfnCallback)
{
	g_pfnUsbNofifyFunc = pfnCallback;

	DI_USB_RegisterCallback(INTRN_FDI_USB_NotifyFunc);

	return FDI_NO_ERR;
}

/*******************************************************************************
* function :
* description :
* input :
* output :
* return :
* 기타 참고자료 및 파일
*******************************************************************************/
DI_ERR_CODE INTRN_FDI_USB_NotifyFunc(DI_USB_Event_t etUsbEventType, void *pvUsbInfo)
{
	int				i, index = FDI_NUMBER_OF_USBS;
	usb_info_t 		*ptDiUsbInfo;

	ptDiUsbInfo = (usb_info_t *)pvUsbInfo;

	if( ptDiUsbInfo != NULL )
	{
	#if defined(CONFIG_TEMP_HA7100S)
	//temporary block crash point in HA7-100S.
	#else
		FDI_PRINT(FDI_PRT_ERROR, ("++[%s] event %d, port %d, dev name %s\n", __FUNCTION__,
			etUsbEventType, ptDiUsbInfo->ePortNum, ptDiUsbInfo->devName[0]));
	#endif
		if( etUsbEventType == DI_USB_Event_Insert )
		{
			/* PortNum을 index로 사용. */
			switch( ptDiUsbInfo->ePortNum )
			{
				default :
				case FDI_USB_REAR_PORT_ID :
						index = 0; break;
#if defined(FDI_USB_FRONT_PORT_ID)
				case FDI_USB_FRONT_PORT_ID :
						index = 1; break;
#endif
#if defined(FDI_USB_FRONT_PORT_2_ID)
				case FDI_USB_FRONT_PORT_2_ID :
						index = 2; break;
#endif
			}

			s_tFdiUsbInfo[index].ulUsbId = index;
			strncpy(s_tFdiUsbInfo[index].szUsbDevName, ptDiUsbInfo->devName[0], 20);

			s_tFdiUsbInfo[index].speed = ptDiUsbInfo->eSpeed;
		}

		else if( etUsbEventType == DI_USB_Event_Extract )
		{
			/* extract 때는 insert 시의 port number를 기억 하여 참조. */
			for(i=0; i<FDI_NUMBER_OF_USBS; i++)
			{
				if( strcmp(s_tFdiUsbInfo[i].szUsbDevName, ptDiUsbInfo->devName[0]) == 0 )
				{
					FDI_PRINT(FDI_PRT_DBGINFO, ("[%s] matched %d, %s\n", __FUNCTION__, i, ptDiUsbInfo->devName[0] ));
					index = i;
					break;
				}
			}
		}

		if( (g_pfnUsbNofifyFunc != NULL) && (index<FDI_NUMBER_OF_USBS) )
		{
			FDI_PRINT(FDI_PRT_DBGINFO, ("--[%s] callback event %d, port %d, dev name %s\n", __FUNCTION__,
				etUsbEventType , s_tFdiUsbInfo[index].ulUsbId, s_tFdiUsbInfo[index].szUsbDevName));

			g_pfnUsbNofifyFunc(etUsbEventType, &s_tFdiUsbInfo[index]);
		}
		else
		{
			FDI_PRINT(FDI_PRT_ERROR, ("--[%s] ERROR :g_pfnUsbNofifyFunc=0x%x, index=%d\n", __FUNCTION__,
				g_pfnUsbNofifyFunc, index));
		}
	}

	return DI_ERR_OK;
}

/*******************************************************************************
* function :
* description :
* input :
* output :
* return :
* 기타 참고자료 및 파일
*******************************************************************************/
FDI_ERR_CODE FDI_USB_SerialOpen(void)
{
#if !defined(CONFIG_PLATFORM_ALI)
	long baud = B115200;
	struct termios tio;

	if (usb2SerialInitialized == TRUE)
	{
		return FDI_NO_ERR;
	}

	if (fd_u2s_rd <= 0)
	{
		fd_u2s_rd = open("/dev/ttyUSB0", O_RDONLY|O_NOCTTY|O_NONBLOCK);
		if (fd_u2s_rd < 0)
		{
			FDI_PRINT(FDI_PRT_WARNING, ("[%s] : ttyUSB0 read open error(0x%x)!!\n", __FUNCTION__, fd_u2s_rd));

			return FDI_ERR;
		}
	}

	if (fd_u2s_wr <= 0)
	{
		fd_u2s_wr = open("/dev/ttyUSB0", O_WRONLY|O_NOCTTY);
		if (fd_u2s_wr < 0)
		{
			FDI_PRINT(FDI_PRT_WARNING, ("[%s] : ttyUSB0 write open error(0x%x)!!\n", __FUNCTION__, fd_u2s_wr));

			return FDI_ERR;
		}
	}

	tcgetattr(0, &tio );

	tio.c_cflag = baud|CS8|CREAD|CLOCAL;
	tio.c_cflag &= ~HUPCL;
	tio.c_lflag = 0;
	tio.c_iflag = IGNPAR;
	tio.c_oflag = 0;

	tcsetattr(fd_u2s_rd, TCSANOW, &tio);
	tcflush(fd_u2s_rd, TCIFLUSH);
	tcsetattr(fd_u2s_wr, TCSANOW, &tio);
	tcflush(fd_u2s_wr, TCIFLUSH);

	usb2SerialInitialized = TRUE;

	FDI_PRINT(FDI_PRT_DBGINFO, ("[%s] : ttyUSB0 open success!! fd_u2s_rd(0x%x) fd_u2s_wr(0x%x)\n", __FUNCTION__, fd_u2s_rd, fd_u2s_wr));

#endif
	return FDI_NO_ERR;
}

/*******************************************************************************
* function :
* description :
* input :
* output :
* return :
* 기타 참고자료 및 파일
*******************************************************************************/
FDI_ERR_CODE FDI_USB_SerialSend(unsigned char *pucData, unsigned long ulSize)
{
#if !defined(CONFIG_PLATFORM_ALI)
	int i, res;

	if (fd_u2s_wr <= 0)
	{
		FDI_PRINT(FDI_PRT_ERROR, ("[%s] : Not opened!!\n", __FUNCTION__));

		return FDI_ERR;
	}

	tcflush(fd_u2s_wr, TCIFLUSH);

	for (i = 0; i < ulSize; i++)
	{
		res = write(fd_u2s_wr, &pucData[i], 1);
		if(res != 1)
		{
			FDI_PRINT(FDI_PRT_ERROR, ("[%s] : write error(0x%x)!!\n", __FUNCTION__, res));

			return FDI_ERR;
		}
	}
#endif

	return FDI_NO_ERR;
}

/*******************************************************************************
* function :
* description :
* input :
* output :
* return :
* 기타 참고자료 및 파일
*******************************************************************************/
FDI_ERR_CODE FDI_USB_SerialReceive(unsigned char *pucData, unsigned long ulSize, unsigned long ulTimeoutMs)
{
#if !defined(CONFIG_PLATFORM_ALI)
	int i = 0, res;
	int bytes_read;
	struct pollfd pfd;

	if (fd_u2s_rd <= 0)
	{
		FDI_PRINT(FDI_PRT_ERROR, ("[%s] : Not opened!!\n", __FUNCTION__));

		return FDI_ERR;
	}

	pfd.fd = fd_u2s_rd;
	pfd.events = POLLIN;

	for (i = 0; i < ulSize; i++)
	{
		res = poll(&pfd, 1, ulTimeoutMs);
		if (res > 0)
		{
			if (!(pfd.revents & POLLIN))
			{
				FDI_PRINT(FDI_PRT_ERROR, ("[%s] : polling error(0x%x)!!\n", __FUNCTION__, res));

				return FDI_ERR;
			}
			else
			{
				bytes_read = read(fd_u2s_rd, &pucData[i], 1);
				if (bytes_read < 0)
				{
					FDI_PRINT(FDI_PRT_ERROR, ("[%s] : Read data failure !!\n", __FUNCTION__));
					return FDI_ERR;
				}
			}
		}
		else
		{
			FDI_PRINT(FDI_PRT_WARNING, ("[%s] : read timeout(0x%x)!!\n", __FUNCTION__, res));

			return FDI_ERR_WAIT;
		}
	}
#endif

	return FDI_NO_ERR;
}

/*******************************************************************************
* function :
* description :
* input :
* output :
* return :
* 기타 참고자료 및 파일
*******************************************************************************/
FDI_ERR_CODE FDI_USB_SerialClose(void)
{
#if !defined(CONFIG_PLATFORM_ALI)
	if (fd_u2s_rd > 0)
	{
		close(fd_u2s_rd);
	}

	if (fd_u2s_wr > 0)
	{
		close(fd_u2s_wr);
	}

	FDI_PRINT(FDI_PRT_DBGINFO, ("[%s] : close success!!\n", __FUNCTION__));
#endif

	return FDI_NO_ERR;
}

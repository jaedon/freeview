/**************************************************************************************************/
#define _____Client_Sec_Handler_Private_Include______________________________________________
/**************************************************************************************************/
#include <stdio.h>
#include <string.h>
#include <stdarg.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>

#include <sys/socket.h>
#include <sys/ioctl.h>

#include <linux/netlink.h>
#include <linux/rtnetlink.h>
#include <linux/if.h>
#include <linux/wireless.h>
#include <linux/if_arp.h>
#include <linux/route.h>
#include <linux/fs.h>
#include <linux/hdreg.h>

#include <arpa/inet.h>
#include <net/ethernet.h>

#include "di_hdd.h"
#include "di_err.h"
#include "di_usb.h"
#include "di_usb_wireless.h"
#include "di_fs.h"

#include "secma_main.h"
#include "secapi.h"


#ifndef _GNU_SOURCE
#define _GNU_SOURCE 1
#endif
#ifndef _LARGEFILE64_SOURCE
#define _LARGEFILE64_SOURCE
#endif
#ifndef _FILE_OFFSET_BITS
#define _FILE_OFFSET_BITS 64
#endif


#define IPC_IOCTL_DEBUG 		(0)

#ifdef CONFIG_DEBUG
#if !defined(VK_Print)
#define VK_Print(_fmt, ...)				printf(_fmt, ## __VA_ARGS__)
#endif
#else
#if !defined(VK_Print)
#define VK_Print(_fmt, ...)				((void)0)
#endif
#endif

#if defined(VK_Print)
#undef VK_Print
#define VK_Print(_fmt, ...)				((void)0) //printf(_fmt, ## __VA_ARGS__)
#endif


/**************************************************************************************************/
#define _____Client_Sec_Handler_Golbal_Value________________________________________________
/**************************************************************************************************/
static pfnDI_HDD_DSTNotify 				CB_HddDstNotify = NULL;
static DI_FS_Event_Callback_Func 		CB_FsEventNotify = NULL;
static pfnDI_HDD_NotifyFUNC 			CB_HddNotify = NULL;
static pfnDI_USB_NotifyFUNC 			CB_UsbNotify = NULL;
static pfnDI_USB_WIRELESS_NotifyFUNC 	CB_USB_WirelessNotify = NULL;

static pfnDI_USB_HID_Notify 			s_fnHIDUSBMouseCallback = NULL;
static pfnDI_USB_HID_Notify 			s_fnHIDUSBKeyboardCallback = NULL;
static pfnDI_USB_HID_Notify 			s_fnHIDUSBJoystickCallback = NULL;


/**************************************************************************************************/
#define _____Client_Sec_Handler_Private_Define_______________________________________________
/**************************************************************************************************/
static HINT32 s_ulMainQId = 0;
static HINT32 s_ulSubQId[2] = {0,};


/**************************************************************************************************/
#define _____Client_Sec_Handler_Private_Type_Define__________________________________________
/**************************************************************************************************/
#define SMART_BUF_SIZE 		(516) // 512+4

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


/**************************************************************************************************/
#define _____Client_Sec_Handler_Private_Static_Value__________________________________________
/**************************************************************************************************/


/**************************************************************************************************/
#define _____Client_Sec_Handler_Private_Static_Prototype______________________________________
/**************************************************************************************************/
static HINT32 local_secapi_File_Read(HUINT8 *pucFile, HINT32 nOffset, HINT32 nSize, HUINT8 *pucBuff);
static HINT32 local_secapi_Encode_Ioctl(HINT32 nId, HINT32 nRequest, void *pvArgv, HINT8 *paucBuf, HINT32 *pacuBufLen);
static HINT32 local_secapi_Decode_Ioctl(HINT8 *paucBuf, HINT32 nRequest, void *pvArgv);
static HINT32 local_secapi_HddCallback(void *pvMsg, HUINT32 ulSize);
static HINT32 local_secapi_UsbCallback(void *pvMsg, HUINT32 ulSize);
static HINT32 local_secapi_WlanCallback(void *pvMsg, HUINT32 ulSize);
static HINT32 local_secapi_HidCallback(void *pvMsg, HUINT32 ulSize);
#if (1)
static HINT32 local_secapi_HddDstCallback(void *pvMsg, HUINT32 ulSize);
static HINT32 local_secapi_HddForamtCallback(void *pvMsg, HUINT32 ulSize);
#endif

static HINT32 local_secapi_InitRegisterFunctions(IpcSecMsgType eMsgType);
static HINT32 local_secapi_findMsgByType(IpcSecMsgType eMsgType, IpcSecManager_t *pstSecMgr);
static HINT32 local_secapi_StartMsg(IpcSecMsgType eMsgType);


/**************************************************************************************************/
#define ______Client_Sec_Handler_Private_Static_Prototype_Body_________________________________
/**************************************************************************************************/
static HINT32 local_secapi_File_Read(HUINT8 *pucFile, HINT32 nOffset, HINT32 nSize, HUINT8 *pucBuff)
{
	HINT32 nFd, nErr;
	HUINT8 ucPathName[50] = "/tmp/";

	if ( (pucFile == NULL) || (pucBuff == NULL) )
	{
		VK_Print("target are null...\n");
		return ERR_FAIL;
	}

	VK_strncpy(ucPathName+5, pucFile, 45);
	nFd = open(ucPathName, O_RDONLY | O_SYNC);
	if (nFd < 0)
	{
		VK_Print("error in open (%d)\n", nFd);
		return ERR_FAIL;
	}

	nErr = lseek(nFd, nOffset, SEEK_SET);
	if (nErr < 0)
	{
		VK_Print("error in lseek (0x%x)\n", nErr);
		return ERR_FAIL;
	}

	nErr = read(nFd, pucBuff, nSize);
	if (nErr < 0)
	{
		VK_Print("error in read (0x%x)\n", nErr);
		return ERR_FAIL;
	}

	fsync(nFd);
	close(nFd);

	nErr = remove(ucPathName);
	if (nErr < 0)
	{
		VK_Print("error in remove (0x%x)\n", nErr);
		return ERR_FAIL;
	}

	return ERR_OK;
}


static HINT32 local_secapi_Encode_Ioctl(HINT32 nId, HINT32 nRequest, void *pvArgv, HINT8 *paucBuf, HINT32 *pacuBufLen)
{
	HINT32 nErr = ERR_OK;

	if( (paucBuf == NULL) || (pacuBufLen == NULL) )
	{
		VK_Print("[%s][%d] Param. NULL \n", __FUNCTION__, __LINE__);
		return ERR_FAIL;
	}

	VK_memset(paucBuf, 0x00, IPC_MSG_Q_BUFF_MAX);

	VK_memcpy(paucBuf, &nId, sizeof(HINT32));
	VK_memcpy(paucBuf+4, &nRequest, sizeof(HINT32));

	switch(nRequest)
	{
		case SIOCSIWESSID : 			//0x8b1a
		case WLAN_PRIVIOCTL_SET :	//0x8be2
		{
			struct iwreq *iwrequest = pvArgv;

			VK_memcpy(paucBuf+8, pvArgv, sizeof(struct iwreq));
			VK_memcpy(paucBuf+8+sizeof(struct iwreq), iwrequest->u.data.pointer, iwrequest->u.data.length);
			*pacuBufLen = 8 + sizeof(struct iwreq) + iwrequest->u.data.length;

			break;
		}

		case SIOCGIWNAME :		//0x8b01
		case SIOCGIWNWID :		//0x8b03
		case SIOCGIWFREQ :		//0x8b05
		case SIOCGIWMODE :		//0x8b07
		case SIOCGIWRANGE :		//0x8b0b
		case SIOCGIWPRIV :		//0x8b0d
		case SIOCGIWSTATS : 	//0x8b0f
		case SIOCGIWAP :		//0x8b15
		case SIOCSIWSCAN :		//0x8b18
		case SIOCGIWSCAN :		//0x8b19
		case SIOCGIWESSID : 	//0x8b1b
		case SIOCGIWRATE :		//0x8b21
		case SIOCGIWENCODE :	//0x8b2b
		case WLAN_PRIVIOCTL_CONNSTAT :		//0x0004
		case WLAN_PRIVIOCTL_DRIVERVER :		//0x0005
		case WLAN_PRIVIOCTL_BAINFO :		//0x0006
		case WLAN_PRIVIOCTL_DESCINFO :		//0x0007
		case WLAN_PRVIIOCTL_RADIO_OFF :		//0x000a
		case WLAN_PRIVIOCTL_RADIO_ON :		//0x000b
		case WLAN_PRIVIOCTL_SHOW :			//0x0014
		case WLAN_PRIVIOCTL_ADHOCENTRY :		//0x0015
		case WLAN_PRIVIOCTL_STAT :				//0x8be9
		case WLAN_PRIVIOCTL_GET_SITE_SURVEY :	//0x8bed
		case 0x8bf1 :
		{
			VK_memcpy(paucBuf+8, pvArgv, sizeof(struct iwreq));
			*pacuBufLen = 8 + sizeof(struct iwreq);
			break;
		}

		case SIOCADDRT :	// 0x890b
		case SIOCDELRT :		// 0x890c
		{
			struct rtentry *rm = pvArgv;

			VK_memcpy(paucBuf+8, pvArgv, sizeof(struct rtentry));
			VK_memcpy(paucBuf+8+sizeof(struct rtentry), rm->rt_dev, 16);

			*pacuBufLen = 8 + sizeof(struct rtentry) + 16;
			break;
		}

		case SIOCGIFFLAGS :		//0x8913
		case SIOCSIFFLAGS :		//0x8914
		case SIOCGIFADDR :		//0x8915
		case SIOCSIFADDR :		//0x8916
		case SIOCGIFBRDADDR :	//0x8919
		case SIOCSIFBRDADDR :	//0x891a
		case SIOCGIFNETMASK :	//0x891b
		case SIOCSIFNETMASK :	//0x891c
		case SIOCSIFHWADDR :	//0x8924
		case SIOCGIFHWADDR :	//0x8927
		{
			VK_memcpy(paucBuf+8, pvArgv, sizeof(struct ifreq));
			*pacuBufLen = 8 + sizeof(struct ifreq);
			break;
		}

		case SIOCGARP :			// 0x8954
		{
			VK_memcpy(paucBuf+8, pvArgv, sizeof(struct arpreq));
			*pacuBufLen = 8 + sizeof(struct arpreq);
			break;
		}

		case HDIO_DRIVE_TASK :	//0x031e
		case SMART_CMD :		//0x031f
		{
			VK_memcpy(paucBuf+8, pvArgv, SMART_BUF_SIZE);
			*pacuBufLen = 8 + SMART_BUF_SIZE;
			break;
		}

		case HDIO_GET_IDENTITY : // 0x030d
		{
			VK_memcpy(paucBuf+8, pvArgv, 256);
			*pacuBufLen = 8 + 256;
			break;
		}

		case BLKGETSIZE64 :
		{
			VK_memcpy(paucBuf+8, pvArgv, sizeof(unsigned long long));
			*pacuBufLen = 8 + sizeof(unsigned long long);
			break;
		}

		case BLKFLSBUF:
		case BLKSSZGET :
		case BLKGETSIZE :
		case BLKRRPART :
		case HDIO_GETGEO :
		case SIOCGIFCONF :
		case SIOCGIFCOUNT :
		{
			*pacuBufLen = 8;
			break;
		}

		default :
			VK_Print("##### [%s][%d] 0x%x ##### \n", __FUNCTION__, __LINE__, nRequest);
			break;
	}

	return nErr;
}


static HINT32 local_secapi_Decode_Ioctl(HINT8 *paucBuf, HINT32 nRequest, void *pvArgv)
{
	HINT32 nErr = ERR_OK;

	if( paucBuf == NULL )
	{
		VK_Print("[%s][%d] Param. NULL \n", __FUNCTION__, __LINE__);
		return ERR_FAIL;
	}

	switch(nRequest)
	{
		case SIOCGIWNAME :		//0x8b01
		case SIOCGIWNWID :		//0x8b03
		case SIOCGIWFREQ :		//0x8b05
		case SIOCGIWAP :		//0x8b15
		case SIOCSIWESSID : 		//0x8b1a
		case WLAN_PRIVIOCTL_SET :	//0x8be2
		case SIOCSIWSCAN :			//0x8b18
		case SIOCADDRT :			// 0x890b
		case SIOCDELRT :			// 0x890c
		case SIOCSIFFLAGS : 		//0x8914
		case SIOCSIFADDR :		//0x8916
		case SIOCSIFBRDADDR :	//0x891a
		case SIOCSIFNETMASK :	//0x891c
		case SIOCSIFHWADDR :	//0x8924
		case HDIO_DRIVE_TASK :	//0x031e
		case BLKFLSBUF:
		case BLKRRPART :
		{
			break;
		}

		case SIOCGIWMODE :		//0x8b07
		case SIOCGIWRATE :		//0x8b21
		{
			VK_memcpy(pvArgv, paucBuf, sizeof(struct iwreq));
			break;
		}

		case SIOCGIWRANGE : 	//0x8b0b
		case SIOCGIWSTATS : 	//0x8b0f
		case SIOCGIWESSID : 	//0x8b1b
		case SIOCGIWENCODE :	//0x8b2b
		case WLAN_PRIVIOCTL_CONNSTAT :		//0x0004
		case WLAN_PRIVIOCTL_DRIVERVER : 	//0x0005
		case WLAN_PRIVIOCTL_BAINFO :		//0x0006
		case WLAN_PRIVIOCTL_DESCINFO :		//0x0007
		case WLAN_PRVIIOCTL_RADIO_OFF : 	//0x000a
		case WLAN_PRIVIOCTL_RADIO_ON :		//0x000b
		case WLAN_PRIVIOCTL_SHOW :			//0x0014
		case WLAN_PRIVIOCTL_ADHOCENTRY :	//0x0015
		case WLAN_PRIVIOCTL_STAT :			//0x8be9
		case WLAN_PRIVIOCTL_GET_SITE_SURVEY :	//0x8bed
		case 0x8bf1 :
		{
			struct iwreq *iwrequest = pvArgv;

			VK_memcpy(&iwrequest->u.data.length, paucBuf, sizeof(HUINT16));
			VK_memcpy(iwrequest->u.data.pointer, paucBuf+sizeof(HUINT16), iwrequest->u.data.length);
			break;
		}

		case SIOCGIWPRIV :		//0x8b0d
		{
			struct iwreq *iwrequest = pvArgv;

			VK_memcpy(&iwrequest->u.data.length, paucBuf, sizeof(HUINT16));
			VK_memcpy(iwrequest->u.data.pointer, paucBuf+sizeof(HUINT16), iwrequest->u.data.length * sizeof(struct iw_priv_args));
			break;
		}

		case SIOCGIWSCAN :		//0x8b19
		{
			struct iwreq *iwrequest = pvArgv;

			VK_memcpy(&iwrequest->u.data.length, paucBuf, sizeof(HUINT16));
			local_secapi_File_Read("SIOCGIWSCAN.raw" , 0 , iwrequest->u.data.length, iwrequest->u.data.pointer);

			break;
		}

		case SIOCGIFFLAGS : 		//0x8913
		case SIOCGIFADDR :		//0x8915
		case SIOCGIFBRDADDR :	//0x8919
		case SIOCGIFNETMASK :	//0x891b
		case SIOCGIFHWADDR :	//0x8927
		case SIOCGIFCOUNT : 		//0x8938
		{
			VK_memcpy(pvArgv, paucBuf, sizeof(struct ifreq));
			break;
		}

		case SIOCGARP : 			// 0x8954
		{
			VK_memcpy(pvArgv, paucBuf, sizeof(struct arpreq));
			break;
		}

		case SMART_CMD :		//0x031f
		{
			VK_memcpy(pvArgv, paucBuf, SMART_BUF_SIZE);
			break;
		}

		case HDIO_GET_IDENTITY : // 0x030d
		{
			VK_memcpy(pvArgv, paucBuf, 256);
			break;
		}

		case BLKGETSIZE64 :
		{
			VK_memcpy(pvArgv, paucBuf, sizeof(unsigned long long));
			break;
		}

		case BLKSSZGET :
		{
			VK_memcpy(pvArgv, paucBuf, sizeof(unsigned int));
			break;
		}

		case BLKGETSIZE :
		{
			VK_memcpy(pvArgv, paucBuf, sizeof(unsigned long));
			break;
		}

		case HDIO_GETGEO :
		{
			VK_memcpy(pvArgv, paucBuf, sizeof(struct hd_geometry));
			break;
		}

		case SIOCGIFCONF :		// 0x8912 mslee -> server, client OK
		{
			VK_memcpy(pvArgv, paucBuf, sizeof(struct ifconf));
			break;
		}

		default :
			VK_Print("##### [%s][%d] 0x%x ##### \n", __FUNCTION__, __LINE__, nRequest);
			break;
	}

	return nErr;
}


static HINT32 local_secapi_HddCallback(void *pvMsg, HUINT32 ulSize)
{
	DI_ERR_CODE nErr;
	IpcSecHddCb_t stCb;

	if (CB_HddNotify == NULL)
	{
		VK_Print("[%s][%d], HDD CB not registered\n", __FUNCTION__, __LINE__);
		return -1;
	}

	VK_memcpy(&stCb, pvMsg, sizeof(IpcSecHddCb_t));

	nErr = CB_HddNotify(stCb.etEventType, (void *)&stCb.etHddInfo);

	return nErr;
}


static HINT32 local_secapi_UsbCallback(void *pvMsg, HUINT32 ulSize)
{
	DI_ERR_CODE nErr;
	IpcSecUsbCb_t stCb;
	usb_info_t etUsbInfo;
	unsigned char index;

	if (CB_UsbNotify == NULL)
	{
		VK_Print("[%s][%d], USB CB not registered\n", __FUNCTION__, __LINE__);
		return -1;
	}

	VK_memcpy(&stCb, pvMsg, sizeof(IpcSecUsbCb_t));

	etUsbInfo.devCount = stCb.devCount;
	etUsbInfo.devNameSize = stCb.devNameSize;
	etUsbInfo.ePortNum = stCb.ePortNum;
	etUsbInfo.eSpeed = stCb.eSpeed;
	etUsbInfo.isOverCurrent = stCb.isOverCurrent;
	etUsbInfo.devName = (char **)HLIB_STD_MemAlloc( sizeof(char *) * etUsbInfo.devCount );
	for (index=0; index<etUsbInfo.devCount; index++)
	{
		etUsbInfo.devName[index] = (char *)HLIB_STD_MemAlloc( sizeof(char) * etUsbInfo.devNameSize);
		VK_strncpy(etUsbInfo.devName[index], stCb.devName, 4);
	}

	nErr = CB_UsbNotify(stCb.etEventType, (void *)&etUsbInfo);

	return nErr;
}


static HINT32 local_secapi_WlanCallback(void *pvMsg, HUINT32 ulSize)
{
	DI_ERR_CODE nErr;
	IpcSecWlanCb_t stCb;

	if (CB_USB_WirelessNotify == NULL)
	{
		VK_Print("[%s][%d], CB_USB_WirelessNotify not registered\n", __FUNCTION__, __LINE__);
		return -1;
	}

	VK_memcpy(&stCb, pvMsg, sizeof(IpcSecWlanCb_t));

	nErr = CB_USB_WirelessNotify(stCb.etEventType, (void *)&stCb.etWlanInfo);

	return nErr;
}


static HINT32 local_secapi_HidCallback(void *pvMsg, HUINT32 ulSize)
{
	HINT32 nErr;
	IpcSecHidCb_t stCb;

	VK_memcpy(&stCb, pvMsg, sizeof(IpcSecHidCb_t));

	switch(stCb.etDevice)
	{
		case DI_USB_HID_HANDLER_USB_KEYBOARD:
			if(s_fnHIDUSBKeyboardCallback != NULL)
				nErr = s_fnHIDUSBKeyboardCallback(stCb.etDevice, stCb.etData);
			break;
		case DI_USB_HID_HANDLER_USB_MOUSE:
			if(s_fnHIDUSBMouseCallback != NULL)
				nErr = s_fnHIDUSBMouseCallback(stCb.etDevice, stCb.etData);
			break;
		case DI_USB_HID_HANDLER_USB_JOYSTICK:
			if(s_fnHIDUSBJoystickCallback != NULL)
				nErr = s_fnHIDUSBJoystickCallback(stCb.etDevice, stCb.etData);
			break;
		default:
			nErr = -1;
			break;
	}

	return nErr;
}


#if (1)
static HINT32 local_secapi_HddDstCallback(void *pvMsg, HUINT32 ulSize)
{
	HINT32 nErr = ERR_OK;
	IpcSecHddDstEvtInfo_t stHddDstCb;

	VK_memcpy(&stHddDstCb, (IpcSecHddDstEvtInfo_t*)pvMsg, sizeof(IpcSecHddDstEvtInfo_t));

	if (CB_HddDstNotify)
	{
		CB_HddDstNotify(stHddDstCb.dstResult, stHddDstCb.nProgress);
	}

	return nErr;
}


static HINT32 local_secapi_HddForamtCallback(void *pvMsg, HUINT32 ulSize)
{
	HINT32 nErr = ERR_OK;
	IpcSecHddFormatEvtInfo_t stHddFormatCb;

	VK_memcpy(&stHddFormatCb, (IpcSecHddFormatEvtInfo_t*)pvMsg, sizeof(IpcSecHddFormatEvtInfo_t));

	if (CB_FsEventNotify)
	{
		CB_FsEventNotify(stHddFormatCb.ulEvent, stHddFormatCb.ulParam1, stHddFormatCb.ulParam2, stHddFormatCb.ulParam3);
	}

	return nErr;
}
#endif


static HINT32 local_secapi_InitRegisterFunctions(IpcSecMsgType eMsgType)
{
	HERROR nErr = ERR_OK;

	if (eMsgType == IPC_COMM_TYPE_OCTO)
	{
		nErr |= VK_IPC_MSG_RegisterCallback(s_ulSubQId[1], IPC_SEC_CONV_FN_HddCb, local_secapi_HddCallback);
		nErr |= VK_IPC_MSG_RegisterCallback(s_ulSubQId[1], IPC_SEC_CONV_FN_UsbCb, local_secapi_UsbCallback);
		nErr |= VK_IPC_MSG_RegisterCallback(s_ulSubQId[1], IPC_SEC_CONV_FN_WlanCb, local_secapi_WlanCallback);
		nErr |= VK_IPC_MSG_RegisterCallback(s_ulSubQId[1], IPC_SEC_CONV_FN_HidCb, local_secapi_HidCallback);
#if (1)
		nErr |= VK_IPC_MSG_RegisterCallback(s_ulSubQId[1], IPC_SEC_CONV_FN_HddDstCb, local_secapi_HddDstCallback);
		nErr |= VK_IPC_MSG_RegisterCallback(s_ulSubQId[1], IPC_SEC_CONV_FN_HddFormatCb, local_secapi_HddForamtCallback);
#endif
	}

	return nErr;
}


static HINT32 local_secapi_findMsgByType(IpcSecMsgType eMsgType, IpcSecManager_t *pstSecMgr)
{
	int i;

	if (pstSecMgr == NULL)
		return ERR_FAIL;

	for (i=0; i< IPC_SYS_MSGQ_MAX_MSG; i++)
	{
		if (eMsgType == s_stMsgMgr[i].eMsgType)
		{
			VK_memcpy(pstSecMgr, &s_stMsgMgr[i], sizeof(IpcSecManager_t));
			break;
		}
	}

	return ((i < IPC_SYS_MSGQ_MAX_MSG) ? ERR_OK : ERR_FAIL);
}


static HINT32 local_secapi_StartMsg(IpcSecMsgType eMsgType)
{
	HINT32 nErr;
	IpcSecManager_t stSecMgr;

	if (VK_IPC_MSG_Open(0, IPC_MSG_Q_BUFF_MAX, IPC_SYS_MSGQ_PATH_SECMA, &s_ulMainQId) != VK_OK)
	{
		VK_Print("error in VK_IPC_MSG_Open....\n");
		return -1;
	}

	VK_memset(&stSecMgr, 0, sizeof(IpcSecManager_t));

	if (IPC_MSG_Q_BUFF_MAX < sizeof(IpcSecManager_t))
	{
		VK_Print("over IPC_MSG_Q_BUFF_MAX....\n");
		return -1;
	}

	if (local_secapi_findMsgByType(eMsgType, &stSecMgr) != ERR_OK)
	{
		VK_Print("error in local_secapi_findMsg....\n");
		return -1;
	}

	VK_Print("\n\t ==> local_secapi_StartMsg : eMsgType = %d\n", eMsgType);
	VK_Print("\n\t ==> local_secapi_StartMsg : Target = %s\n", stSecMgr.ucTarget);

	if (VK_IPC_MSG_Call(s_ulMainQId, IPC_SEC_CONV_FN_InitSec, &nErr, (void*)&stSecMgr, sizeof(IpcSecManager_t)) != VK_OK)
	{
		VK_Print("error in VK_IPC_MSG_Call....\n");
		return -1;
	}

	VK_Print("\n\t ==> local_secapi_StartMsg : success....\n");

	return ERR_OK;
}



/**************************************************************************************************/
#define __________Client_Sec_Handler_Public_Functions_Body___________________________________
/**************************************************************************************************/
HINT32 SECAPI_Init(IpcSecMsgType eMsgType)
{
	HINT32 nErr = 0;
	IpcSecManager_t stSecMgr;

	nErr = local_secapi_StartMsg(eMsgType);
	if (nErr == -1)
	{
		VK_Print("error in local_secapi_StartMsg....\n");
		return ERR_FAIL;
	}

	if (local_secapi_findMsgByType(eMsgType, &stSecMgr) != ERR_OK)
	{
		VK_Print("error in local_secapi_findMsg....\n");
		return ERR_FAIL;
	}

	VK_Print("\n\t ==> SECAPI_Init : eMsgType = %d\n", eMsgType);

	// client(target -> secure handler)
	if (VK_IPC_MSG_Open(0, IPC_MSG_Q_BUFF_MAX, stSecMgr.ucPath[0], &s_ulSubQId[0]) != VK_OK)
	{
		VK_Print("error in VK_IPC_MSG_Open....\n");
		return ERR_FAIL;
	}

	// server(secure handler -> target)
	if (VK_IPC_MSG_Open(0, IPC_MSG_Q_BUFF_MAX, stSecMgr.ucPath[1], &s_ulSubQId[1]) != VK_OK)
	{
		VK_Print("error in VK_IPC_MSG_Open....\n");
		return ERR_FAIL;
	}

	if (local_secapi_InitRegisterFunctions(eMsgType) != 0)
	{
		VK_Print("error in SECAPI_InitRegisterFunctions....\n");
		return ERR_FAIL;
	}

	return ERR_OK;
}


HINT32 SECAPI_Mount(const DI_FS_StorDevIdx_t eDevIdx, const HUINT32 ulPartIdx, const HUINT8 *szDstMountDir, const DI_FileSystemType_t eFsType)
{
	HINT32 nErr;
	IpcSecMount_t stMountInfo;

	VK_memset(&stMountInfo, 0, sizeof(IpcSecMount_t));

	if (IPC_MSG_Q_BUFF_MAX < sizeof(IpcSecMount_t))
	{
		VK_Print("over IPC_MSG_Q_BUFF_MAX....\n");
		return ERR_FAIL;
	}

	stMountInfo.eDevIdx = eDevIdx;
	stMountInfo.ulPartIdx = ulPartIdx;
	if (szDstMountDir != NULL)
	{
		VK_strncpy(&stMountInfo.szDstMountDir, szDstMountDir, IPC_MOUNT_MAX_SIZE);
	}
	stMountInfo.eFsType = eFsType;

	if (VK_IPC_MSG_Call(s_ulSubQId[0], IPC_SEC_CONV_FN_Mount, &nErr, (void*)&stMountInfo, sizeof(IpcSecMount_t)) != VK_OK)
	{
		return ERR_FAIL;
	}

	return nErr;
}


HINT32 SECAPI_Umount(DI_FS_StorDevIdx_t eDevIdx, HUINT32 ulPartIdx, DI_FileSystemType_t eFsType)
{
	HINT32 nErr;
	IpcSecMount_t stMountInfo;

	VK_memset(&stMountInfo, 0, sizeof(IpcSecMount_t));

	if (IPC_MSG_Q_BUFF_MAX < sizeof(IpcSecMount_t))
	{
		VK_Print("over IPC_MSG_Q_BUFF_MAX....\n");
		return ERR_FAIL;
	}

	stMountInfo.eDevIdx = eDevIdx;
	stMountInfo.ulPartIdx = ulPartIdx;
	stMountInfo.eFsType = eFsType;

	if (VK_IPC_MSG_Call(s_ulSubQId[0], IPC_SEC_CONV_FN_Umount, &nErr, (void*)&stMountInfo, sizeof(IpcSecMount_t)) != VK_OK)
	{
		return ERR_FAIL;
	}

	return nErr;
}


HINT32 SECAPI_Stime(VK_CLOCK_SET_MODE eSetMode, unsigned long ulUnixTime)
{
	HINT32 nErr;
	IpcSecSetTime_t stSetTime;

	VK_memset(&stSetTime, 0, sizeof(IpcSecSetTime_t));
	stSetTime.eSetMode = eSetMode;
	stSetTime.ulUnixTime = ulUnixTime;

	if (VK_IPC_MSG_Call(s_ulSubQId[0], IPC_SEC_CONV_FN_Stime, &nErr, (void*)&stSetTime, sizeof(IpcSecSetTime_t)) != VK_OK)
	{
		return ERR_FAIL;
	}

	return nErr;
}


HINT32 SECAPI_StimeSetTime(unsigned long ulUnixTime)
{
	HINT32 nErr;

	if (VK_IPC_MSG_Call(s_ulSubQId[0], IPC_SEC_CONV_FN_StimeSetTime, &nErr, (void*)&ulUnixTime, sizeof(unsigned long)) != VK_OK)
	{
		return ERR_FAIL;
	}

	return nErr;
}


HINT32 SECAPI_StimeSetMode(VK_CLOCK_SET_MODE eSetMode)
{
	HINT32 nErr;

	if (VK_IPC_MSG_Call(s_ulSubQId[0], IPC_SEC_CONV_FN_StimeSetMode, &nErr, (void*)&eSetMode, sizeof(VK_CLOCK_SET_MODE)) != VK_OK)
	{
		return ERR_FAIL;
	}

	return nErr;
}


HINT32 SECAPI_StimeGetMode(VK_CLOCK_SET_MODE *peGetMode)
{
	HINT32 nErr;
	VK_CLOCK_SET_MODE eGetMode;

	if (peGetMode == NULL)
	{
		return ERR_FAIL;
	}

	if (VK_IPC_MSG_Call(s_ulSubQId[0], IPC_SEC_CONV_FN_StimeGetMode, &nErr, (void*)&eGetMode, sizeof(VK_CLOCK_SET_MODE)) != VK_OK)
	{
		return ERR_FAIL;
	}

	*peGetMode = eGetMode;
	return nErr;
}


HINT32 SECAPI_Settimeofday(HINT32 nHour, HINT32 nMin)
{
	HINT32 nErr;
	IpcSecSettimeofday_t stSettimeofDayInfo;

	VK_memset(&stSettimeofDayInfo, 0, sizeof(IpcSecSettimeofday_t));

	if (IPC_MSG_Q_BUFF_MAX < sizeof(IpcSecSettimeofday_t))
	{
		VK_Print("over IPC_MSG_Q_BUFF_MAX....\n");
		return ERR_FAIL;
	}

	stSettimeofDayInfo.nHour = nHour;
	stSettimeofDayInfo.nMin = nMin;

	if (VK_IPC_MSG_Call(s_ulSubQId[0], IPC_SEC_CONV_FN_Settimeofday, &nErr, (void*)&stSettimeofDayInfo, sizeof(IpcSecSettimeofday_t)) != VK_OK)
	{
		return ERR_FAIL;
	}

	return nErr;
}


HINT32 SECAPI_Chmod(const HINT8 *pucPath, HUINT32 ulMode)
{
	HINT32 nErr;
	IpcSecFileCtl_t stFileCtrl;
	HUINT32 ulLen;

	VK_memset(&stFileCtrl, 0, sizeof(IpcSecFileCtl_t));

	if (IPC_MSG_Q_BUFF_MAX < sizeof(IpcSecFileCtl_t))
	{
		VK_Print("over IPC_MSG_Q_BUFF_MAX....\n");
		return ERR_FAIL;
	}

	if (pucPath != NULL)
	{
		ulLen = VK_strlen(pucPath);
		if(ulLen > IPC_PATH_MAX_SIZE)
		{
			ulLen = IPC_PATH_MAX_SIZE;
			VK_Print("##### [%s][%d] File Name:%s, mode:%d, lennth:%d \n", __FUNCTION__, __LINE__, stFileCtrl.ucPath, stFileCtrl.ulMode, ulLen);
		}
		VK_strncpy(&stFileCtrl.ucPath, pucPath, ulLen);
	}

	stFileCtrl.ulMode = ulMode;

	if (VK_IPC_MSG_Call(s_ulSubQId[0], IPC_SEC_CONV_FN_Chmod, &nErr, (void*)&stFileCtrl, sizeof(IpcSecFileCtl_t)) != VK_OK)
	{
		return ERR_FAIL;
	}

	return nErr;
}


HINT32 SECAPI_Chown(const HINT8 *pucPath)
{
	HINT32 nErr;
	IpcSecFileCtl_t stFileCtrl;
	HUINT32 ulLen;

	VK_memset(&stFileCtrl, 0, sizeof(IpcSecFileCtl_t));

	if (IPC_MSG_Q_BUFF_MAX < sizeof(IpcSecFileCtl_t))
	{
		VK_Print("over IPC_MSG_Q_BUFF_MAX....\n");
		return ERR_FAIL;
	}

	if (pucPath != NULL)
	{
		ulLen = VK_strlen(pucPath);
		if(ulLen > IPC_PATH_MAX_SIZE)
		{
			ulLen = IPC_PATH_MAX_SIZE;
			VK_Print("##### [%s][%d] File Name:%s, lennth:%d \n", __FUNCTION__, __LINE__, stFileCtrl.ucPath, ulLen);
		}
		VK_strncpy(&stFileCtrl.ucPath, pucPath, ulLen);
	}

	VK_Print("##### [%s][%d] File Name:%s, lennth:%d \n", __FUNCTION__, __LINE__, stFileCtrl.ucPath, ulLen);

	if (VK_IPC_MSG_Call(s_ulSubQId[0], IPC_SEC_CONV_FN_Chown, &nErr, (void*)&stFileCtrl, sizeof(IpcSecFileCtl_t)) != VK_OK)
	{
		return ERR_FAIL;
	}

	return nErr;
}


HINT32 SECAPI_Remove(const HINT8 *pucPath)
{
	HINT32 nErr;
	IpcSecFileCtl_t stFileCtrl;
	HUINT32 ulLen;

	VK_memset(&stFileCtrl, 0, sizeof(IpcSecFileCtl_t));

	if (IPC_MSG_Q_BUFF_MAX < sizeof(IpcSecFileCtl_t))
	{
		VK_Print("over IPC_MSG_Q_BUFF_MAX....\n");
		return ERR_FAIL;
	}

	if (pucPath != NULL)
	{
		ulLen = VK_strlen(pucPath);
		if(ulLen > IPC_PATH_MAX_SIZE)
		{
			ulLen = IPC_PATH_MAX_SIZE;
			VK_Print("##### [%s][%d] File Name:%s, lennth:%d \n", __FUNCTION__, __LINE__, stFileCtrl.ucPath, ulLen);
		}
		VK_strncpy(&stFileCtrl.ucPath, pucPath, ulLen);
	}

	if (VK_IPC_MSG_Call(s_ulSubQId[0], IPC_SEC_CONV_FN_Remove, &nErr, (void*)&stFileCtrl, sizeof(IpcSecFileCtl_t)) != VK_OK)
	{
		return ERR_FAIL;
	}

	return nErr;
}


HINT32 SECAPI_Mkdir(const HINT8 *pucPath, HUINT32 ulMode)
{
	HINT32 nErr;
	IpcSecFileCtl_t stFileCtrl;
	HUINT32 ulLen;

	VK_memset(&stFileCtrl, 0, sizeof(IpcSecFileCtl_t));

	if (IPC_MSG_Q_BUFF_MAX < sizeof(IpcSecFileCtl_t))
	{
		VK_Print("over IPC_MSG_Q_BUFF_MAX....\n");
		return ERR_FAIL;
	}

	VK_Print("\n\n\t =============> SECAPI_Mkdir : pucPath = %s, Mode = %d", pucPath, ulMode);

	if (pucPath != NULL)
	{
		ulLen = VK_strlen(pucPath);
		if(ulLen > IPC_PATH_MAX_SIZE)
		{
			ulLen = IPC_PATH_MAX_SIZE;
			VK_Print("##### [%s][%d] File Name:%s, mode:%d, length:%d \n", __FUNCTION__, __LINE__, stFileCtrl.ucPath, stFileCtrl.ulMode, ulLen);
		}
		VK_strncpy(&stFileCtrl.ucPath, pucPath, ulLen);
	}

	stFileCtrl.ulMode = ulMode;

	if (VK_IPC_MSG_Call(s_ulSubQId[0], IPC_SEC_CONV_FN_Mkdir, &nErr, (void*)&stFileCtrl, sizeof(IpcSecFileCtl_t)) != VK_OK)
	{
		return ERR_FAIL;
	}

	VK_Print("\t =============> SECAPI_Mkdir : nErr = %d\n", nErr);

	return nErr;
}


HINT32 SECAPI_Mknod(const HINT8 *pucPath, HUINT32 ulMode, HUINT32 ulDev)
{
	HINT32 nErr;
	IpcSecFileCtl_t stFileCtrl;
	HUINT32 ulLen;

	VK_memset(&stFileCtrl, 0, sizeof(IpcSecFileCtl_t));

	if (IPC_MSG_Q_BUFF_MAX < sizeof(IpcSecFileCtl_t))
	{
		VK_Print("over IPC_MSG_Q_BUFF_MAX....\n");
		return ERR_FAIL;
	}

	if (pucPath != NULL)
	{
		ulLen = VK_strlen(pucPath);
		if(ulLen > IPC_PATH_MAX_SIZE)
		{
			ulLen = IPC_PATH_MAX_SIZE;
			VK_Print("##### [%s][%d] File Name:%s, mode:%d, length:%d \n", __FUNCTION__, __LINE__, stFileCtrl.ucPath, stFileCtrl.ulMode, ulLen);
		}
		VK_strncpy(&stFileCtrl.ucPath, pucPath, ulLen);
	}

	stFileCtrl.ulMode = ulMode;
	stFileCtrl.ulDev = ulDev;

	if (VK_IPC_MSG_Call(s_ulSubQId[0], IPC_SEC_CONV_FN_Mknod, &nErr, (void*)&stFileCtrl, sizeof(IpcSecFileCtl_t)) != VK_OK)
	{
		return ERR_FAIL;
	}

	return nErr;
}


HINT32 SECAPI_SysCmd(HUINT8 ucCmdNum, ...)
{
	typedef HUINT8* PUINT8;

	va_list 	args;

	HUINT8 	i;
	HUINT8 	*pucData;

	HINT32 	nErr;
	IpcSecSysCmd_t stSysCmd;

	if (ucCmdNum <= 0)
	{
		return ERR_FAIL;
	}

	if (ucCmdNum > IPC_SYS_CMD_ARGC_MAX_SIZE)
	{
		VK_Print("over IPC_SYS_CMD_ARGC_MAX_SIZE....\n");
		return ERR_FAIL;
	}

	VK_memset(&stSysCmd, 0, sizeof(IpcSecSysCmd_t));

	if (IPC_MSG_Q_BUFF_MAX < sizeof(IpcSecSysCmd_t))
	{
		VK_Print("over IPC_MSG_Q_BUFF_MAX....\n");
		return ERR_FAIL;
	}

	VK_Print("SECAPI_SysCmd : ucNum = %d\n", ucCmdNum);

	stSysCmd.ucArgc = ucCmdNum;

	va_start(args, ucCmdNum);
	for (i=0; i<ucCmdNum; i++)
	{
		pucData = va_arg(args, PUINT8);
		VK_Print("\t\t (%d) Arg (%s)\n", i, pucData);
		VK_strncpy(stSysCmd.ucArgv[i], pucData, IPC_SYS_CMD_ARGV_MAX_SIZE);
	}
	va_end(args);

	if (VK_IPC_MSG_Call(s_ulSubQId[0], IPC_SEC_CONV_FN_SysCmd, &nErr, (void*)&stSysCmd, sizeof(IpcSecSysCmd_t)) != VK_OK)
	{
		return ERR_FAIL;
	}

	VK_Print("##### [%s][%d] nErr :0x%x\n", __FUNCTION__, __LINE__, nErr);

	return nErr;
}


HINT32 SECAPI_Ioctl(HINT32 nId, HINT32 nRequest, HUINT8 ucCmdNum, ...)
{
	typedef void* PVOID;

	va_list 	args;
	HUINT8 	i;
	void 	**ppvData = NULL;
	HINT32 	nErr;
	HINT8 	ucBuf[IPC_MSG_Q_BUFF_MAX];
	HINT32 	nBufLen=0;

	if (ucCmdNum <= 0)
	{
		return ERR_FAIL;
	}

#if !(IPC_IOCTL_DEBUG)
	VK_Print("[%s][%d] nId : %d, nRequest : 0x%x\n", __FUNCTION__, __LINE__, nId, nRequest);
#endif

	ppvData = (PVOID)HLIB_STD_MemAlloc(ucCmdNum * sizeof(PVOID));
	if (ppvData == NULL)
		return ERR_FAIL;

	va_start(args, ucCmdNum);
	for (i=0; i<ucCmdNum; i++)
	{
		ppvData[i] = va_arg(args, PVOID);
#if !(IPC_IOCTL_DEBUG)
		VK_Print("\t\t (%d) Arg (x%0x)\n", i, ppvData[i]);
#endif
	}
	va_end(args);

	/* 여기서는 가변인수가 1로 가정 함 - OCTO에서 현재 하나만 사용함. */
	nErr = local_secapi_Encode_Ioctl(nId, nRequest, ppvData[0], ucBuf, &nBufLen);
	if ( (nErr != ERR_OK) || (nBufLen >  IPC_MSG_Q_BUFF_MAX) )
	{
		HLIB_STD_MemFree(ppvData);
		return -1;
	}

	if (VK_IPC_MSG_Call(s_ulSubQId[0], IPC_SEC_CONV_FN_Ioctl, &nErr, (void*)ucBuf, IPC_MSG_Q_BUFF_MAX) != VK_OK)
	{
		HLIB_STD_MemFree(ppvData);
		return -1;
	}

#if !(IPC_IOCTL_DEBUG)
	VK_Print("##### [%s][%d] nErr :0x%x\n", __FUNCTION__, __LINE__, nErr);
#endif

	if (nErr >= 0)
	{
		local_secapi_Decode_Ioctl(ucBuf, nRequest, ppvData[0]);
	}

	HLIB_STD_MemFree(ppvData);

	return nErr;
}


HINT32 SECAPI_Stat(const HINT8 *pucPath, struct stat *pstBuf)
{
	HINT32 nErr = ERR_OK;
	IpcSecStat_t stStat;

	VK_memset(&stStat, 0, sizeof(IpcSecStat_t));

	if (IPC_MSG_Q_BUFF_MAX < sizeof(IpcSecStat_t))
	{
		VK_Print("over IPC_MSG_Q_BUFF_MAX....\n");
		return ERR_FAIL;
	}

	VK_strncpy(stStat.ucPath, pucPath, IPC_PATH_MAX_SIZE);

	if (VK_IPC_MSG_Call(s_ulSubQId[0], IPC_SEC_CONV_FN_Stat, &nErr, (void*)&stStat, sizeof(IpcSecStat_t)) != VK_OK)
	{
		return ERR_FAIL;
	}

	if (nErr == ERR_OK)
		VK_memcpy(pstBuf, &stStat.stStat, sizeof(struct stat));

	return nErr;
}


HINT32 SECAPI_Lstat(const HINT8 *pucPath, struct stat *pstBuf)
{
	HINT32 nErr =ERR_OK;
	IpcSecStat_t stStat;

	VK_memset(&stStat, 0, sizeof(IpcSecStat_t));

	if (IPC_MSG_Q_BUFF_MAX < sizeof(IpcSecStat_t))
	{
		VK_Print("over IPC_MSG_Q_BUFF_MAX....\n");
		return ERR_FAIL;
	}

	VK_strncpy(stStat.ucPath, pucPath, IPC_PATH_MAX_SIZE);

	if (VK_IPC_MSG_Call(s_ulSubQId[0], IPC_SEC_CONV_FN_Lstat, &nErr, (void*)&stStat, sizeof(IpcSecStat_t)) != VK_OK)
	{
		return ERR_FAIL;
	}

	if (nErr == ERR_OK)
		VK_memcpy(pstBuf, &stStat.stStat, sizeof(struct stat));

	return nErr;
}


HINT32 SECAPI_Socket(HINT32 nDomain, HINT32 nType, HINT32 nProtocol)
{
	HINT32 nSocketFd;
	IpcSecSocket_t stSocket;

	VK_memset(&stSocket, 0, sizeof(IpcSecSocket_t));

	if (IPC_MSG_Q_BUFF_MAX < sizeof(IpcSecSocket_t))
	{
		VK_Print("over IPC_MSG_Q_BUFF_MAX....\n");
		return ERR_FAIL;
	}

	VK_Print("##### [%s][%d] Domain :0x%x, Type = %d, Protocol = %d\n", __FUNCTION__, __LINE__, nDomain, nType, nProtocol);

	VK_memcpy(&stSocket.nDomain, &nDomain, sizeof(HINT32));
	VK_memcpy(&stSocket.nType, &nType, sizeof(HINT32));
	VK_memcpy(&stSocket.nProtocol, &nProtocol, sizeof(HINT32));

	if (VK_IPC_MSG_Call(s_ulSubQId[0], IPC_SEC_CONV_FN_Socket, &nSocketFd, (void*)&stSocket, sizeof(IpcSecSocket_t)) != VK_OK)
	{
		return ERR_FAIL;
	}

	VK_Print("##### [%s][%d] Socket FD :0x%x\n", __FUNCTION__, __LINE__, nSocketFd);

	return nSocketFd;
}


HINT32 SECAPI_Select(HINT32 nFd, fd_set *pstReadFds, fd_set *pstWriteFds, fd_set *pstExceptFds, struct timeval *pstTimeout)
{
	HINT32 nRet;
	IpcSecSelect_t stSelect;

	VK_memset(&stSelect, 0, sizeof(IpcSecSelect_t));

	if (IPC_MSG_Q_BUFF_MAX < sizeof(IpcSecSelect_t))
	{
		VK_Print("over IPC_MSG_Q_BUFF_MAX....\n");
		return ERR_FAIL;
	}

	VK_Print("##### [%s][%d] Socket FD :0x%x\n", __FUNCTION__, __LINE__, nFd);

	stSelect.nFd = nFd;
	if (pstReadFds != NULL)
		VK_memcpy(&stSelect.stReadFds, pstReadFds, sizeof(fd_set));
	if (pstWriteFds != NULL)
		VK_memcpy(&stSelect.stWriteFds, pstWriteFds, sizeof(fd_set));
	if (pstExceptFds != NULL)
		VK_memcpy(&stSelect.stExceptFds, pstExceptFds, sizeof(fd_set));
	if (pstTimeout != NULL)
		VK_memcpy(&stSelect.stTimeout, pstTimeout, sizeof(struct timeval));

	if (VK_IPC_MSG_Call(s_ulSubQId[0], IPC_SEC_CONV_FN_Select, &nRet, (void*)&stSelect, sizeof(IpcSecSelect_t)) != VK_OK)
	{
		return ERR_FAIL;
	}

	VK_Print("##### [%s][%d] nErr :0x%x\n", __FUNCTION__, __LINE__, nRet);

	return nRet;
}


HLONG SECAPI_Send(HINT32 nFd, const void *pucBuf, size_t nLen, HINT32 nFlags)
{
	HLONG nErr = 0;
	HINT8 ucBuf[IPC_MSG_Q_BUFF_MAX];

	VK_memset(ucBuf, 0, IPC_MSG_Q_BUFF_MAX);

	if (IPC_MSG_Q_BUFF_MAX < (nLen+12))
	{
		VK_Print("##### [%s][%d] over IPC_MSG_Q_BUFF_MAX....\n", __FUNCTION__, __LINE__);
		return ERR_FAIL;
	}

	VK_Print("##### [%s][%d] Socket FD :0x%x\n", __FUNCTION__, __LINE__, nFd);

	VK_memcpy(ucBuf, &nFd, sizeof(HINT32));
	VK_memcpy(ucBuf+4, &nLen, sizeof(HLONG));
	VK_memcpy(ucBuf+8, &nFlags, sizeof(HINT32));
	VK_memcpy(ucBuf+12, pucBuf, nLen);

	if (VK_IPC_MSG_Call(s_ulSubQId[0], IPC_SEC_CONV_FN_Send, &nErr, (void*)ucBuf, (nLen+12)) != VK_OK)
	{
		return ERR_FAIL;
	}

	VK_Print("##### [%s][%d] nErr :0x%x\n", __FUNCTION__, __LINE__, nErr);

	return nErr;
}



HLONG SECAPI_Recv(HINT32 nFd, const void *pucBuf, size_t nLen, HINT32 nFlags)
{
	HINT32 nErr;
	IpcSecRead_t stRead;

	VK_memset(&stRead, 0, sizeof(IpcSecRead_t));

	if (IPC_MSG_Q_BUFF_MAX < sizeof(IpcSecRead_t))
	{
		VK_Print("over IPC_MSG_Q_BUFF_MAX....\n");
		return ERR_FAIL;
	}

	stRead.fd = nFd;
	stRead.ulSize = nLen;
	stRead.ulFlag = nFlags;

	VK_Print("##### [%s][%d] nFd(%d), ulSize(%d) \n", __FUNCTION__, __LINE__, stRead.fd, stRead.ulSize);

	if (VK_IPC_MSG_Call(s_ulSubQId[0], IPC_SEC_CONV_FN_Recv, &nErr, (void*)&stRead, sizeof(IpcSecRead_t)) != VK_OK)
	{
		return ERR_FAIL;
	}

	if (nErr != -1)
	{
		local_secapi_File_Read("TempFileForRead", 0, nErr, pucBuf);
	}

	VK_Print("##### [%s][%d] nErr :0x%x\n", __FUNCTION__, __LINE__, nErr);

	return nErr;
}


HINT32 SECAPI_Open(const HINT8 *pucPath, HINT32 nFlags)
{
	HINT32 nErr;
	IpcSecFileOpen_t stFileOpen;
	HUINT32 ulLen;

	VK_memset(&stFileOpen, 0, sizeof(IpcSecFileOpen_t));

	if (IPC_MSG_Q_BUFF_MAX < sizeof(IpcSecFileOpen_t))
	{
		VK_Print("over IPC_MSG_Q_BUFF_MAX....\n");
		return ERR_FAIL;
	}

	if (pucPath != NULL)
	{
		ulLen = VK_strlen(pucPath);
		if(ulLen > IPC_PATH_MAX_SIZE)
		{
			ulLen = IPC_PATH_MAX_SIZE;
			VK_Print("##### [%s][%d] File Name:%s, length:%d \n", __FUNCTION__, __LINE__, stFileOpen.ucPath, ulLen);
		}
		VK_strncpy(&stFileOpen.ucPath, pucPath, ulLen);
	}

	stFileOpen.nFlags = nFlags;

	VK_Print("##### [%s][%d] File Name:%s, length:%d \n", __FUNCTION__, __LINE__, stFileOpen.ucPath, ulLen);

	if (VK_IPC_MSG_Call(s_ulSubQId[0], IPC_SEC_CONV_FN_Open, &nErr, (void*)&stFileOpen, sizeof(IpcSecFileOpen_t)) != VK_OK)
	{
		return ERR_FAIL;
	}

	VK_Print("##### [%s][%d] nErr :0x%x\n", __FUNCTION__, __LINE__, nErr);

	return nErr;
}


HINT32 SECAPI_Open2(const HINT8 *pucPath, HINT32 nFlags, mode_t mode)
{
	HINT32 nErr;
	IpcSecFileOpen_t stFileOpen;
	HUINT32 ulLen;

	VK_memset(&stFileOpen, 0, sizeof(IpcSecFileOpen_t));

	if (IPC_MSG_Q_BUFF_MAX < sizeof(IpcSecFileOpen_t))
	{
		VK_Print("over IPC_MSG_Q_BUFF_MAX....\n");
		return ERR_FAIL;
	}

	if (pucPath != NULL)
	{
		ulLen = VK_strlen(pucPath);
		if(ulLen > IPC_PATH_MAX_SIZE)
		{
			ulLen = IPC_PATH_MAX_SIZE;
			VK_Print("##### [%s][%d] File Name:%s, length:%d \n", __FUNCTION__, __LINE__, stFileOpen.ucPath, ulLen);
		}
		VK_strncpy(&stFileOpen.ucPath, pucPath, ulLen);
	}

	stFileOpen.nFlags = nFlags;
	stFileOpen.mode = mode;

	VK_Print("##### [%s][%d] File Name:%s, length:%d \n", __FUNCTION__, __LINE__, stFileOpen.ucPath, ulLen);

	if (VK_IPC_MSG_Call(s_ulSubQId[0], IPC_SEC_CONV_FN_Open2, &nErr, (void*)&stFileOpen, sizeof(IpcSecFileOpen_t)) != VK_OK)
	{
		return ERR_FAIL;
	}

	VK_Print("##### [%s][%d] nErr :0x%x\n", __FUNCTION__, __LINE__, nErr);

	return nErr;
}


HINT32 SECAPI_Close(HINT32 nFd)
{
	HINT32 nRet;

	VK_Print("##### [%s][%d] nFd :0x%x\n", __FUNCTION__, __LINE__, nFd);

	if (VK_IPC_MSG_Call(s_ulSubQId[0], IPC_SEC_CONV_FN_Close, &nRet, (void*)&nFd, sizeof(HINT32)) != VK_OK)
	{
		return ERR_FAIL;
	}

	VK_Print("##### [%s][%d] nErr :0x%x\n", __FUNCTION__, __LINE__, nRet);

	return nRet;
}


HINT32 SECAPI_Lseek(HINT32 fd, off_t pos, HINT32 origin)
{
	HINT32 nErr;
	IpcSecLseek_t stLseek;

	VK_memset(&stLseek, 0, sizeof(IpcSecLseek_t));

	if (IPC_MSG_Q_BUFF_MAX < sizeof(IpcSecLseek_t))
	{
		VK_Print("over IPC_MSG_Q_BUFF_MAX....\n");
		return ERR_FAIL;
	}

	stLseek.fd = fd;
	stLseek.pos = pos;
	stLseek.origin = origin;

	VK_Print("##### [%s][%d] fd(%d), pos(%ld), origin(%d) \n", __FUNCTION__, __LINE__, stLseek.fd, stLseek.pos, stLseek.origin);

	if (VK_IPC_MSG_Call(s_ulSubQId[0], IPC_SEC_CONV_FN_Lseek, &nErr, (void*)&stLseek, sizeof(IpcSecLseek_t)) != VK_OK)
	{
		return ERR_FAIL;
	}

	return nErr;
}


HINT32 SECAPI_Read(HINT32 nFd, HUINT8 *pucBuf, HUINT32 ulSize)
{
	HINT32 nErr;
	IpcSecRead_t stRead;

	VK_memset(&stRead, 0, sizeof(IpcSecRead_t));

	if (IPC_MSG_Q_BUFF_MAX < sizeof(IpcSecRead_t))
	{
		VK_Print("over IPC_MSG_Q_BUFF_MAX....\n");
		return ERR_FAIL;
	}

	stRead.fd = nFd;
	stRead.ulSize = ulSize;

	VK_Print("##### [%s][%d] nFd(%d), ulSize(%d) \n", __FUNCTION__, __LINE__, stRead.fd, stRead.ulSize);

	if (VK_IPC_MSG_Call(s_ulSubQId[0], IPC_SEC_CONV_FN_Read, &nErr, (void*)&stRead, sizeof(IpcSecRead_t)) != VK_OK)
	{
		return ERR_FAIL;
	}

	if (nErr != -1)
	{
		local_secapi_File_Read("TempFileForRead", 0, stRead.ulSize, pucBuf);
	}
	return nErr;
}


HLONG SECAPI_Write(HINT32 nFd, const void *pvBuf, HLONG nLen)
{
	HLONG nErr = 0;
	HINT8 ucBuf[IPC_MSG_Q_BUFF_MAX];

	VK_memset(ucBuf, 0, IPC_MSG_Q_BUFF_MAX);

	if (IPC_MSG_Q_BUFF_MAX < (nLen+8))
	{
		VK_Print("over IPC_MSG_Q_BUFF_MAX....\n");
		return ERR_FAIL;
	}

	VK_memcpy(ucBuf, &nFd, sizeof(HINT32));
	VK_memcpy(ucBuf+4, &nLen, sizeof(HLONG));
	VK_memcpy(ucBuf+8, pvBuf, nLen);

	if (VK_IPC_MSG_Call(s_ulSubQId[0], IPC_SEC_CONV_FN_Write, &nErr, (void*)ucBuf, (nLen+8)) != VK_OK)
	{
		return ERR_FAIL;
	}

	return nErr;
}


HINT32 SECAPI_Rename(const char *pucOld, const char *pucNew)
{
	HLONG nErr = 0;
	IpcSecFileCtl_t stFileCtl;

	VK_memset(&stFileCtl, 0, sizeof(IpcSecFileCtl_t));
	VK_strncpy(stFileCtl.ucPath, pucOld, IPC_PATH_MAX_SIZE);
	VK_strncpy(stFileCtl.ucNewPath, pucNew, IPC_PATH_MAX_SIZE);

	if (VK_IPC_MSG_Call(s_ulSubQId[0], IPC_SEC_CONV_FN_Rename, &nErr, (void*)&stFileCtl,  sizeof(IpcSecFileCtl_t)) != VK_OK)
	{
		return ERR_FAIL;
	}

	return nErr;
}


HINT32 SECAPI_Vfork(HUINT8 *pucProcName, HINT32 *pnPid)
{
	HINT32 nErr;
	IpcSecVfork_t stVfork;

	if ( (pucProcName == NULL) || (pnPid == NULL) )
	{
		return ERR_FAIL;
	}

	VK_memset(&stVfork, 0, sizeof(IpcSecVfork_t));

	if (IPC_MSG_Q_BUFF_MAX < sizeof(IpcSecVfork_t))
	{
		VK_Print("over IPC_MSG_Q_BUFF_MAX....\n");
		return ERR_FAIL;
	}

	VK_memcpy(stVfork.ucProcName, pucProcName, IPC_PATH_MAX_SIZE);

	VK_Print("SECAPI_Vfork : Proc_Name = %s\n", pucProcName);

	if (VK_IPC_MSG_Call(s_ulSubQId[0], IPC_SEC_CONV_FN_Vfork, &nErr, (void*)&stVfork, sizeof(IpcSecVfork_t)) != VK_OK)
	{
		return ERR_FAIL;
	}

	VK_Print("\SECAPI_Vfork : Pid = 0x%x\n",  stVfork.nPid);

	*pnPid = stVfork.nPid;

	return nErr;
}


HINT32 SECAPI_HddPwrCtrl(DI_HDD_PWR pwr)
{
	HINT32 nErr;

	if (VK_IPC_MSG_Call(s_ulSubQId[0], IPC_SEC_CONV_FN_HddPwrCtrl, &nErr, (void*)&pwr, sizeof(DI_HDD_PWR)) != VK_OK)
	{
		return ERR_FAIL;
	}

	return nErr;
}


// 2nd internal HDD (detachable HDD)만 power control 할때
HINT32 SECAPI_Hdd2PwrCtrl(DI_HDD_PWR pwr)
{
	HINT32 nErr;

	if (VK_IPC_MSG_Call(s_ulSubQId[0], IPC_SEC_CONV_FN_Hdd2PwrCtrl, &nErr, (void*)&pwr, sizeof(DI_HDD_PWR)) != VK_OK)
	{
		return ERR_FAIL;
	}

	return nErr;
}


HINT32 SECAPI_UsbPwrCtrl(DI_USB_PWR_t pwr)
{
	HINT32 nErr;

	if (VK_IPC_MSG_Call(s_ulSubQId[0], IPC_SEC_CONV_FN_UsbPwrCtrl, &nErr, (void*)&pwr, sizeof(DI_USB_PWR_t)) != VK_OK)
	{
		return ERR_FAIL;
	}

	return nErr;
}



HINT32 SECAPI_UsbHostConPwrCtrl(DI_USB_PWR_t pwr)
{
	HINT32 nErr;

	if (VK_IPC_MSG_Call(s_ulSubQId[0], IPC_SEC_CONV_FN_UsbHostConPwrCtrl, &nErr, (void*)&pwr, sizeof(DI_USB_PWR_t)) != VK_OK)
	{
		return ERR_FAIL;
	}

	return nErr;
}


HINT32 SECAPI_UsbStop(void)
{
	HINT32 nErr;

	if (VK_IPC_MSG_Call(s_ulSubQId[0], IPC_SEC_CONV_FN_UsbStop, &nErr, NULL, 0) != VK_OK)
	{
		return ERR_FAIL;
	}

	return nErr;
}


#if (1)
HINT32 SECAPI_FsGetDeviceFileName(DI_FS_StorDevIdx_t eDevIdx, HUINT8 *szFileName)
{
	HINT32 nErr;
	IpcSecDevName_t stDevName;

	VK_memset(&stDevName, 0, sizeof(IpcSecDevName_t));
	stDevName.eDevIdx = eDevIdx;

	VK_Print("\n\n\t =============> SECAPI_FsGetDeviceFileName : eDevIdx = %d\n", eDevIdx);

	if (VK_IPC_MSG_Call(s_ulSubQId[0], IPC_SEC_CONV_FN_GetDevFileName, &nErr, (void*)&stDevName, sizeof(IpcSecDevName_t)) != VK_OK)
	{
		return ERR_FAIL;
	}

	if (nErr == ERR_OK)
	{
		VK_Print("\t =============> SECAPI_FsGetDeviceFileName : szFileName = %d\n", stDevName.szFileName);
		VK_strcpy(szFileName, stDevName.szFileName);
	}

	return nErr;
}


HINT32 SECAPI_FsGetDeviceFileFullName(DI_FS_StorDevIdx_t eDevIdx, HUINT8 *szFileName)
{
	HINT32 nErr;
	IpcSecDevName_t stDevName;

	VK_memset(&stDevName, 0, sizeof(IpcSecDevName_t));
	stDevName.eDevIdx = eDevIdx;

	VK_Print("\n\n\t =============> SECAPI_FsGetDeviceFileFullName : eDevIdx = %d\n", eDevIdx);

	if (VK_IPC_MSG_Call(s_ulSubQId[0], IPC_SEC_CONV_FN_GetDevFileFullName, &nErr, (void*)&stDevName, sizeof(IpcSecDevName_t)) != VK_OK)
	{
		return ERR_FAIL;
	}

	if (nErr == ERR_OK)
	{
		VK_Print("\t =============> SECAPI_FsGetDeviceFileFullName : szFileName = %d\n", stDevName.szFileName);
		VK_strcpy(szFileName, stDevName.szFileName);
	}

	return nErr;
}


HINT32 SECAPI_FsGetPartitionFileName(DI_FS_StorDevIdx_t eDevIdx, HUINT32 ulPartitionIdx, HUINT8 *szFileName, HUINT8 nSizeOfFileName)
{
	HINT32 nErr;
	IpcSecPartitionName_t stPartName;

	VK_memset(&stPartName, 0, sizeof(IpcSecPartitionName_t));
	stPartName.eDevIdx = eDevIdx;
	stPartName.ulPartitionIdx = ulPartitionIdx;
	stPartName.nSizeOfFileName = nSizeOfFileName;

	VK_Print("\n\n\t =============> SECAPI_FsGetPartitionFileName : eDevIdx = %d, ulPartitionIdx = %d, nSizeOfFileName = %d", eDevIdx, ulPartitionIdx, nSizeOfFileName);

	if (VK_IPC_MSG_Call(s_ulSubQId[0], IPC_SEC_CONV_FN_GetPartitionFileName, &nErr, (void*)&stPartName, sizeof(IpcSecPartitionName_t)) != VK_OK)
	{
		return ERR_FAIL;
	}

	if (nErr == ERR_OK)
	{
		VK_Print("\t =============> SECAPI_FsGetPartitionFileName : szFileName = %d\n", stPartName.szFileName);
		VK_strncpy(szFileName, stPartName.szFileName, nSizeOfFileName);
	}

	return nErr;
}


HINT32 SECAPI_FsGetPartitionFileFullName(DI_FS_StorDevIdx_t eDevIdx, HUINT32 ulPartitionIdx, HUINT8 *szFileName)
{
	HINT32 nErr;
	IpcSecPartitionName_t stPartName;

	VK_memset(&stPartName, 0, sizeof(IpcSecPartitionName_t));
	stPartName.eDevIdx = eDevIdx;
	stPartName.ulPartitionIdx = ulPartitionIdx;

	VK_Print("\n\n\t =============> SECAPI_FsGetPartitionFileFullName : eDevIdx = %d, ulPartitionIdx = %d, nSizeOfFileName = %d", eDevIdx, ulPartitionIdx);

	if (VK_IPC_MSG_Call(s_ulSubQId[0], IPC_SEC_CONV_FN_GetPartitionFileFullName, &nErr, (void*)&stPartName, sizeof(IpcSecPartitionName_t)) != VK_OK)
	{
		return ERR_FAIL;
	}

	if (nErr == ERR_OK)
	{
		VK_Print("\t =============> SECAPI_FsGetPartitionFileFullName : szFileName = %d\n", stPartName.szFileName);
		VK_strcpy(szFileName, stPartName.szFileName);
	}

	return nErr;
}


HINT32 SECAPI_FsIsFileExist(HCHAR *pathname)
{
	HINT32 nErr;
	IpcSecFileCtl_t stFileCtl;

	VK_memset(&stFileCtl, 0, sizeof(IpcSecFileCtl_t));
	VK_memcpy(stFileCtl.ucPath, pathname, IPC_PATH_MAX_SIZE);

	VK_Print("\n\n\t =============> SECAPI_FsIsFileExist : pathname = %s", pathname);

	if (VK_IPC_MSG_Call(s_ulSubQId[0], IPC_SEC_CONV_FN_IsFileExist, &nErr, (void*)&stFileCtl, sizeof(IpcSecFileCtl_t)) != VK_OK)
	{
		return 0;
	}

	VK_Print("\t =============> SECAPI_FsIsFileExist : nErr = %d", nErr);

	return nErr;
}


HINT32 SECAPI_FsIsHDDExist(HCHAR *devname)
{
	HINT32 nErr;
	IpcSecFileCtl_t stFileCtl;

	VK_memset(&stFileCtl, 0, sizeof(IpcSecFileCtl_t));
	VK_memcpy(stFileCtl.ucPath, devname, IPC_PATH_MAX_SIZE);

	VK_Print("\n\n\t =============> SECAPI_FsIsHDDExist : devname = %s", devname);

	if (VK_IPC_MSG_Call(s_ulSubQId[0], IPC_SEC_CONV_FN_IsHddExist, &nErr, (void*)&stFileCtl, sizeof(IpcSecFileCtl_t)) != VK_OK)
	{
		return 0;
	}

	VK_Print("\t =============> SECAPI_FsIsHDDExist : nErr = %d", nErr);

	return nErr;
}


HINT32 SECAPI_FsIsDirExist(HCHAR *dirname)
{
	HINT32 nErr;
	IpcSecFileCtl_t stFileCtl;

	VK_memset(&stFileCtl, 0, sizeof(IpcSecFileCtl_t));
	VK_memcpy(stFileCtl.ucPath, dirname, IPC_PATH_MAX_SIZE);

	VK_Print("\n\n\t =============> SECAPI_FsIsDirExist : dirname = %s", dirname);

	if (VK_IPC_MSG_Call(s_ulSubQId[0], IPC_SEC_CONV_FN_IsDirExist, &nErr, (void*)&stFileCtl, sizeof(IpcSecFileCtl_t)) != VK_OK)
	{
		return 0;
	}

	VK_Print("\t =============> SECAPI_FsIsDirExist : nErr = %d", nErr);

	return nErr;
}


HINT32 SECAPI_FsMkdir(HCHAR *dirname)
{
	HINT32 nErr;
	IpcSecFileCtl_t stFileCtl;

	VK_memset(&stFileCtl, 0, sizeof(IpcSecFileCtl_t));
	VK_memcpy(stFileCtl.ucPath, dirname, IPC_PATH_MAX_SIZE);

	VK_Print("\n\n\t =============> SECAPI_FsMkdir : dirname = %s", dirname);

	if (VK_IPC_MSG_Call(s_ulSubQId[0], IPC_SEC_CONV_FN_DiMkDir, &nErr, (void*)&stFileCtl, sizeof(IpcSecFileCtl_t)) != VK_OK)
	{
		return ERR_FAIL;
	}

	VK_Print("\t =============> SECAPI_FsMkdir : nErr = %d", nErr);

	return nErr;
}


HINT32 SECAPI_FsGetVolumeUuid(DI_FS_StorDevIdx_t eDevIdx, HUINT32 ulPartIdx, HUINT8 *buf, HUINT32 bufsize, HUINT32 *uuidsize)
{
	HINT32 nErr;
	IpcSecPartitionName_t stPartName;

	VK_memset(&stPartName, 0, sizeof(IpcSecPartitionName_t));
	stPartName.eDevIdx = eDevIdx;
	stPartName.ulPartitionIdx = ulPartIdx;
	stPartName.nSizeOfFileName = bufsize;

	VK_Print("\n\n\t =============> SECAPI_FsGetVolumeUuid : eDevidx = %d, ulPartIdx = %d, bufsize = %d\n", eDevIdx, ulPartIdx, bufsize);

	if (VK_IPC_MSG_Call(s_ulSubQId[0], IPC_SEC_CONV_FN_GetVolumeUuid, &nErr, (void*)&stPartName, sizeof(IpcSecPartitionName_t)) != VK_OK)
	{
		return ERR_FAIL;
	}

	if (nErr == ERR_OK)
	{
		if (buf)
		{
			//VK_Print("\t =============> SECAPI_FsGetVolumeUuid : buff = %s\n", stPartName.szFileName);
			VK_memcpy(buf, stPartName.szFileName, bufsize);
		}

		if (uuidsize)
		{
			//VK_Print("\t =============> SECAPI_FsGetVolumeUuid : nUuIdSize = %d\n", stPartName.nUuIdSize);
			*uuidsize = stPartName.nUuIdSize;
		}
	}

	return nErr;
}


HINT32 SECAPI_FsGetVolumeNtfsUuid(DI_FS_StorDevIdx_t eDevIdx, HUINT32 ulPartIdx, HUINT8 *buf, HUINT32 size)
{
	HINT32 nErr;
	IpcSecPartitionName_t stPartName;

	VK_memset(&stPartName, 0, sizeof(IpcSecPartitionName_t));
	stPartName.eDevIdx = eDevIdx;
	stPartName.ulPartitionIdx = ulPartIdx;
	stPartName.nSizeOfFileName = size;

	VK_Print("\n\n\t =============> SECAPI_FsGetVolumeNtfsUuid : eDevidx = %d, ulPartIdx = %d, bufsize = %d\n", eDevIdx, ulPartIdx, size);

	if (VK_IPC_MSG_Call(s_ulSubQId[0], IPC_SEC_CONV_FN_GetVolumeNtfsUuid, &nErr, (void*)&stPartName, sizeof(IpcSecPartitionName_t)) != VK_OK)
	{
		return ERR_FAIL;
	}

	if (nErr == ERR_OK)
	{
		if (buf)
		{
			VK_Print("\t =============> SECAPI_FsGetVolumeNtfsUuid : buff = %s\n", stPartName.szFileName);
			VK_memcpy(buf, stPartName.szFileName, size);
		}
	}

	return nErr;
}


HINT32 SECAPI_FsMountPartition(DI_FS_StorDevIdx_t eDevIdx, HUINT32 ulPartIdx, HUINT8 *szDstMountDir, DI_FileSystemType_t eFsType)
{
	HINT32 nErr;
	IpcSecDevMbr_t stDevMbr;

	VK_memset(&stDevMbr, 0, sizeof(IpcSecDevMbr_t));
	stDevMbr.eDevIdx = eDevIdx;
	stDevMbr.ulPartitionIdx = ulPartIdx;
	VK_memcpy(stDevMbr.data.szDstMountDir, szDstMountDir, IPC_PATH_MAX_SIZE);
	stDevMbr.eFsType = eFsType;

	VK_Print("\n\n\t =============> SECAPI_FsMountPartition : eDevidx = %d, ulPartIdx = %d, szDstMountDir = %s, FsType = %d\n", eDevIdx, ulPartIdx, szDstMountDir, eFsType);

	if (VK_IPC_MSG_Call(s_ulSubQId[0], IPC_SEC_CONV_FN_MountPartition, &nErr, (void*)&stDevMbr, sizeof(IpcSecDevMbr_t)) != VK_OK)
	{
		return ERR_FAIL;
	}

	VK_Print("\t =============> SECAPI_FsMountPartition : nErr = %d\n", nErr);

	return nErr;
}


HINT32 SECAPI_FsUmountPartition(DI_FS_StorDevIdx_t eDevIdx, HUINT32 ulPartIdx, DI_FileSystemType_t eFsType)
{
	HINT32 nErr;
	IpcSecDevMbr_t stDevMbr;

	VK_memset(&stDevMbr, 0, sizeof(IpcSecDevMbr_t));
	stDevMbr.eDevIdx = eDevIdx;
	stDevMbr.ulPartitionIdx = ulPartIdx;
	stDevMbr.eFsType = eFsType;

	VK_Print("\n\n\t =============> SECAPI_FsUmountPartition : eDevidx = %d, ulPartIdx = %d, FsType = %d\n", eDevIdx, ulPartIdx, eFsType);

	if (VK_IPC_MSG_Call(s_ulSubQId[0], IPC_SEC_CONV_FN_UmountPartition, &nErr, (void*)&stDevMbr, sizeof(IpcSecDevMbr_t)) != VK_OK)
	{
		return ERR_FAIL;
	}

	VK_Print("\t =============> SECAPI_FsUmountPartition : nErr = %d\n", nErr);

	return nErr;
}


HINT32 SECAPI_FsHddDstStart(void)
{
	HINT32 nErr;

	VK_Print("\n\n\t =============> SECAPI_FsHddDstStart : Start\n");

	if (VK_IPC_MSG_Call(s_ulSubQId[0], IPC_SEC_CONV_FN_HddDstStart, &nErr, (void*)NULL, 0) != VK_OK)
	{
		return ERR_FAIL;
	}

	VK_Print("\t =============> SECAPI_FsHddDstStart : end\n");

	return nErr;
}



HINT32 SECAPI_FsOpenDevMbrBuf(DI_FS_StorDevIdx_t eDevIdx)
{
	HINT32 nErr;
	IpcSecDevMbr_t stDevMbr;

	VK_memset(&stDevMbr, 0, sizeof(IpcSecDevMbr_t));
	stDevMbr.eDevIdx = eDevIdx;

	VK_Print("\n\n\t =============> SECAPI_FsOpenDevMbrBuf : eDevIdx = %d\n", eDevIdx);

	if (VK_IPC_MSG_Call(s_ulSubQId[0], IPC_SEC_CONV_FN_OpenDevMbrBuf, &nErr, (void*)&stDevMbr, sizeof(IpcSecDevMbr_t)) != VK_OK)
	{
		return ERR_FAIL;
	}

	VK_Print("\t =============> SECAPI_FsOpenDevMbrBuf : nErr = %d\n", nErr);

	return nErr;
}


HINT32 SECAPI_FsDeletePartitionInDevMbrBuf(DI_FS_StorDevIdx_t eDevIdx, HUINT32 ulPartIdx)
{
	HINT32 nErr;
	IpcSecDevMbr_t stDevMbr;

	VK_memset(&stDevMbr, 0, sizeof(IpcSecDevMbr_t));
	stDevMbr.eDevIdx = eDevIdx;
	stDevMbr.ulPartitionIdx = ulPartIdx;

	VK_Print("\n\n\t =============> SECAPI_FsDeletePartitionInDevMbrBuf : eDevIdx = %d, ulPartIdx = %d\n", eDevIdx, ulPartIdx);

	if (VK_IPC_MSG_Call(s_ulSubQId[0], IPC_SEC_CONV_FN_DeletePartitionInDevMbrBuf, &nErr, (void*)&stDevMbr, sizeof(IpcSecDevMbr_t)) != VK_OK)
	{
		return ERR_FAIL;
	}

	VK_Print("\t =============> SECAPI_FsDeletePartitionInDevMbrBuf : nErr = %d\n", nErr);

	return nErr;
}


HINT32 SECAPI_FsDeleteAllPartitionInDevMbrBuf(DI_FS_StorDevIdx_t eDevIdx)
{
	HINT32 nErr;
	IpcSecDevMbr_t stDevMbr;

	VK_memset(&stDevMbr, 0, sizeof(IpcSecDevMbr_t));
	stDevMbr.eDevIdx = eDevIdx;

	VK_Print("\n\n\t =============> SECAPI_FsDeleteAllPartitionInDevMbrBuf : eDevIdx = %d\n", eDevIdx);

	if (VK_IPC_MSG_Call(s_ulSubQId[0], IPC_SEC_CONV_FN_DeleteAllPartitionInDevMbrBuf, &nErr, (void*)&stDevMbr, sizeof(IpcSecDevMbr_t)) != VK_OK)
	{
		return ERR_FAIL;
	}

	VK_Print("\t =============> SECAPI_FsDeleteAllPartitionInDevMbrBuf : nErr = %d\n", nErr);

	return nErr;
}


HINT32 SECAPI_FsUpdatePartitionInDevMbrBuf(DI_FS_StorDevIdx_t eDevIdx, HUINT32 ulPartIdx, HUINT32 ulStartUnit, HUINT32 ulEndUnit, DI_FileSystemType_t eFsType)
{
	HINT32 nErr;
	IpcSecDevMbr_t stDevMbr;

	VK_memset(&stDevMbr, 0, sizeof(IpcSecDevMbr_t));
	stDevMbr.eDevIdx = eDevIdx;
	stDevMbr.ulPartitionIdx = ulPartIdx;
	stDevMbr.ulStartUnit = ulStartUnit;
	stDevMbr.ulEndUnit = ulEndUnit;
	stDevMbr.eFsType = eFsType;

	VK_Print("\n\n\t =============> SECAPI_FsUpdatePartitionInDevMbrBuf : eDevIdx = %d, ulPartIdx = %d, ulStartUnit = %d, ulEndUnit = %d, FsType = %d\n", eDevIdx, ulPartIdx, ulStartUnit, ulEndUnit, eFsType);

	if (VK_IPC_MSG_Call(s_ulSubQId[0], IPC_SEC_CONV_FN_UpdatePartitionInDevMbrBuf, &nErr, (void*)&stDevMbr, sizeof(IpcSecDevMbr_t)) != VK_OK)
	{
		return ERR_FAIL;
	}

	VK_Print("\t =============> SECAPI_FsUpdatePartitionInDevMbrBuf : nErr = %d\n", nErr);

	return nErr;
}


HINT32 SECAPI_FsSyncDevMbrBuf(DI_FS_StorDevIdx_t eDevIdx)
{
	HINT32 nErr;
	IpcSecDevMbr_t stDevMbr;

	VK_memset(&stDevMbr, 0, sizeof(IpcSecDevMbr_t));
	stDevMbr.eDevIdx = eDevIdx;

	VK_Print("\n\n\t =============> SECAPI_FsSyncDevMbrBuf : eDevIdx = %d\n", eDevIdx);

	if (VK_IPC_MSG_Call(s_ulSubQId[0], IPC_SEC_CONV_FN_SyncDevMbrBuf, &nErr, (void*)&stDevMbr, sizeof(IpcSecDevMbr_t)) != VK_OK)
	{
		return ERR_FAIL;
	}


	VK_Print("\t =============> SECAPI_FsSyncDevMbrBuf : nErr = %d\n", nErr);

	return nErr;
}


HINT32 SECAPI_FsCloseDevMbrBuf(DI_FS_StorDevIdx_t eDevIdx)
{
	HINT32 nErr;
	IpcSecDevMbr_t stDevMbr;

	VK_memset(&stDevMbr, 0, sizeof(IpcSecDevMbr_t));
	stDevMbr.eDevIdx = eDevIdx;

	VK_Print("\n\n\t =============> SECAPI_FsCloseDevMbrBuf : eDevIdx = %d\n", eDevIdx);

	if (VK_IPC_MSG_Call(s_ulSubQId[0], IPC_SEC_CONV_FN_CloseDevMbrBuf, &nErr, (void*)&stDevMbr, sizeof(IpcSecDevMbr_t)) != VK_OK)
	{
		return ERR_FAIL;
	}

	VK_Print("\t =============> SECAPI_FsCloseDevMbrBuf : nErr = %d\n", nErr);

	return nErr;
}


HINT32 SECAPI_FsGetPartitionFromDeviceMbr(DI_FS_StorDevIdx_t eDevIdx, DI_FS_PartitionList_t *pstPartList)
{
	HINT32 nErr;
	IpcSecPartitionList_t stPartitionList;

	VK_memset(&stPartitionList, 0, sizeof(IpcSecPartitionList_t));
	stPartitionList.eDevIdx = eDevIdx;

	VK_Print("\n\n\t =============> SECAPI_FsGetPartitionFromDeviceMbr : eDevIdx = %d\n", eDevIdx);

	if (VK_IPC_MSG_Call(s_ulSubQId[0], IPC_SEC_CONV_FN_GetPartitionFromDeviceMbr, &nErr, (void*)&stPartitionList, sizeof(IpcSecPartitionList_t)) != VK_OK)
	{
		return ERR_FAIL;
	}

	if (nErr == ERR_OK)
	{
		VK_Print("\t =============> SECAPI_FsGetPartitionFromDeviceMbr : eDevIdx = %d\n", stPartitionList.stPartitionList.eDevIdx);
		VK_memcpy(pstPartList, &stPartitionList.stPartitionList, sizeof(DI_FS_PartitionList_t));
	}

	return nErr;
}


HINT32 SECAPI_FsEraseAllPartitionInDeviceMbr(DI_FS_StorDevIdx_t eDevIdx)
{
	HINT32 nErr;
	IpcSecPartitionList_t stPartitionList;

	VK_memset(&stPartitionList, 0, sizeof(IpcSecPartitionList_t));
	stPartitionList.eDevIdx = eDevIdx;

	VK_Print("\n\n\t =============> SECAPI_FsEraseAllPartitionInDeviceMbr : eDevIdx = %d\n", eDevIdx);

	if (VK_IPC_MSG_Call(s_ulSubQId[0], IPC_SEC_CONV_FN_EraseAllPartitionInDeviceMbr, &nErr, (void*)&stPartitionList, sizeof(IpcSecPartitionList_t)) != VK_OK)
	{
		return ERR_FAIL;
	}

	VK_Print("\t =============> SECAPI_FsEraseAllPartitionInDeviceMbr : nErr = %d\n", nErr);

	return nErr;
}


HINT32 SECAPI_FsInitFormatList(DI_FS_FormatList_t *pstList)
{
	HINT32 nErr;
	IpcSecFormatList_t stFormatList;

	VK_memset(&stFormatList, 0, sizeof(IpcSecFormatList_t));

	VK_Print("\n\n\t =============> SECAPI_FsInitFormatList\n");

	if (VK_IPC_MSG_Call(s_ulSubQId[0], IPC_SEC_CONV_FN_InitFormatList, &nErr, (void*)&stFormatList, sizeof(IpcSecFormatList_t)) != VK_OK)
	{
		return ERR_FAIL;
	}

	return nErr;
}


HINT32 SECAPI_FsAddFormatItemToList(DI_FS_FormatList_t *pstList, DI_FS_FormatItem_t *pstItem)
{
	HINT32 nErr;
	IpcSecFormatList_t stFormatList;

	VK_memset(&stFormatList, 0, sizeof(IpcSecFormatList_t));
	VK_memcpy(&stFormatList.stFormatItem, pstItem, sizeof(DI_FS_FormatItem_t));

	VK_Print("\n\n\t =============> SECAPI_FsAddFormatItemToList\n");
	VK_Print("\t =============>  SECAPI_FsAddFormatItemToList : eDevIdx = %d\n", stFormatList.stFormatItem.eDevIdx);
	VK_Print("\t =============>  SECAPI_FsAddFormatItemToList : ePartFsType = %d\n", stFormatList.stFormatItem.ePartFsType);
	VK_Print("\t =============>  SECAPI_FsAddFormatItemToList : szMountPath = %s\n", stFormatList.stFormatItem.szMountPath);
	//VK_Print("\t =============>  SECAPI_FsAddFormatItemToList : szVolumeName = %s\n", stFormatList.stFormatItem.szVolumeName);
	VK_Print("\t =============>  SECAPI_FsAddFormatItemToList : ulOption = %d\n", stFormatList.stFormatItem.ulOption);
	VK_Print("\t =============>  SECAPI_FsAddFormatItemToList : ulPartitionNo = %d\n", stFormatList.stFormatItem.ulPartitionNo);

	if (VK_IPC_MSG_Call(s_ulSubQId[0], IPC_SEC_CONV_FN_AddFormatItemToList, &nErr, (void*)&stFormatList, sizeof(IpcSecFormatList_t)) != VK_OK)
	{
		return ERR_FAIL;
	}

	VK_Print("\t =============> SECAPI_FsAddFormatItemToList : nErr = %d\n", nErr);

	return nErr;
}


HINT32 SECAPI_FsClearFormatList(DI_FS_FormatList_t *pstList)
{
	HINT32 nErr;
	IpcSecFormatList_t stFormatList;

	VK_memset(&stFormatList, 0, sizeof(IpcSecFormatList_t));

	VK_Print("\n\n\t =============> SECAPI_FsClearFormatList\n");

	if (VK_IPC_MSG_Call(s_ulSubQId[0], IPC_SEC_CONV_FN_ClearFormatList, &nErr, (void*)&stFormatList, sizeof(IpcSecFormatList_t)) != VK_OK)
	{
		return ERR_FAIL;
	}

	VK_Print("\t =============> SECAPI_FsClearFormatList : nErr = %d\n", nErr);

	return nErr;
}


HINT32 SECAPI_FsFormatLists(DI_FS_FormatList_t *pstFormatList, HUINT32 nonblock)
{
	HINT32 nErr;
	IpcSecFormatList_t stFormatList;

	VK_memset(&stFormatList, 0, sizeof(IpcSecFormatList_t));
	stFormatList.nonblock = nonblock;

	VK_Print("\n\n\t =============> SECAPI_FsFormatLists : nonblock = %d\n", nonblock);

	if (VK_IPC_MSG_Call(s_ulSubQId[0], IPC_SEC_CONV_FN_FormatLists, &nErr, (void*)&stFormatList, sizeof(IpcSecFormatList_t)) != VK_OK)
	{
		return ERR_FAIL;
	}

	VK_Print("\t =============> SECAPI_FsFormatLists : nErr = %d\n", nErr);

	return nErr;
}


HINT32 SECAPI_FsGetMountedPartitionStatus(DI_FS_StorDevIdx_t eDevIdx, HUINT32 ulPartIdx, DI_FileSystemType_t eFsType, HUINT64 *pullTotalKbSize, HUINT64 *pullUsedKbSize, HBOOL *pbWritable)
{
	HINT32 nErr;
	IpcSecMountdInfo_t stMountedInfo;

	VK_memset(&stMountedInfo, 0, sizeof(IpcSecMountdInfo_t));
	stMountedInfo.eDevIdx = eDevIdx;
	stMountedInfo.ulPartIdx = ulPartIdx;
	stMountedInfo.eFsType = eFsType;

	VK_Print("\n\n\t =============> SECAPI_FsGetMountedPartitionStatus : eDevIdx = %d, ulPartIdx = %d, FsType = %d\n", eDevIdx, ulPartIdx, eFsType);

	if (VK_IPC_MSG_Call(s_ulSubQId[0], IPC_SEC_CONV_FN_GetMountedPartitionStatus, &nErr, (void*)&stMountedInfo, sizeof(IpcSecMountdInfo_t)) != VK_OK)
	{
		return ERR_FAIL;
	}

	if (nErr == ERR_OK)
	{
		if (pullTotalKbSize)
		{
			VK_Print("\t =============> SECAPI_FsGetMountedPartitionStatus :ullTotalKbSize = %d\n",  stMountedInfo.ullTotalKbSize);
			*pullTotalKbSize = stMountedInfo.ullTotalKbSize;
		}
		if (pullUsedKbSize)
		{
			VK_Print("\t =============> SECAPI_FsGetMountedPartitionStatus :ullUsedKbSize = %d\n",  stMountedInfo.ullUsedKbSize);
			*pullUsedKbSize = stMountedInfo.ullUsedKbSize;
		}
		if (pbWritable)
		{
			VK_Print("\t =============> SECAPI_FsGetMountedPartitionStatus :bWritable = %d\n",  stMountedInfo.bWritable);
			*pbWritable = stMountedInfo.bWritable;
		}
	}

	return nErr;
}


HINT32 SECAPI_Register_FsHddDstCallback(pfnDI_HDD_DSTNotify pfnDstNotify)
{
	HINT32 nErr;

	CB_HddDstNotify = pfnDstNotify;

	if (VK_IPC_MSG_Call(s_ulSubQId[0], IPC_SEC_CONV_FN_HddDstCbNoti, &nErr, (void*)NULL, 0) != VK_OK)
	{
		return ERR_FAIL;
	}

	return nErr;
}


HINT32 SECAPI_Register_FsFormatProgressCallback(DI_FS_Event_Callback_Func pfEvtCallback)
{
	HINT32 nErr;

	CB_FsEventNotify = pfEvtCallback;

	if (VK_IPC_MSG_Call(s_ulSubQId[0], IPC_SEC_CONV_FN_HddFormatCbNoti, &nErr, (void*)NULL, 0) != VK_OK)
	{
		return ERR_FAIL;
	}

	return nErr;
}
#endif


#if (1)
HINT32 SECAPI_EncrFormat(DI_FS_StorDevIdx_t eDevIdx, HUINT32 ulPartIdx, char *szPassword)
{
	HINT32 nErr;
	IpcSecDevMbr_t stDevMbr;

	VK_memset(&stDevMbr, 0, sizeof(IpcSecDevMbr_t));
	stDevMbr.eDevIdx = eDevIdx;
	stDevMbr.ulPartitionIdx = ulPartIdx;
	if (szPassword != NULL)
		VK_memcpy(stDevMbr.szPassword, szPassword, IPC_PATH_MAX_SIZE);

	VK_Print("\n\n\t =============> SECAPI_EncrFormat : eDevIdx = %d, partIdx = %d\n", eDevIdx, ulPartIdx);

	if (VK_IPC_MSG_Call(s_ulSubQId[0], IPC_SEC_CONV_FN_FncrFormat, &nErr, (void*)&stDevMbr, sizeof(IpcSecDevMbr_t)) != VK_OK)
	{
		return ERR_FAIL;
	}

	VK_Print("\t =============> SECAPI_EncrFormat : nErr = %d\n", nErr);

	return nErr;

}


HINT32 SECAPI_EncrOpen(DI_FS_StorDevIdx_t eDevIdx, HUINT32 ulPartIdx, char *szMapperDeviceName, char *szPassword)
{
	HINT32 nErr;
	IpcSecDevMbr_t stDevMbr;

	VK_memset(&stDevMbr, 0, sizeof(IpcSecDevMbr_t));
	stDevMbr.eDevIdx = eDevIdx;
	stDevMbr.ulPartitionIdx = ulPartIdx;
	if (szMapperDeviceName != NULL)
		VK_memcpy(stDevMbr.data.szMapper, szMapperDeviceName, IPC_PATH_MAX_SIZE);
	if (szPassword != NULL)
			VK_memcpy(stDevMbr.szPassword, szPassword, IPC_PATH_MAX_SIZE);

	VK_Print("\n\n\t =============> SECAPI_EncrOpen : eDevIdx = %d, partIdx = %d\n", eDevIdx, ulPartIdx);

	if (VK_IPC_MSG_Call(s_ulSubQId[0], IPC_SEC_CONV_FN_FncrOpen, &nErr, (void*)&stDevMbr, sizeof(IpcSecDevMbr_t)) != VK_OK)
	{
		return ERR_FAIL;
	}

	VK_Print("\t =============> SECAPI_EncrOpen : nErr = %d\n", nErr);

	return nErr;

}


HINT32 SECAPI_EncrClose(DI_FS_StorDevIdx_t eDevIdx, HUINT32 ulPartIdx)
{
	HINT32 nErr;
	IpcSecDevMbr_t stDevMbr;

	VK_memset(&stDevMbr, 0, sizeof(IpcSecDevMbr_t));
	stDevMbr.eDevIdx = eDevIdx;
	stDevMbr.ulPartitionIdx = ulPartIdx;

	VK_Print("\n\n\t =============> SECAPI_EncrClose : eDevIdx = %d, partIdx = %d\n", eDevIdx, ulPartIdx);

	if (VK_IPC_MSG_Call(s_ulSubQId[0], IPC_SEC_CONV_FN_FncrClose, &nErr, (void*)&stDevMbr, sizeof(IpcSecDevMbr_t)) != VK_OK)
	{
		return ERR_FAIL;
	}

	VK_Print("\t =============> SECAPI_EncrClose : nErr = %d\n", nErr);

	return nErr;

}


HINT32 SECAPI_EncrMakeFs(DI_FS_StorDevIdx_t eDevIdx, HUINT32 ulPartIdx, char *szVolumeLable, DI_FileSystemType_t eFsType)
{
	HINT32 nErr;
	IpcSecDevMbr_t stDevMbr;

	VK_memset(&stDevMbr, 0, sizeof(IpcSecDevMbr_t));
	stDevMbr.eDevIdx = eDevIdx;
	stDevMbr.ulPartitionIdx = ulPartIdx;
	if (szVolumeLable != NULL)
		VK_memcpy(stDevMbr.data.szVolumn, szVolumeLable, IPC_PATH_MAX_SIZE);
	stDevMbr.eFsType = eFsType;

	VK_Print("\n\n\t =============> SECAPI_EncrMakeFs : eDevIdx = %d, partIdx = %d, eFsType = %d\n", eDevIdx, ulPartIdx, eFsType);

	if (VK_IPC_MSG_Call(s_ulSubQId[0], IPC_SEC_CONV_FN_FncrMakeFs, &nErr, (void*)&stDevMbr, sizeof(IpcSecDevMbr_t)) != VK_OK)
	{
		return ERR_FAIL;
	}

	VK_Print("\t =============> SECAPI_EncrMakeFs : nErr = %d\n", nErr);

	return nErr;

}


HINT32 SECAPI_EncrMount(DI_FS_StorDevIdx_t eDevIdx, HUINT32 ulPartIdx, char *szMountDir, DI_FileSystemType_t eFsType)
{
	HINT32 nErr;
	IpcSecDevMbr_t stDevMbr;

	VK_memset(&stDevMbr, 0, sizeof(IpcSecDevMbr_t));
	stDevMbr.eDevIdx = eDevIdx;
	stDevMbr.ulPartitionIdx = ulPartIdx;
	if (szMountDir != NULL)
		VK_memcpy(stDevMbr.data.szDstMountDir, szMountDir, IPC_PATH_MAX_SIZE);
	stDevMbr.eFsType = eFsType;

	VK_Print("\n\n\t =============> SECAPI_EncrMount : eDevIdx = %d, partIdx = %d, eFsType = %d\n", eDevIdx, ulPartIdx, eFsType);

	if (VK_IPC_MSG_Call(s_ulSubQId[0], IPC_SEC_CONV_FN_FncrMount, &nErr, (void*)&stDevMbr, sizeof(IpcSecDevMbr_t)) != VK_OK)
	{
		return ERR_FAIL;
	}

	VK_Print("\t =============> SECAPI_EncrMount : nErr = %d\n", nErr);

	return nErr;

}


HINT32 SECAPI_EncrUmount(DI_FS_StorDevIdx_t eDevIdx, HUINT32 ulPartIdx)
{
	HINT32 nErr;
	IpcSecDevMbr_t stDevMbr;

	VK_memset(&stDevMbr, 0, sizeof(IpcSecDevMbr_t));
	stDevMbr.eDevIdx = eDevIdx;
	stDevMbr.ulPartitionIdx = ulPartIdx;

	VK_Print("\n\n\t =============> SECAPI_EncrUmount : eDevIdx = %d, partIdx = %d\n", eDevIdx, ulPartIdx);

	if (VK_IPC_MSG_Call(s_ulSubQId[0], IPC_SEC_CONV_FN_FncrUmount, &nErr, (void*)&stDevMbr, sizeof(IpcSecDevMbr_t)) != VK_OK)
	{
		return ERR_FAIL;
	}

	VK_Print("\t =============> SECAPI_EncrUmount : nErr = %d\n", nErr);

	return nErr;
}
#endif


HINT32 SECAPI_Regster_HddCallback(pfnDI_HDD_NotifyFUNC pfnNotify)
{
	HINT32 nErr;

 	CB_HddNotify = pfnNotify;

	if (VK_IPC_MSG_Call(s_ulSubQId[0], IPC_SEC_CONV_FN_HddCbNoti, &nErr, (void*)NULL, 0) != VK_OK)
	{
		return ERR_FAIL;
	}

	return nErr;
}


HINT32 SECAPI_Regster_UsbCallback(pfnDI_USB_NotifyFUNC pfnNotify)
{
	HINT32 nErr;

	CB_UsbNotify = pfnNotify;

	if (VK_IPC_MSG_Call(s_ulSubQId[0], IPC_SEC_CONV_FN_UsbCbNoti, &nErr, (void*)NULL, 0) != VK_OK)
	{
		return ERR_FAIL;
	}

	return nErr;
}


HINT32 SECAPI_Regster_WlanCallback(pfnDI_USB_NotifyFUNC pfnNotify)
{
	HINT32 nErr;

	CB_USB_WirelessNotify = pfnNotify;

	if (VK_IPC_MSG_Call(s_ulSubQId[0], IPC_SEC_CONV_FN_WlanCbNoti, &nErr, (void*)NULL, 0) != VK_OK)
	{
		return ERR_FAIL;
	}

	return nErr;
}


HINT32 SECAPI_Regster_HidCallback(const DI_USB_HID_DEVICE eDevices, pfnDI_USB_HID_Notify pfnNotify)
{
	HINT32 nErr = DDI_ERR_OK;

	/* 1. register USB Keyboard if it is required */
	if (eDevices & DI_USB_HID_DEVICE_USBKEYBOARD)
	{
		if (s_fnHIDUSBKeyboardCallback != NULL)
		{
			VK_Print( "[DI_USB_HID_RegisterHIDEventCallback] USB Keyboard is already registered.\n");
			nErr = DI_ERR_NO_RESOURCE;
		}
		else
		{
			s_fnHIDUSBKeyboardCallback = pfnNotify;
		}
	}

	/* 2. register USB Mouse if it is required */
	if (eDevices & DI_USB_HID_DEVICE_USBMOUSE)
	{
		if (s_fnHIDUSBMouseCallback != NULL)
		{
			VK_Print( "[DI_USB_HID_RegisterHIDEventCallback] USB Mouse is already registered.\n");
			nErr = DI_ERR_NO_RESOURCE;
		}
		else
		{
			s_fnHIDUSBMouseCallback = pfnNotify;
		}
	}

	/* 3. register USB Joystick if it is required */
	if (eDevices & DI_USB_HID_DEVICE_USBJOYSTICK)
	{
		if (s_fnHIDUSBJoystickCallback != NULL)
		{
			VK_Print( "[DI_USB_HID_RegisterHIDEventCallback] USB Joystick is already registered.\n");
			nErr = DI_ERR_NO_RESOURCE;
		}
		else
		{
			s_fnHIDUSBJoystickCallback = pfnNotify;
		}
	}

	if ( (nErr == DDI_ERR_OK)
		&& ((eDevices & DI_USB_HID_DEVICE_USBKEYBOARD)
			|| (eDevices & DI_USB_HID_DEVICE_USBMOUSE)
			|| (eDevices & DI_USB_HID_DEVICE_USBJOYSTICK)) )
	{
		if (VK_IPC_MSG_Call(s_ulSubQId[0], IPC_SEC_CONV_FN_HidCbNoti, &nErr, (void*)&eDevices, sizeof(DI_USB_HID_DEVICE)) != VK_OK)
		{
			return ERR_FAIL;
		}
	}
	else
	{
		nErr = DI_ERR_NO_RESOURCE;
	}

	return nErr;
}



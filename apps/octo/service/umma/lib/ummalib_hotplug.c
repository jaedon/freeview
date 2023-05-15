/*
 * @defgroup		DRV_HOTPLUG DRV_HOTPLUG : DRV_HOTPLUG module
 * @author			hs.choi
 * @note			DRV_HOTPLUG APIs
 * @brief			Define DRV_HOTPLUG APIs
 * @file 			drv_hotplug.c                           */

/*******************************************************************/
/**************************** Header Files *************************/
/*******************************************************************/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <fcntl.h>
#include <dirent.h>

#include <sys/socket.h>
#include <linux/netlink.h>
#include <linux/rtnetlink.h>

#include "vkernel.h"
#include "taskdef.h"

#include	"ummalib.h"


/*******************************************************************/
/****************************** define *****************************/
/*******************************************************************/
//#define DEBUG_PRINT_HOTPLUG_DATA
//#define DEBUG_PRINT_USB_INFO

#if defined(CONFIG_PROD_OS_EMULATOR)
#define hostent		emu_hostent
#define in_addr		emu_in_addr
#define sockaddr_in emu_sockaddr_in
#define sockaddr_nl emu_sockaddr_nl
#define iovec		emu_iovec
#define msghdr		emu_msghdr
#endif

#define DATA_NUM		256
#define MAX_PATH_NUM 		1024
#define BLOCK_DEV_NAME_CNT	10
#define MAX_USB_PORT_NUM	4
#define HC_NUM			4

#define AHCI_PLATFORM_DEV_PATH "/devices/platform"

/*******************************************************************/
/******************************* extern ******************************/
/*******************************************************************/

/*******************************************************************/
/****************************** typedef ****************************/
/*******************************************************************/
struct usb_device_info {
	int bDeviceClass;
	int bDeviceSubClass;
	int bDeviceProtocol;
	int bNumInterfaces;
	int busnum;
	int portnum;
	int dev_type;
	unsigned int idProduct;
	unsigned int idVendor;
	char physdevpath[16];

	int bInterfaceClass;
	int bInterfaceSubClass;
	int bInterfaceProtocol;
};

/* there are 3 types of ahci device in kernel 3.8.
    when probing ahci device, one of three types might be loaded or more.
    check all types of ahci device when hotplug occurs */
enum DRV_AHCI_TYPE_E {
	AHCI = 0,    /* standard platform ahci */
	IMX53_AHCI,  /* ahci on i.mx53 */
	STRICT_AHCI, /* delayed DMA engine start */
	AHCI_MAX
};

enum {
	BLOCK_ATA = 0,
	BLOCK_MMC,
	USB_DEVICE_WIRELESS,
	USB_DEVICE_MICROPHONE,
	USB_DEVICE_HID,
	USB_DEVICE_UNKNOWN
};

enum {
	ACTION_ADD = 0,
	ACTION_REMOVE,
	ACTION_CHANGE
};

typedef struct {
	char *ahci_devname;
	int nFirstHostNum;
} drv_ahci_type_t;

static drv_ahci_type_t stDrvAhciType[AHCI_MAX] = {
	{"ahci", -1},
	{"imx53-ahci", -1},
	{"strict-ahci", -1}
};

/*******************************************************************/
/************************ global variables *************************/
/*******************************************************************/

/*******************************************************************/
/************************ static variables *************************/
/*******************************************************************/
static unsigned long EventIds[UMMALIB_HP_EVT_NUM];
static unsigned long MsgIds[UMMALIB_HP_EVT_NUM];
static unsigned long ulHotplugEventId;
static struct usb_device_info tUsbInfo[MAX_USB_PORT_NUM];

STATIC pfUmmaLibHp_NotifyFUNC		s_pfUmmaLibHpNotiCb = NULL;

#if defined(DEBUG_PRINT_HOTPLUG_DATA)
static void ummalib_hotplug_Dump(char *buf, int len)
{
	int i;

	for (i = 0; i < len; i++)
	{
		if (buf[i] == 0x00) HLIB_CMD_Printf("\n");
		else HLIB_CMD_Printf("%c", buf[i]);
	}
	HLIB_CMD_Printf("\n");
}
#endif

static int ummalib_hotplug_ReadFile(char *path, char *rdata, int len)
{
	int fd;
	int ret=0;
	char *src=rdata;

	fd = open(path, O_RDONLY);
	if(fd < 0)
	{
		HxLOG_Debug("[%s] can't open %s\n",__FUNCTION__, path);
		return 0;
	}

	if((ret = read(fd,src,len)) < 0)
	{
		HxLOG_Error("[%s] can't read %s\n",__FUNCTION__, path);
		close(fd);
		return 0;
	}

	close(fd);
	return ret;
}

static char *ummalib_hotplug_FindString(char *buf, char *fstr, int len)
{
	char *src = buf;

	while (*src != 0x0)
	{
		if (strncmp(src, fstr, len) == 0) break;

		while (*src != 0x0)
		{
			src++;
		}

		src++;
	}

	if (*src == 0x0)
		return NULL;

	src += len;
	return src;
}

static int ummalib_hotplug_CheckUsbDevice(char *usbdev_path, int remove)
{
	int BaseClass;
	int	SubClass;
	int	Protocol;
	int len;
	int slot;
	char *p;
	char data[32];
	char path[128];
	char link[128];

	if (!usbdev_path)
		return -1;

	if (remove == TRUE)
	{
		p = strrchr(usbdev_path, (int)'/');
		for (slot=0; p && slot<MAX_USB_PORT_NUM; slot++)
		{
			if (!strncmp(tUsbInfo[slot].physdevpath, (char*)(p+1), strlen(p+1)))
			{
				tUsbInfo[slot].physdevpath[0] = 0; //remove
				return slot;
			}
		}

		if (!p || slot==MAX_USB_PORT_NUM)
		{
			return -1;
		}
	}

	for (slot=0; slot<MAX_USB_PORT_NUM; slot++)
	{
		/* find empty slot */
		if (tUsbInfo[slot].physdevpath[0] == 0)
		{
			break;
		}
	}
	if (slot == MAX_USB_PORT_NUM)
	{
		HxLOG_Error("[%s] slot is full\n", __FUNCTION__);
		return -1;
	}

	snprintf(path, sizeof(path), "/sys%s/device/bDeviceClass", usbdev_path);
	len = ummalib_hotplug_ReadFile(path, data, sizeof(data)-1);
	data[len] = 0;
	tUsbInfo[slot].bDeviceClass = strtol(data, NULL, 16);

	snprintf(path, sizeof(path), "/sys%s/device/bDeviceProtocol", usbdev_path);
	len = ummalib_hotplug_ReadFile(path, data, sizeof(data)-1);
	data[len] = 0;
	tUsbInfo[slot].bDeviceProtocol = strtol(data, NULL, 16);

	snprintf(path, sizeof(path), "/sys%s/device/bDeviceSubClass", usbdev_path);
	len = ummalib_hotplug_ReadFile(path, data, sizeof(data)-1);
	data[len] = 0;
	tUsbInfo[slot].bDeviceSubClass = strtol(data, NULL, 16);

	snprintf(path, sizeof(path), "/sys%s/device/bNumInterfaces", usbdev_path);
	len = ummalib_hotplug_ReadFile(path, data, sizeof(data)-1);
	data[len] = 0;
	tUsbInfo[slot].bNumInterfaces = strtol(data, NULL, 16);

	snprintf(path, sizeof(path), "/sys%s/device/idProduct", usbdev_path);
	len = ummalib_hotplug_ReadFile(path, data, sizeof(data)-1);
	data[len] = 0;
	tUsbInfo[slot].idProduct = strtol(data, NULL, 16);

	snprintf(path, sizeof(path), "/sys%s/device/idVendor", usbdev_path);
	len = ummalib_hotplug_ReadFile(path, data, sizeof(data)-1);
	data[len] = 0;
	tUsbInfo[slot].idVendor = strtol(data, NULL, 16);

	snprintf(path, sizeof(path), "/sys%s/device/busnum", usbdev_path);
	len = ummalib_hotplug_ReadFile(path, data, sizeof(data)-1);
	data[len] = 0;
	tUsbInfo[slot].busnum = strtol(data, NULL, 16);

	snprintf(path, sizeof(path), "/sys%s/device", usbdev_path);
	len = readlink(path, link, sizeof(link));
	if (len > 0 && len < (int)sizeof(link))
	{
		link[len] = 0;
		p = strrchr(link, (int)'/');
		if (!p || !strchr(p, '-'))
		{
			tUsbInfo[slot].portnum = 0;
		}
		else
		{
			p += 3;
			tUsbInfo[slot].portnum = strtol(p, NULL, 16);
		}
	}
	else
	{
		tUsbInfo[slot].portnum = 0;
	}

	/* we support only 1.0 interface */
	snprintf(path, sizeof(path), "/sys%s/device/%d-1:1.0/bInterfaceClass", usbdev_path, tUsbInfo[slot].busnum);
	len = ummalib_hotplug_ReadFile(path, data, sizeof(data)-1);
	data[len] = 0;
	tUsbInfo[slot].bInterfaceClass = strtol(data, NULL, 16);

	snprintf(path, sizeof(path), "/sys%s/device/%d-1:1.0/bInterfaceProtocol", usbdev_path, tUsbInfo[slot].busnum);
	len = ummalib_hotplug_ReadFile(path, data, sizeof(data)-1);
	data[len] = 0;
	tUsbInfo[slot].bInterfaceProtocol = strtol(data, NULL, 16);

	snprintf(path, sizeof(path), "/sys%s/device/%d-1:1.0/bInterfaceSubClass", usbdev_path,tUsbInfo[slot].busnum);
	len = ummalib_hotplug_ReadFile(path, data, sizeof(data)-1);
	data[len] = 0;
	tUsbInfo[slot].bInterfaceSubClass = strtol(data, NULL, 16);

	if (tUsbInfo[slot].bDeviceClass ==0 && \
		tUsbInfo[slot].bDeviceSubClass ==0 && \
		tUsbInfo[slot].bDeviceProtocol ==0)
	{
		/* if bDeviceClass, bDeviceSubClass and bDeviceProtocol are 0,
		   this device uses interface class
		*/
		BaseClass = tUsbInfo[slot].bInterfaceClass;
		SubClass = tUsbInfo[slot].bInterfaceSubClass;
		Protocol = tUsbInfo[slot].bInterfaceProtocol;
	}
	else
	{
		BaseClass = tUsbInfo[slot].bDeviceClass;
		SubClass = tUsbInfo[slot].bDeviceSubClass;
		Protocol = tUsbInfo[slot].bDeviceProtocol;
	}

	tUsbInfo[slot].dev_type = USB_DEVICE_UNKNOWN;
	/* Wireless
	   DeviceClass / InterfaceClass is not classified in USB Device/Interface Spec.
	   Only some of usb wireless devices are testified and under the conition, it is wireless device.
	*/

	if (tUsbInfo[slot].bInterfaceClass == 3)
	{
		tUsbInfo[slot].dev_type = USB_DEVICE_HID;
	}
	else if (tUsbInfo[slot].bInterfaceClass == 255 &&  tUsbInfo[slot].bInterfaceSubClass == 93 && tUsbInfo[slot].bInterfaceProtocol == 1)
	{
		tUsbInfo[slot].dev_type = USB_DEVICE_HID;
	}
	else if((tUsbInfo[slot].bDeviceClass == 0 || tUsbInfo[slot].bDeviceClass == 0x6 || tUsbInfo[slot].bDeviceClass == 0xFF /* for ar9271 */) && \
		(tUsbInfo[slot].bInterfaceClass == 0xFF))
	{
		tUsbInfo[slot].dev_type = USB_DEVICE_WIRELESS;
	}

//Joystick  baseclass : 0x03, 0xff면 제조사에서 정한것으로..

	/* Microphone.
	   see in http://www.usb.org/developers/defined_class
	*/
	else if((BaseClass == 0x1) && (SubClass == 0x1))
	{
		/*
		BaseClass : 0x01 - Audio Device
		SubClass : 0x01 - Microphone
		*/
		tUsbInfo[slot].dev_type = USB_DEVICE_MICROPHONE;
	}

	if (tUsbInfo[slot].dev_type != USB_DEVICE_UNKNOWN) {
		if ((p = strrchr(usbdev_path, (int)'/')))
		{
			snprintf(tUsbInfo[slot].physdevpath, sizeof(tUsbInfo[slot].physdevpath), "%s", (char*)(p+1));
		}
		else
		{
			slot = -1;
			goto END_FUNC;
		}
	}

	if (tUsbInfo[slot].dev_type == USB_DEVICE_UNKNOWN)
	{
		HxLOG_Debug("USB_DEVICE_UNKNOWN !!\n");
		slot = -1;
	}

END_FUNC:
#ifdef DEBUG_PRINT_USB_INFO
	HxLOG_Print("[%s] [%d]--------DEBUG_PRINT_USB_INFO------------------------\n",__FUNCTION__,__LINE__);
	HxLOG_Print("-- bDeviceClass : %d\n", tUsbInfo[slot].bDeviceClass);
	HxLOG_Print("-- bDeviceSubClass : %d\n", tUsbInfo[slot].bDeviceSubClass);
	HxLOG_Print("-- bDeviceProtocol : %d\n", tUsbInfo[slot].bDeviceProtocol);
	HxLOG_Print("-- bNumInterfaces : %d\n", tUsbInfo[slot].bNumInterfaces);
	HxLOG_Print("-- busnum : %d\n", tUsbInfo[slot].busnum);
	HxLOG_Print("-- portnum : %d\n", tUsbInfo[slot].portnum);
	HxLOG_Print("-- idProduct : %d\n", tUsbInfo[slot].idProduct);
	HxLOG_Print("-- idVendor : %d\n", tUsbInfo[slot].idVendor);
	HxLOG_Print("-- physdevpath : %s\n", tUsbInfo[slot].physdevpath);
	HxLOG_Print("-- bInterfaceClass : %d\n", tUsbInfo[slot].bInterfaceClass);
	HxLOG_Print("-- bInterfaceSubClass : %d\n", tUsbInfo[slot].bInterfaceSubClass);
	HxLOG_Print("-- bInterfaceProtocol : %d\n", tUsbInfo[slot].bInterfaceProtocol);
	HxLOG_Print("------------------------------------------------------------------\n");
#endif

	return slot;
}


UMMALIB_HP_DEV_EVT_e		ummalib_convDevType(int devType)
{
	UMMALIB_HP_DEV_EVT_e		eDevType = eUMMALIB_HP_MAX;

	switch(devType)
	{
		case USB_DEVICE_MICROPHONE:
			eDevType = eUMMALIB_HP_MIC;
			break;

		case USB_DEVICE_WIRELESS:
			eDevType = eUMMALIB_HP_WIRELESS;
			break;

		case BLOCK_MMC:
			eDevType = eUMMALIB_HP_SD;
			break;

		case BLOCK_ATA:
			eDevType = eUMMALIB_HP_HDD;
			break;

		case USB_DEVICE_HID:
			eDevType = eUMMALIB_HP_HID;
			break;

		default:
			eDevType = eUMMALIB_HP_MAX;
			break;

	}

	return eDevType;
}


static void ummalib_hotplug_SendEvent(int devtype, UMMALIB_HP_Info_t *tEvt)
{
	HUINT32 nResult = 0;

	switch(devtype) {
	case BLOCK_ATA:
		if (EventIds[eUMMALIB_HP_HDD])
		{
			VK_EVENT_Send(EventIds[eUMMALIB_HP_HDD], (unsigned long)tEvt);
			HxLOG_Debug("[%s] send hotplug event (device_type:%d)\n", __FUNCTION__, devtype);
		}
		if (MsgIds[eUMMALIB_HP_HDD])
		{
			nResult = VK_MSG_SendTimeout(MsgIds[eUMMALIB_HP_HDD], tEvt, sizeof(UMMALIB_HP_Info_t), 0);
			if (nResult != ERR_OK)
			{
				HxLOG_Error("[%s][%d] VK_MSG_SendTimeout err !\n",__FUNCTION__,__LINE__);
			}
			HxLOG_Debug("[%s] send hotplug message (device_type:%d)\n", __FUNCTION__, devtype);
		}
		break;
	case BLOCK_MMC:
		if (EventIds[eUMMALIB_HP_SD])
		{
			VK_EVENT_Send(EventIds[eUMMALIB_HP_SD], (unsigned long)tEvt);
			HxLOG_Debug("[%s] send hotplug event (device_type:%d)\n", __FUNCTION__, devtype);
		}
		if (MsgIds[eUMMALIB_HP_SD])
		{
			nResult = VK_MSG_SendTimeout(MsgIds[eUMMALIB_HP_SD], tEvt, sizeof(UMMALIB_HP_Info_t), 0);
			if (nResult != ERR_OK)
			{
				HxLOG_Error("[%s][%d] VK_MSG_SendTimeout err !\n",__FUNCTION__,__LINE__);
			}
			HxLOG_Debug("[%s] send hotplug message (device_type:%d)\n", __FUNCTION__, devtype);
		}
		break;
	case USB_DEVICE_WIRELESS:
		if (EventIds[eUMMALIB_HP_USB_WIRELESS])
		{
			VK_EVENT_Send(EventIds[eUMMALIB_HP_USB_WIRELESS],(unsigned long)tEvt);
			HxLOG_Debug("[%s] send hotplug event (device_type:%d)\n", __FUNCTION__, devtype);
			VK_TASK_Sleep(100);
		}
		if(MsgIds[eUMMALIB_HP_USB_WIRELESS])
		{
			nResult = VK_MSG_SendTimeout(MsgIds[eUMMALIB_HP_USB_WIRELESS], tEvt, sizeof(UMMALIB_HP_Info_t), 0);
			if (nResult != ERR_OK)
			{
				HxLOG_Error("[%s][%d] VK_MSG_SendTimeout err !\n",__FUNCTION__,__LINE__);
			}
			HxLOG_Debug("[%s] send hotplug message (device_type:%d)\n", __FUNCTION__, devtype);
			VK_TASK_Sleep(100);
		}
		break;
	case USB_DEVICE_MICROPHONE:
		if (EventIds[eUMMALIB_HP_MIC])
		{
			VK_EVENT_Send(EventIds[eUMMALIB_HP_MIC], (unsigned long)tEvt);
			HxLOG_Debug("[%s] send hotplug event (device_type:%d)\n", __FUNCTION__, devtype);
		}
		if (MsgIds[eUMMALIB_HP_MIC])
		{
			nResult = VK_MSG_SendTimeout(MsgIds[eUMMALIB_HP_MIC], tEvt, sizeof(UMMALIB_HP_Info_t), 0);
			if (nResult != ERR_OK)
			{
				HxLOG_Error("[%s][%d] VK_MSG_SendTimeout err !\n",__FUNCTION__,__LINE__);
			}
			HxLOG_Debug("[%s] send hotplug message (device_type:%d)\n", __FUNCTION__, devtype);
		}
		break;
	default:
		break;
	}
}

static void ummalib_hotplug_Parse(char *buf)
{
	int action;
	int i;
	int len;
	int iScsiNum;
	int port_num;
	int slot;
	char *subsystem;
	char *devpath;
	char *physdevpath;
	char *devtype;
	char *p;
	char path[256];
	UMMALIB_HP_Info_t hp_evt;


	/* Get action */
	if (!strncmp(buf, "add@" , 4))
	{
		action = ACTION_ADD;
	}
	else if (!strncmp(buf, "remove@", 7))
	{
		action = ACTION_REMOVE;
	}
	else
	{
		goto done;
	}

	/* Get env fields */
	subsystem = ummalib_hotplug_FindString(buf,"SUBSYSTEM=",10);
	devtype = ummalib_hotplug_FindString(buf, "DEVTYPE=", 8);
	devpath = ummalib_hotplug_FindString(buf, "DEVPATH=", 8);
	physdevpath = ummalib_hotplug_FindString(buf, "PHYSDEVPATH=", 12);

	if (!subsystem)
	{
		HxLOG_Error("[%s] unknown device\n", __FUNCTION__);
		goto done;
	}

	if (!strncmp(subsystem, "scsi", 4))
	{
		/* Find first ATA port number from scsi path "../hostX"
		   this is to decide first ata port number */
		if (devtype && (!strncmp(devtype, "scsi_host", 9) || !strncmp(devtype, "scsi_device", 11)))
		{
			for (i=0; i<AHCI_MAX && devpath; i++)
			{
				snprintf(path, sizeof(path), "%s/%s", AHCI_PLATFORM_DEV_PATH, stDrvAhciType[i].ahci_devname);
				len = strlen(path);
				if (!strncmp(devpath, path, len))
				{
					if ((p = strstr(devpath, "host")) && stDrvAhciType[i].nFirstHostNum < 0)
					{
						stDrvAhciType[i].nFirstHostNum = atoi(p+4);
					}
					break;
				}
			}
		}

		goto done;
	}

	/* Find block (disk) for callback.
	   in case of hdd, usb-storage, sd-card and so on */
	if (!strcmp(subsystem, "block"))
	{
		/* search ata device(hdd) */
		for (i=0; i<AHCI_MAX && physdevpath; i++)
		{
			snprintf(path, sizeof(path), "%s/%s", AHCI_PLATFORM_DEV_PATH, stDrvAhciType[i].ahci_devname);
			len = strlen(path);
			if (!(strncmp(physdevpath, path, len)))
			{
				if (stDrvAhciType[i].nFirstHostNum < 0)
				{
					/* host start number not decided */
					goto done;
				}

				if ((p = strstr(physdevpath, "host")))
				{
					iScsiNum = atoi(p+4);
				}
				else
				{
					goto done;
				}

				if (stDrvAhciType[i].nFirstHostNum == iScsiNum)
					port_num = 1;
				else
					port_num = iScsiNum - stDrvAhciType[i].nFirstHostNum + 1;

				len = strlen(devpath);
				if (len != BLOCK_DEV_NAME_CNT)
				{
					/* consider only "/block/sdx" which length is 10 */
					goto done;
				}

				/* make event */
				hp_evt.eConnEvt = (action == ACTION_ADD) ? eUMMALIB_HP_CONN : eUMMALIB_HP_DISCONN;
				hp_evt.nLen = len;
				hp_evt.nPortnum = port_num;
				snprintf(hp_evt.szDevpath, sizeof(hp_evt.szDevpath), "%s", devpath);

				/* send event */
				ummalib_hotplug_SendEvent(BLOCK_ATA, &hp_evt);

				goto done;
			}
		}

		/* search mmc(sd) card device */
		if (physdevpath && !strncmp(physdevpath, "/devices/platform/sdhci.0/mmc_host", 34))
		{
			/* Search block for MMC */
			if (devpath && !strncmp(devtype, "disk", 4))
			{
				hp_evt.eConnEvt = (action == ACTION_ADD) ? eUMMALIB_HP_CONN : eUMMALIB_HP_DISCONN;
				hp_evt.nPortnum = atoi(&devpath[13]) + 1;
				hp_evt.nLen = strlen(devpath);
				snprintf(hp_evt.szDevpath, sizeof(hp_evt.szDevpath), "%s", devpath);

				/* send event */
				ummalib_hotplug_SendEvent(BLOCK_MMC, &hp_evt);

				goto done;
			}
		}

		goto done;
	}

	/* for usb devices which has usb interface.
	   mic, speaker, keyboard, mount .... */
	if (!strncmp(subsystem, "usb_device", 10))
	{
		slot = ummalib_hotplug_CheckUsbDevice(devpath, (action==ACTION_REMOVE));
		if (slot < 0)
		{
			goto done;
		}

		hp_evt.eConnEvt = (action == ACTION_ADD) ? eUMMALIB_HP_CONN : eUMMALIB_HP_DISCONN;
		hp_evt.ulIdvendor=tUsbInfo[slot].idVendor;
		hp_evt.ulIdproduct=tUsbInfo[slot].idProduct;
		hp_evt.nPortnum=tUsbInfo[slot].portnum;
//		hp_evt.nBusnum = tUsbInfo[slot].nBusnum;

		HxLOG_Debug("[usb_device]\n");
		HxLOG_Debug(" + hp_evt.eConnEvt : %d, \n + hp_evt.ulIdvendor :%d, \n + hp_evt.ulIdproduct : %d, \n + hp_evt.nPortnum : %d\n", hp_evt.eConnEvt, hp_evt.ulIdvendor, hp_evt.ulIdproduct, hp_evt.nPortnum);
		HxLOG_Debug(" + dev_type : %d\n",  tUsbInfo[slot].dev_type);

		/* send event */
		if (s_pfUmmaLibHpNotiCb)
		{
			s_pfUmmaLibHpNotiCb((HUINT32)slot, ummalib_convDevType(tUsbInfo[slot].dev_type), &hp_evt);
		}

//		ummalib_hotplug_SendEvent(tUsbInfo[slot].dev_type, &hp_evt);
	}

done:
	return;
}

static void UMMALIB_Hotplug_Task(void *params)
{
	int ns = -1;
	struct sockaddr_nl sa;
	char buf[1024];
	struct iovec iov = { buf, sizeof(buf) };
	struct msghdr msg = { &sa, sizeof(sa), &iov, 1, NULL, 0, 0};
	int len = 1024;

	(void)params;

	VK_MEM_Memset(&sa, 0, sizeof(sa));
	sa.nl_family = AF_NETLINK;
	sa.nl_groups = RTMGRP_LINK | RTMGRP_IPV4_IFADDR;

	ns = socket(AF_NETLINK, SOCK_RAW, NETLINK_KOBJECT_UEVENT);
	if (ns < 0)
	{
		HxLOG_Error("[%s]%d socket()",__func__,__LINE__);
		return;
	}

	if (bind(ns, (struct sockaddr *)&sa, sizeof(sa)))
	{
		HxLOG_Error("[%s]%d bind()",__func__,__LINE__);
		close(ns);
		return;
	}

	while (1)
	{
		len = recvmsg(ns, &msg, 0);
		if (len < 0)
		{
			HxLOG_Error("[%s]%d recvmsg()",__func__,__LINE__);
		}
		else
		{
			ummalib_hotplug_Parse(buf);
		}

#if defined(DEBUG_PRINT_HOTPLUG_DATA)
		ummalib_hotplug_Dump(buf, len);
#endif
		fflush(stdout);
	}

	close(ns);

	return;
}

HERROR UMMALIB_Hotplug_EventRegister(UMMALIB_HP_DEV_EVT_e HpEvt, unsigned long EventId)
{
	if(HpEvt >= UMMALIB_HP_EVT_NUM)
	{
		HxLOG_Error("%s : Can't Register Event %d\n",__func__,HpEvt);
		return ERR_FAIL;
	}
	EventIds[HpEvt]=EventId;

	return ERR_OK;
}

HERROR UMMALIB_Hotplug_MsgRegister(UMMALIB_HP_DEV_EVT_e HpEvt, unsigned long MsgId)
{
	if (HpEvt >= UMMALIB_HP_EVT_NUM)
	{
		HxLOG_Error("[%s] Error. unknown hotplug type(HpEvt:%d)\n", __FUNCTION__, HpEvt);
		return ERR_FAIL;
	}

	MsgIds[HpEvt] = MsgId;

	return ERR_OK;
}

HERROR UMMALIB_Hotplug_ReadSysUsb(void)
{
	int slot;
	char usbdev_path[MAX_PATH_NUM];
	DIR *dir_info;
	UMMALIB_HP_Info_t hp_evt;
	struct dirent *dir_entry;

	dir_info = opendir("/sys/class/usb_device");
	if(dir_info == NULL)
		return ERR_FAIL;

	while((dir_entry = readdir(dir_info)) != NULL)
	{
		snprintf(usbdev_path, sizeof(usbdev_path), "/class/usb_device/%s", dir_entry->d_name);

		slot = ummalib_hotplug_CheckUsbDevice(usbdev_path, 0);
		if (slot < 0)
			continue;

		hp_evt.eConnEvt = eUMMALIB_HP_CONN;
		hp_evt.ulIdvendor=tUsbInfo[slot].idVendor;
		hp_evt.ulIdproduct=tUsbInfo[slot].idProduct;
		hp_evt.nPortnum=tUsbInfo[slot].portnum;
//		hp_evt.nBusnum = tUsbInfo[slot].busnum;

//		ummalib_hotplug_SendEvent(tUsbInfo[slot].dev_type, &hp_evt);
	}
	closedir(dir_info);

	return ERR_OK;
}

HERROR UMMALIB_Hotplug_Init(void)
{
	int ret;

	VK_MEM_Memset(EventIds, 0x00, sizeof(unsigned long)*UMMALIB_HP_EVT_NUM);
	VK_MEM_Memset(MsgIds, 0x00, sizeof(unsigned long)*UMMALIB_HP_EVT_NUM);
#if defined(CONFIG_PROD_OS_EMULATOR)
	//TODO; secma 포팅이후 삭제될 코드
	ret=VK_TASK_Create(UMMALIB_Hotplug_Task, 42, 2048, "UMMALIB_Hotplug_Task",(void *)0, &ulHotplugEventId, 0);
#else
	ret=VK_TASK_Create(UMMALIB_Hotplug_Task, HOTPLUG_TASK_PRIORITY, HOTPLUG_TASK_STACK_SIZE, "UMMALIB_Hotplug_Task",(void *)0, &ulHotplugEventId, 0);
#endif
	if(ret != VK_OK)
	{
		HxLOG_Error("%s[%d] : VK_TASK_Create() error\n",__func__,__LINE__);
		return ERR_FAIL;
	}
	else
	{
		VK_TASK_Start(ulHotplugEventId);
	}

	return ERR_OK;
}

HERROR UMMALIB_Hotplug_RegisterCallback(pfUmmaLibHp_NotifyFUNC pfnNotify)
{
	s_pfUmmaLibHpNotiCb = pfnNotify;
	return ERR_OK;
}


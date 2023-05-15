/*
 * @defgroup		DRV_HOTPLUG DRV_HOTPLUG : DRV_HOTPLUG module
 * @author			hs.choi
 * @note			DRV_HOTPLUG APIs
 * @brief			Define DRV_HOTPLUG APIs
 * @file 			drv_hotplug.c
 * @refurl			http://www.linux-usb.org/FAQ.html, http://www.linux-usb.org/USB-guide/book1.html
 */

/*******************************************************************/
/**************************** Header Files *************************/
/*******************************************************************/
#if defined(CONFIG_VK_STDLIB)
#else
#include 	<stdio.h>
#include 	<stdlib.h>
#include 	<string.h>
#endif
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

#include "drv_hotplug.h"


/*******************************************************************/
/****************************** define *****************************/
/*******************************************************************/
//#define DEBUG_PRINT_HOTPLUG_DATA
//#define DEBUG_PRINT_USB_INFO
//#define DI_HOTPLUG_DEBUG

#ifdef DI_HOTPLUG_DEBUG
#define PrintDebug(fmt, ...)	VK_DBG_Print (fmt, ## __VA_ARGS__)
#define PrintError(fmt, ...)	VK_DBG_Print (fmt, ## __VA_ARGS__)
#define PrintEnter()			VK_DBG_Print (fmt, ## __VA_ARGS__)
#define PrintExit()				VK_DBG_Print (fmt, ## __VA_ARGS__)
#else
#define PrintDebug(fmt, ...)	while (0) ((int (*)(char *, ...)) 0)
#define PrintError(fmt, ...)	VK_DBG_Print (fmt, ## __VA_ARGS__)
#define PrintEnter()			while (0) ((int (*)(char *, ...)) 0)
#define PrintExit()				while (0) ((int (*)(char *, ...)) 0)
#endif

#define DATA_NUM		256
#define MAX_PATH_NUM 		1024
#define BLOCK_DEV_NAME_CNT	10
#define MAX_USB_PORT_NUM	8
#define HC_NUM			4

#define AHCI_PLATFORM_DEV_PATH "/devices/platform"

#define DRV_HOTPLUG_SYS_USB_PATH "/sys/bus/usb/devices"

#define DRV_HOTPLUG_SYS_USB_DEVICE_BASE_PATH "/sys%s"

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
static unsigned long EventIds[HP_EVT_NUM];
static unsigned long MsgIds[HP_EVT_NUM];
static unsigned long ulHotplugEventId;
static struct usb_device_info tUsbInfo[MAX_USB_PORT_NUM];

#if defined(DEBUG_PRINT_HOTPLUG_DATA)
static void P_HOTPLUG_Dump(char *buf, int len)
{
	int i;

	for (i = 0; i < len; i++)
	{
		if (buf[i] == 0x00) VK_printf("\n");
		else VK_printf("%c", buf[i]);
	}
	VK_printf("\n");
}
#endif

static int P_HOTPLUG_ReadFile(char *path, char *rdata, int len)
{
	int fd;
	int ret=0;
	char *src=rdata;

	fd = open(path, O_RDONLY);
	if(fd < 0)
	{
		PrintDebug("[%s] can't open %s\n",__FUNCTION__, path);
		return 0;
	}

	if((ret = read(fd,src,len)) < 0)
	{
		PrintError("[%s] can't read %s\n",__FUNCTION__, path);
		close(fd);
		return 0;
	}

	close(fd);
	return ret;
}

static char *P_HOTPLUG_FindString(char *buf, char *fstr, int len)
{
	char *src = buf;

	while (*src != 0x0)
	{
		if (VK_strncmp(src, fstr, len) == 0) break;

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

static int P_HOTPLUG_CheckUsbDevice(char *usbdev_path, int remove)
{
	int BaseClass;
	int	SubClass;
	int	Protocol;
	int len;
	int slot;
	char *p;
	char data[32];
	char path[128];
	char base_path[128];

	if (!usbdev_path)
		return -1;

	if (remove)
	{
		p = strrchr(usbdev_path, (int)'/');
		for (slot=0; p && slot<MAX_USB_PORT_NUM; slot++)
		{
			if (!VK_strncmp(tUsbInfo[slot].physdevpath, (char*)(p+1), VK_strlen(p+1)))
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
		PrintError("[%s] slot is full\n", __FUNCTION__);
		return -1;
	}

	VK_snprintf(base_path, sizeof(base_path), usbdev_path);
	p = strrchr(base_path, (int)'/');
	if(p != NULL) *p = '\0';
	p = base_path;

	VK_snprintf(path, sizeof(path), DRV_HOTPLUG_SYS_USB_DEVICE_BASE_PATH"/bDeviceClass", p);
	len = P_HOTPLUG_ReadFile(path, data, sizeof(data)-1);
	data[len] = 0;
	tUsbInfo[slot].bDeviceClass = strtol(data, NULL, 16);

	VK_snprintf(path, sizeof(path), DRV_HOTPLUG_SYS_USB_DEVICE_BASE_PATH"/bDeviceProtocol", p);
	len = P_HOTPLUG_ReadFile(path, data, sizeof(data)-1);
	data[len] = 0;
	tUsbInfo[slot].bDeviceProtocol = strtol(data, NULL, 16);

	VK_snprintf(path, sizeof(path), DRV_HOTPLUG_SYS_USB_DEVICE_BASE_PATH"/bDeviceSubClass", p);
	len = P_HOTPLUG_ReadFile(path, data, sizeof(data)-1);
	data[len] = 0;
	tUsbInfo[slot].bDeviceSubClass = strtol(data, NULL, 16);

	VK_snprintf(path, sizeof(path), DRV_HOTPLUG_SYS_USB_DEVICE_BASE_PATH"/bNumInterfaces", p);
	len = P_HOTPLUG_ReadFile(path, data, sizeof(data)-1);
	data[len] = 0;
	tUsbInfo[slot].bNumInterfaces = strtol(data, NULL, 16);

	VK_snprintf(path, sizeof(path), DRV_HOTPLUG_SYS_USB_DEVICE_BASE_PATH"/idProduct", p);
	len = P_HOTPLUG_ReadFile(path, data, sizeof(data)-1);
	data[len] = 0;
	tUsbInfo[slot].idProduct = strtol(data, NULL, 16);

	VK_snprintf(path, sizeof(path), DRV_HOTPLUG_SYS_USB_DEVICE_BASE_PATH"/idVendor", p);
	len = P_HOTPLUG_ReadFile(path, data, sizeof(data)-1);
	data[len] = 0;
	tUsbInfo[slot].idVendor = strtol(data, NULL, 16);

	VK_snprintf(path, sizeof(path), DRV_HOTPLUG_SYS_USB_DEVICE_BASE_PATH"/busnum", p);
	len = P_HOTPLUG_ReadFile(path, data, sizeof(data)-1);
	data[len] = 0;
	tUsbInfo[slot].busnum = strtol(data, NULL, 16);

	VK_snprintf(path, sizeof(path), DRV_HOTPLUG_SYS_USB_DEVICE_BASE_PATH, p);

	p = strrchr(path, (int)'/');

	if (!p || !VK_strchr(p, '-'))
	{
		tUsbInfo[slot].portnum = 0;
	}
	else
	{
		p += 3;
		tUsbInfo[slot].portnum = strtol(p, NULL, 16);
	}

	/* we support only 1.0 interface */
	VK_snprintf(path, sizeof(path), DRV_HOTPLUG_SYS_USB_DEVICE_BASE_PATH"/bInterfaceClass", usbdev_path);
	len = P_HOTPLUG_ReadFile(path, data, sizeof(data)-1);
	data[len] = 0;
	tUsbInfo[slot].bInterfaceClass = strtol(data, NULL, 16);

	VK_snprintf(path, sizeof(path), DRV_HOTPLUG_SYS_USB_DEVICE_BASE_PATH"/bInterfaceProtocol", usbdev_path);
	len = P_HOTPLUG_ReadFile(path, data, sizeof(data)-1);
	data[len] = 0;
	tUsbInfo[slot].bInterfaceProtocol = strtol(data, NULL, 16);

	VK_snprintf(path, sizeof(path), DRV_HOTPLUG_SYS_USB_DEVICE_BASE_PATH"/bInterfaceSubClass", usbdev_path);
	len = P_HOTPLUG_ReadFile(path, data, sizeof(data)-1);
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
	if((tUsbInfo[slot].bDeviceClass == 0 || tUsbInfo[slot].bDeviceClass == 0x6
				|| tUsbInfo[slot].bDeviceClass == 0xFF /* for ar9271 */
				|| tUsbInfo[slot].bDeviceClass == 0xEF /* for SKT LG SD711 */)
			&& (tUsbInfo[slot].bInterfaceClass == 0xFF))
	{
		tUsbInfo[slot].dev_type = USB_DEVICE_WIRELESS;
	}

	/* Microphone.
	   see in http://www.usb.org/developers/defined_class
	 */
	if((BaseClass == 0x1) && (SubClass == 0x1))
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
			VK_snprintf(tUsbInfo[slot].physdevpath, sizeof(tUsbInfo[slot].physdevpath), "%s", (char*)(p+1));
		}
		else
		{
			return -1;
		}
	}

#ifdef DEBUG_PRINT_USB_INFO
	VK_printf("------------------------------------------------------------------\n");
	VK_printf("-- bDeviceClass : %d\n", tUsbInfo[slot].bDeviceClass);
	VK_printf("-- bDeviceSubClass : %d\n", tUsbInfo[slot].bDeviceSubClass);
	VK_printf("-- bDeviceProtocol : %d\n", tUsbInfo[slot].bDeviceProtocol);
	VK_printf("-- bNumInterfaces : %d\n", tUsbInfo[slot].bNumInterfaces);
	VK_printf("-- busnum : %d\n", tUsbInfo[slot].busnum);
	VK_printf("-- portnum : %d\n", tUsbInfo[slot].portnum);
	VK_printf("-- idProduct : %d\n", tUsbInfo[slot].idProduct);
	VK_printf("-- idVendor : %d\n", tUsbInfo[slot].idVendor);
	VK_printf("-- physdevpath : %s\n", (tUsbInfo[slot].physdevpath[0]==0) ? usbdev_path : tUsbInfo[slot].physdevpath);
	VK_printf("-- bInterfaceClass : %d\n", tUsbInfo[slot].bInterfaceClass);
	VK_printf("-- bInterfaceSubClass : %d\n", tUsbInfo[slot].bInterfaceSubClass);
	VK_printf("-- bInterfaceProtocol : %d\n", tUsbInfo[slot].bInterfaceProtocol);
	VK_printf("------------------------------------------------------------------\n");
#endif

	if (tUsbInfo[slot].dev_type == USB_DEVICE_UNKNOWN)
	{
		return -1;
	}

	return slot;
}

static void P_HOTPLUG_SendEvent(int devtype, hp_evt_t *tEvt)
{
	int ret = 0;

	switch(devtype) {
		case BLOCK_ATA:
			if (EventIds[DRV_HP_HDD])
			{
				ret = VK_EVENT_Send(EventIds[DRV_HP_HDD], (unsigned long)tEvt);
				if (ret != VK_OK)
				{
					PrintError("[%s] VK_MSG_Send Error!\n", __FUNCTION__);
				}
				else
				{
					PrintDebug("[%s] send hotplug message (dev_type:%d)\n", __FUNCTION__, devtype);
				}
			}
			if (MsgIds[DRV_HP_HDD])
			{
				ret = VK_MSG_SendTimeout(MsgIds[DRV_HP_HDD], tEvt, sizeof(hp_evt_t), 0);
				if (ret != VK_OK)
				{
					PrintError("[%s] VK_MSG_Send Error!\n", __FUNCTION__);
				}
				else
				{
					PrintDebug("[%s] send hotplug message (dev_type:%d)\n", __FUNCTION__, devtype);
				}
			}
			break;
		case BLOCK_MMC:
			if (EventIds[DRV_HP_SD])
			{
				ret = VK_EVENT_Send(EventIds[DRV_HP_SD], (unsigned long)tEvt);
				if (ret != VK_OK)
				{
					PrintError("[%s] VK_MSG_Send Error!\n", __FUNCTION__);
				}
				else
				{
					PrintDebug("[%s] send hotplug message (dev_type:%d)\n", __FUNCTION__, devtype);
				}
			}
			if (MsgIds[DRV_HP_SD])
			{
				ret = VK_MSG_SendTimeout(MsgIds[DRV_HP_SD], tEvt, sizeof(hp_evt_t), 0);
				if (ret != VK_OK)
				{
					PrintError("[%s] VK_MSG_Send Error!\n", __FUNCTION__);
				}
				else
				{
					PrintDebug("[%s] send hotplug message (dev_type:%d)\n", __FUNCTION__, devtype);
				}
			}
			break;
		case USB_DEVICE_WIRELESS:
			if (EventIds[DRV_HP_USB_WIRELESS])
			{
				ret = VK_EVENT_Send(EventIds[DRV_HP_USB_WIRELESS],(unsigned long)tEvt);
				if (ret != VK_OK)
				{
					PrintError("[%s] VK_MSG_Send Error!\n", __FUNCTION__);
				}
				else
				{
					PrintDebug("[%s] send hotplug message (dev_type:%d)\n", __FUNCTION__, devtype);
				}
				VK_TASK_Sleep(100);
			}
			if(MsgIds[DRV_HP_USB_WIRELESS])
			{
				ret = VK_MSG_SendTimeout(MsgIds[DRV_HP_USB_WIRELESS], tEvt, sizeof(hp_evt_t), 0);
				if (ret != VK_OK)
				{
					PrintError("[%s] VK_MSG_Send Error!\n", __FUNCTION__);
				}
				else
				{
					PrintDebug("[%s] send hotplug message (dev_type:%d)\n", __FUNCTION__, devtype);
				}
				VK_TASK_Sleep(100);
			}
			break;
		case USB_DEVICE_MICROPHONE:
			if (EventIds[DRV_HP_MIC])
			{
				ret = VK_EVENT_Send(EventIds[DRV_HP_MIC], (unsigned long)tEvt);
				if (ret != VK_OK)
				{
					PrintError("[%s] VK_MSG_Send Error!\n", __FUNCTION__);
				}
				else
				{
					PrintDebug("[%s] send hotplug message (dev_type:%d)\n", __FUNCTION__, devtype);
				}
			}
			if (MsgIds[DRV_HP_MIC])
			{
				ret = VK_MSG_SendTimeout(MsgIds[DRV_HP_MIC], tEvt, sizeof(hp_evt_t), 0);
				if (ret != VK_OK)
				{
					PrintError("[%s] VK_MSG_Send Error!\n", __FUNCTION__);
				}
				else
				{
					PrintDebug("[%s] send hotplug message (dev_type:%d)\n", __FUNCTION__, devtype);
				}
			}
			break;
		default:
			break;
	}
}

static void P_HOTPLUG_Parse(char *buf)
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
	char *blockdev;
	char path[256];
	hp_evt_t hp_evt;

	/* Get action */
	if (!VK_strncmp(buf, "add@" , 4))
	{
		action = ACTION_ADD;
	}
	else if (!VK_strncmp(buf, "remove@", 7))
	{
		action = ACTION_REMOVE;
	}
	else
	{
		goto done;
	}

	/* Get env fields */
	subsystem = P_HOTPLUG_FindString(buf,"SUBSYSTEM=",10);
	devtype = P_HOTPLUG_FindString(buf, "DEVTYPE=", 8);
	devpath = P_HOTPLUG_FindString(buf, "DEVPATH=", 8);
	physdevpath = P_HOTPLUG_FindString(buf, "PHYSDEVPATH=", 12);

	if (!subsystem)
	{
		PrintError("[%s] unknown device\n", __FUNCTION__);
		goto done;
	}

	if (!VK_strncmp(subsystem, "scsi", 4))
	{
		/* Find first ATA port number from scsi path "../hostX"
		   this is to decide first ata port number */
		if (devtype && (!VK_strncmp(devtype, "scsi_host", 9) || !VK_strncmp(devtype, "scsi_device", 11)))
		{
			for (i=0; i<AHCI_MAX && devpath; i++)
			{
				VK_snprintf(path, sizeof(path), "%s/%s", AHCI_PLATFORM_DEV_PATH, stDrvAhciType[i].ahci_devname);
				len = VK_strlen(path);
				if (!VK_strncmp(devpath, path, len))
				{
					if ((p = VK_strstr(devpath, "host")) && stDrvAhciType[i].nFirstHostNum < 0)
					{
						stDrvAhciType[i].nFirstHostNum = VK_atoi(p+4);
					}
					break;
				}
			}
		}

		goto done;
	}

	/* Find block (disk) for callback.
	   in case of hdd, usb-storage, sd-card and so on */
	if (!VK_strcmp(subsystem, "block"))
	{
		/* search ata device(hdd) */
		for (i=0; i<AHCI_MAX && physdevpath; i++)
		{
			VK_snprintf(path, sizeof(path), "%s/%s", AHCI_PLATFORM_DEV_PATH, stDrvAhciType[i].ahci_devname);
			len = VK_strlen(path);
			if (!(VK_strncmp(physdevpath, path, len)))
			{
				if (stDrvAhciType[i].nFirstHostNum < 0)
				{
					/* host start number not decided */
					goto done;
				}

				if ((p = VK_strstr(physdevpath, "host")))
				{
					iScsiNum = VK_atoi(p+4);
				}
				else
				{
					goto done;
				}

				if (stDrvAhciType[i].nFirstHostNum == iScsiNum)
					port_num = 1;
				else
					port_num = iScsiNum - stDrvAhciType[i].nFirstHostNum + 1;

				blockdev = VK_strstr(devpath, "/block");
				if(blockdev != NULL)
				{
					len = VK_strlen(blockdev);
					if (len != BLOCK_DEV_NAME_CNT)
					{
						/* consider only "/block/sdx" which length is 10 */
						goto done;
					}

					/* make event */
					hp_evt.eConnEvt = (action == ACTION_ADD) ? DRV_HP_CONN : DRV_HP_DISCONN;
					hp_evt.len = len;
					hp_evt.portnum = port_num;
					VK_snprintf(hp_evt.devpath, sizeof(hp_evt.devpath), "%s", blockdev);

					/* send event */
					P_HOTPLUG_SendEvent(BLOCK_ATA, &hp_evt);
				}

				goto done;
			}
		}

		/* search mmc(sd) card device */
		if (physdevpath && !VK_strncmp(physdevpath, "/devices/platform/sdhci.0/mmc_host", 34))
		{
			/* Search block for MMC */
			if (devpath && devtype && !VK_strncmp(devtype, "disk", 4))
			{
				hp_evt.eConnEvt = (action == ACTION_ADD) ? DRV_HP_CONN : DRV_HP_DISCONN;
				hp_evt.portnum = VK_atoi(&devpath[13]) + 1;
				hp_evt.len = VK_strlen(devpath);
				VK_snprintf(hp_evt.devpath, sizeof(hp_evt.devpath), "%s", devpath);

				/* send event */
				P_HOTPLUG_SendEvent(BLOCK_MMC, &hp_evt);

				goto done;
			}
		}

		goto done;
	}

	/* for usb devices which has usb interface.
	   mic, speaker, keyboard, mount .... */
	if ( !VK_strncmp(subsystem, "usb_device", 10) ||
			((!VK_strncmp(subsystem, "usb", 3) && devtype && !VK_strncmp(devtype, "usb_interface", 13))))
	{
		slot = P_HOTPLUG_CheckUsbDevice(devpath, (action==ACTION_REMOVE));
		if (slot < 0)
		{
			goto done;
		}

		hp_evt.eConnEvt = (action == ACTION_ADD) ? DRV_HP_CONN : DRV_HP_DISCONN;
		hp_evt.idvendor=tUsbInfo[slot].idVendor;
		hp_evt.idproduct=tUsbInfo[slot].idProduct;
		hp_evt.portnum=tUsbInfo[slot].portnum;
		hp_evt.busnum = tUsbInfo[slot].busnum;

		/* send event */
		PrintDebug("[%s] usb_device (action:%s), InterfaceClass=%d, DeviceClass=%d, dev_type(%d)\n", __FUNCTION__, action ? "disconnected" : "connected" , tUsbInfo[slot].bInterfaceClass, tUsbInfo[slot].bDeviceClass, tUsbInfo[slot].dev_type);
		P_HOTPLUG_SendEvent(tUsbInfo[slot].dev_type, &hp_evt);
	}

done:
	return;
}


static void DRV_HOTPLUG_Task(void *params)
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
		PrintError("[%s]%d socket()",__func__,__LINE__);
		return;
	}

	if (bind(ns, (struct sockaddr *)&sa, sizeof(sa)))
	{
		PrintError("[%s]%d bind()",__func__,__LINE__);
		close(ns);
		return;
	}

	while (1)
	{
		len = recvmsg(ns, &msg, 0);
		if (len < 0)
		{
			PrintError("[%s]%d recvmsg()",__func__,__LINE__);
		}
		else
		{
			P_HOTPLUG_Parse(buf);
		}

#if defined(DEBUG_PRINT_HOTPLUG_DATA)
		P_HOTPLUG_Dump(buf, len);
#endif
		VK_fflush(VK_STDOUT);
	}

	close(ns);

	return;
}

DRV_Error DRV_HOTPLUG_EventRegister(DRV_HP_DEV_EVT HpEvt, unsigned long EventId)
{
	if(HpEvt >= HP_EVT_NUM)
	{
		PrintError("%s : Can't Register Event %d\n",__func__,HpEvt);
		return DRV_ERR;
	}
	EventIds[HpEvt]=EventId;

	return DRV_OK;
}

DRV_Error DRV_HOTPLUG_MsgRegister(DRV_HP_DEV_EVT HpEvt, unsigned long MsgId)
{
	if (HpEvt >= HP_EVT_NUM)
	{
		PrintError("[%s] Error. unknown hotplug type(HpEvt:%d)\n", __FUNCTION__, HpEvt);
		return DRV_ERR;
	}

	MsgIds[HpEvt] = MsgId;

	return DRV_OK;
}

DRV_Error DRV_HOTPLUG_ReadSysUsb(void)
{
	int slot;
	char usbdev_path[MAX_PATH_NUM];
	DIR *dir_info;
	hp_evt_t hp_evt;
	struct dirent *dir_entry;

	char *p = NULL;
	char usbdev_link[MAX_PATH_NUM];
	int len = 0;

	dir_info = opendir(DRV_HOTPLUG_SYS_USB_PATH);
	if(dir_info == NULL)
		return DRV_ERR;

	while((dir_entry = readdir(dir_info)) != NULL)
	{
		if(dir_entry->d_name == NULL)
		{
			PrintDebug("[%s]_%d dir_entry->d_name NULL?\n",__func__,__LINE__);
			continue;
		}

		if( dir_entry->d_name[0] == '.' ||
				VK_strncmp(dir_entry->d_name, "usb", 3) == 0 ||
				VK_strchr(dir_entry->d_name, (int)':') == NULL ) {
			continue;
		}

		VK_snprintf(usbdev_path, sizeof(usbdev_path), "%s/%s", DRV_HOTPLUG_SYS_USB_PATH, dir_entry->d_name);
		len = readlink(usbdev_path, usbdev_link, sizeof(usbdev_link));

		if(len < 0 || len > (int)sizeof(usbdev_link)-1 ){
			PrintDebug("usbdev link path = %s\n", usbdev_path);
			continue;
		}

		usbdev_link[len] = '\0';
		p = strrchr(usbdev_link, (int)'d');

		if(p == NULL) {
			continue;
		}
		p--;

		VK_snprintf(usbdev_path, sizeof(usbdev_path), "%s", p );
		PrintDebug("# usbdev_path : %s\n", usbdev_path);

		slot = P_HOTPLUG_CheckUsbDevice(usbdev_path, 0);
		if (slot < 0)
			continue;

		hp_evt.eConnEvt = DRV_HP_CONN;
		hp_evt.idvendor=tUsbInfo[slot].idVendor;
		hp_evt.idproduct=tUsbInfo[slot].idProduct;
		hp_evt.portnum=tUsbInfo[slot].portnum;
		hp_evt.busnum = tUsbInfo[slot].busnum;

		PrintDebug("[%s] usb_device (action:%s), InterfaceClass=%d, DeviceClass=%d, dev_type(%d)\n", __FUNCTION__, hp_evt.eConnEvt ? "disconnected" : "connected" , tUsbInfo[slot].bInterfaceClass, tUsbInfo[slot].bDeviceClass, tUsbInfo[slot].dev_type);
		P_HOTPLUG_SendEvent(tUsbInfo[slot].dev_type, &hp_evt);
	}
	closedir(dir_info);

	return DRV_OK;
}

DRV_Error DRV_HOTPLUG_Init(void)
{
	int ret;

	VK_MEM_Memset(EventIds, 0x00, sizeof(unsigned long)*HP_EVT_NUM);
	VK_MEM_Memset(MsgIds, 0x00, sizeof(unsigned long)*HP_EVT_NUM);

	ret=VK_TASK_Create(DRV_HOTPLUG_Task, HOTPLUG_TASK_PRIORITY, HOTPLUG_TASK_STACK_SIZE, "DRV_HOTPLUG_Task",(void *)0, &ulHotplugEventId, 0);
	if(ret != VK_OK)
	{
		PrintError("%s[%d] : VK_TASK_Create() error\n",__func__,__LINE__);
		return DRV_ERR;
	}
	else
	{
		VK_TASK_Start(ulHotplugEventId);
	}

	return DRV_OK;
}

/*
 * @defgroup		DI_HDD DI_HDD : DI_HDD module
 * @author			hs.choi
 * @note			DI_HDD APIs
 * @brief			Define DI_HDD APIs
 * @file 			di_hdd.c                           */

/*******************************************************************
** Headers
********************************************************************/
#if defined(CONFIG_VK_STDLIB)
#else
#include 	<stdio.h>
#include 	<stdlib.h>
#include 	<string.h>
#endif
#include <sys/types.h>
#include <sys/stat.h>
#include <dirent.h>
#include <fcntl.h>
#include <sys/socket.h>
#include <linux/netlink.h>
#include <linux/rtnetlink.h>

#if defined(CONFIG_DI_SHARED_LIB) || defined(CONFIG_SHARED_COMPILE)
#include <unistd.h>
#include <errno.h>
#include <sys/ioctl.h>
#include "hdd_gpio_map.h"
#endif

#include "nexus_platform.h"
#include "vkernel.h"
#include "taskdef.h"
#include "di_uart.h"
#include "di_hdd.h"
#include "drv_hdd.h"
#include "drv_hotplug.h"
#include "drv_gpio.h"
#include "drv_smart.h"
#include "drv_pm.h"

/*******************************************************************
** Defines
********************************************************************/
//#define HDD_DEBUG
#ifdef HDD_DEBUG
#define PrintDebug(fmt, ...)    VK_DBG_Print (fmt, ## __VA_ARGS__)
#define PrintError(fmt, ...)    VK_DBG_Print (fmt, ## __VA_ARGS__)
#define PrintInfo(fmt, ...)    	VK_DBG_Print (fmt, ## __VA_ARGS__)
#define PrintEnter(...)         VK_printf("(+)%s\n", __FUNCTION__)
#define PrintExit(...)          VK_printf("(-)%s\n", __FUNCTION__)
#else
#define PrintDebug(...)
#define PrintError(...)         VK_DBG_Print (__VA_ARGS__)
#define PrintInfo(fmt, ...)    	VK_DBG_Print (fmt, ## __VA_ARGS__)
#define PrintEnter(...)
#define PrintExit(...)
#endif

#define MAX_PATH_NUM	1024
#define DATA_NUM	256

#if defined(CONFIG_DI_SHARED_LIB) || defined(CONFIG_SHARED_COMPILE)
#define IOCTL_REGCTL_MAGIC 		'H'
#define IOCTL_REGCTL_READ		_IOR(IOCTL_REGCTL_MAGIC, 1, ioctl_driver_info)
#define IOCTL_REGCTL_WRITE 		_IOW(IOCTL_REGCTL_MAGIC, 2, ioctl_driver_info)
#endif

#define USBHDD_HOSTNUMBER_BUF_SIZE		5
#define USBHDD_FILEPATH_BUF_SIZE		256
#define USBHDD_TEMP_BUF_SIZE			512

#define USBHDD_VENDOR_LINE 			1
#define USBHDD_PRODUCT_LINE 			2
#define USBHDD_SERIALNUM_LINE 			3
#define USBHDD_INFO_POSITION 			15

#define DI_HDD_HOTPLUG_MSG_SIZE   8

/*******************************************************************
** Typedef
********************************************************************/
enum {
	SATA_PHYSICAL_PORT_1 = 1,
	SATA_PHYSICAL_PORT_2 = 2	
};

#if defined(CONFIG_DI_SHARED_LIB) || defined(CONFIG_SHARED_COMPILE)
typedef struct
{
	unsigned int addr;
	unsigned int value;
}__attribute__((packed)) ioctl_driver_info;
#endif

#if defined(CONFIG_BRCM_MOD_NEXUS_POWER_MANAGEMENT)
typedef struct sata_hdd_status_s
{
	HBOOL bOn;
	char devName[DEV_NAME_LEN+1];
}sata_hdd_status_t;
#endif

typedef struct
{
	char *name;
	int v_port;
	int hw_port;
}sata_virtual_portmap_t;

/*******************************************************************
** Global Variables
********************************************************************/

/*******************************************************************
** Static Variables
********************************************************************/
static pfnDI_HDD_NotifyFUNC CB_HDDNotify = NULL;
static pfnDI_HDD_ErrNotify CB_HDDErrNotify = NULL;
static unsigned long ulHddMsgId;
static unsigned long ulHddTaskId;
#if !defined(CONFIG_OS_UCOS)
static unsigned long ulHddErrHandleTaskId;
#endif
static hdd_info_t hdd_info;
static unsigned long sHddPowerState; /* atomic */
#if defined(CONFIG_HDD_MODULE)
static int bModuleLinkedUp;
#endif
static unsigned long attached_sd_number;

#if defined(CONFIG_DI_SHARED_LIB) || defined(CONFIG_SHARED_COMPILE)
#if defined(CONFIG_ARM) && !defined(CONFIG_JAPAN_SPEC)
#else
static int g_fd = -1;
#endif
#endif

#if defined(CONFIG_BRCM_MOD_NEXUS_POWER_MANAGEMENT)
static sata_hdd_status_t 	stSataHddStatus[DI_HDD_PORT_NUM_MAX]; /*stSataHddStatus[0] is not used. index starts from 1*/
#endif

static const sata_virtual_portmap_t	stSataVirtualPortmap[] = {
	/*name*/      /*virtual port value*/            /*hw port value*/
#if defined(CONFIG_HDD)
	{"internal",   SATA_VIRTUAL_PORT_INTERNAL,   CONFIG_HDD_INTERNAL_HW_PORTNUM},
	{"detachable", SATA_VIRTUAL_PORT_DETACHABLE, CONFIG_HDD_DETACHABLE_HW_PORTNUM},
	{"e-sata",     SATA_VIRTUAL_PORT_ESATA,      CONFIG_HDD_ESATA_HW_PORTNUM}
#else
	{"interal",    SATA_VIRTUAL_PORT_INTERNAL,   0xff},
	{"detachable", SATA_VIRTUAL_PORT_DETACHABLE, 0xff},
	{"e-sata",     SATA_VIRTUAL_PORT_ESATA,      0xff}
#endif
};

#if !defined(CONFIG_HDD_MODULE)
static const char *sDrvAhciType[] = {
	"ahci",
	"imx53-ahci",
	"strict-ahci",
	NULL
};
#endif

/*******************************************************************
** Functions
********************************************************************/
/*******************************************************************
** Functions
**	Name:
**	Param:
**	Return:
********************************************************************/
static int P_HDD_PORT_VirtualToHw(int v_port)
{
	int max_index = sizeof(stSataVirtualPortmap)/sizeof(sata_virtual_portmap_t);
	int i;

	for (i=0; i<max_index; i++)
	{
		if (stSataVirtualPortmap[i].v_port == v_port)
		{
			return stSataVirtualPortmap[i].hw_port;
		}
	}

	return 0xff;
}

/*******************************************************************
** Functions
**	Name:
**	Param:
**	Return:
********************************************************************/
static int P_HDD_PORT_HwToVirtual(int hw_port)
{
	int max_index = sizeof(stSataVirtualPortmap)/sizeof(sata_virtual_portmap_t);
	int i;

	for (i=0; i<max_index; i++)
	{
		if (stSataVirtualPortmap[i].hw_port == hw_port)
		{
			return stSataVirtualPortmap[i].v_port;
		}
	}

	return -1;
}

/*******************************************************************
** Functions
**	Name:
**	Param:
**	Return:
********************************************************************/
static char *P_HDD_FindString(char *buf, char *fstr, int len)
{
	char *src = buf;

	while (*src != 0x0) {
		if (VK_strncmp(src, fstr, len) == 0)
			break;

		while (*src != 0x0) {
			src++;
		}
		src++;
	}

	if (*src == 0x0)
		return NULL;

	src += len;

	return src;
}

#if !defined(CONFIG_OS_UCOS)
/*******************************************************************
** Functions
**	Name:
**	Param:
**	Return:
********************************************************************/
static void P_HDD_ErrHandleTask(void *pParam)
{
	int 				ns = -1;
	struct sockaddr_nl 	sa;
	char 				buf[1024];
	char 				dev[4];
	struct iovec iov = {buf, sizeof(buf)};
	struct msghdr msg = {&sa, sizeof(sa), &iov, 1, NULL, 0, 0};
	int 				len;
	char				*p;
	unsigned long long	sector;
	char				cmd[256];
	int					devSize = 3;

	(void)pParam;

	len = sizeof(buf);
	VK_MEM_Memset(&sa, 0, sizeof(sa));
	sa.nl_family = AF_NETLINK;
	sa.nl_groups = RTMGRP_LINK | RTMGRP_IPV4_IFADDR;

	ns = socket(AF_NETLINK, SOCK_RAW, NETLINK_KOBJECT_UEVENT);
	if (ns < 0) {
		PrintError("[%s]%d socket()",__func__,__LINE__);
		return;
	}

	if (bind(ns, (struct sockaddr *)&sa, sizeof(sa))) {
		PrintError("[%s]%d bind()",__func__,__LINE__);
		close(ns);
		return;
	}

	while(1) {
		len = recvmsg(ns, &msg, 0);
		if (len < 0) {
			PrintError("[%s]%d recvmsg()",__func__,__LINE__);
			continue;
		}

		if (VK_strncmp(buf,"change@",7) != 0) {
			continue;
		}

		p = P_HDD_FindString(buf, "SCSIERROR,", VK_strlen("SCSIERROR,"));
		if (!p) {
			continue;
		}

		p = P_HDD_FindString(p, "dev=", VK_strlen("dev="));
		if (!p) {
			continue;
		}

		VK_MEM_Memset(dev, 0, sizeof(dev));
		VK_MEM_Memcpy(dev, p, devSize);

		if ((attached_sd_number & (1<<(dev[2]-'a'))) == 0) {
			continue;
		}

		PrintError("[%s] Pending Sector Detected! dev=%s\n", __FUNCTION__, dev);
		p += (devSize+1);
		p = P_HDD_FindString(p, "sector=", VK_strlen("sector="));
		if (!p) {
			continue;
		}

		sector = strtol(p, NULL, 10);
		PrintError("[%s] Pending Sector Detected! sector=%llu\n", __FUNCTION__, sector);

		if (CB_HDDErrNotify) {
			CB_HDDErrNotify(DI_HDD_Event_PendingSector, (const char*)dev, VK_strlen(dev), (unsigned long long)sector);
			continue;
		}

		VK_MEM_Memset(cmd, 0, sizeof(cmd));
		VK_snprintf(cmd, sizeof(cmd)-1, "dd if=/dev/zero of=/dev/%s bs=4096 count=1 seek=%llu", dev, sector*512/4096);
		VK_SYSTEM_Command(cmd);

		PrintError("[%s] Overwrite block(pending Sector). block(%llu), sector(%llu)\n", __FUNCTION__, sector*512/4096, sector);
	}

	return;
}
#endif

/*******************************************************************
** Functions
**	Name:
**	Param:
**	Return:
********************************************************************/
#if !defined(CONFIG_HDD_MODULE)
static void P_HDD_CheckAndSendEvent(void)
{
#define SD_DEV_MAX	8

	DIR           *dir;
	struct dirent *ent;
	unsigned long ata_dev_start_num = 0xffffffff;
	int           i;
	char          tmp[128];
	char          path[256];
	char          link[256];
	int           len;
	char          *p;
	hp_evt_t      tEvent;
	int           rc;

	for (i = 0; sDrvAhciType[i] != NULL; i++)
	{
		VK_snprintf(tmp, sizeof(tmp), "/sys/devices/platform/%s.0", sDrvAhciType[i]);
		dir = opendir(tmp);
		if (dir)
		{
			break;
		}
	}
	if (!dir)
	{
		PrintError("[%s] Error. can't open ahci device path\n", __FUNCTION__);
		return;
	}

	while((ent = readdir(dir)) != NULL)
	{
		/* Find directories of "host#" */
		if ((VK_strlen(ent->d_name) > 4) && (VK_strncmp(ent->d_name, "host", 4) == 0))
		{
			/* Calculate ATA-host start number */
			if (strtoul(&(ent->d_name[4]), NULL, 10) < ata_dev_start_num)
			{
				ata_dev_start_num = strtoul(&(ent->d_name[4]), NULL, 10);
			}
		}
	}
	closedir(dir);

	if (ata_dev_start_num == 0xffffffff)
	{
		// in case of kernel 3.3 and 3.8
		VK_snprintf(path, sizeof(path), "%s/ata1", tmp);
		dir = opendir(path);
		if (!dir)
		{
			// there is no ata host controller.
			return;
		}

		while((ent = readdir(dir)) != NULL)
		{
			/* Find directories of "host#" */
			if ((VK_strlen(ent->d_name) > 4) && (VK_strncmp(ent->d_name, "host", 4) == 0))
			{
				/* Calculate ATA-host start number */
				if (strtoul(&(ent->d_name[4]), NULL, 10) < ata_dev_start_num)
				{
					ata_dev_start_num = strtoul(&(ent->d_name[4]), NULL, 10);
				}
			}
		}

		closedir(dir);
	}

	PrintInfo("[%s] ATA Host Start Number : %d\n", __FUNCTION__, ata_dev_start_num);

	/* And now, find sd device on ATA */
	for (i=0; i<SD_DEV_MAX; i++)
	{
		VK_MEM_Memset(path, 0, sizeof(path));
		VK_MEM_Memset(link, 0, sizeof(link));

		VK_snprintf(path, sizeof(path), "/sys/block/sd%c/device", 'a'+i);
		len = readlink(path, link, sizeof(link)-1);

		if (len > 0 && (unsigned long)len < sizeof(link))
		{
			link[len] = '\0';
			if (VK_strstr(link, "ahci") && (p = VK_strstr(link, "host")))
			{
				/* this sdx is on ATA */
				VK_MEM_Memset(&tEvent, 0, sizeof(hp_evt_t));
				tEvent.eConnEvt = DRV_HP_CONN;
				tEvent.portnum = (int)(VK_atoi(p+4) - ata_dev_start_num + 1);
				VK_snprintf(tEvent.devpath, sizeof(tEvent.devpath), "/block/sd%c", 'a'+i);
				tEvent.len = VK_strlen(tEvent.devpath);

				rc = VK_MSG_SendTimeout(ulHddMsgId, &tEvent, sizeof(hp_evt_t), 0);
				if (rc != VK_OK)
				{
					PrintError("[%s,%d] Error! timeout(rc=%d)\n", __FUNCTION__, __LINE__, rc);
				}
			}
		}
	}

	return;
}
#endif

#if defined(CONFIG_BRCM_MOD_NEXUS_POWER_MANAGEMENT)
static void P_HDD_RemoveFromDeviceSpace(const char *devName)
{
	char cmd[64];
	VK_snprintf(cmd, sizeof(cmd), "echo 1 > /sys/block/%3s/device/delete", devName);
	VK_SYSTEM_Command(cmd);
}
#endif

/*******************************************************************
** Functions
**	Name:
**	Param:
**	Return:
********************************************************************/
static void P_HDD_HotplugTask(void *pvParam)
{
	hp_evt_t php_evt;
	int	ret;
	int v_port;

	(void)pvParam;

#if !defined(CONFIG_HDD_MODULE)
	while(!CB_HDDNotify || !sHddPowerState)
	{
		VK_TASK_Sleep(100);
	}
	/* Check HDD device */
	P_HDD_CheckAndSendEvent();
#endif

	while(1) {
		if(!CB_HDDNotify || !sHddPowerState) {
			VK_TASK_Sleep(100);
			continue;
		}

		VK_TASK_Sleep(100); /* If continuously, releasing for the event, need to sleep */
		ret = VK_MSG_Receive(ulHddMsgId, &php_evt, sizeof(hp_evt_t));
		if(ret != VK_OK) {
			continue;
		}

		/* Get 'sdx' scsi device name from path */
		VK_strncpy(hdd_info.devName, &(php_evt.devpath[7]), sizeof(hdd_info.devName)-1);
		hdd_info.devName[sizeof(hdd_info.devName)-1] = 0;

		v_port = P_HDD_PORT_HwToVirtual(php_evt.portnum);
		if (v_port < 0)
		{
			PrintError("[%s] Hw port not mapped to virtual (hw port : %d)\n", __FUNCTION__, php_evt.portnum);
			continue;
		}
		hdd_info.portnum = v_port;

		switch(php_evt.eConnEvt) {
		case DRV_HP_CONN:
#if defined(CONFIG_BRCM_MOD_NEXUS_POWER_MANAGEMENT)
			if(hdd_info.portnum > 0 && hdd_info.portnum < DI_HDD_PORT_NUM_MAX)
			{
				stSataHddStatus[hdd_info.portnum].bOn = TRUE;
				VK_strncpy(stSataHddStatus[hdd_info.portnum].devName, hdd_info.devName, sizeof(stSataHddStatus[hdd_info.portnum].devName)-1);
				stSataHddStatus[hdd_info.portnum].devName[sizeof(stSataHddStatus[hdd_info.portnum].devName)-1] = 0;
			}
#endif
#if defined(CONFIG_HDD_SMART)
			ret = DRV_SMART_Init(php_evt.portnum, hdd_info.devName);
			if(ret != DRV_OK) {
				PrintError("%s->DRV_SMART_Init(): Error Code(%d), Line(%d), %s\n", __FUNCTION__, ret, __LINE__, __FILE__);
			}
			else {
				PrintDebug("%s->DRV_SMART_Init(): OK! Line(%d), %s\n", __FUNCTION__, __LINE__, __FILE__);
			}
#endif
			//add sd device on ata
			attached_sd_number |= (1 << (hdd_info.devName[2] - 'a'));

			hdd_info.devCount++;
			CB_HDDNotify(DI_HDD_Event_Pwron, (void *)&hdd_info);

			PrintError(" %s : Hdd nofity called ~\n",__func__);
			break;
		case DRV_HP_DISCONN:
#if defined(CONFIG_BRCM_MOD_NEXUS_POWER_MANAGEMENT)
			if(hdd_info.portnum > 0 && hdd_info.portnum < DI_HDD_PORT_NUM_MAX)
			{
				stSataHddStatus[hdd_info.portnum].bOn = FALSE;
				VK_MEM_Memset(stSataHddStatus[hdd_info.portnum].devName, 0x0, sizeof(stSataHddStatus[hdd_info.portnum].devName));
			}
#endif
			//remove sd device on ata
			attached_sd_number &= ~(1 << (hdd_info.devName[2] - 'a'));

			hdd_info.devCount--;
			CB_HDDNotify(DI_HDD_Event_Pwroff, (void *)&hdd_info);
			PrintDebug(" %s : Not registed Hdd nofity ~\n",__func__);

#if defined(CONFIG_HDD_SMART)
			ret = DRV_SMART_Uninit(php_evt.portnum);
			if(ret != DRV_OK) {
				PrintError("%s->DRV_SMART_Uninit(): Error Code(%d), Line(%d), %s\n", __FUNCTION__, ret, __LINE__, __FILE__);
			}
			else {
				PrintError("%s->DRV_SMART_Uninit(): OK! Line(%d), %s\n", __FUNCTION__, __LINE__, __FILE__);
			}
#endif
			break;
		default:
			break;
		}
	}
}

#if defined(CONFIG_HDD_MODULE)
/*******************************************************************
** Functions
**	Name:
**	Param:
**	Return:
********************************************************************/
static void P_HDD_Insmod(void)
{
	char cmd[64];

	VK_snprintf(cmd, sizeof(cmd) - 1, "modprobe ahci");
	PrintDebug("[%d] [%s] [%s] cmd = %s\n", __LINE__, __FUNCTION__, __FILE__, cmd);
	VK_SYSTEM_Command(cmd);

	VK_snprintf(cmd, sizeof(cmd) - 1, "modprobe ahci_platform");
	PrintDebug("[%d] [%s] [%s] cmd = %s\n", __LINE__, __FUNCTION__, __FILE__, cmd);
	VK_SYSTEM_Command(cmd);
}

/*******************************************************************
** Functions
**	Name:
**	Param:
**	Return:
********************************************************************/
#if 0
static void P_P_HDD_Rmmod(void)
{
	char cmd[64];

	VK_snprintf(cmd, sizeof(cmd) - 1, "modprobe -r ahci_platform");
	PrintDebug("[%d] [%s] [%s] cmd = %s\n", __LINE__, __FUNCTION__, __FILE__, cmd);
	VK_SYSTEM_Command(cmd);

	VK_snprintf(cmd, sizeof(cmd) - 1, "modprobe -r ahci");
	PrintDebug("[%d] [%s] [%s] cmd = %s\n", __LINE__, __FUNCTION__, __FILE__, cmd);
	VK_SYSTEM_Command(cmd);
}
#endif
#endif

/*******************************************************************
** Functions
**	Name:
**	Param:
**	Return:
********************************************************************/
DI_ERR_CODE DI_HDD_PWR_Ctrl(DI_HDD_PWR OnOff)
{
	DI_ERR_CODE	eErr = DI_ERR_OK;
	int v_port; /* virtual port */
	int hw_port; /* HW port */

	for (v_port = DI_HDD_PORT_NUM_1; v_port < DI_HDD_PORT_NUM_MAX; v_port++)
	{
		hw_port=P_HDD_PORT_VirtualToHw(v_port);
		if(hw_port != 0xff)
		{
			eErr = DI_HDD_ControlPower(v_port, OnOff);
			if (eErr != DI_ERR_OK)
			{
				PrintError("[%s] Error. fail to turn %s hdd(v_port:%d)\n", __FUNCTION__, OnOff?"ON":"OFF", v_port);
				continue;
			}
		}
	}

	return eErr;
}

/*******************************************************************
** Functions
**	Name:
**	Param:
**	Return:
********************************************************************/
void DI_HDD_RegisterCallback(pfnDI_HDD_NotifyFUNC pfnNotify)
{
	CB_HDDNotify = pfnNotify;
}

/*******************************************************************
** Functions
**	Name:
**	Param:
**	Return:
********************************************************************/
void DI_HDD_RegisterErrHandleCallback(pfnDI_HDD_ErrNotify pfnNotify)
{
	CB_HDDErrNotify = pfnNotify;
}

/*******************************************************************
** Functions
**	Name:
**	Param:
**	Return:
********************************************************************/
void DI_HDD_RegisterDstCallback(pfnDI_HDD_DSTNotify pfnDstNotify)
{
#if defined(CONFIG_HDD_SMART)
	DRV_SMART_RegisterDstCallback((pfnDRV_HDD_DSTNotify)pfnDstNotify);
#endif
}

/*******************************************************************
** Functions
**	Name:
**	Param:
**	Return:
********************************************************************/
DI_ERR_CODE DI_HDD_RestorePendingSector(const char *devname, unsigned long long sector)
{
	char	cmd[256];

	if (!devname) {
		return DI_ERR_ERROR;
	}

	VK_MEM_Memset(cmd, 0, sizeof(cmd));
	VK_snprintf(cmd, sizeof(cmd)-1, "dd if=/dev/zero of=/dev/%s bs=4096 count=1 seek=%llu", devname, sector*512/4096);
	if (VK_SYSTEM_Command(cmd) != VK_OK) {
		return DI_ERR_ERROR;
	}

	return DI_ERR_OK;
}

/*******************************************************************
** Functions
**	Name:
**	Param:
**	Return:
********************************************************************/
void DI_HDD_DstStart(void)
{
#if defined(CONFIG_HDD_SMART)
	DRV_SMART_DstStart(ATA_FIRST_HDD_PORT_NUM);		/* First device only */
#endif
}

/*******************************************************************
** Functions
**	Name:
**	Param:
**	Return:
********************************************************************/
DI_ERR_CODE DI_HDD_ControlPower(DI_HDD_PORT_NUM_E portnum, DI_HDD_PWR on)
{
#if defined(CONFIG_DI_SHARED_LIB) || defined(CONFIG_SHARED_COMPILE)
#if defined(CONFIG_ARM) && !defined(CONFIG_JAPAN_SPEC)
	NEXUS_GpioHandle	hHddPwrGpio;
	NEXUS_GpioType		eHddPwrGpioType;
	NEXUS_GpioSettings  tGpioSettings;
	unsigned int		nHddPwrGpioNum;
#else
	int 				rc;
	ioctl_driver_info 	ctrl_info;
	unsigned int		addr;
	unsigned int		value;
#endif
#else
	GPIO_ID_t			gpio;
#endif
	int v_port = (int)portnum; /* virtual port */
	int hw_port = P_HDD_PORT_VirtualToHw(v_port); /* hw port */

	if (hw_port == 0xff)
	{
		PrintError("[%s] ignore! v_port not mapped to hw-port(v_port:%d)\n", __FUNCTION__, v_port);
		return DI_ERR_ERROR;
	}

	switch(hw_port) {
	case SATA_PHYSICAL_PORT_1:
#if defined(CONFIG_DI_SHARED_LIB) || defined(CONFIG_SHARED_COMPILE)
#if defined(CONFIG_ARM) && !defined(CONFIG_JAPAN_SPEC)
		eHddPwrGpioType = HDD_PWR_GPIO_TYPE;
		nHddPwrGpioNum = HDD_PWR_GPIO_NUM;
#else
		addr = BCHP_GIO_HDD_ADDR;
		value = GPIO_HDD_PWR_MASK;
#endif
#else
		gpio = GPIO_ID_HDD_PWR_CTRL;
#endif
		break;

	case SATA_PHYSICAL_PORT_2:
#if defined(CONFIG_DI_SHARED_LIB) || defined(CONFIG_SHARED_COMPILE)
#if defined(CONFIG_ARM) && !defined(CONFIG_JAPAN_SPEC)
		eHddPwrGpioType = HDD2_PWR_GPIO_TYPE;
		nHddPwrGpioNum = HDD2_PWR_GPIO_NUM;
#else
		addr = BCHP_GIO_HDD2_ADDR;
		value = GPIO_HDD_PWR2_MASK;
#endif
#else
		gpio = GPIO_ID_HDD_PWR_CTRL_2;
#endif
		break;

	default:
		PrintError("%s: Invalid Port Numer(%d)!\n", __FUNCTION__, v_port);
		return DI_ERR_ERROR;
	}

	switch(on) {
	case DI_HDD_PWR_ON:
#if defined(CONFIG_DI_SHARED_LIB) || defined(CONFIG_SHARED_COMPILE)
#if defined(CONFIG_ARM) && !defined(CONFIG_JAPAN_SPEC)
		if (eHddPwrGpioType == (unsigned)(-1) || nHddPwrGpioNum == (unsigned)(-1))
		{
			PrintError("[%s,%d] Error! Unknown gpio type(%d) or number(%d)\n", __FUNCTION__, __LINE__, eHddPwrGpioType, nHddPwrGpioNum);
			return DI_ERR_ERROR;
		}

		NEXUS_Gpio_GetDefaultSettings(eHddPwrGpioType, &tGpioSettings);
		tGpioSettings.mode = NEXUS_GpioMode_eOutputPushPull;
		tGpioSettings.interruptMode = NEXUS_GpioInterrupt_eDisabled;
		tGpioSettings.value = NEXUS_GpioValue_eHigh;

		hHddPwrGpio = NEXUS_Gpio_Open(eHddPwrGpioType, nHddPwrGpioNum, &tGpioSettings);
		if (hHddPwrGpio == NULL)
		{
			PrintError("[%s,%d] Error! NEXUS_Gpio_Open fails\n", __FUNCTION__, __LINE__);
			return DI_ERR_ERROR;
		}
		NEXUS_Gpio_Close(hHddPwrGpio);
#else
		if (g_fd >= 0 && addr && value) {
			ctrl_info.addr = addr;
			ctrl_info.value = 0;
			rc = ioctl(g_fd, IOCTL_REGCTL_READ, &ctrl_info);
			if(rc != 0){
				PrintError("[%s][%d]nexus_gpio ioctl error %x, g_fd=%d\n", __FUNCTION__, __LINE__, rc, g_fd);
				return DI_ERR_ERROR;
			}

			ctrl_info.addr = addr;
			ctrl_info.value |= value;
			rc = ioctl(g_fd, IOCTL_REGCTL_WRITE, &ctrl_info);
			if(rc != 0 || ctrl_info.value != IOCTL_REGCTL_WRITE){
				PrintError("[%s][%d]nexus_gpio ioctl error\n", __FUNCTION__, __LINE__);
				return DI_ERR_ERROR;
			}
		}
#endif
#else /* STATIC DI */
		if (gpio != GPIO_ID_MAX)
		{
			DRV_GPIO_Write(gpio, GPIO_HI);
		}
#endif
#if defined(CONFIG_HDD_MODULE)
		/* modprobe ahci for insmod ahci.ko and libahci.ko to link SATA HDD up */
		if (!bModuleLinkedUp) {
			P_HDD_Insmod();
			bModuleLinkedUp = 1;
		}
#endif
#if defined(CONFIG_BRCM_MOD_NEXUS_POWER_MANAGEMENT)
        {
	        char cmd[64];
            int i = 0, detect = 0;
            DIR *dir = NULL;
            struct dirent *ent = NULL;
        	VK_snprintf(cmd, sizeof(cmd), "echo \"- - -\" > /sys/class/scsi_host/host0/scan");

            for(i = 0; i<30; i++)
            {
                if((dir = opendir("/sys/class/scsi_host/host0/device")) != NULL)
                {
                    while((ent = readdir(dir)) != NULL)
                    {
                        PrintError("%s\n", ent->d_name);
                        if(!strncmp(ent->d_name, "target", 6))
                        {
                            PrintError("Found the target directory %s\n", ent->d_name);
                            detect = 1; 
                        }
                    }
                    closedir(dir);
                    if(detect == 1)
                    {
                        break;
                    }
                }
                else
                {
                    PrintError("Can't not open /sys/class/scsi_host/host0/device directory\n");
                }
                PrintError("SATA Device did not detect #############[%d]\n", i);
                VK_SYSTEM_Command(cmd);
                VK_TASK_Sleep(1000);
            }
#if 0
            FILE *file;
        	VK_snprintf(cmd, sizeof(cmd), "echo \"- - -\" > /sys/class/scsi_host/host0/scan");
            while(1)
            {
                if(file = fopen("/sys/block/sda/stat", "r"))
                {
                    fclose(file);
                    break;
                }
                else
                {
                    PrintError("File not found[[[[[[[[[[%d]]]]]]]]]\n", count);
            	    VK_SYSTEM_Command(cmd);
                    VK_TASK_Sleep(1000);
                    count++;
                }
            }
#endif
        }
#endif
		sHddPowerState |= (1 << (v_port-1));
		break;

	case DI_HDD_PWR_OFF:
#if defined(CONFIG_HDD_SMART)		
		DRV_SMART_ATA_OP_FlushBufferCache(hw_port);	/* flush buffer cache */
		DRV_SMART_ATA_OP_FlushCache(hw_port);	/* Flush write cache */
#endif		
#if defined(CONFIG_BRCM_MOD_NEXUS_POWER_MANAGEMENT)
		/*[temporarily skip standby command while implementing S3 mode.*/
		/* It takes too much time to complete.*/
#else
#if defined(CONFIG_HDD_SMART)
		DRV_SMART_ATA_OP_Reset(hw_port);	/* re-reading partition table and resetting drive */
		DRV_SMART_ATA_OP_StandbyImmediate(hw_port);	/* Standby Immediately command */
#endif		
#endif
#if defined(CONFIG_BRCM_MOD_NEXUS_POWER_MANAGEMENT)
		if(stSataHddStatus[v_port].bOn == TRUE)
		{
			P_HDD_RemoveFromDeviceSpace(stSataHddStatus[v_port].devName);
		}
		VK_TASK_Sleep(1000);
#endif

#if defined(CONFIG_DI_SHARED_LIB) || defined(CONFIG_SHARED_COMPILE)
#if defined(CONFIG_ARM) && !defined(CONFIG_JAPAN_SPEC)
		if (eHddPwrGpioType == (unsigned)(-1) || nHddPwrGpioNum == (unsigned)(-1))
		{
			PrintError("[%s,%d] Error! Unknown gpio type(%d) or number(%d)\n", __FUNCTION__, __LINE__, eHddPwrGpioType, nHddPwrGpioNum);
			return DI_ERR_ERROR;
		}

		NEXUS_Gpio_GetDefaultSettings(eHddPwrGpioType, &tGpioSettings);
		tGpioSettings.mode = NEXUS_GpioMode_eOutputPushPull;
		tGpioSettings.interruptMode = NEXUS_GpioInterrupt_eDisabled;
		tGpioSettings.value = NEXUS_GpioValue_eLow;

		hHddPwrGpio = NEXUS_Gpio_Open(eHddPwrGpioType, nHddPwrGpioNum, &tGpioSettings);
		if (hHddPwrGpio == NULL)
		{
			PrintError("[%s,%d] Error! NEXUS_Gpio_Open fails\n", __FUNCTION__, __LINE__);
			return DI_ERR_ERROR;
		}
		NEXUS_Gpio_Close(hHddPwrGpio);
#else
		if (g_fd >= 0 && addr && value) {
			ctrl_info.addr = addr;
			ctrl_info.value = 0;
			rc = ioctl(g_fd, IOCTL_REGCTL_READ, &ctrl_info);
			if(rc != 0){
				PrintError("[%s][%d]nexus_gpio ioctl error\n", __FUNCTION__, __LINE__);
				return DI_ERR_ERROR;
			}

			ctrl_info.addr = addr;
			ctrl_info.value &= ~value;
			rc = ioctl(g_fd, IOCTL_REGCTL_WRITE, &ctrl_info);
			if(rc != 0 || ctrl_info.value != IOCTL_REGCTL_WRITE){
				PrintError("[%s][%d]nexus_gpio ioctl error\n", __FUNCTION__, __LINE__);
				return DI_ERR_ERROR;
			}
		}
#endif
#else /* STATIC DI */
		if (gpio != GPIO_ID_MAX)
		{
			DRV_GPIO_Write(gpio, GPIO_LOW);
		}
#endif
#if defined(CONFIG_HDD_MODULE)
		/* rmmod ahci for rmmod ahci.ko and libahci.ko to line SATA HDD down */
		/* workaround 2012.11.20
		** if we excute rmmod, kernel gets panic. so at boot, we excute modprobe once and never excute rmmod */
		//P_P_HDD_Rmmod();
#endif
		sHddPowerState &= ~(1 << (v_port-1));
		break;

	default:
		break;
	}

	return DI_ERR_OK;
}

/*******************************************************************
** Functions
**	Name:
**	Param:
**	Return:
********************************************************************/
void DI_HDD_StartDst(DI_HDD_PORT_NUM_E portnum)
{
#if defined(CONFIG_HDD_SMART)
	int v_port = (int)portnum; /* virtual port */
	int hw_port = P_HDD_PORT_VirtualToHw(v_port); /* hw port */

	if (hw_port == 0xff)
	{
		PrintError("[%s] virtual port not mapped to hw-port(v_port:%d)\n", __FUNCTION__, v_port);
		return;
	}
	PrintError("Dist Start (%d)\n", portnum);

	(void)DRV_SMART_DstStart(hw_port);
#endif	
}

#if defined(CONFIG_CRYPTSETUP)
/*******************************************************************
** Functions
**	Name:
**	Param:
**	Return:
********************************************************************/
DI_ERR_CODE DI_HDD_GetEncryptionKey(DI_HDD_PORT_NUM_E portnum, char *pKey)
{
#if defined(CONFIG_HDD_SMART)
	DRV_SMART_SATA_INFO_t tSataInfoT;
	char key[HDD_ENCRYPTION_KEY_LEN]={0,};
	int len;
	int i;
	int v_port = (int)portnum; /* virtual port */
	int hw_port = P_HDD_PORT_VirtualToHw(v_port); /* hw port */

	if (!pKey)
	{
		return DI_ERR_ERROR;
	}
	if (hw_port == 0xff)
	{
		PrintError("[%s] virtual port not mapped to hw-port(v_port:%d)\n", __FUNCTION__, v_port);
		return DI_ERR_ERROR;
	}

	if (DRV_SMART_IdentifyDevice(hw_port, &tSataInfoT) != DRV_OK) {
		PrintError("[%s] ERROR! Can't get sata info\n", __FUNCTION__);
		return DI_ERR_ERROR;
	}

	len = VK_strlen(tSataInfoT.ucSerialNo);
	for(i=0; i<len; i++) {
		if (tSataInfoT.ucSerialNo[i] >= '0' && tSataInfoT.ucSerialNo[i] <='9') {
			key[i] = (char)('0' + '9' - tSataInfoT.ucSerialNo[i]);
		}
		else if (tSataInfoT.ucSerialNo[i] >= 'A' && tSataInfoT.ucSerialNo[i] <= 'Z'){
			key[i] = (char)('A' + 'Z' - tSataInfoT.ucSerialNo[i]);
		}
		else if (tSataInfoT.ucSerialNo[i] >= 'a' && tSataInfoT.ucSerialNo[i] <= 'z'){
			key[i] = (char)('a' + 'z' - tSataInfoT.ucSerialNo[i]);
		}
		else {
			key[i] = (char)(tSataInfoT.ucSerialNo[i]);
		}
	}
	VK_strncpy(pKey, key, VK_strlen(key));

	return DI_ERR_OK;
#else
	return DI_ERR_ERROR;
#endif
}
#endif

#if defined(CONFIG_DI20)
/*restrict till di10 header updated*/
/* 함수 설명 : USB HDD의 Vendor, Product, Serial number를 가져오는 함수 구현
  (1) /proc/scsi/sg/devices를 파싱하여 host_no 정보를 획득 (순차적으로 1번 줄 = sda, 2번 줄 = sdb, ...etc)
      -  [host_no], [channel], [scsi_id], [scsi_lun], [scsi_type], [access_count], [queue_depth], [device_busy], [online] :  '\t' 로 구분

  (2) host_no를 기반으로 /proc/scsi/usb-storage/(host_no)에서 USB  Vendor, product,Serial Number정보 파싱 :  해당 정보의 결과는 [15]번 자리부터 표시
     - [Host scsi0], [Vendor], [Product,], [Serial Number], [Protocol], [Transport], [Quirks] : '\n'으로 구분
*/
DI_ERR_CODE DI_HDD_GetInfoUsbHdd(DI_FS_StorDevIdx_t eDevIdx, DI_HDD_USBHDD_INFO_t *pstUsbHddInfo)
{
	HINT32 nFD, nReadLength;
	HINT32 nIndex, nTabNum=0, nLineNum=0, nParserIndex=0, nPositionIndex=0;
	HUINT8 aucHostNum[USBHDD_HOSTNUMBER_BUF_SIZE];
	char aucFilePath[USBHDD_FILEPATH_BUF_SIZE];
	HUINT8 aucTempBuf[USBHDD_TEMP_BUF_SIZE];

	VK_memset(aucHostNum, 0x00, USBHDD_HOSTNUMBER_BUF_SIZE);
	VK_memset(aucFilePath, 0x00, USBHDD_FILEPATH_BUF_SIZE);
	VK_memset(aucTempBuf, 0x00, USBHDD_TEMP_BUF_SIZE);

/* Get host number of eDevIdx */
	VK_snprintf(aucFilePath, USBHDD_FILEPATH_BUF_SIZE, "/proc/scsi/sg/devices");
	nFD = open(aucFilePath, O_RDONLY, 0);
	if (nFD != -1)
	{
		nReadLength = read(nFD, aucTempBuf, USBHDD_TEMP_BUF_SIZE);
		if(nReadLength != -1)
		{
			for(nIndex=0; nIndex<nReadLength; nIndex++)
			{
				if(aucTempBuf[nIndex] == '\t')
				{
					nTabNum++;
				}
				else if(aucTempBuf[nIndex] == '\n')
				{
					nTabNum=0;
					nLineNum++;
				}
				else
				{
					if(nLineNum == eDevIdx && nTabNum <= 0
						&& aucTempBuf[nIndex] != '\0' && aucTempBuf[nIndex] != ' ' )
					{
						aucHostNum[nParserIndex] = aucTempBuf[nIndex];
						nParserIndex++;
					}
				}

			}
		}
		else
		{
			close(nFD);
			PrintError("[%s] File read fail (%s) \n", __FUNCTION__, aucFilePath );
			return DI_ERR_ERROR;
		}

		close(nFD);
	}
	else
	{
		PrintError("[%s] File open fail (%s) \n", __FUNCTION__, aucFilePath );
		return DI_ERR_ERROR;
	}

	nLineNum = 0;
	nParserIndex = 0;
	VK_memset(aucFilePath, 0x00, USBHDD_FILEPATH_BUF_SIZE);
	VK_memset(aucTempBuf, 0x00, USBHDD_TEMP_BUF_SIZE);

/* Get Vendor, Product and Serial number of USB HDD */
	VK_snprintf(aucFilePath, USBHDD_FILEPATH_BUF_SIZE, "/proc/scsi/usb-storage/%s", aucHostNum);
	nFD = open(aucFilePath, O_RDONLY, 0);
	if (nFD != -1)
	{
		nReadLength = read(nFD, aucTempBuf, USBHDD_TEMP_BUF_SIZE);
		if(nReadLength != -1)
		{
			for(nIndex=0; nIndex<nReadLength; nIndex++)
			{
				if(aucTempBuf[nIndex] == '\n')
				{
					nPositionIndex = 0;
					nParserIndex = 0;

					nLineNum++;
				}
				else
				{
					if(nPositionIndex >= USBHDD_INFO_POSITION && aucTempBuf[nIndex] != '\0' &&  aucTempBuf[nIndex] != '\t')
					{
						if(nLineNum == USBHDD_VENDOR_LINE)
						{
							pstUsbHddInfo->aucUsbHddVendor[nParserIndex] = aucTempBuf[nIndex];
						}
						else if(nLineNum == USBHDD_PRODUCT_LINE)
						{
							pstUsbHddInfo->aucUsbHddProduct[nParserIndex] = aucTempBuf[nIndex];
						}
						else 	if(nLineNum == USBHDD_SERIALNUM_LINE)
						{
							pstUsbHddInfo->aucUsbHddSerialNum[nParserIndex] = aucTempBuf[nIndex];
						}

						nParserIndex++;
					}

					nPositionIndex++;
				}
			}
		}
		else
		{
			close(nFD);
			PrintError("[%s] File read fail (%s) \n", __FUNCTION__, aucFilePath );
			return DI_ERR_ERROR;
		}

		close(nFD);
	}
	else
	{
		PrintError("[%s] File open fail (%s) \n", __FUNCTION__, aucFilePath );
		return DI_ERR_ERROR;
	}

	return DI_ERR_OK;
}
#endif

/*******************************************************************
** Functions
**	Name:
**	Param:
**	Return:
********************************************************************/
DRV_Error DRV_HDD_Init(void)
{
	int ret;
	int max_index = sizeof(stSataVirtualPortmap)/sizeof(sata_virtual_portmap_t);
	int i;

	PrintEnter();

#if defined(CONFIG_DI_SHARED_LIB) || defined(CONFIG_SHARED_COMPILE)
#if defined(CONFIG_ARM) && !defined(CONFIG_JAPAN_SPEC)
	(void)NEXUS_Platform_Join();
#else
	g_fd = open("/dev/nexus_gpio", O_RDWR|O_NDELAY);
	if(g_fd < 0)
	{
		PrintError("[%s][%d], nexus_gpio open error, %s\n", __FUNCTION__, __LINE__, strerror(errno));
		/* ignore this error */
	}
#endif
#endif

#if defined(CONFIG_BRCM_MOD_NEXUS_POWER_MANAGEMENT)
	for(i=0; i< (DI_HDD_PORT_NUM_MAX); i++)
	{
		stSataHddStatus[i].bOn = FALSE;
		VK_MEM_Memset(stSataHddStatus[i].devName, 0x0, sizeof(stSataHddStatus[i].devName));
	}
#endif

	/* Init variable */
	attached_sd_number = 0;
	sHddPowerState = 0;

	ret = VK_MSG_Create(DI_HDD_HOTPLUG_MSG_SIZE, sizeof(hp_evt_t), "HddHotplugMsg", &ulHddMsgId, VK_SUSPENDTYPE_FIFO);
	if (ret != VK_OK)
	{
		PrintError("[%s] Error. can't create msg queue\n", __FUNCTION__);
		return DRV_ERR;
	}

	ret=VK_TASK_Create(P_HDD_HotplugTask, USB_TASK_PRIORITY, USB_TASK_STACK_SIZE, "HDD_Task", NULL, &ulHddTaskId,FALSE);
	if(ret != VK_OK)
	{
		PrintError("%s[%d] : VK_TASK_Create() error\n",__func__,__LINE__);
		return DRV_ERR;
	}
	else
	{
		VK_TASK_Start(ulHddTaskId);
	}
#if !defined(CONFIG_OS_UCOS)
	ret = VK_TASK_Create(P_HDD_ErrHandleTask, USB_TASK_PRIORITY, USB_TASK_STACK_SIZE, "HDD_ERR_HANDLE_TASL",NULL, &ulHddErrHandleTaskId, FALSE);
	if (ret != VK_OK) {
		PrintError("%s[%d] : VK_TASK_Create() error\n",__func__,__LINE__);
		return DRV_ERR;
	}
	VK_TASK_Start(ulHddErrHandleTaskId);
#endif

	DRV_HOTPLUG_MsgRegister(DRV_HP_HDD, ulHddMsgId);

	PrintInfo("[%s]*******************************************************\n", __FUNCTION__);
	for (i=0; i<max_index; i++)
	{
		PrintInfo("[%s]**** hdd-port(%s) <%s> <hw_port:%d>\n", __FUNCTION__, stSataVirtualPortmap[i].name, (stSataVirtualPortmap[i].hw_port == 0xff) ? "not used" : "used", stSataVirtualPortmap[i].hw_port);
	}
	PrintInfo("[%s]*******************************************************\n", __FUNCTION__);

#if defined(CONFIG_HDD_SMART)
	DRV_SMART_DstInit();
#endif
	PrintExit();

	return DRV_OK;
}

DI_ERR_CODE DI_HDD_Init(void)
{
	DRV_Error err;

	err = DRV_HDD_Init();
	if (err != DRV_OK) {
		return DI_ERR_ERROR;
	}

	return DI_ERR_OK;
}

DI_ERR_CODE DI_HDD_GetHddInfo(DI_HDD_PORT_NUM_E ePortnum, DI_HDD_INFO_t *pstHddInfo)
{
#if defined(CONFIG_HDD_SMART)
	DRV_Error	drvErr;
	DRV_SMART_SATA_INFO_t	stHddInfo;
	int v_port = (int)ePortnum; /* virtual port */
	int hw_port = P_HDD_PORT_VirtualToHw(v_port); /* hw port */

	if (hw_port == 0xff)
	{
		PrintError("[%s] ignore! v_port not mapped to hw-port(v_port:%d)\n", __FUNCTION__, v_port);
		return DI_ERR_ERROR;
	}

	drvErr = DRV_SMART_IdentifyDevice(hw_port, &stHddInfo);
	if (drvErr != DRV_OK) {
		return DI_ERR_ERROR;
	}

	VK_MEM_Memset((void *)pstHddInfo, 0x00, sizeof(DI_HDD_INFO_t));

	VK_MEM_Memcpy(pstHddInfo->aucModelName, stHddInfo.ucModelName, ATA_MODEL_NUM_LEN);
	VK_MEM_Memcpy(pstHddInfo->aucSerialNo,  stHddInfo.ucSerialNo, ATA_SERIAL_NUM_LEN);
	VK_MEM_Memcpy(pstHddInfo->aucFirmwareVer,  stHddInfo.ucFirmwareVer, ATA_FIRMWARE_NUM_LEN);
	pstHddInfo->ulSizeMB = stHddInfo.ulSizeMB;

	return DI_ERR_OK;
#else
	return DI_ERR_ERROR;
#endif
}


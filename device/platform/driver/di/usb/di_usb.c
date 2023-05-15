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
#include <unistd.h>
#include <errno.h>
#include <fcntl.h>
#include <sys/ioctl.h>
#include <sys/mman.h>
#include <sys/types.h>
#include <dirent.h>
#include <sys/stat.h>

#include <sys/socket.h>
#include <linux/netlink.h>
#include <linux/rtnetlink.h>

#include "nexus_types.h"
#include "di_err.h"
#include "drv_err.h"
#include "di_uart.h"
#include "vkernel.h"
#include "taskdef.h"
#include "htype.h"
#include "di_usb.h"
#include "drv_usb.h"

#include "nexus_platform.h"

#include "bstd_defs.h"
#include "breg_mem.h"

#if (BCHP_CHIP == 7250) || ((NEXUS_VERSION >= 1502) && (BCHP_CHIP == 7241))
#else
#include "bchp_usb_ctrl.h"
#endif
#if CONFIG_USB_HOSTCONTROLLER_CNT == 2
#if ((BCHP_CHIP == 7439) && (BCHP_VER == BCHP_VER_B0)) || ((BCHP_CHIP == 7252) && (BCHP_VER == BCHP_VER_B0)) || ((NEXUS_VERSION >= 1502) && (BCHP_CHIP == 7241))
#else
#include "bchp_usb1_ctrl.h"
#endif
#endif

/*******************************************************************
**  Defines
********************************************************************/
#if defined(CONFIG_DEBUG)
	#define USB_DEBUG
	#ifdef USB_DEBUG
	#define PrintDebug(fmt, ...)    VK_DBG_Print (fmt, ## __VA_ARGS__)
	#define PrintError(fmt, ...)    VK_DBG_Print (fmt, ## __VA_ARGS__)
	#define PrintInfo(fmt, ...)     VK_DBG_Print (fmt, ## __VA_ARGS__)
	#define PrintEnter(...)         VK_printf("(+)%s\n", __FUNCTION__)
	#define PrintExit(...)          VK_printf("(-)%s\n", __FUNCTION__)
	#else
	#define PrintDebug(...)
	#define PrintError(...)         VK_DBG_Print (__VA_ARGS__)
	#define PrintInfo(...)          VK_DBG_Print (__VA_ARGS__)
	#define PrintEnter(...)
	#define PrintExit(...)
	#endif
#else
	#define PrintDebug(...)    ((void)0)
	#define PrintError(...)    VK_DBG_Print (__VA_ARGS__)
	#define PrintInfo(...)     ((void)0)
	#define PrintEnter(...)    ((void)0)
	#define PrintExit(...)     ((void)0)
#endif

#define MAX_NUM_USB_STORAGE			8
#define USB_SD_NAME_LEN				13

#define USB_DEVICE_DIR				"/dev/bus/usb"
#define USB_SYS_DIR					"/sys/bus/usb/devices"
#define CMD_TIMEOUT					(5*1000)									/* 5ms */

/* HUB Command define */
#define USB_SET_PORT_COMMAND		((0x01 << 5) | 0x03) 						/* USB_TYPE_CLASS	| USB_RECIP_OTHER  */
#define USB_SET_HUB_COMMAND			(0x01 << 5)			 						/* USB_TYPE_CLASS  */
#define USB_GET_COMMAND				(0x80 | (0x01 << 5) | 0x03)					/* USB_ENDPOINT_IN | USB_TYPE_CLASS	| USB_RECIP_OTHER  */

#define USB_REQ_GET_STATUS			0x00
#define USB_REQ_CLEAR_FEATURE		0x01
#define USB_REQ_SET_FEATURE			0x03

/* HUB surbe command define */
#define USB_PORT_FEAT_CONNECTION	0
#define USB_PORT_FEAT_ENABLE		1
#define USB_PORT_FEAT_SUSPEND		2
#define USB_PORT_FEAT_OVER_CURRENT	3
#define USB_PORT_FEAT_RESET			4
#define USB_PORT_FEAT_POWER			8
#define USB_PORT_FEAT_C_CONNECTION	16
#define USB_PORT_FEAT_C_ENABLE		17
#define USB_PORT_FEAT_C_SUSPEND		18
#define USB_PORT_FEAT_C_OVER_CURRENT	19
#define USB_PORT_FEAT_C_RESET		20

/* HUB Status define */
#define USB_STATUS_RESET			0x08
#define USB_STATUS_OVERCURRENT		0x04
#define USB_STATUS_ENABLE			0x02
#define USB_STATUS_CONNECT			0x01

#define USB_POWER_SHOOTING_INTERVAL         10 /* ms */
#define USB_MSG_QUEUE_SIZE					64

#define USB_OCP_CHK_INTERVAL				1000 /* ms */
#define USB_OCP_CLEAR_INTERVAL			(2000 / USB_OCP_CHK_INTERVAL)
#define USB_OCP_NOTI_INTERVAL			(4000 / USB_OCP_CHK_INTERVAL)

/* IOCTL Command define */
#define SCSI_IOCTL_HUMAX_SYNC 0xf5388
/*******************************************************************
**  Type define
********************************************************************/
typedef enum {
	DI_USB_CMD_POWER_PORT = 0,
	DI_USB_CMD_POWER_HUB,
	DI_USB_CMD_OVERCURRENT,
	DI_USB_CMD_MAX
} DI_USB_CMD_E;

typedef struct {
	DI_USB_CMD_E	cmd;
	unsigned int	param;
} DI_USB_CMD_T;

typedef enum
{
	P_USB_INFO_TOPOLOGY = 0,
	P_USB_INFO_DEVICE,
	P_USB_INFO_PRODUCT,
	P_USB_INFO_STRING,
	P_USB_INFO_CONFIGURATION,
	P_USB_INFO_INTERFACE,
	P_USB_INFO_MAX
}P_USB_INFO_e;

typedef enum
{
	P_USB_PW_STATUS_NORMAL = 0,
	P_USB_PW_STATUS_OVER_CURRENT,
	P_USB_PW_STATUS_MAX
}P_USB_PW_STATUS_e;

typedef struct {
	DI_USB_Event_t 	event;
	usb_info_t		tUsbInfo;
}USB_INTR_INFO_T;

enum {
	OHCI_HOST = 0,
	EHCI_HOST,
	XHCI_HOST,
	UNKNOWN_HOST
};

typedef struct {
	int type;
	int hostnum;
	int maxchild;
} host_t;

typedef struct {
	int num_host;
	int num_ports;
	host_t **host;
} HOST_INFO_T;

/*******************************************************************
**  static variables
********************************************************************/
static HOST_INFO_T      tHostInfo;
static unsigned char	bInitialized;

static pfnDI_USB_NotifyFUNC 		CB_USBNotify = NULL;
static pfnDI_USB_ErrNotifyFunc		CB_USBErrNotify = NULL;
static unsigned long 				sHotplugTaskId;
static unsigned long				sIntrTaskId;
static unsigned long				s_UsbMsgId;
static unsigned long				sServiceTaskId;
static unsigned long				sUsbOcTimerId;
static unsigned long				sErrHandleTaskId;
static HBOOL						bRunning = FALSE;
static int 							s_usbmonFd;
static int 							*s_usbhub_fs;
static int							attached_sd_number;
static HINT32						s_ext_usb_dev_status;

struct usb_ctrltransfer {
	unsigned char  bRequestType;
	unsigned char  bRequest;
	unsigned short wValue;
	unsigned short wIndex;
	unsigned short wLength;
	unsigned long timeout;
	void *data;
};
#define IOCTL_USB_CONTROL	_IOWR('U', 0, struct usb_ctrltransfer)
#define UNUSED(x) (void)(x)

/*******************************************************************
**  Functions
********************************************************************/
static char *P_USB_FindString(char *buf, char *fstr, int len)
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

static int P_USB_InitHubDevice(void)
{
	int 	i;
	char 	device_name[40];

	/* open device for ioctl to control hub device */
	s_usbhub_fs = DD_MEM_Alloc(sizeof(int)*(tHostInfo.num_host));
	if (!s_usbhub_fs) {
		PrintError("[%s] Error. can't alloc\n", __FUNCTION__);
		return -1;
	}

	for(i=0; i<tHostInfo.num_host; i++) {
		VK_snprintf(device_name, 39, "%s/%03d/001", USB_DEVICE_DIR, i+1);

		PrintDebug("[%d] device : %s \n",i, device_name);
		s_usbhub_fs[i] = open(device_name, O_RDWR);
		if (s_usbhub_fs[i] < 0) {
			PrintError("[%s] Error. can't open device %s\n", __FUNCTION__, device_name);
			return -1;
		}
	}

	return 0;
}

static int P_USB_ControlMsg(int fs, int requesttype, int request,
	int value, int id, unsigned char *bytes, int size, int timeout)
{
	int 					ret;
	struct usb_ctrltransfer ctrl;

	ctrl.bRequestType = requesttype;
	ctrl.bRequest = request;
	ctrl.wValue = value;
	ctrl.wIndex = id;
	ctrl.wLength = size;

	ctrl.data = (void *)bytes;
	ctrl.timeout = timeout;

	ret = ioctl(fs, IOCTL_USB_CONTROL, &ctrl);
	if (ret < 0) {
		PrintDebug("[P_USB_ControlMsg] can't sending messagee %s\n", strerror(errno));
	}

	return ret;
}

static int P_USB_GetHubStatus(int dev_num, unsigned char *pStatus, int port)
{
	int ret;
	unsigned char status[4] = {0,};

	if( pStatus == NULL ) {
		PrintError("[%s] parm is NULL!!\n", __FUNCTION__);
		return -1;
	}

	/* if host is not alloc, ignore */
	if (!tHostInfo.host[dev_num]) {
		PrintError("[%s] Error! unknown host(%d)\n", __FUNCTION__, dev_num);
		return -1;
	}

	/* if host doesn't have port, ignore */
	if (!tHostInfo.host[dev_num]->maxchild) {
		PrintDebug("[%s] Error! doesn't have ports(maxchild:0)\n", __FUNCTION__);
		return -1;
	}

	/* clear status */
	*pStatus = 0;

	ret = P_USB_ControlMsg(s_usbhub_fs[dev_num],
							USB_GET_COMMAND,
							USB_REQ_GET_STATUS,
							0,
							port + 1,
							status,
							sizeof(unsigned char)*4,
							CMD_TIMEOUT);
	if (ret < 0) {
		PrintError("[%s] can't get messagee %s\n", __FUNCTION__, strerror(errno));
		return ret;
	}

	if(status[0] & 0x01) *pStatus |= USB_STATUS_CONNECT;
	if(status[0] & 0x02) *pStatus |= USB_STATUS_ENABLE;
	if(status[0] & 0x08) *pStatus |= USB_STATUS_OVERCURRENT;
	if(status[0] & 0x10) *pStatus |= USB_STATUS_RESET;
	if(status[2] & 0x08) *pStatus |= USB_STATUS_OVERCURRENT;

	return ret;
}

static int P_USB_HubOverCurrentClear(int dev_num, int portIndex)
{
	int ret;

	/* if host is not alloc, ignore */
	if (!tHostInfo.host[dev_num]) {
		PrintError("[%s] Error! unknown host(%d)\n", __FUNCTION__, dev_num);
		return -1;
	}

	/* if host doesn't have port, ignore */
	if (!tHostInfo.host[dev_num]->maxchild) {
		PrintDebug("[%s] Error! doesn't have ports(maxchild:0)\n", __FUNCTION__);
		return -1;
	}

	ret = P_USB_ControlMsg(s_usbhub_fs[dev_num],
							USB_SET_PORT_COMMAND,
							USB_REQ_CLEAR_FEATURE,
							USB_PORT_FEAT_C_OVER_CURRENT,
							portIndex + 1,
							NULL,
							0,
							CMD_TIMEOUT);

	if (ret < 0) {
		PrintError("[%s] can't send messagee %s\n", __FUNCTION__, strerror(errno));
	}

	return ret;
}

static int P_USB_Sync_Cache()
{
	HINT32 ret = -1, i, fd;
	HCHAR *szUsbDevName;

	szUsbDevName = (char*)DD_MEM_Alloc(sizeof(char)*USB_SD_NAME_LEN);
	if (NULL == szUsbDevName)
	{
		PrintError("[%s] fail to alloc usb device name variable\n", __FUNCTION__);
		goto done;
	}	

	for(i=0; i<MAX_NUM_USB_STORAGE; i++) {
		if(s_ext_usb_dev_status & (1<<i)) {

			VK_MEM_Memset(szUsbDevName, 0, USB_SD_NAME_LEN);
			VK_snprintf(szUsbDevName, USB_SD_NAME_LEN-1, "/dev/sd%c", (char)('a'+i));

			fd = open(szUsbDevName, O_RDWR);
			if (fd>=0) {
				ret = ioctl(fd, SCSI_IOCTL_HUMAX_SYNC, 0);
				if (ret < 0) {
					PrintDebug("[P_USB_Sync_Cache] can't sending message %s\n", strerror(errno));
					close(fd);
					goto done;
				}
				close(fd);
			}
			else {
				PrintDebug("Error open: %s\n", szUsbDevName);
			}
			VK_TASK_Sleep(100);
		}
	}

done :
	if (szUsbDevName)
		DD_MEM_Free(szUsbDevName);
	return ret;
}

static int P_USB_HubPowerOnOff(int dev_num, int portIndex, unsigned long onoff, P_USB_PW_STATUS_e status)
{
	int ret;

	/* if host is not alloc, ignore */
	if (!tHostInfo.host[dev_num]) {
		PrintError("[%s] Error! unknown host(%d)\n", __FUNCTION__, dev_num);
		return -1;
	}

	/* if host doesn't have port, ignore */
	if (!tHostInfo.host[dev_num]->maxchild) {
		PrintDebug("[%s] Error! doesn't have ports(maxchild:0)\n", __FUNCTION__);
		return -1;
	}

	if( onoff == TRUE ) {
		/* Power On */
		ret = P_USB_ControlMsg(s_usbhub_fs[dev_num],
								USB_SET_PORT_COMMAND,
								USB_REQ_SET_FEATURE,
								USB_PORT_FEAT_POWER,
								portIndex + 1,
								NULL,
								0,
								CMD_TIMEOUT);
		/* Hub Power On */
		ret = P_USB_ControlMsg(s_usbhub_fs[dev_num],
								USB_SET_HUB_COMMAND,
								USB_REQ_SET_FEATURE,
								USB_PORT_FEAT_POWER,
								portIndex + 1,
								NULL,
								0,
								CMD_TIMEOUT);
	}
	else {
		ret = P_USB_ControlMsg(s_usbhub_fs[dev_num],
								USB_SET_PORT_COMMAND,
								USB_REQ_CLEAR_FEATURE,
								USB_PORT_FEAT_C_OVER_CURRENT,
								portIndex + 1,
								NULL,
								0,
								CMD_TIMEOUT);
		if(status == P_USB_PW_STATUS_NORMAL) 
		{
			ret |= P_USB_ControlMsg(s_usbhub_fs[dev_num],
									USB_SET_PORT_COMMAND,
									USB_REQ_CLEAR_FEATURE,
									USB_PORT_FEAT_POWER,
									portIndex + 1,
									NULL,
									0,
									CMD_TIMEOUT);
		}
	}

	if (ret < 0) {
		PrintError("[%s] Error. can't send message(devnum:%d, port:%d, errno(%s))\n", __FUNCTION__, dev_num, portIndex + 1, strerror(errno));
	}

	return ret;
}

static unsigned int P_USB_GetPortNumByBus(int bus_idx, int port)
{
	unsigned int 	idx = 0;
	unsigned int	val = 0;
	int i;

	/* if host is not alloc, ignore */
	if (!tHostInfo.host[bus_idx]) {
		PrintError("[%s] Error. unknown host(%d)\n", __FUNCTION__, bus_idx);
		return 0;
	}

	for (i=0; i<bus_idx; i++) {
		idx += tHostInfo.host[bus_idx]->maxchild;
	}
	idx += port;

#if (CONFIG_USB_XHCI_HOSTCONTROLLER_CNT >= 1) && (CONFIG_USB_HOSTCONTROLLER_CNT == 1)
	switch(idx) {
	case 0: /* ehci #1, port #1 */
	case 2: /* ohci #1, port #1 */
	case 4: /* xhci #1, port #1 */
	case 5: /* xhci #1, port #1 */
		val = 1;
		break;
	case 1: /* ehci #1, port #2 */
	case 3: /* ohci #1, port #2 */
		val = 2;
		break;
	default:
		val = 0;
		break;
	}
#elif (CONFIG_USB_HOSTCONTROLLER_CNT == 1)
	switch(idx) {
	case 0: /* ehci #1, port #1 */
	case 2: /* ohci #1, port #1 */
		val = 1;
		break;
	case 1: /* ehci #1, port #2 */
	case 3: /* ohci #1, port #2 */
		val = 2;
		break;
	default:
		val = 0;
		break;
	}
#elif (CONFIG_USB_HOSTCONTROLLER_CNT == 2)
	switch(idx) {
	case 0: /* ehci #1, port #1 */
	case 4: /* ohci #1, port #1 */
		val = 1;
		break;
	case 1: /* ehci #2, port #1 */
	case 5: /* ohci #2, port #1 */
		val = 2;
		break;
	case 2: /* ehci #3, port #1 */
	case 6: /* ohci #3, port #1 */
		val = 3;
		break;
	case 3: /* ehci #4, port #1 */
	case 7: /* ohci #4, port #1 */
		val = 4;
		break;
	case 8: /* xhci #1, port #0 */
		val = 1;
		break;
	case 9: /* xhci #1, port #1 */
		val = 2;
		break;
	default:
		val = 0;
		break;
	}
#else
#error not implemented
#endif

	return val;
}

static void P_USB_GetPortNum(usb_info_t *pUsbInfo, char *str_dev_portnum, int buf_size)
{
	usb_info_t *pCurUsbInfo = pUsbInfo;
	char path[256];
	char devpath[256];
	char portName[8];
	int len=0;
	char *usbpath;
	int i;

	VK_memset(path, 0x00, sizeof(path));
	VK_memset(devpath, 0x00, sizeof(devpath));
	VK_memset(portName, 0x00, sizeof(portName));

	VK_snprintf(path,255,"/sys/block/%s/device",pCurUsbInfo->devName[0]);

	len=readlink(path,devpath,255); /*256->255: reserve the last byte for null-terninated string.(\0)*/
	if(len == 0) return;

	devpath[255] = '\0';

	usbpath=VK_strstr(devpath,"usb");
	if(usbpath == NULL) return;

	usbpath=VK_strchr(usbpath,'/');
	if(usbpath == NULL) return;

	usbpath++;
	for (i=0; i<(int)(sizeof(portName)-1); i++) {
		if (usbpath[i] != '/') {
    		portName[i] = usbpath[i];
		}
		else {
			break;
		}
	}

	if (str_dev_portnum && buf_size)
		VK_snprintf(str_dev_portnum, sizeof(buf_size), "%s", portName);

#if (CONFIG_USB_XHCI_HOSTCONTROLLER_CNT >= 1) && (CONFIG_USB_HOSTCONTROLLER_CNT == 1)
	if (VK_strncmp(portName, "1-1", 3) == 0) {
		//EHCI Controller
		pCurUsbInfo->ePortNum = 1;
	}
	else if (VK_strncmp(portName, "2-1", 3) == 0) {
		//EHCI Controller
		pCurUsbInfo->ePortNum = 2;
	}
	else if (VK_strncmp(portName, "3-1", 3) == 0) {
		//OHCI Controller
		pCurUsbInfo->ePortNum = 1;
	}
	else if (VK_strncmp(portName, "4-1", 3) == 0) {
		//OHCI Controller
		pCurUsbInfo->ePortNum = 2;
	}
	else if (VK_strncmp(portName, "6-1", 4) == 0) {
		//XHCI Controller
		pCurUsbInfo->ePortNum = 1;
	}
	else {
		pCurUsbInfo->ePortNum = 0;
		PrintError("[%s] Error!! Can't get usb port-number!! portname(%c%c%c)\n", __FUNCTION__, portName[0], portName[1], portName[2]);
		return;
	}
#elif (CONFIG_USB_HOSTCONTROLLER_CNT == 1)
	/* considered that each contoller(ehci, ohci) had two devices.
	    if not, we have to consider 1-3, 1-4,...,2-3,2-4,... so on. */
	if (VK_strncmp(portName, "1-1", 3) == 0) {
		//EHCI Controller
		pCurUsbInfo->ePortNum = 1;
	}
	else if (VK_strncmp(portName, "1-2", 3) == 0) {
		//EHCI Controller
		pCurUsbInfo->ePortNum = 2;
	}
	else if (VK_strncmp(portName, "2-1", 3) == 0) {
		//OHCI Controller
		pCurUsbInfo->ePortNum = 1;
	}
	else if (VK_strncmp(portName, "2-2", 3) == 0) {
		//OHCI Controller
		pCurUsbInfo->ePortNum = 2;
	}
	else {
		pCurUsbInfo->ePortNum = 0;
		PrintError("[%s] Error!! Can't get usb port-number!! portname(%c%c%c)\n", __FUNCTION__, portName[0], portName[1], portName[2]);
		return;
	}
#elif (CONFIG_USB_HOSTCONTROLLER_CNT == 2)
	/* considered that each contoller(ehci, ohci) had one port.
	    if not, we have to consider 5-1,5-2,.. on ehci and more on ohci */
	if (VK_strncmp(portName, "1-1", 3) == 0) {
		//EHCI Controller
		pCurUsbInfo->ePortNum = 1;
	}
	else if (VK_strncmp(portName, "2-1", 3) == 0) {
		//EHCI Controller
		pCurUsbInfo->ePortNum = 2;
	}
	else if (VK_strncmp(portName, "3-1", 3) == 0) {
		//EHCI Controller
		pCurUsbInfo->ePortNum = 3;
	}
	else if (VK_strncmp(portName, "4-1", 3) == 0) {
		//EHCI Controller
		pCurUsbInfo->ePortNum = 4;
	}
	else if (VK_strncmp(portName, "5-1", 3) == 0) {
		//OHCI Controller
		pCurUsbInfo->ePortNum = 1;
	}
	else if (VK_strncmp(portName, "6-1", 3) == 0) {
		//OHCI Controller
		pCurUsbInfo->ePortNum = 2;
	}
	else if (VK_strncmp(portName, "7-1", 3) == 0) {
		//OHCI Controller
		pCurUsbInfo->ePortNum = 3;
	}
	else if (VK_strncmp(portName, "8-1", 3) == 0) {
		//OHCI Controller
		pCurUsbInfo->ePortNum = 4;
	}
	else if (VK_strncmp(portName, "10-1", 4) == 0) {
		//XHCI Controller
		pCurUsbInfo->ePortNum = 1;
	}
	else if (VK_strncmp(portName, "10-2", 4) == 0) {
		//XHCI Controller
		pCurUsbInfo->ePortNum = 2;
	}
	else {
		pCurUsbInfo->ePortNum = 0;
		PrintError("[%s] Error!! Can't get usb port-number!! portname(%c%c%c)\n", __FUNCTION__, portName[0], portName[1], portName[2]);
		return;
	}
#else
#error not implemented
#endif

	PrintError("[%s] USB Port Connection: portname(%s) portnum(%d)\n", __FUNCTION__, portName, pCurUsbInfo->ePortNum);
	return;
}

static DRV_Error P_USB_ParsingStrFromSys(const HCHAR *pPath, const HCHAR *s1, HCHAR *pDest)
{
	HCHAR	szPath[256] = {0,};
	HINT32 fd, ret;

	if(NULL == pPath || NULL == s1){
		PrintError("[%s] Error. arg is null!!\n", __FUNCTION__);
		return DRV_ERR;
	}

	VK_snprintf(szPath, sizeof(szPath), "%s%s", pPath, s1);

	fd = open(szPath, O_RDONLY);
	if (fd >=0 ) {
		HCHAR data[256] = {0,};

		ret = read(fd, data, sizeof(data)-1);
		if (ret) {
			VK_strncpy(pDest, data, (ret-1));
		}
		close(fd);
	}
	else
	{
		pDest = NULL;
	}

	return DRV_OK;
}

static HUINT32  P_USB_ParsingNumFromSys(const HCHAR *pPath, const HCHAR *s1, HINT32 base)
{
	HCHAR	szPath[256] = {0,};
	HINT32 fd, ret;
	HUINT32 value = 0;

	if(NULL == pPath || NULL == s1) {
		PrintError("[%s] Error. arg is null!!\n", __FUNCTION__);
		return 0;
	}

	VK_snprintf(szPath, sizeof(szPath), "%s%s", pPath, s1);

	fd = open(szPath, O_RDONLY);
	if (fd >=0 ) {
		HCHAR data[256] = {0,};

		ret = read(fd, data, sizeof(data)-1);
		if (ret) {
			value = strtol(data, NULL, base);
		}
		close(fd);
	}

	return value;
}

static HUINT32  P_USB_CheckLev(const HCHAR *str)
{
	HUINT32 lev = 1;

	if(NULL == str) {
		PrintError("[%s] Error. arg is null!!\n", __FUNCTION__);
		return 0;
	}

	while (*str) {
		if (*str == '.')
			lev++;
		str++;
	}

	return lev;
}

static HUINT32  P_USB_CheckPort(const HCHAR *str, const HUINT32 nlev)
{
	HUINT32 port = 0, lev = 1;

	if(NULL == str) {
		PrintError("[%s] Error. arg is null!!\n", __FUNCTION__);
		return 0;
	}

	while (*str) {
		if(nlev == 1) {
			if(*str == '-') break;
		}
		else{
			if(*str == '.') lev++;
			if(nlev == lev) break;
		}
		str++;
	}

	str++;	/* Skip '-' and '.' */
	port = strtol(str, NULL, 10);

	return port-1;
}

static DRV_Error P_USB_CheckPrnt(const HCHAR *str, const HUINT32 nlev, HCHAR *pDest)
{
	HCHAR *ptr = pDest;
	HUINT32 lev = 1;

	if(NULL == str || NULL == pDest) {
		PrintError("[%s] Error. arg is null!!\n", __FUNCTION__);
		return DRV_ERR;
	}

	while (*str)
	{
		if (*str == '.') lev++;

		if(nlev == lev)
			break;
		else
			*ptr++ = *str++;
	}
	*ptr = '\0';

	return DRV_OK;
}

static DRV_Error P_USB_GetUsbDir(HUINT32 ulHostId, HUINT32 ulDevId, HCHAR *pUsbDir)
{
	HUINT32	unHostNum = 0, unDeviceNum = 0;
	DIR 	*dir_info;
	struct	dirent *dir_entry;

	if(NULL == pUsbDir) {
		PrintError("[%s] Error. arg is null!!\n", __FUNCTION__);
		return DRV_ERR;
	}

	dir_info = opendir(USB_SYS_DIR);
	if (!dir_info) {
		PrintError("[%s] Can't open dir(%s)\n", __FUNCTION__, USB_SYS_DIR);
		return DRV_ERR;
	}
	while((dir_entry = readdir(dir_info)) != NULL)
	{
		if(NULL == dir_entry->d_name)
		{
			PrintDebug("[%s]_%d dir_entry->d_name NULL?\n",__func__,__LINE__);
			continue;
		}

		if( dir_entry->d_name[0] == '.' ||
				VK_strncmp(dir_entry->d_name, "usb", 3) == 0 ||
				VK_strchr(dir_entry->d_name, (HINT32)':') != NULL ) {
			continue;
		}

		unHostNum = strtol(dir_entry->d_name, NULL, 10);
		if(ulHostId == unHostNum)
		{
			unDeviceNum++;
			if(ulDevId == unDeviceNum) {
				VK_strncpy(pUsbDir, dir_entry->d_name, sizeof(dir_entry->d_name));
			}
		}
	}
	closedir(dir_info);

	return DRV_OK;
}

static DRV_Error  P_USB_GetUsbHostInfo(HUINT32 ulHostId, DI_USB_HOSTCONTROLLER_INFO_t *pstUsbHostInfo)
{
	DRV_Error eDrvErr;
	HCHAR szPath[256] = {0,};
	HCHAR szBuf[256] = {0,};
	HUINT32	unHostNum, unDeviceNum = 0;
	DIR		*dir_info;
	struct	dirent *dir_entry;

	dir_info = opendir(USB_SYS_DIR);
	if (!dir_info) {
		PrintError("[%s] Can't open dir(%s)\n", __FUNCTION__, USB_SYS_DIR);
		return DRV_ERR;
	}
	while((dir_entry = readdir(dir_info)) != NULL)
	{
		if(dir_entry->d_name == NULL)
		{
			PrintDebug("[%s]_%d dir_entry->d_name NULL?\n",__func__,__LINE__);
			continue;
		}

		if( dir_entry->d_name[0] == '.' ||
				VK_strncmp(dir_entry->d_name, "usb", 3) == 0 ||
				VK_strchr(dir_entry->d_name, (HINT32)':') != NULL ) {
			continue;
		}

		unHostNum = strtol(dir_entry->d_name, NULL, 10);
		if(ulHostId == unHostNum)
		{
			unDeviceNum++;
		}
	}
	closedir(dir_info);

	/* USB device numver of entries */
	pstUsbHostInfo->unDeviceNumOfEntries = unDeviceNum;

	VK_snprintf(szPath, sizeof(szPath), "%s/usb%d/", USB_SYS_DIR, ulHostId);

	/* USB Host Type : OHCI, EHCI, UHCI, xHCI */
	eDrvErr = P_USB_ParsingStrFromSys(szPath, "serial", szBuf);
	if(DRV_OK != eDrvErr) {
		PrintError("[%s] P_USB_ParsingStrFromSys Error!\n", __FUNCTION__);
		return DRV_ERR;
	}
	if (NULL != szBuf) {
		if (VK_strstr(szBuf, "ohci") != NULL ) {
			pstUsbHostInfo->eHostTye = DI_USB_HOST_OHCI;
		}
		else if (VK_strstr(szBuf, "ehci") != NULL ) {
			pstUsbHostInfo->eHostTye = DI_USB_HOST_EHCI;
		}
		else if (VK_strstr(szBuf, "uhci") != NULL ) {
			pstUsbHostInfo->eHostTye = DI_USB_HOST_UHCI;
		}
		else if (VK_strstr(szBuf, "xhci") != NULL ) {
			pstUsbHostInfo->eHostTye = DI_USB_HOST_XHCI;
		}
		else{
			PrintError("[%s] Error. unknown host(%s)\n", __FUNCTION__, szBuf);
		}
	}

	/* USB Host Name : ex) Linux 2.6.37-3.5 ehci_hcd */
	eDrvErr = P_USB_ParsingStrFromSys(szPath, "manufacturer", pstUsbHostInfo->aucHostName);
	if(DRV_OK != eDrvErr) {
		PrintError("[%s] P_USB_ParsingStrFromSys Error!\n", __FUNCTION__);
		return DRV_ERR;
	}

	/* USB Host Version : 1.1, 2.0 */
	eDrvErr = P_USB_ParsingStrFromSys(szPath, "version", pstUsbHostInfo->aucHostUsbVer);
	if(DRV_OK != eDrvErr) {
		PrintError("[%s] P_USB_ParsingStrFromSys Error!\n", __FUNCTION__);
		return DRV_ERR;
	}

	return DRV_OK;
}

static DRV_Error P_USB_GetUsbDeviceInfo(HUINT32 ulHostId, HUINT32 ulDevId, DI_USB_DEVICE_INFO_t *pstUsbDevInfo)
{
	DRV_Error eDrvErr = DRV_ERR;
	HCHAR	szUsbPath[256] = {0,};
	HCHAR	szPath[256] = {0,};
	HCHAR	szBuf[256] = {0,};
	HCHAR	szUsbDir[256] = {0,};
	HUINT32	unAtr=0;

	eDrvErr = P_USB_GetUsbDir(ulHostId, ulDevId, szUsbDir);
	if(DRV_OK != eDrvErr) {
		PrintError("[%s] P_USB_GetUsbDir Error!\n", __FUNCTION__);
		return DRV_ERR;
	}

	VK_snprintf(szUsbPath, sizeof(szUsbPath), "%s/%s/", USB_SYS_DIR, szUsbDir);

	/* Check level in topology for this bus */
	pstUsbDevInfo->unDevLev = P_USB_CheckLev(szUsbDir);

	/* Check device number */
	pstUsbDevInfo->unDevNum = P_USB_ParsingNumFromSys(szUsbPath, "devnum", 10);

	/* Check device class */
	pstUsbDevInfo->ucDevClass = P_USB_ParsingNumFromSys(szUsbPath, "bDeviceClass", 16);

	/* Check sub class */
	pstUsbDevInfo->ucDevSubClass = P_USB_ParsingNumFromSys(szUsbPath, "bDeviceSubClass", 16);

	/* Check release number */
	pstUsbDevInfo->unDevVer = P_USB_ParsingNumFromSys(szUsbPath, "bcdDevice", 16);

	/* Check device protocol code */
	pstUsbDevInfo->ucDevProtocol = P_USB_ParsingNumFromSys(szUsbPath, "bDeviceProtocol", 16);

	/* Check product ID */
	pstUsbDevInfo->unDevProductId = P_USB_ParsingNumFromSys(szUsbPath, "idProduct", 16);

	/* Check vendor ID */
	pstUsbDevInfo->unDevVendorId = P_USB_ParsingNumFromSys(szUsbPath, "idVendor", 16);

	/* Check port */
	pstUsbDevInfo->unDevPort = P_USB_CheckPort(szUsbDir, pstUsbDevInfo->unDevLev);

	/* Check maxchildren */
	pstUsbDevInfo->unDevMxch = P_USB_ParsingNumFromSys(szUsbPath, "maxchild", 10);

	/* Check configuration number of entries */
	pstUsbDevInfo->unDevConfigurationNumOfEntries = P_USB_ParsingNumFromSys(szUsbPath, "bNumConfigurations", 10);

	/* Check USB version */
	eDrvErr = P_USB_ParsingStrFromSys(szUsbPath, "version", pstUsbDevInfo->aucDevUsbVer);
	if(DRV_OK != eDrvErr) {
		PrintError("[%s] P_USB_ParsingStrFromSys Error!\n", __FUNCTION__);
		return DRV_ERR;
	}

	/* Check manufacturer */
	eDrvErr = P_USB_ParsingStrFromSys(szUsbPath, "manufacturer", pstUsbDevInfo->aucDevManufacturer);
	if(DRV_OK != eDrvErr) {
		PrintError("[%s] P_USB_ParsingStrFromSys Error!\n", __FUNCTION__);
		return DRV_ERR;
	}

	/* Check product */
	eDrvErr = P_USB_ParsingStrFromSys(szUsbPath, "product", pstUsbDevInfo->aucDevProduct);
	if(DRV_OK != eDrvErr) {
		PrintError("[%s] P_USB_ParsingStrFromSys Error!\n", __FUNCTION__);
		return DRV_ERR;
	}

	/* Check serialNumber */
	eDrvErr = P_USB_ParsingStrFromSys(szUsbPath, "serial", pstUsbDevInfo->aucDevSerialNumber);
	if(DRV_OK != eDrvErr) {
		PrintError("[%s] P_USB_ParsingStrFromSys Error!\n", __FUNCTION__);
		return DRV_ERR;
	}

	/* Check speed */
	eDrvErr = P_USB_ParsingStrFromSys(szUsbPath, "speed", szBuf);
	if(DRV_OK != eDrvErr) {
		PrintError("[%s] P_USB_ParsingStrFromSys Error!\n", __FUNCTION__);
		return DRV_ERR;
	}
	if (NULL != szBuf) {
		if (VK_strstr(szBuf, "1.5") != NULL ) {
			pstUsbDevInfo->eDevSpeed = DI_USB_SPEED_1;
		}
		else if (VK_strstr(szBuf, "12") != NULL ) {
			pstUsbDevInfo->eDevSpeed = DI_USB_SPEED_12;
		}
		else if (VK_strstr(szBuf, "480") != NULL ) {
			pstUsbDevInfo->eDevSpeed = DI_USB_SPEED_480;
		}
		else if (VK_strstr(szBuf, "5000") != NULL ) {
			pstUsbDevInfo->eDevSpeed = DI_USB_SPEED_5000;
		}
		else{
			PrintError("[%s] Error. unknown USB Speed(%s)\n", __FUNCTION__, szBuf);
		}
	}

	/* Check parent */
	if(pstUsbDevInfo->unDevLev == 1) {
		VK_snprintf(szPath, sizeof(szPath), "%s/usb%d/", USB_SYS_DIR, ulHostId);
		pstUsbDevInfo->ucDevParent 	= P_USB_ParsingNumFromSys(szPath, "devnum", 10);
	}
	else {
		eDrvErr = P_USB_CheckPrnt(szUsbDir, pstUsbDevInfo->unDevLev, szBuf);
		if(DRV_OK != eDrvErr) {
			PrintError("[%s] P_USB_ParsingStrFromSys Error!\n", __FUNCTION__);
			return DRV_ERR;
		}

		VK_snprintf(szPath, sizeof(szPath), "%s/%s/", USB_SYS_DIR, szBuf);
		pstUsbDevInfo->ucDevParent 	= P_USB_ParsingNumFromSys(szPath, "devnum", 10);
	}

	/* Check suspended */
	eDrvErr = P_USB_ParsingStrFromSys(szUsbPath, "power/runtime_status", szBuf);
	if(DRV_OK != eDrvErr) {
		PrintError("[%s] P_USB_ParsingStrFromSys Error!\n", __FUNCTION__);
		return DRV_ERR;
	}
	if (NULL != szBuf) {
		if (VK_strstr(szBuf, "suspended") != NULL ) {
			pstUsbDevInfo->bDevIsSuspended= TRUE;	/* IsSuspended */
		}
		else {
			pstUsbDevInfo->bDevIsSuspended = FALSE;
		}
	}

	/* Check IsSelfPowered */
	unAtr = P_USB_ParsingNumFromSys(szUsbPath, "bmAttributes", 16);
	/* If a device is self powered, it sets D6. Bit (http://www.beyondlogic.org/usbnutshell/usb5.shtml) */
	if(unAtr & 0x40) {
		pstUsbDevInfo->bDevIsSelfPowered = TRUE;
	}
	else {
		pstUsbDevInfo->bDevIsSelfPowered = FALSE;
	}

	return DRV_OK;
}

static DRV_Error P_USB_GetUsbDevConfigurationsInfo(HUINT32 ulHostId, HUINT32 ulDevId, DI_USB_DEVICE_CONFIGURATION_t *pstUsbDevConfiguInfo)
{
	DRV_Error eDrvErr = DRV_ERR;
	HCHAR	szUsbPath[256] = {0,};
	HCHAR	szUsbDir[256] = {0,};
	HUINT32 unDevConNum, unDevConMaxNum;

	eDrvErr = P_USB_GetUsbDir(ulHostId, ulDevId, szUsbDir);
	if(DRV_OK != eDrvErr) {
		PrintError("[%s] P_USB_GetUsbDir Error!\n", __FUNCTION__);
		return DRV_ERR;
	}

	VK_snprintf(szUsbPath, sizeof(szUsbPath), "%s/%s/", USB_SYS_DIR, szUsbDir);
	unDevConMaxNum = P_USB_ParsingNumFromSys(szUsbPath, "bNumConfigurations", 10);

	for(unDevConNum=0; unDevConNum<unDevConMaxNum; unDevConNum++)
	{
		/* Check configuration number */
		pstUsbDevConfiguInfo[unDevConNum].unDevConfigurationNum = P_USB_ParsingNumFromSys(szUsbPath, "bConfigurationValue", 10);

		/* Check interface number of entries */
		pstUsbDevConfiguInfo[unDevConNum].unDevInterfaceNumOfEntries = P_USB_ParsingNumFromSys(szUsbPath, "bNumInterfaces", 10);
	}

	return DRV_OK;
}

static DRV_Error P_USB_GetUsbDevInterfacesInfo(HUINT32 ulHostId, HUINT32 ulDevId, DI_USB_DEVICE_INTERFACE_t *pstUsbDevInterfaceInfo)
{
	DRV_Error eDrvErr = DRV_ERR;
	HCHAR	szUsbPath[256] = {0,};
	HCHAR	szUsbDir[256] = {0,};
	HUINT32 unDevIntNum, unDevIntMaxNum;

	eDrvErr = P_USB_GetUsbDir(ulHostId, ulDevId, szUsbDir);
	if(DRV_OK != eDrvErr) {
		PrintError("[%s] P_USB_GetUsbDir Error!\n", __FUNCTION__);
		return DRV_ERR;
	}

	VK_snprintf(szUsbPath, sizeof(szUsbPath), "%s/%s/", USB_SYS_DIR, szUsbDir);
	unDevIntMaxNum = P_USB_ParsingNumFromSys(szUsbPath, "bNumInterfaces", 10);

	VK_snprintf(szUsbPath, sizeof(szUsbPath), "%s/%s:1.0/", USB_SYS_DIR, szUsbDir);
	for(unDevIntNum=0; unDevIntNum<unDevIntMaxNum; unDevIntNum++)
	{
		/* Check interface number */
		pstUsbDevInterfaceInfo[unDevIntNum].unDevInterfaceNum = P_USB_ParsingNumFromSys(szUsbPath, "bInterfaceNumber", 10);

		/* Check interface class */
		pstUsbDevInterfaceInfo[unDevIntNum].ucDevInterfaceClass = P_USB_ParsingNumFromSys(szUsbPath, "bInterfaceClass", 16);

		/* Check interface sub class */
		pstUsbDevInterfaceInfo[unDevIntNum].ucDevInterfaceSubClass = P_USB_ParsingNumFromSys(szUsbPath, "bInterfaceSubClass", 16);

		/* Check interface protocol */
		pstUsbDevInterfaceInfo[unDevIntNum].ucDevInterfaceProtocol = P_USB_ParsingNumFromSys(szUsbPath, "bInterfaceProtocol", 16);
	}

	return DRV_OK;
}

static inline char *P_USB_GetHostTypeStr(int type)
{
	switch(type) {
	case OHCI_HOST:
		return "ohci";
	case EHCI_HOST:
		return "ehci";
	case XHCI_HOST:
		return "xhci";
	default:
		break;
	}

	return "unknown";
}

static void P_USB_OCTimerCb(unsigned long tid, void *arg)
{
	DI_USB_CMD_T	msg;
	int				ret;

	UNUSED(tid);
	UNUSED(arg);

	msg.cmd = DI_USB_CMD_OVERCURRENT;
	msg.param = 0;

	ret = VK_MSG_SendTimeout(s_UsbMsgId, &msg, sizeof(DI_USB_CMD_T), 0);
	if (ret != VK_OK) {
		PrintError("[%s] VK_MSG_Send Error!\n", __FUNCTION__);
	}

	return;
}

static int P_USB_Initialize(void)
{
	char szLink[256] = {0,};
	char szPath[256] = {0,};
	int	err;
	int nLen;
	int fd;
	int i;
	DIR	*dir;
	struct dirent *ent;

	if (bInitialized) {
		return 0;
	}

#if defined(CONFIG_USB_MODULE)
	VK_TASK_Sleep(200);
#endif

	/* parsing usb host device to get total host count*/
	dir = opendir(USB_SYS_DIR);
	if (!dir) {
		PrintError("[%s] Can't open dir(%s)\n", __FUNCTION__, USB_SYS_DIR);
		return -1;
	}
	while((ent = readdir(dir)) != NULL) {
		/* get total host count */
		if (VK_strncmp(ent->d_name, "usb", 3) == 0) {
			tHostInfo.num_host++;
		}
	}
	closedir(dir);

	/* if no host are shown */
	if (!tHostInfo.num_host) {
		PrintError("[%s] Error! can't get host count\n", __FUNCTION__);
		return -1;
	}

	/* alloc tHostInfo.host pointer */
	if (!tHostInfo.host) {
		tHostInfo.host = (host_t**)DD_MEM_Alloc(sizeof(host_t*) * tHostInfo.num_host);
		if (!tHostInfo.host) {
			PrintError("[%s] Error. can't alloc tHostInfo.host\n", __FUNCTION__);
			return -1;
		}
		VK_MEM_Memset(tHostInfo.host, 0, sizeof(host_t*) * tHostInfo.num_host);
	}

	/* get host types(xhci, ehci and ohci) and maxchild count */
	for (i=0; i<tHostInfo.num_host; i++) {
		/* read link string to get host type */
		VK_snprintf(szPath, sizeof(szPath), "%s/usb%d", USB_SYS_DIR, i+1);
		nLen = readlink(szPath, szLink, sizeof(szLink)-1);

		if (nLen > 0 && (unsigned long)nLen < sizeof(szLink)) {
			szLink[nLen] = 0;
			if (!tHostInfo.host[i]) {
				tHostInfo.host[i] = DD_MEM_Alloc(sizeof(host_t));
				if (!tHostInfo.host[i]) {
					PrintError("[%s] Error. can't alloc tHostInfo.host[%d]\n", __FUNCTION__, i);
					continue;
				}
			}

			if (VK_strstr(szLink, "ehci")) {
				tHostInfo.host[i]->type = EHCI_HOST;
			}
			else if (VK_strstr(szLink, "ohci")) {
				tHostInfo.host[i]->type = OHCI_HOST;
			}
			else if (VK_strstr(szLink, "xhci")) {
				tHostInfo.host[i]->type = XHCI_HOST;
			}
			else {
				tHostInfo.host[i]->type = UNKNOWN_HOST;
			}
			/* this value means host bus number */
			tHostInfo.host[i]->hostnum = (int)(i+1);

			/* get maxchild count */
			tHostInfo.host[i]->maxchild = 0;
			VK_snprintf(szPath, sizeof(szPath), "%s/usb%d/maxchild", USB_SYS_DIR, i+1);
			fd = open(szPath, O_RDONLY);
			if (fd >=0 ) {
				char data[4] = {0,};

				nLen = read(fd, data, sizeof(data)-1);
				if (nLen) {
					tHostInfo.host[i]->maxchild = (int)strtol(data, NULL, 10);
				}
				close(fd);
			}

			/* calcurate total port count */
			tHostInfo.num_ports += tHostInfo.host[i]->maxchild;
		}
	}

#if defined(CONFIG_DEBUG)
	/* show informations */
	PrintInfo("[%s] ******** USB Informations *********\n", __FUNCTION__);
	for (i=0; i<tHostInfo.num_host; i++) {
		if (tHostInfo.host[i]) {
			PrintInfo("[%s] host(%d), type(%s), maxchild(%d)\n",
				__FUNCTION__,
				tHostInfo.host[i]->hostnum,
				P_USB_GetHostTypeStr(tHostInfo.host[i]->type),
				tHostInfo.host[i]->maxchild);
		}
		else {
			PrintInfo("[%s] host(null), type(null), maxchild(null)\n", __FUNCTION__);
		}
	}
	PrintInfo("[%s] ***********************************\n", __FUNCTION__);
#endif

	err = P_USB_InitHubDevice();
	if( err < 0 ) {
		PrintError("[%s] Error(0x%x) in P_USB_InitHubDevice()\n", __FUNCTION__, err);
		return -1;
	}

	bInitialized = 1;
	return 0;
}

static void P_USB_UpdateDevice(char *sdname)
{
	int fd;
	int rc;
	struct stat st;
	char devpath[16] = {0,};

	if (!sdname) {
		goto ret;
	}

	VK_snprintf(devpath, sizeof(devpath), "/dev/%s", sdname);
	fd = open(devpath, O_RDONLY);
	if (fd < 0) {
		goto ret;
	}

	rc = fstat(fd, &st);
	if (rc < 0) {
		PrintError("[%s] Error. fstat error\n", __FUNCTION__);
	}
	close(fd);

ret:
	return;
}

static void P_USB_HotplugTask(void *pvParam)
{
#define DEV_NAME_LEN	8
	int 			ext_disk_status;
	int 			disk_status_backup = 0;
	unsigned int 	i;
	int 			ret;
	int				loop;
	USB_INTR_INFO_T tIntrInfo;
	char            szPortNum[8];
	char            szStrBuf[256];
	int             fd;
	int             len;

	BSTD_UNUSED(pvParam);

	while(CB_USBNotify == NULL) {
		//wait till all callback registered before running main loop
		VK_TASK_Sleep(100);
	}

	s_usbmonFd = open("/dev/hmxsdcon",O_RDWR);
	if(s_usbmonFd < 0) {
		PrintError("[%s] failed to open device file\n", __FUNCTION__);
		return;
	}

	bRunning = TRUE;
	while (bRunning) {
		ext_disk_status = 0;

		ret = read(s_usbmonFd,&ext_disk_status, sizeof(int));
		if (ret != sizeof(int)) continue;

		for(i=0; i<MAX_NUM_USB_STORAGE; i++) {
			if((disk_status_backup & (1<<i)) != (ext_disk_status & (1<<i))) {

				VK_MEM_Memset(&tIntrInfo, 0, sizeof(USB_INTR_INFO_T));
				tIntrInfo.tUsbInfo.devCount = 1;
				tIntrInfo.tUsbInfo.devName = (char**)DD_MEM_Alloc(sizeof(char*)*tIntrInfo.tUsbInfo.devCount);
				if (!tIntrInfo.tUsbInfo.devName) {
					PrintError("[%s] fail to alloc usbinfo variable\n", __FUNCTION__);
					break;
				}
				tIntrInfo.tUsbInfo.devName[0] = (char*)DD_MEM_Alloc(sizeof(char)*DEV_NAME_LEN);
				if (!tIntrInfo.tUsbInfo.devName[0]) {
					PrintError("[%s] fail to alloc usbinfo variable\n", __FUNCTION__);
					DD_MEM_Free(tIntrInfo.tUsbInfo.devName);
					break;
				}

				VK_MEM_Memset(tIntrInfo.tUsbInfo.devName[0], 0, DEV_NAME_LEN);
				VK_snprintf(tIntrInfo.tUsbInfo.devName[0], DEV_NAME_LEN-1, "sd%c", (char)('a'+i));
				tIntrInfo.tUsbInfo.devNameSize = VK_strlen(tIntrInfo.tUsbInfo.devName[0]) + 1;
				P_USB_GetPortNum(&tIntrInfo.tUsbInfo, szPortNum, sizeof(szPortNum));

				tIntrInfo.tUsbInfo.eSpeed = DI_USB_SPEED_UNKNOWN;

				if((disk_status_backup & (1<<i)) != 0) {
					/* SDx disconnected */
					PrintError("[%s] usb storage disconnected! portnum(%d) -> call callback function\n", __FUNCTION__, tIntrInfo.tUsbInfo.ePortNum);

					/* Remove sd device */
					attached_sd_number &= ~(1 << i);

					tIntrInfo.event = DI_USB_Event_Extract;
					VK_INTR_Event(sIntrTaskId, (void*)&tIntrInfo);
				}
				else {
					/* SDx connected */
					loop = 0;
					while(tIntrInfo.tUsbInfo.ePortNum == 0) {
						/* sleep until getting portnum */
						VK_TASK_Sleep(20);
						if (++loop > 10) break;
						P_USB_GetPortNum(&tIntrInfo.tUsbInfo, szPortNum, sizeof(szPortNum));
					}
					PrintError("[%s] usb storage connected! portnum(%d) -> call callback function\n", __FUNCTION__, tIntrInfo.tUsbInfo.ePortNum);

					/* when connecting, some usb disk doen't update partition table on kernel before accessing dev.
					    so, we make kernel update partition table through accessing device */
					P_USB_UpdateDevice(tIntrInfo.tUsbInfo.devName[0]);

					/* get usb speed */
					len = 0;
					VK_snprintf(szStrBuf, sizeof(szStrBuf), "/sys/bus/usb/devices/%s/speed", szPortNum);
					fd = open(szStrBuf, O_RDONLY);
					if (fd >= 0)
					{
						len = read(fd, szStrBuf, 4);
						close(fd);
					}

					if (len > 0)
					{
						szStrBuf[len] = 0;
						if(VK_strncmp(szStrBuf, "1.5", 3) == 0)
						{
							tIntrInfo.tUsbInfo.eSpeed = DI_USB_SPEED_1;
						}
						else if(VK_strncmp(szStrBuf, "12", 2) == 0)
						{
							tIntrInfo.tUsbInfo.eSpeed = DI_USB_SPEED_12;
						}
						else if(VK_strncmp(szStrBuf, "480", 3) == 0)
						{
							tIntrInfo.tUsbInfo.eSpeed = DI_USB_SPEED_480;
						}
						else if(VK_strncmp(szStrBuf, "5000", 4) == 0)
						{
							tIntrInfo.tUsbInfo.eSpeed = DI_USB_SPEED_5000;
						}
					}

					/* Add sd device */
					attached_sd_number |= (1 << i);

					tIntrInfo.event = DI_USB_Event_Insert;
					VK_INTR_Event(sIntrTaskId, (void*)&tIntrInfo);
				}
			}
		}
		/* backup disk status */
		disk_status_backup = ext_disk_status;
		s_ext_usb_dev_status = ext_disk_status;
	}

	PrintError("[%s] Error! USB hotplug task is terminated!\n", __FUNCTION__);

	close(s_usbmonFd);
	s_usbmonFd = 0;

	return;
}

static void P_USB_ServiceTask(void *pParam)
{
	int				err;
	DI_USB_CMD_T	msg;
	int				bus, port;
	unsigned char	status_reg;
	unsigned int	*oc_clear = NULL;
	unsigned int	port_powered = 0;
	int				idx;
	USB_INTR_INFO_T tIntrInfo;

#if (CONFIG_USB_HOSTCONTROLLER_CNT == 1)
#if defined(CONFIG_USB_SUBPORT_DISABLE_MASK)
	const unsigned int usb_disable_mask = CONFIG_USB_SUBPORT_DISABLE_MASK;
#else
	const unsigned int usb_disable_mask = 0x00;
#endif
#endif

	UNUSED(pParam);

	/* create timer for checking over current on usb */
	err = VK_TIMER_EventEvery(USB_OCP_CHK_INTERVAL,
							P_USB_OCTimerCb,
							NULL,
							0,
							&sUsbOcTimerId);
	if (err != VK_OK) {
		PrintError("[%s] Can't create timer for OC! \n", __FUNCTION__);
	}

	while(1) {
		/* wait for message */
		if (VK_MSG_Receive(s_UsbMsgId, &msg, sizeof(DI_USB_CMD_T)) != VK_OK)
			continue;

		switch(msg.cmd) {
		case DI_USB_CMD_POWER_HUB:
		case DI_USB_CMD_POWER_PORT:
			/* initialize usb module once */
			if (!bInitialized && msg.param==(unsigned int)DI_USB_PWR_ON) {
				err = P_USB_Initialize();
				if (err < 0) {
					PrintError("[%s] Error! Can't Initialize USB Driver\n", __FUNCTION__);
					break;
				}

				/* alloc variable of oc clear count */
				if (!oc_clear) {
					/* this is for modifying prevent warning */
					oc_clear = DD_MEM_Alloc(sizeof(unsigned int)*tHostInfo.num_ports);
					if (!oc_clear) {
						PrintError("[%s] Can't alloc! \n", __FUNCTION__);
					}
					else {
						VK_MEM_Memset(oc_clear, 0, sizeof(unsigned int)*tHostInfo.num_ports);
					}
				}
			}
#if defined(CONFIG_TEMP_HDR4000T)
			else if (bInitialized && msg.param==(unsigned int)DI_USB_PWR_ON) {
				PrintError("S3->Wakeup usb power delay to 10 sec for becoming normal status of Internal HDD\n");
				VK_TASK_Sleep(10000);
			}
#endif

#if defined(CONFIG_WIFI) && defined(CONFIG_BCM43236)
			if (msg.param == 0)
			{
#if defined(CONFIG_WIFI_AARDVARK)
				VK_SYSTEM_Command("rmmod wl");
				VK_TASK_Sleep(500);
				VK_SYSTEM_Command("rmmod bcm_dbus");
				VK_TASK_Sleep(500);
#else
				VK_SYSTEM_Command("rmmod dhd");
				VK_TASK_Sleep(500);
#endif
			}
#endif

#if defined(CONFIG_BLUETOOTH)
			if (msg.param == 0)
			{
				VK_SYSTEM_Command("killall bsa_server");
				VK_TASK_Sleep(500);
				VK_SYSTEM_Command("rmmod btusb");
				VK_TASK_Sleep(500);
				VK_SYSTEM_Command("rmmod bthid");
				VK_TASK_Sleep(500);
			}
#endif

			if(msg.param == 0) {
				err = P_USB_Sync_Cache();
				if (err < 0) {
						PrintError("[%s] Error! ioctl in USB devices\n", __FUNCTION__);
				}
			}
			/* turn on all ports power */
			for (bus=0; bus<tHostInfo.num_host; bus++) {
				for (port = 0; port < tHostInfo.host[bus]->maxchild; port++) {
#if (CONFIG_USB_HOSTCONTROLLER_CNT == 1)
					if( usb_disable_mask & (0x1<<port) )
						continue;
#endif
					err = P_USB_HubPowerOnOff(bus, port, (unsigned long)msg.param, P_USB_PW_STATUS_NORMAL);
					if (err < 0) {
						PrintError("[%s] Error! can't control port power(host:%d, port:%d on:%d)\n", __FUNCTION__, bus+1, port+1, (unsigned int)msg.param);
					}
					else {
						VK_TASK_Sleep(USB_POWER_SHOOTING_INTERVAL);
					}
				}
			}
			port_powered = msg.param;
			break;

		case DI_USB_CMD_OVERCURRENT:
			if (!bInitialized || !port_powered || !oc_clear) {
				break;
			}

			for(bus=0, idx=0; bus<tHostInfo.num_host; bus++) {
				/* if host[bus] is not alloc, ignore this host[bus] */
				if (!tHostInfo.host[bus]) {
					PrintError("[%s] Error. unknown host(%d)\n", __FUNCTION__, bus);
					continue;
				}

				for (port=0; port<tHostInfo.host[bus]->maxchild; port++, idx++) {
#if (CONFIG_USB_HOSTCONTROLLER_CNT == 1)
					if( usb_disable_mask & (0x1<<port) )
						continue;
#endif
					/* get value of status register */
					err = P_USB_GetHubStatus(bus, &status_reg, port);
					if (err < 0) {
						PrintError("[%s] Error! can't get HUB state\n", __FUNCTION__);
						continue;
					}
					/* check whether over current occured or not */
					if (status_reg & USB_STATUS_OVERCURRENT) {
						/* turn port power off */
						err = P_USB_HubPowerOnOff(bus, port, 0, P_USB_PW_STATUS_OVER_CURRENT);
						if (err < 0) {
							PrintError("[%s] Error! can't control port power\n", __FUNCTION__);
							continue;
						}

						/* oc occured. clear oc flag */
						err = P_USB_HubOverCurrentClear(bus, port);
						if (err < 0) {
							PrintError("[%s] Error! can't clear overcurrent bit\n", __FUNCTION__);
							continue;
						}

						if(CB_USBNotify && oc_clear[idx] == 0) {
							VK_MEM_Memset(&tIntrInfo, 0x00, sizeof(USB_INTR_INFO_T));
							tIntrInfo.event = DI_USB_Event_OverCurrent;
							tIntrInfo.tUsbInfo.isOverCurrent = 1;
							tIntrInfo.tUsbInfo.ePortNum = P_USB_GetPortNumByBus(bus, port);
							VK_INTR_Event(sIntrTaskId, (void*)&tIntrInfo);
							PrintError("[%s] Notify that overcurrent event occured. bus(%03d), port(%d)\n", __FUNCTION__, bus+1, port+1);
						}

						oc_clear[idx] = USB_OCP_NOTI_INTERVAL; /* This value means time to clear over current flag */
						PrintError("[%s] USB over current occured! bus(%03d), port(%d)\n", __FUNCTION__, bus+1, port+1);
					}
					else {
						if (oc_clear[idx] == 0) {
							continue;
						}

						oc_clear[idx]--;
						if (oc_clear[idx] == USB_OCP_CLEAR_INTERVAL) {
							/* turn port power on after 2 sec */
							err = P_USB_HubPowerOnOff(bus, port, 1, P_USB_PW_STATUS_NORMAL);
							if (err < 0) {
								PrintError("[%s] Error! can't control port power\n", __FUNCTION__);
								continue;
							}

							PrintError("[%s] turn port power on. bus(%03d), port(%d)\n", __FUNCTION__, bus+1, port+1);
						}
						else if (oc_clear[idx] == 0 && CB_USBNotify) {
							/* after 4 sec, notify oc event through callback */
							VK_MEM_Memset(&tIntrInfo, 0x00, sizeof(USB_INTR_INFO_T));
							tIntrInfo.event = DI_USB_Event_OverCurrent;
							tIntrInfo.tUsbInfo.isOverCurrent = 0;
							tIntrInfo.tUsbInfo.ePortNum = P_USB_GetPortNumByBus(bus, port);
							VK_INTR_Event(sIntrTaskId, (void*)&tIntrInfo);

							PrintError("[%s] Notify that overcurrent event cleare. bus(%03d), port(%d)\n", __FUNCTION__, bus+1, port+1);
						}
					}
				}
			}
			break;

		default:
			PrintError("[%s] Error! unknown command(%d)\n", __FUNCTION__, (unsigned int)msg.cmd);
			break;
		}
	}

	/* free */
	if (oc_clear) DD_MEM_Free(oc_clear);

	for (idx=0; idx<tHostInfo.num_host; idx++) {
		if (tHostInfo.host[idx]) {
			DD_MEM_Free(tHostInfo.host[idx]);
			tHostInfo.host[idx] = NULL;
		}
	}
	if (tHostInfo.host) {
		DD_MEM_Free(tHostInfo.host);
		tHostInfo.host = NULL;
	}

	return;
}

static void P_USB_ErrHandleTask(void *pParam)
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

		p = P_USB_FindString(buf, "SCSIERROR,", VK_strlen("SCSIERROR,"));
		if (!p) {
			continue;
		}

		p = P_USB_FindString(p, "dev=", VK_strlen("dev="));
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
		p = P_USB_FindString(p, "sector=", VK_strlen("sector="));
		if (!p) {
			continue;
		}

		sector = strtol(p, NULL, 10);
		PrintError("[%s] Pending Sector Detected! sector=%llu\n", __FUNCTION__, sector);

		if (CB_USBErrNotify) {
			CB_USBErrNotify(DI_USB_Event_PendingSector, (const char*)dev, VK_strlen(dev), (unsigned long long)sector);
			continue;
		}

		/* Do auto-recovery */
		VK_MEM_Memset(cmd, 0, sizeof(cmd));
		VK_snprintf(cmd, sizeof(cmd)-1, "dd if=/dev/zero of=/dev/%s bs=4096 count=1 seek=%llu", dev, sector*512/4096);
		VK_SYSTEM_Command(cmd);

		PrintError("[%s] Overwrite block(pending Sector). block(%llu), sector(%llu)\n", __FUNCTION__, sector*512/4096, sector);

	}

	return;
}

static void P_USB_Intr(void *arg)
{
	USB_INTR_INFO_T *ptIntrInfo = (USB_INTR_INFO_T*)arg;

	if (!ptIntrInfo) {
		PrintError("[%s] Error. arg is null!!\n", __FUNCTION__);
		return;
	}

	if (CB_USBNotify) {
		CB_USBNotify(ptIntrInfo->event, (void*)&ptIntrInfo->tUsbInfo);
	}

	if(ptIntrInfo->tUsbInfo.devName != NULL)
	{
		DD_MEM_Free(ptIntrInfo->tUsbInfo.devName[0]);
		DD_MEM_Free(ptIntrInfo->tUsbInfo.devName);
		ptIntrInfo->tUsbInfo.devName = NULL;
	}

	PrintDebug("[%s] Intr(USB) callback function called! \n", __FUNCTION__);
}

DRV_Error DRV_USB_Init(void)
{
	int 	ret;

	PrintEnter();

	/* init variables */
	VK_MEM_Memset(&tHostInfo, 0, sizeof(tHostInfo));

#if defined(CONFIG_USB_MODULE)
	/* install usb modules */
	VK_SYSTEM_Command("modprobe usbcore");
	VK_SYSTEM_Command("modprobe usb-brcm");
	VK_SYSTEM_Command("modprobe ehci-brcm");
	VK_SYSTEM_Command("modprobe ohci-brcm");
	VK_SYSTEM_Command("modprobe xhci-hcd");
	VK_SYSTEM_Command("modprobe xhci-brcm");
	VK_SYSTEM_Command("modprobe usbmon");
	VK_SYSTEM_Command("modprobe usb-storage");
	VK_SYSTEM_Command("modprobe usbhid");

	/* mount usbfs */
	VK_SYSTEM_Command("mount -t usbfs usbfs /proc/bus/usb");

	/* install usb net modules */
	VK_SYSTEM_Command("modprobe usbnet");
	VK_SYSTEM_Command("modprobe cdc_ether");
	VK_SYSTEM_Command("modprobe cdc_subset");
	VK_SYSTEM_Command("modprobe net1080");
	VK_SYSTEM_Command("modprobe asix");
	VK_SYSTEM_Command("modprobe pegasus");
	VK_SYSTEM_Command("modprobe zaurus");

	/* install usb serial modules (for z-wave / zigbee) */
	VK_SYSTEM_Command("modprobe usbserial");
	VK_SYSTEM_Command("modprobe cp210x");
	VK_SYSTEM_Command("modprobe cdc-acm");
#endif

	/* Create Hotplug Task for detection of USB */
	ret = VK_TASK_Create(P_USB_HotplugTask,
						USB_TASK_PRIORITY	,
						USB_TASK_STACK_SIZE,
						"usbmon",
						NULL,
						&sHotplugTaskId,
						VK_SUSPENDTYPE_FIFO);
	if( ret != VK_OK ) {
		PrintError("[%s] Error(0x%x) in VK_TASK_Create()\n", __FUNCTION__, ret);
		return DRV_ERR;
	}

	/* Create USB Service task for power control, stop and so on */
	ret = VK_TASK_Create(P_USB_ServiceTask,
						USB_TASK_PRIORITY	,
						USB_TASK_STACK_SIZE,
						"usb-service",
						NULL,
						&sServiceTaskId,
						VK_SUSPENDTYPE_FIFO);
	if (ret != VK_OK ) {
		PrintError("[%s] Error! can't create USB service task\n", __FUNCTION__);
		return DRV_ERR;
	}

	/* Create USB Error Handling task for Error link pending sector */
	ret = VK_TASK_Create(P_USB_ErrHandleTask,
						USB_TASK_PRIORITY,
						USB_TASK_STACK_SIZE,
						"USB_ERR_HANDLE_TASL",
						NULL,
						&sErrHandleTaskId,
						FALSE);
	if (ret != VK_OK) {
		PrintError("%s[%d] : VK_TASK_Create() error\n",__func__,__LINE__);
		return DRV_ERR;
	}

	/* Create Message Queue */
	ret = VK_MSG_Create(USB_MSG_QUEUE_SIZE, sizeof(DI_USB_CMD_T), "UsbMsg", &s_UsbMsgId, VK_SUSPENDTYPE_FIFO);
	if (ret != VK_OK) {
		PrintError("[%s] Error! can't create msg queue\n", __FUNCTION__);
		return DRV_ERR;
	}

	/* Start Hotplug Task */
	ret = VK_TASK_Start(sHotplugTaskId);
	if( ret != VK_OK ) {
		PrintError("[%s] Error(0x%x) in VK_TASK_Start()\n", __FUNCTION__, ret);
		return DRV_ERR;
	}

	/* Start Service Task */
	ret = VK_TASK_Start(sServiceTaskId);
	if( ret != VK_OK ) {
		PrintError("[%s] Error! can't start USB service task\n", __FUNCTION__);
		return DRV_ERR;
	}

	/* Start Error Handle Task */
	ret = VK_TASK_Start(sErrHandleTaskId);
	if( ret != VK_OK ) {
		PrintError("[%s] Error! can't start USB Error Handle task\n", __FUNCTION__);
		return DRV_ERR;
	}

#if defined(CONFIG_DI10)
	ret = DI_USB_PWR_Ctrl(DI_USB_PWR_ON);
	if (ret != DI_ERR_OK) {
		PrintError("[%s] Error(0x%x) USB Power on in DI_USB_PWR_Ctrl()\n", __FUNCTION__, ret);
		return DRV_ERR;
	}
#endif

	ret = VK_INTR_Register(P_USB_Intr, "usb_hotplug", sizeof(USB_INTR_INFO_T), &sIntrTaskId, VK_INTR_TASK0);
	if (ret != VK_OK) {
		PrintError("[%s] Error(0x%x) VK_INTR_Register()\n", __FUNCTION__, ret);
		return DRV_ERR;
	}

	PrintExit();

	return DRV_OK;
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
	DI_ERR_CODE eDiErr = DI_ERR_ERROR;
	HINT32 rc;
	HCHAR temp = 0;

	bRunning = FALSE;
	if(s_usbmonFd > 0)
	{
		rc = write(s_usbmonFd,&temp,1);
		DONE_ERR((rc<=0));
	}

	eDiErr = DI_ERR_OK;
done:
	return eDiErr;
}

DI_ERR_CODE DI_USB_PWR_Ctrl(DI_USB_PWR_t OnOff) /* PORT */
{
	DI_USB_CMD_T	msg;
	unsigned int	ret;

	VK_MEM_Memset(&msg, 0, sizeof(DI_USB_CMD_T));

	msg.cmd = DI_USB_CMD_POWER_PORT;
	msg.param = (unsigned int)OnOff;

	ret = VK_MSG_SendTimeout(s_UsbMsgId, &msg, sizeof(DI_USB_CMD_T), 0);
	if (ret != VK_OK) {
		PrintError("[%s] VK_MSG_Send Error!\n", __FUNCTION__);
		return DI_ERR_ERROR;
	}

	return DI_ERR_OK;
}

DI_ERR_CODE DI_USB_HOST_CONTROLLER_PWR_Ctrl(DI_USB_PWR_t OnOff) /* HOST CONTROLLER */
{
	(void)OnOff;

	return DI_ERR_OK;
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
	if(pulCount == NULL)
	{
		PrintError("[%s] pulCount is NULL!\n", __FUNCTION__);
		return DI_ERR_ERROR;
	}

	*pulCount = tHostInfo.num_host;

	return DI_ERR_OK;
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
	DRV_Error eDrvErr;

	if(pstUsbHostInfo == NULL)
	{
		PrintError("[%s] pstUsbHostInfo is NULL!\n", __FUNCTION__);
		return DI_ERR_ERROR;
	}

	/* Get USB Host info */
	eDrvErr = P_USB_GetUsbHostInfo(ulHostId, pstUsbHostInfo);
	if(eDrvErr != DRV_OK)
	{
		PrintError("[%s] P_USB_GetUsbHostInfo Error!\n", __FUNCTION__);
		return DI_ERR_ERROR;
	}

	return DI_ERR_OK;
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
	DRV_Error eDrvErr;

	if(pstUsbDevInfo == NULL)
	{
		PrintError("[%s] pstUsbDevInfo is NULL!\n", __FUNCTION__);
		return DI_ERR_ERROR;
	}

	/* Get USB device info */
	eDrvErr = P_USB_GetUsbDeviceInfo(ulHostId, ulDevId, pstUsbDevInfo);
	if(eDrvErr != DRV_OK)
	{
		PrintError("[%s] P_USB_GetUsbDeviceInfo Error!\n", __FUNCTION__);
		return DI_ERR_ERROR;
	}

	return DI_ERR_OK;
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
	DRV_Error eDrvErr;

	if(pstUsbDevConfigusInfo == NULL)
	{
		PrintError("[%s] pstUsbDevConfigusInfo is NULL!\n", __FUNCTION__);
		return DI_ERR_ERROR;
	}

	/* Get USB device Configurations info */
	eDrvErr = P_USB_GetUsbDevConfigurationsInfo(ulHostId, ulDevId, pstUsbDevConfigusInfo);
	if(eDrvErr != DRV_OK)
	{
		PrintError("[%s] P_USB_GetUsbDevConfigurationsInfo Error!\n", __FUNCTION__);
		return DI_ERR_ERROR;
	}

	return DI_ERR_OK;
}

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
	DRV_Error eDrvErr;

	if(pstUsbDevInterfacesInfo == NULL)
	{
		PrintError("[%s] pstUsbDevInterfacesInfo is NULL!\n", __FUNCTION__);
		return DI_ERR_ERROR;
	}

	/* Get USB device Configuration Interfaces info */
	eDrvErr = P_USB_GetUsbDevInterfacesInfo(ulHostId, ulDevId, pstUsbDevInterfacesInfo);
	if(eDrvErr != DRV_OK)
	{
		PrintError("[%s] P_USB_GetUsbDevInterfacesInfo Error!\n", __FUNCTION__);
		return DI_ERR_ERROR;
	}

	return DI_ERR_OK;
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
	CB_USBErrNotify = pfnNotify;
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

DI_ERR_CODE DI_USB_Init(void)
{
	DRV_Error err;

	err = DRV_USB_Init();
	if (err != DRV_OK) {
		return DI_ERR_ERROR;
	}

	return DI_ERR_OK;
}

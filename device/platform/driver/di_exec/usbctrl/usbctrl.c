
/***********************************************************
***********************************************************/
#include <stdio.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <sys/ioctl.h>
#include <sys/types.h>
#include <fcntl.h>
#include <sys/mman.h>
#include <unistd.h>
#include <string.h>
#include <dirent.h>
#include <sys/stat.h>
#include "vkernel.h"

/************************************************************
************************************************************/
#define MAX_USB_BUS_NUM	8
#define USB_SET_PORT_COMMAND			((0x01 << 5) | 0x03) 						/* USB_TYPE_CLASS	| USB_RECIP_OTHER  */
#define USB_GET_COMMAND					(0x80 | (0x01 << 5) | 0x03)					/* USB_ENDPOINT_IN | USB_TYPE_CLASS	| USB_RECIP_OTHER  */

#define USB_REQ_GET_STATUS				0x00
#define USB_REQ_CLEAR_FEATURE			0x01
#define USB_REQ_SET_FEATURE				0x03

#define USB_PORT_FEAT_CONNECTION		0
#define USB_PORT_FEAT_ENABLE			1
#define USB_PORT_FEAT_SUSPEND			2
#define USB_PORT_FEAT_OVER_CURRENT		3
#define USB_PORT_FEAT_RESET				4
#define USB_PORT_FEAT_POWER				8
#define USB_PORT_FEAT_C_CONNECTION		16
#define USB_PORT_FEAT_C_ENABLE			17
#define USB_PORT_FEAT_C_SUSPEND			18
#define USB_PORT_FEAT_C_OVER_CURRENT	19
#define USB_PORT_FEAT_C_RESET			20

#define IOCTL_USB_CONTROL	_IOWR('U', 0, struct usb_ctrltransfer)

#define USB_DEVICE_DIR    "/dev/bus/usb"
#define USB_SYS_DIR       "/sys/bus/usb/devices"

/************************************************************
************************************************************/
struct usb_ctrltransfer {
	unsigned char  bRequestType;
	unsigned char  bRequest;
	unsigned short wValue;
	unsigned short wIndex;
	unsigned short wLength;
	unsigned long timeout;
	void *data;
};

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

static HOST_INFO_T      tHostInfo;

/************************************************************
************************************************************/
int main(int argc, char *argv[])
{
	int							fd;
	int							i, j;
	struct usb_ctrltransfer 	ctl;
	int							module = 0;
	int							power = -1;
	DIR                         *dir;
	struct dirent               *ent;
	char                        szLink[256] = {0,};
	char                        szPath[256] = {0,};
	int                         nLen;

	if (argc < 2) {
		VK_printf("[Usage]\n");
		VK_printf("	usbctrl [option...]\n");
		VK_printf("	option:\n");
		VK_printf("		-module: 	if exists, app will try to insmod usb modules\n");
		VK_printf("		-poweron: 	if exists, app will try to turn all ports power on\n");
		VK_printf("		-poweroff: 	if exists, app will try to turn all ports power down\n");
		return 0;
	}

	i = 1;
	while(i < argc) {
		if (argv[i]) {
			if (strncmp(argv[i], "-module", 7)==0) module = 1;
			if (strncmp(argv[i], "-poweron", 8)==0) power = 1;
			if (strncmp(argv[i], "-poweroff", 9)==0) power = 0;
		}
		i++;
	}

	if (module) {
		/* insmod usb drivers */
		system("modprobe usbcore");
		system("modprobe usb-brcm");
		system("modprobe ehci-brcm");
		system("modprobe ohci-brcm");
		system("modprobe xhci-hcd");
		system("modprobe usbmon");
		system("modprobe usb-storage");
		system("modprobe usbhid");
		system("mount -t usbfs usbfs /proc/bus/usb");
		sleep(1);
		VK_printf("Module Loading Success!\n");
	}

	if (power == -1) {
		return 0;
	}

	power = (power) ? USB_REQ_SET_FEATURE : USB_REQ_CLEAR_FEATURE;

	/* Get USB Informations */
	memset(&tHostInfo, 0, sizeof(tHostInfo));

	dir = opendir(USB_SYS_DIR);
	if (!dir) {
		VK_printf("[%s] Can't open dir(%s)\n", __func__, USB_SYS_DIR);
		return -1;
	}
	while((ent = readdir(dir)) != NULL) {
		/* get total host count */
		if (strncmp(ent->d_name, "usb", 3) == 0) {
			tHostInfo.num_host++;
		}
	}
	closedir(dir);

	/* if no host are shown */
	if (!tHostInfo.num_host) {
		VK_printf("[%s] Error! can't get host count\n", __func__);
		return -1;
	}

	/* alloc tHostInfo.host pointer */
	if (!tHostInfo.host) {
		tHostInfo.host = (host_t**)malloc(sizeof(host_t*) * tHostInfo.num_host);
		if (!tHostInfo.host) {
			VK_printf("[%s] Error. can't alloc tHostInfo.host\n", __func__);
			return -1;
		}
		memset(tHostInfo.host, 0, sizeof(host_t*) * tHostInfo.num_host);
	}

	/* get host types(xhci, ehci and ohci) and maxchild count */
	for (i=0; i<tHostInfo.num_host; i++) {
		/* read link string to get host type */
		snprintf(szPath, sizeof(szPath), "%s/usb%d", USB_SYS_DIR, i+1);
		nLen = readlink(szPath, szLink, sizeof(szLink)-1);

		if (nLen > 0 && (unsigned long)nLen < sizeof(szLink)) {
			szLink[nLen] = 0;
			if (!tHostInfo.host[i]) {
				tHostInfo.host[i] = malloc(sizeof(host_t));
				if (!tHostInfo.host[i]) {
					VK_printf("[%s] Error. can't alloc tHostInfo.host[%d]\n", __func__, i);
					continue;
				}
			}

			if (strstr(szLink, "ehci")) {
				tHostInfo.host[i]->type = EHCI_HOST;
			}
			else if (strstr(szLink, "ohci")) {
				tHostInfo.host[i]->type = OHCI_HOST;
			}
			else if (strstr(szLink, "xhci")) {
				tHostInfo.host[i]->type = XHCI_HOST;
			}
			else {
				tHostInfo.host[i]->type = UNKNOWN_HOST;
			}
			/* this value means host bus number */
			tHostInfo.host[i]->hostnum = (int)(i+1);

			/* get maxchild count */
			tHostInfo.host[i]->maxchild = 0;
			snprintf(szPath, sizeof(szPath), "%s/usb%d/maxchild", USB_SYS_DIR, i+1);
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

	/* Enable port power on each hub */
	for(i=0; i<tHostInfo.num_host && i<MAX_USB_BUS_NUM; i++) {
		snprintf(szPath, sizeof(szPath)-1, "%s/%03d/001", USB_DEVICE_DIR, i+1);
		fd = open(szPath, O_RDWR);
		if (fd >= 0) {
			for(j=0; j<tHostInfo.host[i]->maxchild; j++) {
				ctl.bRequestType = USB_SET_PORT_COMMAND;
				ctl.bRequest = power;
				ctl.wValue = USB_PORT_FEAT_POWER;
				ctl.wIndex = j+1;
				ctl.wLength = 0;
				ctl.data = (void *)NULL;
				ctl.timeout = 5000;

				(void)ioctl(fd, IOCTL_USB_CONTROL, &ctl);
			}
			close(fd);
		}
	}
	VK_printf("Port-Power turn %s now\n", (power) ? "ON" : "OFF");

	/* free resources */
	for (i=0; i<tHostInfo.num_host; i++) {
		if (tHostInfo.host[i]) {
			free(tHostInfo.host[i]);
			tHostInfo.host[i] = NULL;
		}
	}
	if (tHostInfo.host) {
		free(tHostInfo.host);
		tHostInfo.host = NULL;
	}

	return 0;
}

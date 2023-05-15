#include <stdio.h>
#include <stdlib.h>
#include <dirent.h>
#include <sys/stat.h>

#include "vkernel.h"
#include "di_init.h"

#if defined(DEBUG)
#include "cmdtool.h"
#endif

extern int Application_Main(void);

#if defined (CONFIG_MEM_CHIP)
#define MEM_CHIP CONFIG_MEM_CHIP
#endif

#if defined(CONFIG_CRC_AUTOTEST) || defined (CONFIG_MEM_AUTOTEST)
#include <sys/stat.h>
#include <sys/ioctl.h>
#include <sys/types.h>
#include <fcntl.h>
#include <sys/mman.h>
#include <unistd.h>
#include <string.h>

struct usb_ctrltransfer {
	unsigned char  bRequestType;
	unsigned char  bRequest;
	unsigned short wValue;
	unsigned short wIndex;
	unsigned short wLength;
	unsigned long timeout;
	void *data;
};

int main(int argc, char *argv[])
{
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

#define USB_DEVICE_DIR				"/dev/bus/usb"
#define USB_SYS_DIR					"/sys/bus/usb/devices"

	int							usb_mounted;
	struct 	stat 				st;
	int							fd;
	int							i, j;
	char						devname[40];
	struct usb_ctrltransfer 	ctl;
	int							ports = 0;
	int							ohcis = 0;
	int							ehcis = 0;
	char                        szLink[256] = {0,};
	char                        szPath[256] = {0,};
	int                         nLen;
	DIR                         *dir;
	struct dirent               *ent;
	char                        cmdBuf[255] = {0,};

	(void)argc;
	(void)argv;
	
	/* insmod usb drivers */
	system("modprobe usbcore");
	system("modprobe usb-brcm");
	system("modprobe ehci-brcm");
	system("modprobe ohci-brcm");	
	system("modprobe usbmon");
	system("modprobe usb-storage");
	system("mount -t usbfs usbfs /proc/bus/usb");
	sleep(5);

	/* Get USB Informations */
	dir = opendir(USB_SYS_DIR);
	if (!dir) {
		printf("[%s] Can't open dir(%s)\n", __FUNCTION__, USB_SYS_DIR);
		return -1;			
	}

	while((ent = readdir(dir)) != NULL) {
		/* Find controller */
		if (strncmp(ent->d_name, "usb", 3) == 0) {			
			memset(szLink, 0, sizeof(szLink));
			memset(szPath, 0, sizeof(szPath));

			snprintf(szPath, sizeof(szPath), "%s/%s", USB_SYS_DIR, ent->d_name);
			nLen = readlink(szPath, szLink, sizeof(szLink)-1);

			if (nLen > 0 && (unsigned long)nLen < sizeof(szLink)) {
				if (strstr(szLink, "ehci")) {
					ehcis++;
				}
				else if (strstr(szLink, "ohci")) {
					ohcis++;
				}

				if (ports == 0) {
					memset(szPath, 0, sizeof(szPath));

					snprintf(szPath, sizeof(szPath), "%s/%s/maxchild", USB_SYS_DIR, ent->d_name);
					fd = open(szPath, O_RDONLY);
					if (fd >=0 ) {
						char data[2] = {0,};
						
						nLen = read(fd, data, sizeof(data)-1);
						if (nLen) {
							ports = (int)atoi(data);
						}
						close(fd);
					}
				}
			}			
		}
	}
	closedir(dir);

	/* Enable port power on each hub */
	for(i=0; i<(ehcis+ohcis) && i<MAX_USB_BUS_NUM; i++) {
		snprintf(devname, sizeof(devname), "%s/%03d/001", USB_DEVICE_DIR, i+1);
		fd = open(devname, O_RDWR);		
		if (fd >= 0) {
			for(j=0; j<ports; j++) {
				ctl.bRequestType = USB_SET_PORT_COMMAND;
				ctl.bRequest = USB_REQ_SET_FEATURE;
				ctl.wValue = USB_PORT_FEAT_POWER;
				ctl.wIndex = j+1;
				ctl.wLength = 0;
				ctl.data = (void *)NULL;
				ctl.timeout = 5000;

				ioctl(fd, IOCTL_USB_CONTROL, &ctl);
			}
			close(fd);
		}
	}
	sleep(5);
	
	/* mount usb storage to /media */
	if (system("mount -t vfat /dev/sda1 /media") == 0) {
		usb_mounted = 1;
	}
	else {
		printf("usb mount fail\n");
		printf("please, connect usb storage\n");
		usb_mounted = 0;
	}
	
	while(!usb_mounted) {
		/* wait for connecting */
		if (stat("/sys/block/sda/sda1", &st) == 0) {
			system("mount -t vfat /dev/sda1 /media");
			break;
		}
		sleep(1);
	}

#if defined (CONFIG_CRC_AUTOTEST)
	/* execute crc test */
	system("sh /media/crc_loop.sh");

	/* umount /media */
	system("umount /media");

	printf("CRC Test is completed\n");
#elif defined (CONFIG_MEM_AUTOTEST)
	/* execute mem test */
	VK_snprintf(cmdBuf, sizeof(cmdBuf), "sh /media/mem_loop.sh %s", MEM_CHIP);
	printf("Start MEM AutoTest\nCommand : %s\n", cmdBuf);
	system(cmdBuf);

	/* umount /media */
	system("umount /media");

	printf("MEM Test is completed\n");
#endif
	while(1){}
}

#else
int main(int argc, char *argv[])
{
	// NOT USED PARAM
	(void)argc;
	(void)argv;

	DI_Init();

	Application_Main();

	exit(EXIT_SUCCESS);
}
#endif


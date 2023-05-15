#if defined(CONFIG_VK_STDLIB)
#else
#include 	<stdio.h>
#include 	<stdlib.h>
#include 	<string.h>
#endif
#include <pthread.h>
#include <sys/socket.h>
#include <vkernel.h>
#include <dirent.h>
#include <signal.h>

#include <ctype.h>
#include <unistd.h>
#include <sys/ioctl.h>

#include <net/if_arp.h>		/* For ARPHRD_ETHER */
#include <sys/socket.h>		/* For AF_INET & struct sockaddr */
#include <netinet/in.h>         /* For struct sockaddr_in */
#include <netinet/if_ether.h>


#include <linux/sockios.h>
#include <linux/if.h>
#include <linux/wireless.h>
#include <errno.h>

#include <math.h>

#include <time.h>

#include "htype.h"
#include "drv_err.h"
#include "taskdef.h"
#include "di_wifi.h"
#include "di_uart.h"
#include "drv_network.h"
#include "drv_wireless_hotplug.h"
#include "crc.h"

#if defined(CONFIG_BCM43242)
#include "drv_nvram.h"
#include "nvram_map.h"

#define MAC_ADDR_LEN	6
#define MAC2STR(a)	(a)[0], a[1], a[2], a[3], a[4], a[5]
#define MACSTR "%02x:%02x:%02x:%02x:%02x:%02x"
#endif

#if defined(CONFIG_SUPPORT_SECDMA)
#include "nexus_base_mmap.h"
#include "nexus_memory.h"
#include "nexus_security.h"
#include "nexus_bsp_config.h"

#define BCM_WIFI_SECDMA_MEM_SIZE	0x10b0000
#endif

#if (defined(CONFIG_SHARED_COMPILE) && defined(CONFIG_SUPPORT_SECDMA))
#include "nexus_platform_client.h"
#endif

#if defined(CONFIG_QUANTENNA_BBIC_MAC)
#include "drv_gpio.h"
#endif

/*******************************************************************/
/****************************** define *****************************/
/*******************************************************************/
// #define DI_DEBUG
#if defined(CONFIG_SHARED_COMPILE)
#ifdef DI_DEBUG
#define PrintDebug(fmt, ...)	VK_DBG_Print (fmt , ## __VA_ARGS__)
#define PrintError(fmt, ...)	VK_DBG_Print (fmt , ## __VA_ARGS__)
#define PrintEnter()			VK_DBG_Print ("(+)%s\n", __FUNCTION__)
#define PrintExit()				VK_DBG_Print ("(-)%s\n", __FUNCTION__)
#else
#define PrintDebug(fmt, ...)
#define PrintError(fmt, ...)	VK_DBG_Print (fmt , ## __VA_ARGS__)
#define PrintEnter()
#define PrintExit()
#endif
#else
#ifdef DI_DEBUG
#define PrintDebug(fmt, ...)	DI_UART_Print (fmt , ## __VA_ARGS__)
#define PrintError(fmt, ...)	DI_UART_Print (fmt , ## __VA_ARGS__)
#define PrintEnter()				DI_UART_Print("(+)%s\n", __FUNCTION__)
#define PrintExit()				DI_UART_Print("(-)%s\n", __FUNCTION__)
#else
#define PrintDebug(fmt, ...)
#define PrintError(fmt, ...)	DI_UART_Print (fmt , ## __VA_ARGS__)
#define PrintEnter()
#define PrintExit()
#endif
#endif

#define UNUSED(x) (void)(x)

#define WIFI_MAC_SIZE	(6)
#define WIFI_AP_MAX		(128)
#define WIFI_MAX_NUM	(3)

#define PROCESS_INFO_CMDLINE_PATH				"/proc/%d/cmdline"
#define PROC_NET_WIRELESS						"/proc/net/wireless"

#define WPA_SUPPLICANT_FILENAME_MAX				48
#define WPA_SUPPLICANT_PID_FILE_PATTERN			"/tmp/wpa.%s.pid"
#define WPA_SUPPLICANT_CONFIG_FILE_PATTERN 		"/tmp/wpa.%s.config"
#define WPA_CLI_STATUS_FILE_PATTERN				"/tmp/wpa.%s.status"

#define WL_CONNECT_STATUS_FILE_PATTERN		"/tmp/wl.%s.connectstatus"
#define WL_KEY_STATUS_FILE_PATTERN			"/tmp/wl.%s.keystatus"
#define WL_STATUS_FILE_PATTERN				"/tmp/wl.%s.status"

#define WIFI_CONNECT_MONITOR	5000
#define WIFI_CONNECT_CHECK		3000
#define SCAN_TIMEOUT			10000000 //usec

/* Type of headers we know about (basically union iwreq_data) */
#define IW_HEADER_TYPE_NULL	0	/* Not available */
#define IW_HEADER_TYPE_CHAR	2	/* char [IFNAMSIZ] */
#define IW_HEADER_TYPE_UINT	4	/* __u32 */
#define IW_HEADER_TYPE_FREQ	5	/* struct iw_freq */
#define IW_HEADER_TYPE_ADDR	6	/* struct sockaddr */
#define IW_HEADER_TYPE_POINT	8	/* struct iw_point */
#define IW_HEADER_TYPE_PARAM	9	/* struct iw_param */
#define IW_HEADER_TYPE_QUAL	10	/* struct iw_quality */

#define IW_DESCR_FLAG_NONE	0x0000	/* Obvious */
/* Wrapper level flags */
#define IW_DESCR_FLAG_DUMP	0x0001	/* Not part of the dump command */
#define IW_DESCR_FLAG_EVENT	0x0002	/* Generate an event on SET */
#define IW_DESCR_FLAG_RESTRICT	0x0004	/* GET : request is ROOT only */
				/* SET : Omit payload from generated iwevent */
#define IW_DESCR_FLAG_NOMAX	0x0008	/* GET : no limit on request size */
/* Driver level flags */
#define IW_DESCR_FLAG_WAIT	0x0100	/* Wait for driver event */

/* Modulation bitmask */
#define SIOCSIWMODUL	0x8B2E		/* set Modulations settings */
#define SIOCGIWMODUL	0x8B2F		/* get Modulations settings */

#define	PCI_INFO_SIZE 256
#define	BCM43228_CONTROLLER "BCM4359"
#define	BCM43228_VENDOR_DEVICE_ID "14e44359" /* 0x14e4 = Broadcom PCI, 0x4359 = BCM943228*/

#define STEP_WPS_CONNECT 		0
#define STEP_WPS_SSID			1
#define STEP_WPS_AUTHENTICATION	2
#define STEP_WPS_KEY			3
#define STEP_WPS_ENCRYPTION		4

#if defined(CONFIG_SWB_B70_WIFI)
#define BCM43228_CCODE	CONFIG_SWB_B70_WIFI_CCODE
#endif

#define BCM43236_CHIP_NAME	"bcm43236"

#if defined(CONFIG_BCM43567)
#define BCM_WIFI_VEN_DEV_ID_LEN	9
#define BCM_WIFI_SUPPORTED_DEV_NUM 2

#define BCM_WIFI_CHIP_NAME			"bcm43567"
#define	BCM_WIFI_CONTROLLER 		"BCMaa31"
#endif

#define BCM_WIFI_KO_PATH			"/lib/modules/%d.%d.%d*/kernel/drivers/wireless/%s/%s.ko"
#define BCM_WIFI_FW_PATH			"/lib/firmware/brcm/%s-firmware.bin"
#define BCM_WIFI_NV_PATH			"/lib/firmware/brcm/%s.nvm"

#if defined(CONFIG_SUPPORT_SECDMA) && defined(CONFIG_WIFI_SCRIPT_TEST)
#define BCM_WIFI_TEST_KO_PATH			"/tmp/bcm_wifi_test/driver/%s.ko"
#define BCM_WIFI_TEST_PRD_FW_PATH		"/tmp/bcm_wifi_test/firmware/%s-firmware_prd.bin"
#define BCM_WIFI_TEST_MFG_FW_PATH		"/tmp/bcm_wifi_test/firmware/%s-firmware_mfg.bin"
#define BCM_WIFI_TEST_NV_PATH			"/tmp/bcm_wifi_test/nvrams/%s.nvm"
#define BCM_WIFI_TEST_DRV_TYPE_PATH		"/tmp/bcm_wifi_test_driver_type.txt"
#endif

#define BCM_WL_VERSION_FILE "/tmp/wl.version"

#define WIFI_CHANNEL_LUT_SIZE 42

#if defined(CONFIG_QV940)
//#define DI_QCSAPI_DEBUG
#define IPADDR_QV940	"169.254.119.1"
#define IPADDR_QCS_HOST	"169.254.119.5"
#define QCSAPI_TMP_FILE "/var/tmp/qv_tmp"

#define QCSAPI_START_SCAN_FILE "/var/tmp/qv_start_scan"
#define QCSAPI_SCAN_STATUS_FILE "/var/tmp/qv_scan_status"
#define QCSAPI_AP_COUNTER_FILE "/var/tmp/qv_ap_counter"
#define QCSAPI_AP_LIST_FILE "/var/tmp/qv_ap_list"
#define QCSAPI_ASSOC_STATUS_FILE "/var/tmp/qv_assoc_status"
#define QCSAPI_WPS_TMP_FILE "/var/tmp/qv_wps_tmp"
#define QCSAPI_WPS_PIN_CODE_FILE "/var/tmp/qv_wps_pincode"
#define QCSAPI_PING_LOG_FILE "/var/tmp/qv_ping_log"
#define QCSAPI_WIFI_MODE_FILE "/var/tmp/qv_wifi_mode"

#define	QV940_DEVICE_NAME "QV940"
#endif

/*******************************************************************/
/****************************** typedef ****************************/
/*******************************************************************/
typedef enum
{
	WIFI_EVT_INSERT,
	WIFI_EVT_EXTRACT,
	WIFI_EVT_SCAN_START,
	WIFI_EVT_SCAN_STOP,
	WIFI_EVT_CONNECT_START,
	WIFI_EVT_CONNECT_OK,
	WIFI_EVT_CONNECT_FAIL,
	WIFI_EVT_CONNECT_CHECK,
	WIFI_EVT_DISCONNECT,
	WIFI_EVT_WPS_PBC_START,
	WIFI_EVT_WPS_PIN_START,
	WIFI_EVT_WPS_STOP,
	WIFI_EVT_MAX
} WIFI_EVENT_e;

typedef enum
{
	WIFI_STATUS_CONNECTED,
	WIFI_STATUS_CONNECTING,  /* Only next to CONNECT or DISCONNECT */
	WIFI_STATUS_RECONNECTING,
	WIFI_STATUS_DISCONNECTED,
	WIFI_STATUS_MAX
} WIFI_STATUS_e;

typedef enum
{
	WIFI_DONGLE_INSERTED,
	WIFI_DONGLE_EXTRACTED,
	WIFI_DONGLE_MAX
} WIFI_DONGLE_e;

typedef enum
{
	WIFI_CTL_NONE,
	WIFI_CTL_WL,
	WIFI_CTL_WPA_SUPPLICANT,
#if defined(CONFIG_QV940)
	WIFI_CTL_QCSAPI,
#endif
	WIFI_CTL_MAX
} WIFI_CONTROL_TYPE_e;

typedef struct
{
	pfnWIFI_EVT_CALLBACK pfnCallback;
} WIFI_CALLBACK_INFO_t;

typedef struct
{
	HUINT32 ulIndex;
	HUINT32 ulTime;
}WIFI_ISCONNECT_INFO_t;

typedef struct
{
	HUINT32 		ulDevId;
	HINT8			acIfName[IFNAMSIZ];
	WIFI_STATUS_e	eUserSetting;
	DI_WIFI_WPS_e	eWPS;
	HINT32 			nWPSPid;
	WIFI_STATUS_e	eStatus;
	HBOOL			bScanning;
	WIFI_DONGLE_e	eDongleStatus;
	HUINT32			ulNumOfAPs;
	HUINT8 			aucMAC[6];		/*HEX*/
	DI_WIFI_INFO_t 	stUserSetAP;
	DI_WIFI_INFO_t 	astAPList[WIFI_AP_MAX];
	WIFI_CALLBACK_INFO_t 	stCallbackInfo[DI_WIFI_NOTIFY_EVT_MAX];
}WIFI_INFO_t;

typedef struct
{
	HUINT32			ulMsg;
	HUINT32			ulTask;
	HUINT32			ulSema;
	HUINT32			ulDisConnectTimerId;
	WIFI_INFO_t		stWiFiInfo[WIFI_MAX_NUM];
} WIFI_INSTANCE_t;

typedef struct
{
	HUINT32			ulIndex;
	WIFI_EVENT_e 	eMsg;
	HUINT32			ulTime;
	HINT8 			acDevName[IW_ESSID_MAX_SIZE+1]; /*IW_ESSID_MAX_SIZE 32*/
	HUINT8			aucPin[8];
} WIFI_MSG_t;

/* Structure used for parsing event streams, such as Wireless Events
 * and scan results */
typedef struct stream_descr
{
	unsigned char *	end;		/* End of the stream */
  	unsigned char *	current;	/* Current event in stream of events */
	unsigned char *	value;		/* Current value in event */
} stream_descr;

struct iw_ioctl_description
{
	unsigned char	header_type;		/* NULL, iw_point or other */
	unsigned char	token_type;		/* Future */
	unsigned short	token_size;		/* Granularity of payload */
	unsigned short	min_tokens;		/* Min acceptable token number */
	unsigned short	max_tokens;		/* Max acceptable token number */
	unsigned int	flags;			/* Special handling of the request */
};

typedef struct
{
	HUINT32	ulChannelNo;
	HUINT32	ulFrequency;
} WIFI_CHANNEL_LUT_t;

#if defined(CONFIG_BCM43567)
typedef struct
{
	HCHAR	acDevId[BCM_WIFI_VEN_DEV_ID_LEN];
} BCM_WIFI_DEVID_t;
#endif

/*******************************************************************/
/************************ global variables *************************/
/*******************************************************************/


/*******************************************************************/
/*********************** extern function prototypes ****************/
/*******************************************************************/
extern DRV_NETWORK_DEV_INFO_t s_stDevList[];
extern HUINT32 DRV_CFG_GetNetMaxDevice(void);

/*******************************************************************/
/*********************** Static function prototypes ****************/
/*******************************************************************/
static WIFI_INSTANCE_t s_stWiFiInstance;
static HINT32	s_nSocketFd = -1;
#if defined(CONFIG_QV940)
static WIFI_CONTROL_TYPE_e s_eWifiCtlInterface = WIFI_CTL_QCSAPI;
#else
static WIFI_CONTROL_TYPE_e s_eWifiCtlInterface = WIFI_CTL_NONE;
#endif

/*
 * Meta-data about all the standard Wireless Extension request we
 * know about.
 */
static const struct iw_ioctl_description standard_ioctl_descr[] = {
	[SIOCSIWCOMMIT	- SIOCIWFIRST] = {
		.header_type	= IW_HEADER_TYPE_NULL,
	},
	[SIOCGIWNAME	- SIOCIWFIRST] = {
		.header_type	= IW_HEADER_TYPE_CHAR,
		.flags		= IW_DESCR_FLAG_DUMP,
	},
	[SIOCSIWNWID	- SIOCIWFIRST] = {
		.header_type	= IW_HEADER_TYPE_PARAM,
		.flags		= IW_DESCR_FLAG_EVENT,
	},
	[SIOCGIWNWID	- SIOCIWFIRST] = {
		.header_type	= IW_HEADER_TYPE_PARAM,
		.flags		= IW_DESCR_FLAG_DUMP,
	},
	[SIOCSIWFREQ	- SIOCIWFIRST] = {
		.header_type	= IW_HEADER_TYPE_FREQ,
		.flags		= IW_DESCR_FLAG_EVENT,
	},
	[SIOCGIWFREQ	- SIOCIWFIRST] = {
		.header_type	= IW_HEADER_TYPE_FREQ,
		.flags		= IW_DESCR_FLAG_DUMP,
	},
	[SIOCSIWMODE	- SIOCIWFIRST] = {
		.header_type	= IW_HEADER_TYPE_UINT,
		.flags		= IW_DESCR_FLAG_EVENT,
	},
	[SIOCGIWMODE	- SIOCIWFIRST] = {
		.header_type	= IW_HEADER_TYPE_UINT,
		.flags		= IW_DESCR_FLAG_DUMP,
	},
	[SIOCSIWSENS	- SIOCIWFIRST] = {
		.header_type	= IW_HEADER_TYPE_PARAM,
	},
	[SIOCGIWSENS	- SIOCIWFIRST] = {
		.header_type	= IW_HEADER_TYPE_PARAM,
	},
	[SIOCSIWRANGE	- SIOCIWFIRST] = {
		.header_type	= IW_HEADER_TYPE_NULL,
	},
	[SIOCGIWRANGE	- SIOCIWFIRST] = {
		.header_type	= IW_HEADER_TYPE_POINT,
		.token_size	= 1,
		.max_tokens	= sizeof(struct iw_range),
		.flags		= IW_DESCR_FLAG_DUMP,
	},
	[SIOCSIWPRIV	- SIOCIWFIRST] = {
		.header_type	= IW_HEADER_TYPE_NULL,
	},
	[SIOCGIWPRIV	- SIOCIWFIRST] = { /* (handled directly by us) */
		.header_type	= IW_HEADER_TYPE_NULL,
	},
	[SIOCSIWSTATS	- SIOCIWFIRST] = {
		.header_type	= IW_HEADER_TYPE_NULL,
	},
	[SIOCGIWSTATS	- SIOCIWFIRST] = { /* (handled directly by us) */
		.header_type	= IW_HEADER_TYPE_NULL,
		.flags		= IW_DESCR_FLAG_DUMP,
	},
	[SIOCSIWSPY	- SIOCIWFIRST] = {
		.header_type	= IW_HEADER_TYPE_POINT,
		.token_size	= sizeof(struct sockaddr),
		.max_tokens	= IW_MAX_SPY,
	},
	[SIOCGIWSPY	- SIOCIWFIRST] = {
		.header_type	= IW_HEADER_TYPE_POINT,
		.token_size	= sizeof(struct sockaddr) +
				  sizeof(struct iw_quality),
		.max_tokens	= IW_MAX_SPY,
	},
	[SIOCSIWTHRSPY	- SIOCIWFIRST] = {
		.header_type	= IW_HEADER_TYPE_POINT,
		.token_size	= sizeof(struct iw_thrspy),
		.min_tokens	= 1,
		.max_tokens	= 1,
	},
	[SIOCGIWTHRSPY	- SIOCIWFIRST] = {
		.header_type	= IW_HEADER_TYPE_POINT,
		.token_size	= sizeof(struct iw_thrspy),
		.min_tokens	= 1,
		.max_tokens	= 1,
	},
	[SIOCSIWAP	- SIOCIWFIRST] = {
		.header_type	= IW_HEADER_TYPE_ADDR,
	},
	[SIOCGIWAP	- SIOCIWFIRST] = {
		.header_type	= IW_HEADER_TYPE_ADDR,
		.flags		= IW_DESCR_FLAG_DUMP,
	},
	[SIOCSIWMLME	- SIOCIWFIRST] = {
		.header_type	= IW_HEADER_TYPE_POINT,
		.token_size	= 1,
		.min_tokens	= sizeof(struct iw_mlme),
		.max_tokens	= sizeof(struct iw_mlme),
	},
	[SIOCGIWAPLIST	- SIOCIWFIRST] = {
		.header_type	= IW_HEADER_TYPE_POINT,
		.token_size	= sizeof(struct sockaddr) +
				  sizeof(struct iw_quality),
		.max_tokens	= IW_MAX_AP,
		.flags		= IW_DESCR_FLAG_NOMAX,
	},
	[SIOCSIWSCAN	- SIOCIWFIRST] = {
		.header_type	= IW_HEADER_TYPE_POINT,
		.token_size	= 1,
		.min_tokens	= 0,
		.max_tokens	= sizeof(struct iw_scan_req),
	},
	[SIOCGIWSCAN	- SIOCIWFIRST] = {
		.header_type	= IW_HEADER_TYPE_POINT,
		.token_size	= 1,
		.max_tokens	= IW_SCAN_MAX_DATA,
		.flags		= IW_DESCR_FLAG_NOMAX,
	},
	[SIOCSIWESSID	- SIOCIWFIRST] = {
		.header_type	= IW_HEADER_TYPE_POINT,
		.token_size	= 1,
		.max_tokens	= IW_ESSID_MAX_SIZE + 1,
		.flags		= IW_DESCR_FLAG_EVENT,
	},
	[SIOCGIWESSID	- SIOCIWFIRST] = {
		.header_type	= IW_HEADER_TYPE_POINT,
		.token_size	= 1,
		.max_tokens	= IW_ESSID_MAX_SIZE + 1,
		.flags		= IW_DESCR_FLAG_DUMP,
	},
	[SIOCSIWNICKN	- SIOCIWFIRST] = {
		.header_type	= IW_HEADER_TYPE_POINT,
		.token_size	= 1,
		.max_tokens	= IW_ESSID_MAX_SIZE + 1,
	},
	[SIOCGIWNICKN	- SIOCIWFIRST] = {
		.header_type	= IW_HEADER_TYPE_POINT,
		.token_size	= 1,
		.max_tokens	= IW_ESSID_MAX_SIZE + 1,
	},
	[SIOCSIWRATE	- SIOCIWFIRST] = {
		.header_type	= IW_HEADER_TYPE_PARAM,
	},
	[SIOCGIWRATE	- SIOCIWFIRST] = {
		.header_type	= IW_HEADER_TYPE_PARAM,
	},
	[SIOCSIWRTS	- SIOCIWFIRST] = {
		.header_type	= IW_HEADER_TYPE_PARAM,
	},
	[SIOCGIWRTS	- SIOCIWFIRST] = {
		.header_type	= IW_HEADER_TYPE_PARAM,
	},
	[SIOCSIWFRAG	- SIOCIWFIRST] = {
		.header_type	= IW_HEADER_TYPE_PARAM,
	},
	[SIOCGIWFRAG	- SIOCIWFIRST] = {
		.header_type	= IW_HEADER_TYPE_PARAM,
	},
	[SIOCSIWTXPOW	- SIOCIWFIRST] = {
		.header_type	= IW_HEADER_TYPE_PARAM,
	},
	[SIOCGIWTXPOW	- SIOCIWFIRST] = {
		.header_type	= IW_HEADER_TYPE_PARAM,
	},
	[SIOCSIWRETRY	- SIOCIWFIRST] = {
		.header_type	= IW_HEADER_TYPE_PARAM,
	},
	[SIOCGIWRETRY	- SIOCIWFIRST] = {
		.header_type	= IW_HEADER_TYPE_PARAM,
	},
	[SIOCSIWENCODE	- SIOCIWFIRST] = {
		.header_type	= IW_HEADER_TYPE_POINT,
		.token_size	= 1,
		.max_tokens	= IW_ENCODING_TOKEN_MAX,
		.flags		= IW_DESCR_FLAG_EVENT | IW_DESCR_FLAG_RESTRICT,
	},
	[SIOCGIWENCODE	- SIOCIWFIRST] = {
		.header_type	= IW_HEADER_TYPE_POINT,
		.token_size	= 1,
		.max_tokens	= IW_ENCODING_TOKEN_MAX,
		.flags		= IW_DESCR_FLAG_DUMP | IW_DESCR_FLAG_RESTRICT,
	},
	[SIOCSIWPOWER	- SIOCIWFIRST] = {
		.header_type	= IW_HEADER_TYPE_PARAM,
	},
	[SIOCGIWPOWER	- SIOCIWFIRST] = {
		.header_type	= IW_HEADER_TYPE_PARAM,
	},
	[SIOCSIWMODUL	- SIOCIWFIRST] = {
		.header_type	= IW_HEADER_TYPE_PARAM,
	},
	[SIOCGIWMODUL	- SIOCIWFIRST] = {
		.header_type	= IW_HEADER_TYPE_PARAM,
	},
	[SIOCSIWGENIE	- SIOCIWFIRST] = {
		.header_type	= IW_HEADER_TYPE_POINT,
		.token_size	= 1,
		.max_tokens	= IW_GENERIC_IE_MAX,
	},
	[SIOCGIWGENIE	- SIOCIWFIRST] = {
		.header_type	= IW_HEADER_TYPE_POINT,
		.token_size	= 1,
		.max_tokens	= IW_GENERIC_IE_MAX,
	},
	[SIOCSIWAUTH	- SIOCIWFIRST] = {
		.header_type	= IW_HEADER_TYPE_PARAM,
	},
	[SIOCGIWAUTH	- SIOCIWFIRST] = {
		.header_type	= IW_HEADER_TYPE_PARAM,
	},
	[SIOCSIWENCODEEXT - SIOCIWFIRST] = {
		.header_type	= IW_HEADER_TYPE_POINT,
		.token_size	= 1,
		.min_tokens	= sizeof(struct iw_encode_ext),
		.max_tokens	= sizeof(struct iw_encode_ext) +
				  IW_ENCODING_TOKEN_MAX,
	},
	[SIOCGIWENCODEEXT - SIOCIWFIRST] = {
		.header_type	= IW_HEADER_TYPE_POINT,
		.token_size	= 1,
		.min_tokens	= sizeof(struct iw_encode_ext),
		.max_tokens	= sizeof(struct iw_encode_ext) +
				  IW_ENCODING_TOKEN_MAX,
	},
	[SIOCSIWPMKSA - SIOCIWFIRST] = {
		.header_type	= IW_HEADER_TYPE_POINT,
		.token_size	= 1,
		.min_tokens	= sizeof(struct iw_pmksa),
		.max_tokens	= sizeof(struct iw_pmksa),
	},
};
static const unsigned int standard_ioctl_num = (sizeof(standard_ioctl_descr) /
						sizeof(struct iw_ioctl_description));

/*
 * Meta-data about all the additional standard Wireless Extension events
 * we know about.
 */
static const struct iw_ioctl_description standard_event_descr[] = {
	[IWEVTXDROP	- IWEVFIRST] = {
		.header_type	= IW_HEADER_TYPE_ADDR,
	},
	[IWEVQUAL	- IWEVFIRST] = {
		.header_type	= IW_HEADER_TYPE_QUAL,
	},
	[IWEVCUSTOM	- IWEVFIRST] = {
		.header_type	= IW_HEADER_TYPE_POINT,
		.token_size	= 1,
		.max_tokens	= IW_CUSTOM_MAX,
	},
	[IWEVREGISTERED	- IWEVFIRST] = {
		.header_type	= IW_HEADER_TYPE_ADDR,
	},
	[IWEVEXPIRED	- IWEVFIRST] = {
		.header_type	= IW_HEADER_TYPE_ADDR,
	},
	[IWEVGENIE	- IWEVFIRST] = {
		.header_type	= IW_HEADER_TYPE_POINT,
		.token_size	= 1,
		.max_tokens	= IW_GENERIC_IE_MAX,
	},
	[IWEVMICHAELMICFAILURE	- IWEVFIRST] = {
		.header_type	= IW_HEADER_TYPE_POINT,
		.token_size	= 1,
		.max_tokens	= sizeof(struct iw_michaelmicfailure),
	},
	[IWEVASSOCREQIE	- IWEVFIRST] = {
		.header_type	= IW_HEADER_TYPE_POINT,
		.token_size	= 1,
		.max_tokens	= IW_GENERIC_IE_MAX,
	},
	[IWEVASSOCRESPIE	- IWEVFIRST] = {
		.header_type	= IW_HEADER_TYPE_POINT,
		.token_size	= 1,
		.max_tokens	= IW_GENERIC_IE_MAX,
	},
	[IWEVPMKIDCAND	- IWEVFIRST] = {
		.header_type	= IW_HEADER_TYPE_POINT,
		.token_size	= 1,
		.max_tokens	= sizeof(struct iw_pmkid_cand),
	},
};
static const unsigned int standard_event_num = (sizeof(standard_event_descr) /
						sizeof(struct iw_ioctl_description));

/* Size (in bytes) of various events */
static const int event_type_size[] = {
	IW_EV_LCP_PK_LEN,	/* IW_HEADER_TYPE_NULL */
	0,
	IW_EV_CHAR_PK_LEN,	/* IW_HEADER_TYPE_CHAR */
	0,
	IW_EV_UINT_PK_LEN,	/* IW_HEADER_TYPE_UINT */
	IW_EV_FREQ_PK_LEN,	/* IW_HEADER_TYPE_FREQ */
	IW_EV_ADDR_PK_LEN,	/* IW_HEADER_TYPE_ADDR */
	0,
	IW_EV_POINT_PK_LEN,	/* Without variable payload */
	IW_EV_PARAM_PK_LEN,	/* IW_HEADER_TYPE_PARAM */
	IW_EV_QUAL_PK_LEN,	/* IW_HEADER_TYPE_QUAL */
};

#if 1 /* __HUMAX_CODE__ - 140417 mskim (redmine #) */
static char wifiDevName[256] = { 0 };
#endif

const WIFI_CHANNEL_LUT_t g_stWifiChLut[WIFI_CHANNEL_LUT_SIZE] = {
	/* 2.4 GHz - 14 channels */
	{1, 2412}, {2, 2417}, {3, 2422}, {4, 2427}, {5, 2432},
	{6, 2437}, {7, 2442}, {8, 2447}, {9, 2452}, {10, 2457},
	{11, 2462}, {12, 2467}, {13, 2472}, {14, 2484},
	/* 5 GHz - 28 channels */
	{34, 5170}, {36, 5180}, {38, 5190}, {40, 5200}, {42, 5210},
	{44, 5220}, {46, 5230}, {48, 5240}, {52, 5260}, {56, 5280},
	{60, 5300}, {64, 5320}, {100, 5500}, {104, 5520}, {108, 5540},
	{112, 5560}, {116, 5580}, {120, 5600}, {124, 5620}, {128, 5640},
	{132, 5660}, {136, 5680}, {140, 5700}, {149, 5745}, {153, 5765},
	{157, 5785}, {161, 5805}, {165, 5825}
};


#if defined(CONFIG_SUPPORT_SECDMA)
/* PCIe max window size signed command. Provided by CA vendor. */
static HUINT8 s_aucSignedCommand[NEXUS_PCIE_MAX_WINDOW_RAW_COMMAND_SIZE] = {
	0x00, 0x01, 0x00, 0x01,
	0x00, 0x01, 0x00, 0x01,
	0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00,
	0x00, 0x11, 0x22, 0x33,
	0x44, 0x55, 0x66, 0x77,
	0x88, 0x99, 0xaa, 0xbb,
	0xcc, 0xdd, 0xee, 0xff,
	0x00, 0x00, 0x00, 0x03,
	0x04, 0x00, 0x00, 0x00,
	0x04, 0x00, 0x00, 0x00,
	0x04, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00,
	0xff, 0xff, 0xff, 0xff,
	0x00, 0x00, 0xff, 0x00,
	0x01, 0x06, 0x00, 0x00,
	0x1a, 0x25, 0xf7, 0xd3,
	0xed, 0xea, 0xff, 0x56,
	0xed, 0x78, 0x4e, 0x07,
	0xe2, 0x8c, 0xbd, 0x3b,
	0x28, 0x2f, 0xb1, 0xca,
	0x58, 0xf4, 0xb1, 0xb5,
	0x0f, 0x83, 0xf4, 0xf4,
	0x7c, 0x1e, 0xb2, 0xba
};
#endif

#if defined(CONFIG_BCM43567)
static BCM_WIFI_DEVID_t s_stDevIdList[BCM_WIFI_SUPPORTED_DEV_NUM] = {
	{"14e4aa31"},
	{"14e443d9"}
};
#endif

/*------------------------------------------------------------------*/
/*
	Function List.
 */
/*------------------------------------------------------------------*/
static void P_WIFI_NotifyDImsg(DRV_Wireless_Event_t etWirelessEventType,void *pucWirelessInfo)
{
	wireless_info_t		*pInfoFromDI = NULL;
	HINT32		nVkErr = VK_OK;
	WIFI_MSG_t	stWiFiMsg;
#if defined(CONFIG_SHARED_COMPILE) && defined(CONFIG_SUPPORT_SECDMA)
	NEXUS_Error rc;
#endif

	pInfoFromDI = (wireless_info_t *)pucWirelessInfo;

	VK_MEM_Memset(stWiFiMsg.acDevName,0x00,sizeof(stWiFiMsg.acDevName));

	switch(etWirelessEventType)
	{
		case DRV_WIRELESS_Insert:
#if defined(CONFIG_SHARED_COMPILE) && defined(CONFIG_SUPPORT_SECDMA)
			if ((rc = NEXUS_Platform_Join()) != NEXUS_SUCCESS) {
				PrintError("[%s] NEXUS_Platfrom_Join() failed, err %d",__FUNCTION__, rc);
				break;
			}
#endif

			stWiFiMsg.eMsg = WIFI_EVT_INSERT;
			VK_strncpy((char *)stWiFiMsg.acDevName, (char *)pInfoFromDI->devName, sizeof(stWiFiMsg.acDevName) -1);
			nVkErr = VK_MSG_Send(s_stWiFiInstance.ulMsg, &stWiFiMsg, sizeof(WIFI_MSG_t));
			if(nVkErr != VK_OK)
			{
				PrintError("[%s] VK_MSG_Send Err(0x%x)!!\n",__FUNCTION__,nVkErr);
			}

			break;

		case DRV_WIRELESS_Extract:

			stWiFiMsg.eMsg = WIFI_EVT_EXTRACT;
			VK_strncpy((char *)stWiFiMsg.acDevName, (char *)pInfoFromDI->devName, sizeof(stWiFiMsg.acDevName) -1);

			nVkErr = VK_MSG_Send(s_stWiFiInstance.ulMsg, &stWiFiMsg, sizeof(WIFI_MSG_t));
			if(nVkErr != VK_OK)
			{
				PrintError("[%s] VK_MSG_Send Err(0x%x)!!\n",__FUNCTION__,nVkErr);
			}

			break;

		default:

			break;
	}
}


#if defined(CONFIG_SWB_B70_WIFI) || defined(CONFIG_BCM43567)
static DRV_Error P_CheckPcieWifiDevice(HBOOL *bConnected)
{
	DRV_Error dResult = DRV_OK;
	int ret = 0;
	FILE *fp;
	HCHAR *pIdBuf = NULL;
	long int ulFileSize=0;
	HBOOL bPciDetected = FALSE;
#if defined(CONFIG_BCM43567)
	HUINT8	ucDevIdx = 0;
#endif

	/* Open PCI device info */
	VK_SYSTEM_Command("cat /proc/bus/pci/devices > /tmp/pci_devices.txt");
	fp = fopen("/tmp/pci_devices.txt","r");
	if(fp == NULL)
	{
		PrintError("[%s] Err fopen %s\n",__FUNCTION__, strerror( errno ));
		dResult = DRV_ERR;
		goto go_out;
	}

	/* Get file size */
	ret = fseek(fp, 0, SEEK_END);
	if(ret != 0)
	{
		PrintError("[%s] Err fseek %s\n",__FUNCTION__, strerror( errno ));
		dResult = DRV_ERR;
		goto go_out;
	}

	ulFileSize = ftell(fp);
	if(ulFileSize <= 0)
	{
		PrintError("ulFileSize = %d \n",ulFileSize);
		dResult = DRV_ERR;
		goto go_out;
	}

	ret = fseek(fp, 0, SEEK_SET);
	if(ret != 0)
	{
		PrintError("[%s] Err fseek %s\n",__FUNCTION__, strerror( errno ));
		dResult = DRV_ERR;
		goto go_out;
	}

	/* Get vendor & device ID */
	pIdBuf = DD_MEM_Alloc(PCI_INFO_SIZE);
	if (NULL == pIdBuf)
	{
		PrintError("[%s] Err Memory Allocation %s\n",__FUNCTION__, strerror( errno ));
		dResult = DRV_ERR;
		goto go_out;
	}

	while(fgets(pIdBuf, PCI_INFO_SIZE, fp))
	{
#if defined(CONFIG_SWB_B70_WIFI)
		if(VK_strstr(pIdBuf, BCM43228_VENDOR_DEVICE_ID))
		{
			bPciDetected = TRUE;
			break;
		}

		VK_MEM_Memset(pIdBuf, 0x0, PCI_INFO_SIZE);
#elif defined(CONFIG_BCM43567)
		if (bPciDetected == FALSE)
		{
			for (ucDevIdx = 0; ucDevIdx < BCM_WIFI_SUPPORTED_DEV_NUM; ucDevIdx++)
			{
				s_stDevIdList[ucDevIdx].acDevId[BCM_WIFI_VEN_DEV_ID_LEN-1] = '\0';
				if(VK_strstr(pIdBuf, s_stDevIdList[ucDevIdx].acDevId))
				{
					bPciDetected = TRUE;
					break;
				}
			}

			if (bPciDetected == TRUE)
			{
				break;
			}
		}

		VK_MEM_Memset(pIdBuf, 0x0, PCI_INFO_SIZE);
#endif
	}

	if(bPciDetected == FALSE)
	{
		PrintError("[%s] PCI Wireless Not found\n",__FUNCTION__);
	}


go_out :
	if(pIdBuf != NULL)
	{
		DD_MEM_Free(pIdBuf);
	}

	if(fp != NULL)
	{
		fclose(fp);
	}

	VK_SYSTEM_Command("rm -f /tmp/pci_devices.txt");

	*bConnected = bPciDetected;

	return dResult;

}

static void P_CheckPcieWifiConnection(void)
{
	DRV_Error dResult = DRV_ERR;
	HBOOL bPcieWifi;
	wireless_info_t wireless;

	dResult = P_CheckPcieWifiDevice(&bPcieWifi);
	if(dResult != DRV_OK)
	{
		PrintError("[%s] Error : P_CheckPcieWifiDevice \n", __FUNCTION__);
		return;
	}
	else
	{
		if(bPcieWifi == TRUE)
		{
			VK_MEM_Memset(wireless.devName,0x00,sizeof(wireless.devName));
#if defined(CONFIG_SWB_B70_WIFI)
			VK_snprintf(wireless.devName, sizeof(wireless.devName),BCM43228_CONTROLLER);
#elif defined (CONFIG_BCM43567)
			VK_snprintf(wireless.devName, sizeof(wireless.devName),BCM_WIFI_CONTROLLER);
#endif
			P_WIFI_NotifyDImsg(DRV_WIRELESS_Insert, &wireless);
		}
	}
}
#endif

#if defined(CONFIG_QV940)
static unsigned int i_qcsapiAPIndex = 0;

/* TODO : check connection ? */
static DRV_Error P_CheckRgmiiWifiDevice(HBOOL *bConnected)
{
	DRV_Error dResult = DRV_OK;
	HBOOL bRgmiiDetected = TRUE /* FALSE */;

	FILE *fp;
	int bin_fp = -1;
	HCHAR cmdBuf[255], buf[128], qcsapiMode[129], *pTemp = 0;

	PrintEnter();

retry_transmit_file:
    VK_snprintf(cmdBuf, sizeof(cmdBuf), "ping -c 1 %s > %s", IPADDR_QV940, QCSAPI_PING_LOG_FILE);
	system(cmdBuf);

	VK_TASK_Sleep(2000);

	fp = fopen(QCSAPI_PING_LOG_FILE, "r");
	if(fp == NULL)
	{
		goto retry_transmit_file;
	}

	while(fgets(buf, 128, fp)!= NULL)
	{
		pTemp = NULL;
		pTemp = VK_strstr(buf,"round-trip");

		if (pTemp)
		{
			PrintError("%s(%d) Network connected!!!\n", __FUNCTION__, __LINE__);
			break;
		}
	}
	fclose(fp);

	if (!pTemp)
	{
		goto retry_transmit_file;
	}
	else
	{
		VK_snprintf(cmdBuf, sizeof(cmdBuf), "call_qcsapi_sockrpc --host %s get_mode wifi0 > %s",
			IPADDR_QV940, QCSAPI_WIFI_MODE_FILE);
		VK_SYSTEM_Command(cmdBuf);

		fp = fopen(QCSAPI_WIFI_MODE_FILE, "r");
		if (fp != NULL)
		{
			fgets(buf, 128, fp);
			VK_sscanf(buf, "%s", &qcsapiMode[0]);
			fclose(fp);

			if (VK_strcmp("Station", qcsapiMode) != 0)
			{
				PrintError("[%s] Need to check the RPC: Program not registered!\n", __FUNCTION__);
				goto retry_transmit_file;
			}
		}
	}

	*bConnected = bRgmiiDetected;
	PrintExit();

	return dResult;

}

static void P_CheckRgmiiWifiConnection(void)
{
	DRV_Error dResult = DRV_ERR;
	HBOOL bRgmiiWifi;
	wireless_info_t wireless;

	dResult = P_CheckRgmiiWifiDevice(&bRgmiiWifi);
	if(dResult != DRV_OK)
	{
		PrintError("[%s] Error : P_CheckRgmiiWifiDevice \n", __FUNCTION__);
		return;
	}
	else
	{
		if(bRgmiiWifi == TRUE)
		{
			VK_MEM_Memset(wireless.devName,0x00,sizeof(wireless.devName));
#if defined(CONFIG_QV940)
			snprintf(wireless.devName, sizeof(wireless.devName),QV940_DEVICE_NAME);
#endif
			P_WIFI_NotifyDImsg(DRV_WIRELESS_Insert, &wireless);
		}
	}
}

static DRV_Error P_WIFI_RunQcsapiCmdForRate(HUINT32 ulIndex, HINT32 *pValue)
{
    FILE *fp;
    char cmdBuf[255], buf[129];
    HFLOAT32 fTmpRate = 0;

	UNUSED(ulIndex);
	PrintEnter();

    VK_snprintf(cmdBuf, sizeof(cmdBuf), "call_qcsapi_sockrpc --host %s get_rx_phy_rate wifi0 > %s",
        IPADDR_QV940, QCSAPI_TMP_FILE);
	VK_SYSTEM_Command(cmdBuf);

	fp = fopen(QCSAPI_TMP_FILE, "r");
	if (fp != NULL)
	{
		fgets(buf, 128, fp);
#if 1 // test
		VK_sscanf(buf,"%f",&fTmpRate);
		*pValue = (HINT32)fTmpRate*1000000;
#else
		VK_sscanf(buf, "%d", pValue);
#endif
		fclose(fp);
	}
	else
    {
		PrintError("[%s] Err fopen %s\n", __FUNCTION__, strerror( errno ));
		return DRV_ERR;
    }

	PrintExit();

	return DRV_OK;
}

static DRV_Error P_WIFI_RunQcsapiCmdForRssi(HUINT32 ulIndex, HINT32 *pValue)
{
    FILE *fp;
    char cmdBuf[255], buf[129];
	DRV_Error dResult = DRV_OK;
    HFLOAT32 fTmpRssi = 0;
    int assocStatus = 0;

	UNUSED(ulIndex);
	PrintEnter();

	VK_snprintf(cmdBuf, sizeof(cmdBuf), "call_qcsapi_sockrpc --host %s get_count_assoc  wifi0 > %s",
		IPADDR_QV940, QCSAPI_ASSOC_STATUS_FILE);
	VK_SYSTEM_Command(cmdBuf);

	fp = fopen(QCSAPI_ASSOC_STATUS_FILE, "r");
	if (fp != NULL)
	{
		fgets(buf, 128, fp);
		VK_sscanf(buf, "%d", &assocStatus);
		fclose(fp);
	}
	else
	{
		PrintError("[%s] Err fopen %s\n", __FUNCTION__, strerror( errno ));
		return DRV_ERR;
	}

	if (assocStatus == 0)
	{
		PrintError("[%s] WiFi association first!!!\n", __FUNCTION__);
		return DRV_ERR;
	}

    VK_snprintf(cmdBuf, sizeof(cmdBuf), "call_qcsapi_sockrpc --host %s get_rssi_dbm wifi0 0 > %s",
        IPADDR_QV940, QCSAPI_TMP_FILE);
	VK_SYSTEM_Command(cmdBuf);

	fp = fopen(QCSAPI_TMP_FILE, "r");
	if (fp != NULL)
	{
		fgets(buf, 128, fp);

		VK_sscanf(buf,"%f",&fTmpRssi);
		*pValue = (HINT32)100-fTmpRssi;

		fclose(fp);
	}
	else
    {
		PrintError("[%s] Err fopen %s\n", __FUNCTION__, strerror( errno ));
		return DRV_ERR;
    }

	PrintExit();

	return dResult;
}

#if defined(CONFIG_SUPPORT_WOWL)
static DI_ERR_CODE P_WIFI_CheckQcsapiCmdReturnValue(void)
{
	FILE *fp;
	char buf[255], qcsapiStatus[129];

	fp = fopen(QCSAPI_TMP_FILE, "r");
	if (fp != NULL)
	{
		fgets(buf, 128, fp);
		VK_sscanf(buf, "%s", &qcsapiStatus[0]);
		fclose(fp);

		if (VK_strcmp("complete", qcsapiStatus) != 0)
		{
			PrintError("[%s] QCSAPI return error!\n", __FUNCTION__);
			return DI_ERR_ERROR;
		}
	}
	else
	{
		PrintError("[%s] Err fopen %s\n", __FUNCTION__, strerror( errno ));
		return DI_ERR_ERROR;
	}

    return DI_ERR_OK;
}
#endif /* CONFIG_SUPPORT_WOWL */
#endif /* CONFIG_QV940 */

/* WPA_Supplicant only functions : start*/
static char * P_WIFI_convertToProtoStr( DI_WIFI_SECURE_e  secure )
{
	char *retStr = NULL;
	switch ( (unsigned int)secure ) {
		case DI_WIFI_SECURE_WPA :
			retStr = "WPA";
			break;
		case DI_WIFI_SECURE_WPA2 :
			retStr = "RSN";
			break;
		case DI_WIFI_SECURE_WPA_WPA2:
			retStr = "WPA RSN";
			break;
	}
	return retStr;
}

static char * P_WIFI_convertToKeyMGMTStr( DI_WIFI_AUTHEN_e  authen )
{
	char *retStr = NULL;
	switch ( (unsigned int)authen ) {
		case DI_WIFI_AUTHEN_OPEN :
			retStr = "NONE";
			break;
		case DI_WIFI_AUTHEN_PSK :
			retStr = "WPA-PSK";
			break;
		case DI_WIFI_AUTHEN_802_1X :
			retStr = "IEEE8021X";
			break;
		case DI_WIFI_AUTHEN_UNKNOWN :
			break;
	}
	return retStr;
}

static char * P_WIFI_convertToPairwiseStr( DI_WIFI_ENCRYPT_e  encrypt )
{
	char *retStr = NULL;
	switch ( (unsigned int)encrypt )
	{
#if 0
		case DI_WIFI_ENCRYPT_WEP_64 :
			retStr = "WEP40";
			break;
		case DI_WIFI_ENCRYPT_WEP_128 :
			retStr = "WEP104";
			break;
#endif
		case DI_WIFI_ENCRYPT_TKIP :
			retStr = "TKIP";
			break;
		case DI_WIFI_ENCRYPT_AES :
			retStr = "CCMP";
			break;
		case DI_WIFI_ENCRYPT_TKIP_AES :
			retStr = "CCMP TKIP";
			break;
	}
	return retStr;
}

static char * P_WIFI_convertToGroupStr( DI_WIFI_ENCRYPT_e  encrypt )
{
	char *retStr = NULL;
	switch ( (unsigned int)encrypt )
	{
#if 0
		case DI_WIFI_ENCRYPT_WEP_64 :
			retStr = "WEP40";
			break;
		case DI_WIFI_ENCRYPT_WEP_128 :
			retStr = "WEP104";
			break;
#endif
		case DI_WIFI_ENCRYPT_TKIP :
			retStr = "TKIP";
			break;
		case DI_WIFI_ENCRYPT_AES :
			//sjpark3 : redmine #92725
			retStr = "CCMP TKIP";
			break;
		case DI_WIFI_ENCRYPT_TKIP_AES :
			retStr = "CCMP TKIP";
			break;
	}
	return retStr;
}

static DI_WIFI_ENCRYPT_e P_WIFI_convertToEncryptEnum( HUINT32 encrypt )
{
	DI_WIFI_ENCRYPT_e result = DI_WIFI_ENCRYPT_UNKNOWN;
	switch ( encrypt )
	{
		case 0 :
			PrintDebug("[%s]_%d DI_WIFI_ENCRYPT_NONE\n",__func__,__LINE__);
			result = DI_WIFI_ENCRYPT_NONE;
			break;
		case 1 :
			PrintDebug("[%s]_%d DI_WIFI_ENCRYPT_WEP_64\n",__func__,__LINE__);
			result = DI_WIFI_ENCRYPT_WEP_64;
			break;
		case 2 :
			PrintDebug("[%s]_%d DI_WIFI_ENCRYPT_TKIP\n",__func__,__LINE__);
			result = DI_WIFI_ENCRYPT_TKIP;
			break;
		case 3 :
			PrintDebug("[%s]_%d WRAP\n",__func__,__LINE__);
			/*"WRAP",*/
			result = DI_WIFI_ENCRYPT_UNKNOWN;
			break;
		case 4 :
			PrintDebug("[%s]_%d DI_WIFI_ENCRYPT_AES\n",__func__,__LINE__);
			result = DI_WIFI_ENCRYPT_AES;
			break;
		case 5 :
			PrintDebug("[%s]_%d DI_WIFI_ENCRYPT_WEP_128\n",__func__,__LINE__);
			result = DI_WIFI_ENCRYPT_WEP_128;
			break;
	}
	return result;
}

static HINT32 P_WIFI_GetKernelWeVer(void)
{
	HUINT8	acBuf[1024];
	FILE *	fh;
	char *	p;
	int 	v;

	/* Check if /proc/net/wireless is available */
	fh = fopen(PROC_NET_WIRELESS, "r");

	if(fh == NULL)
	{
		return(-1);
	}

	VK_MEM_Memset(acBuf,0,sizeof(acBuf));
	/* Read the first line of buffer */
	fgets((char*)acBuf, sizeof(acBuf), fh);

	if(VK_strstr((char*)acBuf, "| WE") == NULL)
	{
		/* Prior to WE16, so explicit version not present */
		/* Black magic */
		if(VK_strstr((char*)acBuf, "| Missed") == NULL)
		{
			v = 11;
		}
		else
		{
			v = 15;
		}

		fclose(fh);
		return(v);
	}

	VK_MEM_Memset((char*)acBuf,0,sizeof(acBuf));
	/* Read the second line of buffer */
	fgets((char*)acBuf, sizeof(acBuf), fh);

	/* Get to the last separator, to get the version */
	p = strrchr((char *)acBuf, '|');
	if((p == NULL) || (VK_sscanf(p + 1, "%d", &v) != 1))
	{
		fclose(fh);
		return(-1);
	}

	fclose(fh);
	return(v);
}

static inline void P_WIFI_IE_WPA(unsigned char *iebuf, int buflen, DI_WIFI_INFO_t *pApInfo)
{
	int			ielen = iebuf[1] + 2;
	int			offset = 2;	/* Skip the IE id, and the length. */
	unsigned char		wpa1_oui[3] = {0x00, 0x50, 0xf2};
	unsigned char		wpa2_oui[3] = {0x00, 0x0f, 0xac};
	unsigned char *	wpa_oui;
	int			i;
	unsigned short int		ver = 0;
	unsigned short int		cnt = 0;
	HUINT32	nEncryptType;

	if(ielen > buflen)
		ielen = buflen;

	switch(iebuf[0])
	{
		case 0x30:		/* WPA2 */
			/* Check if we have enough data */
			if(ielen < 4)
			{
				return;
			}
			wpa_oui = wpa2_oui;
			break;

		case 0xdd:		/* WPA or else */
			wpa_oui = wpa1_oui;

			/* Not all IEs that start with 0xdd are WPA.
			 * So check that the OUI is valid. Note : offset==2 */
			if((ielen < 8)
					|| (VK_memcmp(&iebuf[offset], wpa_oui, 3) != 0)
					|| (iebuf[offset + 3] != 0x01))
			{

				return;
			}

			/* Skip the OUI type */
			offset += 4;
			break;

		default:
			return;
	}

	/* Pick version number (little endian) */
	ver = iebuf[offset] | (iebuf[offset + 1] << 8);
	offset += 2;

	if(iebuf[0] == 0xdd)
	{
		pApInfo->eSecureType= DI_WIFI_SECURE_WPA;
	}
	else if(iebuf[0] == 0x30)
	{
		pApInfo->eSecureType= DI_WIFI_SECURE_WPA2;
	}
	else
	{
	}

	/* From here, everything is technically optional. */
	pApInfo->eEncryptType = DI_WIFI_ENCRYPT_UNKNOWN;


	/* Check if we are done */
	if(ielen < (offset + 4))
	{
		pApInfo->eEncryptType = DI_WIFI_ENCRYPT_TKIP;
		/* We have a short IE.  So we should assume TKIP/TKIP. */
		return;
	}

	/* Next we have our group cipher. */
	if(VK_memcmp(&iebuf[offset], wpa_oui, 3) != 0)
	{
	}
	else
	{
		pApInfo->eEncryptType = P_WIFI_convertToEncryptEnum( iebuf[offset+3] );
	}
	offset += 4;

	/* Check if we are done */
	if(ielen < (offset + 2))
	{
		/* We don't have a pairwise cipher, or auth method. Assume TKIP. */
		pApInfo->eEncryptType = DI_WIFI_ENCRYPT_TKIP;
		return;
	}

	/* 일단은 unknown 처리 */
	cnt = iebuf[offset] | (iebuf[offset + 1] << 8);
	offset += 2;

	if(ielen < (offset + 4*cnt))
	{
		return;
	}

	/* ENCRYPT가 TKIP/AES로 설정되어 있을때(2개)*/
	nEncryptType = pApInfo->eEncryptType;
	for(i = 0; i < cnt; i++)
	{
		if(VK_memcmp(&iebuf[offset], wpa_oui, 3) != 0)
		{
		}
		else
		{
			if(nEncryptType == DI_WIFI_ENCRYPT_AES)
			{
				pApInfo->eEncryptType = P_WIFI_convertToEncryptEnum( iebuf[offset+3] );
				if(pApInfo->eEncryptType == DI_WIFI_ENCRYPT_TKIP)
				{
					pApInfo->eEncryptType = DI_WIFI_ENCRYPT_TKIP_AES;
				}
			}
			else if(nEncryptType == DI_WIFI_ENCRYPT_TKIP)
			{
				pApInfo->eEncryptType = P_WIFI_convertToEncryptEnum( iebuf[offset+3] );
				if(pApInfo->eEncryptType == DI_WIFI_ENCRYPT_AES)
				{
					pApInfo->eEncryptType = DI_WIFI_ENCRYPT_TKIP_AES;
				}
			}
			else
			{
				pApInfo->eEncryptType = P_WIFI_convertToEncryptEnum( iebuf[offset+3] );
			}
		}
		offset+=4;
	}

	/* Check if we are done */
	if(ielen < (offset + 2))
	{
		return;
	}

	/* Now, we have authentication suites. */
	cnt = iebuf[offset] | (iebuf[offset + 1] << 8);
	offset += 2;

	if(ielen < (offset + 4*cnt))
	{
		return;
	}

	for(i = 0; i < cnt; i++)
	{
		if( 0 != VK_memcmp(&iebuf[offset], wpa_oui, 3) )
		{
		}
		else
		{
			/*
			"none",
			"802.1x",
			"PSK",
			*/
			if ( iebuf[offset+3] == 0 )
			{
				pApInfo->eAuthenType = DI_WIFI_AUTHEN_OPEN;
			}
			else if ( iebuf[offset+3] == 1 )
			{
				pApInfo->eAuthenType = DI_WIFI_AUTHEN_802_1X;
			}
			else if ( iebuf[offset+3] == 2 )
			{
				pApInfo->eAuthenType = DI_WIFI_AUTHEN_PSK;
			}
			else
			{
				pApInfo->eAuthenType = DI_WIFI_AUTHEN_UNKNOWN;
			}

		}
		offset+=4;

	}

	/* Check if we are done */
	if(ielen < (offset + 1))
	{
		return;
	}

	/* Otherwise, we have capabilities bytes.
	 * For now, we only care about preauth which is in bit position 1 of the
	 * first byte.  (But, preauth with WPA version 1 isn't supposed to be
	 * allowed.) 8-) */
	if(iebuf[offset] & 0x01)
	{
	}
}

#if defined(CONFIG_BCM43242)
static int P_WIFI_IsValidHwAddr(const HINT8 *pszStrBdAddr)
{
	if (!pszStrBdAddr)
		return -1;

	if (VK_strlen(pszStrBdAddr) != 17)
		return -1;

	if (!VK_strcmp(pszStrBdAddr, "00:00:00:00:00:00"))
		return -1;

	while (*pszStrBdAddr) {
		if (!isxdigit(*pszStrBdAddr++))
			return -1;
		if (!isxdigit(*pszStrBdAddr++))
			return -1;
		if (*pszStrBdAddr == 0)
			break;
		if (*pszStrBdAddr++ != ':')
			return -1;
	}
	return 0;
}

DI_ERR_CODE P_WIFI_GetHwAddr(unsigned char *retStr)
{
	DI_ERR_CODE diResult = DI_ERR_ERROR;
	unsigned char wlanHwAddr[MAC_ADDR_LEN];

	if( wlanHwAddr == NULL )
	{
		PrintError("[%s] Error: Invalid Parameter\n", __FUNCTION__);
		return diResult;
	}

	diResult = DI_NVRAM_Read( DI_NVRAM_FS_PARTITION_RO, "WLAN_MAC_ADDR", 0, wlanHwAddr, MAC_ADDR_LEN);
	VK_sprintf(retStr, MACSTR, MAC2STR(wlanHwAddr));
	if ( diResult != DI_ERR_OK )
	{
		PrintError("[%s] Error: DI_NVRAM_Read Error\n", __FUNCTION__);
		return diResult;
	}

	return diResult;
}
#endif
/*------------------------------------------------------------------*/
/*
 * Process a generic IE and display the info in human readable form
 * for some of the most interesting ones.
 * For now, we only decode the WPA IEs.
 */
static void P_WIFI_GetGenIE(unsigned char *buffer, int buflen, DI_WIFI_INFO_t *pApInfo)
{
	int offset = 0;

	/* Loop on each IE, each IE is minimum 2 bytes */
	while(offset <= (buflen - 2))
	{
		/* Check IE type */
		switch(buffer[offset])
		{
			case 0xdd:	/* WPA1 (and other) */
			case 0x30:	/* WPA2 */
				P_WIFI_IE_WPA(buffer + offset, buflen, pApInfo);
				break;
			default:
				pApInfo->eSecureType= DI_WIFI_SECURE_UNKNOWN;
				break;
	}
	/* Skip over this IE to the next one in the list. */
	offset += buffer[offset+1] + 2;
	}
}

static int P_WIFI_ExtractInfoFormDriverEvent(struct stream_descr *	stream,	/* Stream of events */
														struct iw_event *	iwe,	/* Extracted event */
														int			we_version)
{
  const struct iw_ioctl_description *	descr = NULL;
  int		event_type = 0;
  unsigned int	event_len = 1;		/* Invalid */
  char *	pointer;
  /* Don't "optimise" the following variable, it will crash */
  unsigned	cmd_index;		/* *MUST* be unsigned */

  /* Check for end of stream */
  if((stream->current + IW_EV_LCP_PK_LEN) > stream->end)
	return(0);

  /* Extract the event header (to get the event id).
   * Note : the event may be unaligned, therefore copy... */
  VK_memcpy((char *) iwe, stream->current, IW_EV_LCP_PK_LEN);

  /* Check invalid events */
  if(iwe->len <= IW_EV_LCP_PK_LEN)
	return(-1);

  /* Get the type and length of that event */
  if(iwe->cmd <= SIOCIWLAST)
	{
	  cmd_index = iwe->cmd - SIOCIWFIRST;
	  if(cmd_index < standard_ioctl_num)
	descr = &(standard_ioctl_descr[cmd_index]);
	}
  else
	{
	  cmd_index = iwe->cmd - IWEVFIRST;
	  if(cmd_index < standard_event_num)
	descr = &(standard_event_descr[cmd_index]);
	}
  if(descr != NULL)
	event_type = descr->header_type;
  /* Unknown events -> event_type=0 => IW_EV_LCP_PK_LEN */
  event_len = event_type_size[event_type];
  /* Fixup for earlier version of WE */
  if((we_version <= 18) && (event_type == IW_HEADER_TYPE_POINT))
	event_len += IW_EV_POINT_OFF;

  /* Check if we know about this event */
  if(event_len <= IW_EV_LCP_PK_LEN)
	{
	  /* Skip to next event */
	  stream->current += iwe->len;
	  return(2);
	}
  event_len -= IW_EV_LCP_PK_LEN;

  /* Set pointer on data */
  if(stream->value != NULL)
	pointer = (char *)stream->value;			/* Next value in event */
  else
	pointer = (char *)stream->current + IW_EV_LCP_PK_LEN;	/* First value in event */

  /* Copy the rest of the event (at least, fixed part) */
  if((char *)(pointer + event_len) >(char *) stream->end)
	{
	  /* Go to next event */
	  stream->current += iwe->len;
	  return(-2);
	}
  /* Fixup for WE-19 and later : pointer no longer in the stream */
  /* Beware of alignement. Dest has local alignement, not packed */
  if((we_version > 18) && (event_type == IW_HEADER_TYPE_POINT))
	VK_memcpy((char *) iwe + IW_EV_LCP_LEN + IW_EV_POINT_OFF,
	   pointer, event_len);
  else
	VK_memcpy((char *) iwe + IW_EV_LCP_LEN, pointer, event_len);

  /* Skip event in the stream */
  pointer += event_len;

  /* Special processing for iw_point events */
  if(event_type == IW_HEADER_TYPE_POINT)
	{
	  /* Check the length of the payload */
	  unsigned int	extra_len = iwe->len - (event_len + IW_EV_LCP_PK_LEN);
	  if(extra_len > 0)
	{
	  /* Set pointer on variable part (warning : non aligned) */
	  iwe->u.data.pointer = pointer;

	  /* Check that we have a descriptor for the command */
	  if(descr == NULL)
		/* Can't check payload -> unsafe... */
		iwe->u.data.pointer = NULL; /* Discard paylod */
	  else
		{
		  /* Those checks are actually pretty hard to trigger,
		   * because of the checks done in the kernel... */

		  unsigned int	token_len = iwe->u.data.length * descr->token_size;

		  /* Ugly fixup for alignement issues.
		   * If the kernel is 64 bits and userspace 32 bits,
		   * we have an extra 4+4 bytes.
		   * Fixing that in the kernel would break 64 bits userspace. */
		  if((token_len != extra_len) && (extra_len >= 4))
		{
		  __u16 	alt_dlen = *((__u16 *) pointer);
		  unsigned int	alt_token_len = alt_dlen * descr->token_size;
		  if((alt_token_len + 8) == extra_len)
			{
			  /* Ok, let's redo everything */
			  pointer -= event_len;
			  pointer += 4;
			  /* Dest has local alignement, not packed */
			  VK_memcpy((char *) iwe + IW_EV_LCP_LEN + IW_EV_POINT_OFF,
				 pointer, event_len);
			  pointer += event_len + 4;
			  iwe->u.data.pointer = pointer;
			  token_len = alt_token_len;
			}
		}

		  /* Discard bogus events which advertise more tokens than
		   * what they carry... */
		  if(token_len > extra_len)
		iwe->u.data.pointer = NULL; /* Discard paylod */
		  /* Check that the advertised token size is not going to
		   * produce buffer overflow to our caller... */
		  if((iwe->u.data.length > descr->max_tokens)
		 && !(descr->flags & IW_DESCR_FLAG_NOMAX))
		iwe->u.data.pointer = NULL; /* Discard paylod */
		  /* Same for underflows... */
		  if(iwe->u.data.length < descr->min_tokens)
		iwe->u.data.pointer = NULL; /* Discard paylod */
		}
	}
	  else
	/* No data */
	iwe->u.data.pointer = NULL;

	  /* Go to next event */
	  stream->current += iwe->len;
	}
  else
	{
	  /* Ugly fixup for alignement issues.
	   * If the kernel is 64 bits and userspace 32 bits,
	   * we have an extra 4 bytes.
	   * Fixing that in the kernel would break 64 bits userspace. */
	  if((stream->value == NULL)
	 && ((((iwe->len - IW_EV_LCP_PK_LEN) % event_len) == 4)
		 || ((iwe->len == 12) && ((event_type == IW_HEADER_TYPE_UINT) ||
					  (event_type == IW_HEADER_TYPE_QUAL))) ))
	{

	  pointer -= event_len;
	  pointer += 4;
	  /* Beware of alignement. Dest has local alignement, not packed */
	  VK_memcpy((char *) iwe + IW_EV_LCP_LEN, pointer, event_len);
	  pointer += event_len;
	}

	  /* Is there more value in the event ? */
	  if((char *)(pointer + event_len) <= (char *)(stream->current + iwe->len))
	/* Go to next value */
	stream->value = (unsigned char *)pointer;
	  else
	{
	  /* Go to next event */
	  stream->value = NULL;
	  stream->current += iwe->len;
	}
	}
  return(1);
}

static void P_WIFI_MakeAPlist_ScanningInfo(struct iw_event *event, DI_WIFI_INFO_t *pApInfo)
{
	/* Now, let's decode the event */
	switch(event->cmd)
	{
		case SIOCGIWAP: /*0x8b15*/
			/* get access point MAC addresses */
			VK_memcpy(pApInfo->aucMAC,&event->u.ap_addr.sa_data,WIFI_MAC_SIZE);
			break;

		case SIOCGIWESSID: /*0x8b1b*/
			/* get ESSID */
			if((event->u.essid.pointer) && (event->u.essid.length))
			{
				VK_memcpy((char *)pApInfo->aucESSID, (char *)event->u.essid.pointer, event->u.essid.length);
			}
			break;

		case SIOCGIWMODE: /*0x8b07*/
			/*event->u.mode = { "Auto","Ad-Hoc","Managed","Master","Repeater","Secondary","Monitor","Unknown/bug" }*/
			switch(event->u.mode)
			{
				case 0: /*Auto*/
					pApInfo->eOpMode = DI_WIFI_OP_MODE_ALL;
					break;
				case 1: /*Ad-Hoc*/
					pApInfo->eOpMode = DI_WIFI_OP_MODE_ADHOC;
					break;
				case 2: /*Managed*/
					pApInfo->eOpMode = DI_WIFI_OP_MODE_MANAGED;
					break;
				case 3: /*Master*/
					pApInfo->eOpMode = DI_WIFI_OP_MODE_MASTER;
					break;
				case 4: /*Repeater*/
					pApInfo->eOpMode = DI_WIFI_OP_MODE_UNKNOWN;
					break;
				case 5: /*Secondary*/
					pApInfo->eOpMode = DI_WIFI_OP_MODE_UNKNOWN;
					break;
				case 6: /*Monitor*/
					pApInfo->eOpMode = DI_WIFI_OP_MODE_UNKNOWN;
					break;
				default:
					pApInfo->eOpMode = DI_WIFI_OP_MODE_UNKNOWN;
					break;
			}
			break;
		case SIOCGIWFREQ: /*0x8b01*/
			/* get channel/frequency (Hz) */
			break;
		case IWEVQUAL: /*0x8C01*/
			/* Quality part of statistics (scan) */
			pApInfo->nWaveStrength = event->u.qual.qual;
			break;
		case IWEVGENIE: /*0x8c05 */
			/* Generic IE (WPA, RSN, WMM, ..)*/
			P_WIFI_GetGenIE(event->u.data.pointer, event->u.data.length,pApInfo);
			break;
		case SIOCGIWENCODE: /*0x8B2B*/
			/* get encoding token & mode */
				if(event->u.data.flags & IW_ENCODE_DISABLED)
				{
					/* open */
				}
				else
				{
					/* Security */
					if(pApInfo->eSecureType == DI_WIFI_SECURE_OPEN)
					{
						pApInfo->eSecureType = DI_WIFI_SECURE_WEP;
						pApInfo->eAuthenType = DI_WIFI_AUTHEN_OPEN;
						pApInfo->eEncryptType = DI_WIFI_ENCRYPT_WEP_64;
					}
				}
			break;
		case SIOCGIWRATE: /* 0x8B21 */
			/* get default bit rate (bps) */
			pApInfo->nBitRate = (HINT32)event->u.bitrate.value;
			break;
		case SIOCGIWNWID:
			break;
		/* dont need */
		case SIOCGIWNAME:
			break;
		case SIOCGIWMODUL:
			break;
		case IWEVCUSTOM:
			break;
		default:
			break;
	}	/* switch(event->cmd) */
}

static DRV_Error P_WIFI_MakeWpaSupplicantConf( HINT8 *pszIFname, DI_WIFI_INFO_t *pstConfig )
{
	FILE *fp;
	char *szType = NULL;
	char configFile[48] = { 0 };
	HUINT8 ucKeyLen = 0;

	VK_snprintf( configFile, sizeof(configFile), WPA_SUPPLICANT_CONFIG_FILE_PATTERN, pszIFname );

	if ( DI_WIFI_SECURE_WPA ==  pstConfig->eSecureType || DI_WIFI_SECURE_WPA2 == pstConfig->eSecureType || DI_WIFI_SECURE_WPA_WPA2 == pstConfig->eSecureType)
	{
		/* WAP or WPA2 일경우 wpa_passphrase를 이용하여 KEY값을 PSK(Pre Shared key)로 생성.
		 * 또한, wpa_passphrase를 사용할 경우 network 블록을 포함하는 파일이 생성됨. */
		HINT32 nRet = 0;
		char cmdBuf[512];
		VK_snprintf( cmdBuf, sizeof( cmdBuf ), "wpa_passphrase \"%s\" \"%s\" > %s", pstConfig->aucESSID, pstConfig->aucKey, configFile  );
		VK_SYSTEM_Command( cmdBuf );

		fp = fopen( configFile, "r+" );
		if ( NULL == fp )
		{
			return DRV_ERR;
		}

		nRet = fseek( fp, -2, SEEK_END );
		if(nRet != 0)
		{
			PrintError("[%s] Err fseek %s\n",__FUNCTION__, strerror( errno ));
			fclose(fp);
			return DRV_ERR;
		}
	}
	else
	{
		/* WPA or WPA2가 아닐 경우 network 블록을 파일에 직접 생성 */
		fp = fopen( configFile, "w" );
		if ( NULL == fp )
		{
			return DRV_ERR;
		}
		VK_fprintf( fp, "network={\n" );
		VK_fprintf( fp, "\tssid=\"%s\"\n", pstConfig->aucESSID );
	}

#if 1 /* __HUMAX_CODE__ - 140520 mskim (redmine #91515) */
    VK_fprintf( fp, "scan_ssid=1\n" );
#endif

	PrintDebug("(*) Secure Value : %d\n", pstConfig->eSecureType);
	szType = P_WIFI_convertToProtoStr( pstConfig->eSecureType);
	if ( NULL != szType )
	{
		VK_fprintf( fp, "\tproto=%s\n", szType );
	}

	szType = P_WIFI_convertToKeyMGMTStr( pstConfig->eAuthenType );
	if ( NULL != szType )
	{
		VK_fprintf( fp, "\tkey_mgmt=%s\n", szType );
	}

	if ((DI_WIFI_ENCRYPT_WEP_64 == pstConfig->eEncryptType) ||
		(DI_WIFI_ENCRYPT_WEP_64HEX == pstConfig->eEncryptType) ||
		(DI_WIFI_ENCRYPT_WEP_128 == pstConfig->eEncryptType) ||
		(DI_WIFI_ENCRYPT_WEP_128HEX == pstConfig->eEncryptType))
	{
		ucKeyLen = VK_strlen(pstConfig->aucKey);
		if ((ucKeyLen == 5) || (ucKeyLen == 13))
		{
			/* DI_WIFI_ENCRYPT_WEP_64 or DI_WIFI_ENCRYPT_WEP_128 */
			VK_fprintf( fp, "\twep_key%d=\"%s\"\n", pstConfig->aucKeyIndex,pstConfig->aucKey );
			VK_fprintf( fp, "\twep_tx_keyidx=%d\n", pstConfig->aucKeyIndex);
		}
		else if ((ucKeyLen == 10) || (ucKeyLen == 26))
		{
			/* DI_WIFI_ENCRYPT_WEP_64HEX or DI_WIFI_ENCRYPT_WEP_128HEX */
			VK_fprintf( fp, "\twep_key%d=%s\n", pstConfig->aucKeyIndex,pstConfig->aucKey );
			VK_fprintf( fp, "\twep_tx_keyidx=%d\n", pstConfig->aucKeyIndex );
		}
	}

	PrintDebug("(*) Authen Value : %d\n", pstConfig->eAuthenType );
#if 0
	szType = P_WIFI_convertToAuthenStr( pstConfig->eSecureType );
	if ( NULL != szType )
	{
		VK_fprintf( fp, "\tauth_alg=%s\n", szType );
	}
#else
	VK_fprintf( fp, "\tauth_alg=OPEN\n");
#endif
	PrintDebug("(*) EncryptType Value : %d\n", pstConfig->eEncryptType );

	szType = P_WIFI_convertToPairwiseStr( pstConfig->eEncryptType );
	if ( NULL != szType )
	{
		VK_fprintf( fp, "\tpairwise=%s\n", szType );
	}

	szType = P_WIFI_convertToGroupStr( pstConfig->eEncryptType );
	if ( NULL != szType )
	{
		VK_fprintf( fp, "\tgroup=%s\n", szType );
	}

	VK_fprintf( fp, "}\n" );

	VK_fprintf( fp, "ctrl_interface=DIR=/var/run/wpa_supplicant\n" );
	VK_fprintf( fp, "update_config=1\n" );

#if 1 /* __HUMAX_CODE__ - 140409 mskim (redmine #88454) */
    if (VK_strncmp("rt5572sta", wifiDevName, VK_strlen("rt5572sta")) != 0 && VK_strncmp("mt7601Usta", wifiDevName, VK_strlen("mt7601Usta")) != 0)
    {
    	if ( pstConfig->eEncryptType == DI_WIFI_ENCRYPT_TKIP_AES)
    	{
    		VK_fprintf( fp, "ap_scan=1\n" );
    	}
    	else
    	{
    		VK_fprintf( fp, "ap_scan=2\n" );
    	}
    }
#else
	if ( pstConfig->eEncryptType == DI_WIFI_ENCRYPT_TKIP_AES)
	{
		VK_fprintf( fp, "ap_scan=1\n" );
	}
	else
	{
		VK_fprintf( fp, "ap_scan=2\n" );
	}
#endif

	fclose( fp );

	return DRV_OK;
}

static DRV_Error P_WIFI_IsConnect(HUINT8 *pFileName, HUINT8 *pucMAC)
{
	HUINT32		ulMac[6];
	FILE		*fpStatusFile = NULL;
	char		buf[128] = { 0 };
	HCHAR* 		pTemp = 0;

	if ( NULL == pFileName )
	{
		PrintError("[%s:%d] pFileName is NULL.\n",__FUNCTION__,__LINE__);
		return DRV_ERR;
	}

	fpStatusFile = fopen( pFileName, "r");
	if ( NULL == fpStatusFile )
	{
		PrintError("[%s:%d] fopen failed.\n",__FUNCTION__,__LINE__);
		return DRV_ERR;
	}

	fgets(buf, sizeof(buf), fpStatusFile );

	/*fgets 사용시 개행문자 주의*/
	while( fgets(buf, sizeof(buf), fpStatusFile ) )
	{
		pTemp = NULL;
		pTemp = VK_strstr(buf,"bssid=");
		if(pTemp)
		{
			VK_sscanf(buf+6,"%x:%x:%x:%x:%x:%x", &ulMac[0],&ulMac[1],&ulMac[2],&ulMac[3],&ulMac[4],&ulMac[5]);
			pucMAC[0] = ulMac[0];pucMAC[1] = ulMac[1];
			pucMAC[2] = ulMac[2];pucMAC[3] = ulMac[3];
			pucMAC[4] = ulMac[4];pucMAC[5] = ulMac[5];
			fclose( fpStatusFile );
			PrintError("[%s:%d] %x %x %x %x %x %x\n",__FUNCTION__,__LINE__, pucMAC[0], pucMAC[1], pucMAC[2], pucMAC[3], pucMAC[4], pucMAC[5]);
			return DRV_OK;
		}
		VK_memset( buf, 0x00, sizeof(buf) );
	}


	fclose( fpStatusFile );
	return DRV_ERR;


}

static HINT32 P_GetConnectStatusFromStatusFile ( const HCHAR *pszStatusFile )
{
	FILE		*fpStatusFile = NULL;
	char		buf[128] = { 0 };

	if ( NULL == pszStatusFile )
	{
		PrintError("[%s:%d] pszStatusFile is NULL.\n",__FUNCTION__,__LINE__);
		return DRV_ERR;
	}

	fpStatusFile = fopen( pszStatusFile, "r");
	if ( NULL == fpStatusFile )
	{
		PrintError("[%s:%d] fopen failed.\n",__FUNCTION__,__LINE__);
		return DRV_ERR;
	}

	fgets(buf, sizeof(buf), fpStatusFile );

	/*fgets 사용시 개행문자 주의*/
	while( fgets(buf, sizeof(buf), fpStatusFile ) )
	{
		if ( VK_strstr(buf, "wpa_state=COMPLETED") )
		{
			//PrintDebug("[%s:%d] Complete!!!!!!!!!!!!!!!!!!!!\n",__FUNCTION__,__LINE__);
			fclose( fpStatusFile );
			return DRV_OK;
		}
		VK_memset( buf, 0x00, sizeof(buf) );
	}

	fclose( fpStatusFile );
	return DRV_ERR;
}

static DRV_Error P_WIFI_MakeWpaSupplicantConfForWps( HINT8 *pszIFname, DI_WIFI_INFO_t *pstConfig )
{
	FILE *fp;
	char configFile[48] = { 0 };

	UNUSED(pstConfig);

	VK_snprintf( configFile, sizeof(configFile), WPA_SUPPLICANT_CONFIG_FILE_PATTERN, pszIFname );

	/* WPS 일 경우 기본 config 파일을 직접 생성 */
	fp = fopen( configFile, "w" );
	if ( NULL == fp )
	{
		return DRV_ERR;
	}

	VK_fprintf( fp, "ctrl_interface=DIR=/var/run/wpa_supplicant\n" );
	VK_fprintf( fp, "update_config=1\n" );
	VK_fprintf( fp, "ap_scan=1\n" );

	fclose( fp );

	return DRV_OK;
}

static DRV_Error P_WIFI_KillWpaSupplicant( HINT8 *pszIFname )
{
	int		nWpaSupPid = 0;

	char		pidFileName[48] = { 0 };
	char		confFileName[48] = { 0 };
	char		pidCmdLineFile[256] = { 0 };
	FILE		*pidFile = NULL;
	FILE		*cmdFile = NULL;
	char		pidBuf[8] = { 0 };
	char		cmdBuf[256] = { 0 };
	char		*pRead = NULL;
	char		*pSearch = NULL;

	VK_snprintf( pidFileName, sizeof(pidFileName), WPA_SUPPLICANT_PID_FILE_PATTERN, pszIFname );

	pidFile = fopen( pidFileName, "r");
	if ( NULL == pidFile )
	{
		PrintError("\n[%s:%d]\n",__FUNCTION__,__LINE__);
		return DRV_ERR;
	}

	pRead = fgets( pidBuf, sizeof(pidBuf), pidFile );
	if ( NULL == pRead )
	{
		PrintError("\n[%s:%d]\n",__FUNCTION__,__LINE__);
		fclose( pidFile );
		return DRV_ERR;
	}

	nWpaSupPid = VK_atoi( pidBuf );

	VK_snprintf( pidCmdLineFile, sizeof(pidCmdLineFile), PROCESS_INFO_CMDLINE_PATH, nWpaSupPid );

	// 해당 프로세서가 실제 있는지 파일(roc/pid/cmdline) 존재 여부를  확인한다.
	cmdFile = fopen( pidCmdLineFile, "r" );
	if ( NULL  == cmdFile )
	{
		PrintError("\n[%s:%d]\n",__FUNCTION__,__LINE__);
		fclose( pidFile );
		return DRV_ERR;
	}
	pRead = fgets( cmdBuf, sizeof(cmdBuf), cmdFile );
	if ( NULL == pRead )
	{
		PrintError("\n[%s:%d]\n",__FUNCTION__,__LINE__);
		fclose( pidFile );
		fclose( cmdFile );
		return DRV_ERR;
	}
	// 또한, cmdline의 프로세서가 실제 wpa_supplicant인지 확인해 본다.
	pSearch = VK_strstr( cmdBuf, "wpa_supplicant" );
	if ( NULL == pSearch )
	{
		PrintError("\n[%s:%d]\n",__FUNCTION__,__LINE__);
		fclose( pidFile );
		fclose( cmdFile );
		return DRV_ERR;
	}

	// 그리고 wpa_supplicant를 kill 한다.
	kill( nWpaSupPid, SIGKILL );

	fclose( pidFile );
	fclose( cmdFile );

	// remove files related to wpa_supplicant
	// pid file
	VK_MEM_Memset(cmdBuf, 0x00, sizeof(cmdBuf));
	VK_snprintf(cmdBuf, sizeof(cmdBuf), "rm -rf %s", pidFileName);
	VK_SYSTEM_Command(cmdBuf);

	// config file
	VK_MEM_Memset(cmdBuf, 0x00, sizeof(cmdBuf));
	VK_snprintf(confFileName, sizeof(confFileName), WPA_SUPPLICANT_CONFIG_FILE_PATTERN, pszIFname);
	VK_snprintf(cmdBuf, sizeof(cmdBuf), "rm -rf %s", confFileName);
	VK_SYSTEM_Command(cmdBuf);

	return DRV_OK;
}


/* WPA_Supplicant only functions : END */


static void P_WIFI_SetBrcmUp(void)
{
#if defined(CONFIG_BCM43236_WNC_DONGLE)
	char acWlCmdBuf[64];
#endif

	VK_SYSTEM_Command("wl up");
	PrintDebug("[%s]_%d %s\n",__func__,__LINE__,"wl up");
	VK_TASK_Sleep(200);

#if defined(CONFIG_BCM43236_WNC_DONGLE)
	VK_MEM_Memset(acWlCmdBuf, 0x00, sizeof(acWlCmdBuf));
	VK_snprintf(acWlCmdBuf,sizeof(acWlCmdBuf), "wl country %s", CONFIG_BCM43236_WNC_DONGLE_CCODE);
	VK_SYSTEM_Command(acWlCmdBuf);
	PrintDebug("[%s]_%d %s\n",__func__,__LINE__,acWlCmdBuf);
#endif
}

static void P_WIFI_SetBrcmDown(void)
{
	VK_SYSTEM_Command("wl disassoc");
	PrintDebug("[%s]_%d %s\n",__func__,__LINE__,"wl disassoc");
	VK_TASK_Sleep(50);
	
	VK_SYSTEM_Command("wl down");
	PrintDebug("[%s]_%d %s\n",__func__,__LINE__,"wl down");
	VK_TASK_Sleep(200);
}

static HINT32 P_WIFI_WhichNumber(HINT8 *pcStr)
{
	int len, i;

	len = VK_strlen((char *)pcStr);

	for(i = 0 ; i < len; i++)
	{
		if((pcStr[i] <'0' || pcStr[i] > '9'))
		{
			return -1;
		}
	}
	return VK_atoi((char *)pcStr);
}

static HINT32 P_WIFI_GetPidByName(HINT8 *pcStr)
{
	DIR *dp;
	struct dirent *dir;
	char buf[100],line[1024],tag[100],name[100];
	int pid;
	FILE *fp;

	dp = opendir("proc");
	if(!dp)
	{
		return -1;
	}

	while((dir = readdir(dp)))
	{
		pid = P_WIFI_WhichNumber((HINT8 *)dir->d_name);
		if(pid == -1)
		{
			continue;
		}

		/* open /proc/pid/status file. */
		VK_snprintf(buf,100,"proc/%d/status",pid);
		fp = fopen(buf, "r");
		if(fp == NULL)
		{
			continue;
		}

		/* Get first line with name*/
		fgets(line,1024,fp);

		/*close stream . */
		fclose(fp);
		VK_sscanf(line,"%s %s",tag,name);
		if(!VK_strncmp(name,(char *)pcStr,sizeof(name)))
		{
			closedir(dp);
			return pid;
		}
	}

	closedir(dp);
	return -1;

}

/* P_WIFI_DisConnectCallback 호출 시 s_stWiFiInstance.ulDisConnectTimerId 값 체크 하지 않는다. */
/* Semaphore 로 보호 되지 않음 */
/* WIFI_EVT_DISCONNECT 받는 부분에서 DISCONNECT 상태 체크 함 */
static void P_WIFI_DisConnectCallback(unsigned long ulTid, void *arg)
{
	HINT32 		nVkErr = 0;
	WIFI_MSG_t 	stWiFiMsg;
	HUINT32 ulIndex = *((HUINT32 *)arg);

	PrintEnter();

	if(s_stWiFiInstance.ulDisConnectTimerId == ulTid)
	{
		stWiFiMsg.ulIndex = ulIndex;
		stWiFiMsg.eMsg = WIFI_EVT_CONNECT_FAIL;

		nVkErr = VK_MSG_SendTimeout(s_stWiFiInstance.ulMsg, &stWiFiMsg, sizeof(WIFI_MSG_t),0);
		if( nVkErr != 0 )
		{
			PrintError("[%s] VK_MSG_Send Err(0x%x)!!\n",__func__,nVkErr);
		}
	}
	s_stWiFiInstance.ulDisConnectTimerId = (HUINT32)NULL;

	PrintExit();

	return ;
}

static HUINT32 P_WIFI_GetIndexbyDevId(HUINT32 ulDevId)
{
	HUINT32 ulLoop =0;

	for(ulLoop = 0; ulLoop < WIFI_MAX_NUM; ulLoop++)
	{
		if(s_stWiFiInstance.stWiFiInfo[ulLoop].ulDevId == ulDevId)
		{
			break;
		}
	}
	return ulLoop;
}

static HINT32 P_GetKeyStatus( const HCHAR *pszStatusFile )
{
	FILE		*fpStatusFile = NULL;
	char		buf[5] = { 0 };

	if ( NULL == pszStatusFile )
	{
		PrintError("[%s:%d] pszStatusFile is NULL.\n",__FUNCTION__,__LINE__);
		return DRV_ERR;
	}

	fpStatusFile = fopen( pszStatusFile, "r");
	if ( NULL == fpStatusFile )
	{
		PrintError("[%s:%d] fopen failed.\n",__FUNCTION__,__LINE__);
		return DRV_ERR;
	}

	/*fgets 사용시 개행문자 주의*/
	while( fgets(buf, sizeof(buf), fpStatusFile ) )
	{
		if ( VK_strstr(buf, "6")!=NULL )
		{
			//PrintDebug("Key Exchange Complete !!!!!!!!!!!!!!!!!!!!\n");
			fclose( fpStatusFile );
			return DRV_OK;
		}
		VK_memset( buf, 0x00, sizeof(buf) );
	}

	fclose( fpStatusFile );
	return DRV_ERR;
}

static HINT32 P_GetConnectStatus( const HCHAR *pszStatusFile ,HUINT8 *pucMac)
{
	FILE		*fpStatusFile = NULL;
	char		buf[128] = { 0 };
	HCHAR* 		pTemp = 0;
	HUINT32		ulMac[6];
	HCHAR pCmd0[129];
	HCHAR	*pLine;

	fpStatusFile = fopen( pszStatusFile, "r");
	if ( NULL == fpStatusFile )
	{
		PrintError("[%s:%d] fopen failed.\n",__FUNCTION__,__LINE__);
		return DRV_ERR;
	}

	VK_MEM_Memset(pucMac,0,6);

	/*fgets 사용시 개행문자 주의*/
	while( fgets(buf, sizeof(buf), fpStatusFile ) )
	{
		pTemp = NULL;
		/* Remove '\n' */
		if((pLine = VK_strchr(buf,'\n')) !=NULL)
		{
			*pLine='\0';
		}

		pTemp = VK_strstr(buf,"BSSID:");
		if(pTemp)
		{
			VK_sscanf(buf,"%s %x:%x:%x:%x:%x:%x",pCmd0,&ulMac[0],&ulMac[1],&ulMac[2],&ulMac[3],&ulMac[4],&ulMac[5]);
			pucMac[0] = ulMac[0];pucMac[1] = ulMac[1];
			pucMac[2] = ulMac[2];pucMac[3] = ulMac[3];
			pucMac[4] = ulMac[4];pucMac[5] = ulMac[5];
			fclose( fpStatusFile );
			return DRV_OK;
		}
		VK_memset( buf, 0x00, sizeof(buf) );
	}

	fclose( fpStatusFile );
	return DRV_ERR;
}

static DRV_Error P_WIFI_GetAPInfo(HUINT32 ulIndex, DI_WIFI_INFO_t *pstApInfo)
{
	FILE *fp = NULL;
	HCHAR* pTemp = 0;
	HCHAR input[129];
	HCHAR pCmd0[129];
	HCHAR pCmd1[129];
	HCHAR pCmd2[129];

	HCHAR acWlStatsFilename[32] = {0,};
	HCHAR acCmdLine[64] = {0,};

	HUINT32 ulMac[6];
	HINT32 nValue = 0;
	HUINT32 ulStep = 0;
	HUINT32 ulLutIdx = 0;

	VK_MEM_Memset(acWlStatsFilename, 0x00, sizeof(acWlStatsFilename));
	VK_MEM_Memset(acCmdLine, 0x00, sizeof(acCmdLine));

	VK_snprintf(acWlStatsFilename, sizeof(acWlStatsFilename)-1, WL_STATUS_FILE_PATTERN, s_stWiFiInstance.stWiFiInfo[ulIndex].acIfName);
	VK_snprintf(acCmdLine, sizeof(acCmdLine)-1, "wl status > %s", acWlStatsFilename);

	VK_SYSTEM_Command(acCmdLine);

	fp = fopen(acWlStatsFilename,"r");
	if(fp == NULL)
	{
		PrintError("[P_WIFI_GetAPInfo] Err fopen %s\n",strerror( errno ));
		return DRV_ERR;
	}

	VK_MEM_Memset(input, 0x00, 129);
	VK_MEM_Memset(pCmd0, 0x00, 129);
	VK_MEM_Memset(pCmd1, 0x00, 129);
	VK_MEM_Memset(pCmd2, 0x00, 129);

	while(fgets(input,128,fp)!= NULL)
	{
		pTemp = NULL;
		switch(ulStep)
		{
			case 0:/* SSID: "SWSONG_Network_1" */
				pTemp = VK_strstr(input,"SSID:");
				if(pTemp)
				{
					VK_sscanf(pTemp,"%s \"%s",pCmd0,pCmd1);
					/*== SSID */
					ulStep = 1;

					if(!((pCmd1[0] == 0x5C)&&(pCmd1[1] == 0x78)&&(pCmd1[2] == 0x30)&&(pCmd1[3] == 0x30)))
					{
						HUINT8 aucTempEssid[33] = {0,};
						HCHAR	*p = pTemp;
						HINT32	i = 0;

						while (*p++ != '\"')
						{
							/* move to the start pointer of ESSID string */
						}

						while (1)
						{
							if (*p != '\n')
							{
								aucTempEssid[i++] = *p++;
							}
							else
							{
								aucTempEssid[i-1] = '\0';
								break;
							}
						}

						VK_snprintf(pstApInfo->aucESSID,sizeof(aucTempEssid),aucTempEssid);
					}
				}
				break;

			case 1:/*Mode: Managed	 RSSI: -48 dBm	 SNR: 0 dB	  noise: -69 dBm  Flags: RSSI on-channel  Channel: 3*/
				ulStep = 0;
				pTemp = VK_strstr(input,"Mode:");
				if(pTemp)
				{
					ulStep = 2;
					VK_sscanf(pTemp,"%s %s %s %d",pCmd0,pCmd1,pCmd2,&nValue);

					nValue = (90 + nValue) * 2;
					if(nValue > 100) nValue = 100;
					else if(nValue < 0) nValue = 0;

					pstApInfo->nWaveStrength = nValue;
					if(!VK_strncmp("Managed",pCmd1,VK_strlen(pCmd1)))
					{
						pstApInfo->eOpMode = DI_WIFI_OP_MODE_MANAGED;
						/*== Mode */
					}
					else
					{
						pstApInfo->eOpMode = DI_WIFI_OP_MODE_UNKNOWN;
					}
					pTemp = VK_strstr(input,"Channel:");
					if(pTemp)
					{
						VK_sscanf(pTemp,"%s %d",pCmd0,&nValue);
						/*== Channel */
						pstApInfo->ulChannel = nValue;
						for (ulLutIdx = 0 ; ulLutIdx < WIFI_CHANNEL_LUT_SIZE ; ulLutIdx++)
						{
							if (g_stWifiChLut[ulLutIdx].ulChannelNo == pstApInfo->ulChannel)
							{
								pstApInfo->ulFreq = g_stWifiChLut[ulLutIdx].ulFrequency;
							}
						}
					}
				}
				break;

			case 2:/*BSSID: 00:18:E7:EA:6A:43		 Capability: ESS WEP ShortPre ShortSlot*/
				ulStep = 0;
				pTemp = VK_strstr(input,"BSSID:");
				if(pTemp)
				{
					ulStep = 3;
					VK_sscanf(input,"%s %x:%x:%x:%x:%x:%x",pCmd0,&ulMac[0],&ulMac[1],&ulMac[2],&ulMac[3],&ulMac[4],&ulMac[5]);
					/*== BSSID */
					pstApInfo->aucMAC[0] = ulMac[0];
					pstApInfo->aucMAC[1] = ulMac[1];
					pstApInfo->aucMAC[2] = ulMac[2];
					pstApInfo->aucMAC[3] = ulMac[3];
					pstApInfo->aucMAC[4] = ulMac[4];
					pstApInfo->aucMAC[5] = ulMac[5];

					pTemp = VK_strstr(input,"WEP");
					if(pTemp)
					{
						pstApInfo->eSecureType = DI_WIFI_SECURE_WEP;
						pstApInfo->eAuthenType = DI_WIFI_AUTHEN_OPEN;
						pstApInfo->eEncryptType = DI_WIFI_ENCRYPT_WEP_64;
					}
					else
					{
						pstApInfo->eSecureType = DI_WIFI_SECURE_OPEN;
						pstApInfo->eAuthenType = DI_WIFI_AUTHEN_OPEN;
						pstApInfo->eEncryptType = DI_WIFI_ENCRYPT_NONE;
						ulStep = 6;
					}
				}
				break;

			case 3:/*WPA: or RSN:*/
				/* WPA 인지 RSN인지 선택 */
				pTemp = VK_strstr(input,"WPA:");
				if(pTemp)
				{
					ulStep = 4;
				}
				else
				{
#if defined(CONFIG_BCM43236_BISON) || defined(CONFIG_BCM43567)
					pTemp = VK_strstr(input,"RSN (WPA2)");
#else
					pTemp = VK_strstr(input,"RSN:");
#endif
					if(pTemp)
					{
						ulStep = 5;
					}
				}

				if(pTemp == NULL)
				{
					pTemp = VK_strstr(input,"WPS:");
					if(pTemp)
					{
						pstApInfo->bWPS = TRUE;
						ulStep = 0;
					}
				}
				break;

			case 4: /* WPA */
				/*unicast ciphers(1): AES-CCMP */
				/*unicast ciphers(1): TKIP */
				/*unicast ciphers(2): TKIP AES-CCMP */
				pTemp = VK_strstr(input,"ciphers(1):");
				if(pTemp)
				{
					VK_sscanf(pTemp,"%s %s",pCmd0,pCmd1);
					pstApInfo->eSecureType = DI_WIFI_SECURE_WPA;
					pstApInfo->eAuthenType = DI_WIFI_AUTHEN_PSK;
					if(!VK_strncmp("AES-CCMP",pCmd1,VK_strlen(pCmd1)))
					{
						pstApInfo->eEncryptType = DI_WIFI_ENCRYPT_AES;
					}
					else if(!VK_strncmp("TKIP",pCmd1,VK_strlen(pCmd1)))
					{
						pstApInfo->eEncryptType = DI_WIFI_ENCRYPT_TKIP;
					}
					else
					{
						pstApInfo->eEncryptType = DI_WIFI_ENCRYPT_UNKNOWN;
					}
					ulStep = 5;
				}

				if(pTemp == NULL)
				{
					pTemp = VK_strstr(input,"ciphers(2):");
					if(pTemp)
					{
						VK_sscanf(pTemp,"%s %s %s",pCmd0,pCmd1,pCmd2);
						pstApInfo->eSecureType = DI_WIFI_SECURE_WPA;
						pstApInfo->eAuthenType = DI_WIFI_AUTHEN_PSK;
						pstApInfo->eEncryptType = DI_WIFI_ENCRYPT_TKIP_AES;
						ulStep = 5;
					}
				}
				break;

			case 5: /* RSN */
				/*unicast ciphers(1): AES-CCMP */
				/*unicast ciphers(1): TKIP */
				/*unicast ciphers(2): TKIP AES-CCMP */
				pTemp = VK_strstr(input,"ciphers(1):");
				if(pTemp)
				{
					VK_sscanf(pTemp,"%s %s",pCmd0,pCmd1);
					pstApInfo->eSecureType = DI_WIFI_SECURE_WPA2;
					pstApInfo->eAuthenType = DI_WIFI_AUTHEN_PSK;
					if(!VK_strncmp("AES-CCMP",pCmd1,VK_strlen(pCmd1)))
					{
						pstApInfo->eEncryptType = DI_WIFI_ENCRYPT_AES;
					}
					else if(!VK_strncmp("TKIP",pCmd1,VK_strlen(pCmd1)))
					{
						pstApInfo->eEncryptType = DI_WIFI_ENCRYPT_TKIP;
					}
					else
					{
						pstApInfo->eEncryptType = DI_WIFI_ENCRYPT_UNKNOWN;
					}
					ulStep = 6;
				}

				if(pTemp == NULL)
				{
					pTemp = VK_strstr(input,"ciphers(2):");
					if(pTemp)
					{
						VK_sscanf(pTemp,"%s %s %s",pCmd0,pCmd1,pCmd2);
						pstApInfo->eSecureType = DI_WIFI_SECURE_WPA2;
						pstApInfo->eAuthenType = DI_WIFI_AUTHEN_PSK;
						pstApInfo->eEncryptType = DI_WIFI_ENCRYPT_TKIP_AES;
						ulStep = 6;
					}
				}

				if(pTemp == NULL)
				{
					pTemp = VK_strstr(input,"WPS:");
					if(pTemp)
					{
						pstApInfo->bWPS = TRUE;
						ulStep = 0;
					}
				}
				break;

			case 6:
				if(pTemp == NULL)
				{
					pTemp = VK_strstr(input,"WPS:");
					if(pTemp)
					{
						pstApInfo->bWPS = TRUE;
						ulStep = 0;
					}
				}
				break;
			default:
				ulStep = 0;
				break;
		}
	}

	fclose(fp);
	return DRV_OK;

}

static DRV_Error P_WIFI_GetWPS_APStatus(void)
{
	FILE *fp;
	HCHAR input[129];
	HCHAR pCmd0[129];
	HCHAR* pTemp = 0;
	HCHAR	*pLine;
	HBOOL bAPConnect = TRUE;

	fp = fopen("/tmp/wps.txt","r");
	if(fp == NULL)
	{
		PrintError("P_WIFI_GetAPInfo] Err fopen %s\n",strerror( errno ));
		return DRV_ERR;
	}

	VK_MEM_Memset(input, 0x00, 129);
	VK_MEM_Memset(pCmd0, 0x00, 129);

	while(fgets(input,128,fp)!= NULL)
	{
		/* Remove '\n' */
		if((pLine = VK_strchr(input,'\n')) !=NULL)
		{
			*pLine='\0';
		}
		pTemp = NULL;
		pTemp = VK_strstr(input,"No any WPS PIN Enabled AP exist");
		if(pTemp)
		{
			bAPConnect = FALSE;
			break;
		}
	}
	fclose(fp);

	if(bAPConnect == TRUE)
	{
		return DRV_OK;
	}
	else
	{
		return DRV_ERR;
	}
}

static DRV_Error P_WIFI_GetWPSAPInfo(HUINT32 ulIndex)
{
	FILE *fp;
	HCHAR input[129];
	HCHAR pCmd0[129];
	HCHAR* pTemp = 0;
	HCHAR	*pLine;
	HUINT32 ulStep = STEP_WPS_CONNECT;
	HBOOL bConnected = FALSE;
	HINT32 nValue = 0;

	fp = fopen("/tmp/wps.txt","r");
	if(fp == NULL)
	{
		PrintError("P_WIFI_GetAPInfo] Err fopen %s\n",strerror( errno ));
		return DRV_ERR;
	}

	VK_MEM_Memset(input, 0x00, 129);
	VK_MEM_Memset(pCmd0, 0x00, 129);

	while(fgets(input,128,fp)!= NULL)
	{
		/* Remove '\n' */
		if((pLine = VK_strchr(input,'\n')) !=NULL)
		{
			*pLine='\0';
		}
		pTemp = NULL;

		switch(ulStep)
		{
			case STEP_WPS_CONNECT:
				pTemp = VK_strstr(input,"WPS Protocol SUCCEEDED !!");
				if(pTemp)
				{
					ulStep = STEP_WPS_SSID;

					bConnected = TRUE;
				}
				break;

			case STEP_WPS_SSID: /* SSID */
				pTemp = VK_strstr(input,"SSID =");
				if(pTemp)
				{
					ulStep = STEP_WPS_AUTHENTICATION;

					VK_sscanf(pTemp,"SSID = %s",pCmd0);
					VK_snprintf(s_stWiFiInstance.stWiFiInfo[ulIndex].stUserSetAP.aucESSID,sizeof(s_stWiFiInstance.stWiFiInfo[ulIndex].stUserSetAP.aucESSID),"%s",pCmd0);
				}

				break;

			case STEP_WPS_AUTHENTICATION:
				pTemp = VK_strstr(input,"Key Mgmt type is");
				if(pTemp)
				{
					VK_sscanf(pTemp,"Key Mgmt type is %s",pCmd0);
					if(!VK_strncmp("WPA-PSK",pCmd0,VK_strlen(pCmd0)))
					{
						s_stWiFiInstance.stWiFiInfo[ulIndex].stUserSetAP.eSecureType = DI_WIFI_SECURE_WPA;
					}
					else if(!VK_strncmp("WPA2-PSK",pCmd0,VK_strlen(pCmd0)))
					{
						s_stWiFiInstance.stWiFiInfo[ulIndex].stUserSetAP.eSecureType = DI_WIFI_SECURE_WPA2;
					}
					else if(!VK_strncmp("OPEN",pCmd0,VK_strlen(pCmd0)))
					{
						s_stWiFiInstance.stWiFiInfo[ulIndex].stUserSetAP.eSecureType = DI_WIFI_SECURE_OPEN;
					}

					ulStep = STEP_WPS_KEY;
				}
				break;

			case STEP_WPS_KEY:
				pTemp = VK_strstr(input,"Key :");
				if(pTemp)
				{
					ulStep = STEP_WPS_ENCRYPTION;

					VK_memset(s_stWiFiInstance.stWiFiInfo[ulIndex].stUserSetAP.aucKey,0,128);
					if(s_stWiFiInstance.stWiFiInfo[ulIndex].stUserSetAP.eSecureType != DI_WIFI_SECURE_OPEN)
					{
						VK_sscanf(pTemp,"Key : %s",pCmd0);
						VK_snprintf(s_stWiFiInstance.stWiFiInfo[ulIndex].stUserSetAP.aucKey,sizeof(s_stWiFiInstance.stWiFiInfo[ulIndex].stUserSetAP.aucKey),"%s",pCmd0);
					}
				}
				break;

			case STEP_WPS_ENCRYPTION:
				pTemp = VK_strstr(input,"Encryption :");
				if(pTemp)
				{
					VK_sscanf(pTemp,"Encryption : %s",pCmd0);
					if(!VK_strncmp("NONE",pCmd0,VK_strlen(pCmd0)))
					{
						s_stWiFiInstance.stWiFiInfo[ulIndex].stUserSetAP.eEncryptType = DI_WIFI_ENCRYPT_NONE;
						s_stWiFiInstance.stWiFiInfo[ulIndex].stUserSetAP.eAuthenType = DI_WIFI_AUTHEN_OPEN;

					}
					else if(!VK_strncmp("AES",pCmd0,VK_strlen(pCmd0)))
					{
						s_stWiFiInstance.stWiFiInfo[ulIndex].stUserSetAP.eEncryptType = DI_WIFI_ENCRYPT_AES;
						s_stWiFiInstance.stWiFiInfo[ulIndex].stUserSetAP.eAuthenType = DI_WIFI_AUTHEN_PSK;
					}
					else if(!VK_strncmp("TKIP",pCmd0,VK_strlen(pCmd0)))
					{
						s_stWiFiInstance.stWiFiInfo[ulIndex].stUserSetAP.eEncryptType = DI_WIFI_ENCRYPT_TKIP;
						s_stWiFiInstance.stWiFiInfo[ulIndex].stUserSetAP.eAuthenType = DI_WIFI_AUTHEN_PSK;
					}
				}

				break;
			default:
				break;
		}

	}

	fseek(fp,0,SEEK_SET);

	while(fgets(input,128,fp)!= NULL)
	{
		/* Remove '\n' */
		if((pLine = VK_strchr(input,'\n')) !=NULL)
		{
			*pLine='\0';
		}
		pTemp = NULL;

		pTemp = VK_strstr(input,s_stWiFiInstance.stWiFiInfo[ulIndex].stUserSetAP.aucESSID);
		if(pTemp)
		{
			pTemp = VK_strstr(input,"Channel");
			if(pTemp)
			{
				VK_sscanf(pTemp+8/*Channel:*/,"%d",&nValue);
				s_stWiFiInstance.stWiFiInfo[ulIndex].stUserSetAP.ulChannel = nValue;
				break;
			}
		}

	}

	fclose(fp);

	if(bConnected == TRUE)
	{
		return DRV_OK;
	}
	else
	{
		return DRV_ERR;
	}
}

static DRV_Error P_WIFI_RunWlCmdForRate(HUINT32 ulIndex, HINT32 *pValue)
{
	FILE *fp;
	HCHAR input[129];
	HCHAR	*pLine;
	HFLOAT32 fTmpRate = 0;

	UNUSED(ulIndex);

	VK_SYSTEM_Command("wl rate>/tmp/rate.txt");
	fp = fopen("/tmp/rate.txt","r");
	if(fp == NULL)
	{
		PrintError("P_WIFI_RunWlCmdForRate] Err fopen %s\n",strerror( errno ));
		return DRV_ERR;
	}

	VK_MEM_Memset(input, 0x00, 129);
	while(fgets(input,128,fp)!= NULL)
	{
		/* Remove '\n' */
		if((pLine = VK_strchr(input,'\n')) !=NULL)
		{
			*pLine='\0';
		}
		VK_sscanf(input,"%f",&fTmpRate);
		*pValue = (HINT32)fTmpRate*1000000;
	}
	fclose(fp);

	return DRV_OK;
}

static DRV_Error P_WIFI_RunWpaSplctCmdForRate(HUINT32 ulIndex, HINT32 *pValue)
{
	DRV_Error dResult = DRV_OK;
	struct iwreq	iwrequest;

	if(s_nSocketFd < 0)
	{
		PrintError("[%s] ERR socket open(0x%x)!!\n",__func__,s_nSocketFd);
		dResult = DRV_ERR;
		goto ret;
	}

	VK_MEM_Memset(&iwrequest, 0, sizeof(struct iwreq));
	VK_strncpy(iwrequest.ifr_name, s_stWiFiInstance.stWiFiInfo[ulIndex].acIfName, IFNAMSIZ-1);
	if(ioctl(s_nSocketFd, SIOCGIWRATE, &iwrequest) >= 0)
	{
		*pValue = (HINT32)(iwrequest.u.bitrate.value);
	}
	else
	{
		PrintError("[%s] ERR ioctl(SIOCGIWRATE)!!\n",__func__);
		*pValue = 0;
		dResult = DRV_ERR;
		goto ret;

	}

ret:

	return dResult;

}

static DRV_Error P_WIFI_GetRate(HUINT32 ulIndex, HINT32 *pValue)
{
	if(s_eWifiCtlInterface == WIFI_CTL_WL)
	{
		P_WIFI_RunWlCmdForRate(ulIndex, pValue);
	}
	else if(s_eWifiCtlInterface == WIFI_CTL_WPA_SUPPLICANT)
	{
		P_WIFI_RunWpaSplctCmdForRate(ulIndex, pValue);
	}
#if defined(CONFIG_QV940)
	else if(s_eWifiCtlInterface == WIFI_CTL_QCSAPI)
	{
		P_WIFI_RunQcsapiCmdForRate(ulIndex, pValue);
	}
#endif
	else
	{
		PrintError("[%s] Error : s_eWifiCtlInterface %d \n", __FUNCTION__, s_eWifiCtlInterface);
		*pValue = 0;
	}

	return DRV_OK;
}

static DRV_Error P_WIFI_RunWlCmdForRssi(HUINT32 ulIndex, HINT32 *pValue)
{
	FILE *fp;
	HCHAR input[129];
	HCHAR	*pLine;

	UNUSED(ulIndex);

	VK_SYSTEM_Command("wl rssi>/tmp/rssi.txt");
	fp = fopen("/tmp/rssi.txt","r");
	if(fp == NULL)
	{
		PrintError("P_WIFI_RunWlCmdForRssi] Err fopen %s\n",strerror( errno ));
		return DRV_ERR;
	}
	while(fgets(input,128,fp)!= NULL)
	{
		/* Remove '\n' */
		if((pLine = VK_strchr(input,'\n')) !=NULL)
		{
			*pLine='\0';
		}
		VK_sscanf(input,"%d",pValue);

#if defined(CONFIG_PRODUCT_IMAGE_FACTORY)
#else
		*pValue = (90 + *pValue) * 2;
		if(*pValue > 100)
		{
			*pValue = 100;
		}
		else if(*pValue < 0)
		{
			*pValue = 0;
		}
		else
		{
			PrintError("[%s] RSSI = %d \n",__func__, *pValue);
		}
#endif

	}
	VK_MEM_Memset(input, 0x00, 129);
	fclose(fp);
	return DRV_OK;
}

static DRV_Error P_WIFI_RunWlCmdForEachAntRssi(HUINT32 ulIndex, DI_WIFI_RSSI_INFO_t *pRssiInfo)
{
	FILE *fp;
	HCHAR input[129];
	HCHAR searchStr[32];
	HCHAR	*pLine;
	HUINT32 i=0;

	UNUSED(ulIndex);

	/* Get  RSSI */
	VK_SYSTEM_Command("wl rssi>/tmp/rssi.txt");
	fp = fopen("/tmp/rssi.txt","r");
	if(fp == NULL)
	{
		PrintError("P_WIFI_RunWlCmdForRssi] Err fopen %s\n",strerror( errno ));
		return DRV_ERR;
	}

	VK_MEM_Memset(input, 0x00, 129);

	while(fgets(input,128,fp)!= NULL)
	{
		/* Remove '\n' */
		if((pLine = VK_strchr(input,'\n')) !=NULL)
		{
			*pLine='\0';
		}
		VK_sscanf(input,"%d",&pRssiInfo->uiRssi);
		PrintDebug("[P_WIFI_RunWlCmdForRssi] Rssi = %d \n", pRssiInfo->uiRssi);
	}
	fclose(fp);

	/* Get RSSI of each antena & Antena count */
	VK_SYSTEM_Command("wl phy_rssi_ant > /tmp/each_ant_rssi.txt");
	fp = fopen("/tmp/each_ant_rssi.txt","r");
	if(fp == NULL)
	{
		PrintError("P_WIFI_RunWlCmdForEachAntRssi] Err fopen %s\n",strerror( errno ));
		return DRV_ERR;
	}

	VK_MEM_Memset(input, 0x00, 129);

	pLine = fgets(input, 128, fp);
	if(pLine != NULL)
	{
		/* Remove '\n' */
		pLine = NULL;
		if((pLine = VK_strchr(input,'\n')) != NULL)
		{
			*pLine='\0';
		}

		for(i=0; i<DI_WIFI_ANT_MAX; i++)
		{
			pLine = NULL;
			VK_MEM_Memset(searchStr, 0x00, sizeof(searchStr));
			VK_snprintf(searchStr, sizeof(searchStr), "rssi[%d] ", i);

			if((pLine = VK_strstr(input, searchStr)) != NULL)
			{
				VK_sscanf(pLine+VK_strlen(searchStr), "%d", &(pRssiInfo->uiAntRssi[i]));
				PrintDebug("[P_WIFI_RunWlCmdForRssi] Antena[%d] rssi = %d \n",i, pRssiInfo->uiAntRssi[i]);
			}
			else
			{
				pRssiInfo->iAntCount = i;
				PrintDebug("[P_WIFI_RunWlCmdForRssi] Antena Count = %d \n",pRssiInfo->iAntCount);
				break;
			}
		}
	}

	fclose(fp);

	return DRV_OK;
}

static DRV_Error P_WIFI_RunWpaSplctCmdForRssi(HUINT32 ulIndex, HINT32 *pValue)
{
	DRV_Error dResult = DRV_OK;
	struct iwreq			iwrequest;
	struct iw_statistics	stats;

	if(s_nSocketFd < 0)
	{
		PrintError("[%s] ERR socket open(0x%x)!!\n",__func__,s_nSocketFd);
		dResult = DRV_ERR;
		goto ret;
	}

	VK_MEM_Memset(&iwrequest, 0, sizeof(struct iwreq));
	VK_strncpy(iwrequest.ifr_name, s_stWiFiInstance.stWiFiInfo[ulIndex].acIfName, IFNAMSIZ-1);
	iwrequest.u.data.pointer = (caddr_t) &stats;
	iwrequest.u.data.length = sizeof(struct iw_statistics);
	iwrequest.u.data.flags = 1; 	/* Clear updated flag */

	if(ioctl(s_nSocketFd, SIOCGIWSTATS, &iwrequest) >= 0)
	{
		*pValue = (HINT32)stats.qual.qual;
	}
	else
	{
		PrintError("[%s] ERR s_nSocketFd(SIOCGIWSTATS)!!\n",__func__);
		*pValue = 0;
		dResult = DRV_ERR;
		goto ret;
	}

ret:

	return dResult;

}

#if defined (CONFIG_QV940)
#define MAX_QV940_RX_CHAIN	4
static DRV_Error P_WIFI_RunQcsapiCmdForEachAntRssi(DI_WIFI_RSSI_INFO_t *pRssiInfo)
{
	FILE *fp;
	int i;
	char cmdBuf[255], buf[129];

	for( i=0; i<MAX_QV940_RX_CHAIN; i++ )
	{
		VK_snprintf(cmdBuf, sizeof(cmdBuf), "call_qcsapi_sockrpc --host %s get_rssi_by_chain wifi0 %d > %s", IPADDR_QV940, i, QCSAPI_TMP_FILE);
		VK_SYSTEM_Command(cmdBuf);

		fp = fopen(QCSAPI_TMP_FILE, "r");
		if(fp == NULL)
		{
			PrintError("P_WIFI_RunWlCmdForRssi] Err fopen %s\n",strerror( errno ));
			return DRV_ERR;
		}

		fgets(buf, 128, fp);
		sscanf(buf, "%d", &(pRssiInfo->uiAntRssi[i]));
		fclose(fp);
	}
	pRssiInfo->iAntCount = MAX_QV940_RX_CHAIN;

    VK_snprintf(buf, sizeof(buf), "AntRssi(%d): %d, %d, %d, %d",
        pRssiInfo->iAntCount, pRssiInfo->uiAntRssi[0], pRssiInfo->uiAntRssi[1], pRssiInfo->uiAntRssi[2], pRssiInfo->uiAntRssi[3]);
	VK_DBG_Print(buf);

	return DRV_OK;
}

static DRV_Error P_WIFI_GetQcsapiWPSAPInfo(HUINT32 ulIndex)
{
	FILE *fp;
	HCHAR cmdBuf[255], buf[129], wpsStateString[129]; //, pCmd0[129];
	HINT32 wpsState = 0; // , wpsChannel = 0, wpsRSSI = 0;
	HBOOL bConnected = FALSE;

	UNUSED(ulIndex);
	PrintEnter();

	// Check the QV940 WPS status
	VK_snprintf(cmdBuf, sizeof(cmdBuf), "call_qcsapi_sockrpc --host %s get_wps_state wifi0 > %s", IPADDR_QV940, QCSAPI_WPS_TMP_FILE);
	VK_SYSTEM_Command(cmdBuf);

	fp = fopen(QCSAPI_WPS_TMP_FILE, "r");
	if(fp != NULL)
	{
		fgets(buf, 128, fp);
		sscanf(buf, "%d (%s)", &wpsState, wpsStateString);
	}
	else
	{
		PrintError("[%s] Err fopen %s\n", __FUNCTION__, strerror( errno ));
		return DRV_ERR;
	}
	fclose(fp);

	if (wpsState != 2)
	{
		PrintError("[%s] WPS connection failed. (%d-%s)\n", __FUNCTION__, wpsState, wpsStateString);
		return DRV_ERR;
	}
	else
	{
		bConnected = TRUE;
#ifdef DI_QCSAPI_DEBUG // debug message
		VK_DBG_Print("[%s:%d] WPS connection success! (%d-%s)\n", __FUNCTION__, __LINE__, wpsState, wpsStateString);
#endif
	}

	PrintExit();

	if(bConnected == TRUE)
	{
		return DRV_OK;
	}
	else
	{
		return DRV_ERR;
	}
}

static void P_WIFI_RunQcsCmdForCheckConnectStatus(HUINT32 ulIndex, HUINT32 ulMsgTime)
{
	HINT32	nVkErr = 0;
	HUINT32 ulNum = 0;
	WIFI_MSG_t	WiFiMsg;
    FILE *fp;
    char cmdBuf[255], buf[129];
    int assocStatus = 0;

	PrintEnter();

	UNUSED(ulMsgTime);
	UNUSED(ulNum);

	if ((s_stWiFiInstance.stWiFiInfo[ulIndex].eStatus >= WIFI_STATUS_DISCONNECTED) ||
			(s_stWiFiInstance.stWiFiInfo[ulIndex].eDongleStatus != WIFI_DONGLE_INSERTED))
	{
		PrintDebug("\n(*) WIFI_STATUS_MONITOR : Not Connected (Status = %d)\n",s_stWiFiInstance.stWiFiInfo[ulIndex].eStatus);
		goto ret;
	}

	if(s_stWiFiInstance.stWiFiInfo[ulIndex].eStatus == WIFI_STATUS_CONNECTING
		|| s_stWiFiInstance.stWiFiInfo[ulIndex].eStatus == WIFI_STATUS_RECONNECTING)
	{
        /* check the assoc status */
        {
            VK_snprintf(cmdBuf, sizeof(cmdBuf), "call_qcsapi_sockrpc --host %s get_count_assoc  wifi0 > %s",
                IPADDR_QV940, QCSAPI_ASSOC_STATUS_FILE);
        	VK_SYSTEM_Command(cmdBuf);

        	fp = fopen(QCSAPI_ASSOC_STATUS_FILE, "r");
        	if (fp != NULL)
        	{
        		fgets(buf, 128, fp);
        		VK_sscanf(buf, "%d", &assocStatus);
        		fclose(fp);

        	    if (assocStatus == 1)
                {
                    s_stWiFiInstance.stWiFiInfo[ulIndex].eStatus = WIFI_STATUS_CONNECTED;
                }
                else
                {
                    PrintDebug("\n(*) WIFI_STATUS_MONITOR : Not Connected (QV940 assocStatus = %d)\n", assocStatus);
                    goto ret;
                }
        	}
        	else
            {
        		PrintError("[%s] Err fopen %s\n", __FUNCTION__, strerror( errno ));
        		return;
            }

        	VK_TASK_Sleep(1000);
        }

		/*In case of the WPS type is PBC or PIN, We have to check AP Information*/
		if( s_stWiFiInstance.stWiFiInfo[ulIndex].eWPS != DI_WIFI_WPS_NONE)
		{
			/* WPS 로 연결 된 경우 AP와 연결 된 정보가 필요하다. */
			/* WPS 에서 실제 연결되지 않은 경우는 Retry로 변경..*/
			if(P_WIFI_GetQcsapiWPSAPInfo(ulIndex) != DRV_OK)
			{
				goto ret;
			}
		}

		if(s_stWiFiInstance.ulDisConnectTimerId)
		{
			VK_TIMER_Cancel(s_stWiFiInstance.ulDisConnectTimerId);
			s_stWiFiInstance.ulDisConnectTimerId = (HUINT32)NULL;
		}

    	/* Send WIFI_EVT_CONNECT_OK to Task */
    	WiFiMsg.ulIndex = ulIndex;
    	WiFiMsg.eMsg = WIFI_EVT_CONNECT_OK;
    	nVkErr = VK_MSG_SendTimeout(s_stWiFiInstance.ulMsg, &WiFiMsg, sizeof(WIFI_MSG_t),0);
    	if(nVkErr)
    	{
    		PrintError("[%s] VK_MSG_Send Err(0x%x)!!\n",__func__,nVkErr);
    	}
    	goto ret;
	}
	else if(s_stWiFiInstance.stWiFiInfo[ulIndex].eStatus == WIFI_STATUS_CONNECTED)
	{
#if 0 /* Currently, Don't need this check and DI_WIFI_NOTIFY_EVT_CONNECT_FAIL event */
		VK_snprintf(cmdBuf, sizeof(cmdBuf), "call_qcsapi_sockrpc --host %s get_count_assoc  wifi0 > %s",
				IPADDR_QV940, QCSAPI_ASSOC_STATUS_FILE);
		VK_SYSTEM_Command(cmdBuf);

		fp = fopen(QCSAPI_ASSOC_STATUS_FILE, "r");
		if (fp != NULL)
		{
			fgets(buf, 128, fp);
			VK_sscanf(buf, "%d", &assocStatus);
			fclose(fp);

			if (assocStatus == 0)
			{
				s_stWiFiInstance.stWiFiInfo[ulIndex].eStatus = WIFI_STATUS_DISCONNECTED;
				/* Send WIFI_EVT_CONNECT_OK to Task */
				WiFiMsg.ulIndex = ulIndex;
				WiFiMsg.eMsg = WIFI_EVT_CONNECT_FAIL;
				nVkErr = VK_MSG_SendTimeout(s_stWiFiInstance.ulMsg, &WiFiMsg, sizeof(WIFI_MSG_t),0);
				if(nVkErr)
				{
					PrintError("[%s] VK_MSG_Send Err(0x%x)!!\n",__func__,nVkErr);
				}

				PrintDebug("\n(*) WIFI_STATUS_MONITOR : Disconnected (QV940 assocStatus = %d)\n", assocStatus);
				goto ret;
			}
		}
		else
		{
			PrintError("[%s] Err fopen %s\n", __FUNCTION__, strerror( errno ));
			goto ret;
		}

		PrintDebug("\n(*) WIFI_STATUS_MONITOR : WIFI_STATUS_CONNECTED (MAC : %d) \n", ulNum);
		goto ret;
#endif
	}
ret :
	PrintExit();

	return;
}

static void P_WIFI_RunQcsapiCmdForConnectWpsToAp(HUINT32 ulIndex, WIFI_EVENT_e eConnectType)
{
	FILE *fp;
	char qcsapiStatus[129], cmdBuf[255], buf[129];
	int assocStatus = 0;

	PrintEnter();

	/* Ready to connect */
	s_stWiFiInstance.stWiFiInfo[ulIndex].eStatus = WIFI_STATUS_CONNECTING;
	s_stWiFiInstance.stWiFiInfo[ulIndex].eWPS = DI_WIFI_WPS_NONE;

	VK_MEM_Memset(cmdBuf, 0x00, sizeof(cmdBuf));

	VK_snprintf(cmdBuf, sizeof(cmdBuf), "call_qcsapi_sockrpc --host %s get_count_assoc  wifi0 > %s",
		IPADDR_QV940, QCSAPI_ASSOC_STATUS_FILE);
	VK_SYSTEM_Command(cmdBuf);

	fp = fopen(QCSAPI_ASSOC_STATUS_FILE, "r");
	if (fp != NULL)
	{
		fgets(buf, 128, fp);
		VK_sscanf(buf, "%d", &assocStatus);
		fclose(fp);

		// debug message
		//VK_DBG_Print("  %s(%d) assocStatus=%d\n", __FUNCTION__, __LINE__, assocStatus);

		if (assocStatus)
		{
			/* disassociate */
			VK_snprintf(cmdBuf, sizeof(cmdBuf), "call_qcsapi_sockrpc --host %s disassociate wifi0 > %s", IPADDR_QV940, QCSAPI_TMP_FILE);
			VK_SYSTEM_Command(cmdBuf);
		}
	}
	else
	{
		PrintError("[%s] Err fopen %s\n", __FUNCTION__, strerror( errno ));
		return;
	}

	if (eConnectType == WIFI_EVT_WPS_PBC_START)
	{
		s_stWiFiInstance.stWiFiInfo[ulIndex].eWPS = DI_WIFI_WPS_PBC;
		VK_snprintf(cmdBuf, sizeof(cmdBuf), "call_qcsapi_sockrpc --host %s enrollee_report_button_press wifi0 > %s", IPADDR_QV940, QCSAPI_WPS_TMP_FILE);
		VK_SYSTEM_Command(cmdBuf);
		VK_TASK_Sleep(1000);

		fp = fopen(QCSAPI_WPS_TMP_FILE, "r");
		if (fp != NULL)
		{
			fgets(buf, 128, fp);
			VK_sscanf(buf, "%s", &qcsapiStatus[0]);
			fclose(fp);

			if (VK_strcmp("complete", qcsapiStatus) != 0)
	    	{
	        	PrintError("[%s] WPS(PBC) QCSAPI error!\n", __FUNCTION__);
	        	return;
		    }
		    // debug message
	    	//VK_DBG_Print("  %s(qcsapiStatus='%s')\n", __FUNCTION__, qcsapiStatus);
		}
		else
	    {
			PrintError("[%s] Err fopen %s\n", __FUNCTION__, strerror( errno ));
			return;
	    }
	}
	else if (eConnectType == WIFI_EVT_WPS_PIN_START)
	{
		s_stWiFiInstance.stWiFiInfo[ulIndex].eWPS = DI_WIFI_WPS_PIN;
		VK_snprintf(cmdBuf, sizeof(cmdBuf), "call_qcsapi_sockrpc --host %s enrollee_report_pin wifi0 %s > %s",
				IPADDR_QV940, s_stWiFiInstance.stWiFiInfo[ulIndex].stUserSetAP.aucKey, QCSAPI_WPS_PIN_CODE_FILE);
		VK_SYSTEM_Command(cmdBuf);
		VK_TASK_Sleep(1000);

		fp = fopen(QCSAPI_WPS_PIN_CODE_FILE, "r");
		if (fp != NULL)
		{
			fgets(buf, 128, fp);
			VK_sscanf(buf, "%s", &qcsapiStatus[0]);
			fclose(fp);

			if (VK_strcmp("complete", qcsapiStatus) != 0)
	    	{
	        	PrintError("[%s] WPS(PIN) QCSAPI error!\n", __FUNCTION__);
	        	return;
		    }
		    // debug message
	    	//VK_DBG_Print("  %s(qcsapiStatus='%s')\n", __FUNCTION__, qcsapiStatus);
		}
		else
	    {
			PrintError("[%s] Err fopen %s\n", __FUNCTION__, strerror( errno ));
			return;
	    }
	}
	else
	{
		PrintError("[%s] Error : not supported connection type \n", __FUNCTION__);
		return;
	}

	PrintExit();

	return;
}
#endif

static DRV_Error P_WIFI_GetRssi(HUINT32 ulIndex, HINT32 *pValue)
{
	if(s_eWifiCtlInterface == WIFI_CTL_WL)
	{
		P_WIFI_RunWlCmdForRssi(ulIndex, pValue);
	}
	else if(s_eWifiCtlInterface == WIFI_CTL_WPA_SUPPLICANT)
	{
		P_WIFI_RunWpaSplctCmdForRssi(ulIndex, pValue);
	}
#if defined(CONFIG_QV940)
	else if(s_eWifiCtlInterface == WIFI_CTL_QCSAPI)
	{
		P_WIFI_RunQcsapiCmdForRssi(ulIndex, pValue);
	}
#endif
	else
	{
		PrintError("[%s] Error : s_eWifiCtlInterface %d \n", __FUNCTION__, s_eWifiCtlInterface);
		*pValue = 0;
	}

	return DRV_OK;
}

#if defined (CONFIG_QV940)
static DRV_Error P_WIFI_RunQcsapiCmdForScanAp(HUINT32 ulIndex)
{
	FILE *fp;
	char qcsapiStatus[129], cmdBuf[255], buf[129];
	int qcsapiAPcounter = 0, scanStatus = 0, i = 0;
    // AP info
    char qcs_ap_name_SSID[33];
    unsigned char qcs_mac_addr_string[128];
    int qcs_ap_RSSI=0, qcs_ap_protocol=0, qcs_ap_authentication_mode=0, qcs_ap_encryption_modes=0, i_tmp=0, ulLutIdx=0, qcs_ap_channel=0;
    unsigned int qcs_ap_flags=0;
    HUINT32		ulMac[6];

	PrintEnter();

	/* Scan Start */
    VK_snprintf(cmdBuf, sizeof(cmdBuf), "call_qcsapi_sockrpc --host %s start_scan wifi0 > %s", IPADDR_QV940, QCSAPI_START_SCAN_FILE);
	VK_SYSTEM_Command(cmdBuf);
	VK_TASK_Sleep(1000);
#if 0 // not used
    VK_snprintf(cmdBuf, sizeof(cmdBuf), "call_qcsapi_sockrpc --host %s stop_scan wifi0", IPADDR_QV940);
	VK_SYSTEM_Command(cmdBuf);
	VK_TASK_Sleep(500);
#endif

	fp = fopen(QCSAPI_START_SCAN_FILE, "r");
	if (fp != NULL)
	{
		fgets(buf, 128, fp);
		VK_sscanf(buf, "%s", &qcsapiStatus[0]);
		fclose(fp);

		if (VK_strcmp("complete", qcsapiStatus) != 0)
	    {
	        PrintError("[%s] QCSAPI start scan error!\n", __FUNCTION__);
	        return DRV_ERR;
	    }
	    // debug message
	    //VK_DBG_Print("  %s(qcsapiStatus='%s')\n", __FUNCTION__, qcsapiStatus);
	}
	else
    {
		PrintError("[%s] Err fopen %s\n", __FUNCTION__, strerror( errno ));
		return DRV_ERR;
    }
    VK_TASK_Sleep(1000);

    /* check the scan status */
    while (scanStatus == 0)
    {
	    // debug message
	    //VK_DBG_Print("  %s(scanStatus=%d)\n", __FUNCTION__, scanStatus);

        VK_snprintf(cmdBuf, sizeof(cmdBuf), "call_qcsapi_sockrpc --host %s get_scanstatus wifi0 > %s", IPADDR_QV940, QCSAPI_SCAN_STATUS_FILE);
    	VK_SYSTEM_Command(cmdBuf);

    	fp = fopen(QCSAPI_SCAN_STATUS_FILE, "r");
    	if (fp != NULL)
    	{
    		fgets(buf, 128, fp);
    		VK_sscanf(buf, "%d", &scanStatus);
    		fclose(fp);

    	    // debug message
    	    //VK_DBG_Print("  %s(scanStatus=%d)\n", __FUNCTION__, scanStatus);
    	}
    	else
        {
    		PrintError("[%s] Err fopen %s\n", __FUNCTION__, strerror( errno ));
    		return DRV_ERR;
        }

    	VK_TASK_Sleep(100);
    }

    /* get the count of APs */
	VK_snprintf(cmdBuf, sizeof(cmdBuf), "call_qcsapi_sockrpc --host %s get_results_AP_scan wifi0 > %s", IPADDR_QV940, QCSAPI_AP_COUNTER_FILE);
	VK_SYSTEM_Command(cmdBuf);
	VK_TASK_Sleep(500);

	fp = fopen(QCSAPI_AP_COUNTER_FILE, "r");
	if (fp != NULL)
	{
		fgets(buf, 128, fp);
		VK_sscanf(buf, "%d", &qcsapiAPcounter);
		fclose(fp);

		if (qcsapiAPcounter < 0)
	    {
	        PrintError("[%s] QCSAPI scaned APs counter error!\n", __FUNCTION__);
	        return DRV_ERR;
	    }
#ifdef DI_QCSAPI_DEBUG // debug message
	    VK_DBG_Print("  %s(qcsapiAPcounter=%d)\n", __FUNCTION__, qcsapiAPcounter);
#endif
	}
	else
    {
		PrintError("[%s] Err fopen %s\n", __FUNCTION__, strerror( errno ));
		return DRV_ERR;
    }

    /* get AP list */
    for (i = 0; i < qcsapiAPcounter; i++)
    {
        if (i == 0)
        {
            VK_snprintf(cmdBuf, sizeof(cmdBuf), "call_qcsapi_sockrpc --host %s get_properties_AP wifi0 %d > %s", IPADDR_QV940, i, QCSAPI_AP_LIST_FILE);
        }
        else
        {
            VK_snprintf(cmdBuf, sizeof(cmdBuf), "call_qcsapi_sockrpc --host %s get_properties_AP wifi0 %d >> %s", IPADDR_QV940, i, QCSAPI_AP_LIST_FILE);
        }
    	VK_SYSTEM_Command(cmdBuf);
    	VK_TASK_Sleep(100);
    }

    VK_TASK_Sleep(500);

	fp = fopen(QCSAPI_AP_LIST_FILE, "r");
	if(fp == NULL)
	{
		PrintError("[%s] Err fopen %s\n", __FUNCTION__, strerror( errno ));
		return DRV_ERR;
	}
	i = 0;
	while(fgets(buf, 128, fp) != NULL)
	{
		VK_sscanf(buf, "\"%s %x:%x:%x:%x:%x:%x %d %d %x %d %d %d",
		    &qcs_ap_name_SSID[0],
		    &ulMac[0], &ulMac[1], &ulMac[2], &ulMac[3], &ulMac[4], &ulMac[5],
		    &qcs_ap_channel,
            &qcs_ap_RSSI,
            &qcs_ap_flags,
            &qcs_ap_protocol,
            &qcs_ap_authentication_mode,
            &qcs_ap_encryption_modes );

#if 0 /* Remove below code, because we can use the TKIP in EU region. */
        /* Remove TKIP SSIDs, because Not support TKIP. */
        if ((qcs_ap_encryption_modes == 1) || (qcs_ap_encryption_modes == 3))
        {
            continue;
        }
#endif

        i_tmp = strlen(qcs_ap_name_SSID);
        qcs_ap_name_SSID[i_tmp - 1] = '\0';

        VK_snprintf(qcs_mac_addr_string, sizeof(qcs_mac_addr_string), "%02x:%02x:%02x:%02x:%02x:%02x", ulMac[0], ulMac[1], ulMac[2], ulMac[3], ulMac[4], ulMac[5]);

        VK_TASK_Sleep(100);

        VK_MEM_Memset(&s_stWiFiInstance.stWiFiInfo[ulIndex].astAPList[i],0,sizeof(DI_WIFI_INFO_t));
        VK_snprintf(s_stWiFiInstance.stWiFiInfo[ulIndex].astAPList[i].aucESSID, sizeof(qcs_ap_name_SSID), qcs_ap_name_SSID);
		s_stWiFiInstance.stWiFiInfo[ulIndex].astAPList[i].aucMAC[0] = ulMac[0];
		s_stWiFiInstance.stWiFiInfo[ulIndex].astAPList[i].aucMAC[1] = ulMac[1];
		s_stWiFiInstance.stWiFiInfo[ulIndex].astAPList[i].aucMAC[2] = ulMac[2];
		s_stWiFiInstance.stWiFiInfo[ulIndex].astAPList[i].aucMAC[3] = ulMac[3];
		s_stWiFiInstance.stWiFiInfo[ulIndex].astAPList[i].aucMAC[4] = ulMac[4];
		s_stWiFiInstance.stWiFiInfo[ulIndex].astAPList[i].aucMAC[5] = ulMac[5];

        s_stWiFiInstance.stWiFiInfo[ulIndex].astAPList[i].ulChannel = qcs_ap_channel;

        if (qcs_ap_RSSI == 70)
        {
            s_stWiFiInstance.stWiFiInfo[ulIndex].astAPList[i].nWaveStrength = 100;
        }
        else
        {
            double dTmpRssi, dTmpResult;
            dTmpRssi = (double)qcs_ap_RSSI;
            dTmpResult = (dTmpRssi / 68.0 * 100.0);
            s_stWiFiInstance.stWiFiInfo[ulIndex].astAPList[i].nWaveStrength = abs(dTmpResult);
        }

		for (ulLutIdx = 0 ; ulLutIdx < WIFI_CHANNEL_LUT_SIZE ; ulLutIdx++)
		{
			if (g_stWifiChLut[ulLutIdx].ulChannelNo == (unsigned int)qcs_ap_channel)
			{
				s_stWiFiInstance.stWiFiInfo[ulIndex].astAPList[i].ulFreq = g_stWifiChLut[ulLutIdx].ulFrequency;
			}
		}

        if (qcs_ap_protocol == 0) s_stWiFiInstance.stWiFiInfo[ulIndex].astAPList[i].eSecureType = DI_WIFI_SECURE_OPEN;
        else if (qcs_ap_protocol == 1) s_stWiFiInstance.stWiFiInfo[ulIndex].astAPList[i].eSecureType = DI_WIFI_SECURE_WPA;
        else if (qcs_ap_protocol == 2) s_stWiFiInstance.stWiFiInfo[ulIndex].astAPList[i].eSecureType = DI_WIFI_SECURE_WPA2;
        else if (qcs_ap_protocol == 3) s_stWiFiInstance.stWiFiInfo[ulIndex].astAPList[i].eSecureType = DI_WIFI_SECURE_WPA_WPA2;
        // s_stWiFiInstance.stWiFiInfo[ulIndex].astAPList[i].eSecureType = DI_WIFI_SECURE_WEP; - Not support on QV940

        if (qcs_ap_authentication_mode == 1) s_stWiFiInstance.stWiFiInfo[ulIndex].astAPList[i].eAuthenType = DI_WIFI_AUTHEN_PSK;
        else if (qcs_ap_authentication_mode == 2) s_stWiFiInstance.stWiFiInfo[ulIndex].astAPList[i].eAuthenType = DI_WIFI_AUTHEN_OPEN;

        if (qcs_ap_encryption_modes == 0)
            s_stWiFiInstance.stWiFiInfo[ulIndex].astAPList[i].eEncryptType = DI_WIFI_ENCRYPT_NONE;
        else if (qcs_ap_encryption_modes == 1)
            s_stWiFiInstance.stWiFiInfo[ulIndex].astAPList[i].eEncryptType = DI_WIFI_ENCRYPT_TKIP;
        else if (qcs_ap_encryption_modes == 2)
            s_stWiFiInstance.stWiFiInfo[ulIndex].astAPList[i].eEncryptType = DI_WIFI_ENCRYPT_AES;
        else if (qcs_ap_encryption_modes == 3)
            s_stWiFiInstance.stWiFiInfo[ulIndex].astAPList[i].eEncryptType = DI_WIFI_ENCRYPT_TKIP_AES;
        else
            s_stWiFiInstance.stWiFiInfo[ulIndex].astAPList[i].eEncryptType = DI_WIFI_ENCRYPT_UNKNOWN;
        //s_stWiFiInstance.stWiFiInfo[ulIndex].astAPList[i].eEncryptType = DI_WIFI_ENCRYPT_WEP_64; - Not support on QV940

        ++i;
#ifdef DI_QCSAPI_DEBUG // debug message
	    VK_DBG_Print("%3d  \"%25s\" %s %d %d %x %d %d %d\n", i, qcs_ap_name_SSID, qcs_mac_addr_string,
	        qcs_ap_channel, qcs_ap_RSSI, qcs_ap_flags, qcs_ap_protocol, qcs_ap_authentication_mode, qcs_ap_encryption_modes);
#endif
	}
	s_stWiFiInstance.stWiFiInfo[ulIndex].ulNumOfAPs = i;
	fclose(fp);

	PrintExit();

	return DRV_OK;
}
#endif

static DRV_Error P_WIFI_RunWlCmdForScanAp(HUINT32 ulIndex)
{
	FILE *fp = NULL;
	HCHAR* pTemp = 0;
	HCHAR input[129];
	HCHAR pCmd0[129];
	HCHAR pCmd1[129];
	HCHAR pCmd2[129];

	HUINT32 ulMac[6];
	HINT32 nValue = 0;
	HUINT32 ulStep = 0;
	HINT32 nCnt = -1;
	HUINT32 ulLutIdx = 0;

	unsigned long ulScanWaitTime = 0;

	ulScanWaitTime = (SCAN_TIMEOUT/1000);

	/* Scan Start */
#if defined(CONFIG_BCM43236) || defined(CONFIG_BCM43236_MFG) || defined(CONFIG_BCM43567)
#if defined(CONFIG_BCM43236_BISON) || defined(CONFIG_BCM43567) || defined(CONFIG_WIFI_AARDVARK)
	VK_SYSTEM_Command("wl escanresults >/tmp/scanresults.txt");
#elif defined(CONFIG_BCM43242)
	VK_SYSTEM_Command("wl escanresults >/tmp/scanresults.txt");
#else
{
	HINT32 nRet = 0;
	long int ulFileSize = 0;
	char *pBuf = NULL;
	size_t readSize = 0;

	VK_SYSTEM_Command("wl iscan_s");
	VK_TASK_Sleep(ulScanWaitTime);

	VK_SYSTEM_Command("wl iscanresults > /tmp/scanresults.txt");
	VK_TASK_Sleep(100);

	fp = fopen("/tmp/scanresults.txt","r");
	if(fp == NULL)
	{
		PrintError("[P_WIFI_RunWlCmdForScanAp] Err fopen %s\n",strerror( errno ));
		return DRV_ERR;
	}

	while(1)
	{
		/* Get file size */
		nRet = fseek(fp, 0, SEEK_END);
		if(nRet != 0)
		{
			PrintError("[%s] Err fseek %s\n",__FUNCTION__, strerror( errno ));
			fclose(fp);
			return DRV_ERR;
		}

		ulFileSize = ftell(fp);
		if(ulFileSize <= 0)
		{
			PrintError("[%s] ulFileSize = %d \n",__FUNCTION__, ulFileSize);
			fclose(fp);
			return DRV_ERR;
		}

		nRet = fseek(fp, 0, SEEK_SET);
		if(nRet != 0)
		{
			PrintError("[%s] Err fseek %s\n",__FUNCTION__, strerror( errno ));
			fclose(fp);
			return DRV_ERR;
		}

		pBuf = DD_MEM_Alloc(ulFileSize+1);
		if(pBuf == NULL)
		{
			PrintError("[%s] pBuf is null \n", __FUNCTION__);
			fclose(fp);
			return DRV_ERR;
		}
		VK_MEM_Memset(pBuf, 0x0, ulFileSize+1);

		/* Read AP info from file */
		readSize = fread(pBuf, 1, ulFileSize, fp);
		if (readSize != (size_t)ulFileSize)
		{
			PrintError("[%s] Err fread %s\n",__FUNCTION__, strerror( errno ));
			DD_MEM_Free(pBuf);
			fclose(fp);
			return DRV_ERR;
		}

		pBuf[readSize] = '\0';

		/* Check whether complete or not */
		if(VK_strstr(pBuf,"iscanresults complete"))
		{
			/* complete */
			DD_MEM_Free(pBuf);
			fclose(fp);
			break;
		}
		else
		{
			/* update AP info */
			DD_MEM_Free(pBuf);
			VK_SYSTEM_Command("wl iscanresults >> /tmp/scanresults.txt");
			VK_TASK_Sleep(100);
		}
	}
}
#endif
#else
	PrintDebug("[%s]_%d %s\n",__func__,__LINE__,"wl scan");
	VK_SYSTEM_Command("wl scan");
	VK_TASK_Sleep(ulScanWaitTime);

	/* Get scan results */
	PrintDebug("[%s]_%d %s\n",__func__,__LINE__,"wl scanresults");
	VK_SYSTEM_Command("wl scanresults>/tmp/scanresults.txt");
	VK_TASK_Sleep(100);
#endif

	fp = fopen("/tmp/scanresults.txt","r");
	if(fp == NULL)
	{
		PrintError("[P_WIFI_RunWlCmdForScanAp] Err fopen %s\n",strerror( errno ));
		return DRV_ERR;
	}

	VK_MEM_Memset(input, 0x00, 129);
	VK_MEM_Memset(pCmd0, 0x00, 129);
	VK_MEM_Memset(pCmd1, 0x00, 129);
	VK_MEM_Memset(pCmd2, 0x00, 129);

	while(fgets(input,128,fp)!= NULL)
	{
		pTemp = NULL;
		switch(ulStep)
		{
			case 0:/* SSID: "SWSONG_Network_1" */
				pTemp = VK_strstr(input,"SSID:");
				if(pTemp)
				{
					VK_sscanf(pTemp,"%s \"%s",pCmd0,pCmd1);
					/*== SSID */
					ulStep = 1;
					nCnt++;
					if(nCnt >= WIFI_AP_MAX)
					{
						nCnt--;
						break;
					}
					VK_MEM_Memset(&s_stWiFiInstance.stWiFiInfo[ulIndex].astAPList[nCnt],0,sizeof(DI_WIFI_INFO_t));
					if(!((pCmd1[0] == 0x5C)&&(pCmd1[1] == 0x78)&&(pCmd1[2] == 0x30)&&(pCmd1[3] == 0x30)))
					{
						HUINT8 aucTempEssid[33] = {0,};
						HCHAR	*p = pTemp;
						HINT32	i = 0;

						while (*p++ != '\"')
						{
							/* move to the start pointer of ESSID string */
						}

						while (1)
						{
							if (*p != '\n')
							{
								aucTempEssid[i++] = *p++;
							}
							else
							{
								aucTempEssid[i-1] = '\0';
								break;
							}
						}

						VK_strncpy(s_stWiFiInstance.stWiFiInfo[ulIndex].astAPList[nCnt].aucESSID, aucTempEssid, VK_strlen(aucTempEssid));
					}
					s_stWiFiInstance.stWiFiInfo[ulIndex].astAPList[nCnt].ulFreq = 2400;
				}
				break;

			case 1:/*Mode: Managed   RSSI: -48 dBm   SNR: 0 dB 	  noise: -69 dBm  Flags: RSSI on-channel  Channel: 3*/
				ulStep = 0;
				pTemp = VK_strstr(input,"Mode:");
				if(pTemp)
				{
					ulStep = 2;
					VK_sscanf(pTemp,"%s %s %s %d",pCmd0,pCmd1,pCmd2,&nValue);

					nValue = (90 + nValue) * 2;
					if(nValue > 100) nValue = 100;
					else if(nValue < 0) nValue = 0;

					s_stWiFiInstance.stWiFiInfo[ulIndex].astAPList[nCnt].nWaveStrength = nValue;
					if(!VK_strncmp("Managed",pCmd1,VK_strlen(pCmd1)))
					{
						s_stWiFiInstance.stWiFiInfo[ulIndex].astAPList[nCnt].eOpMode = DI_WIFI_OP_MODE_MANAGED;
						/*== Mode */
					}
					else
					{
						s_stWiFiInstance.stWiFiInfo[ulIndex].astAPList[nCnt].eOpMode = DI_WIFI_OP_MODE_UNKNOWN;
					}
					pTemp = VK_strstr(input,"Channel:");
					if(pTemp)
					{
						VK_sscanf(pTemp,"%s %d",pCmd0,&nValue);
						/*== Channel */
						s_stWiFiInstance.stWiFiInfo[ulIndex].astAPList[nCnt].ulChannel = nValue;
						for (ulLutIdx = 0 ; ulLutIdx < WIFI_CHANNEL_LUT_SIZE ; ulLutIdx++)
						{
							if (g_stWifiChLut[ulLutIdx].ulChannelNo == s_stWiFiInstance.stWiFiInfo[ulIndex].astAPList[nCnt].ulChannel)
							{
								s_stWiFiInstance.stWiFiInfo[ulIndex].astAPList[nCnt].ulFreq = g_stWifiChLut[ulLutIdx].ulFrequency;
							}
						}
					}
				}
				break;

			case 2:/*BSSID: 00:18:E7:EA:6A:43        Capability: ESS WEP ShortPre ShortSlot*/
				ulStep = 0;
				pTemp = VK_strstr(input,"BSSID:");
				if(pTemp)
				{
					ulStep = 3;
					VK_sscanf(input,"%s %x:%x:%x:%x:%x:%x",pCmd0,&ulMac[0],&ulMac[1],&ulMac[2],&ulMac[3],&ulMac[4],&ulMac[5]);
					/*== BSSID */
					s_stWiFiInstance.stWiFiInfo[ulIndex].astAPList[nCnt].aucMAC[0] = ulMac[0];
					s_stWiFiInstance.stWiFiInfo[ulIndex].astAPList[nCnt].aucMAC[1] = ulMac[1];
					s_stWiFiInstance.stWiFiInfo[ulIndex].astAPList[nCnt].aucMAC[2] = ulMac[2];
					s_stWiFiInstance.stWiFiInfo[ulIndex].astAPList[nCnt].aucMAC[3] = ulMac[3];
					s_stWiFiInstance.stWiFiInfo[ulIndex].astAPList[nCnt].aucMAC[4] = ulMac[4];
					s_stWiFiInstance.stWiFiInfo[ulIndex].astAPList[nCnt].aucMAC[5] = ulMac[5];

					pTemp = VK_strstr(input,"WEP");
					if(pTemp)
					{
						s_stWiFiInstance.stWiFiInfo[ulIndex].astAPList[nCnt].eSecureType = DI_WIFI_SECURE_WEP;
						s_stWiFiInstance.stWiFiInfo[ulIndex].astAPList[nCnt].eAuthenType = DI_WIFI_AUTHEN_OPEN;
						s_stWiFiInstance.stWiFiInfo[ulIndex].astAPList[nCnt].eEncryptType = DI_WIFI_ENCRYPT_WEP_64;
					}
					else
					{
						s_stWiFiInstance.stWiFiInfo[ulIndex].astAPList[nCnt].eSecureType = DI_WIFI_SECURE_OPEN;
						s_stWiFiInstance.stWiFiInfo[ulIndex].astAPList[nCnt].eAuthenType = DI_WIFI_AUTHEN_OPEN;
						s_stWiFiInstance.stWiFiInfo[ulIndex].astAPList[nCnt].eEncryptType = DI_WIFI_ENCRYPT_NONE;
						ulStep = 6;
					}
				}
				break;

			case 3:/*WPA: or RSN:*/
				/* WPA 인지 RSN인지 선택 */
				pTemp = VK_strstr(input,"WPA:");
				if(pTemp)
				{
					ulStep = 4;
				}
				else
				{
#if defined(CONFIG_BCM43236_BISON) || defined(CONFIG_BCM43567)
					pTemp = VK_strstr(input,"RSN (WPA2)");
#else
					pTemp = VK_strstr(input,"RSN:");
#endif
					if(pTemp)
					{
						ulStep = 5;
					}
				}

				if(pTemp == NULL)
				{
					pTemp = VK_strstr(input,"WPS:");
					if(pTemp)
					{
						s_stWiFiInstance.stWiFiInfo[ulIndex].astAPList[nCnt].bWPS = TRUE;
						ulStep = 0;
					}
				}

				if(pTemp == NULL)
				{
					pTemp = VK_strstr(input,"SSID:");
					if(pTemp)
					{
						nCnt++;
						if(nCnt >= WIFI_AP_MAX)
						{
							nCnt--;
							break;
						}
						VK_sscanf(pTemp,"%s \"%s",pCmd0,pCmd1);
						VK_MEM_Memset(&s_stWiFiInstance.stWiFiInfo[ulIndex].astAPList[nCnt],0,sizeof(DI_WIFI_INFO_t));
						if(!((pCmd1[0] == 0x5C)&&(pCmd1[1] == 0x78)&&(pCmd1[2] == 0x30)&&(pCmd1[3] == 0x30)))
						{
						HUINT8 aucTempEssid[33] = {0,};
						HCHAR	*p = pTemp;
						HINT32	i = 0;

						while (*p++ != '\"')
						{
							/* move to the start pointer of ESSID string */
						}

						while (1)
						{
							if (*p != '\n')
							{
								aucTempEssid[i++] = *p++;
							}
							else
							{
								aucTempEssid[i-1] = '\0';
								break;
							}
						}

						VK_snprintf(s_stWiFiInstance.stWiFiInfo[ulIndex].astAPList[nCnt].aucESSID,sizeof(aucTempEssid),aucTempEssid);
						}
						s_stWiFiInstance.stWiFiInfo[ulIndex].astAPList[nCnt].ulFreq = 0;

						/*== SSID */
						ulStep = 1;
					}
				}
				break;

			case 4: /* WPA */
				/*unicast ciphers(1): AES-CCMP */
				/*unicast ciphers(1): TKIP */
				/*unicast ciphers(2): TKIP AES-CCMP */
				pTemp = VK_strstr(input,"ciphers(1):");
				if(pTemp)
				{
					VK_sscanf(pTemp,"%s %s",pCmd0,pCmd1);
					s_stWiFiInstance.stWiFiInfo[ulIndex].astAPList[nCnt].eSecureType = DI_WIFI_SECURE_WPA;
					s_stWiFiInstance.stWiFiInfo[ulIndex].astAPList[nCnt].eAuthenType = DI_WIFI_AUTHEN_PSK;
					if(!VK_strncmp("AES-CCMP",pCmd1,VK_strlen(pCmd1)))
					{
						s_stWiFiInstance.stWiFiInfo[ulIndex].astAPList[nCnt].eEncryptType = DI_WIFI_ENCRYPT_AES;
					}
					else if(!VK_strncmp("TKIP",pCmd1,VK_strlen(pCmd1)))
					{
						s_stWiFiInstance.stWiFiInfo[ulIndex].astAPList[nCnt].eEncryptType = DI_WIFI_ENCRYPT_TKIP;
					}
					else
					{
						s_stWiFiInstance.stWiFiInfo[ulIndex].astAPList[nCnt].eEncryptType = DI_WIFI_ENCRYPT_UNKNOWN;
					}
					ulStep = 5;
				}

				if(pTemp == NULL)
				{
					pTemp = VK_strstr(input,"ciphers(2):");
					if(pTemp)
					{
						VK_sscanf(pTemp,"%s %s %s",pCmd0,pCmd1,pCmd2);
						s_stWiFiInstance.stWiFiInfo[ulIndex].astAPList[nCnt].eSecureType = DI_WIFI_SECURE_WPA;
						s_stWiFiInstance.stWiFiInfo[ulIndex].astAPList[nCnt].eAuthenType = DI_WIFI_AUTHEN_PSK;
						s_stWiFiInstance.stWiFiInfo[ulIndex].astAPList[nCnt].eEncryptType = DI_WIFI_ENCRYPT_TKIP_AES;
						ulStep = 5;
					}
				}
				break;

			case 5: /* RSN */
				/*unicast ciphers(1): AES-CCMP */
				/*unicast ciphers(1): TKIP */
				/*unicast ciphers(2): TKIP AES-CCMP */
				pTemp = VK_strstr(input,"ciphers(1):");
				if(pTemp)
				{
					VK_sscanf(pTemp,"%s %s",pCmd0,pCmd1);
					s_stWiFiInstance.stWiFiInfo[ulIndex].astAPList[nCnt].eSecureType = DI_WIFI_SECURE_WPA2;
					s_stWiFiInstance.stWiFiInfo[ulIndex].astAPList[nCnt].eAuthenType = DI_WIFI_AUTHEN_PSK;
					if(!VK_strncmp("AES-CCMP",pCmd1,VK_strlen(pCmd1)))
					{
						s_stWiFiInstance.stWiFiInfo[ulIndex].astAPList[nCnt].eEncryptType = DI_WIFI_ENCRYPT_AES;
					}
					else if(!VK_strncmp("TKIP",pCmd1,VK_strlen(pCmd1)))
					{
						s_stWiFiInstance.stWiFiInfo[ulIndex].astAPList[nCnt].eEncryptType = DI_WIFI_ENCRYPT_TKIP;
					}
					else
					{
						s_stWiFiInstance.stWiFiInfo[ulIndex].astAPList[nCnt].eEncryptType = DI_WIFI_ENCRYPT_UNKNOWN;
					}
					ulStep = 6;
				}

				if(pTemp == NULL)
				{
					pTemp = VK_strstr(input,"ciphers(2):");
					if(pTemp)
					{
						VK_sscanf(pTemp,"%s %s %s",pCmd0,pCmd1,pCmd2);
						s_stWiFiInstance.stWiFiInfo[ulIndex].astAPList[nCnt].eSecureType = DI_WIFI_SECURE_WPA2;
						s_stWiFiInstance.stWiFiInfo[ulIndex].astAPList[nCnt].eAuthenType = DI_WIFI_AUTHEN_PSK;
						s_stWiFiInstance.stWiFiInfo[ulIndex].astAPList[nCnt].eEncryptType = DI_WIFI_ENCRYPT_TKIP_AES;
						ulStep = 6;
					}
				}

				if(pTemp == NULL)
				{
					pTemp = VK_strstr(input,"WPS:");
					if(pTemp)
					{
						s_stWiFiInstance.stWiFiInfo[ulIndex].astAPList[nCnt].bWPS = TRUE;
						ulStep = 0;
					}
				}

				if(pTemp == NULL)
				{
					pTemp = VK_strstr(input,"SSID:");
					if(pTemp)
					{
						nCnt++;
						if(nCnt >= WIFI_AP_MAX)
						{
							nCnt--;
							break;
						}
						VK_sscanf(pTemp,"%s \"%s",pCmd0,pCmd1);
						VK_MEM_Memset(&s_stWiFiInstance.stWiFiInfo[ulIndex].astAPList[nCnt],0,sizeof(DI_WIFI_INFO_t));
						if(!((pCmd1[0] == 0x5C)&&(pCmd1[1] == 0x78)&&(pCmd1[2] == 0x30)&&(pCmd1[3] == 0x30)))
						{
							HUINT8 aucTempEssid[33] = {0,};
							HCHAR	*p = pTemp;
							HINT32	i = 0;

							while (*p++ != '\"')
							{
								/* move to the start pointer of ESSID string */
							}

							while (1)
							{
								if (*p != '\n')
								{
									aucTempEssid[i++] = *p++;
								}
								else
								{
									aucTempEssid[i-1] = '\0';
									break;
								}
							}

							VK_snprintf(s_stWiFiInstance.stWiFiInfo[ulIndex].astAPList[nCnt].aucESSID,sizeof(aucTempEssid),aucTempEssid);
						}
						s_stWiFiInstance.stWiFiInfo[ulIndex].astAPList[nCnt].ulFreq = 0;

						/*== SSID */
						ulStep = 1;
					}
				}
				break;

			case 6:
				if(pTemp == NULL)
				{
					pTemp = VK_strstr(input,"WPS:");
					if(pTemp)
					{
						s_stWiFiInstance.stWiFiInfo[ulIndex].astAPList[nCnt].bWPS = TRUE;
						ulStep = 0;
					}
				}

				if(pTemp == NULL)
				{
					pTemp = VK_strstr(input,"SSID:");
					if(pTemp)
					{
						nCnt++;
						if(nCnt >= WIFI_AP_MAX)
						{
							nCnt--;
							break;
						}
						VK_sscanf(pTemp,"%s \"%s",pCmd0,pCmd1);
						VK_MEM_Memset(&s_stWiFiInstance.stWiFiInfo[ulIndex].astAPList[nCnt],0,sizeof(DI_WIFI_INFO_t));
						if(!((pCmd1[0] == 0x5C)&&(pCmd1[1] == 0x78)&&(pCmd1[2] == 0x30)&&(pCmd1[3] == 0x30)))
						{
							HUINT8 aucTempEssid[33] = {0,};
							HCHAR	*p = pTemp;
							HINT32	i = 0;

							while (*p++ != '\"')
							{
								/* move to the start pointer of ESSID string */
							}

							while (1)
							{
								if (*p != '\n')
								{
									aucTempEssid[i++] = *p++;
								}
								else
								{
									aucTempEssid[i-1] = '\0';
									break;
								}
							}

							VK_snprintf(s_stWiFiInstance.stWiFiInfo[ulIndex].astAPList[nCnt].aucESSID,sizeof(aucTempEssid),aucTempEssid);
						}
						s_stWiFiInstance.stWiFiInfo[ulIndex].astAPList[nCnt].ulFreq = 2400;
						/*== SSID */
						ulStep = 1;
					}
				}
				break;
			default:
				ulStep = 0;
				break;
		}
	}
	s_stWiFiInstance.stWiFiInfo[ulIndex].ulNumOfAPs = nCnt+1;
	fclose(fp);
	return DRV_OK;
}

static DRV_Error P_WIFI_RunWpaSplctCmdForScanAp(HUINT32 ulIndex)
{
	DRV_Error 				dResult = DRV_OK;
	unsigned char			*buffer = NULL; 		/* Results */
	int 					buflen = IW_SCAN_MAX_DATA+1; /* Min for compat WE<17 */
	int 					timeout = SCAN_TIMEOUT;
	int 					nNumOfAPs = 0;
	int 					nTmpNumOfAp = 0;

	struct iwreq			iwrequest;				/* request data entry */
	struct timeval			tv; 			/* Select timeout */

	/* Check socket for request */
	if(s_nSocketFd < 0)
	{
		PrintError("[%s] ERR : s_nSocketFd (0x%x)!!\n", __func__, s_nSocketFd);
		dResult = DRV_ERR;
		goto WIFI_SCAN_END;
	}

	/* name */
	VK_MEM_Memset(&iwrequest, 0, sizeof(struct iwreq));
	VK_strncpy(iwrequest.ifr_name, s_stWiFiInstance.stWiFiInfo[ulIndex].acIfName, IFNAMSIZ-1);

	if(ioctl(s_nSocketFd, SIOCSIWSCAN, &iwrequest) < 0)
	{
		PrintError("[%s] ERR SIOCSIWSCAN !!\n",__func__);
		dResult = DRV_ERR;
		goto WIFI_SCAN_END;
	}

	tv.tv_sec = 0;
	tv.tv_usec = 250000;

	while(1)
	{
		fd_set		rfds;		/* File descriptors for select */
		int 	last_fd;	/* Last fd */
		int 	ret;

		/* Guess what ? We must re-generate rfds each time */
		FD_ZERO(&rfds);
		last_fd = -1;

		/* In here, add the rtnetlink fd in the list */

		/* Wait until something happens */
		ret = select(last_fd + 1, &rfds, NULL, NULL, &tv);
		/* Check if there was an error */
		if(ret < 0)
		{
			/* except these errors */
			if(errno == EAGAIN || errno == EINTR)
			{
				continue;
			}

			PrintError("P_WIFI_Task()- : WIFI_EVT_SCAN_START] select errn(%s)\n", strerror( errno ));
			dResult = DRV_ERR;
			goto WIFI_SCAN_END;
		}

		/* Check if there was a timeout */
		if(ret == 0)
		{
			unsigned char * newbuf;

WLAN_Realloc:
			/* (Re)allocate the buffer - realloc(NULL, len) == malloc(len) */
			newbuf = DD_MEM_Realloc(buffer, buflen);
			if(newbuf == NULL)
			{
				PrintError("P_WIFI_Task()- : WIFI_EVT_SCAN_START] Realloc size %d \n", buflen);
				dResult = DRV_ERR;
				goto WIFI_SCAN_END;
			}
			buffer = newbuf;

			/* Try to read the results */
			VK_MEM_Memset(&iwrequest, 0, sizeof(struct iwreq));
			VK_strncpy(iwrequest.ifr_name, s_stWiFiInstance.stWiFiInfo[ulIndex].acIfName, IFNAMSIZ-1);
			iwrequest.u.data.pointer = buffer;
			iwrequest.u.data.flags = 0;
			iwrequest.u.data.length = buflen;
			if(ioctl(s_nSocketFd, SIOCGIWSCAN, &iwrequest) < 0)
			{
				/* Check if buffer was too small (WE-17 only) */
				if((errno == E2BIG) ) //&& (range.we_version_compiled > 16))
				{
					/* Some driver may return very large scan results, either
					* because there are many cells, or because they have many
					* large elements in cells (like IWEVCUSTOM). Most will
					* only need the regular sized buffer. We now use a dynamic
					* allocation of the buffer to satisfy everybody. Of course,
					* as we don't know in advance the size of the array, we try
					* various increasing sizes. Jean II */

					/* Check if the driver gave us any hints. */
					if(iwrequest.u.data.length > buflen)
					{
						buflen = iwrequest.u.data.length;
					}
					else
					{
						buflen *= 2;
					}
					/* Try again */
					goto WLAN_Realloc;
				}

				/* Check if results not available yet */
				if(errno == EAGAIN)
				{
					/* Restart timer for only 100ms->200ms*/
					timeout -= tv.tv_usec;

					if(timeout > 0)
					{
						continue;	/* Try again later */
					}
				}

				PrintError("[%s] ERR SIOCGIWSCAN time out !!\n",__func__);
				dResult = DRV_ERR;
				goto WIFI_SCAN_END;

			}
			else
			{
				/* We have the results, go to process them */
				break;
			}
		}

		/* In here, check if event and event type
		* if scan event, read results. All errors bad & no reset timeout */
	}

	/* Get Scan results */
	if(iwrequest.u.data.length)
	{
		struct iw_event 	iwe;
		struct stream_descr stream;
		unsigned char		*start = NULL;
		unsigned char		*current = NULL;
		unsigned char		*end = NULL;
		int 		result = 0;
		int 		we_version;

		HBOOL		bSkipFlag = TRUE;

		DI_WIFI_INFO_t	tmpApInfo;

		start = buffer;
		current = buffer;
		end = buffer + iwrequest.u.data.length;
		stream.value = NULL;
		stream.current = buffer;
		stream.end = buffer + iwrequest.u.data.length;

		VK_MEM_Memset(&tmpApInfo, 0x0, sizeof(tmpApInfo));

		/* parsing할 데이터가 작으면 끝낸다. */
		we_version = P_WIFI_GetKernelWeVer();
		nTmpNumOfAp = 0;
		bSkipFlag = TRUE;
		VK_MEM_Memset(s_stWiFiInstance.stWiFiInfo[ulIndex].astAPList, 0, sizeof(s_stWiFiInstance.stWiFiInfo[ulIndex].astAPList));
		do
		{
			result = P_WIFI_ExtractInfoFormDriverEvent(&stream, &iwe, we_version);
			if(result > 0)
			{
				if((iwe.cmd == SIOCGIWAP) && (bSkipFlag == FALSE) && (nNumOfAPs < WIFI_AP_MAX))
				{
					/*NEW AP*/
					/*새로운 AP 이므로 이전 AP를 저장한다.*/
					VK_memcpy(&(s_stWiFiInstance.stWiFiInfo[ulIndex].astAPList[nNumOfAPs]), &tmpApInfo, sizeof(DI_WIFI_INFO_t));
					nNumOfAPs++;
					VK_MEM_Memset(&tmpApInfo, 0x0, sizeof(DI_WIFI_INFO_t));
				}
				bSkipFlag = FALSE; /* Only First skip */

				P_WIFI_MakeAPlist_ScanningInfo(&iwe,&tmpApInfo);
			}
		}
		while(result > 0);
		/* Save Last SSID */
		if(nNumOfAPs < WIFI_AP_MAX)
		{
			HUINT32 nNum = 0;

			nNum = tmpApInfo.aucMAC[0]+tmpApInfo.aucMAC[1]+tmpApInfo.aucMAC[2]+tmpApInfo.aucMAC[3]+tmpApInfo.aucMAC[4]+tmpApInfo.aucMAC[5];
			if(nNum == 0 || nNum == 1530) /* 0:0:0:0:0:0 or FF:FF:FF:FF:FF:FF */
			{
				VK_MEM_Memset(&tmpApInfo, 0x0, sizeof(DI_WIFI_INFO_t));
			}
			else
			{
				VK_memcpy(&(s_stWiFiInstance.stWiFiInfo[ulIndex].astAPList[nNumOfAPs]), &tmpApInfo, sizeof(DI_WIFI_INFO_t));
				nNumOfAPs++;
				VK_MEM_Memset(&tmpApInfo, 0x0, sizeof(DI_WIFI_INFO_t));
			}
		}

	}
	else
	{
		PrintError("[%s] ERR Scan result num 0!!\n",__func__);
	}

	s_stWiFiInstance.stWiFiInfo[ulIndex].ulNumOfAPs = nNumOfAPs;

WIFI_SCAN_END:

	if(buffer)
	{
		DD_MEM_Free(buffer);
	}

	return dResult;
}

static DRV_Error P_WIFI_StartScanAp(HUINT32 ulIndex)
{
	DRV_Error dResult = DRV_OK;

	PrintEnter();

	if(s_eWifiCtlInterface == WIFI_CTL_WL)
	{
		dResult = P_WIFI_RunWlCmdForScanAp(ulIndex);
		if(dResult != DRV_OK)
		{
			PrintError("[%s] Error : P_WIFI_RunWlCmdForScanAp %d \n", __FUNCTION__, dResult);
			goto ret;
		}
	}
	else if(s_eWifiCtlInterface == WIFI_CTL_WPA_SUPPLICANT)
	{
		dResult = P_WIFI_RunWpaSplctCmdForScanAp(ulIndex);
		if(dResult != DRV_OK)
		{
			PrintError("[%s] Error : P_WIFI_RunWpaSplctCmdForScanAp %d \n", __FUNCTION__, dResult);
			goto ret;
		}

	}
#if defined(CONFIG_QV940)
	else if(s_eWifiCtlInterface == WIFI_CTL_QCSAPI)
	{
		dResult = P_WIFI_RunQcsapiCmdForScanAp(ulIndex);
		if(dResult != DRV_OK)
		{
			PrintError("[%s] Error : P_WIFI_RunQcsapiCmdForScanAp %d \n", __FUNCTION__, dResult);
			goto ret;
		}
	}
#endif
	else
	{
		PrintError("[%s] Error : s_eWifiCtlInterface %d \n", __FUNCTION__, s_eWifiCtlInterface);
		dResult = DRV_ERR;
		goto ret;
	}

ret:
	PrintExit();

	return dResult;

}

static DRV_Error P_WIFI_RunWlCmdForConnectToAp(HUINT32 ulIndex, HINT8 *pszIFname, DI_WIFI_INFO_t *pstConfig)
{
	char acWlCmdBuf[384];
	char acTmpSsid[66];
	char acTmpKey[258];

	HUINT8	ucIdx, ucStrLen = 0;
	HUINT8	ucWlWsec = 0, ucWlWpaAuth = 0;

	if ((pszIFname == NULL) || (pstConfig == NULL))
	{
		return DRV_ERR;
	}

	s_stWiFiInstance.stWiFiInfo[ulIndex].eStatus = WIFI_STATUS_CONNECTING;
	s_stWiFiInstance.stWiFiInfo[ulIndex].eWPS = DI_WIFI_WPS_NONE;

	VK_MEM_Memset(acWlCmdBuf, 0x00, sizeof(acWlCmdBuf));

	/* before setting "mimo_bw_cap" & "sgi_rx" & "obss_coex", have to set "down"  */
	P_WIFI_SetBrcmDown();

#if defined(CONFIG_SUPPORT_2P4G_40M_BW)
	/* 20M/40M bandwidth support */
	VK_SYSTEM_Command("wl mimo_bw_cap 1");
	PrintDebug("[%s]_%d %s\n",__func__,__LINE__,"wl mimo_bw_cap 1");
#else
	if ((pstConfig->ulChannel >= 1) && (pstConfig->ulChannel <= 14))
	{
		/* Case 2.4Ghz, only 20M bandwidth support */
		VK_SYSTEM_Command("wl mimo_bw_cap 0");
		PrintDebug("[%s]_%d %s\n",__func__,__LINE__,"wl mimo_bw_cap 0");

		VK_SYSTEM_Command("wl sgi_rx 1");
		PrintDebug("[%s]_%d %s\n",__func__,__LINE__,"wl sgi_rx 1");
	}
	else
	{
		/* Case 5GHz, 20M/40M bandwidth support */
		VK_SYSTEM_Command("wl mimo_bw_cap 1");
		PrintDebug("[%s]_%d %s\n",__func__,__LINE__,"wl mimo_bw_cap 1");

		VK_SYSTEM_Command("wl sgi_rx 3");
		PrintDebug("[%s]_%d %s\n",__func__,__LINE__,"wl sgi_rx 3");
	}
#endif

	/* 5.2.35 Basic Association in 802.11n Environment /  HT Capability IE - 20/40 MHz Supported in 2.4 GHz for  Wifi Alliance  Cerfitication */
	VK_SYSTEM_Command("wl obss_coex 1");
	PrintDebug("[%s]_%d %s\n",__func__,__LINE__,"wl obss_coex 1");

	/* Sets Wireless Multimedia Extensions (WME) mode. */
	VK_SYSTEM_Command("wl wme 1");
	PrintDebug("[%s]_%d %s\n",__func__,__LINE__,"wl wme 1");

	/* and "up"*/
	P_WIFI_SetBrcmUp();

	/* When frame bursting is enabled, multiple frames are sent with a minimum interframe gap, which enhances network efficiency and reduce overhead. */
	VK_SYSTEM_Command("wl frameburst 1");
	PrintDebug("[%s]_%d %s\n",__func__,__LINE__,"wl frameburst 1");

	/* auth - 0 = OpenSystem - Do not change */
	VK_SYSTEM_Command("wl auth 0");
	PrintDebug("[%s]_%d %s\n",__func__,__LINE__,"wl auth 0");

	/* infra - 1 = Infra BSS - Do not change */
	VK_SYSTEM_Command("wl infra 1");
	PrintDebug("[%s]_%d %s\n",__func__,__LINE__,"wl infra 1");

	/* assoc_retry_max = 7 */
	VK_SYSTEM_Command("wl assoc_retry_max 7");
	PrintDebug("[%s]_%d %s\n",__func__,__LINE__,"wl assoc_retry_max 7");

	/*
		wsec - 1 = WEP enabled
			    2 = TKIP enabled
			    4 = AES enabled
	*/
	switch (pstConfig->eEncryptType)
	{
		case DI_WIFI_ENCRYPT_NONE :
			ucWlWsec = 0;
			break;
		case DI_WIFI_ENCRYPT_WEP_64 :
		case DI_WIFI_ENCRYPT_WEP_64HEX :
		case DI_WIFI_ENCRYPT_WEP_128 :
		case DI_WIFI_ENCRYPT_WEP_128HEX :
			ucWlWsec = 1;
			break;
		case DI_WIFI_ENCRYPT_TKIP :
			ucWlWsec = 2;
			break;
		case DI_WIFI_ENCRYPT_AES :
		case DI_WIFI_ENCRYPT_TKIP_AES :
			ucWlWsec = 4;
			break;
		default :
			break;
	}
	VK_snprintf(acWlCmdBuf,sizeof(acWlCmdBuf), "wl wsec %d", ucWlWsec);
	PrintDebug("[%s]_%d %s\n",__func__,__LINE__,acWlCmdBuf);
	VK_SYSTEM_Command(acWlCmdBuf);

	/*
		wpa_auth - 0 = disable WPA
				    1 = WPA-NONE
				    2 =  WPA-802.1X/WPA-Professional
				    4 =  WPA-PSK/WPA-Personal
				    64 = WPA2-802.1X/WPA2-Professional
				    128 = WPA2-PSK/WPA2-Personal
        */
	switch (pstConfig->eSecureType)
	{
		case DI_WIFI_SECURE_OPEN :
			ucWlWpaAuth = 0;
			break;
		case DI_WIFI_SECURE_WEP :
			ucWlWpaAuth = 0;
			break;
		case DI_WIFI_SECURE_WPA :
			ucWlWpaAuth = 4;
			break;
		case DI_WIFI_SECURE_WPA2 :
			ucWlWpaAuth = 128;
			break;
		default :
			break;
	}
	VK_MEM_Memset(acWlCmdBuf, 0x00, sizeof(acWlCmdBuf));
	VK_snprintf(acWlCmdBuf,sizeof(acWlCmdBuf), "wl wpa_auth %d", ucWlWpaAuth);
	PrintDebug("[%s]_%d %s\n",__func__,__LINE__,acWlCmdBuf);
	VK_SYSTEM_Command(acWlCmdBuf);

	/* sup_wpa - What's this?? Can not find in "wl help" */
	VK_SYSTEM_Command("wl sup_wpa 1");
	PrintDebug("[%s]_%d %s\n",__func__,__LINE__,"wl sup_wpa 1");

	/* add "\" to front of each character for handling special characters */
	VK_MEM_Memset(acTmpKey, 0x00, sizeof(acTmpKey));
	ucStrLen = VK_strlen(pstConfig->aucKey);
	PrintDebug("[%s]_%d Key Length = %d \n",__func__,__LINE__,ucStrLen);
	for ( ucIdx = 0 ; ucIdx < ucStrLen ; ucIdx++ )
	{
		PrintDebug("pstConfig->aucKey[%d] = [%c]\n",ucIdx, pstConfig->aucKey[ucIdx]);
		acTmpKey[2*ucIdx]	= 0x5C; 	// Back Slash
		acTmpKey[2*ucIdx+1] = pstConfig->aucKey[ucIdx];
	}

	/* set key - WEP */
	if (pstConfig->eSecureType == DI_WIFI_SECURE_WEP)
	{
		VK_MEM_Memset(acWlCmdBuf, 0x00, sizeof(acWlCmdBuf));
		VK_snprintf(acWlCmdBuf,sizeof(acWlCmdBuf), "wl addwep %d %s", pstConfig->aucKeyIndex, acTmpKey);
		PrintDebug("[%s]_%d %s\n",__func__,__LINE__,acWlCmdBuf);
		VK_SYSTEM_Command(acWlCmdBuf);
	}

	/* set key - WPA / WPA2 */
	if ((pstConfig->eSecureType == DI_WIFI_SECURE_WPA) || (pstConfig->eSecureType == DI_WIFI_SECURE_WPA2))
	{
		VK_MEM_Memset(acWlCmdBuf, 0x00, sizeof(acWlCmdBuf));
		VK_snprintf(acWlCmdBuf,sizeof(acWlCmdBuf), "wl set_pmk %s", acTmpKey);
		PrintDebug("[%s]_%d %s\n",__func__,__LINE__,acWlCmdBuf);
		VK_SYSTEM_Command(acWlCmdBuf);
	}

	/* add "\" to front of each character for handling special characters */
	ucStrLen = 0;
	VK_MEM_Memset(acTmpSsid, 0x00, sizeof(acTmpSsid));
	ucStrLen = VK_strlen(pstConfig->aucESSID);
	PrintDebug("[%s]_%d SSID Length = %d \n",__func__,__LINE__,ucStrLen);
	for ( ucIdx = 0 ; ucIdx < ucStrLen ; ucIdx++ )
	{
		PrintDebug("pstConfig->aucESSID[%d] = [%c]\n",ucIdx, pstConfig->aucESSID[ucIdx]);
		acTmpSsid[2*ucIdx]	= 0x5C;		// Back Slash
		acTmpSsid[2*ucIdx+1]	= pstConfig->aucESSID[ucIdx];
	}

	/* just for debugging
	ucStrLen = VK_strlen(acTmpSsid);
	for ( ucIdx = 0 ; ucIdx < ucStrLen ; ucIdx++ )
	{
		PrintDebug("acTmpSsid[%d] = [%c]\n",ucIdx, acTmpSsid[ucIdx]);
	}
	*/

	/* set option for join */
	VK_MEM_Memset(acWlCmdBuf, 0x00, sizeof(acWlCmdBuf));
	if (pstConfig->eSecureType == DI_WIFI_SECURE_WPA)
	{
		VK_snprintf(acWlCmdBuf,sizeof(acWlCmdBuf), "wl join %s imode bss amode wpapsk", acTmpSsid);
	}
	else if (pstConfig->eSecureType == DI_WIFI_SECURE_WPA2)
	{
		VK_snprintf(acWlCmdBuf,sizeof(acWlCmdBuf), "wl join %s imode bss amode wpa2psk", acTmpSsid);
	}
	else
	{
		VK_snprintf(acWlCmdBuf,sizeof(acWlCmdBuf), "wl join %s", acTmpSsid);
	}

	PrintDebug("[%s]_%d %s\n",__func__,__LINE__,acWlCmdBuf);
	VK_SYSTEM_Command(acWlCmdBuf);

	return DRV_OK;
}

static DRV_Error P_WIFI_RunWpaSplctCmdForConnectToAp(HUINT32 ulIndex, HINT8 *pszIFname, DI_WIFI_INFO_t *pstConfig)
{
	DRV_Error	dResult = DRV_OK;
	char cmdBuf[256];
	HINT8 acPidFile[48],acConfigFile[48];

	UNUSED(pszIFname);
	UNUSED(pstConfig);

	/* If WPA_Supplicant is alive, Kill WPA supplicant */
	P_WIFI_KillWpaSupplicant((HINT8 *)s_stWiFiInstance.stWiFiInfo[ulIndex].acIfName);

	/* Ready to connect */
	s_stWiFiInstance.stWiFiInfo[ulIndex].eStatus = WIFI_STATUS_CONNECTING;
	s_stWiFiInstance.stWiFiInfo[ulIndex].eWPS = DI_WIFI_WPS_NONE;

	VK_MEM_Memset(cmdBuf, 0x00, sizeof(cmdBuf));
	VK_MEM_Memset(acPidFile, 0x00, sizeof(acPidFile));
	VK_MEM_Memset(acConfigFile, 0x00, sizeof(acConfigFile));

	dResult = P_WIFI_MakeWpaSupplicantConf(s_stWiFiInstance.stWiFiInfo[ulIndex].acIfName, &s_stWiFiInstance.stWiFiInfo[ulIndex].stUserSetAP);
	if(dResult != DRV_OK)
	{
		PrintError("[%s] Error : P_WIFI_MakeWpaSupplicantConf %d \n", __FUNCTION__, dResult);
		goto ret;
	}
	VK_snprintf( (char*)acPidFile, sizeof(acPidFile), WPA_SUPPLICANT_PID_FILE_PATTERN, s_stWiFiInstance.stWiFiInfo[ulIndex].acIfName );
	VK_snprintf( (char*)acConfigFile, sizeof(acConfigFile), WPA_SUPPLICANT_CONFIG_FILE_PATTERN, s_stWiFiInstance.stWiFiInfo[ulIndex].acIfName );

	VK_snprintf( cmdBuf, sizeof( cmdBuf ), "wpa_supplicant -Bdddt -Dwext -i %s -c %s -P %s", s_stWiFiInstance.stWiFiInfo[ulIndex].acIfName, acConfigFile, acPidFile  );
	VK_SYSTEM_Command( cmdBuf );

ret:
	return dResult;

}

#if defined(CONFIG_QV940)
static DRV_Error P_WIFI_GetQcsapiAPInfo(HUINT32 ulIndex, DI_WIFI_INFO_t *pstApInfo)
{
	FILE *fp;
	HCHAR cmdBuf[255], buf[129], pCmd0[129];
	HINT32 wpsChannel = 0, wpsRSSI = 0;
	HUINT32 ulMac[6];

	PrintEnter();

	if (s_stWiFiInstance.stWiFiInfo[ulIndex].eWPS == DI_WIFI_WPS_NONE)
	{
		// BSSID
		VK_snprintf(cmdBuf, sizeof(cmdBuf), "call_qcsapi_sockrpc --host %s get_BSSID wifi0 0 > %s", IPADDR_QV940, QCSAPI_WPS_TMP_FILE);
		VK_SYSTEM_Command(cmdBuf);

		fp = fopen(QCSAPI_WPS_TMP_FILE, "r");
		if(fp != NULL)
		{
			fgets(buf, 128, fp);
			sscanf(buf, "%x:%x:%x:%x:%x:%x", &ulMac[0], &ulMac[1], &ulMac[2], &ulMac[3], &ulMac[4], &ulMac[5]);

			s_stWiFiInstance.stWiFiInfo[ulIndex].stUserSetAP.aucMAC[0] = ulMac[0];
			s_stWiFiInstance.stWiFiInfo[ulIndex].stUserSetAP.aucMAC[1] = ulMac[1];
			s_stWiFiInstance.stWiFiInfo[ulIndex].stUserSetAP.aucMAC[2] = ulMac[2];
			s_stWiFiInstance.stWiFiInfo[ulIndex].stUserSetAP.aucMAC[3] = ulMac[3];
			s_stWiFiInstance.stWiFiInfo[ulIndex].stUserSetAP.aucMAC[4] = ulMac[4];
			s_stWiFiInstance.stWiFiInfo[ulIndex].stUserSetAP.aucMAC[5] = ulMac[5];

#ifdef DI_QCSAPI_DEBUG // debug message
			VK_DBG_Print("[%s:%d] BSSID: %02x:%02x:%02x:%02x:%02x:%02x\n", __FUNCTION__, __LINE__, ulMac[0], ulMac[1], ulMac[2], ulMac[3], ulMac[4], ulMac[5]);
#endif
		}
		else
		{
			PrintError("[%s] Err fopen %s\n", __FUNCTION__, strerror( errno ));
			return DRV_ERR;
		}
		fclose(fp);
	}
	else if (s_stWiFiInstance.stWiFiInfo[ulIndex].eWPS == DI_WIFI_WPS_PBC)
	{
		// SSID
		VK_snprintf(cmdBuf, sizeof(cmdBuf), "call_qcsapi_sockrpc --host %s get_SSID wifi0 > %s", IPADDR_QV940, QCSAPI_WPS_TMP_FILE);
		VK_SYSTEM_Command(cmdBuf);

		fp = fopen(QCSAPI_WPS_TMP_FILE, "r");
		if(fp != NULL)
		{
			fgets(buf, 128, fp);
			sscanf(buf, "%s", pCmd0);

			VK_snprintf(s_stWiFiInstance.stWiFiInfo[ulIndex].stUserSetAP.aucESSID, sizeof(s_stWiFiInstance.stWiFiInfo[ulIndex].stUserSetAP.aucESSID), "%s", pCmd0);
#ifdef DI_QCSAPI_DEBUG // debug message
			VK_DBG_Print("[%s:%d] WPS SSID: '%s'\n", __FUNCTION__, __LINE__, pCmd0);
#endif
		}
		else
		{
			PrintError("[%s] Err fopen %s\n", __FUNCTION__, strerror( errno ));
			return DRV_ERR;
		}
		fclose(fp);

		// Secure Type
		VK_snprintf(cmdBuf, sizeof(cmdBuf), "call_qcsapi_sockrpc --host %s SSID_get_proto wifi0 \"%s\" > %s", 
				IPADDR_QV940, s_stWiFiInstance.stWiFiInfo[ulIndex].stUserSetAP.aucESSID, QCSAPI_WPS_TMP_FILE);
		VK_SYSTEM_Command(cmdBuf);

		fp = fopen(QCSAPI_WPS_TMP_FILE, "r");
		if(fp != NULL)
		{
			fgets(buf, 128, fp);
			sscanf(buf, "%s", pCmd0);

			if(!VK_strncmp("WPA", pCmd0, VK_strlen(pCmd0)))
			{
				s_stWiFiInstance.stWiFiInfo[ulIndex].stUserSetAP.eSecureType = DI_WIFI_SECURE_WPA;
			}
			else if(!VK_strncmp("11i", pCmd0, VK_strlen(pCmd0)))
			{
				s_stWiFiInstance.stWiFiInfo[ulIndex].stUserSetAP.eSecureType = DI_WIFI_SECURE_WPA2;
			}
			else if(!VK_strncmp("WPAand11i", pCmd0, VK_strlen(pCmd0)))
			{
				s_stWiFiInstance.stWiFiInfo[ulIndex].stUserSetAP.eSecureType = DI_WIFI_SECURE_WPA_WPA2;
			}
			else if(!VK_strncmp("Basic", pCmd0, VK_strlen(pCmd0)))
			{
				s_stWiFiInstance.stWiFiInfo[ulIndex].stUserSetAP.eSecureType = DI_WIFI_SECURE_OPEN;
			}
#ifdef DI_QCSAPI_DEBUG // debug message
			VK_DBG_Print("[%s:%d] WPS Secure Type: %s\n", __FUNCTION__, __LINE__, pCmd0);
#endif
		}
		else
		{
			PrintError("[%s] Err fopen %s\n", __FUNCTION__, strerror( errno ));
			return DRV_ERR;
		}
		fclose(fp);

		// Encryption
		VK_snprintf(cmdBuf, sizeof(cmdBuf), "call_qcsapi_sockrpc --host %s SSID_get_encryption_modes wifi0 \"%s\" > %s", 
				IPADDR_QV940, s_stWiFiInstance.stWiFiInfo[ulIndex].stUserSetAP.aucESSID, QCSAPI_WPS_TMP_FILE);
		VK_SYSTEM_Command(cmdBuf);

		fp = fopen(QCSAPI_WPS_TMP_FILE, "r");
		if(fp != NULL)
		{
			fgets(buf, 128, fp);
			sscanf(buf, "%s", pCmd0);

			if(!VK_strncmp("AESEncryption", pCmd0, VK_strlen(pCmd0)))
			{
				s_stWiFiInstance.stWiFiInfo[ulIndex].stUserSetAP.eEncryptType = DI_WIFI_ENCRYPT_AES;
			}
			else if(!VK_strncmp("TKIPEncryption", pCmd0, VK_strlen(pCmd0)))
			{
				s_stWiFiInstance.stWiFiInfo[ulIndex].stUserSetAP.eEncryptType = DI_WIFI_ENCRYPT_TKIP;
			}
			else if(!VK_strncmp("TKIPandAESEncryption", pCmd0, VK_strlen(pCmd0)))
			{
				s_stWiFiInstance.stWiFiInfo[ulIndex].stUserSetAP.eEncryptType = DI_WIFI_ENCRYPT_TKIP_AES;
			}
#ifdef DI_QCSAPI_DEBUG // debug message
			VK_DBG_Print("[%s:%d] WPS Encryption: %s\n", __FUNCTION__, __LINE__, pCmd0);
#endif
		}
		else
		{
			PrintError("[%s] Err fopen %s\n", __FUNCTION__, strerror( errno ));
			return DRV_ERR;
		}
		fclose(fp);

		// Authentication
		VK_snprintf(cmdBuf, sizeof(cmdBuf), "call_qcsapi_sockrpc --host %s SSID_get_authentication_mode wifi0 \"%s\" > %s", 
				IPADDR_QV940, s_stWiFiInstance.stWiFiInfo[ulIndex].stUserSetAP.aucESSID, QCSAPI_WPS_TMP_FILE);
		VK_SYSTEM_Command(cmdBuf);

		fp = fopen(QCSAPI_WPS_TMP_FILE, "r");
		if(fp != NULL)
		{
			fgets(buf, 128, fp);
			sscanf(buf, "%s", pCmd0);

			if(!VK_strncmp("PSKAuthentication", pCmd0, VK_strlen(pCmd0)))
			{
				s_stWiFiInstance.stWiFiInfo[ulIndex].stUserSetAP.eAuthenType = DI_WIFI_AUTHEN_PSK;
			}
			else if(!VK_strncmp("EAPAuthentication", pCmd0, VK_strlen(pCmd0)))
			{
				s_stWiFiInstance.stWiFiInfo[ulIndex].stUserSetAP.eAuthenType = DI_WIFI_AUTHEN_UNKNOWN;
			}
			else if(!VK_strncmp("NONE", pCmd0, VK_strlen(pCmd0)))
			{
				s_stWiFiInstance.stWiFiInfo[ulIndex].stUserSetAP.eAuthenType = DI_WIFI_AUTHEN_OPEN;
			}
#ifdef DI_QCSAPI_DEBUG // debug message
			VK_DBG_Print("[%s:%d] WPS Authentication: %s\n", __FUNCTION__, __LINE__, pCmd0);
#endif
		}
		else
		{
			PrintError("[%s] Err fopen %s\n", __FUNCTION__, strerror( errno ));
			return DRV_ERR;
		}
		fclose(fp);

		// Key Passphrase
		VK_snprintf(cmdBuf, sizeof(cmdBuf), "call_qcsapi_sockrpc --host %s SSID_get_key_passphrase wifi0 \"%s\" 0 > %s", 
				IPADDR_QV940, s_stWiFiInstance.stWiFiInfo[ulIndex].stUserSetAP.aucESSID, QCSAPI_WPS_TMP_FILE);
		VK_SYSTEM_Command(cmdBuf);

		fp = fopen(QCSAPI_WPS_TMP_FILE, "r");
		if(fp != NULL)
		{
			fgets(buf, 128, fp);
			sscanf(buf, "%s", pCmd0);

			VK_snprintf(s_stWiFiInstance.stWiFiInfo[ulIndex].stUserSetAP.aucKey, sizeof(s_stWiFiInstance.stWiFiInfo[ulIndex].stUserSetAP.aucKey), "%s", pCmd0);
#ifdef DI_QCSAPI_DEBUG // debug message
			VK_DBG_Print("[%s:%d] WPS Key Passphrase: '%s'\n", __FUNCTION__, __LINE__, pCmd0);
#endif
		}
		else
		{
			PrintError("[%s] Err fopen %s\n", __FUNCTION__, strerror( errno ));
			return DRV_ERR;
		}
		fclose(fp);

		// Channel
		VK_snprintf(cmdBuf, sizeof(cmdBuf), "call_qcsapi_sockrpc --host %s get_channel wifi0 > %s", IPADDR_QV940, QCSAPI_WPS_TMP_FILE);
		VK_SYSTEM_Command(cmdBuf);

		fp = fopen(QCSAPI_WPS_TMP_FILE, "r");
		if(fp != NULL)
		{
			fgets(buf, 128, fp);
			sscanf(buf, "%d", &wpsChannel);
			s_stWiFiInstance.stWiFiInfo[ulIndex].stUserSetAP.ulChannel = wpsChannel;
#ifdef DI_QCSAPI_DEBUG // debug message
			VK_DBG_Print("[%s:%d] WPS Channel: %d\n", __FUNCTION__, __LINE__, wpsChannel);
#endif
		}
		else
		{
			PrintError("[%s] Err fopen %s\n", __FUNCTION__, strerror( errno ));
			return DRV_ERR;
		}
		fclose(fp);

		// RSSI
		VK_snprintf(cmdBuf, sizeof(cmdBuf), "call_qcsapi_sockrpc --host %s get_rssi wifi0 0 > %s", IPADDR_QV940, QCSAPI_WPS_TMP_FILE);
		VK_SYSTEM_Command(cmdBuf);

		fp = fopen(QCSAPI_WPS_TMP_FILE, "r");
		if(fp != NULL)
		{
			fgets(buf, 128, fp);
			sscanf(buf, "%d", &wpsRSSI);

			if (wpsRSSI == 70)
			{
			    s_stWiFiInstance.stWiFiInfo[ulIndex].stUserSetAP.nWaveStrength = 100;
			}
			else
			{
		    	double dTmpRssi, dTmpResult;
			    dTmpRssi = (double)wpsRSSI;
			    dTmpResult = (dTmpRssi / 68.0 * 100.0);
			    s_stWiFiInstance.stWiFiInfo[ulIndex].stUserSetAP.nWaveStrength = abs(dTmpResult);
			}

#ifdef DI_QCSAPI_DEBUG // debug message
			VK_DBG_Print("[%s:%d] WPS WaveStrength: %d\n", __FUNCTION__, __LINE__, s_stWiFiInstance.stWiFiInfo[ulIndex].stUserSetAP.nWaveStrength);
#endif
		}
		else
		{
			PrintError("[%s] Err fopen %s\n", __FUNCTION__, strerror( errno ));
			return DRV_ERR;
		}
		fclose(fp);

		// BSSID
		VK_snprintf(cmdBuf, sizeof(cmdBuf), "call_qcsapi_sockrpc --host %s get_BSSID wifi0 0 > %s", IPADDR_QV940, QCSAPI_WPS_TMP_FILE);
		VK_SYSTEM_Command(cmdBuf);

		fp = fopen(QCSAPI_WPS_TMP_FILE, "r");
		if(fp != NULL)
		{
			fgets(buf, 128, fp);
			sscanf(buf, "%x:%x:%x:%x:%x:%x", &ulMac[0], &ulMac[1], &ulMac[2], &ulMac[3], &ulMac[4], &ulMac[5]);

			s_stWiFiInstance.stWiFiInfo[ulIndex].stUserSetAP.aucMAC[0] = ulMac[0];
			s_stWiFiInstance.stWiFiInfo[ulIndex].stUserSetAP.aucMAC[1] = ulMac[1];
			s_stWiFiInstance.stWiFiInfo[ulIndex].stUserSetAP.aucMAC[2] = ulMac[2];
			s_stWiFiInstance.stWiFiInfo[ulIndex].stUserSetAP.aucMAC[3] = ulMac[3];
			s_stWiFiInstance.stWiFiInfo[ulIndex].stUserSetAP.aucMAC[4] = ulMac[4];
			s_stWiFiInstance.stWiFiInfo[ulIndex].stUserSetAP.aucMAC[5] = ulMac[5];

#ifdef DI_QCSAPI_DEBUG // debug message
			VK_DBG_Print("[%s:%d] BSSID: %02x:%02x:%02x:%02x:%02x:%02x\n", __FUNCTION__, __LINE__, ulMac[0], ulMac[1], ulMac[2], ulMac[3], ulMac[4], ulMac[5]);
#endif
		}
		else
		{
			PrintError("[%s] Err fopen %s\n", __FUNCTION__, strerror( errno ));
			return DRV_ERR;
		}
		fclose(fp);
	}

#ifdef DI_QCSAPI_DEBUG // debug message
	VK_printf("  --------------- AP Info -------------\n");
	VK_printf("  * ESSID  : \"%s\"\n", s_stWiFiInstance.stWiFiInfo[ulIndex].stUserSetAP.aucESSID);
	VK_printf("  * SEC    : %d\n", s_stWiFiInstance.stWiFiInfo[ulIndex].stUserSetAP.eSecureType);
	VK_printf("  * AUTH   : %d\n", s_stWiFiInstance.stWiFiInfo[ulIndex].stUserSetAP.eAuthenType);
	VK_printf("  * ENC    : %d\n", s_stWiFiInstance.stWiFiInfo[ulIndex].stUserSetAP.eEncryptType);
	VK_printf("  * RSSI   : %d\n", s_stWiFiInstance.stWiFiInfo[ulIndex].stUserSetAP.nWaveStrength);
	VK_printf("  \n");
#endif

	VK_memcpy(pstApInfo, &s_stWiFiInstance.stWiFiInfo[ulIndex].stUserSetAP, sizeof(DI_WIFI_INFO_t));

	PrintExit();

	return DRV_OK;
}

static DRV_Error P_WIFI_RunQcsapiCmdForConnectToAp(HUINT32 ulIndex, DI_WIFI_INFO_t *pstConfig)
{
    FILE *fp;
    char cmdBuf[255], buf[129];
    int i, assocStatus = 0, qcsapiAPcounter = 0;
	HUINT32 ulMac[6];

	PrintEnter();

	/* Ready to connect */
	s_stWiFiInstance.stWiFiInfo[ulIndex].eStatus = WIFI_STATUS_CONNECTING;
	s_stWiFiInstance.stWiFiInfo[ulIndex].eWPS = DI_WIFI_WPS_NONE;

	VK_MEM_Memset(cmdBuf, 0x00, sizeof(cmdBuf));

	VK_snprintf(cmdBuf, sizeof(cmdBuf), "call_qcsapi_sockrpc --host %s get_count_assoc  wifi0 > %s",
		IPADDR_QV940, QCSAPI_ASSOC_STATUS_FILE);
	VK_SYSTEM_Command(cmdBuf);

	fp = fopen(QCSAPI_ASSOC_STATUS_FILE, "r");
	if (fp != NULL)
	{
		fgets(buf, 128, fp);
		VK_sscanf(buf, "%d", &assocStatus);
		fclose(fp);

		// debug message
		//VK_DBG_Print("  %s(%d) assocStatus=%d\n", __FUNCTION__, __LINE__, assocStatus);

		if (assocStatus)
		{
			/* disassociate */
			VK_snprintf(cmdBuf, sizeof(cmdBuf), "call_qcsapi_sockrpc --host %s disassociate wifi0 > %s", IPADDR_QV940, QCSAPI_TMP_FILE);
			VK_SYSTEM_Command(cmdBuf);
		}
	}
	else
	{
		PrintError("[%s] Err fopen %s\n", __FUNCTION__, strerror( errno ));
		return DRV_ERR;
	}

	if( pstConfig->eSecureType == DI_WIFI_SECURE_OPEN \
		|| pstConfig->eSecureType == DI_WIFI_SECURE_WPA \
		|| pstConfig->eSecureType == DI_WIFI_SECURE_WPA2 \
		|| pstConfig->eSecureType == DI_WIFI_SECURE_WPA_WPA2 )
	{
		VK_snprintf( cmdBuf, sizeof( cmdBuf ), "call_qcsapi_sockrpc --host %s remove_ssid wifi0 \"%s\" > %s",
		    IPADDR_QV940, pstConfig->aucESSID, QCSAPI_TMP_FILE);
		VK_SYSTEM_Command(cmdBuf);
		VK_snprintf( cmdBuf, sizeof( cmdBuf ), "call_qcsapi_sockrpc --host %s create_ssid wifi0 \"%s\" > %s",
		    IPADDR_QV940, pstConfig->aucESSID, QCSAPI_TMP_FILE);
		VK_SYSTEM_Command(cmdBuf);
	}
	else
	{
		PrintError("[%s] Error : not supported secure type \n", __FUNCTION__);
		return DRV_ERR;
	}

	switch ( pstConfig->eSecureType )
	{
		case DI_WIFI_SECURE_OPEN :
		{
			VK_snprintf( cmdBuf, sizeof( cmdBuf ), "call_qcsapi_sockrpc --host %s SSID_set_authentication_mode wifi0 \"%s\" NONE > %s",
			    IPADDR_QV940, pstConfig->aucESSID, QCSAPI_TMP_FILE);
			VK_SYSTEM_Command(cmdBuf);
			VK_snprintf( cmdBuf, sizeof( cmdBuf ), "call_qcsapi_sockrpc --host %s apply_security_config wifi0 > %s",
			    IPADDR_QV940, QCSAPI_TMP_FILE);
			VK_SYSTEM_Command(cmdBuf);
			VK_snprintf( cmdBuf, sizeof( cmdBuf ), "call_qcsapi_sockrpc --host %s associate wifi0 \"%s\" > %s",
			    IPADDR_QV940, pstConfig->aucESSID, QCSAPI_TMP_FILE);
			VK_SYSTEM_Command(cmdBuf);
			break;
		}
		case DI_WIFI_SECURE_WPA :
		{
			if ( pstConfig->eEncryptType == DI_WIFI_ENCRYPT_TKIP) {
				VK_snprintf( cmdBuf, sizeof( cmdBuf ), "call_qcsapi_sockrpc --host %s SSID_set_authentication_mode wifi0 \"%s\" PSKAuthentication > %s",
				    IPADDR_QV940, pstConfig->aucESSID, QCSAPI_TMP_FILE);
				VK_SYSTEM_Command(cmdBuf);
				VK_snprintf( cmdBuf, sizeof( cmdBuf ), "call_qcsapi_sockrpc --host %s SSID_set_proto wifi0 \"%s\" WPA > %s",
				    IPADDR_QV940, pstConfig->aucESSID, QCSAPI_TMP_FILE);
				VK_SYSTEM_Command(cmdBuf);
				VK_snprintf( cmdBuf, sizeof( cmdBuf ), "call_qcsapi_sockrpc --host %s SSID_set_encryption_modes wifi0 \"%s\" TKIPEncryption > %s",
				    IPADDR_QV940, pstConfig->aucESSID, QCSAPI_TMP_FILE);
				VK_SYSTEM_Command(cmdBuf);
				VK_snprintf( cmdBuf, sizeof( cmdBuf ), "call_qcsapi_sockrpc --host %s SSID_set_key_passphrase wifi0 \"%s\" 0 \"%s\" > %s",
				    IPADDR_QV940, pstConfig->aucESSID, pstConfig->aucKey, QCSAPI_TMP_FILE);
				VK_SYSTEM_Command(cmdBuf);
				VK_snprintf( cmdBuf, sizeof( cmdBuf ), "call_qcsapi_sockrpc --host %s apply_security_config wifi0 > %s",
				    IPADDR_QV940, QCSAPI_TMP_FILE);
				VK_SYSTEM_Command(cmdBuf);
				VK_snprintf( cmdBuf, sizeof( cmdBuf ), "call_qcsapi_sockrpc --host %s associate wifi0 \"%s\" > %s",
				    IPADDR_QV940, pstConfig->aucESSID, QCSAPI_TMP_FILE);
				VK_SYSTEM_Command(cmdBuf);
			} else {
				VK_snprintf( cmdBuf, sizeof( cmdBuf ), "call_qcsapi_sockrpc --host %s SSID_set_authentication_mode wifi0 \"%s\" PSKAuthentication > %s",
				    IPADDR_QV940, pstConfig->aucESSID, QCSAPI_TMP_FILE);
				VK_SYSTEM_Command(cmdBuf);
				VK_snprintf( cmdBuf, sizeof( cmdBuf ), "call_qcsapi_sockrpc --host %s SSID_set_proto wifi0 \"%s\" WPA > %s",
				    IPADDR_QV940, pstConfig->aucESSID, QCSAPI_TMP_FILE);
				VK_SYSTEM_Command(cmdBuf);
				VK_snprintf( cmdBuf, sizeof( cmdBuf ), "call_qcsapi_sockrpc --host %s SSID_set_encryption_modes wifi0 \"%s\" AESEncryption > %s",
				    IPADDR_QV940, pstConfig->aucESSID, QCSAPI_TMP_FILE);
				VK_SYSTEM_Command(cmdBuf);
				VK_snprintf( cmdBuf, sizeof( cmdBuf ), "call_qcsapi_sockrpc --host %s SSID_set_key_passphrase wifi0 \"%s\" 0 \"%s\" > %s",
				    IPADDR_QV940, pstConfig->aucESSID, pstConfig->aucKey, QCSAPI_TMP_FILE);
				VK_SYSTEM_Command(cmdBuf);
				VK_snprintf( cmdBuf, sizeof( cmdBuf ), "call_qcsapi_sockrpc --host %s apply_security_config wifi0 > %s",
				    IPADDR_QV940, QCSAPI_TMP_FILE);
				VK_SYSTEM_Command(cmdBuf);
				VK_snprintf( cmdBuf, sizeof( cmdBuf ), "call_qcsapi_sockrpc --host %s associate wifi0 \"%s\" > %s",
				    IPADDR_QV940, pstConfig->aucESSID, QCSAPI_TMP_FILE);
				VK_SYSTEM_Command(cmdBuf);
			}
			break;
		}
		case DI_WIFI_SECURE_WPA2 :
		{
			if ( pstConfig->eEncryptType == DI_WIFI_ENCRYPT_TKIP) {
				VK_snprintf( cmdBuf, sizeof( cmdBuf ), "call_qcsapi_sockrpc --host %s SSID_set_authentication_mode wifi0 \"%s\" PSKAuthentication > %s",
				    IPADDR_QV940, pstConfig->aucESSID, QCSAPI_TMP_FILE);
				VK_SYSTEM_Command(cmdBuf);
				VK_snprintf( cmdBuf, sizeof( cmdBuf ), "call_qcsapi_sockrpc --host %s SSID_set_proto wifi0 \"%s\" WPA2 > %s",
				    IPADDR_QV940, pstConfig->aucESSID, QCSAPI_TMP_FILE);
				VK_SYSTEM_Command(cmdBuf);
				VK_snprintf( cmdBuf, sizeof( cmdBuf ), "call_qcsapi_sockrpc --host %s SSID_set_encryption_modes wifi0 \"%s\" TKIPEncryption > %s",
				    IPADDR_QV940, pstConfig->aucESSID, QCSAPI_TMP_FILE);
				VK_SYSTEM_Command(cmdBuf);
				VK_snprintf( cmdBuf, sizeof( cmdBuf ), "call_qcsapi_sockrpc --host %s SSID_set_key_passphrase wifi0 \"%s\" 0 \"%s\" > %s",
				    IPADDR_QV940, pstConfig->aucESSID, pstConfig->aucKey, QCSAPI_TMP_FILE);
				VK_SYSTEM_Command(cmdBuf);
				VK_snprintf( cmdBuf, sizeof( cmdBuf ), "call_qcsapi_sockrpc --host %s apply_security_config wifi0 > %s",
				    IPADDR_QV940, QCSAPI_TMP_FILE);
				VK_SYSTEM_Command(cmdBuf);
				VK_snprintf( cmdBuf, sizeof( cmdBuf ), "call_qcsapi_sockrpc --host %s associate wifi0 \"%s\" > %s",
				    IPADDR_QV940, pstConfig->aucESSID, QCSAPI_TMP_FILE);
				VK_SYSTEM_Command(cmdBuf);
			} else {
				VK_snprintf( cmdBuf, sizeof( cmdBuf ), "call_qcsapi_sockrpc --host %s SSID_set_authentication_mode wifi0 \"%s\" PSKAuthentication > %s",
				    IPADDR_QV940, pstConfig->aucESSID, QCSAPI_TMP_FILE);
				VK_SYSTEM_Command(cmdBuf);
				VK_snprintf( cmdBuf, sizeof( cmdBuf ), "call_qcsapi_sockrpc --host %s SSID_set_proto wifi0 \"%s\" WPA2 > %s",
				    IPADDR_QV940, pstConfig->aucESSID, QCSAPI_TMP_FILE);
				VK_SYSTEM_Command(cmdBuf);
				VK_snprintf( cmdBuf, sizeof( cmdBuf ), "call_qcsapi_sockrpc --host %s SSID_set_encryption_modes wifi0 \"%s\" AESEncryption > %s",
				    IPADDR_QV940, pstConfig->aucESSID, QCSAPI_TMP_FILE);
				VK_SYSTEM_Command(cmdBuf);
				VK_snprintf( cmdBuf, sizeof( cmdBuf ), "call_qcsapi_sockrpc --host %s SSID_set_key_passphrase wifi0 \"%s\" 0 \"%s\" > %s",
				    IPADDR_QV940, pstConfig->aucESSID, pstConfig->aucKey, QCSAPI_TMP_FILE);
				VK_SYSTEM_Command(cmdBuf);
				VK_snprintf( cmdBuf, sizeof( cmdBuf ), "call_qcsapi_sockrpc --host %s apply_security_config wifi0 > %s",
				    IPADDR_QV940, QCSAPI_TMP_FILE);
				VK_SYSTEM_Command(cmdBuf);
				VK_snprintf( cmdBuf, sizeof( cmdBuf ), "call_qcsapi_sockrpc --host %s associate wifi0 \"%s\" > %s",
				    IPADDR_QV940, pstConfig->aucESSID, QCSAPI_TMP_FILE);
				VK_SYSTEM_Command(cmdBuf);
			}
			break;
		}
		case DI_WIFI_SECURE_WPA_WPA2 :
		{
			if ( pstConfig->eEncryptType == DI_WIFI_ENCRYPT_TKIP) {
				VK_snprintf( cmdBuf, sizeof( cmdBuf ), "call_qcsapi_sockrpc --host %s SSID_set_authentication_mode wifi0 \"%s\" PSKAuthentication > %s",
				    IPADDR_QV940, pstConfig->aucESSID, QCSAPI_TMP_FILE);
				VK_SYSTEM_Command(cmdBuf);
				VK_snprintf( cmdBuf, sizeof( cmdBuf ), "call_qcsapi_sockrpc --host %s SSID_set_proto wifi0 \"%s\" WPAand11i > %s",
				    IPADDR_QV940, pstConfig->aucESSID, QCSAPI_TMP_FILE);
				VK_SYSTEM_Command(cmdBuf);
				VK_snprintf( cmdBuf, sizeof( cmdBuf ), "call_qcsapi_sockrpc --host %s SSID_set_encryption_modes wifi0 \"%s\" TKIPEncryption > %s",
				    IPADDR_QV940, pstConfig->aucESSID, QCSAPI_TMP_FILE);
				VK_SYSTEM_Command(cmdBuf);
				VK_snprintf( cmdBuf, sizeof( cmdBuf ), "call_qcsapi_sockrpc --host %s SSID_set_key_passphrase wifi0 \"%s\" 0 \"%s\" > %s",
				    IPADDR_QV940, pstConfig->aucESSID, pstConfig->aucKey, QCSAPI_TMP_FILE);
				VK_SYSTEM_Command(cmdBuf);
				VK_snprintf( cmdBuf, sizeof( cmdBuf ), "call_qcsapi_sockrpc --host %s apply_security_config wifi0 > %s",
				    IPADDR_QV940, QCSAPI_TMP_FILE);
				VK_SYSTEM_Command(cmdBuf);
				VK_snprintf( cmdBuf, sizeof( cmdBuf ), "call_qcsapi_sockrpc --host %s associate wifi0 \"%s\" > %s",
				    IPADDR_QV940, pstConfig->aucESSID, QCSAPI_TMP_FILE);
				VK_SYSTEM_Command(cmdBuf);
			} else {
				VK_snprintf( cmdBuf, sizeof( cmdBuf ), "call_qcsapi_sockrpc --host %s SSID_set_authentication_mode wifi0 \"%s\" PSKAuthentication > %s",
				    IPADDR_QV940, pstConfig->aucESSID, QCSAPI_TMP_FILE);
				VK_SYSTEM_Command(cmdBuf);
				VK_snprintf( cmdBuf, sizeof( cmdBuf ), "call_qcsapi_sockrpc --host %s SSID_set_proto wifi0 \"%s\" WPAand11i > %s",
				    IPADDR_QV940, pstConfig->aucESSID, QCSAPI_TMP_FILE);
				VK_SYSTEM_Command(cmdBuf);
				VK_snprintf( cmdBuf, sizeof( cmdBuf ), "call_qcsapi_sockrpc --host %s SSID_set_encryption_modes wifi0 \"%s\" AESEncryption > %s",
				    IPADDR_QV940, pstConfig->aucESSID, QCSAPI_TMP_FILE);
				VK_SYSTEM_Command(cmdBuf);
				VK_snprintf( cmdBuf, sizeof( cmdBuf ), "call_qcsapi_sockrpc --host %s SSID_set_key_passphrase wifi0 \"%s\" 0 \"%s\" > %s",
				    IPADDR_QV940, pstConfig->aucESSID, pstConfig->aucKey, QCSAPI_TMP_FILE);
				VK_SYSTEM_Command(cmdBuf);
				VK_snprintf( cmdBuf, sizeof( cmdBuf ), "call_qcsapi_sockrpc --host %s apply_security_config wifi0 > %s",
				    IPADDR_QV940, QCSAPI_TMP_FILE);
				VK_SYSTEM_Command(cmdBuf);
				VK_snprintf( cmdBuf, sizeof( cmdBuf ), "call_qcsapi_sockrpc --host %s associate wifi0 \"%s\" > %s",
				    IPADDR_QV940, pstConfig->aucESSID, QCSAPI_TMP_FILE);
				VK_SYSTEM_Command(cmdBuf);
			}
			break;
		}
		default :
			PrintError("[%s] Error : not supported secure type \n", __FUNCTION__);
			return DRV_ERR;
			break;
	}

	/* get the count of APs */
	VK_snprintf(cmdBuf, sizeof(cmdBuf), "call_qcsapi_sockrpc --host %s get_results_AP_scan wifi0 > %s", IPADDR_QV940, QCSAPI_AP_COUNTER_FILE);
	VK_SYSTEM_Command(cmdBuf);
	VK_TASK_Sleep(500);

    /* save the user set AP info */
	fp = fopen(QCSAPI_AP_COUNTER_FILE, "r");
	if (fp != NULL)
	{
		fgets(buf, 128, fp);
		VK_sscanf(buf, "%d", &qcsapiAPcounter);
		fclose(fp);

		if (qcsapiAPcounter < 0)
	    {
	        PrintError("[%s] QCSAPI scaned APs counter error!\n", __FUNCTION__);
	        return DRV_ERR;
	    }
	    // debug message
	    //VK_DBG_Print("  %s(qcsapiAPcounter=%d)\n", __FUNCTION__, qcsapiAPcounter);
	}
	else
    {
		PrintError("[%s] Err fopen %s\n", __FUNCTION__, strerror( errno ));
		return DRV_ERR;
    }

    for (i = 0; i < qcsapiAPcounter; i++)
    {
        if (VK_strcmp(pstConfig->aucESSID, s_stWiFiInstance.stWiFiInfo[ulIndex].astAPList[i].aucESSID) == 0)
    	{
    	    i_qcsapiAPIndex = i;
    	}
    }

	// BSSID
	VK_snprintf(cmdBuf, sizeof(cmdBuf), "call_qcsapi_sockrpc --host %s get_BSSID wifi0 0 > %s", IPADDR_QV940, QCSAPI_WPS_TMP_FILE);
	VK_SYSTEM_Command(cmdBuf);

	fp = fopen(QCSAPI_WPS_TMP_FILE, "r");
	if(fp != NULL)
	{
		fgets(buf, 128, fp);
		sscanf(buf, "%x:%x:%x:%x:%x:%x", &ulMac[0], &ulMac[1], &ulMac[2], &ulMac[3], &ulMac[4], &ulMac[5]);

		pstConfig->aucMAC[0] = ulMac[0];
		pstConfig->aucMAC[1] = ulMac[1];
		pstConfig->aucMAC[2] = ulMac[2];
		pstConfig->aucMAC[3] = ulMac[3];
		pstConfig->aucMAC[4] = ulMac[4];
		pstConfig->aucMAC[5] = ulMac[5];

#ifdef DI_QCSAPI_DEBUG // debug message
		VK_DBG_Print("[%s:%d] BSSID: %02x:%02x:%02x:%02x:%02x:%02x\n", __FUNCTION__, __LINE__, ulMac[0], ulMac[1], ulMac[2], ulMac[3], ulMac[4], ulMac[5]);
#endif
	}
	else
	{
		PrintError("[%s] Err fopen %s\n", __FUNCTION__, strerror( errno ));
		return DRV_ERR;
	}
	fclose(fp);

    VK_memcpy(&(s_stWiFiInstance.stWiFiInfo[ulIndex].stUserSetAP), pstConfig, sizeof(DI_WIFI_INFO_t));

	PrintExit();

	return DRV_OK;
}

static DRV_Error P_WIFI_GetQcsapiWpsPinCode(HUINT32 ulDevId, char* pin)
{
	FILE *fp;
	char cmdBuf[255], buf[129];

	UNUSED(ulDevId);

	VK_snprintf(cmdBuf, sizeof(cmdBuf), "call_qcsapi_sockrpc --host %s get_wps_sta_pin wifi0 > %s", IPADDR_QV940, QCSAPI_WPS_PIN_CODE_FILE);
	VK_SYSTEM_Command(cmdBuf);
	VK_TASK_Sleep(1000);

	fp = fopen(QCSAPI_WPS_PIN_CODE_FILE, "r");
	if(fp != NULL)
	{
		fgets(buf, 128, fp);
		sscanf(buf, "%s", pin);
		fclose(fp);
	}
	else
	{
		PrintError("[%s] Err fopen %s\n", __FUNCTION__, strerror( errno ));
		return DRV_ERR;
	}

	return DRV_OK;
}
#endif /* CONFIG_QV940 */

#if !defined(CONFIG_QV940)
static DRV_Error P_WIFI_GetWpsPinCode(HUINT32 ulDevId, HCHAR *acWpsPinCode)
{
	size_t readSize = 0;

	HUINT32 ulMin = 10000000;
	HUINT32 ulMax = 99999999;
	HUINT32 ulPin = 0, ulRange = 0, ulLimit = 0, ulRandVal = 0, ulAccum = 0, ulLastDigit = 0;
	HCHAR acTmpPin[9];

	UNUSED(ulDevId);

	if (acWpsPinCode == NULL)
	{
		PrintError("[%s][%d] pointer for pin is NULL!! \n", __FUNCTION__, __LINE__);
		return DRV_ERR;
	}

	ulRange = 1 + ulMax - ulMin;
	ulLimit = RAND_MAX - (RAND_MAX % ulRange);

	do {
		FILE *fp = fopen("/dev/urandom", "r");
		
		if(fp == NULL)
		{
			PrintError("[%s] Err fopen /dev/random %s\n",__FUNCTION__, strerror( errno ));
			return DRV_ERR;
		}

		readSize = fread(&ulRandVal, sizeof(ulRandVal), 1, fp);
		if (readSize != 1)
		{
			PrintError("[%s] Err fread %s\n",__FUNCTION__, strerror( errno ));
			fclose(fp);
			return DRV_ERR;
		}

		if (fp != NULL)
		{
			fclose(fp);
		}
	} while (ulRandVal >= ulLimit);

	ulRandVal = (ulRandVal % ulRange) + ulMin;

	ulPin = ulRandVal / 10;
	ulPin *= 10;
	ulAccum += 3 * ((ulPin / 10000000) % 10);
	ulAccum += 1 * ((ulPin / 1000000) % 10);
	ulAccum += 3 * ((ulPin / 100000) % 10);
	ulAccum += 1 * ((ulPin / 10000) % 10);
	ulAccum += 3 * ((ulPin / 1000) % 10);
	ulAccum += 1 * ((ulPin / 100) % 10);
	ulAccum += 3 * ((ulPin / 10) % 10);

	ulLastDigit = (ulAccum % 10);

	ulAccum = (10 - ulLastDigit) % 10;

	ulPin += ulAccum;

	VK_snprintf(acTmpPin, sizeof(acTmpPin), "%08u", (HUINT32)ulPin);

	acTmpPin[8] = '\0';

	VK_snprintf(acWpsPinCode, sizeof(acTmpPin), acTmpPin);

	PrintDebug("[%s][%d] PIN Code = %s \n", __FUNCTION__, __LINE__, acWpsPinCode);

	return DRV_OK;
}
#endif

static DRV_Error P_WIFI_IsKeyValid(DI_WIFI_INFO_t *pstConfig)
{
	DRV_Error dResult = DRV_ERR;
	HINT32 nKeyLength = 0;

	nKeyLength = VK_strlen(pstConfig->aucKey);

	switch (pstConfig->eSecureType)
	{
		case DI_WIFI_SECURE_OPEN :
		{
			/* Ignore */
			PrintDebug("(*) [%s] : Security OPEN, Key existed! \n", __FUNCTION__);
			dResult = DRV_OK;
			break;
		}
		case DI_WIFI_SECURE_WEP :
		{
			if ((nKeyLength == 5) || (nKeyLength == 13) || (nKeyLength == 10) || (nKeyLength = 26))
			{
				dResult = DRV_OK;
			}
			else
			{
				PrintDebug("(*) [%s] : Security WEP, Key is invalid %d \n", __FUNCTION__, nKeyLength);
				dResult = DRV_ERR;
			}
			break;
		}

		case DI_WIFI_SECURE_WPA :
		case DI_WIFI_SECURE_WPA2 :
		case DI_WIFI_SECURE_WPA_WPA2:
		{
			if (nKeyLength >= 8)
			{
				dResult = DRV_OK;
			}
			else
			{
				PrintDebug("(*) [%s] : Security WPA or WPA2, Key is invalid %d \n", __FUNCTION__, nKeyLength);
				dResult = DRV_ERR;
			}
			break;
		}

		default :
		{
			PrintDebug("(*) [%s] : Security is invalid %d \n", __FUNCTION__, pstConfig->eSecureType);
			dResult = DRV_ERR;
			break;
		}
	}

	return dResult;
}

static void P_WIFI_ConnectToAp(HUINT32 ulIndex, HINT8 *pszIFname, DI_WIFI_INFO_t *pstConfig)
{
	DRV_Error dResult = DRV_OK;
    char cmdBuf[256] = { 0 };

	if(s_eWifiCtlInterface ==WIFI_CTL_WL)
	{
		dResult = P_WIFI_RunWlCmdForConnectToAp(ulIndex, pszIFname, pstConfig);
		if(dResult != DRV_OK)
		{
			PrintError("[%s] Error : P_WIFI_RunWlCmdForConnectToAp %d \n", __FUNCTION__, dResult);
			goto ret;
		}
	}
	else if(s_eWifiCtlInterface == WIFI_CTL_WPA_SUPPLICANT)
	{
#if 1 /* __HUMAX_CODE__ - 140409 mskim (redmine #88454) */
      /*
        Ralink RT5572 LinuxSTA 2.6.1.4 Driver를 사용하는 경우,
        Channel Assoc 과정에서 Pending되는 문제로 Interface Down/Up을 추가함.
        Driver 변경시 추가 확인 필요.
      */
        if (VK_strncmp("rt5572sta", wifiDevName, VK_strlen("rt5572sta")) == 0)
        {
			VK_snprintf( cmdBuf, sizeof( cmdBuf ), "ifconfig %s down", pszIFname );
			VK_SYSTEM_Command( cmdBuf );
			VK_TASK_Sleep(100);

			VK_snprintf( cmdBuf, sizeof( cmdBuf ), "ifconfig %s up", pszIFname );
			VK_SYSTEM_Command( cmdBuf );
			VK_TASK_Sleep(100);
        }
#endif
		dResult = P_WIFI_RunWpaSplctCmdForConnectToAp(ulIndex, pszIFname, pstConfig);
		if(dResult != DRV_OK)
		{
			PrintError("[%s] Error : P_WIFI_RunWpaSplctCmdForConnectToAp %d \n", __FUNCTION__, dResult);
			goto ret;
		}
	}
#if defined(CONFIG_QV940)
	else if(s_eWifiCtlInterface == WIFI_CTL_QCSAPI)
	{
		dResult = P_WIFI_RunQcsapiCmdForConnectToAp(ulIndex, pstConfig);
		if(dResult != DRV_OK)
		{
			PrintError("[%s] Error : P_WIFI_RunQcsapiCmdForConnectToAp %d \n", __FUNCTION__, dResult);
			goto ret;
		}
	}
#endif
	else
	{
		PrintError("[%s] Error : s_eWifiCtlInterface %d \n", __FUNCTION__, s_eWifiCtlInterface);
		goto ret;
	}

ret:
	return ;

}

static void P_WIFI_RunWlCmdForConnectWpsToAp(HUINT32 ulIndex, WIFI_EVENT_e eConnectType)
{
	HINT32 vResult;
	HBOOL bErr = FALSE;
	HINT32 nPid = 0;
	HUINT32 ulLoop = 0;
	char cmdBuf[256];

	s_stWiFiInstance.stWiFiInfo[ulIndex].eStatus = WIFI_STATUS_CONNECTING;

	VK_MEM_Memset(cmdBuf, 0x00, sizeof(cmdBuf));

	if(eConnectType == WIFI_EVT_WPS_PBC_START)
	{
		s_stWiFiInstance.stWiFiInfo[ulIndex].eWPS = DI_WIFI_WPS_PBC;
#if 1
		VK_snprintf( cmdBuf, sizeof( cmdBuf ), "wpsenr -if %s -pb>/tmp/wps.txt &",s_stWiFiInstance.stWiFiInfo[ulIndex].acIfName );
#else
		VK_snprintf( cmdBuf, sizeof( cmdBuf ), "wpsenr -if %s -v1 -pb>/tmp/wps.txt &",s_stWiFiInstance.stWiFiInfo[ulIndex].acIfName );
#endif
	}
	else
	{
		s_stWiFiInstance.stWiFiInfo[ulIndex].eWPS = DI_WIFI_WPS_PIN;
#if 1
		VK_snprintf( cmdBuf, sizeof( cmdBuf ), "wpsenr -if %s -auto -pin \"%d%d%d%d%d%d%d%d\">/tmp/wps.txt &"
									,s_stWiFiInstance.stWiFiInfo[ulIndex].acIfName
									,s_stWiFiInstance.stWiFiInfo[ulIndex].stUserSetAP.aucKey[0],
									s_stWiFiInstance.stWiFiInfo[ulIndex].stUserSetAP.aucKey[1],
									s_stWiFiInstance.stWiFiInfo[ulIndex].stUserSetAP.aucKey[2],
									s_stWiFiInstance.stWiFiInfo[ulIndex].stUserSetAP.aucKey[3],
									s_stWiFiInstance.stWiFiInfo[ulIndex].stUserSetAP.aucKey[4],
									s_stWiFiInstance.stWiFiInfo[ulIndex].stUserSetAP.aucKey[5],
									s_stWiFiInstance.stWiFiInfo[ulIndex].stUserSetAP.aucKey[6],
									s_stWiFiInstance.stWiFiInfo[ulIndex].stUserSetAP.aucKey[7]);
#else
		VK_snprintf( cmdBuf, sizeof( cmdBuf ), "wpsenr -if %s -auto -v1 -pin \"%d%d%d%d%d%d%d%d\">/tmp/wps.txt &"
			,s_stWiFiInstance.stWiFiInfo[ulIndex].acIfName
			,s_stWiFiInstance.stWiFiInfo[ulIndex].stUserSetAP.aucKey[0],
			s_stWiFiInstance.stWiFiInfo[ulIndex].stUserSetAP.aucKey[1],
			s_stWiFiInstance.stWiFiInfo[ulIndex].stUserSetAP.aucKey[2],
			s_stWiFiInstance.stWiFiInfo[ulIndex].stUserSetAP.aucKey[3],
			s_stWiFiInstance.stWiFiInfo[ulIndex].stUserSetAP.aucKey[4],
			s_stWiFiInstance.stWiFiInfo[ulIndex].stUserSetAP.aucKey[5],
			s_stWiFiInstance.stWiFiInfo[ulIndex].stUserSetAP.aucKey[6],
			s_stWiFiInstance.stWiFiInfo[ulIndex].stUserSetAP.aucKey[7]);
#endif

	}

	/* Send connect command */
	vResult = VK_SYSTEM_Command( cmdBuf );
	if(vResult >= 0)
	{
		for(ulLoop = 0; ulLoop < 10; ulLoop++)
		{
			nPid = P_WIFI_GetPidByName((HINT8 *)"wpsenr");
			if(nPid != -1)
			{
				break;
			}
			VK_TASK_Sleep(20);
		}

		if(nPid != 0)
		{
			s_stWiFiInstance.stWiFiInfo[ulIndex].nWPSPid = nPid;
		}
		else
		{
			bErr = TRUE;
		}
	}
	else
	{
		bErr = TRUE;
	}

	if(bErr == TRUE)
	{
		vResult = VK_TIMER_EventAfter(0, P_WIFI_DisConnectCallback,
										&ulIndex, sizeof(ulIndex),
										(unsigned long*)&s_stWiFiInstance.ulDisConnectTimerId);
		if(vResult)
		{
			PrintError("[%s] VK_TIMER_EventAfter Err(0x%x)!!\n",__func__, vResult);
		}
	}

	return ;
}

static void P_WIFI_RunWpaSplctCmdForConnectWpsToAp(HUINT32 ulIndex, WIFI_EVENT_e eConnectType)
{
	char cmdBuf[256];
	HINT8 acPidFile[48],acConfigFile[48];

	if((eConnectType != WIFI_EVT_WPS_PBC_START) && (eConnectType != WIFI_EVT_WPS_PIN_START))
	{
		PrintError("[%s] Error : eConnectType %d \n", __FUNCTION__, eConnectType);
		goto ret;
	}

	VK_MEM_Memset(cmdBuf, 0x00, sizeof(cmdBuf));
	VK_MEM_Memset(acPidFile,0x00,sizeof(acPidFile));
	VK_MEM_Memset(acConfigFile,0x00,sizeof(acConfigFile));

	P_WIFI_MakeWpaSupplicantConfForWps(s_stWiFiInstance.stWiFiInfo[ulIndex].acIfName,
									&s_stWiFiInstance.stWiFiInfo[ulIndex].stUserSetAP);

	s_stWiFiInstance.stWiFiInfo[ulIndex].eStatus = WIFI_STATUS_CONNECTING;

	if(eConnectType == WIFI_EVT_WPS_PBC_START)
	{
		s_stWiFiInstance.stWiFiInfo[ulIndex].eWPS = DI_WIFI_WPS_PBC;
	}
	else
	{
		s_stWiFiInstance.stWiFiInfo[ulIndex].eWPS = DI_WIFI_WPS_PIN;
	}

	VK_snprintf( (char*)acPidFile, sizeof(acPidFile), WPA_SUPPLICANT_PID_FILE_PATTERN, s_stWiFiInstance.stWiFiInfo[ulIndex].acIfName );
	VK_snprintf( (char*)acConfigFile, sizeof(acConfigFile), WPA_SUPPLICANT_CONFIG_FILE_PATTERN, s_stWiFiInstance.stWiFiInfo[ulIndex].acIfName );

	VK_snprintf( cmdBuf, sizeof( cmdBuf ), "wpa_supplicant -Bdddt -Dwext -i %s -c %s", s_stWiFiInstance.stWiFiInfo[ulIndex].acIfName, acConfigFile);
	VK_SYSTEM_Command( cmdBuf );

	VK_MEM_Memset(cmdBuf, 0x00, sizeof(cmdBuf));
	if(eConnectType == WIFI_EVT_WPS_PBC_START)
	{
		VK_snprintf( cmdBuf, sizeof( cmdBuf ), "wpa_cli wps_pbc any");
	}
	else
	{
		VK_snprintf( cmdBuf, sizeof( cmdBuf ), "wpa_cli wps_pin any %d%d%d%d%d%d%d%d",
											s_stWiFiInstance.stWiFiInfo[ulIndex].stUserSetAP.aucKey[0],
											s_stWiFiInstance.stWiFiInfo[ulIndex].stUserSetAP.aucKey[1],
											s_stWiFiInstance.stWiFiInfo[ulIndex].stUserSetAP.aucKey[2],
											s_stWiFiInstance.stWiFiInfo[ulIndex].stUserSetAP.aucKey[3],
											s_stWiFiInstance.stWiFiInfo[ulIndex].stUserSetAP.aucKey[4],
											s_stWiFiInstance.stWiFiInfo[ulIndex].stUserSetAP.aucKey[5],
											s_stWiFiInstance.stWiFiInfo[ulIndex].stUserSetAP.aucKey[6],
											s_stWiFiInstance.stWiFiInfo[ulIndex].stUserSetAP.aucKey[7]);
	}
	VK_SYSTEM_Command( cmdBuf );

ret:
	return ;
}

static void P_WIFI_ConnectWpsToAp(HUINT32 ulIndex, WIFI_EVENT_e eConnectType)
{
	if((eConnectType != WIFI_EVT_WPS_PBC_START) && (eConnectType != WIFI_EVT_WPS_PIN_START))
	{
		PrintError("[%s] Error : eConnectType %d \n", __FUNCTION__, eConnectType);
		goto ret;
	}

	if(s_eWifiCtlInterface == WIFI_CTL_WL)
	{
		P_WIFI_RunWlCmdForConnectWpsToAp(ulIndex, eConnectType);
	}
	else if(s_eWifiCtlInterface == WIFI_CTL_WPA_SUPPLICANT)
	{
		P_WIFI_RunWpaSplctCmdForConnectWpsToAp(ulIndex, eConnectType);
	}
#if defined(CONFIG_QV940)
	else if(s_eWifiCtlInterface == WIFI_CTL_QCSAPI)
	{
		P_WIFI_RunQcsapiCmdForConnectWpsToAp(ulIndex, eConnectType);
	}
#endif
	else
	{
		PrintError("[%s] Error : s_eWifiCtlInterface %d \n", __FUNCTION__, s_eWifiCtlInterface);
		goto ret;
	}

ret:
	return ;

}

static void P_WIFI_RunWlCmdForCheckConnectStatus(HUINT32 ulIndex, HUINT32 ulMsgTime)
{
	WIFI_MSG_t	WiFiMsg;
	char acWlConnectStatusFileName[64];
	char acWlKeyStatusFileName[64];
	char acWlCmdBuf[96];
	HUINT32 ulNum = 0;
	HINT32 nRet = DRV_ERR;
	HINT32	nVkErr = 0;

	UNUSED(ulMsgTime);

	if (s_stWiFiInstance.stWiFiInfo[ulIndex].eStatus >= WIFI_STATUS_DISCONNECTED)
	{
		/* PrintDebug("\n(*) WIFI_STATUS_MONITOR : Not Connected (Status = %d)\n",s_stWiFiInstance.stWiFiInfo[ulIndex].eStatus); */
		goto ret;
	}

	VK_MEM_Memset(acWlConnectStatusFileName,0x00,sizeof(acWlConnectStatusFileName));
	VK_MEM_Memset(acWlKeyStatusFileName,0x00,sizeof(acWlKeyStatusFileName));
	VK_MEM_Memset(acWlCmdBuf,0x00,sizeof(acWlCmdBuf));

	/* run "wl status" */
	VK_snprintf(acWlConnectStatusFileName, sizeof(acWlConnectStatusFileName), WL_CONNECT_STATUS_FILE_PATTERN, s_stWiFiInstance.stWiFiInfo[ulIndex].acIfName);
	VK_snprintf(acWlCmdBuf, sizeof(acWlCmdBuf), "wl status > %s", acWlConnectStatusFileName);
	VK_SYSTEM_Command(acWlCmdBuf);

	/* run "wl sup_auth_status" */
	VK_snprintf(acWlKeyStatusFileName, sizeof(acWlKeyStatusFileName), WL_KEY_STATUS_FILE_PATTERN, s_stWiFiInstance.stWiFiInfo[ulIndex].acIfName);
	VK_snprintf(acWlCmdBuf, sizeof(acWlCmdBuf), "wl sup_auth_status > %s", acWlKeyStatusFileName);
	VK_SYSTEM_Command(acWlCmdBuf);

	if(s_stWiFiInstance.stWiFiInfo[ulIndex].eStatus == WIFI_STATUS_CONNECTING
		|| s_stWiFiInstance.stWiFiInfo[ulIndex].eStatus == WIFI_STATUS_RECONNECTING)
	{
		static HUINT32 sCnt[WIFI_MAX_NUM] = {0};
		HUINT8 ucMAC[6];

		VK_MEM_Memset(ucMAC, 0x0, 6);
		nRet = P_GetConnectStatus(acWlConnectStatusFileName,ucMAC);
		ulNum = ucMAC[0]+ucMAC[1]+ucMAC[2]+ucMAC[3]+ucMAC[4]+ucMAC[5];
		PrintDebug("\n(*) P_WIFI_Task]WIFI_STATUS_CONNECTING - Status = %d (MAC : %d)\n",nRet,ulNum);
		if(nRet == DRV_ERR || ulNum == 0 || ulNum == 1530) /* 0:0:0:0:0:0 or FF:FF:FF:FF:FF:FF */
		{
			sCnt[ulIndex] = 0;
		}
		else
		{
			sCnt[ulIndex]++;
		}

		if(sCnt[ulIndex] >= 3)
		{
			/* in case of the security type is WPA/WPA2, we have to check key exchange status */
			if (s_stWiFiInstance.stWiFiInfo[ulIndex].stUserSetAP.eSecureType == DI_WIFI_SECURE_WPA ||
				s_stWiFiInstance.stWiFiInfo[ulIndex].stUserSetAP.eSecureType == DI_WIFI_SECURE_WPA2 ||
				s_stWiFiInstance.stWiFiInfo[ulIndex].stUserSetAP.eSecureType == DI_WIFI_SECURE_WPA_WPA2)
			{
				nRet = P_GetKeyStatus(acWlKeyStatusFileName);
				if (nRet == DRV_ERR)
				{
					sCnt[ulIndex] = 0;
					goto ret;
				}
			}

			/*In case of the WPS type is PBC or PIN, We have to check AP Information*/
			if( s_stWiFiInstance.stWiFiInfo[ulIndex].eWPS != DI_WIFI_WPS_NONE)
			{
				/* WPS 로 연결 된 경우 AP와 연결 된 정보가 필요하다. */
				/* WPS 에서 실제 연결되지 않은 경우는 Retry로 변경..*/
				if(P_WIFI_GetWPSAPInfo(ulIndex) != DRV_OK)
				{
					goto ret;
				}
				VK_memcpy(s_stWiFiInstance.stWiFiInfo[0].stUserSetAP.aucMAC,ucMAC,6);
			}

			if(s_stWiFiInstance.ulDisConnectTimerId)
			{
				VK_TIMER_Cancel(s_stWiFiInstance.ulDisConnectTimerId);
				s_stWiFiInstance.ulDisConnectTimerId = (HUINT32)NULL;
			}

			sCnt[ulIndex] = 0;
			VK_memcpy(s_stWiFiInstance.stWiFiInfo[ulIndex].aucMAC,ucMAC,6);

			WiFiMsg.ulIndex = ulIndex;
			WiFiMsg.eMsg = WIFI_EVT_CONNECT_OK;
#if defined(CONFIG_SWB_B70_WIFI)
			VK_SYSTEM_Command("wl phy_watchdog 0");
			PrintDebug("[%s]_%d %s\n",__func__,__LINE__,"wl phy_watchdog 0");
#endif
			nVkErr = VK_MSG_SendTimeout(s_stWiFiInstance.ulMsg, &WiFiMsg, sizeof(WIFI_MSG_t),0);
			if(nVkErr)
			{
				PrintError("[%s] VK_MSG_Send Err(0x%x)!!\n",__func__,nVkErr);
			}
			goto ret;
		}
		else
		{
			if( s_stWiFiInstance.stWiFiInfo[ulIndex].eWPS == DI_WIFI_WPS_PIN)
			{
				/* AP에서 먼저 세팅해야만 됨..*/
				nRet = P_WIFI_GetWPS_APStatus();
				if(nRet == 1)
				{
					WiFiMsg.ulIndex = ulIndex;
					WiFiMsg.eMsg = WIFI_EVT_WPS_PIN_START;
					nVkErr = VK_MSG_SendTimeout(s_stWiFiInstance.ulMsg, &WiFiMsg, sizeof(WIFI_MSG_t),0);
					if(nVkErr)
					{
						PrintError("[%s] VK_MSG_Send Err(0x%x)!!\n", __func__, nVkErr);
					}
					goto ret;
				}
			}

			goto ret;
		}
	}
	else if(s_stWiFiInstance.stWiFiInfo[ulIndex].eStatus == WIFI_STATUS_CONNECTED)
	{
		/* Connection Monitor case */
		HBOOL bConnect = FALSE;
		HUINT8 ucMAC[6];

		nRet = P_GetConnectStatus(acWlConnectStatusFileName,ucMAC);
		ulNum = ucMAC[0]+ucMAC[1]+ucMAC[2]+ucMAC[3]+ucMAC[4]+ucMAC[5];
		if (nRet == DRV_ERR || ulNum == 0 || ulNum == 1530)
		{
			bConnect = FALSE;
		}
		else
		{
			bConnect = TRUE;
			/* in case of the security type is WPA/WPA2, we have to check key exchange status */
			if (s_stWiFiInstance.stWiFiInfo[ulIndex].stUserSetAP.eSecureType == DI_WIFI_SECURE_WPA ||
				s_stWiFiInstance.stWiFiInfo[ulIndex].stUserSetAP.eSecureType == DI_WIFI_SECURE_WPA2 ||
				s_stWiFiInstance.stWiFiInfo[ulIndex].stUserSetAP.eSecureType == DI_WIFI_SECURE_WPA_WPA2)
			{
				nRet = P_GetKeyStatus(acWlKeyStatusFileName);
				if (nRet == DRV_ERR)
				{
					bConnect = FALSE;
				}
			}
		}

		if(bConnect != TRUE)
		{
			PrintDebug("\n(*) WIFI_STATUS_MONITOR : WIFI_STATUS_RECONNECTING (MAC : %d) \n", ulNum);
			s_stWiFiInstance.stWiFiInfo[ulIndex].eStatus = WIFI_STATUS_RECONNECTING;
			if(s_stWiFiInstance.stWiFiInfo[ulIndex].stCallbackInfo[DI_WIFI_NOTIFY_EVT_CONNECT_FAIL].pfnCallback)
			{
				DI_WIFI_CALLBACK_DATA_t stCallbackData;
				stCallbackData.ulDevId= s_stWiFiInstance.stWiFiInfo[ulIndex].ulDevId;
				stCallbackData.eEvent = DI_WIFI_NOTIFY_EVT_CONNECT_FAIL;
				(*s_stWiFiInstance.stWiFiInfo[ulIndex].stCallbackInfo[DI_WIFI_NOTIFY_EVT_CONNECT_FAIL].pfnCallback)(&stCallbackData);
			}
		}

		PrintDebug("\n(*) WIFI_STATUS_MONITOR : WIFI_STATUS_CONNECTED (MAC : %d) \n", ulNum);
		goto ret;
	}

ret :
	return ;

}

static void P_WIFI_RunWpaSplctCmdForCheckConnectStatus(HUINT32 ulIndex, HUINT32 ulMsgTime)
{
	DRV_Error drvResult = DRV_OK;
	HINT32	nVkErr = 0;
	HINT32 nRet = DRV_ERR;
	HUINT32 ulNum = 0;
	WIFI_MSG_t	WiFiMsg;
	char wpaStatusFileName[64];
	char cmdWpaStatusBuf[128];

	UNUSED(ulMsgTime);

	if (s_stWiFiInstance.stWiFiInfo[ulIndex].eStatus >= WIFI_STATUS_DISCONNECTED)
	{
		/* PrintDebug("\n(*) WIFI_STATUS_MONITOR : Not Connected (Status = %d)\n",s_stWiFiInstance.stWiFiInfo[ulIndex].eStatus); */
		goto ret;
	}

	VK_MEM_Memset(wpaStatusFileName,0x00,sizeof(wpaStatusFileName));
	VK_MEM_Memset(cmdWpaStatusBuf,0x00,sizeof(cmdWpaStatusBuf));

	VK_snprintf(wpaStatusFileName, sizeof(wpaStatusFileName), WPA_CLI_STATUS_FILE_PATTERN, s_stWiFiInstance.stWiFiInfo[ulIndex].acIfName);
	VK_snprintf(cmdWpaStatusBuf, sizeof(cmdWpaStatusBuf), "wpa_cli status > %s", wpaStatusFileName);

	VK_SYSTEM_Command(cmdWpaStatusBuf);

	if(s_stWiFiInstance.stWiFiInfo[ulIndex].eStatus == WIFI_STATUS_CONNECTING
		|| s_stWiFiInstance.stWiFiInfo[ulIndex].eStatus == WIFI_STATUS_RECONNECTING)
	{
		static HUINT32 sCnt[WIFI_MAX_NUM] = {0};
		HUINT8 ucMAC[6];

		VK_MEM_Memset(ucMAC, 0x0, 6);
		drvResult = P_WIFI_IsConnect(wpaStatusFileName, ucMAC);
		ulNum = ucMAC[0]+ucMAC[1]+ucMAC[2]+ucMAC[3]+ucMAC[4]+ucMAC[5];
		PrintDebug("\n(*) P_WIFI_Task]WIFI_STATUS_CONNECTING - Status = %d %d \n", drvResult, ulNum);
		if(drvResult == DRV_ERR || ulNum == 0 || ulNum == 1530) /* 0:0:0:0:0:0 or FF:FF:FF:FF:FF:FF */
		{
			sCnt[ulIndex] = 0;
		}
		else
		{
			sCnt[ulIndex]++;
		}

		if(sCnt[ulIndex] >= 3)
		{
			HINT32 nRet = DRV_ERR;
			HUINT32 nLoop;

			nRet = P_GetConnectStatusFromStatusFile(wpaStatusFileName);
			if (nRet == DRV_ERR)
			{
				sCnt[ulIndex] = 0;
				goto ret;
			}

			/*In case of the WPS type is PBC or PIN, We have to check AP Information*/
			// TODO

			if(s_stWiFiInstance.ulDisConnectTimerId)
			{
				VK_TIMER_Cancel(s_stWiFiInstance.ulDisConnectTimerId);
				s_stWiFiInstance.ulDisConnectTimerId = (HUINT32)NULL;
			}

			sCnt[ulIndex] = 0;

			/* Compare ESSID */
			VK_memcpy(s_stWiFiInstance.stWiFiInfo[ulIndex].aucMAC,ucMAC,6);
#if 1
			for(nLoop = 0 ; nLoop < s_stWiFiInstance.stWiFiInfo[ulIndex].ulNumOfAPs ; nLoop++)
			{
				if ( !VK_strncmp((char *)s_stWiFiInstance.stWiFiInfo[ulIndex].stUserSetAP.aucESSID,(char*)s_stWiFiInstance.stWiFiInfo[ulIndex].astAPList[nLoop].aucESSID,
							sizeof(s_stWiFiInstance.stWiFiInfo[ulIndex].stUserSetAP.aucESSID)) )
				{
					break;
				}
			}

			if(nLoop < WIFI_AP_MAX)
			{
				if(nLoop == s_stWiFiInstance.stWiFiInfo[ulIndex].ulNumOfAPs)
				{
					VK_memcpy(&s_stWiFiInstance.stWiFiInfo[ulIndex].astAPList[nLoop],
							&s_stWiFiInstance.stWiFiInfo[ulIndex].stUserSetAP,
							sizeof(s_stWiFiInstance.stWiFiInfo[ulIndex].stUserSetAP));
				}
				else
				{
					s_stWiFiInstance.stWiFiInfo[ulIndex].astAPList[nLoop].eAuthenType = s_stWiFiInstance.stWiFiInfo[ulIndex].stUserSetAP.eAuthenType;
					s_stWiFiInstance.stWiFiInfo[ulIndex].astAPList[nLoop].eEncryptType = s_stWiFiInstance.stWiFiInfo[ulIndex].stUserSetAP.eEncryptType;
					s_stWiFiInstance.stWiFiInfo[ulIndex].astAPList[nLoop].eSecureType = s_stWiFiInstance.stWiFiInfo[ulIndex].stUserSetAP.eSecureType;
				}
				VK_memcpy(s_stWiFiInstance.stWiFiInfo[ulIndex].astAPList[nLoop].aucMAC,ucMAC,6);
				s_stWiFiInstance.stWiFiInfo[ulIndex].ulNumOfAPs++;
			}
#endif
			/* Send WIFI_EVT_CONNECT_OK to Task */
			WiFiMsg.ulIndex = ulIndex;
			WiFiMsg.eMsg = WIFI_EVT_CONNECT_OK;
			nVkErr = VK_MSG_SendTimeout(s_stWiFiInstance.ulMsg, &WiFiMsg, sizeof(WIFI_MSG_t),0);
			if(nVkErr)
			{
				PrintError("[%s] VK_MSG_Send Err(0x%x)!!\n",__func__,nVkErr);
			}
			goto ret;
		}
		else
		{
			goto ret;
		}
	}
	else if(s_stWiFiInstance.stWiFiInfo[ulIndex].eStatus == WIFI_STATUS_CONNECTED)
	{
		HBOOL bConnect = FALSE;
		HUINT8 ucMAC[6];

		drvResult = P_WIFI_IsConnect(wpaStatusFileName, ucMAC);
		ulNum = ucMAC[0]+ucMAC[1]+ucMAC[2]+ucMAC[3]+ucMAC[4]+ucMAC[5];
		if(drvResult == DRV_ERR || ulNum == 0 || ulNum == 1530) /* 0:0:0:0:0:0 or FF:FF:FF:FF:FF:FF */
			{
				bConnect = FALSE;
			}
			else
			{
				nRet = P_GetConnectStatusFromStatusFile(wpaStatusFileName);
			if(nRet == DRV_OK)
				{
				bConnect = TRUE;
			}
			else
			{
				bConnect = FALSE;
				}
			}

		if(bConnect != TRUE)
		{
			PrintDebug("\n(*) WIFI_STATUS_MONITOR : WIFI_STATUS_RECONNECTING (MAC : %d) \n", ulNum);
			s_stWiFiInstance.stWiFiInfo[ulIndex].eStatus = WIFI_STATUS_RECONNECTING;
#if 1
			if(s_stWiFiInstance.stWiFiInfo[ulIndex].stCallbackInfo[DI_WIFI_NOTIFY_EVT_CONNECT_FAIL].pfnCallback)
		{
				DI_WIFI_CALLBACK_DATA_t stCallbackData;
				stCallbackData.ulDevId= s_stWiFiInstance.stWiFiInfo[ulIndex].ulDevId;
				stCallbackData.eEvent = DI_WIFI_NOTIFY_EVT_CONNECT_FAIL;
				(*s_stWiFiInstance.stWiFiInfo[ulIndex].stCallbackInfo[DI_WIFI_NOTIFY_EVT_CONNECT_FAIL].pfnCallback)(&stCallbackData);
			}
#else
			nVkErr = VK_TIMER_EventAfter(0,P_WIFI_DisConnectCallback,&ulIndex,sizeof(ulIndex), (unsigned long*)&s_stWiFiInstance.ulDisConnectTimerId);
			if(nVkErr)
			{
				PrintError("[%s] VK_TIMER_EventAfter Err(0x%x)!!\n",__func__,nVkErr);
			}
#endif
		}
		PrintDebug("\n(*) WIFI_STATUS_MONITOR : WIFI_STATUS_CONNECTED (MAC : %d) \n", ulNum);
		goto ret;
	}
ret :
	return ;
}


static void P_WIFI_CheckConnectStatus(HUINT32 ulIndex, HUINT32 ulMsgTime)
{
	if(s_eWifiCtlInterface == WIFI_CTL_WL)
	{
		P_WIFI_RunWlCmdForCheckConnectStatus(ulIndex, ulMsgTime);
	}
	else if(s_eWifiCtlInterface == WIFI_CTL_WPA_SUPPLICANT)
	{
		P_WIFI_RunWpaSplctCmdForCheckConnectStatus(ulIndex, ulMsgTime);
	}
#if defined(CONFIG_QV940)
    else if(s_eWifiCtlInterface == WIFI_CTL_QCSAPI)
    {
        P_WIFI_RunQcsCmdForCheckConnectStatus(ulIndex, ulMsgTime);
    }
#endif
	else
	{
		PrintError("[%s] WIFI is not controlled : s_eWifiCtlInterface (%d) \n", __FUNCTION__, s_eWifiCtlInterface);
	}

	return ;
}

static HBOOL P_WIFI_FindDevice(HINT8 *pDevName)
{
	HINT8		acBuff[1024] = {0, };
	FILE		*pFile = NULL;
	HINT8		acTmpBuff[1024] = {0, };
	HINT32 nVkErr = 0;

	pFile = fopen("/proc/net/wireless", "r");
	if(pFile == NULL)
	{
		return FALSE;
	}

	/* Success : use data from /proc/net/wireless */
	/* Eat 2 lines of header */
	fgets((char*)acBuff, sizeof(acBuff), pFile);
	fgets((char*)acBuff, sizeof(acBuff), pFile);

	while(fgets((char*)acBuff, sizeof(acBuff), pFile))
	{
		HINT8	*pcStart = acBuff;
		HINT8	*pcEnd;

		/* Skip empty or almost empty lines. It seems that in some
		 * cases fgets return a line with only a newline. */
		if((pcStart[0] == '\0') || (pcStart[1] == '\0'))
		{
			continue;
		}

		/* Skip leading spaces */
		while(isspace(*pcStart))
		{
			pcStart++;
		}

		/* Get name up to ": "
		 * Note : we compare to ": " to make sure to process aliased interfaces
		 * properly. Doesn't work on /proc/net/dev, because it doesn't guarantee
		 * a ' ' after the ':'*/
		pcEnd = (HINT8*)VK_strstr((char*)pcStart, (char*)": ");

		/* Not found ??? To big ??? */
		if((pcEnd == NULL) || (((pcEnd - pcStart) + 1) > IFNAMSIZ + 1))
		{
			continue;
		}
		VK_MEM_Memset(acTmpBuff,0,1024);
		VK_memcpy(acTmpBuff, pcStart, (pcEnd - pcStart));
		/* add & delete */
	}
	fclose(pFile);

	nVkErr = VK_memcmp(pDevName,acTmpBuff,IFNAMSIZ);
	if(nVkErr != 0)
	{
		return FALSE;
	}

	return TRUE;

}

static void P_WIFI_Task(void *pvParam)
{
	DRV_Error dResult = DRV_OK;
	int			vkMsg;
	WIFI_MSG_t	WiFiMsg;
	char cmdBuf[256] = { 0 };
	char devName[256] = { 0 };
	HBOOL bIsDev = FALSE;
	HUINT32 ulLoop = 0;
	HUINT32 ulIndex = 0;
#if defined(CONFIG_BCM43242)
	char wlanHwAddrStr[20] = { 0 };
#endif
#if defined (CONFIG_BCM43567)
	HCHAR acKoPath[80] = {0,};
	HCHAR acFwPath[64] = {0,};
	HCHAR acNvPath[64] = {0,};
#endif
#if defined(CONFIG_SUPPORT_SECDMA)
	NEXUS_Error nRet = NEXUS_SUCCESS;
	NEXUS_SecurityPciEMaxWindowSizeSettings_t stWindowSizeSetting;
	NEXUS_MemoryAllocationSettings stMemAllocSettings;
	HUINT32 *pulMemOffset = NULL;
#if defined(CONFIG_WIFI_SCRIPT_TEST)
	FILE *fp;
	HCHAR buf[8];
#endif
#endif
	pvParam = pvParam;

	while (1)
	{
		vkMsg = VK_MSG_ReceiveTimeout( s_stWiFiInstance.ulMsg, &WiFiMsg, sizeof(WIFI_MSG_t), WIFI_CONNECT_CHECK );
		if( vkMsg != VK_OK )
		{
			if ( vkMsg == VK_TIMEOUT )
			{
				HUINT32	ulTmpLoop = 0;
				HUINT32	ulTmpIndex = 0;
				for (ulTmpLoop = 0 ; ulTmpLoop < WIFI_MAX_NUM; ulTmpLoop++)
				{
					/* check device id */
					if (s_stWiFiInstance.stWiFiInfo[ulTmpLoop].ulDevId != 0xFF)
					{
						ulTmpIndex = P_WIFI_GetIndexbyDevId(s_stWiFiInstance.stWiFiInfo[ulTmpLoop].ulDevId);
						if (s_stWiFiInstance.stWiFiInfo[ulTmpIndex].eDongleStatus == WIFI_DONGLE_INSERTED)
						{
							P_WIFI_CheckConnectStatus(ulTmpIndex, 0);
						}
					}
				}
			}
			else
			{
				/* VK_ERROR */
				PrintError("[%s] VK_MSG_ReceiveTimeout vkMsg = %d \n",__func__,WiFiMsg.eMsg);
			}
			continue;
		}

		switch (WiFiMsg.eMsg)
		{
			case WIFI_EVT_INSERT:
				PrintDebug("\n(*) P_WIFI_Task]WIFI_EVT_INSERT - Index : %d ,Device ID : %d ,Driver Name : %s IfName : %s \n",
					ulLoop,s_stWiFiInstance.stWiFiInfo[ulLoop].ulDevId, WiFiMsg.acDevName,s_stWiFiInstance.stWiFiInfo[ulLoop].acIfName);

				/* loading kernel driver */
				if ( !VK_strncmp("rt3070sta", (char*)WiFiMsg.acDevName,VK_strlen("rt3070sta") ))
				{
					VK_snprintf( devName, sizeof( devName ), "%s", WiFiMsg.acDevName );
					s_eWifiCtlInterface = WIFI_CTL_WPA_SUPPLICANT;
				}
				else if ( !VK_strncmp("rt35xx", (char*)WiFiMsg.acDevName,VK_strlen("rt35xx") ))
				{
					//VK_snprintf( devName, sizeof( devName ), "%s", "rt3572sta" );
					VK_snprintf( devName, sizeof( devName ), "%s", "rt5572sta" );
					s_eWifiCtlInterface = WIFI_CTL_WPA_SUPPLICANT;
				}
				else if ( !VK_strncmp("rt55xx", (char*)WiFiMsg.acDevName,VK_strlen("rt55xx") ))
				{
					VK_snprintf( devName, sizeof( devName ), "%s", "rt5572sta" );
					s_eWifiCtlInterface = WIFI_CTL_WPA_SUPPLICANT;
				}
				else if ( !VK_strncmp("BCM43236", (char*)WiFiMsg.acDevName,(size_t)VK_strlen("BCM43236")))
				{
#if defined(CONFIG_NETAPP_WIFI) && defined(CONFIG_NETAPP_WIFI_AARDVARK)
					VK_snprintf( devName, sizeof( devName ), "%s", "wl" );
#elif defined(CONFIG_WIFI_AARDVARK)
					VK_snprintf( devName, sizeof( devName ), "%s", "wl" );
#else
					VK_snprintf( devName, sizeof( devName ), "%s", "dhd" );
#endif
					s_eWifiCtlInterface = WIFI_CTL_WL;
				}
#if defined(CONFIG_BCM43242)
				else if ( !VK_strncmp("BCM43242", (char*)WiFiMsg.acDevName,(size_t)VK_strlen("BCM43242")))
				{
					VK_snprintf( devName, sizeof( devName ), "%s", "wl" );
					s_eWifiCtlInterface = WIFI_CTL_WL;
				}
#endif
				else if ( !VK_strncmp(BCM43228_CONTROLLER, (char*)WiFiMsg.acDevName,(size_t)VK_strlen(BCM43228_CONTROLLER)))
				{
					VK_snprintf( devName, sizeof( devName ), "%s", "wl" );
					s_eWifiCtlInterface = WIFI_CTL_WL;
				}
#if defined(CONFIG_BCM43567)
				else if ( !VK_strncmp(BCM_WIFI_CONTROLLER, (char*)WiFiMsg.acDevName,(size_t)VK_strlen(BCM_WIFI_CONTROLLER)))
				{
					VK_snprintf( devName, sizeof( devName ), "%s", "dhd" );
					s_eWifiCtlInterface = WIFI_CTL_WL;
				}
#endif
#if defined(CONFIG_MT7601)
				else if ( !VK_strncmp("MT7601", (char*)WiFiMsg.acDevName,VK_strlen("MT7601") ))
				{
					VK_snprintf( devName, sizeof( devName ), "%s", "mt7601Usta" );
					s_eWifiCtlInterface = WIFI_CTL_WPA_SUPPLICANT;
				}
#endif
#if defined(CONFIG_RTL8188)
				else if ( !VK_strncmp("RTL8188", (char*)WiFiMsg.acDevName,VK_strlen("RTL8188") ))
				{
					VK_snprintf( devName, sizeof( devName ), "%s", "rtl8188eu" );
					s_eWifiCtlInterface = WIFI_CTL_WPA_SUPPLICANT;
				}
#endif
#if defined(CONFIG_QV940)
				else if ( !VK_strncmp(QV940_DEVICE_NAME, (char*)WiFiMsg.acDevName,(size_t)VK_strlen(QV940_DEVICE_NAME)))
				{
					s_eWifiCtlInterface = WIFI_CTL_QCSAPI;
					goto insert;
				}
#endif
				else
				{
					s_eWifiCtlInterface = WIFI_CTL_NONE;
					break;
				}

#if 1 /* __HUMAX_CODE__ - 140417 mskim (redmine #88454) */
                VK_memcpy(wifiDevName, devName, sizeof(devName));
#endif
#if (defined(CONFIG_RT3572) || defined(CONFIG_RT5572)) /* __HUMAX_CODE__ - 140330 mskim (redmine #88454) */
			if ( !VK_strncmp("rt35xx", (char*)WiFiMsg.acDevName,VK_strlen("rt35xx")) || !VK_strncmp("rt55xx", (char*)WiFiMsg.acDevName,VK_strlen("rt55xx")) )
                {
                    // rtutil5572sta.ko
    				VK_snprintf( cmdBuf, sizeof( cmdBuf ), "modprobe rtutil5572sta");
    				VK_SYSTEM_Command( cmdBuf );
    				VK_TASK_Sleep(500);

                    // rt5572sta.ko
    				VK_snprintf( cmdBuf, sizeof( cmdBuf ), "modprobe %s", devName );
    				VK_SYSTEM_Command( cmdBuf );
    				VK_TASK_Sleep(500);

                    // rtnet5572sta.ko
    				VK_snprintf( cmdBuf, sizeof( cmdBuf ), "modprobe rtnet5572sta");
    				VK_SYSTEM_Command( cmdBuf );
    				VK_TASK_Sleep(500);
    			}
    			else
			    {
    				VK_snprintf( cmdBuf, sizeof( cmdBuf ), "modprobe %s", devName );
    				VK_SYSTEM_Command( cmdBuf );
    				VK_TASK_Sleep(500);
                }

#elif (defined(CONFIG_MT7601))
			if ( !VK_strncmp("MT7601", (char*)WiFiMsg.acDevName,VK_strlen("MT7601")) )
			{
						VK_snprintf( cmdBuf, sizeof( cmdBuf ), "modprobe mtutil7601Usta");
    				VK_SYSTEM_Command( cmdBuf );
    				VK_TASK_Sleep(500);

    				VK_snprintf( cmdBuf, sizeof( cmdBuf ), "modprobe %s", devName );
    				VK_SYSTEM_Command( cmdBuf );
    				VK_TASK_Sleep(500);

    				VK_snprintf( cmdBuf, sizeof( cmdBuf ), "modprobe mtnet7601Usta");
    				VK_SYSTEM_Command( cmdBuf );
    				VK_TASK_Sleep(500);
    			}
    			else
			    {
    				VK_snprintf( cmdBuf, sizeof( cmdBuf ), "modprobe %s", devName );
    				VK_SYSTEM_Command( cmdBuf );
    				VK_TASK_Sleep(500);
                }
#else
#if defined(CONFIG_BCM43567)
#if defined(CONFIG_SUPPORT_SECDMA)
				NEXUS_Security_GetDefaultPciEMaxWindowSizeSettings(&stWindowSizeSetting);
				stWindowSizeSetting.vkl = NEXUS_SecurityVirtualKeyladderID_eVKL7;

				VK_MEM_Memcpy(stWindowSizeSetting.signedCommand, s_aucSignedCommand, sizeof(stWindowSizeSetting.signedCommand));

				NEXUS_Security_SetPciEMaxWindowSize (&stWindowSizeSetting);

				NEXUS_Memory_GetDefaultAllocationSettings(&stMemAllocSettings);
				stMemAllocSettings.alignment = 0x1000;

				nRet = NEXUS_Memory_Allocate(BCM_WIFI_SECDMA_MEM_SIZE, &stMemAllocSettings, (void *)&pulMemOffset);
				if (!nRet)
				{
					*pulMemOffset = NEXUS_AddrToOffset(pulMemOffset);

					NEXUS_Security_SetPciERestrictedRange(*pulMemOffset, BCM_WIFI_SECDMA_MEM_SIZE, (unsigned)0);

#if defined(CONFIG_WIFI_SCRIPT_TEST)
					fp = fopen(BCM_WIFI_TEST_DRV_TYPE_PATH, "r");
					if (fp != NULL)
					{
						fgets(buf, 8, fp);
						fclose(fp);
						if (VK_strncmp("mfg", buf, 3) == 0)
					    {
					        PrintDebug("[%s] BCM WIFI TEST driver type is mfg\n", __FUNCTION__);
							VK_snprintf(acFwPath, sizeof(acFwPath)-1, BCM_WIFI_TEST_MFG_FW_PATH, BCM_WIFI_CHIP_NAME);
					    }
						else if (VK_strncmp("prd", buf, 3) == 0)
						{
							PrintDebug("[%s] BCM WIFI TEST driver type is prd\n", __FUNCTION__);
							VK_snprintf(acFwPath, sizeof(acFwPath)-1, BCM_WIFI_TEST_PRD_FW_PATH, BCM_WIFI_CHIP_NAME);
						}
						else
						{
							PrintDebug("[%s] BCM TEST driver type is ???? \n", __FUNCTION__);
						}
					}
					else
				    {
						PrintError("[%s] Err fopen %s\n", __FUNCTION__, strerror( errno ));
				    }

					VK_snprintf(acKoPath, sizeof(acKoPath)-1, BCM_WIFI_TEST_KO_PATH, wifiDevName);
					VK_snprintf(acNvPath, sizeof(acNvPath)-1, BCM_WIFI_TEST_NV_PATH, BCM_WIFI_CHIP_NAME);
					VK_snprintf( cmdBuf, sizeof( cmdBuf ), "insmod %s firmware_path=%s nvram_path=%s iface_name=%s secdma_addr=0x%x secdma_size=0x%x", acKoPath, acFwPath, acNvPath, s_stWiFiInstance.stWiFiInfo[ulLoop].acIfName, *pulMemOffset, BCM_WIFI_SECDMA_MEM_SIZE);
#else
					VK_snprintf(acKoPath, sizeof(acKoPath)-1, BCM_WIFI_KO_PATH, LINUX_VERSION, LINUX_PATCHLEVEL, LINUX_SUBLEVEL, BCM_WIFI_CHIP_NAME, wifiDevName);
					VK_snprintf(acFwPath, sizeof(acFwPath)-1, BCM_WIFI_FW_PATH, BCM_WIFI_CHIP_NAME);
					VK_snprintf(acNvPath, sizeof(acNvPath)-1, BCM_WIFI_NV_PATH, BCM_WIFI_CHIP_NAME);
					VK_snprintf( cmdBuf, sizeof( cmdBuf ), "insmod %s firmware_path=%s nvram_path=%s iface_name=%s secdma_addr=0x%x secdma_size=0x%x", acKoPath, acFwPath, acNvPath, s_stWiFiInstance.stWiFiInfo[ulLoop].acIfName, *pulMemOffset, BCM_WIFI_SECDMA_MEM_SIZE);
#endif
				}
				else
				{
					PrintDebug("\n(*) P_WIFI_Task] Failed to allocate %d bytes from NEXUS %s \n", BCM_WIFI_SECDMA_MEM_SIZE);
				}
#else	/* CONFIG_SUPPORT_SECDMA */
				VK_snprintf(acKoPath, sizeof(acKoPath)-1, BCM_WIFI_KO_PATH, LINUX_VERSION, LINUX_PATCHLEVEL, LINUX_SUBLEVEL, BCM_WIFI_CHIP_NAME, wifiDevName);
				VK_snprintf(acFwPath, sizeof(acFwPath)-1, BCM_WIFI_FW_PATH, BCM_WIFI_CHIP_NAME);
				VK_snprintf(acNvPath, sizeof(acNvPath)-1, BCM_WIFI_NV_PATH, BCM_WIFI_CHIP_NAME);
				VK_snprintf( cmdBuf, sizeof( cmdBuf ), "insmod %s firmware_path=%s nvram_path=%s iface_name=%s", acKoPath, acFwPath, acNvPath, s_stWiFiInstance.stWiFiInfo[ulLoop].acIfName);
#endif	/* CONFIG_SUPPORT_SECDMA */
#else	/* BCM43567 */
				VK_snprintf( cmdBuf, sizeof( cmdBuf ), "modprobe %s", devName );
#endif	/* BCM43567 */
				VK_SYSTEM_Command( cmdBuf );

#if defined(CONFIG_SUPPORT_SECDMA)
				if(pulMemOffset != NULL)
				{
					/* Do not free NEXUS memory!! When SECDMA enabled, DHD driver use it during the operation not only FW downloading!! */
					// NEXUS_Memory_Free((void *)pulMemOffset);
				}
#endif
				VK_TASK_Sleep(500);
#endif

				VK_snprintf( cmdBuf, sizeof( cmdBuf ), "ifconfig %s down", s_stWiFiInstance.stWiFiInfo[ulLoop].acIfName );
				VK_SYSTEM_Command( cmdBuf );
				VK_TASK_Sleep(100);

#if defined(CONFIG_BCM43242)
				if ( !VK_strncmp("BCM43242", (char*)WiFiMsg.acDevName,(size_t)VK_strlen("BCM43242"))) {
					dResult = P_WIFI_GetHwAddr(wlanHwAddrStr);
					if( dResult == DI_ERR_OK )
					{
						PrintDebug("\n(*) P_WIFI_Task]WIFI_MAC_FROM_NVRAM %s \n", wlanHwAddrStr);
						if ( P_WIFI_IsValidHwAddr(wlanHwAddrStr) == 0 )
							VK_snprintf( cmdBuf, sizeof( cmdBuf ), "ifconfig %s hw ether '%s' up", s_stWiFiInstance.stWiFiInfo[ulLoop].acIfName, wlanHwAddrStr );
						else
							VK_snprintf( cmdBuf, sizeof( cmdBuf ), "ifconfig %s up", s_stWiFiInstance.stWiFiInfo[ulLoop].acIfName );
					}
					VK_SYSTEM_Command( cmdBuf );
					VK_TASK_Sleep(100);
				}
				else
#else
				VK_snprintf( cmdBuf, sizeof( cmdBuf ), "ifconfig %s up", s_stWiFiInstance.stWiFiInfo[ulLoop].acIfName );
				VK_SYSTEM_Command( cmdBuf );
				VK_TASK_Sleep(100);
#endif
				if(s_eWifiCtlInterface == WIFI_CTL_WL)
				{
					P_WIFI_SetBrcmUp();
#if defined(CONFIG_SWB_B70_WIFI)
					VK_MEM_Memset(cmdBuf, 0x0, sizeof(cmdBuf));
					VK_snprintf( cmdBuf, sizeof(cmdBuf), "wl country %s", BCM43228_CCODE);
					VK_SYSTEM_Command( cmdBuf );
#endif
					VK_SYSTEM_Command( "wl country" );
				}

				if(s_eWifiCtlInterface == WIFI_CTL_WPA_SUPPLICANT)
				{
#if defined(CONFIG_RT5572_WNA_DONGLE)
					VK_MEM_Memset(cmdBuf, 0x0, sizeof(cmdBuf));
					if(VK_strcmp(CONFIG_RT5572_WNA_DONGLE_CCODE, "AU")==0) {
						VK_snprintf( cmdBuf, sizeof(cmdBuf), "iwpriv %s e2p 38=0109", s_stWiFiInstance.stWiFiInfo[ulLoop].acIfName);
					} else if(VK_strcmp(CONFIG_RT5572_WNA_DONGLE_CCODE, "EU")==0) {
						VK_snprintf( cmdBuf, sizeof(cmdBuf), "iwpriv %s e2p 38=010C", s_stWiFiInstance.stWiFiInfo[ulLoop].acIfName);
					} else if(VK_strcmp(CONFIG_RT5572_WNA_DONGLE_CCODE, "Default")==0) {
						VK_snprintf( cmdBuf, sizeof(cmdBuf), "iwpriv %s e2p 38=FFFF", s_stWiFiInstance.stWiFiInfo[ulLoop].acIfName);
					}
					if(VK_strlen(cmdBuf) > 0)
						VK_SYSTEM_Command( cmdBuf );
#endif
					/* Open WPA_SUPLICANT command socket */
					if (s_nSocketFd < 0)
					{
						s_nSocketFd = socket(AF_INET, SOCK_DGRAM, 0);
						if (s_nSocketFd < 0)
						{
							PrintError("\n(*) P_WIFI_Task] WIFI_EVT_INSERT - Socket open fail! (%d)\n",s_nSocketFd);
						}

					}
				}

#if defined(CONFIG_QV940)
insert:
#endif
				s_stWiFiInstance.stWiFiInfo[ulLoop].eDongleStatus = WIFI_DONGLE_INSERTED;

				if(s_stWiFiInstance.stWiFiInfo[ulLoop].stCallbackInfo[DI_WIFI_NOTIFY_EVT_INSERTED].pfnCallback)
				{
					DI_WIFI_CALLBACK_DATA_t stCallbackData;
					stCallbackData.ulDevId= s_stWiFiInstance.stWiFiInfo[ulLoop].ulDevId;
					stCallbackData.eEvent = DI_WIFI_NOTIFY_EVT_INSERTED;
					stCallbackData.ulNumOfData = 0;
					stCallbackData.pvEventData = NULL;
					(*s_stWiFiInstance.stWiFiInfo[ulLoop].stCallbackInfo[DI_WIFI_NOTIFY_EVT_INSERTED].pfnCallback)(&stCallbackData);
				}
				break;

			case WIFI_EVT_EXTRACT:
				for( ulLoop = 0 ; ulLoop < WIFI_MAX_NUM; ulLoop++)
				{
					if(s_stWiFiInstance.stWiFiInfo[ulLoop].eDongleStatus == WIFI_DONGLE_INSERTED)
					{
						bIsDev = P_WIFI_FindDevice((HINT8 *)s_stWiFiInstance.stWiFiInfo[ulLoop].acIfName);
						if(bIsDev != TRUE)
						{
							break;
						}
					}
				}

				if(ulLoop == WIFI_MAX_NUM)
				{
					PrintError("Do not find to Removed WIFI Interface(%s)\n",WiFiMsg.acDevName);
          ulLoop = 0;
					break;
				}

				PrintDebug("\n(*) P_WIFI_Task]WIFI_EVT_EXTRACT - Index : %d ,Device ID : %d ,Driver Name : %s IfName : %s \n",
					ulLoop,s_stWiFiInstance.stWiFiInfo[ulLoop].ulDevId, WiFiMsg.acDevName, s_stWiFiInstance.stWiFiInfo[ulLoop].acIfName);

				if(s_stWiFiInstance.stWiFiInfo[ulIndex].eWPS == DI_WIFI_WPS_PBC || s_stWiFiInstance.stWiFiInfo[ulIndex].eWPS == DI_WIFI_WPS_PIN)
				{
					if(s_eWifiCtlInterface == WIFI_CTL_WL)
					{
						if(s_stWiFiInstance.stWiFiInfo[ulIndex].nWPSPid != -1)
						{
							kill( s_stWiFiInstance.stWiFiInfo[ulIndex].nWPSPid, SIGKILL );
							s_stWiFiInstance.stWiFiInfo[ulIndex].nWPSPid = -1;
						}
					}
					s_stWiFiInstance.stWiFiInfo[ulIndex].eWPS = DI_WIFI_WPS_NONE;
				}

				if(s_eWifiCtlInterface == WIFI_CTL_WL)
				{
					P_WIFI_SetBrcmDown();
				}

				if(s_eWifiCtlInterface == WIFI_CTL_WPA_SUPPLICANT)
				{
					/* Kill WPA supplicant */
					P_WIFI_KillWpaSupplicant((HINT8 *)s_stWiFiInstance.stWiFiInfo[ulIndex].acIfName);

					/* Close socket */
					if(s_nSocketFd != -1)
					{
						close(s_nSocketFd);
						s_nSocketFd = -1;
					}
				}

				if ( !VK_strncmp("rt3070sta", (char*)WiFiMsg.acDevName,VK_strlen("rt3070sta") ))
				{
					VK_snprintf( devName, sizeof( devName ), "%s", WiFiMsg.acDevName );
				}
				else if ( !VK_strncmp("rt35xx", (char*)WiFiMsg.acDevName,VK_strlen("rt35xx") ))
				{
					//VK_snprintf( devName, sizeof( devName ), "%s", "rt3572sta" );
					VK_snprintf( devName, sizeof( devName ), "%s", "rt5572sta" );
				}
				else if ( !VK_strncmp("rt55xx", (char*)WiFiMsg.acDevName,VK_strlen("rt55xx") ))
				{
					VK_snprintf( devName, sizeof( devName ), "%s", "rt5572sta" );
				}
				else if ( !VK_strncmp("BCM43236", (char*)WiFiMsg.acDevName,(size_t)VK_strlen("BCM43236")))
				{
#if defined(CONFIG_NETAPP_WIFI) && defined(CONFIG_NETAPP_WIFI_AARDVARK)
					VK_snprintf( devName, sizeof( devName ), "%s", "wl" );
#elif defined(CONFIG_WIFI_AARDVARK)
					VK_snprintf( devName, sizeof( devName ), "%s", "wl" );
#else
					VK_snprintf( devName, sizeof( devName ), "%s", "dhd" );
#endif
				}
				else if ( !VK_strncmp(BCM43228_CONTROLLER, (char*)WiFiMsg.acDevName,(size_t)VK_strlen(BCM43228_CONTROLLER)))
				{
					VK_snprintf( devName, sizeof( devName ), "%s", "wl" );
				}
#if defined(CONFIG_BCM43567)
				else if ( !VK_strncmp(BCM_WIFI_CONTROLLER, (char*)WiFiMsg.acDevName,(size_t)VK_strlen(BCM_WIFI_CONTROLLER)))
				{
					VK_snprintf( devName, sizeof( devName ), "%s", "dhd" );
				}
#endif
#if defined(CONFIG_MT7601)
				else if ( !VK_strncmp("MT7601", (char*)WiFiMsg.acDevName,VK_strlen("MT7601") ))
				{
					VK_snprintf( devName, sizeof( devName ), "%s", "mt7601Usta" );
					s_eWifiCtlInterface = WIFI_CTL_WPA_SUPPLICANT;
				}
#endif
#if defined(CONFIG_RTL8188)
				else if ( !VK_strncmp("RTL8188", (char*)WiFiMsg.acDevName,VK_strlen("RTL8188") ))
				{
					VK_snprintf( devName, sizeof( devName ), "%s", "rtl8188eu" );
					s_eWifiCtlInterface = WIFI_CTL_WPA_SUPPLICANT;
				}
#endif
				else
				{
					break;
				}

#if (defined(CONFIG_RT3572) || defined(CONFIG_RT5572)) /* __HUMAX_CODE__ - 140330 mskim (redmine #88454) */
				if ( !VK_strncmp("rt35xx", (char*)WiFiMsg.acDevName,VK_strlen("rt35xx")) || !VK_strncmp("rt55xx", (char*)WiFiMsg.acDevName,VK_strlen("rt55xx")) )
                {
                    // rtnet5572sta.ko
    				VK_snprintf( cmdBuf, sizeof( cmdBuf ), "rmmod rtnet5572sta.ko");
    				VK_SYSTEM_Command( cmdBuf );
    				VK_TASK_Sleep(500);

                    // rt5572sta.ko
    				VK_snprintf( cmdBuf, sizeof( cmdBuf ), "rmmod %s", devName );
    				VK_SYSTEM_Command( cmdBuf );
    				VK_TASK_Sleep(500);

                    // rtutil5572sta.ko
    				VK_snprintf( cmdBuf, sizeof( cmdBuf ), "rmmod rtutil5572sta.ko");
    				VK_SYSTEM_Command( cmdBuf );
    				VK_TASK_Sleep(500);
    			}
    			else
			    {
			        VK_snprintf( cmdBuf, sizeof( cmdBuf ), "rmmod %s", devName );
    				VK_SYSTEM_Command( cmdBuf );
    				VK_TASK_Sleep(500);
			    }
#elif (defined(CONFIG_MT7601))
			PrintError("%s():%d [%s]\n", __func__, __LINE__, (char*)WiFiMsg.acDevName);
			if ( !VK_strncmp("MT7601", (char*)WiFiMsg.acDevName,VK_strlen("MT7601")) )
			{
				PrintError("%s():%d\n", __func__, __LINE__);
						VK_snprintf( cmdBuf, sizeof( cmdBuf ), "rmmod mtnet7601Usta");
    				VK_SYSTEM_Command( cmdBuf );
    				VK_TASK_Sleep(500);

       			VK_snprintf( cmdBuf, sizeof( cmdBuf ), "rmmod %s", devName );
    				VK_SYSTEM_Command( cmdBuf );
    				VK_TASK_Sleep(500);

        		VK_snprintf( cmdBuf, sizeof( cmdBuf ), "rmmod mtutil7601Usta");
    				VK_SYSTEM_Command( cmdBuf );
    				VK_TASK_Sleep(500);
			}
			else
			{
				PrintError("%s():%d\n", __func__, __LINE__);
    				VK_snprintf( cmdBuf, sizeof( cmdBuf ), "rmmod %s", devName );
    				VK_SYSTEM_Command( cmdBuf );
    				VK_TASK_Sleep(500);
			    }
#else
				VK_snprintf( cmdBuf, sizeof( cmdBuf ), "rmmod %s", devName );
				VK_SYSTEM_Command( cmdBuf );
				VK_TASK_Sleep(500);
#endif

				s_eWifiCtlInterface = WIFI_CTL_NONE;
				s_stWiFiInstance.stWiFiInfo[ulLoop].eDongleStatus = WIFI_DONGLE_EXTRACTED;
				s_stWiFiInstance.stWiFiInfo[ulLoop].eUserSetting = WIFI_STATUS_DISCONNECTED;
				s_stWiFiInstance.stWiFiInfo[ulLoop].eStatus = WIFI_STATUS_DISCONNECTED;

				if(s_stWiFiInstance.stWiFiInfo[ulLoop].stCallbackInfo[DI_WIFI_NOTIFY_EVT_EXTRACTED].pfnCallback)
				{
					DI_WIFI_CALLBACK_DATA_t stCallbackData;
					stCallbackData.ulDevId= s_stWiFiInstance.stWiFiInfo[ulLoop].ulDevId;
					stCallbackData.eEvent = DI_WIFI_NOTIFY_EVT_EXTRACTED;
					stCallbackData.ulNumOfData = 0;
					stCallbackData.pvEventData = NULL;
					(*s_stWiFiInstance.stWiFiInfo[ulLoop].stCallbackInfo[DI_WIFI_NOTIFY_EVT_EXTRACTED].pfnCallback)(&stCallbackData);
				}

				break;
			case WIFI_EVT_SCAN_START:
				ulIndex = WiFiMsg.ulIndex;
				s_stWiFiInstance.stWiFiInfo[ulIndex].bScanning = TRUE;

				dResult = P_WIFI_StartScanAp(ulIndex);
				if(dResult == DRV_OK)
				{
#if 0 /* Just for debugging */
					int i=0;

					for ( i = 0; i < s_stWiFiInstance.stWiFiInfo[ulIndex].ulNumOfAPs ; i++ )
					{
						VK_printf("  --------------- AP %2d Info -------------\n", i);
						VK_printf("  * ESSID  : %s\n", s_stWiFiInstance.stWiFiInfo[ulIndex].astAPList[i].aucESSID);
						VK_printf("  * MAC    : %d\n", s_stWiFiInstance.stWiFiInfo[ulIndex].astAPList[i].aucMAC[0]+s_stWiFiInstance.stWiFiInfo[ulIndex].astAPList[i].aucMAC[1]+s_stWiFiInstance.stWiFiInfo[ulIndex].astAPList[i].aucMAC[2]+s_stWiFiInstance.stWiFiInfo[ulIndex].astAPList[i].aucMAC[3]+s_stWiFiInstance.stWiFiInfo[ulIndex].astAPList[i].aucMAC[4]+s_stWiFiInstance.stWiFiInfo[ulIndex].astAPList[i].aucMAC[5]);
						VK_printf("  * SEC    : %d\n", s_stWiFiInstance.stWiFiInfo[ulIndex].astAPList[i].eSecureType);
						VK_printf("  * AUTH   : %d\n", s_stWiFiInstance.stWiFiInfo[ulIndex].astAPList[i].eAuthenType);
						VK_printf("  * ENC    : %d\n", s_stWiFiInstance.stWiFiInfo[ulIndex].astAPList[i].eEncryptType);
						VK_printf("  \n");
					}
#endif
					if(s_stWiFiInstance.stWiFiInfo[ulIndex].stCallbackInfo[DI_WIFI_NOTIFY_EVT_SCAN_SUCCESS].pfnCallback)
					{
						DI_WIFI_CALLBACK_DATA_t stCallbackData;
						stCallbackData.ulDevId= s_stWiFiInstance.stWiFiInfo[ulIndex].ulDevId;
						stCallbackData.eEvent = DI_WIFI_NOTIFY_EVT_SCAN_SUCCESS;
						stCallbackData.ulNumOfData = s_stWiFiInstance.stWiFiInfo[ulIndex].ulNumOfAPs;
						stCallbackData.pvEventData = s_stWiFiInstance.stWiFiInfo[ulIndex].astAPList;
						(*s_stWiFiInstance.stWiFiInfo[ulIndex].stCallbackInfo[DI_WIFI_NOTIFY_EVT_SCAN_SUCCESS].pfnCallback)(&stCallbackData);
					}
				}
				else
				{
					if(s_stWiFiInstance.stWiFiInfo[ulIndex].stCallbackInfo[DI_WIFI_NOTIFY_EVT_SCAN_FAIL].pfnCallback)
					{
						DI_WIFI_CALLBACK_DATA_t stCallbackData;
						stCallbackData.ulDevId= s_stWiFiInstance.stWiFiInfo[ulIndex].ulDevId;
						stCallbackData.eEvent = DI_WIFI_NOTIFY_EVT_SCAN_FAIL;
						(*s_stWiFiInstance.stWiFiInfo[ulIndex].stCallbackInfo[DI_WIFI_NOTIFY_EVT_SCAN_FAIL].pfnCallback)(&stCallbackData);
					}
				}

				s_stWiFiInstance.stWiFiInfo[ulIndex].bScanning = FALSE;
				break;

			case WIFI_EVT_SCAN_STOP:
				break;

			case WIFI_EVT_CONNECT_START:
				ulIndex = WiFiMsg.ulIndex;
				P_WIFI_ConnectToAp(ulIndex,
								s_stWiFiInstance.stWiFiInfo[ulIndex].acIfName,
								&s_stWiFiInstance.stWiFiInfo[ulIndex].stUserSetAP);
				break;

			case WIFI_EVT_WPS_PBC_START:
			case WIFI_EVT_WPS_PIN_START:
				P_WIFI_ConnectWpsToAp(WiFiMsg.ulIndex, WiFiMsg.eMsg);
				break;

			case WIFI_EVT_CONNECT_OK:
				ulIndex = WiFiMsg.ulIndex;
				s_stWiFiInstance.stWiFiInfo[ulIndex].eStatus = WIFI_STATUS_CONNECTED;
				if(s_stWiFiInstance.stWiFiInfo[ulIndex].stCallbackInfo[DI_WIFI_NOTIFY_EVT_CONNECT_SUCCESS].pfnCallback)
				{
					DI_WIFI_CALLBACK_DATA_t stCallbackData;
					stCallbackData.ulDevId= s_stWiFiInstance.stWiFiInfo[ulIndex].ulDevId;
					stCallbackData.eEvent = DI_WIFI_NOTIFY_EVT_CONNECT_SUCCESS;
					(*s_stWiFiInstance.stWiFiInfo[ulIndex].stCallbackInfo[DI_WIFI_NOTIFY_EVT_CONNECT_SUCCESS].pfnCallback)(&stCallbackData);
				}

				break;

			case WIFI_EVT_DISCONNECT:
			case WIFI_EVT_CONNECT_FAIL:
				ulIndex = WiFiMsg.ulIndex;

				if(s_stWiFiInstance.stWiFiInfo[ulIndex].eStatus != WIFI_STATUS_DISCONNECTED)
				{
					VK_MEM_Memset(s_stWiFiInstance.stWiFiInfo[ulIndex].aucMAC, 0x00, sizeof(s_stWiFiInstance.stWiFiInfo[ulIndex].aucMAC));

					if(s_eWifiCtlInterface == WIFI_CTL_WL)
					{
						PrintError("Current Status = %d \n",s_stWiFiInstance.stWiFiInfo[ulIndex].eStatus);
						P_WIFI_SetBrcmDown();
						P_WIFI_SetBrcmUp();
					}

					if(s_stWiFiInstance.stWiFiInfo[ulIndex].eWPS == DI_WIFI_WPS_PBC || s_stWiFiInstance.stWiFiInfo[ulIndex].eWPS == DI_WIFI_WPS_PIN)
					{
						if(s_eWifiCtlInterface == WIFI_CTL_WL)
						{
							if(s_stWiFiInstance.stWiFiInfo[ulIndex].nWPSPid != -1)
							{
								kill( s_stWiFiInstance.stWiFiInfo[ulIndex].nWPSPid, SIGKILL );
								s_stWiFiInstance.stWiFiInfo[ulIndex].nWPSPid = -1;
							}
						}
						s_stWiFiInstance.stWiFiInfo[ulIndex].eWPS = DI_WIFI_WPS_NONE;
					}

					s_stWiFiInstance.stWiFiInfo[ulIndex].eStatus = WIFI_STATUS_DISCONNECTED;
				}

				if(WiFiMsg.eMsg == WIFI_EVT_CONNECT_FAIL)
				{
					if(s_stWiFiInstance.stWiFiInfo[ulIndex].stCallbackInfo[DI_WIFI_NOTIFY_EVT_CONNECT_FAIL].pfnCallback)
					{
						DI_WIFI_CALLBACK_DATA_t stCallbackData;
						stCallbackData.ulDevId= s_stWiFiInstance.stWiFiInfo[ulIndex].ulDevId;
						stCallbackData.eEvent = DI_WIFI_NOTIFY_EVT_CONNECT_FAIL;
						(*s_stWiFiInstance.stWiFiInfo[ulIndex].stCallbackInfo[DI_WIFI_NOTIFY_EVT_CONNECT_FAIL].pfnCallback)(&stCallbackData);
					}
				}
				break;

			default:
				break;
		}

	}
}

DRV_Error DRV_WIFI_GetDongleStatus(HUINT32 ulDevId, HBOOL *pbConnect)
{
	HUINT32 ulIndex = 0;

	ulIndex = P_WIFI_GetIndexbyDevId(ulDevId);
	if(ulIndex == WIFI_MAX_NUM || pbConnect == NULL)
	{
		PrintError("[%s] Do not find WIFI device ID !!\n",__func__);
		return DRV_ERR;
	}

	if(s_stWiFiInstance.stWiFiInfo[ulIndex].eDongleStatus == WIFI_DONGLE_INSERTED)
	{
		*pbConnect = TRUE;
	}
	else if(s_stWiFiInstance.stWiFiInfo[ulIndex].eDongleStatus == WIFI_DONGLE_EXTRACTED)
	{
		*pbConnect = FALSE;
	}
	else
	{
		PrintError("[%s] Init Fail !!\n",__func__);
		return DRV_ERR;
	}

	return DRV_OK;
}

DRV_Error DRV_WIFI_Init(void)
{
	DRV_Error dResult = DRV_OK;
	HINT32	nVkErr;
	HUINT32 ulLoop,ulIndex = 0;

	VK_MEM_Memset(&s_stWiFiInstance,0x00,sizeof(s_stWiFiInstance));
	nVkErr = VK_SEM_Create((unsigned long*)&s_stWiFiInstance.ulSema, "Wi-Fi", VK_SUSPENDTYPE_FIFO);
	if(nVkErr != VK_OK)
	{
		PrintError("[%s] Creating Wi-Fi VK_SEM_Create failed!\n",__func__);
		return DRV_ERR;
	}

	nVkErr = VK_MSG_Create(MSGQ_SIZE_NORMAL, sizeof(WIFI_MSG_t),"WiFiMsgQue",(unsigned long*)&s_stWiFiInstance.ulMsg,VK_SUSPENDTYPE_FIFO);
	if(nVkErr != VK_OK)
	{
		PrintError("[%s] Creating Wi-Fi VK_MSG_Create failed!\n",__func__);
		return DRV_ERR;
	}

	nVkErr = VK_TASK_Create(P_WIFI_Task, WIFI_TASK_PRIORITY, WIFI_TASK_STACK_SIZE*100, "P_WIFI_Task", (void *)0, (unsigned long*)&s_stWiFiInstance.ulTask, FALSE);
	if(nVkErr != VK_OK)
	{
		PrintError("[%s] Creating Wi-Fi VK_TASK_Create failed!\n",__func__);
		return DRV_ERR;
	}

	for(ulLoop = 0; ulLoop < WIFI_MAX_NUM ; ulLoop++)
	{
		s_stWiFiInstance.stWiFiInfo[ulLoop].ulDevId			= 0xFF;
		s_stWiFiInstance.stWiFiInfo[ulLoop].nWPSPid 		= -1;
		s_stWiFiInstance.stWiFiInfo[ulLoop].eWPS 			= DI_WIFI_WPS_NONE;
		s_stWiFiInstance.stWiFiInfo[ulLoop].eUserSetting	= WIFI_STATUS_DISCONNECTED;
		s_stWiFiInstance.stWiFiInfo[ulLoop].eStatus 		= WIFI_STATUS_DISCONNECTED;
		s_stWiFiInstance.stWiFiInfo[ulLoop].eDongleStatus	= WIFI_DONGLE_EXTRACTED;
	}

	for(ulLoop = 0; ulLoop < DRV_CFG_GetNetMaxDevice(); ulLoop++)
	{
		if(s_stDevList[ulLoop].eDevType == DI_NETWORK_DEV_WIFI)
		{
			s_stWiFiInstance.stWiFiInfo[ulIndex].ulDevId = s_stDevList[ulLoop].ulDevId;
			VK_snprintf( (char *)s_stWiFiInstance.stWiFiInfo[ulIndex].acIfName, sizeof(s_stDevList[ulLoop].szName), (char *)s_stDevList[ulLoop].szName);
			PrintDebug("[%s] Info] DevId : %d IfName : %s \n",__func__,s_stWiFiInstance.stWiFiInfo[ulIndex].ulDevId,s_stWiFiInstance.stWiFiInfo[ulIndex].acIfName);
			ulIndex++;
		}
	}

	dResult = DRV_WIRELESS_Hotplug_Init();
	if(dResult != DRV_OK)
	{
		PrintError("[%s] Creating Wi-Fi VK_TASK_Create failed!\n",__func__);
		return DRV_ERR;
	}

	dResult = DRV_WIRELESS_RegisterCallback(P_WIFI_NotifyDImsg);
	if(dResult != DRV_OK)
	{
		PrintError("[%s] Creating Wi-Fi VK_TASK_Create failed!\n",__func__);
		return DRV_ERR;
	}

	VK_TASK_Start(s_stWiFiInstance.ulTask);

#if defined(CONFIG_SWB_B70_WIFI) || defined(CONFIG_BCM43567)
	P_CheckPcieWifiConnection();
#endif

#if defined(CONFIG_QV940)
	P_CheckRgmiiWifiConnection();
#endif

	return DRV_OK;
}

DI_ERR_CODE 	DI_WIFI_RegisterEventCallback(HUINT32 ulDevId, DI_WIFI_NOTIFY_EVT_e eEvent, pfnWIFI_EVT_CALLBACK pfnCallback)
{
	HINT32	nVkErr;
	HUINT32 ulIndex = 0;

	PrintEnter();

	nVkErr = VK_SEM_Get(s_stWiFiInstance.ulSema);
	if(nVkErr != VK_OK)
	{
		PrintError("[%s] VK_SEM_Get failed!\n",__func__);
		return DI_ERR_ERROR;
	}

	ulIndex = P_WIFI_GetIndexbyDevId(ulDevId);
	if(ulIndex == WIFI_MAX_NUM)
	{
		PrintError("[%s] Do not find WIFI device ID !!\n",__func__);
		VK_SEM_Release(s_stWiFiInstance.ulSema);
		return DI_ERR_NO_RESOURCE;
	}

	s_stWiFiInstance.stWiFiInfo[ulIndex].stCallbackInfo[eEvent].pfnCallback = pfnCallback;
	if(eEvent == DI_WIFI_NOTIFY_EVT_INSERTED)
	{
		if(s_stWiFiInstance.stWiFiInfo[ulIndex].eDongleStatus == WIFI_DONGLE_INSERTED)
		{
			DI_WIFI_CALLBACK_DATA_t stCallbackData;
			stCallbackData.ulDevId= s_stWiFiInstance.stWiFiInfo[ulIndex].ulDevId;
			stCallbackData.eEvent = DI_WIFI_NOTIFY_EVT_INSERTED;
			stCallbackData.ulNumOfData = 0;
			stCallbackData.pvEventData = NULL;
			(*s_stWiFiInstance.stWiFiInfo[ulIndex].stCallbackInfo[DI_WIFI_NOTIFY_EVT_INSERTED].pfnCallback)(&stCallbackData);
		}
	}
	VK_SEM_Release(s_stWiFiInstance.ulSema);

	PrintExit();

	return DI_ERR_OK;
}

/* SCAN 도중에 SCAN 함수가 호출 되면....? */
/* 기준 SCAN 유지하면 됨.*/
DI_ERR_CODE		DI_WIFI_Scan(HUINT32 ulDevId)
{
	HINT32	nVkErr;
	WIFI_MSG_t tMsg;
	HUINT32 ulIndex = 0;

	PrintEnter();

	nVkErr = VK_SEM_Get(s_stWiFiInstance.ulSema);
	if(nVkErr != VK_OK)
	{
		PrintError("[%s] VK_SEM_Get failed!\n",__func__);
		return DI_ERR_ERROR;
	}

	ulIndex = P_WIFI_GetIndexbyDevId(ulDevId);
	if(ulIndex == WIFI_MAX_NUM)
	{
		PrintError("[%s] Do not find WIFI device ID !!\n",__func__);
		VK_SEM_Release(s_stWiFiInstance.ulSema);
		return DI_ERR_NO_RESOURCE;
	}

#if !defined(CONFIG_QV940)
	if(s_stWiFiInstance.stWiFiInfo[ulIndex].eDongleStatus != WIFI_DONGLE_INSERTED)
	{
		PrintError("[%s][Index:%d] WIFI Dongle was removed DevId[%d] !!\n",__func__,ulIndex,ulDevId);
		VK_SEM_Release(s_stWiFiInstance.ulSema);
		return DI_ERR_ERROR;
	}
#endif
	if(s_stWiFiInstance.stWiFiInfo[ulIndex].eStatus == WIFI_STATUS_CONNECTING)
	{
		tMsg.ulIndex = ulIndex;
		tMsg.eMsg = WIFI_EVT_DISCONNECT;
		PrintError("[%s] WIFI Dongle is connecting!! Try to disconnect and re-scan !!\n",__func__);
		nVkErr = VK_MSG_SendTimeout(s_stWiFiInstance.ulMsg, &tMsg, sizeof(WIFI_MSG_t),0);
		if(nVkErr)
		{
			PrintError("[%s] VK_MSG_Send Err(0x%x)!!\n",__func__,nVkErr);
		VK_SEM_Release(s_stWiFiInstance.ulSema);
			return DI_ERR_ERROR;
		}
	}

	if(s_stWiFiInstance.stWiFiInfo[ulIndex].bScanning == TRUE)
	{
		PrintError("[%s] WIFI Dongle is scanning!!\n",__func__);
		VK_SEM_Release(s_stWiFiInstance.ulSema);
		return DI_ERR_NO_RESOURCE;
	}

	tMsg.ulIndex = ulIndex;
	tMsg.eMsg = WIFI_EVT_SCAN_START;
	nVkErr = VK_MSG_SendTimeout(s_stWiFiInstance.ulMsg, &tMsg, sizeof(WIFI_MSG_t),0);
	if(nVkErr)
	{
		PrintError("[%s] VK_MSG_Send Err(0x%x)!!\n",__func__,nVkErr);
		VK_SEM_Release(s_stWiFiInstance.ulSema);
		return DI_ERR_ERROR;
	}

	VK_SEM_Release(s_stWiFiInstance.ulSema);

	PrintExit();

	return DI_ERR_OK;
}

DI_ERR_CODE		DI_WIFI_Connect(HUINT32 ulDevId, DI_WIFI_WPS_e eWPS, DI_WIFI_INFO_t *pstApInfo,HUINT32 ulWaitSeconds)
{
	int			vkMsg;
	WIFI_MSG_t WiFiMsg;
	HINT32	nVkErr;
	HUINT32 ulIndex = 0;
	DRV_Error dResult = DRV_OK;

	PrintEnter();

	if(eWPS >= DI_WIFI_WPS_MAX)
	{
		PrintError("[%s] eWPS Parameter err %d \n",__func__,eWPS);
		return DI_ERR_ERROR;
	}

	if( ((eWPS == DI_WIFI_WPS_NONE)||(eWPS == DI_WIFI_WPS_PIN)) && (pstApInfo == NULL) )
	{
		PrintError("[%s] eWPS [%d] pstApInfo Parameter NULL \n",__func__,eWPS);
		return DI_ERR_ERROR;
	}

	nVkErr = VK_SEM_Get(s_stWiFiInstance.ulSema);
	if(nVkErr != VK_OK)
	{
		PrintError("[%s] VK_SEM_Get failed!\n",__func__);
		return DI_ERR_ERROR;
	}

	ulIndex = P_WIFI_GetIndexbyDevId(ulDevId);
	if(ulIndex == WIFI_MAX_NUM)
	{
		PrintError("[%s] Do not find WIFI device ID !!\n",__func__);
		VK_SEM_Release(s_stWiFiInstance.ulSema);
		return DI_ERR_NO_RESOURCE;
	}

#if !defined(CONFIG_QV940)
	if(s_stWiFiInstance.stWiFiInfo[ulIndex].eDongleStatus != WIFI_DONGLE_INSERTED)
	{
		PrintError("[%s][Index:%d] WIFI Dongle was removed DevId[%d] !!\n",__func__,ulIndex,ulDevId);
		VK_SEM_Release(s_stWiFiInstance.ulSema);
		return DI_ERR_ERROR;
	}
#endif

	if(s_stWiFiInstance.stWiFiInfo[ulIndex].eUserSetting == WIFI_STATUS_CONNECTED
		&& s_stWiFiInstance.stWiFiInfo[ulIndex].eStatus != WIFI_STATUS_DISCONNECTED )
	{
		PrintError("[%s] MW was already set to WIFI CONNECT \n",__func__);
		VK_SEM_Release(s_stWiFiInstance.ulSema);
		return DI_ERR_ERROR;
	}

	if(s_stWiFiInstance.stWiFiInfo[ulIndex].bScanning == TRUE)
	{
		PrintError("[%s] WIFI Dongle is scanning!!\n",__func__);
		VK_SEM_Release(s_stWiFiInstance.ulSema);
		return DI_ERR_NO_RESOURCE;
	}

	s_stWiFiInstance.stWiFiInfo[ulIndex].eUserSetting = WIFI_STATUS_CONNECTED;

	if(eWPS == DI_WIFI_WPS_NONE)
	{
		/*Key validation check */
		dResult = P_WIFI_IsKeyValid(pstApInfo);
		if (dResult != DRV_OK)
		{
			PrintError("[%s] Error : P_WIFI_IsKeyValid %d \n", __FUNCTION__, dResult);
			return DI_ERR_INVALID_PARAM;
		}

		VK_memcpy(&s_stWiFiInstance.stWiFiInfo[ulIndex].stUserSetAP,pstApInfo,sizeof(DI_WIFI_INFO_t));

		WiFiMsg.ulIndex = ulIndex;
		WiFiMsg.eMsg = WIFI_EVT_CONNECT_START;
		vkMsg = VK_MSG_SendTimeout(s_stWiFiInstance.ulMsg, &WiFiMsg, sizeof(WIFI_MSG_t),0);
		if(vkMsg)
		{
			PrintError("[%s] VK_MSG_Send Err(0x%x)!!\n",__func__,vkMsg);
			VK_SEM_Release(s_stWiFiInstance.ulSema);
			return DI_ERR_ERROR;
		}
	}
	else if(eWPS == DI_WIFI_WPS_PBC)
	{
		if (s_eWifiCtlInterface == WIFI_CTL_WL)
		{
			P_WIFI_SetBrcmDown();
			P_WIFI_SetBrcmUp();
		}

		WiFiMsg.ulIndex = ulIndex;
		WiFiMsg.eMsg = WIFI_EVT_WPS_PBC_START;
		vkMsg = VK_MSG_SendTimeout(s_stWiFiInstance.ulMsg, &WiFiMsg, sizeof(WIFI_MSG_t),0);
		if(vkMsg)
		{
			PrintError("[%s] VK_MSG_Send Err(0x%x)!!\n",__func__,vkMsg);
			VK_SEM_Release(s_stWiFiInstance.ulSema);
			return DI_ERR_ERROR;
		}
	}
	else if(eWPS == DI_WIFI_WPS_PIN)
	{
		if (s_eWifiCtlInterface == WIFI_CTL_WL)
		{
			P_WIFI_SetBrcmDown();
			P_WIFI_SetBrcmUp();
		}

		VK_memcpy(&s_stWiFiInstance.stWiFiInfo[ulIndex].stUserSetAP,pstApInfo,sizeof(DI_WIFI_INFO_t));

		WiFiMsg.ulIndex = ulIndex;
		WiFiMsg.eMsg = WIFI_EVT_WPS_PIN_START;

		vkMsg = VK_MSG_SendTimeout(s_stWiFiInstance.ulMsg, &WiFiMsg, sizeof(WIFI_MSG_t),0);
		if(vkMsg)
		{
			PrintError("[%s] VK_MSG_Send Err(0x%x)!!\n",__func__,vkMsg);
			VK_SEM_Release(s_stWiFiInstance.ulSema);
			return DI_ERR_ERROR;
		}
	}

	ulWaitSeconds = ulWaitSeconds * 1000;

	nVkErr = VK_TIMER_EventAfter(ulWaitSeconds,P_WIFI_DisConnectCallback, (void *)&ulIndex,sizeof(HUINT32), (unsigned long*)&s_stWiFiInstance.ulDisConnectTimerId);
	if(nVkErr)
	{
		PrintError("[%s] VK_TIMER_EventAfter Err(0x%x)!!\n",__func__,nVkErr);
	}

	VK_SEM_Release(s_stWiFiInstance.ulSema);

	PrintExit();

	return DI_ERR_OK;
}

DI_ERR_CODE		DI_WIFI_Disconnect(HUINT32 ulDevId)
{
	HINT32	nVkErr;
	HUINT32 ulIndex = 0;
	WIFI_MSG_t 	stWiFiMsg;

	PrintEnter();

	nVkErr = VK_SEM_Get(s_stWiFiInstance.ulSema);
	if(nVkErr != VK_OK)
	{
		PrintError("[%s] VK_SEM_Get failed!\n",__func__);
		return DI_ERR_ERROR;
	}

	ulIndex = P_WIFI_GetIndexbyDevId(ulDevId);
	if(ulIndex == WIFI_MAX_NUM)
	{
		PrintError("[%s] Do not find WIFI device ID !!\n",__func__);
		VK_SEM_Release(s_stWiFiInstance.ulSema);
		return DI_ERR_NO_RESOURCE;
	}

	if(s_stWiFiInstance.stWiFiInfo[ulIndex].eUserSetting == WIFI_STATUS_DISCONNECTED)
	{
		PrintError("[%s] MW was already set to WIFI DISCONNECT \n",__func__);
		VK_SEM_Release(s_stWiFiInstance.ulSema);
		return DI_ERR_ERROR;
	}

#if defined(CONFIG_QV940)
{
	FILE *fp;
	char qcsapiStatus[129], cmdBuf[255], buf[128];

	if (s_stWiFiInstance.stWiFiInfo[ulIndex].eWPS != DI_WIFI_WPS_NONE)
	{
		VK_snprintf(cmdBuf, sizeof(cmdBuf), "call_qcsapi_sockrpc --host %s wps_cancel wifi0 > %s", IPADDR_QV940, QCSAPI_WPS_TMP_FILE);
		VK_SYSTEM_Command(cmdBuf);
		VK_TASK_Sleep(1000);

		fp = fopen(QCSAPI_WPS_TMP_FILE, "r");
		if (fp != NULL)
		{
			fgets(buf, 128, fp);
			VK_sscanf(buf, "%s", &qcsapiStatus[0]);
			fclose(fp);

			if (VK_strcmp("complete", qcsapiStatus) != 0)
	    	{
	        	PrintError("[%s] WPS cancel QCSAPI error!\n", __FUNCTION__);
	        	VK_SEM_Release(s_stWiFiInstance.ulSema);
	        	return DI_ERR_ERROR;
		    }
		}
		else
	    {
			PrintError("[%s] Err fopen %s\n", __FUNCTION__, strerror( errno ));
			VK_SEM_Release(s_stWiFiInstance.ulSema);
			return DI_ERR_ERROR;
	    }
	}

	/* disassociate */
    VK_snprintf(cmdBuf, sizeof(cmdBuf), "call_qcsapi_sockrpc --host %s disassociate wifi0 > %s", IPADDR_QV940, QCSAPI_TMP_FILE);
	VK_SYSTEM_Command(cmdBuf);
	VK_TASK_Sleep(1000);

	fp = fopen(QCSAPI_TMP_FILE, "r");
	if (fp != NULL)
	{
		fgets(buf, 128, fp);
		VK_sscanf(buf, "%s", &qcsapiStatus[0]);
		fclose(fp);

		if (VK_strcmp("complete", qcsapiStatus) != 0)
	    {
	        PrintError("[%s] QCSAPI disassociate error!\n", __FUNCTION__);
	        s_stWiFiInstance.stWiFiInfo[ulIndex].eUserSetting = WIFI_STATUS_DISCONNECTED;
	        VK_SEM_Release(s_stWiFiInstance.ulSema);
	        return DI_ERR_ERROR;
	    }
	}
	else
    {
		PrintError("[%s] Err fopen %s\n", __FUNCTION__, strerror( errno ));
		VK_SEM_Release(s_stWiFiInstance.ulSema);
		return DI_ERR_ERROR;
    }
}
#endif

	stWiFiMsg.ulIndex = ulIndex;
	stWiFiMsg.eMsg = WIFI_EVT_DISCONNECT;

	nVkErr = VK_MSG_SendTimeout(s_stWiFiInstance.ulMsg, &stWiFiMsg, sizeof(WIFI_MSG_t),0);
	if( nVkErr != 0 )
	{
		PrintError("[%s] VK_MSG_Send Err(0x%x)!!\n",__func__,nVkErr);
		VK_SEM_Release(s_stWiFiInstance.ulSema);
		return DI_ERR_ERROR;
	}
	if(s_stWiFiInstance.ulDisConnectTimerId)
	{
		VK_TIMER_Cancel(s_stWiFiInstance.ulDisConnectTimerId);
		s_stWiFiInstance.ulDisConnectTimerId = 0;
	}
	s_stWiFiInstance.stWiFiInfo[ulIndex].eUserSetting = WIFI_STATUS_DISCONNECTED;

	VK_SEM_Release(s_stWiFiInstance.ulSema);

	PrintExit();

	return DI_ERR_OK;
}

/*
	ESSID,OpMode,MAC,WaveStrength,BitRate,SecureType,AuthenType,EncryptType,KeyIndex,Id,Key
*/
DI_ERR_CODE		DI_WIFI_GetAPInfo(HUINT32 ulDevId, DI_WIFI_INFO_t *pstApInfo)
{
	HINT32	nVkErr;
	HUINT32 ulIndex = 0;

	PrintEnter();

	nVkErr = VK_SEM_Get(s_stWiFiInstance.ulSema);
	if(nVkErr != VK_OK)
	{
		PrintError("[%s] VK_SEM_Get failed!\n",__func__);
		return DI_ERR_ERROR;
	}

	ulIndex = P_WIFI_GetIndexbyDevId(ulDevId);
	if(ulIndex == WIFI_MAX_NUM)
	{
		PrintError("[%s] Do not find WIFI device ID !!\n",__func__);
		VK_SEM_Release(s_stWiFiInstance.ulSema);
		return DI_ERR_NO_RESOURCE;
	}
	/* Connect or Disconnect ? */
	if(!((s_stWiFiInstance.stWiFiInfo[ulIndex].eStatus == WIFI_STATUS_CONNECTED)||(s_stWiFiInstance.stWiFiInfo[ulIndex].eStatus == WIFI_STATUS_RECONNECTING)))
	{
		PrintError("[%s] Disconnect !!\n",__func__);
		VK_SEM_Release(s_stWiFiInstance.ulSema);
		return DI_ERR_NO_RESOURCE;
	}

#if !defined(CONFIG_QV940)
	/* Copy User Setting & BitRate*/
	VK_memcpy(pstApInfo,&s_stWiFiInstance.stWiFiInfo[ulIndex].stUserSetAP,sizeof(DI_WIFI_INFO_t));
#endif
	if(s_eWifiCtlInterface == WIFI_CTL_WL)
	{
		P_WIFI_GetAPInfo(ulIndex, pstApInfo);
		P_WIFI_GetRate(ulIndex, &pstApInfo->nBitRate);
	}
#if defined(CONFIG_QV940)
	else if(s_eWifiCtlInterface == WIFI_CTL_QCSAPI)
	{
		P_WIFI_GetQcsapiAPInfo(ulIndex, pstApInfo);
		P_WIFI_GetRssi(ulIndex, &pstApInfo->nWaveStrength);
	}
#endif
	else
	{
		/* Get Strength */
		P_WIFI_GetRssi(ulIndex, &pstApInfo->nWaveStrength);
		P_WIFI_GetRate(ulIndex, &pstApInfo->nBitRate);

		/* Get AP MAC address */
		VK_memcpy(pstApInfo->aucMAC,&s_stWiFiInstance.stWiFiInfo[ulIndex].aucMAC, sizeof(s_stWiFiInstance.stWiFiInfo[ulIndex].aucMAC));
	}

	VK_SEM_Release(s_stWiFiInstance.ulSema);

	PrintExit();

	return DI_ERR_OK;

}

DI_ERR_CODE		DI_WIFI_GetStrength(HUINT32 ulDevId, HINT32 *pnStrength)
{
	HUINT32 ulIndex = 0;

	HINT32	nVkErr;

	PrintEnter();

	nVkErr = VK_SEM_Get(s_stWiFiInstance.ulSema);
	if(nVkErr != VK_OK)
	{
		PrintError("[%s] VK_SEM_Get failed!\n",__func__);
		return DI_ERR_ERROR;
	}

	ulIndex = P_WIFI_GetIndexbyDevId(ulDevId);
	if(ulIndex == WIFI_MAX_NUM)
	{
		PrintError("[%s] Do not find WIFI device ID !!\n",__func__);
		VK_SEM_Release(s_stWiFiInstance.ulSema);
		return DI_ERR_NO_RESOURCE;
	}

	/* Connect or Disconnect ? */
	if(s_stWiFiInstance.stWiFiInfo[ulIndex].eStatus != WIFI_STATUS_CONNECTED)
	{
		PrintError("[%s] Do not find WIFI device ID !!\n",__func__);
		PrintError("[%s] Disconnect !!\n",__func__);
		VK_SEM_Release(s_stWiFiInstance.ulSema);
		return DI_ERR_NO_RESOURCE;
	}

	/* Get Strength */
	P_WIFI_GetRssi(ulIndex, pnStrength);

	VK_SEM_Release(s_stWiFiInstance.ulSema);

	PrintExit();

	return DI_ERR_OK;
}

DI_ERR_CODE		DI_WIFI_GetBitRate(HUINT32 ulDevId, HINT32 *pnBitRate)
{
	HUINT32 ulIndex = 0;
	HINT32	nVkErr;

	PrintEnter();

	nVkErr = VK_SEM_Get(s_stWiFiInstance.ulSema);
	if(nVkErr != VK_OK)
	{
		PrintError("[%s] ERR VK_SEM_Get!!\n",__func__);
		return DI_ERR_ERROR;
	}

	ulIndex = P_WIFI_GetIndexbyDevId(ulDevId);
	if(ulIndex == WIFI_MAX_NUM)
	{
		PrintError("[%s] ERR DevId(%d)!!\n",__func__,ulDevId);
		VK_SEM_Release(s_stWiFiInstance.ulSema);
		return DI_ERR_NO_RESOURCE;
	}

	/* Connect or Disconnect ? */
	if(s_stWiFiInstance.stWiFiInfo[ulIndex].eStatus != WIFI_STATUS_CONNECTED)
	{
		PrintError("[%s] Do not find WIFI device ID !!\n",__func__);
		PrintError("[%s] Disconnect !!\n",__func__);
		VK_SEM_Release(s_stWiFiInstance.ulSema);
		return DI_ERR_NO_RESOURCE;
	}

	/* bit rate */
	P_WIFI_GetRate(ulIndex, pnBitRate);

	VK_SEM_Release(s_stWiFiInstance.ulSema);

	PrintExit();

	return DI_ERR_OK;
}

DI_ERR_CODE		DI_WIFI_GetRSSI(HUINT32 ulDevId, DI_WIFI_RSSI_INFO_t *pRssiInfo)
{
	/* Used for only Factory test SW */
	DI_ERR_CODE diResult = DI_ERR_ERROR;
	DRV_Error drvError = DRV_ERR;
	FILE 	*f;
	char	buf[256];
	char *	bp;
	int	t;
	HUINT32 ulTmp;
	HINT32	nVkErr;
	HUINT32 ulIndex = 0;

	PrintEnter();

	nVkErr = VK_SEM_Get(s_stWiFiInstance.ulSema);
	if(nVkErr != VK_OK)
	{
		PrintError("[%s] ERR VK_SEM_Get!!\n",__func__);
		return DI_ERR_ERROR;
	}

	ulIndex = P_WIFI_GetIndexbyDevId(ulDevId);
	if(ulIndex == WIFI_MAX_NUM)
	{
		PrintError("[%s] ERR DevId(%d)!!\n",__func__,ulDevId);
		diResult = DI_ERR_NO_RESOURCE;
		goto ret;
	}

#if !defined(CONFIG_QV940)
	/* Connect or Disconnect ? */
	if(s_stWiFiInstance.stWiFiInfo[ulIndex].eStatus != WIFI_STATUS_CONNECTED)
	{
		PrintError("[%s] Do not find WIFI device ID !!\n",__func__);
		PrintError("[%s] Disconnect !!\n",__func__);
		diResult = DI_ERR_NO_RESOURCE;
		goto ret;
	}
#endif
	VK_MEM_Memset(pRssiInfo, 0x0, sizeof(DI_WIFI_RSSI_INFO_t));

	/* BCM43228 driver don't register wlan interface info to PROC_NET_WIRELESS
	    So, use WL command when BCM WI-FI detected */
	if(s_eWifiCtlInterface == WIFI_CTL_WL)
	{
		drvError = P_WIFI_RunWlCmdForEachAntRssi(ulIndex, pRssiInfo);
		if(drvError == DRV_ERR)
		{
			diResult = DI_ERR_ERROR;
		}
		else
		{
			diResult = DI_ERR_OK;
		}
		goto ret;
	}
	else if(s_eWifiCtlInterface == WIFI_CTL_WPA_SUPPLICANT)
	{
		f = fopen(PROC_NET_WIRELESS, "r");

		if(f == NULL)
		{
			diResult = DI_ERR_ERROR;
			goto ret;
		}

		/* Loop on all devices */
		while(fgets(buf,255,f))
		{
			bp=buf;
			while(*bp&&isspace(*bp))
			{
				bp++;
			}

			/* Is it the good device ? ==> comapre "wlanX:" */
			if(VK_strncmp(bp, s_stWiFiInstance.stWiFiInfo[ulIndex].acIfName, VK_strlen(s_stWiFiInstance.stWiFiInfo[ulIndex].acIfName))==0 &&
				bp[VK_strlen(s_stWiFiInstance.stWiFiInfo[ulIndex].acIfName)]==':')
			{
				/* Skip wlanX: */
				bp += VK_strlen(s_stWiFiInstance.stWiFiInfo[ulIndex].acIfName)+1;

				/* -- status -- */
				bp = strtok(bp, " ");
				VK_sscanf(bp, "%X", &ulTmp);

				/* -- link quality -- */
				bp = strtok(NULL, " ");
				VK_sscanf(bp, "%d", &t);

				/* -- signal level -- */
				bp = strtok(NULL, " ");
				VK_sscanf(bp, "%d", &t);
				pRssiInfo->uiRssi = t;

				/* -- noise level -- */
				bp = strtok(NULL, " ");
				VK_sscanf(bp, "%d", &t);

				/* -- discarded packets -- */
				bp = strtok(NULL, " ");
				VK_sscanf(bp, "%d", &t);
				bp = strtok(NULL, " ");
				VK_sscanf(bp, "%d", &t);
				bp = strtok(NULL, " ");
				VK_sscanf(bp, "%d", &t);

				/* No conversion needed */
				fclose(f);
				diResult = DI_ERR_OK;
				goto ret;
			}
		}

		/* Not found wlanX interface info */
		fclose(f);
		diResult = DI_ERR_ERROR;
		goto ret;
	}
#if defined(CONFIG_QV940)
	else if(s_eWifiCtlInterface == WIFI_CTL_QCSAPI)
	{
		drvError = P_WIFI_RunQcsapiCmdForEachAntRssi(pRssiInfo);
		if(drvError == DRV_ERR)
		{
			diResult = DI_ERR_ERROR;
		}
		else
		{
			diResult = DI_ERR_OK;
		}
		goto ret;
	}
#endif
	else
	{
		PrintError("[%s] Error : s_eWifiCtlInterface %d \n", __FUNCTION__, s_eWifiCtlInterface);
		diResult = DI_ERR_ERROR;
		goto ret;
	}

ret:
	VK_SEM_Release(s_stWiFiInstance.ulSema);

	PrintExit();

	return diResult;
}

DI_ERR_CODE		DI_WIFI_GetDrvInfo(HUINT32 ulDevId, DI_WIFI_DRV_INFO_t * pstWifiDrvInfo)
{
#if defined(CONFIG_BCM43236) || defined(CONFIG_BCM43236_MFG) || defined(CONFIG_BCM43567)
	DRV_Error dResult = DRV_OK;
	HINT32 ret = 0;
	FILE *fp;
	HULONG ulTmpCrc32 = 0;
	long int ulFileSize=0;
	size_t readSize = 0;
	HCHAR acBuf[64] = {0,};
	HCHAR *pacBuf = NULL;
	HINT32 anVer[4] = {0,};

	UNUSED(ulDevId);

	if (pstWifiDrvInfo == NULL)
	{
		PrintError("[%s] pstWifiDrvInfo is NULL \n",__FUNCTION__);
		return DI_ERR_ERROR;
	}

	VK_MEM_Memset(pstWifiDrvInfo, 0x00, sizeof(DI_WIFI_DRV_INFO_t));

	/* 1. find driver name via configuration */
	VK_MEM_Memset(acBuf,0x00,sizeof(acBuf));

#if defined(CONFIG_BCM43236) || defined(CONFIG_BCM43236_MFG)
#if defined(CONFIG_BCM43236_BISON)
	VK_snprintf(pstWifiDrvInfo->aucDrvName, sizeof(pstWifiDrvInfo->aucDrvName), "Bison");
#else
	VK_snprintf(pstWifiDrvInfo->aucDrvName, sizeof(pstWifiDrvInfo->aucDrvName), "Falcon");
#endif
#elif defined(CONFIG_WIFI_AARDVARK)
	VK_snprintf(pstWifiDrvInfo->aucDrvName, sizeof(pstWifiDrvInfo->aucDrvName), "Aardvark");
#elif defined(CONFIG_BCM43567)
	VK_snprintf(pstWifiDrvInfo->aucDrvName, sizeof(pstWifiDrvInfo->aucDrvName), "Dhd");
#endif

	PrintDebug("[%s] pstWifiDrvInfo->aucDrvName = %s\n",__FUNCTION__, pstWifiDrvInfo->aucDrvName);

	/* 2. find driver version from 1st line string of "wl ver" result */
	VK_MEM_Memset(acBuf,0x00,sizeof(acBuf));

	VK_snprintf(acBuf, sizeof(acBuf), "wl ver > %s", BCM_WL_VERSION_FILE);
	VK_SYSTEM_Command(acBuf);

	fp = fopen(BCM_WL_VERSION_FILE, "r");
	if(fp == NULL)
	{
		PrintError("[%s] Err fopen %s\n",__FUNCTION__, strerror( errno ));
		dResult = DRV_ERR;
		goto go_out;
	}

	VK_MEM_Memset(acBuf,0x00,sizeof(acBuf));

	fgets(acBuf, sizeof(acBuf), fp );

	pacBuf = NULL;
	if((pacBuf = VK_strchr(acBuf,'\n')) !=NULL)
	{
		*pacBuf = '\0';
	}

	VK_snprintf((char*)pstWifiDrvInfo->aucDrvVer, sizeof(pstWifiDrvInfo->aucDrvVer),acBuf);

	PrintDebug("[%s] pstWifiDrvInfo->aucDrvVer = %s\n",__FUNCTION__, pstWifiDrvInfo->aucDrvVer);

	/* 3. find firmware version from 2nd line string of "wl ver" result */
	VK_MEM_Memset(acBuf,0x00,sizeof(acBuf));

	fgets(acBuf, sizeof(acBuf), fp );

	pacBuf = NULL;
	pacBuf = VK_strstr(acBuf,"version");
	if(pacBuf)
	{
		VK_sscanf(pacBuf+8,"%d.%d.%d.%d", &anVer[0],&anVer[1],&anVer[2],&anVer[3]);
	}

	fclose(fp);

	VK_snprintf(pstWifiDrvInfo->aucFwVer, sizeof(pstWifiDrvInfo->aucFwVer), "%d.%d.%d.%d", anVer[0], anVer[1], anVer[2], anVer[3]);

	PrintDebug("[%s] pstWifiDrvInfo->aucFwVer = %s\n",__FUNCTION__, pstWifiDrvInfo->aucFwVer);

	/* 4. calculate CRC32 of NVRAM file */
	VK_MEM_Memset(acBuf,0x00,sizeof(acBuf));

#if defined(CONFIG_BCM43567)
	VK_snprintf(acBuf, sizeof(acBuf), BCM_WIFI_NV_PATH, BCM_WIFI_CHIP_NAME);
#else
	VK_snprintf(acBuf, sizeof(acBuf), BCM_WIFI_NV_PATH, BCM43236_CHIP_NAME);
#endif

	fp = fopen(acBuf,"r");
	if(fp == NULL)
	{
		PrintError("[%s] Err fopen %s\n",__FUNCTION__, strerror( errno ));
		dResult = DRV_ERR;
		goto go_out;
	}

	/* Get file size */
	ret = fseek(fp, 0, SEEK_END);
	if(ret != 0)
	{
		PrintError("[%s] Err fseek %s\n",__FUNCTION__, strerror( errno ));
		dResult = DRV_ERR;
		goto go_out;
	}

	ulFileSize = ftell(fp);
	if(ulFileSize <= 0)
	{
		PrintError("ulFileSize = %d \n",ulFileSize);
		dResult = DRV_ERR;
		goto go_out;
	}

	ret = fseek(fp, 0, SEEK_SET);
	if(ret != 0)
	{
		PrintError("[%s] Err fseek %s\n",__FUNCTION__, strerror( errno ));
		dResult = DRV_ERR;
		goto go_out;
	}

	pacBuf = NULL;
	pacBuf = DD_MEM_Alloc(ulFileSize);
	if (NULL == pacBuf)
	{
		PrintError("[%s] Err Memory Allocation %s\n",__FUNCTION__, strerror( errno ));
		dResult = DRV_ERR;
		goto go_out;
	}

	readSize = fread(pacBuf, 1, ulFileSize, fp);
	if (readSize != (size_t)ulFileSize)
	{
		PrintError("[%s] Err fread %s\n",__FUNCTION__, strerror( errno ));
		dResult = DRV_ERR;
		goto go_out;
	}

	ulTmpCrc32 = DRV_UTIL_GetCrc32(pacBuf,ulFileSize);

	pstWifiDrvInfo->ulWifiNvramCrc = (HUINT32)(~ulTmpCrc32);

go_out :
	if(pacBuf != NULL)
	{
		DD_MEM_Free(pacBuf);
	}

	if(fp != NULL)
	{
		fclose(fp);
	}

	VK_MEM_Memset(acBuf,0x00,sizeof(acBuf));
	VK_snprintf(acBuf, sizeof(acBuf), "rm -rf %s", BCM_WL_VERSION_FILE);
	VK_SYSTEM_Command(acBuf);

	return dResult;
#else
	UNUSED(ulDevId);
	UNUSED(pstWifiDrvInfo);

	PrintError("[%s] Not supported for this solution\n",__FUNCTION__);

	return DI_ERR_ERROR;
#endif
}

DI_ERR_CODE		DI_WIFI_ResetModule(HUINT32 ulDevId)
{
	DI_ERR_CODE diResult = DI_ERR_ERROR;
	char cmdBuf[256] = { 0 };

	UNUSED(ulDevId);
	UNUSED(cmdBuf);
	PrintEnter();

#if defined(CONFIG_BCM43236)
#if defined(CONFIG_WIFI_AARDVARK)
	VK_snprintf( cmdBuf, sizeof( cmdBuf ), "rmmod wl");
	VK_SYSTEM_Command( cmdBuf );
    	VK_TASK_Sleep(500);

    	VK_snprintf( cmdBuf, sizeof( cmdBuf ), "rmmod bcm_dbus");
	VK_SYSTEM_Command( cmdBuf );
    	VK_TASK_Sleep(500);
#else
	VK_snprintf( cmdBuf, sizeof( cmdBuf ), "rmmod dhd");
	VK_SYSTEM_Command( cmdBuf );
    	VK_TASK_Sleep(500);
#endif
#endif

	PrintExit();
	return diResult;
}


DI_ERR_CODE		DI_WIFI_SetWakeOnWLAN(HUINT32 ulDevId, DI_WIFI_WOWL_SETTINGS_t *pstWowlSettings)
{
#if defined(CONFIG_SUPPORT_WOWL)
	HINT32	nVkErr;
	HUINT32 ulIndex = 0;
	DI_ERR_CODE diResult = DI_ERR_ERROR;
	HCHAR acWowlCmd[128];

	PrintEnter();

	nVkErr = VK_SEM_Get(s_stWiFiInstance.ulSema);
	if(nVkErr != VK_OK)
	{
		PrintError("[%s][%d] Fail to get SEM (%d) \n",__FUNCTION__, __LINE__, nVkErr);
		return DI_ERR_ERROR;
	}

	if(pstWowlSettings == NULL)
	{
		PrintError("[%s][%d] pstWowlSettings is NULL !!\n", __FUNCTION__, __LINE__);
		diResult = DI_ERR_INVALID_PARAM;
		goto ret;
	}

	ulIndex = P_WIFI_GetIndexbyDevId(ulDevId);
	if(ulIndex == WIFI_MAX_NUM)
	{
		PrintError("[%s][%d] Invalid ulDevId(%d) !!\n", __FUNCTION__, __LINE__, ulDevId);
		diResult = DI_ERR_NO_RESOURCE;
		goto ret;
	}

	/* Connect or Disconnect ? */
	if(s_stWiFiInstance.stWiFiInfo[ulIndex].eStatus != WIFI_STATUS_CONNECTED)
	{
		PrintError("[%s][%d] Disconnected !!\n",__FUNCTION__, __LINE__);
		diResult = DI_ERR_NO_RESOURCE;
		goto ret;
	}

#if !defined(CONFIG_QV940)
#if defined(CONFIG_WIFI_DHD)
	VK_MEM_Memset(acWowlCmd, 0x0, sizeof(acWowlCmd));
	VK_snprintf(acWowlCmd, sizeof(acWowlCmd), "dhd -i %s sbreg 0xc40 1", s_stWiFiInstance.stWiFiInfo[ulIndex].acIfName);
	VK_SYSTEM_Command(acWowlCmd);

	VK_MEM_Memset(acWowlCmd, 0x0, sizeof(acWowlCmd));
	VK_snprintf(acWowlCmd, sizeof(acWowlCmd), "dhd -i %s sbreg 0xe00 0x11111111", s_stWiFiInstance.stWiFiInfo[ulIndex].acIfName);
	VK_SYSTEM_Command(acWowlCmd);
#endif

	VK_MEM_Memset(acWowlCmd, 0x0, sizeof(acWowlCmd));

	VK_SYSTEM_Command("wl PM 1");

	PrintDebug("[%s][%d] WOWL event = 0x%x \n", __FUNCTION__, __LINE__, pstWowlSettings->eWowlMask);
	VK_snprintf(acWowlCmd, sizeof(acWowlCmd), "wl wowl %d", pstWowlSettings->eWowlMask);
	VK_SYSTEM_Command(acWowlCmd);

	VK_SYSTEM_Command("wl wowl_clear");

	if(pstWowlSettings->eWowlMask & DI_WIFI_WOWL_EVENT_BCN_LOSS)
	{
		PrintDebug("[%s][%d] WOWL BcnLoss Time = %d Sec. \n", __FUNCTION__, __LINE__, pstWowlSettings->ulBcnLossSec);
		VK_MEM_Memset(acWowlCmd, 0x0, sizeof(acWowlCmd));
		VK_snprintf(acWowlCmd, sizeof(acWowlCmd), "wl wowl_bcn_loss %d", pstWowlSettings->ulBcnLossSec);
		VK_SYSTEM_Command(acWowlCmd);
	}

	VK_SYSTEM_Command("wl wowl_activate 1");

	if(pstWowlSettings->eWowlMask & DI_WIFI_WOWL_EVENT_NET_PATTERN)
	{
		/* for future use, do nothing currently */
	}

	diResult = DI_ERR_OK;
#else /* CONFIG_QV940 */
	PrintDebug("[%s][%d] WOWL event = 0x%x \n", __FUNCTION__, __LINE__, pstWowlSettings->eWowlMask);
	if (pstWowlSettings->eWowlMask == DI_WIFI_WOWL_EVENT_NONE)
	{
		VK_snprintf(acWowlCmd, sizeof(acWowlCmd), "call_qcsapi_sockrpc --host %s wowlan_host_state wifi0 0 > %s", IPADDR_QV940, QCSAPI_TMP_FILE);
		VK_SYSTEM_Command(acWowlCmd);
		if (P_WIFI_CheckQcsapiCmdReturnValue() == DI_ERR_ERROR)
		{
			diResult = DI_ERR_ERROR;
			goto ret;

		}
	}
	else if (pstWowlSettings->eWowlMask == DI_WIFI_WOWL_EVENT_MAGIC_PATTERN)
	{
		/* call_qcsapi_sockrpc --host 169.254.119.1 wowlan_host_state wifi0 1 */
		VK_snprintf(acWowlCmd, sizeof(acWowlCmd), "call_qcsapi_sockrpc --host %s wowlan_host_state wifi0 1 > %s", IPADDR_QV940, QCSAPI_TMP_FILE);
		VK_SYSTEM_Command(acWowlCmd);
		if (P_WIFI_CheckQcsapiCmdReturnValue() == DI_ERR_ERROR)
		{
			diResult = DI_ERR_ERROR;
			goto ret;
		}

		/* call_qcsapi_sockrpc --host 169.254.119.1 wowlan_match_type wifi0 0 */
		VK_snprintf(acWowlCmd, sizeof(acWowlCmd), "call_qcsapi_sockrpc --host %s wowlan_match_type wifi0 0 > %s", IPADDR_QV940, QCSAPI_TMP_FILE);
		VK_SYSTEM_Command(acWowlCmd);
		if (P_WIFI_CheckQcsapiCmdReturnValue() == DI_ERR_ERROR)
		{
			diResult = DI_ERR_ERROR;
			goto ret;
		}

		/* call_qcsapi_sockrpc --host 169.254.119.1 wowlan_udp_port wifi0 7 */
		VK_snprintf(acWowlCmd, sizeof(acWowlCmd), "call_qcsapi_sockrpc --host %s wowlan_udp_port wifi0 7 > %s", IPADDR_QV940, QCSAPI_TMP_FILE);
		VK_SYSTEM_Command(acWowlCmd);
		if (P_WIFI_CheckQcsapiCmdReturnValue() == DI_ERR_ERROR)
		{
			diResult = DI_ERR_ERROR;
			goto ret;
		}

		/* call_qcsapi_sockrpc --host 169.254.119.1 wowlan_pattern wifi0 FFFFFFFFFFFF */
		VK_snprintf(acWowlCmd, sizeof(acWowlCmd), "call_qcsapi_sockrpc --host %s wowlan_pattern wifi0 FFFFFFFFFFFF > %s", IPADDR_QV940, QCSAPI_TMP_FILE);
		VK_SYSTEM_Command(acWowlCmd);
		if (P_WIFI_CheckQcsapiCmdReturnValue() == DI_ERR_ERROR)
		{
			diResult = DI_ERR_ERROR;
			goto ret;
		}
	}
#endif /* CONFIG_QV940 */
ret:
	VK_SEM_Release(s_stWiFiInstance.ulSema);

	PrintExit();

	return diResult;
#else
	UNUSED(ulDevId);
	UNUSED(pstWowlSettings);

	PrintError("[%s][%d] This platform does not supported WoWL !! \n", __FUNCTION__, __LINE__);

	return DI_ERR_NOT_SUPPORTED;
#endif
}

DI_ERR_CODE DI_WIFI_On(HUINT32 ulDevId)
{
	DI_ERR_CODE diResult = DI_ERR_ERROR;

	UNUSED(ulDevId);

#if defined(CONFIG_QUANTENNA_BBIC_MAC)
	DRV_GPIO_Write(GPIO_ID_BBIC_MAC_RESET, GPIO_HI);
	diResult = DI_ERR_OK;
#endif

    return diResult;
}

DI_ERR_CODE DI_WIFI_Off(HUINT32 ulDevId)
{
	DI_ERR_CODE diResult = DI_ERR_ERROR;

	UNUSED(ulDevId);

#if defined(CONFIG_QUANTENNA_BBIC_MAC)
	DRV_GPIO_Write(GPIO_ID_BBIC_MAC_RESET, GPIO_LOW);
	diResult = DI_ERR_OK;
#endif

    return diResult;
}

DI_ERR_CODE DI_WIFI_GetPowerStatus(HUINT32 ulDevId, HBOOL *ulWiFiPowerStatus)
{
	DI_ERR_CODE diResult = DI_ERR_ERROR;
#if defined(CONFIG_QUANTENNA_BBIC_MAC)
	GPIO_STATE_t nPS = 1;
#endif

	UNUSED(ulDevId);
#if !defined(CONFIG_QUANTENNA_BBIC_MAC)
	UNUSED(ulWiFiPowerStatus);
#endif

#if defined(CONFIG_QUANTENNA_BBIC_MAC)
	DRV_GPIO_Read(GPIO_ID_BBIC_MAC_RESET, &nPS);
	if (nPS == GPIO_LOW)
	{
	    *ulWiFiPowerStatus = FALSE;
	}
	else if (nPS == GPIO_HI)
	{
		*ulWiFiPowerStatus = TRUE;
	}
	diResult = DI_ERR_OK;
#endif

    return diResult;
}

DRV_Error DRV_WIFI_GetPowerStatus(HUINT32 ulDevId, HBOOL *ulWiFiPowerStatus)
{
#if defined(CONFIG_QUANTENNA_BBIC_MAC)
	GPIO_STATE_t nPS = 1;
#endif

	UNUSED(ulDevId);
#if !defined(CONFIG_QUANTENNA_BBIC_MAC)
	UNUSED(ulWiFiPowerStatus);
#endif

#if defined(CONFIG_QUANTENNA_BBIC_MAC)
	DRV_GPIO_Read(GPIO_ID_BBIC_MAC_RESET, &nPS);
	if (nPS == GPIO_LOW)
	{
	    *ulWiFiPowerStatus = FALSE;
	}
	else if (nPS == GPIO_HI)
	{
		*ulWiFiPowerStatus = TRUE;
	}
#endif

	return DRV_OK;
}

DRV_Error DRV_WIFI_On(HUINT32 ulDevId)
{
	UNUSED(ulDevId);

#if defined(CONFIG_QUANTENNA_BBIC_MAC)
	DRV_GPIO_Write(GPIO_ID_BBIC_MAC_RESET, GPIO_HI);
#endif

	return DRV_OK;
}

DRV_Error DRV_WIFI_Off(HUINT32 ulDevId)
{
	UNUSED(ulDevId);

#if defined(CONFIG_QUANTENNA_BBIC_MAC)
	DRV_GPIO_Write(GPIO_ID_BBIC_MAC_RESET, GPIO_LOW);
#endif

	return DRV_OK;
}

DI_ERR_CODE DI_WIFI_GetVersion(HUINT32 ulDevId, char* ver)
{
	DI_ERR_CODE diResult = DI_ERR_ERROR;
#if defined(CONFIG_QV940)
	FILE *fp;
	char cmdBuf[255], buf[129];
#endif

	UNUSED(ulDevId);

#if defined(CONFIG_QV940)
	VK_snprintf(cmdBuf, sizeof(cmdBuf), "call_qcsapi_sockrpc --host %s get_firmware_version wifi0 > %s", IPADDR_QV940, QCSAPI_TMP_FILE);
	VK_SYSTEM_Command(cmdBuf);

	fp = fopen(QCSAPI_TMP_FILE, "r");
	if(fp != NULL)
	{
		fgets(buf, 128, fp);
		VK_sscanf(buf, "%s", ver);
		fclose(fp);
		diResult = DI_ERR_OK;
	}
#endif
	HAPPY(ver);
	return diResult;
}

DI_ERR_CODE DI_WIFI_GetWpsPinCode(HUINT32 ulDevId, char* pin)
{
	HINT32	nVkErr;
	DI_ERR_CODE diResult = DI_ERR_ERROR;
	DRV_Error drvError = DRV_ERR;

	PrintEnter();

	nVkErr = VK_SEM_Get(s_stWiFiInstance.ulSema);
	if(nVkErr != VK_OK)
	{
		PrintError("[%s][%d] Fail to get SEM (%d) \n",__FUNCTION__, __LINE__, nVkErr);
		return DI_ERR_ERROR;
	}

	if(pin == NULL)
	{
		PrintError("[%s][%d] pin is NULL !!\n", __FUNCTION__, __LINE__);
		diResult = DI_ERR_INVALID_PARAM;
		goto ret;
	}

#if defined(CONFIG_QV940)
	drvError = P_WIFI_GetQcsapiWpsPinCode(ulDevId, pin);
#else
	drvError = P_WIFI_GetWpsPinCode(ulDevId, pin);
#endif
	if (drvError != DRV_OK)
	{
		PrintError("[%s][%d] Fail to generate WPS Pin Code \n",__FUNCTION__, __LINE__);
		diResult = DI_ERR_ERROR;
		goto ret;
	}

	diResult = DI_ERR_OK;

ret:
	VK_SEM_Release(s_stWiFiInstance.ulSema);

	PrintExit();

	return diResult;
}

#if defined(CONFIG_QV940)
void DI_WIFI_SetMAC(char* mac)
{
	char cmdBuf[255];

	VK_snprintf(cmdBuf, sizeof(cmdBuf), "call_qcsapi_sockrpc --host %s set_mac_addr wifi0 %s", IPADDR_QV940, mac);
	VK_SYSTEM_Command(cmdBuf);
}

void DI_WIFI_GetMAC(char* mac)
{
	FILE *fp;
	char cmdBuf[255];
	char *filename_mac = QCSAPI_TMP_FILE;
	char buf[129];

	VK_snprintf(cmdBuf, sizeof(cmdBuf), "call_qcsapi_sockrpc --host %s get_mac_addr wifi0 > %s", IPADDR_QV940, filename_mac);
	VK_SYSTEM_Command(cmdBuf);

	fp = fopen(filename_mac, "r");
	if(fp != NULL)
	{
		fgets(buf, 128, fp);
		sscanf(buf, "%s", mac);
		fclose(fp);
	}
}
#endif

/* end of file */


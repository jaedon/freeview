/*
 * @defgroup		DI_HDD DI_HDD : DI_HDD module
 * @author			hs.choi
 * @note			DI_HDD APIs
 * @brief			Define DI_HDD APIs
 * @file 			di_hdd.c                           */

/*******************************************************************/
/**************************** Header Files *************************/
/*******************************************************************/
#if defined(CONFIG_VK_STDLIB)
#else
#include 	<stdio.h>
#endif

#include <sys/types.h>
#include <sys/stat.h>
#include <dirent.h>

#if defined(CONFIG_DI_SHARED_LIB) || defined(CONFIG_SHARED_COMPILE)
//NULL
#else
#include "linden_trace.h"
#endif

#include "vkernel.h"
#include "taskdef.h"

#include "di_uart.h"
#include "di_usb_wireless.h"
#include "drv_hotplug.h"

/*******************************************************************/
/****************************** define *****************************/
/*******************************************************************/
#if defined(CONFIG_DI_SHARED_LIB) || defined(CONFIG_SHARED_COMPILE)
//NULL
#else
#define DEBUG_MODULE	TRACE_MODULE_DI_PVR
#endif

//#define DI_WIRELESS_DEBUG

#ifndef DEBUG_MODULE
#ifdef DI_WIRELESS_DEBUG
#define PrintDebug(fmt, ...)	DI_UART_Print (fmt , ## __VA_ARGS__)
#define PrintError(fmt, ...)	DI_UART_Print (fmt , ## __VA_ARGS__)
#else
#define PrintDebug(fmt, ...)
#define PrintError(fmt, ...)	VK_printf (fmt , ## __VA_ARGS__)
#endif

#ifdef DI_WIRELESS_DEBUG
#define PrintEnter		VK_printf("(+)%s\n", __FUNCTION__)
#define PrintExit		VK_printf("(-)%s\n", __FUNCTION__)
#else
#define PrintEnter(X)
#define PrintExit(X)
//#define PrintEnter		while (0) ((int (*)(char *, ...)) 0)
//#define PrintExit		while (0) ((int (*)(char *, ...)) 0)
#endif
#endif

//#define DEBUG_DUMP_INFO

#define MAX_PATH_NUM	1024
#define DATA_NUM	256

#define DI_WIRELESS_HOTPLUG_MSG_SIZE	8

//#define SUPPORT_WIRELESS_RALINK_RT73
//#define SUPPORT_WIRELESS_RALINK_RT2870


//#define SUPPORT_EVENT_TIMEOUT
/*******************************************************************/
/******************************* extern ******************************/
/*******************************************************************/

/*******************************************************************/
/****************************** typedef ****************************/
/*******************************************************************/

struct wireless_dev_ids {
	unsigned int idVendor;	/* Vendor Id */
	unsigned int idProduct;	/* Product Id */
};

struct wireless_dev_ids rt3070_ids[] = {
	{0x148F,0x3070}, /* Ralink 3070 */
	{0x148F,0x3071}, /* Ralink 3071 */
	{0x148F,0x3072}, /* Ralink 3072 */
	{0x0DB0,0x3820}, /* Ralink 3070 */
	{0x0DF6,0x003E}, /* Sitecom 3070 */
	{0x0DF6,0x0042}, /* Sitecom 3072 */
	{0x14B2,0x3C12}, /* AL 3070 */
	{0x18C5,0x0012}, /* Corega 3070 */
	{0x083A,0x7511}, /* Arcadyan 3070 */
	{0x1740,0x9703}, /* EnGenius 3070 */
	{0x1740,0x9705}, /* EnGenius 3071 */
	{0x1740,0x9706}, /* EnGenius 3072 */
	{0x13D3,0x3273}, /* AzureWave 3070*/
	{0x1044,0x800D}, /* Gigabyte GN-WB32L 3070 */
	{0x2019,0xAB25}, /* Planex Communications, Inc. RT3070 */
	{0x07B8,0x3070}, /* AboCom 3070 */
	{0x07B8,0x3071}, /* AboCom 3071 */
	{0x07B8,0x3072}, /* Abocom 3072 */
	{0x7392,0x7711}, /* Edimax 3070 */
	{0x1A32,0x0304}, /* Quanta 3070 */
	{0x1EDA,0x2310}, /* AirTies 3070 */
	{0x07D1,0x3C0A}, /* D-Link 3072 */
	{0x07D1,0x3C0D}, /* D-Link 3070 */
	{0x07D1,0x3C0E}, /* D-Link 3070 */
	{0x07D1,0x3C0F}, /* D-Link 3070 */
	{0x1D4D,0x000C}, /* Pegatron Corporation 3070 */
	{0x1D4D,0x000E}, /* Pegatron Corporation 3070 */
	{0x5A57,0x5257}, /* Zinwell 3070 */
	{0x5A57,0x0283}, /* Zinwell 3072 */
	{0x04BB,0x0945}, /* I-O DATA 3072 */
	{0x203D,0x1480}, /* Encore 3070 */
#if defined(SUPPORT_WIRELESS_RALINK_RT2870)
	{0x148F,0x2770}, /* Ralink */
	{0x148F,0x2870}, /* Ralink */
	{0x07B8,0x2870}, /* AboCom */
	{0x07B8,0x2770}, /* AboCom */
	{0x0DF6,0x0039}, /* Sitecom 2770 */
	{0x083A,0x7512}, /* Arcadyan 2770 */
	{0x0789,0x0162}, /* Logitec 2870 */
	{0x0789,0x0163}, /* Logitec 2870 */
	{0x0789,0x0164}, /* Logitec 2870 */
	{0x177f,0x0302}, /* lsusb */
	{0x0B05,0x1731}, /* Asus */
	{0x0B05,0x1732}, /* Asus */
	{0x0B05,0x1742}, /* Asus */
	{0x0DF6,0x0017}, /* Sitecom */
	{0x0DF6,0x002B}, /* Sitecom */
	{0x0DF6,0x002C}, /* Sitecom */
	{0x0DF6,0x002D}, /* Sitecom */
	{0x14B2,0x3C06}, /* Conceptronic */
	{0x14B2,0x3C28}, /* Conceptronic */
	{0x2019,0xED06}, /* Planex Communications, Inc. */
	{0x07D1,0x3C09}, /* D-Link */
	{0x07D1,0x3C11}, /* D-Link */
	{0x14B2,0x3C07}, /* AL */
	{0x050D,0x8053}, /* Belkin */
	{0x14B2,0x3C23}, /* Airlink */
	{0x14B2,0x3C27}, /* Airlink */
	{0x07AA,0x002F}, /* Corega */
	{0x07AA,0x003C}, /* Corega */
	{0x07AA,0x003F}, /* Corega */
	{0x1044,0x800B}, /* Gigabyte */
	{0x15A9,0x0006}, /* Sparklan */
	{0x083A,0xB522}, /* SMC */
	{0x083A,0xA618}, /* SMC */
	{0x083A,0x8522}, /* Arcadyan */
	{0x083A,0x7522}, /* Arcadyan */
	{0x0CDE,0x0022}, /* ZCOM */
	{0x0586,0x3416}, /* Zyxel */
	{0x0CDE,0x0025}, /* Zyxel */
	{0x1740,0x9701}, /* EnGenius */
	{0x1740,0x9702}, /* EnGenius */
	{0x0471,0x200f}, /* Philips */
	{0x14B2,0x3C25}, /* Draytek */
	{0x13D3,0x3247}, /* AzureWave */
	{0x083A,0x6618}, /* Accton */
	{0x15c5,0x0008}, /* Amit */
	{0x0E66,0x0001}, /* Hawking */
	{0x0E66,0x0003}, /* Hawking */
	{0x129B,0x1828}, /* Siemens */
	{0x157E,0x300E},	/* U-Media */
	{0x050d,0x805c},
	{0x050d,0x815c},
	{0x1482,0x3C09}, /* Abocom*/
	{0x14B2,0x3C09}, /* Alpha */
	{0x04E8,0x2018}, /* samsung */
	{0x5A57,0x0280}, /* Zinwell */
	{0x5A57,0x0282}, /* Zinwell */
	{0x7392,0x7718},
	{0x7392,0x7717},
	{0x1737,0x0070}, /* Linksys WUSB100 */
	{0x1737,0x0071}, /* Linksys WUSB600N */
	{0x0411,0x00e8}, /* Buffalo WLI-UC-G300N*/
	{0x050d,0x815c}, /* Belkin F5D8053 */
#endif
	{0,0}
};

#if defined(SUPPORT_WIRELESS_RALINK_RT73)
struct wireless_dev_ids rt73_ids[] = {
	{0x148f,0x2573}, /* Ralink */
	{0x148f,0x2671}, /* Ralink */
	{0x04bb,0x093d}, /* Ralink */
	{0x18e8,0x6196}, /* Qcom */
	{0x18e8,0x6229}, /* Qcom */
	{0x18e8,0x6238}, /* Qcom */
	{0x1044,0x8008}, /* Gigabyte */
	{0x14b2,0x3c22}, /* Conceptronic */
	{0x0db0,0x6877}, /* MSI */
	{0x0db0,0xa874}, /* MSI */
	{0x0db0,0xa861}, /* MSI */
	{0x07b8,0xb21d}, /* AboCom */
	{0x0769,0x31f3}, /* Surecom */
	{0x1472,0x0009}, /* Huawei-3Com */
	{0x1371,0x9022}, /* CNet */
	{0x1371,0x9032}, /* CNet */
	{0x1631,0xc019}, /* Billionton */
	{0x1044,0x800a}, /* Gigabyte */
	{0x1690,0x0722}, /* Askey */
	{0x0df6,0x9712}, /* Sitecom WL-113 v1 002 */
	{0x0df6,0x90ac}, /* Sitecom */
	{0x148f,0x9021}, /* Amigo */
	{0x0eb0,0x9021}, /* Amigo */
	{0x0411,0x00D8}, /* BUFFALO WLI-U2-SG54HP */
	{0x0586,0x3415}, /* ZyXEL */
	{0x0b05,0x1723}, /* ASUS */
	{0x0b05,0x1724}, /* ASUS */
	{0x050d,0x705a}, /* Belkin */
	{0x07d1,0x3c03}, /* D-Link */
	{0x13b1,0x0023}, /* Linksys */
	{0x13b1,0x0020}, /* Linksys WUS54GC */
	{0,0}
};
#endif

struct wireless_dev_ids ath9k_hif_usb_ids[] = {
	{0x055d, 0x9271}, /* Atheros */
	{0x0cf3, 0x9271}, /* Atheros */
	{0x0cf3, 0x9342}, /* Atheros */
	{0x0cf3, 0x7015}, /* Atheros */
	{0x0846, 0x9030}, /* Netgear N150 */
	{0x0846, 0x9018}, /* Netgear WNDA3200 */
	{0x07D1, 0x3A10}, /* Dlink Wireless 150 */
	{0x13D3, 0x3327}, /* Azurewave */
	{0x13D3, 0x3328}, /* Azurewave */
	{0x13D3, 0x3346}, /* IMC Networks */
	{0x13D3, 0x3348}, /* Azurewave */
	{0x13D3, 0x3349}, /* Azurewave */
	{0x13D3, 0x3350}, /* Azurewave */
	{0x04CA, 0x4605}, /* Liteon */
	{0x083A, 0xA704}, /* SMC Networks */
	{0x040D, 0x3801}, /* VIA */
	{0x1668, 0x1200}, /* Verizon */
	{0,0}
};

#if defined(CONFIG_RT3572)
struct wireless_dev_ids RT35xx_usb_ids[] = {
	{0x148F,0x3572}, /* Ralink 3572 */
	{0x1740,0x9801}, /* EnGenius 3572 */
	{0x0DF6,0x0041}, /* Sitecom 3572 */
	{0x0DF6,0x0042},
	{0x04BB,0x0944}, /* I-O DATA 3572 */
	{0x1690,0x0740}, /* 3572 */
	{0x1690,0x0744}, /* 3572 */
	{0x5A57,0x0284}, /* Zinwell 3572 */
	{0x167B,0x4001}, /* 3572 */
	{0x0930,0x0A07}, /* TOSHIBA */
	{0,0}
};
#endif

#if defined(CONFIG_RT5572)
struct wireless_dev_ids RT55xx_usb_ids[] = {
	{0x148F,0x5572}, /* Ralink 5572 */
	{0,0}
};
#endif

#if defined(CONFIG_BCM43236) || defined(CONFIG_BCM43236_MFG) /* __HUMAX_CODE__ - 150204 mskim (redmine #100538) */
    struct wireless_dev_ids bcm43236b_usb_ids[] = {
           {0x0a5c, 0xbd17},       /* BCM43236B init */
           {0x0a5c, 0xbdc},        /* BCM43236B after nvram download */
           {0, 0}
    };
#endif

#if defined(CONFIG_MOBILE_NETWORK)  /* __HUMAX_CODE__ - 150316 mskim (redmine #101080/#101503) */
struct wireless_dev_ids mobile_dongle_ids[] = {
#if defined(CONFIG_MOBILE_KENYA)
    { 0x19d2, 0x0117 }, /* Equitel      - ZTE MF667     */
//  { 0x19d2, 0x0117 }, /* Airtel       - ZTE MF667     */
    { 0x19d2, 0x1270 }, /* Orange       - ZTE MF667     */
    { 0x12d1, 0x1001 }, /* Safaricom    - HUAWEI E3531  */
#endif
#if defined(CONFIG_MOBILE_KOREA)
    { 0x1004, 0x61e6 }, /* SKT          - LG SD711      */
#endif
	{ 0, 0 }
};
#endif

/*******************************************************************/
/************************ global variables *************************/
/*******************************************************************/

/*******************************************************************/
/************************ static variables *************************/
/*******************************************************************/
static pfnDI_USB_WIRELESS_NotifyFUNC CB_USB_WirelessNotify = NULL;
static unsigned long ulWirelessMsgId;
static unsigned long ulWirelessTaskId;
static usb_wireless_info_t w_info[2];
static unsigned int uiport1scan=0, uiport2scan=0;

#if defined(DEBUG_DUMP_INFO)
static void P_HOTPLUG_DumpInfo(int idx)
{
	VK_printf("[%d]%s Device Count = %ld\n",__LINE__,__func__,w_info[idx].devCount);
	VK_printf("[%d]%s DriverName = %s\n",__LINE__,__func__,w_info[idx].devName);
	VK_printf("[%d]%s DriverName = %p\n",__LINE__,__func__,&w_info[idx].devName);
	VK_printf("[%d]%s Port Num = %d\n",__LINE__,__func__,w_info[idx].ePortNum);
}
#endif

static void DRV_WIRELESS_Task(void *pvParam)
{
	int	ret;
	hp_evt_t *php_evt;
	hp_evt_t hp_evt;
	DRV_HP_CONNECT_EVT eConEvt=DRV_HP_DISCONN;
	(void)pvParam;
	unsigned int i;
	HBOOL	bFoundFlags = FALSE;
	char 	buf[12] = {0,};

	while (CB_USB_WirelessNotify == NULL)
	{
		VK_TASK_Sleep(1000);
	}

	while(1)
	{

		bFoundFlags = FALSE;
#if defined(SUPPORT_EVENT_TIMEOUT)
		ret=VK_MSG_ReceiveTimeout(ulWirelessMsgId, &hp_evt, sizeof(hp_evt_t), 100);
#else
		ret=VK_MSG_Receive(ulWirelessMsgId, &hp_evt, sizeof(hp_evt_t));
#endif
		if(ret != 0 && uiport1scan == 0 && uiport2scan == 0)
			continue;

		php_evt=&hp_evt;

		if((php_evt == NULL) && (uiport1scan == 0) && (uiport2scan == 0))
		{
			continue;
		}

		if(php_evt != NULL)
			eConEvt=php_evt->eConnEvt;

		if((uiport1scan != 0) || (uiport2scan != 0))
			eConEvt=DRV_HP_CONN;

		switch(eConEvt)
		{
		case DRV_HP_CONN:
				/* We can't know which wireless driver is inserted. */
				/* Try to compare all of known idvendor and idproduct in case by case */
				if((php_evt != NULL) && (uiport1scan == 0 || uiport2scan == 0))
				{
#if defined(SUPPORT_WIRELESS_RALINK_RT73)
					for(i=0;i<(sizeof(rt73_ids)/sizeof(struct wireless_dev_ids));i++)
					{
						if((php_evt->idvendor == rt73_ids[i].idVendor)
							&& (php_evt->idproduct == rt73_ids[i].idProduct))
						{
							w_info[php_evt->portnum-1].devCount=1;
							w_info[php_evt->portnum-1].ePortNum = (DI_USB_Wireless_portnum_t)php_evt->portnum;
							VK_MEM_Memset(w_info[php_evt->portnum-1].devName,0x00,12);
							VK_MEM_Memcpy(w_info[php_evt->portnum-1].devName,"rt73",4);
							bFoundFlags = TRUE;
						}
					}
#endif

					if(bFoundFlags == FALSE)
					{
						for(i=0;i<(sizeof(rt3070_ids)/sizeof(struct wireless_dev_ids));i++)
						{
							if((php_evt->idvendor == rt3070_ids[i].idVendor)
								&& (php_evt->idproduct == rt3070_ids[i].idProduct))
							{
								w_info[php_evt->portnum-1].devCount=1;
								w_info[php_evt->portnum-1].ePortNum = (DI_USB_Wireless_portnum_t)php_evt->portnum;
								VK_MEM_Memset(w_info[php_evt->portnum-1].devName,0x00,12);
								VK_MEM_Memcpy(w_info[php_evt->portnum-1].devName,"rt3070sta",9);
								bFoundFlags = TRUE;
							}
						}
					}

					if(bFoundFlags == FALSE)
					{
						for(i=0;i<(sizeof(ath9k_hif_usb_ids)/sizeof(struct wireless_dev_ids));i++)
						{
							if((php_evt->idvendor == ath9k_hif_usb_ids[i].idVendor)
								&& (php_evt->idproduct == ath9k_hif_usb_ids[i].idProduct))
							{
								w_info[php_evt->portnum-1].devCount=1;
								w_info[php_evt->portnum-1].ePortNum = (DI_USB_Wireless_portnum_t)php_evt->portnum;
								VK_MEM_Memset(w_info[php_evt->portnum-1].devName,0x00,12);

								VK_snprintf(buf, sizeof(buf),"ar%x", php_evt->idproduct);
								VK_MEM_Memcpy(w_info[php_evt->portnum-1].devName, buf, sizeof(buf));
								bFoundFlags = TRUE;
							}
						}
					}

#if defined(CONFIG_RT3572)
					if(bFoundFlags == FALSE)
					{
						for(i=0;i<(sizeof(RT35xx_usb_ids)/sizeof(struct wireless_dev_ids));i++)
						{
							if((php_evt->idvendor == RT35xx_usb_ids[i].idVendor)
								&& (php_evt->idproduct == RT35xx_usb_ids[i].idProduct))
							{
								w_info[php_evt->portnum-1].devCount=1;
								w_info[php_evt->portnum-1].ePortNum = (DI_USB_Wireless_portnum_t)php_evt->portnum;
								VK_MEM_Memset(w_info[php_evt->portnum-1].devName,0x00,12);
								VK_MEM_Memcpy(w_info[php_evt->portnum-1].devName,"rt35xx", 6);
								bFoundFlags = TRUE;
							}
						}
					}
#endif

#if defined(CONFIG_RT5572)
					if(bFoundFlags == FALSE)
					{
						for(i=0;i<(sizeof(RT55xx_usb_ids)/sizeof(struct wireless_dev_ids));i++)
						{
							if((php_evt->idvendor == RT55xx_usb_ids[i].idVendor)
								&& (php_evt->idproduct == RT55xx_usb_ids[i].idProduct))
							{
								w_info[php_evt->portnum-1].devCount=1;
								w_info[php_evt->portnum-1].ePortNum = (DI_USB_Wireless_portnum_t)php_evt->portnum;
								VK_MEM_Memset(w_info[php_evt->portnum-1].devName,0x00,12);
								VK_MEM_Memcpy(w_info[php_evt->portnum-1].devName,"rt55xx", 6);
								bFoundFlags = TRUE;
							}
						}
					}
#endif

#if defined(CONFIG_BCM43236) || defined(CONFIG_BCM43236_MFG) /* __HUMAX_CODE__ - 150204 mskim (redmine #100538) */
					if(bFoundFlags == FALSE)
					{
						for(i=0;i<(sizeof(bcm43236b_usb_ids)/sizeof(struct wireless_dev_ids));i++)
						{
							if((php_evt->idvendor == bcm43236b_usb_ids[i].idVendor)
								&& (php_evt->idproduct == bcm43236b_usb_ids[i].idProduct))
							{
								w_info[php_evt->portnum-1].devCount=1;
								w_info[php_evt->portnum-1].ePortNum = (DI_USB_Wireless_portnum_t)php_evt->portnum;
								VK_MEM_Memset(w_info[php_evt->portnum-1].devName,0x00,12);
								VK_MEM_Memcpy(w_info[php_evt->portnum-1].devName, "BCM43236", 8);
								bFoundFlags = TRUE;
							}
						}
					}
#endif

#if defined(CONFIG_MOBILE_NETWORK)  /* __HUMAX_CODE__ - 150316 mskim (redmine #101080/#101503) */
					if(bFoundFlags == FALSE)
					{
						for(i=0;i<(sizeof(mobile_dongle_ids)/sizeof(struct wireless_dev_ids));i++)
						{
							if((php_evt->idvendor == mobile_dongle_ids[i].idVendor)
								&& (php_evt->idproduct == mobile_dongle_ids[i].idProduct))
							{
								w_info[php_evt->portnum-1].devCount=1;
								w_info[php_evt->portnum-1].ePortNum = (DI_USB_Wireless_portnum_t)php_evt->portnum;
								VK_MEM_Memset(w_info[php_evt->portnum-1].devName,0x00,12);
								VK_MEM_Memcpy(w_info[php_evt->portnum-1].devName, "MOBILE", 7);
								bFoundFlags = TRUE;
								w_info[php_evt->portnum-1].idVendor = mobile_dongle_ids[i].idVendor;
								w_info[php_evt->portnum-1].idProduct = mobile_dongle_ids[i].idProduct;

							}
						}
					}
#endif
				}

#if defined(DEBUG_DUMP_INFO)
				if(php_evt != NULL)
				{
					P_HOTPLUG_DumpInfo(php_evt->portnum-1);
				}
				else
				{
					if(uiport1scan == 1)
						P_HOTPLUG_DumpInfo(0);
					if(uiport2scan == 1)
						P_HOTPLUG_DumpInfo(1);
				}
#endif
				if(CB_USB_WirelessNotify != NULL)
				{
					if(uiport1scan == 1)
					{
						PrintDebug("[DRV_WIRELESS_Task]  Insert USB WIRELESS port %d : %s\n", 1, w_info[0].devName);
						CB_USB_WirelessNotify(DI_USB_WIRELESS_Insert,(void *)&w_info[0]);
						uiport1scan=0;
					}

					if(uiport2scan == 1)
					{
						PrintDebug("[DRV_WIRELESS_Task]  Insert USB WIRELESS port %d : %s\n", 2, w_info[0].devName);
						CB_USB_WirelessNotify(DI_USB_WIRELESS_Insert,(void *)&w_info[1]);
						uiport2scan=0;
					}

					if((uiport1scan == 0) && (uiport2scan == 0) && (bFoundFlags == TRUE))
					{
						PrintDebug("[DRV_WIRELESS_Task]2 Insert USB WIRELESS port %d : %s\n", php_evt->portnum, w_info[0].devName);
						CB_USB_WirelessNotify(DI_USB_WIRELESS_Insert,(void *)&w_info[php_evt->portnum-1]);
					}

				}
				else
				{
					if(php_evt == NULL)
						continue;

					if(php_evt->portnum == 1)
						uiport1scan = 1;

					if(php_evt->portnum == 2)
						uiport2scan = 1;
				}

			break;
		case DRV_HP_DISCONN:
				if(php_evt == NULL)
					continue;

				if (w_info[php_evt->portnum-1].devName[0] != 0)
				{
					if(CB_USB_WirelessNotify != NULL)
					{
						PrintDebug("[DRV_WIRELESS_Task] extract USB WIRELESS port %d : %s\n", php_evt->portnum, w_info[0].devName);
						CB_USB_WirelessNotify(DI_USB_WIRELESS_Extract,(void *)&w_info[php_evt->portnum-1]);
					}
					VK_MEM_Memset(w_info[php_evt->portnum-1].devName,0x00,12);
				}
				if(php_evt->portnum == 1)
					uiport1scan = 0;

				if(php_evt->portnum == 2)
					uiport2scan = 0;

			break;
		default:
			break;
		}
	}
}

DRV_Error DRV_USB_WIRELESS_Init(void)
{
	int ret;

	PrintEnter();

	ret = VK_MSG_Create(DI_WIRELESS_HOTPLUG_MSG_SIZE, sizeof(hp_evt_t), "WirelessHotplugMsg", &ulWirelessMsgId, VK_SUSPENDTYPE_FIFO);
	if(ret != VK_OK)
	{
		PrintError("[%s] Error. can't create msg queue\n", __FUNCTION__);
		return DRV_ERR;
	}

	ret=VK_TASK_Create(DRV_WIRELESS_Task,USB_TASK_PRIORITY,USB_TASK_STACK_SIZE,"Wireless_Task",NULL,&ulWirelessTaskId,FALSE);
	if(ret != VK_OK)
	{
		PrintError("%s[%d] : VK_TASK_Create() error\n",__func__,__LINE__);
		return DRV_ERR;
	}
	else
	{
		VK_TASK_Start(ulWirelessTaskId);
	}

	DRV_HOTPLUG_MsgRegister(DRV_HP_USB_WIRELESS, ulWirelessMsgId);

	PrintExit();

	return DRV_OK;
}

void DI_USB_WIRELESS_RegisterCallback(pfnDI_USB_WIRELESS_NotifyFUNC pfnNotify)
{
	CB_USB_WirelessNotify = pfnNotify;
}

DI_ERR_CODE DI_USB_WIRELESS_Init(void)
{
	DRV_Error err;

	err = DRV_USB_WIRELESS_Init();
	if (err != DRV_OK) {
		return DI_ERR_ERROR;
	}

	return DI_ERR_OK;
}

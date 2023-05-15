/*
** drv_pm.c
**

**
** @defgroup
** @author
** @note
** @brief
** @file
*/

/*----------------------------------------------------------------------------------
** Headers
**--------------------------------------------------------------------------------*/
#if defined(CONFIG_VK_STDLIB)
#else
#include 	<stdio.h>
#include 	<stdlib.h>
#include 	<string.h>
#include 	<malloc.h>
#include 	<sys/reboot.h>
#endif
#if defined(CONFIG_BRCM_MOD_NEXUS_POWER_MANAGEMENT)
#include <unistd.h>
#include <signal.h>
#include <net/if.h>
#include <sys/ioctl.h>
#include <sys/socket.h>
#include <linux/sockios.h>
#include <ifaddrs.h>
#include <dirent.h>
#include <fcntl.h>
#endif
#include "htype.h"
#include <time.h>

#include <sys/stat.h>

#include "drv_err.h"
#include "drv_led.h"
#include "drv_pm.h"
#include "drv_gpio.h"
#include "vkernel.h"
#if defined(CONFIG_DI_SERVER_CLIENT)
#include "di_power_server_client.h"
#endif
#include "nexus_platform.h"
#include "nexus_platform_standby.h"

#if defined(CONFIG_CAS_NA)
#include "bchp_aon_ctrl.h"
#endif

/*----------------------------------------------------------------------------------
** defines
**--------------------------------------------------------------------------------*/
#if defined(CONFIG_DEBUG)
	#define DRV_PM_DEBUG
	#if defined(DRV_PM_DEBUG)
	#define PrintError	VK_printf
	#define PrintDebug	VK_printf
	#else
	#define PrintError	VK_printf
	#define PrintDebug
	#endif
#else
	#define PrintError(...)		VK_DBG_Print(__VA_ARGS__)
	#define PrintDebug(...)		((void)0)
#endif

#if defined(CONFIG_CAS_NA)
/*
[Scratch Register]

  4     4    4    4   4    4   4   4   (bit)
--------------------------------------
| A  |  B | rsv | loader | wakup | C |
|    |    |     | upgrad | reason|   |
--------------------------------------
*/
#define SCRATCH_REG_MAGIC_NUM             0xAB00000C
#define SCRATCH_REG_MAGIC_NUM_MASK        0xFF00000F
#define SCRATCH_REG_WAKEUP_REASON_MASK    0x00000FF0
#define SCRATCH_REG_WAKEUP_REASON_SHFT    4

#if defined(CONFIG_ARM) && defined(BCHP_AON_CTRL_SYSTEM_DATA_RAMi_ARRAY_BASE)
	#define AON_SCRATCH_PTR1	(BCHP_AON_CTRL_SYSTEM_DATA_RAMi_ARRAY_BASE+(0xd * 0x4))
#elif defined(CONFIG_MIPS) && defined(BCHP_AON_CTRL_UNCLEARED_SCRATCH)
	#define AON_SCRATCH_PTR1    (BCHP_AON_CTRL_UNCLEARED_SCRATCH)
#endif
#endif

#if defined(CONFIG_BRCM_MOD_NEXUS_POWER_MANAGEMENT)
#ifndef SIOCETHTOOL
#define SIOCETHTOOL    0x8946
#endif
#define SOPASS_MAX    6
#define ETHTOOL_GWOL    0x00000005 /* Get wake-on-lan options. */
#define ETHTOOL_SWOL    0x00000006 /* Set wake-on-lan options. */

/* Wake-On-Lan options. */
#define WAKE_PHY		(1 << 0)
#define WAKE_UCAST		(1 << 1)
#define WAKE_MCAST		(1 << 2)
#define WAKE_BCAST		(1 << 3)
#define WAKE_ARP		(1 << 4)
#define WAKE_MAGIC		(1 << 5)
#define WAKE_MAGICSECURE	(1 << 6) /* only meaningful if WAKE_MAGIC */
#endif

/*----------------------------------------------------------------------------------
** typedef
**--------------------------------------------------------------------------------*/
typedef struct _drv_pm_f_t{
	struct _drv_pm_f_t *next;
	pfn_pm_cb	pCb;
}drv_pm_f_t;

#if defined(CONFIG_BRCM_MOD_NEXUS_POWER_MANAGEMENT)
typedef struct  {
	unsigned int    cmd;
	unsigned int    supported;
	unsigned int    wolopts;
    unsigned char    sopass[SOPASS_MAX]; /* SecureOn(tm) password */
}drv_pm_wol_wolinfo_t;
#endif

/*----------------------------------------------------------------------------------
** Variables
**--------------------------------------------------------------------------------*/
static drv_pm_f_t *pfn_pm_uninit_head = NULL;
static drv_pm_f_t *pfn_pm_reinit_head = NULL;

HBOOL s_bDemodPower = TRUE;

#if defined(CONFIG_BRCM_MOD_NEXUS_POWER_MANAGEMENT)
HBOOL	bInSleep = FALSE;
static DRV_MICOM_WAKEUP_SRC s_nWakeupSrc = DRV_MICOM_WAKEUP_BY_MAX;
static HCHAR *sEthIfName = "eth0";
static HCHAR *sMocaIfName = "eth1";
static HINT32 sWolWakeupCount = -1;
#if defined(CONFIG_WIFI) && defined(CONFIG_SUPPORT_WOWL)
static HUINT8 sWakeupByWlan = FALSE;
#endif
#endif

/*----------------------------------------------------------------------------------
** Functions
**--------------------------------------------------------------------------------*/
DRV_Error DRV_PM_RegisterCallback(pfn_pm_cb pCbUninit, pfn_pm_cb  pCbReinit)
{
	drv_pm_f_t *pnew = NULL;
	drv_pm_f_t *p;


	if(pCbUninit != NULL)
	{
		pnew = VK_MEM_Alloc(sizeof(drv_pm_f_t));
		if(pnew == NULL)
		{
			PrintError("failed to alloc memory.\n");
			return DRV_ERR;
		}

		if(pfn_pm_uninit_head == NULL)
		{
			pfn_pm_uninit_head = pnew;
			pfn_pm_uninit_head->pCb = pCbUninit;
			pfn_pm_uninit_head->next = pfn_pm_uninit_head;
		}
		else
		{
			p = pfn_pm_uninit_head;
			while(p->next != pfn_pm_uninit_head)
				p = p->next;
			p->next = pnew;
			pnew->pCb = pCbUninit;
			pnew->next = pfn_pm_uninit_head;
		}
	}

	if(pCbReinit != NULL)
	{
		pnew = VK_MEM_Alloc(sizeof(drv_pm_f_t));
		if(pnew == NULL)
		{
			PrintError("failed to alloc memory.\n");
			return DRV_ERR;
		}

		if(pfn_pm_reinit_head == NULL)
		{
			pfn_pm_reinit_head = pnew;
			pfn_pm_reinit_head->pCb = pCbReinit;
			pfn_pm_reinit_head->next = pnew;
		}
		else
		{
			p = pfn_pm_reinit_head;
			while(p->next != pfn_pm_reinit_head)
				p = p->next;
			p->next = pnew;
			p = pfn_pm_reinit_head;
			pfn_pm_reinit_head = pnew;
			pnew->pCb = pCbReinit;
			pnew->next = p;
		}
	}
	return DRV_OK;
}

void DRV_PM_PowerDown(void)
{
	drv_pm_f_t *p;
	if(pfn_pm_uninit_head == NULL)
	{
		return;
	}
	p = pfn_pm_uninit_head;
	while(1)
	{
		if(p->pCb != NULL)
			p->pCb();
		p = p->next;
		if(p == pfn_pm_uninit_head)
			break;
	}
	return;
}

void DRV_PM_PowerUp(void)
{
	drv_pm_f_t *p;
	if(pfn_pm_reinit_head == NULL)
	{
		return;
	}
	p = pfn_pm_reinit_head;
	while(1)
	{
		if(p->pCb != NULL)
			p->pCb();
		p = p->next;
		if(p == pfn_pm_reinit_head)
			break;
	}
	return;
}

#if defined(CONFIG_BRCM_MOD_NEXUS_POWER_MANAGEMENT)
/* bInSleep is shared with all di thread. so just sending signal to any 1 thread will be OK.*/
void standby_task(int sig, VK_SIG_INFO_t *siginfo_t, void *context)
{
	HAPPY(sig);
	HAPPY(siginfo_t);
	HAPPY(context);
	PrintDebug("[%s] Set bInSleep to TRUE\n", __func__);
	bInSleep = TRUE;

}
void resume_task(int sig, VK_SIG_INFO_t *siginfo_t, void *context)
{
	HAPPY(sig);
	HAPPY(siginfo_t);
	HAPPY(context);
	PrintDebug("[%s] Set bInSleep to FALSE\n",__func__);
	bInSleep = FALSE;
}

static DRV_Error P_PM_DDRPower_KeepOn(HBOOL bOn)
{
	return DRV_GPIO_Write(GPIO_ID_BOOT_MODE, ((bOn)?GPIO_HI:GPIO_LOW));
}

#if defined(CONFIG_EXT_DEMOD_POWER_CONTROL)
static DRV_Error P_PM_DemodPowerSet(HBOOL bOn)
{
	return DRV_GPIO_Write(GPIO_ID_LOOPTHROUGH, ((bOn)?GPIO_HI:GPIO_LOW));
}
#endif

static void P_PM_WOL_GetIfName(void)
{
	struct ifaddrs *ifaddr, *ifa;
	int rc;

	rc = getifaddrs(&ifaddr);
	if (rc < 0) goto done;

	for (ifa = ifaddr; ifa != NULL; ifa = ifa->ifa_next)
	{
		if (ifa->ifa_addr == NULL)
			continue;

		if(!strncmp(ifa->ifa_name, "gphy", 4))
			sEthIfName = "gphy";
		else if(!strncmp(ifa->ifa_name, "moca", 4))
			sMocaIfName = "moca";
	}

	freeifaddrs(ifaddr);
done:
	return;
}

static void P_PM_WOL_GetWol(HCHAR *devname, HINT32 *wolopts)
{
#if defined(CONFIG_ETH_EXT_PHY)
	HAPPY(devname);
	HAPPY(wolopts);
#else
    struct ifreq ifr;
	int fd =- 1, err;
    drv_pm_wol_wolinfo_t wol;

	DONE_ERR((!devname));
	DONE_ERR((!wolopts));

	*wolopts = 0;

    /* Setup our control structures. */
	VK_memset(&ifr, 0, sizeof(ifr));
	VK_snprintf(ifr.ifr_name, sizeof(ifr.ifr_name), "%s", devname);

    /* Open control socket. */
	fd = socket(AF_INET, SOCK_DGRAM, 0);
	if (fd < 0) goto done;

    wol.cmd = ETHTOOL_GWOL;
    ifr.ifr_data = (caddr_t)&wol;
    err = ioctl(fd, SIOCETHTOOL, &ifr);
    if (err < 0) goto done;

    *wolopts = wol.supported;
done:
    if (fd >= 0) close(fd);
	return;
#endif
}

static void P_PM_WOL_SetWol(char *devname, unsigned int wolopts)
{
#if defined(CONFIG_ETH_EXT_PHY)
	HAPPY(devname);
	HAPPY(wolopts);
#else
    struct ifreq ifr;
	int fd = -1, err;
    drv_pm_wol_wolinfo_t wol;

	/* Setup our control structures. */
	VK_memset(&ifr, 0, sizeof(ifr));
	VK_snprintf(ifr.ifr_name, sizeof(ifr.ifr_name), "%s", devname);

	/* Open control socket. */
	fd = socket(AF_INET, SOCK_DGRAM, 0);
	if (fd < 0) goto done;

    wol.wolopts = wolopts;
    wol.cmd = ETHTOOL_SWOL;
    ifr.ifr_data = (caddr_t)&wol;
    err = ioctl(fd, SIOCETHTOOL, &ifr);
    if (err < 0) goto done;

done:
    if (fd >= 0) close(fd);
	return;
#endif	
}

static int P_PM_WOL_GetWakeupCount(void)
{
#if defined(CONFIG_ETH_EXT_PHY)
	return 0;
#else
#define BCMGENET_SYSFS    "/sys/bus/platform/drivers/bcmgenet"
	DIR * dir = opendir(BCMGENET_SYSFS);
	struct dirent *ent;
	char wakeup_count_file[256] = {0,};
	FILE *fp = NULL;
	int wakeup_count = 0;
	char **p = NULL;
	
	if(dir) {
		while ((ent = readdir(dir)) != NULL)
		{
			if(VK_strstr(ent->d_name, "ethernet"))
			{
				VK_snprintf(wakeup_count_file, sizeof(wakeup_count_file), "%s/%s/%s", BCMGENET_SYSFS, ent->d_name, "power/wakeup_count");
				break;
			}
		}
		closedir(dir);
	}

	if (wakeup_count_file[0] == 0)
		goto done;

	fp = fopen(wakeup_count_file, "r");
	if (!fp) goto done;

	VK_memset(wakeup_count_file, 0,  sizeof(wakeup_count_file));
	if (VK_fgets((char*)wakeup_count_file, sizeof(wakeup_count_file), fp) == NULL)
		goto done;

	wakeup_count_file[sizeof(wakeup_count_file)-1] = 0;
	wakeup_count = strtol(wakeup_count_file, p, 10);
done:
	if (fp) fclose(fp);
	return wakeup_count;
#endif
}

/* Get and save wake up reason before erased */
static DRV_Error P_PM_GetWakeUpStatus(void)
{
	NEXUS_PlatformStandbyStatus stStatus;
	int t;

	VK_memset(&stStatus, 0, sizeof(NEXUS_PlatformStandbyStatus));
#if defined(CONFIG_DI_SERVER_CLIENT)
	(void)DI_POWER_CLIENT_GetStandbyStatus((void*)&stStatus);
#else
	NEXUS_Platform_GetStandbyStatus(&stStatus);
#endif
	
	if (stStatus.wakeupStatus.timeout == TRUE)
	{	
		s_nWakeupSrc = DRV_MICOM_WAKEUP_BY_TIMER;
	}
	else if (stStatus.wakeupStatus.gpio == TRUE)
	{
		s_nWakeupSrc = DRV_MICOM_WAKEUP_BY_KEY;
#if defined(CONFIG_WIFI) && defined(CONFIG_SUPPORT_WOWL)
		s_nWakeupSrc = (sWakeupByWlan)?DRV_MICOM_WAKEUP_BY_WOL:s_nWakeupSrc;		
#endif
	}
	else if (stStatus.wakeupStatus.ir == TRUE)
	{	
		s_nWakeupSrc = DRV_MICOM_WAKEUP_BY_RCU;
	}	
	else if ((t = P_PM_WOL_GetWakeupCount()) != sWolWakeupCount)
	{
		s_nWakeupSrc = DRV_MICOM_WAKEUP_BY_WOL;
		sWolWakeupCount = t;
	}
#if defined(CONFIG_MIPS) && (NEXUS_VERSION == 1502)
	else if (stStatus.wakeupStatus.wol_enet == TRUE)
	{
		s_nWakeupSrc = DRV_MICOM_WAKEUP_BY_WOL;
	}
#endif
	else
	{	
		s_nWakeupSrc = DRV_MICOM_WAKEUP_BY_ACPOWER;
	}

	PrintError("************************************************\n");
	PrintError("** DRV_PM_StandbyStates!!\n");
	PrintError("** \tIR Status : %d\n", stStatus.wakeupStatus.ir);
	PrintError("** \tUHF Status : %d\n", stStatus.wakeupStatus.uhf);
	PrintError("** \tTrasport : %d\n", stStatus.wakeupStatus.transport);
	PrintError("** \tCEC: %d\n", stStatus.wakeupStatus.cec);
	PrintError("** \tGPIO : %d\n", stStatus.wakeupStatus.gpio);
	PrintError("** \t\tWOL : %d\n", (s_nWakeupSrc==DRV_MICOM_WAKEUP_BY_WOL)?1:0);
	PrintError("** \tTimer : %d\n", stStatus.wakeupStatus.timeout);
	PrintError("************************************************\n");

	return DRV_OK;
}

static void P_PM_SysCall(char *pszCmd)
{
	int rc = 0;
	
#if defined(CONFIG_DI_SERVER_CLIENT)
	rc = (DI_POWER_CLIENT_SetSysFs(pszCmd) == DI_ERR_OK) ? 0 : -1;
#else
	if (VK_strncmp(pszCmd, "reboot", VK_strlen("reboot")) == 0)
	{
		sync();
		reboot(RB_AUTOBOOT);
	}
	else
	{
		rc = (VK_SYSTEM_Command(pszCmd) == VK_OK) ? 0 : -1;	
	}
#endif

	if (rc)
		PrintError("[%s] Error. can't excute command\n", __FUNCTION__);
	
	return;
}

static int P_PM_SetPlatformSetting(NEXUS_PlatformStandbySettings *pSetting)
{
	int rc;
#if defined(CONFIG_DI_SERVER_CLIENT)
	rc = (DI_POWER_CLIENT_SetStandbySetting((void*)pSetting, sizeof(NEXUS_PlatformStandbySettings)) == DI_ERR_OK) ? 0 : -1;
#else
	rc = (NEXUS_Platform_SetStandbySettings(pSetting) == NEXUS_SUCCESS) ? 0 : -1;
#endif

	if (rc) PrintError("[%s] Error! platform setting fails\n", __FUNCTION__);
	return rc;
}

#if defined(CONFIG_CAS_NA)
static void P_PM_SaveWakeupReasonFromS2(void)
{
#if defined(AON_SCRATCH_PTR1)
	HUINT32 reg_val = 0;

	NEXUS_Platform_ReadRegister(AON_SCRATCH_PTR1, &reg_val);
	reg_val &= ~(SCRATCH_REG_MAGIC_NUM_MASK | SCRATCH_REG_WAKEUP_REASON_MASK);
	reg_val |= (SCRATCH_REG_MAGIC_NUM | (s_nWakeupSrc << SCRATCH_REG_WAKEUP_REASON_SHFT));
	NEXUS_Platform_WriteRegister(AON_SCRATCH_PTR1, reg_val);
#endif
}

static void P_PM_GetWakeupReasonFromS2(void)
{
#if defined(AON_SCRATCH_PTR1)
	HUINT32 reg_val = 0;

	NEXUS_Platform_ReadRegister(AON_SCRATCH_PTR1, &reg_val);	
	
	if ((reg_val&SCRATCH_REG_MAGIC_NUM) == SCRATCH_REG_MAGIC_NUM)
	{
		s_nWakeupSrc = (DRV_MICOM_WAKEUP_SRC)((reg_val & SCRATCH_REG_WAKEUP_REASON_MASK)>>SCRATCH_REG_WAKEUP_REASON_SHFT);

		/* clear */
		reg_val &= ~SCRATCH_REG_WAKEUP_REASON_MASK;
		NEXUS_Platform_WriteRegister(AON_SCRATCH_PTR1, reg_val);
		
	}
#endif
}
#endif

#if defined(CONFIG_WIFI) && defined(CONFIG_SUPPORT_WOWL)
void P_PM_WOWL_GPIO_IntrCallback(void *ctxt1, int ctxt2)
{
	HAPPY(ctxt1);
	HAPPY(ctxt2);
	sWakeupByWlan = TRUE;
}
#endif

static void P_PM_WOL_SetWOWL(void)
{
#if defined(CONFIG_WIFI) && defined(CONFIG_SUPPORT_WOWL)	

#if defined(CONFIG_WIFI_DHD)
	HCHAR acWowlCmd[64];
	HCHAR *sWlanIfName = "wlan0";

	/* Set Wake-up on WLAN*/
	VK_snprintf(acWowlCmd, sizeof(acWowlCmd), "dhd -i %s sbreg 0xc40 1", sWlanIfName);
	P_PM_SysCall(acWowlCmd);
	
	VK_snprintf(acWowlCmd, sizeof(acWowlCmd), "dhd -i %s sbreg 0xe00 0x11111111", sWlanIfName);
	P_PM_SysCall(acWowlCmd);

	P_PM_SysCall("wl PM 1");
	P_PM_SysCall("wl wowl 1");
	P_PM_SysCall("wl wowl_clear");
	P_PM_SysCall("wl wowl_activate 1");
#elif defined(CONFIG_QUANTENNA_BBIC_MAC)
	{
		HBOOL bPower = FALSE;

		DRV_GPIO_ChangeMode(GPIO_ID_WIFI_HOST_WAKE,GPIO_MODE_READ,GPIO_INVAILD);
		(void)DRV_WIFI_GetPowerStatus(0,&bPower);
		if(bPower == TRUE)
		{
			P_PM_SysCall("call_qcsapi_sockrpc --host 169.254.119.1 wowlan_host_state wifi0 1");
			P_PM_SysCall("call_qcsapi_sockrpc --host 169.254.119.1 wowlan_match_type wifi0 0");
			P_PM_SysCall("call_qcsapi_sockrpc --host 169.254.119.1 wowlan_udp_port wifi0 7");
			P_PM_SysCall("call_qcsapi_sockrpc --host 169.254.119.1 wowlan_pattern wifi0 FFFFFFFFFFFF");
		}
		VK_TASK_Sleep(10);
	}
#endif

	(void)DRV_GPIO_INTR_RegisterEventNotify(GPIO_ID_WIFI_HOST_WAKE, P_PM_WOWL_GPIO_IntrCallback, NULL, 0);
	(void)DRV_GPIO_INTR_Enable(GPIO_ID_WIFI_HOST_WAKE);
#endif
}

static void P_PM_WOL_ClearWOWL(void)
{
#if defined(CONFIG_WIFI) && defined(CONFIG_SUPPORT_WOWL)
	(void)DRV_GPIO_INTR_Disable(GPIO_ID_WIFI_HOST_WAKE);
#if defined(CONFIG_QUANTENNA_BBIC_MAC)
	DRV_GPIO_ChangeMode(GPIO_ID_WIFI_HOST_WAKE,GPIO_MODE_WRITE,GPIO_INVAILD);
#endif
	sWakeupByWlan = FALSE;
#endif
}

void DRV_PM_DemodPowerSet(HBOOL bOn)
{
	s_bDemodPower = bOn;
	return;
}

DRV_Error DRV_PM_StandbyStates1(int active)
{
	int rc;
	DRV_Error eDrvErr = DRV_ERR;
	NEXUS_PlatformStandbySettings 	nexusStandbySettings;

	/* get default standby-setting from nexus */
	NEXUS_Platform_GetStandbySettings(&nexusStandbySettings);
	if (active)
	{
		/* go to standby mode */
		nexusStandbySettings.mode = NEXUS_PlatformStandbyMode_eActive;
	}
	else
	{
		/* wake up */
		P_PM_GetWakeUpStatus();		
		nexusStandbySettings.mode = NEXUS_PlatformStandbyMode_eOn;		
	}

	rc = P_PM_SetPlatformSetting(&nexusStandbySettings);
	DONE_ERR(rc);

	eDrvErr = DRV_OK;
done:
	return eDrvErr;
}

DRV_Error DRV_PM_StandbyStates2(DRV_PM_STBY_Settings stbySettings)
{
	int rc;
	DRV_Error eDrvErr = DRV_ERR;
	NEXUS_PlatformStandbySettings nexusStandbySettings;

	/* get default standby-setting from nexus */
	NEXUS_Platform_GetStandbySettings(&nexusStandbySettings);

	/* set S2 standby mode */
	nexusStandbySettings.mode = NEXUS_PlatformStandbyMode_ePassive;
	nexusStandbySettings.wakeupSettings.ir = (bool)stbySettings.bIR;
#if defined(CONFIG_ETH_EXT_PHY) || defined(CONFIG_SUPPORT_WOWL)
	nexusStandbySettings.wakeupSettings.gpio = (bool)(stbySettings.bFrontKey || stbySettings.bWOL);
#else
	nexusStandbySettings.wakeupSettings.gpio = (bool)stbySettings.bFrontKey;
#endif
	nexusStandbySettings.wakeupSettings.timeout = (stbySettings.bTimer)?stbySettings.stInfo.stTimer.ulInterval:0;

#if defined(CONFIG_MICOM)
	nexusStandbySettings.wakeupSettings.ir = FALSE;
	nexusStandbySettings.wakeupSettings.gpio = FALSE;
	
	/* In case that one of ir and front-key is set,
	   Micom will receive that interrupts and then shake gpio to wake up host.
	   So, let's set wakeup source of host side to gpio. */
	if (stbySettings.bFrontKey || stbySettings.bIR
#if defined(CONFIG_ETH_EXT_PHY) || defined(CONFIG_SUPPORT_WOWL)
		|| stbySettings.bWOL
#endif		
		)
	{
		nexusStandbySettings.wakeupSettings.gpio = TRUE;
	}
#endif

	/* if S2 mode is supported, S2_ON gpio will be designed.
	   control the pio here. */
	(void)DRV_GPIO_Write(GPIO_ID_S2_ON, GPIO_LOW);

	rc = P_PM_SetPlatformSetting(&nexusStandbySettings);
	DONE_ERR(rc);
	
	if (stbySettings.bWOL)
	{
		/* Set Ethernet WOL */
		int wolopts;
		P_PM_WOL_GetWol(sEthIfName, &wolopts);
		wolopts &= (WAKE_MAGIC);
		P_PM_WOL_SetWol(sEthIfName, wolopts);

		/* Set WIFI WOL */
		P_PM_WOL_SetWOWL();
	}

	/* going to standby mode. bye~ */
	P_PM_SysCall("echo standby > /sys/power/state");

	(void)P_PM_GetWakeUpStatus();

	if (stbySettings.bWOL)
	{
		P_PM_WOL_ClearWOWL();
	}

#if defined(CONFIG_CAS_NA)
#if defined(CONFIG_MICOM)
	if (s_nWakeupSrc != DRV_MICOM_WAKEUP_BY_TIMER && s_nWakeupSrc != DRV_MICOM_WAKEUP_BY_WOL)
	{
		(void)DRV_MICOM_GetWakeupSrc(&s_nWakeupSrc);
	}
#endif
	P_PM_SaveWakeupReasonFromS2();
	P_PM_SysCall("reboot");
#endif

	/* now we wake up */
#if defined(CONFIG_DEBUG)
	PrintError("[%s] Wake up from S2 mode\n", __FUNCTION__);
	PrintError("[%s] Resuming Normal Mode\n", __FUNCTION__);
#endif

	/* if S2 mode is supported, S2_ON gpio will be designed.
	   control the pio here. */
	(void)DRV_GPIO_Write(GPIO_ID_S2_ON, GPIO_HI);	

#if (NEXUS_VERSION >= 1501)
	/* Here, we add ealry resume function of di, which can be run before nexus reinit. */

	/* for under nexus 15.1, gpio control doesn't work before nexus re-init.
	   so, call led early-resume function in case of over nexus 15.1.
	   led early-resume will be called in led resume in case of under nexus 15.1 */
	DRV_LED_EarlyResume();
#endif

	/* get default setting of standby from nexus */
	NEXUS_Platform_GetStandbySettings(&nexusStandbySettings);
	nexusStandbySettings.mode = NEXUS_PlatformStandbyMode_eOn;
	rc = P_PM_SetPlatformSetting(&nexusStandbySettings);
	DONE_ERR(rc);

	eDrvErr = DRV_OK;
done:
	return eDrvErr;
}

DRV_Error DRV_PM_StandbyStates3(DRV_PM_BOOTMODE_E bootmode, DRV_PM_STBY_Settings stbySettings)
{	
	int rc;
	DRV_Error eDrvErr = DRV_ERR;
	NEXUS_PlatformStandbySettings nexusStandbySettings;

	DONE_ERR((bootmode >= DRV_PM_BOOTMODE_MAX));

	eDrvErr = P_PM_DDRPower_KeepOn((bootmode!=DRV_PM_BOOTMODE_COLD));
	DONE_ERR((eDrvErr!=DRV_OK));
	
#if defined(CONFIG_EXT_DEMOD_POWER_CONTROL)
	PrintError("[%s] Setting LOOPTHROUGH %s \n", __FUNCTION__, s_bDemodPower? "On":"Off");
	P_PM_DemodPowerSet(s_bDemodPower);
#endif

	/* get default setting of standby from nexus and set S3 mode */
	NEXUS_Platform_GetStandbySettings(&nexusStandbySettings);
	nexusStandbySettings.mode = NEXUS_PlatformStandbyMode_eDeepSleep;
	nexusStandbySettings.wakeupSettings.ir = (bool)stbySettings.bIR;
#if defined(CONFIG_ETH_EXT_PHY) || defined(CONFIG_SUPPORT_WOWL)
	nexusStandbySettings.wakeupSettings.gpio = (bool)(stbySettings.bFrontKey || stbySettings.bWOL);
#else
	nexusStandbySettings.wakeupSettings.gpio = (bool)stbySettings.bFrontKey;
#endif
	nexusStandbySettings.wakeupSettings.timeout = (stbySettings.bTimer)?stbySettings.stInfo.stTimer.ulInterval:0;
	nexusStandbySettings.openFrontend = true;

#if defined(CONFIG_MICOM)
	nexusStandbySettings.wakeupSettings.ir = FALSE;
	nexusStandbySettings.wakeupSettings.gpio = FALSE;
	
	/* In case that one of ir and front-key is set,
	   Micom will receive that interrupts and then shake gpio to wake up host.
	   So, let's set wakeup source of host side to gpio. */
	if (stbySettings.bFrontKey || stbySettings.bIR
#if defined(CONFIG_ETH_EXT_PHY) || defined(CONFIG_SUPPORT_WOWL)
		|| stbySettings.bWOL
#endif		
		)
	{
		nexusStandbySettings.wakeupSettings.gpio = TRUE;
	}
#endif

	/* if S2 mode is supported, S2_ON gpio will be designed.
	   control the pio here. */
	(void)DRV_GPIO_Write(GPIO_ID_S2_ON, GPIO_LOW);


	rc = P_PM_SetPlatformSetting(&nexusStandbySettings);
	DONE_ERR(rc);

	/* going to standby mode. bye~ */
	switch(bootmode) {
	case DRV_PM_BOOTMODE_WARM:
		P_PM_SysCall("echo mem > /sys/power/state");
		break;
	case DRV_PM_BOOTMODE_COLD:
#if (LINUX_VERSION >= 3 && LINUX_PATCHLEVEL >= 8)		
		P_PM_SysCall("poweroff");
#else
		P_PM_SysCall("echo 1 > /sys/devices/platform/brcmstb/halt_mode");
		P_PM_SysCall("halt");
#endif
		PrintError("wait... until deep sleep\n");
		while(1)
		{
			PrintError("Maybe you can't see this message because the cpu already entered deep sleep.\n");
			VK_TASK_Sleep(100);
		}
		break;
	default:
		DONE_ERR(0);
	}

	/* now we wake up */
#if defined(CONFIG_DEBUG)
	PrintError("[%s] Wake up from S3 mode\n", __FUNCTION__);
	PrintError("[%s] Resuming Normal Mode\n", __FUNCTION__);
#endif

	/* if S2 mode is supported, S2_ON gpio will be designed.
	   control the pio here. */
	(void)DRV_GPIO_Write(GPIO_ID_S2_ON, GPIO_HI);

	P_PM_GetWakeUpStatus();

#if (NEXUS_VERSION >= 1501)
	/* Here, we add ealry resume function of di, which can be run before nexus reinit. */

	/* for under nexus 15.1, gpio control doesn't work before nexus re-init.
	   so, call led early-resume function in case of over nexus 15.1.
	   led early-resume will be called in led resume in case of under nexus 15.1 */
	DRV_LED_EarlyResume();
#endif

	/* get default setting of standby from nexus and set standby mode */
	NEXUS_Platform_GetStandbySettings(&nexusStandbySettings);
	nexusStandbySettings.mode = NEXUS_PlatformStandbyMode_eOn;
	rc = P_PM_SetPlatformSetting(&nexusStandbySettings);
	if (rc)
	{
		/* if we meet any error, reset here.
		   system will not be stable after error */
		PrintError("[%s,%d] Fatal Error! can't wake up nexus\n", __FUNCTION__, __LINE__);
		P_PM_SysCall("reboot");
	}

#if defined(CONFIG_EXT_DEMOD_POWER_CONTROL)
	PrintError("[%s] Setting LOOPTHROUGH ON \n", __FUNCTION__);
	P_PM_DemodPowerSet(TRUE);
#endif

	eDrvErr = DRV_OK;
done:
	return eDrvErr;
}

HBOOL DRV_PM_GetStandbyStatus(void)
{
	return bInSleep;
}

DRV_Error DRV_PM_GetWakeUpStatus(DRV_MICOM_WAKEUP_SRC *nWakeupSrc)
{	
	if (DRV_MICOM_WAKEUP_BY_MAX == s_nWakeupSrc) /* true when cold boot */
		P_PM_GetWakeUpStatus();

	*nWakeupSrc = s_nWakeupSrc;

	return DRV_OK;
}

DRV_Error DRV_PM_Init(void)
{	
	VK_SIG_SetSigAction(SIGUSR1, standby_task);
	VK_SIG_SetSigAction(SIGCONT, resume_task);

#if defined(CONFIG_CAS_NA)
	P_PM_GetWakeupReasonFromS2();
#endif

	P_PM_WOL_GetIfName();
	sWolWakeupCount = P_PM_WOL_GetWakeupCount();

#if defined(CONFIG_DI_SERVER_CLIENT)
	(void)DI_POWER_CLIENT_Init();
#endif

	return DRV_OK;
}
#endif


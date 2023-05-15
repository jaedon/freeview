/*******************************************************************************
* File name : di_network.c
* Author :
* description :
*
* Copyright (c)  by Humax Co., Ltd. Since 2012.
* All right reserved
*******************************************************************************/

/*******************************************************************************
* Header and definitions
*******************************************************************************/
/* global header files */
#if defined(CONFIG_VK_STDLIB)
#else
#include 	<stdio.h>
#include 	<stdlib.h>
#include 	<string.h>
#endif
#include <sys/socket.h>
#include <sys/ioctl.h>

#include <linux/ethtool.h>
#include <linux/sockios.h>
#include <linux/if.h>
#include <linux/route.h>
#include <linux/netlink.h>
#include <linux/rtnetlink.h>
#include <net/ethernet.h>
#include <arpa/inet.h>
#include <errno.h>

#include "htype.h"
#include "nexus_platform.h"
#include "nexus_pid_channel.h"
#include "nexus_platform.h"
#include "nexus_pid_channel.h"
#include "bstd_defs.h"
#include "breg_mem.h"
#if defined(CONFIG_WAKE_ON_LAN) && (NEXUS_VERSION < 1401)
#include "bchp_genet_0_umac.h"
#endif

#include "nvram_map.h"
#include "drv_err.h"
#include "di_err.h"
#include "drv_network.h"
#include "drv_micom.h"
#include "vkernel.h"
#include "di_nvram.h"
#include "di_uart.h"

/*******************************************************************************
* Debug level
*******************************************************************************/
#if defined(CONFIG_SHARED_COMPILE)

#ifdef DI_NETWORK_DEBUG
#define PrintDebug(fmt, ...)	VK_DBG_Print (fmt, ## __VA_ARGS__)
#define PrintError(fmt, ...)	VK_DBG_Print (fmt, ## __VA_ARGS__)
#else
#define PrintDebug(fmt, ...)
#define PrintError(fmt, ...)	VK_DBG_Print (fmt, ## __VA_ARGS__)
#endif
#ifdef DI_NETWORK_ENTER_LEAVE_DEBUG
#define PrintEnter()			VK_printf("(+)%s\n", __FUNCTION__)
#define PrintExit()				VK_printf("(-)%s\n", __FUNCTION__)
#else
#define PrintEnter()
#define PrintExit()
#endif

#else

#ifndef DEBUG_MODULE
#ifdef DI_NETWORK_DEBUG
#define PrintDebug(fmt, ...)	DI_UART_Print (fmt , ## __VA_ARGS__)
#define PrintError(fmt, ...)	DI_UART_Print (fmt , ## __VA_ARGS__)
#else
#define PrintDebug(fmt, ...)
#define PrintError(fmt, ...)	DI_UART_Print (fmt , ## __VA_ARGS__)
#endif

#ifdef DI_NETWORK_ENTER_LEAVE_DEBUG
#define PrintEnter()		DI_UART_Print("(+)%s\n", __FUNCTION__)
#define PrintExit()		DI_UART_Print("(-)%s\n", __FUNCTION__)
#else
#define PrintEnter()
#define PrintExit()
#endif
#endif /* #ifndef DEBUG_MODULE */

#endif

/*******************************************************************************
* Definitions, typdef & const data
*******************************************************************************/
#if defined(CONFIG_WAKE_ON_LAN)
#define	START_MDIO_TRANS			0x20000000
#define MDIO_READ					0x08000000
#define MDIO_WRITE					0x04000000
#define MDIO_PHY_ADDR(PhyAddress)	(PhyAddress << BCHP_GENET_0_UMAC_MDIO_CMD_phy_prt_addr_SHIFT)
#endif

#define UNUSED(a) (void)(a)

/*******************************************************************************
* Global variables and structures
*******************************************************************************/


/*******************************************************************************
* External variables and functions
*******************************************************************************/
#if defined(CONFIG_WAKE_ON_LAN)
extern const DRV_NETWORK_DEV_INFO_t s_stDevList[];
extern HUINT32 DRV_CFG_GetNetMaxDevice(void);
#endif
/*******************************************************************************
* Static variables and structures
*******************************************************************************/


/*******************************************************************************
* Functions
*******************************************************************************/
#if defined(CONFIG_WAKE_ON_LAN)
static HUINT32 P_NETWORK_AccessMdioRegister(HUINT8 ucPhyAddr, HUINT32 ulmdioOpCode, HUINT32 ulmdioRegisterAddress, HUINT32 ulmdioDataAddress)
{
	NEXUS_Error nRet = NEXUS_SUCCESS;
	HUINT32 ulREG_RTL8211E = 0;
	HUINT32 ulbitStartBusy, ulbitFail;
	HUINT32 ulVal = 0;
	HINT32 loopCount, maxLoopCount;

	ulREG_RTL8211E = START_MDIO_TRANS | ulmdioOpCode | MDIO_PHY_ADDR(ucPhyAddr) | (ulmdioRegisterAddress << 16) | ulmdioDataAddress;
	PrintDebug( "Reading [MDIO_PHY_ADDR) %x], [(mdioRegisterAddress): %x], [(mdioRegisterAddress * 2^16): %x], [mdioDataAddress : %x], \n", MDIO_PHY_ADDR, ulmdioRegisterAddress, (ulmdioRegisterAddress << 16), ulmdioDataAddress);

	NEXUS_Platform_WriteRegister(BCHP_GENET_0_UMAC_MDIO_CMD, ulREG_RTL8211E);

	loopCount = 0;
	maxLoopCount = 10;
	ulbitStartBusy = 0;
	ulREG_RTL8211E = 0;

	do
	{
		ulVal = 0;
		nRet = NEXUS_Platform_ReadRegister(BCHP_GENET_0_UMAC_MDIO_CMD, &ulVal);
		if(nRet != NEXUS_SUCCESS)
		{
			PrintError("[%s:%d] NEXUS_Platform_ReadRegister failed!\n", __FUNCTION__, __LINE__);
			return -1;
		}
		ulREG_RTL8211E = ulVal;
		ulbitStartBusy = ((ulVal >> 29)&0x1);
		ulbitFail = ((ulVal >> 28)&0x1);
		PrintDebug( "Reading [REG_RTL8211E: %x], [busy : %x], [fail :%x] \n", ulREG_RTL8211E, ulbitStartBusy, ulbitFail);

		VK_TASK_Sleep(1);

		loopCount++;
	}while(( ulbitStartBusy == 1) || (loopCount > maxLoopCount) );

	if ( loopCount > maxLoopCount )
	{
		PrintError("[%s:%d] P_NETWORK_AccessMdioRegister error!\n", __FUNCTION__, __LINE__);
		return -1;
	}
	else
	{
		return (ulREG_RTL8211E & 0xffff);
	}

}

static HUINT32 P_NETWORK_MII_Read(HUINT8 ucPhyAddr, HUINT32 uladdress)
{
	HUINT32 ulretval;
	HUINT32 mdioDummy = 0;

	ulretval = P_NETWORK_AccessMdioRegister(ucPhyAddr, MDIO_READ, uladdress, mdioDummy);

	return ulretval;
}

static DRV_Error P_NETWORK_MII_Write(HUINT8 ucPhyAddr, HUINT32 uladdress, HUINT32 uldata)
{
	HUINT32 ulretval;

	ulretval = P_NETWORK_AccessMdioRegister(ucPhyAddr, MDIO_WRITE, uladdress, uldata);
	if( ulretval > 0 )
	{
		return DRV_OK;
	}
	else
	{
		PrintError("[%s] P_NETWORK_MII_Write error!\n", __func__);
		return DRV_ERR;
	}
}

static DRV_Error P_NETWORK_GetPhyAddress(HUINT32 ulDevId, HUINT8 *ucPhyAddr)
{
	struct ifreq ifr;
	struct ethtool_cmd ecmd;
	int	s_sockfd;

	s_sockfd = socket(AF_INET, SOCK_DGRAM, 0);
	if (s_sockfd < 0)
	{
		PrintError("[%s:%d] socket Error : %s!\n", __FUNCTION__, __LINE__, strerror(errno));
		return DRV_ERR;
	}

	VK_MEM_Memset(&ifr, 0, sizeof(ifr));
	VK_strncpy(ifr.ifr_name, s_stDevList[ulDevId].szName, IF_NAME_SIZE-1);

	ecmd.cmd = ETHTOOL_GSET;
	ifr.ifr_data = (caddr_t)&ecmd;
	if(ioctl(s_sockfd, SIOCETHTOOL, (char *)&ifr) < 0)
	{
		PrintError("[%s:%d] ioctl(SIOCETHTOOL) Error : %s!\n", __FUNCTION__, __LINE__, strerror(errno));
		close(s_sockfd);
		return DRV_ERR;
	}
	PrintError("[%s:%d] phy_address = 0x%x \n", __FUNCTION__, __LINE__, ecmd.phy_address);
	*ucPhyAddr = ecmd.phy_address;

	close(s_sockfd);

	return DRV_OK;
}

static DRV_Error P_NETWORK_DisableWakeOnLan(HUINT32 ulDevId, HUINT8 ucPhyAddr)
{
	DRV_Error	nDrvErr = DRV_OK;

	UNUSED(ulDevId);

	/* Select page */
	nDrvErr = P_NETWORK_MII_Write(ucPhyAddr, 0x1f, 0x0007);
	if (nDrvErr != DRV_OK)
	{
		PrintError("[%s] %d Error(0x%x) \n",__FUNCTION__, __LINE__, nDrvErr);
		return	DRV_ERR;
	}

	/* Select extension page 109 */
	nDrvErr = P_NETWORK_MII_Write(ucPhyAddr, 0x1e, 0x006d);
	if (nDrvErr != DRV_OK)
	{
		PrintError("[%s] %d Error(0x%x) \n",__FUNCTION__, __LINE__, nDrvErr);
		return	DRV_ERR;
	}

	/* Disable all WOL events */
	nDrvErr = P_NETWORK_MII_Write(ucPhyAddr, 0x15, 0x0000);
	if (nDrvErr != DRV_OK)
	{
		PrintError("[%s] %d Error(0x%x) \n",__FUNCTION__, __LINE__, nDrvErr);
		return	DRV_ERR;
	}

	/* Reset WOL */
	nDrvErr = P_NETWORK_MII_Write(ucPhyAddr, 0x16, 0x8000);
	if (nDrvErr != DRV_OK)
	{
		PrintError("[%s] %d Error(0x%x) \n",__FUNCTION__, __LINE__, nDrvErr);
		return	DRV_ERR;
	}

	/* Enable RGMII pad*/
	nDrvErr = P_NETWORK_MII_Write(ucPhyAddr, 0x19, 0x0000);
	if (nDrvErr != DRV_OK)
	{
		PrintError("[%s] %d Error(0x%x) \n",__FUNCTION__, __LINE__, nDrvErr);
		return	DRV_ERR;
	}

	/* Back to page 0 */
	nDrvErr = P_NETWORK_MII_Write(ucPhyAddr, 0x1f, 0x0000);
	if (nDrvErr != DRV_OK)
	{
		PrintError("[%s] %d Error(0x%x) \n",__FUNCTION__, __LINE__, nDrvErr);
		return	DRV_ERR;
	}

	return	DRV_OK;
}

static DRV_Error P_NETWORK_EnableWakeOnLan(HUINT32 ulDevId, HUINT8 ucPhyAddr)
{
	DRV_Error	nDrvErr = DRV_OK;
	HUINT32 ulVal;
	HUINT32 i=0;
	HUINT8 aucMacAddr[MAC_ADDR_LEN];
	HUINT32 ulMiiMacAddr=0;

	ulVal = P_NETWORK_MII_Read(ucPhyAddr, 0x0);
	PrintDebug("[%s] DI_NETWORK_SetWakeOnLAN is (PhyAddress, 0x0) : %x \r\n",__func__, ulVal);

	ulVal = P_NETWORK_MII_Read(ucPhyAddr, 0x1);
	PrintDebug("[%s] DI_NETWORK_SetWakeOnLAN is (PhyAddress, 0x1) : %x \r\n",__func__, ulVal);

	ulVal = P_NETWORK_MII_Read(ucPhyAddr, 0x1);
	PrintDebug("[%s] DI_NETWORK_SetWakeOnLAN is (PhyAddress, 0x1) : %x \r\n",__func__, ulVal);

	ulVal = P_NETWORK_MII_Read(ucPhyAddr, 0x2);
	PrintDebug("[%s] DI_NETWORK_SetWakeOnLAN is (PhyAddress, 0x2) : %x \r\n",__func__, ulVal);

	ulVal = P_NETWORK_MII_Read(ucPhyAddr, 0x3);
	PrintDebug("[%s] DI_NETWORK_SetWakeOnLAN is (PhyAddress, 0x3) : %x \r\n",__func__, ulVal);

	PrintDebug("[%s] =========================================================\n", __func__);
	PrintDebug("[%s] ====================<Set WOL Start>======================\n", __func__);
	PrintDebug("[%s] =========================================================\n", __func__);
	PrintDebug("[%s] <1> Set Maximum packet and WOL Event\n", __func__);

	/* Reset and Maximum Packet byte register */
	nDrvErr = P_NETWORK_MII_Write(ucPhyAddr, 0x1f, 0x0007);
	if (nDrvErr != DRV_OK)
	{
		PrintError("Error(0x%x) In DI_NETWORK_SetWakeOnLAN() \n", nDrvErr);
		return	DRV_ERR;
	}
	nDrvErr = P_NETWORK_MII_Write(ucPhyAddr, 0x1e, 0x006d);
	if (nDrvErr != DRV_OK)
	{
		PrintError("Error(0x%x) In DI_NETWORK_SetWakeOnLAN() \n", nDrvErr);
		return	DRV_ERR;
	}
	nDrvErr = P_NETWORK_MII_Write(ucPhyAddr, 0x16, 0x9fff);
	if (nDrvErr != DRV_OK)
	{
		PrintError("Error(0x%x) In DI_NETWORK_SetWakeOnLAN() \n", nDrvErr);
		return	DRV_ERR;
	}

	/* Enable Magic Packet Event */
	nDrvErr = P_NETWORK_MII_Write(ucPhyAddr, 0x15, 0x1000);
	if (nDrvErr != DRV_OK)
	{
		PrintError("Error(0x%x) In DI_NETWORK_SetWakeOnLAN() \n", nDrvErr);
		return	DRV_ERR;
	}

	/* Set MAC Addres */
	PrintDebug("[%s] <2> Set MAC Address \n", __func__);

	nDrvErr = DRV_NETWORK_ReadMacAddrFromNvram(ulDevId, aucMacAddr);
	if(nDrvErr != DRV_OK)
	{
		PrintError("[%s] ReadMacAddrFromNvram Error\n", __FUNCTION__);
		return DRV_ERR;
	}

	for(i=0; i<MAC_ADDR_LEN; i++)
	{
		PrintError("MAC ADDR : 0x%X\n", aucMacAddr[i]);
	}

	nDrvErr = P_NETWORK_MII_Write(ucPhyAddr, 0x1e, 0x006e);
	if (nDrvErr != DRV_OK)
	{
		PrintError("Error(0x%x) In DI_NETWORK_SetWakeOnLAN() \n", nDrvErr);
		return	DRV_ERR;
	}
	ulMiiMacAddr = aucMacAddr[0] | (aucMacAddr[1] << 8);
	PrintError("ulMiiMacAddr = 0x%x\n", ulMiiMacAddr);
	nDrvErr = P_NETWORK_MII_Write(ucPhyAddr, 0x15, ulMiiMacAddr);
	if (nDrvErr != DRV_OK)
	{
		PrintError("Error(0x%x) In DI_NETWORK_SetWakeOnLAN() \n", nDrvErr);
		return	DRV_ERR;
	}
	ulMiiMacAddr = 0;
	ulMiiMacAddr = aucMacAddr[2] | (aucMacAddr[3] << 8);
	PrintError("ulMiiMacAddr = 0x%x\n", ulMiiMacAddr);
	nDrvErr = P_NETWORK_MII_Write(ucPhyAddr, 0x16, ulMiiMacAddr);
	if (nDrvErr != DRV_OK)
	{
		PrintError("Error(0x%x) In DI_NETWORK_SetWakeOnLAN() \n", nDrvErr);
		return	DRV_ERR;
	}
	ulMiiMacAddr = 0;
	ulMiiMacAddr = aucMacAddr[4] | (aucMacAddr[5] << 8);
	PrintError("ulMiiMacAddr = 0x%x\n", ulMiiMacAddr);
	nDrvErr = P_NETWORK_MII_Write(ucPhyAddr, 0x17, ulMiiMacAddr);
	if (nDrvErr != DRV_OK)
	{
		PrintError("Error(0x%x) In DI_NETWORK_SetWakeOnLAN() \n", nDrvErr);
		return	DRV_ERR;
	}

	/* Set Wake-up frame mask */
	PrintDebug("[%s] <3> Set Wake-up frame mask; wake-up frame mask=0x0000_03C0_0020_3000\n", __func__);

	nDrvErr = P_NETWORK_MII_Write(ucPhyAddr, 0x1e, 0x0064);
	if (nDrvErr != DRV_OK)
	{
		PrintError("Error(0x%x) In DI_NETWORK_SetWakeOnLAN() \n", nDrvErr);
		return	DRV_ERR;
	}
	nDrvErr = P_NETWORK_MII_Write(ucPhyAddr, 0x15, 0x3000);
	if (nDrvErr != DRV_OK)
	{
		PrintError("Error(0x%x) In DI_NETWORK_SetWakeOnLAN() \n", nDrvErr);
		return	DRV_ERR;
	}
	nDrvErr = P_NETWORK_MII_Write(ucPhyAddr, 0x16, 0x0020);
	if (nDrvErr != DRV_OK)
	{
		PrintError("Error(0x%x) In DI_NETWORK_SetWakeOnLAN() \n", nDrvErr);
		return	DRV_ERR;
	}
	nDrvErr = P_NETWORK_MII_Write(ucPhyAddr, 0x17, 0x03c0);
	if (nDrvErr != DRV_OK)
	{
		PrintError("Error(0x%x) In DI_NETWORK_SetWakeOnLAN() \n", nDrvErr);
		return	DRV_ERR;
	}
	nDrvErr = P_NETWORK_MII_Write(ucPhyAddr, 0x18, 0x0000);
	if (nDrvErr != DRV_OK)
	{
		PrintError("Error(0x%x) In DI_NETWORK_SetWakeOnLAN() \n", nDrvErr);
		return	DRV_ERR;
	}
	nDrvErr = P_NETWORK_MII_Write(ucPhyAddr, 0x19, 0x0000);
	if (nDrvErr != DRV_OK)
	{
		PrintError("Error(0x%x) In DI_NETWORK_SetWakeOnLAN() \n", nDrvErr);
		return	DRV_ERR;
	}
	nDrvErr = P_NETWORK_MII_Write(ucPhyAddr, 0x1a, 0x0000);
	if (nDrvErr != DRV_OK)
	{
		PrintError("Error(0x%x) In DI_NETWORK_SetWakeOnLAN() \n", nDrvErr);
		return	DRV_ERR;
	}
	nDrvErr = P_NETWORK_MII_Write(ucPhyAddr, 0x1b, 0x0000);
	if (nDrvErr != DRV_OK)
	{
		PrintError("Error(0x%x) In DI_NETWORK_SetWakeOnLAN() \n", nDrvErr);
		return	DRV_ERR;
	}
	nDrvErr = P_NETWORK_MII_Write(ucPhyAddr, 0x1c, 0x0000);
	if (nDrvErr != DRV_OK)
	{
		PrintError("Error(0x%x) In DI_NETWORK_SetWakeOnLAN() \n", nDrvErr);
		return	DRV_ERR;
	}

	/*  Set Wake-up frame CRC */
	PrintDebug("[%s] <4> Set Wake-up frame CRC\n", __func__);

	nDrvErr = P_NETWORK_MII_Write(ucPhyAddr, 0x1e, 0x006c);
	if (nDrvErr != DRV_OK)
	{
		PrintError("Error(0x%x) In DI_NETWORK_SetWakeOnLAN() \n", nDrvErr);
		return	DRV_ERR;
	}
	nDrvErr = P_NETWORK_MII_Write(ucPhyAddr, 0x15, 0xdf6b);
	if (nDrvErr != DRV_OK)
	{
		PrintError("Error(0x%x) In DI_NETWORK_SetWakeOnLAN() \n", nDrvErr);
		return	DRV_ERR;
	}

	/* Disable RGMII */
	PrintDebug("[%s] <5> Disable GMII / RGMII pad for power saving\n", __func__);

	nDrvErr = P_NETWORK_MII_Write(ucPhyAddr, 0x1e, 0x006d);
	if (nDrvErr != DRV_OK)
	{
		PrintError("Error(0x%x) In DI_NETWORK_SetWakeOnLAN() \n", nDrvErr);
		return	DRV_ERR;
	}
	nDrvErr = P_NETWORK_MII_Write(ucPhyAddr, 0x19, 0x0001);
	if (nDrvErr != DRV_OK)
	{
		PrintError("Error(0x%x) In DI_NETWORK_SetWakeOnLAN() \n", nDrvErr);
		return	DRV_ERR;
	}
	nDrvErr = P_NETWORK_MII_Write(ucPhyAddr, 0x1f, 0x0000);
	if (nDrvErr != DRV_OK)
	{
		PrintError("Error(0x%x) In DI_NETWORK_SetWakeOnLAN() \n", nDrvErr);
		return	DRV_ERR;
	}

	PrintError("[%s] =========================================================\n", __func__);
	PrintError("[%s] ====================<set WOL END>========================\n", __func__);
	PrintError("[%s] =========================================================\n", __func__);

	return DRV_OK;
}
#endif

/*******************************************************************************
* function : DRV_NETWORK_ReadMacAddrFromNvram
* description :
* input :
* output :
* return :
* 기타 참고자료 및 파일
*******************************************************************************/
DRV_Error DRV_NETWORK_ReadMacAddrFromNvram(HUINT32 ulDevId, HUINT8 *pucMacAddr)
{
	DI_ERR_CODE eDiErr = DI_ERR_OK;

	switch(ulDevId)
	{
		case 0:
			eDiErr = DI_NVRAM_GetField(DI_NVRAM_FIELD_MAC_ADDR, 0, pucMacAddr, MAC_ADDR_LEN);
			break;
		case 1:
			eDiErr = DI_NVRAM_Read(DI_NVRAM_FS_PARTITION_RO, (HUINT8 *)MAC2_ADDR, 0, pucMacAddr, MAC_ADDR_LEN);
			break;
		case 2:
			eDiErr = DI_NVRAM_Read(DI_NVRAM_FS_PARTITION_RO, (HUINT8 *)MAC3_ADDR, 0, pucMacAddr, MAC_ADDR_LEN);
			break;
		default:
			eDiErr = DI_ERR_ERROR;
			break;
	}
	if(eDiErr != DI_ERR_OK)
	{
		PrintError("[%s:%d] Device[%d] Read NVRAM Error!\n", __FUNCTION__, __LINE__,ulDevId);
		return DRV_ERR;
	}
	return DRV_OK;
}

/*******************************************************************************
* function : DRV_NETWORK_WriteMacAddrToNvram
* description :
* input :
* output :
* return :
* 기타 참고자료 및 파일
*******************************************************************************/
DRV_Error DRV_NETWORK_WriteMacAddrToNvram(HUINT32 ulDevId, HUINT8 *pucMacAddr)
{
	DI_ERR_CODE eDiErr = DI_ERR_OK;

	switch(ulDevId)
	{
		case 0:
			eDiErr = DI_NVRAM_SetField(DI_NVRAM_FIELD_MAC_ADDR, 0, pucMacAddr, MAC_ADDR_LEN);
			break;
		case 1:
			eDiErr = DI_NVRAM_Write(DI_NVRAM_FS_PARTITION_RO, (HUINT8 *)MAC2_ADDR, 0, pucMacAddr, MAC_ADDR_LEN);
			break;
		case 2:
			eDiErr = DI_NVRAM_Write(DI_NVRAM_FS_PARTITION_RO, (HUINT8 *)MAC3_ADDR, 0, pucMacAddr, MAC_ADDR_LEN);
			break;
		default:
			eDiErr = DI_ERR_ERROR;
			break;
	}
	if(eDiErr != DI_ERR_OK)
	{
		PrintError("[%s:%d] Device[%d] Write NVRAM Error!\n", __FUNCTION__, __LINE__, ulDevId);
		return DRV_ERR;
	}
	return DRV_OK;
}

/*******************************************************************************
* function : DI_NETWORK_SetWakeOnLAN
* description : External PHY support (WOL)
* input :
* output :
* return :
* 기타 참고자료 및 파일
*******************************************************************************/
DI_ERR_CODE DI_NETWORK_SetWakeOnLAN (HUINT32 ulDevId, HBOOL bOn_Off)
{
	DI_ERR_CODE eDiErr = DI_ERR_OK;
	HAPPY(ulDevId);
	HAPPY(bOn_Off);

#if defined(CONFIG_WAKE_ON_LAN)
	DRV_Error	nDrvErr = DRV_OK;
	HUINT8 ucPhyAddr;

	PrintEnter();

	if(ulDevId >= DRV_CFG_GetNetMaxDevice())
	{
		PrintError("[%s:%d] Invalid Device ID!\n", __FUNCTION__, __LINE__);
		return DI_ERR_ERROR;
	}

	nDrvErr = DRV_MICOM_SetWakeOnLAN(bOn_Off);
	if (nDrvErr != DRV_OK)
	{
		PrintError("Error(0x%x) In DI_NETWORK_SetWakeOnLAN() \n", nDrvErr);
		eDiErr = DI_ERR_ERROR;
		goto END;
	}

	nDrvErr = P_NETWORK_GetPhyAddress(ulDevId, &ucPhyAddr);
	if (nDrvErr != DRV_OK)
	{
		PrintError("Error(0x%x) In P_NETWORK_GetPhyAddress() \n", nDrvErr);
		eDiErr = DI_ERR_ERROR;
		goto END;
	}

	if(bOn_Off == TRUE)
	{
		nDrvErr = P_NETWORK_EnableWakeOnLan(ulDevId, ucPhyAddr);
		if (nDrvErr != DRV_OK)
		{
			PrintError("Error(0x%x) In P_NETWORK_EnableWakeOnLan() \n", nDrvErr);
			eDiErr = DI_ERR_ERROR;
			goto END;
		}
	}
	else
	{
		nDrvErr = P_NETWORK_DisableWakeOnLan(ulDevId, ucPhyAddr);
		if (nDrvErr != DRV_OK)
		{
			PrintError("Error(0x%x) In P_NETWORK_DisableWakeOnLan() \n", nDrvErr);
			eDiErr = DI_ERR_ERROR;
			goto END;
		}
	}

	PrintDebug("[%s] DI_NETWORK_SetWakeOnLAN is %s \r\n", __func__, bOn_Off?"ON":"OFF");

END:
#endif
	PrintExit();
	return eDiErr;
}

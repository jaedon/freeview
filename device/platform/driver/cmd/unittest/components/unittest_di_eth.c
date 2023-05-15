/**
 * unittest_di_eth.c
*/

/**
 * @author		Kevin Woo
*/

/*******************************************************************/
/**************************** Header Files *************************/
/*******************************************************************/
#include "linden_trace.h"
#include "vkernel.h"
#include "unittest.h"

#include "di_err.h"

/* linux header files */
#if defined(CONFIG_VK_STDLIB)
#else
#include 	<stdio.h>
#include 	<stdlib.h>
#include 	<string.h>
#endif
#include <unistd.h>
#include <sys/socket.h>
#include <sys/ioctl.h>

#include <linux/sockios.h>
#include <linux/ethtool.h>
#include <linux/if.h>
#include <linux/route.h>
#include <linux/netlink.h>
#include <linux/rtnetlink.h>
#include <net/ethernet.h>
#include <arpa/inet.h>
#include <errno.h>

/*******************************************************************/
/****************************** define *****************************/
/*******************************************************************/

#define DEBUG_MODULE        TRACE_MODULE_DI_FAN	//temp

#define IF_NAME_SIZE		16
#define MAC_ADDR_LEN		6
#define IPV4_ADDR_LEN		4



/*******************************************************************/
/****************************** typedef ****************************/
/*******************************************************************/
typedef enum
{
	DI_ETH_DEV_LAN = 0,
	DI_ETH_DEV_WAN,
	DI_ETH_DEV_CABLEMODEM,
	DI_ETH_DEV_WIFI,
	DI_ETH_DEV_PPPOE,
	DI_ETH_DEV_MAX
} UT_ETH_DEV_TYPE_e;

typedef struct
{
	HUINT8 aucIpAddr[IPV4_ADDR_LEN];
	HUINT8 aucBcastAddr[IPV4_ADDR_LEN];
	HUINT8 aucMaskAddr[IPV4_ADDR_LEN];
} UI_ETH_IPV4_ADDR_t;

typedef struct
{
	unsigned int ulDevId;
	signed char szName[IF_NAME_SIZE];
	UT_ETH_DEV_TYPE_e eDevType;
} UT_ETH_DEV_INFO_t;

typedef struct
{
	UT_ETH_DEV_INFO_t *pstDevInfo;
	int bConnected;
	int s_sockfd;
} UT_ETH_INSTANCE_t;

typedef struct
{
	HUINT8 szName[IF_NAME_SIZE];
	UT_ETH_DEV_TYPE_e eDevType;
	HUINT8 aucMacAddr[MAC_ADDR_LEN];
	HBOOL bIsUp;
	HBOOL bIsConnected;
	HBOOL bIsDynamic;
	UI_ETH_IPV4_ADDR_t stIpv4Addr;
} UT_ETH_GET_INFO_t;

/*******************************************************************/
/************************ global variables *************************/
/*******************************************************************/
UT_ETH_DEV_INFO_t s_stEthDevList[] =
{
	{0, "eth0", DI_ETH_DEV_LAN},
};

/*******************************************************************/
/************************ static variables *************************/
/*******************************************************************/
static unsigned int s_ulEthDevNum;
static UT_ETH_INSTANCE_t *s_stEthInst;

/*******************************************************************/
/*********************** Function Prototypes ***********************/
/*******************************************************************/

static UT_RESULT UT_ETH_SelectTestCase(HUINT32 ulTestNumber);
static void UT_ETH_Menu(void);

static HUINT32 ut_GetNetMaxDevice(void)
{
	return (sizeof(s_stEthDevList)/sizeof(UT_ETH_DEV_INFO_t));
}

static UT_RESULT ut_GetDevLinkStatus(HUINT32 ulDevId, HBOOL *pbLinkState)
{
	UT_RESULT 			ret = UT_FAIL;

	struct ifreq ifreq;
	struct ethtool_value ethdata;

	VK_MEM_Memset(&ifreq, 0, sizeof(struct ifreq));
	/* get link status */
 	VK_strncpy(ifreq.ifr_name, s_stEthInst[ulDevId].pstDevInfo->szName, IF_NAME_SIZE-1);
	ethdata.cmd = ETHTOOL_GLINK;
	ifreq.ifr_data = (caddr_t)&ethdata;
	if(ioctl(s_stEthInst[ulDevId].s_sockfd, SIOCETHTOOL, (char *)&ifreq) < 0)
	{
		//PrintError("[%s:%d] ioctl(SIOCETHTOOL) Error : %s!\n", __FUNCTION__, __LINE__, strerror(errno));
		goto done;
	}

	if(ethdata.data)
	{
		*pbLinkState = TRUE;
	}
	else
	{
		*pbLinkState = FALSE;
	}

	VK_printf("[%s:%d] Link Status : %d : %s!\n", __FUNCTION__, __LINE__, *pbLinkState, strerror(errno));

	ret = UT_OK;
	done:
		return ret;

}

static UT_RESULT ut_GetDevStatus(HUINT32 ulDevId, HBOOL *pbIsUp, HBOOL *pbIsDynamic)
{
	UT_RESULT 			ret = UT_FAIL;

	struct ifreq ifreq;

	VK_MEM_Memset(&ifreq, 0, sizeof(struct ifreq));

	/* get status flags */
	VK_strncpy(ifreq.ifr_name, s_stEthInst[ulDevId].pstDevInfo->szName, IF_NAME_SIZE-1);
	if(ioctl(s_stEthInst[ulDevId].s_sockfd, SIOCGIFFLAGS, (char *)&ifreq) < 0)
	{
		VK_printf("[%s:%d] ioctl(SIOCGIFFLAGS) Error : %s!\n", __FUNCTION__, __LINE__, strerror(errno));
		goto done;
	}

	if(ifreq.ifr_flags&IFF_UP)
		*pbIsUp = TRUE;
	else
		*pbIsUp = FALSE;

	if(ifreq.ifr_flags&IFF_DYNAMIC)
		*pbIsDynamic = TRUE;
	else
		*pbIsDynamic = FALSE;

	ret = UT_OK;
	done:
		return ret;
}

static UT_RESULT ut_GetMacAddr(unsigned int ulDevId, unsigned char *pucMacAddr)
{
	UT_RESULT 			ret = UT_FAIL;

	struct ifreq ifreq;

	VK_MEM_Memset(&ifreq, 0, sizeof(struct ifreq));

	/* get mac addr */
	VK_strncpy(ifreq.ifr_name, s_stEthInst[ulDevId].pstDevInfo->szName, IF_NAME_SIZE-1);
	VK_MEM_Memset(pucMacAddr, 0, MAC_ADDR_LEN);
	if(ioctl(s_stEthInst[ulDevId].s_sockfd, SIOCGIFHWADDR, &ifreq) < 0)
	{
		VK_printf("[%s:%d] ioctl(SIOCGIFHWADDR) Error : %s!\n", __FUNCTION__, __LINE__, strerror(errno));
		goto done;
	}
	VK_memcpy(pucMacAddr, &ifreq.ifr_hwaddr.sa_data, sizeof(struct ether_addr));

	ret = UT_OK;
	done:
		return ret;
}

static UT_RESULT ut_GetIpAddr(HUINT32 ulDevId, HUINT8 *pucIpAddr, HUINT8 *pucBcastAddr, HUINT8 *pucMaskAddr)
{
	UT_RESULT 			ret = UT_FAIL;

	struct ifreq ifreq;
	struct sockaddr_in *sin;

	VK_MEM_Memset(&ifreq, 0, sizeof(struct ifreq));

	/* get ip addr */
	VK_strncpy(ifreq.ifr_name, s_stEthInst[ulDevId].pstDevInfo->szName, IF_NAME_SIZE-1);
	ifreq.ifr_addr.sa_family = AF_INET;
	VK_MEM_Memset(pucIpAddr, 0, IPV4_ADDR_LEN);
	if(ioctl(s_stEthInst[ulDevId].s_sockfd, SIOCGIFADDR, (char *)&ifreq)==0)
	{
		sin = (struct sockaddr_in *)&ifreq.ifr_addr;
		VK_memcpy(pucIpAddr, &sin->sin_addr, sizeof(struct in_addr));

		/* get broadcast addr */
		VK_strncpy(ifreq.ifr_name, s_stEthInst[ulDevId].pstDevInfo->szName, IF_NAME_SIZE-1);
		ifreq.ifr_addr.sa_family = AF_INET;
		VK_MEM_Memset(pucBcastAddr, 0, IPV4_ADDR_LEN);
		if(ioctl(s_stEthInst[ulDevId].s_sockfd, SIOCGIFBRDADDR, (char *)&ifreq) < 0)
		{
			VK_printf("[%s:%d] ioctl(SIOCGIFBRDADDR) Error : %s!\n", __FUNCTION__, __LINE__, strerror(errno));
			goto done;
		}
		sin = (struct sockaddr_in *)&ifreq.ifr_addr;
		VK_memcpy(pucBcastAddr, &sin->sin_addr, sizeof(struct in_addr));

		/* get netmask addr */
		VK_strncpy(ifreq.ifr_name, s_stEthInst[ulDevId].pstDevInfo->szName, IF_NAME_SIZE-1);
		ifreq.ifr_addr.sa_family = AF_INET;
		VK_MEM_Memset(pucMaskAddr, 0, IPV4_ADDR_LEN);
		if(ioctl(s_stEthInst[ulDevId].s_sockfd, SIOCGIFNETMASK, (char *)&ifreq) < 0)
		{
			VK_printf("[%s:%d] ioctl(SIOCGIFNETMASK) Error : %s!\n", __FUNCTION__, __LINE__, strerror(errno));
			goto done;
		}
		sin = (struct sockaddr_in *)&ifreq.ifr_addr;
		VK_memcpy(pucMaskAddr, &sin->sin_addr, sizeof(struct in_addr));
	}

	ret = UT_OK;
	done:
		return ret;
}

static UT_RESULT ut_GetDevInfo(HUINT32 ulDevId, UT_ETH_GET_INFO_t *pstGetInfo)
{
	UT_RESULT 	  ret = UT_FAIL;

	if(ulDevId >= s_ulEthDevNum)
	{
		VK_printf("[%s:%d] Invalid Device ID!\n", __FUNCTION__, __LINE__);
		return UT_FAIL;
	}
	/* copy dev name and type */
	VK_MEM_Memset(pstGetInfo, 0, sizeof(UT_ETH_GET_INFO_t));

	VK_strncpy(pstGetInfo->szName, s_stEthInst[ulDevId].pstDevInfo->szName, IF_NAME_SIZE-1);

	pstGetInfo->eDevType = s_stEthInst[ulDevId].pstDevInfo->eDevType;

	/* get link status */
	ret = ut_GetDevLinkStatus(ulDevId, &pstGetInfo->bIsConnected);
	DONE_ERR(ret != UT_OK);

	/* get dev status */
	ret = ut_GetDevStatus(ulDevId, &pstGetInfo->bIsUp, &pstGetInfo->bIsDynamic);
	DONE_ERR(ret != UT_OK);

	/* get mac addr */
	ret = ut_GetMacAddr(ulDevId, pstGetInfo->aucMacAddr);
	DONE_ERR(ret != UT_OK);

	/* get ip addr for v4 */
	ret = ut_GetIpAddr(ulDevId, pstGetInfo->stIpv4Addr.aucIpAddr, pstGetInfo->stIpv4Addr.aucBcastAddr, pstGetInfo->stIpv4Addr.aucMaskAddr);
	DONE_ERR(ret != UT_OK);

	ret = UT_OK;
	done:
		return ret;
}

static UT_RESULT ut_EthInit(void)
{
	UT_RESULT 			ret = UT_FAIL;
	unsigned int i;

	s_ulEthDevNum = ut_GetNetMaxDevice();

	/* malloc for network instance */
	s_stEthInst = (UT_ETH_INSTANCE_t *)VK_MEM_Alloc(sizeof(UT_ETH_INSTANCE_t)*s_ulEthDevNum);
	VK_MEM_Memset(s_stEthInst, 0, sizeof(UT_ETH_INSTANCE_t)*s_ulEthDevNum);

	/* initialize the network instance */
	for(i=0; i<s_ulEthDevNum; i++)
	{
		/* open socket interface */
		s_stEthInst[i].s_sockfd = socket(AF_INET, SOCK_DGRAM, 0);
		if(s_stEthInst[i].s_sockfd < 0)
		{
			VK_printf("[%s:%d] socket Error : %s!\n", __FUNCTION__, __LINE__, strerror(errno));
			return UT_FAIL;
		}

		s_stEthInst[i].pstDevInfo = &s_stEthDevList[i];
		if(s_stEthInst[i].pstDevInfo->eDevType != DI_ETH_DEV_WIFI
			&& s_stEthInst[i].pstDevInfo->eDevType != DI_ETH_DEV_PPPOE)
		{
			/* get link status */
			ret = ut_GetDevLinkStatus(i, &s_stEthInst[i].bConnected);
			DONE_ERR(ret != UT_OK);
		}

	}

	ret = UT_OK;
	done:
		return ret;
}

static void ut_EthUnInit(void)
{
	unsigned int i;

	/* Uninitialize the network instance */
	for(i=0; i<s_ulEthDevNum; i++)
	{
		/* close socket interface */
		close(s_stEthInst[i].s_sockfd);
	}
	s_ulEthDevNum = 0;
	VK_MEM_Free(s_stEthInst);

}

static UT_RESULT ut_EthInfo(void)
{
	UT_RESULT 			ret = UT_FAIL;
	HUINT32 i;

	UT_ETH_GET_INFO_t stGetInfo;

	ret = ut_EthInit();
	DONE_ERR(ret != UT_OK);


	for(i=0; i<s_ulEthDevNum; i++)
	{
		VK_memset(&stGetInfo, 0, sizeof(UT_ETH_GET_INFO_t));
		ret = ut_GetDevInfo(i, &stGetInfo);
		DONE_ERR(ret != UT_OK);

		VK_printf("============================================\n");
		VK_printf("eth[%d] name	  : %s\n", i, stGetInfo.szName);
		VK_printf("eth[%d] dev type  : %d\n", i, stGetInfo.eDevType);
		VK_printf("eth[%d] mac addr  : %02X.%02X.%02X.%02X.%02X.%02X\n", i, stGetInfo.aucMacAddr[0], stGetInfo.aucMacAddr[1], stGetInfo.aucMacAddr[2], stGetInfo.aucMacAddr[3], stGetInfo.aucMacAddr[4], stGetInfo.aucMacAddr[5]);
		VK_printf("eth[%d] up		  : %d\n", i, stGetInfo.bIsUp);
		if(stGetInfo.bIsUp)
		{
			VK_printf("eth[%d] connected : %d\n", i, stGetInfo.bIsConnected);
			VK_printf("eth[%d] dynamic   : %d\n", i, stGetInfo.bIsDynamic);
			VK_printf("eth[%d] ip addr   : %u.%u.%u.%u\n", i, stGetInfo.stIpv4Addr.aucIpAddr[0], stGetInfo.stIpv4Addr.aucIpAddr[1], stGetInfo.stIpv4Addr.aucIpAddr[2], stGetInfo.stIpv4Addr.aucIpAddr[3]);
			VK_printf("eth[%d] brd addr  : %u.%u.%u.%u\n", i, stGetInfo.stIpv4Addr.aucBcastAddr[0], stGetInfo.stIpv4Addr.aucBcastAddr[1], stGetInfo.stIpv4Addr.aucBcastAddr[2], stGetInfo.stIpv4Addr.aucBcastAddr[3]);
			VK_printf("eth[%d] net addr  : %u.%u.%u.%u\n", i, stGetInfo.stIpv4Addr.aucMaskAddr[0], stGetInfo.stIpv4Addr.aucMaskAddr[1], stGetInfo.stIpv4Addr.aucMaskAddr[2], stGetInfo.stIpv4Addr.aucMaskAddr[3]);
		}
	}

	ut_EthUnInit();

	ret = UT_OK;
	done:
		return ret;
}

void UT_DI_ETH(void)
{
	DI_ERR_CODE ulRet = 0;
	HUINT32 ulTestNumber = 0;

    do {
        UT_ETH_Menu(); /* print menu */

        ulRet = UT_GetNumber(&ulTestNumber);
		if (ulRet == DI_ERR_ERROR || ulTestNumber == 0)
		{
			/* */
			break;
		}
		else if (ulRet == DI_ERR_OK)
		{
            UT_ETH_SelectTestCase(ulTestNumber);
        }
        else {
            VK_printf( "Unknown key Code(%d)!!\n", ulTestNumber);
        }
    } while (1);

}


static void UT_ETH_Menu(void)
{

	VK_printf( "*============================================================*\n"); /* 70 */
	VK_printf( "*    DD TEST TOOL/CHANNEL                                    *\n"); /* 70 */
	VK_printf( "*------------------------------------------------------------*\n"); /* 70 */
	VK_printf( "*    00. Quit                                                *\n"); /* 70 */
	VK_printf( "*    01. DI_Eth_info									  *\n"); /* 70 */
	VK_printf( "*------------------------------------------------------------*\n"); /* 70 */
	VK_printf( "*    Select Number & Press OK                                *\n"); /* 70 */
	VK_printf( "*============================================================*\n"); /* 70 */
}

static UT_RESULT UT_ETH_SelectTestCase(HUINT32 ulTestNumber)
{

	UT_RESULT ulRet = 0;

    switch( ulTestNumber ) {

	case 0 :
		break;

	case 1 :
        break;

	default :
        break;
    }

	return ulRet;
}

static UT_MENU_ITEM_T	menu_eth[] =
{
	MENU_START,
	MENU_FUNC(ut_EthInfo, 5),
	MENU_END,
};

static UT_MENU_ITEM_T	menu_main_eth[] =
{
	MENU_SUB(menu_eth),
};

void UT_ETH_Init(void)
{
	HINT32 i;
	HINT32	array_size;

	array_size = sizeof(menu_main_eth)/sizeof(menu_main_eth[0]);
	for (i=0; i<array_size; i++)
	{
		UT_MENU_AddMainMenu(&menu_main_eth[i]);
	}
	array_size = sizeof(menu_eth)/sizeof(menu_eth[0]);
	for (i=0; i<array_size; i++)
	{
		UT_MENU_AddMenu(&menu_eth[i]);
	}
}


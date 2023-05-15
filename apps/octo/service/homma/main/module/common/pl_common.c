/*************************************************************************************************************
	File            : pl_common.h
	author          : IPM tipmsw1@humaxdigital.com
	comment         :
	date            :
	attention       :

	(c) 2011-2012 Humax Co., Ltd.
	This program is produced by Humax Co., Ltd. (Humax) and
	the proprietary Software of Humax and its licensors. Humax provides you, as an Authorized Licensee,
	non-assignable, non-transferable and non-exclusive license to use this Software.
	You acknowledge that this Software contains valuable trade secrets of Humax and by using this Software
	you agree to the responsibility to take all reasonable efforts to protect the any information
	you receive from Humax. You are not permitted to duplicate, modify, distribute, sell or lease and
	reverse engineer or extract the source code of this Software unless you have Humax's written permission
	to do so.
	If you have no authorized license, discontinue using this Software immediately.

	THE SOFTWARE IS PROVIDED ¡°AS IS¡± AND HUMAX MAKES NO PROMISES, REPRESENTATIONS OR WARRANTIES,
	EITHER EXPRESS, IMPLIED OR STATUTORY, OR OTHERWISE, WITH RESPECT TO THE SOFTWARE.
	IN NO EVENT SHALL HUMAX BE LIABLE FOR LOST PROFITS, REVENUES, OR DATA, FINANCIAL LOSSES OR
	INDIRECT, SPECIAL, CONSEQUENTIAL, EXEMPLARTY OR PUNITIVE DAMAGES WHATSOEVER RELATING TO YOUR USE OR
	INABILITY TO USE THE SOFTWARE.

	This License is effective until terminated. You may terminate this License at any time
	by destroying all copies of the Software including all documentation. This License will terminate
	immediately without notice from Humax to you if you fail to comply with any provision of this License.
	Upon termination, you must destroy all copies of the Software and all documentation.

	The laws of the Republic of Korea will apply to any disputes arising out of or relating to
	this Copyright Notice. All claims arising out of or relating to this Copyright Notice will be litigated
	in the Seoul Central District Court, in the Republic of Korea.
*************************************************************************************************************/

#define	________________Header_Files_______________________________
#include <arpa/inet.h>
#include <net/if.h>
#include <sys/ioctl.h>

#include <vkernel.h>

#include <hlib.h>
#include <dlib.h>

#include <dapi.h>
#include <papi.h>

#include "pl_common.h"

#include "mxdlna.h"
#include "mxlog.h"

#include "mxres.h"
#include "mxfile.h"
#include "mxres_ref.h"
#include "mxmedia.h"

#define PL_COMMON_DEFAULT_SOCKETPORT	"50002"
#define PL_COMMON_IP_DELIM "."

#define ________________Private_Static_Variables___________________
STATIC HBOOL	bIs_mxdlna_init = FALSE;

#define ________________Internal_APIs_Prototype____________________
void PL_DLNA_ResourceTableInit(void);

#define ________________Internal_Functions___________________________

STATIC void	_pl_common_cb_GetContentID(const HCHAR *name, HUID uid, const void *data, HUINT32 size, void *userdata);

STATIC void __plcommon_printf (const char *filename, const char *function , int line, MX_INT32 num, const HCHAR *str)
{
	if(filename == NULL || function == NULL || str == NULL)
		return ;

	HLIB_LOG (filename, function, line, HxLOG_MESSAGE, "%s", str);

#if 0
	if((!strcmp(filename, "HMS_transcoderWrapper.cpp") || (!strcmp(filename, "transcoderControl.cpp"))))
		HxLOG_Debug("[%s:%s:%d] %s \n",filename, function, line, str);
#endif
}

STATIC int	__plcommon_ip_address(int **pp_int)
{
	HBOOL 	bIsAvailable = FALSE;
	HINT32	nDefIdx = -1;

	struct in_addr	 	address;
	HOMMA_INTEROP_NETIFADDR_t 	DevInfo;

	if(pp_int == NULL)
		return 0;

	HxSTD_MemSet(&DevInfo, 0x00, sizeof(HOMMA_INTEROP_NETIFADDR_t));

	if(HOMMA_INTEROP_NETWORK_CONF_IsNetworkAvailable(&bIsAvailable) != ERR_OK)
	{
		HxLOG_Error("network is not available.\n");
		return 0;
	}

	if(HOMMA_INTEROP_NETWORK_CONF_GetDefaultDev(&nDefIdx) != ERR_OK)
	{
		HxLOG_Error("FAIL. default network interface index.\n");
		return 0;
	}

	if(nDefIdx > -1)
	{
		if(HOMMA_INTEROP_NETWORK_CONF_GetAddrInfo(nDefIdx, &DevInfo) != ERR_OK)
		{
			HxLOG_Error("FAIL. default network address info.\n");
			return 0;
		}
	}

	HxSTD_MemCopy(&address, (struct in_addr *)&DevInfo.ulIpAddr, sizeof(struct in_addr));
	HxLOG_Debug("IP - %s \n", (char *)inet_ntoa((struct in_addr)address));

	*pp_int = (int *)malloc(sizeof(int));
	**pp_int = DevInfo.ulIpAddr;

	return 1;
}

// Custom malloc/free (known issue: strdup of hxlib version must be included)
/*
static void *__plcommon_malloc(unsigned int _Size)
{
	return HLIB_STD_MemAlloc(_Size);
}

static void __plcommon_free(void *_Memory)
{
	HLIB_STD_MemFree(_Memory);
}
*/

STATIC void	_pl_common_cb_GetContentID(const HCHAR *name, HUID uid, const void *data, HUINT32 size, void *userdata)
{
	DAxIterator_t		iter;
	HUID    *cid  = (HUID*)userdata;

	HxLOG_Info("%s(name:%s, uid:"HUID_FMT", data:0x%X, size:%u, qid:%d) time:%s(%u)\n"
			, __FUNCTION__, name, uid, data, size, (int)userdata
			, HLIB_DATETIME_UnixTimeToString(HLIB_STD_GetSystemTime(), NULL, 0), HLIB_STD_GetSystemTick());

	if( size == 0 )
	{
		HxLOG_Warning("failed because of size = 0\n" );
		return;
	}

	DAPI_InitIterator(&iter, data, size, 0);
	while (DAPI_NextIterator(&iter))
	{
		*cid = iter.uid;
		return; // 2013/11/27 mhkang: 같은 경로의 같은 파일이름으로 레코드가 2개 들어가 있을 수 없다. 바로 return.
	}
}

#define ________________Public_Functions___________________________
void PL_DLNA_Init(void)
{
	MXDLNA_DISABLE_IP tDisableIP = {0};

	if(bIs_mxdlna_init == TRUE)
		return;

	// Custom malloc/free (known issue: strdup of hxlib version must be included)
	/*
	{
		MXKERNEL_CONFIGURE_DATA configure_data = {0};

		configure_data.cb_malloc = __plcommon_malloc;
		configure_data.cb_free = __plcommon_free;

		mxKernel_configure(&configure_data);
	}
	*/

	mxLog_init( MX_LEVEL_MORE, NULL, __plcommon_printf);
	mxLog_setLevel( MX_LEVEL_MORE);

	mxDlna_start();
#if defined(CONFIG_HOMMA_RC)
	mxDlna_bootid_setPath("/usr/dlna/bootid");
#endif

#if defined(CONFIG_LXC_ONLY) || defined(CONFIG_CHROOT_AND_LXC)
	mxDlna_setDefine(MXLIB_NIC_CB_SUPPORT); /* Only LXC platform model */
	mxDlna_NIC_create(__plcommon_ip_address, NULL);
#else
	/* redmine #78981: Dont set MXLIB_NIC_CB_SUPPORT; dont use mxDlna_NIC_change() under non-LXC platform */
	mxDlna_NIC_create(NULL, NULL);
#endif

	if (1)
	{
		tDisableIP.ip_start = 0; /* htonl(inet_addr("0.0.0.0")); */
		tDisableIP.ip_end = 0;   /* htonl(inet_addr("0.0.0.0")); */

		mxDlna_setDisableIP( &tDisableIP, 1);
	}

	mxFile_init((MXFILE_INIT_OPTION *)NULL);

	PL_DLNA_ResourceTableInit();

	bIs_mxdlna_init = TRUE;
}

void PL_DLNA_MxLib_LogLevel(int nlevel)
{
	if(nlevel)
		mxLog_setLevel( MX_LEVEL_MORE);
	else
		mxLog_setLevel( MX_LEVEL_NONE );
}

void PL_DLNA_IP_Changed(void)
{
	/* defined(CONFIG_PROD_HMS1000S) || defined(CONFIG_PROD_HMS1000T) */
	/* redmine #78981: Dont set MXLIB_NIC_CB_SUPPORT; dont use mxDlna_NIC_change() under non-LXC platform */

#if defined(CONFIG_LXC_ONLY) || defined(CONFIG_CHROOT_AND_LXC)
	if(bIs_mxdlna_init == TRUE)
	{
		HxLOG_Error("[HOM:%s:%d] Info> IP Changed!\n", __FUNCTION__,__LINE__);
		mxDlna_NIC_change(); /* Only LXC platform model */
	}
#endif
}

HERROR	PL_COMMON_GetProductInfo(PL_COMMON_ProductInfo_t *pInfo)
{
	HOM_ASSERT(pInfo);

	HxSTD_MemSet(pInfo, 0, sizeof(PL_COMMON_ProductInfo_t));

	HOMMA_INTEROP_SYSINFO_STB_GetManufacturerName(pInfo->szManufacturer, PL_COMMON_NAME_SHORT);
	if(HxSTD_StrEmpty(pInfo->szManufacturer))
		HxSTD_snprintf(pInfo->szManufacturer, sizeof(pInfo->szManufacturer), "%s", DEFAULT_MANUFACTURER_NAME);

	HOMMA_INTEROP_SYSINFO_STB_GetModelName(pInfo->szModelName, PL_COMMON_NAME_MIDDLE);
	if(HxSTD_StrEmpty(pInfo->szModelName))
		HxSTD_snprintf(pInfo->szModelName, sizeof(pInfo->szModelName), "%s", DEFAULT_MODEL_NAME);

	HxSTD_snprintf(pInfo->szManufacturerUrl, 	sizeof(pInfo->szManufacturerUrl), 	"%s", DEFAULT_MANUFACTURER_URL);
	HxSTD_snprintf(pInfo->szModelDescription,	sizeof(pInfo->szModelDescription),	"%s", DEFAULT_MODEL_DESCRIPTION);
	HxSTD_snprintf(pInfo->szModelNumber, 		sizeof(pInfo->szModelNumber), 		"%s", DEFAULT_MODEL_NUMBER);
	HxSTD_snprintf(pInfo->szModelUrl, 			sizeof(pInfo->szModelUrl), 			"%s", DEFAULT_MODEL_URL);

	HOMMA_INTEROP_SYSINFO_STB_GetSerialNumber(pInfo->szSerialNumber, PL_COMMON_NAME_SHORT);
	if(HxSTD_StrEmpty(pInfo->szSerialNumber))
		HxSTD_snprintf(pInfo->szSerialNumber, sizeof(pInfo->szSerialNumber), "%s", DEFAULT_MODEL_SERIAL_NUMBER);

	return ERR_OK;
}

HBOOL PL_COMMON_IsDigitString(HCHAR *pszIpAddr)
{
	HCHAR *pszIpString = pszIpAddr;

	if (pszIpString == NULL)
	{
		goto func_error;
	}

	while (*pszIpString)
	{
		if ((*pszIpString >= '0') &&
			(*pszIpString <= '9'))
		{
			++pszIpString;
		}
		else
		{
			goto func_error;
		}
	}
	return TRUE;

func_error:
	HxLOG_Error("Error> invalid IP string(%s,%s)\n", pszIpString, pszIpAddr);
	return FALSE;
}

HBOOL PL_COMMON_ValidIPString(HCHAR *pszIpAddr)
{
	HINT32 nIpPart, nNumOfDot = 0;
	HCHAR *pszIpString = NULL;
	HCHAR *pzToken = NULL;
	HCHAR *pSave = NULL;

	pszIpString = HLIB_STD_StrDup(pszIpAddr);
	if (pszIpString == NULL)
	{
		goto func_error;
	}

	pzToken = HxSTD_StrTok(pszIpString, PL_COMMON_IP_DELIM, &pSave);
	if (pzToken == NULL)
	{
		goto func_error;
	}

	while (pzToken)
	{
		if (!PL_COMMON_IsDigitString(pzToken))
		{
			goto func_error;
		}

		if (!HLIB_STD_IsInteger(pzToken))
		{
			goto func_error;
		}
		nIpPart = HLIB_STD_StrToINT32(pzToken);

		/* check for valid IP */
		if (nIpPart >= 0 && nIpPart <= 255)
		{
			/* parse remaining string */
			pzToken = HxSTD_StrTok(NULL, PL_COMMON_IP_DELIM, &pSave);
			if (pzToken != NULL) ++nNumOfDot;
		}
		else
		{
			goto func_error;
		}
	}

	if (nNumOfDot != 3)
	{
		goto func_error;
	}
	//HxLOG_Error("OK> IP string(%s,%s, dot:%d)\n", pszIpString, pszIpAddr, nNumOfDot);

	if (pszIpString != NULL)
	{
		HLIB_STD_MemFree(pszIpString);
		pszIpString = NULL;
	}
	return TRUE;

func_error:
	HxLOG_Error("Error> invalid IP string(%s,%s, dot:%d)\n", pszIpString, pszIpAddr, nNumOfDot);

	if (pszIpString != NULL)
	{
		HLIB_STD_MemFree(pszIpString);
		pszIpString = NULL;
	}
	return FALSE;
}

HERROR	PL_COMMON_GetDefaultNetwork(PL_COMMON_NetworkInfo_t *pInfo)
{
	HERROR hErr;
	HUINT32	dev_idx = 0, i, len;
	HOMMA_INTEROP_NETDEVINFO_t 	dev_info;
	HOMMA_INTEROP_NETIFADDR_t	net_addrs;
	HUINT32				p;
	struct in_addr	 	address;

	if (pInfo == NULL) {
	  return ERR_FAIL;
	}

	HxSTD_MemSet(pInfo, 0, sizeof(PL_COMMON_NetworkInfo_t));
	pInfo->nDevIdx = (-1);
	pInfo->bValid = FALSE;

	HxSTD_MemSet(&dev_info, 0, sizeof(HOMMA_INTEROP_NETDEVINFO_t));
	HxSTD_MemSet(&net_addrs, 0, sizeof(HOMMA_INTEROP_NETIFADDR_t));

	hErr = HOMMA_INTEROP_NETWORK_CONF_GetDefaultDev(&dev_idx);
	if (hErr != ERR_OK)
	{
		HxLOG_Error("Error> GetDefaultDev() failed\n");
		goto func_error;
	}
	pInfo->nDevIdx = dev_idx;

	hErr = HOMMA_INTEROP_NETWORK_CONF_GetDevInfo(dev_idx, &dev_info);
	if (hErr != ERR_OK)
	{
		HxLOG_Error("Error> GetDevInfo(devidx:%d) failed\n", dev_idx);
	}
	pInfo->bConnected = dev_info.bConnected;
	pInfo->bActive    = dev_info.bNetActive;
	pInfo->bRunning   = dev_info.bRunning;

	/*  remove mac ':' */
	len = HxSTD_StrLen(dev_info.szMacAddr);
	for(i=0; i<len; i++)
	{
		if(dev_info.szMacAddr[i] == ':')
			HxSTD_MemMove(dev_info.szMacAddr+i, dev_info.szMacAddr+i+1, len-1);
	}
	HxSTD_snprintf(pInfo->szMacAddr, sizeof(pInfo->szMacAddr), "%s", dev_info.szMacAddr);

	hErr = HOMMA_INTEROP_NETWORK_CONF_GetAddrInfo( dev_idx, &net_addrs);
	if (hErr != ERR_OK)
	{
		HxLOG_Error("Error> GetAddrInfo(devidx:%d) failed\n", dev_idx);
	}

	pInfo->ulIpAddr = net_addrs.ulIpAddr;
	pInfo->ulNetmask= net_addrs.ulNetmask;
	pInfo->ulGateway= net_addrs.ulGateway;

	p = net_addrs.ulIpAddr;
	HxSTD_MemCopy(&address, (struct in_addr *)&p, sizeof(struct in_addr));
	HxSTD_StrNCpy(pInfo->szIp, (HCHAR *)inet_ntoa((struct in_addr)address), sizeof(pInfo->szIp)-1);

	p = net_addrs.ulNetmask;
	HxSTD_MemCopy(&address,  (struct in_addr *)&p, sizeof(struct in_addr));
	HxSTD_StrNCpy(pInfo->szNetmask, (HCHAR *)inet_ntoa((struct in_addr)address), sizeof(pInfo->szNetmask)-1);

	p = net_addrs.ulGateway;
	HxSTD_MemCopy(&address,  (struct in_addr *)&p, sizeof(struct in_addr));
	HxSTD_StrNCpy(pInfo->szGateway, (HCHAR *)inet_ntoa((struct in_addr)address), sizeof(pInfo->szGateway)-1);
	HxLOG_Error("Info> Got network[devidx:%d, IP:0x%x,%s]\n", dev_idx, (unsigned int)pInfo->ulIpAddr, pInfo->szIp);

	if (pInfo->ulIpAddr != 0x0)
	{
		pInfo->bValid = TRUE;
	}
	else
	{
		HxLOG_Error("Error> [devIdx   :%d]\n", pInfo->nDevIdx);
		HxLOG_Error("Error> [Valid    :%d]\n", pInfo->bValid);
		HxLOG_Error("Error> [Connected:%d]\n", pInfo->bConnected);
		HxLOG_Error("Error> [Active   :%d]\n", pInfo->bActive);
		HxLOG_Error("Error> [IpAddr   :0x%x]\n", pInfo->ulIpAddr);
		HxLOG_Error("Error> [IP       :%s]\n", pInfo->szIp);
	}
	return ERR_OK;

func_error:
	pInfo->bValid = FALSE;

	return ERR_FAIL;
}

HERROR	PL_COMMON_GetActiveNetwork(PL_COMMON_NetworkInfo_t *pInfo)
{
	HERROR hErr = ERR_OK;
	HOMMA_INTEROP_NETDEVINFO_t 	tNetInfo = { 0 };
	HOMMA_INTEROP_NETIFADDR_t 	tNetAddr = { 0 };
	HCHAR	szMacBuf[20] = { 0 };
	HINT32 devCnt = 0;
	HINT32 i=0, didx = 0;
	HUINT32	nlen = 0;
	HINT32 nAddr;
	struct in_addr inAddr;
	HUINT32	uMatchCnt = 0;

	if(NULL == pInfo)
	{
		HxLOG_Error("pInfo is NULL\n");
		return ERR_FAIL;
	}

	HxSTD_MemSet(pInfo, 0, sizeof(PL_COMMON_NetworkInfo_t));
	pInfo->nDevIdx = (-1); /* for no matched devIndex */
	hErr = HOMMA_INTEROP_NETWORK_CONF_GetNumOfNetDev( &devCnt );
	if (hErr != ERR_OK)
	{
		HxLOG_Error("Error> GetNumOfNetDev() failed\n");
		pInfo->nDevIdx = (-1);
		goto func_error;
	}

	for ( didx = 0; didx < devCnt && didx < NETWORK_INTERFACE_NUM_MAX ; didx++ )
	{
		HxSTD_MemSet( &tNetInfo, 0x00, sizeof(HOMMA_INTEROP_NETDEVINFO_t) );
		HxSTD_MemSet( &tNetAddr, 0x00, sizeof(HOMMA_INTEROP_NETIFADDR_t) );
		HxSTD_MemSet( szMacBuf, 0x00, sizeof(szMacBuf) );

		hErr = HOMMA_INTEROP_NETWORK_CONF_GetDevInfo( didx, &tNetInfo);
		if (hErr != ERR_OK)
		{
			HxLOG_Error("Error> GetDevInfo(devidx:%d) failed\n", didx);
			continue;
		}
		if (tNetInfo.bNetActive != TRUE)
		{
			continue;
		}
		pInfo->nDevIdx    = didx;
		pInfo->bConnected = tNetInfo.bConnected;
		pInfo->bActive    = tNetInfo.bNetActive;
		pInfo->bRunning   = tNetInfo.bRunning;

		/*  remove mac ':' */
		nlen = HxSTD_StrLen(tNetInfo.szMacAddr);
		for(i=0; i<nlen; i++)
		{
			if(tNetInfo.szMacAddr[i] == ':')
				HxSTD_MemMove(tNetInfo.szMacAddr+i, tNetInfo.szMacAddr+i+1, nlen-1);
		}
		HxSTD_snprintf(pInfo->szMacAddr, sizeof(pInfo->szMacAddr), "%s", tNetInfo.szMacAddr);

		hErr = HOMMA_INTEROP_NETWORK_CONF_GetAddrInfo( didx, &tNetAddr);
		if (hErr != ERR_OK)
		{
			HxLOG_Error("Error> GetAddrInfo(devidx:%d) failed\n", didx);
		}

		pInfo->ulIpAddr = tNetAddr.ulIpAddr;
		pInfo->ulNetmask= tNetAddr.ulNetmask;
		pInfo->ulGateway= tNetAddr.ulGateway;

		nAddr = tNetAddr.ulIpAddr;
		HxSTD_MemCopy(&inAddr, (struct in_addr *)&nAddr, sizeof(struct in_addr));
		HxSTD_StrNCpy(pInfo->szIp, (HCHAR *)inet_ntoa((struct in_addr)inAddr), sizeof(pInfo->szIp)-1);

		nAddr = tNetAddr.ulNetmask;
		HxSTD_MemCopy(&inAddr,  (struct in_addr *)&nAddr, sizeof(struct in_addr));
		HxSTD_StrNCpy(pInfo->szNetmask, (HCHAR *)inet_ntoa((struct in_addr)inAddr), sizeof(pInfo->szNetmask)-1);

		nAddr = tNetAddr.ulGateway;
		HxSTD_MemCopy(&inAddr,  (struct in_addr *)&nAddr, sizeof(struct in_addr));
		HxSTD_StrNCpy(pInfo->szGateway, (HCHAR *)inet_ntoa((struct in_addr)inAddr), sizeof(pInfo->szGateway)-1);
		uMatchCnt++;

		if ((tNetInfo.bNetActive == TRUE) && (tNetInfo.bRunning == TRUE))
		{
			HxLOG_Error("Got (didx:%d)'s IP(0x%x, %s)\n", didx, pInfo->ulIpAddr, pInfo->szIp);
		}
	}

	if (uMatchCnt == 0)
	{
		HxLOG_Error("Error> No matched active network!\n");
		pInfo->nDevIdx = (-1);
		goto func_error;
	}

	if (pInfo->ulIpAddr == 0x0)
	{
		pInfo->nDevIdx = (-1);
		goto func_error;
	}

	pInfo->bValid = TRUE;
	HxLOG_Error("Got networkInfo[didx:%d, IP:0x%x,%s]\n", pInfo->nDevIdx, pInfo->ulIpAddr, pInfo->szIp);
	return ERR_OK;

func_error:
	if (pInfo != NULL)
	{
		pInfo->bValid = FALSE;
		HxLOG_Error("Error> Got invalid networkInfo[didx:%d, IP:0x%x,%s]\n", pInfo->nDevIdx, pInfo->ulIpAddr, pInfo->szIp);
	}
	return ERR_FAIL;
}

HERROR	PL_COMMON_GetDefaultMac(PL_COMMON_MacInfo_t *pMacInfo) /* TODO: Check "Default" whether that means the same as "ethernet" */
{
	HERROR hError = ERR_OK;
	HUINT32	dev_idx = 0, i, len;
	HOMMA_INTEROP_NETDEVINFO_t 	dev_info;

	if (NULL == pMacInfo)
	{
		HxLOG_Error("Error> invalid arg\n");
		return ERR_FAIL;
	}

	HxSTD_MemSet(pMacInfo, 0, sizeof(PL_COMMON_MacInfo_t));
	HxSTD_MemSet(&dev_info, 0, sizeof(HOMMA_INTEROP_NETDEVINFO_t));

	//HOMMA_INTEROP_NETWORK_Init();
	hError = HOMMA_INTEROP_NETWORK_CONF_GetDefaultDev(&dev_idx);
	if (hError != ERR_OK)
	{
		HxLOG_Error("Can not get device(err:%d)!\n", hError);
		goto func_error;
	}

	hError = HOMMA_INTEROP_NETWORK_CONF_GetDevInfo(dev_idx, &dev_info);
	if (hError != ERR_OK)
	{
		HxLOG_Error("Can not get deviceInfo(err:%d)!\n", hError);
		goto func_error;
	}

	len = HxSTD_StrLen(dev_info.szMacAddr);

	/*  remove mac ':' */
	for(i=0; i<len; i++)
	{
		if(dev_info.szMacAddr[i] == ':')
			HxSTD_MemMove(dev_info.szMacAddr+i, dev_info.szMacAddr+i+1, len-1);
	}

	HxSTD_snprintf(pMacInfo->szMacAddr, sizeof(pMacInfo->szMacAddr), "%s", dev_info.szMacAddr);
	HxLOG_Print("Got default mac[dev_idx:%d, %s]\n", pMacInfo->nDevIdx, pMacInfo->szMacAddr);
	return ERR_OK;

func_error:
	HxLOG_Error("Critical/Error> failed to get default mac\n");
	HxSTD_MemSet(pMacInfo, 0, sizeof(PL_COMMON_MacInfo_t));
	return ERR_FAIL;
}

HERROR	PL_COMMON_GetEthMac(PL_COMMON_MacInfo_t *pMacInfo)
{
	HERROR hError = ERR_OK;
	HOMMA_INTEROP_NETDEVINFO_t 	tDevInfo = { 0 };
	HCHAR	szMacBuf[20] = { 0 };
	HINT32 devCnt = 0;
	HINT32 i=0, dev_idx = 0;
	HUINT32	nlen = 0;
	HUINT32	uMatchCnt = 0;

	if (NULL == pMacInfo)
	{
		HxLOG_Error("Error> invalid arg\n");
		return ERR_FAIL;
	}

	HxSTD_MemSet(pMacInfo, 0, sizeof(PL_COMMON_MacInfo_t));
	pMacInfo->nDevIdx = (-1); /* for no matched devIndex */
	hError = HOMMA_INTEROP_NETWORK_CONF_GetNumOfNetDev( &devCnt );
	if (hError != ERR_OK)
	{
		HxLOG_Error("Error> GetNumOfNetDev() failed\n");
		pMacInfo->nDevIdx = (-1);
		goto func_error;
	}

	for ( dev_idx = 0; dev_idx < devCnt && dev_idx < NETWORK_INTERFACE_NUM_MAX ; dev_idx++ )
	{
		HxSTD_MemSet( &tDevInfo, 0x00, sizeof(HOMMA_INTEROP_NETDEVINFO_t) );
		HxSTD_MemSet( szMacBuf, 0x00, sizeof(szMacBuf) );

		hError = HOMMA_INTEROP_NETWORK_CONF_GetDevInfo( dev_idx, &tDevInfo);
		if (hError != ERR_OK)
		{
			HxLOG_Error("Error> GetDevInfo(devidx:%d) failed\n", dev_idx);
			continue;
		}

		if (tDevInfo.eType != eHOMMA_INTEROP_INTERFACE_TYPE_LAN)
		{
			continue;
		}

		pMacInfo->nDevIdx    = dev_idx;

		/*  remove mac ':' */
		nlen = HxSTD_StrLen(tDevInfo.szMacAddr);
		for (i=0; i<nlen; i++)
		{
			if (tDevInfo.szMacAddr[i] == ':')
				HxSTD_MemMove(tDevInfo.szMacAddr+i, tDevInfo.szMacAddr+i+1, nlen-1);
		}
		HxSTD_snprintf(pMacInfo->szMacAddr, sizeof(pMacInfo->szMacAddr)-1, "%s", tDevInfo.szMacAddr);
		uMatchCnt++;
	}

	if (uMatchCnt == 0)
	{
		HxLOG_Error("Error> No matched network!\n");
		pMacInfo->nDevIdx = (-1);
		goto func_error;
	}

	pMacInfo->bValid = TRUE;
	HxLOG_Print("Got ethernet mac[dev_idx:%d, %s]\n", pMacInfo->nDevIdx, pMacInfo->szMacAddr);
	return ERR_OK;

func_error:
	if (pMacInfo != NULL)
	{
		pMacInfo->bValid = FALSE;
		HxLOG_Error("Critical/Error> Got invalid networkInfo[dev_idx:%d]\n", pMacInfo->nDevIdx);
	}
	return ERR_FAIL;
}

void PL_COMMON_PrintTime(UNIXTIME *putCurrentTime)
{
	UNIXTIME        utCurrentTime = 0;
	HxDATETIME_t    stDate;
	HERROR          hErr = ERR_OK;

	if(putCurrentTime == NULL)
	{
		utCurrentTime = HLIB_STD_GetSystemTime();
	}
	else
	{
		utCurrentTime = *putCurrentTime;
	}


	hErr = HLIB_DATETIME_ConvertUnixTimeToDateTime(utCurrentTime, &stDate);
	if(hErr != ERR_OK)
		HxLOG_Error("HLIB_DATETIME_ConvertUnixTimeToDateTime returned err (%d)  [%s : %d]\n",hErr, __FUNCTION__,__LINE__);
	else
		HxLOG_Debug("%s Time : %d / %d / %d \t %d : %d : %d\n", (putCurrentTime==NULL)?"(C)":"-> ",
				stDate.stDate.usYear,
				stDate.stDate.ucMonth,
				stDate.stDate.ucDay,
				stDate.stTime.ucHour,
				stDate.stTime.ucMinute,
				stDate.stTime.ucSecond);
}

HERROR	PL_COMMON_WaitShutDown(const HCHAR *pszFrom, HUINT32 ulWaitTime)
{
	HxLOG_Error("[service:%s] --> WaitShutDown(%d)\n", pszFrom, ulWaitTime);
	return PAPI_WaitForShutDown(ulWaitTime);
}

/**
  @brief media path정보를 갖고 DxRecListData_t db table안의 uid 값을 가져온다.
  @param media_path  DxRecListData_t.path 형태의 string. 주의) 확장자 hjm 없음.
		ex) "/mnt/hd2/Recordings/Death in Paradise_20160204_2112"
  @return 실패하면 -1 값이 반환된다.
**/
HUID PL_COMMON_GetContentID(HCHAR* media_path)
{
	HCHAR*  query = NULL;
	HUID	cid = -1;
	HUINT32	rId;

	query = HLIB_STD_FmtString(NULL, 0, "SELECT * FROM %s WHERE path = \"%s\"" , DxNAMEOF(DxRecListData_t) , media_path );
	if (query == NULL)
	{
		HxLOG_Error("HLIB_STD_FmtString failed(out of memory)\n");
		goto FUNC_END;
	}

	rId =  DAPI_Query(query, (DAPI_Getter)_pl_common_cb_GetContentID, &cid);
	HLIB_STD_FreeFmtString(query);
	query = NULL;

	if (rId == 0)
	{
		HxLOG_Error("DAPI_Query failed. rid = 0\n", rId);
		goto FUNC_END;
	}

	if( DAPI_Wait( 5000, rId ) == DERR_TIMEOUT)
	{
		HxLOG_Warning("DAPI_Wait api timeout!\n");
		goto FUNC_END;
	}

FUNC_END:
	return cid;
}

/* end od file */


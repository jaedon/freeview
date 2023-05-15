/**************************************************************
 *	@file		trport_tr157.c
 *	appkit for cwmp Module
 *
 *	http://www.humaxdigital.com
 *
 *	@data			2013/05/14
 *	@author			humax
 *	@brief			trport for cwmp
 **************************************************************/

/*******************************************************************/
/********************      Header Files    *************************/
/*******************************************************************/

#include <wait.h>
#include <hlib.h>
#include <hapi.h>
#include <papi.h>
#include <napi.h>
#include <oapi.h>

#include "rema_int.h"
#include "rema_cwmp.h"
#include "rema_DM.h"
#include "rema_util.h"

#if defined(CONFIG_OP_TDC)
#include "rema_DM_TDC.h"
#endif

#include "RDM_Network.h"

#include "RMOD_DI.h"


/*******************************************************************/
/********************      Global Variables     ********************/
/*******************************************************************/



/*******************************************************************/
/********************      Defines         *************************/
/*******************************************************************/

#define SERCURITYKEYLENGTH		(128 +32)
#define MANUFACTURER_NAME_LEN	(64)
#define MODEL_NAME_MAX_SIZE		(64)
#define SERIAL_NO_MAX_SIZE		(64)
#define ADDI_HWVER_MAX_LEN		(64)
#define SWVER_MAX_LEN			(64)
#define ADDI_SWVER_MAX_LEN		(64)

static HBOOL _bUseOriginVersion = FALSE;

//static APKE_NETAP_WIFI_Mode_e		s_WifiConnectMode;
#if defined(TEMP_NOT_USED)
static HCHAR	s_szWifiSecurityKey[SERCURITYKEYLENGTH] = {0, };
static HULONG	g_ulDefaultConnectionUpTime;
static HULONG	g_ulHomeNetworkConnectionUpTime;
#endif

#define TRPORT_SOFTWARE_VERSION_PREFIX		6
#define TRPORT_SOFTWARE_VERSION_DISCARD_POS		3


#define	REMA_NETWORK_MAX_MACADDR		(20)
#define DEFAULT_PERIODIC_INFORM_TIME 	"2013-01-01T00:00:00"


/*******************************************************************/
/********************      Typedef         *************************/
/*******************************************************************/



typedef struct {
	HBOOL	dhcp;
	HCHAR	macaddr[REMA_NETWORK_MAX_MACADDR];
	HCHAR	ipAddr[REMA_NETWORK_MAX_MACADDR];
	HCHAR	netmask[REMA_NETWORK_MAX_MACADDR];
	HCHAR	gateway[REMA_NETWORK_MAX_MACADDR];
	HCHAR	dns1[REMA_NETWORK_MAX_MACADDR];
	HCHAR	dns2[REMA_NETWORK_MAX_MACADDR];
} REMA_NET_Info_t;




/*******************************************************************/
/********************      Static Variables     ********************/
/*******************************************************************/
static REMA_NET_Info_t			s_stNetInfo;


static HERROR get_data_LAN_MACAddress(HCHAR *pszOutput);
static HERROR get_data_DeviceInfo_SerialNumber(HCHAR *pszData);


extern HERROR TRP135_Get_JCOM_Config_HomeServer_Enable(HCHAR *pszOutput);
extern HERROR TRP135_Set_JCOM_Config_HomeServer_Enable(HCHAR *pszInput);
extern HERROR TRP_Get_SoftwareModules_ExecEnv_Enable(HCHAR *pszOutput);
extern HERROR TRP_Set_SoftwareModules_ExecEnv_Enable(HCHAR *pszInput);
#if CONFIG_DEBUG
extern HBOOL TR069_DEBUG_IsHumaxNetwork(void);
#endif

/*******************************************************************/
/********************      Functions       *************************/
/*******************************************************************/
#define __________STATIC_FUNCTION_GET_DATA_______________________________________
HCHAR	macaddr[NAPI_MAX_MACADDR];
		HCHAR	ipAddr[NAPI_MAX_IP4ADDR];
		HCHAR	netmask[NAPI_MAX_IP4ADDR];
		HCHAR	gateway[NAPI_MAX_IP4ADDR];
		HCHAR	dns1[NAPI_MAX_IP4ADDR];
		HCHAR	dns2[NAPI_MAX_IP4ADDR];


HERROR TR157_module_Set_NetInfo(NAPINetConfInfo_t* netInfo)
{
#if defined(TEMP_NOT_USED)
	HCHAR szExIP[MAX_IP_LEN] = {0, };
#endif
	s_stNetInfo.dhcp = netInfo->dhcp;

	HxSTD_snprintf(s_stNetInfo.macaddr, NAPI_MAX_MACADDR, "%s", netInfo->ip_info.ipv4.macaddr);
	HxSTD_snprintf(s_stNetInfo.ipAddr, NAPI_MAX_IP4ADDR, "%s", netInfo->ip_info.ipv4.ipAddr);
	HxSTD_snprintf(s_stNetInfo.netmask, NAPI_MAX_IP4ADDR, "%s",netInfo->ip_info.ipv4.netmask);
	HxSTD_snprintf(s_stNetInfo.gateway, NAPI_MAX_IP4ADDR, "%s", netInfo->ip_info.ipv4.gateway);
	HxSTD_snprintf(s_stNetInfo.dns1, NAPI_MAX_IP4ADDR, "%s", netInfo->ip_info.ipv4.dns1);
	HxSTD_snprintf(s_stNetInfo.dns2, NAPI_MAX_IP4ADDR, "%s", netInfo->ip_info.ipv4.dns2);

	return ERR_OK;
}

REMA_NET_Info_t TR157_module_Get_NetInfo(void)
{
	return s_stNetInfo;
}

HERROR TR157_module_Init_NetInfo(void)
{
	HINT32 numOfNetDev, idx = 0;
	NAPINetConfInfo_t* tempInfo = NULL;

	REM_FUNC_ENTER;

	numOfNetDev = NAPI_NetConf_GetCount();
	RemLOG_Print("#####[HKJEONG][%s] numOfNetDev:%d \n",__FUNCTION__,numOfNetDev);
	if(numOfNetDev>0){
		tempInfo = (NAPINetConfInfo_t*) HLIB_STD_MemAlloc(sizeof(NAPINetConfInfo_t));
		if(tempInfo)
		{
			HxSTD_MemSet(tempInfo, 0, sizeof(NAPINetConfInfo_t));
		}else{
			RemLOG_Print("No NetConf!!! %s(%d Line)\r\n",__func__,__LINE__);
			return ERR_FAIL;
		}
	}else{
		RemLOG_Print("No NetConf!!! %s(%d Line)\r\n",__func__,__LINE__);
		return ERR_FAIL;
	}

	for(idx=0; idx<numOfNetDev;idx++)
	{
		NAPI_NetConf_GetInfo(idx, tempInfo);

		if( tempInfo->type == eNAPI_NETCONF_LAN )
		{
			TR157_module_Set_NetInfo(tempInfo);
		}
		else if(tempInfo->type == eNAPI_NETCONF_WIFI)
		{
			RemLOG_Print("eNAPI_NETCONF_WIFI Found!!! %s(%d Line)\r\n",__func__,__LINE__);
		}
	}

	if(tempInfo)
	{
		HLIB_STD_MemFree(tempInfo);
		tempInfo = NULL;
	}

	REM_FUNC_LEAVE;

	return ERR_OK;
}


HERROR TR157_module_Init(void)
{
	REM_FUNC_ENTER;
	HxSTD_MemSet(s_stNetInfo.macaddr,0x00,REMA_NETWORK_MAX_MACADDR);
	HxSTD_MemSet(s_stNetInfo.ipAddr,0x00,REMA_NETWORK_MAX_MACADDR);
	HxSTD_MemSet(s_stNetInfo.netmask,0x00,REMA_NETWORK_MAX_MACADDR);
	HxSTD_MemSet(s_stNetInfo.gateway,0x00,REMA_NETWORK_MAX_MACADDR);
	HxSTD_MemSet(s_stNetInfo.dns1,0x00,REMA_NETWORK_MAX_MACADDR);
	HxSTD_MemSet(s_stNetInfo.dns2,0x00,REMA_NETWORK_MAX_MACADDR);

	TR157_module_Init_NetInfo();//When I get this information?

    REM_FUNC_LEAVE;

    return ERR_OK;
}

HERROR TR157_module_DeInit(void)
{
	REM_FUNC_ENTER;

	HxSTD_MemSet(s_stNetInfo.macaddr,0x00,REMA_NETWORK_MAX_MACADDR);
	HxSTD_MemSet(s_stNetInfo.ipAddr,0x00,REMA_NETWORK_MAX_MACADDR);
	HxSTD_MemSet(s_stNetInfo.netmask,0x00,REMA_NETWORK_MAX_MACADDR);
	HxSTD_MemSet(s_stNetInfo.gateway,0x00,REMA_NETWORK_MAX_MACADDR);
	HxSTD_MemSet(s_stNetInfo.dns1,0x00,REMA_NETWORK_MAX_MACADDR);
	HxSTD_MemSet(s_stNetInfo.dns2,0x00,REMA_NETWORK_MAX_MACADDR);

	REM_FUNC_LEAVE;
	return ERR_OK;
}

HCHAR *TR157_module_Replace_String ( const HCHAR *string, const HCHAR *substr, const HCHAR *replacement )
{
	HCHAR *tok = NULL;
	HCHAR *newstr = NULL;
	HCHAR *oldstr = NULL;
	HCHAR *head = NULL;

	if(string == NULL)
	{
		RemLOG_Error("string is NULL!!!\n");
		return NULL;
	}

	/* if either substr or replacement is NULL, duplicate string a let caller handle it */
	if ( substr == NULL || replacement == NULL )
		return strdup (string);

	newstr = strdup (string);
	if(newstr == NULL)
	{
		RemLOG_Error("newstr is NULL!!!\n");
		return NULL;
	}

	head = newstr;

	while ( (tok = strstr ( head, substr )))
	{
		oldstr = newstr;
		if(oldstr)
		{
			newstr = HLIB_STD_MemAlloc ( strlen ( oldstr ) - strlen ( substr ) + strlen ( replacement ) + 1 );

			/*failed to alloc mem, free old string and return NULL */
			if ( newstr == NULL )
			{
				HLIB_STD_MemFree (oldstr);
				return NULL;
			}

			memcpy ( newstr, oldstr, tok - oldstr );
			memcpy ( newstr + (tok - oldstr), replacement, strlen ( replacement ) );
			memcpy ( newstr + (tok - oldstr) + strlen( replacement ), tok + strlen ( substr ), strlen ( oldstr ) - strlen ( substr ) - ( tok - oldstr ) );
			memset ( newstr + strlen ( oldstr ) - strlen ( substr ) + strlen ( replacement ) , 0, 1 );

			/* move back head right after the last replacement */
			head = newstr + (tok - oldstr) + strlen( replacement );

			HLIB_STD_MemFree(oldstr);
		}
	}

	return newstr;
}

static HERROR get_data_Device_DeviceSummary(HCHAR *pszOutput)
{
	HERROR hErr = ERR_OK;

	CHECK_NULL(pszOutput);
	hErr = rema_DM_getData_Device_DeviceSummary(pszOutput);

	REM_LEAVE(hErr);
}

static HERROR get_data_DeviceInfo_X_JCOM_SubscriberID(HCHAR *pszOutput)
{
	HERROR hErr = ERR_OK;
	//HUINT32 ulLen = 256;
	RemLOG_Print("%s(%d Line)\r\n",__func__,__LINE__);
#if 1
	//Server Connection 에서는 TR069 DB를 사용함
	hErr = rema_DM_getData_DeviceInfo_X_JCOM_SubscriberID(pszOutput);
#else
	HxSTD_MemSet(pszOutput, 0, sizeof(HCHAR)*ulLen);
	CHECK_NULL(pszOutput);

	hErr = APK_SQCSCENARIOJAPAN_GetSubscriberID(pszOutput, ulLen);
	if (ERR_OK != hErr)
	{
		REM_LEAVE(hErr);
	}
	pszOutput[ulLen -1] = '\0';
#endif
	RemLOG_Print("pszOutput(%d)\r\n",pszOutput);

	REM_LEAVE(hErr);
}

static HERROR get_data_DeviceInfo_X_JCOM_ServiceID(HCHAR *pszOutput)
{
	HERROR hErr = ERR_OK;
	//HUINT32 ulLen = 256;
	RemLOG_Print("%s(%d Line)\r\n",__func__,__LINE__);
#if 1
	//Server Connection 에서는 TR069 DB를 사용함
	hErr = rema_DM_getData_DeviceInfo_X_JCOM_ServiceID(pszOutput);
#else
	HxSTD_MemSet(pszOutput, 0, sizeof(HCHAR)*ulLen);
	CHECK_NULL(pszOutput);

	hErr = APK_SQCSCENARIOJAPAN_GetServiceID(pszOutput, ulLen);
	if (ERR_OK != hErr)
	{
		REM_LEAVE(hErr);
	}
	pszOutput[ulLen -1] = '\0';
#endif
	RemLOG_Print("pszOutput(%d)\r\n",pszOutput);

	REM_LEAVE(hErr);
}
static HERROR get_data_DeviceInfo_X_JCOM_FamilyID(HCHAR *pszOutput)
{
	HERROR hErr = ERR_OK;
	//HUINT32 ulLen = 256;
	RemLOG_Print("%s(%d Line)\r\n",__func__,__LINE__);
#if 1
	//Server Connection 에서는 TR069 DB를 사용함
	hErr = rema_DM_getData_DeviceInfo_X_JCOM_FamilyID(pszOutput);
#else
	HxSTD_MemSet(pszOutput, 0, sizeof(HCHAR)*ulLen);
	CHECK_NULL(pszOutput);

	hErr = APK_SQCSCENARIOJAPAN_GetFamilyID(pszOutput, ulLen);
	if (ERR_OK != hErr)
	{
		REM_LEAVE(hErr);
	}
	pszOutput[ulLen -1] = '\0';
#endif
	RemLOG_Print("pszOutput(%d)\r\n",pszOutput);

	REM_LEAVE(hErr);
}
static HERROR get_data_DeviceInfo_X_JCOM_BusinessCode(HCHAR *pszOutput)
{
	HERROR hErr = ERR_OK;
	//HUINT32 ulLen = 256;
	RemLOG_Print("%s(%d Line)\r\n",__func__,__LINE__);
#if 1
	//Server Connection 에서는 TR069 DB를 사용함
	hErr = rema_DM_getData_DeviceInfo_X_JCOM_BusinessCode(pszOutput);
#else
	HxSTD_MemSet(pszOutput, 0, sizeof(HCHAR)*ulLen);
	CHECK_NULL(pszOutput);

	hErr = APK_SQCSCENARIOJAPAN_GetBusinessCode(pszOutput, ulLen);
	if (ERR_OK != hErr)
	{
		REM_LEAVE(hErr);
	}
	pszOutput[ulLen -1] = '\0';
#endif
	RemLOG_Print("pszOutput(%d)\r\n",pszOutput);

	REM_LEAVE(hErr);
}

static HERROR get_data_DeviceInfo_Manufacturer(HCHAR *pszOutput)
{
	HERROR hErr = ERR_OK;

	CHECK_NULL(pszOutput);

	hErr = OAPI_SYSINFO_STB_GetManufacturerName(pszOutput, MANUFACTURER_NAME_LEN);
	if(hErr != ERR_OK)
		RemLOG_Error("Can not get ManufacturerName!!!\n");

	REM_LEAVE(hErr);
}


static HERROR get_data_DeviceInfo_ManufacturerOUI(HCHAR *pszOutput)
{
	HERROR hErr = ERR_OK;
	HCHAR szSTBMac[256] = {0,};
	HCHAR result[32]={0,};
	HINT32 i = 0;

	CHECK_NULL(pszOutput);
	//hErr = rema_DM_getData_DeviceInfo_ManufacturerOUI(pszOutput);

 //2 OUI값은 MAC Address 앞 6자리로 사용하도록 임시 수정. Server 측에서 OUI와 MAC 앞6자리를 비교한다고 함
	get_data_DeviceInfo_SerialNumber(szSTBMac);
	RemLOG_Print("MAC(%s)\r\n",szSTBMac);

	for(i = 0; i < 6; i++)
	{
		result[i] = szSTBMac[i];
	}
	result[6] = '\0';

	HLIB_STD_StrUtf8NCpy(pszOutput, result, SIZE_32);

	//HxSTD_StrUtf8NCpy(pszOutput, TRPORT_HUMAX_OUI, 32);

	REM_LEAVE(hErr);
}

static HERROR get_data_DeviceInfo_ModelName(HCHAR *pszOutput)
{
	HERROR hErr = ERR_OK;
	HCHAR szModelName[MODEL_NAME_MAX_SIZE] = {0, };

	CHECK_NULL(pszOutput);

	hErr = OAPI_SYSINFO_STB_GetModelName(szModelName, MODEL_NAME_MAX_SIZE);
	if(hErr == ERR_OK)
	{
		HxSTD_MemCopy(pszOutput, szModelName, HxSTD_StrLen(szModelName));
	}

	REM_LEAVE(hErr);
}

static HERROR get_data_DeviceInfo_Description(HCHAR *pszOutput)
{
	HERROR hErr = ERR_OK;

	CHECK_NULL(pszOutput);

	hErr = rema_DM_getData_DeviceInfo_Description(pszOutput);

	REM_LEAVE(hErr);
}

static HERROR get_data_DeviceInfo_ProductClass(HCHAR *pszOutput)
{
	HERROR hErr = ERR_OK;
	HCHAR szProductClass[256] = {0,};
	HCHAR szSerial[256] = {0,};
	HCHAR *szTempSerial = NULL;

	CHECK_NULL(pszOutput);

	hErr = rema_DM_getData_DeviceInfo_ProductClass(szProductClass);

	if(hErr != ERR_OK)
	{
		REM_LEAVE(hErr);
	}

	hErr = get_data_LAN_MACAddress(szSerial);

	if(hErr != ERR_OK)
	{
		HxSTD_PrintToStr(pszOutput, "%s", "0");
		REM_LEAVE(hErr);
	}

	szTempSerial = TR157_module_Replace_String(szSerial, ":", "");
	HxSTD_sprintf(pszOutput,"%s%s%s",szProductClass,"_",szTempSerial);

	if(szTempSerial)
	{
		HLIB_STD_MemFree(szTempSerial);
		szTempSerial = NULL;
	}

	REM_LEAVE(hErr);
}

static HERROR get_data_DeviceInfo_SerialNumber(HCHAR *pszData)
{
	HERROR hErr = ERR_OK;
	HCHAR szOutput[256] = {0,};

#if defined(CONFIG_OP_TDC)
#else
	HCHAR *pszTempData = NULL;
#endif

	CHECK_NULL(pszData);

#if defined(CONFIG_OP_TDC)
	hErr = OAPI_SYSINFO_STB_GetSerialNo(szOutput, SERIAL_NO_MAX_SIZE);
	if(hErr == ERR_OK)
	{
		HxSTD_MemCopy(pszData, szOutput, HxSTD_StrLen(szOutput));
	}
#else
	 //2 MAC 정보로 변경되어 사용안함 => get_data_LAN_MACAddress
	//hErr = APK_SQCBOXINFO_GetItem(eBoxInfo_Serial_No, pszData);


	hErr = get_data_LAN_MACAddress(szOutput);
	if(hErr != ERR_OK)
	{
		HxSTD_PrintToStr(pszData, "%s", "0");
		REM_LEAVE(hErr);
	}

	//RemLOG_Print("Serial No.(%s)\r\n",szOutput);

	pszTempData = TR157_module_Replace_String(szOutput, ":", "");
	if(pszTempData)
	{
		HxSTD_MemCopy(pszData, pszTempData, HxSTD_StrLen(pszTempData));
		HLIB_STD_MemFree(pszTempData);
		pszTempData = NULL;
	}
#endif

	REM_LEAVE(hErr);
}

static HERROR get_data_DeviceInfo_HardwareVersion(HCHAR *pszOutput)
{
	HERROR hErr = ERR_OK;

	CHECK_NULL(pszOutput);
	hErr = rema_DM_getData_DeviceInfo_HardwareVersion(pszOutput);
	REM_LEAVE(hErr);
}

static HERROR get_data_DeviceInfo_AdditionalHardwareVersion(HCHAR *pszOutput)
{
	HERROR hErr = ERR_OK;
	HCHAR szAdditionalHWVer[ADDI_HWVER_MAX_LEN] = {0, };

	CHECK_NULL(pszOutput);
	hErr = OAPI_SYSINFO_STB_GetChipId(szAdditionalHWVer, ADDI_HWVER_MAX_LEN);
	if(hErr == ERR_OK)
	{
		HxSTD_MemCopy(pszOutput, szAdditionalHWVer, HxSTD_StrLen(szAdditionalHWVer));
	}

	REM_LEAVE(hErr);
}

static HERROR get_data_DeviceInfo_SoftwareVersion(HCHAR *pszOutput)
{
	HERROR hErr = ERR_OK;
#if defined(CONFIG_OP_TDC)
#else
	HINT32 i = 0, j = 0;
	HINT32 len = 0;
	HCHAR	pszOutputBuff[SWVER_MAX_LEN];	
#endif
	HCHAR	szNewVersion[SWVER_MAX_LEN];
	
	szNewVersion[0]='\0';

	CHECK_NULL(pszOutput);
#if defined(CONFIG_OP_TDC)
	hErr = OAPI_SYSINFO_STB_GetPackageVersion(szNewVersion, SWVER_MAX_LEN);
	HxSTD_MemCopy(pszOutput, szNewVersion, HxSTD_StrLen(szNewVersion));
#else
	pszOutputBuff[0]='\0';

	/*To Do : Remove pre-fix of version name*/	
	len = HxSTD_StrLen(pszOutputBuff);
	RemLOG_Print("Real Software version(%s), len(%d)\r\n",pszOutputBuff,len);

	for(i = (TRPORT_SOFTWARE_VERSION_PREFIX+1); i < (len-TRPORT_SOFTWARE_VERSION_DISCARD_POS); i++)
	{
		szNewVersion[j++] = pszOutputBuff[i];
	}
	szNewVersion[j] = '\0';

#if CONFIG_USES_NFSROOT
	//  In case NFSROOT, Even though there is newer HDF image on Server, It is not needed to upgrade with newer HDF.
	HxSTD_StrCpy(pszOutput, "11.00");
#else
	HxSTD_StrCpy(pszOutput, szNewVersion);
#endif

	RemLOG_Print("Modified Software version(%s/%s), len(%d)\r\n",szNewVersion,pszOutput,HxSTD_StrLen(szNewVersion));
#endif

	REM_LEAVE(hErr);
}

static HERROR get_DevInfo_AdditionalSoftwareVersion(HCHAR *pszOutput)
{
	HERROR hErr = ERR_OK;
	HCHAR	szAddiSWVersion[ADDI_SWVER_MAX_LEN];
	
	CHECK_NULL(pszOutput);

	hErr = OAPI_SYSINFO_STB_GetSoftwareVersion(szAddiSWVersion, SWVER_MAX_LEN);
	HxSTD_MemCopy(pszOutput, szAddiSWVersion, HxSTD_StrLen(szAddiSWVersion));

	REM_LEAVE(hErr);
}

#if CONFIG_DEBUG
HERROR TRP_Modify_SoftwareVersion_ForOrg(void)
{
	_bUseOriginVersion = TRUE;

	RemLOG_Print("Manual Version Up Trigger!\r\n");

	return ERR_OK;
}
#endif
static HERROR get_data_DeviceInfo_DeviceStatus(HCHAR *pszOutput)
{
	HERROR hErr = ERR_OK;

	CHECK_NULL(pszOutput);

	//APK_3RDPARTY_CWMP_TR157_Get_CommonDM_String(pszOutput,eTR157A3_Device_DeviceInfo_DeviceStatus);
	pszOutput[0]='\0';

	REM_LEAVE(hErr);
}

#include <sys/sysinfo.h>
#include <time.h>

static HERROR get_data_DeviceInfo_UpTime(HUINT32 *pszOutput)
{
	HERROR hErr = ERR_OK;
#if defined(TEMP_NOT_USED)	
	HUINT32 ultic;
#endif	
	time_t current_time;
	HUINT32 lastBootTime;
	struct sysinfo info;	

	CHECK_NULL(pszOutput);
#if 0
	//세트의 부팅 시간을 구함. (tr069 프로세스의 동작시간)
	ultic = VK_TIMER_GetSystemTick ();
	//RemLOG_Print(" ultic:%d, currTime:%d \n",ultic, ultic/1000);
	snprintf(pszOutput, SIZE_16, "%d", ultic/1000);
#else

	sysinfo(&info);
 
	current_time = time(NULL);
	lastBootTime = current_time - info.uptime;

	*pszOutput = lastBootTime;
#endif	

	REM_LEAVE(hErr);
}

static HERROR get_data_DeviceInfo_FirstUseDate(HCHAR *pszOutput)
{
	HERROR hErr = ERR_OK;
#if 0
	HCHAR szOutput[SIZE_32];

	HULONG VK_Err;
	HULONG currentTime = 0;
	HxDATETIME_t dateTime;
	szOutput[0]='\0';

	CHECK_NULL(pszOutput);

	// Check DB
	TRP_Get_FirstUseDate(szOutput);

	// 0 => Factory Reset 의미
	if(HxSTD_StrCmp(szOutput,"0")==0)
	{
		// set time in DB
		VK_Err = VK_CLOCK_GetTime(&currentTime);
		if((VK_Err != VK_OK) || (currentTime == 0))
		{

			HxSTD_StrNCpy(pszOutput,"0000-01-01T00:00:00Z",(SIZE_32-1));
			return ERR_FAIL;
		}
		HxSTD_MemSet(&dateTime, 0x00, sizeof(HxDATETIME_T));

		HLIB_DATETIME_ConvertUnixTimeToDateTime(currentTime, &dateTime);

		HxSTD_PrintToStrN(pszOutput,SIZE_32,"%d-%02d-%02dT%02d:%02d:%02dZ",
								dateTime.stDate.usYear, dateTime.stDate.ucMonth, dateTime.stDate.ucDay,
								dateTime.stTime.ucHour,dateTime.stTime.ucMinute,dateTime.stTime.ucSecond);

		TRP_Set_FirstUseDate(pszOutput);
	}
	else
	{
		HxSTD_StrNCpy(pszOutput, szOutput,(SIZE_32-1));
	}
#else
	HxSTD_StrNCpy(pszOutput,"0000-01-01T00:00:00Z",(SIZE_32-1));
#endif

	REM_LEAVE(hErr);
}

static HERROR get_data_ManagementServer_URL(HCHAR *pszOutput)
{
	HERROR hErr = ERR_OK;

	CHECK_NULL(pszOutput);
	hErr = rema_DM_getData_ManagementServer_Url(pszOutput);

	REM_LEAVE(hErr);
}

static HERROR get_data_ManagementServer_Username(HCHAR *pszOutput)
{
	HERROR hErr = ERR_OK;

	CHECK_NULL(pszOutput);
	hErr = rema_DM_getData_ManagementServer_Username(pszOutput);

	REM_LEAVE(hErr);
}

static HERROR get_data_ManagementServer_Password(HCHAR *pszOutput)
{
	HERROR hErr = ERR_OK;

	CHECK_NULL(pszOutput);
	hErr = rema_DM_getData_ManagementServer_Password(pszOutput);

	REM_LEAVE(hErr);
}

static HERROR get_data_ManagementServer_PeriodicInformEnable(HCHAR *pszOutput)
{
	HERROR hErr = ERR_OK;

	CHECK_NULL(pszOutput);
	hErr = rema_DM_getData_ManagementServer_PeriodicInformEnable(pszOutput);

	REM_LEAVE(hErr);
}

static HERROR get_data_ManagementServer_PeriodicInformInterval(HCHAR *pszOutput)
{
	HERROR hErr = ERR_OK;

	CHECK_NULL(pszOutput);
	hErr = rema_DM_getData_ManagementServer_PeriodicInformInterval(pszOutput);

	REM_LEAVE(hErr);
}

static HERROR get_data_ManagementServer_PeriodicInformTime(HCHAR *pszOutput)
{
	HERROR hErr = ERR_OK;

	CHECK_NULL(pszOutput);
	hErr = rema_DM_getData_ManagementServer_PeriodicInformTime(pszOutput);

	REM_LEAVE(hErr);
}


static HERROR get_data_ManagementServer_ParameterKey(HCHAR *pszOutput)
{
	HERROR hErr = ERR_OK;

	CHECK_NULL(pszOutput);
	hErr = rema_DM_getData_ManagementServer_ParameterKey(pszOutput);

	REM_LEAVE(hErr);
}

static HERROR get_data_ManagementServer_ConnectionRequestURL(HCHAR *pszOutput)
{
	HERROR hErr = ERR_OK;
#if defined(TEMP_NOT_USED)	
	HCHAR	szExIP[256] = {0, };
#endif
	HCHAR 	szCHIPID[ADDI_HWVER_MAX_LEN] = {0, };

	CHECK_NULL(pszOutput);

#if defined(TEMP_NOT_USED)
	if (REMA_Util_Get_ExternalIP(szExIP, sizeof(szExIP)) == 0)
	{
		// Use external IP
		HxSTD_sprintf(pszOutput, "http://%s%s", szExIP, ":9090/connreq");
	}
	else
	{
		RemLOG_Warning("Cannot get External IP!!! \n");

		if (s_stNetInfo.ipAddr != 0)
		{
			HxSTD_sprintf(pszOutput, "http://%s%s", s_stNetInfo.ipAddr, ":9090/connreq");
		}
	}
#endif
		
#if defined(CONFIG_OP_TDC)
	hErr = OAPI_SYSINFO_STB_GetChipId(szCHIPID, ADDI_HWVER_MAX_LEN);
	if(hErr == ERR_FAIL)
	{
		RemLOG_Error("OAPI_SYSINFO_STB_GetChipId is FAIL!!!\n");
	}
	
	// For XMPP server
	HxSTD_sprintf(pszOutput, "http://%s:%d/connreq/%s", /*s_stNetInfo.ipAddr*/XMPP_SERVER, CONNREQ_PORT, szCHIPID);
#else
	// External IP already judged TR157_module_Set_NetInfo.
	HxSTD_sprintf(pszOutput, "http://%s%s", s_stNetInfo.ipAddr, ":9090/connreq");
#endif

	REM_LEAVE(hErr);
}

static HERROR get_data_ManagementServer_ConnectionRequestUsername(HCHAR *pszOutput)
{
	HERROR hErr = ERR_OK;

	CHECK_NULL(pszOutput);
	hErr = rema_DM_getData_ManagementServer_ConnectionRequestUsername(pszOutput);

	REM_LEAVE(hErr);
}

static HERROR get_data_ManagementServer_ConnectionRequestPassword(HCHAR *pszOutput)
{
	HERROR hErr = ERR_OK;

	CHECK_NULL(pszOutput);
	hErr = rema_DM_getData_ManagementServer_ConnectionRequestPassword(pszOutput);

	REM_LEAVE(hErr);
}

static HERROR get_data_ManagementServer_UpgradesManaged(HCHAR *pszOutput)
{
	HERROR hErr = ERR_OK;

	CHECK_NULL(pszOutput);
	hErr = rema_DM_getData_ManagementServer_UpgradesManaged(pszOutput);

	REM_LEAVE(hErr);
}

static HERROR get_data_LAN_AddressingType(HCHAR *pszOutput)
{
	REM_FUNC_ENTER;

	if(s_stNetInfo.dhcp)
		HxSTD_PrintToStr(pszOutput,"%s", "DHCP");
	else
		HxSTD_PrintToStr(pszOutput,"%s", "Static");

	REM_FUNC_LEAVE;

	return ERR_OK;
}

static HERROR get_data_LAN_IPAddress(HCHAR *pszOutput)
{
	REM_FUNC_ENTER;
//	printf("#####[HKJEONG][%s] ipAddr(%s)\n",__FUNCTION__,s_stNetInfo.ipAddr);
	if (s_stNetInfo.ipAddr != 0)
	{
		HxSTD_PrintToStr(pszOutput, "%s", s_stNetInfo.ipAddr);
	}
	REM_FUNC_LEAVE;
	return ERR_OK;
}

static HERROR get_data_LAN_SubnetMask(HCHAR *pszOutput)
{
	REM_FUNC_ENTER;

	if (s_stNetInfo.netmask != 0)
	{
		HxSTD_PrintToStr(pszOutput,"%s", s_stNetInfo.netmask);
	}
	REM_FUNC_LEAVE;
	return ERR_OK;
}

static HERROR get_data_LAN_DefaultGateway(HCHAR *pszOutput)
{
	REM_FUNC_ENTER;

	if (s_stNetInfo.gateway != 0)
	{
		HxSTD_PrintToStr(pszOutput,"%s",s_stNetInfo.gateway);
	}
	REM_FUNC_LEAVE;

	return ERR_OK;

}

static HERROR get_data_LAN_DNSServers(HCHAR *pszOutput)
{
	REM_FUNC_ENTER;

	if (s_stNetInfo.dns1 != 0)
	{
		HxSTD_PrintToStr(pszOutput, "%s",s_stNetInfo.dns1);
	}
	REM_FUNC_LEAVE;
	return ERR_OK;
}

static HERROR get_data_LAN_MACAddress(HCHAR *pszOutput)
{
	REM_FUNC_ENTER;

	if (s_stNetInfo.macaddr != 0)
	{
		HxSTD_PrintToStr(pszOutput, "%s",s_stNetInfo.macaddr);
	}
	REM_FUNC_LEAVE;
	return ERR_OK;

}

static HERROR get_data_AudienceMeasurement_CombinedAccess(HCHAR *pszOutput)
{
	HERROR hErr = ERR_OK;

	CHECK_NULL(pszOutput);
	hErr = rema_DM_getData_AudienceMeasurement_CombinedAccess(pszOutput);

	REM_LEAVE(hErr);
}

static HERROR get_data_AudienceMeasurement_PeriodicInformationInterval(HCHAR *pszOutput)
{
	HERROR hErr = ERR_OK;

	CHECK_NULL(pszOutput);
	hErr = rema_DM_getData_AudienceMeasurement_PeriodicInformationInterval(pszOutput);

	REM_LEAVE(hErr);
}

static HERROR get_data_AudienceMeasurement_PeriodicInformationTime(HCHAR *pszOutput)
{
	HCHAR szSN[32];
	HxDATETIME_Time_t stSNTime;
	UNIXTIME utSNTime;
	HxDATETIME_Date_t stCurrDate;

	UNIXTIME utCurrTime;
	unsigned long	vkErr;

	HERROR hErr = ERR_OK;

	CHECK_NULL(pszOutput);
	hErr = rema_DM_getData_AudienceMeasurement_PeriodicInformationTime(pszOutput);

	if (HxSTD_StrNCmp(pszOutput, DEFAULT_PERIODIC_INFORM_TIME,
		HxSTD_StrLen(DEFAULT_PERIODIC_INFORM_TIME)) == 0)
	{
		RemLOG_Print("PeriodicInformationTime[%s] is default value.\n", pszOutput);

		get_data_DeviceInfo_SerialNumber(szSN);

		RemLOG_Print("PeriodicInformationTime must be updated with SN(\'%s\')\n", szSN);
		RemLOG_Print("by \"The Calculating Algorithm of PeriodicInformationTime in ACS side.\"\n");

		utSNTime = strtol(&szSN[6], NULL, 16);
		HLIB_DATETIME_ConvertUnixTimeToTimeWithOutOffset(utSNTime, &stSNTime);

		RemLOG_Print("Converted time[%02d:%02d:%02d] based on lower 6 dits of SN.\n",
			stSNTime.ucHour, stSNTime.ucMinute, stSNTime.ucSecond);

		vkErr = VK_CLOCK_GetTime((unsigned long*)&utCurrTime);
		if (vkErr != VK_OK)
		{
			RemLOG_Print("error VK_CLOCK_GetTime\n", pszOutput);
			hErr = ERR_FAIL;
			goto done;
		}

		HLIB_DATETIME_ConvertUnixTimeToDate(utCurrTime, &stCurrDate);

		RemLOG_Print("Current Date is \'%04d-%02d-%02d\'\n", stCurrDate.usYear, stCurrDate.ucMonth, stCurrDate.ucDay);

		HxSTD_snprintf(pszOutput, 256, "%04d-%02d-%02dT%02d:%02d:%02d.000000Z",
			stCurrDate.usYear, stCurrDate.ucMonth, stCurrDate.ucDay,
			stSNTime.ucHour, stSNTime.ucMinute, stSNTime.ucSecond);

		RemLOG_Print("Updated PeriodicInformationTime is \'%s\'\n", pszOutput);

		// Set PeriodicInformationTime with new updated one
		rema_DM_setData_AudienceMeasurement_PeriodicInformationTime(pszOutput);
	}

done:
	REM_LEAVE(hErr);
}

static HERROR get_data_AudienceMeasurement_PermissionLevel(HCHAR *pszOutput)
{
	HERROR hErr = ERR_OK;

	CHECK_NULL(pszOutput);
	hErr = rema_DM_getData_AudienceMeasurement_PermissionLevel(pszOutput);

	REM_LEAVE(hErr);
}

static HERROR get_SmartCardReaderNumberOfEntries(HINT32 *pszOutput)
{
	HERROR hErr = ERR_OK;
#if 0
	eTRXDMInstancetype_e eType = eTR157A3_Device_SmartCardReaderNumberOfEntries;

	RemLOG_Print("%s(%d Line)\r\n",__func__,__LINE__);

	CHECK_NULL(pszOutput);

	APK_3RDPARTY_CWMP_TR157_Get_CommonDM_Entry(pszOutput,eType);
#else
	REM_LEAVE(ERR_FAIL);
#endif
	REM_LEAVE(hErr);

}
static HERROR get_DevInfo_VendorConfigFileNumberOfEntries(HINT32 *pszOutput)
{
	HERROR hErr = ERR_OK;
	HCHAR szSN[32];
	HCHAR szDestFile[] = "STBConfig.tar";
	HUINT8	*pszShellCmd = NULL;
	HCHAR 	szfullpath[TRPORT_DB_FULLPATH_LEN];

	RemLOG_Print("%s(%d Line)\r\n",__func__,__LINE__);

	CHECK_NULL(pszOutput);

	#if 1

	*pszOutput = 1;


	{
		/////////////////////////////////////////////
		// File Name Rule : STBMAC_YYYYMMDDHHMMSS.dat
		/////////////////////////////////////////////


		pszShellCmd = HLIB_STD_MemAlloc(1024);
		if(pszShellCmd == NULL)
		{
			return HxCMD_ERR;
		}
		szSN[0]='\0';

		//MAC 사용해야함
		get_data_DeviceInfo_SerialNumber(szSN);

		HxSTD_MemSet(pszShellCmd, 0x00, 1024);

		HxSTD_snprintf(pszShellCmd, 1024, "tar -cf tmp/%s /%s*",  szDestFile, STBCONFIG_FILE_NAME);
		RemLOG_Print("CMD : %s\r\n",pszShellCmd);
		VK_SYSTEM_Command(pszShellCmd);

		HxSTD_snprintf(pszShellCmd, 1024, "mv /tmp/%s /tmp/%s.dat",	szDestFile, szSN);
		RemLOG_Print("CMD : %s\r\n",pszShellCmd);
		VK_SYSTEM_Command(pszShellCmd);

		HxSTD_snprintf(szfullpath, TRPORT_DB_PATH_LEN, "tmp/%s.dat",szSN);

		HLIB_STD_MemFree(pszShellCmd);
	}

	#else
	APK_3RDPARTY_CWMP_TR157_Get_CommonDM_Entry(pszOutput,eType);

	//rema_DM_MultiInstance_SetEntry(pszOutput,eType);
	#endif


	REM_LEAVE(hErr);

}

static HERROR get_DevInfo_ProvisioningCode(HCHAR *pszOutput)
{
	HERROR hErr = ERR_OK;
	RemLOG_Print("%s(%d Line)\r\n",__func__,__LINE__);

	CHECK_NULL(pszOutput);

	#if 1
	pszOutput[0]='\0';
	#else
	APK_3RDPARTY_CWMP_TR157_Get_CommonDM_String(pszOutput,eTR157A3_Device_DeviceInfo_ProvisioningCode);
	#endif

	REM_LEAVE(hErr);

}
static HERROR get_DevInfo_UpTime(HCHAR *pszOutput)
{
	HERROR hErr = ERR_OK;
	RemLOG_Print("%s(%d Line)\r\n",__func__,__LINE__);

	CHECK_NULL(pszOutput);

#if 1
	pszOutput[0]='\0';;
#else
	APK_3RDPARTY_CWMP_TR157_Get_CommonDM_String(pszOutput,eTR157A3_Device_DLNA_Capabilities_AudioClassProfileID);
#endif

	REM_LEAVE(hErr);

}
static HERROR get_DevInfo_FirstUseDate(HCHAR *pszOutput)
{
	HERROR hErr = ERR_OK;
	RemLOG_Print("%s(%d Line)\r\n",__func__,__LINE__);

	CHECK_NULL(pszOutput);

	//CPE
	HxSTD_StrCpy(pszOutput, "0000-01-01T00:00:00Z");


	REM_LEAVE(hErr);

}
static HERROR get_DevInfo_DeviceLog(HCHAR *pszOutput)
{
	HERROR hErr = ERR_OK;
	RemLOG_Print("%s(%d Line)\r\n",__func__,__LINE__);

	CHECK_NULL(pszOutput);

	#if 1
	pszOutput[0]='\0';
	#else
	pszOutput[0]='\0';
	//뭘 올려주면 되는거지? Vendor Specific Log....?
	//APK_3RDPARTY_CWMP_TR157_Get_CommonDM_String(pszOutput,eTR157A3_Device_DeviceInfo_DeviceLog);
	#endif


	REM_LEAVE(hErr);

}
#if 0
static HERROR get_DevInfo_SupportedDataModelNumberOfEntries(HINT32 *pszOutput)
{
	HERROR hErr = ERR_OK;
	RemLOG_Print("%s(%d Line)\r\n",__func__,__LINE__);

	CHECK_NULL(pszOutput);

	//CPE // DB
	*pszOutput = 1;
	//APK_3RDPARTY_CWMP_TR157_Get_CommonDM_Entry(pszOutput,eTR157A3_Device_DeviceInfo_SupportedDataModelNumberOfEntries);


	REM_LEAVE(hErr);

}
#endif
static HERROR get_DevInfo_ProcessorNumberOfEntries(HINT32 *pszOutput)
{
	HERROR hErr = ERR_OK;
#if defined(TEMP_NOT_USED)	
	eTRXDMInstancetype_e eType = eTR157A3_Device_DeviceInfo_ProcessorNumberOfEntries;
#endif

	RemLOG_Print("%s(%d Line)\r\n",__func__,__LINE__);

	CHECK_NULL(pszOutput);
#if 1
	*pszOutput = 1;
#else
	APK_3RDPARTY_CWMP_TR157_Get_CommonDM_Entry(pszOutput,eType);
#endif

	REM_LEAVE(hErr);
}
static HERROR get_DevInfo_VendorLogFileNumberOfEntries(HINT32 *pszOutput)
{
	HERROR hErr = ERR_OK;
#if defined(TEMP_NOT_USED)	
	eTRXDMInstancetype_e eType = eTR157A3_Device_DeviceInfo_VendorLogFileNumberOfEntries;
#endif

	RemLOG_Print("%s(%d Line)\r\n",__func__,__LINE__);

	CHECK_NULL(pszOutput);

#if 1
	*pszOutput = 0;
#else
	APK_3RDPARTY_CWMP_TR157_Get_CommonDM_Entry(pszOutput,eType);
#endif

	REM_LEAVE(hErr);

}
static HERROR get_DevInfo_MemoryStatus_Total(HCHAR *pszOutput)
{
	HERROR hErr = ERR_OK;
	RemLOG_Print("%s(%d Line)\r\n",__func__,__LINE__);

	CHECK_NULL(pszOutput);

	#if 1
	HxSTD_StrCpy(pszOutput, "0");
	#else
	APK_3RDPARTY_CWMP_TR157_Get_CommonDM_String(pszOutput,eTR157A3_Device_DeviceInfo_MemoryStatus_Total);
	#endif


	REM_LEAVE(hErr);

}
static HERROR get_DevInfo_MemoryStatus_Free(HCHAR *pszOutput)
{
	HERROR hErr = ERR_OK;
	RemLOG_Print("%s(%d Line)\r\n",__func__,__LINE__);

	CHECK_NULL(pszOutput);

	#if 1
	HxSTD_StrCpy(pszOutput, "0");
	#else
	APK_3RDPARTY_CWMP_TR157_Get_CommonDM_String(pszOutput,eTR157A3_Device_DeviceInfo_MemoryStatus_Free);
	#endif


	REM_LEAVE(hErr);

}
static HERROR get_DevInfo_ProcessStatus_CPUUsage(HINT32 *pszOutput)
{
	HERROR hErr = ERR_OK;
	HUINT32	cpuUsage = 0;
	
	RemLOG_Print("%s(%d Line)\r\n",__func__,__LINE__);
	CHECK_NULL(pszOutput);

	hErr = RMOD_DI_GetCPUUsage(&cpuUsage);
	*pszOutput = cpuUsage;
	
	REM_LEAVE(hErr);

}
static HERROR get_DevInfo_ProcessStatus_ProcessNumberOfEntries(HINT32 *pszOutput)
{
	HERROR hErr = ERR_OK;

	RemLOG_Print("%s(%d Line)\r\n",__func__,__LINE__);

	CHECK_NULL(pszOutput);

	hErr = RMOD_DI_GetProcessStatus_Max(pszOutput);

	RemLOG_Print("\033[01;35m[%s:%s:%d]  process num : %d \033[00m\n", __FILE__, __FUNCTION__, __LINE__, *pszOutput);

	REM_LEAVE(hErr);
}

static HERROR get_DevInfo_ProcessStatus_Process_M_PID(HUINT32 *pszOutput)
{
	HERROR hErr = ERR_OK;
	HINT32 nArg1 	= 0;
	HUINT32	nPID	= 0;

	RemLOG_Print("%s(%d Line)\r\n",__func__,__LINE__);

	CHECK_NULL(pszOutput);

	hErr = rema_DM_MultiInstance_GetEntryCount(eTR157A3_Device_DeviceInfo_ProcessStatus_ProcessNumberOfEntries, &nArg1);

	if(hErr == ERR_FAIL)
	{
		RemLOG_Error("rema_DM_MultiInstance_GetEntryCount is failed!!!\n");
		goto END_FUNC;
	}

	hErr = RMOD_DI_GetProcessStatus_Pid(nArg1, &nPID);
	if(hErr == ERR_FAIL)
	{
		RemLOG_Error("RMOD_DI_GetProcessStatus_Pid is failed!!!\n");
		goto END_FUNC;
	}

	*pszOutput = nPID;
	
END_FUNC:
	REM_LEAVE(hErr);

}
static HERROR get_DevInfo_ProcessStatus_Process_M_Command(HCHAR *pszOutput)
{
	HERROR hErr = ERR_OK;
	HINT32 nArg1 = 0;
	HCHAR  szBuf[STRING_LEN_256] = {0, };

	RemLOG_Print("%s(%d Line)\r\n",__func__,__LINE__);

	CHECK_NULL(pszOutput);

	hErr = rema_DM_MultiInstance_GetEntryCount(eTR157A3_Device_DeviceInfo_ProcessStatus_ProcessNumberOfEntries, &nArg1);
	if(hErr == ERR_FAIL)
	{
		RemLOG_Error("rema_DM_MultiInstance_GetEntryCount is failed!!!\n");
		goto END_FUNC;
	}

	hErr = RMOD_DI_GetProcessStatus_Command(nArg1, szBuf, STRING_LEN_256);
	if(hErr == ERR_FAIL)
	{
		RemLOG_Error("RMOD_DI_GetProcessStatus_Command is failed!!!\n");
		goto END_FUNC;
	}

	HxSTD_MemCopy(pszOutput, szBuf, HxSTD_StrLen(szBuf));
	
END_FUNC:
	REM_LEAVE(hErr);
	
}
static HERROR get_DevInfo_ProcessStatus_Process_M_Size(HUINT32 *pszOutput)
{
	HERROR hErr = ERR_OK;
	HINT32 nArg1 = 0;
	HUINT32 nSize = 0;

	RemLOG_Print("%s(%d Line)\r\n",__func__,__LINE__);

	CHECK_NULL(pszOutput);

	hErr = rema_DM_MultiInstance_GetEntryCount(eTR157A3_Device_DeviceInfo_ProcessStatus_ProcessNumberOfEntries, &nArg1);
	if(hErr == ERR_FAIL)
	{
		RemLOG_Error("rema_DM_MultiInstance_GetEntryCount is failed!!!\n");
		goto END_FUNC;
	}

	hErr = RMOD_DI_GetProcessStatus_Size(nArg1, &nSize);
	if(hErr == ERR_FAIL)
	{
		RemLOG_Error("RMOD_DI_GetProcessStatus_Size is failed!!!\n");
		goto END_FUNC;
	}

	*pszOutput = nSize;

END_FUNC:	
	REM_LEAVE(hErr);

}
static HERROR get_DevInfo_ProcessStatus_Process_M_Priority(HUINT32 *pszOutput)
{
	HERROR hErr = ERR_OK;
	HINT32 nArg1 = 0;
	HUINT32 nPriority = 0;

	RemLOG_Print("%s(%d Line)\r\n",__func__,__LINE__);

	CHECK_NULL(pszOutput);

	hErr = rema_DM_MultiInstance_GetEntryCount(eTR157A3_Device_DeviceInfo_ProcessStatus_ProcessNumberOfEntries, &nArg1);
	if(hErr == ERR_FAIL)
	{
		RemLOG_Error("rema_DM_MultiInstance_GetEntryCount is failed!!!\n");
		goto END_FUNC;
	}

	hErr = RMOD_DI_GetProcessStatus_Priority(nArg1, &nPriority);
	
	if(hErr == ERR_FAIL)
	{
		RemLOG_Error("RMOD_DI_GetProcessStatus_Priority is failed!!!\n");
		goto END_FUNC;
	}

	*pszOutput = nPriority;
	
END_FUNC:	
	REM_LEAVE(hErr);

}
static HERROR get_DevInfo_ProcessStatus_Process_M_CPUTime(HUINT32 *pszOutput)
{
	HERROR hErr = ERR_OK;
	HINT32 nArg1 = 0;
	HUINT32 nCPUTime = 0;

	RemLOG_Print("%s(%d Line)\r\n",__func__,__LINE__);

	CHECK_NULL(pszOutput);

	hErr = rema_DM_MultiInstance_GetEntryCount(eTR157A3_Device_DeviceInfo_ProcessStatus_ProcessNumberOfEntries, &nArg1);
	if(hErr == ERR_FAIL)
	{
		RemLOG_Error("rema_DM_MultiInstance_GetEntryCount is failed!!!\n");
		goto END_FUNC;
	}

	hErr = RMOD_DI_GetProcessStatus_CPUTime(nArg1, &nCPUTime);
	if(hErr == ERR_FAIL)
	{
		RemLOG_Error("RMOD_DI_GetProcessStatus_CPUTime is failed!!!\n");
		goto END_FUNC;
	}

	*pszOutput = nCPUTime;
		
END_FUNC:	
	REM_LEAVE(hErr);

}
static HERROR get_DevInfo_ProcessStatus_Process_M_State(HCHAR *pszOutput)
{
	HERROR hErr = ERR_OK;
	HINT32 nArg1 = 0;
	HCHAR	szBuf[STRING_LEN_32] = {0, };

	RemLOG_Print("%s(%d Line)\r\n",__func__,__LINE__);

	CHECK_NULL(pszOutput);

	hErr = rema_DM_MultiInstance_GetEntryCount(eTR157A3_Device_DeviceInfo_ProcessStatus_ProcessNumberOfEntries, &nArg1);
	if(hErr == ERR_FAIL)
	{
		RemLOG_Error("rema_DM_MultiInstance_GetEntryCount is failed!!!\n");
		goto END_FUNC;
	}

	hErr = RMOD_DI_GetProcessStatus_State(nArg1, szBuf, STRING_LEN_32);
	if(hErr == ERR_FAIL)
	{
		RemLOG_Error("RMOD_DI_GetProcessStatus_State is failed!!!\n");
		goto END_FUNC;
	}

	HxSTD_MemCopy(pszOutput, szBuf, HxSTD_StrLen(szBuf));
		
END_FUNC:
	REM_LEAVE(hErr);

}

static HERROR get_DevInfo_TemperatureStatus_TemperatureSensorNumberOfEntries(HINT32 *pszOutput)
{
	HERROR hErr = ERR_OK;

	RemLOG_Print("%s(%d Line)\r\n",__func__,__LINE__);

	CHECK_NULL(pszOutput);

	hErr = RMOD_DI_GetTemperatureSensor_Max(pszOutput);

	REM_LEAVE(hErr);
}
static HERROR get_DevInfo_TemperatureStatus_TemperatureSensor_M_Value(HUINT32 *pszOutput)
{
	HERROR hErr = ERR_OK;
	HINT32 nArg1 = 0;
	HUINT32 nValue = 0;
	HUINT32 nMaxValue = 0;
 
	RemLOG_Print("%s(%d Line)\r\n",__func__,__LINE__);
 
	CHECK_NULL(pszOutput);
 
	hErr = rema_DM_MultiInstance_GetEntryCount(eTR157A3_Device_DeviceInfo_TemperatureStatus_TemperatureSensorNumberOfEntries, &nArg1);
	if(hErr == ERR_FAIL)
	{
		RemLOG_Error("rema_DM_MultiInstance_GetEntryCount is failed!!!\n");
		goto END_FUNC;
	}
 
	hErr = OAPI_SYSINFO_STB_GetCPUTemperature(&nValue, &nMaxValue);
	if(hErr == ERR_FAIL)
	{
		RemLOG_Error("OAPI_SYSINFO_STB_GetCPUTemperature is failed!!!\n");
		goto END_FUNC;
	}
 
	*pszOutput = nValue;
		 
 END_FUNC:	 
	REM_LEAVE(hErr);
}
static HERROR get_DevInfo_TemperatureStatus_TemperatureSensor_M_MaxValue(HUINT32 *pszOutput)
{
	HERROR hErr = ERR_OK;
	HINT32 nArg1 = 0;
	HUINT32 nValue = 0;	
	HUINT32 nMaxValue = 0;
  
	RemLOG_Print("%s(%d Line)\r\n",__func__,__LINE__);
  
	CHECK_NULL(pszOutput);
  
	hErr = rema_DM_MultiInstance_GetEntryCount(eTR157A3_Device_DeviceInfo_TemperatureStatus_TemperatureSensorNumberOfEntries, &nArg1);
	if(hErr == ERR_FAIL)
	{
		RemLOG_Error("rema_DM_MultiInstance_GetEntryCount is failed!!!\n");
		goto END_FUNC;
	}
  
	hErr = OAPI_SYSINFO_STB_GetCPUTemperature(&nValue, &nMaxValue);
	if(hErr == ERR_FAIL)
	{
		RemLOG_Error("RMOD_DI_GetTemperatureStatus_MaxValue is failed!!!\n");
		goto END_FUNC;
	}
  
	*pszOutput = nMaxValue;
		  
END_FUNC:   
	REM_LEAVE(hErr);
}


 // TODO: Index DB 필요함
static HERROR get_DevInfo_VendorConfigFile_M_Name(HCHAR *pszOutput)
{
	HERROR hErr = ERR_OK;
	HINT32 nArg1 = 0;

	RemLOG_Print("%s(%d Line)\r\n",__func__,__LINE__);

	CHECK_NULL(pszOutput);

	rema_DM_MultiInstance_GetEntryCount(eTR157A3_Device_DeviceInfo_VendorConfigFileNumberOfEntries,&nArg1);

#if 1
	pszOutput[0]='\0';
#else
	APK_3RDPARTY_CWMP_TR157_Get_CommonDM_Multi_D1(pszOutput,eTR157A3_Device_DeviceInfo_VendorConfigFile_M_Name,nArg1);
#endif

	REM_LEAVE(hErr);
}
static HERROR get_DevInfo_VendorConfigFile_M_Version(HCHAR *pszOutput)
{
	HERROR hErr = ERR_OK;
	HINT32 nArg1 = 0;

	RemLOG_Print("%s(%d Line)\r\n",__func__,__LINE__);

	CHECK_NULL(pszOutput);

	rema_DM_MultiInstance_GetEntryCount(eTR157A3_Device_DeviceInfo_VendorConfigFileNumberOfEntries,&nArg1);

#if 1
	pszOutput[0]='\0';
#else
	APK_3RDPARTY_CWMP_TR157_Get_CommonDM_Multi_D1(pszOutput,eTR157A3_Device_DeviceInfo_VendorConfigFile_M_Version,nArg1);
#endif

	REM_LEAVE(hErr);
}
static HERROR get_DevInfo_VendorConfigFile_M_Date(HCHAR *pszOutput)
{
	HERROR hErr = ERR_OK;

#if defined(TEMP_NOT_USED)	
	UNIXTIME currentTime = 0;
	HxDATETIME_t dateTime;
#endif	

	RemLOG_Print("%s(%d Line)\r\n",__func__,__LINE__);

	CHECK_NULL(pszOutput);

#if 1
	HxSTD_StrCpy(pszOutput, "0001-01-01T00:00:00Z");
#else

	//Config File 생성 시, Time값 취득.
	TR069_GetVendorConfigFileGenTime(&currentTime);

	if(currentTime == 0)
	{
		HxSTD_StrNCpy(pszOutput, "0000-00-00T00:00:00Z",(SIZE_256-1));
	}
	else
	{
		HxSTD_MemSet(&dateTime, 0x00, sizeof(HxDATETIME_t));

		HLIB_DATETIME_ConvertUnixTimeToDateTime(currentTime, &dateTime);

		HxSTD_PrintToStrN(pszOutput,SIZE_256,"%d-%02d-%02dT%02d:%02d:%02dZ",
								dateTime.stDate.usYear, dateTime.stDate.ucMonth, dateTime.stDate.ucDay,
								dateTime.stTime.ucHour,dateTime.stTime.ucMinute,dateTime.stTime.ucSecond);
	}
#endif


	REM_LEAVE(hErr);

}
static HERROR get_DevInfo_VendorConfigFile_M_Description(HCHAR *pszOutput)
{
	HERROR hErr = ERR_OK;
	HINT32 nArg1 = 0;

	RemLOG_Print("%s(%d Line)\r\n",__func__,__LINE__);

	CHECK_NULL(pszOutput);

	rema_DM_MultiInstance_GetEntryCount(eTR157A3_Device_DeviceInfo_VendorConfigFileNumberOfEntries,&nArg1);

#if 1
	pszOutput[0]='\0';
#else
	APK_3RDPARTY_CWMP_TR157_Get_CommonDM_Multi_D1(pszOutput,eTR157A3_Device_DeviceInfo_VendorConfigFile_M_Description,nArg1);
#endif

	REM_LEAVE(hErr);

}
static HERROR get_DevInfo_Processor_M_Architecture(HCHAR *pszOutput)
{
	HERROR hErr = ERR_OK;

	RemLOG_Print("%s(%d Line)\r\n",__func__,__LINE__);

	CHECK_NULL(pszOutput);

	#if 1//def 1
	HxSTD_StrCpy(pszOutput, "mipsel (little-endian)");
	#else
	rema_DM_MultiInstance_GetEntryCount(eTR157A3_Device_DeviceInfo_ProcessorNumberOfEntries,&nArg1);

	APK_3RDPARTY_CWMP_TR157_Get_CommonDM_Multi_D1(pszOutput,eTR157A3_Device_DeviceInfo_Processor_M_Architecture,nArg1);
	#endif


	REM_LEAVE(hErr);

}
static HERROR get_DevInfo_VendorLogFile_M_Name(HCHAR *pszOutput)
{
	HERROR hErr = ERR_OK;

	#if 1
	pszOutput[0]='\0';
	#else
	HINT32 nArg1 = 0;
	HCHAR szFileName[SIZE_64];
	HCHAR szFullName[SIZE_64];
	HCHAR szSN[32];
	UNIXTIME currentTime = 0;
	HxDATETIME_T dateTime;


	RemLOG_Print("%s(%d Line)\r\n",__func__,__LINE__);

	CHECK_NULL(pszOutput);
	szFileName[0]='\0';
	szFullName[0]='\0';
	szSN[0]='\0';

	rema_DM_MultiInstance_GetEntryCount(eTR157A3_Device_DeviceInfo_VendorLogFileNumberOfEntries,&nArg1);
	//APK_3RDPARTY_CWMP_TR157_Get_CommonDM_Multi_D1(szFileName,eTR157A3_Device_DeviceInfo_VendorLogFile_M_Name,nArg1);

	TRP_Get_DeviceInfo_SerialNumber(szSN);

	nArg1 = (nArg1 + ENTRY_OFFSET);

	switch(nArg1)
	{
		case UPLOAD_TYPE_VENDOR_LOG_Linux:
			TR069_GetVendorLogFileLinuxGenTime(&currentTime);
			HxSTD_PrintToStrN(szFileName,SIZE_64,"%s_Linux",szSN);
			break;
		case UPLOAD_TYPE_VENDOR_LOG_DOCSIS:
			TR069_GetVendorLogFileDOCSISGenTime(&currentTime);
			HxSTD_PrintToStrN(szFileName,SIZE_64,"%s_DOCSIS",szSN);
			break;
		case UPLOAD_TYPE_VENDOR_LOG_Apps:
			TR069_GetVendorLogFileAppsGenTime(&currentTime);
			HxSTD_PrintToStrN(szFileName,SIZE_64,"%s_Apps",szSN);
			break;
		default:
			RemLOG_Print("Please check Vendorlog File Index\n");
			break;

	}

	if(currentTime == 0)
	{
		VK_CLOCK_GetTime((unsigned long *)&currentTime);
	}

	HxSTD_MemSet(&dateTime, 0x00, sizeof(HxDATETIME_T));
	HxDateTime_ConvertUnixTimeToDateTime(currentTime, &dateTime);

	HxSTD_PrintToStrN(szFullName,SIZE_64,"%s_%d%02d%02d%02d%02d%02d.log",szFileName,
							dateTime.stDate.usYear, dateTime.stDate.ucMonth, dateTime.stDate.ucDay,
							dateTime.stTime.ucHour,dateTime.stTime.ucMinute,dateTime.stTime.ucSecond);

	RemLOG_Print("VendorLogFile(%d), Pre-Name(%s),szFullName(%s)\n",nArg1,szFileName,szFullName);

	HxSTD_StrNCpy(pszOutput, szFullName,(SIZE_64-1));
	#endif


	REM_LEAVE(hErr);

}
static HERROR get_DevInfo_VendorLogFile_M_MaximumSize(HCHAR *pszOutput)
{
	HERROR hErr = ERR_OK;
	HINT32 nArg1 = 0;

	RemLOG_Print("%s(%d Line)\r\n",__func__,__LINE__);

	CHECK_NULL(pszOutput);

	rema_DM_MultiInstance_GetEntryCount(eTR157A3_Device_DeviceInfo_VendorLogFileNumberOfEntries,&nArg1);

#if 1
	HxSTD_StrCpy(pszOutput, "0");
#else

	//APK_3RDPARTY_CWMP_TR157_Get_CommonDM_Multi_D1(pszOutput,eTR157A3_Device_DeviceInfo_VendorLogFile_M_MaximumSize,nArg1);

	nArg1 = (nArg1 + ENTRY_OFFSET);

	// Size is Byte
	switch(nArg1)
	{
		case UPLOAD_TYPE_VENDOR_LOG_Linux:
			HxSTD_StrNCpy(pszOutput,"35",(SIZE_16-1));
			break;
		case UPLOAD_TYPE_VENDOR_LOG_DOCSIS:
			HxSTD_StrNCpy(pszOutput,"35",(SIZE_16-1));
			break;
		case UPLOAD_TYPE_VENDOR_LOG_Apps:
			HxSTD_StrNCpy(pszOutput,"16777216",(SIZE_16-1));
			break;
		default:
			RemLOG_Print("Please check Vendorlog File Index\n");
			break;
	}

#endif

	REM_LEAVE(hErr);
}
static HERROR get_DevInfo_VendorLogFile_M_Persistent(HCHAR *pszOutput)
{
	HERROR hErr = ERR_OK;
	HINT32 nArg1 = 0;

	RemLOG_Print("%s(%d Line)\r\n",__func__,__LINE__);

	CHECK_NULL(pszOutput);

	rema_DM_MultiInstance_GetEntryCount(eTR157A3_Device_DeviceInfo_VendorLogFileNumberOfEntries,&nArg1);	

#if 1
	HxSTD_StrCpy(pszOutput, "false");
#else
	APK_3RDPARTY_CWMP_TR157_Get_CommonDM_Multi_D1(pszOutput,eTR157A3_Device_DeviceInfo_VendorLogFile_M_Persistent,nArg1);
#endif


	REM_LEAVE(hErr);

}
#if 0
static HERROR get_MgmtServer_DefaultActiveNotificationThrottle(HCHAR *pszOutput)
{
	HERROR hErr = ERR_OK;
	RemLOG_Print("%s(%d Line)\r\n",__func__,__LINE__);

	CHECK_NULL(pszOutput);

	//CPE


	REM_LEAVE(hErr);

}
static HERROR get_MgmtServer_CWMPRetryMinimumWaitInterval(HCHAR *pszOutput)
{
	HERROR hErr = ERR_OK;
	RemLOG_Print("%s(%d Line)\r\n",__func__,__LINE__);

	CHECK_NULL(pszOutput);

	//CPE


	REM_LEAVE(hErr);

}
static HERROR get_MgmtServer_CWMPRetryIntervalMultiplier(HCHAR *pszOutput)
{
	HERROR hErr = ERR_OK;
	RemLOG_Print("%s(%d Line)\r\n",__func__,__LINE__);

	CHECK_NULL(pszOutput);

	//CPE


	REM_LEAVE(hErr);

}
static HERROR get_MgmtServer_AutonomousTxCompletePolicy_Enable(HCHAR *pszOutput)
{
	HERROR hErr = ERR_OK;
	RemLOG_Print("%s(%d Line)\r\n",__func__,__LINE__);

	CHECK_NULL(pszOutput);

	HxSTD_StrCpy(pszOutput, "Ready"); //APK_3RDPARTY_CWMP_TR157_Get_CommonDM_String(pszOutput,eTR157A3_Device_MgmtSvr_AutonomousTransferCompletePolicy_Enable);


	REM_LEAVE(hErr);

}
static HERROR get_MgmtServer_AutonomousTxCompletePolicy_TxTypeFilter(HCHAR *pszOutput)
{
	HERROR hErr = ERR_OK;
	RemLOG_Print("%s(%d Line)\r\n",__func__,__LINE__);

	CHECK_NULL(pszOutput);

	HxSTD_StrCpy(pszOutput, "Ready"); //APK_3RDPARTY_CWMP_TR157_Get_CommonDM_String(pszOutput,eTR157A3_Device_MgmtSvr_AutonomousTransferCompletePolicy_TransferTypeFilter);


	REM_LEAVE(hErr);

}
static HERROR get_MgmtServer_AutonomousTxCompletePolicy_FileTypeFilter(HCHAR *pszOutput)
{
	HERROR hErr = ERR_OK;
	RemLOG_Print("%s(%d Line)\r\n",__func__,__LINE__);

	CHECK_NULL(pszOutput);

	HxSTD_StrCpy(pszOutput, "Ready"); //APK_3RDPARTY_CWMP_TR157_Get_CommonDM_String(pszOutput,eTR157A3_Device_MgmtSvr_AutonomousTransferCompletePolicy_FileTypeFilter);


	REM_LEAVE(hErr);

}
#endif
static HERROR get_LAN_Stats_ConnectionUpTime(HCHAR *pszOutput)
{
	HERROR hErr = ERR_OK;

	RemLOG_Print("%s(%d Line)\r\n",__func__,__LINE__);

	CHECK_NULL(pszOutput);

	#if 1
	HxSTD_StrCpy(pszOutput,"0");
	#else
	APK_3RDPARTY_CWMP_TR157_Get_CommonDM_String(pszOutput,eTR157A3_Device_LAN_Stats_ConnectionUpTime);
	#endif


	REM_LEAVE(hErr);

}
static HERROR get_LAN_Stats_TotalBytesSent(HCHAR *pszOutput)
{
	HERROR hErr = ERR_OK;
	RemLOG_Print("%s(%d Line)\r\n",__func__,__LINE__);

	CHECK_NULL(pszOutput);

	#if 1
	HxSTD_StrCpy(pszOutput,"0");
	#else
	APK_3RDPARTY_CWMP_TR157_Get_CommonDM_String(pszOutput,eTR157A3_Device_LAN_Stats_TotalBytesSent);
	#endif


	REM_LEAVE(hErr);

}
static HERROR get_LAN_Stats_TotalBytesReceived(HCHAR *pszOutput)
{
	HERROR hErr = ERR_OK;
	RemLOG_Print("%s(%d Line)\r\n",__func__,__LINE__);

	CHECK_NULL(pszOutput);

	#if 1
	HxSTD_StrCpy(pszOutput,"0");
	#else
	APK_3RDPARTY_CWMP_TR157_Get_CommonDM_String(pszOutput,eTR157A3_Device_LAN_Stats_TotalBytesReceived);
	#endif


	REM_LEAVE(hErr);

}
static HERROR get_LAN_Stats_TotalPacketsSent(HCHAR *pszOutput)
{
	HERROR hErr = ERR_OK;
	RemLOG_Print("%s(%d Line)\r\n",__func__,__LINE__);

	CHECK_NULL(pszOutput);

	#if 1
	HxSTD_StrCpy(pszOutput,"0");
	#else
	APK_3RDPARTY_CWMP_TR157_Get_CommonDM_String(pszOutput,eTR157A3_Device_LAN_Stats_TotalPacketsSent);
	#endif


	REM_LEAVE(hErr);

}
static HERROR get_LAN_Stats_TotalPacketsReceived(HCHAR *pszOutput)
{
	HERROR hErr = ERR_OK;
	RemLOG_Print("%s(%d Line)\r\n",__func__,__LINE__);

	CHECK_NULL(pszOutput);

	#if 1
	HxSTD_StrCpy(pszOutput,"0");
	#else
	APK_3RDPARTY_CWMP_TR157_Get_CommonDM_String(pszOutput,eTR157A3_Device_LAN_Stats_TotalPacketsReceived);
	#endif


	REM_LEAVE(hErr);

}
static HERROR get_LAN_Stats_CurrentDayInterval(HCHAR *pszOutput)
{
	HERROR hErr = ERR_OK;
	RemLOG_Print("%s(%d Line)\r\n",__func__,__LINE__);

	CHECK_NULL(pszOutput);

	#if 1
	HxSTD_StrCpy(pszOutput,"0");
	#else
	APK_3RDPARTY_CWMP_TR157_Get_CommonDM_String(pszOutput,eTR157A3_Device_LAN_Stats_CurrentDayInterval);
	#endif


	REM_LEAVE(hErr);

}
static HERROR get_LAN_Stats_CurrentDayBytesSent(HCHAR *pszOutput)
{
	HERROR hErr = ERR_OK;
	RemLOG_Print("%s(%d Line)\r\n",__func__,__LINE__);

	CHECK_NULL(pszOutput);

	#if 1
	HxSTD_StrCpy(pszOutput,"0");
	#else
	APK_3RDPARTY_CWMP_TR157_Get_CommonDM_String(pszOutput,eTR157A3_Device_LAN_Stats_CurrentDayBytesSent);

	#endif

	REM_LEAVE(hErr);

}
static HERROR get_LAN_Stats_CurrentDayBytesReceived(HCHAR *pszOutput)
{
	HERROR hErr = ERR_OK;
	RemLOG_Print("%s(%d Line)\r\n",__func__,__LINE__);

	CHECK_NULL(pszOutput);

	#if 1
	HxSTD_StrCpy(pszOutput,"0");
	#else
	APK_3RDPARTY_CWMP_TR157_Get_CommonDM_String(pszOutput,eTR157A3_Device_LAN_Stats_CurrentDayBytesReceived);
	#endif


	REM_LEAVE(hErr);

}
static HERROR get_LAN_Stats_CurrentDayPacketsSent(HCHAR *pszOutput)
{
	HERROR hErr = ERR_OK;
	RemLOG_Print("%s(%d Line)\r\n",__func__,__LINE__);

	CHECK_NULL(pszOutput);

	#if 1
	HxSTD_StrCpy(pszOutput,"0");
	#else
	APK_3RDPARTY_CWMP_TR157_Get_CommonDM_String(pszOutput,eTR157A3_Device_LAN_Stats_CurrentDayPacketsSent);
	#endif


	REM_LEAVE(hErr);

}
static HERROR get_LAN_Stats_CurrentDayPacketsReceived(HCHAR *pszOutput)
{
	HERROR hErr = ERR_OK;
	RemLOG_Print("%s(%d Line)\r\n",__func__,__LINE__);

	CHECK_NULL(pszOutput);

	#if 1
	HxSTD_StrCpy(pszOutput,"0");
	#else
	APK_3RDPARTY_CWMP_TR157_Get_CommonDM_String(pszOutput,eTR157A3_Device_LAN_Stats_CurrentDayPacketsReceived);
	#endif


	REM_LEAVE(hErr);

}
static HERROR get_LAN_Stats_QuarterHourInterval(HCHAR *pszOutput)
{
	HERROR hErr = ERR_OK;
	RemLOG_Print("%s(%d Line)\r\n",__func__,__LINE__);

	CHECK_NULL(pszOutput);

	#if 1
	HxSTD_StrCpy(pszOutput,"0");
	#else
	APK_3RDPARTY_CWMP_TR157_Get_CommonDM_String(pszOutput,eTR157A3_Device_LAN_Stats_QuarterHourInterval);
	#endif


	REM_LEAVE(hErr);

}
static HERROR get_LAN_Stats_QuarterHourBytesSent(HCHAR *pszOutput)
{
	HERROR hErr = ERR_OK;
	RemLOG_Print("%s(%d Line)\r\n",__func__,__LINE__);

	CHECK_NULL(pszOutput);

	#if 1
	HxSTD_StrCpy(pszOutput,"0");
	#else
	APK_3RDPARTY_CWMP_TR157_Get_CommonDM_String(pszOutput,eTR157A3_Device_LAN_Stats_QuarterHourBytesSent);
	#endif


	REM_LEAVE(hErr);

}
static HERROR get_LAN_Stats_QuarterHourBytesReceived(HCHAR *pszOutput)
{
	HERROR hErr = ERR_OK;
	RemLOG_Print("%s(%d Line)\r\n",__func__,__LINE__);

	CHECK_NULL(pszOutput);

	#if 1
	HxSTD_StrCpy(pszOutput,"0");
	#else
	APK_3RDPARTY_CWMP_TR157_Get_CommonDM_String(pszOutput,eTR157A3_Device_LAN_Stats_QuarterHourBytesReceived);
	#endif


	REM_LEAVE(hErr);

}
static HERROR get_LAN_Stats_QuarterHourPacketsSent(HCHAR *pszOutput)
{
	HERROR hErr = ERR_OK;
	RemLOG_Print("%s(%d Line)\r\n",__func__,__LINE__);

	CHECK_NULL(pszOutput);

	#if 1
	HxSTD_StrCpy(pszOutput,"0");
	#else
	APK_3RDPARTY_CWMP_TR157_Get_CommonDM_String(pszOutput,eTR157A3_Device_LAN_Stats_QuarterHourPacketsSent);
	#endif


	REM_LEAVE(hErr);

}
static HERROR get_LAN_Stats_QuarterHourPacketsReceived(HCHAR *pszOutput)
{
	HERROR hErr = ERR_OK;
	RemLOG_Print("%s(%d Line)\r\n",__func__,__LINE__);

	CHECK_NULL(pszOutput);

	#if 1
	HxSTD_StrCpy(pszOutput,"0");
	#else
	APK_3RDPARTY_CWMP_TR157_Get_CommonDM_String(pszOutput,eTR157A3_Device_LAN_Stats_QuarterHourPacketsReceived);
	#endif


	REM_LEAVE(hErr);

}
static HERROR get_UPnP_Device_Enable(HCHAR *pszOutput)
{
	HERROR	hErr = ERR_OK;
#if 0
	HBOOL	bDMSEnabled = FALSE;
	HBOOL	bDMREnabled = FALSE;
	RemLOG_Print("%s(%d Line)\r\n",__func__,__LINE__);

	CHECK_NULL(pszOutput);

	APK_DLNADMS_GetDmsDbEnabled(&bDMSEnabled);
	APK_DLNADMR_GetDmrDbEnabled(&bDMREnabled);
	if (bDMSEnabled == TRUE || bDMREnabled == TRUE)
	{
		HxSTD_StrNCpy(pszOutput, "true",(SIZE_16-1));
	}
	else
	{
		HxSTD_StrNCpy(pszOutput, "false",(SIZE_16-1));
	}

	REM_LEAVE(hErr);
#else
	HxSTD_StrNCpy(pszOutput, "false",(SIZE_16-1));
	REM_LEAVE(hErr);
#endif
}
static HERROR get_UPnP_Device_UPnPMediaServer(HCHAR *pszOutput)
{
	HERROR	hErr = ERR_OK;

	RemLOG_Print("%s(%d Line)\r\n",__func__,__LINE__);

	CHECK_NULL(pszOutput);

	TRP135_Get_JCOM_Config_HomeServer_Enable(pszOutput);

	REM_LEAVE(hErr);

}
static HERROR get_UPnP_Device_UPnPMediaRenderer(HCHAR *pszOutput)
{
	HERROR	hErr = ERR_OK;
#if 0
	HBOOL	bEnabled = FALSE;
	RemLOG_Print("%s(%d Line)\r\n",__func__,__LINE__);

	CHECK_NULL(pszOutput);

	APK_DLNADMR_GetDmrDbEnabled(&bEnabled);
	if (bEnabled == TRUE)
	{
		HxSTD_StrNCpy(pszOutput, "true",(SIZE_16-1));
	}
	else
	{
		HxSTD_StrNCpy(pszOutput, "false",(SIZE_16-1));
	}

	REM_LEAVE(hErr);
#else
	HxSTD_StrNCpy(pszOutput, "false",(SIZE_16-1));
	REM_LEAVE(hErr);
#endif
}
static HERROR get_UPnP_Device_UPnPWLANAccessPoint(HCHAR *pszOutput)
{
	HERROR hErr = ERR_OK;
	RemLOG_Print("%s(%d Line)\r\n",__func__,__LINE__);

	CHECK_NULL(pszOutput);

	HxSTD_StrNCpy(pszOutput, "false",(SIZE_16-1));

	REM_LEAVE(hErr);

}
static HERROR get_UPnP_Device_UPnPQoSDevice(HCHAR *pszOutput)
{
	HERROR hErr = ERR_OK;
	RemLOG_Print("%s(%d Line)\r\n",__func__,__LINE__);

	CHECK_NULL(pszOutput);

	HxSTD_StrNCpy(pszOutput, "false",(SIZE_16-1));

	REM_LEAVE(hErr);

}
static HERROR get_UPnP_Device_UPnPQoSPolicyHolder(HCHAR *pszOutput)
{
	HERROR hErr = ERR_OK;
	RemLOG_Print("%s(%d Line)\r\n",__func__,__LINE__);

	CHECK_NULL(pszOutput);

	HxSTD_StrNCpy(pszOutput, "false",(SIZE_16-1));

	REM_LEAVE(hErr);

}
static HERROR get_UPnP_Device_UPnPIGD(HCHAR *pszOutput)
{
	HERROR hErr = ERR_OK;
	RemLOG_Print("%s(%d Line)\r\n",__func__,__LINE__);

	CHECK_NULL(pszOutput);

	HxSTD_StrNCpy(pszOutput, "false",(SIZE_16-1));

	REM_LEAVE(hErr);

}
static HERROR get_UPnP_Device_Capa_UPnPArchitecture(HCHAR *pszOutput)
{
	HERROR hErr = ERR_OK;
	RemLOG_Print("%s(%d Line)\r\n",__func__,__LINE__);

	CHECK_NULL(pszOutput);

#if 1
	HxSTD_StrCpy(pszOutput, "0");
#else
	APK_3RDPARTY_CWMP_TR157_Get_CommonDM_String(pszOutput,eTR157A3_Device_UPnP_Device_Capabilities_UPnPArchitecture);
#endif
	REM_LEAVE(hErr);

}
static HERROR get_UPnP_Device_Capa_UPnPArchitectureMinorVer(HCHAR *pszOutput)
{
	HERROR hErr = ERR_OK;
	RemLOG_Print("%s(%d Line)\r\n",__func__,__LINE__);

	CHECK_NULL(pszOutput);

#if 1
	HxSTD_StrCpy(pszOutput, "0");
#else
	APK_3RDPARTY_CWMP_TR157_Get_CommonDM_String(pszOutput,eTR157A3_Device_UPnP_Device_Capabilities_UPnPArchitectureMinorVer);
#endif

	REM_LEAVE(hErr);

}
static HERROR get_UPnP_Device_Capa_UPnPMediaServer(HCHAR *pszOutput)
{
	HERROR hErr = ERR_OK;
	RemLOG_Print("%s(%d Line)\r\n",__func__,__LINE__);

	CHECK_NULL(pszOutput);

#if 1
	HxSTD_StrCpy(pszOutput, "0");
#else
	APK_3RDPARTY_CWMP_TR157_Get_CommonDM_String(pszOutput,eTR157A3_Device_UPnP_Device_Capabilities_UPnPMediaServer);
#endif

	REM_LEAVE(hErr);

}
static HERROR get_UPnP_Device_Capa_UPnPMediaRenderer(HCHAR *pszOutput)
{
	HERROR hErr = ERR_OK;
	RemLOG_Print("%s(%d Line)\r\n",__func__,__LINE__);

	CHECK_NULL(pszOutput);
#if 1
	HxSTD_StrCpy(pszOutput, "0");
#else
	APK_3RDPARTY_CWMP_TR157_Get_CommonDM_String(pszOutput,eTR157A3_Device_UPnP_Device_Capabilities_UPnPMediaRenderer);
#endif

	REM_LEAVE(hErr);

}
static HERROR get_UPnP_Device_Capa_UPnPWLANAccessPoint(HCHAR *pszOutput)
{
	HERROR hErr = ERR_OK;
	RemLOG_Print("%s(%d Line)\r\n",__func__,__LINE__);

	CHECK_NULL(pszOutput);

	pszOutput[0]='\0';

	REM_LEAVE(hErr);

}
static HERROR get_UPnP_Device_Capa_UPnPBasicDevice(HCHAR *pszOutput)
{
	HERROR hErr = ERR_OK;
	RemLOG_Print("%s(%d Line)\r\n",__func__,__LINE__);

	CHECK_NULL(pszOutput);

	pszOutput[0]='\0';

	REM_LEAVE(hErr);

}
static HERROR get_UPnP_Device_Capa_UPnPQoSDevice(HCHAR *pszOutput)
{
	HERROR hErr = ERR_OK;
	RemLOG_Print("%s(%d Line)\r\n",__func__,__LINE__);

	CHECK_NULL(pszOutput);

	pszOutput[0]='\0';

	REM_LEAVE(hErr);

}
static HERROR get_UPnP_Device_Capa_UPnPQoSPolicyHolder(HCHAR *pszOutput)
{
	HERROR hErr = ERR_OK;
	RemLOG_Print("%s(%d Line)\r\n",__func__,__LINE__);

	CHECK_NULL(pszOutput);

	pszOutput[0]='\0';

	REM_LEAVE(hErr);

}
static HERROR get_UPnP_Device_Capa_UPnPIGD(HCHAR *pszOutput)
{
	HERROR hErr = ERR_OK;
	RemLOG_Print("%s(%d Line)\r\n",__func__,__LINE__);

	CHECK_NULL(pszOutput);

	pszOutput[0]='\0';

	REM_LEAVE(hErr);

}
static HERROR get_UPnP_Discovery_RootDeviceNumberOfEntries(HINT32 *pszOutput)
{
	HERROR hErr = ERR_OK;
	RemLOG_Print("%s(%d Line)\r\n",__func__,__LINE__);

	CHECK_NULL(pszOutput);

#if 1
	*pszOutput = 0;
#else
	APK_3RDPARTY_CWMP_TR157_Get_CommonDM_Entry(pszOutput,eTR157A3_Device_UPnP_Discovery_RootDeviceNumberOfEntries);
#endif

	REM_LEAVE(hErr);

}
static HERROR get_UPnP_Discovery_DeviceNumberOfEntries(HINT32 *pszOutput)
{
	HERROR hErr = ERR_OK;
	RemLOG_Print("%s(%d Line)\r\n",__func__,__LINE__);

	CHECK_NULL(pszOutput);

#if 1
	*pszOutput = 0;
#else
	APK_3RDPARTY_CWMP_TR157_Get_CommonDM_Entry(pszOutput,eTR157A3_Device_UPnP_Discovery_DeviceNumberOfEntries);
#endif

	REM_LEAVE(hErr);

}
static HERROR get_UPnP_Discovery_ServiceNumberOfEntries(HINT32 *pszOutput)
{
	HERROR hErr = ERR_OK;
	RemLOG_Print("%s(%d Line)\r\n",__func__,__LINE__);

	CHECK_NULL(pszOutput);

	*pszOutput = 0;

	REM_LEAVE(hErr);

}
static HERROR get_UPnP_Discovery_RootDevice_Status(HCHAR *pszOutput)
{
	HERROR hErr = ERR_OK;
	RemLOG_Print("%s(%d Line)\r\n",__func__,__LINE__);

	CHECK_NULL(pszOutput);

	HxSTD_StrNCpy(pszOutput, "LeaseActive",(SIZE_16-1));

	REM_LEAVE(hErr);

}
static HERROR get_UPnP_Discovery_RootDevice_UUID(HCHAR *pszOutput)
{
	HERROR hErr = ERR_OK;
	RemLOG_Print("%s(%d Line)\r\n",__func__,__LINE__);

	CHECK_NULL(pszOutput);

	pszOutput[0]='\0';

	REM_LEAVE(hErr);

}
static HERROR get_UPnP_Discovery_RootDevice_USN(HCHAR *pszOutput)
{
	HERROR hErr = ERR_OK;
	RemLOG_Print("%s(%d Line)\r\n",__func__,__LINE__);

	CHECK_NULL(pszOutput);

	pszOutput[0]='\0';

	REM_LEAVE(hErr);

}
static HERROR get_UPnP_Discovery_RootDevice_LeaseTime(HCHAR *pszOutput)
{
	HERROR hErr = ERR_OK;
	RemLOG_Print("%s(%d Line)\r\n",__func__,__LINE__);

	CHECK_NULL(pszOutput);

	pszOutput[0]='\0';

	REM_LEAVE(hErr);

}
static HERROR get_UPnP_Discovery_RootDevice_Location(HCHAR *pszOutput)
{
	HERROR hErr = ERR_OK;
	RemLOG_Print("%s(%d Line)\r\n",__func__,__LINE__);

	CHECK_NULL(pszOutput);

	pszOutput[0]='\0';

	REM_LEAVE(hErr);

}
static HERROR get_UPnP_Discovery_RootDevice_Server(HCHAR *pszOutput)
{
	HERROR hErr = ERR_OK;
	RemLOG_Print("%s(%d Line)\r\n",__func__,__LINE__);

	CHECK_NULL(pszOutput);

	pszOutput[0]='\0';

	REM_LEAVE(hErr);

}
static HERROR get_UPnP_Discovery_Device_Status(HCHAR *pszOutput)
{
	HERROR hErr = ERR_OK;
	RemLOG_Print("%s(%d Line)\r\n",__func__,__LINE__);

	CHECK_NULL(pszOutput);

	pszOutput[0]='\0';

	REM_LEAVE(hErr);

}
static HERROR get_UPnP_Discovery_Device_UUID(HCHAR *pszOutput)
{
	HERROR hErr = ERR_OK;
	RemLOG_Print("%s(%d Line)\r\n",__func__,__LINE__);

	CHECK_NULL(pszOutput);

	HxSTD_StrNCpy(pszOutput, "LeaseActive",(SIZE_16-1));

	REM_LEAVE(hErr);

}
static HERROR get_UPnP_Discovery_Device_USN(HCHAR *pszOutput)
{
	HERROR hErr = ERR_OK;
	RemLOG_Print("%s(%d Line)\r\n",__func__,__LINE__);

	CHECK_NULL(pszOutput);

	pszOutput[0]='\0';

	REM_LEAVE(hErr);

}
static HERROR get_UPnP_Discovery_Device_LeaseTime(HCHAR *pszOutput)
{
	HERROR hErr = ERR_OK;
	RemLOG_Print("%s(%d Line)\r\n",__func__,__LINE__);

	CHECK_NULL(pszOutput);

	pszOutput[0]='\0';

	REM_LEAVE(hErr);

}
static HERROR get_UPnP_Discovery_Device_Location(HCHAR *pszOutput)
{
	HERROR hErr = ERR_OK;
	RemLOG_Print("%s(%d Line)\r\n",__func__,__LINE__);

	CHECK_NULL(pszOutput);

	pszOutput[0]='\0';

	REM_LEAVE(hErr);

}
static HERROR get_UPnP_Discovery_Device_Server(HCHAR *pszOutput)
{
	HERROR hErr = ERR_OK;
	RemLOG_Print("%s(%d Line)\r\n",__func__,__LINE__);

	CHECK_NULL(pszOutput);

	pszOutput[0]='\0';

	REM_LEAVE(hErr);

}
static HERROR get_UPnP_Discovery_Service_Status(HCHAR *pszOutput)
{
	HERROR hErr = ERR_OK;
	RemLOG_Print("%s(%d Line)\r\n",__func__,__LINE__);

	CHECK_NULL(pszOutput);

	HxSTD_StrNCpy(pszOutput, "LeaseActive",(SIZE_16-1));

	REM_LEAVE(hErr);

}
static HERROR get_UPnP_Discovery_Service_USN(HCHAR *pszOutput)
{
	HERROR hErr = ERR_OK;
	RemLOG_Print("%s(%d Line)\r\n",__func__,__LINE__);

	CHECK_NULL(pszOutput);

	pszOutput[0]='\0';

	REM_LEAVE(hErr);

}
static HERROR get_UPnP_Discovery_Service_LeaseTime(HCHAR *pszOutput)
{
	HERROR hErr = ERR_OK;
	RemLOG_Print("%s(%d Line)\r\n",__func__,__LINE__);

	CHECK_NULL(pszOutput);

	pszOutput[0]='\0';

	REM_LEAVE(hErr);

}
static HERROR get_UPnP_Discovery_Service_Location(HCHAR *pszOutput)
{
	HERROR hErr = ERR_OK;
	RemLOG_Print("%s(%d Line)\r\n",__func__,__LINE__);

	CHECK_NULL(pszOutput);

	pszOutput[0]='\0';

	REM_LEAVE(hErr);

}
static HERROR get_UPnP_Discovery_Service_Server(HCHAR *pszOutput)
{
	HERROR hErr = ERR_OK;
	RemLOG_Print("%s(%d Line)\r\n",__func__,__LINE__);

	CHECK_NULL(pszOutput);

	pszOutput[0]='\0';

	REM_LEAVE(hErr);

}
static HERROR get_DLNA_Capabilities_HNDDeviceClass(HCHAR *pszOutput)
{
	HERROR hErr = ERR_OK;
	RemLOG_Print("%s(%d Line)\r\n",__func__,__LINE__);

	CHECK_NULL(pszOutput);

	#if 1
	HxSTD_StrCpy(pszOutput, "Deviceclass1");
	#else
	APK_3RDPARTY_CWMP_TR157_Get_CommonDM_String(pszOutput,eTR157A3_Device_DLNA_Capabilities_HNDDeviceClass);
	#endif


	REM_LEAVE(hErr);

}
static HERROR get_DLNA_Capabilities_DeviceCapability(HCHAR *pszOutput)
{
	HERROR hErr = ERR_OK;
	RemLOG_Print("%s(%d Line)\r\n",__func__,__LINE__);

	CHECK_NULL(pszOutput);

	#if 1
	HxSTD_StrCpy(pszOutput, "DMS");
	#else
	APK_3RDPARTY_CWMP_TR157_Get_CommonDM_String(pszOutput,eTR157A3_Device_DLNA_Capabilities_DeviceCapability);
	#endif


	REM_LEAVE(hErr);

}
static HERROR get_DLNA_Capabilities_HIDDeviceClass(HCHAR *pszOutput)
{
	HERROR hErr = ERR_OK;
	RemLOG_Print("%s(%d Line)\r\n",__func__,__LINE__);

	CHECK_NULL(pszOutput);
#if 1
	pszOutput[0]='\0';
#else
	APK_3RDPARTY_CWMP_TR157_Get_CommonDM_String(pszOutput,eTR157A3_Device_DLNA_Capabilities_HIDDeviceClass);
#endif

	REM_LEAVE(hErr);
}
static HERROR get_DLNA_Capabilities_ImageClassProfileID(HCHAR *pszOutput)
{
	HERROR hErr = ERR_OK;
	RemLOG_Print("%s(%d Line)\r\n",__func__,__LINE__);

	CHECK_NULL(pszOutput);
#if 1
	pszOutput[0]='\0';
#else
	APK_3RDPARTY_CWMP_TR157_Get_CommonDM_String(pszOutput,eTR157A3_Device_DLNA_Capabilities_ImageClassProfileID);
#endif

	REM_LEAVE(hErr);
}
static HERROR get_DLNA_Capabilities_AudioClassProfileID(HCHAR *pszOutput)
{
	HERROR hErr = ERR_OK;
	RemLOG_Print("%s(%d Line)\r\n",__func__,__LINE__);

	CHECK_NULL(pszOutput);

#if 1
	pszOutput[0]='\0';
#else
	APK_3RDPARTY_CWMP_TR157_Get_CommonDM_String(pszOutput,eTR157A3_Device_DLNA_Capabilities_AudioClassProfileID);
#endif

	REM_LEAVE(hErr);
}
static HERROR get_DLNA_Capabilities_AVClassProfileID(HCHAR *pszOutput)
{
	HERROR hErr = ERR_OK;
	RemLOG_Print("%s(%d Line)\r\n",__func__,__LINE__);

	CHECK_NULL(pszOutput);

#if 1
	HxSTD_StrCpy(pszOutput, "JPEG_TN");
#else
	APK_3RDPARTY_CWMP_TR157_Get_CommonDM_String(pszOutput,eTR157A3_Device_DLNA_Capabilities_AVClassProfileID);
#endif

	REM_LEAVE(hErr);
}
static HERROR get_DLNA_Capabilities_MediaCollectionProfileID(HCHAR *pszOutput)
{
	HERROR hErr = ERR_OK;
	RemLOG_Print("%s(%d Line)\r\n",__func__,__LINE__);

	CHECK_NULL(pszOutput);

	//Not Supported
	pszOutput[0]='\0';

	REM_LEAVE(hErr);
}
static HERROR get_DLNA_Capabilities_PrinterClassProfileID(HCHAR *pszOutput)
{
	HERROR hErr = ERR_OK;
	RemLOG_Print("%s(%d Line)\r\n",__func__,__LINE__);

	CHECK_NULL(pszOutput);

	//Not Supported
	pszOutput[0]='\0';

	REM_LEAVE(hErr);
}
static HERROR get_SmartCardReader_M_Enable(HCHAR *pszOutput)
{
	HERROR hErr = ERR_OK;
	RemLOG_Print("%s(%d Line)\r\n",__func__,__LINE__);

	CHECK_NULL(pszOutput);

	//Not Supported
	HxSTD_StrNCpy(pszOutput, "false",(SIZE_16-1));

	REM_LEAVE(hErr);
}
static HERROR get_SmartCardReader_M_Status(HCHAR *pszOutput)
{
	HERROR hErr = ERR_OK;
	HINT32	nArg1 = 0;
	RemLOG_Print("%s(%d Line)\r\n",__func__,__LINE__);

	CHECK_NULL(pszOutput);

	rema_DM_MultiInstance_GetEntryCount(eTR157A3_Device_SmartCardReaderNumberOfEntries,&nArg1);	

#if 1
	HxSTD_StrCpy(pszOutput, "Enabled");
#else
	APK_3RDPARTY_CWMP_TR157_Get_CommonDM_Multi_D1(pszOutput,eTR157A3_Device_SmartCardReader_M_Status,nArg1);
#endif

	REM_LEAVE(hErr);
}
static HERROR get_SmartCardReader_M_Name(HCHAR *pszOutput)
{
	HERROR hErr = ERR_OK;
	HINT32	nArg1 = 0;
	RemLOG_Print("%s(%d Line)\r\n",__func__,__LINE__);

	CHECK_NULL(pszOutput);

	rema_DM_MultiInstance_GetEntryCount(eTR157A3_Device_SmartCardReaderNumberOfEntries,&nArg1);	

#if 1
	pszOutput[0]='\0';
#else
	APK_3RDPARTY_CWMP_TR157_Get_CommonDM_Multi_D1(pszOutput,eTR157A3_Device_SmartCardReader_M_Name,nArg1);
#endif


	REM_LEAVE(hErr);

}
static HERROR get_SmartCardReader_M_Reset(HCHAR *pszOutput)
{
	HERROR hErr = ERR_OK;
	HINT32	nArg1 = 0;
	RemLOG_Print("%s(%d Line)\r\n",__func__,__LINE__);

	CHECK_NULL(pszOutput);

	rema_DM_MultiInstance_GetEntryCount(eTR157A3_Device_SmartCardReaderNumberOfEntries,&nArg1);	

#if 1
	HxSTD_StrCpy(pszOutput, "false");
#else
	APK_3RDPARTY_CWMP_TR157_Get_CommonDM_Multi_D1(pszOutput,eTR157A3_Device_SmartCardReader_M_Reset,nArg1);
#endif


	REM_LEAVE(hErr);

}
static HERROR get_SmartCardReader_M_ResetTime(HCHAR *pszOutput)
{
	HERROR hErr = ERR_OK;
	HINT32	nArg1 = 0;
	RemLOG_Print("%s(%d Line)\r\n",__func__,__LINE__);

	CHECK_NULL(pszOutput);

	rema_DM_MultiInstance_GetEntryCount(eTR157A3_Device_SmartCardReaderNumberOfEntries,&nArg1);

#if 1
	HxSTD_StrCpy(pszOutput, "0000-01-01T00:00:00Z");
#else
	APK_3RDPARTY_CWMP_TR157_Get_CommonDM_Multi_D1(pszOutput,eTR157A3_Device_SmartCardReader_M_ResetTime,nArg1);
#endif

	REM_LEAVE(hErr);

}
static HERROR get_SmartCardReader_M_SmartCard_Status(HCHAR *pszOutput)
{
	HERROR hErr = ERR_OK;
	HINT32	nArg1 = 0;
	RemLOG_Print("%s(%d Line)\r\n",__func__,__LINE__);

	CHECK_NULL(pszOutput);

	rema_DM_MultiInstance_GetEntryCount(eTR157A3_Device_SmartCardReaderNumberOfEntries,&nArg1);	

#if 1
	HxSTD_StrCpy(pszOutput, "None");
#else
	APK_3RDPARTY_CWMP_TR157_Get_CommonDM_Multi_D1(pszOutput,eTR157A3_Device_SmartCardReader_M_SmartCard_Status,nArg1);
#endif

	REM_LEAVE(hErr);
}
static HERROR get_SmartCardReader_M_SmartCard_Type(HCHAR *pszOutput)
{
	HERROR hErr = ERR_OK;
	HINT32	nArg1 = 0;
	RemLOG_Print("%s(%d Line)\r\n",__func__,__LINE__);

	CHECK_NULL(pszOutput);

	rema_DM_MultiInstance_GetEntryCount(eTR157A3_Device_SmartCardReaderNumberOfEntries,&nArg1);	

#if 1
	HxSTD_StrCpy(pszOutput, "CA");
#else
	APK_3RDPARTY_CWMP_TR157_Get_CommonDM_Multi_D1(pszOutput,eTR157A3_Device_SmartCardReader_M_SmartCard_Type,nArg1);
#endif

	REM_LEAVE(hErr);

}
static HERROR get_SmartCardReader_M_SmartCard_Application(HCHAR *pszOutput)
{
	HERROR hErr = ERR_OK;
	HINT32	nArg1 = 0;
	RemLOG_Print("%s(%d Line)\r\n",__func__,__LINE__);

	CHECK_NULL(pszOutput);

	rema_DM_MultiInstance_GetEntryCount(eTR157A3_Device_SmartCardReaderNumberOfEntries,&nArg1);

#if 1
	pszOutput[0]='\0';
#else
	APK_3RDPARTY_CWMP_TR157_Get_CommonDM_Multi_D1(pszOutput,eTR157A3_Device_SmartCardReader_M_SmartCard_Application,nArg1);
#endif

	REM_LEAVE(hErr);

}
static HERROR get_SmartCardReader_M_SmartCard_SerialNumber(HCHAR *pszOutput)
{
	HERROR hErr = ERR_OK;
	HINT32	nArg1 = 0;
	RemLOG_Print("%s(%d Line)\r\n",__func__,__LINE__);

	CHECK_NULL(pszOutput);

	rema_DM_MultiInstance_GetEntryCount(eTR157A3_Device_SmartCardReaderNumberOfEntries,&nArg1);	

#if 1
	HxSTD_StrCpy(pszOutput, "0");
#else
	APK_3RDPARTY_CWMP_TR157_Get_CommonDM_Multi_D1(pszOutput,eTR157A3_Device_SmartCardReader_M_SmartCard_SerialNumber,nArg1);
#endif

	REM_LEAVE(hErr);

}
static HERROR get_SmartCardReader_M_SmartCard_ATR(HCHAR *pszOutput)
{
	HERROR hErr = ERR_OK;
	HINT32	nArg1 = 0;
	RemLOG_Print("%s(%d Line)\r\n",__func__,__LINE__);

	CHECK_NULL(pszOutput);

	rema_DM_MultiInstance_GetEntryCount(eTR157A3_Device_SmartCardReaderNumberOfEntries,&nArg1);

#if 1
	HxSTD_StrCpy(pszOutput, "0");
#else
	APK_3RDPARTY_CWMP_TR157_Get_CommonDM_Multi_D1(pszOutput,eTR157A3_Device_SmartCardReader_M_SmartCard_ATR,nArg1);
#endif

	REM_LEAVE(hErr);
}
static HERROR get_SmartCardReader_M_SmartCard_X_JCOM_CardType(HCHAR *pszOutput)
{
	HERROR hErr = ERR_OK;
	HINT32	nArg1 = 0;
	RemLOG_Print("%s(%d Line)\r\n",__func__,__LINE__);

	CHECK_NULL(pszOutput);

	rema_DM_MultiInstance_GetEntryCount(eTR157A3_Device_SmartCardReaderNumberOfEntries,&nArg1);

#if 1
	pszOutput[0]='\0';
#else
	APK_3RDPARTY_CWMP_TR157_Get_CommonDM_Multi_D1(pszOutput,eTR157A3_Device_SmartCardReader_M_SmartCard_X_JCOM_CardType,nArg1);
#endif


	REM_LEAVE(hErr);

}
static HERROR get_SmartCardReader_M_SmartCard_X_JCOM_GroupIdNumberOfEntries(HINT32 *pszOutput)
{
	HERROR hErr = ERR_OK;
	HINT32	nArg1 = 0;
#if defined(TEMP_NOT_USED)
	eTRXDMInstancetype_e eType = eTR157A3_Device_SmartCardReader_M_SmartCard_X_JCOM_GroupIdNumberOfEntries;
#endif

	RemLOG_Print("%s(%d Line)\r\n",__func__,__LINE__);

	CHECK_NULL(pszOutput);

	rema_DM_MultiInstance_GetEntryCount(eTR157A3_Device_SmartCardReaderNumberOfEntries,&nArg1);

#if 1
	*pszOutput = 0;
#else
	APK_3RDPARTY_CWMP_TR157_Get_CommonDM_Entry_D1(pszOutput,eType,nArg1);
#endif

	REM_LEAVE(hErr);

}
static HERROR get_SmartCardReader_M_SmartCard_X_JCOM_GroupId_M_Number(HCHAR *pszOutput)
{
	HERROR hErr = ERR_OK;
	HINT32	nArg1 = 0, nArg2 = 0;
	RemLOG_Print("%s(%d Line)\r\n",__func__,__LINE__);

	CHECK_NULL(pszOutput);

	rema_DM_MultiInstance_GetEntryCount(eTR157A3_Device_SmartCardReaderNumberOfEntries,&nArg1);
	rema_DM_MultiInstance_GetEntryCount(eTR157A3_Device_SmartCardReader_M_SmartCard_X_JCOM_GroupIdNumberOfEntries,&nArg2);	

#if 1
	pszOutput[0]='\0';
#else
	APK_3RDPARTY_CWMP_TR157_Get_CommonDM_Multi_D2(pszOutput,eTR157A3_Device_SmartCardReader_M_SmartCard_X_JCOM_GroupId_M_Number, nArg1, nArg2);
#endif

	REM_LEAVE(hErr);

}
static HERROR get_SelfTestDiagnostics_DiagnosticsState(HCHAR *pszOutput)
{
	HERROR hErr = ERR_OK;
	RemLOG_Print("%s(%d Line)\r\n",__func__,__LINE__);

	CHECK_NULL(pszOutput);

#if 1
	HxSTD_StrCpy(pszOutput, "None");
#else
	APK_3RDPARTY_CWMP_TR157_Get_CommonDM_String(pszOutput,eTR157A3_Device_SelfTestDiagnostics_DiagnosticsState);
#endif

	REM_LEAVE(hErr);
}
static HERROR get_SelfTestDiagnostics_Results(HCHAR *pszOutput)
{
	HERROR hErr = ERR_OK;
	RemLOG_Print("%s(%d Line)\r\n",__func__,__LINE__);

	CHECK_NULL(pszOutput);
#if 1
	pszOutput[0]='\0';
#else
	APK_3RDPARTY_CWMP_TR157_Get_CommonDM_String(pszOutput,eTR157A3_Device_SelfTestDiagnostics_Results);
#endif

	REM_LEAVE(hErr);

}
static HERROR get_USBHosts_HostNumberOfEntries(HINT32 *pszOutput)
{
	HERROR hErr = ERR_OK;
#if defined(TEMP_NOT_USED)	
	eTRXDMInstancetype_e eType = eTR157A3_Device_USBHosts_HostNumberOfEntries;
#endif

	RemLOG_Print("%s(%d Line)\r\n",__func__,__LINE__);

	CHECK_NULL(pszOutput);

#if 1
	*pszOutput = 0;
#else
	APK_3RDPARTY_CWMP_TR157_Get_CommonDM_Entry(pszOutput,eType);
#endif

	REM_LEAVE(hErr);

}
static HERROR get_USBHosts_Host_M_Enable(HCHAR *pszOutput)
{
	HERROR hErr = ERR_OK;
	RemLOG_Print("%s(%d Line)\r\n",__func__,__LINE__);

	CHECK_NULL(pszOutput);

	HxSTD_StrNCpy(pszOutput, "true",(SIZE_16-1));

	REM_LEAVE(hErr);
}
static HERROR get_USBHosts_Host_M_Name(HCHAR *pszOutput)
{
	HERROR hErr = ERR_OK;
	HINT32	nArg1 = 0;
	RemLOG_Print("%s(%d Line)\r\n",__func__,__LINE__);

	CHECK_NULL(pszOutput);

	rema_DM_MultiInstance_GetEntryCount(eTR157A3_Device_USBHosts_HostNumberOfEntries,&nArg1);

#if 1
	pszOutput[0]='\0';
#else
	APK_3RDPARTY_CWMP_TR157_Get_CommonDM_Multi_D1(pszOutput,eTR157A3_Device_USBHosts_Host_M_Name,nArg1);
#endif
	REM_LEAVE(hErr);
}
static HERROR get_USBHosts_Host_M_Type(HCHAR *pszOutput)
{
	HERROR hErr = ERR_OK;
	HINT32	nArg1 = 0;
	RemLOG_Print("%s(%d Line)\r\n",__func__,__LINE__);

	CHECK_NULL(pszOutput);

	rema_DM_MultiInstance_GetEntryCount(eTR157A3_Device_USBHosts_HostNumberOfEntries,&nArg1);

#if 1
	HxSTD_StrCpy(pszOutput, "EHCI");
#else
	APK_3RDPARTY_CWMP_TR157_Get_CommonDM_Multi_D1(pszOutput,eTR157A3_Device_USBHosts_Host_M_Type,nArg1);
#endif

	REM_LEAVE(hErr);
}
static HERROR get_USBHosts_Host_M_Reset(HCHAR *pszOutput)
{
	HERROR hErr = ERR_OK;
	RemLOG_Print("%s(%d Line)\r\n",__func__,__LINE__);

	CHECK_NULL(pszOutput);

	HxSTD_StrNCpy(pszOutput, "false",(SIZE_16-1));

	REM_LEAVE(hErr);
}
static HERROR get_USBHosts_Host_M_PowerManagementEnable(HCHAR *pszOutput)
{
	HERROR hErr = ERR_OK;
	RemLOG_Print("%s(%d Line)\r\n",__func__,__LINE__);

	CHECK_NULL(pszOutput);

	HxSTD_StrNCpy(pszOutput, "false",(SIZE_16-1));

	REM_LEAVE(hErr);
}
static HERROR get_USBHosts_Host_M_USBVersion(HCHAR *pszOutput)
{
	HERROR hErr = ERR_OK;
	HINT32	nArg1 = 0;
	RemLOG_Print("%s(%d Line)\r\n",__func__,__LINE__);

	CHECK_NULL(pszOutput);

	rema_DM_MultiInstance_GetEntryCount(eTR157A3_Device_USBHosts_HostNumberOfEntries,&nArg1);
	
#if 1
	HxSTD_StrCpy(pszOutput, "2.0");
#else
	APK_3RDPARTY_CWMP_TR157_Get_CommonDM_Multi_D1(pszOutput,eTR157A3_Device_USBHosts_Host_M_USBVersion,nArg1);
#endif

	REM_LEAVE(hErr);

}
static HERROR get_USBHosts_Host_M_DeviceNumberOfEntries(HINT32 *pszOutput)
{
	HERROR hErr = ERR_OK;
	HINT32	nArg1 = 0;
#if defined(TEMP_NOT_USED)	
	eTRXDMInstancetype_e eType = eTR157A3_Device_USBHosts_Host_M_DeviceNumberOfEntries;
#endif

	RemLOG_Print("%s(%d Line)\r\n",__func__,__LINE__);

	CHECK_NULL(pszOutput);

	rema_DM_MultiInstance_GetEntryCount(eTR157A3_Device_USBHosts_HostNumberOfEntries,&nArg1);
	
#if 1
	*pszOutput = 0;
#else
	APK_3RDPARTY_CWMP_TR157_Get_CommonDM_Entry_D1(pszOutput,eType,nArg1);
#endif

	REM_LEAVE(hErr);

}
static HERROR get_USBHosts_Host_M_Device_M_DeviceNumber(HCHAR *pszOutput)
{
	HERROR hErr = ERR_OK;
	HINT32	nArg1 = 0, nArg2 = 0;
	RemLOG_Print("%s(%d Line)\r\n",__func__,__LINE__);

	CHECK_NULL(pszOutput);

	rema_DM_MultiInstance_GetEntryCount(eTR157A3_Device_USBHosts_HostNumberOfEntries,&nArg1);
	rema_DM_MultiInstance_GetEntryCount(eTR157A3_Device_USBHosts_Host_M_DeviceNumberOfEntries,&nArg2);

#if 1
	HxSTD_StrCpy(pszOutput, "0");
#else
	APK_3RDPARTY_CWMP_TR157_Get_CommonDM_Multi_D2(pszOutput,eTR157A3_DUSBHosts_HDev_M_DeviceNumber,nArg1,nArg2);
#endif
	REM_LEAVE(hErr);
}
static HERROR get_USBHosts_Host_M_Device_M_USBVersion(HCHAR *pszOutput)
{
	HERROR hErr = ERR_OK;
	HINT32	nArg1 = 0, nArg2 = 0;
	RemLOG_Print("%s(%d Line)\r\n",__func__,__LINE__);

	CHECK_NULL(pszOutput);

	rema_DM_MultiInstance_GetEntryCount(eTR157A3_Device_USBHosts_HostNumberOfEntries,&nArg1);
	rema_DM_MultiInstance_GetEntryCount(eTR157A3_Device_USBHosts_Host_M_DeviceNumberOfEntries,&nArg2);
	
#if 1
	pszOutput[0]='\0';
#else
	APK_3RDPARTY_CWMP_TR157_Get_CommonDM_Multi_D2(pszOutput,eTR157A3_DUSBHosts_HDev_M_USBVersion,nArg1,nArg2);
#endif
	REM_LEAVE(hErr);
}
static HERROR get_USBHosts_Host_M_Device_M_DeviceClass(HCHAR *pszOutput)
{
	HERROR hErr = ERR_OK;
	HINT32	nArg1 = 0, nArg2 = 0;
	RemLOG_Print("%s(%d Line)\r\n",__func__,__LINE__);

	CHECK_NULL(pszOutput);

	rema_DM_MultiInstance_GetEntryCount(eTR157A3_Device_USBHosts_HostNumberOfEntries,&nArg1);
	rema_DM_MultiInstance_GetEntryCount(eTR157A3_Device_USBHosts_Host_M_DeviceNumberOfEntries,&nArg2);
	
#if 1
	HxSTD_StrCpy(pszOutput, "0");
#else
	APK_3RDPARTY_CWMP_TR157_Get_CommonDM_Multi_D2(pszOutput,eTR157A3_DUSBHosts_HDev_M_DeviceClass,nArg1,nArg2);
#endif

	REM_LEAVE(hErr);
}
static HERROR get_USBHosts_Host_M_Device_M_DeviceSubClass(HCHAR *pszOutput)
{
	HERROR hErr = ERR_OK;
	HINT32	nArg1 = 0, nArg2 = 0;
	RemLOG_Print("%s(%d Line)\r\n",__func__,__LINE__);

	CHECK_NULL(pszOutput);

	rema_DM_MultiInstance_GetEntryCount(eTR157A3_Device_USBHosts_HostNumberOfEntries,&nArg1);
	rema_DM_MultiInstance_GetEntryCount(eTR157A3_Device_USBHosts_Host_M_DeviceNumberOfEntries,&nArg2);
	
#if 1
	HxSTD_StrCpy(pszOutput, "0");
#else
	APK_3RDPARTY_CWMP_TR157_Get_CommonDM_Multi_D2(pszOutput,eTR157A3_DUSBHosts_HDev_M_DeviceSubClass,nArg1,nArg2);
#endif

	REM_LEAVE(hErr);

}
static HERROR get_USBHosts_Host_M_Device_M_DeviceVersion(HCHAR *pszOutput)
{
	HERROR hErr = ERR_OK;
	HINT32	nArg1 = 0, nArg2 = 0;
	RemLOG_Print("%s(%d Line)\r\n",__func__,__LINE__);

	CHECK_NULL(pszOutput);

	rema_DM_MultiInstance_GetEntryCount(eTR157A3_Device_USBHosts_HostNumberOfEntries,&nArg1);
	rema_DM_MultiInstance_GetEntryCount(eTR157A3_Device_USBHosts_Host_M_DeviceNumberOfEntries,&nArg2);
	
#if 1
	HxSTD_StrCpy(pszOutput, "0");
#else
	APK_3RDPARTY_CWMP_TR157_Get_CommonDM_Multi_D2(pszOutput,eTR157A3_DUSBHosts_HDev_M_DeviceVersion,nArg1,nArg2);
#endif

	REM_LEAVE(hErr);

}
static HERROR get_USBHosts_Host_M_Device_M_DeviceProtocol(HCHAR *pszOutput)
{
	HERROR hErr = ERR_OK;
	HINT32	nArg1 = 0, nArg2 = 0;
	RemLOG_Print("%s(%d Line)\r\n",__func__,__LINE__);

	CHECK_NULL(pszOutput);
	
	rema_DM_MultiInstance_GetEntryCount(eTR157A3_Device_USBHosts_HostNumberOfEntries,&nArg1);
	rema_DM_MultiInstance_GetEntryCount(eTR157A3_Device_USBHosts_Host_M_DeviceNumberOfEntries,&nArg2);

#if 1
	HxSTD_StrCpy(pszOutput, "0");
#else
	APK_3RDPARTY_CWMP_TR157_Get_CommonDM_Multi_D2(pszOutput,eTR157A3_DUSBHosts_HDev_M_DeviceProtocol,nArg1,nArg2);
#endif

	REM_LEAVE(hErr);
}
static HERROR get_USBHosts_Host_M_Device_M_ProductID(HCHAR *pszOutput)
{
	HERROR hErr = ERR_OK;
	HINT32	nArg1 = 0, nArg2 = 0;
	RemLOG_Print("%s(%d Line)\r\n",__func__,__LINE__);

	CHECK_NULL(pszOutput);

	rema_DM_MultiInstance_GetEntryCount(eTR157A3_Device_USBHosts_HostNumberOfEntries,&nArg1);
	rema_DM_MultiInstance_GetEntryCount(eTR157A3_Device_USBHosts_Host_M_DeviceNumberOfEntries,&nArg2);
	
#if 1
	HxSTD_StrCpy(pszOutput, "0");
#else
	APK_3RDPARTY_CWMP_TR157_Get_CommonDM_Multi_D2(pszOutput,eTR157A3_DUSBHosts_HDev_M_ProductID,nArg1,nArg2);
#endif

	REM_LEAVE(hErr);
}
static HERROR get_USBHosts_Host_M_Device_M_VendorID(HCHAR *pszOutput)
{
	HERROR hErr = ERR_OK;
	HINT32	nArg1 = 0, nArg2 = 0;
	RemLOG_Print("%s(%d Line)\r\n",__func__,__LINE__);

	CHECK_NULL(pszOutput);

	rema_DM_MultiInstance_GetEntryCount(eTR157A3_Device_USBHosts_HostNumberOfEntries,&nArg1);
	rema_DM_MultiInstance_GetEntryCount(eTR157A3_Device_USBHosts_Host_M_DeviceNumberOfEntries,&nArg2);
	
#if 1
	HxSTD_StrCpy(pszOutput, "0");
#else
	APK_3RDPARTY_CWMP_TR157_Get_CommonDM_Multi_D2(pszOutput,eTR157A3_DUSBHosts_HDev_M_VendorID,nArg1,nArg2);
#endif

	REM_LEAVE(hErr);
}
static HERROR get_USBHosts_Host_M_Device_M_Manufacturer(HCHAR *pszOutput)
{
	HERROR hErr = ERR_OK;
	HINT32	nArg1 = 0, nArg2 = 0;
	RemLOG_Print("%s(%d Line)\r\n",__func__,__LINE__);

	CHECK_NULL(pszOutput);

	rema_DM_MultiInstance_GetEntryCount(eTR157A3_Device_USBHosts_HostNumberOfEntries,&nArg1);
	rema_DM_MultiInstance_GetEntryCount(eTR157A3_Device_USBHosts_Host_M_DeviceNumberOfEntries,&nArg2);

#if 1
	pszOutput[0]='\0';
#else
	APK_3RDPARTY_CWMP_TR157_Get_CommonDM_Multi_D2(pszOutput,eTR157A3_DUSBHosts_HDev_M_Manufacturer,nArg1,nArg2);
#endif

	REM_LEAVE(hErr);
}
static HERROR get_USBHosts_Host_M_Device_M_ProductClass(HCHAR *pszOutput)
{
	HERROR hErr = ERR_OK;
	HINT32	nArg1 = 0, nArg2 = 0;
	RemLOG_Print("%s(%d Line)\r\n",__func__,__LINE__);

	CHECK_NULL(pszOutput);

	rema_DM_MultiInstance_GetEntryCount(eTR157A3_Device_USBHosts_HostNumberOfEntries,&nArg1);
	rema_DM_MultiInstance_GetEntryCount(eTR157A3_Device_USBHosts_Host_M_DeviceNumberOfEntries,&nArg2);

#if 1
	pszOutput[0]='\0';
#else
	APK_3RDPARTY_CWMP_TR157_Get_CommonDM_Multi_D2(pszOutput,eTR157A3_DUSBHosts_HDev_M_ProductClass,nArg1,nArg2);
#endif

	REM_LEAVE(hErr);
}
static HERROR get_USBHosts_Host_M_Device_M_SerialNumber(HCHAR *pszOutput)
{
	HERROR hErr = ERR_OK;
	HINT32	nArg1 = 0, nArg2 = 0;
	RemLOG_Print("%s(%d Line)\r\n",__func__,__LINE__);

	CHECK_NULL(pszOutput);
	
	rema_DM_MultiInstance_GetEntryCount(eTR157A3_Device_USBHosts_HostNumberOfEntries,&nArg1);
	rema_DM_MultiInstance_GetEntryCount(eTR157A3_Device_USBHosts_Host_M_DeviceNumberOfEntries,&nArg2);

#if 1
	pszOutput[0]='\0';
#else
	APK_3RDPARTY_CWMP_TR157_Get_CommonDM_Multi_D2(pszOutput,eTR157A3_DUSBHosts_HDev_M_SerialNumber,nArg1,nArg2);
#endif

	REM_LEAVE(hErr);
}
static HERROR get_USBHosts_Host_M_Device_M_Port(HCHAR *pszOutput)
{
	HERROR hErr = ERR_OK;
	HINT32	nArg1 = 0, nArg2 = 0;
	RemLOG_Print("%s(%d Line)\r\n",__func__,__LINE__);

	CHECK_NULL(pszOutput);

	rema_DM_MultiInstance_GetEntryCount(eTR157A3_Device_USBHosts_HostNumberOfEntries,&nArg1);
	rema_DM_MultiInstance_GetEntryCount(eTR157A3_Device_USBHosts_Host_M_DeviceNumberOfEntries,&nArg2);

#if 1
	HxSTD_StrCpy(pszOutput, "0");
#else
	APK_3RDPARTY_CWMP_TR157_Get_CommonDM_Multi_D2(pszOutput,eTR157A3_DUSBHosts_HDev_M_Port,nArg1,nArg2);
#endif


	REM_LEAVE(hErr);

}
static HERROR get_USBHosts_Host_M_Device_M_Rate(HCHAR *pszOutput)
{
	HERROR hErr = ERR_OK;
	HINT32	nArg1 = 0, nArg2 = 0;
	RemLOG_Print("%s(%d Line)\r\n",__func__,__LINE__);

	CHECK_NULL(pszOutput);

	rema_DM_MultiInstance_GetEntryCount(eTR157A3_Device_USBHosts_HostNumberOfEntries,&nArg1);
	rema_DM_MultiInstance_GetEntryCount(eTR157A3_Device_USBHosts_Host_M_DeviceNumberOfEntries,&nArg2);

#if 1
	pszOutput[0]='\0';
#else
	APK_3RDPARTY_CWMP_TR157_Get_CommonDM_Multi_D2(pszOutput,eTR157A3_DUSBHosts_HDev_M_Rate,nArg1,nArg2);
#endif

	REM_LEAVE(hErr);

}
static HERROR get_USBHosts_Host_M_Device_M_Parent(HCHAR *pszOutput)
{
	HERROR hErr = ERR_OK;
	HINT32	nArg1 = 0, nArg2 = 0;
	RemLOG_Print("%s(%d Line)\r\n",__func__,__LINE__);

	CHECK_NULL(pszOutput);

	rema_DM_MultiInstance_GetEntryCount(eTR157A3_Device_USBHosts_HostNumberOfEntries,&nArg1);
	rema_DM_MultiInstance_GetEntryCount(eTR157A3_Device_USBHosts_Host_M_DeviceNumberOfEntries,&nArg2);
	
#if 1
	pszOutput[0]='\0';
#else
	APK_3RDPARTY_CWMP_TR157_Get_CommonDM_Multi_D2(pszOutput,eTR157A3_DUSBHosts_HDev_M_Parent,nArg1,nArg2);
#endif

	REM_LEAVE(hErr);

}
static HERROR get_USBHosts_Host_M_Device_M_MaxChildren(HCHAR *pszOutput)
{
	HERROR hErr = ERR_OK;
	HINT32	nArg1 = 0, nArg2 = 0;
	RemLOG_Print("%s(%d Line)\r\n",__func__,__LINE__);

	CHECK_NULL(pszOutput);
	
	rema_DM_MultiInstance_GetEntryCount(eTR157A3_Device_USBHosts_HostNumberOfEntries,&nArg1);
	rema_DM_MultiInstance_GetEntryCount(eTR157A3_Device_USBHosts_Host_M_DeviceNumberOfEntries,&nArg2);

#if 1
	HxSTD_StrCpy(pszOutput, "0");
#else
	APK_3RDPARTY_CWMP_TR157_Get_CommonDM_Multi_D2(pszOutput,eTR157A3_DUSBHosts_HDev_M_MaxChildren,nArg1,nArg2);
#endif

	REM_LEAVE(hErr);
}
static HERROR get_USBHosts_Host_M_Device_M_IsSuspended(HCHAR *pszOutput)
{
	HERROR hErr = ERR_OK;
	HINT32	nArg1 = 0, nArg2 = 0;
	RemLOG_Print("%s(%d Line)\r\n",__func__,__LINE__);

	CHECK_NULL(pszOutput);

	rema_DM_MultiInstance_GetEntryCount(eTR157A3_Device_USBHosts_HostNumberOfEntries,&nArg1);
	rema_DM_MultiInstance_GetEntryCount(eTR157A3_Device_USBHosts_Host_M_DeviceNumberOfEntries,&nArg2);

#if 1
	HxSTD_StrCpy(pszOutput, "false");
#else
	APK_3RDPARTY_CWMP_TR157_Get_CommonDM_Multi_D2(pszOutput,eTR157A3_DUSBHosts_HDev_M_IsSuspended,nArg1,nArg2);
#endif

	REM_LEAVE(hErr);
}
static HERROR get_USBHosts_Host_M_Device_M_IsSelfPowered(HCHAR *pszOutput)
{
	HERROR hErr = ERR_OK;
	HINT32	nArg1 = 0, nArg2 = 0;
	RemLOG_Print("%s(%d Line)\r\n",__func__,__LINE__);

	CHECK_NULL(pszOutput);

	rema_DM_MultiInstance_GetEntryCount(eTR157A3_Device_USBHosts_HostNumberOfEntries,&nArg1);
	rema_DM_MultiInstance_GetEntryCount(eTR157A3_Device_USBHosts_Host_M_DeviceNumberOfEntries,&nArg2);
	
#if 1
	HxSTD_StrCpy(pszOutput, "false");
#else
	APK_3RDPARTY_CWMP_TR157_Get_CommonDM_Multi_D2(pszOutput,eTR157A3_DUSBHosts_HDev_M_IsSelfPowered,nArg1,nArg2);
#endif

	REM_LEAVE(hErr);

}
static HERROR get_USBHosts_Host_M_Device_M_ConfigurationNumberOfEntries(HINT32 *pszOutput)
{
	HERROR hErr = ERR_OK;
	HINT32	nArg1 = 0, nArg2 = 0;
#if defined(TEMP_NOT_USED)	
	eTRXDMInstancetype_e eType = eTR157A3_DUSBHosts_HDev_M_ConfigurationNumberOfEntries;
#endif

	RemLOG_Print("%s(%d Line)\r\n",__func__,__LINE__);

	CHECK_NULL(pszOutput);

	rema_DM_MultiInstance_GetEntryCount(eTR157A3_Device_USBHosts_HostNumberOfEntries,&nArg1);
	rema_DM_MultiInstance_GetEntryCount(eTR157A3_Device_USBHosts_Host_M_DeviceNumberOfEntries,&nArg2);

#if 1
	*pszOutput = 0;
#else
	APK_3RDPARTY_CWMP_TR157_Get_CommonDM_Entry_D2(pszOutput,eType,nArg1,nArg2);
#endif

	REM_LEAVE(hErr);

}
static HERROR get_USBHosts_Host_M_Device_M_Config_M_ConfigurationNumber(HCHAR *pszOutput)
{
	HERROR hErr = ERR_OK;
	HINT32	nArg1 = 0, nArg2 = 0, nArg3 = 0;
	RemLOG_Print("%s(%d Line)\r\n",__func__,__LINE__);

	CHECK_NULL(pszOutput);

	rema_DM_MultiInstance_GetEntryCount(eTR157A3_Device_USBHosts_HostNumberOfEntries,&nArg1);
	rema_DM_MultiInstance_GetEntryCount(eTR157A3_Device_USBHosts_Host_M_DeviceNumberOfEntries,&nArg2);
	rema_DM_MultiInstance_GetEntryCount(eTR157A3_DUSBHosts_HDev_M_ConfigurationNumberOfEntries,&nArg3);

#if 1
	HxSTD_StrCpy(pszOutput, "0");
#else
	APK_3RDPARTY_CWMP_TR157_Get_CommonDM_Multi_D3(pszOutput,eTR157A3_DUSBHosts_HDev_Configuration_M_ConfigurationNumber,nArg1,nArg2,nArg3);
#endif

	REM_LEAVE(hErr);
}
static HERROR get_USBHosts_Host_M_Device_M_Config_M_InterfaceNumberOfEntries(HINT32 *pszOutput)
{
	HERROR hErr = ERR_OK;
	HINT32	nArg1 = 0, nArg2 = 0, nArg3 = 0;
#if defined(TEMP_NOT_USED)	
	eTRXDMInstancetype_e eType = eTR157A3_DUSBHosts_HDev_Configuration_M_InterfaceNumberOfEntries;
#endif

	RemLOG_Print("%s(%d Line)\r\n",__func__,__LINE__);

	CHECK_NULL(pszOutput);

	rema_DM_MultiInstance_GetEntryCount(eTR157A3_Device_USBHosts_HostNumberOfEntries,&nArg1);
	rema_DM_MultiInstance_GetEntryCount(eTR157A3_Device_USBHosts_Host_M_DeviceNumberOfEntries,&nArg2);
	rema_DM_MultiInstance_GetEntryCount(eTR157A3_DUSBHosts_HDev_M_ConfigurationNumberOfEntries,&nArg3);

#if 1
	*pszOutput = 0;
#else
	APK_3RDPARTY_CWMP_TR157_Get_CommonDM_Entry_D3(pszOutput,eType,nArg1,nArg2,nArg3);

	//rema_DM_MultiInstance_SetEntry(pszOutput,eType);
#endif

	REM_LEAVE(hErr);

}
static HERROR get_USBHosts_Host_M_Device_M_Config_M_Interface_M_InterfaceNumber(HCHAR *pszOutput)
{
	HERROR hErr = ERR_OK;
	HINT32	nArg1 = 0, nArg2 = 0, nArg3 = 0;
	RemLOG_Print("%s(%d Line)\r\n",__func__,__LINE__);

	CHECK_NULL(pszOutput);

	rema_DM_MultiInstance_GetEntryCount(eTR157A3_Device_USBHosts_HostNumberOfEntries,&nArg1);
	rema_DM_MultiInstance_GetEntryCount(eTR157A3_Device_USBHosts_Host_M_DeviceNumberOfEntries,&nArg2);
	rema_DM_MultiInstance_GetEntryCount(eTR157A3_DUSBHosts_HDev_M_ConfigurationNumberOfEntries,&nArg3);
	
#if 1
	HxSTD_StrCpy(pszOutput, "0");
#else
	APK_3RDPARTY_CWMP_TR157_Get_CommonDM_Multi_D3(pszOutput,eTR157A3_DUSBHosts_HDev_Configuration_Interface_M_InterfaceNumber,nArg1,nArg2,nArg3);
#endif
	REM_LEAVE(hErr);
}
static HERROR get_USBHosts_Host_M_Device_M_Config_M_Interface_M_InterfaceClass(HCHAR *pszOutput)
{
	HERROR hErr = ERR_OK;
	HINT32	nArg1 = 0, nArg2 = 0, nArg3 = 0;
	RemLOG_Print("%s(%d Line)\r\n",__func__,__LINE__);

	CHECK_NULL(pszOutput);

	rema_DM_MultiInstance_GetEntryCount(eTR157A3_Device_USBHosts_HostNumberOfEntries,&nArg1);
	rema_DM_MultiInstance_GetEntryCount(eTR157A3_Device_USBHosts_Host_M_DeviceNumberOfEntries,&nArg2);
	rema_DM_MultiInstance_GetEntryCount(eTR157A3_DUSBHosts_HDev_M_ConfigurationNumberOfEntries,&nArg3);
	
#if 1
	HxSTD_StrCpy(pszOutput, "0");
#else
	APK_3RDPARTY_CWMP_TR157_Get_CommonDM_Multi_D3(pszOutput,eTR157A3_DUSBHosts_HDev_Configuration_Interface_M_InterfaceClass,nArg1,nArg2,nArg3);
#endif
	REM_LEAVE(hErr);
}
static HERROR get_USBHosts_Host_M_Device_M_Config_M_Interface_M_InterfaceSubClass(HCHAR *pszOutput)
{
	HERROR hErr = ERR_OK;
	HINT32	nArg1 = 0, nArg2 = 0, nArg3 = 0;
	RemLOG_Print("%s(%d Line)\r\n",__func__,__LINE__);

	CHECK_NULL(pszOutput);
	
	rema_DM_MultiInstance_GetEntryCount(eTR157A3_Device_USBHosts_HostNumberOfEntries,&nArg1);
	rema_DM_MultiInstance_GetEntryCount(eTR157A3_Device_USBHosts_Host_M_DeviceNumberOfEntries,&nArg2);
	rema_DM_MultiInstance_GetEntryCount(eTR157A3_DUSBHosts_HDev_M_ConfigurationNumberOfEntries,&nArg3);
	
#if 1
	HxSTD_StrCpy(pszOutput, "0");
#else
	APK_3RDPARTY_CWMP_TR157_Get_CommonDM_Multi_D3(pszOutput,eTR157A3_DUSBHosts_HDev_Configuration_Interface_M_InterfaceSubClass,nArg1,nArg2,nArg3);
#endif
	REM_LEAVE(hErr);
}
static HERROR get_USBHosts_Host_M_Device_M_Config_M_Interface_M_InterfaceProtocol(HCHAR *pszOutput)
{
	HERROR hErr = ERR_OK;
	HINT32	nArg1 = 0, nArg2 = 0, nArg3 = 0;
	RemLOG_Print("%s(%d Line)\r\n",__func__,__LINE__);

	CHECK_NULL(pszOutput);

	rema_DM_MultiInstance_GetEntryCount(eTR157A3_Device_USBHosts_HostNumberOfEntries,&nArg1);
	rema_DM_MultiInstance_GetEntryCount(eTR157A3_Device_USBHosts_Host_M_DeviceNumberOfEntries,&nArg2);
	rema_DM_MultiInstance_GetEntryCount(eTR157A3_DUSBHosts_HDev_M_ConfigurationNumberOfEntries,&nArg3);
	
#if 1
	HxSTD_StrCpy(pszOutput, "0");
#else
	APK_3RDPARTY_CWMP_TR157_Get_CommonDM_Multi_D3(pszOutput,eTR157A3_DUSBHosts_HDev_Configuration_Interface_M_InterfaceProtocol,nArg1,nArg2,nArg3);
#endif

	REM_LEAVE(hErr);
}
static HERROR get_PeriodicStatistics_MinSampleInterval(HCHAR *pszOutput)
{
	HERROR hErr = ERR_OK;
	RemLOG_Print("%s(%d Line)\r\n",__func__,__LINE__);

	CHECK_NULL(pszOutput);
#if 1
	HxSTD_StrCpy(pszOutput, "0");
#else
	APK_3RDPARTY_CWMP_TR157_Get_CommonDM_String(pszOutput,eTR157A3_Device_PeriodicStatistics_MinSampleInterval);
#endif

	REM_LEAVE(hErr);

}
static HERROR get_PeriodicStatistics_MaxReportSamples(HCHAR *pszOutput)
{
	HERROR hErr = ERR_OK;
	RemLOG_Print("%s(%d Line)\r\n",__func__,__LINE__);

	CHECK_NULL(pszOutput);

#if 1
	HxSTD_StrCpy(pszOutput, "0");
#else
	APK_3RDPARTY_CWMP_TR157_Get_CommonDM_String(pszOutput,eTR157A3_Device_PeriodicStatistics_MaxReportSamples);
#endif

	REM_LEAVE(hErr);
}
static HERROR get_PeriodicStatistics_SampleSetNumberOfEntries(HINT32 *pszOutput)
{
	HERROR hErr = ERR_OK;

#if defined(TEMP_NOT_USED)
	eTRXDMInstancetype_e eType = eTR157A3_Device_PeriodicStatistics_SampleSetNumberOfEntries;
#endif

	RemLOG_Print("%s(%d Line)\r\n",__func__,__LINE__);

	CHECK_NULL(pszOutput);

#if 1
	*pszOutput = 0;
#else
	APK_3RDPARTY_CWMP_TR157_Get_CommonDM_Entry(pszOutput,eType);
#endif

	REM_LEAVE(hErr);
}
static HERROR get_PeriodicStatistics_SampleSet_M_Enable(HCHAR *pszOutput)
{
	HERROR hErr = ERR_OK;
	RemLOG_Print("%s(%d Line)\r\n",__func__,__LINE__);

	CHECK_NULL(pszOutput);
#if 1
	HxSTD_StrCpy(pszOutput, "false");
#else
	APK_3RDPARTY_CWMP_TR157_Get_CommonDM_Multi(pszOutput,eTR157A3_Device_PeriodicStatistics_SampleSet_M_Enable);
#endif

	REM_LEAVE(hErr);
}
static HERROR get_PeriodicStatistics_SampleSet_M_Status(HCHAR *pszOutput)
{
	HERROR hErr = ERR_OK;
	RemLOG_Print("%s(%d Line)\r\n",__func__,__LINE__);

	CHECK_NULL(pszOutput);
#if 1
	HxSTD_StrCpy(pszOutput, "Disable");
#else
	APK_3RDPARTY_CWMP_TR157_Get_CommonDM_Multi(pszOutput,eTR157A3_Device_PeriodicStatistics_SampleSet_M_Status);
#endif

	REM_LEAVE(hErr);
}
static HERROR get_PeriodicStatistics_SampleSet_M_Name(HCHAR *pszOutput)
{
	HERROR hErr = ERR_OK;
	RemLOG_Print("%s(%d Line)\r\n",__func__,__LINE__);

	CHECK_NULL(pszOutput);

#if 1
	pszOutput[0]='\0';
#else
	APK_3RDPARTY_CWMP_TR157_Get_CommonDM_Multi(pszOutput,eTR157A3_Device_PeriodicStatistics_SampleSet_M_Name);
#endif

	REM_LEAVE(hErr);
}
static HERROR get_PeriodicStatistics_SampleSet_M_SampleInterval(HCHAR *pszOutput)
{
	HERROR hErr = ERR_OK;
	RemLOG_Print("%s(%d Line)\r\n",__func__,__LINE__);

	CHECK_NULL(pszOutput);

#if 1
	HxSTD_StrCpy(pszOutput, "3600");
#else
	APK_3RDPARTY_CWMP_TR157_Get_CommonDM_Multi(pszOutput,eTR157A3_Device_PeriodicStatistics_SampleSet_M_SampleInterval);
#endif

	REM_LEAVE(hErr);

}
static HERROR get_PeriodicStatistics_SampleSet_M_ReportSamples(HCHAR *pszOutput)
{
	HERROR hErr = ERR_OK;
	RemLOG_Print("%s(%d Line)\r\n",__func__,__LINE__);

	CHECK_NULL(pszOutput);
#if 1
	HxSTD_StrCpy(pszOutput, "0");
#else
	APK_3RDPARTY_CWMP_TR157_Get_CommonDM_Multi(pszOutput,eTR157A3_Device_PeriodicStatistics_SampleSet_M_ReportSamples);
#endif

	REM_LEAVE(hErr);
}
static HERROR get_PeriodicStatistics_SampleSet_M_TimeReference(HCHAR *pszOutput)
{
	HERROR hErr = ERR_OK;
	RemLOG_Print("%s(%d Line)\r\n",__func__,__LINE__);

	CHECK_NULL(pszOutput);
#if 1
	HxSTD_StrCpy(pszOutput, "0001-01-01T00:00:00Z");
#else
	APK_3RDPARTY_CWMP_TR157_Get_CommonDM_Multi(pszOutput,eTR157A3_Device_PeriodicStatistics_SampleSet_M_TimeReference);
#endif

	REM_LEAVE(hErr);
}
static HERROR get_PeriodicStatistics_SampleSet_M_FetchSamples(HCHAR *pszOutput)
{
	HERROR hErr = ERR_OK;
	RemLOG_Print("%s(%d Line)\r\n",__func__,__LINE__);

	CHECK_NULL(pszOutput);
#if 1
	HxSTD_StrCpy(pszOutput, "0");
#else
	APK_3RDPARTY_CWMP_TR157_Get_CommonDM_Multi(pszOutput,eTR157A3_Device_PeriodicStatistics_SampleSet_M_FetchSamples);
#endif

	REM_LEAVE(hErr);
}
static HERROR get_PeriodicStatistics_SampleSet_M_ForceSample(HCHAR *pszOutput)
{
	HERROR hErr = ERR_OK;
	RemLOG_Print("%s(%d Line)\r\n",__func__,__LINE__);

	CHECK_NULL(pszOutput);

#if 1
	HxSTD_StrCpy(pszOutput, "false");
#else
	APK_3RDPARTY_CWMP_TR157_Get_CommonDM_Multi(pszOutput,eTR157A3_Device_PeriodicStatistics_SampleSet_M_ForceSample);
#endif
	REM_LEAVE(hErr);

}
static HERROR get_PeriodicStatistics_SampleSet_M_ReportStartTime(HCHAR *pszOutput)
{
	HERROR hErr = ERR_OK;
	RemLOG_Print("%s(%d Line)\r\n",__func__,__LINE__);

	CHECK_NULL(pszOutput);
#if 1
	HxSTD_StrCpy(pszOutput, "0001-01-01T00:00:00Z");
#else
	APK_3RDPARTY_CWMP_TR157_Get_CommonDM_Multi(pszOutput,eTR157A3_Device_PeriodicStatistics_SampleSet_M_ReportStartTime);
#endif

	REM_LEAVE(hErr);

}
static HERROR get_PeriodicStatistics_SampleSet_M_ReportEndTime(HCHAR *pszOutput)
{
	HERROR hErr = ERR_OK;
	RemLOG_Print("%s(%d Line)\r\n",__func__,__LINE__);

	CHECK_NULL(pszOutput);
#if 1
	HxSTD_StrCpy(pszOutput, "0001-01-01T00:00:00Z");
#else
	APK_3RDPARTY_CWMP_TR157_Get_CommonDM_Multi(pszOutput,eTR157A3_Device_PeriodicStatistics_SampleSet_M_ReportEndTime);
#endif
	REM_LEAVE(hErr);

}
static HERROR get_PeriodicStatistics_SampleSet_M_SampleSeconds(HCHAR *pszOutput)
{
	HERROR hErr = ERR_OK;
	RemLOG_Print("%s(%d Line)\r\n",__func__,__LINE__);

	CHECK_NULL(pszOutput);

#if 1
	pszOutput[0]='\0';
#else
	APK_3RDPARTY_CWMP_TR157_Get_CommonDM_Multi(pszOutput,eTR157A3_Device_PeriodicStatistics_SampleSet_M_SampleSeconds);
#endif

	REM_LEAVE(hErr);

}
static HERROR get_PeriodicStatistics_SampleSet_M_ParameterNumberOfEntries(HINT32 *pszOutput)
{
	HERROR hErr = ERR_OK;
	HINT32	nArg1 = 0;
	eTRXDMInstancetype_e eType = eTR157A3_Device_PeriodicStatistics_SampleSet_M_ParameterNumberOfEntries;
								 
	RemLOG_Print("%s(%d Line)\r\n",__func__,__LINE__);

	CHECK_NULL(pszOutput);

	rema_DM_MultiInstance_GetEntryCount(eType,&nArg1);
	
#if 1
	*pszOutput = 0;
#else
	APK_3RDPARTY_CWMP_TR157_Get_CommonDM_Entry_D1(pszOutput,eType,nArg1);
#endif

	REM_LEAVE(hErr);

}
static HERROR get_PeriodicStatistics_SampleSet_M_Parameter_M_Enable(HCHAR *pszOutput)
{
	HERROR hErr = ERR_OK;
	HINT32	nArg1 = 0, nArg2 = 0;
	RemLOG_Print("%s(%d Line)\r\n",__func__,__LINE__);

	CHECK_NULL(pszOutput);

	rema_DM_MultiInstance_GetEntryCount(eTR157A3_Device_PeriodicStatistics_SampleSetNumberOfEntries,&nArg1);
	rema_DM_MultiInstance_GetEntryCount(eTR157A3_Device_PeriodicStatistics_SampleSet_M_ParameterNumberOfEntries,&nArg2);

#if 1
	HxSTD_StrCpy(pszOutput, "false");
#else
	APK_3RDPARTY_CWMP_TR157_Get_CommonDM_Multi_D2(pszOutput,eTR157A3_Device_PeriodicStatistics_SampleSet_M_Parameter_M_Enable,nArg1,nArg2);
#endif

	REM_LEAVE(hErr);

}
static HERROR get_PeriodicStatistics_SampleSet_M_Parameter_M_Reference(HCHAR *pszOutput)
{
	HERROR hErr = ERR_OK;
	HINT32	nArg1 = 0, nArg2 = 0;
	RemLOG_Print("%s(%d Line)\r\n",__func__,__LINE__);

	CHECK_NULL(pszOutput);

	rema_DM_MultiInstance_GetEntryCount(eTR157A3_Device_PeriodicStatistics_SampleSetNumberOfEntries,&nArg1);
	rema_DM_MultiInstance_GetEntryCount(eTR157A3_Device_PeriodicStatistics_SampleSet_M_ParameterNumberOfEntries,&nArg2);
	
#if 1
	pszOutput[0]='\0';
#else
	APK_3RDPARTY_CWMP_TR157_Get_CommonDM_Multi_D2(pszOutput,eTR157A3_Device_PeriodicStatistics_SampleSet_M_Parameter_M_Reference,nArg1,nArg2);
#endif

	REM_LEAVE(hErr);

}
static HERROR get_PeriodicStatistics_SampleSet_M_Parameter_M_SampleMode(HCHAR *pszOutput)
{
	HERROR hErr = ERR_OK;
	HINT32	nArg1 = 0, nArg2 = 0;
	RemLOG_Print("%s(%d Line)\r\n",__func__,__LINE__);

	CHECK_NULL(pszOutput);

	rema_DM_MultiInstance_GetEntryCount(eTR157A3_Device_PeriodicStatistics_SampleSetNumberOfEntries,&nArg1);
	rema_DM_MultiInstance_GetEntryCount(eTR157A3_Device_PeriodicStatistics_SampleSet_M_ParameterNumberOfEntries,&nArg2);
	
#if 1
	HxSTD_StrCpy(pszOutput, "Current");
#else
	APK_3RDPARTY_CWMP_TR157_Get_CommonDM_Multi_D2(pszOutput,eTR157A3_Device_PeriodicStatistics_SampleSet_M_Parameter_M_SampleMode,nArg1,nArg2);
#endif

	REM_LEAVE(hErr);

}
static HERROR get_PeriodicStatistics_SampleSet_M_Parameter_M_CalculationMode(HCHAR *pszOutput)
{
	HERROR hErr = ERR_OK;
	HINT32	nArg1 = 0, nArg2 = 0;
	RemLOG_Print("%s(%d Line)\r\n",__func__,__LINE__);

	CHECK_NULL(pszOutput);

	rema_DM_MultiInstance_GetEntryCount(eTR157A3_Device_PeriodicStatistics_SampleSetNumberOfEntries,&nArg1);
	rema_DM_MultiInstance_GetEntryCount(eTR157A3_Device_PeriodicStatistics_SampleSet_M_ParameterNumberOfEntries,&nArg2);
	
#if 1
	HxSTD_StrCpy(pszOutput, "Latest");
#else
	APK_3RDPARTY_CWMP_TR157_Get_CommonDM_Multi_D2(pszOutput,eTR157A3_Device_PeriodicStatistics_SampleSet_M_Parameter_M_CalculationMode,nArg1,nArg2);
#endif

	REM_LEAVE(hErr);

}
static HERROR get_PeriodicStatistics_SampleSet_M_Parameter_M_LowThreshold(HCHAR *pszOutput)
{
	HERROR hErr = ERR_OK;
	HINT32	nArg1 = 0, nArg2 = 0;
	RemLOG_Print("%s(%d Line)\r\n",__func__,__LINE__);

	CHECK_NULL(pszOutput);

	rema_DM_MultiInstance_GetEntryCount(eTR157A3_Device_PeriodicStatistics_SampleSetNumberOfEntries,&nArg1);
	rema_DM_MultiInstance_GetEntryCount(eTR157A3_Device_PeriodicStatistics_SampleSet_M_ParameterNumberOfEntries,&nArg2);

#if 1
	HxSTD_StrCpy(pszOutput, "0");
#else
	APK_3RDPARTY_CWMP_TR157_Get_CommonDM_Multi_D2(pszOutput,eTR157A3_Device_PeriodicStatistics_SampleSet_M_Parameter_M_LowThreshold,nArg1,nArg2);
#endif

	REM_LEAVE(hErr);

}
static HERROR get_PeriodicStatistics_SampleSet_M_Parameter_M_HighThreshold(HCHAR *pszOutput)
{
	HERROR hErr = ERR_OK;
	HINT32	nArg1 = 0, nArg2 = 0;
	RemLOG_Print("%s(%d Line)\r\n",__func__,__LINE__);

	CHECK_NULL(pszOutput);

	rema_DM_MultiInstance_GetEntryCount(eTR157A3_Device_PeriodicStatistics_SampleSetNumberOfEntries,&nArg1);
	rema_DM_MultiInstance_GetEntryCount(eTR157A3_Device_PeriodicStatistics_SampleSet_M_ParameterNumberOfEntries,&nArg2);
	
#if 1
	pszOutput[0]='\0';
#else
	APK_3RDPARTY_CWMP_TR157_Get_CommonDM_Multi_D2(pszOutput,eTR157A3_Device_PeriodicStatistics_SampleSet_M_Parameter_M_HighThreshold,nArg1,nArg2);
#endif

	REM_LEAVE(hErr);

}
static HERROR get_PeriodicStatistics_SampleSet_M_Parameter_M_SampleSeconds(HCHAR *pszOutput)
{
	HERROR hErr = ERR_OK;
	HINT32	nArg1 = 0, nArg2 = 0;
	RemLOG_Print("%s(%d Line)\r\n",__func__,__LINE__);

	CHECK_NULL(pszOutput);
	
	rema_DM_MultiInstance_GetEntryCount(eTR157A3_Device_PeriodicStatistics_SampleSetNumberOfEntries,&nArg1);
	rema_DM_MultiInstance_GetEntryCount(eTR157A3_Device_PeriodicStatistics_SampleSet_M_ParameterNumberOfEntries,&nArg2);
	
#if 1
	pszOutput[0]='\0';
#else
	APK_3RDPARTY_CWMP_TR157_Get_CommonDM_Multi_D2(pszOutput,eTR157A3_Device_PeriodicStatistics_SampleSet_M_Parameter_M_SampleSeconds,nArg1,nArg2);
#endif

	REM_LEAVE(hErr);

}
static HERROR get_PeriodicStatistics_SampleSet_M_Parameter_M_SuspectData(HCHAR *pszOutput)
{
	HERROR hErr = ERR_OK;
	HINT32	nArg1 = 0, nArg2 = 0;
	RemLOG_Print("%s(%d Line)\r\n",__func__,__LINE__);


	CHECK_NULL(pszOutput);

	rema_DM_MultiInstance_GetEntryCount(eTR157A3_Device_PeriodicStatistics_SampleSetNumberOfEntries,&nArg1);
	rema_DM_MultiInstance_GetEntryCount(eTR157A3_Device_PeriodicStatistics_SampleSet_M_ParameterNumberOfEntries,&nArg2);
	
#if 1
	pszOutput[0]='\0';
#else
	APK_3RDPARTY_CWMP_TR157_Get_CommonDM_Multi_D2(pszOutput,eTR157A3_Device_PeriodicStatistics_SampleSet_M_Parameter_M_SuspectData,nArg1,nArg2);
#endif

	REM_LEAVE(hErr);

}
static HERROR get_PeriodicStatistics_SampleSet_M_Parameter_M_Values(HCHAR *pszOutput)
{
	HERROR hErr = ERR_OK;
	HINT32	nArg1 = 0, nArg2 = 0;
	RemLOG_Print("%s(%d Line)\r\n",__func__,__LINE__);

	CHECK_NULL(pszOutput);
	
	rema_DM_MultiInstance_GetEntryCount(eTR157A3_Device_PeriodicStatistics_SampleSetNumberOfEntries,&nArg1);
	rema_DM_MultiInstance_GetEntryCount(eTR157A3_Device_PeriodicStatistics_SampleSet_M_ParameterNumberOfEntries,&nArg2);
	
#if 1
	pszOutput[0]='\0';
#else
	APK_3RDPARTY_CWMP_TR157_Get_CommonDM_Multi_D2(pszOutput,eTR157A3_Device_PeriodicStatistics_SampleSet_M_Parameter_M_Values,nArg1,nArg2);
#endif

	REM_LEAVE(hErr);

}
static HERROR get_PeriodicStatistics_SampleSet_M_Parameter_M_Failures(HCHAR *pszOutput)
{
	HERROR hErr = ERR_OK;
	HINT32	nArg1 = 0, nArg2 = 0;
	RemLOG_Print("%s(%d Line)\r\n",__func__,__LINE__);

	CHECK_NULL(pszOutput);
	rema_DM_MultiInstance_GetEntryCount(eTR157A3_Device_PeriodicStatistics_SampleSetNumberOfEntries,&nArg1);
	rema_DM_MultiInstance_GetEntryCount(eTR157A3_Device_PeriodicStatistics_SampleSet_M_ParameterNumberOfEntries,&nArg2);
	
#if 1
	HxSTD_StrCpy(pszOutput, "0");
#else
	APK_3RDPARTY_CWMP_TR157_Get_CommonDM_Multi_D2(pszOutput,eTR157A3_Device_PeriodicStatistics_SampleSet_M_Parameter_M_Failures,nArg1,nArg2);
#endif

	REM_LEAVE(hErr);

}
static HERROR get_SoftwareModules_ExecEnvNumberOfEntries(HINT32 *pszOutput)
{
	HERROR hErr = ERR_OK;
	RemLOG_Print("%s(%d Line)\r\n",__func__,__LINE__);

	CHECK_NULL(pszOutput);

	*pszOutput = 1;


	REM_LEAVE(hErr);

}
static HERROR get_SoftwareModules_ExecEnv_M_Enable(HCHAR *pszOutput)
{
	HERROR hErr = ERR_OK;
	RemLOG_Print("%s(%d Line)\r\n",__func__,__LINE__);

	CHECK_NULL(pszOutput);

	TRP_Get_SoftwareModules_ExecEnv_Enable(pszOutput);

	REM_LEAVE(hErr);

}
static HERROR get_SoftwareModules_ExecEnv_M_Status(HCHAR *pszOutput)
{
	HERROR hErr = ERR_OK;
	HCHAR szStatus[SIZE_16] = {0,};
	RemLOG_Print("%s(%d Line)\r\n",__func__,__LINE__);

	CHECK_NULL(pszOutput);

	TRP_Get_SoftwareModules_ExecEnv_Enable(szStatus);
	if(HxSTD_StrCmp(szStatus, "true") == 0)
	{
		HxSTD_StrNCpy(pszOutput, "Up",(SIZE_16-1));
	}
	else
	{
		HxSTD_StrNCpy(pszOutput, "Down",(SIZE_16-1));
	}


	REM_LEAVE(hErr);

}
static HERROR get_SoftwareModules_ExecEnv_M_Name(HCHAR *pszOutput)
{
	HERROR hErr = ERR_OK;
	HCHAR szStatus[SIZE_16] = {0,};
	RemLOG_Print("%s(%d Line)\r\n",__func__,__LINE__);

	CHECK_NULL(pszOutput);

	TRP_Get_SoftwareModules_ExecEnv_Enable(szStatus);
	if(HxSTD_StrCmp(szStatus, "true") == 0)
	{
		HxSTD_StrNCpy(pszOutput, "WA-7000",(SIZE_16-1));
	}
	else
	{
		pszOutput[0]='\0';
	}


	REM_LEAVE(hErr);

}
static HERROR get_SoftwareModules_ExecEnv_M_Type(HCHAR *pszOutput)
{
	HERROR hErr = ERR_OK;
	HCHAR szStatus[SIZE_16] = {0,};
	RemLOG_Print("%s(%d Line)\r\n",__func__,__LINE__);

	CHECK_NULL(pszOutput);

	TRP_Get_SoftwareModules_ExecEnv_Enable(szStatus);
	if(HxSTD_StrCmp(szStatus, "true") == 0)
	{
		HxSTD_StrNCpy(pszOutput, "mipsel",(SIZE_16-1));
	}
	else
	{
		pszOutput[0]='\0';
	}


	REM_LEAVE(hErr);

}
static HERROR get_SoftwareModules_ExecEnv_M_Vendor(HCHAR *pszOutput)
{
	HERROR hErr = ERR_OK;
	HCHAR szStatus[SIZE_16] = {0,};
	RemLOG_Print("%s(%d Line)\r\n",__func__,__LINE__);

	CHECK_NULL(pszOutput);

	TRP_Get_SoftwareModules_ExecEnv_Enable(szStatus);
	if(HxSTD_StrCmp(szStatus, "true") == 0)
	{
		HxSTD_StrNCpy(pszOutput, "Humax Co., Ltd.", SIZE_16);
	}
	else
	{
		pszOutput[0]='\0';
	}


	REM_LEAVE(hErr);

}
static HERROR get_SoftwareModules_ExecEnv_M_Version(HCHAR *pszOutput)
{
	HERROR hErr = ERR_OK;
	HCHAR szStatus[SIZE_16] = {0,};
	RemLOG_Print("%s(%d Line)\r\n",__func__,__LINE__);

	CHECK_NULL(pszOutput);

	TRP_Get_SoftwareModules_ExecEnv_Enable(szStatus);
	if(HxSTD_StrCmp(szStatus, "true") == 0)
	{
		hErr = get_data_DeviceInfo_SoftwareVersion(pszOutput);
	}
	else
	{
		HxSTD_StrNCpy(pszOutput, "0",(SIZE_16-1));
	}


	REM_LEAVE(hErr);

}
static HERROR get_SoftwareModules_ExecEnv_M_AllocatedDiskSpace(HCHAR *pszOutput)
{
	HERROR hErr = ERR_OK;
	RemLOG_Print("%s(%d Line)\r\n",__func__,__LINE__);

	CHECK_NULL(pszOutput);

	HxSTD_StrNCpy(pszOutput, "0",(SIZE_16-1));


	REM_LEAVE(hErr);

}
static HERROR get_SoftwareModules_ExecEnv_M_AvailableDiskSpace(HCHAR *pszOutput)
{
	HERROR hErr = ERR_OK;
	RemLOG_Print("%s(%d Line)\r\n",__func__,__LINE__);

	CHECK_NULL(pszOutput);

	HxSTD_StrNCpy(pszOutput, "0",(SIZE_16-1));


	REM_LEAVE(hErr);

}
static HERROR get_SoftwareModules_ExecEnv_M_AllocatedMemory(HCHAR *pszOutput)
{
	HERROR hErr = ERR_OK;
	HCHAR szStatus[SIZE_16] = {0,};
	RemLOG_Print("%s(%d Line)\r\n",__func__,__LINE__);

	CHECK_NULL(pszOutput);

	TRP_Get_SoftwareModules_ExecEnv_Enable(szStatus);
	if(HxSTD_StrCmp(szStatus, "true") == 0)
	{
		hErr = get_DevInfo_MemoryStatus_Total(pszOutput);
	}
	else
	{
		HxSTD_StrNCpy(pszOutput, "0",(SIZE_16-1));
	}



	REM_LEAVE(hErr);

}
static HERROR get_SoftwareModules_ExecEnv_M_AvailableMemory(HCHAR *pszOutput)
{
	HERROR hErr = ERR_OK;
	HCHAR szStatus[SIZE_16] = {0,};
	RemLOG_Print("%s(%d Line)\r\n",__func__,__LINE__);

	CHECK_NULL(pszOutput);

	TRP_Get_SoftwareModules_ExecEnv_Enable(szStatus);
	if(HxSTD_StrCmp(szStatus, "true") == 0)
	{
		hErr = get_DevInfo_MemoryStatus_Free(pszOutput);
	}
	else
	{
		HxSTD_StrNCpy(pszOutput, "0",(SIZE_16-1));
	}


	REM_LEAVE(hErr);

}
static HERROR get_SoftwareModules_ExecEnv_M_ActiveExecutionUnits(HCHAR *pszOutput)
{
	HERROR hErr = ERR_OK;
	RemLOG_Print("%s(%d Line)\r\n",__func__,__LINE__);

	CHECK_NULL(pszOutput);

	HxSTD_StrNCpy(pszOutput, "TR-069",(SIZE_16-1));


	REM_LEAVE(hErr);

}
static HERROR get_X_JCOM_HomeNet_Enable(HCHAR *pszOutput)
{
#if 0
	APKS_NETIFADDR_t		ptNetAddr;

	CHECK_NULL(pszOutput);

	if (APK_SQCNetwork_GetAddrInfo(TR_NETWORK_DEV_LAN, (APKS_NETIFADDR_t*)&ptNetAddr) != ERR_OK)
	{
		RemLOG_Print("Getting the Address Information of Network is failed.\n");
		HxSTD_StrNCpy(pszOutput, "false",(SIZE_16-1));
		REM_LEAVE(ERR_OK);
	}
	if( ptNetAddr.bEnable == TRUE )
		HxSTD_StrNCpy(pszOutput, "true",(SIZE_16-1));
	else
		HxSTD_StrNCpy(pszOutput, "false",(SIZE_16-1));
	REM_LEAVE(ERR_OK);
#else
	RemLOG_Print("Getting the Address Information of Network is failed.\n");
	HxSTD_StrNCpy(pszOutput, "false",(SIZE_16-1));
	REM_LEAVE(ERR_OK);
#endif
}
/*
Indicates the status of this interface. Enumeration of:
	Up
	NoLink
	Error
	Disabled
*/
static HERROR get_X_JCOM_HomeNet_Status(HCHAR *pszOutput)
{
	HERROR hErr = ERR_FAIL;
#if 0
	APKS_NETDEVINFO_t * stDevInfo = NULL;



	CHECK_NULL(pszOutput);

	HxSTD_StrNCpy(pszOutput, "Disabled",(SIZE_16-1));

	stDevInfo = (APKS_NETDEVINFO_t * )HxSTD_MemAlloc(sizeof(APKS_NETDEVINFO_t));
	if(stDevInfo != NULL)
	{
		//hErr = APK_SQCNetwork_GetDevInfo(TR_NETWORK_DEV_LAN, stDevInfo);
		if(hErr != ERR_OK)
		{
			RemLOG_Print("Fail to get eth0 DevInfo! %s/%d\r\n",__func__,__LINE__);
			hErr = ERR_FAIL;
			goto funcExit;
		}
		if(stDevInfo->bConnected == TRUE){
			HxSTD_StrNCpy(pszOutput, "Up",(SIZE_16-1));
		}
		else{
			HxSTD_StrNCpy(pszOutput, "NoLink",(SIZE_16-1));
		}
	}
	hErr = ERR_OK;
funcExit:
	if(stDevInfo != NULL)
	{
		HLIB_STD_MemFree(stDevInfo);
	}
	REM_LEAVE(hErr);
#else

	CHECK_NULL(pszOutput);
	HxSTD_StrNCpy(pszOutput, "Disabled",(SIZE_16-1));
	REM_LEAVE(hErr);

#endif
}
static HERROR get_X_JCOM_HomeNet_AddressingType(HCHAR *pszOutput)
{
	HERROR hErr = ERR_FAIL;
#if 0
	APKS_NETIFADDR_t	ptNetAddr;
	APKS_NETDEVINFO_t * stDevInfo = NULL;

	CHECK_NULL(pszOutput);

	pszOutput[0]='\0';

	stDevInfo = (APKS_NETDEVINFO_t * )HxSTD_MemAlloc(sizeof(APKS_NETDEVINFO_t));
	if(stDevInfo != NULL)
	{
		//hErr = APK_SQCNetwork_GetDevInfo(TR_NETWORK_DEV_LAN, stDevInfo);
		if(hErr != ERR_OK)
		{
			RemLOG_Print("Fail to get eth0 DevInfo! %s/%d\r\n",__func__,__LINE__);
			hErr = ERR_FAIL;
			goto funcExit;
		}
		if(stDevInfo->bConnected == TRUE)
		{
			if (APK_SQCNetwork_GetAddrInfo(TR_NETWORK_DEV_LAN, (APKS_NETIFADDR_t*)&ptNetAddr) != ERR_OK)
			{
				RemLOG_Print("Getting the Address Information of Network is failed.\n");
				hErr = ERR_FAIL;
				goto funcExit;
			}
			if(ptNetAddr.bDhcpOn)
				HxSTD_StrUtf8NCpy(pszOutput, "DHCP", SIZE_16);
			else
				HxSTD_StrUtf8NCpy(pszOutput, "Static", SIZE_16);

			hErr = ERR_OK;
		}
		hErr = ERR_OK;
	}
	hErr = ERR_OK;
funcExit:
	if(stDevInfo != NULL)
	{
		HLIB_STD_MemFree(stDevInfo);
	}
	REM_LEAVE(hErr);
#else
	pszOutput[0]='\0';
	REM_LEAVE(hErr);
#endif
}
static HERROR get_X_JCOM_HomeNet_IPAddress(HCHAR *pszOutput)
{
	HERROR hErr = ERR_FAIL;

#if 0
	APKS_NETIFADDR_t	ptNetAddr;
	APKS_NETDEVINFO_t * stDevInfo = NULL;

	CHECK_NULL(pszOutput);

	pszOutput[0]='\0';

	stDevInfo = (APKS_NETDEVINFO_t * )HxSTD_MemAlloc(sizeof(APKS_NETDEVINFO_t));
	if(stDevInfo != NULL)
	{
		//hErr = APK_SQCNetwork_GetDevInfo(TR_NETWORK_DEV_LAN, stDevInfo);
		if(hErr != ERR_OK)
		{
			RemLOG_Print("Fail to get eth0 DevInfo! %s/%d\r\n",__func__,__LINE__);
			hErr = ERR_FAIL;
			goto funcExit;
		}
		if(stDevInfo->bConnected == TRUE)
		{
			if (APK_SQCNetwork_GetAddrInfo(TR_NETWORK_DEV_LAN, (APKS_NETIFADDR_t*)&ptNetAddr) != ERR_OK)
			{
				RemLOG_Print("Getting the Address Information of Network is failed.\n");
				hErr = ERR_FAIL;
				goto funcExit;
			}

			if (APK_SQCNetwork_UTIL_AddrNtoA(ptNetAddr.ulIpAddr, pszOutput, TRPORT_IPADDR_LEN) != ERR_OK)
			{
				RemLOG_Print("Getting the IP Address is failed.\n");
				hErr = ERR_FAIL;
				goto funcExit;
			}
			hErr = ERR_OK;
		}
		hErr = ERR_OK;
	}
	hErr = ERR_OK;
funcExit:
	if(stDevInfo != NULL)
	{
		HLIB_STD_MemFree(stDevInfo);
	}
#endif
	REM_LEAVE(hErr);
}
static HERROR get_X_JCOM_HomeNet_SubnetMask(HCHAR *pszOutput)
{
	HERROR hErr = ERR_FAIL;

#if 0
	APKS_NETIFADDR_t	ptNetAddr;
	APKS_NETDEVINFO_t * stDevInfo = NULL;



	CHECK_NULL(pszOutput);

	pszOutput[0]='\0';

	stDevInfo = (APKS_NETDEVINFO_t * )HxSTD_MemAlloc(sizeof(APKS_NETDEVINFO_t));
	if(stDevInfo != NULL)
	{
		//hErr = APK_SQCNetwork_GetDevInfo(TR_NETWORK_DEV_LAN, stDevInfo);
		if(hErr != ERR_OK)
		{
			RemLOG_Print("Fail to get eth0 DevInfo! %s/%d\r\n",__func__,__LINE__);
			hErr = ERR_FAIL;
			goto funcExit;
		}
		if(stDevInfo->bConnected == TRUE)
		{
			if (APK_SQCNetwork_GetAddrInfo(TR_NETWORK_DEV_LAN, (APKS_NETIFADDR_t*)&ptNetAddr) != ERR_OK)
			{
				RemLOG_Print("Getting the Address Information of Network is failed.\n");
				hErr = ERR_FAIL;
				goto funcExit;
			}

			if (APK_SQCNetwork_UTIL_AddrNtoA(ptNetAddr.ulNetmask, pszOutput, TRPORT_IPADDR_LEN) != ERR_OK)
			{
				RemLOG_Print("Getting the Netmask is failed.\n");
				hErr = ERR_FAIL;
				goto funcExit;
			}
			hErr = ERR_OK;
		}
		hErr = ERR_OK;
	}
	hErr = ERR_OK;
funcExit:
	if(stDevInfo != NULL)
	{
		HLIB_STD_MemFree(stDevInfo);
	}
#else
	RemLOG_Print("Fail to get eth0 DevInfo! %s/%d\r\n",__func__,__LINE__);
	hErr = ERR_FAIL;
#endif


	REM_LEAVE(hErr);
}
static HERROR get_X_JCOM_HomeNet_DefaultGateway(HCHAR *pszOutput)
{

	HERROR hErr = ERR_FAIL;
#if 0
	APKS_NETIFADDR_t	ptNetAddr;
	APKS_NETDEVINFO_t * stDevInfo = NULL;



	CHECK_NULL(pszOutput);

	pszOutput[0]='\0';

	stDevInfo = (APKS_NETDEVINFO_t * )HxSTD_MemAlloc(sizeof(APKS_NETDEVINFO_t));
	if(stDevInfo != NULL)
	{
		//hErr = APK_SQCNetwork_GetDevInfo(TR_NETWORK_DEV_LAN, stDevInfo);
		if(hErr != ERR_OK)
		{
			RemLOG_Print("Fail to get eth0 DevInfo! %s/%d\r\n",__func__,__LINE__);
			hErr = ERR_FAIL;
			goto funcExit;
		}
		if(stDevInfo->bConnected == TRUE)
		{
			if (APK_SQCNetwork_GetAddrInfo(TR_NETWORK_DEV_LAN, (APKS_NETIFADDR_t*)&ptNetAddr) != ERR_OK)
			{
				RemLOG_Print("Getting the Address Information of Network is failed.\n");
				hErr = ERR_FAIL;
				goto funcExit;
			}

			if (APK_SQCNetwork_UTIL_AddrNtoA(ptNetAddr.ulGateway, pszOutput, TRPORT_IPADDR_LEN) != ERR_OK)
			{
				RemLOG_Print("Getting the Gateway is failed.\n");
				hErr = ERR_FAIL;
				goto funcExit;
			}
			hErr = ERR_OK;
		}
		hErr = ERR_OK;
	}
	hErr = ERR_OK;
funcExit:
	if(stDevInfo != NULL)
	{
		HLIB_STD_MemFree(stDevInfo);
	}
#else
		RemLOG_Print("Fail to get eth0 DevInfo! %s/%d\r\n",__func__,__LINE__);
		hErr = ERR_FAIL;
#endif

	REM_LEAVE(hErr);
}
static HERROR get_X_JCOM_HomeNet_DNSServers(HCHAR *pszOutput)
{
	// HomeNetwork은 DNS 를 가지지 않는다.
#if 0
	APKS_NETDEVINFO_t * stDevInfo = NULL;
	HERROR hErr = ERR_FAIL;
	char	tmpAddr1[TRPORT_IPADDR_LEN], tmpAddr2[TRPORT_IPADDR_LEN];
	APKS_NETIFADDR_t	ptNetAddr;
	CHECK_NULL(pszOutput);
	HxSTD_MemSet(tmpAddr1, 0x00, TRPORT_IPADDR_LEN);
	HxSTD_MemSet(tmpAddr2, 0x00, TRPORT_IPADDR_LEN);
#endif

	CHECK_NULL(pszOutput);

	pszOutput[0]='\0';


	// HomeNetwork은 DNS 를 가지지 않는다.
#if 0
	stDevInfo = (APKS_NETDEVINFO_t * )HxSTD_MemAlloc(sizeof(APKS_NETDEVINFO_t));
	if(stDevInfo != NULL)
	{
		//hErr = APK_SQCNetwork_GetDevInfo(TR_NETWORK_DEV_LAN, stDevInfo);
		if(hErr != ERR_OK)
		{
			RemLOG_Print("Fail to get eth0 DevInfo! %s/%d\r\n",__func__,__LINE__);
			return ERR_FAIL;
		}
		if(stDevInfo->bConnected == TRUE)
		{
			if (APK_SQCNetwork_GetAddrInfo(TR_NETWORK_DEV_LAN, (APKS_NETIFADDR_t*)&ptNetAddr) != ERR_OK)
			{
				RemLOG_Print("Getting the Address Information of Network is failed.\n");
				REM_LEAVE(ERR_FAIL);
			}

			if (APK_SQCNetwork_UTIL_AddrNtoA(ptNetAddr.ulDns1, tmpAddr1, TRPORT_IPADDR_LEN) != ERR_OK)
			{
				RemLOG_Print("Getting the DNS1 is failed.\n");
				REM_LEAVE(ERR_FAIL);
			}

			if (APK_SQCNetwork_UTIL_AddrNtoA(ptNetAddr.ulDns2, tmpAddr2, TRPORT_IPADDR_LEN) != ERR_OK)
			{
				RemLOG_Print("Getting the DNS2 is failed.\n");
				REM_LEAVE(ERR_FAIL);
			}
			snprintf(pszOutput, SIZE_256, "%s, %s", tmpAddr1, tmpAddr2);
		}
		HLIB_STD_MemFree(stDevInfo);
	}
#endif

	REM_LEAVE(ERR_OK);
}
static HERROR get_X_JCOM_HomeNet_MACAddress(HCHAR *pszOutput)
{
	HERROR hErr = ERR_FAIL;
#if 0
	int 	nIndex = -1; // eth0
	APKS_NETDEVINFO_t * stDevInfo = NULL;



	CHECK_NULL(pszOutput);

	if (APK_SQCNetwork_GetDefaultDev(&nIndex) != ERR_OK)
	{
		RemLOG_Print("Getting the default of Network Interfaces is failed.\n");
		REM_LEAVE(ERR_FAIL);
	}
	if(nIndex != -1)
	{
		stDevInfo = (APKS_NETDEVINFO_t * )HxSTD_MemAlloc(sizeof(APKS_NETDEVINFO_t));
		if(stDevInfo != NULL)
		{
			//hErr = APK_SQCNetwork_GetDevInfo(TR_NETWORK_DEV_LAN, stDevInfo);
			if(hErr != ERR_OK)
			{
				RemLOG_Print("Fail to get eth0 DevInfo! %s/%d\r\n",__func__,__LINE__);
				hErr = ERR_FAIL;
				goto funcExit;
			}
			if(stDevInfo->bConnected == TRUE)
			{
				if (APK_SQCNetwork_GetMacAddr(TR_NETWORK_DEV_LAN, pszOutput, TRPORT_MACADDR_LEN) != ERR_OK)
				{
					RemLOG_Print("Getting the Mac Address is failed.\n");
					hErr = ERR_FAIL;
					goto funcExit;
				}
			}
			hErr = ERR_OK;
		}
		hErr = ERR_OK;
	}
	hErr = ERR_OK;
funcExit:
	if(stDevInfo != NULL)
	{
		HLIB_STD_MemFree(stDevInfo);
	}
#else
	RemLOG_Print("Fail to get eth0 DevInfo! %s/%d\r\n",__func__,__LINE__);
	hErr = ERR_FAIL;
#endif
	REM_LEAVE(hErr);
}
static HERROR get_X_JCOM_HomeNet_Stats_ConnectionUpTime(HCHAR *pszOutput)
{
	HERROR hErr = ERR_OK;
	RemLOG_Print("%s(%d Line)\r\n",__func__,__LINE__);

	CHECK_NULL(pszOutput);
#if 1
	HxSTD_StrCpy(pszOutput, "0");
#else
	APK_3RDPARTY_CWMP_TR157_Get_CommonDM_String(pszOutput,eTR157A3_Device_X_JCOM_HomeNetwork_Stats_ConnectionUpTime);
#endif
	REM_LEAVE(hErr);
}

static HERROR get_X_JCOM_HomeNet_Stats_TotalBytesSent(HCHAR *pszOutput)
{
	HERROR hErr = ERR_OK;
#if 0
	RemLOG_Print("%s(%d Line)\r\n",__func__,__LINE__);

	CHECK_NULL(pszOutput);

	APK_3RDPARTY_CWMP_TR157_Get_CommonDM_String(pszOutput,eTR157A3_Device_X_JCOM_HomeNetwork_Stats_TotalBytesSent);
#else

#endif
	REM_LEAVE(hErr);

}
static HERROR get_X_JCOM_HomeNet_Stats_TotalBytesReceived(HCHAR *pszOutput)
{
	HERROR hErr = ERR_OK;
	RemLOG_Print("%s(%d Line)\r\n",__func__,__LINE__);

	CHECK_NULL(pszOutput);
#if 0
	APK_3RDPARTY_CWMP_TR157_Get_CommonDM_String(pszOutput,eTR157A3_Device_X_JCOM_HomeNetwork_Stats_TotalBytesReceived);
#endif

	REM_LEAVE(hErr);

}
static HERROR get_X_JCOM_HomeNet_Stats_TotalPacketsSent(HCHAR *pszOutput)
{
	HERROR hErr = ERR_OK;
	RemLOG_Print("%s(%d Line)\r\n",__func__,__LINE__);

	CHECK_NULL(pszOutput);
#if 0
	APK_3RDPARTY_CWMP_TR157_Get_CommonDM_String(pszOutput,eTR157A3_Device_X_JCOM_HomeNetwork_Stats_TotalPacketsSent);
#endif

	REM_LEAVE(hErr);

}
static HERROR get_X_JCOM_HomeNet_Stats_TotalPacketsReceived(HCHAR *pszOutput)
{
	HERROR hErr = ERR_OK;
	RemLOG_Print("%s(%d Line)\r\n",__func__,__LINE__);

	CHECK_NULL(pszOutput);
#if 0
	APK_3RDPARTY_CWMP_TR157_Get_CommonDM_String(pszOutput,eTR157A3_Device_X_JCOM_HomeNetwork_Stats_TotalPacketsReceived);
#endif

	REM_LEAVE(hErr);

}

static HERROR get_X_JCOM_Wifi_Enable(HCHAR *pszOutput)
{
#if 0
	APKS_NETIFADDR_t		ptNetAddr;

	CHECK_NULL(pszOutput);

	if (APK_SQCNetwork_GetAddrInfo(TR_NETWORK_DEV_WIFI, (APKS_NETIFADDR_t*)&ptNetAddr) != ERR_OK)
	{
		RemLOG_Print("Getting the Address Information of Network is failed.\n");
		HxSTD_StrNCpy(pszOutput, "false",(SIZE_16-1));
		REM_LEAVE(ERR_OK);
	}
	if( ptNetAddr.bEnable == TRUE )
		HxSTD_StrNCpy(pszOutput, "true",(SIZE_16-1));
	else
		HxSTD_StrNCpy(pszOutput, "false",(SIZE_16-1));
#else

#endif
	REM_LEAVE(ERR_OK);
}

#if 0
const char * get_X_JCOM_Wifi_ConfigMethod_String(APKE_NETAP_WIFI_Mode_e eMode)
{
	char *retStr = (char *)"";

	switch (eMode)
	{
		case eNETAP_WIFI_MODE_Manual :
			retStr = (char *)"Manual";
			break;
		case eNETAP_WIFI_MODE_WpsPin:
			retStr = (char *)"WPSPIN";
			break;
		case eNETAP_WIFI_MODE_WpsPush:
			retStr = (char *)"WPSPushButton";
			break;
		default :
			break;
	}
	return retStr;
}
#endif


#if 0
APKE_NETAP_WIFI_Mode_e get_X_JCOM_Wifi_ConfigMethod_enum(HCHAR *pszInput)
{
	if(HxSTD_StrCmp(pszInput,"Manual")==0)
	{
		return eNETAP_WIFI_MODE_Manual;
	}
	else if(HxSTD_StrCmp(pszInput,"WPSPIN")==0)
	{
		return eNETAP_WIFI_MODE_WpsPin;
	}
	else if(HxSTD_StrCmp(pszInput,"WPSPushButton")==0)
	{
		return eNETAP_WIFI_MODE_WpsPush;
	}

	return eNETAP_WIFI_MODE_MAX;
}
#endif

static HERROR get_X_JCOM_Wifi_ConfigMethod(HCHAR *pszOutput)
{
	HERROR hErr = ERR_OK;
#if 0
	APKE_NETAP_WIFI_Mode_e	eMode;
	APKS_NETIFADDR_t			ptNetAddr;

	CHECK_NULL(pszOutput);

	if (APK_SQCNetwork_GetAddrInfo(TR_NETWORK_DEV_WIFI, (APKS_NETIFADDR_t*)&ptNetAddr) != ERR_OK)
	{
		RemLOG_Print("Getting the Address Information of Network is failed.\n");
		HxSTD_StrNCpy(pszOutput, "false",(SIZE_16-1));
		REM_LEAVE(ERR_OK);
	}

	if( ptNetAddr.bEnable == TRUE )
	{
		if( APK_SQCNetwork_WIFI_bIsConnected(TR_NETWORK_DEV_WIFI) == TRUE)
		{
			APK_SQCNetwork_WIFI_GetConnectMode(&eMode);
		}
		else
		{
			eMode = s_WifiConnectMode;
		}

		HxSTD_PrintToStrN(pszOutput, SIZE_16,"%s", get_X_JCOM_Wifi_ConfigMethod_String(eMode));
	}
	else
	{
		pszOutput[0]='\0';
	}
	REM_LEAVE(hErr);
#else
	pszOutput[0]='\0';
	REM_LEAVE(hErr);
#endif
}
static HERROR get_X_JCOM_Wifi_SSID(HCHAR *pszOutput)
{
	HERROR hErr = ERR_OK;

#if 0
	APKS_NETIFADDR_t			ptNetAddr;
	APKS_NETAP_Info_t 		apInfo;
	HCHAR					szKey[SERCURITYKEYLENGTH] = { 0, };

	CHECK_NULL(pszOutput);

	if (APK_SQCNetwork_GetAddrInfo(TR_NETWORK_DEV_WIFI, (APKS_NETIFADDR_t*)&ptNetAddr) != ERR_OK)
	{
		RemLOG_Print("Getting the Address Information of Network is failed.\n");
		pszOutput[0]='\0';
		REM_LEAVE(ERR_OK);
	}

	if( ptNetAddr.bEnable == TRUE )
	{
		// APK_SQCNetwork_WIFI_bIsConnected 아니어도 Display
		if(1)//( APK_SQCNetwork_WIFI_bIsConnected(TR_NETWORK_DEV_WIFI) == TRUE)
		{
			//hErr = APK_SQCNetwork_WIFI_GetConnectedInfoByTr069(TR_NETWORK_DEV_WIFI, &apInfo, szKey, sizeof(szKey) );
			if ( ERR_OK != hErr )
			{
				RemLOG_Print("Getting Connected AP Information is failed.\n");
				pszOutput[0]='\0';
				return ERR_OK;
			}
			HxSTD_StrNCpy( pszOutput, apInfo.szESSID, SIZE_64 - 1 );
		}
		else
		{
			HxSTD_StrNCpy( pszOutput, s_stNetInfo.szESSID, SIZE_64 - 1 );
		}
	}
	else
	{
		pszOutput[0]='\0';
	}
#else
	pszOutput[0]='\0';
#endif

	RemLOG_Print("%s, pszOutput(%s)\r\n",__func__,pszOutput);

	REM_LEAVE(hErr);
}
/*
?None
?WEP
?WPA-PSK-AES
?WPA-PSK-TKIP
?WPA2-PSK-AES
?WPA2-PSK-TKIP
*/
const char * get_X_JCOM_Wifi_SecurityMethod_String( int nSecureType, int nEncryptType )
{
	char *retStr = (char *)"";
#if 0
	switch ( nSecureType )
	{
		case eInet_Wifi_Sec_Open :
			retStr = (char *)"None";
			break;
		case eInet_Wifi_Sec_WEP :
			retStr = (char *)"WEP";
			break;
		case eInet_Wifi_Sec_WPA :
			if( nEncryptType == eINet_Wifi_Encrypt_WPA_AES )
				retStr = (char *)"WPA-PSK-AES";
			else
				retStr = (char *)"WPA-PSK";
			break;
		case eInet_Wifi_Sec_WPA2 :
			if( nEncryptType == eINet_Wifi_Encrypt_WPA_AES )
				retStr = (char *)"WPA2-PSK-AES";
			else
				retStr = (char *)"WPA2-PSK";

			break;
		default :
			break;
	}
#endif
	return retStr;
}

#if 0
HERROR get_X_JCOM_Wifi_SecurityMethod_enum(APKS_NETAP_Info_t *pstApInfo, HCHAR *pszInput)
{

	if(HxSTD_StrCmp(pszInput,"None")==0)
	{
		pstApInfo->nSecureType = eInet_Wifi_Sec_Open;
		pstApInfo->nAuthenType =  eInet_Wifi_Auth_Open;
	}
	else if(HxSTD_StrCmp(pszInput,"WEP")==0)
	{
		pstApInfo->nSecureType = eInet_Wifi_Sec_WEP;
		pstApInfo->nAuthenType = eInet_Wifi_Auth_Shared;
	}
	else if(HxSTD_StrCmp(pszInput,"WPA-PSK")==0)
	{
		pstApInfo->nSecureType = eInet_Wifi_Sec_WPA;
		pstApInfo->nAuthenType = eInet_Wifi_Auth_WPAPSK;
		pstApInfo->nEncryptType = eINet_Wifi_Encrypt_WPA_TKIP;
	}
	else if(HxSTD_StrCmp(pszInput,"WPA-PSK-AES")==0)
	{
		pstApInfo->nSecureType = eInet_Wifi_Sec_WPA;
		pstApInfo->nAuthenType = eInet_Wifi_Auth_WPAPSK;
		pstApInfo->nEncryptType = eINet_Wifi_Encrypt_WPA_AES;
	}
	else if(HxSTD_StrCmp(pszInput,"WPA2-PSK")==0)
	{
		pstApInfo->nSecureType = eInet_Wifi_Sec_WPA2;
		pstApInfo->nAuthenType = eInet_Wifi_Auth_WPA2PSK;
		pstApInfo->nEncryptType = eINet_Wifi_Encrypt_WPA_TKIP;
	}
	else if(HxSTD_StrCmp(pszInput,"WPA2-PSK-AES")==0)
	{
		pstApInfo->nSecureType = eInet_Wifi_Sec_WPA2;
		pstApInfo->nAuthenType = eInet_Wifi_Auth_WPA2PSK;
		pstApInfo->nEncryptType = eINet_Wifi_Encrypt_WPA_AES;
	}
	else
	{
		pstApInfo->nSecureType = eInet_Wifi_Sec_UnKnown;
		pstApInfo->nAuthenType = eInet_Wifi_Auth_UnKnown;
		pstApInfo->nEncryptType = eINet_Wifi_Encrypt_Type_UnKnown;
	}

	return ERR_OK;
}
#endif

static HERROR get_X_JCOM_Wifi_SecurityMethod(HCHAR *pszOutput)
{
	HERROR hErr = ERR_OK;
#if 0
	APKS_NETIFADDR_t			ptNetAddr;
	APKS_NETAP_Info_t 		apInfo;
	HCHAR					szKey[SERCURITYKEYLENGTH] = { 0, };

	CHECK_NULL(pszOutput);

	if (APK_SQCNetwork_GetAddrInfo(TR_NETWORK_DEV_WIFI, (APKS_NETIFADDR_t*)&ptNetAddr) != ERR_OK)
	{
		RemLOG_Print("Getting the Address Information of Network is failed.\n");
		pszOutput[0]='\0';
		REM_LEAVE(ERR_OK);
	}

	if( ptNetAddr.bEnable == TRUE )
	{
		// APK_SQCNetwork_WIFI_bIsConnected 아니어도 Display
		if(1)//( APK_SQCNetwork_WIFI_bIsConnected(TR_NETWORK_DEV_WIFI) == TRUE)
		{
			//hErr = APK_SQCNetwork_WIFI_GetConnectedInfoByTr069(TR_NETWORK_DEV_WIFI, &apInfo, szKey, sizeof(szKey) );
			if ( ERR_OK != hErr )
			{
				RemLOG_Print("Getting Connected AP Information is failed.\n");
				pszOutput[0]='\0';
				return ERR_OK;
			}
			HxSTD_PrintToStrN(pszOutput,SIZE_16, "%s", get_X_JCOM_Wifi_SecurityMethod_String(apInfo.nSecureType, apInfo.nEncryptType));
		}
		else
		{
			HxSTD_PrintToStrN(pszOutput,SIZE_16, "%s", get_X_JCOM_Wifi_SecurityMethod_String(s_stNetInfo.nSecureType, s_stNetInfo.nEncryptType));
		}
	}
	else
	{
		pszOutput[0]='\0';
	}
#else
	RemLOG_Print("Getting the Address Information of Network is failed.\n");
	pszOutput[0]='\0';
	REM_LEAVE(hErr);
#endif
	RemLOG_Print("%s, pszOutput(%s)\r\n",__func__,pszOutput);

	REM_LEAVE(hErr);
}

static HCHAR*  get_X_JCOM_Wifi_SecurityLevel_Convert2String( int nEncryptType )
{
	char *retStr = (char *)"";
#if 0
	switch ( nEncryptType )
	{
		case eINet_Wifi_Encrypt_None :
			retStr = "None";
			break;
		case eINet_Wifi_Encrypt_WEP_64 :
		case eINet_Wifi_Encrypt_WPA_TKIP :
		case eINet_Wifi_Encrypt_WPA_AES :
		case eINet_Wifi_Encrypt_WPA_TKIP_AES :
		case eINet_Wifi_Encrypt_WEP_64_Hex :
		case eINet_Wifi_Encrypt_WEP_64_Ascii :
			retStr = "64-bit";
			break;
		case eINet_Wifi_Encrypt_WEP_128 :
		case eInet_Wifi_Encrypt_WEP_128_Hex :
		case eInet_Wifi_Encrypt_WEP_128_Ascii :
			retStr = "128-bit";
			break;
		default :
			break;
	}
#endif
	return retStr;
}

static HERROR  get_X_JCOM_Wifi_SecurityLevel_enum(HINT32 *encrypt, HCHAR* nEncryptType)
{
#if 0
	if(HxSTD_StrCmp(nEncryptType,"64-bit")==0){
		*encrypt = eINet_Wifi_Encrypt_WEP_64;
	}
	else if(HxSTD_StrCmp(nEncryptType,"128-bit")==0){
		*encrypt = eINet_Wifi_Encrypt_WEP_128;
	}
	else
	{
		return ERR_FAIL;
	}
#else
	*encrypt = 0;
#endif
	return ERR_OK;
}

static HERROR get_X_JCOM_Wifi_SecurityLevel(HCHAR *pszOutput)
{
	HERROR hErr = ERR_OK;
#if 0
	APKS_NETIFADDR_t			ptNetAddr;
	APKS_NETAP_Info_t 		apInfo;
	HCHAR					szKey[SERCURITYKEYLENGTH] = { 0, };

	CHECK_NULL(pszOutput);

	if (APK_SQCNetwork_GetAddrInfo(TR_NETWORK_DEV_WIFI, (APKS_NETIFADDR_t*)&ptNetAddr) != ERR_OK)
	{
		RemLOG_Print("Getting the Address Information of Network is failed.\n");
		pszOutput[0]='\0';
		REM_LEAVE(ERR_OK);
	}

	if( ptNetAddr.bEnable == TRUE )
	{
		// APK_SQCNetwork_WIFI_bIsConnected 아니어도 Display
		if(1)//( APK_SQCNetwork_WIFI_bIsConnected(TR_NETWORK_DEV_WIFI) == TRUE)
		{
			//hErr = APK_SQCNetwork_WIFI_GetConnectedInfoByTr069(TR_NETWORK_DEV_WIFI, &apInfo, szKey, sizeof(szKey) );
			if ( ERR_OK != hErr )
			{
				RemLOG_Print("Getting Connected AP Information is failed.\n");
				pszOutput[0]='\0';
				return ERR_OK;
			}
			HxSTD_sprintf(pszOutput, "%s", get_X_JCOM_Wifi_SecurityLevel_Convert2String(apInfo.nEncryptType));
		}
		else
		{
			HxSTD_sprintf(pszOutput, "%s", get_X_JCOM_Wifi_SecurityLevel_Convert2String(s_stNetInfo.nEncryptType));
		}
	}
	else
	{
		pszOutput[0]='\0';
	}

	RemLOG_Print("%s, pszOutput(%s)\r\n",__func__,pszOutput);
#else
	RemLOG_Print("Getting Connected AP Information is failed.\n");
	pszOutput[0]='\0';
	return ERR_OK;
#endif
	REM_LEAVE(hErr);
}

static HERROR get_X_JCOM_Wifi_SecurityKey(HCHAR *pszOutput)
{
#if defined(TEMP_NOT_USED)
	HERROR hErr = ERR_OK;
	APKS_NETIFADDR_t			ptNetAddr;
	APKS_NETAP_Info_t 		apInfo;
	HCHAR					szKey[SERCURITYKEYLENGTH] = { 0, };
#endif

	CHECK_NULL(pszOutput);

	//보안상의 문제로, empty 전달
	pszOutput[0]='\0';
	REM_LEAVE(ERR_OK);

#if defined(TEMP_NOT_USED)

	if (APK_SQCNetwork_GetAddrInfo(TR_NETWORK_DEV_WIFI, (APKS_NETIFADDR_t*)&ptNetAddr) != ERR_OK)
	{
		RemLOG_Print("Getting the Address Information of Network is failed.\n");
		pszOutput[0]='\0';
		REM_LEAVE(ERR_OK);
	}

	if( ptNetAddr.bEnable == TRUE )
	{
		// APK_SQCNetwork_WIFI_bIsConnected 아니어도 Display
		if(1)//( APK_SQCNetwork_WIFI_bIsConnected(TR_NETWORK_DEV_WIFI) == TRUE)
		{
			//hErr = APK_SQCNetwork_WIFI_GetConnectedInfoByTr069(TR_NETWORK_DEV_WIFI, &apInfo, szKey, sizeof(szKey) );
			if ( ERR_OK != hErr )
			{
				RemLOG_Print("Getting Connected AP Information is failed.\n");
				pszOutput[0]='\0';
				return ERR_OK;
			}
			HxSTD_StrNCpy(pszOutput, szKey, SIZE_128-1);
		}
		else
		{
			HxSTD_StrNCpy(pszOutput, s_szWifiSecurityKey, SIZE_128-1);
		}
	}
	else
	{
		pszOutput[0]='\0';
	}

	RemLOG_Print("%s, pszOutput(%s), szKey(%s), s_szWifiSecurityKey(%s)\r\n",__func__,pszOutput, szKey, s_szWifiSecurityKey);

	REM_LEAVE(ERR_OK);
#endif
}
static HERROR get_X_JCOM_Wifi_KeyIndex(HCHAR *pszOutput)
{
#if 0
	APKS_NETIFADDR_t			ptNetAddr;

	CHECK_NULL(pszOutput);

	if (APK_SQCNetwork_GetAddrInfo(TR_NETWORK_DEV_WIFI, (APKS_NETIFADDR_t*)&ptNetAddr) != ERR_OK)
	{
		RemLOG_Print("Getting the Address Information of Network is failed.\n");
		pszOutput[0]='\0';
		REM_LEAVE(ERR_OK);
	}

	if( ptNetAddr.bEnable == TRUE )
	{
		HxSTD_StrNCpy(pszOutput, "0",(SIZE_16-1));
	}
	else
	{
		pszOutput[0]='\0';
	}
#else
	RemLOG_Print("Getting the Address Information of Network is failed.\n");
	pszOutput[0]='\0';
	REM_LEAVE(ERR_OK);
#endif
	REM_LEAVE(ERR_OK);
}
static HERROR get_X_JCOM_Wifi_AddressingType(HCHAR *pszOutput)
{
	HERROR hErr = ERR_FAIL;
#if 0
	APKS_NETIFADDR_t	ptNetAddr;
	APKS_NETDEVINFO_t * stDevInfo = NULL;



	CHECK_NULL(pszOutput);

	pszOutput[0]='\0';

	stDevInfo = (APKS_NETDEVINFO_t * )HxSTD_MemAlloc(sizeof(APKS_NETDEVINFO_t));
	if(stDevInfo != NULL)
	{
		//hErr = APK_SQCNetwork_GetDevInfo(TR_NETWORK_DEV_WIFI, stDevInfo);
		if(hErr != ERR_OK)
		{
			RemLOG_Print("Fail to get eth0 DevInfo! %s/%d\r\n",__func__,__LINE__);
			hErr = ERR_FAIL;
			goto funcExit;
		}
		if(stDevInfo->bConnected == TRUE)
		{
			if (APK_SQCNetwork_GetAddrInfo(TR_NETWORK_DEV_WIFI, (APKS_NETIFADDR_t*)&ptNetAddr) != ERR_OK)
			{
				RemLOG_Print("Getting the Address Information of Network is failed.\n");
				hErr = ERR_FAIL;
				goto funcExit;
			}
			if(ptNetAddr.bDhcpOn)
				HxSTD_StrUtf8NCpy(pszOutput, "DHCP", SIZE_16);
			else
				HxSTD_StrUtf8NCpy(pszOutput, "Static", SIZE_16);
			hErr = ERR_OK;
		}
		hErr = ERR_OK;
	}
	hErr = ERR_OK;
funcExit:
	if(stDevInfo != NULL)
	{
		HLIB_STD_MemFree(stDevInfo);
	}
#else
	RemLOG_Print("Getting the Address Information of Network is failed.\n");
	hErr = ERR_FAIL;
#endif
	REM_LEAVE(hErr);
}
static HERROR get_X_JCOM_Wifi_IPAddress(HCHAR *pszOutput)
{
	HERROR hErr = ERR_FAIL;
#if 0
	APKS_NETIFADDR_t	ptNetAddr;
	APKS_NETDEVINFO_t * stDevInfo = NULL;



	CHECK_NULL(pszOutput);

	pszOutput[0]='\0';

	stDevInfo = (APKS_NETDEVINFO_t * )HxSTD_MemAlloc(sizeof(APKS_NETDEVINFO_t));
	if(stDevInfo != NULL)
	{
		//hErr = APK_SQCNetwork_GetDevInfo(TR_NETWORK_DEV_WIFI, stDevInfo);
		if(hErr != ERR_OK)
		{
			RemLOG_Print("Fail to get eth0 DevInfo! %s/%d\r\n",__func__,__LINE__);
			hErr = ERR_FAIL;
			goto funcExit;
		}
		if(stDevInfo->bConnected == TRUE)
		{
			if (APK_SQCNetwork_GetAddrInfo(TR_NETWORK_DEV_WIFI, (APKS_NETIFADDR_t*)&ptNetAddr) != ERR_OK)
			{
				RemLOG_Print("Getting the Address Information of Network is failed.\n");
				hErr = ERR_FAIL;
				goto funcExit;
			}

			if (ptNetAddr.bEnable == TRUE)
			{
				if (APK_SQCNetwork_UTIL_AddrNtoA(ptNetAddr.ulIpAddr, pszOutput, TRPORT_IPADDR_LEN) != ERR_OK)
				{
					RemLOG_Print("Getting the IP Address is failed.\n");
					hErr = ERR_FAIL;
					goto funcExit;
				}

				// Requested by J:COM
				if(HxSTD_StrCmp(pszOutput, "0.0.0.0") == 0)
				{
					pszOutput[0]='\0';
				}
			}
			else
			{
				pszOutput[0]='\0';
			}

			hErr = ERR_OK;
		}
		hErr = ERR_OK;
	}
	hErr = ERR_OK;
funcExit:
	if(stDevInfo != NULL)
	{
		HLIB_STD_MemFree(stDevInfo);
	}
#else
	RemLOG_Print("Fail to get eth0 DevInfo! %s/%d\r\n",__func__,__LINE__);
	hErr = ERR_FAIL;
#endif
	REM_LEAVE(hErr);
}
static HERROR get_X_JCOM_Wifi_SubnetMask(HCHAR *pszOutput)
{
	HERROR hErr = ERR_FAIL;
#if 0
	APKS_NETIFADDR_t	ptNetAddr;
	APKS_NETDEVINFO_t * stDevInfo = NULL;


	CHECK_NULL(pszOutput);

	pszOutput[0]='\0';

	stDevInfo = (APKS_NETDEVINFO_t * )HxSTD_MemAlloc(sizeof(APKS_NETDEVINFO_t));
	if(stDevInfo != NULL)
	{
		//hErr = APK_SQCNetwork_GetDevInfo(TR_NETWORK_DEV_WIFI, stDevInfo);
		if(hErr != ERR_OK)
		{
			RemLOG_Print("Fail to get eth0 DevInfo! %s/%d\r\n",__func__,__LINE__);
			hErr = ERR_FAIL;
			goto funcExit;
		}
		if(stDevInfo->bConnected == TRUE)
		{
			if (APK_SQCNetwork_GetAddrInfo(TR_NETWORK_DEV_WIFI, (APKS_NETIFADDR_t*)&ptNetAddr) != ERR_OK)
			{
				RemLOG_Print("Getting the Address Information of Network is failed.\n");
				hErr = ERR_FAIL;
				goto funcExit;
			}

			if (ptNetAddr.bEnable == TRUE)
			{
				if (APK_SQCNetwork_UTIL_AddrNtoA(ptNetAddr.ulNetmask, pszOutput, TRPORT_IPADDR_LEN) != ERR_OK)
				{
					RemLOG_Print("Getting the Netmask is failed.\n");
					hErr = ERR_FAIL;
					goto funcExit;
				}

				// Requested by J:COM
				if(HxSTD_StrCmp(pszOutput, "0.0.0.0") == 0)
				{
					pszOutput[0]='\0';
				}
			}
			else
			{
				pszOutput[0]='\0';
			}

			hErr = ERR_OK;

		}
		hErr = ERR_OK;
	}
	hErr = ERR_OK;

funcExit:
	if(stDevInfo != NULL)
	{
		HLIB_STD_MemFree(stDevInfo);
	}
#else
	RemLOG_Print("Fail to get eth0 DevInfo! %s/%d\r\n",__func__,__LINE__);
	hErr = ERR_FAIL;
#endif
	REM_LEAVE(hErr);
}
static HERROR get_X_JCOM_Wifi_DefaultGateway(HCHAR *pszOutput)
{
	HERROR hErr = ERR_FAIL;

#if 0
	APKS_NETIFADDR_t	ptNetAddr;
	APKS_NETDEVINFO_t * stDevInfo = NULL;



	CHECK_NULL(pszOutput);

	pszOutput[0]='\0';

	stDevInfo = (APKS_NETDEVINFO_t * )HxSTD_MemAlloc(sizeof(APKS_NETDEVINFO_t));
	if(stDevInfo != NULL)
	{
		//hErr = APK_SQCNetwork_GetDevInfo(TR_NETWORK_DEV_WIFI, stDevInfo);
		if(hErr != ERR_OK)
		{
			RemLOG_Print("Fail to get eth0 DevInfo! %s/%d\r\n",__func__,__LINE__);
			hErr = ERR_FAIL;
			goto funcExit;
		}
		if(stDevInfo->bConnected == TRUE)
		{
			if (APK_SQCNetwork_GetAddrInfo(TR_NETWORK_DEV_WIFI, (APKS_NETIFADDR_t*)&ptNetAddr) != ERR_OK)
			{
				RemLOG_Print("Getting the Address Information of Network is failed.\n");
				hErr = ERR_FAIL;
				goto funcExit;
			}

			if (APK_SQCNetwork_UTIL_AddrNtoA(ptNetAddr.ulGateway, pszOutput, TRPORT_IPADDR_LEN) != ERR_OK)
			{
				RemLOG_Print("Getting the Gateway is failed.\n");
				hErr = ERR_FAIL;
				goto funcExit;
			}

			// Requested by J:COM
			if(HxSTD_StrCmp(pszOutput, "0.0.0.0") == 0)
			{
				pszOutput[0]='\0';
			}
			hErr = ERR_OK;
		}
		hErr = ERR_OK;
	}
	hErr = ERR_OK;
funcExit:
	if(stDevInfo != NULL)
	{
		HLIB_STD_MemFree(stDevInfo);
	}
#else
	pszOutput[0]='\0';
	hErr = ERR_OK;
#endif
	REM_LEAVE(hErr);
}

static HERROR get_Capa_PerformanceDiagnostic_DownloadTransports(HCHAR *pszOutput)
{
	HERROR hErr = ERR_OK;
	HxSTD_StrNCpy(pszOutput, "HTTP",(SIZE_16-1));

	REM_LEAVE(hErr);
}
static HERROR get_Capa_PerformanceDiagnostic_UploadTransports(HCHAR *pszOutput)
{
	HERROR hErr = ERR_OK;
	HxSTD_StrNCpy(pszOutput, "HTTP",(SIZE_16-1));

	REM_LEAVE(hErr);
}




#define __________STATIC_FUNCTION_SET_DATA_______________________________________

static HERROR set_data_DeviceInfo_X_JCOM_SubscriberID(HCHAR *pszInput)
{
	HERROR hErr = ERR_OK;
#if 0
	CHECK_NULL(pszInput);
	hErr = rema_DM_setData_DeviceInfo_X_JCOM_SubscriberID(pszInput);

	//UI Display위해 SQC부분에도 Set
	if(APK_SQCSCENARIOJAPAN_SetSubscriberID(pszInput) != ERR_OK)
	{
		RemLOG_Print("Setting the  subscriberID is failed..\n");
	}
#endif

	REM_LEAVE(hErr);
}

static HERROR set_data_DeviceInfo_X_JCOM_ServiceID(HCHAR *pszInput)
{
	HERROR hErr = ERR_OK;
#if 0
	CHECK_NULL(pszInput);
	hErr = rema_DM_setData_DeviceInfo_X_JCOM_ServiceID(pszInput);

	//UI Display위해 SQC부분에도 Set
	if(APK_SQCSCENARIOJAPAN_SetServiceID(pszInput) != ERR_OK)
	{
		RemLOG_Print("Setting the  subscriberID is failed..\n");
	}
#endif
	REM_LEAVE(hErr);
}
static HERROR set_data_DeviceInfo_X_JCOM_FamilyID(HCHAR *pszInput)
{
	HERROR hErr = ERR_OK;
#if 0
	CHECK_NULL(pszInput);
	hErr = rema_DM_setData_DeviceInfo_X_JCOM_FamilyID(pszInput);

	//UI Display위해 SQC부분에도 Set
	if(APK_SQCSCENARIOJAPAN_SetFamilyID(pszInput) != ERR_OK)
	{
		RemLOG_Print("Setting the  subscriberID is failed..\n");
	}
#endif
	REM_LEAVE(hErr);
}
static HERROR set_data_DeviceInfo_X_JCOM_BusinessCode(HCHAR *pszInput)
{
	HERROR hErr = ERR_OK;
#if 0
	CHECK_NULL(pszInput);
	hErr = rema_DM_setData_DeviceInfo_X_JCOM_BusinessCode(pszInput);

	//UI Display위해 SQC부분에도 Set
	if(APK_SQCSCENARIOJAPAN_SetBusinessCode(pszInput) != ERR_OK)
	{
		RemLOG_Print("Setting the  subscriberID is failed..\n");
	}
#endif
	REM_LEAVE(hErr);
}

static HERROR set_data_ManagementServer_URL(HCHAR *pszInput)
{
	HERROR hErr = ERR_OK;

	CHECK_NULL(pszInput);
	hErr = rema_DM_setData_ManagementServer_Url(pszInput);

	REM_LEAVE(hErr);

}

static HERROR set_data_ManagementServer_Username(HCHAR *pszInput)
{
	HERROR hErr = ERR_OK;

	CHECK_NULL(pszInput);
	hErr = rema_DM_setData_ManagementServer_Username(pszInput);

	REM_LEAVE(hErr);
}

static HERROR set_data_ManagementServer_Password(HCHAR *pszInput)
{
	HERROR hErr = ERR_OK;

	CHECK_NULL(pszInput);
	hErr = rema_DM_setData_ManagementServer_Password(pszInput);

	REM_LEAVE(hErr);
}

static HERROR set_data_ManagementServer_PeriodicInformEnable(HCHAR *pszInput)
{
	HERROR hErr = ERR_OK;

	CHECK_NULL(pszInput);
	hErr = rema_DM_setData_ManagementServer_PeriodicInformEnable(pszInput);

	REM_LEAVE(hErr);
}

static HERROR set_data_ManagementServer_PeriodicInformInterval(HCHAR *pszInput)
{
	HERROR hErr = ERR_OK;

	CHECK_NULL(pszInput);
	hErr = rema_DM_setData_ManagementServer_PeriodicInformInterval(pszInput);

	REM_LEAVE(hErr);
}

static HERROR set_data_ManagementServer_PeriodicInformTime(HCHAR *pszInput)
{
	HERROR hErr = ERR_OK;

	CHECK_NULL(pszInput);
	hErr = rema_DM_setData_ManagementServer_PeriodicInformTime(pszInput);

	REM_LEAVE(hErr);
}
static HERROR set_data_ManagementServer_ConnectionRequestURL(HCHAR *pszInput)
{
	HERROR hErr = ERR_OK;

	CHECK_NULL(pszInput);
	hErr = rema_DM_setData_ManagementServer_ConnectionRequestURL(pszInput);

	REM_LEAVE(hErr);
}
static HERROR set_data_ManagementServer_ConnectionRequestUsername(HCHAR *pszInput)
{
	HERROR hErr = ERR_OK;

	CHECK_NULL(pszInput);
	hErr = rema_DM_setData_ManagementServer_ConnectionRequestUsername(pszInput);

	REM_LEAVE(hErr);
}

static HERROR set_data_ManagementServer_ConnectionRequestPassword(HCHAR *pszInput)
{
	HERROR hErr = ERR_OK;

	CHECK_NULL(pszInput);
	hErr = rema_DM_setData_ManagementServer_ConnectionRequestPassword(pszInput);

	REM_LEAVE(hErr);
}

static HERROR set_data_ManagementServer_UpgradesManaged(HCHAR *pszInput)
{
	HERROR hErr = ERR_OK;

	CHECK_NULL(pszInput);
	hErr = rema_DM_setData_ManagementServer_UpgradesManaged(pszInput);

	REM_LEAVE(hErr);
}

static HERROR set_data_LAN_AddressingType(HCHAR *pszInput)
{
#if 0

	int 	nIndex;
	APKS_NETIFADDR_t	ptNetAddr;

	CHECK_NULL(pszInput);

	if (APK_SQCNetwork_GetDefaultDev(&nIndex) != ERR_OK)
	{
		RemLOG_Print("Getting the default of Network Interfaces is failed.\n");
		REM_LEAVE(ERR_FAIL);
	}

	if( nIndex == TR_NETWORK_DEV_CABLEMODEM )
		REM_LEAVE(ERR_OK);

	if (APK_SQCNetwork_GetAddrInfo(nIndex, (APKS_NETIFADDR_t*)&ptNetAddr) != ERR_OK)
	{
		RemLOG_Print("Getting the Address Information of Network is failed.\n");
		REM_LEAVE(ERR_FAIL);
	}

	if(HxSTD_StrCmp(pszInput, "DHCP") == 0)
	{
		ptNetAddr.bDhcpOn = TRUE;
		ptNetAddr.bDnsAuto = TRUE;
	}
	else if(HxSTD_StrCmp(pszInput, "Static") == 0)
	{
		ptNetAddr.bDhcpOn = FALSE;
	}
	else
	{
		RemLOG_Print("wrong input! %s is not supported![%s:%d]\n", pszInput, __FUNCTION__, __LINE__);
		REM_LEAVE(ERR_FAIL);
	}

	if (APK_SQCNetwork_SetAddrInfo( nIndex, &ptNetAddr ) != ERR_OK)
	{
		RemLOG_Print("Setting the Address Information of Network is failed. [%s:%d]\n", __FUNCTION__, __LINE__);
		REM_LEAVE(ERR_FAIL);
	}
#else
	RemLOG_Print("Setting the Address Information of Network is failed. [%s:%d]\n", __FUNCTION__, __LINE__);
	REM_LEAVE(ERR_FAIL);
#endif

	REM_LEAVE(ERR_OK);
}

static HERROR set_data_LAN_IPAddress(HCHAR *pszInput)
{

#if 0
	int 	nIndex;
	APKS_NETIFADDR_t	ptNetAddr;

	CHECK_NULL(pszInput);

	if (APK_SQCNetwork_GetDefaultDev(&nIndex) != ERR_OK)
	{
		RemLOG_Print("Getting the default of Network Interfaces is failed.\n");
		REM_LEAVE(ERR_FAIL);
	}

	if( nIndex == TR_NETWORK_DEV_CABLEMODEM )
		REM_LEAVE(ERR_OK);

	if (APK_SQCNetwork_GetAddrInfo(nIndex, (APKS_NETIFADDR_t*)&ptNetAddr) != ERR_OK)
	{
		RemLOG_Print("Getting the Address Information of Network is failed.\n");
		REM_LEAVE(ERR_FAIL);
	}

	APK_SQCNetwork_UTIL_AddrAtoN( pszInput, &ptNetAddr.ulIpAddr );

	if (APK_SQCNetwork_SetAddrInfo( nIndex, &ptNetAddr ) != ERR_OK)
	{
		RemLOG_Print("Setting the Address Information of Network is failed. [%s:%d]\n", __FUNCTION__, __LINE__);
		REM_LEAVE(ERR_FAIL);
	}
#else
	RemLOG_Print("Setting the Address Information of Network is failed. [%s:%d]\n", __FUNCTION__, __LINE__);
	REM_LEAVE(ERR_FAIL);
#endif
	REM_LEAVE(ERR_OK);
}

static HERROR set_data_LAN_SubnetMask(HCHAR *pszInput)
{
#if 0
	int 	nIndex;
	APKS_NETIFADDR_t	ptNetAddr;

	CHECK_NULL(pszInput);

	if (APK_SQCNetwork_GetDefaultDev(&nIndex) != ERR_OK)
	{
		RemLOG_Print("Getting the default of Network Interfaces is failed.\n");
		REM_LEAVE(ERR_FAIL);
	}

	if( nIndex == TR_NETWORK_DEV_CABLEMODEM )
		REM_LEAVE(ERR_OK);

	if (APK_SQCNetwork_GetAddrInfo(nIndex, (APKS_NETIFADDR_t*)&ptNetAddr) != ERR_OK)
	{
		RemLOG_Print("Getting the Address Information of Network is failed.\n");
		REM_LEAVE(ERR_FAIL);
	}

	APK_SQCNetwork_UTIL_AddrAtoN( pszInput, &ptNetAddr.ulNetmask );

	if (APK_SQCNetwork_SetAddrInfo( nIndex, &ptNetAddr ) != ERR_OK)
	{
		RemLOG_Print("Setting the Address Information of Network is failed. [%s:%d]\n", __FUNCTION__, __LINE__);
		REM_LEAVE(ERR_FAIL);
	}

	REM_LEAVE(ERR_OK);
#else
	RemLOG_Print("Setting the Address Information of Network is failed. [%s:%d]\n", __FUNCTION__, __LINE__);
	REM_LEAVE(ERR_FAIL);
#endif
}

static HERROR set_data_LAN_DefaultGateway(HCHAR *pszInput)
{
#if 0
	int 	nIndex;
	APKS_NETIFADDR_t	ptNetAddr;

	CHECK_NULL(pszInput);

	if (APK_SQCNetwork_GetDefaultDev(&nIndex) != ERR_OK)
	{
		RemLOG_Print("Getting the default of Network Interfaces is failed.\n");
		REM_LEAVE(ERR_FAIL);
	}

	if( nIndex == TR_NETWORK_DEV_CABLEMODEM )
		REM_LEAVE(ERR_OK);

	if (APK_SQCNetwork_GetAddrInfo(nIndex, (APKS_NETIFADDR_t*)&ptNetAddr) != ERR_OK)
	{
		RemLOG_Print("Getting the Address Information of Network is failed.\n");
		REM_LEAVE(ERR_FAIL);
	}

	APK_SQCNetwork_UTIL_AddrAtoN( pszInput, &ptNetAddr.ulGateway );

	if (APK_SQCNetwork_SetAddrInfo( nIndex, &ptNetAddr ) != ERR_OK)
	{
		RemLOG_Print("Setting the Address Information of Network is failed. [%s:%d]\n", __FUNCTION__, __LINE__);
		REM_LEAVE(ERR_FAIL);
	}

	REM_LEAVE(ERR_OK);
#else
	RemLOG_Print("Getting the default of Network Interfaces is failed.\n");
	REM_LEAVE(ERR_FAIL);
#endif
}

static HERROR set_data_LAN_DNSServers(HCHAR *pszInput)
{
#if 0
	int 	nIndex;
	char	DNSAddr1[TRPORT_IPADDR_LEN], DNSAddr2[TRPORT_IPADDR_LEN];
	APKS_NETIFADDR_t		ptNetAddr;
	HCHAR				*iter;
	HINT32				cplen = 0, inputlen = 0;
	HERROR				hErr = ERR_FAIL;

	CHECK_NULL(pszInput);


	if (APK_SQCNetwork_GetDefaultDev(&nIndex) != ERR_OK)
	{
		RemLOG_Print("Getting the default of Network Interfaces is failed.\n");
		REM_LEAVE(ERR_FAIL);
	}

	if( nIndex == TR_NETWORK_DEV_CABLEMODEM )
		REM_LEAVE(ERR_OK);

	if (APK_SQCNetwork_GetAddrInfo(nIndex, (APKS_NETIFADDR_t*)&ptNetAddr) != ERR_OK)
	{
		RemLOG_Print("Getting the Address Information of Network is failed.\n");
		REM_LEAVE(ERR_FAIL);
	}

	HxSTD_memset(DNSAddr1, 0x0, TRPORT_IPADDR_LEN);
	HxSTD_memset(DNSAddr2, 0x0, TRPORT_IPADDR_LEN);

	inputlen = HxSTD_StrLen(pszInput);

	iter = strchr(pszInput, ',');
	if( iter != NULL )
	{
		cplen = iter - pszInput;

		if( (cplen > TRPORT_IPADDR_LEN-1) || (cplen <= 0) )
			cplen = TRPORT_IPADDR_LEN-1;

		while(((*iter == ' ') || (*iter == ',')) && (*iter != '\0' ))
			iter++;

		HxSTD_StrNCpy(DNSAddr1, pszInput, cplen);
		DNSAddr1[TRPORT_IPADDR_LEN-1] = '\0';

		if (*iter != '\0' )
		{
			HxSTD_StrNCpy(DNSAddr2, iter, TRPORT_IPADDR_LEN-1);
			DNSAddr2[TRPORT_IPADDR_LEN-1] = '\0';
		}
	}
	else
	{
		HxSTD_StrNCpy(DNSAddr1, pszInput, TRPORT_IPADDR_LEN-1);
		DNSAddr1[TRPORT_IPADDR_LEN-1] = '\0';
	}

	ptNetAddr.ulDns1 = 0;
	if(DNSAddr1[0] !=  '\0')
	{
		//hErr = APK_SQCNetwork_UTIL_AddrAtoN( DNSAddr1, &ptNetAddr.ulDns1 );
		if( hErr != ERR_OK)
			ptNetAddr.ulDns1 = 0;
	}

	ptNetAddr.ulDns2 = 0;
	if(DNSAddr2[0] !=  '\0')
	{
		//hErr = APK_SQCNetwork_UTIL_AddrAtoN( DNSAddr2, &ptNetAddr.ulDns2 );
		if( hErr != ERR_OK)
			ptNetAddr.ulDns2 = 0;
	}

	ptNetAddr.bDnsAuto = FALSE;

	if (APK_SQCNetwork_SetAddrInfo( nIndex, &ptNetAddr ) != ERR_OK)
	{
		RemLOG_Print("Setting the Address Information of Network is failed. [%s:%d]\n", __FUNCTION__, __LINE__);
		REM_LEAVE(ERR_FAIL);
	}

	REM_LEAVE(ERR_OK);
#else
	RemLOG_Print("Setting the Address Information of Network is failed. [%s:%d]\n", __FUNCTION__, __LINE__);
	REM_LEAVE(ERR_FAIL);
#endif
}

static HERROR set_data_X_JCOM_HomeNet_Enable(HCHAR *pszInput)
{
#if 0
	APKS_NETIFADDR_t	ptNetAddr;

	CHECK_NULL(pszInput);

	if (APK_SQCNetwork_GetAddrInfo(TR_NETWORK_DEV_LAN, (APKS_NETIFADDR_t*)&ptNetAddr) != ERR_OK)
	{
		RemLOG_Print("Getting the Address Information of Network is failed.\n");
		REM_LEAVE(ERR_FAIL);
	}

	if(HxSTD_StrCmp(pszInput, "true") == 0)
	{
		if( ptNetAddr.bEnable == FALSE )
		{
			if (APK_SQCNetwork_SaveEnableStatus(TR_NETWORK_DEV_LAN, TRUE) != ERR_OK)
			{
				RemLOG_Print("Setting the Enable of Network is failed. [%s:%d]\n", __FUNCTION__, __LINE__);
				REM_LEAVE(ERR_FAIL);
			}
		}
		else
		{
			RemLOG_Print("bEnable(%d) pszInput(%s),  [%s:%d]\n", ptNetAddr.bEnable, pszInput, __FUNCTION__, __LINE__);
		}
	}
	else if(HxSTD_StrCmp(pszInput, "false") == 0)
	{
		if( ptNetAddr.bEnable == TRUE )
		{
			if (APK_SQCNetwork_SaveEnableStatus(TR_NETWORK_DEV_LAN, FALSE) != ERR_OK)
			{
				RemLOG_Print("Setting the Enable of Network is failed. [%s:%d]\n", __FUNCTION__, __LINE__);
				REM_LEAVE(ERR_FAIL);
			}
		}
		else
		{
			RemLOG_Print("bEnable(%d) pszInput(%s),  [%s:%d]\n", ptNetAddr.bEnable, pszInput, __FUNCTION__, __LINE__);
		}
	}
	else
	{
		RemLOG_Print("wrong input! %s is not supported![%s:%d]\n", pszInput, __FUNCTION__, __LINE__);
		REM_LEAVE(ERR_FAIL);
	}

	REM_LEAVE(ERR_OK);
#else
	RemLOG_Print("Getting the Address Information of Network is failed.\n");
	REM_LEAVE(ERR_FAIL);
#endif
}
static HERROR set_data_X_JCOM_HomeNet_AddressingType(HCHAR *pszInput)
{
#if 0
	APKS_NETIFADDR_t	ptNetAddr;

	CHECK_NULL(pszInput);

	if (APK_SQCNetwork_GetAddrInfo(TR_NETWORK_DEV_LAN, (APKS_NETIFADDR_t*)&ptNetAddr) != ERR_OK)
	{
		RemLOG_Print("Getting the Address Information of Network is failed.\n");
		REM_LEAVE(ERR_FAIL);
	}

	if(HxSTD_StrCmp(pszInput, "DHCP") == 0)
	{
		ptNetAddr.bDhcpOn = TRUE;
		ptNetAddr.bDnsAuto = TRUE;
	}
	else if(HxSTD_StrCmp(pszInput, "Static") == 0)
	{
		ptNetAddr.bDhcpOn = FALSE;
	}
	else
	{
		RemLOG_Print("wrong input! %s is not supported![%s:%d]\n", pszInput, __FUNCTION__, __LINE__);
		REM_LEAVE(ERR_FAIL);
	}

	if (APK_SQCNetwork_SetAddrInfo( TR_NETWORK_DEV_LAN, &ptNetAddr ) != ERR_OK)
	{
		RemLOG_Print("Setting the Address Information of Network is failed. [%s:%d]\n", __FUNCTION__, __LINE__);
		REM_LEAVE(ERR_FAIL);
	}
	REM_LEAVE(ERR_OK);
#else
	RemLOG_Print("Getting the Address Information of Network is failed.\n");
	REM_LEAVE(ERR_FAIL);
#endif
}

static HERROR set_data_X_JCOM_HomeNet_IPAddress(HCHAR *pszInput)
{
#if 0
	APKS_NETIFADDR_t	ptNetAddr;

	CHECK_NULL(pszInput);

	if (APK_SQCNetwork_GetAddrInfo(TR_NETWORK_DEV_LAN, (APKS_NETIFADDR_t*)&ptNetAddr) != ERR_OK)
	{
		RemLOG_Print("Getting the Address Information of Network is failed.\n");
		REM_LEAVE(ERR_FAIL);
	}

	APK_SQCNetwork_UTIL_AddrAtoN( pszInput, &ptNetAddr.ulIpAddr );

	if (APK_SQCNetwork_SetAddrInfo( TR_NETWORK_DEV_LAN, &ptNetAddr ) != ERR_OK)
	{
		RemLOG_Print("Setting the Address Information of Network is failed. [%s:%d]\n", __FUNCTION__, __LINE__);
		REM_LEAVE(ERR_FAIL);
	}

	REM_LEAVE(ERR_OK);
#else
	RemLOG_Print("Setting the Address Information of Network is failed. [%s:%d]\n", __FUNCTION__, __LINE__);
	REM_LEAVE(ERR_FAIL);
#endif
}
static HERROR set_data_X_JCOM_HomeNet_SubnetMask(HCHAR *pszInput)
{
#if 0
	APKS_NETIFADDR_t	ptNetAddr;

	CHECK_NULL(pszInput);

	if (APK_SQCNetwork_GetAddrInfo(TR_NETWORK_DEV_LAN, (APKS_NETIFADDR_t*)&ptNetAddr) != ERR_OK)
	{
		RemLOG_Print("Getting the Address Information of Network is failed.\n");
		REM_LEAVE(ERR_FAIL);
	}

	APK_SQCNetwork_UTIL_AddrAtoN( pszInput, &ptNetAddr.ulNetmask );

	if (APK_SQCNetwork_SetAddrInfo( TR_NETWORK_DEV_LAN, &ptNetAddr ) != ERR_OK)
	{
		RemLOG_Print("Setting the Address Information of Network is failed. [%s:%d]\n", __FUNCTION__, __LINE__);
		REM_LEAVE(ERR_FAIL);
	}
#else

#endif
	REM_LEAVE(ERR_OK);
}
static HERROR set_data_X_JCOM_HomeNet_DefaultGateway(HCHAR *pszInput)
{
#if 0
	APKS_NETIFADDR_t	ptNetAddr;

	CHECK_NULL(pszInput);

	if (APK_SQCNetwork_GetAddrInfo(TR_NETWORK_DEV_LAN, (APKS_NETIFADDR_t*)&ptNetAddr) != ERR_OK)
	{
		RemLOG_Print("Getting the Address Information of Network is failed.\n");
		REM_LEAVE(ERR_FAIL);
	}

	APK_SQCNetwork_UTIL_AddrAtoN( pszInput, &ptNetAddr.ulGateway );

	if (APK_SQCNetwork_SetAddrInfo( TR_NETWORK_DEV_LAN, &ptNetAddr ) != ERR_OK)
	{
		RemLOG_Print("Setting the Address Information of Network is failed. [%s:%d]\n", __FUNCTION__, __LINE__);
		REM_LEAVE(ERR_FAIL);
	}
	REM_LEAVE(ERR_OK);
#else
	RemLOG_Print("Getting the Address Information of Network is failed.\n");
	REM_LEAVE(ERR_FAIL);
#endif
}
static HERROR set_data_X_JCOM_HomeNet_DNSServers(HCHAR *pszInput)
{
#if 0
	APKS_NETIFADDR_t		ptNetAddr;
	char					DNSAddr1[TRPORT_IPADDR_LEN], DNSAddr2[TRPORT_IPADDR_LEN];
	HCHAR				*iter;
	HINT32				cplen = 0, inputlen = 0;
	HERROR				hErr = ERR_FAIL;

	CHECK_NULL(pszInput);

	if (APK_SQCNetwork_GetAddrInfo(TR_NETWORK_DEV_LAN, (APKS_NETIFADDR_t*)&ptNetAddr) != ERR_OK)
	{
		RemLOG_Print("Getting the Address Information of Network is failed.\n");
		REM_LEAVE(ERR_FAIL);
	}

	HxSTD_memset(DNSAddr1, 0x0, TRPORT_IPADDR_LEN);
	HxSTD_memset(DNSAddr2, 0x0, TRPORT_IPADDR_LEN);

	inputlen = HxSTD_StrLen(pszInput);

	iter = strchr(pszInput, ',');
	if( iter != NULL )
	{
		cplen = iter - pszInput;

		if( (cplen > TRPORT_IPADDR_LEN-1) || (cplen <= 0) )
			cplen = TRPORT_IPADDR_LEN-1;

		while(((*iter == ' ') || (*iter == ',')) && (*iter != '\0' ))
			iter++;

		HxSTD_StrNCpy(DNSAddr1, pszInput, cplen);
		DNSAddr1[TRPORT_IPADDR_LEN-1] = '\0';

		if (*iter != '\0' )
		{
			HxSTD_StrNCpy(DNSAddr2, iter, TRPORT_IPADDR_LEN-1);
			DNSAddr2[TRPORT_IPADDR_LEN-1] = '\0';
		}
	}
	else
	{
		HxSTD_StrNCpy(DNSAddr1, pszInput, TRPORT_IPADDR_LEN-1);
		DNSAddr1[TRPORT_IPADDR_LEN-1] = '\0';
	}

	ptNetAddr.ulDns1 = 0;
	if(DNSAddr1[0] !=  '\0')
	{
		//hErr = APK_SQCNetwork_UTIL_AddrAtoN( DNSAddr1, &ptNetAddr.ulDns1 );
		if( hErr != ERR_OK)
			ptNetAddr.ulDns1 = 0;
	}

	ptNetAddr.ulDns2 = 0;
	if(DNSAddr2[0] !=  '\0')
	{
		//hErr = APK_SQCNetwork_UTIL_AddrAtoN( DNSAddr2, &ptNetAddr.ulDns2 );
		if( hErr != ERR_OK)
			ptNetAddr.ulDns2 = 0;
	}

	ptNetAddr.bDnsAuto = FALSE;

	if (APK_SQCNetwork_SetAddrInfo( TR_NETWORK_DEV_LAN, &ptNetAddr ) != ERR_OK)
	{
		RemLOG_Print("Setting the Address Information of Network is failed. [%s:%d]\n", __FUNCTION__, __LINE__);
		REM_LEAVE(ERR_FAIL);
	}
	REM_LEAVE(ERR_OK);
#else
	RemLOG_Print("Getting the Address Information of Network is failed.\n");
	REM_LEAVE(ERR_FAIL);
#endif
}

static HERROR set_data_Wifi_Enable(HCHAR *pszInput)
{
#if 0
	APKS_NETIFADDR_t	ptNetAddr;

	CHECK_NULL(pszInput);

	if (APK_SQCNetwork_GetAddrInfo(TR_NETWORK_DEV_WIFI, (APKS_NETIFADDR_t*)&ptNetAddr) != ERR_OK)
	{
		RemLOG_Print("Getting the Address Information of Network is failed.\n");
		REM_LEAVE(ERR_FAIL);
	}

	if(HxSTD_StrCmp(pszInput, "true") == 0)
	{
		if( ptNetAddr.bEnable == FALSE )
		{
			if (APK_SQCNetwork_SaveEnableStatus(TR_NETWORK_DEV_WIFI, TRUE) != ERR_OK)
			{
				RemLOG_Print("Setting the Enable of Network is failed. [%s:%d]\n", __FUNCTION__, __LINE__);
				REM_LEAVE(ERR_FAIL);
			}
		}
		else
		{
			RemLOG_Print("bEnable(%d) pszInput(%s),  [%s:%d]\n", ptNetAddr.bEnable, pszInput, __FUNCTION__, __LINE__);
		}
	}
	else if(HxSTD_StrCmp(pszInput, "false") == 0)
	{
		if( ptNetAddr.bEnable == TRUE )
		{
			if (APK_SQCNetwork_SaveEnableStatus(TR_NETWORK_DEV_WIFI, FALSE) != ERR_OK)
			{
				RemLOG_Print("Setting the Enable of Network is failed. [%s:%d]\n", __FUNCTION__, __LINE__);
				REM_LEAVE(ERR_FAIL);
			}
		}
		else
		{
			RemLOG_Print("bEnable(%d) pszInput(%s),  [%s:%d]\n", ptNetAddr.bEnable, pszInput, __FUNCTION__, __LINE__);
		}
	}
	else
	{
		RemLOG_Print("wrong input! %s is not supported![%s:%d]\n", pszInput, __FUNCTION__, __LINE__);
		REM_LEAVE(ERR_FAIL);
	}

	REM_LEAVE(ERR_OK);
#else
	RemLOG_Print("wrong input! %s is not supported![%s:%d]\n", pszInput, __FUNCTION__, __LINE__);
	REM_LEAVE(ERR_FAIL);
#endif
}

static HERROR set_data_Wifi_ConfigMethod(HCHAR *pszInput)
{
	HERROR hErr = ERR_OK;
#if 0
	APKS_NETIFADDR_t			ptNetAddr;
	APKE_NETAP_WIFI_Mode_e	eOrgMode = eNETAP_WIFI_MODE_MAX, eChgMode = eNETAP_WIFI_MODE_MAX;
	APKS_NETAP_Info_t			stApInfo;
	HCHAR					szKey[SERCURITYKEYLENGTH] = { 0 };

	RemLOG_Print("%s(%d Line)\r\n",__func__,__LINE__);

	CHECK_NULL(pszInput);

	eChgMode = get_X_JCOM_Wifi_ConfigMethod_enum(pszInput);
	s_WifiConnectMode = eChgMode;

	if (APK_SQCNetwork_GetAddrInfo(TR_NETWORK_DEV_WIFI, (APKS_NETIFADDR_t*)&ptNetAddr) != ERR_OK)
	{
		RemLOG_Print("Getting the Address Information of Network is failed.\n");
		REM_LEAVE(ERR_FAIL);
	}

	if( ptNetAddr.bEnable == TRUE )
	{

		if( APK_SQCNetwork_WIFI_bIsConnected(TR_NETWORK_DEV_WIFI) == TRUE)
		{
			APK_SQCNetwork_WIFI_GetConnectMode(&eOrgMode);
			APK_SQCNetwork_WIFI_GetConnectedInfo(TR_NETWORK_DEV_WIFI, &stApInfo, szKey, SERCURITYKEYLENGTH );
		}
		else
		{
			eOrgMode = eNETAP_WIFI_MODE_MAX;
			VK_memcpy(&stApInfo, &s_stNetInfo, sizeof(APKS_NETAP_Info_t));
			HxSTD_StrNCpy(szKey, s_szWifiSecurityKey, SERCURITYKEYLENGTH-1);
			szKey[SERCURITYKEYLENGTH-1] = '\0';
		}

		if( eOrgMode != eChgMode )
		{
			APK_SQCNetwork_WIFI_DisconnectToAP(TR_NETWORK_DEV_WIFI);
			switch(eChgMode)
			{
				case eNETAP_WIFI_MODE_Manual:
					//hErr = APK_SQCNetwork_WIFI_ConnectToAP(TR_NETWORK_DEV_WIFI , &stApInfo, szKey);
					break;

				case eNETAP_WIFI_MODE_WpsPin:
					//hErr = APK_SQCNetwork_WIFI_ConnectToAPbyWPS(TR_NETWORK_DEV_WIFI, eNETAP_WPS_PIN, szKey);
					break;

				case eNETAP_WIFI_MODE_WpsPush:
					//hErr = APK_SQCNetwork_WIFI_ConnectToAPbyWPS(TR_NETWORK_DEV_WIFI, eNETAP_WPS_PBC, NULL);
					break;

				default:
					break;

			}
		}
	}
#endif

	REM_LEAVE(hErr);
}

static HERROR set_data_Wifi_SSID(HCHAR *pszInput)
{
	HERROR hErr = ERR_OK;
#if 0

	APKE_NETAP_WIFI_Mode_e	eMode;
	APKS_NETAP_Info_t			stApInfo;
	HCHAR					szKey[SERCURITYKEYLENGTH] = { 0 };

	RemLOG_Print("%s(%d Line)\r\n",__func__,__LINE__);

	CHECK_NULL(pszInput);

	if( APK_SQCNetwork_WIFI_bIsConnected(TR_NETWORK_DEV_WIFI) == TRUE)
	{
		APK_SQCNetwork_WIFI_GetConnectMode(&eMode);
		APK_SQCNetwork_WIFI_GetConnectedInfo(TR_NETWORK_DEV_WIFI, &stApInfo, szKey, SERCURITYKEYLENGTH );
	}
	else
	{
		eMode = s_WifiConnectMode;
		VK_memcpy(&stApInfo, &s_stNetInfo, sizeof(APKS_NETAP_Info_t));
		HxSTD_StrNCpy(szKey, s_szWifiSecurityKey, SERCURITYKEYLENGTH-1);
		szKey[SERCURITYKEYLENGTH-1] = '\0';
	}

	HxSTD_StrNCpy(s_stNetInfo.szESSID, pszInput, sizeof(s_stNetInfo.szESSID) - 1 );

	switch(eMode)
	{
		case eNETAP_WIFI_MODE_Manual:
			APK_SQCNetwork_WIFI_DisconnectToAP(TR_NETWORK_DEV_WIFI);

			HxSTD_MemSet(stApInfo.szESSID, 0x00, sizeof(stApInfo.szESSID)-1);
			HxSTD_StrNCpy(stApInfo.szESSID, pszInput, sizeof(stApInfo.szESSID) - 1 );
			//hErr = APK_SQCNetwork_WIFI_ConnectToAP(TR_NETWORK_DEV_WIFI , &stApInfo, szKey);
			break;
		/* Manual이 아니라면...??? */
		case eNETAP_WIFI_MODE_WpsPin:
		case eNETAP_WIFI_MODE_WpsPush:
		default:
			break;
	}
#endif
	REM_LEAVE(hErr);
}

static HERROR set_data_Wifi_SecurityMethod(HCHAR *pszInput)
{
	HERROR hErr = ERR_OK;

#if 0
	APKE_NETAP_WIFI_Mode_e	eMode;
	APKS_NETAP_Info_t			stApInfo;
	HCHAR					szKey[SERCURITYKEYLENGTH] = { 0 };

	RemLOG_Print("%s(%d Line)\r\n",__func__,__LINE__);

	CHECK_NULL(pszInput);

	if( APK_SQCNetwork_WIFI_bIsConnected(TR_NETWORK_DEV_WIFI) == TRUE)
	{
		APK_SQCNetwork_WIFI_GetConnectMode(&eMode);
		APK_SQCNetwork_WIFI_GetConnectedInfo(TR_NETWORK_DEV_WIFI, &stApInfo, szKey, SERCURITYKEYLENGTH );
	}
	else
	{
		eMode = s_WifiConnectMode;
		VK_memcpy(&stApInfo, &s_stNetInfo, sizeof(APKS_NETAP_Info_t));
		HxSTD_StrNCpy(szKey, s_szWifiSecurityKey, SERCURITYKEYLENGTH-1);
		szKey[SERCURITYKEYLENGTH-1] = '\0';
	}

	get_X_JCOM_Wifi_SecurityMethod_enum(&stApInfo, pszInput);

	s_stNetInfo.nSecureType = stApInfo.nSecureType;
	s_stNetInfo.nAuthenType = stApInfo.nAuthenType;
	s_stNetInfo.nEncryptType = stApInfo.nEncryptType;
	switch(eMode)
	{
		case eNETAP_WIFI_MODE_Manual:
			APK_SQCNetwork_WIFI_DisconnectToAP(TR_NETWORK_DEV_WIFI);

			//hErr = APK_SQCNetwork_WIFI_ConnectToAP(TR_NETWORK_DEV_WIFI , &stApInfo, szKey);
			break;
		/* Manual이 아니라면...??? */
		case eNETAP_WIFI_MODE_WpsPin:
		case eNETAP_WIFI_MODE_WpsPush:
		default:
			break;
	}
#endif

	REM_LEAVE(hErr);
}

static HERROR set_data_Wifi_SecurityLevel(HCHAR *pszInput)
{
	HERROR hErr = ERR_OK;
#if 0
	APKE_NETAP_WIFI_Mode_e	eMode;
	APKS_NETAP_Info_t			stApInfo;
	HCHAR					szKey[SERCURITYKEYLENGTH] = { 0 };

	RemLOG_Print("%s(%d Line)\r\n",__func__,__LINE__);

	CHECK_NULL(pszInput);

	if( APK_SQCNetwork_WIFI_bIsConnected(TR_NETWORK_DEV_WIFI) == TRUE)
	{
		APK_SQCNetwork_WIFI_GetConnectMode(&eMode);
		APK_SQCNetwork_WIFI_GetConnectedInfo(TR_NETWORK_DEV_WIFI, &stApInfo, szKey, SERCURITYKEYLENGTH );
	}
	else
	{
		eMode = s_WifiConnectMode;
		VK_memcpy(&stApInfo, &s_stNetInfo, sizeof(APKS_NETAP_Info_t));
		HxSTD_StrNCpy(szKey, s_szWifiSecurityKey, SERCURITYKEYLENGTH-1);
		szKey[SERCURITYKEYLENGTH-1] = '\0';
	}

	hErr = get_X_JCOM_Wifi_SecurityLevel_enum(&stApInfo.nEncryptType, pszInput);
	if( hErr == ERR_OK)
		s_stNetInfo.nEncryptType = stApInfo.nEncryptType;

	switch(eMode)
	{
		case eNETAP_WIFI_MODE_Manual:
			APK_SQCNetwork_WIFI_DisconnectToAP(TR_NETWORK_DEV_WIFI);

			//hErr = APK_SQCNetwork_WIFI_ConnectToAP(TR_NETWORK_DEV_WIFI , &stApInfo, szKey);
			break;
		/* Manual이 아니라면...??? */
		case eNETAP_WIFI_MODE_WpsPin:
		case eNETAP_WIFI_MODE_WpsPush:
		default:
			break;
	}
#endif
	REM_LEAVE(hErr);
}

static HERROR set_data_Wifi_SecurityKey(HCHAR *pszInput)
{
	HERROR hErr = ERR_OK;
#if 0
	APKE_NETAP_WIFI_Mode_e	eMode;
	APKS_NETAP_Info_t			stApInfo;
	HCHAR					szKey[SERCURITYKEYLENGTH] = { 0 };

	RemLOG_Print("%s(%d Line)\r\n",__func__,__LINE__);

	CHECK_NULL(pszInput);

	if( APK_SQCNetwork_WIFI_bIsConnected(TR_NETWORK_DEV_WIFI) == TRUE)
	{
		APK_SQCNetwork_WIFI_GetConnectMode(&eMode);
		APK_SQCNetwork_WIFI_GetConnectedInfo(TR_NETWORK_DEV_WIFI, &stApInfo, szKey, SERCURITYKEYLENGTH );
	}
	else
	{
		eMode = s_WifiConnectMode;
		VK_memcpy(&stApInfo, &s_stNetInfo, sizeof(APKS_NETAP_Info_t));
	}

	HxSTD_MemSet(szKey, 0x00, sizeof(szKey));
	HxSTD_StrCpy(szKey, pszInput);
	HxSTD_StrNCpy(s_szWifiSecurityKey, szKey, SERCURITYKEYLENGTH-1);
	switch(eMode)
	{
		case eNETAP_WIFI_MODE_Manual:
			APK_SQCNetwork_WIFI_DisconnectToAP(TR_NETWORK_DEV_WIFI);

			//hErr = APK_SQCNetwork_WIFI_ConnectToAP(TR_NETWORK_DEV_WIFI , &stApInfo, szKey);
			break;
		/* Manual이 아니라면...??? */
		case eNETAP_WIFI_MODE_WpsPin:
			APK_SQCNetwork_WIFI_DisconnectToAP(TR_NETWORK_DEV_WIFI);

			//hErr = APK_SQCNetwork_WIFI_ConnectToAPbyWPS(TR_NETWORK_DEV_WIFI, eNETAP_WPS_PIN, szKey);
			break;
		case eNETAP_WIFI_MODE_WpsPush:
		default:
			break;
	}
#endif
	REM_LEAVE(hErr);
}

static HERROR set_data_Wifi_KeyIndex(HCHAR *pszInput)
{
	/* Humax 에서 KeyIndex 는 무조건 1가지로 고정 사용됨*/
	RemLOG_Print("Not ready, now, can't set KeyIndex on Wifi, but return OK\r\n");
	REM_LEAVE(ERR_OK);
}

static HERROR set_data_LAN_MACAddress(HCHAR *pszInput)
{
	RemLOG_Print("Not ready, now, can't set MACAddress, but return OK\r\n");
	REM_LEAVE(ERR_OK);
}
static HERROR set_data_AudienceMeasurement_CombinedAccess(HCHAR *pszInput)
{
	HERROR hErr = ERR_OK;

	CHECK_NULL(pszInput);
	hErr = rema_DM_setData_AudienceMeasurement_CombinedAccess(pszInput);

	REM_LEAVE(hErr);
}

static HERROR set_data_AudienceMeasurement_PeriodicInformationInterval(HCHAR *pszInput)
{
	HERROR hErr = ERR_OK;

	CHECK_NULL(pszInput);
	hErr = rema_DM_setData_AudienceMeasurement_PeriodicInformationInterval(pszInput);

	REM_LEAVE(hErr);
}

static HERROR set_data_AudienceMeasurement_PeriodicInformationTime(HCHAR *pszInput)
{
	HERROR hErr = ERR_OK;

	CHECK_NULL(pszInput);
	hErr = rema_DM_setData_AudienceMeasurement_PeriodicInformationTime(pszInput);

	REM_LEAVE(hErr);
}

static HERROR set_data_AudienceMeasurement_PermissionLevel(HCHAR *pszInput)
{
	HERROR hErr = ERR_OK;

	CHECK_NULL(pszInput);
	hErr = rema_DM_setData_AudienceMeasurement_PermissionLevel(pszInput);

	REM_LEAVE(hErr);
}
#if 0
static HERROR set_LAN_IPPingDiagnostics_DiagnosticsState(HCHAR *pszInput)
{
	HERROR hErr = ERR_OK;
	RemLOG_Print("%s(%d Line)\r\n",__func__,__LINE__);



	REM_LEAVE(hErr);

}
static HERROR set_LAN_IPPingDiagnostics_Host(HCHAR *pszInput)
{
	HERROR hErr = ERR_OK;
	RemLOG_Print("%s(%d Line)\r\n",__func__,__LINE__);



	REM_LEAVE(hErr);

}
static HERROR set_LAN_IPPingDiagnostics_NumberOfRepetitions(HCHAR *pszInput)
{
	HERROR hErr = ERR_OK;
	RemLOG_Print("%s(%d Line)\r\n",__func__,__LINE__);



	REM_LEAVE(hErr);

}
static HERROR set_LAN_IPPingDiagnostics_Timeout(HCHAR *pszInput)
{
	HERROR hErr = ERR_OK;
	RemLOG_Print("%s(%d Line)\r\n",__func__,__LINE__);


	REM_LEAVE(hErr);

}
static HERROR set_LAN_IPPingDiagnostics_DataBlockSize(HCHAR *pszInput)
{
	HERROR hErr = ERR_OK;
	RemLOG_Print("%s(%d Line)\r\n",__func__,__LINE__);



	REM_LEAVE(hErr);

}
static HERROR set_LAN_IPPingDiagnostics_DSCP(HCHAR *pszInput)
{
	HERROR hErr = ERR_OK;
	RemLOG_Print("%s(%d Line)\r\n",__func__,__LINE__);



	REM_LEAVE(hErr);

}
static HERROR set_LAN_TraceRouteDiagnostics_DiagnosticsState(HCHAR *pszInput)
{
	HERROR hErr = ERR_OK;
	RemLOG_Print("%s(%d Line)\r\n",__func__,__LINE__);

	CHECK_NULL(pszInput);

	APK_3RDPARTY_CWMP_TR157_Get_CommonDM_String(pszInput,eTR135A1_DSSTB_1_Capa_MaxActiveAVStreams);


	REM_LEAVE(hErr);

}
static HERROR set_LAN_TraceRouteDiagnostics_Host(HCHAR *pszInput)
{
	HERROR hErr = ERR_OK;
	RemLOG_Print("%s(%d Line)\r\n",__func__,__LINE__);

	CHECK_NULL(pszInput);

	APK_3RDPARTY_CWMP_TR157_Get_CommonDM_String(pszInput,eTR135A1_DSSTB_1_Capa_MaxActiveAVStreams);


	REM_LEAVE(hErr);

}
static HERROR set_LAN_TraceRouteDiagnostics_Timeout(HCHAR *pszInput)
{
	HERROR hErr = ERR_OK;
	RemLOG_Print("%s(%d Line)\r\n",__func__,__LINE__);

	CHECK_NULL(pszInput);

	APK_3RDPARTY_CWMP_TR157_Get_CommonDM_String(pszInput,eTR135A1_DSSTB_1_Capa_MaxActiveAVStreams);


	REM_LEAVE(hErr);

}
static HERROR set_LAN_TraceRouteDiagnostics_DataBlockSize(HCHAR *pszInput)
{
	HERROR hErr = ERR_OK;
	RemLOG_Print("%s(%d Line)\r\n",__func__,__LINE__);

	CHECK_NULL(pszInput);

	APK_3RDPARTY_CWMP_TR157_Get_CommonDM_String(pszInput,eTR135A1_DSSTB_1_Capa_MaxActiveAVStreams);


	REM_LEAVE(hErr);

}
static HERROR set_LAN_TraceRouteDiagnostics_MaxHopCount(HCHAR *pszInput)
{
	HERROR hErr = ERR_OK;
	RemLOG_Print("%s(%d Line)\r\n",__func__,__LINE__);

	CHECK_NULL(pszInput);

	APK_3RDPARTY_CWMP_TR157_Get_CommonDM_String(pszInput,eTR135A1_DSSTB_1_Capa_MaxActiveAVStreams);


	REM_LEAVE(hErr);

}
static HERROR set_LAN_TraceRouteDiagnostics_DSCP(HCHAR *pszInput)
{
	HERROR hErr = ERR_OK;
	RemLOG_Print("%s(%d Line)\r\n",__func__,__LINE__);

	CHECK_NULL(pszInput);

	APK_3RDPARTY_CWMP_TR157_Get_CommonDM_String(pszInput,eTR135A1_DSSTB_1_Capa_MaxActiveAVStreams);


	REM_LEAVE(hErr);

}
#endif
/*******************************************************************/
/********************      Functions       *************************/
/*******************************************************************/
HERROR TRP_Get_Device_DeviceSummary(HCHAR *pszOutput)
{
	return get_data_Device_DeviceSummary(pszOutput);
}
HERROR TRP_Get_DeviceInfo_X_JCOM_SubscriberID(HCHAR *pszOutput)
{
	return get_data_DeviceInfo_X_JCOM_SubscriberID(pszOutput);
}

HERROR TRP_Get_DeviceInfo_X_JCOM_ServiceID(HCHAR *pszOutput)
{
	return get_data_DeviceInfo_X_JCOM_ServiceID(pszOutput);
}

HERROR TRP_Get_DeviceInfo_X_JCOM_FamilyID(HCHAR *pszOutput)
{
	return get_data_DeviceInfo_X_JCOM_FamilyID(pszOutput);
}
HERROR TRP_Get_DeviceInfo_X_JCOM_BusinessCode(HCHAR *pszOutput)
{
	return get_data_DeviceInfo_X_JCOM_BusinessCode(pszOutput);
}

HERROR TRP_Get_DeviceInfo_Manufacturer(HCHAR *pszOutput)
{
	return get_data_DeviceInfo_Manufacturer(pszOutput);
}


HERROR TRP_Get_DeviceInfo_ManufacturerOUI(HCHAR *pszOutput)
{
	return get_data_DeviceInfo_ManufacturerOUI(pszOutput);
}

HERROR TRP_Get_DeviceInfo_ModelName(HCHAR *pszOutput)
{
	return get_data_DeviceInfo_ModelName(pszOutput);
}

HERROR TRP_Get_DeviceInfo_Description(HCHAR *pszOutput)
{
	return get_data_DeviceInfo_Description(pszOutput);
}

HERROR TRP_Get_DeviceInfo_ProductClass(HCHAR *pszOutput)
{
	return get_data_DeviceInfo_ProductClass(pszOutput);
}

HERROR TRP_Get_DeviceInfo_SerialNumber(HCHAR *pszOutput)
{
	return get_data_DeviceInfo_SerialNumber(pszOutput);
}

HERROR TRP_Get_DeviceInfo_HardwareVersion(HCHAR *pszOutput)
{
	return get_data_DeviceInfo_HardwareVersion(pszOutput);
}

HERROR TRP_Get_DeviceInfo_AdditionalHardwareVersion(HCHAR *pszOutput)
{
	return get_data_DeviceInfo_AdditionalHardwareVersion(pszOutput);
}

HERROR TRP_Get_DeviceInfo_SoftwareVersion(HCHAR *pszOutput)
{
	return get_data_DeviceInfo_SoftwareVersion(pszOutput);
}
HERROR TRP_Get_DeviceInfo_DeviceStatus(HCHAR *pszOutput)
{
	return get_data_DeviceInfo_DeviceStatus(pszOutput);
}

HERROR TRP_Get_DeviceInfo_UpTime(HUINT32 *pszOutput)
{
	return get_data_DeviceInfo_UpTime(pszOutput);
}

HERROR TRP_Get_DeviceInfo_FirstUseDate(HCHAR *pszOutput)
{
	return get_data_DeviceInfo_FirstUseDate(pszOutput);
}

HERROR TRP_Get_ManagementServer_URL(HCHAR *pszOutput)
{
	return get_data_ManagementServer_URL(pszOutput);
}

HERROR TRP_Get_ManagementServer_Username(HCHAR *pszOutput)
{
	return get_data_ManagementServer_Username(pszOutput);
}

HERROR TRP_Get_ManagementServer_Password(HCHAR *pszOutput)
{
	return get_data_ManagementServer_Password(pszOutput);
}

HERROR TRP_Get_ManagementServer_PeriodicInformEnable(HCHAR *pszOutput)
{
	return get_data_ManagementServer_PeriodicInformEnable(pszOutput);
}

HERROR TRP_Get_ManagementServer_PeriodicInformInterval(HCHAR *pszOutput)
{
	return get_data_ManagementServer_PeriodicInformInterval(pszOutput);
}

HERROR TRP_Get_ManagementServer_PeriodicInformTime(HCHAR *pszOutput)
{
	return get_data_ManagementServer_PeriodicInformTime(pszOutput);
}


HERROR TRP_Get_ManagementServer_ParameterKey(HCHAR *pszOutput)
{
	return get_data_ManagementServer_ParameterKey(pszOutput);
}

HERROR TRP_Get_ManagementServer_ConnectionRequestURL(HCHAR *pszOutput)
{
	return get_data_ManagementServer_ConnectionRequestURL(pszOutput);
}

HERROR TRP_Get_ManagementServer_ConnectionRequestUsername(HCHAR *pszOutput)
{
	return get_data_ManagementServer_ConnectionRequestUsername(pszOutput);
}

HERROR TRP_Get_ManagementServer_ConnectionRequestPassword(HCHAR *pszOutput)
{
	return get_data_ManagementServer_ConnectionRequestPassword(pszOutput);
}

HERROR TRP_Get_ManagementServer_UpgradesManaged(HCHAR *pszOutput)
{
	return get_data_ManagementServer_UpgradesManaged(pszOutput);
}

HERROR TRP_Get_LAN_AddressingType(HCHAR *pszOutput)
{
	return get_data_LAN_AddressingType(pszOutput);
}

HERROR TRP_Get_LAN_IPAddress(HCHAR *pszOutput)
{
	return get_data_LAN_IPAddress(pszOutput);
}

HERROR TRP_Get_LAN_SubnetMask(HCHAR *pszOutput)
{
	return get_data_LAN_SubnetMask(pszOutput);
}

HERROR TRP_Get_LAN_DefaultGateway(HCHAR *pszOutput)
{
	return get_data_LAN_DefaultGateway(pszOutput);
}

HERROR TRP_Get_LAN_DNSServers(HCHAR *pszOutput)
{
	return get_data_LAN_DNSServers(pszOutput);
}

HERROR TRP_Get_LAN_MACAddress(HCHAR *pszOutput)
{
	return get_data_LAN_MACAddress(pszOutput);
}

HERROR TRP_Get_AudienceMeasurement_CombinedAccess(HCHAR *pszOutput)
{
	return get_data_AudienceMeasurement_CombinedAccess(pszOutput);
}

HERROR TRP_Get_AudienceMeasurement_PeriodicInformationInterval(HCHAR *pszOutput)
{
	return get_data_AudienceMeasurement_PeriodicInformationInterval(pszOutput);
}

HERROR TRP_Get_AudienceMeasurement_PeriodicInformationTime(HCHAR *pszOutput)
{
	return get_data_AudienceMeasurement_PeriodicInformationTime(pszOutput);
}

HERROR TRP_Get_AudienceMeasurement_PermissionLevel(HCHAR *pszOutput)
{
	return get_data_AudienceMeasurement_PermissionLevel(pszOutput);
}
HERROR TRP157_Get_SmartCardReaderNumberOfEntries(HINT32 *pszOutput)
{
	return	get_SmartCardReaderNumberOfEntries(pszOutput);
}
HERROR TRP157_Get_DevInfo_VendorConfigFileNumberOfEntries(HINT32 *pszOutput)
{
	return	get_DevInfo_VendorConfigFileNumberOfEntries(pszOutput);
}
HERROR TRP157_Get_DevInfo_AdditionalSoftwareVersion(HCHAR *pszOutput)
{
	return	get_DevInfo_AdditionalSoftwareVersion(pszOutput);
}
HERROR TRP157_Get_DevInfo_ProvisioningCode(HCHAR *pszOutput)
{
	return	get_DevInfo_ProvisioningCode(pszOutput);
}
HERROR TRP157_Get_DevInfo_UpTime(HCHAR *pszOutput)
{
	return	get_DevInfo_UpTime(pszOutput);
}
HERROR TRP157_Get_DevInfo_FirstUseDate(HCHAR *pszOutput)
{
	return	get_DevInfo_FirstUseDate(pszOutput);
}
HERROR TRP157_Get_DevInfo_DeviceLog(HCHAR *pszOutput)
{
	return	get_DevInfo_DeviceLog(pszOutput);
}
HERROR TRP157_Get_DevInfo_ProcessorNumberOfEntries(HINT32 *pszOutput)
{
	return	get_DevInfo_ProcessorNumberOfEntries(pszOutput);
}
HERROR TRP157_Get_DevInfo_VendorLogFileNumberOfEntries(HINT32 *pszOutput)
{
	return	get_DevInfo_VendorLogFileNumberOfEntries(pszOutput);
}
HERROR TRP157_Get_DevInfo_MemoryStatus_Total(HCHAR *pszOutput)
{
	return	get_DevInfo_MemoryStatus_Total(pszOutput);
}
HERROR TRP157_Get_DevInfo_MemoryStatus_Free(HCHAR *pszOutput)
{
	return	get_DevInfo_MemoryStatus_Free(pszOutput);
}
HERROR TRP157_Get_DevInfo_ProcessStatus_CPUUsage(HINT32 *pszOutput)
{
	return	get_DevInfo_ProcessStatus_CPUUsage(pszOutput);
}
HERROR TRP157_Get_DevInfo_ProcessStatus_ProcessNumberOfEntries(HINT32 *pszOutput)
{
	return	get_DevInfo_ProcessStatus_ProcessNumberOfEntries(pszOutput);
}
HERROR TRP157_Get_DevInfo_ProcessStatus_Process_M_PID(HUINT32 *pszOutput)
{
	return	get_DevInfo_ProcessStatus_Process_M_PID(pszOutput);
}
HERROR TRP157_Get_DevInfo_ProcessStatus_Process_M_Command(HCHAR *pszOutput)
{
	return	get_DevInfo_ProcessStatus_Process_M_Command(pszOutput);
}
HERROR TRP157_Get_DevInfo_ProcessStatus_Process_M_Size(HUINT32 *pszOutput)
{
	return	get_DevInfo_ProcessStatus_Process_M_Size(pszOutput);
}
HERROR TRP157_Get_DevInfo_ProcessStatus_Process_M_Priority(HUINT32 *pszOutput)
{
	return	get_DevInfo_ProcessStatus_Process_M_Priority(pszOutput);
}
HERROR TRP157_Get_DevInfo_ProcessStatus_Process_M_CPUTime(HUINT32 *pszOutput)
{
	return	get_DevInfo_ProcessStatus_Process_M_CPUTime(pszOutput);
}
HERROR TRP157_Get_DevInfo_ProcessStatus_Process_M_State(HCHAR *pszOutput)
{
	return	get_DevInfo_ProcessStatus_Process_M_State(pszOutput);
}
HERROR TRP157_Get_DevInfo_TemperatureStatus_TemperatureSensorNumberOfEntries(HINT32 *pszOutput)
{
	return	get_DevInfo_TemperatureStatus_TemperatureSensorNumberOfEntries(pszOutput);
}
HERROR TRP157_Get_DevInfo_TemperatureStatus_TemperatureSensor_M_Value(HUINT32 *pszOutput)
{
	return	get_DevInfo_TemperatureStatus_TemperatureSensor_M_Value(pszOutput);
}
HERROR TRP157_Get_DevInfo_TemperatureStatus_TemperatureSensor_M_MaxValue(HUINT32 *pszOutput)
{
	return	get_DevInfo_TemperatureStatus_TemperatureSensor_M_MaxValue(pszOutput);
}
HERROR TRP157_Get_DevInfo_VendorConfigFile_M_Name(HCHAR *pszOutput)
{
	return	get_DevInfo_VendorConfigFile_M_Name(pszOutput);
}
HERROR TRP157_Get_DevInfo_VendorConfigFile_M_Version(HCHAR *pszOutput)
{
	return	get_DevInfo_VendorConfigFile_M_Version(pszOutput);
}
HERROR TRP157_Get_DevInfo_VendorConfigFile_M_Date(HCHAR *pszOutput)
{
	return	get_DevInfo_VendorConfigFile_M_Date(pszOutput);
}
HERROR TRP157_Get_DevInfo_VendorConfigFile_M_Description(HCHAR *pszOutput)
{
	return	get_DevInfo_VendorConfigFile_M_Description(pszOutput);
}
HERROR TRP157_Get_DevInfo_Processor_M_Architecture(HCHAR *pszOutput)
{
	return	get_DevInfo_Processor_M_Architecture(pszOutput);
}
HERROR TRP157_Get_DevInfo_VendorLogFile_M_Name(HCHAR *pszOutput)
{
	return	get_DevInfo_VendorLogFile_M_Name(pszOutput);
}
HERROR TRP157_Get_DevInfo_VendorLogFile_M_MaximumSize(HCHAR *pszOutput)
{
	return	get_DevInfo_VendorLogFile_M_MaximumSize(pszOutput);
}
HERROR TRP157_Get_DevInfo_VendorLogFile_M_Persistent(HCHAR *pszOutput)
{
	return	get_DevInfo_VendorLogFile_M_Persistent(pszOutput);
}
#if 0
HERROR TRP157_Get_MgmtServer_DefaultActiveNotificationThrottle(HCHAR *pszOutput)
{
	return	get_MgmtServer_DefaultActiveNotificationThrottle(pszOutput);
}
HERROR TRP157_Get_MgmtServer_CWMPRetryMinimumWaitInterval(HCHAR *pszOutput)
{
	return	get_MgmtServer_CWMPRetryMinimumWaitInterval(pszOutput);
}
HERROR TRP157_Get_MgmtServer_CWMPRetryIntervalMultiplier(HCHAR *pszOutput)
{
	return	get_MgmtServer_CWMPRetryIntervalMultiplier(pszOutput);
}
HERROR TRP157_Get_MgmtServer_AutonomousTxCompletePolicy_Enable(HCHAR *pszOutput)
{
	return	get_MgmtServer_AutonomousTxCompletePolicy_Enable(pszOutput);
}
HERROR TRP157_Get_MgmtServer_AutonomousTxCompletePolicy_TxTypeFilter(HCHAR *pszOutput)
{
	return	get_MgmtServer_AutonomousTxCompletePolicy_TxTypeFilter(pszOutput);
}
HERROR TRP157_Get_MgmtServer_AutonomousTxCompletePolicy_FileTypeFilter(HCHAR *pszOutput)
{
	return	get_MgmtServer_AutonomousTxCompletePolicy_FileTypeFilter(pszOutput);
}
#endif
HERROR TRP157_Set_LAN_Stats_InAdvanceOfGpv_ConnectionUpTime(HBOOL hHomeNetwork)
{
#if 0
	HULONG VK_Err;
	HULONG currentTime = 0;
	
	VK_Err = VK_CLOCK_GetTime(&currentTime);
	if((VK_Err != VK_OK) || (currentTime == 0))
	{
		RemLOG_Print("Invalid current Time!!!!\n");
		g_ulHomeNetworkConnectionUpTime = 0;
		g_ulDefaultConnectionUpTime = 0;
		return ERR_FAIL;
	}

	if(hHomeNetwork){
		g_ulHomeNetworkConnectionUpTime = currentTime;
	}
	else{
		g_ulDefaultConnectionUpTime = currentTime;
	}
#endif
	return ERR_OK;
}
HERROR TRP157_Get_LAN_Stats_ConnectionUpTime(HCHAR *pszOutput)
{
	return	get_LAN_Stats_ConnectionUpTime(pszOutput);
}
HERROR TRP157_Get_LAN_Stats_TotalBytesSent(HCHAR *pszOutput)
{
	return	get_LAN_Stats_TotalBytesSent(pszOutput);
}
HERROR TRP157_Get_LAN_Stats_TotalBytesReceived(HCHAR *pszOutput)
{
	return	get_LAN_Stats_TotalBytesReceived(pszOutput);
}
HERROR TRP157_Get_LAN_Stats_TotalPacketsSent(HCHAR *pszOutput)
{
	return	get_LAN_Stats_TotalPacketsSent(pszOutput);
}
HERROR TRP157_Get_LAN_Stats_TotalPacketsReceived(HCHAR *pszOutput)
{
	return	get_LAN_Stats_TotalPacketsReceived(pszOutput);
}
HERROR TRP157_Get_LAN_Stats_CurrentDayInterval(HCHAR *pszOutput)
{
	return	get_LAN_Stats_CurrentDayInterval(pszOutput);
}
HERROR TRP157_Get_LAN_Stats_CurrentDayBytesSent(HCHAR *pszOutput)
{
	return	get_LAN_Stats_CurrentDayBytesSent(pszOutput);
}
HERROR TRP157_Get_LAN_Stats_CurrentDayBytesReceived(HCHAR *pszOutput)
{
	return	get_LAN_Stats_CurrentDayBytesReceived(pszOutput);
}
HERROR TRP157_Get_LAN_Stats_CurrentDayPacketsSent(HCHAR *pszOutput)
{
	return	get_LAN_Stats_CurrentDayPacketsSent(pszOutput);
}
HERROR TRP157_Get_LAN_Stats_CurrentDayPacketsReceived(HCHAR *pszOutput)
{
	return	get_LAN_Stats_CurrentDayPacketsReceived(pszOutput);
}
HERROR TRP157_Get_LAN_Stats_QuarterHourInterval(HCHAR *pszOutput)
{
	return	get_LAN_Stats_QuarterHourInterval(pszOutput);
}
HERROR TRP157_Get_LAN_Stats_QuarterHourBytesSent(HCHAR *pszOutput)
{
	return	get_LAN_Stats_QuarterHourBytesSent(pszOutput);
}
HERROR TRP157_Get_LAN_Stats_QuarterHourBytesReceived(HCHAR *pszOutput)
{
	return	get_LAN_Stats_QuarterHourBytesReceived(pszOutput);
}
HERROR TRP157_Get_LAN_Stats_QuarterHourPacketsSent(HCHAR *pszOutput)
{
	return	get_LAN_Stats_QuarterHourPacketsSent(pszOutput);
}
HERROR TRP157_Get_LAN_Stats_QuarterHourPacketsReceived(HCHAR *pszOutput)
{
	return	get_LAN_Stats_QuarterHourPacketsReceived(pszOutput);
}
#if 0
HERROR TRP157_Get_LAN_IPPingDiagnostics_DiagnosticsState(HCHAR *pszOutput)
{
	return	get_LAN_IPPingDiagnostics_DiagnosticsState(pszOutput);
}
HERROR TRP157_Get_LAN_IPPingDiagnostics_Host(HCHAR *pszOutput)
{
	return	get_LAN_IPPingDiagnostics_Host(pszOutput);
}
HERROR TRP157_Get_LAN_IPPingDiagnostics_NumberOfRepetitions(HCHAR *pszOutput)
{
	return	get_LAN_IPPingDiagnostics_NumberOfRepetitions(pszOutput);
}
HERROR TRP157_Get_LAN_IPPingDiagnostics_Timeout(HCHAR *pszOutput)
{
	return	get_LAN_IPPingDiagnostics_Timeout(pszOutput);
}
HERROR TRP157_Get_LAN_IPPingDiagnostics_DataBlockSize(HCHAR *pszOutput)
{
	return	get_LAN_IPPingDiagnostics_DataBlockSize(pszOutput);
}
HERROR TRP157_Get_LAN_IPPingDiagnostics_DSCP(HCHAR *pszOutput)
{
	return	get_LAN_IPPingDiagnostics_DSCP(pszOutput);
}
HERROR TRP157_Get_LAN_IPPingDiagnostics_SuccessCount(HCHAR *pszOutput)
{
	return	get_LAN_IPPingDiagnostics_SuccessCount(pszOutput);
}
HERROR TRP157_Get_LAN_IPPingDiagnostics_FailureCount(HCHAR *pszOutput)
{
	return	get_LAN_IPPingDiagnostics_FailureCount(pszOutput);
}
HERROR TRP157_Get_LAN_IPPingDiagnostics_AverageResponseTime(HCHAR *pszOutput)
{
	return	get_LAN_IPPingDiagnostics_AverageResponseTime(pszOutput);
}
HERROR TRP157_Get_LAN_IPPingDiagnostics_MinimumResponseTime(HCHAR *pszOutput)
{
	return	get_LAN_IPPingDiagnostics_MinimumResponseTime(pszOutput);
}
HERROR TRP157_Get_LAN_IPPingDiagnostics_MaximumResponseTime(HCHAR *pszOutput)
{
	return	get_LAN_IPPingDiagnostics_MaximumResponseTime(pszOutput);
}

#endif
#if 0
HERROR TRP157_Get_LAN_TraceRouteDiagnostics_DiagnosticsState(HCHAR *pszOutput)
{
	return	get_LAN_TraceRouteDiagnostics_DiagnosticsState(pszOutput);
}
HERROR TRP157_Get_LAN_TraceRouteDiagnostics_Host(HCHAR *pszOutput)
{
	return	get_LAN_TraceRouteDiagnostics_Host(pszOutput);
}
HERROR TRP157_Get_LAN_TraceRouteDiagnostics_Timeout(HCHAR *pszOutput)
{
	return	get_LAN_TraceRouteDiagnostics_Timeout(pszOutput);
}
HERROR TRP157_Get_LAN_TraceRouteDiagnostics_DataBlockSize(HCHAR *pszOutput)
{
	return	get_LAN_TraceRouteDiagnostics_DataBlockSize(pszOutput);
}
HERROR TRP157_Get_LAN_TraceRouteDiagnostics_MaxHopCount(HCHAR *pszOutput)
{
	return	get_LAN_TraceRouteDiagnostics_MaxHopCount(pszOutput);
}
HERROR TRP157_Get_LAN_TraceRouteDiagnostics_DSCP(HCHAR *pszOutput)
{
	return	get_LAN_TraceRouteDiagnostics_DSCP(pszOutput);
}
HERROR TRP157_Get_LAN_TraceRouteDiagnostics_ResponseTime(HCHAR *pszOutput)
{
	return	get_LAN_TraceRouteDiagnostics_ResponseTime(pszOutput);
}

#endif
#if 0
HERROR TRP157_Get_LAN_TraceRouteDiagnostics_NumberOfRouteHops(HCHAR *pszOutput)
{
	return	get_LAN_TraceRouteDiagnostics_NumberOfRouteHops(pszOutput);
}
HERROR TRP157_Get_LAN_TraceRouteDiagnostics_RouteHops_M_HopHost(HCHAR *pszOutput)
{
	return	get_LAN_TraceRouteDiagnostics_RouteHops_M_HopHost(pszOutput);
}

HERROR TRP157_Get_DownloadDiagnostics_DiagnosticsState(HCHAR *pszOutput)
{
	return	get_DownloadDiagnostics_DiagnosticsState(pszOutput);
}
HERROR TRP157_Get_DownloadDiagnostics_Interface(HCHAR *pszOutput)
{
	return	get_DownloadDiagnostics_Interface(pszOutput);
}
HERROR TRP157_Get_DownloadDiagnostics_DownloadURL(HCHAR *pszOutput)
{
	return	get_DownloadDiagnostics_DownloadURL(pszOutput);
}
HERROR TRP157_Get_DownloadDiagnostics_DSCP(HCHAR *pszOutput)
{
	return	get_DownloadDiagnostics_DSCP(pszOutput);
}
HERROR TRP157_Get_DownloadDiagnostics_EthernetPriority(HCHAR *pszOutput)
{
	return	get_DownloadDiagnostics_EthernetPriority(pszOutput);
}
HERROR TRP157_Get_DownloadDiagnostics_ROMTime(HCHAR *pszOutput)
{
	return	get_DownloadDiagnostics_ROMTime(pszOutput);
}
HERROR TRP157_Get_DownloadDiagnostics_BOMTime(HCHAR *pszOutput)
{
	return	get_DownloadDiagnostics_BOMTime(pszOutput);
}
HERROR TRP157_Get_DownloadDiagnostics_EOMTime(HCHAR *pszOutput)
{
	return	get_DownloadDiagnostics_EOMTime(pszOutput);
}
HERROR TRP157_Get_DownloadDiagnostics_TestBytesReceived(HCHAR *pszOutput)
{
	return	get_DownloadDiagnostics_TestBytesReceived(pszOutput);
}
HERROR TRP157_Get_DownloadDiagnostics_TotalBytesReceived(HCHAR *pszOutput)
{
	return	get_DownloadDiagnostics_TotalBytesReceived(pszOutput);
}

#endif
#if 0
HERROR TRP157_Get_UploadDiagnostics_DiagnosticsState(HCHAR *pszOutput)
{
	return	get_UploadDiagnostics_DiagnosticsState(pszOutput);
}
HERROR TRP157_Get_UploadDiagnostics_Interface(HCHAR *pszOutput)
{
	return	get_UploadDiagnostics_Interface(pszOutput);
}
HERROR TRP157_Get_UploadDiagnostics_UploadURL(HCHAR *pszOutput)
{
	return	get_UploadDiagnostics_UploadURL(pszOutput);
}
HERROR TRP157_Get_UploadDiagnostics_DSCP(HCHAR *pszOutput)
{
	return	get_UploadDiagnostics_DSCP(pszOutput);
}
HERROR TRP157_Get_UploadDiagnostics_EthernetPriority(HCHAR *pszOutput)
{
	return	get_UploadDiagnostics_EthernetPriority(pszOutput);
}
HERROR TRP157_Get_UploadDiagnostics_TestFileLength(HCHAR *pszOutput)
{
	return	get_UploadDiagnostics_TestFileLength(pszOutput);
}
HERROR TRP157_Get_UploadDiagnostics_ROMTime(HCHAR *pszOutput)
{
	return	get_UploadDiagnostics_ROMTime(pszOutput);
}
HERROR TRP157_Get_UploadDiagnostics_BOMTime(HCHAR *pszOutput)
{
	return	get_UploadDiagnostics_BOMTime(pszOutput);
}
HERROR TRP157_Get_UploadDiagnostics_EOMTime(HCHAR *pszOutput)
{
	return	get_UploadDiagnostics_EOMTime(pszOutput);
}
HERROR TRP157_Get_UploadDiagnostics_TotalBytesSent(HCHAR *pszOutput)
{
	return	get_UploadDiagnostics_TotalBytesSent(pszOutput);
}
#endif
HERROR TRP157_Get_UPnP_Device_Enable(HCHAR* pszOutput)
{
	return get_UPnP_Device_Enable(pszOutput);
}
HERROR TRP157_Get_UPnP_Device_UPnPMediaServer(HCHAR* pszOutput)
{
	return get_UPnP_Device_UPnPMediaServer(pszOutput);
}
HERROR TRP157_Get_UPnP_Device_UPnPMediaRenderer(HCHAR* pszOutput)
{
	return get_UPnP_Device_UPnPMediaRenderer(pszOutput);
}
HERROR TRP157_Get_UPnP_Device_UPnPWLANAccessPoint(HCHAR* pszOutput)
{
	return get_UPnP_Device_UPnPWLANAccessPoint(pszOutput);
}
HERROR TRP157_Get_UPnP_Device_UPnPQoSDevice(HCHAR* pszOutput)
{
	return get_UPnP_Device_UPnPQoSDevice(pszOutput);
}
HERROR TRP157_Get_UPnP_Device_UPnPQoSPolicyHolder(HCHAR* pszOutput)
{
	return get_UPnP_Device_UPnPQoSPolicyHolder(pszOutput);
}
HERROR TRP157_Get_UPnP_Device_UPnPIGD(HCHAR* pszOutput)
{
	return get_UPnP_Device_UPnPIGD(pszOutput);
}
HERROR TRP157_Get_UPnP_Device_Capa_UPnPArchitecture(HCHAR* pszOutput)
{
	return get_UPnP_Device_Capa_UPnPArchitecture(pszOutput);
}
HERROR TRP157_Get_UPnP_Device_Capa_UPnPArchitectureMinorVer(HCHAR* pszOutput)
{
	return get_UPnP_Device_Capa_UPnPArchitectureMinorVer(pszOutput);
}
HERROR TRP157_Get_UPnP_Device_Capa_UPnPMediaServer(HCHAR* pszOutput)
{
	return get_UPnP_Device_Capa_UPnPMediaServer(pszOutput);
}
HERROR TRP157_Get_UPnP_Device_Capa_UPnPMediaRenderer(HCHAR* pszOutput)
{
	return get_UPnP_Device_Capa_UPnPMediaRenderer(pszOutput);
}
HERROR TRP157_Get_UPnP_Device_Capa_UPnPWLANAccessPoint(HCHAR* pszOutput)
{
	return get_UPnP_Device_Capa_UPnPWLANAccessPoint(pszOutput);
}
HERROR TRP157_Get_UPnP_Device_Capa_UPnPBasicDevice(HCHAR* pszOutput)
{
	return get_UPnP_Device_Capa_UPnPBasicDevice(pszOutput);
}
HERROR TRP157_Get_UPnP_Device_Capa_UPnPQoSDevice(HCHAR* pszOutput)
{
	return get_UPnP_Device_Capa_UPnPQoSDevice(pszOutput);
}
HERROR TRP157_Get_UPnP_Device_Capa_UPnPQoSPolicyHolder(HCHAR* pszOutput)
{
	return get_UPnP_Device_Capa_UPnPQoSPolicyHolder(pszOutput);
}
HERROR TRP157_Get_UPnP_Device_Capa_UPnPIGD(HCHAR* pszOutput)
{
	return get_UPnP_Device_Capa_UPnPIGD(pszOutput);
}
HERROR TRP157_Get_UPnP_Discovery_RootDeviceNumberOfEntries(HINT32* pszOutput)
{
	return get_UPnP_Discovery_RootDeviceNumberOfEntries(pszOutput);
}
HERROR TRP157_Get_UPnP_Discovery_DeviceNumberOfEntries(HINT32* pszOutput)
{
	return get_UPnP_Discovery_DeviceNumberOfEntries(pszOutput);
}
HERROR TRP157_Get_UPnP_Discovery_ServiceNumberOfEntries(HINT32* pszOutput)
{
	return get_UPnP_Discovery_ServiceNumberOfEntries(pszOutput);
}
HERROR TRP157_Get_UPnP_Discovery_RootDevice_Status(HCHAR* pszOutput)
{
	return get_UPnP_Discovery_RootDevice_Status(pszOutput);
}
HERROR TRP157_Get_UPnP_Discovery_RootDevice_UUID(HCHAR* pszOutput)
{
	return get_UPnP_Discovery_RootDevice_UUID(pszOutput);
}
HERROR TRP157_Get_UPnP_Discovery_RootDevice_USN(HCHAR* pszOutput)
{
	return get_UPnP_Discovery_RootDevice_USN(pszOutput);
}
HERROR TRP157_Get_UPnP_Discovery_RootDevice_LeaseTime(HCHAR* pszOutput)
{
	return get_UPnP_Discovery_RootDevice_LeaseTime(pszOutput);
}
HERROR TRP157_Get_UPnP_Discovery_RootDevice_Location(HCHAR* pszOutput)
{
	return get_UPnP_Discovery_RootDevice_Location(pszOutput);
}
HERROR TRP157_Get_UPnP_Discovery_RootDevice_Server(HCHAR* pszOutput)
{
	return get_UPnP_Discovery_RootDevice_Server(pszOutput);
}
HERROR TRP157_Get_UPnP_Discovery_Device_Status(HCHAR* pszOutput)
{
	return get_UPnP_Discovery_Device_Status(pszOutput);
}
HERROR TRP157_Get_UPnP_Discovery_Device_UUID(HCHAR* pszOutput)
{
	return get_UPnP_Discovery_Device_UUID(pszOutput);
}
HERROR TRP157_Get_UPnP_Discovery_Device_USN(HCHAR* pszOutput)
{
	return get_UPnP_Discovery_Device_USN(pszOutput);
}
HERROR TRP157_Get_UPnP_Discovery_Device_LeaseTime(HCHAR* pszOutput)
{
	return get_UPnP_Discovery_Device_LeaseTime(pszOutput);
}
HERROR TRP157_Get_UPnP_Discovery_Device_Location(HCHAR* pszOutput)
{
	return get_UPnP_Discovery_Device_Location(pszOutput);
}
HERROR TRP157_Get_UPnP_Discovery_Device_Server(HCHAR* pszOutput)
{
	return get_UPnP_Discovery_Device_Server(pszOutput);
}
HERROR TRP157_Get_UPnP_Discovery_Service_Status(HCHAR* pszOutput)
{
	return get_UPnP_Discovery_Service_Status(pszOutput);
}
HERROR TRP157_Get_UPnP_Discovery_Service_USN(HCHAR* pszOutput)
{
	return get_UPnP_Discovery_Service_USN(pszOutput);
}
HERROR TRP157_Get_UPnP_Discovery_Service_LeaseTime(HCHAR* pszOutput)
{
	return get_UPnP_Discovery_Service_LeaseTime(pszOutput);
}
HERROR TRP157_Get_UPnP_Discovery_Service_Location(HCHAR* pszOutput)
{
	return get_UPnP_Discovery_Service_Location(pszOutput);
}
HERROR TRP157_Get_UPnP_Discovery_Service_Server(HCHAR* pszOutput)
{
	return get_UPnP_Discovery_Service_Server(pszOutput);
}

HERROR TRP157_Get_DLNA_Capabilities_HNDDeviceClass(HCHAR *pszOutput)
{
	return	get_DLNA_Capabilities_HNDDeviceClass(pszOutput);
}
HERROR TRP157_Get_DLNA_Capabilities_DeviceCapability(HCHAR *pszOutput)
{
	return	get_DLNA_Capabilities_DeviceCapability(pszOutput);
}
HERROR TRP157_Get_DLNA_Capabilities_HIDDeviceClass(HCHAR *pszOutput)
{
	return	get_DLNA_Capabilities_HIDDeviceClass(pszOutput);
}
HERROR TRP157_Get_DLNA_Capabilities_ImageClassProfileID(HCHAR *pszOutput)
{
	return	get_DLNA_Capabilities_ImageClassProfileID(pszOutput);
}
HERROR TRP157_Get_DLNA_Capabilities_AudioClassProfileID(HCHAR *pszOutput)
{
	return	get_DLNA_Capabilities_AudioClassProfileID(pszOutput);
}
HERROR TRP157_Get_DLNA_Capabilities_AVClassProfileID(HCHAR *pszOutput)
{
	return	get_DLNA_Capabilities_AVClassProfileID(pszOutput);
}
HERROR TRP157_Get_DLNA_Capabilities_MediaCollectionProfileID(HCHAR *pszOutput)
{
	return	get_DLNA_Capabilities_MediaCollectionProfileID(pszOutput);
}
HERROR TRP157_Get_DLNA_Capabilities_PrinterClassProfileID(HCHAR *pszOutput)
{
	return	get_DLNA_Capabilities_PrinterClassProfileID(pszOutput);
}
HERROR TRP157_Get_SmartCardReader_M_Enable(HCHAR *pszOutput)
{
	return	get_SmartCardReader_M_Enable(pszOutput);
}
HERROR TRP157_Get_SmartCardReader_M_Status(HCHAR *pszOutput)
{
	return	get_SmartCardReader_M_Status(pszOutput);
}
HERROR TRP157_Get_SmartCardReader_M_Name(HCHAR *pszOutput)
{
	return	get_SmartCardReader_M_Name(pszOutput);
}
HERROR TRP157_Get_SmartCardReader_M_Reset(HCHAR *pszOutput)
{
	return	get_SmartCardReader_M_Reset(pszOutput);
}
HERROR TRP157_Get_SmartCardReader_M_ResetTime(HCHAR *pszOutput)
{
	return	get_SmartCardReader_M_ResetTime(pszOutput);
}
HERROR TRP157_Get_SmartCardReader_M_SmartCard_Status(HCHAR *pszOutput)
{
	return	get_SmartCardReader_M_SmartCard_Status(pszOutput);
}
HERROR TRP157_Get_SmartCardReader_M_SmartCard_Type(HCHAR *pszOutput)
{
	return	get_SmartCardReader_M_SmartCard_Type(pszOutput);
}
HERROR TRP157_Get_SmartCardReader_M_SmartCard_Application(HCHAR *pszOutput)
{
	return	get_SmartCardReader_M_SmartCard_Application(pszOutput);
}
HERROR TRP157_Get_SmartCardReader_M_SmartCard_SerialNumber(HCHAR *pszOutput)
{
	return	get_SmartCardReader_M_SmartCard_SerialNumber(pszOutput);
}
HERROR TRP157_Get_SmartCardReader_M_SmartCard_ATR(HCHAR *pszOutput)
{
	return	get_SmartCardReader_M_SmartCard_ATR(pszOutput);
}
HERROR TRP157_Get_SmartCardReader_M_SmartCard_X_JCOM_CardType(HCHAR *pszOutput)
{
	return	get_SmartCardReader_M_SmartCard_X_JCOM_CardType(pszOutput);
}
HERROR TRP157_Get_SmartCardReader_M_SmartCard_X_JCOM_GroupIdNumberOfEntries(HINT32 *pszOutput)
{
	return	get_SmartCardReader_M_SmartCard_X_JCOM_GroupIdNumberOfEntries(pszOutput);
}
HERROR TRP157_Get_SmartCardReader_M_SmartCard_X_JCOM_GroupId_M_Number(HCHAR *pszOutput)
{
	return	get_SmartCardReader_M_SmartCard_X_JCOM_GroupId_M_Number(pszOutput);
}
HERROR TRP157_Get_SelfTestDiagnostics_DiagnosticsState(HCHAR *pszOutput)
{
	return	get_SelfTestDiagnostics_DiagnosticsState(pszOutput);
}
HERROR TRP157_Get_SelfTestDiagnostics_Results(HCHAR *pszOutput)
{
	return	get_SelfTestDiagnostics_Results(pszOutput);
}
#if 0
HERROR TRP157_Get_NSLookupDiagnostics_DiagnosticsState(HCHAR *pszOutput)
{
	return	get_NSLookupDiagnostics_DiagnosticsState(pszOutput);
}
HERROR TRP157_Get_NSLookupDiagnostics_Interface(HCHAR *pszOutput)
{
	return	get_NSLookupDiagnostics_Interface(pszOutput);
}
HERROR TRP157_Get_NSLookupDiagnostics_HostName(HCHAR *pszOutput)
{
	return	get_NSLookupDiagnostics_HostName(pszOutput);
}
HERROR TRP157_Get_NSLookupDiagnostics_DNSServer(HCHAR *pszOutput)
{
	return	get_NSLookupDiagnostics_DNSServer(pszOutput);
}
HERROR TRP157_Get_NSLookupDiagnostics_Timeout(HCHAR *pszOutput)
{
	return	get_NSLookupDiagnostics_Timeout(pszOutput);
}
HERROR TRP157_Get_NSLookupDiagnostics_NumberOfRepetitions(HCHAR *pszOutput)
{
	return	get_NSLookupDiagnostics_NumberOfRepetitions(pszOutput);
}
HERROR TRP157_Get_NSLookupDiagnostics_SuccessCount(HCHAR *pszOutput)
{
	return	get_NSLookupDiagnostics_SuccessCount(pszOutput);
}
HERROR TRP157_Get_NSLookupDiagnostics_ResultNumberOfEntries(HINT32 *pszOutput)
{
	return	get_NSLookupDiagnostics_ResultNumberOfEntries(pszOutput);
}
HERROR TRP157_Get_NSLookupDiagnostics_Result_M_Status(HCHAR *pszOutput)
{
	return	get_NSLookupDiagnostics_Result_M_Status(pszOutput);
}
HERROR TRP157_Get_NSLookupDiagnostics_Result_M_AnswerType(HCHAR *pszOutput)
{
	return	get_NSLookupDiagnostics_Result_M_AnswerType(pszOutput);
}
HERROR TRP157_Get_NSLookupDiagnostics_Result_M_HostNameReturned(HCHAR *pszOutput)
{
	return	get_NSLookupDiagnostics_Result_M_HostNameReturned(pszOutput);
}
HERROR TRP157_Get_NSLookupDiagnostics_Result_M_IPAddresses(HCHAR *pszOutput)
{
	return	get_NSLookupDiagnostics_Result_M_IPAddresses(pszOutput);
}
HERROR TRP157_Get_NSLookupDiagnostics_Result_M_DNSServerIP(HCHAR *pszOutput)
{
	return	get_NSLookupDiagnostics_Result_M_DNSServerIP(pszOutput);
}
HERROR TRP157_Get_NSLookupDiagnostics_Result_M_ResponseTime(HCHAR *pszOutput)
{
	return	get_NSLookupDiagnostics_Result_M_ResponseTime(pszOutput);
}
#endif

HERROR TRP157_Get_USBHosts_HostNumberOfEntries(HINT32 *pszOutput)
{
	return	get_USBHosts_HostNumberOfEntries(pszOutput);
}
HERROR TRP157_Get_USBHosts_Host_M_Enable(HCHAR *pszOutput)
{
	return	get_USBHosts_Host_M_Enable(pszOutput);
}
HERROR TRP157_Get_USBHosts_Host_M_Name(HCHAR *pszOutput)
{
	return	get_USBHosts_Host_M_Name(pszOutput);
}
HERROR TRP157_Get_USBHosts_Host_M_Type(HCHAR *pszOutput)
{
	return	get_USBHosts_Host_M_Type(pszOutput);
}
HERROR TRP157_Get_USBHosts_Host_M_Reset(HCHAR *pszOutput)
{
	return	get_USBHosts_Host_M_Reset(pszOutput);
}
HERROR TRP157_Get_USBHosts_Host_M_PowerManagementEnable(HCHAR *pszOutput)
{
	return	get_USBHosts_Host_M_PowerManagementEnable(pszOutput);
}
HERROR TRP157_Get_USBHosts_Host_M_USBVersion(HCHAR *pszOutput)
{
	return	get_USBHosts_Host_M_USBVersion(pszOutput);
}
HERROR TRP157_Get_USBHosts_Host_M_DeviceNumberOfEntries(HINT32 *pszOutput)
{
	return	get_USBHosts_Host_M_DeviceNumberOfEntries(pszOutput);
}
HERROR TRP157_Get_USBHosts_Host_M_Device_M_DeviceNumber(HCHAR *pszOutput)
{
	return	get_USBHosts_Host_M_Device_M_DeviceNumber(pszOutput);
}
HERROR TRP157_Get_USBHosts_Host_M_Device_M_USBVersion(HCHAR *pszOutput)
{
	return	get_USBHosts_Host_M_Device_M_USBVersion(pszOutput);
}
HERROR TRP157_Get_USBHosts_Host_M_Device_M_DeviceClass(HCHAR *pszOutput)
{
	return	get_USBHosts_Host_M_Device_M_DeviceClass(pszOutput);
}
HERROR TRP157_Get_USBHosts_Host_M_Device_M_DeviceSubClass(HCHAR *pszOutput)
{
	return	get_USBHosts_Host_M_Device_M_DeviceSubClass(pszOutput);
}
HERROR TRP157_Get_USBHosts_Host_M_Device_M_DeviceVersion(HCHAR *pszOutput)
{
	return	get_USBHosts_Host_M_Device_M_DeviceVersion(pszOutput);
}
HERROR TRP157_Get_USBHosts_Host_M_Device_M_DeviceProtocol(HCHAR *pszOutput)
{
	return	get_USBHosts_Host_M_Device_M_DeviceProtocol(pszOutput);
}
HERROR TRP157_Get_USBHosts_Host_M_Device_M_ProductID(HCHAR *pszOutput)
{
	return	get_USBHosts_Host_M_Device_M_ProductID(pszOutput);
}
HERROR TRP157_Get_USBHosts_Host_M_Device_M_VendorID(HCHAR *pszOutput)
{
	return	get_USBHosts_Host_M_Device_M_VendorID(pszOutput);
}
HERROR TRP157_Get_USBHosts_Host_M_Device_M_Manufacturer(HCHAR *pszOutput)
{
	return	get_USBHosts_Host_M_Device_M_Manufacturer(pszOutput);
}
HERROR TRP157_Get_USBHosts_Host_M_Device_M_ProductClass(HCHAR *pszOutput)
{
	return	get_USBHosts_Host_M_Device_M_ProductClass(pszOutput);
}
HERROR TRP157_Get_USBHosts_Host_M_Device_M_SerialNumber(HCHAR *pszOutput)
{
	return	get_USBHosts_Host_M_Device_M_SerialNumber(pszOutput);
}
HERROR TRP157_Get_USBHosts_Host_M_Device_M_Port(HCHAR *pszOutput)
{
	return	get_USBHosts_Host_M_Device_M_Port(pszOutput);
}
HERROR TRP157_Get_USBHosts_Host_M_Device_M_Rate(HCHAR *pszOutput)
{
	return	get_USBHosts_Host_M_Device_M_Rate(pszOutput);
}
HERROR TRP157_Get_USBHosts_Host_M_Device_M_Parent(HCHAR *pszOutput)
{
	return	get_USBHosts_Host_M_Device_M_Parent(pszOutput);
}
HERROR TRP157_Get_USBHosts_Host_M_Device_M_MaxChildren(HCHAR *pszOutput)
{
	return	get_USBHosts_Host_M_Device_M_MaxChildren(pszOutput);
}
HERROR TRP157_Get_USBHosts_Host_M_Device_M_IsSuspended(HCHAR *pszOutput)
{
	return	get_USBHosts_Host_M_Device_M_IsSuspended(pszOutput);
}
HERROR TRP157_Get_USBHosts_Host_M_Device_M_IsSelfPowered(HCHAR *pszOutput)
{
	return	get_USBHosts_Host_M_Device_M_IsSelfPowered(pszOutput);
}
HERROR TRP157_Get_USBHosts_Host_M_Device_M_ConfigurationNumberOfEntries(HINT32 *pszOutput)
{
	return	get_USBHosts_Host_M_Device_M_ConfigurationNumberOfEntries(pszOutput);
}
HERROR TRP157_Get_USBHosts_Host_M_Device_M_Config_M_ConfigurationNumber(HCHAR *pszOutput)
{
	return	get_USBHosts_Host_M_Device_M_Config_M_ConfigurationNumber(pszOutput);
}
HERROR TRP157_Get_USBHosts_Host_M_Device_M_Config_M_InterfaceNumberOfEntries(HINT32 *pszOutput)
{
	return	get_USBHosts_Host_M_Device_M_Config_M_InterfaceNumberOfEntries(pszOutput);
}
HERROR TRP157_Get_USBHosts_Host_M_Device_M_Config_M_Interface_M_InterfaceNumber(HCHAR *pszOutput)
{
	return	get_USBHosts_Host_M_Device_M_Config_M_Interface_M_InterfaceNumber(pszOutput);
}
HERROR TRP157_Get_USBHosts_Host_M_Device_M_Config_M_Interface_M_InterfaceClass(HCHAR *pszOutput)
{
	return	get_USBHosts_Host_M_Device_M_Config_M_Interface_M_InterfaceClass(pszOutput);
}
HERROR TRP157_Get_USBHosts_Host_M_Device_M_Config_M_Interface_M_InterfaceSubClass(HCHAR *pszOutput)
{
	return	get_USBHosts_Host_M_Device_M_Config_M_Interface_M_InterfaceSubClass(pszOutput);
}
HERROR TRP157_Get_USBHosts_Host_M_Device_M_Config_M_Interface_M_InterfaceProtocol(HCHAR *pszOutput)
{
	return	get_USBHosts_Host_M_Device_M_Config_M_Interface_M_InterfaceProtocol(pszOutput);
}
HERROR TRP157_Get_PeriodicStatistics_MinSampleInterval(HCHAR *pszOutput)
{
	return	get_PeriodicStatistics_MinSampleInterval(pszOutput);
}
HERROR TRP157_Get_PeriodicStatistics_MaxReportSamples(HCHAR *pszOutput)
{
	return	get_PeriodicStatistics_MaxReportSamples(pszOutput);
}
HERROR TRP157_Get_PeriodicStatistics_SampleSetNumberOfEntries(HINT32 *pszOutput)
{
	return	get_PeriodicStatistics_SampleSetNumberOfEntries(pszOutput);
}
HERROR TRP157_Get_PeriodicStatistics_SampleSet_M_Enable(HCHAR *pszOutput)
{
	return	get_PeriodicStatistics_SampleSet_M_Enable(pszOutput);
}
HERROR TRP157_Get_PeriodicStatistics_SampleSet_M_Status(HCHAR *pszOutput)
{
	return	get_PeriodicStatistics_SampleSet_M_Status(pszOutput);
}
HERROR TRP157_Get_PeriodicStatistics_SampleSet_M_Name(HCHAR *pszOutput)
{
	return	get_PeriodicStatistics_SampleSet_M_Name(pszOutput);
}
HERROR TRP157_Get_PeriodicStatistics_SampleSet_M_SampleInterval(HCHAR *pszOutput)
{
	return	get_PeriodicStatistics_SampleSet_M_SampleInterval(pszOutput);
}
HERROR TRP157_Get_PeriodicStatistics_SampleSet_M_ReportSamples(HCHAR *pszOutput)
{
	return	get_PeriodicStatistics_SampleSet_M_ReportSamples(pszOutput);
}
HERROR TRP157_Get_PeriodicStatistics_SampleSet_M_TimeReference(HCHAR *pszOutput)
{
	return	get_PeriodicStatistics_SampleSet_M_TimeReference(pszOutput);
}
HERROR TRP157_Get_PeriodicStatistics_SampleSet_M_FetchSamples(HCHAR *pszOutput)
{
	return	get_PeriodicStatistics_SampleSet_M_FetchSamples(pszOutput);
}
HERROR TRP157_Get_PeriodicStatistics_SampleSet_M_ForceSample(HCHAR *pszOutput)
{
	return	get_PeriodicStatistics_SampleSet_M_ForceSample(pszOutput);
}
HERROR TRP157_Get_PeriodicStatistics_SampleSet_M_ReportStartTime(HCHAR *pszOutput)
{
	return	get_PeriodicStatistics_SampleSet_M_ReportStartTime(pszOutput);
}
HERROR TRP157_Get_PeriodicStatistics_SampleSet_M_ReportEndTime(HCHAR *pszOutput)
{
	return	get_PeriodicStatistics_SampleSet_M_ReportEndTime(pszOutput);
}
HERROR TRP157_Get_PeriodicStatistics_SampleSet_M_SampleSeconds(HCHAR *pszOutput)
{
	return	get_PeriodicStatistics_SampleSet_M_SampleSeconds(pszOutput);
}
HERROR TRP157_Get_PeriodicStatistics_SampleSet_M_ParameterNumberOfEntries(HINT32 *pszOutput)
{
	return	get_PeriodicStatistics_SampleSet_M_ParameterNumberOfEntries(pszOutput);
}
HERROR TRP157_Get_PeriodicStatistics_SampleSet_M_Parameter_M_Enable(HCHAR *pszOutput)
{
	return	get_PeriodicStatistics_SampleSet_M_Parameter_M_Enable(pszOutput);
}
HERROR TRP157_Get_PeriodicStatistics_SampleSet_M_Parameter_M_Reference(HCHAR *pszOutput)
{
	return	get_PeriodicStatistics_SampleSet_M_Parameter_M_Reference(pszOutput);
}
HERROR TRP157_Get_PeriodicStatistics_SampleSet_M_Parameter_M_SampleMode(HCHAR *pszOutput)
{
	return	get_PeriodicStatistics_SampleSet_M_Parameter_M_SampleMode(pszOutput);
}
HERROR TRP157_Get_PeriodicStatistics_SampleSet_M_Parameter_M_CalculationMode(HCHAR *pszOutput)
{
	return	get_PeriodicStatistics_SampleSet_M_Parameter_M_CalculationMode(pszOutput);
}
HERROR TRP157_Get_PeriodicStatistics_SampleSet_M_Parameter_M_LowThreshold(HCHAR *pszOutput)
{
	return	get_PeriodicStatistics_SampleSet_M_Parameter_M_LowThreshold(pszOutput);
}
HERROR TRP157_Get_PeriodicStatistics_SampleSet_M_Parameter_M_HighThreshold(HCHAR *pszOutput)
{
	return	get_PeriodicStatistics_SampleSet_M_Parameter_M_HighThreshold(pszOutput);
}
HERROR TRP157_Get_PeriodicStatistics_SampleSet_M_Parameter_M_SampleSeconds(HCHAR *pszOutput)
{
	return	get_PeriodicStatistics_SampleSet_M_Parameter_M_SampleSeconds(pszOutput);
}
HERROR TRP157_Get_PeriodicStatistics_SampleSet_M_Parameter_M_SuspectData(HCHAR *pszOutput)
{
	return	get_PeriodicStatistics_SampleSet_M_Parameter_M_SuspectData(pszOutput);
}
HERROR TRP157_Get_PeriodicStatistics_SampleSet_M_Parameter_M_Values(HCHAR *pszOutput)
{
	return	get_PeriodicStatistics_SampleSet_M_Parameter_M_Values(pszOutput);
}
HERROR TRP157_Get_PeriodicStatistics_SampleSet_M_Parameter_M_Failures(HCHAR *pszOutput)
{
	return	get_PeriodicStatistics_SampleSet_M_Parameter_M_Failures(pszOutput);
}
HERROR TRP157_Get_SoftwareModules_ExecEnvNumberOfEntries(HINT32 *pszOutput)
{
	return	get_SoftwareModules_ExecEnvNumberOfEntries(pszOutput);
}
HERROR TRP157_Get_SoftwareModules_ExecEnv_M_Enable(HCHAR *pszOutput)
{
	return	get_SoftwareModules_ExecEnv_M_Enable(pszOutput);
}
HERROR TRP157_Get_SoftwareModules_ExecEnv_M_Status(HCHAR *pszOutput)
{
	return	get_SoftwareModules_ExecEnv_M_Status(pszOutput);
}
HERROR TRP157_Get_SoftwareModules_ExecEnv_M_Name(HCHAR *pszOutput)
{
	return	get_SoftwareModules_ExecEnv_M_Name(pszOutput);
}
HERROR TRP157_Get_SoftwareModules_ExecEnv_M_Type(HCHAR *pszOutput)
{
	return	get_SoftwareModules_ExecEnv_M_Type(pszOutput);
}
HERROR TRP157_Get_SoftwareModules_ExecEnv_M_Vendor(HCHAR *pszOutput)
{
	return	get_SoftwareModules_ExecEnv_M_Vendor(pszOutput);
}
HERROR TRP157_Get_SoftwareModules_ExecEnv_M_Version(HCHAR *pszOutput)
{
	return	get_SoftwareModules_ExecEnv_M_Version(pszOutput);
}
HERROR TRP157_Get_SoftwareModules_ExecEnv_M_AllocatedDiskSpace(HCHAR *pszOutput)
{
	return	get_SoftwareModules_ExecEnv_M_AllocatedDiskSpace(pszOutput);
}
HERROR TRP157_Get_SoftwareModules_ExecEnv_M_AvailableDiskSpace(HCHAR *pszOutput)
{
	return	get_SoftwareModules_ExecEnv_M_AvailableDiskSpace(pszOutput);
}
HERROR TRP157_Get_SoftwareModules_ExecEnv_M_AllocatedMemory(HCHAR *pszOutput)
{
	return	get_SoftwareModules_ExecEnv_M_AllocatedMemory(pszOutput);
}
HERROR TRP157_Get_SoftwareModules_ExecEnv_M_AvailableMemory(HCHAR *pszOutput)
{
	return	get_SoftwareModules_ExecEnv_M_AvailableMemory(pszOutput);
}
HERROR TRP157_Get_SoftwareModules_ExecEnv_M_ActiveExecutionUnits(HCHAR *pszOutput)
{
	return	get_SoftwareModules_ExecEnv_M_ActiveExecutionUnits(pszOutput);
}
#if 0
HERROR TRP157_Get_X_JLABS_Management_AudienceMeasurement_CombinedAccess(HCHAR *pszOutput)
{
	return	get_X_JLABS_Management_AudienceMeasurement_CombinedAccess(pszOutput);
}
HERROR TRP157_Get_X_JLABS_Management_AudienceMeasurement_PeriodicInformationInterval(HCHAR *pszOutput)
{
	return	get_X_JLABS_Management_AudienceMeasurement_PeriodicInformationInterval(pszOutput);
}
HERROR TRP157_Get_X_JLABS_Management_AudienceMeasurement_PeriodicInformationTime(HCHAR *pszOutput)
{
	return	get_X_JLABS_Management_AudienceMeasurement_PeriodicInformationTime(pszOutput);
}
HERROR TRP157_Get_X_JLABS_Management_AudienceMeasurement_PermissionLevel(HCHAR *pszOutput)
{
	return	get_X_JLABS_Management_AudienceMeasurement_PermissionLevel(pszOutput);
}
#endif
HERROR TRP157_Get_X_JCOM_HomeNet_Enable(HCHAR *pszOutput)
{
	return	get_X_JCOM_HomeNet_Enable(pszOutput);
}
HERROR TRP157_Get_X_JCOM_HomeNet_Status(HCHAR *pszOutput)
{
	return	get_X_JCOM_HomeNet_Status(pszOutput);
}
HERROR TRP157_Get_X_JCOM_HomeNet_AddressingType(HCHAR *pszOutput)
{
	return get_X_JCOM_HomeNet_AddressingType(pszOutput);
}
HERROR TRP157_Get_X_JCOM_HomeNet_IPAddress(HCHAR *pszOutput)
{
	return get_X_JCOM_HomeNet_IPAddress(pszOutput);
}
HERROR TRP157_Get_X_JCOM_HomeNet_SubnetMask(HCHAR *pszOutput)
{
	return get_X_JCOM_HomeNet_SubnetMask(pszOutput);
}
HERROR TRP157_Get_X_JCOM_HomeNet_DefaultGateway(HCHAR *pszOutput)
{
	return get_X_JCOM_HomeNet_DefaultGateway(pszOutput);
}
HERROR TRP157_Get_X_JCOM_HomeNet_DNSServers(HCHAR *pszOutput)
{
	return get_X_JCOM_HomeNet_DNSServers(pszOutput);
}
HERROR TRP157_Get_X_JCOM_HomeNet_MACAddress(HCHAR *pszOutput)
{
	return	get_X_JCOM_HomeNet_MACAddress(pszOutput);
}
HERROR TRP157_Get_X_JCOM_HomeNet_Stats_ConnectionUpTime(HCHAR *pszOutput)
{
	return	get_X_JCOM_HomeNet_Stats_ConnectionUpTime(pszOutput);
}
HERROR TRP157_Get_X_JCOM_HomeNet_Stats_TotalBytesSent(HCHAR *pszOutput)
{
	return	get_X_JCOM_HomeNet_Stats_TotalBytesSent(pszOutput);
}
HERROR TRP157_Get_X_JCOM_HomeNet_Stats_TotalBytesReceived(HCHAR *pszOutput)
{
	return	get_X_JCOM_HomeNet_Stats_TotalBytesReceived(pszOutput);
}
HERROR TRP157_Get_X_JCOM_HomeNet_Stats_TotalPacketsSent(HCHAR *pszOutput)
{
	return	get_X_JCOM_HomeNet_Stats_TotalPacketsSent(pszOutput);
}
HERROR TRP157_Get_X_JCOM_HomeNet_Stats_TotalPacketsReceived(HCHAR *pszOutput)
{
	return	get_X_JCOM_HomeNet_Stats_TotalPacketsReceived(pszOutput);
}
HERROR TRP157_Get_X_JCOM_Wifi_Enable(HCHAR *pszOutput)
{
	return	get_X_JCOM_Wifi_Enable(pszOutput);
}
HERROR TRP157_Get_X_JCOM_Wifi_ConfigMethod(HCHAR *pszOutput)
{
	return	get_X_JCOM_Wifi_ConfigMethod(pszOutput);
}
HERROR TRP157_Get_X_JCOM_Wifi_SSID(HCHAR *pszOutput)
{
	return	get_X_JCOM_Wifi_SSID(pszOutput);
}
HERROR TRP157_Get_X_JCOM_Wifi_SecurityMethod(HCHAR *pszOutput)
{
	return	get_X_JCOM_Wifi_SecurityMethod(pszOutput);
}
HERROR TRP157_Get_X_JCOM_Wifi_SecurityLevel(HCHAR *pszOutput)
{
	return	get_X_JCOM_Wifi_SecurityLevel(pszOutput);
}
HERROR TRP157_Get_X_JCOM_Wifi_SecurityKey(HCHAR *pszOutput)
{
	return	get_X_JCOM_Wifi_SecurityKey(pszOutput);
}
HERROR TRP157_Get_X_JCOM_Wifi_KeyIndex(HCHAR *pszOutput)
{
	return	get_X_JCOM_Wifi_KeyIndex(pszOutput);
}
HERROR TRP157_Get_X_JCOM_Wifi_AddressingType(HCHAR *pszOutput)
{
	return	get_X_JCOM_Wifi_AddressingType(pszOutput);
}
HERROR TRP157_Get_X_JCOM_Wifi_IPAddress(HCHAR *pszOutput)
{
	return	get_X_JCOM_Wifi_IPAddress(pszOutput);
}
HERROR TRP157_Get_X_JCOM_Wifi_SubnetMask(HCHAR *pszOutput)
{
	return	get_X_JCOM_Wifi_SubnetMask(pszOutput);
}
HERROR TRP157_Get_X_JCOM_Wifi_DefaultGateway(HCHAR *pszOutput)
{
	return	get_X_JCOM_Wifi_DefaultGateway(pszOutput);
}
HERROR TRP157_Get_Capa_PerformanceDiagnostic_DownloadTransports(HCHAR *pszOutput)
{
	return	get_Capa_PerformanceDiagnostic_DownloadTransports(pszOutput);
}
HERROR TRP157_Get_Capa_PerformanceDiagnostic_UploadTransports(HCHAR *pszOutput)
{
	return	get_Capa_PerformanceDiagnostic_UploadTransports(pszOutput);
}






#define __________STATIC_FUNCTION_TRP_SET_______________________________________

HERROR TRP_SET_DeviceInfo_X_JCOM_SubscriberID(HCHAR *pszInput)
{
	return set_data_DeviceInfo_X_JCOM_SubscriberID(pszInput);
}

HERROR TRP_SET_DeviceInfo_X_JCOM_ServiceID(HCHAR *pszInput)
{
	return set_data_DeviceInfo_X_JCOM_ServiceID(pszInput);
}
HERROR TRP_SET_DeviceInfo_X_JCOM_FamilyID(HCHAR *pszInput)
{
	return set_data_DeviceInfo_X_JCOM_FamilyID(pszInput);
}
HERROR TRP_SET_DeviceInfo_X_JCOM_BusinessCode(HCHAR *pszInput)
{
	return set_data_DeviceInfo_X_JCOM_BusinessCode(pszInput);
}
HERROR TRP_SET_ManagementServer_URL(HCHAR *pszInput)
{
	return set_data_ManagementServer_URL(pszInput);
}

HERROR TRP_SET_ManagementServer_Username(HCHAR *pszInput)
{
	return set_data_ManagementServer_Username(pszInput);
}

HERROR TRP_SET_ManagementServer_Password(HCHAR *pszInput)
{
	return set_data_ManagementServer_Password(pszInput);
}

HERROR TRP_SET_ManagementServer_PeriodicInformEnable(HCHAR *pszInput)
{
	return set_data_ManagementServer_PeriodicInformEnable(pszInput);
}

HERROR TRP_SET_ManagementServer_PeriodicInformInterval(HCHAR *pszInput)
{
	return set_data_ManagementServer_PeriodicInformInterval(pszInput);
}

HERROR TRP_SET_ManagementServer_PeriodicInformTime(HCHAR *pszInput)
{
	return set_data_ManagementServer_PeriodicInformTime(pszInput);
}
HERROR TRP_SET_ManagementServer_ConnectionRequestURL(HCHAR *pszInput)
{
	return set_data_ManagementServer_ConnectionRequestURL(pszInput);
}
HERROR TRP_SET_ManagementServer_ConnectionRequestUsername(HCHAR *pszInput)
{
	return set_data_ManagementServer_ConnectionRequestUsername(pszInput);
}

HERROR TRP_SET_ManagementServer_ConnectionRequestPassword(HCHAR *pszInput)
{
	return set_data_ManagementServer_ConnectionRequestPassword(pszInput);
}

HERROR TRP_SET_ManagementServer_UpgradesManaged(HCHAR *pszInput)
{
	return set_data_ManagementServer_UpgradesManaged(pszInput);
}

HERROR TRP_SET_LAN_AddressingType(HCHAR *pszInput)
{
	return set_data_LAN_AddressingType(pszInput);
}

HERROR TRP_SET_LAN_IPAddress(HCHAR *pszInput)
{
	return set_data_LAN_IPAddress(pszInput);
}

HERROR TRP_SET_LAN_SubnetMask(HCHAR *pszInput)
{
	return set_data_LAN_SubnetMask(pszInput);
}

HERROR TRP_SET_LAN_DefaultGateway(HCHAR *pszInput)
{
	return set_data_LAN_DefaultGateway(pszInput);
}

HERROR TRP_SET_LAN_DNSServers(HCHAR *pszInput)
{
	return set_data_LAN_DNSServers(pszInput);
}
HERROR TRP_SET_LAN_MACAddress(HCHAR *pszInput)
{
	return set_data_LAN_MACAddress(pszInput);
}

HERROR TRP_SET_AudienceMeasurement_CombinedAccess(HCHAR *pszInput)
{
	return set_data_AudienceMeasurement_CombinedAccess(pszInput);
}

HERROR TRP_SET_AudienceMeasurement_PeriodicInformationInterval(HCHAR *pszInput)
{
	return set_data_AudienceMeasurement_PeriodicInformationInterval(pszInput);
}

HERROR TRP_SET_AudienceMeasurement_PeriodicInformationTime(HCHAR *pszInput)
{
	return set_data_AudienceMeasurement_PeriodicInformationTime(pszInput);
}

HERROR TRP_SET_AudienceMeasurement_PermissionLevel(HCHAR *pszInput)
{
	return set_data_AudienceMeasurement_PermissionLevel(pszInput);
}
HERROR TRP157_Set_UPnP_Device_UPnPMediaServer(HCHAR* pszInput)
{
	TRP135_Set_JCOM_Config_HomeServer_Enable(pszInput);
	return ERR_OK;
}
HERROR TRP157_Set_UPnP_Device_UPnPMediaRenderer(HCHAR* pszInput)
{
#if 0
	HCHAR		szFriendlyName[128];
	HERROR		hErr;
	if(HxSTD_StrNCmp(pszInput, "true", 4) == 0)
	{
		HxSTD_MemSet(szFriendlyName, 0, 128);
		hErr = APK_DLNADMS_GetDmsDbFriendlyName(szFriendlyName, 128);
		APK_DLNADMR_Start(szFriendlyName);
	}
	else
	{
		APK_DLNADMR_Stop();
	}
	return ERR_OK;
#else
	return ERR_FAIL;
#endif

}
HERROR TRP157_Set_UPnP_Device_Enable(HCHAR* pszInput)
{
	TRP157_Set_UPnP_Device_UPnPMediaServer(pszInput);
	TRP157_Set_UPnP_Device_UPnPMediaRenderer(pszInput);
	return ERR_OK;
}
HERROR TRP157_Set_UPnP_Device_UPnPWLANAccessPoint(HCHAR* pszInput)
{
	RemLOG_Print("Not Supported, but return OK(%s)\r\n",__func__);
	return ERR_OK;
}
HERROR TRP157_Set_UPnP_Device_UPnPQoSDevice(HCHAR* pszInput)
{
	RemLOG_Print("Not Supported, but return OK(%s)\r\n",__func__);
	return ERR_OK;
}
HERROR TRP157_Set_UPnP_Device_UPnPQoSPolicyHolder(HCHAR* pszInput)
{
	RemLOG_Print("Not Supported, but return OK(%s)\r\n",__func__);
	return ERR_OK;
}
HERROR TRP157_Set_UPnP_Device_UPnPIGD(HCHAR* pszInput)
{
	RemLOG_Print("Not Supported, but return OK(%s)\r\n",__func__);
	return ERR_OK;
}
HERROR TRP157_Set_SmartCardReader_M_Enable(HCHAR *pszInput)
{
	RemLOG_Print("Not Supported, but return OK(%s)\r\n",__func__);
	return ERR_OK;
}
HERROR TRP157_Set_SmartCardReader_M_Reset(HCHAR *pszInput)
{
#if 0
	HINT32 nArg1 = 0;
	rema_DM_MultiInstance_GetEntryCount(eTR157A3_Device_SmartCardReaderNumberOfEntries,&nArg1);
	APK_3RDPARTY_CWMP_TR157_Set_D1(pszInput,eTR157A3_Device_SmartCardReader_M_Reset,nArg1);
	return ERR_OK;
#else
	return ERR_FAIL;
#endif
}
HERROR TRP157_Set_USBHosts_Host_M_Enable(HCHAR *pszInput)
{
	RemLOG_Print("Not Supported, but return OK(%s)\r\n",__func__);
	return ERR_OK;
}
HERROR TRP157_Set_USBHosts_Host_M_Reset(HCHAR *pszInput)
{
	RemLOG_Print("Not Supported, but return OK(%s)\r\n",__func__);
	return ERR_OK;
}
HERROR TRP157_Set_USBHosts_Host_M_PowerManagementEnable(HCHAR *pszInput)
{
	RemLOG_Print("Not Supported, but return OK(%s)\r\n",__func__);
	return ERR_OK;
}
HERROR TRP157_Set_DevInfo_ProvisioningCode(HCHAR *pszInput)
{
#if 0
	APK_3RDPARTY_CWMP_TR157_Set(pszInput,eTR157A3_Device_DeviceInfo_ProvisioningCode);
	return ERR_OK;
#else
	return ERR_FAIL;
#endif
}
HERROR TRP157_Set_PeriodicStatistics_SampleSet_M_Parameter_M_Enable(HCHAR *pszInput)
{
	RemLOG_Print("Not Supported, but return OK(%s)\r\n",__func__);
	return ERR_OK;
}
HERROR TRP157_Set_PeriodicStatistics_SampleSet_M_Parameter_M_Reference(HCHAR *pszInput)
{
	RemLOG_Print("Not Supported, but return OK(%s)\r\n",__func__);
	return ERR_OK;
}
HERROR TRP157_Set_PeriodicStatistics_SampleSet_M_Parameter_M_SampleMode(HCHAR *pszInput)
{
	RemLOG_Print("Not Supported, but return OK(%s)\r\n",__func__);
	return ERR_OK;
}
HERROR TRP157_Set_PeriodicStatistics_SampleSet_M_Parameter_M_CalculationMode(HCHAR *pszInput)
{
	RemLOG_Print("Not Supported, but return OK(%s)\r\n",__func__);
	return ERR_OK;
}
HERROR TRP157_Set_PeriodicStatistics_SampleSet_M_Parameter_M_LowThreshold(HCHAR *pszInput)
{
	RemLOG_Print("Not Supported, but return OK(%s)\r\n",__func__);
	return ERR_OK;
}
HERROR TRP157_Set_PeriodicStatistics_SampleSet_M_Parameter_M_HighThreshold(HCHAR *pszInput)
{
	RemLOG_Print("Not Supported, but return OK(%s)\r\n",__func__);
	return ERR_OK;
}
HERROR TRP157_Set_PeriodicStatistics_SampleSet_M_Enable(HCHAR *pszInput)
{
	RemLOG_Print("Not Supported, but return OK(%s)\r\n",__func__);
	return ERR_OK;
}
HERROR TRP157_Set_PeriodicStatistics_SampleSet_M_Status(HCHAR *pszInput)
{
	RemLOG_Print("Not Supported, but return OK(%s)\r\n",__func__);
	return ERR_OK;
}
HERROR TRP157_Set_PeriodicStatistics_SampleSet_M_Name(HCHAR *pszInput)
{
	RemLOG_Print("Not Supported, but return OK(%s)\r\n",__func__);
	return ERR_OK;
}
HERROR TRP157_Set_PeriodicStatistics_SampleSet_M_SampleInterval(HCHAR *pszInput)
{
	RemLOG_Print("Not Supported, but return OK(%s)\r\n",__func__);
	return ERR_OK;
}
HERROR TRP157_Set_PeriodicStatistics_SampleSet_M_ReportSamples(HCHAR *pszInput)
{
	RemLOG_Print("Not Supported, but return OK(%s)\r\n",__func__);
	return ERR_OK;
}
HERROR TRP157_Set_PeriodicStatistics_SampleSet_M_TimeReference(HCHAR *pszInput)
{
	RemLOG_Print("Not Supported, but return OK(%s)\r\n",__func__);
	return ERR_OK;
}
HERROR TRP157_Set_PeriodicStatistics_SampleSet_M_FetchSamples(HCHAR *pszInput)
{
	RemLOG_Print("Not Supported, but return OK(%s)\r\n",__func__);
	return ERR_OK;
}
HERROR TRP157_Set_PeriodicStatistics_SampleSet_M_ForceSample(HCHAR *pszInput)
{
	RemLOG_Print("Not Supported, but return OK(%s)\r\n",__func__);
	return ERR_OK;
}
HERROR TRP157_Set_PeriodicStatistics_SampleSet_M_ReportStartTime(HCHAR *pszInput)
{
	RemLOG_Print("Not Supported, but return OK(%s)\r\n",__func__);
	return ERR_OK;
}
HERROR TRP157_Set_PeriodicStatistics_SampleSet_M_ReportEndTime(HCHAR *pszInput)
{
	RemLOG_Print("Not Supported, but return OK(%s)\r\n",__func__);
	return ERR_OK;
}
HERROR TRP157_Set_PeriodicStatistics_SampleSet_M_SampleSeconds(HCHAR *pszInput)
{
	RemLOG_Print("Not Supported, but return OK(%s)\r\n",__func__);
	return ERR_OK;
}
HERROR TRP157_Set_X_JCOM_HomeNet_Enable(HCHAR *pszInput)
{
	return set_data_X_JCOM_HomeNet_Enable(pszInput);
}
HERROR TRP157_Set_X_JCOM_HomeNet_AddressingType(HCHAR *pszInput)
{
	return set_data_X_JCOM_HomeNet_AddressingType(pszInput);
}
HERROR TRP157_Set_X_JCOM_HomeNet_IPAddress(HCHAR *pszInput)
{
	return set_data_X_JCOM_HomeNet_IPAddress(pszInput);
}
HERROR TRP157_Set_X_JCOM_HomeNet_SubnetMask(HCHAR *pszInput)
{
	return set_data_X_JCOM_HomeNet_SubnetMask(pszInput);
}
HERROR TRP157_Set_X_JCOM_HomeNet_DefaultGateway(HCHAR *pszInput)
{
	return set_data_X_JCOM_HomeNet_DefaultGateway(pszInput);
}
HERROR TRP157_Set_X_JCOM_HomeNet_DNSServers(HCHAR *pszInput)
{
	return set_data_X_JCOM_HomeNet_DNSServers(pszInput);
}
HERROR TRP157_Set_X_JCOM_Wifi_Enable(HCHAR *pszInput)
{
	return set_data_Wifi_Enable(pszInput);
}
HERROR TRP157_Set_X_JCOM_Wifi_ConfigMethod(HCHAR *pszInput)
{
	return set_data_Wifi_ConfigMethod(pszInput);
}
HERROR TRP157_Set_X_JCOM_Wifi_SSID(HCHAR *pszInput)
{
 	return set_data_Wifi_SSID(pszInput);
}
HERROR TRP157_Set_X_JCOM_Wifi_SecurityMethod(HCHAR *pszInput)
{
 	return set_data_Wifi_SecurityMethod(pszInput);
}
HERROR TRP157_Set_X_JCOM_Wifi_SecurityLevel(HCHAR *pszInput)
{
 	return set_data_Wifi_SecurityLevel(pszInput);
}
HERROR TRP157_Set_X_JCOM_Wifi_SecurityKey(HCHAR *pszInput)
{
 	return set_data_Wifi_SecurityKey(pszInput);
}
HERROR TRP157_Set_X_JCOM_Wifi_KeyIndex(HCHAR *pszInput)
{
 	return set_data_Wifi_KeyIndex(pszInput);
}
HERROR TRP157_Set_SoftwareModules_ExecEnv_M_Enable(HCHAR *pszInput)
{
	return TRP_Set_SoftwareModules_ExecEnv_Enable(pszInput);
}


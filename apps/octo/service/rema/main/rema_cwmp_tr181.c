/**************************************************************
 *	@file		rema_cwmp_tr181.c
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



#include "rema_int.h"
#include "rema_cwmp.h"
#include "rema_core.h"
#include "rema_cmd.h"

/*******************************************************************/
/********************      Global Variables     ********************/
/*******************************************************************/



/*******************************************************************/
/********************      Defines         *************************/
/*******************************************************************/

#define	REMA_NETWORK_AP_ESSID		48
#define	REMA_NETWORK_AP_KEY			64
#define	REMA_NETWORK_MAX_MACADDR	(20)



/*******************************************************************/
/********************      Typedef         *************************/
/*******************************************************************/
typedef struct Rema_NetConfinfo_t
{
  HBOOL isInit;
  HINT8 napiDev_index;
  NAPINetConfInfo_t *pstNetInfo;
  NAPIWifiAPInfo_t *pstWifiAPInfo; //current
} Rema_NetConfinfo_t;

typedef struct Rema_NetApList_t
{
	HBOOL isUpdated;
	eNApiWifiEvent scaningStatus;

	NAPIWifiAPInfo_t *_ap_list;
	HINT32 dev_id;
	HINT32 count;
} Rema_NetApList_t;

/*******************************************************************/
/********************      Static Variables     ********************/
/*******************************************************************/

Rema_NetConfinfo_t gstNetConfInfo; //I guess STB has only one wifi interfae. if not, it need further implementation.
Rema_NetApList_t gNetApList; //scan result


/*******************************************************************/
/********************      Functions       *************************/
/*******************************************************************/
#define ______________________STATIC_FUNCTION_TRP_GET______________________

static HERROR _TR181_get_Wifi_AccessPoint_Security_ModeStr(eNapiWifiSecurity	security_type,eNapiWifiAuth auth_type, eNapiWifiEncrypt encrypt_type){
	RemLOG_Print("TR181_get_Wifi_AccessPoint_Security_ModeStr Defualt %s(%d Line)\r\n",__func__,__LINE__);
	switch (security_type) {
		case eNAPI_WIFI_SECURITY_OPEN:
			return "None";
		case eNAPI_WIFI_SECURITY_WPA_WPA2:
			return "WPA-WPA2-Personal";
		case eNAPI_WIFI_SECURITY_UNKNOWN:
			return "None";

	}

	if(security_type == eNAPI_WIFI_SECURITY_WEP)
	{
		switch(encrypt_type)
		{
			case eNAPI_WIFI_ENCRYPT_WEP_64:
				return "WEP-64";
			case eNAPI_WIFI_ENCRYPT_WEP_128:
				return "WEP-128";
		}
	}

	if(security_type == eNAPI_WIFI_SECURITY_WPA)
	{
		switch(auth_type){
			case eNAPI_WIFI_AUTH_WPAPSK:
				return "WPA-Personal";
			case eNAPI_WIFI_AUTH_802_1x:
				return "WPA-Enterprise";
		}
	}

	if(security_type == eNAPI_WIFI_SECURITY_WPA2)
	{
		switch(auth_type){
			case eNAPI_WIFI_AUTH_WPAPSK:
				return "WPA2-Personal";
			case eNAPI_WIFI_AUTH_802_1x:
				return "WPA2-Enterprise";
		}
	}


	return "None";
}

static HERROR _TR181_getApList(HINT32 responseTime ,NAPIWifiAPInfo_t *ap_list, HINT32 *count, HBOOL updateList){
	HERROR hErr = ERR_OK;
	HINT8 waitingLoop = 1;

	REM_FUNC_ENTER;

	if(gstNetConfInfo.isInit == TRUE){
		if(updateList == TRUE){
			hErr = NAPI_Wifi_ScanStart(gstNetConfInfo.pstNetInfo->id);
			if(hErr==ERR_OK){
				gNetApList.isUpdated = FALSE;
				if(gNetApList._ap_list!=NULL){
					HLIB_STD_MemFree(gNetApList._ap_list);
				}
				waitingLoop = 1;
				while(waitingLoop){
					//HLIB_STD_TaskSleep(responseTime);
					if(gNetApList.isUpdated==TRUE){
						//set Result
						ap_list = gNetApList._ap_list;
						*count = gNetApList.count;
						waitingLoop=0;
						REM_FUNC_LEAVE;
						return hErr;
					}else{
	//					waitingLoop=1;//when it need to
					}
				}
			}else{
				REM_FUNC_LEAVE;
				return ERR_FAIL;
			}
		}else{
			ap_list = gNetApList._ap_list;
			*count = gNetApList.count;
			REM_FUNC_LEAVE;
			return ERR_OK;
		}
	}else{
		REM_FUNC_LEAVE;
		return ERR_FAIL;
	}

	REM_FUNC_LEAVE;
	return hErr;
}

static HERROR _TR181_getConnectedAPInfo(	HINT32 id, NAPIWifiAPInfo_t *pstWifiAPInfo){
	HERROR hErr = ERR_OK;

	REM_FUNC_ENTER;
	CHECK_NULL(pstWifiAPInfo);
	hErr = NAPI_Wifi_GetConnectedAPInfo(id,pstWifiAPInfo);
	REM_FUNC_LEAVE;
	return hErr;
}

static HERROR _TR181_get_APItemOnAPList(HINT32 index, NAPIWifiAPInfo_t *pstWifiAPInfo){
	HERROR hErr = ERR_FAIL;
	NAPIWifiAPInfo_t *ap_list = NULL;
	NAPIWifiAPInfo_t *indexing_AP = NULL;
	HINT32 count =0;
	HINT32	nArg1 = 0;
	REM_ENTER;

	CHECK_NULL(pstWifiAPInfo);

	if(gstNetConfInfo.isInit == TRUE)
	{
		hErr = _TR181_getApList(0,ap_list,&count, FALSE); //get cached List
		if(hErr==ERR_OK){
			if(index < count)
			{
				if(ap_list)
				{
					indexing_AP = &ap_list[index];
					//COPY Value
					HxSTD_MemCopy(pstWifiAPInfo, indexing_AP, sizeof(NAPIWifiAPInfo_t));
				}
			}
		}
	}
	REM_LEAVE(hErr);
}

static void TR181_rema_wifi_callback(HINT32 event, void* data)
{
	HINT32 i = 0;
	HINT32 eEvent = -1;
	HUINT32 param1 = 0, param2 = 0, param3 = 0;
	NAPIWifiInfo_t *napi_ap_list = NULL;
	HINT32 data_size = 0;
	NAPIWifiAPInfo_t *_ap_list = NULL;

	REM_FUNC_ENTER;

	if(data)
	{
		napi_ap_list = data;
		_ap_list = HLIB_STD_MemAlloc(sizeof(NAPIWifiAPInfo_t)*(napi_ap_list->count));

		if(_ap_list)
		{
			for( i=0; i<napi_ap_list->count; i++ )
			{
				HxSTD_MemCopy(_ap_list[i].essid, napi_ap_list->ap[i].essid, REMA_NETWORK_AP_ESSID);
				HxSTD_MemCopy(_ap_list[i].macaddr, napi_ap_list->ap[i].macaddr, REMA_NETWORK_MAX_MACADDR);
				_ap_list[i].op_mode = napi_ap_list->ap[i].op_mode;
				_ap_list[i].secure_type = napi_ap_list->ap[i].secure_type;
				_ap_list[i].authen_type = napi_ap_list->ap[i].authen_type;
				_ap_list[i].encrypt_type = napi_ap_list->ap[i].encrypt_type;
				_ap_list[i].wave_strength = napi_ap_list->ap[i].wave_strength;
				_ap_list[i].connect_speed = napi_ap_list->ap[i].connect_speed;
				_ap_list[i].bIsConnected = napi_ap_list->ap[i].bIsConnected;
			}
		}
		else
		{
			RemLOG_Error("HLIB_STD_MemAlloc is failed!!!\n");
			return;
		}

		gNetApList.dev_id = napi_ap_list->dev_id;
		gNetApList.count = napi_ap_list->count;
		gNetApList.isUpdated = TRUE;
		gNetApList.scaningStatus = event;
		gNetApList._ap_list = _ap_list;

//		param1 = napi_ap_list->dev_id;
//		param3 = napi_ap_list->count;

//		data_size = sizeof(APKS_NETAP_Info_t)*(napi_ap_list->count);
//		param2 = (HUINT32)APK_MemDup(ap_list, data_size);

//		__apk_network_log_wifi(napi_ap_list);
	}

	switch( event )
	{
		case NAPI_EVENT_WIFI_INSERTED:
			break;
		case NAPI_EVENT_WIFI_EXTRACTED:
			break;
		case NAPI_EVENT_WIFI_SCANNING_START:
			break;
		case NAPI_EVENT_WIFI_SCANNING_END:
			break;
		case NAPI_EVENT_WIFI_SCANNING_OUTPUT:
			break;
		case NAPI_EVENT_WIFI_SCANNING_FAIL:
			break;
		case NAPI_EVENT_WIFI_KEY_INVALID:
			break;
		case NAPI_EVENT_WIFI_AP_CONNECT_SUCCESS:
			break;
		case NAPI_EVENT_WIFI_AP_CONNECT_FAIL:
			break;
		case NAPI_EVENT_WIFI_WPS_SUCCESS:
			break;
		case NAPI_EVENT_WIFI_WPS_FAIL:
			break;
		case NAPI_EVENT_WIFI_AP_DISCONNECTED:
			break;
		case NAPI_EVENT_WIFI_AP_RECONNECTED:
			break;
		case NAPI_EVENT_WIFI_AP_CONNECT_STOP:
			break;
		case NAPI_EVENT_WIFI_LINK_UPDATED:
			break;
	}

	HxLOG_Debug("	(+)[%s:%d] eEvent : %d\n",
		__FUNCTION__,__LINE__, eEvent);

	if(_ap_list)
	{
		HLIB_STD_MemFree(_ap_list);
		_ap_list = NULL;
	}

	REM_FUNC_LEAVE;
	return;
}




/*
This object models an 802.11 wireless radio on a device (a stackable interface object as described in [Section 4.2/TR-181i2]).

If the device can establish more than one connection simultaneously (e.g. a dual radio device), a separate Radio instance MUST be used for each physical radio of the device.
See [Appendix III.1/TR-181i2] for additional information.

Note: A dual-band single-radio device (e.g. an 802.11a/b/g radio) can be configured to operate at 2.4 or 5 GHz frequency bands, but only a single frequency band is used to transmit/receive at a given time.
Therefore, a single Radio instance is used even for a dual-band radio.

At most one entry in this table can exist with a given value for Alias, or with a given value for Name.
*/

static void TR181_rema_netconf_callback(HINT32 event, void* data)
{
    HUINT32 param1 = 0 ; // param2 = 0, param3 = 0;
	REM_FUNC_ENTER;

	NAPINetConfInfo_t *netInfo;
	
    if(data)
    {
		netInfo = (NAPINetConfInfo_t *)data;
		param1 = netInfo->id;
		TR157_module_Set_NetInfo(netInfo);
    }

	switch( event )
    {
    case NAPI_EVENT_NETCONF_CONN_SUCCESS:		
		RemLOG_Print("NAPI_EVENT_NETCONF_CONN_SUCCESS %s(%d Line)\r\n",__func__,__LINE__);

		Cwmp_Cpe_Add_Event(eCWMP_CPE_EVENT_NOTIFY_UPDATED_PARAM);
		RemaSendMsgCmd(eREMA_TR069CwmpCpeOpen);
	break;
    case NAPI_EVENT_NETCONF_CONN_FAIL:
		RemLOG_Print("NAPI_EVENT_NETCONF_CONN_FAIL %s(%d Line)\r\n",__func__,__LINE__);
	break;
    case NAPI_EVENT_NETCONF_AUTOIP_SUCCESS:
		RemLOG_Print("NAPI_EVENT_NETCONF_AUTOIP_SUCCESS %s(%d Line)\r\n",__func__,__LINE__);
	break;
    case NAPI_EVENT_NETCONF_AUTOIP_FAIL:
		RemLOG_Print("NAPI_EVENT_NETCONF_AUTOIP_FAIL %s(%d Line)\r\n",__func__,__LINE__);
	break;
    case NAPI_EVENT_NETCONF_LINK_CONNECTED:
		RemLOG_Print("NAPI_EVENT_NETCONF_LINK_CONNECTED %s(%d Line)\r\n",__func__,__LINE__);
	break;
    case NAPI_EVENT_NETCONF_LINK_DISCONNECTED:
		RemLOG_Print("NAPI_EVENT_NETCONF_LINK_DISCONNECTED %s(%d Line)\r\n",__func__,__LINE__);
	break;
    case NAPI_EVENT_NETCONF_STATUS_UPDATED:
		RemLOG_Print("NAPI_EVENT_NETCONF_STATUS_UPDATED %s(%d Line)\r\n",__func__,__LINE__);
	break;
    default :
	return ;
    }
	REM_FUNC_LEAVE;
    return;
}


HERROR TR181_module_DeInit()
{
   REM_FUNC_ENTER;

//Napi DeInit involved with WiFi

	if(gstNetConfInfo.isInit == TRUE){
		if(gstNetConfInfo.pstNetInfo)
		{
		HLIB_STD_MemFree(gstNetConfInfo.pstNetInfo);
		}
	}

	gstNetConfInfo.isInit == FALSE;

	NAPI_DeInit();


    REM_FUNC_LEAVE;

    return ERR_OK;
}

HERROR TR181_NAPI_Init(void)
{
	//Napi Init involved with WiFi
	NAPI_Init();
	NAPI_NetConf_RegisterCallback(TR181_rema_netconf_callback);
	NAPI_Wifi_RegisterCallback(TR181_rema_wifi_callback);

	return ERR_OK;
}


HERROR TR181_module_Init(){
   REM_FUNC_ENTER;

   if(gstNetConfInfo.isInit == TRUE){
   		TR181_module_DeInit();
   }

	gstNetConfInfo.isInit == FALSE;
	gstNetConfInfo.pstWifiAPInfo = NULL;

	gNetApList.isUpdated = FALSE;
	gNetApList.count=0;
	gNetApList._ap_list = NULL;

#if 0
	//Napi Init involved with WiFi
	TR181_NAPI_Init()
#endif

	TR181_module_Init_WIfi();//When I get this information?

    REM_FUNC_LEAVE;

    return ERR_OK;
}

//searching WIFI interface
HERROR TR181_module_Init_WIfi(){
	HINT32 numOfNetDev, defaultIndex,idx = 0;
	HBOOL  isEnable, wifiDevAvailable = FALSE;
	NAPINetConfInfo_t* tempInfo = NULL;
	HERROR hErr = ERR_OK;

	REM_FUNC_ENTER;

	gstNetConfInfo.isInit = FALSE;

	numOfNetDev = NAPI_NetConf_GetCount();

	if(numOfNetDev>0){
		tempInfo = (NAPINetConfInfo_t*) HLIB_STD_MemAlloc(sizeof(NAPINetConfInfo_t));
		if(tempInfo)
		{
			HxSTD_MemSet(tempInfo, 0, sizeof(NAPINetConfInfo_t));
		}else{
			RemLOG_Print("No NetConf!!! %s(%d Line)\r\n",__func__,__LINE__);
			hErr = ERR_FAIL;
			goto EXIT;
		}
	}else{
		RemLOG_Print("No NetConf!!! %s(%d Line)\r\n",__func__,__LINE__);
		hErr = ERR_FAIL;
		goto EXIT;
	}

	for(idx=0; idx<numOfNetDev;idx++){
		NAPI_NetConf_GetInfo(idx, tempInfo);
		if(tempInfo->type == eNAPI_NETCONF_WIFI){
			RemLOG_Print("eNAPI_NETCONF_WIFI Found!!! %s(%d Line)\r\n",__func__,__LINE__);
			wifiDevAvailable = TRUE;
			break;
		}
	}

	if(wifiDevAvailable==TRUE){
		gstNetConfInfo.pstNetInfo = tempInfo;
		gstNetConfInfo.napiDev_index = idx;
		gstNetConfInfo.isInit = TRUE;
	}else{
		gstNetConfInfo.isInit = FALSE;		
		hErr = ERR_FAIL;
		goto EXIT;
	}

EXIT :
	if(tempInfo)
	{
		HLIB_STD_MemFree(tempInfo);
		tempInfo = NULL;
	}
	
	REM_FUNC_LEAVE;
	return hErr;

}


//The number of entries in the Radio table
HERROR TR181_get_Device_Wifi_RadioNumberOfEntries(HINT32 *pszOutput)
{
	HERROR hErr = ERR_OK;
	RemLOG_Print("%s(%d Line)\r\n",__func__,__LINE__);

	CHECK_NULL(pszOutput);

	//항상 1

	*pszOutput  = 1;

	REM_LEAVE(hErr);
}

//The number of entries in the SSID table.
HERROR TR181_get_Device_Wifi_SSIDNumberOfEntries(HINT32 *pszOutput)
{
	HERROR hErr = ERR_OK;
	RemLOG_Print("%s(%d Line)\r\n",__func__,__LINE__);

	CHECK_NULL(pszOutput);

	//항상 1

	*pszOutput  = 1;

	REM_LEAVE(hErr);
}


//The number of entries in the AccessPoint table

HERROR TR181_get_Device_Wifi_AccessPointNumberOfEntries(HINT32 *pszOutput)
{
	HERROR hErr = ERR_OK;
	NAPIWifiAPInfo_t *ap_list;
	HINT32 count =0;

	RemLOG_Print("%s(%d Line)\r\n",__func__,__LINE__);

	CHECK_NULL(pszOutput);

	//hErr = _TR181_getApList(0,ap_list,&count, TRUE);

	if(hErr == ERR_OK){
		*pszOutput  = count;
	}else{
		*pszOutput  = 0;
	}

	REM_LEAVE(hErr);
}
//The number of entries in the EndPoint table.
HERROR TR181_get_Device_Wifi_EndPointNumberOfEntries(HINT32 *pszOutput)
{
	HERROR hErr = ERR_OK;
	RemLOG_Print("%s(%d Line)\r\n",__func__,__LINE__);

	CHECK_NULL(pszOutput);

	//항상 1

	*pszOutput  = 1;

	REM_LEAVE(hErr);

}

//Enables or disables the SSID entry.
HERROR TR181_get_Device_Wifi_SSID_Enable(HINT32 *pszOutput)
{
	HERROR hErr = ERR_OK;
	HINT32 nArg1 = 0, nArg2 = 0;
	HCHAR *ret = "false";
	HBOOL bEnable = FALSE;
	NAPIWifiAPInfo_t *pstWifiAPInfo = NULL;

	RemLOG_Print("%s(%d Line)\r\n",__func__,__LINE__);
	CHECK_NULL(pszOutput);

	if(gstNetConfInfo.isInit == TRUE){
		pstWifiAPInfo = (NAPIWifiAPInfo_t*) HLIB_STD_MemAlloc(sizeof(NAPIWifiAPInfo_t));
		if(pstWifiAPInfo){
			_TR181_getConnectedAPInfo(gstNetConfInfo.pstNetInfo->id,pstWifiAPInfo);
			if(pstWifiAPInfo->bIsConnected == TRUE){
				ret = "true";
			}
		}
	}

	if(pstWifiAPInfo)	HLIB_STD_MemFree(pstWifiAPInfo);

	HxSTD_StrCpy(pszOutput, ret);
	REM_LEAVE(hErr);
}

/*
When Enable is false then Status SHOULD normally be Down (or NotPresent or Error if there is a fault condition on the interface).

When Enable is changed to true then Status SHOULD change to Up if and only if the interface is able to transmit and receive network traffic;
it SHOULD change to Dormant if and only if the interface is operable but is waiting for external actions before it can transmit and receive network traffic (and subsequently change to Up if still operable when the expected actions have completed);
it SHOULD change to LowerLayerDown if and only if the interface is prevented from entering the Up state because one or more of the interfaces beneath it is down;
it SHOULD remain in the Error state if there is an error or other fault condition detected on the interface;
it SHOULD remain in the NotPresent state if the interface has missing (typically hardware) components;
it SHOULD change to Unknown if the state of the interface can not be determined for some reason
*/

HERROR TR181_get_Device_Wifi_SSID_Status(HINT32 *pszOutput){
	HERROR hErr = ERR_OK;
	HCHAR *ret = "DOWN";
	HBOOL bEnable = FALSE;
	NAPIWifiAPInfo_t *pstWifiAPInfo = NULL;

	RemLOG_Print("%s(%d Line)\r\n",__func__,__LINE__);

	CHECK_NULL(pszOutput);

	if(gstNetConfInfo.isInit == TRUE){
		pstWifiAPInfo = (NAPIWifiAPInfo_t*) HLIB_STD_MemAlloc(sizeof(NAPIWifiAPInfo_t));
		if(pstWifiAPInfo){
			_TR181_getConnectedAPInfo(gstNetConfInfo.pstNetInfo->id,pstWifiAPInfo);
			if(pstWifiAPInfo->bIsConnected == TRUE){
				ret = "Up";
			}
		}
	}

	if(pstWifiAPInfo)	HLIB_STD_MemFree(pstWifiAPInfo);
	HxSTD_StrCpy(pszOutput, ret);
	
	REM_LEAVE(hErr);
}

//A non-volatile handle used to reference this instance. Alias provides a mechanism for an ACS to label this instance for future reference.
/*
If the CPE supports the Alias-based Addressing feature as defined in [Section 3.6.1/TR-069a4] and described in [Appendix II/TR-069a4], the following mandatory constraints MUST be enforced:
Its value MUST NOT be empty.
Its value MUST start with a letter.
If its value is not assigned by the ACS, it MUST start with a "cpe-" prefix.
The CPE MUST NOT change the parameter value.
*/
HERROR TR181_get_Device_Wifi_SSID_Alias(HINT32 *pszOutput){
	HERROR hErr = ERR_OK;
	HINT32 nArg1 = 0, nArg2 = 0;

	RemLOG_Print("%s(%d Line)\r\n",__func__,__LINE__);

	CHECK_NULL(pszOutput);

	HxSTD_StrCpy(pszOutput, "cpe-Wifi");//Dummy.

	REM_LEAVE(hErr);
}

HERROR TR181_get_Device_Wifi_SSID_Name(HINT32 *pszOutput){
	HERROR hErr = ERR_OK;
	HBOOL hasSSID = FALSE;
	NAPIWifiAPInfo_t *pstWifiAPInfo = NULL;

	RemLOG_Print("%s(%d Line)\r\n",__func__,__LINE__);

	CHECK_NULL(pszOutput);

	if(gstNetConfInfo.isInit == TRUE){
		pstWifiAPInfo = (NAPIWifiAPInfo_t*) HLIB_STD_MemAlloc(sizeof(NAPIWifiAPInfo_t));
		if(pstWifiAPInfo){
			_TR181_getConnectedAPInfo(gstNetConfInfo.pstNetInfo->id,pstWifiAPInfo);
			if(pstWifiAPInfo->bIsConnected == TRUE){
				HxSTD_StrCpy(pszOutput, pstWifiAPInfo->essid);
				hasSSID = TRUE;
			}
		}
	}

	if(hasSSID == FALSE) HxSTD_StrCpy(pszOutput, "\n");
	if(pstWifiAPInfo)	HLIB_STD_MemFree(pstWifiAPInfo);
	REM_LEAVE(hErr);
}

//The accumulated time in seconds since the SSID entered its current operational state.
HERROR TR181_get_Device_Wifi_SSID_LastChange(HINT32 *pszOutput){
	HERROR hErr = ERR_OK;
	RemLOG_Print("%s(%d Line)\r\n",__func__,__LINE__);

	CHECK_NULL(pszOutput);

	HxSTD_StrCpy(pszOutput, "0"); //dummy

	//ksjung : how to accumulate time ssid enter? TR069 can't access Wifi manually.

	REM_LEAVE(hErr);
}

/*
Comma-separated list (maximum list length 1024) of strings.
Each list item MUST be the path name of an interface object that is stacked immediately below this interface object.
If the referenced object is deleted, the corresponding item MUST be removed from the list. See [Section 4.2.1/TR-181i2].

*/
HERROR TR181_get_Device_Wifi_SSID_LowerLayers(HINT32 *pszOutput){
	HERROR hErr = ERR_OK;
	RemLOG_Print("%s(%d Line)\r\n",__func__,__LINE__);

	CHECK_NULL(pszOutput);

	HxSTD_StrCpy(pszOutput, "none"); //Dummy

	REM_LEAVE(hErr);

}
//[MACAddress] The Basic Service Set ID
//This is the MAC address of the access point, which can either be local (when this instance models an access point SSID) or remote (when this instance models an end point SSID).
HERROR TR181_get_Device_Wifi_SSID_BSSID(HINT32 *pszOutput){
	HERROR hErr = ERR_OK;
	HBOOL hasAddress = FALSE;
	NAPIWifiAPInfo_t *pstWifiAPInfo = NULL;

	RemLOG_Print("%s(%d Line)\r\n",__func__,__LINE__);

	CHECK_NULL(pszOutput);

	if(gstNetConfInfo.isInit == TRUE){
		pstWifiAPInfo = (NAPIWifiAPInfo_t*) HLIB_STD_MemAlloc(sizeof(NAPIWifiAPInfo_t));
		if(pstWifiAPInfo){
			_TR181_getConnectedAPInfo(gstNetConfInfo.pstNetInfo->id,pstWifiAPInfo);
			if(pstWifiAPInfo->bIsConnected == TRUE){
				HxSTD_StrCpy(pszOutput, pstWifiAPInfo->macaddr);
				hasAddress = TRUE;
			}
		}
	}

	if(hasAddress==FALSE) HxSTD_StrCpy(pszOutput, "\n");
	
	if(pstWifiAPInfo)	HLIB_STD_MemFree(pstWifiAPInfo);

	REM_LEAVE(hErr);
}

/*
[MACAddress] The MAC address of this interface.

If this instance models an access point SSID, MACAddress is the same as MACAddress.

Note: This is not necessarily the same as the Ethernet header source or destination MAC address,
which is associated with the IP interface and is modeled via the Ethernet.Link.{i}.MACAddress parameter
*/
HERROR TR181_get_Device_Wifi_SSID_MACAddress(HINT32 *pszOutput){
	HERROR hErr = ERR_OK;
	HBOOL hasAddress = FALSE;
	NAPIWifiAPInfo_t *pstWifiAPInfo = NULL;

	RemLOG_Print("%s(%d Line)\r\n",__func__,__LINE__);

	CHECK_NULL(pszOutput);

	if(gstNetConfInfo.isInit == TRUE){
		pstWifiAPInfo = (NAPIWifiAPInfo_t*) HLIB_STD_MemAlloc(sizeof(NAPIWifiAPInfo_t));
		if(pstWifiAPInfo){
			_TR181_getConnectedAPInfo(gstNetConfInfo.pstNetInfo->id,pstWifiAPInfo);
			if(pstWifiAPInfo->bIsConnected == TRUE){
				HxSTD_StrCpy(pszOutput, pstWifiAPInfo->macaddr);
				hasAddress = TRUE;
			}
		}
	}

	if(hasAddress==FALSE) HxSTD_StrCpy(pszOutput, "\n");
	if(pstWifiAPInfo)	HLIB_STD_MemFree(pstWifiAPInfo);

	REM_LEAVE(hErr);
}

/*
The current service set identifier in use by the connection.
The SSID is an identifier that is attached to packets sent over the wireless LAN that functions as an ID for joining a particular radio network (BSS).
*/
HERROR TR181_get_Device_Wifi_SSID_SSID(HINT32 *pszOutput){
	HERROR hErr = ERR_OK;
	HBOOL hasSSID = FALSE;
	NAPIWifiAPInfo_t *pstWifiAPInfo = NULL;

	RemLOG_Print("%s(%d Line)\r\n",__func__,__LINE__);

	CHECK_NULL(pszOutput);

	if(gstNetConfInfo.isInit == TRUE){
		pstWifiAPInfo = (NAPIWifiAPInfo_t*) HLIB_STD_MemAlloc(sizeof(NAPIWifiAPInfo_t));
		if(pstWifiAPInfo){
			_TR181_getConnectedAPInfo(gstNetConfInfo.pstNetInfo->id,pstWifiAPInfo);
			if(pstWifiAPInfo->bIsConnected == TRUE){
				HxSTD_StrCpy(pszOutput, pstWifiAPInfo->essid);
				hasSSID = TRUE;
			}
		}
	}

	if(hasSSID == FALSE)HxSTD_StrCpy(pszOutput, "\n");

	if(pstWifiAPInfo)	HLIB_STD_MemFree(pstWifiAPInfo);

	REM_LEAVE(hErr);
}


HERROR TR181_get_Device_Wifi_SSID_fgdn(HINT32 *pszOutput)
{
	HERROR hErr = ERR_OK;

	RemLOG_Print("%s(%d Line)\r\n",__func__,__LINE__);

	CHECK_NULL(pszOutput);

	HxSTD_StrCpy(pszOutput, "fgdn"); //dummy

	REM_LEAVE(hErr);
}

HERROR TR181_get_Device_Wifi_SSID_frequency(HINT32 *pszOutput)
{
	HERROR hErr = ERR_OK;

	RemLOG_Print("%s(%d Line)\r\n",__func__,__LINE__);

	CHECK_NULL(pszOutput);

	HxSTD_StrCpy(pszOutput, "frequency"); //dummy

	REM_LEAVE(hErr);
}

HERROR TR181_get_Device_Wifi_SSID_frequencyunits(HINT32 *pszOutput)
{
	HERROR hErr = ERR_OK;

	RemLOG_Print("%s(%d Line)\r\n",__func__,__LINE__);

	CHECK_NULL(pszOutput);

	HxSTD_StrCpy(pszOutput, "frequencyunits"); //dummy

	REM_LEAVE(hErr);
}

HERROR TR181_get_Device_Wifi_SSID_level(HINT32 *pszOutput)
{
	HERROR hErr = ERR_OK;

	RemLOG_Print("%s(%d Line)\r\n",__func__,__LINE__);

	CHECK_NULL(pszOutput);

	HxSTD_StrCpy(pszOutput, "level"); //dummy

	REM_LEAVE(hErr);
}

HERROR TR181_get_Device_Wifi_SSID_linkspeedunits(HINT32 *pszOutput)
{
	HERROR hErr = ERR_OK;

	RemLOG_Print("%s(%d Line)\r\n",__func__,__LINE__);

	CHECK_NULL(pszOutput);

	HxSTD_StrCpy(pszOutput, "linkspeedunits"); //dummy

	REM_LEAVE(hErr);
}

HERROR TR181_get_Device_Wifi_SSID_networkid(HINT32 *pszOutput)
{
	HERROR hErr = ERR_OK;

	RemLOG_Print("%s(%d Line)\r\n",__func__,__LINE__);

	CHECK_NULL(pszOutput);

	HxSTD_StrCpy(pszOutput, "networkid"); //dummy

	REM_LEAVE(hErr);
}
HERROR TR181_get_Device_Wifi_SSID_rssi(HINT32 *pszOutput)
{
	HERROR hErr = ERR_OK;

	RemLOG_Print("%s(%d Line)\r\n",__func__,__LINE__);

	CHECK_NULL(pszOutput);

	HxSTD_StrCpy(pszOutput, "rssi"); //dummy

	REM_LEAVE(hErr);
}
HERROR TR181_get_Device_Wifi_SSID_hiddenssid(HINT32 *pszOutput)
{
	HERROR hErr = ERR_OK;

	RemLOG_Print("%s(%d Line)\r\n",__func__,__LINE__);

	CHECK_NULL(pszOutput);

	HxSTD_StrCpy(pszOutput, "hiddenssid"); //dummy

	REM_LEAVE(hErr);
}
HERROR TR181_get_Device_Wifi_SSID_supplicantstate(HINT32 *pszOutput)
{
	HERROR hErr = ERR_OK;

	RemLOG_Print("%s(%d Line)\r\n",__func__,__LINE__);

	CHECK_NULL(pszOutput);

	HxSTD_StrCpy(pszOutput, "supplicantstate"); //dummy

	REM_LEAVE(hErr);
}

//Enables or disables this access point.
HERROR TR181_get_Device_Wifi_AccessPoint_Enable(HINT32 *pszOutput)
{
	HERROR hErr = ERR_OK;
	HCHAR *ret = "false";
	HBOOL bEnable = FALSE;
	NAPIWifiAPInfo_t *pstWifiAPInfo = NULL;
	HINT32	nArg1 = 0;

	RemLOG_Print("%s(%d Line)\r\n",__func__,__LINE__);

	rema_DM_MultiInstance_GetEntryCount(eTR181A1_Device_WiFi_AccessPointNumberOfEntries,&nArg1);

	if(gstNetConfInfo.isInit == TRUE){
		HINT32 index;
		index = nArg1;//index start from 1 -> check!!
		pstWifiAPInfo = (NAPIWifiAPInfo_t*) HLIB_STD_MemAlloc(sizeof(NAPIWifiAPInfo_t));
		hErr = _TR181_get_APItemOnAPList(index,pstWifiAPInfo);
		if(hErr == ERR_OK){
			if(pstWifiAPInfo->bIsConnected==TRUE){
				ret = "true";
			}
		}
	}

	if(pstWifiAPInfo)
	{
		HLIB_STD_MemFree(pstWifiAPInfo);
		pstWifiAPInfo = NULL;
	}

	CHECK_NULL(pszOutput);
	HxSTD_StrCpy(pszOutput, ret);

	REM_LEAVE(hErr);
}

/*
Indicates the status of this access point. Enumeration of:

Disabled
Enabled
Error_Misconfigured
Error (OPTIONAL)
The Error_Misconfigured value indicates that a necessary configuration value is undefined or invalid.

The Error value MAY be used by the CPE to indicate a locally defined error condition

*/
HERROR TR181_get_Device_Wifi_AccessPoint_Status(HINT32 *pszOutput)
{
	HERROR hErr = ERR_OK;
	HINT32 nArg1 = 0;
	HCHAR *ret = "Disabled";
	HBOOL bEnable = FALSE;
	NAPIWifiAPInfo_t *pstWifiAPInfo = NULL;

	RemLOG_Print("%s(%d Line)\r\n",__func__,__LINE__);

	rema_DM_MultiInstance_GetEntryCount(eTR181A1_Device_WiFi_AccessPointNumberOfEntries,&nArg1);

	if(gstNetConfInfo.isInit == TRUE){
		HINT32 index;
		index = nArg1;//index start from 1 -> check!!
		pstWifiAPInfo = (NAPIWifiAPInfo_t*) HLIB_STD_MemAlloc(sizeof(NAPIWifiAPInfo_t));
		hErr = _TR181_get_APItemOnAPList(index,pstWifiAPInfo);
		if(hErr == ERR_OK){
			if(pstWifiAPInfo->bIsConnected==TRUE){
				ret = "Enabled";
			}
		}
	}

	if(pstWifiAPInfo)
	{
		HLIB_STD_MemFree(pstWifiAPInfo);
		pstWifiAPInfo = NULL;
	}

	CHECK_NULL(pszOutput);
	HxSTD_StrCpy(pszOutput, ret);
	

	REM_LEAVE(hErr);

}

/*
A non-volatile handle used to reference this instance. Alias provides a mechanism for an ACS to label this instance for future reference.

If the CPE supports the Alias-based Addressing feature as defined in [Section 3.6.1/TR-069a4] and described in [Appendix II/TR-069a4], the following mandatory constraints MUST be enforced:

Its value MUST NOT be empty.
Its value MUST start with a letter.
If its value is not assigned by the ACS, it MUST start with a "cpe-" prefix.
The CPE MUST NOT change the parameter value.
*/
HERROR TR181_get_Device_Wifi_AccessPoint_Alias(HINT32 *pszOutput)
{
	HERROR hErr = ERR_OK;
	HINT32 nArg1 = 0, nArg2 = 0;
	HCHAR *ret = "cpe-AccessPoint_Alias";

	RemLOG_Print("%s(%d Line)\r\n",__func__,__LINE__);


	CHECK_NULL(pszOutput);
	HxSTD_StrCpy(pszOutput, ret);

	REM_LEAVE(hErr);

}

/*
The value MUST be the path name of a row in the SSID table. If the referenced object is deleted, the parameter value MUST be set to an empty string.
*/

HERROR TR181_get_Device_Wifi_AccessPoint_SSIDReference(HINT32 *pszOutput)
{
	HERROR hErr = ERR_OK;
	HINT32 nArg1 = 0;
	NAPIWifiAPInfo_t *pstWifiAPInfo = NULL;
	HBOOL hasSSID = FALSE;

	RemLOG_Print("%s(%d Line)\r\n",__func__,__LINE__);

	CHECK_NULL(pszOutput);

	rema_DM_MultiInstance_GetEntryCount(eTR181A1_Device_WiFi_AccessPointNumberOfEntries,&nArg1);

	/*
	if(gstNetConfInfo.isInit == TRUE){
		HINT32 index;
		index = nArg1;//index start from 1 -> check!!
		pstWifiAPInfo = (NAPIWifiAPInfo_t*) HLIB_STD_MemAlloc(sizeof(NAPIWifiAPInfo_t));
		hErr = _TR181_get_APItemOnAPList(index,pstWifiAPInfo);
		if(hErr == ERR_OK){
			if(pstWifiAPInfo->bIsConnected==TRUE){
				HxSTD_StrCpy(pszOutput, pstWifiAPInfo->essid);
				hasSSID = TRUE;
			}
		}
	}
	*/

	if(hasSSID == FALSE)
	{
		HxSTD_StrCpy(pszOutput, "\n");
	}

	if(pstWifiAPInfo)	HLIB_STD_MemFree(pstWifiAPInfo);

	REM_LEAVE(hErr);

}

//Indicates whether or not beacons include the SSID name.
HERROR TR181_get_Device_Wifi_AccessPoint_SSIDAdvertisementEnabled(HINT32 *pszOutput)
{
	HERROR hErr = ERR_OK;
	HINT32 nArg1 = 0;
	NAPIWifiAPInfo_t *pstWifiAPInfo = NULL;
	HCHAR *ret = "false";

	RemLOG_Print("%s(%d Line)\r\n",__func__,__LINE__);

	CHECK_NULL(pszOutput);

	rema_DM_MultiInstance_GetEntryCount(eTR181A1_Device_WiFi_AccessPointNumberOfEntries,&nArg1);

	if(gstNetConfInfo.isInit == TRUE){
		HINT32 index;
		index = nArg1;//index start from 1 -> check!!
		pstWifiAPInfo = (NAPIWifiAPInfo_t*) HLIB_STD_MemAlloc(sizeof(NAPIWifiAPInfo_t));
		hErr = _TR181_get_APItemOnAPList(index,pstWifiAPInfo);
		if(hErr == ERR_OK){
			if(pstWifiAPInfo->essid!=NULL){
				ret = "true";
			}
		}
	}

	HxSTD_StrCpy(pszOutput, ret);
	if(pstWifiAPInfo)	HLIB_STD_MemFree(pstWifiAPInfo);

	REM_LEAVE(hErr);
}

/*
The maximum number of retransmission for a packet. This corresponds to IEEE 802.11 parameter dot11ShortRetryLimit.
This parameter is DEPRECATED because it is really a Radio attribute. Use Radio.{i}.RetryLimit.
*/
HERROR TR181_get_Device_Wifi_AccessPoint_RetryLimit(HINT32 *pszOutput){
	HERROR hErr = ERR_OK;
	RemLOG_Print("%s(%d Line)\r\n",__func__,__LINE__);

	CHECK_NULL(pszOutput);

	//문의
	*pszOutput  = 0;

	REM_LEAVE(hErr);

}

//Indicates whether this access point supports WiFi Multimedia (WMM) Access Categories (AC).
HERROR TR181_get_Device_Wifi_AccessPoint_WMMCapability(HINT32 *pszOutput){
	HERROR hErr = ERR_OK;
	HINT32 nArg1 = 0, nArg2 = 0;
	HCHAR *ret = "false";

	RemLOG_Print("%s(%d Line)\r\n",__func__,__LINE__);
	CHECK_NULL(pszOutput);
	//문의
	HxSTD_StrCpy(pszOutput, ret);
	REM_LEAVE(hErr);
}

/*
Indicates whether this access point supports WMM Unscheduled Automatic Power Save Delivery (U-APSD).
Note: U-APSD support implies WMM support.
*/
HERROR TR181_get_Device_Wifi_AccessPoint_UAPSDCapability(HINT32 *pszOutput){
	HERROR hErr = ERR_OK;
	HINT32 nArg1 = 0, nArg2 = 0;
	HCHAR *ret = "false";

	RemLOG_Print("%s(%d Line)\r\n",__func__,__LINE__);
	//문의
	CHECK_NULL(pszOutput);
	HxSTD_StrCpy(pszOutput, ret);

	REM_LEAVE(hErr);

}

//Whether WMM support is currently enabled. When enabled, this is indicated in beacon frames.
HERROR TR181_get_Device_Wifi_AccessPoint_WMMEnable(HINT32 *pszOutput){
	HERROR hErr = ERR_OK;
	HINT32 nArg1 = 0, nArg2 = 0;
	HCHAR *ret = "false";

	RemLOG_Print("%s(%d Line)\r\n",__func__,__LINE__);
	//문의
	CHECK_NULL(pszOutput);
	HxSTD_StrCpy(pszOutput, ret);
	REM_LEAVE(hErr);
}

/*
Whether U-APSD support is currently enabled. When enabled, this is indicated in beacon frames.
Note: U-APSD can only be enabled if WMM is also enabled.
*/
HERROR TR181_get_Device_Wifi_AccessPoint_UAPSDEnable(HINT32 *pszOutput){
	HERROR hErr = ERR_OK;
	HCHAR *ret = "false";
	//문의
	CHECK_NULL(pszOutput);
	HxSTD_StrCpy(pszOutput, ret);
	REM_LEAVE(hErr);
}

//The number of entries in the AssociatedDevice table.
HERROR TR181_get_Device_Wifi_AccessPoint_AssociatedDeviceNumberOfEntries(HINT32 *pszOutput)
{
	HERROR hErr = ERR_OK;
	RemLOG_Print("%s(%d Line)\r\n",__func__,__LINE__);

	CHECK_NULL(pszOutput);
	//문의
	//항상 1
	*pszOutput  = 1;
	REM_LEAVE(hErr);
}

/*
The maximum number of devices that can simultaneously be connected to the access point.
A value of 0 means that there is no specific limit.
*/
HERROR TR181_get_Device_Wifi_AccessPoint_MaxAssociatedDevices(HINT32 *pszOutput){
	HERROR hErr = ERR_OK;
	RemLOG_Print("%s(%d Line)\r\n",__func__,__LINE__);

	CHECK_NULL(pszOutput);
	//문의필요
	//항상 1
	*pszOutput  = 1;
	REM_LEAVE(hErr);
}

/*
Enables or disables device isolation.

A value of true means that the devices connected to the Access Point are isolated from all other devices within the home network (as is typically the case for a Wireless Hotspot).

*/
HERROR TR181_get_Device_Wifi_AccessPoint_IsolationEnable(HINT32 *pszOutput){
	HERROR hErr = ERR_OK;
	HCHAR *ret = "false";
	REM_FUNC_ENTER;
	//문의필요
	CHECK_NULL(pszOutput);
	HxSTD_StrCpy(pszOutput, ret);
	REM_LEAVE(hErr);
}

/*
When set to true, this AccessPoint instance's WiFi security settings are reset to their factory default values.
The affected settings include ModeEnabled, WEPKey, PreSharedKey and KeyPassphrase.
If the parameter cannot be set, the CPE MUST reject the request as an invalid parameter value.
Possible failure reasons include a lack of default values or if ModeEnabled is an Enterprise type, i.e. WPA-Enterprise, WPA2-Enterprise or WPA-WPA2-Enterprise.
The value of this parameter is not part of the device configuration and is always false when read.
*/
HERROR TR181_get_Device_Wifi_AccessPoint_Security_Reset(HINT32 *pszOutput){
	HERROR hErr = ERR_OK;
	HCHAR *ret = "false";

	CHECK_NULL(pszOutput);
	HxSTD_StrCpy(pszOutput, ret);

	REM_LEAVE(hErr);
}

/*
Comma-separated list of strings. Indicates which security modes this AccessPoint instance is capable of supporting. Each list item is an enumeration of:
None
WEP-64
WEP-128
WPA-Personal
WPA2-Personal
WPA-WPA2-Personal
WPA-Enterprise
WPA2-Enterprise
WPA-WPA2-Enterprise
*/
HERROR TR181_get_Device_Wifi_AccessPoint_Security_ModesSupported(HINT32 *pszOutput){
	HERROR hErr = ERR_OK;
	HCHAR *ret = "None";
	HBOOL bEnable = FALSE;

	RemLOG_Print("%s(%d Line)\r\n",__func__,__LINE__);

	if(gstNetConfInfo.isInit == TRUE){
		if(gstNetConfInfo.pstWifiAPInfo){
			if(gstNetConfInfo.pstWifiAPInfo->bIsConnected == TRUE){
				eNapiWifiSecurity	secure_type = gstNetConfInfo.pstWifiAPInfo->secure_type;
				eNapiWifiAuth		authen_type = gstNetConfInfo.pstWifiAPInfo->authen_type;
				eNapiWifiEncrypt	encrypt_type = gstNetConfInfo.pstWifiAPInfo->encrypt_type;
				ret = _TR181_get_Wifi_AccessPoint_Security_ModeStr(secure_type,authen_type,encrypt_type);
			}
		}
	}

	CHECK_NULL(pszOutput);
	HxSTD_StrCpy(pszOutput, ret);

	REM_LEAVE(hErr);
}
//The value MUST be a member of the list reported by the ModesSupported parameter. Indicates which security mode is enabled.
HERROR TR181_get_Device_Wifi_AccessPoint_Security_ModeEnabled(HINT32 *pszOutput){
	HERROR hErr = ERR_OK;
	HCHAR *ret = "None";
	HBOOL bEnable = FALSE;

	RemLOG_Print("%s(%d Line)\r\n",__func__,__LINE__);

	if(gstNetConfInfo.isInit == TRUE){
		if(gstNetConfInfo.pstWifiAPInfo){
			if(gstNetConfInfo.pstWifiAPInfo->bIsConnected == TRUE){
				eNapiWifiSecurity	secure_type = gstNetConfInfo.pstWifiAPInfo->secure_type;
				eNapiWifiAuth		authen_type = gstNetConfInfo.pstWifiAPInfo->authen_type;
				eNapiWifiEncrypt	encrypt_type = gstNetConfInfo.pstWifiAPInfo->encrypt_type;
				ret = _TR181_get_Wifi_AccessPoint_Security_ModeStr(secure_type,authen_type,encrypt_type);
			}
		}
	}

	CHECK_NULL(pszOutput);
	HxSTD_StrCpy(pszOutput, ret);

	REM_LEAVE(hErr);
}

/*
 WEP key expressed as a hexadecimal string.
WEPKey is used only if ModeEnabled is set to WEP-64 or WEP-128.
A 5 byte WEPKey corresponds to security mode WEP-64 and a 13 byte WEPKey corresponds to security mode WEP-128.
When read, this parameter returns an empty string, regardless of the actual value.
*/
HERROR TR181_get_Device_Wifi_AccessPoint_Security_WEPKey(HINT32 *pszOutput){
	HERROR hErr = ERR_OK;
	HCHAR *ret = "\0";

	RemLOG_Print("%s(%d Line)\r\n",__func__,__LINE__);

	CHECK_NULL(pszOutput);
	HxSTD_StrCpy(pszOutput, ret);

	REM_LEAVE(hErr);
}

/*
A literal PreSharedKey (PSK) expressed as a hexadecimal string.

PreSharedKey is only used if ModeEnabled is set to WPA-Personal or WPA2-Personal or WPA-WPA2-Personal.

If KeyPassphrase is written, then PreSharedKey is immediately generated. The ACS SHOULD NOT set both the KeyPassphrase and the PreSharedKey directly (the result of doing this is undefined).

When read, this parameter returns an empty string, regardless of the actual value.
*/
HERROR TR181_get_Device_Wifi_AccessPoint_Security_PreSharedKey(HINT32 *pszOutput){
	HERROR hErr = ERR_OK;
	HCHAR *ret = "\0";

	RemLOG_Print("%s(%d Line)\r\n",__func__,__LINE__);

	CHECK_NULL(pszOutput);
	HxSTD_StrCpy(pszOutput, ret);

	REM_LEAVE(hErr);
}
/*
A passphrase from which the PreSharedKey is to be generated, for WPA-Personal or WPA2-Personal or WPA-WPA2-Personal security modes.
If KeyPassphrase is written, then PreSharedKey is immediately generated.
The ACS SHOULD NOT set both the KeyPassphrase and the PreSharedKey directly (the result of doing this is undefined).
The key is generated as specified by WPA, which uses PBKDF2 from PKCS #5: Password-based Cryptography Specification Version 2.0 ([RFC2898]).
When read, this parameter returns an empty string, regardless of the actual value.
*/
HERROR TR181_get_Device_Wifi_AccessPoint_Security_KeyPassphrase(HINT32 *pszOutput){
	HERROR hErr = ERR_OK;
	HCHAR *ret = "\0";

	RemLOG_Print("%s(%d Line)\r\n",__func__,__LINE__);

	CHECK_NULL(pszOutput);
	HxSTD_StrCpy(pszOutput, ret);

	REM_LEAVE(hErr);
}

/*
The interval (expressed in seconds) in which the keys are re-generated.
This is applicable to WPA, WPA2 and Mixed (WPA-WPA2) modes in Personal or Enterprise mode (i.e. when ModeEnabled is set to a value other than None or WEP-64 or WEP-128.
*/
HERROR TR181_get_Device_Wifi_AccessPoint_Security_RekeyingInterval(HINT32 *pszOutput){
	HERROR hErr = ERR_OK;
	HCHAR *ret = "\0";

	RemLOG_Print("%s(%d Line)\r\n",__func__,__LINE__);

	CHECK_NULL(pszOutput);
	HxSTD_StrCpy(pszOutput, ret);

	REM_LEAVE(hErr);
}


/*
[IPAddress] The IP Address of the RADIUS server used for WLAN security.
RadiusServerIPAddr is only applicable when ModeEnabled is an Enterprise type (i.e. WPA-Enterprise, WPA2-Enterprise or WPA-WPA2-Enterprise).
*/
HERROR TR181_get_Device_Wifi_AccessPoint_Security_RadiusServerIPAddr(HINT32 *pszOutput){
	HERROR hErr = ERR_OK;
	HCHAR *ret = "\0";

	RemLOG_Print("%s(%d Line)\r\n",__func__,__LINE__);

	CHECK_NULL(pszOutput);
	HxSTD_StrCpy(pszOutput, ret);

	REM_LEAVE(hErr);

}
/*
[IPAddress] The IP Address of a secondary RADIUS server used for WLAN security.
SecondaryRadiusServerIPAddr is only applicable when ModeEnabled is an Enterprise type (i.e. WPA-Enterprise, WPA2-Enterprise or WPA-WPA2-Enterprise).

The client can forward requests to the secondary server in the event that the primary server is down or unreachable,
or after a number of tries to the primary server fail, or in a round-robin fashion [RFC2865].

*/

HERROR TR181_get_Device_Wifi_AccessPoint_Security_SecondaryRadiusServerIPAddr(HINT32 *pszOutput){
	HERROR hErr = ERR_OK;
	HCHAR *ret = "\0";

	RemLOG_Print("%s(%d Line)\r\n",__func__,__LINE__);

	CHECK_NULL(pszOutput);
	HxSTD_StrCpy(pszOutput, ret);

	REM_LEAVE(hErr);
}

/*
The port number of the RADIUS server used for WLAN security.
RadiusServerPort is only applicable when ModeEnabled is an Enterprise type (i.e. WPA-Enterprise, WPA2-Enterprise or WPA-WPA2-Enterprise).
*/
HERROR TR181_get_Device_Wifi_AccessPoint_Security_RadiusServerPort(HINT32 *pszOutput){
	HERROR hErr = ERR_OK;
	HCHAR *ret = "\0";

	RemLOG_Print("%s(%d Line)\r\n",__func__,__LINE__);

	CHECK_NULL(pszOutput);
	HxSTD_StrCpy(pszOutput, ret);

	REM_LEAVE(hErr);
}

/*
The port number of the secondary RADIUS server used for WLAN security.
SecondaryRadiusServerPort is only applicable when ModeEnabled is an Enterprise type (i.e. WPA-Enterprise, WPA2-Enterprise or WPA-WPA2-Enterprise).
If this parameter is not implemented, the secondary RADIUS server will use the same port number as the primary RADIUS server.
*/
HERROR TR181_get_Device_Wifi_AccessPoint_Security_SecondaryRadiusServerPort(HINT32 *pszOutput){
	HERROR hErr = ERR_OK;
	HCHAR *ret = "\0";

	RemLOG_Print("%s(%d Line)\r\n",__func__,__LINE__);

	CHECK_NULL(pszOutput);
	HxSTD_StrCpy(pszOutput, ret);

	REM_LEAVE(hErr);
}

/*
The secret used for handshaking with the RADIUS server [RFC2865].
When read, this parameter returns an empty string, regardless of the actual value.
*/
HERROR TR181_get_Device_Wifi_AccessPoint_Security_RadiusSecret(HINT32 *pszOutput){
	HERROR hErr = ERR_OK;
	HCHAR *ret = "\0";
	REM_ENTER;

	RemLOG_Print("%s(%d Line)\r\n",__func__,__LINE__);

	CHECK_NULL(pszOutput);
	HxSTD_StrCpy(pszOutput, ret);

	REM_LEAVE(hErr);
}


/*
The secret used for handshaking with the secondary RADIUS server [RFC2865].
If this parameter is not implemented, the secondary RADIUS server will use the same secret as the primary RADIUS server.
When read, this parameter returns an empty string, regardless of the actual value.
*/
HERROR TR181_get_Device_Wifi_AccessPoint_Security_SecondaryRadiusSecret(HINT32 *pszOutput){
	HERROR hErr = ERR_OK;
	HCHAR *ret = "\0";

	RemLOG_Print("%s(%d Line)\r\n",__func__,__LINE__);

	CHECK_NULL(pszOutput);
	HxSTD_StrCpy(pszOutput, ret);

	REM_LEAVE(hErr);
}

#ifdef CONFIG_DEBUG

HERROR TR181_printNetworkType(eNapiNetConfDevType napi_dev_type){
	switch (napi_dev_type) {
	case eNAPI_NETCONF_LAN:
		RemLOG_Print("TR181_printNetworkType eNAPI_NETCONF_LAN %s(%d Line)\r\n",__func__,__LINE__);
		break;
	case eNAPI_NETCONF_WIFI:
		RemLOG_Print("TR181_printNetworkType eNAPI_NETCONF_WIFI %s(%d Line)\r\n",__func__,__LINE__);
		break;
	case eNAPI_NETCONF_PPPOE:
		RemLOG_Print("TR181_printNetworkType eNAPI_NETCONF_PPPOE %s(%d Line)\r\n",__func__,__LINE__);
		break;
	case eNAPI_NETCONF_CM:
		RemLOG_Print("TR181_printNetworkType eNAPI_NETCONF_CM %s(%d Line)\r\n",__func__,__LINE__);
		break;
	case eNAPI_NETCONF_BLUETOOTH:
	case eNAPI_NETCONF_RF4CE:
	default:
		RemLOG_Print("TR181_printNetworkType Defualt %s(%d Line)\r\n",__func__,__LINE__);
		break;
	}
}
HERROR TR181_get_Device_Wifi_SSID_Test(HINT32 *pszOutput){
	HERROR hErr = ERR_OK;
	RemLOG_Print("%s(%d Line)\r\n",__func__,__LINE__);

	CHECK_NULL(pszOutput);

    if(gstNetConfInfo.isInit == FALSE){
		TR181_module_Init_WIfi();
    }

    if(gstNetConfInfo.isInit == TRUE){
		HINT32 numOfNetDev, defaultIndex = 0;
		HBOOL  isEnable = FALSE;

		numOfNetDev = NAPI_NetConf_GetCount();
		defaultIndex = NAPI_NetConf_GetDefaultIndex();
		NAPI_NetConf_IsNetworkAvailable(&isEnable);

		RemLOG_Print("numOfNetDev : %d , defaultIndex : %d , isEnable : %d \r\n",numOfNetDev,defaultIndex,isEnable);
		RemLOG_Print("######## name %s\r\n",gstNetConfInfo.pstNetInfo->name);
		TR181_printNetworkType(gstNetConfInfo.pstNetInfo->type);
   	}

	HxSTD_StrCpy(pszOutput, "false");
	REM_LEAVE(hErr);
}

#endif

#define ______________________PUBLIC_FUNCTION_TRP_GET______________________



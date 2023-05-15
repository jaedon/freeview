 /*----------------------------------------------------------------------*
<:copyright-broadcom 
 
 Copyright (c) 2005 Broadcom Corporation 
 All Rights Reserved 
 No portions of this material may be reproduced in any form without the 
 written permission of: 
          Broadcom Corporation 
          16215 Alton Parkway 
          Irvine, California 92619 
 All information contained in this document is Broadcom Corporation 
 company private, proprietary, and trade secret. 
 
:>
 *----------------------------------------------------------------------*
 * File Name  : bcmLanIF.c
 *
 * Description: LANDevice object functions that interface to the Broadcom library
 *              functions or the hardware configuration dependent
 *              features.
 *              
 * 
 * $Revision: 1.27 $
 * $Id: bcmLanIF.c,v 1.27 2006/01/18 18:37:02 dmounday Exp $
 *----------------------------------------------------------------------*/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdarg.h>
#include <errno.h>
#include <time.h>
#include <sys/time.h>
#include <sys/types.h>
 
#include <syslog.h>
#include "bcm_types.h"

#if 0
//#include "bcmtypes.h"
//#include "bcmcfm.h"
//#include "bcmcfmntwk.h"
//#include "board_api.h"
//#include "dbapi.h"
//#include "ifcwanapi.h"
#endif

#include "syscall.h"
#include "../main/utils.h"
#include "../inc/appdefs.h"
#include "../SOAPParser/CPEframework.h"
#include "../SOAPParser/RPCState.h" /* defines for ACS state */
#include "../standard/lanparams.h" 
#include "bcmWrapper.h"

#if 0
//#include "bcmSysIF.h"
//#include "wldefs.h"
//#include "dbvalid.h"
#endif

#define DEBUG
#ifdef DEBUG
#define DBGPRINT(X) fprintf X
#else
#define DBGPRINT
#endif

#define LAN_INTERFACE_NAME		"br0"


extern TRxObjNode LANDeviceInstanceDesc[];
extern TRxObjNode LANDeviceLANHostConfigManagementIPInterfaceInstanceDesc[];
extern TRxObjNode LANDeviceLANEthernetInterfaceConfigInstanceDesc[];
extern TRxObjNode LANDeviceLANUSBInterfaceConfigInstanceDesc[];
#ifdef WIRELESS
extern TRxObjNode LANDeviceWLANConfigurationInstanceDesc[];
extern TRxObjNode LANDeviceWLANConfigurationWEPKeyInstanceDesc[];
#endif /* #ifdef WIRELESS */
extern TRxObjNode LANDeviceHostsDesc[];
/** */
/** InternetGatewayDevice.LANDevice.LANHostConfigManagement.IPInterface. */

TRX_STATUS setLANDeviceLANHostConfigManagementIPInterface_Enable(char *value)
{
#if 0
	InstanceDesc *idp;
	void *obj;
	if ( (idp = getCurrentInstanceDesc()) ) {
		PBcmCfm_NtwkIntfCfg_t pObj;
		uint32 index = 0;
		while ( BcmCfm_objGet(BCMCFM_OBJ_NTWK_INTF, &obj, &index) == BcmCfm_Ok ) {
			pObj = (PBcmCfm_NtwkIntfCfg_t)obj;
			if ( pObj == NULL )	return TRX_ERR;
			if (pObj->attachToObjId == BCMCFM_OBJ_IFC_ETHERNET && 
				(strcmp(pObj->uName, LAN_INTERFACE_NAME)==0)) {
				pObj->status = streq(value,"1")? BcmCfm_CfgEnabled: BcmCfm_CfgDisabled;
				if ( BcmCfm_objSet(BCMCFM_OBJ_NTWK_INTF, obj, index) == BcmCfm_Ok )
					return TRX_OK;
				else
					return TRX_ERR;
			}
			BcmCfm_objFree(BCMCFM_OBJ_NTWK_INTF, obj);
			index++;
		}
	}
#endif
	BSTD_UNUSED(value);
	return TRX_ERR;
}
TRX_STATUS getLANDeviceLANHostConfigManagementIPInterface_Enable(char **value)
{
#if 0
	InstanceDesc *idp;
	void *obj;
	if ( (idp = getCurrentInstanceDesc()) ) {
		PBcmCfm_NtwkIntfCfg_t pObj;
		uint32 index = 0;
		while ( BcmCfm_objGet(BCMCFM_OBJ_NTWK_INTF, &obj, &index) == BcmCfm_Ok ) {
			pObj = (PBcmCfm_NtwkIntfCfg_t)obj;
			if ( pObj == NULL )	return TRX_ERR;
			if (pObj->attachToObjId == BCMCFM_OBJ_IFC_ETHERNET && 
				(strcmp(pObj->uName, LAN_INTERFACE_NAME)==0)) {
				*value = strdup(pObj->status==BcmCfm_CfgEnabled? "1": "0");
				BcmCfm_objFree(BCMCFM_OBJ_NTWK_INTF, obj);
				return TRX_OK;
			}
			BcmCfm_objFree(BCMCFM_OBJ_NTWK_INTF, obj);
			index++;
		}
	}
#endif
	BSTD_UNUSED(value);
	return TRX_ERR;
}
TRX_STATUS setLANDeviceLANHostConfigManagementIPInterface_IPInterfaceIPAddress(char *value)
{
#if 0
	InstanceDesc *idp;
	void *obj;
	if ( (idp = getCurrentInstanceDesc()) ) {
		PBcmCfm_NtwkIntfCfg_t pObj;
		uint32 index = 0;
		while ( BcmCfm_objGet(BCMCFM_OBJ_NTWK_INTF, &obj, &index) == BcmCfm_Ok ) {
			pObj = (PBcmCfm_NtwkIntfCfg_t)obj;
			if ( pObj == NULL )	return TRX_ERR;
			if (pObj->attachToObjId == BCMCFM_OBJ_IFC_ETHERNET && 
				(strcmp(pObj->uName, LAN_INTERFACE_NAME)==0)) {
				pObj->proto.ip.addr = readIp(value);
				if ( BcmCfm_objSet(BCMCFM_OBJ_NTWK_INTF, obj, index) == BcmCfm_Ok )
					return TRX_OK;
				else
					return TRX_ERR;
			}
			BcmCfm_objFree(BCMCFM_OBJ_NTWK_INTF, obj);
			index++;
		}
	}
#endif

	BSTD_UNUSED(value);
	return TRX_ERR;
}

TRX_STATUS getLANDeviceLANHostConfigManagementIPInterface_IPInterfaceIPAddress(char **value)
{
#if 0
	InstanceDesc *idp;
	void *obj;
	if ( (idp = getCurrentInstanceDesc()) ) {
		PBcmCfm_NtwkIntfCfg_t pObj;
		uint32 index = 0;
		while ( BcmCfm_objGet(BCMCFM_OBJ_NTWK_INTF, &obj, &index) == BcmCfm_Ok ) {
			pObj = (PBcmCfm_NtwkIntfCfg_t)obj;
			if ( pObj == NULL )	return TRX_ERR;
			if (pObj->attachToObjId == BCMCFM_OBJ_IFC_ETHERNET && 
				(strcmp(pObj->uName, LAN_INTERFACE_NAME)==0)) {
				*value = strdup( writeIp(pObj->proto.ip.addr));
				BcmCfm_objFree(BCMCFM_OBJ_NTWK_INTF, obj);
				return TRX_OK;
			}
			BcmCfm_objFree(BCMCFM_OBJ_NTWK_INTF, obj);
			index++;
		}
	}
#endif
	BSTD_UNUSED(value);
	return TRX_ERR;
}

TRX_STATUS setLANDeviceLANHostConfigManagementIPInterface_IPInterfaceSubnetMask(char *value)
{
#if 0
	InstanceDesc *idp;
	void *obj;
	if ( (idp = getCurrentInstanceDesc()) ) {
		PBcmCfm_NtwkIntfCfg_t pObj;
		uint32 index = 0;
		while ( BcmCfm_objGet(BCMCFM_OBJ_NTWK_INTF, &obj, &index) == BcmCfm_Ok ) {
			pObj = (PBcmCfm_NtwkIntfCfg_t)obj;
			if ( pObj == NULL )	return TRX_ERR;
			if (pObj->attachToObjId == BCMCFM_OBJ_IFC_ETHERNET && 
				(strcmp(pObj->uName, LAN_INTERFACE_NAME)==0)) {
				pObj->proto.ip.mask = readIp(value);
				if ( BcmCfm_objSet(BCMCFM_OBJ_NTWK_INTF, obj, index) == BcmCfm_Ok )
					return TRX_OK;
				else
					return TRX_ERR;
			}
			BcmCfm_objFree(BCMCFM_OBJ_NTWK_INTF, obj);
			index++;
		}
	}
#endif

	BSTD_UNUSED(value);
	return TRX_ERR;
}

TRX_STATUS getLANDeviceLANHostConfigManagementIPInterface_IPInterfaceSubnetMask(char **value)
{
#if 0
	InstanceDesc *idp;
	void *obj;
	if ( (idp = getCurrentInstanceDesc()) ) {
		PBcmCfm_NtwkIntfCfg_t pObj;
		uint32 index = 0;
		while ( BcmCfm_objGet(BCMCFM_OBJ_NTWK_INTF, &obj, &index) == BcmCfm_Ok ) {
			pObj = (PBcmCfm_NtwkIntfCfg_t)obj;
			if ( pObj == NULL )	return TRX_ERR;
			if (pObj->attachToObjId == BCMCFM_OBJ_IFC_ETHERNET && 
				(strcmp(pObj->uName, LAN_INTERFACE_NAME)==0)) {
				*value = strdup( writeIp(pObj->proto.ip.mask));
				BcmCfm_objFree(BCMCFM_OBJ_NTWK_INTF, obj);
				return TRX_OK;
			}
			BcmCfm_objFree(BCMCFM_OBJ_NTWK_INTF, obj);
			index++;
		}
	}
#endif
	BSTD_UNUSED(value);
	return TRX_ERR;
}
TRX_STATUS setLANDeviceLANHostConfigManagementIPInterface_IPInterfaceAddressingType(char *value)
{
#if 0
	InstanceDesc *idp;
	void *obj;
	if ( (idp = getCurrentInstanceDesc()) ) {
		PBcmCfm_NtwkIntfCfg_t pObj;
		uint32 index = 0;
		while ( BcmCfm_objGet(BCMCFM_OBJ_NTWK_INTF, &obj, &index) == BcmCfm_Ok ) {
			pObj = (PBcmCfm_NtwkIntfCfg_t)obj;
			if ( pObj == NULL )	return TRX_ERR;
			if (pObj->attachToObjId == BCMCFM_OBJ_IFC_ETHERNET && 
				(strcmp(pObj->uName, LAN_INTERFACE_NAME)==0)) {
				if (strcasecmp(value,"DHCP")==0)
					pObj->proto.ip.dhcpClientEnable = BcmCfm_CfgEnabled;
				else if (strcasecmp(value, "Static")==0 )
					pObj->proto.ip.dhcpClientEnable = BcmCfm_CfgDisabled;
				else /* value == "AutoIP" */
					pObj->proto.ip.dhcpClientEnable = BcmCfm_CfgDisabled;
				if ( BcmCfm_objSet(BCMCFM_OBJ_NTWK_INTF, obj, index) == BcmCfm_Ok )
					return TRX_OK;
				else
					return TRX_ERR;
			}
			BcmCfm_objFree(BCMCFM_OBJ_NTWK_INTF, obj);
			index++;
		}
	}
#endif
	BSTD_UNUSED(value);
	return TRX_ERR;
}
TRX_STATUS getLANDeviceLANHostConfigManagementIPInterface_IPInterfaceAddressingType(char **value)
{
#if 0
	InstanceDesc *idp;
	void *obj;
	if ( (idp = getCurrentInstanceDesc()) ) {
		PBcmCfm_NtwkIntfCfg_t pObj;
		uint32 index = 0;
		while ( BcmCfm_objGet(BCMCFM_OBJ_NTWK_INTF, &obj, &index) == BcmCfm_Ok ) {
			pObj = (PBcmCfm_NtwkIntfCfg_t)obj;
			if ( pObj == NULL )	return TRX_ERR;
			if (pObj->attachToObjId == BCMCFM_OBJ_IFC_ETHERNET && 
				(strcmp(pObj->uName, LAN_INTERFACE_NAME)==0)) {
				if ( pObj->proto.ip.dhcpClientEnable == BcmCfm_CfgEnabled )
					*value = strdup("DHCP");
				else
					*value = strdup("Static");
				BcmCfm_objFree(BCMCFM_OBJ_NTWK_INTF, obj);
				return TRX_OK;
			}
			BcmCfm_objFree(BCMCFM_OBJ_NTWK_INTF, obj);
			index++;
		}
	}
#endif
	BSTD_UNUSED(value);
	return TRX_ERR;
}
#if 0
/** */
/** InternetGatewayDevice.LANDevice.LANHostConfigManagement.IPInterface. */

TRX_STATUS addLANDeviceLANHostConfigManagementIPInterfaceI(char **value)
{
	InstanceDesc *idp;
	if( (idp=getCurrentInstanceDesc())){
		/* Add code here to create new instance************/
		return TRX_OK;
	}
	return TRX_ERR;
}
TRX_STATUS delLANDeviceLANHostConfigManagementIPInterfaceI(char *value)
{
	/*InstanceDesc *idp;*/
	return TRX_ERR;
}
#endif
/** */
/** InternetGatewayDevice.LANDevice.LANHostConfigManagement. */

TRX_STATUS setLANDeviceLANHostConfigManagement_DHCPServerConfigurable(char *value)
{
#if 0
	//InstanceDesc *idp;
	//if( idp=getCurrentInstanceDesc()){
		/* Add code here to set param -- delete idp if not needed *****/
		return TRX_OK;
	//}
	//return TRX_ERR;
#endif
	BSTD_UNUSED(value);
	return TRX_OK;
}
TRX_STATUS getLANDeviceLANHostConfigManagement_DHCPServerConfigurable(char **value)
{
#if 0
	//InstanceDesc *idp;
	//if( idp=getCurrentInstanceDesc()){
		/* Add code here to get param -- delete idp if not needed *****/
		 *value = strdup("");
		return TRX_OK;
	//}
	//return TRX_ERR;
#endif
	BSTD_UNUSED(value);
	return TRX_OK;
}
TRX_STATUS setLANDeviceLANHostConfigManagement_DHCPServerEnable(char *value)
{
#if 0
	void *info;
	uint32 objIndex = 0;
	
	if ( BcmCfm_objGet(BCMCFM_OBJ_NTWK_DHCP_SERVER, &info, &objIndex) == BcmCfm_Ok ) {
		PBcmCfm_NtwkDhcpSrvCfg_t pObj = (PBcmCfm_NtwkDhcpSrvCfg_t)info;
		if ( strcmp(value, "1") == 0 )
			pObj->status = BcmCfm_CfgEnabled;
		else
			pObj->status = BcmCfm_CfgDisabled;
		if ( BcmCfm_objSet(BCMCFM_OBJ_NTWK_DHCP_SERVER, info, objIndex) == BcmCfm_Ok )
			return TRX_REBOOT;
	}
#endif	
	BSTD_UNUSED(value);
	return TRX_ERR;
}
TRX_STATUS getLANDeviceLANHostConfigManagement_DHCPServerEnable(char **value)
{
#if 0
	void *info;
	uint32 objIndex = 0;

	if ( BcmCfm_objGet(BCMCFM_OBJ_NTWK_DHCP_SERVER, &info, &objIndex) == BcmCfm_Ok ) {
		PBcmCfm_NtwkDhcpSrvCfg_t pObj = (PBcmCfm_NtwkDhcpSrvCfg_t)info;
		if ( pObj->status == BcmCfm_CfgEnabled )
			*value = strdup("1");
		else
			*value = strdup("0");
		BcmCfm_objFree(BCMCFM_OBJ_NTWK_DHCP_SERVER, info);
		return TRX_OK;
	}
#endif
	BSTD_UNUSED(value);
	return TRX_ERR;
}
TRX_STATUS getLANDeviceLANHostConfigManagement_DHCPRelay(char **value)
{
#if 0
	//InstanceDesc *idp;
	//if( idp=getCurrentInstanceDesc()){
		/* Add code here to get param -- delete idp if not needed *****/
		 *value = strdup("");
		return TRX_OK;
	//}
	//return TRX_ERR;
#endif
	BSTD_UNUSED(value);
	return TRX_OK;
}
TRX_STATUS setLANDeviceLANHostConfigManagement_MinAddress(char *value)
{
#if 0
	//InstanceDesc *idp;
	//if( idp=getCurrentInstanceDesc()){
		/* Add code here to set param -- delete idp if not needed *****/
		return TRX_OK;
	//}
	//return TRX_ERR;
#endif
	BSTD_UNUSED(value);
	return TRX_OK;
}
TRX_STATUS getLANDeviceLANHostConfigManagement_MinAddress(char **value)
{
#if 0
	//InstanceDesc *idp;
	//if( idp=getCurrentInstanceDesc()){
		/* Add code here to get param -- delete idp if not needed *****/
		 *value = strdup("");
		return TRX_OK;
	//}
	//return TRX_ERR;
#endif
	BSTD_UNUSED(value);
	return TRX_OK;
}
TRX_STATUS setLANDeviceLANHostConfigManagement_MaxAddress(char *value)
{
#if 0
	//InstanceDesc *idp;
	//if( idp=getCurrentInstanceDesc()){
		/* Add code here to set param -- delete idp if not needed *****/
		return TRX_OK;
	//}
	//return TRX_ERR;
#endif
	BSTD_UNUSED(value);
	return TRX_OK;
}
TRX_STATUS getLANDeviceLANHostConfigManagement_MaxAddress(char **value)
{
#if 0
	//InstanceDesc *idp;
	//if( idp=getCurrentInstanceDesc()){
		/* Add code here to get param -- delete idp if not needed *****/
		 *value = strdup("");
		return TRX_OK;
	//}
	//return TRX_ERR;
#endif
	BSTD_UNUSED(value);
	return TRX_OK;
}
TRX_STATUS setLANDeviceLANHostConfigManagement_ReservedAddresses(char *value)
{
#if 0
	//InstanceDesc *idp;
	//if( idp=getCurrentInstanceDesc()){
		/* Add code here to set param -- delete idp if not needed *****/
		return TRX_OK;
	//}
	//return TRX_ERR;
#endif
	BSTD_UNUSED(value);
	return TRX_OK;
}
TRX_STATUS getLANDeviceLANHostConfigManagement_ReservedAddresses(char **value)
{
#if 0
	//InstanceDesc *idp;
	//if( idp=getCurrentInstanceDesc()){
		/* Add code here to get param -- delete idp if not needed *****/
		 *value = strdup("");
		return TRX_OK;
	//}
	//return TRX_ERR;
#endif
	BSTD_UNUSED(value);
	return TRX_OK;
}
TRX_STATUS setLANDeviceLANHostConfigManagement_SubnetMask(char *value)
{
#if 0
	//InstanceDesc *idp;
	//if( idp=getCurrentInstanceDesc()){
		/* Add code here to set param -- delete idp if not needed *****/
		return TRX_OK;
	//}
	//return TRX_ERR;
		return TRX_OK;
#endif
	BSTD_UNUSED(value);
	return TRX_OK;
}
TRX_STATUS getLANDeviceLANHostConfigManagement_SubnetMask(char **value)
{
#if 0
	//InstanceDesc *idp;
	//if( idp=getCurrentInstanceDesc()){
		/* Add code here to get param -- delete idp if not needed *****/
		 *value = strdup("");
		return TRX_OK;
	//}
	//return TRX_ERR;
#endif
	BSTD_UNUSED(value);
	return TRX_OK;
}
TRX_STATUS setLANDeviceLANHostConfigManagement_DNSServers(char *value)
{
#if 0
	return tr69c_setDNSServers(value);
#endif
	BSTD_UNUSED(value);
	return TRX_OK;
}
TRX_STATUS getLANDeviceLANHostConfigManagement_DNSServers(char **value)
{
#if 0
	return tr69c_getDNSServers(value);
#endif
	BSTD_UNUSED(value);
	return TRX_OK;
}
TRX_STATUS setLANDeviceLANHostConfigManagement_DomainName(char *value)
{
#if 0
	void *info;
	uint32 index=0;

	if ( value == NULL ) return TRX_ERR;
	
	if ( BcmCfm_objGet(BCMCFM_OBJ_NTWK_DNS, &info, &index) == BcmCfm_Ok ) {
		PBcmCfm_NtwkDnsCfg_t pDns = (PBcmCfm_NtwkDnsCfg_t)info;
		BcmCfm_free((void *)pDns->domainName);
		BcmCfm_alloc(strlen(value) + 1, (void **)&(pDns->domainName));
		strcpy(pDns->domainName, value);
		if ( BcmCfm_objSet(BCMCFM_OBJ_NTWK_DNS, info, index) == BcmCfm_Ok )
			return TRX_OK;
	}
#endif
	BSTD_UNUSED(value);
	return TRX_OK;
}
TRX_STATUS getLANDeviceLANHostConfigManagement_DomainName(char **value)
{
#if 0
	void *info;
	uint32 index=0;

	if ( BcmCfm_objGet(BCMCFM_OBJ_NTWK_DNS, &info, &index) == BcmCfm_Ok ) {
		PBcmCfm_NtwkDnsCfg_t pDns = (PBcmCfm_NtwkDnsCfg_t)info;
		if ( pDns != NULL )
			*value = strdup(pDns->domainName);   
		else
			 *value = strdup("");
		BcmCfm_objFree(BCMCFM_OBJ_NTWK_DNS, info);
		return TRX_OK;
	}
#endif
	BSTD_UNUSED(value);
	return TRX_OK;
}
TRX_STATUS setLANDeviceLANHostConfigManagement_IPRouters(char *value)
{
#if 0
	return tr69c_setDefaultGateway(value);
#endif
	BSTD_UNUSED(value);
	return TRX_OK;
}
TRX_STATUS getLANDeviceLANHostConfigManagement_IPRouters(char **value)
{
#if 0
	return tr69c_getDefaultGateway(value);
#endif
	BSTD_UNUSED(value);
	return TRX_OK;
}
TRX_STATUS setLANDeviceLANHostConfigManagement_DHCPLeaseTime(char *value)
{
#if 0
	void *info;
	uint32 objIndex = 0;
	
	if ( BcmCfm_objGet(BCMCFM_OBJ_NTWK_DHCP_SERVER, &info, &objIndex) == BcmCfm_Ok ) {
		PBcmCfm_NtwkDhcpSrvCfg_t pObj = (PBcmCfm_NtwkDhcpSrvCfg_t)info;
		pObj->leasedTime = (int) (atoi(value) / 3600);
		if ( BcmCfm_objSet(BCMCFM_OBJ_NTWK_DHCP_SERVER, info, objIndex) == BcmCfm_Ok )
			return TRX_REBOOT;
	}
#endif	
	BSTD_UNUSED(value);
	return TRX_OK;
}
TRX_STATUS getLANDeviceLANHostConfigManagement_DHCPLeaseTime(char **value)
{
#if 0
	void *info;
	uint32 objIndex = 0;

	if ( BcmCfm_objGet(BCMCFM_OBJ_NTWK_DHCP_SERVER, &info, &objIndex) == BcmCfm_Ok ) {
		PBcmCfm_NtwkDhcpSrvCfg_t pObj = (PBcmCfm_NtwkDhcpSrvCfg_t)info;
		char    buf[16];
		snprintf(buf, sizeof(buf), "%u", pObj->leasedTime * 3600);
		*value = strdup(buf);
		BcmCfm_objFree(BCMCFM_OBJ_NTWK_DHCP_SERVER, info);
		return TRX_OK;
	}
#endif
	BSTD_UNUSED(value);
	return TRX_OK;
}
TRX_STATUS setLANDeviceLANHostConfigManagement_UseAllocatedWAN(char *value)
{
#if 0
	//InstanceDesc *idp;
	//if( idp=getCurrentInstanceDesc()){
		/* Add code here to set param -- delete idp if not needed *****/
		return TRX_OK;
	//}
	//return TRX_ERR;
#endif
	BSTD_UNUSED(value);
	return TRX_OK;
}
TRX_STATUS getLANDeviceLANHostConfigManagement_UseAllocatedWAN(char **value)
{
#if 0
	//InstanceDesc *idp;
	//if( idp=getCurrentInstanceDesc()){
		/* Add code here to get param -- delete idp if not needed *****/
		 *value = strdup("");
		return TRX_OK;
	//}
	//return TRX_ERR;
#endif
	BSTD_UNUSED(value);
	return TRX_OK;
}
TRX_STATUS setLANDeviceLANHostConfigManagement_AssociatedConnection(char *value)
{
#if 0
	//InstanceDesc *idp;
	//if( idp=getCurrentInstanceDesc()){
		/* Add code here to set param -- delete idp if not needed *****/
		return TRX_OK;
	//}
	//return TRX_ERR;
#endif
	BSTD_UNUSED(value);
	return TRX_OK;
}
TRX_STATUS getLANDeviceLANHostConfigManagement_AssociatedConnection(char **value)
{
#if 0
	//InstanceDesc *idp;
	//if( idp=getCurrentInstanceDesc()){
		/* Add code here to get param -- delete idp if not needed *****/
		 *value = strdup("");
		return TRX_OK;
	//}
	//return TRX_ERR;
#endif
	BSTD_UNUSED(value);
	return TRX_OK;
}
TRX_STATUS setLANDeviceLANHostConfigManagement_PassthroughLease(char *value)
{
#if 0
	//InstanceDesc *idp;
	//if( idp=getCurrentInstanceDesc()){
		/* Add code here to set param -- delete idp if not needed *****/
		return TRX_OK;
	//}
	//return TRX_ERR;
#endif
	BSTD_UNUSED(value);
	return TRX_OK;
}
TRX_STATUS getLANDeviceLANHostConfigManagement_PassthroughLease(char **value)
{
#if 0
	//InstanceDesc *idp;
	//if( idp=getCurrentInstanceDesc()){
		/* Add code here to get param -- delete idp if not needed *****/
		 *value = strdup("");
		return TRX_OK;
	//}
	//return TRX_ERR;
#endif
	BSTD_UNUSED(value);
	return TRX_OK;
}
TRX_STATUS setLANDeviceLANHostConfigManagement_PassthroughMACAddress(char *value)
{
#if 0
	//InstanceDesc *idp;
	//if( idp=getCurrentInstanceDesc()){
		/* Add code here to set param -- delete idp if not needed *****/
		return TRX_OK;
	//}
	//return TRX_ERR;
#endif
	BSTD_UNUSED(value);
	return TRX_OK;
}
TRX_STATUS getLANDeviceLANHostConfigManagement_PassthroughMACAddress(char **value)
{
#if 0
	//InstanceDesc *idp;
	//if( idp=getCurrentInstanceDesc()){
		/* Add code here to get param -- delete idp if not needed *****/
		 *value = strdup("");
		return TRX_OK;
	//}
	//return TRX_ERR;
#endif
	BSTD_UNUSED(value);
	return TRX_OK;
}
TRX_STATUS setLANDeviceLANHostConfigManagement_AllowedMACAddresses(char *value)
{
#if 0
	//InstanceDesc *idp;
	//if( idp=getCurrentInstanceDesc()){
		/* Add code here to set param -- delete idp if not needed *****/
		return TRX_OK;
	//}
	//return TRX_ERR;
#endif
	BSTD_UNUSED(value);
	return TRX_OK;
}
TRX_STATUS getLANDeviceLANHostConfigManagement_AllowedMACAddresses(char **value)
{
#if 0
	//InstanceDesc *idp;
	//if( idp=getCurrentInstanceDesc()){
		/* Add code here to get param -- delete idp if not needed *****/
		 *value = strdup("");
		return TRX_OK;
	//}
	//return TRX_ERR;
#endif
	BSTD_UNUSED(value);
	return TRX_OK;
}
TRX_STATUS getLANDeviceLANHostConfigManagement_IPInterfaceNumberOfEntries(char **value)
{
#if 0
	//InstanceDesc *idp;
	//if( idp=getCurrentInstanceDesc()){
		/* Add code here to get param -- delete idp if not needed *****/
		 *value = strdup("");
		return TRX_OK;
	//}
	//return TRX_ERR;
#endif
	BSTD_UNUSED(value);
	return TRX_OK;
}

/** */
/** InternetGatewayDevice.LANDevice.LANEthernetInterfaceConfig.Stats. */

TRX_STATUS getLANDeviceLANInterfaceConfigStats(char **value, uint32 index, BcmCfm_Stats statsType)
{
#if 0
	void *obj;
	if ( BcmCfm_stsGet(BCMCFM_OBJ_NTWK_INTF, &obj, &index) == BcmCfm_Ok ) {
		PBcmCfm_NtwkIntfSts_t pObj = (PBcmCfm_NtwkIntfSts_t)obj;
		if ( pObj != NULL ) {
			char buf[64];
			uint32 *statsPtr = NULL;
			switch (statsType) {
				case BcmCfm_StatsTxBytes:
					statsPtr =  &(pObj->txBytes);
					break;
				case BcmCfm_StatsRxBytes:
					statsPtr =  &(pObj->rxBytes);
					break;
				case BcmCfm_StatsTxPkts:
					statsPtr =  &(pObj->txPkts);
					break;
				case BcmCfm_StatsRxPkts:
					statsPtr =  &(pObj->rxPkts);
					break;
				default:
					break;
			}																			
			snprintf(buf, sizeof(buf), "%lu", *statsPtr);
			*value = strdup (buf);
			BcmCfm_stsFree(BCMCFM_OBJ_NTWK_INTF, obj);
			return TRX_OK;
		}
	}
#endif
	BSTD_UNUSED(value);
	BSTD_UNUSED(index);
	BSTD_UNUSED(statsType);
	return TRX_OK;
}
TRX_STATUS getLANDeviceLANEthernetInterfaceConfigStats_BytesSent(char **value)
{
	return(getLANDeviceLANInterfaceConfigStats(value, IFC_ENET_ID, BcmCfm_StatsTxBytes));	
}
TRX_STATUS getLANDeviceLANEthernetInterfaceConfigStats_BytesReceived(char **value)
{
	return(getLANDeviceLANInterfaceConfigStats(value, IFC_ENET_ID, BcmCfm_StatsRxBytes));	
}

TRX_STATUS getLANDeviceLANEthernetInterfaceConfigStats_PacketsSent(char **value)
{
	return(getLANDeviceLANInterfaceConfigStats(value, IFC_ENET_ID, BcmCfm_StatsTxPkts));
}
TRX_STATUS getLANDeviceLANEthernetInterfaceConfigStats_PacketsReceived(char **value)
{
	return(getLANDeviceLANInterfaceConfigStats(value, IFC_ENET_ID, BcmCfm_StatsRxPkts));
}

/** */
/** InternetGatewayDevice.LANDevice.LANEthernetInterfaceConfig. */

TRX_STATUS setLANDeviceLANEthernetInterfaceConfig_Enable(char *value)
{
#if 0
	void *info;
	uint32 index = 0;
	if ( BcmCfm_objGet(BCMCFM_OBJ_IFC_ETHERNET, &info, &index) == BcmCfm_Ok ) {
		PBcmCfm_EthIfcCfg_t pEth = (PBcmCfm_EthIfcCfg_t)info;
		if ( strcmp(value, "1") == 0 )
			pEth->status = BcmCfm_CfgEnabled;
		else
			pEth->status = BcmCfm_CfgDisabled;
		if ( BcmCfm_objSet(BCMCFM_OBJ_IFC_ETHERNET, info, 0) == BcmCfm_Ok )
			return TRX_OK;
	}
#endif	
	BSTD_UNUSED(value);
	return TRX_ERR;
}
TRX_STATUS getLANDeviceLANEthernetInterfaceConfig_Enable(char **value)
{
#if 0
	void *info;
	uint32 index = 0;
	if ( BcmCfm_objGet(BCMCFM_OBJ_IFC_ETHERNET, &info, &index) == BcmCfm_Ok ) {
		PBcmCfm_EthIfcCfg_t pEth = (PBcmCfm_EthIfcCfg_t)info;
		if ( pEth && pEth->status == BcmCfm_CfgEnabled )
			*value = strdup("1");
		else
			*value = strdup("0");
		BcmCfm_objFree(BCMCFM_OBJ_IFC_ETHERNET, info);
		return TRX_OK;
	}
#endif
	BSTD_UNUSED(value);
	return TRX_ERR;
}

TRX_STATUS getLANDeviceLANEthernetInterfaceConfig_Status(char **value)
{
#if 0
	void *info;
	uint32 index = 0;
	if ( BcmCfm_stsGet(BCMCFM_OBJ_IFC_ETHERNET, &info, &index) == BcmCfm_Ok ) {
		PBcmCfm_EthIfcSts_t pEth = (PBcmCfm_EthIfcSts_t)info;
		if ( pEth->linkState == BcmCfm_LinkUp )
			*value = strdup("Up");
		else
			*value = strdup("Disabled");
		BcmCfm_stsFree(BCMCFM_OBJ_IFC_ETHERNET, info);
		return TRX_OK;
	}
#endif
	BSTD_UNUSED(value);
	return TRX_ERR;
}

TRX_STATUS getLANDeviceLANEthernetInterfaceConfig_MACAddress(char **value)
{
#if 0
	void *info;
	uint32 index = 0;
	if ( BcmCfm_objGet(BCMCFM_OBJ_IFC_ETHERNET, &info, &index) == BcmCfm_Ok ) {
		PBcmCfm_EthIfcCfg_t pEth = (PBcmCfm_EthIfcCfg_t)info;
		*value = strdup(writeMac(pEth->mac));
		BcmCfm_objFree(BCMCFM_OBJ_IFC_ETHERNET, info);
		return TRX_OK;
	}
#endif
	BSTD_UNUSED(value);
	return TRX_ERR;
}
#if 0
TRX_STATUS setLANDeviceLANEthernetInterfaceConfig_MACAddressControlEnabled(char *value)
{
	//InstanceDesc *idp;
	//if( idp=getCurrentInstanceDesc()){
		/* Add code here to set param -- delete idp if not needed *****/
		return TRX_OK;
	//}
	//return TRX_ERR;
}
TRX_STATUS getLANDeviceLANEthernetInterfaceConfig_MACAddressControlEnabled(char **value)
{
	//InstanceDesc *idp;
	//if( idp=getCurrentInstanceDesc()){
		/* Add code here to get param -- delete idp if not needed *****/
		 *value = strdup("");
		return TRX_OK;
	//}
	//return TRX_ERR;
}
TRX_STATUS setLANDeviceLANEthernetInterfaceConfig_MaxBitRate(char *value)
{
	//InstanceDesc *idp;
	//if( idp=getCurrentInstanceDesc()){
		/* Add code here to set param -- delete idp if not needed *****/
		return TRX_OK;
	//}
	//return TRX_ERR;
}
TRX_STATUS getLANDeviceLANEthernetInterfaceConfig_MaxBitRate(char **value)
{
	//InstanceDesc *idp;
	//if( idp=getCurrentInstanceDesc()){
		/* Add code here to get param -- delete idp if not needed *****/
		 *value = strdup("");
		return TRX_OK;
	//}
	//return TRX_ERR;
}
TRX_STATUS setLANDeviceLANEthernetInterfaceConfig_DuplexMode(char *value)
{
	//InstanceDesc *idp;
	//if( idp=getCurrentInstanceDesc()){
		/* Add code here to set param -- delete idp if not needed *****/
		return TRX_OK;
	//}
	//return TRX_ERR;
}
TRX_STATUS getLANDeviceLANEthernetInterfaceConfig_DuplexMode(char **value)
{
	//InstanceDesc *idp;
	//if( idp=getCurrentInstanceDesc()){
		/* Add code here to get param -- delete idp if not needed *****/
		 *value = strdup("");
		return TRX_OK;
	//}
	//return TRX_ERR;
}
#endif
/** */
/** InternetGatewayDevice.LANDevice.LANEthernetInterfaceConfig. */


/** */
/** InternetGatewayDevice.LANDevice.LANUSBInterfaceConfig.Stats. */

TRX_STATUS getLANDeviceLANUSBInterfaceConfigStats_BytesSent(char **value)
{
	return(getLANDeviceLANInterfaceConfigStats(value, IFC_USB_ID, BcmCfm_StatsTxBytes));	
}
TRX_STATUS getLANDeviceLANUSBInterfaceConfigStats_BytesReceived(char **value)
{
	return(getLANDeviceLANInterfaceConfigStats(value, IFC_USB_ID, BcmCfm_StatsRxBytes));	
}
TRX_STATUS getLANDeviceLANUSBInterfaceConfigStats_CellsSent(char **value)
{
	return(getLANDeviceLANInterfaceConfigStats(value, IFC_USB_ID, BcmCfm_StatsTxPkts));	
}
TRX_STATUS getLANDeviceLANUSBInterfaceConfigStats_CellsReceived(char **value)
{
	return(getLANDeviceLANInterfaceConfigStats(value, IFC_USB_ID, BcmCfm_StatsRxPkts));	
}

/** */
/** InternetGatewayDevice.LANDevice.LANUSBInterfaceConfig. */

TRX_STATUS setLANDeviceLANUSBInterfaceConfig_Enable(char *value)
{
#if 0
	void *info;
	uint32 index = 0;
	if ( BcmCfm_objGet(BCMCFM_OBJ_IFC_USB, &info, &index) == BcmCfm_Ok ) {
		PBcmCfm_UsbIfcCfg_t pUsb = (PBcmCfm_UsbIfcCfg_t)info;
		if ( strcmp(value, "1") == 0 )
			pUsb->status = BcmCfm_CfgEnabled;
		else
			pUsb->status = BcmCfm_CfgDisabled;
		if ( BcmCfm_objSet(BCMCFM_OBJ_IFC_USB, info, 0) == BcmCfm_Ok )
			return TRX_OK;
	}
#endif	
	BSTD_UNUSED(value);
	return TRX_ERR;
}
TRX_STATUS getLANDeviceLANUSBInterfaceConfig_Enable(char **value)
{
#if 0
	void *info;
	uint32 index = 0;
	if ( BcmCfm_objGet(BCMCFM_OBJ_IFC_USB, &info, &index) == BcmCfm_Ok ) {
		PBcmCfm_UsbIfcCfg_t pUsb = (PBcmCfm_UsbIfcCfg_t)info;
		if ( pUsb->status == BcmCfm_CfgEnabled )
			*value = strdup("1");
		else
			*value = strdup("0");
		BcmCfm_objFree(BCMCFM_OBJ_IFC_USB, info);
		return TRX_OK;
	}
#endif
	BSTD_UNUSED(value);
	return TRX_ERR;
}
TRX_STATUS getLANDeviceLANUSBInterfaceConfig_Status(char **value)
{
#if 0
	void *info;
	uint32	index=0;
	if ( BcmCfm_stsGet(BCMCFM_OBJ_IFC_USB, &info, &index) == BcmCfm_Ok ) {
		PBcmCfm_UsbIfcSts_t pUsb = (PBcmCfm_UsbIfcSts_t)info;
		if ( pUsb->linkState == BcmCfm_LinkUp )
			*value = strdup("Up");
		else
			*value = strdup("Disabled");
		BcmCfm_stsFree(BCMCFM_OBJ_IFC_USB, info);
		return TRX_OK;
	}

	*value = strdup("");
#endif
	BSTD_UNUSED(value);
	return TRX_ERR;
}
TRX_STATUS getLANDeviceLANUSBInterfaceConfig_MACAddress(char **value)
{
#if 0
	//InstanceDesc *idp;
	//if( idp=getCurrentInstanceDesc()){
		/* Add code here to get param -- delete idp if not needed *****/
		 *value = strdup("");
		return TRX_OK;
	//}
	//return TRX_ERR;
#endif
	BSTD_UNUSED(value);
	return TRX_OK;
}
TRX_STATUS setLANDeviceLANUSBInterfaceConfig_MACAddressControlEnabled(char *value)
{
#if 0
	//InstanceDesc *idp;
	//if( idp=getCurrentInstanceDesc()){
		/* Add code here to set param -- delete idp if not needed *****/
		return TRX_OK;
	//}
	//return TRX_ERR;
#endif
	BSTD_UNUSED(value);
	return TRX_OK;
}
TRX_STATUS getLANDeviceLANUSBInterfaceConfig_MACAddressControlEnabled(char **value)
{
#if 0
	//InstanceDesc *idp;
	//if( idp=getCurrentInstanceDesc()){
		/* Add code here to get param -- delete idp if not needed *****/
		 *value = strdup("");
		return TRX_OK;
	//}
	//return TRX_ERR;
#endif
	BSTD_UNUSED(value);
	return TRX_OK;
}
TRX_STATUS getLANDeviceLANUSBInterfaceConfig_Standard(char **value)
{
#if 0
	//InstanceDesc *idp;
	//if( idp=getCurrentInstanceDesc()){
		/* Add code here to get param -- delete idp if not needed *****/
		 *value = strdup("");
		return TRX_OK;
	//}
	//return TRX_ERR;
#endif
	BSTD_UNUSED(value);
	return TRX_OK;
}
TRX_STATUS getLANDeviceLANUSBInterfaceConfig_Type(char **value)
{
#if 0
	//InstanceDesc *idp;
	//if( idp=getCurrentInstanceDesc()){
		/* Add code here to get param -- delete idp if not needed *****/
		 *value = strdup("");
		return TRX_OK;
	//}
	//return TRX_ERR;
#endif
	BSTD_UNUSED(value);
	return TRX_OK;
}
TRX_STATUS getLANDeviceLANUSBInterfaceConfig_Rate(char **value)
{
#if 0
	//InstanceDesc *idp;
	//if( idp=getCurrentInstanceDesc()){
		/* Add code here to get param -- delete idp if not needed *****/
		 *value = strdup("");
		return TRX_OK;
	//}
	//return TRX_ERR;
#endif
	BSTD_UNUSED(value);
	return TRX_OK;
}
TRX_STATUS getLANDeviceLANUSBInterfaceConfig_Power(char **value)
{
#if 0
	//InstanceDesc *idp;
	//if( idp=getCurrentInstanceDesc()){
		/* Add code here to get param -- delete idp if not needed *****/
		 *value = strdup("");
		return TRX_OK;
	//}
	//return TRX_ERR;
#endif
	BSTD_UNUSED(value);
	return TRX_OK;
}

/** */
/** InternetGatewayDevice.LANDevice.LANUSBInterfaceConfig. */


#ifdef WIRELESS
/** */
/** InternetGatewayDevice.LANDevice.WLANConfiguration.AssociatedDevice. */

TRX_STATUS getLANDeviceWLANConfigurationAssociatedDevice_AssociatedDeviceMACAddress(char **value)
{
	//InstanceDesc *idp;
	//if( idp=getCurrentInstanceDesc()){
		/* Add code here to get param -- delete idp if not needed *****/
		 *value = strdup("");
		return TRX_OK;
	//}
	//return TRX_ERR;
}
TRX_STATUS getLANDeviceWLANConfigurationAssociatedDevice_AssociatedDeviceIPAddress(char **value)
{
	//InstanceDesc *idp;
	//if( idp=getCurrentInstanceDesc()){
		/* Add code here to get param -- delete idp if not needed *****/
		 *value = strdup("");
		return TRX_OK;
	//}
	//return TRX_ERR;
}
TRX_STATUS getLANDeviceWLANConfigurationAssociatedDevice_AssociatedDeviceAuthenticationState(char **value)
{
	//InstanceDesc *idp;
	//if( idp=getCurrentInstanceDesc()){
		/* Add code here to get param -- delete idp if not needed *****/
		 *value = strdup("");
		return TRX_OK;
	//}
	//return TRX_ERR;
}
TRX_STATUS getLANDeviceWLANConfigurationAssociatedDevice_LastRequestedUnicastCipher(char **value)
{
	//InstanceDesc *idp;
	//if( idp=getCurrentInstanceDesc()){
		/* Add code here to get param -- delete idp if not needed *****/
		 *value = strdup("");
		return TRX_OK;
	//}
	//return TRX_ERR;
}
TRX_STATUS getLANDeviceWLANConfigurationAssociatedDevice_LastRequestedMulticastCipher(char **value)
{
	//InstanceDesc *idp;
	//if( idp=getCurrentInstanceDesc()){
		/* Add code here to get param -- delete idp if not needed *****/
		 *value = strdup("");
		return TRX_OK;
	//}
	//return TRX_ERR;
}
TRX_STATUS getLANDeviceWLANConfigurationAssociatedDevice_LastPMKId(char **value)
{
	//InstanceDesc *idp;
	//if( idp=getCurrentInstanceDesc()){
		/* Add code here to get param -- delete idp if not needed *****/
		 *value = strdup("");
		return TRX_OK;
	//}
	//return TRX_ERR;
}

/** */
/** InternetGatewayDevice.LANDevice.WLANConfiguration.AssociatedDevice. */


/** */
/** InternetGatewayDevice.LANDevice.WLANConfiguration.WEPKey. */
/* By spec these instances have instance numbers 1..4         */
TRX_STATUS setLANDeviceWLANConfigurationWEPKey_WEPKey(char *value)
{
    void *info;
    uint32 index=0;
    InstanceDesc *idp;
    int valueSize = strlen(value);

    // only allow the correct  key size in hex
    if (valueSize != WL_KEY128_SIZE_HEX && valueSize != WL_KEY64_SIZE_HEX)
        return TRX_ERR;
    if (BcmDb_validateHexNumber(value) == DB_OBJ_VALID_ERR_INVALID_VALUE)
      	return TRX_ERR;
	
    if ( (idp = getCurrentInstanceDesc()) && idp->instanceID>0 && idp->instanceID<=4 ) {
        if ( BcmCfm_objGet(BCMCFM_OBJ_WLAN_SEC, &info, &index) == BcmCfm_Ok ) {
            PBcmCfm_WlanSecCfg_t pWlan = (PBcmCfm_WlanSecCfg_t)info;
            if (pWlan->keyLen == BcmCfm_WlanWepLen_128Bits && valueSize == WL_KEY64_SIZE_HEX)
                pWlan->keyLen = BcmCfm_WlanWepLen_64Bits;
            if (pWlan->keyLen == BcmCfm_WlanWepLen_64Bits && valueSize == WL_KEY128_SIZE_HEX)
                pWlan->keyLen = BcmCfm_WlanWepLen_128Bits;
		    BcmCfm_free((void *)pWlan->key[idp->instanceID-1]);
            DBGPRINT((stderr, "key %d buffer freed\n",idp->instanceID-1 ));
            if ( BcmCfm_alloc(valueSize + 1, (void **)&(pWlan->key[idp->instanceID-1])) == BcmCfm_Ok ) {
                DBGPRINT((stderr, "key buffer alloced\n"));
                strncpy(pWlan->key[idp->instanceID-1], value, valueSize);
                pWlan->tr69cOverride = TRUE;
                DBGPRINT((stderr, "set key[%d]=%s\n",idp->instanceID-1, pWlan->key[idp->instanceID-1]));
                if ( BcmCfm_objSet(BCMCFM_OBJ_WLAN_SEC, info, 0) == BcmCfm_Ok ){
					DBGPRINT((stderr, "key set successful\n"));
                    return TRX_OK;
				}
                else
                    DBGPRINT((stderr, "objSet failed\n"));
            }
            BcmCfm_objFree(BCMCFM_OBJ_WLAN_SEC, info);
        }
    }
    return TRX_ERR;
}

TRX_STATUS getLANDeviceWLANConfigurationWEPKey_WEPKey(char **value)
{
	void *info;
	uint32 index=0;
	InstanceDesc *idp;
	if ( (idp = getCurrentInstanceDesc()) && idp->instanceID>0 && idp->instanceID<=4 ) {
		if ( BcmCfm_objGet(BCMCFM_OBJ_WLAN_SEC, &info, &index) == BcmCfm_Ok ) {
			PBcmCfm_WlanSecCfg_t pWlan = (PBcmCfm_WlanSecCfg_t)info;
			DBGPRINT((stderr, "getWEPKey[%d] = %s\n", idp->instanceID, (char*)pWlan->key[idp->instanceID-1]));
			*value = strdup( pWlan->key[idp->instanceID-1]);
			BcmCfm_objFree(BCMCFM_OBJ_WLAN_SEC, info);
			return TRX_OK;
		}
	}
	return TRX_ERR;
}
/** */
/** InternetGatewayDevice.LANDevice.WLANConfiguration.WEPKey. */


/** */
/** InternetGatewayDevice.LANDevice.WLANConfiguration.PreSharedKey. */

TRX_STATUS setLANDeviceWLANConfigurationPreSharedKey_PreSharedKey(char *value)
{
	//InstanceDesc *idp;
	//if( idp=getCurrentInstanceDesc()){
		/* Add code here to set param -- delete idp if not needed *****/
		return TRX_OK;
	//}
	//return TRX_ERR;
}
TRX_STATUS setLANDeviceWLANConfigurationPreSharedKey_KeyPassphrase(char *value)
{
	//InstanceDesc *idp;
	//if( idp=getCurrentInstanceDesc()){
		/* Add code here to set param -- delete idp if not needed *****/
		return TRX_OK;
	//}
	//return TRX_ERR;
}
TRX_STATUS setLANDeviceWLANConfigurationPreSharedKey_AssociatedDeviceMACAddress(char *value)
{
	//InstanceDesc *idp;
	//if( idp=getCurrentInstanceDesc()){
		/* Add code here to set param -- delete idp if not needed *****/
		return TRX_OK;
	//}
	//return TRX_ERR;
}
TRX_STATUS getLANDeviceWLANConfigurationPreSharedKey_AssociatedDeviceMACAddress(char **value)
{
	//InstanceDesc *idp;
	//if( idp=getCurrentInstanceDesc()){
		/* Add code here to get param -- delete idp if not needed *****/
		 *value = strdup("");
		return TRX_OK;
	//}
	//return TRX_ERR;
}

/** */
/** InternetGatewayDevice.LANDevice.WLANConfiguration.PreSharedKey. */


/** */
/** InternetGatewayDevice.LANDevice.WLANConfiguration. */

TRX_STATUS setLANDeviceWLANConfiguration_Enable(char *value)
{
	void *info;
	uint32 index=0;
	if ( BcmCfm_objGet(BCMCFM_OBJ_IFC_WLAN, &info, &index) == BcmCfm_Ok ) {
		PBcmCfm_WlanIfcCfg_t pWlan = (PBcmCfm_WlanIfcCfg_t)info;
		if ( strcmp(value, "1") == 0 )
			pWlan->status = BcmCfm_CfgEnabled;
		else
			pWlan->status = BcmCfm_CfgDisabled;
		if ( BcmCfm_objSet(BCMCFM_OBJ_IFC_WLAN, info, 0) == BcmCfm_Ok )
			return TRX_OK;
	}
	
	return TRX_ERR;
}
TRX_STATUS getLANDeviceWLANConfiguration_Enable(char **value)
{
	void *info;
	uint32 index=0;
	if ( BcmCfm_objGet(BCMCFM_OBJ_IFC_WLAN, &info, &index) == BcmCfm_Ok ) {
		PBcmCfm_WlanIfcCfg_t pWlan = (PBcmCfm_WlanIfcCfg_t)info;
		if ( pWlan->status == BcmCfm_CfgEnabled )
			*value = strdup("1");
		else
			*value = strdup("0");
		BcmCfm_objFree(BCMCFM_OBJ_IFC_WLAN, info);
		return TRX_OK;
	}
	return TRX_ERR;
}
TRX_STATUS getLANDeviceWLANConfiguration_Status(char **value)
{
	void *info;
	uint32 index=0;
	if ( BcmCfm_stsGet(BCMCFM_OBJ_IFC_WLAN, &info, &index) == BcmCfm_Ok ) {
		PBcmCfm_WlanIfcSts_t pWlan = (PBcmCfm_WlanIfcSts_t)info;
		if ( pWlan->linkState == BcmCfm_LinkUp )
			*value = strdup("Up");
		else
			*value = strdup("Disabled");
		BcmCfm_stsFree(BCMCFM_OBJ_IFC_WLAN, info);
		return TRX_OK;
	}
	return TRX_ERR;
}
#if 0
TRX_STATUS getLANDeviceWLANConfiguration_BSSID(char **value)
{
	//InstanceDesc *idp;
	//if( idp=getCurrentInstanceDesc()){
		/* Add code here to get param -- delete idp if not needed *****/
		 *value = strdup("");
		return TRX_OK;
	//}
	//return TRX_ERR;
}
TRX_STATUS setLANDeviceWLANConfiguration_MaxBitRate(char *value)
{
	//InstanceDesc *idp;
	//if( idp=getCurrentInstanceDesc()){
		/* Add code here to set param -- delete idp if not needed *****/
		return TRX_OK;
	//}
	//return TRX_ERR;
}

TRX_STATUS getLANDeviceWLANConfiguration_MaxBitRate(char **value)
{
	//InstanceDesc *idp;
	//if( idp=getCurrentInstanceDesc()){
		/* Add code here to get param -- delete idp if not needed *****/
		 *value = strdup("");
		return TRX_OK;
	//}
	//return TRX_ERR;
}
#endif
TRX_STATUS setLANDeviceWLANConfiguration_Channel(char *value)
{
	PBcmCfm_WlanIfcCfg_t wp;

	void *objValue;
	uint32 index = 0;
	if ( BcmCfm_objGet(BCMCFM_OBJ_IFC_WLAN, &objValue, &index) == BcmCfm_Ok ) {
		wp = (PBcmCfm_WlanIfcCfg_t) objValue;
		wp->channel = strtoul(value, NULL, 10);
		BcmCfm_objSet(BCMCFM_OBJ_IFC_WLAN, objValue, index);
		return TRX_OK;
	}
	return TRX_ERR;
}
TRX_STATUS getLANDeviceWLANConfiguration_Channel(char **value)
{
	PBcmCfm_WlanIfcCfg_t wp;
	void *objValue;
	uint32 index = 0;
	int	channel;
	if ( BcmCfm_objGet(BCMCFM_OBJ_IFC_WLAN, &objValue, &index) == BcmCfm_Ok ) {
		wp = (PBcmCfm_WlanIfcCfg_t) objValue;
		channel = wp->channel;
		BcmCfm_objFree(BCMCFM_OBJ_IFC_WLAN, objValue);
		 *value = strdup( itoa(channel));
		return TRX_OK;
	}
	return TRX_ERR;
}


TRX_STATUS setLANDeviceWLANConfiguration_SSID(char *value)
{
	void *info;
	uint32 index=0;
	if ( BcmCfm_objGet(BCMCFM_OBJ_IFC_WLAN, &info, &index) == BcmCfm_Ok ) {
		PBcmCfm_WlanIfcCfg_t pWlan = (PBcmCfm_WlanIfcCfg_t)info;
		BcmCfm_free((void *)pWlan->ssid);
		if ( BcmCfm_alloc(strlen(value) + 1, (void **)&(pWlan->ssid)) == BcmCfm_Ok ) {
			strcpy(pWlan->ssid, value);
			if ( BcmCfm_objSet(BCMCFM_OBJ_IFC_WLAN, info, 0) == BcmCfm_Ok )
				return TRX_OK;
		}
	}
	
	return TRX_ERR;
}
TRX_STATUS getLANDeviceWLANConfiguration_SSID(char **value)
{
	void *info;
	uint32 index=0;
	if ( BcmCfm_objGet(BCMCFM_OBJ_IFC_WLAN, &info, &index) == BcmCfm_Ok ) {
		PBcmCfm_WlanIfcCfg_t pWlan = (PBcmCfm_WlanIfcCfg_t)info;
		*value = strdup(pWlan->ssid);
		BcmCfm_objFree(BCMCFM_OBJ_IFC_WLAN, info);
		return TRX_OK;
	}
	
	return TRX_ERR;
}
#if 0
TRX_STATUS setLANDeviceWLANConfiguration_BeaconType(char *value)
{
	//InstanceDesc *idp;
	//if( idp=getCurrentInstanceDesc()){
		/* Add code here to set param -- delete idp if not needed *****/
		return TRX_OK;
	//}
	//return TRX_ERR;
}
TRX_STATUS getLANDeviceWLANConfiguration_BeaconType(char **value)
{
	//InstanceDesc *idp;
	//if( idp=getCurrentInstanceDesc()){
		/* Add code here to get param -- delete idp if not needed *****/
		 *value = strdup("");
		return TRX_OK;
	//}
	//return TRX_ERR;
}
#endif

TRX_STATUS setLANDeviceWLANConfiguration_MACAddressControlEnabled(char *value)
{
	void *info;
	uint32 index=0;
	if ( BcmCfm_objGet(BCMCFM_OBJ_IFC_WLAN, &info, &index) == BcmCfm_Ok ) {
		PBcmCfm_WlanIfcCfg_t pWlan = (PBcmCfm_WlanIfcCfg_t)info;
		if ( strcmp(value, "1") == 0 )
			pWlan->macFilterMode = BcmCfm_Wlan_Mac_FilterMode_Allow;
		else
			pWlan->macFilterMode = BcmCfm_Wlan_Mac_FilterMode_Disabled;
		if ( BcmCfm_objSet(BCMCFM_OBJ_IFC_WLAN, info, 0) == BcmCfm_Ok )
			return TRX_OK;
	}
	
	return TRX_ERR;
}
TRX_STATUS getLANDeviceWLANConfiguration_MACAddressControlEnabled(char **value)
{
	void *info;
	uint32 index=0;
	if ( BcmCfm_objGet(BCMCFM_OBJ_IFC_WLAN, &info, &index) == BcmCfm_Ok ) {
		PBcmCfm_WlanIfcCfg_t pWlan = (PBcmCfm_WlanIfcCfg_t)info;
		if ( pWlan->macFilterMode == BcmCfm_Wlan_Mac_FilterMode_Allow )
			*value = strdup("1");
		else
			*value = strdup("0");
		BcmCfm_objFree(BCMCFM_OBJ_IFC_WLAN, info);
		return TRX_OK;
	}
	return TRX_ERR;
}
#if 0
TRX_STATUS getLANDeviceWLANConfiguration_Standard(char **value)
{
	//InstanceDesc *idp;
	//if( idp=getCurrentInstanceDesc()){
		/* Add code here to get param -- delete idp if not needed *****/
		 *value = strdup("");
		return TRX_OK;
	//}
	//return TRX_ERR;
}
#endif
TRX_STATUS setLANDeviceWLANConfiguration_WEPKeyIndex(char *value)
{
	void *info;
	uint32 index=0;

	if ( BcmCfm_objGet(BCMCFM_OBJ_WLAN_SEC, &info, &index) == BcmCfm_Ok ) {
		PBcmCfm_WlanSecCfg_t pWlan = (PBcmCfm_WlanSecCfg_t)info;
		pWlan->keyIndex = atoi(value);
		if ( BcmCfm_objSet(BCMCFM_OBJ_WLAN_SEC, info, 0) == BcmCfm_Ok )
			return TRX_OK;
	}
	
	return TRX_ERR;
}
TRX_STATUS getLANDeviceWLANConfiguration_WEPKeyIndex(char **value)
{
	void *info;
	uint32 index=0;
	if ( BcmCfm_objGet(BCMCFM_OBJ_WLAN_SEC, &info, &index) == BcmCfm_Ok ) {
		PBcmCfm_WlanSecCfg_t pWlan = (PBcmCfm_WlanSecCfg_t)info;
		char    buf[16];
		snprintf(buf, sizeof(buf), "%lu", pWlan->keyIndex);
		*value = strdup(buf);
		BcmCfm_objFree(BCMCFM_OBJ_WLAN_SEC, info);
		return TRX_OK;
	}
	return TRX_ERR;
}
#if 0
TRX_STATUS setLANDeviceWLANConfiguration_KeyPassphrase(char *value)
{
	//InstanceDesc *idp;
	//if( idp=getCurrentInstanceDesc()){
		/* Add code here to set param -- delete idp if not needed *****/
		return TRX_OK;
	//}
	//return TRX_ERR;
}
#endif
TRX_STATUS getLANDeviceWLANConfiguration_WEPEncryptionLevel(char **value)
{
	void *info;
	uint32 index=0;
	if ( BcmCfm_objGet(BCMCFM_OBJ_WLAN_SEC, &info, &index) == BcmCfm_Ok ) {
		PBcmCfm_WlanSecCfg_t pWlan = (PBcmCfm_WlanSecCfg_t)info;
		if ( pWlan->keyLen == BcmCfm_WlanWepLen_64Bits )
			*value = strdup("40-bit");
		else if ( pWlan->keyLen == BcmCfm_WlanWepLen_128Bits )
			*value = strdup("104-bit");
		else
			*value = strdup("Disabled");
		BcmCfm_objFree(BCMCFM_OBJ_WLAN_SEC, info);
		return TRX_OK;
	}
	return TRX_ERR;
}

TRX_STATUS setLANDeviceWLANConfiguration_BasicEncryptionModes(char *value)
{
	void *info;
	uint32 index=0;

	
	if ( BcmCfm_objGet(BCMCFM_OBJ_WLAN_SEC, &info, &index) == BcmCfm_Ok ) {
		PBcmCfm_WlanSecCfg_t pWlan = (PBcmCfm_WlanSecCfg_t)info;
		if ( strcmp(value, "WEPEncryption") == 0 )
			pWlan->wepEnabled = BcmCfm_CfgEnabled;
		else if ( strcmp(value, "None") == 0 )
			pWlan->wepEnabled = BcmCfm_CfgDisabled;
		else  {
			BcmCfm_objFree(BCMCFM_OBJ_WLAN_SEC, info);
			return TRX_ERR;
		}
		if ( BcmCfm_objSet(BCMCFM_OBJ_WLAN_SEC, info, 0) == BcmCfm_Ok )
			return TRX_OK;
	}
	
	return TRX_ERR;
}

TRX_STATUS getLANDeviceWLANConfiguration_BasicEncryptionModes(char **value)
{
	void *info;
	uint32 index=0;

	if ( BcmCfm_objGet(BCMCFM_OBJ_WLAN_SEC, &info, &index) == BcmCfm_Ok ) {
		PBcmCfm_WlanSecCfg_t pWlan = (PBcmCfm_WlanSecCfg_t)info;
		if ( pWlan->wepEnabled == BcmCfm_CfgEnabled )
			*value = strdup("WEPEncryption");
		else
			*value = strdup("None");
		BcmCfm_objFree(BCMCFM_OBJ_WLAN_SEC, info);
		return TRX_OK;
	}
	return TRX_ERR;
}
#if 0


TRX_STATUS setLANDeviceWLANConfiguration_BasicAuthenticationMode(char *value)
{
	//InstanceDesc *idp;
	//if( idp=getCurrentInstanceDesc()){
		/* Add code here to set param -- delete idp if not needed *****/
		return TRX_OK;
	//}
	//return TRX_ERR;
}
TRX_STATUS getLANDeviceWLANConfiguration_BasicAuthenticationMode(char **value)
{
	//InstanceDesc *idp;
	//if( idp=getCurrentInstanceDesc()){
		/* Add code here to get param -- delete idp if not needed *****/
		 *value = strdup("");
		return TRX_OK;
	//}
	//return TRX_ERR;
}
TRX_STATUS setLANDeviceWLANConfiguration_WPAEncryptionModes(char *value)
{
	//InstanceDesc *idp;
	//if( idp=getCurrentInstanceDesc()){
		/* Add code here to set param -- delete idp if not needed *****/
		return TRX_OK;
	//}
	//return TRX_ERR;
}
TRX_STATUS getLANDeviceWLANConfiguration_WPAEncryptionModes(char **value)
{
	//InstanceDesc *idp;
	//if( idp=getCurrentInstanceDesc()){
		/* Add code here to get param -- delete idp if not needed *****/
		 *value = strdup("");
		return TRX_OK;
	//}
	//return TRX_ERR;
}
TRX_STATUS setLANDeviceWLANConfiguration_WPAAuthenticationMode(char *value)
{
	//InstanceDesc *idp;
	//if( idp=getCurrentInstanceDesc()){
		/* Add code here to set param -- delete idp if not needed *****/
		return TRX_OK;
	//}
	//return TRX_ERR;
}
TRX_STATUS getLANDeviceWLANConfiguration_WPAAuthenticationMode(char **value)
{
	//InstanceDesc *idp;
	//if( idp=getCurrentInstanceDesc()){
		/* Add code here to get param -- delete idp if not needed *****/
		 *value = strdup("");
		return TRX_OK;
	//}
	//return TRX_ERR;
}
TRX_STATUS setLANDeviceWLANConfiguration_IEEE11iEncryptionModes(char *value)
{
	//InstanceDesc *idp;
	//if( idp=getCurrentInstanceDesc()){
		/* Add code here to set param -- delete idp if not needed *****/
		return TRX_OK;
	//}
	//return TRX_ERR;
}
TRX_STATUS getLANDeviceWLANConfiguration_IEEE11iEncryptionModes(char **value)
{
	//InstanceDesc *idp;
	//if( idp=getCurrentInstanceDesc()){
		/* Add code here to get param -- delete idp if not needed *****/
		 *value = strdup("");
		return TRX_OK;
	//}
	//return TRX_ERR;
}
TRX_STATUS setLANDeviceWLANConfiguration_IEEE11iAuthenticationMode(char *value)
{
	//InstanceDesc *idp;
	//if( idp=getCurrentInstanceDesc()){
		/* Add code here to set param -- delete idp if not needed *****/
		return TRX_OK;
	//}
	//return TRX_ERR;
}
TRX_STATUS getLANDeviceWLANConfiguration_IEEE11iAuthenticationMode(char **value)
{
	//InstanceDesc *idp;
	//if( idp=getCurrentInstanceDesc()){
		/* Add code here to get param -- delete idp if not needed *****/
		 *value = strdup("");
		return TRX_OK;
	//}
	//return TRX_ERR;
}
TRX_STATUS getLANDeviceWLANConfiguration_PossibleChannels(char **value)
{
	//InstanceDesc *idp;
	//if( idp=getCurrentInstanceDesc()){
		/* Add code here to get param -- delete idp if not needed *****/
		 *value = strdup("");
		return TRX_OK;
	//}
	//return TRX_ERR;
}
TRX_STATUS setLANDeviceWLANConfiguration_BasicDataTransmitRates(char *value)
{
	//InstanceDesc *idp;
	//if( idp=getCurrentInstanceDesc()){
		/* Add code here to set param -- delete idp if not needed *****/
		return TRX_OK;
	//}
	//return TRX_ERR;
}
TRX_STATUS getLANDeviceWLANConfiguration_BasicDataTransmitRates(char **value)
{
	//InstanceDesc *idp;
	//if( idp=getCurrentInstanceDesc()){
		/* Add code here to get param -- delete idp if not needed *****/
		 *value = strdup("");
		return TRX_OK;
	//}
	//return TRX_ERR;
}
TRX_STATUS setLANDeviceWLANConfiguration_OperationalDataTransmitRates(char *value)
{
	//InstanceDesc *idp;
	//if( idp=getCurrentInstanceDesc()){
		/* Add code here to set param -- delete idp if not needed *****/
		return TRX_OK;
	//}
	//return TRX_ERR;
}
TRX_STATUS getLANDeviceWLANConfiguration_OperationalDataTransmitRates(char **value)
{
	//InstanceDesc *idp;
	//if( idp=getCurrentInstanceDesc()){
		/* Add code here to get param -- delete idp if not needed *****/
		 *value = strdup("");
		return TRX_OK;
	//}
	//return TRX_ERR;
}
TRX_STATUS getLANDeviceWLANConfiguration_PossibleDataTransmitRates(char **value)
{
	//InstanceDesc *idp;
	//if( idp=getCurrentInstanceDesc()){
		/* Add code here to get param -- delete idp if not needed *****/
		 *value = strdup("");
		return TRX_OK;
	//}
	//return TRX_ERR;
}
TRX_STATUS setLANDeviceWLANConfiguration_InsecureOOBAccessEnabled(char *value)
{
	//InstanceDesc *idp;
	//if( idp=getCurrentInstanceDesc()){
		/* Add code here to set param -- delete idp if not needed *****/
		return TRX_OK;
	//}
	//return TRX_ERR;
}
TRX_STATUS getLANDeviceWLANConfiguration_InsecureOOBAccessEnabled(char **value)
{
	//InstanceDesc *idp;
	//if( idp=getCurrentInstanceDesc()){
		/* Add code here to get param -- delete idp if not needed *****/
		 *value = strdup("");
		return TRX_OK;
	//}
	//return TRX_ERR;
}
TRX_STATUS setLANDeviceWLANConfiguration_BeaconAdvertisementEnabled(char *value)
{
	//InstanceDesc *idp;
	//if( idp=getCurrentInstanceDesc()){
		/* Add code here to set param -- delete idp if not needed *****/
		return TRX_OK;
	//}
	//return TRX_ERR;
}
TRX_STATUS getLANDeviceWLANConfiguration_BeaconAdvertisementEnabled(char **value)
{
	//InstanceDesc *idp;
	//if( idp=getCurrentInstanceDesc()){
		/* Add code here to get param -- delete idp if not needed *****/
		 *value = strdup("");
		return TRX_OK;
	//}
	//return TRX_ERR;
}
TRX_STATUS setLANDeviceWLANConfiguration_RadioEnabled(char *value)
{
	//InstanceDesc *idp;
	//if( idp=getCurrentInstanceDesc()){
		/* Add code here to set param -- delete idp if not needed *****/
		return TRX_OK;
	//}
	//return TRX_ERR;
}
TRX_STATUS getLANDeviceWLANConfiguration_RadioEnabled(char **value)
{
	//InstanceDesc *idp;
	//if( idp=getCurrentInstanceDesc()){
		/* Add code here to get param -- delete idp if not needed *****/
		 *value = strdup("");
		return TRX_OK;
	//}
	//return TRX_ERR;
}
TRX_STATUS setLANDeviceWLANConfiguration_AutoRateFallBackEnabled(char *value)
{
	//InstanceDesc *idp;
	//if( idp=getCurrentInstanceDesc()){
		/* Add code here to set param -- delete idp if not needed *****/
		return TRX_OK;
	//}
	//return TRX_ERR;
}
TRX_STATUS getLANDeviceWLANConfiguration_AutoRateFallBackEnabled(char **value)
{
	//InstanceDesc *idp;
	//if( idp=getCurrentInstanceDesc()){
		/* Add code here to get param -- delete idp if not needed *****/
		 *value = strdup("");
		return TRX_OK;
	//}
	//return TRX_ERR;
}
TRX_STATUS setLANDeviceWLANConfiguration_LocationDescription(char *value)
{
	//InstanceDesc *idp;
	//if( idp=getCurrentInstanceDesc()){
		/* Add code here to set param -- delete idp if not needed *****/
		return TRX_OK;
	//}
	//return TRX_ERR;
}
TRX_STATUS getLANDeviceWLANConfiguration_LocationDescription(char **value)
{
	//InstanceDesc *idp;
	//if( idp=getCurrentInstanceDesc()){
		/* Add code here to get param -- delete idp if not needed *****/
		 *value = strdup("");
		return TRX_OK;
	//}
	//return TRX_ERR;
}
TRX_STATUS setLANDeviceWLANConfiguration_RegulatoryDomain(char *value)
{
	//InstanceDesc *idp;
	//if( idp=getCurrentInstanceDesc()){
		/* Add code here to set param -- delete idp if not needed *****/
		return TRX_OK;
	//}
	//return TRX_ERR;
}
TRX_STATUS getLANDeviceWLANConfiguration_RegulatoryDomain(char **value)
{
	//InstanceDesc *idp;
	//if( idp=getCurrentInstanceDesc()){
		/* Add code here to get param -- delete idp if not needed *****/
		 *value = strdup("");
		return TRX_OK;
	//}
	//return TRX_ERR;
}
TRX_STATUS getLANDeviceWLANConfiguration_TotalPSKFailures(char **value)
{
	//InstanceDesc *idp;
	//if( idp=getCurrentInstanceDesc()){
		/* Add code here to get param -- delete idp if not needed *****/
		 *value = strdup("");
		return TRX_OK;
	//}
	//return TRX_ERR;
}
TRX_STATUS getLANDeviceWLANConfiguration_TotalIntegrityFailures(char **value)
{
	//InstanceDesc *idp;
	//if( idp=getCurrentInstanceDesc()){
		/* Add code here to get param -- delete idp if not needed *****/
		 *value = strdup("");
		return TRX_OK;
	//}
	//return TRX_ERR;
}
TRX_STATUS getLANDeviceWLANConfiguration_ChannelsInUse(char **value)
{
	//InstanceDesc *idp;
	//if( idp=getCurrentInstanceDesc()){
		/* Add code here to get param -- delete idp if not needed *****/
		 *value = strdup("");
		return TRX_OK;
	//}
	//return TRX_ERR;
}
TRX_STATUS setLANDeviceWLANConfiguration_DeviceOperationMode(char *value)
{
	//InstanceDesc *idp;
	//if( idp=getCurrentInstanceDesc()){
		/* Add code here to set param -- delete idp if not needed *****/
		return TRX_OK;
	//}
	//return TRX_ERR;
}
TRX_STATUS getLANDeviceWLANConfiguration_DeviceOperationMode(char **value)
{
	//InstanceDesc *idp;
	//if( idp=getCurrentInstanceDesc()){
		/* Add code here to get param -- delete idp if not needed *****/
		 *value = strdup("");
		return TRX_OK;
	//}
	//return TRX_ERR;
}
TRX_STATUS setLANDeviceWLANConfiguration_DistanceFromRoot(char *value)
{
	//InstanceDesc *idp;
	//if( idp=getCurrentInstanceDesc()){
		/* Add code here to set param -- delete idp if not needed *****/
		return TRX_OK;
	//}
	//return TRX_ERR;
}
TRX_STATUS getLANDeviceWLANConfiguration_DistanceFromRoot(char **value)
{
	//InstanceDesc *idp;
	//if( idp=getCurrentInstanceDesc()){
		/* Add code here to get param -- delete idp if not needed *****/
		 *value = strdup("");
		return TRX_OK;
	//}
	//return TRX_ERR;
}
TRX_STATUS setLANDeviceWLANConfiguration_PeerBSSID(char *value)
{
	//InstanceDesc *idp;
	//if( idp=getCurrentInstanceDesc()){
		/* Add code here to set param -- delete idp if not needed *****/
		return TRX_OK;
	//}
	//return TRX_ERR;
}
TRX_STATUS getLANDeviceWLANConfiguration_PeerBSSID(char **value)
{
	//InstanceDesc *idp;
	//if( idp=getCurrentInstanceDesc()){
		/* Add code here to get param -- delete idp if not needed *****/
		 *value = strdup("");
		return TRX_OK;
	//}
	//return TRX_ERR;
}
TRX_STATUS setLANDeviceWLANConfiguration_AuthenticationServiceMode(char *value)
{
	//InstanceDesc *idp;
	//if( idp=getCurrentInstanceDesc()){
		/* Add code here to set param -- delete idp if not needed *****/
		return TRX_OK;
	//}
	//return TRX_ERR;
}
TRX_STATUS getLANDeviceWLANConfiguration_AuthenticationServiceMode(char **value)
{
	//InstanceDesc *idp;
	//if( idp=getCurrentInstanceDesc()){
		/* Add code here to get param -- delete idp if not needed *****/
		 *value = strdup("");
		return TRX_OK;
	//}
	//return TRX_ERR;
}
#endif

TRX_STATUS getLANDeviceWLANConfiguration_TotalBytesSent(char **value)
{
	return(getLANDeviceLANInterfaceConfigStats(value, IFC_WIRELESS_ID, BcmCfm_StatsTxBytes));	
}
TRX_STATUS getLANDeviceWLANConfiguration_TotalBytesReceived(char **value)
{
	return(getLANDeviceLANInterfaceConfigStats(value, IFC_WIRELESS_ID, BcmCfm_StatsRxBytes));	
}

TRX_STATUS getLANDeviceWLANConfiguration_TotalPacketsSent(char **value)
{
	return(getLANDeviceLANInterfaceConfigStats(value, IFC_WIRELESS_ID, BcmCfm_StatsTxPkts));
}
TRX_STATUS getLANDeviceWLANConfiguration_TotalPacketsReceived(char **value)
{
	return(getLANDeviceLANInterfaceConfigStats(value, IFC_WIRELESS_ID, BcmCfm_StatsRxPkts));
}

#if 0
TRX_STATUS getLANDeviceWLANConfiguration_TotalAssociations(char **value)
{
	//InstanceDesc *idp;
	//if( idp=getCurrentInstanceDesc()){
		/* Add code here to get param -- delete idp if not needed *****/
		 *value = strdup("");
		return TRX_OK;
	//}
	//return TRX_ERR;
}
#endif

/** */
/** InternetGatewayDevice.LANDevice.WLANConfiguration. */
#endif /* #ifdef WIRELESS*/

/** */
/** InternetGatewayDevice.LANDevice.Hosts.Host. */

TRX_STATUS getLANDeviceHostsHost_IPAddress(char **value)
{
#if 0
	InstanceDesc *idp = getCurrentInstanceDesc();
	PBcmCfm_NtwkLanHostSts_t pObj;
	uint32 index;
	void	*obj;

	index = idp->instanceID;
	if ( BcmCfm_stsGet(BCMCFM_OBJ_NTWK_LAN_HOST, &obj, &index) == BcmCfm_Ok ) {
		pObj = (PBcmCfm_NtwkLanHostSts_t)obj;
		if ( pObj != NULL ){
			*value = writeIp(pObj->ipAddress);
			BcmCfm_stsFree(BCMCFM_OBJ_NTWK_LAN_HOST, obj);
			return TRX_OK;
		}
	}
#endif
	BSTD_UNUSED(value);
    return TRX_ERR;
}

TRX_STATUS getLANDeviceHostsHost_AddressSource(char **value)
{
#if 0
	InstanceDesc *idp = getCurrentInstanceDesc();
	PBcmCfm_NtwkLanHostSts_t pObj;
	uint32 index;
	void	*obj;

	index = idp->instanceID;
	if ( BcmCfm_stsGet(BCMCFM_OBJ_NTWK_LAN_HOST, &obj, &index) == BcmCfm_Ok ) {
		pObj = (PBcmCfm_NtwkLanHostSts_t)obj;
		if ( pObj != NULL ){
			if ( pObj->expires == 0 )
				*value = strdup ("Static");
			else
				*value = strdup ("DHCP");
			BcmCfm_stsFree(BCMCFM_OBJ_NTWK_LAN_HOST, obj);
			return TRX_OK;
		}
	}
#endif
	BSTD_UNUSED(value);
	return TRX_ERR;
}

TRX_STATUS getLANDeviceHostsHost_LeaseTimeRemaining(char **value)
{
#if 0
	InstanceDesc *idp = getCurrentInstanceDesc();
	PBcmCfm_NtwkLanHostSts_t pObj;
	uint32 index;
	void	*obj;
	char buf[16];

	index = idp->instanceID;
	if ( BcmCfm_stsGet(BCMCFM_OBJ_NTWK_LAN_HOST, &obj, &index) == BcmCfm_Ok ) {
		pObj = (PBcmCfm_NtwkLanHostSts_t)obj;
		if ( pObj != NULL ){
			sprintf(buf, "%lu", pObj->expires);
			*value = strdup (buf);
			BcmCfm_stsFree(BCMCFM_OBJ_NTWK_LAN_HOST, obj);
			return TRX_OK;
		}
	}
#endif
	BSTD_UNUSED(value);
	return TRX_ERR;
}

TRX_STATUS getLANDeviceHostsHost_MACAddress(char **value)
{
#if 0
	InstanceDesc *idp = getCurrentInstanceDesc();
	PBcmCfm_NtwkLanHostSts_t pObj;
	uint32 index;
	void	*obj;

	index = idp->instanceID;
	if ( BcmCfm_stsGet(BCMCFM_OBJ_NTWK_LAN_HOST, &obj, &index) == BcmCfm_Ok ) {
		pObj = (PBcmCfm_NtwkLanHostSts_t)obj;
		if ( pObj != NULL ){
			*value = strdup(writeMac( pObj->macAddress));
			BcmCfm_stsFree(BCMCFM_OBJ_NTWK_LAN_HOST, obj);
			return TRX_OK;
		}
	}
#endif
	BSTD_UNUSED(value);
    return TRX_ERR;
}

TRX_STATUS getLANDeviceHostsHost_HostName(char **value)
{
#if 0
	InstanceDesc *idp = getCurrentInstanceDesc();
	PBcmCfm_NtwkLanHostSts_t pObj;
	uint32 index;
	void	*obj;

	index = idp->instanceID;
	if ( BcmCfm_stsGet(BCMCFM_OBJ_NTWK_LAN_HOST, &obj, &index) == BcmCfm_Ok ) {
		pObj = (PBcmCfm_NtwkLanHostSts_t)obj;
		if ( pObj != NULL ){
			*value = strdup( pObj->name );
			BcmCfm_stsFree(BCMCFM_OBJ_NTWK_LAN_HOST, obj);
			return TRX_OK;
		}
	}
#endif
	BSTD_UNUSED(value);
    return TRX_ERR;
}

TRX_STATUS getLANDeviceHostsHost_InterfaceType(char **value)
{
#if 0
	InstanceDesc *idp = getCurrentInstanceDesc();
	PBcmCfm_NtwkLanHostSts_t pObj;
	uint32 index;
	void	*obj;
	char ifcType[IFC_TINY_LEN];

	index = idp->instanceID;
	if ( BcmCfm_stsGet(BCMCFM_OBJ_NTWK_LAN_HOST, &obj, &index) == BcmCfm_Ok ) {
		pObj = (PBcmCfm_NtwkLanHostSts_t)obj;
		if ( pObj != NULL ){
			BcmCfmNtwk_getLanIfcTypeByMacAddr(writeMac(pObj->macAddress), ifcType);
			BcmCfm_stsFree(BCMCFM_OBJ_NTWK_LAN_HOST, obj);
			*value = strdup(ifcType);
			return TRX_OK;
		}
	}
#endif
	BSTD_UNUSED(value);
	return TRX_ERR;
}

TRX_STATUS getLANDeviceHostsHost_Active(char **value)
{
	*value = strdup("1");
	return TRX_OK;
}

/** */
/** InternetGatewayDevice.LANDevice.Hosts.Host. */



extern TRxObjNode 	LANDeviceHostsHostInstanceDesc[];
/* this is the TR69 representation of the lease table */
/* note that the ACS can't add or delete these instances */
/* return number of entries in lease table */

/*static int lanHostTableInit = 0;*/

int initLANDeviceHostsHostInstance(void){
#if 0	
	TRxObjNode 	*n = LANDeviceHostsHostInstanceDesc;
	InstanceDesc *lanDeviceIdp = LANDeviceInstanceDesc->paramAttrib.instance;
	int cnt;

	if (!lanHostTableInit) {
		BcmCfmNtwk_initLanHost();	/* init host table */
		lanHostTableInit = 1;
	}
	cnt = reInitInstancesFromBCMObj( BCMCFM_OBJ_NTWK_LAN_HOST,
									  n, lanDeviceIdp);
	return cnt;
#endif
	return 1;
}

/** */
/** InternetGatewayDevice.LANDevice.Hosts. */ /* leaase table contents *****/

TRX_STATUS getLANDeviceHosts_HostNumberOfEntries(char **value)
{	
	int	cnt;
	cnt = initLANDeviceHostsHostInstance();
	*value = strdup( itoa(cnt));
	return TRX_OK;
}

/** */
/** InternetGatewayDevice.LANDevice. */

TRX_STATUS getLANDevice_LANEthernetInterfaceNumberOfEntries(char **value)
{
	*value = strdup("1");
	return TRX_OK;
}
TRX_STATUS getLANDevice_LANUSBInterfaceNumberOfEntries(char **value)
{
	*value = strdup("1");
	return TRX_OK;
}
#ifdef WIRELESS
TRX_STATUS getLANDevice_LANWLANConfigurationNumberOfEntries(char **value)
{
	*value = strdup("1");
	return TRX_OK;
}
#endif /* #ifdef WIRELESS*/

/** */
/** InternetGatewayDevice.LANDevice. */
/*  since the instances of lan devices, USB, WLAN, etc. are static then */
/*  just initialize them once. If we allow moving these devices around */
/*  then need to handle reinitialization. I.e. adding and deleting of */
/* 	instances from the instance list */

static int lanInstancesInitialized;
void initLANDeviceInstances(void)
{
	TRxObjNode *n;
	int id;
	InstanceDesc    *idp, *lanDeviceIdp;

	if ( !lanInstancesInitialized ) {
		/* lanDeviceDesc- There is only a single Lan Device Instance on this design */
		n = LANDeviceInstanceDesc;
		id = 1;	/* getLanDeviceInstanceId() This value needs to be persistent- so just use same a constant  */
		lanDeviceIdp = getNewInstanceDesc(n, NULL, id);
		
		/* Now create instance at the next level down. i.e. - LANDevice.1.xxxx.{i}
		 * or  LANDevice.1.xxxx.yyyy.{i}
		 * This version is a bit simple minded in that it assumes that there is a bridge device
		 * that represents the LANDevices as HostConfigManaegment object with a single IPInterface instance
		 */
		n = LANDeviceLANHostConfigManagementIPInterfaceInstanceDesc;
		idp = getNewInstanceDesc(n, lanDeviceIdp,  id);
	
		/* define instance of LANEthernetInterfaceConfig.{i} -- There is only 1 */
		n = LANDeviceLANEthernetInterfaceConfigInstanceDesc;
		idp = getNewInstanceDesc(n, lanDeviceIdp, 1);
		/* */
		n = LANDeviceLANUSBInterfaceConfigInstanceDesc;
		idp = getNewInstanceDesc(n, lanDeviceIdp, 1);
	
	#ifdef WIRELESS
		{
			int i;
			/* initialize the WLANConfiguration instance - always id = 1*/
			n = LANDeviceWLANConfigurationInstanceDesc;
			idp = getNewInstanceDesc(n, lanDeviceIdp, 1);
			/* init WEPKey instances. They are ids = 1..4 */
			n = LANDeviceWLANConfigurationWEPKeyInstanceDesc;
			for	(i=1; i<=4; ++i ){
				InstanceDesc *wepIdp;
				wepIdp = getNewInstanceDesc(n, idp, i);
			}
		}
	#endif /* #ifdef WIRELESS */
		lanInstancesInitialized = 1;
	}
	/* this is the TR69 representation of the lease table */
	/* note that the ACS can't add or delete these instances */
	initLANDeviceHostsHostInstance();
}


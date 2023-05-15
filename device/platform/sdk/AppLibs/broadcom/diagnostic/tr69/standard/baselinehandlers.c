
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
 * File Name  : baselinehandlers.c
 *
 * Description: Functions that interface to the Broadcom library
 *              functions or the hardware configuration dependent
 *              features.
 *              
 *----------------------------------------------------------------------*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#if 0
//#include "bcmtypes.h"
//#include "bcmcfm.h"
//#include "ifcdefs.h"
//#include "board_api.h"
#endif
#include "../inc/appdefs.h"
#include "../bcmLibIF/bcm_types.h"

#include "baselineparams.h"

/*#define DEBUG*/

static time_t  startTime;
extern ACSState    acsState;

TRX_STATUS getManufacturerOUI(char **value)
{
#if 0
	void *info;

	BcmCfm_objGet(BCMCFM_OBJ_SYS_DEVICE_INFO, &info, 0);
	PBcmCfm_DevInfoCfg_t pDevInfo = (PBcmCfm_DevInfoCfg_t)info;
	*value = strdup(pDevInfo->manufacturerOui);
	BcmCfm_objFree(BCMCFM_OBJ_SYS_DEVICE_INFO, info);
#endif
	*value = strdup("123456789012345678901234");
	return TRX_OK;
}

TRX_STATUS getManufacturer(char **value)
{
#if 0
	*value = strdup("Broadcom");
#endif        
	*value = strdup("Broadcom");
	return TRX_OK;
}

TRX_STATUS getModelName(char **value)
{
#if 0
	void *info;

	BcmCfm_objGet(BCMCFM_OBJ_SYS_DEVICE_INFO, &info, 0);
	PBcmCfm_DevInfoCfg_t pDevInfo = (PBcmCfm_DevInfoCfg_t)info;
	*value = strdup(pDevInfo->hwVersion);
	BcmCfm_objFree(BCMCFM_OBJ_SYS_DEVICE_INFO, info);
#endif
	*value = strdup("BCM97425");
	return TRX_OK;
}

TRX_STATUS getProductClass (char **value)
{
#if 0
	void *info;

	BcmCfm_objGet(BCMCFM_OBJ_SYS_DEVICE_INFO, &info, 0);
	PBcmCfm_DevInfoCfg_t pDevInfo = (PBcmCfm_DevInfoCfg_t)info;
	*value = strdup(pDevInfo->hwVersion);
	BcmCfm_objFree(BCMCFM_OBJ_SYS_DEVICE_INFO, info);
#endif
	*value = strdup("STB");
	return TRX_OK;
}

TRX_STATUS getSerialNumber(char **value)
{
#if 0
	void *info;

	BcmCfm_objGet(BCMCFM_OBJ_SYS_DEVICE_INFO, &info, 0);
	PBcmCfm_DevInfoCfg_t pDevInfo = (PBcmCfm_DevInfoCfg_t)info;
	*value = strdup(pDevInfo->serialNumber);
	BcmCfm_objFree(BCMCFM_OBJ_SYS_DEVICE_INFO, info);
#endif
	*value = strdup("1452002");
	return TRX_OK;
}

TRX_STATUS getSoftwareVersion(char **value)
{
#if 0
	void *info;

	BcmCfm_objGet(BCMCFM_OBJ_SYS_DEVICE_INFO, &info, 0);
	PBcmCfm_DevInfoCfg_t pDevInfo = (PBcmCfm_DevInfoCfg_t)info;
	*value = strdup(pDevInfo->releaseVersion);
	BcmCfm_objFree(BCMCFM_OBJ_SYS_DEVICE_INFO, info);
#endif
	*value = strdup("1");
	return TRX_OK;
}

TRX_STATUS getHardwareVersion(char **value)
{
#if 0
	void *info;

	BcmCfm_objGet(BCMCFM_OBJ_SYS_DEVICE_INFO, &info, 0);
	PBcmCfm_DevInfoCfg_t pDevInfo = (PBcmCfm_DevInfoCfg_t)info;
	*value = strdup(pDevInfo->hwVersion);
	BcmCfm_objFree(BCMCFM_OBJ_SYS_DEVICE_INFO, info);
#endif
	*value = strdup("1");
	return TRX_OK;
}

TRX_STATUS getSpecVersion(char **value)
{
	*value = strdup("1");
	return TRX_OK;
}

TRX_STATUS getProvisioningCode(char **value)
{
#if 0
	void *info;

	BcmCfm_objGet(BCMCFM_OBJ_SYS_TR069, &info, 0);
	PBcmCfm_TR069Cfg_t pTr69 = (PBcmCfm_TR069Cfg_t)info;
	free(acsState.provisioningCode);
	acsState.provisioningCode = strdup(pTr69->provisioningCode);
	BcmCfm_objFree(BCMCFM_OBJ_SYS_TR069, info);

	*value = acsState.provisioningCode?strdup(acsState.provisioningCode):strdup("");
#endif
	*value = acsState.provisioningCode?strdup(acsState.provisioningCode):strdup("");
	return TRX_OK;
}

TRX_STATUS setProvisioningCode(char *value)
{
	if (acsState.provisioningCode)
		free(acsState.provisioningCode);
	acsState.provisioningCode = strdup(value);
    
	return TRX_OK;
}

TRX_STATUS getUpTime(char **value)
{
	time_t  upt;
	char buf[100];
            
	upt = time(NULL) - startTime;
	sprintf(buf, "%lu", upt);       
	*value = strdup(buf);
        
	return TRX_OK;
}

TRX_STATUS getDeviceLog(char **value)
{
#if 0
	void *info;

	BcmCfm_objGet(BCMCFM_OBJ_SYS_DEVICE_INFO, &info, 0);
	PBcmCfm_DevInfoCfg_t pDevInfo = (PBcmCfm_DevInfoCfg_t)info;
	*value = strdup(pDevInfo->deviceLog);
	BcmCfm_objFree(BCMCFM_OBJ_SYS_DEVICE_INFO, info);
#endif
	BSTD_UNUSED(value);
	return TRX_OK;
}

void initBaslineProfile(void) {
	/* init start time */
	startTime = time(NULL);
}

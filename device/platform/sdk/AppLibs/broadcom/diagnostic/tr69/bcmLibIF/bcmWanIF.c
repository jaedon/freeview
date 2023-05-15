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
 * File Name  : bcmWanIF.c
 *
 * Description: WANDevice object functions that interface to the Broadcom library
 *              functions or the hardware configuration dependent
 *              features.
 *              
 * 
 * $Revision: 1.28 $
 * $Id: bcmWanIF.c,v 1.28 2006/01/16 20:38:08 dmounday Exp $
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
//#include "bcmcfmsec.h"
//#include "board_api.h"
//#include "objectdefs.h"
//#include "dbapi.h"
//#include "ifcwanapi.h"
//#include "AdslMibDef.h"
#endif
#include "syscall.h"
#include "../main/utils.h"
#include "../inc/appdefs.h"
#include "../SOAPParser/CPEframework.h"
#include "../SOAPParser/RPCState.h" /* defines for ACS state */
#include "../standard/rootparams.h"
#include "../standard/wanparams.h"
#include "bcmWrapper.h"
#include "bcmSysIF.h"
#if 0
//#include "bcmcfmdiag.h"
//#include "bcmcfmphys.h"
//#define DEBUG
#endif
#ifdef DEBUG
#define DBGPRINT(X) fprintf X
#else
#define DBGPRINT(X)
#endif

extern char *strndup(const char *s, size_t n);

/** */
/** InternetGatewayDevice.WANDevice. */

#if 0
/** */
/** InternetGatewayDevice.WANDevice.WANCommonInterfaceConfig.Connection. */

TRX_STATUS getWANDeviceWANCommonInterfaceConfigConnection_ActiveConnectionDeviceContainer(char **value)
{
	//InstanceDesc *idp;
	//if( idp=getCurrentInstanceDesc()){
		/* Add code here to get param -- delete idp if not needed *****/
		 *value = strdup("");
		return TRX_OK;
	//}
	//return TRX_ERR;
}
TRX_STATUS getWANDeviceWANCommonInterfaceConfigConnection_ActiveConnectionServiceID(char **value)
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
/** InternetGatewayDevice.WANDevice.WANCommonInterfaceConfig.Connection. */
#endif


/** */
/** InternetGatewayDevice.WANDevice.WANCommonInterfaceConfig. */

TRX_STATUS setWANDeviceWANCommonInterfaceConfig_EnabledForInternet(char *value)
{
#if 0
	uint32 objIndex = 0;
	void *objValue;   
	BcmCfm_CfgStatus	enableStatus = BcmCfm_CfgDisabled;

	if (strcmp(value, "1") == 0)
		enableStatus = BcmCfm_CfgEnabled;
	while ( BcmCfm_objGet(BCMCFM_OBJ_NTWK_INTF, &objValue, &objIndex) == BcmCfm_Ok ) {
		PBcmCfm_NtwkIntfCfg_t pObj = (PBcmCfm_NtwkIntfCfg_t)objValue;
		if (pObj != NULL) {
			pObj->status = enableStatus;
			if ( BcmCfm_objSet(BCMCFM_OBJ_NTWK_INTF, objValue, objIndex) != BcmCfm_Ok ) {
				BcmCfm_objFree(BCMCFM_OBJ_NTWK_INTF, objValue);
				return TRX_ERR;
			}
		}
		objIndex++;                        
	}
#endif
	BSTD_UNUSED(value);
	return TRX_REBOOT;
}
TRX_STATUS getWANDeviceWANCommonInterfaceConfig_EnabledForInternet(char **value)
{
	*value=strdup("1");
	
	return TRX_OK;
}
TRX_STATUS getWANDeviceWANCommonInterfaceConfig_WANAccessType(char **value)
{
	*value = strdup("DSL");
	return TRX_OK;
}
TRX_STATUS getWANDeviceWANCommonInterfaceConfig_Layer1UpstreamMaxBitRate(char **value)
{
#if 0
	void *obj;
	uint32 index = 0;
        
	if ( BcmCfm_stsGet(BCMCFM_OBJ_IFC_ADSL, &obj, &index) == BcmCfm_Ok ) {
		PBcmCfm_AdslIfcSts_t pObj = (PBcmCfm_AdslIfcSts_t)obj;
		if ( pObj->linkState == BcmCfm_AdslLinkUp ) {
			char buf[IFC_TINY_LEN];
			sprintf(buf, "%lu", pObj->adslPhysXmt.adslCurrAttainableRate);
			 *value = strdup(buf);
		} else                         
			*value = strdup("0");
		BcmCfm_stsFree(BCMCFM_OBJ_IFC_ADSL, pObj);
		return TRX_OK;
	}
#endif        
	BSTD_UNUSED(value);
	return TRX_ERR;
}
TRX_STATUS getWANDeviceWANCommonInterfaceConfig_Layer1DownstreamMaxBitRate(char **value)
{
#if 0
	void *obj;
	uint32 index = 0;
        
	if ( BcmCfm_stsGet(BCMCFM_OBJ_IFC_ADSL, &obj, &index) == BcmCfm_Ok ) {
		PBcmCfm_AdslIfcSts_t pObj = (PBcmCfm_AdslIfcSts_t)obj;
		if ( pObj->linkState == BcmCfm_AdslLinkUp ) {
			char buf[IFC_TINY_LEN];
			sprintf(buf, "%lu", pObj->adslPhysRcv.adslCurrAttainableRate);
			 *value = strdup(buf);
		} else                         
			*value = strdup("0");
		BcmCfm_stsFree(BCMCFM_OBJ_IFC_ADSL, pObj);
		return TRX_OK;
	}
#endif
        
	BSTD_UNUSED(value);
	return TRX_ERR;
}
TRX_STATUS getWANDeviceWANCommonInterfaceConfig_PhysicalLinkStatus(char **value)
{
#if 0
	void *obj;
	uint32 index = 0;
        
	if ( BcmCfm_stsGet(BCMCFM_OBJ_IFC_ADSL, &obj, &index) == BcmCfm_Ok ) {
		PBcmCfm_AdslIfcSts_t pObj = (PBcmCfm_AdslIfcSts_t)obj;
		switch ( pObj->linkState ) {
		case BcmCfm_AdslLinkUp:
			*value = strdup("Up");
			break;                        
		case BcmCfm_AdslLinkDown:
			*value = strdup("Down");
			break;                        
		case BcmCfm_AdslTrainingExchange:
		case BcmCfm_AdslTrainingChannelAnalysis:
		case BcmCfm_AdslTrainingStarted:
		case BcmCfm_AdslTrainingG994:
		case BcmCfm_AdslTrainingG994NonstdInfoRx:
		case BcmCfm_AdslAtmIdle:
			*value = strdup("Initializing");
			break;                        
		default:                         
			*value = strdup("Unavailable");
			break;                        
		}
		BcmCfm_stsFree(BCMCFM_OBJ_IFC_ADSL, pObj);
		return TRX_OK;
	}
#endif        
	BSTD_UNUSED(value);
	return TRX_ERR;
}
TRX_STATUS getWANDeviceWANCommonInterfaceConfig_WANAccessProvider(char **value)
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
TRX_STATUS getWANDeviceWANCommonInterfaceConfig_TotalBytesSent(char **value)
{
#if 0
	unsigned long wanSts[OBJ_STS_IFC_COLS];
	char buf[IFC_TINY_LEN];
      
	BcmDb_getIfcStats(STS_FROM_WAN, "all", wanSts);
	sprintf(buf, "%lu", wanSts[OBJ_STS_TX_BYTE]);
	*value = strdup(buf);
#endif        
	BSTD_UNUSED(value);
	return TRX_OK;
}
TRX_STATUS getWANDeviceWANCommonInterfaceConfig_TotalBytesReceived(char **value)
{
#if 0
	unsigned long wanSts[OBJ_STS_IFC_COLS];
	char buf[IFC_TINY_LEN];
      
	BcmDb_getIfcStats(STS_FROM_WAN, "all", wanSts);
	sprintf(buf, "%lu", wanSts[OBJ_STS_RX_BYTE]);
	*value = strdup(buf);
#endif        
	BSTD_UNUSED(value);
	return TRX_OK;
}
TRX_STATUS getWANDeviceWANCommonInterfaceConfig_TotalPacketsSent(char **value)
{
#if 0
	unsigned long wanSts[OBJ_STS_IFC_COLS];
	char buf[IFC_TINY_LEN];
      
	BcmDb_getIfcStats(STS_FROM_WAN, "all", wanSts);
	sprintf(buf, "%lu", wanSts[OBJ_STS_TX_PACKET]);
	*value = strdup(buf);
#endif        
	BSTD_UNUSED(value);
	return TRX_OK;
}
TRX_STATUS getWANDeviceWANCommonInterfaceConfig_TotalPacketsReceived(char **value)
{
#if 0
	unsigned long wanSts[OBJ_STS_IFC_COLS];
	char buf[IFC_TINY_LEN];
      
	BcmDb_getIfcStats(STS_FROM_WAN, "all", wanSts);
	sprintf(buf, "%lu", wanSts[OBJ_STS_RX_PACKET]);
	*value = strdup(buf);
#endif        
	BSTD_UNUSED(value);
	return TRX_OK;
}
#if 0
TRX_STATUS getWANDeviceWANCommonInterfaceConfig_MaximumActiveConnections(char **value)
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
		return TRX_OK;
}
TRX_STATUS getWANDeviceWANCommonInterfaceConfig_NumberOfActiveConnections(char **value)
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
		return TRX_OK;
}
#endif /* 0 */

/** */
/** InternetGatewayDevice.WANDevice.WANDSLInterfaceConfig.Stats.Total. */
TRX_STATUS getWanDslStats(char **value, int statField) 
{
#if 0
  	void *obj;
	uint32 index = 0;
        uint32 count;
	char buf[IFC_TINY_LEN];

	if ( BcmCfm_stsGet(BCMCFM_OBJ_IFC_ADSL, &obj, &index) == BcmCfm_Ok ) {
		PBcmCfm_AdslIfcSts_t pObj = (PBcmCfm_AdslIfcSts_t)obj;
		switch (statField)
		  {
		  case DSL_TOTAL_CHAN_RX_BLKS:
		    sprintf(buf,"%lu",pObj->adslStatSincePowerOn.rcvStat.cntSF);
		    break;
		  case DSL_TOTAL_CHAN_TX_BLKS:
		    sprintf(buf,"%lu",pObj->adslStatSincePowerOn.xmtStat.cntSF);
		    break;
		  case DSL_TOTAL_CELL_DELIN:
		    count = pObj->atmStatSincePowerOn.rcvStat.cntOCD + pObj->atmStatSincePowerOn.rcvStat.cntLCD;
		    sprintf(buf,"%lu",count);
		    break;
 		  case DSL_TOTAL_LINK_RETRAIN:
		    sprintf(buf,"%lu",pObj->adslRxPerfData.failTotal.adslRetr);
		    break;
		  case DSL_TOTAL_INIT_ERROR:
		    sprintf(buf,"%lu",pObj->adslRxPerfData.failTotal.adslInitErr);
		    break;
		  case DSL_TOTAL_INIT_TIMEOUT:
		    sprintf(buf,"%lu",pObj->adslRxPerfData.failTotal.adslInitTo);
		    break;
		  case DSL_TOTAL_LOFS:
		    sprintf(buf,"%lu",pObj->adslRxPerfData.perfTotal.adslLofs);
		    break;
		  case DSL_TOTAL_ES:
		    sprintf(buf,"%lu",pObj->adslRxPerfData.perfTotal.adslESs);
		    break;
		  case DSL_TOTAL_SES:
		    sprintf(buf,"%lu",pObj->adslRxPerfData.perfTotal.adslSES);
		    break;
		  case DSL_TOTAL_FEC:
		    sprintf(buf,"%lu",pObj->adslStatSincePowerOn.rcvStat.cntRSCor);
		    break;
		  case DSL_TOTAL_ATUCFEC:
		    sprintf(buf,"%lu",pObj->adslStatSincePowerOn.xmtStat.cntRSCor);
		    break;
		  case DSL_TOTAL_HEC:
		    sprintf(buf,"%lu",pObj->atmConnStat.rcvStat.cntHEC);
		    break;
		  case DSL_TOTAL_ATUCHEC:
		    sprintf(buf,"%lu",pObj->atmConnStat.xmtStat.cntHEC);
		    break;
		  case DSL_TOTAL_CRC:
		    sprintf(buf,"%lu",pObj->adslStatSincePowerOn.rcvStat.cntSFErr);		    
		    break;
		  case DSL_TOTAL_ACTUCCRC:
		    sprintf(buf,"%lu",pObj->adslStatSincePowerOn.xmtStat.cntSFErr);		    
		    break;

		  case DSL_SHOWTIME_CHAN_RX_BLKS:
		    sprintf(buf,"%lu",pObj->adslConnStat.rcvStat.cntSF);
		    break;
		  case DSL_SHOWTIME_CHAN_TX_BLKS:
		    sprintf(buf,"%lu",pObj->adslConnStat.xmtStat.cntSF);
		    break;
		  case DSL_SHOWTIME_CELL_DELIN:
		    count = pObj->atmConnStat.rcvStat.cntOCD + pObj->atmConnStat.rcvStat.cntLCD;
		    sprintf(buf,"%lu",count);
		    break;
 		  case DSL_SHOWTIME_LINK_RETRAIN:
		    sprintf(buf,"%lu",pObj->perfSinceShowTime.failTotal.adslRetr);
		    break;
		  case DSL_SHOWTIME_INIT_ERROR:
		    sprintf(buf,"%lu",pObj->perfSinceShowTime.failTotal.adslInitErr);
		    break;
		  case DSL_SHOWTIME_INIT_TIMEOUT:
		    sprintf(buf,"%lu",pObj->perfSinceShowTime.failTotal.adslInitTo);
		    break;
		  case DSL_SHOWTIME_LOFS:
		    sprintf(buf,"%lu",pObj->perfSinceShowTime.perfTotal.adslLofs);
		    break;
		  case DSL_SHOWTIME_ES:
		    sprintf(buf,"%lu",pObj->perfSinceShowTime.perfTotal.adslESs);
		    break;
		  case DSL_SHOWTIME_SES:
		    sprintf(buf,"%lu",pObj->perfSinceShowTime.perfTotal.adslSES);
		    break;
		  case DSL_SHOWTIME_FEC:
		    sprintf(buf,"%lu",pObj->adslConnStat.rcvStat.cntRSCor);
		    break;
		  case DSL_SHOWTIME_ATUCFEC:
		    sprintf(buf,"%lu",pObj->adslConnStat.xmtStat.cntRSCor);
		    break;
		  case DSL_SHOWTIME_HEC:
		    sprintf(buf,"%lu",pObj->atmConnStat.rcvStat.cntHEC);
		    break;
		  case DSL_SHOWTIME_ATUCHEC:
		    sprintf(buf,"%lu",pObj->atmConnStat.xmtStat.cntHEC);
		    break;
		  case DSL_SHOWTIME_CRC:
		    sprintf(buf,"%lu",pObj->adslConnStat.rcvStat.cntSFErr);		    
		    break;
		  case DSL_SHOWTIME_ACTUCCRC:
		    sprintf(buf,"%lu",pObj->adslConnStat.xmtStat.cntSFErr);		    
		    break;
		  default:
		    sprintf(buf,"%d",0);
		  }
		*value = strdup(buf);
		BcmCfm_stsFree(BCMCFM_OBJ_IFC_ADSL, pObj);
		return TRX_OK;
	}
#endif
	BSTD_UNUSED(value);
	BSTD_UNUSED(statField);
	return TRX_ERR;
}

TRX_STATUS getWANDeviceWANDSLInterfaceConfigStatsTotal_ReceiveBlocks(char **value)
{
#if 0
        return (getWanDslStats(value,DSL_TOTAL_CHAN_RX_BLKS));
#endif
	BSTD_UNUSED(value);
	return TRX_OK;
}
TRX_STATUS getWANDeviceWANDSLInterfaceConfigStatsTotal_TransmitBlocks(char **value)
{
#if 0
        return (getWanDslStats(value,DSL_TOTAL_CHAN_TX_BLKS));
#endif
	BSTD_UNUSED(value);
	return TRX_OK;
}
TRX_STATUS getWANDeviceWANDSLInterfaceConfigStatsTotal_CellDelin(char **value)
{
#if 0
        return (getWanDslStats(value,DSL_TOTAL_CELL_DELIN));
#endif
	BSTD_UNUSED(value);
	return TRX_OK;
}
TRX_STATUS getWANDeviceWANDSLInterfaceConfigStatsTotal_LinkRetrain(char **value)
{
#if 0
        return (getWanDslStats(value,DSL_TOTAL_LINK_RETRAIN));
#endif
	BSTD_UNUSED(value);
	return TRX_OK;
}
TRX_STATUS getWANDeviceWANDSLInterfaceConfigStatsTotal_InitErrors(char **value)
{
#if 0
        return (getWanDslStats(value,DSL_TOTAL_INIT_ERROR));
#endif
	BSTD_UNUSED(value);
	return TRX_OK;
}
TRX_STATUS getWANDeviceWANDSLInterfaceConfigStatsTotal_InitTimeouts(char **value)
{
#if 0
        return (getWanDslStats(value,DSL_TOTAL_INIT_TIMEOUT));
#endif
	BSTD_UNUSED(value);
	return TRX_OK;
}
TRX_STATUS getWANDeviceWANDSLInterfaceConfigStatsTotal_LossOfFraming(char **value)
{
#if 0
        return (getWanDslStats(value,DSL_TOTAL_LOFS));
#endif
	BSTD_UNUSED(value);
	return TRX_OK;
}
TRX_STATUS getWANDeviceWANDSLInterfaceConfigStatsTotal_ErroredSecs(char **value)
{
#if 0
        return (getWanDslStats(value,DSL_TOTAL_ES));
#endif
	BSTD_UNUSED(value);
	return TRX_OK;
}
TRX_STATUS getWANDeviceWANDSLInterfaceConfigStatsTotal_SeverelyErroredSecs(char **value)
{
#if 0
        return (getWanDslStats(value,DSL_TOTAL_SES));
#endif
	BSTD_UNUSED(value);
	return TRX_OK;
}
TRX_STATUS getWANDeviceWANDSLInterfaceConfigStatsTotal_FECErrors(char **value)
{
#if 0
        return (getWanDslStats(value,DSL_TOTAL_FEC));
#endif
	BSTD_UNUSED(value);
	return TRX_OK;
}
TRX_STATUS getWANDeviceWANDSLInterfaceConfigStatsTotal_ATUCFECErrors(char **value)
{
#if 0
        return (getWanDslStats(value,DSL_TOTAL_ATUCFEC));
#endif
	BSTD_UNUSED(value);
	return TRX_OK;
}
TRX_STATUS getWANDeviceWANDSLInterfaceConfigStatsTotal_HECErrors(char **value)
{
#if 0
        return (getWanDslStats(value,DSL_TOTAL_HEC));
#endif
	BSTD_UNUSED(value);
	return TRX_OK;
}
TRX_STATUS getWANDeviceWANDSLInterfaceConfigStatsTotal_ATUCHECErrors(char **value)
{
#if 0
        return (getWanDslStats(value,DSL_TOTAL_ATUCHEC));
#endif
	BSTD_UNUSED(value);
	return TRX_OK;
}
TRX_STATUS getWANDeviceWANDSLInterfaceConfigStatsTotal_CRCErrors(char **value)
{
#if 0
        return (getWanDslStats(value,DSL_TOTAL_CRC));
#endif
	BSTD_UNUSED(value);
	return TRX_OK;
}
TRX_STATUS getWANDeviceWANDSLInterfaceConfigStatsTotal_ATUCCRCErrors(char **value)
{
#if 0
        return (getWanDslStats(value,DSL_TOTAL_ACTUCCRC));
#endif
	BSTD_UNUSED(value);
	return TRX_OK;
}

/** */
/** InternetGatewayDevice.WANDevice.WANDSLInterfaceConfig.Stats.Showtime. */

TRX_STATUS getWANDeviceWANDSLInterfaceConfigStatsShowtime_ReceiveBlocks(char **value)
{
#if 0
        return (getWanDslStats(value,DSL_SHOWTIME_CHAN_RX_BLKS));
#endif
	BSTD_UNUSED(value);
	return TRX_OK;
}
TRX_STATUS getWANDeviceWANDSLInterfaceConfigStatsShowtime_TransmitBlocks(char **value)
{
#if 0
        return (getWanDslStats(value,DSL_SHOWTIME_CHAN_TX_BLKS));
#endif
	BSTD_UNUSED(value);
	return TRX_OK;
}
TRX_STATUS getWANDeviceWANDSLInterfaceConfigStatsShowtime_CellDelin(char **value)
{
#if 0
        return (getWanDslStats(value,DSL_SHOWTIME_CELL_DELIN));
#endif
	BSTD_UNUSED(value);
	return TRX_OK;
}
TRX_STATUS getWANDeviceWANDSLInterfaceConfigStatsShowtime_LinkRetrain(char **value)
{
#if 0
        return (getWanDslStats(value,DSL_SHOWTIME_LINK_RETRAIN));
#endif
	BSTD_UNUSED(value);
	return TRX_OK;
}
TRX_STATUS getWANDeviceWANDSLInterfaceConfigStatsShowtime_InitErrors(char **value)
{
#if 0
        return (getWanDslStats(value,DSL_SHOWTIME_INIT_ERROR));
#endif
	BSTD_UNUSED(value);
	return TRX_OK;
}
TRX_STATUS getWANDeviceWANDSLInterfaceConfigStatsShowtime_InitTimeouts(char **value)
{
#if 0
        return (getWanDslStats(value,DSL_SHOWTIME_INIT_TIMEOUT));
#endif
	BSTD_UNUSED(value);
	return TRX_OK;
}
TRX_STATUS getWANDeviceWANDSLInterfaceConfigStatsShowtime_LossOfFraming(char **value)
{
#if 0
        return (getWanDslStats(value,DSL_SHOWTIME_LOFS));
#endif
	BSTD_UNUSED(value);
	return TRX_OK;
}
TRX_STATUS getWANDeviceWANDSLInterfaceConfigStatsShowtime_ErroredSecs(char **value)
{
#if 0
        return (getWanDslStats(value,DSL_SHOWTIME_ES));
#endif
	BSTD_UNUSED(value);
	return TRX_OK;
}
TRX_STATUS getWANDeviceWANDSLInterfaceConfigStatsShowtime_SeverelyErroredSecs(char **value)
{
#if 0
        return (getWanDslStats(value,DSL_SHOWTIME_SES));
#endif
	BSTD_UNUSED(value);
	return TRX_OK;
}
TRX_STATUS getWANDeviceWANDSLInterfaceConfigStatsShowtime_FECErrors(char **value)
{
#if 0
        return (getWanDslStats(value,DSL_SHOWTIME_FEC));
#endif
	BSTD_UNUSED(value);
	return TRX_OK;
}
TRX_STATUS getWANDeviceWANDSLInterfaceConfigStatsShowtime_ATUCFECErrors(char **value)
{
#if 0
        return (getWanDslStats(value,DSL_SHOWTIME_ATUCFEC));
#endif
	BSTD_UNUSED(value);
	return TRX_OK;
}
TRX_STATUS getWANDeviceWANDSLInterfaceConfigStatsShowtime_HECErrors(char **value)
{
#if 0
        return (getWanDslStats(value,DSL_SHOWTIME_HEC));
#endif
	BSTD_UNUSED(value);
	return TRX_OK;
}
TRX_STATUS getWANDeviceWANDSLInterfaceConfigStatsShowtime_ATUCHECErrors(char **value)
{
#if 0
        return (getWanDslStats(value,DSL_SHOWTIME_ATUCHEC));
#endif
	BSTD_UNUSED(value);
	return TRX_OK;
}
TRX_STATUS getWANDeviceWANDSLInterfaceConfigStatsShowtime_CRCErrors(char **value)
{
#if 0
        return (getWanDslStats(value,DSL_SHOWTIME_CRC));
#endif
	BSTD_UNUSED(value);
	return TRX_OK;
}
TRX_STATUS getWANDeviceWANDSLInterfaceConfigStatsShowtime_ATUCCRCErrors(char **value)
{
#if 0
        return (getWanDslStats(value,DSL_SHOWTIME_ACTUCCRC));
#endif
	BSTD_UNUSED(value);
	return TRX_OK;
}

#if 0
/** */
/** InternetGatewayDevice.WANDevice.WANDSLInterfaceConfig.Stats.LastShowtime. */

TRX_STATUS getWANDeviceWANDSLInterfaceConfigStatsLastShowtime_ReceiveBlocks(char **value)
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
TRX_STATUS getWANDeviceWANDSLInterfaceConfigStatsLastShowtime_TransmitBlocks(char **value)
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
TRX_STATUS getWANDeviceWANDSLInterfaceConfigStatsLastShowtime_CellDelin(char **value)
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
TRX_STATUS getWANDeviceWANDSLInterfaceConfigStatsLastShowtime_LinkRetrain(char **value)
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
TRX_STATUS getWANDeviceWANDSLInterfaceConfigStatsLastShowtime_InitErrors(char **value)
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
TRX_STATUS getWANDeviceWANDSLInterfaceConfigStatsLastShowtime_InitTimeouts(char **value)
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
TRX_STATUS getWANDeviceWANDSLInterfaceConfigStatsLastShowtime_LossOfFraming(char **value)
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
TRX_STATUS getWANDeviceWANDSLInterfaceConfigStatsLastShowtime_ErroredSecs(char **value)
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
TRX_STATUS getWANDeviceWANDSLInterfaceConfigStatsLastShowtime_SeverelyErroredSecs(char **value)
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
TRX_STATUS getWANDeviceWANDSLInterfaceConfigStatsLastShowtime_FECErrors(char **value)
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
TRX_STATUS getWANDeviceWANDSLInterfaceConfigStatsLastShowtime_ATUCFECErrors(char **value)
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
TRX_STATUS getWANDeviceWANDSLInterfaceConfigStatsLastShowtime_HECErrors(char **value)
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
TRX_STATUS getWANDeviceWANDSLInterfaceConfigStatsLastShowtime_ATUCHECErrors(char **value)
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
TRX_STATUS getWANDeviceWANDSLInterfaceConfigStatsLastShowtime_CRCErrors(char **value)
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
TRX_STATUS getWANDeviceWANDSLInterfaceConfigStatsLastShowtime_ATUCCRCErrors(char **value)
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
/** InternetGatewayDevice.WANDevice.WANDSLInterfaceConfig.Stats.CurrentDay. */

TRX_STATUS getWANDeviceWANDSLInterfaceConfigStatsCurrentDay_ReceiveBlocks(char **value)
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
TRX_STATUS getWANDeviceWANDSLInterfaceConfigStatsCurrentDay_TransmitBlocks(char **value)
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
TRX_STATUS getWANDeviceWANDSLInterfaceConfigStatsCurrentDay_CellDelin(char **value)
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
TRX_STATUS getWANDeviceWANDSLInterfaceConfigStatsCurrentDay_LinkRetrain(char **value)
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
TRX_STATUS getWANDeviceWANDSLInterfaceConfigStatsCurrentDay_InitErrors(char **value)
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
TRX_STATUS getWANDeviceWANDSLInterfaceConfigStatsCurrentDay_InitTimeouts(char **value)
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
TRX_STATUS getWANDeviceWANDSLInterfaceConfigStatsCurrentDay_LossOfFraming(char **value)
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
TRX_STATUS getWANDeviceWANDSLInterfaceConfigStatsCurrentDay_ErroredSecs(char **value)
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
TRX_STATUS getWANDeviceWANDSLInterfaceConfigStatsCurrentDay_SeverelyErroredSecs(char **value)
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
TRX_STATUS getWANDeviceWANDSLInterfaceConfigStatsCurrentDay_FECErrors(char **value)
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
TRX_STATUS getWANDeviceWANDSLInterfaceConfigStatsCurrentDay_ATUCFECErrors(char **value)
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
TRX_STATUS getWANDeviceWANDSLInterfaceConfigStatsCurrentDay_HECErrors(char **value)
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
TRX_STATUS getWANDeviceWANDSLInterfaceConfigStatsCurrentDay_ATUCHECErrors(char **value)
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
TRX_STATUS getWANDeviceWANDSLInterfaceConfigStatsCurrentDay_CRCErrors(char **value)
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
TRX_STATUS getWANDeviceWANDSLInterfaceConfigStatsCurrentDay_ATUCCRCErrors(char **value)
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
/** InternetGatewayDevice.WANDevice.WANDSLInterfaceConfig.Stats.QuarterHour. */

TRX_STATUS getWANDeviceWANDSLInterfaceConfigStatsQuarterHour_ReceiveBlocks(char **value)
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
TRX_STATUS getWANDeviceWANDSLInterfaceConfigStatsQuarterHour_TransmitBlocks(char **value)
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
TRX_STATUS getWANDeviceWANDSLInterfaceConfigStatsQuarterHour_CellDelin(char **value)
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
TRX_STATUS getWANDeviceWANDSLInterfaceConfigStatsQuarterHour_LinkRetrain(char **value)
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
TRX_STATUS getWANDeviceWANDSLInterfaceConfigStatsQuarterHour_InitErrors(char **value)
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
TRX_STATUS getWANDeviceWANDSLInterfaceConfigStatsQuarterHour_InitTimeouts(char **value)
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
TRX_STATUS getWANDeviceWANDSLInterfaceConfigStatsQuarterHour_LossOfFraming(char **value)
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
TRX_STATUS getWANDeviceWANDSLInterfaceConfigStatsQuarterHour_ErroredSecs(char **value)
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
TRX_STATUS getWANDeviceWANDSLInterfaceConfigStatsQuarterHour_SeverelyErroredSecs(char **value)
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
TRX_STATUS getWANDeviceWANDSLInterfaceConfigStatsQuarterHour_FECErrors(char **value)
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
TRX_STATUS getWANDeviceWANDSLInterfaceConfigStatsQuarterHour_ATUCFECErrors(char **value)
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
TRX_STATUS getWANDeviceWANDSLInterfaceConfigStatsQuarterHour_HECErrors(char **value)
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
TRX_STATUS getWANDeviceWANDSLInterfaceConfigStatsQuarterHour_ATUCHECErrors(char **value)
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
TRX_STATUS getWANDeviceWANDSLInterfaceConfigStatsQuarterHour_CRCErrors(char **value)
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
TRX_STATUS getWANDeviceWANDSLInterfaceConfigStatsQuarterHour_ATUCCRCErrors(char **value)
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
/** InternetGatewayDevice.WANDevice.WANDSLInterfaceConfig.Stats. */
#endif

/** */
/** InternetGatewayDevice.WANDevice.WANDSLInterfaceConfig. */

TRX_STATUS setWANDeviceWANDSLInterfaceConfig_Enable(char *value)
{
#if 0
	void *obj;
	uint32 index = 0;

	if ( BcmCfm_objGet(BCMCFM_OBJ_IFC_ADSL, &obj, &index) == BcmCfm_Ok ) {
		PBcmCfm_AdslIfcCfg_t pObj = (PBcmCfm_AdslIfcCfg_t)obj;
		if ( strcmp(value, "1") == 0 ) {
			if (pObj->enabled == BcmCfm_CfgEnabled) {
				BcmCfm_objFree(BCMCFM_OBJ_IFC_ADSL, pObj);
				return TRX_OK;
			} else {
				pObj->enabled = BcmCfm_CfgEnabled;
      		     		bcmSystem("adslctl connection --up");
			}		
		}
		else if ( strcmp(value, "0") == 0 ) {
			pObj->enabled = BcmCfm_CfgDisabled;
			bcmSystem("adslctl connection --down");
		}
		else {
			BcmCfm_objFree(BCMCFM_OBJ_IFC_ADSL, pObj);
			return TRX_ERR;
		}
		if ( BcmCfm_objSet(BCMCFM_OBJ_IFC_ADSL, obj, index) == BcmCfm_Ok )
			return TRX_OK;
	}
#endif        
	BSTD_UNUSED(value);
	return TRX_ERR;
}
TRX_STATUS getWANDeviceWANDSLInterfaceConfig_Enable(char **value)
{
#if 0
	void *obj;
	uint32 index = 0;
        
	if ( BcmCfm_objGet(BCMCFM_OBJ_IFC_ADSL, &obj, &index) == BcmCfm_Ok ) {
		PBcmCfm_AdslIfcCfg_t pObj = (PBcmCfm_AdslIfcCfg_t)obj;
		if ( pObj->enabled == BcmCfm_CfgDisabled )
			*value = strdup("0");
		else                        
			*value = strdup("1");
		BcmCfm_objFree(BCMCFM_OBJ_IFC_ADSL, pObj);
		return TRX_OK;
	}
#endif        
	BSTD_UNUSED(value);
	return TRX_ERR;
}
TRX_STATUS getWANDeviceWANDSLInterfaceConfig_Status(char **value)
{
#if 0
	void *obj;
	uint32 index = 0;
        
	if ( BcmCfm_objGet(BCMCFM_OBJ_IFC_ADSL, &obj, &index) == BcmCfm_Ok ) {
		PBcmCfm_AdslIfcCfg_t pObj = (PBcmCfm_AdslIfcCfg_t)obj;
		if ( pObj->enabled == BcmCfm_CfgDisabled ) {
			*value = strdup("Disabled");
			BcmCfm_objFree(BCMCFM_OBJ_IFC_ADSL, pObj);
			return TRX_OK;
		} else
			BcmCfm_objFree(BCMCFM_OBJ_IFC_ADSL, pObj);
	}
        
	index = 0;
        
	if ( BcmCfm_stsGet(BCMCFM_OBJ_IFC_ADSL, &obj, &index) == BcmCfm_Ok ) {
		PBcmCfm_AdslIfcSts_t pObj = (PBcmCfm_AdslIfcSts_t)obj;
		switch ( pObj->linkState ) {
		case BcmCfm_AdslLinkUp:                
			*value = strdup("Up");
			break;                        
		case BcmCfm_AdslLinkDown:                
		case BcmCfm_AdslAtmIdle:                
			*value = strdup("NoSignal");
			break;                        
		case BcmCfm_AdslTrainingExchange:                
		case BcmCfm_AdslTrainingChannelAnalysis:                
		case BcmCfm_AdslTrainingStarted:                
		case BcmCfm_AdslTrainingG994:                
		case BcmCfm_AdslTrainingG994NonstdInfoRx:                
			*value = strdup("EstablishingLink");
			break;                        
		default:                
			*value = strdup("Error");
			break;                        
		}                         
		BcmCfm_stsFree(BCMCFM_OBJ_IFC_ADSL, pObj);
		return TRX_OK;
	}
#endif        
	BSTD_UNUSED(value);
	return TRX_ERR;
}
TRX_STATUS getWANDeviceWANDSLInterfaceConfig_ModulationType(char **value)
{
#if 0
	void *obj;
	uint32 index = 0;
        
	if ( BcmCfm_stsGet(BCMCFM_OBJ_IFC_ADSL, &obj, &index) == BcmCfm_Ok ) {
		PBcmCfm_AdslIfcSts_t pObj = (PBcmCfm_AdslIfcSts_t)obj;
		switch ( pObj->adslConnInfo.modType ) {
		case kAdslModGdmt:                
			*value = strdup("ADSL_G.dmt");
			break;                        
		case kAdslModT1413:                
			*value = strdup("ADSL_ANSI_T1.413");
			break;                        
		case kAdslModGlite:                
			*value = strdup("ADSL_G.lite");
			break;                        
		case kAdslModAdsl2:
			*value = strdup("ADSL_2");
			break;                        
		case kAdslModAdsl2p:
			*value = strdup("ADSL_2plus");
			break;                        
		case kAdslModReAdsl2:
			*value = strdup("ADSL_re-adsl");
			break;                        
		default:                
		 *value = strdup("");
			break;                        
		}                         
		BcmCfm_stsFree(BCMCFM_OBJ_IFC_ADSL, pObj);
		return TRX_OK;
	}
#endif        
	BSTD_UNUSED(value);
	return TRX_ERR;
}
TRX_STATUS getWANDeviceWANDSLInterfaceConfig_LineEncoding(char **value)
{
#if 0
	void *obj;
	uint32 index = 0;
        
	if ( BcmCfm_stsGet(BCMCFM_OBJ_IFC_ADSL, &obj, &index) == BcmCfm_Ok ) {
		PBcmCfm_AdslIfcSts_t pObj = (PBcmCfm_AdslIfcSts_t)obj;
		switch ( pObj->adslLine.adslLineCoding ) {
		case kAdslLineCodingDMT:                
			*value = strdup("DMT");
			break;                        
		case kAdslLineCodingCAP:                
			*value = strdup("CAP");
			break;                        
		case kAdslLineCodingQAM:                
			*value = strdup("QAM");
			break;                        
		default:                
		 *value = strdup("");
			break;                        
		}                         
		BcmCfm_stsFree(BCMCFM_OBJ_IFC_ADSL, pObj);
		return TRX_OK;
	}
#endif        
	BSTD_UNUSED(value);
	return TRX_ERR;
}
TRX_STATUS getWANDeviceWANDSLInterfaceConfig_DataPath(char **value)
{
#if 0
	void *obj;
	uint32 index = 0;
        
	if ( BcmCfm_stsGet(BCMCFM_OBJ_IFC_ADSL, &obj, &index) == BcmCfm_Ok ) {
		PBcmCfm_AdslIfcSts_t pObj = (PBcmCfm_AdslIfcSts_t)obj;
		switch ( pObj->adslConnInfo.chType ) {
		case kAdslFastChannel:                
			*value = strdup("Fast");
			break;                        
		case kAdslIntlChannel:                
			*value = strdup("Interleaved");
			break;                        
		default:                
		 *value = strdup("");
			break;                        
		}                         
		BcmCfm_stsFree(BCMCFM_OBJ_IFC_ADSL, pObj);
		return TRX_OK;
	}
#endif        
	BSTD_UNUSED(value);
	return TRX_ERR;
}
TRX_STATUS getWANDeviceWANDSLInterfaceConfig_InterleaveDepth(char **value)
{
#if 0
	void *obj;
	uint32 index = 0;
        
	if ( BcmCfm_stsGet(BCMCFM_OBJ_IFC_ADSL, &obj, &index) == BcmCfm_Ok ) {
		PBcmCfm_AdslIfcSts_t pObj = (PBcmCfm_AdslIfcSts_t)obj;
		if ( pObj->adslConnInfo.chType == kAdslIntlChannel ) {
			char buf[IFC_TINY_LEN];
			sprintf(buf, "%d",pObj->adslConnInfo.rcvInfo.D);                                         
			*value = strdup(buf);
		} else
			*value = strdup("0");
		BcmCfm_stsFree(BCMCFM_OBJ_IFC_ADSL, pObj);
		return TRX_OK;
	}
#endif        
	BSTD_UNUSED(value);
	return TRX_ERR;
}
TRX_STATUS getWANDeviceWANDSLInterfaceConfig_LineNumber(char **value)
{
#if 0
	void *obj;
	uint32 index = 0;
        
	if ( BcmCfm_objGet(BCMCFM_OBJ_IFC_ADSL, &obj, &index) == BcmCfm_Ok ) {
		PBcmCfm_AdslIfcCfg_t pObj = (PBcmCfm_AdslIfcCfg_t)obj;
		if ( pObj->lpair & kAdslCfgLineOuterPair )
			*value = strdup("0");
		else                        
			*value = strdup("1");
		BcmCfm_objFree(BCMCFM_OBJ_IFC_ADSL, pObj);
		return TRX_OK;
	}
#endif        
	BSTD_UNUSED(value);
	return TRX_ERR;
}
TRX_STATUS getWANDeviceWANDSLInterfaceConfig_UpstreamCurrRate(char **value)
{
#if 0
	void *obj;
	uint32 index = 0;
        
	if ( BcmCfm_stsGet(BCMCFM_OBJ_IFC_ADSL, &obj, &index) == BcmCfm_Ok ) {
		PBcmCfm_AdslIfcSts_t pObj = (PBcmCfm_AdslIfcSts_t)obj;
		if ( pObj->linkState == BcmCfm_AdslLinkUp ) {
			char buf[IFC_TINY_LEN];
			sprintf(buf, "%lu", pObj->upStreamRate / 1000);
			 *value = strdup(buf);
		} else                         
			*value = strdup("0");
		BcmCfm_stsFree(BCMCFM_OBJ_IFC_ADSL, pObj);
		return TRX_OK;
	}
#endif        
	BSTD_UNUSED(value);
	return TRX_ERR;
}
TRX_STATUS getWANDeviceWANDSLInterfaceConfig_DownstreamCurrRate(char **value)
{
#if 0
	void *obj;
	uint32 index = 0;
        
	if ( BcmCfm_stsGet(BCMCFM_OBJ_IFC_ADSL, &obj, &index) == BcmCfm_Ok ) {
		PBcmCfm_AdslIfcSts_t pObj = (PBcmCfm_AdslIfcSts_t)obj;
		if ( pObj->linkState == BcmCfm_AdslLinkUp ) {
			char buf[IFC_TINY_LEN];
			sprintf(buf, "%lu", pObj->downStreamRate / 1000);
			 *value = strdup(buf);
		} else                         
			*value = strdup("0");
		BcmCfm_stsFree(BCMCFM_OBJ_IFC_ADSL, pObj);
		return TRX_OK;
	}
#endif        
	BSTD_UNUSED(value);
	return TRX_ERR;
}
TRX_STATUS getWANDeviceWANDSLInterfaceConfig_UpstreamMaxRate(char **value)
{
#if 0
	void *obj;
	uint32 index = 0;
        
	if ( BcmCfm_stsGet(BCMCFM_OBJ_IFC_ADSL, &obj, &index) == BcmCfm_Ok ) {
		PBcmCfm_AdslIfcSts_t pObj = (PBcmCfm_AdslIfcSts_t)obj;
		if ( pObj->linkState == BcmCfm_AdslLinkUp ) {
			char buf[IFC_TINY_LEN];
			sprintf(buf, "%lu", pObj->adslPhysXmt.adslCurrAttainableRate / 1000);
			 *value = strdup(buf);
		} else                         
			*value = strdup("0");
		BcmCfm_stsFree(BCMCFM_OBJ_IFC_ADSL, pObj);
		return TRX_OK;
	}
#endif        
	BSTD_UNUSED(value);
	return TRX_ERR;
}
TRX_STATUS getWANDeviceWANDSLInterfaceConfig_DownstreamMaxRate(char **value)
{
#if 0
	void *obj;
	uint32 index = 0;
        
	if ( BcmCfm_stsGet(BCMCFM_OBJ_IFC_ADSL, &obj, &index) == BcmCfm_Ok ) {
		PBcmCfm_AdslIfcSts_t pObj = (PBcmCfm_AdslIfcSts_t)obj;
		if ( pObj->linkState == BcmCfm_AdslLinkUp ) {
			char buf[IFC_TINY_LEN];
			sprintf(buf, "%lu", pObj->adslPhysRcv.adslCurrAttainableRate / 1000);
			 *value = strdup(buf);
		} else                         
			*value = strdup("0");
		BcmCfm_stsFree(BCMCFM_OBJ_IFC_ADSL, pObj);
		return TRX_OK;
	}
#endif        
	BSTD_UNUSED(value);
	return TRX_ERR;
}
TRX_STATUS getWANDeviceWANDSLInterfaceConfig_UpstreamNoiseMargin(char **value)
{
#if 0
	void *obj;
	uint32 index = 0;
        
	if ( BcmCfm_stsGet(BCMCFM_OBJ_IFC_ADSL, &obj, &index) == BcmCfm_Ok ) {
		PBcmCfm_AdslIfcSts_t pObj = (PBcmCfm_AdslIfcSts_t)obj;
		if ( pObj->linkState == BcmCfm_AdslLinkUp ) {
			char buf[IFC_TINY_LEN];
			sprintf(buf, "%d", (int)(pObj->adslPhysXmt.adslCurrSnrMgn / 10.0));
			 *value = strdup(buf);
		} else                         
			*value = strdup("0");
		BcmCfm_stsFree(BCMCFM_OBJ_IFC_ADSL, pObj);
		return TRX_OK;
	}
#endif        
	BSTD_UNUSED(value);
	return TRX_ERR;
}
TRX_STATUS getWANDeviceWANDSLInterfaceConfig_DownstreamNoiseMargin(char **value)
{
#if 0
	void *obj;
	uint32 index = 0;
        
	if ( BcmCfm_stsGet(BCMCFM_OBJ_IFC_ADSL, &obj, &index) == BcmCfm_Ok ) {
		PBcmCfm_AdslIfcSts_t pObj = (PBcmCfm_AdslIfcSts_t)obj;
		if ( pObj->linkState == BcmCfm_AdslLinkUp ) {
			char buf[IFC_TINY_LEN];
			sprintf(buf, "%d", (int)(pObj->adslPhysRcv.adslCurrSnrMgn / 10.0));
			 *value = strdup(buf);
		} else                         
			*value = strdup("0");
		BcmCfm_stsFree(BCMCFM_OBJ_IFC_ADSL, pObj);
		return TRX_OK;
	}
#endif        
	BSTD_UNUSED(value);
	return TRX_ERR;
}
TRX_STATUS getWANDeviceWANDSLInterfaceConfig_UpstreamAttenuation(char **value)
{
#if 0
	void *obj;
	uint32 index = 0;
        
	if ( BcmCfm_stsGet(BCMCFM_OBJ_IFC_ADSL, &obj, &index) == BcmCfm_Ok ) {
		PBcmCfm_AdslIfcSts_t pObj = (PBcmCfm_AdslIfcSts_t)obj;
		if ( pObj->linkState == BcmCfm_AdslLinkUp ) {
			char buf[IFC_TINY_LEN];
			sprintf(buf, "%d", (int)(pObj->adslPhysXmt.adslCurrAtn / 10.0));
			 *value = strdup(buf);
		} else                         
			*value = strdup("0");
		BcmCfm_stsFree(BCMCFM_OBJ_IFC_ADSL, pObj);
		return TRX_OK;
	}
#endif        
	BSTD_UNUSED(value);
	return TRX_ERR;
}
TRX_STATUS getWANDeviceWANDSLInterfaceConfig_DownstreamAttenuation(char **value)
{
#if 0
	void *obj;
	uint32 index = 0;
        
	if ( BcmCfm_stsGet(BCMCFM_OBJ_IFC_ADSL, &obj, &index) == BcmCfm_Ok ) {
		PBcmCfm_AdslIfcSts_t pObj = (PBcmCfm_AdslIfcSts_t)obj;
		if ( pObj->linkState == BcmCfm_AdslLinkUp ) {
			char buf[IFC_TINY_LEN];
			sprintf(buf, "%d", (int)(pObj->adslPhysRcv.adslCurrAtn / 10.0));
			*value = strdup(buf);
		} else                         
			*value = strdup("0");
		BcmCfm_stsFree(BCMCFM_OBJ_IFC_ADSL, pObj);
		return TRX_OK;
	}
#endif        
	BSTD_UNUSED(value);
	return TRX_ERR;
}
TRX_STATUS getWANDeviceWANDSLInterfaceConfig_UpstreamPower(char **value)
{
#if 0
	void *obj;
	uint32 index = 0;
        
	if ( BcmCfm_stsGet(BCMCFM_OBJ_IFC_ADSL, &obj, &index) == BcmCfm_Ok ) {
		PBcmCfm_AdslIfcSts_t pObj = (PBcmCfm_AdslIfcSts_t)obj;
		if ( pObj->linkState == BcmCfm_AdslLinkUp ) {
			char buf[IFC_TINY_LEN];
			sprintf(buf, "%d", (int)(pObj->adslPhysXmt.adslCurrOutputPwr / 10.0));
			 *value = strdup(buf);
		} else                         
			*value = strdup("0");
		BcmCfm_stsFree(BCMCFM_OBJ_IFC_ADSL, pObj);
		return TRX_OK;
	}
#endif        
	BSTD_UNUSED(value);
	return TRX_ERR;
}
TRX_STATUS getWANDeviceWANDSLInterfaceConfig_DownstreamPower(char **value)
{
#if 0
	void *obj;
	uint32 index = 0;
        
	if ( BcmCfm_stsGet(BCMCFM_OBJ_IFC_ADSL, &obj, &index) == BcmCfm_Ok ) {
		PBcmCfm_AdslIfcSts_t pObj = (PBcmCfm_AdslIfcSts_t)obj;
		if ( pObj->linkState == BcmCfm_AdslLinkUp ) {
			char buf[IFC_TINY_LEN];
			sprintf(buf, "%d", (int)(pObj->adslPhysRcv.adslCurrOutputPwr / 10.0));
			 *value = strdup(buf);
		} else                         
			*value = strdup("0");
		BcmCfm_stsFree(BCMCFM_OBJ_IFC_ADSL, pObj);
		return TRX_OK;
	}
#endif        
	BSTD_UNUSED(value);
	return TRX_ERR;
}

TRX_STATUS getWANDeviceWANDSLInterfaceConfig_ATURVendor(char **value)
{
#if 0
	void *obj;
	uint32 index = 0;
	char vendor[8];
	char *adslVendor;

	if ( BcmCfm_stsGet(BCMCFM_OBJ_IFC_ADSL, &obj, &index) == BcmCfm_Ok ) {
		PBcmCfm_AdslIfcSts_t pObj = (PBcmCfm_AdslIfcSts_t)obj;
		if ( pObj->linkState == BcmCfm_AdslLinkUp ) {
		  adslVendor = pObj->adslPhysRcv.adslVendorID;
		  sprintf(vendor,"%c%c%c%c%02x%02x",
			  (adslVendor[2]),
			  (adslVendor[3]),(adslVendor[4]),(adslVendor[5]),
			  (adslVendor[6]&0xff),
			  (adslVendor[7]&0xff));
		  *value = strdup(vendor);
		}
		else                         
		  *value = strdup("");
		BcmCfm_stsFree(BCMCFM_OBJ_IFC_ADSL, pObj);
		return TRX_OK;
	}
#endif
	BSTD_UNUSED(value);
	return TRX_ERR;
}


TRX_STATUS getWANDeviceWANDSLInterfaceConfig_ATURCountry(char **value)
{
#if 0
	void *obj;
	uint32 index = 0;
	char code[IFC_TINY_LEN];	//JJC:was 4
	char *adslCode;

	if ( BcmCfm_stsGet(BCMCFM_OBJ_IFC_ADSL, &obj, &index) == BcmCfm_Ok ) {
		PBcmCfm_AdslIfcSts_t pObj = (PBcmCfm_AdslIfcSts_t)obj;
		if ( pObj->linkState == BcmCfm_AdslLinkUp ) {
		  adslCode = pObj->adslPhysRcv.adslVendorID;
//		  sprintf(code,"%02x%02x", (adslCode[0]&0xff),(adslCode[1]&0xff));
		  sprintf(code, "%d", *(uint16 *)adslCode);		//JJC
		  *value = strdup(code);
		}
		else                         
		  *value = strdup("");
		BcmCfm_stsFree(BCMCFM_OBJ_IFC_ADSL, pObj);
		return TRX_OK;
	}
#endif
	BSTD_UNUSED(value);
	return TRX_ERR;
}
#if 0
TRX_STATUS getWANDeviceWANDSLInterfaceConfig_ATURANSIStd(char **value)
{
	//InstanceDesc *idp;
	//if( idp=getCurrentInstanceDesc()){
		/* Add code here to get param -- delete idp if not needed *****/
		 *value = strdup("");
		return TRX_OK;
	//}
	//return TRX_ERR;
}
TRX_STATUS getWANDeviceWANDSLInterfaceConfig_ATURANSIRev(char **value)
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
TRX_STATUS getWANDeviceWANDSLInterfaceConfig_ATUCVendor(char **value)
{
#if 0
	void *obj;
	uint32 index = 0;
	char vendor[8];
	char *adslVendor;

	if ( BcmCfm_stsGet(BCMCFM_OBJ_IFC_ADSL, &obj, &index) == BcmCfm_Ok ) {
		PBcmCfm_AdslIfcSts_t pObj = (PBcmCfm_AdslIfcSts_t)obj;
		if ( pObj->linkState == BcmCfm_AdslLinkUp ) {
		  adslVendor = pObj->adslPhysXmt.adslVendorID;
		  sprintf(vendor,"%c%c%c%c%02x%02x",
			  (adslVendor[2]),
			  (adslVendor[3]),(adslVendor[4]),(adslVendor[5]),
			  (adslVendor[6]&0xff),
			  (adslVendor[7]&0xff));
		  *value = strdup(vendor);
		}
		else                         
		 *value = strdup("");
		BcmCfm_stsFree(BCMCFM_OBJ_IFC_ADSL, pObj);
		return TRX_OK;
	}
#endif        
	BSTD_UNUSED(value);
	return TRX_ERR;
}

TRX_STATUS getWANDeviceWANDSLInterfaceConfig_ATUCCountry(char **value)
{
#if 0
	void *obj;
	uint32 index = 0;
	char code[IFC_TINY_LEN];	//JJC:was 4
	char *adslCode;

	if ( BcmCfm_stsGet(BCMCFM_OBJ_IFC_ADSL, &obj, &index) == BcmCfm_Ok ) {
		PBcmCfm_AdslIfcSts_t pObj = (PBcmCfm_AdslIfcSts_t)obj;
		if ( pObj->linkState == BcmCfm_AdslLinkUp ) {
		  adslCode = pObj->adslPhysXmt.adslVendorID;
//		  sprintf(code,"%02x%02x", (adslCode[0]&0xff),(adslCode[1]&0xff));
		  sprintf(code, "%d", *(uint16 *)adslCode);		//JJC
		  *value = strdup(code);
		}
		else                         
		  *value = strdup("");
		BcmCfm_stsFree(BCMCFM_OBJ_IFC_ADSL, pObj);
		return TRX_OK;
	}
#endif
	BSTD_UNUSED(value);
	return TRX_ERR;
}

#if 0
TRX_STATUS getWANDeviceWANDSLInterfaceConfig_ATUCANSIStd(char **value)
{
	//InstanceDesc *idp;
	//if( idp=getCurrentInstanceDesc()){
		/* Add code here to get param -- delete idp if not needed *****/
		 *value = strdup("");
		return TRX_OK;
	//}
	//return TRX_ERR;
}
TRX_STATUS getWANDeviceWANDSLInterfaceConfig_ATUCANSIRev(char **value)
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
TRX_STATUS getWANDeviceWANDSLInterfaceConfig_TotalStart(char **value)
{
#if 0
	void *obj;
	uint32 index = 0;
        
	if ( BcmCfm_stsGet(BCMCFM_OBJ_IFC_ADSL, &obj, &index) == BcmCfm_Ok ) {
		PBcmCfm_AdslIfcSts_t pObj = (PBcmCfm_AdslIfcSts_t)obj;
		if ( pObj->linkState == BcmCfm_AdslLinkUp ) {
			char buf[IFC_TINY_LEN];
			if ( pObj->adslRxPerfData.adslPerfCurr1DayTimeElapsed == 0 )                        
				sprintf(buf, "%lu", pObj->adslRxPerfData.adslPerfCurr15MinTimeElapsed);
			else                                
				sprintf(buf, "%lu", pObj->adslRxPerfData.adslPerfCurr1DayTimeElapsed +
					pObj->adslRxPerfData.adslAturPerfPrev1DayMoniSecs);
			 *value = strdup(buf);
		} else                         
			*value = strdup("0");
		BcmCfm_stsFree(BCMCFM_OBJ_IFC_ADSL, pObj);
		return TRX_OK;
	}
#endif        
	BSTD_UNUSED(value);
	return TRX_ERR;
}
#if 0
TRX_STATUS getWANDeviceWANDSLInterfaceConfig_ShowtimeStart(char **value)
{
	//InstanceDesc *idp;
	//if( idp=getCurrentInstanceDesc()){
		/* Add code here to get param -- delete idp if not needed *****/
		 *value = strdup("");
		return TRX_OK;
	//}
	//return TRX_ERR;
}
TRX_STATUS getWANDeviceWANDSLInterfaceConfig_LastShowtimeStart(char **value)
{
	//InstanceDesc *idp;
	//if( idp=getCurrentInstanceDesc()){
		/* Add code here to get param -- delete idp if not needed *****/
		 *value = strdup("");
		return TRX_OK;
	//}
	//return TRX_ERR;
}
TRX_STATUS getWANDeviceWANDSLInterfaceConfig_CurrentDayStart(char **value)
{
	//InstanceDesc *idp;
	//if( idp=getCurrentInstanceDesc()){
		/* Add code here to get param -- delete idp if not needed *****/
		 *value = strdup("");
		return TRX_OK;
	//}
	//return TRX_ERR;
}
TRX_STATUS getWANDeviceWANDSLInterfaceConfig_QuarterHourStart(char **value)
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
/** InternetGatewayDevice.WANDevice.WANEthernetInterfaceConfig.Stats. */

TRX_STATUS getWANDeviceWANEthernetInterfaceConfigStats_BytesSent(char **value)
{
	//InstanceDesc *idp;
	//if( idp=getCurrentInstanceDesc()){
		/* Add code here to get param -- delete idp if not needed *****/
		 *value = strdup("");
		return TRX_OK;
	//}
	//return TRX_ERR;
}
TRX_STATUS getWANDeviceWANEthernetInterfaceConfigStats_BytesReceived(char **value)
{
	//InstanceDesc *idp;
	//if( idp=getCurrentInstanceDesc()){
		/* Add code here to get param -- delete idp if not needed *****/
		 *value = strdup("");
		return TRX_OK;
	//}
	//return TRX_ERR;
}
TRX_STATUS getWANDeviceWANEthernetInterfaceConfigStats_PacketsSent(char **value)
{
	//InstanceDesc *idp;
	//if( idp=getCurrentInstanceDesc()){
		/* Add code here to get param -- delete idp if not needed *****/
		 *value = strdup("");
		return TRX_OK;
	//}
	//return TRX_ERR;
}
TRX_STATUS getWANDeviceWANEthernetInterfaceConfigStats_PacketsReceived(char **value)
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
/** InternetGatewayDevice.WANDevice.WANEthernetInterfaceConfig. */

TRX_STATUS setWANDeviceWANEthernetInterfaceConfig_Enable(char *value)
{
	//InstanceDesc *idp;
	//if( idp=getCurrentInstanceDesc()){
		/* Add code here to set param -- delete idp if not needed *****/
		return TRX_OK;
	//}
	//return TRX_ERR;
}
TRX_STATUS getWANDeviceWANEthernetInterfaceConfig_Enable(char **value)
{
	//InstanceDesc *idp;
	//if( idp=getCurrentInstanceDesc()){
		/* Add code here to get param -- delete idp if not needed *****/
		 *value = strdup("");
		return TRX_OK;
	//}
	//return TRX_ERR;
}
TRX_STATUS getWANDeviceWANEthernetInterfaceConfig_Status(char **value)
{
	//InstanceDesc *idp;
	//if( idp=getCurrentInstanceDesc()){
		/* Add code here to get param -- delete idp if not needed *****/
		 *value = strdup("");
		return TRX_OK;
	//}
	//return TRX_ERR;
}
TRX_STATUS getWANDeviceWANEthernetInterfaceConfig_MACAddress(char **value)
{
	//InstanceDesc *idp;
	//if( idp=getCurrentInstanceDesc()){
		/* Add code here to get param -- delete idp if not needed *****/
		 *value = strdup("");
		return TRX_OK;
	//}
	//return TRX_ERR;
}
TRX_STATUS setWANDeviceWANEthernetInterfaceConfig_MaxBitRate(char *value)
{
	//InstanceDesc *idp;
	//if( idp=getCurrentInstanceDesc()){
		/* Add code here to set param -- delete idp if not needed *****/
		return TRX_OK;
	//}
	//return TRX_ERR;
}
TRX_STATUS getWANDeviceWANEthernetInterfaceConfig_MaxBitRate(char **value)
{
	//InstanceDesc *idp;
	//if( idp=getCurrentInstanceDesc()){
		/* Add code here to get param -- delete idp if not needed *****/
		 *value = strdup("");
		return TRX_OK;
	//}
	//return TRX_ERR;
}
TRX_STATUS setWANDeviceWANEthernetInterfaceConfig_DuplexMode(char *value)
{
	//InstanceDesc *idp;
	//if( idp=getCurrentInstanceDesc()){
		/* Add code here to set param -- delete idp if not needed *****/
		return TRX_OK;
	//}
	//return TRX_ERR;
}
TRX_STATUS getWANDeviceWANEthernetInterfaceConfig_DuplexMode(char **value)
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
/** InternetGatewayDevice.WANDevice.WANDSLConnectionManagement.ConnectionService. */

TRX_STATUS getWANDeviceWANDSLConnectionManagementConnectionService_WANConnectionDevice(char **value)
{
#if 0
	InstanceDesc *idp;
	if( (idp=getCurrentInstanceDesc()) ){
		char wanDev[256];
		/* instance Id is WANConnecionDevice instance id*/
		snprintf(wanDev, sizeof(wanDev), "%s.%s.1.%s.%d", InternetGatewayDevice, WANDevice,
				 WANConnectionDevice,idp->instanceID);
		*value = strdup(wanDev);
		return TRX_OK;
	}
#endif
	BSTD_UNUSED(value);
	return TRX_ERR;
}

TRX_STATUS getWANDeviceWANDSLConnectionManagementConnectionService_WANConnectionService(char **value)
{
#if 0
	InstanceDesc *idp;
        
	if( (idp=getCurrentInstanceDesc()) ){
		uint32 objIndex = 0;
		void *objValue;                
		char wanDev[256];
		char *objName;
		while ( BcmCfm_objGet(BCMCFM_OBJ_NTWK_INTF, &objValue, &objIndex) == BcmCfm_Ok ) {
			PBcmCfm_NtwkIntfCfg_t pObj = (PBcmCfm_NtwkIntfCfg_t)objValue;
			if ( pObj != NULL && 
				pObj->attachToObjId == BCMCFM_OBJ_IFC_ATM_VCC &&
				pObj->attachToIndex == idp->instanceID ) {
				if ( pObj->protocol == BcmCfm_NtwkIntfProtoPPPoA ||
					pObj->protocol == BcmCfm_NtwkIntfProtoPPPoE )
					objName = WANPPPConnection;
				else
					objName = WANIPConnection;
				/* instance Id is WANConnecionDevice instance id*/
				snprintf(wanDev, sizeof(wanDev), "%s.%s.1.%s.%d.%s.%lu", InternetGatewayDevice, WANDevice,
				              WANConnectionDevice,idp->instanceID, objName, objIndex);
				*value = strdup(wanDev);
				BcmCfm_objFree(BCMCFM_OBJ_NTWK_INTF, objValue);
				return TRX_OK;
			}
			BcmCfm_objFree(BCMCFM_OBJ_NTWK_INTF, objValue);
			objIndex++;                        
		}
	}
#endif        
	BSTD_UNUSED(value);
	return TRX_ERR;
}

TRX_STATUS getWANDeviceWANDSLConnectionManagementConnectionService_DestinationAddress(char **value)
{
#if 0
	InstanceDesc *idp;
        
	if( (idp=getCurrentInstanceDesc()) ){
		char buf[32];
		snprintf(buf, sizeof(buf), "%d/%d", (int)idp->hwUserData&0x0000ffff, (int)idp->hwUserData>>16);
		*value = strdup(buf);
		return TRX_OK;
	}
#endif        
	BSTD_UNUSED(value);
	return TRX_ERR;
}

TRX_STATUS getDSLLinkType(char **);
TRX_STATUS getWANDeviceWANDSLConnectionManagementConnectionService_LinkType(char **value)
{
	return getDSLLinkType(value);
}
TRX_STATUS getWANDeviceWANDSLConnectionManagementConnectionService_ConnectionType(char **value)
{
#if 0
	InstanceDesc *idp;
        
	if( (idp=getCurrentInstanceDesc()) ){
		uint32 objIndex = 0;
		void *objValue;                
		while ( BcmCfm_objGet(BCMCFM_OBJ_NTWK_INTF, &objValue, &objIndex) == BcmCfm_Ok ) {
			PBcmCfm_NtwkIntfCfg_t pObj = (PBcmCfm_NtwkIntfCfg_t)objValue;
			if ( pObj != NULL && 
				pObj->attachToObjId == BCMCFM_OBJ_IFC_ATM_VCC &&
				pObj->attachToIndex == idp->instanceID ) {
				if ( pObj->protocol == BcmCfm_NtwkIntfProtoMAC )
					*value = strdup((pObj->proto.ip.addr == 0 &&
						pObj->proto.ip.dhcpClientEnable == BcmCfm_CfgDisabled)
						? "IP_Bridged" : "IP_Routed");
				else
					*value = strdup("IP_Routed");
				BcmCfm_objFree(BCMCFM_OBJ_NTWK_INTF, objValue);
				return TRX_OK;
			}
			BcmCfm_objFree(BCMCFM_OBJ_NTWK_INTF, objValue);
			objIndex++;                        
		}
	}
#endif        
	BSTD_UNUSED(value);
	return TRX_ERR;
}

/** */
/** InternetGatewayDevice.WANDevice.WANDSLConnectionManagement.ConnectionService. */


/** */
/** InternetGatewayDevice.WANDevice.WANDSLConnectionManagement. */

extern TRxObjNode WANDeviceWANDSLConnectionManagementConnectionServiceInstanceDesc[];
TRX_STATUS getWANDeviceWANDSLConnectionManagement_ConnectionServiceNumberOfEntries(char **value)
{
	int num = getInstanceCount(WANDeviceWANDSLConnectionManagementConnectionServiceInstanceDesc);
	*value = strdup( itoa(num));
		return TRX_OK;
}

/** */
/** InternetGatewayDevice.WANDevice.WANDSLDiagnostics. */
TRX_STATUS setWANDeviceWANDSLDiagnostics_LoopDiagnosticsState(char *value)
{
#if 0
	BcmCfm_DiagDsl data;
#if 0
	printf("setWANDeviceWANConnectionDeviceWANDslLoopbackDiagnostics_DiagnosticsState(entry)\n");
#endif

	if (strcmp(value,"Requested") != 0)
            return TRX_ERR;

	strcpy(data.state,value);
	BcmDiag_setDslDiagData(&data);
#endif
	BSTD_UNUSED(value);
	return TRX_OK;
}

TRX_STATUS getWANDeviceWANDSLDiagnostics_LoopDiagnosticsState(char **value)
{
#if 0
        char *resultStr;
#if 0
	printf("getWANDeviceWANConnectionDeviceWANDslLoopbackDiagnostics_DiagnosticsState(entry)\n");
#endif

	resultStr = malloc(DIAG_MAX_STATE_LENGTH);
	if (BcmDiag_getDslDiagStr("state",&resultStr) == TRX_OK) {
	  *value = strdup(resultStr);
         free(resultStr);
	  return TRX_OK;
	}
#endif
	BSTD_UNUSED(value);
	return TRX_ERR;
}

TRX_STATUS getWANDeviceWANDSLDiagnostics_ACTPSDds(char **value)
{
#if 0
	BcmCfm_DiagDsl data;
	char count[IFC_TINY_LEN];
#if 0
	printf("getWANDeviceWANDSLDiagnostics_ACTPSDds(entry)\n");
#endif
	
	if (BcmDiag_getDslDiagDataInt(&data) == TRX_OK) {
	  sprintf(count,"%d",data.ACTPSDds);
	  *value = strdup(count);
	  return TRX_OK;
	}
#endif
	BSTD_UNUSED(value);
	return TRX_ERR;
}
TRX_STATUS getWANDeviceWANDSLDiagnostics_ACTPSDus(char **value)
{
#if 0
	BcmCfm_DiagDsl data;
	char count[IFC_TINY_LEN];
	
	if (BcmDiag_getDslDiagDataInt(&data) == TRX_OK) {
	  sprintf(count,"%d",data.ACTPSDus);
	  *value = strdup(count);
	  return TRX_OK;
	}
#endif
	BSTD_UNUSED(value);
	return TRX_ERR;
}
TRX_STATUS getWANDeviceWANDSLDiagnostics_ACTATPds(char **value)
{
#if 0
	BcmCfm_DiagDsl data;
	char count[IFC_TINY_LEN];
	
	if (BcmDiag_getDslDiagDataInt(&data) == TRX_OK) {
	  sprintf(count,"%d",data.ACTATPds);
	  *value = strdup(count);
	  return TRX_OK;
	}
#endif
	BSTD_UNUSED(value);
	return TRX_ERR;
}
TRX_STATUS getWANDeviceWANDSLDiagnostics_ACTATPus(char **value)
{
#if 0
	BcmCfm_DiagDsl data;
	char count[IFC_TINY_LEN];
	
	if (BcmDiag_getDslDiagDataInt(&data) == TRX_OK) {
	  sprintf(count,"%d",data.ACTATPus);
	  *value = strdup(count);
	  return TRX_OK;
	}
#endif
	BSTD_UNUSED(value);
	return TRX_ERR;
}
TRX_STATUS getWANDeviceWANDSLDiagnostics_HLINSCds(char **value)
{
#if 0
	BcmCfm_DiagDsl data;
	char count[IFC_TINY_LEN];
	
	if (BcmDiag_getDslDiagDataInt(&data) == TRX_OK) {
	  sprintf(count,"%d",data.HLINSCds);
	  *value = strdup(count);
	  return TRX_OK;
	}
#endif
	BSTD_UNUSED(value);
	return TRX_ERR;
}
TRX_STATUS getWANDeviceWANDSLDiagnostics_HLINpsds(char **value)
{
#if 0
        char *result;
	int len;

        result = malloc(DIAG_MAX_DSL_DATA_LENGTH*2);
	if (result == NULL)
	  return TRX_ERR;

	if (BcmDiag_getDslDiagStr("HLINpsds",&result) == TRX_OK) {
	  len = strlen(result);
	  *value = (char*)strndup(result,len);
	  free(result);
	  return TRX_OK;
	}
#endif
	BSTD_UNUSED(value);
	return TRX_ERR;
}
TRX_STATUS getWANDeviceWANDSLDiagnostics_QLNpsds(char **value)
{
#if 0
        char *result;
	int len;

        result = malloc(DIAG_MAX_DSL_DATA_LENGTH*2);
	if (result == NULL)
	  return TRX_ERR;
	if (BcmDiag_getDslDiagStr("QLNpsds",&result) == TRX_OK) {
	  len = strlen(result);
	  *value = (char*)strndup(result,len);
	  free(result);
	  return TRX_OK;
	}
#endif
	BSTD_UNUSED(value);
	return TRX_ERR;
}
TRX_STATUS getWANDeviceWANDSLDiagnostics_SNRpsds(char **value)
{
#if 0
        char *result;
	int len;

        result = malloc(DIAG_MAX_DSL_DATA_LENGTH*2);
	if (result == NULL)
	  return TRX_ERR;
	if (BcmDiag_getDslDiagStr("SNRpsds",&result) == TRX_OK) {
	  len = strlen(result);
	  *value = (char*)strndup(result,len);
	  free(result);
	  return TRX_OK;
	}
#endif
	BSTD_UNUSED(value);
	return TRX_ERR;
}
TRX_STATUS getWANDeviceWANDSLDiagnostics_BITSpsds(char **value)
{
#if 0
        char *result;
	int len;

        result = malloc(DIAG_MAX_DSL_DATA_LENGTH*2);

	if (result == NULL)
	  return TRX_ERR;

	if (BcmDiag_getDslDiagStr("BITSpsds",&result) == TRX_OK) {
	  len = strlen(result);
	  *value = (char*)strndup(result,len);
	  free(result);
	  return TRX_OK;
	}
#endif
	BSTD_UNUSED(value);
	return TRX_ERR;
}
TRX_STATUS getWANDeviceWANDSLDiagnostics_GAINSpsds(char **value)
{
#if 0
        char *result;
	int len;

        result = malloc(DIAG_MAX_DSL_DATA_LENGTH*2);

	if (result == NULL)
	  return TRX_ERR;

	if (BcmDiag_getDslDiagStr("GAINSpsds",&result) == TRX_OK) {
	  len = strlen(result);
	  *value = (char*)strndup(result,len);
	  free(result);
	  return TRX_OK;
	}
#endif
	BSTD_UNUSED(value);
	return TRX_ERR;
}


/** */
/* If a WANIPConnection or WANPPPConnection object is connected to this
 * WANConnectionDevice object, then a reboot is needed.  Otherwise, no
 * reboot is needed.
 */
TRX_STATUS TRX_OK_or_TRX_REBOOT(int instanceID)
{
#if 0
	TRX_STATUS status = TRX_OK;
	uint32 index = 0;
	PBcmCfm_NtwkIntfCfg_t pObj;
	while(BcmCfm_objGet(BCMCFM_OBJ_NTWK_INTF,(void **)&pObj,&index)==BcmCfm_Ok) {
		if( pObj->attachToIndex == (uint32) instanceID ) {
			BcmCfm_objFree(BCMCFM_OBJ_NTWK_INTF, pObj);
			status = TRX_REBOOT;
			break;
		}

		BcmCfm_objFree(BCMCFM_OBJ_NTWK_INTF, pObj);
		index++;
	}
	return status;
#endif
	BSTD_UNUSED(instanceID);
	return TRX_ERR;
}

/** */
/** InternetGatewayDevice.WANDevice.WANConnectionDevice.WANDSLLinkConfig. */
/* InternetGatewayDevice.WANDevice.{i}.WANConnectionDevice.{i}.WANDSLLinkConfig. */

TRX_STATUS setDSLLinkEnable(char *value)
{
#if 0
	InstanceDesc *idp;

	if ( (idp = getCurrentInstanceDesc()) ) {
		PBcmCfm_AtmVccCfg_t pObj = NULL;
		uint32 index = idp->instanceID;
		if ( BcmCfm_objGet(BCMCFM_OBJ_IFC_ATM_VCC, 
						   &(idp->hwUserData), &index) == BcmCfm_Ok ) {
			pObj = (PBcmCfm_AtmVccCfg_t)idp->hwUserData;
			if ( pObj == NULL )	{
				BcmCfm_objFree(BCMCFM_OBJ_IFC_ATM_VCC, pObj);
				return TRX_ERR;
			}
			if ( strcmp(value, "1") == 0 )
				pObj->status = BcmCfm_CfgEnabled;
			else
				pObj->status = BcmCfm_CfgDisabled;
			if ( BcmCfm_objSet(BCMCFM_OBJ_IFC_ATM_VCC, 
							   idp->hwUserData, index) == BcmCfm_Ok )
				return TRX_OK_or_TRX_REBOOT(idp->instanceID);
		}
	}
#endif
	BSTD_UNUSED(value);
	return TRX_ERR;
}
TRX_STATUS getDSLLinkEnable(char **value)
{
#if 0
	InstanceDesc *idp;

	if ( (idp = getCurrentInstanceDesc()) ) {
		PBcmCfm_AtmVccCfg_t pObj = NULL;
		uint32 index = idp->instanceID;
		if ( BcmCfm_objGet(BCMCFM_OBJ_IFC_ATM_VCC, 
						   &(idp->hwUserData), &index) == BcmCfm_Ok ) {
			pObj = (PBcmCfm_AtmVccCfg_t)idp->hwUserData;
			if ( pObj == NULL )	{
				BcmCfm_objFree(BCMCFM_OBJ_IFC_ATM_VCC, pObj);
				return TRX_ERR;
			}
			if ( pObj->status == BcmCfm_CfgEnabled )
				*value = strdup("1");
			else
				*value = strdup("0");
			BcmCfm_objFree(BCMCFM_OBJ_IFC_ATM_VCC, idp->hwUserData);
			return TRX_OK;
		}
	}
#endif
	BSTD_UNUSED(value);
	return TRX_ERR;
}
TRX_STATUS setDSLLinkType(char *value)
{
#if 0
	InstanceDesc *idp;

	if ( (idp = getCurrentInstanceDesc()) ) {
		char token[16];

		sprintf(token, "LinkType%4.4x", idp->instanceID);
		if(BcmCfm_scratchPadSet(token, value, strlen(value) + 1) == BcmCfm_Ok)
			return TRX_OK_or_TRX_REBOOT(idp->instanceID);
	}
#endif
	BSTD_UNUSED(value);
	return TRX_ERR;
}
TRX_STATUS getDSLLinkType(char **value)
{
#if 0
	InstanceDesc *idp;

	if ( (idp = getCurrentInstanceDesc()) ) {
		char token[16];
		char *linktype;
		uint32 len;

		sprintf(token, "LinkType%4.4x", idp->instanceID);
		if(BcmCfm_scratchPadGet(token, (void **) &linktype, &len) == BcmCfm_Ok) {
			*value = strdup(linktype);
			BcmCfm_free(linktype);
		}
		else
			*value = strdup("EoA");

		return TRX_OK;
	}
#endif
	BSTD_UNUSED(value);
	return TRX_ERR;
}
TRX_STATUS setDSLLinkAddress(char *value)
{
#if 0
	char *pChar = NULL;
	char addr[IFC_LARGE_LEN];

	if ( value == NULL ) return TRX_ERR;
	strcpy(addr, value);

	// only support PVC type with format vpi/vci
	if ( (pChar = strchr(addr, '/')) == NULL ) return TRX_ERR;
	*pChar = '\0';

	InstanceDesc *idp;

	if ( (idp = getCurrentInstanceDesc()) ) {
		PBcmCfm_AtmVccCfg_t pObj = NULL;
		uint32 index = idp->instanceID;
		if ( BcmCfm_objGet(BCMCFM_OBJ_IFC_ATM_VCC, 
						   &(idp->hwUserData), &index) == BcmCfm_Ok ) {
			pObj = (PBcmCfm_AtmVccCfg_t)idp->hwUserData;
			if ( pObj == NULL )	{
				BcmCfm_objFree(BCMCFM_OBJ_IFC_ATM_VCC, pObj);
				return TRX_ERR;
			}
			pObj->vpi = strtoul(addr, (char **)NULL, 0);
			pObj->vci = strtoul(pChar+1, (char **)NULL, 0);
			if ( BcmCfm_objSet(BCMCFM_OBJ_IFC_ATM_VCC, 
							   idp->hwUserData, index) == BcmCfm_Ok )
				return TRX_OK_or_TRX_REBOOT(idp->instanceID);
		}
	}
#endif
	BSTD_UNUSED(value);
	return TRX_ERR;
}
TRX_STATUS getDSLLinkAddress(char **value)
{
#if 0
	InstanceDesc *idp;

	if ( (idp = getCurrentInstanceDesc()) ) {
		char  addr[IFC_TINY_LEN];
		PBcmCfm_AtmVccCfg_t pObj = NULL;
		uint32 index = idp->instanceID;
		if ( BcmCfm_objGet(BCMCFM_OBJ_IFC_ATM_VCC, 
						   &(idp->hwUserData), &index) == BcmCfm_Ok ) {
			pObj = (PBcmCfm_AtmVccCfg_t)idp->hwUserData;
			if ( pObj == NULL )	{
				BcmCfm_objFree(BCMCFM_OBJ_IFC_ATM_VCC, pObj);
				return TRX_ERR;
			}
			sprintf(addr, "%lu/%lu", pObj->vpi, pObj->vci);
			*value = strdup(addr);
			BcmCfm_objFree(BCMCFM_OBJ_IFC_ATM_VCC, idp->hwUserData);
			return TRX_OK;
		}
	}
#endif
	BSTD_UNUSED(value);
	return TRX_ERR;
}
#if 0
TRX_STATUS getWANDeviceWANConnectionDeviceWANDSLLinkConfig_LinkStatus(char **value)
{
	//InstanceDesc *idp;
	//if( idp=getCurrentInstanceDesc()){
		/* Add code here to get param -- delete idp if not needed *****/
		 *value = strdup("");
		return TRX_OK;
	//}
	//return TRX_ERR;
}
TRX_STATUS getWANDeviceWANConnectionDeviceWANDSLLinkConfig_AutoConfig(char **value)
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

TRX_STATUS getWANDeviceWANConnectionDeviceWANDSLLinkConfig_ModulationType(char **value)
{
        return (getWANDeviceWANDSLInterfaceConfig_ModulationType(value));
}

TRX_STATUS setATMEncap(char *value)
{
#if 0
	InstanceDesc *idp;

	if ( (idp = getCurrentInstanceDesc()) ) {
		PBcmCfm_AtmVccCfg_t pObj = NULL;
		uint32 index = idp->instanceID;
		if ( BcmCfm_objGet(BCMCFM_OBJ_IFC_ATM_VCC, 
						   &(idp->hwUserData), &index) == BcmCfm_Ok ) {
			pObj = (PBcmCfm_AtmVccCfg_t)idp->hwUserData;
			if ( pObj == NULL )	{
				BcmCfm_objFree(BCMCFM_OBJ_IFC_ATM_VCC, pObj);
				return TRX_ERR;
			}
			if ( strcmp(value, "LLC") == 0 )
				pObj->encapMode = BcmCfm_ATMEncapLlc;
			else
				pObj->encapMode	= BcmCfm_ATMEncapVcmuxRoute;
			if ( BcmCfm_objSet(BCMCFM_OBJ_IFC_ATM_VCC, 
							   idp->hwUserData, index) == BcmCfm_Ok )
				return TRX_OK_or_TRX_REBOOT(idp->instanceID);
		}
	}
#endif
	BSTD_UNUSED(value);
	return TRX_ERR;
}
TRX_STATUS getATMEncap(char **value)
{
#if 0
	InstanceDesc *idp;

	if ( (idp = getCurrentInstanceDesc()) ) {
		PBcmCfm_AtmVccCfg_t pObj = NULL;
		uint32 index = idp->instanceID;
		if ( BcmCfm_objGet(BCMCFM_OBJ_IFC_ATM_VCC, 
						   &(idp->hwUserData), &index) == BcmCfm_Ok ) {
			pObj = (PBcmCfm_AtmVccCfg_t)idp->hwUserData;
			if ( pObj == NULL )	{
				BcmCfm_objFree(BCMCFM_OBJ_IFC_ATM_VCC, pObj);
				return TRX_ERR;
			}
			if ( pObj->encapMode == BcmCfm_ATMEncapLlc )
				*value = strdup("LLC");
			else
				*value = strdup("VCMUX");
			BcmCfm_objFree(BCMCFM_OBJ_IFC_ATM_VCC, idp->hwUserData);
			return TRX_OK;
		}
	}
#endif
	BSTD_UNUSED(value);
	return TRX_ERR;
}
TRX_STATUS setATMQoS(char *value)
{
#if 0
	InstanceDesc *idp;

	if ( (idp = getCurrentInstanceDesc()) ) {
		PBcmCfm_AtmVccCfg_t pObj = NULL;
		uint32 index = idp->instanceID;
		if ( BcmCfm_objGet(BCMCFM_OBJ_IFC_ATM_VCC, 
						   &(idp->hwUserData), &index) == BcmCfm_Ok ) {
			pObj = (PBcmCfm_AtmVccCfg_t)idp->hwUserData;
			if ( pObj == NULL )	{
				BcmCfm_objFree(BCMCFM_OBJ_IFC_ATM_VCC, pObj);
				return TRX_ERR;
			}
			if ( strcmp(value, "UBR+") == 0 )
				pObj->atmCategory = BcmCfm_ATMCatUBRwithPCR;
			else if ( strcmp(value, "CBR") == 0 )
				pObj->atmCategory = BcmCfm_ATMCatCBR;
			else if ( strcmp(value, "VBR-rt") == 0 )
				pObj->atmCategory = BcmCfm_ATMCatVBR;
			else if ( strcmp(value, "VBR-nrt") == 0 )
				pObj->atmCategory = BcmCfm_ATMCatVBRnrt;
			else
				pObj->atmCategory = BcmCfm_ATMCatUBR;
			if ( BcmCfm_objSet(BCMCFM_OBJ_IFC_ATM_VCC, 
							   idp->hwUserData, index) == BcmCfm_Ok )
				return TRX_OK_or_TRX_REBOOT(idp->instanceID);
		}
	}
#endif
	BSTD_UNUSED(value);
	return TRX_ERR;
}
TRX_STATUS getATMQoS(char **value)
{
#if 0
	InstanceDesc *idp;

	if ( (idp = getCurrentInstanceDesc()) ) {
		PBcmCfm_AtmVccCfg_t pObj = NULL;
		uint32 index = idp->instanceID;
		if ( BcmCfm_objGet(BCMCFM_OBJ_IFC_ATM_VCC, 
						   &(idp->hwUserData), &index) == BcmCfm_Ok ) {
			pObj = (PBcmCfm_AtmVccCfg_t)idp->hwUserData;
			if ( pObj == NULL )	{
				BcmCfm_objFree(BCMCFM_OBJ_IFC_ATM_VCC, pObj);
				return TRX_ERR;
			}
			switch ( pObj->atmCategory ) {
			case BcmCfm_ATMCatUBRwithPCR:
				*value = strdup("UBR+");
				break;
			case BcmCfm_ATMCatCBR:
				*value = strdup("CBR");
				break;
			case BcmCfm_ATMCatVBR:
				*value = strdup("VBR-rt");
				break;
			case BcmCfm_ATMCatVBRnrt:
				*value = strdup("VBR-nrt");
				break;
			default:
				*value = strdup("UBR");
				break;
			}
			BcmCfm_objFree(BCMCFM_OBJ_IFC_ATM_VCC, idp->hwUserData);
			return TRX_OK;
		}
	}
#endif
	BSTD_UNUSED(value);
	return TRX_ERR;
}
TRX_STATUS setATMPeakCellRate(char *value)
{
#if 0
	InstanceDesc *idp;

	if ( (idp = getCurrentInstanceDesc()) ) {
		PBcmCfm_AtmVccCfg_t pObj = NULL;
		uint32 index = idp->instanceID;
		if ( BcmCfm_objGet(BCMCFM_OBJ_IFC_ATM_VCC, 
						   &(idp->hwUserData), &index) == BcmCfm_Ok ) {
			pObj = (PBcmCfm_AtmVccCfg_t)idp->hwUserData;
			if ( pObj == NULL )	{
				BcmCfm_objFree(BCMCFM_OBJ_IFC_ATM_VCC, pObj);
				return TRX_ERR;
			}
			pObj->pcr = strtoul(value, (char **)NULL, 0);
			if ( BcmCfm_objSet(BCMCFM_OBJ_IFC_ATM_VCC, 
							   idp->hwUserData, index) == BcmCfm_Ok )
				return TRX_OK;
		}
	}
#endif
	BSTD_UNUSED(value);
	return TRX_ERR;
}
TRX_STATUS getATMPeakCellRate(char **value)
{
#if 0
	InstanceDesc *idp;

	if ( (idp = getCurrentInstanceDesc()) ) {
		char  rate[IFC_TINY_LEN];
		PBcmCfm_AtmVccCfg_t pObj = NULL;
		uint32 index = idp->instanceID;
		if ( BcmCfm_objGet(BCMCFM_OBJ_IFC_ATM_VCC, 
						   &(idp->hwUserData), &index) == BcmCfm_Ok ) {
			pObj = (PBcmCfm_AtmVccCfg_t)idp->hwUserData;
			if ( pObj == NULL )	{
				BcmCfm_objFree(BCMCFM_OBJ_IFC_ATM_VCC, pObj);
				return TRX_ERR;
			}
			sprintf(rate, "%lu", pObj->pcr);
			*value = strdup(rate);
			BcmCfm_objFree(BCMCFM_OBJ_IFC_ATM_VCC, idp->hwUserData);
			return TRX_OK;
		}
	}
#endif
	BSTD_UNUSED(value);
	return TRX_ERR;
}
TRX_STATUS setATMMaximumBurstSize(char *value)
{
#if 0
	InstanceDesc *idp;

	if ( (idp = getCurrentInstanceDesc()) ) {
		PBcmCfm_AtmVccCfg_t pObj = NULL;
		uint32 index = idp->instanceID;
		if ( BcmCfm_objGet(BCMCFM_OBJ_IFC_ATM_VCC, 
						   &(idp->hwUserData), &index) == BcmCfm_Ok ) {
			pObj = (PBcmCfm_AtmVccCfg_t)idp->hwUserData;
			if ( pObj == NULL )	{
				BcmCfm_objFree(BCMCFM_OBJ_IFC_ATM_VCC, pObj);
				return TRX_ERR;
			}
			pObj->mbs = strtoul(value, (char **)NULL, 0);
			if ( BcmCfm_objSet(BCMCFM_OBJ_IFC_ATM_VCC, 
							   idp->hwUserData, index) == BcmCfm_Ok )
				return TRX_OK;
		}
	}
#endif
	BSTD_UNUSED(value);
	return TRX_ERR;
}
TRX_STATUS getATMMaximumBurstSize(char **value)
{
#if 0
	InstanceDesc *idp;

	if ( (idp = getCurrentInstanceDesc()) ) {
		char  size[IFC_TINY_LEN];
		PBcmCfm_AtmVccCfg_t pObj = NULL;
		uint32 index = idp->instanceID;
		if ( BcmCfm_objGet(BCMCFM_OBJ_IFC_ATM_VCC, 
						   &(idp->hwUserData), &index) == BcmCfm_Ok ) {
			pObj = (PBcmCfm_AtmVccCfg_t)idp->hwUserData;
			if ( pObj == NULL )	{
				BcmCfm_objFree(BCMCFM_OBJ_IFC_ATM_VCC, pObj);
				return TRX_ERR;
			}
			sprintf(size, "%lu", pObj->mbs);
			*value = strdup(size);
			BcmCfm_objFree(BCMCFM_OBJ_IFC_ATM_VCC, idp->hwUserData);
			return TRX_OK;
		}
	}
#endif
	BSTD_UNUSED(value);
	return TRX_ERR;
}
TRX_STATUS setATMSustainableCellRate(char *value)
{
#if 0
	InstanceDesc *idp;

	if ( (idp = getCurrentInstanceDesc()) ) {
		PBcmCfm_AtmVccCfg_t pObj = NULL;
		uint32 index = idp->instanceID;
		if ( BcmCfm_objGet(BCMCFM_OBJ_IFC_ATM_VCC, 
						   &(idp->hwUserData), &index) == BcmCfm_Ok ) {
			pObj = (PBcmCfm_AtmVccCfg_t)idp->hwUserData;
			if ( pObj == NULL )	{
				BcmCfm_objFree(BCMCFM_OBJ_IFC_ATM_VCC, pObj);
				return TRX_ERR;
			}
			pObj->scr = strtoul(value, (char **)NULL, 0);
			if ( BcmCfm_objSet(BCMCFM_OBJ_IFC_ATM_VCC, 
							   idp->hwUserData, index) == BcmCfm_Ok )
				return TRX_OK;
		}
	}
#endif
	BSTD_UNUSED(value);
	return TRX_ERR;
}
TRX_STATUS getATMSustainableCellRate(char **value)
{
#if 0
	InstanceDesc *idp;

	if ( (idp = getCurrentInstanceDesc()) ) {
		char  rate[IFC_TINY_LEN];
		PBcmCfm_AtmVccCfg_t pObj = NULL;
		uint32 index = idp->instanceID;
		if ( BcmCfm_objGet(BCMCFM_OBJ_IFC_ATM_VCC, 
						   &(idp->hwUserData), &index) == BcmCfm_Ok ) {
			pObj = (PBcmCfm_AtmVccCfg_t)idp->hwUserData;
			if ( pObj == NULL )	{
				BcmCfm_objFree(BCMCFM_OBJ_IFC_ATM_VCC, pObj);
				return TRX_ERR;
			}
			sprintf(rate, "%lu", pObj->scr);
			*value = strdup(rate);
			BcmCfm_objFree(BCMCFM_OBJ_IFC_ATM_VCC, idp->hwUserData);
			return TRX_OK;
		}
	}
#endif
	BSTD_UNUSED(value);
	return TRX_ERR;
}

#if 0
TRX_STATUS setWANDeviceWANConnectionDeviceWANDSLLinkConfig_FCSPreserved(char *value)
{
	//InstanceDesc *idp;
	//if( idp=getCurrentInstanceDesc()){
		/* Add code here to set param -- delete idp if not needed *****/
		return TRX_OK;
	//}
	//return TRX_ERR;
}
TRX_STATUS getWANDeviceWANConnectionDeviceWANDSLLinkConfig_FCSPreserved(char **value)
{
	//InstanceDesc *idp;
	//if( idp=getCurrentInstanceDesc()){
		/* Add code here to get param -- delete idp if not needed *****/
		 *value = strdup("");
		return TRX_OK;
	//}
	//return TRX_ERR;
}
TRX_STATUS setWANDeviceWANConnectionDeviceWANDSLLinkConfig_VCSearchList(char *value)
{
	//InstanceDesc *idp;
	//if( idp=getCurrentInstanceDesc()){
		/* Add code here to set param -- delete idp if not needed *****/
		return TRX_OK;
	//}
	//return TRX_ERR;
}
TRX_STATUS getWANDeviceWANConnectionDeviceWANDSLLinkConfig_VCSearchList(char **value)
{
	//InstanceDesc *idp;
	//if( idp=getCurrentInstanceDesc()){
		/* Add code here to get param -- delete idp if not needed *****/
		 *value = strdup("");
		return TRX_OK;
	//}
	//return TRX_ERR;
}
TRX_STATUS getWANDeviceWANConnectionDeviceWANDSLLinkConfig_ATMAAL(char **value)
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

TRX_STATUS getAtmIntfStats(char **value, int statField)
{
#if 0
	InstanceDesc *idp;
	void *obj;
	uint32 count;
	if ( (idp = getCurrentInstanceDesc()) ) {
		uint32 index = idp->instanceID;
		char buf[64];
		if ( BcmCfm_stsGet(BCMCFM_OBJ_IFC_ATM, &obj, &index) == BcmCfm_Ok ) {
			PBcmCfm_AtmIfcSts_t pObj = (PBcmCfm_AtmIfcSts_t)obj;
			if ( pObj != NULL ) {
			  switch (statField)
			    {
			    case ATM_TOTAL_RX_CELLS:
			      count = pObj->AtmIntfStats.ulIfInOctets / 48;
			      break;
			    case ATM_TOTAL_TX_CELLS:
			      count = pObj->AtmIntfStats.ulIfOutOctets/48;
			      break;
			    case ATM_HEC:
			      count = pObj->AtmIntfStats.ulIfInHecErrors;
			      break;
			    default:
			      count = 0;
			    }
			  snprintf(buf, sizeof(buf), "%lu", count);
			  *value = strdup (buf);
			  BcmCfm_stsFree(BCMCFM_OBJ_IFC_ATM, obj);
			  return TRX_OK;
			}
		}
	}
#endif
	BSTD_UNUSED(value);
	BSTD_UNUSED(statField);
	return TRX_ERR;  
}

TRX_STATUS getAtmVccStats(char **value, int statField)
{
#if 0
  InstanceDesc *idp;
  void *obj;
  uint32 count;
  PBcmCfm_AtmVccSts_t pObj = NULL;
  if ( (idp = getCurrentInstanceDesc()) ) {
    uint32 index = idp->instanceID;
    char buf[64];

    if ( BcmCfm_stsGet(BCMCFM_OBJ_IFC_ATM_VCC, 
		       &obj, &index) == BcmCfm_Ok ) {
      pObj = (PBcmCfm_AtmVccSts_t)obj;
      if ( pObj != NULL ) {
	switch (statField)
	  {
	  case ATM_AAL5_CRC:
	    count = pObj->vccCrcErrors;
	    break;
	  default:
	    count = 0;
	  }
	snprintf(buf, sizeof(buf), "%lu", count);
	*value = strdup (buf);
	BcmCfm_stsFree(BCMCFM_OBJ_IFC_ATM_VCC, obj);
	return TRX_OK;
      }
    }
  }
#endif
	BSTD_UNUSED(value);
	BSTD_UNUSED(statField);
  return TRX_ERR;  
}

TRX_STATUS getWANDeviceWANConnectionDeviceWANDSLLinkConfig_ATMTransmittedBlocks(char **value)
{
#if 0
        /* transmit blocks are actually transmitted cells */
        return (getAtmIntfStats(value,ATM_TOTAL_TX_CELLS));
#endif
	BSTD_UNUSED(value);
  return TRX_ERR;  
}
TRX_STATUS getWANDeviceWANConnectionDeviceWANDSLLinkConfig_ATMReceivedBlocks(char **value)
{
#if 0
        /* received blocks are actually received cells */
        return (getAtmIntfStats(value,ATM_TOTAL_RX_CELLS));
#endif
	BSTD_UNUSED(value);
  return TRX_ERR;  
}
TRX_STATUS getWANDeviceWANConnectionDeviceWANDSLLinkConfig_AAL5CRCErrors(char **value)
{
#if 0
        return (getAtmVccStats(value,ATM_AAL5_CRC));
#endif
	BSTD_UNUSED(value);
  return TRX_ERR;  
}

#if 0
TRX_STATUS getWANDeviceWANConnectionDeviceWANDSLLinkConfig_ATMCRCErrors(char **value)
{
	//InstanceDesc *idp;
	//if( idp=getCurrentInstanceDesc()){
		/* Add code here to get param -- delete idp if not needed *****/
		 *value = strdup("");
		return TRX_OK;
	//}
	//return TRX_ERR;
}
#endif /* 0 */

TRX_STATUS getWANDeviceWANConnectionDeviceWANDSLLinkConfig_ATMHECErrors(char **value)
{
#if 0
        return (getAtmIntfStats(value,ATM_HEC));
#endif
	BSTD_UNUSED(value);
  return TRX_ERR;  
}

/** */
/** InternetGatewayDevice.WANDevice.WANConnectionDevice.WANATMF5LoopbackDiagnostics. */

/*TRX_STATUS getWanAtmF5LoopbackData(PBcmCfm_DiagAtm pData)*/
TRX_STATUS getWanAtmF5LoopbackData(void * pData)
{
#if 0

  uint32 index;
  InstanceDesc *idp;
  PBcmCfm_AtmVccCfg_t pObj = NULL;

  if ( (idp = getCurrentInstanceDesc()) ) {
    index = idp->instanceID;
    if ( BcmCfm_objGet(BCMCFM_OBJ_IFC_ATM_VCC, 
		       &(idp->hwUserData), &index) == BcmCfm_Ok ) {
      pObj = (PBcmCfm_AtmVccCfg_t)idp->hwUserData;
      if ( pObj == NULL )	{
	BcmCfm_objFree(BCMCFM_OBJ_IFC_ATM_VCC, pObj);
	return TRX_ERR;
      }
    }
  }
  BcmDiag_getAtmDiagData(pObj->vpi,pObj->vci,pData);
  pData->vpi = pObj->vpi;
  pData->vci = pObj->vci;
  BcmCfm_objFree(BCMCFM_OBJ_IFC_ATM_VCC, pObj);
#endif
	BSTD_UNUSED(pData);
  return TRX_OK;
}

/*void resetWanAtmF5LoopbackData(PBcmCfm_DiagAtm pData)*/
void resetWanAtmF5LoopbackData(void* pData)
{
#if 0
  if (strcmp(pData->state,"Complete") == 0) {
    strcpy(pData->state,"None");
    pData->repetition = 0;
    pData->timeout = 0;
  }
#endif
	BSTD_UNUSED(pData);
}

TRX_STATUS setWANDeviceWANConnectionDeviceWANATMF5LoopbackDiagnostics_DiagnosticsState(char *value)
{
#if 0
	BcmCfm_DiagAtm data;
#if 0
	printf("setWANDeviceWANConnectionDeviceWANATMF5LoopbackDiagnostics_DiagnosticsState(entry)\n");
#endif

	if (strcmp(value,"Requested") != 0)
            return TRX_ERR;
	if (getWanAtmF5LoopbackData(&data) == TRX_OK) {
	  resetWanAtmF5LoopbackData(&data);
	  strcpy(data.state,value);
	  BcmDiag_setAtmDiagData(data.vpi,data.vci,&data);
	  return TRX_OK;
	}
#endif
	BSTD_UNUSED(value);
	return TRX_ERR;
}

TRX_STATUS getWANDeviceWANConnectionDeviceWANATMF5LoopbackDiagnostics_DiagnosticsState(char **value)
{
#if 0
	BcmCfm_DiagAtm data;
#if 0
	printf("getWANDeviceWANConnectionDeviceWANATMF5LoopbackDiagnostics_DiagnosticsState(entry)\n");
#endif

	if (getWanAtmF5LoopbackData(&data) == TRX_OK) {
	  *value = strdup(data.state);
	  return TRX_OK;
	}
#endif
	BSTD_UNUSED(value);
	return TRX_ERR;
}

TRX_STATUS setWANDeviceWANConnectionDeviceWANATMF5LoopbackDiagnostics_NumberOfRepetitions(char *value)
{
#if 0
	BcmCfm_DiagAtm data;

#if 0
	printf("setWANDeviceWANConnectionDeviceWANATMF5LoopbackDiagnostics_NumberOfRepetitions(entry)\n");
#endif

	if (getWanAtmF5LoopbackData(&data) == TRX_OK) {
	  resetWanAtmF5LoopbackData(&data);
	  data.repetition = strtoul(value, (char **)NULL, 0);
	  BcmDiag_setAtmDiagData(data.vpi,data.vci,&data);
	  return TRX_OK;
	}
#endif
	BSTD_UNUSED(value);
	return TRX_ERR;
}

TRX_STATUS getWANDeviceWANConnectionDeviceWANATMF5LoopbackDiagnostics_NumberOfRepetitions(char **value)
{
#if 0
	BcmCfm_DiagAtm data;
	char repetition[IFC_TINY_LEN];
#if 0
	printf("getWANDeviceWANConnectionDeviceWANATMF5LoopbackDiagnostics_NumberOfRepetitions(entry)\n");
#endif
	
	if (getWanAtmF5LoopbackData(&data) == TRX_OK) {
	  sprintf(repetition,"%lu",data.repetition);
	  *value = strdup(repetition);
	  return TRX_OK;
	}
#endif
	BSTD_UNUSED(value);
	return TRX_ERR;
}

TRX_STATUS setWANDeviceWANConnectionDeviceWANATMF5LoopbackDiagnostics_Timeout(char *value)
{
#if 0
	BcmCfm_DiagAtm data;

#if 0
	printf("setWANDeviceWANConnectionDeviceWANATMF5LoopbackDiagnostics_Timeout(entry)\n");
#endif

	if (getWanAtmF5LoopbackData(&data) == TRX_OK) {
	  resetWanAtmF5LoopbackData(&data);
	  data.timeout = strtoul(value, (char **)NULL, 0);
	  BcmDiag_setAtmDiagData(data.vpi,data.vci,&data);
	  return TRX_OK;
	}
#endif
	BSTD_UNUSED(value);
	return TRX_ERR;
}

TRX_STATUS getWANDeviceWANConnectionDeviceWANATMF5LoopbackDiagnostics_Timeout(char **value)
{
#if 0
	BcmCfm_DiagAtm data;
	char timeout[IFC_TINY_LEN];
#if 0
	printf("getWANDeviceWANConnectionDeviceWANATMF5LoopbackDiagnostics_Timeout(entry)\n");
#endif

	if (getWanAtmF5LoopbackData(&data) == TRX_OK) {
	  sprintf(timeout,"%lu",data.timeout);
	  *value = strdup(timeout);
	  return TRX_OK;
	}
#endif
	BSTD_UNUSED(value);
	return TRX_ERR;
}
TRX_STATUS getWANDeviceWANConnectionDeviceWANATMF5LoopbackDiagnostics_SuccessCount(char **value)
{
#if 0
	BcmCfm_DiagAtm data;
	char  count[IFC_TINY_LEN];

#if 0
	printf("getWANDeviceWANConnectionDeviceWANATMF5LoopbackDiagnostics_SucessCount(entry)\n");
#endif

	if (getWanAtmF5LoopbackData(&data) == TRX_OK) {
	  sprintf(count,"%lu",data.successCount);
	  *value = strdup(count);
	  return TRX_OK;
	}
#endif
	BSTD_UNUSED(value);
	return TRX_ERR;

}
TRX_STATUS getWANDeviceWANConnectionDeviceWANATMF5LoopbackDiagnostics_FailureCount(char **value)
{
#if 0
	char  count[IFC_TINY_LEN];
	BcmCfm_DiagAtm data;

#if 0
	printf("getWANDeviceWANConnectionDeviceWANATMF5LoopbackDiagnostics_FailureCount(entry)\n");
#endif

	if (getWanAtmF5LoopbackData(&data) == TRX_OK) {
	  sprintf(count,"%lu",data.failCount);
	  *value = strdup(count);
	  return TRX_OK;
	}
#endif
	BSTD_UNUSED(value);
	return TRX_ERR;
}
TRX_STATUS getWANDeviceWANConnectionDeviceWANATMF5LoopbackDiagnostics_AverageResponseTime(char **value)
{
#if 0
	char  count[IFC_TINY_LEN];
	BcmCfm_DiagAtm data;
#if 0
	printf("getWANDeviceWANConnectionDeviceWANATMF5LoopbackDiagnostics_AverageResponseTime(entry)\n");
#endif

	if (getWanAtmF5LoopbackData(&data) == TRX_OK) {
	  sprintf(count,"%lu",data.avgResponseTime);
	  *value = strdup(count);
	  return TRX_OK;
	}
#endif
	BSTD_UNUSED(value);
	return TRX_ERR;
}
TRX_STATUS getWANDeviceWANConnectionDeviceWANATMF5LoopbackDiagnostics_MinimumResponseTime(char **value)
{
#if 0
	char  count[IFC_TINY_LEN];
	BcmCfm_DiagAtm data;

#if 0
	printf("getWANDeviceWANConnectionDeviceWANATMF5LoopbackDiagnostics_MinimumResponseTime(entry)\n");
#endif

	if (getWanAtmF5LoopbackData(&data) == TRX_OK) {
	  sprintf(count,"%lu",data.minResponseTime);
	  *value = strdup(count);
	  return TRX_OK;
	}
#endif
	BSTD_UNUSED(value);
	return TRX_ERR;
}
TRX_STATUS getWANDeviceWANConnectionDeviceWANATMF5LoopbackDiagnostics_MaximumResponseTime(char **value)
{
#if 0
	char  count[IFC_TINY_LEN];
	BcmCfm_DiagAtm data;

#if 0
	printf("getWANDeviceWANConnectionDeviceWANATMF5LoopbackDiagnostics_maxResponseTime(entry)\n");
#endif

	if (getWanAtmF5LoopbackData(&data) == TRX_OK) {
	  sprintf(count,"%lu",data.maxResponseTime);
	  *value = strdup(count);
	  return TRX_OK;
	}
#endif
	BSTD_UNUSED(value);
	return TRX_ERR;
}

#if 0
/** */
/** InternetGatewayDevice.WANDevice.WANConnectionDevice.WANEthernetLinkConfig. */

TRX_STATUS getWANDeviceWANConnectionDeviceWANEthernetLinkConfig_EthernetLinkStatus(char **value)
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
/** InternetGatewayDevice.WANDevice.WANConnectionDevice.WANPOTSLinkConfig. */

TRX_STATUS setWANDeviceWANConnectionDeviceWANPOTSLinkConfig_Enable(char *value)
{
	//InstanceDesc *idp;
	//if( idp=getCurrentInstanceDesc()){
		/* Add code here to set param -- delete idp if not needed *****/
		return TRX_OK;
	//}
	//return TRX_ERR;
}
TRX_STATUS getWANDeviceWANConnectionDeviceWANPOTSLinkConfig_Enable(char **value)
{
	//InstanceDesc *idp;
	//if( idp=getCurrentInstanceDesc()){
		/* Add code here to get param -- delete idp if not needed *****/
		 *value = strdup("");
		return TRX_OK;
	//}
	//return TRX_ERR;
}
TRX_STATUS getWANDeviceWANConnectionDeviceWANPOTSLinkConfig_LinkStatus(char **value)
{
	//InstanceDesc *idp;
	//if( idp=getCurrentInstanceDesc()){
		/* Add code here to get param -- delete idp if not needed *****/
		 *value = strdup("");
		return TRX_OK;
	//}
	//return TRX_ERR;
}
TRX_STATUS setWANDeviceWANConnectionDeviceWANPOTSLinkConfig_ISPPhoneNumber(char *value)
{
	//InstanceDesc *idp;
	//if( idp=getCurrentInstanceDesc()){
		/* Add code here to set param -- delete idp if not needed *****/
		return TRX_OK;
	//}
	//return TRX_ERR;
}
TRX_STATUS getWANDeviceWANConnectionDeviceWANPOTSLinkConfig_ISPPhoneNumber(char **value)
{
	//InstanceDesc *idp;
	//if( idp=getCurrentInstanceDesc()){
		/* Add code here to get param -- delete idp if not needed *****/
		 *value = strdup("");
		return TRX_OK;
	//}
	//return TRX_ERR;
}
TRX_STATUS setWANDeviceWANConnectionDeviceWANPOTSLinkConfig_ISPInfo(char *value)
{
	//InstanceDesc *idp;
	//if( idp=getCurrentInstanceDesc()){
		/* Add code here to set param -- delete idp if not needed *****/
		return TRX_OK;
	//}
	//return TRX_ERR;
}
TRX_STATUS getWANDeviceWANConnectionDeviceWANPOTSLinkConfig_ISPInfo(char **value)
{
	//InstanceDesc *idp;
	//if( idp=getCurrentInstanceDesc()){
		/* Add code here to get param -- delete idp if not needed *****/
		 *value = strdup("");
		return TRX_OK;
	//}
	//return TRX_ERR;
}
TRX_STATUS setWANDeviceWANConnectionDeviceWANPOTSLinkConfig_LinkType(char *value)
{
	//InstanceDesc *idp;
	//if( idp=getCurrentInstanceDesc()){
		/* Add code here to set param -- delete idp if not needed *****/
		return TRX_OK;
	//}
	//return TRX_ERR;
}
TRX_STATUS getWANDeviceWANConnectionDeviceWANPOTSLinkConfig_LinkType(char **value)
{
	//InstanceDesc *idp;
	//if( idp=getCurrentInstanceDesc()){
		/* Add code here to get param -- delete idp if not needed *****/
		 *value = strdup("");
		return TRX_OK;
	//}
	//return TRX_ERR;
}
TRX_STATUS setWANDeviceWANConnectionDeviceWANPOTSLinkConfig_NumberOfRetries(char *value)
{
	//InstanceDesc *idp;
	//if( idp=getCurrentInstanceDesc()){
		/* Add code here to set param -- delete idp if not needed *****/
		return TRX_OK;
	//}
	//return TRX_ERR;
}
TRX_STATUS getWANDeviceWANConnectionDeviceWANPOTSLinkConfig_NumberOfRetries(char **value)
{
	//InstanceDesc *idp;
	//if( idp=getCurrentInstanceDesc()){
		/* Add code here to get param -- delete idp if not needed *****/
		 *value = strdup("");
		return TRX_OK;
	//}
	//return TRX_ERR;
}
TRX_STATUS setWANDeviceWANConnectionDeviceWANPOTSLinkConfig_DelayBetweenRetries(char *value)
{
	//InstanceDesc *idp;
	//if( idp=getCurrentInstanceDesc()){
		/* Add code here to set param -- delete idp if not needed *****/
		return TRX_OK;
	//}
	//return TRX_ERR;
}
TRX_STATUS getWANDeviceWANConnectionDeviceWANPOTSLinkConfig_DelayBetweenRetries(char **value)
{
	//InstanceDesc *idp;
	//if( idp=getCurrentInstanceDesc()){
		/* Add code here to get param -- delete idp if not needed *****/
		 *value = strdup("");
		return TRX_OK;
	//}
	//return TRX_ERR;
}
TRX_STATUS getWANDeviceWANConnectionDeviceWANPOTSLinkConfig_Fclass(char **value)
{
	//InstanceDesc *idp;
	//if( idp=getCurrentInstanceDesc()){
		/* Add code here to get param -- delete idp if not needed *****/
		 *value = strdup("");
		return TRX_OK;
	//}
	//return TRX_ERR;
}
TRX_STATUS getWANDeviceWANConnectionDeviceWANPOTSLinkConfig_DataModulationSupported(char **value)
{
	//InstanceDesc *idp;
	//if( idp=getCurrentInstanceDesc()){
		/* Add code here to get param -- delete idp if not needed *****/
		 *value = strdup("");
		return TRX_OK;
	//}
	//return TRX_ERR;
}
TRX_STATUS getWANDeviceWANConnectionDeviceWANPOTSLinkConfig_DataProtocol(char **value)
{
	//InstanceDesc *idp;
	//if( idp=getCurrentInstanceDesc()){
		/* Add code here to get param -- delete idp if not needed *****/
		 *value = strdup("");
		return TRX_OK;
	//}
	//return TRX_ERR;
}
TRX_STATUS getWANDeviceWANConnectionDeviceWANPOTSLinkConfig_DataCompression(char **value)
{
	//InstanceDesc *idp;
	//if( idp=getCurrentInstanceDesc()){
		/* Add code here to get param -- delete idp if not needed *****/
		 *value = strdup("");
		return TRX_OK;
	//}
	//return TRX_ERR;
}
TRX_STATUS getWANDeviceWANConnectionDeviceWANPOTSLinkConfig_PlusVTRCommandSupported(char **value)
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
TRX_STATUS setPortMappingEnabled(char *value)
{
#if 0
	InstanceDesc *idp;

	if ( (idp = getCurrentInstanceDesc()) ) {
		BcmCfm_SecVirtualServerCfg_t *pObj;
		uint32 index = idp->instanceID;
		if ( BcmCfm_objGet(BCMCFM_OBJ_SEC_VIRTUAL_SERVER, 
						   &(idp->hwUserData), &index) == BcmCfm_Ok ) {
			pObj = (BcmCfm_SecVirtualServerCfg_t *)idp->hwUserData;
			if ( strcmp(value, "1") == 0 )
				pObj->status = BcmCfm_CfgEnabled;
			else
				pObj->status = BcmCfm_CfgDisabled;
			if ( BcmCfm_objSet(BCMCFM_OBJ_SEC_VIRTUAL_SERVER, 
							   idp->hwUserData, index) == BcmCfm_Ok )
				return TRX_OK;
		}
	}
#endif
	BSTD_UNUSED(value);
	return TRX_ERR;
}
TRX_STATUS getPortMappingEnabled(char **value)
{
#if 0
	InstanceDesc *idp;

	if ( (idp = getCurrentInstanceDesc()) ) {
		BcmCfm_SecVirtualServerCfg_t *pObj;
		uint32 index = idp->instanceID;
		if ( BcmCfm_objGet(BCMCFM_OBJ_SEC_VIRTUAL_SERVER, 
						   &(idp->hwUserData), &index) == BcmCfm_Ok ) {
			pObj = (BcmCfm_SecVirtualServerCfg_t *)idp->hwUserData;
			if ( pObj->status == BcmCfm_CfgEnabled )
				*value = strdup("1");
			else
				*value = strdup("0");
			BcmCfm_objFree(BCMCFM_OBJ_SEC_VIRTUAL_SERVER, idp->hwUserData);
			return TRX_OK;
		}
	}
#endif
	BSTD_UNUSED(value);
	return TRX_ERR;
}
TRX_STATUS setPortMappingExternalPort(char *value)
{
#if 0
	InstanceDesc *idp;

	if ( (idp = getCurrentInstanceDesc()) ) {
		BcmCfm_SecVirtualServerCfg_t *pObj;
		uint32 index = idp->instanceID;
		if ( BcmCfm_objGet(BCMCFM_OBJ_SEC_VIRTUAL_SERVER, 
						   &(idp->hwUserData), &index) == BcmCfm_Ok ) {
			pObj = (BcmCfm_SecVirtualServerCfg_t *)idp->hwUserData;
			pObj->externalPortStart = pObj->externalPortEnd = atoi(value);
			if ( BcmCfm_objSet(BCMCFM_OBJ_SEC_VIRTUAL_SERVER, 
							   idp->hwUserData, index) == BcmCfm_Ok )
				return TRX_OK;
		}
	}
#endif
	BSTD_UNUSED(value);
	return TRX_ERR;
}
TRX_STATUS getPortMappingExternalPort(char **value)
{
#if 0
	InstanceDesc *idp;

	if ( (idp = getCurrentInstanceDesc()) ) {
		BcmCfm_SecVirtualServerCfg_t *pObj;
		uint32 index = idp->instanceID;
		char buf[IFC_TINY_LEN];
		if ( BcmCfm_objGet(BCMCFM_OBJ_SEC_VIRTUAL_SERVER, 
						   &(idp->hwUserData), &index) == BcmCfm_Ok ) {
			pObj = (BcmCfm_SecVirtualServerCfg_t *)idp->hwUserData;
			sprintf(buf, "%u", pObj->externalPortStart);
			*value = strdup(buf);
			BcmCfm_objFree(BCMCFM_OBJ_SEC_VIRTUAL_SERVER, idp->hwUserData);
			return TRX_OK;
		}
	}
#endif
	BSTD_UNUSED(value);
	return TRX_ERR;
}
TRX_STATUS setPortMappingInternalPort(char *value)
{
#if 0
	InstanceDesc *idp;

	if ( (idp = getCurrentInstanceDesc()) ) {
		BcmCfm_SecVirtualServerCfg_t *pObj;
		uint32 index = idp->instanceID;
		if ( BcmCfm_objGet(BCMCFM_OBJ_SEC_VIRTUAL_SERVER, 
						   &(idp->hwUserData), &index) == BcmCfm_Ok ) {
			pObj = (BcmCfm_SecVirtualServerCfg_t *)idp->hwUserData;
			pObj->internalPortStart = pObj->internalPortEnd = atoi(value);
			if ( BcmCfm_objSet(BCMCFM_OBJ_SEC_VIRTUAL_SERVER, 
							   idp->hwUserData, index) == BcmCfm_Ok )
				return TRX_OK;
		}
	}
#endif
	BSTD_UNUSED(value);
	return TRX_ERR;
}
TRX_STATUS getPortMappingInternalPort(char **value)
{
#if 0
	InstanceDesc *idp;

	if ( (idp = getCurrentInstanceDesc()) ) {
		BcmCfm_SecVirtualServerCfg_t *pObj;
		uint32 index = idp->instanceID;
		char buf[IFC_TINY_LEN];
		if ( BcmCfm_objGet(BCMCFM_OBJ_SEC_VIRTUAL_SERVER, 
						   &(idp->hwUserData), &index) == BcmCfm_Ok ) {
			pObj = (BcmCfm_SecVirtualServerCfg_t *)idp->hwUserData;
			sprintf(buf, "%u", pObj->internalPortStart);
			*value = strdup(buf);
			BcmCfm_objFree(BCMCFM_OBJ_SEC_VIRTUAL_SERVER, idp->hwUserData);
			return TRX_OK;
		}
	}
#endif
	BSTD_UNUSED(value);
	return TRX_ERR;
}
TRX_STATUS setPortMappingProtocol(char *value)
{
#if 0
	InstanceDesc *idp;

	if ( (idp = getCurrentInstanceDesc()) ) {
		BcmCfm_SecVirtualServerCfg_t *pObj;
		uint32 index = idp->instanceID;
		if ( BcmCfm_objGet(BCMCFM_OBJ_SEC_VIRTUAL_SERVER, 
						   &(idp->hwUserData), &index) == BcmCfm_Ok ) {
			pObj = (BcmCfm_SecVirtualServerCfg_t *)idp->hwUserData;
			/* clear pObj->protocol before setting it */
			pObj->protocol.tcp = pObj->protocol.udp = pObj->protocol.icmp = 0;

			if ( strcmp(value, "TCP") == 0 )
				pObj->protocol.tcp = TRUE;
			else if ( strcmp(value, "UDP") == 0 )
				pObj->protocol.udp = TRUE;
			else if ( strcmp(value, "ICMP") == 0 )
				pObj->protocol.icmp = TRUE;
			if ( BcmCfm_objSet(BCMCFM_OBJ_SEC_VIRTUAL_SERVER, 
							   idp->hwUserData, index) == BcmCfm_Ok )
				return TRX_OK;
		}
	}
#endif
	BSTD_UNUSED(value);
	return TRX_ERR;
}
TRX_STATUS getPortMappingProtocol(char **value)
{
#if 0
	InstanceDesc *idp;

	if ( (idp = getCurrentInstanceDesc()) ) {
		BcmCfm_SecVirtualServerCfg_t *pObj;
		uint32 index = idp->instanceID;
		if ( BcmCfm_objGet(BCMCFM_OBJ_SEC_VIRTUAL_SERVER, 
						   &(idp->hwUserData), &index) == BcmCfm_Ok ) {
			pObj = (BcmCfm_SecVirtualServerCfg_t *)idp->hwUserData;
			if ( pObj->protocol.tcp == TRUE && pObj->protocol.udp == TRUE )
				*value = strdup("TCP/UDP");
			else if ( pObj->protocol.tcp == TRUE )
				*value = strdup("TCP");
			else if ( pObj->protocol.udp == TRUE )
				*value = strdup("UDP");
			else if ( pObj->protocol.icmp == TRUE )
				*value = strdup("ICMP");
			else
				*value = strdup("");
			BcmCfm_objFree(BCMCFM_OBJ_SEC_VIRTUAL_SERVER, idp->hwUserData);
			return TRX_OK;
		}
	}
#endif
	BSTD_UNUSED(value);
	return TRX_ERR;
}
TRX_STATUS setPortMappingInternalClient(char *value)
{
#if 0
	InstanceDesc *idp;

	if ( (idp = getCurrentInstanceDesc()) ) {
		BcmCfm_SecVirtualServerCfg_t *pObj;
		struct in_addr inaddr;
		uint32 index = idp->instanceID;
		if ( BcmCfm_objGet(BCMCFM_OBJ_SEC_VIRTUAL_SERVER, 
						   &(idp->hwUserData), &index) == BcmCfm_Ok ) {
			pObj = (BcmCfm_SecVirtualServerCfg_t *)idp->hwUserData;
			inet_aton(value, &inaddr);
			pObj->ipAddress = inaddr.s_addr;
			if ( BcmCfm_objSet(BCMCFM_OBJ_SEC_VIRTUAL_SERVER, 
							   idp->hwUserData, index) == BcmCfm_Ok )
				return TRX_OK;
		}
	}
#endif
	BSTD_UNUSED(value);
	return TRX_ERR;
}
TRX_STATUS getPortMappingInternalClient(char **value)
{
#if 0
	InstanceDesc *idp;

	if ( (idp = getCurrentInstanceDesc()) ) {
		BcmCfm_SecVirtualServerCfg_t *pObj;
		struct in_addr inaddr;
		uint32 index = idp->instanceID;
		if ( BcmCfm_objGet(BCMCFM_OBJ_SEC_VIRTUAL_SERVER, 
						   &(idp->hwUserData), &index) == BcmCfm_Ok ) {
			pObj = (BcmCfm_SecVirtualServerCfg_t *)idp->hwUserData;
			inaddr.s_addr = pObj->ipAddress;
			BcmCfm_objFree(BCMCFM_OBJ_SEC_VIRTUAL_SERVER, idp->hwUserData);
			*value = strdup(inet_ntoa(inaddr));
			return TRX_OK;
		}
	}
#endif
	BSTD_UNUSED(value);
	return TRX_ERR;
}
TRX_STATUS setPortMappingDescription(char *value)
{
#if 0
	InstanceDesc *idp;

	if ( (idp = getCurrentInstanceDesc()) ) {
		BcmCfm_SecVirtualServerCfg_t *pObj;
		uint32 index = idp->instanceID;
		if ( BcmCfm_objGet(BCMCFM_OBJ_SEC_VIRTUAL_SERVER, 
						   &(idp->hwUserData), &index) == BcmCfm_Ok ) {
			pObj = (BcmCfm_SecVirtualServerCfg_t *)idp->hwUserData;
			if ( pObj->uName != NULL )
				BcmCfm_free(pObj->uName);
			BcmCfm_alloc(strlen(value) + 1, (void **)&(pObj->uName));
			strcpy(pObj->uName, value);
			if ( BcmCfm_objSet(BCMCFM_OBJ_SEC_VIRTUAL_SERVER, 
							   idp->hwUserData, index) == BcmCfm_Ok )
				return TRX_OK;
		}
	}
#endif
	BSTD_UNUSED(value);
	return TRX_ERR;
}
TRX_STATUS getPortMappingDescription(char **value)
{
#if 0
	InstanceDesc *idp;

	if ( (idp = getCurrentInstanceDesc()) ) {
		BcmCfm_SecVirtualServerCfg_t *pObj;
		uint32 index = idp->instanceID;
		if ( BcmCfm_objGet(BCMCFM_OBJ_SEC_VIRTUAL_SERVER, 
						   &(idp->hwUserData), &index) == BcmCfm_Ok ) {
			pObj = (BcmCfm_SecVirtualServerCfg_t *)idp->hwUserData;
			*value = strdup(pObj->uName);
			BcmCfm_objFree(BCMCFM_OBJ_SEC_VIRTUAL_SERVER, idp->hwUserData);
			return TRX_OK;
		}
	}
#endif
	BSTD_UNUSED(value);
	return TRX_ERR;
}

/** */
/** InternetGatewayDevice.WANDevice.WANConnectionDevice.WANIPConnection.PortMapping. */

TRX_STATUS setWANDeviceWANConnectionDeviceWANIPConnectionPortMapping_PortMappingEnabled(char *value)
{
	return setPortMappingEnabled(value);
}
TRX_STATUS getWANDeviceWANConnectionDeviceWANIPConnectionPortMapping_PortMappingEnabled(char **value)
{
	return getPortMappingEnabled(value);
}
#if 0
TRX_STATUS setWANDeviceWANConnectionDeviceWANIPConnectionPortMapping_PortMappingLeaseDuration(char *value)
{
	//InstanceDesc *idp;
	//if( idp=getCurrentInstanceDesc()){
		/* Add code here to set param -- delete idp if not needed *****/
		return TRX_OK;
	//}
	//return TRX_ERR;
}
TRX_STATUS getWANDeviceWANConnectionDeviceWANIPConnectionPortMapping_PortMappingLeaseDuration(char **value)
{
	//InstanceDesc *idp;
	//if( idp=getCurrentInstanceDesc()){
		/* Add code here to get param -- delete idp if not needed *****/
		 *value = strdup("");
		return TRX_OK;
	//}
	//return TRX_ERR;
}
TRX_STATUS setWANDeviceWANConnectionDeviceWANIPConnectionPortMapping_RemoteHost(char *value)
{
	//InstanceDesc *idp;
	//if( idp=getCurrentInstanceDesc()){
		/* Add code here to set param -- delete idp if not needed *****/
		return TRX_OK;
	//}
	//return TRX_ERR;
}
TRX_STATUS getWANDeviceWANConnectionDeviceWANIPConnectionPortMapping_RemoteHost(char **value)
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
TRX_STATUS setWANDeviceWANConnectionDeviceWANIPConnectionPortMapping_ExternalPort(char *value)
{
	return setPortMappingExternalPort(value);
}
TRX_STATUS getWANDeviceWANConnectionDeviceWANIPConnectionPortMapping_ExternalPort(char **value)
{
	return getPortMappingExternalPort(value);
}
TRX_STATUS setWANDeviceWANConnectionDeviceWANIPConnectionPortMapping_InternalPort(char *value)
{
	return setPortMappingInternalPort(value);
}
TRX_STATUS getWANDeviceWANConnectionDeviceWANIPConnectionPortMapping_InternalPort(char **value)
{
	return getPortMappingInternalPort(value);
}
TRX_STATUS setWANDeviceWANConnectionDeviceWANIPConnectionPortMapping_PortMappingProtocol(char *value)
{
	return setPortMappingProtocol(value);
}
TRX_STATUS getWANDeviceWANConnectionDeviceWANIPConnectionPortMapping_PortMappingProtocol(char **value)
{
	return getPortMappingProtocol(value);
}
TRX_STATUS setWANDeviceWANConnectionDeviceWANIPConnectionPortMapping_InternalClient(char *value)
{
	return setPortMappingInternalClient(value);
}
TRX_STATUS getWANDeviceWANConnectionDeviceWANIPConnectionPortMapping_InternalClient(char **value)
{
	return getPortMappingInternalClient(value);
}
TRX_STATUS setWANDeviceWANConnectionDeviceWANIPConnectionPortMapping_PortMappingDescription(char *value)
{
	return setPortMappingDescription(value);
}
TRX_STATUS getWANDeviceWANConnectionDeviceWANIPConnectionPortMapping_PortMappingDescription(char **value)
{
	return getPortMappingDescription(value);
}

/** */
/** InternetGatewayDevice.WANDevice.WANConnectionDevice.WANIPConnection.PortMapping. */

TRX_STATUS addWANDeviceWANConnectionDeviceWANIPConnectionPortMappingI(char **value)
{
#if 0
	InstanceDesc *idp;

	if ( (idp = getNewInstanceDesc(getCurrentNode(), getCurrentInstanceDesc(), 0)) ) {
		uint32 index = 0;
		if ( BcmCfm_objCreate(BCMCFM_OBJ_SEC_VIRTUAL_SERVER,
							  &index) == BcmCfm_Ok ) {
			/* now set it with something so it doesn't go away */
			void *info;
			BcmCfm_SecVirtualServerCfg_t *pObj;
			if ( BcmCfm_objGet(BCMCFM_OBJ_SEC_VIRTUAL_SERVER, 
							   &info, &index) == BcmCfm_Ok ) {
				pObj = (BcmCfm_SecVirtualServerCfg_t *)info;
				pObj->status = BcmCfm_CfgDisabled;
				pObj->externalPortStart = 65535;
				pObj->externalPortEnd = 65535;
				pObj->internalPortStart = 65535;
				pObj->internalPortEnd = 65535;
				pObj->protocol.tcp = TRUE;
				BcmCfm_objSet(BCMCFM_OBJ_SEC_VIRTUAL_SERVER, 
							  info, index);
				idp->instanceID = index;
				idp->hwUserData = NULL;
				*value = strdup(itoa(idp->instanceID));
				return TRX_OK; /* or TRX_BOOT if necessary */
			}
		}
	}
#endif
	BSTD_UNUSED(value);
	return TRX_ERR;
}
TRX_STATUS delWANDeviceWANConnectionDeviceWANIPConnectionPortMappingI(char *value)
{
	TRxObjNode *n;
	InstanceDesc *idp;
	int     id = atoi(value);

	if ( (idp = findInstanceDesc(n=getCurrentNode(), id)) ) {
		uint32 index = idp->instanceID;
		if ( BcmCfm_objDelete(BCMCFM_OBJ_SEC_VIRTUAL_SERVER,
							  index) == BcmCfm_Ok ) {
			if ( !deleteInstanceDesc(n, id) )
				return TRX_OK; /* or TRX_BOOT if necessary */
		}
	}

	return TRX_ERR;
}

/** */
/** InternetGatewayDevice.WANDevice.WANConnectionDevice.WANIPConnection.Stats. */
TRX_STATUS getWANDeviceWANConnectionDeviceWANIPConnectionStats(char **value, BcmCfm_Stats statsType)
{
#if 0
	InstanceDesc *idp;
	void *obj;
	if ( (idp = getCurrentInstanceDesc()) ) {
		uint32 index = idp->instanceID;
		char buf[64];
		if ( BcmCfm_stsGet(BCMCFM_OBJ_NTWK_INTF, &obj, &index) == BcmCfm_Ok ) {
			PBcmCfm_NtwkIntfSts_t pObj = (PBcmCfm_NtwkIntfSts_t)obj;
			if ( pObj != NULL ) {
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
	}
#endif
	BSTD_UNUSED(value);
	BSTD_UNUSED(statsType);
	return TRX_ERR;
}

TRX_STATUS getWANDeviceWANConnectionDeviceWANIPConnectionStats_EthernetBytesSent(char **value)
{
	return (getWANDeviceWANConnectionDeviceWANIPConnectionStats(value, BcmCfm_StatsTxBytes));
}

TRX_STATUS getWANDeviceWANConnectionDeviceWANIPConnectionStats_EthernetBytesReceived(char **value)
{
	return (getWANDeviceWANConnectionDeviceWANIPConnectionStats(value, BcmCfm_StatsRxBytes));
}
TRX_STATUS getWANDeviceWANConnectionDeviceWANIPConnectionStats_EthernetPacketsSent(char **value)
{
	return (getWANDeviceWANConnectionDeviceWANIPConnectionStats(value, BcmCfm_StatsTxPkts));
}
TRX_STATUS getWANDeviceWANConnectionDeviceWANIPConnectionStats_EthernetPacketsReceived(char **value)
{
	return (getWANDeviceWANConnectionDeviceWANIPConnectionStats(value, BcmCfm_StatsRxPkts));
}

/** */
/** InternetGatewayDevice.WANDevice.WANConnectionDevice.WANIPConnection. */

TRX_STATUS setWANDeviceWANConnectionDeviceWANIPConnection_Enable(char *value)
{
#if 0
	InstanceDesc *idp;
	void *obj;
	if ( (idp = getCurrentInstanceDesc()) ) {
		PBcmCfm_NtwkIntfCfg_t pObj;
		uint32 index = idp->instanceID;
		if ( BcmCfm_objGet(BCMCFM_OBJ_NTWK_INTF, &obj, &index) == BcmCfm_Ok ) {
			pObj = (PBcmCfm_NtwkIntfCfg_t)obj;
			if ( pObj != NULL ){
				pObj->status = streq(value,"1")? BcmCfm_CfgEnabled: BcmCfm_CfgDisabled;
				if ( BcmCfm_objSet(BCMCFM_OBJ_NTWK_INTF, obj, index) == BcmCfm_Ok )
					return TRX_REBOOT;
			}
		}
	}
#endif
	BSTD_UNUSED(value);
	return TRX_ERR;
}

TRX_STATUS getWANDeviceWANConnectionDeviceWANIPConnection_Enable(char **value)
{
#if 0
	InstanceDesc *idp;
	void *obj;
	if ( (idp = getCurrentInstanceDesc()) ) {
		PBcmCfm_NtwkIntfCfg_t pObj;
		uint32 index = idp->instanceID;
		if ( BcmCfm_objGet(BCMCFM_OBJ_NTWK_INTF, &obj, &index) == BcmCfm_Ok ) {
			pObj = (PBcmCfm_NtwkIntfCfg_t)obj;
			if ( pObj != NULL ){
				*value = strdup( pObj->status == BcmCfm_CfgEnabled? "1": "0");
				BcmCfm_objFree(BCMCFM_OBJ_NTWK_INTF, obj);
				return TRX_OK;
			} 
		}
	}
#endif

	BSTD_UNUSED(value);
	return TRX_ERR;
}

TRX_STATUS getWANDeviceWANConnectionDeviceWANIPConnection_ConnectionStatus(char **value)
{
#if 0
	InstanceDesc *idp;
	void	*obj;

	if ( (idp = getCurrentInstanceDesc()) ) {
		PBcmCfm_NtwkIntfSts_t pObj = NULL;
		uint32 index = idp->instanceID;	   
		if ( BcmCfm_stsGet(BCMCFM_OBJ_NTWK_INTF, &obj, &index) == BcmCfm_Ok ) {
			pObj = (PBcmCfm_NtwkIntfSts_t)obj;	  
			if ( pObj == NULL )	{
				BcmCfm_objFree(BCMCFM_OBJ_NTWK_INTF, pObj);
				return TRX_ERR;
			}
			switch(pObj->proto.ip.state) {
			case BcmCfm_DHCPStateUp:
				*value = strdup("Connected");
				break;
			case BcmCfm_DHCPStateDown:
				*value = strdup("Disconnected");
				break;
			case BcmCfm_DHCPStateUnknown:
			case BcmCfm_DHCPStateInProgress:
			default:
				*value = strdup("Connecting");
				break;
			}
			BcmCfm_stsFree(BCMCFM_OBJ_NTWK_INTF, obj);
			return TRX_OK;
		} else {
			*value = strdup("Unconfigured");
			return TRX_OK;
		}
	}
#endif
	BSTD_UNUSED(value);
	return TRX_ERR;
}
#if 0
TRX_STATUS getWANDeviceWANConnectionDeviceWANIPConnection_PossibleConnectionTypes(char **value)
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
TRX_STATUS setWANDeviceWANConnectionDeviceWANIPConnection_ConnectionType(char *value)
{
#if 0
	InstanceDesc *idp;
	void *obj;
	if ( (idp = getCurrentInstanceDesc()) ) {
		PBcmCfm_NtwkIntfCfg_t pObj;
		uint32 index = idp->instanceID;
		if ( BcmCfm_objGet(BCMCFM_OBJ_NTWK_INTF, &obj, &index) == BcmCfm_Ok ) {
			pObj = (PBcmCfm_NtwkIntfCfg_t)obj;
			if ( pObj != NULL ){
				if ( strcasecmp(value, "IP_Bridged")==0 )
					pObj->proto.ip.addr = 0; /* otherwise we expect ExternalIPAddress to supply value */
				if ( BcmCfm_objSet(BCMCFM_OBJ_NTWK_INTF, obj, index) == BcmCfm_Ok )
					return TRX_REBOOT;
			}
		}
	}
#endif
	BSTD_UNUSED(value);
	return TRX_ERR;
}
TRX_STATUS getWANDeviceWANConnectionDeviceWANIPConnection_ConnectionType(char **value)
{
#if 0
	InstanceDesc *idp;
	void *obj;
	if ( (idp = getCurrentInstanceDesc()) ) {
		PBcmCfm_NtwkIntfCfg_t pObj;
		uint32 index = idp->instanceID;
		if ( BcmCfm_objGet(BCMCFM_OBJ_NTWK_INTF, &obj, &index) == BcmCfm_Ok ) {
			pObj = (PBcmCfm_NtwkIntfCfg_t)obj;
			if ( pObj != NULL ){
				*value = strdup((pObj->proto.ip.addr==0 &&
					pObj->proto.ip.dhcpClientEnable == BcmCfm_CfgDisabled)
					? "IP_Bridged": "IP_Routed");
				BcmCfm_objFree(BCMCFM_OBJ_NTWK_INTF, obj);
				return TRX_OK;
			} 
		}
	}
#endif
	BSTD_UNUSED(value);
	return TRX_ERR;
}
TRX_STATUS setWANDeviceWANConnectionDeviceWANIPConnection_Name(char *value)
{
#if 0
	InstanceDesc *idp;
	void *obj;
	if ( (idp = getCurrentInstanceDesc()) ) {
		PBcmCfm_NtwkIntfCfg_t pObj;
		uint32 index = idp->instanceID;
		if ( BcmCfm_objGet(BCMCFM_OBJ_NTWK_INTF, &obj, &index) == BcmCfm_Ok ) {
			pObj = (PBcmCfm_NtwkIntfCfg_t)obj;
			if ( pObj != NULL ){
				BcmCfm_free((void *)pObj->uName);
				BcmCfm_alloc(strlen(value) + 1, (void **)&(pObj->uName));
				strcpy(pObj->uName, value);
				if ( BcmCfm_objSet(BCMCFM_OBJ_NTWK_INTF, obj, index) == BcmCfm_Ok )
					return TRX_OK;
			}
		}
	}
#endif
	BSTD_UNUSED(value);
	return TRX_ERR;
}
TRX_STATUS getWANDeviceWANConnectionDeviceWANIPConnection_Name(char **value)
{
#if 0
	InstanceDesc *idp;
	void *obj;
	if ( (idp = getCurrentInstanceDesc()) ) {
		PBcmCfm_NtwkIntfCfg_t pObj;
		uint32 index = idp->instanceID;
		if ( BcmCfm_objGet(BCMCFM_OBJ_NTWK_INTF, &obj, &index) == BcmCfm_Ok ) {
			pObj = (PBcmCfm_NtwkIntfCfg_t)obj;
			if ( pObj != NULL ){
				*value = strdup( pObj->uName);
				BcmCfm_objFree(BCMCFM_OBJ_NTWK_INTF, obj);
				return TRX_OK;
			} 
		}
	}
#endif
	BSTD_UNUSED(value);
	return TRX_ERR;
}
#if 0
TRX_STATUS getWANDeviceWANConnectionDeviceWANIPConnection_Uptime(char **value)
{
	//InstanceDesc *idp;
	//if( idp=getCurrentInstanceDesc()){
		/* Add code here to get param -- delete idp if not needed *****/
		 *value = strdup("");
		return TRX_OK;
	//}
	//return TRX_ERR;
}
TRX_STATUS getWANDeviceWANConnectionDeviceWANIPConnection_LastConnectionError(char **value)
{
	//InstanceDesc *idp;
	//if( idp=getCurrentInstanceDesc()){
		/* Add code here to get param -- delete idp if not needed *****/
		 *value = strdup("");
		return TRX_OK;
	//}
	//return TRX_ERR;
}
TRX_STATUS setWANDeviceWANConnectionDeviceWANIPConnection_AutoDisconnectTime(char *value)
{
	//InstanceDesc *idp;
	//if( idp=getCurrentInstanceDesc()){
		/* Add code here to set param -- delete idp if not needed *****/
		return TRX_OK;
	//}
	//return TRX_ERR;
}
TRX_STATUS getWANDeviceWANConnectionDeviceWANIPConnection_AutoDisconnectTime(char **value)
{
	//InstanceDesc *idp;
	//if( idp=getCurrentInstanceDesc()){
		/* Add code here to get param -- delete idp if not needed *****/
		 *value = strdup("");
		return TRX_OK;
	//}
	//return TRX_ERR;
}
TRX_STATUS setWANDeviceWANConnectionDeviceWANIPConnection_IdleDisconnectTime(char *value)
{
	//InstanceDesc *idp;
	//if( idp=getCurrentInstanceDesc()){
		/* Add code here to set param -- delete idp if not needed *****/
		return TRX_OK;
	//}
	//return TRX_ERR;
}
TRX_STATUS getWANDeviceWANConnectionDeviceWANIPConnection_IdleDisconnectTime(char **value)
{
	//InstanceDesc *idp;
	//if( idp=getCurrentInstanceDesc()){
		/* Add code here to get param -- delete idp if not needed *****/
		 *value = strdup("");
		return TRX_OK;
	//}
	//return TRX_ERR;
}
TRX_STATUS setWANDeviceWANConnectionDeviceWANIPConnection_WarnDisconnectDelay(char *value)
{
	//InstanceDesc *idp;
	//if( idp=getCurrentInstanceDesc()){
		/* Add code here to set param -- delete idp if not needed *****/
		return TRX_OK;
	//}
	//return TRX_ERR;
}
TRX_STATUS getWANDeviceWANConnectionDeviceWANIPConnection_WarnDisconnectDelay(char **value)
{
	//InstanceDesc *idp;
	//if( idp=getCurrentInstanceDesc()){
		/* Add code here to get param -- delete idp if not needed *****/
		 *value = strdup("");
		return TRX_OK;
	//}
	//return TRX_ERR;
}
TRX_STATUS getWANDeviceWANConnectionDeviceWANIPConnection_RSIPAvailable(char **value)
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
TRX_STATUS setWANDeviceWANConnectionDeviceWANIPConnection_NATEnabled(char *value)
{
	TRX_STATUS setPPPNATEnabled(char *value);
	return(setPPPNATEnabled(value));
}
TRX_STATUS getWANDeviceWANConnectionDeviceWANIPConnection_NATEnabled(char **value)
{
	TRX_STATUS getPPPNATEnabled(char **value);
	return(getPPPNATEnabled(value));
}
TRX_STATUS setWANDeviceWANConnectionDeviceWANIPConnection_AddressingType(char *value)
{
#if 0
	InstanceDesc *idp;
	void *obj;
	if ( (idp = getCurrentInstanceDesc()) ) {
		PBcmCfm_NtwkIntfCfg_t pObj;
		uint32 index = idp->instanceID;
		if ( BcmCfm_objGet(BCMCFM_OBJ_NTWK_INTF, &obj, &index) == BcmCfm_Ok ) {
			pObj = (PBcmCfm_NtwkIntfCfg_t)obj;
			if ( pObj != NULL ){
				pObj->proto.ip.dhcpClientEnable = strcasecmp(value,"DHCP")==0?
												BcmCfm_CfgEnabled: BcmCfm_CfgDisabled;
				if ( BcmCfm_objSet(BCMCFM_OBJ_NTWK_INTF, obj, index) == BcmCfm_Ok )
					return TRX_REBOOT;
			}
		}
	}
#endif
	BSTD_UNUSED(value);
	return TRX_ERR;
}

TRX_STATUS getWANDeviceWANConnectionDeviceWANIPConnection_AddressingType(char **value)
{
#if 0
	InstanceDesc *idp;
	void *obj;
	if ( (idp = getCurrentInstanceDesc()) ) {
		PBcmCfm_NtwkIntfCfg_t pObj;
		uint32 index = idp->instanceID;
		if ( BcmCfm_objGet(BCMCFM_OBJ_NTWK_INTF, &obj, &index) == BcmCfm_Ok ) {
			pObj = (PBcmCfm_NtwkIntfCfg_t)obj;
			if ( pObj != NULL ){
				*value = strdup( pObj->proto.ip.dhcpClientEnable==BcmCfm_CfgEnabled?
								 "DHCP": "Static");
				BcmCfm_objFree(BCMCFM_OBJ_NTWK_INTF, obj);
				return TRX_OK;
			} 
		}
	}
#endif
	BSTD_UNUSED(value);
	return TRX_ERR;
}

TRX_STATUS setWANDeviceWANConnectionDeviceWANIPConnection_ExternalIPAddress(char *value)
{
#if 0
	InstanceDesc *idp;
	void *obj;
	if ( (idp = getCurrentInstanceDesc()) ) {
		PBcmCfm_NtwkIntfCfg_t pObj;
		uint32 index = idp->instanceID;
		if ( BcmCfm_objGet(BCMCFM_OBJ_NTWK_INTF, &obj, &index) == BcmCfm_Ok ) {
			pObj = (PBcmCfm_NtwkIntfCfg_t)obj;
			if ( pObj != NULL ){
				pObj->proto.ip.addr = readIp(value);
				if ( BcmCfm_objSet(BCMCFM_OBJ_NTWK_INTF, obj, index) == BcmCfm_Ok )
					return TRX_REBOOT;
			}
		}
	}
#endif
	BSTD_UNUSED(value);
	return TRX_ERR;
}
TRX_STATUS getWANDeviceWANConnectionDeviceWANIPConnection_ExternalIPAddress(char **value)
{
#if 0
	InstanceDesc *idp;
	void *obj;
	if ( (idp = getCurrentInstanceDesc()) ) {
		PBcmCfm_NtwkIntfCfg_t pObj;
		uint32 index = idp->instanceID;
		if ( BcmCfm_objGet(BCMCFM_OBJ_NTWK_INTF, &obj, &index) == BcmCfm_Ok ) {
			pObj = (PBcmCfm_NtwkIntfCfg_t)obj;
			if ( pObj != NULL ){
				*value = strdup( writeIp(pObj->proto.ip.addr));
				BcmCfm_objFree(BCMCFM_OBJ_NTWK_INTF, obj);
				return TRX_OK;
			} 
		}
	}
#endif
	BSTD_UNUSED(value);
	return TRX_ERR;
}
TRX_STATUS setWANDeviceWANConnectionDeviceWANIPConnection_SubnetMask(char *value)
{
#if 0
	InstanceDesc *idp;
	void *obj;
	if ( (idp = getCurrentInstanceDesc()) ) {
		PBcmCfm_NtwkIntfCfg_t pObj;
		uint32 index = idp->instanceID;
		if ( BcmCfm_objGet(BCMCFM_OBJ_NTWK_INTF, &obj, &index) == BcmCfm_Ok ) {
			pObj = (PBcmCfm_NtwkIntfCfg_t)obj;
			if ( pObj != NULL ){
				pObj->proto.ip.mask = readIp(value);
				if ( BcmCfm_objSet(BCMCFM_OBJ_NTWK_INTF, obj, index) == BcmCfm_Ok )
					return TRX_REBOOT;
			}
		}
	}
#endif
	BSTD_UNUSED(value);
	return TRX_ERR;
}
TRX_STATUS getWANDeviceWANConnectionDeviceWANIPConnection_SubnetMask(char **value)
{
#if 0
	InstanceDesc *idp;
	void *obj;
	if ( (idp = getCurrentInstanceDesc()) ) {
		PBcmCfm_NtwkIntfCfg_t pObj;
		uint32 index = idp->instanceID;
		if ( BcmCfm_objGet(BCMCFM_OBJ_NTWK_INTF, &obj, &index) == BcmCfm_Ok ) {
			pObj = (PBcmCfm_NtwkIntfCfg_t)obj;
			if ( pObj != NULL ){
				*value = strdup( writeIp(pObj->proto.ip.mask));
				BcmCfm_objFree(BCMCFM_OBJ_NTWK_INTF, obj);
				return TRX_OK;
			}
		}
	}
#endif
	BSTD_UNUSED(value);
	return TRX_ERR;
}
TRX_STATUS setWANDeviceWANConnectionDeviceWANIPConnection_DefaultGateway(char *value)
{
	return tr69c_setDefaultGateway(value);
}
TRX_STATUS getWANDeviceWANConnectionDeviceWANIPConnection_DefaultGateway(char **value)
{
	return tr69c_getDefaultGateway(value);
}
#if 0
TRX_STATUS setWANDeviceWANConnectionDeviceWANIPConnection_DNSEnabled(char *value)
{
	//InstanceDesc *idp;
	//if( idp=getCurrentInstanceDesc()){
		/* Add code here to set param -- delete idp if not needed *****/
		return TRX_OK;
	//}
	//return TRX_ERR;
}
TRX_STATUS getWANDeviceWANConnectionDeviceWANIPConnection_DNSEnabled(char **value)
{
	//InstanceDesc *idp;
	//if( idp=getCurrentInstanceDesc()){
		/* Add code here to get param -- delete idp if not needed *****/
		 *value = strdup("");
		return TRX_OK;
	//}
	//return TRX_ERR;
}
TRX_STATUS setWANDeviceWANConnectionDeviceWANIPConnection_DNSOverrideAllowed(char *value)
{
	//InstanceDesc *idp;
	//if( idp=getCurrentInstanceDesc()){
		/* Add code here to set param -- delete idp if not needed *****/
		return TRX_OK;
	//}
	//return TRX_ERR;
}
TRX_STATUS getWANDeviceWANConnectionDeviceWANIPConnection_DNSOverrideAllowed(char **value)
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

TRX_STATUS setWANDeviceWANConnectionDeviceWANIPConnection_DNSServers(char *value)
{
	return tr69c_setDNSServers(value);
}

TRX_STATUS getWANDeviceWANConnectionDeviceWANIPConnection_DNSServers(char **value)
{
	return tr69c_getDNSServers(value);
}

#if 0
TRX_STATUS setWANDeviceWANConnectionDeviceWANIPConnection_MaxMTUSize(char *value)
{
	//InstanceDesc *idp;
	//if( idp=getCurrentInstanceDesc()){
		/* Add code here to set param -- delete idp if not needed *****/
		return TRX_OK;
	//}
	//return TRX_ERR;
}
TRX_STATUS getWANDeviceWANConnectionDeviceWANIPConnection_MaxMTUSize(char **value)
{
	//InstanceDesc *idp;
	//if( idp=getCurrentInstanceDesc()){
		/* Add code here to get param -- delete idp if not needed *****/
		 *value = strdup("");
		return TRX_OK;
	//}
	//return TRX_ERR;
}
TRX_STATUS setWANDeviceWANConnectionDeviceWANIPConnection_MACAddress(char *value)
{
	//InstanceDesc *idp;
	//if( idp=getCurrentInstanceDesc()){
		/* Add code here to set param -- delete idp if not needed *****/
		return TRX_OK;
	//}
	//return TRX_ERR;
}
TRX_STATUS getWANDeviceWANConnectionDeviceWANIPConnection_MACAddress(char **value)
{
	//InstanceDesc *idp;
	//if( idp=getCurrentInstanceDesc()){
		/* Add code here to get param -- delete idp if not needed *****/
		 *value = strdup("");
		return TRX_OK;
	//}
	//return TRX_ERR;
}
TRX_STATUS setWANDeviceWANConnectionDeviceWANIPConnection_MACAddressOverride(char *value)
{
	//InstanceDesc *idp;
	//if( idp=getCurrentInstanceDesc()){
		/* Add code here to set param -- delete idp if not needed *****/
		return TRX_OK;
	//}
	//return TRX_ERR;
}
TRX_STATUS getWANDeviceWANConnectionDeviceWANIPConnection_MACAddressOverride(char **value)
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

TRX_STATUS setWANDeviceWANConnectionDeviceWANIPConnection_ConnectionTrigger(char *value)
{
	InstanceDesc *idp;

	if ( (idp = getCurrentInstanceDesc()) ) {
		/* only support AlwaysOn so just ignore set if instance is correct */
		return TRX_OK;
	}
	BSTD_UNUSED(value);
	return TRX_ERR;
}

TRX_STATUS getWANDeviceWANConnectionDeviceWANIPConnection_ConnectionTrigger(char **value)
{
	InstanceDesc *idp;

	if ( (idp = getCurrentInstanceDesc()) ) {
		/* only support AlwaysOn so just ignore set if instance is correct */
		*value = strdup("AlwaysOn");
		return TRX_OK;
	}
	return TRX_ERR;
}

#if 0
TRX_STATUS setWANDeviceWANConnectionDeviceWANIPConnection_RouteProtocolRx(char *value)
{
	//InstanceDesc *idp;
	//if( idp=getCurrentInstanceDesc()){
		/* Add code here to set param -- delete idp if not needed *****/
		return TRX_OK;
	//}
	//return TRX_ERR;
}
TRX_STATUS getWANDeviceWANConnectionDeviceWANIPConnection_RouteProtocolRx(char **value)
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

extern TRxObjNode  WANDeviceWANConnectionDeviceWANIPConnectionInstanceDesc[];
extern TRxObjNode  WANDeviceWANConnectionDeviceWANIPConnectionPortMappingInstanceDesc[];
/*static int	vrtSrvInit=0;*/
int initWANPortMappingInstances(TRxObjNode *pmNode, InstanceDesc *pppParentIdp)
{
#if 0
	int cnt;
	if (vrtSrvInit==0) {
		BcmCfmSec_initVrtSrv();
		vrtSrvInit = 1;
	}
	cnt = reInitInstancesFromBCMObj(BCMCFM_OBJ_SEC_VIRTUAL_SERVER, pmNode, pppParentIdp);
	return cnt;
#endif
	BSTD_UNUSED(pmNode);
	BSTD_UNUSED(pppParentIdp);
	return 1;
}

int initWANIPConnectionPortMappingInstance(InstanceDesc *ipParentIdp) {
	TRxObjNode *pmNode;
	int cnt = 0;
	/* InternetGatewayDevice.WANDevice.{i}.WANConnectionDevice.{i}.WANIPConnection.PortMapping.{i}.*/
	pmNode = WANDeviceWANConnectionDeviceWANIPConnectionPortMappingInstanceDesc;
	cnt = initWANPortMappingInstances(pmNode,ipParentIdp);
	return cnt;
}

TRX_STATUS getWANDeviceWANConnectionDeviceWANIPConnection_PortMappingNumberOfEntries(char **value)
{
	int		cnt;
	cnt = getInstanceCount( WANDeviceWANConnectionDeviceWANIPConnectionPortMappingInstanceDesc );
	*value = strdup( itoa(cnt));
	return TRX_OK;
}

/** */
/** InternetGatewayDevice.WANDevice.WANConnectionDevice.WANIPConnection. */

TRX_STATUS addWANDeviceWANConnectionDeviceWANIPConnectionI(char **value)
{
#if 0
	InstanceDesc *idp;
	InstanceDesc *connDevI = getCurrentInstanceDesc(); /* get instance of ConnectionDevice */
	if ( (idp = getNewInstanceDesc(getCurrentNode(), connDevI, 0)) ) {
		uint32 index = 0;
		void *objValue;
		PBcmCfm_NtwkIntfCfg_t pObj = NULL;
		if ( BcmCfm_objCreate(BCMCFM_OBJ_NTWK_INTF,
							  &index) == BcmCfm_Ok ) {
			idp->instanceID = index;
			idp->hwUserData = NULL;
			*value = strdup(itoa(idp->instanceID));
			BcmCfm_objGet(BCMCFM_OBJ_NTWK_INTF, &objValue, &index);
			pObj = (PBcmCfm_NtwkIntfCfg_t)objValue;
			if ( pObj != NULL ) {
				char *linktype = NULL;
				getDSLLinkType(&linktype);
				pObj->protocol = BcmCfm_NtwkIntfProtoMAC;
				if( linktype ) {
					if( strcasecmp(linktype, "IPoA") == 0 ||
						strcasecmp(linktype, "CIP") == 0 )
						pObj->protocol = BcmCfm_NtwkIntfProtoIPoA;
					free(linktype);
				}
   				pObj->attachToObjId = BCMCFM_OBJ_IFC_ATM_VCC;
				if( connDevI )
					pObj->attachToIndex = connDevI->instanceID;
				BcmCfm_objSet(BCMCFM_OBJ_NTWK_INTF, objValue, index);
				return TRX_OK; 
			}
		}
	}
#endif
	BSTD_UNUSED(value);
	return TRX_ERR;
}
TRX_STATUS delWANDeviceWANConnectionDeviceWANIPConnectionI(char *value)
{
	TRxObjNode *n;
	InstanceDesc *idp;
	int     id = atoi(value);

	if ( (idp = findInstanceDesc(n=getCurrentNode(), id)) ) {
		uint32 index = idp->instanceID;
		if ( BcmCfm_objDelete(BCMCFM_OBJ_NTWK_INTF,
							  index) == BcmCfm_Ok ) {
			if ( !deleteInstanceDesc(n, id) )
				return TRX_REBOOT; /* or TRX_BOOT if necessary */
		}
	}

	return TRX_ERR;
}

/** */
/** InternetGatewayDevice.WANDevice.WANConnectionDevice.WANPPPConnection.PortMapping. */

TRX_STATUS setWANDeviceWANConnectionDeviceWANPPPConnectionPortMapping_PortMappingEnabled(char *value)
{
	return setPortMappingEnabled(value);
}
TRX_STATUS getWANDeviceWANConnectionDeviceWANPPPConnectionPortMapping_PortMappingEnabled(char **value)
{
	return getPortMappingEnabled(value);
}
#if 0
TRX_STATUS setWANDeviceWANConnectionDeviceWANPPPConnectionPortMapping_PortMappingLeaseDuration(char *value)
{
	//InstanceDesc *idp;
	//if( idp=getCurrentInstanceDesc()){
		/* Add code here to set param -- delete idp if not needed *****/
		return TRX_OK;
	//}
	//return TRX_ERR;
}
TRX_STATUS getWANDeviceWANConnectionDeviceWANPPPConnectionPortMapping_PortMappingLeaseDuration(char **value)
{
	//InstanceDesc *idp;
	//if( idp=getCurrentInstanceDesc()){
		/* Add code here to get param -- delete idp if not needed *****/
		 *value = strdup("");
		return TRX_OK;
	//}
	//return TRX_ERR;
}
TRX_STATUS setWANDeviceWANConnectionDeviceWANPPPConnectionPortMapping_RemoteHost(char *value)
{
	//InstanceDesc *idp;
	//if( idp=getCurrentInstanceDesc()){
		/* Add code here to set param -- delete idp if not needed *****/
		return TRX_OK;
	//}
	//return TRX_ERR;
}
TRX_STATUS getWANDeviceWANConnectionDeviceWANPPPConnectionPortMapping_RemoteHost(char **value)
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
TRX_STATUS setWANDeviceWANConnectionDeviceWANPPPConnectionPortMapping_ExternalPort(char *value)
{
	return setPortMappingExternalPort(value);
}
TRX_STATUS getWANDeviceWANConnectionDeviceWANPPPConnectionPortMapping_ExternalPort(char **value)
{
	return getPortMappingExternalPort(value);
}
TRX_STATUS setWANDeviceWANConnectionDeviceWANPPPConnectionPortMapping_InternalPort(char *value)
{
	return setPortMappingInternalPort(value);
}
TRX_STATUS getWANDeviceWANConnectionDeviceWANPPPConnectionPortMapping_InternalPort(char **value)
{
	return getPortMappingInternalPort(value);
}
TRX_STATUS setWANDeviceWANConnectionDeviceWANPPPConnectionPortMapping_PortMappingProtocol(char *value)
{
	return setPortMappingProtocol(value);
}
TRX_STATUS getWANDeviceWANConnectionDeviceWANPPPConnectionPortMapping_PortMappingProtocol(char **value)
{
	return getPortMappingProtocol(value);
}
TRX_STATUS setWANDeviceWANConnectionDeviceWANPPPConnectionPortMapping_InternalClient(char *value)
{
	return setPortMappingInternalClient(value);
}
TRX_STATUS getWANDeviceWANConnectionDeviceWANPPPConnectionPortMapping_InternalClient(char **value)
{
	return getPortMappingInternalClient(value);
}
TRX_STATUS setWANDeviceWANConnectionDeviceWANPPPConnectionPortMapping_PortMappingDescription(char *value)
{
	return setPortMappingDescription(value);
}
TRX_STATUS getWANDeviceWANConnectionDeviceWANPPPConnectionPortMapping_PortMappingDescription(char **value)
{
	return getPortMappingDescription(value);
}

/** */
/** InternetGatewayDevice.WANDevice.WANConnectionDevice.WANPPPConnection.PortMapping. */

TRX_STATUS addWANDeviceWANConnectionDeviceWANPPPConnectionPortMappingI(char **value)
{
#if 0
	InstanceDesc *idp;

	if ( (idp = getNewInstanceDesc(getCurrentNode(), getCurrentInstanceDesc(), 0)) ) {
		uint32 index = 0;
		if ( BcmCfm_objCreate(BCMCFM_OBJ_SEC_VIRTUAL_SERVER,
							  &index) == BcmCfm_Ok ) {
			/* now set it with something so it doesn't go away */
			void *info;
			BcmCfm_SecVirtualServerCfg_t *pObj;
			if ( BcmCfm_objGet(BCMCFM_OBJ_SEC_VIRTUAL_SERVER, 
							   &info, &index) == BcmCfm_Ok ) {
				pObj = (BcmCfm_SecVirtualServerCfg_t *)info;
				pObj->status = BcmCfm_CfgDisabled;
				pObj->externalPortStart = 65535;
				pObj->externalPortEnd = 65535;
				pObj->internalPortStart = 65535;
				pObj->internalPortEnd = 65535;
				BcmCfm_objSet(BCMCFM_OBJ_SEC_VIRTUAL_SERVER, 
							  info, index);
				idp->instanceID = index;
				idp->hwUserData = NULL;
				*value = strdup(itoa(idp->instanceID));
				return TRX_OK; /* or TRX_BOOT if necessary */
			}
		}
	}
#endif
	BSTD_UNUSED(value);
	return TRX_ERR;
}
TRX_STATUS delWANDeviceWANConnectionDeviceWANPPPConnectionPortMappingI(char *value)
{
	TRxObjNode *n;
	InstanceDesc *idp;
	int     id = atoi(value);

	if ( (idp = findInstanceDesc(n=getCurrentNode(), id)) ) {
		uint32 index = idp->instanceID;
		if ( BcmCfm_objDelete(BCMCFM_OBJ_SEC_VIRTUAL_SERVER,
							  index) == BcmCfm_Ok ) {
			if ( !deleteInstanceDesc(n, id) )
				return TRX_OK; /* or TRX_BOOT if necessary */
		}
	}

	return TRX_ERR;
}

/** */
/** InternetGatewayDevice.WANDevice.WANConnectionDevice.WANPPPConnection.Stats. */
TRX_STATUS getWANDeviceWANConnectionDeviceWANPPPConnectionStats(char **value, BcmCfm_Stats statsType)
{
	return (getWANDeviceWANConnectionDeviceWANIPConnectionStats(value, statsType));
}
TRX_STATUS getWANDeviceWANConnectionDeviceWANPPPConnectionStats_EthernetBytesSent(char **value)
{
	return (getWANDeviceWANConnectionDeviceWANPPPConnectionStats(value, BcmCfm_StatsTxBytes));
}
TRX_STATUS getWANDeviceWANConnectionDeviceWANPPPConnectionStats_EthernetBytesReceived(char **value)
{
	return (getWANDeviceWANConnectionDeviceWANPPPConnectionStats(value, BcmCfm_StatsRxBytes));
}
TRX_STATUS getWANDeviceWANConnectionDeviceWANPPPConnectionStats_EthernetPacketsSent(char **value)
{
	return (getWANDeviceWANConnectionDeviceWANPPPConnectionStats(value, BcmCfm_StatsTxPkts));
}
TRX_STATUS getWANDeviceWANConnectionDeviceWANPPPConnectionStats_EthernetPacketsReceived(char **value)
{
	return (getWANDeviceWANConnectionDeviceWANPPPConnectionStats(value, BcmCfm_StatsRxPkts));
}

/** */
/** InternetGatewayDevice.WANDevice.WANConnectionDevice.WANPPPConnection. */
/* InternetGatewayDevice.WANDevice.{i}.WANConnectionDevice.{i}.WANPPPConnection.{i}. */

TRX_STATUS setPPPEnable(char *value)
{
#if 0
	InstanceDesc *idp;

	if ( (idp = getCurrentInstanceDesc()) ) {
		PBcmCfm_NtwkIntfCfg_t pObj = NULL;
		uint32 index = idp->instanceID;
		if ( BcmCfm_objGet(BCMCFM_OBJ_NTWK_INTF, 
						   &(idp->hwUserData), &index) == BcmCfm_Ok ) {
			pObj = (PBcmCfm_NtwkIntfCfg_t)idp->hwUserData;
			if ( pObj == NULL )	{
				BcmCfm_objFree(BCMCFM_OBJ_NTWK_INTF, pObj);
				return TRX_ERR;
			}
			if ( strcmp(value, "false") == 0 )
				pObj->status = BcmCfm_CfgDisabled;
			else
				pObj->status = BcmCfm_CfgEnabled;
			if ( BcmCfm_objSet(BCMCFM_OBJ_NTWK_INTF, 
							   idp->hwUserData, index) == BcmCfm_Ok )
				return TRX_REBOOT;
		}
	}
#endif
	BSTD_UNUSED(value);
	return TRX_ERR;
}
TRX_STATUS getPPPEnable(char **value)
{
#if 0
	InstanceDesc *idp;

	if ( (idp = getCurrentInstanceDesc()) ) {
		PBcmCfm_NtwkIntfCfg_t pObj = NULL;
		uint32 index = idp->instanceID;
		if ( BcmCfm_objGet(BCMCFM_OBJ_NTWK_INTF, 
						   &(idp->hwUserData), &index) == BcmCfm_Ok ) {
			pObj = (PBcmCfm_NtwkIntfCfg_t)idp->hwUserData;
			if ( pObj == NULL )	{
				BcmCfm_objFree(BCMCFM_OBJ_NTWK_INTF, pObj);
				return TRX_ERR;
			}
			if ( pObj->status == BcmCfm_CfgEnabled )
				*value = strdup("true");
			else
				*value = strdup("false");
			BcmCfm_objFree(BCMCFM_OBJ_NTWK_INTF, idp->hwUserData);
			return TRX_OK;
		}
	}
#endif
	BSTD_UNUSED(value);
	return TRX_ERR;
}
TRX_STATUS getPPPStatus(char **value)
{
#if 0
	InstanceDesc *idp;
	void	*obj;

	if ( (idp = getCurrentInstanceDesc()) ) {
		PBcmCfm_NtwkIntfSts_t pObj = NULL;
		uint32 index = idp->instanceID;	   
		if ( BcmCfm_stsGet(BCMCFM_OBJ_NTWK_INTF, &obj, &index) == BcmCfm_Ok ) {
			pObj = (PBcmCfm_NtwkIntfSts_t)obj;	  
			if ( pObj == NULL )	{
				BcmCfm_objFree(BCMCFM_OBJ_NTWK_INTF, pObj);
				return TRX_ERR;
			}
			switch(pObj->proto.ppp.state) {
			case BcmCfm_PPPStateUp:
				*value = strdup("Connected");
				break;
			case BcmCfm_PPPStateConnecting:
				*value = strdup("Connecting");
				break;
			case BcmCfm_PPPStateAuthFail:
				*value = strdup("Authenticating"); /* forever, in this case  */
				break;
			case BcmCfm_PPPStateUnknown:
			case BcmCfm_PPPStateDown:
			default:
				*value = strdup("Disconnected");
				break;
			}
			BcmCfm_stsFree(BCMCFM_OBJ_NTWK_INTF, obj);
			return TRX_OK;
		} else {
			*value = strdup("Unconfigured");
			return TRX_OK;
		}
	}
#endif
	BSTD_UNUSED(value);
	return TRX_ERR;
}

TRX_STATUS setPPPType(char *value)
{
	/* only supported "IP_Routed" type for PPPoE and PPPoA */
	/* see chapter 2 in provisioning_option_3.doc from Pirelli */
	BSTD_UNUSED(value);
	return TRX_OK;
}
TRX_STATUS getPPPType(char **value)
{
	/* only supported "IP_Routed" type for PPPoE and PPPoA*/
	/* see chapter 2 in provisioning_option_3.doc from Pirelli */
	*value = strdup("IP_Routed");
	return TRX_OK;
}
TRX_STATUS setPPPName(char *value)
{
#if 0
	InstanceDesc *idp;

	if ( (idp = getCurrentInstanceDesc()) ) {
		PBcmCfm_NtwkIntfCfg_t pObj = NULL;
		uint32 index = idp->instanceID;
		if ( BcmCfm_objGet(BCMCFM_OBJ_NTWK_INTF, 
						   &(idp->hwUserData), &index) == BcmCfm_Ok ) {
			pObj = (PBcmCfm_NtwkIntfCfg_t)idp->hwUserData;
			if ( pObj == NULL )	{
				BcmCfm_objFree(BCMCFM_OBJ_NTWK_INTF, pObj);
				return TRX_ERR;
			}
			BcmCfm_free((void *)pObj->uName);
			BcmCfm_alloc(strlen(value) + 1, (void **)&(pObj->uName));
			strcpy(pObj->uName, value);
			if ( BcmCfm_objSet(BCMCFM_OBJ_NTWK_INTF, 
							   idp->hwUserData, index) == BcmCfm_Ok )
				return TRX_REBOOT;
		}
	}
#endif
	BSTD_UNUSED(value);
	return TRX_ERR;
}
TRX_STATUS getPPPName(char **value)
{
#if 0
	InstanceDesc *idp;

	if ( (idp = getCurrentInstanceDesc()) ) {
		PBcmCfm_NtwkIntfCfg_t pObj = NULL;
		uint32 index = idp->instanceID;
		if ( BcmCfm_objGet(BCMCFM_OBJ_NTWK_INTF, 
						   &(idp->hwUserData), &index) == BcmCfm_Ok ) {
			pObj = (PBcmCfm_NtwkIntfCfg_t)idp->hwUserData;
			if ( pObj == NULL )	{
				BcmCfm_objFree(BCMCFM_OBJ_NTWK_INTF, pObj);
				return TRX_ERR;
			}
			*value = strdup(pObj->uName);
			BcmCfm_objFree(BCMCFM_OBJ_NTWK_INTF, idp->hwUserData);
			return TRX_OK;
		}
	}
#endif
	BSTD_UNUSED(value);
	return TRX_ERR;
}

TRX_STATUS pseudoGetIFName(char **value)
{
	BSTD_UNUSED(value);
	return TRX_ERR;

}

TRX_STATUS setPPPIdleTime(char *value)
{
#if 0
	InstanceDesc *idp;

	if ( (idp = getCurrentInstanceDesc()) ) {
		PBcmCfm_NtwkIntfCfg_t pObj = NULL;
		uint32 index = idp->instanceID;
		if ( BcmCfm_objGet(BCMCFM_OBJ_NTWK_INTF, 
						   &(idp->hwUserData), &index) == BcmCfm_Ok ) {
			pObj = (PBcmCfm_NtwkIntfCfg_t)idp->hwUserData;
			if ( pObj == NULL )	{
				BcmCfm_objFree(BCMCFM_OBJ_NTWK_INTF, pObj);
				return TRX_ERR;
			}
			pObj->proto.ppp.idleTimeout = atoi(value);
			if ( BcmCfm_objSet(BCMCFM_OBJ_NTWK_INTF, 
							   idp->hwUserData, index) == BcmCfm_Ok )
				return TRX_REBOOT;
		}
	}
#endif
	BSTD_UNUSED(value);
	return TRX_ERR;
}
TRX_STATUS getPPPIdleTime(char **value)
{
#if 0
	InstanceDesc *idp;

	if ( (idp = getCurrentInstanceDesc()) ) {
		char  idleTime[IFC_TINY_LEN];
		PBcmCfm_NtwkIntfCfg_t pObj = NULL;
		uint32 index = idp->instanceID;
		if ( BcmCfm_objGet(BCMCFM_OBJ_NTWK_INTF, 
						   &(idp->hwUserData), &index) == BcmCfm_Ok ) {
			pObj = (PBcmCfm_NtwkIntfCfg_t)idp->hwUserData;
			if ( pObj == NULL )	{
				BcmCfm_objFree(BCMCFM_OBJ_NTWK_INTF, pObj);
				return TRX_ERR;
			}
			sprintf(idleTime, "%lu", pObj->proto.ppp.idleTimeout);
			*value = strdup(idleTime);
			BcmCfm_objFree(BCMCFM_OBJ_NTWK_INTF, idp->hwUserData);
			return TRX_OK;
		}
	}
#endif
	BSTD_UNUSED(value);
	return TRX_ERR;
}
TRX_STATUS setPPPNATEnabled(char *value)
{
#if 0
	InstanceDesc *idp;

	if ( (idp = getCurrentInstanceDesc()) ) {
		PBcmCfm_NtwkIntfCfg_t pObj = NULL;
		uint32 index = idp->instanceID;
		if ( BcmCfm_objGet(BCMCFM_OBJ_NTWK_INTF, 
						   &(idp->hwUserData), &index) == BcmCfm_Ok ) {
			pObj = (PBcmCfm_NtwkIntfCfg_t)idp->hwUserData;
			if ( pObj == NULL )	{
				BcmCfm_objFree(BCMCFM_OBJ_NTWK_INTF, pObj);
				return TRX_ERR;
			}
			pObj->natEnable = streq(value, "1")? 1: 0;
			if ( BcmCfm_objSet(BCMCFM_OBJ_NTWK_INTF, 
							   idp->hwUserData, index) == BcmCfm_Ok )
				return TRX_REBOOT;
		}
	}
#endif
	BSTD_UNUSED(value);
	return TRX_ERR;
}
TRX_STATUS getPPPNATEnabled(char **value)
{
#if 0
	InstanceDesc *idp;

	if ( (idp = getCurrentInstanceDesc()) ) {
		char  natEnabled[IFC_TINY_LEN];
		PBcmCfm_NtwkIntfCfg_t pObj = NULL;
		uint32 index = idp->instanceID;
		if ( BcmCfm_objGet(BCMCFM_OBJ_NTWK_INTF, 
						   &(idp->hwUserData), &index) == BcmCfm_Ok ) {
			pObj = (PBcmCfm_NtwkIntfCfg_t)idp->hwUserData;
			if ( pObj == NULL )	{
				BcmCfm_objFree(BCMCFM_OBJ_NTWK_INTF, idp->hwUserData);
				return TRX_ERR;
			}
			sprintf(natEnabled, "%lu", pObj->natEnable);
			*value = strdup(natEnabled);
			BcmCfm_objFree(BCMCFM_OBJ_NTWK_INTF, idp->hwUserData);
			return TRX_OK;
		}
	}
#endif
	BSTD_UNUSED(value);
	return TRX_ERR;
}
TRX_STATUS setPPPUsername(char *value)
{
#if 0
	InstanceDesc *idp;

	if ( (idp = getCurrentInstanceDesc()) ) {
		PBcmCfm_NtwkIntfCfg_t pObj = NULL;
		uint32 index = idp->instanceID;
		if ( BcmCfm_objGet(BCMCFM_OBJ_NTWK_INTF, 
						   &(idp->hwUserData), &index) == BcmCfm_Ok ) {
			pObj = (PBcmCfm_NtwkIntfCfg_t)idp->hwUserData;
			if ( pObj == NULL )	{
				BcmCfm_objFree(BCMCFM_OBJ_NTWK_INTF, pObj);
				return TRX_ERR;
			}
			BcmCfm_free((void *)pObj->proto.ppp.uName);
			BcmCfm_alloc(strlen(value), (void **)&(pObj->proto.ppp.uName));
			strcpy(pObj->proto.ppp.uName, value);
			if ( BcmCfm_objSet(BCMCFM_OBJ_NTWK_INTF, 
							   idp->hwUserData, index) == BcmCfm_Ok )
				return TRX_REBOOT;
		}
	}
#endif
	BSTD_UNUSED(value);
	return TRX_ERR;
}
TRX_STATUS getPPPUsername(char **value)
{
#if 0
	InstanceDesc *idp;

	if ( (idp = getCurrentInstanceDesc()) ) {
		PBcmCfm_NtwkIntfCfg_t pObj = NULL;
		uint32 index = idp->instanceID;
		if ( BcmCfm_objGet(BCMCFM_OBJ_NTWK_INTF, 
						   &(idp->hwUserData), &index) == BcmCfm_Ok ) {
			pObj = (PBcmCfm_NtwkIntfCfg_t)idp->hwUserData;
			if ( pObj == NULL )	{
				BcmCfm_objFree(BCMCFM_OBJ_NTWK_INTF, pObj);
				return TRX_ERR;
			}
			*value = strdup(pObj->proto.ppp.uName);
			BcmCfm_objFree(BCMCFM_OBJ_NTWK_INTF, idp->hwUserData);
			return TRX_OK;
		}
	}
#endif
	BSTD_UNUSED(value);
	return TRX_ERR;
}
TRX_STATUS setPPPPassword(char *value)
{
#if 0
	InstanceDesc *idp;

	if ( (idp = getCurrentInstanceDesc()) ) {
		PBcmCfm_NtwkIntfCfg_t pObj = NULL;
		uint32 index = idp->instanceID;
		if ( BcmCfm_objGet(BCMCFM_OBJ_NTWK_INTF, 
						   &(idp->hwUserData), &index) == BcmCfm_Ok ) {
			pObj = (PBcmCfm_NtwkIntfCfg_t)idp->hwUserData;
			if ( pObj == NULL )	{
				BcmCfm_objFree(BCMCFM_OBJ_NTWK_INTF, pObj);
				return TRX_ERR;
			}
			BcmCfm_free((void *)pObj->proto.ppp.passwd);
			BcmCfm_alloc(strlen(value), (void **)&(pObj->proto.ppp.passwd));
			strcpy(pObj->proto.ppp.passwd, value);
			if ( BcmCfm_objSet(BCMCFM_OBJ_NTWK_INTF, 
							   idp->hwUserData, index) == BcmCfm_Ok )
				return TRX_REBOOT;
		}
	}
#endif
	BSTD_UNUSED(value);
	return TRX_ERR;
}
TRX_STATUS getPPPPassword(char **value)
{
#if 0
	InstanceDesc *idp;

	if ( (idp = getCurrentInstanceDesc()) ) {
		PBcmCfm_NtwkIntfCfg_t pObj = NULL;
		uint32 index = idp->instanceID;
		if ( BcmCfm_objGet(BCMCFM_OBJ_NTWK_INTF, 
						   &(idp->hwUserData), &index) == BcmCfm_Ok ) {
			pObj = (PBcmCfm_NtwkIntfCfg_t)idp->hwUserData;
			if ( pObj == NULL )	{
				BcmCfm_objFree(BCMCFM_OBJ_NTWK_INTF, pObj);
				return TRX_ERR;
			}
			*value = strdup(pObj->proto.ppp.passwd);
			BcmCfm_objFree(BCMCFM_OBJ_NTWK_INTF, idp->hwUserData);
			return TRX_OK;
		}
	}
#endif
	BSTD_UNUSED(value);
	return TRX_ERR;
}
TRX_STATUS setWanPPPAddressingType(char *value)
{
#if 0
	InstanceDesc *idp;

	if ( (idp = getCurrentInstanceDesc()) ) {
		PBcmCfm_NtwkIntfCfg_t pObj = NULL;
		uint32 index = idp->instanceID;
		if ( BcmCfm_objGet(BCMCFM_OBJ_NTWK_INTF, 
						   &(idp->hwUserData), &index) == BcmCfm_Ok ) {
			pObj = (PBcmCfm_NtwkIntfCfg_t)idp->hwUserData;
			if ( pObj == NULL )	{
				BcmCfm_objFree(BCMCFM_OBJ_NTWK_INTF, pObj);
				return TRX_ERR;
			}
			pObj->proto.ppp.staticIpEnable = strcasecmp(value,"Static")==0?
											BcmCfm_CfgEnabled: BcmCfm_CfgDisabled;
			if ( BcmCfm_objSet(BCMCFM_OBJ_NTWK_INTF, 
							   idp->hwUserData, index) == BcmCfm_Ok )
				return TRX_REBOOT;
		}
	}
#endif
	BSTD_UNUSED(value);
	return TRX_ERR;
}
TRX_STATUS getWanPPPAddressingType(char **value)
{
#if 0
	InstanceDesc *idp;

	if ( (idp = getCurrentInstanceDesc()) ) {
		PBcmCfm_NtwkIntfCfg_t pObj = NULL;
		uint32 index = idp->instanceID;
		if ( BcmCfm_objGet(BCMCFM_OBJ_NTWK_INTF, 
						   &(idp->hwUserData), &index) == BcmCfm_Ok ) {
			pObj = (PBcmCfm_NtwkIntfCfg_t)idp->hwUserData;
			if ( pObj == NULL )	{
				BcmCfm_objFree(BCMCFM_OBJ_NTWK_INTF, pObj);
				return TRX_ERR;
			}
			*value = strdup(pObj->proto.ppp.staticIpEnable==BcmCfm_CfgEnabled?
							 "Static": "DHCP");
			BcmCfm_objFree(BCMCFM_OBJ_NTWK_INTF, idp->hwUserData);
			return TRX_OK;
		}
	}
#endif
	BSTD_UNUSED(value);
	return TRX_ERR;
}
TRX_STATUS setWanPPPExtIP(char *value)
{
#if 0
	InstanceDesc *idp;

	if ( (idp = getCurrentInstanceDesc()) ) {
		PBcmCfm_NtwkIntfCfg_t pObj = NULL;
		uint32 index = idp->instanceID;
		if ( BcmCfm_objGet(BCMCFM_OBJ_NTWK_INTF, 
						   &(idp->hwUserData), &index) == BcmCfm_Ok ) {
			pObj = (PBcmCfm_NtwkIntfCfg_t)idp->hwUserData;
			if ( pObj == NULL )	{
				BcmCfm_objFree(BCMCFM_OBJ_NTWK_INTF, pObj);
				return TRX_ERR;
			}
			pObj->proto.ppp.ipAddress = readIp(value);
			if ( BcmCfm_objSet(BCMCFM_OBJ_NTWK_INTF, 
							   idp->hwUserData, index) == BcmCfm_Ok )
				return TRX_REBOOT;
		}
	}
#endif
	BSTD_UNUSED(value);
	return TRX_ERR;
}
TRX_STATUS getWanPPPExtIP(char **value)
{
#if 0
	InstanceDesc *idp;

	if ( (idp = getCurrentInstanceDesc()) ) {
		PBcmCfm_NtwkIntfCfg_t pObj = NULL;
		uint32 index = idp->instanceID;
		if ( BcmCfm_objGet(BCMCFM_OBJ_NTWK_INTF, 
						   &(idp->hwUserData), &index) == BcmCfm_Ok ) {
			pObj = (PBcmCfm_NtwkIntfCfg_t)idp->hwUserData;
			if ( pObj == NULL )	{
				BcmCfm_objFree(BCMCFM_OBJ_NTWK_INTF, pObj);
				return TRX_ERR;
			}
			*value = strdup(writeIp(pObj->proto.ppp.ipAddress));
			BcmCfm_objFree(BCMCFM_OBJ_NTWK_INTF, idp->hwUserData);
			return TRX_OK;
		}
	}
#endif
	BSTD_UNUSED(value);
	return TRX_ERR;
}
TRX_STATUS setPPPDNSServers(char *value)
{
	return tr69c_setDNSServers(value);
}
TRX_STATUS getPPPDNSServers(char **value)
{
	return tr69c_getDNSServers(value);
}
TRX_STATUS setPPPConTrigger(char *value)
{
#if 0
	InstanceDesc *idp;

	if ( (idp = getCurrentInstanceDesc()) ) {
		PBcmCfm_NtwkIntfCfg_t pObj = NULL;
		uint32 index = idp->instanceID;
		if ( BcmCfm_objGet(BCMCFM_OBJ_NTWK_INTF, 
						   &(idp->hwUserData), &index) == BcmCfm_Ok ) {
			pObj = (PBcmCfm_NtwkIntfCfg_t)idp->hwUserData;
			if ( pObj == NULL )	{
				BcmCfm_objFree(BCMCFM_OBJ_NTWK_INTF, pObj);
				return TRX_ERR;
			}
			if ( strcmp(value, "OnDemand") == 0 )
				pObj->proto.ppp.onDemandEnable = BcmCfm_CfgEnabled;
			else
				pObj->proto.ppp.onDemandEnable = BcmCfm_CfgDisabled;
			if ( BcmCfm_objSet(BCMCFM_OBJ_NTWK_INTF, 
							   idp->hwUserData, index) == BcmCfm_Ok )
				return TRX_REBOOT;
		}
	}
#endif
	BSTD_UNUSED(value);
	return TRX_ERR;
}
TRX_STATUS getPPPConTrigger(char **value)
{
#if 0
	InstanceDesc *idp;

	if ( (idp = getCurrentInstanceDesc()) ) {
		PBcmCfm_NtwkIntfCfg_t pObj = NULL;
		uint32 index = idp->instanceID;
		if ( BcmCfm_objGet(BCMCFM_OBJ_NTWK_INTF, 
						   &(idp->hwUserData), &index) == BcmCfm_Ok ) {
			pObj = (PBcmCfm_NtwkIntfCfg_t)idp->hwUserData;
			if ( pObj == NULL )	{
				BcmCfm_objFree(BCMCFM_OBJ_NTWK_INTF, pObj);
				return TRX_ERR;
			}
			if ( pObj->proto.ppp.onDemandEnable == BcmCfm_CfgEnabled )
				*value = strdup("OnDemand");
			else
				*value = strdup("AlwaysOn");
			BcmCfm_objFree(BCMCFM_OBJ_NTWK_INTF, idp->hwUserData);
			return TRX_OK;
		}
	}
#endif
	BSTD_UNUSED(value);
	return TRX_ERR;
}
TRX_STATUS getWANDeviceWANConnectionDeviceWANPPPConnection_ConnectionStatus(char **value)
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
TRX_STATUS getWANDeviceWANConnectionDeviceWANPPPConnection_PossibleConnectionTypes(char **value)
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
TRX_STATUS getWANDeviceWANConnectionDeviceWANPPPConnection_Uptime(char **value)
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
TRX_STATUS getWANDeviceWANConnectionDeviceWANPPPConnection_LastConnectionError(char **value)
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
TRX_STATUS setWANDeviceWANConnectionDeviceWANPPPConnection_AutoDisconnectTime(char *value)
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
TRX_STATUS getWANDeviceWANConnectionDeviceWANPPPConnection_AutoDisconnectTime(char **value)
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
TRX_STATUS setWANDeviceWANConnectionDeviceWANPPPConnection_WarnDisconnectDelay(char *value)
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
TRX_STATUS getWANDeviceWANConnectionDeviceWANPPPConnection_WarnDisconnectDelay(char **value)
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
TRX_STATUS getWANDeviceWANConnectionDeviceWANPPPConnection_RSIPAvailable(char **value)
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
TRX_STATUS setWANDeviceWANConnectionDeviceWANPPPConnection_NATEnabled(char *value)
{
	return(setPPPNATEnabled(value));
}
TRX_STATUS getWANDeviceWANConnectionDeviceWANPPPConnection_NATEnabled(char **value)
{
	return(getPPPNATEnabled(value));
}
TRX_STATUS setWANDeviceWANConnectionDeviceWANPPPConnection_Username(char *value)
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
TRX_STATUS getWANDeviceWANConnectionDeviceWANPPPConnection_PPPEncryptionProtocol(char **value)
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
TRX_STATUS getWANDeviceWANConnectionDeviceWANPPPConnection_PPPCompressionProtocol(char **value)
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
TRX_STATUS getWANDeviceWANConnectionDeviceWANPPPConnection_PPPAuthenticationProtocol(char **value)
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
TRX_STATUS getWANDeviceWANConnectionDeviceWANPPPConnection_RemoteIPAddress(char **value)
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
TRX_STATUS setWANDeviceWANConnectionDeviceWANPPPConnection_MaxMRUSize(char *value)
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
TRX_STATUS getWANDeviceWANConnectionDeviceWANPPPConnection_MaxMRUSize(char **value)
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
TRX_STATUS getWANDeviceWANConnectionDeviceWANPPPConnection_CurrentMRUSize(char **value)
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
TRX_STATUS setWANDeviceWANConnectionDeviceWANPPPConnection_DNSEnabled(char *value)
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
TRX_STATUS getWANDeviceWANConnectionDeviceWANPPPConnection_DNSEnabled(char **value)
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
TRX_STATUS setWANDeviceWANConnectionDeviceWANPPPConnection_DNSOverrideAllowed(char *value)
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
TRX_STATUS getWANDeviceWANConnectionDeviceWANPPPConnection_DNSOverrideAllowed(char **value)
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
TRX_STATUS setWANDeviceWANConnectionDeviceWANPPPConnection_MACAddress(char *value)
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
TRX_STATUS getWANDeviceWANConnectionDeviceWANPPPConnection_MACAddress(char **value)
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
TRX_STATUS setWANDeviceWANConnectionDeviceWANPPPConnection_MACAddressOverride(char *value)
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
TRX_STATUS getWANDeviceWANConnectionDeviceWANPPPConnection_MACAddressOverride(char **value)
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
TRX_STATUS getWANDeviceWANConnectionDeviceWANPPPConnection_TransportType(char **value)
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
TRX_STATUS setWANDeviceWANConnectionDeviceWANPPPConnection_PPPoEACName(char *value)
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
TRX_STATUS getWANDeviceWANConnectionDeviceWANPPPConnection_PPPoEACName(char **value)
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
TRX_STATUS setWANDeviceWANConnectionDeviceWANPPPConnection_PPPoEServiceName(char *value)
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
TRX_STATUS getWANDeviceWANConnectionDeviceWANPPPConnection_PPPoEServiceName(char **value)
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
TRX_STATUS setWANDeviceWANConnectionDeviceWANPPPConnection_RouteProtocolRx(char *value)
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
TRX_STATUS getWANDeviceWANConnectionDeviceWANPPPConnection_RouteProtocolRx(char **value)
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
TRX_STATUS getWANDeviceWANConnectionDeviceWANPPPConnection_PPPLCPEcho(char **value)
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
TRX_STATUS getWANDeviceWANConnectionDeviceWANPPPConnection_PPPLCPEchoRetry(char **value)
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

extern TRxObjNode  WANDeviceWANConnectionDeviceWANPPPConnectionInstanceDesc[];
extern TRxObjNode  WANDeviceWANConnectionDeviceWANPPPConnectionPortMappingInstanceDesc[];

/* called before framework is initialized: can't use getCurrentNode, etc. */
int initWANPPPConnectionPortMappingInstance(InstanceDesc *pppParentIdp) {
	TRxObjNode *pmNode;
	int cnt = 0;
	/* InternetGatewayDevice.WANDevice.{i}.WANConnectionDevice.{i}.WANPPPConnection.PortMapping.{i}.*/
	pmNode = WANDeviceWANConnectionDeviceWANPPPConnectionPortMappingInstanceDesc;
	cnt = initWANPortMappingInstances(pmNode,pppParentIdp);
	return cnt;
}

TRX_STATUS getWANDeviceWANConnectionDeviceWANPPPConnection_PortMappingNumberOfEntries(char **value)
{
	int cnt = getInstanceCount(WANDeviceWANConnectionDeviceWANPPPConnectionPortMappingInstanceDesc);
	*value = strdup( itoa(cnt));
	return TRX_OK;
}

TRX_STATUS addPPPConnection(char **value)
{
#if 0
	InstanceDesc *idp;
	InstanceDesc *connDevI = getCurrentInstanceDesc(); /* get instance of ConnectionDevice */
	if ( (idp = getNewInstanceDesc(getCurrentNode(), connDevI, 0)) ) {
		uint32 index = 0;
		void *objValue;
		PBcmCfm_NtwkIntfCfg_t pObj = NULL;
		if ( BcmCfm_objCreate(BCMCFM_OBJ_NTWK_INTF,
							  &index) == BcmCfm_Ok ) {
			idp->instanceID = index;
			idp->hwUserData = NULL;
			*value = strdup(itoa(idp->instanceID));
			BcmCfm_objGet(BCMCFM_OBJ_NTWK_INTF, &objValue, &index);
			pObj = (PBcmCfm_NtwkIntfCfg_t)objValue;
			if ( pObj != NULL ) {
				char *linktype = NULL;

				// The protocol for a PPP connection can only be PPPoE or PPPoA.
				getDSLLinkType(&linktype);
				pObj->protocol = BcmCfm_NtwkIntfProtoPPPoE;
				if( linktype ) {
					if( strcasecmp(linktype, "PPPoA") == 0 )
						pObj->protocol = BcmCfm_NtwkIntfProtoPPPoA;

					free(linktype);
				}

   				pObj->attachToObjId = BCMCFM_OBJ_IFC_ATM_VCC;
				if( connDevI )
					pObj->attachToIndex = connDevI->instanceID;
				BcmCfm_objSet(BCMCFM_OBJ_NTWK_INTF, objValue, index);
				return TRX_OK; 
			}
		}
	}
#endif
	BSTD_UNUSED(value);
	return TRX_ERR;
}
TRX_STATUS deletePPPConnection(char *value)
{
	TRxObjNode *n;
	InstanceDesc *idp;
	int     id = atoi(value);

	if ( (idp = findInstanceDesc(n=getCurrentNode(), id)) ) {
		uint32 index = idp->instanceID;
		if ( BcmCfm_objDelete(BCMCFM_OBJ_NTWK_INTF,
							  index) == BcmCfm_Ok ) {
			if ( !deleteInstanceDesc(n, id) )
				return TRX_REBOOT; /* or TRX_BOOT if necessary */
		}
	}

	BSTD_UNUSED(value);
	return TRX_ERR;
}

/* InternetGatewayDevice.WANDevice.{i}.WANConnectionDevice.{i}. */

TRX_STATUS addWANConnectionDevice(char **value)
{
#if 0
	InstanceDesc *idp;

	if ( (idp = getNewInstanceDesc(getCurrentNode(), getCurrentInstanceDesc(), 0)) ) {
		uint32 index = 0;
		if ( BcmCfm_objCreate(BCMCFM_OBJ_IFC_ATM_VCC,
							  &index) == BcmCfm_Ok ) {
			PBcmCfm_AtmVccCfg_t pObj;

			idp->instanceID = index;
			idp->hwUserData = NULL;
			*value = strdup(itoa(idp->instanceID));

			if(BcmCfm_objGet(BCMCFM_OBJ_IFC_ATM_VCC, (void **) &pObj,
				&index) == BcmCfm_Ok) {

				/* Set default values. */
				pObj->status = BcmCfm_CfgDisabled;
				pObj->vpi = 255;
				pObj->vci = 200 + index;
				pObj->atmCategory = BcmCfm_ATMCatUBR;
				pObj->aalType = BcmCfm_ATMAal5;
				pObj->encapMode = BcmCfm_ATMEncapUnknown;

				BcmCfm_objSet(BCMCFM_OBJ_IFC_ATM_VCC, (void *) pObj, index);
			}

			return TRX_OK; /* or TRX_BOOT if necessary */
		}
	}
#endif
	BSTD_UNUSED(value);
	return TRX_ERR;
}
/** */
/* Delete all WAN connections that use this WAN device. */
void deleteWANConnections(int instanceID)
{
#if 0
	uint32 index = 0;
	PBcmCfm_NtwkIntfCfg_t pObj;
	while(BcmCfm_objGet(BCMCFM_OBJ_NTWK_INTF,(void **)&pObj,&index)==BcmCfm_Ok) {
		if( pObj->attachToIndex == (uint32) instanceID )
			BcmCfm_objDelete(BCMCFM_OBJ_NTWK_INTF, index);
		BcmCfm_objFree(BCMCFM_OBJ_NTWK_INTF, pObj);
		index++;
	}
#endif
	BSTD_UNUSED(instanceID);
}

TRX_STATUS deleteWANConnectionDevice(char *value)
{
#if 0
	TRxObjNode *n;
	InstanceDesc *idp;
	int     id = atoi(value);

	if ( (idp = findInstanceDesc(n=getCurrentNode(), id)) ) {
		uint32 index = idp->instanceID;
		deleteWANConnections(index);
		if ( BcmCfm_objDelete(BCMCFM_OBJ_IFC_ATM_VCC,
							  index) == BcmCfm_Ok ) {
			if ( !deleteInstanceDesc(n, id) ) {
				char token[16];

				sprintf(token, "LinkType%4.4x", idp->instanceID);
				BcmCfm_scratchPadSet(token, NULL, 0);
				return TRX_REBOOT; /* or TRX_BOOT if necessary */
			}
		}
	}
#endif
	BSTD_UNUSED(value);
	return TRX_ERR;
}

#if 0
// If the network configuration object does not have a 'LinkType' scratch pad
// entry, add it.  This implies that the object was created outside of this
// program by the cfm Web UI or similar.
#endif
void setScratchPadLinkTypeEntry( uint32 index, BcmCfm_NtwkIntfProtocol protocol )
{
#if 0
	char token[16];
	char value[16] = "";

	sprintf(token, "LinkType%4.4x", (uint16) index);
	switch( protocol ) {
	case BcmCfm_NtwkIntfProtoMAC:
	case BcmCfm_NtwkIntfProtoPPPoE:
		strcpy(value, "EoA");
		break;
	case BcmCfm_NtwkIntfProtoPPPoA:
		strcpy(value, "PPPoA");
		break;
	case BcmCfm_NtwkIntfProtoIPoA:
		strcpy(value, "IPoA");
		break;
	case BcmCfm_NtwkIntfProtoIPoWAN:
		strcpy(value, "IPoW");
		break;
	default:
		break;
	}

	if( value[0] != '\0' )
		BcmCfm_scratchPadSet(token, value, strlen(value) + 1);
#endif
	BSTD_UNUSED(index);
	BSTD_UNUSED(protocol);
}

void checkForObjScratchPadLinkTypeEntry( uint32 index )
{
#if 0
	char token[16];
	char *linktype;
	uint32 len;

	sprintf(token, "LinkType%4.4x", (uint16) index);
	if( BcmCfm_scratchPadGet(token, (void **) &linktype,  &len) == BcmCfm_Ok )
		BcmCfm_free(linktype);
	else
		BcmCfm_scratchPadSet(token, "EoA", strlen("EoA") + 1);
#endif
	BSTD_UNUSED(index);
}

/* Enumerates through the scratch pad looking for 'LinkType' entries that are no
 * longer used.
 */
void checkForUnsuedScratchPadLinkTypeEntry( uint32 highestVccIndex )
{
#if 0
	uint32 i, j;
	char token[16];
	char *linktype;
	uint32 len;
	PBcmCfm_AtmVccCfg_t pObj;

	for( i = 1; i < highestVccIndex; i++ ) {
		sprintf(token, "LinkType%4.4x", (uint16) i);
		if(BcmCfm_scratchPadGet(token, (void **) &linktype, &len) == BcmCfm_Ok) {
			// There is a scratch pad entry. See if there is a VCC object for it.
			j = i;
			if(BcmCfm_objGet(BCMCFM_OBJ_IFC_ATM_VCC, (void **) &pObj, &j) == BcmCfm_Ok) {
				if( j != i )
					// There is not a VCC object. Remove the scratch pad entry.
					BcmCfm_scratchPadSet(token, NULL, 0);

				BcmCfm_objFree(BCMCFM_OBJ_IFC_ATM_VCC, pObj);
			}
			else {
				// There is not a VCC object. Remove the scratch pad entry.
				BcmCfm_scratchPadSet(token, NULL, 0);
			}

			BcmCfm_free(linktype);
		}
	}
#endif
	BSTD_UNUSED(highestVccIndex);
}

/** */
/** InternetGatewayDevice.WANDevice.WANConnectionDevice. */
extern TRxObjNode  WANDeviceWANConnectionDeviceWANIPConnectionInstanceDesc[];
TRX_STATUS getWANDeviceWANConnectionDevice_WANIPConnectionNumberOfEntries(char **value)
{
	*value = strdup( itoa(getInstanceCount(WANDeviceWANConnectionDeviceWANIPConnectionInstanceDesc)));
	return TRX_OK;
}

extern TRxObjNode  WANDeviceWANConnectionDeviceWANPPPConnectionInstanceDesc[];
TRX_STATUS getPPPConNumberEntries(char **value)
{
	*value = strdup( itoa(getInstanceCount(WANDeviceWANConnectionDeviceWANPPPConnectionInstanceDesc)));
	return TRX_OK;
}

/* InternetGatewayDevice.WANDevice.{i}. */

extern TRxObjNode WANDeviceWANConnectionDeviceInstanceDesc[];
TRX_STATUS getWANDevice_WANConnectionNumberOfEntries(char **value)
{
	int num = getInstanceCount(WANDeviceWANConnectionDeviceInstanceDesc);
	*value = strdup( itoa(num));
	return TRX_OK;
}


extern TRxObjNode   WANDeviceInstanceDesc[];
extern TRxObjNode   WANDeviceWANConnectionDeviceInstanceDesc[];

void initWANDeviceInstances(void)
{
#if 0
	TRxObjNode *n, *pppNode, *connDevNode, *ipNode, *connServ;
	int id;
	InstanceDesc    *wanDeviceIdp;
	uint32 wanObjIndex = 0;
	void *objValue;
	uint32 highestVccIndex = 0;

	/* wanDeviceDesc */
	n = WANDeviceInstanceDesc;
	id = 1;	/* getWanDeviceInstanceId() This value needs to be persistent */
	if( (wanDeviceIdp = findInstanceDescNoPathCheck(n, id)) == NULL ) {
		wanDeviceIdp = getNewInstanceDesc(n, NULL, id);
		wanDeviceIdp->hwUserData = (void *)1; /* link to hw level or system level data */
	}

	/* Create: */
	/* InternetGatewayDevice.WANDevice.{i}.WANConnectionDevice.{i}.*/
	/* Need to create one WANConnectionDevice instance for each PVC */
	/* First determine the PPP network config then create a WANConnectionDevice instance*/
	/* to match it. */
	connDevNode = WANDeviceWANConnectionDeviceInstanceDesc;
	pppNode = WANDeviceWANConnectionDeviceWANPPPConnectionInstanceDesc;				
	ipNode = WANDeviceWANConnectionDeviceWANIPConnectionInstanceDesc;
	connServ = WANDeviceWANDSLConnectionManagementConnectionServiceInstanceDesc;
	wanObjIndex = 0;
	while ( BcmCfm_objGet(BCMCFM_OBJ_NTWK_INTF, 
						  &objValue, &wanObjIndex) == BcmCfm_Ok ) {
		PBcmCfm_NtwkIntfCfg_t pObj = (PBcmCfm_NtwkIntfCfg_t)objValue;
		if ( pObj != NULL && pObj->attachToObjId == BCMCFM_OBJ_IFC_ATM_VCC ) {
			void *vccObjValue;
			DBGPRINT((stderr, "NTWK_INTF wanObjIndex=%d, attachToIndex=%d\n", wanObjIndex, pObj->attachToIndex));
			if ( BcmCfm_objGet(BCMCFM_OBJ_IFC_ATM_VCC, 
							   &vccObjValue, &pObj->attachToIndex ) == BcmCfm_Ok ) {
				InstanceDesc *vccIdp, *wanXXXIdp, *servIdp;
				PBcmCfm_AtmVccCfg_t vcc = (PBcmCfm_AtmVccCfg_t)vccObjValue;
				DBGPRINT((stderr, "IFC_ATM_VCC =%lu/%lu\n", vcc->vpi, vcc->vci));
				// ConnectionService instances use same instance id as ConnectionDevice.{i}.
				// since there is a one-to-one mapping between them.
				if( (vccIdp = findInstanceDescNoPathCheck(connDevNode, pObj->attachToIndex)) == NULL ) {
					// add new instances for ConnectionDevice and ServiceConnection
					vccIdp = getNewInstanceDesc(connDevNode, wanDeviceIdp, pObj->attachToIndex);
					servIdp = getNewInstanceDesc(connServ, wanDeviceIdp, pObj->attachToIndex );
				} else if ( (servIdp = findInstanceDescNoPathCheck(connServ, vccIdp->instanceID))==NULL ) {
					servIdp = getNewInstanceDesc(connServ, wanDeviceIdp, pObj->attachToIndex);

				}
				// need to link ConnectionService instance to the PPP or IP WANXXXConnection
				servIdp->hwUserData = (void *) (vcc->vci<<16|vcc->vpi);

				if ( pObj->protocol == BcmCfm_NtwkIntfProtoPPPoA  
					  || pObj->protocol==BcmCfm_NtwkIntfProtoPPPoE ) {
					if( (wanXXXIdp = findInstanceDescNoPathCheck(pppNode, wanObjIndex)) == NULL ) {
						// Create associated WANPPPConnection instance; existance implied by NTWK_INTF object
						wanXXXIdp = getNewInstanceDesc(pppNode, vccIdp, wanObjIndex);
					}
						// Create associated WANPPPConnectionPortMapping instance if any exist
						initWANPPPConnectionPortMappingInstance(wanXXXIdp);
				} else if ( pObj->protocol == BcmCfm_NtwkIntfProtoIPoA 
					  || pObj->protocol == BcmCfm_NtwkIntfProtoMAC
					  || pObj->protocol == BcmCfm_NtwkIntfProtoIPoWAN ) {
					if( (wanXXXIdp = findInstanceDescNoPathCheck(ipNode, wanObjIndex)) == NULL ) {
						// Create associated WANIPConnection instance
						wanXXXIdp = getNewInstanceDesc(ipNode, vccIdp, wanObjIndex);
					}
						// Create associated WANIPConnectionPortMapping instance if any exist
						initWANIPConnectionPortMappingInstance(wanXXXIdp);	
				} else
					fprintf(stderr, "WANXXXConnection Instance initialization error\n");

				// Free the mem allocated this object by the get API.
				BcmCfm_objFree(BCMCFM_OBJ_IFC_ATM_VCC, (void*)vcc);
			}

			// Verify that this object instance has a 'LinkType' scratch pad entry.
			//checkForObjScratchPadLinkTypeEntry( pObj->attachToIndex, pObj->protocol );
			setScratchPadLinkTypeEntry( pObj->attachToIndex, pObj->protocol );
			if( pObj->attachToIndex > highestVccIndex )
				highestVccIndex = pObj->attachToIndex;
		}
		// Free the mem allocated this object by the get API.
		BcmCfm_objFree(BCMCFM_OBJ_NTWK_INTF, objValue);
		wanObjIndex++;
	}

	uint32 objIndex = 0;

	// Search through BCMCFM_OBJ_IFC_ATM_VCC objects that are not connected to
	// a BCMCFM_OBJ_NTWK_INTF object.
	while ( BcmCfm_objGet(BCMCFM_OBJ_IFC_ATM_VCC, &objValue, &objIndex) == BcmCfm_Ok ) {
		if( findInstanceDescNoPathCheck(connDevNode, objIndex) == NULL ) {
			checkForObjScratchPadLinkTypeEntry( objIndex );
			if( objIndex > highestVccIndex )
				highestVccIndex = objIndex;
		}

		BcmCfm_objFree(BCMCFM_OBJ_IFC_ATM_VCC, objValue);
		objIndex++;
	}

	checkForUnsuedScratchPadLinkTypeEntry( highestVccIndex );
#endif
}



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
 * $Revision: 1.2 $
 * $Id: testWanIF.c,v 1.2 2006/02/17 21:12:52 dmounday Exp $
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
#include "../main/utils.h"
#include "../inc/appdefs.h"
#include "../SOAPParser/CPEframework.h"
#include "../SOAPParser/RPCState.h" /* defines for ACS state */
#include "bcmTestWrapper.h"

#ifdef DMALLOC
#include "dmalloc.h"
#endif


/** */
/** InternetGatewayDevice.LANDevice. */


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


/** */
/** InternetGatewayDevice.WANDevice.WANCommonInterfaceConfig. */

TRX_STATUS setWANDeviceWANCommonInterfaceConfig_EnabledForInternet(char *value)
{
	//InstanceDesc *idp;
	//if( idp=getCurrentInstanceDesc()){
		/* Add code here to set param -- delete idp if not needed *****/
		return TRX_OK;
	//}
	//return TRX_ERR;
}
TRX_STATUS getWANDeviceWANCommonInterfaceConfig_EnabledForInternet(char **value)
{
	//InstanceDesc *idp;
	//if( idp=getCurrentInstanceDesc()){
		/* Add code here to get param -- delete idp if not needed *****/
		 *value = strdup("");
		return TRX_OK;
	//}
	//return TRX_ERR;
}
TRX_STATUS getWANDeviceWANCommonInterfaceConfig_WANAccessType(char **value)
{
	//InstanceDesc *idp;
	//if( idp=getCurrentInstanceDesc()){
		/* Add code here to get param -- delete idp if not needed *****/
		 *value = strdup("");
		return TRX_OK;
	//}
	//return TRX_ERR;
}
TRX_STATUS getWANDeviceWANCommonInterfaceConfig_Layer1UpstreamMaxBitRate(char **value)
{
	//InstanceDesc *idp;
	//if( idp=getCurrentInstanceDesc()){
		/* Add code here to get param -- delete idp if not needed *****/
		 *value = strdup("");
		return TRX_OK;
	//}
	//return TRX_ERR;
}
TRX_STATUS getWANDeviceWANCommonInterfaceConfig_Layer1DownstreamMaxBitRate(char **value)
{
	//InstanceDesc *idp;
	//if( idp=getCurrentInstanceDesc()){
		/* Add code here to get param -- delete idp if not needed *****/
		 *value = strdup("");
		return TRX_OK;
	//}
	//return TRX_ERR;
}
TRX_STATUS getWANDeviceWANCommonInterfaceConfig_PhysicalLinkStatus(char **value)
{
	//InstanceDesc *idp;
	//if( idp=getCurrentInstanceDesc()){
		/* Add code here to get param -- delete idp if not needed *****/
		 *value = strdup("");
		return TRX_OK;
	//}
	//return TRX_ERR;
}
TRX_STATUS getWANDeviceWANCommonInterfaceConfig_WANAccessProvider(char **value)
{
	//InstanceDesc *idp;
	//if( idp=getCurrentInstanceDesc()){
		/* Add code here to get param -- delete idp if not needed *****/
		 *value = strdup("");
		return TRX_OK;
	//}
	//return TRX_ERR;
}
TRX_STATUS getWANDeviceWANCommonInterfaceConfig_TotalBytesSent(char **value)
{
	//InstanceDesc *idp;
	//if( idp=getCurrentInstanceDesc()){
		/* Add code here to get param -- delete idp if not needed *****/
		 *value = strdup("");
		return TRX_OK;
	//}
	//return TRX_ERR;
}
TRX_STATUS getWANDeviceWANCommonInterfaceConfig_TotalBytesReceived(char **value)
{
	//InstanceDesc *idp;
	//if( idp=getCurrentInstanceDesc()){
		/* Add code here to get param -- delete idp if not needed *****/
		 *value = strdup("");
		return TRX_OK;
	//}
	//return TRX_ERR;
}
TRX_STATUS getWANDeviceWANCommonInterfaceConfig_TotalPacketsSent(char **value)
{
	//InstanceDesc *idp;
	//if( idp=getCurrentInstanceDesc()){
		/* Add code here to get param -- delete idp if not needed *****/
		 *value = strdup("");
		return TRX_OK;
	//}
	//return TRX_ERR;
}
TRX_STATUS getWANDeviceWANCommonInterfaceConfig_TotalPacketsReceived(char **value)
{
	//InstanceDesc *idp;
	//if( idp=getCurrentInstanceDesc()){
		/* Add code here to get param -- delete idp if not needed *****/
		 *value = strdup("");
		return TRX_OK;
	//}
	//return TRX_ERR;
}
TRX_STATUS getWANDeviceWANCommonInterfaceConfig_MaximumActiveConnections(char **value)
{
	//InstanceDesc *idp;
	//if( idp=getCurrentInstanceDesc()){
		/* Add code here to get param -- delete idp if not needed *****/
		 *value = strdup("");
		return TRX_OK;
	//}
	//return TRX_ERR;
}
TRX_STATUS getWANDeviceWANCommonInterfaceConfig_NumberOfActiveConnections(char **value)
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
/** InternetGatewayDevice.WANDevice.WANDSLInterfaceConfig.Stats.Total. */

TRX_STATUS getWANDeviceWANDSLInterfaceConfigStatsTotal_ReceiveBlocks(char **value)
{
	//InstanceDesc *idp;
	//if( idp=getCurrentInstanceDesc()){
		/* Add code here to get param -- delete idp if not needed *****/
		 *value = strdup("");
		return TRX_OK;
	//}
	//return TRX_ERR;
}
TRX_STATUS getWANDeviceWANDSLInterfaceConfigStatsTotal_TransmitBlocks(char **value)
{
	//InstanceDesc *idp;
	//if( idp=getCurrentInstanceDesc()){
		/* Add code here to get param -- delete idp if not needed *****/
		 *value = strdup("");
		return TRX_OK;
	//}
	//return TRX_ERR;
}
TRX_STATUS getWANDeviceWANDSLInterfaceConfigStatsTotal_CellDelin(char **value)
{
	//InstanceDesc *idp;
	//if( idp=getCurrentInstanceDesc()){
		/* Add code here to get param -- delete idp if not needed *****/
		 *value = strdup("");
		return TRX_OK;
	//}
	//return TRX_ERR;
}
TRX_STATUS getWANDeviceWANDSLInterfaceConfigStatsTotal_LinkRetrain(char **value)
{
	//InstanceDesc *idp;
	//if( idp=getCurrentInstanceDesc()){
		/* Add code here to get param -- delete idp if not needed *****/
		 *value = strdup("");
		return TRX_OK;
	//}
	//return TRX_ERR;
}
TRX_STATUS getWANDeviceWANDSLInterfaceConfigStatsTotal_InitErrors(char **value)
{
	//InstanceDesc *idp;
	//if( idp=getCurrentInstanceDesc()){
		/* Add code here to get param -- delete idp if not needed *****/
		 *value = strdup("");
		return TRX_OK;
	//}
	//return TRX_ERR;
}
TRX_STATUS getWANDeviceWANDSLInterfaceConfigStatsTotal_InitTimeouts(char **value)
{
	//InstanceDesc *idp;
	//if( idp=getCurrentInstanceDesc()){
		/* Add code here to get param -- delete idp if not needed *****/
		 *value = strdup("");
		return TRX_OK;
	//}
	//return TRX_ERR;
}
TRX_STATUS getWANDeviceWANDSLInterfaceConfigStatsTotal_LossOfFraming(char **value)
{
	//InstanceDesc *idp;
	//if( idp=getCurrentInstanceDesc()){
		/* Add code here to get param -- delete idp if not needed *****/
		 *value = strdup("");
		return TRX_OK;
	//}
	//return TRX_ERR;
}
TRX_STATUS getWANDeviceWANDSLInterfaceConfigStatsTotal_ErroredSecs(char **value)
{
	//InstanceDesc *idp;
	//if( idp=getCurrentInstanceDesc()){
		/* Add code here to get param -- delete idp if not needed *****/
		 *value = strdup("");
		return TRX_OK;
	//}
	//return TRX_ERR;
}
TRX_STATUS getWANDeviceWANDSLInterfaceConfigStatsTotal_SeverelyErroredSecs(char **value)
{
	//InstanceDesc *idp;
	//if( idp=getCurrentInstanceDesc()){
		/* Add code here to get param -- delete idp if not needed *****/
		 *value = strdup("");
		return TRX_OK;
	//}
	//return TRX_ERR;
}
TRX_STATUS getWANDeviceWANDSLInterfaceConfigStatsTotal_FECErrors(char **value)
{
	//InstanceDesc *idp;
	//if( idp=getCurrentInstanceDesc()){
		/* Add code here to get param -- delete idp if not needed *****/
		 *value = strdup("");
		return TRX_OK;
	//}
	//return TRX_ERR;
}
TRX_STATUS getWANDeviceWANDSLInterfaceConfigStatsTotal_ATUCFECErrors(char **value)
{
	//InstanceDesc *idp;
	//if( idp=getCurrentInstanceDesc()){
		/* Add code here to get param -- delete idp if not needed *****/
		 *value = strdup("");
		return TRX_OK;
	//}
	//return TRX_ERR;
}
TRX_STATUS getWANDeviceWANDSLInterfaceConfigStatsTotal_HECErrors(char **value)
{
	//InstanceDesc *idp;
	//if( idp=getCurrentInstanceDesc()){
		/* Add code here to get param -- delete idp if not needed *****/
		 *value = strdup("");
		return TRX_OK;
	//}
	//return TRX_ERR;
}
TRX_STATUS getWANDeviceWANDSLInterfaceConfigStatsTotal_ATUCHECErrors(char **value)
{
	//InstanceDesc *idp;
	//if( idp=getCurrentInstanceDesc()){
		/* Add code here to get param -- delete idp if not needed *****/
		 *value = strdup("");
		return TRX_OK;
	//}
	//return TRX_ERR;
}
TRX_STATUS getWANDeviceWANDSLInterfaceConfigStatsTotal_CRCErrors(char **value)
{
	//InstanceDesc *idp;
	//if( idp=getCurrentInstanceDesc()){
		/* Add code here to get param -- delete idp if not needed *****/
		 *value = strdup("");
		return TRX_OK;
	//}
	//return TRX_ERR;
}
TRX_STATUS getWANDeviceWANDSLInterfaceConfigStatsTotal_ATUCCRCErrors(char **value)
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
/** InternetGatewayDevice.WANDevice.WANDSLInterfaceConfig.Stats.Showtime. */

TRX_STATUS getWANDeviceWANDSLInterfaceConfigStatsShowtime_ReceiveBlocks(char **value)
{
	//InstanceDesc *idp;
	//if( idp=getCurrentInstanceDesc()){
		/* Add code here to get param -- delete idp if not needed *****/
		 *value = strdup("");
		return TRX_OK;
	//}
	//return TRX_ERR;
}
TRX_STATUS getWANDeviceWANDSLInterfaceConfigStatsShowtime_TransmitBlocks(char **value)
{
	//InstanceDesc *idp;
	//if( idp=getCurrentInstanceDesc()){
		/* Add code here to get param -- delete idp if not needed *****/
		 *value = strdup("");
		return TRX_OK;
	//}
	//return TRX_ERR;
}
TRX_STATUS getWANDeviceWANDSLInterfaceConfigStatsShowtime_CellDelin(char **value)
{
	//InstanceDesc *idp;
	//if( idp=getCurrentInstanceDesc()){
		/* Add code here to get param -- delete idp if not needed *****/
		 *value = strdup("");
		return TRX_OK;
	//}
	//return TRX_ERR;
}
TRX_STATUS getWANDeviceWANDSLInterfaceConfigStatsShowtime_LinkRetrain(char **value)
{
	//InstanceDesc *idp;
	//if( idp=getCurrentInstanceDesc()){
		/* Add code here to get param -- delete idp if not needed *****/
		 *value = strdup("");
		return TRX_OK;
	//}
	//return TRX_ERR;
}
TRX_STATUS getWANDeviceWANDSLInterfaceConfigStatsShowtime_InitErrors(char **value)
{
	//InstanceDesc *idp;
	//if( idp=getCurrentInstanceDesc()){
		/* Add code here to get param -- delete idp if not needed *****/
		 *value = strdup("");
		return TRX_OK;
	//}
	//return TRX_ERR;
}
TRX_STATUS getWANDeviceWANDSLInterfaceConfigStatsShowtime_InitTimeouts(char **value)
{
	//InstanceDesc *idp;
	//if( idp=getCurrentInstanceDesc()){
		/* Add code here to get param -- delete idp if not needed *****/
		 *value = strdup("");
		return TRX_OK;
	//}
	//return TRX_ERR;
}
TRX_STATUS getWANDeviceWANDSLInterfaceConfigStatsShowtime_LossOfFraming(char **value)
{
	//InstanceDesc *idp;
	//if( idp=getCurrentInstanceDesc()){
		/* Add code here to get param -- delete idp if not needed *****/
		 *value = strdup("");
		return TRX_OK;
	//}
	//return TRX_ERR;
}
TRX_STATUS getWANDeviceWANDSLInterfaceConfigStatsShowtime_ErroredSecs(char **value)
{
	//InstanceDesc *idp;
	//if( idp=getCurrentInstanceDesc()){
		/* Add code here to get param -- delete idp if not needed *****/
		 *value = strdup("");
		return TRX_OK;
	//}
	//return TRX_ERR;
}
TRX_STATUS getWANDeviceWANDSLInterfaceConfigStatsShowtime_SeverelyErroredSecs(char **value)
{
	//InstanceDesc *idp;
	//if( idp=getCurrentInstanceDesc()){
		/* Add code here to get param -- delete idp if not needed *****/
		 *value = strdup("");
		return TRX_OK;
	//}
	//return TRX_ERR;
}
TRX_STATUS getWANDeviceWANDSLInterfaceConfigStatsShowtime_FECErrors(char **value)
{
	//InstanceDesc *idp;
	//if( idp=getCurrentInstanceDesc()){
		/* Add code here to get param -- delete idp if not needed *****/
		 *value = strdup("");
		return TRX_OK;
	//}
	//return TRX_ERR;
}
TRX_STATUS getWANDeviceWANDSLInterfaceConfigStatsShowtime_ATUCFECErrors(char **value)
{
	//InstanceDesc *idp;
	//if( idp=getCurrentInstanceDesc()){
		/* Add code here to get param -- delete idp if not needed *****/
		 *value = strdup("");
		return TRX_OK;
	//}
	//return TRX_ERR;
}
TRX_STATUS getWANDeviceWANDSLInterfaceConfigStatsShowtime_HECErrors(char **value)
{
	//InstanceDesc *idp;
	//if( idp=getCurrentInstanceDesc()){
		/* Add code here to get param -- delete idp if not needed *****/
		 *value = strdup("");
		return TRX_OK;
	//}
	//return TRX_ERR;
}
TRX_STATUS getWANDeviceWANDSLInterfaceConfigStatsShowtime_ATUCHECErrors(char **value)
{
	//InstanceDesc *idp;
	//if( idp=getCurrentInstanceDesc()){
		/* Add code here to get param -- delete idp if not needed *****/
		 *value = strdup("");
		return TRX_OK;
	//}
	//return TRX_ERR;
}
TRX_STATUS getWANDeviceWANDSLInterfaceConfigStatsShowtime_CRCErrors(char **value)
{
	//InstanceDesc *idp;
	//if( idp=getCurrentInstanceDesc()){
		/* Add code here to get param -- delete idp if not needed *****/
		 *value = strdup("");
		return TRX_OK;
	//}
	//return TRX_ERR;
}
TRX_STATUS getWANDeviceWANDSLInterfaceConfigStatsShowtime_ATUCCRCErrors(char **value)
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
/** InternetGatewayDevice.WANDevice.WANDSLInterfaceConfig.Stats.LastShowtime. */

TRX_STATUS getWANDeviceWANDSLInterfaceConfigStatsLastShowtime_ReceiveBlocks(char **value)
{
	//InstanceDesc *idp;
	//if( idp=getCurrentInstanceDesc()){
		/* Add code here to get param -- delete idp if not needed *****/
		 *value = strdup("");
		return TRX_OK;
	//}
	//return TRX_ERR;
}
TRX_STATUS getWANDeviceWANDSLInterfaceConfigStatsLastShowtime_TransmitBlocks(char **value)
{
	//InstanceDesc *idp;
	//if( idp=getCurrentInstanceDesc()){
		/* Add code here to get param -- delete idp if not needed *****/
		 *value = strdup("");
		return TRX_OK;
	//}
	//return TRX_ERR;
}
TRX_STATUS getWANDeviceWANDSLInterfaceConfigStatsLastShowtime_CellDelin(char **value)
{
	//InstanceDesc *idp;
	//if( idp=getCurrentInstanceDesc()){
		/* Add code here to get param -- delete idp if not needed *****/
		 *value = strdup("");
		return TRX_OK;
	//}
	//return TRX_ERR;
}
TRX_STATUS getWANDeviceWANDSLInterfaceConfigStatsLastShowtime_LinkRetrain(char **value)
{
	//InstanceDesc *idp;
	//if( idp=getCurrentInstanceDesc()){
		/* Add code here to get param -- delete idp if not needed *****/
		 *value = strdup("");
		return TRX_OK;
	//}
	//return TRX_ERR;
}
TRX_STATUS getWANDeviceWANDSLInterfaceConfigStatsLastShowtime_InitErrors(char **value)
{
	//InstanceDesc *idp;
	//if( idp=getCurrentInstanceDesc()){
		/* Add code here to get param -- delete idp if not needed *****/
		 *value = strdup("");
		return TRX_OK;
	//}
	//return TRX_ERR;
}
TRX_STATUS getWANDeviceWANDSLInterfaceConfigStatsLastShowtime_InitTimeouts(char **value)
{
	//InstanceDesc *idp;
	//if( idp=getCurrentInstanceDesc()){
		/* Add code here to get param -- delete idp if not needed *****/
		 *value = strdup("");
		return TRX_OK;
	//}
	//return TRX_ERR;
}
TRX_STATUS getWANDeviceWANDSLInterfaceConfigStatsLastShowtime_LossOfFraming(char **value)
{
	//InstanceDesc *idp;
	//if( idp=getCurrentInstanceDesc()){
		/* Add code here to get param -- delete idp if not needed *****/
		 *value = strdup("");
		return TRX_OK;
	//}
	//return TRX_ERR;
}
TRX_STATUS getWANDeviceWANDSLInterfaceConfigStatsLastShowtime_ErroredSecs(char **value)
{
	//InstanceDesc *idp;
	//if( idp=getCurrentInstanceDesc()){
		/* Add code here to get param -- delete idp if not needed *****/
		 *value = strdup("");
		return TRX_OK;
	//}
	//return TRX_ERR;
}
TRX_STATUS getWANDeviceWANDSLInterfaceConfigStatsLastShowtime_SeverelyErroredSecs(char **value)
{
	//InstanceDesc *idp;
	//if( idp=getCurrentInstanceDesc()){
		/* Add code here to get param -- delete idp if not needed *****/
		 *value = strdup("");
		return TRX_OK;
	//}
	//return TRX_ERR;
}
TRX_STATUS getWANDeviceWANDSLInterfaceConfigStatsLastShowtime_FECErrors(char **value)
{
	//InstanceDesc *idp;
	//if( idp=getCurrentInstanceDesc()){
		/* Add code here to get param -- delete idp if not needed *****/
		 *value = strdup("");
		return TRX_OK;
	//}
	//return TRX_ERR;
}
TRX_STATUS getWANDeviceWANDSLInterfaceConfigStatsLastShowtime_ATUCFECErrors(char **value)
{
	//InstanceDesc *idp;
	//if( idp=getCurrentInstanceDesc()){
		/* Add code here to get param -- delete idp if not needed *****/
		 *value = strdup("");
		return TRX_OK;
	//}
	//return TRX_ERR;
}
TRX_STATUS getWANDeviceWANDSLInterfaceConfigStatsLastShowtime_HECErrors(char **value)
{
	//InstanceDesc *idp;
	//if( idp=getCurrentInstanceDesc()){
		/* Add code here to get param -- delete idp if not needed *****/
		 *value = strdup("");
		return TRX_OK;
	//}
	//return TRX_ERR;
}
TRX_STATUS getWANDeviceWANDSLInterfaceConfigStatsLastShowtime_ATUCHECErrors(char **value)
{
	//InstanceDesc *idp;
	//if( idp=getCurrentInstanceDesc()){
		/* Add code here to get param -- delete idp if not needed *****/
		 *value = strdup("");
		return TRX_OK;
	//}
	//return TRX_ERR;
}
TRX_STATUS getWANDeviceWANDSLInterfaceConfigStatsLastShowtime_CRCErrors(char **value)
{
	//InstanceDesc *idp;
	//if( idp=getCurrentInstanceDesc()){
		/* Add code here to get param -- delete idp if not needed *****/
		 *value = strdup("");
		return TRX_OK;
	//}
	//return TRX_ERR;
}
TRX_STATUS getWANDeviceWANDSLInterfaceConfigStatsLastShowtime_ATUCCRCErrors(char **value)
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
/** InternetGatewayDevice.WANDevice.WANDSLInterfaceConfig.Stats.CurrentDay. */

TRX_STATUS getWANDeviceWANDSLInterfaceConfigStatsCurrentDay_ReceiveBlocks(char **value)
{
	//InstanceDesc *idp;
	//if( idp=getCurrentInstanceDesc()){
		/* Add code here to get param -- delete idp if not needed *****/
		 *value = strdup("");
		return TRX_OK;
	//}
	//return TRX_ERR;
}
TRX_STATUS getWANDeviceWANDSLInterfaceConfigStatsCurrentDay_TransmitBlocks(char **value)
{
	//InstanceDesc *idp;
	//if( idp=getCurrentInstanceDesc()){
		/* Add code here to get param -- delete idp if not needed *****/
		 *value = strdup("");
		return TRX_OK;
	//}
	//return TRX_ERR;
}
TRX_STATUS getWANDeviceWANDSLInterfaceConfigStatsCurrentDay_CellDelin(char **value)
{
	//InstanceDesc *idp;
	//if( idp=getCurrentInstanceDesc()){
		/* Add code here to get param -- delete idp if not needed *****/
		 *value = strdup("");
		return TRX_OK;
	//}
	//return TRX_ERR;
}
TRX_STATUS getWANDeviceWANDSLInterfaceConfigStatsCurrentDay_LinkRetrain(char **value)
{
	//InstanceDesc *idp;
	//if( idp=getCurrentInstanceDesc()){
		/* Add code here to get param -- delete idp if not needed *****/
		 *value = strdup("");
		return TRX_OK;
	//}
	//return TRX_ERR;
}
TRX_STATUS getWANDeviceWANDSLInterfaceConfigStatsCurrentDay_InitErrors(char **value)
{
	//InstanceDesc *idp;
	//if( idp=getCurrentInstanceDesc()){
		/* Add code here to get param -- delete idp if not needed *****/
		 *value = strdup("");
		return TRX_OK;
	//}
	//return TRX_ERR;
}
TRX_STATUS getWANDeviceWANDSLInterfaceConfigStatsCurrentDay_InitTimeouts(char **value)
{
	//InstanceDesc *idp;
	//if( idp=getCurrentInstanceDesc()){
		/* Add code here to get param -- delete idp if not needed *****/
		 *value = strdup("");
		return TRX_OK;
	//}
	//return TRX_ERR;
}
TRX_STATUS getWANDeviceWANDSLInterfaceConfigStatsCurrentDay_LossOfFraming(char **value)
{
	//InstanceDesc *idp;
	//if( idp=getCurrentInstanceDesc()){
		/* Add code here to get param -- delete idp if not needed *****/
		 *value = strdup("");
		return TRX_OK;
	//}
	//return TRX_ERR;
}
TRX_STATUS getWANDeviceWANDSLInterfaceConfigStatsCurrentDay_ErroredSecs(char **value)
{
	//InstanceDesc *idp;
	//if( idp=getCurrentInstanceDesc()){
		/* Add code here to get param -- delete idp if not needed *****/
		 *value = strdup("");
		return TRX_OK;
	//}
	//return TRX_ERR;
}
TRX_STATUS getWANDeviceWANDSLInterfaceConfigStatsCurrentDay_SeverelyErroredSecs(char **value)
{
	//InstanceDesc *idp;
	//if( idp=getCurrentInstanceDesc()){
		/* Add code here to get param -- delete idp if not needed *****/
		 *value = strdup("");
		return TRX_OK;
	//}
	//return TRX_ERR;
}
TRX_STATUS getWANDeviceWANDSLInterfaceConfigStatsCurrentDay_FECErrors(char **value)
{
	//InstanceDesc *idp;
	//if( idp=getCurrentInstanceDesc()){
		/* Add code here to get param -- delete idp if not needed *****/
		 *value = strdup("");
		return TRX_OK;
	//}
	//return TRX_ERR;
}
TRX_STATUS getWANDeviceWANDSLInterfaceConfigStatsCurrentDay_ATUCFECErrors(char **value)
{
	//InstanceDesc *idp;
	//if( idp=getCurrentInstanceDesc()){
		/* Add code here to get param -- delete idp if not needed *****/
		 *value = strdup("");
		return TRX_OK;
	//}
	//return TRX_ERR;
}
TRX_STATUS getWANDeviceWANDSLInterfaceConfigStatsCurrentDay_HECErrors(char **value)
{
	//InstanceDesc *idp;
	//if( idp=getCurrentInstanceDesc()){
		/* Add code here to get param -- delete idp if not needed *****/
		 *value = strdup("");
		return TRX_OK;
	//}
	//return TRX_ERR;
}
TRX_STATUS getWANDeviceWANDSLInterfaceConfigStatsCurrentDay_ATUCHECErrors(char **value)
{
	//InstanceDesc *idp;
	//if( idp=getCurrentInstanceDesc()){
		/* Add code here to get param -- delete idp if not needed *****/
		 *value = strdup("");
		return TRX_OK;
	//}
	//return TRX_ERR;
}
TRX_STATUS getWANDeviceWANDSLInterfaceConfigStatsCurrentDay_CRCErrors(char **value)
{
	//InstanceDesc *idp;
	//if( idp=getCurrentInstanceDesc()){
		/* Add code here to get param -- delete idp if not needed *****/
		 *value = strdup("");
		return TRX_OK;
	//}
	//return TRX_ERR;
}
TRX_STATUS getWANDeviceWANDSLInterfaceConfigStatsCurrentDay_ATUCCRCErrors(char **value)
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
/** InternetGatewayDevice.WANDevice.WANDSLInterfaceConfig.Stats.QuarterHour. */

TRX_STATUS getWANDeviceWANDSLInterfaceConfigStatsQuarterHour_ReceiveBlocks(char **value)
{
	//InstanceDesc *idp;
	//if( idp=getCurrentInstanceDesc()){
		/* Add code here to get param -- delete idp if not needed *****/
		 *value = strdup("");
		return TRX_OK;
	//}
	//return TRX_ERR;
}
TRX_STATUS getWANDeviceWANDSLInterfaceConfigStatsQuarterHour_TransmitBlocks(char **value)
{
	//InstanceDesc *idp;
	//if( idp=getCurrentInstanceDesc()){
		/* Add code here to get param -- delete idp if not needed *****/
		 *value = strdup("");
		return TRX_OK;
	//}
	//return TRX_ERR;
}
TRX_STATUS getWANDeviceWANDSLInterfaceConfigStatsQuarterHour_CellDelin(char **value)
{
	//InstanceDesc *idp;
	//if( idp=getCurrentInstanceDesc()){
		/* Add code here to get param -- delete idp if not needed *****/
		 *value = strdup("");
		return TRX_OK;
	//}
	//return TRX_ERR;
}
TRX_STATUS getWANDeviceWANDSLInterfaceConfigStatsQuarterHour_LinkRetrain(char **value)
{
	//InstanceDesc *idp;
	//if( idp=getCurrentInstanceDesc()){
		/* Add code here to get param -- delete idp if not needed *****/
		 *value = strdup("");
		return TRX_OK;
	//}
	//return TRX_ERR;
}
TRX_STATUS getWANDeviceWANDSLInterfaceConfigStatsQuarterHour_InitErrors(char **value)
{
	//InstanceDesc *idp;
	//if( idp=getCurrentInstanceDesc()){
		/* Add code here to get param -- delete idp if not needed *****/
		 *value = strdup("");
		return TRX_OK;
	//}
	//return TRX_ERR;
}
TRX_STATUS getWANDeviceWANDSLInterfaceConfigStatsQuarterHour_InitTimeouts(char **value)
{
	//InstanceDesc *idp;
	//if( idp=getCurrentInstanceDesc()){
		/* Add code here to get param -- delete idp if not needed *****/
		 *value = strdup("");
		return TRX_OK;
	//}
	//return TRX_ERR;
}
TRX_STATUS getWANDeviceWANDSLInterfaceConfigStatsQuarterHour_LossOfFraming(char **value)
{
	//InstanceDesc *idp;
	//if( idp=getCurrentInstanceDesc()){
		/* Add code here to get param -- delete idp if not needed *****/
		 *value = strdup("");
		return TRX_OK;
	//}
	//return TRX_ERR;
}
TRX_STATUS getWANDeviceWANDSLInterfaceConfigStatsQuarterHour_ErroredSecs(char **value)
{
	//InstanceDesc *idp;
	//if( idp=getCurrentInstanceDesc()){
		/* Add code here to get param -- delete idp if not needed *****/
		 *value = strdup("");
		return TRX_OK;
	//}
	//return TRX_ERR;
}
TRX_STATUS getWANDeviceWANDSLInterfaceConfigStatsQuarterHour_SeverelyErroredSecs(char **value)
{
	//InstanceDesc *idp;
	//if( idp=getCurrentInstanceDesc()){
		/* Add code here to get param -- delete idp if not needed *****/
		 *value = strdup("");
		return TRX_OK;
	//}
	//return TRX_ERR;
}
TRX_STATUS getWANDeviceWANDSLInterfaceConfigStatsQuarterHour_FECErrors(char **value)
{
	//InstanceDesc *idp;
	//if( idp=getCurrentInstanceDesc()){
		/* Add code here to get param -- delete idp if not needed *****/
		 *value = strdup("");
		return TRX_OK;
	//}
	//return TRX_ERR;
}
TRX_STATUS getWANDeviceWANDSLInterfaceConfigStatsQuarterHour_ATUCFECErrors(char **value)
{
	//InstanceDesc *idp;
	//if( idp=getCurrentInstanceDesc()){
		/* Add code here to get param -- delete idp if not needed *****/
		 *value = strdup("");
		return TRX_OK;
	//}
	//return TRX_ERR;
}
TRX_STATUS getWANDeviceWANDSLInterfaceConfigStatsQuarterHour_HECErrors(char **value)
{
	//InstanceDesc *idp;
	//if( idp=getCurrentInstanceDesc()){
		/* Add code here to get param -- delete idp if not needed *****/
		 *value = strdup("");
		return TRX_OK;
	//}
	//return TRX_ERR;
}
TRX_STATUS getWANDeviceWANDSLInterfaceConfigStatsQuarterHour_ATUCHECErrors(char **value)
{
	//InstanceDesc *idp;
	//if( idp=getCurrentInstanceDesc()){
		/* Add code here to get param -- delete idp if not needed *****/
		 *value = strdup("");
		return TRX_OK;
	//}
	//return TRX_ERR;
}
TRX_STATUS getWANDeviceWANDSLInterfaceConfigStatsQuarterHour_CRCErrors(char **value)
{
	//InstanceDesc *idp;
	//if( idp=getCurrentInstanceDesc()){
		/* Add code here to get param -- delete idp if not needed *****/
		 *value = strdup("");
		return TRX_OK;
	//}
	//return TRX_ERR;
}
TRX_STATUS getWANDeviceWANDSLInterfaceConfigStatsQuarterHour_ATUCCRCErrors(char **value)
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
/** InternetGatewayDevice.WANDevice.WANDSLInterfaceConfig.Stats. */


/** */
/** InternetGatewayDevice.WANDevice.WANDSLInterfaceConfig. */

TRX_STATUS setWANDeviceWANDSLInterfaceConfig_Enable(char *value)
{
	//InstanceDesc *idp;
	//if( idp=getCurrentInstanceDesc()){
		/* Add code here to set param -- delete idp if not needed *****/
		return TRX_OK;
	//}
	//return TRX_ERR;
}
TRX_STATUS getWANDeviceWANDSLInterfaceConfig_Enable(char **value)
{
	//InstanceDesc *idp;
	//if( idp=getCurrentInstanceDesc()){
		/* Add code here to get param -- delete idp if not needed *****/
		 *value = strdup("");
		return TRX_OK;
	//}
	//return TRX_ERR;
}
TRX_STATUS getWANDeviceWANDSLInterfaceConfig_Status(char **value)
{
	//InstanceDesc *idp;
	//if( idp=getCurrentInstanceDesc()){
		/* Add code here to get param -- delete idp if not needed *****/
		 *value = strdup("");
		return TRX_OK;
	//}
	//return TRX_ERR;
}
TRX_STATUS getWANDeviceWANDSLInterfaceConfig_ModulationType(char **value)
{
	//InstanceDesc *idp;
	//if( idp=getCurrentInstanceDesc()){
		/* Add code here to get param -- delete idp if not needed *****/
		 *value = strdup("");
		return TRX_OK;
	//}
	//return TRX_ERR;
}
TRX_STATUS getWANDeviceWANDSLInterfaceConfig_LineEncoding(char **value)
{
	//InstanceDesc *idp;
	//if( idp=getCurrentInstanceDesc()){
		/* Add code here to get param -- delete idp if not needed *****/
		 *value = strdup("");
		return TRX_OK;
	//}
	//return TRX_ERR;
}
TRX_STATUS getWANDeviceWANDSLInterfaceConfig_DataPath(char **value)
{
	//InstanceDesc *idp;
	//if( idp=getCurrentInstanceDesc()){
		/* Add code here to get param -- delete idp if not needed *****/
		 *value = strdup("");
		return TRX_OK;
	//}
	//return TRX_ERR;
}
TRX_STATUS getWANDeviceWANDSLInterfaceConfig_InterleaveDepth(char **value)
{
	//InstanceDesc *idp;
	//if( idp=getCurrentInstanceDesc()){
		/* Add code here to get param -- delete idp if not needed *****/
		 *value = strdup("");
		return TRX_OK;
	//}
	//return TRX_ERR;
}
TRX_STATUS getWANDeviceWANDSLInterfaceConfig_LineNumber(char **value)
{
	//InstanceDesc *idp;
	//if( idp=getCurrentInstanceDesc()){
		/* Add code here to get param -- delete idp if not needed *****/
		 *value = strdup("");
		return TRX_OK;
	//}
	//return TRX_ERR;
}
TRX_STATUS getWANDeviceWANDSLInterfaceConfig_UpstreamCurrRate(char **value)
{
	//InstanceDesc *idp;
	//if( idp=getCurrentInstanceDesc()){
		/* Add code here to get param -- delete idp if not needed *****/
		 *value = strdup("");
		return TRX_OK;
	//}
	//return TRX_ERR;
}
TRX_STATUS getWANDeviceWANDSLInterfaceConfig_DownstreamCurrRate(char **value)
{
	//InstanceDesc *idp;
	//if( idp=getCurrentInstanceDesc()){
		/* Add code here to get param -- delete idp if not needed *****/
		 *value = strdup("");
		return TRX_OK;
	//}
	//return TRX_ERR;
}
TRX_STATUS getWANDeviceWANDSLInterfaceConfig_UpstreamMaxRate(char **value)
{
	//InstanceDesc *idp;
	//if( idp=getCurrentInstanceDesc()){
		/* Add code here to get param -- delete idp if not needed *****/
		 *value = strdup("");
		return TRX_OK;
	//}
	//return TRX_ERR;
}
TRX_STATUS getWANDeviceWANDSLInterfaceConfig_DownstreamMaxRate(char **value)
{
	//InstanceDesc *idp;
	//if( idp=getCurrentInstanceDesc()){
		/* Add code here to get param -- delete idp if not needed *****/
		 *value = strdup("");
		return TRX_OK;
	//}
	//return TRX_ERR;
}
TRX_STATUS getWANDeviceWANDSLInterfaceConfig_UpstreamNoiseMargin(char **value)
{
	//InstanceDesc *idp;
	//if( idp=getCurrentInstanceDesc()){
		/* Add code here to get param -- delete idp if not needed *****/
		 *value = strdup("");
		return TRX_OK;
	//}
	//return TRX_ERR;
}
TRX_STATUS getWANDeviceWANDSLInterfaceConfig_DownstreamNoiseMargin(char **value)
{
	//InstanceDesc *idp;
	//if( idp=getCurrentInstanceDesc()){
		/* Add code here to get param -- delete idp if not needed *****/
		 *value = strdup("");
		return TRX_OK;
	//}
	//return TRX_ERR;
}
TRX_STATUS getWANDeviceWANDSLInterfaceConfig_UpstreamAttenuation(char **value)
{
	//InstanceDesc *idp;
	//if( idp=getCurrentInstanceDesc()){
		/* Add code here to get param -- delete idp if not needed *****/
		 *value = strdup("");
		return TRX_OK;
	//}
	//return TRX_ERR;
}
TRX_STATUS getWANDeviceWANDSLInterfaceConfig_DownstreamAttenuation(char **value)
{
	//InstanceDesc *idp;
	//if( idp=getCurrentInstanceDesc()){
		/* Add code here to get param -- delete idp if not needed *****/
		 *value = strdup("");
		return TRX_OK;
	//}
	//return TRX_ERR;
}
TRX_STATUS getWANDeviceWANDSLInterfaceConfig_UpstreamPower(char **value)
{
	//InstanceDesc *idp;
	//if( idp=getCurrentInstanceDesc()){
		/* Add code here to get param -- delete idp if not needed *****/
		 *value = strdup("");
		return TRX_OK;
	//}
	//return TRX_ERR;
}
TRX_STATUS getWANDeviceWANDSLInterfaceConfig_DownstreamPower(char **value)
{
	//InstanceDesc *idp;
	//if( idp=getCurrentInstanceDesc()){
		/* Add code here to get param -- delete idp if not needed *****/
		 *value = strdup("");
		return TRX_OK;
	//}
	//return TRX_ERR;
}
TRX_STATUS getWANDeviceWANDSLInterfaceConfig_ATURVendor(char **value)
{
	//InstanceDesc *idp;
	//if( idp=getCurrentInstanceDesc()){
		/* Add code here to get param -- delete idp if not needed *****/
		 *value = strdup("");
		return TRX_OK;
	//}
	//return TRX_ERR;
}
TRX_STATUS getWANDeviceWANDSLInterfaceConfig_ATURCountry(char **value)
{
	//InstanceDesc *idp;
	//if( idp=getCurrentInstanceDesc()){
		/* Add code here to get param -- delete idp if not needed *****/
		 *value = strdup("");
		return TRX_OK;
	//}
	//return TRX_ERR;
}
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
TRX_STATUS getWANDeviceWANDSLInterfaceConfig_ATUCVendor(char **value)
{
	//InstanceDesc *idp;
	//if( idp=getCurrentInstanceDesc()){
		/* Add code here to get param -- delete idp if not needed *****/
		 *value = strdup("");
		return TRX_OK;
	//}
	//return TRX_ERR;
}
TRX_STATUS getWANDeviceWANDSLInterfaceConfig_ATUCCountry(char **value)
{
	//InstanceDesc *idp;
	//if( idp=getCurrentInstanceDesc()){
		/* Add code here to get param -- delete idp if not needed *****/
		 *value = strdup("");
		return TRX_OK;
	//}
	//return TRX_ERR;
}
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
TRX_STATUS getWANDeviceWANDSLInterfaceConfig_TotalStart(char **value)
{
	//InstanceDesc *idp;
	//if( idp=getCurrentInstanceDesc()){
		/* Add code here to get param -- delete idp if not needed *****/
		 *value = strdup("");
		return TRX_OK;
	//}
	//return TRX_ERR;
}
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

/** */
/** InternetGatewayDevice.WANDevice.WANDSLConnectionManagement.ConnectionService. */

TRX_STATUS getWANDeviceWANDSLConnectionManagementConnectionService_WANConnectionDevice(char **value)
{
	//InstanceDesc *idp;
	//if( idp=getCurrentInstanceDesc()){
		/* Add code here to get param -- delete idp if not needed *****/
		 *value = strdup("");
		return TRX_OK;
	//}
	//return TRX_ERR;
}
TRX_STATUS getWANDeviceWANDSLConnectionManagementConnectionService_WANConnectionService(char **value)
{
	//InstanceDesc *idp;
	//if( idp=getCurrentInstanceDesc()){
		/* Add code here to get param -- delete idp if not needed *****/
		 *value = strdup("");
		return TRX_OK;
	//}
	//return TRX_ERR;
}
TRX_STATUS getWANDeviceWANDSLConnectionManagementConnectionService_DestinationAddress(char **value)
{
	//InstanceDesc *idp;
	//if( idp=getCurrentInstanceDesc()){
		/* Add code here to get param -- delete idp if not needed *****/
		 *value = strdup("");
		return TRX_OK;
	//}
	//return TRX_ERR;
}
TRX_STATUS getWANDeviceWANDSLConnectionManagementConnectionService_LinkType(char **value)
{
	//InstanceDesc *idp;
	//if( idp=getCurrentInstanceDesc()){
		/* Add code here to get param -- delete idp if not needed *****/
		 *value = strdup("");
		return TRX_OK;
	//}
	//return TRX_ERR;
}
TRX_STATUS getWANDeviceWANDSLConnectionManagementConnectionService_ConnectionType(char **value)
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
/** InternetGatewayDevice.WANDevice.WANDSLConnectionManagement.ConnectionService. */


/** */
/** InternetGatewayDevice.WANDevice.WANDSLConnectionManagement. */

TRX_STATUS getWANDeviceWANDSLConnectionManagement_ConnectionServiceNumberOfEntries(char **value)
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
/** InternetGatewayDevice.WANDevice.WANDSLDiagnostics. */

TRX_STATUS setWANDeviceWANDSLDiagnostics_LoopDiagnosticsState(char *value)
{
	//InstanceDesc *idp;
	//if( idp=getCurrentInstanceDesc()){
		/* Add code here to set param -- delete idp if not needed *****/
		return TRX_OK;
	//}
	//return TRX_ERR;
}
TRX_STATUS getWANDeviceWANDSLDiagnostics_LoopDiagnosticsState(char **value)
{
	//InstanceDesc *idp;
	//if( idp=getCurrentInstanceDesc()){
		/* Add code here to get param -- delete idp if not needed *****/
		 *value = strdup("");
		return TRX_OK;
	//}
	//return TRX_ERR;
}
TRX_STATUS getWANDeviceWANDSLDiagnostics_ACTPSDds(char **value)
{
	//InstanceDesc *idp;
	//if( idp=getCurrentInstanceDesc()){
		/* Add code here to get param -- delete idp if not needed *****/
		 *value = strdup("");
		return TRX_OK;
	//}
	//return TRX_ERR;
}
TRX_STATUS getWANDeviceWANDSLDiagnostics_ACTPSDus(char **value)
{
	//InstanceDesc *idp;
	//if( idp=getCurrentInstanceDesc()){
		/* Add code here to get param -- delete idp if not needed *****/
		 *value = strdup("");
		return TRX_OK;
	//}
	//return TRX_ERR;
}
TRX_STATUS getWANDeviceWANDSLDiagnostics_ACTATPds(char **value)
{
	//InstanceDesc *idp;
	//if( idp=getCurrentInstanceDesc()){
		/* Add code here to get param -- delete idp if not needed *****/
		 *value = strdup("");
		return TRX_OK;
	//}
	//return TRX_ERR;
}
TRX_STATUS getWANDeviceWANDSLDiagnostics_ACTATPus(char **value)
{
	//InstanceDesc *idp;
	//if( idp=getCurrentInstanceDesc()){
		/* Add code here to get param -- delete idp if not needed *****/
		 *value = strdup("");
		return TRX_OK;
	//}
	//return TRX_ERR;
}
TRX_STATUS getWANDeviceWANDSLDiagnostics_HLINSCds(char **value)
{
	//InstanceDesc *idp;
	//if( idp=getCurrentInstanceDesc()){
		/* Add code here to get param -- delete idp if not needed *****/
		 *value = strdup("");
		return TRX_OK;
	//}
	//return TRX_ERR;
}
TRX_STATUS getWANDeviceWANDSLDiagnostics_HLINpsds(char **value)
{
	//InstanceDesc *idp;
	//if( idp=getCurrentInstanceDesc()){
		/* Add code here to get param -- delete idp if not needed *****/
		 *value = strdup("");
		return TRX_OK;
	//}
	//return TRX_ERR;
}
TRX_STATUS getWANDeviceWANDSLDiagnostics_QLNpsds(char **value)
{
	//InstanceDesc *idp;
	//if( idp=getCurrentInstanceDesc()){
		/* Add code here to get param -- delete idp if not needed *****/
		 *value = strdup("");
		return TRX_OK;
	//}
	//return TRX_ERR;
}
TRX_STATUS getWANDeviceWANDSLDiagnostics_SNRpsds(char **value)
{
	//InstanceDesc *idp;
	//if( idp=getCurrentInstanceDesc()){
		/* Add code here to get param -- delete idp if not needed *****/
		 *value = strdup("");
		return TRX_OK;
	//}
	//return TRX_ERR;
}
TRX_STATUS getWANDeviceWANDSLDiagnostics_BITSpsds(char **value)
{
	//InstanceDesc *idp;
	//if( idp=getCurrentInstanceDesc()){
		/* Add code here to get param -- delete idp if not needed *****/
		 *value = strdup("");
		return TRX_OK;
	//}
	//return TRX_ERR;
}
TRX_STATUS getWANDeviceWANDSLDiagnostics_GAINSpsds(char **value)
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
/** InternetGatewayDevice.WANDevice.WANConnectionDevice.WANDSLLinkConfig. */
/* InternetGatewayDevice.WANDevice.{i}.WANConnectionDevice.{i}.WANDSLLinkConfig. */

TRX_STATUS setDSLLinkEnable(char *value)
{
	return TRX_OK;
}
TRX_STATUS getDSLLinkEnable(char **value)
{
	return TRX_OK;
}
TRX_STATUS setDSLLinkType(char *value)
{
	// only supported PPPoE on release 06/30/05
	return TRX_OK;
}
TRX_STATUS getDSLLinkType(char **value)
{
	// only supported PPPoE on release 06/30/05
	*value = strdup("EoA");
	return TRX_OK;
}
TRX_STATUS setDSLLinkAddress(char *value)
{
	char *pChar = NULL;

	if ( value == NULL ) return TRX_ERR;
	// only support PVC type with format vpi/vci
	return TRX_OK;
	
}
TRX_STATUS getDSLLinkAddress(char **value)
{
	return TRX_OK;
}

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
TRX_STATUS getWANDeviceWANConnectionDeviceWANDSLLinkConfig_ModulationType(char **value)
{
	//InstanceDesc *idp;
	//if( idp=getCurrentInstanceDesc()){
		/* Add code here to get param -- delete idp if not needed *****/
		 *value = strdup("");
		return TRX_OK;
	//}
	//return TRX_ERR;
}


TRX_STATUS setATMEncap(char *value)
{
				return TRX_OK;
}
TRX_STATUS getATMEncap(char **value)
{
			return TRX_OK;
}
TRX_STATUS setATMQoS(char *value)
{
				return TRX_OK;
}
TRX_STATUS getATMQoS(char **value)
{
			return TRX_OK;
}
TRX_STATUS setATMPeakCellRate(char *value)
{
				return TRX_OK;
}
TRX_STATUS getATMPeakCellRate(char **value)
{
			return TRX_OK;
}
TRX_STATUS setATMMaximumBurstSize(char *value)
{
				return TRX_OK;
}
TRX_STATUS getATMMaximumBurstSize(char **value)
{
	return TRX_OK;
}
TRX_STATUS setATMSustainableCellRate(char *value)
{
	return TRX_OK;
}
TRX_STATUS getATMSustainableCellRate(char **value)
{
	return TRX_OK;
}


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
TRX_STATUS getWANDeviceWANConnectionDeviceWANDSLLinkConfig_ATMTransmittedBlocks(char **value)
{
	//InstanceDesc *idp;
	//if( idp=getCurrentInstanceDesc()){
		/* Add code here to get param -- delete idp if not needed *****/
		 *value = strdup("");
		return TRX_OK;
	//}
	//return TRX_ERR;
}
TRX_STATUS getWANDeviceWANConnectionDeviceWANDSLLinkConfig_ATMReceivedBlocks(char **value)
{
	//InstanceDesc *idp;
	//if( idp=getCurrentInstanceDesc()){
		/* Add code here to get param -- delete idp if not needed *****/
		 *value = strdup("");
		return TRX_OK;
	//}
	//return TRX_ERR;
}
TRX_STATUS getWANDeviceWANConnectionDeviceWANDSLLinkConfig_AAL5CRCErrors(char **value)
{
	//InstanceDesc *idp;
	//if( idp=getCurrentInstanceDesc()){
		/* Add code here to get param -- delete idp if not needed *****/
		 *value = strdup("");
		return TRX_OK;
	//}
	//return TRX_ERR;
}
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
TRX_STATUS getWANDeviceWANConnectionDeviceWANDSLLinkConfig_ATMHECErrors(char **value)
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
/** InternetGatewayDevice.WANDevice.WANConnectionDevice.WANATMF5LoopbackDiagnostics. */

TRX_STATUS setWANDeviceWANConnectionDeviceWANATMF5LoopbackDiagnostics_DiagnosticsState(char *value)
{
	//InstanceDesc *idp;
	//if( idp=getCurrentInstanceDesc()){
		/* Add code here to set param -- delete idp if not needed *****/
		return TRX_OK;
	//}
	//return TRX_ERR;
}
TRX_STATUS getWANDeviceWANConnectionDeviceWANATMF5LoopbackDiagnostics_DiagnosticsState(char **value)
{
	//InstanceDesc *idp;
	//if( idp=getCurrentInstanceDesc()){
		/* Add code here to get param -- delete idp if not needed *****/
		 *value = strdup("");
		return TRX_OK;
	//}
	//return TRX_ERR;
}
TRX_STATUS setWANDeviceWANConnectionDeviceWANATMF5LoopbackDiagnostics_NumberOfRepetitions(char *value)
{
	//InstanceDesc *idp;
	//if( idp=getCurrentInstanceDesc()){
		/* Add code here to set param -- delete idp if not needed *****/
		return TRX_OK;
	//}
	//return TRX_ERR;
}
TRX_STATUS getWANDeviceWANConnectionDeviceWANATMF5LoopbackDiagnostics_NumberOfRepetitions(char **value)
{
	//InstanceDesc *idp;
	//if( idp=getCurrentInstanceDesc()){
		/* Add code here to get param -- delete idp if not needed *****/
		 *value = strdup("");
		return TRX_OK;
	//}
	//return TRX_ERR;
}
TRX_STATUS setWANDeviceWANConnectionDeviceWANATMF5LoopbackDiagnostics_Timeout(char *value)
{
	//InstanceDesc *idp;
	//if( idp=getCurrentInstanceDesc()){
		/* Add code here to set param -- delete idp if not needed *****/
		return TRX_OK;
	//}
	//return TRX_ERR;
}
TRX_STATUS getWANDeviceWANConnectionDeviceWANATMF5LoopbackDiagnostics_Timeout(char **value)
{
	//InstanceDesc *idp;
	//if( idp=getCurrentInstanceDesc()){
		/* Add code here to get param -- delete idp if not needed *****/
		 *value = strdup("");
		return TRX_OK;
	//}
	//return TRX_ERR;
}
TRX_STATUS getWANDeviceWANConnectionDeviceWANATMF5LoopbackDiagnostics_SuccessCount(char **value)
{
	//InstanceDesc *idp;
	//if( idp=getCurrentInstanceDesc()){
		/* Add code here to get param -- delete idp if not needed *****/
		 *value = strdup("");
		return TRX_OK;
	//}
	//return TRX_ERR;
}
TRX_STATUS getWANDeviceWANConnectionDeviceWANATMF5LoopbackDiagnostics_FailureCount(char **value)
{
	//InstanceDesc *idp;
	//if( idp=getCurrentInstanceDesc()){
		/* Add code here to get param -- delete idp if not needed *****/
		 *value = strdup("");
		return TRX_OK;
	//}
	//return TRX_ERR;
}
TRX_STATUS getWANDeviceWANConnectionDeviceWANATMF5LoopbackDiagnostics_AverageResponseTime(char **value)
{
	//InstanceDesc *idp;
	//if( idp=getCurrentInstanceDesc()){
		/* Add code here to get param -- delete idp if not needed *****/
		 *value = strdup("");
		return TRX_OK;
	//}
	//return TRX_ERR;
}
TRX_STATUS getWANDeviceWANConnectionDeviceWANATMF5LoopbackDiagnostics_MinimumResponseTime(char **value)
{
	//InstanceDesc *idp;
	//if( idp=getCurrentInstanceDesc()){
		/* Add code here to get param -- delete idp if not needed *****/
		 *value = strdup("");
		return TRX_OK;
	//}
	//return TRX_ERR;
}
TRX_STATUS getWANDeviceWANConnectionDeviceWANATMF5LoopbackDiagnostics_MaximumResponseTime(char **value)
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

/** */
/** InternetGatewayDevice.WANDevice.WANConnectionDevice.WANIPConnection.PortMapping. */

TRX_STATUS setWANDeviceWANConnectionDeviceWANIPConnectionPortMapping_PortMappingEnabled(char *value)
{
	//InstanceDesc *idp;
	//if( idp=getCurrentInstanceDesc()){
		/* Add code here to set param -- delete idp if not needed *****/
		return TRX_OK;
	//}
	//return TRX_ERR;
}
TRX_STATUS getWANDeviceWANConnectionDeviceWANIPConnectionPortMapping_PortMappingEnabled(char **value)
{
	//InstanceDesc *idp;
	//if( idp=getCurrentInstanceDesc()){
		/* Add code here to get param -- delete idp if not needed *****/
		 *value = strdup("");
		return TRX_OK;
	//}
	//return TRX_ERR;
}
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
TRX_STATUS setWANDeviceWANConnectionDeviceWANIPConnectionPortMapping_ExternalPort(char *value)
{
	//InstanceDesc *idp;
	//if( idp=getCurrentInstanceDesc()){
		/* Add code here to set param -- delete idp if not needed *****/
		return TRX_OK;
	//}
	//return TRX_ERR;
}
TRX_STATUS getWANDeviceWANConnectionDeviceWANIPConnectionPortMapping_ExternalPort(char **value)
{
	//InstanceDesc *idp;
	//if( idp=getCurrentInstanceDesc()){
		/* Add code here to get param -- delete idp if not needed *****/
		 *value = strdup("");
		return TRX_OK;
	//}
	//return TRX_ERR;
}
TRX_STATUS setWANDeviceWANConnectionDeviceWANIPConnectionPortMapping_InternalPort(char *value)
{
	//InstanceDesc *idp;
	//if( idp=getCurrentInstanceDesc()){
		/* Add code here to set param -- delete idp if not needed *****/
		return TRX_OK;
	//}
	//return TRX_ERR;
}
TRX_STATUS getWANDeviceWANConnectionDeviceWANIPConnectionPortMapping_InternalPort(char **value)
{
	//InstanceDesc *idp;
	//if( idp=getCurrentInstanceDesc()){
		/* Add code here to get param -- delete idp if not needed *****/
		 *value = strdup("");
		return TRX_OK;
	//}
	//return TRX_ERR;
}
TRX_STATUS setWANDeviceWANConnectionDeviceWANIPConnectionPortMapping_PortMappingProtocol(char *value)
{
	//InstanceDesc *idp;
	//if( idp=getCurrentInstanceDesc()){
		/* Add code here to set param -- delete idp if not needed *****/
		return TRX_OK;
	//}
	//return TRX_ERR;
}
TRX_STATUS getWANDeviceWANConnectionDeviceWANIPConnectionPortMapping_PortMappingProtocol(char **value)
{
	//InstanceDesc *idp;
	//if( idp=getCurrentInstanceDesc()){
		/* Add code here to get param -- delete idp if not needed *****/
		 *value = strdup("");
		return TRX_OK;
	//}
	//return TRX_ERR;
}
TRX_STATUS setWANDeviceWANConnectionDeviceWANIPConnectionPortMapping_InternalClient(char *value)
{
	//InstanceDesc *idp;
	//if( idp=getCurrentInstanceDesc()){
		/* Add code here to set param -- delete idp if not needed *****/
		return TRX_OK;
	//}
	//return TRX_ERR;
}
TRX_STATUS getWANDeviceWANConnectionDeviceWANIPConnectionPortMapping_InternalClient(char **value)
{
	//InstanceDesc *idp;
	//if( idp=getCurrentInstanceDesc()){
		/* Add code here to get param -- delete idp if not needed *****/
		 *value = strdup("");
		return TRX_OK;
	//}
	//return TRX_ERR;
}
TRX_STATUS setWANDeviceWANConnectionDeviceWANIPConnectionPortMapping_PortMappingDescription(char *value)
{
	//InstanceDesc *idp;
	//if( idp=getCurrentInstanceDesc()){
		/* Add code here to set param -- delete idp if not needed *****/
		return TRX_OK;
	//}
	//return TRX_ERR;
}
TRX_STATUS getWANDeviceWANConnectionDeviceWANIPConnectionPortMapping_PortMappingDescription(char **value)
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
/** InternetGatewayDevice.WANDevice.WANConnectionDevice.WANIPConnection.PortMapping. */

TRX_STATUS addWANDeviceWANConnectionDeviceWANIPConnectionPortMappingI(char **value)
{
	InstanceDesc *idp;
	if( idp=getCurrentInstanceDesc()){
		/* Add code here to create new instance************/
		return TRX_OK;
	}
	return TRX_ERR;
}
TRX_STATUS delWANDeviceWANConnectionDeviceWANIPConnectionPortMappingI(char *value)
{
	InstanceDesc *idp;
	if (idp= getNewInstanceDesc( getCurrentNode(), NULL, 0)){
		/* Add code here to delete instance ****************/
		return TRX_OK;
	}
	return TRX_ERR;
}

/** */
/** InternetGatewayDevice.WANDevice.WANConnectionDevice.WANIPConnection.Stats. */

TRX_STATUS getWANDeviceWANConnectionDeviceWANIPConnectionStats_EthernetBytesSent(char **value)
{
	//InstanceDesc *idp;
	//if( idp=getCurrentInstanceDesc()){
		/* Add code here to get param -- delete idp if not needed *****/
		 *value = strdup("");
		return TRX_OK;
	//}
	//return TRX_ERR;
}
TRX_STATUS getWANDeviceWANConnectionDeviceWANIPConnectionStats_EthernetBytesReceived(char **value)
{
	//InstanceDesc *idp;
	//if( idp=getCurrentInstanceDesc()){
		/* Add code here to get param -- delete idp if not needed *****/
		 *value = strdup("");
		return TRX_OK;
	//}
	//return TRX_ERR;
}
TRX_STATUS getWANDeviceWANConnectionDeviceWANIPConnectionStats_EthernetPacketsSent(char **value)
{
	//InstanceDesc *idp;
	//if( idp=getCurrentInstanceDesc()){
		/* Add code here to get param -- delete idp if not needed *****/
		 *value = strdup("");
		return TRX_OK;
	//}
	//return TRX_ERR;
}
TRX_STATUS getWANDeviceWANConnectionDeviceWANIPConnectionStats_EthernetPacketsReceived(char **value)
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
/** InternetGatewayDevice.WANDevice.WANConnectionDevice.WANIPConnection. */

TRX_STATUS setWANDeviceWANConnectionDeviceWANIPConnection_Enable(char *value)
{
	//InstanceDesc *idp;
	//if( idp=getCurrentInstanceDesc()){
		/* Add code here to set param -- delete idp if not needed *****/
		return TRX_OK;
	//}
	//return TRX_ERR;
}
TRX_STATUS getWANDeviceWANConnectionDeviceWANIPConnection_Enable(char **value)
{
	//InstanceDesc *idp;
	//if( idp=getCurrentInstanceDesc()){
		/* Add code here to get param -- delete idp if not needed *****/
		 *value = strdup("");
		return TRX_OK;
	//}
	//return TRX_ERR;
}
TRX_STATUS getWANDeviceWANConnectionDeviceWANIPConnection_ConnectionStatus(char **value)
{
	//InstanceDesc *idp;
	//if( idp=getCurrentInstanceDesc()){
		/* Add code here to get param -- delete idp if not needed *****/
		 *value = strdup("");
		return TRX_OK;
	//}
	//return TRX_ERR;
}
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
TRX_STATUS setWANDeviceWANConnectionDeviceWANIPConnection_ConnectionType(char *value)
{
	//InstanceDesc *idp;
	//if( idp=getCurrentInstanceDesc()){
		/* Add code here to set param -- delete idp if not needed *****/
		return TRX_OK;
	//}
	//return TRX_ERR;
}
TRX_STATUS getWANDeviceWANConnectionDeviceWANIPConnection_ConnectionType(char **value)
{
	//InstanceDesc *idp;
	//if( idp=getCurrentInstanceDesc()){
		/* Add code here to get param -- delete idp if not needed *****/
		 *value = strdup("");
		return TRX_OK;
	//}
	//return TRX_ERR;
}
TRX_STATUS setWANDeviceWANConnectionDeviceWANIPConnection_Name(char *value)
{
	//InstanceDesc *idp;
	//if( idp=getCurrentInstanceDesc()){
		/* Add code here to set param -- delete idp if not needed *****/
		return TRX_OK;
	//}
	//return TRX_ERR;
}
TRX_STATUS getWANDeviceWANConnectionDeviceWANIPConnection_Name(char **value)
{
	//InstanceDesc *idp;
	//if( idp=getCurrentInstanceDesc()){
		/* Add code here to get param -- delete idp if not needed *****/
		 *value = strdup("");
		return TRX_OK;
	//}
	//return TRX_ERR;
}
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
TRX_STATUS setWANDeviceWANConnectionDeviceWANIPConnection_NATEnabled(char *value)
{
	//InstanceDesc *idp;
	//if( idp=getCurrentInstanceDesc()){
		/* Add code here to set param -- delete idp if not needed *****/
		return TRX_OK;
	//}
	//return TRX_ERR;
}
TRX_STATUS getWANDeviceWANConnectionDeviceWANIPConnection_NATEnabled(char **value)
{
	//InstanceDesc *idp;
	//if( idp=getCurrentInstanceDesc()){
		/* Add code here to get param -- delete idp if not needed *****/
		 *value = strdup("");
		return TRX_OK;
	//}
	//return TRX_ERR;
}
TRX_STATUS setWANDeviceWANConnectionDeviceWANIPConnection_AddressingType(char *value)
{
	//InstanceDesc *idp;
	//if( idp=getCurrentInstanceDesc()){
		/* Add code here to set param -- delete idp if not needed *****/
		return TRX_OK;
	//}
	//return TRX_ERR;
}
TRX_STATUS getWANDeviceWANConnectionDeviceWANIPConnection_AddressingType(char **value)
{
	//InstanceDesc *idp;
	//if( idp=getCurrentInstanceDesc()){
		/* Add code here to get param -- delete idp if not needed *****/
		 *value = strdup("");
		return TRX_OK;
	//}
	//return TRX_ERR;
}
TRX_STATUS setWANDeviceWANConnectionDeviceWANIPConnection_ExternalIPAddress(char *value)
{
	//InstanceDesc *idp;
	//if( idp=getCurrentInstanceDesc()){
		/* Add code here to set param -- delete idp if not needed *****/
		return TRX_OK;
	//}
	//return TRX_ERR;
}
TRX_STATUS getWANDeviceWANConnectionDeviceWANIPConnection_ExternalIPAddress(char **value)
{
	//InstanceDesc *idp;
	//if( idp=getCurrentInstanceDesc()){
		/* Add code here to get param -- delete idp if not needed *****/
		 *value = strdup("68.98.207.27");
		return TRX_OK;
	//}
	//return TRX_ERR;
}
TRX_STATUS setWANDeviceWANConnectionDeviceWANIPConnection_SubnetMask(char *value)
{
	//InstanceDesc *idp;
	//if( idp=getCurrentInstanceDesc()){
		/* Add code here to set param -- delete idp if not needed *****/
		return TRX_OK;
	//}
	//return TRX_ERR;
}
TRX_STATUS getWANDeviceWANConnectionDeviceWANIPConnection_SubnetMask(char **value)
{
	//InstanceDesc *idp;
	//if( idp=getCurrentInstanceDesc()){
		/* Add code here to get param -- delete idp if not needed *****/
		 *value = strdup("");
		return TRX_OK;
	//}
	//return TRX_ERR;
}
TRX_STATUS setWANDeviceWANConnectionDeviceWANIPConnection_DefaultGateway(char *value)
{
	//InstanceDesc *idp;
	//if( idp=getCurrentInstanceDesc()){
		/* Add code here to set param -- delete idp if not needed *****/
		return TRX_OK;
	//}
	//return TRX_ERR;
}
TRX_STATUS getWANDeviceWANConnectionDeviceWANIPConnection_DefaultGateway(char **value)
{
	//InstanceDesc *idp;
	//if( idp=getCurrentInstanceDesc()){
		/* Add code here to get param -- delete idp if not needed *****/
		 *value = strdup("");
		return TRX_OK;
	//}
	//return TRX_ERR;
}
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
TRX_STATUS setWANDeviceWANConnectionDeviceWANIPConnection_DNSServers(char *value)
{
	//InstanceDesc *idp;
	//if( idp=getCurrentInstanceDesc()){
		/* Add code here to set param -- delete idp if not needed *****/
		return TRX_OK;
	//}
	//return TRX_ERR;
}
TRX_STATUS getWANDeviceWANConnectionDeviceWANIPConnection_DNSServers(char **value)
{
	//InstanceDesc *idp;
	//if( idp=getCurrentInstanceDesc()){
		/* Add code here to get param -- delete idp if not needed *****/
		 *value = strdup("");
		return TRX_OK;
	//}
	//return TRX_ERR;
}
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
TRX_STATUS setWANDeviceWANConnectionDeviceWANIPConnection_ConnectionTrigger(char *value)
{
	//InstanceDesc *idp;
	//if( idp=getCurrentInstanceDesc()){
		/* Add code here to set param -- delete idp if not needed *****/
		return TRX_OK;
	//}
	//return TRX_ERR;
}
TRX_STATUS getWANDeviceWANConnectionDeviceWANIPConnection_ConnectionTrigger(char **value)
{
	//InstanceDesc *idp;
	//if( idp=getCurrentInstanceDesc()){
		/* Add code here to get param -- delete idp if not needed *****/
		 *value = strdup("");
		return TRX_OK;
	//}
	//return TRX_ERR;
}
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
TRX_STATUS getWANDeviceWANConnectionDeviceWANIPConnection_PortMappingNumberOfEntries(char **value)
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
/** InternetGatewayDevice.WANDevice.WANConnectionDevice.WANIPConnection. */

TRX_STATUS addWANDeviceWANConnectionDeviceWANIPConnectionI(char **value)
{
	InstanceDesc *idp;
	if( idp=getCurrentInstanceDesc()){
		/* Add code here to create new instance************/
		return TRX_OK;
	}
	return TRX_ERR;
}
TRX_STATUS delWANDeviceWANConnectionDeviceWANIPConnectionI(char *value)
{
	InstanceDesc *idp;
	if (idp= getNewInstanceDesc( getCurrentNode(), NULL, 0)){
		/* Add code here to delete instance ****************/
		return TRX_OK;
	}
	return TRX_ERR;
}

/** */
/** InternetGatewayDevice.WANDevice.WANConnectionDevice.WANPPPConnection.PortMapping. */

TRX_STATUS setWANDeviceWANConnectionDeviceWANPPPConnectionPortMapping_PortMappingEnabled(char *value)
{
	//InstanceDesc *idp;
	//if( idp=getCurrentInstanceDesc()){
		/* Add code here to set param -- delete idp if not needed *****/
		return TRX_OK;
	//}
	//return TRX_ERR;
}
TRX_STATUS getWANDeviceWANConnectionDeviceWANPPPConnectionPortMapping_PortMappingEnabled(char **value)
{
	//InstanceDesc *idp;
	//if( idp=getCurrentInstanceDesc()){
		/* Add code here to get param -- delete idp if not needed *****/
		 *value = strdup("");
		return TRX_OK;
	//}
	//return TRX_ERR;
}
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
TRX_STATUS setWANDeviceWANConnectionDeviceWANPPPConnectionPortMapping_ExternalPort(char *value)
{
	//InstanceDesc *idp;
	//if( idp=getCurrentInstanceDesc()){
		/* Add code here to set param -- delete idp if not needed *****/
		return TRX_OK;
	//}
	//return TRX_ERR;
}
TRX_STATUS getWANDeviceWANConnectionDeviceWANPPPConnectionPortMapping_ExternalPort(char **value)
{
	//InstanceDesc *idp;
	//if( idp=getCurrentInstanceDesc()){
		/* Add code here to get param -- delete idp if not needed *****/
		 *value = strdup("");
		return TRX_OK;
	//}
	//return TRX_ERR;
}
TRX_STATUS setWANDeviceWANConnectionDeviceWANPPPConnectionPortMapping_InternalPort(char *value)
{
	//InstanceDesc *idp;
	//if( idp=getCurrentInstanceDesc()){
		/* Add code here to set param -- delete idp if not needed *****/
		return TRX_OK;
	//}
	//return TRX_ERR;
}
TRX_STATUS getWANDeviceWANConnectionDeviceWANPPPConnectionPortMapping_InternalPort(char **value)
{
	//InstanceDesc *idp;
	//if( idp=getCurrentInstanceDesc()){
		/* Add code here to get param -- delete idp if not needed *****/
		 *value = strdup("");
		return TRX_OK;
	//}
	//return TRX_ERR;
}
TRX_STATUS setWANDeviceWANConnectionDeviceWANPPPConnectionPortMapping_PortMappingProtocol(char *value)
{
	//InstanceDesc *idp;
	//if( idp=getCurrentInstanceDesc()){
		/* Add code here to set param -- delete idp if not needed *****/
		return TRX_OK;
	//}
	//return TRX_ERR;
}
TRX_STATUS getWANDeviceWANConnectionDeviceWANPPPConnectionPortMapping_PortMappingProtocol(char **value)
{
	//InstanceDesc *idp;
	//if( idp=getCurrentInstanceDesc()){
		/* Add code here to get param -- delete idp if not needed *****/
		 *value = strdup("");
		return TRX_OK;
	//}
	//return TRX_ERR;
}
TRX_STATUS setWANDeviceWANConnectionDeviceWANPPPConnectionPortMapping_InternalClient(char *value)
{
	//InstanceDesc *idp;
	//if( idp=getCurrentInstanceDesc()){
		/* Add code here to set param -- delete idp if not needed *****/
		return TRX_OK;
	//}
	//return TRX_ERR;
}
TRX_STATUS getWANDeviceWANConnectionDeviceWANPPPConnectionPortMapping_InternalClient(char **value)
{
	//InstanceDesc *idp;
	//if( idp=getCurrentInstanceDesc()){
		/* Add code here to get param -- delete idp if not needed *****/
		 *value = strdup("");
		return TRX_OK;
	//}
	//return TRX_ERR;
}
TRX_STATUS setWANDeviceWANConnectionDeviceWANPPPConnectionPortMapping_PortMappingDescription(char *value)
{
	//InstanceDesc *idp;
	//if( idp=getCurrentInstanceDesc()){
		/* Add code here to set param -- delete idp if not needed *****/
		return TRX_OK;
	//}
	//return TRX_ERR;
}
TRX_STATUS getWANDeviceWANConnectionDeviceWANPPPConnectionPortMapping_PortMappingDescription(char **value)
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
/** InternetGatewayDevice.WANDevice.WANConnectionDevice.WANPPPConnection.PortMapping. */

TRX_STATUS addWANDeviceWANConnectionDeviceWANPPPConnectionPortMappingI(char **value)
{
	InstanceDesc *idp;
	if( idp=getCurrentInstanceDesc()){
		/* Add code here to create new instance************/
		return TRX_OK;
	}
	return TRX_ERR;
}
TRX_STATUS delWANDeviceWANConnectionDeviceWANPPPConnectionPortMappingI(char *value)
{
	InstanceDesc *idp;
	if (idp= getNewInstanceDesc( getCurrentNode(), NULL, 0)){
		/* Add code here to delete instance ****************/
		return TRX_OK;
	}
	return TRX_ERR;
}

/** */
/** InternetGatewayDevice.WANDevice.WANConnectionDevice.WANPPPConnection.Stats. */

TRX_STATUS getWANDeviceWANConnectionDeviceWANPPPConnectionStats_EthernetBytesSent(char **value)
{
	//InstanceDesc *idp;
	//if( idp=getCurrentInstanceDesc()){
		/* Add code here to get param -- delete idp if not needed *****/
		 *value = strdup("");
		return TRX_OK;
	//}
	//return TRX_ERR;
}
TRX_STATUS getWANDeviceWANConnectionDeviceWANPPPConnectionStats_EthernetBytesReceived(char **value)
{
	//InstanceDesc *idp;
	//if( idp=getCurrentInstanceDesc()){
		/* Add code here to get param -- delete idp if not needed *****/
		 *value = strdup("");
		return TRX_OK;
	//}
	//return TRX_ERR;
}
TRX_STATUS getWANDeviceWANConnectionDeviceWANPPPConnectionStats_EthernetPacketsSent(char **value)
{
	//InstanceDesc *idp;
	//if( idp=getCurrentInstanceDesc()){
		/* Add code here to get param -- delete idp if not needed *****/
		 *value = strdup("");
		return TRX_OK;
	//}
	//return TRX_ERR;
}
TRX_STATUS getWANDeviceWANConnectionDeviceWANPPPConnectionStats_EthernetPacketsReceived(char **value)
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
/** InternetGatewayDevice.WANDevice.WANConnectionDevice.WANPPPConnection. */
/* InternetGatewayDevice.WANDevice.{i}.WANConnectionDevice.{i}.WANPPPConnection.{i}. */

TRX_STATUS setPPPEnable(char *value)
{
					return TRX_OK;
		
	return TRX_ERR;
}
TRX_STATUS getPPPEnable(char **value)
{
   			return TRX_OK;
}
TRX_STATUS getPPPStatus(char **value)
{
	return TRX_OK;
}

TRX_STATUS setPPPType(char *value)
{
	// only supported "IP_Routed" type for PPPoE and PPPoA
	// see chapter 2 in provisioning_option_3.doc from Pirelli
	return TRX_OK;
}
TRX_STATUS getPPPType(char **value)
{
	// only supported "IP_Routed" type for PPPoE and PPPoA
	// see chapter 2 in provisioning_option_3.doc from Pirelli
	*value = strdup("IP_Routed");
	return TRX_OK;
}
TRX_STATUS setPPPName(char *value)
{
	return TRX_OK;
}
TRX_STATUS getPPPName(char **value)
{
	return TRX_OK;
}
TRX_STATUS setPPPIdleTime(char *value)
{
	return TRX_OK;
}
TRX_STATUS getPPPIdleTime(char **value)
{
	return TRX_OK;
}
TRX_STATUS setPPPNATEnabled(char *value)
{
	// nat enable is not supported yet
	return TRX_OK;
}
TRX_STATUS getPPPNATEnabled(char **value)
{
	// nat enable is not supported yet
	*value = strdup("");
	return TRX_OK;
}
TRX_STATUS setPPPUsername(char *value)
{
	return TRX_OK;
}
TRX_STATUS getPPPUsername(char **value)
{
	return TRX_OK;
}
TRX_STATUS setPPPPassword(char *value)
{
	return TRX_OK;

	return TRX_ERR;
}
TRX_STATUS getPPPPassword(char **value)
{
	return TRX_OK;
}
TRX_STATUS getWanPPPExtIP(char **value)
{
	*value=strdup("68.98.207.260");
   	return TRX_OK;
}
TRX_STATUS setPPPDNSServers(char *value)
{
				return TRX_OK;
	}
TRX_STATUS getPPPDNSServers(char **value)
{
			return TRX_OK;
	}
TRX_STATUS setPPPConTrigger(char *value)
{
				return TRX_OK;
  }
TRX_STATUS getPPPConTrigger(char **value)
{
	InstanceDesc *idp;

			return TRX_OK;

	return TRX_ERR;
}
TRX_STATUS getWANDeviceWANConnectionDeviceWANPPPConnection_ConnectionStatus(char **value)
{
	//InstanceDesc *idp;
	//if( idp=getCurrentInstanceDesc()){
		/* Add code here to get param -- delete idp if not needed *****/
		 *value = strdup("");
		return TRX_OK;
	//}
	//return TRX_ERR;
}
TRX_STATUS getWANDeviceWANConnectionDeviceWANPPPConnection_PossibleConnectionTypes(char **value)
{
	//InstanceDesc *idp;
	//if( idp=getCurrentInstanceDesc()){
		/* Add code here to get param -- delete idp if not needed *****/
		 *value = strdup("");
		return TRX_OK;
	//}
	//return TRX_ERR;
}
TRX_STATUS getWANDeviceWANConnectionDeviceWANPPPConnection_Uptime(char **value)
{
	//InstanceDesc *idp;
	//if( idp=getCurrentInstanceDesc()){
		/* Add code here to get param -- delete idp if not needed *****/
		 *value = strdup("");
		return TRX_OK;
	//}
	//return TRX_ERR;
}
TRX_STATUS getWANDeviceWANConnectionDeviceWANPPPConnection_LastConnectionError(char **value)
{
	//InstanceDesc *idp;
	//if( idp=getCurrentInstanceDesc()){
		/* Add code here to get param -- delete idp if not needed *****/
		 *value = strdup("");
		return TRX_OK;
	//}
	//return TRX_ERR;
}
TRX_STATUS setWANDeviceWANConnectionDeviceWANPPPConnection_AutoDisconnectTime(char *value)
{
	//InstanceDesc *idp;
	//if( idp=getCurrentInstanceDesc()){
		/* Add code here to set param -- delete idp if not needed *****/
		return TRX_OK;
	//}
	//return TRX_ERR;
}
TRX_STATUS getWANDeviceWANConnectionDeviceWANPPPConnection_AutoDisconnectTime(char **value)
{
	//InstanceDesc *idp;
	//if( idp=getCurrentInstanceDesc()){
		/* Add code here to get param -- delete idp if not needed *****/
		 *value = strdup("");
		return TRX_OK;
	//}
	//return TRX_ERR;
}
TRX_STATUS setWANDeviceWANConnectionDeviceWANPPPConnection_WarnDisconnectDelay(char *value)
{
	//InstanceDesc *idp;
	//if( idp=getCurrentInstanceDesc()){
		/* Add code here to set param -- delete idp if not needed *****/
		return TRX_OK;
	//}
	//return TRX_ERR;
}
TRX_STATUS getWANDeviceWANConnectionDeviceWANPPPConnection_WarnDisconnectDelay(char **value)
{
	//InstanceDesc *idp;
	//if( idp=getCurrentInstanceDesc()){
		/* Add code here to get param -- delete idp if not needed *****/
		 *value = strdup("");
		return TRX_OK;
	//}
	//return TRX_ERR;
}
TRX_STATUS getWANDeviceWANConnectionDeviceWANPPPConnection_RSIPAvailable(char **value)
{
	//InstanceDesc *idp;
	//if( idp=getCurrentInstanceDesc()){
		/* Add code here to get param -- delete idp if not needed *****/
		 *value = strdup("");
		return TRX_OK;
	//}
	//return TRX_ERR;
}
TRX_STATUS setWANDeviceWANConnectionDeviceWANPPPConnection_NATEnabled(char *value)
{
	//InstanceDesc *idp;
	//if( idp=getCurrentInstanceDesc()){
		/* Add code here to set param -- delete idp if not needed *****/
		return TRX_OK;
	//}
	//return TRX_ERR;
}
TRX_STATUS getWANDeviceWANConnectionDeviceWANPPPConnection_NATEnabled(char **value)
{
	//InstanceDesc *idp;
	//if( idp=getCurrentInstanceDesc()){
		/* Add code here to get param -- delete idp if not needed *****/
		 *value = strdup("");
		return TRX_OK;
	//}
	//return TRX_ERR;
}
TRX_STATUS setWANDeviceWANConnectionDeviceWANPPPConnection_Username(char *value)
{
	//InstanceDesc *idp;
	//if( idp=getCurrentInstanceDesc()){
		/* Add code here to set param -- delete idp if not needed *****/
		return TRX_OK;
	//}
	//return TRX_ERR;
}
TRX_STATUS getWANDeviceWANConnectionDeviceWANPPPConnection_PPPEncryptionProtocol(char **value)
{
	//InstanceDesc *idp;
	//if( idp=getCurrentInstanceDesc()){
		/* Add code here to get param -- delete idp if not needed *****/
		 *value = strdup("");
		return TRX_OK;
	//}
	//return TRX_ERR;
}
TRX_STATUS getWANDeviceWANConnectionDeviceWANPPPConnection_PPPCompressionProtocol(char **value)
{
	//InstanceDesc *idp;
	//if( idp=getCurrentInstanceDesc()){
		/* Add code here to get param -- delete idp if not needed *****/
		 *value = strdup("");
		return TRX_OK;
	//}
	//return TRX_ERR;
}
TRX_STATUS getWANDeviceWANConnectionDeviceWANPPPConnection_PPPAuthenticationProtocol(char **value)
{
	//InstanceDesc *idp;
	//if( idp=getCurrentInstanceDesc()){
		/* Add code here to get param -- delete idp if not needed *****/
		 *value = strdup("");
		return TRX_OK;
	//}
	//return TRX_ERR;
}
TRX_STATUS getWANDeviceWANConnectionDeviceWANPPPConnection_RemoteIPAddress(char **value)
{
	//InstanceDesc *idp;
	//if( idp=getCurrentInstanceDesc()){
		/* Add code here to get param -- delete idp if not needed *****/
		 *value = strdup("");
		return TRX_OK;
	//}
	//return TRX_ERR;
}
TRX_STATUS setWANDeviceWANConnectionDeviceWANPPPConnection_MaxMRUSize(char *value)
{
	//InstanceDesc *idp;
	//if( idp=getCurrentInstanceDesc()){
		/* Add code here to set param -- delete idp if not needed *****/
		return TRX_OK;
	//}
	//return TRX_ERR;
}
TRX_STATUS getWANDeviceWANConnectionDeviceWANPPPConnection_MaxMRUSize(char **value)
{
	//InstanceDesc *idp;
	//if( idp=getCurrentInstanceDesc()){
		/* Add code here to get param -- delete idp if not needed *****/
		 *value = strdup("");
		return TRX_OK;
	//}
	//return TRX_ERR;
}
TRX_STATUS getWANDeviceWANConnectionDeviceWANPPPConnection_CurrentMRUSize(char **value)
{
	//InstanceDesc *idp;
	//if( idp=getCurrentInstanceDesc()){
		/* Add code here to get param -- delete idp if not needed *****/
		 *value = strdup("");
		return TRX_OK;
	//}
	//return TRX_ERR;
}
TRX_STATUS setWANDeviceWANConnectionDeviceWANPPPConnection_DNSEnabled(char *value)
{
	//InstanceDesc *idp;
	//if( idp=getCurrentInstanceDesc()){
		/* Add code here to set param -- delete idp if not needed *****/
		return TRX_OK;
	//}
	//return TRX_ERR;
}
TRX_STATUS getWANDeviceWANConnectionDeviceWANPPPConnection_DNSEnabled(char **value)
{
	//InstanceDesc *idp;
	//if( idp=getCurrentInstanceDesc()){
		/* Add code here to get param -- delete idp if not needed *****/
		 *value = strdup("");
		return TRX_OK;
	//}
	//return TRX_ERR;
}
TRX_STATUS setWANDeviceWANConnectionDeviceWANPPPConnection_DNSOverrideAllowed(char *value)
{
	//InstanceDesc *idp;
	//if( idp=getCurrentInstanceDesc()){
		/* Add code here to set param -- delete idp if not needed *****/
		return TRX_OK;
	//}
	//return TRX_ERR;
}
TRX_STATUS getWANDeviceWANConnectionDeviceWANPPPConnection_DNSOverrideAllowed(char **value)
{
	//InstanceDesc *idp;
	//if( idp=getCurrentInstanceDesc()){
		/* Add code here to get param -- delete idp if not needed *****/
		 *value = strdup("");
		return TRX_OK;
	//}
	//return TRX_ERR;
}
TRX_STATUS setWANDeviceWANConnectionDeviceWANPPPConnection_MACAddress(char *value)
{
	//InstanceDesc *idp;
	//if( idp=getCurrentInstanceDesc()){
		/* Add code here to set param -- delete idp if not needed *****/
		return TRX_OK;
	//}
	//return TRX_ERR;
}
TRX_STATUS getWANDeviceWANConnectionDeviceWANPPPConnection_MACAddress(char **value)
{
	//InstanceDesc *idp;
	//if( idp=getCurrentInstanceDesc()){
		/* Add code here to get param -- delete idp if not needed *****/
		 *value = strdup("");
		return TRX_OK;
	//}
	//return TRX_ERR;
}
TRX_STATUS setWANDeviceWANConnectionDeviceWANPPPConnection_MACAddressOverride(char *value)
{
	//InstanceDesc *idp;
	//if( idp=getCurrentInstanceDesc()){
		/* Add code here to set param -- delete idp if not needed *****/
		return TRX_OK;
	//}
	//return TRX_ERR;
}
TRX_STATUS getWANDeviceWANConnectionDeviceWANPPPConnection_MACAddressOverride(char **value)
{
	//InstanceDesc *idp;
	//if( idp=getCurrentInstanceDesc()){
		/* Add code here to get param -- delete idp if not needed *****/
		 *value = strdup("");
		return TRX_OK;
	//}
	//return TRX_ERR;
}
TRX_STATUS getWANDeviceWANConnectionDeviceWANPPPConnection_TransportType(char **value)
{
	//InstanceDesc *idp;
	//if( idp=getCurrentInstanceDesc()){
		/* Add code here to get param -- delete idp if not needed *****/
		 *value = strdup("");
		return TRX_OK;
	//}
	//return TRX_ERR;
}
TRX_STATUS setWANDeviceWANConnectionDeviceWANPPPConnection_PPPoEACName(char *value)
{
	//InstanceDesc *idp;
	//if( idp=getCurrentInstanceDesc()){
		/* Add code here to set param -- delete idp if not needed *****/
		return TRX_OK;
	//}
	//return TRX_ERR;
}
TRX_STATUS getWANDeviceWANConnectionDeviceWANPPPConnection_PPPoEACName(char **value)
{
	//InstanceDesc *idp;
	//if( idp=getCurrentInstanceDesc()){
		/* Add code here to get param -- delete idp if not needed *****/
		 *value = strdup("");
		return TRX_OK;
	//}
	//return TRX_ERR;
}
TRX_STATUS setWANDeviceWANConnectionDeviceWANPPPConnection_PPPoEServiceName(char *value)
{
	//InstanceDesc *idp;
	//if( idp=getCurrentInstanceDesc()){
		/* Add code here to set param -- delete idp if not needed *****/
		return TRX_OK;
	//}
	//return TRX_ERR;
}
TRX_STATUS getWANDeviceWANConnectionDeviceWANPPPConnection_PPPoEServiceName(char **value)
{
	//InstanceDesc *idp;
	//if( idp=getCurrentInstanceDesc()){
		/* Add code here to get param -- delete idp if not needed *****/
		 *value = strdup("");
		return TRX_OK;
	//}
	//return TRX_ERR;
}
TRX_STATUS setWANDeviceWANConnectionDeviceWANPPPConnection_RouteProtocolRx(char *value)
{
	//InstanceDesc *idp;
	//if( idp=getCurrentInstanceDesc()){
		/* Add code here to set param -- delete idp if not needed *****/
		return TRX_OK;
	//}
	//return TRX_ERR;
}
TRX_STATUS getWANDeviceWANConnectionDeviceWANPPPConnection_RouteProtocolRx(char **value)
{
	//InstanceDesc *idp;
	//if( idp=getCurrentInstanceDesc()){
		/* Add code here to get param -- delete idp if not needed *****/
		 *value = strdup("");
		return TRX_OK;
	//}
	//return TRX_ERR;
}
TRX_STATUS getWANDeviceWANConnectionDeviceWANPPPConnection_PPPLCPEcho(char **value)
{
	//InstanceDesc *idp;
	//if( idp=getCurrentInstanceDesc()){
		/* Add code here to get param -- delete idp if not needed *****/
		 *value = strdup("");
		return TRX_OK;
	//}
	//return TRX_ERR;
}
TRX_STATUS getWANDeviceWANConnectionDeviceWANPPPConnection_PPPLCPEchoRetry(char **value)
{
	//InstanceDesc *idp;
	//if( idp=getCurrentInstanceDesc()){
		/* Add code here to get param -- delete idp if not needed *****/
		 *value = strdup("");
		return TRX_OK;
	//}
	//return TRX_ERR;
}
TRX_STATUS getWANDeviceWANConnectionDeviceWANPPPConnection_PortMappingNumberOfEntries(char **value)
{
	//InstanceDesc *idp;
	//if( idp=getCurrentInstanceDesc()){
		/* Add code here to get param -- delete idp if not needed *****/
		 *value = strdup("");
		return TRX_OK;
	//}
	//return TRX_ERR;
}

TRX_STATUS setWanPPPAddressingType(char *value)
{
	return TRX_OK;
}
TRX_STATUS getWanPPPAddressingType(char **value)
{
	*value=strdup("testreturnvalue");
	return TRX_OK;
}

TRX_STATUS setWanPPPExtIP(char *value)
{
				return TRX_OK;
}

TRX_STATUS addPPPConnection(char **value)
{
    InstanceDesc *idp;
    if ( idp= getNewInstanceDesc( getCurrentNode(),getCurrentInstanceDesc(),  0) ) {
        *value = strdup(itoa(idp->instanceID));
        return TRX_REBOOT; /* or TRX_BOOT if necessary */
    }
    return TRX_ERR;
}

TRX_STATUS deletePPPConnection(char *value)
{
	TRxObjNode *n = getCurrentNode();
	InstanceDesc *idp;
	int     id = atoi(value);
	if ( !deleteInstanceDesc(n,id )){
		fprintf(stderr, "Delete PPPConnection instance = %d\n", id);
		return TRX_REBOOT; /* or TRX_BOOT if necessary */
	}
	return TRX_ERR;
}
/* InternetGatewayDevice.WANDevice.{i}.WANConnectionDevice.{i}. */
TRX_STATUS addWANConnectionDevice(char **value)
{
	InstanceDesc *idp;
	if ( idp= getNewInstanceDesc( getCurrentNode(),getCurrentInstanceDesc(),  0) ) {
        *value = strdup(itoa(idp->instanceID));
        return TRX_REBOOT; /* or TRX_BOOT if necessary */
    }
    return TRX_ERR;
}

extern TRxObjNode  WANDeviceWANConnectionDeviceWANPPPConnectionInstanceDesc[];
extern TRxObjNode WANDeviceWANConnectionDeviceWANIPConnectionInstanceDesc[];
TRX_STATUS deleteWANConnectionDevice(char *value)
{
	TRxObjNode *n = getCurrentNode();
	InstanceDesc *idp;
	int     id = atoi(value);
	
	if ( getInstanceCount(WANDeviceWANConnectionDeviceWANPPPConnectionInstanceDesc)== 0
		 && getInstanceCount(WANDeviceWANConnectionDeviceWANIPConnectionInstanceDesc)==0 ) {
		fprintf(stderr, "Child object NOT DELETED\n");
	}
	if (!deleteInstanceDesc(n,id)){
		fprintf(stderr, "Delete WANConnectionDevice Instance = %d\n", id);
		return TRX_REBOOT; /* or TRX_BOOT if necessary */
	}
	return TRX_ERR;
}

TRX_STATUS getPPPConNumberEntries(char **value)
{
	*value = strdup( itoa(getInstanceCount(WANDeviceWANConnectionDeviceWANPPPConnectionInstanceDesc)));
	return TRX_OK;
}

TRX_STATUS getWANDeviceWANConnectionDevice_WANIPConnectionNumberOfEntries(char **value)
{
	*value = strdup( itoa(getInstanceCount(WANDeviceWANConnectionDeviceWANIPConnectionInstanceDesc)));
	return TRX_OK;
}
/* InternetGatewayDevice.WANDevice.{i}. */
extern TRxObjNode WANDeviceWANConnectionDeviceInstanceDesc[];
TRX_STATUS getWANDevice_WANConnectionNumberOfEntries(char **value)
{
	int num = getInstanceCount( WANDeviceWANConnectionDeviceInstanceDesc);
	*value = strdup( itoa(num));
	return TRX_OK;
}




TRX_STATUS pseudoGetIFName(char **value)
{
	return TRX_ERR;

}

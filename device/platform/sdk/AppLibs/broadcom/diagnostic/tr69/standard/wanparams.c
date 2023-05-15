/*****************************************************************************
//
//  Copyright (c) 2005  Broadcom Corporation
//  All Rights Reserved
//  No portions of this material may be reproduced in any form without the
//  written permission of:
//          Broadcom Corporation
//          16215 Alton Parkway
//          Irvine, California 92619
//  All information contained in this document is Broadcom Corporation
//  company private, proprietary, and trade secret.
//
******************************************************************************
//
//  Filename:       wanparams.c
//
******************************************************************************
//  Description:
//             WANDevice profile supported objects
//
*****************************************************************************/

#include "sharedparams.h"
#include "wanparams.h" 

#if 0
/** */
/** InternetGatewayDevice.WANDevice.WANCommonInterfaceConfig.Connection. */

TRXGFUNC(getWANDeviceWANCommonInterfaceConfigConnection_ActiveConnectionDeviceContainer);
TRXGFUNC(getWANDeviceWANCommonInterfaceConfigConnection_ActiveConnectionServiceID);
TRxObjNode WANDeviceWANCommonInterfaceConfigConnectionDesc[] = {
	{ActiveConnectionDeviceContainer,{{tString,256}},
	  NULL,
	  getWANDeviceWANCommonInterfaceConfigConnection_ActiveConnectionDeviceContainer,
	  NULL,NULL},
	{ActiveConnectionServiceID,{{tString,256}},
	  NULL,
	  getWANDeviceWANCommonInterfaceConfigConnection_ActiveConnectionServiceID,
	  NULL,NULL},
	{NULL}
};

/** */
/** InternetGatewayDevice.WANDevice.WANCommonInterfaceConfig.Connection. */

TRxObjNode WANDeviceWANCommonInterfaceConfigConnectionInstanceDesc[] = {
	{instanceIDMASK,{{0}},
	  NULL,
	  NULL,
	  WANDeviceWANCommonInterfaceConfigConnectionDesc},
};
#endif
/** */
/** InternetGatewayDevice.WANDevice.WANCommonInterfaceConfig. */

TRXGFUNC(getWANDeviceWANCommonInterfaceConfig_EnabledForInternet);
TRXSFUNC(setWANDeviceWANCommonInterfaceConfig_EnabledForInternet);
TRXGFUNC(getWANDeviceWANCommonInterfaceConfig_WANAccessType);
TRXGFUNC(getWANDeviceWANCommonInterfaceConfig_Layer1UpstreamMaxBitRate);
TRXGFUNC(getWANDeviceWANCommonInterfaceConfig_Layer1DownstreamMaxBitRate);
TRXGFUNC(getWANDeviceWANCommonInterfaceConfig_PhysicalLinkStatus);
/*TRXGFUNC(getWANDeviceWANCommonInterfaceConfig_WANAccessProvider);*/
TRXGFUNC(getWANDeviceWANCommonInterfaceConfig_TotalBytesSent);
TRXGFUNC(getWANDeviceWANCommonInterfaceConfig_TotalBytesReceived);
TRXGFUNC(getWANDeviceWANCommonInterfaceConfig_TotalPacketsSent);
TRXGFUNC(getWANDeviceWANCommonInterfaceConfig_TotalPacketsReceived);
#if 0
TRXGFUNC(getWANDeviceWANCommonInterfaceConfig_MaximumActiveConnections);
TRXGFUNC(getWANDeviceWANCommonInterfaceConfig_NumberOfActiveConnections);
#endif
TRxObjNode WANDeviceWANCommonInterfaceConfigDesc[] = {
	{EnabledForInternet,{{tBool,0,0}},
	  setWANDeviceWANCommonInterfaceConfig_EnabledForInternet,
	  getWANDeviceWANCommonInterfaceConfig_EnabledForInternet,
	  NULL,NULL},
	{WANAccessType,{{tString,16,0}},
	  NULL,
	  getWANDeviceWANCommonInterfaceConfig_WANAccessType,
	  NULL,NULL},
	{Layer1UpstreamMaxBitRate,{{tUnsigned,0,0}},
	  NULL,
	  getWANDeviceWANCommonInterfaceConfig_Layer1UpstreamMaxBitRate,
	  NULL,NULL},
	{Layer1DownstreamMaxBitRate,{{tUnsigned,0,0}},
	  NULL,
	  getWANDeviceWANCommonInterfaceConfig_Layer1DownstreamMaxBitRate,
	  NULL,NULL},
	{PhysicalLinkStatus,{{tString,16,0}},
	  NULL,
	  getWANDeviceWANCommonInterfaceConfig_PhysicalLinkStatus,
	  NULL,NULL},
#if 0          
	{WANAccessProvider,{{tString,256}},
	  NULL,
	  getWANDeviceWANCommonInterfaceConfig_WANAccessProvider,
	  NULL,NULL},
#endif          
	{TotalBytesSent,{{tUnsigned,0,1}},
	  NULL,
	  getWANDeviceWANCommonInterfaceConfig_TotalBytesSent,
	  NULL,NULL},
	{TotalBytesReceived,{{tUnsigned,0,1}},
	  NULL,
	  getWANDeviceWANCommonInterfaceConfig_TotalBytesReceived,
	  NULL,NULL},
	{TotalPacketsSent,{{tUnsigned,0,1}},
	  NULL,
	  getWANDeviceWANCommonInterfaceConfig_TotalPacketsSent,
	  NULL,NULL},
	{TotalPacketsReceived,{{tUnsigned,0,1}},
	  NULL,
	  getWANDeviceWANCommonInterfaceConfig_TotalPacketsReceived,
	  NULL,NULL},
#if 0
	{MaximumActiveConnections,{{tUnsigned,0,1}},
	  NULL,
	  getWANDeviceWANCommonInterfaceConfig_MaximumActiveConnections,
	  NULL,NULL},
	{NumberOfActiveConnections,{{tUnsigned,0,1}},
	  NULL,
	  getWANDeviceWANCommonInterfaceConfig_NumberOfActiveConnections,
	  NULL,NULL},
	{Connection,{{tInstance,0}},
	  NULL,
	  NULL,
	  WANDeviceWANCommonInterfaceConfigConnectionInstanceDesc},
#endif
    {NULL, {{0,0,0}}, NULL, NULL, NULL, NULL}
};

/** */
/** InternetGatewayDevice.WANDevice.WANDSLInterfaceConfig.Stats.Total. */

TRXGFUNC(getWANDeviceWANDSLInterfaceConfigStatsTotal_ReceiveBlocks);
TRXGFUNC(getWANDeviceWANDSLInterfaceConfigStatsTotal_TransmitBlocks);
TRXGFUNC(getWANDeviceWANDSLInterfaceConfigStatsTotal_CellDelin);
TRXGFUNC(getWANDeviceWANDSLInterfaceConfigStatsTotal_LinkRetrain);
TRXGFUNC(getWANDeviceWANDSLInterfaceConfigStatsTotal_InitErrors);
TRXGFUNC(getWANDeviceWANDSLInterfaceConfigStatsTotal_InitTimeouts);
TRXGFUNC(getWANDeviceWANDSLInterfaceConfigStatsTotal_LossOfFraming);
TRXGFUNC(getWANDeviceWANDSLInterfaceConfigStatsTotal_ErroredSecs);
TRXGFUNC(getWANDeviceWANDSLInterfaceConfigStatsTotal_SeverelyErroredSecs);
TRXGFUNC(getWANDeviceWANDSLInterfaceConfigStatsTotal_FECErrors);
TRXGFUNC(getWANDeviceWANDSLInterfaceConfigStatsTotal_ATUCFECErrors);
TRXGFUNC(getWANDeviceWANDSLInterfaceConfigStatsTotal_HECErrors);
TRXGFUNC(getWANDeviceWANDSLInterfaceConfigStatsTotal_ATUCHECErrors);
TRXGFUNC(getWANDeviceWANDSLInterfaceConfigStatsTotal_CRCErrors);
TRXGFUNC(getWANDeviceWANDSLInterfaceConfigStatsTotal_ATUCCRCErrors);
TRxObjNode WANDeviceWANDSLInterfaceConfigStatsTotalDesc[] = {
	{ReceiveBlocks,{{tUnsigned,0,1}},
	  NULL,
	  getWANDeviceWANDSLInterfaceConfigStatsTotal_ReceiveBlocks,
	  NULL,NULL},
	{TransmitBlocks,{{tUnsigned,0,1}},
	  NULL,
	  getWANDeviceWANDSLInterfaceConfigStatsTotal_TransmitBlocks,
	  NULL,NULL},
	{CellDelin,{{tUnsigned,0,1}},
	  NULL,
	  getWANDeviceWANDSLInterfaceConfigStatsTotal_CellDelin,
	  NULL,NULL},
	{LinkRetrain,{{tUnsigned,0,1}},
	  NULL,
	  getWANDeviceWANDSLInterfaceConfigStatsTotal_LinkRetrain,
	  NULL,NULL},
	{InitErrors,{{tUnsigned,0,1}},
	  NULL,
	  getWANDeviceWANDSLInterfaceConfigStatsTotal_InitErrors,
	  NULL,NULL},
	{InitTimeouts,{{tUnsigned,0,1}},
	  NULL,
	  getWANDeviceWANDSLInterfaceConfigStatsTotal_InitTimeouts,
	  NULL,NULL},
	{LossOfFraming,{{tUnsigned,0,1}},
	  NULL,
	  getWANDeviceWANDSLInterfaceConfigStatsTotal_LossOfFraming,
	  NULL,NULL},
	{ErroredSecs,{{tUnsigned,0,1}},
	  NULL,
	  getWANDeviceWANDSLInterfaceConfigStatsTotal_ErroredSecs,
	  NULL,NULL},
	{SeverelyErroredSecs,{{tUnsigned,0,1}},
	  NULL,
	  getWANDeviceWANDSLInterfaceConfigStatsTotal_SeverelyErroredSecs,
	  NULL,NULL},
	{FECErrors,{{tUnsigned,0,1}},
	  NULL,
	  getWANDeviceWANDSLInterfaceConfigStatsTotal_FECErrors,
	  NULL,NULL},
	{ATUCFECErrors,{{tUnsigned,0,1}},
	  NULL,
	  getWANDeviceWANDSLInterfaceConfigStatsTotal_ATUCFECErrors,
	  NULL,NULL},
	{HECErrors,{{tUnsigned,0,1}},
	  NULL,
	  getWANDeviceWANDSLInterfaceConfigStatsTotal_HECErrors,
	  NULL,NULL},
	{ATUCHECErrors,{{tUnsigned,0,1}},
	  NULL,
	  getWANDeviceWANDSLInterfaceConfigStatsTotal_ATUCHECErrors,
	  NULL,NULL},
	{CRCErrors,{{tUnsigned,0,1}},
	  NULL,
	  getWANDeviceWANDSLInterfaceConfigStatsTotal_CRCErrors,
	  NULL,NULL},
	{ATUCCRCErrors,{{tUnsigned,0,1}},
	  NULL,
	  getWANDeviceWANDSLInterfaceConfigStatsTotal_ATUCCRCErrors,
	  NULL,NULL},
    {NULL, {{0,0,0}}, NULL, NULL, NULL, NULL}
};

/** */
/** InternetGatewayDevice.WANDevice.WANDSLInterfaceConfig.Stats.Showtime. */

TRXGFUNC(getWANDeviceWANDSLInterfaceConfigStatsShowtime_ReceiveBlocks);
TRXGFUNC(getWANDeviceWANDSLInterfaceConfigStatsShowtime_TransmitBlocks);
TRXGFUNC(getWANDeviceWANDSLInterfaceConfigStatsShowtime_CellDelin);
TRXGFUNC(getWANDeviceWANDSLInterfaceConfigStatsShowtime_LinkRetrain);
TRXGFUNC(getWANDeviceWANDSLInterfaceConfigStatsShowtime_InitErrors);
TRXGFUNC(getWANDeviceWANDSLInterfaceConfigStatsShowtime_InitTimeouts);
TRXGFUNC(getWANDeviceWANDSLInterfaceConfigStatsShowtime_LossOfFraming);
TRXGFUNC(getWANDeviceWANDSLInterfaceConfigStatsShowtime_ErroredSecs);
TRXGFUNC(getWANDeviceWANDSLInterfaceConfigStatsShowtime_SeverelyErroredSecs);
TRXGFUNC(getWANDeviceWANDSLInterfaceConfigStatsShowtime_FECErrors);
TRXGFUNC(getWANDeviceWANDSLInterfaceConfigStatsShowtime_ATUCFECErrors);
TRXGFUNC(getWANDeviceWANDSLInterfaceConfigStatsShowtime_HECErrors);
TRXGFUNC(getWANDeviceWANDSLInterfaceConfigStatsShowtime_ATUCHECErrors);
TRXGFUNC(getWANDeviceWANDSLInterfaceConfigStatsShowtime_CRCErrors);
TRXGFUNC(getWANDeviceWANDSLInterfaceConfigStatsShowtime_ATUCCRCErrors);
TRxObjNode WANDeviceWANDSLInterfaceConfigStatsShowtimeDesc[] = {
	{ReceiveBlocks,{{tUnsigned,0,1}},
	  NULL,
	  getWANDeviceWANDSLInterfaceConfigStatsShowtime_ReceiveBlocks,
	  NULL,NULL},
	{TransmitBlocks,{{tUnsigned,0,1}},
	  NULL,
	  getWANDeviceWANDSLInterfaceConfigStatsShowtime_TransmitBlocks,
	  NULL,NULL},
	{CellDelin,{{tUnsigned,0,1}},
	  NULL,
	  getWANDeviceWANDSLInterfaceConfigStatsShowtime_CellDelin,
	  NULL,NULL},
	{LinkRetrain,{{tUnsigned,0,1}},
	  NULL,
	  getWANDeviceWANDSLInterfaceConfigStatsShowtime_LinkRetrain,
	  NULL,NULL},
	{InitErrors,{{tUnsigned,0,1}},
	  NULL,
	  getWANDeviceWANDSLInterfaceConfigStatsShowtime_InitErrors,
	  NULL,NULL},
	{InitTimeouts,{{tUnsigned,0,1}},
	  NULL,
	  getWANDeviceWANDSLInterfaceConfigStatsShowtime_InitTimeouts,
	  NULL,NULL},
	{LossOfFraming,{{tUnsigned,0,1}},
	  NULL,
	  getWANDeviceWANDSLInterfaceConfigStatsShowtime_LossOfFraming,
	  NULL,NULL},
	{ErroredSecs,{{tUnsigned,0,1}},
	  NULL,
	  getWANDeviceWANDSLInterfaceConfigStatsShowtime_ErroredSecs,
	  NULL,NULL},
	{SeverelyErroredSecs,{{tUnsigned,0,1}},
	  NULL,
	  getWANDeviceWANDSLInterfaceConfigStatsShowtime_SeverelyErroredSecs,
	  NULL,NULL},
	{FECErrors,{{tUnsigned,0,1}},
	  NULL,
	  getWANDeviceWANDSLInterfaceConfigStatsShowtime_FECErrors,
	  NULL,NULL},
	{ATUCFECErrors,{{tUnsigned,0,1}},
	  NULL,
	  getWANDeviceWANDSLInterfaceConfigStatsShowtime_ATUCFECErrors,
	  NULL,NULL},
	{HECErrors,{{tUnsigned,0,1}},
	  NULL,
	  getWANDeviceWANDSLInterfaceConfigStatsShowtime_HECErrors,
	  NULL,NULL},
	{ATUCHECErrors,{{tUnsigned,0,1}},
	  NULL,
	  getWANDeviceWANDSLInterfaceConfigStatsShowtime_ATUCHECErrors,
	  NULL,NULL},
	{CRCErrors,{{tUnsigned,0,1}},
	  NULL,
	  getWANDeviceWANDSLInterfaceConfigStatsShowtime_CRCErrors,
	  NULL,NULL},
	{ATUCCRCErrors,{{tUnsigned,0,1}},
	  NULL,
	  getWANDeviceWANDSLInterfaceConfigStatsShowtime_ATUCCRCErrors,
	  NULL,NULL},
    {NULL, {{0,0,0}}, NULL, NULL, NULL, NULL}
};

#if 0
/** */
/** InternetGatewayDevice.WANDevice.WANDSLInterfaceConfig.Stats.LastShowtime. */

TRXGFUNC(getWANDeviceWANDSLInterfaceConfigStatsLastShowtime_ReceiveBlocks);
TRXGFUNC(getWANDeviceWANDSLInterfaceConfigStatsLastShowtime_TransmitBlocks);
TRXGFUNC(getWANDeviceWANDSLInterfaceConfigStatsLastShowtime_CellDelin);
TRXGFUNC(getWANDeviceWANDSLInterfaceConfigStatsLastShowtime_LinkRetrain);
TRXGFUNC(getWANDeviceWANDSLInterfaceConfigStatsLastShowtime_InitErrors);
TRXGFUNC(getWANDeviceWANDSLInterfaceConfigStatsLastShowtime_InitTimeouts);
TRXGFUNC(getWANDeviceWANDSLInterfaceConfigStatsLastShowtime_LossOfFraming);
TRXGFUNC(getWANDeviceWANDSLInterfaceConfigStatsLastShowtime_ErroredSecs);
TRXGFUNC(getWANDeviceWANDSLInterfaceConfigStatsLastShowtime_SeverelyErroredSecs);
TRXGFUNC(getWANDeviceWANDSLInterfaceConfigStatsLastShowtime_FECErrors);
TRXGFUNC(getWANDeviceWANDSLInterfaceConfigStatsLastShowtime_ATUCFECErrors);
TRXGFUNC(getWANDeviceWANDSLInterfaceConfigStatsLastShowtime_HECErrors);
TRXGFUNC(getWANDeviceWANDSLInterfaceConfigStatsLastShowtime_ATUCHECErrors);
TRXGFUNC(getWANDeviceWANDSLInterfaceConfigStatsLastShowtime_CRCErrors);
TRXGFUNC(getWANDeviceWANDSLInterfaceConfigStatsLastShowtime_ATUCCRCErrors);
TRxObjNode WANDeviceWANDSLInterfaceConfigStatsLastShowtimeDesc[] = {
	{ReceiveBlocks,{{tUnsigned,0,1}},
	  NULL,
	  getWANDeviceWANDSLInterfaceConfigStatsLastShowtime_ReceiveBlocks,
	  NULL,NULL},
	{TransmitBlocks,{{tUnsigned,0,1}},
	  NULL,
	  getWANDeviceWANDSLInterfaceConfigStatsLastShowtime_TransmitBlocks,
	  NULL,NULL},
	{CellDelin,{{tUnsigned,0,1}},
	  NULL,
	  getWANDeviceWANDSLInterfaceConfigStatsLastShowtime_CellDelin,
	  NULL,NULL},
	{LinkRetrain,{{tUnsigned,0,1}},
	  NULL,
	  getWANDeviceWANDSLInterfaceConfigStatsLastShowtime_LinkRetrain,
	  NULL,NULL},
	{InitErrors,{{tUnsigned,0,1}},
	  NULL,
	  getWANDeviceWANDSLInterfaceConfigStatsLastShowtime_InitErrors,
	  NULL,NULL},
	{InitTimeouts,{{tUnsigned,0,1}},
	  NULL,
	  getWANDeviceWANDSLInterfaceConfigStatsLastShowtime_InitTimeouts,
	  NULL,NULL},
	{LossOfFraming,{{tUnsigned,0,1}},
	  NULL,
	  getWANDeviceWANDSLInterfaceConfigStatsLastShowtime_LossOfFraming,
	  NULL,NULL},
	{ErroredSecs,{{tUnsigned,0,1}},
	  NULL,
	  getWANDeviceWANDSLInterfaceConfigStatsLastShowtime_ErroredSecs,
	  NULL,NULL},
	{SeverelyErroredSecs,{{tUnsigned,0,1}},
	  NULL,
	  getWANDeviceWANDSLInterfaceConfigStatsLastShowtime_SeverelyErroredSecs,
	  NULL,NULL},
	{FECErrors,{{tUnsigned,0,1}},
	  NULL,
	  getWANDeviceWANDSLInterfaceConfigStatsLastShowtime_FECErrors,
	  NULL,NULL},
	{ATUCFECErrors,{{tUnsigned,0,1}},
	  NULL,
	  getWANDeviceWANDSLInterfaceConfigStatsLastShowtime_ATUCFECErrors,
	  NULL,NULL},
	{HECErrors,{{tUnsigned,0,1}},
	  NULL,
	  getWANDeviceWANDSLInterfaceConfigStatsLastShowtime_HECErrors,
	  NULL,NULL},
	{ATUCHECErrors,{{tUnsigned,0,1}},
	  NULL,
	  getWANDeviceWANDSLInterfaceConfigStatsLastShowtime_ATUCHECErrors,
	  NULL,NULL},
	{CRCErrors,{{tUnsigned,0,1}},
	  NULL,
	  getWANDeviceWANDSLInterfaceConfigStatsLastShowtime_CRCErrors,
	  NULL,NULL},
	{ATUCCRCErrors,{{tUnsigned,0,1}},
	  NULL,
	  getWANDeviceWANDSLInterfaceConfigStatsLastShowtime_ATUCCRCErrors,
	  NULL,NULL},
    {NULL, {{0,0,0}}, NULL, NULL, NULL, NULL}
};

/** */
/** InternetGatewayDevice.WANDevice.WANDSLInterfaceConfig.Stats.CurrentDay. */

TRXGFUNC(getWANDeviceWANDSLInterfaceConfigStatsCurrentDay_ReceiveBlocks);
TRXGFUNC(getWANDeviceWANDSLInterfaceConfigStatsCurrentDay_TransmitBlocks);
TRXGFUNC(getWANDeviceWANDSLInterfaceConfigStatsCurrentDay_CellDelin);
TRXGFUNC(getWANDeviceWANDSLInterfaceConfigStatsCurrentDay_LinkRetrain);
TRXGFUNC(getWANDeviceWANDSLInterfaceConfigStatsCurrentDay_InitErrors);
TRXGFUNC(getWANDeviceWANDSLInterfaceConfigStatsCurrentDay_InitTimeouts);
TRXGFUNC(getWANDeviceWANDSLInterfaceConfigStatsCurrentDay_LossOfFraming);
TRXGFUNC(getWANDeviceWANDSLInterfaceConfigStatsCurrentDay_ErroredSecs);
TRXGFUNC(getWANDeviceWANDSLInterfaceConfigStatsCurrentDay_SeverelyErroredSecs);
TRXGFUNC(getWANDeviceWANDSLInterfaceConfigStatsCurrentDay_FECErrors);
TRXGFUNC(getWANDeviceWANDSLInterfaceConfigStatsCurrentDay_ATUCFECErrors);
TRXGFUNC(getWANDeviceWANDSLInterfaceConfigStatsCurrentDay_HECErrors);
TRXGFUNC(getWANDeviceWANDSLInterfaceConfigStatsCurrentDay_ATUCHECErrors);
TRXGFUNC(getWANDeviceWANDSLInterfaceConfigStatsCurrentDay_CRCErrors);
TRXGFUNC(getWANDeviceWANDSLInterfaceConfigStatsCurrentDay_ATUCCRCErrors);
TRxObjNode WANDeviceWANDSLInterfaceConfigStatsCurrentDayDesc[] = {
	{ReceiveBlocks,{{tUnsigned,0,1}},
	  NULL,
	  getWANDeviceWANDSLInterfaceConfigStatsCurrentDay_ReceiveBlocks,
	  NULL,NULL},
	{TransmitBlocks,{{tUnsigned,0,1}},
	  NULL,
	  getWANDeviceWANDSLInterfaceConfigStatsCurrentDay_TransmitBlocks,
	  NULL,NULL},
	{CellDelin,{{tUnsigned,0,1}},
	  NULL,
	  getWANDeviceWANDSLInterfaceConfigStatsCurrentDay_CellDelin,
	  NULL,NULL},
	{LinkRetrain,{{tUnsigned,0,1}},
	  NULL,
	  getWANDeviceWANDSLInterfaceConfigStatsCurrentDay_LinkRetrain,
	  NULL,NULL},
	{InitErrors,{{tUnsigned,0,1}},
	  NULL,
	  getWANDeviceWANDSLInterfaceConfigStatsCurrentDay_InitErrors,
	  NULL,NULL},
	{InitTimeouts,{{tUnsigned,0,1}},
	  NULL,
	  getWANDeviceWANDSLInterfaceConfigStatsCurrentDay_InitTimeouts,
	  NULL,NULL},
	{LossOfFraming,{{tUnsigned,0,1}},
	  NULL,
	  getWANDeviceWANDSLInterfaceConfigStatsCurrentDay_LossOfFraming,
	  NULL,NULL},
	{ErroredSecs,{{tUnsigned,0,1}},
	  NULL,
	  getWANDeviceWANDSLInterfaceConfigStatsCurrentDay_ErroredSecs,
	  NULL,NULL},
	{SeverelyErroredSecs,{{tUnsigned,0,1}},
	  NULL,
	  getWANDeviceWANDSLInterfaceConfigStatsCurrentDay_SeverelyErroredSecs,
	  NULL,NULL},
	{FECErrors,{{tUnsigned,0,1}},
	  NULL,
	  getWANDeviceWANDSLInterfaceConfigStatsCurrentDay_FECErrors,
	  NULL,NULL},
	{ATUCFECErrors,{{tUnsigned,0,1}},
	  NULL,
	  getWANDeviceWANDSLInterfaceConfigStatsCurrentDay_ATUCFECErrors,
	  NULL,NULL},
	{HECErrors,{{tUnsigned,0,1}},
	  NULL,
	  getWANDeviceWANDSLInterfaceConfigStatsCurrentDay_HECErrors,
	  NULL,NULL},
	{ATUCHECErrors,{{tUnsigned,0,1}},
	  NULL,
	  getWANDeviceWANDSLInterfaceConfigStatsCurrentDay_ATUCHECErrors,
	  NULL,NULL},
	{CRCErrors,{{tUnsigned,0,1}},
	  NULL,
	  getWANDeviceWANDSLInterfaceConfigStatsCurrentDay_CRCErrors,
	  NULL,NULL},
	{ATUCCRCErrors,{{tUnsigned,0,1}},
	  NULL,
	  getWANDeviceWANDSLInterfaceConfigStatsCurrentDay_ATUCCRCErrors,
	  NULL,NULL},
    {NULL, {{0,0,0}}, NULL, NULL, NULL, NULL}
};

/** */
/** InternetGatewayDevice.WANDevice.WANDSLInterfaceConfig.Stats.QuarterHour. */

TRXGFUNC(getWANDeviceWANDSLInterfaceConfigStatsQuarterHour_ReceiveBlocks);
TRXGFUNC(getWANDeviceWANDSLInterfaceConfigStatsQuarterHour_TransmitBlocks);
TRXGFUNC(getWANDeviceWANDSLInterfaceConfigStatsQuarterHour_CellDelin);
TRXGFUNC(getWANDeviceWANDSLInterfaceConfigStatsQuarterHour_LinkRetrain);
TRXGFUNC(getWANDeviceWANDSLInterfaceConfigStatsQuarterHour_InitErrors);
TRXGFUNC(getWANDeviceWANDSLInterfaceConfigStatsQuarterHour_InitTimeouts);
TRXGFUNC(getWANDeviceWANDSLInterfaceConfigStatsQuarterHour_LossOfFraming);
TRXGFUNC(getWANDeviceWANDSLInterfaceConfigStatsQuarterHour_ErroredSecs);
TRXGFUNC(getWANDeviceWANDSLInterfaceConfigStatsQuarterHour_SeverelyErroredSecs);
TRXGFUNC(getWANDeviceWANDSLInterfaceConfigStatsQuarterHour_FECErrors);
TRXGFUNC(getWANDeviceWANDSLInterfaceConfigStatsQuarterHour_ATUCFECErrors);
TRXGFUNC(getWANDeviceWANDSLInterfaceConfigStatsQuarterHour_HECErrors);
TRXGFUNC(getWANDeviceWANDSLInterfaceConfigStatsQuarterHour_ATUCHECErrors);
TRXGFUNC(getWANDeviceWANDSLInterfaceConfigStatsQuarterHour_CRCErrors);
TRXGFUNC(getWANDeviceWANDSLInterfaceConfigStatsQuarterHour_ATUCCRCErrors);
TRxObjNode WANDeviceWANDSLInterfaceConfigStatsQuarterHourDesc[] = {
	{ReceiveBlocks,{{tUnsigned,0,1}},
	  NULL,
	  getWANDeviceWANDSLInterfaceConfigStatsQuarterHour_ReceiveBlocks,
	  NULL,NULL},
	{TransmitBlocks,{{tUnsigned,0,1}},
	  NULL,
	  getWANDeviceWANDSLInterfaceConfigStatsQuarterHour_TransmitBlocks,
	  NULL,NULL},
	{CellDelin,{{tUnsigned,0,1}},
	  NULL,
	  getWANDeviceWANDSLInterfaceConfigStatsQuarterHour_CellDelin,
	  NULL,NULL},
	{LinkRetrain,{{tUnsigned,0,1}},
	  NULL,
	  getWANDeviceWANDSLInterfaceConfigStatsQuarterHour_LinkRetrain,
	  NULL,NULL},
	{InitErrors,{{tUnsigned,0,1}},
	  NULL,
	  getWANDeviceWANDSLInterfaceConfigStatsQuarterHour_InitErrors,
	  NULL,NULL},
	{InitTimeouts,{{tUnsigned,0,1}},
	  NULL,
	  getWANDeviceWANDSLInterfaceConfigStatsQuarterHour_InitTimeouts,
	  NULL,NULL},
	{LossOfFraming,{{tUnsigned,0,1}},
	  NULL,
	  getWANDeviceWANDSLInterfaceConfigStatsQuarterHour_LossOfFraming,
	  NULL,NULL},
	{ErroredSecs,{{tUnsigned,0,1}},
	  NULL,
	  getWANDeviceWANDSLInterfaceConfigStatsQuarterHour_ErroredSecs,
	  NULL,NULL},
	{SeverelyErroredSecs,{{tUnsigned,0,1}},
	  NULL,
	  getWANDeviceWANDSLInterfaceConfigStatsQuarterHour_SeverelyErroredSecs,
	  NULL,NULL},
	{FECErrors,{{tUnsigned,0,1}},
	  NULL,
	  getWANDeviceWANDSLInterfaceConfigStatsQuarterHour_FECErrors,
	  NULL,NULL},
	{ATUCFECErrors,{{tUnsigned,0,1}},
	  NULL,
	  getWANDeviceWANDSLInterfaceConfigStatsQuarterHour_ATUCFECErrors,
	  NULL,NULL},
	{HECErrors,{{tUnsigned,0,1}},
	  NULL,
	  getWANDeviceWANDSLInterfaceConfigStatsQuarterHour_HECErrors,
	  NULL,NULL},
	{ATUCHECErrors,{{tUnsigned,0,1}},
	  NULL,
	  getWANDeviceWANDSLInterfaceConfigStatsQuarterHour_ATUCHECErrors,
	  NULL,NULL},
	{CRCErrors,{{tUnsigned,0,1}},
	  NULL,
	  getWANDeviceWANDSLInterfaceConfigStatsQuarterHour_CRCErrors,
	  NULL,NULL},
	{ATUCCRCErrors,{{tUnsigned,0,1}},
	  NULL,
	  getWANDeviceWANDSLInterfaceConfigStatsQuarterHour_ATUCCRCErrors,
	  NULL,NULL},
	{NULL}
};
#endif /* 0 */

/** */
/** InternetGatewayDevice.WANDevice.WANDSLInterfaceConfig.Stats. */

TRxObjNode WANDeviceWANDSLInterfaceConfigStatsDesc[] = {
	{Total,{{tObject,0,0}},
	  NULL,
	  NULL,
	  WANDeviceWANDSLInterfaceConfigStatsTotalDesc,NULL},
	{Showtime,{{tObject,0,0}},
	  NULL,
	  NULL,
	  WANDeviceWANDSLInterfaceConfigStatsShowtimeDesc,NULL},
#if 0
	{LastShowtime,{{tObject,0}},
	  NULL,
	  NULL,
	  WANDeviceWANDSLInterfaceConfigStatsLastShowtimeDesc},
	{CurrentDay,{{tObject,0}},
	  NULL,
	  NULL,
	  WANDeviceWANDSLInterfaceConfigStatsCurrentDayDesc},
	{QuarterHour,{{tObject,0}},
	  NULL,
	  NULL,
	  WANDeviceWANDSLInterfaceConfigStatsQuarterHourDesc},
#endif
    {NULL, {{0,0,0}}, NULL, NULL, NULL, NULL}
};

/** */
/** InternetGatewayDevice.WANDevice.WANDSLInterfaceConfig. */

TRXGFUNC(getWANDeviceWANDSLInterfaceConfig_Enable);
TRXSFUNC(setWANDeviceWANDSLInterfaceConfig_Enable);
TRXGFUNC(getWANDeviceWANDSLInterfaceConfig_Status);
TRXGFUNC(getWANDeviceWANDSLInterfaceConfig_ModulationType);
TRXGFUNC(getWANDeviceWANDSLInterfaceConfig_LineEncoding);
TRXGFUNC(getWANDeviceWANDSLInterfaceConfig_DataPath);
TRXGFUNC(getWANDeviceWANDSLInterfaceConfig_InterleaveDepth);
TRXGFUNC(getWANDeviceWANDSLInterfaceConfig_LineNumber);
TRXGFUNC(getWANDeviceWANDSLInterfaceConfig_UpstreamCurrRate);
TRXGFUNC(getWANDeviceWANDSLInterfaceConfig_DownstreamCurrRate);
TRXGFUNC(getWANDeviceWANDSLInterfaceConfig_UpstreamMaxRate);
TRXGFUNC(getWANDeviceWANDSLInterfaceConfig_DownstreamMaxRate);
TRXGFUNC(getWANDeviceWANDSLInterfaceConfig_UpstreamNoiseMargin);
TRXGFUNC(getWANDeviceWANDSLInterfaceConfig_DownstreamNoiseMargin);
TRXGFUNC(getWANDeviceWANDSLInterfaceConfig_UpstreamAttenuation);
TRXGFUNC(getWANDeviceWANDSLInterfaceConfig_DownstreamAttenuation);
TRXGFUNC(getWANDeviceWANDSLInterfaceConfig_UpstreamPower);
TRXGFUNC(getWANDeviceWANDSLInterfaceConfig_DownstreamPower);

TRXGFUNC(getWANDeviceWANDSLInterfaceConfig_ATURVendor);
TRXGFUNC(getWANDeviceWANDSLInterfaceConfig_ATURCountry);
#if 0
TRXGFUNC(getWANDeviceWANDSLInterfaceConfig_ATURANSIStd);
TRXGFUNC(getWANDeviceWANDSLInterfaceConfig_ATURANSIRev);
#endif
TRXGFUNC(getWANDeviceWANDSLInterfaceConfig_ATUCVendor);
TRXGFUNC(getWANDeviceWANDSLInterfaceConfig_ATUCCountry);
#if 0
TRXGFUNC(getWANDeviceWANDSLInterfaceConfig_ATUCANSIStd);
TRXGFUNC(getWANDeviceWANDSLInterfaceConfig_ATUCANSIRev);
#endif
TRXGFUNC(getWANDeviceWANDSLInterfaceConfig_TotalStart);
#if 0
TRXGFUNC(getWANDeviceWANDSLInterfaceConfig_ShowtimeStart);
TRXGFUNC(getWANDeviceWANDSLInterfaceConfig_LastShowtimeStart);
TRXGFUNC(getWANDeviceWANDSLInterfaceConfig_CurrentDayStart);
TRXGFUNC(getWANDeviceWANDSLInterfaceConfig_QuarterHourStart);
#endif
TRxObjNode WANDeviceWANDSLInterfaceConfigDesc[] = {
	{Enable,{{tBool,0,0}},
	  setWANDeviceWANDSLInterfaceConfig_Enable,
	  getWANDeviceWANDSLInterfaceConfig_Enable,
	  NULL,NULL},
	{Status,{{tString,16,0}},
	  NULL,
	  getWANDeviceWANDSLInterfaceConfig_Status,
	  NULL,NULL},
	{ModulationType,{{tString,16,0}},
	  NULL,
	  getWANDeviceWANDSLInterfaceConfig_ModulationType,
	  NULL,NULL},
	{LineEncoding,{{tString,16,0}},
	  NULL,
	  getWANDeviceWANDSLInterfaceConfig_LineEncoding,
	  NULL,NULL},
	{DataPath,{{tString,16,0}},
	  NULL,
	  getWANDeviceWANDSLInterfaceConfig_DataPath,
	  NULL,NULL},
	{InterleaveDepth,{{tUnsigned,0,0}},
	  NULL,
	  getWANDeviceWANDSLInterfaceConfig_InterleaveDepth,
	  NULL,NULL},
	{LineNumber,{{tInt,0,0}},
	  NULL,
	  getWANDeviceWANDSLInterfaceConfig_LineNumber,
	  NULL,NULL},
	{UpstreamCurrRate,{{tUnsigned,0,1}},
	  NULL,
	  getWANDeviceWANDSLInterfaceConfig_UpstreamCurrRate,
	  NULL,NULL},
	{DownstreamCurrRate,{{tUnsigned,0,1}},
	  NULL,
	  getWANDeviceWANDSLInterfaceConfig_DownstreamCurrRate,
	  NULL,NULL},
	{UpstreamMaxRate,{{tUnsigned,0,1}},
	  NULL,
	  getWANDeviceWANDSLInterfaceConfig_UpstreamMaxRate,
	  NULL,NULL},
	{DownstreamMaxRate,{{tUnsigned,0,1}},
	  NULL,
	  getWANDeviceWANDSLInterfaceConfig_DownstreamMaxRate,
	  NULL,NULL},
	{UpstreamNoiseMargin,{{tInt,0,1}},
	  NULL,
	  getWANDeviceWANDSLInterfaceConfig_UpstreamNoiseMargin,
	  NULL,NULL},
	{DownstreamNoiseMargin,{{tInt,0,1}},
	  NULL,
	  getWANDeviceWANDSLInterfaceConfig_DownstreamNoiseMargin,
	  NULL,NULL},
	{UpstreamAttenuation,{{tInt,0,1}},
	  NULL,
	  getWANDeviceWANDSLInterfaceConfig_UpstreamAttenuation,
	  NULL,NULL},
	{DownstreamAttenuation,{{tInt,0,1}},
	  NULL,
	  getWANDeviceWANDSLInterfaceConfig_DownstreamAttenuation,
	  NULL,NULL},
	{UpstreamPower,{{tInt,0,1}},
	  NULL,
	  getWANDeviceWANDSLInterfaceConfig_UpstreamPower,
	  NULL,NULL},
	{DownstreamPower,{{tInt,0,1}},
	  NULL,
	  getWANDeviceWANDSLInterfaceConfig_DownstreamPower,
	  NULL,NULL},
	{ATURVendor,{{tString,8,0}},
	  NULL,
	  getWANDeviceWANDSLInterfaceConfig_ATURVendor,
	  NULL,NULL},
	{ATURCountry,{{tUnsigned,0,0}},
	  NULL,
	  getWANDeviceWANDSLInterfaceConfig_ATURCountry,
	  NULL,NULL},
#if 0          
	{ATURANSIStd,{{tUnsigned,0}},
	  NULL,
	  getWANDeviceWANDSLInterfaceConfig_ATURANSIStd,
	  NULL,NULL},
	{ATURANSIRev,{{tUnsigned,0}},
	  NULL,
	  getWANDeviceWANDSLInterfaceConfig_ATURANSIRev,
	  NULL,NULL},
#endif
	{ATUCVendor,{{tString,8,0}},
	  NULL,
	  getWANDeviceWANDSLInterfaceConfig_ATUCVendor,
	  NULL,NULL},
	{ATUCCountry,{{tUnsigned,0,0}},
	  NULL,
	  getWANDeviceWANDSLInterfaceConfig_ATUCCountry,
	  NULL,NULL},
#if 0          
	{ATUCANSIStd,{{tUnsigned,0}},
	  NULL,
	  getWANDeviceWANDSLInterfaceConfig_ATUCANSIStd,
	  NULL,NULL},
	{ATUCANSIRev,{{tUnsigned,0}},
	  NULL,
	  getWANDeviceWANDSLInterfaceConfig_ATUCANSIRev,
	  NULL,NULL},
	{TotalStart,{{tUnsigned,0,1}},
	  NULL,
	  getWANDeviceWANDSLInterfaceConfig_TotalStart,
	  NULL,NULL},
	{ShowtimeStart,{{tUnsigned,0,1}},
	  NULL,
	  getWANDeviceWANDSLInterfaceConfig_ShowtimeStart,
	  NULL,NULL},
	{LastShowtimeStart,{{tUnsigned,0,1}},
	  NULL,
	  getWANDeviceWANDSLInterfaceConfig_LastShowtimeStart,
	  NULL,NULL},
	{CurrentDayStart,{{tUnsigned,0,1}},
	  NULL,
	  getWANDeviceWANDSLInterfaceConfig_CurrentDayStart,
	  NULL,NULL},
	{QuarterHourStart,{{tUnsigned,0,1}},
	  NULL,
	  getWANDeviceWANDSLInterfaceConfig_QuarterHourStart,
	  NULL,NULL},
#endif
	{Stats,{{tObject,0,0}},
	  NULL,
	  NULL,
	  WANDeviceWANDSLInterfaceConfigStatsDesc,NULL},
    {NULL, {{0,0,0}}, NULL, NULL, NULL, NULL}
};

#if 0
/** */
/** InternetGatewayDevice.WANDevice.WANEthernetInterfaceConfig.Stats. */

TRXGFUNC(getWANDeviceWANEthernetInterfaceConfigStats_BytesSent);
TRXGFUNC(getWANDeviceWANEthernetInterfaceConfigStats_BytesReceived);
TRXGFUNC(getWANDeviceWANEthernetInterfaceConfigStats_PacketsSent);
TRXGFUNC(getWANDeviceWANEthernetInterfaceConfigStats_PacketsReceived);
TRxObjNode WANDeviceWANEthernetInterfaceConfigStatsDesc[] = {
	{BytesSent,{{tUnsigned,0,1}},
	  NULL,
	  getWANDeviceWANEthernetInterfaceConfigStats_BytesSent,
	  NULL,NULL},
	{BytesReceived,{{tUnsigned,0,1}},
	  NULL,
	  getWANDeviceWANEthernetInterfaceConfigStats_BytesReceived,
	  NULL,NULL},
	{PacketsSent,{{tUnsigned,0,1}},
	  NULL,
	  getWANDeviceWANEthernetInterfaceConfigStats_PacketsSent,
	  NULL,NULL},
	{PacketsReceived,{{tUnsigned,0,1}},
	  NULL,
	  getWANDeviceWANEthernetInterfaceConfigStats_PacketsReceived,
	  NULL,NULL},
    {NULL, {{0,0,0}}, NULL, NULL, NULL, NULL}
};

/** */
/** InternetGatewayDevice.WANDevice.WANEthernetInterfaceConfig. */

TRXGFUNC(getWANDeviceWANEthernetInterfaceConfig_Enable);
TRXSFUNC(setWANDeviceWANEthernetInterfaceConfig_Enable);
TRXGFUNC(getWANDeviceWANEthernetInterfaceConfig_Status);
TRXGFUNC(getWANDeviceWANEthernetInterfaceConfig_MACAddress);
TRXGFUNC(getWANDeviceWANEthernetInterfaceConfig_MaxBitRate);
TRXSFUNC(setWANDeviceWANEthernetInterfaceConfig_MaxBitRate);
TRXGFUNC(getWANDeviceWANEthernetInterfaceConfig_DuplexMode);
TRXSFUNC(setWANDeviceWANEthernetInterfaceConfig_DuplexMode);
TRxObjNode WANDeviceWANEthernetInterfaceConfigDesc[] = {
	{Enable,{{tBool,0}},
	  setWANDeviceWANEthernetInterfaceConfig_Enable,
	  getWANDeviceWANEthernetInterfaceConfig_Enable,
	  NULL,NULL},
	{Status,{{tString,16}},
	  NULL,
	  getWANDeviceWANEthernetInterfaceConfig_Status,
	  NULL,NULL},
	{MACAddress,{{tString,16}},
	  NULL,
	  getWANDeviceWANEthernetInterfaceConfig_MACAddress,
	  NULL,NULL},
	{MaxBitRate,{{tString,16}},
	  setWANDeviceWANEthernetInterfaceConfig_MaxBitRate,
	  getWANDeviceWANEthernetInterfaceConfig_MaxBitRate,
	  NULL,NULL},
	{DuplexMode,{{tString,16}},
	  setWANDeviceWANEthernetInterfaceConfig_DuplexMode,
	  getWANDeviceWANEthernetInterfaceConfig_DuplexMode,
	  NULL,NULL},
	{Stats,{{tObject,0}},
	  NULL,
	  NULL,
	  WANDeviceWANEthernetInterfaceConfigStatsDesc},
    {NULL, {{0,0,0}}, NULL, NULL, NULL, NULL}
};
#endif

/** */
/** InternetGatewayDevice.WANDevice.WANDSLConnectionManagement.ConnectionService. */
TRXGFUNC(getWANDeviceWANDSLConnectionManagementConnectionService_WANConnectionDevice);
TRXGFUNC(getWANDeviceWANDSLConnectionManagementConnectionService_WANConnectionService);
TRXGFUNC(getWANDeviceWANDSLConnectionManagementConnectionService_DestinationAddress);
TRXGFUNC(getWANDeviceWANDSLConnectionManagementConnectionService_LinkType);
TRXGFUNC(getWANDeviceWANDSLConnectionManagementConnectionService_ConnectionType);
TRxObjNode WANDeviceWANDSLConnectionManagementConnectionServiceDesc[] = {
	{WANConnectionDevice,{{tString,256,0}},
	  NULL,
	  getWANDeviceWANDSLConnectionManagementConnectionService_WANConnectionDevice,
	  NULL,NULL},
	{WANConnectionService,{{tString,256,0}},
	  NULL,
	  getWANDeviceWANDSLConnectionManagementConnectionService_WANConnectionService,
	  NULL,NULL},
	{DestinationAddress,{{tString,256,0}},
	  NULL,
	  getWANDeviceWANDSLConnectionManagementConnectionService_DestinationAddress,
	  NULL,NULL},
	{LinkType,{{tString,16,0}},
	  NULL,
	  getWANDeviceWANDSLConnectionManagementConnectionService_LinkType,
	  NULL,NULL},
	{ConnectionType,{{tString,16,0}},
	  NULL,
	  getWANDeviceWANDSLConnectionManagementConnectionService_ConnectionType,
	  NULL,NULL},
	{Name,{{tStringSOnly,32,0}},
	  NULL,
	  NULL,
	  NULL,NULL},
    {NULL, {{0,0,0}}, NULL, NULL, NULL, NULL}
};

/** */
/** InternetGatewayDevice.WANDevice.WANDSLConnectionManagement.ConnectionService. */

TRxObjNode WANDeviceWANDSLConnectionManagementConnectionServiceInstanceDesc[] = {
	{instanceIDMASK,{{0,0,0}},
	  NULL,
	  NULL,
	  WANDeviceWANDSLConnectionManagementConnectionServiceDesc,NULL},
};

/** */
/** InternetGatewayDevice.WANDevice.WANDSLConnectionManagement. */

TRXGFUNC(getWANDeviceWANDSLConnectionManagement_ConnectionServiceNumberOfEntries);
TRxObjNode WANDeviceWANDSLConnectionManagementDesc[] = {
	{ConnectionServiceNumberOfEntries,{{tUnsigned,0,0}},
	  NULL,
	  getWANDeviceWANDSLConnectionManagement_ConnectionServiceNumberOfEntries,
	  NULL,NULL},
	{ConnectionService,{{tInstance,0,0}},
	  NULL,
	  NULL,
	  WANDeviceWANDSLConnectionManagementConnectionServiceInstanceDesc,NULL},
    {NULL, {{0,0,0}}, NULL, NULL, NULL, NULL}
};

/** */
/** InternetGatewayDevice.WANDevice.WANDSLDiagnostics. */

TRXGFUNC(getWANDeviceWANDSLDiagnostics_LoopDiagnosticsState);
TRXSFUNC(setWANDeviceWANDSLDiagnostics_LoopDiagnosticsState);
TRXGFUNC(getWANDeviceWANDSLDiagnostics_ACTPSDds);
TRXGFUNC(getWANDeviceWANDSLDiagnostics_ACTPSDus);
TRXGFUNC(getWANDeviceWANDSLDiagnostics_ACTATPds);
TRXGFUNC(getWANDeviceWANDSLDiagnostics_ACTATPus);
TRXGFUNC(getWANDeviceWANDSLDiagnostics_HLINSCds);
TRXGFUNC(getWANDeviceWANDSLDiagnostics_HLINpsds);
TRXGFUNC(getWANDeviceWANDSLDiagnostics_QLNpsds);
TRXGFUNC(getWANDeviceWANDSLDiagnostics_SNRpsds);
TRXGFUNC(getWANDeviceWANDSLDiagnostics_BITSpsds);
TRXGFUNC(getWANDeviceWANDSLDiagnostics_GAINSpsds);
TRxObjNode WANDeviceWANDSLDiagnosticsDesc[] = {
	{LoopDiagnosticsState,{{tString,16,1}},
	  setWANDeviceWANDSLDiagnostics_LoopDiagnosticsState,
	  getWANDeviceWANDSLDiagnostics_LoopDiagnosticsState,
	  NULL,NULL},
	{ACTPSDds,{{tInt,0,1}},
	  NULL,
	  getWANDeviceWANDSLDiagnostics_ACTPSDds,
	  NULL,NULL},
	{ACTPSDus,{{tInt,0,1}},
	  NULL,
	  getWANDeviceWANDSLDiagnostics_ACTPSDus,
	  NULL,NULL},
	{ACTATPds,{{tInt,0,1}},
	  NULL,
	  getWANDeviceWANDSLDiagnostics_ACTATPds,
	  NULL,NULL},
	{ACTATPus,{{tInt,0,1}},
	  NULL,
	  getWANDeviceWANDSLDiagnostics_ACTATPus,
	  NULL,NULL},
	{HLINSCds,{{tInt,0,1}},
	  NULL,
	  getWANDeviceWANDSLDiagnostics_HLINSCds,
	  NULL,NULL},
	{HLINpsds,{{tString,16,1}},
	  NULL,
	  getWANDeviceWANDSLDiagnostics_HLINpsds,
	  NULL,NULL},
	{QLNpsds,{{tString,16,1}},
	  NULL,
	  getWANDeviceWANDSLDiagnostics_QLNpsds,
	  NULL,NULL},
	{SNRpsds,{{tString,16,1}},
	  NULL,
	  getWANDeviceWANDSLDiagnostics_SNRpsds,
	  NULL,NULL},
	{BITSpsds,{{tString,16,1}},
	  NULL,
	  getWANDeviceWANDSLDiagnostics_BITSpsds,
	  NULL,NULL},
	{GAINSpsds,{{tString,16,1}},
	  NULL,
	  getWANDeviceWANDSLDiagnostics_GAINSpsds,
	  NULL,NULL},
    {NULL, {{0,0,0}}, NULL, NULL, NULL, NULL}
};

/** */
/** InternetGatewayDevice.WANDevice.WANConnectionDevice.WANDSLLinkConfig. */
TRXSFUNC(setDSLLinkEnable);
TRXGFUNC(getDSLLinkEnable);
TRXSFUNC(setDSLLinkType);
TRXGFUNC(getDSLLinkType);
TRXSFUNC(setDSLLinkAddress);
TRXGFUNC(getDSLLinkAddress);
TRXSFUNC(setATMEncap);
TRXGFUNC(getATMEncap);
TRXSFUNC(setATMQoS);
TRXGFUNC(getATMQoS);
TRXSFUNC(setATMPeakCellRate);
TRXGFUNC(getATMPeakCellRate);
TRXSFUNC(setATMMaximumBurstSize);
TRXGFUNC(getATMMaximumBurstSize);
TRXSFUNC(setATMSustainableCellRate);
TRXGFUNC(getATMSustainableCellRate);
TRXGFUNC(getWANDeviceWANConnectionDeviceWANDSLLinkConfig_ModulationType);
TRXGFUNC(getWANDeviceWANConnectionDeviceWANDSLLinkConfig_ATMTransmittedBlocks);
TRXGFUNC(getWANDeviceWANConnectionDeviceWANDSLLinkConfig_ATMReceivedBlocks);
TRXGFUNC(getWANDeviceWANConnectionDeviceWANDSLLinkConfig_AAL5CRCErrors);
TRXGFUNC(getWANDeviceWANConnectionDeviceWANDSLLinkConfig_ATMHECErrors);

/* InternetGatewayDevice.WANDevice.{i}.WANConnectionDevice.{i}.WANDSLLinkConfig. */
TRxObjNode WANDeviceWANConnectionDeviceWANDSLLinkConfigDesc[] = {
        {Enable,{{tBool,0,0}},setDSLLinkEnable,getDSLLinkEnable,NULL,NULL},
	#if 0
	{LinkStatus,{{tString,16}},
	  NULL,
	  getWANDeviceWANConnectionDeviceWANDSLLinkConfig_LinkStatus,
	  NULL,NULL},
	#endif
        {LinkType,{{tString,16,0}},setDSLLinkType,getDSLLinkType,NULL,NULL},
	#if 0
	{AutoConfig,{{tBool,0}},
	  NULL,
	  getWANDeviceWANConnectionDeviceWANDSLLinkConfig_AutoConfig,
	  NULL,NULL},
	#endif
	{ModulationType,{{tString,16,0}},
	  NULL,
	  getWANDeviceWANConnectionDeviceWANDSLLinkConfig_ModulationType,
	  NULL,NULL},
        {DestinationAddress,{{tString,16,0}},setDSLLinkAddress,getDSLLinkAddress,NULL,NULL},
        {ATMEncapsulation,{{tString,16,0}},setATMEncap,getATMEncap,NULL,NULL},
	#if 0
	{FCSPreserved,{{tBool,0,0}},
	  setWANDeviceWANConnectionDeviceWANDSLLinkConfig_FCSPreserved,
	  getWANDeviceWANConnectionDeviceWANDSLLinkConfig_FCSPreserved,
	  NULL,NULL},
	{VCSearchList,{{tString,16,0}},
	  setWANDeviceWANConnectionDeviceWANDSLLinkConfig_VCSearchList,
	  getWANDeviceWANConnectionDeviceWANDSLLinkConfig_VCSearchList,
	  NULL,NULL},
	{ATMAAL,{{tString,16,0}},
	  NULL,
	  getWANDeviceWANConnectionDeviceWANDSLLinkConfig_ATMAAL,
	  NULL,NULL},
        #endif
	{ATMTransmittedBlocks,{{tUnsigned,0,1}},
	  NULL,
	  getWANDeviceWANConnectionDeviceWANDSLLinkConfig_ATMTransmittedBlocks,
	  NULL,NULL},
	{ATMReceivedBlocks,{{tUnsigned,0,1}},
	  NULL,
	  getWANDeviceWANConnectionDeviceWANDSLLinkConfig_ATMReceivedBlocks,
	  NULL,NULL},
        {ATMQoS,{{tString,16,0}},setATMQoS,getATMQoS,NULL,NULL},
        {ATMPeakCellRate,{{tUnsigned,0,0}},setATMPeakCellRate,getATMPeakCellRate,NULL,NULL},
        {ATMMaximumBurstSize,{{tUnsigned,0,0}},setATMMaximumBurstSize,getATMMaximumBurstSize,NULL,NULL},
        {ATMSustainableCellRate,{{tUnsigned,0,0}},setATMSustainableCellRate,getATMSustainableCellRate,NULL,NULL},
	{AAL5CRCErrors,{{tUnsigned,0,1}},
	  NULL,
	  getWANDeviceWANConnectionDeviceWANDSLLinkConfig_AAL5CRCErrors,
	  NULL,NULL},
        #if 0
	{ATMCRCErrors,{{tUnsigned,0,1}},
	  NULL,
	  getWANDeviceWANConnectionDeviceWANDSLLinkConfig_ATMCRCErrors,
	  NULL,NULL},
	#endif
	{ATMHECErrors,{{tUnsigned,0,1}},
	  NULL,
	  getWANDeviceWANConnectionDeviceWANDSLLinkConfig_ATMHECErrors,
	  NULL,NULL},
    {NULL, {{0,0,0}}, NULL, NULL, NULL, NULL}
};

/** */
/** InternetGatewayDevice.WANDevice.WANConnectionDevice.WANATMF5LoopbackDiagnostics. */
TRXGFUNC(getWANDeviceWANConnectionDeviceWANATMF5LoopbackDiagnostics_DiagnosticsState);
TRXSFUNC(setWANDeviceWANConnectionDeviceWANATMF5LoopbackDiagnostics_DiagnosticsState);
TRXGFUNC(getWANDeviceWANConnectionDeviceWANATMF5LoopbackDiagnostics_NumberOfRepetitions);
TRXSFUNC(setWANDeviceWANConnectionDeviceWANATMF5LoopbackDiagnostics_NumberOfRepetitions);
TRXGFUNC(getWANDeviceWANConnectionDeviceWANATMF5LoopbackDiagnostics_Timeout);
TRXSFUNC(setWANDeviceWANConnectionDeviceWANATMF5LoopbackDiagnostics_Timeout);
TRXGFUNC(getWANDeviceWANConnectionDeviceWANATMF5LoopbackDiagnostics_SuccessCount);
TRXGFUNC(getWANDeviceWANConnectionDeviceWANATMF5LoopbackDiagnostics_FailureCount);
TRXGFUNC(getWANDeviceWANConnectionDeviceWANATMF5LoopbackDiagnostics_AverageResponseTime);
TRXGFUNC(getWANDeviceWANConnectionDeviceWANATMF5LoopbackDiagnostics_MinimumResponseTime);
TRXGFUNC(getWANDeviceWANConnectionDeviceWANATMF5LoopbackDiagnostics_MaximumResponseTime);
TRxObjNode WANDeviceWANConnectionDeviceWANATMF5LoopbackDiagnosticsDesc[] = {
	{DiagnosticsState,{{tString,16,1}},
	  setWANDeviceWANConnectionDeviceWANATMF5LoopbackDiagnostics_DiagnosticsState,
	  getWANDeviceWANConnectionDeviceWANATMF5LoopbackDiagnostics_DiagnosticsState,
	  NULL,NULL},
	{NumberOfRepetitions,{{tUnsigned,0,0}},
	  setWANDeviceWANConnectionDeviceWANATMF5LoopbackDiagnostics_NumberOfRepetitions,
	  getWANDeviceWANConnectionDeviceWANATMF5LoopbackDiagnostics_NumberOfRepetitions,
	  NULL,NULL},
	{Timeout,{{tUnsigned,0,0}},
	  setWANDeviceWANConnectionDeviceWANATMF5LoopbackDiagnostics_Timeout,
	  getWANDeviceWANConnectionDeviceWANATMF5LoopbackDiagnostics_Timeout,
	  NULL,NULL},
	{SuccessCount,{{tUnsigned,0,1}},
	  NULL,
	  getWANDeviceWANConnectionDeviceWANATMF5LoopbackDiagnostics_SuccessCount,
	  NULL,NULL},
	{FailureCount,{{tUnsigned,0,1}},
	  NULL,
	  getWANDeviceWANConnectionDeviceWANATMF5LoopbackDiagnostics_FailureCount,
	  NULL,NULL},
	{AverageResponseTime,{{tUnsigned,0,1}},
	  NULL,
	  getWANDeviceWANConnectionDeviceWANATMF5LoopbackDiagnostics_AverageResponseTime,
	  NULL,NULL},
	{MinimumResponseTime,{{tUnsigned,0,1}},
	  NULL,
	  getWANDeviceWANConnectionDeviceWANATMF5LoopbackDiagnostics_MinimumResponseTime,
	  NULL,NULL},
	{MaximumResponseTime,{{tUnsigned,0,1}},
	  NULL,
	  getWANDeviceWANConnectionDeviceWANATMF5LoopbackDiagnostics_MaximumResponseTime,
	  NULL,NULL},
    {NULL, {{0,0,0}}, NULL, NULL, NULL, NULL}
};

#if 0
/** */
/** InternetGatewayDevice.WANDevice.WANConnectionDevice.WANEthernetLinkConfig. */

TRXGFUNC(getWANDeviceWANConnectionDeviceWANEthernetLinkConfig_EthernetLinkStatus);
TRxObjNode WANDeviceWANConnectionDeviceWANEthernetLinkConfigDesc[] = {
	{EthernetLinkStatus,{{tString,16}},
	  NULL,
	  getWANDeviceWANConnectionDeviceWANEthernetLinkConfig_EthernetLinkStatus,
	  NULL,NULL},
	{NULL}
};

/** */
/** InternetGatewayDevice.WANDevice.WANConnectionDevice.WANPOTSLinkConfig. */

TRXGFUNC(getWANDeviceWANConnectionDeviceWANPOTSLinkConfig_Enable);
TRXSFUNC(setWANDeviceWANConnectionDeviceWANPOTSLinkConfig_Enable);
TRXGFUNC(getWANDeviceWANConnectionDeviceWANPOTSLinkConfig_LinkStatus);
TRXGFUNC(getWANDeviceWANConnectionDeviceWANPOTSLinkConfig_ISPPhoneNumber);
TRXSFUNC(setWANDeviceWANConnectionDeviceWANPOTSLinkConfig_ISPPhoneNumber);
TRXGFUNC(getWANDeviceWANConnectionDeviceWANPOTSLinkConfig_ISPInfo);
TRXSFUNC(setWANDeviceWANConnectionDeviceWANPOTSLinkConfig_ISPInfo);
TRXGFUNC(getWANDeviceWANConnectionDeviceWANPOTSLinkConfig_LinkType);
TRXSFUNC(setWANDeviceWANConnectionDeviceWANPOTSLinkConfig_LinkType);
TRXGFUNC(getWANDeviceWANConnectionDeviceWANPOTSLinkConfig_NumberOfRetries);
TRXSFUNC(setWANDeviceWANConnectionDeviceWANPOTSLinkConfig_NumberOfRetries);
TRXGFUNC(getWANDeviceWANConnectionDeviceWANPOTSLinkConfig_DelayBetweenRetries);
TRXSFUNC(setWANDeviceWANConnectionDeviceWANPOTSLinkConfig_DelayBetweenRetries);
TRXGFUNC(getWANDeviceWANConnectionDeviceWANPOTSLinkConfig_Fclass);
TRXGFUNC(getWANDeviceWANConnectionDeviceWANPOTSLinkConfig_DataModulationSupported);
TRXGFUNC(getWANDeviceWANConnectionDeviceWANPOTSLinkConfig_DataProtocol);
TRXGFUNC(getWANDeviceWANConnectionDeviceWANPOTSLinkConfig_DataCompression);
TRXGFUNC(getWANDeviceWANConnectionDeviceWANPOTSLinkConfig_PlusVTRCommandSupported);
TRxObjNode WANDeviceWANConnectionDeviceWANPOTSLinkConfigDesc[] = {
	{Enable,{{tBool,0}},
	  setWANDeviceWANConnectionDeviceWANPOTSLinkConfig_Enable,
	  getWANDeviceWANConnectionDeviceWANPOTSLinkConfig_Enable,
	  NULL,NULL},
	{LinkStatus,{{tString,16}},
	  NULL,
	  getWANDeviceWANConnectionDeviceWANPOTSLinkConfig_LinkStatus,
	  NULL,NULL},
	{ISPPhoneNumber,{{tString,64}},
	  setWANDeviceWANConnectionDeviceWANPOTSLinkConfig_ISPPhoneNumber,
	  getWANDeviceWANConnectionDeviceWANPOTSLinkConfig_ISPPhoneNumber,
	  NULL,NULL},
	{ISPInfo,{{tString,64}},
	  setWANDeviceWANConnectionDeviceWANPOTSLinkConfig_ISPInfo,
	  getWANDeviceWANConnectionDeviceWANPOTSLinkConfig_ISPInfo,
	  NULL,NULL},
	{LinkType,{{tString,16}},
	  setWANDeviceWANConnectionDeviceWANPOTSLinkConfig_LinkType,
	  getWANDeviceWANConnectionDeviceWANPOTSLinkConfig_LinkType,
	  NULL,NULL},
	{NumberOfRetries,{{tUnsigned,0}},
	  setWANDeviceWANConnectionDeviceWANPOTSLinkConfig_NumberOfRetries,
	  getWANDeviceWANConnectionDeviceWANPOTSLinkConfig_NumberOfRetries,
	  NULL,NULL},
	{DelayBetweenRetries,{{tUnsigned,0}},
	  setWANDeviceWANConnectionDeviceWANPOTSLinkConfig_DelayBetweenRetries,
	  getWANDeviceWANConnectionDeviceWANPOTSLinkConfig_DelayBetweenRetries,
	  NULL,NULL},
	{Fclass,{{tString,16}},
	  NULL,
	  getWANDeviceWANConnectionDeviceWANPOTSLinkConfig_Fclass,
	  NULL,NULL},
	{DataModulationSupported,{{tString,16}},
	  NULL,
	  getWANDeviceWANConnectionDeviceWANPOTSLinkConfig_DataModulationSupported,
	  NULL,NULL},
	{DataProtocol,{{tString,16}},
	  NULL,
	  getWANDeviceWANConnectionDeviceWANPOTSLinkConfig_DataProtocol,
	  NULL,NULL},
	{DataCompression,{{tString,16}},
	  NULL,
	  getWANDeviceWANConnectionDeviceWANPOTSLinkConfig_DataCompression,
	  NULL,NULL},
	{PlusVTRCommandSupported,{{tBool,0}},
	  NULL,
	  getWANDeviceWANConnectionDeviceWANPOTSLinkConfig_PlusVTRCommandSupported,
	  NULL,NULL},
	{NULL}
};
#endif


/** */
/** InternetGatewayDevice.WANDevice.WANConnectionDevice.WANIPConnection.PortMapping. */

TRXGFUNC(getWANDeviceWANConnectionDeviceWANIPConnectionPortMapping_PortMappingEnabled);
TRXSFUNC(setWANDeviceWANConnectionDeviceWANIPConnectionPortMapping_PortMappingEnabled);
#if 0
TRXGFUNC(getWANDeviceWANConnectionDeviceWANIPConnectionPortMapping_PortMappingLeaseDuration);
TRXSFUNC(setWANDeviceWANConnectionDeviceWANIPConnectionPortMapping_PortMappingLeaseDuration);
TRXGFUNC(getWANDeviceWANConnectionDeviceWANIPConnectionPortMapping_RemoteHost);
TRXSFUNC(setWANDeviceWANConnectionDeviceWANIPConnectionPortMapping_RemoteHost);
#endif
TRXGFUNC(getWANDeviceWANConnectionDeviceWANIPConnectionPortMapping_ExternalPort);
TRXSFUNC(setWANDeviceWANConnectionDeviceWANIPConnectionPortMapping_ExternalPort);
TRXGFUNC(getWANDeviceWANConnectionDeviceWANIPConnectionPortMapping_InternalPort);
TRXSFUNC(setWANDeviceWANConnectionDeviceWANIPConnectionPortMapping_InternalPort);
TRXGFUNC(getWANDeviceWANConnectionDeviceWANIPConnectionPortMapping_PortMappingProtocol);
TRXSFUNC(setWANDeviceWANConnectionDeviceWANIPConnectionPortMapping_PortMappingProtocol);
TRXGFUNC(getWANDeviceWANConnectionDeviceWANIPConnectionPortMapping_InternalClient);
TRXSFUNC(setWANDeviceWANConnectionDeviceWANIPConnectionPortMapping_InternalClient);
TRXGFUNC(getWANDeviceWANConnectionDeviceWANIPConnectionPortMapping_PortMappingDescription);
TRXSFUNC(setWANDeviceWANConnectionDeviceWANIPConnectionPortMapping_PortMappingDescription);
TRxObjNode WANDeviceWANConnectionDeviceWANIPConnectionPortMappingDesc[] = {
	{PortMappingEnabled,{{tBool,0,0}},
	  setWANDeviceWANConnectionDeviceWANIPConnectionPortMapping_PortMappingEnabled,
	  getWANDeviceWANConnectionDeviceWANIPConnectionPortMapping_PortMappingEnabled,
	  NULL,NULL},
#if 0
	//{PortMappingLeaseDuration,{{tUnsigned,0,1}},
	//  setWANDeviceWANConnectionDeviceWANIPConnectionPortMapping_PortMappingLeaseDuration,
	//  getWANDeviceWANConnectionDeviceWANIPConnectionPortMapping_PortMappingLeaseDuration,
	//  NULL,NULL},
	//{RemoteHost,{{tString,16}},
	//  setWANDeviceWANConnectionDeviceWANIPConnectionPortMapping_RemoteHost,
	//  getWANDeviceWANConnectionDeviceWANIPConnectionPortMapping_RemoteHost,
	//  NULL,NULL},
#endif
	{ExternalPort,{{tUnsigned,0,0}},
	  setWANDeviceWANConnectionDeviceWANIPConnectionPortMapping_ExternalPort,
	  getWANDeviceWANConnectionDeviceWANIPConnectionPortMapping_ExternalPort,
	  NULL,NULL},
	{InternalPort,{{tUnsigned,0,0}},
	  setWANDeviceWANConnectionDeviceWANIPConnectionPortMapping_InternalPort,
	  getWANDeviceWANConnectionDeviceWANIPConnectionPortMapping_InternalPort,
	  NULL,NULL},
	{PortMappingProtocol,{{tString,16,0}},
	  setWANDeviceWANConnectionDeviceWANIPConnectionPortMapping_PortMappingProtocol,
	  getWANDeviceWANConnectionDeviceWANIPConnectionPortMapping_PortMappingProtocol,
	  NULL,NULL},
	{InternalClient,{{tString,16,0}},
	  setWANDeviceWANConnectionDeviceWANIPConnectionPortMapping_InternalClient,
	  getWANDeviceWANConnectionDeviceWANIPConnectionPortMapping_InternalClient,
	  NULL,NULL},
	{PortMappingDescription,{{tString,256,0}},
	  setWANDeviceWANConnectionDeviceWANIPConnectionPortMapping_PortMappingDescription,
	  getWANDeviceWANConnectionDeviceWANIPConnectionPortMapping_PortMappingDescription,
	  NULL,NULL},
    {NULL, {{0,0,0}}, NULL, NULL, NULL, NULL}
};

/** */
/** InternetGatewayDevice.WANDevice.WANConnectionDevice.WANIPConnection.PortMapping. */

TRXGFUNC(addWANDeviceWANConnectionDeviceWANIPConnectionPortMappingI);
TRXSFUNC(delWANDeviceWANConnectionDeviceWANIPConnectionPortMappingI);
TRxObjNode WANDeviceWANConnectionDeviceWANIPConnectionPortMappingInstanceDesc[] = {
	{instanceIDMASK,{{0,0,0}},
	  delWANDeviceWANConnectionDeviceWANIPConnectionPortMappingI,
	  addWANDeviceWANConnectionDeviceWANIPConnectionPortMappingI,
	  WANDeviceWANConnectionDeviceWANIPConnectionPortMappingDesc,NULL},
};

/** */
/** InternetGatewayDevice.WANDevice.WANConnectionDevice.WANIPConnection.Stats. */
TRXGFUNC(getWANDeviceWANConnectionDeviceWANIPConnectionStats_EthernetBytesSent);
TRXGFUNC(getWANDeviceWANConnectionDeviceWANIPConnectionStats_EthernetBytesReceived);
TRXGFUNC(getWANDeviceWANConnectionDeviceWANIPConnectionStats_EthernetPacketsSent);
TRXGFUNC(getWANDeviceWANConnectionDeviceWANIPConnectionStats_EthernetPacketsReceived);
TRxObjNode WANDeviceWANConnectionDeviceWANIPConnectionStatsDesc[] = {
	{EthernetBytesSent,{{tUnsigned,0,1}},
	  NULL,
	  getWANDeviceWANConnectionDeviceWANIPConnectionStats_EthernetBytesSent,
	  NULL,NULL},
	{EthernetBytesReceived,{{tUnsigned,0,1}},
	  NULL,
	  getWANDeviceWANConnectionDeviceWANIPConnectionStats_EthernetBytesReceived,
	  NULL,NULL},
	{EthernetPacketsSent,{{tUnsigned,0,1}},
	  NULL,
	  getWANDeviceWANConnectionDeviceWANIPConnectionStats_EthernetPacketsSent,
	  NULL,NULL},
	{EthernetPacketsReceived,{{tUnsigned,0,1}},
	  NULL,
	  getWANDeviceWANConnectionDeviceWANIPConnectionStats_EthernetPacketsReceived,
	  NULL,NULL},
    {NULL, {{0,0,0}}, NULL, NULL, NULL, NULL}
};

/** */
/** InternetGatewayDevice.WANDevice.WANConnectionDevice.WANIPConnection. */

TRXGFUNC(getWANDeviceWANConnectionDeviceWANIPConnection_Enable);
TRXSFUNC(setWANDeviceWANConnectionDeviceWANIPConnection_Enable);
TRXGFUNC(getWANDeviceWANConnectionDeviceWANIPConnection_ConnectionStatus);
TRXGFUNC(getWANDeviceWANConnectionDeviceWANIPConnection_PossibleConnectionTypes);
TRXGFUNC(getWANDeviceWANConnectionDeviceWANIPConnection_ConnectionType);
TRXSFUNC(setWANDeviceWANConnectionDeviceWANIPConnection_ConnectionType);
TRXGFUNC(getWANDeviceWANConnectionDeviceWANIPConnection_Name);
TRXSFUNC(setWANDeviceWANConnectionDeviceWANIPConnection_Name);
TRXGFUNC(getWANDeviceWANConnectionDeviceWANIPConnection_Uptime);
TRXGFUNC(getWANDeviceWANConnectionDeviceWANIPConnection_LastConnectionError);
TRXGFUNC(getWANDeviceWANConnectionDeviceWANIPConnection_AutoDisconnectTime);
TRXSFUNC(setWANDeviceWANConnectionDeviceWANIPConnection_AutoDisconnectTime);
TRXGFUNC(getWANDeviceWANConnectionDeviceWANIPConnection_IdleDisconnectTime);
TRXSFUNC(setWANDeviceWANConnectionDeviceWANIPConnection_IdleDisconnectTime);
TRXGFUNC(getWANDeviceWANConnectionDeviceWANIPConnection_WarnDisconnectDelay);
TRXSFUNC(setWANDeviceWANConnectionDeviceWANIPConnection_WarnDisconnectDelay);
TRXGFUNC(getWANDeviceWANConnectionDeviceWANIPConnection_RSIPAvailable);
TRXGFUNC(getWANDeviceWANConnectionDeviceWANIPConnection_NATEnabled);
TRXSFUNC(setWANDeviceWANConnectionDeviceWANIPConnection_NATEnabled);
TRXGFUNC(getWANDeviceWANConnectionDeviceWANIPConnection_AddressingType);
TRXSFUNC(setWANDeviceWANConnectionDeviceWANIPConnection_AddressingType);
TRXGFUNC(getWANDeviceWANConnectionDeviceWANIPConnection_ExternalIPAddress);
TRXSFUNC(setWANDeviceWANConnectionDeviceWANIPConnection_ExternalIPAddress);
TRXGFUNC(getWANDeviceWANConnectionDeviceWANIPConnection_SubnetMask);
TRXSFUNC(setWANDeviceWANConnectionDeviceWANIPConnection_SubnetMask);
TRXGFUNC(getWANDeviceWANConnectionDeviceWANIPConnection_DefaultGateway);
TRXSFUNC(setWANDeviceWANConnectionDeviceWANIPConnection_DefaultGateway);
TRXGFUNC(getWANDeviceWANConnectionDeviceWANIPConnection_DNSEnabled);
TRXSFUNC(setWANDeviceWANConnectionDeviceWANIPConnection_DNSEnabled);
TRXGFUNC(getWANDeviceWANConnectionDeviceWANIPConnection_DNSOverrideAllowed);
TRXSFUNC(setWANDeviceWANConnectionDeviceWANIPConnection_DNSOverrideAllowed);
TRXGFUNC(getWANDeviceWANConnectionDeviceWANIPConnection_DNSServers);
TRXSFUNC(setWANDeviceWANConnectionDeviceWANIPConnection_DNSServers);
TRXGFUNC(getWANDeviceWANConnectionDeviceWANIPConnection_MaxMTUSize);
TRXSFUNC(setWANDeviceWANConnectionDeviceWANIPConnection_MaxMTUSize);
TRXGFUNC(getWANDeviceWANConnectionDeviceWANIPConnection_MACAddress);
TRXSFUNC(setWANDeviceWANConnectionDeviceWANIPConnection_MACAddress);
TRXGFUNC(getWANDeviceWANConnectionDeviceWANIPConnection_MACAddressOverride);
TRXSFUNC(setWANDeviceWANConnectionDeviceWANIPConnection_MACAddressOverride);
TRXGFUNC(getWANDeviceWANConnectionDeviceWANIPConnection_ConnectionTrigger);
TRXSFUNC(setWANDeviceWANConnectionDeviceWANIPConnection_ConnectionTrigger);
TRXGFUNC(getWANDeviceWANConnectionDeviceWANIPConnection_RouteProtocolRx);
TRXSFUNC(setWANDeviceWANConnectionDeviceWANIPConnection_RouteProtocolRx);
TRXGFUNC(getWANDeviceWANConnectionDeviceWANIPConnection_PortMappingNumberOfEntries);
TRXGFUNC(addWANDeviceWANConnectionDeviceWANIPConnectionI);
TRXSFUNC(delWANDeviceWANConnectionDeviceWANIPConnectionI);
TRxObjNode WANDeviceWANConnectionDeviceWANIPConnectionDesc[] = {
	{Enable,{{tBool,0,0}},
	  setWANDeviceWANConnectionDeviceWANIPConnection_Enable,
	  getWANDeviceWANConnectionDeviceWANIPConnection_Enable,
	  NULL,NULL},
	{ConnectionStatus,{{tString,16,0}},
	  NULL,
	  getWANDeviceWANConnectionDeviceWANIPConnection_ConnectionStatus,
	  NULL,NULL},
	#if 0
	{PossibleConnectionTypes,{{tString,16}},
	  NULL,
	  getWANDeviceWANConnectionDeviceWANIPConnection_PossibleConnectionTypes,
	  NULL,NULL},
	#endif
	{ConnectionType,{{tString,16,0}},
	  setWANDeviceWANConnectionDeviceWANIPConnection_ConnectionType,
	  getWANDeviceWANConnectionDeviceWANIPConnection_ConnectionType,
	  NULL,NULL},
	{Name,{{tString,256,0}},
	  setWANDeviceWANConnectionDeviceWANIPConnection_Name,
	  getWANDeviceWANConnectionDeviceWANIPConnection_Name,
	  NULL,NULL},
	#if 0
	{Uptime,{{tUnsigned,0,1}},
	  NULL,
	  getWANDeviceWANConnectionDeviceWANIPConnection_Uptime,
	  NULL,NULL},
	{LastConnectionError,{{tString,16}},
	  NULL,
	  getWANDeviceWANConnectionDeviceWANIPConnection_LastConnectionError,
	  NULL,NULL},
	{AutoDisconnectTime,{{tUnsigned,0}},
	  setWANDeviceWANConnectionDeviceWANIPConnection_AutoDisconnectTime,
	  getWANDeviceWANConnectionDeviceWANIPConnection_AutoDisconnectTime,
	  NULL,NULL},
	{IdleDisconnectTime,{{tUnsigned,0}},
	  setWANDeviceWANConnectionDeviceWANIPConnection_IdleDisconnectTime,
	  getWANDeviceWANConnectionDeviceWANIPConnection_IdleDisconnectTime,
	  NULL,NULL},
	{WarnDisconnectDelay,{{tUnsigned,0}},
	  setWANDeviceWANConnectionDeviceWANIPConnection_WarnDisconnectDelay,
	  getWANDeviceWANConnectionDeviceWANIPConnection_WarnDisconnectDelay,
	  NULL,NULL},
	{RSIPAvailable,{{tBool,0}},
	  NULL,
	  getWANDeviceWANConnectionDeviceWANIPConnection_RSIPAvailable,
	  NULL,NULL},
	#endif
	{NATEnabled,{{tBool,0,0}},
	  setWANDeviceWANConnectionDeviceWANIPConnection_NATEnabled,
	  getWANDeviceWANConnectionDeviceWANIPConnection_NATEnabled,
	  NULL,NULL},
	{AddressingType,{{tString,16,0}},
	  setWANDeviceWANConnectionDeviceWANIPConnection_AddressingType,
	  getWANDeviceWANConnectionDeviceWANIPConnection_AddressingType,
	  NULL,NULL},
	{ExternalIPAddress,{{tString,16,0}},
	  setWANDeviceWANConnectionDeviceWANIPConnection_ExternalIPAddress,
	  getWANDeviceWANConnectionDeviceWANIPConnection_ExternalIPAddress,
	  NULL,NULL},
	{SubnetMask,{{tString,16,0}},
	  setWANDeviceWANConnectionDeviceWANIPConnection_SubnetMask,
	  getWANDeviceWANConnectionDeviceWANIPConnection_SubnetMask,
	  NULL,NULL},
	{DefaultGateway,{{tString,16,0}},
	  setWANDeviceWANConnectionDeviceWANIPConnection_DefaultGateway,
	  getWANDeviceWANConnectionDeviceWANIPConnection_DefaultGateway,
	  NULL,NULL},
	#if 0
	{DNSEnabled,{{tBool,0}},
	  setWANDeviceWANConnectionDeviceWANIPConnection_DNSEnabled,
	  getWANDeviceWANConnectionDeviceWANIPConnection_DNSEnabled,
	  NULL,NULL},
	{DNSOverrideAllowed,{{tBool,0}},
	  setWANDeviceWANConnectionDeviceWANIPConnection_DNSOverrideAllowed,
	  getWANDeviceWANConnectionDeviceWANIPConnection_DNSOverrideAllowed,
	  NULL,NULL},
	#endif
	{DNSServers,{{tString,16,0}},
	  setWANDeviceWANConnectionDeviceWANIPConnection_DNSServers,
	  getWANDeviceWANConnectionDeviceWANIPConnection_DNSServers,
	  NULL,NULL},
	#if 0
	{MaxMTUSize,{{tUnsigned,0}},
	  setWANDeviceWANConnectionDeviceWANIPConnection_MaxMTUSize,
	  getWANDeviceWANConnectionDeviceWANIPConnection_MaxMTUSize,
	  NULL,NULL},
	{MACAddress,{{tString,16}},
	  setWANDeviceWANConnectionDeviceWANIPConnection_MACAddress,
	  getWANDeviceWANConnectionDeviceWANIPConnection_MACAddress,
	  NULL,NULL},
	{MACAddressOverride,{{tBool,0}},
	  setWANDeviceWANConnectionDeviceWANIPConnection_MACAddressOverride,
	  getWANDeviceWANConnectionDeviceWANIPConnection_MACAddressOverride,
	  NULL,NULL},
	#endif
	{ConnectionTrigger,{{tString,16,0}},
	  setWANDeviceWANConnectionDeviceWANIPConnection_ConnectionTrigger,
	  getWANDeviceWANConnectionDeviceWANIPConnection_ConnectionTrigger,
	  NULL,NULL},
	#if 0
	{RouteProtocolRx,{{tString,16}},
	  setWANDeviceWANConnectionDeviceWANIPConnection_RouteProtocolRx,
	  getWANDeviceWANConnectionDeviceWANIPConnection_RouteProtocolRx,
	  NULL,NULL},
	#endif
	{PortMappingNumberOfEntries,{{tUnsigned,0,0}},
	  NULL,
	  getWANDeviceWANConnectionDeviceWANIPConnection_PortMappingNumberOfEntries,
	  NULL,NULL},
	{PortMapping,{{tInstance,0,0}},
	  NULL,
	  NULL,
	  WANDeviceWANConnectionDeviceWANIPConnectionPortMappingInstanceDesc,NULL},
	{Stats,{{tObject,0,0}},
	  NULL,
	  NULL,
	  WANDeviceWANConnectionDeviceWANIPConnectionStatsDesc,NULL},
    {NULL, {{0,0,0}}, NULL, NULL, NULL, NULL}
};

/** */
/** InternetGatewayDevice.WANDevice.WANConnectionDevice.WANIPConnection. */

TRXGFUNC(addWANDeviceWANConnectionDeviceWANIPConnectionI);
TRXSFUNC(delWANDeviceWANConnectionDeviceWANIPConnectionI);
TRxObjNode WANDeviceWANConnectionDeviceWANIPConnectionInstanceDesc[] = {
	{instanceIDMASK,{{0,0,0}},
	  delWANDeviceWANConnectionDeviceWANIPConnectionI,
	  addWANDeviceWANConnectionDeviceWANIPConnectionI,
	  WANDeviceWANConnectionDeviceWANIPConnectionDesc,NULL},
};

/** */
/** InternetGatewayDevice.WANDevice.WANConnectionDevice.WANPPPConnection.PortMapping. */

TRXGFUNC(getWANDeviceWANConnectionDeviceWANPPPConnectionPortMapping_PortMappingEnabled);
TRXSFUNC(setWANDeviceWANConnectionDeviceWANPPPConnectionPortMapping_PortMappingEnabled);
#if 0
TRXGFUNC(getWANDeviceWANConnectionDeviceWANPPPConnectionPortMapping_PortMappingLeaseDuration);
TRXSFUNC(setWANDeviceWANConnectionDeviceWANPPPConnectionPortMapping_PortMappingLeaseDuration);
TRXGFUNC(getWANDeviceWANConnectionDeviceWANPPPConnectionPortMapping_RemoteHost);
TRXSFUNC(setWANDeviceWANConnectionDeviceWANPPPConnectionPortMapping_RemoteHost);
#endif
TRXGFUNC(getWANDeviceWANConnectionDeviceWANPPPConnectionPortMapping_ExternalPort);
TRXSFUNC(setWANDeviceWANConnectionDeviceWANPPPConnectionPortMapping_ExternalPort);
TRXGFUNC(getWANDeviceWANConnectionDeviceWANPPPConnectionPortMapping_InternalPort);
TRXSFUNC(setWANDeviceWANConnectionDeviceWANPPPConnectionPortMapping_InternalPort);
TRXGFUNC(getWANDeviceWANConnectionDeviceWANPPPConnectionPortMapping_PortMappingProtocol);
TRXSFUNC(setWANDeviceWANConnectionDeviceWANPPPConnectionPortMapping_PortMappingProtocol);
TRXGFUNC(getWANDeviceWANConnectionDeviceWANPPPConnectionPortMapping_InternalClient);
TRXSFUNC(setWANDeviceWANConnectionDeviceWANPPPConnectionPortMapping_InternalClient);
TRXGFUNC(getWANDeviceWANConnectionDeviceWANPPPConnectionPortMapping_PortMappingDescription);
TRXSFUNC(setWANDeviceWANConnectionDeviceWANPPPConnectionPortMapping_PortMappingDescription);
TRxObjNode WANDeviceWANConnectionDeviceWANPPPConnectionPortMappingDesc[] = {
	{PortMappingEnabled,{{tBool,0,0}},
	  setWANDeviceWANConnectionDeviceWANPPPConnectionPortMapping_PortMappingEnabled,
	  getWANDeviceWANConnectionDeviceWANPPPConnectionPortMapping_PortMappingEnabled,
	  NULL,NULL},
#if 0
	//{PortMappingLeaseDuration,{{tUnsigned,0,1}},
	//  setWANDeviceWANConnectionDeviceWANPPPConnectionPortMapping_PortMappingLeaseDuration,
	//  getWANDeviceWANConnectionDeviceWANPPPConnectionPortMapping_PortMappingLeaseDuration,
	//  NULL,NULL},
	//{RemoteHost,{{tString,16}},
	//  setWANDeviceWANConnectionDeviceWANPPPConnectionPortMapping_RemoteHost,
	//  getWANDeviceWANConnectionDeviceWANPPPConnectionPortMapping_RemoteHost,
	//  NULL,NULL},
#endif
	{ExternalPort,{{tUnsigned,0,0}},
	  setWANDeviceWANConnectionDeviceWANPPPConnectionPortMapping_ExternalPort,
	  getWANDeviceWANConnectionDeviceWANPPPConnectionPortMapping_ExternalPort,
	  NULL,NULL},
	{InternalPort,{{tUnsigned,0,0}},
	  setWANDeviceWANConnectionDeviceWANPPPConnectionPortMapping_InternalPort,
	  getWANDeviceWANConnectionDeviceWANPPPConnectionPortMapping_InternalPort,
	  NULL,NULL},
	{PortMappingProtocol,{{tString,16,0}},
	  setWANDeviceWANConnectionDeviceWANPPPConnectionPortMapping_PortMappingProtocol,
	  getWANDeviceWANConnectionDeviceWANPPPConnectionPortMapping_PortMappingProtocol,
	  NULL,NULL},
	{InternalClient,{{tString,16,0}},
	  setWANDeviceWANConnectionDeviceWANPPPConnectionPortMapping_InternalClient,
	  getWANDeviceWANConnectionDeviceWANPPPConnectionPortMapping_InternalClient,
	  NULL,NULL},
	{PortMappingDescription,{{tString,256,0}},
	  setWANDeviceWANConnectionDeviceWANPPPConnectionPortMapping_PortMappingDescription,
	  getWANDeviceWANConnectionDeviceWANPPPConnectionPortMapping_PortMappingDescription,
	  NULL,NULL},
    {NULL, {{0,0,0}}, NULL, NULL, NULL, NULL}
};

/** */
/** InternetGatewayDevice.WANDevice.WANConnectionDevice.WANPPPConnection.PortMapping. */

TRXGFUNC(addWANDeviceWANConnectionDeviceWANPPPConnectionPortMappingI);
TRXSFUNC(delWANDeviceWANConnectionDeviceWANPPPConnectionPortMappingI);
TRxObjNode WANDeviceWANConnectionDeviceWANPPPConnectionPortMappingInstanceDesc[] = {
	{instanceIDMASK,{{0,0,0}},
	  delWANDeviceWANConnectionDeviceWANPPPConnectionPortMappingI,
	  addWANDeviceWANConnectionDeviceWANPPPConnectionPortMappingI,
	  WANDeviceWANConnectionDeviceWANPPPConnectionPortMappingDesc,NULL},
};

/** */
/** InternetGatewayDevice.WANDevice.WANConnectionDevice.WANPPPConnection.Stats. */
TRXGFUNC(getWANDeviceWANConnectionDeviceWANPPPConnectionStats_EthernetBytesSent);
TRXGFUNC(getWANDeviceWANConnectionDeviceWANPPPConnectionStats_EthernetBytesReceived);
TRXGFUNC(getWANDeviceWANConnectionDeviceWANPPPConnectionStats_EthernetPacketsSent);
TRXGFUNC(getWANDeviceWANConnectionDeviceWANPPPConnectionStats_EthernetPacketsReceived);
TRxObjNode WANDeviceWANConnectionDeviceWANPPPConnectionStatsDesc[] = {
	{EthernetBytesSent,{{tUnsigned,0,1}},
	  NULL,
	  getWANDeviceWANConnectionDeviceWANPPPConnectionStats_EthernetBytesSent,
	  NULL,NULL},
	{EthernetBytesReceived,{{tUnsigned,0,1}},
	  NULL,
	  getWANDeviceWANConnectionDeviceWANPPPConnectionStats_EthernetBytesReceived,
	  NULL,NULL},
	{EthernetPacketsSent,{{tUnsigned,0,1}},
	  NULL,
	  getWANDeviceWANConnectionDeviceWANPPPConnectionStats_EthernetPacketsSent,
	  NULL,NULL},
	{EthernetPacketsReceived,{{tUnsigned,0,1}},
	  NULL,
	  getWANDeviceWANConnectionDeviceWANPPPConnectionStats_EthernetPacketsReceived,
	  NULL,NULL},
    {NULL, {{0,0,0}}, NULL, NULL, NULL, NULL}
};
TRXSFUNC(setPPPEnable);
TRXGFUNC(getPPPEnable);
TRXGFUNC(getPPPStatus);
TRXSFUNC(setPPPType);
TRXGFUNC(getPPPType);
TRXSFUNC(setPPPName);
TRXGFUNC(getPPPName);
TRXSFUNC(setPPPIdleTime);
TRXGFUNC(getPPPIdleTime);
TRXSFUNC(setPPPNATEnabled);
TRXGFUNC(getPPPNATEnabled);
TRXSFUNC(setPPPUsername);
TRXGFUNC(getPPPUsername);
TRXSFUNC(setPPPPassword);
TRXGFUNC(getPPPPassword);
TRXSFUNC(setWanPPPAddressingType);
TRXGFUNC(getWanPPPAddressingType);
TRXSFUNC(setWanPPPExtIP);
TRXGFUNC(getWanPPPExtIP);
TRXSFUNC(setPPPDNSServers);
TRXGFUNC(getPPPDNSServers);
TRXSFUNC(setPPPConTrigger);
TRXGFUNC(getPPPConTrigger);
TRXGFUNC(getWANDeviceWANConnectionDeviceWANPPPConnection_NATEnabled);
TRXSFUNC(setWANDeviceWANConnectionDeviceWANPPPConnection_NATEnabled);
#if 0
TRXGFUNC(getWANDeviceWANConnectionDeviceWANPPPConnection_PossibleConnectionTypes);
TRXGFUNC(getWANDeviceWANConnectionDeviceWANPPPConnection_ConnectionType);
TRXSFUNC(setWANDeviceWANConnectionDeviceWANPPPConnection_ConnectionType);
TRXGFUNC(getWANDeviceWANConnectionDeviceWANPPPConnection_Name);
TRXSFUNC(setWANDeviceWANConnectionDeviceWANPPPConnection_Name);
TRXGFUNC(getWANDeviceWANConnectionDeviceWANPPPConnection_Uptime);
TRXGFUNC(getWANDeviceWANConnectionDeviceWANPPPConnection_LastConnectionError);
TRXGFUNC(getWANDeviceWANConnectionDeviceWANPPPConnection_AutoDisconnectTime);
TRXSFUNC(setWANDeviceWANConnectionDeviceWANPPPConnection_AutoDisconnectTime);
TRXGFUNC(getWANDeviceWANConnectionDeviceWANPPPConnection_IdleDisconnectTime);
TRXSFUNC(setWANDeviceWANConnectionDeviceWANPPPConnection_IdleDisconnectTime);
TRXGFUNC(getWANDeviceWANConnectionDeviceWANPPPConnection_WarnDisconnectDelay);
TRXSFUNC(setWANDeviceWANConnectionDeviceWANPPPConnection_WarnDisconnectDelay);
TRXGFUNC(getWANDeviceWANConnectionDeviceWANPPPConnection_RSIPAvailable);
TRXGFUNC(getWANDeviceWANConnectionDeviceWANPPPConnection_Username);
TRXSFUNC(setWANDeviceWANConnectionDeviceWANPPPConnection_Username);
TRXSFUNC(setWANDeviceWANConnectionDeviceWANPPPConnection_Password);
TRXGFUNC(getWANDeviceWANConnectionDeviceWANPPPConnection_PPPEncryptionProtocol);
TRXGFUNC(getWANDeviceWANConnectionDeviceWANPPPConnection_PPPCompressionProtocol);
TRXGFUNC(getWANDeviceWANConnectionDeviceWANPPPConnection_PPPAuthenticationProtocol);
TRXGFUNC(getWANDeviceWANConnectionDeviceWANPPPConnection_ExternalIPAddress);
TRXGFUNC(getWANDeviceWANConnectionDeviceWANPPPConnection_RemoteIPAddress);
TRXGFUNC(getWANDeviceWANConnectionDeviceWANPPPConnection_MaxMRUSize);
TRXSFUNC(setWANDeviceWANConnectionDeviceWANPPPConnection_MaxMRUSize);
TRXGFUNC(getWANDeviceWANConnectionDeviceWANPPPConnection_CurrentMRUSize);
TRXGFUNC(getWANDeviceWANConnectionDeviceWANPPPConnection_DNSEnabled);
TRXSFUNC(setWANDeviceWANConnectionDeviceWANPPPConnection_DNSEnabled);
TRXGFUNC(getWANDeviceWANConnectionDeviceWANPPPConnection_DNSOverrideAllowed);
TRXSFUNC(setWANDeviceWANConnectionDeviceWANPPPConnection_DNSOverrideAllowed);
TRXGFUNC(getWANDeviceWANConnectionDeviceWANPPPConnection_DNSServers);
TRXSFUNC(setWANDeviceWANConnectionDeviceWANPPPConnection_DNSServers);
TRXGFUNC(getWANDeviceWANConnectionDeviceWANPPPConnection_MACAddress);
TRXSFUNC(setWANDeviceWANConnectionDeviceWANPPPConnection_MACAddress);
TRXGFUNC(getWANDeviceWANConnectionDeviceWANPPPConnection_MACAddressOverride);
TRXSFUNC(setWANDeviceWANConnectionDeviceWANPPPConnection_MACAddressOverride);
TRXGFUNC(getWANDeviceWANConnectionDeviceWANPPPConnection_TransportType);
TRXGFUNC(getWANDeviceWANConnectionDeviceWANPPPConnection_PPPoEACName);
TRXSFUNC(setWANDeviceWANConnectionDeviceWANPPPConnection_PPPoEACName);
TRXGFUNC(getWANDeviceWANConnectionDeviceWANPPPConnection_PPPoEServiceName);
TRXSFUNC(setWANDeviceWANConnectionDeviceWANPPPConnection_PPPoEServiceName);
TRXGFUNC(getWANDeviceWANConnectionDeviceWANPPPConnection_ConnectionTrigger);
TRXSFUNC(setWANDeviceWANConnectionDeviceWANPPPConnection_ConnectionTrigger);
TRXGFUNC(getWANDeviceWANConnectionDeviceWANPPPConnection_RouteProtocolRx);
TRXSFUNC(setWANDeviceWANConnectionDeviceWANPPPConnection_RouteProtocolRx);
TRXGFUNC(getWANDeviceWANConnectionDeviceWANPPPConnection_PPPLCPEcho);
TRXGFUNC(getWANDeviceWANConnectionDeviceWANPPPConnection_PPPLCPEchoRetry);
#endif
TRXGFUNC(getWANDeviceWANConnectionDeviceWANPPPConnection_PortMappingNumberOfEntries);
TRXGFUNC(pseudoGetIFName);
/* InternetGatewayDevice.WANDevice.{i}.WANConnectionDevice.WANPPPConnection.{i}. */
TRxObjNode  WANDeviceWANConnectionDeviceWANPPPConnectionDesc[] = {
    {Enable,{{tBool,0,0}},setPPPEnable,getPPPEnable,NULL,NULL},
    {ConnectionStatus,{{tString,16,0}}, NULL,getPPPStatus,NULL,NULL},
	#if 0
	{PossibleConnectionTypes,{{tString,16}},
	  NULL,
	  getWANDeviceWANConnectionDeviceWANPPPConnection_PossibleConnectionTypes,
	  NULL,NULL},
	#endif
    {ConnectionType,{{tString,16,0}},setPPPType,getPPPType,NULL,NULL},
    {Name,{{tString,16,0}},setPPPName,getPPPName,NULL,NULL},
	{"#ifname",{{tUnsigned,0,0}}, NULL, pseudoGetIFName, NULL,NULL},
	#if 0
	{Uptime,{{tUnsigned,0,1}},
	  NULL,
	  getWANDeviceWANConnectionDeviceWANPPPConnection_Uptime,
	  NULL,NULL},
	{LastConnectionError,{{tString,16}},
	  NULL,
	  getWANDeviceWANConnectionDeviceWANPPPConnection_LastConnectionError,
	  NULL,NULL},
	{AutoDisconnectTime,{{tUnsigned,0}},
	  setWANDeviceWANConnectionDeviceWANPPPConnection_AutoDisconnectTime,
	  getWANDeviceWANConnectionDeviceWANPPPConnection_AutoDisconnectTime,
	  NULL,NULL},
	#endif
    {IdleDisconnectTime,{{tUnsigned,0,0}},setPPPIdleTime,getPPPIdleTime,NULL,NULL},
	#if 0
	{WarnDisconnectDelay,{{tUnsigned,0}},
	  setWANDeviceWANConnectionDeviceWANPPPConnection_WarnDisconnectDelay,
	  getWANDeviceWANConnectionDeviceWANPPPConnection_WarnDisconnectDelay,
	  NULL,NULL},
	{RSIPAvailable,{{tBool,0}},
	  NULL,
	  getWANDeviceWANConnectionDeviceWANPPPConnection_RSIPAvailable,
	  NULL,NULL},
	#endif 
	{NATEnabled,{{tBool,0,0}},
	  setWANDeviceWANConnectionDeviceWANPPPConnection_NATEnabled,
	  getWANDeviceWANConnectionDeviceWANPPPConnection_NATEnabled,
	  NULL,NULL},
    /*{NATEnabled,{{tBool}},setPPPNATEnabled,getPPPNATEnabled,NULL,NULL},*/
    {Username,{{tString,16,0}},setPPPUsername,getPPPUsername,NULL,NULL},
    {Password,{{tStringSOnly,16,0}},setPPPPassword,getPPPPassword,NULL,NULL},
	#if 0
	{PPPEncryptionProtocol,{{tString,16}},
	  NULL,
	  getWANDeviceWANConnectionDeviceWANPPPConnection_PPPEncryptionProtocol,
	  NULL,NULL},
	{PPPCompressionProtocol,{{tString,16}},
	  NULL,
	  getWANDeviceWANConnectionDeviceWANPPPConnection_PPPCompressionProtocol,
	  NULL,NULL},
	{PPPAuthenticationProtocol,{{tString,16}},
	  NULL,
	  getWANDeviceWANConnectionDeviceWANPPPConnection_PPPAuthenticationProtocol,
	  NULL,NULL},
	#endif
	{AddressingType,{{tString,16,0}}, setWanPPPAddressingType, getWanPPPAddressingType,
	  NULL,NULL},
	{ExternalIPAddress,{{tString,16,0}}, setWanPPPExtIP, getWanPPPExtIP, NULL,NULL},
	#if 0
	{RemoteIPAddress,{{tString,16}},
	  NULL,
	  getWANDeviceWANConnectionDeviceWANPPPConnection_RemoteIPAddress,
	  NULL,NULL},
	{MaxMRUSize,{{tUnsigned,0}},
	  setWANDeviceWANConnectionDeviceWANPPPConnection_MaxMRUSize,
	  getWANDeviceWANConnectionDeviceWANPPPConnection_MaxMRUSize,
	  NULL,NULL},
	{CurrentMRUSize,{{tUnsigned,0,1}},
	  NULL,
	  getWANDeviceWANConnectionDeviceWANPPPConnection_CurrentMRUSize,
	  NULL,NULL},
	{DNSEnabled,{{tBool,0}},
	  setWANDeviceWANConnectionDeviceWANPPPConnection_DNSEnabled,
	  getWANDeviceWANConnectionDeviceWANPPPConnection_DNSEnabled,
	  NULL,NULL},
	{DNSOverrideAllowed,{{tBool,0}},
	  setWANDeviceWANConnectionDeviceWANPPPConnection_DNSOverrideAllowed,
	  getWANDeviceWANConnectionDeviceWANPPPConnection_DNSOverrideAllowed,
	  NULL,NULL},
	#endif
    {DNSServers,{{tString,16,0}},setPPPDNSServers,getPPPDNSServers,NULL,NULL},
	#if 0
    {MACAddress,{{tString,16}},
	  setWANDeviceWANConnectionDeviceWANPPPConnection_MACAddress,
	  getWANDeviceWANConnectionDeviceWANPPPConnection_MACAddress,
	  NULL,NULL},
	{MACAddressOverride,{{tBool,0}},
	  setWANDeviceWANConnectionDeviceWANPPPConnection_MACAddressOverride,
	  getWANDeviceWANConnectionDeviceWANPPPConnection_MACAddressOverride,
	  NULL,NULL},
	{TransportType,{{tString,16}},
	  NULL,
	  getWANDeviceWANConnectionDeviceWANPPPConnection_TransportType,
	  NULL,NULL},
	{PPPoEACName,{{tString,256}},
	  setWANDeviceWANConnectionDeviceWANPPPConnection_PPPoEACName,
	  getWANDeviceWANConnectionDeviceWANPPPConnection_PPPoEACName,
	  NULL,NULL},
	{PPPoEServiceName,{{tString,256}},
	  setWANDeviceWANConnectionDeviceWANPPPConnection_PPPoEServiceName,
	  getWANDeviceWANConnectionDeviceWANPPPConnection_PPPoEServiceName,
	  NULL,NULL},
	#endif
    {ConnectionTrigger,{{tString,16,0}},setPPPConTrigger,getPPPConTrigger,NULL,NULL},
	#if 0
	{RouteProtocolRx,{{tString,16}},
	  setWANDeviceWANConnectionDeviceWANPPPConnection_RouteProtocolRx,
	  getWANDeviceWANConnectionDeviceWANPPPConnection_RouteProtocolRx,
	  NULL,NULL},
	{PPPLCPEcho,{{tUnsigned,0}},
	  NULL,
	  getWANDeviceWANConnectionDeviceWANPPPConnection_PPPLCPEcho,
	  NULL,NULL},
	{PPPLCPEchoRetry,{{tUnsigned,0}},
	  NULL,
	  getWANDeviceWANConnectionDeviceWANPPPConnection_PPPLCPEchoRetry,
	  NULL,NULL},
	#endif
	{PortMappingNumberOfEntries,{{tUnsigned,0,0}},
	  NULL,
	  getWANDeviceWANConnectionDeviceWANPPPConnection_PortMappingNumberOfEntries,
	  NULL,NULL},
	{PortMapping,{{tInstance,0,0}},
	  NULL,
	  NULL,
	  WANDeviceWANConnectionDeviceWANPPPConnectionPortMappingInstanceDesc,NULL},
	{Stats,{{tObject,0,0}},
	  NULL,
	  NULL,
	  WANDeviceWANConnectionDeviceWANPPPConnectionStatsDesc,NULL},
    {NULL, {{0,0,0}}, NULL, NULL, NULL, NULL}
};
TRXGFUNC(addPPPConnection);
TRXSFUNC(deletePPPConnection);
/* InternetGatewayDevice.WANDevice.{i}.WANConnectionDevice.WANPPPConnection. */
TRxObjNode  WANDeviceWANConnectionDeviceWANPPPConnectionInstanceDesc[] = {
    {instanceIDMASK,{{0,0,0}},
	   deletePPPConnection,
	   addPPPConnection,
	   WANDeviceWANConnectionDeviceWANPPPConnectionDesc,NULL},
};

TRXGFUNC(getPPPConNumberEntries);
TRXGFUNC(getWANDeviceWANConnectionDevice_WANIPConnectionNumberOfEntries);
/* InternetGatewayDevice.WANDevice.{i}.WANConnectionDevice.{i}. */
TRxObjNode  wanConnectionDeviceDesc[] = {
    {WANIPConnectionNumberOfEntries,{{tUnsigned,0,0}}, NULL,getWANDeviceWANConnectionDevice_WANIPConnectionNumberOfEntries,NULL,NULL},
    {WANPPPConnectionNumberOfEntries,{{tUnsigned,0,0}}, NULL,getPPPConNumberEntries,NULL,NULL},
	{WANDSLLinkConfig,{{tObject,0,0}},
	  NULL,
	  NULL,
	  WANDeviceWANConnectionDeviceWANDSLLinkConfigDesc,NULL},
	{WANATMF5LoopbackDiagnostics,{{tObject,0,0}},
	  NULL,
	  NULL,
	  WANDeviceWANConnectionDeviceWANATMF5LoopbackDiagnosticsDesc,NULL},
	#if 0
	{WANEthernetLinkConfig,{{tObject,0}},
	  NULL,
	  NULL,
	  WANDeviceWANConnectionDeviceWANEthernetLinkConfigDesc},
	{WANPOTSLinkConfig,{{tObject,0}},
	  NULL,
	  NULL,
	  WANDeviceWANConnectionDeviceWANPOTSLinkConfigDesc},
	#endif
	{WANIPConnection,{{tInstance,0,0}},
	  NULL,
	  NULL,
	  WANDeviceWANConnectionDeviceWANIPConnectionInstanceDesc,NULL},
    {WANPPPConnection,{{tInstance,0,0}},
	  NULL,
	  NULL,
	  WANDeviceWANConnectionDeviceWANPPPConnectionInstanceDesc,NULL},
    {NULL, {{0,0,0}}, NULL, NULL, NULL, NULL}
};
TRXGFUNC(addWANConnectionDevice);
TRXSFUNC(deleteWANConnectionDevice);
/* InternetGatewayDevice.WANDevice.{i}.WANConnectionDevice. */
TRxObjNode  WANDeviceWANConnectionDeviceInstanceDesc[] = {
    {instanceIDMASK,{{0,0,0}},
	   deleteWANConnectionDevice,
	   addWANConnectionDevice,
	   wanConnectionDeviceDesc,NULL},
};

TRXGFUNC(getWANDevice_WANConnectionNumberOfEntries);
/* InternetGatewayDevice.WANDevice.{i}. */
TRxObjNode WANDeviceDesc[] = {
	{WANConnectionNumberOfEntries,{{tUnsigned,0,0}},
	  NULL,
	  getWANDevice_WANConnectionNumberOfEntries,
	  NULL,NULL},
	{WANCommonInterfaceConfig,{{tObject,0,0}},
	  NULL,
	  NULL,
	  WANDeviceWANCommonInterfaceConfigDesc,NULL},
	{WANDSLInterfaceConfig,{{tObject,0,0}},
	  NULL,
	  NULL,
	  WANDeviceWANDSLInterfaceConfigDesc,NULL},
	#if 0
	{WANEthernetInterfaceConfig,{{tObject,0}},
	  NULL,
	  NULL,
	  WANDeviceWANEthernetInterfaceConfigDesc},
	#endif
	{WANDSLConnectionManagement,{{tObject,0,0}},
	  NULL,
	  NULL,
	  WANDeviceWANDSLConnectionManagementDesc,NULL},
	{WANDSLDiagnostics,{{tObject,0,0}},
	  NULL,
	  NULL,
	  WANDeviceWANDSLDiagnosticsDesc,NULL},
	{WANConnectionDevice,{{tInstance,0,0}},
	  NULL,
	  NULL,
	  WANDeviceWANConnectionDeviceInstanceDesc, NULL},
    {NULL, {{0,0,0}}, NULL, NULL, NULL, NULL}
};

/* InternetGatewayDevice.WANDevice. */
/* Since the WANDevice instances can't be Add or Deleted the setter/getter funcs are NULL*/
/** */
/** InternetGatewayDevice.WANDevice. */

TRxObjNode WANDeviceInstanceDesc[] = {
	{instanceIDMASK,{{0, 0, 0}},
	  NULL,
	  NULL,
	  WANDeviceDesc,NULL}
};

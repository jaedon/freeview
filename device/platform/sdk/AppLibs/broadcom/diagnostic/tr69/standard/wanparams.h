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
//  Filename:       wanparams.h
//
******************************************************************************
//  Description:
//             WANDevice profile supported object varable names
//
//
*****************************************************************************/

#ifndef WAN_PARAMS_H
#define WAN_PARAMS_H

#include "../inc/tr69cdefs.h"

/* */
/* InternetGatewayDevice.WANDevice.WANCommonInterfaceConfig.Connection. */

SVAR(ActiveConnectionDeviceContainer);
SVAR(ActiveConnectionServiceID);

/* */
/* InternetGatewayDevice.WANDevice.WANCommonInterfaceConfig.Connection. */


/* */
/* InternetGatewayDevice.WANDevice.WANCommonInterfaceConfig. */

SVAR(WANCommonInterfaceConfig);
SVAR(EnabledForInternet);
SVAR(WANAccessType);
SVAR(Layer1UpstreamMaxBitRate);
SVAR(Layer1DownstreamMaxBitRate);
SVAR(PhysicalLinkStatus);
SVAR(WANAccessProvider);
#if 0
//SVAR(TotalBytesSent);
//SVAR(TotalBytesReceived);
//SVAR(TotalPacketsReceived);
#endif
SVAR(MaximumActiveConnections);
SVAR(NumberOfActiveConnections);
SVAR(Connection);

/* */
/* InternetGatewayDevice.WANDevice.WANDSLInterfaceConfig.Stats.Total. */

SVAR(ReceiveBlocks);
SVAR(TransmitBlocks);
SVAR(CellDelin);
SVAR(LinkRetrain);
SVAR(InitErrors);
SVAR(InitTimeouts);
SVAR(LossOfFraming);
SVAR(ErroredSecs);
SVAR(SeverelyErroredSecs);
SVAR(FECErrors);
SVAR(ATUCFECErrors);
SVAR(HECErrors);
SVAR(ATUCHECErrors);
SVAR(CRCErrors);
SVAR(ATUCCRCErrors);


/* */
/* InternetGatewayDevice.WANDevice.WANDSLInterfaceConfig.Stats. */

SVAR(Total);
SVAR(Showtime);
SVAR(LastShowtime);
SVAR(CurrentDay);
SVAR(QuarterHour);

/* */
/* InternetGatewayDevice.WANDevice.WANDSLInterfaceConfig. */

SVAR(WANDSLInterfaceConfig);
#if 0
//SVAR(Enable);
//SVAR(Status);
#endif
SVAR(ModulationType);
SVAR(LineEncoding);
SVAR(DataPath);
SVAR(InterleaveDepth);
SVAR(LineNumber);
SVAR(UpstreamCurrRate);
SVAR(DownstreamCurrRate);
SVAR(UpstreamMaxRate);
SVAR(DownstreamMaxRate);
SVAR(UpstreamNoiseMargin);
SVAR(DownstreamNoiseMargin);
SVAR(UpstreamAttenuation);
SVAR(DownstreamAttenuation);
SVAR(UpstreamPower);
SVAR(DownstreamPower);
SVAR(ATURVendor);
SVAR(ATURCountry);
SVAR(ATURANSIStd);
SVAR(ATURANSIRev);
SVAR(ATUCVendor);
SVAR(ATUCCountry);
SVAR(ATUCANSIStd);
SVAR(ATUCANSIRev);
SVAR(TotalStart);
SVAR(ShowtimeStart);
SVAR(LastShowtimeStart);
SVAR(CurrentDayStart);
SVAR(QuarterHourStart);
/*SVAR(Stats);*/


/* */
/* InternetGatewayDevice.WANDevice.WANDSLConnectionManagement.ConnectionService. */
SVAR(WANDSLConnectionManagement);
SVAR(WANConnectionDevice);
SVAR(WANConnectionService);
SVAR(DestinationAddress);
SVAR(LinkType);
SVAR(ConnectionType);

/* */
/* InternetGatewayDevice.WANDevice.WANDSLConnectionManagement.ConnectionService. */


/* */
/* InternetGatewayDevice.WANDevice.WANDSLConnectionManagement. */

SVAR(ConnectionServiceNumberOfEntries);
SVAR(ConnectionService);

/* */
/* InternetGatewayDevice.WANDevice.WANDSLDiagnostics. */

SVAR(LoopDiagnosticsState);
SVAR(WANDSLDiagnostics);
SVAR(ACTPSDds);
SVAR(ACTPSDus);
SVAR(ACTATPds);
SVAR(ACTATPus);
SVAR(HLINSCds);
SVAR(HLINpsds);
SVAR(QLNpsds);
SVAR(SNRpsds);
SVAR(BITSpsds);
SVAR(GAINSpsds);

/* */
/* InternetGatewayDevice.WANDevice.WANConnectionDevice.WANDSLLinkConfig. */

/*SVAR(Enable);*/
SVAR(LinkStatus);
/*SVAR(LinkType);*/
SVAR(AutoConfig);
#if 0
//SVAR(ModulationType);
//SVAR(DestinationAddress);
#endif
SVAR(ATMEncapsulation);
SVAR(FCSPreserved);
SVAR(VCSearchList);
SVAR(ATMAAL);
SVAR(ATMTransmittedBlocks);
SVAR(ATMReceivedBlocks);
SVAR(ATMQoS);
SVAR(ATMPeakCellRate);
SVAR(ATMMaximumBurstSize);
SVAR(ATMSustainableCellRate);
SVAR(AAL5CRCErrors);
SVAR(ATMCRCErrors);
SVAR(ATMHECErrors);

/* */
/* InternetGatewayDevice.WANDevice.WANConnectionDevice.WANEthernetLinkConfig. */

SVAR(EthernetLinkStatus);

/* */
/* InternetGatewayDevice.WANDevice.WANConnectionDevice.WANPOTSLinkConfig. */

/*SVAR(Enable);*/
/*SVAR(LinkStatus);	*/
SVAR(ISPPhoneNumber);
SVAR(ISPInfo);
/*SVAR(LinkType); */
SVAR(NumberOfRetries);
SVAR(DelayBetweenRetries);
SVAR(Fclass);
SVAR(DataModulationSupported);
SVAR(DataProtocol);
SVAR(DataCompression);
SVAR(PlusVTRCommandSupported);

/* */
/* InternetGatewayDevice.WANDevice.WANConnectionDevice.WANIPConnection.PortMapping. */

SVAR(PortMappingEnabled);
SVAR(PortMappingLeaseDuration);
SVAR(RemoteHost);
SVAR(ExternalPort);
SVAR(InternalPort);
SVAR(PortMappingProtocol);
SVAR(InternalClient);
SVAR(PortMappingDescription);

/* */
/* InternetGatewayDevice.WANDevice.WANConnectionDevice.WANIPConnection.PortMapping. */


/* */
/* InternetGatewayDevice.WANDevice.WANConnectionDevice.WANIPConnection.Stats. */

SVAR(EthernetBytesSent);
SVAR(EthernetBytesReceived);
SVAR(EthernetPacketsSent);
SVAR(EthernetPacketsReceived);

/* */
/* InternetGatewayDevice.WANDevice.WANConnectionDevice.WANIPConnection. */

/*SVAR(Enable);	*/
/*SVAR(ConnectionStatus); */
SVAR(PossibleConnectionTypes);
/*SVAR(ConnectionType);	*/
SVAR(Uptime);
SVAR(LastConnectionError);
SVAR(AutoDisconnectTime);
SVAR(IdleDisconnectTime);
SVAR(WarnDisconnectDelay);
SVAR(RSIPAvailable);
SVAR(NATEnabled);
SVAR(AddressingType);
SVAR(ExternalIPAddress);
/*SVAR(SubnetMask);	*/
SVAR(DefaultGateway);
SVAR(DNSEnabled);
SVAR(DNSOverrideAllowed);
/*SVAR(DNSServers);	 */
SVAR(MaxMTUSize);
/*SVAR(MACAddress);	*/
SVAR(MACAddressOverride);
SVAR(ConnectionTrigger);
SVAR(RouteProtocolRx);
SVAR(PortMappingNumberOfEntries);
SVAR(PortMapping);
/*SVAR(Stats); */

/* */
/* InternetGatewayDevice.WANDevice.WANConnectionDevice.WANIPConnection. */


/* */
/* InternetGatewayDevice.WANDevice.WANConnectionDevice.WANPPPConnection.PortMapping. */

#if 0
//SVAR(PortMappingEnabled);
//SVAR(PortMappingLeaseDuration);
//SVAR(RemoteHost);
//SVAR(ExternalPort);
//SVAR(InternalPort);
//SVAR(PortMappingProtocol);
//SVAR(InternalClient);
//SVAR(PortMappingDescription);
#endif
/* */
/* InternetGatewayDevice.WANDevice.WANConnectionDevice.WANPPPConnection.PortMapping. */


/* */
/* InternetGatewayDevice.WANDevice.WANConnectionDevice.WANPPPConnection.Stats. */

#if 0
//SVAR(EthernetBytesSent);
//SVAR(EthernetBytesReceived);
//SVAR(EthernetPacketsSent);
//SVAR(EthernetPacketsReceived);
#endif
/* */
/* InternetGatewayDevice.WANDevice.WANConnectionDevice.WANPPPConnection. */

/*SVAR(Enable);	*/
SVAR(ConnectionStatus);
#if 0
//SVAR(PossibleConnectionTypes);
//SVAR(ConnectionType);
//SVAR(Uptime);
//SVAR(LastConnectionError);
//SVAR(AutoDisconnectTime);
//SVAR(IdleDisconnectTime);
//SVAR(WarnDisconnectDelay);
//SVAR(RSIPAvailable);
//SVAR(NATEnabled);
#endif
SVAR(PPPEncryptionProtocol);
SVAR(PPPCompressionProtocol);
SVAR(PPPAuthenticationProtocol);
/*SVAR(ExternalIPAddress); */
SVAR(RemoteIPAddress);
SVAR(MaxMRUSize);
SVAR(CurrentMRUSize);
#if 0
//SVAR(DNSEnabled);
//SVAR(DNSOverrideAllowed);
//SVAR(DNSServers);
//SVAR(MACAddress);
//SVAR(MACAddressOverride);
#endif
SVAR(TransportType);
SVAR(PPPoEACName);
SVAR(PPPoEServiceName);
#if 0
//SVAR(ConnectionTrigger);
//SVAR(RouteProtocolRx);
#endif
SVAR(PPPLCPEcho);
SVAR(PPPLCPEchoRetry);
#if 0
//SVAR(PortMappingNumberOfEntries);
//SVAR(PortMapping);
//SVAR(Stats);
#endif

/* */
/* InternetGatewayDevice.WANDevice.WANConnectionDevice.WANPPPConnection. */


/* */
/* InternetGatewayDevice.WANDevice.WANConnectionDevice. */

SVAR(WANIPConnectionNumberOfEntries);
SVAR(WANPPPConnectionNumberOfEntries);
SVAR(WANDSLLinkConfig);
SVAR(WANATMF5LoopbackDiagnostics);
SVAR(WANEthernetLinkConfig);
SVAR(WANPOTSLinkConfig);
SVAR(WANIPConnection);
SVAR(WANPPPConnection);

SVAR(WANConnectionNumberOfEntries);

#endif   /* WAN_PARAMS_H */

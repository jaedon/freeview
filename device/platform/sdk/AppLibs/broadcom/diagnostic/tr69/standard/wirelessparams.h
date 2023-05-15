/*****************************************************************************
//
//  Copyright (c) 2004  Broadcom Corporation
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
//  Filename:       wirelessparams.h
//
******************************************************************************
//  Description:
//             Wireless supported object varable names
//
//
*****************************************************************************/

#ifndef WIRELESS_PARAMS_H
#define WIRELESS_PARAMS_H

#ifdef WIRELESS

#include "../inc/tr69cdefs.h"

/* */
/* InternetGatewayDevice.LANDevice.WLANConfiguration.AssociatedDevice. */

SVAR(AssociatedDeviceMACAddress);
SVAR(AssociatedDeviceIPAddress);
SVAR(AssociatedDeviceAuthenticationState);
SVAR(LastRequestedUnicastCipher);
SVAR(LastRequestedMulticastCipher);
SVAR(LastPMKId);

/* */
/* InternetGatewayDevice.LANDevice.WLANConfiguration.AssociatedDevice. */


/* */
/* InternetGatewayDevice.LANDevice.WLANConfiguration.WEPKey. */

SVAR(WEPKey);

/* */
/* InternetGatewayDevice.LANDevice.WLANConfiguration.WEPKey. */


/* */
/* InternetGatewayDevice.LANDevice.WLANConfiguration.PreSharedKey. */

SVAR(PreSharedKey);
SVAR(KeyPassphrase);
//SVAR(AssociatedDeviceMACAddress);

/* */
/* InternetGatewayDevice.LANDevice.WLANConfiguration.PreSharedKey. */


/* */
/* InternetGatewayDevice.LANDevice.WLANConfiguration. */

//SVAR(Enable);
//SVAR(Status);
SVAR(BSSID);
//SVAR(MaxBitRate);
SVAR(Channel);
SVAR(SSID);
SVAR(BeaconType);
//SVAR(MACAddressControlEnabled);
//SVAR(Standard);
SVAR(WEPKeyIndex);
//SVAR(KeyPassphrase);
SVAR(WEPEncryptionLevel);
SVAR(BasicEncryptionModes);
SVAR(BasicAuthenticationMode);
SVAR(WPAEncryptionModes);
SVAR(WPAAuthenticationMode);
SVAR(IEEE11iEncryptionModes);
SVAR(IEEE11iAuthenticationMode);
SVAR(PossibleChannels);
SVAR(BasicDataTransmitRates);
SVAR(OperationalDataTransmitRates);
SVAR(PossibleDataTransmitRates);
SVAR(InsecureOOBAccessEnabled);
SVAR(BeaconAdvertisementEnabled);
SVAR(RadioEnabled);
SVAR(AutoRateFallBackEnabled);
SVAR(LocationDescription);
SVAR(RegulatoryDomain);
SVAR(TotalPSKFailures);
SVAR(TotalIntegrityFailures);
SVAR(ChannelsInUse);
SVAR(DeviceOperationMode);
SVAR(DistanceFromRoot);
SVAR(PeerBSSID);
SVAR(AuthenticationServiceMode);
SVAR(TotalAssociations);
SVAR(AssociatedDevice);
//SVAR(WEPKey);
//SVAR(PreSharedKey);

/* InternetGatewayDevice.LANDevice.WLANConfiguration. */
SVAR(WLANConfiguration);

/* InternetGatewayDevice.LANDevice.LANWLANConfigurationNumberOfEntries */
SVAR(LANWLANConfigurationNumberOfEntries);

#endif // #ifdef WIRELESS

#endif   // WIRELESS_PARAMS_H

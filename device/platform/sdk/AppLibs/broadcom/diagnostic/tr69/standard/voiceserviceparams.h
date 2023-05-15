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
//  Filename:       voiceserviceparams.h
//  Author:         Farhan Ali
//  Creation Date:  07/29/05
//
******************************************************************************
//  Description:
//             WT-104 supported object varable names
//
//
*****************************************************************************/

#ifndef VOICE_SERVICE_PARAMS_H
#define VOICE_SERVICE_PARAMS_H

#include "../inc/tr69cdefs.h"

SVAR(VoiceService);
SVAR(VoiceServiceNumberOfEntries);

/* VoiceService.{i}. */
SVAR(VoiceProfileNumberOfEntries);
SVAR(Capabilities);
SVAR(VoiceProfile);

/* VoiceService.{i}.Capabilities. */
SVAR(MaxProfileCount);
SVAR(MaxSessionCount);
SVAR(SignalingProtocols);
SVAR(FaxT38);
SVAR(FaxPassThrough);
SVAR(ModemPassThrough);
SVAR(ToneGeneration);
SVAR(RingGeneration);
SVAR(VoiceLineTests);
SVAR(SIP);
SVAR(Codecs);


/* VoiceService.{i}.Capabilities.SIP */
SVAR(Role);
SVAR(Extensions);
SVAR(Transports);
SVAR(URISchemes);
SVAR(EventSubscription);
SVAR(ResponseMap);
SVAR(TLSAuthenticationProtocols);
SVAR(TLSAuthenticationKeySize);
SVAR(TLSEncryptionProtocols);
SVAR(TLSEncryptionKeySize);
SVAR(TLSKeyExchangProtocols);


/* VoiceService.{i}.Capabilities.Codecs.{i}. */
SVAR(EntryId);
//SVAR(Codec);
//SVAR(BitRate);
SVAR(PacketizationPeriod);
SVAR(SilenceSuppression);

/* VoiceService.{i}.VoiceProfile.{i}. */
//SVAR(Enable);
SVAR(Reset);
SVAR(NumberOfLines);
//SVAR(Name);
SVAR(SignalingProtocol);
SVAR(MaxSessions);
SVAR(DTMFMethod);
// SVAR(SIPOutBoundProxy);
SVAR(RTP);
SVAR(Line);
SVAR(ServiceProviderInfo);
//SVAR(SIP);

 /* VoiceService.{i}.VoiceProfile.{i}.ServiceProviderInfo. */
//SVAR(Name);

 /* VoiceService.{i}.VoiceProfile.{i}.SIP */
SVAR(ProxyServer);
SVAR(ProxyServerPort);
SVAR(ProxyServerTransport);
SVAR(RegistrarServer);
SVAR(RegistrarServerPort);
SVAR(RegistrarServerTransport);
SVAR(UserAgentDomain);
SVAR(UserAgentPort);
SVAR(UserAgentTransport);
SVAR(OutboundProxy);  
SVAR(OutboundProxyPort);  
SVAR(Organization);
SVAR(RegistrationPeriod);
SVAR(RegisterExpires);
SVAR(DSCPMark);
SVAR(VLANIDMark);
SVAR(EthernetPriorityMark);
SVAR(SIPEventSubscribeNumberOfElements);
SVAR(SipResponseMapNumberOfElements);
SVAR(EventSubscribe);

 /* VoiceService.{i}.VoiceProfile.{i}.SIP.EventSubscribe.{i}. */
SVAR(Event);
SVAR(Notifier);
SVAR(NotifierPort);
SVAR(NotifierTransport);

// /* VoiceService.{i}.VoiceProfile.{i}.SIPOutboundProxy. */-------------------- Removed in latest Spec
//SVAR(OutboundProxy); -------------------- Removed in latest Spec

 /* VoiceService.{i}.VoiceProfile.{i}.RTP. */
SVAR(LocalPortMin);
SVAR(LocalPortMax);
//SVAR(DSCPMark);
SVAR(TelephoneEventPayloadType);

 /* VoiceService.{i}.VoiceProfile.{i}.Line.{i}. */
//SVAR(Enable);
SVAR(DirectoryNumber);
//SVAR(Status);
SVAR(CallState);
//SVAR(SIP);
// SVAR(Stats);
SVAR(Codec);
SVAR(PhyInterface);

 /* VoiceService.{i}.VoiceProfile.{i}.Line.{i}.SIP */
SVAR(AuthUserName);
SVAR(AuthPassword);
SVAR(URI);

 /* VoiceService.{i}.VoiceProfile.{i}.Line.{i}.Stats */
SVAR(ResetStatistics);
// SVAR(PacketsSent);
// SVAR(PacketsReceived);
// SVAR(BytesSent);
// SVAR(BytesReceived);
SVAR(PacketsLost);
//SVAR(Overruns);
//SVAR(Underruns);
SVAR(IncomingCallsReceived);
SVAR(IncomingCallsAnswered);
SVAR(IncomingCallsConnected);
SVAR(IncomingCallsFailed);
SVAR(OutgoingCallsAttempted);
SVAR(OutgoingCallsAnswered);
SVAR(OutgoingCallsConnected);
SVAR(OutgoingCallsFailed);
SVAR(ReccvIAJitter);
SVAR(RTDelay);
//SVAR(CallsDropped);
//SVAR(TotalCallTime);
//SVAR(ServerDownTime);

 /* VoiceService.{i}.VoiceProfile.{i}.Line.{i}.Codec.*/
SVAR(List);

 /* VoiceService.{i}.VoiceProfile.{i}.Line.{i}.Codec.List.{i}. */
SVAR(EntryID);
//SVAR(Codec);
SVAR(BitRate);
//SVAR(PacketizationPeriod);
//SVAR(SilenceSuppression);
//SVAR(Enable);
SVAR(Priority);

 /* VoiceService.{i}.VoiceProfile.{i}.Line.{i}.PhyInterface.{i}. */
SVAR(Tests);

 /* VoiceService.{i}.VoiceProfile.{i}.Line.{i}.PhyInterface.{i}.Tests. */
SVAR(TestState);
SVAR(TestSelector);
SVAR(PhoneConnectivity);

#endif   // VOICE_SERVICE_PARAMS_H

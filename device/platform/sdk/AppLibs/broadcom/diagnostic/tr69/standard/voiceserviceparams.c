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
//  Filename:       voiceserviceparams.c
//  Author:         Farhan Ali
//  Creation Date:  07/29/05
//
******************************************************************************
//  Description:
//             WT-104 supported objects
//
//
*****************************************************************************/

/*
* Declare instances of CPE strings
*/
#include "sharedparams.h"
#include "voiceserviceparams.h" /* profiles for parameter callbacks */

/*---------------VoiceService end---------------------*/

/*---------------VoiceService.Capabilities end---------------------*/

TRXGFUNC(getSipRole);
TRXGFUNC(getSipExt);
TRXGFUNC(getSipTransport);
TRXGFUNC(getSipURISch);
TRXGFUNC(getSipEvSubs);
TRXGFUNC(getSipRespMap);
TRXGFUNC(getSipTLSAuthProt);
TRXGFUNC(getSipTLSAuthKey);
TRXGFUNC(getSipTLSEncProt);
TRXGFUNC(getSipTLSEncKeySz);
TRXGFUNC(getSipTLSKeyExchProt);
TRxObjNode sipDesc[] = {
    {Role,{{tUnsigned,0}},NULL,getSipRole,NULL},
    {Extensions,{{tUnsigned,0}},NULL,getSipExt,NULL},
    {Transports,{{tString,256,0}},NULL,getSipTransport,NULL},
    {URISchemes,{{tBool,0}},NULL,getSipURISch,NULL},
    {EventSubscription,{{tBool,0}},NULL,getSipEvSubs,NULL},
    {ResponseMap,{{tBool,0}},NULL,getSipRespMap,NULL},
    {TLSAuthenticationProtocols,{{tBool,0}},NULL,getSipTLSAuthProt,NULL},
    {TLSAuthenticationKeySize,{{tBool,0}},NULL,getSipTLSAuthKey,NULL},            
    {TLSEncryptionProtocols,{{tBool,0}},NULL,getSipTLSEncProt,NULL},
    {TLSEncryptionKeySize,{{tBool,0}},NULL,getSipTLSEncKeySz,NULL},            
    {TLSKeyExchangProtocols,{{tBool,0}},NULL,getSipTLSKeyExchProt,NULL},
    {NULL}
};   

TRXGFUNC(getCodecEntryId);
TRXGFUNC(getCodec);
TRXGFUNC(getCodecBitRate);
TRXGFUNC(getCodecPtime);
TRXGFUNC(getCodecSilSup);
TRxObjNode codecsInstanceDesc[] = {
    {EntryID,{{tUnsigned,0}},NULL,getCodecEntryId,NULL},
    {Codec,{{tUnsigned,0}},NULL,getCodec,NULL},
    {BitRate,{{tString,256,0}},NULL,getCodecBitRate,NULL},
    {PacketizationPeriod,{{tString,64,0}},NULL,getCodecPtime,NULL},
    {SilenceSuppression,{{tBool,0}},NULL,getCodecSilSup,NULL},
    {NULL}
};   

TRxObjNode codecsDesc[] = {
    {instanceIDMASK,{{0,0}},NULL,NULL, codecsInstanceDesc},
};

TRXGFUNC(getMaxProfCnt);
TRXGFUNC(getMaxSesCnt);
TRXGFUNC(getSigProtocols);
TRXGFUNC(getFaxT38sup);
TRXGFUNC(getFaxPassThrSup);
TRXGFUNC(getMdmPassThrSup);
TRXGFUNC(getToneGenSup);
TRXGFUNC(getRingGenSup);
TRXGFUNC(getVoiceLineTest);
TRxObjNode capabilitiesDesc[] = {
    {MaxProfileCount,{{tUnsigned,0}},NULL,getMaxProfCnt,NULL},
    {MaxSessionCount,{{tUnsigned,0}},NULL,getMaxSesCnt,NULL},
    {SignalingProtocols,{{tString,256,0}},NULL,getSigProtocols,NULL},
    {FaxT38,{{tBool,0}},NULL,getFaxT38sup,NULL},
    {FaxPassThrough,{{tBool,0}},NULL,getFaxPassThrSup,NULL},
    {ModemPassThrough,{{tBool,0}},NULL,getMdmPassThrSup,NULL},
    {ToneGeneration,{{tBool,0}},NULL,getToneGenSup,NULL},
    {RingGeneration,{{tBool,0}},NULL,getRingGenSup,NULL},            
    {VoiceLineTests,{{tBool,0}},NULL,getVoiceLineTest,NULL},  
    {SIP,{{tObject,0}},NULL,NULL,sipDesc},
    {Codecs,{{tInstance,0}},NULL,NULL,codecsDesc},
    {NULL}
}; 

/*---------------VoiceService.Capabilities start---------------------*/
 
/*---------------VoiceService.VoiceProfile end---------------------*/

/*---------------VoiceService.VoiceProfile.Sip end---------------------*/

TRXGFUNC(getSipEvSubscEvent);
TRXGFUNC(getSipEvSubsNotif);
TRXGFUNC(getSipEvSubsNotifPort);
TRXGFUNC(getSipEvSubsNotifTrans);
TRxObjNode vpSipEvSubscInstanceDesc[] = {
    {Event,{{tString,256,0}},NULL,getSipEvSubscEvent,NULL},            
    {Notifier,{{tString,256,0}},NULL,getSipEvSubsNotif,NULL},         
    {NotifierPort,{{tString,256,0}},NULL,getSipEvSubsNotifPort,NULL},    
    {NotifierTransport,{{tString,256,0}},NULL,getSipEvSubsNotifTrans,NULL},   
    {NULL}
};    

TRxObjNode vpSipEvSubscDesc[] = {
    {instanceIDMASK,{{0,0}},NULL,NULL,vpSipEvSubscInstanceDesc},
};

TRXGFUNC(getProxServer);
TRXGFUNC(getProxServerPort);
TRXSFUNC(setProxServer);
TRXSFUNC(setProxServerPort);
TRXGFUNC(getProxServerTransport);
TRXGFUNC(getRegServer);
TRXGFUNC(getRegServerPort);
TRXSFUNC(setRegServer);
TRXSFUNC(setRegServerPort);
TRXGFUNC(getRegServerTransport);
TRXGFUNC(getUADomain);
TRXSFUNC(setUADomain);
TRXGFUNC(getUAPort);
TRXSFUNC(setUAPort);
TRXGFUNC(getUATransport);
TRXGFUNC(getObProxy);
TRXGFUNC(getObProxyPort);
TRXSFUNC(setObProxy);
TRXSFUNC(setObProxyPort);
TRXGFUNC(getOrganization);
TRXGFUNC(getRegPeriod);
TRXSFUNC(setRegPeriod);
TRXGFUNC(getRegExp);
TRXGFUNC(getDSCPMark);
TRXGFUNC(getVLANIDMark);
TRXGFUNC(getEthPrioMark);
TRXGFUNC(getSipSubscEvNOfE);
TRXGFUNC(getSipRespMapNOfE);
TRxObjNode vpSipDesc[] = {
    {ProxyServer,{{tString,256}},setProxServer,getProxServer,NULL,NULL},
    {ProxyServerPort,{{tUnsigned,0}},setProxServerPort,getProxServerPort,NULL},
    {ProxyServerTransport,{{tString,0}},NULL,getProxServerTransport,NULL},
    {RegistrarServer,{{tString,256}},setRegServer,getRegServer,NULL},
    {RegistrarServerPort,{{tUnsigned}},setRegServerPort,getRegServerPort,NULL},
    {RegistrarServerTransport,{{tString,0}},NULL,getRegServerTransport,NULL},
    {UserAgentDomain,{{tString,256}},setUADomain,getUADomain,NULL},
    {UserAgentPort,{{tUnsigned}},setUAPort,getUAPort,NULL},
    {UserAgentTransport,{{tString,0}},NULL,getUATransport,NULL},
    {OutboundProxy,{{tString,256}},setObProxy,getObProxy,NULL},
    {OutboundProxyPort,{{tUnsigned}},setObProxyPort,getObProxyPort,NULL},
    {Organization,{{tString,256,0}},NULL,getOrganization,NULL},
    {RegistrationPeriod,{{tUnsigned}},setRegPeriod,getRegPeriod,NULL},
    {RegisterExpires,{{tUnsigned,0}},NULL,getRegExp,NULL},
    {DSCPMark,{{tUnsigned,0}},NULL,getDSCPMark,NULL},
    {VLANIDMark,{{tInt,0}},NULL,getVLANIDMark,NULL},
    {EthernetPriorityMark,{{tInt,0}},NULL,getEthPrioMark,NULL},
    {SIPEventSubscribeNumberOfElements,{{tUnsigned,0}},NULL,getSipSubscEvNOfE,NULL},
    {SipResponseMapNumberOfElements,{{tUnsigned,0}},NULL,getSipRespMapNOfE,NULL},
    {EventSubscribe,{{tInstance,0}},NULL,NULL,vpSipEvSubscDesc},
    {NULL}
};  
/*---------------VoiceService.VoiceProfile.Sip start---------------------*/
   
/*---------------VoiceService.VoiceProfile.RTP end---------------------*/
   

TRXGFUNC(getRTPLocPortMin);
TRXGFUNC(getRTPLocPortMax);
TRXGFUNC(getRTPDSCPMark);
TRXGFUNC(getRTPTelEvpayloadType);
TRxObjNode vpRTPDesc[] = {
   {LocalPortMin,{{tUnsigned,0}},NULL,getRTPLocPortMin,NULL},
   {LocalPortMax,{{tUnsigned,0}},NULL,getRTPLocPortMax,NULL},
   {DSCPMark,{{tUnsigned,0}},NULL,getRTPDSCPMark,NULL},
   {TelephoneEventPayloadType,{{tUnsigned,0}},NULL,getRTPTelEvpayloadType,NULL},
    {NULL}
}; 

/*---------------VoiceService.VoiceProfile.RTP start---------------------*/

/*---------------VoiceService.VoiceProfile.PhyInterface end---------------------*/

TRXGFUNC(getTestState);
TRXGFUNC(getTestSelector);
TRXGFUNC(getPhnConn);
TRxObjNode vpPhyIntTestsDesc[] = {
   {TestState,{{tString,0}},NULL,getTestState,NULL},
   {TestSelector,{{tString,64,0}},NULL,getTestSelector,NULL},
   {PhoneConnectivity,{{tBool,0}},NULL,getPhnConn,NULL},   
    {NULL}
};

TRxObjNode vpPhyIntInstanceDesc[] = {
    {Tests,{{tObject,0}},NULL,NULL, vpPhyIntTestsDesc},
    {NULL}
};

TRxObjNode vpPhyIntDesc[] = {
    {instanceIDMASK,{{0,0}},NULL,NULL, vpPhyIntInstanceDesc},
};

/*---------------VoiceService.VoiceProfile.PhyInterface Start---------------------*/

/*---------------VoiceService.VoiceProfile.Line end---------------------*/

/*---------------VoiceService.VoiceProfile.Line.Codec End---------------------*/

TRXGFUNC(getVPLineCodecEntryId);
TRXGFUNC(getVPLineCodecName);
TRXGFUNC(getVPLineCodecRate);
TRXGFUNC(getVPLineCodecPtime);
TRXSFUNC(setVPLineCodecPtime);
TRXGFUNC(getVPLineCodecSilenceSup);
TRXSFUNC(setVPLineCodecSilenceSup);
TRXGFUNC(getVPLineCodecEnable);
TRXGFUNC(getVPLineCodecPriority);
TRXSFUNC(setVPLineCodecPriority);
TRxObjNode vpLineCodecListInstanceDesc[] = {
   {EntryID,{{tUnsigned,0}},NULL,getVPLineCodecEntryId,NULL},
   {Codec,{{tString,64,0}},NULL,getVPLineCodecName,NULL},
   {BitRate,{{tUnsigned,0}},NULL,getVPLineCodecRate,NULL},
   {PacketizationPeriod,{{tString,64}},setVPLineCodecPtime,getVPLineCodecPtime,NULL},
   {SilenceSuppression,{{tBool}},setVPLineCodecSilenceSup,getVPLineCodecSilenceSup,NULL},
   {Enable,{{tBool,0}},NULL,getVPLineCodecEnable,NULL},
   {Priority,{{tUnsigned}},setVPLineCodecPriority,getVPLineCodecPriority,NULL},  
    {NULL}
}; 

TRxObjNode vpLineCodecListDesc[] = {
    {instanceIDMASK,{{0,0}},NULL,NULL, vpLineCodecListInstanceDesc},
};

TRxObjNode vpLineCodecDesc[] = {
    {List,{{tInstance,0}},NULL,NULL, vpLineCodecListDesc},
    {NULL}

};


/*---------------VoiceService.VoiceProfile.Line.Codec Start---------------------*/

/*---------------VoiceService.VoiceProfile.Line.Sip end---------------------*/

TRXGFUNC(getLineSIPAuthUName);
TRXSFUNC(setLineSIPAuthUName);
TRXGFUNC(getLineSIPAuthPass);
TRXSFUNC(setLineSIPAuthPass);
TRXGFUNC(getLineSIPURI);
TRXSFUNC(setLineSIPURI);
TRxObjNode vpLineSipDesc[] = {
    {AuthUserName,{{tString,128}},setLineSIPAuthUName,getLineSIPAuthUName,NULL},
//    {AuthPassword,{{tString,128}},setLineSIPAuthUName,getLineSIPAuthUName,NULL},
    {AuthPassword,{{tString,128}},setLineSIPAuthPass,getLineSIPAuthPass,NULL},
    {URI,{{tString,389}},setLineSIPURI,getLineSIPURI,NULL},   
    {NULL}
}; 

/*---------------VoiceService.VoiceProfile.Line.Sip start---------------------*/

/*---------------VoiceService.VoiceProfile.Line.Stats end---------------------*/


TRXSFUNC(setStatResetStats);
TRXGFUNC(getStatPacketsSent);
TRXGFUNC(getStatPacketsRcvd);
TRXGFUNC(getStatBytesSent);
TRXGFUNC(getStatBytesRcvd);
TRXGFUNC(getStatPacketsLost);
//TRXGFUNC(getStatUnderruns);
//TRXGFUNC(getStatOverruns);
TRXGFUNC(getStatInCallsRcvd);
TRXGFUNC(getStatInCallsAns);
TRXGFUNC(getStatInCallsConn);
TRXGFUNC(getStatInCallsFail);
TRXGFUNC(getStatOutCallsAttempt);
TRXGFUNC(getStatOutCallsAns);
TRXGFUNC(getStatOutCallsConn);
TRXGFUNC(getStatOutCallsFail);
//TRXGFUNC(getStatRecvIAJitter);
//TRXGFUNC(getStatRTDelay);
TRxObjNode vpLineStatsDesc[] = {
   {ResetStatistics,{{tBool}},setStatResetStats,NULL,NULL},
   {PacketsSent,{{tUnsigned,0}},NULL,getStatPacketsSent,NULL},
   {PacketsReceived,{{tUnsigned,0}},NULL,getStatPacketsRcvd,NULL},
   {BytesSent,{{tUnsigned,0}},NULL,getStatBytesSent,NULL},
   {BytesReceived,{{tUnsigned,0}},NULL,getStatBytesRcvd,NULL},
   {PacketsLost,{{tUnsigned,0}},NULL,getStatPacketsLost,NULL},
   {IncomingCallsReceived,{{tUnsigned,0}},NULL,getStatInCallsRcvd,NULL},
   {IncomingCallsAnswered,{{tUnsigned,0}},NULL,getStatInCallsAns,NULL},
   {IncomingCallsConnected,{{tUnsigned,0}},NULL,getStatInCallsConn,NULL},
   {IncomingCallsFailed,{{tUnsigned,0}},NULL,getStatInCallsFail,NULL},
   {OutgoingCallsAttempted,{{tUnsigned,0}},NULL,getStatOutCallsAttempt,NULL},
   {OutgoingCallsAnswered,{{tUnsigned,0}},NULL,getStatOutCallsAns,NULL},
   {OutgoingCallsConnected,{{tUnsigned,0}},NULL,getStatOutCallsConn,NULL},
   {OutgoingCallsFailed,{{tUnsigned,0}},NULL,getStatOutCallsFail,NULL},
//   {RecvIAJitter,{{tUnsigned,0}},NULL,getStatRecvIAJitter,NULL},   
//   {RTDelay,{{tUnsigned,0}},NULL,getStatRTDelay,NULL},   
    {NULL}
}; 
   
/*---------------VoiceService.VoiceProfile.Line.Stats start---------------------*/


TRXGFUNC(getLineEnable);
TRXSFUNC(setLineEnable);
TRXGFUNC(getLineDirNum);
TRXSFUNC(setLineDirNum);
TRXGFUNC(getLineStatus);
TRXGFUNC(getLineCallState);
TRxObjNode vpLineInstanceDesc[] = {
   {Enable,{{tString}},setLineEnable,getLineEnable,NULL},
   {DirectoryNumber,{{tString,32}},setLineDirNum,getLineDirNum,NULL},
   {Status,{{tString,0}},NULL,getLineStatus,NULL},
   {CallState,{{tString,0}},NULL,getLineCallState,NULL},
   {SIP,{{tObject,0}},NULL,NULL,vpLineSipDesc},
   {Stats,{{tObject,0}},NULL,NULL,vpLineStatsDesc},
//    {Tests,{{tObject,0}},NULL,NULL,vpLineTestsDesc}, ----- Removed in latest Spec
    {Codec,{{tObject,0}},NULL,NULL,vpLineCodecDesc},
  
    {NULL}
}; 

TRxObjNode vpLineDesc[] = {
    {instanceIDMASK,{{0,0}},NULL,NULL, vpLineInstanceDesc},
};

/*---------------VoiceService.VoiceProfile.Line start---------------------*/

TRXGFUNC(getVPServprovName);
TRxObjNode vpServprovInfoDesc[] = {
    {Name,{{tString,256,0}},NULL,getVPServprovName,NULL},
    {NULL}
};  

TRXGFUNC(getVPEnable);
TRXGFUNC(getVPReset);
TRXSFUNC(setVPReset);
TRXGFUNC(getVPNOfLines);
TRXGFUNC(getVPName);
TRXGFUNC(getVPSigPro);
TRXGFUNC(getVPMaxSession);
TRXGFUNC(getVPDTMFMethod);
TRxObjNode voiceProfInstanceDesc[] = { 
    {Enable,{{tString,0}},NULL,getVPEnable,NULL},
    {Reset,{{tBool}},setVPReset,getVPReset,NULL},
    {NumberOfLines,{{tUnsigned,0}},NULL,getVPNOfLines,NULL},
    {Name,{{tString,64,0}},NULL,getVPName,NULL},
    {SignalingProtocol,{{tString,64,0}},NULL,getVPSigPro,NULL},
    {MaxSessions,{{tUnsigned,0}},NULL,getVPMaxSession,NULL},
    {DTMFMethod,{{tString,64,0}},NULL,getVPDTMFMethod,NULL},
    {RTP,{{tObject,0}},NULL,NULL,vpRTPDesc},
//     {Codecs,{{tInstance,0}},NULL,NULL,vpCodecsDesc},   ----- Removed in latest Spec
    {Line,{{tInstance,0}},NULL,NULL,vpLineDesc},
    {ServiceProviderInfo,{{tObject,0}},NULL,NULL,vpServprovInfoDesc},
    {SIP,{{tObject,0}},NULL,NULL,vpSipDesc},
    {PhyInterface,{{tInstance,0}},NULL,NULL,vpPhyIntDesc},
    {NULL}
}; 

TRxObjNode voiceProfDesc[] = {
    {instanceIDMASK,{{0,0}},NULL,NULL,voiceProfInstanceDesc},
}; 

/*---------------VoiceService.VoiceProfile start---------------------*/


TRXGFUNC(getVoiceProfNOfE);
TRxObjNode voiceServiceInstanceDesc[] = {
    {VoiceProfileNumberOfEntries,{{tUnsigned,0}},NULL,getVoiceProfNOfE,NULL},
    {Capabilities,{{tObject,0}},NULL,NULL,capabilitiesDesc},
    {VoiceProfile,{{tInstance,0}},NULL,NULL,voiceProfDesc},
    {NULL}
}; 

int testStub;

TRxObjNode voiceServiceDesc[] = {
    {instanceIDMASK,{{0,0}},NULL,NULL, voiceServiceInstanceDesc},
};

/*---------------VoiceService start---------------------*/

TRXGFUNC(getVoiceServiceNumberOfEntries);
/* InternetGatewayDevice.Services*/
TRxObjNode  servicesDesc[] = {
    {VoiceServiceNumberOfEntries,{{tUnsigned,0,0}}, NULL, getVoiceServiceNumberOfEntries, NULL},
    {VoiceService,{{tInstance,0,0}}, NULL, NULL, voiceServiceDesc},
    {NULL}
};


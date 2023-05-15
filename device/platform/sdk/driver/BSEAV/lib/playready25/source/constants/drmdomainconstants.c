/**@@@+++@@@@******************************************************************
**
** Microsoft (r) PlayReady (r)
** Copyright (c) Microsoft Corporation. All rights reserved.
**
***@@@---@@@@******************************************************************
*/

#include <drmtypes.h>
#include <drmdomainconstants.h>

ENTER_PK_NAMESPACE_CODE;

/*
** -------------------------------------------------------------
** XML strings used in the construction of domain join challenge
** -------------------------------------------------------------
*/
/* "JoinDomain" */
DRM_STR_CONST DRM_CHAR g_rgchDomJoinRootTag [] = { INIT_CHAR_OBFUS('J'), INIT_CHAR_OBFUS('o'), INIT_CHAR_OBFUS('i'), INIT_CHAR_OBFUS('n'), INIT_CHAR_OBFUS('D'), INIT_CHAR_OBFUS('o'), INIT_CHAR_OBFUS('m'), INIT_CHAR_OBFUS('a'), INIT_CHAR_OBFUS('i'), INIT_CHAR_OBFUS('n'), INIT_CHAR_OBFUS('\0')};
const DRM_DISCARDABLE DRM_ANSI_CONST_STRING g_dastrDomJoinRootTag = CREATE_DRM_ANSI_STRING_EX( g_rgchDomJoinRootTag, 10 );


/* "Version" */
DRM_STR_CONST DRM_CHAR g_rgchDomJoinVersionTag [] = { INIT_CHAR_OBFUS('V'), INIT_CHAR_OBFUS('e'), INIT_CHAR_OBFUS('r'), INIT_CHAR_OBFUS('s'), INIT_CHAR_OBFUS('i'), INIT_CHAR_OBFUS('o'), INIT_CHAR_OBFUS('n'), INIT_CHAR_OBFUS('\0')};
const DRM_DISCARDABLE DRM_ANSI_CONST_STRING g_dastrDomJoinVersionTag = CREATE_DRM_ANSI_STRING_EX( g_rgchDomJoinVersionTag, 7 );


/* "challenge" */
DRM_STR_CONST DRM_CHAR g_rgchDomJoinChallenge1Tag [] = { INIT_CHAR_OBFUS('c'), INIT_CHAR_OBFUS('h'), INIT_CHAR_OBFUS('a'), INIT_CHAR_OBFUS('l'), INIT_CHAR_OBFUS('l'), INIT_CHAR_OBFUS('e'), INIT_CHAR_OBFUS('n'), INIT_CHAR_OBFUS('g'), INIT_CHAR_OBFUS('e'), INIT_CHAR_OBFUS('\0')};
const DRM_DISCARDABLE DRM_ANSI_CONST_STRING g_dastrDomJoinChallenge1Tag = CREATE_DRM_ANSI_STRING_EX( g_rgchDomJoinChallenge1Tag, 9 );


/* "Challenge" */
DRM_STR_CONST DRM_CHAR g_rgchDomJoinChallenge2Tag [] = { INIT_CHAR_OBFUS('C'), INIT_CHAR_OBFUS('h'), INIT_CHAR_OBFUS('a'), INIT_CHAR_OBFUS('l'), INIT_CHAR_OBFUS('l'), INIT_CHAR_OBFUS('e'), INIT_CHAR_OBFUS('n'), INIT_CHAR_OBFUS('g'), INIT_CHAR_OBFUS('e'), INIT_CHAR_OBFUS('\0')};
const DRM_DISCARDABLE DRM_ANSI_CONST_STRING g_dastrDomJoinChallenge2Tag = CREATE_DRM_ANSI_STRING_EX( g_rgchDomJoinChallenge2Tag, 9 );


/* "Domain" */
DRM_STR_CONST DRM_CHAR g_rgchDomJoinDomainTag [] = { INIT_CHAR_OBFUS('D'), INIT_CHAR_OBFUS('o'), INIT_CHAR_OBFUS('m'), INIT_CHAR_OBFUS('a'), INIT_CHAR_OBFUS('i'), INIT_CHAR_OBFUS('n'), INIT_CHAR_OBFUS('\0')};
const DRM_DISCARDABLE DRM_ANSI_CONST_STRING g_dastrDomJoinDomainTag = CREATE_DRM_ANSI_STRING_EX( g_rgchDomJoinDomainTag, 6 );


/* "Data" */
DRM_STR_CONST DRM_CHAR g_rgchDomJoinDataTag [] = { INIT_CHAR_OBFUS('D'), INIT_CHAR_OBFUS('a'), INIT_CHAR_OBFUS('t'), INIT_CHAR_OBFUS('a'), INIT_CHAR_OBFUS('\0')};
const DRM_DISCARDABLE DRM_ANSI_CONST_STRING g_dastrDomJoinDataTag = CREATE_DRM_ANSI_STRING_EX( g_rgchDomJoinDataTag, 4 );


/* "ServiceID" */
DRM_STR_CONST DRM_CHAR g_rgchDomJoinServiceIDTag [] = { INIT_CHAR_OBFUS('S'), INIT_CHAR_OBFUS('e'), INIT_CHAR_OBFUS('r'), INIT_CHAR_OBFUS('v'), INIT_CHAR_OBFUS('i'), INIT_CHAR_OBFUS('c'), INIT_CHAR_OBFUS('e'), INIT_CHAR_OBFUS('I'), INIT_CHAR_OBFUS('D'), INIT_CHAR_OBFUS('\0')};
const DRM_DISCARDABLE DRM_ANSI_CONST_STRING g_dastrDomJoinServiceIDTag = CREATE_DRM_ANSI_STRING_EX( g_rgchDomJoinServiceIDTag, 9 );


/* "AccountID" */
DRM_STR_CONST DRM_CHAR g_rgchDomJoinAccountIDTag [] = { INIT_CHAR_OBFUS('A'), INIT_CHAR_OBFUS('c'), INIT_CHAR_OBFUS('c'), INIT_CHAR_OBFUS('o'), INIT_CHAR_OBFUS('u'), INIT_CHAR_OBFUS('n'), INIT_CHAR_OBFUS('t'), INIT_CHAR_OBFUS('I'), INIT_CHAR_OBFUS('D'), INIT_CHAR_OBFUS('\0')};
const DRM_DISCARDABLE DRM_ANSI_CONST_STRING g_dastrDomJoinAccountIDTag = CREATE_DRM_ANSI_STRING_EX( g_rgchDomJoinAccountIDTag, 9 );


/* "Revision" */
DRM_STR_CONST DRM_CHAR g_rgchDomJoinRevisionTag [] = { INIT_CHAR_OBFUS('R'), INIT_CHAR_OBFUS('e'), INIT_CHAR_OBFUS('v'), INIT_CHAR_OBFUS('i'), INIT_CHAR_OBFUS('s'), INIT_CHAR_OBFUS('i'), INIT_CHAR_OBFUS('o'), INIT_CHAR_OBFUS('n'), INIT_CHAR_OBFUS('\0')};
const DRM_DISCARDABLE DRM_ANSI_CONST_STRING g_dastrDomJoinRevisionTag = CREATE_DRM_ANSI_STRING_EX( g_rgchDomJoinRevisionTag, 8 );


/* "FriendlyName" */
DRM_STR_CONST DRM_CHAR g_rgchDomJoinFriendlyNameTag [] = { INIT_CHAR_OBFUS('F'), INIT_CHAR_OBFUS('r'), INIT_CHAR_OBFUS('i'), INIT_CHAR_OBFUS('e'), INIT_CHAR_OBFUS('n'), INIT_CHAR_OBFUS('d'), INIT_CHAR_OBFUS('l'), INIT_CHAR_OBFUS('y'), INIT_CHAR_OBFUS('N'), INIT_CHAR_OBFUS('a'), INIT_CHAR_OBFUS('m'), INIT_CHAR_OBFUS('e'), INIT_CHAR_OBFUS('\0')};
const DRM_DISCARDABLE DRM_ANSI_CONST_STRING g_dastrDomJoinFriendlyNameTag = CREATE_DRM_ANSI_STRING_EX( g_rgchDomJoinFriendlyNameTag, 12 );


/* "CustomData" */
DRM_STR_CONST DRM_CHAR g_rgchDomJoinCustomDataTag [] = { INIT_CHAR_OBFUS('C'), INIT_CHAR_OBFUS('u'), INIT_CHAR_OBFUS('s'), INIT_CHAR_OBFUS('t'), INIT_CHAR_OBFUS('o'), INIT_CHAR_OBFUS('m'), INIT_CHAR_OBFUS('D'), INIT_CHAR_OBFUS('a'), INIT_CHAR_OBFUS('t'), INIT_CHAR_OBFUS('a'), INIT_CHAR_OBFUS('\0')};
const DRM_DISCARDABLE DRM_ANSI_CONST_STRING g_dastrDomJoinCustomDataTag = CREATE_DRM_ANSI_STRING_EX( g_rgchDomJoinCustomDataTag, 10 );


/* "CertificateChain" */
DRM_STR_CONST DRM_CHAR g_rgchDomJoinCertificateChainTag [] = { INIT_CHAR_OBFUS('C'), INIT_CHAR_OBFUS('e'), INIT_CHAR_OBFUS('r'), INIT_CHAR_OBFUS('t'), INIT_CHAR_OBFUS('i'), INIT_CHAR_OBFUS('f'), INIT_CHAR_OBFUS('i'), INIT_CHAR_OBFUS('c'), INIT_CHAR_OBFUS('a'), INIT_CHAR_OBFUS('t'), INIT_CHAR_OBFUS('e'), INIT_CHAR_OBFUS('C'), INIT_CHAR_OBFUS('h'), INIT_CHAR_OBFUS('a'), INIT_CHAR_OBFUS('i'), INIT_CHAR_OBFUS('n'), INIT_CHAR_OBFUS('\0')};
const DRM_DISCARDABLE DRM_ANSI_CONST_STRING g_dastrDomJoinCertificateChainTag = CREATE_DRM_ANSI_STRING_EX( g_rgchDomJoinCertificateChainTag, 16 );



/* "xmlns" */
DRM_STR_CONST DRM_CHAR g_rgchDomJoinRootAttribName [] = { INIT_CHAR_OBFUS('x'), INIT_CHAR_OBFUS('m'), INIT_CHAR_OBFUS('l'), INIT_CHAR_OBFUS('n'), INIT_CHAR_OBFUS('s'), INIT_CHAR_OBFUS('\0')};
const DRM_DISCARDABLE DRM_ANSI_CONST_STRING g_dastrDomJoinRootAttribName = CREATE_DRM_ANSI_STRING_EX( g_rgchDomJoinRootAttribName, 5 );

/* "http://schemas.microsoft.com/DRM/2007/03/protocols" */
DRM_STR_CONST DRM_CHAR g_rgchDomJoinRootAttribValue [] = { INIT_CHAR_OBFUS('h'), INIT_CHAR_OBFUS('t'), INIT_CHAR_OBFUS('t'), INIT_CHAR_OBFUS('p'), INIT_CHAR_OBFUS(':'), INIT_CHAR_OBFUS('/'), INIT_CHAR_OBFUS('/'), INIT_CHAR_OBFUS('s'), INIT_CHAR_OBFUS('c'), INIT_CHAR_OBFUS('h'), INIT_CHAR_OBFUS('e'), INIT_CHAR_OBFUS('m'), INIT_CHAR_OBFUS('a'), INIT_CHAR_OBFUS('s'), INIT_CHAR_OBFUS('.'), INIT_CHAR_OBFUS('m'), INIT_CHAR_OBFUS('i'), INIT_CHAR_OBFUS('c'), INIT_CHAR_OBFUS('r'), INIT_CHAR_OBFUS('o'), INIT_CHAR_OBFUS('s'), INIT_CHAR_OBFUS('o'), INIT_CHAR_OBFUS('f'), INIT_CHAR_OBFUS('t'), INIT_CHAR_OBFUS('.'), INIT_CHAR_OBFUS('c'), INIT_CHAR_OBFUS('o'), INIT_CHAR_OBFUS('m'), INIT_CHAR_OBFUS('/'), INIT_CHAR_OBFUS('D'), INIT_CHAR_OBFUS('R'), INIT_CHAR_OBFUS('M'), INIT_CHAR_OBFUS('/'), INIT_CHAR_OBFUS('2'), INIT_CHAR_OBFUS('0'), INIT_CHAR_OBFUS('0'), INIT_CHAR_OBFUS('7'), INIT_CHAR_OBFUS('/'), INIT_CHAR_OBFUS('0'), INIT_CHAR_OBFUS('3'), INIT_CHAR_OBFUS('/'), INIT_CHAR_OBFUS('p'), INIT_CHAR_OBFUS('r'), INIT_CHAR_OBFUS('o'), INIT_CHAR_OBFUS('t'), INIT_CHAR_OBFUS('o'), INIT_CHAR_OBFUS('c'), INIT_CHAR_OBFUS('o'), INIT_CHAR_OBFUS('l'), INIT_CHAR_OBFUS('s'), INIT_CHAR_OBFUS('\0')};
const DRM_DISCARDABLE DRM_ANSI_CONST_STRING g_dastrDomJoinRootAttribValue = CREATE_DRM_ANSI_STRING_EX( g_rgchDomJoinRootAttribValue, 50 );


/* "xmlns" */
DRM_STR_CONST DRM_CHAR g_rgchDomJoinChallenge2AttribName [] = { INIT_CHAR_OBFUS('x'), INIT_CHAR_OBFUS('m'), INIT_CHAR_OBFUS('l'), INIT_CHAR_OBFUS('n'), INIT_CHAR_OBFUS('s'), INIT_CHAR_OBFUS('\0')};
const DRM_DISCARDABLE DRM_ANSI_CONST_STRING g_dastrDomJoinChallenge2AttribName = CREATE_DRM_ANSI_STRING_EX( g_rgchDomJoinChallenge2AttribName, 5 );

/* "http://schemas.microsoft.com/DRM/2007/03/protocols/messages" */
DRM_STR_CONST DRM_CHAR g_rgchDomJoinChallenge2AttribValue [] = { INIT_CHAR_OBFUS('h'), INIT_CHAR_OBFUS('t'), INIT_CHAR_OBFUS('t'), INIT_CHAR_OBFUS('p'), INIT_CHAR_OBFUS(':'), INIT_CHAR_OBFUS('/'), INIT_CHAR_OBFUS('/'), INIT_CHAR_OBFUS('s'), INIT_CHAR_OBFUS('c'), INIT_CHAR_OBFUS('h'), INIT_CHAR_OBFUS('e'), INIT_CHAR_OBFUS('m'), INIT_CHAR_OBFUS('a'), INIT_CHAR_OBFUS('s'), INIT_CHAR_OBFUS('.'), INIT_CHAR_OBFUS('m'), INIT_CHAR_OBFUS('i'), INIT_CHAR_OBFUS('c'), INIT_CHAR_OBFUS('r'), INIT_CHAR_OBFUS('o'), INIT_CHAR_OBFUS('s'), INIT_CHAR_OBFUS('o'), INIT_CHAR_OBFUS('f'), INIT_CHAR_OBFUS('t'), INIT_CHAR_OBFUS('.'), INIT_CHAR_OBFUS('c'), INIT_CHAR_OBFUS('o'), INIT_CHAR_OBFUS('m'), INIT_CHAR_OBFUS('/'), INIT_CHAR_OBFUS('D'), INIT_CHAR_OBFUS('R'), INIT_CHAR_OBFUS('M'), INIT_CHAR_OBFUS('/'), INIT_CHAR_OBFUS('2'), INIT_CHAR_OBFUS('0'), INIT_CHAR_OBFUS('0'), INIT_CHAR_OBFUS('7'), INIT_CHAR_OBFUS('/'), INIT_CHAR_OBFUS('0'), INIT_CHAR_OBFUS('3'), INIT_CHAR_OBFUS('/'), INIT_CHAR_OBFUS('p'), INIT_CHAR_OBFUS('r'), INIT_CHAR_OBFUS('o'), INIT_CHAR_OBFUS('t'), INIT_CHAR_OBFUS('o'), INIT_CHAR_OBFUS('c'), INIT_CHAR_OBFUS('o'), INIT_CHAR_OBFUS('l'), INIT_CHAR_OBFUS('s'), INIT_CHAR_OBFUS('/'), INIT_CHAR_OBFUS('m'), INIT_CHAR_OBFUS('e'), INIT_CHAR_OBFUS('s'), INIT_CHAR_OBFUS('s'), INIT_CHAR_OBFUS('a'), INIT_CHAR_OBFUS('g'), INIT_CHAR_OBFUS('e'), INIT_CHAR_OBFUS('s'), INIT_CHAR_OBFUS('\0')};
const DRM_DISCARDABLE DRM_ANSI_CONST_STRING g_dastrDomJoinChallenge2AttribValue = CREATE_DRM_ANSI_STRING_EX( g_rgchDomJoinChallenge2AttribValue, 59 );


/* "xmlns" */
DRM_STR_CONST DRM_CHAR g_rgchDomJoinDomainAttrib1Name [] = { INIT_CHAR_OBFUS('x'), INIT_CHAR_OBFUS('m'), INIT_CHAR_OBFUS('l'), INIT_CHAR_OBFUS('n'), INIT_CHAR_OBFUS('s'), INIT_CHAR_OBFUS('\0')};
const DRM_DISCARDABLE DRM_ANSI_CONST_STRING g_dastrDomJoinDomainAttrib1Name = CREATE_DRM_ANSI_STRING_EX( g_rgchDomJoinDomainAttrib1Name, 5 );

/* "http://schemas.microsoft.com/DRM/2007/03/protocols" */
DRM_STR_CONST DRM_CHAR g_rgchDomJoinDomainAttrib1Value [] = { INIT_CHAR_OBFUS('h'), INIT_CHAR_OBFUS('t'), INIT_CHAR_OBFUS('t'), INIT_CHAR_OBFUS('p'), INIT_CHAR_OBFUS(':'), INIT_CHAR_OBFUS('/'), INIT_CHAR_OBFUS('/'), INIT_CHAR_OBFUS('s'), INIT_CHAR_OBFUS('c'), INIT_CHAR_OBFUS('h'), INIT_CHAR_OBFUS('e'), INIT_CHAR_OBFUS('m'), INIT_CHAR_OBFUS('a'), INIT_CHAR_OBFUS('s'), INIT_CHAR_OBFUS('.'), INIT_CHAR_OBFUS('m'), INIT_CHAR_OBFUS('i'), INIT_CHAR_OBFUS('c'), INIT_CHAR_OBFUS('r'), INIT_CHAR_OBFUS('o'), INIT_CHAR_OBFUS('s'), INIT_CHAR_OBFUS('o'), INIT_CHAR_OBFUS('f'), INIT_CHAR_OBFUS('t'), INIT_CHAR_OBFUS('.'), INIT_CHAR_OBFUS('c'), INIT_CHAR_OBFUS('o'), INIT_CHAR_OBFUS('m'), INIT_CHAR_OBFUS('/'), INIT_CHAR_OBFUS('D'), INIT_CHAR_OBFUS('R'), INIT_CHAR_OBFUS('M'), INIT_CHAR_OBFUS('/'), INIT_CHAR_OBFUS('2'), INIT_CHAR_OBFUS('0'), INIT_CHAR_OBFUS('0'), INIT_CHAR_OBFUS('7'), INIT_CHAR_OBFUS('/'), INIT_CHAR_OBFUS('0'), INIT_CHAR_OBFUS('3'), INIT_CHAR_OBFUS('/'), INIT_CHAR_OBFUS('p'), INIT_CHAR_OBFUS('r'), INIT_CHAR_OBFUS('o'), INIT_CHAR_OBFUS('t'), INIT_CHAR_OBFUS('o'), INIT_CHAR_OBFUS('c'), INIT_CHAR_OBFUS('o'), INIT_CHAR_OBFUS('l'), INIT_CHAR_OBFUS('s'), INIT_CHAR_OBFUS('\0')};
const DRM_DISCARDABLE DRM_ANSI_CONST_STRING g_dastrDomJoinDomainAttrib1Value = CREATE_DRM_ANSI_STRING_EX( g_rgchDomJoinDomainAttrib1Value, 50 );


/* "Id" */
DRM_STR_CONST DRM_CHAR g_rgchDomJoinDomainAttrib2Name [] = { INIT_CHAR_OBFUS('I'), INIT_CHAR_OBFUS('d'), INIT_CHAR_OBFUS('\0')};
const DRM_DISCARDABLE DRM_ANSI_CONST_STRING g_dastrDomJoinDomainAttrib2Name = CREATE_DRM_ANSI_STRING_EX( g_rgchDomJoinDomainAttrib2Name, 2 );

/* "SignedData" */
DRM_STR_CONST DRM_CHAR g_rgchDomJoinDomainAttrib2Value [] = { INIT_CHAR_OBFUS('S'), INIT_CHAR_OBFUS('i'), INIT_CHAR_OBFUS('g'), INIT_CHAR_OBFUS('n'), INIT_CHAR_OBFUS('e'), INIT_CHAR_OBFUS('d'), INIT_CHAR_OBFUS('D'), INIT_CHAR_OBFUS('a'), INIT_CHAR_OBFUS('t'), INIT_CHAR_OBFUS('a'), INIT_CHAR_OBFUS('\0')};
const DRM_DISCARDABLE DRM_ANSI_CONST_STRING g_dastrDomJoinDomainAttrib2Value = CREATE_DRM_ANSI_STRING_EX( g_rgchDomJoinDomainAttrib2Value, 10 );


/* "Type" */
DRM_STR_CONST DRM_CHAR g_rgchDomJoinCertificateChainAttribName [] = { INIT_CHAR_OBFUS('T'), INIT_CHAR_OBFUS('y'), INIT_CHAR_OBFUS('p'), INIT_CHAR_OBFUS('e'), INIT_CHAR_OBFUS('\0')};
const DRM_DISCARDABLE DRM_ANSI_CONST_STRING g_dastrDomJoinCertificateChainAttribName = CREATE_DRM_ANSI_STRING_EX( g_rgchDomJoinCertificateChainAttribName, 4 );

/* "device" */
DRM_STR_CONST DRM_CHAR g_rgchDomJoinCertificateChainAttribValue [] = { INIT_CHAR_OBFUS('d'), INIT_CHAR_OBFUS('e'), INIT_CHAR_OBFUS('v'), INIT_CHAR_OBFUS('i'), INIT_CHAR_OBFUS('c'), INIT_CHAR_OBFUS('e'), INIT_CHAR_OBFUS('\0')};
const DRM_DISCARDABLE DRM_ANSI_CONST_STRING g_dastrDomJoinCertificateChainAttribValue = CREATE_DRM_ANSI_STRING_EX( g_rgchDomJoinCertificateChainAttribValue, 6 );



/* "2" */
DRM_STR_CONST DRM_CHAR g_rgchDomainJoinVersion [] = { INIT_CHAR_OBFUS('2'), INIT_CHAR_OBFUS('\0')};
const DRM_DISCARDABLE DRM_ANSI_CONST_STRING g_dastrDomainJoinVersion = CREATE_DRM_ANSI_STRING_EX( g_rgchDomainJoinVersion, 1 );




/*
** --------------------------------------------------------------
** XML strings used in the construction of domain leave challenge
** --------------------------------------------------------------
*/
/* "LeaveDomain" */
DRM_STR_CONST DRM_CHAR g_rgchDomLeaveRootTag [] = { INIT_CHAR_OBFUS('L'), INIT_CHAR_OBFUS('e'), INIT_CHAR_OBFUS('a'), INIT_CHAR_OBFUS('v'), INIT_CHAR_OBFUS('e'), INIT_CHAR_OBFUS('D'), INIT_CHAR_OBFUS('o'), INIT_CHAR_OBFUS('m'), INIT_CHAR_OBFUS('a'), INIT_CHAR_OBFUS('i'), INIT_CHAR_OBFUS('n'), INIT_CHAR_OBFUS('\0')};
const DRM_DISCARDABLE DRM_ANSI_CONST_STRING g_dastrDomLeaveRootTag = CREATE_DRM_ANSI_STRING_EX( g_rgchDomLeaveRootTag, 11 );


/* "Version" */
DRM_STR_CONST DRM_CHAR g_rgchDomLeaveVersionTag [] = { INIT_CHAR_OBFUS('V'), INIT_CHAR_OBFUS('e'), INIT_CHAR_OBFUS('r'), INIT_CHAR_OBFUS('s'), INIT_CHAR_OBFUS('i'), INIT_CHAR_OBFUS('o'), INIT_CHAR_OBFUS('n'), INIT_CHAR_OBFUS('\0')};
const DRM_DISCARDABLE DRM_ANSI_CONST_STRING g_dastrDomLeaveVersionTag = CREATE_DRM_ANSI_STRING_EX( g_rgchDomLeaveVersionTag, 7 );


/* "challenge" */
DRM_STR_CONST DRM_CHAR g_rgchDomLeaveChallenge1Tag [] = { INIT_CHAR_OBFUS('c'), INIT_CHAR_OBFUS('h'), INIT_CHAR_OBFUS('a'), INIT_CHAR_OBFUS('l'), INIT_CHAR_OBFUS('l'), INIT_CHAR_OBFUS('e'), INIT_CHAR_OBFUS('n'), INIT_CHAR_OBFUS('g'), INIT_CHAR_OBFUS('e'), INIT_CHAR_OBFUS('\0')};
const DRM_DISCARDABLE DRM_ANSI_CONST_STRING g_dastrDomLeaveChallenge1Tag = CREATE_DRM_ANSI_STRING_EX( g_rgchDomLeaveChallenge1Tag, 9 );


/* "Challenge" */
DRM_STR_CONST DRM_CHAR g_rgchDomLeaveChallenge2Tag [] = { INIT_CHAR_OBFUS('C'), INIT_CHAR_OBFUS('h'), INIT_CHAR_OBFUS('a'), INIT_CHAR_OBFUS('l'), INIT_CHAR_OBFUS('l'), INIT_CHAR_OBFUS('e'), INIT_CHAR_OBFUS('n'), INIT_CHAR_OBFUS('g'), INIT_CHAR_OBFUS('e'), INIT_CHAR_OBFUS('\0')};
const DRM_DISCARDABLE DRM_ANSI_CONST_STRING g_dastrDomLeaveChallenge2Tag = CREATE_DRM_ANSI_STRING_EX( g_rgchDomLeaveChallenge2Tag, 9 );


/* "Domain" */
DRM_STR_CONST DRM_CHAR g_rgchDomLeaveDomainTag [] = { INIT_CHAR_OBFUS('D'), INIT_CHAR_OBFUS('o'), INIT_CHAR_OBFUS('m'), INIT_CHAR_OBFUS('a'), INIT_CHAR_OBFUS('i'), INIT_CHAR_OBFUS('n'), INIT_CHAR_OBFUS('\0')};
const DRM_DISCARDABLE DRM_ANSI_CONST_STRING g_dastrDomLeaveDomainTag = CREATE_DRM_ANSI_STRING_EX( g_rgchDomLeaveDomainTag, 6 );


/* "Data" */
DRM_STR_CONST DRM_CHAR g_rgchDomLeaveDataTag [] = { INIT_CHAR_OBFUS('D'), INIT_CHAR_OBFUS('a'), INIT_CHAR_OBFUS('t'), INIT_CHAR_OBFUS('a'), INIT_CHAR_OBFUS('\0')};
const DRM_DISCARDABLE DRM_ANSI_CONST_STRING g_dastrDomLeaveDataTag = CREATE_DRM_ANSI_STRING_EX( g_rgchDomLeaveDataTag, 4 );


/* "AccountID" */
DRM_STR_CONST DRM_CHAR g_rgchDomLeaveAccountIDTag [] = { INIT_CHAR_OBFUS('A'), INIT_CHAR_OBFUS('c'), INIT_CHAR_OBFUS('c'), INIT_CHAR_OBFUS('o'), INIT_CHAR_OBFUS('u'), INIT_CHAR_OBFUS('n'), INIT_CHAR_OBFUS('t'), INIT_CHAR_OBFUS('I'), INIT_CHAR_OBFUS('D'), INIT_CHAR_OBFUS('\0')};
const DRM_DISCARDABLE DRM_ANSI_CONST_STRING g_dastrDomLeaveAccountIDTag = CREATE_DRM_ANSI_STRING_EX( g_rgchDomLeaveAccountIDTag, 9 );


/* "CustomData" */
DRM_STR_CONST DRM_CHAR g_rgchDomLeaveCustomDataTag [] = { INIT_CHAR_OBFUS('C'), INIT_CHAR_OBFUS('u'), INIT_CHAR_OBFUS('s'), INIT_CHAR_OBFUS('t'), INIT_CHAR_OBFUS('o'), INIT_CHAR_OBFUS('m'), INIT_CHAR_OBFUS('D'), INIT_CHAR_OBFUS('a'), INIT_CHAR_OBFUS('t'), INIT_CHAR_OBFUS('a'), INIT_CHAR_OBFUS('\0')};
const DRM_DISCARDABLE DRM_ANSI_CONST_STRING g_dastrDomLeaveCustomDataTag = CREATE_DRM_ANSI_STRING_EX( g_rgchDomLeaveCustomDataTag, 10 );


/* "CertificateChain" */
DRM_STR_CONST DRM_CHAR g_rgchDomLeaveCertificateChainTag [] = { INIT_CHAR_OBFUS('C'), INIT_CHAR_OBFUS('e'), INIT_CHAR_OBFUS('r'), INIT_CHAR_OBFUS('t'), INIT_CHAR_OBFUS('i'), INIT_CHAR_OBFUS('f'), INIT_CHAR_OBFUS('i'), INIT_CHAR_OBFUS('c'), INIT_CHAR_OBFUS('a'), INIT_CHAR_OBFUS('t'), INIT_CHAR_OBFUS('e'), INIT_CHAR_OBFUS('C'), INIT_CHAR_OBFUS('h'), INIT_CHAR_OBFUS('a'), INIT_CHAR_OBFUS('i'), INIT_CHAR_OBFUS('n'), INIT_CHAR_OBFUS('\0')};
const DRM_DISCARDABLE DRM_ANSI_CONST_STRING g_dastrDomLeaveCertificateChainTag = CREATE_DRM_ANSI_STRING_EX( g_rgchDomLeaveCertificateChainTag, 16 );



/* "xmlns" */
DRM_STR_CONST DRM_CHAR g_rgchDomLeaveRootAttribName [] = { INIT_CHAR_OBFUS('x'), INIT_CHAR_OBFUS('m'), INIT_CHAR_OBFUS('l'), INIT_CHAR_OBFUS('n'), INIT_CHAR_OBFUS('s'), INIT_CHAR_OBFUS('\0')};
const DRM_DISCARDABLE DRM_ANSI_CONST_STRING g_dastrDomLeaveRootAttribName = CREATE_DRM_ANSI_STRING_EX( g_rgchDomLeaveRootAttribName, 5 );

/* "http://schemas.microsoft.com/DRM/2007/03/protocols" */
DRM_STR_CONST DRM_CHAR g_rgchDomLeaveRootAttribValue [] = { INIT_CHAR_OBFUS('h'), INIT_CHAR_OBFUS('t'), INIT_CHAR_OBFUS('t'), INIT_CHAR_OBFUS('p'), INIT_CHAR_OBFUS(':'), INIT_CHAR_OBFUS('/'), INIT_CHAR_OBFUS('/'), INIT_CHAR_OBFUS('s'), INIT_CHAR_OBFUS('c'), INIT_CHAR_OBFUS('h'), INIT_CHAR_OBFUS('e'), INIT_CHAR_OBFUS('m'), INIT_CHAR_OBFUS('a'), INIT_CHAR_OBFUS('s'), INIT_CHAR_OBFUS('.'), INIT_CHAR_OBFUS('m'), INIT_CHAR_OBFUS('i'), INIT_CHAR_OBFUS('c'), INIT_CHAR_OBFUS('r'), INIT_CHAR_OBFUS('o'), INIT_CHAR_OBFUS('s'), INIT_CHAR_OBFUS('o'), INIT_CHAR_OBFUS('f'), INIT_CHAR_OBFUS('t'), INIT_CHAR_OBFUS('.'), INIT_CHAR_OBFUS('c'), INIT_CHAR_OBFUS('o'), INIT_CHAR_OBFUS('m'), INIT_CHAR_OBFUS('/'), INIT_CHAR_OBFUS('D'), INIT_CHAR_OBFUS('R'), INIT_CHAR_OBFUS('M'), INIT_CHAR_OBFUS('/'), INIT_CHAR_OBFUS('2'), INIT_CHAR_OBFUS('0'), INIT_CHAR_OBFUS('0'), INIT_CHAR_OBFUS('7'), INIT_CHAR_OBFUS('/'), INIT_CHAR_OBFUS('0'), INIT_CHAR_OBFUS('3'), INIT_CHAR_OBFUS('/'), INIT_CHAR_OBFUS('p'), INIT_CHAR_OBFUS('r'), INIT_CHAR_OBFUS('o'), INIT_CHAR_OBFUS('t'), INIT_CHAR_OBFUS('o'), INIT_CHAR_OBFUS('c'), INIT_CHAR_OBFUS('o'), INIT_CHAR_OBFUS('l'), INIT_CHAR_OBFUS('s'), INIT_CHAR_OBFUS('\0')};
const DRM_DISCARDABLE DRM_ANSI_CONST_STRING g_dastrDomLeaveRootAttribValue = CREATE_DRM_ANSI_STRING_EX( g_rgchDomLeaveRootAttribValue, 50 );


/* "xmlns" */
DRM_STR_CONST DRM_CHAR g_rgchDomLeaveChallenge2AttribName [] = { INIT_CHAR_OBFUS('x'), INIT_CHAR_OBFUS('m'), INIT_CHAR_OBFUS('l'), INIT_CHAR_OBFUS('n'), INIT_CHAR_OBFUS('s'), INIT_CHAR_OBFUS('\0')};
const DRM_DISCARDABLE DRM_ANSI_CONST_STRING g_dastrDomLeaveChallenge2AttribName = CREATE_DRM_ANSI_STRING_EX( g_rgchDomLeaveChallenge2AttribName, 5 );

/* "http://schemas.microsoft.com/DRM/2007/03/protocols/messages" */
DRM_STR_CONST DRM_CHAR g_rgchDomLeaveChallenge2AttribValue [] = { INIT_CHAR_OBFUS('h'), INIT_CHAR_OBFUS('t'), INIT_CHAR_OBFUS('t'), INIT_CHAR_OBFUS('p'), INIT_CHAR_OBFUS(':'), INIT_CHAR_OBFUS('/'), INIT_CHAR_OBFUS('/'), INIT_CHAR_OBFUS('s'), INIT_CHAR_OBFUS('c'), INIT_CHAR_OBFUS('h'), INIT_CHAR_OBFUS('e'), INIT_CHAR_OBFUS('m'), INIT_CHAR_OBFUS('a'), INIT_CHAR_OBFUS('s'), INIT_CHAR_OBFUS('.'), INIT_CHAR_OBFUS('m'), INIT_CHAR_OBFUS('i'), INIT_CHAR_OBFUS('c'), INIT_CHAR_OBFUS('r'), INIT_CHAR_OBFUS('o'), INIT_CHAR_OBFUS('s'), INIT_CHAR_OBFUS('o'), INIT_CHAR_OBFUS('f'), INIT_CHAR_OBFUS('t'), INIT_CHAR_OBFUS('.'), INIT_CHAR_OBFUS('c'), INIT_CHAR_OBFUS('o'), INIT_CHAR_OBFUS('m'), INIT_CHAR_OBFUS('/'), INIT_CHAR_OBFUS('D'), INIT_CHAR_OBFUS('R'), INIT_CHAR_OBFUS('M'), INIT_CHAR_OBFUS('/'), INIT_CHAR_OBFUS('2'), INIT_CHAR_OBFUS('0'), INIT_CHAR_OBFUS('0'), INIT_CHAR_OBFUS('7'), INIT_CHAR_OBFUS('/'), INIT_CHAR_OBFUS('0'), INIT_CHAR_OBFUS('3'), INIT_CHAR_OBFUS('/'), INIT_CHAR_OBFUS('p'), INIT_CHAR_OBFUS('r'), INIT_CHAR_OBFUS('o'), INIT_CHAR_OBFUS('t'), INIT_CHAR_OBFUS('o'), INIT_CHAR_OBFUS('c'), INIT_CHAR_OBFUS('o'), INIT_CHAR_OBFUS('l'), INIT_CHAR_OBFUS('s'), INIT_CHAR_OBFUS('/'), INIT_CHAR_OBFUS('m'), INIT_CHAR_OBFUS('e'), INIT_CHAR_OBFUS('s'), INIT_CHAR_OBFUS('s'), INIT_CHAR_OBFUS('a'), INIT_CHAR_OBFUS('g'), INIT_CHAR_OBFUS('e'), INIT_CHAR_OBFUS('s'), INIT_CHAR_OBFUS('\0')};
const DRM_DISCARDABLE DRM_ANSI_CONST_STRING g_dastrDomLeaveChallenge2AttribValue = CREATE_DRM_ANSI_STRING_EX( g_rgchDomLeaveChallenge2AttribValue, 59 );


/* "xmlns" */
DRM_STR_CONST DRM_CHAR g_rgchDomLeaveDomainAttrib1Name [] = { INIT_CHAR_OBFUS('x'), INIT_CHAR_OBFUS('m'), INIT_CHAR_OBFUS('l'), INIT_CHAR_OBFUS('n'), INIT_CHAR_OBFUS('s'), INIT_CHAR_OBFUS('\0')};
const DRM_DISCARDABLE DRM_ANSI_CONST_STRING g_dastrDomLeaveDomainAttrib1Name = CREATE_DRM_ANSI_STRING_EX( g_rgchDomLeaveDomainAttrib1Name, 5 );

/* "http://schemas.microsoft.com/DRM/2007/03/protocols" */
DRM_STR_CONST DRM_CHAR g_rgchDomLeaveDomainAttrib1Value [] = { INIT_CHAR_OBFUS('h'), INIT_CHAR_OBFUS('t'), INIT_CHAR_OBFUS('t'), INIT_CHAR_OBFUS('p'), INIT_CHAR_OBFUS(':'), INIT_CHAR_OBFUS('/'), INIT_CHAR_OBFUS('/'), INIT_CHAR_OBFUS('s'), INIT_CHAR_OBFUS('c'), INIT_CHAR_OBFUS('h'), INIT_CHAR_OBFUS('e'), INIT_CHAR_OBFUS('m'), INIT_CHAR_OBFUS('a'), INIT_CHAR_OBFUS('s'), INIT_CHAR_OBFUS('.'), INIT_CHAR_OBFUS('m'), INIT_CHAR_OBFUS('i'), INIT_CHAR_OBFUS('c'), INIT_CHAR_OBFUS('r'), INIT_CHAR_OBFUS('o'), INIT_CHAR_OBFUS('s'), INIT_CHAR_OBFUS('o'), INIT_CHAR_OBFUS('f'), INIT_CHAR_OBFUS('t'), INIT_CHAR_OBFUS('.'), INIT_CHAR_OBFUS('c'), INIT_CHAR_OBFUS('o'), INIT_CHAR_OBFUS('m'), INIT_CHAR_OBFUS('/'), INIT_CHAR_OBFUS('D'), INIT_CHAR_OBFUS('R'), INIT_CHAR_OBFUS('M'), INIT_CHAR_OBFUS('/'), INIT_CHAR_OBFUS('2'), INIT_CHAR_OBFUS('0'), INIT_CHAR_OBFUS('0'), INIT_CHAR_OBFUS('7'), INIT_CHAR_OBFUS('/'), INIT_CHAR_OBFUS('0'), INIT_CHAR_OBFUS('3'), INIT_CHAR_OBFUS('/'), INIT_CHAR_OBFUS('p'), INIT_CHAR_OBFUS('r'), INIT_CHAR_OBFUS('o'), INIT_CHAR_OBFUS('t'), INIT_CHAR_OBFUS('o'), INIT_CHAR_OBFUS('c'), INIT_CHAR_OBFUS('o'), INIT_CHAR_OBFUS('l'), INIT_CHAR_OBFUS('s'), INIT_CHAR_OBFUS('\0')};
const DRM_DISCARDABLE DRM_ANSI_CONST_STRING g_dastrDomLeaveDomainAttrib1Value = CREATE_DRM_ANSI_STRING_EX( g_rgchDomLeaveDomainAttrib1Value, 50 );


/* "Id" */
DRM_STR_CONST DRM_CHAR g_rgchDomLeaveDomainAttrib2Name [] = { INIT_CHAR_OBFUS('I'), INIT_CHAR_OBFUS('d'), INIT_CHAR_OBFUS('\0')};
const DRM_DISCARDABLE DRM_ANSI_CONST_STRING g_dastrDomLeaveDomainAttrib2Name = CREATE_DRM_ANSI_STRING_EX( g_rgchDomLeaveDomainAttrib2Name, 2 );

/* "SignedData" */
DRM_STR_CONST DRM_CHAR g_rgchDomLeaveDomainAttrib2Value [] = { INIT_CHAR_OBFUS('S'), INIT_CHAR_OBFUS('i'), INIT_CHAR_OBFUS('g'), INIT_CHAR_OBFUS('n'), INIT_CHAR_OBFUS('e'), INIT_CHAR_OBFUS('d'), INIT_CHAR_OBFUS('D'), INIT_CHAR_OBFUS('a'), INIT_CHAR_OBFUS('t'), INIT_CHAR_OBFUS('a'), INIT_CHAR_OBFUS('\0')};
const DRM_DISCARDABLE DRM_ANSI_CONST_STRING g_dastrDomLeaveDomainAttrib2Value = CREATE_DRM_ANSI_STRING_EX( g_rgchDomLeaveDomainAttrib2Value, 10 );


/* "Type" */
DRM_STR_CONST DRM_CHAR g_rgchDomLeaveCertificateChainAttribName [] = { INIT_CHAR_OBFUS('T'), INIT_CHAR_OBFUS('y'), INIT_CHAR_OBFUS('p'), INIT_CHAR_OBFUS('e'), INIT_CHAR_OBFUS('\0')};
const DRM_DISCARDABLE DRM_ANSI_CONST_STRING g_dastrDomLeaveCertificateChainAttribName = CREATE_DRM_ANSI_STRING_EX( g_rgchDomLeaveCertificateChainAttribName, 4 );

/* "device" */
DRM_STR_CONST DRM_CHAR g_rgchDomLeaveCertificateChainAttribValue [] = { INIT_CHAR_OBFUS('d'), INIT_CHAR_OBFUS('e'), INIT_CHAR_OBFUS('v'), INIT_CHAR_OBFUS('i'), INIT_CHAR_OBFUS('c'), INIT_CHAR_OBFUS('e'), INIT_CHAR_OBFUS('\0')};
const DRM_DISCARDABLE DRM_ANSI_CONST_STRING g_dastrDomLeaveCertificateChainAttribValue = CREATE_DRM_ANSI_STRING_EX( g_rgchDomLeaveCertificateChainAttribValue, 6 );


/* "1" */
DRM_STR_CONST DRM_CHAR g_rgchDomainLeaveVersion [] = { INIT_CHAR_OBFUS('1'), INIT_CHAR_OBFUS('\0')};
const DRM_DISCARDABLE DRM_ANSI_CONST_STRING g_dastrDomainLeaveVersion = CREATE_DRM_ANSI_STRING_EX( g_rgchDomainLeaveVersion, 1 );




/*
** -------------------------------------------------------
** XML strings used in the parsing of domain join response
** -------------------------------------------------------
*/
/* "JoinDomainResponse" */
DRM_STR_CONST DRM_CHAR g_rgchDomJoinResponsePath [] = { INIT_CHAR_OBFUS('J'), INIT_CHAR_OBFUS('o'), INIT_CHAR_OBFUS('i'), INIT_CHAR_OBFUS('n'), INIT_CHAR_OBFUS('D'), INIT_CHAR_OBFUS('o'), INIT_CHAR_OBFUS('m'), INIT_CHAR_OBFUS('a'), INIT_CHAR_OBFUS('i'), INIT_CHAR_OBFUS('n'), INIT_CHAR_OBFUS('R'), INIT_CHAR_OBFUS('e'), INIT_CHAR_OBFUS('s'), INIT_CHAR_OBFUS('p'), INIT_CHAR_OBFUS('o'), INIT_CHAR_OBFUS('n'), INIT_CHAR_OBFUS('s'), INIT_CHAR_OBFUS('e'), INIT_CHAR_OBFUS('\0')};
const DRM_DISCARDABLE DRM_ANSI_CONST_STRING g_dastrDomJoinResponsePath = CREATE_DRM_ANSI_STRING_EX( g_rgchDomJoinResponsePath, 18 );


/* "Version" */
DRM_STR_CONST DRM_CHAR g_rgchDomJoinVersionPath [] = { INIT_CHAR_OBFUS('V'), INIT_CHAR_OBFUS('e'), INIT_CHAR_OBFUS('r'), INIT_CHAR_OBFUS('s'), INIT_CHAR_OBFUS('i'), INIT_CHAR_OBFUS('o'), INIT_CHAR_OBFUS('n'), INIT_CHAR_OBFUS('\0')};
const DRM_DISCARDABLE DRM_ANSI_CONST_STRING g_dastrDomJoinVersionPath = CREATE_DRM_ANSI_STRING_EX( g_rgchDomJoinVersionPath, 7 );


/* "ServiceID" */
DRM_STR_CONST DRM_CHAR g_rgchDomJoinServiceIDPath [] = { INIT_CHAR_OBFUS('S'), INIT_CHAR_OBFUS('e'), INIT_CHAR_OBFUS('r'), INIT_CHAR_OBFUS('v'), INIT_CHAR_OBFUS('i'), INIT_CHAR_OBFUS('c'), INIT_CHAR_OBFUS('e'), INIT_CHAR_OBFUS('I'), INIT_CHAR_OBFUS('D'), INIT_CHAR_OBFUS('\0')};
const DRM_DISCARDABLE DRM_ANSI_CONST_STRING g_dastrDomJoinServiceIDPath = CREATE_DRM_ANSI_STRING_EX( g_rgchDomJoinServiceIDPath, 9 );


/* "AccountID" */
DRM_STR_CONST DRM_CHAR g_rgchDomJoinAccountIDPath [] = { INIT_CHAR_OBFUS('A'), INIT_CHAR_OBFUS('c'), INIT_CHAR_OBFUS('c'), INIT_CHAR_OBFUS('o'), INIT_CHAR_OBFUS('u'), INIT_CHAR_OBFUS('n'), INIT_CHAR_OBFUS('t'), INIT_CHAR_OBFUS('I'), INIT_CHAR_OBFUS('D'), INIT_CHAR_OBFUS('\0')};
const DRM_DISCARDABLE DRM_ANSI_CONST_STRING g_dastrDomJoinAccountIDPath = CREATE_DRM_ANSI_STRING_EX( g_rgchDomJoinAccountIDPath, 9 );


/* "CertificateChain" */
DRM_STR_CONST DRM_CHAR g_rgchDomJoinCertificateChainPath [] = { INIT_CHAR_OBFUS('C'), INIT_CHAR_OBFUS('e'), INIT_CHAR_OBFUS('r'), INIT_CHAR_OBFUS('t'), INIT_CHAR_OBFUS('i'), INIT_CHAR_OBFUS('f'), INIT_CHAR_OBFUS('i'), INIT_CHAR_OBFUS('c'), INIT_CHAR_OBFUS('a'), INIT_CHAR_OBFUS('t'), INIT_CHAR_OBFUS('e'), INIT_CHAR_OBFUS('C'), INIT_CHAR_OBFUS('h'), INIT_CHAR_OBFUS('a'), INIT_CHAR_OBFUS('i'), INIT_CHAR_OBFUS('n'), INIT_CHAR_OBFUS('\0')};
const DRM_DISCARDABLE DRM_ANSI_CONST_STRING g_dastrDomJoinCertificateChainPath = CREATE_DRM_ANSI_STRING_EX( g_rgchDomJoinCertificateChainPath, 16 );


/* "DomainKeys" */
DRM_STR_CONST DRM_CHAR g_rgchDomJoinDomainKeysPath [] = { INIT_CHAR_OBFUS('D'), INIT_CHAR_OBFUS('o'), INIT_CHAR_OBFUS('m'), INIT_CHAR_OBFUS('a'), INIT_CHAR_OBFUS('i'), INIT_CHAR_OBFUS('n'), INIT_CHAR_OBFUS('K'), INIT_CHAR_OBFUS('e'), INIT_CHAR_OBFUS('y'), INIT_CHAR_OBFUS('s'), INIT_CHAR_OBFUS('\0')};
const DRM_DISCARDABLE DRM_ANSI_CONST_STRING g_dastrDomJoinDomainKeysPath = CREATE_DRM_ANSI_STRING_EX( g_rgchDomJoinDomainKeysPath, 10 );

/* "JoinDomainResult/Response/Domain/SigningCertificateChain" */
DRM_STR_CONST DRM_CHAR g_rgchDomJoinSigningCertPath [] = { INIT_CHAR_OBFUS('J'), INIT_CHAR_OBFUS('o'), INIT_CHAR_OBFUS('i'), INIT_CHAR_OBFUS('n'), INIT_CHAR_OBFUS('D'), INIT_CHAR_OBFUS('o'), INIT_CHAR_OBFUS('m'), INIT_CHAR_OBFUS('a'), INIT_CHAR_OBFUS('i'), INIT_CHAR_OBFUS('n'), INIT_CHAR_OBFUS('R'), INIT_CHAR_OBFUS('e'), INIT_CHAR_OBFUS('s'), INIT_CHAR_OBFUS('u'), INIT_CHAR_OBFUS('l'), INIT_CHAR_OBFUS('t'), INIT_CHAR_OBFUS('/'), INIT_CHAR_OBFUS('R'), INIT_CHAR_OBFUS('e'), INIT_CHAR_OBFUS('s'), INIT_CHAR_OBFUS('p'), INIT_CHAR_OBFUS('o'), INIT_CHAR_OBFUS('n'), INIT_CHAR_OBFUS('s'), INIT_CHAR_OBFUS('e'), INIT_CHAR_OBFUS('/'), INIT_CHAR_OBFUS('D'), INIT_CHAR_OBFUS('o'), INIT_CHAR_OBFUS('m'), INIT_CHAR_OBFUS('a'), INIT_CHAR_OBFUS('i'), INIT_CHAR_OBFUS('n'), INIT_CHAR_OBFUS('/'), INIT_CHAR_OBFUS('S'), INIT_CHAR_OBFUS('i'), INIT_CHAR_OBFUS('g'), INIT_CHAR_OBFUS('n'), INIT_CHAR_OBFUS('i'), INIT_CHAR_OBFUS('n'), INIT_CHAR_OBFUS('g'), INIT_CHAR_OBFUS('C'), INIT_CHAR_OBFUS('e'), INIT_CHAR_OBFUS('r'), INIT_CHAR_OBFUS('t'), INIT_CHAR_OBFUS('i'), INIT_CHAR_OBFUS('f'), INIT_CHAR_OBFUS('i'), INIT_CHAR_OBFUS('c'), INIT_CHAR_OBFUS('a'), INIT_CHAR_OBFUS('t'), INIT_CHAR_OBFUS('e'), INIT_CHAR_OBFUS('C'), INIT_CHAR_OBFUS('h'), INIT_CHAR_OBFUS('a'), INIT_CHAR_OBFUS('i'), INIT_CHAR_OBFUS('n'), INIT_CHAR_OBFUS('\0')};
const DRM_DISCARDABLE DRM_ANSI_CONST_STRING g_dastrDomJoinSigningCertPath = CREATE_DRM_ANSI_STRING_EX( g_rgchDomJoinSigningCertPath, 56 );

/*
** --------------------------------------------------------
** XML strings used in the parsing of domain leave response
** --------------------------------------------------------
*/
/* "LeaveDomainResult/Response/Domain/AccountID" */
DRM_STR_CONST DRM_CHAR g_rgchDomLeaveAccountIDPath [] = { INIT_CHAR_OBFUS('L'), INIT_CHAR_OBFUS('e'), INIT_CHAR_OBFUS('a'), INIT_CHAR_OBFUS('v'), INIT_CHAR_OBFUS('e'), INIT_CHAR_OBFUS('D'), INIT_CHAR_OBFUS('o'), INIT_CHAR_OBFUS('m'), INIT_CHAR_OBFUS('a'), INIT_CHAR_OBFUS('i'), INIT_CHAR_OBFUS('n'), INIT_CHAR_OBFUS('R'), INIT_CHAR_OBFUS('e'), INIT_CHAR_OBFUS('s'), INIT_CHAR_OBFUS('u'), INIT_CHAR_OBFUS('l'), INIT_CHAR_OBFUS('t'), INIT_CHAR_OBFUS('/'), INIT_CHAR_OBFUS('R'), INIT_CHAR_OBFUS('e'), INIT_CHAR_OBFUS('s'), INIT_CHAR_OBFUS('p'), INIT_CHAR_OBFUS('o'), INIT_CHAR_OBFUS('n'), INIT_CHAR_OBFUS('s'), INIT_CHAR_OBFUS('e'), INIT_CHAR_OBFUS('/'), INIT_CHAR_OBFUS('D'), INIT_CHAR_OBFUS('o'), INIT_CHAR_OBFUS('m'), INIT_CHAR_OBFUS('a'), INIT_CHAR_OBFUS('i'), INIT_CHAR_OBFUS('n'), INIT_CHAR_OBFUS('/'), INIT_CHAR_OBFUS('A'), INIT_CHAR_OBFUS('c'), INIT_CHAR_OBFUS('c'), INIT_CHAR_OBFUS('o'), INIT_CHAR_OBFUS('u'), INIT_CHAR_OBFUS('n'), INIT_CHAR_OBFUS('t'), INIT_CHAR_OBFUS('I'), INIT_CHAR_OBFUS('D'), INIT_CHAR_OBFUS('\0')};
const DRM_DISCARDABLE DRM_ANSI_CONST_STRING g_dastrDomLeaveAccountIDPath = CREATE_DRM_ANSI_STRING_EX( g_rgchDomLeaveAccountIDPath, 43 );


EXIT_PK_NAMESPACE_CODE;

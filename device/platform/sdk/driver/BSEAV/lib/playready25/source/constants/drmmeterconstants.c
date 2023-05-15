/**@@@+++@@@@******************************************************************
**
** Microsoft (r) PlayReady (r)
** Copyright (c) Microsoft Corporation. All rights reserved.
**
***@@@---@@@@******************************************************************
*/

#include <drmmeterconstants.h>
#include <drmbytemanip.h>

ENTER_PK_NAMESPACE_CODE;

/*
** ------------------------------------------------------------
** XML strings used in the construction of meter data challenge
** ------------------------------------------------------------
*/
/* "ProcessMeteringData" */
DRM_STR_CONST DRM_EXPORT_VAR DRM_CHAR g_rgchMeterDataRootTag [] = { INIT_CHAR_OBFUS('P'), INIT_CHAR_OBFUS('r'), INIT_CHAR_OBFUS('o'), INIT_CHAR_OBFUS('c'), INIT_CHAR_OBFUS('e'), INIT_CHAR_OBFUS('s'), INIT_CHAR_OBFUS('s'), INIT_CHAR_OBFUS('M'), INIT_CHAR_OBFUS('e'), INIT_CHAR_OBFUS('t'), INIT_CHAR_OBFUS('e'), INIT_CHAR_OBFUS('r'), INIT_CHAR_OBFUS('i'), INIT_CHAR_OBFUS('n'), INIT_CHAR_OBFUS('g'), INIT_CHAR_OBFUS('D'), INIT_CHAR_OBFUS('a'), INIT_CHAR_OBFUS('t'), INIT_CHAR_OBFUS('a'), INIT_CHAR_OBFUS('\0')};
const DRM_EXPORT_VAR DRM_DISCARDABLE DRM_ANSI_CONST_STRING g_dastrMeterDataRootTag = CREATE_DRM_ANSI_STRING_EX( g_rgchMeterDataRootTag, 19 );


/* "challenge" */
DRM_STR_CONST DRM_EXPORT_VAR DRM_CHAR g_rgchMeterDataChallenge1Tag [] = { INIT_CHAR_OBFUS('c'), INIT_CHAR_OBFUS('h'), INIT_CHAR_OBFUS('a'), INIT_CHAR_OBFUS('l'), INIT_CHAR_OBFUS('l'), INIT_CHAR_OBFUS('e'), INIT_CHAR_OBFUS('n'), INIT_CHAR_OBFUS('g'), INIT_CHAR_OBFUS('e'), INIT_CHAR_OBFUS('\0')};
const DRM_EXPORT_VAR DRM_DISCARDABLE DRM_ANSI_CONST_STRING g_dastrMeterDataChallenge1Tag = CREATE_DRM_ANSI_STRING_EX( g_rgchMeterDataChallenge1Tag, 9 );


/* "Challenge" */
DRM_STR_CONST DRM_EXPORT_VAR DRM_CHAR g_rgchMeterDataChallenge2Tag [] = { INIT_CHAR_OBFUS('C'), INIT_CHAR_OBFUS('h'), INIT_CHAR_OBFUS('a'), INIT_CHAR_OBFUS('l'), INIT_CHAR_OBFUS('l'), INIT_CHAR_OBFUS('e'), INIT_CHAR_OBFUS('n'), INIT_CHAR_OBFUS('g'), INIT_CHAR_OBFUS('e'), INIT_CHAR_OBFUS('\0')};
const DRM_EXPORT_VAR DRM_DISCARDABLE DRM_ANSI_CONST_STRING g_dastrMeterDataChallenge2Tag = CREATE_DRM_ANSI_STRING_EX( g_rgchMeterDataChallenge2Tag, 9 );


/* "MeteringData" */
DRM_STR_CONST DRM_EXPORT_VAR DRM_CHAR g_rgchMeterDataMeteringDataTag [] = { INIT_CHAR_OBFUS('M'), INIT_CHAR_OBFUS('e'), INIT_CHAR_OBFUS('t'), INIT_CHAR_OBFUS('e'), INIT_CHAR_OBFUS('r'), INIT_CHAR_OBFUS('i'), INIT_CHAR_OBFUS('n'), INIT_CHAR_OBFUS('g'), INIT_CHAR_OBFUS('D'), INIT_CHAR_OBFUS('a'), INIT_CHAR_OBFUS('t'), INIT_CHAR_OBFUS('a'), INIT_CHAR_OBFUS('\0')};
const DRM_EXPORT_VAR DRM_DISCARDABLE DRM_ANSI_CONST_STRING g_dastrMeterDataMeteringDataTag = CREATE_DRM_ANSI_STRING_EX( g_rgchMeterDataMeteringDataTag, 12 );


/* "Version" */
DRM_STR_CONST DRM_EXPORT_VAR DRM_CHAR g_rgchMeterDataVersionTag [] = { INIT_CHAR_OBFUS('V'), INIT_CHAR_OBFUS('e'), INIT_CHAR_OBFUS('r'), INIT_CHAR_OBFUS('s'), INIT_CHAR_OBFUS('i'), INIT_CHAR_OBFUS('o'), INIT_CHAR_OBFUS('n'), INIT_CHAR_OBFUS('\0')};
const DRM_EXPORT_VAR DRM_DISCARDABLE DRM_ANSI_CONST_STRING g_dastrMeterDataVersionTag = CREATE_DRM_ANSI_STRING_EX( g_rgchMeterDataVersionTag, 7 );


/* "MID" */
DRM_STR_CONST DRM_EXPORT_VAR DRM_CHAR g_rgchMeterDataMIDTag [] = { INIT_CHAR_OBFUS('M'), INIT_CHAR_OBFUS('I'), INIT_CHAR_OBFUS('D'), INIT_CHAR_OBFUS('\0')};
const DRM_EXPORT_VAR DRM_DISCARDABLE DRM_ANSI_CONST_STRING g_dastrMeterDataMIDTag = CREATE_DRM_ANSI_STRING_EX( g_rgchMeterDataMIDTag, 3 );


/* "TID" */
DRM_STR_CONST DRM_EXPORT_VAR DRM_CHAR g_rgchMeterDataTIDTag [] = { INIT_CHAR_OBFUS('T'), INIT_CHAR_OBFUS('I'), INIT_CHAR_OBFUS('D'), INIT_CHAR_OBFUS('\0')};
const DRM_EXPORT_VAR DRM_DISCARDABLE DRM_ANSI_CONST_STRING g_dastrMeterDataTIDTag = CREATE_DRM_ANSI_STRING_EX( g_rgchMeterDataTIDTag, 3 );


/* "CustomData" */
DRM_STR_CONST DRM_EXPORT_VAR DRM_CHAR g_rgchMeterDataCustomDataTag [] = { INIT_CHAR_OBFUS('C'), INIT_CHAR_OBFUS('u'), INIT_CHAR_OBFUS('s'), INIT_CHAR_OBFUS('t'), INIT_CHAR_OBFUS('o'), INIT_CHAR_OBFUS('m'), INIT_CHAR_OBFUS('D'), INIT_CHAR_OBFUS('a'), INIT_CHAR_OBFUS('t'), INIT_CHAR_OBFUS('a'), INIT_CHAR_OBFUS('\0')};
const DRM_EXPORT_VAR DRM_DISCARDABLE DRM_ANSI_CONST_STRING g_dastrMeterDataCustomDataTag = CREATE_DRM_ANSI_STRING_EX( g_rgchMeterDataCustomDataTag, 10 );


/* "PARTIALDATA" */
DRM_STR_CONST DRM_EXPORT_VAR DRM_CHAR g_rgchMeterDataPartialDataTag [] = { INIT_CHAR_OBFUS('P'), INIT_CHAR_OBFUS('A'), INIT_CHAR_OBFUS('R'), INIT_CHAR_OBFUS('T'), INIT_CHAR_OBFUS('I'), INIT_CHAR_OBFUS('A'), INIT_CHAR_OBFUS('L'), INIT_CHAR_OBFUS('D'), INIT_CHAR_OBFUS('A'), INIT_CHAR_OBFUS('T'), INIT_CHAR_OBFUS('A'), INIT_CHAR_OBFUS('\0')};
const DRM_EXPORT_VAR DRM_DISCARDABLE DRM_ANSI_CONST_STRING g_dastrMeterDataPartialDataTag = CREATE_DRM_ANSI_STRING_EX( g_rgchMeterDataPartialDataTag, 11 );


/* "Data" */
DRM_STR_CONST DRM_EXPORT_VAR DRM_CHAR g_rgchMeterDataDataTag [] = { INIT_CHAR_OBFUS('D'), INIT_CHAR_OBFUS('a'), INIT_CHAR_OBFUS('t'), INIT_CHAR_OBFUS('a'), INIT_CHAR_OBFUS('\0')};
const DRM_EXPORT_VAR DRM_DISCARDABLE DRM_ANSI_CONST_STRING g_dastrMeterDataDataTag = CREATE_DRM_ANSI_STRING_EX( g_rgchMeterDataDataTag, 4 );


/* "CertificateChain" */
DRM_STR_CONST DRM_EXPORT_VAR DRM_CHAR g_rgchMeterDataDeviceCertTag [] = { INIT_CHAR_OBFUS('C'), INIT_CHAR_OBFUS('e'), INIT_CHAR_OBFUS('r'), INIT_CHAR_OBFUS('t'), INIT_CHAR_OBFUS('i'), INIT_CHAR_OBFUS('f'), INIT_CHAR_OBFUS('i'), INIT_CHAR_OBFUS('c'), INIT_CHAR_OBFUS('a'), INIT_CHAR_OBFUS('t'), INIT_CHAR_OBFUS('e'), INIT_CHAR_OBFUS('C'), INIT_CHAR_OBFUS('h'), INIT_CHAR_OBFUS('a'), INIT_CHAR_OBFUS('i'), INIT_CHAR_OBFUS('n'), INIT_CHAR_OBFUS('\0')};
const DRM_EXPORT_VAR DRM_DISCARDABLE DRM_ANSI_CONST_STRING g_dastrMeterDataDeviceCertTag = CREATE_DRM_ANSI_STRING_EX( g_rgchMeterDataDeviceCertTag, 16 );


/* "Records" */
DRM_STR_CONST DRM_EXPORT_VAR DRM_CHAR g_rgchMeterDataRecordsTag [] = { INIT_CHAR_OBFUS('R'), INIT_CHAR_OBFUS('e'), INIT_CHAR_OBFUS('c'), INIT_CHAR_OBFUS('o'), INIT_CHAR_OBFUS('r'), INIT_CHAR_OBFUS('d'), INIT_CHAR_OBFUS('s'), INIT_CHAR_OBFUS('\0')};
const DRM_EXPORT_VAR DRM_DISCARDABLE DRM_ANSI_CONST_STRING g_dastrMeterDataRecordsTag = CREATE_DRM_ANSI_STRING_EX( g_rgchMeterDataRecordsTag, 7 );


/* "Record" */
DRM_STR_CONST DRM_EXPORT_VAR DRM_CHAR g_rgchMeterDataRecordTag [] = { INIT_CHAR_OBFUS('R'), INIT_CHAR_OBFUS('e'), INIT_CHAR_OBFUS('c'), INIT_CHAR_OBFUS('o'), INIT_CHAR_OBFUS('r'), INIT_CHAR_OBFUS('d'), INIT_CHAR_OBFUS('\0')};
const DRM_EXPORT_VAR DRM_DISCARDABLE DRM_ANSI_CONST_STRING g_dastrMeterDataRecordTag = CREATE_DRM_ANSI_STRING_EX( g_rgchMeterDataRecordTag, 6 );


/* "Action" */
DRM_STR_CONST DRM_EXPORT_VAR DRM_CHAR g_rgchMeterDataActionTag [] = { INIT_CHAR_OBFUS('A'), INIT_CHAR_OBFUS('c'), INIT_CHAR_OBFUS('t'), INIT_CHAR_OBFUS('i'), INIT_CHAR_OBFUS('o'), INIT_CHAR_OBFUS('n'), INIT_CHAR_OBFUS('\0')};
const DRM_EXPORT_VAR DRM_DISCARDABLE DRM_ANSI_CONST_STRING g_dastrMeterDataActionTag = CREATE_DRM_ANSI_STRING_EX( g_rgchMeterDataActionTag, 6 );



/* "xmlns" */
DRM_STR_CONST DRM_EXPORT_VAR DRM_CHAR g_rgchMeterDataRootAttribName [] = { INIT_CHAR_OBFUS('x'), INIT_CHAR_OBFUS('m'), INIT_CHAR_OBFUS('l'), INIT_CHAR_OBFUS('n'), INIT_CHAR_OBFUS('s'), INIT_CHAR_OBFUS('\0')};
const DRM_EXPORT_VAR DRM_DISCARDABLE DRM_ANSI_CONST_STRING g_dastrMeterDataRootAttribName = CREATE_DRM_ANSI_STRING_EX( g_rgchMeterDataRootAttribName, 5 );

/* "http://schemas.microsoft.com/DRM/2007/03/protocols" */
DRM_STR_CONST DRM_EXPORT_VAR DRM_CHAR g_rgchMeterDataRootAttribValue [] = { INIT_CHAR_OBFUS('h'), INIT_CHAR_OBFUS('t'), INIT_CHAR_OBFUS('t'), INIT_CHAR_OBFUS('p'), INIT_CHAR_OBFUS(':'), INIT_CHAR_OBFUS('/'), INIT_CHAR_OBFUS('/'), INIT_CHAR_OBFUS('s'), INIT_CHAR_OBFUS('c'), INIT_CHAR_OBFUS('h'), INIT_CHAR_OBFUS('e'), INIT_CHAR_OBFUS('m'), INIT_CHAR_OBFUS('a'), INIT_CHAR_OBFUS('s'), INIT_CHAR_OBFUS('.'), INIT_CHAR_OBFUS('m'), INIT_CHAR_OBFUS('i'), INIT_CHAR_OBFUS('c'), INIT_CHAR_OBFUS('r'), INIT_CHAR_OBFUS('o'), INIT_CHAR_OBFUS('s'), INIT_CHAR_OBFUS('o'), INIT_CHAR_OBFUS('f'), INIT_CHAR_OBFUS('t'), INIT_CHAR_OBFUS('.'), INIT_CHAR_OBFUS('c'), INIT_CHAR_OBFUS('o'), INIT_CHAR_OBFUS('m'), INIT_CHAR_OBFUS('/'), INIT_CHAR_OBFUS('D'), INIT_CHAR_OBFUS('R'), INIT_CHAR_OBFUS('M'), INIT_CHAR_OBFUS('/'), INIT_CHAR_OBFUS('2'), INIT_CHAR_OBFUS('0'), INIT_CHAR_OBFUS('0'), INIT_CHAR_OBFUS('7'), INIT_CHAR_OBFUS('/'), INIT_CHAR_OBFUS('0'), INIT_CHAR_OBFUS('3'), INIT_CHAR_OBFUS('/'), INIT_CHAR_OBFUS('p'), INIT_CHAR_OBFUS('r'), INIT_CHAR_OBFUS('o'), INIT_CHAR_OBFUS('t'), INIT_CHAR_OBFUS('o'), INIT_CHAR_OBFUS('c'), INIT_CHAR_OBFUS('o'), INIT_CHAR_OBFUS('l'), INIT_CHAR_OBFUS('s'), INIT_CHAR_OBFUS('\0')};
const DRM_EXPORT_VAR DRM_DISCARDABLE DRM_ANSI_CONST_STRING g_dastrMeterDataRootAttribValue = CREATE_DRM_ANSI_STRING_EX( g_rgchMeterDataRootAttribValue, 50 );


/* "xmlns" */
DRM_STR_CONST DRM_EXPORT_VAR DRM_CHAR g_rgchMeterDataChallenge2AttribName [] = { INIT_CHAR_OBFUS('x'), INIT_CHAR_OBFUS('m'), INIT_CHAR_OBFUS('l'), INIT_CHAR_OBFUS('n'), INIT_CHAR_OBFUS('s'), INIT_CHAR_OBFUS('\0')};
const DRM_EXPORT_VAR DRM_DISCARDABLE DRM_ANSI_CONST_STRING g_dastrMeterDataChallenge2AttribName = CREATE_DRM_ANSI_STRING_EX( g_rgchMeterDataChallenge2AttribName, 5 );

/* "http://schemas.microsoft.com/DRM/2007/03/protocols/messages" */
DRM_STR_CONST DRM_EXPORT_VAR DRM_CHAR g_rgchMeterDataChallenge2AttribValue [] = { INIT_CHAR_OBFUS('h'), INIT_CHAR_OBFUS('t'), INIT_CHAR_OBFUS('t'), INIT_CHAR_OBFUS('p'), INIT_CHAR_OBFUS(':'), INIT_CHAR_OBFUS('/'), INIT_CHAR_OBFUS('/'), INIT_CHAR_OBFUS('s'), INIT_CHAR_OBFUS('c'), INIT_CHAR_OBFUS('h'), INIT_CHAR_OBFUS('e'), INIT_CHAR_OBFUS('m'), INIT_CHAR_OBFUS('a'), INIT_CHAR_OBFUS('s'), INIT_CHAR_OBFUS('.'), INIT_CHAR_OBFUS('m'), INIT_CHAR_OBFUS('i'), INIT_CHAR_OBFUS('c'), INIT_CHAR_OBFUS('r'), INIT_CHAR_OBFUS('o'), INIT_CHAR_OBFUS('s'), INIT_CHAR_OBFUS('o'), INIT_CHAR_OBFUS('f'), INIT_CHAR_OBFUS('t'), INIT_CHAR_OBFUS('.'), INIT_CHAR_OBFUS('c'), INIT_CHAR_OBFUS('o'), INIT_CHAR_OBFUS('m'), INIT_CHAR_OBFUS('/'), INIT_CHAR_OBFUS('D'), INIT_CHAR_OBFUS('R'), INIT_CHAR_OBFUS('M'), INIT_CHAR_OBFUS('/'), INIT_CHAR_OBFUS('2'), INIT_CHAR_OBFUS('0'), INIT_CHAR_OBFUS('0'), INIT_CHAR_OBFUS('7'), INIT_CHAR_OBFUS('/'), INIT_CHAR_OBFUS('0'), INIT_CHAR_OBFUS('3'), INIT_CHAR_OBFUS('/'), INIT_CHAR_OBFUS('p'), INIT_CHAR_OBFUS('r'), INIT_CHAR_OBFUS('o'), INIT_CHAR_OBFUS('t'), INIT_CHAR_OBFUS('o'), INIT_CHAR_OBFUS('c'), INIT_CHAR_OBFUS('o'), INIT_CHAR_OBFUS('l'), INIT_CHAR_OBFUS('s'), INIT_CHAR_OBFUS('/'), INIT_CHAR_OBFUS('m'), INIT_CHAR_OBFUS('e'), INIT_CHAR_OBFUS('s'), INIT_CHAR_OBFUS('s'), INIT_CHAR_OBFUS('a'), INIT_CHAR_OBFUS('g'), INIT_CHAR_OBFUS('e'), INIT_CHAR_OBFUS('s'), INIT_CHAR_OBFUS('\0')};
const DRM_EXPORT_VAR DRM_DISCARDABLE DRM_ANSI_CONST_STRING g_dastrMeterDataChallenge2AttribValue = CREATE_DRM_ANSI_STRING_EX( g_rgchMeterDataChallenge2AttribValue, 59 );


/* "xmlns" */
DRM_STR_CONST DRM_EXPORT_VAR DRM_CHAR g_rgchMeterDataMeteringDataAttrib1Name [] = { INIT_CHAR_OBFUS('x'), INIT_CHAR_OBFUS('m'), INIT_CHAR_OBFUS('l'), INIT_CHAR_OBFUS('n'), INIT_CHAR_OBFUS('s'), INIT_CHAR_OBFUS('\0')};
const DRM_EXPORT_VAR DRM_DISCARDABLE DRM_ANSI_CONST_STRING g_dastrMeterDataMeteringDataAttrib1Name = CREATE_DRM_ANSI_STRING_EX( g_rgchMeterDataMeteringDataAttrib1Name, 5 );


/* "http://schemas.microsoft.com/DRM/2007/03/protocols" */
DRM_STR_CONST DRM_EXPORT_VAR DRM_CHAR g_rgchMeterDataMeteringDataAttrib1Value [] = { INIT_CHAR_OBFUS('h'), INIT_CHAR_OBFUS('t'), INIT_CHAR_OBFUS('t'), INIT_CHAR_OBFUS('p'), INIT_CHAR_OBFUS(':'), INIT_CHAR_OBFUS('/'), INIT_CHAR_OBFUS('/'), INIT_CHAR_OBFUS('s'), INIT_CHAR_OBFUS('c'), INIT_CHAR_OBFUS('h'), INIT_CHAR_OBFUS('e'), INIT_CHAR_OBFUS('m'), INIT_CHAR_OBFUS('a'), INIT_CHAR_OBFUS('s'), INIT_CHAR_OBFUS('.'), INIT_CHAR_OBFUS('m'), INIT_CHAR_OBFUS('i'), INIT_CHAR_OBFUS('c'), INIT_CHAR_OBFUS('r'), INIT_CHAR_OBFUS('o'), INIT_CHAR_OBFUS('s'), INIT_CHAR_OBFUS('o'), INIT_CHAR_OBFUS('f'), INIT_CHAR_OBFUS('t'), INIT_CHAR_OBFUS('.'), INIT_CHAR_OBFUS('c'), INIT_CHAR_OBFUS('o'), INIT_CHAR_OBFUS('m'), INIT_CHAR_OBFUS('/'), INIT_CHAR_OBFUS('D'), INIT_CHAR_OBFUS('R'), INIT_CHAR_OBFUS('M'), INIT_CHAR_OBFUS('/'), INIT_CHAR_OBFUS('2'), INIT_CHAR_OBFUS('0'), INIT_CHAR_OBFUS('0'), INIT_CHAR_OBFUS('7'), INIT_CHAR_OBFUS('/'), INIT_CHAR_OBFUS('0'), INIT_CHAR_OBFUS('3'), INIT_CHAR_OBFUS('/'), INIT_CHAR_OBFUS('p'), INIT_CHAR_OBFUS('r'), INIT_CHAR_OBFUS('o'), INIT_CHAR_OBFUS('t'), INIT_CHAR_OBFUS('o'), INIT_CHAR_OBFUS('c'), INIT_CHAR_OBFUS('o'), INIT_CHAR_OBFUS('l'), INIT_CHAR_OBFUS('s'), INIT_CHAR_OBFUS('\0')};
const DRM_EXPORT_VAR DRM_DISCARDABLE DRM_ANSI_CONST_STRING g_dastrMeterDataMeteringDataAttrib1Value = CREATE_DRM_ANSI_STRING_EX( g_rgchMeterDataMeteringDataAttrib1Value, 50 );


/* "Id" */
DRM_STR_CONST DRM_EXPORT_VAR DRM_CHAR g_rgchMeterDataMeteringDataAttrib2Name [] = { INIT_CHAR_OBFUS('I'), INIT_CHAR_OBFUS('d'), INIT_CHAR_OBFUS('\0')};
const DRM_EXPORT_VAR DRM_DISCARDABLE DRM_ANSI_CONST_STRING g_dastrMeterDataMeteringDataAttrib2Name = CREATE_DRM_ANSI_STRING_EX( g_rgchMeterDataMeteringDataAttrib2Name, 2 );

/* "SignedData" */
DRM_STR_CONST DRM_EXPORT_VAR DRM_CHAR g_rgchMeterDataMeteringDataAttrib2Value [] = { INIT_CHAR_OBFUS('S'), INIT_CHAR_OBFUS('i'), INIT_CHAR_OBFUS('g'), INIT_CHAR_OBFUS('n'), INIT_CHAR_OBFUS('e'), INIT_CHAR_OBFUS('d'), INIT_CHAR_OBFUS('D'), INIT_CHAR_OBFUS('a'), INIT_CHAR_OBFUS('t'), INIT_CHAR_OBFUS('a'), INIT_CHAR_OBFUS('\0')};
const DRM_EXPORT_VAR DRM_DISCARDABLE DRM_ANSI_CONST_STRING g_dastrMeterDataMeteringDataAttrib2Value = CREATE_DRM_ANSI_STRING_EX( g_rgchMeterDataMeteringDataAttrib2Value, 10 );


/* "KID" */
DRM_STR_CONST DRM_EXPORT_VAR DRM_CHAR g_rgchMeterDataRecordAttribName [] = { INIT_CHAR_OBFUS('K'), INIT_CHAR_OBFUS('I'), INIT_CHAR_OBFUS('D'), INIT_CHAR_OBFUS('\0')};
const DRM_DISCARDABLE DRM_ANSI_CONST_STRING g_dastrMeterDataRecordAttribName = CREATE_DRM_ANSI_STRING_EX( g_rgchMeterDataRecordAttribName, 3 );


/* "Name" */
DRM_STR_CONST DRM_EXPORT_VAR DRM_CHAR g_rgchMeterDataActionAttrib1Name [] = { INIT_CHAR_OBFUS('N'), INIT_CHAR_OBFUS('a'), INIT_CHAR_OBFUS('m'), INIT_CHAR_OBFUS('e'), INIT_CHAR_OBFUS('\0')};
const DRM_EXPORT_VAR DRM_DISCARDABLE DRM_ANSI_CONST_STRING g_dastrMeterDataActionAttrib1Name = CREATE_DRM_ANSI_STRING_EX( g_rgchMeterDataActionAttrib1Name, 4 );


/* "Value" */
DRM_STR_CONST DRM_EXPORT_VAR DRM_CHAR g_rgchMeterDataActionAttrib2Name [] = { INIT_CHAR_OBFUS('V'), INIT_CHAR_OBFUS('a'), INIT_CHAR_OBFUS('l'), INIT_CHAR_OBFUS('u'), INIT_CHAR_OBFUS('e'), INIT_CHAR_OBFUS('\0')};
const DRM_EXPORT_VAR DRM_DISCARDABLE DRM_ANSI_CONST_STRING g_dastrMeterDataActionAttrib2Name = CREATE_DRM_ANSI_STRING_EX( g_rgchMeterDataActionAttrib2Name, 5 );



/* "2" */
DRM_STR_CONST DRM_EXPORT_VAR DRM_CHAR g_rgchMeterDataVersionValue [] = { INIT_CHAR_OBFUS('2'), INIT_CHAR_OBFUS('\0')};
const DRM_EXPORT_VAR DRM_DISCARDABLE DRM_ANSI_CONST_STRING g_dastrMeterDataVersionValue = CREATE_DRM_ANSI_STRING_EX( g_rgchMeterDataVersionValue, 1 );


/* "1" */
DRM_STR_CONST DRM_EXPORT_VAR DRM_CHAR g_rgchMeterDataPartialDataValue [] = { INIT_CHAR_OBFUS('1'), INIT_CHAR_OBFUS('\0')};
const DRM_EXPORT_VAR DRM_DISCARDABLE DRM_ANSI_CONST_STRING g_dastrMeterDataPartialDataValue = CREATE_DRM_ANSI_STRING_EX( g_rgchMeterDataPartialDataValue, 1 );


/*
** ------------------------------------------------------
** XML strings used in the parsing of meter data response
** ------------------------------------------------------
*/
/* "ProcessMeteringDataResult/Response/MeteringResponse/MeterCert" */
DRM_STR_CONST DRM_EXPORT_VAR DRM_CHAR g_rgchMeterDataMeterCertPath [] = { INIT_CHAR_OBFUS('P'), INIT_CHAR_OBFUS('r'), INIT_CHAR_OBFUS('o'), INIT_CHAR_OBFUS('c'), INIT_CHAR_OBFUS('e'), INIT_CHAR_OBFUS('s'), INIT_CHAR_OBFUS('s'), INIT_CHAR_OBFUS('M'), INIT_CHAR_OBFUS('e'), INIT_CHAR_OBFUS('t'), INIT_CHAR_OBFUS('e'), INIT_CHAR_OBFUS('r'), INIT_CHAR_OBFUS('i'), INIT_CHAR_OBFUS('n'), INIT_CHAR_OBFUS('g'), INIT_CHAR_OBFUS('D'), INIT_CHAR_OBFUS('a'), INIT_CHAR_OBFUS('t'), INIT_CHAR_OBFUS('a'), INIT_CHAR_OBFUS('R'), INIT_CHAR_OBFUS('e'), INIT_CHAR_OBFUS('s'), INIT_CHAR_OBFUS('u'), INIT_CHAR_OBFUS('l'), INIT_CHAR_OBFUS('t'), INIT_CHAR_OBFUS('/'), INIT_CHAR_OBFUS('R'), INIT_CHAR_OBFUS('e'), INIT_CHAR_OBFUS('s'), INIT_CHAR_OBFUS('p'), INIT_CHAR_OBFUS('o'), INIT_CHAR_OBFUS('n'), INIT_CHAR_OBFUS('s'), INIT_CHAR_OBFUS('e'), INIT_CHAR_OBFUS('/'), INIT_CHAR_OBFUS('M'), INIT_CHAR_OBFUS('e'), INIT_CHAR_OBFUS('t'), INIT_CHAR_OBFUS('e'), INIT_CHAR_OBFUS('r'), INIT_CHAR_OBFUS('i'), INIT_CHAR_OBFUS('n'), INIT_CHAR_OBFUS('g'), INIT_CHAR_OBFUS('R'), INIT_CHAR_OBFUS('e'), INIT_CHAR_OBFUS('s'), INIT_CHAR_OBFUS('p'), INIT_CHAR_OBFUS('o'), INIT_CHAR_OBFUS('n'), INIT_CHAR_OBFUS('s'), INIT_CHAR_OBFUS('e'), INIT_CHAR_OBFUS('/'), INIT_CHAR_OBFUS('M'), INIT_CHAR_OBFUS('e'), INIT_CHAR_OBFUS('t'), INIT_CHAR_OBFUS('e'), INIT_CHAR_OBFUS('r'), INIT_CHAR_OBFUS('C'), INIT_CHAR_OBFUS('e'), INIT_CHAR_OBFUS('r'), INIT_CHAR_OBFUS('t'), INIT_CHAR_OBFUS('\0')};
const DRM_EXPORT_VAR DRM_DISCARDABLE DRM_ANSI_CONST_STRING g_dastrMeterDataMeterCertPath= CREATE_DRM_ANSI_STRING_EX( g_rgchMeterDataMeterCertPath, 61 );


/* "EncryptedData" */
DRM_STR_CONST DRM_EXPORT_VAR DRM_CHAR g_rgchMeterDataEncryptedDataPath [] = { INIT_CHAR_OBFUS('E'), INIT_CHAR_OBFUS('n'), INIT_CHAR_OBFUS('c'), INIT_CHAR_OBFUS('r'), INIT_CHAR_OBFUS('y'), INIT_CHAR_OBFUS('p'), INIT_CHAR_OBFUS('t'), INIT_CHAR_OBFUS('e'), INIT_CHAR_OBFUS('d'), INIT_CHAR_OBFUS('D'), INIT_CHAR_OBFUS('a'), INIT_CHAR_OBFUS('t'), INIT_CHAR_OBFUS('a'), INIT_CHAR_OBFUS('\0')};
const DRM_EXPORT_VAR DRM_DISCARDABLE DRM_ANSI_CONST_STRING g_dastrMeterDataEncryptedDataPath = CREATE_DRM_ANSI_STRING_EX( g_rgchMeterDataEncryptedDataPath, 13 );


/* "MID" */
DRM_STR_CONST DRM_EXPORT_VAR DRM_CHAR g_rgchMeterDataMIDPath [] = { INIT_CHAR_OBFUS('M'), INIT_CHAR_OBFUS('I'), INIT_CHAR_OBFUS('D'), INIT_CHAR_OBFUS('\0')};
const DRM_EXPORT_VAR DRM_DISCARDABLE DRM_ANSI_CONST_STRING g_dastrMeterDataMIDPath = CREATE_DRM_ANSI_STRING_EX( g_rgchMeterDataMIDPath, 3 );


/* "TID" */
DRM_STR_CONST DRM_EXPORT_VAR DRM_CHAR g_rgchMeterDataTIDPath [] = { INIT_CHAR_OBFUS('T'), INIT_CHAR_OBFUS('I'), INIT_CHAR_OBFUS('D'), INIT_CHAR_OBFUS('\0')};
const DRM_EXPORT_VAR DRM_DISCARDABLE DRM_ANSI_CONST_STRING g_dastrMeterDataTIDPath = CREATE_DRM_ANSI_STRING_EX( g_rgchMeterDataTIDPath, 3 );


/* "Records" */
DRM_STR_CONST DRM_EXPORT_VAR DRM_CHAR g_rgchMeterDataRecordsPath [] = { INIT_CHAR_OBFUS('R'), INIT_CHAR_OBFUS('e'), INIT_CHAR_OBFUS('c'), INIT_CHAR_OBFUS('o'), INIT_CHAR_OBFUS('r'), INIT_CHAR_OBFUS('d'), INIT_CHAR_OBFUS('s'), INIT_CHAR_OBFUS('\0')};
const DRM_EXPORT_VAR DRM_DISCARDABLE DRM_ANSI_CONST_STRING g_dastrMeterDataRecordsPath = CREATE_DRM_ANSI_STRING_EX( g_rgchMeterDataRecordsPath, 7 );


/* "KID" */
DRM_STR_CONST DRM_EXPORT_VAR DRM_CHAR g_rgchMeterDataKIDPath [] = { INIT_CHAR_OBFUS('K'), INIT_CHAR_OBFUS('I'), INIT_CHAR_OBFUS('D'), INIT_CHAR_OBFUS('\0')};
const DRM_EXPORT_VAR DRM_DISCARDABLE DRM_ANSI_CONST_STRING g_dastrMeterDataKIDPath = CREATE_DRM_ANSI_STRING_EX( g_rgchMeterDataKIDPath, 3 );

/* "KeyInfo/KeyName" */
DRM_STR_CONST DRM_EXPORT_VAR DRM_CHAR g_rgchMeterDataKeyNamePath [] = { INIT_CHAR_OBFUS('K'), INIT_CHAR_OBFUS('e'), INIT_CHAR_OBFUS('y'), INIT_CHAR_OBFUS('I'), INIT_CHAR_OBFUS('n'), INIT_CHAR_OBFUS('f'), INIT_CHAR_OBFUS('o'), INIT_CHAR_OBFUS('/'), INIT_CHAR_OBFUS('K'), INIT_CHAR_OBFUS('e'), INIT_CHAR_OBFUS('y'), INIT_CHAR_OBFUS('N'), INIT_CHAR_OBFUS('a'), INIT_CHAR_OBFUS('m'), INIT_CHAR_OBFUS('e'), INIT_CHAR_OBFUS('\0')};
const DRM_EXPORT_VAR DRM_DISCARDABLE DRM_ANSI_CONST_STRING g_dastrMeterDataKeyNamePath = CREATE_DRM_ANSI_STRING_EX( g_rgchMeterDataKeyNamePath, 15 );

/* "TIDKey" */
DRM_STR_CONST DRM_EXPORT_VAR DRM_CHAR g_rgchMeterDataKeyNameValue [] = { INIT_CHAR_OBFUS('T'), INIT_CHAR_OBFUS('I'), INIT_CHAR_OBFUS('D'), INIT_CHAR_OBFUS('K'), INIT_CHAR_OBFUS('e'), INIT_CHAR_OBFUS('y'), INIT_CHAR_OBFUS('\0')};
const DRM_EXPORT_VAR DRM_DISCARDABLE DRM_ANSI_CONST_STRING g_dastrMeterDataKeyNameValue = CREATE_DRM_ANSI_STRING_EX( g_rgchMeterDataKeyNameValue, 6 );


/*
** ------------------------------------------------------------
** XML strings used in the construction of meter cert challenge
** ------------------------------------------------------------
*/
/* "GetMeteringCertificate" */
DRM_STR_CONST DRM_EXPORT_VAR DRM_CHAR g_rgchMeterCertRootTag [] = { INIT_CHAR_OBFUS('G'), INIT_CHAR_OBFUS('e'), INIT_CHAR_OBFUS('t'), INIT_CHAR_OBFUS('M'), INIT_CHAR_OBFUS('e'), INIT_CHAR_OBFUS('t'), INIT_CHAR_OBFUS('e'), INIT_CHAR_OBFUS('r'), INIT_CHAR_OBFUS('i'), INIT_CHAR_OBFUS('n'), INIT_CHAR_OBFUS('g'), INIT_CHAR_OBFUS('C'), INIT_CHAR_OBFUS('e'), INIT_CHAR_OBFUS('r'), INIT_CHAR_OBFUS('t'), INIT_CHAR_OBFUS('i'), INIT_CHAR_OBFUS('f'), INIT_CHAR_OBFUS('i'), INIT_CHAR_OBFUS('c'), INIT_CHAR_OBFUS('a'), INIT_CHAR_OBFUS('t'), INIT_CHAR_OBFUS('e'), INIT_CHAR_OBFUS('\0')};
const DRM_EXPORT_VAR DRM_DISCARDABLE DRM_ANSI_CONST_STRING g_dastrMeterCertRootTag = CREATE_DRM_ANSI_STRING_EX( g_rgchMeterCertRootTag, 22 );

/* "challenge" */
DRM_STR_CONST DRM_EXPORT_VAR DRM_CHAR g_rgchMeteringChallenge1Tag [] = { INIT_CHAR_OBFUS('c'), INIT_CHAR_OBFUS('h'), INIT_CHAR_OBFUS('a'), INIT_CHAR_OBFUS('l'), INIT_CHAR_OBFUS('l'), INIT_CHAR_OBFUS('e'), INIT_CHAR_OBFUS('n'), INIT_CHAR_OBFUS('g'), INIT_CHAR_OBFUS('e'), INIT_CHAR_OBFUS('\0')};
const DRM_EXPORT_VAR DRM_DISCARDABLE DRM_ANSI_CONST_STRING g_dastrMeterCertChallenge1Tag = CREATE_DRM_ANSI_STRING_EX( g_rgchMeteringChallenge1Tag, 9 );

/* "Challenge" */
DRM_STR_CONST DRM_EXPORT_VAR DRM_CHAR g_rgchMeteringChallenge2Tag [] = { INIT_CHAR_OBFUS('C'), INIT_CHAR_OBFUS('h'), INIT_CHAR_OBFUS('a'), INIT_CHAR_OBFUS('l'), INIT_CHAR_OBFUS('l'), INIT_CHAR_OBFUS('e'), INIT_CHAR_OBFUS('n'), INIT_CHAR_OBFUS('g'), INIT_CHAR_OBFUS('e'), INIT_CHAR_OBFUS('\0')};
const DRM_EXPORT_VAR DRM_DISCARDABLE DRM_ANSI_CONST_STRING g_dastrMeterCertChallenge2Tag = CREATE_DRM_ANSI_STRING_EX( g_rgchMeteringChallenge2Tag, 9 );

/* "Version" */
DRM_STR_CONST DRM_EXPORT_VAR DRM_CHAR g_rgchMeterCertVersionTag [] = { INIT_CHAR_OBFUS('V'), INIT_CHAR_OBFUS('e'), INIT_CHAR_OBFUS('r'), INIT_CHAR_OBFUS('s'), INIT_CHAR_OBFUS('i'), INIT_CHAR_OBFUS('o'), INIT_CHAR_OBFUS('n'), INIT_CHAR_OBFUS('\0')};
const DRM_EXPORT_VAR DRM_DISCARDABLE DRM_ANSI_CONST_STRING g_dastrMeterCertVersionTag = CREATE_DRM_ANSI_STRING_EX( g_rgchMeterCertVersionTag, 7 );

/* "MID" */
DRM_STR_CONST DRM_EXPORT_VAR DRM_CHAR g_rgchMeteringMIDTag [] = { INIT_CHAR_OBFUS('M'), INIT_CHAR_OBFUS('I'), INIT_CHAR_OBFUS('D'), INIT_CHAR_OBFUS('\0')};
const DRM_EXPORT_VAR DRM_DISCARDABLE DRM_ANSI_CONST_STRING g_dastrMeterCertMIDTag = CREATE_DRM_ANSI_STRING_EX( g_rgchMeteringMIDTag, 3 );

/* "SupportsMeterCertSignature" */
DRM_STR_CONST DRM_EXPORT_VAR DRM_CHAR g_rgchSupportsMeterCertSignatureTag [] = { INIT_CHAR_OBFUS('S'), INIT_CHAR_OBFUS('u'), INIT_CHAR_OBFUS('p'), INIT_CHAR_OBFUS('p'), INIT_CHAR_OBFUS('o'), INIT_CHAR_OBFUS('r'), INIT_CHAR_OBFUS('t'), INIT_CHAR_OBFUS('s'), INIT_CHAR_OBFUS('M'), INIT_CHAR_OBFUS('e'), INIT_CHAR_OBFUS('t'), INIT_CHAR_OBFUS('e'), INIT_CHAR_OBFUS('r'), INIT_CHAR_OBFUS('C'), INIT_CHAR_OBFUS('e'), INIT_CHAR_OBFUS('r'), INIT_CHAR_OBFUS('t'), INIT_CHAR_OBFUS('S'), INIT_CHAR_OBFUS('i'), INIT_CHAR_OBFUS('g'), INIT_CHAR_OBFUS('n'), INIT_CHAR_OBFUS('a'), INIT_CHAR_OBFUS('t'), INIT_CHAR_OBFUS('u'), INIT_CHAR_OBFUS('r'), INIT_CHAR_OBFUS('e'), INIT_CHAR_OBFUS('\0')};
const DRM_EXPORT_VAR DRM_DISCARDABLE DRM_ANSI_CONST_STRING g_dastrSupportsMeterCertSignatureTag = CREATE_DRM_ANSI_STRING_EX( g_rgchSupportsMeterCertSignatureTag, 26 );

/* "true" */
DRM_STR_CONST DRM_EXPORT_VAR DRM_CHAR g_rgchTrueValue [] = { INIT_CHAR_OBFUS('t'), INIT_CHAR_OBFUS('r'), INIT_CHAR_OBFUS('u'), INIT_CHAR_OBFUS('e'), INIT_CHAR_OBFUS('\0')};
const DRM_EXPORT_VAR DRM_DISCARDABLE DRM_ANSI_CONST_STRING g_dastrTrueValue = CREATE_DRM_ANSI_STRING_EX( g_rgchTrueValue, 4 );

/* "CustomData" */
DRM_STR_CONST DRM_EXPORT_VAR DRM_CHAR g_rgchMeterCertCustomDataTag [] = { INIT_CHAR_OBFUS('C'), INIT_CHAR_OBFUS('u'), INIT_CHAR_OBFUS('s'), INIT_CHAR_OBFUS('t'), INIT_CHAR_OBFUS('o'), INIT_CHAR_OBFUS('m'), INIT_CHAR_OBFUS('D'), INIT_CHAR_OBFUS('a'), INIT_CHAR_OBFUS('t'), INIT_CHAR_OBFUS('a'), INIT_CHAR_OBFUS('\0')};
const DRM_EXPORT_VAR DRM_DISCARDABLE DRM_ANSI_CONST_STRING g_dastrMeterCertCustomDataTag = CREATE_DRM_ANSI_STRING_EX( g_rgchMeterCertCustomDataTag, 10 );


/* "xmlns" */
DRM_STR_CONST DRM_EXPORT_VAR DRM_CHAR g_rgchMeterCertRootAttribName [] = { INIT_CHAR_OBFUS('x'), INIT_CHAR_OBFUS('m'), INIT_CHAR_OBFUS('l'), INIT_CHAR_OBFUS('n'), INIT_CHAR_OBFUS('s'), INIT_CHAR_OBFUS('\0')};
const DRM_EXPORT_VAR DRM_DISCARDABLE DRM_ANSI_CONST_STRING g_dastrMeterCertRootAttribName = CREATE_DRM_ANSI_STRING_EX( g_rgchMeterCertRootAttribName, 5 );

/* "http://schemas.microsoft.com/DRM/2007/03/protocols" */
DRM_STR_CONST DRM_EXPORT_VAR DRM_CHAR g_rgchMeterCertRootAttribValue [] = { INIT_CHAR_OBFUS('h'), INIT_CHAR_OBFUS('t'), INIT_CHAR_OBFUS('t'), INIT_CHAR_OBFUS('p'), INIT_CHAR_OBFUS(':'), INIT_CHAR_OBFUS('/'), INIT_CHAR_OBFUS('/'), INIT_CHAR_OBFUS('s'), INIT_CHAR_OBFUS('c'), INIT_CHAR_OBFUS('h'), INIT_CHAR_OBFUS('e'), INIT_CHAR_OBFUS('m'), INIT_CHAR_OBFUS('a'), INIT_CHAR_OBFUS('s'), INIT_CHAR_OBFUS('.'), INIT_CHAR_OBFUS('m'), INIT_CHAR_OBFUS('i'), INIT_CHAR_OBFUS('c'), INIT_CHAR_OBFUS('r'), INIT_CHAR_OBFUS('o'), INIT_CHAR_OBFUS('s'), INIT_CHAR_OBFUS('o'), INIT_CHAR_OBFUS('f'), INIT_CHAR_OBFUS('t'), INIT_CHAR_OBFUS('.'), INIT_CHAR_OBFUS('c'), INIT_CHAR_OBFUS('o'), INIT_CHAR_OBFUS('m'), INIT_CHAR_OBFUS('/'), INIT_CHAR_OBFUS('D'), INIT_CHAR_OBFUS('R'), INIT_CHAR_OBFUS('M'), INIT_CHAR_OBFUS('/'), INIT_CHAR_OBFUS('2'), INIT_CHAR_OBFUS('0'), INIT_CHAR_OBFUS('0'), INIT_CHAR_OBFUS('7'), INIT_CHAR_OBFUS('/'), INIT_CHAR_OBFUS('0'), INIT_CHAR_OBFUS('3'), INIT_CHAR_OBFUS('/'), INIT_CHAR_OBFUS('p'), INIT_CHAR_OBFUS('r'), INIT_CHAR_OBFUS('o'), INIT_CHAR_OBFUS('t'), INIT_CHAR_OBFUS('o'), INIT_CHAR_OBFUS('c'), INIT_CHAR_OBFUS('o'), INIT_CHAR_OBFUS('l'), INIT_CHAR_OBFUS('s'), INIT_CHAR_OBFUS('\0')};
const DRM_EXPORT_VAR DRM_DISCARDABLE DRM_ANSI_CONST_STRING g_dastrMeterCertRootAttribValue = CREATE_DRM_ANSI_STRING_EX( g_rgchMeterCertRootAttribValue, 50 );



/* "1" */
DRM_STR_CONST DRM_EXPORT_VAR DRM_CHAR g_rgchMeterCertVersion [] = { INIT_CHAR_OBFUS('1'), INIT_CHAR_OBFUS('\0')};
const DRM_EXPORT_VAR DRM_DISCARDABLE DRM_ANSI_CONST_STRING g_dastrMeterCertVersion = CREATE_DRM_ANSI_STRING_EX( g_rgchMeterCertVersion, 1 );



/*
** ------------------------------------------------------
** XML strings used in the parsing of meter cert response
** ------------------------------------------------------
*/

/* "GetMeteringCertificateResult/Response/MeteringCertificateResponse/MeterCert" */
DRM_STR_CONST DRM_EXPORT_VAR DRM_CHAR g_rgchMeterCertMeterCertPath [] = { INIT_CHAR_OBFUS('G'), INIT_CHAR_OBFUS('e'), INIT_CHAR_OBFUS('t'), INIT_CHAR_OBFUS('M'), INIT_CHAR_OBFUS('e'), INIT_CHAR_OBFUS('t'), INIT_CHAR_OBFUS('e'), INIT_CHAR_OBFUS('r'), INIT_CHAR_OBFUS('i'), INIT_CHAR_OBFUS('n'), INIT_CHAR_OBFUS('g'), INIT_CHAR_OBFUS('C'), INIT_CHAR_OBFUS('e'), INIT_CHAR_OBFUS('r'), INIT_CHAR_OBFUS('t'), INIT_CHAR_OBFUS('i'), INIT_CHAR_OBFUS('f'), INIT_CHAR_OBFUS('i'), INIT_CHAR_OBFUS('c'), INIT_CHAR_OBFUS('a'), INIT_CHAR_OBFUS('t'), INIT_CHAR_OBFUS('e'), INIT_CHAR_OBFUS('R'), INIT_CHAR_OBFUS('e'), INIT_CHAR_OBFUS('s'), INIT_CHAR_OBFUS('u'), INIT_CHAR_OBFUS('l'), INIT_CHAR_OBFUS('t'), INIT_CHAR_OBFUS('/'), INIT_CHAR_OBFUS('R'), INIT_CHAR_OBFUS('e'), INIT_CHAR_OBFUS('s'), INIT_CHAR_OBFUS('p'), INIT_CHAR_OBFUS('o'), INIT_CHAR_OBFUS('n'), INIT_CHAR_OBFUS('s'), INIT_CHAR_OBFUS('e'), INIT_CHAR_OBFUS('/'), INIT_CHAR_OBFUS('M'), INIT_CHAR_OBFUS('e'), INIT_CHAR_OBFUS('t'), INIT_CHAR_OBFUS('e'), INIT_CHAR_OBFUS('r'), INIT_CHAR_OBFUS('i'), INIT_CHAR_OBFUS('n'), INIT_CHAR_OBFUS('g'), INIT_CHAR_OBFUS('C'), INIT_CHAR_OBFUS('e'), INIT_CHAR_OBFUS('r'), INIT_CHAR_OBFUS('t'), INIT_CHAR_OBFUS('i'), INIT_CHAR_OBFUS('f'), INIT_CHAR_OBFUS('i'), INIT_CHAR_OBFUS('c'), INIT_CHAR_OBFUS('a'), INIT_CHAR_OBFUS('t'), INIT_CHAR_OBFUS('e'), INIT_CHAR_OBFUS('R'), INIT_CHAR_OBFUS('e'), INIT_CHAR_OBFUS('s'), INIT_CHAR_OBFUS('p'), INIT_CHAR_OBFUS('o'), INIT_CHAR_OBFUS('n'), INIT_CHAR_OBFUS('s'), INIT_CHAR_OBFUS('e'), INIT_CHAR_OBFUS('/'), INIT_CHAR_OBFUS('M'), INIT_CHAR_OBFUS('e'), INIT_CHAR_OBFUS('t'), INIT_CHAR_OBFUS('e'), INIT_CHAR_OBFUS('r'), INIT_CHAR_OBFUS('C'), INIT_CHAR_OBFUS('e'), INIT_CHAR_OBFUS('r'), INIT_CHAR_OBFUS('t'), INIT_CHAR_OBFUS('\0')};
const DRM_EXPORT_VAR DRM_DISCARDABLE DRM_ANSI_CONST_STRING g_dastrMeterCertMeterCertPath= CREATE_DRM_ANSI_STRING_EX( g_rgchMeterCertMeterCertPath, 75 );


/*
** -----------------------------
** Additional metering constants
** -----------------------------
*/
DRM_STR_CONST                DRM_WCHAR      g_rgwchTagMeterCert                          [] =  { INIT_WCHAR_OBFUS('M'),  INIT_WCHAR_OBFUS('E'),  INIT_WCHAR_OBFUS('T'),  INIT_WCHAR_OBFUS('E'),  INIT_WCHAR_OBFUS('R'),  INIT_WCHAR_OBFUS('C'),  INIT_WCHAR_OBFUS('E'),  INIT_WCHAR_OBFUS('R'),  INIT_WCHAR_OBFUS('T'),  INIT_WCHAR_OBFUS('\0')};
const DRM_DISCARDABLE DRM_CONST_STRING      g_dstrTagMeterCert                          = CREATE_DRM_STRING( g_rgwchTagMeterCert );

const DRM_DISCARDABLE PRIVKEY g_privkeyMeterCert =
{
    0xB6, 0x18, 0xCA, 0x39, 0xCF, 0xD6, 0x09, 0x8E,
    0xCC, 0x1A, 0x43, 0xA6, 0x25, 0xD4, 0xFF, 0xC0,
    0xE4, 0x52, 0x23, 0x5E
};

const DRM_DISCARDABLE PUBKEY g_pubkeyMeteringCertVerify =
{
    0xAE, 0xF2, 0x91, 0xD5, 0xDA, 0xBE, 0x13, 0x37,
    0x46, 0x0F, 0xC3, 0x43, 0xD8, 0x88, 0x64, 0x9F,
    0x43, 0x8F, 0x12, 0x85, 0x99, 0x64, 0xA0, 0xB0,
    0x82, 0x27, 0x69, 0xED, 0x8E, 0x52, 0x1D, 0x1F,
    0x8D, 0x14, 0x92, 0x5A, 0xCD, 0xD3, 0xD6, 0x7C
};

const DRM_DISCARDABLE PUBKEY g_pubkeyRootMeteringCert = /* "pub" */
{
    0x45, 0xB1, 0xA7, 0xE1, 0x90, 0x81, 0x98, 0x37,
    0x00, 0xCC, 0x89, 0xA7, 0x57, 0x24, 0x72, 0xB9,
    0xC1, 0x29, 0xA3, 0x62, 0xD9, 0x55, 0x74, 0x04,
    0x02, 0x7D, 0x6E, 0x69, 0x79, 0xE9, 0x6A, 0xD9,
    0x7A, 0x92, 0xE4, 0xF3, 0x4B, 0x6B, 0x42, 0x6C
};

EXIT_PK_NAMESPACE_CODE;

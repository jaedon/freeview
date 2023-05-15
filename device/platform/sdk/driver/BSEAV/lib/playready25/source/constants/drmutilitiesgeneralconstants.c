/**@@@+++@@@@******************************************************************
**
** Microsoft (r) PlayReady (r)
** Copyright (c) Microsoft Corporation. All rights reserved.
**
***@@@---@@@@******************************************************************
*/

#include <drmtypes.h>
#include <drmutilitiesgeneralconstants.h>

/*
** Utilities strings.
*/

ENTER_PK_NAMESPACE_CODE;

DRM_STR_CONST DRM_WCHAR g_rgwchEncodedAmpersand[]               = { INIT_WCHAR_OBFUS('&'), INIT_WCHAR_OBFUS('a'), INIT_WCHAR_OBFUS('m'), INIT_WCHAR_OBFUS('p'), INIT_WCHAR_OBFUS(';'), INIT_WCHAR_OBFUS('\0')};
DRM_STR_CONST DRM_WCHAR g_rgwchEncodedQuote[]                   = { INIT_WCHAR_OBFUS('&'), INIT_WCHAR_OBFUS('q'), INIT_WCHAR_OBFUS('u'), INIT_WCHAR_OBFUS('o'), INIT_WCHAR_OBFUS('t'), INIT_WCHAR_OBFUS(';'), INIT_WCHAR_OBFUS('\0')};
DRM_STR_CONST DRM_WCHAR g_rgwchEncodedLesserThan[]              = { INIT_WCHAR_OBFUS('&'), INIT_WCHAR_OBFUS('l'), INIT_WCHAR_OBFUS('t'), INIT_WCHAR_OBFUS(';'), INIT_WCHAR_OBFUS('\0')};
DRM_STR_CONST DRM_WCHAR g_rgwchEncodedGreaterThan[]             = { INIT_WCHAR_OBFUS('&'), INIT_WCHAR_OBFUS('g'), INIT_WCHAR_OBFUS('t'), INIT_WCHAR_OBFUS(';'), INIT_WCHAR_OBFUS('\0')};
DRM_STR_CONST DRM_WCHAR g_rgwchEncodedCarriageReturnNewLine[]   = { INIT_WCHAR_OBFUS('\n'), INIT_WCHAR_OBFUS('\0')};
DRM_STR_CONST DRM_WCHAR g_rgwchDecodedCarriageReturnNewLine[]   = { INIT_WCHAR_OBFUS('\r'), INIT_WCHAR_OBFUS('\n'), INIT_WCHAR_OBFUS('\0')};
DRM_STR_CONST DRM_WCHAR g_rgwchEncodedCarriageReturn[]          = { INIT_WCHAR_OBFUS('\n'), INIT_WCHAR_OBFUS('\0')};
DRM_STR_CONST DRM_WCHAR g_rgwchDecodedCarriageReturn[]          = { INIT_WCHAR_OBFUS('\r'), INIT_WCHAR_OBFUS('\0')};

const DRM_CONST_STRING g_dstrEncodedAmpersand               = CREATE_DRM_STRING( g_rgwchEncodedAmpersand );
const DRM_CONST_STRING g_dstrEncodedQuote                   = CREATE_DRM_STRING( g_rgwchEncodedQuote );
const DRM_CONST_STRING g_dstrEncodedLesserThan              = CREATE_DRM_STRING( g_rgwchEncodedLesserThan );
const DRM_CONST_STRING g_dstrEncodedGreaterThan             = CREATE_DRM_STRING( g_rgwchEncodedGreaterThan );
const DRM_CONST_STRING g_dstrEncodedCarriageReturnNewLine   = CREATE_DRM_STRING( g_rgwchEncodedCarriageReturnNewLine );
const DRM_CONST_STRING g_dstrDecodedCarriageReturnNewLine   = CREATE_DRM_STRING( g_rgwchDecodedCarriageReturnNewLine );
const DRM_CONST_STRING g_dstrEncodedCarriageReturn          = CREATE_DRM_STRING( g_rgwchEncodedCarriageReturn );
const DRM_CONST_STRING g_dstrDecodedCarriageReturn          = CREATE_DRM_STRING( g_rgwchDecodedCarriageReturn );

DRM_STR_CONST DRM_CHAR g_rgchEncodedAmpersand[]             = { INIT_CHAR_OBFUS('&'), INIT_CHAR_OBFUS('a'), INIT_CHAR_OBFUS('m'), INIT_CHAR_OBFUS('p'), INIT_CHAR_OBFUS(';'), INIT_CHAR_OBFUS('\0')};
DRM_STR_CONST DRM_CHAR g_rgchEncodedQuote[]                 = { INIT_CHAR_OBFUS('&'), INIT_CHAR_OBFUS('q'), INIT_CHAR_OBFUS('u'), INIT_CHAR_OBFUS('o'), INIT_CHAR_OBFUS('t'), INIT_CHAR_OBFUS(';'), INIT_CHAR_OBFUS('\0')};
DRM_STR_CONST DRM_CHAR g_rgchEncodedLesserThan[]            = { INIT_CHAR_OBFUS('&'), INIT_CHAR_OBFUS('l'), INIT_CHAR_OBFUS('t'), INIT_CHAR_OBFUS(';'), INIT_CHAR_OBFUS('\0')};
DRM_STR_CONST DRM_CHAR g_rgchEncodedGreaterThan[]           = { INIT_CHAR_OBFUS('&'), INIT_CHAR_OBFUS('g'), INIT_CHAR_OBFUS('t'), INIT_CHAR_OBFUS(';'), INIT_CHAR_OBFUS('\0')};
DRM_STR_CONST DRM_CHAR g_rgchEncodedCarriageReturnNewLine[] = { INIT_CHAR_OBFUS('\n'), INIT_CHAR_OBFUS('\0')};
DRM_STR_CONST DRM_CHAR g_rgchDecodedCarriageReturnNewLine[] = { INIT_CHAR_OBFUS('\r'), INIT_CHAR_OBFUS('\n'), INIT_CHAR_OBFUS('\0')};
DRM_STR_CONST DRM_CHAR g_rgchEncodedCarriageReturn[]        = { INIT_CHAR_OBFUS('\n'), INIT_CHAR_OBFUS('\0')};
DRM_STR_CONST DRM_CHAR g_rgchDecodedCarriageReturn[]        = { INIT_CHAR_OBFUS('\r'), INIT_CHAR_OBFUS('\0')};

const DRM_ANSI_CONST_STRING g_dastrEncodedAmpersand             = CREATE_DRM_ANSI_STRING_EX( g_rgchEncodedAmpersand, 5 );
const DRM_ANSI_CONST_STRING g_dastrEncodedQuote                 = CREATE_DRM_ANSI_STRING_EX( g_rgchEncodedQuote, 6 );
const DRM_ANSI_CONST_STRING g_dastrEncodedLesserThan            = CREATE_DRM_ANSI_STRING_EX( g_rgchEncodedLesserThan, 4 );
const DRM_ANSI_CONST_STRING g_dastrEncodedGreaterThan           = CREATE_DRM_ANSI_STRING_EX( g_rgchEncodedGreaterThan, 4 );
const DRM_ANSI_CONST_STRING g_dastrEncodedCarriageReturnNewLine = CREATE_DRM_ANSI_STRING_EX( g_rgchEncodedCarriageReturnNewLine, 1 );
const DRM_ANSI_CONST_STRING g_dastrDecodedCarriageReturnNewLine = CREATE_DRM_ANSI_STRING_EX( g_rgchDecodedCarriageReturnNewLine, 2 );
const DRM_ANSI_CONST_STRING g_dastrEncodedCarriageReturn        = CREATE_DRM_ANSI_STRING_EX( g_rgchEncodedCarriageReturn, 1 );
const DRM_ANSI_CONST_STRING g_dastrDecodedCarriageReturn        = CREATE_DRM_ANSI_STRING_EX( g_rgchDecodedCarriageReturn, 1 );


EXIT_PK_NAMESPACE_CODE;

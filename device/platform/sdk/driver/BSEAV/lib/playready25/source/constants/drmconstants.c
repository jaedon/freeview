/**@@@+++@@@@******************************************************************
**
** Microsoft (r) PlayReady (r)
** Copyright (c) Microsoft Corporation. All rights reserved.
**
***@@@---@@@@******************************************************************
*/

#include <drmconstants.h>
#include <drmlicense.h>
#include <drmbytemanip.h>


ENTER_PK_NAMESPACE_CODE;

/* License constants */
const DRM_DISCARDABLE PUBKEY g_pubkeyLicenseServer =
{
    {
         0x52, 0xF5, 0x5A, 0x8A, 0x8A, 0x7F, 0xEA, 0x57,
         0x55, 0x24, 0x93, 0xA9, 0x8C, 0x11, 0x06, 0x58,
         0x37, 0xAD, 0x9F, 0x66, 0x68, 0xC1, 0x1C, 0x16,
         0x2D, 0x4A, 0x57, 0xBA, 0x7B, 0x6C, 0xA4, 0x24,
         0x14, 0x2B, 0x4E, 0x2E, 0x89, 0xF1, 0x39, 0x4E
    }
};

const DRM_DISCARDABLE DRM_LID g_rgbSecStoreGlobalName =
{
    'g', 'l', 'o', 'b', 'a', 'l',
    '.', 's', 'e', 'c', 's', 't',
    'a', 't', 'e', '\0'
};

/* Character constants */
const                DRM_WCHAR             g_wchNull                                   = WCHAR_CAST('\0');
const                DRM_WCHAR             g_wchMinus                                  = WCHAR_CAST('-');
const                DRM_WCHAR             g_wchPlus                                   = WCHAR_CAST('+');
const                DRM_WCHAR             g_wchForwardSlash                           = WCHAR_CAST('/');
const                DRM_WCHAR             g_wchColon                                  = WCHAR_CAST(':');
const                DRM_WCHAR             g_wchComma                                  = WCHAR_CAST(',');
const                DRM_WCHAR             g_wchQuote                                  = WCHAR_CAST('\"');
const                DRM_WCHAR             g_wchSingleQuote                            = WCHAR_CAST('\'');
const                DRM_WCHAR             g_wchNewLine                                = WCHAR_CAST('\n');
const                DRM_WCHAR             g_wchBackSlash                              = WCHAR_CAST('\\');
const                DRM_WCHAR             g_wch0                                      = WCHAR_CAST('0');
const                DRM_WCHAR             g_wch1                                      = WCHAR_CAST('1');
const                DRM_WCHAR             g_wch2                                      = WCHAR_CAST('2');
const                DRM_WCHAR             g_wch9                                      = WCHAR_CAST('9');
const                DRM_WCHAR             g_wcha                                      = WCHAR_CAST('a');
const                DRM_WCHAR             g_wchd                                      = WCHAR_CAST('d');
const                DRM_WCHAR             g_wchf                                      = WCHAR_CAST('f');
const                DRM_WCHAR             g_wchh                                      = WCHAR_CAST('h');
const                DRM_WCHAR             g_wchm                                      = WCHAR_CAST('m');
const                DRM_WCHAR             g_wchn                                      = WCHAR_CAST('n');
const                DRM_WCHAR             g_wchs                                      = WCHAR_CAST('s');
const                DRM_WCHAR             g_wchx                                      = WCHAR_CAST('x');
const                DRM_WCHAR             g_wchy                                      = WCHAR_CAST('y');
const                DRM_WCHAR             g_wchz                                      = WCHAR_CAST('z');
const                DRM_WCHAR             g_wchA                                      = WCHAR_CAST('A');
const                DRM_WCHAR             g_wchF                                      = WCHAR_CAST('F');
const                DRM_WCHAR             g_wchG                                      = WCHAR_CAST('G');
const                DRM_WCHAR             g_wchM                                      = WCHAR_CAST('M');
const                DRM_WCHAR             g_wchT                                      = WCHAR_CAST('T');
const                DRM_WCHAR             g_wchX                                      = WCHAR_CAST('X');
const                DRM_WCHAR             g_wchZ                                      = WCHAR_CAST('Z');
const                DRM_WCHAR             g_wchUnderscore                             = WCHAR_CAST('_');
const                DRM_WCHAR             g_wchPeriod                                 = WCHAR_CAST('.');
const                DRM_WCHAR             g_wchQuestionMark                           = WCHAR_CAST('?');
const                DRM_WCHAR             g_wchExclamationMark                        = WCHAR_CAST('!');
const                DRM_WCHAR             g_wchOpenParen                              = WCHAR_CAST('(');
const                DRM_WCHAR             g_wchCloseParen                             = WCHAR_CAST(')');
const                DRM_WCHAR             g_wchPound                                  = WCHAR_CAST('#');
const                DRM_WCHAR             g_wchSpace                                  = WCHAR_CAST(' ');
const                DRM_WCHAR             g_wchTab                                    = WCHAR_CAST('\x9');
const                DRM_WCHAR             g_wchLineFeed                               = WCHAR_CAST('\xA');
const                DRM_WCHAR             g_wchVerticalTab                            = WCHAR_CAST('\xB');
const                DRM_WCHAR             g_wchFormFeed                               = WCHAR_CAST('\xC');
const                DRM_WCHAR             g_wchCarriageReturn                         = WCHAR_CAST('\xD');
const                DRM_WCHAR             g_wchEqual                                  = WCHAR_CAST('=');
const                DRM_WCHAR             g_wchOpenCurly                              = WCHAR_CAST('{');
const                DRM_WCHAR             g_wchCloseCurly                             = WCHAR_CAST('}');
const                DRM_WCHAR             g_wchLessThan                               = WCHAR_CAST('<');
const                DRM_WCHAR             g_wchGreaterThan                            = WCHAR_CAST('>');
const                DRM_WCHAR             g_wchLeftBracket                            = WCHAR_CAST('[');
const                DRM_WCHAR             g_wchRightBracket                           = WCHAR_CAST(']');
const                DRM_WCHAR             g_wchAsterisk                               = WCHAR_CAST('*');
const                DRM_WCHAR             g_wchPercent                                = WCHAR_CAST('%');
const                DRM_WCHAR             g_wchSemiColon                              = WCHAR_CAST(';');
const                DRM_WCHAR             g_wchAmpersand                              = WCHAR_CAST('&');
const                DRM_WCHAR             g_wchPipe                                   = WCHAR_CAST('|');

/* Character constants - ANSI */
const                DRM_CHAR             g_chForwardSlash                             = '/';
const                DRM_CHAR             g_chPeriod                                   = '.';
const                DRM_CHAR             g_chNull                                     = '\0';
const                DRM_CHAR             g_chMinus                                    = '-';
const                DRM_CHAR             g_chPlus                                     = '+';
const                DRM_CHAR             g_ch0                                        = '0';
const                DRM_CHAR             g_ch9                                        = '9';
const                DRM_CHAR             g_cha                                        = 'a';
const                DRM_CHAR             g_chA                                        = 'A';

/* Misc strings shared across disparate functional areas */
DRM_STR_CONST        DRM_CHAR       g_rgchTagSLK                                 [] =  { INIT_CHAR_OBFUS('S'), INIT_CHAR_OBFUS('L'), INIT_CHAR_OBFUS('K'), INIT_CHAR_OBFUS('\0')};
DRM_STR_CONST        DRM_CHAR       g_rgchTagSLKID                               [] =  { INIT_CHAR_OBFUS('S'), INIT_CHAR_OBFUS('L'), INIT_CHAR_OBFUS('K'), INIT_CHAR_OBFUS('I'), INIT_CHAR_OBFUS('D'), INIT_CHAR_OBFUS('\0')};
DRM_STR_CONST        DRM_CHAR       g_rgchTagSLKDATA                             [] =  { INIT_CHAR_OBFUS('S'), INIT_CHAR_OBFUS('L'), INIT_CHAR_OBFUS('K'), INIT_CHAR_OBFUS('D'), INIT_CHAR_OBFUS('A'), INIT_CHAR_OBFUS('T'), INIT_CHAR_OBFUS('A'), INIT_CHAR_OBFUS('\0')};
DRM_STR_CONST        DRM_WCHAR      g_rgwchXPathSymSig                           [] =  { INIT_WCHAR_OBFUS('L'),  INIT_WCHAR_OBFUS('I'),  INIT_WCHAR_OBFUS('C'),  INIT_WCHAR_OBFUS('E'),  INIT_WCHAR_OBFUS('N'),  INIT_WCHAR_OBFUS('S'),  INIT_WCHAR_OBFUS('O'),  INIT_WCHAR_OBFUS('R'),  INIT_WCHAR_OBFUS('I'),  INIT_WCHAR_OBFUS('N'),  INIT_WCHAR_OBFUS('F'),  INIT_WCHAR_OBFUS('O'),  INIT_WCHAR_OBFUS('/'),  INIT_WCHAR_OBFUS('S'),  INIT_WCHAR_OBFUS('Y'),  INIT_WCHAR_OBFUS('M'),  INIT_WCHAR_OBFUS('S'),  INIT_WCHAR_OBFUS('I'),  INIT_WCHAR_OBFUS('G'),  INIT_WCHAR_OBFUS('N'),  INIT_WCHAR_OBFUS('A'),  INIT_WCHAR_OBFUS('T'),  INIT_WCHAR_OBFUS('U'),  INIT_WCHAR_OBFUS('R'),  INIT_WCHAR_OBFUS('E'),  INIT_WCHAR_OBFUS('\0')};
DRM_STR_CONST        DRM_CHAR       g_rgchLicenseRespTag                         [] =  { INIT_CHAR_OBFUS('L'), INIT_CHAR_OBFUS('I'), INIT_CHAR_OBFUS('C'), INIT_CHAR_OBFUS('E'), INIT_CHAR_OBFUS('N'), INIT_CHAR_OBFUS('S'), INIT_CHAR_OBFUS('E'), INIT_CHAR_OBFUS('R'), INIT_CHAR_OBFUS('E'), INIT_CHAR_OBFUS('S'), INIT_CHAR_OBFUS('P'), INIT_CHAR_OBFUS('O'), INIT_CHAR_OBFUS('N'), INIT_CHAR_OBFUS('S'), INIT_CHAR_OBFUS('E'), INIT_CHAR_OBFUS('\0')}; /* ODD count */
DRM_STR_CONST        DRM_WCHAR      g_rgwchGUID                                  [] =  { INIT_WCHAR_OBFUS('G'), INIT_WCHAR_OBFUS('U'), INIT_WCHAR_OBFUS('I'), INIT_WCHAR_OBFUS('D'), INIT_WCHAR_OBFUS('\0') };
DRM_STR_CONST        DRM_WCHAR      g_rgwchFlag                                  [] =  { INIT_WCHAR_OBFUS('F'),  INIT_WCHAR_OBFUS('L'),  INIT_WCHAR_OBFUS('A'),  INIT_WCHAR_OBFUS('G'),  INIT_WCHAR_OBFUS('\0')};
DRM_STR_CONST        DRM_WCHAR      g_rgwchSecClockNotSet                        [] =  { INIT_WCHAR_OBFUS('D'),  INIT_WCHAR_OBFUS('R'),  INIT_WCHAR_OBFUS('M'),  INIT_WCHAR_OBFUS('_'),  INIT_WCHAR_OBFUS('C'),  INIT_WCHAR_OBFUS('L'),  INIT_WCHAR_OBFUS('K'),  INIT_WCHAR_OBFUS('_'),  INIT_WCHAR_OBFUS('N'),  INIT_WCHAR_OBFUS('O'),  INIT_WCHAR_OBFUS('T'),  INIT_WCHAR_OBFUS('_'),  INIT_WCHAR_OBFUS('S'),  INIT_WCHAR_OBFUS('E'),  INIT_WCHAR_OBFUS('T'),  INIT_WCHAR_OBFUS('\0')};
DRM_STR_CONST        DRM_WCHAR      g_rgwchSecClockSet                           [] =  { INIT_WCHAR_OBFUS('D'),  INIT_WCHAR_OBFUS('R'),  INIT_WCHAR_OBFUS('M'),  INIT_WCHAR_OBFUS('_'),  INIT_WCHAR_OBFUS('C'),  INIT_WCHAR_OBFUS('L'),  INIT_WCHAR_OBFUS('K'),  INIT_WCHAR_OBFUS('_'),  INIT_WCHAR_OBFUS('S'),  INIT_WCHAR_OBFUS('E'),  INIT_WCHAR_OBFUS('T'),  INIT_WCHAR_OBFUS('\0')};
DRM_STR_CONST        DRM_WCHAR      g_rgwchSecClockNeedsRefresh                  [] =  { INIT_WCHAR_OBFUS('D'),  INIT_WCHAR_OBFUS('R'),  INIT_WCHAR_OBFUS('M'),  INIT_WCHAR_OBFUS('_'),  INIT_WCHAR_OBFUS('C'),  INIT_WCHAR_OBFUS('L'),  INIT_WCHAR_OBFUS('K'),  INIT_WCHAR_OBFUS('_'),  INIT_WCHAR_OBFUS('N'),  INIT_WCHAR_OBFUS('E'),  INIT_WCHAR_OBFUS('E'),  INIT_WCHAR_OBFUS('D'),  INIT_WCHAR_OBFUS('S'),  INIT_WCHAR_OBFUS('_'),  INIT_WCHAR_OBFUS('R'),  INIT_WCHAR_OBFUS('E'),  INIT_WCHAR_OBFUS('F'),  INIT_WCHAR_OBFUS('R'),  INIT_WCHAR_OBFUS('E'),  INIT_WCHAR_OBFUS('S'),  INIT_WCHAR_OBFUS('H'),  INIT_WCHAR_OBFUS('\0')};
DRM_STR_CONST        DRM_WCHAR      g_rgwchExprVarSavedDateTime                  [] =  { INIT_WCHAR_OBFUS('s'),  INIT_WCHAR_OBFUS('a'),  INIT_WCHAR_OBFUS('v'),  INIT_WCHAR_OBFUS('e'),  INIT_WCHAR_OBFUS('d'),  INIT_WCHAR_OBFUS('d'),  INIT_WCHAR_OBFUS('a'),  INIT_WCHAR_OBFUS('t'),  INIT_WCHAR_OBFUS('e'),  INIT_WCHAR_OBFUS('t'),  INIT_WCHAR_OBFUS('i'),  INIT_WCHAR_OBFUS('m'),  INIT_WCHAR_OBFUS('e'),  INIT_WCHAR_OBFUS('\0')};
DRM_STR_CONST        DRM_WCHAR      g_rgwchInitialExtensibleStateAttributeName   [] =  { INIT_WCHAR_OBFUS('I'), INIT_WCHAR_OBFUS('n'), INIT_WCHAR_OBFUS('i'), INIT_WCHAR_OBFUS('t'), INIT_WCHAR_OBFUS('i'), INIT_WCHAR_OBFUS('a'), INIT_WCHAR_OBFUS('l'), INIT_WCHAR_OBFUS('E'), INIT_WCHAR_OBFUS('x'), INIT_WCHAR_OBFUS('t'), INIT_WCHAR_OBFUS('R'), INIT_WCHAR_OBFUS('e'), INIT_WCHAR_OBFUS('s'), INIT_WCHAR_OBFUS('t'), INIT_WCHAR_OBFUS('S'), INIT_WCHAR_OBFUS('t'), INIT_WCHAR_OBFUS('a'), INIT_WCHAR_OBFUS('t'), INIT_WCHAR_OBFUS('e'), INIT_WCHAR_OBFUS('\0') };
DRM_STR_CONST        DRM_WCHAR      g_rgwchTagMetering                           [] =  { INIT_WCHAR_OBFUS('M'),  INIT_WCHAR_OBFUS('E'),  INIT_WCHAR_OBFUS('T'),  INIT_WCHAR_OBFUS('E'),  INIT_WCHAR_OBFUS('R'),  INIT_WCHAR_OBFUS('D'),  INIT_WCHAR_OBFUS('A'),  INIT_WCHAR_OBFUS('T'),  INIT_WCHAR_OBFUS('A'),  INIT_WCHAR_OBFUS('\0')};
DRM_STR_CONST        DRM_WCHAR      g_rgwchTagHash                               [] =  { INIT_WCHAR_OBFUS('H'),  INIT_WCHAR_OBFUS('A'),  INIT_WCHAR_OBFUS('S'),  INIT_WCHAR_OBFUS('H'),  INIT_WCHAR_OBFUS('\0')};
DRM_STR_CONST        DRM_WCHAR      g_rgwchTagCmd                                [] =  { INIT_WCHAR_OBFUS('C'),  INIT_WCHAR_OBFUS('O'),  INIT_WCHAR_OBFUS('M'),  INIT_WCHAR_OBFUS('M'),  INIT_WCHAR_OBFUS('A'),  INIT_WCHAR_OBFUS('N'),  INIT_WCHAR_OBFUS('D'),  INIT_WCHAR_OBFUS('\0')};
DRM_STR_CONST        DRM_WCHAR      g_rgwchTagPartialData                        [] =  { INIT_WCHAR_OBFUS('P'),  INIT_WCHAR_OBFUS('A'),  INIT_WCHAR_OBFUS('R'),  INIT_WCHAR_OBFUS('T'),  INIT_WCHAR_OBFUS('I'),  INIT_WCHAR_OBFUS('A'),  INIT_WCHAR_OBFUS('L'),  INIT_WCHAR_OBFUS('D'),  INIT_WCHAR_OBFUS('A'),  INIT_WCHAR_OBFUS('T'),  INIT_WCHAR_OBFUS('A'),  INIT_WCHAR_OBFUS('\0')};
DRM_STR_CONST        DRM_WCHAR      g_rgwchHMAC                                  [] =  { INIT_WCHAR_OBFUS('H'),  INIT_WCHAR_OBFUS('M'),  INIT_WCHAR_OBFUS('A'),  INIT_WCHAR_OBFUS('C'),  INIT_WCHAR_OBFUS('\0')};
DRM_STR_CONST        DRM_WCHAR      g_rgwch1                                     [] =  { INIT_WCHAR_OBFUS('1'),  INIT_WCHAR_OBFUS('\0')};
DRM_STR_CONST        DRM_WCHAR      g_rgwchReset                                 [] =  { INIT_WCHAR_OBFUS('R'),  INIT_WCHAR_OBFUS('E'),  INIT_WCHAR_OBFUS('S'),  INIT_WCHAR_OBFUS('E'),  INIT_WCHAR_OBFUS('T'),  INIT_WCHAR_OBFUS('\0')};
DRM_STR_CONST        DRM_WCHAR      g_rgwchLicense                               [] =  { INIT_WCHAR_OBFUS('L'), INIT_WCHAR_OBFUS('I'), INIT_WCHAR_OBFUS('C'), INIT_WCHAR_OBFUS('E'), INIT_WCHAR_OBFUS('N'), INIT_WCHAR_OBFUS('S'), INIT_WCHAR_OBFUS('E'), INIT_WCHAR_OBFUS('\0')};
DRM_STR_CONST        DRM_WCHAR      g_rgwchTagRevocation                         [] =  { INIT_WCHAR_OBFUS('R'), INIT_WCHAR_OBFUS('E'), INIT_WCHAR_OBFUS('V'), INIT_WCHAR_OBFUS('O'), INIT_WCHAR_OBFUS('C'), INIT_WCHAR_OBFUS('A'), INIT_WCHAR_OBFUS('T'), INIT_WCHAR_OBFUS('I'), INIT_WCHAR_OBFUS('O'), INIT_WCHAR_OBFUS('N'), INIT_WCHAR_OBFUS('\0')};

/* Rights */
DRM_STR_CONST        DRM_WCHAR      g_rgwchWMDRM_RIGHT_NONE                      [] =  { INIT_WCHAR_OBFUS('N'),  INIT_WCHAR_OBFUS('o'),  INIT_WCHAR_OBFUS('n'),  INIT_WCHAR_OBFUS('e'),  INIT_WCHAR_OBFUS('\0')};
DRM_STR_CONST        DRM_WCHAR      g_rgwchWMDRM_RIGHT_PLAYBACK                  [] =  { INIT_WCHAR_OBFUS('P'),  INIT_WCHAR_OBFUS('l'),  INIT_WCHAR_OBFUS('a'),  INIT_WCHAR_OBFUS('y'),  INIT_WCHAR_OBFUS('\0')};
DRM_STR_CONST        DRM_WCHAR      g_rgwchWMDRM_RIGHT_COLLABORATIVE_PLAY        [] =  { INIT_WCHAR_OBFUS('C'),  INIT_WCHAR_OBFUS('o'),  INIT_WCHAR_OBFUS('l'),  INIT_WCHAR_OBFUS('l'),  INIT_WCHAR_OBFUS('a'),  INIT_WCHAR_OBFUS('b'),  INIT_WCHAR_OBFUS('o'),  INIT_WCHAR_OBFUS('r'),  INIT_WCHAR_OBFUS('a'),  INIT_WCHAR_OBFUS('t'),  INIT_WCHAR_OBFUS('i'),  INIT_WCHAR_OBFUS('v'),  INIT_WCHAR_OBFUS('e'),  INIT_WCHAR_OBFUS('P'),  INIT_WCHAR_OBFUS('l'),  INIT_WCHAR_OBFUS('a'),  INIT_WCHAR_OBFUS('y'),  INIT_WCHAR_OBFUS('\0')};
DRM_STR_CONST        DRM_WCHAR      g_rgwchWMDRM_RIGHT_COPY_TO_CD                [] =  { INIT_WCHAR_OBFUS('P'),  INIT_WCHAR_OBFUS('r'),  INIT_WCHAR_OBFUS('i'),  INIT_WCHAR_OBFUS('n'),  INIT_WCHAR_OBFUS('t'),  INIT_WCHAR_OBFUS('.'),  INIT_WCHAR_OBFUS('r'),  INIT_WCHAR_OBFUS('e'),  INIT_WCHAR_OBFUS('d'),  INIT_WCHAR_OBFUS('b'),  INIT_WCHAR_OBFUS('o'),  INIT_WCHAR_OBFUS('o'),  INIT_WCHAR_OBFUS('k'),  INIT_WCHAR_OBFUS('\0')};
DRM_STR_CONST        DRM_WCHAR      g_rgwchWMDRM_RIGHT_COPY                      [] =  { INIT_WCHAR_OBFUS('C'),  INIT_WCHAR_OBFUS('o'),  INIT_WCHAR_OBFUS('p'),  INIT_WCHAR_OBFUS('y'),  INIT_WCHAR_OBFUS('\0')};
DRM_STR_CONST        DRM_WCHAR      g_rgwchWMDRM_RIGHT_CREATE_THUMBNAIL_IMAGE    [] =  { INIT_WCHAR_OBFUS('C'),  INIT_WCHAR_OBFUS('r'),  INIT_WCHAR_OBFUS('e'),  INIT_WCHAR_OBFUS('a'),  INIT_WCHAR_OBFUS('t'),  INIT_WCHAR_OBFUS('e'),  INIT_WCHAR_OBFUS('T'),  INIT_WCHAR_OBFUS('h'),  INIT_WCHAR_OBFUS('u'),  INIT_WCHAR_OBFUS('m'),  INIT_WCHAR_OBFUS('b'),  INIT_WCHAR_OBFUS('n'),  INIT_WCHAR_OBFUS('a'),  INIT_WCHAR_OBFUS('i'),  INIT_WCHAR_OBFUS('l'),  INIT_WCHAR_OBFUS('I'),  INIT_WCHAR_OBFUS('m'),  INIT_WCHAR_OBFUS('a'),  INIT_WCHAR_OBFUS('g'),  INIT_WCHAR_OBFUS('e'),  INIT_WCHAR_OBFUS('\0') };  /* L"CreateThumbnailImage"; */
DRM_STR_CONST        DRM_WCHAR      g_rgwchWMDRM_RIGHT_MOVE                      [] =  { INIT_WCHAR_OBFUS('M'),  INIT_WCHAR_OBFUS('o'),  INIT_WCHAR_OBFUS('v'),  INIT_WCHAR_OBFUS('e'),  INIT_WCHAR_OBFUS('\0')};

/* Script varibles used for license properties. */
DRM_STR_CONST        DRM_WCHAR      g_rgwchDRM_LS_BEGIN_ATTR                     [] =  { INIT_WCHAR_OBFUS('B'),  INIT_WCHAR_OBFUS('e'),  INIT_WCHAR_OBFUS('g'),  INIT_WCHAR_OBFUS('i'),  INIT_WCHAR_OBFUS('n'),  INIT_WCHAR_OBFUS('\0')};
DRM_STR_CONST        DRM_WCHAR      g_rgwchDRM_LS_COUNT_ATTR                     [] =  { INIT_WCHAR_OBFUS('C'),  INIT_WCHAR_OBFUS('o'),  INIT_WCHAR_OBFUS('u'),  INIT_WCHAR_OBFUS('n'),  INIT_WCHAR_OBFUS('t'),  INIT_WCHAR_OBFUS('\0')};
DRM_STR_CONST        DRM_WCHAR      g_rgwchDRM_LS_BEGDATE_ATTR                   [] =  { INIT_WCHAR_OBFUS('B'),  INIT_WCHAR_OBFUS('e'),  INIT_WCHAR_OBFUS('g'),  INIT_WCHAR_OBFUS('i'),  INIT_WCHAR_OBFUS('n'),  INIT_WCHAR_OBFUS('D'),  INIT_WCHAR_OBFUS('a'),  INIT_WCHAR_OBFUS('t'),  INIT_WCHAR_OBFUS('e'),  INIT_WCHAR_OBFUS('\0')};
DRM_STR_CONST        DRM_WCHAR      g_rgwchDRM_LS_ENDDATE_ATTR                   [] =  { INIT_WCHAR_OBFUS('E'),  INIT_WCHAR_OBFUS('x'),  INIT_WCHAR_OBFUS('p'),  INIT_WCHAR_OBFUS('i'),  INIT_WCHAR_OBFUS('r'),  INIT_WCHAR_OBFUS('a'),  INIT_WCHAR_OBFUS('t'),  INIT_WCHAR_OBFUS('i'),  INIT_WCHAR_OBFUS('o'),  INIT_WCHAR_OBFUS('n'),  INIT_WCHAR_OBFUS('D'),  INIT_WCHAR_OBFUS('a'),  INIT_WCHAR_OBFUS('t'),  INIT_WCHAR_OBFUS('e'),  INIT_WCHAR_OBFUS('\0')};
DRM_STR_CONST        DRM_WCHAR      g_rgwchDRM_LS_FIRSTUSE_ATTR                  [] =  { INIT_WCHAR_OBFUS('F'),  INIT_WCHAR_OBFUS('i'),  INIT_WCHAR_OBFUS('r'),  INIT_WCHAR_OBFUS('s'),  INIT_WCHAR_OBFUS('t'),  INIT_WCHAR_OBFUS('U'),  INIT_WCHAR_OBFUS('s'),  INIT_WCHAR_OBFUS('e'),  INIT_WCHAR_OBFUS('D'),  INIT_WCHAR_OBFUS('a'),  INIT_WCHAR_OBFUS('t'),  INIT_WCHAR_OBFUS('e'),  INIT_WCHAR_OBFUS('\0')};
DRM_STR_CONST        DRM_WCHAR      g_rgwchDRM_LS_FIRSTSTORE_ATTR                [] =  { INIT_WCHAR_OBFUS('F'),  INIT_WCHAR_OBFUS('i'),  INIT_WCHAR_OBFUS('r'),  INIT_WCHAR_OBFUS('s'),  INIT_WCHAR_OBFUS('t'),  INIT_WCHAR_OBFUS('S'),  INIT_WCHAR_OBFUS('t'),  INIT_WCHAR_OBFUS('o'),  INIT_WCHAR_OBFUS('r'),  INIT_WCHAR_OBFUS('e'),  INIT_WCHAR_OBFUS('D'),  INIT_WCHAR_OBFUS('a'),  INIT_WCHAR_OBFUS('t'),  INIT_WCHAR_OBFUS('e'),  INIT_WCHAR_OBFUS('\0')};
DRM_STR_CONST        DRM_WCHAR      g_rgwchDRM_LS_APPSEC_ATTR                    [] =  { INIT_WCHAR_OBFUS('A'),  INIT_WCHAR_OBFUS('p'),  INIT_WCHAR_OBFUS('p'),  INIT_WCHAR_OBFUS('s'),  INIT_WCHAR_OBFUS('e'),  INIT_WCHAR_OBFUS('c'),  INIT_WCHAR_OBFUS('\0')};
DRM_STR_CONST        DRM_WCHAR      g_rgwchDRM_LS_PLAYCOUNT_ATTR                 [] =  { INIT_WCHAR_OBFUS('P'),  INIT_WCHAR_OBFUS('l'),  INIT_WCHAR_OBFUS('a'),  INIT_WCHAR_OBFUS('y'),  INIT_WCHAR_OBFUS('C'),  INIT_WCHAR_OBFUS('o'),  INIT_WCHAR_OBFUS('u'),  INIT_WCHAR_OBFUS('n'),  INIT_WCHAR_OBFUS('t'),  INIT_WCHAR_OBFUS('\0')};
DRM_STR_CONST        DRM_WCHAR      g_rgwchDRM_LS_COPYCOUNT_ATTR                 [] =  { INIT_WCHAR_OBFUS('C'),  INIT_WCHAR_OBFUS('o'),  INIT_WCHAR_OBFUS('p'),  INIT_WCHAR_OBFUS('y'),  INIT_WCHAR_OBFUS('C'),  INIT_WCHAR_OBFUS('o'),  INIT_WCHAR_OBFUS('u'),  INIT_WCHAR_OBFUS('n'),  INIT_WCHAR_OBFUS('t'),  INIT_WCHAR_OBFUS('\0')};
DRM_STR_CONST        DRM_WCHAR      g_rgwchDRM_LS_DELETED_ATTR                   [] =  { INIT_WCHAR_OBFUS('d'),  INIT_WCHAR_OBFUS('e'),  INIT_WCHAR_OBFUS('l'),  INIT_WCHAR_OBFUS('e'),  INIT_WCHAR_OBFUS('t'),  INIT_WCHAR_OBFUS('e'),  INIT_WCHAR_OBFUS('d'),  INIT_WCHAR_OBFUS('\0') };  /*L"deleted";*/

/* Shared XML tags */
DRM_STR_CONST        DRM_WCHAR      g_rgwchResponse                              [] =  { INIT_WCHAR_OBFUS('r'),  INIT_WCHAR_OBFUS('e'),  INIT_WCHAR_OBFUS('s'),  INIT_WCHAR_OBFUS('p'),  INIT_WCHAR_OBFUS('o'),  INIT_WCHAR_OBFUS('n'),  INIT_WCHAR_OBFUS('s'),  INIT_WCHAR_OBFUS('e'),  INIT_WCHAR_OBFUS('\0')};
DRM_STR_CONST        DRM_WCHAR      g_rgwchAttributeVersion                      [] =  { INIT_WCHAR_OBFUS('v'),  INIT_WCHAR_OBFUS('e'),  INIT_WCHAR_OBFUS('r'),  INIT_WCHAR_OBFUS('s'),  INIT_WCHAR_OBFUS('i'),  INIT_WCHAR_OBFUS('o'),  INIT_WCHAR_OBFUS('n'),  INIT_WCHAR_OBFUS('\0')};
DRM_STR_CONST        DRM_WCHAR      g_rgwchTagData                               [] =  { INIT_WCHAR_OBFUS('D'),  INIT_WCHAR_OBFUS('A'),  INIT_WCHAR_OBFUS('T'),  INIT_WCHAR_OBFUS('A'),  INIT_WCHAR_OBFUS('\0')};
DRM_STR_CONST        DRM_WCHAR      g_rgwchTagIndex                              [] =  { INIT_WCHAR_OBFUS('I'),  INIT_WCHAR_OBFUS('N'),  INIT_WCHAR_OBFUS('D'),  INIT_WCHAR_OBFUS('E'),  INIT_WCHAR_OBFUS('X'),  INIT_WCHAR_OBFUS('\0')};
DRM_STR_CONST        DRM_WCHAR      g_rgwchTagSymSig                             [] =  { INIT_WCHAR_OBFUS('S'),  INIT_WCHAR_OBFUS('Y'),  INIT_WCHAR_OBFUS('M'),  INIT_WCHAR_OBFUS('S'),  INIT_WCHAR_OBFUS('I'),  INIT_WCHAR_OBFUS('G'),  INIT_WCHAR_OBFUS('N'),  INIT_WCHAR_OBFUS('A'),  INIT_WCHAR_OBFUS('T'),  INIT_WCHAR_OBFUS('U'),  INIT_WCHAR_OBFUS('R'),  INIT_WCHAR_OBFUS('E'),  INIT_WCHAR_OBFUS('\0')};
DRM_STR_CONST        DRM_WCHAR      g_rgwchTagPubkey                             [] =  { INIT_WCHAR_OBFUS('P'),  INIT_WCHAR_OBFUS('U'),  INIT_WCHAR_OBFUS('B'),  INIT_WCHAR_OBFUS('K'),  INIT_WCHAR_OBFUS('E'),  INIT_WCHAR_OBFUS('Y'),  INIT_WCHAR_OBFUS('\0')};
DRM_STR_CONST        DRM_WCHAR      g_rgwchTagPRIVKEY                            [] =  { INIT_WCHAR_OBFUS('P'),  INIT_WCHAR_OBFUS('R'),  INIT_WCHAR_OBFUS('I'),  INIT_WCHAR_OBFUS('V'),  INIT_WCHAR_OBFUS('K'),  INIT_WCHAR_OBFUS('E'),   INIT_WCHAR_OBFUS('Y'), INIT_WCHAR_OBFUS('\0') };
DRM_STR_CONST        DRM_WCHAR      g_rgwchAttributePubkey                       [] =  { INIT_WCHAR_OBFUS('p'),  INIT_WCHAR_OBFUS('u'),  INIT_WCHAR_OBFUS('b'),  INIT_WCHAR_OBFUS('k'),  INIT_WCHAR_OBFUS('e'),  INIT_WCHAR_OBFUS('y'),  INIT_WCHAR_OBFUS('\0')};
DRM_STR_CONST        DRM_WCHAR      g_rgwchAttributeVerifiedCK                   [] =  { INIT_WCHAR_OBFUS('I'),  INIT_WCHAR_OBFUS('D'),  INIT_WCHAR_OBFUS('\0')};
DRM_STR_CONST        DRM_WCHAR      g_rgwchTagValue                              [] =  { INIT_WCHAR_OBFUS('V'),  INIT_WCHAR_OBFUS('A'),  INIT_WCHAR_OBFUS('L'),  INIT_WCHAR_OBFUS('U'),  INIT_WCHAR_OBFUS('E'),  INIT_WCHAR_OBFUS('\0')};
DRM_STR_CONST        DRM_WCHAR      g_rgwchTagSignature                          [] =  { INIT_WCHAR_OBFUS('S'),  INIT_WCHAR_OBFUS('I'),  INIT_WCHAR_OBFUS('G'),  INIT_WCHAR_OBFUS('N'),  INIT_WCHAR_OBFUS('A'),  INIT_WCHAR_OBFUS('T'),  INIT_WCHAR_OBFUS('U'),  INIT_WCHAR_OBFUS('R'),  INIT_WCHAR_OBFUS('E'),  INIT_WCHAR_OBFUS('\0')};
DRM_STR_CONST        DRM_WCHAR      g_rgwchTagHashAlg                            [] =  { INIT_WCHAR_OBFUS('H'),  INIT_WCHAR_OBFUS('A'),  INIT_WCHAR_OBFUS('S'),  INIT_WCHAR_OBFUS('H'),  INIT_WCHAR_OBFUS('A'),  INIT_WCHAR_OBFUS('L'),  INIT_WCHAR_OBFUS('G'),  INIT_WCHAR_OBFUS('O'),  INIT_WCHAR_OBFUS('R'),  INIT_WCHAR_OBFUS('I'),  INIT_WCHAR_OBFUS('T'),  INIT_WCHAR_OBFUS('H'),  INIT_WCHAR_OBFUS('M'),  INIT_WCHAR_OBFUS('\0')};
DRM_STR_CONST        DRM_WCHAR      g_rgwchTagSignAlg                            [] =  { INIT_WCHAR_OBFUS('S'),  INIT_WCHAR_OBFUS('I'),  INIT_WCHAR_OBFUS('G'),  INIT_WCHAR_OBFUS('N'),  INIT_WCHAR_OBFUS('A'),  INIT_WCHAR_OBFUS('L'),  INIT_WCHAR_OBFUS('G'),  INIT_WCHAR_OBFUS('O'),  INIT_WCHAR_OBFUS('R'),  INIT_WCHAR_OBFUS('I'),  INIT_WCHAR_OBFUS('T'),  INIT_WCHAR_OBFUS('H'),  INIT_WCHAR_OBFUS('M'),  INIT_WCHAR_OBFUS('\0')};
DRM_STR_CONST        DRM_WCHAR      g_rgwchSHA                                   [] =  { INIT_WCHAR_OBFUS('S'),  INIT_WCHAR_OBFUS('H'),  INIT_WCHAR_OBFUS('A'),  INIT_WCHAR_OBFUS('\0')};
DRM_STR_CONST        DRM_WCHAR      g_rgwchMSDRM                                 [] =  { INIT_WCHAR_OBFUS('M'),  INIT_WCHAR_OBFUS('S'),  INIT_WCHAR_OBFUS('D'),  INIT_WCHAR_OBFUS('R'),  INIT_WCHAR_OBFUS('M'),  INIT_WCHAR_OBFUS('\0')};
DRM_STR_CONST        DRM_CHAR       g_rgchAttributeType                          [] =  { INIT_CHAR_OBFUS('t'),   INIT_CHAR_OBFUS('y'),   INIT_CHAR_OBFUS('p'),   INIT_CHAR_OBFUS('e'),   INIT_CHAR_OBFUS('\0') };
DRM_STR_CONST        DRM_WCHAR      g_rgwchAttributeType                         [] =  { INIT_WCHAR_OBFUS('t'),  INIT_WCHAR_OBFUS('y'),  INIT_WCHAR_OBFUS('p'),  INIT_WCHAR_OBFUS('e'),  INIT_WCHAR_OBFUS('\0')};
DRM_STR_CONST        DRM_WCHAR      g_rgwchTagCertificate                        [] =  { INIT_WCHAR_OBFUS('C'),  INIT_WCHAR_OBFUS('E'),  INIT_WCHAR_OBFUS('R'),  INIT_WCHAR_OBFUS('T'),  INIT_WCHAR_OBFUS('I'),  INIT_WCHAR_OBFUS('F'),  INIT_WCHAR_OBFUS('I'),  INIT_WCHAR_OBFUS('C'),  INIT_WCHAR_OBFUS('A'),  INIT_WCHAR_OBFUS('T'),  INIT_WCHAR_OBFUS('E'),  INIT_WCHAR_OBFUS('\0')};
DRM_STR_CONST        DRM_WCHAR      g_rgwchTagDevCert                            [] =  { INIT_WCHAR_OBFUS('D'),  INIT_WCHAR_OBFUS('E'),  INIT_WCHAR_OBFUS('V'),  INIT_WCHAR_OBFUS('C'),  INIT_WCHAR_OBFUS('E'),  INIT_WCHAR_OBFUS('R'),  INIT_WCHAR_OBFUS('T'),  INIT_WCHAR_OBFUS('\0')};
DRM_STR_CONST        DRM_WCHAR      g_rgwchTagWrmHeader                          [] =  { INIT_WCHAR_OBFUS('W'),  INIT_WCHAR_OBFUS('R'),  INIT_WCHAR_OBFUS('M'),  INIT_WCHAR_OBFUS('H'),  INIT_WCHAR_OBFUS('E'),  INIT_WCHAR_OBFUS('A'),  INIT_WCHAR_OBFUS('D'),  INIT_WCHAR_OBFUS('E'),  INIT_WCHAR_OBFUS('R'),  INIT_WCHAR_OBFUS('\0')};
DRM_STR_CONST        DRM_WCHAR      g_rgwchAttributeVersion2Value                [] =  { INIT_WCHAR_OBFUS('2'),  INIT_WCHAR_OBFUS('.'),  INIT_WCHAR_OBFUS('0'),  INIT_WCHAR_OBFUS('.'),  INIT_WCHAR_OBFUS('0'),  INIT_WCHAR_OBFUS('.'),  INIT_WCHAR_OBFUS('0'),  INIT_WCHAR_OBFUS('\0')};
DRM_STR_CONST        DRM_WCHAR      g_rgwchAttributeVersion4Value                [] =  { INIT_WCHAR_OBFUS('4'),  INIT_WCHAR_OBFUS('.'),  INIT_WCHAR_OBFUS('0'),  INIT_WCHAR_OBFUS('.'),  INIT_WCHAR_OBFUS('0'),  INIT_WCHAR_OBFUS('.'),  INIT_WCHAR_OBFUS('0'),  INIT_WCHAR_OBFUS('\0')};
DRM_STR_CONST        DRM_WCHAR      g_rgwchAttributeVersion4_1Value              [] =  { INIT_WCHAR_OBFUS('4'),  INIT_WCHAR_OBFUS('.'),  INIT_WCHAR_OBFUS('1'),  INIT_WCHAR_OBFUS('.'),  INIT_WCHAR_OBFUS('0'),  INIT_WCHAR_OBFUS('.'),  INIT_WCHAR_OBFUS('0'),  INIT_WCHAR_OBFUS('\0')};
DRM_STR_CONST        DRM_WCHAR      g_rgwchTagLAINFO                             [] =  { INIT_WCHAR_OBFUS('L'),  INIT_WCHAR_OBFUS('A'),  INIT_WCHAR_OBFUS('I'),  INIT_WCHAR_OBFUS('N'),  INIT_WCHAR_OBFUS('F'),  INIT_WCHAR_OBFUS('O'),  INIT_WCHAR_OBFUS('\0')};
DRM_STR_CONST        DRM_WCHAR      g_rgwchTagV4DATA                             [] =  { INIT_WCHAR_OBFUS('V'),  INIT_WCHAR_OBFUS('4'),  INIT_WCHAR_OBFUS('D'),  INIT_WCHAR_OBFUS('A'),  INIT_WCHAR_OBFUS('T'),  INIT_WCHAR_OBFUS('A'),  INIT_WCHAR_OBFUS('\0')};
DRM_STR_CONST        DRM_WCHAR      g_rgwchTagLAURL                              [] =  { INIT_WCHAR_OBFUS('L'),  INIT_WCHAR_OBFUS('A'),  INIT_WCHAR_OBFUS('_'),  INIT_WCHAR_OBFUS('U'),  INIT_WCHAR_OBFUS('R'),  INIT_WCHAR_OBFUS('L'),  INIT_WCHAR_OBFUS('\0')};
DRM_STR_CONST        DRM_WCHAR      g_rgwchTagLUIURL                             [] =  { INIT_WCHAR_OBFUS('L'),  INIT_WCHAR_OBFUS('U'),  INIT_WCHAR_OBFUS('I'),  INIT_WCHAR_OBFUS('_'),  INIT_WCHAR_OBFUS('U'),  INIT_WCHAR_OBFUS('R'),  INIT_WCHAR_OBFUS('L'),  INIT_WCHAR_OBFUS('\0')};
DRM_STR_CONST        DRM_WCHAR      g_rgwchTagDSID                               [] =  { INIT_WCHAR_OBFUS('D'),  INIT_WCHAR_OBFUS('S'),  INIT_WCHAR_OBFUS('_'),  INIT_WCHAR_OBFUS('I'),  INIT_WCHAR_OBFUS('D'),  INIT_WCHAR_OBFUS('\0')};
DRM_STR_CONST        DRM_WCHAR      g_rgwchTagKID                                [] =  { INIT_WCHAR_OBFUS('K'),  INIT_WCHAR_OBFUS('I'),  INIT_WCHAR_OBFUS('D'),  INIT_WCHAR_OBFUS('\0')};
DRM_STR_CONST        DRM_WCHAR      g_rgwchTagChecksum                           [] =  { INIT_WCHAR_OBFUS('C'),  INIT_WCHAR_OBFUS('H'),  INIT_WCHAR_OBFUS('E'),  INIT_WCHAR_OBFUS('C'),  INIT_WCHAR_OBFUS('K'),  INIT_WCHAR_OBFUS('S'),  INIT_WCHAR_OBFUS('U'),  INIT_WCHAR_OBFUS('M'),  INIT_WCHAR_OBFUS('\0')};
DRM_STR_CONST        DRM_WCHAR      g_rgwchTagAlgID                              [] =  { INIT_WCHAR_OBFUS('A'),  INIT_WCHAR_OBFUS('L'),  INIT_WCHAR_OBFUS('G'),  INIT_WCHAR_OBFUS('I'),  INIT_WCHAR_OBFUS('D'),  INIT_WCHAR_OBFUS('\0')};
DRM_STR_CONST        DRM_WCHAR      g_rgwchTagUplink                             [] =  { INIT_WCHAR_OBFUS('U'),  INIT_WCHAR_OBFUS('P'),  INIT_WCHAR_OBFUS('L'),  INIT_WCHAR_OBFUS('I'),  INIT_WCHAR_OBFUS('N'),  INIT_WCHAR_OBFUS('K'),  INIT_WCHAR_OBFUS('\0')};
DRM_STR_CONST        DRM_WCHAR      g_rgwchTagDecryptorSetup                     [] =  { INIT_WCHAR_OBFUS('D'),  INIT_WCHAR_OBFUS('E'),  INIT_WCHAR_OBFUS('C'),  INIT_WCHAR_OBFUS('R'),  INIT_WCHAR_OBFUS('Y'),  INIT_WCHAR_OBFUS('P'),  INIT_WCHAR_OBFUS('T'),  INIT_WCHAR_OBFUS('O'),  INIT_WCHAR_OBFUS('R'),  INIT_WCHAR_OBFUS('S'),  INIT_WCHAR_OBFUS('E'),  INIT_WCHAR_OBFUS('T'),  INIT_WCHAR_OBFUS('U'),  INIT_WCHAR_OBFUS('P'),  INIT_WCHAR_OBFUS('\0')};
DRM_STR_CONST        DRM_WCHAR      g_rgwchTagProtectInfo                        [] =  { INIT_WCHAR_OBFUS( 'P'), INIT_WCHAR_OBFUS( 'R'), INIT_WCHAR_OBFUS( 'O'), INIT_WCHAR_OBFUS( 'T'), INIT_WCHAR_OBFUS( 'E'), INIT_WCHAR_OBFUS( 'C'), INIT_WCHAR_OBFUS( 'T'), INIT_WCHAR_OBFUS( 'I'), INIT_WCHAR_OBFUS( 'N'), INIT_WCHAR_OBFUS( 'F'), INIT_WCHAR_OBFUS( 'O'), INIT_WCHAR_OBFUS( '\0') };
DRM_STR_CONST        DRM_WCHAR      g_rgwchTagKeyLen                             [] =  { INIT_WCHAR_OBFUS( 'K'), INIT_WCHAR_OBFUS( 'E'), INIT_WCHAR_OBFUS( 'Y'), INIT_WCHAR_OBFUS( 'L'), INIT_WCHAR_OBFUS( 'E'), INIT_WCHAR_OBFUS( 'N'), INIT_WCHAR_OBFUS( '\0') };
DRM_STR_CONST        DRM_WCHAR      g_rgwchTagKeyLenNodeDataCocktail             [] =  { INIT_WCHAR_OBFUS( '7'), INIT_WCHAR_OBFUS( '\0') };
DRM_STR_CONST        DRM_WCHAR      g_rgwchTagKeyLenNodeDataAESCTR               [] =  { INIT_WCHAR_OBFUS( '1'), INIT_WCHAR_OBFUS( '6'), INIT_WCHAR_OBFUS( '\0') };
DRM_STR_CONST        DRM_WCHAR      g_rgwchTagCOCKTAIL                           [] =  { INIT_WCHAR_OBFUS( 'C'), INIT_WCHAR_OBFUS( 'O'), INIT_WCHAR_OBFUS( 'C'), INIT_WCHAR_OBFUS( 'K'), INIT_WCHAR_OBFUS( 'T'), INIT_WCHAR_OBFUS( 'A'), INIT_WCHAR_OBFUS( 'I'), INIT_WCHAR_OBFUS( 'L'), INIT_WCHAR_OBFUS( '\0') };
DRM_STR_CONST        DRM_WCHAR      g_rgwchTagAESCTR                             [] =  { INIT_WCHAR_OBFUS( 'A'), INIT_WCHAR_OBFUS( 'E'), INIT_WCHAR_OBFUS( 'S'), INIT_WCHAR_OBFUS( 'C'), INIT_WCHAR_OBFUS( 'T'), INIT_WCHAR_OBFUS( 'R'), INIT_WCHAR_OBFUS( '\0') };
DRM_STR_CONST        DRM_WCHAR      g_rgwchTagCustomAttributes                   [] =  { INIT_WCHAR_OBFUS( 'C'), INIT_WCHAR_OBFUS( 'U'), INIT_WCHAR_OBFUS( 'S'), INIT_WCHAR_OBFUS( 'T'), INIT_WCHAR_OBFUS( 'O'), INIT_WCHAR_OBFUS( 'M'), INIT_WCHAR_OBFUS( 'A'), INIT_WCHAR_OBFUS( 'T'), INIT_WCHAR_OBFUS( 'T'), INIT_WCHAR_OBFUS( 'R'), INIT_WCHAR_OBFUS( 'I'), INIT_WCHAR_OBFUS( 'B'), INIT_WCHAR_OBFUS( 'U'), INIT_WCHAR_OBFUS( 'T'), INIT_WCHAR_OBFUS( 'E'), INIT_WCHAR_OBFUS( 'S'), INIT_WCHAR_OBFUS( '\0') };
DRM_STR_CONST        DRM_WCHAR      g_rgwchTagONDEMAND                           [] =  { INIT_WCHAR_OBFUS( 'O'), INIT_WCHAR_OBFUS( 'N'), INIT_WCHAR_OBFUS( 'D'), INIT_WCHAR_OBFUS( 'E'), INIT_WCHAR_OBFUS( 'M'), INIT_WCHAR_OBFUS( 'A'), INIT_WCHAR_OBFUS( 'N'), INIT_WCHAR_OBFUS( 'D'), INIT_WCHAR_OBFUS( '\0') };
DRM_STR_CONST        DRM_WCHAR      g_rgwchEmptyWRMHeaderV4_1                    [] =  { INIT_WCHAR_OBFUS( '<'), INIT_WCHAR_OBFUS( 'W'), INIT_WCHAR_OBFUS( 'R'), INIT_WCHAR_OBFUS( 'M'),
                                                                                         INIT_WCHAR_OBFUS( 'H'), INIT_WCHAR_OBFUS( 'E'), INIT_WCHAR_OBFUS( 'A'), INIT_WCHAR_OBFUS( 'D'),
                                                                                         INIT_WCHAR_OBFUS( 'E'), INIT_WCHAR_OBFUS( 'R'), INIT_WCHAR_OBFUS( ' '), INIT_WCHAR_OBFUS( 'x'),
                                                                                         INIT_WCHAR_OBFUS( 'm'), INIT_WCHAR_OBFUS( 'l'), INIT_WCHAR_OBFUS( 'n'), INIT_WCHAR_OBFUS( 's'),
                                                                                         INIT_WCHAR_OBFUS( '='), INIT_WCHAR_OBFUS( '"'), INIT_WCHAR_OBFUS( 'h'), INIT_WCHAR_OBFUS( 't'),
                                                                                         INIT_WCHAR_OBFUS( 't'), INIT_WCHAR_OBFUS( 'p'), INIT_WCHAR_OBFUS( ':'), INIT_WCHAR_OBFUS( '/'),
                                                                                         INIT_WCHAR_OBFUS( '/'), INIT_WCHAR_OBFUS( 's'), INIT_WCHAR_OBFUS( 'c'), INIT_WCHAR_OBFUS( 'h'),
                                                                                         INIT_WCHAR_OBFUS( 'e'), INIT_WCHAR_OBFUS( 'm'), INIT_WCHAR_OBFUS( 'a'), INIT_WCHAR_OBFUS( 's'),
                                                                                         INIT_WCHAR_OBFUS( '.'), INIT_WCHAR_OBFUS( 'm'), INIT_WCHAR_OBFUS( 'i'), INIT_WCHAR_OBFUS( 'c'),
                                                                                         INIT_WCHAR_OBFUS( 'r'), INIT_WCHAR_OBFUS( 'o'), INIT_WCHAR_OBFUS( 's'), INIT_WCHAR_OBFUS( 'o'),
                                                                                         INIT_WCHAR_OBFUS( 'f'), INIT_WCHAR_OBFUS( 't'), INIT_WCHAR_OBFUS( '.'), INIT_WCHAR_OBFUS( 'c'),
                                                                                         INIT_WCHAR_OBFUS( 'o'), INIT_WCHAR_OBFUS( 'm'), INIT_WCHAR_OBFUS( '/'), INIT_WCHAR_OBFUS( 'D'),
                                                                                         INIT_WCHAR_OBFUS( 'R'), INIT_WCHAR_OBFUS( 'M'), INIT_WCHAR_OBFUS( '/'), INIT_WCHAR_OBFUS( '2'),
                                                                                         INIT_WCHAR_OBFUS( '0'), INIT_WCHAR_OBFUS( '0'), INIT_WCHAR_OBFUS( '7'), INIT_WCHAR_OBFUS( '/'),
                                                                                         INIT_WCHAR_OBFUS( '0'), INIT_WCHAR_OBFUS( '3'), INIT_WCHAR_OBFUS( '/'), INIT_WCHAR_OBFUS( 'P'),
                                                                                         INIT_WCHAR_OBFUS( 'l'), INIT_WCHAR_OBFUS( 'a'), INIT_WCHAR_OBFUS( 'y'), INIT_WCHAR_OBFUS( 'R'),
                                                                                         INIT_WCHAR_OBFUS( 'e'), INIT_WCHAR_OBFUS( 'a'), INIT_WCHAR_OBFUS( 'd'), INIT_WCHAR_OBFUS( 'y'),
                                                                                         INIT_WCHAR_OBFUS( 'H'), INIT_WCHAR_OBFUS( 'e'), INIT_WCHAR_OBFUS( 'a'), INIT_WCHAR_OBFUS( 'd'),
                                                                                         INIT_WCHAR_OBFUS( 'e'), INIT_WCHAR_OBFUS( 'r'), INIT_WCHAR_OBFUS( '"'), INIT_WCHAR_OBFUS( ' '),
                                                                                         INIT_WCHAR_OBFUS( 'v'), INIT_WCHAR_OBFUS( 'e'), INIT_WCHAR_OBFUS( 'r'), INIT_WCHAR_OBFUS( 's'),
                                                                                         INIT_WCHAR_OBFUS( 'i'), INIT_WCHAR_OBFUS( 'o'), INIT_WCHAR_OBFUS( 'n'), INIT_WCHAR_OBFUS( '='),
                                                                                         INIT_WCHAR_OBFUS( '"'), INIT_WCHAR_OBFUS( '4'), INIT_WCHAR_OBFUS( '.'), INIT_WCHAR_OBFUS( '1'),
                                                                                         INIT_WCHAR_OBFUS( '.'), INIT_WCHAR_OBFUS( '0'), INIT_WCHAR_OBFUS( '.'), INIT_WCHAR_OBFUS( '0'),
                                                                                         INIT_WCHAR_OBFUS( '"'), INIT_WCHAR_OBFUS( '>'), INIT_WCHAR_OBFUS( '<'), INIT_WCHAR_OBFUS( '/'),
                                                                                         INIT_WCHAR_OBFUS( 'W'), INIT_WCHAR_OBFUS( 'R'), INIT_WCHAR_OBFUS( 'M'), INIT_WCHAR_OBFUS( 'H'),
                                                                                         INIT_WCHAR_OBFUS( 'E'), INIT_WCHAR_OBFUS( 'A'), INIT_WCHAR_OBFUS( 'D'), INIT_WCHAR_OBFUS( 'E'),
                                                                                         INIT_WCHAR_OBFUS( 'R'), INIT_WCHAR_OBFUS( '>'), INIT_WCHAR_OBFUS( '\0') };
DRM_STR_CONST        DRM_WCHAR      g_rgwchTagAction                             [] =  { INIT_WCHAR_OBFUS('A'),  INIT_WCHAR_OBFUS('C'),  INIT_WCHAR_OBFUS('T'),  INIT_WCHAR_OBFUS('I'),  INIT_WCHAR_OBFUS('O'),  INIT_WCHAR_OBFUS('N'),  INIT_WCHAR_OBFUS('\0')};
DRM_STR_CONST        DRM_WCHAR      g_rgwchTagSecurityVersion                    [] =  { INIT_WCHAR_OBFUS('S'),  INIT_WCHAR_OBFUS('E'),  INIT_WCHAR_OBFUS('C'),  INIT_WCHAR_OBFUS('U'),  INIT_WCHAR_OBFUS('R'),  INIT_WCHAR_OBFUS('I'),  INIT_WCHAR_OBFUS('T'),  INIT_WCHAR_OBFUS('Y'),  INIT_WCHAR_OBFUS('V'),  INIT_WCHAR_OBFUS('E'),  INIT_WCHAR_OBFUS('R'),  INIT_WCHAR_OBFUS('S'),  INIT_WCHAR_OBFUS('I'),  INIT_WCHAR_OBFUS('O'),  INIT_WCHAR_OBFUS('N'),  INIT_WCHAR_OBFUS('\0')};
DRM_STR_CONST        DRM_WCHAR      g_rgwchTagMSDRMSignature                     [] =  { INIT_WCHAR_OBFUS('M'),  INIT_WCHAR_OBFUS('S'),  INIT_WCHAR_OBFUS('D'),  INIT_WCHAR_OBFUS('R'),  INIT_WCHAR_OBFUS('M'),  INIT_WCHAR_OBFUS('_'),  INIT_WCHAR_OBFUS('S'),  INIT_WCHAR_OBFUS('I'),  INIT_WCHAR_OBFUS('G'),  INIT_WCHAR_OBFUS('N'),  INIT_WCHAR_OBFUS('A'),  INIT_WCHAR_OBFUS('T'),  INIT_WCHAR_OBFUS('U'),  INIT_WCHAR_OBFUS('R'),  INIT_WCHAR_OBFUS('E'),  INIT_WCHAR_OBFUS('_'),  INIT_WCHAR_OBFUS('V'),  INIT_WCHAR_OBFUS('A'),  INIT_WCHAR_OBFUS('L'),  INIT_WCHAR_OBFUS('U'),  INIT_WCHAR_OBFUS('E'),  INIT_WCHAR_OBFUS('\0')};
DRM_STR_CONST        DRM_WCHAR      g_rgwchTagMID                                [] =  { INIT_WCHAR_OBFUS('M'),  INIT_WCHAR_OBFUS('I'),  INIT_WCHAR_OBFUS('D'),  INIT_WCHAR_OBFUS('\0')};
DRM_STR_CONST        DRM_WCHAR      g_rgwchTagTID                                [] =  { INIT_WCHAR_OBFUS('T'),  INIT_WCHAR_OBFUS('I'),  INIT_WCHAR_OBFUS('D'),  INIT_WCHAR_OBFUS('\0')};
DRM_STR_CONST        DRM_WCHAR      g_rgwchTagRecords                            [] =  { INIT_WCHAR_OBFUS('R'),  INIT_WCHAR_OBFUS('E'),  INIT_WCHAR_OBFUS('C'),  INIT_WCHAR_OBFUS('O'),  INIT_WCHAR_OBFUS('R'),  INIT_WCHAR_OBFUS('D'),  INIT_WCHAR_OBFUS('S'),  INIT_WCHAR_OBFUS('\0')};
DRM_STR_CONST        DRM_WCHAR      g_rgwchTagURL                                [] =  { INIT_WCHAR_OBFUS('U'),  INIT_WCHAR_OBFUS('R'),  INIT_WCHAR_OBFUS('L'),  INIT_WCHAR_OBFUS('\0')};
DRM_STR_CONST        DRM_WCHAR      g_rgwchLabelValue                            [] =  { INIT_WCHAR_OBFUS('v'),  INIT_WCHAR_OBFUS('a'),  INIT_WCHAR_OBFUS('l'),  INIT_WCHAR_OBFUS('u'),  INIT_WCHAR_OBFUS('e'),  INIT_WCHAR_OBFUS('\0')};
DRM_STR_CONST        DRM_WCHAR      g_rgwchChallenge                             [] =  { INIT_WCHAR_OBFUS('c'),  INIT_WCHAR_OBFUS('h'),  INIT_WCHAR_OBFUS('a'),  INIT_WCHAR_OBFUS('l'),  INIT_WCHAR_OBFUS('l'),  INIT_WCHAR_OBFUS('e'),  INIT_WCHAR_OBFUS('n'),  INIT_WCHAR_OBFUS('g'),  INIT_WCHAR_OBFUS('e'),  INIT_WCHAR_OBFUS('\0')};
DRM_STR_CONST        DRM_WCHAR      g_rgwchTagCertificateChain                   [] =  { INIT_WCHAR_OBFUS('C'),  INIT_WCHAR_OBFUS('E'),  INIT_WCHAR_OBFUS('R'),  INIT_WCHAR_OBFUS('T'),  INIT_WCHAR_OBFUS('I'),  INIT_WCHAR_OBFUS('F'),  INIT_WCHAR_OBFUS('I'),  INIT_WCHAR_OBFUS('C'),  INIT_WCHAR_OBFUS('A'),  INIT_WCHAR_OBFUS('T'),  INIT_WCHAR_OBFUS('E'),  INIT_WCHAR_OBFUS('C'),  INIT_WCHAR_OBFUS('H'),  INIT_WCHAR_OBFUS('A'),  INIT_WCHAR_OBFUS('I'),  INIT_WCHAR_OBFUS('N'),  INIT_WCHAR_OBFUS('\0')};
DRM_STR_CONST        DRM_WCHAR      g_rgwchTagPrivateKey                         [] =  { INIT_WCHAR_OBFUS('P'),  INIT_WCHAR_OBFUS('R'),  INIT_WCHAR_OBFUS('I'),  INIT_WCHAR_OBFUS('V'),  INIT_WCHAR_OBFUS('A'),  INIT_WCHAR_OBFUS('T'),  INIT_WCHAR_OBFUS('E'),  INIT_WCHAR_OBFUS('K'),  INIT_WCHAR_OBFUS('E'),  INIT_WCHAR_OBFUS('Y'),  INIT_WCHAR_OBFUS('\0')};
DRM_STR_CONST        DRM_WCHAR      g_rgwchTagDevice                             [] =  { INIT_WCHAR_OBFUS('D'),  INIT_WCHAR_OBFUS('E'),  INIT_WCHAR_OBFUS('V'),  INIT_WCHAR_OBFUS('I'),  INIT_WCHAR_OBFUS('C'),  INIT_WCHAR_OBFUS('E'),  INIT_WCHAR_OBFUS('\0')};
DRM_STR_CONST        DRM_WCHAR      g_rgwchRootSigValueVersionTag                [] =  { INIT_WCHAR_OBFUS('p'),  INIT_WCHAR_OBFUS('r'),  INIT_WCHAR_OBFUS('i'),  INIT_WCHAR_OBFUS('v'),  INIT_WCHAR_OBFUS('a'),  INIT_WCHAR_OBFUS('t'),  INIT_WCHAR_OBFUS('e'),  INIT_WCHAR_OBFUS('\0')};
DRM_STR_CONST        DRM_WCHAR      g_rgwchRootSigValueVersionVal                [] =  { INIT_WCHAR_OBFUS('1'),  INIT_WCHAR_OBFUS('\0')};
DRM_STR_CONST        DRM_WCHAR      g_rgwchTagDataPubKey                         [] =  { INIT_WCHAR_OBFUS('P'),  INIT_WCHAR_OBFUS('U'),  INIT_WCHAR_OBFUS('B'),  INIT_WCHAR_OBFUS('L'),  INIT_WCHAR_OBFUS('I'),  INIT_WCHAR_OBFUS('C'),  INIT_WCHAR_OBFUS('K'),  INIT_WCHAR_OBFUS('E'),  INIT_WCHAR_OBFUS('Y'),  INIT_WCHAR_OBFUS('\0')};
DRM_STR_CONST        DRM_WCHAR      g_rgwchTemplate                              [] =  { INIT_WCHAR_OBFUS('T'),  INIT_WCHAR_OBFUS('E'),  INIT_WCHAR_OBFUS('M'),  INIT_WCHAR_OBFUS('P'),  INIT_WCHAR_OBFUS('L'),  INIT_WCHAR_OBFUS('A'),  INIT_WCHAR_OBFUS('T'),  INIT_WCHAR_OBFUS('E'),  INIT_WCHAR_OBFUS('\0')};
DRM_STR_CONST        DRM_WCHAR      g_rgwchTagDataSecVerPlatform                 [] =  { INIT_WCHAR_OBFUS('P'),  INIT_WCHAR_OBFUS('L'),  INIT_WCHAR_OBFUS('A'),  INIT_WCHAR_OBFUS('T'),  INIT_WCHAR_OBFUS('F'),  INIT_WCHAR_OBFUS('O'),  INIT_WCHAR_OBFUS('R'),  INIT_WCHAR_OBFUS('M'),  INIT_WCHAR_OBFUS('_'),  INIT_WCHAR_OBFUS('I'),  INIT_WCHAR_OBFUS('D'),  INIT_WCHAR_OBFUS('\0')};

/* Shared Certificate tags */
DRM_STR_CONST        DRM_CHAR  g_rgchAttributeAlgorithm        [] = { INIT_CHAR_OBFUS('A'), INIT_CHAR_OBFUS('l'), INIT_CHAR_OBFUS('g'), INIT_CHAR_OBFUS('o'), INIT_CHAR_OBFUS('r'), INIT_CHAR_OBFUS('i'), INIT_CHAR_OBFUS('t'), INIT_CHAR_OBFUS('h'), INIT_CHAR_OBFUS('m'), INIT_CHAR_OBFUS('\0')};
#define g_rgchAttributeAlgorithm_LEN 9
DRM_STR_CONST        DRM_CHAR  g_rgchAttributeVersionWMDRM     [] = { INIT_CHAR_OBFUS('c'), INIT_CHAR_OBFUS(':'), INIT_CHAR_OBFUS('V'), INIT_CHAR_OBFUS('e'), INIT_CHAR_OBFUS('r'), INIT_CHAR_OBFUS('s'), INIT_CHAR_OBFUS('i'), INIT_CHAR_OBFUS('o'), INIT_CHAR_OBFUS('n'), INIT_CHAR_OBFUS('\0')};
#define g_rgchAttributeVersionWMDRM_LEN 9
DRM_STR_CONST        DRM_CHAR  g_rgchKeyUsageSignCert          [] = { INIT_CHAR_OBFUS('c'), INIT_CHAR_OBFUS(':'), INIT_CHAR_OBFUS('S'), INIT_CHAR_OBFUS('i'), INIT_CHAR_OBFUS('g'), INIT_CHAR_OBFUS('n'), INIT_CHAR_OBFUS('C'), INIT_CHAR_OBFUS('e'), INIT_CHAR_OBFUS('r'), INIT_CHAR_OBFUS('t'), INIT_CHAR_OBFUS('i'), INIT_CHAR_OBFUS('f'), INIT_CHAR_OBFUS('i'), INIT_CHAR_OBFUS('c'), INIT_CHAR_OBFUS('a'), INIT_CHAR_OBFUS('t'), INIT_CHAR_OBFUS('e'), INIT_CHAR_OBFUS('\0')};
#define g_rgchKeyUsageSignCert_LEN 17
DRM_STR_CONST        DRM_CHAR  g_rgchKeyUsageEncryptKey        [] = { INIT_CHAR_OBFUS('c'), INIT_CHAR_OBFUS(':'), INIT_CHAR_OBFUS('E'), INIT_CHAR_OBFUS('n'), INIT_CHAR_OBFUS('c'), INIT_CHAR_OBFUS('r'), INIT_CHAR_OBFUS('y'), INIT_CHAR_OBFUS('p'), INIT_CHAR_OBFUS('t'), INIT_CHAR_OBFUS('K'), INIT_CHAR_OBFUS('e'), INIT_CHAR_OBFUS('y'), INIT_CHAR_OBFUS('\0')};
#define g_rgchKeyUsageEncryptKey_LEN 12
DRM_STR_CONST        DRM_CHAR  g_rgchOne                       [] = { INIT_CHAR_OBFUS('1'), INIT_CHAR_OBFUS('\0')};
#define g_rgchOne_LEN 1
DRM_STR_CONST        DRM_CHAR  g_rgchPrefixManufacturer        [] = { INIT_CHAR_OBFUS('x'), INIT_CHAR_OBFUS('m'), INIT_CHAR_OBFUS('l'), INIT_CHAR_OBFUS('n'), INIT_CHAR_OBFUS('s'), INIT_CHAR_OBFUS(':'), INIT_CHAR_OBFUS('f'), INIT_CHAR_OBFUS('\0')};
#define g_rgchPrefixManufacturer_LEN 7
DRM_STR_CONST        DRM_CHAR  g_rgchTagCanonicalization       [] = { INIT_CHAR_OBFUS('C'), INIT_CHAR_OBFUS('a'), INIT_CHAR_OBFUS('n'), INIT_CHAR_OBFUS('o'), INIT_CHAR_OBFUS('n'), INIT_CHAR_OBFUS('i'), INIT_CHAR_OBFUS('c'), INIT_CHAR_OBFUS('a'), INIT_CHAR_OBFUS('l'), INIT_CHAR_OBFUS('i'), INIT_CHAR_OBFUS('z'), INIT_CHAR_OBFUS('a'), INIT_CHAR_OBFUS('t'), INIT_CHAR_OBFUS('i'), INIT_CHAR_OBFUS('o'), INIT_CHAR_OBFUS('n'), INIT_CHAR_OBFUS('M'), INIT_CHAR_OBFUS('e'), INIT_CHAR_OBFUS('t'), INIT_CHAR_OBFUS('h'), INIT_CHAR_OBFUS('o'), INIT_CHAR_OBFUS('d'), INIT_CHAR_OBFUS('\0')};
#define g_rgchTagCanonicalization_LEN 22
DRM_STR_CONST        DRM_CHAR  g_rgchTagCertificateCollection  [] = { INIT_CHAR_OBFUS('c'), INIT_CHAR_OBFUS(':'), INIT_CHAR_OBFUS('C'), INIT_CHAR_OBFUS('e'), INIT_CHAR_OBFUS('r'), INIT_CHAR_OBFUS('t'), INIT_CHAR_OBFUS('i'), INIT_CHAR_OBFUS('f'), INIT_CHAR_OBFUS('i'), INIT_CHAR_OBFUS('c'), INIT_CHAR_OBFUS('a'), INIT_CHAR_OBFUS('t'), INIT_CHAR_OBFUS('e'), INIT_CHAR_OBFUS('C'), INIT_CHAR_OBFUS('o'), INIT_CHAR_OBFUS('l'), INIT_CHAR_OBFUS('l'), INIT_CHAR_OBFUS('e'), INIT_CHAR_OBFUS('c'), INIT_CHAR_OBFUS('t'), INIT_CHAR_OBFUS('i'), INIT_CHAR_OBFUS('o'), INIT_CHAR_OBFUS('n'), INIT_CHAR_OBFUS('\0')};
#define g_rgchTagCertificateCollection_LEN 23
DRM_STR_CONST        DRM_CHAR  g_rgchTagDigestMethod           [] = { INIT_CHAR_OBFUS('D'), INIT_CHAR_OBFUS('i'), INIT_CHAR_OBFUS('g'), INIT_CHAR_OBFUS('e'), INIT_CHAR_OBFUS('s'), INIT_CHAR_OBFUS('t'), INIT_CHAR_OBFUS('M'), INIT_CHAR_OBFUS('e'), INIT_CHAR_OBFUS('t'), INIT_CHAR_OBFUS('h'), INIT_CHAR_OBFUS('o'), INIT_CHAR_OBFUS('d'), INIT_CHAR_OBFUS('\0')};
#define g_rgchTagDigestMethod_LEN 12
DRM_STR_CONST        DRM_CHAR  g_rgchURIDSigSHA1               [] = { INIT_CHAR_OBFUS('h'), INIT_CHAR_OBFUS('t'), INIT_CHAR_OBFUS('t'), INIT_CHAR_OBFUS('p'), INIT_CHAR_OBFUS(':'), INIT_CHAR_OBFUS('/'), INIT_CHAR_OBFUS('/'), INIT_CHAR_OBFUS('w'), INIT_CHAR_OBFUS('w'), INIT_CHAR_OBFUS('w'), INIT_CHAR_OBFUS('.'), INIT_CHAR_OBFUS('w'), INIT_CHAR_OBFUS('3'), INIT_CHAR_OBFUS('.'), INIT_CHAR_OBFUS('o'), INIT_CHAR_OBFUS('r'), INIT_CHAR_OBFUS('g'), INIT_CHAR_OBFUS('/'), INIT_CHAR_OBFUS('2'), INIT_CHAR_OBFUS('0'), INIT_CHAR_OBFUS('0'), INIT_CHAR_OBFUS('0'), INIT_CHAR_OBFUS('/'), INIT_CHAR_OBFUS('0'), INIT_CHAR_OBFUS('9'), INIT_CHAR_OBFUS('/'), INIT_CHAR_OBFUS('x'), INIT_CHAR_OBFUS('m'), INIT_CHAR_OBFUS('l'), INIT_CHAR_OBFUS('d'), INIT_CHAR_OBFUS('s'), INIT_CHAR_OBFUS('i'), INIT_CHAR_OBFUS('g'), INIT_CHAR_OBFUS('#'), INIT_CHAR_OBFUS('s'), INIT_CHAR_OBFUS('h'), INIT_CHAR_OBFUS('a'), INIT_CHAR_OBFUS('1'), INIT_CHAR_OBFUS('\0')};
#define g_rgchURIDSigSHA1_LEN 38
DRM_STR_CONST        DRM_CHAR  g_rgchTagDigestValue            [] = { INIT_CHAR_OBFUS('D'), INIT_CHAR_OBFUS('i'), INIT_CHAR_OBFUS('g'), INIT_CHAR_OBFUS('e'), INIT_CHAR_OBFUS('s'), INIT_CHAR_OBFUS('t'), INIT_CHAR_OBFUS('V'), INIT_CHAR_OBFUS('a'), INIT_CHAR_OBFUS('l'), INIT_CHAR_OBFUS('u'), INIT_CHAR_OBFUS('e'), INIT_CHAR_OBFUS('\0')};
#define g_rgchTagDigestValue_LEN 11
DRM_STR_CONST        DRM_CHAR  g_rgchTagSignatureValue         [] = { INIT_CHAR_OBFUS('S'), INIT_CHAR_OBFUS('i'), INIT_CHAR_OBFUS('g'), INIT_CHAR_OBFUS('n'), INIT_CHAR_OBFUS('a'), INIT_CHAR_OBFUS('t'), INIT_CHAR_OBFUS('u'), INIT_CHAR_OBFUS('r'), INIT_CHAR_OBFUS('e'), INIT_CHAR_OBFUS('V'), INIT_CHAR_OBFUS('a'), INIT_CHAR_OBFUS('l'), INIT_CHAR_OBFUS('u'), INIT_CHAR_OBFUS('e'), INIT_CHAR_OBFUS('\0')};
#define g_rgchTagSignatureValue_LEN 14
DRM_STR_CONST        DRM_CHAR  g_rgchTagKeyInfo                [] = { INIT_CHAR_OBFUS('K'), INIT_CHAR_OBFUS('e'), INIT_CHAR_OBFUS('y'), INIT_CHAR_OBFUS('I'), INIT_CHAR_OBFUS('n'), INIT_CHAR_OBFUS('f'), INIT_CHAR_OBFUS('o'), INIT_CHAR_OBFUS('\0')};
#define g_rgchTagKeyInfo_LEN 7
DRM_STR_CONST        DRM_CHAR  g_rgchTagPublicKey              [] = { INIT_CHAR_OBFUS('P'), INIT_CHAR_OBFUS('u'), INIT_CHAR_OBFUS('b'), INIT_CHAR_OBFUS('l'), INIT_CHAR_OBFUS('i'), INIT_CHAR_OBFUS('c'), INIT_CHAR_OBFUS('K'), INIT_CHAR_OBFUS('e'), INIT_CHAR_OBFUS('y'), INIT_CHAR_OBFUS('\0')};
#define g_rgchTagPublicKey_LEN 9
DRM_STR_CONST        DRM_CHAR  g_rgchTagPrivateKey             [] = { INIT_CHAR_OBFUS('P'), INIT_CHAR_OBFUS('r'), INIT_CHAR_OBFUS('i'), INIT_CHAR_OBFUS('v'), INIT_CHAR_OBFUS('a'), INIT_CHAR_OBFUS('t'), INIT_CHAR_OBFUS('e'), INIT_CHAR_OBFUS('K'), INIT_CHAR_OBFUS('e'), INIT_CHAR_OBFUS('y'), INIT_CHAR_OBFUS('\0')};
#define g_rgchTagPrivateKey_LEN 10
DRM_STR_CONST        DRM_CHAR  g_rgchTagKeyValue               [] = { INIT_CHAR_OBFUS('K'), INIT_CHAR_OBFUS('e'), INIT_CHAR_OBFUS('y'), INIT_CHAR_OBFUS('V'), INIT_CHAR_OBFUS('a'), INIT_CHAR_OBFUS('l'), INIT_CHAR_OBFUS('u'), INIT_CHAR_OBFUS('e'), INIT_CHAR_OBFUS('\0')};
#define g_rgchTagKeyValue_LEN 8
DRM_STR_CONST        DRM_CHAR  g_rgchTagRSAKeyValue            [] = { INIT_CHAR_OBFUS('R'), INIT_CHAR_OBFUS('S'), INIT_CHAR_OBFUS('A'), INIT_CHAR_OBFUS('K'), INIT_CHAR_OBFUS('e'), INIT_CHAR_OBFUS('y'), INIT_CHAR_OBFUS('V'), INIT_CHAR_OBFUS('a'), INIT_CHAR_OBFUS('l'), INIT_CHAR_OBFUS('u'), INIT_CHAR_OBFUS('e'), INIT_CHAR_OBFUS('\0')};
#define g_rgchTagRSAKeyValue_LEN 11
DRM_STR_CONST        DRM_CHAR  g_rgchTagModulus                [] = { INIT_CHAR_OBFUS('M'), INIT_CHAR_OBFUS('o'), INIT_CHAR_OBFUS('d'), INIT_CHAR_OBFUS('u'), INIT_CHAR_OBFUS('l'), INIT_CHAR_OBFUS('u'), INIT_CHAR_OBFUS('s'), INIT_CHAR_OBFUS('\0')};
#define g_rgchTagModulus_LEN 7
DRM_STR_CONST        DRM_CHAR  g_rgchTagExponent               [] = { INIT_CHAR_OBFUS('E'), INIT_CHAR_OBFUS('x'), INIT_CHAR_OBFUS('p'), INIT_CHAR_OBFUS('o'), INIT_CHAR_OBFUS('n'), INIT_CHAR_OBFUS('e'), INIT_CHAR_OBFUS('n'), INIT_CHAR_OBFUS('t'), INIT_CHAR_OBFUS('\0')};
#define g_rgchTagExponent_LEN 8
DRM_STR_CONST        DRM_CHAR  g_rgchTagManufacturerName       [] = { INIT_CHAR_OBFUS('c'), INIT_CHAR_OBFUS(':'), INIT_CHAR_OBFUS('M'), INIT_CHAR_OBFUS('a'), INIT_CHAR_OBFUS('n'), INIT_CHAR_OBFUS('u'), INIT_CHAR_OBFUS('f'), INIT_CHAR_OBFUS('a'), INIT_CHAR_OBFUS('c'), INIT_CHAR_OBFUS('t'), INIT_CHAR_OBFUS('u'), INIT_CHAR_OBFUS('r'), INIT_CHAR_OBFUS('e'), INIT_CHAR_OBFUS('r'), INIT_CHAR_OBFUS('N'), INIT_CHAR_OBFUS('a'), INIT_CHAR_OBFUS('m'), INIT_CHAR_OBFUS('e'), INIT_CHAR_OBFUS('\0')};
#define g_rgchTagManufacturerName_LEN 18
DRM_STR_CONST        DRM_CHAR  g_rgchTagManufacturerData       [] = { INIT_CHAR_OBFUS('c'), INIT_CHAR_OBFUS(':'), INIT_CHAR_OBFUS('M'), INIT_CHAR_OBFUS('a'), INIT_CHAR_OBFUS('n'), INIT_CHAR_OBFUS('u'), INIT_CHAR_OBFUS('f'), INIT_CHAR_OBFUS('a'), INIT_CHAR_OBFUS('c'), INIT_CHAR_OBFUS('t'), INIT_CHAR_OBFUS('u'), INIT_CHAR_OBFUS('r'), INIT_CHAR_OBFUS('e'), INIT_CHAR_OBFUS('r'), INIT_CHAR_OBFUS('D'), INIT_CHAR_OBFUS('a'), INIT_CHAR_OBFUS('t'), INIT_CHAR_OBFUS('a'), INIT_CHAR_OBFUS('\0')};
#define g_rgchTagManufacturerData_LEN 18
DRM_STR_CONST        DRM_CHAR  g_rgchURIRSASHA1                [] = { INIT_CHAR_OBFUS('h'), INIT_CHAR_OBFUS('t'), INIT_CHAR_OBFUS('t'), INIT_CHAR_OBFUS('p'), INIT_CHAR_OBFUS(':'), INIT_CHAR_OBFUS('/'), INIT_CHAR_OBFUS('/'), INIT_CHAR_OBFUS('w'), INIT_CHAR_OBFUS('w'), INIT_CHAR_OBFUS('w'), INIT_CHAR_OBFUS('.'), INIT_CHAR_OBFUS('w'), INIT_CHAR_OBFUS('3'), INIT_CHAR_OBFUS('.'), INIT_CHAR_OBFUS('o'), INIT_CHAR_OBFUS('r'), INIT_CHAR_OBFUS('g'), INIT_CHAR_OBFUS('/'), INIT_CHAR_OBFUS('2'), INIT_CHAR_OBFUS('0'), INIT_CHAR_OBFUS('0'), INIT_CHAR_OBFUS('0'), INIT_CHAR_OBFUS('/'), INIT_CHAR_OBFUS('0'), INIT_CHAR_OBFUS('9'), INIT_CHAR_OBFUS('/'), INIT_CHAR_OBFUS('x'), INIT_CHAR_OBFUS('m'), INIT_CHAR_OBFUS('l'), INIT_CHAR_OBFUS('d'), INIT_CHAR_OBFUS('s'), INIT_CHAR_OBFUS('i'), INIT_CHAR_OBFUS('g'), INIT_CHAR_OBFUS('#'), INIT_CHAR_OBFUS('r'), INIT_CHAR_OBFUS('s'), INIT_CHAR_OBFUS('a'), INIT_CHAR_OBFUS('-'), INIT_CHAR_OBFUS('s'), INIT_CHAR_OBFUS('h'), INIT_CHAR_OBFUS('a'), INIT_CHAR_OBFUS('1'), INIT_CHAR_OBFUS('\0')};
#define g_rgchURIRSASHA1_LEN 42
DRM_STR_CONST        DRM_CHAR  g_rgchURIRSASHA1_Old            [] = { INIT_CHAR_OBFUS('h'), INIT_CHAR_OBFUS('t'), INIT_CHAR_OBFUS('t'), INIT_CHAR_OBFUS('p'), INIT_CHAR_OBFUS(':'), INIT_CHAR_OBFUS('/'), INIT_CHAR_OBFUS('/'), INIT_CHAR_OBFUS('s'), INIT_CHAR_OBFUS('c'), INIT_CHAR_OBFUS('h'), INIT_CHAR_OBFUS('e'), INIT_CHAR_OBFUS('m'), INIT_CHAR_OBFUS('a'), INIT_CHAR_OBFUS('s'), INIT_CHAR_OBFUS('.'), INIT_CHAR_OBFUS('m'), INIT_CHAR_OBFUS('i'), INIT_CHAR_OBFUS('c'), INIT_CHAR_OBFUS('r'), INIT_CHAR_OBFUS('o'), INIT_CHAR_OBFUS('s'), INIT_CHAR_OBFUS('o'), INIT_CHAR_OBFUS('f'), INIT_CHAR_OBFUS('t'), INIT_CHAR_OBFUS('.'), INIT_CHAR_OBFUS('c'), INIT_CHAR_OBFUS('o'), INIT_CHAR_OBFUS('m'), INIT_CHAR_OBFUS('/'), INIT_CHAR_OBFUS('D'), INIT_CHAR_OBFUS('R'), INIT_CHAR_OBFUS('M'), INIT_CHAR_OBFUS('/'), INIT_CHAR_OBFUS('2'), INIT_CHAR_OBFUS('0'), INIT_CHAR_OBFUS('0'), INIT_CHAR_OBFUS('4'), INIT_CHAR_OBFUS('/'), INIT_CHAR_OBFUS('0'), INIT_CHAR_OBFUS('2'), INIT_CHAR_OBFUS('/'), INIT_CHAR_OBFUS('C'), INIT_CHAR_OBFUS('E'), INIT_CHAR_OBFUS('R'), INIT_CHAR_OBFUS('T'), INIT_CHAR_OBFUS('/'), INIT_CHAR_OBFUS('R'), INIT_CHAR_OBFUS('s'), INIT_CHAR_OBFUS('a'), INIT_CHAR_OBFUS('-'), INIT_CHAR_OBFUS('s'), INIT_CHAR_OBFUS('h'), INIT_CHAR_OBFUS('a'), INIT_CHAR_OBFUS('1'), INIT_CHAR_OBFUS('\0')};
#define g_rgchURIRSASHA1_Old_LEN 54
DRM_STR_CONST        DRM_CHAR  g_rgchTagReference              [] = { INIT_CHAR_OBFUS('R'), INIT_CHAR_OBFUS('e'), INIT_CHAR_OBFUS('f'), INIT_CHAR_OBFUS('e'), INIT_CHAR_OBFUS('r'), INIT_CHAR_OBFUS('e'), INIT_CHAR_OBFUS('n'), INIT_CHAR_OBFUS('c'), INIT_CHAR_OBFUS('e'), INIT_CHAR_OBFUS('\0')};
#define g_rgchTagReference_LEN 9
DRM_STR_CONST        DRM_CHAR  g_rgchTagTransforms             [] = { INIT_CHAR_OBFUS('T'), INIT_CHAR_OBFUS('r'), INIT_CHAR_OBFUS('a'), INIT_CHAR_OBFUS('n'), INIT_CHAR_OBFUS('s'), INIT_CHAR_OBFUS('f'), INIT_CHAR_OBFUS('o'), INIT_CHAR_OBFUS('r'), INIT_CHAR_OBFUS('m'), INIT_CHAR_OBFUS('s'), INIT_CHAR_OBFUS('\0')};
#define g_rgchTagTransforms_LEN 10
DRM_STR_CONST        DRM_CHAR  g_rgchTagTransform              [] = { INIT_CHAR_OBFUS('T'), INIT_CHAR_OBFUS('r'), INIT_CHAR_OBFUS('a'), INIT_CHAR_OBFUS('n'), INIT_CHAR_OBFUS('s'), INIT_CHAR_OBFUS('f'), INIT_CHAR_OBFUS('o'), INIT_CHAR_OBFUS('r'), INIT_CHAR_OBFUS('m'), INIT_CHAR_OBFUS('\0')};
#define g_rgchTagTransform_LEN 9
DRM_STR_CONST        DRM_CHAR  g_rgchURITransformMSCert        [] = { INIT_CHAR_OBFUS('h'), INIT_CHAR_OBFUS('t'), INIT_CHAR_OBFUS('t'), INIT_CHAR_OBFUS('p'), INIT_CHAR_OBFUS(':'), INIT_CHAR_OBFUS('/'), INIT_CHAR_OBFUS('/'), INIT_CHAR_OBFUS('w'), INIT_CHAR_OBFUS('w'), INIT_CHAR_OBFUS('w'), INIT_CHAR_OBFUS('.'), INIT_CHAR_OBFUS('m'), INIT_CHAR_OBFUS('i'), INIT_CHAR_OBFUS('c'), INIT_CHAR_OBFUS('r'), INIT_CHAR_OBFUS('o'), INIT_CHAR_OBFUS('s'), INIT_CHAR_OBFUS('o'), INIT_CHAR_OBFUS('f'), INIT_CHAR_OBFUS('t'), INIT_CHAR_OBFUS('.'), INIT_CHAR_OBFUS('c'), INIT_CHAR_OBFUS('o'), INIT_CHAR_OBFUS('m'), INIT_CHAR_OBFUS('/'), INIT_CHAR_OBFUS('D'), INIT_CHAR_OBFUS('R'), INIT_CHAR_OBFUS('M'), INIT_CHAR_OBFUS('/'), INIT_CHAR_OBFUS('C'), INIT_CHAR_OBFUS('E'), INIT_CHAR_OBFUS('R'), INIT_CHAR_OBFUS('T'), INIT_CHAR_OBFUS('/'), INIT_CHAR_OBFUS('v'), INIT_CHAR_OBFUS('2'), INIT_CHAR_OBFUS('/'), INIT_CHAR_OBFUS('D'), INIT_CHAR_OBFUS('a'), INIT_CHAR_OBFUS('t'), INIT_CHAR_OBFUS('a'), INIT_CHAR_OBFUS('\0')};
#define g_rgchURITransformMSCert_LEN 41
DRM_STR_CONST        DRM_CHAR  g_rgchURITransformMSCertColl    [] = { INIT_CHAR_OBFUS('h'), INIT_CHAR_OBFUS('t'), INIT_CHAR_OBFUS('t'), INIT_CHAR_OBFUS('p'), INIT_CHAR_OBFUS(':'), INIT_CHAR_OBFUS('/'), INIT_CHAR_OBFUS('/'), INIT_CHAR_OBFUS('s'), INIT_CHAR_OBFUS('c'), INIT_CHAR_OBFUS('h'), INIT_CHAR_OBFUS('e'), INIT_CHAR_OBFUS('m'), INIT_CHAR_OBFUS('a'), INIT_CHAR_OBFUS('s'), INIT_CHAR_OBFUS('.'), INIT_CHAR_OBFUS('m'), INIT_CHAR_OBFUS('i'), INIT_CHAR_OBFUS('c'), INIT_CHAR_OBFUS('r'), INIT_CHAR_OBFUS('o'), INIT_CHAR_OBFUS('s'), INIT_CHAR_OBFUS('o'), INIT_CHAR_OBFUS('f'), INIT_CHAR_OBFUS('t'), INIT_CHAR_OBFUS('.'), INIT_CHAR_OBFUS('c'), INIT_CHAR_OBFUS('o'), INIT_CHAR_OBFUS('m'), INIT_CHAR_OBFUS('/'), INIT_CHAR_OBFUS('D'), INIT_CHAR_OBFUS('R'), INIT_CHAR_OBFUS('M'), INIT_CHAR_OBFUS('/'), INIT_CHAR_OBFUS('2'), INIT_CHAR_OBFUS('0'), INIT_CHAR_OBFUS('0'), INIT_CHAR_OBFUS('4'), INIT_CHAR_OBFUS('/'), INIT_CHAR_OBFUS('0'), INIT_CHAR_OBFUS('2'), INIT_CHAR_OBFUS('/'), INIT_CHAR_OBFUS('c'), INIT_CHAR_OBFUS('e'), INIT_CHAR_OBFUS('r'), INIT_CHAR_OBFUS('t'), INIT_CHAR_OBFUS('\0')};
#define g_rgchURITransformMSCertColl_LEN 45
DRM_STR_CONST        DRM_CHAR  g_rgchURITransformMSCert_Old    [] = { INIT_CHAR_OBFUS('h'), INIT_CHAR_OBFUS('t'), INIT_CHAR_OBFUS('t'), INIT_CHAR_OBFUS('p'), INIT_CHAR_OBFUS(':'), INIT_CHAR_OBFUS('/'), INIT_CHAR_OBFUS('/'), INIT_CHAR_OBFUS('s'), INIT_CHAR_OBFUS('c'), INIT_CHAR_OBFUS('h'), INIT_CHAR_OBFUS('e'), INIT_CHAR_OBFUS('m'), INIT_CHAR_OBFUS('a'), INIT_CHAR_OBFUS('s'), INIT_CHAR_OBFUS('.'), INIT_CHAR_OBFUS('m'), INIT_CHAR_OBFUS('i'), INIT_CHAR_OBFUS('c'), INIT_CHAR_OBFUS('r'), INIT_CHAR_OBFUS('o'), INIT_CHAR_OBFUS('s'), INIT_CHAR_OBFUS('o'), INIT_CHAR_OBFUS('f'), INIT_CHAR_OBFUS('t'), INIT_CHAR_OBFUS('.'), INIT_CHAR_OBFUS('c'), INIT_CHAR_OBFUS('o'), INIT_CHAR_OBFUS('m'), INIT_CHAR_OBFUS('/'), INIT_CHAR_OBFUS('D'), INIT_CHAR_OBFUS('R'), INIT_CHAR_OBFUS('M'), INIT_CHAR_OBFUS('/'), INIT_CHAR_OBFUS('2'), INIT_CHAR_OBFUS('0'), INIT_CHAR_OBFUS('0'), INIT_CHAR_OBFUS('4'), INIT_CHAR_OBFUS('/'), INIT_CHAR_OBFUS('0'), INIT_CHAR_OBFUS('2'), INIT_CHAR_OBFUS('/'), INIT_CHAR_OBFUS('C'), INIT_CHAR_OBFUS('E'), INIT_CHAR_OBFUS('R'), INIT_CHAR_OBFUS('T'), INIT_CHAR_OBFUS('/'), INIT_CHAR_OBFUS('D'), INIT_CHAR_OBFUS('a'), INIT_CHAR_OBFUS('t'), INIT_CHAR_OBFUS('a'), INIT_CHAR_OBFUS('\0')};
#define g_rgchURITransformMSCert_Old_LEN 50
DRM_STR_CONST        DRM_CHAR  g_rgchURITransformC14N          [] = { INIT_CHAR_OBFUS('h'), INIT_CHAR_OBFUS('t'), INIT_CHAR_OBFUS('t'), INIT_CHAR_OBFUS('p'), INIT_CHAR_OBFUS(':'), INIT_CHAR_OBFUS('/'), INIT_CHAR_OBFUS('/'), INIT_CHAR_OBFUS('w'), INIT_CHAR_OBFUS('w'), INIT_CHAR_OBFUS('w'), INIT_CHAR_OBFUS('.'), INIT_CHAR_OBFUS('w'), INIT_CHAR_OBFUS('3'), INIT_CHAR_OBFUS('.'), INIT_CHAR_OBFUS('o'), INIT_CHAR_OBFUS('r'), INIT_CHAR_OBFUS('g'), INIT_CHAR_OBFUS('/'), INIT_CHAR_OBFUS('T'), INIT_CHAR_OBFUS('R'), INIT_CHAR_OBFUS('/'), INIT_CHAR_OBFUS('2'), INIT_CHAR_OBFUS('0'), INIT_CHAR_OBFUS('0'), INIT_CHAR_OBFUS('1'), INIT_CHAR_OBFUS('/'), INIT_CHAR_OBFUS('R'), INIT_CHAR_OBFUS('E'), INIT_CHAR_OBFUS('C'), INIT_CHAR_OBFUS('-'), INIT_CHAR_OBFUS('x'), INIT_CHAR_OBFUS('m'), INIT_CHAR_OBFUS('l'), INIT_CHAR_OBFUS('-'), INIT_CHAR_OBFUS('c'), INIT_CHAR_OBFUS('1'), INIT_CHAR_OBFUS('4'), INIT_CHAR_OBFUS('n'), INIT_CHAR_OBFUS('-'), INIT_CHAR_OBFUS('2'), INIT_CHAR_OBFUS('0'), INIT_CHAR_OBFUS('0'), INIT_CHAR_OBFUS('1'), INIT_CHAR_OBFUS('0'), INIT_CHAR_OBFUS('3'), INIT_CHAR_OBFUS('1'), INIT_CHAR_OBFUS('5'), INIT_CHAR_OBFUS('\0')};
#define g_rgchURITransformC14N_LEN 47
DRM_STR_CONST        DRM_CHAR  g_rgchTagWMDRMCertificate       [] = { INIT_CHAR_OBFUS('c'), INIT_CHAR_OBFUS(':'), INIT_CHAR_OBFUS('C'), INIT_CHAR_OBFUS('e'), INIT_CHAR_OBFUS('r'), INIT_CHAR_OBFUS('t'), INIT_CHAR_OBFUS('i'), INIT_CHAR_OBFUS('f'), INIT_CHAR_OBFUS('i'), INIT_CHAR_OBFUS('c'), INIT_CHAR_OBFUS('a'), INIT_CHAR_OBFUS('t'), INIT_CHAR_OBFUS('e'), INIT_CHAR_OBFUS('\0')};
#define g_rgchTagWMDRMCertificate_LEN 13
DRM_STR_CONST        DRM_CHAR  g_rgchTagDataWMDRM              [] = { INIT_CHAR_OBFUS('c'), INIT_CHAR_OBFUS(':'), INIT_CHAR_OBFUS('D'), INIT_CHAR_OBFUS('a'), INIT_CHAR_OBFUS('t'), INIT_CHAR_OBFUS('a'), INIT_CHAR_OBFUS('\0')};
#define g_rgchTagDataWMDRM_LEN 6
DRM_STR_CONST        DRM_CHAR  g_rgchTagWMDRMCertPublicKey     [] = { INIT_CHAR_OBFUS('c'), INIT_CHAR_OBFUS(':'), INIT_CHAR_OBFUS('P'), INIT_CHAR_OBFUS('u'), INIT_CHAR_OBFUS('b'), INIT_CHAR_OBFUS('l'), INIT_CHAR_OBFUS('i'), INIT_CHAR_OBFUS('c'), INIT_CHAR_OBFUS('K'), INIT_CHAR_OBFUS('e'), INIT_CHAR_OBFUS('y'), INIT_CHAR_OBFUS('\0')};
#define g_rgchTagWMDRMCertPublicKey_LEN 11
DRM_STR_CONST        DRM_CHAR  g_rgchTagWMDRMCertSecurityVersion[]= { INIT_CHAR_OBFUS('c'), INIT_CHAR_OBFUS(':'), INIT_CHAR_OBFUS('S'), INIT_CHAR_OBFUS('e'), INIT_CHAR_OBFUS('c'), INIT_CHAR_OBFUS('u'), INIT_CHAR_OBFUS('r'), INIT_CHAR_OBFUS('i'), INIT_CHAR_OBFUS('t'), INIT_CHAR_OBFUS('y'), INIT_CHAR_OBFUS('V'), INIT_CHAR_OBFUS('e'), INIT_CHAR_OBFUS('r'), INIT_CHAR_OBFUS('s'), INIT_CHAR_OBFUS('i'), INIT_CHAR_OBFUS('o'), INIT_CHAR_OBFUS('n'), INIT_CHAR_OBFUS('\0')};
#define g_rgchTagWMDRMCertSecurityVersion_LEN 17
DRM_STR_CONST        DRM_CHAR  g_rgchTagWMDRMCertSecurityLevel [] = { INIT_CHAR_OBFUS('c'), INIT_CHAR_OBFUS(':'), INIT_CHAR_OBFUS('S'), INIT_CHAR_OBFUS('e'), INIT_CHAR_OBFUS('c'), INIT_CHAR_OBFUS('u'), INIT_CHAR_OBFUS('r'), INIT_CHAR_OBFUS('i'), INIT_CHAR_OBFUS('t'), INIT_CHAR_OBFUS('y'), INIT_CHAR_OBFUS('L'), INIT_CHAR_OBFUS('e'), INIT_CHAR_OBFUS('v'), INIT_CHAR_OBFUS('e'), INIT_CHAR_OBFUS('l'), INIT_CHAR_OBFUS('\0')};
#define g_rgchTagWMDRMCertSecurityLevel_LEN 15
DRM_STR_CONST        DRM_CHAR  g_rgchTagWMDRMCertFeatures      [] = { INIT_CHAR_OBFUS('c'), INIT_CHAR_OBFUS(':'), INIT_CHAR_OBFUS('F'), INIT_CHAR_OBFUS('e'), INIT_CHAR_OBFUS('a'), INIT_CHAR_OBFUS('t'), INIT_CHAR_OBFUS('u'), INIT_CHAR_OBFUS('r'), INIT_CHAR_OBFUS('e'), INIT_CHAR_OBFUS('s'), INIT_CHAR_OBFUS('\0')};
#define g_rgchTagWMDRMCertFeatures_LEN 10
DRM_STR_CONST        DRM_CHAR  g_rgchTagWMDRMCertKeyUsage      [] = { INIT_CHAR_OBFUS('c'), INIT_CHAR_OBFUS(':'), INIT_CHAR_OBFUS('K'), INIT_CHAR_OBFUS('e'), INIT_CHAR_OBFUS('y'), INIT_CHAR_OBFUS('U'), INIT_CHAR_OBFUS('s'), INIT_CHAR_OBFUS('a'), INIT_CHAR_OBFUS('g'), INIT_CHAR_OBFUS('e'), INIT_CHAR_OBFUS('\0')};
#define g_rgchTagWMDRMCertKeyUsage_LEN 10
DRM_STR_CONST        DRM_CHAR  g_rgchFeaturesWMDRMTransmitter  [] = { INIT_CHAR_OBFUS('c'), INIT_CHAR_OBFUS(':'), INIT_CHAR_OBFUS('W'), INIT_CHAR_OBFUS('M'), INIT_CHAR_OBFUS('D'), INIT_CHAR_OBFUS('R'), INIT_CHAR_OBFUS('M'), INIT_CHAR_OBFUS('T'), INIT_CHAR_OBFUS('r'), INIT_CHAR_OBFUS('a'), INIT_CHAR_OBFUS('n'), INIT_CHAR_OBFUS('s'), INIT_CHAR_OBFUS('m'), INIT_CHAR_OBFUS('i'), INIT_CHAR_OBFUS('t'), INIT_CHAR_OBFUS('t'), INIT_CHAR_OBFUS('e'), INIT_CHAR_OBFUS('r'), INIT_CHAR_OBFUS('\0')};
#define g_rgchFeaturesWMDRMTransmitter_LEN 18
DRM_STR_CONST        DRM_CHAR  g_rgchFeaturesWMDRMReceiver     [] = { INIT_CHAR_OBFUS('c'), INIT_CHAR_OBFUS(':'), INIT_CHAR_OBFUS('W'), INIT_CHAR_OBFUS('M'), INIT_CHAR_OBFUS('D'), INIT_CHAR_OBFUS('R'), INIT_CHAR_OBFUS('M'), INIT_CHAR_OBFUS('R'), INIT_CHAR_OBFUS('e'), INIT_CHAR_OBFUS('c'), INIT_CHAR_OBFUS('e'), INIT_CHAR_OBFUS('i'), INIT_CHAR_OBFUS('v'), INIT_CHAR_OBFUS('e'), INIT_CHAR_OBFUS('r'), INIT_CHAR_OBFUS('\0')};
#define g_rgchFeaturesWMDRMReceiver_LEN 15
DRM_STR_CONST        DRM_CHAR  g_rgchTagSignature              [] = { INIT_CHAR_OBFUS('S'), INIT_CHAR_OBFUS('i'), INIT_CHAR_OBFUS('g'), INIT_CHAR_OBFUS('n'), INIT_CHAR_OBFUS('a'), INIT_CHAR_OBFUS('t'), INIT_CHAR_OBFUS('u'), INIT_CHAR_OBFUS('r'), INIT_CHAR_OBFUS('e'), INIT_CHAR_OBFUS('\0')};
#define g_rgchTagSignature_LEN 9
DRM_STR_CONST        DRM_CHAR  g_rgchTagSignatureMethod        [] = { INIT_CHAR_OBFUS('S'), INIT_CHAR_OBFUS('i'), INIT_CHAR_OBFUS('g'), INIT_CHAR_OBFUS('n'), INIT_CHAR_OBFUS('a'), INIT_CHAR_OBFUS('t'), INIT_CHAR_OBFUS('u'), INIT_CHAR_OBFUS('r'), INIT_CHAR_OBFUS('e'), INIT_CHAR_OBFUS('M'), INIT_CHAR_OBFUS('e'), INIT_CHAR_OBFUS('t'), INIT_CHAR_OBFUS('h'), INIT_CHAR_OBFUS('o'), INIT_CHAR_OBFUS('d'), INIT_CHAR_OBFUS('\0')};
#define g_rgchTagSignatureMethod_LEN 15
DRM_STR_CONST        DRM_CHAR  g_rgchTagSignedInfo             [] = { INIT_CHAR_OBFUS('S'), INIT_CHAR_OBFUS('i'), INIT_CHAR_OBFUS('g'), INIT_CHAR_OBFUS('n'), INIT_CHAR_OBFUS('e'), INIT_CHAR_OBFUS('d'), INIT_CHAR_OBFUS('I'), INIT_CHAR_OBFUS('n'), INIT_CHAR_OBFUS('f'), INIT_CHAR_OBFUS('o'), INIT_CHAR_OBFUS('\0')};
#define g_rgchTagSignedInfo_LEN 10
DRM_STR_CONST        DRM_CHAR  g_rgchURIC14N                   [] = { INIT_CHAR_OBFUS('h'), INIT_CHAR_OBFUS('t'), INIT_CHAR_OBFUS('t'), INIT_CHAR_OBFUS('p'), INIT_CHAR_OBFUS(':'), INIT_CHAR_OBFUS('/'), INIT_CHAR_OBFUS('/'), INIT_CHAR_OBFUS('w'), INIT_CHAR_OBFUS('w'), INIT_CHAR_OBFUS('w'), INIT_CHAR_OBFUS('.'), INIT_CHAR_OBFUS('w'), INIT_CHAR_OBFUS('3'), INIT_CHAR_OBFUS('.'), INIT_CHAR_OBFUS('o'), INIT_CHAR_OBFUS('r'), INIT_CHAR_OBFUS('g'), INIT_CHAR_OBFUS('/'), INIT_CHAR_OBFUS('T'), INIT_CHAR_OBFUS('R'), INIT_CHAR_OBFUS('/'), INIT_CHAR_OBFUS('2'), INIT_CHAR_OBFUS('0'), INIT_CHAR_OBFUS('0'), INIT_CHAR_OBFUS('1'), INIT_CHAR_OBFUS('/'), INIT_CHAR_OBFUS('R'), INIT_CHAR_OBFUS('E'), INIT_CHAR_OBFUS('C'), INIT_CHAR_OBFUS('-'), INIT_CHAR_OBFUS('x'), INIT_CHAR_OBFUS('m'), INIT_CHAR_OBFUS('l'), INIT_CHAR_OBFUS('-'), INIT_CHAR_OBFUS('c'), INIT_CHAR_OBFUS('1'), INIT_CHAR_OBFUS('4'), INIT_CHAR_OBFUS('n'), INIT_CHAR_OBFUS('-'), INIT_CHAR_OBFUS('2'), INIT_CHAR_OBFUS('0'), INIT_CHAR_OBFUS('0'), INIT_CHAR_OBFUS('1'), INIT_CHAR_OBFUS('0'), INIT_CHAR_OBFUS('3'), INIT_CHAR_OBFUS('1'), INIT_CHAR_OBFUS('5'), INIT_CHAR_OBFUS('\0')};
#define g_rgchURIC14N_LEN 47
DRM_STR_CONST        DRM_CHAR  g_rgchVersionWMDRM              [] = { INIT_CHAR_OBFUS('2'), INIT_CHAR_OBFUS('.'), INIT_CHAR_OBFUS('0'), INIT_CHAR_OBFUS('\0')};
#define g_rgchVersionWMDRM_LEN 3
DRM_STR_CONST        DRM_CHAR  g_rgchWMDRMCertExponent         [] = { INIT_CHAR_OBFUS('A'), INIT_CHAR_OBFUS('Q'), INIT_CHAR_OBFUS('A'), INIT_CHAR_OBFUS('B'), INIT_CHAR_OBFUS('\0')};
#define g_rgchWMDRMCertExponent_LEN 4
DRM_STR_CONST        DRM_CHAR  g_rgchPrefixMicrosoftCert       [] = { INIT_CHAR_OBFUS('x'), INIT_CHAR_OBFUS('m'), INIT_CHAR_OBFUS('l'), INIT_CHAR_OBFUS('n'), INIT_CHAR_OBFUS('s'), INIT_CHAR_OBFUS(':'), INIT_CHAR_OBFUS('c'), INIT_CHAR_OBFUS('\0')};
#define g_rgchPrefixMicrosoftCert_LEN 7
DRM_STR_CONST        DRM_CHAR  g_rgchMSNDRootPubKeyB64         [] = { INIT_CHAR_OBFUS('p'), INIT_CHAR_OBFUS('j'), INIT_CHAR_OBFUS('o'), INIT_CHAR_OBFUS('e'), INIT_CHAR_OBFUS('W'), INIT_CHAR_OBFUS('L'), INIT_CHAR_OBFUS('S'), INIT_CHAR_OBFUS('T'), INIT_CHAR_OBFUS('L'), INIT_CHAR_OBFUS('D'), INIT_CHAR_OBFUS('o'), INIT_CHAR_OBFUS('n'), INIT_CHAR_OBFUS('Q'), INIT_CHAR_OBFUS('G'), INIT_CHAR_OBFUS('8'), INIT_CHAR_OBFUS('S'), INIT_CHAR_OBFUS('h'), INIT_CHAR_OBFUS('e'), INIT_CHAR_OBFUS('6'), INIT_CHAR_OBFUS('Q'), INIT_CHAR_OBFUS('h'), INIT_CHAR_OBFUS('k'), INIT_CHAR_OBFUS('Y'), INIT_CHAR_OBFUS('b'), INIT_CHAR_OBFUS('Y'), INIT_CHAR_OBFUS('o'), INIT_CHAR_OBFUS('t'), INIT_CHAR_OBFUS('t'), INIT_CHAR_OBFUS('9'), INIT_CHAR_OBFUS('f'), INIT_CHAR_OBFUS('P'), INIT_CHAR_OBFUS('Z'), INIT_CHAR_OBFUS('8'), INIT_CHAR_OBFUS('t'), INIT_CHAR_OBFUS('H'), INIT_CHAR_OBFUS('d'), INIT_CHAR_OBFUS('B'), INIT_CHAR_OBFUS('1'), INIT_CHAR_OBFUS('2'), INIT_CHAR_OBFUS('8'), INIT_CHAR_OBFUS('Z'), INIT_CHAR_OBFUS('E'), INIT_CHAR_OBFUS('T'), INIT_CHAR_OBFUS('c'), INIT_CHAR_OBFUS('g'), INIT_CHAR_OBFUS('h'), INIT_CHAR_OBFUS('n'), INIT_CHAR_OBFUS('5'), INIT_CHAR_OBFUS('K'), INIT_CHAR_OBFUS('H'), INIT_CHAR_OBFUS('o'), INIT_CHAR_OBFUS('y'), INIT_CHAR_OBFUS('i'), INIT_CHAR_OBFUS('n'), INIT_CHAR_OBFUS('7'), INIT_CHAR_OBFUS('H'), INIT_CHAR_OBFUS('k'), INIT_CHAR_OBFUS('J'), INIT_CHAR_OBFUS('E'), INIT_CHAR_OBFUS('c'), INIT_CHAR_OBFUS('P'), INIT_CHAR_OBFUS('J'), INIT_CHAR_OBFUS('0'), INIT_CHAR_OBFUS('E'), INIT_CHAR_OBFUS('g'), INIT_CHAR_OBFUS('4'), INIT_CHAR_OBFUS('U'), INIT_CHAR_OBFUS('d'), INIT_CHAR_OBFUS('S'), INIT_CHAR_OBFUS('v'), INIT_CHAR_OBFUS('a'), INIT_CHAR_OBFUS('0'), INIT_CHAR_OBFUS('K'), INIT_CHAR_OBFUS('D'), INIT_CHAR_OBFUS('I'), INIT_CHAR_OBFUS('Y'), INIT_CHAR_OBFUS('D'), INIT_CHAR_OBFUS('j'), INIT_CHAR_OBFUS('A'), INIT_CHAR_OBFUS('3'), INIT_CHAR_OBFUS('E'), INIT_CHAR_OBFUS('X'), INIT_CHAR_OBFUS('d'), INIT_CHAR_OBFUS('6'), INIT_CHAR_OBFUS('9'), INIT_CHAR_OBFUS('R'), INIT_CHAR_OBFUS('3'), INIT_CHAR_OBFUS('C'), INIT_CHAR_OBFUS('N'), INIT_CHAR_OBFUS('2'), INIT_CHAR_OBFUS('W'), INIT_CHAR_OBFUS('p'), INIT_CHAR_OBFUS('/'), INIT_CHAR_OBFUS('Q'), INIT_CHAR_OBFUS('y'), INIT_CHAR_OBFUS('O'), INIT_CHAR_OBFUS('o'), INIT_CHAR_OBFUS('0'), INIT_CHAR_OBFUS('Z'), INIT_CHAR_OBFUS('P'), INIT_CHAR_OBFUS('Y'), INIT_CHAR_OBFUS('W'), INIT_CHAR_OBFUS('Y'), INIT_CHAR_OBFUS('p'), INIT_CHAR_OBFUS('3'), INIT_CHAR_OBFUS('N'), INIT_CHAR_OBFUS('X'), INIT_CHAR_OBFUS('p'), INIT_CHAR_OBFUS('J'), INIT_CHAR_OBFUS('7'), INIT_CHAR_OBFUS('0'), INIT_CHAR_OBFUS('0'), INIT_CHAR_OBFUS('t'), INIT_CHAR_OBFUS('K'), INIT_CHAR_OBFUS('P'), INIT_CHAR_OBFUS('g'), INIT_CHAR_OBFUS('I'), INIT_CHAR_OBFUS('p'), INIT_CHAR_OBFUS('l'), INIT_CHAR_OBFUS('z'), INIT_CHAR_OBFUS('o'), INIT_CHAR_OBFUS('5'), INIT_CHAR_OBFUS('w'), INIT_CHAR_OBFUS('V'), INIT_CHAR_OBFUS('d'), INIT_CHAR_OBFUS('/'), INIT_CHAR_OBFUS('6'), INIT_CHAR_OBFUS('9'), INIT_CHAR_OBFUS('g'), INIT_CHAR_OBFUS('7'), INIT_CHAR_OBFUS('j'), INIT_CHAR_OBFUS('+'), INIT_CHAR_OBFUS('j'), INIT_CHAR_OBFUS('8'), INIT_CHAR_OBFUS('M'), INIT_CHAR_OBFUS('6'), INIT_CHAR_OBFUS('6'), INIT_CHAR_OBFUS('W'), INIT_CHAR_OBFUS('7'), INIT_CHAR_OBFUS('V'), INIT_CHAR_OBFUS('N'), INIT_CHAR_OBFUS('m'), INIT_CHAR_OBFUS('D'), INIT_CHAR_OBFUS('w'), INIT_CHAR_OBFUS('a'), INIT_CHAR_OBFUS('N'), INIT_CHAR_OBFUS('s'), INIT_CHAR_OBFUS('9'), INIT_CHAR_OBFUS('m'), INIT_CHAR_OBFUS('D'), INIT_CHAR_OBFUS('c'), INIT_CHAR_OBFUS('1'), INIT_CHAR_OBFUS('p'), INIT_CHAR_OBFUS('2'), INIT_CHAR_OBFUS('+'), INIT_CHAR_OBFUS('V'), INIT_CHAR_OBFUS('V'), INIT_CHAR_OBFUS('M'), INIT_CHAR_OBFUS('s'), INIT_CHAR_OBFUS('D'), INIT_CHAR_OBFUS('h'), INIT_CHAR_OBFUS('O'), INIT_CHAR_OBFUS('s'), INIT_CHAR_OBFUS('V'), INIT_CHAR_OBFUS('/'), INIT_CHAR_OBFUS('A'), INIT_CHAR_OBFUS('u'), INIT_CHAR_OBFUS('6'), INIT_CHAR_OBFUS('E'), INIT_CHAR_OBFUS('+'), INIT_CHAR_OBFUS('E'), INIT_CHAR_OBFUS('='), INIT_CHAR_OBFUS('\0')};
#define g_rgchMSNDRootPubKeyB64_LEN 172

/* Misc strings shared across disparate functional areas */
const DRM_DISCARDABLE DRM_ANSI_CONST_STRING g_dastrTagSLK                               = CREATE_DRM_ANSI_STRING_EX( g_rgchTagSLK,     3 );
const DRM_DISCARDABLE DRM_ANSI_CONST_STRING g_dastrTagSLKID                             = CREATE_DRM_ANSI_STRING_EX( g_rgchTagSLKID,   5 );
const DRM_DISCARDABLE DRM_ANSI_CONST_STRING g_dastrTagSLKDATA                           = CREATE_DRM_ANSI_STRING_EX( g_rgchTagSLKDATA, 7 );
const DRM_DISCARDABLE DRM_CONST_STRING      g_dstrXPathSymSig                           = CREATE_DRM_STRING( g_rgwchXPathSymSig );
const DRM_DISCARDABLE DRM_ANSI_CONST_STRING g_adstrLicenseRespTag                       = CREATE_DRM_ANSI_STRING_EX( g_rgchLicenseRespTag,    15 );
const DRM_DISCARDABLE DRM_CONST_STRING      g_dstrGUID                                  = CREATE_DRM_STRING( g_rgwchGUID );
const DRM_DISCARDABLE DRM_CONST_STRING      g_dstrFlag                                  = CREATE_DRM_STRING( g_rgwchFlag );
const DRM_DISCARDABLE DRM_CONST_STRING      g_dstrSecureClockNotSet                     = CREATE_DRM_STRING( g_rgwchSecClockNotSet );
const DRM_DISCARDABLE DRM_CONST_STRING      g_dstrSecureClockSet                        = CREATE_DRM_STRING( g_rgwchSecClockSet );
const DRM_DISCARDABLE DRM_CONST_STRING      g_dstrSecureClockNeedsRefresh               = CREATE_DRM_STRING( g_rgwchSecClockNeedsRefresh );
const DRM_DISCARDABLE DRM_CONST_STRING      g_dstrExprVarSavedDateTime                  = CREATE_DRM_STRING( g_rgwchExprVarSavedDateTime );
const DRM_DISCARDABLE DRM_CONST_STRING      g_dstrInitialExtensibleStateAttributeName = CREATE_DRM_STRING( g_rgwchInitialExtensibleStateAttributeName );
const DRM_DISCARDABLE DRM_CONST_STRING      g_dstrTagMetering                           = CREATE_DRM_STRING( g_rgwchTagMetering );
const DRM_DISCARDABLE DRM_CONST_STRING      g_dstrTagHash                               = CREATE_DRM_STRING( g_rgwchTagHash );
const DRM_DISCARDABLE DRM_CONST_STRING      g_dstrTagCmd                                = CREATE_DRM_STRING( g_rgwchTagCmd );
const DRM_DISCARDABLE DRM_CONST_STRING      g_dstrTagPartialData                        = CREATE_DRM_STRING( g_rgwchTagPartialData );
const DRM_DISCARDABLE DRM_CONST_STRING      g_dstrHMAC                                  = CREATE_DRM_STRING( g_rgwchHMAC );
const DRM_DISCARDABLE DRM_CONST_STRING      g_dstr1                                     = CREATE_DRM_STRING( g_rgwch1 );
const DRM_DISCARDABLE DRM_CONST_STRING      g_dstrReset                                 = CREATE_DRM_STRING( g_rgwchReset );
const DRM_DISCARDABLE DRM_CONST_STRING      g_dstrLicense                               = CREATE_DRM_STRING( g_rgwchLicense );
const DRM_DISCARDABLE DRM_CONST_STRING      g_dstrTagRevocation                         = CREATE_DRM_STRING( g_rgwchTagRevocation );

/* Rights */
const DRM_DISCARDABLE DRM_CONST_STRING      g_dstrWMDRM_RIGHT_NONE                       = CREATE_DRM_STRING( g_rgwchWMDRM_RIGHT_NONE );
const DRM_DISCARDABLE DRM_CONST_STRING      g_dstrWMDRM_RIGHT_PLAYBACK                   = CREATE_DRM_STRING( g_rgwchWMDRM_RIGHT_PLAYBACK );
const DRM_DISCARDABLE DRM_CONST_STRING      g_dstrWMDRM_RIGHT_COLLABORATIVE_PLAY         = CREATE_DRM_STRING( g_rgwchWMDRM_RIGHT_COLLABORATIVE_PLAY );
const DRM_DISCARDABLE DRM_CONST_STRING      g_dstrWMDRM_RIGHT_COPY_TO_CD                 = CREATE_DRM_STRING( g_rgwchWMDRM_RIGHT_COPY_TO_CD );
const DRM_DISCARDABLE DRM_CONST_STRING      g_dstrWMDRM_RIGHT_COPY                       = CREATE_DRM_STRING( g_rgwchWMDRM_RIGHT_COPY );
const DRM_DISCARDABLE DRM_CONST_STRING      g_dstrWMDRM_RIGHT_CREATE_THUMBNAIL_IMAGE     = CREATE_DRM_STRING( g_rgwchWMDRM_RIGHT_CREATE_THUMBNAIL_IMAGE );
const DRM_DISCARDABLE DRM_CONST_STRING      g_dstrWMDRM_RIGHT_MOVE                       = CREATE_DRM_STRING( g_rgwchWMDRM_RIGHT_MOVE );

/* Script varibles used for license properties. */
const DRM_DISCARDABLE DRM_CONST_STRING      g_dstrDRM_LS_BEGIN_ATTR                     = CREATE_DRM_STRING( g_rgwchDRM_LS_BEGIN_ATTR );
const DRM_DISCARDABLE DRM_CONST_STRING      g_dstrDRM_LS_COUNT_ATTR                     = CREATE_DRM_STRING( g_rgwchDRM_LS_COUNT_ATTR );
const DRM_DISCARDABLE DRM_CONST_STRING      g_dstrDRM_LS_BEGDATE_ATTR                   = CREATE_DRM_STRING( g_rgwchDRM_LS_BEGDATE_ATTR );
const DRM_DISCARDABLE DRM_CONST_STRING      g_dstrDRM_LS_ENDDATE_ATTR                   = CREATE_DRM_STRING( g_rgwchDRM_LS_ENDDATE_ATTR );
const DRM_DISCARDABLE DRM_CONST_STRING      g_dstrDRM_LS_FIRSTUSE_ATTR                  = CREATE_DRM_STRING( g_rgwchDRM_LS_FIRSTUSE_ATTR );
const DRM_DISCARDABLE DRM_CONST_STRING      g_dstrDRM_LS_FIRSTSTORE_ATTR                = CREATE_DRM_STRING( g_rgwchDRM_LS_FIRSTSTORE_ATTR );
const DRM_DISCARDABLE DRM_CONST_STRING      g_dstrDRM_LS_APPSEC_ATTR                    = CREATE_DRM_STRING( g_rgwchDRM_LS_APPSEC_ATTR );
const DRM_DISCARDABLE DRM_CONST_STRING      g_dstrDRM_LS_PLAYCOUNT_ATTR                 = CREATE_DRM_STRING( g_rgwchDRM_LS_PLAYCOUNT_ATTR );
const DRM_DISCARDABLE DRM_CONST_STRING      g_dstrDRM_LS_COPYCOUNT_ATTR                 = CREATE_DRM_STRING( g_rgwchDRM_LS_COPYCOUNT_ATTR );
const DRM_DISCARDABLE DRM_CONST_STRING      g_dstrDRM_LS_DELETED_ATTR                   = CREATE_DRM_STRING( g_rgwchDRM_LS_DELETED_ATTR );

/* Shared XML tags */
const DRM_DISCARDABLE DRM_CONST_STRING      g_dstrResponse                              = CREATE_DRM_STRING( g_rgwchResponse );
const DRM_DISCARDABLE DRM_CONST_STRING      g_dstrAttributeVersion                      = CREATE_DRM_STRING( g_rgwchAttributeVersion );
const DRM_DISCARDABLE DRM_CONST_STRING      g_dstrTagData                               = CREATE_DRM_STRING( g_rgwchTagData );
const DRM_DISCARDABLE DRM_CONST_STRING      g_dstrTagIndex                              = CREATE_DRM_STRING( g_rgwchTagIndex );
const DRM_DISCARDABLE DRM_CONST_STRING      g_dstrTagSymSig                             = CREATE_DRM_STRING( g_rgwchTagSymSig );
const DRM_DISCARDABLE DRM_CONST_STRING      g_dstrTagPubkey                             = CREATE_DRM_STRING( g_rgwchTagPubkey );
const DRM_DISCARDABLE DRM_CONST_STRING      g_dstrTagPRIVKEY                            = CREATE_DRM_STRING( g_rgwchTagPRIVKEY );
const DRM_DISCARDABLE DRM_CONST_STRING      g_dstrAttributePubkey                       = CREATE_DRM_STRING( g_rgwchAttributePubkey );
const DRM_DISCARDABLE DRM_CONST_STRING      g_dstrAttributeVerifiedCK                   = CREATE_DRM_STRING( g_rgwchAttributeVerifiedCK );
const DRM_DISCARDABLE DRM_CONST_STRING      g_dstrTagValue                              = CREATE_DRM_STRING( g_rgwchTagValue );
const DRM_DISCARDABLE DRM_CONST_STRING      g_dstrTagSignature                          = CREATE_DRM_STRING( g_rgwchTagSignature );
const DRM_DISCARDABLE DRM_CONST_STRING      g_dstrTagHashAlg                            = CREATE_DRM_STRING( g_rgwchTagHashAlg );
const DRM_DISCARDABLE DRM_CONST_STRING      g_dstrTagSignAlg                            = CREATE_DRM_STRING( g_rgwchTagSignAlg );
const DRM_DISCARDABLE DRM_CONST_STRING      g_dstrSHA                                   = CREATE_DRM_STRING( g_rgwchSHA );
const DRM_DISCARDABLE DRM_CONST_STRING      g_dstrMSDRM                                 = CREATE_DRM_STRING( g_rgwchMSDRM );
const DRM_DISCARDABLE DRM_ANSI_CONST_STRING g_dastrAttributeType                        = CREATE_DRM_ANSI_STRING_EX( g_rgchAttributeType, 4 );
const DRM_DISCARDABLE DRM_CONST_STRING      g_dstrAttributeType                         = CREATE_DRM_STRING( g_rgwchAttributeType );
const DRM_DISCARDABLE DRM_CONST_STRING      g_dstrTagCertificate                        = CREATE_DRM_STRING( g_rgwchTagCertificate );
const DRM_DISCARDABLE DRM_CONST_STRING      g_dstrTagDevCert                            = CREATE_DRM_STRING( g_rgwchTagDevCert );
const DRM_DISCARDABLE DRM_CONST_STRING      g_dstrTagWrmHeader                          = CREATE_DRM_STRING( g_rgwchTagWrmHeader );
const DRM_DISCARDABLE DRM_CONST_STRING      g_dstrAttributeVersion2Value                = CREATE_DRM_STRING( g_rgwchAttributeVersion2Value );
const DRM_DISCARDABLE DRM_CONST_STRING      g_dstrAttributeVersion4Value                = CREATE_DRM_STRING( g_rgwchAttributeVersion4Value );
const DRM_DISCARDABLE DRM_CONST_STRING      g_dstrAttributeVersion4_1Value              = CREATE_DRM_STRING( g_rgwchAttributeVersion4_1Value );
const DRM_DISCARDABLE DRM_CONST_STRING      g_dstrTagLAINFO                             = CREATE_DRM_STRING( g_rgwchTagLAINFO );
const DRM_DISCARDABLE DRM_CONST_STRING      g_dstrTagV4DATA                             = CREATE_DRM_STRING( g_rgwchTagV4DATA );
const DRM_DISCARDABLE DRM_CONST_STRING      g_dstrTagLAURL                              = CREATE_DRM_STRING( g_rgwchTagLAURL );
const DRM_DISCARDABLE DRM_CONST_STRING      g_dstrTagLUIURL                             = CREATE_DRM_STRING( g_rgwchTagLUIURL );
const DRM_DISCARDABLE DRM_CONST_STRING      g_dstrTagDSID                               = CREATE_DRM_STRING( g_rgwchTagDSID );
const DRM_DISCARDABLE DRM_CONST_STRING      g_dstrTagKID                                = CREATE_DRM_STRING( g_rgwchTagKID );
const DRM_DISCARDABLE DRM_CONST_STRING      g_dstrTagChecksum                           = CREATE_DRM_STRING( g_rgwchTagChecksum );
const DRM_DISCARDABLE DRM_CONST_STRING      g_dstrTagAlgID                              = CREATE_DRM_STRING( g_rgwchTagAlgID );
const DRM_DISCARDABLE DRM_CONST_STRING      g_dstrTagUplink                             = CREATE_DRM_STRING( g_rgwchTagUplink );
const DRM_DISCARDABLE DRM_CONST_STRING      g_dstrTagDecryptorSetup                     = CREATE_DRM_STRING( g_rgwchTagDecryptorSetup );
const DRM_DISCARDABLE DRM_CONST_STRING      g_dstrTagProtectInfo                        = CREATE_DRM_STRING( g_rgwchTagProtectInfo );
const DRM_DISCARDABLE DRM_CONST_STRING      g_dstrTagKeyLen                             = CREATE_DRM_STRING( g_rgwchTagKeyLen );
const DRM_DISCARDABLE DRM_CONST_STRING      g_dstrKeyLenNodeDataCocktail                = CREATE_DRM_STRING( g_rgwchTagKeyLenNodeDataCocktail );
const DRM_DISCARDABLE DRM_CONST_STRING      g_dstrKeyLenNodeDataAESCTR                  = CREATE_DRM_STRING( g_rgwchTagKeyLenNodeDataAESCTR );
const DRM_DISCARDABLE DRM_CONST_STRING      g_dstrTagCOCKTAIL                           = CREATE_DRM_STRING( g_rgwchTagCOCKTAIL );
const DRM_DISCARDABLE DRM_CONST_STRING      g_dstrTagAESCTR                             = CREATE_DRM_STRING( g_rgwchTagAESCTR );
const DRM_DISCARDABLE DRM_CONST_STRING      g_dstrTagCustomAttributes                   = CREATE_DRM_STRING( g_rgwchTagCustomAttributes );
const DRM_DISCARDABLE DRM_CONST_STRING      g_dstrTagONDEMAND                           = CREATE_DRM_STRING( g_rgwchTagONDEMAND );
const DRM_DISCARDABLE DRM_CONST_STRING      g_dstrEmptyWRMHeaderV4_1                    = CREATE_DRM_STRING( g_rgwchEmptyWRMHeaderV4_1 );
const DRM_DISCARDABLE DRM_CONST_STRING      g_dstrTagAction                             = CREATE_DRM_STRING( g_rgwchTagAction );
const DRM_DISCARDABLE DRM_CONST_STRING      g_dstrTagSecurityVersion                    = CREATE_DRM_STRING( g_rgwchTagSecurityVersion );
const DRM_DISCARDABLE DRM_CONST_STRING      g_dstrTagMSDRMSignature                     = CREATE_DRM_STRING( g_rgwchTagMSDRMSignature );
const DRM_DISCARDABLE DRM_CONST_STRING      g_dstrTagMID                                = CREATE_DRM_STRING( g_rgwchTagMID );
const DRM_DISCARDABLE DRM_CONST_STRING      g_dstrTagTID                                = CREATE_DRM_STRING( g_rgwchTagTID );
const DRM_DISCARDABLE DRM_CONST_STRING      g_dstrTagRecords                            = CREATE_DRM_STRING( g_rgwchTagRecords );
const DRM_DISCARDABLE DRM_CONST_STRING      g_dstrTagURL                                = CREATE_DRM_STRING( g_rgwchTagURL );
const DRM_DISCARDABLE DRM_CONST_STRING      g_dstrLabelValue                            = CREATE_DRM_STRING( g_rgwchLabelValue );
const DRM_DISCARDABLE DRM_CONST_STRING      g_dstrChallenge                             = CREATE_DRM_STRING( g_rgwchChallenge );
const DRM_DISCARDABLE DRM_CONST_STRING      g_dstrTagCertificateChain                   = CREATE_DRM_STRING( g_rgwchTagCertificateChain );
const DRM_DISCARDABLE DRM_CONST_STRING      g_dstrTagPrivateKey                         = CREATE_DRM_STRING( g_rgwchTagPrivateKey );
const DRM_DISCARDABLE DRM_CONST_STRING      g_dstrTagDevice                             = CREATE_DRM_STRING( g_rgwchTagDevice );
const DRM_DISCARDABLE DRM_CONST_STRING      g_dstrRootSigValueVersionTag                = CREATE_DRM_STRING( g_rgwchRootSigValueVersionTag );
const DRM_DISCARDABLE DRM_CONST_STRING      g_dstrRootSigValueVersionVal                = CREATE_DRM_STRING( g_rgwchRootSigValueVersionVal );
const DRM_DISCARDABLE DRM_CONST_STRING      g_dstrTagDataPubKey                         = CREATE_DRM_STRING( g_rgwchTagDataPubKey );
const DRM_DISCARDABLE DRM_CONST_STRING      g_dstrTemplate                              = CREATE_DRM_STRING( g_rgwchTemplate );
const DRM_DISCARDABLE DRM_CONST_STRING      g_dstrTagDataSecVerPlatform                 = CREATE_DRM_STRING( g_rgwchTagDataSecVerPlatform );

/* Shared Certificate tags */
const DRM_DISCARDABLE DRM_ANSI_CONST_STRING  g_dastrAttributeAlgorithm                  = CREATE_DRM_ANSI_STRING_EX( g_rgchAttributeAlgorithm, g_rgchAttributeAlgorithm_LEN );
const DRM_DISCARDABLE DRM_ANSI_CONST_STRING  g_dastrAttributeVersionWMDRM               = CREATE_DRM_ANSI_STRING_EX( g_rgchAttributeVersionWMDRM, g_rgchAttributeVersionWMDRM_LEN );
const DRM_DISCARDABLE DRM_ANSI_CONST_STRING  g_dastrKeyUsageSignCert                    = CREATE_DRM_ANSI_STRING_EX( g_rgchKeyUsageSignCert, g_rgchKeyUsageSignCert_LEN );
const DRM_DISCARDABLE DRM_ANSI_CONST_STRING  g_dastrKeyUsageEncryptKey                  = CREATE_DRM_ANSI_STRING_EX( g_rgchKeyUsageEncryptKey, g_rgchKeyUsageEncryptKey_LEN );
const DRM_DISCARDABLE DRM_ANSI_CONST_STRING  g_dastrOne                                 = CREATE_DRM_ANSI_STRING_EX( g_rgchOne, g_rgchOne_LEN );
const DRM_DISCARDABLE DRM_ANSI_CONST_STRING  g_dastrPrefixManufacturer                  = CREATE_DRM_ANSI_STRING_EX( g_rgchPrefixManufacturer, g_rgchPrefixManufacturer_LEN );
const DRM_DISCARDABLE DRM_ANSI_CONST_STRING  g_dastrTagCanonicalization                 = CREATE_DRM_ANSI_STRING_EX( g_rgchTagCanonicalization, g_rgchTagCanonicalization_LEN );
const DRM_DISCARDABLE DRM_ANSI_CONST_STRING  g_dastrTagCertificateCollection            = CREATE_DRM_ANSI_STRING_EX( g_rgchTagCertificateCollection, g_rgchTagCertificateCollection_LEN );
const DRM_DISCARDABLE DRM_ANSI_CONST_STRING  g_dastrTagDigestMethod                     = CREATE_DRM_ANSI_STRING_EX( g_rgchTagDigestMethod, g_rgchTagDigestMethod_LEN );
const DRM_DISCARDABLE DRM_ANSI_CONST_STRING  g_dastrURIDSigSHA1                         = CREATE_DRM_ANSI_STRING_EX( g_rgchURIDSigSHA1, g_rgchURIDSigSHA1_LEN );
const DRM_DISCARDABLE DRM_ANSI_CONST_STRING  g_dastrTagDigestValue                      = CREATE_DRM_ANSI_STRING_EX( g_rgchTagDigestValue, g_rgchTagDigestValue_LEN );
const DRM_DISCARDABLE DRM_ANSI_CONST_STRING  g_dastrTagSignatureValue                   = CREATE_DRM_ANSI_STRING_EX( g_rgchTagSignatureValue, g_rgchTagSignatureValue_LEN );
const DRM_DISCARDABLE DRM_ANSI_CONST_STRING  g_dastrTagKeyInfo                          = CREATE_DRM_ANSI_STRING_EX( g_rgchTagKeyInfo, g_rgchTagKeyInfo_LEN );
const DRM_DISCARDABLE DRM_ANSI_CONST_STRING  g_dastrTagPublicKey                        = CREATE_DRM_ANSI_STRING_EX( g_rgchTagPublicKey, g_rgchTagPublicKey_LEN );
const DRM_DISCARDABLE DRM_ANSI_CONST_STRING  g_dastrTagPrivateKey                       = CREATE_DRM_ANSI_STRING_EX( g_rgchTagPrivateKey, g_rgchTagPrivateKey_LEN );
const DRM_DISCARDABLE DRM_ANSI_CONST_STRING  g_dastrTagKeyValue                         = CREATE_DRM_ANSI_STRING_EX( g_rgchTagKeyValue, g_rgchTagKeyValue_LEN );
const DRM_DISCARDABLE DRM_ANSI_CONST_STRING  g_dastrTagRSAKeyValue                      = CREATE_DRM_ANSI_STRING_EX( g_rgchTagRSAKeyValue, g_rgchTagRSAKeyValue_LEN );
const DRM_DISCARDABLE DRM_ANSI_CONST_STRING  g_dastrTagModulus                          = CREATE_DRM_ANSI_STRING_EX( g_rgchTagModulus, g_rgchTagModulus_LEN );
const DRM_DISCARDABLE DRM_ANSI_CONST_STRING  g_dastrTagExponent                         = CREATE_DRM_ANSI_STRING_EX( g_rgchTagExponent, g_rgchTagExponent_LEN );
const DRM_DISCARDABLE DRM_ANSI_CONST_STRING  g_dastrTagManufacturerName                 = CREATE_DRM_ANSI_STRING_EX( g_rgchTagManufacturerName, g_rgchTagManufacturerName_LEN );
const DRM_DISCARDABLE DRM_ANSI_CONST_STRING  g_dastrTagManufacturerData                 = CREATE_DRM_ANSI_STRING_EX( g_rgchTagManufacturerData, g_rgchTagManufacturerData_LEN );
const DRM_DISCARDABLE DRM_ANSI_CONST_STRING  g_dastrURIRSASHA1                          = CREATE_DRM_ANSI_STRING_EX( g_rgchURIRSASHA1, g_rgchURIRSASHA1_LEN );
const DRM_DISCARDABLE DRM_ANSI_CONST_STRING  g_dastrURIRSASHA1_Old                      = CREATE_DRM_ANSI_STRING_EX( g_rgchURIRSASHA1_Old, g_rgchURIRSASHA1_Old_LEN );
const DRM_DISCARDABLE DRM_ANSI_CONST_STRING  g_dastrTagReference                        = CREATE_DRM_ANSI_STRING_EX( g_rgchTagReference, g_rgchTagReference_LEN );
const DRM_DISCARDABLE DRM_ANSI_CONST_STRING  g_dastrTagTransforms                       = CREATE_DRM_ANSI_STRING_EX( g_rgchTagTransforms, g_rgchTagTransforms_LEN );
const DRM_DISCARDABLE DRM_ANSI_CONST_STRING  g_dastrTagTransform                        = CREATE_DRM_ANSI_STRING_EX( g_rgchTagTransform, g_rgchTagTransform_LEN );
const DRM_DISCARDABLE DRM_ANSI_CONST_STRING  g_dastrURITransformMSCert                  = CREATE_DRM_ANSI_STRING_EX( g_rgchURITransformMSCert, g_rgchURITransformMSCert_LEN );
const DRM_DISCARDABLE DRM_ANSI_CONST_STRING  g_dastrURITransformMSCertColl              = CREATE_DRM_ANSI_STRING_EX( g_rgchURITransformMSCertColl, g_rgchURITransformMSCertColl_LEN );
const DRM_DISCARDABLE DRM_ANSI_CONST_STRING  g_dastrURITransformMSCert_Old              = CREATE_DRM_ANSI_STRING_EX( g_rgchURITransformMSCert_Old, g_rgchURITransformMSCert_Old_LEN );
const DRM_DISCARDABLE DRM_ANSI_CONST_STRING  g_dastrURITransformC14N                    = CREATE_DRM_ANSI_STRING_EX( g_rgchURITransformC14N, g_rgchURITransformC14N_LEN );
const DRM_DISCARDABLE DRM_ANSI_CONST_STRING  g_dastrTagWMDRMCertificate                 = CREATE_DRM_ANSI_STRING_EX( g_rgchTagWMDRMCertificate, g_rgchTagWMDRMCertificate_LEN );
const DRM_DISCARDABLE DRM_ANSI_CONST_STRING  g_dastrTagWMDRMData                        = CREATE_DRM_ANSI_STRING_EX( g_rgchTagDataWMDRM, g_rgchTagDataWMDRM_LEN );
const DRM_DISCARDABLE DRM_ANSI_CONST_STRING  g_dastrTagWMDRMCertPublicKey               = CREATE_DRM_ANSI_STRING_EX( g_rgchTagWMDRMCertPublicKey, g_rgchTagWMDRMCertPublicKey_LEN );
const DRM_DISCARDABLE DRM_ANSI_CONST_STRING  g_dastrTagWMDRMCertSecurityVersion         = CREATE_DRM_ANSI_STRING_EX( g_rgchTagWMDRMCertSecurityVersion, g_rgchTagWMDRMCertSecurityVersion_LEN );
const DRM_DISCARDABLE DRM_ANSI_CONST_STRING  g_dastrTagWMDRMCertSecurityLevel           = CREATE_DRM_ANSI_STRING_EX( g_rgchTagWMDRMCertSecurityLevel, g_rgchTagWMDRMCertSecurityLevel_LEN );
const DRM_DISCARDABLE DRM_ANSI_CONST_STRING  g_dastrTagWMDRMCertFeatures                = CREATE_DRM_ANSI_STRING_EX( g_rgchTagWMDRMCertFeatures, g_rgchTagWMDRMCertFeatures_LEN );
const DRM_DISCARDABLE DRM_ANSI_CONST_STRING  g_dastrTagWMDRMCertKeyUsage                = CREATE_DRM_ANSI_STRING_EX( g_rgchTagWMDRMCertKeyUsage, g_rgchTagWMDRMCertKeyUsage_LEN );
const DRM_DISCARDABLE DRM_ANSI_CONST_STRING  g_dastrFeaturesWMDRMTransmitter            = CREATE_DRM_ANSI_STRING_EX( g_rgchFeaturesWMDRMTransmitter, g_rgchFeaturesWMDRMTransmitter_LEN);
const DRM_DISCARDABLE DRM_ANSI_CONST_STRING  g_dastrFeaturesWMDRMReceiver               = CREATE_DRM_ANSI_STRING_EX( g_rgchFeaturesWMDRMReceiver, g_rgchFeaturesWMDRMReceiver_LEN);
const DRM_DISCARDABLE DRM_ANSI_CONST_STRING  g_dastrTagSignature                        = CREATE_DRM_ANSI_STRING_EX( g_rgchTagSignature, g_rgchTagSignature_LEN );
const DRM_DISCARDABLE DRM_ANSI_CONST_STRING  g_dastrTagSignatureMethod                  = CREATE_DRM_ANSI_STRING_EX( g_rgchTagSignatureMethod, g_rgchTagSignatureMethod_LEN );
const DRM_DISCARDABLE DRM_ANSI_CONST_STRING  g_dastrTagSignedInfo                       = CREATE_DRM_ANSI_STRING_EX( g_rgchTagSignedInfo, g_rgchTagSignedInfo_LEN );
const DRM_DISCARDABLE DRM_ANSI_CONST_STRING  g_dastrURIC14N                             = CREATE_DRM_ANSI_STRING_EX( g_rgchURIC14N, g_rgchURIC14N_LEN );
const DRM_DISCARDABLE DRM_ANSI_CONST_STRING  g_dastrVersionWMDRM                        = CREATE_DRM_ANSI_STRING_EX( g_rgchVersionWMDRM, g_rgchVersionWMDRM_LEN );
const DRM_DISCARDABLE DRM_ANSI_CONST_STRING  g_dastrWMDRMCertExponent                   = CREATE_DRM_ANSI_STRING_EX( g_rgchWMDRMCertExponent, g_rgchWMDRMCertExponent_LEN );
const DRM_DISCARDABLE DRM_ANSI_CONST_STRING  g_dastrPrefixMicrosoftCert                 = CREATE_DRM_ANSI_STRING_EX( g_rgchPrefixMicrosoftCert, g_rgchPrefixMicrosoftCert_LEN );
const DRM_DISCARDABLE DRM_ANSI_CONST_STRING  g_dastrMSNDRootPubKeyB64                   = CREATE_DRM_ANSI_STRING_EX( g_rgchMSNDRootPubKeyB64, g_rgchMSNDRootPubKeyB64_LEN );

/* Well-known public Keys */
const DRM_DISCARDABLE DRM_BYTE              g_rgbMSPlayReadyRootIssuerPubKey[] =
{
     0x86, 0x4D, 0x61, 0xCF, 0xF2, 0x25, 0x6E, 0x42, 0x2C, 0x56, 0x8B, 0x3C, 0x28, 0x00, 0x1C, 0xFB,
     0x3E, 0x15, 0x27, 0x65, 0x85, 0x84, 0xBA, 0x05, 0x21, 0xB7, 0x9B, 0x18, 0x28, 0xD9, 0x36, 0xDE,
     0x1D, 0x82, 0x6A, 0x8F, 0xC3, 0xE6, 0xE7, 0xFA, 0x7A, 0x90, 0xD5, 0xCA, 0x29, 0x46, 0xF1, 0xF6,
     0x4A, 0x2E, 0xFB, 0x9F, 0x5D, 0xCF, 0xFE, 0x7E, 0x43, 0x4E, 0xB4, 0x42, 0x93, 0xFA, 0xC5, 0xAB
};

/* Revocation GUIDs */
DRM_DEFINE_GUID( g_guidRevocationTypeRevInfo,
                 0xCCDE5A55, 0xA688, 0x4405, 0xA8, 0x8B, 0xD1, 0x3F, 0x90, 0xD5, 0xBA, 0x3E );

/*
** This GUID is for the PlayReady REVINFO2
** {52D1FF11-D388-4edd-82B7-68EA4C20A16C}
Note that this GUID will also be present in the WMDRM rev-info, in order to
identify the minimum PlayReady Revinfo(V2) version in the WMDRM Revinfo (V1).
This is required to export from WMDRM to PlayReady
*/
DRM_DEFINE_GUID( g_guidRevocationTypeRevInfo2,
                 0x52D1FF11, 0xD388, 0x4EDD, 0x82, 0xB7, 0x68, 0xEA, 0x4C, 0x20, 0xA1, 0x6C );

DRM_DEFINE_GUID( g_guidRevocationTypeWMDRMNET,
                 0xCD75E604, 0x543D, 0x4A9C, 0x9F, 0x09, 0xFE, 0x6D, 0x24, 0xE8, 0xBF, 0x90 );

DRM_DEFINE_GUID( g_guidRevocationTypeDevice,
                 0x3129E375, 0xCEB0, 0x47D5, 0x9C, 0xCA, 0x9D, 0xB7, 0x4C, 0xFD, 0x43, 0x32 );

DRM_DEFINE_GUID( g_guidRevocationTypeApp,
                 0x90A37313, 0x0ECF, 0x4CAA, 0xA9, 0x06, 0xB1, 0x88, 0xF6, 0x12, 0x93, 0x00 );

DRM_DEFINE_GUID( g_guidRevocationTypePlayReadySilverLightRuntime,
                 0x4E9D8C8A, 0xB652, 0x45A7, 0x97, 0x91, 0x69, 0x25, 0xA6, 0xB4, 0x79, 0x1F );

DRM_DEFINE_GUID( g_guidRevocationTypePlayReadySilverLightApplication,
                 0x28082E80, 0xC7A3, 0x40b1, 0x82, 0x56, 0x19, 0xE5, 0xB6, 0xD8, 0x9B, 0x27 );

/* {A2190240-B2CA-40B3-B48D-9BC4C2DC428D} */
DRM_DEFINE_GUID( g_guidRevocationTypeGRL,
                 0xA2190240, 0xB2CA, 0x40B3, 0xB4, 0x8D, 0x9B, 0xC4, 0xC2, 0xDC, 0x42, 0x8D );

/* Actions GUIDs. Used with DRM_LEVL_PerformOperationsXMR() API */
DRM_DEFINE_GUID( DRM_ACTION_PLAY,
                 0x4C3FC9B3, 0x31C2, 0x4FD4, 0x82, 0x4A, 0x04, 0xD4, 0x23, 0x41, 0xA9, 0xD3 );

DRM_DEFINE_GUID( DRM_ACTION_COPY,
                 0xD8C5502C, 0x41B1, 0x4681, 0x8B, 0x61, 0x8A, 0x16, 0x18, 0xA3, 0x1D, 0xA7 );

DRM_DEFINE_GUID( DRM_ACTION_MOVE,
                 0xF73D6BFB, 0x9E70, 0x47FA, 0xAD, 0x1F, 0xEC, 0x15, 0x0F, 0x6C, 0x9A, 0x16 );

DRM_DEFINE_GUID( DRM_ACTION_CREATE_THUMBNAIL,
                 0x8CC2C885, 0xBB0D, 0x4AA2, 0xA3, 0x58, 0x96, 0x37, 0xBB, 0x35, 0x59, 0xA9 );

DRM_DEFINE_GUID( DRM_ACTION_COLLABORATIVE_PLAY,
                 0x6B6B0837, 0x46A4, 0x4015, 0x84, 0xE3, 0x8C, 0x20, 0xB5, 0xA7, 0xCD, 0x3A );

DRM_DEFINE_GUID( DRM_ACTION_COPY_TO_CD,
                 0xEC930B7D, 0x1F2D, 0x4682, 0xA3, 0x8B, 0x8A, 0xB9, 0x77, 0x72, 0x1D, 0x0D );

DRM_DEFINE_GUID( DRM_ACTION_COPY_TO_PC,
                 0xCE480EDE, 0x516B, 0x40B3, 0x90, 0xE1, 0xD6, 0xCF, 0xC4, 0x76, 0x30, 0xC5 );

DRM_DEFINE_GUID( DRM_ACTION_COPY_TO_DEVICE,
                 0x6848955D, 0x516B, 0x4EB0, 0x90, 0xE8, 0x8F, 0x6D, 0x5A, 0x77, 0xB8, 0x5F );

DRM_DEFINE_GUID( DRM_PR_PROTECTION_SYSTEM_ID,
                 0x9A04F079, 0x9840, 0x4286, 0xAB, 0x92, 0xE6, 0x5B, 0xE0, 0x88, 0x5F, 0x95 );

DRM_DEFINE_GUID( g_guidNull,
                 0x00000000, 0x0000, 0x0000, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 );

/*
** PlayEnabler GUID constants
*/
DRM_DEFINE_GUID( DRM_PLAYENABLER_UNKNOWN_OUTPUT,
                 0x786627D8, 0xC2A6, 0x44BE, 0x8F, 0x88, 0x08, 0xAE, 0x25, 0x5B, 0x01, 0xA7 );

DRM_DEFINE_GUID( DRM_PLAYENABLER_CONSTRAINED_RESOLUTION_UNKNOWN_OUTPUT,
                 0xB621D91F, 0xEDCC, 0x4035, 0x8D, 0x4B, 0xDC, 0x71, 0x76, 0x0D, 0x43, 0xE9 );


/* Versioning constants */
/* Indicates the public root key needed to verify the license server certificates. */
const DRM_DISCARDABLE DRM_BYTE              CERT_VER [__CB_DECL(VERSION_LEN)]          =
{
    0, 1, 0, 0
};

/* The version for client id. */
const DRM_DISCARDABLE DRM_BYTE              CLIENT_ID_VER [__CB_DECL(VERSION_LEN)]      =
{
    2, 0, 0, 0
};

EXIT_PK_NAMESPACE_CODE;


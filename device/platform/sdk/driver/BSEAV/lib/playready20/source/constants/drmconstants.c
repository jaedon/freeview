/**@@@+++@@@@******************************************************************
**
** Microsoft (r) PlayReady (r)
** Copyright (c) Microsoft Corporation. All rights reserved.
**
***@@@---@@@@******************************************************************
*/
#include <drmcommon.h>
#include <drmlicense.h>

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

/* Misc strings shared across disparate functional areas */
const                DRM_CHAR       g_rgchTagSLK                                 [] = "SLK";
const                DRM_CHAR       g_rgchTagSLKID                               [] = "SLKID";
const                DRM_CHAR       g_rgchTagSLKDATA                             [] = "SLKDATA";
const                DRM_WCHAR      g_rgwchXPathSymSig                           [] =  { ONE_WCHAR('L', '\0'),  ONE_WCHAR('I', '\0'),  ONE_WCHAR('C', '\0'),  ONE_WCHAR('E', '\0'),  ONE_WCHAR('N', '\0'),  ONE_WCHAR('S', '\0'),  ONE_WCHAR('O', '\0'),  ONE_WCHAR('R', '\0'),  ONE_WCHAR('I', '\0'),  ONE_WCHAR('N', '\0'),  ONE_WCHAR('F', '\0'),  ONE_WCHAR('O', '\0'),  ONE_WCHAR('/', '\0'),  ONE_WCHAR('S', '\0'),  ONE_WCHAR('Y', '\0'),  ONE_WCHAR('M', '\0'),  ONE_WCHAR('S', '\0'),  ONE_WCHAR('I', '\0'),  ONE_WCHAR('G', '\0'),  ONE_WCHAR('N', '\0'),  ONE_WCHAR('A', '\0'),  ONE_WCHAR('T', '\0'),  ONE_WCHAR('U', '\0'),  ONE_WCHAR('R', '\0'),  ONE_WCHAR('E', '\0'),  ONE_WCHAR('\0', '\0')};
const                DRM_CHAR       g_rgchLicenseRespTag                         [] = "LICENSERESPONSE"; /* ODD count */
const                DRM_WCHAR      g_rgwchGUID                                  [] =  { ONE_WCHAR('G', '\0'), ONE_WCHAR('U', '\0'), ONE_WCHAR('I', '\0'), ONE_WCHAR('D', '\0'), ONE_WCHAR('\0', '\0') };
const                DRM_WCHAR      g_rgwchFlag                                  [] =  { ONE_WCHAR('F', '\0'),  ONE_WCHAR('L', '\0'),  ONE_WCHAR('A', '\0'),  ONE_WCHAR('G', '\0'),  ONE_WCHAR('\0', '\0')};
const                DRM_WCHAR      g_rgwchSecClockNotSet                        [] =  { ONE_WCHAR('D', '\0'),  ONE_WCHAR('R', '\0'),  ONE_WCHAR('M', '\0'),  ONE_WCHAR('_', '\0'),  ONE_WCHAR('C', '\0'),  ONE_WCHAR('L', '\0'),  ONE_WCHAR('K', '\0'),  ONE_WCHAR('_', '\0'),  ONE_WCHAR('N', '\0'),  ONE_WCHAR('O', '\0'),  ONE_WCHAR('T', '\0'),  ONE_WCHAR('_', '\0'),  ONE_WCHAR('S', '\0'),  ONE_WCHAR('E', '\0'),  ONE_WCHAR('T', '\0'),  ONE_WCHAR('\0', '\0')};
const                DRM_WCHAR      g_rgwchSecClockSet                           [] =  { ONE_WCHAR('D', '\0'),  ONE_WCHAR('R', '\0'),  ONE_WCHAR('M', '\0'),  ONE_WCHAR('_', '\0'),  ONE_WCHAR('C', '\0'),  ONE_WCHAR('L', '\0'),  ONE_WCHAR('K', '\0'),  ONE_WCHAR('_', '\0'),  ONE_WCHAR('S', '\0'),  ONE_WCHAR('E', '\0'),  ONE_WCHAR('T', '\0'),  ONE_WCHAR('\0', '\0')};
const                DRM_WCHAR      g_rgwchSecClockNeedsRefresh                  [] =  { ONE_WCHAR('D', '\0'),  ONE_WCHAR('R', '\0'),  ONE_WCHAR('M', '\0'),  ONE_WCHAR('_', '\0'),  ONE_WCHAR('C', '\0'),  ONE_WCHAR('L', '\0'),  ONE_WCHAR('K', '\0'),  ONE_WCHAR('_', '\0'),  ONE_WCHAR('N', '\0'),  ONE_WCHAR('E', '\0'),  ONE_WCHAR('E', '\0'),  ONE_WCHAR('D', '\0'),  ONE_WCHAR('S', '\0'),  ONE_WCHAR('_', '\0'),  ONE_WCHAR('R', '\0'),  ONE_WCHAR('E', '\0'),  ONE_WCHAR('F', '\0'),  ONE_WCHAR('R', '\0'),  ONE_WCHAR('E', '\0'),  ONE_WCHAR('S', '\0'),  ONE_WCHAR('H', '\0'),  ONE_WCHAR('\0', '\0')};
const                DRM_WCHAR      g_rgwchExprVarSavedDateTime                  [] =  { ONE_WCHAR('s', '\0'),  ONE_WCHAR('a', '\0'),  ONE_WCHAR('v', '\0'),  ONE_WCHAR('e', '\0'),  ONE_WCHAR('d', '\0'),  ONE_WCHAR('d', '\0'),  ONE_WCHAR('a', '\0'),  ONE_WCHAR('t', '\0'),  ONE_WCHAR('e', '\0'),  ONE_WCHAR('t', '\0'),  ONE_WCHAR('i', '\0'),  ONE_WCHAR('m', '\0'),  ONE_WCHAR('e', '\0'),  ONE_WCHAR('\0', '\0')};
const                DRM_WCHAR      g_rgwchInitialExtensibleStateAttributeName   [] =  { ONE_WCHAR('I', '\0'), ONE_WCHAR('n', '\0'), ONE_WCHAR('i', '\0'), ONE_WCHAR('t', '\0'), ONE_WCHAR('i', '\0'), ONE_WCHAR('a', '\0'), ONE_WCHAR('l', '\0'), ONE_WCHAR('E', '\0'), ONE_WCHAR('x', '\0'), ONE_WCHAR('t', '\0'), ONE_WCHAR('R', '\0'), ONE_WCHAR('e', '\0'), ONE_WCHAR('s', '\0'), ONE_WCHAR('t', '\0'), ONE_WCHAR('S', '\0'), ONE_WCHAR('t', '\0'), ONE_WCHAR('a', '\0'), ONE_WCHAR('t', '\0'), ONE_WCHAR('e', '\0'), ONE_WCHAR('\0', '\0') };
const                DRM_WCHAR      g_rgwchTagMetering                           [] =  { ONE_WCHAR('M', '\0'),  ONE_WCHAR('E', '\0'),  ONE_WCHAR('T', '\0'),  ONE_WCHAR('E', '\0'),  ONE_WCHAR('R', '\0'),  ONE_WCHAR('D', '\0'),  ONE_WCHAR('A', '\0'),  ONE_WCHAR('T', '\0'),  ONE_WCHAR('A', '\0'),  ONE_WCHAR('\0', '\0')};
const                DRM_WCHAR      g_rgwchTagHash                               [] =  { ONE_WCHAR('H', '\0'),  ONE_WCHAR('A', '\0'),  ONE_WCHAR('S', '\0'),  ONE_WCHAR('H', '\0'),  ONE_WCHAR('\0', '\0')};
const                DRM_WCHAR      g_rgwchTagCmd                                [] =  { ONE_WCHAR('C', '\0'),  ONE_WCHAR('O', '\0'),  ONE_WCHAR('M', '\0'),  ONE_WCHAR('M', '\0'),  ONE_WCHAR('A', '\0'),  ONE_WCHAR('N', '\0'),  ONE_WCHAR('D', '\0'),  ONE_WCHAR('\0', '\0')};
const                DRM_WCHAR      g_rgwchTagPartialData                        [] =  { ONE_WCHAR('P', '\0'),  ONE_WCHAR('A', '\0'),  ONE_WCHAR('R', '\0'),  ONE_WCHAR('T', '\0'),  ONE_WCHAR('I', '\0'),  ONE_WCHAR('A', '\0'),  ONE_WCHAR('L', '\0'),  ONE_WCHAR('D', '\0'),  ONE_WCHAR('A', '\0'),  ONE_WCHAR('T', '\0'),  ONE_WCHAR('A', '\0'),  ONE_WCHAR('\0', '\0')};
const                DRM_WCHAR      g_rgwchHMAC                                  [] =  { ONE_WCHAR('H', '\0'),  ONE_WCHAR('M', '\0'),  ONE_WCHAR('A', '\0'),  ONE_WCHAR('C', '\0'),  ONE_WCHAR('\0', '\0')};
const                DRM_WCHAR      g_rgwch1                                     [] =  { ONE_WCHAR('1', '\0'),  ONE_WCHAR('\0', '\0')};
const                DRM_WCHAR      g_rgwchReset                                 [] =  { ONE_WCHAR('R', '\0'),  ONE_WCHAR('E', '\0'),  ONE_WCHAR('S', '\0'),  ONE_WCHAR('E', '\0'),  ONE_WCHAR('T', '\0'),  ONE_WCHAR('\0', '\0')};
const                DRM_WCHAR      g_rgwchLicense                               [] =  { ONE_WCHAR('L', '\0'), ONE_WCHAR('I', '\0'), ONE_WCHAR('C', '\0'), ONE_WCHAR('E', '\0'), ONE_WCHAR('N', '\0'), ONE_WCHAR('S', '\0'), ONE_WCHAR('E', '\0'), ONE_WCHAR('\0', '\0')};
const                DRM_WCHAR      g_rgwchTagRevocation                         [] =  { ONE_WCHAR('R', '\0'), ONE_WCHAR('E', '\0'), ONE_WCHAR('V', '\0'), ONE_WCHAR('O', '\0'), ONE_WCHAR('C', '\0'), ONE_WCHAR('A', '\0'), ONE_WCHAR('T', '\0'), ONE_WCHAR('I', '\0'), ONE_WCHAR('O', '\0'), ONE_WCHAR('N', '\0'), ONE_WCHAR('\0', '\0')};

/* Rights */
const                DRM_WCHAR      g_rgwchWMDRM_RIGHT_NONE                      [] =  { ONE_WCHAR('N', '\0'),  ONE_WCHAR('o', '\0'),  ONE_WCHAR('n', '\0'),  ONE_WCHAR('e', '\0'),  ONE_WCHAR('\0', '\0')};
const                DRM_WCHAR      g_rgwchWMDRM_RIGHT_PLAYBACK                  [] =  { ONE_WCHAR('P', '\0'),  ONE_WCHAR('l', '\0'),  ONE_WCHAR('a', '\0'),  ONE_WCHAR('y', '\0'),  ONE_WCHAR('\0', '\0')};
const                DRM_WCHAR      g_rgwchWMDRM_RIGHT_COLLABORATIVE_PLAY        [] =  { ONE_WCHAR('C', '\0'),  ONE_WCHAR('o', '\0'),  ONE_WCHAR('l', '\0'),  ONE_WCHAR('l', '\0'),  ONE_WCHAR('a', '\0'),  ONE_WCHAR('b', '\0'),  ONE_WCHAR('o', '\0'),  ONE_WCHAR('r', '\0'),  ONE_WCHAR('a', '\0'),  ONE_WCHAR('t', '\0'),  ONE_WCHAR('i', '\0'),  ONE_WCHAR('v', '\0'),  ONE_WCHAR('e', '\0'),  ONE_WCHAR('P', '\0'),  ONE_WCHAR('l', '\0'),  ONE_WCHAR('a', '\0'),  ONE_WCHAR('y', '\0'),  ONE_WCHAR('\0', '\0')};
const                DRM_WCHAR      g_rgwchWMDRM_RIGHT_COPY_TO_CD                [] =  { ONE_WCHAR('P', '\0'),  ONE_WCHAR('r', '\0'),  ONE_WCHAR('i', '\0'),  ONE_WCHAR('n', '\0'),  ONE_WCHAR('t', '\0'),  ONE_WCHAR('.', '\0'),  ONE_WCHAR('r', '\0'),  ONE_WCHAR('e', '\0'),  ONE_WCHAR('d', '\0'),  ONE_WCHAR('b', '\0'),  ONE_WCHAR('o', '\0'),  ONE_WCHAR('o', '\0'),  ONE_WCHAR('k', '\0'),  ONE_WCHAR('\0', '\0')};
const                DRM_WCHAR      g_rgwchWMDRM_RIGHT_COPY                      [] =  { ONE_WCHAR('C', '\0'),  ONE_WCHAR('o', '\0'),  ONE_WCHAR('p', '\0'),  ONE_WCHAR('y', '\0'),  ONE_WCHAR('\0', '\0')};
const                DRM_WCHAR      g_rgwchWMDRM_RIGHT_CREATE_THUMBNAIL_IMAGE    [] =  { ONE_WCHAR('C', '\0'),  ONE_WCHAR('r', '\0'),  ONE_WCHAR('e', '\0'),  ONE_WCHAR('a', '\0'),  ONE_WCHAR('t', '\0'),  ONE_WCHAR('e', '\0'),  ONE_WCHAR('T', '\0'),  ONE_WCHAR('h', '\0'),  ONE_WCHAR('u', '\0'),  ONE_WCHAR('m', '\0'),  ONE_WCHAR('b', '\0'),  ONE_WCHAR('n', '\0'),  ONE_WCHAR('a', '\0'),  ONE_WCHAR('i', '\0'),  ONE_WCHAR('l', '\0'),  ONE_WCHAR('I', '\0'),  ONE_WCHAR('m', '\0'),  ONE_WCHAR('a', '\0'),  ONE_WCHAR('g', '\0'),  ONE_WCHAR('e', '\0'),  ONE_WCHAR('\0', '\0') };  /* L"CreateThumbnailImage"; */
const                DRM_WCHAR      g_rgwchWMDRM_RIGHT_MOVE                      [] =  { ONE_WCHAR('M', '\0'),  ONE_WCHAR('o', '\0'),  ONE_WCHAR('v', '\0'),  ONE_WCHAR('e', '\0'),  ONE_WCHAR('\0', '\0')};

/* Script varibles used for license properties. */
const                DRM_WCHAR      g_rgwchDRM_LS_BEGIN_ATTR                     [] =  { ONE_WCHAR('B', '\0'),  ONE_WCHAR('e', '\0'),  ONE_WCHAR('g', '\0'),  ONE_WCHAR('i', '\0'),  ONE_WCHAR('n', '\0'),  ONE_WCHAR('\0', '\0')};
const                DRM_WCHAR      g_rgwchDRM_LS_COUNT_ATTR                     [] =  { ONE_WCHAR('C', '\0'),  ONE_WCHAR('o', '\0'),  ONE_WCHAR('u', '\0'),  ONE_WCHAR('n', '\0'),  ONE_WCHAR('t', '\0'),  ONE_WCHAR('\0', '\0')};
const                DRM_WCHAR      g_rgwchDRM_LS_BEGDATE_ATTR                   [] =  { ONE_WCHAR('B', '\0'),  ONE_WCHAR('e', '\0'),  ONE_WCHAR('g', '\0'),  ONE_WCHAR('i', '\0'),  ONE_WCHAR('n', '\0'),  ONE_WCHAR('D', '\0'),  ONE_WCHAR('a', '\0'),  ONE_WCHAR('t', '\0'),  ONE_WCHAR('e', '\0'),  ONE_WCHAR('\0', '\0')};
const                DRM_WCHAR      g_rgwchDRM_LS_ENDDATE_ATTR                   [] =  { ONE_WCHAR('E', '\0'),  ONE_WCHAR('x', '\0'),  ONE_WCHAR('p', '\0'),  ONE_WCHAR('i', '\0'),  ONE_WCHAR('r', '\0'),  ONE_WCHAR('a', '\0'),  ONE_WCHAR('t', '\0'),  ONE_WCHAR('i', '\0'),  ONE_WCHAR('o', '\0'),  ONE_WCHAR('n', '\0'),  ONE_WCHAR('D', '\0'),  ONE_WCHAR('a', '\0'),  ONE_WCHAR('t', '\0'),  ONE_WCHAR('e', '\0'),  ONE_WCHAR('\0', '\0')};
const                DRM_WCHAR      g_rgwchDRM_LS_FIRSTUSE_ATTR                  [] =  { ONE_WCHAR('F', '\0'),  ONE_WCHAR('i', '\0'),  ONE_WCHAR('r', '\0'),  ONE_WCHAR('s', '\0'),  ONE_WCHAR('t', '\0'),  ONE_WCHAR('U', '\0'),  ONE_WCHAR('s', '\0'),  ONE_WCHAR('e', '\0'),  ONE_WCHAR('D', '\0'),  ONE_WCHAR('a', '\0'),  ONE_WCHAR('t', '\0'),  ONE_WCHAR('e', '\0'),  ONE_WCHAR('\0', '\0')};
const                DRM_WCHAR      g_rgwchDRM_LS_FIRSTSTORE_ATTR                [] =  { ONE_WCHAR('F', '\0'),  ONE_WCHAR('i', '\0'),  ONE_WCHAR('r', '\0'),  ONE_WCHAR('s', '\0'),  ONE_WCHAR('t', '\0'),  ONE_WCHAR('S', '\0'),  ONE_WCHAR('t', '\0'),  ONE_WCHAR('o', '\0'),  ONE_WCHAR('r', '\0'),  ONE_WCHAR('e', '\0'),  ONE_WCHAR('D', '\0'),  ONE_WCHAR('a', '\0'),  ONE_WCHAR('t', '\0'),  ONE_WCHAR('e', '\0'),  ONE_WCHAR('\0', '\0')};
const                DRM_WCHAR      g_rgwchDRM_LS_APPSEC_ATTR                    [] =  { ONE_WCHAR('A', '\0'),  ONE_WCHAR('p', '\0'),  ONE_WCHAR('p', '\0'),  ONE_WCHAR('s', '\0'),  ONE_WCHAR('e', '\0'),  ONE_WCHAR('c', '\0'),  ONE_WCHAR('\0', '\0')};
const                DRM_WCHAR      g_rgwchDRM_LS_PLAYCOUNT_ATTR                 [] =  { ONE_WCHAR('P', '\0'),  ONE_WCHAR('l', '\0'),  ONE_WCHAR('a', '\0'),  ONE_WCHAR('y', '\0'),  ONE_WCHAR('C', '\0'),  ONE_WCHAR('o', '\0'),  ONE_WCHAR('u', '\0'),  ONE_WCHAR('n', '\0'),  ONE_WCHAR('t', '\0'),  ONE_WCHAR('\0', '\0')};
const                DRM_WCHAR      g_rgwchDRM_LS_COPYCOUNT_ATTR                 [] =  { ONE_WCHAR('C', '\0'),  ONE_WCHAR('o', '\0'),  ONE_WCHAR('p', '\0'),  ONE_WCHAR('y', '\0'),  ONE_WCHAR('C', '\0'),  ONE_WCHAR('o', '\0'),  ONE_WCHAR('u', '\0'),  ONE_WCHAR('n', '\0'),  ONE_WCHAR('t', '\0'),  ONE_WCHAR('\0', '\0')};
const                DRM_WCHAR      g_rgwchDRM_LS_DELETED_ATTR                   [] =  { ONE_WCHAR('d', '\0'),  ONE_WCHAR('e', '\0'),  ONE_WCHAR('l', '\0'),  ONE_WCHAR('e', '\0'),  ONE_WCHAR('t', '\0'),  ONE_WCHAR('e', '\0'),  ONE_WCHAR('d', '\0'),  ONE_WCHAR('\0', '\0') };  /*L"deleted";*/

/* Shared XML tags */
const                DRM_WCHAR      g_rgwchResponse                              [] =  { ONE_WCHAR('r', '\0'),  ONE_WCHAR('e', '\0'),  ONE_WCHAR('s', '\0'),  ONE_WCHAR('p', '\0'),  ONE_WCHAR('o', '\0'),  ONE_WCHAR('n', '\0'),  ONE_WCHAR('s', '\0'),  ONE_WCHAR('e', '\0'),  ONE_WCHAR('\0', '\0')};
const                DRM_WCHAR      g_rgwchAttributeVersion                      [] =  { ONE_WCHAR('v', '\0'),  ONE_WCHAR('e', '\0'),  ONE_WCHAR('r', '\0'),  ONE_WCHAR('s', '\0'),  ONE_WCHAR('i', '\0'),  ONE_WCHAR('o', '\0'),  ONE_WCHAR('n', '\0'),  ONE_WCHAR('\0', '\0')};
const                DRM_WCHAR      g_rgwchTagData                               [] =  { ONE_WCHAR('D', '\0'),  ONE_WCHAR('A', '\0'),  ONE_WCHAR('T', '\0'),  ONE_WCHAR('A', '\0'),  ONE_WCHAR('\0', '\0')};
const                DRM_WCHAR      g_rgwchTagIndex                              [] =  { ONE_WCHAR('I', '\0'),  ONE_WCHAR('N', '\0'),  ONE_WCHAR('D', '\0'),  ONE_WCHAR('E', '\0'),  ONE_WCHAR('X', '\0'),  ONE_WCHAR('\0', '\0')};
const                DRM_WCHAR      g_rgwchTagSymSig                             [] =  { ONE_WCHAR('S', '\0'),  ONE_WCHAR('Y', '\0'),  ONE_WCHAR('M', '\0'),  ONE_WCHAR('S', '\0'),  ONE_WCHAR('I', '\0'),  ONE_WCHAR('G', '\0'),  ONE_WCHAR('N', '\0'),  ONE_WCHAR('A', '\0'),  ONE_WCHAR('T', '\0'),  ONE_WCHAR('U', '\0'),  ONE_WCHAR('R', '\0'),  ONE_WCHAR('E', '\0'),  ONE_WCHAR('\0', '\0')};
const                DRM_WCHAR      g_rgwchTagPubkey                             [] =  { ONE_WCHAR('P', '\0'),  ONE_WCHAR('U', '\0'),  ONE_WCHAR('B', '\0'),  ONE_WCHAR('K', '\0'),  ONE_WCHAR('E', '\0'),  ONE_WCHAR('Y', '\0'),  ONE_WCHAR('\0', '\0')};
const                DRM_WCHAR      g_rgwchTagPRIVKEY                            [] =  { ONE_WCHAR('P', '\0'),  ONE_WCHAR('R', '\0'),  ONE_WCHAR('I', '\0'),  ONE_WCHAR('V', '\0'),  ONE_WCHAR('K', '\0'),  ONE_WCHAR('E', '\0'),   ONE_WCHAR('Y', '\0'), ONE_WCHAR('\0', '\0') };
const                DRM_WCHAR      g_rgwchAttributePubkey                       [] =  { ONE_WCHAR('p', '\0'),  ONE_WCHAR('u', '\0'),  ONE_WCHAR('b', '\0'),  ONE_WCHAR('k', '\0'),  ONE_WCHAR('e', '\0'),  ONE_WCHAR('y', '\0'),  ONE_WCHAR('\0', '\0')};
const                DRM_WCHAR      g_rgwchAttributeVerifiedCK                   [] =  { ONE_WCHAR('I', '\0'),  ONE_WCHAR('D', '\0'),  ONE_WCHAR('\0', '\0')};
const                DRM_WCHAR      g_rgwchTagValue                              [] =  { ONE_WCHAR('V', '\0'),  ONE_WCHAR('A', '\0'),  ONE_WCHAR('L', '\0'),  ONE_WCHAR('U', '\0'),  ONE_WCHAR('E', '\0'),  ONE_WCHAR('\0', '\0')};
const                DRM_WCHAR      g_rgwchTagSignature                          [] =  { ONE_WCHAR('S', '\0'),  ONE_WCHAR('I', '\0'),  ONE_WCHAR('G', '\0'),  ONE_WCHAR('N', '\0'),  ONE_WCHAR('A', '\0'),  ONE_WCHAR('T', '\0'),  ONE_WCHAR('U', '\0'),  ONE_WCHAR('R', '\0'),  ONE_WCHAR('E', '\0'),  ONE_WCHAR('\0', '\0')};
const                DRM_WCHAR      g_rgwchTagHashAlg                            [] =  { ONE_WCHAR('H', '\0'),  ONE_WCHAR('A', '\0'),  ONE_WCHAR('S', '\0'),  ONE_WCHAR('H', '\0'),  ONE_WCHAR('A', '\0'),  ONE_WCHAR('L', '\0'),  ONE_WCHAR('G', '\0'),  ONE_WCHAR('O', '\0'),  ONE_WCHAR('R', '\0'),  ONE_WCHAR('I', '\0'),  ONE_WCHAR('T', '\0'),  ONE_WCHAR('H', '\0'),  ONE_WCHAR('M', '\0'),  ONE_WCHAR('\0', '\0')};
const                DRM_WCHAR      g_rgwchTagSignAlg                            [] =  { ONE_WCHAR('S', '\0'),  ONE_WCHAR('I', '\0'),  ONE_WCHAR('G', '\0'),  ONE_WCHAR('N', '\0'),  ONE_WCHAR('A', '\0'),  ONE_WCHAR('L', '\0'),  ONE_WCHAR('G', '\0'),  ONE_WCHAR('O', '\0'),  ONE_WCHAR('R', '\0'),  ONE_WCHAR('I', '\0'),  ONE_WCHAR('T', '\0'),  ONE_WCHAR('H', '\0'),  ONE_WCHAR('M', '\0'),  ONE_WCHAR('\0', '\0')};
const                DRM_WCHAR      g_rgwchSHA                                   [] =  { ONE_WCHAR('S', '\0'),  ONE_WCHAR('H', '\0'),  ONE_WCHAR('A', '\0'),  ONE_WCHAR('\0', '\0')};
const                DRM_WCHAR      g_rgwchMSDRM                                 [] =  { ONE_WCHAR('M', '\0'),  ONE_WCHAR('S', '\0'),  ONE_WCHAR('D', '\0'),  ONE_WCHAR('R', '\0'),  ONE_WCHAR('M', '\0'),  ONE_WCHAR('\0', '\0')};
const                DRM_CHAR       g_rgchAttributeType                          [] = "type";
const                DRM_WCHAR      g_rgwchAttributeType                         [] =  { ONE_WCHAR('t', '\0'),  ONE_WCHAR('y', '\0'),  ONE_WCHAR('p', '\0'),  ONE_WCHAR('e', '\0'),  ONE_WCHAR('\0', '\0')};
const                DRM_WCHAR      g_rgwchTagCertificate                        [] =  { ONE_WCHAR('C', '\0'),  ONE_WCHAR('E', '\0'),  ONE_WCHAR('R', '\0'),  ONE_WCHAR('T', '\0'),  ONE_WCHAR('I', '\0'),  ONE_WCHAR('F', '\0'),  ONE_WCHAR('I', '\0'),  ONE_WCHAR('C', '\0'),  ONE_WCHAR('A', '\0'),  ONE_WCHAR('T', '\0'),  ONE_WCHAR('E', '\0'),  ONE_WCHAR('\0', '\0')};
const                DRM_WCHAR      g_rgwchTagDevCert                            [] =  { ONE_WCHAR('D', '\0'),  ONE_WCHAR('E', '\0'),  ONE_WCHAR('V', '\0'),  ONE_WCHAR('C', '\0'),  ONE_WCHAR('E', '\0'),  ONE_WCHAR('R', '\0'),  ONE_WCHAR('T', '\0'),  ONE_WCHAR('\0', '\0')};
const                DRM_WCHAR      g_rgwchTagWrmHeader                          [] =  { ONE_WCHAR('W', '\0'),  ONE_WCHAR('R', '\0'),  ONE_WCHAR('M', '\0'),  ONE_WCHAR('H', '\0'),  ONE_WCHAR('E', '\0'),  ONE_WCHAR('A', '\0'),  ONE_WCHAR('D', '\0'),  ONE_WCHAR('E', '\0'),  ONE_WCHAR('R', '\0'),  ONE_WCHAR('\0', '\0')};
const                DRM_WCHAR      g_rgwchAttributeVersion2Value                [] =  { ONE_WCHAR('2', '\0'),  ONE_WCHAR('.', '\0'),  ONE_WCHAR('0', '\0'),  ONE_WCHAR('.', '\0'),  ONE_WCHAR('0', '\0'),  ONE_WCHAR('.', '\0'),  ONE_WCHAR('0', '\0'),  ONE_WCHAR('\0', '\0')};
const                DRM_WCHAR      g_rgwchAttributeVersion4Value                [] =  { ONE_WCHAR('4', '\0'),  ONE_WCHAR('.', '\0'),  ONE_WCHAR('0', '\0'),  ONE_WCHAR('.', '\0'),  ONE_WCHAR('0', '\0'),  ONE_WCHAR('.', '\0'),  ONE_WCHAR('0', '\0'),  ONE_WCHAR('\0', '\0')};
const                DRM_WCHAR      g_rgwchAttributeVersion4_1Value              [] =  { ONE_WCHAR('4', '\0'),  ONE_WCHAR('.', '\0'),  ONE_WCHAR('1', '\0'),  ONE_WCHAR('.', '\0'),  ONE_WCHAR('0', '\0'),  ONE_WCHAR('.', '\0'),  ONE_WCHAR('0', '\0'),  ONE_WCHAR('\0', '\0')};
const                DRM_WCHAR      g_rgwchTagLAINFO                             [] =  { ONE_WCHAR('L', '\0'),  ONE_WCHAR('A', '\0'),  ONE_WCHAR('I', '\0'),  ONE_WCHAR('N', '\0'),  ONE_WCHAR('F', '\0'),  ONE_WCHAR('O', '\0'),  ONE_WCHAR('\0', '\0')};
const                DRM_WCHAR      g_rgwchTagV4DATA                             [] =  { ONE_WCHAR('V', '\0'),  ONE_WCHAR('4', '\0'),  ONE_WCHAR('D', '\0'),  ONE_WCHAR('A', '\0'),  ONE_WCHAR('T', '\0'),  ONE_WCHAR('A', '\0'),  ONE_WCHAR('\0', '\0')};
const                DRM_WCHAR      g_rgwchTagLAURL                              [] =  { ONE_WCHAR('L', '\0'),  ONE_WCHAR('A', '\0'),  ONE_WCHAR('_', '\0'),  ONE_WCHAR('U', '\0'),  ONE_WCHAR('R', '\0'),  ONE_WCHAR('L', '\0'),  ONE_WCHAR('\0', '\0')};
const                DRM_WCHAR      g_rgwchTagLUIURL                             [] =  { ONE_WCHAR('L', '\0'),  ONE_WCHAR('U', '\0'),  ONE_WCHAR('I', '\0'),  ONE_WCHAR('_', '\0'),  ONE_WCHAR('U', '\0'),  ONE_WCHAR('R', '\0'),  ONE_WCHAR('L', '\0'),  ONE_WCHAR('\0', '\0')};
const                DRM_WCHAR      g_rgwchTagDSID                               [] =  { ONE_WCHAR('D', '\0'),  ONE_WCHAR('S', '\0'),  ONE_WCHAR('_', '\0'),  ONE_WCHAR('I', '\0'),  ONE_WCHAR('D', '\0'),  ONE_WCHAR('\0', '\0')};
const                DRM_WCHAR      g_rgwchTagKID                                [] =  { ONE_WCHAR('K', '\0'),  ONE_WCHAR('I', '\0'),  ONE_WCHAR('D', '\0'),  ONE_WCHAR('\0', '\0')};
const                DRM_WCHAR      g_rgwchTagChecksum                           [] =  { ONE_WCHAR('C', '\0'),  ONE_WCHAR('H', '\0'),  ONE_WCHAR('E', '\0'),  ONE_WCHAR('C', '\0'),  ONE_WCHAR('K', '\0'),  ONE_WCHAR('S', '\0'),  ONE_WCHAR('U', '\0'),  ONE_WCHAR('M', '\0'),  ONE_WCHAR('\0', '\0')};
const                DRM_WCHAR      g_rgwchTagAlgID                              [] =  { ONE_WCHAR('A', '\0'),  ONE_WCHAR('L', '\0'),  ONE_WCHAR('G', '\0'),  ONE_WCHAR('I', '\0'),  ONE_WCHAR('D', '\0'),  ONE_WCHAR('\0', '\0')};
const                DRM_WCHAR      g_rgwchTagUplink                             [] =  { ONE_WCHAR('U', '\0'),  ONE_WCHAR('P', '\0'),  ONE_WCHAR('L', '\0'),  ONE_WCHAR('I', '\0'),  ONE_WCHAR('N', '\0'),  ONE_WCHAR('K', '\0'),  ONE_WCHAR('\0', '\0')};
const                DRM_WCHAR      g_rgwchTagDecryptorSetup                     [] =  { ONE_WCHAR('D', '\0'),  ONE_WCHAR('E', '\0'),  ONE_WCHAR('C', '\0'),  ONE_WCHAR('R', '\0'),  ONE_WCHAR('Y', '\0'),  ONE_WCHAR('P', '\0'),  ONE_WCHAR('T', '\0'),  ONE_WCHAR('O', '\0'),  ONE_WCHAR('R', '\0'),  ONE_WCHAR('S', '\0'),  ONE_WCHAR('E', '\0'),  ONE_WCHAR('T', '\0'),  ONE_WCHAR('U', '\0'),  ONE_WCHAR('P', '\0'),  ONE_WCHAR('\0', '\0')};
const                DRM_WCHAR      g_rgwchTagProtectInfo                        [] =  { ONE_WCHAR( 'P', '\0' ), ONE_WCHAR( 'R', '\0' ), ONE_WCHAR( 'O', '\0' ), ONE_WCHAR( 'T', '\0' ), ONE_WCHAR( 'E', '\0' ), ONE_WCHAR( 'C', '\0' ), ONE_WCHAR( 'T', '\0' ), ONE_WCHAR( 'I', '\0' ), ONE_WCHAR( 'N', '\0' ), ONE_WCHAR( 'F', '\0' ), ONE_WCHAR( 'O', '\0' ), ONE_WCHAR( '\0', '\0' ) };
const                DRM_WCHAR      g_rgwchTagKeyLen                             [] =  { ONE_WCHAR( 'K', '\0' ), ONE_WCHAR( 'E', '\0' ), ONE_WCHAR( 'Y', '\0' ), ONE_WCHAR( 'L', '\0' ), ONE_WCHAR( 'E', '\0' ), ONE_WCHAR( 'N', '\0' ), ONE_WCHAR( '\0', '\0' ) };
const                DRM_WCHAR      g_rgwchTagKeyLenNodeDataCocktail             [] =  { ONE_WCHAR( '7', '\0' ), ONE_WCHAR( '\0', '\0' ) };
const                DRM_WCHAR      g_rgwchTagKeyLenNodeDataAESCTR               [] =  { ONE_WCHAR( '1', '\0' ), ONE_WCHAR( '6', '\0' ), ONE_WCHAR( '\0', '\0' ) };
const                DRM_WCHAR      g_rgwchTagCOCKTAIL                           [] =  { ONE_WCHAR( 'C', '\0' ), ONE_WCHAR( 'O', '\0' ), ONE_WCHAR( 'C', '\0' ), ONE_WCHAR( 'K', '\0' ), ONE_WCHAR( 'T', '\0' ), ONE_WCHAR( 'A', '\0' ), ONE_WCHAR( 'I', '\0' ), ONE_WCHAR( 'L', '\0' ), ONE_WCHAR( '\0', '\0' ) };
const                DRM_WCHAR      g_rgwchTagAESCTR                             [] =  { ONE_WCHAR( 'A', '\0' ), ONE_WCHAR( 'E', '\0' ), ONE_WCHAR( 'S', '\0' ), ONE_WCHAR( 'C', '\0' ), ONE_WCHAR( 'T', '\0' ), ONE_WCHAR( 'R', '\0' ), ONE_WCHAR( '\0', '\0' ) };
const                DRM_WCHAR      g_rgwchTagCustomAttributes                   [] =  { ONE_WCHAR( 'C', '\0' ), ONE_WCHAR( 'U', '\0' ), ONE_WCHAR( 'S', '\0' ), ONE_WCHAR( 'T', '\0' ), ONE_WCHAR( 'O', '\0' ), ONE_WCHAR( 'M', '\0' ), ONE_WCHAR( 'A', '\0' ), ONE_WCHAR( 'T', '\0' ), ONE_WCHAR( 'T', '\0' ), ONE_WCHAR( 'R', '\0' ), ONE_WCHAR( 'I', '\0' ), ONE_WCHAR( 'B', '\0' ), ONE_WCHAR( 'U', '\0' ), ONE_WCHAR( 'T', '\0' ), ONE_WCHAR( 'E', '\0' ), ONE_WCHAR( 'S', '\0' ), ONE_WCHAR( '\0', '\0' ) };
const                DRM_WCHAR      g_rgwchTagONDEMAND                           [] =  { ONE_WCHAR( 'O', '\0' ), ONE_WCHAR( 'N', '\0' ), ONE_WCHAR( 'D', '\0' ), ONE_WCHAR( 'E', '\0' ), ONE_WCHAR( 'M', '\0' ), ONE_WCHAR( 'A', '\0' ), ONE_WCHAR( 'N', '\0' ), ONE_WCHAR( 'D', '\0' ), ONE_WCHAR( '\0', '\0' ) };
const                DRM_WCHAR      g_rgwchEmptyWRMHeaderV4_1                    [] =  { ONE_WCHAR( '<', '\0' ), ONE_WCHAR( 'W', '\0' ), ONE_WCHAR( 'R', '\0' ), ONE_WCHAR( 'M', '\0' ),
                                                                                         ONE_WCHAR( 'H', '\0' ), ONE_WCHAR( 'E', '\0' ), ONE_WCHAR( 'A', '\0' ), ONE_WCHAR( 'D', '\0' ),
                                                                                         ONE_WCHAR( 'E', '\0' ), ONE_WCHAR( 'R', '\0' ), ONE_WCHAR( ' ', '\0' ), ONE_WCHAR( 'x', '\0' ),
                                                                                         ONE_WCHAR( 'm', '\0' ), ONE_WCHAR( 'l', '\0' ), ONE_WCHAR( 'n', '\0' ), ONE_WCHAR( 's', '\0' ),
                                                                                         ONE_WCHAR( '=', '\0' ), ONE_WCHAR( '"', '\0' ), ONE_WCHAR( 'h', '\0' ), ONE_WCHAR( 't', '\0' ),
                                                                                         ONE_WCHAR( 't', '\0' ), ONE_WCHAR( 'p', '\0' ), ONE_WCHAR( ':', '\0' ), ONE_WCHAR( '/', '\0' ),
                                                                                         ONE_WCHAR( '/', '\0' ), ONE_WCHAR( 's', '\0' ), ONE_WCHAR( 'c', '\0' ), ONE_WCHAR( 'h', '\0' ),
                                                                                         ONE_WCHAR( 'e', '\0' ), ONE_WCHAR( 'm', '\0' ), ONE_WCHAR( 'a', '\0' ), ONE_WCHAR( 's', '\0' ),
                                                                                         ONE_WCHAR( '.', '\0' ), ONE_WCHAR( 'm', '\0' ), ONE_WCHAR( 'i', '\0' ), ONE_WCHAR( 'c', '\0' ),
                                                                                         ONE_WCHAR( 'r', '\0' ), ONE_WCHAR( 'o', '\0' ), ONE_WCHAR( 's', '\0' ), ONE_WCHAR( 'o', '\0' ),
                                                                                         ONE_WCHAR( 'f', '\0' ), ONE_WCHAR( 't', '\0' ), ONE_WCHAR( '.', '\0' ), ONE_WCHAR( 'c', '\0' ),
                                                                                         ONE_WCHAR( 'o', '\0' ), ONE_WCHAR( 'm', '\0' ), ONE_WCHAR( '/', '\0' ), ONE_WCHAR( 'D', '\0' ),
                                                                                         ONE_WCHAR( 'R', '\0' ), ONE_WCHAR( 'M', '\0' ), ONE_WCHAR( '/', '\0' ), ONE_WCHAR( '2', '\0' ),
                                                                                         ONE_WCHAR( '0', '\0' ), ONE_WCHAR( '0', '\0' ), ONE_WCHAR( '7', '\0' ), ONE_WCHAR( '/', '\0' ),
                                                                                         ONE_WCHAR( '0', '\0' ), ONE_WCHAR( '3', '\0' ), ONE_WCHAR( '/', '\0' ), ONE_WCHAR( 'P', '\0' ),
                                                                                         ONE_WCHAR( 'l', '\0' ), ONE_WCHAR( 'a', '\0' ), ONE_WCHAR( 'y', '\0' ), ONE_WCHAR( 'R', '\0' ),
                                                                                         ONE_WCHAR( 'e', '\0' ), ONE_WCHAR( 'a', '\0' ), ONE_WCHAR( 'd', '\0' ), ONE_WCHAR( 'y', '\0' ),
                                                                                         ONE_WCHAR( 'H', '\0' ), ONE_WCHAR( 'e', '\0' ), ONE_WCHAR( 'a', '\0' ), ONE_WCHAR( 'd', '\0' ),
                                                                                         ONE_WCHAR( 'e', '\0' ), ONE_WCHAR( 'r', '\0' ), ONE_WCHAR( '"', '\0' ), ONE_WCHAR( ' ', '\0' ),
                                                                                         ONE_WCHAR( 'v', '\0' ), ONE_WCHAR( 'e', '\0' ), ONE_WCHAR( 'r', '\0' ), ONE_WCHAR( 's', '\0' ),
                                                                                         ONE_WCHAR( 'i', '\0' ), ONE_WCHAR( 'o', '\0' ), ONE_WCHAR( 'n', '\0' ), ONE_WCHAR( '=', '\0' ),
                                                                                         ONE_WCHAR( '"', '\0' ), ONE_WCHAR( '4', '\0' ), ONE_WCHAR( '.', '\0' ), ONE_WCHAR( '1', '\0' ),
                                                                                         ONE_WCHAR( '.', '\0' ), ONE_WCHAR( '0', '\0' ), ONE_WCHAR( '.', '\0' ), ONE_WCHAR( '0', '\0' ),
                                                                                         ONE_WCHAR( '"', '\0' ), ONE_WCHAR( '>', '\0' ), ONE_WCHAR( '<', '\0' ), ONE_WCHAR( '/', '\0' ),
                                                                                         ONE_WCHAR( 'W', '\0' ), ONE_WCHAR( 'R', '\0' ), ONE_WCHAR( 'M', '\0' ), ONE_WCHAR( 'H', '\0' ),
                                                                                         ONE_WCHAR( 'E', '\0' ), ONE_WCHAR( 'A', '\0' ), ONE_WCHAR( 'D', '\0' ), ONE_WCHAR( 'E', '\0' ),
                                                                                         ONE_WCHAR( 'R', '\0' ), ONE_WCHAR( '>', '\0' ), ONE_WCHAR( '\0', '\0' ) };
const                DRM_WCHAR      g_rgwchTagAction                             [] =  { ONE_WCHAR('A', '\0'),  ONE_WCHAR('C', '\0'),  ONE_WCHAR('T', '\0'),  ONE_WCHAR('I', '\0'),  ONE_WCHAR('O', '\0'),  ONE_WCHAR('N', '\0'),  ONE_WCHAR('\0', '\0')};
const                DRM_WCHAR      g_rgwchTagSecurityVersion                    [] =  { ONE_WCHAR('S', '\0'),  ONE_WCHAR('E', '\0'),  ONE_WCHAR('C', '\0'),  ONE_WCHAR('U', '\0'),  ONE_WCHAR('R', '\0'),  ONE_WCHAR('I', '\0'),  ONE_WCHAR('T', '\0'),  ONE_WCHAR('Y', '\0'),  ONE_WCHAR('V', '\0'),  ONE_WCHAR('E', '\0'),  ONE_WCHAR('R', '\0'),  ONE_WCHAR('S', '\0'),  ONE_WCHAR('I', '\0'),  ONE_WCHAR('O', '\0'),  ONE_WCHAR('N', '\0'),  ONE_WCHAR('\0', '\0')};
const                DRM_WCHAR      g_rgwchTagMSDRMSignature                     [] =  { ONE_WCHAR('M', '\0'),  ONE_WCHAR('S', '\0'),  ONE_WCHAR('D', '\0'),  ONE_WCHAR('R', '\0'),  ONE_WCHAR('M', '\0'),  ONE_WCHAR('_', '\0'),  ONE_WCHAR('S', '\0'),  ONE_WCHAR('I', '\0'),  ONE_WCHAR('G', '\0'),  ONE_WCHAR('N', '\0'),  ONE_WCHAR('A', '\0'),  ONE_WCHAR('T', '\0'),  ONE_WCHAR('U', '\0'),  ONE_WCHAR('R', '\0'),  ONE_WCHAR('E', '\0'),  ONE_WCHAR('_', '\0'),  ONE_WCHAR('V', '\0'),  ONE_WCHAR('A', '\0'),  ONE_WCHAR('L', '\0'),  ONE_WCHAR('U', '\0'),  ONE_WCHAR('E', '\0'),  ONE_WCHAR('\0', '\0')};
const                DRM_WCHAR      g_rgwchTagMID                                [] =  { ONE_WCHAR('M', '\0'),  ONE_WCHAR('I', '\0'),  ONE_WCHAR('D', '\0'),  ONE_WCHAR('\0', '\0')};
const                DRM_WCHAR      g_rgwchTagTID                                [] =  { ONE_WCHAR('T', '\0'),  ONE_WCHAR('I', '\0'),  ONE_WCHAR('D', '\0'),  ONE_WCHAR('\0', '\0')};
const                DRM_WCHAR      g_rgwchTagRecords                            [] =  { ONE_WCHAR('R', '\0'),  ONE_WCHAR('E', '\0'),  ONE_WCHAR('C', '\0'),  ONE_WCHAR('O', '\0'),  ONE_WCHAR('R', '\0'),  ONE_WCHAR('D', '\0'),  ONE_WCHAR('S', '\0'),  ONE_WCHAR('\0', '\0')};
const                DRM_WCHAR      g_rgwchTagURL                                [] =  { ONE_WCHAR('U', '\0'),  ONE_WCHAR('R', '\0'),  ONE_WCHAR('L', '\0'),  ONE_WCHAR('\0', '\0')};
const                DRM_WCHAR      g_rgwchLabelValue                            [] =  { ONE_WCHAR('v', '\0'),  ONE_WCHAR('a', '\0'),  ONE_WCHAR('l', '\0'),  ONE_WCHAR('u', '\0'),  ONE_WCHAR('e', '\0'),  ONE_WCHAR('\0', '\0')};
const                DRM_WCHAR      g_rgwchChallenge                             [] =  { ONE_WCHAR('c', '\0'),  ONE_WCHAR('h', '\0'),  ONE_WCHAR('a', '\0'),  ONE_WCHAR('l', '\0'),  ONE_WCHAR('l', '\0'),  ONE_WCHAR('e', '\0'),  ONE_WCHAR('n', '\0'),  ONE_WCHAR('g', '\0'),  ONE_WCHAR('e', '\0'),  ONE_WCHAR('\0', '\0')};
const                DRM_WCHAR      g_rgwchTagCertificateChain                   [] =  { ONE_WCHAR('C', '\0'),  ONE_WCHAR('E', '\0'),  ONE_WCHAR('R', '\0'),  ONE_WCHAR('T', '\0'),  ONE_WCHAR('I', '\0'),  ONE_WCHAR('F', '\0'),  ONE_WCHAR('I', '\0'),  ONE_WCHAR('C', '\0'),  ONE_WCHAR('A', '\0'),  ONE_WCHAR('T', '\0'),  ONE_WCHAR('E', '\0'),  ONE_WCHAR('C', '\0'),  ONE_WCHAR('H', '\0'),  ONE_WCHAR('A', '\0'),  ONE_WCHAR('I', '\0'),  ONE_WCHAR('N', '\0'),  ONE_WCHAR('\0', '\0')};
const                DRM_WCHAR      g_rgwchTagPrivateKey                         [] =  { ONE_WCHAR('P', '\0'),  ONE_WCHAR('R', '\0'),  ONE_WCHAR('I', '\0'),  ONE_WCHAR('V', '\0'),  ONE_WCHAR('A', '\0'),  ONE_WCHAR('T', '\0'),  ONE_WCHAR('E', '\0'),  ONE_WCHAR('K', '\0'),  ONE_WCHAR('E', '\0'),  ONE_WCHAR('Y', '\0'),  ONE_WCHAR('\0', '\0')};
const                DRM_WCHAR      g_rgwchTagDevice                             [] =  { ONE_WCHAR('D', '\0'),  ONE_WCHAR('E', '\0'),  ONE_WCHAR('V', '\0'),  ONE_WCHAR('I', '\0'),  ONE_WCHAR('C', '\0'),  ONE_WCHAR('E', '\0'),  ONE_WCHAR('\0', '\0')};
const                DRM_WCHAR      g_rgwchRootSigValueVersionTag                [] =  { ONE_WCHAR('p', '\0'),  ONE_WCHAR('r', '\0'),  ONE_WCHAR('i', '\0'),  ONE_WCHAR('v', '\0'),  ONE_WCHAR('a', '\0'),  ONE_WCHAR('t', '\0'),  ONE_WCHAR('e', '\0'),  ONE_WCHAR('\0', '\0')};
const                DRM_WCHAR      g_rgwchRootSigValueVersionVal                [] =  { ONE_WCHAR('1', '\0'),  ONE_WCHAR('\0', '\0')};
const                DRM_WCHAR      g_rgwchTagDataPubKey                         [] =  { ONE_WCHAR('P', '\0'),  ONE_WCHAR('U', '\0'),  ONE_WCHAR('B', '\0'),  ONE_WCHAR('L', '\0'),  ONE_WCHAR('I', '\0'),  ONE_WCHAR('C', '\0'),  ONE_WCHAR('K', '\0'),  ONE_WCHAR('E', '\0'),  ONE_WCHAR('Y', '\0'),  ONE_WCHAR('\0', '\0')};
const                DRM_WCHAR      g_rgwchTemplate                              [] =  { ONE_WCHAR('T', '\0'),  ONE_WCHAR('E', '\0'),  ONE_WCHAR('M', '\0'),  ONE_WCHAR('P', '\0'),  ONE_WCHAR('L', '\0'),  ONE_WCHAR('A', '\0'),  ONE_WCHAR('T', '\0'),  ONE_WCHAR('E', '\0'),  ONE_WCHAR('\0', '\0')};
const                DRM_WCHAR      g_rgwchTagDataSecVerPlatform                 [] =  { ONE_WCHAR('P', '\0'),  ONE_WCHAR('L', '\0'),  ONE_WCHAR('A', '\0'),  ONE_WCHAR('T', '\0'),  ONE_WCHAR('F', '\0'),  ONE_WCHAR('O', '\0'),  ONE_WCHAR('R', '\0'),  ONE_WCHAR('M', '\0'),  ONE_WCHAR('_', '\0'),  ONE_WCHAR('I', '\0'),  ONE_WCHAR('D', '\0'),  ONE_WCHAR('\0', '\0')};

/* Shared Certificate tags */
const                DRM_CHAR  g_rgchAttributeAlgorithm        [] = "Algorithm";
#define g_rgchAttributeAlgorithm_LEN 9
const                DRM_CHAR  g_rgchAttributeVersionWMDRM     [] = "c:Version";
#define g_rgchAttributeVersionWMDRM_LEN 9
const                DRM_CHAR  g_rgchKeyUsageSignCert          [] = "c:SignCertificate";
#define g_rgchKeyUsageSignCert_LEN 17
const                DRM_CHAR  g_rgchKeyUsageEncryptKey        [] = "c:EncryptKey";
#define g_rgchKeyUsageEncryptKey_LEN 12
const                DRM_CHAR  g_rgchOne                       [] = "1";
#define g_rgchOne_LEN 1
const                DRM_CHAR  g_rgchPrefixManufacturer        [] = "xmlns:f";
#define g_rgchPrefixManufacturer_LEN 7
const                DRM_CHAR  g_rgchTagCanonicalization       [] = "CanonicalizationMethod";
#define g_rgchTagCanonicalization_LEN 22
const                DRM_CHAR  g_rgchTagCertificateCollection  [] = "c:CertificateCollection";
#define g_rgchTagCertificateCollection_LEN 23
const                DRM_CHAR  g_rgchTagDigestMethod           [] = "DigestMethod";
#define g_rgchTagDigestMethod_LEN 12
const                DRM_CHAR  g_rgchURIDSigSHA1               [] = "http://www.w3.org/2000/09/xmldsig#sha1";
#define g_rgchURIDSigSHA1_LEN 38
const                DRM_CHAR  g_rgchTagDigestValue            [] = "DigestValue";
#define g_rgchTagDigestValue_LEN 11
const                DRM_CHAR  g_rgchTagSignatureValue         [] = "SignatureValue";
#define g_rgchTagSignatureValue_LEN 14
const                DRM_CHAR  g_rgchTagKeyInfo                [] = "KeyInfo";
#define g_rgchTagKeyInfo_LEN 7
const                DRM_CHAR  g_rgchTagPublicKey              [] = "PublicKey";
#define g_rgchTagPublicKey_LEN 9
const                DRM_CHAR  g_rgchTagPrivateKey             [] = "PrivateKey";
#define g_rgchTagPrivateKey_LEN 10
const                DRM_CHAR  g_rgchTagKeyValue               [] = "KeyValue";
#define g_rgchTagKeyValue_LEN 8
const                DRM_CHAR  g_rgchTagRSAKeyValue            [] = "RSAKeyValue";
#define g_rgchTagRSAKeyValue_LEN 11
const                DRM_CHAR  g_rgchTagModulus                [] = "Modulus";
#define g_rgchTagModulus_LEN 7
const                DRM_CHAR  g_rgchTagExponent               [] = "Exponent";
#define g_rgchTagExponent_LEN 8
const                DRM_CHAR  g_rgchTagManufacturerName       [] = "c:ManufacturerName";
#define g_rgchTagManufacturerName_LEN 18
const                DRM_CHAR  g_rgchTagManufacturerData       [] = "c:ManufacturerData";
#define g_rgchTagManufacturerData_LEN 18
const                DRM_CHAR  g_rgchURIRSASHA1                [] = "http://www.w3.org/2000/09/xmldsig#rsa-sha1";
#define g_rgchURIRSASHA1_LEN 42
const                DRM_CHAR  g_rgchURIRSASHA1_Old            [] = "http://schemas.microsoft.com/DRM/2004/02/CERT/Rsa-sha1";
#define g_rgchURIRSASHA1_Old_LEN 54
const                DRM_CHAR  g_rgchTagReference              [] = "Reference";
#define g_rgchTagReference_LEN 9
const                DRM_CHAR  g_rgchTagTransforms             [] = "Transforms";
#define g_rgchTagTransforms_LEN 10
const                DRM_CHAR  g_rgchTagTransform              [] = "Transform";
#define g_rgchTagTransform_LEN 9
const                DRM_CHAR  g_rgchURITransformMSCert        [] = "http://www.microsoft.com/DRM/CERT/v2/Data";
#define g_rgchURITransformMSCert_LEN 41
const                DRM_CHAR  g_rgchURITransformMSCertColl    [] = "http://schemas.microsoft.com/DRM/2004/02/cert";
#define g_rgchURITransformMSCertColl_LEN 45
const                DRM_CHAR  g_rgchURITransformMSCert_Old    [] = "http://schemas.microsoft.com/DRM/2004/02/CERT/Data";
#define g_rgchURITransformMSCert_Old_LEN 50
const                DRM_CHAR  g_rgchURITransformC14N          [] = "http://www.w3.org/TR/2001/REC-xml-c14n-20010315";
#define g_rgchURITransformC14N_LEN 47
const                DRM_CHAR  g_rgchTagWMDRMCertificate       [] = "c:Certificate";
#define g_rgchTagWMDRMCertificate_LEN 13
const                DRM_CHAR  g_rgchTagDataWMDRM              [] = "c:Data";
#define g_rgchTagDataWMDRM_LEN 6
const                DRM_CHAR  g_rgchTagWMDRMCertPublicKey     [] = "c:PublicKey";
#define g_rgchTagWMDRMCertPublicKey_LEN 11
const                DRM_CHAR  g_rgchTagWMDRMCertSecurityVersion[]= "c:SecurityVersion";
#define g_rgchTagWMDRMCertSecurityVersion_LEN 17
const                DRM_CHAR  g_rgchTagWMDRMCertSecurityLevel [] = "c:SecurityLevel";
#define g_rgchTagWMDRMCertSecurityLevel_LEN 15
const                DRM_CHAR  g_rgchTagWMDRMCertFeatures      [] = "c:Features";
#define g_rgchTagWMDRMCertFeatures_LEN 10
const                DRM_CHAR  g_rgchTagWMDRMCertKeyUsage      [] = "c:KeyUsage";
#define g_rgchTagWMDRMCertKeyUsage_LEN 10
const                DRM_CHAR  g_rgchFeaturesWMDRMTransmitter  [] = "c:WMDRMTransmitter";
#define g_rgchFeaturesWMDRMTransmitter_LEN 18
const                DRM_CHAR  g_rgchFeaturesWMDRMReceiver     [] = "c:WMDRMReceiver";
#define g_rgchFeaturesWMDRMReceiver_LEN 15
const                DRM_CHAR  g_rgchTagSignature              [] = "Signature";
#define g_rgchTagSignature_LEN 9
const                DRM_CHAR  g_rgchTagSignatureMethod        [] = "SignatureMethod";
#define g_rgchTagSignatureMethod_LEN 15
const                DRM_CHAR  g_rgchTagSignedInfo             [] = "SignedInfo";
#define g_rgchTagSignedInfo_LEN 10
const                DRM_CHAR  g_rgchURIC14N                   [] = "http://www.w3.org/TR/2001/REC-xml-c14n-20010315";
#define g_rgchURIC14N_LEN 47
const                DRM_CHAR  g_rgchVersionWMDRM              [] = "2.0";
#define g_rgchVersionWMDRM_LEN 3
const                DRM_CHAR  g_rgchWMDRMCertExponent         [] = "AQAB";
#define g_rgchWMDRMCertExponent_LEN 4
const                DRM_CHAR  g_rgchPrefixMicrosoftCert       [] = "xmlns:c";
#define g_rgchPrefixMicrosoftCert_LEN 7
const                DRM_CHAR  g_rgchMSNDRootPubKeyB64         [] = "pjoeWLSTLDonQG8She6QhkYbYott9fPZ8tHdB128ZETcghn5KHoyin7HkJEcPJ0Eg4UdSva0KDIYDjA3EXd69R3CN2Wp/QyOo0ZPYWYp3NXpJ700tKPgIplzo5wVd/69g7j+j8M66W7VNmDwaNs9mDc1p2+VVMsDhOsV/Au6E+E=";
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
                 0xCCDE5A55, 0xA688, 0x4405, 0xA8, 0x8B, 0xD1, 0x3F, 0x90, 0xD5, 0xBA, 0x3E);

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


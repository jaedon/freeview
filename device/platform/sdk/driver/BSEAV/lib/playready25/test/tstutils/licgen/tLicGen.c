/**@@@+++@@@@******************************************************************
**
** Microsoft (r) PlayReady (r)
** Copyright (c) Microsoft Corporation. All rights reserved.
**
***@@@---@@@@******************************************************************
*/

#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include <drmcommon.h>
#include <drmbase64.h>
#include <drmxmlbuilder.h>
#include <drmkeygenerator.h>
#include <drmutilities.h>
#include <drmcontextsizes.h>
#include <drmblackbox.h>
#include <drmcrt.h>
#include <drmtestresults.h>
#include <tLicGen.h>
#include <tstutils.h>
#include <SetClientID.h>

PREFAST_PUSH_DISABLE_EXPLAINED(__WARNING_USE_WIDE_API, "Ignore prefast warning about ANSI functions for this entire file.");

ENTER_PKTEST_NAMESPACE_CODE;

const char *szLicGenDefaultKeySeed = "zWVMVj8hZun5DyUU38zET5B1E4sl6QsmPIrqzkDP";
const char *szLicGenDefaultKID = "QCl1EY0ISE2qXlCDaIMJ0A==";
const char *szLicGenDefaultUplinkKID = "Acl2BY3ITE0qe11DalmK0D==";
const char *szLicGenDefaultLID = "{00000507-0000-0010-8000-00AA006D2EA4}";
DRM_WCHAR DevExclusionList [] = { ONE_WCHAR('D', '\0'), ONE_WCHAR('e', '\0'), ONE_WCHAR('v', '\0'), ONE_WCHAR('i', '\0'), ONE_WCHAR('c', '\0'),ONE_WCHAR('e', '\0'),ONE_WCHAR('E', '\0'),ONE_WCHAR('x', '\0'),ONE_WCHAR('c', '\0'),ONE_WCHAR('l', '\0'),ONE_WCHAR('u', '\0'),ONE_WCHAR('s', '\0'),ONE_WCHAR('i', '\0'),ONE_WCHAR('o', '\0'),ONE_WCHAR('n', '\0'),ONE_WCHAR('L', '\0'),ONE_WCHAR('i', '\0'),ONE_WCHAR('s', '\0'),ONE_WCHAR('t', '\0'),ONE_WCHAR('\0', '\0') };

const DRM_WCHAR XMLHeader[]  = { ONE_WCHAR('<','\0'),  ONE_WCHAR('?','\0'),  ONE_WCHAR('x','\0'),  ONE_WCHAR('m','\0'),  ONE_WCHAR('l','\0'),  ONE_WCHAR(' ','\0'),  ONE_WCHAR('v','\0'),  ONE_WCHAR('e','\0'),  ONE_WCHAR('r','\0'),  ONE_WCHAR('s','\0'),  ONE_WCHAR('i','\0'),  ONE_WCHAR('o','\0'),  ONE_WCHAR('n','\0'),  ONE_WCHAR('=','\0'),  ONE_WCHAR('"','\0'),  ONE_WCHAR('1','\0'),  ONE_WCHAR('.','\0'),  ONE_WCHAR('0','\0'),  ONE_WCHAR('"','\0'),  ONE_WCHAR('?','\0'),  ONE_WCHAR('>','\0'), ONE_WCHAR('\0','\0') } ;
const DRM_WCHAR License[]  = { ONE_WCHAR('L','\0'),  ONE_WCHAR('I','\0'),  ONE_WCHAR('C','\0'),  ONE_WCHAR('E','\0'),  ONE_WCHAR('N','\0'),  ONE_WCHAR('S','\0'),  ONE_WCHAR('E','\0'), ONE_WCHAR('\0','\0') } ;
const DRM_WCHAR VersionAttr[]  = { ONE_WCHAR('v','\0'),  ONE_WCHAR('e','\0'),  ONE_WCHAR('r','\0'),  ONE_WCHAR('s','\0'),  ONE_WCHAR('i','\0'),  ONE_WCHAR('o','\0'),  ONE_WCHAR('n','\0'), ONE_WCHAR('\0','\0') } ;
const DRM_WCHAR VersionValue[]  = { ONE_WCHAR('2','\0'),  ONE_WCHAR('.','\0'),  ONE_WCHAR('0','\0'),  ONE_WCHAR('.','\0'),  ONE_WCHAR('0','\0'),  ONE_WCHAR('.','\0'),  ONE_WCHAR('0','\0'), ONE_WCHAR('\0','\0') } ;
const DRM_WCHAR LicensorInfo[]  = { ONE_WCHAR('L','\0'),  ONE_WCHAR('I','\0'),  ONE_WCHAR('C','\0'),  ONE_WCHAR('E','\0'),  ONE_WCHAR('N','\0'),  ONE_WCHAR('S','\0'),  ONE_WCHAR('O','\0'),  ONE_WCHAR('R','\0'),  ONE_WCHAR('I','\0'),  ONE_WCHAR('N','\0'),  ONE_WCHAR('F','\0'),  ONE_WCHAR('O','\0'), ONE_WCHAR('\0','\0') } ;
const DRM_WCHAR DRMRestoreInfo [] = { ONE_WCHAR('D','\0'),  ONE_WCHAR('R','\0'),  ONE_WCHAR('M','\0'),  ONE_WCHAR('R','\0'),  ONE_WCHAR('E','\0'),  ONE_WCHAR('S','\0'),  ONE_WCHAR('T','\0'),  ONE_WCHAR('O','\0'),  ONE_WCHAR('R','\0'),  ONE_WCHAR('E','\0'),  ONE_WCHAR('I','\0'),  ONE_WCHAR('N','\0'),  ONE_WCHAR('F','\0'),  ONE_WCHAR('O','\0'), ONE_WCHAR('\0','\0') } ;
const DRM_WCHAR Data[]  = { ONE_WCHAR('D','\0'),  ONE_WCHAR('A','\0'),  ONE_WCHAR('T','\0'),  ONE_WCHAR('A','\0'), ONE_WCHAR('\0','\0') } ;
const DRM_WCHAR Signature[]  = { ONE_WCHAR('S','\0'),  ONE_WCHAR('I','\0'),  ONE_WCHAR('G','\0'),  ONE_WCHAR('N','\0'),  ONE_WCHAR('A','\0'),  ONE_WCHAR('T','\0'),  ONE_WCHAR('U','\0'),  ONE_WCHAR('R','\0'),  ONE_WCHAR('E','\0'), ONE_WCHAR('\0','\0') } ;
const DRM_WCHAR CertChain[]  = { ONE_WCHAR('C','\0'),  ONE_WCHAR('E','\0'),  ONE_WCHAR('R','\0'),  ONE_WCHAR('T','\0'),  ONE_WCHAR('I','\0'),  ONE_WCHAR('F','\0'),  ONE_WCHAR('I','\0'),  ONE_WCHAR('C','\0'),  ONE_WCHAR('A','\0'),  ONE_WCHAR('T','\0'),  ONE_WCHAR('E','\0'),  ONE_WCHAR('C','\0'),  ONE_WCHAR('H','\0'),  ONE_WCHAR('A','\0'),  ONE_WCHAR('I','\0'),  ONE_WCHAR('N','\0'), ONE_WCHAR('\0','\0') } ;
const DRM_WCHAR Certificate  []= { ONE_WCHAR('C','\0'),  ONE_WCHAR('E','\0'),  ONE_WCHAR('R','\0'),  ONE_WCHAR('T','\0'),  ONE_WCHAR('I','\0'),  ONE_WCHAR('F','\0'),  ONE_WCHAR('I','\0'),  ONE_WCHAR('C','\0'),  ONE_WCHAR('A','\0'),  ONE_WCHAR('T','\0'),  ONE_WCHAR('E','\0'), ONE_WCHAR('\0','\0') } ;
const DRM_WCHAR EnablingBits [] = { ONE_WCHAR('E','\0'),  ONE_WCHAR('N','\0'),  ONE_WCHAR('A','\0'),  ONE_WCHAR('B','\0'),  ONE_WCHAR('L','\0'),  ONE_WCHAR('I','\0'),  ONE_WCHAR('N','\0'),  ONE_WCHAR('G','\0'),  ONE_WCHAR('B','\0'),  ONE_WCHAR('I','\0'),  ONE_WCHAR('T','\0'),  ONE_WCHAR('S','\0'), ONE_WCHAR('\0','\0') } ;
const DRM_WCHAR ChainedEnablingBits  []= { ONE_WCHAR('C','\0'),  ONE_WCHAR('H','\0'),  ONE_WCHAR('A','\0'),  ONE_WCHAR('I','\0'),  ONE_WCHAR('N','\0'),  ONE_WCHAR('E','\0'),  ONE_WCHAR('D','\0'),  ONE_WCHAR('E','\0'),  ONE_WCHAR('N','\0'),  ONE_WCHAR('A','\0'),  ONE_WCHAR('B','\0'),  ONE_WCHAR('L','\0'),  ONE_WCHAR('I','\0'),  ONE_WCHAR('N','\0'),  ONE_WCHAR('G','\0'),  ONE_WCHAR('B','\0'),  ONE_WCHAR('I','\0'),  ONE_WCHAR('T','\0'),  ONE_WCHAR('S','\0'), ONE_WCHAR('\0','\0') };
const DRM_WCHAR ChainedCheckSum  [] = { ONE_WCHAR('C','\0'),  ONE_WCHAR('H','\0'),  ONE_WCHAR('A','\0'),  ONE_WCHAR('I','\0'),  ONE_WCHAR('N','\0'),  ONE_WCHAR('C','\0'),  ONE_WCHAR('H','\0'),  ONE_WCHAR('E','\0'),  ONE_WCHAR('C','\0'),  ONE_WCHAR('K','\0'),  ONE_WCHAR('S','\0'),  ONE_WCHAR('U','\0'),  ONE_WCHAR('M','\0'), ONE_WCHAR('\0','\0') };
const DRM_WCHAR ContentRevocation  [] = { ONE_WCHAR('C','\0'),  ONE_WCHAR('O','\0'),  ONE_WCHAR('N','\0'),  ONE_WCHAR('T','\0'),  ONE_WCHAR('E','\0'),  ONE_WCHAR('N','\0'),  ONE_WCHAR('T','\0'),  ONE_WCHAR('R','\0'),  ONE_WCHAR('E','\0'),  ONE_WCHAR('V','\0'),  ONE_WCHAR('O','\0'),  ONE_WCHAR('C','\0'),  ONE_WCHAR('A','\0'),  ONE_WCHAR('T','\0'),  ONE_WCHAR('I','\0'),  ONE_WCHAR('O','\0'),  ONE_WCHAR('N','\0'), ONE_WCHAR('\0','\0') };
const DRM_WCHAR Value [] =  { ONE_WCHAR('V','\0'),  ONE_WCHAR('A','\0'),  ONE_WCHAR('L','\0'),  ONE_WCHAR('U','\0'),  ONE_WCHAR('E','\0'), ONE_WCHAR('\0','\0') };
const DRM_WCHAR ContentPubKey   []= { ONE_WCHAR('C','\0'),  ONE_WCHAR('O','\0'),  ONE_WCHAR('N','\0'),  ONE_WCHAR('T','\0'),  ONE_WCHAR('E','\0'),  ONE_WCHAR('N','\0'),  ONE_WCHAR('T','\0'),  ONE_WCHAR('P','\0'),  ONE_WCHAR('U','\0'),  ONE_WCHAR('B','\0'),  ONE_WCHAR('K','\0'),  ONE_WCHAR('E','\0'),  ONE_WCHAR('Y','\0'), ONE_WCHAR('\0','\0') };
const DRM_WCHAR LicenseServerPubKey  [] = { ONE_WCHAR('L','\0'),  ONE_WCHAR('I','\0'),  ONE_WCHAR('C','\0'),  ONE_WCHAR('E','\0'),  ONE_WCHAR('N','\0'),  ONE_WCHAR('S','\0'),  ONE_WCHAR('E','\0'),  ONE_WCHAR('S','\0'),  ONE_WCHAR('E','\0'),  ONE_WCHAR('R','\0'),  ONE_WCHAR('V','\0'),  ONE_WCHAR('E','\0'),  ONE_WCHAR('R','\0'),  ONE_WCHAR('P','\0'),  ONE_WCHAR('U','\0'),  ONE_WCHAR('B','\0'),  ONE_WCHAR('K','\0'),  ONE_WCHAR('E','\0'),  ONE_WCHAR('Y','\0'), ONE_WCHAR('\0','\0') };
const DRM_WCHAR SequenceNumber  []= { ONE_WCHAR('S','\0'),  ONE_WCHAR('E','\0'),  ONE_WCHAR('Q','\0'),  ONE_WCHAR('U','\0'),  ONE_WCHAR('E','\0'),  ONE_WCHAR('N','\0'),  ONE_WCHAR('C','\0'),  ONE_WCHAR('E','\0'),  ONE_WCHAR('N','\0'),  ONE_WCHAR('U','\0'),  ONE_WCHAR('M','\0'),  ONE_WCHAR('B','\0'),  ONE_WCHAR('E','\0'),  ONE_WCHAR('R','\0'), ONE_WCHAR('\0','\0') };
const DRM_WCHAR KEYID  []= { ONE_WCHAR('K','\0'),  ONE_WCHAR('I','\0'),  ONE_WCHAR('D','\0'), ONE_WCHAR('\0','\0') };
const DRM_WCHAR LID [] = { ONE_WCHAR('L','\0'),  ONE_WCHAR('I','\0'),  ONE_WCHAR('D','\0'), ONE_WCHAR('\0','\0') };
const DRM_WCHAR WCPUBKEY [] = { ONE_WCHAR('P','\0'),  ONE_WCHAR('U','\0'),  ONE_WCHAR('B','\0'),  ONE_WCHAR('K','\0'),  ONE_WCHAR('E','\0'),  ONE_WCHAR('Y','\0'), ONE_WCHAR('\0','\0') };
const DRM_WCHAR Type [] = { ONE_WCHAR('t','\0'),  ONE_WCHAR('y','\0'),  ONE_WCHAR('p','\0'),  ONE_WCHAR('e','\0'), ONE_WCHAR('\0','\0') };
const DRM_WCHAR Machine  [] = { ONE_WCHAR('m','\0'),  ONE_WCHAR('a','\0'),  ONE_WCHAR('c','\0'),  ONE_WCHAR('h','\0'),  ONE_WCHAR('i','\0'),  ONE_WCHAR('n','\0'),  ONE_WCHAR('e','\0'), ONE_WCHAR('\0','\0') };
const DRM_WCHAR UPLINK  [] = { ONE_WCHAR('U','\0'),  ONE_WCHAR('P','\0'),  ONE_WCHAR('L','\0'),  ONE_WCHAR('I','\0'),  ONE_WCHAR('N','\0'),  ONE_WCHAR('K','\0'), ONE_WCHAR('\0','\0') };
const DRM_WCHAR REVOCATION [] = { ONE_WCHAR('R','\0'),  ONE_WCHAR('E','\0'),  ONE_WCHAR('V','\0'),  ONE_WCHAR('O','\0'),  ONE_WCHAR('C','\0'),  ONE_WCHAR('A','\0'),  ONE_WCHAR('T','\0'),  ONE_WCHAR('I','\0'),  ONE_WCHAR('O','\0'),  ONE_WCHAR('N','\0'), ONE_WCHAR('\0','\0') };
const DRM_WCHAR INDEX  [] = { ONE_WCHAR('I','\0'),  ONE_WCHAR('N','\0'),  ONE_WCHAR('D','\0'),  ONE_WCHAR('E','\0'),  ONE_WCHAR('X','\0'), ONE_WCHAR('\0','\0') };
const DRM_WCHAR LIDNode2 [] = { ONE_WCHAR('{','\0'),  ONE_WCHAR('1','\0'),  ONE_WCHAR('0','\0'),  ONE_WCHAR('0','\0'),  ONE_WCHAR('0','\0'),  ONE_WCHAR('0','\0'),  ONE_WCHAR('5','\0'),  ONE_WCHAR('5','\0'),  ONE_WCHAR('5','\0'),  ONE_WCHAR('-','\0'),  ONE_WCHAR('0','\0'),  ONE_WCHAR('0','\0'),  ONE_WCHAR('0','\0'),  ONE_WCHAR('0','\0'),  ONE_WCHAR('-','\0'),  ONE_WCHAR('0','\0'),  ONE_WCHAR('0','\0'),  ONE_WCHAR('1','\0'),  ONE_WCHAR('0','\0'),  ONE_WCHAR('-','\0'),  ONE_WCHAR('8','\0'),  ONE_WCHAR('0','\0'),  ONE_WCHAR('0','\0'),  ONE_WCHAR('0','\0'),  ONE_WCHAR('-','\0'),  ONE_WCHAR('0','\0'),  ONE_WCHAR('0','\0'),  ONE_WCHAR('A','\0'),  ONE_WCHAR('A','\0'),  ONE_WCHAR('0','\0'),  ONE_WCHAR('0','\0'),  ONE_WCHAR('6','\0'),  ONE_WCHAR('D','\0'),  ONE_WCHAR('2','\0'),  ONE_WCHAR('E','\0'),  ONE_WCHAR('B','\0'),  ONE_WCHAR('7','\0'),  ONE_WCHAR('}','\0'), ONE_WCHAR('\0','\0') };
const DRM_WCHAR IssuedDate  []= { ONE_WCHAR('<','\0'),  ONE_WCHAR('I','\0'),  ONE_WCHAR('S','\0'),  ONE_WCHAR('S','\0'),  ONE_WCHAR('U','\0'),  ONE_WCHAR('E','\0'),  ONE_WCHAR('D','\0'),  ONE_WCHAR('A','\0'),  ONE_WCHAR('T','\0'),  ONE_WCHAR('E','\0'),  ONE_WCHAR('>','\0'),  ONE_WCHAR('2','\0'),  ONE_WCHAR('0','\0'),  ONE_WCHAR('0','\0'),  ONE_WCHAR('0','\0'),  ONE_WCHAR('0','\0'),  ONE_WCHAR('1','\0'),  ONE_WCHAR('0','\0'),  ONE_WCHAR('2','\0'),  ONE_WCHAR(' ','\0'),  ONE_WCHAR('2','\0'),  ONE_WCHAR('3','\0'),  ONE_WCHAR(':','\0'),  ONE_WCHAR('2','\0'),  ONE_WCHAR('0','\0'),  ONE_WCHAR(':','\0'),  ONE_WCHAR('1','\0'),  ONE_WCHAR('4','\0'),  ONE_WCHAR('Z','\0'),  ONE_WCHAR('<','\0'),  ONE_WCHAR('/','\0'),  ONE_WCHAR('I','\0'),  ONE_WCHAR('S','\0'),  ONE_WCHAR('S','\0'),  ONE_WCHAR('U','\0'),  ONE_WCHAR('E','\0'),  ONE_WCHAR('D','\0'),  ONE_WCHAR('A','\0'),  ONE_WCHAR('T','\0'),  ONE_WCHAR('E','\0'),  ONE_WCHAR('>','\0'), ONE_WCHAR('\0','\0') };
const DRM_WCHAR Priority [] = { ONE_WCHAR('<','\0'),  ONE_WCHAR('P','\0'),  ONE_WCHAR('R','\0'),  ONE_WCHAR('I','\0'),  ONE_WCHAR('O','\0'),  ONE_WCHAR('R','\0'),  ONE_WCHAR('I','\0'),  ONE_WCHAR('T','\0'),  ONE_WCHAR('Y','\0'),  ONE_WCHAR('>','\0'),  ONE_WCHAR('1','\0'),  ONE_WCHAR('5','\0'),  ONE_WCHAR('<','\0'),  ONE_WCHAR('/','\0'),  ONE_WCHAR('P','\0'),  ONE_WCHAR('R','\0'),  ONE_WCHAR('I','\0'),  ONE_WCHAR('O','\0'),  ONE_WCHAR('R','\0'),  ONE_WCHAR('I','\0'),  ONE_WCHAR('T','\0'),  ONE_WCHAR('Y','\0'),  ONE_WCHAR('>','\0'), ONE_WCHAR('\0','\0') };
const DRM_WCHAR EnablingBitsAlgorithm [] = { ONE_WCHAR('<','\0'),  ONE_WCHAR('A','\0'),  ONE_WCHAR('L','\0'),  ONE_WCHAR('G','\0'),  ONE_WCHAR('O','\0'),  ONE_WCHAR('R','\0'),  ONE_WCHAR('I','\0'),  ONE_WCHAR('T','\0'),  ONE_WCHAR('H','\0'),  ONE_WCHAR('M','\0'),  ONE_WCHAR(' ','\0'),  ONE_WCHAR('t','\0'),  ONE_WCHAR('y','\0'),  ONE_WCHAR('p','\0'),  ONE_WCHAR('e','\0'),  ONE_WCHAR('=','\0'),  ONE_WCHAR('"','\0'),  ONE_WCHAR('M','\0'),  ONE_WCHAR('S','\0'),  ONE_WCHAR('D','\0'),  ONE_WCHAR('R','\0'),  ONE_WCHAR('M','\0'),  ONE_WCHAR('"','\0'),  ONE_WCHAR('>','\0'),  ONE_WCHAR('<','\0'),  ONE_WCHAR('/','\0'),  ONE_WCHAR('A','\0'),  ONE_WCHAR('L','\0'),  ONE_WCHAR('G','\0'),  ONE_WCHAR('O','\0'),  ONE_WCHAR('R','\0'),  ONE_WCHAR('I','\0'),  ONE_WCHAR('T','\0'),  ONE_WCHAR('H','\0'),  ONE_WCHAR('M','\0'),  ONE_WCHAR('>','\0'), ONE_WCHAR('\0','\0') };
const DRM_WCHAR ChainedEnablingBitsAlgorithm  []= { ONE_WCHAR('<','\0'),  ONE_WCHAR('A','\0'),  ONE_WCHAR('L','\0'),  ONE_WCHAR('G','\0'),  ONE_WCHAR('O','\0'),  ONE_WCHAR('R','\0'),  ONE_WCHAR('I','\0'),  ONE_WCHAR('T','\0'),  ONE_WCHAR('H','\0'),  ONE_WCHAR('M','\0'),  ONE_WCHAR(' ','\0'),  ONE_WCHAR('t','\0'),  ONE_WCHAR('y','\0'),  ONE_WCHAR('p','\0'),  ONE_WCHAR('e','\0'),  ONE_WCHAR('=','\0'),  ONE_WCHAR('"','\0'),  ONE_WCHAR('M','\0'),  ONE_WCHAR('S','\0'),  ONE_WCHAR('D','\0'),  ONE_WCHAR('R','\0'),  ONE_WCHAR('M','\0'),  ONE_WCHAR(' ','\0'),  ONE_WCHAR('C','\0'),  ONE_WCHAR('K','\0'),  ONE_WCHAR('"','\0'),  ONE_WCHAR('>','\0'),  ONE_WCHAR('<','\0'),  ONE_WCHAR('/','\0'),  ONE_WCHAR('A','\0'),  ONE_WCHAR('L','\0'),  ONE_WCHAR('G','\0'),  ONE_WCHAR('O','\0'),  ONE_WCHAR('R','\0'),  ONE_WCHAR('I','\0'),  ONE_WCHAR('T','\0'),  ONE_WCHAR('H','\0'),  ONE_WCHAR('M','\0'),  ONE_WCHAR('>','\0'), ONE_WCHAR('\0','\0') };
const DRM_WCHAR HashAlgorithm []  = { ONE_WCHAR('<','\0'),  ONE_WCHAR('H','\0'),  ONE_WCHAR('A','\0'),  ONE_WCHAR('S','\0'),  ONE_WCHAR('H','\0'),  ONE_WCHAR('A','\0'),  ONE_WCHAR('L','\0'),  ONE_WCHAR('G','\0'),  ONE_WCHAR('O','\0'),  ONE_WCHAR('R','\0'),  ONE_WCHAR('I','\0'),  ONE_WCHAR('T','\0'),  ONE_WCHAR('H','\0'),  ONE_WCHAR('M','\0'),  ONE_WCHAR(' ','\0'),  ONE_WCHAR('t','\0'),  ONE_WCHAR('y','\0'),  ONE_WCHAR('p','\0'),  ONE_WCHAR('e','\0'),  ONE_WCHAR('=','\0'),  ONE_WCHAR('"','\0'),  ONE_WCHAR('S','\0'),  ONE_WCHAR('H','\0'),  ONE_WCHAR('A','\0'),  ONE_WCHAR('"','\0'),  ONE_WCHAR('>','\0'),  ONE_WCHAR('<','\0'),  ONE_WCHAR('/','\0'),  ONE_WCHAR('H','\0'),  ONE_WCHAR('A','\0'),  ONE_WCHAR('S','\0'),  ONE_WCHAR('H','\0'),  ONE_WCHAR('A','\0'),  ONE_WCHAR('L','\0'),  ONE_WCHAR('G','\0'),  ONE_WCHAR('O','\0'),  ONE_WCHAR('R','\0'),  ONE_WCHAR('I','\0'),  ONE_WCHAR('T','\0'),  ONE_WCHAR('H','\0'),  ONE_WCHAR('M','\0'),  ONE_WCHAR('>','\0'), ONE_WCHAR('\0','\0') };
const DRM_WCHAR SignAlgorithm []  = { ONE_WCHAR('<','\0'),  ONE_WCHAR('S','\0'),  ONE_WCHAR('I','\0'),  ONE_WCHAR('G','\0'),  ONE_WCHAR('N','\0'),  ONE_WCHAR('A','\0'),  ONE_WCHAR('L','\0'),  ONE_WCHAR('G','\0'),  ONE_WCHAR('O','\0'),  ONE_WCHAR('R','\0'),  ONE_WCHAR('I','\0'),  ONE_WCHAR('T','\0'),  ONE_WCHAR('H','\0'),  ONE_WCHAR('M','\0'),  ONE_WCHAR(' ','\0'),  ONE_WCHAR('t','\0'),  ONE_WCHAR('y','\0'),  ONE_WCHAR('p','\0'),  ONE_WCHAR('e','\0'),  ONE_WCHAR('=','\0'),  ONE_WCHAR('"','\0'),  ONE_WCHAR('M','\0'),  ONE_WCHAR('S','\0'),  ONE_WCHAR('D','\0'),  ONE_WCHAR('R','\0'),  ONE_WCHAR('M','\0'),  ONE_WCHAR('"','\0'),  ONE_WCHAR('>','\0'),  ONE_WCHAR('<','\0'),  ONE_WCHAR('/','\0'),  ONE_WCHAR('S','\0'),  ONE_WCHAR('I','\0'),  ONE_WCHAR('G','\0'),  ONE_WCHAR('N','\0'),  ONE_WCHAR('A','\0'),  ONE_WCHAR('L','\0'),  ONE_WCHAR('G','\0'),  ONE_WCHAR('O','\0'),  ONE_WCHAR('R','\0'),  ONE_WCHAR('I','\0'),  ONE_WCHAR('T','\0'),  ONE_WCHAR('H','\0'),  ONE_WCHAR('M','\0'),  ONE_WCHAR('>','\0'), ONE_WCHAR('\0','\0') };
const DRM_WCHAR HashAlgorithmUnknown   []= { ONE_WCHAR('<','\0'),  ONE_WCHAR('H','\0'),  ONE_WCHAR('A','\0'),  ONE_WCHAR('S','\0'),  ONE_WCHAR('H','\0'),  ONE_WCHAR('A','\0'),  ONE_WCHAR('L','\0'),  ONE_WCHAR('G','\0'),  ONE_WCHAR('O','\0'),  ONE_WCHAR('R','\0'),  ONE_WCHAR('I','\0'),  ONE_WCHAR('T','\0'),  ONE_WCHAR('H','\0'),  ONE_WCHAR('M','\0'),  ONE_WCHAR(' ','\0'),  ONE_WCHAR('t','\0'),  ONE_WCHAR('y','\0'),  ONE_WCHAR('p','\0'),  ONE_WCHAR('e','\0'),  ONE_WCHAR('=','\0'),  ONE_WCHAR('"','\0'),  ONE_WCHAR('U','\0'),  ONE_WCHAR('N','\0'),  ONE_WCHAR('K','\0'),  ONE_WCHAR('N','\0'),  ONE_WCHAR('O','\0'),  ONE_WCHAR('W','\0'),  ONE_WCHAR('N','\0'),  ONE_WCHAR('"','\0'),  ONE_WCHAR('>','\0'),  ONE_WCHAR('<','\0'),  ONE_WCHAR('/','\0'),  ONE_WCHAR('H','\0'),  ONE_WCHAR('A','\0'),  ONE_WCHAR('S','\0'),  ONE_WCHAR('H','\0'),  ONE_WCHAR('A','\0'),  ONE_WCHAR('L','\0'),  ONE_WCHAR('G','\0'),  ONE_WCHAR('O','\0'),  ONE_WCHAR('R','\0'),  ONE_WCHAR('I','\0'),  ONE_WCHAR('T','\0'),  ONE_WCHAR('H','\0'),  ONE_WCHAR('M','\0'),  ONE_WCHAR('>','\0'), ONE_WCHAR('\0','\0') };
const DRM_WCHAR SignAlgorithmUnknown   []= { ONE_WCHAR('<','\0'),  ONE_WCHAR('S','\0'),  ONE_WCHAR('I','\0'),  ONE_WCHAR('G','\0'),  ONE_WCHAR('N','\0'),  ONE_WCHAR('A','\0'),  ONE_WCHAR('L','\0'),  ONE_WCHAR('G','\0'),  ONE_WCHAR('O','\0'),  ONE_WCHAR('R','\0'),  ONE_WCHAR('I','\0'),  ONE_WCHAR('T','\0'),  ONE_WCHAR('H','\0'),  ONE_WCHAR('M','\0'),  ONE_WCHAR(' ','\0'),  ONE_WCHAR('t','\0'),  ONE_WCHAR('y','\0'),  ONE_WCHAR('p','\0'),  ONE_WCHAR('e','\0'),  ONE_WCHAR('=','\0'),  ONE_WCHAR('"','\0'),  ONE_WCHAR('U','\0'),  ONE_WCHAR('N','\0'),  ONE_WCHAR('K','\0'),  ONE_WCHAR('N','\0'),  ONE_WCHAR('O','\0'),  ONE_WCHAR('W','\0'),  ONE_WCHAR('N','\0'),  ONE_WCHAR('"','\0'),  ONE_WCHAR('>','\0'),  ONE_WCHAR('<','\0'),  ONE_WCHAR('/','\0'),  ONE_WCHAR('S','\0'),  ONE_WCHAR('I','\0'),  ONE_WCHAR('G','\0'),  ONE_WCHAR('N','\0'),  ONE_WCHAR('A','\0'),  ONE_WCHAR('L','\0'),  ONE_WCHAR('G','\0'),  ONE_WCHAR('O','\0'),  ONE_WCHAR('R','\0'),  ONE_WCHAR('I','\0'),  ONE_WCHAR('T','\0'),  ONE_WCHAR('H','\0'),  ONE_WCHAR('M','\0'),  ONE_WCHAR('>','\0'), ONE_WCHAR('\0','\0') };
const DRM_WCHAR BrLicSrvPriv  [] = { ONE_WCHAR('h','\0'),  ONE_WCHAR('j','\0'),  ONE_WCHAR('w','\0'),  ONE_WCHAR('R','\0'),  ONE_WCHAR('Y','\0'),  ONE_WCHAR('B','\0'),  ONE_WCHAR('v','\0'),  ONE_WCHAR('i','\0'),  ONE_WCHAR('j','\0'),  ONE_WCHAR('N','\0'),  ONE_WCHAR('J','\0'),  ONE_WCHAR('f','\0'),  ONE_WCHAR('g','\0'),  ONE_WCHAR('i','\0'),  ONE_WCHAR('e','\0'),  ONE_WCHAR('t','\0'),  ONE_WCHAR('z','\0'),  ONE_WCHAR('h','\0'),  ONE_WCHAR('h','\0'),  ONE_WCHAR('X','\0'),  ONE_WCHAR('1','\0'),  ONE_WCHAR('U','\0'),  ONE_WCHAR('G','\0'),  ONE_WCHAR('J','\0'),  ONE_WCHAR('O','\0'),  ONE_WCHAR('R','\0'),  ONE_WCHAR('A','\0'),  ONE_WCHAR('=','\0'), ONE_WCHAR('\0','\0') };
const DRM_WCHAR ContentRevocationCondition  []= { ONE_WCHAR('<','\0'),  ONE_WCHAR('C','\0'),  ONE_WCHAR('O','\0'),  ONE_WCHAR('N','\0'),  ONE_WCHAR('D','\0'),  ONE_WCHAR('I','\0'),  ONE_WCHAR('T','\0'),  ONE_WCHAR('I','\0'),  ONE_WCHAR('O','\0'),  ONE_WCHAR('N','\0'),  ONE_WCHAR('>','\0'),  ONE_WCHAR('<','\0'),  ONE_WCHAR('!','\0'),  ONE_WCHAR('[','\0'),  ONE_WCHAR('C','\0'),  ONE_WCHAR('D','\0'),  ONE_WCHAR('A','\0'),  ONE_WCHAR('T','\0'),  ONE_WCHAR('A','\0'),  ONE_WCHAR('[','\0'),  ONE_WCHAR('(','\0'),  ONE_WCHAR('!','\0'),  ONE_WCHAR('e','\0'),  ONE_WCHAR('x','\0'),  ONE_WCHAR('i','\0'),  ONE_WCHAR('s','\0'),  ONE_WCHAR('t','\0'),  ONE_WCHAR('s','\0'),  ONE_WCHAR('(','\0'),  ONE_WCHAR('l','\0'),
                                           ONE_WCHAR('i','\0'),  ONE_WCHAR('c','\0'),  ONE_WCHAR('e','\0'),  ONE_WCHAR('n','\0'),  ONE_WCHAR('s','\0'),  ONE_WCHAR('e','\0'),  ONE_WCHAR('.','\0'),  ONE_WCHAR('C','\0'),  ONE_WCHAR('O','\0'),  ONE_WCHAR('N','\0'),  ONE_WCHAR('T','\0'),  ONE_WCHAR('E','\0'),  ONE_WCHAR('N','\0'),  ONE_WCHAR('T','\0'),  ONE_WCHAR('R','\0'),  ONE_WCHAR('E','\0'),  ONE_WCHAR('V','\0'),  ONE_WCHAR('O','\0'),  ONE_WCHAR('C','\0'),  ONE_WCHAR('A','\0'),  ONE_WCHAR('T','\0'),  ONE_WCHAR('I','\0'),  ONE_WCHAR('O','\0'),  ONE_WCHAR('N','\0'),  ONE_WCHAR(')','\0'),  ONE_WCHAR('?','\0'),  ONE_WCHAR('d','\0'),  ONE_WCHAR('e','\0'),  ONE_WCHAR('l','\0'),  ONE_WCHAR('e','\0'),  ONE_WCHAR('t','\0'),
                                           ONE_WCHAR('e','\0'),  ONE_WCHAR('l','\0'),  ONE_WCHAR('i','\0'),  ONE_WCHAR('c','\0'),  ONE_WCHAR('e','\0'),  ONE_WCHAR('n','\0'),  ONE_WCHAR('s','\0'),  ONE_WCHAR('e','\0'),  ONE_WCHAR('(','\0'),  ONE_WCHAR(')','\0'),  ONE_WCHAR(':','\0'),  ONE_WCHAR('0','\0'),  ONE_WCHAR(')','\0'),  ONE_WCHAR(' ','\0'),  ONE_WCHAR(';','\0'),  ONE_WCHAR(' ','\0'),  ONE_WCHAR('0','\0'),  ONE_WCHAR(']','\0'),  ONE_WCHAR(']','\0'),  ONE_WCHAR('>','\0'),  ONE_WCHAR('<','\0'),  ONE_WCHAR('/','\0'),  ONE_WCHAR('C','\0'),  ONE_WCHAR('O','\0'),  ONE_WCHAR('N','\0'),  ONE_WCHAR('D','\0'),  ONE_WCHAR('I','\0'),  ONE_WCHAR('T','\0'),  ONE_WCHAR('I','\0'),  ONE_WCHAR('O','\0'),  ONE_WCHAR('N','\0'),  ONE_WCHAR('>','\0'), ONE_WCHAR('\0','\0') };
const DRM_WCHAR SYMVALUE      [] = { ONE_WCHAR('S','\0'),  ONE_WCHAR('Y','\0'),  ONE_WCHAR('M','\0'),  ONE_WCHAR('V','\0'),  ONE_WCHAR('A','\0'),  ONE_WCHAR('L','\0'),  ONE_WCHAR('U','\0'),  ONE_WCHAR('E','\0'), ONE_WCHAR('\0','\0') };
const DRM_WCHAR SYMSIGNATURE  [] = { ONE_WCHAR('S','\0'),  ONE_WCHAR('Y','\0'),  ONE_WCHAR('M','\0'),  ONE_WCHAR('S','\0'),  ONE_WCHAR('I','\0'),  ONE_WCHAR('G','\0'),  ONE_WCHAR('N','\0'),  ONE_WCHAR('A','\0'),  ONE_WCHAR('T','\0'),  ONE_WCHAR('U','\0'),  ONE_WCHAR('R','\0'),  ONE_WCHAR('E','\0'), ONE_WCHAR('\0','\0') };

/*
    L"<ONACTION type=\"Restore\"><CONDITION><![CDATA[1]]></CONDITION><ACTION><![CDATA[2]]></ACTION></ONACTION>"
    L"<ONACTION type=\"Backup\"><ACTION><![CDATA[1]]></ACTION></ONACTION>"
    L"<ONACTION type=\"CREATE_PM_LICENSE\"><CONDITION><![CDATA[pmlicense.rights = 19;pmlicense.appseclevel = 150;1]]></CONDITION></ONACTION>"
    L"<ONACTION type=\"Play\"><CONDITION><![CDATA[1]]></CONDITION></ONACTION>"
    L"<ONSELECT><CONDITION><![CDATA[(!exists(secstate.deleted) && (app.minseclevel >= 150?1:(secstate.drmreason=3;0)))]]></CONDITION></ONSELECT>"
    L"<ONSTORE><CONDITION><![CDATA[1]]></CONDITION><ACTION><![CDATA[!exists(secstate.MinAppSecLevel)?secstate.MinAppSecLevel=150:1]]></ACTION></ONSTORE>"
*/
const DRM_WCHAR Rights [] = { ONE_WCHAR('<','\0'),  ONE_WCHAR('O','\0'),  ONE_WCHAR('N','\0'),  ONE_WCHAR('A','\0'),  ONE_WCHAR('C','\0'),  ONE_WCHAR('T','\0'),  ONE_WCHAR('I','\0'),  ONE_WCHAR('O','\0'),  ONE_WCHAR('N','\0'),  ONE_WCHAR(' ','\0'),  ONE_WCHAR('t','\0'),  ONE_WCHAR('y','\0'),  ONE_WCHAR('p','\0'),  ONE_WCHAR('e','\0'),  ONE_WCHAR('=','\0'),  ONE_WCHAR('"','\0'),  ONE_WCHAR('R','\0'),  ONE_WCHAR('e','\0'),  ONE_WCHAR('s','\0'),  ONE_WCHAR('t','\0'),  ONE_WCHAR('o','\0'),  ONE_WCHAR('r','\0'),  ONE_WCHAR('e','\0'),  ONE_WCHAR('"','\0'),  ONE_WCHAR('>','\0'),  ONE_WCHAR('<','\0'),  ONE_WCHAR('C','\0'),  ONE_WCHAR('O','\0'),  ONE_WCHAR('N','\0'),  ONE_WCHAR('D','\0'),  ONE_WCHAR('I','\0'),  ONE_WCHAR('T','\0'),  ONE_WCHAR('I','\0'),  ONE_WCHAR('O','\0'),
                      ONE_WCHAR('N','\0'),  ONE_WCHAR('>','\0'),  ONE_WCHAR('<','\0'),  ONE_WCHAR('!','\0'),  ONE_WCHAR('[','\0'),  ONE_WCHAR('C','\0'),  ONE_WCHAR('D','\0'),  ONE_WCHAR('A','\0'),  ONE_WCHAR('T','\0'),  ONE_WCHAR('A','\0'),  ONE_WCHAR('[','\0'),  ONE_WCHAR('1','\0'),  ONE_WCHAR(']','\0'),  ONE_WCHAR(']','\0'),  ONE_WCHAR('>','\0'),  ONE_WCHAR('<','\0'),  ONE_WCHAR('/','\0'),  ONE_WCHAR('C','\0'),  ONE_WCHAR('O','\0'),  ONE_WCHAR('N','\0'),  ONE_WCHAR('D','\0'),  ONE_WCHAR('I','\0'),  ONE_WCHAR('T','\0'),  ONE_WCHAR('I','\0'),  ONE_WCHAR('O','\0'),  ONE_WCHAR('N','\0'),  ONE_WCHAR('>','\0'),  ONE_WCHAR('<','\0'),  ONE_WCHAR('A','\0'),  ONE_WCHAR('C','\0'),  ONE_WCHAR('T','\0'),  ONE_WCHAR('I','\0'),  ONE_WCHAR('O','\0'),  ONE_WCHAR('N','\0'),  ONE_WCHAR('>','\0'),
                      ONE_WCHAR('<','\0'),  ONE_WCHAR('!','\0'),  ONE_WCHAR('[','\0'),  ONE_WCHAR('C','\0'),  ONE_WCHAR('D','\0'),  ONE_WCHAR('A','\0'),  ONE_WCHAR('T','\0'),  ONE_WCHAR('A','\0'),  ONE_WCHAR('[','\0'),  ONE_WCHAR('2','\0'),  ONE_WCHAR(']','\0'),  ONE_WCHAR(']','\0'),  ONE_WCHAR('>','\0'),  ONE_WCHAR('<','\0'),  ONE_WCHAR('/','\0'),  ONE_WCHAR('A','\0'),  ONE_WCHAR('C','\0'),  ONE_WCHAR('T','\0'),  ONE_WCHAR('I','\0'),  ONE_WCHAR('O','\0'),  ONE_WCHAR('N','\0'),  ONE_WCHAR('>','\0'),  ONE_WCHAR('<','\0'),  ONE_WCHAR('/','\0'),  ONE_WCHAR('O','\0'),  ONE_WCHAR('N','\0'),  ONE_WCHAR('A','\0'),  ONE_WCHAR('C','\0'),  ONE_WCHAR('T','\0'),  ONE_WCHAR('I','\0'),  ONE_WCHAR('O','\0'),  ONE_WCHAR('N','\0'),  ONE_WCHAR('>','\0'),
                      ONE_WCHAR('<','\0'),  ONE_WCHAR('O','\0'),  ONE_WCHAR('N','\0'),  ONE_WCHAR('A','\0'),  ONE_WCHAR('C','\0'),  ONE_WCHAR('T','\0'),  ONE_WCHAR('I','\0'),  ONE_WCHAR('O','\0'),  ONE_WCHAR('N','\0'),  ONE_WCHAR(' ','\0'),  ONE_WCHAR('t','\0'),  ONE_WCHAR('y','\0'),  ONE_WCHAR('p','\0'),  ONE_WCHAR('e','\0'),  ONE_WCHAR('=','\0'),  ONE_WCHAR('"','\0'),  ONE_WCHAR('B','\0'),  ONE_WCHAR('a','\0'),  ONE_WCHAR('c','\0'),  ONE_WCHAR('k','\0'),  ONE_WCHAR('u','\0'),  ONE_WCHAR('p','\0'),  ONE_WCHAR('"','\0'),  ONE_WCHAR('>','\0'),  ONE_WCHAR('<','\0'),  ONE_WCHAR('A','\0'),  ONE_WCHAR('C','\0'),  ONE_WCHAR('T','\0'),  ONE_WCHAR('I','\0'),  ONE_WCHAR('O','\0'),  ONE_WCHAR('N','\0'),  ONE_WCHAR('>','\0'),  ONE_WCHAR('<','\0'),  ONE_WCHAR('!','\0'),  ONE_WCHAR('[','\0'),  ONE_WCHAR('C','\0'),
                      ONE_WCHAR('D','\0'),  ONE_WCHAR('A','\0'),  ONE_WCHAR('T','\0'),  ONE_WCHAR('A','\0'),  ONE_WCHAR('[','\0'),  ONE_WCHAR('1','\0'),  ONE_WCHAR(']','\0'),  ONE_WCHAR(']','\0'),  ONE_WCHAR('>','\0'),  ONE_WCHAR('<','\0'),  ONE_WCHAR('/','\0'),  ONE_WCHAR('A','\0'),  ONE_WCHAR('C','\0'),  ONE_WCHAR('T','\0'),  ONE_WCHAR('I','\0'),  ONE_WCHAR('O','\0'),  ONE_WCHAR('N','\0'),  ONE_WCHAR('>','\0'),  ONE_WCHAR('<','\0'),  ONE_WCHAR('/','\0'),  ONE_WCHAR('O','\0'),  ONE_WCHAR('N','\0'),  ONE_WCHAR('A','\0'),  ONE_WCHAR('C','\0'),  ONE_WCHAR('T','\0'),  ONE_WCHAR('I','\0'),  ONE_WCHAR('O','\0'),  ONE_WCHAR('N','\0'),  ONE_WCHAR('>','\0'),  ONE_WCHAR('<','\0'),  ONE_WCHAR('O','\0'),  ONE_WCHAR('N','\0'),  ONE_WCHAR('A','\0'),  ONE_WCHAR('C','\0'),  ONE_WCHAR('T','\0'),  ONE_WCHAR('I','\0'),
                      ONE_WCHAR('O','\0'),  ONE_WCHAR('N','\0'),  ONE_WCHAR(' ','\0'),  ONE_WCHAR('t','\0'),  ONE_WCHAR('y','\0'),  ONE_WCHAR('p','\0'),  ONE_WCHAR('e','\0'),  ONE_WCHAR('=','\0'),  ONE_WCHAR('"','\0'),  ONE_WCHAR('C','\0'),  ONE_WCHAR('R','\0'),  ONE_WCHAR('E','\0'),  ONE_WCHAR('A','\0'),  ONE_WCHAR('T','\0'),  ONE_WCHAR('E','\0'),  ONE_WCHAR('_','\0'),  ONE_WCHAR('P','\0'),  ONE_WCHAR('M','\0'),  ONE_WCHAR('_','\0'),  ONE_WCHAR('L','\0'),  ONE_WCHAR('I','\0'),  ONE_WCHAR('C','\0'),  ONE_WCHAR('E','\0'),  ONE_WCHAR('N','\0'),  ONE_WCHAR('S','\0'),  ONE_WCHAR('E','\0'),  ONE_WCHAR('"','\0'),  ONE_WCHAR('>','\0'),  ONE_WCHAR('<','\0'),  ONE_WCHAR('C','\0'),  ONE_WCHAR('O','\0'),  ONE_WCHAR('N','\0'),  ONE_WCHAR('D','\0'),  ONE_WCHAR('I','\0'),  ONE_WCHAR('T','\0'),  ONE_WCHAR('I','\0'),  ONE_WCHAR('O','\0'),  ONE_WCHAR('N','\0'),
                      ONE_WCHAR('>','\0'),  ONE_WCHAR('<','\0'),  ONE_WCHAR('!','\0'),  ONE_WCHAR('[','\0'),  ONE_WCHAR('C','\0'),  ONE_WCHAR('D','\0'),  ONE_WCHAR('A','\0'),  ONE_WCHAR('T','\0'),  ONE_WCHAR('A','\0'),  ONE_WCHAR('[','\0'),  ONE_WCHAR('p','\0'),  ONE_WCHAR('m','\0'),  ONE_WCHAR('l','\0'),  ONE_WCHAR('i','\0'),  ONE_WCHAR('c','\0'),  ONE_WCHAR('e','\0'),  ONE_WCHAR('n','\0'),  ONE_WCHAR('s','\0'),  ONE_WCHAR('e','\0'),  ONE_WCHAR('.','\0'),  ONE_WCHAR('r','\0'),  ONE_WCHAR('i','\0'),  ONE_WCHAR('g','\0'),  ONE_WCHAR('h','\0'),  ONE_WCHAR('t','\0'),  ONE_WCHAR('s','\0'),  ONE_WCHAR(' ','\0'),  ONE_WCHAR('=','\0'),  ONE_WCHAR(' ','\0'),  ONE_WCHAR('1','\0'),  ONE_WCHAR('9','\0'),  ONE_WCHAR(';','\0'),  ONE_WCHAR('p','\0'),  ONE_WCHAR('m','\0'),  ONE_WCHAR('l','\0'),  ONE_WCHAR('i','\0'),  ONE_WCHAR('c','\0'),  ONE_WCHAR('e','\0'),  ONE_WCHAR('n','\0'),  ONE_WCHAR('s','\0'),  ONE_WCHAR('e','\0'),  ONE_WCHAR('.','\0'),  ONE_WCHAR('a','\0'),  ONE_WCHAR('p','\0'),  ONE_WCHAR('p','\0'),  ONE_WCHAR('s','\0'),  ONE_WCHAR('e','\0'),  ONE_WCHAR('c','\0'),
                      ONE_WCHAR('l','\0'),  ONE_WCHAR('e','\0'),  ONE_WCHAR('v','\0'),  ONE_WCHAR('e','\0'),  ONE_WCHAR('l','\0'),  ONE_WCHAR(' ','\0'),  ONE_WCHAR('=','\0'),  ONE_WCHAR(' ','\0'),  ONE_WCHAR('1','\0'),  ONE_WCHAR('5','\0'),  ONE_WCHAR('0','\0'),  ONE_WCHAR(';','\0'),  ONE_WCHAR('1','\0'),  ONE_WCHAR(']','\0'),  ONE_WCHAR(']','\0'),  ONE_WCHAR('>','\0'),  ONE_WCHAR('<','\0'),  ONE_WCHAR('/','\0'),  ONE_WCHAR('C','\0'),  ONE_WCHAR('O','\0'),  ONE_WCHAR('N','\0'),  ONE_WCHAR('D','\0'),  ONE_WCHAR('I','\0'),  ONE_WCHAR('T','\0'),  ONE_WCHAR('I','\0'),  ONE_WCHAR('O','\0'),  ONE_WCHAR('N','\0'),  ONE_WCHAR('>','\0'),  ONE_WCHAR('<','\0'),  ONE_WCHAR('/','\0'),  ONE_WCHAR('O','\0'),  ONE_WCHAR('N','\0'),  ONE_WCHAR('A','\0'),  ONE_WCHAR('C','\0'),  ONE_WCHAR('T','\0'),  ONE_WCHAR('I','\0'),  ONE_WCHAR('O','\0'),  ONE_WCHAR('N','\0'),  ONE_WCHAR('>','\0'),  ONE_WCHAR('<','\0'),  ONE_WCHAR('O','\0'),  ONE_WCHAR('N','\0'),  ONE_WCHAR('A','\0'),  ONE_WCHAR('C','\0'),  ONE_WCHAR('T','\0'),  ONE_WCHAR('I','\0'),  ONE_WCHAR('O','\0'),  ONE_WCHAR('N','\0'),
                      ONE_WCHAR(' ','\0'),  ONE_WCHAR('t','\0'),  ONE_WCHAR('y','\0'),  ONE_WCHAR('p','\0'),  ONE_WCHAR('e','\0'),  ONE_WCHAR('=','\0'),  ONE_WCHAR('"','\0'),  ONE_WCHAR('P','\0'),  ONE_WCHAR('l','\0'),  ONE_WCHAR('a','\0'),  ONE_WCHAR('y','\0'),  ONE_WCHAR('"','\0'),  ONE_WCHAR('>','\0'),  ONE_WCHAR('<','\0'),  ONE_WCHAR('C','\0'),  ONE_WCHAR('O','\0'),  ONE_WCHAR('N','\0'),  ONE_WCHAR('D','\0'),  ONE_WCHAR('I','\0'),  ONE_WCHAR('T','\0'),  ONE_WCHAR('I','\0'),  ONE_WCHAR('O','\0'),  ONE_WCHAR('N','\0'),  ONE_WCHAR('>','\0'),  ONE_WCHAR('<','\0'),  ONE_WCHAR('!','\0'),  ONE_WCHAR('[','\0'),  ONE_WCHAR('C','\0'),  ONE_WCHAR('D','\0'),  ONE_WCHAR('A','\0'),  ONE_WCHAR('T','\0'),  ONE_WCHAR('A','\0'),  ONE_WCHAR('[','\0'),  ONE_WCHAR('1','\0'),  ONE_WCHAR(']','\0'),  ONE_WCHAR(']','\0'),  ONE_WCHAR('>','\0'),  ONE_WCHAR('<','\0'),  ONE_WCHAR('/','\0'),  ONE_WCHAR('C','\0'),  ONE_WCHAR('O','\0'),  ONE_WCHAR('N','\0'),  ONE_WCHAR('D','\0'),  ONE_WCHAR('I','\0'),  ONE_WCHAR('T','\0'),  ONE_WCHAR('I','\0'),  ONE_WCHAR('O','\0'),
                      ONE_WCHAR('N','\0'),  ONE_WCHAR('>','\0'),  ONE_WCHAR('<','\0'),  ONE_WCHAR('/','\0'),  ONE_WCHAR('O','\0'),  ONE_WCHAR('N','\0'),  ONE_WCHAR('A','\0'),  ONE_WCHAR('C','\0'),  ONE_WCHAR('T','\0'),  ONE_WCHAR('I','\0'),  ONE_WCHAR('O','\0'),  ONE_WCHAR('N','\0'),  ONE_WCHAR('>','\0'),  ONE_WCHAR('<','\0'),  ONE_WCHAR('O','\0'),  ONE_WCHAR('N','\0'),  ONE_WCHAR('S','\0'),  ONE_WCHAR('E','\0'),  ONE_WCHAR('L','\0'),  ONE_WCHAR('E','\0'),  ONE_WCHAR('C','\0'),  ONE_WCHAR('T','\0'),  ONE_WCHAR('>','\0'),  ONE_WCHAR('<','\0'),  ONE_WCHAR('C','\0'),  ONE_WCHAR('O','\0'),  ONE_WCHAR('N','\0'),  ONE_WCHAR('D','\0'),  ONE_WCHAR('I','\0'),  ONE_WCHAR('T','\0'),  ONE_WCHAR('I','\0'),  ONE_WCHAR('O','\0'),  ONE_WCHAR('N','\0'),  ONE_WCHAR('>','\0'),  ONE_WCHAR('<','\0'),  ONE_WCHAR('!','\0'),  ONE_WCHAR('[','\0'),  ONE_WCHAR('C','\0'),  ONE_WCHAR('D','\0'),  ONE_WCHAR('A','\0'),  ONE_WCHAR('T','\0'),  ONE_WCHAR('A','\0'),  ONE_WCHAR('[','\0'),  ONE_WCHAR('(','\0'),  ONE_WCHAR('!','\0'),  ONE_WCHAR('e','\0'),  ONE_WCHAR('x','\0'),  ONE_WCHAR('i','\0'),
                      ONE_WCHAR('s','\0'),  ONE_WCHAR('t','\0'),  ONE_WCHAR('s','\0'),  ONE_WCHAR('(','\0'),  ONE_WCHAR('s','\0'),  ONE_WCHAR('e','\0'),  ONE_WCHAR('c','\0'),  ONE_WCHAR('s','\0'),  ONE_WCHAR('t','\0'),  ONE_WCHAR('a','\0'),  ONE_WCHAR('t','\0'),  ONE_WCHAR('e','\0'),  ONE_WCHAR('.','\0'),  ONE_WCHAR('d','\0'),  ONE_WCHAR('e','\0'),  ONE_WCHAR('l','\0'),  ONE_WCHAR('e','\0'),  ONE_WCHAR('t','\0'),  ONE_WCHAR('e','\0'),  ONE_WCHAR('d','\0'),  ONE_WCHAR(')','\0'),  ONE_WCHAR(' ','\0'),  ONE_WCHAR('&','\0'),  ONE_WCHAR('&','\0'),  ONE_WCHAR(' ','\0'),  ONE_WCHAR('(','\0'),  ONE_WCHAR('a','\0'),  ONE_WCHAR('p','\0'),  ONE_WCHAR('p','\0'),  ONE_WCHAR('.','\0'),  ONE_WCHAR('m','\0'),  ONE_WCHAR('i','\0'),  ONE_WCHAR('n','\0'),  ONE_WCHAR('s','\0'),  ONE_WCHAR('e','\0'),  ONE_WCHAR('c','\0'),  ONE_WCHAR('l','\0'),  ONE_WCHAR('e','\0'),  ONE_WCHAR('v','\0'),  ONE_WCHAR('e','\0'),  ONE_WCHAR('l','\0'),  ONE_WCHAR(' ','\0'),  ONE_WCHAR('>','\0'),  ONE_WCHAR('=','\0'),  ONE_WCHAR(' ','\0'),  ONE_WCHAR('1','\0'),  ONE_WCHAR('5','\0'),  ONE_WCHAR('0','\0'),
                      ONE_WCHAR('?','\0'),  ONE_WCHAR('1','\0'),  ONE_WCHAR(':','\0'),  ONE_WCHAR('(','\0'),  ONE_WCHAR('s','\0'),  ONE_WCHAR('e','\0'),  ONE_WCHAR('c','\0'),  ONE_WCHAR('s','\0'),  ONE_WCHAR('t','\0'),  ONE_WCHAR('a','\0'),  ONE_WCHAR('t','\0'),  ONE_WCHAR('e','\0'),  ONE_WCHAR('.','\0'),  ONE_WCHAR('d','\0'),  ONE_WCHAR('r','\0'),  ONE_WCHAR('m','\0'),  ONE_WCHAR('r','\0'),  ONE_WCHAR('e','\0'),  ONE_WCHAR('a','\0'),  ONE_WCHAR('s','\0'),  ONE_WCHAR('o','\0'),  ONE_WCHAR('n','\0'),  ONE_WCHAR('=','\0'),  ONE_WCHAR('3','\0'),  ONE_WCHAR(';','\0'),  ONE_WCHAR('0','\0'),  ONE_WCHAR(')','\0'),  ONE_WCHAR(')','\0'),  ONE_WCHAR(')','\0'),  ONE_WCHAR(']','\0'),  ONE_WCHAR(']','\0'),  ONE_WCHAR('>','\0'),  ONE_WCHAR('<','\0'),  ONE_WCHAR('/','\0'),  ONE_WCHAR('C','\0'),  ONE_WCHAR('O','\0'),  ONE_WCHAR('N','\0'),  ONE_WCHAR('D','\0'),  ONE_WCHAR('I','\0'),  ONE_WCHAR('T','\0'),  ONE_WCHAR('I','\0'),  ONE_WCHAR('O','\0'),  ONE_WCHAR('N','\0'),
                      ONE_WCHAR('>','\0'),  ONE_WCHAR('<','\0'),  ONE_WCHAR('/','\0'),  ONE_WCHAR('O','\0'),  ONE_WCHAR('N','\0'),  ONE_WCHAR('S','\0'),  ONE_WCHAR('E','\0'),  ONE_WCHAR('L','\0'),  ONE_WCHAR('E','\0'),  ONE_WCHAR('C','\0'),  ONE_WCHAR('T','\0'),  ONE_WCHAR('>','\0'),  ONE_WCHAR('<','\0'),  ONE_WCHAR('O','\0'),  ONE_WCHAR('N','\0'),  ONE_WCHAR('S','\0'),  ONE_WCHAR('T','\0'),  ONE_WCHAR('O','\0'),  ONE_WCHAR('R','\0'),  ONE_WCHAR('E','\0'),  ONE_WCHAR('>','\0'),  ONE_WCHAR('<','\0'),  ONE_WCHAR('C','\0'),  ONE_WCHAR('O','\0'),  ONE_WCHAR('N','\0'),  ONE_WCHAR('D','\0'),  ONE_WCHAR('I','\0'),  ONE_WCHAR('T','\0'),  ONE_WCHAR('I','\0'),  ONE_WCHAR('O','\0'),  ONE_WCHAR('N','\0'),  ONE_WCHAR('>','\0'),  ONE_WCHAR('<','\0'),  ONE_WCHAR('!','\0'),  ONE_WCHAR('[','\0'),  ONE_WCHAR('C','\0'),  ONE_WCHAR('D','\0'),  ONE_WCHAR('A','\0'),  ONE_WCHAR('T','\0'),  ONE_WCHAR('A','\0'),  ONE_WCHAR('[','\0'),  ONE_WCHAR('1','\0'),  ONE_WCHAR(']','\0'),  ONE_WCHAR(']','\0'),
                      ONE_WCHAR('>','\0'),  ONE_WCHAR('<','\0'),  ONE_WCHAR('/','\0'),  ONE_WCHAR('C','\0'),  ONE_WCHAR('O','\0'),  ONE_WCHAR('N','\0'),  ONE_WCHAR('D','\0'),  ONE_WCHAR('I','\0'),  ONE_WCHAR('T','\0'),  ONE_WCHAR('I','\0'),  ONE_WCHAR('O','\0'),  ONE_WCHAR('N','\0'),  ONE_WCHAR('>','\0'),  ONE_WCHAR('<','\0'),  ONE_WCHAR('A','\0'),  ONE_WCHAR('C','\0'),  ONE_WCHAR('T','\0'),  ONE_WCHAR('I','\0'),  ONE_WCHAR('O','\0'),  ONE_WCHAR('N','\0'),  ONE_WCHAR('>','\0'),  ONE_WCHAR('<','\0'),  ONE_WCHAR('!','\0'),  ONE_WCHAR('[','\0'),  ONE_WCHAR('C','\0'),  ONE_WCHAR('D','\0'),  ONE_WCHAR('A','\0'),  ONE_WCHAR('T','\0'),  ONE_WCHAR('A','\0'),  ONE_WCHAR('[','\0'),  ONE_WCHAR('!','\0'),  ONE_WCHAR('e','\0'),  ONE_WCHAR('x','\0'),  ONE_WCHAR('i','\0'),  ONE_WCHAR('s','\0'),  ONE_WCHAR('t','\0'),  ONE_WCHAR('s','\0'),  ONE_WCHAR('(','\0'),  ONE_WCHAR('s','\0'),  ONE_WCHAR('e','\0'),  ONE_WCHAR('c','\0'),  ONE_WCHAR('s','\0'),  ONE_WCHAR('t','\0'),
                      ONE_WCHAR('a','\0'),  ONE_WCHAR('t','\0'),  ONE_WCHAR('e','\0'),  ONE_WCHAR('.','\0'),  ONE_WCHAR('M','\0'),  ONE_WCHAR('i','\0'),  ONE_WCHAR('n','\0'),  ONE_WCHAR('A','\0'),  ONE_WCHAR('p','\0'),  ONE_WCHAR('p','\0'),  ONE_WCHAR('S','\0'),  ONE_WCHAR('e','\0'),  ONE_WCHAR('c','\0'),  ONE_WCHAR('L','\0'),  ONE_WCHAR('e','\0'),  ONE_WCHAR('v','\0'),  ONE_WCHAR('e','\0'),  ONE_WCHAR('l','\0'),  ONE_WCHAR(')','\0'),  ONE_WCHAR('?','\0'),  ONE_WCHAR('s','\0'),  ONE_WCHAR('e','\0'),  ONE_WCHAR('c','\0'),  ONE_WCHAR('s','\0'),  ONE_WCHAR('t','\0'),  ONE_WCHAR('a','\0'),  ONE_WCHAR('t','\0'),  ONE_WCHAR('e','\0'),  ONE_WCHAR('.','\0'),  ONE_WCHAR('M','\0'),  ONE_WCHAR('i','\0'),  ONE_WCHAR('n','\0'),  ONE_WCHAR('A','\0'),  ONE_WCHAR('p','\0'),  ONE_WCHAR('p','\0'),  ONE_WCHAR('S','\0'),  ONE_WCHAR('e','\0'),  ONE_WCHAR('c','\0'),  ONE_WCHAR('L','\0'),  ONE_WCHAR('e','\0'),  ONE_WCHAR('v','\0'),  ONE_WCHAR('e','\0'),  ONE_WCHAR('l','\0'),  ONE_WCHAR('=','\0'),
                      ONE_WCHAR('1','\0'),  ONE_WCHAR('5','\0'),  ONE_WCHAR('0','\0'),  ONE_WCHAR(':','\0'),  ONE_WCHAR('1','\0'),  ONE_WCHAR(']','\0'),  ONE_WCHAR(']','\0'),  ONE_WCHAR('>','\0'),  ONE_WCHAR('<','\0'),  ONE_WCHAR('/','\0'),  ONE_WCHAR('A','\0'),  ONE_WCHAR('C','\0'),  ONE_WCHAR('T','\0'),  ONE_WCHAR('I','\0'),  ONE_WCHAR('O','\0'),  ONE_WCHAR('N','\0'),  ONE_WCHAR('>','\0'),  ONE_WCHAR('<','\0'),  ONE_WCHAR('/','\0'),  ONE_WCHAR('O','\0'),  ONE_WCHAR('N','\0'),  ONE_WCHAR('S','\0'),  ONE_WCHAR('T','\0'),  ONE_WCHAR('O','\0'),  ONE_WCHAR('R','\0'),  ONE_WCHAR('E','\0'),  ONE_WCHAR('>','\0'), ONE_WCHAR('\0','\0') };

const DRM_CONST_STRING g_cszXMLHeader = CREATE_DRM_STRING   (XMLHeader);/*(L"<?xml version=\"1.0\"?>" );*/
const DRM_CONST_STRING g_cszLicense = CREATE_DRM_STRING (License);/* L"LICENSE" );*/
const DRM_CONST_STRING g_cszVersionAttr = CREATE_DRM_STRING ( VersionAttr);/*L"version" );*/
const DRM_CONST_STRING g_cszVersionValue = CREATE_DRM_STRING (VersionValue);/* L"2.0.0.0" );*/
const DRM_CONST_STRING g_cszLicensorInfo = CREATE_DRM_STRING ( LicensorInfo);/*L"LICENSORINFO" );*/
const DRM_CONST_STRING g_cszDRMRestoreInfo = CREATE_DRM_STRING (DRMRestoreInfo);/*L"DRMRESTOREINFO" );*/
const DRM_CONST_STRING g_cszData = CREATE_DRM_STRING (Data);/*L"DATA" );*/
const DRM_CONST_STRING g_cszSignature = CREATE_DRM_STRING ( Signature);/*L"SIGNATURE" );*/
const DRM_CONST_STRING g_cszCertChain = CREATE_DRM_STRING ( CertChain);/*L"CERTIFICATECHAIN" );*/
const DRM_CONST_STRING g_cszCertificate = CREATE_DRM_STRING (Certificate);/*L"CERTIFICATE" );*/
const DRM_CONST_STRING g_cszEnablingBits = CREATE_DRM_STRING (EnablingBits);/* L"ENABLINGBITS" );*/
const DRM_CONST_STRING g_cszChainedEnablingBits = CREATE_DRM_STRING (ChainedEnablingBits);/*L"CHAINEDENABLINGBITS" );*/
const DRM_CONST_STRING g_cszChainedCheckSum = CREATE_DRM_STRING ( ChainedCheckSum);/*L"CHAINCHECKSUM" );*/
const DRM_CONST_STRING g_cszContentRevocation = CREATE_DRM_STRING (ContentRevocation);/* L"CONTENTREVOCATION" );*/
const DRM_CONST_STRING g_cszValue = CREATE_DRM_STRING (Value);/* L"VALUE" );*/
const DRM_CONST_STRING g_cszContentPubKey = CREATE_DRM_STRING (ContentPubKey);/*L"CONTENTPUBKEY");*/
const DRM_CONST_STRING g_cszLicenseServerPubKey = CREATE_DRM_STRING (LicenseServerPubKey);/*L"LICENSESERVERPUBKEY");*/
const DRM_CONST_STRING g_cszSequenceNumber = CREATE_DRM_STRING (SequenceNumber);/*L"SEQUENCENUMBER");*/
const DRM_CONST_STRING g_cszKEYID = CREATE_DRM_STRING (KEYID);/*L"KID");*/
const DRM_CONST_STRING g_cszLID = CREATE_DRM_STRING (LID );/*L"LID");*/
const DRM_CONST_STRING g_cszPUBKEY = CREATE_DRM_STRING (WCPUBKEY);/*L"PUBKEY");*/
const DRM_CONST_STRING g_cszType = CREATE_DRM_STRING (Type);/*L"type");*/
const DRM_CONST_STRING g_cszMachine = CREATE_DRM_STRING (Machine);/*L"machine");*/
const DRM_CONST_STRING g_cszUPLINK = CREATE_DRM_STRING (UPLINK);/*L"UPLINK");*/
const DRM_CONST_STRING g_cszREVOCATION = CREATE_DRM_STRING (REVOCATION);/*L"REVOCATION");*/
const DRM_CONST_STRING g_cszINDEX = CREATE_DRM_STRING (INDEX);/*L"INDEX");*/
const DRM_CONST_STRING g_cszLIDNode2 = CREATE_DRM_STRING (LIDNode2);/* L"{10000555-0000-0010-8000-00AA006D2EB7}" );*/
const DRM_CONST_STRING g_cszSymVal = CREATE_DRM_STRING (SYMVALUE);/*L"SYMVALUE" );*/
const DRM_CONST_STRING g_cszSymSig = CREATE_DRM_STRING (SYMSIGNATURE);/*L"SYMSIGNATURE" );*/

/* drm client parses this tag but then ignores it */
const DRM_CONST_STRING g_cszIssuedDate = CREATE_DRM_STRING(IssuedDate);/* L"<ISSUEDATE>20000102 23:20:14Z</ISSUEDATE>" );*/
const DRM_CONST_STRING g_cszPriority = CREATE_DRM_STRING(Priority);/* L"<PRIORITY>15</PRIORITY>" );*/

const DRM_CONST_STRING g_cszRights = CREATE_DRM_STRING(Rights);

const DRM_CONST_STRING g_cszEnablingBitsAlgorithm = CREATE_DRM_STRING (EnablingBitsAlgorithm);/*L"<ALGORITHM type=\"MSDRM\"></ALGORITHM>");*/
const DRM_CONST_STRING g_cszChainedEnablingBitsAlgorithm = CREATE_DRM_STRING (ChainedEnablingBitsAlgorithm);/*L"<ALGORITHM type=\"MSDRM CK\"></ALGORITHM>");*/

/*
const DRM_CONST_STRING g_cszEnablingBitsData = CREATE_DRM_STRING(
    L"<ALGORITHM type=\"MSDRM\"></ALGORITHM>"
    L"<PUBKEY type=\"machine\">WEJKJKJK==</PUBKEY>"
    L"<VALUE>AAAABBBBCCCCDDDD</VALUE>"
    L"<SIGNATURE>anvjkhihs==</SIGNATURE>");
*/
const DRM_CONST_STRING g_cszContentRevocationCondition= CREATE_DRM_STRING (ContentRevocationCondition);/*L"<CONDITION><![CDATA[(!exists(license.CONTENTREVOCATION)?deletelicense():0) ; 0]]></CONDITION>");*/

const DRM_CONST_STRING g_cszHashAlgorithm = CREATE_DRM_STRING (HashAlgorithm);/*L"<HASHALGORITHM type=\"SHA\"></HASHALGORITHM>");*/
const DRM_CONST_STRING g_cszSignAlgorithm = CREATE_DRM_STRING (SignAlgorithm);/*L"<SIGNALGORITHM type=\"MSDRM\"></SIGNALGORITHM>");*/

/* invalid items */
const DRM_CONST_STRING g_cszHashAlgorithmUnknown = CREATE_DRM_STRING (HashAlgorithmUnknown);/*L"<HASHALGORITHM type=\"UNKNOWN\"></HASHALGORITHM>");*/
const DRM_CONST_STRING g_cszSignAlgorithmUnknown = CREATE_DRM_STRING (SignAlgorithmUnknown);/*L"<SIGNALGORITHM type=\"UNKNOWN\"></SIGNALGORITHM>");*/

/*PRIVKEY s_BrLicSrvPriv = {{
    0x86, 0x3C, 0x11, 0x60, 0x1B, 0xE2, 0x8C, 0xD2, 0x5F, 0x82,
    0x27, 0xAD, 0xCE, 0x18, 0x57, 0xD5, 0x41, 0x89, 0x39, 0x10
}};*/
DRM_CONST_STRING g_szBrLicSrvPriv = CREATE_DRM_STRING (BrLicSrvPriv);/*L"hjwRYBvijNJfgietzhhX1UGJORA=" );*/

/* globals */
enum {
    IDX_CONTENT_OWNER_PUB_KEY,
    IDX_CONTENT_OWNER_SIGNINGKEY,
    IDX_REVOKED_CONTENT_PUB_KEY,
    IDX_REVOKED_CONTENT_SIGNINGKEY,
    IDX_LICENSE_SERVER_PUB_KEY,
    IDX_LICENSE_SERVER_SIGNINGKEY,
    IDX_LICENSE_SERVER_CERT_ROOT,
    IDX_LICENSE_SERVER_CERT_LS,
    IDX_LICENSE_SERVER_CERT_LS0,
    IDX_LICENSE_SERVER_KEYSEED,
    IDX_LICENSE_KID,
    IDX_LICENSE_LID,
    IDX_LICENSE_UPLINK_KID,
    IDX_DEVICE_REVOCATION_LIST_INDEX
};
#define MAX_LICGEN_KEYS                     14

DRM_CONST_STRING g_keys[MAX_LICGEN_KEYS];
_XMBContext *g_pbXMLContext;
DRM_DWORD g_cbXMLContext;
DRM_BOOL g_fLicGenInited = FALSE;
DRM_BYTE *g_rgbCryptoContext = NULL;
DRM_CONST_STRING g_szContentKey = {0};

DRM_RESULT MakeDRMStringFromKeyName(DRM_CONST_STRING *pwszStr, const char *szWhat)
{
    DRM_RESULT dr    = DRM_SUCCESS;
    char      *szKey = GetTestCert(szWhat);

    ChkDR( MakeDRMString(pwszStr, szKey) );

ErrorExit:
    SAFE_OEM_FREE(szKey);
    return dr;
}


#define TESTCERTTXT "testcerts.txt"
#define MAXBUFLENGTH 300



char* GetTestCert(const char *szGetWhat)
{
    DRM_RESULT  dr        = DRM_SUCCESS;
    DRM_DWORD   cbGetWhat = 0;
    char       *buf       = NULL;
    char       *pOut      = NULL;
    FILE       *f         = NULL;
    char       *pszPath   = NULL;
    DRM_DWORD   cchPath   = 0;


    if ( !szGetWhat )
    {
        goto ErrorExit;
    }

    cbGetWhat = ( DRM_DWORD ) strlen( szGetWhat );

    ChkDR( TST_UTL_MakePathA( NULL, TESTCERTTXT, &pszPath, &cchPath, FALSE ) );
    f = fopen( pszPath, "rt" );
    if ( f == NULL )
    {
        goto ErrorExit;
    }

    ChkMem( buf = (char *)Oem_MemAlloc( MAXBUFLENGTH ) );

    while ( !feof( f ) )
    {
        if ( !fgets( buf, MAXBUFLENGTH, f ) )
        {
            goto ErrorExit;
        }
        if ( !strncmp( szGetWhat, buf, cbGetWhat ) )
        {
            DRM_DWORD cbKey = ( DRM_DWORD ) strlen( buf );
            while( cbKey > 0 && ( buf[cbKey - 1] == '\n' || buf[cbKey - 1] == '\r' ) )
            {
                buf[--cbKey] = 0; /* eat EOL characters */
            }

            ChkArg( cbKey > cbGetWhat );

            ChkMem( pOut = (char *)Oem_MemAlloc( cbKey - cbGetWhat ) );
            MEMCPY( pOut, buf + cbGetWhat + 1, cbKey - cbGetWhat );
            goto ErrorExit;
        }
    }

ErrorExit:
    SAFE_OEM_FREE( buf );
    SAFE_OEM_FREE( pszPath );
    if ( f != NULL )
    {
        fclose( f );
    }
    return pOut;
}



DRM_RESULT TestLicGenInit(DRM_DWORD dwLicMaxLength)
{
    DRM_RESULT dr = DRM_SUCCESS;

    ChkArg(!g_fLicGenInited); /* TestLicGenInit has been started already. */

    ChkMem(g_rgbCryptoContext = (DRM_BYTE*)Oem_MemAlloc(DRM_PKCRYPTO_CONTEXT_BUFFER_SIZE));
    ZEROMEM(g_rgbCryptoContext, DRM_PKCRYPTO_CONTEXT_BUFFER_SIZE);

    ZEROMEM(g_keys, SIZEOF(DRM_CONST_STRING) * MAX_LICGEN_KEYS);
    g_cbXMLContext = dwLicMaxLength;

#pragma prefast(push)
#pragma prefast(disable:22011) /* ignore arithmetic overflow warning, already handled */
    ChkOverflow( dwLicMaxLength * SIZEOF(DRM_WCHAR), dwLicMaxLength );
    ChkMem(g_pbXMLContext = (_XMBContext*)Oem_MemAlloc(dwLicMaxLength * SIZEOF(DRM_WCHAR)) );
#pragma prefast(pop)
    ZEROMEM( g_pbXMLContext, dwLicMaxLength * SIZEOF(DRM_WCHAR) );
    g_fLicGenInited = TRUE; /* TestLicGenShutDown can be called now. */

    /* Set default keys */

    ChkDR( MakeDRMStringFromKeyName(&(g_keys[IDX_CONTENT_OWNER_PUB_KEY]), "CONTENTPUBKEY") );
    ChkDR( MakeDRMStringFromKeyName(&(g_keys[IDX_CONTENT_OWNER_SIGNINGKEY]), "CONTENTPRVKEY") );
    ChkDR( MakeDRMString(&(g_keys[IDX_REVOKED_CONTENT_PUB_KEY]), "z4Jx7AKiY*tCKPGj8zJloMNuElScxMaQirsP4DBkNccc9eONPFbkSA==") );
    ChkDR( MakeDRMString(&(g_keys[IDX_REVOKED_CONTENT_SIGNINGKEY]), "0b5nYnU6*8TYXMygX40cCsNcZTo=") );
    ChkDR( MakeDRMStringFromKeyName(&(g_keys[IDX_LICENSE_SERVER_PUB_KEY]), "LSPUBKEY") );
    ChkDR( MakeDRMStringFromKeyName(&(g_keys[IDX_LICENSE_SERVER_SIGNINGKEY]), "LSPRVKEY") );
    ChkDR( MakeDRMStringFromKeyName(&(g_keys[IDX_LICENSE_SERVER_CERT_ROOT]), "CERT2") );
    ChkDR( MakeDRMStringFromKeyName(&(g_keys[IDX_LICENSE_SERVER_CERT_LS]), "CERT1") );
    ChkDR( MakeDRMString(&(g_keys[IDX_LICENSE_SERVER_KEYSEED]), szLicGenDefaultKeySeed) );
    ChkDR( MakeDRMString(&(g_keys[IDX_LICENSE_KID]), szLicGenDefaultKID) );
    ChkDR( MakeDRMString(&(g_keys[IDX_LICENSE_LID]), szLicGenDefaultLID) );
    ChkDR( MakeDRMString(&(g_keys[IDX_LICENSE_UPLINK_KID]), szLicGenDefaultUplinkKID) );
ErrorExit:
    return dr;
}

DRM_RESULT TestLicGenShutDown()
{
    int i;
    if (g_fLicGenInited) {
        for (i = 0; i < MAX_LICGEN_KEYS; i++)
        {
            FREEDRMSTRING( g_keys[i] );
        }
        SAFE_OEM_FREE(g_pbXMLContext);
        g_fLicGenInited = FALSE;
    }

    SAFE_OEM_FREE(g_rgbCryptoContext);
    g_rgbCryptoContext = NULL;

    return DRM_SUCCESS;
}

DRM_RESULT TestLicGenSetKeysByName(const char *setWhat, const char *szKeyName)
{
    DRM_RESULT dr;
    char *szKey = GetTestCert(szKeyName);
    dr = TestLicGenSetKeys(setWhat, szKey);
    SAFE_OEM_FREE(szKey);
    return dr;
}

DRM_RESULT TestLicGenSetKeys(const char *setWhat, const char *szKey)
{
    DRM_RESULT dr = DRM_SUCCESS;

    char *setWhatMap[] = {
        "CONTENT_OWNER_PUB_KEY",
        "CONTENT_OWNER_SIGNINGKEY",
        "REVOKED_CONTENT_PUB_KEY",
        "REVOKED_CONTENT_SIGNINGKEY",
        "LICENSE_SERVER_PUB_KEY",
        "LICENSE_SERVER_SIGNINGKEY",
        "LICENSE_SERVER_CERT_ROOT",
        "LICENSE_SERVER_CERT_LS",
        "LICENSE_SERVER_CERT_LS0",
        "LICENSE_SERVER_KEYSEED",
        "LICENSE_KID",
        "LICENSE_LID",
        "LICENSE_UPLINK_KID",
        "DEVICE_REVOCATION_LIST_INDEX"
    };

    int i = 0;


    ChkArg(setWhat && MAX_LICGEN_KEYS == SIZEOF(setWhatMap) / SIZEOF(setWhatMap[0]));
    while (i < MAX_LICGEN_KEYS && strcmp(setWhatMap[i], setWhat))
        i++;
    ChkArg(i < MAX_LICGEN_KEYS);

    FREEDRMSTRING( g_keys[i] );

    if( szKey != NULL
     && strcmp( szKey, "RANDOM" ) == 0 )
    {
        /*
        ** Generate a random LID or KID
        */

        DRM_DWORD cchRandomID = 0;

        cchRandomID = DRM_GUID_STRING_LEN + 1;
        g_keys[i].pwszString = (const DRM_WCHAR*)Oem_MemAlloc( cchRandomID * SIZEOF( DRM_WCHAR ) );
        g_keys[i].cchString = cchRandomID - 1;
        ZEROMEM( (DRM_BYTE*)g_keys[i].pwszString, cchRandomID * SIZEOF( DRM_WCHAR ) );

        if( strcmp( setWhat, "LICENSE_KID" ) == 0 )
        {
            ChkDR ( DRM_KG_GenerateKeyID( NULL, (DRM_WCHAR*)g_keys[i].pwszString, &cchRandomID ) );
        }
        else if( strcmp( setWhat, "LICENSE_LID" ) == 0 )
        {
            DRM_GUID  randomGUID = {0};

            ChkDR( Oem_Random_GetBytes( NULL, (DRM_BYTE*)&randomGUID, SIZEOF( randomGUID ) ) );
            ChkDR( DRM_UTL_GuidToString( &randomGUID, (DRM_WCHAR*)g_keys[i].pwszString ) );
        }
        else
        {
            ChkDR( DRM_E_TEST_INVALIDARG );
        }
    }
    else
    {
        ChkDR( MakeDRMString(&(g_keys[i]), szKey) );
    }

ErrorExit:
    return dr;
}


DRM_RESULT tDRM_B64_Decode(DRM_CONST_STRING *szEncoded, DRM_DWORD *pcbDecode, DRM_BYTE **ppbDecode)
{
    DRM_RESULT dr;

    ChkDRMString( szEncoded );

    /* szEncoded size may include the null terminating wchar which would break DRM_B64_DecodeW() */
    if ( szEncoded->pwszString[szEncoded->cchString - 1] == g_wchNull )
    {
        szEncoded->cchString--;
    }

    dr = DRM_B64_DecodeW( szEncoded, pcbDecode, NULL, 0 );
    if ( dr != DRM_E_BUFFERTOOSMALL )
    {
        ChkDR(dr);
    }

    SAFE_OEM_FREE( *ppbDecode );
    ChkMem( *ppbDecode = (DRM_BYTE *)Oem_MemAlloc( *pcbDecode ) );
    ChkDR( DRM_B64_DecodeW( szEncoded, pcbDecode, *ppbDecode, 0 ) );

ErrorExit:
    return dr;
}


DRM_RESULT tDRM_B64_Encode(const DRM_BYTE *pbData, DRM_DWORD cbData, DRM_CONST_STRING *pszEncode)
{
    DRM_RESULT dr        = DRM_SUCCESS;
    DRM_DWORD  cchString = 0;
    dr = DRM_B64_EncodeW(pbData, cbData, NULL, &cchString, 0);
    if (dr != DRM_E_BUFFERTOOSMALL)
    {
        ChkDR(dr);
    }
    FREEDRMSTRINGPOINTER( pszEncode );
    pszEncode->cchString = cchString;

/*
** Since ChkOverflow Mitigates the threat of am arithmetic overflow, turn off that warning for this block
*/
#pragma prefast(push)
#pragma prefast(disable:22011)
    ChkOverflow( pszEncode->cchString * SIZEOF(DRM_WCHAR), pszEncode->cchString );
    ChkMem(pszEncode->pwszString = (const DRM_WCHAR*)Oem_MemAlloc(pszEncode->cchString * SIZEOF(DRM_WCHAR)));
    ChkDR(DRM_B64_EncodeW(pbData, cbData, (DRM_WCHAR*)pszEncode->pwszString, &pszEncode->cchString, 0));
#pragma prefast(pop)
ErrorExit:
    return dr;
}

static DRM_RESULT tDRM_KG_GenerateContentKey(DRM_CONST_STRING *pszKeySeed, DRM_CONST_STRING *pszKid, DRM_CONST_STRING *pszKey)
{
    DRM_RESULT dr;
    DRM_DWORD  cchKID = pszKid->cchString;
    DRM_DWORD  cchKey = 0;
    DRM_BYTE  *pbScratchKeySeed = NULL;

#pragma prefast(push)
#pragma prefast(disable:22022) /* ignore arithmetic overflow warning, already handled */
    ChkOverflow( pszKeySeed->cchString * SIZEOF(DRM_WCHAR), pszKeySeed->cchString );
    ChkMem( pbScratchKeySeed = (DRM_BYTE *)Oem_MemAlloc( pszKeySeed->cchString * SIZEOF(DRM_WCHAR) ) );
    ZEROMEM( ( DRM_BYTE * )pbScratchKeySeed, pszKeySeed->cchString * SIZEOF( DRM_WCHAR ) );
#pragma prefast(pop)

    dr = DRM_KG_GenerateContentKeyB64(pszKeySeed->pwszString, pszKeySeed->cchString, pszKid->pwszString, cchKID, eDRM_RC4_CIPHER, DRMCIPHERKEYLEN_RC4, NULL, &cchKey, pbScratchKeySeed, pszKeySeed->cchString * SIZEOF(DRM_WCHAR));

    if (dr == DRM_E_INVALIDARG && cchKID >= DRM_KEYSEED_BASE64_LEN) {
        /*  DRM_KG_GenerateContentKey limits the KID length. The content key generated from this function
            will not match the one from WMRM for long KIDs. This is OK for our test.*/

        cchKID = DRM_KEYSEED_BASE64_LEN - 1;
        dr = DRM_KG_GenerateContentKeyB64(pszKeySeed->pwszString, pszKeySeed->cchString, pszKid->pwszString, cchKID, eDRM_RC4_CIPHER, DRMCIPHERKEYLEN_RC4, NULL, &cchKey, pbScratchKeySeed, pszKeySeed->cchString * SIZEOF(DRM_WCHAR));
    }

    if (dr != DRM_E_BUFFERTOOSMALL)
    {
        ChkDR(dr);
    }
    FREEDRMSTRINGPOINTER( pszKey );
    pszKey->cchString = cchKey;
/*
** Since ChkOverflow Mitigates the threat of am arithmetic overflow, turn off that warning for this block
*/
#pragma prefast(push)
#pragma prefast(disable:22011)
    ChkOverflow( pszKey->cchString * SIZEOF(DRM_WCHAR), pszKey->cchString );
    ChkMem(pszKey->pwszString = (DRM_WCHAR*)Oem_MemAlloc(pszKey->cchString * SIZEOF(DRM_WCHAR)));
    ChkDR(DRM_KG_GenerateContentKeyB64((DRM_WCHAR*)pszKeySeed->pwszString, pszKeySeed->cchString, (DRM_WCHAR*)pszKid->pwszString, cchKID, eDRM_RC4_CIPHER, DRMCIPHERKEYLEN_RC4, (DRM_WCHAR*)pszKey->pwszString, &pszKey->cchString, pbScratchKeySeed, pszKeySeed->cchString * SIZEOF(DRM_WCHAR)));
#pragma prefast(pop)
ErrorExit:
    SAFE_OEM_FREE( pbScratchKeySeed );
    return dr;
}

static DRM_RESULT _CalculateChecksum(
    const DRM_BYTE         *pbData,
          DRM_DWORD         cbData,
          DRM_CONST_STRING *pStr )
{
    DRM_RESULT dr=DRM_SUCCESS;
    DRM_SHA_CONTEXT  shaVal = DRM_SHA_CONTEXT_DEFAULT;
    DRM_BYTE res[DRM_SHA1_DIGEST_LEN+1];
    int i;

    ChkArg(cbData <= DRM_SHA1_DIGEST_LEN);
    memset(res, 0, DRM_SHA1_DIGEST_LEN+1);
    memcpy(res, pbData, cbData);
    for (i = 0; i < SHA_ITERATIONS; i++) {
        ChkDR( DRM_SHA_Init(&shaVal, eDRM_SHA_1) );
        ChkDR( DRM_SHA_Update(res, DRM_SHA1_DIGEST_LEN+1, &shaVal) );
        ChkDR( DRM_SHA_Finalize(&shaVal, DRM_SHA1_DIGEST_LEN, res ) );
    }
    ChkDR(tDRM_B64_Encode(res, CHECKSUM_LENGTH, pStr));
ErrorExit:
    return dr;
}

static DRM_RESULT _GetPlainContentKey(
    __in                                      ELicenseType f_eLicenseType,
    __out_bcount( DRM_ECC160_PLAINTEXT_LEN )  DRM_BYTE     f_pbPlain[__CB_DECL( DRM_ECC160_PLAINTEXT_LEN )],
    __out                                     DRM_DWORD   *f_pcbPlain )
{
    DRM_RESULT  dr           = DRM_SUCCESS;
    DRM_BYTE   *pbContentKey = NULL;

    ChkArg( f_pcbPlain != NULL );

    /* Decode the content key */
    ChkDR(tDRM_B64_Decode(&g_szContentKey, f_pcbPlain, &pbContentKey));

    /* Put the content key to the plain text buffer. For root license, we use the 7-byte content key with a padding 0 as the DES key. */
    MEMSET(f_pbPlain, 0, DRM_ECC160_PLAINTEXT_LEN);
    DRM_BYT_CopyBytes(f_pbPlain, 1,pbContentKey,0 , min( *f_pcbPlain, DRM_ECC160_PLAINTEXT_LEN - 1 ) );

    if ( f_eLicenseType == MAKE_ROOT_LICENSE )
    {
        *f_pcbPlain = DES_KEYSIZE;
    }

    f_pbPlain[0] = (DRM_BYTE)*f_pcbPlain;

    /*
    ** Increment the size of the plain key to account for adding the length as the first byte.
    */
    (*f_pcbPlain)++;

ErrorExit:
    SAFE_OEM_FREE( pbContentKey );
    return dr;
}

static DRM_RESULT AddEnablingBits(DRM_DWORD dwOption, ELicenseType eLicenseType, DRM_CONST_STRING *pszSigningPrvKey)
{
    DRM_RESULT dr;
    DRM_CONST_STRING szStr = {0};
    DRM_BYTE *pbContentKey = NULL, *pbUplinkKey = NULL, pbDesKey[__CB_DECL( DES_KEYSIZE )],
              pbPlain[__CB_DECL( DRM_ECC160_PLAINTEXT_LEN )],
              pbEncrypted[__CB_DECL( DRM_ECC160_CIPHERTEXT_LEN )],
              pbSign[__CB_DECL( DRM_ECC160_SIGNATURE_LEN )];
    DRM_DWORD cbContentKey, cbUplinkKey, cbPvkSigning,
              cbEncrypted = DRM_ECC160_CIPHERTEXT_LEN,
              cbSignature = DRM_ECC160_SIGNATURE_LEN;
    PRIVKEY *pPvkSigning = NULL;

    /* Open the ENABLINGBITS tag and add the algorithm block */
    ChkDR(DRM_XMB_OpenNode(g_pbXMLContext, (eLicenseType == MAKE_LEAF_LICENSE)? &g_cszChainedEnablingBits: &g_cszEnablingBits));
    ChkDR(DRM_XMB_AppendNode(g_pbXMLContext, (eLicenseType == MAKE_LEAF_LICENSE)? &g_cszChainedEnablingBitsAlgorithm: &g_cszEnablingBitsAlgorithm));

    if (eLicenseType == MAKE_LEAF_LICENSE) {
        DESTable des;

        /* Add UPLINK block */
        ChkDR(DRM_XMB_OpenNode(g_pbXMLContext, &g_cszUPLINK));
        ChkDR(DRM_XMB_AppendNode(g_pbXMLContext, &g_keys[IDX_LICENSE_UPLINK_KID]));
        ChkDR(DRM_XMB_CloseCurrNode(g_pbXMLContext, NULL));

        /* generate the uplink key with uplink keyid and keyseed */
        ChkDR(tDRM_KG_GenerateContentKey(&g_keys[IDX_LICENSE_SERVER_KEYSEED], &g_keys[IDX_LICENSE_UPLINK_KID], &szStr));
        ChkDR(tDRM_B64_Decode(&szStr, &cbUplinkKey, &pbUplinkKey)); /* This is a 7 byte key */

        /* The uplink key is an 8-byte DES key. We pad a zero byte to the 7-byte content key to the DES key */
        MEMSET(pbDesKey, 0, DES_KEYSIZE);
        MEMCPY(pbDesKey, pbUplinkKey, min(DES_KEYSIZE, cbUplinkKey));

        /* Add the CHAINCHECKSUM block */
        ChkDR(_CalculateChecksum(pbDesKey, DES_KEYSIZE, &szStr));
        ChkDR(DRM_XMB_OpenNode(g_pbXMLContext, &g_cszChainedCheckSum));
        ChkDR(DRM_XMB_AppendNode(g_pbXMLContext, &szStr));
        ChkDR(DRM_XMB_CloseCurrNode(g_pbXMLContext, NULL));

        /* calculate the VALUE string */
        ChkDR(tDRM_KG_GenerateContentKey(&g_keys[IDX_LICENSE_SERVER_KEYSEED], &g_keys[IDX_LICENSE_KID], &g_szContentKey));
        ChkDR(tDRM_B64_Decode(&g_szContentKey, &cbContentKey, &pbContentKey));

        /* Use the DES key to encrypt the pbContentKey */
        /* ChkArg(DRM_ECC160_PLAINTEXT_LEN >= DES_BLOCKLEN + DES_BLOCKLEN); */
        pbPlain[0] = (DRM_BYTE)cbContentKey;
        MEMCPY(pbPlain + 1, pbContentKey, min( cbContentKey, DRM_ECC160_PLAINTEXT_LEN - 1 ) );

        DRM_DES_KeySetup(&des, pbDesKey);
        DRM_DES_Encrypt(pbPlain, pbEncrypted, &des);
        DRM_DES_Encrypt(pbPlain + DES_BLOCKLEN, pbEncrypted + DES_BLOCKLEN, &des);
        ChkDR(tDRM_B64_Encode(pbEncrypted, DES_BLOCKLEN + DES_BLOCKLEN, &szStr));

    } else { /* Add PUBKEY and VALUE for root and simple license */

        /* Add the PUBKEY block for root and simple license*/
        ChkDR(tDRM_B64_Encode((DRM_BYTE*)&(g_ClientID.pk.pk.pk), SIZEOF(PUBKEY), &szStr));
        ChkDR(DRM_XMB_OpenNode(g_pbXMLContext, &g_cszPUBKEY));
        ChkDR(DRM_XMB_AddAttribute(g_pbXMLContext, &g_cszType, &g_cszMachine));
        ChkDR(DRM_XMB_AppendNode(g_pbXMLContext, &szStr));
        ChkDR(DRM_XMB_CloseCurrNode(g_pbXMLContext, NULL));

        /* generate the content key with keyid and keyseed */
        ChkDR(tDRM_KG_GenerateContentKey(&g_keys[IDX_LICENSE_SERVER_KEYSEED], &g_keys[IDX_LICENSE_KID], &g_szContentKey));

        /*
        ** Get the plain content key
        */
        ChkDR( _GetPlainContentKey( eLicenseType, pbPlain, &cbContentKey ) );

        /* Encrypt the plain text buffer using the client's public key and then encode it
            back to szStr. This is the string to put into ENABLINGBITS/VALUE
        */
        ChkDR( DRM_PK_Encrypt( NULL,
                               g_rgbCryptoContext,
                               eDRM_ECC_P160,
                               DRM_ECC160_PUBLIC_KEY_LEN,
                               ( DRM_BYTE * )&(g_ClientID.pk.pk.pk),
                               DRM_ECC160_PLAINTEXT_LEN,
                               pbPlain,
                               &cbEncrypted,
                               pbEncrypted ) );

        ChkDR(tDRM_B64_Encode(pbEncrypted, DRM_ECC160_CIPHERTEXT_LEN, &szStr));
    }

    /* Add the VALUE block */
    ChkDR(DRM_XMB_OpenNode(g_pbXMLContext, &g_cszValue));
    ChkDR(DRM_XMB_AppendNode(g_pbXMLContext, &szStr));
    ChkDR(DRM_XMB_CloseCurrNode(g_pbXMLContext, NULL));

    /* Sign the content key, then encode and put it as SIGNATURE in ENABLINGBITS/SIGNATURE  */
    ChkDR(tDRM_B64_Decode(pszSigningPrvKey, &cbPvkSigning, (DRM_BYTE**)&pPvkSigning));
    ChkArg(cbPvkSigning == DRM_ECC160_PRIVATE_KEY_LEN);
    ChkDR(DRM_PK_Sign(g_rgbCryptoContext,
                      eDRM_ECC_P160,
                      DRM_ECC160_PRIVATE_KEY_LEN,
                      ( const DRM_BYTE * )pPvkSigning,
                      (DRM_DWORD)pbPlain[0],
                      pbPlain + 1,
                      &cbSignature,
                      pbSign));

    ChkDR(tDRM_B64_Encode(pbSign, DRM_ECC160_SIGNATURE_LEN, &szStr));

    ChkDR(DRM_XMB_OpenNode(g_pbXMLContext, &g_cszSignature));
    ChkDR(DRM_XMB_AppendNode(g_pbXMLContext, &szStr));
    ChkDR(DRM_XMB_CloseCurrNode(g_pbXMLContext, NULL));

    ChkDR(DRM_XMB_CloseCurrNode(g_pbXMLContext, NULL));
ErrorExit:
    FREEDRMSTRING( szStr );
    SAFE_OEM_FREE(pbContentKey);
    SAFE_OEM_FREE(pbUplinkKey);
    SAFE_OEM_FREE(pPvkSigning);
    return dr;
}

static DRM_RESULT AddSignature(
    const DRM_STRING       *szData,
    const DRM_CONST_STRING *pszSigningPrivateKey,
          DRM_DWORD         dwOption )
{
    DRM_RESULT dr = DRM_SUCCESS;
    DRM_CONST_STRING szSignature;
    DRM_BYTE  pbSignature[DRM_ECC160_SIGNATURE_LEN] = { 0 };
    DRM_DWORD cbSignature = DRM_ECC160_SIGNATURE_LEN;
    PRIVKEY   pvk;
    DRM_DWORD cbPvk = DRM_ECC160_PRIVATE_KEY_LEN;

    szSignature.pwszString = NULL;
    dwOption &= OP_LICGEN_SIGNATURE_TEST_OPTION_MASK;

    if (dwOption == OP_LICGEN_MISSING_SIGNATURE) /* don't do anything */
        goto ErrorExit;

    /* Write the signature tag */
    ChkDR(DRM_XMB_OpenNode(g_pbXMLContext, &g_cszSignature));

    if (dwOption != OP_LICGEN_EMPTY_SIGNATURE) {
        if (dwOption != OP_LICGEN_MISSING_HASH_ALGORITHM) {
            ChkDR(DRM_XMB_AppendNode(g_pbXMLContext,
                (dwOption == OP_LICGEN_UNKNOWN_HASH_ALGORITHM)? &g_cszHashAlgorithmUnknown: &g_cszHashAlgorithm));
        }

        if (dwOption != OP_LICGEN_MISSING_SIGN_ALGORITHM) {
            ChkDR(DRM_XMB_AppendNode(g_pbXMLContext,
                (dwOption == OP_LICGEN_UNKNOWN_SIGN_ALGORITHM)? &g_cszSignAlgorithmUnknown: &g_cszSignAlgorithm));
        }

        if (dwOption != OP_LICGEN_MISSING_SIGNATURE_VALUE) {
            ChkDR(DRM_XMB_OpenNode(g_pbXMLContext, &g_cszValue));

            if (dwOption != OP_LICGEN_EMPTY_SIGNATURE_VALUE) {
                /* Sign the data block and get the signature in binary. */
                ChkDR(DRM_B64_DecodeW(pszSigningPrivateKey, &cbPvk, pvk.x, 0));

                /* simulate data tampering by excluding the last char from the data */
                ChkDR(DRM_PK_Sign(g_rgbCryptoContext,
                                  eDRM_ECC_P160,
                                  DRM_ECC160_PRIVATE_KEY_LEN,
                                  ( const DRM_BYTE * )&pvk,
                                  (szData->cchString - ((dwOption == OP_LICGEN_TAMPER_DATA)? 1: 0)) * SIZEOF(DRM_WCHAR),
                                  (const DRM_BYTE*)szData->pwszString,
                                  &cbSignature,
                                  pbSignature));

                /* Encode the signature to base64 string */
                DRM_B64_EncodeW(pbSignature, cbSignature, NULL, &(szSignature.cchString), 0);
/*
** Since ChkOverflow Mitigates the threat of am arithmetic overflow, turn off that warning for this block
*/
#pragma prefast(push)
#pragma prefast(disable:22011)
                ChkOverflow( szSignature.cchString * SIZEOF(DRM_WCHAR), szSignature.cchString );
                ChkMem(szSignature.pwszString = (DRM_WCHAR*)Oem_MemAlloc(szSignature.cchString * SIZEOF(DRM_WCHAR)));
#pragma prefast(pop)
                ChkDR(DRM_B64_EncodeW(pbSignature, cbSignature, (DRM_WCHAR*)szSignature.pwszString, &(szSignature.cchString), 0));

                ChkDR(DRM_XMB_AppendNode(g_pbXMLContext, &szSignature));
            }
            ChkDR(DRM_XMB_CloseCurrNode(g_pbXMLContext, NULL)); /* close the VALUE tag */
        }
    }

    ChkDR(DRM_XMB_CloseCurrNode(g_pbXMLContext, NULL));

ErrorExit:
    FREEDRMSTRING( szSignature );
    return dr;
}

static DRM_RESULT AddSymSignature(
    const DRM_STRING      *szData,
          DRM_ANSI_STRING *pdastrSlk )
{
    DRM_RESULT dr = DRM_SUCCESS;
    DRM_BYTE rgbSymmSig[ __CB_DECL( DRM_SHA1_DIGEST_LEN ) ] = { 0 };
    DRM_CONST_STRING szSymSig = EMPTY_DRM_STRING;

    ChkDR( DRM_HMAC_CreateMAC( eDRM_SHA_1,
                                (DRM_BYTE*)pdastrSlk->pszString,
                                DES_KEYSIZE,
                                (const DRM_BYTE*)szData->pwszString,
                                0,
                                (szData->cchString) * SIZEOF(DRM_WCHAR),
                                rgbSymmSig,
                                0,
                                DRM_SHA1_DIGEST_LEN ) );

    ChkDR(tDRM_B64_Encode(rgbSymmSig, DRM_SHA1_DIGEST_LEN, &szSymSig));

    ChkDR(DRM_XMB_OpenNode(g_pbXMLContext, &g_cszSymSig));

    ChkDR(DRM_XMB_AppendNode(g_pbXMLContext, &szSymSig));

    ChkDR(DRM_XMB_CloseCurrNode(g_pbXMLContext, NULL));

ErrorExit:
    FREEDRMSTRING( szSymSig );
    return dr;
}

static DRM_RESULT AddSymValue(DRM_ANSI_STRING *pdastrSlk, ELicenseType eLicenseType)
{
    DRM_RESULT        dr           = DRM_SUCCESS;
    DRM_DWORD         cbContentKey = DRM_ECC160_CIPHERTEXT_LEN;
    DRM_CONST_STRING  szSymValue   = EMPTY_DRM_STRING;
    DRM_BYTE          pbPlain[__CB_DECL( DRM_ECC160_PLAINTEXT_LEN )];

    /*
    ** Get the plain content key
    */
    ChkDR( _GetPlainContentKey( eLicenseType, pbPlain, &cbContentKey ) );

    ChkDR( DRM_BBX_Legacy_SymmetricCipher( SYMMETRIC_ENCRYPT,
                                           (DRM_BYTE*)pdastrSlk->pszString,
                                           DES_KEYSIZE,
                                           pbPlain,
                                           NULL,
                                           cbContentKey ) );

    ChkDR(tDRM_B64_Encode(pbPlain, cbContentKey, &szSymValue));

    ChkDR(DRM_XMB_OpenNode(g_pbXMLContext, &g_cszSymVal));
    ChkDR(DRM_XMB_AppendNode(g_pbXMLContext, &szSymValue));
    ChkDR(DRM_XMB_CloseCurrNode(g_pbXMLContext, NULL));

ErrorExit:
    FREEDRMSTRING( szSymValue );
    return dr;
}

static DRM_RESULT AddCertChain()
{
    DRM_RESULT dr = DRM_SUCCESS;
    int i;

    ChkDR(DRM_XMB_OpenNode(g_pbXMLContext, &g_cszCertChain));
    for (i = IDX_LICENSE_SERVER_CERT_ROOT; i <= IDX_LICENSE_SERVER_CERT_LS0 && g_keys[i].pwszString; i++) {
        ChkDR(DRM_XMB_OpenNode(g_pbXMLContext, &g_cszCertificate));
        ChkDR(DRM_XMB_AppendNode(g_pbXMLContext, &(g_keys[i])));
        ChkDR(DRM_XMB_CloseCurrNode(g_pbXMLContext, NULL));
    }
    ChkDR(DRM_XMB_CloseCurrNode(g_pbXMLContext, NULL));
ErrorExit:
    return dr;
}

static DRM_RESULT AddRestore(DRM_DWORD dwOption, ELicenseType eLicenseType)
{
    DRM_RESULT dr;
    DRM_STRING szData;

    ChkDR(DRM_XMB_OpenNode(g_pbXMLContext, &g_cszDRMRestoreInfo));

    if (!(dwOption & OP_LICGEN_EMPTY_RESTOREINFO)) {
        ChkDR(DRM_XMB_OpenNode(g_pbXMLContext, &g_cszData)); /* Open the Data block */

        if (!(dwOption & OP_LICGEN_NO_RESTORE_LID)) {
            ChkDR(DRM_XMB_OpenNode(g_pbXMLContext, &g_cszLID));
            ChkDR(DRM_XMB_AppendNode(g_pbXMLContext,
                (dwOption & OP_LICGEN_TAMPER_RESTORE_LID)? &g_cszLIDNode2:
                &g_keys[IDX_LICENSE_LID]));
            ChkDR(DRM_XMB_CloseCurrNode(g_pbXMLContext, NULL));
        }

        ChkDR(AddEnablingBits(dwOption, eLicenseType, &g_szBrLicSrvPriv));
        ChkDR(DRM_XMB_CloseCurrNode(g_pbXMLContext, &szData)); /* Close Data block */

        ChkDR(AddSignature(&szData, &g_szBrLicSrvPriv,
            (dwOption & OP_LICGEN_TEST_RESTORE_SIGNATURE)? dwOption: 0));
    }

    ChkDR(DRM_XMB_CloseCurrNode(g_pbXMLContext, NULL));
ErrorExit:
    return dr;
}

static DRM_RESULT AddContentRevocations(DRM_DWORD dwOption, DRM_DWORD dwSequenceNumber)
{
    DRM_RESULT dr = DRM_SUCCESS;
    DRM_STRING szData = EMPTY_DRM_STRING;
    DRM_CONST_STRING szSequenceNumber = EMPTY_DRM_STRING;
    DRM_WCHAR wSequenceNumber[DRM_MAX_CCH_BASE10_DWORD_STRING+1] = {0};

    if (!(g_keys[IDX_REVOKED_CONTENT_PUB_KEY].pwszString))
        goto ErrorExit;

        ChkDR(DRM_XMB_OpenNode(g_pbXMLContext, &g_cszContentRevocation));

    /* build the DATA block in the revocation string */
    ChkDR(DRM_XMB_OpenNode(g_pbXMLContext, &g_cszData));

    /* Add sequence number */
    ChkDR(DRM_XMB_OpenNode(g_pbXMLContext, &g_cszSequenceNumber));

    ChkDR( DRM_UTL_NumberToString(dwSequenceNumber, wSequenceNumber, NO_OF( wSequenceNumber ), 0, 10, &szSequenceNumber.cchString ) );

    /*
    ** Puts terminating zero. There should be place for it.
    */

    DRMASSERT( szSequenceNumber.cchString < NO_OF( wSequenceNumber ) );
    if ( szSequenceNumber.cchString < NO_OF( wSequenceNumber ) )
    {
        wSequenceNumber[ szSequenceNumber.cchString ] = ONE_WCHAR( '\0','\0' );
    }
    szSequenceNumber.pwszString = wSequenceNumber;

    ChkDR(DRM_XMB_AppendNode(g_pbXMLContext, &szSequenceNumber));
    ChkDR(DRM_XMB_CloseCurrNode(g_pbXMLContext, NULL));

    /* Add Content Public key */
    if (!(dwOption & OP_LICGEN_CONTENT_REVOCATION_NO_CONTENTPUBKEY)) {
        ChkDR(DRM_XMB_OpenNode(g_pbXMLContext, &g_cszContentPubKey));
        ChkDR(DRM_XMB_AppendNode(g_pbXMLContext, &(g_keys[IDX_REVOKED_CONTENT_PUB_KEY])));
        ChkDR(DRM_XMB_CloseCurrNode(g_pbXMLContext, NULL));
    }

    /* Add license server public key */
    if (g_keys[IDX_LICENSE_SERVER_PUB_KEY].pwszString) {
        ChkDR(DRM_XMB_OpenNode(g_pbXMLContext, &g_cszLicenseServerPubKey));
        ChkDR(DRM_XMB_AppendNode(g_pbXMLContext, &(g_keys[IDX_LICENSE_SERVER_PUB_KEY])));
        ChkDR(DRM_XMB_CloseCurrNode(g_pbXMLContext, NULL));
    }

    /* Add content revocation condition */
    ChkDR(DRM_XMB_AppendNode(g_pbXMLContext, &g_cszContentRevocationCondition));
    ChkDR(DRM_XMB_CloseCurrNode(g_pbXMLContext, &szData)); /* Data */

    ChkDR(AddSignature(&szData, &(g_keys[IDX_REVOKED_CONTENT_SIGNINGKEY]),
        (dwOption & OP_LICGEN_TEST_CONTENT_REVOCATION_SIGNATURE)? dwOption: 0));
    ChkDR(DRM_XMB_CloseCurrNode(g_pbXMLContext, NULL));
ErrorExit:
    return dr;
}

static DRM_RESULT AddRevocationData(
    const DRM_WCHAR        *wszType,
    const DRM_CONST_STRING *pdstrValue )
{
    DRM_RESULT dr;
    DRM_CONST_STRING dstrType;

    ChkArg(wszType);
    dstrType.pwszString = wszType;
    dstrType.cchString = ( DRM_DWORD ) DRMCRT_wcslen(wszType);

    ChkDR(DRM_XMB_OpenNode(g_pbXMLContext, &g_cszREVOCATION));
    ChkDR(DRM_XMB_AddAttribute(g_pbXMLContext, &g_cszType, &dstrType));
    ChkDR(DRM_XMB_OpenNode(g_pbXMLContext, &g_cszINDEX));
    ChkDR(DRM_XMB_AddData(g_pbXMLContext, pdstrValue));
    ChkDR(DRM_XMB_CloseCurrNode(g_pbXMLContext, NULL));
    ChkDR(DRM_XMB_CloseCurrNode(g_pbXMLContext, NULL));
ErrorExit:
    return dr;
}

static DRM_RESULT AddDataSection(DRM_DWORD dwOption, const char *szExtraAttributes, const char *szExtraAttributesFile, ELicenseType eLicenseType, DRM_STRING *pszXMLFragment)
{
    DRM_RESULT dr;
    DRM_DWORD i, cContentRevocations = 3;
    DRM_CONST_STRING wszExtraAttributes;
    DRM_CONST_STRING wszExtraAttributesFile;
    DRM_BYTE *pbData=NULL;
    DRM_DWORD dwData=0;

    wszExtraAttributesFile.pwszString=NULL;
    wszExtraAttributes.pwszString = NULL;

    ChkDR(DRM_XMB_OpenNode(g_pbXMLContext, &g_cszData));

    if (g_keys[IDX_LICENSE_LID].pwszString) {
        ChkDR(DRM_XMB_OpenNode(g_pbXMLContext, &g_cszLID));
        ChkDR(DRM_XMB_AppendNode(g_pbXMLContext, &(g_keys[IDX_LICENSE_LID])));
        ChkDR(DRM_XMB_CloseCurrNode(g_pbXMLContext, NULL));
    }

    if (g_keys[IDX_LICENSE_KID].pwszString) {
        ChkDR(DRM_XMB_OpenNode(g_pbXMLContext, &g_cszKEYID));
        ChkDR(DRM_XMB_AppendNode(g_pbXMLContext, &(g_keys[IDX_LICENSE_KID])));
        ChkDR(DRM_XMB_CloseCurrNode(g_pbXMLContext, NULL));
    }

    ChkDR(DRM_XMB_AppendNode(g_pbXMLContext, &g_cszIssuedDate));

    /* Add content pub key */
    if (g_keys[IDX_CONTENT_OWNER_PUB_KEY].pwszString) {
        ChkDR(DRM_XMB_OpenNode(g_pbXMLContext, &g_cszContentPubKey));
        ChkDR(DRM_XMB_AppendNode(g_pbXMLContext, &g_keys[IDX_CONTENT_OWNER_PUB_KEY]));
        ChkDR(DRM_XMB_CloseCurrNode(g_pbXMLContext, NULL));
    }

    /* Add license priority */
    if (!(dwOption & OP_LICGEN_NO_LICENSE_PRIORITY)) {
        ChkDR(DRM_XMB_AppendNode(g_pbXMLContext, &g_cszPriority));
    }

    /* Add user defined xml string */
    if (szExtraAttributes) {
        ChkDR( MakeDRMString(&wszExtraAttributes, szExtraAttributes) );
        ChkDR(DRM_XMB_AppendNode(g_pbXMLContext, &wszExtraAttributes)); /* My custom attribute */
    }

    /*if there is lengthy extra attribute in the file, e.g. derived license*/
    if(szExtraAttributesFile){
        /*get derived license from a file. All files are at a Default extrafiles folder*/
        ChkArg(LoadTestFile(NULL, szExtraAttributesFile, &pbData, &dwData));
        ChkDR( MakeDRMString(&wszExtraAttributesFile, (char*)pbData) );
        ChkDR(DRM_XMB_AppendNode(g_pbXMLContext, &wszExtraAttributesFile));
    }

    /* Add default rights if the user does not want to excluding it */
    if (!(dwOption & OP_LICGEN_NO_DEFAULT_RIGHTS)) {


        ChkDR(DRM_XMB_AppendNode(g_pbXMLContext, &g_cszRights));
    }

    ChkDR(AddEnablingBits(dwOption, eLicenseType, &g_keys[IDX_LICENSE_SERVER_SIGNINGKEY]));

    if (!(dwOption & OP_LICGEN_NO_CONTENT_REVOCATION)) {
        if (!(dwOption & OP_LICGEN_CONTENT_REVOCATION_MULTIPLE))
            cContentRevocations = 1;
        for (i = 1; i <= cContentRevocations; i++) {
            ChkDR(AddContentRevocations(dwOption, i * 100));
        }
    }

    if (g_keys[IDX_DEVICE_REVOCATION_LIST_INDEX].pwszString) { // Add DCEL index
        ChkDR(AddRevocationData(DevExclusionList, &g_keys[IDX_DEVICE_REVOCATION_LIST_INDEX]));
    }
    ChkDR(DRM_XMB_CloseCurrNode(g_pbXMLContext, pszXMLFragment));
ErrorExit:
    FREEDRMSTRING( wszExtraAttributes );
    FREEDRMSTRING( wszExtraAttributesFile );
    SAFE_OEM_FREE(pbData);
    return dr;
}

DRM_RESULT TestLicGenMakeLicense(DRM_DWORD dwOption,
                                 const char *szExtraAttributes,
                                 const char *szExtraAttributesFile,
                                 ELicenseType eLicenseType,
                                 DRM_CONST_STRING *pszLic,
                                 DRM_ANSI_STRING *pdastrSlk,
                                 DRM_BOOL fNoSymVal)
{
    DRM_RESULT dr;
    DRM_STRING szData;

    ChkArg(pszLic);

    /* Create the root node */
    ChkDR(DRM_XMB_CreateDocument(g_cbXMLContext, g_pbXMLContext, &g_cszLicense));
    ChkDR(DRM_XMB_AddAttribute(g_pbXMLContext, &g_cszVersionAttr, &g_cszVersionValue));

    /* Add the <LICENSORINFO> node */
    ChkDR(DRM_XMB_OpenNode(g_pbXMLContext, &g_cszLicensorInfo));
    ChkDR(AddDataSection(dwOption, szExtraAttributes, szExtraAttributesFile, eLicenseType, &szData));

    if( ( ( dwOption & OP_LICGEN_SYM_OPTIMIZED ) != 0 ) && ( pdastrSlk != NULL ) )
    {
        AddSymSignature(&szData, pdastrSlk);
        /* Ideally, SymValue Node is not used if the license is a leaf and only Simple & Root Licenses need it.
        ** HOWEVER, there is a known issue in the PC code that symmetrically optimizes leaf licenses with the SLK
        ** even though the leaf license is already symmetrically encrypted with the root content key.
        ** The following code will not add the SymValue to the license if specified by a test case.
        */
        if( fNoSymVal == FALSE )
        {
            AddSymValue(pdastrSlk, eLicenseType);
        }
    }

    /* if the signature option is for content revocation block or the restore block then mask off it. */
    ChkDR(AddSignature(&szData, &(g_keys[IDX_LICENSE_SERVER_SIGNINGKEY]),
        (dwOption & (OP_LICGEN_TEST_CONTENT_REVOCATION_SIGNATURE | OP_LICGEN_TEST_RESTORE_SIGNATURE))?
        (dwOption & ~OP_LICGEN_SIGNATURE_TEST_OPTION_MASK) : dwOption));

    ChkDR(AddCertChain());
    ChkDR(DRM_XMB_CloseCurrNode(g_pbXMLContext, NULL));

    /* Optionally add the <DRMRESTOREINFO> node */
    if (dwOption & OP_LICGEN_RESTORE) {
        ChkDR(AddRestore(dwOption, eLicenseType));
    }

    ChkDR(DRM_XMB_CloseDocument(g_pbXMLContext, &szData));

    /* Prefix the license string with the XML header */
    MEMMOVE((DRM_BYTE*)(szData.pwszString + g_cszXMLHeader.cchString), (DRM_BYTE*)szData.pwszString, SIZEOF(DRM_WCHAR) * (szData.cchString + 1));
    MEMCPY((DRM_BYTE*)szData.pwszString, (DRM_BYTE*)g_cszXMLHeader.pwszString, SIZEOF(DRM_WCHAR) * g_cszXMLHeader.cchString);

    pszLic->pwszString = (const DRM_WCHAR*)szData.pwszString;
    pszLic->cchString = szData.cchString + g_cszXMLHeader.cchString;
ErrorExit:

    FREEDRMSTRING( g_szContentKey );
    return dr;
}

EXIT_PKTEST_NAMESPACE_CODE;

PREFAST_POP /* ignore prefast warning about ANSI functions for this entire file  */


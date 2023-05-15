/**@@@+++@@@@******************************************************************
**
** Microsoft (r) PlayReady (r)
** Copyright (c) Microsoft Corporation. All rights reserved.
**
***@@@---@@@@******************************************************************
*/
#include <drmcommon.h>
#include <drmtoolsconstants.h>

ENTER_PK_NAMESPACE_CODE;

const DRM_WCHAR g_rgwchRootTag                     [] = { ONE_WCHAR('P', '\0'), ONE_WCHAR('A', '\0'), ONE_WCHAR('C', '\0'), ONE_WCHAR('K', '\0'), ONE_WCHAR('A', '\0'), ONE_WCHAR('G', '\0'), ONE_WCHAR('E', '\0'), ONE_WCHAR('P', '\0'), ONE_WCHAR('A', '\0'), ONE_WCHAR('R', '\0'), ONE_WCHAR('A', '\0'), ONE_WCHAR('M', '\0'), ONE_WCHAR('S', '\0'), ONE_WCHAR('\0', '\0')}; /* "PACKAGEPARAMS" */
const DRM_WCHAR g_rgwchKeySeedTag                  [] = { ONE_WCHAR('K', '\0'), ONE_WCHAR('E', '\0'), ONE_WCHAR('Y', '\0'), ONE_WCHAR('S', '\0'), ONE_WCHAR('E', '\0'), ONE_WCHAR('E', '\0'), ONE_WCHAR('D', '\0'), ONE_WCHAR('\0', '\0')}; /* "KEYSEED" */
const DRM_WCHAR g_rgwchSigningKey                  [] = { ONE_WCHAR('S', '\0'), ONE_WCHAR('I', '\0'), ONE_WCHAR('G', '\0'), ONE_WCHAR('N', '\0'), ONE_WCHAR('I', '\0'), ONE_WCHAR('N', '\0'), ONE_WCHAR('G', '\0'), ONE_WCHAR('K', '\0'), ONE_WCHAR('E', '\0'), ONE_WCHAR('Y', '\0'), ONE_WCHAR('\0', '\0')}; /* "SIGNINGKEY" */
const DRM_WCHAR g_rgwchLaInfoTag                   [] = { ONE_WCHAR('L', '\0'), ONE_WCHAR('A', '\0'), ONE_WCHAR('I', '\0'), ONE_WCHAR('N', '\0'), ONE_WCHAR('F', '\0'), ONE_WCHAR('O', '\0'), ONE_WCHAR('\0', '\0')}; /* "LAINFO" */
const DRM_WCHAR g_rgwchCustomData                  [] = { ONE_WCHAR('C', '\0'), ONE_WCHAR('U', '\0'), ONE_WCHAR('S', '\0'), ONE_WCHAR('T', '\0'), ONE_WCHAR('O', '\0'), ONE_WCHAR('M', '\0'), ONE_WCHAR('_', '\0'), ONE_WCHAR('D', '\0'), ONE_WCHAR('A', '\0'), ONE_WCHAR('T', '\0'), ONE_WCHAR('A', '\0'), ONE_WCHAR('\0', '\0')}; /* "CUSTOM_DATA" */
const DRM_WCHAR g_rgwchEncBlkSize                  [] = { ONE_WCHAR('B', '\0'), ONE_WCHAR('L', '\0'), ONE_WCHAR('O', '\0'), ONE_WCHAR('C', '\0'), ONE_WCHAR('K', '\0'), ONE_WCHAR('S', '\0'), ONE_WCHAR('I', '\0'), ONE_WCHAR('Z', '\0'), ONE_WCHAR('E', '\0'), ONE_WCHAR('\0', '\0')}; /* "BLOCKSIZE" */
const DRM_WCHAR g_rgwchCID                         [] = { ONE_WCHAR('C', '\0'), ONE_WCHAR('I', '\0'), ONE_WCHAR('D', '\0'), ONE_WCHAR('\0', '\0')}; /* "CID" */
const DRM_WCHAR g_rgwchVerifyKey                   [] = { ONE_WCHAR('V', '\0'), ONE_WCHAR('E', '\0'), ONE_WCHAR('R', '\0'), ONE_WCHAR('I', '\0'), ONE_WCHAR('F', '\0'), ONE_WCHAR('Y', '\0'), ONE_WCHAR('K', '\0'), ONE_WCHAR('E', '\0'), ONE_WCHAR('Y', '\0'), ONE_WCHAR('\0', '\0')}; /* "VERIFYKEY" */
const DRM_WCHAR g_rgwchDefaultURL                  [] = { ONE_WCHAR('h', '\0'), ONE_WCHAR('t', '\0'), ONE_WCHAR('t', '\0'), ONE_WCHAR('p', '\0'), ONE_WCHAR(':', '\0'), ONE_WCHAR('/', '\0'), ONE_WCHAR('/', '\0'), ONE_WCHAR('b', '\0'), ONE_WCHAR('a', '\0'), ONE_WCHAR('d', '\0'), ONE_WCHAR('.', '\0'), ONE_WCHAR('c', '\0'), ONE_WCHAR('o', '\0'), ONE_WCHAR('n', '\0'), ONE_WCHAR('t', '\0'), ONE_WCHAR('o', '\0'), ONE_WCHAR('s', '\0'), ONE_WCHAR('o', '\0'), ONE_WCHAR('.', '\0'), ONE_WCHAR('c', '\0'), ONE_WCHAR('o', '\0'), ONE_WCHAR('m', '\0'), ONE_WCHAR('/', '\0'), ONE_WCHAR('g', '\0'), ONE_WCHAR('e', '\0'), ONE_WCHAR('t', '\0'), ONE_WCHAR('v', '\0'), ONE_WCHAR('2', '\0'), ONE_WCHAR('l', '\0'), ONE_WCHAR('i', '\0'), ONE_WCHAR('c', '\0'), ONE_WCHAR('.', '\0'), ONE_WCHAR('a', '\0'), ONE_WCHAR('s', '\0'), ONE_WCHAR('p', '\0'), ONE_WCHAR('\0', '\0')}; /* "http://bad.contoso.com/getv2lic.asp" */
const DRM_WCHAR g_rgwchDefaultBlksize              [] = { ONE_WCHAR('2', '\0'), ONE_WCHAR('0', '\0'), ONE_WCHAR('4', '\0'), ONE_WCHAR('8', '\0'), ONE_WCHAR('\0', '\0')}; /* "2048" */
const DRM_WCHAR g_rgwchDefaultFilename             [] = { ONE_WCHAR('c', '\0'), ONE_WCHAR(':', '\0'), ONE_WCHAR('\\', '\0'), ONE_WCHAR('W', '\0'), ONE_WCHAR('M', '\0'), ONE_WCHAR('D', '\0'), ONE_WCHAR('R', '\0'), ONE_WCHAR('M', '\0'), ONE_WCHAR('P', '\0'), ONE_WCHAR('D', '\0'), ONE_WCHAR('\\', '\0'), ONE_WCHAR('P', '\0'), ONE_WCHAR('k', '\0'), ONE_WCHAR('g', '\0'), ONE_WCHAR('I', '\0'), ONE_WCHAR('n', '\0'), ONE_WCHAR('i', '\0'), ONE_WCHAR('t', '\0'), ONE_WCHAR('.', '\0'), ONE_WCHAR('x', '\0'), ONE_WCHAR('m', '\0'), ONE_WCHAR('l', '\0'), ONE_WCHAR('\0', '\0')}; /* "c:\\WMDRMPD\\PkgInit.xml" */
const DRM_WCHAR g_rgwchContents                    [] = { ONE_WCHAR('C', '\0'), ONE_WCHAR('o', '\0'), ONE_WCHAR('n', '\0'), ONE_WCHAR('t', '\0'), ONE_WCHAR('e', '\0'), ONE_WCHAR('n', '\0'), ONE_WCHAR('t', '\0'), ONE_WCHAR('s', '\0'), ONE_WCHAR('\0', '\0')}; /* "Contents" */
const DRM_WCHAR g_rgwchSerial                      [] = { ONE_WCHAR('S', '\0'), ONE_WCHAR('e', '\0'), ONE_WCHAR('r', '\0'), ONE_WCHAR('i', '\0'), ONE_WCHAR('a', '\0'), ONE_WCHAR('l', '\0'), ONE_WCHAR('\0', '\0')}; /* "Serial" */
const DRM_WCHAR g_rgwchTagPrivkey                  [] = { ONE_WCHAR('P', '\0'), ONE_WCHAR('R', '\0'), ONE_WCHAR('I', '\0'), ONE_WCHAR('V', '\0'), ONE_WCHAR('K', '\0'), ONE_WCHAR('E', '\0'), ONE_WCHAR('Y', '\0'), ONE_WCHAR('\0', '\0')}; /* "PRIVKEY" */
const DRM_WCHAR g_rgwchTagKeypair                  [] = { ONE_WCHAR('K', '\0'), ONE_WCHAR('E', '\0'), ONE_WCHAR('Y', '\0'), ONE_WCHAR('P', '\0'), ONE_WCHAR('A', '\0'), ONE_WCHAR('I', '\0'), ONE_WCHAR('R', '\0'), ONE_WCHAR('\0', '\0')}; /* "KEYPAIR" */
const DRM_WCHAR g_rgwchTagExpression               [] = { ONE_WCHAR('E', '\0'), ONE_WCHAR('X', '\0'), ONE_WCHAR('P', '\0'), ONE_WCHAR('R', '\0'), ONE_WCHAR('\0', '\0')}; /* "EXPR" */

const DRM_WCHAR g_rgwchTagRequestDAC               [] = { ONE_WCHAR('R', '\0'), ONE_WCHAR('e', '\0'), ONE_WCHAR('q', '\0'), ONE_WCHAR('u', '\0'), ONE_WCHAR('e', '\0'), ONE_WCHAR('s', '\0'), ONE_WCHAR('t', '\0'), ONE_WCHAR('D', '\0'), ONE_WCHAR('A', '\0'), ONE_WCHAR('C', '\0'), ONE_WCHAR('\0', '\0')}; /* "RequestDAC" */
const DRM_WCHAR g_rgwchTagDAC                      [] = { ONE_WCHAR('D', '\0'), ONE_WCHAR('A', '\0'), ONE_WCHAR('C', '\0'), ONE_WCHAR('\0', '\0')}; /* "DAC" */
const DRM_WCHAR g_rgwchTagDAC_WMDRM_MD             [] = { ONE_WCHAR('P', '\0'), ONE_WCHAR('l', '\0'), ONE_WCHAR('a', '\0'), ONE_WCHAR('y', '\0'), ONE_WCHAR('R', '\0'), ONE_WCHAR('e', '\0'), ONE_WCHAR('a', '\0'), ONE_WCHAR('d', '\0'), ONE_WCHAR('y', '\0'), ONE_WCHAR('\0', '\0')}; /* "PlayReady" */
const DRM_WCHAR g_rgwchTagDAC_PlayReadyND          [] = { ONE_WCHAR('P', '\0'), ONE_WCHAR('l', '\0'), ONE_WCHAR('a', '\0'), ONE_WCHAR('y', '\0'), ONE_WCHAR('R', '\0'), ONE_WCHAR('e', '\0'), ONE_WCHAR('a', '\0'), ONE_WCHAR('d', '\0'), ONE_WCHAR('y', '\0'), ONE_WCHAR('N', '\0'), ONE_WCHAR('D', '\0'), ONE_WCHAR('\0', '\0')}; /* "PlayReadyND" */
const DRM_WCHAR g_rgwchTagDACResponse              [] = { ONE_WCHAR('D', '\0'), ONE_WCHAR('a', '\0'), ONE_WCHAR('c', '\0'), ONE_WCHAR('R', '\0'), ONE_WCHAR('e', '\0'), ONE_WCHAR('s', '\0'), ONE_WCHAR('p', '\0'), ONE_WCHAR('o', '\0'), ONE_WCHAR('n', '\0'), ONE_WCHAR('s', '\0'), ONE_WCHAR('e', '\0'), ONE_WCHAR('\0', '\0')}; /* "DacResponse" */
const DRM_WCHAR g_rgwchTagManufName                [] = { ONE_WCHAR('M', '\0'), ONE_WCHAR('a', '\0'), ONE_WCHAR('n', '\0'), ONE_WCHAR('u', '\0'), ONE_WCHAR('f', '\0'), ONE_WCHAR('a', '\0'), ONE_WCHAR('c', '\0'), ONE_WCHAR('t', '\0'), ONE_WCHAR('u', '\0'), ONE_WCHAR('r', '\0'), ONE_WCHAR('e', '\0'), ONE_WCHAR('r', '\0'), ONE_WCHAR('N', '\0'), ONE_WCHAR('a', '\0'), ONE_WCHAR('m', '\0'), ONE_WCHAR('e', '\0'), ONE_WCHAR('\0', '\0')}; /* "ManufacturerName" */

const DRM_BYTE  g_rgbFFFE              [__CB_DECL(2)] = { 0xff, 0xfe };

/*
**  XPaths
*/
const DRM_WCHAR g_rgwchXPathDACPubKey                [] = { ONE_WCHAR('D', '\0'), ONE_WCHAR('A', '\0'), ONE_WCHAR('C', '\0'), ONE_WCHAR('/', '\0'), ONE_WCHAR('P', '\0'), ONE_WCHAR('U', '\0'), ONE_WCHAR('B', '\0'), ONE_WCHAR('L', '\0'), ONE_WCHAR('I', '\0'), ONE_WCHAR('C', '\0'), ONE_WCHAR('K', '\0'), ONE_WCHAR('E', '\0'), ONE_WCHAR('Y', '\0'), ONE_WCHAR('\0', '\0')}; /* "DAC/PUBLICKEY" */
const DRM_WCHAR g_rgwchXPathFallbackPubKey           [] = { ONE_WCHAR('F', '\0'), ONE_WCHAR('A', '\0'), ONE_WCHAR('L', '\0'), ONE_WCHAR('L', '\0'), ONE_WCHAR('B', '\0'), ONE_WCHAR('A', '\0'), ONE_WCHAR('C', '\0'), ONE_WCHAR('K', '\0'), ONE_WCHAR('/', '\0'), ONE_WCHAR('P', '\0'), ONE_WCHAR('U', '\0'), ONE_WCHAR('B', '\0'), ONE_WCHAR('L', '\0'), ONE_WCHAR('I', '\0'), ONE_WCHAR('C', '\0'), ONE_WCHAR('K', '\0'), ONE_WCHAR('E', '\0'), ONE_WCHAR('Y', '\0'), ONE_WCHAR('\0', '\0')}; /* "FALLBACK/PUBLICKEY" */
const DRM_WCHAR g_rgwchXPathWMDRM_MDPubKey           [] = { ONE_WCHAR('P', '\0'), ONE_WCHAR('l', '\0'), ONE_WCHAR('a', '\0'), ONE_WCHAR('y', '\0'), ONE_WCHAR('R', '\0'), ONE_WCHAR('e', '\0'), ONE_WCHAR('a', '\0'), ONE_WCHAR('d', '\0'), ONE_WCHAR('y', '\0'), ONE_WCHAR('/', '\0'), ONE_WCHAR('P', '\0'), ONE_WCHAR('U', '\0'), ONE_WCHAR('B', '\0'), ONE_WCHAR('L', '\0'), ONE_WCHAR('I', '\0'), ONE_WCHAR('C', '\0'), ONE_WCHAR('K', '\0'), ONE_WCHAR('E', '\0'), ONE_WCHAR('Y', '\0'), ONE_WCHAR('\0', '\0')}; /* "PlayReady/PUBLICKEY" */
const DRM_WCHAR g_rgwchXPathPlayReadyNDPubKeyModulus [] = { ONE_WCHAR('P', '\0'), ONE_WCHAR('l', '\0'), ONE_WCHAR('a', '\0'), ONE_WCHAR('y', '\0'), ONE_WCHAR('R', '\0'), ONE_WCHAR('e', '\0'), ONE_WCHAR('a', '\0'), ONE_WCHAR('d', '\0'), ONE_WCHAR('y', '\0'), ONE_WCHAR('N', '\0'), ONE_WCHAR('D', '\0'), ONE_WCHAR('/', '\0'), ONE_WCHAR('P', '\0'), ONE_WCHAR('U', '\0'), ONE_WCHAR('B', '\0'), ONE_WCHAR('L', '\0'), ONE_WCHAR('I', '\0'), ONE_WCHAR('C', '\0'), ONE_WCHAR('K', '\0'), ONE_WCHAR('E', '\0'), ONE_WCHAR('Y', '\0'), ONE_WCHAR('/', '\0'), ONE_WCHAR('M', '\0'), ONE_WCHAR('o', '\0'), ONE_WCHAR('d', '\0'), ONE_WCHAR('u', '\0'), ONE_WCHAR('l', '\0'), ONE_WCHAR('u', '\0'), ONE_WCHAR('s', '\0'), ONE_WCHAR('\0', '\0')}; /* "PlayReadyND/PUBLICKEY/Modulus" */
const DRM_WCHAR g_rgwchXPathPlayReadyNDPubKeyExponent[] = { ONE_WCHAR('P', '\0'), ONE_WCHAR('l', '\0'), ONE_WCHAR('a', '\0'), ONE_WCHAR('y', '\0'), ONE_WCHAR('R', '\0'), ONE_WCHAR('e', '\0'), ONE_WCHAR('a', '\0'), ONE_WCHAR('d', '\0'), ONE_WCHAR('y', '\0'), ONE_WCHAR('N', '\0'), ONE_WCHAR('D', '\0'), ONE_WCHAR('/', '\0'), ONE_WCHAR('P', '\0'), ONE_WCHAR('U', '\0'), ONE_WCHAR('B', '\0'), ONE_WCHAR('L', '\0'), ONE_WCHAR('I', '\0'), ONE_WCHAR('C', '\0'), ONE_WCHAR('K', '\0'), ONE_WCHAR('E', '\0'), ONE_WCHAR('Y', '\0'), ONE_WCHAR('/', '\0'), ONE_WCHAR('E', '\0'), ONE_WCHAR('x', '\0'), ONE_WCHAR('p', '\0'), ONE_WCHAR('o', '\0'), ONE_WCHAR('n', '\0'), ONE_WCHAR('e', '\0'), ONE_WCHAR('n', '\0'), ONE_WCHAR('t', '\0'), ONE_WCHAR('\0', '\0')}; /* "PlayReadyND/PUBLICKEY/Exponent" */

const DRM_WCHAR g_rgwchXPathDACPrivKey             [] = { ONE_WCHAR('D', '\0'), ONE_WCHAR('A', '\0'), ONE_WCHAR('C', '\0'), ONE_WCHAR('/', '\0'), ONE_WCHAR('P', '\0'), ONE_WCHAR('R', '\0'), ONE_WCHAR('I', '\0'), ONE_WCHAR('V', '\0'), ONE_WCHAR('K', '\0'), ONE_WCHAR('E', '\0'), ONE_WCHAR('Y', '\0'), ONE_WCHAR('\0', '\0')}; /* "DAC/PRIVKEY" */
const DRM_WCHAR g_rgwchXPathFallbackPrivKey        [] = { ONE_WCHAR('F', '\0'), ONE_WCHAR('A', '\0'), ONE_WCHAR('L', '\0'), ONE_WCHAR('L', '\0'), ONE_WCHAR('B', '\0'), ONE_WCHAR('A', '\0'), ONE_WCHAR('C', '\0'), ONE_WCHAR('K', '\0'), ONE_WCHAR('/', '\0'), ONE_WCHAR('P', '\0'), ONE_WCHAR('R', '\0'), ONE_WCHAR('I', '\0'), ONE_WCHAR('V', '\0'), ONE_WCHAR('K', '\0'), ONE_WCHAR('E', '\0'), ONE_WCHAR('Y', '\0'), ONE_WCHAR('\0', '\0')}; /* "FALLBACK/PRIVKEY" */
const DRM_WCHAR g_rgwchXPathWMDRMPrivKey           [] = { ONE_WCHAR('P', '\0'), ONE_WCHAR('l', '\0'), ONE_WCHAR('a', '\0'), ONE_WCHAR('y', '\0'), ONE_WCHAR('R', '\0'), ONE_WCHAR('e', '\0'), ONE_WCHAR('a', '\0'), ONE_WCHAR('d', '\0'), ONE_WCHAR('y', '\0'), ONE_WCHAR('/', '\0'), ONE_WCHAR('P', '\0'), ONE_WCHAR('R', '\0'), ONE_WCHAR('I', '\0'), ONE_WCHAR('V', '\0'), ONE_WCHAR('K', '\0'), ONE_WCHAR('E', '\0'), ONE_WCHAR('Y', '\0'), ONE_WCHAR('\0', '\0')}; /* "PlayReady/PRIVKEY" */
const DRM_WCHAR g_rgwchXPathWMDRMPubKey            [] = { ONE_WCHAR('P', '\0'), ONE_WCHAR('l', '\0'), ONE_WCHAR('a', '\0'), ONE_WCHAR('y', '\0'), ONE_WCHAR('R', '\0'), ONE_WCHAR('e', '\0'), ONE_WCHAR('a', '\0'), ONE_WCHAR('d', '\0'), ONE_WCHAR('y', '\0'), ONE_WCHAR('/', '\0'), ONE_WCHAR('P', '\0'), ONE_WCHAR('U', '\0'), ONE_WCHAR('B', '\0'), ONE_WCHAR('K', '\0'), ONE_WCHAR('E', '\0'), ONE_WCHAR('Y', '\0'), ONE_WCHAR('\0', '\0')}; /* "PlayReady/PUBKEY" */
const DRM_WCHAR g_rgwchXPathFallbackSecVer         [] = { ONE_WCHAR('F', '\0'), ONE_WCHAR('A', '\0'), ONE_WCHAR('L', '\0'), ONE_WCHAR('L', '\0'), ONE_WCHAR('B', '\0'), ONE_WCHAR('A', '\0'), ONE_WCHAR('C', '\0'), ONE_WCHAR('K', '\0'), ONE_WCHAR('/', '\0'), ONE_WCHAR('S', '\0'), ONE_WCHAR('E', '\0'), ONE_WCHAR('C', '\0'), ONE_WCHAR('U', '\0'), ONE_WCHAR('R', '\0'), ONE_WCHAR('I', '\0'), ONE_WCHAR('T', '\0'), ONE_WCHAR('Y', '\0'), ONE_WCHAR('V', '\0'), ONE_WCHAR('E', '\0'), ONE_WCHAR('R', '\0'), ONE_WCHAR('S', '\0'), ONE_WCHAR('I', '\0'), ONE_WCHAR('O', '\0'), ONE_WCHAR('N', '\0'), ONE_WCHAR('\0', '\0')}; /* "FALLBACK/SECURITYVERSION" */

/* generateDAC */

const DRM_WCHAR g_rgwchTagDataSecurityVersionValue [] = { ONE_WCHAR('2', '\0'), ONE_WCHAR('.', '\0'), ONE_WCHAR('3', '\0'), ONE_WCHAR('.', '\0'), ONE_WCHAR('0', '\0'), ONE_WCHAR('.', '\0'), ONE_WCHAR('2', '\0'), ONE_WCHAR('\0', '\0')}; /* "2.3.0.2" */
const DRM_WCHAR g_rgwchAuthorizationIDDAC          [] = { ONE_WCHAR('4', '\0'), ONE_WCHAR('2', '\0'), ONE_WCHAR('\0', '\0')}; /* "42" */
const DRM_WCHAR g_rgwchAuthIDDACRoot               [] = { ONE_WCHAR('1', '\0'), ONE_WCHAR('\0', '\0')}; /* "1" */
const DRM_WCHAR g_rgwchTagSecurityLevelValue       [] = { ONE_WCHAR('2', '\0'), ONE_WCHAR('0', '\0'), ONE_WCHAR('0', '\0'), ONE_WCHAR('0', '\0'), ONE_WCHAR('\0', '\0')}; /* "2000" */

/* metering tools */

const DRM_WCHAR g_rgwchCertMetercert               [] = { ONE_WCHAR('A', '\0'), ONE_WCHAR('A', '\0'), ONE_WCHAR('E', '\0'), ONE_WCHAR('A', '\0'), ONE_WCHAR('A', '\0'), ONE_WCHAR('D', '\0'), ONE_WCHAR('g', '\0'), ONE_WCHAR('A', '\0'), ONE_WCHAR('A', '\0'), ONE_WCHAR('A', '\0'), ONE_WCHAR('B', '\0'), ONE_WCHAR('1', '\0'), ONE_WCHAR('7', '\0'), ONE_WCHAR('b', '\0'), ONE_WCHAR('f', '\0'), ONE_WCHAR('g', '\0'), ONE_WCHAR('!', '\0'), ONE_WCHAR('c', '\0'), ONE_WCHAR('q', '\0'), ONE_WCHAR('D', '\0'), ONE_WCHAR('L', '\0'), ONE_WCHAR('B', '\0'), ONE_WCHAR('2', '\0'), ONE_WCHAR('c', '\0'), ONE_WCHAR('l', '\0'), ONE_WCHAR('H', '\0'), ONE_WCHAR('G', '\0'), ONE_WCHAR('U', '\0'), ONE_WCHAR('7', '\0'), ONE_WCHAR('P', '\0'), ONE_WCHAR('6', '\0'), ONE_WCHAR('z', '\0'), ONE_WCHAR('k', '\0'), ONE_WCHAR('r', '\0'), ONE_WCHAR('O', '\0'), ONE_WCHAR('C', '\0'), ONE_WCHAR('V', '\0'), ONE_WCHAR('U', '\0'), ONE_WCHAR('d', '\0'), ONE_WCHAR('d', '\0'), ONE_WCHAR('X', '\0'), ONE_WCHAR('9', '\0'), ONE_WCHAR('5', '\0'), ONE_WCHAR('U', '\0'), ONE_WCHAR('Y', '\0'), ONE_WCHAR('k', '\0'), ONE_WCHAR('q', '\0'), ONE_WCHAR('p', '\0'), ONE_WCHAR('j', '\0'), ONE_WCHAR('q', '\0'), ONE_WCHAR('v', '\0'), ONE_WCHAR('o', '\0'), ONE_WCHAR('I', '\0'), ONE_WCHAR('r', '\0'), ONE_WCHAR('4', '\0'), ONE_WCHAR('e', '\0'), ONE_WCHAR('z', '\0'), ONE_WCHAR('q', '\0'), ONE_WCHAR('w', '\0'), ONE_WCHAR('2', '\0'), ONE_WCHAR('z', '\0'), ONE_WCHAR('g', '\0'), ONE_WCHAR('w', '\0'), ONE_WCHAR('f', '\0'), ONE_WCHAR('r', '\0'), ONE_WCHAR('v', '\0'), ONE_WCHAR('K', '\0'), ONE_WCHAR('R', '\0'), ONE_WCHAR('1', '\0'), ONE_WCHAR('d', '\0'), ONE_WCHAR('q', '\0'), ONE_WCHAR('!', '\0'), ONE_WCHAR('E', '\0'), ONE_WCHAR('z', '\0'), ONE_WCHAR('d', '\0'), ONE_WCHAR('G', '\0'), ONE_WCHAR('D', '\0'), ONE_WCHAR('8', '\0'), ONE_WCHAR('N', '\0'), ONE_WCHAR('D', '\0'), ONE_WCHAR('2', '\0'), ONE_WCHAR('I', '\0'), ONE_WCHAR('h', '\0'), ONE_WCHAR('k', '\0'), ONE_WCHAR('n', '\0'), ONE_WCHAR('0', '\0'), ONE_WCHAR('O', '\0'), ONE_WCHAR('P', '\0'), ONE_WCHAR('E', '\0'), ONE_WCHAR('o', '\0'), ONE_WCHAR('W', '\0'), ONE_WCHAR('Z', '\0'), ONE_WCHAR('Z', '\0'), ONE_WCHAR('K', '\0'), ONE_WCHAR('C', '\0'), ONE_WCHAR('w', '\0'), ONE_WCHAR('g', '\0'), ONE_WCHAR('i', '\0'), ONE_WCHAR('d', '\0'), ONE_WCHAR('p', '\0'), ONE_WCHAR('7', '\0'), ONE_WCHAR('Y', '\0'), ONE_WCHAR('5', '\0'), ONE_WCHAR('S', '\0'), ONE_WCHAR('H', '\0'), ONE_WCHAR('R', '\0'), ONE_WCHAR('!', '\0'), ONE_WCHAR('N', '\0'), ONE_WCHAR('F', '\0'), ONE_WCHAR('J', '\0'), ONE_WCHAR('J', '\0'), ONE_WCHAR('a', '\0'), ONE_WCHAR('z', '\0'), ONE_WCHAR('d', '\0'), ONE_WCHAR('P', '\0'), ONE_WCHAR('W', '\0'), ONE_WCHAR('f', '\0'), ONE_WCHAR('B', '\0'), ONE_WCHAR('Q', '\0'), ONE_WCHAR('E', '\0'), ONE_WCHAR('B', '\0'), ONE_WCHAR('B', '\0'), ONE_WCHAR('4', '\0'), ONE_WCHAR('A', '\0'), ONE_WCHAR('A', '\0'), ONE_WCHAR('A', '\0'), ONE_WCHAR('A', '\0'), ONE_WCHAR('B', '\0'), ONE_WCHAR('A', '\0'), ONE_WCHAR('A', '\0'), ONE_WCHAR('A', '\0'), ONE_WCHAR('A', '\0'), ONE_WCHAR('A', '\0'), ONE_WCHAR('Q', '\0'), ONE_WCHAR('A', '\0'), ONE_WCHAR('A', '\0'), ONE_WCHAR('A', '\0'), ONE_WCHAR('A', '\0'), ONE_WCHAR('E', '\0'), ONE_WCHAR('=', '\0'), ONE_WCHAR('\0', '\0')}; /* "AAEAADgAAAB17bfg!cqDLB2clHGU7P6zkrOCVUddX95UYkqpjqvoIr4ezqw2zgwfrvKR1dq!EzdGD8ND2Ihkn0OPEoWZZKCwgidp7Y5SHR!NFJJazdPWfBQEBB4AAAABAAAAAQAAAAE=" */

/* secure clock */

const DRM_WCHAR g_rgwchCertSecureClock             [] = { ONE_WCHAR('A', '\0'), ONE_WCHAR('A', '\0'), ONE_WCHAR('E', '\0'), ONE_WCHAR('A', '\0'), ONE_WCHAR('A', '\0'), ONE_WCHAR('D', '\0'), ONE_WCHAR('g', '\0'), ONE_WCHAR('A', '\0'), ONE_WCHAR('A', '\0'), ONE_WCHAR('A', '\0'), ONE_WCHAR('C', '\0'), ONE_WCHAR('5', '\0'), ONE_WCHAR('z', '\0'), ONE_WCHAR('9', '\0'), ONE_WCHAR('y', '\0'), ONE_WCHAR('i', '\0'), ONE_WCHAR('h', '\0'), ONE_WCHAR('d', '\0'), ONE_WCHAR('T', '\0'), ONE_WCHAR('T', '\0'), ONE_WCHAR('D', '\0'), ONE_WCHAR('j', '\0'), ONE_WCHAR('6', '\0'), ONE_WCHAR('g', '\0'), ONE_WCHAR('W', '\0'), ONE_WCHAR('u', '\0'), ONE_WCHAR('C', '\0'), ONE_WCHAR('2', '\0'), ONE_WCHAR('h', '\0'), ONE_WCHAR('o', '\0'), ONE_WCHAR('v', '\0'), ONE_WCHAR('Q', '\0'), ONE_WCHAR('Q', '\0'), ONE_WCHAR('i', '\0'), ONE_WCHAR('L', '\0'), ONE_WCHAR('z', '\0'), ONE_WCHAR('A', '\0'), ONE_WCHAR('x', '\0'), ONE_WCHAR('N', '\0'), ONE_WCHAR('d', '\0'), ONE_WCHAR('j', '\0'), ONE_WCHAR('i', '\0'), ONE_WCHAR('u', '\0'), ONE_WCHAR('p', '\0'), ONE_WCHAR('l', '\0'), ONE_WCHAR('d', '\0'), ONE_WCHAR('!', '\0'), ONE_WCHAR('0', '\0'), ONE_WCHAR('6', '\0'), ONE_WCHAR('z', '\0'), ONE_WCHAR('j', '\0'), ONE_WCHAR('V', '\0'), ONE_WCHAR('y', '\0'), ONE_WCHAR('J', '\0'), ONE_WCHAR('C', '\0'), ONE_WCHAR('P', '\0'), ONE_WCHAR('S', '\0'), ONE_WCHAR('j', '\0'), ONE_WCHAR('O', '\0'), ONE_WCHAR('L', '\0'), ONE_WCHAR('w', '\0'), ONE_WCHAR('j', '\0'), ONE_WCHAR('k', '\0'), ONE_WCHAR('I', '\0'), ONE_WCHAR('5', '\0'), ONE_WCHAR('S', '\0'), ONE_WCHAR('m', '\0'), ONE_WCHAR('q', '\0'), ONE_WCHAR('r', '\0'), ONE_WCHAR('n', '\0'), ONE_WCHAR('E', '\0'), ONE_WCHAR('D', '\0'), ONE_WCHAR('j', '\0'), ONE_WCHAR('o', '\0'), ONE_WCHAR('2', '\0'), ONE_WCHAR('H', '\0'), ONE_WCHAR('!', '\0'), ONE_WCHAR('K', '\0'), ONE_WCHAR('J', '\0'), ONE_WCHAR('V', '\0'), ONE_WCHAR('4', '\0'), ONE_WCHAR('q', '\0'), ONE_WCHAR('k', '\0'), ONE_WCHAR('2', '\0'), ONE_WCHAR('C', '\0'), ONE_WCHAR('Q', '\0'), ONE_WCHAR('0', '\0'), ONE_WCHAR('g', '\0'), ONE_WCHAR('M', '\0'), ONE_WCHAR('w', '\0'), ONE_WCHAR('y', '\0'), ONE_WCHAR('u', '\0'), ONE_WCHAR('r', '\0'), ONE_WCHAR('m', '\0'), ONE_WCHAR('C', '\0'), ONE_WCHAR('1', '\0'), ONE_WCHAR('o', '\0'), ONE_WCHAR('O', '\0'), ONE_WCHAR('V', '\0'), ONE_WCHAR('c', '\0'), ONE_WCHAR('1', '\0'), ONE_WCHAR('u', '\0'), ONE_WCHAR('T', '\0'), ONE_WCHAR('6', '\0'), ONE_WCHAR('4', '\0'), ONE_WCHAR('v', '\0'), ONE_WCHAR('Q', '\0'), ONE_WCHAR('!', '\0'), ONE_WCHAR('u', '\0'), ONE_WCHAR('o', '\0'), ONE_WCHAR('H', '\0'), ONE_WCHAR('o', '\0'), ONE_WCHAR('b', '\0'), ONE_WCHAR('i', '\0'), ONE_WCHAR('F', '\0'), ONE_WCHAR('e', '\0'), ONE_WCHAR('g', '\0'), ONE_WCHAR('2', '\0'), ONE_WCHAR('N', '\0'), ONE_WCHAR('j', '\0'), ONE_WCHAR('D', '\0'), ONE_WCHAR('B', '\0'), ONE_WCHAR('8', '\0'), ONE_WCHAR('A', '\0'), ONE_WCHAR('A', '\0'), ONE_WCHAR('A', '\0'), ONE_WCHAR('A', '\0'), ONE_WCHAR('B', '\0'), ONE_WCHAR('A', '\0'), ONE_WCHAR('A', '\0'), ONE_WCHAR('A', '\0'), ONE_WCHAR('A', '\0'), ONE_WCHAR('A', '\0'), ONE_WCHAR('Q', '\0'), ONE_WCHAR('A', '\0'), ONE_WCHAR('A', '\0'), ONE_WCHAR('A', '\0'), ONE_WCHAR('A', '\0'), ONE_WCHAR('E', '\0'), ONE_WCHAR('=', '\0'), ONE_WCHAR('\0', '\0')}; /* "AAEAADgAAAC5z9yihdTTDj6gWuC2hovQQiLzAxNdjiupld!06zjVyJCPSjOLwjkI5SmqrnEDjo2H!KJV4qk2CQ0gMwyurmC1oOVc1uT64vQ!uoHobiFeg2NjDB8AAAABAAAAAQAAAAE=" */

/* XML utils */

const DRM_WCHAR g_rgwchTestTemplate                [] = { ONE_WCHAR('<', '\0'), ONE_WCHAR('I', '\0'), ONE_WCHAR('N', '\0'), ONE_WCHAR('C', '\0'), ONE_WCHAR('L', '\0'), ONE_WCHAR('U', '\0'), ONE_WCHAR('D', '\0'), ONE_WCHAR('E', '\0'), ONE_WCHAR('S', '\0'), ONE_WCHAR('>', '\0'), ONE_WCHAR('<', '\0'), ONE_WCHAR('I', '\0'), ONE_WCHAR('N', '\0'), ONE_WCHAR('C', '\0'), ONE_WCHAR('L', '\0'), ONE_WCHAR('U', '\0'), ONE_WCHAR('D', '\0'), ONE_WCHAR('E', '\0'), ONE_WCHAR('>', '\0'), ONE_WCHAR('x', '\0'), ONE_WCHAR('y', '\0'), ONE_WCHAR('z', '\0'), ONE_WCHAR('<', '\0'), ONE_WCHAR('/', '\0'), ONE_WCHAR('I', '\0'), ONE_WCHAR('N', '\0'), ONE_WCHAR('C', '\0'), ONE_WCHAR('L', '\0'), ONE_WCHAR('U', '\0'), ONE_WCHAR('D', '\0'), ONE_WCHAR('E', '\0'), ONE_WCHAR('>', '\0'), ONE_WCHAR('<', '\0'), ONE_WCHAR('/', '\0'), ONE_WCHAR('I', '\0'), ONE_WCHAR('N', '\0'), ONE_WCHAR('C', '\0'), ONE_WCHAR('L', '\0'), ONE_WCHAR('U', '\0'), ONE_WCHAR('D', '\0'), ONE_WCHAR('E', '\0'), ONE_WCHAR('S', '\0'), ONE_WCHAR('>', '\0'), ONE_WCHAR('\0', '\0')}; /* "<INCLUDES><INCLUDE>xyz</INCLUDE></INCLUDES>" */
const DRM_WCHAR g_rgwchXMLTree1                    [] = { ONE_WCHAR('<', '\0'), ONE_WCHAR('I', '\0'), ONE_WCHAR('N', '\0'), ONE_WCHAR('C', '\0'), ONE_WCHAR('L', '\0'), ONE_WCHAR('U', '\0'), ONE_WCHAR('D', '\0'), ONE_WCHAR('E', '\0'), ONE_WCHAR('S', '\0'), ONE_WCHAR('>', '\0'), ONE_WCHAR('<', '\0'), ONE_WCHAR('I', '\0'), ONE_WCHAR('N', '\0'), ONE_WCHAR('C', '\0'), ONE_WCHAR('L', '\0'), ONE_WCHAR('U', '\0'), ONE_WCHAR('D', '\0'), ONE_WCHAR('E', '\0'), ONE_WCHAR('1', '\0'), ONE_WCHAR('>', '\0'), ONE_WCHAR('a', '\0'), ONE_WCHAR('b', '\0'), ONE_WCHAR('c', '\0'), ONE_WCHAR('<', '\0'), ONE_WCHAR('/', '\0'), ONE_WCHAR('I', '\0'), ONE_WCHAR('N', '\0'), ONE_WCHAR('C', '\0'), ONE_WCHAR('L', '\0'), ONE_WCHAR('U', '\0'), ONE_WCHAR('D', '\0'), ONE_WCHAR('E', '\0'), ONE_WCHAR('1', '\0'), ONE_WCHAR('>', '\0'), ONE_WCHAR('<', '\0'), ONE_WCHAR('I', '\0'), ONE_WCHAR('N', '\0'), ONE_WCHAR('C', '\0'), ONE_WCHAR('L', '\0'), ONE_WCHAR('U', '\0'), ONE_WCHAR('D', '\0'), ONE_WCHAR('E', '\0'), ONE_WCHAR('>', '\0'), ONE_WCHAR('x', '\0'), ONE_WCHAR('y', '\0'), ONE_WCHAR('x', '\0'), ONE_WCHAR('<', '\0'), ONE_WCHAR('/', '\0'), ONE_WCHAR('I', '\0'), ONE_WCHAR('N', '\0'), ONE_WCHAR('C', '\0'), ONE_WCHAR('L', '\0'), ONE_WCHAR('U', '\0'), ONE_WCHAR('D', '\0'), ONE_WCHAR('E', '\0'), ONE_WCHAR('>', '\0'), ONE_WCHAR('<', '\0'), ONE_WCHAR('/', '\0'), ONE_WCHAR('I', '\0'), ONE_WCHAR('N', '\0'), ONE_WCHAR('C', '\0'), ONE_WCHAR('L', '\0'), ONE_WCHAR('U', '\0'), ONE_WCHAR('D', '\0'), ONE_WCHAR('E', '\0'), ONE_WCHAR('S', '\0'), ONE_WCHAR('>', '\0'), ONE_WCHAR('\0', '\0')}; /* "<INCLUDES><INCLUDE1>abc</INCLUDE1><INCLUDE>xyx</INCLUDE></INCLUDES>" */
const DRM_WCHAR g_rgwchXMLTree2                    [] = { ONE_WCHAR('<', '\0'), ONE_WCHAR('D', '\0'), ONE_WCHAR('A', '\0'), ONE_WCHAR('T', '\0'), ONE_WCHAR('A', '\0'), ONE_WCHAR('>', '\0'), ONE_WCHAR('<', '\0'), ONE_WCHAR('K', '\0'), ONE_WCHAR('I', '\0'), ONE_WCHAR('D', '\0'), ONE_WCHAR('>', '\0'), ONE_WCHAR('a', '\0'), ONE_WCHAR('b', '\0'), ONE_WCHAR('c', '\0'), ONE_WCHAR('<', '\0'), ONE_WCHAR('/', '\0'), ONE_WCHAR('K', '\0'), ONE_WCHAR('I', '\0'), ONE_WCHAR('D', '\0'), ONE_WCHAR('>', '\0'), ONE_WCHAR('<', '\0'), ONE_WCHAR('L', '\0'), ONE_WCHAR('I', '\0'), ONE_WCHAR('D', '\0'), ONE_WCHAR('>', '\0'), ONE_WCHAR('p', '\0'), ONE_WCHAR('q', '\0'), ONE_WCHAR('r', '\0'), ONE_WCHAR('<', '\0'), ONE_WCHAR('/', '\0'), ONE_WCHAR('L', '\0'), ONE_WCHAR('I', '\0'), ONE_WCHAR('D', '\0'), ONE_WCHAR('>', '\0'), ONE_WCHAR('<', '\0'), ONE_WCHAR('M', '\0'), ONE_WCHAR('E', '\0'), ONE_WCHAR('T', '\0'), ONE_WCHAR('A', '\0'), ONE_WCHAR('>', '\0'), ONE_WCHAR('<', '\0'), ONE_WCHAR('U', '\0'), ONE_WCHAR('I', '\0'), ONE_WCHAR('D', '\0'), ONE_WCHAR('>', '\0'), ONE_WCHAR('x', '\0'), ONE_WCHAR('y', '\0'), ONE_WCHAR('z', '\0'), ONE_WCHAR('<', '\0'), ONE_WCHAR('/', '\0'), ONE_WCHAR('U', '\0'), ONE_WCHAR('I', '\0'), ONE_WCHAR('D', '\0'), ONE_WCHAR('>', '\0'), ONE_WCHAR('<', '\0'), ONE_WCHAR('L', '\0'), ONE_WCHAR('G', '\0'), ONE_WCHAR('P', '\0'), ONE_WCHAR('U', '\0'), ONE_WCHAR('B', '\0'), ONE_WCHAR('K', '\0'), ONE_WCHAR('E', '\0'), ONE_WCHAR('Y', '\0'), ONE_WCHAR('>', '\0'), ONE_WCHAR('s', '\0'), ONE_WCHAR('f', '\0'), ONE_WCHAR('s', '\0'), ONE_WCHAR('d', '\0'), ONE_WCHAR('<', '\0'), ONE_WCHAR('/', '\0'), ONE_WCHAR('L', '\0'), ONE_WCHAR('G', '\0'), ONE_WCHAR('P', '\0'), ONE_WCHAR('U', '\0'), ONE_WCHAR('B', '\0'), ONE_WCHAR('K', '\0'), ONE_WCHAR('E', '\0'), ONE_WCHAR('Y', '\0'), ONE_WCHAR('>', '\0'), ONE_WCHAR('<', '\0'), ONE_WCHAR('/', '\0'), ONE_WCHAR('M', '\0'), ONE_WCHAR('E', '\0'), ONE_WCHAR('T', '\0'), ONE_WCHAR('A', '\0'), ONE_WCHAR('>', '\0'), ONE_WCHAR('<', '\0'), ONE_WCHAR('/', '\0'), ONE_WCHAR('D', '\0'), ONE_WCHAR('A', '\0'), ONE_WCHAR('T', '\0'), ONE_WCHAR('A', '\0'), ONE_WCHAR('>', '\0'), ONE_WCHAR('\0', '\0')}; /* "<DATA><KID>abc</KID><LID>pqr</LID><META><UID>xyz</UID><LGPUBKEY>sfsd</LGPUBKEY></META></DATA>" */
const DRM_WCHAR g_rgwchXMLTree3                    [] = { ONE_WCHAR('<', '\0'), ONE_WCHAR('L', '\0'), ONE_WCHAR('I', '\0'), ONE_WCHAR('D', '\0'), ONE_WCHAR('>', '\0'), ONE_WCHAR('p', '\0'), ONE_WCHAR('q', '\0'), ONE_WCHAR('r', '\0'), ONE_WCHAR('<', '\0'), ONE_WCHAR('/', '\0'), ONE_WCHAR('L', '\0'), ONE_WCHAR('I', '\0'), ONE_WCHAR('D', '\0'), ONE_WCHAR('>', '\0'), ONE_WCHAR('<', '\0'), ONE_WCHAR('M', '\0'), ONE_WCHAR('E', '\0'), ONE_WCHAR('T', '\0'), ONE_WCHAR('A', '\0'), ONE_WCHAR('>', '\0'), ONE_WCHAR('<', '\0'), ONE_WCHAR('U', '\0'), ONE_WCHAR('I', '\0'), ONE_WCHAR('D', '\0'), ONE_WCHAR('>', '\0'), ONE_WCHAR('x', '\0'), ONE_WCHAR('y', '\0'), ONE_WCHAR('z', '\0'), ONE_WCHAR('<', '\0'), ONE_WCHAR('/', '\0'), ONE_WCHAR('U', '\0'), ONE_WCHAR('I', '\0'), ONE_WCHAR('D', '\0'), ONE_WCHAR('>', '\0'), ONE_WCHAR('<', '\0'), ONE_WCHAR('/', '\0'), ONE_WCHAR('M', '\0'), ONE_WCHAR('E', '\0'), ONE_WCHAR('T', '\0'), ONE_WCHAR('A', '\0'), ONE_WCHAR('>', '\0'), ONE_WCHAR('\0', '\0')}; /* "<LID>pqr</LID><META><UID>xyz</UID></META>" */
const DRM_WCHAR g_rgwchXMLTree4                    [] = { ONE_WCHAR('<', '\0'), ONE_WCHAR('K', '\0'), ONE_WCHAR('I', '\0'), ONE_WCHAR('D', '\0'), ONE_WCHAR('>', '\0'), ONE_WCHAR('a', '\0'), ONE_WCHAR('b', '\0'), ONE_WCHAR('c', '\0'), ONE_WCHAR('<', '\0'), ONE_WCHAR('/', '\0'), ONE_WCHAR('K', '\0'), ONE_WCHAR('I', '\0'), ONE_WCHAR('D', '\0'), ONE_WCHAR('>', '\0'), ONE_WCHAR('<', '\0'), ONE_WCHAR('M', '\0'), ONE_WCHAR('E', '\0'), ONE_WCHAR('T', '\0'), ONE_WCHAR('A', '\0'), ONE_WCHAR('>', '\0'), ONE_WCHAR('<', '\0'), ONE_WCHAR('U', '\0'), ONE_WCHAR('I', '\0'), ONE_WCHAR('D', '\0'), ONE_WCHAR('>', '\0'), ONE_WCHAR('x', '\0'), ONE_WCHAR('y', '\0'), ONE_WCHAR('z', '\0'), ONE_WCHAR('<', '\0'), ONE_WCHAR('/', '\0'), ONE_WCHAR('U', '\0'), ONE_WCHAR('I', '\0'), ONE_WCHAR('D', '\0'), ONE_WCHAR('>', '\0'), ONE_WCHAR('<', '\0'), ONE_WCHAR('/', '\0'), ONE_WCHAR('M', '\0'), ONE_WCHAR('E', '\0'), ONE_WCHAR('T', '\0'), ONE_WCHAR('A', '\0'), ONE_WCHAR('>', '\0'), ONE_WCHAR('\0', '\0')}; /* "<KID>abc</KID><META><UID>xyz</UID></META>" */

const DRM_CONST_STRING  g_dstrTagRequestDAC                     = CREATE_DRM_STRING(g_rgwchTagRequestDAC);
const DRM_CONST_STRING  g_dstrTagDAC                            = CREATE_DRM_STRING(g_rgwchTagDAC);
const DRM_CONST_STRING  g_dstrTagDAC_WMDRM_MD                   = CREATE_DRM_STRING(g_rgwchTagDAC_WMDRM_MD);
const DRM_CONST_STRING  g_dstrTagDAC_PlayReadyND                = CREATE_DRM_STRING(g_rgwchTagDAC_PlayReadyND);
const DRM_CONST_STRING  g_dstrTagDACResponse                    = CREATE_DRM_STRING(g_rgwchTagDACResponse);
const DRM_CONST_STRING  g_dstrTagManufName                      = CREATE_DRM_STRING(g_rgwchTagManufName);
const DRM_CONST_STRING  g_dstrXPathDACPubKey                    = CREATE_DRM_STRING(g_rgwchXPathDACPubKey);
const DRM_CONST_STRING  g_dstrXPathFallbackPubKey               = CREATE_DRM_STRING(g_rgwchXPathFallbackPubKey);
const DRM_CONST_STRING  g_dstrXPathWMDRM_MDPubKey               = CREATE_DRM_STRING(g_rgwchXPathWMDRM_MDPubKey);
const DRM_CONST_STRING  g_dstrXPathPlayReadyNDPubKeyModulus     = CREATE_DRM_STRING(g_rgwchXPathPlayReadyNDPubKeyModulus);
const DRM_CONST_STRING  g_dstrXPathPlayReadyNDPubKeyExponent    = CREATE_DRM_STRING(g_rgwchXPathPlayReadyNDPubKeyExponent);

const DRM_CONST_STRING  g_dstrXPathDACPrivKey       = CREATE_DRM_STRING(g_rgwchXPathDACPrivKey);
const DRM_CONST_STRING  g_dstrXPathFallbackPrivKey  = CREATE_DRM_STRING(g_rgwchXPathFallbackPrivKey);
const DRM_CONST_STRING  g_dstrXPathWMDRMPrivKey     = CREATE_DRM_STRING(g_rgwchXPathWMDRMPrivKey);
const DRM_CONST_STRING  g_dstrXPathWMDRMPubKey      = CREATE_DRM_STRING(g_rgwchXPathWMDRMPubKey);
const DRM_CONST_STRING  g_dstrXPathFallbackSecVer   = CREATE_DRM_STRING(g_rgwchXPathFallbackSecVer);

/* generateDAC */

const DRM_CONST_STRING  g_dstrTagDataSecurityVersionValue = CREATE_DRM_STRING(g_rgwchTagDataSecurityVersionValue);
const DRM_CONST_STRING  g_dstrAuthorizationIDDAC          = CREATE_DRM_STRING(g_rgwchAuthorizationIDDAC);
const DRM_CONST_STRING  g_dstrAuthIDDACRoot               = CREATE_DRM_STRING(g_rgwchAuthIDDACRoot);
const DRM_CONST_STRING  g_dstrTagSecurityLevelValue       = CREATE_DRM_STRING(g_rgwchTagSecurityLevelValue);

/* metering tools */

const DRM_CONST_STRING g_dstrCertMetercert                = CREATE_DRM_STRING(g_rgwchCertMetercert);

/* secure clock */

const DRM_CONST_STRING g_dstrCertSecureClock        = CREATE_DRM_STRING(g_rgwchCertSecureClock);

/* XML utils */

const DRM_CONST_STRING  g_dstrTestTemplate          = CREATE_DRM_STRING(g_rgwchTestTemplate);
const DRM_CONST_STRING  g_dstrXMLTree1              = CREATE_DRM_STRING(g_rgwchXMLTree1);
const DRM_CONST_STRING  g_dstrXMLTree2              = CREATE_DRM_STRING(g_rgwchXMLTree2);
const DRM_CONST_STRING  g_dstrXMLTree3              = CREATE_DRM_STRING(g_rgwchXMLTree3);
const DRM_CONST_STRING  g_dstrXMLTree4              = CREATE_DRM_STRING(g_rgwchXMLTree4);

/* package tools */

const DRM_CONST_STRING  g_dstrSigningKey            = CREATE_DRM_STRING(g_rgwchSigningKey);
const DRM_CONST_STRING  g_dstrTagRoot               = CREATE_DRM_STRING(g_rgwchRootTag);
const DRM_CONST_STRING  g_dstrTagKeySeed            = CREATE_DRM_STRING(g_rgwchKeySeedTag);
const DRM_CONST_STRING  g_dstrTagLicAcqInfo         = CREATE_DRM_STRING(g_rgwchLaInfoTag);
const DRM_CONST_STRING  g_dstrTagCustomData         = CREATE_DRM_STRING(g_rgwchCustomData);
const DRM_CONST_STRING  g_dstrEncBlkSize            = CREATE_DRM_STRING(g_rgwchEncBlkSize);
const DRM_CONST_STRING  g_dstrCID                   = CREATE_DRM_STRING(g_rgwchCID);
const DRM_CONST_STRING  g_dstrVerifyKey             = CREATE_DRM_STRING(g_rgwchVerifyKey);
const DRM_CONST_STRING  g_dstrDefaultURL            = CREATE_DRM_STRING(g_rgwchDefaultURL);
const DRM_CONST_STRING  g_dstrDefaultBlksize        = CREATE_DRM_STRING(g_rgwchDefaultBlksize);
const DRM_CONST_STRING  g_dstrDefaultFilename       = CREATE_DRM_STRING(g_rgwchDefaultFilename);
const DRM_CONST_STRING  g_dstrContents              = CREATE_DRM_STRING(g_rgwchContents);
const DRM_CONST_STRING  g_dstrSerial                = CREATE_DRM_STRING(g_rgwchSerial);
const DRM_CONST_STRING  g_dstrTagPrivkey            = CREATE_DRM_STRING(g_rgwchTagPrivkey);
const DRM_CONST_STRING  g_dstrTagKeypair            = CREATE_DRM_STRING(g_rgwchTagKeypair);
const DRM_CONST_STRING  g_dstrTagExpression         = CREATE_DRM_STRING(g_rgwchTagExpression);

/* PlayReady Certificate Generator */
const DRM_WCHAR g_rgwchVersion                           [] = { ONE_WCHAR('v', '\0'), ONE_WCHAR('e', '\0'), ONE_WCHAR('r', '\0'), ONE_WCHAR('s', '\0'), ONE_WCHAR('i', '\0'), ONE_WCHAR('o', '\0'), ONE_WCHAR('n', '\0'), ONE_WCHAR('\0', '\0')}; /* "version" */
const DRM_WCHAR g_rgwchCertType                          [] = { ONE_WCHAR('t', '\0'), ONE_WCHAR('y', '\0'), ONE_WCHAR('p', '\0'), ONE_WCHAR('e', '\0'), ONE_WCHAR('\0', '\0')}; /* "type" */
const DRM_WCHAR g_rgwchValue                             [] = { ONE_WCHAR('v', '\0'), ONE_WCHAR('a', '\0'), ONE_WCHAR('l', '\0'), ONE_WCHAR('u', '\0'), ONE_WCHAR('e', '\0'), ONE_WCHAR('\0', '\0')}; /* "value" */
const DRM_WCHAR g_rgwchDevice                            [] = { ONE_WCHAR('d', '\0'), ONE_WCHAR('e', '\0'), ONE_WCHAR('v', '\0'), ONE_WCHAR('i', '\0'), ONE_WCHAR('c', '\0'), ONE_WCHAR('e', '\0'), ONE_WCHAR('\0', '\0')}; /* "device" */
const DRM_WCHAR g_rgwchPC                                [] = { ONE_WCHAR('p', '\0'), ONE_WCHAR('c', '\0'), ONE_WCHAR('\0', '\0')}; /* "pc" */
const DRM_WCHAR  g_rgwchExtendedData                     [] = { ONE_WCHAR('e', '\0'), ONE_WCHAR('x', '\0'), ONE_WCHAR('t', '\0'), ONE_WCHAR('e', '\0'), ONE_WCHAR('n', '\0'), ONE_WCHAR('d', '\0'), ONE_WCHAR('e', '\0'), ONE_WCHAR('d', '\0'), ONE_WCHAR('d', '\0'), ONE_WCHAR('a', '\0'), ONE_WCHAR('t', '\0'), ONE_WCHAR('a', '\0'), ONE_WCHAR('\0', '\0')}; /* "extendeddata" */
const DRM_WCHAR  g_rgwchExtendedDataSigningKey           [] = { ONE_WCHAR('s', '\0'), ONE_WCHAR('i', '\0'), ONE_WCHAR('g', '\0'), ONE_WCHAR('n', '\0'), ONE_WCHAR('i', '\0'), ONE_WCHAR('n', '\0'), ONE_WCHAR('g', '\0'), ONE_WCHAR('k', '\0'), ONE_WCHAR('e', '\0'), ONE_WCHAR('y', '\0'), ONE_WCHAR('\0', '\0')}; /* "signingkey" */
const DRM_WCHAR  g_rgwchExtendedDataSigningKeyPublicKey  [] = { ONE_WCHAR('p', '\0'), ONE_WCHAR('u', '\0'), ONE_WCHAR('b', '\0'), ONE_WCHAR('l', '\0'), ONE_WCHAR('i', '\0'), ONE_WCHAR('c', '\0'), ONE_WCHAR('k', '\0'), ONE_WCHAR('e', '\0'), ONE_WCHAR('y', '\0'), ONE_WCHAR('\0', '\0')}; /* "publickey" */
const DRM_WCHAR  g_rgwchExtendedDataSigningKeyPrivateKey [] = { ONE_WCHAR('p', '\0'), ONE_WCHAR('r', '\0'), ONE_WCHAR('i', '\0'), ONE_WCHAR('v', '\0'), ONE_WCHAR('a', '\0'), ONE_WCHAR('t', '\0'), ONE_WCHAR('e', '\0'), ONE_WCHAR('k', '\0'), ONE_WCHAR('e', '\0'), ONE_WCHAR('y', '\0'), ONE_WCHAR('\0', '\0')}; /* "privatekey" */
const DRM_WCHAR  g_rgwchExtendedDataRecord               [] = { ONE_WCHAR('d', '\0'), ONE_WCHAR('a', '\0'), ONE_WCHAR('t', '\0'), ONE_WCHAR('a', '\0'), ONE_WCHAR('r', '\0'), ONE_WCHAR('e', '\0'), ONE_WCHAR('c', '\0'), ONE_WCHAR('o', '\0'), ONE_WCHAR('r', '\0'), ONE_WCHAR('d', '\0'), ONE_WCHAR('\0', '\0')}; /* "datarecord" */
const DRM_WCHAR g_rgwchDomain                            [] = { ONE_WCHAR('d', '\0'), ONE_WCHAR('o', '\0'), ONE_WCHAR('m', '\0'), ONE_WCHAR('a', '\0'), ONE_WCHAR('i', '\0'), ONE_WCHAR('n', '\0'), ONE_WCHAR('\0', '\0')}; /* "domain" */
const DRM_WCHAR g_rgwchSilverLight                       [] = { ONE_WCHAR('s', '\0'), ONE_WCHAR('i', '\0'), ONE_WCHAR('l', '\0'), ONE_WCHAR('v', '\0'), ONE_WCHAR('e', '\0'), ONE_WCHAR('r', '\0'), ONE_WCHAR('l', '\0'), ONE_WCHAR('i', '\0'), ONE_WCHAR('g', '\0'), ONE_WCHAR('h', '\0'), ONE_WCHAR('t', '\0'), ONE_WCHAR('\0', '\0')}; /* "silverlight" */
const DRM_WCHAR g_rgwchApplication                       [] = { ONE_WCHAR('a', '\0'), ONE_WCHAR('p', '\0'), ONE_WCHAR('p', '\0'), ONE_WCHAR('l', '\0'), ONE_WCHAR('i', '\0'), ONE_WCHAR('c', '\0'), ONE_WCHAR('a', '\0'), ONE_WCHAR('t', '\0'), ONE_WCHAR('i', '\0'), ONE_WCHAR('o', '\0'), ONE_WCHAR('n', '\0'), ONE_WCHAR('\0', '\0')}; /* "application" */
const DRM_WCHAR g_rgwchMeteringCertType                  [] = { ONE_WCHAR('m', '\0'), ONE_WCHAR('e', '\0'), ONE_WCHAR('t', '\0'), ONE_WCHAR('e', '\0'), ONE_WCHAR('r', '\0'), ONE_WCHAR('i', '\0'), ONE_WCHAR('n', '\0'), ONE_WCHAR('g', '\0'), ONE_WCHAR('\0', '\0')}; /* "metering" */
const DRM_WCHAR g_rgwchServerCertType                    [] = { ONE_WCHAR('s', '\0'), ONE_WCHAR('e', '\0'), ONE_WCHAR('r', '\0'), ONE_WCHAR('v', '\0'), ONE_WCHAR('e', '\0'), ONE_WCHAR('r', '\0'), ONE_WCHAR('\0', '\0')}; /* "server" */
const DRM_WCHAR g_rgwchIssuer                            [] = { ONE_WCHAR('i', '\0'), ONE_WCHAR('s', '\0'), ONE_WCHAR('s', '\0'), ONE_WCHAR('u', '\0'), ONE_WCHAR('e', '\0'), ONE_WCHAR('r', '\0'), ONE_WCHAR('\0', '\0')}; /* "issuer" */
const DRM_WCHAR g_rgwchService                           [] = { ONE_WCHAR('s', '\0'), ONE_WCHAR('e', '\0'), ONE_WCHAR('r', '\0'), ONE_WCHAR('v', '\0'), ONE_WCHAR('i', '\0'), ONE_WCHAR('c', '\0'), ONE_WCHAR('e', '\0'), ONE_WCHAR('\0', '\0')}; /* "service" */
const DRM_WCHAR g_rgwchCRLSigner                         [] = { ONE_WCHAR('c', '\0'), ONE_WCHAR('r', '\0'), ONE_WCHAR('l', '\0'), ONE_WCHAR('s', '\0'), ONE_WCHAR('i', '\0'), ONE_WCHAR('g', '\0'), ONE_WCHAR('n', '\0'), ONE_WCHAR('e', '\0'), ONE_WCHAR('r', '\0'), ONE_WCHAR('\0', '\0')}; /* "crlsigner" */
const DRM_WCHAR g_rgwchKeyFileSigner                     [] = { ONE_WCHAR('k', '\0'), ONE_WCHAR('e', '\0'), ONE_WCHAR('y', '\0'), ONE_WCHAR('f', '\0'), ONE_WCHAR('i', '\0'), ONE_WCHAR('l', '\0'), ONE_WCHAR('e', '\0'), ONE_WCHAR('s', '\0'), ONE_WCHAR('i', '\0'), ONE_WCHAR('g', '\0'), ONE_WCHAR('n', '\0'), ONE_WCHAR('e', '\0'), ONE_WCHAR('r', '\0'), ONE_WCHAR('\0', '\0')}; /* "keyfilesigner" */
const DRM_WCHAR g_rgwchLicenseSigner                     [] = { ONE_WCHAR('l', '\0'), ONE_WCHAR('i', '\0'), ONE_WCHAR('c', '\0'), ONE_WCHAR('e', '\0'), ONE_WCHAR('n', '\0'), ONE_WCHAR('s', '\0'), ONE_WCHAR('e', '\0'), ONE_WCHAR('s', '\0'), ONE_WCHAR('i', '\0'), ONE_WCHAR('g', '\0'), ONE_WCHAR('n', '\0'), ONE_WCHAR('e', '\0'), ONE_WCHAR('r', '\0'), ONE_WCHAR('\0', '\0')}; /* "licensesigner" */
const DRM_WCHAR g_rgwchBasicInfo                         [] = { ONE_WCHAR('b', '\0'), ONE_WCHAR('a', '\0'), ONE_WCHAR('s', '\0'), ONE_WCHAR('i', '\0'), ONE_WCHAR('c', '\0'), ONE_WCHAR('i', '\0'), ONE_WCHAR('n', '\0'), ONE_WCHAR('f', '\0'), ONE_WCHAR('o', '\0'), ONE_WCHAR('\0', '\0')}; /* "basicinfo" */
const DRM_WCHAR g_rgwchSecurityLevel                     [] = { ONE_WCHAR('s', '\0'), ONE_WCHAR('e', '\0'), ONE_WCHAR('c', '\0'), ONE_WCHAR('u', '\0'), ONE_WCHAR('r', '\0'), ONE_WCHAR('i', '\0'), ONE_WCHAR('t', '\0'), ONE_WCHAR('y', '\0'), ONE_WCHAR('l', '\0'), ONE_WCHAR('e', '\0'), ONE_WCHAR('v', '\0'), ONE_WCHAR('e', '\0'), ONE_WCHAR('l', '\0'), ONE_WCHAR('\0', '\0')}; /* "securitylevel" */
const DRM_WCHAR g_rgwchClientID                          [] = { ONE_WCHAR('c', '\0'), ONE_WCHAR('l', '\0'), ONE_WCHAR('i', '\0'), ONE_WCHAR('e', '\0'), ONE_WCHAR('n', '\0'), ONE_WCHAR('t', '\0'), ONE_WCHAR('i', '\0'), ONE_WCHAR('d', '\0'), ONE_WCHAR('\0', '\0')}; /* "clientid" */
const DRM_WCHAR g_rgwchExpiration                        [] = { ONE_WCHAR('e', '\0'), ONE_WCHAR('x', '\0'), ONE_WCHAR('p', '\0'), ONE_WCHAR('i', '\0'), ONE_WCHAR('r', '\0'), ONE_WCHAR('a', '\0'), ONE_WCHAR('t', '\0'), ONE_WCHAR('i', '\0'), ONE_WCHAR('o', '\0'), ONE_WCHAR('n', '\0'), ONE_WCHAR('\0', '\0')}; /* "expiration" */
const DRM_WCHAR g_rgwchWarningDays                       [] = { ONE_WCHAR('W', '\0'), ONE_WCHAR('a', '\0'), ONE_WCHAR('r', '\0'), ONE_WCHAR('n', '\0'), ONE_WCHAR('i', '\0'), ONE_WCHAR('n', '\0'), ONE_WCHAR('g', '\0'), ONE_WCHAR('D', '\0'), ONE_WCHAR('a', '\0'), ONE_WCHAR('y', '\0'), ONE_WCHAR('s', '\0'), ONE_WCHAR('\0', '\0')}; /* "WarningDays" */
const DRM_WCHAR g_rgwchServiceID                         [] = { ONE_WCHAR('s', '\0'), ONE_WCHAR('e', '\0'), ONE_WCHAR('r', '\0'), ONE_WCHAR('v', '\0'), ONE_WCHAR('i', '\0'), ONE_WCHAR('c', '\0'), ONE_WCHAR('e', '\0'), ONE_WCHAR('i', '\0'), ONE_WCHAR('d', '\0'), ONE_WCHAR('\0', '\0')}; /* "serviceid" */
const DRM_WCHAR g_rgwchAccountID                         [] = { ONE_WCHAR('a', '\0'), ONE_WCHAR('c', '\0'), ONE_WCHAR('c', '\0'), ONE_WCHAR('o', '\0'), ONE_WCHAR('u', '\0'), ONE_WCHAR('n', '\0'), ONE_WCHAR('t', '\0'), ONE_WCHAR('i', '\0'), ONE_WCHAR('d', '\0'), ONE_WCHAR('\0', '\0')}; /* "accountid" */
const DRM_WCHAR g_rgwchRevision                          [] = { ONE_WCHAR('r', '\0'), ONE_WCHAR('e', '\0'), ONE_WCHAR('v', '\0'), ONE_WCHAR('i', '\0'), ONE_WCHAR('s', '\0'), ONE_WCHAR('i', '\0'), ONE_WCHAR('o', '\0'), ONE_WCHAR('n', '\0'), ONE_WCHAR('\0', '\0')}; /* "revision" */
const DRM_WCHAR g_rgwchUrl                               [] = { ONE_WCHAR('u', '\0'), ONE_WCHAR('r', '\0'), ONE_WCHAR('l', '\0'), ONE_WCHAR('\0', '\0')}; /* "url" */
const DRM_WCHAR g_rgwchSecurityVersion                   [] = { ONE_WCHAR('s', '\0'), ONE_WCHAR('e', '\0'), ONE_WCHAR('c', '\0'), ONE_WCHAR('u', '\0'), ONE_WCHAR('r', '\0'), ONE_WCHAR('i', '\0'), ONE_WCHAR('t', '\0'), ONE_WCHAR('y', '\0'), ONE_WCHAR('v', '\0'), ONE_WCHAR('e', '\0'), ONE_WCHAR('r', '\0'), ONE_WCHAR('s', '\0'), ONE_WCHAR('i', '\0'), ONE_WCHAR('o', '\0'), ONE_WCHAR('n', '\0'), ONE_WCHAR('\0', '\0')}; /* "securityversion" */
const DRM_WCHAR g_rgwchExtendedDataRecordFlags           [] = { ONE_WCHAR('f', '\0'), ONE_WCHAR('l', '\0'), ONE_WCHAR('a', '\0'), ONE_WCHAR('g', '\0'), ONE_WCHAR('s', '\0'), ONE_WCHAR('\0', '\0')}; /* "flags" */
const DRM_WCHAR g_rgwchExtendedDataRecordMaxLength       [] = { ONE_WCHAR('m', '\0'), ONE_WCHAR('a', '\0'), ONE_WCHAR('x', '\0'), ONE_WCHAR('L', '\0'), ONE_WCHAR('e', '\0'), ONE_WCHAR('n', '\0'), ONE_WCHAR('g', '\0'), ONE_WCHAR('t', '\0'), ONE_WCHAR('h', '\0'), ONE_WCHAR('B', '\0'), ONE_WCHAR('y', '\0'), ONE_WCHAR('t', '\0'), ONE_WCHAR('e', '\0'), ONE_WCHAR('s', '\0'), ONE_WCHAR('\0', '\0')}; /* "maxLengthBytes" */
const DRM_WCHAR g_rgwchSecurityVersionSeperator          [] = { ONE_WCHAR('.', '\0'), ONE_WCHAR('\0', '\0')}; /* "." */
const DRM_WCHAR g_rgwchTransmitter                       [] = { ONE_WCHAR('T', '\0'), ONE_WCHAR('r', '\0'), ONE_WCHAR('a', '\0'), ONE_WCHAR('n', '\0'), ONE_WCHAR('s', '\0'), ONE_WCHAR('m', '\0'), ONE_WCHAR('i', '\0'), ONE_WCHAR('t', '\0'), ONE_WCHAR('t', '\0'), ONE_WCHAR('e', '\0'), ONE_WCHAR('r', '\0'), ONE_WCHAR('\0', '\0')}; /* "Transmitter" */
const DRM_WCHAR g_rgwchReceiver                          [] = { ONE_WCHAR('R', '\0'), ONE_WCHAR('e', '\0'), ONE_WCHAR('c', '\0'), ONE_WCHAR('e', '\0'), ONE_WCHAR('i', '\0'), ONE_WCHAR('v', '\0'), ONE_WCHAR('e', '\0'), ONE_WCHAR('r', '\0'), ONE_WCHAR('\0', '\0')}; /* "Receiver" */
const DRM_WCHAR g_rgwchSharedCertificate                 [] = { ONE_WCHAR('S', '\0'), ONE_WCHAR('h', '\0'), ONE_WCHAR('a', '\0'), ONE_WCHAR('r', '\0'), ONE_WCHAR('e', '\0'), ONE_WCHAR('d', '\0'), ONE_WCHAR('C', '\0'), ONE_WCHAR('e', '\0'), ONE_WCHAR('r', '\0'), ONE_WCHAR('t', '\0'), ONE_WCHAR('i', '\0'), ONE_WCHAR('f', '\0'), ONE_WCHAR('i', '\0'), ONE_WCHAR('c', '\0'), ONE_WCHAR('a', '\0'), ONE_WCHAR('t', '\0'), ONE_WCHAR('e', '\0'), ONE_WCHAR('\0', '\0')}; /* "SharedCertificate" */
const DRM_WCHAR g_rgwchSecureClock                       [] = { ONE_WCHAR('S', '\0'), ONE_WCHAR('e', '\0'), ONE_WCHAR('c', '\0'), ONE_WCHAR('u', '\0'), ONE_WCHAR('r', '\0'), ONE_WCHAR('e', '\0'), ONE_WCHAR('C', '\0'), ONE_WCHAR('l', '\0'), ONE_WCHAR('o', '\0'), ONE_WCHAR('c', '\0'), ONE_WCHAR('k', '\0'), ONE_WCHAR('\0', '\0')}; /* "SecureClock" */
const DRM_WCHAR g_rgwchSampleProtection                  [] = { ONE_WCHAR('E', '\0'), ONE_WCHAR('n', '\0'), ONE_WCHAR('c', '\0'), ONE_WCHAR('r', '\0'), ONE_WCHAR('y', '\0'), ONE_WCHAR('p', '\0'), ONE_WCHAR('t', '\0'), ONE_WCHAR('K', '\0'), ONE_WCHAR('e', '\0'), ONE_WCHAR('y', '\0'), ONE_WCHAR('S', '\0'), ONE_WCHAR('a', '\0'), ONE_WCHAR('m', '\0'), ONE_WCHAR('p', '\0'), ONE_WCHAR('l', '\0'), ONE_WCHAR('e', '\0'), ONE_WCHAR('P', '\0'), ONE_WCHAR('r', '\0'), ONE_WCHAR('o', '\0'), ONE_WCHAR('t', '\0'), ONE_WCHAR('e', '\0'), ONE_WCHAR('c', '\0'), ONE_WCHAR('t', '\0'), ONE_WCHAR('i', '\0'), ONE_WCHAR('o', '\0'), ONE_WCHAR('n', '\0'), ONE_WCHAR('\0', '\0')}; /* "EncryptKeySampleProtection" */
const DRM_WCHAR g_rgwchRollbackClock                     [] = { ONE_WCHAR('A', '\0'), ONE_WCHAR('n', '\0'), ONE_WCHAR('t', '\0'), ONE_WCHAR('i', '\0'), ONE_WCHAR('r', '\0'), ONE_WCHAR('o', '\0'), ONE_WCHAR('l', '\0'), ONE_WCHAR('l', '\0'), ONE_WCHAR('b', '\0'), ONE_WCHAR('a', '\0'), ONE_WCHAR('c', '\0'), ONE_WCHAR('k', '\0'), ONE_WCHAR('C', '\0'), ONE_WCHAR('l', '\0'), ONE_WCHAR('o', '\0'), ONE_WCHAR('c', '\0'), ONE_WCHAR('k', '\0'), ONE_WCHAR('\0', '\0')}; /* "AntirollbackClock" */
const DRM_WCHAR g_rgwchMetering                          [] = { ONE_WCHAR('M', '\0'), ONE_WCHAR('e', '\0'), ONE_WCHAR('t', '\0'), ONE_WCHAR('e', '\0'), ONE_WCHAR('r', '\0'), ONE_WCHAR('i', '\0'), ONE_WCHAR('n', '\0'), ONE_WCHAR('g', '\0'), ONE_WCHAR('\0', '\0')}; /* "Metering" */
const DRM_WCHAR g_rgwchLicenseSync                       [] = { ONE_WCHAR('L', '\0'), ONE_WCHAR('i', '\0'), ONE_WCHAR('c', '\0'), ONE_WCHAR('e', '\0'), ONE_WCHAR('n', '\0'), ONE_WCHAR('s', '\0'), ONE_WCHAR('e', '\0'), ONE_WCHAR('S', '\0'), ONE_WCHAR('y', '\0'), ONE_WCHAR('n', '\0'), ONE_WCHAR('c', '\0'), ONE_WCHAR('\0', '\0')}; /* "LicenseSync" */
const DRM_WCHAR g_rgwchSymmOpt                           [] = { ONE_WCHAR('S', '\0'), ONE_WCHAR('y', '\0'), ONE_WCHAR('m', '\0'), ONE_WCHAR('m', '\0'), ONE_WCHAR('O', '\0'), ONE_WCHAR('p', '\0'), ONE_WCHAR('t', '\0'), ONE_WCHAR('\0', '\0')}; /* "SymmOpt" */
const DRM_WCHAR g_rgwchSupportCRL                        [] = { ONE_WCHAR('S', '\0'), ONE_WCHAR('u', '\0'), ONE_WCHAR('p', '\0'), ONE_WCHAR('p', '\0'), ONE_WCHAR('o', '\0'), ONE_WCHAR('r', '\0'), ONE_WCHAR('t', '\0'), ONE_WCHAR('C', '\0'), ONE_WCHAR('R', '\0'), ONE_WCHAR('L', '\0'), ONE_WCHAR('\0', '\0')}; /* "SupportCRL" */

/* Server SKUification feature flags */
const DRM_WCHAR g_rgwchServerBasicEdition                [] = { ONE_WCHAR('S', '\0'), ONE_WCHAR('e', '\0'), ONE_WCHAR('r', '\0'), ONE_WCHAR('v', '\0'), ONE_WCHAR('e', '\0'), ONE_WCHAR('r', '\0'), ONE_WCHAR('B', '\0'), ONE_WCHAR('a', '\0'), ONE_WCHAR('s', '\0'), ONE_WCHAR('i', '\0'), ONE_WCHAR('c', '\0'), ONE_WCHAR('E', '\0'), ONE_WCHAR('d', '\0'), ONE_WCHAR('i', '\0'), ONE_WCHAR('t', '\0'), ONE_WCHAR('i', '\0'), ONE_WCHAR('o', '\0'), ONE_WCHAR('n', '\0'), ONE_WCHAR('\0', '\0')}; /* "ServerBasicEdition" */
const DRM_WCHAR g_rgwchServerStandardEdition             [] = { ONE_WCHAR('S', '\0'), ONE_WCHAR('e', '\0'), ONE_WCHAR('r', '\0'), ONE_WCHAR('v', '\0'), ONE_WCHAR('e', '\0'), ONE_WCHAR('r', '\0'), ONE_WCHAR('S', '\0'), ONE_WCHAR('t', '\0'), ONE_WCHAR('a', '\0'), ONE_WCHAR('n', '\0'), ONE_WCHAR('d', '\0'), ONE_WCHAR('a', '\0'), ONE_WCHAR('r', '\0'), ONE_WCHAR('d', '\0'), ONE_WCHAR('E', '\0'), ONE_WCHAR('d', '\0'), ONE_WCHAR('i', '\0'), ONE_WCHAR('t', '\0'), ONE_WCHAR('i', '\0'), ONE_WCHAR('o', '\0'), ONE_WCHAR('n', '\0'), ONE_WCHAR('\0', '\0')}; /* "ServerStandardEdition" */
const DRM_WCHAR g_rgwchServerPremiumEdition              [] = { ONE_WCHAR('S', '\0'), ONE_WCHAR('e', '\0'), ONE_WCHAR('r', '\0'), ONE_WCHAR('v', '\0'), ONE_WCHAR('e', '\0'), ONE_WCHAR('r', '\0'), ONE_WCHAR('P', '\0'), ONE_WCHAR('r', '\0'), ONE_WCHAR('e', '\0'), ONE_WCHAR('m', '\0'), ONE_WCHAR('i', '\0'), ONE_WCHAR('u', '\0'), ONE_WCHAR('m', '\0'), ONE_WCHAR('E', '\0'), ONE_WCHAR('d', '\0'), ONE_WCHAR('i', '\0'), ONE_WCHAR('t', '\0'), ONE_WCHAR('i', '\0'), ONE_WCHAR('o', '\0'), ONE_WCHAR('n', '\0'), ONE_WCHAR('\0', '\0')}; /* "ServerPremiumEdition" */

const DRM_WCHAR g_rgwchCertFeatures                      [] = { ONE_WCHAR('F', '\0'), ONE_WCHAR('E', '\0'), ONE_WCHAR('A', '\0'), ONE_WCHAR('T', '\0'), ONE_WCHAR('U', '\0'), ONE_WCHAR('R', '\0'), ONE_WCHAR('E', '\0'), ONE_WCHAR('S', '\0'), ONE_WCHAR('\0', '\0')}; /* "FEATURES" */
const DRM_WCHAR g_rgwchCertManufacturer                  [] = { ONE_WCHAR('m', '\0'), ONE_WCHAR('a', '\0'), ONE_WCHAR('n', '\0'), ONE_WCHAR('u', '\0'), ONE_WCHAR('f', '\0'), ONE_WCHAR('a', '\0'), ONE_WCHAR('c', '\0'), ONE_WCHAR('t', '\0'), ONE_WCHAR('u', '\0'), ONE_WCHAR('r', '\0'), ONE_WCHAR('e', '\0'), ONE_WCHAR('r', '\0'), ONE_WCHAR('\0', '\0')}; /* "manufacturer" */
const DRM_WCHAR g_rgwchManufacturerName                  [] = { ONE_WCHAR('n', '\0'), ONE_WCHAR('a', '\0'), ONE_WCHAR('m', '\0'), ONE_WCHAR('e', '\0'), ONE_WCHAR('\0', '\0')}; /* "name" */
const DRM_WCHAR g_rgwchMake                              [] = { ONE_WCHAR('M', '\0'), ONE_WCHAR('A', '\0'), ONE_WCHAR('K', '\0'), ONE_WCHAR('E', '\0'), ONE_WCHAR('\0', '\0')}; /* "MAKE" */
const DRM_WCHAR g_rgwchModelName                         [] = { ONE_WCHAR('m', '\0'), ONE_WCHAR('o', '\0'), ONE_WCHAR('d', '\0'), ONE_WCHAR('e', '\0'), ONE_WCHAR('l', '\0'), ONE_WCHAR('n', '\0'), ONE_WCHAR('a', '\0'), ONE_WCHAR('m', '\0'), ONE_WCHAR('e', '\0'), ONE_WCHAR('\0', '\0')}; /* "modelname" */
const DRM_WCHAR g_rgwchModelNumber                       [] = { ONE_WCHAR('m', '\0'), ONE_WCHAR('o', '\0'), ONE_WCHAR('d', '\0'), ONE_WCHAR('e', '\0'), ONE_WCHAR('l', '\0'), ONE_WCHAR('n', '\0'), ONE_WCHAR('u', '\0'), ONE_WCHAR('m', '\0'), ONE_WCHAR('b', '\0'), ONE_WCHAR('e', '\0'), ONE_WCHAR('r', '\0'), ONE_WCHAR('\0', '\0')}; /* "modelnumber" */
const DRM_WCHAR g_rgwchLimits                            [] = { ONE_WCHAR('L', '\0'), ONE_WCHAR('I', '\0'), ONE_WCHAR('M', '\0'), ONE_WCHAR('I', '\0'), ONE_WCHAR('T', '\0'), ONE_WCHAR('S', '\0'), ONE_WCHAR('\0', '\0')}; /* "LIMITS" */
const DRM_WCHAR g_rgwchCertKey                           [] = { ONE_WCHAR('c', '\0'), ONE_WCHAR('e', '\0'), ONE_WCHAR('r', '\0'), ONE_WCHAR('t', '\0'), ONE_WCHAR('k', '\0'), ONE_WCHAR('e', '\0'), ONE_WCHAR('y', '\0'), ONE_WCHAR('\0', '\0')}; /* "certkey" */
const DRM_WCHAR g_rgwchCertKeyType                       [] = { ONE_WCHAR('t', '\0'), ONE_WCHAR('y', '\0'), ONE_WCHAR('p', '\0'), ONE_WCHAR('e', '\0'), ONE_WCHAR('\0', '\0')}; /* "type" */
const DRM_WCHAR g_rgwchCertKeys                          [] = { ONE_WCHAR('c', '\0'), ONE_WCHAR('e', '\0'), ONE_WCHAR('r', '\0'), ONE_WCHAR('t', '\0'), ONE_WCHAR('k', '\0'), ONE_WCHAR('e', '\0'), ONE_WCHAR('y', '\0'), ONE_WCHAR('s', '\0'), ONE_WCHAR('\0', '\0')}; /* "certkeys" */
const DRM_WCHAR g_rgwchCertECC256                        [] = { ONE_WCHAR('E', '\0'), ONE_WCHAR('C', '\0'), ONE_WCHAR('C', '\0'), ONE_WCHAR('-', '\0'), ONE_WCHAR('2', '\0'), ONE_WCHAR('5', '\0'), ONE_WCHAR('6', '\0'), ONE_WCHAR('\0', '\0')}; /* "ECC-256" */
const DRM_WCHAR g_rgwchCertKeyUsages                     [] = { ONE_WCHAR('k', '\0'), ONE_WCHAR('e', '\0'), ONE_WCHAR('y', '\0'), ONE_WCHAR('u', '\0'), ONE_WCHAR('s', '\0'), ONE_WCHAR('a', '\0'), ONE_WCHAR('g', '\0'), ONE_WCHAR('e', '\0'), ONE_WCHAR('s', '\0'), ONE_WCHAR('\0', '\0')}; /* "keyusages" */
const DRM_WCHAR g_rgwchCertKeyUsageSign                  [] = { ONE_WCHAR('S', '\0'), ONE_WCHAR('i', '\0'), ONE_WCHAR('g', '\0'), ONE_WCHAR('n', '\0'), ONE_WCHAR('\0', '\0')}; /* "Sign" */
const DRM_WCHAR g_rgwchCertKeyUsageEncryptKey            [] = { ONE_WCHAR('E', '\0'), ONE_WCHAR('n', '\0'), ONE_WCHAR('c', '\0'), ONE_WCHAR('r', '\0'), ONE_WCHAR('y', '\0'), ONE_WCHAR('p', '\0'), ONE_WCHAR('t', '\0'), ONE_WCHAR('K', '\0'), ONE_WCHAR('e', '\0'), ONE_WCHAR('y', '\0'), ONE_WCHAR('\0', '\0')}; /* "EncryptKey" */
const DRM_WCHAR g_rgwchCertKeyUsageSignCRL               [] = { ONE_WCHAR('S', '\0'), ONE_WCHAR('i', '\0'), ONE_WCHAR('g', '\0'), ONE_WCHAR('n', '\0'), ONE_WCHAR('C', '\0'), ONE_WCHAR('R', '\0'), ONE_WCHAR('L', '\0'), ONE_WCHAR('\0', '\0')}; /* "SignCRL" */
const DRM_WCHAR g_rgwchCertKeyUsageIssuerAll             [] = { ONE_WCHAR('I', '\0'), ONE_WCHAR('s', '\0'), ONE_WCHAR('s', '\0'), ONE_WCHAR('u', '\0'), ONE_WCHAR('e', '\0'), ONE_WCHAR('r', '\0'), ONE_WCHAR('A', '\0'), ONE_WCHAR('l', '\0'), ONE_WCHAR('l', '\0'), ONE_WCHAR('\0', '\0')}; /* "IssuerAll" */
const DRM_WCHAR g_rgwchCertKeyUsageIssuerIndiv           [] = { ONE_WCHAR('I', '\0'), ONE_WCHAR('s', '\0'), ONE_WCHAR('s', '\0'), ONE_WCHAR('u', '\0'), ONE_WCHAR('e', '\0'), ONE_WCHAR('r', '\0'), ONE_WCHAR('I', '\0'), ONE_WCHAR('n', '\0'), ONE_WCHAR('d', '\0'), ONE_WCHAR('i', '\0'), ONE_WCHAR('v', '\0'), ONE_WCHAR('\0', '\0')}; /* "IssuerIndiv" */
const DRM_WCHAR g_rgwchCertKeyUsageIssuerDevice          [] = { ONE_WCHAR('I', '\0'), ONE_WCHAR('s', '\0'), ONE_WCHAR('s', '\0'), ONE_WCHAR('u', '\0'), ONE_WCHAR('e', '\0'), ONE_WCHAR('r', '\0'), ONE_WCHAR('D', '\0'), ONE_WCHAR('e', '\0'), ONE_WCHAR('v', '\0'), ONE_WCHAR('i', '\0'), ONE_WCHAR('c', '\0'), ONE_WCHAR('e', '\0'), ONE_WCHAR('\0', '\0')}; /* "IssuerDevice" */
const DRM_WCHAR g_rgwchCertKeyUsageIssuerLink            [] = { ONE_WCHAR('I', '\0'), ONE_WCHAR('s', '\0'), ONE_WCHAR('s', '\0'), ONE_WCHAR('u', '\0'), ONE_WCHAR('e', '\0'), ONE_WCHAR('r', '\0'), ONE_WCHAR('L', '\0'), ONE_WCHAR('i', '\0'), ONE_WCHAR('n', '\0'), ONE_WCHAR('k', '\0'), ONE_WCHAR('\0', '\0')}; /* "IssuerLink" */
const DRM_WCHAR g_rgwchCertKeyUsageIssuerDomain          [] = { ONE_WCHAR('I', '\0'), ONE_WCHAR('s', '\0'), ONE_WCHAR('s', '\0'), ONE_WCHAR('u', '\0'), ONE_WCHAR('e', '\0'), ONE_WCHAR('r', '\0'), ONE_WCHAR('D', '\0'), ONE_WCHAR('o', '\0'), ONE_WCHAR('m', '\0'), ONE_WCHAR('a', '\0'), ONE_WCHAR('i', '\0'), ONE_WCHAR('n', '\0'), ONE_WCHAR('\0', '\0')}; /* "IssuerDomain" */
const DRM_WCHAR g_rgwchCertKeyUsageIssuerSilverLight     [] = { ONE_WCHAR('I', '\0'), ONE_WCHAR('s', '\0'), ONE_WCHAR('s', '\0'), ONE_WCHAR('u', '\0'), ONE_WCHAR('e', '\0'), ONE_WCHAR('r', '\0'), ONE_WCHAR('S', '\0'), ONE_WCHAR('i', '\0'), ONE_WCHAR('l', '\0'), ONE_WCHAR('v', '\0'), ONE_WCHAR('e', '\0'), ONE_WCHAR('r', '\0'), ONE_WCHAR('L', '\0'), ONE_WCHAR('i', '\0'), ONE_WCHAR('g', '\0'), ONE_WCHAR('h', '\0'), ONE_WCHAR('t', '\0'), ONE_WCHAR('\0', '\0')}; /* "IssuerSilverLight" */
const DRM_WCHAR g_rgwchCertKeyUsageIssuerApplication     [] = { ONE_WCHAR('I', '\0'), ONE_WCHAR('s', '\0'), ONE_WCHAR('s', '\0'), ONE_WCHAR('u', '\0'), ONE_WCHAR('e', '\0'), ONE_WCHAR('r', '\0'), ONE_WCHAR('A', '\0'), ONE_WCHAR('p', '\0'), ONE_WCHAR('p', '\0'), ONE_WCHAR('l', '\0'), ONE_WCHAR('i', '\0'), ONE_WCHAR('c', '\0'), ONE_WCHAR('a', '\0'), ONE_WCHAR('t', '\0'), ONE_WCHAR('i', '\0'), ONE_WCHAR('o', '\0'), ONE_WCHAR('n', '\0'), ONE_WCHAR('\0', '\0')}; /* "IssuerApplication" */
const DRM_WCHAR g_rgwchCertKeyUsageIssuerCRLSigner       [] = { ONE_WCHAR('I', '\0'), ONE_WCHAR('s', '\0'), ONE_WCHAR('s', '\0'), ONE_WCHAR('u', '\0'), ONE_WCHAR('e', '\0'), ONE_WCHAR('r', '\0'), ONE_WCHAR('C', '\0'), ONE_WCHAR('R', '\0'), ONE_WCHAR('L', '\0'), ONE_WCHAR('S', '\0'), ONE_WCHAR('i', '\0'), ONE_WCHAR('g', '\0'), ONE_WCHAR('n', '\0'), ONE_WCHAR('e', '\0'), ONE_WCHAR('r', '\0'), ONE_WCHAR('\0', '\0')}; /* "IssuerCRLSigner" */
const DRM_WCHAR g_rgwchCertKeyUsageIssuerMetering        [] = { ONE_WCHAR('I', '\0'), ONE_WCHAR('s', '\0'), ONE_WCHAR('s', '\0'), ONE_WCHAR('u', '\0'), ONE_WCHAR('e', '\0'), ONE_WCHAR('r', '\0'), ONE_WCHAR('M', '\0'), ONE_WCHAR('e', '\0'), ONE_WCHAR('t', '\0'), ONE_WCHAR('e', '\0'), ONE_WCHAR('r', '\0'), ONE_WCHAR('i', '\0'), ONE_WCHAR('n', '\0'), ONE_WCHAR('g', '\0'), ONE_WCHAR('\0', '\0')}; /* "IssuerMetering" */
const DRM_WCHAR g_rgwchCertKeyUsageIssuerKeyFileSigner   [] = { ONE_WCHAR('I', '\0'), ONE_WCHAR('s', '\0'), ONE_WCHAR('s', '\0'), ONE_WCHAR('u', '\0'), ONE_WCHAR('e', '\0'), ONE_WCHAR('r', '\0'), ONE_WCHAR('K', '\0'), ONE_WCHAR('e', '\0'), ONE_WCHAR('y', '\0'), ONE_WCHAR('F', '\0'), ONE_WCHAR('i', '\0'), ONE_WCHAR('l', '\0'), ONE_WCHAR('e', '\0'), ONE_WCHAR('S', '\0'), ONE_WCHAR('i', '\0'), ONE_WCHAR('g', '\0'), ONE_WCHAR('n', '\0'), ONE_WCHAR('e', '\0'), ONE_WCHAR('r', '\0'), ONE_WCHAR('\0', '\0')}; /* "IssuerKeyFileSigner" */
const DRM_WCHAR g_rgwchCertKeyUsageIssuerLicenseSigner   [] = { ONE_WCHAR('I', '\0'), ONE_WCHAR('s', '\0'), ONE_WCHAR('s', '\0'), ONE_WCHAR('u', '\0'), ONE_WCHAR('e', '\0'), ONE_WCHAR('r', '\0'), ONE_WCHAR('L', '\0'), ONE_WCHAR('i', '\0'), ONE_WCHAR('c', '\0'), ONE_WCHAR('e', '\0'), ONE_WCHAR('n', '\0'), ONE_WCHAR('s', '\0'), ONE_WCHAR('e', '\0'), ONE_WCHAR('S', '\0'), ONE_WCHAR('i', '\0'), ONE_WCHAR('g', '\0'), ONE_WCHAR('n', '\0'), ONE_WCHAR('e', '\0'), ONE_WCHAR('r', '\0'), ONE_WCHAR('\0', '\0')}; /* "IssuerLicenseSigner" */
const DRM_WCHAR g_rgwchCertKeyUsageIssuerServer          [] = { ONE_WCHAR('I', '\0'), ONE_WCHAR('s', '\0'), ONE_WCHAR('s', '\0'), ONE_WCHAR('u', '\0'), ONE_WCHAR('e', '\0'), ONE_WCHAR('r', '\0'), ONE_WCHAR('S', '\0'), ONE_WCHAR('e', '\0'), ONE_WCHAR('r', '\0'), ONE_WCHAR('v', '\0'), ONE_WCHAR('e', '\0'), ONE_WCHAR('r', '\0'), ONE_WCHAR('\0', '\0')}; /* "IssuerServer" */
const DRM_WCHAR g_rgwchCertKeyUsageIssuerIndivWindows    [] = { ONE_WCHAR('I', '\0'), ONE_WCHAR('s', '\0'), ONE_WCHAR('s', '\0'), ONE_WCHAR('u', '\0'), ONE_WCHAR('e', '\0'), ONE_WCHAR('r', '\0'), ONE_WCHAR('I', '\0'), ONE_WCHAR('n', '\0'), ONE_WCHAR('d', '\0'), ONE_WCHAR('i', '\0'), ONE_WCHAR('v', '\0'), ONE_WCHAR('W', '\0'), ONE_WCHAR('i', '\0'), ONE_WCHAR('n', '\0'), ONE_WCHAR('d', '\0'), ONE_WCHAR('o', '\0'), ONE_WCHAR('w', '\0'), ONE_WCHAR('s', '\0'), ONE_WCHAR('\0', '\0')}; /* "IssuerIndivWindows" */
const DRM_WCHAR g_rgwchCertKeyUsageIssuerIndivOsx        [] = { ONE_WCHAR('I', '\0'), ONE_WCHAR('s', '\0'), ONE_WCHAR('s', '\0'), ONE_WCHAR('u', '\0'), ONE_WCHAR('e', '\0'), ONE_WCHAR('r', '\0'), ONE_WCHAR('I', '\0'), ONE_WCHAR('n', '\0'), ONE_WCHAR('d', '\0'), ONE_WCHAR('i', '\0'), ONE_WCHAR('v', '\0'), ONE_WCHAR('O', '\0'), ONE_WCHAR('s', '\0'), ONE_WCHAR('x', '\0'), ONE_WCHAR('\0', '\0')}; /* "IssuerIndivOsx" */
const DRM_WCHAR g_rgwchCertKeyUsageSignKeyFile           [] = { ONE_WCHAR('S', '\0'), ONE_WCHAR('i', '\0'), ONE_WCHAR('g', '\0'), ONE_WCHAR('n', '\0'), ONE_WCHAR('K', '\0'), ONE_WCHAR('e', '\0'), ONE_WCHAR('y', '\0'), ONE_WCHAR('F', '\0'), ONE_WCHAR('i', '\0'), ONE_WCHAR('l', '\0'), ONE_WCHAR('e', '\0'), ONE_WCHAR('\0', '\0')}; /* "SignKeyFile" */
const DRM_WCHAR g_rgwchCertKeyUsageSignLicense           [] = { ONE_WCHAR('S', '\0'), ONE_WCHAR('i', '\0'), ONE_WCHAR('g', '\0'), ONE_WCHAR('n', '\0'), ONE_WCHAR('L', '\0'), ONE_WCHAR('i', '\0'), ONE_WCHAR('c', '\0'), ONE_WCHAR('e', '\0'), ONE_WCHAR('n', '\0'), ONE_WCHAR('s', '\0'), ONE_WCHAR('e', '\0'), ONE_WCHAR('\0', '\0')}; /* "SignLicense" */
const DRM_WCHAR g_rgwchCertKeyUsageSignResponse          [] = { ONE_WCHAR('S', '\0'), ONE_WCHAR('i', '\0'), ONE_WCHAR('g', '\0'), ONE_WCHAR('n', '\0'), ONE_WCHAR('R', '\0'), ONE_WCHAR('e', '\0'), ONE_WCHAR('s', '\0'), ONE_WCHAR('p', '\0'), ONE_WCHAR('o', '\0'), ONE_WCHAR('n', '\0'), ONE_WCHAR('s', '\0'), ONE_WCHAR('e', '\0'), ONE_WCHAR('\0', '\0')}; /* "SignResponse" */
const DRM_WCHAR g_rgwchCertSigningKey                    [] = { ONE_WCHAR('s', '\0'), ONE_WCHAR('i', '\0'), ONE_WCHAR('g', '\0'), ONE_WCHAR('n', '\0'), ONE_WCHAR('i', '\0'), ONE_WCHAR('n', '\0'), ONE_WCHAR('g', '\0'), ONE_WCHAR('k', '\0'), ONE_WCHAR('e', '\0'), ONE_WCHAR('y', '\0'), ONE_WCHAR('\0', '\0')}; /* "signingkey" */
const DRM_WCHAR g_rgwchCertSigningKeyValue               [] = { ONE_WCHAR('v', '\0'), ONE_WCHAR('a', '\0'), ONE_WCHAR('l', '\0'), ONE_WCHAR('u', '\0'), ONE_WCHAR('e', '\0'), ONE_WCHAR('\0', '\0')}; /* "value" */
const DRM_WCHAR g_rgwchPlatformIdentifier                [] = { ONE_WCHAR('p', '\0'), ONE_WCHAR('l', '\0'), ONE_WCHAR('a', '\0'), ONE_WCHAR('t', '\0'), ONE_WCHAR('f', '\0'), ONE_WCHAR('o', '\0'), ONE_WCHAR('r', '\0'), ONE_WCHAR('m', '\0'), ONE_WCHAR('i', '\0'), ONE_WCHAR('d', '\0'), ONE_WCHAR('e', '\0'), ONE_WCHAR('n', '\0'), ONE_WCHAR('t', '\0'), ONE_WCHAR('i', '\0'), ONE_WCHAR('f', '\0'), ONE_WCHAR('i', '\0'), ONE_WCHAR('e', '\0'), ONE_WCHAR('r', '\0'), ONE_WCHAR('\0', '\0')}; /* "platformidentifier" */
const DRM_WCHAR g_rgwchMeteringID                        [] = { ONE_WCHAR('m', '\0'), ONE_WCHAR('e', '\0'), ONE_WCHAR('t', '\0'), ONE_WCHAR('e', '\0'), ONE_WCHAR('r', '\0'), ONE_WCHAR('i', '\0'), ONE_WCHAR('n', '\0'), ONE_WCHAR('g', '\0'), ONE_WCHAR('i', '\0'), ONE_WCHAR('d', '\0'), ONE_WCHAR('\0', '\0')}; /* "meteringid" */
const DRM_WCHAR g_rgwchOne                               [] = { ONE_WCHAR('1', '\0'), ONE_WCHAR('\0', '\0')}; /* "1" */
const DRM_WCHAR g_rgwchSigningKeyPrivKeyXPath            [] = { ONE_WCHAR('c', '\0'), ONE_WCHAR('e', '\0'), ONE_WCHAR('r', '\0'), ONE_WCHAR('t', '\0'), ONE_WCHAR('k', '\0'), ONE_WCHAR('e', '\0'), ONE_WCHAR('y', '\0'), ONE_WCHAR('/', '\0'), ONE_WCHAR('K', '\0'), ONE_WCHAR('E', '\0'), ONE_WCHAR('Y', '\0'), ONE_WCHAR('P', '\0'), ONE_WCHAR('A', '\0'), ONE_WCHAR('I', '\0'), ONE_WCHAR('R', '\0'), ONE_WCHAR('/', '\0'), ONE_WCHAR('P', '\0'), ONE_WCHAR('R', '\0'), ONE_WCHAR('I', '\0'), ONE_WCHAR('V', '\0'), ONE_WCHAR('K', '\0'), ONE_WCHAR('E', '\0'), ONE_WCHAR('Y', '\0'), ONE_WCHAR('\0', '\0')}; /* "certkey/KEYPAIR/PRIVKEY" */
const DRM_WCHAR g_rgwchSigningKeyPubKeyXPath             [] = { ONE_WCHAR('c', '\0'), ONE_WCHAR('e', '\0'), ONE_WCHAR('r', '\0'), ONE_WCHAR('t', '\0'), ONE_WCHAR('k', '\0'), ONE_WCHAR('e', '\0'), ONE_WCHAR('y', '\0'), ONE_WCHAR('/', '\0'), ONE_WCHAR('K', '\0'), ONE_WCHAR('E', '\0'), ONE_WCHAR('Y', '\0'), ONE_WCHAR('P', '\0'), ONE_WCHAR('A', '\0'), ONE_WCHAR('I', '\0'), ONE_WCHAR('R', '\0'), ONE_WCHAR('/', '\0'), ONE_WCHAR('P', '\0'), ONE_WCHAR('U', '\0'), ONE_WCHAR('B', '\0'), ONE_WCHAR('K', '\0'), ONE_WCHAR('E', '\0'), ONE_WCHAR('Y', '\0'), ONE_WCHAR('\0', '\0')}; /* "certkey/KEYPAIR/PUBKEY" */
const DRM_WCHAR g_rgwchSecurityVersionObject             [] = { ONE_WCHAR('s', '\0'), ONE_WCHAR('e', '\0'), ONE_WCHAR('c', '\0'), ONE_WCHAR('u', '\0'), ONE_WCHAR('r', '\0'), ONE_WCHAR('i', '\0'), ONE_WCHAR('t', '\0'), ONE_WCHAR('y', '\0'), ONE_WCHAR('v', '\0'), ONE_WCHAR('e', '\0'), ONE_WCHAR('r', '\0'), ONE_WCHAR('s', '\0'), ONE_WCHAR('i', '\0'), ONE_WCHAR('o', '\0'), ONE_WCHAR('n', '\0'), ONE_WCHAR('\0', '\0')}; /* "securityversion" */
const DRM_WCHAR g_rgwchSecurityVersionObjectPlatformId   [] = { ONE_WCHAR('p', '\0'), ONE_WCHAR('l', '\0'), ONE_WCHAR('a', '\0'), ONE_WCHAR('t', '\0'), ONE_WCHAR('f', '\0'), ONE_WCHAR('o', '\0'), ONE_WCHAR('r', '\0'), ONE_WCHAR('m', '\0'), ONE_WCHAR('_', '\0'), ONE_WCHAR('i', '\0'), ONE_WCHAR('d', '\0'), ONE_WCHAR('\0', '\0')}; /* "platform_id" */
const DRM_WCHAR g_rgwchSecurityVersionObjectNumber       [] = { ONE_WCHAR('n', '\0'), ONE_WCHAR('u', '\0'), ONE_WCHAR('m', '\0'), ONE_WCHAR('b', '\0'), ONE_WCHAR('e', '\0'), ONE_WCHAR('r', '\0'), ONE_WCHAR('\0', '\0')}; /* "number" */


const DRM_CONST_STRING  g_dstrVersion                               = CREATE_DRM_STRING(g_rgwchVersion);
const DRM_CONST_STRING  g_dstrCertType                              = CREATE_DRM_STRING(g_rgwchCertType);
const DRM_CONST_STRING  g_dstrDevice                                = CREATE_DRM_STRING(g_rgwchDevice);
const DRM_CONST_STRING  g_dstrPC                                    = CREATE_DRM_STRING(g_rgwchPC);
const DRM_CONST_STRING  g_dstrExtendedData                          = CREATE_DRM_STRING(g_rgwchExtendedData);
const DRM_CONST_STRING  g_dstrExtendedDataSigningKey                = CREATE_DRM_STRING(g_rgwchExtendedDataSigningKey);
const DRM_CONST_STRING  g_dstrExtendedDataSigningKeyPublicKey       = CREATE_DRM_STRING(g_rgwchExtendedDataSigningKeyPublicKey);
const DRM_CONST_STRING  g_dstrExtendedDataSigningKeyPrivateKey      = CREATE_DRM_STRING(g_rgwchExtendedDataSigningKeyPrivateKey);
const DRM_CONST_STRING  g_dstrExtendedDataRecord                    = CREATE_DRM_STRING(g_rgwchExtendedDataRecord);
const DRM_CONST_STRING  g_dstrDomain                                = CREATE_DRM_STRING(g_rgwchDomain);
const DRM_CONST_STRING  g_dstrSilverLight                           = CREATE_DRM_STRING(g_rgwchSilverLight);
const DRM_CONST_STRING  g_dstrApplication                           = CREATE_DRM_STRING(g_rgwchApplication);
const DRM_CONST_STRING  g_dstrMeteringCertType                      = CREATE_DRM_STRING(g_rgwchMeteringCertType);
const DRM_CONST_STRING  g_dstrServerCertType                        = CREATE_DRM_STRING(g_rgwchServerCertType);
const DRM_CONST_STRING  g_dstrIssuer                                = CREATE_DRM_STRING(g_rgwchIssuer);
const DRM_CONST_STRING  g_dstrService                               = CREATE_DRM_STRING(g_rgwchService);
const DRM_CONST_STRING  g_dstrCRLSigner                             = CREATE_DRM_STRING(g_rgwchCRLSigner);
const DRM_CONST_STRING  g_dstrKeyFileSigner                         = CREATE_DRM_STRING(g_rgwchKeyFileSigner);
const DRM_CONST_STRING  g_dstrLicenseSigner                         = CREATE_DRM_STRING(g_rgwchLicenseSigner);
const DRM_CONST_STRING  g_dstrBasicInfo                             = CREATE_DRM_STRING(g_rgwchBasicInfo);
const DRM_CONST_STRING  g_dstrSecurityLevel                         = CREATE_DRM_STRING(g_rgwchSecurityLevel);
const DRM_CONST_STRING  g_dstrClientID                              = CREATE_DRM_STRING(g_rgwchClientID);
const DRM_CONST_STRING  g_dstrExpiration                            = CREATE_DRM_STRING(g_rgwchExpiration);
const DRM_CONST_STRING  g_dstrWarningDays                           = CREATE_DRM_STRING(g_rgwchWarningDays);
const DRM_CONST_STRING  g_dstrServiceID                             = CREATE_DRM_STRING(g_rgwchServiceID);
const DRM_CONST_STRING  g_dstrAccountID                             = CREATE_DRM_STRING(g_rgwchAccountID);
const DRM_CONST_STRING  g_dstrRevision                              = CREATE_DRM_STRING(g_rgwchRevision);
const DRM_CONST_STRING  g_dstrDomainUrl                             = CREATE_DRM_STRING(g_rgwchUrl);
const DRM_CONST_STRING  g_dstrSecurityVersion                       = CREATE_DRM_STRING(g_rgwchSecurityVersion);
const DRM_CONST_STRING  g_dstrSecurityVersionSeperator              = CREATE_DRM_STRING(g_rgwchSecurityVersionSeperator);
const DRM_CONST_STRING  g_dstrExtendedDataSigningKeyType            = CREATE_DRM_STRING(g_rgwchCertType);
const DRM_CONST_STRING  g_dstrExtendedDataSigningKeyPublicKeyValue  = CREATE_DRM_STRING(g_rgwchValue);
const DRM_CONST_STRING  g_dstrExtendedDataSigningKeyPrivateKeyValue = CREATE_DRM_STRING(g_rgwchValue);
const DRM_CONST_STRING  g_dstrExtendedDataRecordType                = CREATE_DRM_STRING(g_rgwchCertType);
const DRM_CONST_STRING  g_dstrExtendedDataRecordFlags               = CREATE_DRM_STRING(g_rgwchExtendedDataRecordFlags);
const DRM_CONST_STRING  g_dstrExtendedDataRecordMaxLength           = CREATE_DRM_STRING(g_rgwchExtendedDataRecordMaxLength);
const DRM_CONST_STRING  g_dstrExtendedDataRecordValue               = CREATE_DRM_STRING(g_rgwchValue);
const DRM_CONST_STRING  g_dstrTransmitter                           = CREATE_DRM_STRING(g_rgwchTransmitter);
const DRM_CONST_STRING  g_dstrReceiver                              = CREATE_DRM_STRING(g_rgwchReceiver);
const DRM_CONST_STRING  g_dstrSharedCertificate                     = CREATE_DRM_STRING(g_rgwchSharedCertificate);
const DRM_CONST_STRING  g_dstrSecureClock                           = CREATE_DRM_STRING(g_rgwchSecureClock);
const DRM_CONST_STRING  g_dstrSampleProtection                      = CREATE_DRM_STRING(g_rgwchSampleProtection);
const DRM_CONST_STRING  g_dstrRollbackClock                         = CREATE_DRM_STRING(g_rgwchRollbackClock);
const DRM_CONST_STRING  g_dstrMetering                              = CREATE_DRM_STRING(g_rgwchMetering);
const DRM_CONST_STRING  g_dstrLicenseSync                           = CREATE_DRM_STRING(g_rgwchLicenseSync);
const DRM_CONST_STRING  g_dstrSymmOpt                               = CREATE_DRM_STRING(g_rgwchSymmOpt);
const DRM_CONST_STRING  g_dstrSupportCRL                            = CREATE_DRM_STRING(g_rgwchSupportCRL);

const DRM_CONST_STRING  g_dstrServerBasicEdition        = CREATE_DRM_STRING(g_rgwchServerBasicEdition);
const DRM_CONST_STRING  g_dstrServerStandardEdition     = CREATE_DRM_STRING(g_rgwchServerStandardEdition);
const DRM_CONST_STRING  g_dstrServerPremiumEdition      = CREATE_DRM_STRING(g_rgwchServerPremiumEdition);

const DRM_CONST_STRING  g_dstrCertFeatures                      = CREATE_DRM_STRING(g_rgwchCertFeatures);
const DRM_CONST_STRING  g_dstrCertManufacturer                  = CREATE_DRM_STRING(g_rgwchCertManufacturer);
const DRM_CONST_STRING  g_dstrManufacturerName                  = CREATE_DRM_STRING(g_rgwchManufacturerName);
const DRM_CONST_STRING  g_dstrMake                              = CREATE_DRM_STRING(g_rgwchMake);
const DRM_CONST_STRING  g_dstrModelName                         = CREATE_DRM_STRING(g_rgwchModelName);
const DRM_CONST_STRING  g_dstrModelNumber                       = CREATE_DRM_STRING(g_rgwchModelNumber);
const DRM_CONST_STRING  g_dstrLimits                            = CREATE_DRM_STRING(g_rgwchLimits);
const DRM_CONST_STRING  g_dstrCertKey                           = CREATE_DRM_STRING(g_rgwchCertKey);
const DRM_CONST_STRING  g_dstrCertKeyType                       = CREATE_DRM_STRING(g_rgwchCertKeyType);
const DRM_CONST_STRING  g_dstrCertKeys                          = CREATE_DRM_STRING(g_rgwchCertKeys);
const DRM_CONST_STRING  g_dstrCertECC256                        = CREATE_DRM_STRING(g_rgwchCertECC256);
const DRM_CONST_STRING  g_dstrCertKeyUsages                     = CREATE_DRM_STRING(g_rgwchCertKeyUsages);
const DRM_CONST_STRING  g_dstrKeyUsageSign                      = CREATE_DRM_STRING(g_rgwchCertKeyUsageSign);
const DRM_CONST_STRING  g_dstrKeyUsageEncryptKey                = CREATE_DRM_STRING(g_rgwchCertKeyUsageEncryptKey);
const DRM_CONST_STRING  g_dstrKeyUsageSignCRL                   = CREATE_DRM_STRING(g_rgwchCertKeyUsageSignCRL);
const DRM_CONST_STRING  g_dstrKeyUsageIssuerAll                 = CREATE_DRM_STRING(g_rgwchCertKeyUsageIssuerAll);
const DRM_CONST_STRING  g_dstrKeyUsageIssuerIndiv               = CREATE_DRM_STRING(g_rgwchCertKeyUsageIssuerIndiv);
const DRM_CONST_STRING  g_dstrKeyUsageIssuerDevice              = CREATE_DRM_STRING(g_rgwchCertKeyUsageIssuerDevice);
const DRM_CONST_STRING  g_dstrKeyUsageIssuerLink                = CREATE_DRM_STRING(g_rgwchCertKeyUsageIssuerLink);
const DRM_CONST_STRING  g_dstrKeyUsageIssuerDomain              = CREATE_DRM_STRING(g_rgwchCertKeyUsageIssuerDomain);
const DRM_CONST_STRING  g_dstrKeyUsageIssuerSilverLight         = CREATE_DRM_STRING(g_rgwchCertKeyUsageIssuerSilverLight);
const DRM_CONST_STRING  g_dstrKeyUsageIssuerApplication         = CREATE_DRM_STRING(g_rgwchCertKeyUsageIssuerApplication);
const DRM_CONST_STRING  g_dstrKeyUsageIssuerCRLSigner           = CREATE_DRM_STRING(g_rgwchCertKeyUsageIssuerCRLSigner);
const DRM_CONST_STRING  g_dstrKeyUsageIssuerMetering            = CREATE_DRM_STRING(g_rgwchCertKeyUsageIssuerMetering);
const DRM_CONST_STRING  g_dstrKeyUsageIssuerKeyFileSigner       = CREATE_DRM_STRING(g_rgwchCertKeyUsageIssuerKeyFileSigner);
const DRM_CONST_STRING  g_dstrKeyUsageIssuerLicenseSigner       = CREATE_DRM_STRING(g_rgwchCertKeyUsageIssuerLicenseSigner);
const DRM_CONST_STRING  g_dstrKeyUsageIssuerServer              = CREATE_DRM_STRING(g_rgwchCertKeyUsageIssuerServer);
const DRM_CONST_STRING  g_dstrKeyUsageIssuerIndivWindows        = CREATE_DRM_STRING(g_rgwchCertKeyUsageIssuerIndivWindows);
const DRM_CONST_STRING  g_dstrKeyUsageIssuerIndivOsx            = CREATE_DRM_STRING(g_rgwchCertKeyUsageIssuerIndivOsx);
const DRM_CONST_STRING  g_dstrKeyUsageSignKeyFile               = CREATE_DRM_STRING(g_rgwchCertKeyUsageSignKeyFile);
const DRM_CONST_STRING  g_dstrKeyUsageSignLicense               = CREATE_DRM_STRING(g_rgwchCertKeyUsageSignLicense);
const DRM_CONST_STRING  g_dstrKeyUsageSignResponse              = CREATE_DRM_STRING(g_rgwchCertKeyUsageSignResponse);
const DRM_CONST_STRING  g_dstrCertSigningKey                    = CREATE_DRM_STRING(g_rgwchCertSigningKey);
const DRM_CONST_STRING  g_dstrCertSigningKeyValue               = CREATE_DRM_STRING(g_rgwchCertSigningKeyValue);
const DRM_CONST_STRING  g_dstrPlatformIdentifier                = CREATE_DRM_STRING(g_rgwchPlatformIdentifier);
const DRM_CONST_STRING  g_dstrMeteringID                        = CREATE_DRM_STRING(g_rgwchMeteringID);
const DRM_CONST_STRING  g_dstrMeteringUrl                       = CREATE_DRM_STRING(g_rgwchUrl);
const DRM_CONST_STRING  g_dstrOne                               = CREATE_DRM_STRING(g_rgwchOne);
const DRM_CONST_STRING  g_dstrSigningKeyPrivKeyXPath            = CREATE_DRM_STRING(g_rgwchSigningKeyPrivKeyXPath);
const DRM_CONST_STRING  g_dstrSigningKeyPubKeyXPath             = CREATE_DRM_STRING(g_rgwchSigningKeyPubKeyXPath);
const DRM_CONST_STRING  g_dstrSecurityVersionObject             = CREATE_DRM_STRING(g_rgwchSecurityVersionObject);
const DRM_CONST_STRING  g_dstrSecurityVersionObjectPlatformId   = CREATE_DRM_STRING(g_rgwchSecurityVersionObjectPlatformId);
const DRM_CONST_STRING  g_dstrSecurityVersionObjectNumber       = CREATE_DRM_STRING(g_rgwchSecurityVersionObjectNumber);

const DRM_CHAR  g_rgchTagName [] = "NAME";
#define g_rgchTagName_LEN 4
const DRM_ANSI_CONST_STRING g_dastrTagName = CREATE_DRM_ANSI_STRING_EX( g_rgchTagName, g_rgchTagName_LEN );

const DRM_CHAR  g_rgchManufacturer [] = "MANUFACTURER";
#define g_rgchManufacturer_LEN 12
const DRM_ANSI_CONST_STRING g_dastrManufacturer = CREATE_DRM_ANSI_STRING_EX( g_rgchManufacturer, g_rgchManufacturer_LEN );

const DRM_CHAR  g_rgchMake [] = "MAKE";
#define g_rgchMake_LEN 4
const DRM_ANSI_CONST_STRING g_dastrMake = CREATE_DRM_ANSI_STRING_EX( g_rgchMake, g_rgchMake_LEN );

const DRM_CHAR  g_rgchModel [] = "MODEL";
#define g_rgchModel_LEN 5
const DRM_ANSI_CONST_STRING g_dastrModel = CREATE_DRM_ANSI_STRING_EX( g_rgchModel, g_rgchModel_LEN );

const DRM_CHAR  g_rgchDefaultSecurityLevel [] = "150";
#define g_rgchDefaultSecurityLevel_LEN 3
const DRM_ANSI_CONST_STRING g_dastrDefaultSecurityLevel = CREATE_DRM_ANSI_STRING_EX( g_rgchDefaultSecurityLevel, g_rgchDefaultSecurityLevel_LEN );

/* PlayReadyND/PRIVKEY/Prime0 */
const DRM_WCHAR g_rgwchPlayReadyNDPRIVKEYPrime0 [] =  {
    ONE_WCHAR( 'P', '\0' ), ONE_WCHAR( 'l', '\0' ), ONE_WCHAR( 'a', '\0' ), ONE_WCHAR( 'y', '\0' ),
    ONE_WCHAR( 'R', '\0' ), ONE_WCHAR( 'e', '\0' ), ONE_WCHAR( 'a', '\0' ), ONE_WCHAR( 'd', '\0' ),
    ONE_WCHAR( 'y', '\0' ), ONE_WCHAR( 'N', '\0' ), ONE_WCHAR( 'D', '\0' ), ONE_WCHAR( '/', '\0' ),
    ONE_WCHAR( 'P', '\0' ), ONE_WCHAR( 'R', '\0' ), ONE_WCHAR( 'I', '\0' ), ONE_WCHAR( 'V', '\0' ),
    ONE_WCHAR( 'K', '\0' ), ONE_WCHAR( 'E', '\0' ), ONE_WCHAR( 'Y', '\0' ), ONE_WCHAR( '/', '\0' ),
    ONE_WCHAR( 'P', '\0' ), ONE_WCHAR( 'r', '\0' ), ONE_WCHAR( 'i', '\0' ), ONE_WCHAR( 'm', '\0' ),
    ONE_WCHAR( 'e', '\0' ), ONE_WCHAR( '0', '\0' ), ONE_WCHAR( '\0', '\0' )
};
const DRM_CONST_STRING g_dstrPlayReadyNDPRIVKEYPrime0 = CREATE_DRM_STRING( g_rgwchPlayReadyNDPRIVKEYPrime0 );

/* PlayReadyND/PRIVKEY/Prime1 */
const DRM_WCHAR g_rgwchPlayReadyNDPRIVKEYPrime1 [] =  {
    ONE_WCHAR( 'P', '\0' ), ONE_WCHAR( 'l', '\0' ), ONE_WCHAR( 'a', '\0' ), ONE_WCHAR( 'y', '\0' ),
    ONE_WCHAR( 'R', '\0' ), ONE_WCHAR( 'e', '\0' ), ONE_WCHAR( 'a', '\0' ), ONE_WCHAR( 'd', '\0' ),
    ONE_WCHAR( 'y', '\0' ), ONE_WCHAR( 'N', '\0' ), ONE_WCHAR( 'D', '\0' ), ONE_WCHAR( '/', '\0' ),
    ONE_WCHAR( 'P', '\0' ), ONE_WCHAR( 'R', '\0' ), ONE_WCHAR( 'I', '\0' ), ONE_WCHAR( 'V', '\0' ),
    ONE_WCHAR( 'K', '\0' ), ONE_WCHAR( 'E', '\0' ), ONE_WCHAR( 'Y', '\0' ), ONE_WCHAR( '/', '\0' ),
    ONE_WCHAR( 'P', '\0' ), ONE_WCHAR( 'r', '\0' ), ONE_WCHAR( 'i', '\0' ), ONE_WCHAR( 'm', '\0' ),
    ONE_WCHAR( 'e', '\0' ), ONE_WCHAR( '1', '\0' ), ONE_WCHAR( '\0', '\0' )
};
const DRM_CONST_STRING g_dstrPlayReadyNDPRIVKEYPrime1 = CREATE_DRM_STRING( g_rgwchPlayReadyNDPRIVKEYPrime1 );

/* PlayReadyND/PRIVKEY/CRTExponent0 */
const DRM_WCHAR g_rgwchPlayReadyNDPRIVKEYCRTExponent0 [] =  {
    ONE_WCHAR( 'P', '\0' ), ONE_WCHAR( 'l', '\0' ), ONE_WCHAR( 'a', '\0' ), ONE_WCHAR( 'y', '\0' ),
    ONE_WCHAR( 'R', '\0' ), ONE_WCHAR( 'e', '\0' ), ONE_WCHAR( 'a', '\0' ), ONE_WCHAR( 'd', '\0' ),
    ONE_WCHAR( 'y', '\0' ), ONE_WCHAR( 'N', '\0' ), ONE_WCHAR( 'D', '\0' ), ONE_WCHAR( '/', '\0' ),
    ONE_WCHAR( 'P', '\0' ), ONE_WCHAR( 'R', '\0' ), ONE_WCHAR( 'I', '\0' ), ONE_WCHAR( 'V', '\0' ),
    ONE_WCHAR( 'K', '\0' ), ONE_WCHAR( 'E', '\0' ), ONE_WCHAR( 'Y', '\0' ), ONE_WCHAR( '/', '\0' ),
    ONE_WCHAR( 'C', '\0' ), ONE_WCHAR( 'R', '\0' ), ONE_WCHAR( 'T', '\0' ), ONE_WCHAR( 'E', '\0' ),
    ONE_WCHAR( 'x', '\0' ), ONE_WCHAR( 'p', '\0' ), ONE_WCHAR( 'o', '\0' ), ONE_WCHAR( 'n', '\0' ),
    ONE_WCHAR( 'e', '\0' ), ONE_WCHAR( 'n', '\0' ), ONE_WCHAR( 't', '\0' ), ONE_WCHAR( '0', '\0' ),
    ONE_WCHAR( '\0', '\0' )
};
const DRM_CONST_STRING g_dstrPlayReadyNDPRIVKEYCRTExponent0 = CREATE_DRM_STRING( g_rgwchPlayReadyNDPRIVKEYCRTExponent0 );

/* PlayReadyND/PRIVKEY/CRTExponent1 */
const DRM_WCHAR g_rgwchPlayReadyNDPRIVKEYCRTExponent1 [] =  {
    ONE_WCHAR( 'P', '\0' ), ONE_WCHAR( 'l', '\0' ), ONE_WCHAR( 'a', '\0' ), ONE_WCHAR( 'y', '\0' ),
    ONE_WCHAR( 'R', '\0' ), ONE_WCHAR( 'e', '\0' ), ONE_WCHAR( 'a', '\0' ), ONE_WCHAR( 'd', '\0' ),
    ONE_WCHAR( 'y', '\0' ), ONE_WCHAR( 'N', '\0' ), ONE_WCHAR( 'D', '\0' ), ONE_WCHAR( '/', '\0' ),
    ONE_WCHAR( 'P', '\0' ), ONE_WCHAR( 'R', '\0' ), ONE_WCHAR( 'I', '\0' ), ONE_WCHAR( 'V', '\0' ),
    ONE_WCHAR( 'K', '\0' ), ONE_WCHAR( 'E', '\0' ), ONE_WCHAR( 'Y', '\0' ), ONE_WCHAR( '/', '\0' ),
    ONE_WCHAR( 'C', '\0' ), ONE_WCHAR( 'R', '\0' ), ONE_WCHAR( 'T', '\0' ), ONE_WCHAR( 'E', '\0' ),
    ONE_WCHAR( 'x', '\0' ), ONE_WCHAR( 'p', '\0' ), ONE_WCHAR( 'o', '\0' ), ONE_WCHAR( 'n', '\0' ),
    ONE_WCHAR( 'e', '\0' ), ONE_WCHAR( 'n', '\0' ), ONE_WCHAR( 't', '\0' ), ONE_WCHAR( '1', '\0' ),
    ONE_WCHAR( '\0', '\0' )
};
const DRM_CONST_STRING g_dstrPlayReadyNDPRIVKEYCRTExponent1 = CREATE_DRM_STRING( g_rgwchPlayReadyNDPRIVKEYCRTExponent1 );

/* PlayReadyND/PRIVKEY/IQMP */
const DRM_WCHAR g_rgwchPlayReadyNDPRIVKEYIQMP [] =  {
    ONE_WCHAR( 'P', '\0' ), ONE_WCHAR( 'l', '\0' ), ONE_WCHAR( 'a', '\0' ), ONE_WCHAR( 'y', '\0' ),
    ONE_WCHAR( 'R', '\0' ), ONE_WCHAR( 'e', '\0' ), ONE_WCHAR( 'a', '\0' ), ONE_WCHAR( 'd', '\0' ),
    ONE_WCHAR( 'y', '\0' ), ONE_WCHAR( 'N', '\0' ), ONE_WCHAR( 'D', '\0' ), ONE_WCHAR( '/', '\0' ),
    ONE_WCHAR( 'P', '\0' ), ONE_WCHAR( 'R', '\0' ), ONE_WCHAR( 'I', '\0' ), ONE_WCHAR( 'V', '\0' ),
    ONE_WCHAR( 'K', '\0' ), ONE_WCHAR( 'E', '\0' ), ONE_WCHAR( 'Y', '\0' ), ONE_WCHAR( '/', '\0' ),
    ONE_WCHAR( 'I', '\0' ), ONE_WCHAR( 'Q', '\0' ), ONE_WCHAR( 'M', '\0' ), ONE_WCHAR( 'P', '\0' ),
    ONE_WCHAR( '\0', '\0' )
};
const DRM_CONST_STRING g_dstrPlayReadyNDPRIVKEYIQMP = CREATE_DRM_STRING( g_rgwchPlayReadyNDPRIVKEYIQMP );

/* DAC160Key/PUBKEY */
const DRM_WCHAR g_rgwchDAC160KeyPUBKEY [] =  {
    ONE_WCHAR( 'D', '\0' ), ONE_WCHAR( 'A', '\0' ), ONE_WCHAR( 'C', '\0' ), ONE_WCHAR( '1', '\0' ),
    ONE_WCHAR( '6', '\0' ), ONE_WCHAR( '0', '\0' ), ONE_WCHAR( 'K', '\0' ), ONE_WCHAR( 'e', '\0' ),
    ONE_WCHAR( 'y', '\0' ), ONE_WCHAR( '/', '\0' ), ONE_WCHAR( 'P', '\0' ), ONE_WCHAR( 'U', '\0' ),
    ONE_WCHAR( 'B', '\0' ), ONE_WCHAR( 'K', '\0' ), ONE_WCHAR( 'E', '\0' ), ONE_WCHAR( 'Y', '\0' ),
    ONE_WCHAR( '\0', '\0' )
};
const DRM_CONST_STRING g_dstrDAC160KeyPUBKEY = CREATE_DRM_STRING( g_rgwchDAC160KeyPUBKEY );

/* MSBinCert256Key/PUBKEY */
const DRM_WCHAR g_rgwchMSBinCert256KeyPUBKEY [] =  {
    ONE_WCHAR( 'M', '\0' ), ONE_WCHAR( 'S', '\0' ), ONE_WCHAR( 'B', '\0' ), ONE_WCHAR( 'i', '\0' ),
    ONE_WCHAR( 'n', '\0' ), ONE_WCHAR( 'C', '\0' ), ONE_WCHAR( 'e', '\0' ), ONE_WCHAR( 'r', '\0' ),
    ONE_WCHAR( 't', '\0' ), ONE_WCHAR( '2', '\0' ), ONE_WCHAR( '5', '\0' ), ONE_WCHAR( '6', '\0' ),
    ONE_WCHAR( 'K', '\0' ), ONE_WCHAR( 'e', '\0' ), ONE_WCHAR( 'y', '\0' ), ONE_WCHAR( '/', '\0' ),
    ONE_WCHAR( 'P', '\0' ), ONE_WCHAR( 'U', '\0' ), ONE_WCHAR( 'B', '\0' ), ONE_WCHAR( 'K', '\0' ),
    ONE_WCHAR( 'E', '\0' ), ONE_WCHAR( 'Y', '\0' ), ONE_WCHAR( '\0', '\0' )
};
const DRM_CONST_STRING g_dstrMSBinCert256KeyPUBKEY = CREATE_DRM_STRING( g_rgwchMSBinCert256KeyPUBKEY );

/* MSRSA1024CertKey/PUBKEY/Modulus */
const DRM_WCHAR g_rgwchMSRSA1024CertKeyPUBKEYModulus [] =  {
    ONE_WCHAR( 'M', '\0' ), ONE_WCHAR( 'S', '\0' ), ONE_WCHAR( 'R', '\0' ), ONE_WCHAR( 'S', '\0' ),
    ONE_WCHAR( 'A', '\0' ), ONE_WCHAR( '1', '\0' ), ONE_WCHAR( '0', '\0' ), ONE_WCHAR( '2', '\0' ),
    ONE_WCHAR( '4', '\0' ), ONE_WCHAR( 'C', '\0' ), ONE_WCHAR( 'e', '\0' ), ONE_WCHAR( 'r', '\0' ),
    ONE_WCHAR( 't', '\0' ), ONE_WCHAR( 'K', '\0' ), ONE_WCHAR( 'e', '\0' ), ONE_WCHAR( 'y', '\0' ),
    ONE_WCHAR( '/', '\0' ), ONE_WCHAR( 'P', '\0' ), ONE_WCHAR( 'U', '\0' ), ONE_WCHAR( 'B', '\0' ),
    ONE_WCHAR( 'K', '\0' ), ONE_WCHAR( 'E', '\0' ), ONE_WCHAR( 'Y', '\0' ), ONE_WCHAR( '/', '\0' ),
    ONE_WCHAR( 'M', '\0' ), ONE_WCHAR( 'o', '\0' ), ONE_WCHAR( 'd', '\0' ), ONE_WCHAR( 'u', '\0' ),
    ONE_WCHAR( 'l', '\0' ), ONE_WCHAR( 'u', '\0' ), ONE_WCHAR( 's', '\0' ), ONE_WCHAR( '\0', '\0' )
};
const DRM_CONST_STRING g_dstrMSRSA1024CertKeyPUBKEYModulus = CREATE_DRM_STRING( g_rgwchMSRSA1024CertKeyPUBKEYModulus );

/* MSRSA1024CertKey/PUBKEY/Exponent */
const DRM_WCHAR g_rgwchMSRSA1024CertKeyPUBKEYExponent [] =  {
    ONE_WCHAR( 'M', '\0' ), ONE_WCHAR( 'S', '\0' ), ONE_WCHAR( 'R', '\0' ), ONE_WCHAR( 'S', '\0' ),
    ONE_WCHAR( 'A', '\0' ), ONE_WCHAR( '1', '\0' ), ONE_WCHAR( '0', '\0' ), ONE_WCHAR( '2', '\0' ),
    ONE_WCHAR( '4', '\0' ), ONE_WCHAR( 'C', '\0' ), ONE_WCHAR( 'e', '\0' ), ONE_WCHAR( 'r', '\0' ),
    ONE_WCHAR( 't', '\0' ), ONE_WCHAR( 'K', '\0' ), ONE_WCHAR( 'e', '\0' ), ONE_WCHAR( 'y', '\0' ),
    ONE_WCHAR( '/', '\0' ), ONE_WCHAR( 'P', '\0' ), ONE_WCHAR( 'U', '\0' ), ONE_WCHAR( 'B', '\0' ),
    ONE_WCHAR( 'K', '\0' ), ONE_WCHAR( 'E', '\0' ), ONE_WCHAR( 'Y', '\0' ), ONE_WCHAR( '/', '\0' ),
    ONE_WCHAR( 'E', '\0' ), ONE_WCHAR( 'x', '\0' ), ONE_WCHAR( 'p', '\0' ), ONE_WCHAR( 'o', '\0' ),
    ONE_WCHAR( 'n', '\0' ), ONE_WCHAR( 'e', '\0' ), ONE_WCHAR( 'n', '\0' ), ONE_WCHAR( 't', '\0' ),
    ONE_WCHAR( '\0', '\0' )
};
const DRM_CONST_STRING g_dstrMSRSA1024CertKeyPUBKEYExponent = CREATE_DRM_STRING( g_rgwchMSRSA1024CertKeyPUBKEYExponent );

/* DAC160Key/PRIVKEY */
const DRM_WCHAR g_rgwchDAC160KeyPRIVKEY [] =  {
    ONE_WCHAR( 'D', '\0' ), ONE_WCHAR( 'A', '\0' ), ONE_WCHAR( 'C', '\0' ), ONE_WCHAR( '1', '\0' ),
    ONE_WCHAR( '6', '\0' ), ONE_WCHAR( '0', '\0' ), ONE_WCHAR( 'K', '\0' ), ONE_WCHAR( 'e', '\0' ),
    ONE_WCHAR( 'y', '\0' ), ONE_WCHAR( '/', '\0' ), ONE_WCHAR( 'P', '\0' ), ONE_WCHAR( 'R', '\0' ),
    ONE_WCHAR( 'I', '\0' ), ONE_WCHAR( 'V', '\0' ), ONE_WCHAR( 'K', '\0' ), ONE_WCHAR( 'E', '\0' ),
    ONE_WCHAR( 'Y', '\0' ), ONE_WCHAR( '\0', '\0' )
};
const DRM_CONST_STRING g_dstrDAC160KeyPRIVKEY = CREATE_DRM_STRING( g_rgwchDAC160KeyPRIVKEY );

/* MS160Key/PRIVKEY */
const DRM_WCHAR g_rgwchMS160KeyPRIVKEY [] =  {
    ONE_WCHAR( 'M', '\0' ), ONE_WCHAR( 'S', '\0' ), ONE_WCHAR( '1', '\0' ), ONE_WCHAR( '6', '\0' ),
    ONE_WCHAR( '0', '\0' ), ONE_WCHAR( 'K', '\0' ), ONE_WCHAR( 'e', '\0' ), ONE_WCHAR( 'y', '\0' ),
    ONE_WCHAR( '/', '\0' ), ONE_WCHAR( 'P', '\0' ), ONE_WCHAR( 'R', '\0' ), ONE_WCHAR( 'I', '\0' ),
    ONE_WCHAR( 'V', '\0' ), ONE_WCHAR( 'K', '\0' ), ONE_WCHAR( 'E', '\0' ), ONE_WCHAR( 'Y', '\0' ),
    ONE_WCHAR( '\0', '\0' )
};
const DRM_CONST_STRING g_dstrMS160KeyPRIVKEY = CREATE_DRM_STRING( g_rgwchMS160KeyPRIVKEY );

/* MSBinCert256Key/PRIVKEY */
const DRM_WCHAR g_rgwchMSBinCert256KeyPRIVKEY [] =  {
    ONE_WCHAR( 'M', '\0' ), ONE_WCHAR( 'S', '\0' ), ONE_WCHAR( 'B', '\0' ), ONE_WCHAR( 'i', '\0' ),
    ONE_WCHAR( 'n', '\0' ), ONE_WCHAR( 'C', '\0' ), ONE_WCHAR( 'e', '\0' ), ONE_WCHAR( 'r', '\0' ),
    ONE_WCHAR( 't', '\0' ), ONE_WCHAR( '2', '\0' ), ONE_WCHAR( '5', '\0' ), ONE_WCHAR( '6', '\0' ),
    ONE_WCHAR( 'K', '\0' ), ONE_WCHAR( 'e', '\0' ), ONE_WCHAR( 'y', '\0' ), ONE_WCHAR( '/', '\0' ),
    ONE_WCHAR( 'P', '\0' ), ONE_WCHAR( 'R', '\0' ), ONE_WCHAR( 'I', '\0' ), ONE_WCHAR( 'V', '\0' ),
    ONE_WCHAR( 'K', '\0' ), ONE_WCHAR( 'E', '\0' ), ONE_WCHAR( 'Y', '\0' ), ONE_WCHAR( '\0', '\0' )
};
const DRM_CONST_STRING g_dstrMSBinCert256KeyPRIVKEY = CREATE_DRM_STRING( g_rgwchMSBinCert256KeyPRIVKEY );

/* MSRSA1024CertKey/PRIVKEY/Prime0 */
const DRM_WCHAR g_rgwchMSRSA1024CertKeyPRIVKEYPrime0 [] =  {
    ONE_WCHAR( 'M', '\0' ), ONE_WCHAR( 'S', '\0' ), ONE_WCHAR( 'R', '\0' ), ONE_WCHAR( 'S', '\0' ),
    ONE_WCHAR( 'A', '\0' ), ONE_WCHAR( '1', '\0' ), ONE_WCHAR( '0', '\0' ), ONE_WCHAR( '2', '\0' ),
    ONE_WCHAR( '4', '\0' ), ONE_WCHAR( 'C', '\0' ), ONE_WCHAR( 'e', '\0' ), ONE_WCHAR( 'r', '\0' ),
    ONE_WCHAR( 't', '\0' ), ONE_WCHAR( 'K', '\0' ), ONE_WCHAR( 'e', '\0' ), ONE_WCHAR( 'y', '\0' ),
    ONE_WCHAR( '/', '\0' ), ONE_WCHAR( 'P', '\0' ), ONE_WCHAR( 'R', '\0' ), ONE_WCHAR( 'I', '\0' ),
    ONE_WCHAR( 'V', '\0' ), ONE_WCHAR( 'K', '\0' ), ONE_WCHAR( 'E', '\0' ), ONE_WCHAR( 'Y', '\0' ),
    ONE_WCHAR( '/', '\0' ), ONE_WCHAR( 'P', '\0' ), ONE_WCHAR( 'r', '\0' ), ONE_WCHAR( 'i', '\0' ),
    ONE_WCHAR( 'm', '\0' ), ONE_WCHAR( 'e', '\0' ), ONE_WCHAR( '0', '\0' ), ONE_WCHAR( '\0', '\0' )
};
const DRM_CONST_STRING g_dstrMSRSA1024CertKeyPRIVKEYPrime0 = CREATE_DRM_STRING( g_rgwchMSRSA1024CertKeyPRIVKEYPrime0 );

/* MSRSA1024CertKey/PRIVKEY/Prime1 */
const DRM_WCHAR g_rgwchMSRSA1024CertKeyPRIVKEYPrime1 [] =  {
    ONE_WCHAR( 'M', '\0' ), ONE_WCHAR( 'S', '\0' ), ONE_WCHAR( 'R', '\0' ), ONE_WCHAR( 'S', '\0' ),
    ONE_WCHAR( 'A', '\0' ), ONE_WCHAR( '1', '\0' ), ONE_WCHAR( '0', '\0' ), ONE_WCHAR( '2', '\0' ),
    ONE_WCHAR( '4', '\0' ), ONE_WCHAR( 'C', '\0' ), ONE_WCHAR( 'e', '\0' ), ONE_WCHAR( 'r', '\0' ),
    ONE_WCHAR( 't', '\0' ), ONE_WCHAR( 'K', '\0' ), ONE_WCHAR( 'e', '\0' ), ONE_WCHAR( 'y', '\0' ),
    ONE_WCHAR( '/', '\0' ), ONE_WCHAR( 'P', '\0' ), ONE_WCHAR( 'R', '\0' ), ONE_WCHAR( 'I', '\0' ),
    ONE_WCHAR( 'V', '\0' ), ONE_WCHAR( 'K', '\0' ), ONE_WCHAR( 'E', '\0' ), ONE_WCHAR( 'Y', '\0' ),
    ONE_WCHAR( '/', '\0' ), ONE_WCHAR( 'P', '\0' ), ONE_WCHAR( 'r', '\0' ), ONE_WCHAR( 'i', '\0' ),
    ONE_WCHAR( 'm', '\0' ), ONE_WCHAR( 'e', '\0' ), ONE_WCHAR( '1', '\0' ), ONE_WCHAR( '\0', '\0' )
};
const DRM_CONST_STRING g_dstrMSRSA1024CertKeyPRIVKEYPrime1 = CREATE_DRM_STRING( g_rgwchMSRSA1024CertKeyPRIVKEYPrime1 );

/* MSRSA1024CertKey/PRIVKEY/CRTExponent0 */
const DRM_WCHAR g_rgwchMSRSA1024CertKeyPRIVKEYCRTExponent0 [] =  {
    ONE_WCHAR( 'M', '\0' ), ONE_WCHAR( 'S', '\0' ), ONE_WCHAR( 'R', '\0' ), ONE_WCHAR( 'S', '\0' ),
    ONE_WCHAR( 'A', '\0' ), ONE_WCHAR( '1', '\0' ), ONE_WCHAR( '0', '\0' ), ONE_WCHAR( '2', '\0' ),
    ONE_WCHAR( '4', '\0' ), ONE_WCHAR( 'C', '\0' ), ONE_WCHAR( 'e', '\0' ), ONE_WCHAR( 'r', '\0' ),
    ONE_WCHAR( 't', '\0' ), ONE_WCHAR( 'K', '\0' ), ONE_WCHAR( 'e', '\0' ), ONE_WCHAR( 'y', '\0' ),
    ONE_WCHAR( '/', '\0' ), ONE_WCHAR( 'P', '\0' ), ONE_WCHAR( 'R', '\0' ), ONE_WCHAR( 'I', '\0' ),
    ONE_WCHAR( 'V', '\0' ), ONE_WCHAR( 'K', '\0' ), ONE_WCHAR( 'E', '\0' ), ONE_WCHAR( 'Y', '\0' ),
    ONE_WCHAR( '/', '\0' ), ONE_WCHAR( 'C', '\0' ), ONE_WCHAR( 'R', '\0' ), ONE_WCHAR( 'T', '\0' ),
    ONE_WCHAR( 'E', '\0' ), ONE_WCHAR( 'x', '\0' ), ONE_WCHAR( 'p', '\0' ), ONE_WCHAR( 'o', '\0' ),
    ONE_WCHAR( 'n', '\0' ), ONE_WCHAR( 'e', '\0' ), ONE_WCHAR( 'n', '\0' ), ONE_WCHAR( 't', '\0' ),
    ONE_WCHAR( '0', '\0' ), ONE_WCHAR( '\0', '\0' )
};
const DRM_CONST_STRING g_dstrMSRSA1024CertKeyPRIVKEYCRTExponent0 = CREATE_DRM_STRING( g_rgwchMSRSA1024CertKeyPRIVKEYCRTExponent0 );

/* MSRSA1024CertKey/PRIVKEY/CRTExponent1 */
const DRM_WCHAR g_rgwchMSRSA1024CertKeyPRIVKEYCRTExponent1 [] =  {
    ONE_WCHAR( 'M', '\0' ), ONE_WCHAR( 'S', '\0' ), ONE_WCHAR( 'R', '\0' ), ONE_WCHAR( 'S', '\0' ),
    ONE_WCHAR( 'A', '\0' ), ONE_WCHAR( '1', '\0' ), ONE_WCHAR( '0', '\0' ), ONE_WCHAR( '2', '\0' ),
    ONE_WCHAR( '4', '\0' ), ONE_WCHAR( 'C', '\0' ), ONE_WCHAR( 'e', '\0' ), ONE_WCHAR( 'r', '\0' ),
    ONE_WCHAR( 't', '\0' ), ONE_WCHAR( 'K', '\0' ), ONE_WCHAR( 'e', '\0' ), ONE_WCHAR( 'y', '\0' ),
    ONE_WCHAR( '/', '\0' ), ONE_WCHAR( 'P', '\0' ), ONE_WCHAR( 'R', '\0' ), ONE_WCHAR( 'I', '\0' ),
    ONE_WCHAR( 'V', '\0' ), ONE_WCHAR( 'K', '\0' ), ONE_WCHAR( 'E', '\0' ), ONE_WCHAR( 'Y', '\0' ),
    ONE_WCHAR( '/', '\0' ), ONE_WCHAR( 'C', '\0' ), ONE_WCHAR( 'R', '\0' ), ONE_WCHAR( 'T', '\0' ),
    ONE_WCHAR( 'E', '\0' ), ONE_WCHAR( 'x', '\0' ), ONE_WCHAR( 'p', '\0' ), ONE_WCHAR( 'o', '\0' ),
    ONE_WCHAR( 'n', '\0' ), ONE_WCHAR( 'e', '\0' ), ONE_WCHAR( 'n', '\0' ), ONE_WCHAR( 't', '\0' ),
    ONE_WCHAR( '1', '\0' ), ONE_WCHAR( '\0', '\0' )
};
const DRM_CONST_STRING g_dstrMSRSA1024CertKeyPRIVKEYCRTExponent1 = CREATE_DRM_STRING( g_rgwchMSRSA1024CertKeyPRIVKEYCRTExponent1 );

/* MSRSA1024CertKey/PRIVKEY/IQMP */
const DRM_WCHAR g_rgwchMSRSA1024CertKeyPRIVKEYIQMP [] =  {
    ONE_WCHAR( 'M', '\0' ), ONE_WCHAR( 'S', '\0' ), ONE_WCHAR( 'R', '\0' ), ONE_WCHAR( 'S', '\0' ),
    ONE_WCHAR( 'A', '\0' ), ONE_WCHAR( '1', '\0' ), ONE_WCHAR( '0', '\0' ), ONE_WCHAR( '2', '\0' ),
    ONE_WCHAR( '4', '\0' ), ONE_WCHAR( 'C', '\0' ), ONE_WCHAR( 'e', '\0' ), ONE_WCHAR( 'r', '\0' ),
    ONE_WCHAR( 't', '\0' ), ONE_WCHAR( 'K', '\0' ), ONE_WCHAR( 'e', '\0' ), ONE_WCHAR( 'y', '\0' ),
    ONE_WCHAR( '/', '\0' ), ONE_WCHAR( 'P', '\0' ), ONE_WCHAR( 'R', '\0' ), ONE_WCHAR( 'I', '\0' ),
    ONE_WCHAR( 'V', '\0' ), ONE_WCHAR( 'K', '\0' ), ONE_WCHAR( 'E', '\0' ), ONE_WCHAR( 'Y', '\0' ),
    ONE_WCHAR( '/', '\0' ), ONE_WCHAR( 'I', '\0' ), ONE_WCHAR( 'Q', '\0' ), ONE_WCHAR( 'M', '\0' ),
    ONE_WCHAR( 'P', '\0' ), ONE_WCHAR( '\0', '\0' )
};
const DRM_CONST_STRING g_dstrMSRSA1024CertKeyPRIVKEYIQMP = CREATE_DRM_STRING( g_rgwchMSRSA1024CertKeyPRIVKEYIQMP );

/*********************************
** DRM-related ASF Header GUIDs **
*********************************/

/* ASF_Header_Object    75B22630-668E-11CF-A6D9-00AA0062CE6C */
const DRM_GUID g_guidASFHeaderObject = {
    0x75B22630,
    0x668E,
    0x11CF,
    {
        0xA6, 0xD9, 0x00, 0xAA,
        0x00, 0x62, 0xCE, 0x6C
    }
};

/* ASF_Data_Object    75B22636-668E-11CF-A6D9-00AA0062CE6C */
const DRM_GUID g_guidASFDataObject = {
    0x75B22636,
    0x668E,
    0x11CF,
    {
        0xA6, 0xD9, 0x00, 0xAA,
        0x00, 0x62, 0xCE, 0x6C
    }
};

/* ASF_Content_Encryption_Object    2211B3FB-BD23-11D2-B4B7-00A0C955FC6E */
const DRM_GUID g_guidASFContentEncryptionObject = {
    0x2211B3FB,
    0xBD23,
    0x11D2,
    {
        0xB4, 0xB7, 0x00, 0xA0,
        0xC9, 0x55, 0xFC, 0x6E
    }
};

/* ASF_Extended_Content_Encryption_Object    298AE614-2622-4C17-B935-DAE07EE9289C */
const DRM_GUID g_guidASFExtendedContentEncryptionObject = {
    0x298AE614,
    0x2622,
    0x4C17,
    {
        0xB9, 0x35, 0xDA, 0xE0,
        0x7E, 0xE9, 0x28, 0x9C
    }
};

/* ASF_Header_Extension_Object    5FBF03B5-A92E-11CF-8EE3-00C00C205365 */
const DRM_GUID g_guidASFHeaderExtensionObject = {
    0x5FBF03B5,
    0xA92E,
    0x11CF,
    {
        0x8E, 0xE3, 0x00, 0xC0,
        0x0C, 0x20, 0x53, 0x65
    }
};

/* ASF_Advanced_Content_Encryption_Object    43058533-6981-49E6-9B74-AD12CB86D58C */
const DRM_GUID g_guidASFAdvancedContentEncryptionObject = {
    0x43058533,
    0x6981,
    0x49E6,
    {
        0x9B, 0x74, 0xAD, 0x12,
        0xCB, 0x86, 0xD5, 0x8C
    }
};

/* ASF_V4_Content_Encryption_Object      9a04f079-9840-4286-ab92e65be0885f95 */
const DRM_GUID g_guidASFV4ContentEncryptionObject = {
    0x9A04F079,
    0x9840,
    0x4286,
    {
        0xAB, 0x92, 0xE6, 0x5B,
        0xE0, 0x88, 0x5F, 0x95
    }
};

/* ASF_File_Properties_Object    8CABDCA1-A947-11CF-8EE4-00C00C205365 */
const DRM_GUID g_guidASFFilePropertiesObject = {
    0x8CABDCA1,
    0xA947,
    0x11CF,
    {
        0x8E, 0xE4, 0x00, 0xC0,
        0x0C, 0x20, 0x53, 0x65
    }
};

/* ASF_Stream_Properties_Object    b7dc079-A9b7-11CF-8EE6-00C00C205365 */
const DRM_GUID g_guidASFStreamPropertiesObject = {
    0xb7dc0791,
    0xa9b7,
    0x11cf,
    {
        0x8e, 0xe6, 0x00, 0xc0,
        0x0c, 0x20, 0x53, 0x65
    }
};
/* ASF_Extended_Stream_Properties_Object    14e6a5cb-c672-4332-8399-a96952065b5a */
const DRM_GUID g_guidASFStreamPropertiesObjectEx = {
    0x14e6a5cb,
    0xc672,
    0x4332,
    {
        0x83, 0x99, 0xa9, 0x69,
        0x52, 0x06, 0x5b, 0x5a
    }
};
/* ASF_Digital_Signature_Object    2211B3FC-BD23-11D2-B4B7-00A0C955FC6E */
const DRM_GUID g_guidASFDigitalSignatureObject = {
    0x2211B3FC,
    0xBD23,
    0x11D2,
    {
        0xB4, 0xB7, 0x00, 0xA0,
        0xC9, 0x55, 0xFC, 0x6E
    }
};

/* SystemID_PlayReady    f4637010-03c3-42cd-b932b48adf3a6a54 */
const DRM_GUID g_guidSystemIDPlayReady = {
    0xF4637010,
    0x03C3,
    0x42CD,
    {
        0xB9, 0x32, 0xB4, 0x8A,
        0xDF, 0x3A, 0x6A, 0x54
    }
};

/* ASF_Reserved_1    ABD3D211-A9BA-11cf-8EE6-00C00C205365 */
const DRM_GUID g_guidASFReserved1 = {
    0xABD3D211,
    0xA9BA,
    0x11CF,
    {
        0x8E, 0xE6, 0x00, 0xC0,
        0x0C, 0x20, 0x53, 0x65
    }
};

/*
** ASF Stream Types
*/
/* Audio f8699e40-5b4d-11cf-a8fd-00805f5c442b */
const DRM_GUID g_guidASFXStreamTypeAcmAudio = {
    0xf8699e40,
    0x5b4d,
    0x11cf,
    {
        0xa8, 0xfd, 0x00, 0x80,
        0x5f, 0x5c, 0x44, 0x2b
    }
};
/* Video bc19efc0-5b4d-11cf-a8fd-00805f5c442b */
const DRM_GUID g_guidASFXStreamTypeIcmVideo = {
    0xbc19efc0,
    0x5b4d,
    0x11cf,
    {
        0xa8, 0xfd, 0x00, 0x80,
        0x5f, 0x5c, 0x44, 0x2b
    }
};

/*
** Constants for header evaluation
*/
const DRM_WCHAR      g_rgwchTagV24LAURL     [] =  { ONE_WCHAR('L', '\0'), ONE_WCHAR('A', '\0'), ONE_WCHAR('_', '\0'), ONE_WCHAR('U', '\0'), ONE_WCHAR('R', '\0'), ONE_WCHAR('L', '\0'), ONE_WCHAR('\0', '\0')}; /* "LA_URL" */
const DRM_WCHAR      g_rgwchTagV24LUIURL    [] =  { ONE_WCHAR('L', '\0'), ONE_WCHAR('U', '\0'), ONE_WCHAR('I', '\0'), ONE_WCHAR('_', '\0'), ONE_WCHAR('U', '\0'), ONE_WCHAR('R', '\0'), ONE_WCHAR('L', '\0'), ONE_WCHAR('\0', '\0')}; /* "LUI_URL" */

const DRM_CONST_STRING      g_dstrTagV24LAURL    = CREATE_DRM_STRING( g_rgwchTagV24LAURL );
const DRM_CONST_STRING      g_dstrTagV24LUIURL   = CREATE_DRM_STRING( g_rgwchTagV24LUIURL );

/* Profiling string constants */
DRM_SCOPE g_Scopes[] =
{
    /* blackbox\blackbox.c */
    {"DRM_BBX_CipherKeySetup", PERF_SCOPE_ID(PERF_MOD_BLACKBOX, PERF_FUNC_DRM_BBX_CipherKeySetup, 1), 0, 0},
    {"DRM_BBX_HashValue", PERF_SCOPE_ID(PERF_MOD_BLACKBOX, PERF_FUNC_DRM_BBX_HashValue, 1), 0, 0},
    {"DRM_BBX_SymmetricVerify", PERF_SCOPE_ID(PERF_MOD_BLACKBOX, PERF_FUNC_DRM_BBX_SymmetricVerify, 1), 0, 0},
    {"DRM_BBX_Initialize", PERF_SCOPE_ID(PERF_MOD_BLACKBOX, PERF_FUNC_DRM_BBX_Initialize, 1), 0, 0},
    {"DRM_BBX_CanBind", PERF_SCOPE_ID(PERF_MOD_BLACKBOX, PERF_FUNC_DRM_BBX_CanBind, 1), 0, 0},
    {"_DecryptContentKeyXML", PERF_SCOPE_ID(PERF_MOD_BLACKBOX, PERF_FUNC__DecryptContentKeyXML, 1), 0, 0},
    {"VerifyChecksum", PERF_SCOPE_ID(PERF_MOD_BLACKBOX, PERF_FUNC_VerifyChecksum, 1), 0, 0},
    {"_GetDevicePrivkey", PERF_SCOPE_ID(PERF_MOD_BLACKBOX, PERF_FUNC__GetDevicePrivkey, 1), 0, 0},
    {"DRM_BBX_RebindSLK", PERF_SCOPE_ID(PERF_MOD_BLACKBOX, PERF_FUNC_DRM_BBX_RebindSLK, 1), 0, 0},
    {"DRM_BBX_DecryptLicense", PERF_SCOPE_ID(PERF_MOD_BLACKBOX, PERF_FUNC_DRM_BBX_DecryptLicense, 1), 0, 0},
    {"DRM_BBX_SymmetricCipher", PERF_SCOPE_ID(PERF_MOD_BLACKBOX, PERF_FUNC_DRM_BBX_SymmetricCipher, 1), 0, 0},

    /* core\drmblobcache.c */
    {"DRM_BLOBCACHE_Verify", PERF_SCOPE_ID(PERF_MOD_BLOBCACHE, PERF_FUNC_DRM_BLOBCACHE_Verify, 1), 0, 0},
    {"DRM_BLOBCACHE_Update", PERF_SCOPE_ID(PERF_MOD_BLOBCACHE, PERF_FUNC_DRM_BLOBCACHE_Update, 1), 0, 0},

    /* core\drmchain.c */
    {"_PerformActions", PERF_SCOPE_ID(PERF_MOD_DRMCHAIN, PERF_FUNC__PerformActions, 1), 0, 0},
    {"_ProcessEndOfChain", PERF_SCOPE_ID(PERF_MOD_DRMCHAIN, PERF_FUNC__ProcessEndOfChain, 1), 0, 0},
    {"DRM_LIC_CompleteLicenseChain", PERF_SCOPE_ID(PERF_MOD_DRMCHAIN, PERF_FUNC_DRM_LIC_CompleteLicenseChain, 1), 0, 0},
    {"DRM_LIC_ReportActions", PERF_SCOPE_ID(PERF_MOD_DRMCHAIN, PERF_FUNC_DRM_LIC_ReportActions, 1), 0, 0},
    {"DRM_LIC_CheckClockRollback", PERF_SCOPE_ID(PERF_MOD_DRMCHAIN, PERF_FUNC_DRM_LIC_CheckClockRollback, 1), 2473, 2474},
    {"_GetLicenseInfoAndSetup", PERF_SCOPE_ID(PERF_MOD_DRMCHAIN, PERF_FUNC__GetLicenseInfoAndSetup, 1), 0, 0},
    {"_ValidateRevocationVersions", PERF_SCOPE_ID(PERF_MOD_DRMCHAIN, PERF_FUNC__ValidateRevocationVersions, 1), 0, 0},
    {"DRM_WMDRM_LIC_GetInclusionList", PERF_SCOPE_ID(PERF_MOD_DRMCHAIN, PERF_FUNC_DRM_WMDRM_LIC_GetInclusionList, 1), 0, 0},

    /* core\drmhds.c */
    {"DRM_HDS_OpenStore", PERF_SCOPE_ID(PERF_MOD_DRMHDS, PERF_FUNC_DRM_HDS_OpenStore, 1), 0, 0},
    {"DRM_HDS_CloseStore", PERF_SCOPE_ID(PERF_MOD_DRMHDS, PERF_FUNC_DRM_HDS_CloseStore, 1), 0, 0},
    {"DRM_HDS_OpenNamespace", PERF_SCOPE_ID(PERF_MOD_DRMHDS, PERF_FUNC_DRM_HDS_OpenNamespace, 1), 2472, 0},
    {"DRM_HDS_OpenSlot", PERF_SCOPE_ID(PERF_MOD_DRMHDS, PERF_FUNC_DRM_HDS_OpenSlot, 1), 0, 0},
    {"DRM_HDS_InitSlotEnum", PERF_SCOPE_ID(PERF_MOD_DRMHDS, PERF_FUNC_DRM_HDS_InitSlotEnum, 1), 0, 0},
    {"DRM_HDS_SlotEnumNext", PERF_SCOPE_ID(PERF_MOD_DRMHDS, PERF_FUNC_DRM_HDS_SlotEnumNext, 1), 0, 0},

    /* core\drmhds.c */
    {"_HdsGetPutBlockHDR", PERF_SCOPE_ID(PERF_MOD_DRMHDSIMPL, PERF_FUNC__HdsGetPutBlockHDR, 1), 0, 0},
    {"_HdsGetPutBlockPayload", PERF_SCOPE_ID(PERF_MOD_DRMHDSIMPL, PERF_FUNC__HdsGetPutBlockPayload, 1), 2466, 0},
    {"_HdsSearchSlotInFile", PERF_SCOPE_ID(PERF_MOD_DRMHDSIMPL, PERF_FUNC__HdsSearchSlotInFile, 1), 0, 0},
    {"_HdsHashToChildBlock", PERF_SCOPE_ID(PERF_MOD_DRMHDSIMPL, PERF_FUNC__HdsHashToChildBlock, 1), 0, 0},
    {"_HdsSearchSlotInBlock", PERF_SCOPE_ID(PERF_MOD_DRMHDSIMPL, PERF_FUNC__HdsSearchSlotInBlock, 1), 0, 0},
    {"_HdsCopyBlockBuffer", PERF_SCOPE_ID(PERF_MOD_DRMHDSIMPL, PERF_FUNC__HdsCopyBlockBuffer, 1), 0, 0},
    {"DRM_HDSIMPL_AllocBlockBuffer", PERF_SCOPE_ID(PERF_MOD_DRMHDSIMPL, PERF_FUNC_DRM_HDSIMPL_AllocBlockBuffer, 1), 0, 0},
    {"_HdsHashKeyToIndex", PERF_SCOPE_ID(PERF_MOD_DRMHDSIMPL, PERF_FUNC__HdsHashKeyToIndex, 1), 0, 0},
    {"_HdsLoadBlockHDR", PERF_SCOPE_ID(PERF_MOD_DRMHDSIMPL, PERF_FUNC__HdsLoadBlockHDR, 1), 0, 0},
    {"_HdsLoadSRN", PERF_SCOPE_ID(PERF_MOD_DRMHDSIMPL, PERF_FUNC__HdsLoadSRN, 1), 0, 0},
    {"_HdsGenSRNHash", PERF_SCOPE_ID(PERF_MOD_DRMHDSIMPL, PERF_FUNC__HdsGenSRNHash, 1), 0, 0},
    {"_WriteSRN", PERF_SCOPE_ID(PERF_MOD_DRMHDSIMPL, PERF_FUNC__WriteSRN, 1), 0, 0},
    {"_ReadSRN", PERF_SCOPE_ID(PERF_MOD_DRMHDSIMPL, PERF_FUNC__ReadSRN, 1), 0, 0},
    {"_WriteCommonBlockHeader", PERF_SCOPE_ID(PERF_MOD_DRMHDSIMPL, PERF_FUNC__WriteCommonBlockHeader, 1), 0, 0},
    {"_ReadCommonBlockHeader", PERF_SCOPE_ID(PERF_MOD_DRMHDSIMPL, PERF_FUNC__ReadCommonBlockHeader, 1), 0, 0},
    {"_WriteChildBlockHeader", PERF_SCOPE_ID(PERF_MOD_DRMHDSIMPL, PERF_FUNC__WriteChildBlockHeader, 1), 0, 0},
    {"_ReadChildBlockHeader", PERF_SCOPE_ID(PERF_MOD_DRMHDSIMPL, PERF_FUNC__ReadChildBlockHeader, 1), 0, 0},
    {"_WriteDataBlockHeader", PERF_SCOPE_ID(PERF_MOD_DRMHDSIMPL, PERF_FUNC__WriteDataBlockHeader, 1), 0, 0},
    {"_ReadDataBlockHeader", PERF_SCOPE_ID(PERF_MOD_DRMHDSIMPL, PERF_FUNC__ReadDataBlockHeader, 1), 0, 0},
    {"_HdsGenBlockHash", PERF_SCOPE_ID(PERF_MOD_DRMHDSIMPL, PERF_FUNC__HdsGenBlockHash, 1), 0, 0},
    {"_HdsGetPutChildBlockNum", PERF_SCOPE_ID(PERF_MOD_DRMHDSIMPL, PERF_FUNC__HdsGetPutChildBlockNum, 1), 0, 0},
    {"_HdsExpandStore", PERF_SCOPE_ID(PERF_MOD_DRMHDSIMPL, PERF_FUNC__HdsExpandStore, 1), 0, 0},
    {"_HdsUpdateSRN", PERF_SCOPE_ID(PERF_MOD_DRMHDSIMPL, PERF_FUNC__HdsUpdateSRN, 1), 2649, 0},
    {"_HdsAllocBlock", PERF_SCOPE_ID(PERF_MOD_DRMHDSIMPL, PERF_FUNC__HdsAllocBlock, 1), 0, 0},
    {"_HdsFreeBlock", PERF_SCOPE_ID(PERF_MOD_DRMHDSIMPL, PERF_FUNC__HdsFreeBlock, 1), 0, 0},
    {"_HdsLocateFreeBlockForSlot", PERF_SCOPE_ID(PERF_MOD_DRMHDSIMPL, PERF_FUNC__HdsLocateFreeBlockForSlot, 1), 0, 0},
    {"_HdsCreateOversizedSlot", PERF_SCOPE_ID(PERF_MOD_DRMHDSIMPL, PERF_FUNC__HdsCreateOversizedSlot, 1), 0, 0},
    {"_HdsOpenSlotFromHint", PERF_SCOPE_ID(PERF_MOD_DRMHDSIMPL, PERF_FUNC__HdsOpenSlotFromHint, 1), 0, 0},
    {"_HdsAllocSlotInFile", PERF_SCOPE_ID(PERF_MOD_DRMHDSIMPL, PERF_FUNC__HdsAllocSlotInFile, 1), 0, 0},
    {"_HdsOpenSlot", PERF_SCOPE_ID(PERF_MOD_DRMHDSIMPL, PERF_FUNC__HdsOpenSlot, 1), 0, 0},
    {"_HdsVerifySlotContext", PERF_SCOPE_ID(PERF_MOD_DRMHDSIMPL, PERF_FUNC__HdsVerifySlotContext, 1), 0, 0},
    {"_HdsReadWriteSlot", PERF_SCOPE_ID(PERF_MOD_DRMHDSIMPL, PERF_FUNC__HdsReadWriteSlot, 1), 0, 0},
    {"_HdsAdjustChildPayload", PERF_SCOPE_ID(PERF_MOD_DRMHDSIMPL, PERF_FUNC__HdsAdjustChildPayload, 1), 0, 0},
    {"_HdsFindLeftmostLeafBlock", PERF_SCOPE_ID(PERF_MOD_DRMHDSIMPL, PERF_FUNC__HdsFindLeftmostLeafBlock, 1), 0, 0},
    {"_HdsCopySlot_Child2Child", PERF_SCOPE_ID(PERF_MOD_DRMHDSIMPL, PERF_FUNC__HdsCopySlot_Child2Child, 1), 0, 0},
    {"_HdsCopyChildPayload", PERF_SCOPE_ID(PERF_MOD_DRMHDSIMPL, PERF_FUNC__HdsCopyChildPayload, 1), 0, 0},
    {"_HdsDefragmentFile", PERF_SCOPE_ID(PERF_MOD_DRMHDSIMPL, PERF_FUNC__HdsDefragmentFile, 1), 0, 0},
    {"_HdsDeleteSubTree", PERF_SCOPE_ID(PERF_MOD_DRMHDSIMPL, PERF_FUNC__HdsDeleteSubTree, 1), 0, 0},
    {"_HdsTraverseNextRightSiblingBlock", PERF_SCOPE_ID(PERF_MOD_DRMHDSIMPL, PERF_FUNC__HdsTraverseNextRightSiblingBlock, 1), 0, 0},
    {"_HdsTraverseBlocksInPostOrder", PERF_SCOPE_ID(PERF_MOD_DRMHDSIMPL, PERF_FUNC__HdsTraverseBlocksInPostOrder, 1), 0, 0},

    /* modules\dla\real\drmdlaimplreal.c */
    {"DRM_LA_CreateChallenge", PERF_SCOPE_ID(PERF_MOD_DRMLICACQREQ, PERF_FUNC_DRM_LA_CreateChallenge, 1), 0, 0},
    {"DRM_LA_GetClientInfo", PERF_SCOPE_ID(PERF_MOD_DRMLICACQREQ, PERF_FUNC_DRM_LA_GetClientInfo, 1), 0, 0},

    /* source\modules\wmdrm\real\* */
    {"DRM_WMDRM_LA_ProcessResponse", PERF_SCOPE_ID(PERF_MOD_DRMLICACQRESP, PERF_FUNC_DRM_WMDRM_LA_ProcessResponse, 1), 2477, 2478},

    /* core\drmliceval.c */
    /* modules\contentrevocation\real\contentrevocationlevlreal.c */
    /* source\modules\wmdrm\real\* */
    /* core\drmexpreval.c */
    {"_PerformOperationsXML", PERF_SCOPE_ID(PERF_MOD_DRMLICEVAL, PERF_FUNC__PerformOperationsXML, 1), 0, 0},
    {"DRM_WMDRM_LEVL_GetLicenseReasonForUnusable", PERF_SCOPE_ID(PERF_MOD_DRMLICEVAL, PERF_FUNC_DRM_WMDRM_LEVL_GetLicenseReasonForUnusable, 1), 0, 0},
    {"_Eval", PERF_SCOPE_ID(PERF_MOD_DRMLICEVAL, PERF_FUNC__Eval, 1), 0, 0},
    {"DRM_WMDRM_EXPR_LEVL_EvaluateExpression", PERF_SCOPE_ID(PERF_MOD_DRMLICEVAL, PERF_FUNC_DRM_WMDRM_EXPR_LEVL_EvaluateExpression, 1), 0, 0},
    {"DRM_LEVL_PerformOperationsXMR", PERF_SCOPE_ID(PERF_MOD_DRMLICEVAL, PERF_FUNC_DRM_LEVL_PerformOperationsXMR, 1), 0, 0},
    {"DRM_CONTENTREVOCATION_LEVL_SetOrUpdateContentRevocation", PERF_SCOPE_ID(PERF_MOD_DRMLICEVAL, PERF_FUNC_DRM_CONTENTREVOCATION_LEVL_SetOrUpdateContentRevocation, 1), 0, 0},

    /* modules\symopt\real\symoptlicreal.c */
    {"DRM_SYMOPT_LIC_UpdateLicenseWithSymmetricData", PERF_SCOPE_ID(PERF_MOD_DRMLICPARSER, PERF_FUNC_DRM_SYMOPT_LIC_UpdateLicenseWithSymmetricData, 1), 0, 0},
    {"_UpdateLicenseWithSymmetricKey", PERF_SCOPE_ID(PERF_MOD_DRMLICPARSER, PERF_FUNC__UpdateLicenseWithSymmetricKey, 1), 0, 0},
    {"_UpdateLicenseWithSymmetricSignature", PERF_SCOPE_ID(PERF_MOD_DRMLICPARSER, PERF_FUNC__UpdateLicenseWithSymmetricSignature, 1), 0, 0},

    /* core\drmlicstore.c */
    {"_InitEnum", PERF_SCOPE_ID(PERF_MOD_DRMLICSTORE, PERF_FUNC__InitEnum, 1), 0, 0},
    {"DRM_LST_EnumNext", PERF_SCOPE_ID(PERF_MOD_DRMLICSTORE, PERF_FUNC_DRM_LST_EnumNext, 1), 0, 0},
    {"DRM_LST_Open",PERF_SCOPE_ID(PERF_MOD_DRMLICSTORE, PERF_FUNC_DRM_LST_Open, 1), 0, 0},
    {"DRM_LST_Close",PERF_SCOPE_ID(PERF_MOD_DRMLICSTORE, PERF_FUNC_DRM_LST_Close, 1), 0, 0},
    {"DRM_LST_GetLicense",PERF_SCOPE_ID(PERF_MOD_DRMLICSTORE, PERF_FUNC_DRM_LST_GetLicense, 1), 0, 0},

    /* core\drmsecstore.c */
    {"DRM_SST_OpenKeyTokens", PERF_SCOPE_ID(PERF_MOD_DRMSECSTORE, PERF_FUNC_DRM_SST_OpenKeyTokens, 1), 2470, 2471},
    {"DRM_SST_CreateGlobalStorePassword", PERF_SCOPE_ID(PERF_MOD_DRMSECSTORE, PERF_FUNC_DRM_SST_CreateGlobalStorePassword, 1), 0, 0},
    {"DRM_SST_CreateLicenseStatePassword", PERF_SCOPE_ID(PERF_MOD_DRMSECSTORE, PERF_FUNC_DRM_SST_CreateLicenseStatePassword, 1), 0, 0},
    {"DRM_SST_CloseKey", PERF_SCOPE_ID(PERF_MOD_DRMSECSTORE, PERF_FUNC_DRM_SST_CloseKey, 1), 0, 0},
    {"DRM_SST_GetData", PERF_SCOPE_ID(PERF_MOD_DRMSECSTORE, PERF_FUNC_DRM_SST_GetData, 1), 0, 0},
    {"DRM_SST_SetData", PERF_SCOPE_ID(PERF_MOD_DRMSECSTORE, PERF_FUNC_DRM_SST_SetData, 1), 0, 0},
    {"DRM_SST_GetTokenValue", PERF_SCOPE_ID(PERF_MOD_DRMSECSTORE, PERF_FUNC_DRM_SST_GetTokenValue, 1), 0, 0},
    {"DRM_SST_SetExplicitResolutionTokenValue", PERF_SCOPE_ID(PERF_MOD_DRMSECSTORE, PERF_FUNC_DRM_SST_SetExplicitResolutionTokenValue, 1), 0, 0},
    {"_VerifySlotHash", PERF_SCOPE_ID(PERF_MOD_DRMSECSTORE, PERF_FUNC__VerifySlotHash, 1), 0, 0},

    /* core\drmplayreadyobj.c */
    {"DRM_PRO_Create ", PERF_SCOPE_ID(PERF_MOD_DRMPRO, PERF_FUNC_DRM_PRO_Create , 1), 0, 0},
    {"DRM_PRO_GetCipherTypeFromPlayReadyHeader", PERF_SCOPE_ID(PERF_MOD_DRMPRO, PERF_FUNC_DRM_PRO_GetCipherTypeFromPlayReadyHeader, 1), 0, 0},
    {"DRM_PRO_GetCipherType", PERF_SCOPE_ID(PERF_MOD_DRMPRO, PERF_FUNC_DRM_PRO_GetCipherType, 1), 0, 0},
    {"DRM_PRO_ConvertHeaderFromWmdrmToPlayReady", PERF_SCOPE_ID(PERF_MOD_DRMPRO, PERF_FUNC_DRM_PRO_ConvertHeaderFromWmdrmToPlayReady, 1), 0, 0},
    {"Drm_PlayReadyObject_ConvertFromWmdrmHeader", PERF_SCOPE_ID(PERF_MOD_DRMPRO, PERF_FUNC_Drm_PlayReadyObject_ConvertFromWmdrmHeader, 1), 0, 0},

    /* devcertparser\devcert.c */
    {"DRM_DCP_GetAttribute", PERF_SCOPE_ID(PERF_MOD_DEVCERT, PERF_FUNC_DRM_DCP_GetAttribute, 1), 0, 0},
    {"DRM_DCP_LoadPropertiesCache", PERF_SCOPE_ID(PERF_MOD_DEVCERT, PERF_FUNC_DRM_DCP_LoadPropertiesCache, 1), 0, 0},

    /* devicedevcert\devicedevcert.c */
    /* modules\symopt\real\symoptdevicedevcertreal.c */
    {"_CompareMachineId", PERF_SCOPE_ID(PERF_MOD_DEVICEDEVCERT, PERF_FUNC__CompareMachineId, 1), 0, 0},
    {"DRM_SYMOPT_DDC_VerifySymmetricSignature", PERF_SCOPE_ID(PERF_MOD_DEVICEDEVCERT, PERF_FUNC_DRM_SYMOPT_DDC_VerifySymmetricSignature, 1), 0, 0},
    {"DRM_DDC_Certificates_Initialize", PERF_SCOPE_ID(PERF_MOD_DEVICEDEVCERT, PERF_FUNC_DRM_DDC_Certificates_Initialize, 1), 0, 0},

    /* drmmanager\drmmanager.c */
    /* modules\secure_clock\real\secureclockmanager.c */
    {"_SetupLicEvalObjectToShare", PERF_SCOPE_ID(PERF_MOD_DRM_APP, PERF_FUNC__SetupLicEvalObjectToShare, 1), 2647, 2648},
    {"Drm_Initialize", PERF_SCOPE_ID(PERF_MOD_DRM_APP, PERF_FUNC_Drm_Initialize, 1), 2459, 2460},
    {"Drm_Uninitialize", PERF_SCOPE_ID(PERF_MOD_DRM_APP, PERF_FUNC_Drm_Uninitialize, 1), 0, 0},
    {"Drm_LicenseAcq_GenerateChallenge", PERF_SCOPE_ID(PERF_MOD_DRM_APP, PERF_FUNC_Drm_LicenseAcq_GenerateChallenge, 1), 2596, 2597},
    {"Drm_Device_GetProperty", PERF_SCOPE_ID(PERF_MOD_DRM_APP, PERF_FUNC_Drm_Device_GetProperty, 1), 0, 0},
    {"Drm_LicenseAcq_ProcessResponse", PERF_SCOPE_ID(PERF_MOD_DRM_APP, PERF_FUNC_Drm_LicenseAcq_ProcessResponse, 1), 2475, 2476},
    {"Drm_Reader_Bind", PERF_SCOPE_ID(PERF_MOD_DRM_APP, PERF_FUNC_Drm_Reader_Bind, 1), 2485, 2486},
    {"Drm_Reader_Commit", PERF_SCOPE_ID(PERF_MOD_DRM_APP, PERF_FUNC_Drm_Reader_Commit, 1), 2484, 2677},
    {"Drm_LicenseQuery_GetState", PERF_SCOPE_ID(PERF_MOD_DRM_APP, PERF_FUNC_Drm_LicenseQuery_GetState, 1), 2481, 2482},
    {"Drm_Sync_GenerateChallenge", PERF_SCOPE_ID(PERF_MOD_DRM_APP, PERF_FUNC_Drm_Sync_GenerateChallenge, 1), 2598, 2599},
    {"DRM_CLK_Drm_SecureClock_ProcessResponseImpl", PERF_SCOPE_ID(PERF_MOD_DRM_APP, PERF_FUNC_DRM_CLK_Drm_SecureClock_ProcessResponseImpl, 1), 0, 0},
    {"DRM_CLK_Drm_SecureClock_GenerateChallengeImpl", PERF_SCOPE_ID(PERF_MOD_DRM_APP, PERF_FUNC_DRM_CLK_Drm_SecureClock_GenerateChallengeImpl, 1), 0, 0},
    {"DRM_CLK_Drm_SecureClock_GenerateChallengeImpl", PERF_SCOPE_ID(PERF_MOD_DRM_APP, PERF_FUNC_DRM_CLK_Drm_SecureClock_GenerateChallengeImpl, 1), 0, 0},
    {"Drm_StoreMgmt_CleanupStore", PERF_SCOPE_ID(PERF_MOD_DRM_APP, PERF_FUNC_Drm_StoreMgmt_CleanupStore, 1), 2656, 2657},
    {"Drm_StoreMgmt_DeleteLicenses", PERF_SCOPE_ID(PERF_MOD_DRM_APP, PERF_FUNC_Drm_StoreMgmt_DeleteLicenses, 1), 2658, 2659},
    {"Drm_LicenseQuery_IsAllowed", PERF_SCOPE_ID(PERF_MOD_DRM_APP, PERF_FUNC_Drm_LicenseQuery_IsAllowed, 1), 2497, 0},
    {"DRM_CLK_CheckSecureClock", PERF_SCOPE_ID(PERF_MOD_DRM_APP, PERF_FUNC_DRM_CLK_CheckSecureClock, 1), 0, 0},
    {"_CreateDeviceStore", PERF_SCOPE_ID(PERF_MOD_DRM_APP, PERF_FUNC__CreateDeviceStore, 1), 0, 0},
    {"Drm_SyncListItem_InitEnum", PERF_SCOPE_ID(PERF_MOD_DRM_APP, PERF_FUNC_Drm_SyncListItem_InitEnum, 1), 0, 0},
    {"Drm_SyncListItem_EnumNext", PERF_SCOPE_ID(PERF_MOD_DRM_APP, PERF_FUNC_Drm_SyncListItem_EnumNext, 1), 0, 0},
    {"Drm_Envelope_Open", PERF_SCOPE_ID(PERF_MOD_DRM_APP, PERF_FUNC_Drm_Envelope_Open, 1), 0, 0},
    {"Drm_Envelope_InitializeRead", PERF_SCOPE_ID(PERF_MOD_DRM_APP, PERF_FUNC_Drm_Envelope_InitializeRead, 1), 0, 0},
    {"Drm_Envelope_Close", PERF_SCOPE_ID(PERF_MOD_DRM_APP, PERF_FUNC_Drm_Envelope_Close, 1), 0, 0},
    {"Drm_Envelope_GetSize", PERF_SCOPE_ID(PERF_MOD_DRM_APP, PERF_FUNC_Drm_Envelope_GetSize, 1), 0, 0},
    {"Drm_Envelope_Read", PERF_SCOPE_ID(PERF_MOD_DRM_APP, PERF_FUNC_Drm_Envelope_Read, 1), 2702, 2703},
    {"Drm_Envelope_Seek", PERF_SCOPE_ID(PERF_MOD_DRM_APP, PERF_FUNC_Drm_Envelope_Seek, 1), 0, 0},
    {"Drm_License_GetProperty", PERF_SCOPE_ID(PERF_MOD_DRM_APP, PERF_FUNC_Drm_License_GetProperty, 1), 2700,2701},
    {"Drm_Content_UpdateEmbeddedStore", PERF_SCOPE_ID(PERF_MOD_DRM_APP, PERF_FUNC_Drm_Content_UpdateEmbeddedStore, 1), 0, 0},
    {"Drm_Content_GetProperty", PERF_SCOPE_ID(PERF_MOD_DRM_APP, PERF_FUNC_Drm_Content_GetProperty, 1), 0, 0},
    {"Drm_Content_UpdateEmbeddedStore_Commit", PERF_SCOPE_ID(PERF_MOD_DRM_APP, PERF_FUNC_Drm_Content_UpdateEmbeddedStore_Commit, 1), 0, 0},
    {"Drm_Revocation_SetBuffer", PERF_SCOPE_ID(PERF_MOD_DRM_APP, PERF_FUNC_Drm_Revocation_SetBuffer, 1), 0, 0},
    {"Drm_Activation_ProcessResponseGenerateChallenge", PERF_SCOPE_ID( PERF_MOD_ACTIVATION, PERF_FUNC_Drm_Activation_ProcessResponseGenerateChallenge, 1 ), 0, 0 },

    /* modules\metering\real\drmmeterimpv1.c */
    /* modules\metering\real\drmmeterimp.c */
    /* modules\metering\real\drmmeterapi.c */
    {"DRM_MTR_UpdateData", PERF_SCOPE_ID(PERF_MOD_METERING, PERF_FUNC_DRM_MTR_UpdateData, 1), 0, 0},
    {"DRM_MTR_ProcessMeterResponse", PERF_SCOPE_ID(PERF_MOD_METERING, PERF_FUNC_DRM_MTR_ProcessMeterResponse, 1), 0, 0},
    {"_BuildMeterCertChallengeXML", PERF_SCOPE_ID(PERF_MOD_METERING, PERF_FUNC__BuildMeterCertChallengeXML, 1), 0, 0},
    {"_ProcessMeterCertInResponse", PERF_SCOPE_ID(PERF_MOD_METERING, PERF_FUNC__ProcessMeterCertInResponse, 1), 0, 0},
    {"DRM_MTR_GenerateMeterCertChallenge", PERF_SCOPE_ID(PERF_MOD_METERING, PERF_FUNC_DRM_MTR_GenerateMeterCertChallenge, 1), 0, 0},
    {"DRM_MTR_ProcessMeterCertResponse", PERF_SCOPE_ID(PERF_MOD_METERING, PERF_FUNC_DRM_MTR_ProcessMeterCertResponse, 1), 0, 0},
    {"Drm_Metering_GenerateChallenge", PERF_SCOPE_ID(PERF_MOD_METERING, PERF_FUNC_Drm_Metering_GenerateChallenge, 1), 2615, 2616},
    {"Drm_Metering_ProcessResponse", PERF_SCOPE_ID(PERF_MOD_METERING, PERF_FUNC_Drm_Metering_ProcessResponse, 1), 2617, 2618},
    {"Drm_MeterCert_Update", PERF_SCOPE_ID(PERF_MOD_METERING, PERF_FUNC_Drm_MeterCert_Update, 1), 2619, 2620},
    {"Drm_MeterCert_InitEnum", PERF_SCOPE_ID(PERF_MOD_METERING, PERF_FUNC_Drm_MeterCert_InitEnum, 1), 0, 0},
    {"Drm_MeterCert_EnumNext", PERF_SCOPE_ID(PERF_MOD_METERING, PERF_FUNC_Drm_MeterCert_EnumNext, 1), 0, 0},
    {"Drm_MeterCert_GenerateChallenge", PERF_SCOPE_ID( PERF_MOD_METERING, PERF_FUNC_Drm_MeterCert_GenerateChallenge, 1 ), 0, 0 },
    {"Drm_MeterCert_ProcessResponse", PERF_SCOPE_ID( PERF_MOD_METERING, PERF_FUNC_Drm_MeterCert_ProcessResponse, 1 ), 0, 0 },
    {"DRM_MTR_GenerateMeterDataChallenge", PERF_SCOPE_ID( PERF_MOD_METERING, PERF_FUNC_DRM_MTR_GenerateMeterDataChallenge, 1 ), 0, 0 },
    {"DRM_MTR_ProcessMeterDataResponse", PERF_SCOPE_ID( PERF_MOD_METERING, PERF_FUNC_DRM_MTR_ProcessMeterDataResponse, 1 ), 0, 0 },
    {"_ProcessMeterDataResponse", PERF_SCOPE_ID( PERF_MOD_METERING, PERF_FUNC__ProcessMeterDataResponse, 1 ), 0, 0 },
    {"_CalcMaximumChallengeCharCount", PERF_SCOPE_ID( PERF_MOD_METERING, PERF_FUNC__CalcMaximumChallengeCharCount, 1 ), 0, 0 },
    {"_CalcFixedUnencryptedChallengeCharCount", PERF_SCOPE_ID( PERF_MOD_METERING, PERF_FUNC__CalcFixedUnencryptedChallengeCharCount, 1 ), 0, 0 },
    {"_CalcFixedEncryptedChallengeCharCount", PERF_SCOPE_ID( PERF_MOD_METERING, PERF_FUNC__CalcFixedEncryptedChallengeCharCount, 1 ), 0, 0 },
    {"_CalcVariableChallengeCharCount", PERF_SCOPE_ID( PERF_MOD_METERING, PERF_FUNC__CalcVariableChallengeCharCount, 1 ), 0, 0 },
    {"_ProcessAllKIDs", PERF_SCOPE_ID( PERF_MOD_METERING, PERF_FUNC__ProcessAllKIDs, 1 ), 0, 0 },
    {"_CalcKIDDataCharCount", PERF_SCOPE_ID( PERF_MOD_METERING, PERF_FUNC__CalcKIDDataCharCount, 1 ), 0, 0 },
    {"_BuildKIDDataXML", PERF_SCOPE_ID( PERF_MOD_METERING, PERF_FUNC__BuildKIDDataXML, 1 ), 0, 0 },
    {"_BuildMeterDataChallengeXML", PERF_SCOPE_ID( PERF_MOD_METERING, PERF_FUNC__BuildMeterDataChallengeXML, 1 ), 0, 0 },
    {"_BuildMeterDataChallengeDataXML", PERF_SCOPE_ID( PERF_MOD_METERING, PERF_FUNC__BuildMeterDataChallengeDataXML, 1 ), 0, 0 },
    {"_PrepareMeterChallengeContext", PERF_SCOPE_ID( PERF_MOD_METERING, PERF_FUNC__PrepareMeterChallengeContext, 1 ), 0, 0 },
    { "DRM_MTR_ParseBinaryMeterCert", PERF_SCOPE_ID( PERF_MOD_METERING, PERF_FUNC_DRM_MTR_ParseBinaryMeterCert, 1 ), 0, 0 },
    {"_DecryptCipherData", PERF_SCOPE_ID( PERF_MOD_METERING, PERF_FUNC__DecryptCipherData, 1 ), 0, 0 },
    {"DRM_MCS_GetMeterCertByMID", PERF_SCOPE_ID( PERF_MOD_METERING, PERF_FUNC_Drm_GetMeterCert_By_MID, 1 ), 0, 0 },

    /* oem\ansi\oemfileio.c */
    {"Oem_File_SetFilePointer", PERF_SCOPE_ID(PERF_MOD_OEMFILEIO, PERF_FUNC_Oem_File_SetFilePointer, 1), 0, 0},
    {"Oem_File_Lock", PERF_SCOPE_ID(PERF_MOD_OEMFILEIO, PERF_FUNC_Oem_File_Lock, 1), 0, 0},
    {"Oem_File_Unlock", PERF_SCOPE_ID(PERF_MOD_OEMFILEIO, PERF_FUNC_Oem_File_Unlock, 1), 0, 0},
    {"Oem_File_Open", PERF_SCOPE_ID(PERF_MOD_OEMFILEIO, PERF_FUNC_Oem_File_Open, 1), 0, 0},
    {"Oem_File_Read", PERF_SCOPE_ID(PERF_MOD_OEMFILEIO, PERF_FUNC_Oem_File_Read, 1), 0, 0},
    {"Oem_File_Write", PERF_SCOPE_ID(PERF_MOD_OEMFILEIO, PERF_FUNC_Oem_File_Write, 1), 0, 0},
    {"Oem_File_GetSize", PERF_SCOPE_ID(PERF_MOD_OEMFILEIO, PERF_FUNC_Oem_File_GetSize, 1), 0, 0},
    {"Oem_File_FlushBuffers", PERF_SCOPE_ID(PERF_MOD_OEMFILEIO, PERF_FUNC_Oem_File_FlushBuffers, 1), 0, 0},

    /* modules\secure_clock\real\secureclockrequest.c */
    {"DRM_CLK_CreateChallenge", PERF_SCOPE_ID(PERF_MOD_SECURECLOCKREQUEST, PERF_FUNC_DRM_CLK_CreateChallenge, 1), 0, 0},

    /* secure_clock\secureclockresponse.c */
    {"DRM_CLK_ProcessResponse", PERF_SCOPE_ID(PERF_MOD_SECURECLOCKRESPONSE, PERF_FUNC_DRM_CLK_ProcessResponse, 1), 0, 0},

    /* modules\licensesync\real\drmgensyncchallenge.c */
    {"DRM_SNC_GenerateSyncChallenge", PERF_SCOPE_ID(PERF_MOD_DRMGENSYNCCHALLENGE, PERF_FUNC_DRM_SNC_GenerateSyncChallenge, 1), 0, 0},

    /* source\modules\wmdrm\real\* */
    {"DRM_WMDRM_LIC_VerifySignature", PERF_SCOPE_ID(PERF_MOD_DRMLICPARSER, PERF_FUNC_DRM_WMDRM_LIC_VerifySignature, 1), 0, 0},
    {"DRM_WMDRM_LIC_VerifyCertChain", PERF_SCOPE_ID(PERF_MOD_DRMLICPARSER, PERF_FUNC_DRM_WMDRM_LIC_VerifyCertChain, 1), 2479, 2480},

    /* oem\common\ecc\base\oemeccp160.c */
    {"OEM_ECC_Verify_P160", PERF_SCOPE_ID(PERF_MOD_DRM_ECC_P160, PERF_FUNC_OEM_ECC_Verify_P160, 1), 0, 0},
    {"OEM_ECC_GenKeyPair_P160", PERF_SCOPE_ID(PERF_MOD_DRM_ECC_P160, PERF_FUNC_OEM_ECC_GenKeyPair_P160, 1), 0, 0},
    {"OEM_ECC_Sign_P160", PERF_SCOPE_ID(PERF_MOD_DRM_ECC_P160, PERF_FUNC_OEM_ECC_Sign_P160, 1), 0, 0},
    {"OEM_ECC_Decrypt_P160", PERF_SCOPE_ID(PERF_MOD_DRM_ECC_P160, PERF_FUNC_OEM_ECC_Decrypt_P160, 1), 0, 0},

    /* modules\licensesync\real\drmsyncstore.c */
    {"DRM_SNC_GetSyncStoreEntry", PERF_SCOPE_ID(PERF_MOD_DRMSYNCSTORE, PERF_FUNC_DRM_SNC_GetSyncStoreEntry, 1), 0, 0},
    {"DRM_SNC_UpdateKID", PERF_SCOPE_ID(PERF_MOD_DRMSYNCSTORE, PERF_FUNC_DRM_SNC_UpdateKID, 1), 0, 0},

    /* core\drmutilities.c */
    /* core\drmutilitieslite.c */
    /* modules\certcache\real\certcachereal.c */
    {"_CheckCertificate", PERF_SCOPE_ID(PERF_MOD_DRMUTILITIES, PERF_FUNC__CheckCertificate, 1), 0, 0},
    {"DRM_CERTCACHE_VerifyCachedCertificate", PERF_SCOPE_ID(PERF_MOD_DRMUTILITIES, PERF_FUNC_DRM_CERTCACHE_VerifyCachedCertificate, 1), 0, 0},

    /* modules\xmlparser\common\drmxmlparserlite.c */
    {"_GetXMLSubNodeW", PERF_SCOPE_ID(PERF_MOD_DRMXMLPARSER, PERF_FUNC__GetXMLSubNodeW, 1), 0, 0},
    {"DRM_XML_GetSubNodeByPath", PERF_SCOPE_ID(PERF_MOD_DRMXMLPARSER, PERF_FUNC_DRM_XML_GetSubNodeByPath, 1), 0, 0},
    {"DRM_XML_GetNodeAttribute", PERF_SCOPE_ID(PERF_MOD_DRMXMLPARSER, PERF_FUNC_DRM_XML_GetNodeAttribute, 1), 0, 0},
    {"DRM_XML_GetSubNodeA", PERF_SCOPE_ID(PERF_MOD_DRMXMLPARSER, PERF_FUNC_DRM_XML_GetSubNodeA, 1), 0, 0},
    {"_GetXMLSubNodeA", PERF_SCOPE_ID(PERF_MOD_DRMXMLPARSER, PERF_FUNC__GetXMLSubNodeA, 1), 0, 0},

    /* oem\ansi\oeminfo.c */
    /* oem\wince\oeminfo.c */
    {"Oem_Device_GetModelInfo", PERF_SCOPE_ID(PERF_MOD_OEMINFO, PERF_FUNC_Oem_Device_GetModelInfo, 1), 0, 0},

    /* core\drmcipher.c */
    {"DRM_CPHR_InitDecrypt", PERF_SCOPE_ID(PERF_MOD_DRMCIPHER, PERF_FUNC_DRM_CPHR_InitDecrypt, 1), 2631, 2632},
    {"DRM_CPHR_Decrypt", PERF_SCOPE_ID(PERF_MOD_DRMCIPHER, PERF_FUNC_DRM_CPHR_Decrypt, 1), 2633, 2634},
    {"DRM_CPHR_Encrypt", PERF_SCOPE_ID(PERF_MOD_DRMCIPHER, PERF_FUNC_DRM_CPHR_Encrypt, 1), 0, 0},
    {"DRM_CPHR_Init", PERF_SCOPE_ID(PERF_MOD_DRMCIPHER, PERF_FUNC_DRM_CPHR_Init, 1), 0, 0},

    /* oem\common\RSA\... */
    {"OEM_RSA_OaepDecrypt", PERF_SCOPE_ID(PERF_MOD_DRMRSA, PERF_FUNC_OEM_RSA_OaepDecrypt, 1), 0, 0},
    {"OEM_RSA_OaepEncrypt", PERF_SCOPE_ID(PERF_MOD_DRMRSA, PERF_FUNC_OEM_RSA_OaepEncrypt, 1), 0, 0},
    {"OEM_RSA_PssSign", PERF_SCOPE_ID(PERF_MOD_DRMRSA, PERF_FUNC_OEM_RSA_PssSign, 1), 0, 0},
    {"OEM_RSA_PssVerify", PERF_SCOPE_ID(PERF_MOD_DRMRSA, PERF_FUNC_OEM_RSA_PssVerify, 1), 0, 0},

    /* oem\common\aes\oemaes.c */
    {"Oem_Aes_SetKey", PERF_SCOPE_ID(PERF_MOD_OEMAES, PERF_FUNC_Oem_Aes_SetKey, 1), 0, 0},
    {"Oem_Aes_EncryptOne", PERF_SCOPE_ID(PERF_MOD_OEMAES, PERF_FUNC_Oem_Aes_EncryptOne, 1), 0, 0},
    {"Oem_Aes_DecryptOne", PERF_SCOPE_ID(PERF_MOD_OEMAES, PERF_FUNC_Oem_Aes_DecryptOne, 1), 0, 0},

    /* oem\common\aes\oemaesmulti.c */
    {"DRM_Aes_CtrProcessData", PERF_SCOPE_ID(PERF_MOD_DRMAES, PERF_FUNC_Oem_Aes_CtrProcessData, 1), 0, 0},
    {"Oem_Aes_EcbEncryptData", PERF_SCOPE_ID(PERF_MOD_DRMAES, PERF_FUNC_Oem_Aes_EcbEncryptData, 1), 0, 0},
    {"DRM_Aes_EcbDecryptData", PERF_SCOPE_ID(PERF_MOD_DRMAES, PERF_FUNC_Oem_Aes_EcbDecryptData, 1), 0, 0},
    {"_Omac1_GenerateSignTag", PERF_SCOPE_ID(PERF_MOD_DRMAES, PERF_FUNC_Omac1_GenerateSignTag, 1), 0, 0},
    {"_Omac1_GenerateSignInfo", PERF_SCOPE_ID(PERF_MOD_DRMAES, PERF_FUNC_Omac1_GenerateSignInfo, 1), 0, 0},
    {"DRM_Omac1_Sign", PERF_SCOPE_ID(PERF_MOD_DRMAES, PERF_FUNC_Oem_Omac1_Sign, 1), 0, 0},
    {"DRM_Omac1_Verify", PERF_SCOPE_ID(PERF_MOD_DRMAES, PERF_FUNC_Oem_Omac1_Verify, 1), 0, 0},
    {"DRM_Aes_CbcEncryptData", PERF_SCOPE_ID(PERF_MOD_DRMAES, PERF_FUNC_Oem_Aes_CbcEncryptData, 1), 0, 0},
    {"DRM_Aes_CbcDecryptData", PERF_SCOPE_ID(PERF_MOD_DRMAES, PERF_FUNC_Oem_Aes_CbcDecryptData, 1), 0, 0},

    /* certs\drmbcertbuilder.c */
    /* certs\drmbcertparser.c  */
    {"_calcManufacturerInfoSize", PERF_SCOPE_ID(PERF_MOD_BCERT, PERF_FUNC_calcManufacturerInfoSize, 1), 0, 0},
    {"_calcKeyInfoSize", PERF_SCOPE_ID(PERF_MOD_BCERT, PERF_FUNC_calcKeyInfoSize, 1), 0, 0},
    {"_calcFeatureInfoSize", PERF_SCOPE_ID(PERF_MOD_BCERT, PERF_FUNC_calcFeatureInfoSize, 1), 0, 0},
    {"_calcSignatureInfoSize", PERF_SCOPE_ID(PERF_MOD_BCERT, PERF_FUNC_calcSignatureInfoSize, 1), 0, 0},
    {"_calcDomainInfoSize", PERF_SCOPE_ID(PERF_MOD_BCERT, PERF_FUNC_calcDomainInfoSize, 1), 0, 0},
    {"_calcCertSize", PERF_SCOPE_ID(PERF_MOD_BCERT, PERF_FUNC_calcCertSize, 1), 0, 0},
    {"_checkBuffer", PERF_SCOPE_ID(PERF_MOD_BCERT, PERF_FUNC_checkBuffer, 1), 0, 0},
    {"_addAlignedData", PERF_SCOPE_ID(PERF_MOD_BCERT, PERF_FUNC_addAlignedData, 1), 0, 0},
    {"_updateCertChainHeader", PERF_SCOPE_ID(PERF_MOD_BCERT, PERF_FUNC_updateCertChainHeader, 1), 0, 0},
    {"_addCertChainHeader", PERF_SCOPE_ID(PERF_MOD_BCERT, PERF_FUNC_addCertChainHeader, 1), 0, 0},
    {"_addCertHeader", PERF_SCOPE_ID(PERF_MOD_BCERT, PERF_FUNC_addCertHeader, 1), 0, 0},
    {"_addCertObjectHeader", PERF_SCOPE_ID(PERF_MOD_BCERT, PERF_FUNC_addCertObjectHeader, 1), 0, 0},
    {"_addCertBasicInfo", PERF_SCOPE_ID(PERF_MOD_BCERT, PERF_FUNC_addCertBasicInfo, 1), 0, 0},
    {"_addCertFeatureInfo", PERF_SCOPE_ID(PERF_MOD_BCERT, PERF_FUNC_addCertFeatureInfo, 1), 0, 0},
    {"_addCertKeyInfo", PERF_SCOPE_ID(PERF_MOD_BCERT, PERF_FUNC_addCertKeyInfo, 1), 0, 0},
    {"_addCertManufacturerString", PERF_SCOPE_ID(PERF_MOD_BCERT, PERF_FUNC_addCertManufacturerString, 1), 0, 0},
    {"_addCertManufacturerInfo", PERF_SCOPE_ID(PERF_MOD_BCERT, PERF_FUNC_addCertManufacturerInfo, 1), 0, 0},
    {"_addCertSignatureInfo", PERF_SCOPE_ID(PERF_MOD_BCERT, PERF_FUNC_addCertSignatureInfo, 1), 0, 0},
    {"_addCertDomainInfo", PERF_SCOPE_ID(PERF_MOD_BCERT, PERF_FUNC_addCertDomainInfo, 1), 0, 0},
    {"_addCertPCInfo", PERF_SCOPE_ID(PERF_MOD_BCERT, PERF_FUNC_addCertPCInfo, 1), 0, 0},
    {"_addCertDeviceInfo", PERF_SCOPE_ID(PERF_MOD_BCERT, PERF_FUNC_addCertDeviceInfo, 1), 0, 0},
    {"BCert_AddCert", PERF_SCOPE_ID(PERF_MOD_BCERT, PERF_FUNC_BCert_AddCert, 1), 0, 0},
    {"_addCertSilverLightInfo", PERF_SCOPE_ID(PERF_MOD_BCERT, PERF_FUNC_addCertSilverLightInfo, 1), 0, 0},
    {"_calcExtDataSignKeyInfoSize", PERF_SCOPE_ID(PERF_MOD_BCERT, PERF_FUNC_calcExtDataSignKeyInfoSize, 1), 0, 0},
    {"_calcExtDataContainerSize", PERF_SCOPE_ID(PERF_MOD_BCERT, PERF_FUNC_calcExtDataContainerSize, 1), 0, 0},
    {"_addExtDataSignKeyInfo", PERF_SCOPE_ID(PERF_MOD_BCERT, PERF_FUNC_addExtDataSignKeyInfo, 1), 0, 0},
    {"_addExtDataContainer", PERF_SCOPE_ID(PERF_MOD_BCERT, PERF_FUNC_addExtDataContainer, 1), 0, 0},
    {"BCert_AddExtendedDataToCert", PERF_SCOPE_ID(PERF_MOD_BCERT, PERF_FUNC_BCert_AddExtendedDataToCert, 1), 0, 0},
    {"_addCertMeteringInfo", PERF_SCOPE_ID(PERF_MOD_BCERT, PERF_FUNC_addCertMeteringInfo, 1), 0, 0},
    {"_calcMeteringInfoSize", PERF_SCOPE_ID(PERF_MOD_BCERT, PERF_FUNC_calcMeteringInfoSize, 1), 0, 0},
    {"_addCertServerInfo", PERF_SCOPE_ID(PERF_MOD_BCERT, PERF_FUNC_addCertServerInfo, 1), 0, 0},
    {"_addCertSecurityVersion", PERF_SCOPE_ID(PERF_MOD_BCERT, PERF_FUNC_addCertSecurityVersion, 1), 0, 0},
    {"_verifyAdjacentCerts", PERF_SCOPE_ID(PERF_MOD_BCERT, PERF_FUNC_verifyAdjacentCerts, 1), 0, 0},
    {"_getObjectHeader", PERF_SCOPE_ID(PERF_MOD_BCERT, PERF_FUNC_getObjectHeader, 1), 0, 0},
    {"_parseCertHeader", PERF_SCOPE_ID(PERF_MOD_BCERT, PERF_FUNC_parseCertHeader, 1), 0, 0},
    {"_parseCertBasicInfo", PERF_SCOPE_ID(PERF_MOD_BCERT, PERF_FUNC_parseCertBasicInfo, 1), 0, 0},
    {"_parseDomainInfo", PERF_SCOPE_ID(PERF_MOD_BCERT, PERF_FUNC_parseDomainInfo, 1), 0, 0},
    {"_parsePCInfo", PERF_SCOPE_ID(PERF_MOD_BCERT, PERF_FUNC_parsePCInfo, 1), 0, 0},
    {"_parseDeviceInfo", PERF_SCOPE_ID(PERF_MOD_BCERT, PERF_FUNC_parseDeviceInfo, 1), 0, 0},
    {"_parseFeatureInfo", PERF_SCOPE_ID(PERF_MOD_BCERT, PERF_FUNC_parseFeatureInfo, 1), 0, 0},
    {"_parseManufacturerString", PERF_SCOPE_ID(PERF_MOD_BCERT, PERF_FUNC_parseManufacturerString, 1), 0, 0},
    {"_parseSignatureInfo", PERF_SCOPE_ID(PERF_MOD_BCERT, PERF_FUNC_parseSignatureInfo, 1), 0, 0},
    {"_parseKeyInfo", PERF_SCOPE_ID(PERF_MOD_BCERT, PERF_FUNC_parseKeyInfo, 1), 0, 0},
    {"_parseCertificate", PERF_SCOPE_ID(PERF_MOD_BCERT, PERF_FUNC_parseCertificate, 1), 0, 0},
    {"DRM_BCert_FindObjectInCertByType", PERF_SCOPE_ID(PERF_MOD_BCERT, PERF_FUNC_DRM_BCert_FindObjectInCertByType, 1), 0, 0},
    {"DRM_BCert_GetChainHeader", PERF_SCOPE_ID(PERF_MOD_BCERT, PERF_FUNC_DRM_BCert_GetChainHeader, 1), 0, 0},
    {"DRM_BCert_ParseCertificateChain", PERF_SCOPE_ID(PERF_MOD_BCERT, PERF_FUNC_DRM_BCert_ParseCertificateChain, 1), 0, 0},
    {"DRM_BCert_GetCertificate", PERF_SCOPE_ID(PERF_MOD_BCERT, PERF_FUNC_DRM_BCert_GetCertificate, 1), 0, 0},
    {"DRM_BCert_GetPublicKey", PERF_SCOPE_ID(PERF_MOD_BCERT, PERF_FUNC_DRM_BCert_GetPublicKey, 1), 0, 0},
    {"DRM_BCert_GetPublicKeyByUsage", PERF_SCOPE_ID(PERF_MOD_BCERT, PERF_FUNC_DRM_BCert_GetPublicKeyByUsage, 1), 0, 0},
    {"_parseSilverLightInfo", PERF_SCOPE_ID(PERF_MOD_BCERT, PERF_FUNC_parseSilverLightInfo, 1), 0, 0},
    {"DRM_BCert_verifySignatureInfo", PERF_SCOPE_ID(PERF_MOD_BCERT, PERF_FUNC_DRM_BCert_VerifySignature, 1), 0, 0},
    {"_verifyExtDataSignature", PERF_SCOPE_ID(PERF_MOD_BCERT, PERF_FUNC_verifyExtDataSignature, 1), 0, 0},
    {"_parseExtDataSignKeyInfo", PERF_SCOPE_ID(PERF_MOD_BCERT, PERF_FUNC_parseExtDataSignKeyInfo, 1), 0, 0},
    {"_parseExtDataContainer", PERF_SCOPE_ID(PERF_MOD_BCERT, PERF_FUNC_parseExtDataContainer, 1), 0, 0},
    {"_parseServerInfo", PERF_SCOPE_ID(PERF_MOD_BCERT, PERF_FUNC_parseServerInfo, 1), 0, 0},
    {"_parseSecurityVersion", PERF_SCOPE_ID(PERF_MOD_BCERT, PERF_FUNC_parseSecurityVersion, 1), 0, 0},
    {"DRM_BCert_GetSecurityVersion", PERF_SCOPE_ID(PERF_MOD_BCERT, PERF_FUNC_DRM_BCert_GetSecurityVersion, 1), 0, 0},
    {"DRM_BCert_GetManufacturerStrings", PERF_SCOPE_ID(PERF_MOD_BCERT, PERF_FUNC_DRM_BCert_GetManufacturerStrings, 1), 0, 0},

    /* domainstore\drmdomainstore.c */
    {"_DomainStore_AddData", PERF_SCOPE_ID(PERF_MOD_DOMAINSTORE, PERF_FUNC__DomainStore_AddData, 1), 0, 0},
    {"_DomainStore_GetData", PERF_SCOPE_ID(PERF_MOD_DOMAINSTORE, PERF_FUNC__DomainStore_GetData, 1), 0, 0},
    {"_DomainStore_DeleteData", PERF_SCOPE_ID(PERF_MOD_DOMAINSTORE, PERF_FUNC__DomainStore_DeleteData, 1), 0, 0},
    {"_DomainStore_InitEnumData", PERF_SCOPE_ID(PERF_MOD_DOMAINSTORE, PERF_FUNC__DomainStore_InitEnumData, 1), 0, 0},
    {"_DomainStore_EnumNextData", PERF_SCOPE_ID(PERF_MOD_DOMAINSTORE, PERF_FUNC__DomainStore_EnumNextData, 1), 0, 0},
    {"DRM_DOMST_OpenStore", PERF_SCOPE_ID(PERF_MOD_DOMAINSTORE, PERF_FUNC_DRM_DOMST_OpenStore, 1), 0, 0},
    {"DRM_DOMST_CloseStore", PERF_SCOPE_ID(PERF_MOD_DOMAINSTORE, PERF_FUNC_DRM_DOMST_CloseStore, 1), 0, 0},
    {"DRM_DOMST_DeleteKeys", PERF_SCOPE_ID(PERF_MOD_DOMAINSTORE, PERF_FUNC_DRM_DOMST_DeleteKeys, 1), 0, 0},

    /* domain\drmdomainimp.c */
    /* domain\drmdomainkeyxmrparser.c */
    {"_BuildJoinChallengeDataXML", PERF_SCOPE_ID(PERF_MOD_DOMAINAPI, PERF_FUNC__BuildJoinChallengeDataXML, 1), 0, 0},
    {"_BuildLeaveChallengeDataXML", PERF_SCOPE_ID(PERF_MOD_DOMAINAPI, PERF_FUNC__BuildLeaveChallengeDataXML, 1), 0, 0},
    {"_BuildJoinChallengeXML", PERF_SCOPE_ID(PERF_MOD_DOMAINAPI, PERF_FUNC__BuildJoinChallengeXML, 1), 0, 0},
    {"_BuildLeaveChallengeXML", PERF_SCOPE_ID(PERF_MOD_DOMAINAPI, PERF_FUNC__BuildLeaveChallengeXML, 1), 0, 0},
    {"_ProcessJoinDataFromResponse", PERF_SCOPE_ID(PERF_MOD_DOMAINAPI, PERF_FUNC__ProcessJoinDataFromResponse, 1), 0, 0},
    {"DRM_DOM_GenerateJoinChallenge", PERF_SCOPE_ID(PERF_MOD_DOMAINAPI, PERF_FUNC_DRM_DOM_GenerateJoinChallenge, 1), 2623, 2624},
    {"DRM_DOM_ProcessJoinResponse", PERF_SCOPE_ID(PERF_MOD_DOMAINAPI, PERF_FUNC_DRM_DOM_ProcessJoinResponse, 1), 2625, 2626},
    {"DRM_DOM_GenerateLeaveChallenge", PERF_SCOPE_ID(PERF_MOD_DOMAINAPI, PERF_FUNC_DRM_DOM_GenerateLeaveChallenge, 1), 2627, 2628},
    {"DRM_DOM_ProcessLeaveResponse", PERF_SCOPE_ID(PERF_MOD_DOMAINAPI, PERF_FUNC_DRM_DOM_ProcessLeaveResponse, 1), 2629, 2630},
    {"DRM_DOM_FindCert", PERF_SCOPE_ID(PERF_MOD_DOMAINAPI, PERF_FUNC_DRM_DOM_FindCert, 1), 0, 0},
    {"DRM_DOM_InitCertEnum", PERF_SCOPE_ID(PERF_MOD_DOMAINAPI, PERF_FUNC_DRM_DOM_InitCertEnum, 1), 0, 0},
    {"DRM_DOM_EnumNextCert", PERF_SCOPE_ID(PERF_MOD_DOMAINAPI, PERF_FUNC_DRM_DOM_EnumNextCert, 1), 0, 0},
    {"_ParseJoinDomainChallengeQueryData", PERF_SCOPE_ID(PERF_MOD_DOMAINAPI, PERF_FUNC__ParseJoinDomainChallengeQueryData, 1), 0, 0},
    {"_ParseLeaveDomainChallengeQueryData", PERF_SCOPE_ID(PERF_MOD_DOMAINAPI, PERF_FUNC__ParseLeaveDomainChallengeQueryData, 1), 0, 0},
    {"DRM_DOMKEYXMR_GetHeader", PERF_SCOPE_ID(PERF_MOD_DOMAINAPI, PERF_FUNC_DRM_DOMKEYXMR_GetHeader, 1), 0, 0},
    {"DRM_DOMKEYXMR_GetSessionKey", PERF_SCOPE_ID(PERF_MOD_DOMAINAPI, PERF_FUNC_DRM_DOMKEYXMR_GetSessionKey, 1), 0, 0},
    {"DRM_DOMKEYXMR_GetPrivKeyContainer", PERF_SCOPE_ID(PERF_MOD_DOMAINAPI, PERF_FUNC_DRM_DOMKEYXMR_GetPrivKeyContainer, 1), 0, 0},
    {"DRM_DOMKEYXMR_GetPrivKey", PERF_SCOPE_ID(PERF_MOD_DOMAINAPI, PERF_FUNC_DRM_DOMKEYXMR_GetPrivKey, 1), 0, 0},

    /* core\drmxmlsig.c */
    {"_CalcSHA256Hash", PERF_SCOPE_ID(PERF_MOD_XMLSIG, PERF_FUNC__CalcSHA256Hash, 1), 0, 0},
    {"_VerifySHA256Hash", PERF_SCOPE_ID(PERF_MOD_XMLSIG, PERF_FUNC__VerifySHA256Hash, 1), 0, 0},
    {"_BuildCipherDataNode", PERF_SCOPE_ID(PERF_MOD_XMLSIG, PERF_FUNC__BuildCipherDataNode, 1), 0, 0},
    {"DRM_XMLSIG_ExtractCipherData", PERF_SCOPE_ID(PERF_MOD_XMLSIG, PERF_FUNC_DRM_XMLSIG_ExtractCipherData, 1), 0, 0},
    {"_BuildSignedInfoNode", PERF_SCOPE_ID(PERF_MOD_XMLSIG, PERF_FUNC__BuildSignedInfoNode, 1), 0, 0},
    {"_BuildSignatureValueNode", PERF_SCOPE_ID(PERF_MOD_XMLSIG, PERF_FUNC__BuildSignatureValueNode, 1), 0, 0},
    {"_BuildPublicKeyInfoNodeWithName", PERF_SCOPE_ID(PERF_MOD_XMLSIG, PERF_FUNC__BuildPublicKeyInfoNodeWithName, 1), 0, 0},
    {"_ExtractPublicKeyByName", PERF_SCOPE_ID(PERF_MOD_XMLSIG, PERF_FUNC__ExtractPublicKeyByName, 1), 0, 0},
    {"_BuildECC256PublicKeyInfoNode", PERF_SCOPE_ID(PERF_MOD_XMLSIG, PERF_FUNC__BuildECC256PublicKeyInfoNode, 1), 0, 0},
    {"_ExtractECC256PublicKey", PERF_SCOPE_ID(PERF_MOD_XMLSIG, PERF_FUNC__ExtractECC256PublicKey, 1), 0, 0},
    {"DRM_XMLSIG_BuildEncryptedKeyInfoNode", PERF_SCOPE_ID(PERF_MOD_XMLSIG, PERF_FUNC_DRM_XMLSIG_BuildEncryptedKeyInfoNode, 1), 0, 0},
    {"DRM_XMLSIG_BuildSignatureNode", PERF_SCOPE_ID(PERF_MOD_XMLSIG, PERF_FUNC_DRM_XMLSIG_BuildSignatureNode, 1), 0, 0},
    {"DRM_XMLSIG_VerifySignature", PERF_SCOPE_ID(PERF_MOD_XMLSIG, PERF_FUNC_DRM_XMLSIG_VerifySignature, 1), 0, 0},
    {"DRM_XMLSIG_BuildEncryptedDataNode", PERF_SCOPE_ID(PERF_MOD_XMLSIG, PERF_FUNC_DRM_XMLSIG_BuildEncryptedDataNode, 1), 0, 0},

    /* core\drmsoapxmlutility.c */
    {"DRM_SOAPXML_EncodeData", PERF_SCOPE_ID(PERF_MOD_SOAPXML, PERF_FUNC_DRM_SOAPXML_EncodeData, 1), 0, 0},
    {"DRM_SOAPXML_DecodeData", PERF_SCOPE_ID(PERF_MOD_SOAPXML, PERF_FUNC_DRM_SOAPXML_DecodeData, 1), 0, 0},
    {"DRM_SOAPXML_GetDeviceCert", PERF_SCOPE_ID(PERF_MOD_SOAPXML, PERF_FUNC_DRM_SOAPXML_GetDeviceCert, 1), 0, 0},
    {"DRM_SOAPXML_InitXMLKey", PERF_SCOPE_ID(PERF_MOD_SOAPXML, PERF_FUNC_DRM_SOAPXML_InitXMLKey, 1), 0, 0},
    {"DRM_SOAPXML_EncryptDataWithXMLKey", PERF_SCOPE_ID(PERF_MOD_SOAPXML, PERF_FUNC_DRM_SOAPXML_EncryptDataWithXMLKey, 1), 0, 0},
    {"DRM_SOAPXML_BuildSOAPHeaderXML", PERF_SCOPE_ID(PERF_MOD_SOAPXML, PERF_FUNC_DRM_SOAPXML_BuildSOAPHeaderXML, 1), 0, 0},
    {"_ParseCustomData", PERF_SCOPE_ID(PERF_MOD_SOAPXML, PERF_FUNC__ParseCustomData, 1), 0, 0},
    {"_ParseCustomDataByPath", PERF_SCOPE_ID(PERF_MOD_SOAPXML, PERF_FUNC__ParseCustomDataByPath, 1), 0, 0},
    {"DRM_SOAPXML_ParseStatusCode", PERF_SCOPE_ID(PERF_MOD_SOAPXML, PERF_FUNC_DRM_SOAPXML_ParseStatusCode, 1), 0, 0},
    {"Drm_GetAdditionalResponseData", PERF_SCOPE_ID(PERF_MOD_SOAPXML, PERF_FUNC_Drm_GetAdditionalResponseData, 1), 0, 0},
    {"DRM_SOAPXML_GetAdditionalResponseData", PERF_SCOPE_ID(PERF_MOD_SOAPXML, PERF_FUNC_DRM_SOAPXML_GetAdditionalResponseData, 1), 0, 0},
    {"DRM_SOAPXML_ParseCustomDataForProtocol", PERF_SCOPE_ID(PERF_MOD_SOAPXML, PERF_FUNC_DRM_SOAPXML_ParseCustomDataForProtocol, 1), 0, 0},
    {"DRM_SOAPXML_ValidateProtocolSignature", PERF_SCOPE_ID(PERF_MOD_SOAPXML, PERF_FUNC_DRM_SOAPXML_ValidateProtocolSignature, 1), 0, 0},
    {"DRM_SOAPXML_BuildClientInfo", PERF_SCOPE_ID(PERF_MOD_SOAPXML, PERF_FUNC_DRM_SOAPXML_BuildClientInfo, 1), 0, 0},
    {"_GetAdditionalResponseDomainIDData", PERF_SCOPE_ID(PERF_MOD_SOAPXML, PERF_FUNC__GetAdditionalResponseDomainIDData, 1), 0, 0},
    {"_GetAdditionalResponseExceptionData", PERF_SCOPE_ID(PERF_MOD_SOAPXML, PERF_FUNC__GetAdditionalResponseExceptionData, 1), 0, 0},

    /* core\drmlicacqv3.c */
    /* drmmanager\drmmanager.c */
    {"_CalcDeviceCertCharCount", PERF_SCOPE_ID(PERF_MOD_LICACQV3, PERF_FUNC__CalcDeviceCertCharCount, 1), 0, 0},
    {"_CalcDomainCertsCharCount", PERF_SCOPE_ID(PERF_MOD_LICACQV3, PERF_FUNC__CalcDomainCertsCharCount, 1), 0, 0},
    {"_BuildDomainCertsXML", PERF_SCOPE_ID(PERF_MOD_LICACQV3, PERF_FUNC__BuildDomainCertsXML, 1), 0, 0},
    {"_BuildCertChainsXML", PERF_SCOPE_ID(PERF_MOD_LICACQV3, PERF_FUNC__BuildCertChainsXML, 1), 0, 0},
    {"_BuildRevListInfoXML", PERF_SCOPE_ID(PERF_MOD_LICACQV3, PERF_FUNC__BuildRevListInfoXML, 1), 0, 0},
    {"_BuildLicenseChallengeDataXML", PERF_SCOPE_ID(PERF_MOD_LICACQV3, PERF_FUNC__BuildLicenseChallengeDataXML, 1), 0, 0},
    {"_BuildLicenseAcknowledgementDataXML", PERF_SCOPE_ID(PERF_MOD_LICACQV3, PERF_FUNC__BuildLicenseAcknowledgementDataXML, 1), 0, 0},
    {"_BuildLicenseChallengeXML", PERF_SCOPE_ID(PERF_MOD_LICACQV3, PERF_FUNC__BuildLicenseChallengeXML, 1), 0, 0},
    {"_BuildLicenseAcknowledgementXML", PERF_SCOPE_ID(PERF_MOD_LICACQV3, PERF_FUNC__BuildLicenseAcknowledgementXML, 1), 0, 0},
    {"_PrepareViewRights", PERF_SCOPE_ID(PERF_MOD_LICACQV3, PERF_FUNC__PrepareViewRights, 1), 0, 0},
    {"_GetLicenseState", PERF_SCOPE_ID(PERF_MOD_LICACQV3, PERF_FUNC__GetLicenseState, 1), 0, 0},
    {"_PrepareUplinks", PERF_SCOPE_ID(PERF_MOD_LICACQV3, PERF_FUNC__PrepareUplinks, 1), 0, 0},
    {"_PrepareLicenseChallenge", PERF_SCOPE_ID(PERF_MOD_LICACQV3, PERF_FUNC__PrepareLicenseChallenge, 1), 0, 0},
    {"DRM_LA_ParseLicenseAcquisitionURL", PERF_SCOPE_ID(PERF_MOD_LICACQV3, PERF_FUNC_DRM_LA_ParseLicenseAcquisitionURL, 1), 0, 0},
    {"_ExtractLicensesFromLicenseResponse", PERF_SCOPE_ID(PERF_MOD_LICACQV3, PERF_FUNC__ExtractLicensesFromLicenseResponse, 1), 0, 0},
    {"DRM_LA_ProcessRevocationPackage", PERF_SCOPE_ID(PERF_MOD_LICACQV3, PERF_FUNC_DRM_LA_ProcessRevocationPackage, 1), 0, 0},
    {"_ExtractDataFromLicenseResponse", PERF_SCOPE_ID(PERF_MOD_LICACQV3, PERF_FUNC__ExtractDataFromLicenseResponse, 1), 0, 0},
    {"_GenerateLicenseChallengeV3", PERF_SCOPE_ID(PERF_MOD_LICACQV3, PERF_FUNC__GenerateLicenseChallengeV3, 1), 0, 0},
    {"_ProcessLicenseResponseV3", PERF_SCOPE_ID(PERF_MOD_LICACQV3, PERF_FUNC__ProcessLicenseResponseV3, 1), 0, 0},
    {"Drm_LicenseAcq_GenerateAck", PERF_SCOPE_ID(PERF_MOD_LICACQV3, PERF_FUNC_Drm_LicenseAcq_GenerateAck, 1), 0, 0},
    {"Drm_LicenseAcq_ProcessAckResponse", PERF_SCOPE_ID(PERF_MOD_LICACQV3, PERF_FUNC_Drm_LicenseAcq_ProcessAckResponse, 1), 0, 0},
    {"DRM_LA_ProcessLicenseV3", PERF_SCOPE_ID(PERF_MOD_LICACQV3, PERF_FUNC_DRM_LA_ProcessLicenseV3, 1), 0, 0},

    /* core\drmutf.c */
    {"DRM_UTF8_VerifyBytes", PERF_SCOPE_ID(PERF_MOD_UTF8, PERF_FUNC_DRM_UTF8_VerifyBytes, 1), 0, 0},

    /* core\drmhmac.c */
    {"DRM_HMAC_CreateMAC", PERF_SCOPE_ID(PERF_MOD_DRMHMAC, PERF_FUNC_DRM_HMAC_CreateMAC, 1), 0, 0},
    {"DRM_HMAC_VerifyMAC", PERF_SCOPE_ID(PERF_MOD_DRMHMAC, PERF_FUNC_DRM_HMAC_VerifyMAC, 1), 0, 0},

    /* oem\common\ecc\base\oemeccp256.c */
    /* oem\common\ecc\baseimpl\oemeccp256impl.c */
    {"OEM_ECDSA_Verify_P256", PERF_SCOPE_ID(PERF_MOD_DRM_ECC_P256, PERF_FUNC_OEM_ECDSA_Verify_P256, 1), 0, 0},
    {"OEM_ECDSA_Sign_P256", PERF_SCOPE_ID(PERF_MOD_DRM_ECC_P256, PERF_FUNC_OEM_ECDSA_Sign_P256, 1), 0, 0},
    {"OEM_ECC_GenKeyPair_P256", PERF_SCOPE_ID(PERF_MOD_DRM_ECC_P256, PERF_FUNC_OEM_ECC_GenKeyPair_P256, 1), 0, 0},
    {"OEM_ECC_Encrypt_P256", PERF_SCOPE_ID(PERF_MOD_DRM_ECC_P256, PERF_FUNC_OEM_ECC_Encrypt_P256, 1), 0, 0},
    {"OEM_ECC_Decrypt_P256", PERF_SCOPE_ID(PERF_MOD_DRM_ECC_P256, PERF_FUNC_OEM_ECC_Decrypt_P256, 1), 0, 0},
    {"OEM_ECC_CanMapToPoint_P256", PERF_SCOPE_ID(PERF_MOD_DRM_ECC_P256, PERF_FUNC_OEM_ECC_CanMapToPoint_P256, 1), 0, 0},
    {"OEM_ECC_GenerateHMACKey_P256", PERF_SCOPE_ID(PERF_MOD_DRM_ECC_P256, PERF_FUNC_OEM_ECC_GenerateHMACKey_P256, 1), 0, 0},
    {"Convert_P256_PointToPlaintext", PERF_SCOPE_ID(PERF_MOD_DRM_ECC_P256, PERF_FUNC_Convert_P256_PointToPlaintext, 1), 0, 0},
    {"Convert_P256_PointToBigEndianBytes", PERF_SCOPE_ID(PERF_MOD_DRM_ECC_P256, PERF_FUNC_Convert_P256_PointToBigEndianBytes, 1), 0, 0},
    {"Convert_P256_PlaintextToPoint", PERF_SCOPE_ID(PERF_MOD_DRM_ECC_P256, PERF_FUNC_Convert_P256_PlaintextToPoint, 1), 0, 0},
    {"Convert_P256_ModularIntToDigitsModOrder", PERF_SCOPE_ID(PERF_MOD_DRM_ECC_P256, PERF_FUNC_Convert_P256_ModularIntToDigitsModOrder, 1), 0, 0},
    {"Convert_P256_ModularIntToBigEndianBytesModOrder", PERF_SCOPE_ID(PERF_MOD_DRM_ECC_P256, PERF_FUNC_Convert_P256_ModularIntToBigEndianBytesModOrder, 1), 0, 0},
    {"Convert_P256_DigitsToBigEndianBytes", PERF_SCOPE_ID(PERF_MOD_DRM_ECC_P256, PERF_FUNC_Convert_P256_DigitsToBigEndianBytes, 1), 0, 0},
    {"Convert_P256_BigEndianBytesToPoint", PERF_SCOPE_ID(PERF_MOD_DRM_ECC_P256, PERF_FUNC_Convert_P256_BigEndianBytesToPoint, 1), 0, 0},
    {"Convert_P256_BigEndianBytesToModular", PERF_SCOPE_ID(PERF_MOD_DRM_ECC_P256, PERF_FUNC_Convert_P256_BigEndianBytesToModular, 1), 0, 0},
    {"Convert_P256_BigEndianBytesToDigitsModOrder", PERF_SCOPE_ID(PERF_MOD_DRM_ECC_P256, PERF_FUNC_Convert_P256_BigEndianBytesToDigitsModOrder, 1), 0, 0},
    {"Convert_DigitsToBigEndianBytes", PERF_SCOPE_ID(PERF_MOD_DRM_ECC_P256, PERF_FUNC_Convert_DigitsToBigEndianBytes, 1), 0, 0},
    {"Convert_P256_BigEndianBytesToDigits", PERF_SCOPE_ID(PERF_MOD_DRM_ECC_P256, PERF_FUNC_Convert_P256_BigEndianBytesToDigits, 1), 0, 0},
    {"Convert_BigEndianBytesToDigits", PERF_SCOPE_ID(PERF_MOD_DRM_ECC_P256, PERF_FUNC_Convert_BigEndianBytesToDigits, 1), 0, 0},
    {"OEM_ECC_GenKeyPairRestrictedPriv_P256", PERF_SCOPE_ID(PERF_MOD_DRM_ECC_P256, PERF_FUNC_OEM_ECC_GenKeyPairRestrictedPriv_P256, 1), 0, 0},

    /* crypto\drmsha256\drmsha256.c */
    {"DRM_SHA256_Init", PERF_SCOPE_ID(PERF_MOD_DRMSHA256, PERF_FUNC_DRM_SHA256_Init, 1), 0, 0},
    {"DRM_SHA256_UpdateOffset", PERF_SCOPE_ID(PERF_MOD_DRMSHA256, PERF_FUNC_DRM_SHA256_UpdateOffset, 1), 0, 0},
    {"DRM_SHA256_Finalize", PERF_SCOPE_ID(PERF_MOD_DRMSHA256, PERF_FUNC_DRM_SHA256_Finalize, 1), 0, 0},
    {"_SHA256_Transform", PERF_SCOPE_ID(PERF_MOD_DRMSHA256, PERF_FUNC_SHA256_Transform, 1), 0, 0},

    /* ndr\drmndr.c */
    {"Drm_Ndr_Registration_GenerateChallenge", PERF_SCOPE_ID(PERF_MOD_NDR, PERF_FUNC_Drm_Ndr_Registration_GenerateChallenge, 1), 0, 0},
    {"Drm_Ndr_Registration_ProcessResponse", PERF_SCOPE_ID(PERF_MOD_NDR, PERF_FUNC_Drm_Ndr_Registration_ProcessResponse, 1), 0, 0},
    {"Drm_Ndr_License_GenerateChallenge", PERF_SCOPE_ID(PERF_MOD_NDR, PERF_FUNC_Drm_Ndr_License_GenerateChallenge, 1), 0, 0},
    {"Drm_Ndr_License_ProcessResponse", PERF_SCOPE_ID(PERF_MOD_NDR, PERF_FUNC_Drm_Ndr_License_ProcessResponse, 1), 0, 0},
    {"Drm_Ndr_Proximity_GenerateStart", PERF_SCOPE_ID(PERF_MOD_NDR, PERF_FUNC_Drm_Ndr_Proximity_GenerateStart, 1), 0, 0},
    {"Drm_Ndr_Proximity_GenerateResponse", PERF_SCOPE_ID(PERF_MOD_NDR, PERF_FUNC_Drm_Ndr_Proximity_GenerateResponse, 1), 0, 0},
    {"Drm_Ndr_Proximity_ProcessChallenge", PERF_SCOPE_ID(PERF_MOD_NDR, PERF_FUNC_Drm_Ndr_Proximity_ProcessChallenge, 1), 0, 0},
    {"Drm_Ndr_Proximity_ProcessResult", PERF_SCOPE_ID(PERF_MOD_NDR, PERF_FUNC_Drm_Ndr_Proximity_ProcessResult, 1), 0, 0},

    /* Device Store */
    /* modules\devicestore\real\drmdevicestore.c */
    /* modules\devicestore\real\drmdevstore.c */
    { "_SetDirtyFlag", PERF_SCOPE_ID( PERF_MOD_DEVICESTORE, PERF_FUNC__SetDirtyFlag, 1 ), 0, 0 },
    { "_GetDirtyFlag", PERF_SCOPE_ID( PERF_MOD_DEVICESTORE, PERF_FUNC__GetDirtyFlag, 1 ), 0, 0 },
    { "_InitDeviceHandle", PERF_SCOPE_ID( PERF_MOD_DEVICESTORE, PERF_FUNC__InitDeviceHandle, 1 ), 0, 0 },
    { "DRM_DVS_Initialize", PERF_SCOPE_ID( PERF_MOD_DEVICESTORE, PERF_FUNC_DRM_DVS_Initialize, 1 ), 0, 0 },
    { "DRM_DVS_RegisterDevice", PERF_SCOPE_ID( PERF_MOD_DEVICESTORE, PERF_FUNC_DRM_DVS_RegisterDevice, 1 ), 0, 0 },
    { "_CalculateStoreID", PERF_SCOPE_ID( PERF_MOD_DEVICESTORE, PERF_FUNC__CalculateStoreID, 1 ), 0, 0 },
    { "_WriteDeviceStoreEntryItem", PERF_SCOPE_ID( PERF_MOD_DEVICESTORE, PERF_FUNC__WriteDeviceStoreEntryItem, 1 ), 0, 0 },
    { "DRM_DVS_UnRegisterDevice", PERF_SCOPE_ID( PERF_MOD_DEVICESTORE, PERF_FUNC_DRM_DVS_UnRegisterDevice, 1 ), 0, 0 },
    { "DRM_DVS_GetFirstDevice", PERF_SCOPE_ID( PERF_MOD_DEVICESTORE, PERF_FUNC_DRM_DVS_GetFirstDevice, 1 ), 0, 0 },
    { "DRM_DVS_GetNextDevice", PERF_SCOPE_ID( PERF_MOD_DEVICESTORE, PERF_FUNC_DRM_DVS_GetNextDevice, 1 ), 0, 0 },
    { "DRM_DVS_GetDeviceByID", PERF_SCOPE_ID( PERF_MOD_DEVICESTORE, PERF_FUNC_DRM_DVS_GetDeviceByID, 1 ), 0, 0 },
    { "DRM_DVS_GetDeviceID", PERF_SCOPE_ID( PERF_MOD_DEVICESTORE, PERF_FUNC_DRM_DVS_GetDeviceID, 1 ), 0, 0 },
    { "DRM_DVS_GetDeviceAttr", PERF_SCOPE_ID( PERF_MOD_DEVICESTORE, PERF_FUNC_DRM_DVS_GetDeviceAttr, 1 ), 0, 0 },
    { "_ReadDeviceStoreEntryItem", PERF_SCOPE_ID( PERF_MOD_DEVICESTORE, PERF_FUNC__ReadDeviceStoreEntryItem, 1 ), 0, 0 },
    { "DRM_DVS_GetDeviceAttrCount", PERF_SCOPE_ID( PERF_MOD_DEVICESTORE, PERF_FUNC_DRM_DVS_GetDeviceAttrCount, 1 ), 0, 0 },
    { "DRM_DVS_GetDeviceAttrByIndex", PERF_SCOPE_ID( PERF_MOD_DEVICESTORE, PERF_FUNC_DRM_DVS_GetDeviceAttrByIndex, 1 ), 0, 0 },
    { "DRM_DVS_SetDeviceAttr", PERF_SCOPE_ID( PERF_MOD_DEVICESTORE, PERF_FUNC_DRM_DVS_SetDeviceAttr, 1 ), 0, 0 },
    { "DRM_DVS_CommitDevice", PERF_SCOPE_ID( PERF_MOD_DEVICESTORE, PERF_FUNC_DRM_DVS_CommitDevice, 1 ), 0, 0 },
    { "DRM_DeviceStore_MarkDeviceAsValid", PERF_SCOPE_ID( PERF_MOD_DEVICESTORE, PERF_FUNC_DRM_DeviceStore_MarkDeviceAsValid, 1 ), 0, 0 },
    { "DRM_DeviceStore_UnRegisterDevice", PERF_SCOPE_ID( PERF_MOD_DEVICESTORE, PERF_FUNC_DRM_DeviceStore_UnRegisterDevice, 1 ), 0, 0 },
    { "_GenerateDeviceID", PERF_SCOPE_ID( PERF_MOD_DEVICESTORE, PERF_FUNC__GenerateDeviceID, 1 ), 0, 0 },
    { "DRM_DeviceStore_GetDeviceByID", PERF_SCOPE_ID( PERF_MOD_DEVICESTORE, PERF_FUNC_DRM_DeviceStore_GetDeviceByID, 1 ), 0, 0 },
    { "DRM_DeviceStore_GetDeviceInfo", PERF_SCOPE_ID( PERF_MOD_DEVICESTORE, PERF_FUNC_DRM_DeviceStore_GetDeviceInfo, 1 ), 0, 0 },
    { "_ExtractDeviceInfo", PERF_SCOPE_ID( PERF_MOD_DEVICESTORE, PERF_FUNC__ExtractDeviceInfo, 1 ), 0, 0 },

    /* Revocation */
    /* modules\apprevocation\real\apprevocationimplreal.c */
    /* core\drmrevocation.c */
    /* modules\devicerevocation\real\devicerevocationimplreal.c */
    /* modules\wmdrmnet\real\wmdrmnetimplreal.c */
    /* core\drmrevocationstore.c */
    /* core\drmbcrlparser.c*/
    { "DRM_RVK_PerformAppRevocationCheck", PERF_SCOPE_ID( PERF_MOD_REVOCATION, PERF_FUNC_DRM_RVK_PerformAppRevocationCheck, 1 ), 0, 0 },
    { "DRM_RVK_GetCurrentAppRevocationList", PERF_SCOPE_ID( PERF_MOD_REVOCATION, PERF_FUNC_DRM_RVK_GetCurrentAppRevocationList, 1 ), 0, 0 },
    { "_CheckCertInRevocationList", PERF_SCOPE_ID( PERF_MOD_REVOCATION, PERF_FUNC__CheckCertInRevocationList, 1 ), 0, 0 },
    { "DRM_RVK_UpdateAppRevocationListA", PERF_SCOPE_ID( PERF_MOD_REVOCATION, PERF_FUNC_DRM_RVK_UpdateAppRevocationListA, 1 ), 0, 0 },
    { "DRM_RVK_UpdateAppRevocationListW", PERF_SCOPE_ID( PERF_MOD_REVOCATION, PERF_FUNC_DRM_RVK_UpdateAppRevocationListW, 1 ), 0, 0 },
    { "DRM_RVK_UpdateAppRevocationListDecoded", PERF_SCOPE_ID( PERF_MOD_REVOCATION, PERF_FUNC_DRM_RVK_UpdateAppRevocationListDecoded, 1 ), 0, 0 },
    { "DRM_RVK_UpdateRevocationVersionsCache", PERF_SCOPE_ID( PERF_MOD_REVOCATION, PERF_FUNC_DRM_RVK_UpdateRevocationVersionsCache, 1 ), 0, 0 },
    { "DRM_RVK_VerifyRevocationList", PERF_SCOPE_ID( PERF_MOD_REVOCATION, PERF_FUNC_DRM_RVK_VerifyRevocationList, 1 ), 0, 0 },
    { "DRM_RVK_UpdateRevocationList", PERF_SCOPE_ID( PERF_MOD_REVOCATION, PERF_FUNC_DRM_RVK_UpdateRevocationList, 1 ), 0, 0 },
    { "DRM_RVK_GetSSTRevocationList", PERF_SCOPE_ID( PERF_MOD_REVOCATION, PERF_FUNC_DRM_RVK_GetSSTRevocationList, 1 ), 0, 0 },
    { "DRM_RVK_VerifyWMDRMNETRevocationList", PERF_SCOPE_ID( PERF_MOD_REVOCATION, PERF_FUNC_DRM_RVK_VerifyWMDRMNETRevocationList, 1 ), 0, 0 },
    { "DRM_RVK_UpdateWMDRMNETRevocationList", PERF_SCOPE_ID( PERF_MOD_REVOCATION, PERF_FUNC_DRM_RVK_UpdateWMDRMNETRevocationList, 1 ), 0, 0 },
    { "DRM_RVK_UpdateWMDRMNETRevocationListDecoded", PERF_SCOPE_ID( PERF_MOD_REVOCATION, PERF_FUNC_DRM_RVK_UpdateWMDRMNETRevocationListDecoded, 1 ), 0, 0 },
    { "DRM_RVK_VerifyCRLSignature", PERF_SCOPE_ID( PERF_MOD_REVOCATION, PERF_FUNC_DRM_RVK_VerifyCRLSignature, 1 ), 0, 0 },
    { "DRM_RVK_GetWMDRMNETRevocationEntries", PERF_SCOPE_ID( PERF_MOD_REVOCATION, PERF_FUNC_DRM_RVK_GetWMDRMNETRevocationEntries, 1 ), 0, 0 },
    { "DRM_RVK_VerifyBinaryWMDRMNETSignature", PERF_SCOPE_ID( PERF_MOD_REVOCATION, PERF_FUNC_DRM_RVK_VerifyBinaryWMDRMNETSignature, 1 ), 0, 0 },
    { "DRM_RVK_VerifyRevocationInfo", PERF_SCOPE_ID( PERF_MOD_REVOCATION, PERF_FUNC_DRM_RVK_VerifyRevocationInfo, 1 ), 0, 0 },
    { "DRM_RVK_GetCurrentRevocationInfo", PERF_SCOPE_ID( PERF_MOD_REVOCATION, PERF_FUNC_DRM_RVK_GetCurrentRevocationInfo, 1 ), 0, 0 },
    { "DRM_RVK_StoreRevocationLists", PERF_SCOPE_ID( PERF_MOD_REVOCATION, PERF_FUNC_DRM_RVK_StoreRevocationLists, 1 ), 0, 0 },
    { "DRM_RVK_StoreRevInfo", PERF_SCOPE_ID( PERF_MOD_REVOCATION, PERF_FUNC_DRM_RVK_StoreRevInfo, 1 ), 0, 0 },
    { "DRM_RVK_ProcessRevocationInfo", PERF_SCOPE_ID( PERF_MOD_REVOCATION, PERF_FUNC_DRM_RVK_ProcessRevocationInfo, 1 ), 0, 0 },
    { "_ExtractRevocationList", PERF_SCOPE_ID( PERF_MOD_REVOCATION, PERF_FUNC__ExtractRevocationList, 1 ), 0, 0 },
    { "DRM_RVK_GetCRL", PERF_SCOPE_ID( PERF_MOD_REVOCATION, PERF_FUNC_DRM_RVK_GetCRL, 1 ), 0, 0 },
    { "DRM_RVK_SetCRL", PERF_SCOPE_ID( PERF_MOD_REVOCATION, PERF_FUNC_DRM_RVK_SetCRL, 1 ), 0, 0 },
    { "DRM_RVS_InitRevocationStore", PERF_SCOPE_ID( PERF_MOD_REVOCATION, PERF_FUNC_DRM_RVS_InitRevocationStore, 1 ), 0, 0 },
    { "DRM_RVS_StoreRevocationData", PERF_SCOPE_ID( PERF_MOD_REVOCATION, PERF_FUNC_DRM_RVS_StoreRevocationData, 1 ), 0, 0 },
    { "_CreateRevocationStorePassword", PERF_SCOPE_ID( PERF_MOD_REVOCATION, PERF_FUNC__CreateRevocationStorePassword, 1 ), 0, 0 },
    { "_LoopkupRevocationLIDFromGUID", PERF_SCOPE_ID( PERF_MOD_REVOCATION, PERF_FUNC__LoopkupRevocationLIDFromGUID, 1 ), 0, 0 },
    { "DRM_RVS_GetRevocationData", PERF_SCOPE_ID( PERF_MOD_REVOCATION, PERF_FUNC_DRM_RVS_GetRevocationData, 1 ), 0, 0 },
    { "DRM_BCrl_VerifySignature", PERF_SCOPE_ID( PERF_MOD_REVOCATION, PERF_FUNC_DRM_BCrl_VerifySignature, 1 ), 0, 0 },
    { "DRM_RVK_GetDeviceRevocationList", PERF_SCOPE_ID( PERF_MOD_REVOCATION, PERF_FUNC_DRM_RVK_GetDeviceRevocationList, 1 ), 0, 0 },
    { "DRM_RVK_GetWMDRMNETList", PERF_SCOPE_ID( PERF_MOD_REVOCATION, PERF_FUNC_DRM_RVK_GetWMDRMNETList, 1 ), 0, 0 },
    { "DRM_RVK_UpdateRevocationListDecoded", PERF_SCOPE_ID( PERF_MOD_REVOCATION, PERF_FUNC__UpdateRevocationList, 1 ), 0, 0 },
    /* RSA */
    /* oem\common\RSA\oaeppssimpl\oemrsaoaeppssimpl.c */
    /* oem\common\RSA\base\oemrsa.c */
    { "_OAEPDecode", PERF_SCOPE_ID( PERF_MOD_RSA, PERF_FUNC__OAEPDecode, 1 ), 0, 0 },
    { "_GenerateMGF1Mask", PERF_SCOPE_ID( PERF_MOD_RSA, PERF_FUNC__GenerateMGF1Mask, 1 ), 0, 0 },
    { "_OAEPEncode", PERF_SCOPE_ID( PERF_MOD_RSA, PERF_FUNC__OAEPEncode, 1 ), 0, 0 },
    { "_PSSEncode", PERF_SCOPE_ID( PERF_MOD_RSA, PERF_FUNC__PSSEncode, 1 ), 0, 0 },
    { "_PSSVerify", PERF_SCOPE_ID( PERF_MOD_RSA, PERF_FUNC__PSSVerify, 1 ), 0, 0 },
    { "_BigEndianBytesToDigits", PERF_SCOPE_ID( PERF_MOD_RSA, PERF_FUNC__BigEndianBytesToDigits, 1 ), 0, 0 },
    { "_DigitsToBigEndianBytes", PERF_SCOPE_ID( PERF_MOD_RSA, PERF_FUNC__DigitsToBigEndianBytes, 1 ), 0, 0 },
    { "OEM_RSA_SetPublicKey", PERF_SCOPE_ID( PERF_MOD_RSA, PERF_FUNC_OEM_RSA_SetPublicKey, 1 ), 0, 0 },
    { "OEM_RSA_SetPrivateKey", PERF_SCOPE_ID( PERF_MOD_RSA, PERF_FUNC_OEM_RSA_SetPrivateKey, 1 ), 0, 0 },
    { "OEM_RSA_ParsePublicKey", PERF_SCOPE_ID( PERF_MOD_RSA, PERF_FUNC_OEM_RSA_ParsePublicKey, 1 ), 0, 0 },
    { "OEM_RSA_ZeroPublicKey", PERF_SCOPE_ID( PERF_MOD_RSA, PERF_FUNC_OEM_RSA_ZeroPublicKey, 1 ), 0, 0 },
    { "OEM_RSA_ZeroPrivateKey", PERF_SCOPE_ID( PERF_MOD_RSA, PERF_FUNC_OEM_RSA_ZeroPrivateKey, 1 ), 0, 0 },
    { "_ModularExponentiate", PERF_SCOPE_ID( PERF_MOD_RSA, PERF_FUNC__ModularExponentiate, 1 ), 0, 0 },
    { "OEM_RSA_Decrypt", PERF_SCOPE_ID( PERF_MOD_RSA, PERF_FUNC_OEM_RSA_Decrypt, 1 ), 0, 0 },

    /* NDT */
    /* wmdrmcert\certparser\drmcertparser.c */
    /* ndt\drmpfdmanager.c */
    /* ndtmgr\ndtlicense.c */
    /* ndtmgr\ndtmanager_internal.c */
    /* modules\wmdrmnet\real\wmdrmnetimplreal.c */
    /* ndtmgr\ndtreg.c */
    /* ndtmgr\ndtencrypt.c */
    /* ndt\drmpfd.c */
    /* oem\common\oemndt.c */
    { "DRM_WCP_GetCertificateType", PERF_SCOPE_ID( PERF_MOD_NDT, PERF_FUNC_DRM_WCP_GetCertificateType, 1 ), 0, 0 },
    { "DRM_WCP_GetAttribute", PERF_SCOPE_ID( PERF_MOD_NDT, PERF_FUNC_DRM_WCP_GetAttribute, 1 ), 0, 0 },
    { "_ExtractValueFromAttribute", PERF_SCOPE_ID( PERF_MOD_NDT, PERF_FUNC__ExtractValueFromAttribute, 1 ), 0, 0 },
    { "DRM_WCP_GetKeyUsage", PERF_SCOPE_ID( PERF_MOD_NDT, PERF_FUNC_DRM_WCP_GetKeyUsage, 1 ), 0, 0 },
    { "DRM_WCP_GetFeatures", PERF_SCOPE_ID( PERF_MOD_NDT, PERF_FUNC_DRM_WCP_GetFeatures, 1 ), 0, 0 },
    { "DRM_WCP_GetSecurityLevel", PERF_SCOPE_ID( PERF_MOD_NDT, PERF_FUNC_DRM_WCP_GetSecurityLevel, 1 ), 0, 0 },
    { "DRM_WCP_GetPublicKey", PERF_SCOPE_ID( PERF_MOD_NDT, PERF_FUNC_DRM_WCP_GetPublicKey, 1 ), 0, 0 },
    { "DRM_WCP_GetHardwareID", PERF_SCOPE_ID( PERF_MOD_NDT, PERF_FUNC_DRM_WCP_GetHardwareID, 1 ), 0, 0 },
    { "DRM_WCP_GetSignatureDigest", PERF_SCOPE_ID( PERF_MOD_NDT, PERF_FUNC_DRM_WCP_GetSignatureDigest, 1 ), 0, 0 },
    { "DRM_WCP_GetManufacturerNodes", PERF_SCOPE_ID( PERF_MOD_NDT, PERF_FUNC_DRM_WCP_GetManufacturerNodes, 1 ), 0, 0 },
    { "DRM_WCP_VerifyCertificate", PERF_SCOPE_ID( PERF_MOD_NDT, PERF_FUNC_DRM_WCP_VerifyCertificate, 1 ), 0, 0 },
    { "_VerifyCertificateMethods", PERF_SCOPE_ID( PERF_MOD_NDT, PERF_FUNC__VerifyCertificateMethods, 1 ), 0, 0 },
    { "_VerifyCertificateSignature", PERF_SCOPE_ID( PERF_MOD_NDT, PERF_FUNC__VerifyCertificateSignature, 1 ), 0, 0 },
    { "_VerifyCertificateRequiredTags", PERF_SCOPE_ID( PERF_MOD_NDT, PERF_FUNC__VerifyCertificateRequiredTags, 1 ), 0, 0 },
    { "DRM_WCP_VerifyCertificateCollection", PERF_SCOPE_ID( PERF_MOD_NDT, PERF_FUNC_DRM_WCP_VerifyCertificateCollection, 1 ), 0, 0 },
    { "_GetPublicKey", PERF_SCOPE_ID( PERF_MOD_NDT, PERF_FUNC__GetPublicKey, 1 ), 0, 0 },
    { "DRM_WCP_VerifyDigitalSignature", PERF_SCOPE_ID( PERF_MOD_NDT, PERF_FUNC_DRM_WCP_VerifyDigitalSignature, 1 ), 0, 0 },
    { "_ExtractPublicKey", PERF_SCOPE_ID( PERF_MOD_NDT, PERF_FUNC__ExtractPublicKey, 1 ), 0, 0 },
    { "Drm_Pfd_Initialize", PERF_SCOPE_ID( PERF_MOD_NDT, PERF_FUNC_Drm_Pfd_Initialize, 1 ), 0, 0 },
    { "Drm_Pfd_Registration_ProcessRequest", PERF_SCOPE_ID( PERF_MOD_NDT, PERF_FUNC_Drm_Pfd_Registration_ProcessRequest, 1 ), 0, 0 },
    { "Drm_Pfd_Registration_GenerateResponse", PERF_SCOPE_ID( PERF_MOD_NDT, PERF_FUNC_Drm_Pfd_Registration_GenerateResponse, 1 ), 0, 0 },
    { "Drm_Pfd_Proximity_GenerateChallenge", PERF_SCOPE_ID( PERF_MOD_NDT, PERF_FUNC_Drm_Pfd_Proximity_GenerateChallenge, 1 ), 0, 0 },
    { "Drm_Pfd_Proximity_ProcessResponse", PERF_SCOPE_ID( PERF_MOD_NDT, PERF_FUNC_Drm_Pfd_Proximity_ProcessResponse, 1 ), 0, 0 },
    { "Drm_Pfd_License_ProcessRequest", PERF_SCOPE_ID( PERF_MOD_NDT, PERF_FUNC_Drm_Pfd_License_ProcessRequest, 1 ), 0, 0 },
    { "Drm_Pfd_License_GenerateResponse", PERF_SCOPE_ID( PERF_MOD_NDT, PERF_FUNC_Drm_Pfd_License_GenerateResponse, 1 ), 0, 0 },
    { "_OutputLevelsCallback", PERF_SCOPE_ID( PERF_MOD_NDT, PERF_FUNC__OutputLevelsCallback, 1 ), 0, 0 },
    { "NDT_RetrieveLicenseDataV1", PERF_SCOPE_ID( PERF_MOD_NDT, PERF_FUNC_NDT_RetrieveLicenseDataV1, 1 ), 0, 0 },
    { "_RetrieveLicense", PERF_SCOPE_ID( PERF_MOD_NDT, PERF_FUNC__RetrieveLicense, 1 ), 0, 0 },
    { "NDT_RetrieveLicenseDataV2", PERF_SCOPE_ID( PERF_MOD_NDT, PERF_FUNC_NDT_RetrieveLicenseDataV2, 1 ), 0, 0 },
    { "_DRM_XMR_RootSignatureCallback", PERF_SCOPE_ID( PERF_MOD_NDT, PERF_FUNC__DRM_XMR_RootSignatureCallback, 1 ), 0, 0 },
    { "_DRM_XMR_LeafSignatureCallback", PERF_SCOPE_ID( PERF_MOD_NDT, PERF_FUNC__DRM_XMR_LeafSignatureCallback, 1 ), 0, 0 },
    { "NDT_License_BuildSimpleXMR", PERF_SCOPE_ID( PERF_MOD_NDT, PERF_FUNC_NDT_License_BuildSimpleXMR, 1 ), 0, 0 },
    { "NDT_License_BuildRootXMR", PERF_SCOPE_ID( PERF_MOD_NDT, PERF_FUNC_NDT_License_BuildRootXMR, 1 ), 0, 0 },
    { "NDT_License_BuildLeafXMR", PERF_SCOPE_ID( PERF_MOD_NDT, PERF_FUNC_NDT_License_BuildLeafXMR, 1 ), 0, 0 },
    { "Drm_Ndt_Initialize", PERF_SCOPE_ID( PERF_MOD_NDT, PERF_FUNC_Drm_Ndt_Initialize, 1 ), 0, 0 },
    { "Drm_Ndt_Session_Create", PERF_SCOPE_ID( PERF_MOD_NDT, PERF_FUNC_Drm_Ndt_Session_Create, 1 ), 0, 0 },
    { "Drm_Ndt_Session_Close", PERF_SCOPE_ID( PERF_MOD_NDT, PERF_FUNC_Drm_Ndt_Session_Close, 1 ), 0, 0 },
    { "Drm_Ndt_Registration_ProcessRequest", PERF_SCOPE_ID( PERF_MOD_NDT, PERF_FUNC_Drm_Ndt_Registration_ProcessRequest, 1 ), 0, 0 },
    { "DRM_NDT_Proximity_GetChallenge", PERF_SCOPE_ID( PERF_MOD_NDT, PERF_FUNC_DRM_NDT_Proximity_GetChallenge, 1 ), 0, 0 },
    { "Drm_Ndt_Proximity_ProcessStartMsg", PERF_SCOPE_ID( PERF_MOD_NDT, PERF_FUNC_Drm_Ndt_Proximity_ProcessStartMsg, 1 ), 0, 0 },
    { "DRM_NDT_Proximity_VerifyEncryptedChallenge", PERF_SCOPE_ID( PERF_MOD_NDT, PERF_FUNC_DRM_NDT_Proximity_VerifyEncryptedChallenge, 1 ), 0, 0 },
    { "_DecryptNonce", PERF_SCOPE_ID( PERF_MOD_NDT, PERF_FUNC__DecryptNonce, 1 ), 0, 0 },
    { "Drm_Ndt_Proximity_ProcessResponseMsg", PERF_SCOPE_ID( PERF_MOD_NDT, PERF_FUNC_Drm_Ndt_Proximity_ProcessResponseMsg, 1 ), 0, 0 },
    { "Drm_Ndt_License_ProcessChallenge", PERF_SCOPE_ID( PERF_MOD_NDT, PERF_FUNC_Drm_Ndt_License_ProcessChallenge, 1 ), 0, 0 },
    { "DRM_NDT_License_GenerateKeys", PERF_SCOPE_ID( PERF_MOD_NDT, PERF_FUNC_DRM_NDT_License_GenerateKeys, 1 ), 0, 0 },
    { "_GetDeviceHandle", PERF_SCOPE_ID( PERF_MOD_NDT, PERF_FUNC__GetDeviceHandle, 1 ), 0, 0 },
    { "DRM_NDT_License_SetRootPolicy", PERF_SCOPE_ID( PERF_MOD_NDT, PERF_FUNC_DRM_NDT_License_SetRootPolicy, 1 ), 0, 0 },
    { "Drm_Ndt_License_GenerateLeafResponse", PERF_SCOPE_ID( PERF_MOD_NDT, PERF_FUNC_Drm_Ndt_License_GenerateLeafResponse, 1 ), 0, 0 },
    { "Drm_Ndt_RevocationList_GenerateChallenge", PERF_SCOPE_ID( PERF_MOD_NDT, PERF_FUNC_Drm_Ndt_RevocationList_GenerateChallenge, 1 ), 0, 0 },
    { "Drm_Ndt_RevocationList_ProcessRequest", PERF_SCOPE_ID( PERF_MOD_NDT, PERF_FUNC_Drm_Ndt_RevocationList_ProcessRequest, 1 ), 0, 0 },
    { "_GUIDFromDrmRevocationTypeEnum", PERF_SCOPE_ID( PERF_MOD_NDT, PERF_FUNC__GUIDFromDrmRevocationTypeEnum, 1 ), 0, 0 },
    { "Drm_Ndt_RevocationList_ProcessResponse", PERF_SCOPE_ID( PERF_MOD_NDT, PERF_FUNC_Drm_Ndt_RevocationList_ProcessResponse, 1 ), 0, 0 },
    { "GetWMDRMNETRevocationEntries", PERF_SCOPE_ID( PERF_MOD_NDT, PERF_FUNC_GetWMDRMNETRevocationEntries, 1 ), 0, 0 },
    { "GetWMDRMNDRevocationList", PERF_SCOPE_ID( PERF_MOD_NDT, PERF_FUNC_GetWMDRMNDRevocationList, 1 ), 0, 0 },
    { "NDTMGR_Internal_FindSession", PERF_SCOPE_ID( PERF_MOD_NDT, PERF_FUNC_NDTMGR_Internal_FindSession, 1 ), 0, 0 },
    { "NDTMGR_Internal_CreateSession", PERF_SCOPE_ID( PERF_MOD_NDT, PERF_FUNC_NDTMGR_Internal_CreateSession, 1 ), 0, 0 },
    { "NDTMGR_Internal_RemoveSession", PERF_SCOPE_ID( PERF_MOD_NDT, PERF_FUNC_NDTMGR_Internal_RemoveSession, 1 ), 0, 0 },
    { "_NdtManagerFreeContentSession", PERF_SCOPE_ID( PERF_MOD_NDT, PERF_FUNC__NdtManagerFreeContentSession, 1 ), 0, 0 },
    { "NDT_ValidateCertificate", PERF_SCOPE_ID( PERF_MOD_NDT, PERF_FUNC_NDT_ValidateCertificate, 1 ), 0, 0 },
    { "_GetCertificateAttributes", PERF_SCOPE_ID( PERF_MOD_NDT, PERF_FUNC__GetCertificateAttributes, 1 ), 0, 0 },
    { "NDT_EncryptSeed", PERF_SCOPE_ID( PERF_MOD_NDT, PERF_FUNC_NDT_EncryptSeed, 1 ), 0, 0 },
    { "NDT_SignRegistrationMessage", PERF_SCOPE_ID( PERF_MOD_NDT, PERF_FUNC_NDT_SignRegistrationMessage, 1 ), 0, 0 },
    { "NDTMGR_Encrypt", PERF_SCOPE_ID( PERF_MOD_NDT, PERF_FUNC_NDTMGR_Encrypt, 1 ), 0, 0 },
    { "_PackDataSegmentDescriptor", PERF_SCOPE_ID( PERF_MOD_NDT, PERF_FUNC__PackDataSegmentDescriptor, 1 ), 0, 0 },
    { "Drm_Pfd_Aavt_OpenMediaSession", PERF_SCOPE_ID( PERF_MOD_NDT, PERF_FUNC_Drm_Pfd_Aavt_OpenMediaSession, 1 ), 0, 0 },
    { "Drm_Pfd_Aavt_CloseMediaSession", PERF_SCOPE_ID( PERF_MOD_NDT, PERF_FUNC_Drm_Pfd_Aavt_CloseMediaSession, 1 ), 0, 0 },
    { "Drm_Pfd_Aavt_GetNextDataBlock", PERF_SCOPE_ID( PERF_MOD_NDT, PERF_FUNC_Drm_Pfd_Aavt_GetNextDataBlock, 1 ), 0, 0 },
    { "Drm_Pfd_Aavt_SetCurrentTimePosition", PERF_SCOPE_ID( PERF_MOD_NDT, PERF_FUNC_Drm_Pfd_Aavt_SetCurrentTimePosition, 1 ), 0, 0 },
    { "_PfdDrmManagerFreeContentSession", PERF_SCOPE_ID( PERF_MOD_NDT, PERF_FUNC__PfdDrmManagerFreeContentSession, 1 ), 0, 0 },
    { "_PfdDrmManagerFreeAllContentSessions", PERF_SCOPE_ID( PERF_MOD_NDT, PERF_FUNC__PfdDrmManagerFreeAllContentSessions, 1 ), 0, 0 },
    { "_PfdDrmManagerGetSession", PERF_SCOPE_ID( PERF_MOD_NDT, PERF_FUNC__PfdDrmManagerGetSession, 1 ), 0, 0 },
    { "OEM_NDT_AllowNewDeviceRegistration", PERF_SCOPE_ID( PERF_MOD_NDT, PERF_FUNC_OEM_NDT_AllowNewDeviceRegistration, 1 ), 0, 0 },
    { "OEM_NDT_GetProximityDetectionAddress", PERF_SCOPE_ID( PERF_MOD_NDT, PERF_FUNC_OEM_NDT_GetProximityDetectionAddress, 1 ), 0, 0 },
    { "OEM_NDT_GetLicenseOPLs", PERF_SCOPE_ID( PERF_MOD_NDT, PERF_FUNC_OEM_NDT_GetLicenseOPLs, 1 ), 0, 0 },
    { "OEM_NDT_GetLicenseInclusionList", PERF_SCOPE_ID( PERF_MOD_NDT, PERF_FUNC_OEM_NDT_GetLicenseInclusionList, 1 ), 0, 0 },
    { "OEM_NDT_GetSourceID", PERF_SCOPE_ID( PERF_MOD_NDT, PERF_FUNC_OEM_NDT_GetSourceID, 1 ), 0, 0 },
    { "OEM_NDT_GetLicenseRevInfoVersion", PERF_SCOPE_ID( PERF_MOD_NDT, PERF_FUNC_OEM_NDT_GetLicenseRevInfoVersion, 1 ), 0, 0 },
    { "OEM_NDT_GetWMDRMNETMinAppSec", PERF_SCOPE_ID( PERF_MOD_NDT, PERF_FUNC_OEM_NDT_GetWMDRMNETMinAppSec, 1 ), 0, 0 },

    /* NDT NetMsg */
    /* ndtmgr\ndtnetmsg.c */
    { "WmdrmNetAllocateMessageDescriptor", PERF_SCOPE_ID( PERF_MOD_NDT_NETMSG, PERF_FUNC_WmdrmNetAllocateMessageDescriptor, 1 ), 0, 0 },
    { "_ValidateMessage", PERF_SCOPE_ID( PERF_MOD_NDT_NETMSG, PERF_FUNC__ValidateMessage, 1 ), 0, 0 },
    { "WmdrmNetUnpackRegistrationRequestMessage", PERF_SCOPE_ID( PERF_MOD_NDT_NETMSG, PERF_FUNC_WmdrmNetUnpackRegistrationRequestMessage, 1 ), 0, 0 },
    { "WmdrmNetPackRegistrationResponseMessage", PERF_SCOPE_ID( PERF_MOD_NDT_NETMSG, PERF_FUNC_WmdrmNetPackRegistrationResponseMessage, 1 ), 0, 0 },
    { "WmdrmNetPackRegistrationMessage", PERF_SCOPE_ID( PERF_MOD_NDT_NETMSG, PERF_FUNC_WmdrmNetPackRegistrationMessage, 1 ), 0, 0 },
    { "WmdrmNetUnpackRegistrationResponseMessage", PERF_SCOPE_ID( PERF_MOD_NDT_NETMSG, PERF_FUNC_WmdrmNetUnpackRegistrationResponseMessage, 1 ), 0, 0 },
    { "WmdrmNetUnpackProximityStartMessage", PERF_SCOPE_ID( PERF_MOD_NDT_NETMSG, PERF_FUNC_WmdrmNetUnpackProximityStartMessage, 1 ), 0, 0 },
    { "WmdrmNetPackProximityChallengeMessage", PERF_SCOPE_ID( PERF_MOD_NDT_NETMSG, PERF_FUNC_WmdrmNetPackProximityChallengeMessage, 1 ), 0, 0 },
    { "WmdrmNetUnpackProximityResponseMessage", PERF_SCOPE_ID( PERF_MOD_NDT_NETMSG, PERF_FUNC_WmdrmNetUnpackProximityResponseMessage, 1 ), 0, 0 },
    { "WmdrmNetPackProximityResultMessage", PERF_SCOPE_ID( PERF_MOD_NDT_NETMSG, PERF_FUNC_WmdrmNetPackProximityResultMessage, 1 ), 0, 0 },
    { "WmdrmNetPackProximityResponseMessage", PERF_SCOPE_ID( PERF_MOD_NDT_NETMSG, PERF_FUNC_WmdrmNetPackProximityResponseMessage, 1 ), 0, 0 },
    { "WmdrmNetUnpackProximityChallengeMessage", PERF_SCOPE_ID( PERF_MOD_NDT_NETMSG, PERF_FUNC_WmdrmNetUnpackProximityChallengeMessage, 1 ), 0, 0 },
    { "WmdrmNetUnpackProximityResultMessage", PERF_SCOPE_ID( PERF_MOD_NDT_NETMSG, PERF_FUNC_WmdrmNetUnpackProximityResultMessage, 1 ), 0, 0 },
    { "WmdrmNetPackProximityStartMessage", PERF_SCOPE_ID( PERF_MOD_NDT_NETMSG, PERF_FUNC_WmdrmNetPackProximityStartMessage, 1 ), 0, 0 },
    { "WmdrmNetUnpackLicenseRequestMessage", PERF_SCOPE_ID( PERF_MOD_NDT_NETMSG, PERF_FUNC_WmdrmNetUnpackLicenseRequestMessage, 1 ), 0, 0 },
    { "WmdrmNetPackLicenseResponseMessage", PERF_SCOPE_ID( PERF_MOD_NDT_NETMSG, PERF_FUNC_WmdrmNetPackLicenseResponseMessage, 1 ), 0, 0 },
    { "WmdrmNetPackLicenseRequestMessage", PERF_SCOPE_ID( PERF_MOD_NDT_NETMSG, PERF_FUNC_WmdrmNetPackLicenseRequestMessage, 1 ), 0, 0 },
    { "WmdrmNetUnpackLicenseResponseMessage", PERF_SCOPE_ID( PERF_MOD_NDT_NETMSG, PERF_FUNC_WmdrmNetUnpackLicenseResponseMessage, 1 ), 0, 0 },
    { "WmdrmNetUnpackRevocationListRequestMessage", PERF_SCOPE_ID( PERF_MOD_NDT_NETMSG, PERF_FUNC_WmdrmNetUnpackRevocationListRequestMessage, 1 ), 0, 0 },
    { "WmdrmNetPackRevocationListResponseMessage", PERF_SCOPE_ID( PERF_MOD_NDT_NETMSG, PERF_FUNC_WmdrmNetPackRevocationListResponseMessage, 1 ), 0, 0 },
    { "WmdrmNetPackRevocationListRequestMessage", PERF_SCOPE_ID( PERF_MOD_NDT_NETMSG, PERF_FUNC_WmdrmNetPackRevocationListRequestMessage, 1 ), 0, 0 },
    { "WmdrmNetUnpackRevocationListResponseMessage", PERF_SCOPE_ID( PERF_MOD_NDT_NETMSG, PERF_FUNC_WmdrmNetUnpackRevocationListResponseMessage, 1 ), 0, 0 },

    /* NDT ASF */
    /* ndtmgr\ndtlicense.c */
    /* ndtasf\drmndtasf.c */
    /* ndtasf\ndtasfdrm.c */
    /* ndtasf\ndtasf_internal.c */
    { "DRM_NDT_License_CreateDefaultPolicy", PERF_SCOPE_ID( PERF_MOD_NDT_ASF, PERF_FUNC_DRM_NDT_License_CreateDefaultPolicy, 1 ), 0, 0 },
    { "Drm_Ndt_Asf_Init", PERF_SCOPE_ID( PERF_MOD_NDT_ASF, PERF_FUNC_Drm_Ndt_Asf_Init, 1 ), 0, 0 },
    { "Drm_Ndt_Asf_InitTranscryption", PERF_SCOPE_ID( PERF_MOD_NDT_ASF, PERF_FUNC_Drm_Ndt_Asf_InitTranscryption, 1 ), 0, 0 },
    { "Drm_Ndt_Asf_GetHeader", PERF_SCOPE_ID( PERF_MOD_NDT_ASF, PERF_FUNC_Drm_Ndt_Asf_GetHeader, 1 ), 0, 0 },
    { "Drm_Ndt_Asf_GenerateLeafLicenseResponse", PERF_SCOPE_ID( PERF_MOD_NDT_ASF, PERF_FUNC_Drm_Ndt_Asf_GenerateLeafLicenseResponse, 1 ), 0, 0 },
    { "Drm_Ndt_Asf_Seek", PERF_SCOPE_ID( PERF_MOD_NDT_ASF, PERF_FUNC_Drm_Ndt_Asf_Seek, 1 ), 0, 0 },
    { "Drm_Ndt_Asf_ReadData", PERF_SCOPE_ID( PERF_MOD_NDT_ASF, PERF_FUNC_Drm_Ndt_Asf_ReadData, 1 ), 0, 0 },
    { "Drm_Ndt_Asf_ReadSample", PERF_SCOPE_ID( PERF_MOD_NDT_ASF, PERF_FUNC_Drm_Ndt_Asf_ReadSample, 1 ), 0, 0 },
    { "Drm_Ndt_Asf_Close", PERF_SCOPE_ID( PERF_MOD_NDT_ASF, PERF_FUNC_Drm_Ndt_Asf_Close, 1 ), 0, 0 },
    { "NDTMGR_ASF_Internal_CheckFileDRM", PERF_SCOPE_ID( PERF_MOD_NDT_ASF, PERF_FUNC_NDTMGR_ASF_Internal_CheckFileDRM, 1 ), 0, 0 },
    { "_ASFParsePacketHeader", PERF_SCOPE_ID( PERF_MOD_NDT_ASF, PERF_FUNC__ASFParsePacketHeader, 1 ), 0, 0 },
    { "_ASFReadData", PERF_SCOPE_ID( PERF_MOD_NDT_ASF, PERF_FUNC__ASFReadData, 1 ), 0, 0 },
    { "_ASFParseLengthType", PERF_SCOPE_ID( PERF_MOD_NDT_ASF, PERF_FUNC__ASFParseLengthType, 1 ), 0, 0 },
    { "_ASFParsePayloadHeader", PERF_SCOPE_ID( PERF_MOD_NDT_ASF, PERF_FUNC__ASFParsePayloadHeader, 1 ), 0, 0 },
    { "NDTMGR_ASF_Internal_InitializeTranscryption", PERF_SCOPE_ID( PERF_MOD_NDT_ASF, PERF_FUNC_NDTMGR_ASF_Internal_InitializeTranscryption, 1 ), 0, 0 },
    { "NDTMGR_ASF_Internal_Initialize", PERF_SCOPE_ID( PERF_MOD_NDT_ASF, PERF_FUNC_NDTMGR_ASF_Internal_Initialize, 1 ), 0, 0 },
    { "NDTMGR_ASF_Internal_GetFileInfo", PERF_SCOPE_ID( PERF_MOD_NDT_ASF, PERF_FUNC_NDTMGR_ASF_Internal_GetFileInfo, 1 ), 0, 0 },
    { "_ASFParseHeaders", PERF_SCOPE_ID( PERF_MOD_NDT_ASF, PERF_FUNC__ASFParseHeaders, 1 ), 0, 0 },
    { "_ASFLoadHeaderObject", PERF_SCOPE_ID( PERF_MOD_NDT_ASF, PERF_FUNC__ASFLoadHeaderObject, 1 ), 0, 0 },
    { "_ASFLoadObjectHeader", PERF_SCOPE_ID( PERF_MOD_NDT_ASF, PERF_FUNC__ASFLoadObjectHeader, 1 ), 0, 0 },
    { "_ASFParseFilePropertiesObject", PERF_SCOPE_ID( PERF_MOD_NDT_ASF, PERF_FUNC__ASFParseFilePropertiesObject, 1 ), 0, 0 },
    { "_ASFParseStreamPropertiesObject", PERF_SCOPE_ID( PERF_MOD_NDT_ASF, PERF_FUNC__ASFParseStreamPropertiesObject, 1 ), 0, 0 },
    { "_AddToStreamInfoList", PERF_SCOPE_ID( PERF_MOD_NDT_ASF, PERF_FUNC__AddToStreamInfoList, 1 ), 0, 0 },
    { "_FindStreamInStreamInfoList", PERF_SCOPE_ID( PERF_MOD_NDT_ASF, PERF_FUNC__FindStreamInStreamInfoList, 1 ), 0, 0 },
    { "_ASFParseHeaderExtensionObject", PERF_SCOPE_ID( PERF_MOD_NDT_ASF, PERF_FUNC__ASFParseHeaderExtensionObject, 1 ), 0, 0 },
    { "_ASFParseStreamPropertiesObjectEx", PERF_SCOPE_ID( PERF_MOD_NDT_ASF, PERF_FUNC__ASFParseStreamPropertiesObjectEx, 1 ), 0, 0 },
    { "_ASFParseStreamBitratePropertiesObject", PERF_SCOPE_ID( PERF_MOD_NDT_ASF, PERF_FUNC__ASFParseStreamBitratePropertiesObject, 1 ), 0, 0 },
    { "_ASFTranscryptNextBlock", PERF_SCOPE_ID( PERF_MOD_NDT_ASF, PERF_FUNC__ASFTranscryptNextBlock, 1 ), 0, 0 },
    { "_ASFSetHeaderDone", PERF_SCOPE_ID( PERF_MOD_NDT_ASF, PERF_FUNC__ASFSetHeaderDone, 1 ), 0, 0 },
    { "_ASFTranscryptHeader", PERF_SCOPE_ID( PERF_MOD_NDT_ASF, PERF_FUNC__ASFTranscryptHeader, 1 ), 0, 0 },
    { "_ASFProcessHeaders", PERF_SCOPE_ID( PERF_MOD_NDT_ASF, PERF_FUNC__ASFProcessHeaders, 1 ), 0, 0 },
    { "_CopyObjectToHeaderBuffer", PERF_SCOPE_ID( PERF_MOD_NDT_ASF, PERF_FUNC__CopyObjectToHeaderBuffer, 1 ), 0, 0 },
    { "_AppendToHeaderBuffer", PERF_SCOPE_ID( PERF_MOD_NDT_ASF, PERF_FUNC__AppendToHeaderBuffer, 1 ), 0, 0 },
    { "_SaveHeaderObjectToNewHeader", PERF_SCOPE_ID( PERF_MOD_NDT_ASF, PERF_FUNC__SaveHeaderObjectToNewHeader, 1 ), 0, 0 },
    { "_ASFProcessContentEncryptionObject", PERF_SCOPE_ID( PERF_MOD_NDT_ASF, PERF_FUNC__ASFProcessContentEncryptionObject, 1 ), 0, 0 },
    { "_ASFProcessFilePropertiesObject", PERF_SCOPE_ID( PERF_MOD_NDT_ASF, PERF_FUNC__ASFProcessFilePropertiesObject, 1 ), 0, 0 },
    { "_ASFProcessStreamPropertiesObject", PERF_SCOPE_ID( PERF_MOD_NDT_ASF, PERF_FUNC__ASFProcessStreamPropertiesObject, 1 ), 0, 0 },
    { "_ASFProcessHeaderExtensionObject", PERF_SCOPE_ID( PERF_MOD_NDT_ASF, PERF_FUNC__ASFProcessHeaderExtensionObject, 1 ), 0, 0 },
    { "_ASFProcessStreamPropertiesObjectEx", PERF_SCOPE_ID( PERF_MOD_NDT_ASF, PERF_FUNC__ASFProcessStreamPropertiesObjectEx, 1 ), 0, 0 },
    { "_ASFAppendAdvContentEncryptionObject", PERF_SCOPE_ID( PERF_MOD_NDT_ASF, PERF_FUNC__ASFAppendAdvContentEncryptionObject, 1 ), 0, 0 },
    { "_ASFTranscryptNextPackets", PERF_SCOPE_ID( PERF_MOD_NDT_ASF, PERF_FUNC__ASFTranscryptNextPackets, 1 ), 0, 0 },
    { "_ASFTranscryptData", PERF_SCOPE_ID( PERF_MOD_NDT_ASF, PERF_FUNC__ASFTranscryptData, 1 ), 0, 0 },
    { "_ASFTranscryptPacket", PERF_SCOPE_ID( PERF_MOD_NDT_ASF, PERF_FUNC__ASFTranscryptPacket, 1 ), 0, 0 },
    { "_AppendToDataBuffer", PERF_SCOPE_ID( PERF_MOD_NDT_ASF, PERF_FUNC__AppendToDataBuffer, 1 ), 0, 0 },
    { "_ASFProcessPacketHeader", PERF_SCOPE_ID( PERF_MOD_NDT_ASF, PERF_FUNC__ASFProcessPacketHeader, 1 ), 0, 0 },
    { "_ParseLengthType", PERF_SCOPE_ID( PERF_MOD_NDT_ASF, PERF_FUNC__ParseLengthType, 1 ), 0, 0 },
    { "_ASFProcessPayloadHeader", PERF_SCOPE_ID( PERF_MOD_NDT_ASF, PERF_FUNC__ASFProcessPayloadHeader, 1 ), 0, 0 },
    { "_DRMDecrypt", PERF_SCOPE_ID( PERF_MOD_NDT_ASF, PERF_FUNC__DRMDecrypt, 1 ), 0, 0 },
    { "_ASFTranscryptDataObjectHeader", PERF_SCOPE_ID( PERF_MOD_NDT_ASF, PERF_FUNC__ASFTranscryptDataObjectHeader, 1 ), 0, 0 },
    { "_FillOutputBufferWithMuxPackets", PERF_SCOPE_ID( PERF_MOD_NDT_ASF, PERF_FUNC__FillOutputBufferWithMuxPackets, 1 ), 0, 0 },
    { "_ASFSeekToTimePosition", PERF_SCOPE_ID( PERF_MOD_NDT_ASF, PERF_FUNC__ASFSeekToTimePosition, 1 ), 0, 0 },
    { "NDTMGR_ASF_Internal_SetTimePosition", PERF_SCOPE_ID( PERF_MOD_NDT_ASF, PERF_FUNC_NDTMGR_ASF_Internal_SetTimePosition, 1 ), 0, 0 },
    { "NDTMGR_ASF_Internal_Uninitialize", PERF_SCOPE_ID( PERF_MOD_NDT_ASF, PERF_FUNC_NDTMGR_ASF_Internal_Uninitialize, 1 ), 0, 0 },
    { "NDTMGR_ASF_Internal_GetHeader", PERF_SCOPE_ID( PERF_MOD_NDT_ASF, PERF_FUNC_NDTMGR_ASF_Internal_GetHeader, 1 ), 0, 0 },
    { "NDTMGR_ASF_Internal_ReadSample", PERF_SCOPE_ID( PERF_MOD_NDT_ASF, PERF_FUNC_NDTMGR_ASF_Internal_ReadSample, 1 ), 0, 0 },
    { "_GetNextReadySample", PERF_SCOPE_ID( PERF_MOD_NDT_ASF, PERF_FUNC__GetNextReadySample, 1 ), 0, 0 },
    { "_GetSampleInProgress", PERF_SCOPE_ID( PERF_MOD_NDT_ASF, PERF_FUNC__GetSampleInProgress, 1 ), 0, 0 },
    { "NDTMGR_ASF_Internal_ReadData", PERF_SCOPE_ID( PERF_MOD_NDT_ASF, PERF_FUNC_NDTMGR_ASF_Internal_ReadData, 1 ), 0, 0 },
    { "_ASFSetupTranscryption", PERF_SCOPE_ID( PERF_MOD_NDT_ASF, PERF_FUNC__ASFSetupTranscryption, 1 ), 0, 0 },

    /* NDT ASF Sample */
    /* ndtasf\asfsample.c */
    { "ASFSample_Initialize", PERF_SCOPE_ID( PERF_MOD_NDT_ASF_SAMPLE, PERF_FUNC_ASFSample_Initialize, 1 ), 0, 0 },
    { "ASFSample_GetBufferByIndex", PERF_SCOPE_ID( PERF_MOD_NDT_ASF_SAMPLE, PERF_FUNC_ASFSample_GetBufferByIndex, 1 ), 0, 0 },
    { "ASFSample_AddBuffer", PERF_SCOPE_ID( PERF_MOD_NDT_ASF_SAMPLE, PERF_FUNC_ASFSample_AddBuffer, 1 ), 0, 0 },
    { "ASFSample_CopyToBuffer", PERF_SCOPE_ID( PERF_MOD_NDT_ASF_SAMPLE, PERF_FUNC_ASFSample_CopyToBuffer, 1 ), 0, 0 },

    /* NDT ASF Mux */
    /* ndtasf\mux.c */
    /* ndtasf\muxstream.c */
    { "ASFMux_Initialize", PERF_SCOPE_ID( PERF_MOD_NDT_ASF_MUX, PERF_FUNC_ASFMux_Initialize, 1 ), 0, 0 },
    { "ASFMux_ProcessSample", PERF_SCOPE_ID( PERF_MOD_NDT_ASF_MUX, PERF_FUNC_ASFMux_ProcessSample, 1 ), 0, 0 },
    { "ASFMux_GetNextPacket", PERF_SCOPE_ID( PERF_MOD_NDT_ASF_MUX, PERF_FUNC_ASFMux_GetNextPacket, 1 ), 0, 0 },
    { "ASFMux_Flush", PERF_SCOPE_ID( PERF_MOD_NDT_ASF_MUX, PERF_FUNC_ASFMux_Flush, 1 ), 0, 0 },
    { "ASFMux_OnCompletedPacket", PERF_SCOPE_ID( PERF_MOD_NDT_ASF_MUX, PERF_FUNC_ASFMux_OnCompletedPacket, 1 ), 0, 0 },
    { "ASFMux_GetMinStreamSendtimeCutoff", PERF_SCOPE_ID( PERF_MOD_NDT_ASF_MUX, PERF_FUNC_ASFMux_GetMinStreamSendtimeCutoff, 1 ), 0, 0 },
    { "ASFMux_AdjustSendTimeForOffset", PERF_SCOPE_ID( PERF_MOD_NDT_ASF_MUX, PERF_FUNC_ASFMux_AdjustSendTimeForOffset, 1 ), 0, 0 },
    { "_CreateMuxStream", PERF_SCOPE_ID( PERF_MOD_NDT_ASF_MUX, PERF_FUNC_CreateMuxStream, 1 ), 0, 0 },
    { "_SizeQueueObjectsArray", PERF_SCOPE_ID( PERF_MOD_NDT_ASF_MUX, PERF_FUNC_SizeQueueObjectsArray, 1 ), 0, 0 },
    { "ASFMuxStream_Initialize", PERF_SCOPE_ID( PERF_MOD_NDT_ASF_MUX, PERF_FUNC_ASFMuxStream_Initialize, 1 ), 0, 0 },
    { "ASFMuxStream_HowManyObjectsWillBeReturned", PERF_SCOPE_ID( PERF_MOD_NDT_ASF_MUX, PERF_FUNC_ASFMuxStream_HowManyObjectsWillBeReturned, 1 ), 0, 0 },
    { "ASFMuxStream_OnSample", PERF_SCOPE_ID( PERF_MOD_NDT_ASF_MUX, PERF_FUNC_ASFMuxStream_OnSample, 1 ), 0, 0 },
    { "OnSample", PERF_SCOPE_ID( PERF_MOD_NDT_ASF_MUX, PERF_FUNC_OnSample, 1 ), 0, 0 },
    { "ASFMuxStream_GetObjectSizeForBandwidthPolicing", PERF_SCOPE_ID( PERF_MOD_NDT_ASF_MUX, PERF_FUNC_ASFMuxStream_GetObjectSizeForBandwidthPolicing, 1 ), 0, 0 },
    { "ASFMuxStream_ApproxSampleDuration", PERF_SCOPE_ID( PERF_MOD_NDT_ASF_MUX, PERF_FUNC_ASFMuxStream_ApproxSampleDuration, 1 ), 0, 0 },
    { "ApproxSampleDuration", PERF_SCOPE_ID( PERF_MOD_NDT_ASF_MUX, PERF_FUNC_ApproxSampleDuration, 1 ), 0, 0 },
    { "ASFMuxStream_GetHappyCutoffSendtime", PERF_SCOPE_ID( PERF_MOD_NDT_ASF_MUX, PERF_FUNC_ASFMuxStream_GetHappyCutoffSendtime, 1 ), 0, 0 },
    { "GetHappyCutoffSendtime", PERF_SCOPE_ID( PERF_MOD_NDT_ASF_MUX, PERF_FUNC_GetHappyCutoffSendtime, 1 ), 0, 0 },
    { "ASFMuxStream_AdjustSendTimeForOffset", PERF_SCOPE_ID( PERF_MOD_NDT_ASF_MUX, PERF_FUNC_ASFMuxStream_AdjustSendTimeForOffset, 1 ), 0, 0 },
    { "AdjustSendTimeForOffset", PERF_SCOPE_ID( PERF_MOD_NDT_ASF_MUX, PERF_FUNC_AdjustSendTimeForOffset, 1 ), 0, 0 },
    { "ASFMuxStream_ReadMediaType", PERF_SCOPE_ID( PERF_MOD_NDT_ASF_MUX, PERF_FUNC_ASFMuxStream_ReadMediaType, 1 ), 0, 0 },
    { "InternalInit", PERF_SCOPE_ID( PERF_MOD_NDT_ASF_MUX, PERF_FUNC_InternalInit, 1 ), 0, 0 },
    { "SetupLeakyBucket", PERF_SCOPE_ID( PERF_MOD_NDT_ASF_MUX, PERF_FUNC_SetupLeakyBucket, 1 ), 0, 0 },
    { "ProcessNewWriterSample", PERF_SCOPE_ID( PERF_MOD_NDT_ASF_MUX, PERF_FUNC_ProcessNewWriterSample, 1 ), 0, 0 },
    { "SendTimeFromPresTime", PERF_SCOPE_ID( PERF_MOD_NDT_ASF_MUX, PERF_FUNC_SendTimeFromPresTime, 1 ), 0, 0 },
    { "GetAdjustedSampleTime", PERF_SCOPE_ID( PERF_MOD_NDT_ASF_MUX, PERF_FUNC_GetAdjustedSampleTime, 1 ), 0, 0 },
    { "ASFAudioMuxStream_HowManyObjectsWillBeReturned", PERF_SCOPE_ID( PERF_MOD_NDT_ASF_MUX, PERF_FUNC_ASFAudioMuxStream_HowManyObjectsWillBeReturned, 1 ), 0, 0 },
    { "ASFAudioMuxStream_OnSample", PERF_SCOPE_ID( PERF_MOD_NDT_ASF_MUX, PERF_FUNC_ASFAudioMuxStream_OnSample, 1 ), 0, 0 },
    { "ASFAudioMuxStream_ApproxSampleDuration", PERF_SCOPE_ID( PERF_MOD_NDT_ASF_MUX, PERF_FUNC_ASFAudioMuxStream_ApproxSampleDuration, 1 ), 0, 0 },
    { "ASFAudioMuxStream_GetHappyCutoffSendtime", PERF_SCOPE_ID( PERF_MOD_NDT_ASF_MUX, PERF_FUNC_ASFAudioMuxStream_GetHappyCutoffSendtime, 1 ), 0, 0 },
    { "ASFVideoMuxStream_OnSample", PERF_SCOPE_ID( PERF_MOD_NDT_ASF_MUX, PERF_FUNC_ASFVideoMuxStream_OnSample, 1 ), 0, 0 },
    { "ASFVideoMuxStream_ApproxSampleDuration", PERF_SCOPE_ID( PERF_MOD_NDT_ASF_MUX, PERF_FUNC_ASFVideoMuxStream_ApproxSampleDuration, 1 ), 0, 0 },
    { "ASFVideoMuxStream_GetHappyCutoffSendtime", PERF_SCOPE_ID( PERF_MOD_NDT_ASF_MUX, PERF_FUNC_ASFVideoMuxStream_GetHappyCutoffSendtime, 1 ), 0, 0 },
    { "ASFVideoMuxStream_ReadMediaType", PERF_SCOPE_ID( PERF_MOD_NDT_ASF_MUX, PERF_FUNC_ASFVideoMuxStream_ReadMediaType, 1 ), 0, 0 },

    /* NDT ASF Streaming */
    /* ndtasf\sendwindow.c */
    /* ndtasf\leakybucket.c */
    /* ndtasf\packetwriter.c */
    /* ndtasf\asfmediabuffer.c */
    { "ASFMuxSendWindow_Initialize", PERF_SCOPE_ID( PERF_MOD_NDT_ASF_STREAM, PERF_FUNC_ASFMuxSendWindow_Initialize, 1 ), 0, 0 },
    { "ASFMuxSendWindow_AddObjectsAndSendPackets", PERF_SCOPE_ID( PERF_MOD_NDT_ASF_STREAM, PERF_FUNC_ASFMuxSendWindow_AddObjectsAndSendPackets, 1 ), 0, 0 },
    { "ASFMuxSendWindow_Flush", PERF_SCOPE_ID( PERF_MOD_NDT_ASF_STREAM, PERF_FUNC_ASFMuxSendWindow_Flush, 1 ), 0, 0 },
    { "InitializeWritePacket", PERF_SCOPE_ID( PERF_MOD_NDT_ASF_STREAM, PERF_FUNC_InitializeWritePacket, 1 ), 0, 0 },
    { "PacketizeAndSend", PERF_SCOPE_ID( PERF_MOD_NDT_ASF_STREAM, PERF_FUNC_PacketizeAndSend, 1 ), 0, 0 },
    { "PacketizeSample", PERF_SCOPE_ID( PERF_MOD_NDT_ASF_STREAM, PERF_FUNC_PacketizeSample, 1 ), 0, 0 },
    { "AddPayload", PERF_SCOPE_ID( PERF_MOD_NDT_ASF_STREAM, PERF_FUNC_AddPayload, 1 ), 0, 0 },
    { "FindFirstSampleThatFits", PERF_SCOPE_ID( PERF_MOD_NDT_ASF_STREAM, PERF_FUNC_FindFirstSampleThatFits, 1 ), 0, 0 },
    { "SendOffWritePacket", PERF_SCOPE_ID( PERF_MOD_NDT_ASF_STREAM, PERF_FUNC_SendOffWritePacket, 1 ), 0, 0 },
    { "PackageAndSend", PERF_SCOPE_ID( PERF_MOD_NDT_ASF_STREAM, PERF_FUNC_PackageAndSend, 1 ), 0, 0 },
    { "HandleFinishedQueueObject", PERF_SCOPE_ID( PERF_MOD_NDT_ASF_STREAM, PERF_FUNC_HandleFinishedQueueObject, 1 ), 0, 0 },
    { "ASFMuxQueueObject_Initialize", PERF_SCOPE_ID( PERF_MOD_NDT_ASF_STREAM, PERF_FUNC_ASFMuxQueueObject_Initialize, 1 ), 0, 0 },
    { "ASFMuxQueueObject_GeneratePayload", PERF_SCOPE_ID( PERF_MOD_NDT_ASF_STREAM, PERF_FUNC_ASFMuxQueueObject_GeneratePayload, 1 ), 0, 0 },
    { "ASFMuxQueueObject_FillInASFPayloadInfo", PERF_SCOPE_ID( PERF_MOD_NDT_ASF_STREAM, PERF_FUNC_ASFMuxQueueObject_FillInASFPayloadInfo, 1 ), 0, 0 },
    { "ASFMuxQueueObject_PopUsedSample", PERF_SCOPE_ID( PERF_MOD_NDT_ASF_STREAM, PERF_FUNC_ASFMuxQueueObject_PopUsedSample, 1 ), 0, 0 },
    { "ASFMuxQueueObject_AddSegment", PERF_SCOPE_ID( PERF_MOD_NDT_ASF_STREAM, PERF_FUNC_ASFMuxQueueObject_AddSegment, 1 ), 0, 0 },
    { "FragmentableObject_GeneratePayload", PERF_SCOPE_ID( PERF_MOD_NDT_ASF_STREAM, PERF_FUNC_FragmentableObject_GeneratePayload, 1 ), 0, 0 },
    { "FragmentableObject_PopUsedSample", PERF_SCOPE_ID( PERF_MOD_NDT_ASF_STREAM, PERF_FUNC_FragmentableObject_PopUsedSample, 1 ), 0, 0 },
    { "UnfragmentableObject_Initialize", PERF_SCOPE_ID( PERF_MOD_NDT_ASF_STREAM, PERF_FUNC_UnfragmentableObject_Initialize, 1 ), 0, 0 },
    { "UnfragmentableObject_GeneratePayload", PERF_SCOPE_ID( PERF_MOD_NDT_ASF_STREAM, PERF_FUNC_UnfragmentableObject_GeneratePayload, 1 ), 0, 0 },
    { "UnfragmentableObject_PopUsedSample", PERF_SCOPE_ID( PERF_MOD_NDT_ASF_STREAM, PERF_FUNC_UnfragmentableObject_PopUsedSample, 1 ), 0, 0 },
    { "UnfragmentableObject_AddSegment", PERF_SCOPE_ID( PERF_MOD_NDT_ASF_STREAM, PERF_FUNC_UnfragmentableObject_AddSegment, 1 ), 0, 0 },
    { "UnfragmentableObject_StartMFSample", PERF_SCOPE_ID( PERF_MOD_NDT_ASF_STREAM, PERF_FUNC_UnfragmentableObject_StartMFSample, 1 ), 0, 0 },
    { "ASFLeakyBucket_AddToBucket", PERF_SCOPE_ID( PERF_MOD_NDT_ASF_STREAM, PERF_FUNC_ASFLeakyBucket_AddToBucket, 1 ), 0, 0 },
    { "ASFPacketWriter_Initialize", PERF_SCOPE_ID( PERF_MOD_NDT_ASF_STREAM, PERF_FUNC_ASFPacketWriter_Initialize, 1 ), 0, 0 },
    { "ASFPacketWriter_SetPacketInfo", PERF_SCOPE_ID( PERF_MOD_NDT_ASF_STREAM, PERF_FUNC_ASFPacketWriter_SetPacketInfo, 1 ), 0, 0 },
    { "ASFPacketWriter_GetPacketInfo", PERF_SCOPE_ID( PERF_MOD_NDT_ASF_STREAM, PERF_FUNC_ASFPacketWriter_GetPacketInfo, 1 ), 0, 0 },
    { "ASFPacketWriter_GetDataSpaceAvailable", PERF_SCOPE_ID( PERF_MOD_NDT_ASF_STREAM, PERF_FUNC_ASFPacketWriter_GetDataSpaceAvailable, 1 ), 0, 0 },
    { "ASFPacketWriter_AddPayload", PERF_SCOPE_ID( PERF_MOD_NDT_ASF_STREAM, PERF_FUNC_ASFPacketWriter_AddPayload, 1 ), 0, 0 },
    { "ASFPacketWriter_WritePacket", PERF_SCOPE_ID( PERF_MOD_NDT_ASF_STREAM, PERF_FUNC_ASFPacketWriter_WritePacket, 1 ), 0, 0 },
    { "ASFPacketWriter_Clear", PERF_SCOPE_ID( PERF_MOD_NDT_ASF_STREAM, PERF_FUNC_ASFPacketWriter_Clear, 1 ), 0, 0 },
    { "InitializePacketInfoForWrite", PERF_SCOPE_ID( PERF_MOD_NDT_ASF_STREAM, PERF_FUNC_InitializePacketInfoForWrite, 1 ), 0, 0 },
    { "GetCurrentPacketSnapshot", PERF_SCOPE_ID( PERF_MOD_NDT_ASF_STREAM, PERF_FUNC_GetCurrentPacketSnapshot, 1 ), 0, 0 },
    { "RestorePacket", PERF_SCOPE_ID( PERF_MOD_NDT_ASF_STREAM, PERF_FUNC_RestorePacket, 1 ), 0, 0 },
    { "InternalAddPayloadInfo", PERF_SCOPE_ID( PERF_MOD_NDT_ASF_STREAM, PERF_FUNC_InternalAddPayloadInfo, 1 ), 0, 0 },
    { "CalcPayloadSize", PERF_SCOPE_ID( PERF_MOD_NDT_ASF_STREAM, PERF_FUNC_CalcPayloadSize, 1 ), 0, 0 },
    { "GetReplicatedData", PERF_SCOPE_ID( PERF_MOD_NDT_ASF_STREAM, PERF_FUNC_GetReplicatedData, 1 ), 0, 0 },
    { "WritePacketHeader", PERF_SCOPE_ID( PERF_MOD_NDT_ASF_STREAM, PERF_FUNC_WritePacketHeader, 1 ), 0, 0 },
    { "WritePayloadHeader", PERF_SCOPE_ID( PERF_MOD_NDT_ASF_STREAM, PERF_FUNC_WritePayloadHeader, 1 ), 0, 0 },
    { "WritePayloadData", PERF_SCOPE_ID( PERF_MOD_NDT_ASF_STREAM, PERF_FUNC_WritePayloadData, 1 ), 0, 0 },
    { "WritePadding", PERF_SCOPE_ID( PERF_MOD_NDT_ASF_STREAM, PERF_FUNC_WritePadding, 1 ), 0, 0 },
    { "WritePayloadDataWithoutEncrypt", PERF_SCOPE_ID( PERF_MOD_NDT_ASF_STREAM, PERF_FUNC_WritePayloadDataWithoutEncrypt, 1 ), 0, 0 },
    { "SaveVarField", PERF_SCOPE_ID( PERF_MOD_NDT_ASF_STREAM, PERF_FUNC_SaveVarField, 1 ), 0, 0 },
    { "ASFMediaBuffer_Initialize", PERF_SCOPE_ID( PERF_MOD_NDT_ASF_STREAM, PERF_FUNC_ASFMediaBuffer_Initialize, 1 ), 0, 0 },
    { "ASFMediaBuffer_InitializeWrapper", PERF_SCOPE_ID( PERF_MOD_NDT_ASF_STREAM, PERF_FUNC_ASFMediaBuffer_InitializeWrapper, 1 ), 0, 0 },

    /* core\drmembedding.c */
    { "DRM_EMB_UpdateEmbeddedStore", PERF_SCOPE_ID( PERF_MOD_DRMEMB, PERF_FUNC_DRM_EMB_UpdateEmbeddedStore, 1 ), 0, 0 },

    /* DRM XML builder, UTF-8 and UTF-16 */
    /* modules\xmlbuilder\common\drmxmlbuilderalite.c */
    /* modules\wmdrmnet\real\wmdrmnetimplreal.c */
    /* modules\xmlbuilder\common\drmxmlbuilderulite.c */
    /* modules\xmlbuilder\real\drmxmlbuilder.c */
    { "DRM_XMB_CreateDocumentA", PERF_SCOPE_ID( PERF_MOD_DRMXMLBUILDER, PERF_FUNC_DRM_XMB_CreateDocumentA, 1 ), 0, 0 },
    { "DRM_XMB_CloseDocumentA", PERF_SCOPE_ID( PERF_MOD_DRMXMLBUILDER, PERF_FUNC_DRM_XMB_CloseDocumentA, 1 ), 0, 0 },
    { "DRM_XMB_OpenNodeA", PERF_SCOPE_ID( PERF_MOD_DRMXMLBUILDER, PERF_FUNC_DRM_XMB_OpenNodeA, 1 ), 0, 0 },
    { "DRM_XMB_CloseCurrNodeA", PERF_SCOPE_ID( PERF_MOD_DRMXMLBUILDER, PERF_FUNC_DRM_XMB_CloseCurrNodeA, 1 ), 0, 0 },
    { "DRM_XMB_AddAttributeA", PERF_SCOPE_ID( PERF_MOD_DRMXMLBUILDER, PERF_FUNC_DRM_XMB_AddAttributeA, 1 ), 0, 0 },
    { "DRM_XMB_AddDataA", PERF_SCOPE_ID( PERF_MOD_DRMXMLBUILDER, PERF_FUNC_DRM_XMB_AddDataA, 1 ), 0, 0 },
    { "DRM_XMB_WriteTagA", PERF_SCOPE_ID( PERF_MOD_DRMXMLBUILDER, PERF_FUNC_DRM_XMB_WriteTagA, 1 ), 0, 0 },
    { "DRM_XMB_AddCDataA", PERF_SCOPE_ID( PERF_MOD_DRMXMLBUILDER, PERF_FUNC_DRM_XMB_AddCDataA, 1 ), 0, 0 },
    { "DRM_XMB_WriteCDATATagA", PERF_SCOPE_ID( PERF_MOD_DRMXMLBUILDER, PERF_FUNC_DRM_XMB_WriteCDATATagA, 1 ), 0, 0 },
    { "DRM_XMB_ReserveSpaceA", PERF_SCOPE_ID( PERF_MOD_DRMXMLBUILDER, PERF_FUNC_DRM_XMB_ReserveSpaceA, 1 ), 0, 0 },
    { "DRM_XMB_ShiftDataFromCurrentPositionA", PERF_SCOPE_ID( PERF_MOD_DRMXMLBUILDER, PERF_FUNC_DRM_XMB_ShiftDataFromCurrentPositionA, 1 ), 0, 0 },
    { "DRM_XMB_ShiftCurrentPointerA", PERF_SCOPE_ID( PERF_MOD_DRMXMLBUILDER, PERF_FUNC_DRM_XMB_ShiftCurrentPointerA, 1 ), 0, 0 },
    { "DRM_XMB_GetCurrentBufferPointerA", PERF_SCOPE_ID( PERF_MOD_DRMXMLBUILDER, PERF_FUNC_DRM_XMB_GetCurrentBufferPointerA, 1 ), 0, 0 },
    { "DRM_XMB_AppendNodeA", PERF_SCOPE_ID( PERF_MOD_DRMXMLBUILDER, PERF_FUNC_DRM_XMB_AppendNodeA, 1 ), 0, 0 },
    { "DRM_XMB_AESEncryptAndCloseCurrNodeA", PERF_SCOPE_ID( PERF_MOD_DRMXMLBUILDER, PERF_FUNC_DRM_XMB_AESEncryptAndCloseCurrNodeA, 1 ), 0, 0 },
    { "DRM_XMB_RSASignAndCloseCurrNodeA", PERF_SCOPE_ID( PERF_MOD_DRMXMLBUILDER, PERF_FUNC_DRM_XMB_RSASignAndCloseCurrNodeA, 1 ), 0, 0 },
    { "DRM_XMB_HashAndRSASignAndCloseCurrNodeA", PERF_SCOPE_ID( PERF_MOD_DRMXMLBUILDER, PERF_FUNC_DRM_XMB_HashAndRSASignAndCloseCurrNodeA, 1 ), 0, 0 },
    { "DRM_XMB_HashAndCloseCurrNodeA", PERF_SCOPE_ID( PERF_MOD_DRMXMLBUILDER, PERF_FUNC_DRM_XMB_HashAndCloseCurrNodeA, 1 ), 0, 0 },
    { "DRM_XMB_CreateDocument", PERF_SCOPE_ID( PERF_MOD_DRMXMLBUILDER, PERF_FUNC_DRM_XMB_CreateDocument, 1 ), 0, 0 },
    { "DRM_XMB_ReallocDocument", PERF_SCOPE_ID( PERF_MOD_DRMXMLBUILDER, PERF_FUNC_DRM_XMB_ReallocDocument, 1 ), 0, 0 },
    { "DRM_XMB_CloseDocument", PERF_SCOPE_ID( PERF_MOD_DRMXMLBUILDER, PERF_FUNC_DRM_XMB_CloseDocument, 1 ), 0, 0 },
    { "DRM_XMB_OpenNode", PERF_SCOPE_ID( PERF_MOD_DRMXMLBUILDER, PERF_FUNC_DRM_XMB_OpenNode, 1 ), 0, 0 },
    { "DRM_XMB_CloseCurrNode", PERF_SCOPE_ID( PERF_MOD_DRMXMLBUILDER, PERF_FUNC_DRM_XMB_CloseCurrNode, 1 ), 0, 0 },
    { "DRM_XMB_GetCurrNodeName", PERF_SCOPE_ID( PERF_MOD_DRMXMLBUILDER, PERF_FUNC_DRM_XMB_GetCurrNodeName, 1 ), 0, 0 },
    { "DRM_XMB_AddAttribute", PERF_SCOPE_ID( PERF_MOD_DRMXMLBUILDER, PERF_FUNC_DRM_XMB_AddAttribute, 1 ), 0, 0 },
    { "DRM_XMB_AddData", PERF_SCOPE_ID( PERF_MOD_DRMXMLBUILDER, PERF_FUNC_DRM_XMB_AddData, 1 ), 0, 0 },
    { "DRM_XMB_ReserveSpace", PERF_SCOPE_ID( PERF_MOD_DRMXMLBUILDER, PERF_FUNC_DRM_XMB_ReserveSpace, 1 ), 0, 0 },
    { "DRM_XMB_AddCData", PERF_SCOPE_ID( PERF_MOD_DRMXMLBUILDER, PERF_FUNC_DRM_XMB_AddCData, 1 ), 0, 0 },
    { "DRM_XMB_AppendNode", PERF_SCOPE_ID( PERF_MOD_DRMXMLBUILDER, PERF_FUNC_DRM_XMB_AppendNode, 1 ), 0, 0 },
    { "DRM_XMB_AddXMLNode", PERF_SCOPE_ID( PERF_MOD_DRMXMLBUILDER, PERF_FUNC_DRM_XMB_AddXMLNode, 1 ), 0, 0 },
    { "DRM_XMB_WriteTag", PERF_SCOPE_ID( PERF_MOD_DRMXMLBUILDER, PERF_FUNC_DRM_XMB_WriteTag, 1 ), 0, 0 },
    { "DRM_XMB_WriteCDATATag", PERF_SCOPE_ID( PERF_MOD_DRMXMLBUILDER, PERF_FUNC_DRM_XMB_WriteCDATATag, 1 ), 0, 0 },
    { "DRM_XMB_EncryptAndCloseCurrNode", PERF_SCOPE_ID( PERF_MOD_DRMXMLBUILDER, PERF_FUNC_DRM_XMB_EncryptAndCloseCurrNode, 1 ), 0, 0 },
    { "DRM_XMB_SignAndCloseCurrNode", PERF_SCOPE_ID( PERF_MOD_DRMXMLBUILDER, PERF_FUNC_DRM_XMB_SignAndCloseCurrNode, 1 ), 0, 0 },
    { "DRM_XMB_KeyedHashAndCloseCurrNode", PERF_SCOPE_ID( PERF_MOD_DRMXMLBUILDER, PERF_FUNC_DRM_XMB_KeyedHashAndCloseCurrNode, 1 ), 0, 0 },

    /* NULL termination of the list */
    {NULL, 0, 0, 0},

    /* Scope that gets displayed when an unknown value is encountered */
    {"Unknown", 0, 0, 0}

};

const                DRM_WCHAR      g_rgwchTagOEMPrivateKeys                     [] =  { ONE_WCHAR('O', '\0'), ONE_WCHAR('E', '\0'), ONE_WCHAR('M', '\0'), ONE_WCHAR('P', '\0'), ONE_WCHAR('r', '\0'), ONE_WCHAR('i', '\0'), ONE_WCHAR('v', '\0'), ONE_WCHAR('a', '\0'), ONE_WCHAR('t', '\0'), ONE_WCHAR('e', '\0'), ONE_WCHAR('K', '\0'), ONE_WCHAR('e', '\0'), ONE_WCHAR('y', '\0'), ONE_WCHAR('s', '\0'), ONE_WCHAR('\0', '\0')}; /* "OEMPrivateKeys" */
const                DRM_WCHAR      g_rgwchTagKeyValue                           [] =  { ONE_WCHAR('K', '\0'), ONE_WCHAR('e', '\0'), ONE_WCHAR('y', '\0'), ONE_WCHAR('V', '\0'), ONE_WCHAR('a', '\0'), ONE_WCHAR('l', '\0'), ONE_WCHAR('u', '\0'), ONE_WCHAR('e', '\0'), ONE_WCHAR('\0', '\0')}; /* "KeyValue" */
const                DRM_WCHAR      g_rgwchTagRSAKeyValue                        [] =  { ONE_WCHAR('R', '\0'), ONE_WCHAR('S', '\0'), ONE_WCHAR('A', '\0'), ONE_WCHAR('K', '\0'), ONE_WCHAR('e', '\0'), ONE_WCHAR('y', '\0'), ONE_WCHAR('V', '\0'), ONE_WCHAR('a', '\0'), ONE_WCHAR('l', '\0'), ONE_WCHAR('u', '\0'), ONE_WCHAR('e', '\0'), ONE_WCHAR('\0', '\0')}; /* "RSAKeyValue" */
const                DRM_WCHAR      g_rgwchVersion_1_0                           [] =  { ONE_WCHAR('1', '\0'), ONE_WCHAR('.', '\0'), ONE_WCHAR('0', '\0'), ONE_WCHAR('\0', '\0')}; /* "1.0" */
const                DRM_WCHAR      g_rgwchTagFallback                           [] =  { ONE_WCHAR('F', '\0'), ONE_WCHAR('A', '\0'), ONE_WCHAR('L', '\0'), ONE_WCHAR('L', '\0'), ONE_WCHAR('B', '\0'), ONE_WCHAR('A', '\0'), ONE_WCHAR('C', '\0'), ONE_WCHAR('K', '\0'), ONE_WCHAR('\0', '\0')}; /* "FALLBACK" */
const                DRM_WCHAR      g_rgwchTagDNIUniqueID                        [] =  { ONE_WCHAR('U', '\0'), ONE_WCHAR('N', '\0'), ONE_WCHAR('I', '\0'), ONE_WCHAR('Q', '\0'), ONE_WCHAR('U', '\0'), ONE_WCHAR('E', '\0'), ONE_WCHAR('I', '\0'), ONE_WCHAR('D', '\0'), ONE_WCHAR('\0', '\0')}; /* "UNIQUEID" */
const                DRM_WCHAR      g_rgwchDeviceRevocationList                  [] =  { ONE_WCHAR('D', '\0'), ONE_WCHAR('e', '\0'), ONE_WCHAR('v', '\0'), ONE_WCHAR('i', '\0'), ONE_WCHAR('c', '\0'), ONE_WCHAR('e', '\0'), ONE_WCHAR('R', '\0'), ONE_WCHAR('e', '\0'), ONE_WCHAR('v', '\0'), ONE_WCHAR('o', '\0'), ONE_WCHAR('c', '\0'), ONE_WCHAR('a', '\0'), ONE_WCHAR('t', '\0'), ONE_WCHAR('i', '\0'), ONE_WCHAR('o', '\0'), ONE_WCHAR('n', '\0'), ONE_WCHAR('L', '\0'), ONE_WCHAR('i', '\0'), ONE_WCHAR('s', '\0'), ONE_WCHAR('t', '\0'), ONE_WCHAR('\0', '\0')}; /* "DeviceRevocationList" */
const                DRM_WCHAR      g_rgwchPFRootTag                             [] =  { ONE_WCHAR('K', '\0'), ONE_WCHAR('E', '\0'), ONE_WCHAR('Y', '\0'), ONE_WCHAR('D', '\0'), ONE_WCHAR('A', '\0'), ONE_WCHAR('T', '\0'), ONE_WCHAR('A', '\0'), ONE_WCHAR('\0', '\0')}; /* "KEYDATA" */
const                DRM_WCHAR      g_rgwchTagSecurityLevel                      [] =  { ONE_WCHAR('S', '\0'), ONE_WCHAR('E', '\0'), ONE_WCHAR('C', '\0'), ONE_WCHAR('U', '\0'), ONE_WCHAR('R', '\0'), ONE_WCHAR('I', '\0'), ONE_WCHAR('T', '\0'), ONE_WCHAR('Y', '\0'), ONE_WCHAR('L', '\0'), ONE_WCHAR('E', '\0'), ONE_WCHAR('V', '\0'), ONE_WCHAR('E', '\0'), ONE_WCHAR('L', '\0'), ONE_WCHAR('\0', '\0')}; /* "SECURITYLEVEL" */
const                DRM_WCHAR      g_rgwchTagDataId                             [] =  { ONE_WCHAR('A', '\0'), ONE_WCHAR('U', '\0'), ONE_WCHAR('T', '\0'), ONE_WCHAR('H', '\0'), ONE_WCHAR('_', '\0'), ONE_WCHAR('I', '\0'), ONE_WCHAR('D', '\0'), ONE_WCHAR('\0', '\0')}; /* "AUTH_ID" */
const                DRM_WCHAR      g_rgwchManufacturer                          [] =  { ONE_WCHAR('M', '\0'), ONE_WCHAR('A', '\0'), ONE_WCHAR('N', '\0'), ONE_WCHAR('U', '\0'), ONE_WCHAR('F', '\0'), ONE_WCHAR('A', '\0'), ONE_WCHAR('C', '\0'), ONE_WCHAR('T', '\0'), ONE_WCHAR('U', '\0'), ONE_WCHAR('R', '\0'), ONE_WCHAR('E', '\0'), ONE_WCHAR('R', '\0'), ONE_WCHAR('\0', '\0')}; /* "MANUFACTURER" */
const                DRM_WCHAR      g_rgwchModel                                 [] =  { ONE_WCHAR('M', '\0'), ONE_WCHAR('O', '\0'), ONE_WCHAR('D', '\0'), ONE_WCHAR('E', '\0'), ONE_WCHAR('L', '\0'), ONE_WCHAR('\0', '\0')}; /* "MODEL" */
const                DRM_WCHAR      g_rgwchUnsignedtemplate                      [] =  { ONE_WCHAR('U', '\0'), ONE_WCHAR('N', '\0'), ONE_WCHAR('S', '\0'), ONE_WCHAR('I', '\0'), ONE_WCHAR('G', '\0'), ONE_WCHAR('N', '\0'), ONE_WCHAR('E', '\0'), ONE_WCHAR('D', '\0'), ONE_WCHAR('T', '\0'), ONE_WCHAR('E', '\0'), ONE_WCHAR('M', '\0'), ONE_WCHAR('P', '\0'), ONE_WCHAR('L', '\0'), ONE_WCHAR('A', '\0'), ONE_WCHAR('T', '\0'), ONE_WCHAR('E', '\0'), ONE_WCHAR('\0', '\0')}; /* "UNSIGNEDTEMPLATE" */
const                DRM_WCHAR      g_rgwchTagDataSecVerNumber                   [] =  { ONE_WCHAR('N', '\0'), ONE_WCHAR('U', '\0'), ONE_WCHAR('M', '\0'), ONE_WCHAR('B', '\0'), ONE_WCHAR('E', '\0'), ONE_WCHAR('R', '\0'), ONE_WCHAR('\0', '\0')}; /* "NUMBER" */
const                DRM_WCHAR      g_rgwchLicenseTag                            [] =  { ONE_WCHAR('L', '\0'), ONE_WCHAR('I', '\0'), ONE_WCHAR('C', '\0'), ONE_WCHAR('E', '\0'), ONE_WCHAR('N', '\0'), ONE_WCHAR('S', '\0'), ONE_WCHAR('E', '\0'), ONE_WCHAR('\0', '\0')}; /* "LICENSE" */
const                DRM_WCHAR      g_rgwchTagName                               [] =  { ONE_WCHAR('N', '\0'), ONE_WCHAR('A', '\0'), ONE_WCHAR('M', '\0'), ONE_WCHAR('E', '\0'), ONE_WCHAR('\0', '\0')}; /* "NAME" */

const DRM_DISCARDABLE DRM_CONST_STRING      g_dstrTagOEMPrivateKeys                     = CREATE_DRM_STRING( g_rgwchTagOEMPrivateKeys );
const DRM_DISCARDABLE DRM_CONST_STRING      g_dstrTagKeyValue                           = CREATE_DRM_STRING( g_rgwchTagKeyValue );
const DRM_DISCARDABLE DRM_CONST_STRING      g_dstrTagRSAKeyValue                        = CREATE_DRM_STRING( g_rgwchTagRSAKeyValue );
const DRM_DISCARDABLE DRM_CONST_STRING      g_dstrVersion_1_0                           = CREATE_DRM_STRING( g_rgwchVersion_1_0 );
const DRM_DISCARDABLE DRM_CONST_STRING      g_dstrTagFallback                           = CREATE_DRM_STRING( g_rgwchTagFallback );
const DRM_DISCARDABLE DRM_CONST_STRING      g_dstrTagDNIUniqueID                        = CREATE_DRM_STRING( g_rgwchTagDNIUniqueID );
const DRM_DISCARDABLE DRM_CONST_STRING      g_dstrDeviceRevocationList                  = CREATE_DRM_STRING( g_rgwchDeviceRevocationList );
const DRM_DISCARDABLE DRM_CONST_STRING      g_dstrPFRootTag                             = CREATE_DRM_STRING( g_rgwchPFRootTag );
const DRM_DISCARDABLE DRM_CONST_STRING      g_dstrTagSecurityLevel                      = CREATE_DRM_STRING( g_rgwchTagSecurityLevel );
const DRM_DISCARDABLE DRM_CONST_STRING      g_dstrTagDataId                             = CREATE_DRM_STRING( g_rgwchTagDataId );
const DRM_DISCARDABLE DRM_CONST_STRING      g_dstrManufacturer                          = CREATE_DRM_STRING( g_rgwchManufacturer );
const DRM_DISCARDABLE DRM_CONST_STRING      g_dstrModel                                 = CREATE_DRM_STRING( g_rgwchModel );
const DRM_DISCARDABLE DRM_CONST_STRING      g_dstrUnsignedtemplate                      = CREATE_DRM_STRING( g_rgwchUnsignedtemplate );
const DRM_DISCARDABLE DRM_CONST_STRING      g_dstrTagDataSecVerNumber                   = CREATE_DRM_STRING( g_rgwchTagDataSecVerNumber );
const DRM_DISCARDABLE DRM_CONST_STRING      g_dstrTagLicense                            = CREATE_DRM_STRING( g_rgwchLicenseTag );
const DRM_DISCARDABLE DRM_CONST_STRING      g_dstrTagName                               = CREATE_DRM_STRING( g_rgwchTagName );

EXIT_PK_NAMESPACE_CODE;


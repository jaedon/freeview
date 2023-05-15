/**@@@+++@@@@******************************************************************
**
** Microsoft (r) PlayReady (r)
** Copyright (c) Microsoft Corporation. All rights reserved.
**
***@@@---@@@@******************************************************************
*/
#include "stdafx.h"

#include <PKTestNameSpace.h>

USING_PK_NAMESPACE
USING_PKTEST_NAMESPACE

#include <drmcommon.h>
#include <tstutils.h>
#include <trightsparser.h>
#include <drmchkerr.h>
#include <drmxmlparser.h>
#include <logger.h>
#include <tstutils.h>

#ifdef WINCE_TEST
//#include <winbase.h>
#endif 

/*MID*/
static const DRM_WCHAR dwchMID[]                                        = { WCHAR_CAST('M'), WCHAR_CAST('I'), WCHAR_CAST('D'), WCHAR_CAST('\0')};
/*METERCERT*/
static const DRM_WCHAR dwchMETERCERT[]                                  = { WCHAR_CAST('M'), WCHAR_CAST('E'), WCHAR_CAST('T'), WCHAR_CAST('E'), WCHAR_CAST('R'), WCHAR_CAST('C'), WCHAR_CAST('E'), WCHAR_CAST('R'), WCHAR_CAST('T'), WCHAR_CAST('\0') };
/*DATA*/
static const DRM_WCHAR dwchData[]                                       = { WCHAR_CAST('D'), WCHAR_CAST('A'), WCHAR_CAST('T'), WCHAR_CAST('A'), WCHAR_CAST('\0') };
/*RightsPool*/
static const DRM_WCHAR dwchTagRightsPool[]                              = { WCHAR_CAST('R'), WCHAR_CAST('i'), WCHAR_CAST('g'), WCHAR_CAST('h'), WCHAR_CAST('t'), WCHAR_CAST('s'), WCHAR_CAST('P'), WCHAR_CAST('o'), WCHAR_CAST('o'), WCHAR_CAST('l'), WCHAR_CAST('\0') };
/*LicenseGroup*/
static const DRM_WCHAR dwchTagLicenseGroup[]                            = { WCHAR_CAST('L'), WCHAR_CAST('i'), WCHAR_CAST('c'), WCHAR_CAST('e'), WCHAR_CAST('n'), WCHAR_CAST('s'), WCHAR_CAST('e'), WCHAR_CAST('G'), WCHAR_CAST('r'), WCHAR_CAST('o'), WCHAR_CAST('u'), WCHAR_CAST('p'), WCHAR_CAST('\0') };
/*name*/
static const DRM_WCHAR dwchAttribname[]                                 = { WCHAR_CAST('n'), WCHAR_CAST('a'), WCHAR_CAST('m'), WCHAR_CAST('e'), WCHAR_CAST('\0') };
/*LicenseRights*/
static const DRM_WCHAR dwchTagLicenseRights[]                           = { WCHAR_CAST('L'), WCHAR_CAST('i'), WCHAR_CAST('c'), WCHAR_CAST('e'), WCHAR_CAST('n'), WCHAR_CAST('s'), WCHAR_CAST('e'), WCHAR_CAST('R'), WCHAR_CAST('i'), WCHAR_CAST('g'), WCHAR_CAST('h'), WCHAR_CAST('t'), WCHAR_CAST('s'), WCHAR_CAST('\0') };
/*Leaf*/
static const DRM_WCHAR dwchTagLeaf[]                                    = { WCHAR_CAST('L'), WCHAR_CAST('e'), WCHAR_CAST('a'), WCHAR_CAST('f'), WCHAR_CAST('\0') };
/*METERCERT*/
static const DRM_WCHAR dwchTagMETERCERT[]                               = { WCHAR_CAST('M'), WCHAR_CAST('E'), WCHAR_CAST('T'), WCHAR_CAST('E'), WCHAR_CAST('R'), WCHAR_CAST('C'), WCHAR_CAST('E'), WCHAR_CAST('R'), WCHAR_CAST('T'), WCHAR_CAST('\0') };
/*META*/
static const DRM_WCHAR dwchTagMETA[]                                    = { WCHAR_CAST('M'), WCHAR_CAST('E'), WCHAR_CAST('T'), WCHAR_CAST('A'), WCHAR_CAST('\0') };
/*TestCaseID*/
static const DRM_WCHAR dwchAttribTestCaseID[]                           = { WCHAR_CAST('T'), WCHAR_CAST('e'), WCHAR_CAST('s'), WCHAR_CAST('t'), WCHAR_CAST('C'), WCHAR_CAST('a'), WCHAR_CAST('s'), WCHAR_CAST('e'), WCHAR_CAST('I'), WCHAR_CAST('D'), WCHAR_CAST('\0') };
/*Type*/
static const DRM_WCHAR dwchAttribType[]                                 = { WCHAR_CAST('T'), WCHAR_CAST('y'), WCHAR_CAST('p'), WCHAR_CAST('e'), WCHAR_CAST('\0') };
/*AllowPlay*/
static const DRM_WCHAR dwchAttribAllowPlay[]                            = { WCHAR_CAST('A'), WCHAR_CAST('l'), WCHAR_CAST('l'), WCHAR_CAST('o'), WCHAR_CAST('w'), WCHAR_CAST('P'), WCHAR_CAST('l'), WCHAR_CAST('a'), WCHAR_CAST('y'), WCHAR_CAST('\0') };
/*PlayCount*/
static const DRM_WCHAR dwchAttribPlayCount[]                            = { WCHAR_CAST('P'), WCHAR_CAST('l'), WCHAR_CAST('a'), WCHAR_CAST('y'), WCHAR_CAST('C'), WCHAR_CAST('o'), WCHAR_CAST('u'), WCHAR_CAST('n'), WCHAR_CAST('t'), WCHAR_CAST('\0') };
/*AllowCopy*/
static const DRM_WCHAR dwchAttribAllowCopy[]                            = { WCHAR_CAST('A'), WCHAR_CAST('l'), WCHAR_CAST('l'), WCHAR_CAST('o'), WCHAR_CAST('w'), WCHAR_CAST('C'), WCHAR_CAST('o'), WCHAR_CAST('p'), WCHAR_CAST('y'), WCHAR_CAST('\0') };
/*CopyCount*/
static const DRM_WCHAR dwchAttribCopyCount[]                            = { WCHAR_CAST('C'), WCHAR_CAST('o'), WCHAR_CAST('p'), WCHAR_CAST('y'), WCHAR_CAST('C'), WCHAR_CAST('o'), WCHAR_CAST('u'), WCHAR_CAST('n'), WCHAR_CAST('t'), WCHAR_CAST('\0') };
/*AllowTransferToSDMI*/
static const DRM_WCHAR dwchAttribAllowTransferToSDMI[]                  = { WCHAR_CAST('A'), WCHAR_CAST('l'), WCHAR_CAST('l'), WCHAR_CAST('o'), WCHAR_CAST('w'), WCHAR_CAST('T'), WCHAR_CAST('r'), WCHAR_CAST('a'), WCHAR_CAST('n'), WCHAR_CAST('s'), WCHAR_CAST('f'), WCHAR_CAST('e'), WCHAR_CAST('r'), WCHAR_CAST('T'), WCHAR_CAST('o'), WCHAR_CAST('S'), WCHAR_CAST('D'), WCHAR_CAST('M'), WCHAR_CAST('I'), WCHAR_CAST('\0') };
/*AllowTransferToNonSDMI*/
static const DRM_WCHAR dwchAttribAllowTransferToNonSDMI[]               = { WCHAR_CAST('A'), WCHAR_CAST('l'), WCHAR_CAST('l'), WCHAR_CAST('o'), WCHAR_CAST('w'), WCHAR_CAST('T'), WCHAR_CAST('r'), WCHAR_CAST('a'), WCHAR_CAST('n'), WCHAR_CAST('s'), WCHAR_CAST('f'), WCHAR_CAST('e'), WCHAR_CAST('r'), WCHAR_CAST('T'), WCHAR_CAST('o'), WCHAR_CAST('N'), WCHAR_CAST('o'), WCHAR_CAST('n'), WCHAR_CAST('S'), WCHAR_CAST('D'), WCHAR_CAST('M'), WCHAR_CAST('I'), WCHAR_CAST('\0') };
/*TransferCount*/
static const DRM_WCHAR dwchAttribTransferCount[]                        = { WCHAR_CAST('T'), WCHAR_CAST('r'), WCHAR_CAST('a'), WCHAR_CAST('n'), WCHAR_CAST('s'), WCHAR_CAST('f'), WCHAR_CAST('e'), WCHAR_CAST('r'), WCHAR_CAST('C'), WCHAR_CAST('o'), WCHAR_CAST('u'), WCHAR_CAST('n'), WCHAR_CAST('t'), WCHAR_CAST('\0') };
/*CopyProtectionLevel*/
static const DRM_WCHAR dwchAttribCopyProtectionLevel[]                  = { WCHAR_CAST('C'), WCHAR_CAST('o'), WCHAR_CAST('p'), WCHAR_CAST('y'), WCHAR_CAST('P'), WCHAR_CAST('r'), WCHAR_CAST('o'), WCHAR_CAST('t'), WCHAR_CAST('e'), WCHAR_CAST('c'), WCHAR_CAST('t'), WCHAR_CAST('i'), WCHAR_CAST('o'), WCHAR_CAST('n'), WCHAR_CAST('L'), WCHAR_CAST('e'), WCHAR_CAST('v'), WCHAR_CAST('e'), WCHAR_CAST('l'), WCHAR_CAST('\0') };
/*AnalogVideo*/
static const DRM_WCHAR dwchAttribAnalogVideo[]                          = { WCHAR_CAST('A'), WCHAR_CAST('n'), WCHAR_CAST('a'), WCHAR_CAST('l'), WCHAR_CAST('o'), WCHAR_CAST('g'), WCHAR_CAST('V'), WCHAR_CAST('i'), WCHAR_CAST('d'), WCHAR_CAST('e'), WCHAR_CAST('o'), WCHAR_CAST('\0') };
/*UncompressedDigitalVideo*/
static const DRM_WCHAR dwchAttribUnCompressedDigitalVideo[]             = { WCHAR_CAST('U'), WCHAR_CAST('n'), WCHAR_CAST('C'), WCHAR_CAST('o'), WCHAR_CAST('m'), WCHAR_CAST('p'), WCHAR_CAST('r'), WCHAR_CAST('e'), WCHAR_CAST('s'), WCHAR_CAST('s'), WCHAR_CAST('e'), WCHAR_CAST('d'), WCHAR_CAST('D'), WCHAR_CAST('i'), WCHAR_CAST('g'), WCHAR_CAST('i'), WCHAR_CAST('t'), WCHAR_CAST('a'), WCHAR_CAST('l'), WCHAR_CAST('V'), WCHAR_CAST('i'), WCHAR_CAST('d'), WCHAR_CAST('e'), WCHAR_CAST('o'),WCHAR_CAST('\0') };
/*UncompressedDigitalAudio*/
static const DRM_WCHAR dwchAttribUnCompressedDigitalAudio[]             = { WCHAR_CAST('U'), WCHAR_CAST('n'), WCHAR_CAST('C'), WCHAR_CAST('o'), WCHAR_CAST('m'), WCHAR_CAST('p'), WCHAR_CAST('r'), WCHAR_CAST('e'), WCHAR_CAST('s'), WCHAR_CAST('s'), WCHAR_CAST('e'), WCHAR_CAST('d'), WCHAR_CAST('D'), WCHAR_CAST('i'), WCHAR_CAST('g'), WCHAR_CAST('i'), WCHAR_CAST('t'), WCHAR_CAST('a'), WCHAR_CAST('l'), WCHAR_CAST('A'), WCHAR_CAST('u'), WCHAR_CAST('d'), WCHAR_CAST('i'), WCHAR_CAST('o'),WCHAR_CAST('\0') };
/*CompressedDigitalVideo*/
static const DRM_WCHAR dwchAttribCompressedDigitalVideo[]               = { WCHAR_CAST('C'), WCHAR_CAST('o'), WCHAR_CAST('m'), WCHAR_CAST('p'), WCHAR_CAST('r'), WCHAR_CAST('e'), WCHAR_CAST('s'), WCHAR_CAST('s'), WCHAR_CAST('e'), WCHAR_CAST('d'), WCHAR_CAST('D'), WCHAR_CAST('i'), WCHAR_CAST('g'), WCHAR_CAST('i'), WCHAR_CAST('t'), WCHAR_CAST('a'), WCHAR_CAST('l'), WCHAR_CAST('V'), WCHAR_CAST('i'), WCHAR_CAST('d'), WCHAR_CAST('e'), WCHAR_CAST('o'), WCHAR_CAST('\0') };
/*CompressedDigitalAudio*/
static const DRM_WCHAR dwchAttribCompressedDigitalAudio[]               = { WCHAR_CAST('C'), WCHAR_CAST('o'), WCHAR_CAST('m'), WCHAR_CAST('p'), WCHAR_CAST('r'), WCHAR_CAST('e'), WCHAR_CAST('s'), WCHAR_CAST('s'), WCHAR_CAST('e'), WCHAR_CAST('d'), WCHAR_CAST('D'), WCHAR_CAST('i'), WCHAR_CAST('g'), WCHAR_CAST('i'), WCHAR_CAST('t'), WCHAR_CAST('a'), WCHAR_CAST('l'), WCHAR_CAST('A'), WCHAR_CAST('u'), WCHAR_CAST('d'), WCHAR_CAST('i'), WCHAR_CAST('o'), WCHAR_CAST('\0') };
/*AllowBackUpRestore*/
static const DRM_WCHAR dwchAttribAllowBackUpRestore[]                   = { WCHAR_CAST('A'), WCHAR_CAST('l'), WCHAR_CAST('l'), WCHAR_CAST('o'), WCHAR_CAST('w'), WCHAR_CAST('B'), WCHAR_CAST('a'), WCHAR_CAST('c'), WCHAR_CAST('k'), WCHAR_CAST('U'), WCHAR_CAST('p'), WCHAR_CAST('R'), WCHAR_CAST('e'), WCHAR_CAST('s'), WCHAR_CAST('t'), WCHAR_CAST('o'), WCHAR_CAST('r'), WCHAR_CAST('e'), WCHAR_CAST('\0') };
/*AllowBurnToCD*/
static const DRM_WCHAR dwchAttribAllowBurnToCD[]                        = { WCHAR_CAST('A'), WCHAR_CAST('l'), WCHAR_CAST('l'), WCHAR_CAST('o'), WCHAR_CAST('w'), WCHAR_CAST('B'), WCHAR_CAST('u'), WCHAR_CAST('r'), WCHAR_CAST('n'), WCHAR_CAST('T'), WCHAR_CAST('o'), WCHAR_CAST('C'), WCHAR_CAST('D'), WCHAR_CAST('\0') };
/*BurnToCDCount*/
static const DRM_WCHAR dwchAttribBurnToCDCount[]                        = { WCHAR_CAST('B'), WCHAR_CAST('u'), WCHAR_CAST('r'), WCHAR_CAST('n'), WCHAR_CAST('T'), WCHAR_CAST('o'), WCHAR_CAST('C'), WCHAR_CAST('D'), WCHAR_CAST('C'), WCHAR_CAST('o'), WCHAR_CAST('u'), WCHAR_CAST('n'), WCHAR_CAST('t'), WCHAR_CAST('\0') };
/*AllowPlaylistBurn*/
static const DRM_WCHAR dwchAttribAllowPlaylistBurn[]                    = { WCHAR_CAST('A'), WCHAR_CAST('l'), WCHAR_CAST('l'), WCHAR_CAST('o'), WCHAR_CAST('w'), WCHAR_CAST('P'), WCHAR_CAST('l'), WCHAR_CAST('a'), WCHAR_CAST('y'), WCHAR_CAST('l'), WCHAR_CAST('i'), WCHAR_CAST('s'), WCHAR_CAST('t'), WCHAR_CAST('B'), WCHAR_CAST('u'), WCHAR_CAST('r'), WCHAR_CAST('n'), WCHAR_CAST('\0') };
/*PlaylistBurnTrackCount*/
static const DRM_WCHAR dwchAttribPlaylistBurnTrackCount[]               = { WCHAR_CAST('P'), WCHAR_CAST('l'), WCHAR_CAST('a'), WCHAR_CAST('y'), WCHAR_CAST('l'), WCHAR_CAST('i'), WCHAR_CAST('s'), WCHAR_CAST('t'), WCHAR_CAST('B'), WCHAR_CAST('u'), WCHAR_CAST('r'), WCHAR_CAST('n'), WCHAR_CAST('T'), WCHAR_CAST('r'), WCHAR_CAST('a'), WCHAR_CAST('c'), WCHAR_CAST('k'), WCHAR_CAST('C'), WCHAR_CAST('o'), WCHAR_CAST('u'), WCHAR_CAST('n'), WCHAR_CAST('t'), WCHAR_CAST('\0') };
/*MaxPlaylistBurnCount*/
static const DRM_WCHAR dwchAttribMaxPlaylistBurnCount[]                 = { WCHAR_CAST('M'), WCHAR_CAST('a'), WCHAR_CAST('x'), WCHAR_CAST('P'), WCHAR_CAST('l'), WCHAR_CAST('a'), WCHAR_CAST('y'), WCHAR_CAST('l'), WCHAR_CAST('i'), WCHAR_CAST('s'), WCHAR_CAST('t'), WCHAR_CAST('B'), WCHAR_CAST('u'), WCHAR_CAST('r'), WCHAR_CAST('n'), WCHAR_CAST('C'), WCHAR_CAST('o'), WCHAR_CAST('u'), WCHAR_CAST('n'), WCHAR_CAST('t'), WCHAR_CAST('\0') };
/*AllowCollaborativePlay*/
static const DRM_WCHAR dwchAttribAllowCollaborativePlay[]               = { WCHAR_CAST('A'), WCHAR_CAST('l'), WCHAR_CAST('l'), WCHAR_CAST('o'), WCHAR_CAST('w'), WCHAR_CAST('C'), WCHAR_CAST('o'), WCHAR_CAST('l'), WCHAR_CAST('l'), WCHAR_CAST('a'), WCHAR_CAST('b'), WCHAR_CAST('o'), WCHAR_CAST('r'), WCHAR_CAST('a'), WCHAR_CAST('t'), WCHAR_CAST('i'), WCHAR_CAST('v'), WCHAR_CAST('e'), WCHAR_CAST('P'), WCHAR_CAST('l'), WCHAR_CAST('a'), WCHAR_CAST('y'), WCHAR_CAST('\0') };
/*BeginDate*/
static const DRM_WCHAR dwchAttribBeginDate[]                            = { WCHAR_CAST('B'), WCHAR_CAST('e'), WCHAR_CAST('g'), WCHAR_CAST('i'), WCHAR_CAST('n'), WCHAR_CAST('D'), WCHAR_CAST('a'), WCHAR_CAST('t'), WCHAR_CAST('e'), WCHAR_CAST('\0') };
/*ExpirationDate*/
static const DRM_WCHAR dwchAttribExpirationDate[]                       = { WCHAR_CAST('E'), WCHAR_CAST('x'), WCHAR_CAST('p'), WCHAR_CAST('i'), WCHAR_CAST('r'), WCHAR_CAST('a'), WCHAR_CAST('t'), WCHAR_CAST('i'), WCHAR_CAST('o'), WCHAR_CAST('n'), WCHAR_CAST('D'), WCHAR_CAST('a'), WCHAR_CAST('t'), WCHAR_CAST('e'), WCHAR_CAST('\0') };
/*ExpirationAfterFirstUse*/
static const DRM_WCHAR dwchAttribExpirationAfterFirstUse[]              = { WCHAR_CAST('E'), WCHAR_CAST('x'), WCHAR_CAST('p'), WCHAR_CAST('i'), WCHAR_CAST('r'), WCHAR_CAST('a'), WCHAR_CAST('t'), WCHAR_CAST('i'), WCHAR_CAST('o'), WCHAR_CAST('n'), WCHAR_CAST('A'), WCHAR_CAST('f'), WCHAR_CAST('t'), WCHAR_CAST('e'), WCHAR_CAST('r'), WCHAR_CAST('F'), WCHAR_CAST('i'), WCHAR_CAST('r'), WCHAR_CAST('s'), WCHAR_CAST('t'), WCHAR_CAST('U'), WCHAR_CAST('s'), WCHAR_CAST('e'), WCHAR_CAST('\0') };
/*ExpirationAfterFirstPlay*/
static const DRM_WCHAR dwchAttribExpirationAfterFirstPlay[]             = { WCHAR_CAST('E'), WCHAR_CAST('x'), WCHAR_CAST('p'), WCHAR_CAST('i'), WCHAR_CAST('r'), WCHAR_CAST('a'), WCHAR_CAST('t'), WCHAR_CAST('i'), WCHAR_CAST('o'), WCHAR_CAST('n'), WCHAR_CAST('A'), WCHAR_CAST('f'), WCHAR_CAST('t'), WCHAR_CAST('e'), WCHAR_CAST('r'), WCHAR_CAST('F'), WCHAR_CAST('i'), WCHAR_CAST('r'), WCHAR_CAST('s'), WCHAR_CAST('t'), WCHAR_CAST('P'), WCHAR_CAST('l'), WCHAR_CAST('a'), WCHAR_CAST('y'), WCHAR_CAST('\0') };
/*ExpirationOnStore*/
static const DRM_WCHAR dwchAttribExpirationOnStore[]                    = { WCHAR_CAST('E'), WCHAR_CAST('x'), WCHAR_CAST('p'), WCHAR_CAST('i'), WCHAR_CAST('r'), WCHAR_CAST('a'), WCHAR_CAST('t'), WCHAR_CAST('i'), WCHAR_CAST('o'), WCHAR_CAST('n'), WCHAR_CAST('O'), WCHAR_CAST('n'), WCHAR_CAST('S'), WCHAR_CAST('t'), WCHAR_CAST('o'), WCHAR_CAST('r'), WCHAR_CAST('e'), WCHAR_CAST('\0') };
/*GracePeriod*/
static const DRM_WCHAR dwchAttribGracePeriod[]                          = { WCHAR_CAST('G'), WCHAR_CAST('r'), WCHAR_CAST('a'), WCHAR_CAST('c'), WCHAR_CAST('e'), WCHAR_CAST('P'), WCHAR_CAST('e'), WCHAR_CAST('r'), WCHAR_CAST('i'), WCHAR_CAST('o'), WCHAR_CAST('d'), WCHAR_CAST('\0') };
/*DeleteOnClockRollback*/
static const DRM_WCHAR dwchAttribDeleteOnClockRollback[]                = { WCHAR_CAST('D'), WCHAR_CAST('e'), WCHAR_CAST('l'), WCHAR_CAST('e'), WCHAR_CAST('t'), WCHAR_CAST('e'), WCHAR_CAST('O'), WCHAR_CAST('n'), WCHAR_CAST('C'), WCHAR_CAST('l'), WCHAR_CAST('o'), WCHAR_CAST('c'), WCHAR_CAST('k'), WCHAR_CAST('R'), WCHAR_CAST('o'), WCHAR_CAST('l'), WCHAR_CAST('l'), WCHAR_CAST('b'), WCHAR_CAST('a'), WCHAR_CAST('c'), WCHAR_CAST('k'), WCHAR_CAST('\0') };
/*DisableOnClockRollback*/
static const DRM_WCHAR dwchAttribDisableOnClockRollback[]               = { WCHAR_CAST('D'), WCHAR_CAST('i'), WCHAR_CAST('s'), WCHAR_CAST('a'), WCHAR_CAST('b'), WCHAR_CAST('l'), WCHAR_CAST('e'), WCHAR_CAST('O'), WCHAR_CAST('n'), WCHAR_CAST('C'), WCHAR_CAST('l'), WCHAR_CAST('o'), WCHAR_CAST('c'), WCHAR_CAST('k'), WCHAR_CAST('R'), WCHAR_CAST('o'), WCHAR_CAST('l'), WCHAR_CAST('l'), WCHAR_CAST('b'), WCHAR_CAST('a'), WCHAR_CAST('c'), WCHAR_CAST('k'), WCHAR_CAST('\0') };
/*MinimumSecurityLevel*/
static const DRM_WCHAR dwchAttribMinimumSecurityLevel[]                 = { WCHAR_CAST('M'), WCHAR_CAST('i'), WCHAR_CAST('n'), WCHAR_CAST('i'), WCHAR_CAST('m'), WCHAR_CAST('u'), WCHAR_CAST('m'), WCHAR_CAST('S'), WCHAR_CAST('e'), WCHAR_CAST('c'), WCHAR_CAST('u'), WCHAR_CAST('r'), WCHAR_CAST('i'), WCHAR_CAST('t'), WCHAR_CAST('y'), WCHAR_CAST('L'), WCHAR_CAST('e'), WCHAR_CAST('v'), WCHAR_CAST('e'), WCHAR_CAST('l'), WCHAR_CAST('\0') };
/*MinimumAppCrlVersion*/
static const DRM_WCHAR dwchAttribMinimumAppCrlVersion[]                 = { WCHAR_CAST('M'), WCHAR_CAST('i'), WCHAR_CAST('n'), WCHAR_CAST('i'), WCHAR_CAST('m'), WCHAR_CAST('u'), WCHAR_CAST('m'), WCHAR_CAST('A'), WCHAR_CAST('p'), WCHAR_CAST('p'), WCHAR_CAST('C'), WCHAR_CAST('r'), WCHAR_CAST('l'), WCHAR_CAST('V'), WCHAR_CAST('e'), WCHAR_CAST('r'), WCHAR_CAST('s'), WCHAR_CAST('i'), WCHAR_CAST('o'), WCHAR_CAST('n'), WCHAR_CAST('\0') };
/*MinimumDeviceCrlVersion*/
static const DRM_WCHAR dwchAttribMinimumDeviceCrlVersion[]              = { WCHAR_CAST('M'), WCHAR_CAST('i'), WCHAR_CAST('n'), WCHAR_CAST('i'), WCHAR_CAST('m'), WCHAR_CAST('u'), WCHAR_CAST('m'), WCHAR_CAST('D'), WCHAR_CAST('e'), WCHAR_CAST('v'), WCHAR_CAST('i'), WCHAR_CAST('c'), WCHAR_CAST('e'), WCHAR_CAST('C'), WCHAR_CAST('r'), WCHAR_CAST('l'), WCHAR_CAST('V'), WCHAR_CAST('e'), WCHAR_CAST('r'), WCHAR_CAST('s'), WCHAR_CAST('i'), WCHAR_CAST('o'), WCHAR_CAST('n'), WCHAR_CAST('\0') };
/*MinimumClientSDKVersion*/
static const DRM_WCHAR dwchAttribMinimumClientSDKSecurity[]             = { WCHAR_CAST('M'), WCHAR_CAST('i'), WCHAR_CAST('n'), WCHAR_CAST('i'), WCHAR_CAST('m'), WCHAR_CAST('u'), WCHAR_CAST('m'), WCHAR_CAST('C'), WCHAR_CAST('l'), WCHAR_CAST('i'), WCHAR_CAST('e'), WCHAR_CAST('n'), WCHAR_CAST('t'), WCHAR_CAST('S'), WCHAR_CAST('D'), WCHAR_CAST('K'), WCHAR_CAST('S'), WCHAR_CAST('e'), WCHAR_CAST('c'), WCHAR_CAST('u'), WCHAR_CAST('r'), WCHAR_CAST('i'), WCHAR_CAST('t'), WCHAR_CAST('y'), WCHAR_CAST('\0') };
/*PMRights*/
static const DRM_WCHAR dwchAttribPMRights[]                             = { WCHAR_CAST('P'), WCHAR_CAST('M'), WCHAR_CAST('R'), WCHAR_CAST('i'), WCHAR_CAST('g'), WCHAR_CAST('h'), WCHAR_CAST('t'), WCHAR_CAST('s'), WCHAR_CAST('\0') };
/*PMAppSecurity*/
static const DRM_WCHAR dwchAttribPMAppSecurity[]                        = { WCHAR_CAST('P'), WCHAR_CAST('M'), WCHAR_CAST('A'), WCHAR_CAST('p'), WCHAR_CAST('p'), WCHAR_CAST('S'), WCHAR_CAST('e'), WCHAR_CAST('c'), WCHAR_CAST('u'), WCHAR_CAST('r'), WCHAR_CAST('i'), WCHAR_CAST('t'), WCHAR_CAST('y'), WCHAR_CAST('\0') };
/*PMExpirationDate*/
static const DRM_WCHAR dwchAttribPMExpirationDate[]                     = { WCHAR_CAST('P'), WCHAR_CAST('M'), WCHAR_CAST('E'), WCHAR_CAST('x'), WCHAR_CAST('p'), WCHAR_CAST('i'), WCHAR_CAST('r'), WCHAR_CAST('a'), WCHAR_CAST('t'), WCHAR_CAST('i'), WCHAR_CAST('o'), WCHAR_CAST('n'), WCHAR_CAST('D'), WCHAR_CAST('a'), WCHAR_CAST('t'), WCHAR_CAST('e'), WCHAR_CAST('\0') };
/*AllowSaveStreamProtected*/
static const DRM_WCHAR dwchAttribAllowSaveStreamProtected[]             = { WCHAR_CAST('A'), WCHAR_CAST('l'), WCHAR_CAST('l'), WCHAR_CAST('o'), WCHAR_CAST('w'), WCHAR_CAST('S'), WCHAR_CAST('a'), WCHAR_CAST('v'), WCHAR_CAST('e'), WCHAR_CAST('S'), WCHAR_CAST('t'), WCHAR_CAST('r'), WCHAR_CAST('e'), WCHAR_CAST('a'), WCHAR_CAST('m'), WCHAR_CAST('P'), WCHAR_CAST('r'), WCHAR_CAST('o'), WCHAR_CAST('t'), WCHAR_CAST('e'), WCHAR_CAST('c'), WCHAR_CAST('t'), WCHAR_CAST('e') ,WCHAR_CAST('d'),WCHAR_CAST('\0') };
/*MeteringID*/
static const DRM_WCHAR dwchAttribMeteringID[]                           = { WCHAR_CAST('M'), WCHAR_CAST('e'), WCHAR_CAST('t'), WCHAR_CAST('e'), WCHAR_CAST('r'), WCHAR_CAST('i'), WCHAR_CAST('n'), WCHAR_CAST('g'), WCHAR_CAST('I'), WCHAR_CAST('D'), WCHAR_CAST('\0') };
/*KID*/
static const DRM_WCHAR dwchAttribKID[]                                  = { WCHAR_CAST('K'), WCHAR_CAST('I'), WCHAR_CAST('D'), WCHAR_CAST('\0') };
/*Priority*/
static const DRM_WCHAR dwchAttribPriority[]                             = { WCHAR_CAST('P'), WCHAR_CAST('r'), WCHAR_CAST('i'), WCHAR_CAST('o'), WCHAR_CAST('r'), WCHAR_CAST('i'), WCHAR_CAST('t'), WCHAR_CAST('y'), WCHAR_CAST('\0') };
/*RevInfoVersion*/
static const DRM_WCHAR dwchAttribRevInfoVersion[]                       = { WCHAR_CAST('R'), WCHAR_CAST('e'), WCHAR_CAST('v'), WCHAR_CAST('I'), WCHAR_CAST('n'), WCHAR_CAST('f'), WCHAR_CAST('o'), WCHAR_CAST('V'), WCHAR_CAST('e'), WCHAR_CAST('r'), WCHAR_CAST('s'), WCHAR_CAST('i'), WCHAR_CAST('o'), WCHAR_CAST('n'), WCHAR_CAST('\0') };
/*IndivVersion*/
static const DRM_WCHAR dwchAttribIndivVersion[]                         = { WCHAR_CAST('I'), WCHAR_CAST('n'), WCHAR_CAST('d'), WCHAR_CAST('i'), WCHAR_CAST('v'), WCHAR_CAST('V'), WCHAR_CAST('e'), WCHAR_CAST('r'), WCHAR_CAST('s'), WCHAR_CAST('i'), WCHAR_CAST('o'), WCHAR_CAST('n'), WCHAR_CAST('\0') };
/*LID*/
static const DRM_WCHAR dwchAttribLID[]                                  = { WCHAR_CAST('L'), WCHAR_CAST('I'), WCHAR_CAST('D'), WCHAR_CAST('\0') };
/*RevocationList*/
static const DRM_WCHAR dwchAttribRevocationList[]                       = { WCHAR_CAST('R'), WCHAR_CAST('e'), WCHAR_CAST('v'), WCHAR_CAST('o'), WCHAR_CAST('c'), WCHAR_CAST('a'), WCHAR_CAST('t'), WCHAR_CAST('i'), WCHAR_CAST('o'), WCHAR_CAST('n'), WCHAR_CAST('L'), WCHAR_CAST('i'), WCHAR_CAST('s'), WCHAR_CAST('t'), WCHAR_CAST('\0') };
/*ContentPubKey*/
static const DRM_WCHAR dwchAttribContentPubKey[]                        = { WCHAR_CAST('C'), WCHAR_CAST('o'), WCHAR_CAST('n'), WCHAR_CAST('t'), WCHAR_CAST('e'), WCHAR_CAST('n'), WCHAR_CAST('t'), WCHAR_CAST('P'), WCHAR_CAST('u'), WCHAR_CAST('b'), WCHAR_CAST('K'), WCHAR_CAST('e'), WCHAR_CAST('y'), WCHAR_CAST('\0') };
/*IssueDate*/
static const DRM_WCHAR dwchAttribIssueDate[]                            = { WCHAR_CAST('I'), WCHAR_CAST('s'), WCHAR_CAST('s'), WCHAR_CAST('u'), WCHAR_CAST('e'), WCHAR_CAST('D'), WCHAR_CAST('a'), WCHAR_CAST('t'), WCHAR_CAST('e'), WCHAR_CAST('\0') };
/*RemovalDate*/
static const DRM_WCHAR dwchAttribRemovalDate[]                          = { WCHAR_CAST('R'), WCHAR_CAST('e'), WCHAR_CAST('m'), WCHAR_CAST('o'), WCHAR_CAST('v'), WCHAR_CAST('a'), WCHAR_CAST('l'), WCHAR_CAST('D'), WCHAR_CAST('a'), WCHAR_CAST('t'), WCHAR_CAST('e'), WCHAR_CAST('\0') };
/*ServerPrivateKey*/
static const DRM_WCHAR dwchAttribServerPrivateKey[]                     = { WCHAR_CAST('S'), WCHAR_CAST('e'), WCHAR_CAST('r'), WCHAR_CAST('v'), WCHAR_CAST('e'), WCHAR_CAST('r'), WCHAR_CAST('P'), WCHAR_CAST('r'), WCHAR_CAST('i'), WCHAR_CAST('v'), WCHAR_CAST('a'), WCHAR_CAST('t'), WCHAR_CAST('e'), WCHAR_CAST('K'), WCHAR_CAST('e'), WCHAR_CAST('y'), WCHAR_CAST('\0') };
/*ServerPublicKey*/
static const DRM_WCHAR dwchAttribServerPublicKey[]                      = { WCHAR_CAST('S'), WCHAR_CAST('e'), WCHAR_CAST('r'), WCHAR_CAST('v'), WCHAR_CAST('e'), WCHAR_CAST('r'), WCHAR_CAST('P'), WCHAR_CAST('u'), WCHAR_CAST('b'), WCHAR_CAST('l'), WCHAR_CAST('i'), WCHAR_CAST('c'), WCHAR_CAST('K'), WCHAR_CAST('e'), WCHAR_CAST('y'), WCHAR_CAST('\0') };
/*ServerSeed*/
static const DRM_WCHAR dwchAttribServerSeed[]                           = { WCHAR_CAST('S'), WCHAR_CAST('e'), WCHAR_CAST('r'), WCHAR_CAST('v'), WCHAR_CAST('e'), WCHAR_CAST('r'), WCHAR_CAST('S'), WCHAR_CAST('e'), WCHAR_CAST('e'), WCHAR_CAST('d'), WCHAR_CAST('\0') };
/*WMDRMNETRevocationIndex*/
static const DRM_WCHAR dwchAttribWMDRMNETRevocationIndex[]              = { WCHAR_CAST('W'), WCHAR_CAST('M'), WCHAR_CAST('D'), WCHAR_CAST('R'), WCHAR_CAST('M'), WCHAR_CAST('N'), WCHAR_CAST('e'), WCHAR_CAST('t'), WCHAR_CAST('R'), WCHAR_CAST('e'), WCHAR_CAST('v'), WCHAR_CAST('o'), WCHAR_CAST('c'), WCHAR_CAST('a'), WCHAR_CAST('t'), WCHAR_CAST('i'), WCHAR_CAST('o'), WCHAR_CAST('n'), WCHAR_CAST('I'), WCHAR_CAST('n'), WCHAR_CAST('d'), WCHAR_CAST('e'), WCHAR_CAST('x'), WCHAR_CAST('\0') };
/*DeviceRevocationIndex*/
static const DRM_WCHAR dwchAttribDeviceRevocationIndex[]                = { WCHAR_CAST('D'), WCHAR_CAST('e'), WCHAR_CAST('v'), WCHAR_CAST('i'), WCHAR_CAST('c'), WCHAR_CAST('e'), WCHAR_CAST('R'), WCHAR_CAST('e'), WCHAR_CAST('v'), WCHAR_CAST('o'), WCHAR_CAST('c'), WCHAR_CAST('a'), WCHAR_CAST('t'), WCHAR_CAST('i'), WCHAR_CAST('o'), WCHAR_CAST('n'), WCHAR_CAST('I'), WCHAR_CAST('n'), WCHAR_CAST('d'), WCHAR_CAST('e'), WCHAR_CAST('x'), WCHAR_CAST('\0') };
/*TestOptions*/
static const DRM_WCHAR dwchAttribTestOptions[]                          = { WCHAR_CAST('T'), WCHAR_CAST('e'), WCHAR_CAST('s'), WCHAR_CAST('t'), WCHAR_CAST('O'), WCHAR_CAST('p'), WCHAR_CAST('t'), WCHAR_CAST('i'), WCHAR_CAST('o'), WCHAR_CAST('n'), WCHAR_CAST('s'), WCHAR_CAST('\0') };
/*ExcludeApps*/
static const DRM_WCHAR dwchAttribExcludeApps[]                          = { WCHAR_CAST('E'), WCHAR_CAST('x'), WCHAR_CAST('c'), WCHAR_CAST('l'), WCHAR_CAST('u'), WCHAR_CAST('d'), WCHAR_CAST('e'), WCHAR_CAST('A'), WCHAR_CAST('p'), WCHAR_CAST('p'), WCHAR_CAST('s'), WCHAR_CAST('\0') };
/*Description*/
static const DRM_WCHAR dwchAttribDescription[]                          = { WCHAR_CAST('D'), WCHAR_CAST('e'), WCHAR_CAST('s'), WCHAR_CAST('c'), WCHAR_CAST('r'), WCHAR_CAST('i'), WCHAR_CAST('p'), WCHAR_CAST('t'), WCHAR_CAST('i'), WCHAR_CAST('o'), WCHAR_CAST('n'), WCHAR_CAST('\0') };
/*SAPMode*/
static const DRM_WCHAR dwchAttribSAPMode[]                              = { WCHAR_CAST('S'), WCHAR_CAST('A'), WCHAR_CAST('P'), WCHAR_CAST('M'), WCHAR_CAST('o'), WCHAR_CAST('d'), WCHAR_CAST('e'), WCHAR_CAST('\0') };
/*UplinkKIDS*/
static const DRM_WCHAR dwchAttribUplinkKIDs[]                           = { WCHAR_CAST('U'), WCHAR_CAST('p'), WCHAR_CAST('l'), WCHAR_CAST('i'), WCHAR_CAST('n'), WCHAR_CAST('k'), WCHAR_CAST('K'), WCHAR_CAST('I'), WCHAR_CAST('D'), WCHAR_CAST('s'), WCHAR_CAST('\0') };
/*LGPUBKEY*/
static const DRM_WCHAR dwchAttribLGPUBKEY[]                             = { WCHAR_CAST('L'), WCHAR_CAST('G'), WCHAR_CAST('P'), WCHAR_CAST('U'), WCHAR_CAST('B'), WCHAR_CAST('K'), WCHAR_CAST('E'), WCHAR_CAST('Y'), WCHAR_CAST('\0') };
/*UID*/
static const DRM_WCHAR dwchAttribUID[]                                  = { WCHAR_CAST('U'), WCHAR_CAST('I'), WCHAR_CAST('D'), WCHAR_CAST('\0' )};
/*AttributeList*/
static const DRM_WCHAR dwchAttribAttributeList[]                        = { WCHAR_CAST('A'), WCHAR_CAST('t'), WCHAR_CAST('t'), WCHAR_CAST('r'), WCHAR_CAST('i'), WCHAR_CAST('b'), WCHAR_CAST('u'), WCHAR_CAST('t'), WCHAR_CAST('e'), WCHAR_CAST('L'), WCHAR_CAST('i'), WCHAR_CAST('s'), WCHAR_CAST('t'), WCHAR_CAST('\0') };
/*UseAESContentKey*/
static const DRM_WCHAR dwchAttribUseAESContentKey[]                     = { WCHAR_CAST('U'), WCHAR_CAST('s'), WCHAR_CAST('e'), WCHAR_CAST('A'), WCHAR_CAST('E'), WCHAR_CAST('S'), WCHAR_CAST('C'), WCHAR_CAST('o'), WCHAR_CAST('n'), WCHAR_CAST('t'), WCHAR_CAST('e'), WCHAR_CAST('n'), WCHAR_CAST('t'), WCHAR_CAST('K'), WCHAR_CAST('e'), WCHAR_CAST('y'), WCHAR_CAST('\0') };
/*UseECCPubKey*/
static const DRM_WCHAR dwchAttribUseECCPubKey[]                         = { WCHAR_CAST('U'), WCHAR_CAST('s'), WCHAR_CAST('e'), WCHAR_CAST('E'), WCHAR_CAST('C'), WCHAR_CAST('C'), WCHAR_CAST('P'), WCHAR_CAST('u'), WCHAR_CAST('b'), WCHAR_CAST('K'), WCHAR_CAST('e'), WCHAR_CAST('y'), WCHAR_CAST('\0') };
/*SourceID*/
static const DRM_WCHAR dwchAttribSourceID[]                             = { WCHAR_CAST('S'), WCHAR_CAST('o'), WCHAR_CAST('u'), WCHAR_CAST('r'), WCHAR_CAST('c'), WCHAR_CAST('e'), WCHAR_CAST('I'), WCHAR_CAST('D'), WCHAR_CAST('\0') };
/*UncompressedDigitalAudioLevels*/
static const DRM_WCHAR dwchAttribUnCompressedDigitalAudioLevels[]       = { WCHAR_CAST('U'), WCHAR_CAST('n'), WCHAR_CAST('c'), WCHAR_CAST('o'), WCHAR_CAST('m'), WCHAR_CAST('p'), WCHAR_CAST('r'), WCHAR_CAST('e'), WCHAR_CAST('s'), WCHAR_CAST('s'), WCHAR_CAST('e'), WCHAR_CAST('d'), WCHAR_CAST('D'), WCHAR_CAST('i'), WCHAR_CAST('g'), WCHAR_CAST('i'), WCHAR_CAST('t'), WCHAR_CAST('a'), WCHAR_CAST('l'), WCHAR_CAST('A'), WCHAR_CAST('u'), WCHAR_CAST('d'), WCHAR_CAST('i'), WCHAR_CAST('o'), WCHAR_CAST('L'), WCHAR_CAST('e'), WCHAR_CAST('v'), WCHAR_CAST('e'), WCHAR_CAST('l'), WCHAR_CAST('s'), WCHAR_CAST('\0') };
/*Cert1*/
static const DRM_WCHAR dwchAttribCert1[]                                = { WCHAR_CAST('C'), WCHAR_CAST('e'), WCHAR_CAST('r'), WCHAR_CAST('t'), WCHAR_CAST('1'), WCHAR_CAST('\0') };
/*Cert2*/
static const DRM_WCHAR dwchAttribCert2[]                                = { WCHAR_CAST('C'), WCHAR_CAST('e'), WCHAR_CAST('r'), WCHAR_CAST('t'), WCHAR_CAST('2'), WCHAR_CAST('\0') };
/*ContentKey*/
static const DRM_WCHAR dwchAttribContentKey[]                           = { WCHAR_CAST('C'), WCHAR_CAST('o'), WCHAR_CAST('n'), WCHAR_CAST('t'), WCHAR_CAST('e'), WCHAR_CAST('n'), WCHAR_CAST('t'), WCHAR_CAST('K'), WCHAR_CAST('e'), WCHAR_CAST('y'), WCHAR_CAST('\0') };
/*InclusionList*/
static const DRM_WCHAR dwchAttribInclusionList[]                        = { WCHAR_CAST('I'), WCHAR_CAST('n'), WCHAR_CAST('c'), WCHAR_CAST('l'), WCHAR_CAST('u'), WCHAR_CAST('s'), WCHAR_CAST('i'), WCHAR_CAST('o'), WCHAR_CAST('n'), WCHAR_CAST('L'), WCHAR_CAST('i'), WCHAR_CAST('s'), WCHAR_CAST('t'), WCHAR_CAST('\0') };
/*CannotPersist*/
static const DRM_WCHAR dwchAttribCannotPersist[]                        = { WCHAR_CAST('C'), WCHAR_CAST('a'), WCHAR_CAST('n'), WCHAR_CAST('n'), WCHAR_CAST('o'), WCHAR_CAST('t'), WCHAR_CAST('P'), WCHAR_CAST('e'), WCHAR_CAST('r'), WCHAR_CAST('s'), WCHAR_CAST('i'), WCHAR_CAST('s'), WCHAR_CAST('t'), WCHAR_CAST('\0') };
/*RestrictedSourceID*/
static const DRM_WCHAR dwchAttribRestrictedSourceID[]                   = { WCHAR_CAST('R'), WCHAR_CAST('e'), WCHAR_CAST('s'), WCHAR_CAST('t'), WCHAR_CAST('r'), WCHAR_CAST('i'), WCHAR_CAST('c'), WCHAR_CAST('t'), WCHAR_CAST('e'), WCHAR_CAST('d'), WCHAR_CAST('S'), WCHAR_CAST('o'), WCHAR_CAST('u'), WCHAR_CAST('r'), WCHAR_CAST('c'), WCHAR_CAST('e'), WCHAR_CAST('I'), WCHAR_CAST('D'), WCHAR_CAST('\0') };
/*WMDRMNETRevocationData*/
static const DRM_WCHAR dwchAttribWMDRMNETRevocationData[]               = { WCHAR_CAST('W'), WCHAR_CAST('M'), WCHAR_CAST('D'), WCHAR_CAST('R'), WCHAR_CAST('M'), WCHAR_CAST('N'), WCHAR_CAST('E'), WCHAR_CAST('T'), WCHAR_CAST('R'), WCHAR_CAST('e'), WCHAR_CAST('v'), WCHAR_CAST('o'), WCHAR_CAST('c'), WCHAR_CAST('a'), WCHAR_CAST('t'), WCHAR_CAST('i'), WCHAR_CAST('o'), WCHAR_CAST('n'), WCHAR_CAST('D'), WCHAR_CAST('a'), WCHAR_CAST('t') ,WCHAR_CAST('a'), WCHAR_CAST('\0') };
/*AnalogVideoExtensions*/
static const DRM_WCHAR dwchTagAnalogVideoExtensions[]                   = { WCHAR_CAST('A'), WCHAR_CAST('n'), WCHAR_CAST('a'), WCHAR_CAST('l'), WCHAR_CAST('o'), WCHAR_CAST('g'), WCHAR_CAST('V'), WCHAR_CAST('i'), WCHAR_CAST('d'), WCHAR_CAST('e'), WCHAR_CAST('o'), WCHAR_CAST('E'), WCHAR_CAST('x'), WCHAR_CAST('t'), WCHAR_CAST('e'), WCHAR_CAST('n'), WCHAR_CAST('s'), WCHAR_CAST('i'), WCHAR_CAST('o'), WCHAR_CAST('n'), WCHAR_CAST('s'), WCHAR_CAST('\0') };
/*EXTENSIONLIST*/
static const DRM_WCHAR dwchTagOPLExtensionList[]                        = { WCHAR_CAST('E'), WCHAR_CAST('X'), WCHAR_CAST('T'), WCHAR_CAST('E'), WCHAR_CAST('N'), WCHAR_CAST('S'), WCHAR_CAST('I'), WCHAR_CAST('O'), WCHAR_CAST('N'), WCHAR_CAST('L'), WCHAR_CAST('I'), WCHAR_CAST('S'), WCHAR_CAST('T'), WCHAR_CAST('\0') };
/*EXTENSION*/
static const DRM_WCHAR dwchTagOPLExtension[]                            = { WCHAR_CAST('E'), WCHAR_CAST('X'), WCHAR_CAST('T'), WCHAR_CAST('E'), WCHAR_CAST('N'), WCHAR_CAST('S'), WCHAR_CAST('I'), WCHAR_CAST('O'), WCHAR_CAST('N'), WCHAR_CAST('\0') };
/*guid*/
static const DRM_WCHAR dwchAttribGuid[]                                 = { WCHAR_CAST('g'), WCHAR_CAST('u'), WCHAR_CAST('i'), WCHAR_CAST('d'), WCHAR_CAST('\0') };
/*value*/
static const DRM_WCHAR dwchAttribValue[]                                = { WCHAR_CAST('v'), WCHAR_CAST('a'), WCHAR_CAST('l'), WCHAR_CAST('u'), WCHAR_CAST('e'), WCHAR_CAST('\0') };
/*length*/
static const DRM_WCHAR dwchAttribLength[]                               = { WCHAR_CAST('l'), WCHAR_CAST('e'), WCHAR_CAST('n'), WCHAR_CAST('g'), WCHAR_CAST('t'), WCHAR_CAST('h'), WCHAR_CAST('\0') };
/*RevInfoID*/
static const DRM_WCHAR dwchTagLARevInfo[]                               = { WCHAR_CAST('R'), WCHAR_CAST('e'), WCHAR_CAST('v'), WCHAR_CAST('I'), WCHAR_CAST('n'), WCHAR_CAST('f'), WCHAR_CAST('o'), WCHAR_CAST('\0') };
/*RevInfoID*/
static const DRM_WCHAR dwchAttribRevInfoID[]                            = { WCHAR_CAST('R'), WCHAR_CAST('e'), WCHAR_CAST('v'), WCHAR_CAST('I'), WCHAR_CAST('n'), WCHAR_CAST('f'), WCHAR_CAST('o'), WCHAR_CAST('I'), WCHAR_CAST('D'), WCHAR_CAST('\0') };
/*XMRVersion*/
static const DRM_WCHAR dwchAttribXMRVersion[]                           = { WCHAR_CAST('X'), WCHAR_CAST('M'), WCHAR_CAST('R'), WCHAR_CAST('V'), WCHAR_CAST('e'), WCHAR_CAST('r'), WCHAR_CAST('s'), WCHAR_CAST('i'), WCHAR_CAST('o'), WCHAR_CAST('n'), WCHAR_CAST('\0') };
/*XPolicy*/
static const DRM_WCHAR dwchTagExPolicy[]                                = { WCHAR_CAST('X'), WCHAR_CAST('P'), WCHAR_CAST('o'), WCHAR_CAST('l'), WCHAR_CAST('i'), WCHAR_CAST('c'), WCHAR_CAST('y'), WCHAR_CAST('\0') };
/*XPolicyRightsList*/
static const DRM_WCHAR dwchTagExPolicyRightsList[]                      = { WCHAR_CAST('X'), WCHAR_CAST('P'), WCHAR_CAST('o'), WCHAR_CAST('l'), WCHAR_CAST('i'), WCHAR_CAST('c'), WCHAR_CAST('y'), WCHAR_CAST('R'), WCHAR_CAST('i'), WCHAR_CAST('g'), WCHAR_CAST('h'), WCHAR_CAST('t'), WCHAR_CAST('s'), WCHAR_CAST('L'), WCHAR_CAST('i'), WCHAR_CAST('s'), WCHAR_CAST('t'), WCHAR_CAST('\0') };
/*XPolicyRight*/
static const DRM_WCHAR dwchTagExPolicyRight[]                           = { WCHAR_CAST('X'), WCHAR_CAST('P'), WCHAR_CAST('o'), WCHAR_CAST('l'), WCHAR_CAST('i'), WCHAR_CAST('c'), WCHAR_CAST('y'), WCHAR_CAST('R'), WCHAR_CAST('i'), WCHAR_CAST('g'), WCHAR_CAST('h'), WCHAR_CAST('t'), WCHAR_CAST('\0') };
/*XPolicyRestrictionList*/
static const DRM_WCHAR dwchTagExPolicyRestrictionsList[]                = { WCHAR_CAST('X'), WCHAR_CAST('P'), WCHAR_CAST('o'), WCHAR_CAST('l'), WCHAR_CAST('i'), WCHAR_CAST('c'), WCHAR_CAST('y'), WCHAR_CAST('R'), WCHAR_CAST('e'), WCHAR_CAST('s'), WCHAR_CAST('t'), WCHAR_CAST('r'), WCHAR_CAST('i'), WCHAR_CAST('c'), WCHAR_CAST('t'), WCHAR_CAST('i'), WCHAR_CAST('o'), WCHAR_CAST('n'), WCHAR_CAST('s'), WCHAR_CAST('L'), WCHAR_CAST('i'), WCHAR_CAST('s'), WCHAR_CAST('t'), WCHAR_CAST('\0') };
/*XPolicyRestriction*/
static const DRM_WCHAR dwchTagExPolicyRestriction[]                     = { WCHAR_CAST('X'), WCHAR_CAST('P'), WCHAR_CAST('o'), WCHAR_CAST('l'), WCHAR_CAST('i'), WCHAR_CAST('c'), WCHAR_CAST('y'), WCHAR_CAST('R'), WCHAR_CAST('e'), WCHAR_CAST('s'), WCHAR_CAST('t'), WCHAR_CAST('r'), WCHAR_CAST('i'), WCHAR_CAST('c'), WCHAR_CAST('t'), WCHAR_CAST('i'), WCHAR_CAST('o'), WCHAR_CAST('n'), WCHAR_CAST('\0') };
/*XPolicyObjectType*/
static const DRM_WCHAR dwchAttribExPolicyObjectType[]                   = { WCHAR_CAST('X'), WCHAR_CAST('P'), WCHAR_CAST('o'), WCHAR_CAST('l'), WCHAR_CAST('i'), WCHAR_CAST('c'), WCHAR_CAST('y'), WCHAR_CAST('O'), WCHAR_CAST('b'), WCHAR_CAST('j'), WCHAR_CAST('e'), WCHAR_CAST('c'), WCHAR_CAST('t'), WCHAR_CAST('T'), WCHAR_CAST('y'), WCHAR_CAST('p'), WCHAR_CAST('e'), WCHAR_CAST('\0') };
/*XPolicyObjectFlags*/
static const DRM_WCHAR dwchAttribExPolicyObjectFlags[]                  = { WCHAR_CAST('X'), WCHAR_CAST('P'), WCHAR_CAST('o'), WCHAR_CAST('l'), WCHAR_CAST('i'), WCHAR_CAST('c'), WCHAR_CAST('y'), WCHAR_CAST('O'), WCHAR_CAST('b'), WCHAR_CAST('j'), WCHAR_CAST('e'), WCHAR_CAST('c'), WCHAR_CAST('t'), WCHAR_CAST('F'), WCHAR_CAST('l'), WCHAR_CAST('a'), WCHAR_CAST('g'), WCHAR_CAST('s'), WCHAR_CAST('\0') };
/*XPolicySubObjectType*/
static const DRM_WCHAR dwchAttribExPolicySubObjectType[]                = { WCHAR_CAST('X'), WCHAR_CAST('P'), WCHAR_CAST('o'), WCHAR_CAST('l'), WCHAR_CAST('i'), WCHAR_CAST('c'), WCHAR_CAST('y'), WCHAR_CAST('S'), WCHAR_CAST('u'), WCHAR_CAST('b'), WCHAR_CAST('O'), WCHAR_CAST('b'), WCHAR_CAST('j'), WCHAR_CAST('e'), WCHAR_CAST('c'), WCHAR_CAST('t'), WCHAR_CAST('T'), WCHAR_CAST('y'), WCHAR_CAST('p'), WCHAR_CAST('e'), WCHAR_CAST('\0') };
/*XPolicySubObjectFlags*/
static const DRM_WCHAR dwchAttribExPolicySubObjectFlags[]               = { WCHAR_CAST('X'), WCHAR_CAST('P'), WCHAR_CAST('o'), WCHAR_CAST('l'), WCHAR_CAST('i'), WCHAR_CAST('c'), WCHAR_CAST('y'), WCHAR_CAST('S'), WCHAR_CAST('u'), WCHAR_CAST('b'), WCHAR_CAST('O'), WCHAR_CAST('b'), WCHAR_CAST('j'), WCHAR_CAST('e'), WCHAR_CAST('c'), WCHAR_CAST('t'), WCHAR_CAST('F'), WCHAR_CAST('l'), WCHAR_CAST('a'), WCHAR_CAST('g'), WCHAR_CAST('s'), WCHAR_CAST('\0') };
/*XPolicyPolicyData*/
static const DRM_WCHAR dwchAttribExPolicyPolicyData[]                   = { WCHAR_CAST('X'), WCHAR_CAST('P'), WCHAR_CAST('o'), WCHAR_CAST('l'), WCHAR_CAST('i'), WCHAR_CAST('c'), WCHAR_CAST('y'), WCHAR_CAST('P'), WCHAR_CAST('o'), WCHAR_CAST('l'), WCHAR_CAST('i'), WCHAR_CAST('c'), WCHAR_CAST('y'), WCHAR_CAST('D'), WCHAR_CAST('a'), WCHAR_CAST('t'), WCHAR_CAST('a'), WCHAR_CAST('\0') };
/*XPolicySecureStateType*/
static const DRM_WCHAR dwchAttribExPolicySecureStateType[]              = { WCHAR_CAST('X'), WCHAR_CAST('P'), WCHAR_CAST('o'), WCHAR_CAST('l'), WCHAR_CAST('i'), WCHAR_CAST('c'), WCHAR_CAST('y'), WCHAR_CAST('S'), WCHAR_CAST('e'), WCHAR_CAST('c'), WCHAR_CAST('u'), WCHAR_CAST('r'), WCHAR_CAST('e'), WCHAR_CAST('S'), WCHAR_CAST('t'), WCHAR_CAST('a'), WCHAR_CAST('t'), WCHAR_CAST('e'), WCHAR_CAST('T'), WCHAR_CAST('y'), WCHAR_CAST('p'), WCHAR_CAST('e'), WCHAR_CAST('\0') };
/*XPolicySecureStateData*/
static const DRM_WCHAR dwchAttribExPolicySecureStateData[]              = { WCHAR_CAST('X'), WCHAR_CAST('P'), WCHAR_CAST('o'), WCHAR_CAST('l'), WCHAR_CAST('i'), WCHAR_CAST('c'), WCHAR_CAST('y'), WCHAR_CAST('S'), WCHAR_CAST('e'), WCHAR_CAST('c'), WCHAR_CAST('u'), WCHAR_CAST('r'), WCHAR_CAST('e'), WCHAR_CAST('S'), WCHAR_CAST('t'), WCHAR_CAST('a'), WCHAR_CAST('t'), WCHAR_CAST('e'), WCHAR_CAST('D'), WCHAR_CAST('a'), WCHAR_CAST('t'), WCHAR_CAST('a'), WCHAR_CAST('\0') };
/*XPolicySecureStateDate*/
static const DRM_WCHAR dwchAttribExPolicySecureStateDate[]              = { WCHAR_CAST('X'), WCHAR_CAST('P'), WCHAR_CAST('o'), WCHAR_CAST('l'), WCHAR_CAST('i'), WCHAR_CAST('c'), WCHAR_CAST('y'), WCHAR_CAST('S'), WCHAR_CAST('e'), WCHAR_CAST('c'), WCHAR_CAST('u'), WCHAR_CAST('r'), WCHAR_CAST('e'), WCHAR_CAST('S'), WCHAR_CAST('t'), WCHAR_CAST('a'), WCHAR_CAST('t'), WCHAR_CAST('e'), WCHAR_CAST('D'), WCHAR_CAST('a'), WCHAR_CAST('t'), WCHAR_CAST('e'), WCHAR_CAST('\0') };
/*XPolicySecureStateCount*/
static const DRM_WCHAR dwchAttribExPolicySecureStateCount[]             = { WCHAR_CAST('X'), WCHAR_CAST('P'), WCHAR_CAST('o'), WCHAR_CAST('l'), WCHAR_CAST('i'), WCHAR_CAST('c'), WCHAR_CAST('y'), WCHAR_CAST('S'), WCHAR_CAST('e'), WCHAR_CAST('c'), WCHAR_CAST('u'), WCHAR_CAST('r'), WCHAR_CAST('e'), WCHAR_CAST('S'), WCHAR_CAST('t'), WCHAR_CAST('a'), WCHAR_CAST('t'), WCHAR_CAST('e'), WCHAR_CAST('C'), WCHAR_CAST('o'), WCHAR_CAST('u'), WCHAR_CAST('n'), WCHAR_CAST('t'), WCHAR_CAST('\0') };
/*XPolicySecureStateSize*/
static const DRM_WCHAR dwchAttribExPolicySecureStateSize[]              = { WCHAR_CAST('X'), WCHAR_CAST('P'), WCHAR_CAST('o'), WCHAR_CAST('l'), WCHAR_CAST('i'), WCHAR_CAST('c'), WCHAR_CAST('y'), WCHAR_CAST('S'), WCHAR_CAST('e'), WCHAR_CAST('c'), WCHAR_CAST('u'), WCHAR_CAST('r'), WCHAR_CAST('e'), WCHAR_CAST('S'), WCHAR_CAST('t'), WCHAR_CAST('a'), WCHAR_CAST('t'), WCHAR_CAST('e'), WCHAR_CAST('S'), WCHAR_CAST('i'), WCHAR_CAST('z'), WCHAR_CAST('e'), WCHAR_CAST('\0') };
/*XPolicyPlayExtended*/
static const DRM_WCHAR dwchTagPlayExtendedRestriction[]                 = { WCHAR_CAST('X'), WCHAR_CAST('P'), WCHAR_CAST('o'), WCHAR_CAST('l'), WCHAR_CAST('i'), WCHAR_CAST('c'), WCHAR_CAST('y'), WCHAR_CAST('P'), WCHAR_CAST('l'), WCHAR_CAST('a'), WCHAR_CAST('y'), WCHAR_CAST('E'), WCHAR_CAST('x'), WCHAR_CAST('t'), WCHAR_CAST('e'), WCHAR_CAST('n'), WCHAR_CAST('d'), WCHAR_CAST('e'), WCHAR_CAST('d'), WCHAR_CAST('\0') };
/*XPolicyBurnExtended*/
static const DRM_WCHAR dwchTagBurnExtendedRestriction[]                 = { WCHAR_CAST('X'), WCHAR_CAST('P'), WCHAR_CAST('o'), WCHAR_CAST('l'), WCHAR_CAST('i'), WCHAR_CAST('c'), WCHAR_CAST('y'), WCHAR_CAST('B'), WCHAR_CAST('u'), WCHAR_CAST('r'), WCHAR_CAST('n'), WCHAR_CAST('E'), WCHAR_CAST('x'), WCHAR_CAST('t'), WCHAR_CAST('e'), WCHAR_CAST('n'), WCHAR_CAST('d'), WCHAR_CAST('e'), WCHAR_CAST('d'), WCHAR_CAST('\0') };
/*DigitalAudioRestrictions*/
static const DRM_WCHAR dwchTagDigitalAudioRestrictions[]                = { WCHAR_CAST('D'), WCHAR_CAST('i'), WCHAR_CAST('g'), WCHAR_CAST('i'), WCHAR_CAST('t'), WCHAR_CAST('a'), WCHAR_CAST('l'), WCHAR_CAST('A'), WCHAR_CAST('u'), WCHAR_CAST('d'), WCHAR_CAST('i'), WCHAR_CAST('o'), WCHAR_CAST('R'), WCHAR_CAST('e'), WCHAR_CAST('s'), WCHAR_CAST('t'), WCHAR_CAST('r'), WCHAR_CAST('i'), WCHAR_CAST('c'), WCHAR_CAST('t'), WCHAR_CAST('i'), WCHAR_CAST('o'), WCHAR_CAST('n'), WCHAR_CAST('s'), WCHAR_CAST('\0') };
/*PolicyMetadataList*/
static const DRM_WCHAR dwchTagPolicyMetadataList[]                      = { WCHAR_CAST('P'), WCHAR_CAST('o'), WCHAR_CAST('l'), WCHAR_CAST('i'), WCHAR_CAST('c'), WCHAR_CAST('y'), WCHAR_CAST('M'), WCHAR_CAST('e'), WCHAR_CAST('t'), WCHAR_CAST('a'), WCHAR_CAST('d'), WCHAR_CAST('a'), WCHAR_CAST('t'), WCHAR_CAST('a'), WCHAR_CAST('L'), WCHAR_CAST('i'), WCHAR_CAST('s'), WCHAR_CAST('t'), WCHAR_CAST('\0') };
/*PolicyMetaData*/
static const DRM_WCHAR dwchTagPolicyMetadata[]                          = { WCHAR_CAST('P'), WCHAR_CAST('o'), WCHAR_CAST('l'), WCHAR_CAST('i'), WCHAR_CAST('c'), WCHAR_CAST('y'), WCHAR_CAST('M'), WCHAR_CAST('e'), WCHAR_CAST('t'), WCHAR_CAST('a'), WCHAR_CAST('d'), WCHAR_CAST('a'), WCHAR_CAST('t'), WCHAR_CAST('a'), WCHAR_CAST('\0') };
/*CanNotBind*/
static const DRM_WCHAR dwchAttribCanNotBind[]                           = { WCHAR_CAST('C'), WCHAR_CAST('a'), WCHAR_CAST('n'), WCHAR_CAST('N'), WCHAR_CAST('o'), WCHAR_CAST('t'), WCHAR_CAST('B'), WCHAR_CAST('i'), WCHAR_CAST('n'), WCHAR_CAST('d'), WCHAR_CAST('\0') };
/*DoNotSign*/
static const DRM_WCHAR dwchAttribDoNotSign[]                            = { WCHAR_CAST('D'), WCHAR_CAST('o'), WCHAR_CAST('N'), WCHAR_CAST('o'), WCHAR_CAST('t'), WCHAR_CAST('S'), WCHAR_CAST('i'), WCHAR_CAST('g'), WCHAR_CAST('n'), WCHAR_CAST('\0') };
/*RSAPublicKey*/
static const DRM_WCHAR dwchAttribRSAPublicKey[]                         = { WCHAR_CAST('R'), WCHAR_CAST('S'), WCHAR_CAST('A'), WCHAR_CAST('P'), WCHAR_CAST('u'), WCHAR_CAST('b'), WCHAR_CAST('l'), WCHAR_CAST('i'), WCHAR_CAST('c'), WCHAR_CAST('K'), WCHAR_CAST('e'), WCHAR_CAST('y'), WCHAR_CAST('\0') };
/*ECCPublicKey*/
static const DRM_WCHAR dwchAttribECCPublicKey[]                         = { WCHAR_CAST('E'), WCHAR_CAST('C'), WCHAR_CAST('C'), WCHAR_CAST('P'), WCHAR_CAST('u'), WCHAR_CAST('b'), WCHAR_CAST('l'), WCHAR_CAST('i'), WCHAR_CAST('c'), WCHAR_CAST('K'), WCHAR_CAST('e'), WCHAR_CAST('y'), WCHAR_CAST('\0') };
/*UplinkContentKey*/
static const DRM_WCHAR dwchAttribUplinkContentKey[]                     = { WCHAR_CAST('U'), WCHAR_CAST('p'), WCHAR_CAST('l'), WCHAR_CAST('i'), WCHAR_CAST('n'), WCHAR_CAST('k'), WCHAR_CAST('C'), WCHAR_CAST('o'), WCHAR_CAST('n'), WCHAR_CAST('t'), WCHAR_CAST('e'), WCHAR_CAST('n'), WCHAR_CAST('t'), WCHAR_CAST('K'), WCHAR_CAST('e'), WCHAR_CAST('y'), WCHAR_CAST('\0') };
/*Use16ByteCheckSum*/
static const DRM_WCHAR dwchAttribUse16ByteCheckSum[]                    = { WCHAR_CAST('U'), WCHAR_CAST('s'), WCHAR_CAST('e'), WCHAR_CAST('1'), WCHAR_CAST('6'), WCHAR_CAST('B'), WCHAR_CAST('y'), WCHAR_CAST('t'), WCHAR_CAST('e'), WCHAR_CAST('C'), WCHAR_CAST('h'), WCHAR_CAST('e'), WCHAR_CAST('c'), WCHAR_CAST('k'), WCHAR_CAST('S'), WCHAR_CAST('u'), WCHAR_CAST('m'), WCHAR_CAST('\0') };
/*RevInfo*/
static const DRM_WCHAR dwchTagRevInfoForLicenseResponse[]               = { WCHAR_CAST('R'), WCHAR_CAST('e'), WCHAR_CAST('v'), WCHAR_CAST('I'), WCHAR_CAST('n'), WCHAR_CAST('f'), WCHAR_CAST('o'), WCHAR_CAST('\0') };
/*EmbedLicense*/
static const DRM_WCHAR dwchAttribEmbedLicense[]                         = { WCHAR_CAST('E'), WCHAR_CAST('m'), WCHAR_CAST('b'), WCHAR_CAST('e'), WCHAR_CAST('d'), WCHAR_CAST('L'), WCHAR_CAST('i'), WCHAR_CAST('c'), WCHAR_CAST('e'), WCHAR_CAST('n'), WCHAR_CAST('s'), WCHAR_CAST('e'), WCHAR_CAST('\0') };
/*DomainID*/
static const DRM_WCHAR dwchTagDomainID[]                                = { WCHAR_CAST('D'), WCHAR_CAST('o'), WCHAR_CAST('m'), WCHAR_CAST('a'), WCHAR_CAST('i'), WCHAR_CAST('n'), WCHAR_CAST('I'), WCHAR_CAST('D'), WCHAR_CAST('\0') };
/*AccountID*/
static const DRM_WCHAR dwchAttribAccountID[]                            = { WCHAR_CAST('A'), WCHAR_CAST('c'), WCHAR_CAST('c'), WCHAR_CAST('o'), WCHAR_CAST('u'), WCHAR_CAST('n'), WCHAR_CAST('t'), WCHAR_CAST('I'), WCHAR_CAST('D'), WCHAR_CAST('\0') };
/*Revision*/
static const DRM_WCHAR dwchAttribRevision[]                             = { WCHAR_CAST('R'), WCHAR_CAST('e'), WCHAR_CAST('v'), WCHAR_CAST('i'), WCHAR_CAST('s'), WCHAR_CAST('i'), WCHAR_CAST('o'), WCHAR_CAST('n'), WCHAR_CAST('\0') };
/*SerialNumber*/
static const DRM_WCHAR dwchAttribSerialNumber[]                         = { WCHAR_CAST('S'), WCHAR_CAST('e'), WCHAR_CAST('r'), WCHAR_CAST('i'), WCHAR_CAST('a'), WCHAR_CAST('l'), WCHAR_CAST('N'), WCHAR_CAST('u'), WCHAR_CAST('m'), WCHAR_CAST('b'), WCHAR_CAST('e'), WCHAR_CAST('r'), WCHAR_CAST('\0') };
/*PlayEnablerGUIDList*/
static const DRM_WCHAR dwchTagPlayEnablerGUIDList[]                     = { WCHAR_CAST('P'), WCHAR_CAST('l'), WCHAR_CAST('a'), WCHAR_CAST('y'), WCHAR_CAST('E'), WCHAR_CAST('n'), WCHAR_CAST('a'), WCHAR_CAST('b'), WCHAR_CAST('l'), WCHAR_CAST('e'), WCHAR_CAST('r'), WCHAR_CAST('G'), WCHAR_CAST('U'), WCHAR_CAST('I'), WCHAR_CAST('D'), WCHAR_CAST('L'), WCHAR_CAST('i'), WCHAR_CAST('s'), WCHAR_CAST('t'), WCHAR_CAST('\0') };
/*PlayEnablerGUID*/
static const DRM_WCHAR dwchTagPlayEnablerGUID[]                         = { WCHAR_CAST('P'), WCHAR_CAST('l'), WCHAR_CAST('a'), WCHAR_CAST('y'), WCHAR_CAST('E'), WCHAR_CAST('n'), WCHAR_CAST('a'), WCHAR_CAST('b'), WCHAR_CAST('l'), WCHAR_CAST('e'), WCHAR_CAST('r'), WCHAR_CAST('G'), WCHAR_CAST('U'), WCHAR_CAST('I'), WCHAR_CAST('D'), WCHAR_CAST('\0') };
/*CopyEnablerGUIDList*/
static const DRM_WCHAR dwchTagCopyEnablerGUIDList[]                     = { WCHAR_CAST('C'), WCHAR_CAST('o'), WCHAR_CAST('p'), WCHAR_CAST('y'), WCHAR_CAST('E'), WCHAR_CAST('n'), WCHAR_CAST('a'), WCHAR_CAST('b'), WCHAR_CAST('l'), WCHAR_CAST('e'), WCHAR_CAST('r'), WCHAR_CAST('G'), WCHAR_CAST('U'), WCHAR_CAST('I'), WCHAR_CAST('D'), WCHAR_CAST('L'), WCHAR_CAST('i'), WCHAR_CAST('s'), WCHAR_CAST('t'), WCHAR_CAST('\0') };
/*CopyEnablerGUID*/
static const DRM_WCHAR dwchTagCopyEnablerGUID[]                         = { WCHAR_CAST('C'), WCHAR_CAST('o'), WCHAR_CAST('p'), WCHAR_CAST('y'), WCHAR_CAST('E'), WCHAR_CAST('n'), WCHAR_CAST('a'), WCHAR_CAST('b'), WCHAR_CAST('l'), WCHAR_CAST('e'), WCHAR_CAST('r'), WCHAR_CAST('G'), WCHAR_CAST('U'), WCHAR_CAST('I'), WCHAR_CAST('D'), WCHAR_CAST('\0') };
/*CopyCount2*/
static const DRM_WCHAR dwchAttribCopyCount2[]                           = { WCHAR_CAST('C'), WCHAR_CAST('o'), WCHAR_CAST('p'), WCHAR_CAST('y'), WCHAR_CAST('C'), WCHAR_CAST('o'), WCHAR_CAST('u'), WCHAR_CAST('n'), WCHAR_CAST('t'), WCHAR_CAST('2'), WCHAR_CAST('\0') };
/*DeviceKey*/
static const DRM_WCHAR dwchAttribDeviceKey[]                            = { WCHAR_CAST('D'), WCHAR_CAST('e'), WCHAR_CAST('v'), WCHAR_CAST('i'), WCHAR_CAST('c'), WCHAR_CAST('e'), WCHAR_CAST('K'), WCHAR_CAST('e'), WCHAR_CAST('y'), WCHAR_CAST('\0') };
/*VerifyExtensibleObject*/
static const DRM_WCHAR dwchAttribVerifyExtensibleObject[]               = { WCHAR_CAST('V'), WCHAR_CAST('e'), WCHAR_CAST('r'), WCHAR_CAST('i'), WCHAR_CAST('f'), WCHAR_CAST('y'), WCHAR_CAST('E'), WCHAR_CAST('x'), WCHAR_CAST('t'), WCHAR_CAST('e'), WCHAR_CAST('n'), WCHAR_CAST('s'), WCHAR_CAST('i'), WCHAR_CAST('b'), WCHAR_CAST('l'), WCHAR_CAST('e'), WCHAR_CAST('O'), WCHAR_CAST('b'), WCHAR_CAST('j'), WCHAR_CAST('e'), WCHAR_CAST('c'), WCHAR_CAST('t'), WCHAR_CAST('\0') };
/*ExtensibleDepth*/
static const DRM_WCHAR dwchAttribExtensibleDepth[]                      = { WCHAR_CAST('E'), WCHAR_CAST('x'), WCHAR_CAST('t'), WCHAR_CAST('e'), WCHAR_CAST('n'), WCHAR_CAST('s'), WCHAR_CAST('i'), WCHAR_CAST('b'), WCHAR_CAST('l'), WCHAR_CAST('e'), WCHAR_CAST('D'), WCHAR_CAST('e'), WCHAR_CAST('p'), WCHAR_CAST('t'), WCHAR_CAST('h'), WCHAR_CAST('\0') };
/*VerifyPlayRightsExtensibleContainer*/
static const DRM_WCHAR dwchAttribVerifyPlayRightsExtensibleContainer[]  = { WCHAR_CAST('V'), WCHAR_CAST('e'), WCHAR_CAST('r'), WCHAR_CAST('i'), WCHAR_CAST('f'), WCHAR_CAST('y'), WCHAR_CAST('P'), WCHAR_CAST('l'), WCHAR_CAST('a'), WCHAR_CAST('y'), WCHAR_CAST('R'), WCHAR_CAST('i'), WCHAR_CAST('g'), WCHAR_CAST('h'), WCHAR_CAST('t'), WCHAR_CAST('s'), WCHAR_CAST('E'), WCHAR_CAST('x'), WCHAR_CAST('t'), WCHAR_CAST('e'), WCHAR_CAST('n'), WCHAR_CAST('s'), WCHAR_CAST('i'), WCHAR_CAST('b'), WCHAR_CAST('l'), WCHAR_CAST('e'), WCHAR_CAST('C'), WCHAR_CAST('o'), WCHAR_CAST('n'), WCHAR_CAST('t'), WCHAR_CAST('a'), WCHAR_CAST('i'), WCHAR_CAST('n'), WCHAR_CAST('e'), WCHAR_CAST('r'), WCHAR_CAST('\0') };
/*VerifyPlayRightsExtensibleObject*/
static const DRM_WCHAR dwchAttribVerifyPlayRightsExtensibleObject[]     = { WCHAR_CAST('V'), WCHAR_CAST('e'), WCHAR_CAST('r'), WCHAR_CAST('i'), WCHAR_CAST('f'), WCHAR_CAST('y'), WCHAR_CAST('P'), WCHAR_CAST('l'), WCHAR_CAST('a'), WCHAR_CAST('y'), WCHAR_CAST('R'), WCHAR_CAST('i'), WCHAR_CAST('g'), WCHAR_CAST('h'), WCHAR_CAST('t'), WCHAR_CAST('s'), WCHAR_CAST('E'), WCHAR_CAST('x'), WCHAR_CAST('t'), WCHAR_CAST('e'), WCHAR_CAST('n'), WCHAR_CAST('s'), WCHAR_CAST('i'), WCHAR_CAST('b'), WCHAR_CAST('l'), WCHAR_CAST('e'), WCHAR_CAST('O'), WCHAR_CAST('b'), WCHAR_CAST('j'), WCHAR_CAST('e'), WCHAR_CAST('c'), WCHAR_CAST('t'), WCHAR_CAST('\0') };
/*MoveEnabler*/
static const DRM_WCHAR dwchAttribMoveEnabler[]                          = { WCHAR_CAST('M'), WCHAR_CAST('o'), WCHAR_CAST('v'), WCHAR_CAST('e'), WCHAR_CAST('E'), WCHAR_CAST('n'), WCHAR_CAST('a'), WCHAR_CAST('b'), WCHAR_CAST('l'), WCHAR_CAST('e'), WCHAR_CAST('r'), WCHAR_CAST('\0') };
/*TempStoreOnly*/
static const DRM_WCHAR dwchAttribTempStoreOnly[]                        = { WCHAR_CAST('T'), WCHAR_CAST('e'), WCHAR_CAST('m'), WCHAR_CAST('p'), WCHAR_CAST('S'), WCHAR_CAST('t'), WCHAR_CAST('o'), WCHAR_CAST('r'), WCHAR_CAST('e'), WCHAR_CAST('O'), WCHAR_CAST('n'), WCHAR_CAST('l'), WCHAR_CAST('y'), WCHAR_CAST('\0') };
/*AuxKey*/
static const DRM_WCHAR dwchTagAuxKey[]                                  = { WCHAR_CAST('A'), WCHAR_CAST('u'), WCHAR_CAST('x'), WCHAR_CAST('K'), WCHAR_CAST('e'), WCHAR_CAST('y'), WCHAR_CAST('\0') };
/*UplinkX*/
static const DRM_WCHAR dwchTagUplinkX[]                                 = { WCHAR_CAST('U'), WCHAR_CAST('p'), WCHAR_CAST('l'), WCHAR_CAST('i'), WCHAR_CAST('n'), WCHAR_CAST('k'), WCHAR_CAST('X'), WCHAR_CAST('\0') };

static const DRM_CONST_STRING g_cszMID                                          = CREATE_DRM_STRING(dwchMID);
static const DRM_CONST_STRING g_cszMETERCERT                                    = CREATE_DRM_STRING(dwchMETERCERT);
static const DRM_CONST_STRING g_cszData                                         = CREATE_DRM_STRING( dwchData );
extern const DRM_CONST_STRING g_dstrTagRightsPool                               = CREATE_DRM_STRING(dwchTagRightsPool);
extern const DRM_CONST_STRING g_dstrTagLicenseGroup                             = CREATE_DRM_STRING(dwchTagLicenseGroup);
extern const DRM_CONST_STRING g_dstrAttribname                                  = CREATE_DRM_STRING(dwchAttribname);
extern const DRM_CONST_STRING g_dstrTagLicenseRights                            = CREATE_DRM_STRING(dwchTagLicenseRights);
extern const DRM_CONST_STRING g_dstrTagLeaf                                     = CREATE_DRM_STRING(dwchTagLeaf);
extern const DRM_CONST_STRING g_dstrTagMETERCERT                                = CREATE_DRM_STRING(dwchTagMETERCERT);
extern const DRM_CONST_STRING g_dstrTagMETA                                     = CREATE_DRM_STRING(dwchTagMETA);
extern const DRM_CONST_STRING g_dstrAttribTestCaseID                            = CREATE_DRM_STRING(dwchAttribTestCaseID);
extern const DRM_CONST_STRING g_dstrAttribType                                  = CREATE_DRM_STRING(dwchAttribType);
extern const DRM_CONST_STRING g_dstrAttribAllowPlay                             = CREATE_DRM_STRING(dwchAttribAllowPlay);
extern const DRM_CONST_STRING g_dstrAttribPlayCount                             = CREATE_DRM_STRING(dwchAttribPlayCount);
extern const DRM_CONST_STRING g_dstrAttribAllowCopy                             = CREATE_DRM_STRING(dwchAttribAllowCopy);
extern const DRM_CONST_STRING g_dstrAttribCopyCount                             = CREATE_DRM_STRING(dwchAttribCopyCount);
extern const DRM_CONST_STRING g_dstrAttribAllowTransferToSDMI                   = CREATE_DRM_STRING(dwchAttribAllowTransferToSDMI);
extern const DRM_CONST_STRING g_dstrAttribAllowTransferToNonSDMI                = CREATE_DRM_STRING(dwchAttribAllowTransferToNonSDMI);
extern const DRM_CONST_STRING g_dstrAttribTransferCount                         = CREATE_DRM_STRING(dwchAttribTransferCount);
extern const DRM_CONST_STRING g_dstrAttribCopyProtectionLevel                   = CREATE_DRM_STRING(dwchAttribCopyProtectionLevel);
extern const DRM_CONST_STRING g_dstrAttribAnalogVideo                           = CREATE_DRM_STRING(dwchAttribAnalogVideo);
extern const DRM_CONST_STRING g_dstrAttribUnCompressedDigitalVideo              = CREATE_DRM_STRING(dwchAttribUnCompressedDigitalVideo);
extern const DRM_CONST_STRING g_dstrAttribUnCompressedDigitalAudio              = CREATE_DRM_STRING(dwchAttribUnCompressedDigitalAudio);
extern const DRM_CONST_STRING g_dstrAttribCompressedDigitalVideo                = CREATE_DRM_STRING(dwchAttribCompressedDigitalVideo);
extern const DRM_CONST_STRING g_dstrAttribCompressedDigitalAudio                = CREATE_DRM_STRING(dwchAttribCompressedDigitalAudio);
extern const DRM_CONST_STRING g_dstrAttribAllowBackUpRestore                    = CREATE_DRM_STRING(dwchAttribAllowBackUpRestore);
extern const DRM_CONST_STRING g_dstrAttribAllowBurnToCD                         = CREATE_DRM_STRING(dwchAttribAllowBurnToCD);
extern const DRM_CONST_STRING g_dstrAttribBurnToCDCount                         = CREATE_DRM_STRING(dwchAttribBurnToCDCount);
extern const DRM_CONST_STRING g_dstrAttribAllowPlaylistBurn                     = CREATE_DRM_STRING(dwchAttribAllowPlaylistBurn);
extern const DRM_CONST_STRING g_dstrAttribPlaylistBurnTrackCount                = CREATE_DRM_STRING(dwchAttribPlaylistBurnTrackCount);
extern const DRM_CONST_STRING g_dstrAttribMaxPlaylistBurnCount                  = CREATE_DRM_STRING(dwchAttribMaxPlaylistBurnCount);
extern const DRM_CONST_STRING g_dstrAttribAllowCollaborativePlay                = CREATE_DRM_STRING(dwchAttribAllowCollaborativePlay);
extern const DRM_CONST_STRING g_dstrAttribBeginDate                             = CREATE_DRM_STRING(dwchAttribBeginDate);
extern const DRM_CONST_STRING g_dstrAttribExpirationDate                        = CREATE_DRM_STRING(dwchAttribExpirationDate);
extern const DRM_CONST_STRING g_dstrAttribExpirationAfterFirstUse               = CREATE_DRM_STRING(dwchAttribExpirationAfterFirstUse);
extern const DRM_CONST_STRING g_dstrAttribExpirationAfterFirstPlay              = CREATE_DRM_STRING(dwchAttribExpirationAfterFirstPlay);
extern const DRM_CONST_STRING g_dstrAttribExpirationOnStore                     = CREATE_DRM_STRING(dwchAttribExpirationOnStore);
extern const DRM_CONST_STRING g_dstrAttribGracePeriod                           = CREATE_DRM_STRING(dwchAttribGracePeriod);
extern const DRM_CONST_STRING g_dstrAttribDeleteOnClockRollback                 = CREATE_DRM_STRING(dwchAttribDeleteOnClockRollback);
extern const DRM_CONST_STRING g_dstrAttribDisableOnClockRollback                = CREATE_DRM_STRING(dwchAttribDisableOnClockRollback);
extern const DRM_CONST_STRING g_dstrAttribMinimumSecurityLevel                  = CREATE_DRM_STRING(dwchAttribMinimumSecurityLevel);
extern const DRM_CONST_STRING g_dstrAttribMinimumAppCrlVersion                  = CREATE_DRM_STRING(dwchAttribMinimumAppCrlVersion);
extern const DRM_CONST_STRING g_dstrAttribMinimumDeviceCrlVersion               = CREATE_DRM_STRING(dwchAttribMinimumDeviceCrlVersion);
extern const DRM_CONST_STRING g_dstrAttribMinimumClientSDKSecurity              = CREATE_DRM_STRING(dwchAttribMinimumClientSDKSecurity);
extern const DRM_CONST_STRING g_dstrAttribPMRights                              = CREATE_DRM_STRING(dwchAttribPMRights);
extern const DRM_CONST_STRING g_dstrAttribPMAppSecurity                         = CREATE_DRM_STRING(dwchAttribPMAppSecurity);
extern const DRM_CONST_STRING g_dstrAttribPMExpirationDate                      = CREATE_DRM_STRING(dwchAttribPMExpirationDate);
extern const DRM_CONST_STRING g_dstrAttribAllowSaveStreamProtected              = CREATE_DRM_STRING(dwchAttribAllowSaveStreamProtected);
extern const DRM_CONST_STRING g_dstrAttribMeteringID                            = CREATE_DRM_STRING(dwchAttribMeteringID);
extern const DRM_CONST_STRING g_dstrAttribKID                                   = CREATE_DRM_STRING(dwchAttribKID);
extern const DRM_CONST_STRING g_dstrAttribPriority                              = CREATE_DRM_STRING(dwchAttribPriority);
extern const DRM_CONST_STRING g_dstrAttribRevInfoVersion                        = CREATE_DRM_STRING(dwchAttribRevInfoVersion);
extern const DRM_CONST_STRING g_dstrAttribIndivVersion                          = CREATE_DRM_STRING(dwchAttribIndivVersion);
extern const DRM_CONST_STRING g_dstrAttribLID                                   = CREATE_DRM_STRING(dwchAttribLID);
extern const DRM_CONST_STRING g_dstrAttribRevocationList                        = CREATE_DRM_STRING(dwchAttribRevocationList);
extern const DRM_CONST_STRING g_dstrAttribContentPubKey                         = CREATE_DRM_STRING(dwchAttribContentPubKey);
extern const DRM_CONST_STRING g_dstrAttribIssueDate                             = CREATE_DRM_STRING(dwchAttribIssueDate);
extern const DRM_CONST_STRING g_dstrAttribRemovalDate                           = CREATE_DRM_STRING(dwchAttribRemovalDate);
extern const DRM_CONST_STRING g_dstrAttribServerPrivateKey                      = CREATE_DRM_STRING(dwchAttribServerPrivateKey);
extern const DRM_CONST_STRING g_dstrAttribServerPublicKey                       = CREATE_DRM_STRING(dwchAttribServerPublicKey);
extern const DRM_CONST_STRING g_dstrAttribServerSeed                            = CREATE_DRM_STRING(dwchAttribServerSeed);
extern const DRM_CONST_STRING g_dstrAttribWMDRMNetRevocationIndex               = CREATE_DRM_STRING(dwchAttribWMDRMNETRevocationIndex);
extern const DRM_CONST_STRING g_dstrAttribDeviceRevocationIndex                 = CREATE_DRM_STRING(dwchAttribDeviceRevocationIndex);
extern const DRM_CONST_STRING g_dstrAttribTestOptions                           = CREATE_DRM_STRING(dwchAttribTestOptions);
extern const DRM_CONST_STRING g_dstrAttribExcludeApps                           = CREATE_DRM_STRING(dwchAttribExcludeApps);
extern const DRM_CONST_STRING g_dstrAttribDescription                           = CREATE_DRM_STRING(dwchAttribDescription);
extern const DRM_CONST_STRING g_dstrAttribSAPMode                               = CREATE_DRM_STRING(dwchAttribSAPMode);
extern const DRM_CONST_STRING g_dstrAttribUplinkKIDs                            = CREATE_DRM_STRING(dwchAttribUplinkKIDs);
extern const DRM_CONST_STRING g_dstrAttribLGPUBKEY                              = CREATE_DRM_STRING(dwchAttribLGPUBKEY);
extern const DRM_CONST_STRING g_dstrAttribUID                                   = CREATE_DRM_STRING(dwchAttribUID);
extern const DRM_CONST_STRING g_dstrAttribAttributeList                         = CREATE_DRM_STRING(dwchAttribAttributeList);
extern const DRM_CONST_STRING g_dstrAttribUseAESContentKey                      = CREATE_DRM_STRING(dwchAttribUseAESContentKey);
extern const DRM_CONST_STRING g_dstrAttribUseECCPubKey                          = CREATE_DRM_STRING(dwchAttribUseECCPubKey);
extern const DRM_CONST_STRING g_dstrAttribSourceID                              = CREATE_DRM_STRING(dwchAttribSourceID);
extern const DRM_CONST_STRING g_dstrAttribUnCompressedDigitalAudioLevels        = CREATE_DRM_STRING(dwchAttribUnCompressedDigitalAudioLevels);
extern const DRM_CONST_STRING g_dstrAttribCert1                                 = CREATE_DRM_STRING(dwchAttribCert1);
extern const DRM_CONST_STRING g_dstrAttribCert2                                 = CREATE_DRM_STRING(dwchAttribCert2);
extern const DRM_CONST_STRING g_dstrAttribContentKey                            = CREATE_DRM_STRING(dwchAttribContentKey);
extern const DRM_CONST_STRING g_dstrAttribInclusionList                         = CREATE_DRM_STRING(dwchAttribInclusionList);
extern const DRM_CONST_STRING g_dstrAttribCannotPersist                         = CREATE_DRM_STRING(dwchAttribCannotPersist);
extern const DRM_CONST_STRING g_dstrAttribRestrictedSourceID                    = CREATE_DRM_STRING(dwchAttribRestrictedSourceID);
extern const DRM_CONST_STRING g_dstrAttribWMDRMNetRevocationData                = CREATE_DRM_STRING(dwchAttribWMDRMNETRevocationData);
extern const DRM_CONST_STRING g_dstrTagAnalogVideoExtensions                    = CREATE_DRM_STRING(dwchTagAnalogVideoExtensions);
extern const DRM_CONST_STRING g_dstrTagOPLExtensionList                         = CREATE_DRM_STRING(dwchTagOPLExtensionList);
extern const DRM_CONST_STRING g_dstrTagOPLExtension                             = CREATE_DRM_STRING(dwchTagOPLExtension);
extern const DRM_CONST_STRING g_dstrAttribGuid                                  = CREATE_DRM_STRING(dwchAttribGuid);
extern const DRM_CONST_STRING g_dstrAttribValue                                 = CREATE_DRM_STRING(dwchAttribValue);
extern const DRM_CONST_STRING g_dstrAttribLength                                = CREATE_DRM_STRING(dwchAttribLength);
extern const DRM_CONST_STRING g_dstrTagLARevInfo                                = CREATE_DRM_STRING(dwchTagLARevInfo);
extern const DRM_CONST_STRING g_dstrAttribRevInfoID                             = CREATE_DRM_STRING(dwchAttribRevInfoID);
extern const DRM_CONST_STRING g_dstrAttribXMRVersion                            = CREATE_DRM_STRING(dwchAttribXMRVersion);
extern const DRM_CONST_STRING g_dstrTagExPolicy                                 = CREATE_DRM_STRING(dwchTagExPolicy);
extern const DRM_CONST_STRING g_dstrTagExPolicyRightsList                       = CREATE_DRM_STRING(dwchTagExPolicyRightsList);
extern const DRM_CONST_STRING g_dstrTagExPolicyRight                            = CREATE_DRM_STRING(dwchTagExPolicyRight);
extern const DRM_CONST_STRING g_dstrTagExPolicyRestrictionsList                 = CREATE_DRM_STRING(dwchTagExPolicyRestrictionsList);
extern const DRM_CONST_STRING g_dstrTagExPolicyRestriction                      = CREATE_DRM_STRING(dwchTagExPolicyRestriction);
extern const DRM_CONST_STRING g_dstrAttribExPolicyObjectType                    = CREATE_DRM_STRING(dwchAttribExPolicyObjectType);
extern const DRM_CONST_STRING g_dstrAttribExPolicyObjectFlags                   = CREATE_DRM_STRING(dwchAttribExPolicyObjectFlags);
extern const DRM_CONST_STRING g_dstrAttribExPolicySubObjectType                 = CREATE_DRM_STRING(dwchAttribExPolicySubObjectType);
extern const DRM_CONST_STRING g_dstrAttribExPolicySubObjectFlags                = CREATE_DRM_STRING(dwchAttribExPolicyObjectFlags);
extern const DRM_CONST_STRING g_dstrAttribExPolicyPolicyData                    = CREATE_DRM_STRING(dwchAttribExPolicyPolicyData);
extern const DRM_CONST_STRING g_dstrAttribExPolicySecureStateType               = CREATE_DRM_STRING(dwchAttribExPolicySecureStateType);
extern const DRM_CONST_STRING g_dstrAttribExPolicySecureStateData               = CREATE_DRM_STRING(dwchAttribExPolicySecureStateData);
extern const DRM_CONST_STRING g_dstrAttribExPolicySecureStateDate               = CREATE_DRM_STRING(dwchAttribExPolicySecureStateDate);
extern const DRM_CONST_STRING g_dstrAttribExPolicySecureStateCount              = CREATE_DRM_STRING(dwchAttribExPolicySecureStateCount);
extern const DRM_CONST_STRING g_dstrAttribExPolicySecureStateSize               = CREATE_DRM_STRING(dwchAttribExPolicySecureStateSize);
extern const DRM_CONST_STRING g_dstrTagPlayExtendedRestriction                  = CREATE_DRM_STRING(dwchTagPlayExtendedRestriction);
extern const DRM_CONST_STRING g_dstrTagBurnExtendedRestriction                  = CREATE_DRM_STRING(dwchTagBurnExtendedRestriction);
extern const DRM_CONST_STRING g_dstrTagDigitalAudioRestrictions                 = CREATE_DRM_STRING(dwchTagDigitalAudioRestrictions);
extern const DRM_CONST_STRING g_dstrTagPolicyMetadataList                       = CREATE_DRM_STRING(dwchTagPolicyMetadataList);
extern const DRM_CONST_STRING g_dstrTagPolicyMetadata                           = CREATE_DRM_STRING(dwchTagPolicyMetadata);
extern const DRM_CONST_STRING g_dstrAttribCanNotBind                            = CREATE_DRM_STRING(dwchAttribCanNotBind);
extern const DRM_CONST_STRING g_dstrAttribDoNotSign                             = CREATE_DRM_STRING(dwchAttribDoNotSign);
extern const DRM_CONST_STRING g_dstrAttribRSAPublicKey                          = CREATE_DRM_STRING(dwchAttribRSAPublicKey);
extern const DRM_CONST_STRING g_dstrAttribECCPublicKey                          = CREATE_DRM_STRING(dwchAttribECCPublicKey);
extern const DRM_CONST_STRING g_dstrAttribUplinkContentKey                      = CREATE_DRM_STRING(dwchAttribUplinkContentKey);
extern const DRM_CONST_STRING g_dstrAttribUse16ByteCheckSum                     = CREATE_DRM_STRING(dwchAttribUse16ByteCheckSum);
extern const DRM_CONST_STRING g_dstrTagRevInfoForLicenseResponse                = CREATE_DRM_STRING(dwchTagRevInfoForLicenseResponse);
extern const DRM_CONST_STRING g_dstrAttribEmbedLicense                          = CREATE_DRM_STRING(dwchAttribEmbedLicense);
extern const DRM_CONST_STRING g_dstrTagDomainID                                 = CREATE_DRM_STRING(dwchTagDomainID);
extern const DRM_CONST_STRING g_dstrAttribAccountID                             = CREATE_DRM_STRING(dwchAttribAccountID);
extern const DRM_CONST_STRING g_dstrAttribRevision                              = CREATE_DRM_STRING(dwchAttribRevision);
extern const DRM_CONST_STRING g_dstrAttribSerialNumber                          = CREATE_DRM_STRING(dwchAttribSerialNumber);
extern const DRM_CONST_STRING g_dstrTagPlayEnablerGUIDList                      = CREATE_DRM_STRING(dwchTagPlayEnablerGUIDList);
extern const DRM_CONST_STRING g_dstrTagPlayEnablerGUID                          = CREATE_DRM_STRING(dwchTagPlayEnablerGUID);
extern const DRM_CONST_STRING g_dstrTagCopyEnablerGUIDList                      = CREATE_DRM_STRING(dwchTagCopyEnablerGUIDList);
extern const DRM_CONST_STRING g_dstrTagCopyEnablerGUID                          = CREATE_DRM_STRING(dwchTagCopyEnablerGUID);
extern const DRM_CONST_STRING g_dstrAttribCopyCount2                            = CREATE_DRM_STRING(dwchAttribCopyCount2);
extern const DRM_CONST_STRING g_dstrAttribDeviceKey                             = CREATE_DRM_STRING(dwchAttribDeviceKey);
extern const DRM_CONST_STRING g_dstrAttribVerifyExtensibleObject                = CREATE_DRM_STRING(dwchAttribVerifyExtensibleObject);
extern const DRM_CONST_STRING g_dstrAttribExtensibleDepth                       = CREATE_DRM_STRING(dwchAttribExtensibleDepth);
extern const DRM_CONST_STRING g_dstrAttribVerifyPlayRightsExtensibleContainer   = CREATE_DRM_STRING(dwchAttribVerifyPlayRightsExtensibleContainer);
extern const DRM_CONST_STRING g_dstrAttribVerifyPlayRightsExtensibleObject      = CREATE_DRM_STRING(dwchAttribVerifyPlayRightsExtensibleObject);
extern const DRM_CONST_STRING g_dstrAttribMoveEnabler                           = CREATE_DRM_STRING(dwchAttribMoveEnabler);
extern const DRM_CONST_STRING g_dstrAttribTempStoreOnly                         = CREATE_DRM_STRING(dwchAttribTempStoreOnly);
extern const DRM_CONST_STRING g_dstrTagAuxKey                                   = CREATE_DRM_STRING(dwchTagAuxKey);
extern const DRM_CONST_STRING g_dstrTagUplinkX                                  = CREATE_DRM_STRING(dwchTagUplinkX);

DRM_RESULT _GetTestCaseNode(DRM_CONST_STRING *pdstrFile, const DRM_WCHAR *pTestCaseID, DRM_CONST_STRING *pdstrCaseNode)
{
    DRM_RESULT dr = DRM_SUCCESS;
    DRM_CONST_STRING dstrNode = EMPTY_DRM_STRING;
    DRM_CONST_STRING dstrID = EMPTY_DRM_STRING;
    DRM_DWORD index = 0;
    DRM_DWORD lenID = 0;
    int iCmp = 0;

    ChkArg(pdstrFile);
    ChkArg(pTestCaseID);
    ChkArg(pdstrCaseNode);

    /* some processors require WORD-alignment; FAIL if this string is unaligned */
    ChkArg(((DRM_DWORD_PTR) pdstrFile->pwszString) % SIZEOF (DRM_WCHAR) == 0);

    lenID = DRMCRT_wcslen(pTestCaseID);

    do
    {
        ChkDR(DRM_XML_GetNode(pdstrFile, &g_dstrTagLicenseRights, NULL, NULL, index, &dstrNode, NULL));
        dr = DRM_XML_GetNodeAttribute(&dstrNode, &g_dstrAttribTestCaseID, &dstrID);
        if (dr != DRM_SUCCESS || dstrID.cchString > lenID)
        {
            continue;  /* not end of the world error. */
        }

        iCmp = DRMCRT_wcsnicmp(dstrID.pwszString, pTestCaseID, lenID);
        if (iCmp)
        {
            continue;   /* not a match */
        }

        pdstrCaseNode->pwszString = dstrNode.pwszString;
        pdstrCaseNode->cchString = dstrNode.cchString;
        break;

    } while (++index);

ErrorExit:
    if (DRM_FAILED(dr) && pTestCaseID)
    {
        Log("Error ", " Unable to find RightsID node \"%S\"", pTestCaseID);
    }
    return dr;
}


DRM_RESULT _GetRights(DRM_CONST_STRING *dstrNode, LICENSERIGHTS **ppRetRights, DRM_CONST_STRING dstrRIV)
{
    DRM_RESULT dr = DRM_SUCCESS;
    LICENSERIGHTS *pRights = NULL;
    LICENSERIGHTS *pBrother = NULL;
    LICENSERIGHTS *pLeaf = NULL;
    LICENSERIGHTS *pTemp = NULL;
    DRM_CONST_STRING dstrValue = EMPTY_DRM_STRING;
    DRM_CONST_STRING dstrLeaf = EMPTY_DRM_STRING;
    DRM_CONST_STRING dstrBrother = EMPTY_DRM_STRING;
    DRM_CONST_STRING dstrExtensions = EMPTY_DRM_STRING;
    DRM_CONST_STRING dstrDomainID = EMPTY_DRM_STRING;
    DRM_DWORD index = 0;
    DRM_DWORD idx = 0;

    *ppRetRights = NULL;

    pRights = (LICENSERIGHTS *)Oem_MemAlloc(sizeof(LICENSERIGHTS));
    ChkMem(pRights);
    memset(pRights, 0, sizeof(LICENSERIGHTS));

    ChkDR(DRM_XML_GetNodeAttribute(dstrNode, &g_dstrAttribType, &dstrValue));
    ChkDR(CopyDRMString(&pRights->LicenseType, &dstrValue));

    dr = DRM_XML_GetNodeAttribute(dstrNode, &g_dstrAttribAllowPlay, &dstrValue);
    if (dr == DRM_SUCCESS)
        ChkDR(CopyDRMString(&pRights->AllowPlay, &dstrValue));

    dr = DRM_XML_GetNodeAttribute(dstrNode, &g_dstrAttribPlayCount, &dstrValue);
    if (dr == DRM_SUCCESS)
        ChkDR(CopyDRMString(&pRights->PlayCount, &dstrValue));

    dr = DRM_XML_GetNodeAttribute(dstrNode, &g_dstrAttribAllowCopy, &dstrValue);
    if (dr == DRM_SUCCESS)
        ChkDR(CopyDRMString(&pRights->AllowCopy, &dstrValue));

    dr = DRM_XML_GetNodeAttribute(dstrNode, &g_dstrAttribCopyCount, &dstrValue);
    if (dr == DRM_SUCCESS)
        ChkDR(CopyDRMString(&pRights->CopyCount, &dstrValue));

    dr = DRM_XML_GetNodeAttribute(dstrNode, &g_dstrAttribCopyCount2, &dstrValue);
    if (dr == DRM_SUCCESS)
        ChkDR(CopyDRMString(&pRights->CopyCount2, &dstrValue));

    dr = DRM_XML_GetNodeAttribute(dstrNode, &g_dstrAttribAllowTransferToSDMI, &dstrValue);
    if (dr == DRM_SUCCESS)
        ChkDR(CopyDRMString(&pRights->AllowTransferToSDMI, &dstrValue));

    dr = DRM_XML_GetNodeAttribute(dstrNode, &g_dstrAttribAllowTransferToNonSDMI, &dstrValue);
    if (dr == DRM_SUCCESS)
        ChkDR(CopyDRMString(&pRights->AllowTransferToNonSDMI, &dstrValue));

    dr = DRM_XML_GetNodeAttribute(dstrNode, &g_dstrAttribTransferCount, &dstrValue);
    if (dr == DRM_SUCCESS)
        ChkDR(CopyDRMString(&pRights->TransferCount, &dstrValue));

    dr = DRM_XML_GetNodeAttribute(dstrNode, &g_dstrAttribCopyProtectionLevel, &dstrValue);
    if (dr == DRM_SUCCESS)
        ChkDR(CopyDRMString(&pRights->CopyProtectionLevel, &dstrValue));

    dr = DRM_XML_GetNodeAttribute(dstrNode, &g_dstrAttribAnalogVideo, &dstrValue);
    if (dr == DRM_SUCCESS)
        ChkDR(CopyDRMString(&pRights->AnalogVideo, &dstrValue));

    dr = DRM_XML_GetNodeAttribute(dstrNode, &g_dstrAttribUnCompressedDigitalVideo, &dstrValue);
    if (dr == DRM_SUCCESS)
        ChkDR(CopyDRMString(&pRights->UnCompressedDigitalVideo, &dstrValue));

    dr = DRM_XML_GetNodeAttribute(dstrNode, &g_dstrAttribUnCompressedDigitalAudio, &dstrValue);
    if (dr == DRM_SUCCESS)
        ChkDR(CopyDRMString(&pRights->UnCompressedDigitalAudio, &dstrValue));

    dr = DRM_XML_GetNodeAttribute(dstrNode, &g_dstrAttribCompressedDigitalVideo, &dstrValue);
    if (dr == DRM_SUCCESS)
        ChkDR(CopyDRMString(&pRights->CompressedDigitalVideo, &dstrValue));

    dr = DRM_XML_GetNodeAttribute(dstrNode, &g_dstrAttribCompressedDigitalAudio, &dstrValue);
    if (dr == DRM_SUCCESS)
        ChkDR(CopyDRMString(&pRights->CompressedDigitalAudio, &dstrValue));

    dr = DRM_XML_GetNodeAttribute(dstrNode, &g_dstrAttribAllowBackUpRestore, &dstrValue);
    if (dr == DRM_SUCCESS)
        ChkDR(CopyDRMString(&pRights->AllowBackUpRestore, &dstrValue));

    dr = DRM_XML_GetNodeAttribute(dstrNode, &g_dstrAttribAllowBurnToCD, &dstrValue);
    if (dr == DRM_SUCCESS)
        ChkDR(CopyDRMString(&pRights->AllowBurnToCD, &dstrValue));

    dr = DRM_XML_GetNodeAttribute(dstrNode, &g_dstrAttribBurnToCDCount, &dstrValue);
    if (dr == DRM_SUCCESS)
        ChkDR(CopyDRMString(&pRights->BurnToCDCount, &dstrValue));

    dr = DRM_XML_GetNodeAttribute(dstrNode, &g_dstrAttribAllowPlaylistBurn, &dstrValue);
    if (dr == DRM_SUCCESS)
        ChkDR(CopyDRMString(&pRights->AllowPlaylistBurn, &dstrValue));

    dr = DRM_XML_GetNodeAttribute(dstrNode, &g_dstrAttribPlaylistBurnTrackCount, &dstrValue);
    if (dr == DRM_SUCCESS)
        ChkDR(CopyDRMString(&pRights->PlaylistBurnTrackCount, &dstrValue));

    dr = DRM_XML_GetNodeAttribute(dstrNode, &g_dstrAttribMaxPlaylistBurnCount, &dstrValue);
    if (dr == DRM_SUCCESS)
        ChkDR(CopyDRMString(&pRights->MaxPlaylistBurnCount, &dstrValue));

    dr = DRM_XML_GetNodeAttribute(dstrNode, &g_dstrAttribAllowCollaborativePlay, &dstrValue);
    if (dr == DRM_SUCCESS)
        ChkDR(CopyDRMString(&pRights->AllowCollaborativePlay, &dstrValue));

    dr = DRM_XML_GetNodeAttribute(dstrNode, &g_dstrAttribBeginDate, &dstrValue);
    if (dr == DRM_SUCCESS)
        ChkDR(CopyDRMString(&pRights->BeginDate, &dstrValue));

    dr = DRM_XML_GetNodeAttribute(dstrNode, &g_dstrAttribExpirationDate, &dstrValue);
    if (dr == DRM_SUCCESS)
        ChkDR(CopyDRMString(&pRights->ExpirationDate, &dstrValue));

    dr = DRM_XML_GetNodeAttribute(dstrNode, &g_dstrAttribExpirationAfterFirstUse, &dstrValue);
    if (dr == DRM_SUCCESS)
        ChkDR(CopyDRMString(&pRights->ExpirationAfterFirstUse, &dstrValue));

    dr = DRM_XML_GetNodeAttribute(dstrNode, &g_dstrAttribExpirationAfterFirstPlay, &dstrValue);
    if (dr == DRM_SUCCESS)
        ChkDR(CopyDRMString(&pRights->ExpirationAfterFirstPlay, &dstrValue));


    dr = DRM_XML_GetNodeAttribute(dstrNode, &g_dstrAttribExpirationOnStore, &dstrValue);
    if (dr == DRM_SUCCESS)
        ChkDR(CopyDRMString(&pRights->ExpirationOnStore, &dstrValue));

    dr = DRM_XML_GetNodeAttribute(dstrNode, &g_dstrAttribGracePeriod, &dstrValue);
    if (dr == DRM_SUCCESS)
        ChkDR(CopyDRMString(&pRights->GracePeriod, &dstrValue));

    dr = DRM_XML_GetNodeAttribute(dstrNode, &g_dstrAttribDeleteOnClockRollback, &dstrValue);
    if (dr == DRM_SUCCESS)
        ChkDR(CopyDRMString(&pRights->DeleteOnClockRollback, &dstrValue));

    dr = DRM_XML_GetNodeAttribute(dstrNode, &g_dstrAttribDisableOnClockRollback, &dstrValue);
    if (dr == DRM_SUCCESS)
        ChkDR(CopyDRMString(&pRights->DisableOnClockRollback, &dstrValue));

    dr = DRM_XML_GetNodeAttribute(dstrNode, &g_dstrAttribMinimumSecurityLevel, &dstrValue);
    if (dr == DRM_SUCCESS)
        ChkDR(CopyDRMString(&pRights->MinimumSecurityLevel, &dstrValue));

    dr = DRM_XML_GetNodeAttribute(dstrNode, &g_dstrAttribMinimumAppCrlVersion, &dstrValue);
    if (dr == DRM_SUCCESS)
        ChkDR(CopyDRMString(&pRights->MinimumAppCrlVersion, &dstrValue));

    dr = DRM_XML_GetNodeAttribute(dstrNode, &g_dstrAttribMinimumDeviceCrlVersion, &dstrValue);
    if (dr == DRM_SUCCESS)
        ChkDR(CopyDRMString(&pRights->MinimumDeviceCrlVersion, &dstrValue));

    dr = DRM_XML_GetNodeAttribute(dstrNode, &g_dstrAttribMinimumClientSDKSecurity, &dstrValue);
    if (dr == DRM_SUCCESS)
        ChkDR(CopyDRMString(&pRights->MinimumClientSDKSecurity, &dstrValue));

    dr = DRM_XML_GetNodeAttribute(dstrNode, &g_dstrAttribPMRights, &dstrValue);
    if (dr == DRM_SUCCESS)
        ChkDR(CopyDRMString(&pRights->PMRights, &dstrValue));

    dr = DRM_XML_GetNodeAttribute(dstrNode, &g_dstrAttribPMAppSecurity, &dstrValue);
    if (dr == DRM_SUCCESS)
        ChkDR(CopyDRMString(&pRights->PMAppSecurity, &dstrValue));

    dr = DRM_XML_GetNodeAttribute(dstrNode, &g_dstrAttribPMExpirationDate, &dstrValue);
    if (dr == DRM_SUCCESS)
        ChkDR(CopyDRMString(&pRights->PMExpirationDate, &dstrValue));

    dr = DRM_XML_GetNodeAttribute(dstrNode, &g_dstrAttribAllowSaveStreamProtected, &dstrValue);
    if (dr == DRM_SUCCESS)
        ChkDR(CopyDRMString(&pRights->AllowSaveStreamProtected, &dstrValue));

    dr = DRM_XML_GetNodeAttribute(dstrNode, &g_dstrAttribWMDRMNetRevocationData, &dstrValue);
    if ( DRM_SUCCESS == dr)
    {
        ChkDR(CopyDRMString(&pRights->WMDRMNETRevocationData, &dstrValue));
    }

    dr = DRM_XML_GetNodeAttribute(dstrNode, &g_dstrAttribKID, &dstrValue);
    if (dr == DRM_SUCCESS)
    {
        ChkDR(CopyDRMString(&pRights->KID, &dstrValue));
    }

    dr = DRM_XML_GetNodeAttribute(dstrNode, &g_dstrAttribPriority, &dstrValue);
    if (dr == DRM_SUCCESS)
    {
        ChkDR(CopyDRMString(&pRights->Priority, &dstrValue));
    }

    dr = DRM_XML_GetNodeAttribute(dstrNode, &g_dstrAttribSerialNumber, &dstrValue);
    if (dr == DRM_SUCCESS)
    {
        ChkDR(CopyDRMString(&pRights->SerialNumber, &dstrValue));
    }

    /*********************************************************************************
    // The logic here for setting RIV is as following:
    // if user explicitly specifies a RIV value inside the license, then we use this
    //   value regardless of what the suggested RIV value passed in. This gives us the
    //   ability to immitate the case that server pass down a mismatch REVINFO and RIV
    //   in license.
    // if user does not specifies anything about RIV, and if suggested RVI value is not
    //   empty, we use suggested value
    // otherwise, the RIV value will be empty
    **********************************************************************************/
    dr = DRM_XML_GetNodeAttribute(dstrNode, &g_dstrAttribRevInfoVersion, &dstrValue);
    if (dr == DRM_SUCCESS)
    {
    ChkDR(CopyDRMString(&pRights->RevInfoVersion, &dstrValue));
    }
    else if ( dstrRIV.cchString > 0 )
    {
        ChkDR(CopyDRMString(&pRights->RevInfoVersion, &dstrRIV));
    }

    dr = DRM_XML_GetNodeAttribute(dstrNode, &g_dstrAttribIndivVersion, &dstrValue);
    if (dr == DRM_SUCCESS)
        ChkDR(CopyDRMString(&pRights->IndivVersion, &dstrValue));

    dr = DRM_XML_GetNodeAttribute(dstrNode, &g_dstrAttribLID, &dstrValue);
    if (dr == DRM_SUCCESS)
    {
        ChkDR(CopyDRMString(&pRights->LID, &dstrValue));
    }

    dr = DRM_XML_GetNodeAttribute(dstrNode, &g_dstrAttribRevocationList, &dstrValue);
    if (dr == DRM_SUCCESS)
        ChkDR(CopyDRMString(&pRights->RevocationList, &dstrValue));

    dr = DRM_XML_GetNodeAttribute(dstrNode, &g_dstrAttribContentPubKey, &dstrValue);
    if (dr == DRM_SUCCESS)
        ChkDR(CopyDRMString(&pRights->ContentPubKey, &dstrValue));

    dr = DRM_XML_GetNodeAttribute(dstrNode, &g_dstrAttribIssueDate, &dstrValue);
    if (dr == DRM_SUCCESS)
        ChkDR(CopyDRMString(&pRights->IssueDate, &dstrValue));

    dr = DRM_XML_GetNodeAttribute(dstrNode, &g_dstrAttribRemovalDate, &dstrValue);
    if (dr == DRM_SUCCESS)
        ChkDR(CopyDRMString(&pRights->RemovalDate, &dstrValue));

    dr = DRM_XML_GetNodeAttribute(dstrNode, &g_dstrAttribServerPrivateKey, &dstrValue);
    if (dr == DRM_SUCCESS)
        ChkDR(CopyDRMString(&pRights->ServerPrivateKey, &dstrValue));

    dr = DRM_XML_GetNodeAttribute(dstrNode, &g_dstrAttribServerPublicKey, &dstrValue);
    if (dr == DRM_SUCCESS)
        ChkDR(CopyDRMString(&pRights->ServerPublicKey, &dstrValue));

    dr = DRM_XML_GetNodeAttribute(dstrNode, &g_dstrAttribServerSeed, &dstrValue);
    if (dr == DRM_SUCCESS)
        ChkDR(CopyDRMString(&pRights->ServerSeed, &dstrValue));

    dr = DRM_XML_GetNodeAttribute(dstrNode, &g_dstrAttribWMDRMNetRevocationIndex, &dstrValue);
    if (dr == DRM_SUCCESS)
        ChkDR(CopyDRMString(&pRights->WMDRMNETRevocationIndex, &dstrValue));

    dr = DRM_XML_GetNodeAttribute(dstrNode, &g_dstrAttribDeviceRevocationIndex, &dstrValue);
    if (dr == DRM_SUCCESS)
        ChkDR(CopyDRMString(&pRights->DeviceRevocationIndex, &dstrValue));

    dr = DRM_XML_GetNodeAttribute(dstrNode, &g_dstrAttribTestOptions, &dstrValue);
    if (dr == DRM_SUCCESS)
        ChkDR(CopyDRMString(&pRights->TestOptions, &dstrValue));

    dr = DRM_XML_GetNodeAttribute(dstrNode, &g_dstrAttribExcludeApps, &dstrValue);
    if (dr == DRM_SUCCESS)
        ChkDR(CopyDRMString(&pRights->ExcludeApps, &dstrValue));

    dr = DRM_XML_GetNodeAttribute(dstrNode, &g_dstrAttribDescription, &dstrValue);
    if (dr == DRM_SUCCESS)
        ChkDR(CopyDRMString(&pRights->Description, &dstrValue));

    dr = DRM_XML_GetNodeAttribute(dstrNode, &g_dstrAttribSAPMode, &dstrValue);
    if (dr == DRM_SUCCESS)
    {
        ChkDR(CopyDRMString(&pRights->SAPMode, &dstrValue));
    }

    dr = DRM_XML_GetNodeAttribute(dstrNode, &g_dstrAttribUplinkKIDs, &dstrValue);
    if (dr == DRM_SUCCESS)
        ChkDR(CopyDRMString(&pRights->UplinkKIDs, &dstrValue));

    dr = DRM_XML_GetNodeAttribute(dstrNode, &g_dstrAttribLGPUBKEY, &dstrValue);
    if (dr == DRM_SUCCESS)
        ChkDR(CopyDRMString(&pRights->LGPUBKEY, &dstrValue));

    dr = DRM_XML_GetNodeAttribute(dstrNode, &g_dstrAttribUID, &dstrValue);
    if (dr == DRM_SUCCESS)
        ChkDR(CopyDRMString(&pRights->UID, &dstrValue));

    dr = DRM_XML_GetNodeAttribute(dstrNode, &g_dstrAttribAttributeList, &dstrValue);
    if (dr == DRM_SUCCESS)
        ChkDR(CopyDRMString(&pRights->AttributeList, &dstrValue));

    dr = DRM_XML_GetNodeAttribute(dstrNode, &g_dstrAttribUseAESContentKey, &dstrValue);
    if (dr == DRM_SUCCESS)
        ChkDR(CopyDRMString(&pRights->UseAESContentKey, &dstrValue));

    dr = DRM_XML_GetNodeAttribute(dstrNode, &g_dstrAttribUseECCPubKey, &dstrValue);
    if (dr == DRM_SUCCESS)
        ChkDR(CopyDRMString(&pRights->UseECCPubKey, &dstrValue));

    dr = DRM_XML_GetSubNode(dstrNode, &g_dstrTagMETA, NULL, NULL, 0, &dstrValue, NULL,1);
    if (dr == DRM_SUCCESS)
        ChkDR(CopyDRMString(&pRights->Meta, &dstrValue));

    dr = DRM_XML_GetNodeAttribute(dstrNode, &g_dstrAttribMeteringID, &dstrValue);
    if (dr == DRM_SUCCESS)
    {
        ChkDR(CopyDRMString(&pRights->MeteringID, &dstrValue));
    }

    dr = DRM_XML_GetSubNode(dstrNode, &g_dstrTagMETERCERT, NULL, NULL, 0, &dstrValue, NULL,1);
    if (dr == DRM_SUCCESS)
    {
        ChkDR(CopyDRMString(&pRights->MeteringCert, &dstrValue));
    }

    dr = DRM_XML_GetNodeAttribute(dstrNode, &g_dstrAttribSourceID, &dstrValue);
    if (dr == DRM_SUCCESS)
    {
        ChkDR(CopyDRMString(&pRights->SourceID, &dstrValue));
    }

    dr = DRM_XML_GetNodeAttribute(dstrNode, &g_dstrAttribUnCompressedDigitalAudioLevels, &dstrValue);
    if (dr == DRM_SUCCESS)
    {
        ChkDR(CopyDRMString(&pRights->UnCompressedDigitalAudioLevels, &dstrValue));
    }

    dr = DRM_XML_GetNodeAttribute(dstrNode, &g_dstrAttribCert1, &dstrValue);
    if (dr == DRM_SUCCESS)
    {
        ChkDR(CopyDRMString(&pRights->Cert1, &dstrValue));
    }

    dr = DRM_XML_GetNodeAttribute(dstrNode, &g_dstrAttribCert2, &dstrValue);
    if (dr == DRM_SUCCESS)
    {
        ChkDR(CopyDRMString(&pRights->Cert2, &dstrValue));
    }

    dr = DRM_XML_GetNodeAttribute(dstrNode, &g_dstrAttribContentKey, &dstrValue);
    if (dr == DRM_SUCCESS)
    {
        ChkDR(CopyDRMString(&pRights->ContentKey, &dstrValue));

    }

    dr = DRM_XML_GetNodeAttribute(dstrNode, &g_dstrAttribInclusionList, &dstrValue);
    if (dr == DRM_SUCCESS)
    {
        ChkDR(CopyDRMString(&pRights->InclusionList, &dstrValue));
    }

    dr = DRM_XML_GetNodeAttribute(dstrNode, &g_dstrAttribCannotPersist, &dstrValue);
    if (dr == DRM_SUCCESS)
    {
        ChkDR(CopyDRMString(&pRights->CannotPersist, &dstrValue));
    }

    dr = DRM_XML_GetNodeAttribute(dstrNode, &g_dstrAttribRestrictedSourceID, &dstrValue);
    if (dr == DRM_SUCCESS)
    {
        ChkDR(CopyDRMString(&pRights->RestrictedSourceID, &dstrValue));
    }

    dr = DRM_XML_GetNodeAttribute(dstrNode, &g_dstrAttribXMRVersion, &dstrValue);
    if (dr == DRM_SUCCESS)
    {
        ChkDR(CopyDRMString(&pRights->XMRVersion, &dstrValue));
    }

    dr = DRM_XML_GetNodeAttribute(dstrNode, &g_dstrAttribCanNotBind, &dstrValue);
    if (dr == DRM_SUCCESS)
    {
        ChkDR(CopyDRMString(&pRights->CanNotBind, &dstrValue));
    }

    dr = DRM_XML_GetNodeAttribute(dstrNode, &g_dstrAttribDoNotSign, &dstrValue);
    if (dr == DRM_SUCCESS)
    {
        ChkDR(CopyDRMString(&pRights->DoNotSign, &dstrValue));
    }

    dr = DRM_XML_GetNodeAttribute(dstrNode, &g_dstrAttribRSAPublicKey, &dstrValue);
    if (dr == DRM_SUCCESS)
    {
        ChkDR(CopyDRMString(&pRights->RSAPublicKey, &dstrValue));
    }

    dr = DRM_XML_GetNodeAttribute(dstrNode, &g_dstrAttribECCPublicKey, &dstrValue);
    if (dr == DRM_SUCCESS)
    {
        ChkDR(CopyDRMString(&pRights->ECCPublicKey, &dstrValue));
    }

    dr = DRM_XML_GetNodeAttribute(dstrNode, &g_dstrAttribUplinkContentKey, &dstrValue);
    if (dr == DRM_SUCCESS)
    {
        ChkDR(CopyDRMString(&pRights->UplinkContentKey, &dstrValue));
    }

    dr = DRM_XML_GetNodeAttribute(dstrNode, &g_dstrAttribUse16ByteCheckSum, &dstrValue);
    if (dr == DRM_SUCCESS)
    {
        ChkDR(CopyDRMString(&pRights->Use16ByteCheckSum, &dstrValue));
    }

    dr = DRM_XML_GetNodeAttribute(dstrNode, &g_dstrAttribDeviceKey, &dstrValue);
    if (dr == DRM_SUCCESS)
    {
        ChkDR(CopyDRMString(&pRights->DeviceKey, &dstrValue));
    }

    dr = DRM_XML_GetNodeAttribute(dstrNode, &g_dstrAttribVerifyExtensibleObject, &dstrValue);
    if (dr == DRM_SUCCESS)
    {
        ChkDR(CopyDRMString(&pRights->VerifyExtensibleObject, &dstrValue));
    }

    dr = DRM_XML_GetNodeAttribute(dstrNode, &g_dstrAttribExtensibleDepth, &dstrValue);
    if (dr == DRM_SUCCESS)
    {
        ChkDR(CopyDRMString(&pRights->ExtensibleDepth, &dstrValue));
    }

    dr = DRM_XML_GetNodeAttribute(dstrNode, &g_dstrAttribVerifyPlayRightsExtensibleContainer, &dstrValue);
    if (dr == DRM_SUCCESS)
    {
        ChkDR(CopyDRMString(&pRights->VerifyPlayRightsExtensibleContainer, &dstrValue));
    }

    dr = DRM_XML_GetNodeAttribute(dstrNode, &g_dstrAttribVerifyPlayRightsExtensibleObject, &dstrValue);
    if (dr == DRM_SUCCESS)
    {
        ChkDR(CopyDRMString(&pRights->VerifyPlayRightsExtensibleObject, &dstrValue));
    }

    dr = DRM_XML_GetNodeAttribute(dstrNode, &g_dstrAttribEmbedLicense, &dstrValue);
    if (dr == DRM_SUCCESS)
    {
        ChkDR(CopyDRMString(&pRights->EmbedLicense, &dstrValue));
    }

    dr = DRM_XML_GetNodeAttribute(dstrNode, &g_dstrAttribMoveEnabler, &dstrValue);
    if (dr == DRM_SUCCESS)
    {
        ChkDR(CopyDRMString(&pRights->MoveEnabler, &dstrValue));
    }

    dr = DRM_XML_GetNodeAttribute(dstrNode, &g_dstrAttribTempStoreOnly, &dstrValue);
    if (dr == DRM_SUCCESS)
    {
        ChkDR(CopyDRMString(&pRights->TempStoreOnly, &dstrValue));
    }

    for( idx = 0; idx < g_cdwExtensiblePolicyMapMax; idx++ )
    {
        dr = DRM_XML_GetNodeAttribute(dstrNode, &g_XMRLicGenExtensiblePolicyMap[idx].dstrAllowExtensibleRight, &dstrValue);
        if (dr == DRM_SUCCESS)
        {
            ChkDR(CopyDRMString(&pRights->AllowExtensibleRights[idx], &dstrValue));
        }
    }

    /************************************************************************************************
    //    DomainID should be formatted as a child node of LicenseRights as following:
    //     <DomainID AccountID="{guid}", Revision=""/>
    *************************************************************************************************/
    dr = DRM_XML_GetSubNode( dstrNode,
                             &g_dstrTagDomainID,
                             NULL,
                             NULL,
                             0,
                             &dstrDomainID,
                             NULL,
                             1 );
    if (DRM_SUCCESS == dr )
    {
        ChkDR( CopyDRMString( &pRights->DomainID, &dstrDomainID ) );

        /* Get the AccountID attribute if it exists */
        dr = DRM_XML_GetNodeAttribute( &dstrDomainID, &g_dstrAttribAccountID, &pRights->DomainAccountID );
        if ( dr != DRM_SUCCESS && dr != DRM_E_INVALIDXMLTAG )
        {
            ChkDR( dr );
        }

        /* Get the Revision attribute if it exists */
        dr = DRM_XML_GetNodeAttribute( &dstrDomainID, &g_dstrAttribRevision, &pRights->DomainRevision );
        if ( dr != DRM_SUCCESS && dr != DRM_E_INVALIDXMLTAG )
        {
            ChkDR( dr );
        }
    }

    /************************************************************************************************
    //    Analog Video OPL Extensions should be formatted as a child node of LicenseRights as following:
    //     <EXTENSIONLIST><EXTENSION guid="XXXXX", value="XXX" /></EXTENSIONLIST>
    //    NOTE:
    //      1).It is user's responsibility to ensure that EXTENSIONLIST is well formated, the Node
    //         name must be in capital case and the attribute name must be in lower case.
    //      2).Both guid and value attribute for EXTENSION is mandatory, it is user's responsibility
    //         to make sure these two attributes are there, you should use 0 as default for value
    *************************************************************************************************/
    dr = DRM_XML_GetSubNode( dstrNode,
                             &g_dstrTagAnalogVideoExtensions,
                             NULL,
                             NULL,
                             0,
                             NULL,
                             &dstrExtensions,
                             1 );
    if (DRM_SUCCESS == dr )
    {
        ChkDR(CopyDRMString(&pRights->AnalogVideoExtensions, &dstrExtensions));
    }

    /************************************************************************************************
    //    Digital Audio Extensions should be formatted as a child node of LicenseRights as following:
    //     <EXTENSIONLIST><EXTENSION guid="XXXXX", value="XXX" /></EXTENSIONLIST>
    //    NOTE:
    //      1).It is user's responsibility to ensure that EXTENSIONLIST is well formated, the Node
    //         name must be in capital case and the attribute name must be in lower case.
    //      2).Both guid and value attribute for EXTENSION is mandatory, it is user's responsibility
    //         to make sure these two attributes are there, you should use 0 as default for value
    *************************************************************************************************/
    dr = DRM_XML_GetSubNode( dstrNode,
                             &g_dstrTagDigitalAudioRestrictions,
                             NULL,
                             NULL,
                             0,
                             NULL,
                             &dstrExtensions,
                             1 );
    if (DRM_SUCCESS == dr )
    {
        ChkDR(CopyDRMString(&pRights->DigitalAudioRestrictions, &dstrExtensions));
    }

    /************************************************************************************************
    //    PolicyMetadata should be formatted as a child node of LicenseRights as following:
    //     <PolicyMetadataList><PolicyMetaData guid="XXXXX", value="XXX", Type="XXXX" /></PolicyMetadataList>
    //    NOTE:
    //      1).It is user's responsibility to ensure that PolicyMetadataList is well formated,
    //      2).The guid and value attributes for PolicyMetaData are mandatory, it is user's responsibility
    //         to make sure these two attributes are there, you should use 0 as default for value
    //      3).The type attribute denotes what sort of data type value is.
    //         Options are: DWORD, WCHAR, BINARY (b64 encoded binary).
    *************************************************************************************************/
    dr = DRM_XML_GetSubNode( dstrNode,
                             &g_dstrTagPolicyMetadataList,
                             NULL,
                             NULL,
                             0,
                             NULL,
                             &dstrValue,
                             1 );
    if (DRM_SUCCESS == dr )
    {
        ChkDR(CopyDRMString(&pRights->PolicyMetadataList, &dstrValue));
    }

    /************************************************************************************************
    //    PlayEnablerGUID should be formatted as a child node of LicenseRights as following:
    //     <PlayEnablerGUIDList><PlayEnablerGUID guid="XXXXX"/></PlayEnablerGUIDList>
    //    NOTE:
    //      1).It is user's responsibility to ensure that PlayEnablerGUIDList is well formated,
    //      2).The guid for PlayEnablerGUID is mandatory, it is user's responsibility
    //         to make sure the attribute is there
    *************************************************************************************************/
    dr = DRM_XML_GetSubNode( dstrNode,
                             &g_dstrTagPlayEnablerGUIDList,
                             NULL,
                             NULL,
                             0,
                             NULL,
                             &dstrValue,
                             1 );
    if (DRM_SUCCESS == dr )
    {
        ChkDR(CopyDRMString(&pRights->PlayEnablerGUIDList, &dstrValue));
    }


    /************************************************************************************************
    //    CopyEnablerGUID should be formatted as a child node of LicenseRights as following:
    //     <CopyEnablerGUIDList><CopyEnablerGUID guid="XXXXX"/></CopyEnablerGUIDList>
    //    NOTE:
    //      1).It is user's responsibility to ensure that CopyEnablerGUIDList is well formated,
    //      2).The guid for CopyEnablerGUID is mandatory, it is user's responsibility
    //         to make sure the attribute is there
    *************************************************************************************************/
    dr = DRM_XML_GetSubNode( dstrNode,
                             &g_dstrTagCopyEnablerGUIDList,
                             NULL,
                             NULL,
                             0,
                             NULL,
                             &dstrValue,
                             1 );
    if (DRM_SUCCESS == dr )
    {
        ChkDR(CopyDRMString(&pRights->CopyEnablerGUIDList, &dstrValue));
    }


    /************************************************************************************************
    //    Extensible Policy should be formatted as the child node of LicenseRights as follows:
    *************************************************************************************************


    *************************************************************************************************/
    dr = DRM_XML_GetSubNode( dstrNode,
                             &g_dstrTagExPolicy,
                             NULL,
                             NULL,
                             0,
                             NULL,
                             &dstrValue,
                             1 );

    if (DRM_SUCCESS == dr )
    {
        ChkDR(CopyDRMString(&pRights->XPolicy, &dstrValue));
    }

    /***************************************************************************************************
        Extensible Policy restrictions for existing Play Right should be formatted as the child node of LicenseRights as follows:
    *************************************************************************************************
    <XPolicyPlayExtended>
    <XPolicyRestrictionsList>
    <XPolicyRestriction XPolicyObjectType="1004" XPolicyObjectFlags ="14" XPolicyPolicyData="22" XPolicySecureStateType="02" XPolicySecureStateDate="33" XPolicySecureStateCount=" " XPolicySecureStateData=" " XPolicySecureStateSize =" "/>
    <XPolicyRestriction XPolicyObjectType="1005" XPolicyObjectFlags ="14" XPolicyPolicyData="22" XPolicySecureStateType="02" XPolicySecureStateDate="33" XPolicySecureStateCount=" " XPolicySecureStateData=" " XPolicySecureStateSize =" "/>
    </XPolicyRestrictionsList>
    </XPolicyPlayExtended>
    *************************************************************************************************/
     dr = DRM_XML_GetSubNode( dstrNode,
                              &g_dstrTagPlayExtendedRestriction,
                              NULL,
                              NULL,
                              0,
                              NULL,
                              &dstrValue,
                              1 );

    if (DRM_SUCCESS == dr )
    {
        ChkDR(CopyDRMString(&pRights->XPolicyPlayExtended, &dstrValue));
    }

    /***************************************************************************************************
        Extensible Policy restrictions for existing Play Right should be formatted as the child node of LicenseRights as follows:
    *************************************************************************************************
    <XPolicyRestrictionsList>
    <XPolicyRestriction XPolicyObjectType="1006" XPolicyObjectFlags ="0014" XPolicyPolicyData="22" XPolicySecureStateType="02" XPolicySecureStateDate="33" XPolicySecureStateCount=" " XPolicySecureStateData=" " XPolicySecureStateSize =" "/>
    <XPolicyRestriction XPolicyObjectType="1007" XPolicyObjectFlags ="0014" XPolicyPolicyData="22" XPolicySecureStateType="02" XPolicySecureStateDate="33" XPolicySecureStateCount=" " XPolicySecureStateData=" " XPolicySecureStateSize =" "/>
    </XPolicyRestrictionsList>
    </XPolicyBurnExtended>
    *************************************************************************************************/
    dr = DRM_XML_GetSubNode( dstrNode,
                             &g_dstrTagBurnExtendedRestriction,
                             NULL,
                             NULL,
                             0,
                             NULL,
                             &dstrValue,
                             1 );

    if (DRM_SUCCESS == dr )
    {
        ChkDR(CopyDRMString(&pRights->XPolicyBurnExtended, &dstrValue));
    }

    /***************************************************************************************************
        Revocation information should be formatted as the child node of LicenseRights as follows:
    *************************************************************************************************
    <RevInfo>
     <Revocation>
      <ListID>{B64-encoded GUID}</ListID>
      <ListData>{B64-encoded list data}</ListData>
     </Revocation>
     <Revocation>
      <ListID>{B64-encoded GUID}</ListID>
      <ListData>{B64-encoded list data}</ListData>
     </Revocation>
    </RevInfo>

    *************************************************************************************************/
    dr = DRM_XML_GetSubNode( dstrNode,
                             &g_dstrTagRevInfoForLicenseResponse,
                             NULL,
                             NULL,
                             0,
                             NULL,
                             &dstrValue,
                             1 );

    if (DRM_SUCCESS == dr )
    {
        ChkDR(CopyDRMString(&pRights->RevInfoForLicenseResponse, &dstrValue));
    }


    /*
    The format of the AuxKey node is:
        <AuxKey number_of_entries="number of entries" key="the b64 encoded base key used to derive the key in an AUX_KEY_ENTRY node">
          <AUX_KEY_ENTRY index="index into the array of elements" location="the key's location value as decimal or hex" key="b64 encoded key" />
        …
        </AuxKey>
    */
    dr = DRM_XML_GetSubNode( dstrNode, &g_dstrTagAuxKey, NULL, NULL, 0, &dstrValue, NULL, 1 );
    if (DRM_SUCCESS == dr )
    {
        ChkDR(CopyDRMString(&pRights->AuxKey, &dstrValue));
    }

    /*
    The format of the UplinkX node is:
        <UplinkX number_of_entries="number of entries" key="the b64 encoded base key used to derive the key in an AUX_KEY_ENTRY node">
          <AUX_KEY_ENTRY index="index into the array of elements" location="the key's location value as decimal or hex" key="b64 encoded key" />
        …
        </UplinkX>
    */
    dr = DRM_XML_GetSubNode( dstrNode, &g_dstrTagUplinkX, NULL, NULL, 0, &dstrValue, NULL, 1 );
    if (DRM_SUCCESS == dr )
    {
        ChkDR(CopyDRMString(&pRights->UplinkX, &dstrValue));
    }

    /* now scan for leaf nodes. */
    do
    {
        dr = DRM_XML_GetSubNode(dstrNode, &g_dstrTagLeaf, NULL, NULL, index, &dstrLeaf, NULL,1);
        if (dr != DRM_SUCCESS)
        {
            dr = DRM_SUCCESS;   /* not fatal error.  Probably did not find a leaf. */
            break;
        }
        ChkDR(_GetRights(&dstrLeaf, &pLeaf, dstrRIV));
        pLeaf->pNext = pRights->pLeaf;      /* by setting pNext we allow a leaf to also be a root. */
        pRights->pLeaf = pLeaf;

        /* Leaf node and Root node need to be consistent about the Assymmetric Encryption algorithm they are using */
        FREEDRMSTRING(pLeaf->UseECCPubKey);
        CopyDRMString(&pLeaf->UseECCPubKey, &pRights->UseECCPubKey);

    } while (++index);

    /* now scan for same level nodes. */
    index = 0;
    do
    {
        dr = DRM_XML_GetSubNode(dstrNode, &g_dstrTagLicenseRights, NULL, NULL, index, &dstrBrother, NULL,1);
        if (dr != DRM_SUCCESS)
        {
            dr = DRM_SUCCESS;   /* not fatal error.  Probably did not find a leaf. */
            break;
        }

        ChkDR(_GetRights(&dstrBrother, &pBrother, dstrRIV));
        pTemp = pRights;
        while (pTemp->pNext)
            pTemp = pTemp->pNext;
        pTemp->pNext = pBrother;
    } while(++index);

    *ppRetRights = pRights;
    pRights = NULL;

ErrorExit:
    if ( pRights != NULL )
    {
        // this represents an error condition, as we set pRights = NULL upon
        // success
        FreeLicenseRights( pRights );
    }

    return dr;
};

static DRM_RESULT _GetGroupRights(DRM_CONST_STRING *pdstrPool, const DRM_WCHAR *pTestCaseID, LICENSERIGHTS **ppLicenseRights, DRM_CONST_STRING dstrRIV)
{
    DRM_RESULT dr = DRM_SUCCESS;
    DRM_CONST_STRING dstrGroup = EMPTY_DRM_STRING;
    DRM_CONST_STRING dstrName = EMPTY_DRM_STRING;
    DRM_CONST_STRING dstrRights = EMPTY_DRM_STRING;
    LICENSERIGHTS *pRights = NULL;
    LICENSERIGHTS *pTemp = NULL;
    int index = 0;
    DRM_DWORD lenID = 0;
    int iCmp = 0;

    ChkArg(pdstrPool);
    ChkArg(pTestCaseID);
    ChkArg(ppLicenseRights);

    lenID = DRMCRT_wcslen(pTestCaseID);

    *ppLicenseRights = 0;
    do
    {
        dr = DRM_XML_GetNode(pdstrPool, &g_dstrTagLicenseGroup, NULL, NULL, index, &dstrGroup, NULL);
        if (dr == DRM_E_XMLNOTFOUND)
        {
            goto ErrorExit; /* do not a fatal error.  Do not log it. */
        }
        ChkDR(dr);

        ChkDR(DRM_XML_GetNodeAttribute(&dstrGroup, &g_dstrAttribname, &dstrName));
        iCmp = DRMCRT_wcsncmp(dstrName.pwszString, pTestCaseID, lenID);
        if (!iCmp)
        {
            break;  /* we have a match */
        }
    } while (++index);

    ChkDR(DRM_XML_GetNode(pdstrPool, &g_dstrTagLicenseGroup, NULL, NULL, index, NULL, &dstrGroup));

    index = 0;
    do
    {
        dr = DRM_XML_GetNode(&dstrGroup, &g_dstrTagLicenseRights, NULL, NULL, index, &dstrRights, NULL);
        if (dr != DRM_SUCCESS)
        {   /* not end of the world */
            dr = DRM_SUCCESS;
            break;
        }
        ChkDR(_GetRights(&dstrRights, &pRights, dstrRIV));
        pTemp = pRights;
        while (pTemp->pNext)
        {
            pTemp = pTemp->pNext;
        }
        pTemp->pNext = *ppLicenseRights;
        *ppLicenseRights = pRights;
    } while (++index);

ErrorExit:
    return dr;
}


/**
    GetLicenseRightsAndSetRIV - Gets a LICENSERIGHTS structure for a test case
    @Param pRightsPool - string pointer to block of memory containing the rights pool
    @Param pTestCaseID - string pointer to test case ID
    @Param ppLicenseRights - returns pointer to a LICENSERIGHTS structure
    @Param dstrRIV - optional, suggested RIV value that need to be set into every license
    @Return A valid DRM_RESULT value
*/
DRM_RESULT GetLicenseRightsAndSetRIV(const DRM_WCHAR *pRightsPool, const DRM_WCHAR *pTestCaseID, LICENSERIGHTS **ppLicenseRights, DRM_CONST_STRING dstrRIV)
{
    DRM_RESULT dr = DRM_SUCCESS;
    DRM_CONST_STRING dstrCaseNode = EMPTY_DRM_STRING;
    DRM_CONST_STRING dstrPool = EMPTY_DRM_STRING;
    DRM_CONST_STRING dstrFile = EMPTY_DRM_STRING;
    LICENSERIGHTS *pRights = NULL;

    ChkArg(pRightsPool != NULL && pTestCaseID != NULL && ppLicenseRights != NULL);

    dstrFile.pwszString = pRightsPool;
    dstrFile.cchString = DRMCRT_wcslen(pRightsPool);

    dr = DRM_XML_GetNode(&dstrFile, &g_dstrTagRightsPool, NULL, NULL, 0, NULL, &dstrPool);
    if (dr != DRM_SUCCESS)
    {
        /* not end of the world */
        dstrPool.pwszString = dstrFile.pwszString;
        dstrPool.cchString = dstrFile.cchString;
    }

    dr = DRM_XML_GetNode(&dstrPool, &g_dstrTagLicenseGroup, NULL, NULL, 0, NULL, &dstrCaseNode);
    if (dr == DRM_SUCCESS)
    {
        dr = _GetGroupRights(&dstrPool, pTestCaseID, &pRights, dstrRIV);
        if(dr == DRM_SUCCESS && pRights)
        {
            goto ErrorExit;
        }
    }

    ChkDR(_GetTestCaseNode(&dstrPool, pTestCaseID, &dstrCaseNode));
    ChkDR(_GetRights(&dstrCaseNode, &pRights, dstrRIV));

    ChkArg(pRights);


ErrorExit:
    if (ppLicenseRights)
    {
        *ppLicenseRights = pRights;
    }

    return dr;
}

/**
    GetLicenseRightsAndSetRIV - Gets a LICENSERIGHTS structure for a test case
    @Param pRightsPool - string pointer to block of memory containing the rights pool
    @Param pTestCaseID - string pointer to test case ID
    @Param ppLicenseRights - returns pointer to a LICENSERIGHTS structure
    @Return A valid DRM_RESULT value
*/
DRM_RESULT GetLicenseRights(const DRM_WCHAR *pRightsPool, const DRM_WCHAR *pTestCaseID, LICENSERIGHTS **ppLicenseRights)
{
    DRM_RESULT dr = DRM_SUCCESS;
    DRM_CONST_STRING dstrRIV = EMPTY_DRM_STRING;

    ChkDR(GetLicenseRightsAndSetRIV( pRightsPool, pTestCaseID, ppLicenseRights, dstrRIV));
ErrorExit:
    return dr;
}

/**
    GetRevInfo - Gets a RevInfo string for a test case, this RevInfo string is signed and encoded
                 and ready to add to LicenseResponse.
    @Param pwszRevInfoPool - string pointer to block of memory containing the RevInfo pool
    @Param pRevInfoID - ID attribute value of RevInfo node in above RevInfo pool
    @Param pdstrRevInfo - returns pointer to a RevInfo string
    @Return A valid DRM_RESULT value
*/
DRM_RESULT GetRevInfo(const DRM_WCHAR* pwszRevInfoPool, const DRM_WCHAR* pwszRevInfoID, DRM_CONST_STRING* pdstrRevInfo)
{
    DRM_RESULT dr = DRM_SUCCESS;
    DRM_CONST_STRING dstrFile = EMPTY_DRM_STRING;
    DRM_CONST_STRING dstrRevInfoPool = EMPTY_DRM_STRING;
    DRM_CONST_STRING dstrRevInfoID = EMPTY_DRM_STRING;
    DRM_CONST_STRING dstrRevInfoValue = EMPTY_DRM_STRING;

    ChkArg( pwszRevInfoPool );
    ChkArg( pwszRevInfoID );
    ChkArg( pdstrRevInfo );

    dstrFile.pwszString = pwszRevInfoPool;
    dstrFile.cchString = DRMCRT_wcslen(pwszRevInfoPool);
    dr = DRM_XML_GetNode(&dstrFile, &g_dstrTagRightsPool, NULL, NULL, 0, NULL, &dstrRevInfoPool);
    if (dr != DRM_SUCCESS)
    {
        dstrRevInfoPool.pwszString = pwszRevInfoPool;
        dstrRevInfoPool.cchString = DRMCRT_wcslen( pwszRevInfoPool );
    }

    dstrRevInfoID.pwszString = pwszRevInfoID;
    dstrRevInfoID.cchString = DRMCRT_wcslen( pwszRevInfoID );

    ChkDR( DRM_XML_GetNode( &dstrRevInfoPool,
                            &g_dstrTagLARevInfo,
                            &g_dstrAttribRevInfoID,
                            &dstrRevInfoID,
                            0,
                            NULL,
                            &dstrRevInfoValue) );
    ChkDR( CopyDRMString( pdstrRevInfo, &dstrRevInfoValue ) );

ErrorExit:
    return dr;
}

DRM_RESULT FreeLicenseRights(LICENSERIGHTS *pLicenseRights)
{
    DRM_RESULT dr  = DRM_SUCCESS;
    DRM_DWORD  idx = 0;
    if (!pLicenseRights)
    {
        goto ErrorExit;
    }

    FREEDRMSTRING(pLicenseRights->LicenseType);
    FREEDRMSTRING(pLicenseRights->AllowPlay);
    FREEDRMSTRING(pLicenseRights->PlayCount);
    FREEDRMSTRING(pLicenseRights->AllowCopy);
    FREEDRMSTRING(pLicenseRights->CopyCount);
    FREEDRMSTRING(pLicenseRights->AllowTransferToSDMI);
    FREEDRMSTRING(pLicenseRights->AllowTransferToNonSDMI);
    FREEDRMSTRING(pLicenseRights->TransferCount);
    FREEDRMSTRING(pLicenseRights->CopyProtectionLevel);
    FREEDRMSTRING(pLicenseRights->AnalogVideo);
    FREEDRMSTRING(pLicenseRights->AnalogVideoExtensions);
    FREEDRMSTRING(pLicenseRights->UnCompressedDigitalVideo);
    FREEDRMSTRING(pLicenseRights->UnCompressedDigitalAudio);
    FREEDRMSTRING(pLicenseRights->CompressedDigitalVideo);
    FREEDRMSTRING(pLicenseRights->CompressedDigitalAudio);
    FREEDRMSTRING(pLicenseRights->AllowBackUpRestore);
    FREEDRMSTRING(pLicenseRights->AllowBurnToCD);
    FREEDRMSTRING(pLicenseRights->BurnToCDCount);
    FREEDRMSTRING(pLicenseRights->AllowPlaylistBurn);
    FREEDRMSTRING(pLicenseRights->PlaylistBurnTrackCount);
    FREEDRMSTRING(pLicenseRights->MaxPlaylistBurnCount);
    FREEDRMSTRING(pLicenseRights->AllowCollaborativePlay);
    FREEDRMSTRING(pLicenseRights->BeginDate);
    FREEDRMSTRING(pLicenseRights->ExpirationDate);
    FREEDRMSTRING(pLicenseRights->ExpirationAfterFirstUse);
    FREEDRMSTRING(pLicenseRights->ExpirationAfterFirstPlay);
    FREEDRMSTRING(pLicenseRights->ExpirationOnStore);
    FREEDRMSTRING(pLicenseRights->GracePeriod);
    FREEDRMSTRING(pLicenseRights->DeleteOnClockRollback);
    FREEDRMSTRING(pLicenseRights->DisableOnClockRollback);
    FREEDRMSTRING(pLicenseRights->MinimumSecurityLevel);
    FREEDRMSTRING(pLicenseRights->MinimumAppCrlVersion);
    FREEDRMSTRING(pLicenseRights->MinimumDeviceCrlVersion);
    FREEDRMSTRING(pLicenseRights->MinimumClientSDKSecurity);
    FREEDRMSTRING(pLicenseRights->PMRights);
    FREEDRMSTRING(pLicenseRights->PMAppSecurity);
    FREEDRMSTRING(pLicenseRights->PMExpirationDate);
    FREEDRMSTRING(pLicenseRights->AllowSaveStreamProtected);
    FREEDRMSTRING(pLicenseRights->MeteringCert);
    FREEDRMSTRING(pLicenseRights->MeteringID);
    FREEDRMSTRING(pLicenseRights->KID);
    FREEDRMSTRING(pLicenseRights->Priority);
    FREEDRMSTRING(pLicenseRights->RevInfoVersion);
    FREEDRMSTRING(pLicenseRights->IndivVersion);
    FREEDRMSTRING(pLicenseRights->LID);
    FREEDRMSTRING(pLicenseRights->RevocationList);
    FREEDRMSTRING(pLicenseRights->ContentPubKey);
    FREEDRMSTRING(pLicenseRights->IssueDate);
    FREEDRMSTRING(pLicenseRights->RemovalDate);
    FREEDRMSTRING(pLicenseRights->ServerPrivateKey);
    FREEDRMSTRING(pLicenseRights->ServerPublicKey);
    FREEDRMSTRING(pLicenseRights->ServerSeed);
    FREEDRMSTRING(pLicenseRights->WMDRMNETRevocationIndex);
    FREEDRMSTRING(pLicenseRights->DeviceRevocationIndex);
    FREEDRMSTRING(pLicenseRights->Meta);
    FREEDRMSTRING(pLicenseRights->TestOptions);
    FREEDRMSTRING(pLicenseRights->ExcludeApps);
    FREEDRMSTRING(pLicenseRights->Description);
    FREEDRMSTRING(pLicenseRights->SAPMode);
    FREEDRMSTRING(pLicenseRights->UplinkKIDs);
    FREEDRMSTRING(pLicenseRights->LGPUBKEY);
    FREEDRMSTRING(pLicenseRights->UID);
    FREEDRMSTRING(pLicenseRights->AttributeList);
    FREEDRMSTRING(pLicenseRights->UseAESContentKey);
    FREEDRMSTRING(pLicenseRights->UseECCPubKey);
    FREEDRMSTRING(pLicenseRights->SourceID);
    FREEDRMSTRING(pLicenseRights->UnCompressedDigitalAudioLevels);
    FREEDRMSTRING(pLicenseRights->Cert1);
    FREEDRMSTRING(pLicenseRights->Cert2);
    FREEDRMSTRING(pLicenseRights->ContentKey);
    FREEDRMSTRING(pLicenseRights->InclusionList);
    FREEDRMSTRING(pLicenseRights->CannotPersist);
    FREEDRMSTRING(pLicenseRights->RestrictedSourceID);
    FREEDRMSTRING(pLicenseRights->WMDRMNETRevocationData);
    FREEDRMSTRING(pLicenseRights->XMRVersion);
    FREEDRMSTRING(pLicenseRights->XPolicy);
    FREEDRMSTRING(pLicenseRights->XPolicyPlayExtended);
    FREEDRMSTRING(pLicenseRights->XPolicyBurnExtended);
    FREEDRMSTRING(pLicenseRights->DigitalAudioRestrictions);
    FREEDRMSTRING(pLicenseRights->PolicyMetadataList);
    FREEDRMSTRING(pLicenseRights->CanNotBind);
    FREEDRMSTRING(pLicenseRights->DoNotSign);
    FREEDRMSTRING(pLicenseRights->RSAPublicKey);
    FREEDRMSTRING(pLicenseRights->ECCPublicKey);
    FREEDRMSTRING(pLicenseRights->UplinkContentKey);
    FREEDRMSTRING(pLicenseRights->Use16ByteCheckSum);
    FREEDRMSTRING(pLicenseRights->RevInfoForLicenseResponse);
    FREEDRMSTRING(pLicenseRights->EmbedLicense);
    FREEDRMSTRING(pLicenseRights->DomainID);
    FREEDRMSTRING(pLicenseRights->DomainAccountID);
    FREEDRMSTRING(pLicenseRights->DomainRevision);
    FREEDRMSTRING(pLicenseRights->SerialNumber);
    FREEDRMSTRING(pLicenseRights->PlayEnablerGUIDList);
    FREEDRMSTRING(pLicenseRights->CopyEnablerGUIDList);
    FREEDRMSTRING(pLicenseRights->CopyCount2);
    FREEDRMSTRING(pLicenseRights->DeviceKey);
    FREEDRMSTRING(pLicenseRights->VerifyExtensibleObject);
    FREEDRMSTRING(pLicenseRights->ExtensibleDepth);
    FREEDRMSTRING(pLicenseRights->VerifyPlayRightsExtensibleContainer);
    FREEDRMSTRING(pLicenseRights->VerifyPlayRightsExtensibleObject);
    FREEDRMSTRING(pLicenseRights->MoveEnabler);
    FREEDRMSTRING(pLicenseRights->TempStoreOnly);
    FREEDRMSTRING(pLicenseRights->AuxKey);
    FREEDRMSTRING(pLicenseRights->UplinkX);

    for( idx = 0; idx < g_cdwExtensiblePolicyMapMax; idx++ )
    {
        FREEDRMSTRING(pLicenseRights->AllowExtensibleRights[idx]);
    }
    FreeLicenseRights(pLicenseRights->pNext);
    FreeLicenseRights(pLicenseRights->pLeaf);

    Oem_MemFree(pLicenseRights);

ErrorExit:
    return dr;
}

DRM_RESULT _tGetKIDsFromLicenseRightsNode(const DRM_WCHAR *pNode, const DRM_WCHAR **ppwszKID, const DRM_WCHAR **ppwszRootKID)
{
    DRM_RESULT dr = DRM_SUCCESS;
    DRM_CONST_STRING dstrNode = EMPTY_DRM_STRING;
    DRM_CONST_STRING dstrValue = EMPTY_DRM_STRING;
    DRM_CONST_STRING dstrTemp = EMPTY_DRM_STRING;

    ChkArg(pNode);
    ChkArg(ppwszKID);
    ChkArg(ppwszRootKID);

    *ppwszKID = NULL;
    *ppwszRootKID = NULL;

    dstrNode.pwszString = pNode;
    dstrNode.cchString = DRMCRT_wcslen(pNode);

    ChkDR(DRM_XML_GetNodeAttribute(&dstrNode, &g_dstrAttribKID, &dstrValue));
    ChkDR(CopyDRMString(&dstrTemp, &dstrValue));
    *ppwszKID = dstrTemp.pwszString;
    dstrTemp.pwszString = NULL;
    dstrTemp.cchString = 0;

    dr = DRM_XML_GetNodeAttribute(&dstrNode, &g_dstrAttribUplinkKIDs, &dstrValue);
    if (dr == DRM_SUCCESS)
    {
        ChkDR(CopyDRMString(&dstrTemp, &dstrValue));
        *ppwszRootKID = dstrTemp.pwszString;
        dstrTemp.pwszString = NULL;
        dstrTemp.cchString = 0;
    }
    else if (dr == DRM_E_INVALIDXMLTAG)
    {   /* not a fatal error */
        dr = DRM_SUCCESS;
    }

ErrorExit:
    FREEDRMSTRING(dstrTemp);

    return dr;
}

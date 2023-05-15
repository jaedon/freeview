/**@@@+++@@@@******************************************************************
**
** Microsoft (r) PlayReady (r)
** Copyright (c) Microsoft Corporation. All rights reserved.
**
***@@@---@@@@******************************************************************
*/

#ifndef __tOEMIMP_H
#define __tOEMIMP_H

#include <drmcommon.h>
#include <drmcontextsizes.h>
#include <PKTestNameSpace.h>

ENTER_PKTEST_NAMESPACE;

/*************
The following platform dependent functions are used by test code
**************/
DRM_BOOL DRM_CALL tMoveFile(const DRM_WCHAR *wszSrc, const DRM_WCHAR *wszTarget);
void DRM_CALL tDRMSleep(DRM_DWORD dwMillSecond);

/*
** The size of the pipes should be big enough to accomodate the whole console
** output to avoid deadlock.
*/
#define PIPE_SIZE   16384

DRM_VOID DRM_CALL TST_OEM_LeakScan();
DRM_VOID DRM_CALL TST_OEM_LeakClear();
DRM_RESULT DRM_CALL TST_OEM_CopyFile(DRM_CONST_STRING f_dstrSource, DRM_CONST_STRING f_dstrDest);
DRM_RESULT DRM_CALL TST_OEM_CreateDirectory( __in_z const DRM_CHAR *f_szPath );
DRM_RESULT DRM_CALL TST_OEM_ExecuteShellCommand( __in_z DRM_CHAR *f_szCommandLine );

#define DRMTEST_AsyncShellCommand_Map_Size      (1024*1024)     /* 1 MB */

/* L"Local\\DRMTEST_AsyncShellCommand_Map_Name" */
#define DRMTEST_AsyncShellCommand_Map_Name      { WCHAR_CAST('L'), WCHAR_CAST('o'), WCHAR_CAST('c'), WCHAR_CAST('a'), WCHAR_CAST('l'), WCHAR_CAST('\\'), WCHAR_CAST('D'), WCHAR_CAST('R'), WCHAR_CAST('M'), WCHAR_CAST('T'), WCHAR_CAST('E'), WCHAR_CAST('S'), WCHAR_CAST('T'), WCHAR_CAST('_'), WCHAR_CAST('A'), WCHAR_CAST('s'), WCHAR_CAST('y'), WCHAR_CAST('n'), WCHAR_CAST('c'), WCHAR_CAST('S'), WCHAR_CAST('h'), WCHAR_CAST('e'), WCHAR_CAST('l'), WCHAR_CAST('l'), WCHAR_CAST('C'), WCHAR_CAST('o'), WCHAR_CAST('m'), WCHAR_CAST('m'), WCHAR_CAST('a'), WCHAR_CAST('n'), WCHAR_CAST('d'), WCHAR_CAST('_'), WCHAR_CAST('M'), WCHAR_CAST('a'), WCHAR_CAST('p'), WCHAR_CAST('_'), WCHAR_CAST('N'), WCHAR_CAST('a'), WCHAR_CAST('m'), WCHAR_CAST('e'), WCHAR_CAST('\0') }

/* L"DRMTEST_AsyncShellCommand_Event1" */
#define DRMTEST_AsyncShellCommand_Event1_Name   { WCHAR_CAST('D'), WCHAR_CAST('R'), WCHAR_CAST('M'), WCHAR_CAST('T'), WCHAR_CAST('E'), WCHAR_CAST('S'), WCHAR_CAST('T'), WCHAR_CAST('_'), WCHAR_CAST('A'), WCHAR_CAST('s'), WCHAR_CAST('y'), WCHAR_CAST('n'), WCHAR_CAST('c'), WCHAR_CAST('S'), WCHAR_CAST('h'), WCHAR_CAST('e'), WCHAR_CAST('l'), WCHAR_CAST('l'), WCHAR_CAST('C'), WCHAR_CAST('o'), WCHAR_CAST('m'), WCHAR_CAST('m'), WCHAR_CAST('a'), WCHAR_CAST('n'), WCHAR_CAST('d'), WCHAR_CAST('_'), WCHAR_CAST('E'), WCHAR_CAST('v'), WCHAR_CAST('e'), WCHAR_CAST('n'), WCHAR_CAST('t'), WCHAR_CAST('1'), WCHAR_CAST('\0') }

/* L"DRMTEST_AsyncShellCommand_Event2" */
#define DRMTEST_AsyncShellCommand_Event2_Name   { WCHAR_CAST('D'), WCHAR_CAST('R'), WCHAR_CAST('M'), WCHAR_CAST('T'), WCHAR_CAST('E'), WCHAR_CAST('S'), WCHAR_CAST('T'), WCHAR_CAST('_'), WCHAR_CAST('A'), WCHAR_CAST('s'), WCHAR_CAST('y'), WCHAR_CAST('n'), WCHAR_CAST('c'), WCHAR_CAST('S'), WCHAR_CAST('h'), WCHAR_CAST('e'), WCHAR_CAST('l'), WCHAR_CAST('l'), WCHAR_CAST('C'), WCHAR_CAST('o'), WCHAR_CAST('m'), WCHAR_CAST('m'), WCHAR_CAST('a'), WCHAR_CAST('n'), WCHAR_CAST('d'), WCHAR_CAST('_'), WCHAR_CAST('E'), WCHAR_CAST('v'), WCHAR_CAST('e'), WCHAR_CAST('n'), WCHAR_CAST('t'), WCHAR_CAST('2'), WCHAR_CAST('\0') }

#define ASYNC_SHELL_COMMAND_DEFAULT_PING_TIME   50              /* 50 ms */
#define ASYNC_SHELL_COMMAND_WAIT_PINGS          120             /* 120*50 ms = 6 s */

#define ASYNC_SHELL_COMMAND_MESSAGE_TYPE_HALT_PROCESS       0

DRM_RESULT DRM_CALL TST_OEM_AsyncShellCommandStart(
    __in_z                        const DRM_CHAR                         *f_szCommandLine, 
    __out                               DRM_VOID                        **f_ppvAsyncShellCommandData );
DRM_RESULT DRM_CALL TST_OEM_AsyncShellCommandEnd(
    __inout                             DRM_VOID                        **f_ppvAsyncShellCommandData );
DRM_RESULT DRM_CALL TST_OEM_AsyncShellCommandPing(
    __in                                DRM_VOID                         *f_pvAsyncShellCommandData,
    __in                                DRM_DWORD                         f_dwMs );
DRM_RESULT DRM_CALL TST_OEM_AsyncShellCommandReadMessage(
    __in                                DRM_VOID                         *f_pvAsyncShellCommandData,
    __out                               DRM_DWORD                        *f_pdwMessageType,
    __deref_out_ecount(*f_pcbMessage)   DRM_BYTE                        **f_ppbMessage,
    __out                               DRM_DWORD                        *f_pcbMessage );
DRM_RESULT DRM_CALL TST_OEM_AsyncShellCommandWriteMessage(
    __in                                DRM_VOID                         *f_pvAsyncShellCommandData,
    __in                                DRM_DWORD                         f_dwMessageType,
    __in_ecount(f_cbMessage)      const DRM_BYTE                         *f_pbMessage,
    __in                                DRM_DWORD                         f_cbMessage );

DRM_VOID   DRM_CALL tOEMSignalTestDone();

DRM_RESULT DRM_CALL TST_OEM_IsConditionTrue( __out DRM_BOOL       *f_pfCondition,
                                             __in  const DRM_CHAR *f_pszCondition );

#define GET_DRM_FILE_NAME_PATH_LENGTH 256

DRM_RESULT tGetDRMFileName(
    __in_ecount(GET_DRM_FILE_NAME_PATH_LENGTH)  DRM_WCHAR            f_wszPathName[GET_DRM_FILE_NAME_PATH_LENGTH],
    __out                                       DRM_CONST_STRING    *f_pdstrPathname,
    __in                                  const DRM_WCHAR           *f_wszFileName );
DRM_RESULT DRM_CALL tGetDeviceStorePathname(
    __in_ecount(GET_DRM_FILE_NAME_PATH_LENGTH)  DRM_WCHAR            f_wszPathName[GET_DRM_FILE_NAME_PATH_LENGTH],
    __out                                       DRM_CONST_STRING    *f_pdstrPathname );
DRM_RESULT DRM_CALL tGetDeviceCertPathname(
    __in_ecount(GET_DRM_FILE_NAME_PATH_LENGTH)  DRM_WCHAR            f_wszPathName[GET_DRM_FILE_NAME_PATH_LENGTH],
    __out                                       DRM_CONST_STRING    *f_pdstrPathname );
DRM_RESULT DRM_CALL tGetBinCertPathname(
    __in_ecount(GET_DRM_FILE_NAME_PATH_LENGTH)  DRM_WCHAR            f_wszPathName[GET_DRM_FILE_NAME_PATH_LENGTH],
    __out                                       DRM_CONST_STRING    *f_pdstrPathname );
DRM_RESULT DRM_CALL tGetDRMPath(
    __in_ecount(GET_DRM_FILE_NAME_PATH_LENGTH)  DRM_WCHAR            f_wszPathName[GET_DRM_FILE_NAME_PATH_LENGTH],
    __out                                       DRM_CONST_STRING    *f_pdstrPathname );
DRM_RESULT DRM_CALL tRemoveFile(const DRM_CHAR* filename);
DRM_RESULT DRM_CALL tRemoveFileW(const DRM_WCHAR *wcszPathname);

#define RMFILE_STORE              0x0001        /* Delete the file "c:\wmdrmpd\DRMDeviceStore.hds" */

#define RMFILE_PD_DEVCERT         0x0002        /* Delete the file "c:\wmdrmpd\devcert.dat" */
#define RMFILE_PR_DEVCERT         0x0004        /* Delete the file "c:\wmdrmpd\bdevcert.dat" */
#define RMFILE_ND_DEVCERT         0x0008        /* Delete the file "c:\wmdrmpd\ndrcert.dat" */
#define RMFILE_DEVCERT            RMFILE_PD_DEVCERT \
                                | RMFILE_ND_DEVCERT \
                                | RMFILE_PR_DEVCERT

#define RMFILE_PD_DEVCERTTEMPLATE 0x0010        /* Delete the file "c:\wmdrmpd\devcerttemplate.dat" */
#define RMFILE_PR_DEVCERTTEMPLATE 0x0020        /* Delete the file "c:\wmdrmpd\bgroupcert.dat" */
#define RMFILE_ND_DEVCERTTEMPLATE 0x0040        /* Delete the file "c:\wmdrmpd\ndrcerttemplate.dat" */
#define RMFILE_DEVCERTTEMPLATE    RMFILE_PD_DEVCERTTEMPLATE \
                                | RMFILE_ND_DEVCERTTEMPLATE \
                                | RMFILE_PR_DEVCERTTEMPLATE


#define RMFILE_PD_PRVKEY          0x0100        /* Delete the file "c:\wmdrmpd\privkey.dat" */
#define RMFILE_PR_PRVKEY          0x0200        /* Delete the file "c:\wmdrmpd\zprivencr.dat" and  "c:\wmdrmpd\zprivsig.dat" */
#define RMFILE_PR_GROUP_PRVKEY    0x0400        /* Delete the file "c:\wmdrmpd\zgpriv.dat" */
#define RMFILE_ND_PRVKEY          0x0800        /* Delete the file "c:\wmdrmpd\ndrpriv.dat" */
#define RMFILE_ND_GROUP_PRVKEY    0x1000        /* Delete the file "c:\wmdrmpd\ndrgpriv.dat" */
#define RMFILE_KEYFILE            0x2000        /* Delete the file "c:\wmdrmpd\keyfile.dat" */
#define RMFILE_PRVKEY             RMFILE_PD_PRVKEY       \
                                | RMFILE_PR_PRVKEY       \
                                | RMFILE_PR_GROUP_PRVKEY \
                                | RMFILE_ND_PRVKEY       \
                                | RMFILE_ND_GROUP_PRVKEY

#define RMFILE_ALL                0xffff     /* Delete everything */
DRM_RESULT DRM_CALL RemoveDRMFile(DRM_DWORD dwRemove);
DRM_RESULT DRM_CALL _CopyFileToDRMFolder(const DRM_WCHAR *wszSrc, const DRM_WCHAR *wszDest);
DRM_BOOL DRM_CALL _IsDebuggerAttached();

/* DoTest uses the following two functions to compile a list of test case files. */
DRM_RESULT DRM_CALL tGetTCFileList(const char *szTestFilePrefix, const char **szPlatforms, const char ***pszTestFileNames);
void DRM_CALL tFreeFileNameList(__in_opt const char **szTestFileNames);




DRM_UINT64 DRM_CALL GetTimer(void);

#define START_TIMER(x) do { (x) = GetTimer(); }while( FALSE )
#define STOP_TIMER(x,y) do { (y) = DRM_UI64Sub(GetTimer(), (x)); }while( FALSE )

/* These 4 files are WMDRM-specific */
extern const DRM_WCHAR DEVCERT_FILE_NAME [];
extern const DRM_WCHAR DEVCERT_TEMPLATE_FILE_NAME [];
extern const DRM_WCHAR PRIVATE_KEY_FILE_NAME [];
extern const DRM_WCHAR WMDRMPD_KEY_FILE_NAME [];

/* These files are used for either WMDRM or PlayReady */
extern const DRM_WCHAR GROUP_CERT_FILE_NAME [];
extern const DRM_WCHAR BDEVCERT_FILE_NAME [];
extern const DRM_WCHAR KEYFILE_FILE_NAME[];
extern const DRM_WCHAR Z_PRIVATE_KEY_FILE_NAME_SIGN [];
extern const DRM_WCHAR Z_PRIVATE_KEY_FILE_NAME_ENCRYPT [];
extern const DRM_WCHAR GC_PRIVATE_KEY_FILE_NAME [];
extern const DRM_WCHAR DEVICE_STORE_FILE_NAME[];
extern const DRM_WCHAR NDR_MODEL_CERT[];
extern const DRM_WCHAR NDR_MODEL_PRIVATE_KEY[];
extern const DRM_WCHAR NDR_CERT[];
extern const DRM_WCHAR NDR_PRIVATE_KEY[];

EXIT_PKTEST_NAMESPACE;

#endif //__tOEMIMP_H

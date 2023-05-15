/**@@@+++@@@@******************************************************************
**
** Microsoft (r) PlayReady (r)
** Copyright (c) Microsoft Corporation. All rights reserved.
**
***@@@---@@@@******************************************************************
*/

#ifndef __tOEMIMP_H
#define __tOEMIMP_H

#include "drmcommon.h"
#include "drmcontextsizes.h"


#include "PKTestNameSpace.h"

ENTER_PKTEST_NAMESPACE

/*************
The following platform dependent functions are used by test code
**************/

#if SIXTEEN_BIT_ADDRESSING
/* 
** DoTest uses the following two functions to compile a list of test case files. These functions 
** are modified for 16-bit platform. 
*/
DRM_RESULT DRM_CALL tGetTCFileList_pfd(char *szTestFilePrefix, char **szPlatforms, char ***pszTestFileNames);

void DRM_CALL tFreeFileNameList_pfd(char **szTestFileNames);
#else
/* 
** Some calls used in tMoveFile are not available on 16-bit platform 
*/
DRM_BOOL DRM_CALL tMoveFile(const DRM_WCHAR *wszSrc, const DRM_WCHAR *wszTarget);
#endif

void DRM_CALL tDRMSleep(DRM_DWORD dwMillSecond);

/*
** The size of the pipes should be big enough to accomodate the whole console
** output to avoid deadlock.
*/
#define PIPE_SIZE   16384

DRM_RESULT DRM_CALL TST_OEM_CopyFile(DRM_CONST_STRING f_dstrSource, DRM_CONST_STRING f_dstrDest);
DRM_RESULT DRM_CALL TST_OEM_CreateDirectory( __in_z DRM_CHAR *f_szPath );
DRM_RESULT DRM_CALL TST_OEM_ExecuteShellCommand( __in_z DRM_CHAR *f_szCommandLine );
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
void DRM_CALL tFreeFileNameList(IN const char **szTestFileNames);




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



EXIT_PKTEST_NAMESPACE

#endif //__tOEMIMP_H

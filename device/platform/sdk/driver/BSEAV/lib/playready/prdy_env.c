/***************************************************************************
 *     Copyright (c) 2002-2012, Broadcom Corporation
 *     All Rights Reserved
 *     Confidential Property of Broadcom Corporation
 *
 *  THIS SOFTWARE MAY ONLY BE USED SUBJECT TO AN EXECUTED SOFTWARE LICENSE
 *  AGREEMENT  BETWEEN THE USER AND BROADCOM.  YOU HAVE NO RIGHT TO USE OR
 *  EXPLOIT THIS MATERIAL EXCEPT SUBJECT TO THE TERMS OF SUCH AN AGREEMENT.
 *
 * $brcm_Workfile: prdy_env.c $
 * $brcm_Revision: 5 $
 * $brcm_Date: 7/17/12 4:13p $
 *
 * Module Description:
 *
 * Playready library
 * 
 * Revision History:
 *
 * $brcm_Log: /BSEAV/lib/playready/prdy_env.c $
 * 
 * 5   7/17/12 4:13p piyushg
 * SW7425-3281: Add playready kernel mode support and nexus drm
 * implementation.
 * 
 * SW7425-2317/2   4/9/12 10:11a hugost
 * Robustness rules support in kernel mode
 * 
 * SW7425-2317/1   2/28/12 10:01p piyushg
 * Added file element "*.a".
 * Added file element "certs.a".
 * Added file element "cmdlnpars.a".
 * Added file element "constants.a".
 * Added file element "domain.a".
 * Added file element "drmbignum.a".
 * Added file element "drmblackbox.a".
 * Added file element "drmcore.a".
 * Added file element "drmcrypto.a".
 * Added file element "drmdevcert.a".
 * Added file element "drmdevicedevcert.a".
 * Added file element "drmmanager.a".
 * Added file element "drmmetering.a".
 * Added file element "drmpkcrypto.a".
 * Added file element "drmsecureclock.a".
 * Added file element "drmsha256.a".
 * Added file element "drmsynclist.a".
 * Added file element "drmtoolscommon.a".
 * Added file element "drmxmr.a".
 * Added file element "ndt.a".
 * Added file element "ndtasf.a".
 * Added file element "ndtmgr.a".
 * Added file element "noncestore.a".
 * Added file element "oem_ansi.a".
 * Added file element "oem_common.a".
 * Added file element "wmdrmcert.a".
 * Added directory element "playreadylinuxkernel".
 * Added file element "Makefile.wmdrmpd".
 * SW7425-2317: Initial checkin for Nexus DRM usrmode and kernelmode
 * support
 * 
 * 4   11/1/11 9:51a hugost
 * SW7425-1625: Added MP4 fragment support
 * 
 * 3   10/25/11 3:18p piyushg
 * SW7425-1607: Added file header to track revision history.
 ***************************************************************************/

#include <drmcommon.h>
#include <drmutilities.h>
#include <drmcrt.h>
#include <drmcontextsizes.h>
#include <drmmanager.h>
#include <oemcommon.h>
#ifndef __KERNEL__
#include <stdio.h>
#include <string.h>
#else
#include <generated/autoconf.h>
#include <linux/string.h>
#endif
#include <drmcmdlnpars.h>
#include <prdy_env.h>
#include "bstd.h"

#ifdef PRDY_ROBUSTNESS
#include "drm_metadata.h"
#include "drm_data.h"

static DRM_STRING currDataStore = EMPTY_DRM_STRING;
static DRM_WCHAR hds_default_path[DRM_MAX_PATH];
static DRM_WCHAR hds_current_path[DRM_MAX_PATH];

/* Set current data store path
*/
void bdrm_env_set_curr_datastore(DRM_VOID *ptrToDSPath, int from_char_to_drm_string)
{

    currDataStore.pwszString = hds_current_path;
    currDataStore.cchString = DRM_MAX_PATH;

    if(from_char_to_drm_string == 1)
    {
        DRM_SUBSTRING substr = { 0, 0 };
        DRM_CHAR    tmp_buf[128] = {0x00};

        /*Promote to a Unicode string        */
        strcpy((char *)tmp_buf, (char *)ptrToDSPath);
        substr.m_cch     = strlen((char *)tmp_buf);

        BKNI_Memset(currDataStore.pwszString, 0, DRM_MAX_PATH * SIZEOF (DRM_WCHAR) ); 
        DRM_UTL_PromoteASCIItoUNICODE(tmp_buf, &substr, &currDataStore);
    }
    else
    {
        DRM_STRING *tmp = NULL;

        tmp = (DRM_STRING *)ptrToDSPath;
        BKNI_Memcpy(currDataStore.pwszString, tmp->pwszString, tmp->cchString * SIZEOF (DRM_WCHAR));
        currDataStore.cchString = tmp->cchString;
    }
}

/* Get current data store path
*/
void bdrm_env_get_curr_datastore(DRM_CONST_STRING *pwszDataStore)
{
    if (pwszDataStore != NULL) 
    {
        (pwszDataStore)->pwszString = currDataStore.pwszString;
        (pwszDataStore)->cchString =  currDataStore.cchString;
    }
}


/* Get default data store path
*/

void bdrm_env_default_datastore(DRM_CONST_STRING *pwszDataStore)
{
    DRM_CONST_STRING path;
    BDBG_ASSERT(pwszDataStore != NULL);

    path.pwszString = (const DRM_WCHAR *)bdrm_get_hds_dir();
    path.cchString  = bdrm_get_hds_dir_lgth();

    if (pwszDataStore != NULL) {
        DRM_STR_StringCchCopyNW( 
            hds_default_path,
            DRM_MAX_PATH,
            path.pwszString,
            path.cchString);

        pwszDataStore->cchString = path.cchString;

        path.pwszString = (const DRM_WCHAR *)bdrm_get_hds_fname();
        path.cchString  = bdrm_get_hds_fname_lgth();

        DRM_STR_StringCchCatNW( 
            hds_default_path,
            DRM_MAX_PATH,
            path.pwszString,
            path.cchString);

        pwszDataStore->pwszString = hds_default_path;
        pwszDataStore->cchString += path.cchString;
    }

    return;
}
/* Get default data store path
*/
void bdrm_env_datastore_dir(DRM_CONST_STRING *pwszDataStoreDir)
{
    BDBG_ASSERT(pwszDataStoreDir != NULL);

    if (pwszDataStoreDir != NULL) {        
        (pwszDataStoreDir)->pwszString = (const DRM_WCHAR *)bdrm_get_hds_dir();
        (pwszDataStoreDir)->cchString =  bdrm_get_hds_dir_lgth();
    }
}

/* Retrieves data store password filename */
void bdrm_env_hds_password_filename (DRM_CONST_STRING *pwszFilename)
{
    BDBG_ASSERT(pwszFilename != NULL);

    if (pwszFilename != NULL) {
        (pwszFilename)->pwszString = (const DRM_WCHAR *)bdrm_get_hds_pwd_fname();
        (pwszFilename)->cchString =  bdrm_get_hds_pwd_fname_lgth();
    }
}

/* Retrieves data store filename */
void bdrm_env_hds_filename (DRM_CONST_STRING *pwszFilename)
{
    BDBG_ASSERT(pwszFilename != NULL);

    if (pwszFilename != NULL) {
        (pwszFilename)->pwszString = (const DRM_WCHAR *)bdrm_get_hds_fname();
        (pwszFilename)->cchString =  bdrm_get_hds_fname_lgth();
    }
}

/* Retrieves cert dir path */
void bdrm_env_cert_dir (
    DRM_CONST_STRING *pwszDevCertDir
    )
{
    BDBG_ASSERT(pwszDevCertDir != NULL);

    if (pwszDevCertDir != NULL) {
        (pwszDevCertDir)->pwszString = (const DRM_WCHAR *)bdrm_get_cert_dir();
        (pwszDevCertDir)->cchString =  bdrm_get_cert_dir_lgth();
    }
}

/* Retrieves zgpriv signature filename */
void bdrm_env_zgpriv_sig_filename (DRM_CONST_STRING *pwszFilename)
{
    BDBG_ASSERT(pwszFilename != NULL);

    if (pwszFilename != NULL) {
        (pwszFilename)->pwszString = (const DRM_WCHAR *)bdrm_get_zgpriv_sig_key_fname();
        (pwszFilename)->cchString =  bdrm_get_zgpriv_sig_key_fname_lgth();
    }
}

/* Retrieves zgpriv enc filename */
void bdrm_env_zgpriv_enc_filename (DRM_CONST_STRING *pwszFilename)
{
    BDBG_ASSERT(pwszFilename != NULL);

    if (pwszFilename != NULL) {
        (pwszFilename)->pwszString = (const DRM_WCHAR *)bdrm_get_zgpriv_enc_key_fname();
        (pwszFilename)->cchString =  bdrm_get_zgpriv_enc_key_fname_lgth();
    }
}


/* Retrieves zdev certificate filename */
void bdrm_env_zdev_cert_filename (DRM_CONST_STRING *pwszFilename)
{
    BDBG_ASSERT(pwszFilename != NULL);

    if (pwszFilename != NULL) {
        (pwszFilename)->pwszString = (const DRM_WCHAR *)bdrm_get_zdev_cert_fname();
        (pwszFilename)->cchString =  bdrm_get_zdev_cert_fname_lgth();
    }
}


/* Retrieves device cert filename
*/
void bdrm_env_dev_cert_filename (
    DRM_CONST_STRING *pwszFilename
)
{
    BDBG_ASSERT(pwszFilename != NULL);

    if (pwszFilename != NULL) {
        (pwszFilename)->pwszString = (const DRM_WCHAR *)bdrm_get_dev_cert_fname();
        (pwszFilename)->cchString =  bdrm_get_dev_cert_fname_lgth();
    }
}

/* Retrieves revocation info filename
*/
void bdrm_env_revocation_filename (
    DRM_CONST_STRING *pwszFilename
)
{
    BDBG_ASSERT(pwszFilename != NULL);

    if (pwszFilename != NULL) {
        (pwszFilename)->pwszString = (const DRM_WCHAR *)bdrm_get_revocation_fname();
        (pwszFilename)->cchString =  bdrm_get_revocation_fname_lgth();
    }
}

#else
void _GetDevCertDir(DRM_CONST_STRING *path)
{
    path->pwszString = g_dstrDrmPath.pwszString;
    path->cchString  = g_dstrDrmPath.cchString;
}

void _GetDevTemplateDir(DRM_CONST_STRING *path)
{
    path->pwszString = g_dstrDrmPath.pwszString;
    path->cchString  = g_dstrDrmPath.cchString;
}

void _GetDevCertFilename(DRM_CONST_STRING *path)
{
    path->pwszString = g_dstrDevcert.pwszString;
    path->cchString  = g_dstrDevcert.cchString;
}

void _GetDrmDevTemplateFilename(DRM_CONST_STRING *path)
{
    path->pwszString = g_dstrDevcerttemplate.pwszString;
    path->cchString  = g_dstrDevcerttemplate.cchString;
}

void _GetDrmHdsPasswordFilename(DRM_CONST_STRING *path)
{
    path->pwszString = g_dstrHdsPasswordFile.pwszString;
    path->cchString  = g_dstrHdsPasswordFile.cchString;
}

void _GetPrivateKeyFilename(DRM_CONST_STRING *path)
{
    path->pwszString = g_dstrPrivKey.pwszString;
    path->cchString  = g_dstrPrivKey.cchString;
}

void _GetHDSFilename(DRM_CONST_STRING *path) 
{
    path->pwszString = g_dstrDrmHDS.pwszString;
    path->cchString  = g_dstrDrmHDS.cchString;
}

/* Read path from the env variable or from some other designated place (weapper)
*/
void bdrm_env_path(DRM_CONST_STRING *pwszPath)
{
    BDBG_ASSERT(pwszPath != NULL);

    if (pwszPath != NULL) {		
        _GetDevCertDir(pwszPath);
    }
}

/* Get private key and device certificate (wrapper)
*/
void bdrm_env_privcert(DRM_CONST_STRING *pwszPrivateDat, DRM_CONST_STRING *pwszDeviceCertDat)
{
    BDBG_ASSERT(pwszPrivateDat != NULL); 
    BDBG_ASSERT(pwszDeviceCertDat != NULL);

    if (pwszPrivateDat != NULL) {		
        _GetPrivateKeyFilename(pwszPrivateDat);
    }

    if (pwszDeviceCertDat != NULL) {		
        _GetDevCertFilename(pwszDeviceCertDat);
    }
}

/* Get default data store path (wrapper)
*/
void
    bdrm_env_datastore(DRM_CONST_STRING *pwszDataStore)
{
    BDBG_ASSERT(pwszDataStore != NULL);

    if (pwszDataStore != NULL) {		
        _GetHDSFilename(pwszDataStore);
    }
}
#endif

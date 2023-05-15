/***************************************************************************
 *     Copyright (c) 2002-2011, Broadcom Corporation
 *     All Rights Reserved
 *     Confidential Property of Broadcom Corporation
 *
 *  THIS SOFTWARE MAY ONLY BE USED SUBJECT TO AN EXECUTED SOFTWARE LICENSE
 *  AGREEMENT  BETWEEN THE USER AND BROADCOM.  YOU HAVE NO RIGHT TO USE OR
 *  EXPLOIT THIS MATERIAL EXCEPT SUBJECT TO THE TERMS OF SUCH AN AGREEMENT.
 *
 * $brcm_Workfile: prdy_env.h $
 * $brcm_Revision: 4 $
 * $brcm_Date: 11/1/11 9:51a $
 *
 * Module Description:
 *
 * Playready library
 * 
 * Revision History:
 *
 * $brcm_Log: /BSEAV/lib/playready/prdy_env.h $
 * 
 * 4   11/1/11 9:51a hugost
 * SW7425-1625: Added MP4 fragment support
 * 
 * 3   10/25/11 3:18p piyushg
 * SW7425-1607: Added file header to track revision history.
 ***************************************************************************/

#ifndef __DRMENV_H__
#define __DRMENV_H__

#ifdef PRDY_ROBUSTNESS
/* Set the current data store path set by the user */
void bdrm_env_set_curr_datastore(
    DRM_VOID *ptrToDSPath, 
    int from_char_to_drm_string
);

/* Clear the current data store path */
void bdrm_env_clear_curr_datastore(void);

/* Retrieves the current data store path set by the user */
void bdrm_env_get_curr_datastore(
    DRM_CONST_STRING *pwszDataStore
    );


/* Retrieves private key and device certificate file locations */
void bdrm_env_privcert ( 
	DRM_CONST_STRING **pwszPrivateDat,		/* [out] private key file location */ 
	DRM_CONST_STRING **pwszDeviceCertDat 	/* [out] device certificate file location */
	);

/* Retrieves the default data store file location */
void bdrm_env_default_datastore ( 
    DRM_CONST_STRING *pwszDataStore 		/* [out] data store file location */
    );

/* Retrieves data store directory location */
void bdrm_env_datastore_dir (
    DRM_CONST_STRING *pwszDataStoreDir
    );

/* Retrieves environment path, used in all previous functions.
** This should default to '.' for most cases. */
void bdrm_env_path ( 
    DRM_CONST_STRING **pwszPath 			/* [out] environment dir location */
    );

/* Retrieves data store password filename */
void bdrm_env_hds_password_filename (DRM_CONST_STRING *pwszDataStorePassword);

/* Retrieves data store filename */
void bdrm_env_hds_filename (DRM_CONST_STRING *pwszFilename);

/* Retrieves zgpriv enc filename */
void bdrm_env_zgpriv_enc_filename (DRM_CONST_STRING *pwszFilename);

/* Retrieves zgpriv signature filename */
void bdrm_env_zgpriv_sig_filename (DRM_CONST_STRING *pwszFilename);

/* Retrieves zdev certificate filename */
void bdrm_env_zdev_cert_filename (DRM_CONST_STRING *pwszFilename);

/* Retrieves cert dir path */
void bdrm_env_cert_dir (DRM_CONST_STRING *pwszDevCertDir);


/* Retrieves device cert filename */
void bdrm_env_dev_cert_filename (
		DRM_CONST_STRING *pwszDevCertFileName
);

/* Retrieves revocation info filename */
void bdrm_env_revocation_filename (
		DRM_CONST_STRING *pwszFilename
    );

#else

/* get paths, names from oem layer, already exported 
 * */
extern const DRM_CONST_STRING g_dstrDrmHDS;
extern DRM_EXPORT_VAR DRM_CONST_STRING g_dstrDrmPath;
extern const DRM_CONST_STRING g_dstrDevcert;
extern const DRM_CONST_STRING g_dstrDevcerttemplate;
extern const DRM_CONST_STRING g_dstrHdsPasswordFile;
extern const DRM_CONST_STRING g_dstrPrivKey;

void _GetDevCertDir(DRM_CONST_STRING *path);
void _GetDevTemplateDir(DRM_CONST_STRING *path);
void _GetDevCertFilename(DRM_CONST_STRING *path);
void _GetDrmDevTemplateFilename(DRM_CONST_STRING *path);
void _GetDrmHdsPasswordFilename(DRM_CONST_STRING *path);
void _GetPrivateKeyFilename(DRM_CONST_STRING *path);
void _GetHDSFilename(DRM_CONST_STRING *path);

/* Retreive private key and device certificate file locations
*/
void bdrm_env_privcert ( 
	DRM_CONST_STRING *pwszPrivateDat,		/* [out] private key file location */ 
	DRM_CONST_STRING *pwszDeviceCertDat 	/* [out] device certificate file location */
	);

/* Retreive data store file location
*/
void bdrm_env_datastore ( 
    DRM_CONST_STRING *pwszDataStore 		/* [out] data store file location */
    );

/* Retreive environment path, used in all previous functions.
** This should default to '.' for most cases.
*/
void bdrm_env_path ( 
    DRM_CONST_STRING *pwszPath 			/* [out] environment dir location */
	);
#endif

#endif /* __DRMENV_H__ */


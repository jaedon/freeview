/*********************************************************************************
*     Copyright (c) 2011-2012, Broadcom Corporation
*     All Rights Reserved
*     Confidential Property of Broadcom Corporation
*
*   This program is the proprietary software of Broadcom Corporation and/or its licensors, 
*   and may only be used, duplicated, modified or distributed pursuant to the terms and 
*   conditions of a separate, written license agreement executed between you and Broadcom 
*   (an "Authorized License").  Except as set forth in an Authorized License, 
*   Broadcom grants no license (express or implied), right to use, or waiver of any kind 
*   with respect to the Software, and Broadcom expressly reserves all rights in and to the 
*   Software and all intellectual property rights therein.  
* 
*   IF YOU HAVE NO AUTHORIZED LICENSE, THEN YOU HAVE NO RIGHT TO USE THIS SOFTWARE IN ANY WAY, 
*   AND SHOULD IMMEDIATELY NOTIFY BROADCOM AND DISCONTINUE ALL USE OF THE SOFTWARE.
* 
*   Except as expressly set forth in the Authorized License,
*   1.     This program, including its structure, sequence and organization, constitutes the 
*       valuable trade secrets of Broadcom, and you shall use all reasonable efforts to protect 
*       the confidentiality thereof, and to use this information only in connection with your use 
*       of Broadcom integrated circuit products.
*   2.     TO THE MAXIMUM EXTENT PERMITTED BY LAW, THE SOFTWARE IS PROVIDED "AS IS" AND WITH ALL 
*       FAULTS AND BROADCOM MAKES NO PROMISES, REPRESENTATIONS OR WARRANTIES, EITHER EXPRESS, IMPLIED, 
*       STATUTORY, OR OTHERWISE, WITH RESPECT TO THE SOFTWARE.  BROADCOM SPECIFICALLY DISCLAIMS ANY AND 
*       ALL IMPLIED WARRANTIES OF TITLE, MERCHANTABILITY, NONINFRINGEMENT, FITNESS FOR A PARTICULAR PURPOSE, 
*       LACK OF VIRUSES, ACCURACY OR COMPLETENESS, QUIET ENJOYMENT, QUIET POSSESSION OR CORRESPONDENCE TO 
*       DESCRIPTION. YOU ASSUME THE ENTIRE RISK ARISING OUT OF USE OR PERFORMANCE OF THE SOFTWARE.
*   3.     TO THE MAXIMUM EXTENT PERMITTED BY LAW, IN NO EVENT SHALL BROADCOM OR ITS LICENSORS BE LIABLE 
*       FOR (i) CONSEQUENTIAL, INCIDENTAL, SPECIAL, INDIRECT, OR EXEMPLARY DAMAGES WHATSOEVER ARISING OUT 
*       OF OR IN ANY WAY RELATING TO YOUR USE OF OR INABILITY TO USE THE SOFTWARE EVEN IF BROADCOM HAS BEEN 
*       ADVISED OF THE POSSIBILITY OF SUCH DAMAGES; OR (ii) ANY AMOUNT IN EXCESS OF THE AMOUNT ACTUALLY PAID 
*       FOR THE SOFTWARE ITSELF OR U.S. $1, WHICHEVER IS GREATER. THESE LIMITATIONS SHALL APPLY NOTWITHSTANDING 
*       ANY FAILURE OF ESSENTIAL PURPOSE OF ANY LIMITED REMEDY.
*
*********************************************************************************/
#include "drm_data.h"

#include "config_inc/drm_playready.inc"
#include "config_inc/drm_rootfs.inc"

/************************** DRM KEY REGION DEFINITIONS BELOW **********************************/

char * bdrm_get_drm_key_region_tmp_dirname(void) {return drm_key_region_tmp_dir;}
char * bdrm_get_drm_bin_file_path(void) {return drm_bin_file_rootfs_path;}


/************************** PLAYREADY DEFINITIONS BELOW **********************************/

_DRM_SHORT_WCHAR_ * bdrm_get_pr_bin_fname(void) {return pr_bin_filename;}

_DRM_SHORT_WCHAR_ * bdrm_get_revocation_fname(void) {return pr_revoke_info;}
uint32_t  bdrm_get_revocation_fname_lgth(void) { return (sizeof(pr_revoke_info) / sizeof(pr_revoke_info[0])) -1;}

_DRM_SHORT_WCHAR_ * bdrm_get_cert_dir(void) {return  pr_certs_dir;}
uint32_t  bdrm_get_cert_dir_lgth(void) {return (sizeof(pr_certs_dir) / sizeof(pr_certs_dir[0])) -1;}

_DRM_SHORT_WCHAR_ * bdrm_get_cert_templates_dir(void) {return pr_cert_templates_dir;}
uint32_t  bdrm_get_cert_templates_dir_lgth(void) {return (sizeof(pr_cert_templates_dir) / sizeof(pr_cert_templates_dir[0])) -1;}

_DRM_SHORT_WCHAR_ * bdrm_get_hds_dir(void) {return pr_hds_dir;}
uint32_t  bdrm_get_hds_dir_lgth(void) {return (sizeof(pr_hds_dir) / sizeof(pr_hds_dir[0])) -1;}

_DRM_SHORT_WCHAR_ * bdrm_get_hds_fname(void) {return pr_hds_file;}
uint32_t  bdrm_get_hds_fname_lgth(void) {return (sizeof(pr_hds_file) / sizeof(pr_hds_file[0])) -1;}

_DRM_SHORT_WCHAR_ * bdrm_get_hds_pwd_fname(void) {return pr_hds_pw_file;}
uint32_t  bdrm_get_hds_pwd_fname_lgth(void) {return (sizeof(pr_hds_pw_file) / sizeof(pr_hds_pw_file[0])) -1;}

_DRM_SHORT_WCHAR_ * bdrm_get_dev_cert_fname(void) {return pr_dev_cert;}
uint32_t  bdrm_get_dev_cert_fname_lgth(void) {return (sizeof(pr_dev_cert) / sizeof(pr_dev_cert[0])) -1;}

_DRM_SHORT_WCHAR_ * bdrm_get_zdev_cert_fname(void) {return pr_zdev_cert;}
uint32_t  bdrm_get_zdev_cert_fname_lgth(void) {return (sizeof(pr_zdev_cert) / sizeof(pr_zdev_cert[0])) -1;}

_DRM_SHORT_WCHAR_ * bdrm_get_ndr_cert_fname(void) {return pr_ndr_cert;}
uint32_t  bdrm_get_ndr_cert_fname_lgth(void) {return (sizeof(pr_ndr_cert) / sizeof(pr_ndr_cert[0])) -1;}

_DRM_SHORT_WCHAR_ * bdrm_get_ndr_cert_template_fname(void) {return pr_ndr_cert_template;}
uint32_t  bdrm_get_ndr_cert_template_fname_lgth(void) {return (sizeof(pr_ndr_cert_template) / sizeof(pr_ndr_cert_template[0])) -1;}

_DRM_SHORT_WCHAR_ * bdrm_get_zgpriv_sig_key_fname(void) {return pr_zgpriv_sig_key;}
uint32_t  bdrm_get_zgpriv_sig_key_fname_lgth(void) {return (sizeof(pr_zgpriv_sig_key) / sizeof(pr_zgpriv_sig_key[0])) -1;}

_DRM_SHORT_WCHAR_ * bdrm_get_zgpriv_enc_key_fname(void) {return pr_zgpriv_enc_key;}
uint32_t  bdrm_get_zgpriv_enc_key_fname_lgth(void) {return (sizeof(pr_zgpriv_enc_key) / sizeof(pr_zgpriv_enc_key[0])) -1;}

_DRM_SHORT_WCHAR_ * bdrm_get_ndr_priv_key_fname(void) {return pr_ndr_priv_key;}
uint32_t  bdrm_get_ndr_priv_key_fname_lgth(void) {return (sizeof(pr_ndr_priv_key) / sizeof(pr_ndr_priv_key[0])) -1;}




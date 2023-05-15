/***************************************************************************
 *     Copyright (c) 2003-2010, Broadcom Corporation
 *     All Rights Reserved
 *     Confidential Property of Broadcom Corporation
 *
 *  THIS SOFTWARE MAY ONLY BE USED SUBJECT TO AN EXECUTED SOFTWARE LICENSE
 *  AGREEMENT  BETWEEN THE USER AND BROADCOM.  YOU HAVE NO RIGHT TO USE OR
 *  EXPLOIT THIS MATERIAL EXCEPT SUBJECT TO THE TERMS OF SUCH AN AGREEMENT.
 *
 * $brcm_Workfile: bxdm_pp_ver.h $
 * $brcm_Revision: Hydra_Software_Devel/1 $
 * $brcm_Date: 2/16/10 10:52a $
 *
 * [File Description:]
 *
 * Revision History:
 *
 * $brcm_Log: /magnum/commonutils/xdm/bxdm_pp_ver.h $
 * 
 * Hydra_Software_Devel/1   2/16/10 10:52a nilesh
 * SW7405-2993: Initial XDM version
 *
 ***************************************************************************/

#ifndef bxdm_pp_VER_H__
#define bxdm_pp_VER_H__

/* DM version is split into MAJOR.MINOR.SUBMINOR.
 *
 * MAJOR should be incremented when changes are made that could break
 * existing applications.  Major re-writes, changes to default
 * accepted behavior, etc would cause the MAJOR number fo increase
 *
 * MINOR should be incremented when features are added that are
 * backwards compatible with existing applications
 *
 * SUBMINOR should be incremented when bug fixes are made to existing
 * functionality
 *
 */
#define BXDM_PictureProvider_P_VERSION_MAJOR    3
#define BXDM_PictureProvider_P_VERSION_MINOR    0
#define BXDM_PictureProvider_P_VERSION_SUBMINOR 0

#endif /* #ifndef bxdm_pp_VER_H__ */

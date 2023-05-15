/***************************************************************************
 *     Copyright (c) 2003-2008, Broadcom Corporation
 *     All Rights Reserved
 *     Confidential Property of Broadcom Corporation
 *
 *  THIS SOFTWARE MAY ONLY BE USED SUBJECT TO AN EXECUTED SOFTWARE LICENSE
 *  AGREEMENT  BETWEEN THE USER AND BROADCOM.  YOU HAVE NO RIGHT TO USE OR
 *  EXPLOIT THIS MATERIAL EXCEPT SUBJECT TO THE TERMS OF SUCH AN AGREEMENT.
 *
 * $brcm_Workfile: bhdcplib_keyloader.h $
 * $brcm_Revision: Hydra_Software_Devel/2 $
 * $brcm_Date: 3/18/08 7:18p $
 *
 * Module Description:
 *
 * Revision History:
 *
 * $brcm_Log: /magnum/syslib/hdcplib/7401/bhdcplib_keyloader.h $
 * 
 * Hydra_Software_Devel/2   3/18/08 7:18p vle
 * PR 39991: Rename HDMIlib to HDCPlib. Remove unused implementation.
 * 
 * Hydra_Software_Devel/1   2/26/08 5:43p jgarrett
 * PR 39991: Renaming new HDMlib to HDCPlib
 * 
 * Hydra_Software_Devel/Syslib_Initiative/2   1/31/08 10:35p vle
 * PR 31924: Add BHDCPLIB_FastLoadEncryptedHdcpKeys
 * 
 * Hydra_Software_Devel/Syslib_Initiative/1   1/4/08 6:50p vle
 * PR 31924: HDMI Tx Syslib Support
 * 
 * Hydra_Software_Devel/6   7/11/07 6:25p rgreen
 * PR22187:Remove invalid/unused extern references
 * 
 * Hydra_Software_Devel/5   8/11/06 3:49p rgreen
 * PR22187: Fix HDCP Key Loader to use standalone HSM PI
 * 
 * Hydra_Software_Devel/4   8/8/06 6:39p rgreen
 * PR22187: Add support for standalone HSM PI for 7401
 * 
 ***************************************************************************/

#ifndef BHDCPLIB_KEYLOADER__
#define BHDCPLIB_KEYLOADER__

#include "bstd.h"
#include "bavc_hdmi.h"
#include "bhdcplib.h"
#include "bhdcplib_priv.h"


BERR_Code BHDCPlib_FastLoadEncryptedHdcpKeys(BHDCPlib_Handle hHDCPlib);


BERR_Code BHDCPlib_GetKeySet(BAVC_HDMI_HDCP_KSV pTxAksv, BHDCPlib_EncryptedHdcpKeyStruct * pHdcpKeys);


#endif /* BHDCPLIB_KEYLOADER_H */


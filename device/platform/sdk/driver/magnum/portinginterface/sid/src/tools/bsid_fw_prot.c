/***************************************************************************
 *	   Copyright (c) 2003-2012, Broadcom Corporation
 *	   All Rights Reserved
 *	   Confidential Property of Broadcom Corporation
 *
 *	THIS SOFTWARE MAY ONLY BE USED SUBJECT TO AN EXECUTED SOFTWARE LICENSE
 *	AGREEMENT  BETWEEN THE USER AND BROADCOM.  YOU HAVE NO RIGHT TO USE OR
 *	EXPLOIT THIS MATERIAL EXCEPT SUBJECT TO THE TERMS OF SUCH AN AGREEMENT.
 *
 * $brcm_Workfile: bsid_fw_prot.c $
 * $brcm_Revision: Hydra_Software_Devel/1 $
 * $brcm_Date: 1/20/12 2:05p $
 *
 * Module Description:
 *
 * Revision History:
 *
 * $brcm_Log: /magnum/portinginterface/sid/7425/tools/bsid_fw_prot.c $
 * 
 * Hydra_Software_Devel/1   1/20/12 2:05p fbasso
 * SW7425-1619: release SID multi channel interface to
 * Hydra_Software_Devel
 * 
 * Hydra_Software_Devel/1   3/3/08 2:09p vobadm
 * PR38951: Add sid to CC
 * Added file element "arc".
 * Added file element "bsid_fw_prot.c".
 * Added file element "sid.hex".
 * Added file element "sid_pi.doc".
 * Added file element "srec".
 * PR38951: Add sid to CC
 * Added file element "bsid.c".
 * Added file element "bsid.h".
 * Added file element "bsid_err.h".
 * Added file element "bsid_fw.c".
 * Added file element "bsid_img.c".
 * Added file element "bsid_img.h".
 * Added file element "bsid_msg.c".
 * Added file element "bsid_msg.h".
 * Added file element "bsid_priv.c".
 * Added file element "bsid_priv.h".
 * Added directory element "tools".
 * 
 * bdvd_v10/1   2/23/07 9:37a fbasso
 * SID PI: code clean up. In GetStreamInfo returning color key information
 * for PNG images; SID UOD: updated code to reflect changes to SID
 * interface;
 * 
 * bdvd_v10/fbasso_cleanup_000/1   2/15/07 1:48p fbasso
 * Added tools to convert .hex to .c
 *
 ***************************************************************************/

#include "bstd.h"
#include "bsid_img.h"


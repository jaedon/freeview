/***************************************************************************
 *     Copyright (c) 2003-2010, Broadcom Corporation
 *     All Rights Reserved
 *     Confidential Property of Broadcom Corporation
 *
 *  THIS SOFTWARE MAY ONLY BE USED SUBJECT TO AN EXECUTED SOFTWARE LICENSE
 *  AGREEMENT  BETWEEN THE USER AND BROADCOM.  YOU HAVE NO RIGHT TO USE OR
 *  EXPLOIT THIS MATERIAL EXCEPT SUBJECT TO THE TERMS OF SUCH AN AGREEMENT.
 *
 * $brcm_Workfile: bvbilib_dccparse_dss.h $
 * $brcm_Revision: Hydra_Software_Devel/3 $
 * $brcm_Date: 10/21/10 4:37p $
 *
 * Module Description:
 *
 * Revision History:
 *
 * $brcm_Log: /magnum/syslib/vbilib/bvbilib_dccparse_dss.h $
 * 
 * Hydra_Software_Devel/3   10/21/10 4:37p darnstein
 * SW7401-2571: cut over to the DSS userdata parser that DSS customer
 * engineers perfer.
 * 
 * Hydra_Software_Devel/2   10/21/10 2:44p darnstein
 * SW7401-2571: preliminary update to DSS parser. More work is needed.
 * 
 * Hydra_Software_Devel/1   10/18/10 6:18p darnstein
 * SW3548-3022: Bring DSS work of BIll Hsu and Richard Li to the main
 * Hydra_Software_Devel branch of ClearCase.
 * 
 * SanJose_CDI_Devel/1   1/15/08 5:28p bhsu
 * PR38704: Added Support CC Subtitle
 ***************************************************************************/

/*= Module Overview *********************************************************
<verbatim>

Overview
This software has been moved to the BUDP commonutils module.

</verbatim>
***************************************************************************/

#ifndef BVBILIBDCCPARSEDSS_H__
#define BVBILIBDCCPARSEDSS_H__

#include "budp_dccparse_dss.h"

typedef enum {
	BVBIlib_DCCparse_CC_Dss_Type_Undefined = 
		BUDP_DCCparse_CC_Dss_Type_Undefined,
	BVBIlib_DCCparse_CC_Dss_Type_ClosedCaption = 
		BUDP_DCCparse_CC_Dss_Type_ClosedCaption,
	BVBIlib_DCCparse_CC_Dss_Type_Subtitle = 
		BUDP_DCCparse_CC_Dss_Type_Subtitle
} BVBIlib_DCCparse_Dss_CC_Type;

#define BVBIlib_DCCparse_dss_cc_subtitle BUDP_DCCparse_dss_cc_subtitle

#define BVBIlib_DCCparse_DSS BUDP_DCCparse_DSS 

#endif /* BVBILIBDCCPARSEDSS_H__ */


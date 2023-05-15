/***************************************************************************
 *     Copyright (c) 2004-2007, Broadcom Corporation
 *     All Rights Reserved
 *     Confidential Property of Broadcom Corporation
 *
 *  THIS SOFTWARE MAY ONLY BE USED SUBJECT TO AN EXECUTED SOFTWARE LICENSE
 *  AGREEMENT  BETWEEN THE USER AND BROADCOM.  YOU HAVE NO RIGHT TO USE OR
 *  EXPLOIT THIS MATERIAL EXCEPT SUBJECT TO THE TERMS OF SUCH AN AGREEMENT.
 *
 * $brcm_Workfile: memview.h $s
 * $brcm_Revision: Hydra_Software_Devel/1 $
 * $brcm_Date: 10/15/07 9:34a $
 *
 * Module Description:
 *
 * Revision History:
 *
 * $brcm_Log: /rockford/unittests/portinginterface/vdc/memview.h $
 * 
 * Hydra_Software_Devel/1   10/15/07 9:34a rpan
 * PR34857: Ported from mvd_vdc_test.
 * 
 * Hydra_Software_Devel/1   4/12/07 7:30p pntruong
 * PR29806: Write out capture memory for viewing with MemView tool.
 *
 ***************************************************************************/
#include "bstd.h"
#include "bsur.h"


/***************************************************************************
 *
 */
bool BTST_DumpToMemViewFile
	( BSUR_Surface_Handle              hCapMem,
	  const char                      *pchMemFname );

/* end of file */

/***************************************************************************
 *     Copyright (c) 2003-2010, Broadcom Corporation
 *     All Rights Reserved
 *     Confidential Property of Broadcom Corporation
 *
 *  THIS SOFTWARE MAY ONLY BE USED SUBJECT TO AN EXECUTED SOFTWARE LICENSE
 *  AGREEMENT  BETWEEN THE USER AND BROADCOM.  YOU HAVE NO RIGHT TO USE OR
 *  EXPLOIT THIS MATERIAL EXCEPT SUBJECT TO THE TERMS OF SUCH AN AGREEMENT.
 *
 * $brcm_Workfile: bvbilib_dccparse.h $
 * $brcm_Revision: Hydra_Software_Devel/26 $
 * $brcm_Date: 10/21/10 4:36p $
 *
 * Module Description:
 *
 * Revision History:
 *
 * $brcm_Log: /magnum/syslib/vbilib/bvbilib_dccparse.h $
 * 
 * Hydra_Software_Devel/26   10/21/10 4:36p darnstein
 * SW7401-2571: cut over to the DSS userdata parser that DSS customer
 * engineers perfer.
 * 
 * Hydra_Software_Devel/25   8/27/10 1:51p darnstein
 * SW3548-3022: merge to production branch.
 * 
 * Hydra_Software_Devel/SW3548-3022/2   8/2/10 1:28p darnstein
 * SW3548-3022: #include'd another file for backwards compatibiliby.
 * 
 * Hydra_Software_Devel/SW3548-3022/1   7/28/10 4:38p darnstein
 * SW3548-3022: bitstream parsing is relayed to commonutils module BUDP.
 * 
 * Hydra_Software_Devel/24   3/24/10 3:17p darnstein
 * SW7405-4084: add function BVBIlib_DCCparse_Divicom() to parse Divicom
 * closed caption data.
 * 
 * Hydra_Software_Devel/23   12/14/09 3:58p darnstein
 * SW7405-3527: There are now two SEI parsers. One is for legacy
 * customers. The other is for up to data, ATSC-53 style data.
 * 
 * Hydra_Software_Devel/22   10/26/07 10:50a ptimariu
 * PR36451: closing bracket, __cplusplus
 * 
 * Hydra_Software_Devel/22   10/25/07 6:35p ptimariu
 * PR36451: closing bracket, __cplusplus
 * 
 * Hydra_Software_Devel/22   10/25/07 6:31p ptimariu
 * PR36451: closing bracket, __cplusplus
 * 
 * Hydra_Software_Devel/21   9/11/07 5:17p darnstein
 * PR25708: First release of SCTE encoder software.
 * 
 * Hydra_Software_Devel/20   6/6/07 12:50p darnstein
 * PR30411: multi-line closed caption encoding is ready for test.
 * 
 * Hydra_Software_Devel/19   12/6/06 1:55p gmohile
 * PR 25904: add support for AFD parsing
 * 
 * Hydra_Software_Devel/19   12/6/06 1:50p gmohile
 * PR 25904: add support for parsing AFD
 * 
 * Hydra_Software_Devel/19   12/6/06 12:47p gmohile
 * PR 25904: add support for parsing AFD
 * 
 * Hydra_Software_Devel/19   12/6/06 10:22a gmohile
 * PR 25904: add support for parsing AFD
 * 
 * Hydra_Software_Devel/18   3/20/06 3:34p darnstein
 * PR20137: Return data items even if they have cc_valid=0. Also, return
 * the information cc_valid=0/1 to the caller of BVBIlib_dccparse(). All
 * this is needed to use the data for EIA-708B processing.
 * 
 * Hydra_Software_Devel/17   8/15/05 4:15p darnstein
 * PR16057: Remove reference to a BMVD header file.
 * 
 * Hydra_Software_Devel/16   6/8/05 3:10p darnstein
 * PR 15784: Can now parse DSS/AVC/SEI userdata.  Lightly tested. Is the
 * top/bottom field decision correct?
 * 
 * Hydra_Software_Devel/15   5/31/05 2:33p darnstein
 * PR 15707: Adapt to new data structure BAVC_USERDATA_info.
 * 
 * Hydra_Software_Devel/15   5/31/05 2:30p darnstein
 * PR 15707: Adapt to new data structure BAVC_USERDATA_info.
 * 
 * Hydra_Software_Devel/14   5/27/05 4:38p vsilyaev
 * PR14397: Accented that BMVD_USERDATA parameter isn't modified
 * 
 * Hydra_Software_Devel/13   4/11/05 12:42p darnstein
 * PR 14477: fix coding errors discovered by Biyong's customer.
 * 
 * Hydra_Software_Devel/12   4/6/05 5:12p darnstein
 * PR 14477: use a new attribute bIsAnalog to distinguish analog closed
 * caption data from more general (digital) forms.
 * 
 * Hydra_Software_Devel/11   4/6/05 3:30p darnstein
 * PR 14477: Parse DVS 157 and DVS053 data according to cc_field_number.
 * 
 * Hydra_Software_Devel/10   1/14/05 4:22p darnstein
 * PR 13798:  Add Bill Fassl's parsing capability as a separate function.
 * 
 * Hydra_Software_Devel/9   12/14/04 12:45p darnstein
 * PR 13504:  Take out prototype of function BVBIlib_DCCparse_ccdata().
 * It is not implemented, and a different calling sequence is required
 * anyway.
 * 
 * Hydra_Software_Devel/8   12/14/04 12:43p darnstein
 * PR 13504: Provide userdata type to caller.  May help to resolve this
 * PR.
 * 
 * Hydra_Software_Devel/7   12/6/04 4:57p darnstein
 * PR 13248: Improve a comment block.
 * 
 * Hydra_Software_Devel/6   12/6/04 2:28p darnstein
 * PR 13248: Use a simple method for determining whether to output CC
 * bytes top or bottom field.
 * 
 * Hydra_Software_Devel/5   11/16/04 7:58p shyam
 * PR 8365 : Added CC type in parse module
 * 
 * Hydra_Software_Devel/4   10/18/04 5:11p darnstein
 * PR 12874: remove an unnecessary #include file from API header file
 * bvbilibdccparse.h.
 * 
 * Hydra_Software_Devel/3   10/18/04 5:06p darnstein
 * PR 12874: Restore the calling sequence of BVBILIB_DCCparse() to avoid
 * giving problems to Broadcom customers.
 * 
 * Hydra_Software_Devel/2   10/12/04 11:39a darnstein
 * PR 12874: Only access MPEG userdata on 32-bit address boundaries.
 * Otherwise, userdata seems to read as 0xff.  I do not know why.  Note
 * that the API of this sub-module had to change.
 * 
 * Hydra_Software_Devel/1   4/20/04 2:10p darnstein
 * PR 9080: ready for initial testing.
 * 
 ***************************************************************************/

/*= Module Overview *********************************************************
<verbatim>

Overview
This software has been moved to the BUDP commonutils module.

</verbatim>
***************************************************************************/

#ifndef BVBILIBDCCPARSE_H__
#define BVBILIBDCCPARSE_H__

#include "budp_dccparse.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef enum {
	BVBIlib_DCCparse_Format_Unknown = BUDP_DCCparse_Format_Unknown,
	BVBIlib_DCCparse_Format_DVS157  = BUDP_DCCparse_Format_DVS157,
	BVBIlib_DCCparse_Format_ATSC53  = BUDP_DCCparse_Format_ATSC53,
	BVBIlib_DCCparse_Format_DVS053  = BUDP_DCCparse_Format_DVS053,
	BVBIlib_DCCparse_Format_SEI     = BUDP_DCCparse_Format_SEI,
	BVBIlib_DCCparse_Format_SEI2    = BUDP_DCCparse_Format_SEI2,
	BVBIlib_DCCparse_Format_AFD53   = BUDP_DCCparse_Format_AFD53,
#if defined(HUMAX_PLATFORM_BASE) && defined(CONFIG_BRCM_PATCH_TRANSCODING_32BIT_TIMESTAMP) /*Timestamp*/
	BVBIlib_DCCparse_Format_LAST    = BUDP_DCCparse_Format_LAST
#else
	BVBIlib_DCCparse_Format_Divicom = BUDP_DCCparse_Format_Divicom
#endif
}
BVBIlib_DCCparse_Format;

/* Data structure */
#define BVBIlib_DCCparse_ccdata BUDP_DCCparse_ccdata

/* Functions */
#define BVBIlib_DCCparse BUDP_DCCparse
#define BVBIlib_DCCparse_SEI BUDP_DCCparse_SEI
#if defined(HUMAX_PLATFORM_BASE) && defined(CONFIG_BRCM_PATCH_TRANSCODING_32BIT_TIMESTAMP) /*Timestamp*/
/* NONE */
#else
#define BVBIlib_DCCparse_Divicom BUDP_DCCparse_Divicom
#endif
#ifdef __cplusplus
}
#endif

#endif /* BVBILIBDCCPARSE_H__ */

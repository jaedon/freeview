/***************************************************************************
 *     Copyright (c) 2003-2010, Broadcom Corporation
 *     All Rights Reserved
 *     Confidential Property of Broadcom Corporation
 *
 *  THIS SOFTWARE MAY ONLY BE USED SUBJECT TO AN EXECUTED SOFTWARE LICENSE
 *  AGREEMENT  BETWEEN THE USER AND BROADCOM.  YOU HAVE NO RIGHT TO USE OR
 *  EXPLOIT THIS MATERIAL EXCEPT SUBJECT TO THE TERMS OF SUCH AN AGREEMENT.
 *
 * $brcm_Workfile: bvbilib_dccparse.c $
 * $brcm_Revision: Hydra_Software_Devel/46 $
 * $brcm_Date: 8/27/10 1:49p $
 *
 * Module Description:
 *
 * Revision History:
 *
 * $brcm_Log: /magnum/syslib/vbilib/bvbilib_dccparse.c $
 * 
 * Hydra_Software_Devel/46   8/27/10 1:49p darnstein
 * SW3548-3022: merge to production branch.
 * 
 * Hydra_Software_Devel/SW3548-3022/1   7/28/10 4:38p darnstein
 * SW3548-3022: bitstream parsing is relayed to commonutils module BUDP.
 * 
 * Hydra_Software_Devel/45   12/14/09 3:57p darnstein
 * SW7405-3527: There are now two SEI parsers. One is for legacy
 * customers. The other is for up to data, ATSC-53 style data.
 * 
 * Hydra_Software_Devel/44   3/18/09 7:52p darnstein
 * PR53264: fix logic error in previous check-in.
 * 
 * Hydra_Software_Devel/43   3/16/09 1:08p darnstein
 * PR53264: New error code for MPEG parsing: BERR_BVBIlib_SOURCE_CHECK.
 * 
 * Hydra_Software_Devel/42   5/28/08 3:59p darnstein
 * PR42839: When parsing AFD53 data, fill in the descriptors cc_valid,
 * bIsAnalog, and polarity. The other types of data have these fields
 * defined. So applications software can be made simpler now.
 * 
 * Hydra_Software_Devel/41   11/9/07 11:04a darnstein
 * PR36734: In last checkin, I forgot to initialize a function argument.
 * 
 * Hydra_Software_Devel/40   11/8/07 3:53p darnstein
 * PR36734: When parsing AFD data, report the number of items parsed (1)
 * to the caller.
 * 
 * Hydra_Software_Devel/39   9/26/07 1:29p darnstein
 * PR35298: A counting error is fixed. This may or may not be a
 * contributing factor to this PR.
 * 
 * Hydra_Software_Devel/38   8/30/07 4:30p darnstein
 * PR33711: declare static tables to be const. This is required by
 * Broadcom's programming standards.
 * 
 * Hydra_Software_Devel/37   6/14/07 3:41p darnstein
 * PR28732: Fix parsing for case of ATSC-53 encoding, MPEG FIELD PICTURES.
 * There was speculation that this parsing error was a factor in the PR.
 * Perhaps not. But in any case, the parsing was incorrect.
 * 
 * Hydra_Software_Devel/36   12/6/06 1:55p gmohile
 * PR 25904: add support for parsing AFD
 * 
 * Hydra_Software_Devel/35   10/17/06 12:46p darnstein
 * PR24970: These changes capture most of what Bill Fassl and Abhay Gadre
 * asked for. With minor changes in applications software, all test
 * bitstreams should decode properly now.
 * 
 * Hydra_Software_Devel/34   10/9/06 2:49p darnstein
 * PR24702: Support user_data_code_length in both compliant bitstreams,
 * and older, non-compliant bitstreams.
 * 
 * Hydra_Software_Devel/33   9/6/06 7:07p darnstein
 * PR24125: Adjust interpretation of user_data_code_length, per customer
 * experts.
 * 
 * Hydra_Software_Devel/32   4/28/06 4:09p darnstein
 * PR21293: Check for DSS userdatalength != 3.
 * 
 * Hydra_Software_Devel/31   3/20/06 3:35p darnstein
 * PR20137: Return data items even if they have cc_valid=0. Also, return
 * the information cc_valid=0/1 to the caller of BVBIlib_dccparse(). All
 * this is needed to use the data for EIA-708B processing.
 * 
 * Hydra_Software_Devel/30   2/27/06 12:04p darnstein
 * PR15767: The last revision is effectively reversed. The test bitstream
 * dcii.ts will be "broken" now. It is not possible to successfully
 * decode all bitstreams with one algorithm.
 * 
 * Hydra_Software_Devel/29   2/10/06 5:34p darnstein
 * PR15767: for ATSC data, use the _TFF method instead of the _SIMPLE
 * method. This takes care of dcii.ts. I hope that other bitstreams
 * continue to decode correctly...
 * 
 * Hydra_Software_Devel/28   1/16/06 12:58p darnstein
 * PR18903: silence compiler's complaint about an unused function via a
 * kludge of sorts.
 * 
 * Hydra_Software_Devel/27   1/13/06 5:44p darnstein
 * PR18540: Fix special case of parsing SEI data (field polarity). Add
 * some debugging support.
 * 
 * Hydra_Software_Devel/26   10/31/05 3:26p darnstein
 * PR16086: For ATSC data, switch to the method SetFieldsParity_SIMPLE().
 * This causes the test file brutus4.mpg to decode properly.
 * 
 * Hydra_Software_Devel/25   10/11/05 6:12p darnstein
 * PR15959: Provide multiple methods for setting field parity of closed
 * caption data.
 * 
 * Hydra_Software_Devel/24   6/16/05 10:37a erickson
 * PR15767: fixed SetFieldsParity for 3:2 pulldown streams
 * 
 * Hydra_Software_Devel/23   6/8/05 3:10p darnstein
 * PR 15784: Can now parse DSS/AVC/SEI userdata.  Lightly tested. Is the
 * top/bottom field decision correct?
 * 
 * Hydra_Software_Devel/22   6/6/05 12:47p darnstein
 * PR 15767: Use Vladimir's formula for setting top field or bottom field
 * in ATSC userdata types.
 * 
 * Hydra_Software_Devel/21   5/31/05 2:30p darnstein
 * PR 15707: Adapt to new data structure BAVC_USERDATA_info.
 * 
 * Hydra_Software_Devel/20   5/27/05 4:37p vsilyaev
 * PR14397: Accented that BMVD_USERDATA parameter isn't modified
 * 
 * Hydra_Software_Devel/19   4/11/05 12:42p darnstein
 * PR 14477: fix coding errors discovered by Biyong's customer.
 * 
 * Hydra_Software_Devel/18   4/6/05 5:12p darnstein
 * PR 14477: use a new attribute bIsAnalog to distinguish analog closed
 * caption data from more general (digital) forms.
 * 
 * Hydra_Software_Devel/17   4/6/05 3:30p darnstein
 * PR 14477: Parse DVS 157 and DVS053 data according to cc_field_number.
 * 
 * Hydra_Software_Devel/16   3/22/05 1:51p darnstein
 * PR 14504: Arman's speed optimizations have been applied.
 * 
 * Hydra_Software_Devel/15   1/25/05 2:20p darnstein
 * PR 10907: Allow operation on big-endian machines.  Will be tested by
 * Bill Fassl.
 * 
 * Hydra_Software_Devel/14   1/19/05 3:22p darnstein
 * PR 13798: Incorporate Bill Fassl's latest changes for DSS parsing.
 * Also include a minor optimization from Bill.
 * 
 * Hydra_Software_Devel/13   1/14/05 4:22p darnstein
 * PR 13798:  Add Bill Fassl's parsing capability as a separate function.
 * 
 * Hydra_Software_Devel/12   1/14/05 11:58a darnstein
 * PR 13797: Allow read from address that is not a multiple of 4.  Fix
 * counting error that Bill Fassl discovered.
 * 
 * Hydra_Software_Devel/11   12/20/04 1:39p darnstein
 * PR 13030: Adapt to changed file names.
 * 
 * Hydra_Software_Devel/10   12/14/04 12:43p darnstein
 * PR 13504: Provide userdata type to caller.  May help to resolve this
 * PR.
 * 
 * Hydra_Software_Devel/9   12/6/04 2:27p darnstein
 * PR 13248: Use a simple method for determining whether to output CC
 * bytes top or bottom field.
 * 
 * Hydra_Software_Devel/8   11/16/04 8:01p shyam
 * PR 8365 : Removed Parity bit clear in VBIlibparser
 * 
 * Hydra_Software_Devel/7   10/18/04 5:11p darnstein
 * PR 12874: remove an unnecessary #include file from API header file
 * bvbilibdccparse.h.
 * 
 * Hydra_Software_Devel/6   10/18/04 5:06p darnstein
 * PR 12874: Restore the calling sequence of BVBILIB_DCCparse() to avoid
 * giving problems to Broadcom customers.
 * 
 * Hydra_Software_Devel/5   10/12/04 11:39a darnstein
 * PR 12874: Only access MPEG userdata on 32-bit address boundaries.
 * Otherwise, userdata seems to read as 0xff.  I do not know why.  Note
 * that the API of this sub-module had to change.
 * 
 * Hydra_Software_Devel/4   6/8/04 5:01p darnstein
 * PR 9080: fix a byte counting error.
 * 
 * Hydra_Software_Devel/3   4/26/04 8:34p darnstein
 * PR 9080: Fix numerous small bugs.  The implementation seems to be
 * correct for ATSC-53 digital closed caption data.
 * 
 * Hydra_Software_Devel/2   4/21/04 1:54p darnstein
 * PR 9080: adapt to changes in data structure BMVD_USERDATA_info.
 * 
 * Hydra_Software_Devel/1   4/20/04 2:09p darnstein
 * PR 9080: ready for initial testing.
 * 
 ***************************************************************************/

/* Software has been moved to the BUDP commonutils module */

/* End of File */

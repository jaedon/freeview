/***************************************************************************
 *     Copyright (c) 2003-2013, Broadcom Corporation
 *     All Rights Reserved
 *     Confidential Property of Broadcom Corporation
 *
 *  THIS SOFTWARE MAY ONLY BE USED SUBJECT TO AN EXECUTED SOFTWARE LICENSE
 *  AGREEMENT  BETWEEN THE USER AND BROADCOM.  YOU HAVE NO RIGHT TO USE OR
 *  EXPLOIT THIS MATERIAL EXCEPT SUBJECT TO THE TERMS OF SUCH AN AGREEMENT.
 *
 * $brcm_Workfile: bvce_output.h $
 * $brcm_Revision: Hydra_Software_Devel/4 $
 * $brcm_Date: 4/18/13 3:28p $
 *
 * [File Description:]
 *
 * Revision History:
 *
 * $brcm_Log: /magnum/portinginterface/vce/src/bvce_output.h $
 * 
 * Hydra_Software_Devel/4   4/18/13 3:28p nilesh
 * SW7425-4614: Fixed deadlock when CABAC is not ready and the CDB/ITB is
 * full from previous unclean stop
 * 
 * Hydra_Software_Devel/3   4/3/13 2:39p nilesh
 * SW7425-4614: Added support for flushing when ITB/CDB accessed directly
 * 
 * Hydra_Software_Devel/2   3/21/13 12:41p nilesh
 * SW7425-4614: Fixed race condition between app calling Start/FlushEncode
 * and the mux thread calling GetBuffer/ConsumeBuffer using mutexes.
 * Cleaned up interface between output and channel modules.  Fixed
 * problem with flushed descriptors causin CABAC ready to be set
 * prematurely.
 * 
 * Hydra_Software_Devel/1   10/1/12 2:03p nilesh
 * SW7445-39: Split power and output code into separate files
 *
 ***************************************************************************/

#ifndef BVCE_OUTPUT_H_
#define BVCE_OUTPUT_H_

#ifdef __cplusplus
extern "C" {
#endif

BERR_Code
BVCE_Output_Reset(
         BVCE_Output_Handle hVceOutput
         );

void
BVCE_Output_Flush(
   BVCE_Output_Handle hVceOutput
   );

#ifdef __cplusplus
}
#endif

#endif /* BVCE_OUTPUT_H_ */
/* End of File */

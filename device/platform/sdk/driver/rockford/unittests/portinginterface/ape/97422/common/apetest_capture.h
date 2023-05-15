/***************************************************************************
 *     Copyright (c) 2003-2012, Broadcom Corporation
 *     All Rights Reserved
 *     Confidential Property of Broadcom Corporation
 *
 *  THIS SOFTWARE MAY ONLY BE USED SUBJECT TO AN EXECUTED SOFTWARE LICENSE
 *  AGREEMENT  BETWEEN THE USER AND BROADCOM.  YOU HAVE NO RIGHT TO USE OR
 *  EXPLOIT THIS MATERIAL EXCEPT SUBJECT TO THE TERMS OF SUCH AN AGREEMENT.
 *
 * $brcm_Workfile: apetest_capture.h $
 * $brcm_Revision: Hydra_Software_Devel/2 $
 * $brcm_Date: 8/30/12 4:47p $
 *
 * Captureback testing library. 
 *
 * Revision History:
 *
 * $brcm_Log: /rockford/unittests/portinginterface/ape/97422/common/apetest_capture.h $
 * 
 * Hydra_Software_Devel/2   8/30/12 4:47p jgarrett
 * SW7425-3629: Fixes for cached access
 * 
 * Hydra_Software_Devel/1   7/10/12 9:25a jgarrett
 * SW7425-3457: Adding ape_decode application
 * 
 * Hydra_Software_Devel/2   11/2/10 1:42p gmullen
 * SW7425-15: Fixed compile issue
 * 
 * Hydra_Software_Devel/1   10/25/10 10:28a gmullen
 * SW7422-20: Added testapp files
 * 
 * Hydra_Software_Devel/2   8/31/07 10:28a gmullen
 * PR15309: Updated tests.
 * 
 * Hydra_Software_Devel/5   5/15/07 8:23a gmullen
 * PR29510: Updated tests for PR 29510
 * 
 * Hydra_Software_Devel/4   5/10/07 3:33p gmullen
 * PR15309: Updated for new RAVE and playback tests.
 * 
 * Hydra_Software_Devel/3   3/8/07 5:47p gmullen
 * PR15309: Added playback with timestamp test.
 * 
 * Hydra_Software_Devel/2   11/23/05 9:23a gmullen
 * PR15309: Added verification support,.
 * 
 * Hydra_Software_Devel/1   9/21/05 2:21p gmullen
 * PR15309: Added tests for playback and changed RAVE tests for new PI.
 * 
 * 
 ***************************************************************************/

#ifndef APETEST_CAPTURE_H__
#define APETEST_CAPTURE_H__

#ifdef __cplusplus
extern "C" {
#endif
  
#include "bape.h"

typedef struct Capture_Vars * APETEST_Capture_Handle;

BERR_Code APETEST_Capture_Open ( BMEM_Handle mem, BAPE_Handle ape, APETEST_Capture_Handle * phCapture, BAPE_MixerHandle handle );
void      APETEST_Capture_Close( APETEST_Capture_Handle hCapture );
BERR_Code APETEST_Capture_Start( APETEST_Capture_Handle hCapture, const char *FileName );
BERR_Code APETEST_Capture_Stop ( APETEST_Capture_Handle hCapture );

#ifdef __cplusplus
}
#endif

#endif /* #ifndef APETEST_CAPTURE_H__ */

/* end of file */

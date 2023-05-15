/***************************************************************************
 *     Copyright (c) 2003-2012, Broadcom Corporation
 *     All Rights Reserved
 *     Confidential Property of Broadcom Corporation
 *
 *  THIS SOFTWARE MAY ONLY BE USED SUBJECT TO AN EXECUTED SOFTWARE LICENSE
 *  AGREEMENT  BETWEEN THE USER AND BROADCOM.  YOU HAVE NO RIGHT TO USE OR
 *  EXPLOIT THIS MATERIAL EXCEPT SUBJECT TO THE TERMS OF SUCH AN AGREEMENT.
 *
 * $brcm_Workfile: apetest_pblib.h $
 * $brcm_Revision: Hydra_Software_Devel/1 $
 * $brcm_Date: 7/10/12 9:25a $
 *
 * Playback testing library. 
 *
 * Revision History:
 *
 * $brcm_Log: /rockford/unittests/portinginterface/ape/97422/common/apetest_pblib.h $
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

#ifndef APETEST_PLAYBACKLIB_H__
#define APETEST_PLAYBACKLIB_H__

#ifdef __cplusplus
extern "C" {
#endif
  
#include "bxpt.h"
#include "bxpt_playback.h"
#include "bxpt_directv_playback.h"

typedef struct PbLib_Vars * APETEST_PbLib_Handle;

APETEST_PbLib_Handle APETEST_PbLib_Init( BXPT_Handle hXpt, unsigned PbChannelNum );
void APETEST_PbLib_Shutdown(APETEST_PbLib_Handle hPbLib );
BXPT_Playback_Handle APETEST_PbLib_GetPbHandle( APETEST_PbLib_Handle hPbLib );
BERR_Code APETEST_PbLib_DoPlay( APETEST_PbLib_Handle hPbLib, char *FileName, unsigned int SyncMode );
BERR_Code APETEST_PbLib_DoEsPlay( APETEST_PbLib_Handle hPbLib, char *FileName );
BERR_Code APETEST_PbLib_DoPlayNonBlocking( APETEST_PbLib_Handle hPbLib, char *FileName,	unsigned int SyncMode );
BERR_Code APETEST_PbLib_StopPlay( APETEST_PbLib_Handle hPbLib );
BERR_Code APETEST_PbLib_DoEsPlayNonBlocking( APETEST_PbLib_Handle hPbLib, char *FileName );

/* notice not all have been tested after pulling over from xpt pb_lib 
        May or May not work ...
BERR_Code APETEST_PbLib_DoRepeatingPlay( APETEST_PbLib_Handle hPbLib, char *FileName, unsigned int SyncMode );
BERR_Code APETEST_PbLib_DoPlayWithTimestamps( APETEST_PbLib_Handle hPbLib, char *FileName, unsigned int SyncMode );
BERR_Code APETEST_PbLib_WaitForPlayComplete( APETEST_PbLib_Handle hPbLib );
bool APETEST_PbLib_IsPlayComplete( APETEST_PbLib_Handle hPbLib );
BERR_Code APETEST_PbLib_DoPesPlay( APETEST_PbLib_Handle hPbLib, char *FileName );
*/
#ifdef __cplusplus
}
#endif

#endif /* #ifndef APETEST_PLAYBACKLIB_H__ */

/* end of file */

/***************************************************************************
 *     Copyright (c) 2003-2011, Broadcom Corporation
 *     All Rights Reserved
 *     Confidential Property of Broadcom Corporation
 *
 *  THIS SOFTWARE MAY ONLY BE USED SUBJECT TO AN EXECUTED SOFTWARE LICENSE
 *  AGREEMENT  BETWEEN THE USER AND BROADCOM.  YOU HAVE NO RIGHT TO USE OR
 *  EXPLOIT THIS MATERIAL EXCEPT SUBJECT TO THE TERMS OF SUCH AN AGREEMENT.
 *
 * $brcm_Workfile: xpt_pb.h $
 * $brcm_Revision: Hydra_Software_Devel/1 $
 * $brcm_Date: 1/14/11 3:28p $
 *
 * Playback testing library. 
 *
 * Revision History:
 *
 * $brcm_Log: /rockford/unittests/basemodules/dsp/raaga/xpt_pb.h $
 * 
 * Hydra_Software_Devel/1   1/14/11 3:28p gdata
 * SW7422-191:[7422] Adding the raaga test application
 * 
 * Hydra_Software_Devel/3   11/15/10 1:40p speter
 * SW35125-15: [35125] Adding support for 35125
 * 
 * Hydra_Software_Devel/2   7/28/10 11:49a speter
 * SW35230-564: [35230] Removing old checkin comments
 * 
 * Hydra_Software_Devel/1   4/30/10 10:26a speter
 * SW35230-24: [35230] Adding base2 directory for new RAAGA_test for raaga
 * 
 ***************************************************************************/

#ifndef PLAYBACK_LIB_H__
#define PLAYBACK_LIB_H__

#ifdef __cplusplus
extern "C" {
#endif
  
#include "bxpt.h"
#include "bxpt_playback.h"

#if RAAGA_TEST_DIRECTV_SUPPORT
#include "bxpt_directv_playback.h"
#endif

typedef struct BRAAGA_TEST_PbLib_Vars *BRAAGA_TEST_PbLib_Handle;

BRAAGA_TEST_PbLib_Handle BRAAGA_TEST_PbLib_Init( BXPT_Handle hXpt, 
                                             unsigned PbChannelNum );
void BRAAGA_TEST_PbLib_Shutdown( BRAAGA_TEST_PbLib_Handle hPbLib );
BERR_Code BRAAGA_TEST_PbLib_DoPlay( BRAAGA_TEST_PbLib_Handle hPbLib, 
                                  char *FileName, 
                                  unsigned int SyncMode );
BXPT_Playback_Handle BRAAGA_TEST_PbLib_GetPbHandle( BRAAGA_TEST_PbLib_Handle hPbLib );
BERR_Code BRAAGA_TEST_PbLib_DoEsPlay( BRAAGA_TEST_PbLib_Handle hPbLib, char *FileName );
BERR_Code BRAAGA_TEST_PbLib_DoRepeatingPlay( BRAAGA_TEST_PbLib_Handle hPbLib, 
                                           char *FileName, 
                                           unsigned int SyncMode );
BERR_Code BRAAGA_TEST_PbLib_DoPlayWithTimestamps( BRAAGA_TEST_PbLib_Handle hPbLib, 
                                                char *FileName, 
                                                unsigned int SyncMode );
BERR_Code BRAAGA_TEST_PbLib_DoPlayNonBlocking( BRAAGA_TEST_PbLib_Handle hPbLib, 
                                             char *FileName,	
                                             unsigned int SyncMode );
BERR_Code BRAAGA_TEST_PbLib_WaitForPlayComplete( BRAAGA_TEST_PbLib_Handle hPbLib );
BERR_Code BRAAGA_TEST_PbLib_StopPlay( BRAAGA_TEST_PbLib_Handle hPbLib );
bool BRAAGA_TEST_PbLib_IsPlayComplete( BRAAGA_TEST_PbLib_Handle hPbLib );
BERR_Code BRAAGA_TEST_PbLib_DoEsPlayNonBlocking( BRAAGA_TEST_PbLib_Handle hPbLib, 
                                               char *FileName );
BERR_Code BRAAGA_TEST_PbLib_DoPesPlay( BRAAGA_TEST_PbLib_Handle hPbLib, 
                                     char *FileName );

#ifdef __cplusplus
}
#endif

#endif /* #ifndef PLAYBACK_LIB_H__ */

/* end of file */

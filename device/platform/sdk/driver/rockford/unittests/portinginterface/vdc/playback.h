/***************************************************************************
 *     Copyright (c) 2004-2012, Broadcom Corporation
 *     All Rights Reserved
 *     Confidential Property of Broadcom Corporation
 *
 *  THIS SOFTWARE MAY ONLY BE USED SUBJECT TO AN EXECUTED SOFTWARE LICENSE
 *  AGREEMENT  BETWEEN THE USER AND BROADCOM.  YOU HAVE NO RIGHT TO USE OR
 *  EXPLOIT THIS MATERIAL EXCEPT SUBJECT TO THE TERMS OF SUCH AN AGREEMENT.
 *
 * $brcm_Workfile: playback.h $
 * $brcm_Revision: Hydra_Software_Devel/6 $
 * $brcm_Date: 10/11/12 12:09p $
 *
 * Module Description:
 *
 * Revision History:
 *
 * $brcm_Log: /rockford/unittests/portinginterface/vdc/playback.h $
 * 
 * Hydra_Software_Devel/6   10/11/12 12:09p syang
 * SW7346-572: add long-time-lock for vdc_test
 * 
 * Hydra_Software_Devel/5   10/12/11 11:02a pntruong
 * SW7429-16: Initial port.
 *
 * Hydra_Software_Devel/4   4/22/11 5:41p tdo
 * SW7425-366: Add support for playback of more than 1 file to support
 * dual playback and decode.
 *
 * Hydra_Software_Devel/3   1/6/11 4:26p tdo
 * SW7231-23: Remove include bxpt_priv.h from playback.[ch]
 *
 * Hydra_Software_Devel/2   12/7/09 5:32p tdo
 * SW7408-23: Remove direct TV stuff from playback
 *
 * Hydra_Software_Devel/1   12/4/09 3:55p tdo
 * SW7408-23: Add playback feature for digital source
 *
 ***************************************************************************/

#ifndef VDC_TEST_PLAYBACK_H__
#define VDC_TEST_PLAYBACK_H__

#ifdef __cplusplus
extern "C" {
#endif


#include <stdio.h>
#include <string.h>
#include <fcntl.h>

#include "config.h"
#include "framework.h"
#include "bkni_multi.h"
#include "btst_kni.h"

#include "bxpt.h"
#include "bxpt_playback.h"
#include "test_util.h"

/* PVR atom mutlilpier of both transport packet size, filesystem block size */
/* and filesystem block size */
#define PB_BUFFER_LEN           ((188 / 4) * 4096)
#define NUM_PB_BUFFERS          (8)

#ifdef BXPT_NUM_PLAYBACKS
#define MAX_PLAYBACKS           BXPT_NUM_PLAYBACKS
#else
#define MAX_PLAYBACKS           BXPT_P_MAX_PLAYBACKS
#endif

typedef struct Playback_Vars
{
	BXPT_Handle                 hXpt;
	unsigned int                uiChannelNum;
	bool                        bInit;
	bool                        bRunning;
	void                       *Buffer[NUM_PB_BUFFERS];
	BTST_TaskHandle             hPbTask;
	BKNI_EventHandle            PbStartEvent;
	BKNI_EventHandle            PbPrimedEvent;
	BKNI_EventHandle            PbDoneEvent;
	BKNI_EventHandle            DescriptorDoneEvent;
	BXPT_Playback_Handle        hPb;
	BMEM_Handle                 hMem;
	BINT_Handle                 hInt;
	BXPT_PvrDescriptor         *Descriptor[NUM_PB_BUFFERS];
	unsigned int                uiBufferSize[NUM_PB_BUFFERS];
	int                         fd;
	FILE                       *InFilePtr;
	BINT_CallbackHandle         DescDoneIntCb;
	bool                        bRepeat;
	int                         iBufferNext;
	BTST_P_LongTimeLock         stFileLock;
} Playback_Vars;

typedef struct Playback_Vars *Playback_Handle;

typedef struct
{
	char                        szFileName[1024]; /* Stream file path */
	unsigned int                uiSyncMode;
	unsigned int                uiChannelNum;
	bool                        bIsES;
	bool                        bRepeat;
} Playback_Settings;


Playback_Handle Init_Playback
	( BXPT_Handle               hXpt,
	  BMEM_Handle               hMem,
	  BINT_Handle               hInt,
	  unsigned int              uiPbChannelNum );

void Open_PlaybackChan
(
    BXPT_Handle                 hXpt,
    Playback_Handle             hPlayback,
    unsigned int                uiPbChannelNum );

void Close_PlaybackChan
	( Playback_Handle           hPlayback );

void Playback_Shutdown
	( Playback_Handle           hPlayback );

BERR_Code Playback_DoStartPlay
	( Playback_Handle           hPlayback,
	  char                     *FileName,
	  unsigned int              SyncMode,
	  bool                      isES );

BERR_Code Playback_DoStopPlay
	( Playback_Handle           hPlayback );

#ifdef __cplusplus
}
#endif

#endif /* VDC_TEST_PLAYBACK_H__ */

/* end of file */

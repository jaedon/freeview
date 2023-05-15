/***************************************************************************
*     Copyright (c) 2004-2010, Broadcom Corporation
*     All Rights Reserved
*     Confidential Property of Broadcom Corporation
*
*  THIS SOFTWARE MAY ONLY BE USED SUBJECT TO AN EXECUTED SOFTWARE LICENSE
*  AGREEMENT  BETWEEN THE USER AND BROADCOM.  YOU HAVE NO RIGHT TO USE OR
*  EXPLOIT THIS MATERIAL EXCEPT SUBJECT TO THE TERMS OF SUCH AN AGREEMENT.
*
* $brcm_Workfile: bsynclib_algo.h $
* $brcm_Revision: Hydra_Software_Devel/3 $
* $brcm_Date: 5/28/10 6:31p $
*
* Revision History:
*
* $brcm_Log: /magnum/syslib/synclib/noarch/bsynclib_algo.h $
* 
* Hydra_Software_Devel/3   5/28/10 6:31p bandrews
* SW7405-4436: printing channel index in dbg messages
* 
* Hydra_Software_Devel/2   12/10/09 9:18p bandrews
* SW7401-3634: adding PWC (now JTI) support to synclib
* 
* Hydra_Software_Devel/1   3/24/08 3:09p bandrews
* PR40865: Fixed
* 
* Hydra_Software_Devel/3   2/26/08 3:03p bandrews
* PR37951: Added video delay requantization
* 
* Hydra_Software_Devel/2   1/3/08 5:17p bandrews
* PR37951: Updated based on initial feedback
* 
* Hydra_Software_Devel/1   12/12/07 2:53p bandrews
* PR37951: Initial check-in
***************************************************************************/

#include "bstd.h"
#include "bsynclib.h"
#include "bsynclib_channel_priv.h"

#ifndef BSYNCLIB_ALGO_H__
#define BSYNCLIB_ALGO_H__

void BSYNClib_Algo_AudioVideo_Sync(BSYNClib_Channel_Path * psAudio, BSYNClib_Channel_Path * psVideo, BSYNClib_Channel_Results * psResults);
void BSYNClib_Algo_AudioAudio_Sync(BSYNClib_Channel_Path * psPath);
void BSYNClib_Algo_VideoVideo_Sync(BSYNClib_Channel_Path * psPath);

void BSYNClib_Algo_AudioVideo_Allocator(BSYNClib_Channel_Path * psAudio, BSYNClib_Channel_Path * psVideo, BSYNClib_Channel_Results * psResults);
void BSYNClib_Algo_AudioAudio_Allocator(BSYNClib_Channel_Path * psPath);
void BSYNClib_Algo_VideoVideo_Allocator(BSYNClib_Channel_Path * psPath);

void BSYNClib_Algo_AudioVideo_MaxFinder(BSYNClib_Channel_Path * psAudio, BSYNClib_Channel_Path * psVideo, BSYNClib_Channel_Results * psResults);
void BSYNClib_Algo_AudioSource_MaxFinder(BSYNClib_Channel_Path * psPath);
void BSYNClib_Algo_AudioSink_MaxFinder(BSYNClib_Channel_Path * psPath);
void BSYNClib_Algo_VideoSource_MaxFinder(BSYNClib_Channel_Path * psPath);
void BSYNClib_Algo_VideoSink_MaxFinder(BSYNClib_Channel_Path * psPath);

BERR_Code BSYNClib_Algo_AudioSource_Applicator(BSYNClib_Channel_Path * psPath, BSYNClib_Channel_SetDelay pfSetDelay, void * pvParm1, int iParm2);
BERR_Code BSYNClib_Algo_AudioSink_Applicator(BSYNClib_Channel_Path * psPath, BSYNClib_Channel_SetDelay pfSetDelay, void * pvParm1, int iParm2);
BERR_Code BSYNClib_Algo_VideoSource_Applicator(BSYNClib_Channel_Path * psPath, BSYNClib_Channel_SetDelay pfSetDelay, void * pvParm1, int iParm2);
BERR_Code BSYNClib_Algo_VideoSink_Applicator(BSYNClib_Channel_Path * psPath, BSYNClib_Channel_SetDelay pfSetDelay, void * pvParm1, int iParm2);

void BSYNClib_Algo_RequantizeDelay(int iChannelIndex, unsigned int uiDelay, unsigned int uiQuantizationLevel, unsigned int * puiRequantizedDelay);
void BSYNClib_Algo_CalculateJitterToleranceImprovementFactor(BSYNClib_Channel_Path * psVideo, BSYNClib_VideoSource * psSource, unsigned int uiCurrentDelay, int * piJtiFactor, unsigned int * puiAdditionalDelay);

#endif /* BSYNCLIB_ALGO_H__ */


/***************************************************************************
* Copyright (c) 2002-2012, Broadcom Corporation
* All Rights Reserved
* Confidential Property of Broadcom Corporation
*
* THIS SOFTWARE MAY ONLY BE USED SUBJECT TO AN EXECUTED SOFTWARE LICENSE
* AGREEMENT BETWEEN THE USER AND BROADCOM. YOU HAVE NO RIGHT TO USE OR
* EXPLOIT THIS MATERIAL EXCEPT SUBJECT TO THE TERMS OF SUCH AN AGREEMENT.
*
* $brcm_Workfile: brutus_platform.h $
* $brcm_Revision: 3 $
* $brcm_Date: 9/15/12 1:14a $
*
* Module Description:
*
* Revision History:
*
* $brcm_Log: /BSEAV/app/brutus2/src/platform/brutus_platform.h $
* 
* 3   9/15/12 1:14a tokushig
* SW7231-749: add graphics resource
* 
* 2   9/10/12 4:19p tokushig
* SW7231-749: add simple audio/video decoders to resource manager.  for
* now we will add just 1 of each but this can be easily increased as
* necessary in the future.
* 
* 1   5/25/12 6:03p tokushig
* SW7231-749: moved from plaform specific directories
* 
* 1   3/28/12 4:47p jrubio
* SW7231-749: add brutus2 support
* 
* 
* 
*********************************************************************************/

#ifndef PLATFORM_H__
#define PLATFORM_H__

#define BRUTUS_NUM_PCM_PLAYBACKS            2
#define BRUTUS_NUM_PCM_CAPTURES             2
#define BRUTUS_NUM_ES_DECODES               1
#define BRUTUS_NUM_STREAMERS                1 
#define BRUTUS_NUM_LINEIN                   0
#define BRUTUS_NUM_IR_REMOTES               1 
#define BRUTUS_MAX_PROGRAMS                 8
#define BRUTUS_NUM_SIMPLE_AUDIO_DECODES     1
#define BRUTUS_NUM_SIMPLE_VIDEO_DECODES     1
#define BRUTUS_NUM_GRAPHICS                 1

#endif /* PLATFORM_H__ */

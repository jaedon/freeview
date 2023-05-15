/***************************************************************************
*     Copyright (c) 1999-2015, Broadcom Corporation
*     All Rights Reserved
*     Confidential Property of Broadcom Corporation
* 
* 
* THIS SOFTWARE MAY ONLY BE USED SUBJECT TO AN EXECUTED SOFTWARE LICENSE
* AGREEMENT  BETWEEN THE USER AND BROADCOM.  YOU HAVE NO RIGHT TO USE OR
* EXPLOIT THIS MATERIAL EXCEPT SUBJECT TO THE TERMS OF SUCH AN AGREEMENT.
*
* $brcm_Workfile: bdsp_raaga_version.h $
* $brcm_Revision: Hydra_Software_Devel/sw7425_5121/SW7429-1116/SW7346_1782_FIX/SW7346_1790_FIX/SW7346_1800_FIX/1 $
* $brcm_Date: 5/22/15 5:17p $
*
* Module Description: BDSP Version Info File
*
* Revision History:
*
* $brcm_Log: /magnum/basemodules/dsp/src/raaga/firmware/fp2008/bdsp_raaga_version.h $
* 
* Hydra_Software_Devel/sw7425_5121/SW7429-1116/SW7346_1782_FIX/SW7346_1790_FIX/SW7346_1800_FIX/1   5/22/15 5:17p blraudiofw
* SW7346-1800 : - Limit the pan values to +/-30 degrees when main audio
* is Stereo
* 
* Hydra_Software_Devel/sw7425_5121/SW7429-1116/SW7346_1782_FIX/SW7346_1790_FIX/1   4/27/15 6:18p blraudiofw
* SW7346-1790: - Description audio is also muted on AAC+ and MPEG when
* AD_fade_byte is 0xff
* 
* Hydra_Software_Devel/sw7425_5121/SW7429-1116/SW7346_1782_FIX/1   4/21/15 3:45p blraudiofw
* SW7346-1782: - AD_pan_byte of Audio Descriptor doesn't work on MPEG
* audio
* 
* Hydra_Software_Devel/sw7425_5121/SW7429-1116/2   7/25/14 4:03p ananthan
* SW7429-1116:Error return issue in case of using five audio output
* [Adding support for 6 output ports]
* 
* Hydra_Software_Devel/sw7425_5121/SW7429-1116/1   7/25/14 2:20p ananthan
* SW7429-1116:Error return issue in case of using five audio output
* [Adding support for 6 output ports]
* 
* Hydra_Software_Devel/sw7425_5121/1   9/13/13 2:16p rasundar
* sw7425-5121: additionalDelay on Audio output settings doesn't work in
* case of using DSP mixer
* 
* Hydra_Software_Devel/3   2/11/13 5:09p nihar
* SW7425-4229: Updating versions for 7p6 release, fp2008.
* 
* Hydra_Software_Devel/2   2/4/13 1:56p nihar
* SW7425-4229: Adding support for BDSP Version - Adding file
* bdsp_raaga_version.h to fp2008.
* 
***************************************************************************/

#define BDSP_RAAGA_MAJOR_VERSION       7
#define BDSP_RAAGA_MINOR_VERSION       6
#define BDSP_RAAGA_BRANCH_VERSION      9
#define BDSP_RAAGA_BRANCH_SUBVERSION   3

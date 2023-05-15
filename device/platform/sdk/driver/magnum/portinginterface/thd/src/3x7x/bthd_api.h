/***************************************************************************
 *     (c)2005-2011 Broadcom Corporation
 *  
 *  This program is the proprietary software of Broadcom Corporation and/or its licensors,
 *  and may only be used, duplicated, modified or distributed pursuant to the terms and
 *  conditions of a separate, written license agreement executed between you and Broadcom
 *  (an "Authorized License").  Except as set forth in an Authorized License, Broadcom grants
 *  no license (express or implied), right to use, or waiver of any kind with respect to the
 *  Software, and Broadcom expressly reserves all rights in and to the Software and all
 *  intellectual property rights therein.  IF YOU HAVE NO AUTHORIZED LICENSE, THEN YOU
 *  HAVE NO RIGHT TO USE THIS SOFTWARE IN ANY WAY, AND SHOULD IMMEDIATELY
 *  NOTIFY BROADCOM AND DISCONTINUE ALL USE OF THE SOFTWARE.  
 *   
 *  Except as expressly set forth in the Authorized License,
 *   
 *  1.     This program, including its structure, sequence and organization, constitutes the valuable trade
 *  secrets of Broadcom, and you shall use all reasonable efforts to protect the confidentiality thereof,
 *  and to use this information only in connection with your use of Broadcom integrated circuit products.
 *   
 *  2.     TO THE MAXIMUM EXTENT PERMITTED BY LAW, THE SOFTWARE IS PROVIDED "AS IS" 
 *  AND WITH ALL FAULTS AND BROADCOM MAKES NO PROMISES, REPRESENTATIONS OR 
 *  WARRANTIES, EITHER EXPRESS, IMPLIED, STATUTORY, OR OTHERWISE, WITH RESPECT TO 
 *  THE SOFTWARE.  BROADCOM SPECIFICALLY DISCLAIMS ANY AND ALL IMPLIED WARRANTIES 
 *  OF TITLE, MERCHANTABILITY, NONINFRINGEMENT, FITNESS FOR A PARTICULAR PURPOSE, 
 *  LACK OF VIRUSES, ACCURACY OR COMPLETENESS, QUIET ENJOYMENT, QUIET POSSESSION 
 *  OR CORRESPONDENCE TO DESCRIPTION. YOU ASSUME THE ENTIRE RISK ARISING OUT OF 
 *  USE OR PERFORMANCE OF THE SOFTWARE.
 *  
 *  3.     TO THE MAXIMUM EXTENT PERMITTED BY LAW, IN NO EVENT SHALL BROADCOM OR ITS 
 *  LICENSORS BE LIABLE FOR (i) CONSEQUENTIAL, INCIDENTAL, SPECIAL, INDIRECT, OR 
 *  EXEMPLARY DAMAGES WHATSOEVER ARISING OUT OF OR IN ANY WAY RELATING TO YOUR 
 *  USE OF OR INABILITY TO USE THE SOFTWARE EVEN IF BROADCOM HAS BEEN ADVISED OF 
 *  THE POSSIBILITY OF SUCH DAMAGES; OR (ii) ANY AMOUNT IN EXCESS OF THE AMOUNT 
 *  ACTUALLY PAID FOR THE SOFTWARE ITSELF OR U.S. $1, WHICHEVER IS GREATER. THESE 
 *  LIMITATIONS SHALL APPLY NOTWITHSTANDING ANY FAILURE OF ESSENTIAL PURPOSE OF 
 *  ANY LIMITED REMEDY.
 * 
 * $brcm_Workfile: bthd_api.h $
 * $brcm_Revision: Hydra_Software_Devel/2 $
 * $brcm_Date: 10/16/12 5:33p $
 *
 * [File Description:]
 *
 * Revision History:
 *
 * $brcm_Log: /magnum/portinginterface/thd/src/3x7x/bthd_api.h $
 * 
 * Hydra_Software_Devel/2   10/16/12 5:33p farshidf
 * SW3472-21: add ssi and sqi for 7552
 * 
 * Hydra_Software_Devel/1   9/7/12 9:34a farshidf
 * SW7425-3819: Remove symbolic links and flatten out THD directory
 * structure
 * 
 * Hydra_Software_Devel/22   8/27/12 5:51p farshidf
 * SW7552-334:  re-structure the thd PI files to use the Thd files in AP
 * vob
 * 
 * Hydra_Software_Devel/21   7/18/12 10:16a farshidf
 * SW7552-305: fix signal strength
 * 
 * Hydra_Software_Devel/20   7/12/12 3:33p farshidf
 * SW7552-305: update enum
 * 
 * Hydra_Software_Devel/19   7/5/12 4:35p farshidf
 * SW7552-295: fix kernel mode crash
 * 
 * Hydra_Software_Devel/18   6/28/12 4:00p farshidf
 * SW7552-278: update the signal stength
 * 
 * Hydra_Software_Devel/17   6/19/12 12:05p farshidf
 * SW7552-286: fix the signal strength
 * 
 * Hydra_Software_Devel/16   6/14/12 5:09p farshidf
 * SW7552-288: update the code to 5.0 release
 * 
 * Hydra_Software_Devel/15   5/24/12 11:01a farshidf
 * SW3461-1: update TS mode
 * 
 * Hydra_Software_Devel/14   5/22/12 5:20p farshidf
 * SW3461-211: make dev branch to compile for ISDBT
 * 
 * Hydra_Software_Devel/13   3/29/12 7:46p farshidf
 * SW7552-239: fix compile
 * 
 * Hydra_Software_Devel/12   3/29/12 7:40p farshidf
 * SW7552-239: fix the version
 * 
 * Hydra_Software_Devel/11   3/27/12 12:03p farshidf
 * SW3461-1: update to version 4.6
 * 
 * Hydra_Software_Devel/10   3/27/12 11:50a farshidf
 * SW7552-207: fix the ews
 * 
 * Hydra_Software_Devel/9   3/2/12 2:11p farshidf
 * SW7552-223: implement 7552 THD Frontend PM
 * 
 * Hydra_Software_Devel/8   2/24/12 12:43p farshidf
 * SW7552-209: update tHD staus
 * 
 * Hydra_Software_Devel/7   2/23/12 6:22p farshidf
 * SW7552-209: remove the flag AAT
 * 
 * Hydra_Software_Devel/6   2/9/12 2:57p farshidf
 * SW7552-205: add the new flag
 * 
 * Hydra_Software_Devel/5   2/8/12 5:14p farshidf
 * SW7552-205: update to 4.0 release
 * 
 * Hydra_Software_Devel/4   12/21/11 2:23p farshidf
 * SW3128-1: remove warning
 * 
 * Hydra_Software_Devel/3   12/20/11 7:02p farshidf
 * SW3128-1: remove warning
 * 
 * Hydra_Software_Devel/2   11/21/11 11:18p farshidf
 * SW7552-139: adapt to the new THD header API file
 * 
 * Hydra_Software_Devel/1   10/10/11 2:13p farshidf
 * SW7552-134: update to 2.0 of 3461 code
 * 
 * Hydra_Software_Devel/20   10/10/11 1:52p farshidf
 * SW7552-134: update to match V2_0 of 3461
 * 
 * Hydra_Software_Devel/19   8/17/11 2:24p farshidf
 * SW7552-105:  update the frontend with 3461_V1_0_REL  label
 * 
 * Hydra_Software_Devel/18   8/16/11 6:12p farshidf
 * SW7552-105: update the frontend with 3461_V1_0_RC0 label
 * 
 * Hydra_Software_Devel/17   8/16/11 6:04p farshidf
 * SW7552-105: update the frontend with 3461_V1_0_RC0 label
 * 
 * Hydra_Software_Devel/16   7/20/11 12:20p farshidf
 * SWDTV-7620: merge to main
 * 
 * Hydra_Software_Devel/SWDTV-7620/2   7/20/11 12:15p jchien
 * SWDTV-7620: Modified RS_SYNC Interrupt logic. Add EWS.
 * 
 * Hydra_Software_Devel/SWDTV-7620/1   6/21/11 6:54p jchien
 * SWDTV-7620:  Add EWS Event
 * 
 * Hydra_Software_Devel/14   6/10/11 2:45p farshidf
 * SWDTV-7504: update the isdbt compile flag
 * 
 * Hydra_Software_Devel/13   6/6/11 10:12a farshidf
 * SW7552-36: fix compile issue
 * 
 * Hydra_Software_Devel/12   6/3/11 5:15p farshidf
 * SW7552-36: merge main
 * 
 * Hydra_Software_Devel/11   6/3/11 4:46p farshidf
 * SW7552-36: merge to main
 * 
 * Hydra_Software_Devel/SWDTV-7361/2   6/2/11 4:26p mbsingh
 * SWDTV-7361: Fix the order of THD status parameters
 * 
 * Hydra_Software_Devel/SWDTV-7361/1   6/2/11 2:19p mbsingh
 * SWDTV-7361: Fix the order of THD status parameters
 * 
 * Hydra_Software_Devel/10   6/1/11 11:45a farshidf
 * SW7552-36: clean up the code
 * 
 * Hydra_Software_Devel/9   5/27/11 10:52a farshidf
 * SW7552-36: compiel fix
 * 
 * Hydra_Software_Devel/8   5/24/11 2:38p farshidf
 * SWDTV-7146: merge main
 * 
 * Hydra_Software_Devel/SWDTV-7146/1   5/19/11 10:31a jchien
 * SWDTV-7146: add RS_SYNC interrupts for ISDBT
 * 
 * Hydra_Software_Devel/7   5/6/11 2:20p farshidf
 * SWDTV-6857: update the auto acquire mode
 * 
 * Hydra_Software_Devel/6   5/5/11 8:37p farshidf
 * SWDTV-6857: merge main
 * 
 * Hydra_Software_Devel/SWDTV-6857/3   5/5/11 8:34p farshidf
 * SWDTV-6857: fix the event report to nexus
 * 
 * Hydra_Software_Devel/SWDTV-6857/2   5/5/11 3:15p farshidf
 * SWDTV-6857: merge main
 * 
 * Hydra_Software_Devel/5   5/5/11 1:07p farshidf
 * SWDTV-6857: add ISDB-T
 * 
 * Hydra_Software_Devel/SWDTV-6857/1   5/3/11 2:56p jchien
 * SWDTV-6857: add ISDBT changes
 * 
 * Hydra_Software_Devel/3   4/29/11 10:58a shyi
 * SWDTV-6190: Changed signature of a private API for initializing
 * parameters
 * 
 * Hydra_Software_Devel/2   4/27/11 12:13p farshidf
 * SWDTV-6190: add the BBS call from Nexus
 * 
 * Hydra_Software_Devel/1   4/21/11 6:40p farshidf
 * SWDTV-6191: add the THD code
 * 
 * 23   4/20/11 4:56p farshidf
 * SW3461-1: make it host compatible
 * 
 * 22   4/9/11 10:23a mpovich
 * SW3461-1: Add Internal HAB commands for THD.
 * 
 * 21   4/8/11 3:56p farshidf
 * SW3461-1: merge main
 * 
 * DVB_3461_1/3   4/7/11 9:32p jputnam
 * SW3461-1: Initial version of interrupt-based lock detection
 * 
 * DVB_3461_1/2   4/6/11 4:26p farshidf
 * SW3461-1: merge main
 * 
 * 20   4/6/11 4:26p farshidf
 * SW3461-1: merge main
 * 
 * DVB_3461_1/1   4/6/11 2:16p jputnam
 * SW3461-1: Removed obsolete AcqRequest field from Common Acquire Params
 * 
 * 19   4/6/11 11:50a mpovich
 * SW3461-1: Add Core-specific HAB power command.  Add call to process THD
 *  timer event to THD manager/task.
 * 
 * 18   4/4/11 4:32p mpovich
 * SW3461-1: Update THD Status HAB command to include missing parameters:
 *  Acquire Mode, and block counters.
 * 
 * 17   4/1/11 4:27p farshidf
 * SW3461-1: merge main
 * 
 * Rom_Devel_3461/1   3/31/11 4:50p mpovich
 * SW3461-1: Add support for THD (DVB-T and ISDB-T) HAB commands (Acquire,
 *  Status, Get Constellation).
 * 
 * 16   3/8/11 12:39p farshidf
 * SW3461-1: fix teh THD API
 * 
 * 15   3/4/11 5:10p farshidf
 * SW3461-1: add the power seq
 * 
 * 14   3/4/11 4:52p farshidf
 * SW3461-1: add the power seq
 * 
 * 13   2/8/11 11:57a jputnam
 * SW3461-1: Added spectral inversion field to status
 * 
 * 12   1/21/11 5:57p farshidf
 * SW3461-1: update names
 * 
 * 11   1/20/11 4:22p farshidf
 * SW3461-1: update the thd/isdbt
 * 
 * 10   1/7/11 2:27p jputnam
 * SW3461-1:  Make THD_ResetEvent and THD_EventIsr private - Change
 *  THD_SetAcqSettings() to THD_InitializeParams()
 * 
 * 9   1/6/11 10:09a jputnam
 * SW3461-1: Major cosmetic overhaul - Conform to new API conventions
 *  common to all demods - Conform to naming conventions for private
 *  functions and typedefs - Separate DVB-T and ISDB-T params, status,
 *  functions
 * 
 * 8   1/3/11 5:36p jputnam
 * SW3461-1: changed enumerated type and #define names from THD_ to THD_
 * 
 * 7   12/16/10 6:49p farshidf
 * SW3461-1: clean up
 * 
 * 6   12/14/10 1:33p farshidf
 * SW3461-1: add the new sync IRQ event
 * 
 * 5   12/3/10 4:48p farshidf
 * SW3461-1: update api
 * 
 * 4   11/30/10 10:56a farshidf
 * SW3461-1: update the thd files
 * 
 * 3   11/15/10 12:40p farshidf
 * SW3461-1: compile fix
 * 
 * 2   9/23/10 5:39p farshidf
 * SW3461-1: implement the new event flag
 * 
 * 1   8/17/10 5:11p farshidf
 * SW3461-1: initial THD code
 * 
 * Hydra_Software_Devel/8   12/11/09 6:30p farshidf
 * SW7550-38: code clean uo
 * 
 * 
 ***************************************************************************/

/***************************************************************************
 * Definitions
 ***************************************************************************/
#ifndef _BTHD_API_H__
#define _BTHD_API_H__

#if __cplusplus
extern "C" {
#endif

#include "bthd_def.h"

#ifndef MIN
#define MIN(x,y)           ((x) < (y) ? (x) : (y))
#endif

#define THD_IQ_CONSTELLATION_VECTORS   30
#define THD_SignalStrengthCalibrationFactor -15  /* Determined empirically using BCM93461DC */
/***************************************************************************
 * Enumerated Type Definitions (General)
 ***************************************************************************/
typedef enum THD_Standard
{
  THD_Standard_DVBT=0,
  THD_Standard_ISDBT=1
} THD_Standard_t;

typedef enum THD_FrontEndMode
{
  THD_FrontEndMode_Baseband=0,
  THD_FrontEndMode_IF=1
} THD_FrontEndMode_t;

typedef enum THD_ChannelEstimatorMode
{
  THD_ChannelEstimatorMode_Fixed=0,
  THD_ChannelEstimatorMode_Pedestrian=1,
  THD_ChannelEstimatorMode_Mobile=2,
  THD_ChannelEstimatorMode_Auto=3
} THD_ChannelEstimatorMode_t;

typedef enum THD_FFTWindowMode
{
  THD_FFTWindowMode_InSpan=0,
  THD_FFTWindowMode_OutOfSpanPost=1,
  THD_FFTWindowMode_OutOfSpanPre=2,
  THD_FFTWindowMode_Auto=3
} THD_FFTWindowMode_t;

typedef enum THD_CoChannelMode
{
  THD_CoChannelMode_None=0,
  THD_CoChannelMode_Auto=1,
  THD_CoChannelMode_Other=2
} THD_CoChannelMode_t;

typedef enum THD_ImpulseMode
{
  THD_ImpulseMode_None=0,
  THD_ImpulseMode_Auto=1,
  THD_ImpulseMode_Other=2
} THD_ImpulseMode_t;

typedef enum THD_Spectrum
{
  THD_Spectrum_Normal=0,
  THD_Spectrum_Inverted=1
} THD_Spectrum_t;

typedef enum THD_TransGuardMode
{
  THD_TransGuardMode_Manual=0,
  THD_TransGuardMode_Auto=1,
  THD_TransGuardMode_Auto_DVBT=2,
  THD_TransGuardMode_Auto_ISDBT=3
} THD_TransGuardMode_t;

typedef enum THD_CarrierRange
{
  THD_CarrierRange_Narrow=0,
  THD_CarrierRange_Wide=1
} THD_CarrierRange_t;

typedef enum THD_FrequencyInterpolatorMode
{
  THD_FrequencyInterpolatorMode_InSpan=0,
  THD_FrequencyInterpolatorMode_OutOfSpanPost=1,
  THD_FrequencyInterpolatorMode_OutOfSpanPre_1_4=2,
  THD_FrequencyInterpolatorMode_OutOfSpanPre_1_8=3,
  THD_FrequencyInterpolatorMode_OutOfSpanPre_1_16=4
} THD_FrequencyInterpolatorMode_t;

typedef enum THD_TSMode
{
  THD_TSMode_None=0,
  THD_TSMode_Serial=1,
  THD_TSMode_Parallel=2
} THD_TSMode_t;

typedef enum THD_TSSuppressClock
{
  THD_TSSuppressClock_None=0,
  THD_TSSuppressClock_WhenNotValid=1,
  THD_TSSuppressClock_OnStart=2,	
  THD_TSSuppressClock_WhenNotValidAndOnStart=3
} THD_TSSuppressClock_t;

typedef enum THD_TSSuppressClockEdge
{
  THD_TSSuppressClockEdge_Positive=0,
  THD_TSSuppressClockEdge_Negative=1
} THD_TSSuppressClockEdge_t;

typedef enum THD_TSSyncLength
{
  THD_TSSyncLength_Byte=0,
  THD_TSSyncLength_Bit=1
} THD_TSSyncLength_t;

typedef enum THD_TSHeaderLength
{
  THD_TSHeaderLength_1Byte=0,
  THD_TSHeaderLength_4Bytes=1
} THD_TSHeaderLength_t;

typedef enum THD_SnooperMode
{
  THD_SnooperMode_Ce=0,
  THD_SnooperMode_CePwr=1,
  THD_SnooperMode_Ne=2,  
  THD_SnooperMode_Exp=3,
  THD_SnooperMode_Fft=4     
} THD_SnooperMode_t;

typedef enum THD_InterpolatorMode
{
  THD_InterpolatorMode_Fi=0,
  THD_InterpolatorMode_Ti=1
} THD_InterpolatorMode_t;

#define THD_3x7x_NUM_SPARE_STATUS_PARAMS (8) /* spare internal status */

typedef enum THD_State
{
  THD_State_Init=0,
  THD_State_SP=1,
  THD_State_TPS=2,
  THD_State_FFTTrigger=3,
  THD_State_FEC=4,
  THD_State_Track=5,
  THD_State_CheckLock=6,
  THD_State_ChangeChannelEstimator=7,
  #ifdef BTHD_ISDBT_SUPPORT
  THD_State_ChangeFFTWindow=8,
  #endif
  THD_State_Done=9
} THD_State_t;

/***************************************************************************
 * Parameter Enumerated Type Definitions
 ***************************************************************************/
/* TPS/TMCC Parameter, Matches register definition */
typedef enum THD_Bandwidth
{
  THD_Bandwidth_8MHz=0,
  THD_Bandwidth_7MHz=1,
  THD_Bandwidth_6MHz=2,
  THD_Bandwidth_5MHz=3
} THD_Bandwidth_t;

/* TPS/TMCC Parameter, Matches register definition */
typedef enum THD_TransmissionMode
{
  THD_TransmissionMode_2k=0,
  THD_TransmissionMode_8k=1,
  THD_TransmissionMode_4k=2
} THD_TransmissionMode_t;

/* TPS/TMCC Parameter, Matches register definition */
typedef enum THD_GuardInterval
{
  THD_GuardInterval_1_32=0,
  THD_GuardInterval_1_16=1,
  THD_GuardInterval_1_8=2,
  THD_GuardInterval_1_4=3
} THD_GuardInterval_t;

/* TPS/TMCC Parameter, Matches register definition */
typedef enum THD_Qam
{
  THD_Qam_Qpsk=0,
  THD_Qam_16Qam=1,
  THD_Qam_64Qam=2,
  THD_Qam_Dqpsk=3
} THD_Qam_t;

/* TPS/TMCC Parameter, Matches register definition */
typedef enum THD_CodeRate
{
  THD_CodeRate_1_2=0,
  THD_CodeRate_2_3=1,
  THD_CodeRate_3_4=2,
  THD_CodeRate_5_6=3,
  THD_CodeRate_7_8=4
} THD_CodeRate_t;

/***************************************************************************
 * DVB-T Specific Enumerated Type Definitions
 ***************************************************************************/
typedef enum THD_DvbtTPSMode
{
  THD_DvbtTPSMode_Manual=0,
  THD_DvbtTPSMode_Auto=1
} THD_DvbtTPSMode_t;

typedef enum THD_DvbtPriorityMode
{
  THD_DvbtPriorityMode_High=0,
  THD_DvbtPriorityMode_Low=1
} THD_DvbtPriorityMode_t;

/* TPS Parameter, Matches register definition */
typedef enum THD_DvbtInterleaving 
{
  THD_DvbtInterleaving_Native=0,
  THD_DvbtInterleaving_InDepth=1
} THD_DvbtInterleaving_t;

/* TPS Parameter, Matches register definition */
typedef enum THD_DvbtHierarchy
{
  THD_DvbtHierarchy_None=0,
  THD_DvbtHierarchy_1=1,
  THD_DvbtHierarchy_2=2,
  THD_DvbtHierarchy_4=3
} THD_DvbtHierarchy_t;

/***************************************************************************
 * ISDB-T Specific Enumerated Type Definitions
 ***************************************************************************/
typedef enum THD_IsdbtTMCCMode
{
  THD_IsdbtTMCCMode_Manual=0,
  THD_IsdbtTMCCMode_Auto=1
} THD_IsdbtTMCCMode_t;

/* TMCC Parameter, Matches register definition */
typedef enum THD_IsdbtTimeInt
{
  THD_IsdbtTimeInt_0X=0,
  THD_IsdbtTimeInt_1X=1,
  THD_IsdbtTimeInt_2X=2,
  THD_IsdbtTimeInt_4X=3
} THD_IsdbtTimeInt_t;

typedef enum THD_IsdbtPr
{
  THD_IsdbtPr_Disable=0,
  THD_IsdbtPr_Enable=1
} THD_IsdbtPr_t;


typedef enum THD_IsdbtEws
{
  THD_IsdbtEws_Disable=0,
  THD_IsdbtEws_Enable=1
} THD_IsdbtEws_t;

typedef enum THD_IsdbtEwsFlag
{
  THD_IsdbtEwsFlag_eLockUpdate = 1,
  THD_IsdbtEwsFlag_eUnlockStopEWS = 2, /* if this channel is unlock, stop reporting EWS */
  THD_IsdbtEwsFlag_eLockStartEWS = 4     /* if this channel is lock, start reporting EWS */
} THD_IsdbtEwsFlag_t;

/***************************************************************************
 * Miscellaneous Definitions
 ***************************************************************************/
typedef enum THD_TransGuardResult
{
  THD_TransGuardResult_None=0,
  THD_TransGuardResult_NoSignal=1,
  THD_TransGuardResult_CoChannelPresent=2
} THD_TransGuardResult_t;

typedef enum THD_AcquireResult
{
  THD_AcquireResult_Lock=0,
  THD_AcquireResult_NoSignal=1,
  THD_AcquireResult_NoOFDMFound=2,
  THD_AcquireResult_NoFFTLock=3,
  THD_AcquireResult_NoCarrierLock=4,
  THD_AcquireResult_NoSPLock=5,
  THD_AcquireResult_NoTPSLock=6,
  THD_AcquireResult_NoFECLock=7,
  THD_AcquireResult_BadBER=8,
  THD_AcquireResult_InitLockState=9,
  THD_AcquireResult_NoDVBTSignal=10,
  THD_AcquireResult_AbortedEarly = 11
} THD_AcquireResult_t;

typedef enum THD_LockStatusBit
{
  THD_LockStatusBit_TPSLock=0,
  THD_LockStatusBit_VitLock=1,
  THD_LockStatusBit_FECLock=2,
  THD_LockStatusBit_SystemLock=3,
  THD_LockStatusBit_NoDVBTSignal=4
} THD_LockStatusBit_t;

#define THD_Qam_N           3
#define THD_GuardInterval_N 4
#define THD_CodeRate_N      5

#define THD_FrequencyInterpolatorCoefLength   396
#define THD_SNRAlpha_N 1
#define THD_SNRAlpha_D 16
#define THD_StatusFramesForReset 10
#define THD_ICESpectrumInversionThreshold 8

/*************************************************************************
 * Summary:
 *   Enumeration for analog input
 * 
 * Description:
 *   This enum specifies analog audio input options for the IFD decoder. This
 *   only applies to IFD core in cases where this PI controls both ofdm and
 *   analog video receiver. 
 * 
 * See Also:
 *   None.
 * 
 ***************************************************************************/
typedef enum THD_AnalogInput {
  THD_AnalogInput_eIf = 0,		/* Audio input with IF signal */
  THD_AnalogInput_eSif_1 = 0x80	/* Audio input from SIF Channel 1 */
} THD_AnalogInput;


/*************************************************************************
 * Summary:
 *   Enumeration for power status
 * 
 * See Also:
 *   None.
 * 
 ***************************************************************************/
typedef enum THD_PowerStatus
{
    THD_ePower_Off = 0,
    THD_ePower_On  = 1,
    THD_ePower_Unknown = 0xFF
}  THD_PowerStatus;


/*************************************************************************
 * Summary:
 *   Set Param type
 * 
 * See Also:
 *   None.
 * 
 ***************************************************************************/
typedef enum THD_Params_Request_Type_s
{
  THD_eParams_Request_Type_Common =1,
  THD_eAcquireParams_Request_Type_Common =THD_eParams_Request_Type_Common,
  THD_eStatus_Request_Type_Common =THD_eParams_Request_Type_Common,

  THD_eParams_Request_Type_Dvbt =2,
  THD_eAcquireParams_Request_Type_Dvbt =THD_eParams_Request_Type_Dvbt,
  THD_eStatus_Request_Type_Dvbt =THD_eParams_Request_Type_Dvbt,

  THD_eParams_Request_Type_Isdbt =3,
  THD_eAcquireParams_Request_Type_Isdbt =THD_eParams_Request_Type_Isdbt,
  THD_eStatus_Request_Type_Isdbt =THD_eParams_Request_Type_Isdbt,

  THD_eParams_Request_Type_Internal_Dvbt =4,
  THD_eAcquireParams_Request_Type_Internal_Dvbt =THD_eParams_Request_Type_Internal_Dvbt,
  THD_eStatus_Request_Type_Internal_Dvbt        =THD_eParams_Request_Type_Internal_Dvbt,

  THD_eParams_Request_Type_Internal_Isdbt =5,
  THD_eAcquireParams_Request_Type_Internal_Isdbt =THD_eParams_Request_Type_Internal_Isdbt,
  THD_eStatus_Request_Type_Internal_Isdbt        =THD_eParams_Request_Type_Internal_Isdbt

} THD_Params_Request_Type_t, THD_Status_Request_Type_t, THD_AcquireParams_Request_Type_t;

/*************************************************************************
 * Summary:
 *  BBS Enum
 * 
 * See Also:
 *   None.
 * 
 ***************************************************************************/

typedef enum THD_AcquireMode_s
{
    THD_AcquireMode_Auto = 1,
	THD_AcquireMode_Manual = 2,
	THD_AcquireMode_ResetStatus = 4,
	THD_AcquireMode_Scan = 8
} THD_AcquireMode_t;

typedef enum THD_NexusStatusMode
{
  THD_NexusStatusMode_EnableStatusForNexus = 1
} THD_NexusStatusMode_t;
/*************************************************************************
 * Summary:
 *   Structure for chip-specific Acquire Parameters
 *    
 * Description:
 *   This is the chip-specific component of the Thd Acquire command. It maps
 *   byte for byte to the acquire parameter inside the chip's firmware
 *     
 * See Also:
 *   None.
 *      
 ***************************************************************************/

typedef struct THD_DvbtAcquireParam_s
{
  THD_DvbtTPSMode_t               TPSMode;
  THD_DvbtPriorityMode_t          PriorityMode;
  THD_Qam_t                       Qam;
  THD_DvbtHierarchy_t             Hierarchy;
  THD_CodeRate_t                  CodeRateHP;
  THD_CodeRate_t                  CodeRateLP;
} THD_DvbtAcquireParam_t;

typedef struct THD_IsdbtAcquireParam_s
{
  THD_IsdbtTMCCMode_t             TMCCMode;
  THD_IsdbtPr_t                   Pr;
  THD_Qam_t                       Qam[3];
  THD_CodeRate_t                  CodeRate[3];
  THD_IsdbtTimeInt_t              TimeInt[3];
  uint32_t                        Segments[3];
} THD_IsdbtAcquireParam_t;

typedef struct THD_InternalAcquireParam_s
{
  THD_FrontEndMode_t              FrontEndMode;
  THD_ChannelEstimatorMode_t      ChannelEstimatorMode;
  THD_FFTWindowMode_t             FFTWindowMode;
  THD_ImpulseMode_t               ImpulseMode;
  uint32_t                         SampleFreq;
  uint32_t                         TransGuardMaxThreshold;
  uint32_t                         TransGuardMaxMinRatioThreshold[4];
  bool                             AllowRsSyncEvent;  
  bool                             AbortAcquireRequested;  /* Acquire Abort-Early flag */
  BKNI_EventHandle                 hCurrAbortableEventHandle; /* Event pending/triggered when an Abort-Early condition occurs */
#ifdef SmartNotchEnabled  
  bool														SmartNotchPresent;
#endif 
} THD_InternalAcquireParam_t;

typedef struct THD_InternalStatus_s
{
   THD_CoChannelMode_t            CoChannelMode;      
   THD_FFTWindowMode_t            FFTWindowMode;        
   THD_ChannelEstimatorMode_t     ChannelEstimatorMode; 
   uint32_t                        LowICECount;       
} THD_InternalStatus_t;


typedef struct THD_CommonAcquireParam_s
{
  volatile THD_AcquireMode_t	  AcquireMode;
  volatile THD_NexusStatusMode_t  NexusStatusMode;
  THD_Standard_t                  Standard;
  THD_Bandwidth_t                 Bandwidth;
  uint32_t                         CenterFreq;
  THD_TransGuardMode_t            TransGuardMode;
  THD_TransmissionMode_t          TransmissionMode;
  THD_GuardInterval_t             GuardInterval;
  THD_CoChannelMode_t             CoChannelMode;
  THD_CarrierRange_t              CarrierRange;
  THD_TSMode_t                    TSMode;
}THD_CommonAcquireParam_t;

#ifdef BTHD_ISDBT_SUPPORT
typedef struct THD_IsdbtLocalParam_s
{
  bool					set_a;
  bool					set_b;
  bool					set_c;
  bool					clr_a;
  bool					clr_b;
  bool					clr_c;
  bool					phase;
/*  bool					fallback; */ /* pedestrial fallback flag */ 
  THD_IsdbtEwsFlag_t	EwsFlag;
}THD_IsdbtLocalParam_t;
#endif
/* This is the acquire params kept by the driver */
typedef struct THD_3x7x_AcquireParam_s
{
  THD_CommonAcquireParam_t   CommonAcquireParam;
  union StdAcquire
  {
	THD_DvbtAcquireParam_t          DvbtAcquireParam;
	THD_IsdbtAcquireParam_t         IsdbtAcquireParam;
  }StdAcquire;
#ifdef BTHD_ISDBT_SUPPORT
   THD_IsdbtLocalParam_t         IsdbtLocaleParam;
#endif
} THD_3x7x_AcquireParam_t;


typedef struct THD_3x7x_ConfigParam_s
{
	THD_TSMode_t                    TSMode;
	bool							TSInvertClock;
	bool							TSInvertValid;
	bool							TSInvertSync;
	bool							TSInvertError;
	THD_TSSuppressClock_t			TSSuppressClock;
	THD_TSSuppressClockEdge_t		TSSuppressClockEdge;	
	bool							TSSuppressData;
	THD_TSSyncLength_t				TSSyncLength;
	THD_TSHeaderLength_t			TSHeaderLength;	
	bool							TSDeleteHeader;	
} THD_3x7x_ConfigParam_t;
        
/***************************************************************************
 * Summary:
 *   Structure for THD status
 *    
 * Description:
 *   This structure contains THD status that is return by the chip
 *     
 * See Also:
 *   None.
 *      
 ***************************************************************************/

typedef struct THD_DvbtStatus_s
{
  THD_Qam_t              Qam;
  THD_CodeRate_t         CodeRateHP;
  THD_CodeRate_t         CodeRateLP;
  THD_DvbtHierarchy_t    Hierarchy;
  THD_DvbtInterleaving_t Interleaving;
  THD_DvbtPriorityMode_t Priority;
  int32_t                 SNRData;
  uint32_t                CellID;
  uint32_t                PreVitBER;
  uint32_t                VitBER;
  uint32_t                TS_PER;
  uint32_t                TS_ESR;
  uint32_t                TS_CERC;
  uint32_t                TS_UBERC;
  uint32_t                TS_CBERC;
  uint32_t                TS_NBERC;
  uint32_t                TS_TBERC;
  uint32_t                TS_UFERC;
  uint32_t                TS_NFERC;
  uint32_t                TS_TFERC;
  uint32_t                TS_CERC_ref;
  uint32_t                TS_UBERC_ref;
  uint32_t                TS_CBERC_ref;
  uint32_t                TS_NBERC_ref;
  uint32_t                TS_TBERC_ref;
  uint32_t                TS_UFERC_ref;
  uint32_t                TS_NFERC_ref;
  uint32_t                TS_TFERC_ref;
} THD_DvbtStatus_t;

typedef struct THD_IsdbtStatus_s
{
  THD_Qam_t              Qam[3];
  THD_CodeRate_t         CodeRate[3];
  THD_IsdbtTimeInt_t     TimeInt[3];
  THD_IsdbtPr_t          Pr;
  THD_IsdbtEws_t         Ews;
  uint32_t               Segments[3];
  int32_t                SNRData[3];
  uint32_t               TS_UFERC;
  uint32_t               TS_NFERC;
  uint32_t               TS_TFERC;
  uint32_t               TS_UFERC_ref;
  uint32_t               TS_NFERC_ref;
  uint32_t               TS_TFERC_ref;
  uint32_t               TS_ESR;
  uint32_t               VitBER[3];   
  uint32_t               TS_PER[3];
  uint32_t               TS_CERC[3];
  uint32_t               TS_UBERC[3];
  uint32_t               TS_CBERC[3];
  uint32_t               TS_NBERC[3];
  uint32_t               TS_TBERC[3];
  uint32_t               TS_CERC_ref[3];
  uint32_t               TS_UBERC_ref[3];
  uint32_t               TS_CBERC_ref[3];
  uint32_t               TS_NBERC_ref[3];
  uint32_t               TS_TBERC_ref[3];
  uint32_t				  Ssi[3];
  uint32_t				  Sqi[3];
} THD_IsdbtStatus_t;

typedef struct THD_3x7x_CommonStatus_s
{
  THD_AcquireMode_t           AcquireMode;
  uint32_t                    Lock;
  THD_Bandwidth_t             Bandwidth;
  THD_TransmissionMode_t     TransmissionMode;
  THD_GuardInterval_t        GuardInterval;
  int32_t                     CarrierOffset;
  int32_t                     TimingOffset;
  uint32_t                    RagcGain;
  uint32_t                    IagcGain;
  int32_t                     DagcGain;
  int32_t                     SNR;
  int32_t                     SNRPilot;
  uint32_t                    ReacquireCount;
  uint32_t                    AcquisitionTime;
  THD_Spectrum_t             Spectrum;
  THD_ChannelEstimatorMode_t ChannelEstimatorMode;
  THD_FFTWindowMode_t        FFTWindowMode;
  THD_CoChannelMode_t        CoChannelMode;
  uint32_t                    CoChannelPresent;
  uint32_t                    LowICECount;
  uint32_t                    ChannelSpan;
  int32_t                     FFTTriggerOffset;
  int32_t                     FFTTriggerSlope;
  uint32_t                    spare[THD_3x7x_NUM_SPARE_STATUS_PARAMS];
  THD_PowerStatus			  PowerStatus;                     /* channel power (on/off) status */
  int32_t                     SignalStrength;
  int32_t                     FFTTriggerMissed;
  int32_t                     FFTTriggerOutOfSpan; 
  int32_t                     FFTTriggerInSpan;       
  int32_t                     FFTTriggerOnGuard; 
  uint32_t                    FECLock;
  uint32_t                    NoSignal;
  uint32_t                    Ssi;
  uint32_t                    Sqi;
  THD_TSMode_t                TSMode;
} THD_3x7x_CommonStatus_t;


typedef struct THD_3x7x_Status_s
{
  THD_3x7x_CommonStatus_t	  ThdCommonStatus;
  union StdStatus
  {
	THD_DvbtStatus_t		  DvbtStatus;
	THD_IsdbtStatus_t         IsdbtStatus;
  }StdStatus;
} THD_3x7x_Status_t;

/***************************************************************************
 							Callback from THD to Tuner Info
****************************************************************************/




/***************************************************************************
 * Summary:
 *   Structure for chip-specific THD handle
 *    
 * Description:
 *   This is the chip-specific component of the BTHD_3x7x_Handle.
 *     
 * See Also:
 *   None.
 *      
 ***************************************************************************/
typedef struct BTHD_3x7x_P_Handle
{
  BREG_Handle					hRegister;         /* handle used to access AP registers */
  BCHP_Handle					hChip;
  BINT_Handle					hInterrupt;
  THD_3x7x_AcquireParam_t		*pAcquireParam;
  THD_InternalAcquireParam_t	*pInternalAcquireParam;
  THD_3x7x_ConfigParam_t		*pConfigParam;
  THD_3x7x_Status_t				*pStatus;
#if defined BCHP_THD_CORE_V_4_0	|| defined BTHD_ISDBT_SUPPORT
  int32_t						fftpwr[768];
#else
  int32_t						fftpwr[68];
#endif
  BTMR_Handle					hTmr;          /* user settings */
  BTMR_TimerHandle				hTimer;
  BTMR_TimerHandle				hBBSTimer;
  THD_AnalogInput				eAnalogInput;
  uint32_t						ThdLockStatus;
  uint32_t						LockStatusTracking;
  BTHD_CallbackFunc				pCallback[BTHD_Callback_eLast];
  void 						   *pCallbackParam[BTHD_Callback_eLast];
  bool						    AutoAcquireMasked;

  /* THD code callbacks */
  BKNI_EventHandle				hFwCorrMaxEvent;
  BKNI_EventHandle				hFwSyncEvent;
  BKNI_EventHandle				hSpSyncEvent;
  BKNI_EventHandle				hTpsSyncEvent;
  BKNI_EventHandle				hFecSyncEvent;
  BKNI_EventHandle				hFbcntZeroEvent;
  BKNI_EventHandle				hRsSyncEvent; 
  BKNI_EventHandle				hLockEvent; 
  BKNI_EventHandle				hEWSEvent;
  BKNI_EventHandle				hIntEvent;
  BKNI_EventHandle				hBBSInterruptEvent;
#ifdef BTHD_ISDBT_SUPPORT
  BKNI_EventHandle				hTmccSyncEvent;
  BINT_CallbackHandle			hTmccSyncCallback;
  BINT_CallbackHandle			hRsSyncBCallback;
  BINT_CallbackHandle			hRsSyncLossBCallback;
  BINT_CallbackHandle			hRsSyncCCallback;
  BINT_CallbackHandle			hRsSyncLossCCallback;
  uint32_t						*pIsdbtMemory;
#endif
  BINT_CallbackHandle			hFwCorrMaxCallback;
  BINT_CallbackHandle			hFwSyncCallback;
  BINT_CallbackHandle			hSpSyncCallback;
  BINT_CallbackHandle			hTpsSyncCallback;
  BINT_CallbackHandle			hFecSyncCallback;
  BINT_CallbackHandle			hFbcCntCallback;
  BINT_CallbackHandle			hRsSyncCallback;
  BINT_CallbackHandle			hRsSyncLossCallback;

} BTHD_3x7x_P_Handle;

typedef struct BTHD_3x7x_P_Handle *BTHD_3x7x_Handle;

/****************************************************************
 * API function interface
 ****************************************************************/
BERR_Code BTHD_3x7x_Open(BTHD_Handle *, BCHP_Handle, void*, BINT_Handle, const struct BTHD_Settings*);
BERR_Code BTHD_3x7x_Close(BTHD_Handle);
BERR_Code BTHD_3x7x_InitializeParams(BTHD_Handle, const uint8_t *, uint32_t);
BERR_Code BTHD_3x7x_Acquire(BTHD_Handle, const BTHD_InbandParams *);
BERR_Code BTHD_3x7x_GetStatus( BTHD_Handle, BTHD_THDStatus *);
BERR_Code BTHD_3x7x_GetInterruptEventHandle(BTHD_Handle, BKNI_EventHandle*);
BERR_Code BTHD_3x7x_GetBBSInterruptEventHandle(BTHD_Handle, BKNI_EventHandle*);
BERR_Code BTHD_3x7x_GetEWSEventHandle(BTHD_Handle h, BKNI_EventHandle* hEvent);
BERR_Code BTHD_3x7x_ProcessInterruptEvent(BTHD_Handle);
BERR_Code BTHD_3x7x_ProcessBBSInterruptEvent(BTHD_Handle);
BERR_Code BTHD_3x7x_Acquire(BTHD_Handle, const BTHD_InbandParams *);
BERR_Code BTHD_3x7x_GetLockStatus(BTHD_Handle, BTHD_LockStatus *);
BERR_Code BTHD_3x7x_ResetStatus(BTHD_Handle);
BERR_Code BTHD_3x7x_GetLockStateChangeEvent(BTHD_Handle, BKNI_EventHandle*);
BERR_Code BTHD_3x7x_GetDefaultInbandParams(BTHD_InbandParams *);
BERR_Code BTHD_3x7x_GetConstellation(BTHD_Handle, int16_t *,int16_t *);  
BERR_Code BTHD_3x7x_PowerDown(BTHD_Handle);
BERR_Code BTHD_3x7x_PowerUp(BTHD_Handle h);
BERR_Code BTHD_3x7x_P_TNR_callback(BTHD_3x7x_Handle h, uint16_t Mode, int16_t *Gain, uint16_t *SmartTune, uint32_t	*RF_Freq);
BERR_Code BTHD_3x7x_InstallCallback(BTHD_Handle, BTHD_Callback, BTHD_CallbackFunc, void *); 
						

#define BREG_WriteField(h, Register, Field, Data) \
  BREG_Write32(h, BCHP_##Register, ((BREG_Read32(h, BCHP_##Register) & \
  ~((uint32_t)BCHP_MASK(Register, Field))) | \
  BCHP_FIELD_DATA(Register, Field, Data)))


#define BREG_ReadField(h, Register, Field) \
  ((((BREG_Read32(h, BCHP_##Register)) & BCHP_MASK(Register,Field)) >> \
  BCHP_SHIFT(Register,Field)))


/****************************************************************
 * Internal define and functions calls 
 ***************************************************************/
/*Event definitions - match THD_CORE_STATUS bit locations  */
#define THD_EVENT_FFT_SYNC         0
#define THD_EVENT_SP_SYNC          1
#define THD_EVENT_TPS_SYNC         3
#define THD_EVENT_TMCC_SYNC        6
#define THD_EVENT_VIT_SYNC         9
#define THD_EVENT_FEC_SYNC        11
#define THD_EVENT_TS_ONE_PKT      14
#define THD_EVENT_FSCNT_ZERO      17
#define THD_EVENT_FBCNT_ZERO      18
#define THD_EVENT_FW_CORR_MAX_RDY 21
#define THD_EVENT_RS_SYNC	   	  32
#define THD_EVENT_RS_SYNC_LOSS    33
#define THD_EVENT_RS_SYNC_B	   	  34
#define THD_EVENT_RS_SYNC_B_LOSS  35
#define THD_EVENT_RS_SYNC_C	   	  36
#define THD_EVENT_RS_SYNC_C_LOSS  37



#ifdef __cplusplus
}
#endif

#endif /* THD_API_H__ */

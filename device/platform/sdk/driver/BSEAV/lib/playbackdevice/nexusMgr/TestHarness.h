/******************************************************************************
 *    (c)2010-2011 Broadcom Corporation
 * 
 * This program is the proprietary software of Broadcom Corporation and/or its licensors,
 * and may only be used, duplicated, modified or distributed pursuant to the terms and
 * conditions of a separate, written license agreement executed between you and Broadcom
 * (an "Authorized License").  Except as set forth in an Authorized License, Broadcom grants
 * no license (express or implied), right to use, or waiver of any kind with respect to the
 * Software, and Broadcom expressly reserves all rights in and to the Software and all
 * intellectual property rights therein.  IF YOU HAVE NO AUTHORIZED LICENSE, THEN YOU
 * HAVE NO RIGHT TO USE THIS SOFTWARE IN ANY WAY, AND SHOULD IMMEDIATELY
 * NOTIFY BROADCOM AND DISCONTINUE ALL USE OF THE SOFTWARE.  
 *  
 * Except as expressly set forth in the Authorized License,
 *  
 * 1.     This program, including its structure, sequence and organization, constitutes the valuable trade
 * secrets of Broadcom, and you shall use all reasonable efforts to protect the confidentiality thereof,
 * and to use this information only in connection with your use of Broadcom integrated circuit products.
 *  
 * 2.     TO THE MAXIMUM EXTENT PERMITTED BY LAW, THE SOFTWARE IS PROVIDED "AS IS" 
 * AND WITH ALL FAULTS AND BROADCOM MAKES NO PROMISES, REPRESENTATIONS OR 
 * WARRANTIES, EITHER EXPRESS, IMPLIED, STATUTORY, OR OTHERWISE, WITH RESPECT TO 
 * THE SOFTWARE.  BROADCOM SPECIFICALLY DISCLAIMS ANY AND ALL IMPLIED WARRANTIES 
 * OF TITLE, MERCHANTABILITY, NONINFRINGEMENT, FITNESS FOR A PARTICULAR PURPOSE, 
 * LACK OF VIRUSES, ACCURACY OR COMPLETENESS, QUIET ENJOYMENT, QUIET POSSESSION 
 * OR CORRESPONDENCE TO DESCRIPTION. YOU ASSUME THE ENTIRE RISK ARISING OUT OF 
 * USE OR PERFORMANCE OF THE SOFTWARE.
 * 
 * 3.     TO THE MAXIMUM EXTENT PERMITTED BY LAW, IN NO EVENT SHALL BROADCOM OR ITS 
 * LICENSORS BE LIABLE FOR (i) CONSEQUENTIAL, INCIDENTAL, SPECIAL, INDIRECT, OR 
 * EXEMPLARY DAMAGES WHATSOEVER ARISING OUT OF OR IN ANY WAY RELATING TO YOUR 
 * USE OF OR INABILITY TO USE THE SOFTWARE EVEN IF BROADCOM HAS BEEN ADVISED OF 
 * THE POSSIBILITY OF SUCH DAMAGES; OR (ii) ANY AMOUNT IN EXCESS OF THE AMOUNT 
 * ACTUALLY PAID FOR THE SOFTWARE ITSELF OR U.S. $1, WHICHEVER IS GREATER. THESE 
 * LIMITATIONS SHALL APPLY NOTWITHSTANDING ANY FAILURE OF ESSENTIAL PURPOSE OF 
 * ANY LIMITED REMEDY.
 *
 * $brcm_Workfile: TestHarness.h $
 * $brcm_Revision: 1 $
 * $brcm_Date: 8/16/11 2:24p $
 * 
 * Module Description:
 * 
 * Revision History:
 * 
 * $brcm_Log: /AppLibs/broadcom/playbackdevice/nexusMgr/TestHarness.h $
 * 
 * 1   8/16/11 2:24p cdisc
 * SW7425-1114: adding playback device
 * 
 * 2   6/8/10 5:02p ajitabhp
 * SW7405-4474:
 * Added test harness support in stagecraft for direct FB.
 * 
 *****************************************************************************/
#ifndef _BROADCOM_TEST_HARNESS_
#define _BROADCOM_TEST_HARNESS_

#define U32 unsigned int
#define KEY_DIRECTION_DOWN (0)
#define KEY_DIRECTION_UP   (1)

typedef enum {
    TestHarnessModeNone,
    TestHarnessModeInputCapture,
    TestHarnessModeOutputCapture,
    TestHarnessModePlayback
} TestHarnessMode;

typedef void (*STG_KEY_UPDATE)	(void *,U32,U32);
#ifdef __cplusplus
extern "C" {
#endif

void                                TestHarnessAddKey(U32 key, U32 direction);
void                                TestHarnessAddFrameHash(U32 crc); 
U32                                 TestHarnessGetKey(U32 frame, U32 * key, U32 * direction);
U32                                 TestHarnessGetFrameHash(U32 frame);

TestHarnessMode                     TestHarnessGetMode(void);
void                                TestHarnessFail(U32 frame);
void                                TestHarnessPass(void);

void                                TestHarnessStart(void);
void                                TestHarnessTerminate(void);

#ifdef __cplusplus
}
#endif

#endif /* _BROADCOM_TEST_HARNESS_BRIDGE_ */

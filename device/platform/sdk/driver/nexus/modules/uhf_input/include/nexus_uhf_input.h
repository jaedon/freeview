/***************************************************************************
*     (c)2004-2012 Broadcom Corporation
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
* $brcm_Workfile: nexus_uhf_input.h $
* $brcm_Revision: 5 $
* $brcm_Date: 4/27/12 12:25p $
*
* API Description:
*   API name: UhfInput
*    Specific APIs related to UHF Input Control.
*
* Revision History:
*
* $brcm_Log: /nexus/modules/uhf_input/7400/include/nexus_uhf_input.h $
* 
* 5   4/27/12 12:25p erickson
* SW7435-148: fix NEXUS_UhfInput_GetEvents proxy
* 
* 4   12/14/10 3:18p erickson
* SW7420-1285: add null_allowed attribute
* 
* 3   3/24/08 5:26p mphillip
* PR39982: Correct the channel names
* 
* 2   2/7/08 11:42p vsilyaev
* PR 38682: Added attributes for the linux kernel/user proxy mode
* 
* 1   1/18/08 2:22p jgarrett
* PR 38808: Merging to main branch
* 
* Nexus_Devel/2   11/21/07 11:12a erickson
* PR37423: update
* 
* Nexus_Devel/1   11/20/07 5:56p erickson
* PR37423: added module for ir_blaster, keypad and uhf_input
*
* Nexus_Devel/1   9/13/07 5:40p jgarrett
* PR 34702: Adding UHF Input
*
***************************************************************************/

#ifndef NEXUS_UHF_INPUT_H__
#define NEXUS_UHF_INPUT_H__

#ifdef __cplusplus
extern "C" {
#endif

typedef struct NEXUS_UhfInput *NEXUS_UhfInputHandle;

/***************************************************************************
 * UHF Input Channel
 ***************************************************************************/
typedef enum NEXUS_UhfInputChannel
{
    NEXUS_UhfInputMode_eChannel1,    /* 369.50 MHz channel 1 */
    NEXUS_UhfInputMode_eChannel2,    /* 371.10 MHz channel 2 */
    NEXUS_UhfInputMode_eChannel3,    /* 375.30 MHz channel 3 */
    NEXUS_UhfInputMode_eChannel4,    /* 376.90 MHz channel 4 */
    NEXUS_UhfInputMode_eChannel5,    /* 388.30 MHz channel 5 */
    NEXUS_UhfInputMode_eChannel6,    /* 391.50 MHz channel 6 */
    NEXUS_UhfInputMode_eChannel7,    /* 394.30 MHz channel 7 */
    NEXUS_UhfInputMode_eChannel8,    /* 395.90 MHz channel 8 */
    NEXUS_UhfInputMode_eChannel9,    /* 433.92 MHz channel 9 */
    NEXUS_UhfInputMode_eMax
} NEXUS_UhfInputChannel;

/***************************************************************************
 * UHF Input Device Settings
 ***************************************************************************/
typedef struct NEXUS_UhfInputSettings
{
    unsigned repeatFilterTime;      /* Key Repeat filter time (in ms) -- keys less than this amount apart will be sent as repeats */
    unsigned eventQueueSize;        /* Max number of events that can be queued before an overflow */
    NEXUS_CallbackDesc dataReady;   /* Data ready callback */
    NEXUS_UhfInputChannel channel;  /* UHF Channel */
} NEXUS_UhfInputSettings;

/***************************************************************************
Summary:
    Get default settings for an UHF receiver.
See Also:
    NEXUS_UhfInput_Open
 ***************************************************************************/
void NEXUS_UhfInput_GetDefaultSettings(
    NEXUS_UhfInputSettings *pSettings    /* [out] */
    );

/***************************************************************************
Summary:
    Open an UHF receiver.
See Also:
    NEXUS_UhfInput_Close
 ***************************************************************************/
NEXUS_UhfInputHandle NEXUS_UhfInput_Open(  /* attr{destructor=NEXUS_UhfInput_Close}  */
    unsigned index,
    const NEXUS_UhfInputSettings *pSettings  /* attr{null_allowed=y} May be passed as NULL for defaults */
    );

/***************************************************************************
Summary:
    Close an UHF receiver.
See Also:
    NEXUS_UhfInput_Open
 ***************************************************************************/
void NEXUS_UhfInput_Close(
    NEXUS_UhfInputHandle handle
    );

/***************************************************************************
 * UHF Input Event
 ***************************************************************************/
typedef struct NEXUS_UhfInputEvent
{
    bool repeat;        /* Is this a key repeat? */
    uint8_t header;     /* Initial 8 bits for a 40-bit code (DirecTV only) */
    uint32_t code;      /* Code from receiver, shifted down if appropriate */
} NEXUS_UhfInputEvent;

/***************************************************************************
Summary:
    Retrieve queued UHF events.
Description:
    This function should be called in response to the dataReady callback.
    It should be called until numEventsRead returns 0.
See Also:
    NEXUS_UhfInput_FlushEvents
 ***************************************************************************/
NEXUS_Error NEXUS_UhfInput_GetEvents(
    NEXUS_UhfInputHandle handle,
    NEXUS_UhfInputEvent *pEvents,   /* attr{nelem=numEvents;nelem_out=pNumEventsRead} Pointer to an array of events */
    size_t numEvents,               /* Size of the event array */
    size_t *pNumEventsRead,         /* [out] Number of events actually read */
    bool *pOverflow                 /* [out] Has an overflow occurred? */
    );

/***************************************************************************
Summary:
    Discard all queued UHF events
See Also:
    NEXUS_UhfInput_GetEvents
 ***************************************************************************/
void NEXUS_UhfInput_FlushEvents(
    NEXUS_UhfInputHandle handle
    );

#ifdef __cplusplus
}
#endif

#endif /* #ifndef NEXUS_UHF_INPUT_H__ */


/******************************************************************************
 *    (c)2008-2012 Broadcom Corporation
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
 * $brcm_Workfile: apetest_utils.h $
 * $brcm_Revision: Hydra_Software_Devel/1 $
 * $brcm_Date: 7/10/12 9:25a $
 *
 * Module Description:
 *
 * Revision History:
 *
 * $brcm_Log: /rockford/unittests/portinginterface/ape/97422/common/apetest_utils.h $
 * 
 * Hydra_Software_Devel/1   7/10/12 9:25a jgarrett
 * SW7425-3457: Adding ape_decode application
 * 
 
 *
 *****************************************************************************/
#ifndef APETEST_UTILS_H__
#define APETEST_UTILS_H__

/* APE includes */
#include "bape.h"

/* DSP includes */
#include "bdsp.h"

 /* bstream_mpeg_type */
#include "bmedia_probe.h"

#define APETEST_INVALID (0xffffffff)

typedef struct {
    const char *name;
    unsigned int value;
}namevalue_t;


BAVC_AudioCompressionStd    BdspAudio2BavcAudio (BDSP_AudioType bdsp_value);
BDSP_AudioType              BavcAudio2BdspAudio (BAVC_AudioCompressionStd magnum_value);

BAVC_AudioCompressionStd    Baudio2BavcAudio    (baudio_format settop_value);
baudio_format               BavcAudio2Baudio    (BAVC_AudioCompressionStd magnum_value);

BAVC_StreamType             Bstream2BavcStream  (bstream_mpeg_type type);

const char * GetStr_baudio_format(baudio_format val);
const char * GetStr_BAVC_AudioCompressionStd(BAVC_AudioCompressionStd val);
const char * GetStr_BAVC_StreamType(BAVC_StreamType val);




typedef struct MENU_ENTRY {
    char *cmd_name;
    uint cmd_opcode;
    int arg_count;
    char *arg_desc;
    char *cmd_desc;
} MENU_ENTRY;

#define MENU_INVALID APETEST_INVALID


#define MENU_MAX_PARM_CNT (20)
#define MENU_MAX_PARM_LINE_LEN (1024)

typedef struct MENU_ARGS {
    int argc;
    char *argv[MENU_MAX_PARM_CNT];  /* will point to str below */
    char line[MENU_MAX_PARM_LINE_LEN];  /* some where to put the scanned command line */
} MENU_ARGS;

void APETEST_MenuPrintHelp (MENU_ENTRY *pMenuCmd);
/* can return false only if *pExit gets set to true */ 
bool APETEST_MenuGetCmd (
        volatile bool *pExit,   /* in, App/ISR tell this function to give up and return back, can be NULL */     
        MENU_ENTRY *pMenuCmd,   /* in, ptr to array/table of commands */
        unsigned int *opcode,   /* out, opcode/command input from user*/
        MENU_ARGS *pMenuArg     /* out, arguments input from user */
        );

/* use these if you like to write complicated forground loops that do all kinds of things, else use above function */
#if 0
void APETEST_MenuPrtPrompt (void);
void APETEST_MenuInputAvailable (void);
/* true if valid user input returned */ 
bool APETEST_MenuGetCmdBlock (MENU_ENTRY *pMenuCmd, unsigned int *opcode, MENU_ARGS *pMenuArg);
#endif



void APETEST_WaitForDecoderDone(BAPE_DecoderHandle handle);


#if BDBG_DEBUG_BUILD
void APETEST_InitModuleDebug(void);
#endif


#endif /* APETEST_UTILS_H__ */


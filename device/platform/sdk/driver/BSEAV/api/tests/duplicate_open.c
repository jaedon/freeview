/***************************************************************************
 *     (c)2012 Broadcom Corporation
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
 * $brcm_Workfile: duplicate_open.c $
 * $brcm_Revision: 2 $
 * $brcm_Date: 9/6/12 6:05p $
 *
 * Module Description:
 *
 * Revision History:
 *
 * $brcm_Log: /BSEAV/api/tests/duplicate_open.c $
 * 
 * 2   9/6/12 6:05p erickson
 * SW7435-349: add standard header
 * 
 **************************************************************************/
/* Test that duplicate opens fail. */
#include "bsettop.h"
#include <bstd.h>

int main()
{
	int rc = bsettop_init(BSETTOP_VERSION);
	BDBG_ASSERT(!rc);
	
	bdisplay_t display = bdisplay_open(B_ID(0));
	BDBG_ASSERT(display);
	BDBG_ASSERT(!bdisplay_open(B_ID(0)));

	bgraphics_t graphics = bgraphics_open(B_ID(0), display);
	BDBG_ASSERT(graphics);
	BDBG_ASSERT(!bgraphics_open(B_ID(0), display));

	bdecode_t decode = bdecode_open(B_ID(0));
	BDBG_ASSERT(decode);
	BDBG_ASSERT(!bdecode_open(B_ID(0)));

	bdecode_window_t window = bdecode_window_open(B_ID(0), display);
	BDBG_ASSERT(window);
	BDBG_ASSERT(!bdecode_window_open(B_ID(0), display));
	
	btuner_t tuner = btuner_open(B_ID(0));
	BDBG_ASSERT(tuner);
	BDBG_ASSERT(!btuner_open(B_ID(0)));
	
	bpcm_play_t bpcm_play = bpcm_play_open(B_ID(0));
	BDBG_ASSERT(bpcm_play);
	BDBG_ASSERT(!bpcm_play_open(B_ID(0)));
	
	brecpump_t brecpump = brecpump_open(B_ID(0), NULL);
	BDBG_ASSERT(brecpump);
	BDBG_ASSERT(!brecpump_open(B_ID(0), NULL));
	
	bplaypump_t bplaypump = bplaypump_open(B_ID(0), NULL);
	BDBG_ASSERT(bplaypump);
	BDBG_ASSERT(!bplaypump_open(B_ID(0), NULL));
	
	buser_input_t buser_input = buser_input_open(B_ID(0));
	BDBG_ASSERT(buser_input);
	BDBG_ASSERT(!buser_input_open(B_ID(0)));
	
	buser_output_t buser_output = buser_output_open(B_ID(0));
	BDBG_ASSERT(buser_output);
	BDBG_ASSERT(!buser_output_open(B_ID(0)));
	
	return 0;
}

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
 * $brcm_Workfile: oob.c $
 * $brcm_Revision: 4 $
 * $brcm_Date: 9/6/12 6:04p $
 *
 * Module Description:
 *
 * Revision History:
 *
 * $brcm_Log: /BSEAV/api/examples/oob.c $
 * 
 * 4   9/6/12 6:04p erickson
 * SW7435-349: add standard header
 * 
 **************************************************************************/
#include "bsettop.h"
#include <stdio.h>


/*Use following example to test OOB downstream*/
/* Two format are supported: DVS178 and Annex A*/
#define TEST_DVS178		/*test DVS178 OOB if defined*/
						/*test Annex A OOB if not defined*/

int main() {
	bresult rc = bsettop_init(BSETTOP_VERSION);
#if BCHP_CHIP == 7038
	btuner_t tuner = btuner_open(B_ID(6)); /* QAM OOB tuner on 97038 */
#elif BCHP_CHIP == 7110
	btuner_t tuner = btuner_open(B_ID(2)); /* QAM OOB tuner on 97110 */
#elif BCHP_CHIP == 7400 || BCHP_CHIP == 7401 || BCHP_CHIP == 7118
	btuner_t tuner = btuner_open(B_ID(3)); /* QAM OOB tuner on 9745x, 97118  */
#else
	/* QAM oob not supported */
	btuner_t tuner = NULL;
#endif
	btuner_qam_params qam;
	if (rc || !tuner) goto error;

	btuner_qam_params_init(&qam, tuner);
#ifdef TEST_DVS178
	qam.symbol_rate = 1024000;
	qam.annex = btuner_qam_oob_dvs178;

	if (!btuner_tune_oob(tuner, 103750000, &qam))
		printf("Locked\n");
#else
	qam.symbol_rate = 772000;
	qam.annex = btuner_qam_oob_annex_a;

	if (!btuner_tune_oob(tuner, 163000000, &qam))
		printf("Locked\n");
#endif

	return 0;
error:
	return 1;
}

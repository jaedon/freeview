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
 * $brcm_Workfile: smartcard.c $
 * $brcm_Revision: 2 $
 * $brcm_Date: 9/6/12 6:03p $
 *
 * Module Description:
 *
 * Revision History:
 *
 * $brcm_Log: /BSEAV/api/examples/smartcard.c $
 * 
 * 2   9/6/12 6:03p erickson
 * SW7435-349: add standard header
 * 
 **************************************************************************/
#include "bsettop.h"
#include "bcmscdef.h"
#include <unistd.h>

/* The test was written for the NDS Transport IC card. */

void reset_interface(bsmartcard_t smartcard);
int reset_card(bsmartcard_t smartcard);

unsigned char g_resetdata[] = {
	0x3F,  0xFA,  0x11,  0x25,  0x02,  0x00,  0x21,
	0xB0,  0xA1,  0x04,  0x05,  0x06,  0x07,  0x08,  0x09,  0x0A};

#define TOTAL_COMMANDS 4
struct commands {
	int num_send;
	unsigned char send[264];
	int num_receive;
	unsigned char receive[264];
} g_commands[4] = {
	{5, {0x00, 0x42, 0x00, 0x00, 0x01}, 1, {0x42}},	//Sending NDS Baud Rate
	{1, {0x00}, 2, {0x90,0x00}},				//Set to 9600 baud
	{5, {0x00, 0x84,  0x00,  0x84,  0x00}, 3, {0x84,  0x90,  0x00}}, //Sending NDS Procedure:
	{5, {0x00, 0x84,  0x90,  0x00,  0x00}, 2, {0x90,  0x00}}	// Sending NDS Procedure:
	};

#define FAIL() \
	do {printf("Failed on line %d\n", __LINE__); goto error; } while (0)

int main() {

#define BUFSIZE 256
	unsigned char buf[BUFSIZE];
	int n, counter = 0;
	int loops;

	bresult rc = bsettop_init(BSETTOP_VERSION);
	if (rc) FAIL();

    /* Don't call bsmartcard_init() for legacy chips
	rc = bsmartcard_init();
	if (rc) FAIL();
	*/

	/* Open will block until the card is inserted.
	On the 97115, id 0 is b, id 1 is a. Other platforms are not swapped. */
	bsmartcard_t smartcard = bsmartcard_open(B_ID(1));

	if (rc) FAIL();

	/* You must reset the interface after opening. */
	reset_interface(smartcard);

	loops = 0;
	while (counter < 300) {
		int i;
		if (loops == 0)
			i = 0;
		else
			i = 2; /* don't repeat the 1st two commands */
		for (;i<TOTAL_COMMANDS;i++) {
			//printf("bsmartcard_write\n");
			rc = bsmartcard_write(smartcard, g_commands[i].send, g_commands[i].num_send, &n);
			if (rc) {
				reset_interface(smartcard);
				goto startover;
			}
			if (n != g_commands[i].num_send) FAIL();

			//printf("bsmartcard_read\n");
			rc = bsmartcard_read(smartcard, buf, g_commands[i].num_receive, &n);
			if (rc) {
				reset_interface(smartcard);
				goto startover;
			}
			if (n != g_commands[i].num_receive ||
				memcmp(g_commands[i].receive, buf, n))
			{
				reset_interface(smartcard);
				goto startover;
			}
		}
		printf("Successful transaction.\n");
		counter++;
		continue;
startover:
		i = -1;
		loops = 0;
	}

	return 0;
error:
	return 1;
}

int reset_card(bsmartcard_t smartcard)
{
	unsigned char buf[BUFSIZE];
	int n=16;

	if (bsmartcard_reset_card(smartcard, buf, sizeof(g_resetdata), &n)) FAIL();
	if (n != sizeof(g_resetdata)) FAIL();
	if (memcmp(buf, g_resetdata, n)) {
		int j;
		printf("bsmartcard_reset_card failed:\n%d: ", n);
		for (j=0;j<n;j++) printf("0x%02x ", buf[j]);
		printf("\n");
		FAIL();
	}
	return 0;
error:
	return -1;
}

/*
Reset the interface.

If the card is not present, or if the bsmartcard_reset_card fails,
we must call bsmartcard_reset to reset the interface. i
The result is that this function blocks until a card is inserted.

The reason it must do this is that any error on any function might simply be
the removal of the card.
*/
void reset_interface(bsmartcard_t smartcard)
{
	bsmartcard_status status;

    while (1) {
		if (bsmartcard_reset(smartcard)) continue;

		if (bsmartcard_get_status(smartcard, &status))
			status.status_bits = 0xFFFF;


		if (!(status.status_bits & BCM_SC_CARD_PRES)) {
			continue;
		}

		if (!reset_card(smartcard))
			break;

	}
}


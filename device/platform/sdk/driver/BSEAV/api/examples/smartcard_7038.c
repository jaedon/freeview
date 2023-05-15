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
 * $brcm_Workfile: smartcard_7038.c $
 * $brcm_Revision: 6 $
 * $brcm_Date: 9/6/12 6:04p $
 *
 * Module Description:
 *
 * Revision History:
 *
 * $brcm_Log: /BSEAV/api/examples/smartcard_7038.c $
 * 
 * 6   9/6/12 6:04p erickson
 * SW7435-349: add standard header
 * 
 **************************************************************************/
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include "bsettop.h"
#include "bscd_datatypes.h"

#define BUFSIZE 256
#define TOTAL_COMMANDS 10
#define FAIL() \
	do {printf("Failed on line %d\n", __LINE__); goto error; } while (0)

/* The test was written for the xxx Transport IC card. */
void reset_interface(bsmartcard_t smartcard);
int reset_card(bsmartcard_t smartcard);

unsigned char g_resetdata[] = {
	0x3F,  0xFA,  0x11,  0x25,  0x02,  0x00,  0x21,
	0xB0,  0xA1,  0x04,  0x05,  0x06,  0x07,  0x08,  0x09,  0x0A};

struct commands {
	int num_send;
	unsigned char send[264];
	int num_receive;
	unsigned char receive[264];
} g_commands[10] = {
	{5, {0x00, 0x42, 0x00, 0x00, 0x01}, 1, {0x42}},	/*Sending Baud Rate */
	{1, {0x00}, 2, {0x90,0x00}},				/*Set to 76800 baud */	
	{5, {0x00, 0x42, 0x00, 0x00, 0x01}, 1, {0x42}},	/*Sending Baud Rate */
	{1, {0x40}, 2, {0x90,0x00}},				/*Set to 76800 baud */	
	{5, {0x00, 0x42, 0x00, 0x00, 0x01}, 1, {0x42}},	/*Sending Baud Rate */
	{1, {0x80}, 2, {0x90,0x00}},				/*Set to 76800 baud */	
	{5, {0x00, 0x42, 0x00, 0x00, 0x01}, 1, {0x42}},	/*Sending Baud Rate */
	{1, {0xFF}, 2, {0x90,0x00}},				/*Set to 76800 baud */	
	{5, {0x00, 0x84,  0x00,  0x84,  0x00}, 3, {0x84,  0x90,  0x00}}, /*Sending Procedure: */
	{5, {0x00, 0x84,  0x90,  0x00,  0x00}, 2, {0x90,  0x00}},	/* Sending Procedure: */
	};


int main() 
{
	/* Open will block until the card is inserted. */
	bsmartcard_t smartcard;
	bsmartcard_settings_t settings;
	unsigned char buf[BUFSIZE];
	int n, counter = 0;
	int loops;
	int i;
	bresult rc ;

	rc = bsettop_init(BSETTOP_VERSION);
	if (rc)  FAIL();

	rc = bsmartcard_init();
	if (rc)   FAIL();

	bsmartcard_settings_init(&settings);
	smartcard = bsmartcard_open(B_ID(0), &settings);

	bsmartcard_reset(smartcard, false);	/* This may error if no card is present, the error may be safely ignored.  It is required to finish the interface initialization. */
	bsmartcard_detect_card(smartcard);	/* This should block until a smartcard is inserted */

	/* You must reset the interface after opening. */
	reset_interface(smartcard);

	loops = 0;
	while (counter < 30) {
		if (loops == 0)
			i = 0;
		else
			i = 2; /* don't repeat the 1st two commands */
		for ( ; i<TOTAL_COMMANDS; i++) {
			rc = bsmartcard_write(smartcard, g_commands[i].send, g_commands[i].num_send, (size_t *)&n);
			if (rc) {
       
				reset_interface(smartcard);
				goto startover;
			}
			
			if (n != g_commands[i].num_send) 
				FAIL();
        
			rc = bsmartcard_read(smartcard, buf, g_commands[i].num_receive, (size_t *)&n);
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
#if 1
            if(g_commands[i].num_send == 1) {
             
                bsmartcard_get_settings(smartcard, &settings);
                        
                switch(g_commands[i].send[0]) {
		        case 0:
			        printf("Switched to 9600 Baud\n");
			        settings.dfactor = 1;
			        settings.ffactor = 1;			
			        break;
        
		        case 64:
                    printf("Switched to 19200 Baud\n");
			        settings.dfactor = 2;
			        settings.ffactor = 1;
			        break;
        
		        case 128:
			        printf("Switched to 38400 Baud\n");
			        settings.dfactor = 3;
			        settings.ffactor = 1;
			        break;
        
		        case 255:
                    printf("Switched to 76800 Baud\n");
			        settings.dfactor = 4;
			        settings.ffactor = 1;
                    settings.extra_guard_time.value= 4;
			        break;
                default:
                    break;
                }

                rc = bsmartcard_set_settings(smartcard, &settings);
                if (rc) {
			    	goto error;
    			}
	        }
#endif            
		}
		printf("Successful transaction.\n");
		counter++;
		loops++;
		continue;
startover:
		i = -1;
		loops = 0;
	}

error:
	bsmartcard_close(smartcard);
	bsmartcard_shutdown();
	return 0;
}

int reset_card(bsmartcard_t smartcard)
{
	unsigned char buf[BUFSIZE];
	int n;
	int j;

	if (bsmartcard_reset_card(smartcard, buf, sizeof(g_resetdata), (size_t *)&n)) 
		FAIL();
	
	if (n != sizeof(g_resetdata)) 
		FAIL();
	
	if (memcmp(buf, g_resetdata, n)) {
		printf("bsmartcard_reset_card failed:\n%d: ", n);
		for (j=0; j<n; j++) 
			printf("0x%02x ", buf[j]);
		printf("\n");
		FAIL();
	}
	else {
		if (bsmartcard_set_params_from_atr(smartcard)) 
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
	while (1) {
		if (bsmartcard_reset(smartcard, false)) 
			continue;

        bsmartcard_detect_card(smartcard);

		if (!reset_card(smartcard))
			break;
	}	
}



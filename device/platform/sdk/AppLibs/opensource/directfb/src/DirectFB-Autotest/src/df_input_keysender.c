/******************************************************************************
 *    (c)2009-2012 Broadcom Corporation
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
 * $brcm_Workfile:  $
 * $brcm_Revision:  $
 * $brcm_Date:  $
 *
 * Module Description:
 *
 * Revision History:
 *
 * $brcm_Log:  $
 *
 *
 *****************************************************************************/


#include <linux/input.h>
#include <time.h>
#include <fcntl.h>
#include <string.h>
#include <errno.h>
#include <stdio.h>
#include <stdbool.h>
#include <unistd.h>
#include <sys/stat.h>

typedef struct {
        unsigned keyCode;
        bool modifier;                  /* true if it needs a left shift modifier*/
} keyMapping;


unsigned buttons[] = {\
	BTN_LEFT,  /*DIBM_LEFT*/\
	BTN_RIGHT,  /*DIBM_RIGHT*/\
	BTN_MIDDLE,  /*DIBM_MIDDLE*/\
};


/*mapping list from DirectFBKeySymbolNames defined in directfb_keynames.h to keycodes defined in linux/input.h*/
keyMapping keyCodes[] = {\
	{KEY_BACKSPACE, false},  /*DIKS_BACKSPACE*/\
	{KEY_TAB, false}, /*DIKS_TAB*/\
	{KEY_ENTER, false}, /*DIKS_RETURN*/\
	{KEY_ESC, false}, /*DIKS_ESCAPE*/\
	{KEY_SPACE, false}, /*DIKS_SPACE*/\
	{KEY_1, true}, /*DIKS_EXCLAMATION_MARK*/\
	{KEY_APOSTROPHE, true}, /*DIKS_QUOTATION*/\
	{KEY_3, true}, /*DIKS_NUMBER_SIGN*/\
	{KEY_4, true}, /*DIKS_DOLLAR_SIGN*/\	
	{KEY_5, true}, /*DIKS_PERCENT_SIGN*/\	
	{KEY_7, true}, /*DIKS_AMPERSAND*/\	
	{KEY_APOSTROPHE, false}, /*DIKS_APOSTROPHE*/\	
	{KEY_9, true}, /*DIKS_PARENTHESIS_LEFT*/\	
	{KEY_0, true}, /*DIKS_PARENTHESIS_RIGHT*/\	
	{KEY_8, true}, /*DIKS_ASTERISK*/\	
	{KEY_EQUAL, true}, /*DIKS_PLUS_SIGN*/\	
	{KEY_COMMA, false}, /*DIKS_COMMA*/\	
	{KEY_MINUS, false}, /*DIKS_MINUS_SIGN*/\	
	{KEY_DOT, false}, /*DIKS_PERIOD*/\	
	{KEY_SLASH, false}, /*DIKS_SLASH*/\	
	{KEY_0, false}, /*DIKS_0*/\	
	{KEY_1, false}, /*DIKS_1*/\	
	{KEY_2, false}, /*DIKS_2*/\	
	{KEY_3, false}, /*DIKS_3*/\	
	{KEY_4, false}, /*DIKS_4*/\	
	{KEY_5, false}, /*DIKS_5*/\	
	{KEY_6, false}, /*DIKS_6*/\	
	{KEY_7, false}, /*DIKS_7*/\	
	{KEY_8, false}, /*DIKS_8*/\	
	{KEY_9, false}, /*DIKS_9*/\	
	{KEY_SEMICOLON, true}, /*DIKS_COLON*/\	
	{KEY_SEMICOLON, false}, /*DIKS_SEMICOLON*/\	
	{KEY_COMMA, true}, /*DIKS_LESS_THAN_SIGN*/\	
	{KEY_EQUAL, false}, /*DIKS_EQUALS_SIGN*/\	
	{KEY_DOT, true}, /*DIKS_GREATER_THAN_SIGN*/\	
	{KEY_SLASH, true}, /*DIKS_QUESTION_MARK*/\	
	{KEY_2, true}, /*DIKS_AT*/\	
	{KEY_A, true}, /*DIKS_CAPITAL_A*/\	
	{KEY_B, true}, /*DIKS_CAPITAL_B*/\	
	{KEY_C, true}, /*DIKS_CAPITAL_C*/\	
	{KEY_D, true}, /*DIKS_CAPITAL_D*/\	
	{KEY_E, true}, /*DIKS_CAPITAL_E*/\	
	{KEY_F, true}, /*DIKS_CAPITAL_F*/\	
	{KEY_G, true}, /*DIKS_CAPITAL_G*/\	
	{KEY_H, true}, /*DIKS_CAPITAL_H*/\	
	{KEY_I, true}, /*DIKS_CAPITAL_I*/\	
	{KEY_J, true}, /*DIKS_CAPITAL_J*/\	
	{KEY_K, true}, /*DIKS_CAPITAL_K*/\	
	{KEY_L, true}, /*DIKS_CAPITAL_L*/\	
	{KEY_M, true}, /*DIKS_CAPITAL_M*/\	
	{KEY_N, true}, /*DIKS_CAPITAL_N*/\	
	{KEY_O, true}, /*DIKS_CAPITAL_O*/\	
	{KEY_P, true}, /*DIKS_CAPITAL_P*/\	
	{KEY_Q, true}, /*DIKS_CAPITAL_Q*/\	
	{KEY_R, true}, /*DIKS_CAPITAL_R*/\	
	{KEY_S, true}, /*DIKS_CAPITAL_S*/\	
	{KEY_T, true}, /*DIKS_CAPITAL_T*/\	
	{KEY_U, true}, /*DIKS_CAPITAL_U*/\	
	{KEY_V, true}, /*DIKS_CAPITAL_V*/\	
	{KEY_W, true}, /*DIKS_CAPITAL_W*/\	
	{KEY_X, true}, /*DIKS_CAPITAL_X*/\	
	{KEY_Y, true}, /*DIKS_CAPITAL_Y*/\	
	{KEY_Z, true}, /*DIKS_CAPITAL_Z*/\	
	{KEY_LEFTBRACE, false}, /*DIKS_SQUARE_BRACKET_LEFT*/\	
	{KEY_BACKSLASH, false}, /*DIKS_BACKSLASH*/\
	{KEY_RIGHTBRACE, false}, /*DIKS_SQUARE_BRACKET_RIGHT*/\		
	{KEY_6, true}, /*DIKS_CIRCUMFLEX_ACCENT */\	
	{KEY_MINUS, true}, /*DIKS_UNDERSCORE*/\	
	{KEY_GRAVE, false}, /*DIKS_GRAVE_ACCENT*/\	
	{KEY_A, false}, /*DIKS_SMALL_A*/\	
	{KEY_B, false}, /*DIKS_SMALL_B*/\	
	{KEY_C, false}, /*DIKS_SMALL_C*/\	
	{KEY_D, false}, /*DIKS_SMALL_D*/\	
	{KEY_E, false}, /*DIKS_SMALL_E*/\	
	{KEY_F, false}, /*DIKS_SMALL_F*/\	
	{KEY_G, false}, /*DIKS_SMALL_G*/\	
	{KEY_H, false}, /*DIKS_SMALL_H*/\	
	{KEY_I, false}, /*DIKS_SMALL_I*/\	
	{KEY_J, false}, /*DIKS_SMALL_J*/\	
	{KEY_K, false}, /*DIKS_SMALL_K*/\	
	{KEY_L, false}, /*DIKS_SMALL_L*/\	
	{KEY_M, false}, /*DIKS_SMALL_M*/\	
	{KEY_N, false}, /*DIKS_SMALL_N*/\	
	{KEY_O, false}, /*DIKS_SMALL_O*/\	
	{KEY_P, false}, /*DIKS_SMALL_P*/\	
	{KEY_Q, false}, /*DIKS_SMALL_Q*/\	
	{KEY_R, false}, /*DIKS_SMALL_R*/\	
	{KEY_S, false}, /*DIKS_SMALL_S*/\	
	{KEY_T, false}, /*DIKS_SMALL_T*/\	
	{KEY_U, false}, /*DIKS_SMALL_U*/\	
	{KEY_V, false}, /*DIKS_SMALL_V*/\	
	{KEY_W, false}, /*DIKS_SMALL_W*/\	
	{KEY_X, false}, /*DIKS_SMALL_X*/\	
	{KEY_Y, false}, /*DIKS_SMALL_Y*/\	
	{KEY_Z, false}, /*DIKS_SMALL_Z*/\	
	{KEY_LEFTBRACE, true}, /*DIKS_CURLY_BRACKET_LEFT*/\	
	{KEY_BACKSLASH, true}, /*DIKS_VERTICAL_BAR*/\		
	{KEY_RIGHTBRACE, true}, /*DIKS_CURLY_BRACKET_RIGHT*/\	
	{KEY_GRAVE, true}, /*DIKS_TILDE*/\	
	{KEY_DELETE, false}, /*DIKS_DELETE*/\	
	{KEY_LEFT, false}, /*DIKS_CURSOR_LEFT*/\	
	{KEY_RIGHT, false}, /*DIKS_CURSOR_RIGHT*/\	
	{KEY_UP, false}, /*DIKS_CURSOR_UP*/\	
	{KEY_DOWN, false}, /*DIKS_CURSOR_DOWN*/\	
	{KEY_INSERT, false}, /*DIKS_INSERT*/\	
	{KEY_HOME, false}, /*DIKS_HOME*/\	
	{KEY_END, false}, /*DIKS_END*/\	
	{KEY_HOME, false}, /*DIKS_HOME*/\	
	{KEY_PAGEUP, false}, /*DIKS_PAGE_UP*/\	
	{KEY_PAGEDOWN, false}, /*DIKS_PAGE_DOWN*/\	
	{KEY_PAUSE, false}, /*DIKS_PAUSE*/\	
	{KEY_POWER, false}, /*DIKS_POWER*/\	
	{KEY_HELP, false}, /*DIKS_HELP*/\	
	{KEY_VOLUMEUP, false}, /*DIKS_VOLUME_UP*/\	
	{KEY_VOLUMEDOWN, false}, /*DIKS_VOLUME_DOWN*/\	
	{KEY_MUTE, false}, /*DIKS_MUTE*/\	
	{KEY_STOP, false}, /*DIKS_STOP*/\	
	{KEY_F1, false}, /*DIKS_F1*/\	
	{KEY_F2, false}, /*DIKS_F2*/\	
	{KEY_F3, false}, /*DIKS_F3*/\	
	{KEY_F4, false}, /*DIKS_F4*/\	
	{KEY_F5, false}, /*DIKS_F5*/\	
	{KEY_F6, false}, /*DIKS_F6*/\	
	{KEY_F7, false}, /*DIKS_F7*/\	
	{KEY_F8, false}, /*DIKS_F8*/\	
	{KEY_F9, false}, /*DIKS_F9*/\	
	{KEY_F10, false}, /*DIKS_F10*/\	
	{KEY_F11, false}, /*DIKS_F11*/\	
	{KEY_F12, false}, /*DIKS_F12*/\	
	{KEY_LEFTSHIFT, false}, /*DIKS_SHIFT*/\	
	{KEY_LEFTALT, false}, /*DIKS_ALT*/\	
	{KEY_LEFTMETA, false}, /*DIKS_META*/\	
	{KEY_CAPSLOCK, false}, /*DIKS_CAPS_LOCK*/\	
	{KEY_CAPSLOCK, false}, /*sending CAPSLOCK twice to return initial state*/\		
	{KEY_NUMLOCK, false}, /*DIKS_NUM_LOCK*/\	
	{KEY_SCROLLLOCK, false} /*DIKS_SCROLL_LOCK*/\	
};



void simulate_key(int fd, unsigned keyCode, int keyValue)
{
	struct input_event event;

	event.type = EV_KEY;
	event.code = keyCode;
	event.value = keyValue;
	gettimeofday(&event.time, 0);

	if (write(fd, &event, sizeof(event)) < 0) {
		printf("Simulate key error\n");
		return;
	}
}


int main ( int argc, char *argv[] )
{
	int i, fd, keyboardHandle = 0, mouseHandle = 0;  

	char *device[2] = {"/dev/input/event0", "/dev/input/event1"};

	char deviceName[256] = "";
	struct stat buffer;	
	

	printf("\nSend key & mouse button events for df_input test...\n");
	printf("Please make sure Caps Lock is turned off before running this test.\n\n");
	
	if ((getuid ()) != 0)
		printf ("@BAT_FAILURE@ You are not root! This may not work...\n");

	if(stat("/dev/input", &buffer))	{		/*check if "/dev/input" exist or not*/
		printf ("@BAT_FAILURE@ Please run \"rundfb.sh install\" before the test.\n");
		return 1;
	}

	
        /*detect mouse and keyboard automatically because the device files vary for USB devices*/	
	for(i=0; i<2; i++){
		fd = open(device[i], O_RDWR);
		if(fd <= 0) {
			printf("@BAT_FAILURE@ Cannot open %s :%s\n", device[i], strerror(errno));
			printf("@BAT_FAILURE@ Please make sure mouse and keyboard are connected to the STB porperly.\n");		
			return 1;
		}

		ioctl (fd, EVIOCGNAME (sizeof (deviceName)), deviceName);
		printf("%s detected\n", deviceName);				
		if(strstr (deviceName, "Mouse") || strstr (deviceName, "mouse")){
			mouseHandle = fd;
		}else if (strstr (deviceName, "Keyboard") || strstr (deviceName, "keyboard")){
			keyboardHandle = fd;
		}
	}

	if(mouseHandle == 0){	
		printf("@BAT_FAILURE@ Mouse may not be porperly connected.\n");		
		return 1;
	}
	if(keyboardHandle == 0){	
		printf("@BAT_FAILURE@ Keyboard was not connected porperly.\n");		
		return 1;
	}


	/*start to send key events after 20s*/
	printf("\nWait for 20 seconds to make sure df_input is loaded properly\n");
	sleep(20);		
	
	for(i=0; i<3; i++){
		simulate_key(mouseHandle, buttons[i], 1);
		simulate_key(mouseHandle, buttons[i], 0); 
		usleep(500000);					/*sleep 0.5 second*/
	}
	close(mouseHandle);
	

	for(i=0; i<sizeof(keyCodes)/sizeof(keyCodes[0]); i++){
		if(keyCodes[i].modifier){
			simulate_key(keyboardHandle, 42, 1);  	/*hold left shift key*/
			usleep(100000);
			simulate_key(keyboardHandle, keyCodes[i].keyCode, 1);
			simulate_key(keyboardHandle, keyCodes[i].keyCode, 0);
			usleep(100000);
			simulate_key(keyboardHandle, 42, 0);  	/*release left shift key*/	
		}
		else{
			simulate_key(keyboardHandle, keyCodes[i].keyCode, 1);
			simulate_key(keyboardHandle, keyCodes[i].keyCode, 0);			
		}
		usleep(500000);	
	}

	/*send ESC twice to finish df_input test*/
	simulate_key(keyboardHandle, 1, 1);
	simulate_key(keyboardHandle, 1, 0); 	
	usleep(500000);
	simulate_key(keyboardHandle, 1, 1);
	simulate_key(keyboardHandle, 1, 0); 		


	close(keyboardHandle);
	printf("\nDone with key sending.\n");
	return 0;
}   


/***************************************************************************
 *     Copyright (c) 2005-2008, Broadcom Corporation
 *     All Rights Reserved
 *     Confidential Property of Broadcom Corporation
 *
 *  THIS SOFTWARE MAY ONLY BE USED SUBJECT TO AN EXECUTED SOFTWARE LICENSE
 *  AGREEMENT  BETWEEN THE USER AND BROADCOM.  YOU HAVE NO RIGHT TO USE OR
 *  EXPLOIT THIS MATERIAL EXCEPT SUBJECT TO THE TERMS OF SUCH AN AGREEMENT.
 *
 * $brcm_Workfile: dcs_test.c $
 * $brcm_Revision: 17 $
 * $brcm_Date: 10/28/08 8:17a $
 *
 * Module Description:
 *
 * Revision History:
 *
 * $brcm_Log: /BSEAV/api/tests/dcs_test.c $
 * 
 * 17   10/28/08 8:17a ahulse
 * PR35165: fix corrupt white background
 * 
 * 16   8/14/08 1:14p ahulse
 * PR35165: Make modes 3 & 4 available as default
 * 
 * 15   6/3/08 9:53a ahulse
 * PR43266: Suppress warnings for string functions
 * 
 * 14   6/3/08 9:39a ahulse
 * PR43266: Add support for DCS in nexus
 * 
 * 13   5/7/08 6:17p ahulse
 * PR35165: Add support for Tertiary Vec for 7400 platforms
 * 
 * 12   3/4/08 2:34p ahulse
 * PR35165: Set Pri & Sec Vec correctly for default mode
 * 
 * 11   2/22/08 9:35a ahulse
 * PR35165: Temporarily revert menu options back to old values
 * 
 * 10   2/20/08 11:23a ahulse
 * PR35165: Simplify default modes available, add version info
 * 
 * 9   1/24/08 7:06a ahulse
 * PR35165: fix variable name error, remove debug statements
 * 
 * 8   1/23/08 1:36p ahulse
 * PR35165: set correct gfx size for ntsc and pal
 * 
 * 7   1/21/08 10:04a ahulse
 * PR35165: Use graphic drawing for 7038 platforms, not background color
 * 
 * 6   1/17/08 8:48a ahulse
 * PR35165: Add "-white" option so you can set off white background when
 * using nodecode
 * 
 * 5   12/21/07 11:12a ahulse
 * PR35165: Improve key reading, so backspace etc is ignored
 * 
 * 4   11/30/07 9:43a ahulse
 * PR35165: On 7038 / 7400 based platforms clone from window 0 to 2
 *
****************************************************************************/

#include "bsettop.h"
#include "bstd.h"
#include <stdio.h>
#include <string.h>
#include <bstd.h>
#include <bkni.h>

BDBG_MODULE(dcs_test);

char dcsTestVersion[] = "1.3";		/* Update version when check in a new file ! */

/* #define FINAL - use to temproraily maintain menu "legacy" order until current DCS cert finishes */

bdcs_type gKeyVal = bdcs_type_dcs_off;
int fullTest = 1;	/* If non Zero all output combinations of primary and secondary vec allowed */

void locked(void *data)
{
    BKNI_SetEvent((BKNI_EventHandle)data);
}

int showHelp( void ) {

	printf("\n\nDCS signal test app - dcs_test\n\n" );
	printf("Usage :\n" );
	printf("  --help             print this usage guide\n" );
	printf("  --examples         show example usage\n" );
	printf("  -q <frequency>     tune to QAM 64 channel at 'frequency' \n" );
	printf("  -f <filename>      open file filename for playback (must be .mpg extension)\n" );
	printf("  -nodecode          if no playback file or tuner avalable, use test app without decoding video\n" );
	printf("  -vidpid <videopid> Use video pid 'videopid' in the stream \n" );
	printf("  -audpid <audiopid> Use audio pid 'audiopid' in the stream \n" );
	printf("  -white             Sets a white background when have no signal\n" );
	printf("  -fulltest          Allows full testing of all output modes\n" );
	printf("\n" );

	return 0;
}

int showExamples( void ) {

	printf("\n\n Example usage of dcs_test \n\n" );
	printf("  ./settop dcs_test -f videos/brutus6.mpg -vidpid 0x31 -audpid 0x34 \n" );
	printf("  ./settop dcs_test -q 765000000 -vidpid 0x31 -audpid 0x34 \n" );
	printf("  ./settop dcs_test -nodecode -white\n" );
	printf("\n" );
}

int printMenu( bdisplay_settings priSettings, bdisplay_settings secSettings, int selection, int isPal) {

	char c,d=0, tempKey;
	char componentString[21];
	char terVecString[21];

#ifndef FINAL
	switch( selection ) {
		case 3: selection = 5; break;   /* map 3 to 5 */
		case 4: if( fullTest ) selection = 3; break;   /* map 4 to 3 */
		case 5: if( fullTest ) selection = 4; break;   /* map 5 to 4 */
		default: break;               /* do nothing */
	}
#endif

	printf( "\n\n" );
	printf( "    DWIGHT CAVENDISH SYSTEMS COPY PROTECTION TEST PROGRAM \n" );
	printf( "                     ( dcs_test v %s ) \n\n", dcsTestVersion );
	printf( "                    CVBS/SVideo   Component\n" );
	printf( "1, Pri  Vec Only       %s", isPal ? "575i            OFF  \n" : "480i            OFF  \n" );
	printf( "---------------\n" );
	printf( "2, Sec  Vec only       %s", isPal ? "575i            575i \n" :"480i            480i \n" );
	printf( "---------------\n" );
#if BCHP_CHIP == 7400
	printf( "a, Tert Vec only       %s", isPal ? "576i            OFF  \n" : "480i            OFF  \n" );
	printf( "---------------\n" );
#endif

	if( fullTest ) {
#ifdef FINAL
		printf( "4, Pri Vec + Sec Vec   %s", isPal ? "575i            575i \n" : "480i            480i \n" );
		printf( "---------------\n" );
		printf( "5, Pri Vec + Sec Vec   %s", isPal ? "575i            575p \n" : "480i            480p \n" );
#else
		printf( "3, Pri Vec + Sec Vec   %s", isPal ? "575i            575i \n" : "480i            480i \n" );
		printf( "---------------\n" );
		printf( "4, Pri Vec + Sec Vec   %s", isPal ? "575i            575p \n" : "480i            480p \n" );
#endif
		printf( "---------------\n" );
	}

#ifdef FINAL
	printf( "3, Sec  Vec only       %s", isPal ? "OFF             575p \n" : "OFF             480p \n" );
#else
	printf( "5, Sec  Vec only       %s", isPal ? "OFF             575p \n" : "OFF             480p \n" );

#endif
	printf( "--------------------------\n" );

	printf( "\n6, Toggle NTSC / PAL \n" );
	printf( "--------------------------\n" );
	printf( "7, Set Key (1,2 or 3) \n" );
	printf( "--------------------------\n" );
	printf( "8, DCS OFF\n" );
	printf( "9, DCS ON\n" );
	printf( "--------------------------\n" );
	printf( "0, Exit program\n\n" );

	printf( "Current Selection: %d  Format %s \n", selection , isPal ? "PAL":"NTSC" );

	if( priSettings.component == NULL ) {
		/* Primary Vec only , therefore NO Component Output */
		strcpy( componentString, "Component OFF" );
	}
	else {
		sprintf( componentString, "Component %s", isPal ? (priSettings.format == bvideo_format_pal) ? "575i":"575p":(priSettings.format == bvideo_format_ntsc) ? "480i":"480p" );
	}
#if BCHP_CHIP == 7400
	sprintf( terVecString, " Ter Vec %s ", selection==0xa ? "ON":"OFF" );
#else
	strcpy( terVecString, " " );
#endif
	printf( "Pri Vec %s Sec Vec %s%s%s, DCS %s (key=%d)\n",
			(secSettings.composite == NULL) ? "OFF":"ON",
			(priSettings.component == NULL) ? "OFF":"ON",
			terVecString,
			componentString,
			(priSettings.dcs_type  == bdcs_type_dcs_off) ? "OFF":"ON" , gKeyVal-bdcs_type_dcs_off );

	printf( "\n" );

	c = getchar();
	if( c == 0xa ) /* User just hits return key */
		goto end;

#if BCHP_CHIP == 7400
	if( c == 0X41 || c == 0x61 ) {
		c = 0x3a;
		printf("Set key to 0x%x", c );
	}
#endif
	if( c < 0x30 && c > 0x3a ) {
		c = 0x29;
		printf("limited key to 0x%x", c );
	}
#ifndef FINAL
	switch( c ) {
		case 0x35: c = 0x33; break;   /* map 5 to 3 */
		case 0x33: if( fullTest ) c = 0x34; else c = 0x29; break;   /* map 3 to 4 */
		case 0x34: if( fullTest ) c = 0x35; else c = 0x29; break;   /* map 4 to 5 */
		default: break;               /* do nothing */
	}
#endif

	while( d != 0xa ) {
		d = getchar(); /* read all chars out until we get a return char */
	}

	if( c-0x30 == 7 ) {
		/* get key value */
tryAgain:
		printf("Type in key value: " );
		tempKey = getchar();
		d = getchar(); /* get return char */
		if ( tempKey < 0x31 || tempKey > 0x33 ) {
			printf("\n Incorrect Key Value !\n\n" );
			printf("Valid values : 1, 2 or 3 only !\n" );
			goto tryAgain;
		}
#if USE_ENUM_VALS
		switch( tempKey - 0x30 ) {
			case 1: gKeyVal = bdcs_type_dcs_on_key1; break;
			case 2: gKeyVal = bdcs_type_dcs_on_key2; break;
			case 3: gKeyVal = bdcs_type_dcs_on_key3; break;
			default : gKeyVal = bdcs_type_dcs_on_key1; break;
		}
#else
		gKeyVal = tempKey - 0x30;
#endif
	}
end:
	return (int)(c - 0x30);
}

/* Example playback usage :
	./settop dcs_test -f videos/brutus3.mpg -vidpid 0x31 -audpid 0x34
*/

int main( int argc, char **argv ) {

	bresult                 rc;
	btuner_t                tuner=NULL;
	bdecode_t               decode;
	bdisplay_t              displayPri,displaySec,displayTer;
	bdecode_window_t        windowPri=NULL,windowSec=NULL;
	bplaypump_t             playpump=NULL;
	bplayback_t             playback=NULL;
	bplayback_file_t        file=NULL;
	bplayback_params        playback_params;
	bvideo_format           VideoFormat = bvideo_format_ntsc;

	bstream_t               stream=NULL;
	bstream_mpeg            mpeg;
	btuner_qam_params       qam;
	bband_t                 band=NULL;
	BKNI_EventHandle        event;
	bdisplay_settings       priSettings,secSettings,
                            initialPriSettings, initialSecSettings ;
#if BCHP_CHIP ==7400
	bdisplay_settings       terSettings;
#endif

	/* For graphic output */
    bgraphics_t graphicsPri=NULL;
    bsurface_t  surfacePri=NULL, fbPri=NULL;
#if BCHP_CHIP == 7038
	bgraphics_t graphicsSec=NULL;
    bsurface_t  surfaceSec=NULL, fbSec=NULL;
#endif
	bgraphics_settings gfx_cfg;
    bsurface_create_settings create_settings;
    bsurface_settings surface_settings;
    bsettop_rect r = {0,0,720,480};

	int                     crntMode,selection=0;
	int                     curarg  = 1;
	int                     qamFreq = 0;
	int                     isPal   = 0;
	bool					doFlatField  = false;
	bool                    playbackMode = false;
	bool                    doDecode     = true;
	uint16_t                audioPid=0xffff, videoPid=0xffff;
	char                    fileName[61], mpegFileName[61],navFileName[61];
	char                    *pIndexFile;

	if ( argc < 2 )  {
		showHelp();
		return 1;
	}
	/* parse params before starting settop api */
	while (curarg < argc) {
		if (!strcmp("-f", argv[curarg])) {
			playbackMode = true;
			strcpy( fileName, argv[++curarg] );
		}
		else if ( !strcmp("-audpid", argv[curarg]) ) {
			curarg++;
			if( curarg < argc ) 
				audioPid = (int)strtol(argv[curarg], NULL, 0);
		}
		else if ( !strcmp("-vidpid", argv[curarg]) ) {
			curarg++;
			if( curarg < argc ) 
				videoPid = (int)strtol(argv[curarg], NULL, 0);
		}
		else if ( !strcmp("-q", argv[curarg]) ) {
			curarg++;
			if( curarg < argc ) 
				qamFreq = (int)strtol(argv[curarg], NULL, 0);
		}
		else if ( !strcmp("-nodecode", argv[curarg]) ) {
			doDecode = false;
		}
		else if ( !strcmp("-white", argv[curarg]) ) {
			doFlatField = true;
		}
		else if ( !strcmp("--examples", argv[curarg]) ) {
			showExamples();
			return 1;
        }
		else if ( !strcmp("-fulltest", argv[curarg]) ) {
			fullTest = 1;
        }
		else if ( !strcmp("--help", argv[curarg]) ) {
			showHelp();
			return 1;
        }
		else {
		}
		curarg++;
	}

	if( videoPid == 0xffff ) videoPid = 0x31;
	if( audioPid == 0xffff ) audioPid = 0x34;
	if( qamFreq  == 0      ) qamFreq  = 765000000;

	rc = bsettop_init(BSETTOP_VERSION);

	if( playbackMode ) {
		playpump = bplaypump_open(B_ID(0), NULL);
		playback = bplayback_open();
	}
	else {
		tuner = btuner_open(bconfig->resources.qam.objects[0]);
		BKNI_CreateEvent(&event);
	}

	decode		= bdecode_open(B_ID(0));
	displayPri	= bdisplay_open(B_ID(0));
	displaySec	= bdisplay_open(B_ID(1));
#if BCHP_CHIP ==7400
	displayTer	= bdisplay_open(B_ID(2));
#endif

#if BCHP_CHIP == 7038 || BCHP_CHIP==7400 || BCHP_CHIP==7405
	windowPri	= bdecode_window_open(B_ID(0), displayPri);
	windowSec	= bdecode_window_clone( windowPri, B_ID(2), displaySec );
#else
	windowPri	= bdecode_window_open(B_ID(0), displayPri);
	windowSec	= bdecode_window_clone( windowPri, B_ID(1), displaySec );
#endif

	if (rc) goto error;

	VideoFormat = bvideo_format_ntsc;

	/* switch to dual VEC: HD and SD */
	bdisplay_get(displayPri, &priSettings);
	priSettings.format      = VideoFormat;
	priSettings.svideo    	= boutput_svideo_open(B_ID(0));
	priSettings.composite 	= boutput_composite_open(B_ID(0));
	priSettings.composite2 	= NULL;
	priSettings.rf        	= boutput_rf_open(B_ID(0));

#if BCHP_CHIP == 7400 || BCHP_CHIP==7401 || BCHP_CHIP==7405
	priSettings.background_color  	= 0xFF7F7F7F;
#endif
	memcpy( &initialPriSettings, &priSettings, sizeof(bdisplay_settings) );

	bdisplay_set( displayPri, &priSettings );

	bdisplay_get( displaySec, &secSettings );
	secSettings.format      = VideoFormat;
	secSettings.svideo    	= NULL;
	secSettings.composite 	= NULL;
	secSettings.composite2 	= NULL;
	secSettings.rf        	= NULL;
	secSettings.component  	= NULL;

#if BCHP_CHIP == 7400 || BCHP_CHIP==7401 || BCHP_CHIP==7405
	secSettings.background_color  	= 0xFF7F7F7F;
#endif
	bdisplay_set(displaySec, &secSettings);

	memcpy( &initialSecSettings, &secSettings, sizeof(bdisplay_settings) );

#if BCHP_CHIP ==7400
	bdisplay_get( displayTer, &terSettings );
	terSettings.format      = VideoFormat;
	terSettings.svideo      = NULL;
	terSettings.composite   = NULL;
	terSettings.composite2  = NULL;
	terSettings.rf          = NULL;
	terSettings.component   = NULL;
	terSettings.background_color    = 0xFF7F7F7F;
	bdisplay_set(displayTer, &terSettings);
#endif

	if ( doFlatField ) {
		graphicsPri = bgraphics_open(B_ID(0),displayPri);
		bsurface_create_settings_init(&create_settings, graphicsPri);
		create_settings.width = 720;
		create_settings.height = 576;
		create_settings.pixel_format = bgraphics_pixel_format_r5_g6_b5;

		fbPri = bgraphics_create_framebuffer( graphicsPri, &create_settings, false );

		bgraphics_get( graphicsPri, &gfx_cfg );
		gfx_cfg.destination_width  = 720;
		gfx_cfg.destination_height = 480;
		gfx_cfg.source_width       = 720;
		gfx_cfg.source_height      = 480;
		bgraphics_set( graphicsPri, &gfx_cfg );

		surfacePri = bsurface_create(graphicsPri, &create_settings);

		bsurface_get(surfacePri, &surface_settings);
		surface_settings.position.x = 0;
		surface_settings.position.y = 0;
		surface_settings.visible = true;
		bsurface_set(surfacePri, &surface_settings);

		bsurface_fill(surfacePri, &r, 0xFF7F7F7F); /* RGB565 white */
		bgraphics_sync(graphicsPri);

#if BCHP_CHIP == 7038
		graphicsSec = bgraphics_open(B_ID(1),displaySec);
		bsurface_create_settings_init(&create_settings, graphicsSec);
		create_settings.width = 720;
		create_settings.height = 576;
		create_settings.pixel_format = bgraphics_pixel_format_r5_g6_b5;

		fbSec = bgraphics_create_framebuffer( graphicsSec, &create_settings, true );

		bgraphics_get( graphicsSec, &gfx_cfg );
		gfx_cfg.destination_width  = 720;
		gfx_cfg.destination_height = 480;
		gfx_cfg.source_width       = 720;
		gfx_cfg.source_height      = 480;
		bgraphics_set( graphicsPri, &gfx_cfg );

		surfaceSec = bsurface_create(graphicsSec, &create_settings);

		bsurface_get(surfaceSec, &surface_settings);
		surface_settings.position.x = 0;
		surface_settings.position.y = 0;
		surface_settings.visible = true;
		bsurface_set(surfaceSec, &surface_settings);

		bsurface_fill(surfaceSec, &r, 0xFF7F7F7F); /* RGB565 white */
		bgraphics_sync(graphicsSec);
#endif
	}

	crntMode = 2; /* sec vec only */

	if( doDecode ) {
		if( playbackMode ) {
			char * suffixPtr;

			bplayback_params_init(&playback_params, playback);
			playback_params.index_format = bindex_format_bcm;

			suffixPtr = NULL;
			strcpy( navFileName , fileName );

			if( (suffixPtr = strstr( navFileName, ".mpg" )) != NULL ) {
				*suffixPtr = 0;	
				strcat( navFileName, ".nav" );
			}
			else {
				printf("\n\n Can't find mpg extension, Aborting program !\n\n");
				printf("\n Need files filename.mpg AND filename.nav to playback a file \n\n" );
				goto error;
			}
			pIndexFile = navFileName;
tryagain:
			printf("opening %s\n", fileName );
			file = bplayback_file_open( fileName, pIndexFile );
			if (!file) {
				if( pIndexFile ) {
					pIndexFile = NULL;
    				playback_params.index_format = bindex_format_none;
					printf("Failed to open indexfile, try again without it...\n" );
					goto tryagain;
				}
				goto error;
			}
		} else {
			/* Use tuner */
			btuner_qam_params_init(&qam, tuner);
			qam.symbol_rate		= 5056900;
			qam.mode			= 64;
			qam.wait_for_lock	= false;
			qam.lock_callback	= locked;
			qam.callback_context = event;

			/* Tune to a QAM 64 signal at 765 Mhz, change */
			/* according to your setup                    */
			band = btuner_tune_qam(tuner, qamFreq, &qam);
			if (!band) goto error;

			printf(" Waiting to lock to signal.....\n" );
			printf(" Ctrl-C to abort ! \n" );
			BKNI_WaitForEvent(event, 0xFFFFFFFF);
			printf(" Locked to signal , continuing \n" );
		}

		bstream_mpeg_init(&mpeg);
	
		mpeg.video[0].pid       = videoPid;
		mpeg.audio[0].pid       = audioPid;
		mpeg.audio[0].format    = baudio_format_ac3;
		mpeg.pcr_pid            = videoPid;

		if( playbackMode ) {
			bplayback_params_init(&playback_params, playback);
			playback_params.index_format = bindex_format_bcm;
		
			stream = bplayback_start(playback, playpump, &mpeg, file, &playback_params);
			if (!stream) goto error;
			printf("Started playback OK\n" );
		}
		else {
			stream = bstream_open(band, &mpeg);
			if (!stream) goto error;
		}

		if (bdecode_start(decode, stream, windowPri))
			goto error;

	} /* end of doDecode = true */

	if ( VideoFormat == bvideo_format_pal ) isPal = 1;

	do { 
		selection = printMenu( priSettings,secSettings, crntMode, isPal );
		if( selection == crntMode ) 
			continue;

		switch( selection ) {
			case 0:		/* Exit Program */
			break;
#if BCHP_CHIP == 7400
			case 0xa:		/* Ter Vec Only */
				/* But first null out video handles */
				/* displayPri may have */
				bdisplay_get(displayPri, &priSettings);
				priSettings.svideo     = NULL;
				priSettings.composite  = NULL;
				priSettings.composite2 = NULL;
				priSettings.rf         = NULL;
				priSettings.component  = NULL;
				bdisplay_set(displayPri, &priSettings );	
	
				bdecode_stop( decode );
	
				bdisplay_get( displaySec, &secSettings );
				secSettings.format      = isPal ? bvideo_format_pal : bvideo_format_ntsc;
				secSettings.svideo      = NULL;
				secSettings.composite   = NULL;
				secSettings.composite2 	= NULL;
				secSettings.rf          = NULL;
				bdisplay_set(displaySec, &secSettings);

				bdisplay_get( displayTer, &terSettings );
				terSettings.format      = isPal ? bvideo_format_pal : bvideo_format_ntsc;
				terSettings.svideo      = NULL;
				terSettings.composite   = NULL;
				terSettings.composite2 	= boutput_composite_open(B_ID(1));
				terSettings.rf          = NULL;
				bdisplay_set(displayTer, &terSettings);

				if( doDecode ) {
					if (bdecode_start(decode, stream, windowPri))
						goto error;
				}
				crntMode = selection;
				break;
#endif
			case 1:		/* Pri Vec Only */

				if( crntMode == 2 || crntMode == 3 || crntMode == 0xa ) {
					/* Need to clone window */

					/* But first null out video handles */
					/* displayPri may have */
					bdisplay_get(displayPri, &priSettings);
					priSettings.svideo     = NULL;
					priSettings.composite  = NULL;
					priSettings.composite2 = NULL;
					priSettings.rf         = NULL;
					priSettings.component  = NULL;
					bdisplay_set(displayPri, &priSettings );	
	
					bdecode_stop( decode );
				
#if BCHP_CHIP == 7400
					bdisplay_get( displayTer, &terSettings );
					terSettings.format      = isPal ? bvideo_format_pal : bvideo_format_ntsc;
					terSettings.composite2 	= NULL;
					bdisplay_set(displayTer, &terSettings);
#endif	
					if( !windowSec ) { 
						BDBG_MSG(("Cloning sec Window"));
						windowSec = bdecode_window_clone( windowPri, B_ID(1), displaySec );
					}

					bdisplay_get( displaySec, &secSettings );
					secSettings.format      = isPal ? bvideo_format_pal : bvideo_format_ntsc;
					secSettings.svideo      = boutput_svideo_open(B_ID(0));
					secSettings.composite   = boutput_composite_open(B_ID(0));
					secSettings.composite2 	= NULL;
					secSettings.rf          = boutput_rf_open(B_ID(0));
					bdisplay_set(displaySec, &secSettings);

					if( doDecode ) {
						if (bdecode_start(decode, stream, windowPri))
							goto error;
					}
				}
				else {
					/* Must have been Mode 4 or 3 */
					/* All we need to do is turn component off */
					bdisplay_get(displayPri, &priSettings);
					priSettings.component = NULL;
					priSettings.composite2 = NULL;
					bdisplay_set(displayPri, &priSettings );	
				}
				crntMode = selection;
				break;
			case 2:		/* Sec Vec Only */
			case 3:		/* Sec Vec Only */
				/* Null out composite handles, stop decode */
				/* and then start up decode again */
				bdisplay_get( displaySec, &secSettings );
				secSettings.format      = isPal ? bvideo_format_pal: bvideo_format_ntsc;
				secSettings.svideo      = NULL;
				secSettings.composite   = NULL;
				secSettings.composite2  = NULL;
				secSettings.rf          = NULL;
				bdisplay_set(displaySec, &secSettings);

				if( doDecode ) {
					bdecode_stop( decode );
				}
#if BCHP_CHIP == 7400
				bdisplay_get( displayTer, &terSettings );
				terSettings.format      = isPal ? bvideo_format_pal : bvideo_format_ntsc;
				terSettings.composite2 	= NULL;
				bdisplay_set(displayTer, &terSettings);
#endif	

				if( selection == 2 ) {
					/* 480i output on composite and component */
					bdisplay_get(displayPri, &priSettings);
					priSettings.format    = isPal ? bvideo_format_pal : bvideo_format_ntsc;
					priSettings.svideo    = boutput_svideo_open(B_ID(0));
					priSettings.composite = boutput_composite_open(B_ID(0));
					priSettings.composite2= NULL;
					priSettings.rf        = boutput_rf_open(B_ID(0));
					priSettings.component = initialPriSettings.component;

					bdisplay_set(displayPri, &priSettings );	
				}
				else {
					/* Specifically null out */
					bdisplay_get(displayPri, &priSettings);
					priSettings.format    = isPal ? bvideo_format_576p : bvideo_format_480p;
					priSettings.svideo    = NULL;
					priSettings.composite = NULL;
					priSettings.composite2= NULL;
					priSettings.rf        = NULL;
					priSettings.component = initialPriSettings.component;

					bdisplay_set(displayPri, &priSettings );	
				}

				if( doDecode ) {
					if (bdecode_start(decode, stream, windowPri))
						goto error;
				}

				crntMode = selection;
				break;
			case 4:		/* Pri Vec + Sec Vec 480i 480 i*/
			case 5:		/* Pri Vec + Sec Vec 480i 480p */
				if( crntMode != 2 && crntMode != 3 && crntMode != 0xa ) {
					/* simply change format of Sec Vec */
					/* and make sure its enabled */
					bdisplay_get(displayPri, &priSettings);

					if( selection == 4 ) priSettings.format = isPal ? bvideo_format_pal : bvideo_format_ntsc;
					else priSettings.format = isPal ? bvideo_format_576p: bvideo_format_480p;

					priSettings.component = initialPriSettings.component;
					bdisplay_set(displayPri, &priSettings );	
				}
				else {
					/* Must be 2 or 3 so clone Pri again */
					/* But first null out video handles */
					/* displayPri may have */
					bdisplay_get(displayPri, &priSettings);
					priSettings.svideo    = NULL;
					priSettings.composite = NULL;
					priSettings.rf        = NULL;
					priSettings.component = initialPriSettings.component;

					if( selection == 4 ) priSettings.format = isPal ? bvideo_format_pal : bvideo_format_ntsc;
					else priSettings.format = isPal ? bvideo_format_576p : bvideo_format_480p;

					bdisplay_set(displayPri, &priSettings );	
	
					if( doDecode ) {
						bdecode_stop( decode );
					}
	
					if( !windowSec ) {
						BDBG_MSG(("Cloning sec Window"));
						windowSec = bdecode_window_clone( windowPri, B_ID(1), displaySec );
					}

					bdisplay_get( displaySec, &secSettings );
					secSettings.format		= isPal ? bvideo_format_pal : bvideo_format_ntsc;
					secSettings.svideo		= boutput_svideo_open(B_ID(0));
					secSettings.composite	= boutput_composite_open(B_ID(0));
					secSettings.rf			= boutput_rf_open(B_ID(0));
					bdisplay_set(displaySec, &secSettings);

					if( doDecode ) {
						if (bdecode_start(decode, stream, windowPri))
							goto error;
					}

				}
				crntMode = selection;
				break;
			case 6:		/* Toggle format NTSC->PAL->NTSC */
				if( !isPal  ) {
					isPal = 1;

					bdisplay_get(displayPri, &priSettings);
					priSettings.format      = priSettings.format == bvideo_format_480p ? bvideo_format_576p : bvideo_format_pal;
					bdisplay_set(displayPri, &priSettings);

					bdisplay_get( displaySec, &secSettings );
					secSettings.format      = secSettings.format == bvideo_format_480p ? bvideo_format_576p : bvideo_format_pal;
					bdisplay_set(displaySec, &secSettings);

					if ( doFlatField ) {
						/* Redraw background */
						bgraphics_get(graphicsPri, &gfx_cfg);
						gfx_cfg.destination_height = 576;
						gfx_cfg.source_height = 576;
						r.height=576;
						bgraphics_set(graphicsPri, &gfx_cfg);

						bsurface_fill(surfacePri, &r, 0xFF7F7F7F); /* RGB565 white */
						bgraphics_sync(graphicsPri);

#if BCHP_CHIP == 7038
						bgraphics_get(graphicsSec, &gfx_cfg);
						gfx_cfg.destination_height = 576;
						gfx_cfg.source_height = 576;
						r.height=576;
						bgraphics_set(graphicsSec, &gfx_cfg);

						bsurface_fill(surfaceSec, &r, 0xFF7F7F7F); /* RGB565 white */
						bgraphics_sync(graphicsSec);
#endif
					}
				}
				else {
					isPal = 0;

					bdisplay_get(displayPri, &priSettings);
					priSettings.format      = priSettings.format == bvideo_format_576p ? bvideo_format_480p : bvideo_format_ntsc;
					bdisplay_set(displayPri, &priSettings);

					bdisplay_get( displaySec, &secSettings );
					secSettings.format     = secSettings.format == bvideo_format_576p ? bvideo_format_480p : bvideo_format_ntsc;
					bdisplay_set(displaySec, &secSettings);
					if ( doFlatField ) {
						bgraphics_get(graphicsPri, &gfx_cfg);
						gfx_cfg.destination_height = 480;
						gfx_cfg.source_height = 480;
						bgraphics_set(graphicsPri, &gfx_cfg);
						r.height=480;
						bsurface_fill(surfacePri, &r, 0xFF7F7F7F); /* RGB565 white */
						bgraphics_sync(graphicsPri);
#if BCHP_CHIP == 7038
						bgraphics_get(graphicsSec, &gfx_cfg);
						gfx_cfg.destination_height = 480;
						gfx_cfg.source_height = 480;
						bgraphics_set(graphicsSec, &gfx_cfg);
						r.height=480;
						bsurface_fill(surfaceSec, &r, 0xFF7F7F7F); /* RGB565 white */
						bgraphics_sync(graphicsSec);
#endif
					}
				}
					
				break;
			case 8:		/* DCS OFF */
				bdisplay_get(displayPri, &priSettings);
				priSettings.dcs_type  = bdcs_type_dcs_off;
				bdisplay_set(displayPri, &priSettings );	

				bdisplay_get(displaySec, &secSettings);
				secSettings.dcs_type  = bdcs_type_dcs_off;
				bdisplay_set(displaySec, &secSettings );	

#if BCHP_CHIP == 7400
				bdisplay_get(displayTer, &terSettings);
				terSettings.dcs_type  = bdcs_type_dcs_off;
				bdisplay_set(displayTer, &terSettings );	
#endif
				break;
			case 7:		/* Set key val */
			case 9:		/* DCS ON */
#if USE_ENUM_VALS
				if ( gKeyVal == bdcs_type_dcs_off ) gKeyVal = bdcs_type_dcs_on_key1;
#else
				if ( gKeyVal == bdcs_type_dcs_off ) gKeyVal = bdcs_type_dcs_on;
#endif
				bdisplay_get(displayPri, &priSettings);
				priSettings.dcs_type  = gKeyVal;
				bdisplay_set(displayPri, &priSettings );	

				bdisplay_get(displaySec, &secSettings);
				secSettings.dcs_type  = gKeyVal;
				bdisplay_set(displaySec, &secSettings );	

#if BCHP_CHIP == 7400
				bdisplay_get(displayTer, &terSettings);
				terSettings.dcs_type  = gKeyVal;
				bdisplay_set(displayTer, &terSettings );	
#endif
				break;
			default:
#if 0 
				printf("\n\nInvalid selection %d!\n", selection );
				printf("Try again !\n\n\n" );
				selection = crntMode;
#endif
				break;
		}

#if BCHP_CHIP == 7038
		if ( doFlatField ) {
			bgraphics_get(graphicsPri, &gfx_cfg);
			gfx_cfg.enabled = true;
			bgraphics_set(graphicsPri, &gfx_cfg);

			bgraphics_get(graphicsSec, &gfx_cfg);
			gfx_cfg.enabled = true;
			bgraphics_set(graphicsSec, &gfx_cfg);
		}
#endif

	} while ( selection );

	if( doDecode ) {
		bdecode_stop(decode);
	}

	if( playbackMode ) {
		bplayback_stop(playback);
		bplayback_file_close(file);
		if (playpump)
			bplaypump_close(playpump);
	}

	return 0;
error:
	if (playback)	bplayback_close(playback);
	if (playpump)	bplaypump_close(playpump);

	return 1;
}

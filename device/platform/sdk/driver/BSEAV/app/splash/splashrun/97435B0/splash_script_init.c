/***************************************************************************
 *     Copyright (c) 2003-2012, Broadcom Corporation
 *     All Rights Reserved
 *     Confidential Property of Broadcom Corporation
 *
 *  THIS SOFTWARE MAY ONLY BE USED SUBJECT TO AN EXECUTED SOFTWARE LICENSE
 *  AGREEMENT  BETWEEN THE USER AND BROADCOM.  YOU HAVE NO RIGHT TO USE OR
 *  EXPLOIT THIS MATERIAL EXCEPT SUBJECT TO THE TERMS OF SUCH AN AGREEMENT.
 *
 * $brcm_Workfile: splash_script_init.c $
 * $brcm_Revision: 1 $
 * $brcm_Date: 11/9/12 5:42p $
 *
 * Module Description:
 *
 * Revision History:
 *
 * $brcm_Log: /BSEAV/app/splash/splashrun/97435B0/splash_script_init.c $
 * 
 * 1   11/9/12 5:42p ltokuda
 * SW7435-114:  Add support for 97435 platform(s).
 * 
 *
 ***************************************************************************/
#ifdef CFG_SPLASH_PAL /* pal support */

#include "splash_vdc_reg_pal_2t.h"
#include "splash_vdc_reg_pal_4t.h"

#else /* NTSC */

#include "splash_vdc_reg_def_2t.h"
#include "splash_vdc_reg_def_4t.h"

#endif
 
extern uint32_t aulReg[]; 
void splash_script_init(int select_4t);


#ifdef CFG_SPLASH_PAL /* pal support */

#define SPLASH_WIDTH_2T 	720
#define SPLASH_HEIGHT_2T 	576
#define SPLASH_PHYS_ADDR_2T	0xa0000080
#define SPLASH_PITCH_2T		1440
#define SPLASH_FORMAT_2T	0x06e40565
#define VIDEO_FORMAT_2T		43
#define VIDEO_FORMAT2_2T	10

#define SPLASH_WIDTH_4T 	720
#define SPLASH_HEIGHT_4T 	576
#define SPLASH_PHYS_ADDR_4T	0x043f8100
#define SPLASH_PITCH_4T		1440
#define SPLASH_FORMAT_4T	0x06e40565
#define VIDEO_FORMAT_4T		43
#define VIDEO_FORMAT2_4T	10

#else /* NTSC */

#define SPLASH_WIDTH_2T 	720
#define SPLASH_HEIGHT_2T 	480
#define SPLASH_PHYS_ADDR_2T	0xa0000080
#define SPLASH_PITCH_2T		1440
#define SPLASH_FORMAT_2T	0x06e40565
#define VIDEO_FORMAT_2T		28
#define VIDEO_FORMAT2_2T	0

#define SPLASH_WIDTH_4T 	720
#define SPLASH_HEIGHT_4T 	480
#define SPLASH_PHYS_ADDR_4T	0x043f8100
#define SPLASH_PITCH_4T		1440
#define SPLASH_FORMAT_4T	0x06e40565
#define VIDEO_FORMAT_4T		28
#define VIDEO_FORMAT2_4T	0

#endif


uint32_t       gSplashWidth;
uint32_t       gSplashHeight;
uint32_t       gSplashPhysAddr; 
uint32_t       gSplashPitch;
BPXL_Format    gSplashFormat; /* BPXL_eR5_G6_B5 */
BFMT_VideoFmt  gVideoFormat; /* BFMT_VideoFmt_e480p */
BFMT_VideoFmt  gVideoFormat2; /* BFMT_VideoFmt_eNTSC */



void splash_script_init(int select_4t)
{
	int i;

    if (select_4t)
	{
		gSplashWidth    = SPLASH_WIDTH_4T;
		gSplashHeight   = SPLASH_HEIGHT_4T;
		gSplashPhysAddr = SPLASH_PHYS_ADDR_4T; 
		gSplashPitch    = SPLASH_PITCH_4T;
		gSplashFormat   = (BPXL_Format)SPLASH_FORMAT_4T; /* BPXL_eR5_G6_B5 */
		gVideoFormat    = (BFMT_VideoFmt)VIDEO_FORMAT_4T; /* BFMT_VideoFmt_e480p */
		gVideoFormat2   = (BFMT_VideoFmt)VIDEO_FORMAT2_4T; /* BFMT_VideoFmt_eNTSC */

	    for(i=0; i<(int)(sizeof(aulReg_4t)/sizeof(uint32_t)); i++)
    	{
        	/* Write dumped register value */
        	BREG_Write32(hRegister, aulReg[i], aulReg_4t[i]);
	    }
	}
	else
	{
		gSplashWidth    = SPLASH_WIDTH_2T;
		gSplashHeight   = SPLASH_HEIGHT_2T;
		gSplashPhysAddr = SPLASH_PHYS_ADDR_2T; 
		gSplashPitch    = SPLASH_PITCH_2T;
		gSplashFormat   = (BPXL_Format)SPLASH_FORMAT_2T; /* BPXL_eR5_G6_B5 */
		gVideoFormat    = (BFMT_VideoFmt)VIDEO_FORMAT_2T; /* BFMT_VideoFmt_e480p */
		gVideoFormat2   = (BFMT_VideoFmt)VIDEO_FORMAT2_2T; /* BFMT_VideoFmt_eNTSC */

	    for(i=0; i<(int)(sizeof(aulReg_2t)/sizeof(uint32_t)); i++)
    	{
        	/* Write dumped register value */
        	BREG_Write32(hRegister, aulReg[i], aulReg_2t[i]);
	    }
	}
}




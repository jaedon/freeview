/***************************************************************************
 *     Copyright (c) 2003-2012, Broadcom Corporation
 *     All Rights Reserved
 *     Confidential Property of Broadcom Corporation
 *
 *  THIS SOFTWARE MAY ONLY BE USED SUBJECT TO AN EXECUTED SOFTWARE LICENSE
 *  AGREEMENT  BETWEEN THE USER AND BROADCOM.  YOU HAVE NO RIGHT TO USE OR
 *  EXPLOIT THIS MATERIAL EXCEPT SUBJECT TO THE TERMS OF SUCH AN AGREEMENT.
 *
 * $brcm_Workfile: bxpt_xcbuf.h $
 * $brcm_Revision: Hydra_Software_Devel/28 $
 * $brcm_Date: 6/18/12 10:53a $
 *
 * Porting interface code for the transport client buffer block. 
 *
 * Revision History:
 *
 * $brcm_Log: /magnum/portinginterface/xpt/include/bxpt_xcbuf.h $
 * 
 * Hydra_Software_Devel/28   6/18/12 10:53a gmullen
 * SW7425-2914: Merged to Hydra
 * 
 * Hydra_Software_Devel/SW7425-2914/1   6/11/12 10:59a gmullen
 * SW7425-2914: Moved and updated files to new home
 * 
 * Hydra_Software_Devel/27   12/17/09 9:34a gmullen
 * SW7630-62: Merged fix to mainline
 * 
 * Hydra_Software_Devel/SW7630-62/1   12/17/09 9:13a gmullen
 * SW7630-62: Buffer enums were incorrect
 * 
 * Hydra_Software_Devel/26   9/9/09 8:07a piyushg
 * SW7630-30: Add 7630 XPT PI support.
 * Added directory element "7630".
 * 
 * Hydra_Software_Devel/25   8/11/09 10:40a piyushg
 * PR55216: Added initial 7340 XPT support.
 * 
 * Hydra_Software_Devel/24   8/5/09 4:52p piyushg
 * PR55545: Add 7125 XPT PI support
 * Added file element "bxpt_rave_ihex.c".
 * Added file element "bxpt_rave_ihex.h".
 * 
 * Hydra_Software_Devel/PR55545/1   7/24/09 10:51a mward
 * PR55548: Adapted for 7125 based on 7342, 7340.
 * 
 * Hydra_Software_Devel/23   7/14/09 10:23a piyushg
 * PR56771: Add XPT PI code for 7342.
 * 
 * Hydra_Software_Devel/22   4/16/09 7:20p piyushg
 * PR53728: Each of the XC buffer clients (Remux, RAVE, and the Mesg
 * filters) would get a separate API to set
 * their input bitrates. That API would take a client handle and the ID of
 * the parser band. The XPT PI would
 * then walk backwards through the pipeline, increasing only the bitrates
 * for the XC and RS buffers that it
 * needs to.
 * 
 * Hydra_Software_Devel/21   4/7/09 5:26p piyushg
 * PR52986: Add support for 7635
 * Added directory element "7635".
 * 
 * Hydra_Software_Devel/20   1/27/09 1:10p gmullen
 * PR51625: Added 7336 support
 * 
 * Hydra_Software_Devel/19   12/17/08 10:55p piyushg
 * PR47755: Added support for 8 playbacks for 7420.
 * 
 * Hydra_Software_Devel/18   11/26/08 4:15p gmullen
 * PR47755: Added support for 7420.
 * 
 * Hydra_Software_Devel/17   5/7/08 10:47a gmullen
 * PR42443: Removed call to change band rates when changing parser mode.
 * 
 * Hydra_Software_Devel/16   5/5/08 3:07p gmullen
 * PR42437: 7325, 3556, and 3548 only have 2 playback channels.
 * 
 * Hydra_Software_Devel/15   3/26/08 11:24a gmullen
 * PR38954: Added 3548 support to XPT PI.
 * 
 * Hydra_Software_Devel/14   2/22/08 11:09a gmullen
 * PR39874: Disabled XC buffer pause before disabling remux.
 * 
 * Hydra_Software_Devel/13   12/3/07 10:54a katrep
 * PR36900: Added support for 7335.
 * 
 * Hydra_Software_Devel/12   10/30/07 3:12p gmullen
 * PR35018: Added support for 7325
 * 
 * Hydra_Software_Devel/11   6/19/07 9:43a katrep
 * PR27642: Updated XcBuf table for 7405.
 * 
 * Hydra_Software_Devel/10   2/21/07 1:04p gmullen
 * PR26217: Added support for IB4 parser.
 * 
 * Hydra_Software_Devel/9   1/8/07 4:33p katrep
 * PR26929: Remux and Playback 3 are not available on 7440.
 * 
 * Hydra_Software_Devel/8   11/2/06 9:21a gmullen
 * PR23189: BXPT_XcBuf_Id_RAVE_IBP5 not included when building for 7401
 * 
 * Hydra_Software_Devel/7   11/2/06 8:40a gmullen
 * PR23189: Added 3563 support.
 * 
 * Hydra_Software_Devel/6   7/20/06 5:30p katrep
 * PR22365: Added better mangement of XC buffer client BW for playback.XC
 * buf for PB is given max BW & BW throtle is done at PB engine.Spare BW
 * enabllag is used to provide more BW for PB if available.
 * 
 * Hydra_Software_Devel/5   7/17/06 5:19p gmullen
 * PR20624: Added code for 7118.
 * 
 * Hydra_Software_Devel/4   7/12/06 5:54p gmullen
 * PR21042: XC and RS buffers can be customized to reduce total memory
 * needs.
 * 
 * Hydra_Software_Devel/3   4/13/06 4:02p gmullen
 * PR20914: Ported over Prashant's fix from 7401A0
 * 
 * Hydra_Software_Devel/2   2/1/06 10:19a gmullen
 * PR18998: Fixed overflow issue in RAVE ITB/CDB, added support for PB
 * channels.
 * 
 * Hydra_Software_Devel/2   11/8/05 10:22a gmullen
 * PR15309: Updated.
 * 
 * Hydra_Software_Devel/1   5/13/05 1:27p gmullen
 * PR15309: First draft of XPT PI.
 * 
 * 
 ***************************************************************************/

#ifndef BXPT_XCBUFF_H__
#define BXPT_XCBUFF_H__

#include "bxpt.h"

#ifdef __cplusplus
extern "C"{
#endif

/*= Module Overview *********************************************************

Overview
Usage / Sample Code

%CODE{"cpp"}%


%ENDCODE%
***************************************************************************/

#if BXPT_HAS_FIXED_XCBUF_CONFIG
	/* Newer chips set the XC config through the defaults struct at BXPT_Open() */
#else

/***************************************************************************
Summary:
****************************************************************************/
typedef enum BXPT_XcBuf_Id
{
#if ( BCHP_CHIP == 7635 )|| ( BCHP_CHIP == 7630 ) 
    BXPT_XcBuf_Id_RMX0_IBP0, 
    BXPT_XcBuf_Id_RMX0_IBP1, 
    BXPT_XcBuf_Id_RMX0_IBP2, 
    BXPT_XcBuf_Id_RMX0_IBP3, 
    BXPT_XcBuf_Id_RMX0_IBP4, 
    BXPT_XcBuf_Id_RMX0_IBP5, 

    BXPT_XcBuf_Id_RMX0_PBP0, 
    BXPT_XcBuf_Id_RMX0_PBP1,
    BXPT_XcBuf_Id_RMX0_PBP2,
    BXPT_XcBuf_Id_RMX0_PBP3,

    BXPT_XcBuf_Id_RMX1_IBP0, 
    BXPT_XcBuf_Id_RMX1_IBP1, 
    BXPT_XcBuf_Id_RMX1_IBP2, 
    BXPT_XcBuf_Id_RMX1_IBP3, 
    BXPT_XcBuf_Id_RMX1_IBP4, 
    BXPT_XcBuf_Id_RMX1_IBP5, 

    BXPT_XcBuf_Id_RMX1_PBP0, 
    BXPT_XcBuf_Id_RMX1_PBP1, 
    BXPT_XcBuf_Id_RMX1_PBP2,
    BXPT_XcBuf_Id_RMX1_PBP3,

    BXPT_XcBuf_Id_RAVE_IBP0, 
    BXPT_XcBuf_Id_RAVE_IBP1, 
    BXPT_XcBuf_Id_RAVE_IBP2, 
    BXPT_XcBuf_Id_RAVE_IBP3, 
    BXPT_XcBuf_Id_RAVE_IBP4, 
    BXPT_XcBuf_Id_RAVE_IBP5, 

    BXPT_XcBuf_Id_RAVE_PBP0, 
    BXPT_XcBuf_Id_RAVE_PBP1, 
    BXPT_XcBuf_Id_RAVE_PBP2,
    BXPT_XcBuf_Id_RAVE_PBP3,

    BXPT_XcBuf_Id_MSG_IBP0,  
    BXPT_XcBuf_Id_MSG_IBP1,  
    BXPT_XcBuf_Id_MSG_IBP2,  
    BXPT_XcBuf_Id_MSG_IBP3,  
    BXPT_XcBuf_Id_MSG_IBP4,  
    BXPT_XcBuf_Id_MSG_IBP5,  

    BXPT_XcBuf_Id_MSG_PBP0,  
    BXPT_XcBuf_Id_MSG_PBP1,  
    BXPT_XcBuf_Id_MSG_PBP2,
    BXPT_XcBuf_Id_MSG_PBP3,

#elif ( BCHP_CHIP == 7325 ) || ( BCHP_CHIP == 3548 ) || ( BCHP_CHIP == 3556 ) || ( BCHP_CHIP == 7420 ) 
    BXPT_XcBuf_Id_RMX0_IBP0, 
    BXPT_XcBuf_Id_RMX0_IBP1, 
    BXPT_XcBuf_Id_RMX0_IBP2, 
    BXPT_XcBuf_Id_RMX0_IBP3, 
    BXPT_XcBuf_Id_RMX0_IBP4, 
    BXPT_XcBuf_Id_RMX0_IBP5, 
    BXPT_XcBuf_Id_RMX0_IBP6, 

    BXPT_XcBuf_Id_RMX0_PBP0, 
    BXPT_XcBuf_Id_RMX0_PBP1,

	#if ( BCHP_CHIP == 7420 )  
    BXPT_XcBuf_Id_RMX0_PBP2,
    BXPT_XcBuf_Id_RMX0_PBP3,
    BXPT_XcBuf_Id_RMX0_PBP4,
    BXPT_XcBuf_Id_RMX0_PBP5,
    BXPT_XcBuf_Id_RMX0_PBP6,
    BXPT_XcBuf_Id_RMX0_PBP7,
	#endif

    BXPT_XcBuf_Id_RMX1_IBP0, 
    BXPT_XcBuf_Id_RMX1_IBP1, 
    BXPT_XcBuf_Id_RMX1_IBP2, 
    BXPT_XcBuf_Id_RMX1_IBP3, 
    BXPT_XcBuf_Id_RMX1_IBP4, 
    BXPT_XcBuf_Id_RMX1_IBP5, 
    BXPT_XcBuf_Id_RMX1_IBP6, 

    BXPT_XcBuf_Id_RMX1_PBP0, 
    BXPT_XcBuf_Id_RMX1_PBP1, 
	#if ( BCHP_CHIP == 7420 )  
    BXPT_XcBuf_Id_RMX1_PBP2,
    BXPT_XcBuf_Id_RMX1_PBP3,
    BXPT_XcBuf_Id_RMX1_PBP4,
    BXPT_XcBuf_Id_RMX1_PBP5,
    BXPT_XcBuf_Id_RMX1_PBP6,
    BXPT_XcBuf_Id_RMX1_PBP7,
	#endif

    BXPT_XcBuf_Id_RAVE_IBP0, 
    BXPT_XcBuf_Id_RAVE_IBP1, 
    BXPT_XcBuf_Id_RAVE_IBP2, 
    BXPT_XcBuf_Id_RAVE_IBP3, 
    BXPT_XcBuf_Id_RAVE_IBP4, 
    BXPT_XcBuf_Id_RAVE_IBP5, 
    BXPT_XcBuf_Id_RAVE_IBP6, 

    BXPT_XcBuf_Id_RAVE_PBP0, 
    BXPT_XcBuf_Id_RAVE_PBP1, 
	#if ( BCHP_CHIP == 7420 )  
    BXPT_XcBuf_Id_RAVE_PBP2,
    BXPT_XcBuf_Id_RAVE_PBP3,
    BXPT_XcBuf_Id_RAVE_PBP4,
    BXPT_XcBuf_Id_RAVE_PBP5,
    BXPT_XcBuf_Id_RAVE_PBP6,
    BXPT_XcBuf_Id_RAVE_PBP7,
	#endif

    BXPT_XcBuf_Id_MSG_IBP0,  
    BXPT_XcBuf_Id_MSG_IBP1,  
    BXPT_XcBuf_Id_MSG_IBP2,  
    BXPT_XcBuf_Id_MSG_IBP3,  
    BXPT_XcBuf_Id_MSG_IBP4,  
    BXPT_XcBuf_Id_MSG_IBP5,  
    BXPT_XcBuf_Id_MSG_IBP6,  

    BXPT_XcBuf_Id_MSG_PBP0,  
    BXPT_XcBuf_Id_MSG_PBP1,  
	#if ( BCHP_CHIP == 7420 ) 
    BXPT_XcBuf_Id_MSG_PBP2,
    BXPT_XcBuf_Id_MSG_PBP3,
    BXPT_XcBuf_Id_MSG_PBP4,
    BXPT_XcBuf_Id_MSG_PBP5,
    BXPT_XcBuf_Id_MSG_PBP6,
    BXPT_XcBuf_Id_MSG_PBP7,
	#endif

#elif ( BCHP_CHIP == 7335 ) || ( BCHP_CHIP == 7336  )  || ( BCHP_CHIP == 7342 )
    BXPT_XcBuf_Id_RMX0_IBP0, 
    BXPT_XcBuf_Id_RMX0_IBP1, 
    BXPT_XcBuf_Id_RMX0_IBP2, 
    BXPT_XcBuf_Id_RMX0_IBP3, 
    BXPT_XcBuf_Id_RMX0_IBP4, 
    BXPT_XcBuf_Id_RMX0_IBP5, 
    BXPT_XcBuf_Id_RMX0_IBP6, 

    BXPT_XcBuf_Id_RMX0_PBP0, 
    BXPT_XcBuf_Id_RMX0_PBP1, 
    BXPT_XcBuf_Id_RMX0_PBP2, 
    BXPT_XcBuf_Id_RMX0_PBP3, 
    BXPT_XcBuf_Id_RMX0_PBP4, 
	#if ( BCHP_CHIP == 7342 )
    BXPT_XcBuf_Id_RMX0_PBP5, 
    BXPT_XcBuf_Id_RMX0_PBP6, 
    BXPT_XcBuf_Id_RMX0_PBP7, 
	#endif

    BXPT_XcBuf_Id_RMX1_IBP0, 
    BXPT_XcBuf_Id_RMX1_IBP1, 
    BXPT_XcBuf_Id_RMX1_IBP2, 
    BXPT_XcBuf_Id_RMX1_IBP3, 
    BXPT_XcBuf_Id_RMX1_IBP4, 
    BXPT_XcBuf_Id_RMX1_IBP5, 
    BXPT_XcBuf_Id_RMX1_IBP6, 

    BXPT_XcBuf_Id_RMX1_PBP0, 
    BXPT_XcBuf_Id_RMX1_PBP1, 
    BXPT_XcBuf_Id_RMX1_PBP2, 
    BXPT_XcBuf_Id_RMX1_PBP3, 
    BXPT_XcBuf_Id_RMX1_PBP4, 
	#if ( BCHP_CHIP == 7342 )
    BXPT_XcBuf_Id_RMX1_PBP5, 
    BXPT_XcBuf_Id_RMX1_PBP6, 
    BXPT_XcBuf_Id_RMX1_PBP7, 
	#endif

    BXPT_XcBuf_Id_RAVE_IBP0, 
    BXPT_XcBuf_Id_RAVE_IBP1, 
    BXPT_XcBuf_Id_RAVE_IBP2, 
    BXPT_XcBuf_Id_RAVE_IBP3, 
    BXPT_XcBuf_Id_RAVE_IBP4, 
    BXPT_XcBuf_Id_RAVE_IBP5, 
    BXPT_XcBuf_Id_RAVE_IBP6, 

    BXPT_XcBuf_Id_RAVE_PBP0, 
    BXPT_XcBuf_Id_RAVE_PBP1, 
    BXPT_XcBuf_Id_RAVE_PBP2, 
    BXPT_XcBuf_Id_RAVE_PBP3, 
    BXPT_XcBuf_Id_RAVE_PBP4, 
	#if ( BCHP_CHIP == 7342 )
    BXPT_XcBuf_Id_RAVE_PBP5, 
    BXPT_XcBuf_Id_RAVE_PBP6, 
    BXPT_XcBuf_Id_RAVE_PBP7, 
	#endif
    BXPT_XcBuf_Id_MSG_IBP0,  
    BXPT_XcBuf_Id_MSG_IBP1,  
    BXPT_XcBuf_Id_MSG_IBP2,  
    BXPT_XcBuf_Id_MSG_IBP3,  
    BXPT_XcBuf_Id_MSG_IBP4,  
    BXPT_XcBuf_Id_MSG_IBP5,  
    BXPT_XcBuf_Id_MSG_IBP6,  

    BXPT_XcBuf_Id_MSG_PBP0,  
    BXPT_XcBuf_Id_MSG_PBP1,  
    BXPT_XcBuf_Id_MSG_PBP2,  
    BXPT_XcBuf_Id_MSG_PBP3,  
    BXPT_XcBuf_Id_MSG_PBP4,  
	#if ( BCHP_CHIP == 7342 )
    BXPT_XcBuf_Id_MSG_PBP5,  
    BXPT_XcBuf_Id_MSG_PBP6,  
    BXPT_XcBuf_Id_MSG_PBP7,  
	#endif

#elif ( BCHP_CHIP == 7125)
    BXPT_XcBuf_Id_RMX0_IBP0, 
    BXPT_XcBuf_Id_RMX0_IBP1, 
    BXPT_XcBuf_Id_RMX0_IBP2, 
    BXPT_XcBuf_Id_RMX0_IBP3, 
    BXPT_XcBuf_Id_RMX0_IBP4, 
    BXPT_XcBuf_Id_RMX0_IBP5, 

    BXPT_XcBuf_Id_RMX0_PBP0, 
    BXPT_XcBuf_Id_RMX0_PBP1, 

    BXPT_XcBuf_Id_RMX1_IBP0, 
    BXPT_XcBuf_Id_RMX1_IBP1, 
    BXPT_XcBuf_Id_RMX1_IBP2, 
    BXPT_XcBuf_Id_RMX1_IBP3, 
    BXPT_XcBuf_Id_RMX1_IBP4, 
    BXPT_XcBuf_Id_RMX1_IBP5, 

    BXPT_XcBuf_Id_RMX1_PBP0, 
    BXPT_XcBuf_Id_RMX1_PBP1, 

    BXPT_XcBuf_Id_RAVE_IBP0, 
    BXPT_XcBuf_Id_RAVE_IBP1, 
    BXPT_XcBuf_Id_RAVE_IBP2, 
    BXPT_XcBuf_Id_RAVE_IBP3, 
    BXPT_XcBuf_Id_RAVE_IBP4, 
    BXPT_XcBuf_Id_RAVE_IBP5, 

    BXPT_XcBuf_Id_RAVE_PBP0, 
    BXPT_XcBuf_Id_RAVE_PBP1, 

	BXPT_XcBuf_Id_MSG_IBP0,  
    BXPT_XcBuf_Id_MSG_IBP1,  
    BXPT_XcBuf_Id_MSG_IBP2,  
    BXPT_XcBuf_Id_MSG_IBP3,  
    BXPT_XcBuf_Id_MSG_IBP4,  
    BXPT_XcBuf_Id_MSG_IBP5,  

    BXPT_XcBuf_Id_MSG_PBP0,  
    BXPT_XcBuf_Id_MSG_PBP1,  

#elif ( BCHP_CHIP == 7340)
    BXPT_XcBuf_Id_RMX0_IBP0, 
    BXPT_XcBuf_Id_RMX0_IBP1, 
    BXPT_XcBuf_Id_RMX0_IBP2, 
    BXPT_XcBuf_Id_RMX0_IBP3, 
    BXPT_XcBuf_Id_RMX0_IBP4, 
    BXPT_XcBuf_Id_RMX0_IBP5, 

    BXPT_XcBuf_Id_RMX0_PBP0, 
    BXPT_XcBuf_Id_RMX0_PBP1, 

    BXPT_XcBuf_Id_RMX1_IBP0, 
    BXPT_XcBuf_Id_RMX1_IBP1, 
    BXPT_XcBuf_Id_RMX1_IBP2, 
    BXPT_XcBuf_Id_RMX1_IBP3, 
    BXPT_XcBuf_Id_RMX1_IBP4, 
    BXPT_XcBuf_Id_RMX1_IBP5, 

    BXPT_XcBuf_Id_RMX1_PBP0, 
    BXPT_XcBuf_Id_RMX1_PBP1, 

    BXPT_XcBuf_Id_RAVE_IBP0, 
    BXPT_XcBuf_Id_RAVE_IBP1, 
    BXPT_XcBuf_Id_RAVE_IBP2, 
    BXPT_XcBuf_Id_RAVE_IBP3, 
    BXPT_XcBuf_Id_RAVE_IBP4, 
    BXPT_XcBuf_Id_RAVE_IBP5, 

    BXPT_XcBuf_Id_RAVE_PBP0, 
    BXPT_XcBuf_Id_RAVE_PBP1, 

	BXPT_XcBuf_Id_MSG_IBP0,  
    BXPT_XcBuf_Id_MSG_IBP1,  
    BXPT_XcBuf_Id_MSG_IBP2,  
    BXPT_XcBuf_Id_MSG_IBP3,  
    BXPT_XcBuf_Id_MSG_IBP4,  
    BXPT_XcBuf_Id_MSG_IBP5,  

    BXPT_XcBuf_Id_MSG_PBP0,  
    BXPT_XcBuf_Id_MSG_PBP1,  
#else
    #if ( BCHP_CHIP != 7440 )
        BXPT_XcBuf_Id_RMX0_A,       /* Remux 0, input A */   
        BXPT_XcBuf_Id_RMX0_B,       /* Remux 0, input B */
    #endif

    #if ( BCHP_CHIP == 3563 )|| ( BCHP_CHIP == 7440 )
        /* These chips have only 1 remux. */
    #else
        BXPT_XcBuf_Id_RMX1_A,       /* Remux 1, input A */
        BXPT_XcBuf_Id_RMX1_B,       /* Remux 1, input B */
    #endif

        BXPT_XcBuf_Id_RAVE_IBP0,    /* RAVE input band parser 0 */
        BXPT_XcBuf_Id_RAVE_IBP1,    /* RAVE input band parser 1 */
        BXPT_XcBuf_Id_RAVE_IBP2,    /* RAVE input band parser 2 */
        BXPT_XcBuf_Id_RAVE_IBP3,    /* RAVE input band parser 3 */
        BXPT_XcBuf_Id_RAVE_IBP4,    /* RAVE input band parser 4 */

    #if ( BCHP_CHIP == 7118 ) || ( BCHP_CHIP == 3563 )
        /* 7118 does not have IB5 or 6 */
    #else
        BXPT_XcBuf_Id_RAVE_IBP5,    /* RAVE input band parser 5 */
        BXPT_XcBuf_Id_RAVE_IBP6,    /* RAVE input band parser 6 */
    #endif

    #if ( BCHP_CHIP == 3563 )
        /* 3563 has only PB0 */
        BXPT_XcBuf_Id_RAVE_PBP0,    /* RAVE playback band parser 0 */
    #else
        BXPT_XcBuf_Id_RAVE_PBP0,    /* RAVE playback band parser 0 */
        BXPT_XcBuf_Id_RAVE_PBP1,    /* RAVE playback band parser 1 */
    #endif

    #if ( BCHP_CHIP == 7118 ) || ( BCHP_CHIP == 3563 )
        /* Not in either of these chips */
    #else
        BXPT_XcBuf_Id_RAVE_PBP2,    /* RAVE playback band parser 2 */
        #if ( BCHP_CHIP != 7440 )
        BXPT_XcBuf_Id_RAVE_PBP3,    /* RAVE playback band parser 3 */
        #endif
    #endif

    #if ( BCHP_CHIP == 7400 && BCHP_VER >= BCHP_VER_B0 ) || ( BCHP_CHIP == 7405 )
        BXPT_XcBuf_Id_RAVE_PBP4,    /* RAVE playback band parser 4 */
    #endif

        BXPT_XcBuf_Id_MSG_IBP0,     /* Mesg filters input band parser 0 */
        BXPT_XcBuf_Id_MSG_IBP1,     /* Mesg filters input band parser 1 */
        BXPT_XcBuf_Id_MSG_IBP2,     /* Mesg filters input band parser 2 */
        BXPT_XcBuf_Id_MSG_IBP3,     /* Mesg filters input band parser 3 */
        BXPT_XcBuf_Id_MSG_IBP4,     /* Mesg filters input band parser 4 */

    #if ( BCHP_CHIP == 7118 ) || ( BCHP_CHIP == 3563 )
        /* 7118 doesn't have either IB5 or 6 */
    #else
        BXPT_XcBuf_Id_MSG_IBP5,     /* Mesg filters input band parser 5 */
        BXPT_XcBuf_Id_MSG_IBP6,     /* Mesg filters input band parser 6 */
    #endif

    #if ( BCHP_CHIP == 3563 )
        /* 3563 has only PB0 */
        BXPT_XcBuf_Id_MSG_PBP0,     /* Mesg filters playback band parser 0 */
    #else
        BXPT_XcBuf_Id_MSG_PBP0,     /* Mesg filters playback band parser 0 */
        BXPT_XcBuf_Id_MSG_PBP1,     /* Mesg filters playback band parser 1 */
    #endif

    #if ( BCHP_CHIP == 7118 ) || ( BCHP_CHIP == 3563 )
        /* Not in either of these chips */
    #else
        BXPT_XcBuf_Id_MSG_PBP2,     /* Mesg filters playback band parser 2 */
        #if ( BCHP_CHIP != 7440 )
        BXPT_XcBuf_Id_MSG_PBP3,     /* Mesg filters playback band parser 3 */
        #endif
    #endif

    #if ( BCHP_CHIP == 7400 && BCHP_VER >= BCHP_VER_B0 ) || ( BCHP_CHIP == 7405 )
        BXPT_XcBuf_Id_MSG_PBP4,     /* Mesg filters playback band parser 4 */
    #endif
#endif
    BXPT_XcBuf_Id_END           /* Marks end of the list. */
}
BXPT_XcBuf_Id;

/***************************************************************************
Summary:
Set the max data rate for a given client.

Description:
Set the max average data rate for the given client. This is the max rate that
can be read on the transport data pipe. A default value of 25 Mbps will be
set when BXPT_Open() is called.

Returns:
    BERR_SUCCESS                - New rate has been set.
    BERR_INVALID_PARAMETER      - Bad input parameter
 
See Also:
BXPT_Open, BXPT_XcBuf_SetBufferSize                     
****************************************************************************/
BERR_Code BXPT_XcBuf_SetBandDataRate(
    BXPT_Handle hXpt,           /* [in] Handle for this transport */
    BXPT_XcBuf_Id Id,           /* [in] Which client buffer we are dealing with */
    unsigned long Rate,         /* [in] Max rate in Mbps */
    unsigned PacketLen          /* [in] size of mpeg packet */
    );

/***************************************************************************
Summary:
Set the buffer size for the given client.

Description:
Using the given size, allocate memory for the client's DRAM buffer. A default
size of 200kB will set when BXPT_Open() is called. Successive calls to this 
function will delete the old buffer before allocating new memory. The old
buffer will also be freed during BXPT_Close(). 

Returns:
    BERR_SUCCESS                - New buffer size has been set.
    BERR_INVALID_PARAMETER      - Bad input parameter
 
See Also:                   
BXPT_Open, BXPT_XcBuf_SetBandDataRate, BXPT_Close                   
****************************************************************************/
BERR_Code BXPT_XcBuf_SetBufferSize(
    BXPT_Handle hXpt,           /* [in] Handle for this transport */
    BXPT_XcBuf_Id Id,           /* [in] Which client buffer we are dealing with */
    unsigned long Size          /* [in] Size in bytes. Must be a multiple of 256. */
    );

/*
** These functions are called internally from BXPT_Open() and BXPT_Close(). 
** Users should NOT uses these functions directly.
*/

BERR_Code BXPT_P_XcBuf_Init(
    BXPT_Handle hXpt,           /* [in] Handle for this transport */
    const BXPT_DramBufferCfg *Cfg
    );

BERR_Code BXPT_P_XcBuf_Shutdown(
    BXPT_Handle hXpt            /* [in] Handle for this transport */
    );

BERR_Code BXPT_P_XcBuf_Pause(
    BXPT_Handle hXpt,           /* [in] Handle for this transport */
    BXPT_XcBuf_Id Id,           /* [in] Which client buffer we are dealing with */
    bool Enable
    );

bool BXPT_P_XcBuf_IsBufferEnabled( 
    BXPT_Handle hXpt, 
    BXPT_XcBuf_Id Id
    );

BERR_Code BXPT_P_XcBuf_RemuxPause(
    BXPT_Handle hXpt,           /* [in] Handle for this transport */
    unsigned RemuxNum,          /* [in] Which client buffer we are dealing with */
    bool Enable
    );

unsigned long BXPT_P_XcBuf_GetBlockout(
    BXPT_Handle hXpt,           /* [in] Handle for this transport */
    BXPT_XcBuf_Id Id           /* [in] size of mpeg packet */
    );

BERR_Code BXPT_P_XcBuf_SetBlockout(
    BXPT_Handle hXpt,           /* [in] Handle for this transport */
    BXPT_XcBuf_Id Id,           /* [in] Which client buffer we are dealing with */
    unsigned long NewB0        
    );

uint32_t BXPT_P_XcBuf_ComputeBlockOut( 
	unsigned long PeakRate,			/* [in] Max data rate (in bps) the band will handle. */
    unsigned PacketLen             /* [in] Packet size ,130 for dss and 188 for mpeg */
    );

#endif	/*  BXPT_HAS_FIXED_XCBUF_CONFIG */

#ifdef __cplusplus
}
#endif

#endif /* BXPT_XCBUFF_H__ */



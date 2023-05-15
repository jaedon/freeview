/***************************************************************************
 *     Copyright (c) 2003-2012, Broadcom Corporation
 *     All Rights Reserved
 *     Confidential Property of Broadcom Corporation
 *
 *  THIS SOFTWARE MAY ONLY BE USED SUBJECT TO AN EXECUTED SOFTWARE LICENSE
 *  AGREEMENT  BETWEEN THE USER AND BROADCOM.  YOU HAVE NO RIGHT TO USE OR
 *  EXPLOIT THIS MATERIAL EXCEPT SUBJECT TO THE TERMS OF SUCH AN AGREEMENT.
 *
 * $brcm_Workfile: bxpt_directv_remux.h $
 * $brcm_Revision: Hydra_Software_Devel/2 $
 * $brcm_Date: 6/18/12 1:45p $
 *
 * Porting interface code for the DirecTV portions of the packet remultiplexor. 
 *
 * Revision History:
 *
 * $brcm_Log: /magnum/portinginterface/xpt/include/bxpt_directv_remux.h $
 * 
 * Hydra_Software_Devel/2   6/18/12 1:45p gmullen
 * SW7425-2914: Merged to Hydra
 * 
 * Hydra_Software_Devel/SW7425-2914/1   6/11/12 10:59a gmullen
 * SW7425-2914: Moved and updated files to new home
 * 
 * Hydra_Software_Devel/1   10/25/10 2:09p gmullen
 * SW7425-15: Moved srcs to base2 folder
 * 
 * Hydra_Software_Devel/1   10/8/10 2:57p gmullen
 * SW7425-15: Added header files
 * 
 * Hydra_Software_Devel/7   8/11/09 10:39a piyushg
 * PR55216: Added initial 7340 XPT support.
 * 
 * Hydra_Software_Devel/6   8/5/09 4:52p piyushg
 * PR55545: Add 7125 XPT PI support
 * Added file element "bxpt_rave_ihex.c".
 * Added file element "bxpt_rave_ihex.h".
 * 
 * Hydra_Software_Devel/PR55545/1   7/24/09 10:47a mward
 * PR55548: Adapted for 7125 based on 7342, 7340.
 * 
 * Hydra_Software_Devel/5   7/14/09 10:21a piyushg
 * PR56771: Add XPT PI code for 7342.
 * 
 * Hydra_Software_Devel/4   1/27/09 1:09p gmullen
 * PR51625: Added 7336 support
 * 
 * Hydra_Software_Devel/3   3/26/08 11:23a gmullen
 * PR38954: Added 3548 support to XPT PI.
 * 
 * Hydra_Software_Devel/2   10/30/07 3:11p gmullen
 * PR35018: Added support for 7325
 * 
 * Hydra_Software_Devel/1   5/13/05 1:27p gmullen
 * PR15309: First draft of XPT PI.
 * 
 * Hydra_Software_Devel/5   10/3/03 5:07p gmullen
 * Replaced stubs with real code.
 * 
 * Hydra_Software_Devel/4   9/8/03 9:20a gmullen
 * Updated for channel-based API.
 * 
 * Hydra_Software_Devel/3   8/15/03 3:04p gmullen
 * Replaced Get/Set calls.
 * 
 * Hydra_Software_Devel/2   7/31/03 10:54a gmullen
 * Changed 'Description' fields in enum and structure comments to
 * 'Summary' so they'll show up in auto-generated docs.
 * 
 * Hydra_Software_Devel/1   7/30/03 12:02p gmullen
 * Initial release for 7038.
 * 
 ***************************************************************************/

/*= Module Overview *********************************************************
This module supports the DirecTV portions of the packet remux block. The 
function calls allow the user to get or set the DirecTV fields in the 
remux. The calls are otherwise identical to the MPEG remux API.
***************************************************************************/

#ifndef BXPT_DIRECTV_REMUX_H__
#define BXPT_DIRECTV_REMUX_H__

#include "bxpt.h"
#include "bxpt_remux.h"

/***************************************************************************
Summary:
Defines the types of packets that the remux supports. 
****************************************************************************/
typedef enum BXPT_RemuxMode
{
	BXPT_RemuxMode_eDirecTv,
	BXPT_RemuxMode_eMpeg
}
BXPT_RemuxMode;

#ifdef __cplusplus
extern "C" {
#endif

/***************************************************************************
Summary:
Enable or disable DirecTV mode operation in the remux channel. 

Description:
This call should be used to put a remux channel into DirecTV mode, or to
restore MPEG mode after entering DirecTV mode.

Returns:
    BERR_SUCCESS                - Call completed successfully.
    BERR_INVALID_PARAMETER      - Bad input parameter
****************************************************************************/
BERR_Code BXPT_DirecTvRemux_SetMode( 
	BXPT_Remux_Handle hRmx,		/* [in] Handle for the remux channel */
	BXPT_RemuxMode Mode			/* [in] Selects the mode. */
	);

#if BXPT_HAS_PSUB_IN_REMUX
/***************************************************************************
Summary:
Enable or disable HD field matching in the Packet Sub block. 

Description:
Require packet substitution logic in the PacketSub block to also match the 
HD field in the transport packet to a given value before allowing a 
substitution of the payload.

Returns:
    BERR_SUCCESS                - Call completed successfully.
    BERR_INVALID_PARAMETER      - Bad input parameter
****************************************************************************/
BERR_Code BXPT_Remux_PsubMatchHdField( 
	BXPT_Remux_Handle hRmx,	/* [in] Handle for the remux channel */
	int WhichTable,		/* [in] The remux packet sub to set. */
	bool MatchHd, 		  /* [in] Enable or disable HD match requirement */
	uint8_t HdCompValue	  /* [in] Value HD field must match. */
	);
#endif
#ifdef __cplusplus
}
#endif

#endif /* #ifndef BXPT_DIRECTV_REMUX_H__ */

/* end of file */



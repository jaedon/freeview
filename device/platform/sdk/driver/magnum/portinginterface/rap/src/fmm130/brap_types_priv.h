/***************************************************************************
*     Copyright (c) 2004-2006, Broadcom Corporation
*     All Rights Reserved
*     Confidential Property of Broadcom Corporation
*
*  THIS SOFTWARE MAY ONLY BE USED SUBJECT TO AN EXECUTED SOFTWARE LICENSE
*  AGREEMENT  BETWEEN THE USER AND BROADCOM.  YOU HAVE NO RIGHT TO USE OR
*  EXPLOIT THIS MATERIAL EXCEPT SUBJECT TO THE TERMS OF SUCH AN AGREEMENT.
*
* $brcm_Workfile: brap_types_priv.h $
* $brcm_Revision: Hydra_Software_Devel/6 $
* $brcm_Date: 4/11/06 6:01p $
*
* Module Description: Private data structures common to all the modules of
*					  Raptor PI.
* Revision History:
* $brcm_Log: /magnum/portinginterface/rap/base/brap_types_priv.h $
* 
* Hydra_Software_Devel/6   4/11/06 6:01p sushmit
* PR18604: Modifying
* #if (( BCHP_CHIP == 7401 )||( BCHP_CHIP == 7400 ))
* With code like
* #ifndef BCHP_7411_VER
* 
* Hydra_Software_Devel/5   3/28/06 12:23p rnayak
* PR20439: Restore Bangalore Raptor files after magnum vob corruption on
* 17th March 2006
* 
* Hydra_Software_Devel/5   3/21/06 6:25p sushmit
* PR18604: Updating review comments.
* 
* Hydra_Software_Devel/4   2/28/06 11:14a sushmit
* PR18604: Fix for 7411 compilation issues.
* 
* Hydra_Software_Devel/3   2/17/06 9:51a sushmit
* PR18604: Merging Encoder to latest.
* 
* Hydra_Software_Devel/RAP_AUD_ENC/3   2/6/06 4:14p sushmit
* PR18604: Merging latest Decoder PI to Encoder PI branch
* 
* Hydra_Software_Devel/RAP_AUD_ENC/2   1/16/06 5:36p sushmit
* PR18604: Updating Audio Encoder PI.
* 
* Hydra_Software_Devel/1   1/10/05 8:14p nitinb
* PR 13082: Initial version
***************************************************************************/
#ifndef _BRAP_TYPES_PRIV_H__ /*{{{*/
#define _BRAP_TYPES_PRIV_H__

#ifdef __cplusplus
extern "C" {
#endif

/***************************************************************************
Summary:
	Supported Audio Channels.

Description:
	This enum defines the different types of supported Audio Channels.
	
See Also:
	
****************************************************************************/
typedef enum BRAP_P_ChannelType
{
	BRAP_P_ChannelType_eDecode = 0,/* Live Decode Channel:
								   The input comes from the transport module 
								   as compressed stream. This channel can do 
								   decode and/or pass-through or SRC on the 
								   input stream */
	BRAP_P_ChannelType_ePcmPlayback,/* Playback Channel:
								   The input is PCM audio coming directly 
								   from secondary storage device. */
	BRAP_P_ChannelType_eCapture,/* Capture Channel:
								   The input is PCM data coming from any one 
								   of the capture input. This data is 
								   used for capturing. */
#ifndef BCHP_7411_VER /* For chips other than 7411 */
/* Encoder support in RM */
	BRAP_P_ChannelType_eEncode, /* Encode channel, data can come either RT or NRT */
#endif
	BRAP_P_ChannelType_eMax     /* This can be used both as invalid or max 
	                               number of channel types */
}BRAP_P_ChannelType;

#ifdef __cplusplus
}
#endif

#endif /*}}} #ifndef _BRAP_TYPES_PRIV_H__ */

/* End of File */

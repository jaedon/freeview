/***************************************************************************
 *     Copyright (c) 2003-2009, Broadcom Corporation
 *     All Rights Reserved
 *     Confidential Property of Broadcom Corporation
 *
 *  THIS SOFTWARE MAY ONLY BE USED SUBJECT TO AN EXECUTED SOFTWARE LICENSE
 *  AGREEMENT  BETWEEN THE USER AND BROADCOM.  YOU HAVE NO RIGHT TO USE OR
 *  EXPLOIT THIS MATERIAL EXCEPT SUBJECT TO THE TERMS OF SUCH AN AGREEMENT.
 *
 * $brcm_Workfile: pep_config.h $
 * $brcm_Revision: 3 $
 * $brcm_Date: 11/11/09 6:37p $
 *
 * Module Description:
 *
 * Revision History:
 *
 * $brcm_Log: /BSEAV/app/brutus/src/pep_config.h $
 * 
 * 3   11/11/09 6:37p mward
 * SW7400-2606: Sharpness should default to minimum.
 * 
 * 2   8/1/06 9:43a erickson
 * PR23130: moved instantiation of g_pep_settings into .cpp file
 * 
 * 1   3/31/05 6:13p bandrews
 * PR14646: Moved default values to separate header file
 ***************************************************************************/
#ifndef PEP_CONFIG_H__
#define PEP_CONFIG_H__

#ifdef __cplusplus
extern "C"
{
#endif

typedef struct PepConfig
{
	int16_t contrast;		/* Color space conversion for the decode window (not the
								display). Ranges between -32768 and 32767.
								Default is 0. */
	int16_t saturation;		/* Color space conversion for the decode window (not the
								display). Ranges between -32768 and 32767.
								Default is 0. */
	int16_t hue;			/* Color space conversion for the decode window (not the
								display). Ranges between -32768 and 32767.
								Default is 0. */
	int16_t brightness;		/* Color space conversion for the decode window (not the
								display). Ranges between -32768 and 32767.
								Default is 0. */
	int16_t sharpness;		/* Sharpness enhancement for the decode window (not the
								display). Ranges between -32768 and 32767.
								Default is -32768. */		
} PepConfig;

/* global for PEP settings. Instantiated in control.cpp. */
extern const PepConfig g_pep_settings;

#ifdef __cplusplus
}
#endif


#endif /* PEP_CONFIG_H__ */


/***************************************************************************
 *     Copyright (c) 2002-2007, Broadcom Corporation
 *     All Rights Reserved
 *     Confidential Property of Broadcom Corporation
 *
 *  THIS SOFTWARE MAY ONLY BE USED SUBJECT TO AN EXECUTED SOFTWARE LICENSE
 *  AGREEMENT  BETWEEN THE USER AND BROADCOM.  YOU HAVE NO RIGHT TO USE OR
 *  EXPLOIT THIS MATERIAL EXCEPT SUBJECT TO THE TERMS OF SUCH AN AGREEMENT.
 *
 * $brcm_Workfile: bsettop_compat.c $
 * $brcm_Revision: 1 $
 * $brcm_Date: 10/15/07 2:35p $
 *
 * Module Description:
 *
 * Revision History:
 *
 * $brcm_Log: /BSEAV/api/src/nexus/bsettop_compat.c $
 * 
 * 1   10/15/07 2:35p erickson
 * PR36068: initial
 * 
 * 1   6/14/07 10:12a jgarrett
 * PR 32123: Adding bsettop_compat.h
 * 
 * PR32123/1   6/13/07 4:20p vle
 * PR32123: header file mapping old API's name to new API's name for
 * backward compatibility
 * 
 ***************************************************************************/
#ifndef BSETTOP_COMPACT_H__
#define BSETTOP_COMPACT_H__
	  
#ifdef __cplusplus
extern "C"
{
#endif

#define boutput_dvi_t boutput_hdmi_t
#define boutput_dvi boutput_hdmi
#define boutput_dvi_settings boutput_hdmi_settings
#define boutput_dvi_capabilities boutput_hdmi_capabilities


#define boutput_dvi_open boutput_hdmi_open
#define boutput_dvi_set boutput_hdmi_set
#define boutput_dvi_get boutput_hdmi_get
#define boutput_dvi_get_capabilities boutput_hdmi_get_capabilities






#ifdef __cplusplus
 }
#endif
 
#endif /* BSETTOP_COMPACT_H__ */



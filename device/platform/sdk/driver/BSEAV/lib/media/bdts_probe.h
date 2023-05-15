/***************************************************************************
 *     Copyright (c) 2007 Broadcom Corporation
 *     All Rights Reserved
 *     Confidential Property of Broadcom Corporation
 *
 *  THIS SOFTWARE MAY ONLY BE USED SUBJECT TO AN EXECUTED SOFTWARE LICENSE
 *  AGREEMENT  BETWEEN THE USER AND BROADCOM.  YOU HAVE NO RIGHT TO USE OR
 *  EXPLOIT THIS MATERIAL EXCEPT SUBJECT TO THE TERMS OF SUCH AN AGREEMENT.
 *
 * $brcm_Workfile: bdts_probe.h $
 * $brcm_Revision: 1 $
 * $brcm_Date: 11/19/08 9:56a $
 *
 * Module Description:
 *
 * BMedia library, DTS elementary stream probe
 * 
 * Revision History:
 *
 * $brcm_Log: /BSEAV/lib/media/bdts_probe.h $
 * 
 * 1   11/19/08 9:56a gmohile
 * PR 48852 : Add DTS probe for ES layer
 * 
 *******************************************************************************/
#ifndef _BDTS_PROBE_H__
#define _BDTS_PROBE_H__

#include "bmedia_probe_es.h"

#ifdef __cplusplus
extern "C"
{
#endif
	
extern const bmedia_probe_es_desc bdts_probe;

#ifdef __cplusplus
}
#endif


#endif /* _BDTS_PROBE_H__ */

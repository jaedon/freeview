/***************************************************************************
 *     Copyright (c) 2002-2003, Broadcom Corporation
 *     All Rights Reserved
 *     Confidential Property of Broadcom Corporation
 *
 *  THIS SOFTWARE MAY ONLY BE USED SUBJECT TO AN EXECUTED SOFTWARE LICENSE
 *  AGREEMENT  BETWEEN THE USER AND BROADCOM.  YOU HAVE NO RIGHT TO USE OR
 *  EXPLOIT THIS MATERIAL EXCEPT SUBJECT TO THE TERMS OF SUCH AN AGREEMENT.
 *
 * $brcm_Workfile: pixmap.h $
 * $brcm_Revision: 1 $
 * $brcm_Date: 2/7/05 7:37p $
 *
 * Module Description:
 *
 * Revision History:
 *
 * $brcm_Log: /BSEAV/api/utils/g1/pixmap.h $
 * 
 * 1   2/7/05 7:37p dlwin
 * Merge down for release 2005_REFSW_MERGETOMAIN:
 * 
 * Irvine_BSEAVSW_Devel/2   10/28/03 11:27a erickson
 * settop api reworks after internal design view
 * 
 * Irvine_BSEAVSW_Devel/1   7/8/03 3:20p erickson
 * added graphics api
 *
 ****************************************************************/
#ifndef	_PIXMAP_H
#define	_PIXMAP_H

#include "bsettop.h"

typedef struct _PIXMAP {
	bgraphics_pixel_format format;
	unsigned char		*buf;
	unsigned short		width;
	unsigned short		height;
	unsigned short		pitch;
} PIXMAP;

#define SizeOfPixMap(p) (p.height*p.pitch)

#endif /* _PIXMAP_H */

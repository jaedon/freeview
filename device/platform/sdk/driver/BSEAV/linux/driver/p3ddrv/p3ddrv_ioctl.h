/***************************************************************************
 *     Copyright (c) 2007, Broadcom Corporation
 *     All Rights Reserved
 *     Confidential Property of Broadcom Corporation
 *
 *  THIS SOFTWARE MAY ONLY BE USED SUBJECT TO AN EXECUTED SOFTWARE LICENSE
 *  AGREEMENT  BETWEEN THE USER AND BROADCOM.  YOU HAVE NO RIGHT TO USE OR
 *  EXPLOIT THIS MATERIAL EXCEPT SUBJECT TO THE TERMS OF SUCH AN AGREEMENT.
 *
 * $brcm_Workfile: p3ddrv_ioctl.h $
 * $brcm_Revision: 2 $
 * $brcm_Date: 12/6/07 9:07p $
 *
 * Module Description:
 *
 * Revision History:
 *
 * $brcm_Log: /BSEAV/linux/driver/p3ddrv/p3ddrv_ioctl.h $
 * 
 * 2   12/6/07 9:07p mphillip
 * PR35854: Add support for GRC calls for mipmapped textures
 * 
 * 1   12/3/07 6:42p mphillip
 * PR35854: 35854
 *
 ***************************************************************************/
#ifndef _P3DDRV_IOCTL_H__
#define _P3DDRV_IOCTL_H__

typedef enum {
	P3D_IOCTL_CHECK_FOR_INTERRUPT,
	P3D_IOCTL_MEMORY_REQUEST,
	P3D_IOCTL_SURFACE_REQUEST,
	P3D_IOCTL_SURFACE_COPY
} P3D_Ioctl_Req_Type ;

typedef struct p3d_int_request {
	int enable;
	int was_fired;
} p3d_int_request_t;
    	
typedef struct p3d_mem_request {
	long size;
	void *ptr;
	void *hP3d;
} p3d_mem_request_t;
    	
typedef struct p3d_sur_request {
	void *bsurface;
	void *surface;
	int width;
	int height;
	int pixel_format;
	void *palette;
} p3d_sur_request_t;
    	
typedef struct p3d_sur_copy {
	void *bgraphics;
	void *src_addr;
	uint32_t src_width;
	uint32_t src_height;
	uint32_t src_pitch;
	uint32_t src_constraint;
	uint32_t src_uid;
	uint32_t src_format;
	void *src_palette;
	uint32_t src_driver_owned;
	void *dst_addr;
	uint32_t dst_width;
	uint32_t dst_height;
	uint32_t dst_pitch;
	uint32_t dst_constraint;
	uint32_t dst_uid;
	uint32_t dst_format;
	void *dst_palette;
	uint32_t dst_driver_owned;
} p3d_sur_copy_t;
    	
#endif /* _P3DDRV_IOCTL_H__ */

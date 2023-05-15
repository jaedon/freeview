/***************************************************************************
 *     Copyright (c) 2011, Broadcom Corporation
 *     All Rights Reserved
 *     Confidential Property of Broadcom Corporation
 *
 *  THIS SOFTWARE MAY ONLY BE USED SUBJECT TO AN EXECUTED SOFTWARE LICENSE
 *  AGREEMENT  BETWEEN THE USER AND BROADCOM.  YOU HAVE NO RIGHT TO USE OR
 *  EXPLOIT THIS MATERIAL EXCEPT SUBJECT TO THE TERMS OF SUCH AN AGREEMENT.
 * 
 *******************************************************************************/

#ifndef _DRM_PRDY_ASF_H__
#define _DRM_PRDY_ASF_H__


#ifdef __cplusplus
extern "C"
{
#endif


/*
** Init the DRM ASF container object. 
*/ 
void bdrm_init_asf_container (bdrm_t drm);
void b_asf_init_handlers (bdrm_t drm);
void b_asf_set_drm_parser(bdrm_t drm, basf_parser_t former_parser, basf_parser_t new_parser);

#ifdef __cplusplus
}
#endif

#endif /* _DRM_PRDY_ASF_H__ */


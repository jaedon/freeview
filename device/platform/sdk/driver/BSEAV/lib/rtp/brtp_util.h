/***************************************************************************
 *     Copyright (c) 2006-2011, Broadcom Corporation
 *     All Rights Reserved
 *     Confidential Property of Broadcom Corporation
 *
 *  THIS SOFTWARE MAY ONLY BE USED SUBJECT TO AN EXECUTED SOFTWARE LICENSE
 *  AGREEMENT  BETWEEN THE USER AND BROADCOM.  YOU HAVE NO RIGHT TO USE OR
 *  EXPLOIT THIS MATERIAL EXCEPT SUBJECT TO THE TERMS OF SUCH AN AGREEMENT.
 *
 * $brcm_Workfile: brtp_util.h $
 * $brcm_Revision: 2 $
 * $brcm_Date: 11/22/11 1:20p $
 *
 * Module Description:
 *
 * RTP parser library
 * 
 * Revision History:
 *
 * $brcm_Log: /BSEAV/lib/rtp/brtp_util.h $
 * 
 * 2   11/22/11 1:20p vsilyaev
 * SW7425-1797: Merged G711 support for rtp parser mpge4
 * 
 * SW7425-1797/1   11/21/11 2:51p xpl
 * SW7425-1797: Add G711 support for rtp parser mpge4
 * 
 * 1   9/18/06 4:42p vsilyaev
 * PR 23826: More work for RTP timing
 * 
 *******************************************************************************/
#ifndef _BRTP_UTIL_H__
#define _BRTP_UTIL_H__

#ifdef __cplusplus
extern "C"
{
#endif

int
bhex_decode(const char *src, size_t src_len, uint8_t *dest, size_t dst_len);

#define B_RTP_SAVE_UINT16_BE(b, d) do { (b)[0] = (uint8_t)((d)>>8); (b)[1]=(uint8_t)(d);}while(0)
#define B_RTP_SAVE_UINT32_BE(b, d) do { (b)[0] = (uint8_t)((d)>>24);(b)[1]=(uint8_t)((d)>>16);(b)[2]=(uint8_t)((d)>>8); (b)[3] = (uint8_t)(d);}while(0)

#define B_RTP_SAVE_UINT16_LE(b, d) do { (b)[0] = (uint8_t)(d); (b)[1]=(uint8_t)((d)>>8);}while(0)
#define B_RTP_SAVE_UINT32_LE(b, d) do { (b)[0] = (uint8_t)(d); (b)[1]=(uint8_t)((d)>>8);(b)[2]=(uint8_t)((d)>>16); (b)[3] = (uint8_t)((d)>>24);}while(0)

#ifdef __cplusplus
}
#endif

#endif /* _BRTP_UTIL_H__ */



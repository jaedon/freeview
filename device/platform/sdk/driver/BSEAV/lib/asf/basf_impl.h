/***************************************************************************
 *     Copyright (c) 2006-2007, Broadcom Corporation
 *     All Rights Reserved
 *     Confidential Property of Broadcom Corporation
 *
 *  THIS SOFTWARE MAY ONLY BE USED SUBJECT TO AN EXECUTED SOFTWARE LICENSE
 *  AGREEMENT  BETWEEN THE USER AND BROADCOM.  YOU HAVE NO RIGHT TO USE OR
 *  EXPLOIT THIS MATERIAL EXCEPT SUBJECT TO THE TERMS OF SUCH AN AGREEMENT.
 *
 * $brcm_Workfile: basf_impl.h $
 * $brcm_Revision: 5 $
 * $brcm_Date: 7/9/07 3:38p $
 *
 * Module Description:
 *
 * ASF parser library
 * 
 * Revision History:
 *
 * $brcm_Log: /BSEAV/lib/asf/basf_impl.h $
 * 
 * 5   7/9/07 3:38p vsilyaev
 * PR 32846: Updated ASF code to work with batom/SPF framework
 * 
 * 4   1/26/07 4:50p vsilyaev
 * PR 27347: Added recovery code to BASF_ASSERT
 * 
 * 3   4/12/06 10:33a vsilyaev
 * PR 20308: Improved parsing of compressed payload
 * 
 * 2   4/7/06 12:29p vsilyaev
 * PR 20683: added basf_player
 * 
 * 1   4/4/06 6:49p vsilyaev
 * PR 20577: Separated ASF parser and ASF stream
 * 
 * 4   4/3/06 6:00p vsilyaev
 * PR 20577: added support for index parsing
 * 
 * 3   3/31/06 7:00p vsilyaev
 * PR 20308: Fixed handling of WMVA and WMV3 streams
 * 
 *******************************************************************************/

#ifndef _BASF_IMPL_H__
#define _BASF_IMPL_H__

#ifdef __cplusplus
extern "C"
{
#endif

#define B_ASF_FIELD_LEN(name,type) sizeof(basf_##type)
#define B_ASF_HEADER_LENGTH (B_ASF_FIELD_LEN(Object ID, guid)+B_ASF_FIELD_LEN(Object Size, qword)) 

#define BASF_ASSERT(expr) do{ if(!(expr)){b_asf_parser_assert_failed(asf, cursor, #expr, __FILE__, __LINE__); goto asf_assert_handler;}}while(0)

void b_asf_parser_assert_failed(basf_parser_t asf, batom_cursor *cursor, const char *expr, const char *file, unsigned line);
const char *b_asf_parser_guid2str(basf_parser_t asf, const basf_guid *guid);

#ifdef __cplusplus
}
#endif


#endif /* _BASF_IMPL_H__  */


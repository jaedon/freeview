/***************************************************************************
 *     Copyright (c) 2003-2008, Broadcom Corporation
 *     All Rights Reserved
 *     Confidential Property of Broadcom Corporation
 *
 *  THIS SOFTWARE MAY ONLY BE USED SUBJECT TO AN EXECUTED SOFTWARE LICENSE
 *  AGREEMENT  BETWEEN THE USER AND BROADCOM.  YOU HAVE NO RIGHT TO USE OR
 *  EXPLOIT THIS MATERIAL EXCEPT SUBJECT TO THE TERMS OF SUCH AN AGREEMENT.
 *
 * $brcm_Workfile: psip_ett.c $
 * $brcm_Revision: 2 $
 * $brcm_Date: 6/9/08 11:19a $
 *
 * [File Description:]
 *
 * Revision History:
 *
 * $brcm_Log: /BSEAV/lib/mpeg2_ts_parse/psip_ett.c $
 * 
 * 2   6/9/08 11:19a tokushig
 * PR42421: fix compile warning about missing newline at end of the file
 * 
 * 1   2/7/05 11:25p dlwin
 * Merge down for release 2005_REFSW_MERGETOMAIN:
 * 
 * Irvine_BSEAVSW_Devel/1   8/29/03 5:03p marcusk
 * Initial Version.
 *
 ***************************************************************************/
#include "bstd.h"
#include "psip_priv.h"
#include "psip_ett.h"

void PSIP_ETT_getHeader( const uint8_t *buf, PSIP_ETT_header *p_header )
{
	p_header->ETM_id.source_id = TS_READ_16( &buf[PSIP_TABLE_DATA_OFFSET] );
	p_header->ETM_id.event_id = (uint16_t)(TS_READ_16( &buf[PSIP_TABLE_DATA_OFFSET+2] ) >> 2);
	p_header->ETM_id.id_type = ((buf[PSIP_TABLE_DATA_OFFSET+3]&0x2)?PSIP_ETT_event_id:PSIP_ETT_channel_id);
	p_header->p_extended_text_message = &buf[PSIP_TABLE_DATA_OFFSET+4];
}


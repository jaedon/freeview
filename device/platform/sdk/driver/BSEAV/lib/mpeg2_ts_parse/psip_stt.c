/***************************************************************************
 *     Copyright (c) 2003, Broadcom Corporation
 *     All Rights Reserved
 *     Confidential Property of Broadcom Corporation
 *
 *  THIS SOFTWARE MAY ONLY BE USED SUBJECT TO AN EXECUTED SOFTWARE LICENSE
 *  AGREEMENT  BETWEEN THE USER AND BROADCOM.  YOU HAVE NO RIGHT TO USE OR
 *  EXPLOIT THIS MATERIAL EXCEPT SUBJECT TO THE TERMS OF SUCH AN AGREEMENT.
 *
 * $brcm_Workfile: psip_stt.c $
 * $brcm_Revision: 1 $
 * $brcm_Date: 2/7/05 11:27p $
 *
 * [File Description:]
 *
 * Revision History:
 *
 * $brcm_Log: /BSEAV/lib/mpeg2_ts_parse/psip_stt.c $
 * 
 * 1   2/7/05 11:27p dlwin
 * Merge down for release 2005_REFSW_MERGETOMAIN:
 * 
 * Irvine_BSEAVSW_Devel/2   2/4/04 9:56a erickson
 * PR9217: converted BDBG_ASSERT calls to CHECK calls. Don't assert on bad
 * data.
 * 
 * Irvine_BSEAVSW_Devel/1   8/29/03 5:04p marcusk
 * Initial Version.
 *
 ***************************************************************************/
#include "bstd.h"
#include "psip_priv.h"
#include "psip_stt.h"
BDBG_MODULE(psip_stt);

void PSIP_STT_getHeader( const uint8_t *buf, PSIP_STT_header *p_header )
{
	CHECK( buf[0] == 0xCD );

	p_header->system_time = TS_READ_32( &buf[PSIP_TABLE_DATA_OFFSET] );
	p_header->GPS_UTC_offset = buf[PSIP_TABLE_DATA_OFFSET+4];
	p_header->daylight_savings.DS_status = (buf[PSIP_TABLE_DATA_OFFSET+5]>>7)&1;
	p_header->daylight_savings.DS_day_of_month = (uint8_t)(buf[PSIP_TABLE_DATA_OFFSET+5]&0x1F);
	p_header->daylight_savings.DS_hour = buf[PSIP_TABLE_DATA_OFFSET+6];
}

TS_PSI_descriptor PSIP_STT_getDescriptor( const uint8_t *buf, int descriptorNum )
{
	CHECK( buf[0] == 0xCD );

	return TS_P_getDescriptor( &buf[PSIP_TABLE_DATA_OFFSET+7], TS_PSI_GET_SECTION_LENGTH(buf)-(8+PSIP_TABLE_DATA_OFFSET), descriptorNum );
}


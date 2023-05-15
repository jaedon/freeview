/***************************************************************************
 *     Copyright (c) 2003, Broadcom Corporation
 *     All Rights Reserved
 *     Confidential Property of Broadcom Corporation
 *
 *  THIS SOFTWARE MAY ONLY BE USED SUBJECT TO AN EXECUTED SOFTWARE LICENSE
 *  AGREEMENT  BETWEEN THE USER AND BROADCOM.  YOU HAVE NO RIGHT TO USE OR
 *  EXPLOIT THIS MATERIAL EXCEPT SUBJECT TO THE TERMS OF SUCH AN AGREEMENT.
 *
 * $brcm_Workfile: psip_mgt.h $
 * $brcm_Revision: 1 $
 * $brcm_Date: 2/7/05 11:26p $
 *
 * [File Description:]
 *
 * Revision History:
 *
 * $brcm_Log: /BSEAV/lib/mpeg2_ts_parse/psip_mgt.h $
 * 
 * 1   2/7/05 11:26p dlwin
 * Merge down for release 2005_REFSW_MERGETOMAIN:
 * 
 * Irvine_BSEAVSW_Devel/1   8/29/03 5:04p marcusk
 * Initial Version.
 *
 ***************************************************************************/
#ifndef PSIP_MGT_H__
#define PSIP_MGT_H__

#ifdef __cplusplus
extern "C" {
#endif

typedef struct
{
	uint16_t	table_type;
	uint16_t	table_type_PID;
	uint8_t		table_type_version_number;
	uint32_t	number_bytes;
} PSIP_MGT_table;

uint16_t PSIP_MGT_getTablesDefined( const uint8_t *buf );
TS_PSI_descriptor PSIP_MGT_getAdditionalDescriptor( const uint8_t *buf, int descriptorNum );

BERR_Code PSIP_MGT_getTable( const uint8_t *buf, int tableNum, PSIP_MGT_table *p_table );
TS_PSI_descriptor PSIP_MGT_getTableDescriptor( const uint8_t *buf, int tableNum, int descriptorNum);

#ifdef __cplusplus
}
#endif
#endif
/* End of File */

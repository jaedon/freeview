
/***************************************************************************
 *     Copyright (c) 2003-2012, Broadcom Corporation
 *     All Rights Reserved
 *     Confidential Property of Broadcom Corporation
 *
 *  THIS SOFTWARE MAY ONLY BE USED SUBJECT TO AN EXECUTED SOFTWARE LICENSE
 *  AGREEMENT  BETWEEN THE USER AND BROADCOM.  YOU HAVE NO RIGHT TO USE OR
 *  EXPLOIT THIS MATERIAL EXCEPT SUBJECT TO THE TERMS OF SUCH AN AGREEMENT.
 *
 * $brcm_Workfile: ts_priv.h $
 * $brcm_Revision: 3 $
 * $brcm_Date: 2/17/12 5:46p $
 *
 * [File Description:]
 *
 * Revision History:
 *
 * $brcm_Log: /BSEAV/lib/mpeg2_ts_parse/ts_priv.h $
 * 
 * 3   2/17/12 5:46p jtna
 * SW7425-2388: Coverity Defect ID:34078 SIGN_EXTENSION
 * 
 * 2   3/3/05 4:44p erickson
 * PR14332: fixed uint64_t warnings (probably errors)
 * 
 * 1   2/7/05 11:30p dlwin
 * Merge down for release 2005_REFSW_MERGETOMAIN:
 * 
 * Irvine_BSEAVSW_Devel/2   2/4/04 9:57a erickson
 * PR9217: converted BDBG_ASSERT calls to CHECK calls. Don't assert on bad
 * data.
 * 
 * Irvine_BSEAVSW_Devel/1   8/29/03 5:05p marcusk
 * Initial Version.
 *
 ***************************************************************************/
#ifndef TS_PRIV_H__
#define TS_PRIV_H__

#ifdef __cplusplus
extern "C" {
#endif

#include "ts_psi.h"

#define TS_READ_64( buf ) ((uint64_t)( \
	(uint64_t)(buf)[0]<<56| \
	(uint64_t)(buf)[1]<<48| \
	(uint64_t)(buf)[2]<<40| \
	(uint64_t)(buf)[3]<<32| \
	(uint32_t)((buf)[4]<<24|(buf)[5]<<16|(buf)[6]<<8|(buf)[7])) )
#define TS_READ_32( buf ) ((uint32_t)((buf)[0]<<24|(buf)[1]<<16|(buf)[2]<<8|(buf)[3]))
#define TS_READ_16( buf ) ((uint16_t)((buf)[0]<<8|(buf)[1]))

#define TS_PSI_TABLE_ID_OFFSET				0
#define TS_PSI_SECTION_LENGTH_OFFSET		1
#define TS_PSI_TABLE_ID_EXT_OFFSET			3
#define TS_PSI_CNI_OFFSET					5
#define TS_PSI_SECTION_NUMBER_OFFSET		6
#define TS_PSI_LAST_SECTION_NUMBER_OFFSET	7

#define TS_PSI_GET_SECTION_LENGTH( buf )	(uint16_t)(TS_READ_16( &(buf)[TS_PSI_SECTION_LENGTH_OFFSET] ) & 0x0FFF)
#define TS_PSI_MAX_BYTE_OFFSET( buf )		(TS_PSI_GET_SECTION_LENGTH(buf) - 1)

TS_PSI_descriptor TS_P_getDescriptor( const uint8_t *p_descBfr, uint32_t descriptorsLength, int descriptorNum );

#define CHECK(COND) \
	do {if (!(COND)) BDBG_ERR(("Bad CHECK: %s at %s, %d", #COND, __FILE__, __LINE__)); } while (0)

#ifdef __cplusplus
}
#endif
#endif
/* End of File */

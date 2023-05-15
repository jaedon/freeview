/***************************************************************************
 *     Copyright (c) 2003-2007, Broadcom Corporation
 *     All Rights Reserved
 *     Confidential Property of Broadcom Corporation
 *
 *  THIS SOFTWARE MAY ONLY BE USED SUBJECT TO AN EXECUTED SOFTWARE LICENSE
 *  AGREEMENT  BETWEEN THE USER AND BROADCOM.  YOU HAVE NO RIGHT TO USE OR
 *  EXPLOIT THIS MATERIAL EXCEPT SUBJECT TO THE TERMS OF SUCH AN AGREEMENT.
 *
 * $brcm_Workfile: psip_rrt.h $
 * $brcm_Revision: 2 $
 * $brcm_Date: 9/19/07 3:25p $
 *
 * [File Description:]
 *
 * Revision History:
 *
 * $brcm_Log: /BSEAV/lib/mpeg2_ts_parse/psip_rrt.h $
 * 
 * 2   9/19/07 3:25p marcusk
 * PR34935: Added new fields [3~
 * 
 * 1   2/7/05 11:27p dlwin
 * Merge down for release 2005_REFSW_MERGETOMAIN:
 * 
 * Irvine_BSEAVSW_Devel/1   8/29/03 5:04p marcusk
 * Initial Version.
 *
 ***************************************************************************/
#ifndef PSIP_RRT_H__
#define PSIP_RRT_H__

#ifdef __cplusplus
extern "C" {
#endif

typedef struct
{
	uint8_t			rating_region;
	uint8_t			version_number;
	const uint8_t	*p_rating_region_name_text;
	uint8_t			dimensions_defined;
} PSIP_RRT_header;

typedef struct
{
	const uint8_t	*p_dimension_name_text;
	bool			graduated_scale;
	uint8_t			values_defined;
} PSIP_RRT_dimension;

typedef struct
{
	const uint8_t		*p_abbrev_rating_value_text;
	const uint8_t		*p_rating_value_text;
} PSIP_RRT_value;

void PSIP_RRT_getHeader( const uint8_t *buf, PSIP_RRT_header *p_header );
TS_PSI_descriptor PSIP_RRT_getDescriptor( const uint8_t *buf, int descriptorNum );
BERR_Code PSIP_RRT_getDimension( const uint8_t *buf, int dimensionNum, PSIP_RRT_dimension *p_dimension );
BERR_Code PSIP_RRT_getValue( const uint8_t *buf, int dimensionNum, int valueNum, PSIP_RRT_value *p_value );

#ifdef __cplusplus
}
#endif
#endif
/* End of File */

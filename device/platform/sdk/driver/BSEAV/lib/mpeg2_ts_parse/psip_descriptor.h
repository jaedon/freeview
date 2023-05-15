/***************************************************************************
 *     Copyright (c) 2003-2008, Broadcom Corporation
 *     All Rights Reserved
 *     Confidential Property of Broadcom Corporation
 *
 *  THIS SOFTWARE MAY ONLY BE USED SUBJECT TO AN EXECUTED SOFTWARE LICENSE
 *  AGREEMENT  BETWEEN THE USER AND BROADCOM.  YOU HAVE NO RIGHT TO USE OR
 *  EXPLOIT THIS MATERIAL EXCEPT SUBJECT TO THE TERMS OF SUCH AN AGREEMENT.
 *
 * $brcm_Workfile: psip_descriptor.h $
 * $brcm_Revision: 2 $
 * $brcm_Date: 5/8/08 1:48p $
 *
 * [File Description:]
 *
 * Revision History:
 *
 * $brcm_Log: /BSEAV/lib/mpeg2_ts_parse/psip_descriptor.h $
 * 
 * 2   5/8/08 1:48p tokushig
 * PR42421: fixed issue with interpretation of cc_type field of caption
 * service descriptor.  added parse routines for genre descriptor
 * 
 * 1   2/7/05 11:25p dlwin
 * Merge down for release 2005_REFSW_MERGETOMAIN:
 * 
 * Irvine_BSEAVSW_Devel/1   8/29/03 5:03p marcusk
 * Initial Version.
 *
 ***************************************************************************/
#ifndef PSIP_DESCRIPTOR_H__
#define PSIP_DESCRIPTOR_H__

#ifdef __cplusplus
extern "C" {
#endif

#include "psip_common.h"

typedef struct
{
	uint8_t		descriptor_tag;
	uint8_t		descriptor_length;
} PSIP_descriptor_header;

/*

  Caption Service Descriptor (0x86)

 */

typedef struct
{
	uint8_t		language[3];
	bool		cc_type;
	union
	{
		bool	line21_field;
		uint8_t	caption_service_number;
	} cc;
	bool		easy_reader;
	bool		wide_aspect_ratio;
} PSIP_CSD_service;

uint8_t PSIP_CSD_getNumServices( TS_PSI_descriptor descriptor );
BERR_Code PSIP_CSD_getService( TS_PSI_descriptor descriptor, int serviceNum, PSIP_CSD_service *p_service );

/*

  Content Advisory Descriptor (0x87)

 */

typedef struct
{
	uint8_t	rating_region;
	uint8_t	rated_dimensions;
	const uint8_t	*p_rating_description_text;
} PSIP_CAD_rating_region;

typedef struct
{
	uint8_t	rating_dimension_j;
	uint8_t	rating_value;
} PSIP_CAD_rating_dimension;

uint8_t PSIP_CAD_getRatingRegionCount( TS_PSI_descriptor descriptor );
BERR_Code PSIP_CAD_getRatingRegion( TS_PSI_descriptor descriptor, int ratingRegionNum, PSIP_CAD_rating_region *p_ratingRegion );
BERR_Code PSIP_CAD_getRatingDimension( TS_PSI_descriptor descriptor, int ratingRegionNum, int ratingDimensionNum, PSIP_CAD_rating_dimension *p_ratingDimension );

/*

  Extended Channel Name Descriptor (0xA0)

 */

PSIP_MSS_string PSIP_ECND_getLongChannelName( TS_PSI_descriptor descriptor );

/*

  Service Location Descriptor (0xA1)

 */

typedef struct
{
	uint16_t	PCR_PID;
	uint8_t		number_elements;
} PSIP_SLD_header;

typedef struct
{
	uint8_t		stream_type;
	uint16_t	elementary_PID;
	uint8_t		ISO_639_language_code[3];
} PSIP_SLD_element;

void PSIP_SLD_getHeader( TS_PSI_descriptor descriptor, PSIP_SLD_header *p_header );
BERR_Code PSIP_SLD_getElement( TS_PSI_descriptor descriptor, int elementNum, PSIP_SLD_element *p_element );

/*

  Time-Shifted Service Descriptor (0xA2)

 */

typedef struct
{
	uint16_t	time_shift;
	uint16_t	major_channel_number;
	uint16_t	minor_channel_number;
} PSIP_TSSD_service;

uint8_t PSIP_TSSD_getNumServices( TS_PSI_descriptor descriptor );
BERR_Code PSIP_TSSD_getService( TS_PSI_descriptor descriptor, int serviceNum, PSIP_TSSD_service *p_service );

/*

  Component Name Descriptor (0xA3)

 */

PSIP_MSS_string PSIP_CND_getComponentName( TS_PSI_descriptor descriptor );

/*

  DCC Departing Request Descriptor (0xA8) and  DCC Arriving Request Descriptor (0xA9)

 */

typedef enum
{
	PSIP_DCC_RD_cancel = 1,
	PSIP_DCC_RD_10_seconds,
	PSIP_DCC_RD_indefinite	
} PSIP_DCC_RD_type;

typedef struct
{
	PSIP_DCC_RD_type	dcc_request_type;
	const uint8_t		*p_dcc_request_text;
} PSIP_DCC_RD_request;

void PSIP_DCC_RD_getRequest( uint8_t *descriptor, PSIP_DCC_RD_request *p_request );

/*

  Redistribution Control Descriptor (0xAA)

 */

void PSIP_RCD_getRcInformation( TS_PSI_descriptor descriptor, uint8_t *p_numBytes, const uint8_t **pp_rc_information );

/*

  Genre Descriptor (0xAB)

 */

typedef uint8_t PSIP_GD_Genre;

uint8_t PSIP_GD_getNumGenres( TS_PSI_descriptor descriptor );
BERR_Code PSIP_GD_getGenre( TS_PSI_descriptor descriptor, int genreNum, PSIP_GD_Genre *p_genre );

#ifdef __cplusplus
}
#endif
#endif
/* End of File */

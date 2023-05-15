/***************************************************************************
 *     Copyright (c) 2003-2012, Broadcom Corporation
 *     All Rights Reserved
 *     Confidential Property of Broadcom Corporation
 *
 *  THIS SOFTWARE MAY ONLY BE USED SUBJECT TO AN EXECUTED SOFTWARE LICENSE
 *  AGREEMENT  BETWEEN THE USER AND BROADCOM.  YOU HAVE NO RIGHT TO USE OR
 *  EXPLOIT THIS MATERIAL EXCEPT SUBJECT TO THE TERMS OF SUCH AN AGREEMENT.
 *
 * $brcm_Workfile: budp_dccparse.h $
 * $brcm_Revision: Hydra_Software_Devel/5 $
 * $brcm_Date: 11/15/12 1:13p $
 *
 * Module Description:
 *
 * Revision History:
 *
 * $brcm_Log: /magnum/commonutils/udp/include/budp_dccparse.h $
 * 
 * Hydra_Software_Devel/5   11/15/12 1:13p darnstein
 * SW7435-448: Add a LAST entry to some enums.
 * 
 * Hydra_Software_Devel/4   11/14/12 11:58a darnstein
 * SW7435-448: Isolate customer-specific parsers.
 * 
 * Hydra_Software_Devel/3   10/28/10 3:58p darnstein
 * SW3548-2364: trivial implementation of _isr functions for parsing.
 * 
 * Hydra_Software_Devel/2   10/21/10 4:38p darnstein
 * SW7401-2571: cut over to the DSS userdata parser that DSS customer
 * engineers perfer.
 * 
 * Hydra_Software_Devel/1   7/27/10 5:06p darnstein
 * SW3548-3022: userdata parsing software.
 * 
 ***************************************************************************/

/*= Module Overview *********************************************************
<verbatim>


Overview
BUDPCCparse module provides parsing of digital closed caption
data.  Digital closed caption data occurs as userdata in MPEG
bitstreams.  There are three standards supported:  DVS 157, ATSC 53,
and DVS 053.

In general, this software accepts a buffer of data that contains the
MPEG userdata.  It parses out the closed caption data, and returns
it as a pair of bytes.  These two bytes can then be sent directly
to a digital (EIA-708-B) closed caption encoder.  Byte pairs that
have an associated bIsAnalog value of "true" may be sent directly
to an analog closed caption (line 21) encoder.

</verbatim>
***************************************************************************/

#ifndef BUDPDCCPARSE_H__
#define BUDPDCCPARSE_H__

#include "bstd.h"
#include "bavc.h"
#include "berr.h"
#include "bavc.h"
#include "budp.h"

#ifdef __cplusplus
extern "C" {
#endif


/*****************************************************************************
 * Structures
 *****************************************************************************/

/***************************************************************************
Summary:
	This enum specifies the format used to encode a pair of closed caption data
	bytes into MPEG userdata.
***************************************************************************/
typedef enum {
	BUDP_DCCparse_Format_Unknown = 0,
	BUDP_DCCparse_Format_DVS157,
	BUDP_DCCparse_Format_ATSC53,
	BUDP_DCCparse_Format_DVS053,
	BUDP_DCCparse_Format_SEI,
	BUDP_DCCparse_Format_SEI2,
	BUDP_DCCparse_Format_AFD53,
	BUDP_DCCparse_Format_LAST       /* Yes, this has to be last in list. */
}
BUDP_DCCparse_Format;

/***************************************************************************
Summary:
	This structure describes a "piece" of closed caption data extracted
	from MPEG userdata.

Description:
	BUDP_DCCparse_ccdata is a data structure that models
	MPEG syntax within userdata.  Mostly, it reflects bitstream
	items that are common among the different standards for
	closed caption data within userdata.

	For most applications, the only fields of interest are
	bIsAnalog, polarity, and format.

	If set, bIsAnalog indicates that the datum is suitable for
	EIA-608-A analog encoding.  If not set, the datum is either
	EIA-708-B only, or could not be parse properly.

	If bIsAnalog is set, then the field polarity indicates
	whether the datum should be sent to a top or bottom field
	of an analog video wave form.

	The format field indicates the method of encoding the closed
	caption datum into MPEG userdata.  This field is important
	because in some bitstreams, multiple copies of the closed
	caption data will be present, with each copy encoded in a
	different format.  Before such data can be sent to any sort
	of closed caption processor (analog or digital) one and
	only one format must be chosen by the application designer.
***************************************************************************/
typedef struct {
	bool bIsAnalog;
	BAVC_Polarity polarity;
	BUDP_DCCparse_Format format;
	uint8_t cc_valid;
	uint8_t cc_priority;
	uint8_t line_offset;
	union {
		uint8_t field_number;	/* For DVS 157 formatted data   */
		uint8_t cc_type;		/* For all other formatted data */
	} seq;
	uint8_t cc_data_1;
	uint8_t cc_data_2;
	uint8_t active_format;
} 
BUDP_DCCparse_ccdata;

/*****************************************************************************
 * Public API
 *****************************************************************************/

/*****************************************************************************
  Summary:
    Parses digital closed caption data from a buffer of MPEG userdata.

  Description:
    This function accepts a "packet" of MPEG userdata and searches it for 
	digital closed caption data.  It can recognize closed caption data that 
	conforms to one of these standards: DVS 157, ATSC 53, or DVS 053.

	In order to avoid having an output array of indefinite length, this
	function will only parse at most one "packet" of MPEG userdata from its
	input.  A "packet" is defined as the userdata that lies between two
	successive userdata start codes (0x000001B2).  Therefore, several calls to
	this function may be necessary to parse out all the userdata.  Parsing will
	be complete when the function returns a *pBytesParsed argument such that
	(offset + *pBytesParsed == pUserdata_info->ui32UserDataBufSize).

	Consider that the closed caption data can only be
	encoded analog if the associated bIsAnalog field is set.
	This consideration, together with the need to call the
	function multiple times (previous paragraph) implies that
	the typical usage of this function is as follows:

    size_t offset = 0;
    while (offset < info->ui32UserDataBufSize)
    {
        eStatus = 
            BUDP_DCCparse_isr (info, offset, &bytesParsed, &cc_count, ccdata);
        if (eStatus == BERR_SUCCESS)
        {
            int icount;
            for (icount = 0 ; icount < cc_count ; ++icount)
            {
                BUDP_DCCparse_ccdata* pccdata = &ccdata[icount];
                if (pccdata->bIsAnalog == true)
                {
				     // Send pccdata->cc_data_1 and pccdata->cc_data_2
					 // to analog closed caption encoder, if any.  These
					 // bytes must be sent on video field with polarity
					 // pcc->polarity.
                }
				// Possibly, send pccdata->cc_data_1 and pccdata->cc_data_2 
				// to digital closed caption encoder.  The customer must
				// know if EIA-708-B processing is appropriate.
            }
        }
		else if (eStatus == BERR_BUDP_NO_DATA)
		{
			// Just keep looping, there may be more data to process.
		}
		else
		{
            // A real error, get out quick.
            return eStatus;
		}
		offset += bytesParsed;
	}

  Returns:
  	BERR_SUCCESS              - The handle was successfully created.
	BERR_INVALID_PARAMETER    - One of the supplied parameters was invalid,
							    possibly NULL.
	BERR_BUDP_NO_DATA      - No closed caption data was found.  As the above
								programming example shows, this is not really
								an error.
	BERR_BUDP_PARSE_ERROR  - Closed caption data was detected, but could not
	                            be successfully parsed.

  See Also:
 *****************************************************************************/
BERR_Code BUDP_DCCparse_isr ( 
	const BAVC_USERDATA_info* 
	       pUserdata_info, /*  [in] The MPEG userdata to be parsed.          */
	size_t         offset, /*  [in] Parsing will start at this offset in the
	                                input buffer 
								    userdata_info->pUserDataBuffer.          */
	size_t*  pBytesParsed, /* [out] The number of bytes parsed from the 
	                                input buffer 
									userdata_info->pUserDataBuffer.          */
	uint8_t*    pcc_count, /* [out] The length of the following output 
	                                array.  Will be between 0 and 31.        */
	BUDP_DCCparse_ccdata* 
	              pCCdata  /* [out] An array holding extracted closed 
				                    caption data.  The user must provide an 
									array of length 32, to accomodate the 
									maximum data that the standards will 
									allow.                                   */
);
#define BUDP_DCCparse BUDP_DCCparse_isr

/*****************************************************************************
  Summary:
    Parses digital closed caption data from a buffer of SEI userdata.

  Description:
    This function accepts a "packet" of SEI userdata and searches
    it for digital closed caption data. SEI is the closed caption
    format for older bitstreams that are AVC MPEG-4.  The userdata
    occurs in the NAL.

	In order to avoid having an output array of indefinite
	length, this function will only parse at most one "packet"
	of userdata from its input.  A "packet" is defined as the
	userdata that lies between two successive userdata start
	codes (0x000001B2).  Therefore, several calls to this
	function may be necessary to parse out all the userdata.
	Parsing will be complete when the function returns a
	*pBytesParsed argument such that (offset + *pBytesParsed ==
	pUserdata_info->ui32UserDataBufSize).

	Consider that the closed caption data can only be
	encoded analog if the associated bIsAnalog field is set.
	This consideration, together with the need to call the
	function multiple times (previous paragraph) implies that
	the typical usage of this function is as follows:

    size_t offset = 0;
    while (offset < info->ui32UserDataBufSize)
    {
        eStatus = 
            BUDP_DCCparse_SEI_isr (
				info, offset, &bytesParsed, &cc_count, ccdata);
        if (eStatus == BERR_SUCCESS)
        {
            int icount;
            for (icount = 0 ; icount < cc_count ; ++icount)
            {
                BUDP_DCCparse_ccdata* pccdata = &ccdata[icount];
                if (pccdata->bIsAnalog == true)
                {
				     // Send pccdata->cc_data_1 and pccdata->cc_data_2
					 // to analog closed caption encoder, if any.  These
					 // bytes must be sent on video field with polarity
					 // pcc->polarity.
                }
				// Possibly, send pccdata->cc_data_1 and pccdata->cc_data_2 
				// to digital closed caption encoder.  The customer must
				// know if EIA-708-B processing is appropriate.
            }
        }
		else if (eStatus == BERR_BUDP_NO_DATA)
		{
			// Just keep looping, there may be more data to process.
		}
		else
		{
            // A real error, get out quick.
            return eStatus;
		}
		offset += bytesParsed;
	}

  Returns:
  	BERR_SUCCESS              - The handle was successfully created.
	BERR_INVALID_PARAMETER    - One of the supplied parameters was invalid,
							    possibly NULL.
	BERR_BUDP_NO_DATA      - No closed caption data was found.  As the above
								programming example shows, this is not really
								an error.
	BERR_BUDP_PARSE_ERROR  - Closed caption data was detected, but could not
	                            be successfully parsed.

  See Also:
 *****************************************************************************/
BERR_Code BUDP_DCCparse_SEI_isr ( 
	const BAVC_USERDATA_info* 
	       pUserdata_info, /*  [in] The MPEG userdata to be parsed.          */
	size_t         offset, /*  [in] Parsing will start at this offset in the
	                                input buffer 
								    userdata_info->pUserDataBuffer.          */
	size_t*  pBytesParsed, /* [out] The number of bytes parsed from the 
	                                input buffer 
									userdata_info->pUserDataBuffer.          */
	uint8_t*    pcc_count, /* [out] The length of the following output 
	                                array.  Will be between 0 and 31.        */
	BUDP_DCCparse_ccdata* 
	              pCCdata  /* [out] An array holding extracted closed 
				                    caption data.  The user must provide an 
									array of length 32, to accomodate the 
									maximum data that the standards will 
									allow.                                   */
);
#define BUDP_DCCparse_SEI BUDP_DCCparse_SEI_isr

#ifdef __cplusplus
}
#endif

#endif /* BUDPDCCPARSE_H__ */

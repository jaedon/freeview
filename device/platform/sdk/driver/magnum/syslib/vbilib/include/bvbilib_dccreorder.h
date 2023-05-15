/***************************************************************************
 *     Copyright (c) 2007, Broadcom Corporation
 *     All Rights Reserved
 *     Confidential Property of Broadcom Corporation
 *
 *  THIS SOFTWARE MAY ONLY BE USED SUBJECT TO AN EXECUTED SOFTWARE LICENSE
 *  AGREEMENT  BETWEEN THE USER AND BROADCOM.  YOU HAVE NO RIGHT TO USE OR
 *  EXPLOIT THIS MATERIAL EXCEPT SUBJECT TO THE TERMS OF SUCH AN AGREEMENT.
 *
 * $brcm_Workfile: bvbilib_dccreorder.h $
 * $brcm_Revision: Hydra_Software_Devel/5 $
 * $brcm_Date: 5/29/07 1:12p $
 *
 * Module Description:
 *
 * Revision History:
 *
 * $brcm_Log: /magnum/syslib/vbilib/bvbilib_dccreorder.h $
 * 
 * Hydra_Software_Devel/5   5/29/07 1:12p darnstein
 * PR31097: fix an ordinary typing error.
 * 
 * Hydra_Software_Devel/4   5/25/07 6:09p darnstein
 * PR31097: ready for testing now.
 * 
 * Hydra_Software_Devel/3   5/24/07 6:54p darnstein
 * PR31097: Expand tabs for readability. I won't keep doing this forever.
 * 
 * Hydra_Software_Devel/2   5/24/07 6:50p darnstein
 * PR31097: Second draft of the proposed API.
 *
 ***************************************************************************/

/*= Module Overview *********************************************************
<verbatim>

Overview:

BVBIlibDCCReorder module provides solves a temporary problem for
some headend equipment. The problem at hand is an MPEG or AVC
bitstream containing closed caption data that has been reordered. In
particular, the bitstream will contain repeated top field data and
repeated bottom field data.

This situation leads to missed opportunities for the VEC closed
caption encoder to insert its waveform into video fields. This causes
the queue of waiting closed caption data to grow without bound.

The 'Reorder module simply rearranges pieces of closed caption
data. It attempts to restore the alternating top/bottom/top/bottom
ordering of the data.

Usage:

The module has a state that is stored in a handle object, and it
must be initialized:

    BVBIlib_DCCReorder_Open (&handle, histSize, threshold);

Thereafter, there are two API calls BVBIlib_DCCReorder_Put()
and BVBIlib_DCCReorder_Get() that should be called after calling
one of the BVBIlib_DCCparseXXX functions. For best results, these
two BVBIlib_DCCReorder calls should NOT be interleaved. This will
give the reordering algorithm maximum opportunity to find a good
reordering. For example:

    BVBIlib_DCCparse (&info, offset, &bytesParsed, &cc_count, ccdata);
    for (icount = 0 ; icount < cc_count ; ++icount)
    {
        BAVC_Polarity polarity;
        BVBIlib_DCCparse_ccdata* pccdata = &ccdata[icount];
        if (!pccdata->bIsAnalog) continue;
        if (pccdata->format != BVBIlib_DCCparse_Format_ATSC53) continue;
        if (pccdata->polarity == BAVC_Polarity_eFrame) continue;
        BVBIlib_DCCReorder_Put (
            handle, pccdata->cc_data_1, pccdata->cc_data_2, pccdata->polarity);
    }

    uint8_t datumL;
    uint8_t datumH;
    BAVC_Polarity polarity;
    while (BVBIlib_DCCReorder_Get (handle, &datumL, &datumH, &polarity) == 
        BERR_SUCCESS)
    {
        BVBI_Field_Handle hField;
        uint8_t parityMask = (1 << polarity);
        BVBI_Field_SetCCData_isr (hField, datumL, datumH);
        BVBI_Field_SetPolarity_isr (hField, parityMask);
        BVBIlib_Encode_Enqueue_isr (hVbilibEncode, hField);
    }

Note:

This module misuses the defined symbol BERR_OUT_OF_SYSTEM_MEMORY in
several places. This module is expected to have a limited life time,
so it is better to NOT define new global symbols instead.

</verbatim>
***************************************************************************/

#ifndef BVBILIBDCCREORDER_H__
#define BVBILIBDCCREORDER_H__

#include "bstd.h"
#include "bavc.h"

/* Structures */

/*****************************************************************************
  Summary:
    The BVBIlib_DCCReorder_Handle, once opened, stores the state
    of the BVBIlib_DCCReorder module. It is a required argument to
    all other BVBIlib_DCCReorder functions.

  See Also:
    BVBIlib_DCCReorder_Open, BVBIlib_DCCReorder_Close
*****************************************************************************/
typedef struct BVBIlib_P_DCCReorder_Handle* 
    BVBIlib_DCCReorder_Handle; /* Opaque */


/*****************************************************************************
 * Public API
 *****************************************************************************/

/*****************************************************************************
  Summary:
    Initializes and configures state for BVBIlib_DCCReorder module.

  Description:
    This function initializes the BVBIlib_DCCReorder queueing/reordering 
    module. The user indicates the amount of memory to be used, and the 
    function returns a handle, which is used for all subsequent calls into
    the BVBIlib_DCCReorder module.

  Returns:
    BERR_SUCCESS              - The handle was successfully created.
    BERR_OUT_OF_SYSTEM_MEMORY - Memory allocation failed.

  See Also:
    BVBILIB_DCCReorder_Close
*****************************************************************************/
BERR_Code BVBIlib_DCCReorder_Open (
    BVBIlib_DCCReorder_Handle* pHandle, /* [out] Initialized module handle. */
    unsigned int histSize,              /*  [in] Maximum number of top 
                                                 field closed caption data 
						                         and bottom field closed 
						                         caption data that the 
						                         module will remember. THIS 
						                         MUST BE A POWER OF 2!      */
    unsigned int threshold              /*  [in] Affects how aggressive 
                                                 BVBIlib_DCCReorder_Get 
						                         will be about returning CC
						                         data with "bad" polarity. 
						                         See the description of 
						                         that function for details. */
);

/*****************************************************************************
  Summary:
    Destroys module handle and frees storage space.

  Description:
    This function destroys the BVBIlib_DCCReorder queueing/reordering 
    module and frees resources (memory) used by the module. Any stored
    closed caption data is lost.

  See Also:
    BVBILIB_DCCReorder_Open
*****************************************************************************/
void BVBIlib_DCCReorder_Close (
    BVBIlib_DCCReorder_Handle handle /* [in] Initialized module handle.     */
);

/*****************************************************************************
  Summary:
    Stores one piece of closed caption data in module.

  Description:
    This function accepts one piece of closed caption data into the 
    BVBIlib_DCCReorder module, as long as there is room for it.

  Returns:
    BERR_SUCCESS              - Success.
    BERR_INVALID_PARAMETER    - Invalid handle detected.
    BERR_OUT_OF_SYSTEM_MEMORY - No room in queues for any more data.

  See Also:
    BVBILIB_DCCReorder_Get
*****************************************************************************/
BERR_Code BVBIlib_DCCReorder_Put (
    BVBIlib_DCCReorder_Handle handle, /* [in] Initialized module handle.    */
    uint8_t pDatumL,                  /* [in] Closed caption data, low 
                                              byte.                         */
    uint8_t pDatumH,                  /* [in] Closed caption data, high 
                                              byte.                         */
    BAVC_Polarity pPolarity           /* [in] Field polarity required for
                                              the above closed caption 
                                              data.                         */
);

/*****************************************************************************
  Summary:
    Retrieves one piece of closed caption data from module.

  Description:
    This function retrieves one piece of reordered, closed
    caption data from the BVBIlib_DCCReorder module.

    This function "tries" to return alternating top and bottom field closed 
    caption data in repeated calls to it. The details of this algorithm depend 
    on the threshold argument passed in to BVBIlib_DCCReorder_Open and are as
    follows:

    If the module contains a piece "xxx" of closed caption data having
    opposite polarity as the last piece of closed caption data returned,
    then datum "xxx" will be returned.

    Otherwise, if the module contains (histSize - threshold) pieces of
    top field or bottom field data, then one of those pieces of data will
    be returned. Here, histSize and threshold are the arguments that were
    passed in to function BVBIlib_DCCReorder_Open.

  Returns:
    BERR_SUCCESS              - Success.
    BERR_INVALID_PARAMETER    - Invalid handle detected.
    BERR_OUT_OF_SYSTEM_MEMORY - No stored data in module to return, according 
                                to the rules above.

  See Also:
    BVBILIB_DCCReorder_Put
*****************************************************************************/
BERR_Code BVBIlib_DCCReorder_Get (
    BVBIlib_DCCReorder_Handle handle,  /*  [in] Initialized module handle.  */
    uint8_t* datumL,                   /* [out] Returned closed caption 
                                                data, low byte.             */
    uint8_t* datumH,                   /* [out] Returned closed caption 
                                                data, high byte.            */
    BAVC_Polarity* polarity            /* [out] Field polarity required for
                                                the above closed caption 
                                                data.                       */
);

/*****************************************************************************
  Summary:
    Indicates how full the queues in the BVBIlib_DCCReorder module are.

  Description:
    This function counts the number of pieces of closed caption top
    field data, and the number of pieces of closed caption bottom
    field data. It returns the maximum of these two numbers.

  Returns:
    BERR_SUCCESS              - Success.
    BERR_INVALID_PARAMETER    - Invalid handle detected.

*****************************************************************************/
BERR_Code BVBIlib_DCCReorder_Count (
    BVBIlib_DCCReorder_Handle* handle, /*  [in] Initialized module handle.  */
    unsigned int* count                /* [out] Maximum queue length, as
                                                defined above.              */
);


#endif /* BVBILIBDCCREORDER_H__ */

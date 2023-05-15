/***************************************************************************
 *     Copyright (c) 2004-2006, Broadcom Corporation
 *     All Rights Reserved
 *     Confidential Property of Broadcom Corporation
 *
 *  THIS SOFTWARE MAY ONLY BE USED SUBJECT TO AN EXECUTED SOFTWARE LICENSE
 *  AGREEMENT  BETWEEN THE USER AND BROADCOM.  YOU HAVE NO RIGHT TO USE OR
 *  EXPLOIT THIS MATERIAL EXCEPT SUBJECT TO THE TERMS OF SUCH AN AGREEMENT.
 *
 * $brcm_Workfile: bgrc_errors.h $
 * $brcm_Revision: Hydra_Software_Devel/5 $
 * $brcm_Date: 2/21/06 12:07p $
 *
 * Module Description:
 *
 * Revision History:
 *
 * $brcm_Log: /magnum/portinginterface/grc/7400/bgrc_errors.h $
 * 
 * Hydra_Software_Devel/5   2/21/06 12:07p nissen
 * PR 19084: Added support for second m2mc device.
 * 
 * Hydra_Software_Devel/4   12/1/05 6:07p nissen
 * PR 18007: Added code to return error when down scaling more than 15x
 * for M2MC Cx and above.
 * 
 * Hydra_Software_Devel/3   9/24/04 4:36p nissen
 * PR 12598: Added error code for YCbCr422 surfaces with odd rectangle
 * edges.
 * 
 * Hydra_Software_Devel/2   3/31/04 12:03p nissen
 * PR 10165: Added error codes for invalid surface dimensions.
 * 
 * Hydra_Software_Devel/1   3/12/04 2:21p nissen
 * PR 10024: Error header file.
 * 
 ***************************************************************************/
#ifndef BGRC_ERRORS_H__
#define BGRC_ERRORS_H__

#include "berr_ids.h"

#ifdef __cplusplus
extern "C" {
#endif

#define BGRC_ERR_NO_OUTPUT_SURFACE               BERR_MAKE_CODE(BERR_GRC_ID, 0x0000)
#define BGRC_ERR_SOURCE_RECT_OUT_OF_BOUNDS       BERR_MAKE_CODE(BERR_GRC_ID, 0x0001)
#define BGRC_ERR_DESTINATION_RECT_OUT_OF_BOUNDS  BERR_MAKE_CODE(BERR_GRC_ID, 0x0002)
#define BGRC_ERR_OUTPUT_RECT_OUT_OF_BOUNDS       BERR_MAKE_CODE(BERR_GRC_ID, 0x0003)
#define BGRC_ERR_SOURCE_DIMENSIONS_INVALID       BERR_MAKE_CODE(BERR_GRC_ID, 0x0004)
#define BGRC_ERR_DESTINATION_DIMENSIONS_INVALID  BERR_MAKE_CODE(BERR_GRC_ID, 0x0005)
#define BGRC_ERR_OUTPUT_DIMENSIONS_INVALID       BERR_MAKE_CODE(BERR_GRC_ID, 0x0006)
#define BGRC_ERR_YCBCR422_SURFACE_HAS_ODD_EDGE   BERR_MAKE_CODE(BERR_GRC_ID, 0x0007)
#define BGRC_ERR_MAX_SCALE_DOWN_LIMIT_EXCEEDED   BERR_MAKE_CODE(BERR_GRC_ID, 0x0008)
#define BGRC_ERR_M2MC_DEVICE_IS_HUNG             BERR_MAKE_CODE(BERR_GRC_ID, 0x0010)
#define BGRC_ERR_M2MC_DEVICE_NUM_INVALID         BERR_MAKE_CODE(BERR_GRC_ID, 0x0011)

#ifdef __cplusplus
}
#endif

#endif /* #ifndef BGRC_ERRORS_H__ */

/* End of file. */

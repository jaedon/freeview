/***************************************************************************
 *     Copyright (c) 1999-2013, Broadcom Corporation
 *     All Rights Reserved
 *     Confidential Property of Broadcom Corporation
 *
 *
 * THIS SOFTWARE MAY ONLY BE USED SUBJECT TO AN EXECUTED SOFTWARE LICENSE
 * AGREEMENT  BETWEEN THE USER AND BROADCOM.  YOU HAVE NO RIGHT TO USE OR
 * EXPLOIT THIS MATERIAL EXCEPT SUBJECT TO THE TERMS OF SUCH AN AGREEMENT.
 *
 ***************************************************************************/

#include "bchp.h"
const uint32_t BDSP_IMG_vp8_encode_array1[] = {
	0xa9fd0fff,
	0x07bffe47
};
const uint32_t BDSP_IMG_vp8_encode_header [2] = {sizeof(BDSP_IMG_vp8_encode_array1), 1};
const void * const BDSP_IMG_vp8_encode [2] = {BDSP_IMG_vp8_encode_header, BDSP_IMG_vp8_encode_array1};
/* End of File */
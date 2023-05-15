/***************************************************************************
 *     Copyright (c) 2002-2008, Broadcom Corporation
 *     All Rights Reserved
 *     Confidential Property of Broadcom Corporation
 *
 *  THIS SOFTWARE MAY ONLY BE USED SUBJECT TO AN EXECUTED SOFTWARE LICENSE
 *  AGREEMENT  BETWEEN THE USER AND BROADCOM.  YOU HAVE NO RIGHT TO USE OR
 *  EXPLOIT THIS MATERIAL EXCEPT SUBJECT TO THE TERMS OF SUCH AN AGREEMENT.
 *
 * $brcm_Workfile: bthumbnail.h $
 * $brcm_Revision: 2 $
 * $brcm_Date: 3/18/08 3:55p $
 *
 * Module Description:
 *
 * Revision History:
 *
 * $brcm_Log: /BSEAV/lib/thumbnail/bthumbnail.h $
 * 
 * 2   3/18/08 3:55p erickson
 * PR40307: update
 * 
 * 1   3/17/08 9:22a erickson
 * PR40307: add thumbnail library
 * 
 ******************************************************************/
#ifndef BTHUMBNAIL_H__
#define BTHUMBNAIL_H__

#include "bstd.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct bthumbnail *bthumbnail_t;

#include "bthumbnail_manager.h"
#include "bthumbnail_stream.h"

typedef struct bthumbnail_data {
    bthumbnail_stream_t stream;
    void *surface; /* surface handle (defined by application). */
    unsigned time; /* time of the still, in seconds */
} bthumbnail_data;

int bthumbnail_get_data(
    bthumbnail_t thumbnail,
    bthumbnail_data *data   /* [out] */
    );

#ifdef __cplusplus
}
#endif

#endif /* BTHUMBNAIL_H__ */


/***************************************************************************
 *     Copyright (c) 2002-2009, Broadcom Corporation
 *     All Rights Reserved
 *     Confidential Property of Broadcom Corporation
 *
 *  THIS SOFTWARE MAY ONLY BE USED SUBJECT TO AN EXECUTED SOFTWARE LICENSE
 *  AGREEMENT  BETWEEN THE USER AND BROADCOM.  YOU HAVE NO RIGHT TO USE OR
 *  EXPLOIT THIS MATERIAL EXCEPT SUBJECT TO THE TERMS OF SUCH AN AGREEMENT.
 *
 * $brcm_Workfile: bthumbnail_priv.h $
 * $brcm_Revision: 7 $
 * $brcm_Date: 8/14/09 3:13p $
 *
 * Module Description:
 *
 * Revision History:
 *
 * $brcm_Log: /BSEAV/lib/thumbnail/bthumbnail_priv.h $
 * 
 * 7   8/14/09 3:13p erickson
 * PR55994: refactor so that bthumbnail_extractor can be used standlone
 * with nexus
 *
 * 6   3/24/08 10:01a erickson
 * PR40307: improved cleanup
 *
 * 5   3/22/08 2:08a erickson
 * PR40307: update
 *
 * 4   3/20/08 3:17a erickson
 * PR40307: clean up surfaces if we exceed the maximum_memory
 *
 * 3   3/20/08 1:47a erickson
 * PR40307: update
 *
 * 2   3/18/08 3:55p erickson
 * PR40307: update
 *
 * 1   3/17/08 9:22a erickson
 * PR40307: add thumbnail library
 *
 ******************************************************************/
#ifndef BTHUMBNAIL_PRIV_H__
#define BTHUMBNAIL_PRIV_H__

#include "bstd.h"
#include "bkni.h"
#include "blst_list.h"

BDBG_OBJECT_ID_DECLARE(bthumbnail_manager);
BDBG_OBJECT_ID_DECLARE(bthumbnail_stream);
BDBG_OBJECT_ID_DECLARE(bthumbnail);

struct bthumbnail_manager {
    BDBG_OBJECT(bthumbnail_manager)
    BLST_D_HEAD(bthumbnail_stream_list, bthumbnail_stream) list;
    BLST_D_HEAD(bthumbnail_dangling_list, bthumbnail) dangling;
    bthumbnail_manager_settings settings;
    void *compressed_buffer;
    unsigned total_size;
    unsigned total_thumbnails;
    bool wait_for_change; /* avoid an infinite busy loop of creating and destroying the same thumbnail over and over again */
    unsigned amount_short; /* how many still were we unable to request. when reducing memory, clean enough for those. */
};

struct bthumbnail_stream {
    BDBG_OBJECT(bthumbnail_stream)
    bthumbnail_manager_t manager;
    BLST_D_ENTRY(bthumbnail_stream) link; /* list in bthumbnail_manager */
    BLST_D_HEAD(bthumbnail_list, bthumbnail) list;
    bthumbnail_stream_create_settings create_settings;
    bthumbnail_stream_settings settings;
    uint8_t endcode_packet[188]; /* TODO: support non-188 DSS and timestamp streams? */
    unsigned actual_full_view_spacing;
};

struct bthumbnail {
    BDBG_OBJECT(bthumbnail)
    BLST_D_ENTRY(bthumbnail) link; /* list in bthumbnail_stream */
    bthumbnail_data data;
    bthumbnail_manager_t manager; /* set if this was deleted while is_decoding is true and now the manager owns it in the dangling list */
    unsigned size;
};

bthumbnail_t bthumbnail_stream_p_create_next_thumbnail(bthumbnail_stream_t stream);
bthumbnail_t bthumbnail_p_create_thumbnail( bthumbnail_stream_t stream, unsigned time );
void bthumbnail_p_destroy( bthumbnail_t thumbnail );
int bthumbnail_p_request_decode(bthumbnail_t thumbnail, uint8_t *buffer, unsigned buffer_size, unsigned *amount_read);
void bthumbnail_manager_p_delete_surface(bthumbnail_manager_t manager, bthumbnail_t thumbnail);
bool bthumbnail_stream_p_delete_thumbnails(bthumbnail_stream_t stream, bool aggressive);

#endif /* BTHUMBNAIL_PRIV_H__ */


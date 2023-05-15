/***************************************************************************
 *     Copyright (c) 2007 Broadcom Corporation
 *     All Rights Reserved
 *     Confidential Property of Broadcom Corporation
 *
 *  THIS SOFTWARE MAY ONLY BE USED SUBJECT TO AN EXECUTED SOFTWARE LICENSE
 *  AGREEMENT  BETWEEN THE USER AND BROADCOM.  YOU HAVE NO RIGHT TO USE OR
 *  EXPLOIT THIS MATERIAL EXCEPT SUBJECT TO THE TERMS OF SUCH AN AGREEMENT.
 *
 * $brcm_Workfile: bpvrlib_feed.c $
 * $brcm_Revision: Hydra_Software_Devel/34 $
 * $brcm_Date: 8/15/12 1:17p $
 *
 * Module Description:
 *
 * PVR library, playback feeder module
 * Revision History:
 *
 * $brcm_Log: /magnum/syslib/pvrlib/bpvrlib_feed.c $
 * 
 * Hydra_Software_Devel/34   8/15/12 1:17p vsilyaev
 * SW7425-3637: Use cached addreeses to update descriptors
 * 
 * Hydra_Software_Devel/33   8/13/12 1:27p vsilyaev
 * SW7425-3723: Added test for result of BINT_EnableCallback
 * 
 * Hydra_Software_Devel/32   9/2/11 3:01p vsilyaev
 * SW7425-433: Better account for reserved descriptor in returned status
 * 
 * Hydra_Software_Devel/31   4/26/11 4:11p vsilyaev
 * SW7425-433: Don't allow reuse of the 'reserved' playback HW descriptor
 * 
 * Hydra_Software_Devel/30   4/25/11 1:34p vsilyaev
 * SW7425-433: Demoted debug message
 * 
 * Hydra_Software_Devel/29   4/5/11 4:20p vsilyaev
 * SW7405-5216: Use XPT defined defint to tag specific workaround
 * 
 * Hydra_Software_Devel/28   3/8/11 12:24p vsilyaev
 * SW7425-93: Flush cache while saving data and use better offset->address
 * conversion
 * 
 * Hydra_Software_Devel/27   12/29/10 3:04p vsilyaev
 * SW7425-39: Fixed BPVRlib_Feed_GetStatus with enabled extended
 * descriptors
 * 
 * Hydra_Software_Devel/26   12/13/10 6:59p vsilyaev
 * SW7425-39: Added support for MUX input
 * 
 * Hydra_Software_Devel/25   1/25/10 12:50p vsilyaev
 * SW7405-3822: Updated to use right offset in the BPVRlib_Feed_AddEntries
 * 
 * Hydra_Software_Devel/24   7/16/09 11:32a vsilyaev
 * PR 55361: Fixed data capturing
 * 
 * Hydra_Software_Devel/23   5/22/09 5:03p vsilyaev
 * PR 51821 PR 54925 PR 55054: Detect and handle the 'false' finished
 * state, allocate and hide one extra descriptor
 * 
 * Hydra_Software_Devel/22   5/21/09 4:57p vsilyaev
 * PR 51821 PR 54925 PR 55054: Don't reset descriptors FIFO when HW
 * Playback is completed
 * 
 * Hydra_Software_Devel/21   5/21/09 1:33p vsilyaev
 * PR 55361: Added code to capture data after it was submitted to HW and
 * after HW completed DMA
 * 
 * Hydra_Software_Devel/20   2/9/09 5:36p vsilyaev
 * PR50211 PR46250 PR45133 PR45435 PR44649: Relaxed condiitions to test
 * for alligment with the playback descriptor
 * 
 * Hydra_Software_Devel/19   1/26/09 11:12a vsilyaev
 * PR50211 PR46250 PR45133 PR45435 PR44649: Fixed condition for the
 * address match
 * 
 * Hydra_Software_Devel/18   1/26/09 9:42a vsilyaev
 * PR50211 PR46250 PR45133 PR45435 PR44649: Reduce cases where S/G
 * descriptor is updated and when it's updated preserve payload wherever
 * is possible
 * 
 * Hydra_Software_Devel/17   1/23/09 9:01a gmohile
 * PR 50211 : Revert small desc bug fix for 7405 B0
 * 
 * Hydra_Software_Devel/16   1/22/09 4:23p gmohile
 * PR 50211 : Disable use of small desc bug for 7405 B0
 * 
 * Hydra_Software_Devel/15   10/28/08 1:15p vsilyaev
 * PR 45490: Fixed use of B_PVRLIB_SMALL_DESC
 * 
 * Hydra_Software_Devel/14   10/17/08 1:18p vsilyaev
 * PR 45490: Fixed typo
 * 
 * Hydra_Software_Devel/13   10/17/08 11:03a vsilyaev
 * PR 45490: Removed B_PVRLIB_SMALL_DESC for 3548/3556 B0+
 * 
 * Hydra_Software_Devel/12   9/3/08 6:20p vsilyaev
 * PR46250 PR45133 PR45435 PR44649: Print a warning and substitute size of
 * S/G entry not compatible with HW
 * 
 * Hydra_Software_Devel/11   8/5/08 5:32p vsilyaev
 * PR 45435: Print a warning of first or last descriptor has size of 0
 * 
 * Hydra_Software_Devel/10   4/16/08 6:57p mward
 * PR39994: Use BXPT_Playback_GetIntId().
 * 
 * Hydra_Software_Devel/9   4/8/08 4:03p erickson
 * PR39453: add 3548 & 3556
 *
 * Hydra_Software_Devel/8   12/5/07 10:33a katrep
 * PR37217: Added support for 7335.
 * 
 * Hydra_Software_Devel/7   11/1/07 9:40a erickson
 * PR36570: added 3563
 * 
 * Hydra_Software_Devel/6   10/31/07 4:44p jrubio
 * PR35015: adding 7325 support
 * 
 * Hydra_Software_Devel/5   8/17/07 10:19a vsilyaev
 * PR 33751: Deallocate uncached address
 * 
 * Hydra_Software_Devel/4   8/6/07 1:49p katrep
 * PR27643: 7405 has 5 playback inputs.
 * 
 * Hydra_Software_Devel/3   8/2/07 5:36p vsilyaev
 * PR 33751: Improved documentation of bpvrlib_feed module
 * 
 * Hydra_Software_Devel/2   8/2/07 4:49p vsilyaev
 * PR 33751: Reduced stack use
 * 
 * Hydra_Software_Devel/1   8/2/07 4:18p vsilyaev
 * PR 33751:
 * 
 *  
 ***************************************************************************/
#include "bstd.h"
#include "bkni.h"
#include "bpvrlib_feed.h"

/* to import TRANS_DESC flags */
#include "bxpt_priv.h"
#if BXPT_HAS_TSMUX
#include "bxpt_tsmux.h"
#endif

BDBG_MODULE(bpvrlib_feed);

#define BDBG_MSG_TRACE(x)   /* BDBG_MSG(x) */

/* save data prior to sending data into the XPT playback */
#define B_PVR_LIB_FEED_SAVE_BEFORE 0
/* save data after XPT playback completed transfer */
#define B_PVR_LIB_FEED_SAVE_AFTER 0
#define B_PVR_LIB_FEED_CONTEXT_NAME 0

BDBG_OBJECT_ID(BPVRlib_Feed);

#if B_PVR_LIB_FEED_SAVE_BEFORE || B_PVR_LIB_FEED_SAVE_AFTER
#include <stdio.h>
typedef struct b_pvr_feed_save{
    FILE *fout;
    unsigned no;
    const char *name;
} b_pvr_feed_save;

static void b_pvr_feed_save_init(b_pvr_feed_save *save, const char *name)
{
    save->name = name;
    save->fout = NULL;
    save->no = 0;
    return;
}

static void b_pvr_feed_save_open(b_pvr_feed_save *save)
{
    char name[64];
    BKNI_Snprintf(name, sizeof(name), 
#if B_PVR_LIB_FEED_CONTEXT_NAME
  "videos/bpvrlib_feed_%#x_%s_%u.mpg", save,
#else
  "videos/bpvrlib_feed_%s_%u.mpg", 
#endif
    save->name, save->no);
    save->fout = fopen(name, "w+b");
    return;
}

static void b_pvr_feed_save_close(b_pvr_feed_save *save)
{
    if(save->fout) {
        fclose(save->fout);
        save->fout = NULL;
        save->no++;
    }
    return;
}

static void b_pvr_feed_save_data(b_pvr_feed_save *save, const void *data, size_t len)
{
   if(save->fout) {
       BDBG_MSG(("D: %-8s: %#x:%u", save->name, (unsigned)data, (unsigned)len)); 
       fwrite(data, len, 1, save->fout);
   }
   return;
}

static void b_pvr_feed_save_data_offset(BMEM_Heap_Handle  heap, b_pvr_feed_save *save, uint32_t data, size_t len)
{
    void *addr;
    void *cachedAddr;
    BERR_Code rc;

    rc = BMEM_Heap_ConvertOffsetToAddress(heap, data, &addr);
    BDBG_ASSERT(rc==BERR_SUCCESS);
    BDBG_ASSERT(addr!=0);
    rc = BMEM_Heap_ConvertAddressToCached(heap, addr, &cachedAddr);    
    BDBG_ASSERT(cachedAddr!=0);
    BDBG_ASSERT(rc==BERR_SUCCESS);
    BMEM_Heap_FlushCache(heap, cachedAddr, len);
    b_pvr_feed_save_data(save, cachedAddr, len);
}


#endif /* B_PVR_LIB_FEED_SAVE_BEFORE || B_PVR_LIB_FEED_SAVE_AFTER */

#define B_PVRLIB_SMALL_DESC_BUG   BXPT_2_BYTE_MINIMUM_PLAYBACK_BUFFER

#define BFIFO_HEAD(name, type) struct name { type *bf_base; type *bf_last; type *bf_read; type *bf_write; int bf_wrap; }

#define BFIFO_INIT(fifo, base, size) do {(fifo)->bf_wrap=0; \
    (fifo)->bf_base=(fifo)->bf_read=(fifo)->bf_write=(base);\
    (fifo)->bf_last=(fifo)->bf_base+(size);}while(0)

#define BFIFO_WRITE(fifo) (fifo)->bf_write
#define BFIFO_READ(fifo) (fifo)->bf_read

#define BFIFO_WRITE_PEEK(fifo) \
    /* |====W---R===| */ ((unsigned)(((fifo)->bf_write < (fifo)->bf_read) ? (fifo)->bf_read - (fifo)->bf_write : ( \
    /* |---R===W---| */ ((fifo)->bf_write > (fifo)->bf_read) ? (fifo)->bf_last - (fifo)->bf_write  : ( \
    /* |---RW---| */ (fifo)->bf_wrap ? 0 : (fifo)->bf_last - (fifo)->bf_write))))

#define BFIFO_READ_PEEK(fifo) \
    /* |====W---R===| */ ((unsigned)(((fifo)->bf_write < (fifo)->bf_read) ? (fifo)->bf_last - (fifo)->bf_read : ( \
    /* |---R===W---| */ ((fifo)->bf_write > (fifo)->bf_read) ? (fifo)->bf_write - (fifo)->bf_read : ( \
    /* |---RW---| */ (fifo)->bf_wrap ? (fifo)->bf_last - (fifo)->bf_read:0))))

#define BFIFO_WRITE_COMMIT(fifo, size) do { \
    BDBG_ASSERT((size)>0 && BFIFO_WRITE_PEEK(fifo) >= (size));  \
    (fifo)->bf_write += (size); \
    if ((fifo)->bf_write >= (fifo)->bf_last) {(fifo)->bf_write = (fifo)->bf_base;(fifo)->bf_wrap++;}  \
    } while(0)

#define BFIFO_READ_COMMIT(fifo, size) do { \
    BDBG_ASSERT(BFIFO_READ_PEEK(fifo) >= (unsigned)(size)); \
    (fifo)->bf_read += (size); \
    if ((fifo)->bf_read >= (fifo)->bf_last) {(fifo)->bf_read = (fifo)->bf_base;(fifo)->bf_wrap--;}  \
    } while(0)

#define BFIFO_WRITE_LEFT(fifo)  \
    /* |====W---R===| */ (((fifo)->bf_write < (fifo)->bf_read) ? (fifo)->bf_read - (fifo)->bf_write : ( \
    /* |---R===W---| */ ((fifo)->bf_write > (fifo)->bf_read) ? ((fifo)->bf_read - (fifo)->bf_base) + ((fifo)->bf_last - (fifo)->bf_write)  : ( \
    /* |---RW---| */ (fifo)->bf_wrap ? 0 : (fifo)->bf_last - (fifo)->bf_base)))

#define BFIFO_READ_LEFT(fifo) \
    /* |====W---R===| */ (((fifo)->bf_write < (fifo)->bf_read) ? ((fifo)->bf_write - (fifo)->bf_base) + (fifo)->bf_last - (fifo)->bf_read : ( \
    /* |---R===W---| */ ((fifo)->bf_write > (fifo)->bf_read) ? (fifo)->bf_write - (fifo)->bf_read : ( \
    /* |---RW---| */ (fifo)->bf_wrap ? (fifo)->bf_last - (fifo)->bf_base:0)))

#define BFIFO_VALIDATE(fifo) do { \
        BDBG_ASSERT((fifo)->bf_wrap==0 || (fifo)->bf_wrap==1); \
        BDBG_ASSERT((fifo)->bf_read>=(fifo)->bf_base && (fifo)->bf_read<(fifo)->bf_last); \
        BDBG_ASSERT((fifo)->bf_write>=(fifo)->bf_base && (fifo)->bf_write<(fifo)->bf_last); \
    } while(0)

#define BFIFO_STATUS(DBG, header,fifo)  DBG((header ":(%#lx:%#lx) write (%u/%u/%#lx), read (%u/%u/%#lx)", (unsigned long)(fifo)->bf_base, (unsigned long)(fifo)->bf_last, BFIFO_WRITE_PEEK(fifo), BFIFO_WRITE_LEFT(fifo), (unsigned long)BFIFO_WRITE(fifo), BFIFO_READ_PEEK(fifo), BFIFO_READ_LEFT(fifo), (unsigned long)BFIFO_READ(fifo)))

BFIFO_HEAD(B_PVRlib_DescrFifo, BXPT_PvrDescriptor);


/* nc_ stands for Non Cached address, all other pointers are cached */
struct BPVRlib_Feed {
    BDBG_OBJECT(BPVRlib_Feed)
    bool active; /* set to true if hardware sending data */
#if defined(HUMAX_PLATFORM_BASE) && defined(CONFIG_BRCM_PATCH_TRANSCODING_32BIT_TIMESTAMP) /*Timestamp*/
#if !BXPT_HAS_MULTICHANNEL_PLAYBACK
    bool finished; /* true if finished bit is set */
    bool false_finished; /* true if finished bit is set and new descriptor was added, finished bit then might belong to the previous descriptor */
    const BXPT_PvrDescriptor *prev_hw_finished_desc; /* previous descriptor that was active in the hardware */
#else
    const BXPT_PvrDescriptor *prev_hw_completed_desc; /* previous descriptor that was completed by the hardware */
#endif
#else
    bool finished; /* true if finished bit is set */
    bool false_finished; /* true if finished bit is set and new descriptor was added, finished bit then might belong to the previous descriptor */
    const BXPT_PvrDescriptor *prev_hw_finished_desc; /* previous descriptor that was active in the hardware */
#endif
    const BXPT_PvrDescriptor *last_desc; /* last descriptor submitted to hardware */
    struct B_PVRlib_DescrFifo descFifo;
    size_t  ncompleted;
    BXPT_PvrDescriptor *desc; /* cached pointer to the descriptors array */
    BXPT_PvrDescriptor *nc_desc; /* no-cached pointer to the descriptors array */
    uint32_t off_desc; /* device offset for the descriptor */
    BPVRlib_Feed_Settings config;
    BINT_CallbackHandle hPlayInt;   /* cb Handle for playback interrupt */
#if defined(HUMAX_PLATFORM_BASE) && defined(CONFIG_BRCM_PATCH_TRANSCODING_32BIT_TIMESTAMP) /*Timestamp*/
    /*NONE*/
#else
    BXPT_PvrDescriptor dummy_descr;
#endif
    unsigned descScale;
#if B_PVRLIB_SMALL_DESC_BUG   
#define B_PVRLIB_BOUNCE_BUF_SIZE    256
#define B_PVRLIB_SMALL_ADDR_TEST(a1,a2) (((a1)&0x1F) == ((a2)&0x1F) || ((a2)&0xF)==0)
    uint8_t *bounce_buffer;
    uint32_t bounce_offset;
    size_t bounce_ptr;
    uint32_t last_data_addr;
#else
#define B_PVRLIB_BOUNCE_BUF_SIZE    0
#endif
    BPVRlib_Feed_OffsetEntry offset_entries[8];
#if B_PVR_LIB_FEED_SAVE_BEFORE || B_PVR_LIB_FEED_SAVE_AFTER
    struct {
#if B_PVR_LIB_FEED_SAVE_BEFORE 
        b_pvr_feed_save before;
#endif
#if B_PVR_LIB_FEED_SAVE_AFTER
        b_pvr_feed_save after;
#endif
    } save;
#endif
};

static void BPVRlib_Feed_Priv_Update(BPVRlib_Feed_Handle feed);

void 
BPVRlib_Feed_GetDefaultSettings(BPVRlib_Feed_Settings *config)
{
    BKNI_Memset(config, 0, sizeof(*config));
    return;
}

static void 
BPVRlib_Feed_Priv_Reset(BPVRlib_Feed_Handle feed)
{
    feed->ncompleted = 0;
    feed->active = false;
#if defined(HUMAX_PLATFORM_BASE) && defined(CONFIG_BRCM_PATCH_TRANSCODING_32BIT_TIMESTAMP) /*Timestamp*/
#if !BXPT_HAS_MULTICHANNEL_PLAYBACK
    feed->false_finished = false;
    feed->finished = false;
    feed->prev_hw_finished_desc = NULL;
#else
    feed->prev_hw_completed_desc = NULL;
#endif
    feed->last_desc = NULL;
#else
    feed->false_finished = false;
    feed->finished = false;
    feed->last_desc = NULL;
    feed->prev_hw_finished_desc = NULL;
#endif
    BFIFO_INIT(&feed->descFifo, feed->desc, feed->config.numDesc*feed->descScale);
#if B_PVRLIB_SMALL_DESC_BUG   
    feed->bounce_buffer = (uint8_t *)feed->nc_desc + feed->descScale*sizeof(*feed->desc)*feed->config.numDesc;
    feed->bounce_offset = feed->off_desc + feed->descScale*sizeof(*feed->desc)*feed->config.numDesc;
    feed->bounce_ptr = 0;
    feed->last_data_addr = 0;
#endif
    return;
}

BERR_Code 
BPVRlib_Feed_Open(BPVRlib_Feed_Handle *pfeed, const BPVRlib_Feed_Settings *config)
{
    BERR_Code rc;
    BPVRlib_Feed_Handle feed;
    void *cached;
    BINT_Id playInt;

    BDBG_ASSERT(pfeed);
    *pfeed = NULL;
    if(config == NULL) {
        rc = BERR_TRACE(BERR_INVALID_PARAMETER);
        goto err_parameter;
    }
    if(config->xptHandle==NULL || config->xptPlayHandle==NULL) {
        rc = BERR_TRACE(BERR_INVALID_PARAMETER);
        goto err_parameter;
    }
    if(config->heap==NULL || config->intHandle==NULL) {
        rc = BERR_TRACE(BERR_INVALID_PARAMETER);
        goto err_parameter;
    }
    if( config->numDesc<2) {
        rc = BERR_TRACE(BERR_INVALID_PARAMETER);
        goto err_parameter;
    }
#if !BXPT_HAS_TSMUX
    if(config->useExtndedDesc) {
        rc = BERR_TRACE(BERR_NOT_SUPPORTED);
        goto err_parameter;
    }
#endif

    feed = BKNI_Malloc(sizeof(*feed));
    if(!feed) {
        rc = BERR_TRACE(BERR_OUT_OF_SYSTEM_MEMORY);
        goto err_alloc;
    }
    BDBG_OBJECT_INIT(feed, BPVRlib_Feed);
    feed->config = *config;
#if defined(HUMAX_PLATFORM_BASE) && defined(CONFIG_BRCM_PATCH_TRANSCODING_32BIT_TIMESTAMP) /*Timestamp*/
#if BXPT_HAS_MULTICHANNEL_PLAYBACK
    feed->descScale = 1;
#elif BXPT_HAS_TSMUX
    feed->descScale = config->useExtndedDesc ? 2: 1;
#else
    feed->descScale = 1;
    if(config->useExtndedDesc) {
        rc = BERR_TRACE(BERR_INVALID_PARAMETER);
        goto err_desc;
    }
#endif
#else
    feed->descScale = config->useExtndedDesc ? 2: 1;
#endif
    feed->config.numDesc += feed->descScale; /* increase number of descriptors, since last descriptor can't be always used */
    feed->nc_desc = BMEM_Heap_AllocAligned(config->heap, sizeof(*feed->desc)*feed->descScale*feed->config.numDesc+B_PVRLIB_BOUNCE_BUF_SIZE, 4, 0);
    if(!feed->nc_desc) {
        rc = BERR_TRACE(BERR_OUT_OF_DEVICE_MEMORY);
        goto err_desc;
    }
    rc = BMEM_Heap_ConvertAddressToOffset(config->heap, feed->nc_desc, &feed->off_desc);
    if(rc!=BERR_SUCCESS) {
        rc = BERR_TRACE(rc);
        goto err_offset;
    }
    rc = BMEM_Heap_ConvertAddressToCached(config->heap, feed->nc_desc, &cached);
    if(rc!=BERR_SUCCESS) {
        rc = BERR_TRACE(rc);
        goto err_cached;
    }
    feed->desc = cached;
#if defined(HUMAX_PLATFORM_BASE) && defined(CONFIG_BRCM_PATCH_TRANSCODING_32BIT_TIMESTAMP) /*Timestamp*/
    BKNI_Memset(feed->desc, 0, sizeof(*feed->desc)*feed->descScale*feed->config.numDesc+B_PVRLIB_BOUNCE_BUF_SIZE);
#endif
    BPVRlib_Feed_Priv_Reset(feed);
	playInt = BXPT_Playback_GetIntId(config->xptPlayHandle,BXPT_PbInt_eDone);
    feed->hPlayInt = NULL;
    if(config->descAvaliable_isr) {
        rc = BINT_CreateCallback(&feed->hPlayInt, feed->config.intHandle, playInt, config->descAvaliable_isr, config->applicationCnxt, config->applicationNumber);
        if(rc!=BERR_SUCCESS) {
            rc = BERR_TRACE(rc);
            goto err_int_callback;
        }
    }
#if B_PVR_LIB_FEED_SAVE_BEFORE 
    b_pvr_feed_save_init(&feed->save.before,"before");
#endif
#if B_PVR_LIB_FEED_SAVE_AFTER
    b_pvr_feed_save_init(&feed->save.after,"after");
#endif

    *pfeed = feed;
    return BERR_SUCCESS;

err_int_callback:
err_cached:
err_offset:
    BMEM_Heap_Free(feed->config.heap, feed->desc);
err_desc:
    BKNI_Free(feed);
err_alloc:
err_parameter:
    return rc;
}


void 
BPVRlib_Feed_Close(BPVRlib_Feed_Handle feed)
{
    BDBG_OBJECT_ASSERT(feed, BPVRlib_Feed);
#if B_PVR_LIB_FEED_SAVE_BEFORE 
    b_pvr_feed_save_close(&feed->save.before);
#endif
#if B_PVR_LIB_FEED_SAVE_AFTER
    b_pvr_feed_save_close(&feed->save.after);
#endif
    if(feed->hPlayInt) {
        BINT_DestroyCallback(feed->hPlayInt);
    }
    BMEM_Heap_Free(feed->config.heap, feed->nc_desc);
    BDBG_OBJECT_DESTROY(feed, BPVRlib_Feed);
    BKNI_Free(feed);
    return;
}


BERR_Code 
BPVRlib_Feed_Start(BPVRlib_Feed_Handle feed)
{
    BERR_Code rc;
    BERR_Code erc;

    BDBG_OBJECT_ASSERT(feed, BPVRlib_Feed);
    rc = BXPT_Playback_StartChannel(feed->config.xptPlayHandle);
    if(rc!=BERR_SUCCESS) { rc = BERR_TRACE(rc); goto err_start; }

    if(feed->hPlayInt) {
        rc = BINT_EnableCallback(feed->hPlayInt);
        if(rc!=BERR_SUCCESS) { rc = BERR_TRACE(rc);goto err_enable_int;}
    }
    BPVRlib_Feed_Priv_Reset(feed);
#if B_PVR_LIB_FEED_SAVE_BEFORE 
    b_pvr_feed_save_open(&feed->save.before);
#endif
#if B_PVR_LIB_FEED_SAVE_AFTER
    b_pvr_feed_save_open(&feed->save.after);
#endif
    return BERR_SUCCESS;

err_enable_int:
    erc = BXPT_Playback_StopChannel(feed->config.xptPlayHandle); /* use different 'erc' in the error path */
    if(erc!=BERR_SUCCESS) { erc = BERR_TRACE(erc); } /* keep on going */
err_start:
    return rc;
}


void 
BPVRlib_Feed_Stop(BPVRlib_Feed_Handle feed)
{
    BERR_Code rc;
    BDBG_OBJECT_ASSERT(feed, BPVRlib_Feed);
#if B_PVR_LIB_FEED_SAVE_BEFORE 
    b_pvr_feed_save_close(&feed->save.before);
#endif
#if B_PVR_LIB_FEED_SAVE_AFTER
    b_pvr_feed_save_close(&feed->save.after);
#endif
    rc = BXPT_Playback_StopChannel(feed->config.xptPlayHandle);
    if(rc!=BERR_SUCCESS) {
        rc = BERR_TRACE(rc);
    }
    if(feed->hPlayInt) {
        BINT_DisableCallback(feed->hPlayInt);
    }
    return;
}
#if defined(HUMAX_PLATFORM_BASE) && defined(CONFIG_BRCM_PATCH_TRANSCODING_32BIT_TIMESTAMP) /*Timestamp*/
static unsigned 
BPVRlib_Feed_Priv_AddOffsetEntries(BPVRlib_Feed_Handle feed, const BPVRlib_Feed_OffsetEntry *entries, size_t nentries, bool last, bool extended)
{
    unsigned entries_left;
    unsigned entrySize;
    BERR_Code rc;
    BXPT_PvrDescriptor *prev_desc;
    BXPT_PvrDescriptor *first_desc=NULL;
    BDBG_MSG_TRACE(("BPVRlib_Feed_Priv_AddOffsetEntries>: %#lx %#lx:%u %s", (unsigned long)feed, (unsigned long)entries, nentries, last?"LAST":""));

    entrySize = extended ? sizeof(BPVRlib_Feed_ExtendedOffsetEntry) : sizeof(*entries);

    for(prev_desc=NULL, entries_left=nentries;entries_left>0;) {
        unsigned i;
        unsigned nwrite_desc = BFIFO_WRITE_PEEK(&feed->descFifo);
        BXPT_PvrDescriptor *write_desc;
        BXPT_PvrDescriptor *desc;
    
        if(nwrite_desc==0 || 
                (nwrite_desc<=feed->descScale && BFIFO_WRITE_LEFT(&feed->descFifo)==(int)feed->descScale) /* we should always keep one entry in the FIFO empty since it's reserved for the 'last' dscriptor that is used for chaining */
                ) { 
            break;
        }
        if(nwrite_desc>entries_left*feed->descScale) {
            nwrite_desc = entries_left*feed->descScale;
        }
        BDBG_ASSERT(nwrite_desc>0);
        write_desc = BFIFO_WRITE(&feed->descFifo);
#if !BXPT_HAS_MULTICHANNEL_PLAYBACK
        if(feed->finished) {
            feed->finished = false;
            feed->false_finished = true;
        }
        if(feed->false_finished && write_desc+(nwrite_desc-feed->descScale) == feed->prev_hw_finished_desc) {
            BDBG_WRN(("BPVRlib_Feed_Priv_AddOffsetEntries: %#lx preventing use of the last_finished_desc %#lx:%u", (unsigned long)feed, (unsigned long)feed->prev_hw_finished_desc, nwrite_desc));
            if(nwrite_desc==feed->descScale) {
                unsigned i;
                for(i=0;i<10;i++) {
                    BPVRlib_Feed_Priv_Update(feed);
                    if(!feed->false_finished) {
                        break;
                    }
                    BDBG_WRN(("BPVRlib_Feed_Priv_AddOffsetEntries: %#lx wating(%u) to release last_finished_desc %#lx:%u", (unsigned long)feed, i, (unsigned long)feed->prev_hw_finished_desc));
                    BKNI_Sleep(1);
                } 
                /* try to use it anyway */
            } else {
                nwrite_desc-=feed->descScale;
            }
        }
#endif
        BDBG_ASSERT(write_desc);
        BFIFO_WRITE_COMMIT(&feed->descFifo, nwrite_desc);
        entries_left -= nwrite_desc/feed->descScale; 
        if(first_desc==NULL) {
            first_desc = write_desc;
        }
        for( desc=write_desc, i=0; i<nwrite_desc; i+=feed->descScale, desc+=feed->descScale, entries=(void *)((uint8_t*)entries+entrySize) ) {
#if BXPT_HAS_TSMUX
            if(feed->config.useExtndedDesc) {
                BXPT_PvrDescriptor *desc8 = (void *)(feed->nc_desc + (desc - feed->desc));

                if(entrySize==sizeof(*entries)) {
                    BAVC_TsMux_DescConfig flags;
    
                    BKNI_Memset( &flags, 0, sizeof (flags) );
                    BXPT_Playback_ConfigTsMuxDesc(feed->config.xptPlayHandle, desc8, &flags);
                } else {
                    BXPT_Playback_ConfigTsMuxDesc(feed->config.xptPlayHandle, desc8, &((BPVRlib_Feed_ExtendedOffsetEntry *)entries)->flags);
                }
            }
#endif
            desc->BufferStartAddr = entries->offset;
            desc->BufferLength = entries->len;
            desc->Flags = 0;
            if(prev_desc) {
                prev_desc->NextDescAddr = feed->off_desc + ((uint32_t)desc - (uint32_t)feed->desc);
                if(i==0) { /* set NextDescAddr over the wrap */
                    rc = BMEM_Heap_FlushCache(feed->config.heap, prev_desc, sizeof(*prev_desc)); /* flush a cache */
                    if(rc) { rc = BERR_TRACE(rc);}
                }
            }

            prev_desc = desc;
            BDBG_MSG_TRACE(("BPVRlib_Feed_Priv_AddOffsetEntries: %#lx desc %u:%#lx %#lx(%#lx:%u)", (unsigned long)feed, i, desc, feed->off_desc + ((uint32_t)desc - (uint32_t)feed->desc), entries->offset, entries->len));
#if B_PVRLIB_SMALL_DESC_BUG   
            switch(entries->len) {
            case 0:
                desc->BufferLength = 2;
                BDBG_WRN(("BPVRlib_Feed_Priv_AddOffsetEntries: %#lx not supported S/G entry %#lx:%u, replaced with %#lx:%u", (unsigned long)feed, (unsigned long)entries->offset, (unsigned)entries->len, (unsigned long)desc->BufferStartAddr, (unsigned)desc->BufferLength));
                break;
            case 1:
                BDBG_WRN(("BPVRlib_Feed_Priv_AddOffsetEntries: %#lx S/G entry %#lx:%u(%#lx)", (unsigned long)feed, (unsigned long)entries->offset, (unsigned)entries->len, (unsigned long)feed->last_data_addr));
                if(B_PVRLIB_SMALL_ADDR_TEST(feed->last_data_addr, entries->offset)) { 
                    void *addr;
                    BERR_Code rc = BMEM_Heap_ConvertOffsetToAddress(feed->config.heap, entries->offset, &addr);
                    if(rc==BERR_SUCCESS) { /* XXX This path uses uncached addressses to copy contents of the descriptor to a bounce buffer */
                        for(;B_PVRLIB_SMALL_ADDR_TEST(feed->last_data_addr, feed->bounce_offset+(feed->bounce_ptr%B_PVRLIB_BOUNCE_BUF_SIZE));feed->bounce_ptr++) { }
                        feed->bounce_buffer[feed->bounce_ptr%B_PVRLIB_BOUNCE_BUF_SIZE]=*(uint8_t *)addr;
                        desc->BufferStartAddr = feed->bounce_offset+(feed->bounce_ptr%B_PVRLIB_BOUNCE_BUF_SIZE);
                        feed->bounce_ptr++;
                    } else {
                        rc=BERR_TRACE(rc);
                        desc->BufferLength = 2;
                    } 
                    BDBG_WRN(("BPVRlib_Feed_Priv_AddOffsetEntries: %#lx not supported S/G entry %#lx:%u, replaced with %#lx:%u", (unsigned long)feed, (unsigned long)entries->offset, (unsigned)entries->len, (unsigned long)desc->BufferStartAddr, (unsigned)desc->BufferLength));
                }
                break;
            default:
                break;
            }
            feed->last_data_addr = entries->offset + entries->len + 1;
#endif /* B_PVRLIB_SMALL_ADDR_TEST */
#if B_PVR_LIB_FEED_SAVE_BEFORE 
            b_pvr_feed_save_data_offset(feed->config.heap, &feed->save.before, desc->BufferStartAddr, desc->BufferLength);
#endif
        }
        prev_desc->NextDescAddr = TRANS_DESC_LAST_DESCR_IND;
        rc = BMEM_Heap_FlushCache(feed->config.heap, write_desc, nwrite_desc*sizeof(*write_desc));
        if(rc!=BERR_SUCCESS) {
            rc = BERR_TRACE(rc);
            break;
        }
        feed->last_desc = prev_desc;
    }
    if(first_desc) {
        BXPT_PvrDescriptor *nc_prev_desc;
        BXPT_PvrDescriptor *nc_first_desc;
        BDBG_ASSERT(prev_desc!=NULL);
        /* convert first descriptor to uncached address */
        feed->active = true;
        nc_first_desc = feed->nc_desc + (first_desc - feed->desc);
        nc_prev_desc = feed->nc_desc + (prev_desc- feed->desc);
        if(last || nentries!=entries_left ) {
            prev_desc->Flags = TRANS_DESC_INT_FLAG;
            rc = BMEM_Heap_FlushCache(feed->config.heap, prev_desc, sizeof(*prev_desc)); /* flush a cache */
            if(rc!=BERR_SUCCESS) { rc = BERR_TRACE(rc); }
        }
        BDBG_MSG_TRACE(("BPVRlib_Feed_Priv_AddOffsetEntries: %#lx first:%#lx(%#lx) last:%#lx(%#lx)", (unsigned long)feed, (unsigned long)first_desc, feed->off_desc + ((uint32_t)first_desc - (uint32_t)feed->nc_desc), (unsigned long)prev_desc, feed->off_desc + ((uint32_t)prev_desc - (uint32_t)feed->nc_desc)));
        rc = BXPT_Playback_AddDescriptors(feed->config.xptPlayHandle, nc_prev_desc, nc_first_desc); /* last descriptor goes as second argument and first descriptor goes as third argument */
        if(rc!=BERR_SUCCESS) {
            rc = BERR_TRACE(rc);
        }
    }
    BDBG_MSG_TRACE(("BPVRlib_Feed_Priv_AddOffsetEntries<: %#lx %#lx:%u ->%u", (unsigned long)feed, (unsigned long)entries, nentries, nentries-entries_left));
    return nentries-entries_left;
}
#else
static unsigned 
BPVRlib_Feed_Priv_AddOffsetEntries(BPVRlib_Feed_Handle feed, const BPVRlib_Feed_OffsetEntry *entries, size_t nentries, bool last, bool extended)
{
    unsigned entries_left;
    unsigned entrySize;
    BERR_Code rc;
    BXPT_PvrDescriptor *prev_desc;
    BXPT_PvrDescriptor *first_desc=NULL;
    BDBG_MSG_TRACE(("BPVRlib_Feed_Priv_AddOffsetEntries>: %#lx %#lx:%u %s", (unsigned long)feed, (unsigned long)entries, nentries, last?"LAST":""));

    entrySize = extended ? sizeof(BPVRlib_Feed_ExtendedOffsetEntry) : sizeof(*entries);

    for(prev_desc=&feed->dummy_descr, entries_left=nentries;entries_left>0;) {
        unsigned i;
        unsigned nwrite_desc = BFIFO_WRITE_PEEK(&feed->descFifo);
        BXPT_PvrDescriptor *write_desc;
        BXPT_PvrDescriptor *desc;
    
        if(nwrite_desc==0 || 
                (nwrite_desc<=feed->descScale && BFIFO_WRITE_LEFT(&feed->descFifo)==(int)feed->descScale) /* we should always keep one entry in the FIFO empty since it's reserved for the 'last' dscriptor that is used for chaining */
                ) { 
            break;
        }
        if(nwrite_desc>entries_left*feed->descScale) {
            nwrite_desc = entries_left*feed->descScale;
        }
        BDBG_ASSERT(nwrite_desc>0);
        write_desc = BFIFO_WRITE(&feed->descFifo);
        if(feed->finished) {
            feed->finished = false;
            feed->false_finished = true;
        }
        if(feed->false_finished && write_desc+(nwrite_desc-feed->descScale) == feed->prev_hw_finished_desc) {
            BDBG_WRN(("BPVRlib_Feed_Priv_AddOffsetEntries: %#lx preventing use of the last_finished_desc %#lx:%u", (unsigned long)feed, (unsigned long)feed->prev_hw_finished_desc, nwrite_desc));
            if(nwrite_desc==feed->descScale) {
                unsigned i;
                for(i=0;i<10;i++) {
                    BPVRlib_Feed_Priv_Update(feed);
                    if(!feed->false_finished) {
                        break;
                    }
                    BDBG_WRN(("BPVRlib_Feed_Priv_AddOffsetEntries: %#lx wating(%u) to release last_finished_desc %#lx:%u", (unsigned long)feed, i, (unsigned long)feed->prev_hw_finished_desc));
                    BKNI_Sleep(1);
                } 
                /* try to use it anyway */
            } else {
                nwrite_desc-=feed->descScale;
            }
        }
        BDBG_ASSERT(write_desc);
        BFIFO_WRITE_COMMIT(&feed->descFifo, nwrite_desc);
        entries_left -= nwrite_desc/feed->descScale;
        if(first_desc==NULL) {
            first_desc = write_desc;
        }
        for( desc=write_desc, i=0; i<nwrite_desc; i+=feed->descScale, desc+=feed->descScale, entries=(void *)((uint8_t*)entries+entrySize) ) {
#if BXPT_HAS_TSMUX
            if(feed->config.useExtndedDesc) {
                BXPT_PvrDescriptor8 *desc8 = (void *)desc;
                if(entrySize==sizeof(*entries)) {
                    desc8->Reserved0=0;
                    desc8->MuxingFlags = 0;
                    desc8->NextPacketPacingTimestamp = 0;
                    desc8->Pkt2PktPacingTimestampDelta = 0;
                } else {
                    BXPT_Tsmux_ConfigDesc(desc8, &((BPVRlib_Feed_ExtendedOffsetEntry *)entries)->flags);
                }
            }
#endif
            desc->BufferStartAddr = entries->offset;
            desc->BufferLength = entries->len;
            desc->Flags = 0;
            prev_desc->NextDescAddr = feed->off_desc + ((uint32_t)desc - (uint32_t)feed->desc);
            prev_desc = desc;
            BDBG_MSG_TRACE(("BPVRlib_Feed_Priv_AddOffsetEntries: %#lx desc %u:%#lx %#lx(%#lx:%u)", (unsigned long)feed, i, desc, feed->off_desc + ((uint32_t)desc - (uint32_t)feed->desc), entries->offset, entries->len));
#if B_PVRLIB_SMALL_DESC_BUG   
            switch(entries->len) {
            case 0:
                desc->BufferLength = 2;
                BDBG_WRN(("BPVRlib_Feed_Priv_AddOffsetEntries: %#lx not supported S/G entry %#lx:%u, replaced with %#lx:%u", (unsigned long)feed, (unsigned long)entries->offset, (unsigned)entries->len, (unsigned long)desc->BufferStartAddr, (unsigned)desc->BufferLength));
                break;
            case 1:
                BDBG_WRN(("BPVRlib_Feed_Priv_AddOffsetEntries: %#lx S/G entry %#lx:%u(%#lx)", (unsigned long)feed, (unsigned long)entries->offset, (unsigned)entries->len, (unsigned long)feed->last_data_addr));
                if(B_PVRLIB_SMALL_ADDR_TEST(feed->last_data_addr, entries->offset)) { 
                    void *addr;
                    BERR_Code rc = BMEM_Heap_ConvertOffsetToAddress(feed->config.heap, entries->offset, &addr);
                    if(rc==BERR_SUCCESS) { /* XXX This path uses uncached addressses to copy contents of the descriptor to a bounce buffer */
                        for(;B_PVRLIB_SMALL_ADDR_TEST(feed->last_data_addr, feed->bounce_offset+(feed->bounce_ptr%B_PVRLIB_BOUNCE_BUF_SIZE));feed->bounce_ptr++) { }
                        feed->bounce_buffer[feed->bounce_ptr%B_PVRLIB_BOUNCE_BUF_SIZE]=*(uint8_t *)addr;
                        desc->BufferStartAddr = feed->bounce_offset+(feed->bounce_ptr%B_PVRLIB_BOUNCE_BUF_SIZE);
                        feed->bounce_ptr++;
                    } else {
                        rc=BERR_TRACE(rc);
                        desc->BufferLength = 2;
                    } 
                    BDBG_WRN(("BPVRlib_Feed_Priv_AddOffsetEntries: %#lx not supported S/G entry %#lx:%u, replaced with %#lx:%u", (unsigned long)feed, (unsigned long)entries->offset, (unsigned)entries->len, (unsigned long)desc->BufferStartAddr, (unsigned)desc->BufferLength));
                }
                break;
            default:
                break;
            }
            feed->last_data_addr = entries->offset + entries->len + 1;
#endif /* B_PVRLIB_SMALL_ADDR_TEST */
#if B_PVR_LIB_FEED_SAVE_BEFORE 
            b_pvr_feed_save_data_offset(feed->config.heap, &feed->save.before, desc->BufferStartAddr, desc->BufferLength);
#endif
        }
        prev_desc->NextDescAddr = TRANS_DESC_LAST_DESCR_IND;
        rc = BMEM_Heap_FlushCache(feed->config.heap, write_desc, nwrite_desc*sizeof(*write_desc));
        if(rc!=BERR_SUCCESS) {
            rc = BERR_TRACE(rc);
            break;
        }
        feed->last_desc = prev_desc;
        /* prev_desc points to the last descriptor, convert it to uncached address */
        prev_desc = feed->nc_desc + (prev_desc - feed->desc);
    }
    if(first_desc) {
        BDBG_ASSERT(prev_desc!=&feed->dummy_descr);
        /* convert first descriptor to uncached address */
        feed->active = true;
        first_desc = feed->nc_desc + (first_desc - feed->desc);
        if(last || nentries!=entries_left ) {
            BXPT_PvrDescriptor *desc = feed->desc + (prev_desc - feed->nc_desc); /* convert to cached address */
            desc->Flags = TRANS_DESC_INT_FLAG;
            rc = BMEM_Heap_FlushCache(feed->config.heap, desc, sizeof(*desc)); /* flush a cache */
            if(rc!=BERR_SUCCESS) { rc = BERR_TRACE(rc); }
        }
        BDBG_MSG_TRACE(("BPVRlib_Feed_Priv_AddOffsetEntries: %#lx first:%#lx(%#lx) last:%#lx(%#lx)", (unsigned long)feed, (unsigned long)first_desc, feed->off_desc + ((uint32_t)first_desc - (uint32_t)feed->nc_desc), (unsigned long)prev_desc, feed->off_desc + ((uint32_t)prev_desc - (uint32_t)feed->nc_desc)));
        rc = BXPT_Playback_AddDescriptors(feed->config.xptPlayHandle, prev_desc, first_desc); /* last descriptor goes as second argument and first descriptor goes as third argument */
        if(rc!=BERR_SUCCESS) {
            rc = BERR_TRACE(rc);
        }
    }
    BDBG_MSG_TRACE(("BPVRlib_Feed_Priv_AddOffsetEntries<: %#lx %#lx:%u ->%u", (unsigned long)feed, (unsigned long)entries, nentries, nentries-entries_left));
    return nentries-entries_left;
}
#endif

static void 
BPVRlib_Feed_Priv_Update(BPVRlib_Feed_Handle feed)
{
    BERR_Code rc;
    BXPT_PvrDescriptor *nc_hw_desc;
    BXPT_PvrDescriptor *hw_desc;
    unsigned hw_complete;
    unsigned nread_desc;

    BDBG_MSG_TRACE(("BPVRlib_Feed_Priv_Update>: %#lx ncompleted:%u", (unsigned long)feed, feed->ncompleted));
    if(!feed->active) {
        goto done;
    }
#if defined(HUMAX_PLATFORM_BASE) && defined(CONFIG_BRCM_PATCH_TRANSCODING_32BIT_TIMESTAMP) /*Timestamp*/
#if BXPT_HAS_MULTICHANNEL_PLAYBACK
    rc = BXPT_Playback_GetLastCompletedDescriptorAddress(feed->config.xptPlayHandle, &nc_hw_desc); /* [in] Handle for the playback channel */
#else
    rc = BXPT_Playback_GetCurrentDescriptorAddress(feed->config.xptPlayHandle, &nc_hw_desc);
#endif
#else
    rc = BXPT_Playback_GetCurrentDescriptorAddress(feed->config.xptPlayHandle, &nc_hw_desc);
#endif
    if(rc!=BERR_SUCCESS) {
        rc = BERR_TRACE(rc);
        goto error;
    }
    hw_desc = feed->desc + (nc_hw_desc - feed->nc_desc);  /* convert to cached address */
#if defined(HUMAX_PLATFORM_BASE) && defined(CONFIG_BRCM_PATCH_TRANSCODING_32BIT_TIMESTAMP) /*Timestamp*/
#if BXPT_HAS_MULTICHANNEL_PLAYBACK
    BDBG_MSG_TRACE(("BPVRlib_Feed_Priv_Update: %#lx hw_desc:%#lx prev_hw_completed_desc:%#lx", (unsigned long)feed, (unsigned long)hw_desc, (unsigned long)feed->prev_hw_completed_desc));
    if(hw_desc == feed->prev_hw_completed_desc) { /* there was no progress in HW */
        goto done;
    }
    feed->prev_hw_completed_desc = hw_desc;
#else 
    if(hw_desc != feed->prev_hw_finished_desc) {
        feed->false_finished = false;
        feed->finished = false;
    }
#endif
#else
    if(hw_desc != feed->prev_hw_finished_desc) {
        feed->false_finished = false;
        feed->finished = false;
    }
#endif
    BDBG_MSG_TRACE(("BPVRlib_Feed_Priv_Update: %#lx hw_desc:%#lx last_desc:%#lx %u", (unsigned long)feed, (unsigned long)hw_desc, (unsigned long)feed->last_desc, BFIFO_READ_LEFT(&feed->descFifo)));
    BFIFO_STATUS(BDBG_MSG_TRACE,"BPVRlib_Feed_Priv_Update: fifo", &feed->descFifo);
#if defined(HUMAX_PLATFORM_BASE) && defined(CONFIG_BRCM_PATCH_TRANSCODING_32BIT_TIMESTAMP) /*Timestamp*/
#if !BXPT_HAS_MULTICHANNEL_PLAYBACK
    if(feed->false_finished) {
        BDBG_MSG(("BPVRlib_Feed_Priv_Update: %#lx false finished desc:%#lx .. %#lx (%u)", (unsigned long)feed, (unsigned long)hw_desc, BFIFO_READ(&feed->descFifo), BFIFO_READ_PEEK(&feed->descFifo)));
        goto done;
    }
#endif
   if(hw_desc==feed->last_desc) { /* if we completed last descriptor test whether playback completed */
#if !BXPT_HAS_MULTICHANNEL_PLAYBACK
        BXPT_Playback_ChannelStatus status;
        rc = BXPT_Playback_GetChannelStatus(feed->config.xptPlayHandle, &status);
        if(rc!=BERR_SUCCESS) {
            rc=BERR_TRACE(rc);
        } 
        BDBG_MSG_TRACE(("BPVRlib_Feed_Priv_Update: %#lx hw_desc:%#lx %s", (unsigned long)feed, (unsigned long)hw_desc, status.Finished?"FINISHED":""));
        if(status.Finished) { /* last descriptor was completed */
            feed->finished = true;
            feed->prev_hw_finished_desc = hw_desc;
#else /* !BXPT_HAS_MULTICHANNEL_PLAYBACK */
        {
#endif /* !BXPT_HAS_MULTICHANNEL_PLAYBACK */
#else
    if(feed->false_finished) {
        BDBG_MSG(("BPVRlib_Feed_Priv_Update: %#lx false finished desc:%#lx .. %#lx (%u)", (unsigned long)feed, (unsigned long)hw_desc, BFIFO_READ(&feed->descFifo), BFIFO_READ_PEEK(&feed->descFifo)));
        goto done;
    }
    if(hw_desc==feed->last_desc) { /* if we completed last descriptor test whether playback completed */
        BXPT_Playback_ChannelStatus status;
        rc = BXPT_Playback_GetChannelStatus(feed->config.xptPlayHandle, &status);
        if(rc!=BERR_SUCCESS) {
            rc=BERR_TRACE(rc);
        } 
        BDBG_MSG_TRACE(("BPVRlib_Feed_Priv_Update: %#lx last_desc:%#lx %s", (unsigned long)feed, (unsigned long)hw_desc, status.Finished?"FINISHED":""));
        if(status.Finished) { /* last descriptor was completed */
            feed->finished = true;
            feed->prev_hw_finished_desc = hw_desc;
#endif
            hw_complete = BFIFO_READ_LEFT(&feed->descFifo);
#if B_PVR_LIB_FEED_SAVE_AFTER
            {
                unsigned i;
                struct B_PVRlib_DescrFifo descFifo = feed->descFifo; /* do a copy, so we don't interfere with actual FIFO */
                for(i=0; BFIFO_READ_PEEK(&descFifo)!=0;i++) {
                    const BXPT_PvrDescriptor *desc = BFIFO_READ(&descFifo);
                    b_pvr_feed_save_data_offset(feed->config.heap, &feed->save.after, desc->BufferStartAddr, desc->BufferLength);
                    BFIFO_READ_COMMIT(&descFifo, feed->descScale);
                }
                BDBG_ASSERT(i==hw_complete/feed->descScale);
            }
#endif /* B_PVR_LIB_FEED_SAVE_AFTER */
            BDBG_MSG_TRACE(("BPVRlib_Feed_Priv_Update: %#lx recycle all(%u) data from FIFO", (unsigned long)feed, hw_complete));
            feed->ncompleted += hw_complete/feed->descScale;
            /* update read pointer to walk through all descriptors */
            nread_desc = BFIFO_READ_PEEK(&feed->descFifo);
            if(nread_desc) {
                BDBG_ASSERT(nread_desc <= hw_complete);
                hw_complete -= nread_desc;
                BFIFO_READ_COMMIT(&feed->descFifo, nread_desc);
            } /* we could have a wrap */
            nread_desc = BFIFO_READ_PEEK(&feed->descFifo);
            if(nread_desc) {
                BFIFO_READ_COMMIT(&feed->descFifo, nread_desc);
            }
            BDBG_ASSERT(nread_desc == hw_complete); /* number of reclaimed descriptors shoild be the same as number of queued descriptors */
            feed->active = false;
            goto done;
        }
        /* last descriptor still in use, proceed as usual */
    }
    if(hw_desc >= feed->desc && hw_desc < (feed->desc + feed->descScale * feed->config.numDesc)) {
        const BXPT_PvrDescriptor *desc;

        desc = BFIFO_READ(&feed->descFifo);
        nread_desc = BFIFO_READ_PEEK(&feed->descFifo);

        if(hw_desc < desc) { /* |===H---R===| HW descriptor is after wraparound, consume all 'peek'ed data */
            BDBG_MSG_TRACE(("BPVRlib_Feed_Priv_Update: %#lx [wrap] hw_desc:%#lx desc:%#lx nread_desc:%d", (unsigned long)feed, (unsigned long)hw_desc, (unsigned long)desc, nread_desc));
#if B_PVR_LIB_FEED_SAVE_AFTER
            {
                unsigned i;
                for(i=0;i<nread_desc;i++) {
                    b_pvr_feed_save_data_offset(feed->config.heap, &feed->save.after, desc[i].BufferStartAddr, desc[i].BufferLength);
                }
            }
#endif /* B_PVR_LIB_FEED_SAVE_AFTER */
            feed->ncompleted += nread_desc/feed->descScale;
            BFIFO_READ_COMMIT(&feed->descFifo, nread_desc);
            nread_desc = BFIFO_READ_PEEK(&feed->descFifo);
            desc = BFIFO_READ(&feed->descFifo);
        }
        BDBG_MSG_TRACE(("BPVRlib_Feed_Priv_Update: %#lx hw_desc:%#lx desc:%#lx nread_desc:%d", (unsigned long)feed, (unsigned long)hw_desc, (unsigned long)desc, nread_desc));
        if(hw_desc>=desc) { /* |-R===H-----| */
            hw_complete = hw_desc - desc;
            BDBG_MSG_TRACE(("BPVRlib_Feed_Priv_Update: %#lx off:%d nread_desc:%d", (unsigned long)feed, hw_complete, nread_desc));
            if(hw_complete<=nread_desc) {
                BDBG_MSG_TRACE(("BPVRlib_Feed_Priv_Update: %#lx completed:%u", (unsigned long)feed, (unsigned)hw_complete));
#if B_PVR_LIB_FEED_SAVE_AFTER
                {
                    unsigned i;
                    for(i=0;i<hw_complete;i++) {
                        b_pvr_feed_save_data_offset(feed->config.heap, &feed->save.after, desc[i].BufferStartAddr, desc[i].BufferLength);
                    }
                }
#endif /* B_PVR_LIB_FEED_SAVE_AFTER */
                feed->ncompleted += hw_complete/feed->descScale;
                BFIFO_READ_COMMIT(&feed->descFifo, hw_complete);
            } else {
                BDBG_ERR(("BPVRlib_Feed_Priv_Update: %#lx underflow %u .. %u(%#lx..%#lx[%#lx..%#lx])", (unsigned long)feed, (unsigned long)hw_complete, (unsigned long)nread_desc, (unsigned long)desc, (unsigned long)hw_desc, (unsigned long)feed->desc, (unsigned long)(feed->desc+feed->config.numDesc*feed->descScale)));
                goto error;
            }
        } else {
            BDBG_ERR(("BPVRlib_Feed_Priv_Update: %#lx descriptor out of sequence %#lx .. %#lx [%#lx..%#lx]", (unsigned long)feed, (unsigned long)desc, (unsigned long)hw_desc, (unsigned long)feed->desc, (unsigned long)(feed->desc+feed->config.numDesc*feed->descScale)));
            goto error;
        }
    } else {
        BDBG_ERR(("BPVRlib_Feed_Priv_Update: %#lx descriptor out of range %#lx .. %#lx .. %#lx", (unsigned long)feed, (unsigned long)feed->desc, (unsigned long)hw_desc, (unsigned long)(feed->desc+feed->config.numDesc*feed->descScale)));
        goto error;
    }

error:
done:
    BFIFO_STATUS(BDBG_MSG_TRACE,"BPVRlib_Feed_Priv_Update: fifo", &feed->descFifo);
    BDBG_MSG_TRACE(("BPVRlib_Feed_Priv_Update<: %#lx ncompleted:%u", (unsigned long)feed, feed->ncompleted));
    return;
}

BERR_Code
BPVRlib_Feed_AddOffsetEntries(BPVRlib_Feed_Handle feed, const BPVRlib_Feed_OffsetEntry *entries, size_t count, size_t *nconsumed)
{
    BDBG_MSG_TRACE(("BPVRlib_Feed_AddOffsetEntries: %#lx entries %#lx:%u", (unsigned long)feed, (unsigned long)entries, count));
    BDBG_OBJECT_ASSERT(feed, BPVRlib_Feed);

    BDBG_ASSERT(entries);
    BDBG_ASSERT(nconsumed);

    BPVRlib_Feed_Priv_Update(feed);
    *nconsumed = BPVRlib_Feed_Priv_AddOffsetEntries(feed, entries, count, true, false);
    return BERR_SUCCESS;
}

BERR_Code
BPVRlib_Feed_AddExtendedOffsetEntries(BPVRlib_Feed_Handle feed, const BPVRlib_Feed_ExtendedOffsetEntry *entries, size_t count, size_t *nconsumed)
{
    BDBG_MSG_TRACE(("BPVRlib_Feed_AddExtendedOffsetEntries: %#lx entries %#lx:%u", (unsigned long)feed, (unsigned long)entries, count));
    BDBG_OBJECT_ASSERT(feed, BPVRlib_Feed);

    BDBG_ASSERT(entries);
    BDBG_ASSERT(nconsumed);
    if(!feed->config.useExtndedDesc) {
        return BERR_TRACE(BERR_NOT_SUPPORTED);
    }

    BPVRlib_Feed_Priv_Update(feed);
    *nconsumed = BPVRlib_Feed_Priv_AddOffsetEntries(feed, (void*)entries, count, true, true);
    return BERR_SUCCESS;
}

BERR_Code 
BPVRlib_Feed_AddEntries(BPVRlib_Feed_Handle feed, BMEM_Heap_Handle heap, const BPVRlib_Feed_Entry *entries /* pointer to array of entries */ , size_t nentries /* number of entries in the array */ , size_t *nconsumed)
{
    unsigned off;
    BPVRlib_Feed_OffsetEntry *offset_entries = feed->offset_entries;
    BERR_Code rc = BERR_SUCCESS;

    BDBG_MSG_TRACE(("BPVRlib_Feed_AddEntries>: %#lx entries %#lx:%u", (unsigned long)feed, (unsigned long)entries, nentries));
    BDBG_OBJECT_ASSERT(feed, BPVRlib_Feed);
    BDBG_ASSERT(heap);
    BDBG_ASSERT(entries);
    BDBG_ASSERT(nconsumed);

    *nconsumed=0;
    BPVRlib_Feed_Priv_Update(feed);

    for(off=0;;) {
        unsigned i;
        unsigned added;
        for(i=0;i<sizeof(feed->offset_entries)/sizeof(*feed->offset_entries) && off<nentries;off++,i++) {
            offset_entries[i].len = entries[off].len;
            rc = BMEM_Heap_ConvertAddressToOffset(heap, entries[off].addr, &offset_entries[i].offset);
            if(rc!=BERR_SUCCESS) {
                rc = BERR_TRACE(rc);
                goto err_offset;
            }
        }
        if(i==0) {
            break;
        }
        added = BPVRlib_Feed_Priv_AddOffsetEntries(feed, offset_entries, i, off==nentries, false);
        *nconsumed+=added;
        if(added!=i) {
            break;
        }
    }
err_offset:
    BDBG_MSG_TRACE(("BPVRlib_Feed_AddEntries<: %#lx entries %#lx:%u -> %u", (unsigned long)feed, (unsigned long)entries, nentries, *nconsumed));
    return rc;
}

BERR_Code 
BPVRlib_Feed_GetCompleted(BPVRlib_Feed_Handle feed, size_t *ncompleted)
{
    BDBG_MSG_TRACE(("BPVRlib_Feed_GetCompleted>: %#lx", (unsigned long)feed));

    BDBG_OBJECT_ASSERT(feed, BPVRlib_Feed);
    BDBG_ASSERT(ncompleted);
    BPVRlib_Feed_Priv_Update(feed);
    *ncompleted = feed->ncompleted;
    feed->ncompleted = 0;

    BDBG_MSG_TRACE(("BPVRlib_Feed_GetCompleted<: %#lx -> %u", (unsigned long)feed, *ncompleted));
    return BERR_SUCCESS;
}

void 
BPVRlib_Feed_GetStatus(BPVRlib_Feed_Handle feed, BPVRlib_Feed_Status *status)
{
    size_t freeDesc;
    BDBG_OBJECT_ASSERT(feed, BPVRlib_Feed);
    BDBG_ASSERT(status);

    BPVRlib_Feed_Priv_Update(feed);
    freeDesc =  BFIFO_WRITE_LEFT(&feed->descFifo);
    freeDesc /= feed->descScale;
#if defined(HUMAX_PLATFORM_BASE) && defined(CONFIG_BRCM_PATCH_TRANSCODING_32BIT_TIMESTAMP) /*Timestamp*/
#if !BXPT_HAS_MULTICHANNEL_PLAYBACK
    if(feed->false_finished) {
        if(freeDesc>0) {
            BDBG_MSG(("BPVRlib_Feed_GetStatus: %#lx correcting number of avaliable descriptors %u", (unsigned long)feed, freeDesc));
            freeDesc --;
        }
    }
#endif
#else
    if(feed->false_finished) {
        if(freeDesc>0) {
            BDBG_MSG(("BPVRlib_Feed_GetStatus: %#lx correcting number of avaliable descriptors %u", (unsigned long)feed, freeDesc));
            freeDesc --;
        }
    }
#endif
    if(freeDesc>0) {
        freeDesc --;
    }
    status->freeDesc = freeDesc;
    BDBG_MSG_TRACE(("BPVRlib_Feed_GetStatus: %#lx freeDesc:%u", (unsigned long)feed, freeDesc));
    return;
}

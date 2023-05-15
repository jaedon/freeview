/***************************************************************************
 *     Copyright (c) 2009-2011, Broadcom Corporation
 *     All Rights Reserved
 *     Confidential Property of Broadcom Corporation
 *
 *  THIS SOFTWARE MAY ONLY BE USED SUBJECT TO AN EXECUTED SOFTWARE LICENSE
 *  AGREEMENT  BETWEEN THE USER AND BROADCOM.  YOU HAVE NO RIGHT TO USE OR
 *  EXPLOIT THIS MATERIAL EXCEPT SUBJECT TO THE TERMS OF SUCH AN AGREEMENT.
 *
 * $brcm_Workfile: bgrc_packet.h $
 * $brcm_Revision: Hydra_Software_Devel/29 $
 * $brcm_Date: 11/29/11 4:22p $
 *
 * Module Description: GRC Packet header
 *
 * Revision History:
 *
 * $brcm_Log: /magnum/portinginterface/grc/7405/bgrc_packet.h $
 * 
 * Hydra_Software_Devel/29   11/29/11 4:22p nissen
 * SW7425-1795: Changed to BMEM_Handle.
 * 
 * Hydra_Software_Devel/28   11/22/11 6:26p nissen
 * SW7425-1795: Added heap field to BGRC_PacketContext_CreateSettings
 * structure for packet memory allocations.
 * 
 * Hydra_Software_Devel/27   12/6/10 12:49p nissen
 * SW7420-1138: Added support for validating memory bounds used by blits.
 * 
 * Hydra_Software_Devel/26   11/11/10 5:36p nissen
 * SW7420-1200: Added support for storing packets before submitting,
 * through a field in the context's create settings structure.
 * 
 * Hydra_Software_Devel/25   9/3/10 4:21p nissen
 * SW7405-4810: Added BGRC_Packet_GetStatus function.
 * 
 * Hydra_Software_Devel/24   7/21/10 4:30p nissen
 * SW7405-3671: Added support for user supplying private data ptr during
 * context creation which gets passed back when getting status.
 * 
 * Hydra_Software_Devel/23   7/15/10 9:06p nissen
 * SW7405-3671: Added more interrupt support.
 * 
 * Hydra_Software_Devel/22   7/12/10 2:36p nissen
 * SW7405-3671: Updated api functions.
 * 
 * Hydra_Software_Devel/21   6/30/10 9:29p nissen
 * SW7405-3671: Updated color matrix and filter conversion functions.
 * 
 * Hydra_Software_Devel/20   6/30/10 7:53p nissen
 * SW7405-3671: Updated pixel format conversion function and removed blend
 * conversion function.
 * 
 * Hydra_Software_Devel/19   6/24/10 2:18p nissen
 * SW7405-3671: Added source feeder destripe packet function.
 * 
 * Hydra_Software_Devel/18   6/21/10 2:25p nissen
 * SW7405-3671: Fixed packet settings.
 * 
 * Hydra_Software_Devel/17   6/21/10 12:41p nissen
 * SW7405-3671: Added context support. Switched to M2MC packets.
 * 
 * Hydra_Software_Devel/16   3/19/10 2:33p nissen
 * SW7405-3671: Fixed alpha premultiply.
 * 
 * Hydra_Software_Devel/15   3/11/10 1:14p nissen
 * SW7405-3671: Added support for alpha premultiply.
 * 
 * Hydra_Software_Devel/14   3/10/10 2:50p nissen
 * SW7405-3671: Added offset buffer pointers to batch packet blits.
 * 
 * Hydra_Software_Devel/13   2/10/10 2:16p nissen
 * SW7405-3671: Fixed palette to palette blits.
 * 
 * Hydra_Software_Devel/12   2/9/10 4:35p nissen
 * SW7405-3671: Updated batch blits.
 * 
 * Hydra_Software_Devel/11   2/8/10 6:00p nissen
 * SW7405-3671: Updated batching to submit as many blits as possible.
 * 
 * Hydra_Software_Devel/10   2/5/10 8:20p nissen
 * SW7405-3671: Fixed palette to palette blits.
 * 
 * Hydra_Software_Devel/9   2/5/10 12:08p nissen
 * SW7405-3671: Fixed problem with init packet.
 * 
 * Hydra_Software_Devel/8   2/5/10 10:50a nissen
 * SW7405-3671: Moved packet function.
 * 
 * Hydra_Software_Devel/7   2/3/10 2:01p nissen
 * SW7405-3671: Added batch packets.
 * 
 * Hydra_Software_Devel/6   1/29/10 7:20p nissen
 * SW7405-3671: Added support for using packets in any order.
 * 
 * Hydra_Software_Devel/5   1/19/10 4:45p nissen
 * SW7405-3671: Added support for different M2MC cores.
 * 
 * Hydra_Software_Devel/4   1/19/10 12:31p nissen
 * SW7405-3671: Added support for sync'ing and 420 destriping.
 * 
 * Hydra_Software_Devel/3   1/8/10 2:26p nissen
 * SW7405-3671: Removed buffer size parameters when opening packet module.
 * 
 * Hydra_Software_Devel/2   12/31/09 10:17p nissen
 * SW7405-3671: Added palette support and software striping while scaling
 * support.
 * 
 * Hydra_Software_Devel/1   12/24/09 1:10a nissen
 * SW7405-3671: Packet blits
 * 
 ***************************************************************************/

#ifndef BGRC_PACKET_H__
#define BGRC_PACKET_H__

#ifdef __cplusplus
extern "C" {
#endif

#include "bm2mc_packet.h"
#include "berr_ids.h"

/***************************************************************************/
typedef struct BGRC_P_PacketContext *BGRC_PacketContext_Handle;

/***************************************************************************/
#define BGRC_PACKET_MSG_PACKETS_INCOMPLETE   BERR_MAKE_CODE(BERR_GRC_ID, 0x0100)
#define BGRC_PACKET_MSG_BLITS_COMPLETE       BERR_MAKE_CODE(BERR_GRC_ID, 0x0101)

/***************************************************************************/
typedef struct
{
	BMEM_Handle packet_buffer_heap;      /* heap for packet buffer allocations */
	size_t packet_buffer_size;           /* size of packet ring buffer to allocate */
	size_t packet_buffer_store;          /* size of packet buffer to store before submitting */
	void *private_data;                  /* ptr passed back to the user when getting status */
	struct {
		uint32_t offset;                 /* bounded memory offset (0=no bounds check) */
		size_t size;                     /* bounded memory size  (0=no bounds check)*/
	} memory_bounds;                     /* verify blits do not violate memory bounds */
}
BGRC_PacketContext_CreateSettings;

/***************************************************************************/
typedef struct
{
	BGRC_PacketContext_Handle hContext;
	void *private_data;               /* ptr to user's private data */
	size_t packet_buffer_available;   /* size of available packet memory */
	bool sync;                        /* indicates blit sync */
}
BGRC_Packet_ContextStatus;

/***************************************************************************/
typedef struct
{
	bool m2mc_busy;                   /* indicates m2mc is busy */
}
BGRC_Packet_Status;

/***************************************************************************
 PACKET API - MAIN FUNCTIONS
 ***************************************************************************/
/*************************************************************************** 
Set one callback for all contexts, which overrides any context callbacks.
Either a single callback will be called for all interrupts, or context's
can get their own callbacks using BGRC_Packet_SetContextCallbacks.
 ***************************************************************************/
BERR_Code BGRC_Packet_SetCallback(
	BGRC_Handle hGrc,
	BGRC_Callback callback_isr,   /* general callback for all contexts */
	void *callback_data           /* ptr to user's data */
);

/***************************************************************************/
BERR_Code BGRC_Packet_GetDefaultCreateContextSettings(
	BGRC_Handle hGrc,
	BGRC_PacketContext_CreateSettings *pSettings
);

/***************************************************************************/
BERR_Code BGRC_Packet_CreateContext(
	BGRC_Handle hGrc,
	BGRC_PacketContext_Handle *phContext,
	BGRC_PacketContext_CreateSettings *pSettings
);

/***************************************************************************/
BERR_Code BGRC_Packet_DestroyContext(
	BGRC_Handle hGrc,
	BGRC_PacketContext_Handle hContext
);

/*************************************************************************** 
Set Advance and Sync callbacks per context. These callbacks will not be used 
if setting a general callback through BGRC_Packet_SetCallback.
 ***************************************************************************/
BERR_Code BGRC_Packet_SetContextCallbacks(
	BGRC_Handle hGrc,
	BGRC_PacketContext_Handle hContext,
	BGRC_Callback advance_callback_isr,   /* callback indicating packet procssing can be */
	void *advance_callback_data,          /* advanced because device fifo memory is available */
	BGRC_Callback sync_callback_isr,      /* callback indicating blits are complete */
	void *sync_callback_data              /* up to the point of the sync */
);

/***************************************************************************
Gets packet memory from the packet ring buffer. If the specified minimum
size is not available, then the ring buffer will wrap if possible and 
return memory from the front of the buffer, otherwise a size of zero will
be returned. The returned size will be the maximium amount available.
 ***************************************************************************/
BERR_Code BGRC_Packet_GetPacketMemory(
	BGRC_Handle hGrc,
	BGRC_PacketContext_Handle hContext, /* cannot be null */
	void **buffer,                      /* [out] ptr to packet buffer ptr */
	size_t *size_out,                   /* [out] ptr to size of buffer */
	size_t size_in                      /* minimum size requested (4-bytes or greater) */
);

/***************************************************************************
Submit packets for processing to a context. If all packets cannot be 
processed, then _MSG_PACKETS_INCOMPLETE is returned, and the callback is 
called when packet processing becomes possible again. If there is no 
callback specified, remaining packets still need to be advanced.
 ***************************************************************************/
BERR_Code BGRC_Packet_SubmitPackets(
	BGRC_Handle hGrc,
	BGRC_PacketContext_Handle hContext, /* cannot be null */
	size_t size                         /* size of packet buffer submitted */
);

/***************************************************************************
Advances the context's uncompleted packet processing. If all the packets 
cannot be finished, then _MSG_PACKETS_INCOMPLETE is returned, and the 
callback is called when packet processing becomes possible again. If there 
is no callback specified, then all packets will have finished processing 
upon return.
 ***************************************************************************/
BERR_Code BGRC_Packet_AdvancePackets(
	BGRC_Handle hGrc,
	BGRC_PacketContext_Handle hContext /* set to null to advance all contexts */
);

/***************************************************************************
Synchronizes the context's blits. If there are any remaining packets that 
still need to be Advanced, then _MSG_PACKETS_INCOMPLETE is returned. If all 
the context's blits are already complete, then _MSG_BLITS_COMPLETE is 
returned, otherwise the callback will be called when the blits are complete. 
If there is no callback specified, then all outstanding blits will be 
complete upon return. Sync can be called once per context, and cannot be 
called again until the callback is called. 
 ***************************************************************************/
BERR_Code BGRC_Packet_SyncPackets(
	BGRC_Handle hGrc,
	BGRC_PacketContext_Handle hContext /* cannot be null */
);

/***************************************************************************
Returns the how much packet memory is available for contexts with packets 
that need advancing, and the blit sync status of contexts that have synced 
since the last call to this function.
 ***************************************************************************/
BERR_Code BGRC_Packet_GetContextStatus(
	BGRC_Handle hGrc,
	BGRC_Packet_ContextStatus *status_array, /* [out] context status array */
	size_t *size_out,                        /* [out] size of array*/
	size_t size_in                           /* size of array */
);

/***************************************************************************
Returns the M2MC device status.
 ***************************************************************************/
BERR_Code BGRC_Packet_GetStatus(
	BGRC_Handle hGrc,
	BGRC_Packet_Status *status              /* [out] status */
);

/***************************************************************************
 PACKET API - STATE DATA CONVERSION
 ***************************************************************************/
BERR_Code BGRC_Packet_ConvertPixelFormat(
	BM2MC_PACKET_PixelFormat *format,       /* [out] packet pixel format */
	BPXL_Format pxl_format                  /* BPXL pixel format */
);

/***************************************************************************/
BERR_Code BGRC_Packet_ConvertFilter( 
	BM2MC_PACKET_FilterCoeffs *coeffs,      /* [out] packet filter coeffs */
	BGRC_FilterCoeffs filter,               /* BGRC filter enum */
	size_t src_size,                        /* source width or height */
	size_t out_size                         /* output width or height */
);

/***************************************************************************/
BERR_Code BGRC_Packet_ConvertColorMatrix( 
	BM2MC_PACKET_ColorMatrix *matrix_out,   /* [out] packet colormatrix */
	const int32_t *matrix_in,               /* BGRC color matrix */
	size_t shift                            /* BGRC matrix element shift */
);

/***************************************************************************/

#ifdef __cplusplus
}
#endif

#endif /* #ifndef BGRC_PACKET_H__ */

/* end of file */

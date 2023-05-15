/***************************************************************************
 *     Copyright (c) 2009-2012, Broadcom Corporation
 *     All Rights Reserved
 *     Confidential Property of Broadcom Corporation
 *
 *  THIS SOFTWARE MAY ONLY BE USED SUBJECT TO AN EXECUTED SOFTWARE LICENSE
 *  AGREEMENT  BETWEEN THE USER AND BROADCOM.  YOU HAVE NO RIGHT TO USE OR
 *  EXPLOIT THIS MATERIAL EXCEPT SUBJECT TO THE TERMS OF SUCH AN AGREEMENT.
 *
 * $brcm_Workfile: bgrc_packet_priv.h $
 * $brcm_Revision: Hydra_Software_Devel/47 $
 * $brcm_Date: 8/21/12 12:15p $
 *
 * Module Description: GRC Packet private header
 *
 * Revision History:
 *
 * $brcm_Log: /magnum/portinginterface/grc/7435/bgrc_packet_priv.h $
 * 
 * Hydra_Software_Devel/47   8/21/12 12:15p nissen
 * SW7425-3625: Switched to using cached memory access.
 * 
 * Hydra_Software_Devel/46   6/28/12 2:45p nissen
 * SW7425-2480: Added filter reset to sync blit.
 * 
 * Hydra_Software_Devel/45   6/27/12 4:05p nissen
 * SW7425-2480: Added alpha premultiply reset to sync blit.
 * 
 * Hydra_Software_Devel/44   6/26/12 6:30p nissen
 * SW7425-2480: Added dest colorkey reset to sync blit.
 * 
 * Hydra_Software_Devel/43   3/16/12 4:16p nissen
 * SW7425-2480: Added rop reset to sync blit.
 * 
 * Hydra_Software_Devel/42   1/6/12 1:46p nissen
 * SW7435-13 : Fixed support for 7435 A0.
 * 
 * Hydra_Software_Devel/41   12/20/11 5:40p nissen
 * SW7231-540: Fixed sync and scaling problems with mirroring.
 * 
 * Hydra_Software_Devel/40   12/15/11 6:15p nissen
 * SW7435-13 : Added support for 7435 A0.
 * 
 * Hydra_Software_Devel/39   12/14/11 4:48p nissen
 * SW7435-13 : Added support for 7435 A0.
 * 
 * Hydra_Software_Devel/38   12/13/11 4:44p nissen
 * SW7231-535: Fixed surface rectangle verification after sync blit.
 * 
 * Hydra_Software_Devel/37   11/4/11 8:43p nissen
 * SWBLURAY-27238: Added include file for 2nd M2MC for 7640 B0.
 * 
 * Hydra_Software_Devel/36   11/3/11 4:50p nissen
 * SWBLURAY-27238: Added support for 2nd M2MC for 7640 B0.
 * 
 * Hydra_Software_Devel/35   10/6/11 12:45p nissen
 * SW7420-2082: Fixed debug define.
 * 
 * Hydra_Software_Devel/34   10/6/11 12:08p nissen
 * SW7420-2082: Added debug prints for FixedScale
 * 
 * Hydra_Software_Devel/33   10/5/11 5:39p nissen
 * SW7425-248: Updated use of BDBG_OBJECTs.
 * 
 * Hydra_Software_Devel/32   10/5/11 12:47p nissen
 * SW7425-248: Removed BDBG_OBJECT support for the 7038.
 * 
 * Hydra_Software_Devel/31   9/17/11 7:06p nissen
 * SW7425-248: Added BDBG_OBJECT_ID_DECLARE.
 * 
 * Hydra_Software_Devel/30   9/17/11 6:38p nissen
 * SW7425-248: Added BDBG_OBJECT_ASSERT.
 * 
 * Hydra_Software_Devel/29   9/13/11 4:42p nissen
 * SW7420-1575: Fixed surface rectangle verification, and added support
 * for surface format verification.
 * 
 * Hydra_Software_Devel/28   7/27/11 5:52p nissen
 * SW7405-3671: Added support for fixed scaling with 420 destriping.
 * 
 * Hydra_Software_Devel/27   6/23/11 10:13a nissen
 * SW7420-1893: Prevented sync blit frin over writting packet memory.
 * 
 * Hydra_Software_Devel/26   5/27/11 6:27p nissen
 * SW7346-149: Added packet submitted field on context.
 * 
 * Hydra_Software_Devel/25   5/26/11 6:09p nissen
 * SW7335-916: Fixed linear destripe for SourceControl packet.
 * 
 * Hydra_Software_Devel/24   5/20/11 5:56p nissen
 * SW7335-1213: Fixing sync blit.
 * 
 * Hydra_Software_Devel/23   3/7/11 3:57p nissen
 * SW7420-1575: Added support for surface rectangle verification.
 * 
 * Hydra_Software_Devel/22   3/7/11 1:33p nissen
 * SW7038-3500: Fixed header size.
 * 
 * Hydra_Software_Devel/21   3/2/11 7:49p nissen
 * SW7038-3500: Fixed build errors.
 * 
 * Hydra_Software_Devel/20   2/25/11 5:02p nissen
 * SW7038-3500: Added initial support for 7038.
 * 
 * Hydra_Software_Devel/19   12/6/10 12:36p nissen
 * SW7420-1138: Added support for validating memory bounds used by blits.
 * 
 * Hydra_Software_Devel/18   11/30/10 3:34p nissen
 * SW7335-916: Fixed macro warnings.
 * 
 * Hydra_Software_Devel/17   11/25/10 2:32p nissen
 * SW7335-916: Added more debug print parameters to FixedScale packet.
 * 
 * Hydra_Software_Devel/16   11/15/10 3:12p nissen
 * SW7420-1150: Fixed palette to palette blits.
 * 
 * Hydra_Software_Devel/15   11/11/10 1:38p nissen
 * SW7335-916: Added support for FixedScale packet.
 * 
 * Hydra_Software_Devel/14   10/27/10 12:16p nissen
 * SW7420-1150: Fixed support for palette to palatte blits.
 * 
 * Hydra_Software_Devel/13   9/20/10 1:55p nissen
 * SW7405-3671: Added support for Source, Destination, and Output Control
 * packets.
 * 
 * Hydra_Software_Devel/12   9/15/10 5:34p nissen
 * SW7420-1081: Switched to using BMEM_Alloc with cached memory when
 * allocating user packets.
 * 
 * Hydra_Software_Devel/11   8/18/10 3:21p nissen
 * SW7405-3671: Checking if user overwrote submited packet memory.
 * 
 * Hydra_Software_Devel/10   8/9/10 10:35p nissen
 * SW7405-3671: Added field in context structure to support forced SW
 * striping.
 * 
 * Hydra_Software_Devel/9   8/2/10 2:57p nissen
 * SW7405-3671: Removed reset state packet from sync blit.
 * 
 * Hydra_Software_Devel/8   7/31/10 5:12p nissen
 * SW7405-3671: Fixed packet size macros.
 * 
 * Hydra_Software_Devel/7   7/28/10 4:01p nissen
 * SW7405-3671: Added group header field to context.
 * 
 * Hydra_Software_Devel/6   7/26/10 2:43p nissen
 * SW7405-3671: Added packet size define.
 * 
 * Hydra_Software_Devel/5   7/18/10 4:07p nissen
 * SW7405-3671: Fixed reset state and sync.
 * 
 * Hydra_Software_Devel/4   7/15/10 9:14p nissen
 * SW7405-3671: Added more interrupt support.
 * 
 * Hydra_Software_Devel/3   7/12/10 2:40p nissen
 * SW7405-3671: Updated context structure.
 * 
 * Hydra_Software_Devel/2   6/21/10 2:29p nissen
 * SW7405-3671: Fixed packet settings.
 * 
 * Hydra_Software_Devel/1   6/21/10 11:05a nissen
 * SW7405-3671: Initial version.
 * 
 ***************************************************************************/

#ifndef BGRC_PACKET_PRIV_H__
#define BGRC_PACKET_PRIV_H__

#include "bdbg.h"
#include "bchp.h"
#include "bmem.h"
#include "bsur.h"
#include "bint.h"
#include "bkni.h"
#include "breg_mem.h"
#include "bgrc_packet.h"
#include "bgrc_errors.h"
#include "bchp_common.h"
#include "blst_list.h"

#if (BCHP_CHIP==7435)
#include "bchp_m2mc.h"
#include "bchp_m2mc1.h"
#else
#include "bchp_m2mc.h"
#endif

#if ((BCHP_CHIP==7640) && (BCHP_VER >= BCHP_VER_B0))
#include "bchp_m2mc_1.h"
#endif

#ifdef __cplusplus
extern "C" {
#endif

/***************************************************************************/
#ifndef BDBG_OBJECT_ID
#define BDBG_OBJECT(P0)
#define BDBG_OBJECT_SET(P0,P1)
#define BDBG_OBJECT_ASSERT(P0,P1)
#define BDBG_OBJECT_DESTROY(P0,P1)
#define BDBG_OBJECT_ID(P0)         extern const char bgrc_id__##P0
#define BDBG_OBJECT_ID_DECLARE(P0) extern const char bgrc_id_declare_##P0
#endif

/***************************************************************************/
BDBG_OBJECT_ID_DECLARE(BGRC_PacketContext);

/***************************************************************************/
/*#define BGRC_PACKET_P_DEBUG*/
/*#define BGRC_PACKET_P_VERIFY_SURFACE_RECTANGLE*/

/***************************************************************************/
#define BGRC_PACKET_P_BLIT_GROUP_SIZE_MAX (1024*16)

/***************************************************************************/
#define BGRC_PACKET_P_MEMORY_ALIGN_BITS   5
#define BGRC_PACKET_P_MEMORY_ALIGN_MASK   ((1 << BGRC_PACKET_P_MEMORY_ALIGN_BITS) - 1)

/***************************************************************************/
#ifdef BCHP_M2MC0_REVISION
#define BGRC_M2MC(val)   BCHP_M2MC_##val
#else
#define BGRC_M2MC(val)   BCHP_M2MC_##val
#endif

/***************************************************************************/
#define BGRC_PACKET_P_REGISTER_COUNT \
	((BGRC_M2MC(SRC_CLUT_ENTRY_i_ARRAY_BASE) - BGRC_M2MC(SRC_FEEDER_ENABLE)) / 4 + 1)
#define BGRC_PACKET_P_GET_STORED_REG_INDEX( reg ) \
	((BGRC_M2MC(reg) - BGRC_M2MC(SRC_FEEDER_ENABLE)) / sizeof (uint32_t))

/***************************************************************************/
#ifdef BGRC_PACKET_P_DEBUG
#define BGRC_PACKET_P_DEBUG_PRINT( value ) BKNI_Printf( value )
#define BGRC_PACKET_P_DEBUG_PRINT_VALUE( value ) BKNI_Printf( "%08x ", value )
#else
#define BGRC_PACKET_P_DEBUG_PRINT( value )
#define BGRC_PACKET_P_DEBUG_PRINT_VALUE( value )
#endif

#ifdef BGRC_PACKET_P_DEBUG
#define BGRC_PACKET_P_STORE_REG( reg, value ) { BKNI_Printf("%08x ", (value)); hContext->stored_registers[BGRC_PACKET_P_GET_STORED_REG_INDEX(reg)] = (value); }
#else
#define BGRC_PACKET_P_STORE_REG( reg, value ) hContext->stored_registers[BGRC_PACKET_P_GET_STORED_REG_INDEX(reg)] = (value)
#endif

#define BGRC_PACKET_P_STORE_REG_MASK( reg, value, mask ) BGRC_PACKET_P_STORE_REG( reg, (BGRC_PACKET_P_GET_REG(reg) & (mask)) | (value) )

#define BGRC_PACKET_P_STORE_REG_FIELD( reg, field, value ) \
{ \
	hContext->stored_registers[BGRC_PACKET_P_GET_STORED_REG_INDEX(reg)] &= ~BGRC_M2MC(reg##_##field##_MASK); \
	hContext->stored_registers[BGRC_PACKET_P_GET_STORED_REG_INDEX(reg)] |= BGRC_M2MC(reg##_##field##_##value) << BGRC_M2MC(reg##_##field##_SHIFT); \
}

#define BGRC_PACKET_P_STORE_REG_FIELD_DATA( reg, field, value ) \
{ \
	hContext->stored_registers[BGRC_PACKET_P_GET_STORED_REG_INDEX(reg)] &= ~BGRC_M2MC(reg##_##field##_MASK); \
	hContext->stored_registers[BGRC_PACKET_P_GET_STORED_REG_INDEX(reg)] |= (value) << BGRC_M2MC(reg##_##field##_SHIFT); \
}

#define BGRC_PACKET_P_STORE_REG_FIELD_COMP( reg, field, value_false, value_true, comp ) \
{ \
	if( comp ) \
		BGRC_PACKET_P_STORE_REG_FIELD(reg, field, value_true) \
	else \
		BGRC_PACKET_P_STORE_REG_FIELD(reg, field, value_false) \
}

#if defined(BCHP_M2MC_BLIT_SRC_TOP_LEFT_1) || defined(BCHP_M2MC0_BLIT_SRC_TOP_LEFT_1)
#define BGRC_PACKET_P_STORE_RECT_REGS( src_pos0, src_size0, src_pos1, src_size1, dst_pos, dst_size, out_pos, out_size ) \
{ \
	BGRC_PACKET_P_STORE_REG( BLIT_SRC_TOP_LEFT, src_pos0 ); \
	BGRC_PACKET_P_STORE_REG( BLIT_SRC_SIZE, src_size0 ); \
	BGRC_PACKET_P_STORE_REG( BLIT_SRC_TOP_LEFT_1, src_pos1 ); \
	BGRC_PACKET_P_STORE_REG( BLIT_SRC_SIZE_1, src_size1 ); \
	BGRC_PACKET_P_STORE_REG( BLIT_DEST_TOP_LEFT, dst_pos ); \
	BGRC_PACKET_P_STORE_REG( BLIT_DEST_SIZE, dst_size ); \
	BGRC_PACKET_P_STORE_REG( BLIT_OUTPUT_TOP_LEFT, out_pos ); \
	BGRC_PACKET_P_STORE_REG( BLIT_OUTPUT_SIZE, out_size ); \
}
#else
#define BGRC_PACKET_P_STORE_RECT_REGS( src_pos0, src_size0, src_pos1, src_size1, dst_pos, dst_size, out_pos, out_size ) \
{ \
	BGRC_PACKET_P_STORE_REG( BLIT_SRC_TOP_LEFT, src_pos0 ); \
	BGRC_PACKET_P_STORE_REG( BLIT_SRC_SIZE, src_size0 ); \
	BGRC_PACKET_P_STORE_REG( BLIT_DEST_TOP_LEFT, dst_pos ); \
	BGRC_PACKET_P_STORE_REG( BLIT_DEST_SIZE, dst_size ); \
	BGRC_PACKET_P_STORE_REG( BLIT_OUTPUT_TOP_LEFT, out_pos ); \
	BGRC_PACKET_P_STORE_REG( BLIT_OUTPUT_SIZE, out_size ); \
}
#endif

#if defined(BCHP_M2MC_HORIZ_FIR_1_COEFF_PHASE0_01) || defined(BCHP_M2MC0_HORIZ_FIR_1_COEFF_PHASE0_01)
#define BGRC_PACKET_P_STORE_SCALE_REGS( reg, num, phase, step ) \
{ \
	BGRC_PACKET_P_STORE_REG( reg##_SCALER_##num##_INITIAL_PHASE, (phase) & BGRC_M2MC(reg##_SCALER_##num##_INITIAL_PHASE_PHASE_MASK) ); \
	BGRC_PACKET_P_STORE_REG( reg##_SCALER_##num##_STEP, (step) & BGRC_M2MC(reg##_SCALER_##num##_STEP_STEP_MASK) ); \
}
#else
#define BGRC_PACKET_P_STORE_SCALE_REGS( reg, num, phase, step ) \
{ \
	BGRC_PACKET_P_STORE_REG( reg##_SCALER_INITIAL_PHASE, (phase) & BGRC_M2MC(reg##_SCALER_INITIAL_PHASE_PHASE_MASK) ); \
	BGRC_PACKET_P_STORE_REG( reg##_SCALER_STEP, (step) & BGRC_M2MC(reg##_SCALER_STEP_STEP_MASK) ); \
}
#endif

/***************************************************************************/
#ifdef BGRC_PACKET_P_DEBUG
#define BGRC_PACKET_P_WRITE_REG( dst, index ) { BKNI_Printf("%08x ", hContext->stored_registers[index]); dst = hContext->stored_registers[index]; }
#define BGRC_PACKET_P_WRITE_DATA( dst, data ) { BKNI_Printf("%08x ", (data)); dst = data; }
#else
#define BGRC_PACKET_P_WRITE_REG( dst, index ) dst = hContext->stored_registers[index]
#define BGRC_PACKET_P_WRITE_DATA( dst, data ) dst = data
#endif

#define BGRC_PACKET_P_GET_REG_FROM_INDEX( index )       hContext->stored_registers[index]
#define BGRC_PACKET_P_GET_REG( reg )                    BGRC_PACKET_P_GET_REG_FROM_INDEX(BGRC_PACKET_P_GET_STORED_REG_INDEX(reg))

#ifdef BCHP_M2MC0_REVISION
#define BGRC_PACKET_P_GET_REG_FIELD( reg, field )       (BGRC_PACKET_P_GET_REG(reg) & BCHP_M2MC0_##reg##_##field##_MASK)
#define BGRC_PACKET_P_GET_REG_FIELD_SHIFT( reg, field ) ((BGRC_PACKET_P_GET_REG(reg) & BCHP_M2MC0_##reg##_##field##_MASK) >> BCHP_M2MC0_##reg##_##field##_SHIFT)
#else
#define BGRC_PACKET_P_GET_REG_FIELD( reg, field )       (BGRC_PACKET_P_GET_REG(reg) & BCHP_M2MC_##reg##_##field##_MASK)
#define BGRC_PACKET_P_GET_REG_FIELD_SHIFT( reg, field ) ((BGRC_PACKET_P_GET_REG(reg) & BCHP_M2MC_##reg##_##field##_MASK) >> BCHP_M2MC_##reg##_##field##_SHIFT)
#endif

/***************************************************************************/
#ifdef BCHP_M2MC1_REVISION
#define BGRC_P_WriteReg32( handle, reg, data ) BREG_Write32( handle, BCHP_M2MC_##reg + hGrc->ulDeviceNum * (BCHP_M2MC1_REVISION - BCHP_M2MC_REVISION), data )
#define BGRC_P_ReadReg32( handle, reg )        BREG_Read32( handle, BCHP_M2MC_##reg + hGrc->ulDeviceNum * (BCHP_M2MC1_REVISION - BCHP_M2MC_REVISION) )
#else
#ifdef BCHP_M2MC_1_REVISION
#define BGRC_P_WriteReg32( handle, reg, data ) BREG_Write32( handle, BCHP_M2MC_##reg + hGrc->ulDeviceNum * (BCHP_M2MC_1_REVISION - BCHP_M2MC_REVISION), data )
#define BGRC_P_ReadReg32( handle, reg )        BREG_Read32( handle, BCHP_M2MC_##reg + hGrc->ulDeviceNum * (BCHP_M2MC_1_REVISION - BCHP_M2MC_REVISION) )
#else
#define BGRC_P_WriteReg32( handle, reg, data ) BREG_Write32( handle, BCHP_M2MC_##reg, data )
#define BGRC_P_ReadReg32( handle, reg )        BREG_Read32( handle, BCHP_M2MC_##reg )
#endif
#endif

/***************************************************************************/
#define BGRC_PACKET_P_SRC_PXL_422_ALIGN(x, w) \
  if (BGRC_PACKET_P_GET_REG_FIELD(SRC_SURFACE_0_FORMAT_DEF_1, FORMAT_TYPE) == M2MC_FT_YCbCr422) \
  { (x) = ((x) + 1) & 0xFFFFFFFE; (w) = (w) & 0xFFFFFFFE; }
#define BGRC_PACKET_P_DST_PXL_422_ALIGN(x, w) \
  if (BGRC_PACKET_P_GET_REG_FIELD(DEST_SURFACE_FORMAT_DEF_1, FORMAT_TYPE) == M2MC_FT_YCbCr422) \
  { (x) = ((x) + 1) & 0xFFFFFFFE; (w) = (w) & 0xFFFFFFFE; }
#define BGRC_PACKET_P_OUT_PXL_422_ALIGN(x, w) \
  if (BGRC_PACKET_P_GET_REG_FIELD(OUTPUT_SURFACE_FORMAT_DEF_1, FORMAT_TYPE) == M2MC_FT_YCbCr422) \
  { (x) = ((x) + 1) & 0xFFFFFFFE; (w) = (w) & 0xFFFFFFFE; }
#define BGRC_PACKET_P_SRC_OUT_PXL_422_ALIGN(sx, sw, ox) \
  if ((BGRC_PACKET_P_GET_REG_FIELD(SRC_SURFACE_0_FORMAT_DEF_1, FORMAT_TYPE) == M2MC_FT_YCbCr422) || \
      (BGRC_PACKET_P_GET_REG_FIELD(OUTPUT_SURFACE_FORMAT_DEF_1, FORMAT_TYPE) == M2MC_FT_YCbCr422)) \
  { (sx) = ((sx) + 1) & 0xFFFFFFFE; (sw) = (sw) & 0xFFFFFFFE; (ox) = ((ox) + 1) & 0xFFFFFFFE;}
#define BGRC_PACKET_P_SRC_DST_OUT_PXL_422_ALIGN(sx, sw, dx, ox) \
  if ((BGRC_PACKET_P_GET_REG_FIELD(SRC_SURFACE_0_FORMAT_DEF_1, FORMAT_TYPE) == M2MC_FT_YCbCr422) || \
      (BGRC_PACKET_P_GET_REG_FIELD(DEST_SURFACE_FORMAT_DEF_1, FORMAT_TYPE) == M2MC_FT_YCbCr422) || \
      (BGRC_PACKET_P_GET_REG_FIELD(OUTPUT_SURFACE_FORMAT_DEF_1, FORMAT_TYPE) == M2MC_FT_YCbCr422)) \
  { (sx) = ((sx) + 1) & 0xFFFFFFFE; (sw) = (sw) & 0xFFFFFFFE; (dx) = ((dx) + 1) & 0xFFFFFFFE; (ox) = ((ox) + 1) & 0xFFFFFFFE;}

/***************************************************************************/
#define BGRC_PACKET_P_SYNC_REGISTER_COUNT \
	((BCHP_M2MC_ROP_OPERATION - BCHP_M2MC_SRC_FEEDER_ENABLE) / sizeof (uint32_t))

#define BGRC_PACKET_P_SYNC_PACKET_SIZE (\
	sizeof (BM2MC_PACKET_PacketSaveState) + \
	sizeof (BM2MC_PACKET_PacketSourceNone) + \
	sizeof (BM2MC_PACKET_PacketDestinationNone) + \
	sizeof (BM2MC_PACKET_PacketOutputFeeder) + \
	sizeof (BM2MC_PACKET_PacketBlend) + \
	sizeof (BM2MC_PACKET_PacketRop) + \
	sizeof (BM2MC_PACKET_PacketSourceColorkeyEnable) + \
	sizeof (BM2MC_PACKET_PacketDestinationColorkeyEnable) + \
	sizeof (BM2MC_PACKET_PacketFilterEnable) + \
	sizeof (BM2MC_PACKET_PacketFilterControl) + \
	sizeof (BM2MC_PACKET_PacketSourceColorMatrixEnable) + \
	sizeof (BM2MC_PACKET_PacketAlphaPremultiply) + \
	sizeof (BM2MC_PACKET_PacketAlphaDemultiply) + \
	sizeof (BM2MC_PACKET_PacketDestAlphaPremultiply) + \
	sizeof (BM2MC_PACKET_PacketMirror) + \
	sizeof (BM2MC_PACKET_PacketFillBlit) + \
	sizeof (BM2MC_PACKET_PacketRestoreState))

#define BGRC_PACKET_P_EXTRA_PACKET_SIZE \
	(BGRC_PACKET_P_SYNC_PACKET_SIZE + sizeof (BM2MC_PACKET_Header))

/***************************************************************************/
typedef struct
{
	uint32_t stripe_count;
	uint32_t stripe_num;
	uint32_t output_stripe_width;
	uint32_t input_stripe_width;
	uint32_t hor_chroma_phase;
	uint32_t hor_phase;
	uint32_t hor_step;
	uint32_t ver_chroma_phase;
	uint32_t ver_phase;
	uint32_t ver_step;
	uint16_t stripe_overlap;
}
BGRC_PACKET_P_Scaler;

/***************************************************************************/
typedef struct
{
	uint32_t hor_phase;
	uint32_t src_x;
	uint32_t dst_x;
	uint32_t out_x;
	uint32_t src_width;
	uint32_t out_width;
}
BGRC_PACKET_P_Stripe;

/***************************************************************************/
typedef enum
{
	BGRC_PACKET_P_SyncState_eCleared,
	BGRC_PACKET_P_SyncState_eRequested,
	BGRC_PACKET_P_SyncState_eQueuedInSw,
	BGRC_PACKET_P_SyncState_eQueuedInHw
}
BGRC_PACKET_P_SyncState;

/***************************************************************************/
BDBG_OBJECT_ID_DECLARE(BGRC_PacketContext);
typedef struct BGRC_P_PacketContext
{
	BDBG_OBJECT(BGRC_PacketContext)
	BGRC_Handle hGrc;
	BGRC_PacketContext_CreateSettings create_settings;
	BINT_CallbackHandle hCallback;

	BLST_D_ENTRY(BGRC_P_PacketContext) context_link;
	uint32_t ulId;

	BGRC_PACKET_P_Scaler scaler;
	BM2MC_PACKET_Header *scaler_header;

	uint32_t *buffer_alloc_addr;
	uint32_t *buffer_base_addr;
	uint32_t  buffer_total_size;
	uint32_t  buffer_alloc_offset;
	uint32_t  buffer_submit_offset;
	uint32_t  buffer_submit_size;

	uint32_t *last_blitheader_addr;
	uint32_t  last_blitheader;
	uint32_t  last_fifo_offset;
	uint32_t  last_size_out;
	uint32_t  m2mc_offset;
	uint32_t  group_header;

	uint32_t *sync_output_addr;
	uint32_t *sync_output_cache_addr;
	uint32_t  sync_output_offset;
	uint32_t  sync_pkt_offset;

	BGRC_PACKET_P_SyncState eSyncState;
	bool advance_interrupt;
	bool reset_state;
	bool packets_submitted; /* user has submitted a blit since open or last checkpoint */

	uint8_t last_blit_type;

	uint32_t stored_registers[BGRC_PACKET_P_REGISTER_COUNT];
	uint32_t saved_registers[BGRC_PACKET_P_REGISTER_COUNT];

	BM2MC_PACKET_PacketFixedScale fixed_scale;
	BM2MC_PACKET_PacketFixedScale saved_fixed_scale;
	BM2MC_PACKET_PacketDestripeFixedScale destripe_fixed_scale;
	BM2MC_PACKET_PacketDestripeFixedScale saved_destripe_fixed_scale;
	uint32_t src_format0;
	uint32_t dst_format0;
	uint32_t out_format0;
	uint32_t saved_out_format0;
	uint32_t saved_src_format0;
	bool linear_destripe;
	bool saved_linear_destripe;

	uint32_t SRC_surface_width;
	uint32_t SRC_surface_height;
	uint32_t SRC_surface_format;
	uint32_t DEST_surface_width;
	uint32_t DEST_surface_height;
	uint32_t DEST_surface_format;
	uint32_t OUTPUT_surface_width;
	uint32_t OUTPUT_surface_height;
	uint32_t OUTPUT_surface_format;

	uint32_t saved_SRC_surface_width;
	uint32_t saved_SRC_surface_height;
	uint32_t saved_SRC_surface_format;
	uint32_t saved_DEST_surface_width;
	uint32_t saved_DEST_surface_height;
	uint32_t saved_DEST_surface_format;
	uint32_t saved_OUTPUT_surface_width;
	uint32_t saved_OUTPUT_surface_height;
	uint32_t saved_OUTPUT_surface_format;
}
BGRC_P_PacketContext;

/***************************************************************************/
void BGRC_PACKET_P_PrintRegisters( BGRC_Handle hGrc );

void BGRC_PACKET_P_Isr( void *pvParam1, int iParam2 );
void BGRC_PACKET_P_ResetState( BGRC_PacketContext_Handle hContext );
void BGRC_PACKET_P_SubmitPackets( BGRC_Handle hGrc, BGRC_PacketContext_Handle hContext );
BERR_Code BGRC_PACKET_P_SyncBlit( BGRC_Handle hGrc, BGRC_PacketContext_Handle hContext );

BM2MC_PACKET_PixelFormat BGRC_PACKET_P_ConvertPixelFormat( BPXL_Format format );
void BGRC_PACKET_P_ConvertFilterCoeffs( BM2MC_PACKET_FilterCoeffs *coeffs, BGRC_FilterCoeffs filter, size_t src_size, size_t out_size );
void BGRC_PACKET_P_ConvertColorMatrix( BM2MC_PACKET_ColorMatrix *matrix_out, const int32_t *matrix_in, size_t shift );

/***************************************************************************/
#ifdef __cplusplus
}
#endif

#endif /* #ifndef BGRC_PACKET_PRIV_H__ */

/* end of file */

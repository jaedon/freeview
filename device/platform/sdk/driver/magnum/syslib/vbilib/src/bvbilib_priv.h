/***************************************************************************
 *     Copyright (c) 2003-2007, Broadcom Corporation
 *     All Rights Reserved
 *     Confidential Property of Broadcom Corporation
 *
 *  THIS SOFTWARE MAY ONLY BE USED SUBJECT TO AN EXECUTED SOFTWARE LICENSE
 *  AGREEMENT  BETWEEN THE USER AND BROADCOM.  YOU HAVE NO RIGHT TO USE OR
 *  EXPLOIT THIS MATERIAL EXCEPT SUBJECT TO THE TERMS OF SUCH AN AGREEMENT.
 *
 * $brcm_Workfile: bvbilib_priv.h $
 * $brcm_Revision: Hydra_Software_Devel/7 $
 * $brcm_Date: 10/26/07 10:51a $
 *
 * Module Description:
 *
 * Revision History:
 *
 * $brcm_Log: /magnum/syslib/vbilib/bvbilib_priv.h $
 * 
 * Hydra_Software_Devel/7   10/26/07 10:51a ptimariu
 * PR36451: closing bracket, __cplusplus
 * 
 * Hydra_Software_Devel/7   10/25/07 6:32p ptimariu
 * PR36451: closing bracket, __cplusplus
 * 
 * Hydra_Software_Devel/6   1/18/07 3:13p darnstein
 * PR26464: correctly handle teletext output to multiple VECs
 * 
 * Hydra_Software_Devel/5   10/21/05 7:46p darnstein
 * PR17732: Gemstar encoder can output a waveform, but I can't make it
 * change yet.
 * 
 * Hydra_Software_Devel/4   3/22/05 11:11a darnstein
 * PR 14477:  Measure encode queue depth.  For diagnostic purposes.
 * 
 * Hydra_Software_Devel/3   1/27/04 6:24p darnstein
 * PR 9080: Don't check for overrun errors on encoding.  It is only
 * possible for closed caption encoding, anyway.
 * 
 * Hydra_Software_Devel/2   1/8/04 7:27p darnstein
 * PR 9080: Add capability to query for encoding errors from field
 * 
 * Hydra_Software_Devel/1   10/14/03 11:32a darnstein
 * Initial version
 * 
 ***************************************************************************/
#ifndef BVBILIB_PRIV_H__
#define BVBILIB_PRIV_H__

#include "bvbilib.h"
#include "blst_queue.h"

#ifdef __cplusplus
extern "C" {
#endif

/*
 * This macro take the check for a validity of a handle, and
 * cast to context pointer.
 */
#define BVBILIB_P_GENERIC_GET_CONTEXT(handle, context, structname) \
do { \
	if(!(handle) || \
	   (((structname*)(handle))->ulBlackMagic != \
	   sizeof(structname))) \
	{ \
		BDBG_ERR(("Corrupted context handle\n")); \
		(context) = NULL; \
	} \
	else \
	{ \
		(context) = (structname*)(handle); \
	} \
} while (0)

/* Applications of the above macro */
#define BVBILIB_P_GET_CONTEXT(handle, context) \
	BVBILIB_P_GENERIC_GET_CONTEXT(handle, context, BVBIlib_P_Handle)
#define BVBILIB_P_GET_LIST_CONTEXT(handle, context) \
	BVBILIB_P_GENERIC_GET_CONTEXT(handle, context, BVBIlib_P_List_Handle)
#define BVBILIB_P_GET_DECODE_CONTEXT(handle, context) \
	BVBILIB_P_GENERIC_GET_CONTEXT(handle, context, BVBIlib_P_Decode_Handle)
#define BVBILIB_P_GET_ENCODE_CONTEXT(handle, context) \
	BVBILIB_P_GENERIC_GET_CONTEXT(handle, context, BVBIlib_P_Encode_Handle)

/***************************************************************************
 * VBI internal enumerations and constants
 ***************************************************************************/


/***************************************************************************
 * VBI Internal data structures
 ***************************************************************************/

/* Linked list support: define decode_head, encode_head, field_head. */
typedef struct decode_head decode_head;
BLST_Q_HEAD(decode_head, BVBIlib_P_Decode_Handle);
typedef struct encode_head encode_head;
BLST_Q_HEAD(encode_head, BVBIlib_P_Encode_Handle);
typedef struct field_head field_head;
BLST_Q_HEAD(field_head, BVBIlib_P_FieldHanger);


typedef struct BVBIlib_P_FieldHanger
{
	/* The field handle itself */
	BVBI_Field_Handle hField;

	/* Linked list membership */
	BLST_Q_ENTRY(BVBIlib_P_FieldHanger) link;

} BVBIlib_P_FieldHanger;


typedef struct BVBIlib_P_Handle
{
	/* Black magic */
	uint32_t ulBlackMagic;

	/* handed down from app. */
	BVBI_Handle hBvbi;

	/* List of decode contexts */
	decode_head decode_contexts;

	/* List of encode contexts */
	encode_head encode_contexts;

} BVBIlib_P_Handle;


typedef struct BVBIlib_P_List_Handle
{
	/* Black magic */
	uint32_t ulBlackMagic;

	/* Main VBI context, for creating field handles */
	BVBI_Handle hVbi;

	/* Number of BVBI_Field_Handle entries allocated at creation */
	int nAllocated;

	/* Number of BVBI_Field_Handle entries handed out.  
	  For debugging, mainly.  */
	int nInUse;

	/* Free list of field handles */
	field_head field_contexts;

	/* Field handle hangers with no associated field handles */
	field_head empty_hangers;

	/* Other settings */
	BVBIlib_List_Settings settings;

} BVBIlib_P_List_Handle;


typedef struct BVBIlib_P_Decode_Handle
{
	/* Black magic */
	uint32_t ulBlackMagic;

	/* Back pointer to the BVBIlib context */
	BVBIlib_P_Handle *pVbilib;

	/* The subordinate BVBI_Decode handle */
	BVBI_Decode_Handle hVbiDec;

	/* Source of empty BVBI_Field_Handles to decode into */
	BVBIlib_List_Handle hVbill;

	/* Linked list membership */
	BLST_Q_ENTRY(BVBIlib_P_Decode_Handle) link;

} BVBIlib_P_Decode_Handle;


typedef struct BVBIlib_P_Encode_Handle
{
	/* Black magic */
	uint32_t ulBlackMagic;

	/* Back pointer to the VBI context */
	BVBIlib_P_Handle *pVbilib;

	/* The subordinate BVBI_Encode handle */
	BVBI_Encode_Handle hVbiEnc;

	/* Maximum number of BVBI_Field_Handles to queue for encoding */
	int nMaxQueue;

	/* Input queue of field handles to encode */
	field_head encode_queue;

	/* Size of above queue */
	int encode_queue_length;

	/* Field handle hangers with no associated field handles */
	field_head empty_hangers;

	/* List of BVBI_Field_Handles to recycle to after use */
	BVBIlib_List_Handle hVbill;

	/* Linked list membership */
	BLST_Q_ENTRY(BVBIlib_P_Encode_Handle) link;

} BVBIlib_P_Encode_Handle;


/***************************************************************************
 * VBI private functions
 ***************************************************************************/

#ifdef __cplusplus
}
#endif

#endif /* BVBILIB_PRIV_H__ */

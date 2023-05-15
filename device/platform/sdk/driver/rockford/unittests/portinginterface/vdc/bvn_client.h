/***************************************************************************
 *     Copyright (c) 2003-2012, Broadcom Corporation
 *     All Rights Reserved
 *     Confidential Property of Broadcom Corporation
 *
 *  THIS SOFTWARE MAY ONLY BE USED SUBJECT TO AN EXECUTED SOFTWARE LICENSE
 *  AGREEMENT  BETWEEN THE USER AND BROADCOM.  YOU HAVE NO RIGHT TO USE OR
 *  EXPLOIT THIS MATERIAL EXCEPT SUBJECT TO THE TERMS OF SUCH AN AGREEMENT.
 *
 * $brcm_Workfile: bvn_client.h $
 * $brcm_Revision: Hydra_Software_Devel/6 $
 *
 * Module Description:
 *   Define bvn clients on which memory controller
 * Revision History:
 *
 * $ $
 *
 ***************************************************************************/

/*
 *   Define bvn clients on which memory controller
 *   0/1/2: on memc 0/1/2 respectively
 *   -1:    not available for the current chipset
*/


/* different RTS config has client switched from one memc controller to another one */

#ifndef VDC_TEST_BVNCLIENT_H__
#define VDC_TEST_BVNCLIENT_H__

#ifdef __cplusplus
extern "C" {
#endif


 /***************************************************************************
 Summary:
	 List of BVN/XVD/ViCE mem client.
 
 See Also:
 ****************************************************************************/
 typedef enum
 {

	/* BVN */
	 BTST_P_eMPEG0_MEM = 0,
	 BTST_P_eMPEG1_MEM,
	 BTST_P_eMPEG2_MEM,
	 BTST_P_eMPEG3_MEM,
	 BTST_P_eMPEG4_MEM,
	 BTST_P_eMPEG5_MEM,

	 BTST_P_eVEC0_MEM,
	 BTST_P_eVEC1_MEM,

	 BTST_P_e656IN0_MEM,
	 BTST_P_e656IN1_MEM,

	 BTST_P_eHDDVI0_MEM,
	 BTST_P_eHDDVI1_MEM,
	 BTST_P_eDS0_MEM,

	 BTST_P_eGFD0_MEM,
	 BTST_P_eGFD1_MEM,
	 BTST_P_eGFD2_MEM,
	 BTST_P_eGFD3_MEM,
	 BTST_P_eGFD4_MEM,
	 BTST_P_eGFD5_MEM,
	 BTST_P_eGFD6_MEM,

	 BTST_P_eVFD0_MEM,
	 BTST_P_eVFD1_MEM,
	 BTST_P_eVFD2_MEM,
	 BTST_P_eVFD3_MEM,
	 BTST_P_eVFD4_MEM,
	 BTST_P_eVFD5_MEM,
	 BTST_P_eVFD6_MEM,
	 BTST_P_eVFD7_MEM,

	 BTST_P_4U_eMPEG0_MEM,
	 BTST_P_4U_eMPEG1_MEM,
	 BTST_P_4U_eMPEG2_MEM,
	 BTST_P_4U_eMPEG3_MEM,
	 BTST_P_4U_eMPEG4_MEM,
	 BTST_P_4U_eMPEG5_MEM,

	 BTST_P_4U_eVEC0_MEM,
	 BTST_P_4U_eVEC1_MEM,

	 BTST_P_4U_e656IN0_MEM,
	 BTST_P_4U_e656IN1_MEM,

	 BTST_P_4U_eHDDVI0_MEM,
	 BTST_P_4U_eHDDVI1_MEM,
	 BTST_P_4U_eDS0_MEM,

	 BTST_P_4U_eGFD0_MEM,
	 BTST_P_4U_eGFD1_MEM,
	 BTST_P_4U_eGFD2_MEM,
	 BTST_P_4U_eGFD3_MEM,
	 BTST_P_4U_eGFD4_MEM,
	 BTST_P_4U_eGFD5_MEM,
	 BTST_P_4U_eGFD6_MEM,

	 BTST_P_4U_eVFD0_MEM,
	 BTST_P_4U_eVFD1_MEM,
	 BTST_P_4U_eVFD2_MEM,
	 BTST_P_4U_eVFD3_MEM,
	 BTST_P_4U_eVFD4_MEM,
	 BTST_P_4U_eVFD5_MEM,
	 BTST_P_4U_eVFD6_MEM,
	 BTST_P_4U_eVFD7_MEM,

	 BTST_P_eMaxCount,
 
	 BTST_P_eInvalid = BTST_P_eMaxCount  /* must be last */
 } BTST_SrcMemClients;


 /* Table for memory id bvn/xvd/vice.client */
 typedef struct
 {
	 BTST_SrcMemClients             eMemcClient;
	 uint32_t                       ulMemcId;
 } BTST_MemcIDTbl;


 typedef enum
 {

	BTST_P_eCMP0_W0_MEM = 0,
	BTST_P_eCMP0_W1_MEM,
	BTST_P_eCMP0_W2_MEM,

	BTST_P_eCMP1_W0_MEM,
	BTST_P_eCMP1_W1_MEM,
	BTST_P_eCMP1_W2_MEM,

	BTST_P_eCMP2_W0_MEM,
	BTST_P_eCMP2_W1_MEM,
	BTST_P_eCMP2_W2_MEM,

	BTST_P_eCMP3_W0_MEM,
	BTST_P_eCMP3_W1_MEM,
	BTST_P_eCMP3_W2_MEM,

	BTST_P_eCMP4_W0_MEM,
	BTST_P_eCMP4_W1_MEM,
	BTST_P_eCMP4_W2_MEM,

	BTST_P_eCMP5_W0_MEM,
	BTST_P_eCMP5_W1_MEM,
	BTST_P_eCMP5_W2_MEM,

	BTST_P_eCMP6_W0_MEM,
	BTST_P_eCMP6_W1_MEM,
	BTST_P_eCMP6_W2_MEM,

	BTST_P_4U_eCMP0_W0_MEM,
	BTST_P_4U_eCMP0_W1_MEM,
	BTST_P_4U_eCMP0_W2_MEM,

	BTST_P_4U_eCMP1_W0_MEM,
	BTST_P_4U_eCMP1_W1_MEM,
	BTST_P_4U_eCMP1_W2_MEM,

	BTST_P_4U_eCMP2_W0_MEM,
	BTST_P_4U_eCMP2_W1_MEM,
	BTST_P_4U_eCMP2_W2_MEM,

	BTST_P_4U_eCMP3_W0_MEM,
	BTST_P_4U_eCMP3_W1_MEM,
	BTST_P_4U_eCMP3_W2_MEM,

	BTST_P_4U_eCMP4_W0_MEM,
	BTST_P_4U_eCMP4_W1_MEM,
	BTST_P_4U_eCMP4_W2_MEM,

	BTST_P_4U_eCMP5_W0_MEM,
	BTST_P_4U_eCMP5_W1_MEM,
	BTST_P_4U_eCMP5_W2_MEM,

	BTST_P_4U_eCMP6_W0_MEM,
	BTST_P_4U_eCMP6_W1_MEM,
	BTST_P_4U_eCMP6_W2_MEM
	
} BTST_WinMemClients;

typedef struct
{
	BTST_WinMemClients             eMemcClient;
	uint32_t                       ulMemcId;
} BTST_WinMemcIDTbl;

typedef enum
{
	 BTST_P_eXVD0_MEM = 0,
	 BTST_P_eXVD1_MEM,
	 
	 BTST_P_eViCE0_MEM,
	 BTST_P_eViCE1_MEM,
	 
	 BTST_P_eDEF_MEM,
	 BTST_P_eVDC_MEM,

	 BTST_P_eVDC_HEAP
}BTST_CoreMemClients;

 typedef struct
 {
	 BTST_CoreMemClients 			eMemcClient;
	 int32_t						ulMemcId;
 } BTST_CoreMemcIDTbl;

 /* Short hand for table entry */
#define BTST_P_MAKE_MEMC_ID(enum_name, memc_id)   \
{ \
	 BTST_P_##enum_name##_MEM,\
	 memc_id \
}

#define BTST_P_MAKE_HEAP_ID(enum_name, memc_id)   \
{ \
	 BTST_P_##enum_name##_HEAP,\
	 memc_id \
}


#define BTST_P_MAKE_MPEG_MEMC_ID(memc_id0, memc_id1, memc_id2, memc_id3, memc_id4, memc_id5)   \
	BTST_P_MAKE_MEMC_ID(eMPEG0,  memc_id0), \
	BTST_P_MAKE_MEMC_ID(eMPEG1,  memc_id1), \
	BTST_P_MAKE_MEMC_ID(eMPEG2,  memc_id2), \
	BTST_P_MAKE_MEMC_ID(eMPEG3,  memc_id3), \
	BTST_P_MAKE_MEMC_ID(eMPEG4,  memc_id4), \
	BTST_P_MAKE_MEMC_ID(eMPEG5,  memc_id5)


#define BTST_P_MAKE_4UMPEG_MEMC_ID(memc_id0, memc_id1, memc_id2, memc_id3, memc_id4, memc_id5)   \
	BTST_P_MAKE_MEMC_ID(4U_eMPEG0,  memc_id0), \
	BTST_P_MAKE_MEMC_ID(4U_eMPEG1,  memc_id1), \
	BTST_P_MAKE_MEMC_ID(4U_eMPEG2,  memc_id2), \
	BTST_P_MAKE_MEMC_ID(4U_eMPEG3,  memc_id3), \
	BTST_P_MAKE_MEMC_ID(4U_eMPEG4,  memc_id4), \
	BTST_P_MAKE_MEMC_ID(4U_eMPEG5,  memc_id5)

#define BTST_P_MAKE_VEC_MEMC_ID(memc_id0, memc_id1)   \
	BTST_P_MAKE_MEMC_ID(eVEC0,  memc_id0),    \
	BTST_P_MAKE_MEMC_ID(eVEC1,  memc_id1)

#define BTST_P_MAKE_4UVEC_MEMC_ID(memc_id0, memc_id1)   \
	BTST_P_MAKE_MEMC_ID(4U_eVEC0,  memc_id0),    \
	BTST_P_MAKE_MEMC_ID(4U_eVEC1,  memc_id1)

#define BTST_P_MAKE_656_MEMC_ID(memc_id0, memc_id1)   \
	BTST_P_MAKE_MEMC_ID(e656IN0,  memc_id0), \
	BTST_P_MAKE_MEMC_ID(e656IN1,  memc_id1)

#define BTST_P_MAKE_4U656_MEMC_ID(memc_id0, memc_id1)   \
	BTST_P_MAKE_MEMC_ID(4U_e656IN0,  memc_id0), \
	BTST_P_MAKE_MEMC_ID(4U_e656IN1,  memc_id1)

#define BTST_P_MAKE_HDDVI_MEMC_ID(memc_id0, memc_id1)  \
	BTST_P_MAKE_MEMC_ID(eHDDVI0,  memc_id0), \
	BTST_P_MAKE_MEMC_ID(eHDDVI1,  memc_id1)

#define BTST_P_MAKE_4UHDDVI_MEMC_ID(memc_id0, memc_id1)  \
	BTST_P_MAKE_MEMC_ID(4U_eHDDVI0,  memc_id0), \
	BTST_P_MAKE_MEMC_ID(4U_eHDDVI1,  memc_id1)

#define BTST_P_MAKE_DS_MEMC_ID(memc_id0)              \
	BTST_P_MAKE_MEMC_ID  (eDS0,memc_id0)

#define BTST_P_MAKE_4UDS_MEMC_ID(memc_id0)            \
	BTST_P_MAKE_MEMC_ID  (4U_eDS0,memc_id0)


#define BTST_P_MAKE_GFD_MEMC_ID(memc_id0, memc_id1, memc_id2, memc_id3, memc_id4, memc_id5, memc_id6)   \
	BTST_P_MAKE_MEMC_ID(eGFD0,  memc_id0), \
	BTST_P_MAKE_MEMC_ID(eGFD1,  memc_id1), \
	BTST_P_MAKE_MEMC_ID(eGFD2,  memc_id2), \
	BTST_P_MAKE_MEMC_ID(eGFD3,  memc_id3), \
	BTST_P_MAKE_MEMC_ID(eGFD4,  memc_id4), \
	BTST_P_MAKE_MEMC_ID(eGFD5,  memc_id5), \
	BTST_P_MAKE_MEMC_ID(eGFD6,  memc_id6)

#define BTST_P_MAKE_4UGFD_MEMC_ID(memc_id0, memc_id1, memc_id2, memc_id3, memc_id4, memc_id5, memc_id6)   \
	BTST_P_MAKE_MEMC_ID(4U_eGFD0,  memc_id0), \
	BTST_P_MAKE_MEMC_ID(4U_eGFD1,  memc_id1), \
	BTST_P_MAKE_MEMC_ID(4U_eGFD2,  memc_id2), \
	BTST_P_MAKE_MEMC_ID(4U_eGFD3,  memc_id3), \
	BTST_P_MAKE_MEMC_ID(4U_eGFD4,  memc_id4), \
	BTST_P_MAKE_MEMC_ID(4U_eGFD5,  memc_id5), \
	BTST_P_MAKE_MEMC_ID(4U_eGFD6,  memc_id6)


#define BTST_P_MAKE_VFD_MEMC_ID(memc_id0, memc_id1, memc_id2, memc_id3, memc_id4, memc_id5, memc_id6, memc_id7)   \
	BTST_P_MAKE_MEMC_ID(eVFD0,  memc_id0), \
	BTST_P_MAKE_MEMC_ID(eVFD1,  memc_id1), \
	BTST_P_MAKE_MEMC_ID(eVFD2,  memc_id2), \
	BTST_P_MAKE_MEMC_ID(eVFD3,  memc_id3), \
	BTST_P_MAKE_MEMC_ID(eVFD4,  memc_id4), \
	BTST_P_MAKE_MEMC_ID(eVFD5,  memc_id5), \
	BTST_P_MAKE_MEMC_ID(eVFD6,  memc_id6), \
	BTST_P_MAKE_MEMC_ID(eVFD7,  memc_id7)



#define BTST_P_MAKE_4UVFD_MEMC_ID(memc_id0, memc_id1, memc_id2, memc_id3, memc_id4, memc_id5, memc_id6, memc_id7)   \
	BTST_P_MAKE_MEMC_ID(4U_eVFD0,  memc_id0), \
	BTST_P_MAKE_MEMC_ID(4U_eVFD1,  memc_id1), \
	BTST_P_MAKE_MEMC_ID(4U_eVFD2,  memc_id2), \
	BTST_P_MAKE_MEMC_ID(4U_eVFD3,  memc_id3), \
	BTST_P_MAKE_MEMC_ID(4U_eVFD4,  memc_id4), \
	BTST_P_MAKE_MEMC_ID(4U_eVFD5,  memc_id5), \
	BTST_P_MAKE_MEMC_ID(4U_eVFD6,  memc_id6), \
	BTST_P_MAKE_MEMC_ID(4U_eVFD7,  memc_id7)
	

#define BTST_P_MAKE_CMP0_MEMC_ID(memc_id0, memc_id1, memc_id2) \
	BTST_P_MAKE_MEMC_ID(eCMP0_W0, memc_id0),       \
	BTST_P_MAKE_MEMC_ID(eCMP0_W1, memc_id1),       \
	BTST_P_MAKE_MEMC_ID(eCMP0_W2, memc_id2)

#define BTST_P_MAKE_CMP1_MEMC_ID(memc_id0, memc_id1, memc_id2) \
	BTST_P_MAKE_MEMC_ID(eCMP1_W0, memc_id0),       \
	BTST_P_MAKE_MEMC_ID(eCMP1_W1, memc_id1),       \
	BTST_P_MAKE_MEMC_ID(eCMP1_W2, memc_id2)

#define BTST_P_MAKE_CMP2_MEMC_ID(memc_id0, memc_id1, memc_id2) \
	BTST_P_MAKE_MEMC_ID(eCMP2_W0, memc_id0),       \
	BTST_P_MAKE_MEMC_ID(eCMP2_W1, memc_id1),       \
	BTST_P_MAKE_MEMC_ID(eCMP2_W2, memc_id2)

#define BTST_P_MAKE_CMP3_MEMC_ID(memc_id0, memc_id1, memc_id2) \
	BTST_P_MAKE_MEMC_ID(eCMP3_W0, memc_id0),       \
	BTST_P_MAKE_MEMC_ID(eCMP3_W1, memc_id1),       \
	BTST_P_MAKE_MEMC_ID(eCMP3_W2, memc_id2)

#define BTST_P_MAKE_CMP4_MEMC_ID(memc_id0, memc_id1, memc_id2) \
	BTST_P_MAKE_MEMC_ID(eCMP4_W0, memc_id0),       \
	BTST_P_MAKE_MEMC_ID(eCMP4_W1, memc_id1),       \
	BTST_P_MAKE_MEMC_ID(eCMP4_W2, memc_id2)
	
#define BTST_P_MAKE_CMP5_MEMC_ID(memc_id0, memc_id1, memc_id2) \
	BTST_P_MAKE_MEMC_ID(eCMP5_W0, memc_id0),       \
	BTST_P_MAKE_MEMC_ID(eCMP5_W1, memc_id1),       \
	BTST_P_MAKE_MEMC_ID(eCMP5_W2, memc_id2)

#define BTST_P_MAKE_CMP6_MEMC_ID(memc_id0, memc_id1, memc_id2) \
	BTST_P_MAKE_MEMC_ID(eCMP6_W0, memc_id0),       \
	BTST_P_MAKE_MEMC_ID(eCMP6_W1, memc_id1),       \
	BTST_P_MAKE_MEMC_ID(eCMP6_W2, memc_id2)
	

#define BTST_P_MAKE_4UCMP0_MEMC_ID(memc_id0, memc_id1, memc_id2) \
	BTST_P_MAKE_MEMC_ID(4U_eCMP0_W0, memc_id0),       \
	BTST_P_MAKE_MEMC_ID(4U_eCMP0_W1, memc_id1),       \
	BTST_P_MAKE_MEMC_ID(4U_eCMP0_W2, memc_id2)
 
#define BTST_P_MAKE_4UCMP1_MEMC_ID(memc_id0, memc_id1, memc_id2) \
	BTST_P_MAKE_MEMC_ID(4U_eCMP1_W0, memc_id0),       \
	BTST_P_MAKE_MEMC_ID(4U_eCMP1_W1, memc_id1),       \
	BTST_P_MAKE_MEMC_ID(4U_eCMP1_W2, memc_id2)
 
#define BTST_P_MAKE_4UCMP2_MEMC_ID(memc_id0, memc_id1, memc_id2) \
	BTST_P_MAKE_MEMC_ID(4U_eCMP2_W0, memc_id0),       \
	BTST_P_MAKE_MEMC_ID(4U_eCMP2_W1, memc_id1),       \
	BTST_P_MAKE_MEMC_ID(4U_eCMP2_W2, memc_id2)
 
#define BTST_P_MAKE_4UCMP3_MEMC_ID(memc_id0, memc_id1, memc_id2) \
	BTST_P_MAKE_MEMC_ID(4U_eCMP3_W0, memc_id0),       \
	BTST_P_MAKE_MEMC_ID(4U_eCMP3_W1, memc_id1),       \
	BTST_P_MAKE_MEMC_ID(4U_eCMP3_W2, memc_id2)
 
#define BTST_P_MAKE_4UCMP4_MEMC_ID(memc_id0, memc_id1, memc_id2) \
	BTST_P_MAKE_MEMC_ID(4U_eCMP4_W0, memc_id0),       \
	BTST_P_MAKE_MEMC_ID(4U_eCMP4_W1, memc_id1),       \
	BTST_P_MAKE_MEMC_ID(4U_eCMP4_W2, memc_id2)
	 
#define BTST_P_MAKE_4UCMP5_MEMC_ID(memc_id0, memc_id1, memc_id2) \
	BTST_P_MAKE_MEMC_ID(4U_eCMP5_W0, memc_id0),       \
	BTST_P_MAKE_MEMC_ID(4U_eCMP5_W1, memc_id1),       \
	BTST_P_MAKE_MEMC_ID(4U_eCMP5_W2, memc_id2)

#define BTST_P_MAKE_4UCMP6_MEMC_ID(memc_id0, memc_id1, memc_id2) \
	BTST_P_MAKE_MEMC_ID(4U_eCMP6_W0, memc_id0),       \
	BTST_P_MAKE_MEMC_ID(4U_eCMP6_W1, memc_id1),       \
	BTST_P_MAKE_MEMC_ID(4U_eCMP6_W2, memc_id2)


#define BTST_P_MAKE_XVD(memc_id0, memc_id1)                 \
	BTST_P_MAKE_MEMC_ID(eXVD0, memc_id0),                    \
	BTST_P_MAKE_MEMC_ID(eXVD1, memc_id1)

#define BTST_P_MAKE_VICE(memc_id0, memc_id1)                 \
	BTST_P_MAKE_MEMC_ID(eViCE0, memc_id0),                    \
	BTST_P_MAKE_MEMC_ID(eViCE1, memc_id1)

static const BTST_MemcIDTbl s_MemcIds[] =
{
#if ((BCHP_CHIP==7400) || (BCHP_CHIP==7405))

	BTST_P_MAKE_MPEG_MEMC_ID ( 0,  0, -1, -1, -1, -1),
	BTST_P_MAKE_VEC_MEMC_ID  (-1, -1),
	
#if (BCHP_CHIP==7400) 
	BTST_P_MAKE_656_MEMC_ID  ( 0,  0),
#else
	BTST_P_MAKE_656_MEMC_ID  (-1, -1),
#endif

	BTST_P_MAKE_GFD_MEMC_ID  ( 0,  0, -1, -1, -1, -1, -1),
	BTST_P_MAKE_HDDVI_MEMC_ID( 0,  0),
	BTST_P_MAKE_DS_MEMC_ID   (-1),
	BTST_P_MAKE_VFD_MEMC_ID  ( 0,  0, -1, -1, -1, -1, -1, -1)

#elif (BCHP_CHIP==7420) 

	BTST_P_MAKE_MPEG_MEMC_ID ( 0,  1, -1, -1, -1, -1),
	BTST_P_MAKE_VEC_MEMC_ID  (-1, -1),
	BTST_P_MAKE_656_MEMC_ID  (-1, -1),
	BTST_P_MAKE_GFD_MEMC_ID  ( 1,  1,  0, -1, -1, -1, -1),
	BTST_P_MAKE_HDDVI_MEMC_ID( 0,  0),
	BTST_P_MAKE_DS_MEMC_ID   (-1),
	BTST_P_MAKE_VFD_MEMC_ID  ( 1,  1,  1, -1, -1, -1, -1, -1)

#elif ((BCHP_CHIP==7422) || (BCHP_CHIP==7425))

#if (BCHP_CHIP==7422)
	BTST_P_MAKE_MPEG_MEMC_ID ( 0,  1,  1, -1, -1, -1),
#else
	BTST_P_MAKE_MPEG_MEMC_ID ( 1,  0,  0, -1, -1, -1),
#endif

	BTST_P_MAKE_VEC_MEMC_ID  (-1, -1),
	BTST_P_MAKE_656_MEMC_ID  (-1, -1),
	
#if (BCHP_CHIP==7422)
	BTST_P_MAKE_GFD_MEMC_ID  ( 0,  0,  1,  1, -1, -1, -1),
#else
	BTST_P_MAKE_GFD_MEMC_ID  ( 1,  1,  1,  1, -1, -1, -1),
#endif
	BTST_P_MAKE_HDDVI_MEMC_ID( 1,  1),
	BTST_P_MAKE_DS_MEMC_ID   (-1),
	
#if (BCHP_CHIP==7422)
	BTST_P_MAKE_VFD_MEMC_ID  ( 1,  0,  1,  1, -1, -1, -1, -1)
#else
	BTST_P_MAKE_VFD_MEMC_ID  ( 0,  1,  0,  1, -1, -1, -1, -1)
#endif

#elif (BCHP_CHIP==7435) 

	BTST_P_MAKE_MPEG_MEMC_ID ( 1,  1,  0,  0, -1, -1),
	BTST_P_MAKE_VEC_MEMC_ID  (-1, -1),
	BTST_P_MAKE_656_MEMC_ID  (-1, -1),
	BTST_P_MAKE_GFD_MEMC_ID  ( 1,  1,  1,  0,  1,  1, -1),
	BTST_P_MAKE_HDDVI_MEMC_ID( 1,  1),
	BTST_P_MAKE_DS_MEMC_ID   (-1),
	BTST_P_MAKE_VFD_MEMC_ID  ( 0,  1,  0,  1,  1,  1, -1, -1),


	BTST_P_MAKE_4UMPEG_MEMC_ID ( 1,  1,  0,  0, -1, -1),
	BTST_P_MAKE_4UVEC_MEMC_ID  (-1, -1),
	BTST_P_MAKE_4U656_MEMC_ID  (-1, -1),
	BTST_P_MAKE_4UGFD_MEMC_ID  ( 0,  1,  0,  0,  1,  1, -1 ),
	BTST_P_MAKE_4UHDDVI_MEMC_ID( 1,  1),
	BTST_P_MAKE_4UDS_MEMC_ID   (-1),
	BTST_P_MAKE_4UVFD_MEMC_ID  ( 1,  1,  0,  1,  1,  0, -1, -1)


#elif (BCHP_CHIP==7445) 

	BTST_P_MAKE_MPEG_MEMC_ID ( 2,  2,  1,  1,  0,  0),
	BTST_P_MAKE_VEC_MEMC_ID  (-1, -1),
	BTST_P_MAKE_656_MEMC_ID  (-1, -1),
	BTST_P_MAKE_GFD_MEMC_ID  ( 1,  1,  1,  0,  0,  1,  1),
	BTST_P_MAKE_HDDVI_MEMC_ID( 1,  1),
	BTST_P_MAKE_DS_MEMC_ID   (-1),
	BTST_P_MAKE_VFD_MEMC_ID  ( 1,  1,  1,  1,  1,  1,  1,  1),

	BTST_P_MAKE_4UMPEG_MEMC_ID ( 2,  2,  1,  1,  0,  0),
	BTST_P_MAKE_4UVEC_MEMC_ID  (-1, -1),
	BTST_P_MAKE_4U656_MEMC_ID  (-1, -1),
	BTST_P_MAKE_4UGFD_MEMC_ID  ( 2,  2,  2,  0,  0,  1,  1 ),
	BTST_P_MAKE_4UHDDVI_MEMC_ID( 1,  1),
	BTST_P_MAKE_4UDS_MEMC_ID   (-1),
	BTST_P_MAKE_4UVFD_MEMC_ID  ( 2,  2,  2,  1,  1,  1,  1,  1)


#elif ((BCHP_CHIP==7401) || (BCHP_CHIP==7403) || (BCHP_CHIP==7408) || \
       (BCHP_CHIP==7410) || (BCHP_CHIP==7125) || (BCHP_CHIP==7468) || \
       (BCHP_CHIP==7550) || (BCHP_CHIP==7552) || (BCHP_CHIP==7358) || \
       (BCHP_CHIP==7360) || (BCHP_CHIP==7340) || (BCHP_CHIP==7342) || \
       (BCHP_CHIP==7335) || (BCHP_CHIP==7336) || (BCHP_CHIP==7325) || \
       (BCHP_CHIP==7346) || (BCHP_CHIP==7344) || (BCHP_CHIP==7231) || \
       (BCHP_CHIP==7428) || (BCHP_CHIP==7429) || (BCHP_CHIP==7584) || \
       (BCHP_CHIP==7208))

#if ((BCHP_CHIP==7346) || (BCHP_CHIP==7344) || (BCHP_CHIP==7231) || \
	 (BCHP_CHIP==7428) || (BCHP_CHIP==7429))|| (BCHP_CHIP==7584) 
	BTST_P_MAKE_MPEG_MEMC_ID ( 0,  0, -1, -1, -1, -1),
#else
	BTST_P_MAKE_MPEG_MEMC_ID ( 0, -1, -1, -1, -1, -1),
#endif
	BTST_P_MAKE_VEC_MEMC_ID  (-1, -1),
	BTST_P_MAKE_656_MEMC_ID  (-1, -1),
	BTST_P_MAKE_GFD_MEMC_ID  ( 0,  0, -1, -1, -1, -1, -1),
	BTST_P_MAKE_HDDVI_MEMC_ID( 0,  0),
	BTST_P_MAKE_DS_MEMC_ID   (-1),

#if ((BCHP_CHIP==7346) || (BCHP_CHIP==7344) || (BCHP_CHIP==7231) || \
	 (BCHP_CHIP==7428) || (BCHP_CHIP==7429))|| (BCHP_CHIP==7584) 
	BTST_P_MAKE_VFD_MEMC_ID  ( 0,  0,  0,  0, -1, -1, -1, -1)
#else
	BTST_P_MAKE_VFD_MEMC_ID  ( 0,  0, -1, -1, -1, -1, -1, -1)
#endif

#else
#error "Port reqired for BVN Client Error."
#endif
};


/* window on each memory controller */
static const BTST_WinMemcIDTbl s_WinMemId[] =
{
#if ((BCHP_CHIP==7400) || (BCHP_CHIP==7405))

	BTST_P_MAKE_CMP0_MEMC_ID ( 0,  0,  0),
	BTST_P_MAKE_CMP1_MEMC_ID ( 0,  0,  0),
	BTST_P_MAKE_CMP2_MEMC_ID (-1, -1, -1),
	BTST_P_MAKE_CMP3_MEMC_ID (-1, -1, -1),
	BTST_P_MAKE_CMP4_MEMC_ID (-1, -1, -1),
	BTST_P_MAKE_CMP5_MEMC_ID (-1, -1, -1),
	BTST_P_MAKE_CMP6_MEMC_ID (-1, -1, -1)

	
#elif (BCHP_CHIP==7420) 
	BTST_P_MAKE_CMP0_MEMC_ID ( 1,  0,  1),
	BTST_P_MAKE_CMP1_MEMC_ID ( 1,  0,  1),
	BTST_P_MAKE_CMP2_MEMC_ID ( 0,  0,  0),
	BTST_P_MAKE_CMP3_MEMC_ID (-1, -1, -1),
	BTST_P_MAKE_CMP4_MEMC_ID (-1, -1, -1),
	BTST_P_MAKE_CMP5_MEMC_ID (-1, -1, -1),
	BTST_P_MAKE_CMP6_MEMC_ID (-1, -1, -1)


#elif (BCHP_CHIP==7422)
	
	BTST_P_MAKE_CMP0_MEMC_ID ( 1,  0,  0),
	BTST_P_MAKE_CMP1_MEMC_ID ( 1,  1,  0),
	BTST_P_MAKE_CMP2_MEMC_ID ( 1,  1,  1),
	BTST_P_MAKE_CMP3_MEMC_ID ( 1,  1,  1),
	BTST_P_MAKE_CMP4_MEMC_ID (-1, -1, -1),
	BTST_P_MAKE_CMP5_MEMC_ID (-1, -1, -1),
	BTST_P_MAKE_CMP6_MEMC_ID (-1, -1, -1)


#elif (BCHP_CHIP==7425)
	BTST_P_MAKE_CMP0_MEMC_ID ( 0,  1,  1),
	BTST_P_MAKE_CMP1_MEMC_ID ( 0,  1,  1),
	BTST_P_MAKE_CMP2_MEMC_ID ( 1,  1,  1),
	BTST_P_MAKE_CMP3_MEMC_ID ( 1,  1,  1),
	BTST_P_MAKE_CMP4_MEMC_ID (-1, -1, -1),
	BTST_P_MAKE_CMP5_MEMC_ID (-1, -1, -1),
	BTST_P_MAKE_CMP6_MEMC_ID (-1, -1, -1)



#elif (BCHP_CHIP==7435) 
	BTST_P_MAKE_CMP0_MEMC_ID ( 0,  1,  1),
	BTST_P_MAKE_CMP1_MEMC_ID ( 0,  1,  1),
	BTST_P_MAKE_CMP2_MEMC_ID ( 1,  1,  1),
	BTST_P_MAKE_CMP3_MEMC_ID ( 1,  1,  1),
	BTST_P_MAKE_CMP4_MEMC_ID ( 1,  1,  1),
	BTST_P_MAKE_CMP5_MEMC_ID ( 1,  1,  1),
	BTST_P_MAKE_CMP6_MEMC_ID (-1, -1, -1),

	BTST_P_MAKE_4UCMP0_MEMC_ID ( 1,  1,  1),
	BTST_P_MAKE_4UCMP1_MEMC_ID ( 0,  1,  1),
	BTST_P_MAKE_4UCMP2_MEMC_ID ( 1,  1,  1),
	BTST_P_MAKE_4UCMP3_MEMC_ID ( 1,  1,  1),
	BTST_P_MAKE_4UCMP4_MEMC_ID ( 1,  1,  1),
	BTST_P_MAKE_4UCMP5_MEMC_ID ( 0,  1,  1),
	BTST_P_MAKE_4UCMP6_MEMC_ID (-1, -1, -1)

#elif (BCHP_CHIP==7445) 

	BTST_P_MAKE_CMP0_MEMC_ID ( 1,  1,  1),
	BTST_P_MAKE_CMP1_MEMC_ID ( 1,  1,  1),
	BTST_P_MAKE_CMP2_MEMC_ID ( 1, -1,  1),
	BTST_P_MAKE_CMP3_MEMC_ID ( 1, -1,  0),
	BTST_P_MAKE_CMP4_MEMC_ID ( 1, -1,  0),
	BTST_P_MAKE_CMP5_MEMC_ID ( 1, -1,  1),
	BTST_P_MAKE_CMP6_MEMC_ID ( 1, -1,  1),

	BTST_P_MAKE_4UCMP0_MEMC_ID ( 2,  2,  2),
	BTST_P_MAKE_4UCMP1_MEMC_ID ( 2,  1,  2),
	BTST_P_MAKE_4UCMP2_MEMC_ID ( 1, -1,  2),
	BTST_P_MAKE_4UCMP3_MEMC_ID ( 1, -1,  0),
	BTST_P_MAKE_4UCMP4_MEMC_ID ( 1, -1,  0),
	BTST_P_MAKE_4UCMP5_MEMC_ID ( 1, -1,  1),
	BTST_P_MAKE_4UCMP6_MEMC_ID ( 1, -1,  1)

#elif ((BCHP_CHIP==7401) || (BCHP_CHIP==7403) || (BCHP_CHIP==7408) || \
      (BCHP_CHIP==7410) || (BCHP_CHIP==7125) || (BCHP_CHIP==7468) || \
      (BCHP_CHIP==7550) || (BCHP_CHIP==7552) || (BCHP_CHIP==7358) || \
      (BCHP_CHIP==7360) || (BCHP_CHIP==7340) || (BCHP_CHIP==7342) || \
      (BCHP_CHIP==7335) || (BCHP_CHIP==7336) || (BCHP_CHIP==7325) || \
      (BCHP_CHIP==7346) || (BCHP_CHIP==7344) || (BCHP_CHIP==7231) || \
      (BCHP_CHIP==7428) || (BCHP_CHIP==7429) || (BCHP_CHIP==7584))
      
	BTST_P_MAKE_CMP0_MEMC_ID ( 0,  0,  0),
	BTST_P_MAKE_CMP1_MEMC_ID ( 0,  0,  0),
	BTST_P_MAKE_CMP2_MEMC_ID (-1, -1, -1),
	BTST_P_MAKE_CMP3_MEMC_ID (-1, -1, -1),
	BTST_P_MAKE_CMP4_MEMC_ID (-1, -1, -1),
	BTST_P_MAKE_CMP5_MEMC_ID (-1, -1, -1),
	BTST_P_MAKE_CMP6_MEMC_ID (-1, -1, -1)



#else
#error "Port reqired for BufCnt Err."
#endif
};


/* window on each memory controller */
static const BTST_CoreMemcIDTbl s_CoreMemId[] =
{
#if ((BCHP_CHIP==7400) || (BCHP_CHIP==7405))

#if (BCHP_CHIP==7400) 
	BTST_P_MAKE_XVD  ( 1,  2),
#else
	BTST_P_MAKE_XVD  ( 1, -1),
#endif
	BTST_P_MAKE_VICE (-1, -1),

	BTST_P_MAKE_MEMC_ID(eDEF, 0),
	BTST_P_MAKE_MEMC_ID(eVDC, 0),
	BTST_P_MAKE_HEAP_ID(eVDC, 0),


	
#elif (BCHP_CHIP==7420) 

	BTST_P_MAKE_XVD  ( 0,  1),
	BTST_P_MAKE_VICE (-1, -1),

	BTST_P_MAKE_MEMC_ID(eDEF, 1),
	BTST_P_MAKE_MEMC_ID(eVDC, 0),
	BTST_P_MAKE_HEAP_ID(eVDC, 1),



#elif ((BCHP_CHIP==7422) || (BCHP_CHIP==7425))


	BTST_P_MAKE_XVD  ( 1,  2),
#if (BCHP_CHIP==7422)
	BTST_P_MAKE_VICE (-1, -1),
#else
	BTST_P_MAKE_VICE ( 0,  1),
#endif

	BTST_P_MAKE_MEMC_ID(eDEF, 1),
	BTST_P_MAKE_MEMC_ID(eVDC, 0),
	BTST_P_MAKE_HEAP_ID(eVDC, 1),


#elif (BCHP_CHIP==7435)

	BTST_P_MAKE_XVD  ( 1,  2),
	BTST_P_MAKE_VICE ( 2,  1),
	BTST_P_MAKE_MEMC_ID(eDEF, 1),
	BTST_P_MAKE_MEMC_ID(eVDC, 0),
	BTST_P_MAKE_HEAP_ID(eVDC, 1),
	
#elif (BCHP_CHIP==7445)

	BTST_P_MAKE_XVD  ( 0,  1),
	BTST_P_MAKE_VICE ( 0,  1),
	BTST_P_MAKE_MEMC_ID(eDEF, 1),
	BTST_P_MAKE_MEMC_ID(eVDC, 0),
	BTST_P_MAKE_HEAP_ID(eVDC, 1),



#elif ((BCHP_CHIP==7401) || (BCHP_CHIP==7403) || (BCHP_CHIP==7408) || \
       (BCHP_CHIP==7410) || (BCHP_CHIP==7125) || (BCHP_CHIP==7468) || \
       (BCHP_CHIP==7550) || (BCHP_CHIP==7552) || (BCHP_CHIP==7358) || \
       (BCHP_CHIP==7360) || (BCHP_CHIP==7340) || (BCHP_CHIP==7342) || \
       (BCHP_CHIP==7335) || (BCHP_CHIP==7336) || (BCHP_CHIP==7325) || \
       (BCHP_CHIP==7346) || (BCHP_CHIP==7344) || (BCHP_CHIP==7231) || \
       (BCHP_CHIP==7428) || (BCHP_CHIP==7429) || (BCHP_CHIP==7584) || \
       (BCHP_CHIP==7208))
      
#if ((BCHP_CHIP==7325) || (BCHP_CHIP==7335)|| (BCHP_CHIP==7336))
		BTST_P_MAKE_XVD  ( 1, -1),
#else
		BTST_P_MAKE_XVD  ( 0, -1),
#endif
		BTST_P_MAKE_VICE (-1, -1),
	
		BTST_P_MAKE_MEMC_ID(eDEF, 0),
		BTST_P_MAKE_MEMC_ID(eVDC, 1),
		BTST_P_MAKE_HEAP_ID(eVDC, 0),



#else
#error "Port reqired for BufCnt Err."
#endif
};

#ifdef __cplusplus
}
#endif

#endif /* VDC_TEST_BVNCLIENT_H__ */


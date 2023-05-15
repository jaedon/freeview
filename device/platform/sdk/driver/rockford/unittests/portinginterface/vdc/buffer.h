/***************************************************************************
 *     Copyright (c) 2003-2012, Broadcom Corporation
 *     All Rights Reserved
 *     Confidential Property of Broadcom Corporation
 *
 *  THIS SOFTWARE MAY ONLY BE USED SUBJECT TO AN EXECUTED SOFTWARE LICENSE
 *  AGREEMENT  BETWEEN THE USER AND BROADCOM.  YOU HAVE NO RIGHT TO USE OR
 *  EXPLOIT THIS MATERIAL EXCEPT SUBJECT TO THE TERMS OF SUCH AN AGREEMENT.
 *
 * $brcm_Workfile: buffer.h $
 * $brcm_Revision: Hydra_Software_Devel/4 $
 *
 * Module Description:
 *   See Module Overview below.
 *
 * Revision History:
 *
 * $ $
 *
 ***************************************************************************/
#ifndef VDC_TEST_BUFFER_H__
#define VDC_TEST_BUFFER_H__
	 
#ifdef __cplusplus
	 extern "C" {
#endif

/* Buffer pixel Fmt */
#define BTST_P_MEM_PIXFMT_2HD              (BPXL_eCr8_Y18_Cb8_Y08)
#define BTST_P_MEM_PIXFMT_HD               (BPXL_eCr8_Y18_Cb8_Y08)
#define BTST_P_MEM_PIXFMT_SD               (BPXL_eCr8_Y18_Cb8_Y08)

/* Table for buffer cnt on heap */

typedef enum
{
	BTST_P_MEM0_BUFCNT_2HD = 0,
	BTST_P_MEM0_BUFCNT_2HD_PIP,
	BTST_P_MEM0_BUFCNT_HD,
	BTST_P_MEM0_BUFCNT_HD_PIP,
	BTST_P_MEM0_BUFCNT_SD,
	BTST_P_MEM0_BUFCNT_SD_PIP,
	
	BTST_P_MEM1_BUFCNT_2HD,
	BTST_P_MEM1_BUFCNT_2HD_PIP,
	BTST_P_MEM1_BUFCNT_HD,
	BTST_P_MEM1_BUFCNT_HD_PIP,
	BTST_P_MEM1_BUFCNT_SD,
	BTST_P_MEM1_BUFCNT_SD_PIP,

	BTST_P_MEM2_BUFCNT_2HD,
	BTST_P_MEM2_BUFCNT_2HD_PIP,
	BTST_P_MEM2_BUFCNT_HD,
	BTST_P_MEM2_BUFCNT_HD_PIP,
	BTST_P_MEM2_BUFCNT_SD,
	BTST_P_MEM2_BUFCNT_SD_PIP

}BTST_HeapBufCnt;

typedef struct
{
  BTST_HeapBufCnt                eMemcClient;
  uint32_t                       ulBufCnt;
} BTST_BufCnt_Tbl;

 /* Short hand for table entry */
#define BTST_P_MAKE_MEMC_BUFCNT(memc_id, bufsize, bufcnt)   \
{ \
	 BTST_P_##memc_id##_BUFCNT_##bufsize,\
	 bufcnt \
}

#define BTST_P_MAKE_MEMC0(bufcnt_2HD, bufcnt_2HDPIP, bufcnt_HD, bufcnt_HDPIP, bufcnt_SD, bufcnt_SDPIP)   \
	BTST_P_MAKE_MEMC_BUFCNT(MEM0,     2HD,    bufcnt_2HD),  \
	BTST_P_MAKE_MEMC_BUFCNT(MEM0, 2HD_PIP, bufcnt_2HDPIP),  \
	BTST_P_MAKE_MEMC_BUFCNT(MEM0,      HD,     bufcnt_HD),  \
	BTST_P_MAKE_MEMC_BUFCNT(MEM0,  HD_PIP,  bufcnt_HDPIP),  \
	BTST_P_MAKE_MEMC_BUFCNT(MEM0,      SD,     bufcnt_SD),  \
	BTST_P_MAKE_MEMC_BUFCNT(MEM0,  SD_PIP,  bufcnt_SDPIP)

#define BTST_P_MAKE_MEMC1(bufcnt_2HD, bufcnt_2HDPIP, bufcnt_HD, bufcnt_HDPIP, bufcnt_SD, bufcnt_SDPIP)   \
	BTST_P_MAKE_MEMC_BUFCNT(MEM1,     2HD,    bufcnt_2HD),  \
	BTST_P_MAKE_MEMC_BUFCNT(MEM1, 2HD_PIP, bufcnt_2HDPIP),  \
	BTST_P_MAKE_MEMC_BUFCNT(MEM1,      HD,     bufcnt_HD),  \
	BTST_P_MAKE_MEMC_BUFCNT(MEM1,  HD_PIP,  bufcnt_HDPIP),  \
	BTST_P_MAKE_MEMC_BUFCNT(MEM1,      SD,     bufcnt_SD),  \
	BTST_P_MAKE_MEMC_BUFCNT(MEM1,  SD_PIP,  bufcnt_SDPIP)

#define BTST_P_MAKE_MEMC2(bufcnt_2HD, bufcnt_2HDPIP, bufcnt_HD, bufcnt_HDPIP, bufcnt_SD, bufcnt_SDPIP)   \
	BTST_P_MAKE_MEMC_BUFCNT(MEM2,     2HD,    bufcnt_2HD),  \
	BTST_P_MAKE_MEMC_BUFCNT(MEM2, 2HD_PIP, bufcnt_2HDPIP),  \
	BTST_P_MAKE_MEMC_BUFCNT(MEM2,      HD,     bufcnt_HD),  \
	BTST_P_MAKE_MEMC_BUFCNT(MEM2,  HD_PIP,  bufcnt_HDPIP),  \
	BTST_P_MAKE_MEMC_BUFCNT(MEM2,      SD,     bufcnt_SD),  \
	BTST_P_MAKE_MEMC_BUFCNT(MEM2,  SD_PIP,  bufcnt_SDPIP)

#define BTST_P_INVALID_BUFFER             (-1)
/* buffer cnt on each memory controller */
static const BTST_BufCnt_Tbl s_BufCnts[] =
{
#if ((BCHP_CHIP==7400) || (BCHP_CHIP==7405))

	BTST_P_MAKE_MEMC0(10, 4, 0, 0, 4, 0),
	BTST_P_MAKE_MEMC1( 0, 0, 0, 0, 0, 0),
	BTST_P_MAKE_MEMC2(-1,-1,-1,-1,-1,-1),
	
#elif (BCHP_CHIP==7420) 
	BTST_P_MAKE_MEMC0( 3, 0, 0, 0, 3, 0),
	BTST_P_MAKE_MEMC1( 3, 0, 4, 0, 4, 0),
	BTST_P_MAKE_MEMC2(-1,-1,-1,-1,-1,-1),

#elif ((BCHP_CHIP==7422) || (BCHP_CHIP==7425))

#if (BCHP_CHIP==7422)
	BTST_P_MAKE_MEMC0( 0, 8, 0, 0, 0, 0),
	BTST_P_MAKE_MEMC1(10, 4, 0, 0, 4, 0),
#else
	BTST_P_MAKE_MEMC0( 4, 9, 4, 0, 4, 0),
	BTST_P_MAKE_MEMC1(10, 4, 0, 0, 0, 0),
#endif
	BTST_P_MAKE_MEMC2(-1,-1,-1,-1,-1,-1),

#elif (BCHP_CHIP==7435) 
	BTST_P_MAKE_MEMC0( 4, 9, 4, 0, 4, 0),
	BTST_P_MAKE_MEMC1(10, 4, 0, 0, 0, 0),
	BTST_P_MAKE_MEMC2(-1,-1,-1,-1,-1,-1),

#elif (BCHP_CHIP==7445) /*default copy from 7435 more work on bringup */
	BTST_P_MAKE_MEMC0( 4, 9, 4, 0, 4, 0),
	BTST_P_MAKE_MEMC1(10, 4, 0, 0, 0, 0),
	BTST_P_MAKE_MEMC2(10, 4, 0, 0, 0, 0),


#elif ((BCHP_CHIP==7401) || (BCHP_CHIP==7403) || (BCHP_CHIP==7408) || \
       (BCHP_CHIP==7410) || (BCHP_CHIP==7125) || (BCHP_CHIP==7468) || \
       (BCHP_CHIP==7550) || (BCHP_CHIP==7552) || (BCHP_CHIP==7358) || \
       (BCHP_CHIP==7360) || (BCHP_CHIP==7340) || (BCHP_CHIP==7342) || \
       (BCHP_CHIP==7335) || (BCHP_CHIP==7336) || (BCHP_CHIP==7325) || \
       (BCHP_CHIP==7346) || (BCHP_CHIP==7344) || (BCHP_CHIP==7231) || \
       (BCHP_CHIP==7428) || (BCHP_CHIP==7429) || (BCHP_CHIP==7584) || \
       (BCHP_CHIP==7208))
      
#if (BCHP_CHIP == 7358) || (BCHP_CHIP == 7552) || (BCHP_CHIP == 7360) 
		BTST_P_MAKE_MEMC0( 2, 1, 3, 0, 4, 0),
#else
		BTST_P_MAKE_MEMC0( 0, 0, 8, 0,22, 0),
#endif
		BTST_P_MAKE_MEMC1(-1,-1,-1,-1,-1,-1),
		BTST_P_MAKE_MEMC2(-1,-1,-1,-1,-1,-1),
#else
#error "Port reqired for BufCnt Err."
#endif
};


#ifdef __cplusplus
}
#endif

#endif /* VDC_TEST_BUFFER_H__ */


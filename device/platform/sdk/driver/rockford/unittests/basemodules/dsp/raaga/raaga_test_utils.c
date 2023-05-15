/***************************************************************************
 *     Copyright (c) 2002-2012, Broadcom Corporation
 *     All Rights Reserved
 *     Confidential Property of Broadcom Corporation
 *
 *  THIS SOFTWARE MAY ONLY BE USED SUBJECT TO AN EXECUTED SOFTWARE LICENSE
 *  AGREEMENT  BETWEEN THE USER AND BROADCOM.  YOU HAVE NO RIGHT TO USE OR
 *  EXPLOIT THIS MATERIAL EXCEPT SUBJECT TO THE TERMS OF SUCH AN AGREEMENT.
 *
 * $brcm_Workfile: raaga_test_utils.c $
 * $brcm_Revision: Hydra_Software_Devel/4 $
 * $brcm_Date: 10/17/12 1:45p $
 *
 * Module Description:
 *      This file contains all utility functions which are used in raaga_test. 
 *      These functions auxilary functions, which ase not used frequently.
 *
 * Revision History:
 *
 * $brcm_Log: /rockford/unittests/basemodules/dsp/raaga/raaga_test_utils.c $
 * 
 * Hydra_Software_Devel/4   10/17/12 1:45p gprasad
 * SW7425-3628: Changes for cached access
 * 
 * Hydra_Software_Devel/3   4/3/12 11:22a gprasad
 * FWRAA-382: Updating the error messages to logging messages
 * 
 * Hydra_Software_Devel/2   11/29/11 12:10p ashoky
 * SW7429-18: Changes for new style RDB.
 * 
 * Hydra_Software_Devel/1   1/14/11 3:26p gdata
 * SW7422-191:[7422] Adding the raaga test application
 * 
 ****************************************************************************/ 

#include "raaga_test.h"

BDBG_MODULE(raaga_test);

#if (RAAGA_TEST_STATIC_CDB_ITB == 1)
#if (RAAGA_TEST_STATIC_CDB_ITB_BINARYFILE != 1) 
extern uint32_t cdb_array[];
extern uint32_t itb_array[];
extern uint32_t cdb_array_size;
extern uint32_t itb_array_size;
#endif
#endif

void DumpRaveCxtRegisters 
(
    BREG_Handle hReg, 
    unsigned int uiContext
)
{
    uint32_t ui32Offset;

    ui32Offset = uiContext * (BCHP_XPT_RAVE_CX1_AV_CDB_WRITE_PTR - BCHP_XPT_RAVE_CX0_AV_CDB_WRITE_PTR);

    printf("\nRave Cxt %d register dump\n", uiContext);
    printf("\nBCHP_XPT_RAVE_CX%d_AV_CDB_WRITE_PTR = 0x%08x", uiContext, BREG_Read32(hReg, ui32Offset + BCHP_XPT_RAVE_CX0_AV_CDB_WRITE_PTR));
    printf("\nBCHP_XPT_RAVE_CX%d_AV_CDB_READ_PTR = 0x%08x", uiContext, BREG_Read32(hReg, ui32Offset + BCHP_XPT_RAVE_CX0_AV_CDB_READ_PTR));
    printf("\nBCHP_XPT_RAVE_CX%d_AV_CDB_BASE_PTR = 0x%08x", uiContext, BREG_Read32(hReg, ui32Offset + BCHP_XPT_RAVE_CX0_AV_CDB_BASE_PTR));
    printf("\nBCHP_XPT_RAVE_CX%d_AV_CDB_END_PTR = 0x%08x", uiContext, BREG_Read32(hReg, ui32Offset + BCHP_XPT_RAVE_CX0_AV_CDB_END_PTR));
    printf("\nBCHP_XPT_RAVE_CX%d_AV_CDB_WRAPAROUND_PTR = 0x%08x", uiContext, BREG_Read32(hReg, ui32Offset + BCHP_XPT_RAVE_CX0_AV_CDB_WRAPAROUND_PTR));
    printf("\nBCHP_XPT_RAVE_CX%d_AV_CDB_THRESHOLD_PTR = 0x%08x", uiContext, BREG_Read32(hReg, ui32Offset + BCHP_XPT_RAVE_CX0_AV_CDB_THRESHOLD_LEVEL));
    printf("\nBCHP_XPT_RAVE_CX%d_AV_CDB_DEPTH = 0x%08x", uiContext, BREG_Read32(hReg, ui32Offset + BCHP_XPT_RAVE_CX0_AV_CDB_DEPTH));
    printf("\nBCHP_XPT_RAVE_CX%d_AV_THRESHOLDS = 0x%08x", uiContext, BREG_Read32(hReg, ui32Offset + BCHP_XPT_RAVE_CX0_AV_THRESHOLDS));
    printf("\nBCHP_XPT_RAVE_CX%d_AV_ITB_WRITE_PTR = 0x%08x", uiContext, BREG_Read32(hReg, ui32Offset + BCHP_XPT_RAVE_CX0_AV_ITB_WRITE_PTR));
    printf("\nBCHP_XPT_RAVE_CX%d_AV_ITB_READ_PTR = 0x%08x", uiContext, BREG_Read32(hReg, ui32Offset + BCHP_XPT_RAVE_CX0_AV_ITB_READ_PTR));
    printf("\nBCHP_XPT_RAVE_CX%d_AV_ITB_BASE_PTR = 0x%08x", uiContext, BREG_Read32(hReg, ui32Offset + BCHP_XPT_RAVE_CX0_AV_ITB_BASE_PTR));
    printf("\nBCHP_XPT_RAVE_CX%d_AV_ITB_END_PTR = 0x%08x", uiContext, BREG_Read32(hReg, ui32Offset + BCHP_XPT_RAVE_CX0_AV_ITB_END_PTR));
    printf("\nBCHP_XPT_RAVE_CX%d_AV_ITB_WRAPAROUND_PTR = 0x%08x", uiContext, BREG_Read32(hReg, ui32Offset + BCHP_XPT_RAVE_CX0_AV_ITB_WRAPAROUND_PTR));
    printf("\nBCHP_XPT_RAVE_CX%d_AV_ITB_THRESHOLD_PTR = 0x%08x", uiContext, BREG_Read32(hReg, ui32Offset + BCHP_XPT_RAVE_CX0_AV_ITB_THRESHOLD_LEVEL));
    printf("\nBCHP_XPT_RAVE_CX%d_AV_ITB_DEPTH = 0x%08x", uiContext, BREG_Read32(hReg, ui32Offset + BCHP_XPT_RAVE_CX0_AV_ITB_DEPTH));
    printf("\nBCHP_XPT_RAVE_CX%d_AV_CDB_VALID_PTR = 0x%08x", uiContext, BREG_Read32(hReg, ui32Offset + BCHP_XPT_RAVE_CX0_AV_CDB_VALID_PTR));
    printf("\nBCHP_XPT_RAVE_CX%d_AV_ITB_VALID_PTR = 0x%08x", uiContext, BREG_Read32(hReg, ui32Offset + BCHP_XPT_RAVE_CX0_AV_ITB_VALID_PTR));
/*    printf("\nBCHP_XPT_RAVE_CX%d_AV_VID_SC_RANGE_AV = 0x%08x", uiContext, BREG_Read32(hReg, ui32Offset + BCHP_XPT_RAVE_CX0_AV_VID_SC_RANGE_AB));
    printf("\nBCHP_XPT_RAVE_CX%d_AV_VID_SC_RANGE_CD = 0x%08x", uiContext, BREG_Read32(hReg, ui32Offset + BCHP_XPT_RAVE_CX0_AV_VID_SC_RANGE_CD));*/
    printf("\nBCHP_XPT_RAVE_CX%d_AV_PID_STREAM_ID = 0x%08x", uiContext, BREG_Read32(hReg, ui32Offset + BCHP_XPT_RAVE_CX0_AV_PID_STREAM_ID));
    printf("\nBCHP_XPT_RAVE_CX%d_AV_MISC_CONFIG1 = 0x%08x", uiContext, BREG_Read32(hReg, ui32Offset + BCHP_XPT_RAVE_CX0_AV_MISC_CONFIG1));
    printf("\nBCHP_XPT_RAVE_CX%d_AV_MISC_CONFIG2 = 0x%08x", uiContext, BREG_Read32(hReg, ui32Offset + BCHP_XPT_RAVE_CX0_AV_MISC_CONFIG2));
    printf("\nBCHP_XPT_RAVE_CX%d_AV_COMP1_CONTROL = 0x%08x", uiContext, BREG_Read32(hReg, ui32Offset + BCHP_XPT_RAVE_CX0_AV_COMP1_CONTROL));
    printf("\nBCHP_XPT_RAVE_CX%d_AV_COMP1_COMPARE_VAL = 0x%08x", uiContext, BREG_Read32(hReg, ui32Offset + BCHP_XPT_RAVE_CX0_AV_COMP1_COMPARE_VAL));
    printf("\nBCHP_XPT_RAVE_CX%d_AV_COMP1_MASK_VAL = 0x%08x", uiContext, BREG_Read32(hReg, ui32Offset + BCHP_XPT_RAVE_CX0_AV_COMP1_MASK_VAL));
    printf("\nBCHP_XPT_RAVE_CX%d_AV_COMP2_CONTROL = 0x%08x", uiContext, BREG_Read32(hReg, ui32Offset + BCHP_XPT_RAVE_CX0_AV_COMP2_CONTROL));
    printf("\nBCHP_XPT_RAVE_CX%d_AV_COMP2_COMPARE_VAL = 0x%08x", uiContext, BREG_Read32(hReg, ui32Offset + BCHP_XPT_RAVE_CX0_AV_COMP2_COMPARE_VAL));
    printf("\nBCHP_XPT_RAVE_CX%d_AV_COMP2_MASK_VAL = 0x%08x", uiContext, BREG_Read32(hReg, ui32Offset + BCHP_XPT_RAVE_CX0_AV_COMP2_MASK_VAL));
/*    printf("\nBCHP_XPT_RAVE_CX%d_AV_AV_EXCLUSION = 0x%08x", uiContext, BREG_Read32(hReg, ui32Offset + BCHP_XPT_RAVE_CX0_AV_EXCLUSION));*/
    printf("\nBCHP_XPT_RAVE_CX%d_AV_INTERRUPT_ENABLE = 0x%08x", uiContext, BREG_Read32(hReg, ui32Offset + BCHP_XPT_RAVE_CX0_AV_INTERRUPT_ENABLES));
    printf("\nBCHP_XPT_RAVE_CX%d_AV_MISC_CONFIG3 = 0x%08x", uiContext, BREG_Read32(hReg, ui32Offset + BCHP_XPT_RAVE_CX0_AV_MISC_CONFIG3));
}

#if (RAAGA_TEST_ENABLE_TRANSPORT == 1)
void DumpPtrs 
( 
    BXPT_Rave_ContextPtrs *CtxPtrs 
)
{
    unsigned i;

    printf( "Cdb.DataPtr 0x%lX\n", (unsigned long) CtxPtrs->Cdb.DataPtr );      
    printf( "Cdb.ByteCount %lu\n", (unsigned long) CtxPtrs->Cdb.ByteCount );
    if( CtxPtrs->Cdb.DataPtr )
    {       
        for( i = 0; i < 32; i++ )
        {
            printf( "0x%02hX ", (unsigned short) *( CtxPtrs->Cdb.DataPtr + i ) );
        }
        printf( "\n" );
    }

    printf( "Cdb.WrapDataPtr 0x%lX\n", (unsigned long) CtxPtrs->Cdb.WrapDataPtr );      
    printf( "Cdb.WrapByteCount %lu\n", (unsigned long) CtxPtrs->Cdb.WrapByteCount );        

    printf( "Itb.DataPtr 0x%lX\n", (unsigned long) CtxPtrs->Itb.DataPtr );      
    printf( "Itb.ByteCount %lu\n", (unsigned long) CtxPtrs->Itb.ByteCount );        
    if( CtxPtrs->Itb.DataPtr )
    {
        for( i = 0; i < 32; i++ )
        {
            printf( "0x%02hX ", (unsigned short) *( CtxPtrs->Itb.DataPtr + i ) );
        }
        printf( "\n" );
    }

    printf( "Itb.WrapDataPtr 0x%lX\n", (unsigned long) CtxPtrs->Itb.WrapDataPtr );      
    printf( "Itb.WrapByteCount %lu\n", (unsigned long) CtxPtrs->Itb.WrapByteCount );        
}

void DumpCdbItb 
(
    BMEM_Handle hMemory, 
    BREG_Handle hReg, 
    BAVC_XptContextMap *CtxMap
)
{

    uint8_t *pCdbBase, *pItbBase;
    int i;
    BERR_Code err = BERR_SUCCESS;

    err = BMEM_ConvertOffsetToAddress(hMemory, BREG_Read32( hReg, CtxMap->CDB_Base ), (void **)(void *)&pCdbBase);
    BDBG_ASSERT(err == BERR_SUCCESS);
    BDBG_MSG(("\nCdb Mem ptr = 0x%x, Cdb Phy Addr = 0x%x", (uint32_t)pCdbBase, BREG_Read32( hReg, CtxMap->CDB_Base )));

    err = BMEM_ConvertOffsetToAddress(hMemory, BREG_Read32( hReg, CtxMap->ITB_Base ), (void **)(void *)&pItbBase);
    BDBG_ASSERT(err == BERR_SUCCESS);
    printf("\nItb Mem ptr = 0x%x, Itb Phy Addr = 0x%x", (uint32_t)pItbBase, BREG_Read32( hReg, CtxMap->ITB_Base ));

    printf("\n\nCDB Data:\n");
    for( i = 0; i < 256; i++ )
    {
        if(i%4 == 0)printf("\n");
        printf( "%02hX ", *(pCdbBase+i));
    }
    printf("\n\nITB Data:\n");
    for( i = 0; i < 256; i++ )
    {
        if(i%4 == 0)printf("\n");
        printf( "%02hX ", *(pItbBase+i));
    }
}
#endif
#if RAAGA_TEST_PROFILING_SUPPORT
/* Usage Description:
1. #define RAAGA_TEST_PROFILING_SUPPORT 1
2. Write following code around an API: 

    struct timeval start, stop;
    b_time_get(&start);
	API()
    b_time_get(&stop);
    BDBG_LOG(("\n\n\n Time taken by API is %ld ms \n\n\n", 
        b_time_diff(&stop, &start)));
*/
void
b_time_get(struct timeval *time)
{
	int rc;

	rc = gettimeofday(time, NULL);
	if (rc!=0) {
		BDBG_LOG(("gettimeofday returned %d, ignored", rc));
	}
	return;
}

long
b_time_diff(struct timeval *future, struct timeval *past)
{
	return 1000*(future->tv_sec - past->tv_sec) + (future->tv_usec - past->tv_usec)/1000;
}
#endif /* RAAGA_TEST_PROFILING_SUPPORT */

/* Get a user-configurable setting. This is useful for development. */
const char *bsettop_get_config(const char *name)
{
#if defined(__vxworks) || defined(B_SYSTEM_linuxkernel)
    int i;
    for (i=0;i<g_config_total;i++) {
        if (!strcmp(g_config[i].name, name))
            return g_config[i].value;
    }
    return NULL;
#else
    /* linux user - get a value from the environment */
    return getenv(name);
#endif
}


void TestRegAccessByHost( )
{
    uint32_t ui32RegReserve[30];
    unsigned int i = 0, k = 0;;

    /* Save the original contents */
    ui32RegReserve[i] = BREG_Read32(g_hRegister, BRAAGA_AUD_FMM_BF_CTRL_RINGBUF_0_RDADDR); i++;    
    ui32RegReserve[i] = BREG_Read32(g_hRegister, BRAAGA_AUD_FMM_BF_CTRL_RINGBUF_0_WRADDR); i++;    
    ui32RegReserve[i] = BREG_Read32(g_hRegister, BRAAGA_AUD_FMM_BF_CTRL_RINGBUF_0_BASEADDR); i++;  
    ui32RegReserve[i] = BREG_Read32(g_hRegister, BRAAGA_AUD_FMM_BF_CTRL_RINGBUF_0_ENDADDR); i++;   
    ui32RegReserve[i] = BREG_Read32(g_hRegister, BRAAGA_AUD_FMM_BF_CTRL_RINGBUF_0_FREEFULL_MARK); i++; 
    ui32RegReserve[i] = BREG_Read32(g_hRegister, BRAAGA_AUD_FMM_BF_CTRL_RINGBUF_0_START_WRPOINT); i++; 
    ui32RegReserve[i] = BREG_Read32(g_hRegister, BRAAGA_AUD_FMM_BF_CTRL_RINGBUF_1_RDADDR); i++;    
    ui32RegReserve[i] = BREG_Read32(g_hRegister, BRAAGA_AUD_FMM_BF_CTRL_RINGBUF_1_WRADDR); i++;    
    ui32RegReserve[i] = BREG_Read32(g_hRegister, BRAAGA_AUD_FMM_BF_CTRL_RINGBUF_1_BASEADDR); i++;  
    ui32RegReserve[i] = BREG_Read32(g_hRegister, BRAAGA_AUD_FMM_BF_CTRL_RINGBUF_1_ENDADDR); i++;   
    ui32RegReserve[i] = BREG_Read32(g_hRegister, BRAAGA_AUD_FMM_BF_CTRL_RINGBUF_1_FREEFULL_MARK); i++; 
    ui32RegReserve[i] = BREG_Read32(g_hRegister, BRAAGA_AUD_FMM_BF_CTRL_RINGBUF_1_START_WRPOINT); i++; 
    ui32RegReserve[i] = BREG_Read32(g_hRegister, BRAAGA_AUD_FMM_BF_CTRL_RINGBUF_2_RDADDR); i++;    
    ui32RegReserve[i] = BREG_Read32(g_hRegister, BRAAGA_AUD_FMM_BF_CTRL_RINGBUF_2_WRADDR); i++;    
    ui32RegReserve[i] = BREG_Read32(g_hRegister, BRAAGA_AUD_FMM_BF_CTRL_RINGBUF_2_BASEADDR); i++;  
    ui32RegReserve[i] = BREG_Read32(g_hRegister, BRAAGA_AUD_FMM_BF_CTRL_RINGBUF_2_ENDADDR); i++;   
    ui32RegReserve[i] = BREG_Read32(g_hRegister, BRAAGA_AUD_FMM_BF_CTRL_RINGBUF_2_FREEFULL_MARK); i++; 
    ui32RegReserve[i] = BREG_Read32(g_hRegister, BRAAGA_AUD_FMM_BF_CTRL_RINGBUF_2_START_WRPOINT); i++; 
    ui32RegReserve[i] = BREG_Read32(g_hRegister, BRAAGA_AUD_FMM_BF_CTRL_RINGBUF_3_RDADDR); i++;    
    ui32RegReserve[i] = BREG_Read32(g_hRegister, BRAAGA_AUD_FMM_BF_CTRL_RINGBUF_3_WRADDR); i++;    
    ui32RegReserve[i] = BREG_Read32(g_hRegister, BRAAGA_AUD_FMM_BF_CTRL_RINGBUF_3_BASEADDR); i++;  
    ui32RegReserve[i] = BREG_Read32(g_hRegister, BRAAGA_AUD_FMM_BF_CTRL_RINGBUF_3_ENDADDR); i++;   
    ui32RegReserve[i] = BREG_Read32(g_hRegister, BRAAGA_AUD_FMM_BF_CTRL_RINGBUF_3_FREEFULL_MARK); i++; 
    ui32RegReserve[i] = BREG_Read32(g_hRegister, BRAAGA_AUD_FMM_BF_CTRL_RINGBUF_3_START_WRPOINT); i++; 
    ui32RegReserve[i] = BREG_Read32(g_hRegister, BRAAGA_AUD_FMM_BF_CTRL_RINGBUF_4_RDADDR); i++;    
    ui32RegReserve[i] = BREG_Read32(g_hRegister, BRAAGA_AUD_FMM_BF_CTRL_RINGBUF_4_WRADDR); i++;    
    ui32RegReserve[i] = BREG_Read32(g_hRegister, BRAAGA_AUD_FMM_BF_CTRL_RINGBUF_4_BASEADDR); i++;  
    ui32RegReserve[i] = BREG_Read32(g_hRegister, BRAAGA_AUD_FMM_BF_CTRL_RINGBUF_4_ENDADDR); i++;   
    ui32RegReserve[i] = BREG_Read32(g_hRegister, BRAAGA_AUD_FMM_BF_CTRL_RINGBUF_4_FREEFULL_MARK); i++; 
    ui32RegReserve[i] = BREG_Read32(g_hRegister, BRAAGA_AUD_FMM_BF_CTRL_RINGBUF_4_START_WRPOINT); i++;

    /* Write new values */    
    BREG_Write32(g_hRegister, BRAAGA_AUD_FMM_BF_CTRL_RINGBUF_0_RDADDR, ~k); k++;    
    BREG_Write32(g_hRegister, BRAAGA_AUD_FMM_BF_CTRL_RINGBUF_0_WRADDR, ~k); k++;    
    BREG_Write32(g_hRegister, BRAAGA_AUD_FMM_BF_CTRL_RINGBUF_0_BASEADDR, ~k); k++;  
    BREG_Write32(g_hRegister, BRAAGA_AUD_FMM_BF_CTRL_RINGBUF_0_ENDADDR, ~k); k++;   
    BREG_Write32(g_hRegister, BRAAGA_AUD_FMM_BF_CTRL_RINGBUF_0_FREEFULL_MARK, ~k); k++; 
    BREG_Write32(g_hRegister, BRAAGA_AUD_FMM_BF_CTRL_RINGBUF_0_START_WRPOINT, ~k); k++; 
    BREG_Write32(g_hRegister, BRAAGA_AUD_FMM_BF_CTRL_RINGBUF_1_RDADDR  , ~k); k++;  
    BREG_Write32(g_hRegister, BRAAGA_AUD_FMM_BF_CTRL_RINGBUF_1_WRADDR  , ~k); k++;    
    BREG_Write32(g_hRegister, BRAAGA_AUD_FMM_BF_CTRL_RINGBUF_1_BASEADDR , ~k); k++; 
    BREG_Write32(g_hRegister, BRAAGA_AUD_FMM_BF_CTRL_RINGBUF_1_ENDADDR  , ~k); k++; 
    BREG_Write32(g_hRegister, BRAAGA_AUD_FMM_BF_CTRL_RINGBUF_1_FREEFULL_MARK , ~k); k++;
    BREG_Write32(g_hRegister, BRAAGA_AUD_FMM_BF_CTRL_RINGBUF_1_START_WRPOINT , ~k); k++;
    BREG_Write32(g_hRegister, BRAAGA_AUD_FMM_BF_CTRL_RINGBUF_2_RDADDR    , ~k); k++;
    BREG_Write32(g_hRegister, BRAAGA_AUD_FMM_BF_CTRL_RINGBUF_2_WRADDR    , ~k); k++;
    BREG_Write32(g_hRegister, BRAAGA_AUD_FMM_BF_CTRL_RINGBUF_2_BASEADDR  , ~k); k++;
    BREG_Write32(g_hRegister, BRAAGA_AUD_FMM_BF_CTRL_RINGBUF_2_ENDADDR   , ~k); k++;
    BREG_Write32(g_hRegister, BRAAGA_AUD_FMM_BF_CTRL_RINGBUF_2_FREEFULL_MARK , ~k); k++;
    BREG_Write32(g_hRegister, BRAAGA_AUD_FMM_BF_CTRL_RINGBUF_2_START_WRPOINT , ~k); k++;
    BREG_Write32(g_hRegister, BRAAGA_AUD_FMM_BF_CTRL_RINGBUF_3_RDADDR    , ~k); k++;
    BREG_Write32(g_hRegister, BRAAGA_AUD_FMM_BF_CTRL_RINGBUF_3_WRADDR    , ~k); k++;
    BREG_Write32(g_hRegister, BRAAGA_AUD_FMM_BF_CTRL_RINGBUF_3_BASEADDR  , ~k); k++;
    BREG_Write32(g_hRegister, BRAAGA_AUD_FMM_BF_CTRL_RINGBUF_3_ENDADDR   , ~k); k++;
    BREG_Write32(g_hRegister, BRAAGA_AUD_FMM_BF_CTRL_RINGBUF_3_FREEFULL_MARK , ~k); k++;
    BREG_Write32(g_hRegister, BRAAGA_AUD_FMM_BF_CTRL_RINGBUF_3_START_WRPOINT , ~k); k++;
    BREG_Write32(g_hRegister, BRAAGA_AUD_FMM_BF_CTRL_RINGBUF_4_RDADDR    , ~k); k++;
    BREG_Write32(g_hRegister, BRAAGA_AUD_FMM_BF_CTRL_RINGBUF_4_WRADDR    , ~k); k++;
    BREG_Write32(g_hRegister, BRAAGA_AUD_FMM_BF_CTRL_RINGBUF_4_BASEADDR  , ~k); k++;
    BREG_Write32(g_hRegister, BRAAGA_AUD_FMM_BF_CTRL_RINGBUF_4_ENDADDR   , ~k); k++;
    BREG_Write32(g_hRegister, BRAAGA_AUD_FMM_BF_CTRL_RINGBUF_4_FREEFULL_MARK , ~k); k++;
    BREG_Write32(g_hRegister, BRAAGA_AUD_FMM_BF_CTRL_RINGBUF_4_START_WRPOINT, ~k); k++;

    printf("Register write successful. Check through BBS.\n");
  
    /* Read and dump on screen these registers values */
    k = 0;
    printf("%d -> 0x%x\n",k,  BREG_Read32(g_hRegister, BRAAGA_AUD_FMM_BF_CTRL_RINGBUF_0_RDADDR)); k++;
    printf("%d -> 0x%x\n",k,  BREG_Read32(g_hRegister, BRAAGA_AUD_FMM_BF_CTRL_RINGBUF_0_WRADDR)); k++;    
    printf("%d -> 0x%x\n",k,  BREG_Read32(g_hRegister, BRAAGA_AUD_FMM_BF_CTRL_RINGBUF_0_BASEADDR)); k++;  
    printf("%d -> 0x%x\n",k,  BREG_Read32(g_hRegister, BRAAGA_AUD_FMM_BF_CTRL_RINGBUF_0_ENDADDR)); k++;   
    printf("%d -> 0x%x\n",k,  BREG_Read32(g_hRegister, BRAAGA_AUD_FMM_BF_CTRL_RINGBUF_0_FREEFULL_MARK)); k++; 
    printf("%d -> 0x%x\n",k,  BREG_Read32(g_hRegister, BRAAGA_AUD_FMM_BF_CTRL_RINGBUF_0_START_WRPOINT)); k++; 
    printf("%d -> 0x%x\n",k,  BREG_Read32(g_hRegister, BRAAGA_AUD_FMM_BF_CTRL_RINGBUF_1_RDADDR)); k++;    
    printf("%d -> 0x%x\n",k,  BREG_Read32(g_hRegister, BRAAGA_AUD_FMM_BF_CTRL_RINGBUF_1_WRADDR)); k++;    
    printf("%d -> 0x%x\n",k,  BREG_Read32(g_hRegister, BRAAGA_AUD_FMM_BF_CTRL_RINGBUF_1_BASEADDR)); k++;  
    printf("%d -> 0x%x\n",k,  BREG_Read32(g_hRegister, BRAAGA_AUD_FMM_BF_CTRL_RINGBUF_1_ENDADDR)); k++;   
    printf("%d -> 0x%x\n",k,  BREG_Read32(g_hRegister, BRAAGA_AUD_FMM_BF_CTRL_RINGBUF_1_FREEFULL_MARK)); k++; 
    printf("%d -> 0x%x\n",k,  BREG_Read32(g_hRegister, BRAAGA_AUD_FMM_BF_CTRL_RINGBUF_1_START_WRPOINT)); k++; 
    printf("%d -> 0x%x\n",k,  BREG_Read32(g_hRegister, BRAAGA_AUD_FMM_BF_CTRL_RINGBUF_2_RDADDR)); k++;    
    printf("%d -> 0x%x\n",k,  BREG_Read32(g_hRegister, BRAAGA_AUD_FMM_BF_CTRL_RINGBUF_2_WRADDR)); k++;    
    printf("%d -> 0x%x\n",k,  BREG_Read32(g_hRegister, BRAAGA_AUD_FMM_BF_CTRL_RINGBUF_2_BASEADDR)); k++;  
    printf("%d -> 0x%x\n",k,  BREG_Read32(g_hRegister, BRAAGA_AUD_FMM_BF_CTRL_RINGBUF_2_ENDADDR)); k++;   
    printf("%d -> 0x%x\n",k,  BREG_Read32(g_hRegister, BRAAGA_AUD_FMM_BF_CTRL_RINGBUF_2_FREEFULL_MARK)); k++; 
    printf("%d -> 0x%x\n",k,  BREG_Read32(g_hRegister, BRAAGA_AUD_FMM_BF_CTRL_RINGBUF_2_START_WRPOINT)); k++; 
    printf("%d -> 0x%x\n",k,  BREG_Read32(g_hRegister, BRAAGA_AUD_FMM_BF_CTRL_RINGBUF_3_RDADDR)); k++;    
    printf("%d -> 0x%x\n",k,  BREG_Read32(g_hRegister, BRAAGA_AUD_FMM_BF_CTRL_RINGBUF_3_WRADDR)); k++;    
    printf("%d -> 0x%x\n",k,  BREG_Read32(g_hRegister, BRAAGA_AUD_FMM_BF_CTRL_RINGBUF_3_BASEADDR)); k++;  
    printf("%d -> 0x%x\n",k,  BREG_Read32(g_hRegister, BRAAGA_AUD_FMM_BF_CTRL_RINGBUF_3_ENDADDR)); k++;   
    printf("%d -> 0x%x\n",k,  BREG_Read32(g_hRegister, BRAAGA_AUD_FMM_BF_CTRL_RINGBUF_3_FREEFULL_MARK)); k++; 
    printf("%d -> 0x%x\n",k,  BREG_Read32(g_hRegister, BRAAGA_AUD_FMM_BF_CTRL_RINGBUF_3_START_WRPOINT)); k++; 
    printf("%d -> 0x%x\n",k,  BREG_Read32(g_hRegister, BRAAGA_AUD_FMM_BF_CTRL_RINGBUF_4_RDADDR)); k++;    
    printf("%d -> 0x%x\n",k,  BREG_Read32(g_hRegister, BRAAGA_AUD_FMM_BF_CTRL_RINGBUF_4_WRADDR)); k++;    
    printf("%d -> 0x%x\n",k,  BREG_Read32(g_hRegister, BRAAGA_AUD_FMM_BF_CTRL_RINGBUF_4_BASEADDR)); k++;  
    printf("%d -> 0x%x\n",k,  BREG_Read32(g_hRegister, BRAAGA_AUD_FMM_BF_CTRL_RINGBUF_4_ENDADDR)); k++;   
    printf("%d -> 0x%x\n",k,  BREG_Read32(g_hRegister, BRAAGA_AUD_FMM_BF_CTRL_RINGBUF_4_FREEFULL_MARK)); k++; 
    printf("%d -> 0x%x\n",k,  BREG_Read32(g_hRegister, BRAAGA_AUD_FMM_BF_CTRL_RINGBUF_4_START_WRPOINT)); k++;
    
    printf("Register read successful!!\n");
 
    /* Write back the original values */    
    k = 0;
    BREG_Write32(g_hRegister, BRAAGA_AUD_FMM_BF_CTRL_RINGBUF_0_RDADDR, ui32RegReserve[k]); k++;    
    BREG_Write32(g_hRegister, BRAAGA_AUD_FMM_BF_CTRL_RINGBUF_0_WRADDR, ui32RegReserve[k]); k++;    
    BREG_Write32(g_hRegister, BRAAGA_AUD_FMM_BF_CTRL_RINGBUF_0_BASEADDR, ui32RegReserve[k]); k++;  
    BREG_Write32(g_hRegister, BRAAGA_AUD_FMM_BF_CTRL_RINGBUF_0_ENDADDR, ui32RegReserve[k]); k++;   
    BREG_Write32(g_hRegister, BRAAGA_AUD_FMM_BF_CTRL_RINGBUF_0_FREEFULL_MARK, ui32RegReserve[k]); k++; 
    BREG_Write32(g_hRegister, BRAAGA_AUD_FMM_BF_CTRL_RINGBUF_0_START_WRPOINT, ui32RegReserve[k]); k++; 
    BREG_Write32(g_hRegister, BRAAGA_AUD_FMM_BF_CTRL_RINGBUF_1_RDADDR  , ui32RegReserve[k]); k++;  
    BREG_Write32(g_hRegister, BRAAGA_AUD_FMM_BF_CTRL_RINGBUF_1_WRADDR  , ui32RegReserve[k]); k++;    
    BREG_Write32(g_hRegister, BRAAGA_AUD_FMM_BF_CTRL_RINGBUF_1_BASEADDR , ui32RegReserve[k]); k++; 
    BREG_Write32(g_hRegister, BRAAGA_AUD_FMM_BF_CTRL_RINGBUF_1_ENDADDR  , ui32RegReserve[k]); k++; 
    BREG_Write32(g_hRegister, BRAAGA_AUD_FMM_BF_CTRL_RINGBUF_1_FREEFULL_MARK , ui32RegReserve[k]); k++;
    BREG_Write32(g_hRegister, BRAAGA_AUD_FMM_BF_CTRL_RINGBUF_1_START_WRPOINT , ui32RegReserve[k]); k++;
    BREG_Write32(g_hRegister, BRAAGA_AUD_FMM_BF_CTRL_RINGBUF_2_RDADDR    , ui32RegReserve[k]); k++;
    BREG_Write32(g_hRegister, BRAAGA_AUD_FMM_BF_CTRL_RINGBUF_2_WRADDR    , ui32RegReserve[k]); k++;
    BREG_Write32(g_hRegister, BRAAGA_AUD_FMM_BF_CTRL_RINGBUF_2_BASEADDR  , ui32RegReserve[k]); k++;
    BREG_Write32(g_hRegister, BRAAGA_AUD_FMM_BF_CTRL_RINGBUF_2_ENDADDR   , ui32RegReserve[k]); k++;
    BREG_Write32(g_hRegister, BRAAGA_AUD_FMM_BF_CTRL_RINGBUF_2_FREEFULL_MARK , ui32RegReserve[k]); k++;
    BREG_Write32(g_hRegister, BRAAGA_AUD_FMM_BF_CTRL_RINGBUF_2_START_WRPOINT , ui32RegReserve[k]); k++;
    BREG_Write32(g_hRegister, BRAAGA_AUD_FMM_BF_CTRL_RINGBUF_3_RDADDR    , ui32RegReserve[k]); k++;
    BREG_Write32(g_hRegister, BRAAGA_AUD_FMM_BF_CTRL_RINGBUF_3_WRADDR    , ui32RegReserve[k]); k++;
    BREG_Write32(g_hRegister, BRAAGA_AUD_FMM_BF_CTRL_RINGBUF_3_BASEADDR  , ui32RegReserve[k]); k++;
    BREG_Write32(g_hRegister, BRAAGA_AUD_FMM_BF_CTRL_RINGBUF_3_ENDADDR   , ui32RegReserve[k]); k++;
    BREG_Write32(g_hRegister, BRAAGA_AUD_FMM_BF_CTRL_RINGBUF_3_FREEFULL_MARK , ui32RegReserve[k]); k++;
    BREG_Write32(g_hRegister, BRAAGA_AUD_FMM_BF_CTRL_RINGBUF_3_START_WRPOINT , ui32RegReserve[k]); k++;
    BREG_Write32(g_hRegister, BRAAGA_AUD_FMM_BF_CTRL_RINGBUF_4_RDADDR    , ui32RegReserve[k]); k++;
    BREG_Write32(g_hRegister, BRAAGA_AUD_FMM_BF_CTRL_RINGBUF_4_WRADDR    , ui32RegReserve[k]); k++;
    BREG_Write32(g_hRegister, BRAAGA_AUD_FMM_BF_CTRL_RINGBUF_4_BASEADDR  , ui32RegReserve[k]); k++;
    BREG_Write32(g_hRegister, BRAAGA_AUD_FMM_BF_CTRL_RINGBUF_4_ENDADDR   , ui32RegReserve[k]); k++;
    BREG_Write32(g_hRegister, BRAAGA_AUD_FMM_BF_CTRL_RINGBUF_4_FREEFULL_MARK , ui32RegReserve[k]); k++;
    BREG_Write32(g_hRegister, BRAAGA_AUD_FMM_BF_CTRL_RINGBUF_4_START_WRPOINT, ui32RegReserve[k]); k++;
    
    printf(" TestRegAccessByHost done \n");   
}

void TestDramAccessByHost( )
{
#if (RAAGA_TEST_STATIC_CDB_ITB_BINARYFILE != 1)         
    uint32_t    *pCdbMemPtr = NULL;
    uint32_t    ui32ByteCount = 0;
	uint32_t	data = 0;
    unsigned int uiSize = 0x00505000 - 0x00405000;
#ifdef ENABLE_CACHED_ACCESS_CHANGES
    void        *pCached;
#endif

	pCdbMemPtr = BMEM_AllocAligned(g_hMemory, uiSize, 
                			8, /* uiAlignBits, */
                			0 /* uiBoundary */
                			) ;

#ifdef ENABLE_CACHED_ACCESS_CHANGES
    /* Converting all memory accesses to cached */
    BMEM_ConvertAddressToCached(hMemory, pCdbMemPtr, &pCached);
    *(char *)pCached = 0;
    pCdbMemPtr = pCached;
#endif

	printf("\n Loading CDB Values ....");
    /* Write the CDB */
    for(ui32ByteCount = 0; ui32ByteCount < 0x0044ffff-0x00405000/*cdb_array_size*/; ui32ByteCount = ui32ByteCount+4)
    {
#if RAAGA_TEST_STATIC_CDB_ITB
#if 1 
		*((volatile uint32_t *)pCdbMemPtr + ui32ByteCount) = cdb_array[ui32ByteCount/4;
#else
        data = cdb_array[ui32ByteCount/4];
		data = ((data&0x0000ffff) << 16)|((data&0xffff0000)>>16);
		data = (data&0xffff0000) | ((data&0x0000ff00)>>8) | ((data&0x000000ff)<<8);
		data = (data&0x0000ffff) | ((data&0xff000000)>>8) | ((data&0x00ff0000)<<8);
		*((volatile uint32_t *)pCdbMemPtr + ui32ByteCount) = data;
#endif
#else
        BDBG_ASSERT(0);
#endif /* RAAGA_TEST_STATIC_CDB_ITB */
    }

#ifdef ENABLE_CACHED_ACCESS_CHANGES
    BMEM_FlushCache(hMemory, pCdbMemPtr, 0x0044ffff-0x00405000);
#endif

	printf ("\n Checking Loaded CDB Values Till 80");

	for(ui32ByteCount = 0; ui32ByteCount < 80; ui32ByteCount = ui32ByteCount+4)
	{
		data = *((volatile uint32_t *)pCdbMemPtr + ui32ByteCount);  
		printf ("\n CDB VALUE READ at address %x is %x", (uint32_t)pCdbMemPtr+ui32ByteCount, data);
	}
#endif   
}

#if (RAAGA_TEST_ENABLE_BM_MODE == 1)
void DumpBMReg (
    BREG_Handle hReg
    )
{
	while(1)
			{
                uint32_t regVal;
                uint32_t i;
                uint32_t mydata[512];

                regVal = BREG_Read32( hReg, BCHP_XPT_RAVE_CX0_AV_CDB_READ_PTR);
		        BDBG_LOG(("\t - CDB 0 Read - 0x%08x",regVal));

                regVal = BREG_Read32( hReg, BCHP_XPT_RAVE_CX0_AV_CDB_WRITE_PTR);
		        BDBG_LOG(("\t - CDB 0 Write - 0x%08x",regVal));

                regVal = BREG_Read32( hReg, BCHP_XPT_RAVE_CX0_AV_CDB_BASE_PTR);
		        BDBG_LOG(("\t - CDB 0 Base - 0x%08x",regVal));

                regVal = BREG_Read32( hReg, BCHP_XPT_RAVE_CX0_AV_CDB_END_PTR);
		        BDBG_LOG(("\t - CDB 0 End - 0x%08x",regVal));

                regVal = BREG_Read32( hReg, BCHP_XPT_RAVE_CX0_AV_CDB_VALID_PTR);
		        BDBG_LOG(("\t - CDB 0 Valid - 0x%08x",regVal));

#if 1
                regVal = BREG_Read32( hReg, BCHP_XPT_RAVE_CX0_AV_CDB_BASE_PTR);
                BEMU_Client_ReadMemory(regVal, mydata, 2048);
                for(i=0; i<8; i+=4)
                {
                    BDBG_LOG(("CDB 0x%08x%08x%08x%08x",mydata[i+0], mydata[i+1], mydata[i+2], mydata[i+3]));
                }
#endif

                regVal = BREG_Read32( hReg, BCHP_XPT_RAVE_CX0_AV_ITB_READ_PTR);
		        BDBG_LOG(("\t - ITB 0 Read - 0x%08x",regVal));

                regVal = BREG_Read32( hReg, BCHP_XPT_RAVE_CX0_AV_ITB_WRITE_PTR);
		        BDBG_LOG(("\t - ITB 0 Write - 0x%08x",regVal));

                regVal = BREG_Read32( hReg, BCHP_XPT_RAVE_CX0_AV_ITB_BASE_PTR);
		        BDBG_LOG(("\t - ITB 0 Base - 0x%08x",regVal));

                regVal = BREG_Read32( hReg, BCHP_XPT_RAVE_CX0_AV_ITB_END_PTR);
		        BDBG_LOG(("\t - ITB 0 End - 0x%08x",regVal));

                regVal = BREG_Read32( hReg, BCHP_XPT_RAVE_CX0_AV_ITB_VALID_PTR);
		        BDBG_LOG(("\t - ITB 0 Valid - 0x%08x",regVal));

#if 1
                regVal = BREG_Read32( hReg, BCHP_XPT_RAVE_CX0_AV_ITB_BASE_PTR);
                BEMU_Client_ReadMemory(regVal, mydata, 512);
                for(i=0; i<8; i+=4)
                {
                    BDBG_LOG(("ITB 0x%08x%08x%08x%08x",mydata[i+0], mydata[i+1], mydata[i+2], mydata[i+3]));
                }
#endif
                
				regVal = BREG_Read32( hReg, 0x746030);
				BDBG_LOG(("\t*********** STC = 0x%08x **************",regVal));

				while(0)
				{	
					regVal = BREG_Read32( hReg, 0x746030);
					BREG_Write32(hReg, BCHP_AUD_FMM_MISC_STC_UPPERi_ARRAY_BASE,regVal);
				}
				BKNI_Sleep(5);
			}
}
#endif


/* end of file */

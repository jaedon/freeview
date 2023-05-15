/***************************************************************************
 *     Copyright (c) 2010, Broadcom Corporation
 *     All Rights Reserved
 *     Confidential Property of Broadcom Corporation
 *
 *  THIS SOFTWARE MAY ONLY BE USED SUBJECT TO AN EXECUTED SOFTWARE LICENSE
 *  AGREEMENT  BETWEEN THE USER AND BROADCOM.  YOU HAVE NO RIGHT TO USE OR
 *  EXPLOIT THIS MATERIAL EXCEPT SUBJECT TO THE TERMS OF SUCH AN AGREEMENT.
 *
 * $brcm_Workfile: bhsm_user_cmds.c $
 * $brcm_Revision: Hydra_Software_Devel/3 $
 * $brcm_Date: 12/3/10 11:50a $
 *
 * Module Description: This file contains Broadcom Host Secure Module (OS/platform 
 *                     independent) porting interface public functions.
 *                    			                    
 *
 * Revision History:
 *
 * $brcm_Log: /magnum/portinginterface/hsm/35230/a0/common/bhsm_user_cmds.c $
 * 
 * Hydra_Software_Devel/3   12/3/10 11:50a cdisc
 * SW35230-2490: removing unnecessary BREG_Write when generating random
 * number (HSM)
 * 
 * Hydra_Software_Devel/2   8/20/10 10:02a cdisc
 * SW35230-849: checking in fix
 * 
 * Hydra_Software_Devel/1   7/21/10 3:04p cdisc
 * SW35230-581: adding bhsm_user_cmds for RNG support
 * 
 *
 ***************************************************************************/
#include "bstd.h"
#include "bkni.h"
#include "bdbg_os_priv.h"

#include "bhsm_user_cmds.h"

#include "xpt_security_key_table.h"
#include "chip_secure_35230.h"


BDBG_MODULE(BHSM);

#define MAX_TIMEOUT (5000)

void BHSM_Pack_8BIT_TO_32BIT(uint32_t *p_dest, unsigned char *p_src, unsigned int len);
void BHSM_UnPack_32BIT_TO_8BIT(unsigned char *p_dest, uint32_t *p_src, unsigned int len);
/* #define BDBG_MSG(x) printf x */

BERR_Code BHSM_UserRandomNumber (
		BHSM_Handle					in_handle,
		BHSM_UserRandomNumberIO_t	*inoutp_userRandomNumberIO
)
{
	BERR_Code errCode = BERR_SUCCESS;
	uint32_t regVal = 0; 	/* Parameter length based on Unit32( 32Bit ) */
    /*bool bit_cleared = true;*/
    uint32_t num_32bit_words = 0;
    uint32_t i = 0;
    uint32_t timeout_count = 0;

	BDBG_MSG(("Inside BHSM_UserRandomNumber\n"));
	BDBG_ENTER(BHSM_UserRandomNumber);
	BDBG_ASSERT( in_handle );

	BHSM_P_CHECK_ERR_CODE_CONDITION( errCode, BHSM_STATUS_FAILED, 
		(in_handle->ulMagicNumber != BHSM_P_HANDLE_MAGIC_NUMBER ) );	

    num_32bit_words = inoutp_userRandomNumberIO->unDataLen/4;
    if(inoutp_userRandomNumberIO->unDataLen%4 != 0){
        num_32bit_words += 1;
    }

    BDBG_MSG(("%s - num_32bit_words = '%u'\n", __FUNCTION__, num_32bit_words));
    
    for(i=0; i<num_32bit_words; i++)
    {

        for(timeout_count = 0; timeout_count < MAX_TIMEOUT; timeout_count++)
        {
            regVal = BREG_Read32(in_handle->regHandle, XPT_SECURITY_RNG_STATUS);
            if(regVal & 0x00000001)
            {
                regVal = BREG_Read32(in_handle->regHandle, XPT_SECURITY_RNG_WORD);
                BDBG_MSG(("%s - XPT_SECURITY_RNG_WORD = (0x%08x)\n", __FUNCTION__, regVal));
                BHSM_UnPack_32BIT_TO_8BIT( &inoutp_userRandomNumberIO->aucData[i*4], &regVal, 4);
                break;
            }
        }

        if(timeout_count == MAX_TIMEOUT)
        {
            BDBG_ERR(("%s - Maximum timeout reached for RNG, error.\n", __FUNCTION__, regVal));
            errCode = BHSM_STATUS_FAILED;
            inoutp_userRandomNumberIO->unStatus = 1;
            goto BHSM_P_DONE_LABEL;
        }
    }

    /* Set success code */
    inoutp_userRandomNumberIO->unStatus = 0;
	
BHSM_P_DONE_LABEL:
	
	BDBG_LEAVE(BHSM_UserRandomNumber);
	return( errCode );
}

void BHSM_Pack_8BIT_TO_32BIT(uint32_t *p_dest, unsigned char *p_src, unsigned int len)
{
    unsigned int i;

    if(len==0) return;

    for ( i = 0 ; i < len ; i++ )
    {
        if ( i % UINT32_SIZE_IN_BYTES == 0) *(p_dest + i/UINT32_SIZE_IN_BYTES ) = 0;
        *(p_dest + i /UINT32_SIZE_IN_BYTES )  |=  (*(p_src + i ) << (32 - ( i%UINT32_SIZE_IN_BYTES + 1)*8));    
    }   

}


void BHSM_UnPack_32BIT_TO_8BIT(unsigned char *p_dest, uint32_t *p_src, unsigned int len)
{
    unsigned int i;

    for ( i = 0 ; i < len ; i++ )
        *(p_dest + i )  =  (unsigned char)((*(p_src+i /UINT32_SIZE_IN_BYTES) 
                                        >>  (32 - ( i %UINT32_SIZE_IN_BYTES +1) *8)) & 0xFF) ;

}


/*****************************************************************************
*                  COPYRIGHT (C) 2011 IRDETO B.V.                            *
******************************************************************************
*          File : KeyUpdate.c                                                *
*                                                                            *
*   Description : This takes and RSA encrypted message encrypted under a     *
*                 1024 bit key, and extracts the new 640 bit code download   *
*                 and mux verifier key.  A message version number is used to *
*                 avoid re-using a previously used message, as the process   *
*                 is time consuming.                                         *
*                                                                            *
*****************************************************************************/

#include "LoaderCorePrivate.h"


/* Safe key size for RSA operation */
#define RSA_SMALLEST_KEY_SIZE    (((2 * 0xff + 2) + (0xff + 2)))
#define SAFE_KEY_SIZE   ( (RSA_SMALLEST_KEY_SIZE > SIZE_PUBKEY)? RSA_SMALLEST_KEY_SIZE : SIZE_PUBKEY )

#if (RSA_SMALLEST_KEY_SIZE < SIZE_PUBKEY)
    #error "RSA_SMALLEST_KEY_SIZE can not be smaller than SIZE_PUBKEY"
#endif



#ifdef USEDEVELOPMENTMASTERKEY
static const lc_uchar abMasterKey [] =
{
   0x80, 0x08,
   0x00, 0x00, 0x04, 0x7D, 0xC1, 0xAE, 0x3D, 0x03,
   0x54, 0x59, 0xAF, 0xB7, 0x7C, 0x74, 0x58, 0x8B,
   0xC5, 0xEE, 0x79, 0xBE, 0x40, 0x5C, 0x69, 0xA4,
   0xD3, 0x30, 0x44, 0x8A, 0xD8, 0x76, 0x75, 0x1A,
   0xEB, 0x11, 0xF7, 0x72, 0x0A, 0x09, 0x10, 0x2F,
   0x79, 0x3E, 0x1C, 0xB4, 0xAE, 0x0E, 0xBF, 0x83,
   0x22, 0xC3, 0x29, 0x52, 0xD9, 0xEC, 0xC8, 0x16,
   0xA3, 0xCE, 0xF9, 0xC3, 0x69, 0x90, 0x28, 0x15,
   0x99, 0x57, 0x9D, 0xF1, 0x64, 0xD9, 0x5A, 0xB7,
   0x9B, 0x5C, 0x0D, 0x26, 0x1D, 0xB6, 0x48, 0xA2,
   0xC5, 0x68, 0x16, 0x43, 0x2E, 0xFC, 0xE2, 0x06,
   0x9B, 0x1F, 0x8A, 0xBB, 0x2A, 0xAB, 0xE9, 0x03,
   0xB6, 0xD2, 0xA8, 0x4A, 0xF4, 0xA0, 0xF9, 0x10,
   0x21, 0x2C, 0xE5, 0x2A, 0x7E, 0x79, 0x24, 0xDF,
   0x2F, 0x1E, 0x20, 0xFF, 0x70, 0xBF, 0x01, 0xDE,
   0x1D, 0xCF, 0x4D, 0x56, 0x9F, 0xA7, 0xDF, 0x09,
   0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x05,
};
#else

static const lc_uchar abMasterKey [] =
{
   MASTERMODLEN,
   MASTEREXPLEN,
   /* complement of key modulus */
   0x00, 0x00, 0x0E, 0xB9, 0xF7, 0xFC, 0xD8, 0x8D,
   0x17, 0x55, 0xC3, 0x27, 0x8F, 0x32, 0xE7, 0x8E,
   0xB8, 0xE1, 0xA2, 0x83, 0x1A, 0x6A, 0x32, 0x05,
   0xDD, 0xC3, 0xEA, 0xC4, 0x4B, 0x2E, 0xD3, 0x1C,
   0xC8, 0x2D, 0xE0, 0xE6, 0xB8, 0x20, 0x8C, 0xDC,
   0xC5, 0x8E, 0x8D, 0xA2, 0x06, 0x42, 0x3C, 0x1E,
   0xC8, 0x2D, 0x2E, 0x63, 0x9B, 0x41, 0x83, 0xB3,
   0x01, 0x8B, 0x05, 0x30, 0x65, 0xAE, 0x8B, 0xC1,
   0x80, 0x65, 0x7F, 0x65, 0x44, 0xEA, 0x76, 0x92,
   0x77, 0x31, 0x08, 0xDE, 0xB0, 0x03, 0x0C, 0x05,
   0x2A, 0x78, 0x82, 0xCC, 0xF7, 0xAB, 0x36, 0x1E,
   0x09, 0x11, 0x5F, 0x7D, 0x72, 0x8D, 0x0D, 0x2C,
   0xA5, 0x2C, 0x09, 0x69, 0xEE, 0xAD, 0x25, 0xF0,
   0xF9, 0xB3, 0x65, 0xE4, 0x70, 0xE9, 0x58, 0x40,
   0x8B, 0x3B, 0x3B, 0xEB, 0x75, 0x0A, 0x16, 0xC6,
   0xB4, 0xB6, 0xE0, 0x87, 0x2E, 0xFD, 0x57, 0xDD,
   /* public key exponent */
   0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x07,
};

#endif


static const lc_uchar abFixedData [] =
{
   0x80,0x11,0x22,0x33,0x44,0x55,0x66,0x77
};



static lc_bool CheckKeyMsg (const lc_uchar pabRsaKey[SIZE_PUBKEY])
{
    lc_bool keyLegal = LC_TRUE;

    if (LC_TRUE == keyLegal)
    {
        if (((pabRsaKey[0] | pabRsaKey[1]) & 0x01) != 0)
        {
            keyLegal = LC_FALSE;
        }
        else if (pabRsaKey[1] > 8)
        {
            keyLegal = LC_FALSE;
        }
#if 0
        else if ((2 + pabRsaKey[0] + pabRsaKey[1]) > SIZE_PUBKEY)
        {
            keyLegal = LC_FALSE;
        }
#endif
        else if ((pabRsaKey[2] & 0x80) != 0)
        {
            keyLegal = LC_FALSE;
        }
        /* ... check that remainder to SIZE_PUBKEY is filled with 0xA5 */
        else
        {
        }
    }

    return keyLegal;
}

static lc_result UkCheckValid(const lc_key_update_message *pUkData)
{
    lc_result result = LC_FAILURE;

    if ((LC_memcmp(pUkData->abFixed, abFixedData, SIZE_FIXED) == 0)
        && (LC_memcmp(pUkData->abClearControl, pUkData->abFullControl, SIZE_CLEARCONTROL) == 0)
        && CheckKeyMsg(pUkData->abPubKey) )
    {
        result = LC_SUCCESS;
    }

    return result;
}

/*****************************************************************************
*                                                                            *
*       ROUTINE : DecryptUK                                        *
*                                                                            *
*   DESCRIPTION : This routine authenticates a message using the MasterKey   *
*                 embedded in the loader image.                              *
*                                                                            *
*         INPUT : pabMsg - the message to authenticate                       *
*                                                                            *
*       RETURNS : LC_TRUE if message is authenticated, else LC_FALSE.              *
*                                                                            *
*        Author : Jacobus Ellis                                              *
*                                                                            *
*  Last Changed : 23 Jan 1998 - RLS                                          *
*                 Modified for PC Card loader.                               *
*                 13 Jan 2005 - JE                                           *
*                 Modified for IP/Module loader core.                        *
*                                                                            *
*****************************************************************************/
static lc_result DecryptUK (
    lc_key_update_message* pabAuthed,
    const lc_uchar* pabMsg,
    lc_uint32 abMsgLength)
{

    lc_result result = LC_SUCCESS;
    lc_uchar *psMsg = LC_NULL;
    lc_uchar *psMsgRsaOperate = LC_NULL;
    lc_key_update_message *pMsgParsed = LC_NULL;
    lc_uchar *pMsk = LC_NULL;

    if (LC_NULL == pabMsg ||
        abMsgLength < SIZE_CLEARCONTROL + SIZE_FIXED + SIZE_NEWCONTROL + SIZE_PUBKEY)
    {
        result = LC_ERROR_INVALID_PARAMETER;
    }

    if (LC_SUCCESS == result)
    {
        psMsg = (lc_uchar *)LC_MALLOC(abMsgLength);
        if (LC_NULL == psMsg)
        {
            result = LC_ERROR_INSUFFICIENT_BUFFER;
        }
    }

    if (LC_SUCCESS == result)
    {
        pMsgParsed = LC_MALLOC(sizeof(lc_key_update_message));
        if (LC_NULL == pMsgParsed)
        {
            result = LC_ERROR_INSUFFICIENT_BUFFER;
        }
    }

    if (LC_SUCCESS == result)
    {
        pMsk = LC_MALLOC(SAFE_KEY_SIZE);
        if (LC_NULL == pMsk)
        {
            result = LC_ERROR_INSUFFICIENT_BUFFER;
        }
    }

    if (LC_SUCCESS == result)
    {
        LC_memcpy(psMsg, pabMsg, abMsgLength);
        psMsgRsaOperate = (lc_uchar *)psMsg;
        /* decrypt UK data with master key hardcoded */
        LC_memcpy(pMsk, abMasterKey, sizeof(abMasterKey));
        RSA_Operation(&psMsgRsaOperate[SIZE_CLEARCONTROL], pMsk);

        /* parse */
        LC_memcpy(pMsgParsed->abClearControl, &psMsg[0], SIZE_CLEARCONTROL);
        LC_memcpy(pMsgParsed->abFixed, &psMsg[SIZE_CLEARCONTROL], SIZE_FIXED);
        LC_memcpy(pMsgParsed->abFullControl, &psMsg[SIZE_CLEARCONTROL + SIZE_FIXED], SIZE_NEWCONTROL);
        LC_memcpy(pMsgParsed->abPubKey, &psMsg[SIZE_CLEARCONTROL + SIZE_FIXED + SIZE_NEWCONTROL], SIZE_PUBKEY);

        /* check if UK information corrupted */
        result = UkCheckValid(pMsgParsed);

        if (LC_SUCCESS == result)
        {
            /* if pointer of Authentication result not equals NULL, copy them back */
            if (LC_NULL != pabAuthed )
            {
                LC_memcpy(pabAuthed, pMsgParsed, sizeof(lc_key_update_message));
            }

        }
    }

    LC_Free_Memory((void **)&pMsk);
    LC_Free_Memory((void **)&pMsgParsed);
    LC_Free_Memory((void **)&psMsg);

    return result;

} /* DecryptUK */




lc_result LC_AuthenticateUK (const lc_uchar* pabMsg, lc_uint32 wMsgLen, lc_bool *pIsAManKey)
{

    lc_uint32 errCode = ERROR_CODE_SUCCESS;
    lc_result result = LC_SUCCESS;
    lc_key_update_message *pMsgUkDecrypted = LC_NULL;
    lc_key_update_message *pExistingUkClear = LC_NULL;
    /* system ID and key version (to be updated) retrieved from module */
    lc_uint16 sysIdNow = 0;
    lc_uint16 keyVerNow = 0;
    /* system ID and key version (for authentication use only) retrieved from Message */
    lc_uint16 sysIdMsgClear = 0;
    lc_uint16 keyVerMsgClear = 0;
    /* system ID and key version (to be updated) retrieved from Message */
    lc_uint16 sysIdMsgFull = 0;
    lc_uint16 keyVerMsgFull = 0;
    /* minimum new version and maximum increment acceptable for update */
    lc_uint16 minVer = 0;
    lc_uint16 maxInc = 0;


    if (wMsgLen < SIZE_CLEARCONTROL + SIZE_FIXED + SIZE_NEWCONTROL + SIZE_PUBKEY)
    {
        result = LC_FAILURE;
        errCode = ERROR_CODE_DL_KEY_CORRUPTED;
    }

    /* Try to get and decrypt existing UK. */
    if (LC_SUCCESS == result)
    {
        pExistingUkClear = LC_MALLOC(sizeof(lc_key_update_message));
        if (LC_NULL == pExistingUkClear)
        {
            result = LC_ERROR_INSUFFICIENT_BUFFER;
        }
    }

    if (LC_SUCCESS == result)
    {
        result = LC_GetClearKeyData(pExistingUkClear);
    }


    /* Set parameters for authentication */

    /* If UK module exists and has been decrypted. Get system ID and key version from
     * abFullControl field for authentication. */
    if (LC_SUCCESS == result)
    {

        sysIdNow = LC_GetUint16(pExistingUkClear->abFullControl, 4);
        keyVerNow = LC_GetUint16(pExistingUkClear->abFullControl, 6);
        minVer = keyVerNow + 1; /* Key version must at least increase 1. */
        maxInc = DEFAULT_MAX_INCREMENT_FOR_KEY_VERSION;
    }
    /* If UK module does not exist(e.g., a virgin device) or has been corrupted, we assume this device
     * is a virgin one, and set parameters to default so that this device can only update Manufacturer Key. */
    else
    {
        /* The device shall assume system ID = 0xffff if UK not exists */
        sysIdNow = 0xFFFF;
        minVer = 0;
        maxInc = 1;                    /* Only one key version is acceptable */

        /* Not an error in this case. */
        result = LC_SUCCESS;
    }

    LC_Free_Memory((void **)&pExistingUkClear);

    /* Do authentication on UK. */

    /* Step 1: System ID and "key sequence number" */
    if (LC_SUCCESS == result)
    {

        if (LC_SUCCESS == result)
        {
            /* check if UK should be updated */
            sysIdMsgClear = LC_GetUint16(pabMsg, 0);
            keyVerMsgClear = LC_GetUint16(pabMsg, 2);
        }

        /* Check System ID, the System ID derived from Clear Field must be equal to that of the current key */
        if (LC_SUCCESS == result)
        {
            if (sysIdNow != sysIdMsgClear)
            {
                result = LC_ERROR_INVALID_KEY;
                errCode = ERROR_CODE_DL_KEY_INCORRECT_SYS_ID;
            }
        }

        /* Check "key version "? */
        if (LC_SUCCESS == result)
        {
            if (((keyVerMsgClear - minVer) & 0xFFFF) > maxInc)
            {
                result = LC_ERROR_INVALID_KEY;
                errCode = ERROR_CODE_DL_KEY_INCORRECT_KEY_VER;
            }
        }
    }

    /* Step 2: and Key version match? */
    if (LC_SUCCESS == result)
    {
        pMsgUkDecrypted = LC_MALLOC(sizeof(lc_key_update_message));
        if (LC_NULL == pMsgUkDecrypted)
        {
            result = LC_ERROR_INSUFFICIENT_BUFFER;
        }

        /* Check if UK information distorted */
        if (LC_SUCCESS == result)
        {
            result = DecryptUK(pMsgUkDecrypted, pabMsg, wMsgLen);
            if (LC_SUCCESS != result)
            {
                errCode = ERROR_CODE_DL_KEY_CORRUPTED;
            }
        }

        if (LC_SUCCESS == result)
        {
            /* Authentication OK */
            sysIdMsgFull = LC_GetUint16(pMsgUkDecrypted->abFullControl, 4);
            keyVerMsgFull = LC_GetUint16(pMsgUkDecrypted->abFullControl, 6);

            /* Some Type Key update(e.g, DEV-KEY -> DEV-KEY), ought to check key version. */
            if (sysIdMsgFull == sysIdNow)
            {
                if ( (keyVerMsgFull <= keyVerNow) ||
                   (((keyVerMsgFull - keyVerNow) & 0xFFFF) > maxInc) )
                {
                    result = LC_ERROR_INVALID_KEY;
                    errCode = ERROR_CODE_DL_KEY_OLD_KEY_VER;
                }

            }

        }

        LC_Free_Memory((void **)&pMsgUkDecrypted);
    }

    /* Is it a Manufacturer Key? */
    if (LC_SUCCESS == result)
    {
        *pIsAManKey = (SYS_ID_MANKEY == sysIdMsgFull)? LC_TRUE : LC_FALSE;
    }

    /* Report Errorcode */
    ERR_REPORT_SetErrorCode(errCode);

    return result;
}

lc_result LC_RetrieveUK(lc_uchar** ppUkData, lc_uint32* pUkLength)
{
    lc_result result = LC_SUCCESS;
    lc_uint32 errCode = ERROR_CODE_SUCCESS;
    lc_buffer_st mp_uk = {LC_NULL, 0};
    lc_uchar* pAuthKey = LC_NULL;
    lc_key_update_message *pMsgParsed = LC_NULL;

    if (LC_NULL == ppUkData)
    {
        result = LC_ERROR_NULL_PARAM;
    }

    if (LC_SUCCESS == result)
    {
        pMsgParsed = LC_MALLOC(sizeof(lc_key_update_message));
        if (LC_NULL == pMsgParsed)
        {
            result = LC_ERROR_INSUFFICIENT_BUFFER;
        }
    }

    /* read UK from UK module */
    if (LC_SUCCESS == result)
    {
        result = LC_GetClearKeyData(pMsgParsed);
        if (LC_SUCCESS != result)
        {
            errCode = ERROR_CODE_RESIDENT_KEY_INVALID;
        }
    }

    if (LC_SUCCESS == result)
    {
        if (LC_SUCCESS == result)
        {
            pAuthKey = (lc_uchar*)LC_MALLOC(SAFE_KEY_SIZE);
            if (LC_NULL == pAuthKey)
            {
                result = LC_ERROR_INSUFFICIENT_BUFFER;
            }
        }

        /* return UK data and length */
        if (LC_SUCCESS == result)
        {
            /* copy authenticated key to user pointer */
            LC_memset(pAuthKey, 0, SAFE_KEY_SIZE);                   /* clear output buffer */
            LC_memcpy(pAuthKey, pMsgParsed->abPubKey, SIZE_PUBKEY); /* write UK pub */
            *ppUkData = pAuthKey;

            /* return authenticated key length */
            if (LC_NULL != pUkLength)
            {
                *pUkLength = SIZE_PUBKEY;
            }
        }
    }

    LC_Free_Memory((void **)&pMsgParsed);
    LC_Free_Memory((void **)&mp_uk.bytes);

    /* Report Errorcode */
    ERR_REPORT_SetErrorCode(errCode);

    return result;
}

lc_result LC_CheckMasterKey(void)
{
    lc_result result = LC_SUCCESS;

    if (LC_TRUE != CheckKeyMsg(abMasterKey))
    {
        result = LC_ERROR_INVALID_KEY;
    }
    return result;
}

lc_result LC_GetClearKeyData (lc_key_update_message* pClearKeyData)
{
    lc_result result = LC_SUCCESS;
    lc_uint32 errCode = ERROR_CODE_SUCCESS;
    lc_buffer_st mp_uk = {LC_NULL, 0};


    /* check & read UK from UK module */
    if( LC_SUCCESS == result )
    {
        lc_bbcb_st bbcb;
        result = BBCB_GetBBCB(&bbcb, LC_NULL);
        if( LC_SUCCESS != result)
        {
            errCode = ERROR_CODE_INVALID_BBCB;
            TRACE_LOG0(TRACE_ERROR, ("&[KeyUpdate] Error: Failed to retrieve BBCB!\n"));
        }
        else
        {
            result = LC_ReadAndVerifyExistingModule(
                                MODULE_DRV_ID_UK,
                                LC_FALSE,
                                bbcb.boot_check_algorithm,
                                LC_NULL,
                                LC_NULL,
                                LC_NULL,
                                &mp_uk );
        }
    }

    if (LC_SUCCESS == result)
    {
        /* decrypt UK and replace it with the clear one */
        result = DecryptUK( pClearKeyData, mp_uk.bytes, mp_uk.length);
        if (LC_SUCCESS != result)
        {
            errCode = ERROR_CODE_RESIDENT_KEY_CORRUPT;
        }
    }

    LC_Free_Memory((void **)&mp_uk.bytes);

    /* Report Errorcode */
    ERR_REPORT_SetErrorCode(errCode);

    return result;
}



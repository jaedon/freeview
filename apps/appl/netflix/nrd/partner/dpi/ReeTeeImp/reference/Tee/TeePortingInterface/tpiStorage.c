/*
 * tpiStorage.c
 *
 *  Created on: Jan 31, 2014
 *      Author: scott
 */
/*
 * (c) 1997-2015 Netflix, Inc.  All content herein is protected by
 * U.S. copyright and other applicable intellectual property laws and
 * may not be copied without the express permission of Netflix, Inc.,
 * which reserves all rights.  Reuse of any of this content for any
 * purpose without the permission of Netflix, Inc. is strictly
 * prohibited.
 */

/*
 * This is our reference implementation of the storage TPI code for the NRD
 * ref app. If you are porting the SDK to another platform (e.g. a game console), then
 * you'll need to replace this file with your own storage TPI implementation. See
 * TEE/include/StorageAgentTPI.h for the interface definitions. At a high level, this
 * file needs to provide functions to populate saManufSSTable[]. We currently assume
 * the following:
 *
 * - an encrypted and integrity-protected "manufacturing secure store" was injected
 *   during manufacturing
 *
 * - we have the keys to authenticate/decrypt it (#defined in tpiStorage.h, but in a real
 *   implementation, you would either store these keys in OTP, derive them from something
 *   in OTP, or store them in NAND, encrypted with a ROM/OTP key)
 *
 * - that we have the ability to create client-unique encryption/authentication
 *   keys based on the clientID of the requester (used for protect/unprotect operations)
 */
#include <stdio.h>
#include <stdarg.h>
#include <ctype.h>
#include <unistd.h>
#include <stdint.h>
#include <stdlib.h>
#include <assert.h>
#include <string.h>
#include <strings.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <arpa/inet.h>
#include "DebugTypes.h"
#include "CommTypes.h"
#include "StorageTypes.h"
#include "TeeInternalDpi.h"
#include "TeeDebug.h"
#include "StorageAgent.h"
#include "StorageAgentTPI.h"
#include "b64.h"
#define nfSS_NEED_KEYS
#include "tpiStorage.h"
#undef nfSS_NEED_KEYS

#if defined(HUMAX_PLATFORM_BASE)
void* DI_MI_DECRYPTOR_SOC_Malloc(unsigned int unBytes);
int DI_MI_DECRYPTOR_SOC_OTPKEY_DoEncrypt(unsigned char *in, unsigned char *out, unsigned int size);
int DI_MI_DECRYPTOR_SOC_OTPKEY_DoDecrypt(unsigned char *in, unsigned char *out, unsigned int size);
void DI_MI_DECRYPTOR_SOC_Free(void *pMemory);
#endif

// internal buffer for holding manuf secure store in memory
static uint8_t    _saMemBuf[nfSS_MAX_FILE_SIZE];



/*
 * These will refer to the in-memory manufacturing secure store data
 */
static uint8_t *saManufSSDataPtr = NULL;
static uint32_t saManufSSDataLength = 0;

static uint8_t saLocalKPWStorage[16];

/*
 * This is the key ladder base for application services offered by storageAgent. We
 * use this, together with our secure store keys, to derive encryption and HMAC keys
 * for each application. Note that we hold this key in memory, but it could also
 * be loaded directly into a crypto processor.
 *
 * In a "real" implementation, this key would be directly loaded from OTP, or
 * derived from a device-unique key that is loaded from OTP. It is very important
 * that this key be device-unique. Otherwise, it would be possible to migrate an
 * application secure store from one device to another.
 *
 * For our reference implementation, we are hard-coding this, but you would never
 * do this in a real implementation.
 */
uint8_t saKeyLadderBase[] = {0xFF,0xFE,0xFD,0xFC,0xFB,0xFA,0xF0,0x0F,
                             0xAF,0xBF,0xCF,0xDF,0xEF,0xFF,0x00,0x01};

/*
 * Our implementation of storageAgent has its own device unique keys. In a "real"
 * implementation, we would probably re-name these variables to something less obvious, and
 * consider obfuscating the fact that these are crypto keys. Reverse engineers load binaries
 * into tools like IDA Pro, and then look for informative variable names like these. So at
 * minimum, you should strip your binaries.
 */
uint8_t saInternalAesKey[saAES_128_KEY_SIZE_BYTES];
uint8_t saInternalHMACKey[saHMAC_SHA256_KEY_SIZE_BYTES];

/*
 *  We use the client ID to limit access to data items. Since the TEE comm agent
 *  controls the client ID, we can trust it to accurately report the client ID to
 *  us. Even if the REE is compromised, it cannot spoof a client ID.
 *
 *  We have one special client id (all 0x00); for an internal "all-access" pass, use
 *  this client id. This may be removed in a future release, but is currently here for
 *  utility.
 */
static nfTeeClientId_t saMgmtClientId = nfSS_CLIENT_ID_TEE_MGMT;
static nfTeeClientId_t cryptoAgentClientId = nfSS_CLIENT_ID_TEE_NTBA_RO;

#if defined(HUMAX_NRDP_4_2_0)
/*
 * We can't assume that we have dynamic memory allocation capability (e.g. malloc),
 * so we pre-allocated a work buffer for crypto operations. If your implementation
 * supports malloc, you could use dynamic allocation instead.
 */
static uint8_t WorkBuf[nfTEE_MAX_INTERNAL_SIZE];
#endif /* Removed at NRDP-4.2.1 */

/*
 * The following definition relates to how we protect/unprotect data. "Protect" is
 * our name for the act of providing client "secure store" services. For example, the
 * NRD application needs to protect data at rest (e.g. MSL tokens, the UI trust store, etc.)
 *
 * We use a proprietary "wrapper" for this data. Version is prepended to protected data so
 * that we can apply proper algorithms, formatting, etc. when unprotecting. If we don't recognize
 * (or implement) the version, we won't attempt to decrypt the data.
 */
static uint32_t    saStorageVersion = 0x10000001;
static uint32_t    saStorageVersionLE = 0x01000010;


/*
 * Most of the code in this file is about deserialization. For that to be performed properly,
 * it is helpful to define appropriate macros to consume byte arrays and certain basic types.
 */
#define SA_CONSUME_BYTES( p_, sz_, n_, b_ ) \
    do { \
        if ( (n_) > (sz_) ) { \
            STORAGEDBG("%s: failed to read %d bytes (%d remaining)\n", __FUNCTION__, (int)(n_), (int)(sz_)); \
            goto parseError; \
        } \
        \
        teeMemcpy( (b_), (p_), (n_) ); \
        \
        (p_) += (n_); \
        (sz_) -= (n_); \
    } while (0)

/* consume bytes not by copy, but by pointing to bytes in question */
#define SA_CONSUME_BYTES_PTR( p_, sz_, n_, pb_ ) \
    do { \
        if ( (n_) > (sz_) ) { \
            STORAGEDBG("%s: failed to read %d bytes (%d remaining)\n", __FUNCTION__, (int)(n_), (int)(sz_)); \
            goto parseError; \
        } \
        \
        (pb_) = (p_); \
        \
        (p_) += (n_); \
        (sz_) -= (n_); \
    } while (0)

#define SA_CONSUME_WORD32( p_, sz_, w32_ ) \
    do { \
        if ( (sz_) < sizeof(uint32_t) ) { \
            STORAGEDBG("%s: failed to read 32-bit word (%d bytes remaining)\n", __FUNCTION__, (int)(sz_)); \
            goto parseError; \
        } \
        \
        nfGETWORD32((p_), (w32_)); \
        \
        (p_) += sizeof(uint32_t); \
        (sz_) -= sizeof(uint32_t); \
    } while (0)

#define SA_CONSUME_WORD16( p_, sz_, w16_ ) \
    do { \
        if ( (sz_) < sizeof(uint16_t) ) { \
            STORAGEDBG("%s failed to read 16 bit word (%d bytes remaining)\n", __FUNCTION__, (int)(sz_)); \
            goto parseError; \
        } \
        \
        nfGETWORD16((p_), (w16_)); \
        \
        (p_) += sizeof(uint16_t); \
        (sz_) -= sizeof(uint16_t); \
    } while (0)

#define SA_PRODUCE_BYTES( p_, sz_, n_, b_ ) \
    do { \
        if ( (sz_) < (n_) ) { \
            STORAGEDBG("%s: failed to write %d bytes (%d remaining)\n", __FUNCTION__, (int)(n_), (int)(sz_)); \
            goto serializeError; \
        } \
        \
        teeMemcpy( (p_), (b_), (n_) ); \
        \
        (p_) += (n_); \
        (sz_) -= (n_); \
    } while (0)

#define SA_PRODUCE_WORD32( p_, sz_, w32_ ) \
    do { \
        if ( (sz_) < sizeof(uint32_t) ) { \
            STORAGEDBG("%s: failed to write 32 bit word (%d bytes remaining)\n", __FUNCTION__, (int)(sz_)); \
            goto serializeError; \
        } \
        \
        nfPUTWORD32((p_), (w32_)); \
        \
        (p_) += sizeof(uint32_t); \
        (sz_) -= sizeof(uint32_t); \
    } while (0)

/*********************************************************************
 * @function ProcessClientSection
 *
 * @brief Load client section into runtime secure store
 *
 * @param[in] SectionID - ID of this section (we'll map to ClientID)
 * @param[in] BufPtr - pointer to to buffer, *BufPtr is pointing to padding length
 * @param[in] BufLength - current buffer length
 *
 * @returns 0 upon success, -1 otherwise
 *
 *********************************************************************/
static int ProcessClientSection( uint8_t **p_data, uint16_t *p_remaining_bytes, uint16_t sectionLength, nfSectionID_t SectionID )
{
    (void)SectionID;
    uint8_t *p = *p_data;
    uint16_t remaining_bytes = *p_remaining_bytes;
    uint16_t remaining_bytes_in_section = sectionLength;

    /* match structure of SA_CONSUME_* macros:
     * 1. check enough space
     */
    if ( sectionLength > remaining_bytes )
    {
        STORAGEDBG("%s: failed to read %d bytes (%d remaining)\n", __FUNCTION__, sectionLength, remaining_bytes);
        goto parseError;
    }

    /*
     * This section looks like this:
     *
     *       <key><length><data>
     *       <key><length><data>
     *              :
     *       <key><length><data>
     *     ---------------------------------------------------------------------
     */

    STORAGEDBG("%s: called with SectionID %08x, remaining_bytes %d\n", __FUNCTION__, SectionID, remaining_bytes );
//    teePrintHexDump( BufPtr, remaining_bytes );

    do
    {
        nfSecureStoreClientKey_t Key;
        uint16_t DataLength;
        void *DataPtr;

        /*
         * We have a key-length-data record, with variable-length data. We need
         * read the length, and allocate a variable-sized section data structure
         * to hold this record
         */
        SA_CONSUME_WORD32( p, remaining_bytes_in_section, Key );

        SA_CONSUME_WORD16( p, remaining_bytes_in_section, DataLength );

        SA_CONSUME_BYTES_PTR( p, remaining_bytes_in_section, DataLength, DataPtr );

        /*
         * Remember, each table row looks like this:
         *
         *     typedef struct
         *     {
         *        nfSecureStoreClientKey_t    ClientKey;
         *        nfTeeClientId_t            *ClientIdPtr;
         *        uint32_t                    DataLength;
         *        uint8_t                    *DataPtr;
         *    } nfSecureStoreTableRow_t;
         *
         * Here, we are updating DataLength and DataPtr for individual elements.
         * Look at the key to see which element to update.
         */

        switch ( Key )
        {
            // save length and pointer to data
            case ntbaAGENT_KEY_ESN:
                saManufSSTable[nfSSRI_ESN].DataLength = DataLength;
                saManufSSTable[nfSSRI_ESN].DataPtr = DataPtr;
                STORAGEDBG("%s: got pointer to %d bytes of ESN at %p\n", __FUNCTION__, DataLength, DataPtr);
                break;
            case ntbaAGENT_KEY_KPE:
                saManufSSTable[nfSSRI_KPE].DataLength = DataLength;
                saManufSSTable[nfSSRI_KPE].DataPtr = DataPtr;
                STORAGEDBG("%s: got pointer to %d bytes of KPE at %p\n", __FUNCTION__, DataLength, DataPtr);
                break;
            case ntbaAGENT_KEY_KPH:
                saManufSSTable[nfSSRI_KPH].DataLength = DataLength;
                saManufSSTable[nfSSRI_KPH].DataPtr = DataPtr;
                STORAGEDBG("%s: got pointer to %d bytes of KPH at %p\n", __FUNCTION__, DataLength, DataPtr);
                break;
            case drmAGENT_KEY_MODEL_PRIV:
                saManufSSTable[nfSSRI_DRM_GROUP_PRIV].DataLength = DataLength;
                saManufSSTable[nfSSRI_DRM_GROUP_PRIV].DataPtr = DataPtr;
                STORAGEDBG("%s: got pointer to %d bytes of GROUP_PRIV at %p\n", __FUNCTION__, DataLength, DataPtr);
                break;
            case drmAGENT_KEY_MODEL_CERT:
                saManufSSTable[nfSSRI_DRM_GROUP_CERT].DataLength = DataLength;
                saManufSSTable[nfSSRI_DRM_GROUP_CERT].DataPtr = DataPtr;
                STORAGEDBG("%s: got pointer to %d bytes of GROUP_CERT at %p\n", __FUNCTION__, DataLength, DataPtr);
                break;
            case drmAGENT_KEY_DEVCERT_TEMPLATE:
                saManufSSTable[nfSSRI_DRM_CERTTEMPLATE].DataLength = DataLength;
                saManufSSTable[nfSSRI_DRM_CERTTEMPLATE].DataPtr = DataPtr;
                STORAGEDBG("%s: got pointer to %d bytes of CERT_TEMP at %p\n", __FUNCTION__, DataLength, DataPtr);
                break;
            default:
                break;
        }/* end switch( Key ) */

    } while ( remaining_bytes_in_section > 0 );

    if (0 != remaining_bytes_in_section) {
        STORAGEDBG("%s: failed to consume all bytes in section %08x (%d remaining)\n", __FUNCTION__, SectionID, remaining_bytes_in_section);
        return -1;
    }
    remaining_bytes -= sectionLength;

    // we're done...
    *p_data = p;
    *p_remaining_bytes = remaining_bytes;

    return 0;

// NOT REACHED
parseError:
    return -2;
}/* end ProcessClientSection */

/*********************************************************************
 * @function ProcessMgmtSection
 *
 * @brief Load mgmt section into runtime secure store
 *
 * @param[in] BufPtr - pointer to to buffer, *BufPtr is pointing to padding length
 * @param[in] BufLength - current buffer length
 *
 * @returns 0 upon success, -1 otherwise
 *
 *********************************************************************/
static int ProcessMgmtSection( uint8_t **p_data, uint16_t *p_remaining_bytes, uint16_t sectionLength )
{
    uint8_t *p = *p_data;
    uint16_t remaining_bytes = *p_remaining_bytes;
    uint16_t remaining_bytes_in_section = sectionLength;

    /* match structure of SA_CONSUME_* macros:
     * 1. check enough space
     */
    if ( sectionLength > remaining_bytes )
    {
        STORAGEDBG("%s: failed to read %d bytes (%d remaining)\n", __FUNCTION__, sectionLength, remaining_bytes);
        goto parseError;
    }

    /* 2. process data */
    /*
     * This record looks like this:
     *
     * TEE MGMT Section ID                 4 bytes
     * section length                      2 bytes
     * NTBA RO section ID                  4 bytes
     * NTBA RO client ID                  16 bytes
     * NTBA RW section ID                  4 bytes
     * NTBA RW client ID                  16 bytes
     * DRM RO section ID                   4 bytes
     * DRM RO client ID                   16 bytes
     * DRM RW section ID                   4 bytes
     * DRM RW client ID                   16 bytes
     * NRD section ID                      4 bytes
     * NRD client ID                      16 bytes
     *
     * When this function is called, the TEE MGMT section ID and section
     * length have already been extracted, and *p_data is pointing at
     * first section ID
     *
     * We want to save pointers to the client IDs in saManufSSTable[]
     */

    STORAGEDBG("%s: called with %d-byte buffer\n", __FUNCTION__, remaining_bytes );
//    teePrintHexDump( p, remaining_bytes );

    /*
     * We've already stripped the section ID, section length, padding length,
     * padding, and MAC, so what we have are a series of section/client
     * ID pairs. We will take these and construct the list of section
     * headers. See secstoreinternals.h for the secStoreSectionType
     * definition -- that is the struct we use to create the list.
     */
    do
    {
        uint32_t tmp32;
        nfSectionID_t SectionID;
        void *ClientIdPtr;

        // SectionID
        SA_CONSUME_WORD32( p, remaining_bytes_in_section, tmp32 );
        SectionID = tmp32;
        STORAGEDBG("%s: SectionID is %08x, p is %p\n", __FUNCTION__, SectionID, p );

        SA_CONSUME_BYTES_PTR( p, remaining_bytes_in_section, sizeof(nfTeeClientId_t) , ClientIdPtr );

        // we're looking for specific sections
        switch ( SectionID )
        {

            case teeSID_NTBA_RO:
                /*
                 * ESN, KPE, KPH, KPW belong to this client ID; set pointers in table, each table
                 * row looks like this:
                 *
                 * typedef struct
                 * {
                 *   nfSecureStoreClientKey_t    ClientKey;
                 *   nfTeeClientId_t             ClientId;
                 *   uint32_t                    DataLength;
                 *   uint8_t                    *DataPtr;
                 * } nfSecureStoreTableRow_t;
                 *
                 */
                saManufSSTable[nfSSRI_ESN].ClientKey = ntbaAGENT_KEY_ESN;
                saManufSSTable[nfSSRI_ESN].ClientIdPtr = ClientIdPtr;
                saManufSSTable[nfSSRI_KPE].ClientKey = ntbaAGENT_KEY_KPE;
                saManufSSTable[nfSSRI_KPE].ClientIdPtr = ClientIdPtr;
                saManufSSTable[nfSSRI_KPH].ClientKey = ntbaAGENT_KEY_KPH;
                saManufSSTable[nfSSRI_KPH].ClientIdPtr = ClientIdPtr;
                STORAGEDBG("%s: got NTBA section, keys/clientid's init'd\n", __FUNCTION__);
                break;
            case teeSID_DRM_RO:
                /*
                 * DRM device cert, model cert, and model priv key belong to this client ID
                 */
                saManufSSTable[nfSSRI_DRM_CERTTEMPLATE].ClientKey = drmAGENT_KEY_DEVCERT_TEMPLATE;
                saManufSSTable[nfSSRI_DRM_CERTTEMPLATE].ClientIdPtr = ClientIdPtr;
                saManufSSTable[nfSSRI_DRM_GROUP_CERT].ClientKey = drmAGENT_KEY_MODEL_CERT;
                saManufSSTable[nfSSRI_DRM_GROUP_CERT].ClientIdPtr = ClientIdPtr;
                saManufSSTable[nfSSRI_DRM_GROUP_PRIV].ClientKey = drmAGENT_KEY_MODEL_PRIV;
                saManufSSTable[nfSSRI_DRM_GROUP_PRIV].ClientIdPtr = ClientIdPtr;
                STORAGEDBG("%s: got DRM section, keys/clientid's init'd\n", __FUNCTION__);
                break;
            case teeSID_NRD_APP1:
                /*
                 * This one is a special case; no data, we just want the NRD client ID
                 */
                saManufSSTable[nfSSRI_NRD_APP_ID].ClientKey = teeAPPID_KEY_NRD;
                saManufSSTable[nfSSRI_NRD_APP_ID].ClientIdPtr = &saMgmtClientId;
                saManufSSTable[nfSSRI_NRD_APP_ID].DataPtr = ClientIdPtr;
                saManufSSTable[nfSSRI_NRD_APP_ID].DataLength = sizeof(nfTeeClientId_t);
                STORAGEDBG("%s: got NRD section, key/clientid init'd\n", __FUNCTION__);
                break;
            default:
                STORAGEDBG("%s: skipping unused section id (%08x)\n", __FUNCTION__, SectionID );
                break;
       }/* end switch ( SectionID ) */

    } while ( remaining_bytes_in_section > 0 );

    if (0 != remaining_bytes_in_section) {
        STORAGEDBG("%s: failed to consume all bytes in management section (%d remaining)\n", __FUNCTION__, remaining_bytes_in_section);
        return -1;
    }
    remaining_bytes -= sectionLength;

    STORAGEDBG("%s: finished decoding mgmt section...\n", __FUNCTION__ );
    STORAGEDBG("%s: ------------------------------------\n", __FUNCTION__ );

    /* 3. update data pointer and remaining bytes */
    *p_data = p;
    *p_remaining_bytes = remaining_bytes;

    return 0;

/* NOT REACHED */
parseError:
    return -2;
}/* end ProcessMgmtSection */

/*********************************************************************
 * @function ProcessSection
 *
 * @brief TEE secure store initialization
 *
 * @param[inout] p_data - pointer buffer at beginning of next section
 *                        updated to point to the following section after consuming this section
 * @param[inout] p_remaining_bytes - remaining data to be processed
 *                                   updated to remaining data size after consuming this section
 *
 * @returns 0 upon success, -1 otherwise
 *
 *********************************************************************/
 static int ProcessSection( uint8_t **p_data, uint16_t *p_remaining_bytes )
 {
    uint8_t                 *p = *p_data;
    uint16_t                remaining_bytes = *p_remaining_bytes;

    uint32_t                sectionID;
    uint16_t                sectionLength;
    int                     rc;

    /*
     * (already decrypted) manufacturing secure store sections look like this:
     *
     * <32-bit unique ID><16-bit section length><data>
     *
     * The unique ID can be used to find the client ID, which can
     * in turn be used to decrypt the data. Each section results in
     * a linked list of section data entries, and this list is in
     * turn linked into the list of sections in the SecStoreContext
     * structure. See secstoreInternals.h for pretty pictures.
     */
    /* section ID */
    SA_CONSUME_WORD32( p, remaining_bytes, sectionID );
    STORAGEDBG("%s: sectionID: %08x\n", __FUNCTION__, sectionID );

    /* section length */
    SA_CONSUME_WORD16( p, remaining_bytes, sectionLength );
    STORAGEDBG("%s: sectionLength is %d, remaining_bytes is %d\n", __FUNCTION__, sectionLength, remaining_bytes);

    /*
     * Okay: now we have a section ID + length. We need to find the client
     * ID (based on section ID), and then derive the client crypto key from
     * that.
     */

    if ( sectionID == teeSID_TEE_MGMT )
    {
        /*
         * This one is a special case: it contains all sectionID to clientID
         * mappings. From this, we'll build our internal map, and this will be
         * used to look up the client ID for other sections. This is encrypted
         * with the first key in the internal TEE key ladder.
         */
        if ( ( rc = ProcessMgmtSection( &p, &remaining_bytes, sectionLength ) ) != 0 )
        {
            STORAGEDBG("%s: ProcessMgmtSection failed, bailing.\n", __FUNCTION__ );
            return -1;
        }
    }
    else
    {
        // this is a non-mgmt section
        STORAGEDBG("%s: calling ProcessClientSection for ID %08x\n", __FUNCTION__, sectionID );
        if ( ( rc = ProcessClientSection( &p, &remaining_bytes, sectionLength, sectionID ) ) != 0 )
        {
            STORAGEDBG("%s: getCkuebtSection failed, bailing.\n", __FUNCTION__ );
            return -1;
        }
    }

    /*
     * If we get here, we succeeded in loading a section; adjust
     * TotalLength and return.
     */
    *p_data = p;
    *p_remaining_bytes = remaining_bytes;

    return 0;

    /* NOTREACHED */
parseError:
    return -2;

 }/* end ProcessSection */

 /*********************************************************************
 * @function saDecryptManufStore
 *
 * @brief Decrypt/validate manufacturing secure store and remove padding
 *
 * @param[inout] p_data - pointer to encrypted data section (will decrypt in place)
 *                        updated to expected beginning of management section
 * @param[inout] p_remaining_bytes - encrypted data section size
 *                                   updated to remaining size of all sections
 *
 * @returns 0 upon success, -1 otherwise
 *
 *********************************************************************/
static int saDecryptManufStore( uint8_t **p_data, uint16_t *p_remaining_bytes )
{
    int  rc;
    uint8_t *data = *p_data;
    uint16_t remaining_bytes = *p_remaining_bytes;
    uint16_t tmp16;

    /* section must at least contain the initial padding size and final integrity value */
    if ( remaining_bytes < sizeof(uint16_t) + saSHA1_HASH_SIZE )
    {
        STORAGEDBG("%s: encrypted data section smaller than expected (%d|%d).\n", __FUNCTION__, remaining_bytes, sizeof(uint16_t) + saSHA1_HASH_SIZE );
        return -1;
    }

    STORAGEDBG("%s: calling decrypt...\n", __FUNCTION__ );
#if defined(HUMAX_PLATFORM_BASE)
	{
		//printf("\n\nDecrypt By DI_MI_XXX\n\n");
		uint8_t *bufferptr = (uint8_t *)DI_MI_DECRYPTOR_SOC_Malloc((uint16_t)remaining_bytes);
		if(bufferptr == NULL)
		{
			STORAGEDBG("%s: (nexus)mem alloc failed!\n", __FUNCTION__ );
	        return -1;
		}
		memset(bufferptr, 0, (uint16_t)remaining_bytes);
		memcpy((void *)bufferptr, (void *)data, (uint16_t)remaining_bytes);
		if(DI_MI_DECRYPTOR_SOC_OTPKEY_DoDecrypt((unsigned char *)bufferptr, (unsigned char *)bufferptr, (unsigned int) remaining_bytes)!=0)
		{
			STORAGEDBG("%s: DI_MI_DECRYPTOR_SOC_OTPKEY_DoDecrypt failed!\n", __FUNCTION__ );
			DI_MI_DECRYPTOR_SOC_Free(bufferptr);
			bufferptr = NULL;
			return -1;
		}
		memcpy(data, bufferptr, (uint16_t) remaining_bytes);
		DI_MI_DECRYPTOR_SOC_Free(bufferptr);
		bufferptr = NULL;
	}
#else //Dec by OTP Key A
    if ( ( rc = teeAesCbc128DecryptZeroIV( data, remaining_bytes, _SecStoreAesKey ) ) != 0 ) /* TODO: why zero IV??? */
    {
        // encryption failed, need to bail...
        STORAGEDBG("%s: EncryptWithClientKey returned %d, exiting.\n", __FUNCTION__, rc );
        return -1;
    }
#endif

#if 0
    STORAGEDBG("%s: here's a dump of the %d-byte decrypted buffer:\n", __FUNCTION__, remaining_bytes);
    teePrintHexDump(data, remaining_bytes);
#endif

    /* TODO: note: this is a classic, bad MAC-then-encrypt format. Why not the correct encrypt-then-MAC? */

    /*
    * Okay, so we decrypted in place. Now, we need to check the hash. The
    * buffer looks like this:
    *
    * <padlen><data><pad><hash>
    *
    * The hash is 20 bytes, and there may or may not be padding. First thing
    * we want to do is verify the hash
    */
    remaining_bytes -= saSHA1_HASH_SIZE;
    if ( teeValidateSha1( data, remaining_bytes, data + remaining_bytes ) != 0 )
    {
        STORAGEDBG("%s: SHA1 validation failed on manuf-secstore.\n", __FUNCTION__ );
        return -1;
    }

    /* process padding */ /* TODO: padding removal does not properly verify padding value */
    SA_CONSUME_WORD16( data, remaining_bytes, tmp16 );
    if (tmp16 >= 16 /* TODO: find #define for AES block size*/) { /* TODO: this test is *not* compatible with PKCS#7 padding.
                                                                           However, we don't correctly apply PKCS#7 padding.
                                                                           The test is compatible with our incorrect padding algorithm */
        STORAGEDBG("%s: padding length %d not in [0,16[\n", tmp16);
        return -1;
    }
    remaining_bytes -= tmp16;

    // if we get here, everything is dandy
    *p_data = data;
    *p_remaining_bytes = remaining_bytes;

    return 0;

/* NOTREACHED */
parseError:
    return -2;
 }/* end saDecryptManufStore */

/*********************************************************************
 * @function saValidateManufSecStore
 *
 * @brief validate manufacturing secure store
 *
 * @param[inout] p_data - pointer to in-memory copy of manuf secure store
 *                        updated to point at encrypted data
 * @param[inout] p_remaining_bytes - size of manuf secure store data
 *                                   updated to the value of the total length field.
 *
 * @return 0 if secure store is valid, negative otherwise
 *
 *********************************************************************/
static int saValidateManufSecStore( uint8_t **p_data, uint16_t *p_remaining_bytes )
{
    uint8_t *p = *p_data;
    uint32_t remaining_bytes = *p_remaining_bytes;

    uint8_t     headerData[nfSS_FILE_HEADER_SIZE];
    uint8_t     referenceHmacData[nfSS_HEADER_HMAC_SIZE];
    uint8_t     HmacData[nfSS_HEADER_HMAC_SIZE];
    int         rc;
    uint32_t    tmp32;
    uint16_t    tmp16;

    SA_CONSUME_BYTES( p, remaining_bytes, nfSS_FILE_HEADER_SIZE, headerData );

#if 0
    printf("%s: here's the file header I read: \n", __FUNCTION__ );
    teePrintHexDump(BufPtr, nfSS_FILE_HEADER_SIZE );
#endif

#if defined(HUMAX_PLATFORM_BASE)
	{
		//printf("\n\nvalidate by DI_MI_XXX\n\n");
		uint8_t *bufferptr = (uint8_t *)DI_MI_DECRYPTOR_SOC_Malloc(nfSS_HEADER_HMAC_SIZE);
		if(bufferptr == NULL)
		{
			STORAGEDBG("%s: (nexus)mem alloc failed!\n", __FUNCTION__ );
	        goto FILE_ERROR;
		}
		memset(bufferptr, 0, nfSS_HEADER_HMAC_SIZE);
		memcpy(bufferptr, (void *)_SecStoreHmacKey, nfSS_HEADER_HMAC_SIZE);
		if(DI_MI_DECRYPTOR_SOC_OTPKEY_DoEncrypt((unsigned char *)bufferptr, (unsigned char *)bufferptr, (unsigned int)nfSS_HEADER_HMAC_SIZE)!=0)
		{
			STORAGEDBG("%s: DI_MI_DECRYPTOR_SOC_OTPKEY_DoEncrypt failed!\n", __FUNCTION__ );
			DI_MI_DECRYPTOR_SOC_Free(bufferptr);
			bufferptr = NULL;
	        goto FILE_ERROR;
		}
		memcpy((void *)_SecStoreHmacKey, (void *)bufferptr, nfSS_HEADER_HMAC_SIZE);
		DI_MI_DECRYPTOR_SOC_Free(bufferptr);
		bufferptr = NULL;
	}
#endif

    // compute HMAC on header data
    rc = teeHmacSha256( headerData,
                        nfSS_FILE_HEADER_SIZE,
                        _SecStoreHmacKey,
                        sizeof(_SecStoreHmacKey),
                        HmacData);
    if ( rc <= 0 )
    {
        STORAGEDBG("%s: Hmac error?!\n", __FUNCTION__ );
        goto FILE_ERROR;
    }

#if 0
    printf("%s: Here's the computed HMAC: \n", __FUNCTION__ );
    teePrintHexDump(HmacData, nfSS_HEADER_HMAC_SIZE);
#endif

    // verify computed HMAC against value in file
    SA_CONSUME_BYTES( p, remaining_bytes, nfSS_HEADER_HMAC_SIZE, referenceHmacData );
    if ( teeMemcmp( HmacData, referenceHmacData, nfSS_HEADER_HMAC_SIZE ) != 0 )
    {
        STORAGEDBG("%s: Hmac mismatch, exiting\n", __FUNCTION__ );
        goto FILE_ERROR;
    }

    /*
     * okay, header looks okay, check magic/version
     */
    /* reset p and remaining_bytes to initial values */
    p = *p_data;
    remaining_bytes = *p_remaining_bytes;

    /* magic 1 */
    SA_CONSUME_WORD32( p, remaining_bytes, tmp32 );
    if ( tmp32 != nfSS_FILE_MAGIC1 )
    {
        STORAGEDBG("%s: invalid magic1 (%08x)\n", __FUNCTION__, tmp32);
        goto FILE_ERROR;
    }

    /* magic 2 */
    SA_CONSUME_WORD32( p, remaining_bytes, tmp32 );
    if ( tmp32 != nfSS_FILE_MAGIC2 )
    {
        STORAGEDBG("%s: invalid magic2 (%08x)\n", __FUNCTION__, tmp32);
        goto FILE_ERROR;
    }

    STORAGEDBG("%s: file magic is fine.\n", __FUNCTION__ );

    /* version */
    SA_CONSUME_WORD16( p, remaining_bytes, tmp16 );
    if ( tmp16 != nfSS_FILE_VERSION )
    {
        STORAGEDBG("%s: invalid version (%04x)\n", __FUNCTION__, tmp16);
        goto FILE_ERROR;
    }

    /* total length */
    SA_CONSUME_WORD16( p, remaining_bytes, tmp16 );
    if (tmp16 != remaining_bytes - nfSS_HEADER_HMAC_SIZE /* TODO: TotalLength does not include HMAC size? */)
    {
        STORAGEDBG("%s: total length field larger than expected (%d|%d)\n", __FUNCTION__, tmp16, remaining_bytes - nfSS_HEADER_HMAC_SIZE);
        goto FILE_ERROR;
    }

    STORAGEDBG("%s: header verified, total length: %d\n", __FUNCTION__, tmp16);

    /* consume HMAC again */
    SA_CONSUME_BYTES( p, remaining_bytes, nfSS_HEADER_HMAC_SIZE, referenceHmacData );

    *p_data = p;
    *p_remaining_bytes = tmp16;

    // okay, file header is good...
    return 0;

// NOTREACHED
FILE_ERROR:
    return -1;

parseError:
    return -2;
}/* end saValidateManufSecStore */

/*********************************************************************
 * @function saDeriveKpw
 *
 * @brief RFC5869 Kpw derivation function using SHA256
 *
 * @param[in] Kpe - preshared netflix encryption key
 * @param[in] Kph - preshared netflix hmac key
 * @param[out] Kpw - buffer for Kpw
 * @param[in] KpwMaxLength - max length of Kpw
 *
 *
 * @returns length of stored Kpw on success, -1 on failure
 *
 *********************************************************************/
static int saDeriveKpw( uint8_t Kpe[], uint8_t Kph[], uint8_t Kpw[], uint32_t KpwMaxLength )
{
    int     rc;

#define saKPW_LEN 16
    uint8_t Salt[saKPW_LEN] = {0x02, 0x76, 0x17, 0x98, 0x4f, 0x62, 0x27, 0x53,
                               0x9a, 0x63, 0x0b, 0x89, 0x7c, 0x01, 0x7d, 0x69};
    uint8_t Info[saKPW_LEN] = {0x80, 0x9f, 0x82, 0xa7, 0xad, 0xdf, 0x54, 0x8d,
                               0x3e, 0xa9, 0xdd, 0x06, 0x7f, 0xf9, 0xbb, 0x91 };
#define saSHA256_LEN 32
    uint8_t FirstResult[saSHA256_LEN]; // HMAC-SHA256(salt, Kde||Kdh)
    uint8_t SecondResult[saSHA256_LEN]; // HMAC-SHA256(FirstResult, info))

#define saKPE_LEN 16
#define saKPH_LEN 32
#define saCONCAT_KEY_LEN (saKPE_LEN + saKPH_LEN) // concatentate Kpe|Kph
    uint8_t    ConcatKeys[saCONCAT_KEY_LEN];

    /* Kdw = trunc_128(HMAC-SHA256(HMAC-SHA256(salt, Kde||Kdh), info)) */

    // concatenate Kpe|Kph
    (void) teeMemcpy(ConcatKeys, Kpe, saKPE_LEN);
    (void) teeMemcpy(ConcatKeys + saKPE_LEN, Kph, saKPH_LEN);

    // here's our HMAC prototype. NB: data first, key second.
    // int teeHmacSha256( uint8_t     *DataPtr,
    //                    uint32_t    DataLength,
    //                    uint8_t     *KeyPtr,
    //                    uint32_t    KeyLength,
    //                    uint8_t     *ResultPtr )

    // compute FirstResult: key is Salt[], data is concatenated keys
    rc = teeHmacSha256( ConcatKeys, saCONCAT_KEY_LEN, Salt, saKPW_LEN, FirstResult );
    if ( rc != saSHA256_LEN )
    {
        STORAGEDBG("%s: teeHmacSha256(FirstResult) failed, returned %d\n", __FUNCTION__, rc );
        return -1;
    }

    // compute SecondResult: key is FirstResult, data is Info[]
    rc = teeHmacSha256( Info, saKPW_LEN, FirstResult, saSHA256_LEN, SecondResult );
    if ( rc != saSHA256_LEN )
    {
        STORAGEDBG("%s: teeHmacSha256(SecondResult) failed, returned %d\n", __FUNCTION__, rc );
        return -1;
    }

    // copy first 128 bits into Kpw if there is room
    if ( KpwMaxLength >= saKPW_LEN )
        (void) teeMemcpy(Kpw, SecondResult, saKPW_LEN );

    STORAGEDBG("%s: dumping derived Kpw:\n", __FUNCTION__ );
    //teePrintHexDump( Kpw, saKPW_LEN );

    return saKPW_LEN;

}/* end saDeriveKpw */

/***************************************************************************
 *
 * @function   teeGetSecStoreData
 *
 * @brief   Load secure store data into memory
 *
 * @param[in] file_name - pointer to filename for secure store
 * @param[in] p_ss_data - pointer to pointer to buffer
 * @param[in] BufLengthPtr - pointer to where you want length stored
 *
 * @returns  0 if everything okay, -1 otherwise
 *
 **************************************************************************/
static int teeGetSecStoreData( char *file_name, uint8_t **p_ss_data, uint32_t *p_ss_data_sz )
{
    FILE       *fp;
    int         ss_data_sz;
    struct stat StatBuf;
    uint8_t    *ss_data;
    char       *PathPtr;

#define nfMANUF_SS_PATH "./manuf_ss.bin"
#define nfMANUF_SS_PATH_BACKUP "/tmp/manuf_ss.bin"

    if ((PathPtr = (char *) file_name) == NULL )
        PathPtr = nfMANUF_SS_PATH;

    // get file size; try cwd, and if that fails, try backup path (/tmp)
    if ( stat( PathPtr, &StatBuf ) != 0 )
    {
        PathPtr = nfMANUF_SS_PATH_BACKUP;
        if ( stat( PathPtr, &StatBuf ) != 0 )
        {
            STORAGEDBG("%s: failed to stat >%s<\n", __FUNCTION__, PathPtr );
            return -1;
        }
    }

    STORAGEDBG("%s: allocating %d-byte buffer to hold file...\n", __FUNCTION__, (int) StatBuf.st_size );
    if ( StatBuf.st_size  > nfSS_MAX_FILE_SIZE )
    {
        STORAGEDBG("%s: SS is too large (%d/%d), bailing.\n", __FUNCTION__, (int)StatBuf.st_size, nfSS_MAX_FILE_SIZE );
        return -1;
    }

    if ( ( fp = fopen( PathPtr, "r") ) == NULL )
    {
        STORAGEDBG("%s: failed to open >%s<\n", __FUNCTION__, PathPtr );
        return -1;
    }

    /*
     * Use ss_data to point to local static buffer. If system supports dynamic
     * mem allocation, can alloc instead
     */
    ss_data = _saMemBuf;

    if ( (ss_data_sz = fread(ss_data, 1, StatBuf.st_size, fp)) != StatBuf.st_size )
    {
        STORAGEDBG("%s: fread returned %d, bailing.\n", __FUNCTION__, ss_data_sz);
        (void) fclose(fp);
        return -1;
    }

    (void) fclose(fp);
    *p_ss_data = ss_data;
    *p_ss_data_sz = ss_data_sz;

    return 0;

}/* end teeGetSecStoreData */

static nfTeeStorageResult_t deriveKeys( uint8_t id[],
                                        uint8_t derivationKey[saAES_128_KEY_SIZE_BYTES],
                                        uint8_t AesKey[],
                                        uint8_t HmacKey[] ) {

    uint8_t    internalKey[saAES_128_KEY_SIZE_BYTES];
    uint8_t    TempBuf[saAES_128_KEY_SIZE_BYTES + saHMAC_SHA256_KEY_SIZE_BYTES];
    int        rc;

    /*
     * Compute application-specific keys
     */

    // first, encrypt ClientId; this becomes internalKey
    (void) teeMemcpy( internalKey, id, sizeof internalKey );
    rc = teeAesCbc128EncryptZeroIV( internalKey, sizeof internalKey, derivationKey );
    if ( rc != 0 )
    {
        CRYPTODBG("%s: failed to create app key ladder base!?\n", __FUNCTION__);
        return nfTSR_INTERNAL_ERROR;
    }

    if ( ( rc = teeSha384(internalKey, sizeof internalKey, TempBuf) ) != 0 )
    {
        CRYPTODBG("%s: teeSha384() failed!?\n", __FUNCTION__);
        return nfTSR_INTERNAL_ERROR;
    }

    // first 128 bits is AES key, next 256 is HMAC key
    (void)teeMemcpy( AesKey, TempBuf, saAES_128_KEY_SIZE_BYTES );
    (void)teeMemcpy( HmacKey, TempBuf + saAES_128_KEY_SIZE_BYTES, saHMAC_SHA256_KEY_SIZE_BYTES );

    // don't leave key material laying around...
    (void) teeBzero(internalKey, sizeof internalKey);
    (void) teeBzero(TempBuf, sizeof TempBuf);

    return (nfTeeStorageResult_t)nfTGR_SUCCESS;
}

/*********************************************************************
 * @function saGetAppKeys
 *
 * @brief gets AES/Hmac keys for a given client ID
 *
 * @param[in] ClientId - ID for client who made request
 * @param[out] AesKeyBuf - buffer for AES key
 * @param[out] HmacKeyBuf - buffer for HmacSha256 key
 *
 * NB: Assumes 128-bit AES key and 256-bit HMAC key; make sure your buffers
 *     are big enough...
 *
 * @return nfTGR_SUCCESS if everything okay, appropriate error otherwise
 *
 *********************************************************************/
static nfTeeStorageResult_t saGetAppKeys( nfTeeClientId_t     ClientId,
                                          uint8_t             AesKey[],
                                          uint8_t             HmacKey[] )
{
    return deriveKeys(ClientId, saInternalAesKey, AesKey, HmacKey );

}/* end saGetAppKeys */

/*********************************************************************
 * @function saTpiGetTAKeyLadderBase
 *
 * @brief creates device-unique key ladder base for a given (trusted) client ID
 *
 * @param[in] ClientId - ID for client who made request
 * @param[out] Buf - buffer for key ladder
 * @param[in] BufSize - size of buffer
  * @param[out] ActualSizePtr - pointer to where you output size
 *
 * @return nfTGR_SUCCESS if everything okay, appropriate error otherwise
 *
 *********************************************************************/
nfTeeStorageResult_t saTpiGetTAKeyLadderBase( nfTeeClientId_t ClientId,
                                              uint8_t         Buf[],
                                              uint32_t        BufSize,
                                              uint32_t       *ActualSizePtr )
{
    int rc;

    /*
     * NB: we currently only support this for cryptoAgent, so make
     * sure it's the right clientId
     */
    if ( teeMemcmp(cryptoAgentClientId, ClientId, sizeof(nfTeeClientId_t)) )
    {
        // they don't match, this is a security violation.
        STORAGEDBG("%s: wrong client ID.\n", __FUNCTION__);
        return nfTSR_NOT_FOUND;
    }

    // okay, generate the key. Remember, encrypts in place...

    if ( BufSize < sizeof(nfTeeClientId_t) )
    {
        STORAGEDBG("%s: passed buffer is too small (%d/%d)", __FUNCTION__,
                    BufSize, sizeof(nfTeeClientId_t) );
        return nfTSR_BUFFER_TOO_SHORT;
    }

    teeMemcpy( Buf, ClientId, sizeof(nfTeeClientId_t) );
    rc = teeAesCbc128EncryptZeroIV( Buf, sizeof(nfTeeClientId_t), saKeyLadderBase);
    if ( rc != 0 )
    {
        STORAGEDBG("%s: failed to create internal key for cryptoAgent\n", __FUNCTION__ );
        return nfTSR_INTERNAL_ERROR;
    }

    STORAGEDBG("%s: successfully created cryptoAgent ladder key.\n", __FUNCTION__);
    *ActualSizePtr = sizeof(saKeyLadderBase);

    return (nfTeeStorageResult_t)nfTGR_SUCCESS;

}/* end saTpiGetKeyLadderBase */

/**************************************************
 * @function saTpiLoadNflxIdFile
 *
 * @brief storage agent TPI function to load Netflix idfile
 *
 * @param[in] ClientId - ID of client who can access these
 * @param[in] DataPtr - pointer to idfile data
 * @param[in] DataLength - length of data
 *
 * @return 0 if successful, -1 otherwise
 *
 *********************************************************************/
nfTeeStorageResult_t saTpiLoadNflxIdFile( nfTeeClientId_t ClientId, uint8_t *DataPtr, uint32_t DataLength )
{
    (void) ClientId;
    (void) DataPtr;
    (void) DataLength;

    STORAGEDBG("%s: this function is unimplemented\n",__FUNCTION__ );
    assert(0);
    return (nfTeeStorageResult_t) nfTGR_INTERNAL_ERROR;
}/* end saTpiLoadNflxIdFile */

/**************************************************
 * @function saTpiLoadDrmCredentials
 *
 * @brief   storage agent TPI function to load DRM credentials
 *
 * @param[in] ClientId - ID of client who can access these
 * @param[in] DataPtr - pointer to credential data
 * @param[in] DataLength - length of data
 *
 * @return 0 if successful, -1 otherwise
 *
 *********************************************************************/
nfTeeStorageResult_t saTpiLoadNflxDrmCredentials( nfTeeClientId_t ClientId, uint8_t *DataPtr, uint32_t DataLength )
{
    (void) ClientId;
    (void) DataPtr;
    (void) DataLength;

    STORAGEDBG("%s: this function is unimplemented\n", __FUNCTION__ );
    assert(0);
    return (nfTeeStorageResult_t) nfTGR_INTERNAL_ERROR;
}

/**************************************************
 * @function saTpiProtectData
 *
 * @brief   storage agent TPI function to protect application data
 *
 * @param[in] ClientId - ID of client requesting operation
 * @param[in] DataPtr - pointer to data that you want protected
 * @param[in] DataLength - length of unprotected data
 * @param[out] ProtectedDataPtr - pointer to preallocated buffer for protected data
 * @param[in] MaxProtectedDataLength - size of buffer at ProtectedDataPtr
 * @param[out] ProtectedDataLengthPtr - actual output length will be stored here.
 *
 * @return 0 if successful, -1 otherwise
 *
 *********************************************************************/

nfTeeResult_t saTpiProtectData( nfTeeClientId_t ClientId,
                                 const uint8_t        *DataPtr,
                                       uint32_t        DataLength,
                                       uint8_t        *ProtectedDataPtr,
                                       uint32_t        MaxProtectedDataLength,
                                       uint32_t       *ProtectedDataLengthPtr )
{
    nfTeeResult_t ResponseCode = nfTGR_SUCCESS;
    uint8_t       AppAesKey[saAES_128_KEY_SIZE_BYTES];
    uint8_t       AppHmacKey[saHMAC_SHA256_KEY_SIZE_BYTES];
    uint8_t      *p = ProtectedDataPtr; /* work directly into output buffer */
    uint32_t      remaining_bytes = MaxProtectedDataLength;
    uint8_t       IV[saAES_128_KEY_SIZE_BYTES];

    /*
     * Layout of the produced message:
     * +----------+-----------------------------------------+
     * |  4 bytes | Storage version (0x10000001)            |
     * +----------+-----------------------------------------+
     * | 16 bytes | random IV                               |
     * +----------+-----------------------------------------+
     * |  N bytes | +-------------------------------------+ |
     * |          | | AES-128-CBC encryption of:          | |
     * |          | | +------------------+--------------+ | |
     * |          | | | DataLength bytes | Data copy    | | |
     * |          | | +------------------+--------------+ | |
     * |          | | | Up to 15 bytes   | PKCS padding | | |
     * |          | | +------------------+--------------+ | |
     * |          | +-------------------------------------+ |
     * +----------+-----------------------------------------+
     * | 32 bytes | HMAC of all of the above                |
     * +----------+-----------------------------------------+
     */

    /* 1. obtain app keys */
    if ( ( ResponseCode = saGetAppKeys( ClientId, AppAesKey, AppHmacKey ) ) != nfTGR_SUCCESS )
    {
        STORAGEDBG("%s: failed to get app keys, bailing.\n", __FUNCTION__);
        goto error;
    }

    /* 2. store version */
    SA_PRODUCE_WORD32( p, remaining_bytes, saStorageVersion );

    /* 3. store IV */
    if ( ! teeCryptoRandomBytes( IV, sizeof IV ) )
    {
        STORAGEDBG("%s: failed to allocate random bytes for key seal IV\n", __FUNCTION__ );
        ResponseCode = nfTSR_CRYPT_RANDOM_FAILURE;
        goto error;
    }

    SA_PRODUCE_BYTES( p, remaining_bytes, sizeof IV, IV );

    /* 3. padded-then-encrypted data
     *    mimic the three steps of the SA_PRODUCE_* macros:
     *      - check length,
     *      - perform data serialization,
     *      - update pointer and remaining bytes
     */
    {
        uint32_t expectedCiphertextLength = DataLength + (saAES_128_KEY_SIZE_BYTES - (DataLength % saAES_128_KEY_SIZE_BYTES));
        uint32_t CiphertextLength;

        if ( expectedCiphertextLength < DataLength /* uint overflow case */ || remaining_bytes < expectedCiphertextLength ) {
            STORAGEDBG("%s: failed to write %u bytes (%u remaining)\n", __FUNCTION__, expectedCiphertextLength, remaining_bytes);
            goto serializeError;
        }


#if defined(HUMAX_PLATFORM_BASE)
		{
			CiphertextLength = DataLength + (saAES_128_KEY_SIZE_BYTES - (DataLength % saAES_128_KEY_SIZE_BYTES));
			uint8_t *bufferptr = (uint8_t *)DI_MI_DECRYPTOR_SOC_Malloc((uint32_t)CiphertextLength);
			memset(bufferptr, 0, (uint32_t) CiphertextLength);
			memcpy((void *)bufferptr, (void *)DataPtr, (uint32_t) DataLength);
			DI_MI_DECRYPTOR_SOC_OTPKEY_DoEncrypt((unsigned char *)bufferptr, (unsigned char *)bufferptr, (unsigned int) CiphertextLength);
			memcpy(p, bufferptr, (uint32_t) CiphertextLength);
			DI_MI_DECRYPTOR_SOC_Free(bufferptr);
			bufferptr = NULL;
		}

#else
        CiphertextLength = teeAes128CbcEncrypt( DataPtr, DataLength, AppAesKey, IV, p, expectedCiphertextLength);
        if ( CiphertextLength <= 0 )
        {
            STORAGEDBG("%s: encrypt failure, bailing.", __FUNCTION__);
            ResponseCode = nfTSR_CIPHER_ERROR;
            goto error;
        }

#endif

        if ( CiphertextLength != expectedCiphertextLength )
        {
            STORAGEDBG("%s: CiphertextLength (%d) != expectedCiphertextLength (%d): possible out-of-bounds access. Aborting\n", __FUNCTION__, CiphertextLength, expectedCiphertextLength);
            goto serializeError;
        }

        p += CiphertextLength;
        remaining_bytes -= CiphertextLength;
    }

    /* 4. HMAC
     *    as above: mimic the three steps of the SA_PRODUCE_* macros
     */
    {
        int         HmacLength;

        if ( remaining_bytes < saHMAC_SHA256_KEY_SIZE_BYTES ) {
            STORAGEDBG("%s: failed to write %d bytes (%d remaining)\n", __FUNCTION__, saHMAC_SHA256_KEY_SIZE_BYTES, remaining_bytes);
            goto serializeError;
        }
#if defined(HUMAX_PLATFORM_BASE)
		{// HMACKey Encrypted By H/W
			//printf("\n\nEncrypt By DI_MI_XXX(SS HMAC Key)\n\n");
			uint8_t *bufferptr = (uint8_t *)DI_MI_DECRYPTOR_SOC_Malloc(saHMAC_SHA256_KEY_SIZE_BYTES);
			memset(bufferptr, 0, saHMAC_SHA256_KEY_SIZE_BYTES);
			memcpy(bufferptr, (void *)AppHmacKey, saHMAC_SHA256_KEY_SIZE_BYTES);
			DI_MI_DECRYPTOR_SOC_OTPKEY_DoEncrypt((unsigned char *)bufferptr, (unsigned char *)bufferptr, (unsigned int)saHMAC_SHA256_KEY_SIZE_BYTES);
			memcpy((void *)AppHmacKey, (void *)bufferptr, saHMAC_SHA256_KEY_SIZE_BYTES);
			DI_MI_DECRYPTOR_SOC_Free(bufferptr);
			bufferptr = NULL;
		}
#endif
#if defined(HUMAX_NRDP_4_2_0)
        HmacLength = teeHmacSha256( WorkBuf, (p - WorkBuf), AppHmacKey, saHMAC_SHA256_KEY_SIZE_BYTES, p );
#else
        /* HUMAX_NRDP_4_2_1 */
        HmacLength = teeHmacSha256( ProtectedDataPtr, (p - ProtectedDataPtr), AppHmacKey, saHMAC_SHA256_KEY_SIZE_BYTES, p );
#endif
        if ( saHMAC_SHA256_KEY_SIZE_BYTES != HmacLength )
        {
            STORAGEDBG("%s: HMAC failed (length: %d)\n", __FUNCTION__, HmacLength );
            ResponseCode = nfTSR_CIPHER_ERROR;
            goto error;
        }

        p += saHMAC_SHA256_KEY_SIZE_BYTES;
        remaining_bytes -= saHMAC_SHA256_KEY_SIZE_BYTES;
    }

    /* 5. update actual protected data length */
    *ProtectedDataLengthPtr = p - ProtectedDataPtr;

    return ResponseCode;

/* NOT REACHED */
serializeError:
    ResponseCode = nfTSR_BUFFER_TOO_SHORT;
    goto error;

error:
    teeMemset(ProtectedDataPtr, 0, MaxProtectedDataLength);
    return ResponseCode;

}/* end saTpiProtectData */


/**************************************************
 * @function saTpiUnrotectData
 *
 * @brief   storage agent TPI function to protect application data
 *
 * @param[in] ClientId - ID of client requesting operation
 * @param[in] ProtectedDataPtr - pointer to protected data
 * @param[in] ProtectedDataLength - length of protected data
 * @param[out] DataPtr - pointer to preallocated buffer for unprotected data
 * @param[in] MaxDataLength - size of buffer at DataPtr
 * @param[out] UnprotectedDataLengthPtr - actual output length will be stored here.
 *
 * @return 0 if successful, -1 otherwise
 *
 *********************************************************************/
nfTeeResult_t saTpiUnprotectData( nfTeeClientId_t  ClientId,
                            const uint8_t         *ProtectedDataPtr,
                                  uint32_t         ProtectedDataLength,
                                  uint8_t         *DataPtr,
                                  uint32_t         MaxDataLength,
                                  uint32_t        *UnprotectedDataLengthPtr )
{
    nfTeeResult_t  ResponseCode;
    const uint8_t *p = ProtectedDataPtr;
    uint32_t       remaining_bytes = ProtectedDataLength;

    uint32_t       tmp32;
    const uint8_t *IVPtr;
    const uint8_t *PayloadPtr;

    uint8_t        AppAesKey[saAES_128_KEY_SIZE_BYTES];
    uint8_t        AppHmacKey[saHMAC_SHA256_KEY_SIZE_BYTES];

    uint32_t       CiphertextLength;
    int            HmacLength;
    uint8_t        HmacValue[saHMAC_SHA256_KEY_SIZE_BYTES];

    int            CleartextLength;

    /*
     * Expected layout of message to process:
     * +----------+-----------------------------------------+
     * |  4 bytes | Storage version (0x10000001)            |
     * +----------+-----------------------------------------+
     * | 16 bytes | random IV                               |
     * +----------+-----------------------------------------+
     * |  N bytes | +-------------------------------------+ |
     * |          | | AES-128-CBC encryption of:          | |
     * |          | | +------------------+--------------+ | |
     * |          | | | DataLength bytes | Data copy    | | |
     * |          | | +------------------+--------------+ | |
     * |          | | | Up to 15 bytes   | PKCS padding | | |
     * |          | | +------------------+--------------+ | |
     * |          | +-------------------------------------+ |
     * +----------+-----------------------------------------+
     * | 32 bytes | HMAC of all of the above                |
     * +----------+-----------------------------------------+
     */

    /* 2. version */
    SA_CONSUME_WORD32( p, remaining_bytes, tmp32 );
    if (tmp32 != saStorageVersion
     && tmp32 != saStorageVersionLE) { /* NOTE: must also check against other-endian version value to deal with legacy code that
                                                failed to serialize the version as big-endian but used native-endian instead) */

        STORAGEDBG("%s: unrecognized version (%08x).\n", __FUNCTION__, tmp32 );
        return nfTSR_UNRECOGNIZED_VERSION;
    }

    /* 3. IV */
    SA_CONSUME_BYTES_PTR( p, remaining_bytes, saAES_128_KEY_SIZE_BYTES, IVPtr );

    /* 4. encrypted payload */
    CiphertextLength = remaining_bytes - saHMAC_SHA256_KEY_SIZE_BYTES;
    SA_CONSUME_BYTES_PTR( p, remaining_bytes, CiphertextLength, PayloadPtr );

    /* 5. get the app keys for this client */
    if ( ( ResponseCode = saGetAppKeys( ClientId, AppAesKey, AppHmacKey ) ) != nfTGR_SUCCESS )
    {
        STORAGEDBG("%s: failed to get app keys, bailing.\n", __FUNCTION__);
        return ResponseCode;
    }

#if defined(HUMAX_PLATFORM_BASE)
	{ // HUMAX MODIFY
		// HMACKey Encrypted By H/W
		//printf("\n\nEncrypt By DI_MI_XXX(SS HMAC Key)\n\n");
		uint8_t *bufferptr = (uint8_t *)DI_MI_DECRYPTOR_SOC_Malloc(saHMAC_SHA256_KEY_SIZE_BYTES);
		memset(bufferptr, 0, saHMAC_SHA256_KEY_SIZE_BYTES);
		memcpy(bufferptr, (void *)AppHmacKey, saHMAC_SHA256_KEY_SIZE_BYTES);
		DI_MI_DECRYPTOR_SOC_OTPKEY_DoEncrypt((unsigned char *)bufferptr, (unsigned char *)bufferptr, (unsigned int)saHMAC_SHA256_KEY_SIZE_BYTES);
		memcpy((void *)AppHmacKey, (void *)bufferptr, saHMAC_SHA256_KEY_SIZE_BYTES);
		DI_MI_DECRYPTOR_SOC_Free(bufferptr);
		bufferptr = NULL;
	}
#endif

    /* 6. HMAC (the version check above confirms the location of the HMAC at the end of the buffer */
#if defined(HUMAX_NRDP_4_2_0)
    STORAGEDBG("%s: HmacDataLength: %d.\n", __FUNCTION__, p - WorkBuf);
    HmacLength = teeHmacSha256( WorkBuf, p - WorkBuf , AppHmacKey, saHMAC_SHA256_KEY_SIZE_BYTES, HmacValue );
#else
    /* HUMAX_NRDP_4_2_1 */
    STORAGEDBG("%s: HmacDataLength: %d.\n", __FUNCTION__, p - ProtectedDataPtr);
    HmacLength = teeHmacSha256( ProtectedDataPtr, p - ProtectedDataPtr, AppHmacKey, saHMAC_SHA256_KEY_SIZE_BYTES, HmacValue );
#endif
    if ( HmacLength != saHMAC_SHA256_KEY_SIZE_BYTES )
    {
        STORAGEDBG("%s: HMAC failed (length: %d)\n", __FUNCTION__, HmacLength );
        return nfTSR_CIPHER_ERROR;
    }

    if ( teeMemcmp( HmacValue, p, saHMAC_SHA256_KEY_SIZE_BYTES ) != 0 )
    {
        STORAGEDBG("%s: HMAC failure on protected data\n", __FUNCTION__);
        return nfTSR_CIPHER_ERROR;
    }


#if defined(HUMAX_PLATFORM_BASE)
	{// Data Decrypted By H/W
		// TODO: Need Test (BlockSize)
		//printf("\n\nEncrypt By DI_MI_XXX(SS Data)\n\n");
		//BlockSize is 16.
		uint8_t *bufferptr = (uint8_t *)DI_MI_DECRYPTOR_SOC_Malloc((uint32_t)CiphertextLength);
		memset(bufferptr, 0, (uint32_t) CiphertextLength);
		memcpy((void *)bufferptr, (void *)PayloadPtr, (uint32_t) CiphertextLength);
		DI_MI_DECRYPTOR_SOC_OTPKEY_DoDecrypt((unsigned char *)bufferptr, (unsigned char *)bufferptr, (unsigned int) CiphertextLength);
		memcpy(DataPtr, bufferptr, (uint32_t) CiphertextLength);
		DI_MI_DECRYPTOR_SOC_Free(bufferptr);
		CleartextLength = CiphertextLength ;
		bufferptr = NULL;
	}
#else
    /* 7. decrypt payload */
    CleartextLength = teeAes128CbcDecrypt( PayloadPtr, CiphertextLength, AppAesKey, IVPtr, DataPtr, MaxDataLength );

#endif

    if ( CleartextLength <= 0 )
    {
        STORAGEDBG("%s: decrypt failure, bailing.", __FUNCTION__);
        return nfTSR_CIPHER_ERROR;
    }

    *UnprotectedDataLengthPtr = (uint32_t) CleartextLength;

    return nfTGR_SUCCESS;

/* NOT REACHED */
parseError:
    return -2;
}/* end saTpiUnprotectData */

/*********************************************************************
 * @function saTpiInit
 *
 * @brief   storage agent TPI initialization
 *
 * @param[in] InitDataPtr - pointer to initialization data
 * @param[in] InitDataLength - length of initialization data
 * @param[out] StatePtr - pointer to where you want storage agent state
 *
 * @return nfTGR_SUCCESS if initialized, error otherwise
 *
 *********************************************************************/
nfTeeStorageResult_t saTpiInit( uint8_t                 *InitDataPtr,
                                uint32_t                 InitDataLength,
                                nfStorageAgentState_t   *StatePtr )
{
    int         rc;
    uint8_t    *p;
    uint16_t    remaining_bytes;

    (void) InitDataLength;

    /*
     * We need to do the following:
     *
     *     - construct pathname for secure store
     *     - load encrypted/hmac'd secure store into memory
     *     - validate (hmac)
     *     - decrypt
     *     - initialize saManufSSTable
     *     - initialize app service
     */

    // XXX temporarily, leave filesystem mechanism here. This
    // XXX should be removed prior to release
    if ( InitDataPtr == NULL )
    {
        char *SSFileNamePtr = nfMANUF_SECSTORE_FILENAME;

        // load encrypted/hmac'd secure store into memory
        if ( ( rc = teeGetSecStoreData( SSFileNamePtr, &saManufSSDataPtr, &saManufSSDataLength ) ) != 0 )
        {
            // failed to load data, bail.
            printf("%s: failed to load manufacturing secure store data (%s)\n", __FUNCTION__, SSFileNamePtr );

            printf("\nYou can generate the manufacturing secure store using the tool: manufss\n");
            printf("You can also launch netflix using the script \"netflix_manuss.sh\" which will\n");
            printf("call \"manufss\" for you.\n\n");
            exit(0);
        }
    }
    else
    {
        saManufSSDataPtr = InitDataPtr;
        saManufSSDataLength = InitDataLength;
    }

    p = saManufSSDataPtr;
    remaining_bytes = saManufSSDataLength;

    if ( ( rc = saValidateManufSecStore( &p, &remaining_bytes ) ) != 0 )
    {
        // secure store corrupted/invalid
        STORAGEDBG("%s: invalid manufacturing secure store.\n", __FUNCTION__);
        return nfTSR_MANUF_SSTORE_INVALID;
    }

    if ( ( rc = saDecryptManufStore( &p, &remaining_bytes ) ) != 0 )
    {
        STORAGEDBG("%s: decryption/validation of manuf-ss failed, aborting\n", __FUNCTION__ );
        return nfTSR_MANUF_SSTORE_INVALID;
    }

    /*
     * We've successfully decrypted the data, p points to beginning of the management section.
     * Next, decode each section, storing associated pointers in saManufSSTable.
     */

    do
    {
        /*
         * process each manuf-secstore file section. Remember, each section looks like this:
         *
         * <unique ID><16-bit section length><data>
         *
         * and there are an arbitrary number of sections (actually, only 3 today).
         */
        if ( ( rc = ProcessSection( &p, &remaining_bytes ) ) != 0 )
        {
           STORAGEDBG("%s: GetNextSection returned %d; bailing.\n", __FUNCTION__, rc );
           return nfTSR_MANUF_SSTORE_INVALID;
        }

    } while (remaining_bytes > 0);

    /*
     * Special case for KPW: need to derive from Kpe/Kph; do that now.
     */
    rc = saDeriveKpw( saManufSSTable[nfSSRI_KPE].DataPtr, saManufSSTable[nfSSRI_KPH].DataPtr, saLocalKPWStorage, sizeof(saLocalKPWStorage) );
    if (rc <= 0)
    {
        STORAGEDBG("%s: failed to derive Kpw, bailing.\n", __FUNCTION__);
        return nfTSR_KEY_DERIV_ERROR;
    }

    saManufSSTable[nfSSRI_KPW].DataLength = rc;
    saManufSSTable[nfSSRI_KPW].DataPtr = saLocalKPWStorage;
    saManufSSTable[nfSSRI_KPW].ClientKey = ntbaAGENT_KEY_KPW;
    saManufSSTable[nfSSRI_KPW].ClientIdPtr = saManufSSTable[nfSSRI_KPE].ClientIdPtr;

    /*
     * Derive key ladder for app storage services
     */
    /*
     * caInternalKey is our key ladder key. We will use it to derive our internal
     * AES and HMAC-SHA256 keys.
     */
    rc = deriveKeys( saKeyLadderBase, _SecStoreAesKey, saInternalAesKey, saInternalHMACKey );
    if ( (int)nfTGR_SUCCESS != rc )
    {
        STORAGEDBG("%s: failed to compute internal AES and HMAC keys\n", __FUNCTION__);
        return rc;
    }

    // update state
    *StatePtr = nfSAS_RUN;

    return (nfTeeStorageResult_t) nfTGR_SUCCESS;

}/* end saTpiInit */

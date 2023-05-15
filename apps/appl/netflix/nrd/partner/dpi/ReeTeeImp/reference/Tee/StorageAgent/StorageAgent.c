/*
 * (c) 1997-2015 Netflix, Inc.  All content herein is protected by
 * U.S. copyright and other applicable intellectual property laws and
 * may not be copied without the express permission of Netflix, Inc.,
 * which reserves all rights.  Reuse of any of this content for any
 * purpose without the permission of Netflix, Inc. is strictly
 * prohibited.
 */

#include "CommTypes.h"
#include "StorageTypes.h"
#include "StorageAgent.h"
//#include "./StorageAgentInternals.h"
//#include "StorageAgentData.h"
#include "StorageAgentTPI.h"
#include "TeeDebug.h"
#include "TeeInternalDpi.h"


/*
 * This is the definition for the manufacturing secure store table. This
 * table is shared with StorageAgentRead.c
 */
nfSecureStoreTableRow_t saManufSSTable[saMANUF_SS_TABLE_SIZE] = {{0, NULL, 0, NULL}};
static nfStorageAgentState_t saStorageAgentState = nfSAS_UNINIT;

// an all-zeros clientID means "any"
static nfTeeClientId_t saClientIdAny = {0x00};

/*********************************************************************
 * @function saGetStorageAgentState
 *
 * @brief get storageAgent state
 *
 *
 * @return nfTSR_SUCCESS, error otherwise
 *
 *********************************************************************/
nfStorageAgentState_t saGetStorageAgentState()
{
    return saStorageAgentState;
}

/*********************************************************************
 * @function saSetStorageAgentState
 *
 * @brief get storageAgent state
 *
 *
 * @return nfTSR_SUCCESS, error otherwise
 *
 *********************************************************************/
nfTeeResult_t saSetStorageAgentState(nfStorageAgentState_t NewState )
{
    saStorageAgentState = NewState;
    return nfTGR_SUCCESS;
}

/*********************************************************************
 * @function saLookupKey
 *
 * @brief find data for matching client key
 *
 * @param[in/out] ClientId - client ID for requester (must match key)
 * @param[in] Key - the client key
 * @param[in] DataPtr - pointer to where you want data
 * @param[out] DataLengthPtr - pointer to where you want length returned
 * @param[in] MaxLength - max size of data
 *
 * @return nfTSR_SUCCESS, error otherwise
 *
 * NOTE: If you pass in the special mgmt client ID, this will be
 *       over-written with the actual client ID associated with
 *       the key upon return...
 *
 *********************************************************************/
nfTeeResult_t saLookupKey( nfTeeClientId_t             ClientId,
                             nfSecureStoreClientKey_t    Key,
                             uint8_t                    *DataPtr,
                             uint32_t                    *DataLengthPtr,
                             uint32_t                    MaxLength )
{
    int row;

    /*
     * We need to do the following:
     *     - validate arguments
     *     - look for matching key/clientid in saManufSSTable
     *     - if found
     *         - make sure there is enough room at DataPtr
     *         - copy data
     *         - set DataLengthPtr
     *         - copy client ID
     *         - return
     *     - else
     *         - return error
     */

    STORAGEDBG("%s: looking for key %0x in saManufSSTable[]\n ", __FUNCTION__, Key);
    for ( row = 0; row < nfSSRI_INVALID; ++row )
    {
        STORAGEDBG("%s: comparing to key %0x in row %d\n", __FUNCTION__, saManufSSTable[row].ClientKey, row );
        if ( saManufSSTable[row].ClientKey == Key )
        {
            STORAGEDBG("%s: found match in row %d, looking at clientId\n", __FUNCTION__, row );
            if ( ( teeMemcmp( saManufSSTable[row].ClientIdPtr, saClientIdAny, sizeof(nfTeeClientId_t) ) == 0 )
              || ( teeMemcmp( ClientId, saManufSSTable[row].ClientIdPtr, sizeof(nfTeeClientId_t)) == 0 ) )
            {
                STORAGEDBG("%s: client id's match, checking length (%d/%d)\n", __FUNCTION__,
                         saManufSSTable[row].DataLength, MaxLength );
#if 0
                STORAGEDBG("\tdumping stored client ID:\n");
                teePrintHexDump(saManufSSTable[row].ClientIdPtr, sizeof(nfTeeClientId_t));
                STORAGEDBG("\tdumping passeded client ID:\n");
                teePrintHexDump(ClientId, sizeof(nfTeeClientId_t));
#endif

                // found it -- how long is it?
                if ( saManufSSTable[row].DataLength <= MaxLength )
                {
                    STORAGEDBG("%s: found data for key %08x, length is %d\n", __FUNCTION__, Key, saManufSSTable[row].DataLength );
                    *DataLengthPtr = saManufSSTable[row].DataLength;
                    teeMemcpy( DataPtr, saManufSSTable[row].DataPtr, *DataLengthPtr );
#if 0
                    STORAGEDBG("\tHere's the data I'm returning:\n");
                    teePrintHexDump(DataPtr, *DataLengthPtr);
#endif
                    return nfTGR_SUCCESS;
                }
                else
                {
                    STORAGEDBG("%s: not enough room in passed buffer (%d/%d)\n", __FUNCTION__,
                            saManufSSTable[row].DataLength, MaxLength );
                    return nfTSR_BUFFER_TOO_SHORT;
                }
            }/* end dual teeMemcmp() */
        }
    }/* end for ( row = 0; row < nfSSRI_INVALID; ++row ) */

    /*
     * Each TEE agent can request its own personal key. These keys must be
     * device unique. We provide a TPI function for this purpose.
     *
     * So far, cryptoAgent is the only one who uses this service. In that
     * case, it is used to generate the keys used for "persisting" keys.
     */
    // if we get here, we didn't find it. Is it a magic internal key?
    if ( Key == ntbaAGENT_KEY_INTERNAL )
    {
        STORAGEDBG("%s: creating application key ladder base.\n", __FUNCTION__ );
        return saTpiGetTAKeyLadderBase( ClientId, DataPtr, MaxLength, DataLengthPtr );

    }/* end if ( Key == ntbaAGENT_KEY_INTERNAL ) */

    STORAGEDBG("%s: returing not-found for key %0x\n", __FUNCTION__, Key);
    return nfTSR_NOT_FOUND;

}/* end saLookupKey */


/*********************************************************************
 * @function storageAgentInit
 *
 * @brief Storage Agent initialization
 *
 * @param[in] argc - argument count
 * @param[in] argv - pointer to argument vector
 *
 * @returns nfTGR_SUCCESS, nfTSR_MANUF_SSTORE_INVALID
 *
 *********************************************************************/
nfTeeResult_t storageAgentInit( int argc, char **argv )
{
     nfTeeStorageResult_t     Result;
     nfStorageAgentState_t     State = saStorageAgentState;
     uint32_t                Length;

     STORAGEDBG("%s: argc is %d, calling saTpiInit()\n", __FUNCTION__, argc );

     /*
      * secure store must be passed in.
      * argv[0] is length, argv[1] is secure store.
      */
     if ( argc == 2 )
     {
         Length = *((uint32_t *)argv[0]);
         STORAGEDBG("%s: using %d-byte in-memory manuf_ss.bin\n", __FUNCTION__, Length);
         Result = saTpiInit( (uint8_t *) argv[1], Length, &State );

     }
     else
     {
         STORAGEDBG("\n******************************************\n");
         STORAGEDBG("%s: no manufacturing secure store data passed in, aborting!\n", __FUNCTION__);
         STORAGEDBG("\n******************************************\n");
         return nfTSR_NOT_FOUND;
     }
     STORAGEDBG("%s: new state is %d\n", __FUNCTION__, State);

     saStorageAgentState = State;

     return (nfTeeResult_t) Result;

}/* end storageAgentInit */

/**@@@+++@@@@******************************************************************
**
** Microsoft (r) PlayReady (r)
** Copyright (c) Microsoft Corporation. All rights reserved.
**
***@@@---@@@@******************************************************************
*/

#ifndef __DRM2TSMACROS_H__
#define __DRM2TSMACROS_H__ 1

ENTER_PK_NAMESPACE_CODE;

#define DRM_M2TS_IS_PES( eType )                                ( ( (eType) & eDRM_M2TS_PACKET_TYPE_PES ) != 0 )

#define DRM_M2TS_IS_VIDEO_STREAM( eType )                       ( (eType) == DRM_M2TS_STREAM_TYPE_MPEG2_VIDEO ||            \
                                                                  (eType) == DRM_M2TS_STREAM_TYPE_DIGICIPHER_II_VIDEO  ||   \
                                                                  (eType) == DRM_M2TS_STREAM_TYPE_H264 )

#define DRM_M2TS_IS_SUPPORTED_STEAM_TYPE( eType )               ( (eType) == DRM_M2TS_STREAM_TYPE_MPEG2_VIDEO ||            \
                                                                  (eType) == DRM_M2TS_STREAM_TYPE_DIGICIPHER_II_VIDEO  ||   \
                                                                  (eType) == DRM_M2TS_STREAM_TYPE_MPEGA_AUDIO ||            \
                                                                  (eType) == DRM_M2TS_STREAM_TYPE_ADTS ||                   \
                                                                  (eType) == DRM_M2TS_STREAM_TYPE_H264 ||                   \
                                                                  (eType) == DRM_M2TS_STREAM_TYPE_DDPLUS )

#define DRM_M2TS_GET_PID( rgbPacketData )                       ( (DRM_WORD)( ( ( (rgbPacketData)[1] & DRM_M2TS_PID_HIGH_BYTE_MASK ) << 8 ) + (rgbPacketData)[2] ) )
#define DRM_M2TS_IS_UNIT_START_PACKET( rgbPacketData )          ( ( (rgbPacketData)[1] & DRM_M2TS_UNIT_START_MASK ) != 0 )
#define DRM_M2TS_GET_CONTINUITY_COUNTER( rgbPacketData )        ( (DRM_BYTE)( (rgbPacketData)[3] & DRM_M2TS_CONTINUITY_COUNTER_MASK ) )
#define DRM_M2TS_ADAPTATION_FIELD_ONLY( rgbPacketData )         ( ((rgbPacketData)[3] & DRM_M2TS_ADAPTATION_FIELD_MASK) != 0 && ((rgbPacketData)[3] & DRM_M2TS_PAYLOAD_MASK) == 0 )
#define DRM_M2TS_HAS_PAYLOAD( rgbPacketData )                   ( ((rgbPacketData)[3] & DRM_M2TS_PAYLOAD_MASK) != 0 )
#define DRM_M2TS_SET_PAYLOAD_FLAG( rgbPacketData )              (rgbPacketData)[3] |= DRM_M2TS_PAYLOAD_MASK
#define DRM_M2TS_REMOVE_PAYLOAD_FLAG( rgbPacketData )           (rgbPacketData)[3] &= ~DRM_M2TS_PAYLOAD_MASK
#define DRM_M2TS_HAS_ADAPTATION_FIELD( rgbPacketData )          ( (rgbPacketData)[3] & DRM_M2TS_ADAPTATION_FIELD_MASK )
#define DRM_M2TS_SET_ADAPTATION_FIELD( rgbPacketData )          (rgbPacketData)[3] |= DRM_M2TS_ADAPTATION_FIELD_MASK
#define DRM_M2TS_REMOVE_ADAPTATION_FIELD( rgbPacketData )       (rgbPacketData)[3] &= ~DRM_M2TS_ADAPTATION_FIELD_MASK
#define DRM_M2TS_REMOVE_UNIT_START_FLAG( rgbPacketData )        (rgbPacketData)[1] &= ~DRM_M2TS_UNIT_START_MASK
                                                        
#define DRM_M2TS_IS_LAST_OF_LAST_PES_PACKET( pPacket )          ( ( (pPacket)->wPacketFlags & DRM_M2TS_PACKET_FLAG_LAST_OF_LAST_PES_PACKET ) != 0 )
#define DRM_M2TS_IS_START_WITH_PARTIAL_START_CODE( pPacket )    ( ( (pPacket)->wPacketFlags & DRM_M2TS_PACKET_FLAG_START_WITH_PARTIAL_START_CODE ) != 0 )

#define DRM_M2TS_SET_AND_INC_CONTINUITY_COUNTER( rgbPacketData, bCounter )                                      \
    do {                                                                                                        \
        (rgbPacketData)[3] = (DRM_BYTE)( ((rgbPacketData)[3] & ~DRM_M2TS_CONTINUITY_COUNTER_MASK) | bCounter ); \
        bCounter = (DRM_BYTE)( ( bCounter + 1 ) & DRM_M2TS_CONTINUITY_COUNTER_MASK );                           \
    } while( FALSE )


/* Updates the continuity counter in pPacket->rgbData and increment the counter in the stream object */
#define DRM_M2TS_UPDATE_CONTINUITY_COUNTER( pStream, pPacket, dwFlags )                             \
    do {                                                                                            \
        if( ( (dwFlags) & DRM_M2TS_PACKET_FLAG_DISCONTINUITY_PACKET ) != 0 )                        \
        {                                                                                           \
            /* Add 2 to make it discontinued */                                                     \
            (pStream)->bNextContinuityCounter += 2;                                                 \
        }                                                                                           \
        else if( ( (dwFlags) & DRM_M2TS_PACKET_FLAG_DONT_INCREMENT_CONTINUITY_COUNTER ) == 0 )      \
        {                                                                                           \
            (pStream)->bNextContinuityCounter++;                                                    \
        }                                                                                           \
        (pStream)->bNextContinuityCounter &= 0x0F;                                                  \
        (pPacket)->rgbData[3] &= 0xF0;                                                              \
        (pPacket)->rgbData[3] |= (pStream)->bNextContinuityCounter;                                 \
    } while( FALSE )


#define SAFE_M2TS_OEM_FREE( pEncContext, pv, size )                                                 \
    do {                                                                                            \
        if( pv != NULL )                                                                            \
        {                                                                                           \
            DRM_RESULT dr2;                                                                         \
            dr2 = DRM_M2TS_MemFree( (pEncContext), (pv), (size) );                                  \
            DRMASSERT( dr2 == DRM_SUCCESS );                                                        \
            (pv) = NULL;                                                                            \
        }                                                                                           \
    } while( FALSE )

/* Linked list related macros */

#define DRM_M2TS_INIT_LIST( list )                                                                  \
    do {                                                                                            \
        (list).pHead = NULL;                                                                        \
        (list).pTail = NULL;                                                                        \
        (list).cItems = 0;                                                                          \
    } while( FALSE )

#define DRM_M2TS_INSERT_ITEM( pList, pItem, pItemToBeInserted )                                     \
    do {                                                                                            \
        (pItemToBeInserted)->pNext = (pItem)->pNext;                                                \
        (pItem)->pNext = (pItemToBeInserted);                                                       \
        ChkDR( DRM_DWordIncOne( (pList)->cItems ) );                                                \
        if( (pList)->pTail == (pItem) )                                                             \
        {                                                                                           \
            ChkBOOL( (pItemToBeInserted)->pNext == NULL, DRM_E_M2TS_INTERNAL_ERROR );               \
            (pList)->pTail = (pItemToBeInserted);                                                   \
        }                                                                                           \
    }while( FALSE )

#define DRM_M2TS_INSERT_ITEM_ON_PER_UNIT_LIST( pList, pItem, pItemToBeInserted )                    \
    do {                                                                                            \
        (pItemToBeInserted)->pNextPerUnit = (pItem)->pNextPerUnit;                                  \
        (pItem)->pNextPerUnit = (pItemToBeInserted);                                                \
        ChkDR( DRM_DWordIncOne( (pList)->cItems ) );                                                \
        if( (pList)->pTail == (pItem) )                                                             \
        {                                                                                           \
            ChkBOOL( (pItemToBeInserted)->pNextPerUnit == NULL, DRM_E_M2TS_INTERNAL_ERROR );        \
            (pList)->pTail = (pItemToBeInserted);                                                   \
        }                                                                                           \
    }while( FALSE )

/* Veirfy the the number of items on the chain is as expected */
#if DBG
#define DRM_M2TS_VERIFY_LINKED_LIST_LENGTH( pList, type )                                           \
    do {                                                                                            \
        const type *pItem = (pList)->pHead;                                                         \
        DRM_DWORD count = 0;                                                                        \
        while( pItem != NULL )                                                                      \
        {                                                                                           \
            ChkDR( DRM_DWordIncOne( count ) );                                                      \
            pItem = ( type * )pItem->pNext;                                                         \
        }                                                                                           \
        DRMASSERT( count == (pList)->cItems );                                                      \
    } while( FALSE )

#define DRM_M2TS_VERIFY_PER_UNIT_LINKED_LIST_LENGTH( pList, type )                                  \
    do {                                                                                            \
        const type *pItem = (pList)->pHead;                                                         \
        DRM_DWORD count = 0;                                                                        \
        while( pItem != NULL )                                                                      \
        {                                                                                           \
            ChkDR( DRM_DWordIncOne( count ) );                                                      \
            pItem = ( type * )pItem->pNextPerUnit;                                                  \
        }                                                                                           \
        DRMASSERT( count == (pList)->cItems );                                                      \
    } while( FALSE )

#else
#define DRM_M2TS_VERIFY_LINKED_LIST_LENGTH( pList, type )
#define DRM_M2TS_VERIFY_PER_UNIT_LINKED_LIST_LENGTH( pList, type )
#endif /* DBG */

/* Ensure that pItemToCheck is an item on the pList */
#if DBG 
#define DRM_M2TS_ENSURE_ITEM_ON_PER_UNIT_LIST( pList, type, pItemToCheck )                          \
    do {                                                                                            \
        if( pItemToCheck != NULL )                                                                  \
        {                                                                                           \
            const type *pItem = (pList)->pHead;                                                     \
            while( pItem != NULL )                                                                  \
            {                                                                                       \
                if( pItem == (pItemToCheck) )                                                       \
                {                                                                                   \
                    break;                                                                          \
                }                                                                                   \
                pItem = ( type * )pItem->pNextPerUnit;                                              \
            }                                                                                       \
            DRMASSERT( pItem != NULL );                                                             \
        }                                                                                           \
    } while( FALSE )
#else
#define DRM_M2TS_ENSURE_ITEM_ON_PER_UNIT_LIST( pList, type, pItemToCheck )
#endif /* DBG */

/* Free every item on the list */
#define DRM_M2TS_FREE_LIST( pEncContext, pList, type )                                              \
    do {                                                                                            \
        type *pNext = NULL;                                                                         \
                                                                                                    \
        while( (pList)->pHead != NULL )                                                             \
        {                                                                                           \
            pNext = (pList)->pHead->pNext;                                                          \
                                                                                                    \
            ChkDR( DRM_M2TS_MemFree( pEncContext, (pList)->pHead, SIZEOF( type ) ) );               \
                                                                                                    \
            (pList)->pHead = pNext;                                                                 \
            ChkDR( DRM_DWordDecOne( (pList)->cItems ) );                                            \
        }                                                                                           \
                                                                                                    \
        DRMASSERT( (pList)->cItems == 0 );                                                          \
                                                                                                    \
        (pList)->cItems = 0;                                                                        \
        (pList)->pTail = NULL;                                                                      \
                                                                                                    \
    }  while( FALSE )

/* Adds an item to the end of a linked list */
#define DRM_M2TS_APPEND_TO_END_OF_LIST( pList, pItem )                                              \
    do {                                                                                            \
        if( (pList)->pTail == NULL )                                                                \
        {                                                                                           \
            DRMASSERT( (pList)->cItems == 0                                                         \
                    && (pList)->pHead == NULL                                                       \
                    && (pList)->pTail == NULL );                                                    \
            (pList)->pHead = pItem;                                                                 \
            (pList)->pTail = pItem;                                                                 \
        }                                                                                           \
        else                                                                                        \
        {                                                                                           \
            (pList)->pTail->pNext = pItem;                                                          \
            (pList)->pTail = pItem;                                                                 \
        }                                                                                           \
        (pItem)->pNext = NULL;                                                                      \
        ChkDR( DRM_DWordIncOne( (pList)->cItems ) );                                                \
    } while( FALSE )

/* Adds an item to the end of a per unit linked list */
#define DRM_M2TS_APPEND_TO_END_OF_PER_UNIT_LIST( pList, pItem )                                     \
    do {                                                                                            \
        if( (pList)->pTail == NULL )                                                                \
        {                                                                                           \
            DRMASSERT( (pList)->cItems == 0                                                         \
                    && (pList)->pHead == NULL                                                       \
                    && (pList)->pTail == NULL );                                                    \
            (pList)->pHead = pItem;                                                                 \
            (pList)->pTail = pItem;                                                                 \
        }                                                                                           \
        else                                                                                        \
        {                                                                                           \
            (pList)->pTail->pNextPerUnit = pItem;                                                   \
            (pList)->pTail = pItem;                                                                 \
        }                                                                                           \
        (pItem)->pNextPerUnit = NULL;                                                               \
        ChkDR( DRM_DWordIncOne( (pList)->cItems ) );                                                \
    } while( FALSE )

/* Reset the list to its original tail */
#define DRM_M2TS_RESET_PER_UNIT_LIST_TO_ORIGINAL_TAIL( pList, type, pOriginalTailPacket )           \
    do {                                                                                            \
        if( pOriginalTailPacket == NULL )                                                           \
        {                                                                                           \
            DRMASSERT( (pList)->cItems == 1 );                                                      \
            (pList)->pHead = NULL;                                                                  \
            (pList)->pTail = NULL;                                                                  \
            (pList)->cItems = 0;                                                                    \
        }                                                                                           \
        else                                                                                        \
        {                                                                                           \
            const type *pItem = (pOriginalTailPacket)->pNextPerUnit;                                \
            while( pItem != NULL )                                                                  \
            {                                                                                       \
                ChkDR( DRM_DWordDecOne( (pList)->cItems ) );                                        \
                pItem = pItem->pNextPerUnit;                                                        \
            }                                                                                       \
            (pList)->pTail = pOriginalTailPacket;                                                   \
            (pList)->pTail->pNextPerUnit = NULL;                                                    \
        }                                                                                           \
    } while( FALSE )

#define DRM_M2TS_RESET_PER_UNIT_LIST_TO_ORIGINAL_TAIL_NO_FAIL( pList, type, pOriginalTailPacket )   \
    do {                                                                                            \
        if( pOriginalTailPacket == NULL )                                                           \
        {                                                                                           \
            DRMASSERT( (pList)->cItems == 1 );                                                      \
            (pList)->pHead = NULL;                                                                  \
            (pList)->pTail = NULL;                                                                  \
            (pList)->cItems = 0;                                                                    \
        }                                                                                           \
        else                                                                                        \
        {                                                                                           \
            const type *pItem = (pOriginalTailPacket)->pNextPerUnit;                                \
            while( pItem != NULL )                                                                  \
            {                                                                                       \
                (pList)->cItems--;                                                                  \
                pItem = pItem->pNextPerUnit;                                                        \
            }                                                                                       \
            (pList)->pTail = pOriginalTailPacket;                                                   \
            (pList)->pTail->pNextPerUnit = NULL;                                                    \
        }                                                                                           \
    } while( FALSE )

/* Add a list (sublist) to the top of another (master) list */
#define DRM_M2TS_ADD_LIST_TO_TOP_LIST( pMasterList, type, pSublist )                                \
    do {                                                                                            \
        if( (pMasterList)->pHead == NULL )                                                          \
        {                                                                                           \
            DRMASSERT( (pMasterList)->pTail == NULL );                                              \
            DRMASSERT( (pMasterList)->cItems == 0 );                                                \
            *pMasterList = *pSublist;                                                               \
        }                                                                                           \
        else                                                                                        \
        {                                                                                           \
            (pSublist)->pTail->pNext = (pMasterList)->pHead;                                        \
            (pMasterList)->pHead = (pSublist)->pHead;                                               \
            ChkDR( DRM_DWordAdd( (pMasterList)->cItems,                                             \
                                 (pSublist)->cItems,                                                \
                                 &(pMasterList)->cItems ) );                                        \
        }                                                                                           \
    } while( FALSE )

#define DRM_M2TS_ADD_LIST_TO_TOP_LIST_NO_FAIL( pMasterList, type, pSublist )                        \
    do {                                                                                            \
        if( (pMasterList)->pHead == NULL )                                                          \
        {                                                                                           \
            DRMASSERT( (pMasterList)->pTail == NULL );                                              \
            DRMASSERT( (pMasterList)->cItems == 0 );                                                \
            *pMasterList = *pSublist;                                                               \
        }                                                                                           \
        else                                                                                        \
        {                                                                                           \
            (pSublist)->pTail->pNext = (pMasterList)->pHead;                                        \
            (pMasterList)->pHead = (pSublist)->pHead;                                               \
            (pMasterList)->cItems += (pSublist)->cItems;                                            \
        }                                                                                           \
    } while( FALSE )

/* Push an item to the top of the list */
#define DRM_M2TS_PUSH_TO_TOP_LIST( pList, pItem )                                                   \
    do {                                                                                            \
        DRMASSERT( (pItem) != NULL );                                                               \
        __analysis_assume( (pItem) != NULL );                                                       \
        DRMASSERT( (pList)->cItems < MAX_UNSIGNED_TYPE(DRM_DWORD) );                                \
        __analysis_assume( (pList)->cItems < MAX_UNSIGNED_TYPE(DRM_DWORD) );                        \
        (pItem)->pNext = (pList)->pHead;                                                            \
        (pList)->pHead = pItem;                                                                     \
        if( (pList)->pTail == NULL )                                                                \
        {                                                                                           \
            (pList)->pTail = (pList)->pHead;                                                        \
        }                                                                                           \
        ChkDR( DRM_DWordIncOne( (pList)->cItems ) );                                                \
    } while( FALSE )

#define DRM_M2TS_PUSH_TO_TOP_LIST_NO_FAIL( pList, pItem )                                           \
    do {                                                                                            \
        DRMASSERT( (pItem) != NULL );                                                               \
        __analysis_assume( (pItem) != NULL );                                                       \
        DRMASSERT( (pList)->cItems < MAX_UNSIGNED_TYPE(DRM_DWORD) );                                \
        __analysis_assume( (pList)->cItems < MAX_UNSIGNED_TYPE(DRM_DWORD) );                        \
        (pItem)->pNext = (pList)->pHead;                                                            \
        (pList)->pHead = pItem;                                                                     \
        if( (pList)->pTail == NULL )                                                                \
        {                                                                                           \
            (pList)->pTail = (pList)->pHead;                                                        \
        }                                                                                           \
        (pList)->cItems += 1;                                                                       \
    } while( FALSE )

/* Pop the top item from the per unit list */
#define DRM_M2TS_POP_FROM_TOP_PER_UNIT_LIST( pList, type, pItem )                                   \
    do {                                                                                            \
        DRMASSERT( (pList) != NULL );                                                               \
        __analysis_assume( (pList) != NULL );                                                       \
        DRMASSERT( (pList)->cItems > 0 );                                                           \
        __analysis_assume( (pList)->cItems != 0 );                                                  \
        DRMASSERT( (pList)->pHead != NULL );                                                        \
        __analysis_assume( (pList)->pHead != NULL );                                                \
        DRMASSERT( (pList)->pTail != NULL );                                                        \
        __analysis_assume( (pList)->pTail != NULL );                                                \
                                                                                                    \
        (pItem) = (pList)->pHead;                                                                   \
        (pList)->pHead = ( type * )(pList)->pHead->pNextPerUnit;                                    \
        if( (pList)->pHead == NULL )                                                                \
        {                                                                                           \
            (pList)->pTail = NULL;                                                                  \
        }                                                                                           \
        ChkDR( DRM_DWordDecOne( (pList)->cItems ) );                                                \
                                                                                                    \
    } while( FALSE )

#define DRM_M2TS_POP_FROM_TOP_PER_UNIT_LIST_NO_FAIL( pList, type, pItem )                           \
    do {                                                                                            \
        DRMASSERT( (pList) != NULL );                                                               \
        __analysis_assume( (pList) != NULL );                                                       \
        DRMASSERT( (pList)->cItems > 0 );                                                           \
        __analysis_assume( (pList)->cItems != 0 );                                                  \
        DRMASSERT( (pList)->pHead != NULL );                                                        \
        __analysis_assume( (pList)->pHead != NULL );                                                \
        DRMASSERT( (pList)->pTail != NULL );                                                        \
        __analysis_assume( (pList)->pTail != NULL );                                                \
                                                                                                    \
        (pItem) = (pList)->pHead;                                                                   \
        (pList)->pHead = ( type * )(pList)->pHead->pNextPerUnit;                                    \
        if( (pList)->pHead == NULL )                                                                \
        {                                                                                           \
            (pList)->pTail = NULL;                                                                  \
        }                                                                                           \
        (pList)->cItems -= 1;                                                                       \
                                                                                                    \
    } while( FALSE )


/* Pop the top item from the list */
#define DRM_M2TS_POP_FROM_TOP_LIST( pList, type, pItem )                                            \
    do {                                                                                            \
        ChkArg( (pList)->cItems > 0 );                                                              \
        DRMASSERT( (pList)->pHead != NULL );                                                        \
        DRMASSERT( (pList)->pTail != NULL );                                                        \
                                                                                                    \
        (pItem) = (pList)->pHead;                                                                   \
        (pList)->pHead = ( type * )(pList)->pHead->pNext;                                           \
        if( (pList)->pHead == NULL )                                                                \
        {                                                                                           \
            (pList)->pTail = NULL;                                                                  \
        }                                                                                           \
        ChkDR( DRM_DWordDecOne( (pList)->cItems ) );                                                \
                                                                                                    \
    } while( FALSE )

/* Mark each packet on the per unit list to eState */
#define DRM_M2TS_SET_PACKET_STATE_ON_PERUNIT_LIST( type, pList, eState )                            \
    do {                                                                                            \
        type *pPacket = (pList)->pHead;                                                             \
        while( pPacket != NULL )                                                                    \
        {                                                                                           \
            pPacket->ePacketState = eState;                                                         \
            pPacket = pPacket->pNextPerUnit;                                                        \
        }                                                                                           \
    } while( FALSE )

/* MISC */
#define DRM_M2TS_INCREASE_SUBSAMPLE_BYTE_COUNT( poSubsampleContext, pSubsample, count )             \
    do {                                                                                            \
        if( (poSubsampleContext)->fProcessProtected )                                               \
        {                                                                                           \
            ChkDR( DRM_DWordAdd( (pSubsample)->cbEncryptedBytes,                                    \
                                 (count),                                                           \
                                 &(pSubsample)->cbEncryptedBytes ) );                               \
        }                                                                                           \
        else                                                                                        \
        {                                                                                           \
            ChkDR( DRM_WordAdd( (pSubsample)->cbClearBytes,                                         \
                                (count),                                                            \
                                &(pSubsample)->cbClearBytes ) );                                    \
        }                                                                                           \
    } while( FALSE )

#define DRM_M2TS_DECREASE_SUBSAMPLE_BYTE_COUNT( poSubsampleContext, pSubsample, count )             \
    do {                                                                                            \
        if( (poSubsampleContext)->fProcessProtected )                                               \
        {                                                                                           \
            ChkDR( DRM_DWordSub( (pSubsample)->cbEncryptedBytes,                                    \
                                 (count),                                                           \
                                 &(pSubsample)->cbEncryptedBytes ) );                               \
        }                                                                                           \
        else                                                                                        \
        {                                                                                           \
            ChkDR( DRM_WordSub( (pSubsample)->cbClearBytes,                                         \
                                (count),                                                            \
                                &(pSubsample)->cbClearBytes ) );                                    \
        }                                                                                           \
    } while( FALSE )

#define DRM_M2TS_SAMPLE_COUNT( pPES, cSamples )                                                     \
    do {                                                                                            \
        if( (pPES)->fHasSample )                                                                    \
        {                                                                                           \
            ChkDR( DRM_DWordSub( (pPES)->dwIndexLastSample,                                         \
                                 (pPES)->dwIndexFirstSample,                                        \
                                 &(cSamples) ) );                                                   \
            ChkDR( DRM_DWordIncOne( (cSamples) ) );                                                 \
        }                                                                                           \
        else                                                                                        \
        {                                                                                           \
            (cSamples) = 0;                                                                         \
        }                                                                                           \
    } while( FALSE )

#define DRM_M2TS_SUBSAMPLE_COUNT( pSample, cSubsamples )                                            \
    do {                                                                                            \
        if( (pSample)->fHasSubsample )                                                              \
        {                                                                                           \
            ChkDR( DRM_DWordSub( (pSample)->dwIndexLastSubsample,                                   \
                                 (pSample)->dwIndexFirstSubsample,                                  \
                                 &(cSubsamples) ) );                                                \
            ChkDR( DRM_DWordIncOne( (cSubsamples) ) );                                              \
        }                                                                                           \
        else                                                                                        \
        {                                                                                           \
            (cSubsamples) = 0;                                                                      \
        }                                                                                           \
    } while( FALSE )

EXIT_PK_NAMESPACE_CODE;

#endif  /* __DRM2TSMACROS_H__ */

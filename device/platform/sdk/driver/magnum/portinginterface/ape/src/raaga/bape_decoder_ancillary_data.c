/***************************************************************************
 i     Copyright (c) 2006-2012, Broadcom Corporation
 *     All Rights Reserved
 *     Confidential Property of Broadcom Corporation
 *
 *  THIS SOFTWARE MAY ONLY BE USED SUBJECT TO AN EXECUTED SOFTWARE LICENSE
 *  AGREEMENT  BETWEEN THE USER AND BROADCOM.  YOU HAVE NO RIGHT TO USE OR
 *  EXPLOIT THIS MATERIAL EXCEPT SUBJECT TO THE TERMS OF SUCH AN AGREEMENT.
 *
 * $brcm_Workfile: bape_decoder_ancillary_data.c $
 * $brcm_Revision: Hydra_Software_Devel/2 $
 * $brcm_Date: 10/9/12 2:30a $
 *
 * Module Description: Audio Decoder Interface
 *
 * Revision History:
 *
 * $brcm_Log: /magnum/portinginterface/ape/src/raaga/bape_decoder_ancillary_data.c $
 * 
 * Hydra_Software_Devel/2   10/9/12 2:30a jgarrett
 * SW7346-914: Adding init flag
 * 
 * Hydra_Software_Devel/1   10/7/12 8:19p jgarrett
 * SW7346-914: Merge to main branch
 * 
 * Hydra_Software_Devel/SW7346-914/4   10/7/12 6:50p jgarrett
 * SW7346-914: Initial working version of ancillary data
 * 
 * Hydra_Software_Devel/SW7346-914/2   10/3/12 3:54a jgarrett
 * SW7346-914: Initializing ancillary data structure prior to start and
 * enabling configuration
 * 
 * Hydra_Software_Devel/SW7346-914/1   9/27/12 2:52p jgarrett
 * SW7346-914: Initial version of ancillary data parser
 * 
 ***************************************************************************/

#include "bape.h"
#include "bape_priv.h"

BDBG_MODULE(bape_decoder_ancillary_data);

void BAPE_Decoder_P_AdvanceAncDspRead(BAPE_DecoderHandle hDecoder, unsigned amount, void *pBase, void **pRead, unsigned *pNumBytes, unsigned *pNumWrapBytes)
{
    uint32_t newReadAddr;
    BDSP_AF_P_sDRAM_CIRCULAR_BUFFER *pDescriptor;
    unsigned dspBytes, dspWrapBytes;
    BDBG_MSG(("Consuming %u bytes from DSP buffer (pre-wrap %u post-wrap %u)", amount, *pNumBytes, *pNumWrapBytes));
    BDBG_ASSERT(amount <= (*pNumBytes + *pNumWrapBytes));
    pDescriptor = hDecoder->pAncDataBufferDescriptor;
    newReadAddr = pDescriptor->ui32ReadAddr + amount;
    if ( newReadAddr >= pDescriptor->ui32EndAddr )
    {
        newReadAddr -= hDecoder->ancDataBufferSize;
    }
    BDBG_ASSERT(newReadAddr >= pDescriptor->ui32BaseAddr);
    BDBG_ASSERT(newReadAddr < pDescriptor->ui32EndAddr);
    pDescriptor->ui32ReadAddr = newReadAddr;
    if ( pDescriptor->ui32WriteAddr >= pDescriptor->ui32ReadAddr )
    {
        dspBytes = pDescriptor->ui32WriteAddr - pDescriptor->ui32ReadAddr;
        dspWrapBytes = 0;
    }
    else
    {
        dspBytes = pDescriptor->ui32EndAddr - pDescriptor->ui32ReadAddr;
        dspWrapBytes = pDescriptor->ui32WriteAddr - pDescriptor->ui32BaseAddr;
    }
    *pRead = (void *)((unsigned)pBase + (pDescriptor->ui32ReadAddr-pDescriptor->ui32BaseAddr));
    *pNumBytes = dspBytes;
    *pNumWrapBytes = dspWrapBytes;
}

BERR_Code BAPE_Decoder_P_ReadAncData(BAPE_DecoderHandle hDecoder, unsigned amount, void *pBuffer, const void *pBase, const void *pRead, unsigned numBytes, unsigned numWrapBytes)
{
    const uint32_t *pSrc = (const uint32_t *)pRead;
    uint32_t *pDest = (uint32_t *)pBuffer;

    BSTD_UNUSED(hDecoder);

    if ( amount > (numBytes + numWrapBytes) )
    {
        /* Intentionally omitted BERR_TRACE */
        return BERR_TIMEOUT;
    }
    BDBG_ASSERT(amount % 4 == 0);
    BDBG_ASSERT(numBytes % 4 == 0);
    BDBG_ASSERT(numWrapBytes % 4 == 0);

    while ( amount > 0 && numBytes > 0 )
    {
        *pDest++ = *pSrc++;
        amount -= 4;
        numBytes -= 4;
    }
    if ( amount > 0 && numWrapBytes > 0 )
    {
        pSrc = pBase;
        while ( amount > 0 && numWrapBytes > 0 )
        {
            *pDest++ = *pSrc++;
            amount -= 4;
            numWrapBytes -= 4;
        }
    }

    return BERR_SUCCESS;
}

static void BAPE_Decoder_P_WriteHostAncData(
    BAPE_DecoderHandle hDecoder, 
    const void *pData, 
    size_t dataSize, 
    size_t paddingBytes, 
    unsigned *pNumBytes, 
    unsigned *pNumWrapBytes
    )
{
    void *pHostCached;

    BDBG_ASSERT((dataSize + paddingBytes) <= (*pNumBytes + *pNumWrapBytes));

    (void)BMEM_Heap_ConvertAddressToCached(hDecoder->deviceHandle->memHandle, hDecoder->pAncDataHostBuffer, &pHostCached);

    BDBG_MSG(("WriteHost %u/%u %u/%u %#x", dataSize, paddingBytes, *pNumBytes, *pNumWrapBytes, ((unsigned)pHostCached) + hDecoder->ancDataBufferWrite));

    if ( dataSize > 0 )
    {
        if ( dataSize > *pNumBytes )
        {
            BKNI_Memcpy((uint8_t *)hDecoder->pAncDataHostBuffer + hDecoder->ancDataBufferWrite, pData, *pNumBytes);
            pData = (const uint8_t *)pData + *pNumBytes;
            dataSize -= *pNumBytes;
            *pNumBytes = *pNumWrapBytes;
            hDecoder->ancDataBufferWrite = 0;
        }
        BKNI_Memcpy((uint8_t *)pHostCached + hDecoder->ancDataBufferWrite, pData, dataSize);
    }
    *pNumBytes = *pNumBytes - dataSize;
    hDecoder->ancDataBufferWrite += dataSize;
    if ( *pNumBytes == 0 )
    {
        *pNumBytes = *pNumWrapBytes;
    }
    BDBG_ASSERT(hDecoder->ancDataBufferWrite <= hDecoder->ancDataBufferSize);
    if ( hDecoder->ancDataBufferWrite == hDecoder->ancDataBufferSize )
    {
        hDecoder->ancDataBufferWrite = 0;
        hDecoder->ancDataBufferWriteWrap = !hDecoder->ancDataBufferWriteWrap;
    }

    if ( paddingBytes > *pNumBytes )
    {
        BKNI_Memset((uint8_t *)hDecoder->pAncDataHostBuffer + hDecoder->ancDataBufferWrite, 0, *pNumBytes);
        pData = (const uint8_t *)pData + *pNumBytes;
        paddingBytes -= *pNumBytes;
        *pNumBytes = *pNumWrapBytes;
        hDecoder->ancDataBufferWrite = 0;
    }
    BKNI_Memset((uint8_t *)hDecoder->pAncDataHostBuffer + hDecoder->ancDataBufferWrite, 0, paddingBytes);
    *pNumBytes = *pNumBytes - paddingBytes;
    hDecoder->ancDataBufferWrite += paddingBytes;
    if ( *pNumBytes == 0 )
    {
        *pNumBytes = *pNumWrapBytes;
    }
    BDBG_ASSERT(hDecoder->ancDataBufferWrite <= hDecoder->ancDataBufferSize);
    if ( hDecoder->ancDataBufferWrite == hDecoder->ancDataBufferSize )
    {
        hDecoder->ancDataBufferWrite = 0;
        hDecoder->ancDataBufferWriteWrap = !hDecoder->ancDataBufferWriteWrap;
    }    
}

BERR_Code BAPE_Decoder_GetAncillaryDataBuffer(
    BAPE_DecoderHandle hDecoder,
    void **pBuffer, /* [out] pointer to ancillary data buffer */
    size_t *pSize   /* [out] size of data buffer in bytes */
    )
{
    unsigned dspBytes, dspWrapBytes, hostBytes, hostWrapBytes;
    BDSP_AF_P_sDRAM_CIRCULAR_BUFFER *pDescriptor;
    void *pDspCached, *pHostCached;    
    uint32_t *pDspRead, value;
    BDSP_Raaga_Audio_MpegAncDataPacket packet;
    BAPE_AncillaryDataHeader hostHeader;
    BERR_Code errCode;

    BDBG_OBJECT_ASSERT(hDecoder, BAPE_Decoder);

    errCode = BMEM_ConvertAddressToCached(hDecoder->deviceHandle->memHandle, hDecoder->pAncDataDspBuffer, &pDspCached);
    if ( errCode )
    {
        return BERR_TRACE(errCode);
    }
    errCode = BMEM_ConvertAddressToCached(hDecoder->deviceHandle->memHandle, hDecoder->pAncDataHostBuffer, &pHostCached);
    if ( errCode )
    {
        return BERR_TRACE(errCode);
    }

    /* TODO: This is not cache-safe so using uncached for now.  Cache bursts will potentially overwrite the DSP write pointer */
    pDescriptor = hDecoder->pAncDataBufferDescriptor;
    if ( pDescriptor->ui32WriteAddr >= pDescriptor->ui32ReadAddr )
    {
        dspBytes = pDescriptor->ui32WriteAddr - pDescriptor->ui32ReadAddr;
        dspWrapBytes = 0;
    }
    else
    {
        dspBytes = pDescriptor->ui32EndAddr - pDescriptor->ui32ReadAddr;
        dspWrapBytes = pDescriptor->ui32WriteAddr - pDescriptor->ui32BaseAddr;
    }
    /* These are offsets, not addresses */
    BDBG_MSG(("Host Read %u Write %u Size %u", hDecoder->ancDataBufferRead, hDecoder->ancDataBufferWrite, hDecoder->ancDataBufferSize));
    if ( hDecoder->ancDataBufferWrite == hDecoder->ancDataBufferRead )
    {
        if ( hDecoder->ancDataBufferWriteWrap == hDecoder->ancDataBufferReadWrap )
        {
            /* Buffer is empty */
            hostBytes = hDecoder->ancDataBufferSize - hDecoder->ancDataBufferWrite;
            hostWrapBytes = hDecoder->ancDataBufferRead;
        }
        else
        {
            /* Buffer is full */
            hostBytes = 0;
            hostWrapBytes = 0;
        }
    }
    if ( hDecoder->ancDataBufferWrite < hDecoder->ancDataBufferRead )
    {
        hostBytes = hDecoder->ancDataBufferRead - hDecoder->ancDataBufferWrite;
        hostWrapBytes = 0;
    }
    else
    {
        hostBytes = hDecoder->ancDataBufferSize - hDecoder->ancDataBufferWrite;
        hostWrapBytes = hDecoder->ancDataBufferRead;
    }
    BDBG_MSG(("%u/%u bytes in host buffer", hostBytes, hostWrapBytes));
    
    pDspRead = (uint32_t *)((unsigned)pDspCached + (pDescriptor->ui32ReadAddr - pDescriptor->ui32BaseAddr));
    /* Invalidate cache for data we are about to read */
    (void)BMEM_Heap_FlushCache(hDecoder->deviceHandle->memHandle, pDspRead, dspBytes);
    if ( dspWrapBytes > 0 )
    {
        (void)BMEM_Heap_FlushCache(hDecoder->deviceHandle->memHandle, pDspCached, dspWrapBytes);
    }

    BDBG_ASSERT(dspBytes % 4 == 0);

    while ( dspBytes > 0 && hostBytes > 0 )
    {
        unsigned numDataBytes;

        /* Read packet header into local memory */
        if ( BAPE_Decoder_P_ReadAncData(hDecoder, sizeof(packet), &packet, pDspCached, pDspRead, dspBytes, dspWrapBytes) )
        {
            /* Insufficient Data.  Return. */
            BDBG_MSG(("Insufficient data in DSP buffer %u bytes of %u/%u", sizeof(packet), dspBytes, dspWrapBytes));
            goto done;
        }
#ifndef BDSP_AF_P_MPEG_ANC_DATA_SYNCWORD
#define BDSP_AF_P_MPEG_ANC_DATA_SYNCWORD (0x4d504547)
#endif
        /* See if syncword matches */
        if ( packet.ui32Syncword != BDSP_AF_P_MPEG_ANC_DATA_SYNCWORD )
        {
            /* Syncword mismatch.  Consume first word and retry */
            BDBG_MSG(("Syncword mismatch - expecting %#x got %#x", BDSP_AF_P_MPEG_ANC_DATA_SYNCWORD, packet.ui32Syncword));
            BAPE_Decoder_P_AdvanceAncDspRead(hDecoder, 4, pDspCached, (void **)&pDspRead, &dspBytes, &dspWrapBytes);
            continue;
        }
        else
        {
            BDBG_MSG(("Syncword match"));
        }

        numDataBytes = (packet.ui32AncDataBitsWritten + 7) / 8;
        numDataBytes = (numDataBytes+3) & ~3; /* Round off to 32-bit boundary */
        BDBG_MSG(("Packet has %u bytes (%u bits)", numDataBytes, packet.ui32AncDataBitsWritten));
        if ( numDataBytes <= ((dspBytes + dspWrapBytes) - sizeof(packet)) )
        {
            unsigned blockSize, paddingBytes = 0;

            blockSize = sizeof(hostHeader) + numDataBytes;
            /* Few wraparound cases to handle.  If we can't fit another header in the buffer, add sufficient padding to this entry
               so that we fill the circular buffer */
            if ( (hDecoder->ancDataBufferSize - hDecoder->ancDataBufferWrite) < sizeof(hostHeader) )
            {
                blockSize += hDecoder->ancDataBufferSize - hDecoder->ancDataBufferWrite;
            }
            /* If the block won't fit before wraparound we need to create a dummy entry to fill the pre-wrap space. */
            if ( blockSize > hostBytes && hostWrapBytes > 0 )
            {
                hostHeader.blockSize = hostBytes;
                hostHeader.payloadSize = 0;
                hostHeader.payloadSizeBits = 0;
                hostHeader.frameNumber = 0;
                BDBG_MSG(("Host Payload Size %u bytes", hostHeader.payloadSize));
                BAPE_Decoder_P_WriteHostAncData(hDecoder, &hostHeader, sizeof(hostHeader), hostHeader.blockSize-sizeof(hostHeader), &hostBytes, &hostWrapBytes);
                BDBG_MSG(("WroteDummy - rd %u wr %u", hDecoder->ancDataBufferRead, hDecoder->ancDataBufferWrite));
                continue;
            }
            /* If another packet header can not fit in the buffer, add padding bytes to fill the remainder */
            if ( hDecoder->ancDataBufferSize - hDecoder->ancDataBufferWrite < (sizeof(hostHeader)+blockSize) )
            {
                if ( blockSize <= hostBytes )
                {
                    BDBG_MSG(("Buffer about to wrap - Write %u Size %u block %u - adding %u padding bytes",
                              hDecoder->ancDataBufferWrite, hDecoder->ancDataBufferSize, blockSize, 
                              (hDecoder->ancDataBufferSize - hDecoder->ancDataBufferWrite) - blockSize));
                    paddingBytes = (hDecoder->ancDataBufferSize - hDecoder->ancDataBufferWrite) - blockSize;
                }
                else
                {
                    BDBG_MSG(("Padding bytes required, but insufficient space in host buffer"));
                }
            }
            if ( blockSize <= hostBytes )
            {
                hostHeader.blockSize = blockSize;
                hostHeader.payloadSize = (packet.ui32AncDataBitsWritten+7)/8;
                hostHeader.payloadSizeBits = packet.ui32AncDataBitsWritten;
                hostHeader.frameNumber = packet.ui32FrameNumber;

                /* consume packet header from dsp */
                BAPE_Decoder_P_AdvanceAncDspRead(hDecoder, sizeof(BDSP_Raaga_Audio_MpegAncDataPacket), pDspCached, (void **)&pDspRead, &dspBytes, &dspWrapBytes);
                /* Write host header */
                BDBG_MSG(("Host Payload Size %u bytes blockSize %u bytes", hostHeader.payloadSize, hostHeader.blockSize));
                BAPE_Decoder_P_WriteHostAncData(hDecoder, &hostHeader, sizeof(hostHeader), 0, &hostBytes, &hostWrapBytes);
                /* Get Data and write a bytestream */
                while ( numDataBytes > 0 )
                {
                    unsigned i;
                    (void)BAPE_Decoder_P_ReadAncData(hDecoder, 4, &value, pDspCached, pDspRead, dspBytes, dspWrapBytes);
                    BAPE_Decoder_P_AdvanceAncDspRead(hDecoder, 4, pDspCached, (void **)&pDspRead, &dspBytes, &dspWrapBytes);
                    for ( i = 0; i < 4; i++ )
                    {
                        uint8_t dataByte;
                        dataByte = value>>24;
                        value <<= 8;
                        BAPE_Decoder_P_WriteHostAncData(hDecoder, &dataByte, 1, 0, &hostBytes, &hostWrapBytes);                        
                    }
                    numDataBytes-=4;
                }
                if ( paddingBytes > 0 )
                {
                    /* Add padding bytes to fill remainder of buffer before wraparound */
                    BAPE_Decoder_P_WriteHostAncData(hDecoder, &hostHeader, 0, paddingBytes, &hostBytes, &hostWrapBytes);
                }
                BDBG_MSG(("WrotePacket - rd %u wr %u", hDecoder->ancDataBufferRead, hDecoder->ancDataBufferWrite));
                /* JDG - Temp */
                goto done;
            }
            else
            {
                BDBG_MSG(("Insufficient space in host buffer for packet"));
                goto done;
            }
        }
        else
        {
            BDBG_WRN(("Data payload (%u bytes/%u bits) not available in DSP buffer (%u/%u) - dropping packet", numDataBytes, packet.ui32AncDataBitsWritten, dspBytes, dspWrapBytes));
            BAPE_Decoder_P_AdvanceAncDspRead(hDecoder, 4, pDspCached, (void **)&pDspRead, &dspBytes, &dspWrapBytes);
            continue;
        }
    }

done:
    *pBuffer = (void *)((unsigned)hDecoder->pAncDataHostBuffer + hDecoder->ancDataBufferRead);
    if ( hDecoder->ancDataBufferWrite == hDecoder->ancDataBufferRead )
    {
        if ( hDecoder->ancDataBufferWriteWrap == hDecoder->ancDataBufferReadWrap )
        {
            /* Buffer is empty */
            hostBytes = 0;
            hostWrapBytes = 0;
        }
        else
        {
            hostBytes = hDecoder->ancDataBufferSize - hDecoder->ancDataBufferRead;
            hostWrapBytes = hDecoder->ancDataBufferWrite;
        }
    }
    else if ( hDecoder->ancDataBufferWrite < hDecoder->ancDataBufferRead )
    {
        hostBytes = hDecoder->ancDataBufferSize - hDecoder->ancDataBufferRead;
        hostWrapBytes = hDecoder->ancDataBufferWrite;
    }
    else
    {
        hostBytes = hDecoder->ancDataBufferWrite - hDecoder->ancDataBufferRead;
        hostWrapBytes = 0;
    }
    hDecoder->ancDataInit = false;
    BDBG_MSG(("GetAncillaryData %u %u/%u wr %u rd %u size %u", hostBytes, hostBytes, hostWrapBytes, hDecoder->ancDataBufferWrite, hDecoder->ancDataBufferRead, hDecoder->ancDataBufferSize));
    *pSize = hostBytes;
    return BERR_SUCCESS;
}

/***************************************************************************
Summary:
Consume Decoder Ancillary Data
***************************************************************************/
BERR_Code BAPE_Decoder_ConsumeAncillaryData(
    BAPE_DecoderHandle hDecoder,
    size_t numBytes
    )
{
    unsigned hostBytes, hostWrapBytes;

    BDBG_OBJECT_ASSERT(hDecoder, BAPE_Decoder);

    if ( hDecoder->ancDataInit )
    {
        /* The decoder has been restarted since GetBuffer was last called */
        return BERR_SUCCESS;
    }

    /* These are offsets, not addresses */
    if ( hDecoder->ancDataBufferWrite == hDecoder->ancDataBufferRead )
    {
        if ( hDecoder->ancDataBufferWriteWrap == hDecoder->ancDataBufferReadWrap )
        {
            /* Buffer is empty */
            hostBytes = 0;
            hostWrapBytes = 0;
        }
        else
        {
            hostBytes = hDecoder->ancDataBufferSize - hDecoder->ancDataBufferRead;
            hostWrapBytes = hDecoder->ancDataBufferWrite;
        }
    }
    else if ( hDecoder->ancDataBufferWrite < hDecoder->ancDataBufferRead )
    {
        hostBytes = hDecoder->ancDataBufferSize - hDecoder->ancDataBufferRead;
        hostWrapBytes = hDecoder->ancDataBufferWrite;
    }
    else
    {
        hostBytes = hDecoder->ancDataBufferWrite - hDecoder->ancDataBufferRead;
        hostWrapBytes = 0;
    }

    BDBG_MSG(("ConsumeAncillaryData %u %u/%u wr %u rd %u size %u", numBytes, hostBytes, hostWrapBytes, hDecoder->ancDataBufferWrite, hDecoder->ancDataBufferRead, hDecoder->ancDataBufferSize));

    if ( numBytes > hostBytes + hostWrapBytes )
    {
        BDBG_ERR(("Attempt to consume %u ancillary data bytes but only %u are buffered", numBytes, hostBytes + hostWrapBytes));
        return BERR_TRACE(BERR_INVALID_PARAMETER);
    }

    hDecoder->ancDataBufferRead += numBytes;
    if ( hDecoder->ancDataBufferRead >= hDecoder->ancDataBufferSize )
    {
        hDecoder->ancDataBufferRead -= hDecoder->ancDataBufferSize;
        hDecoder->ancDataBufferReadWrap = !hDecoder->ancDataBufferReadWrap;
    }

    BDBG_MSG(("Consumed - rd %u wr %u", hDecoder->ancDataBufferRead, hDecoder->ancDataBufferWrite));
    
    return BERR_SUCCESS;
}

/***************************************************************************
Summary:
Flush Decoder Ancillary Data
***************************************************************************/
void BAPE_Decoder_FlushAncillaryData(
    BAPE_DecoderHandle hDecoder
    )
{
    BDSP_AF_P_sDRAM_CIRCULAR_BUFFER *pDescriptor;

    BDBG_OBJECT_ASSERT(hDecoder, BAPE_Decoder);

    /* TODO: This is not cache-safe so using uncached for now.  Cache bursts will potentially overwrite the DSP write pointer */
    pDescriptor = hDecoder->pAncDataBufferDescriptor;
    pDescriptor->ui32ReadAddr = pDescriptor->ui32WriteAddr;

    hDecoder->ancDataBufferRead = hDecoder->ancDataBufferWrite;
}

void BAPE_Decoder_P_InitAncillaryDataBuffer(BAPE_DecoderHandle hDecoder)
{
    BDBG_MSG(("InitAncillaryDataBuffer"));
    if ( hDecoder->pAncDataBufferDescriptor )
    {
        uint32_t base, end;
        (void)BMEM_Heap_ConvertAddressToOffset(hDecoder->deviceHandle->memHandle, hDecoder->pAncDataDspBuffer, &base);
        end = base + hDecoder->ancDataBufferSize;
        /* TODO: Should use cached memory - but it's not safe since both host and dsp write within a cache line */
        hDecoder->pAncDataBufferDescriptor->ui32BaseAddr = base;
        hDecoder->pAncDataBufferDescriptor->ui32ReadAddr = base;
        hDecoder->pAncDataBufferDescriptor->ui32WriteAddr = base;
        hDecoder->pAncDataBufferDescriptor->ui32EndAddr = end;
        hDecoder->pAncDataBufferDescriptor->ui32WrapAddr = end;
    }
    hDecoder->ancDataBufferRead = hDecoder->ancDataBufferWrite = 0;
    hDecoder->ancDataBufferReadWrap = hDecoder->ancDataBufferWriteWrap = false;
    hDecoder->ancDataInit = true;
}

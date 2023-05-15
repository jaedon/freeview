/*****************************************************************************
*                  COPYRIGHT (C) 2011 IRDETO B.V.                            *
******************************************************************************
*          File : UartInterface.c
*                                                                            *
*   Description : Temu is a PC hosted serial communication tool for          *
*                 delivering a download to a single IRD. This module         *
*                 will provide all relevant communication functions          *
*                 between Temu and IRD, according to the communication       *
*                 protocal prepared by Irdeto Loader.                        *
*                                                                            *
*****************************************************************************/


#include "LoaderCorePrivate.h"



#define MAX_BYTES_PER_READ  (0x0000ffff)    /* maximum 16 bits long */


lc_result LC_TemuStart( void )
{
    lc_result result = LC_SUCCESS;
    lc_buffer_st aUARTPacket;
    lc_uchar abCmdSeq[3] = { STX, CMD_STB_START, ETX };
    lc_uchar abTemuSeq[4];

    aUARTPacket.bytes = abCmdSeq;
    aUARTPacket.length = sizeof(abCmdSeq);

    if (LC_SUCCESS == result)
    {
        /* STB -> Temu */
        result = LoaderCoreSPI_UART_Write(&aUARTPacket);
    }

    if (LC_SUCCESS == result)
    {
        /* STB <- Temu { STX, CMD_TEMU_START, DATA_TEMU_START, ETX } */
        aUARTPacket.bytes = abTemuSeq;
        aUARTPacket.length = sizeof(abTemuSeq);
        result = LoaderCoreSPI_UART_Read( &aUARTPacket );
    }

    if (LC_SUCCESS == result)
    {
        TRACE_LOG0(TRACE_DEBUG, "[UART_IFC] Temu Start (STB <- Temu)\n");
        TRACE_LOG1(TRACE_DEBUG, "[UART_IFC] Byte number = %d\n", aUARTPacket.length);
        TRACE_LOG4(TRACE_DEBUG, "[UART_IFC] Byte sequence = 0x%02x 0x%02x 0x%02x 0x%02x\n",
                abTemuSeq[0],
                abTemuSeq[1],
                abTemuSeq[2],
                abTemuSeq[3]);

        if(!((aUARTPacket.length == 4) &&
            (abTemuSeq[0] == STX) &&
            (abTemuSeq[1] == CMD_TEMU_START) &&
            (abTemuSeq[2] == DATA_TEMU_START) &&
            (abTemuSeq[3] == ETX)) )
        {
            result = LC_FAILURE;
        }

        if (LC_SUCCESS == result)
        {
            TRACE_LOG0(TRACE_DEBUG, ("Temu is present.\n"));
        }
    }

    return result;

}


lc_result
LC_TemuDataRequest(
    lc_uchar *pData,
    lc_uint32 length,
    lc_uint32 offset,
    lc_uint32 maxRetry)
{
    lc_result result = LC_SUCCESS;
    lc_uchar abCmdSeq[9];
    lc_uint32 bytesToRead = length;
    lc_uchar *pRead = pData;
    lc_uint32 off = offset;
    lc_uint32 bytesPerRead = 0;
    lc_buffer_st aUARTPacket = {NULL, 0};
    lc_uint32 retry = maxRetry;

    if (0 == bytesToRead)
    {
        result = LC_FAILURE;
    }

    if (LC_SUCCESS == result)
    {
        for (retry = maxRetry; retry > 0; )
        {
            do
            {
                bytesPerRead = MAX_BYTES_PER_READ;
                if (bytesToRead < MAX_BYTES_PER_READ)
                {
                    bytesPerRead = bytesToRead;
                }

                abCmdSeq[0] = STX;
                abCmdSeq[1] = CMD_STB_DATA_REQUEST;
                abCmdSeq[2] = (lc_uchar)(bytesPerRead & 0xFF);         /* LSB of length */
                abCmdSeq[3] = (lc_uchar)((bytesPerRead >> 8) & 0xFF);  /* MSB of length */
                abCmdSeq[4] = (lc_uchar)(off & 0xFF);         /* LSB of offset */
                abCmdSeq[5] = (lc_uchar)((off >>  8) & 0xFF);
                abCmdSeq[6] = (lc_uchar)((off >> 16) & 0xFF);
                abCmdSeq[7] = (lc_uchar)((off >> 24) & 0xFF); /* MSB of offset */
                abCmdSeq[8] = ETX;

                aUARTPacket.bytes = abCmdSeq;
                aUARTPacket.length = sizeof(abCmdSeq);

                /* STB -> Temu */
                result = LoaderCoreSPI_UART_Write(&aUARTPacket);

                /* STB <- Temu { data[wLength] }
                    Data sent from Temu to the STB in response to a STB_DATA_REQUEST
                    cmd is transmitted as raw data with no STX/ETX framing.
                 */
                if (LC_SUCCESS == result)
                {
                    aUARTPacket.bytes = pRead;
                    aUARTPacket.length = bytesPerRead;
                    result = LoaderCoreSPI_UART_Read(&aUARTPacket);
                    /*DBG(LC_printf("[UART_IFC] lReadByte = 0x%x\n", lReadByte));*/

                    if( aUARTPacket.length != bytesPerRead )
                    {
                        /* read error */
                        result = LC_FAILURE;
                    }
                }

                if (LC_SUCCESS == result)
                {
                    pRead += bytesPerRead;
                    bytesToRead -= bytesPerRead;
                    off += bytesPerRead;
                }

            } while ((bytesToRead > 0) && (LC_SUCCESS == result));

            if (LC_SUCCESS == result)
            {
                break;
            }
            else
            {
                retry--;
            }

        }

        if (0 == retry)
        {
            result = LC_FAILURE;
            ERR_REPORT_SetErrorCode(ERROR_CODE_UART_TIMEOUT);
        }
    }


    return result;

}



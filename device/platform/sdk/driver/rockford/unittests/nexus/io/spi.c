/******************************************************************************
 *    (c)2008 Broadcom Corporation
 *
 * This program is the proprietary software of Broadcom Corporation and/or its licensors,
 * and may only be used, duplicated, modified or distributed pursuant to the terms and
 * conditions of a separate, written license agreement executed between you and Broadcom
 * (an "Authorized License").  Except as set forth in an Authorized License, Broadcom grants
 * no license (express or implied), right to use, or waiver of any kind with respect to the
 * Software, and Broadcom expressly reserves all rights in and to the Software and all
 * intellectual property rights therein.  IF YOU HAVE NO AUTHORIZED LICENSE, THEN YOU
 * HAVE NO RIGHT TO USE THIS SOFTWARE IN ANY WAY, AND SHOULD IMMEDIATELY
 * NOTIFY BROADCOM AND DISCONTINUE ALL USE OF THE SOFTWARE.
 *
 * Except as expressly set forth in the Authorized License,
 *
 * 1.     This program, including its structure, sequence and organization, constitutes the valuable trade
 * secrets of Broadcom, and you shall use all reasonable efforts to protect the confidentiality thereof,
 * and to use this information only in connection with your use of Broadcom integrated circuit products.
 *
 * 2.     TO THE MAXIMUM EXTENT PERMITTED BY LAW, THE SOFTWARE IS PROVIDED "AS IS"
 * AND WITH ALL FAULTS AND BROADCOM MAKES NO PROMISES, REPRESENTATIONS OR
 * WARRANTIES, EITHER EXPRESS, IMPLIED, STATUTORY, OR OTHERWISE, WITH RESPECT TO
 * THE SOFTWARE.  BROADCOM SPECIFICALLY DISCLAIMS ANY AND ALL IMPLIED WARRANTIES
 * OF TITLE, MERCHANTABILITY, NONINFRINGEMENT, FITNESS FOR A PARTICULAR PURPOSE,
 * LACK OF VIRUSES, ACCURACY OR COMPLETENESS, QUIET ENJOYMENT, QUIET POSSESSION
 * OR CORRESPONDENCE TO DESCRIPTION. YOU ASSUME THE ENTIRE RISK ARISING OUT OF
 * USE OR PERFORMANCE OF THE SOFTWARE.
 *
 * 3.     TO THE MAXIMUM EXTENT PERMITTED BY LAW, IN NO EVENT SHALL BROADCOM OR ITS
 * LICENSORS BE LIABLE FOR (i) CONSEQUENTIAL, INCIDENTAL, SPECIAL, INDIRECT, OR
 * EXEMPLARY DAMAGES WHATSOEVER ARISING OUT OF OR IN ANY WAY RELATING TO YOUR
 * USE OF OR INABILITY TO USE THE SOFTWARE EVEN IF BROADCOM HAS BEEN ADVISED OF
 * THE POSSIBILITY OF SUCH DAMAGES; OR (ii) ANY AMOUNT IN EXCESS OF THE AMOUNT
 * ACTUALLY PAID FOR THE SOFTWARE ITSELF OR U.S. $1, WHICHEVER IS GREATER. THESE
 * LIMITATIONS SHALL APPLY NOTWITHSTANDING ANY FAILURE OF ESSENTIAL PURPOSE OF
 * ANY LIMITED REMEDY.
 *
 * $brcm_Workfile: spi.c $
 *
 * Module Description:
 *
 * Revision History:
 *
 * $brcm_Log: /rockford/unittests/nexus/io/spi.c $
 * 
 * Hydra_Software_Devel/6   8/4/10 11:38a erickson
 * SW35230-67: make compile for non-35230
 *
 * Hydra_Software_Devel/5   6/9/10 4:03p tqiu
 * SW35230-67: update SPI test application
 *
 * Hydra_Software_Devel/4   6/9/10 10:00a tqiu
 * SW35230-67: update the spi test code
 *
 * Hydra_Software_Devel/3   5/19/10 3:44p tqiu
 * SW35230-67: update spi test app.
 *
 * Hydra_Software_Devel/2   5/17/10 2:57p tqiu
 * SW35230-67: update SPI test code
 *
 *
 *****************************************************************************/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include "nexus_types.h"
#include "nexus_platform.h"
#include "nexus_spi.h"
#include "bstd.h"
#include "bkni.h"



/*
 * Notes:
 *
 * For 35230 A0 qual board, SPI2, 3, 4 are all used for LVDS/Dimming. SPI testing for these controls require panel to be hooked up.
 * If necessary, we need to probe the signal on the path, or wire up a serial rom for testing.
 *
 * For SPI 0 and 1, we can just change the pin mux to use SPI to read/write the serial flash on the board.
 *
 */

#define SPI_TEST_BUFFER_SIZE     200
#define SPI_TEST_BUFFER_OFFSET   4

#define SPI_TEST_STRING_BUFFER_SIZE    256

#define SPI_MX25_WEL_MASK      0x02
#define SPI_MX25_WIP_MASK      0x01

int g_data_offset = 0;
/*
 * read a string from keyboard
 */
void spi_test_read_string(char *pString)
{
    int      length;
    fgets(pString, SPI_TEST_STRING_BUFFER_SIZE, stdin);
    length = strlen(pString);
    if(length >= 1)
    {
        *(pString+length-1) = 0;
    }
}


/*
 * get input value
 */
int spi_test_get_input(char *pString)
{
    char        str[SPI_TEST_STRING_BUFFER_SIZE];
    int         len = 0;

    while(len == 0)
    {
        printf("%s", pString);
        spi_test_read_string(str);
        if(isalpha(str[0]))   return -1;
        len = strlen(str);
    }
    return atoi(str);
}



/*
 * check MX25 status
 */
int spi_check_mx25_status(unsigned char status, bool bPrint, bool *bWEL, bool *bWIP)
{
    if(bPrint == true)
    {
        printf("\n=========== MX25 Status ==============\n");
        printf("SRWD = %d\n", ((status&0x80)==0x80)? 1:0);
        printf("QE   = %d\n", ((status&0x40)==0x40)? 1:0);
        printf("BP3  = %d\n", ((status&0x20)==0x20)? 1:0);
        printf("BP2  = %d\n", ((status&0x10)==0x10)? 1:0);
        printf("BP1  = %d\n", ((status&0x08)==0x08)? 1:0);
        printf("BP0  = %d\n", ((status&0x04)==0x04)? 1:0);
        printf("WEL  = %d\n", ((status&0x02)==0x02)? 1:0);
        printf("WIP  = %d\n", ((status&0x01)==0x01)? 1:0);
        printf("=======================================\n");
    }

    *bWEL = ((status & SPI_MX25_WEL_MASK) == SPI_MX25_WEL_MASK)? true : false;
    *bWIP = ((status & SPI_MX25_WIP_MASK) == SPI_MX25_WIP_MASK)? true : false;

    return 0;
}



#if NEXUS_DTV_PLATFORM > 2
/* main SPI test */
void spi_test(NEXUS_PlatformConfiguration *pConfig, int index, uint32_t baud, bool clockActiveLow, bool txLeadingCapFalling, bool rxLeading, uint8_t bitsPerTransfer, uint8_t cs, bool dmaMode)
{
    NEXUS_Error            rc;
    NEXUS_SpiHandle        spi;
    NEXUS_SpiSettings      spiSettings;
    uint8_t                writeBuffer[SPI_TEST_BUFFER_SIZE+SPI_TEST_BUFFER_OFFSET];
    uint8_t                tempBuffer[SPI_TEST_BUFFER_SIZE+SPI_TEST_BUFFER_OFFSET];
    uint8_t                readBuffer[SPI_TEST_BUFFER_SIZE+SPI_TEST_BUFFER_OFFSET];
    uint8_t                *ptr;
    int                    i, j, num=8;
    bool                   bCheck=true;
    bool                   bWEL, bWIP;


    /* get default settings */
    NEXUS_Spi_GetDefaultSettings(&spiSettings);


    /* open SPI */
    spiSettings.baud = baud;                                   /* baud rate */
    spiSettings.clockActiveLow = clockActiveLow;               /* CPOL */
    spiSettings.txLeadingCapFalling = txLeadingCapFalling;     /* CPHA */
    spiSettings.bitsPerTransfer = bitsPerTransfer;             /* bits per transfer. max is 32 */
    spiSettings.pcs = cs;                                      /* chip select */
    spiSettings.xferDMAMode = dmaMode;                         /* dma mode flag */
    spiSettings.interruptMode = false;

    spiSettings.rxLeading = rxLeading;

    if(dmaMode == true)
    {
        /* magnum heap handle */
        spiSettings.hHeap = pConfig->memc[0].heap[0];

        /* default buffer size */
        spiSettings.bufferSize = 1024;                         /* size of buffer to be allocated from the heap */
    }
    spi = NEXUS_Spi_Open(index, &spiSettings);
    if(spi == NULL)
    {
        printf("Error!!!  NEXUS_Spi_Open() failed.\n");
        return;
    }


    /*
     * Note: we are using onboard serial flash(M25P80) for SPI testing.
     * Memory organization in M25P80:
     *   (1) 1M bytes total
     *   (2) 16 sectors (64KB each)
     *   (3) 4096 pages (256Bytes each)
     *
     * Here are the steps:
     *   (1) CS to active low
     *   (2) write enable (0x06)
     *   (3) CS to active high
     *   (4) CS to active low
     *   (5) page program data (PP instruction code 0x02 + address + data)
     *   (6) CS to active high
     */
    spiSettings.xferDMAMode = false;
    NEXUS_Spi_SetSettings(spi, &spiSettings);

    /* check status */
    bWEL = bWIP = false;
    writeBuffer[0] = 0x5;             /* write enable code is 0x06 */
    writeBuffer[1] = 0x05;
    readBuffer[1] = 0xAA;
    NEXUS_Spi_Read(spi, writeBuffer, readBuffer, 2);
    spi_check_mx25_status(readBuffer[1], true, &bWEL, &bWIP);
    spiSettings.xferDMAMode = dmaMode;
    NEXUS_Spi_SetSettings(spi, &spiSettings);

#if 1
    printf("************  enable write ************\n");
    spiSettings.xferDMAMode = false;
    NEXUS_Spi_SetSettings(spi, &spiSettings);
    /* enable SPI write */
    writeBuffer[0] = 0x06;             /* write enable code is 0x06 */
    rc = NEXUS_Spi_Write(spi, writeBuffer, 1);
    if(rc != NEXUS_SUCCESS)
    {
        printf("Error!!!  NEXUS_Spi_Write() failed for write_enable. error code = %d\n", rc);
    }
    spiSettings.xferDMAMode = dmaMode;
    NEXUS_Spi_SetSettings(spi, &spiSettings);


    printf("*************** erase block **************\n");
    spiSettings.xferDMAMode = false;
    NEXUS_Spi_SetSettings(spi, &spiSettings);
    /* erase block */
    writeBuffer[0] = 0xd8;
    writeBuffer[1] = writeBuffer[2] = writeBuffer[3] = 0;
    NEXUS_Spi_Write(spi, writeBuffer, 4);
    spiSettings.xferDMAMode = dmaMode;
    NEXUS_Spi_SetSettings(spi, &spiSettings);

    while(1)
    {
        spiSettings.xferDMAMode = false;
        NEXUS_Spi_SetSettings(spi, &spiSettings);
        bWEL = bWIP = false;
        /* check status */
        writeBuffer[0] = 0x05;
        readBuffer[1] = 0xFF;
        NEXUS_Spi_Read(spi, writeBuffer, readBuffer, 2);
        spiSettings.xferDMAMode = dmaMode;
        NEXUS_Spi_SetSettings(spi, &spiSettings);
        spi_check_mx25_status(readBuffer[1], false, &bWEL, &bWIP);
        if(bWIP == false && bWEL == false)   break;
        BKNI_Delay(1000);
    }
#endif

    printf("***************** enable write **********************\n");
    spiSettings.xferDMAMode = false;
    NEXUS_Spi_SetSettings(spi, &spiSettings);
    /* enable SPI write */
    writeBuffer[0] = 0x06;             /* write enable code is 0x06 */
    rc = NEXUS_Spi_Write(spi, writeBuffer, 1);
    spiSettings.xferDMAMode = dmaMode;
    NEXUS_Spi_SetSettings(spi, &spiSettings);
    if(rc != NEXUS_SUCCESS)
    {
        printf("Error!!!  NEXUS_Spi_Write() failed for write_enable. error code = %d\n", rc);
    }

    printf("*****************  PP ************************\n");
    /*
     * page program with data
     */
    spiSettings.xferDMAMode = dmaMode;
    NEXUS_Spi_SetSettings(spi, &spiSettings);
    memset(writeBuffer, 0, SPI_TEST_BUFFER_SIZE+SPI_TEST_BUFFER_OFFSET);
    writeBuffer[0] = 0x02;              /* page program code is 0x02 and address set to 0 */
    for(i=0; i<SPI_TEST_BUFFER_SIZE; i++)
    {
        writeBuffer[i+SPI_TEST_BUFFER_OFFSET] = (i+g_data_offset) & 0xFF;
        tempBuffer[i+SPI_TEST_BUFFER_OFFSET] = writeBuffer[i+SPI_TEST_BUFFER_OFFSET];
    }
    rc = NEXUS_Spi_Write(spi, writeBuffer, SPI_TEST_BUFFER_SIZE+SPI_TEST_BUFFER_OFFSET);
    if(rc != NEXUS_SUCCESS)
    {
        printf("Error!!!  NEXUS_Spi_Write() failed for writing. error code = %d\n", rc);
    }
    spiSettings.xferDMAMode = dmaMode;
    NEXUS_Spi_SetSettings(spi, &spiSettings);

    while(1)
    {
        spiSettings.xferDMAMode = false;
        NEXUS_Spi_SetSettings(spi, &spiSettings);
        bWEL = bWIP = false;
        /* check status */
        writeBuffer[0] = 0x05;             /* write enable code is 0x06 */
        NEXUS_Spi_Read(spi, writeBuffer, readBuffer, 2);
        spiSettings.xferDMAMode = dmaMode;
        NEXUS_Spi_SetSettings(spi, &spiSettings);
        spi_check_mx25_status(readBuffer[1], false, &bWEL, &bWIP);
        if(bWIP == false)   break;
        BKNI_Delay(100);
    }

    /* check status */
    spiSettings.xferDMAMode = false;
    NEXUS_Spi_SetSettings(spi, &spiSettings);
    writeBuffer[0] = 0x05;             /* write enable code is 0x06 */
    NEXUS_Spi_Read(spi, writeBuffer, readBuffer, 2);
    spi_check_mx25_status(readBuffer[1], true, &bWEL, &bWIP);
    spiSettings.xferDMAMode = dmaMode;
    NEXUS_Spi_SetSettings(spi, &spiSettings);


    printf("\n\nSPI write:\n");
    ptr = writeBuffer + SPI_TEST_BUFFER_OFFSET;
    for(i=0; i<SPI_TEST_BUFFER_SIZE; i++)
    {
        printf("0x%02X  ", *ptr++);
        if(i>0 && i%8==7)   printf("\n");
    }
    printf("\n\n\n");

    /*
     * Note: we are using onboard serial flash for SPI testing. Here are the steps:
     *   (1) read data (read instruction code 0x03 + address + dumy data)
     */
    printf("******************  read data **********************\n");
    spiSettings.xferDMAMode = dmaMode;
    NEXUS_Spi_SetSettings(spi, &spiSettings);
    if(spiSettings.xferDMAMode == true)
    {
        /* SPI read */
        memset(readBuffer, 0xAA, SPI_TEST_BUFFER_SIZE+SPI_TEST_BUFFER_OFFSET);
        memset(writeBuffer, 0, SPI_TEST_BUFFER_SIZE+SPI_TEST_BUFFER_OFFSET);
        writeBuffer[0] = 4;        /* read code is 0x03 and address set to 0 */
        writeBuffer[1] = 0x03;
        rc = NEXUS_Spi_Read(spi, writeBuffer, readBuffer+SPI_TEST_BUFFER_OFFSET, SPI_TEST_BUFFER_SIZE);
        if(rc != NEXUS_SUCCESS)
        {
            printf("Error!!!  NEXUS_Spi_Read() failed. error code = %d\n", rc);
        }
    }
    else
    {
        /* SPI read */
        memset(readBuffer, 0xAA, SPI_TEST_BUFFER_SIZE+SPI_TEST_BUFFER_OFFSET);
        memset(writeBuffer, 0, SPI_TEST_BUFFER_SIZE+SPI_TEST_BUFFER_OFFSET);
        writeBuffer[0] = 0x03;        /* read code is 0x03 and address set to 0 */
        rc = NEXUS_Spi_Read(spi, writeBuffer, readBuffer, SPI_TEST_BUFFER_SIZE+SPI_TEST_BUFFER_OFFSET);
        if(rc != NEXUS_SUCCESS)
        {
            printf("Error!!!  NEXUS_Spi_Read() failed. error code = %d\n", rc);
        }
    }
    spiSettings.xferDMAMode = dmaMode;
    NEXUS_Spi_SetSettings(spi, &spiSettings);


    printf("\n\nSPI read:\n");
    ptr = readBuffer + SPI_TEST_BUFFER_OFFSET;
    for(i=0; i<SPI_TEST_BUFFER_SIZE; i++)
    {
        printf("0x%02X  ", *ptr++);
        if(i>0 && i%8==7)   printf("\n");
    }
    printf("\n\n\n");

    /*
     * check contents in readBuffer and tempBuffer
     */
    printf("\n\n*****************************************\n");
    for(i=0; i<SPI_TEST_BUFFER_SIZE; i++)
    {
        if(*(readBuffer+i+SPI_TEST_BUFFER_OFFSET) != *(tempBuffer+i+SPI_TEST_BUFFER_OFFSET))   break;
    }
    if(i>=SPI_TEST_BUFFER_SIZE)
        printf("*      Buffer comparing OK.\n");
    else
        printf("*      Buffer comparing FAIL.\n");
    printf("*****************************************\n\n");

    NEXUS_Spi_Close(spi);
}
#else
void spi_test(NEXUS_PlatformConfiguration *pConfig, int index, uint32_t baud, bool clockActiveLow, bool txLeadingCapFalling, bool rxLeading, uint8_t bitsPerTransfer, uint8_t cs, bool dmaMode)
{
    unsigned char writeBuffer[1];
    unsigned char readBuffer[1];
    NEXUS_SpiHandle        spi;
    NEXUS_SpiSettings      spiSettings;
    NEXUS_Error rc;

    NEXUS_Spi_GetDefaultSettings(&spiSettings);

    spiSettings.baud = baud;                                   /* baud rate */
    spiSettings.clockActiveLow = clockActiveLow;               /* CPOL */
    spiSettings.txLeadingCapFalling = txLeadingCapFalling;     /* CPHA */
    spiSettings.bitsPerTransfer = bitsPerTransfer;             /* bits per transfer. max is 32 */
    spiSettings.interruptMode = false;
    spi = NEXUS_Spi_Open(index, &spiSettings);
    BDBG_ASSERT(spi);

    BSTD_UNUSED(pConfig);
    BSTD_UNUSED(txLeadingCapFalling);
    BSTD_UNUSED(rxLeading);
    BSTD_UNUSED(bitsPerTransfer);
    BSTD_UNUSED(cs);
    BSTD_UNUSED(dmaMode);

    rc = NEXUS_Spi_Write(spi, writeBuffer, 1);
    BDBG_ASSERT(!rc);

    rc = NEXUS_Spi_Read(spi, writeBuffer, readBuffer, 1);
    BDBG_ASSERT(!rc);

    NEXUS_Spi_Close(spi);
}
#endif


int main(void)
{
    NEXUS_Error                  rc;
    NEXUS_PlatformSettings       platformSettings;
    int                          index;
    uint32_t                     baud;
    bool                         clockActiveLow, txLeadingCapFalling, rxLeading, dmaMode;
    int                          bitsPerTransfer, cs;
    NEXUS_PlatformConfiguration  platformConfig;

    NEXUS_Platform_GetDefaultSettings(&platformSettings);
    platformSettings.openI2c = false;
    platformSettings.openFpga = false;
    platformSettings.openFrontend = false;
    platformSettings.openOutputs = false;
    rc = NEXUS_Platform_Init(&platformSettings);
    if(rc != NEXUS_SUCCESS)
    {
        printf("Error!!!  NEXUS_Platform_Init() failed. error code = %d\n", rc);
        return -1;
    }

    NEXUS_Platform_GetConfiguration(&platformConfig);



    while(1)
    {
#if 0
        printf("+++++++++   SPI Test +++++++++++++\n");
        index = spi_test_get_input("SPI channel index: ");
        if(index < 0)   break;

        baud = spi_test_get_input("SPI Baud rate: ");
        if((int)baud < 0)   break;

        clockActiveLow = spi_test_get_input("SPI clock active low : ");
        if((int)clockActiveLow < 0)   break;

        txLeadingCapFalling = spi_test_get_input("SPI tx leading cap falling : ");
        if(txLeadingCapFalling < 0)   break;

        bitsPerTransfer = spi_test_get_input("SPI bits per transfer: ");
        if((int)bitsPerTransfer < 0)   break;

        cs = spi_test_get_input("SPI chip select: ");
        if((int)cs < 0)   break;

        dmaMode = spi_test_get_input("SPI DMA mode <1/0>: ");
        if((int)dmaMode < 0)   break;
#else
        index = 0;
        baud = 13500000;
        clockActiveLow = 0;
        txLeadingCapFalling = 0;
        rxLeading = 0;
        bitsPerTransfer = 8;
        cs = 1;
        dmaMode = 0;

        dmaMode = spi_test_get_input("SPI DMA mode <1/0>: ");
        if((int)dmaMode < 0)   break;
#endif
        spi_test(&platformConfig, index, baud, clockActiveLow, txLeadingCapFalling, rxLeading, bitsPerTransfer, cs, dmaMode);

        g_data_offset++;
    }


    printf("+++++++ End of SPI Test +++++++++++++\n");

    NEXUS_Platform_Uninit();
    return 0;
}

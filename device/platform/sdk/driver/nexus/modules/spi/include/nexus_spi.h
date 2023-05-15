/***************************************************************************
*     (c)2004-2012 Broadcom Corporation
*
*  This program is the proprietary software of Broadcom Corporation and/or its licensors,
*  and may only be used, duplicated, modified or distributed pursuant to the terms and
*  conditions of a separate, written license agreement executed between you and Broadcom
*  (an "Authorized License").  Except as set forth in an Authorized License, Broadcom grants
*  no license (express or implied), right to use, or waiver of any kind with respect to the
*  Software, and Broadcom expressly reserves all rights in and to the Software and all
*  intellectual property rights therein.  IF YOU HAVE NO AUTHORIZED LICENSE, THEN YOU
*  HAVE NO RIGHT TO USE THIS SOFTWARE IN ANY WAY, AND SHOULD IMMEDIATELY
*  NOTIFY BROADCOM AND DISCONTINUE ALL USE OF THE SOFTWARE.
*
*  Except as expressly set forth in the Authorized License,
*
*  1.     This program, including its structure, sequence and organization, constitutes the valuable trade
*  secrets of Broadcom, and you shall use all reasonable efforts to protect the confidentiality thereof,
*  and to use this information only in connection with your use of Broadcom integrated circuit products.
*
*  2.     TO THE MAXIMUM EXTENT PERMITTED BY LAW, THE SOFTWARE IS PROVIDED "AS IS"
*  AND WITH ALL FAULTS AND BROADCOM MAKES NO PROMISES, REPRESENTATIONS OR
*  WARRANTIES, EITHER EXPRESS, IMPLIED, STATUTORY, OR OTHERWISE, WITH RESPECT TO
*  THE SOFTWARE.  BROADCOM SPECIFICALLY DISCLAIMS ANY AND ALL IMPLIED WARRANTIES
*  OF TITLE, MERCHANTABILITY, NONINFRINGEMENT, FITNESS FOR A PARTICULAR PURPOSE,
*  LACK OF VIRUSES, ACCURACY OR COMPLETENESS, QUIET ENJOYMENT, QUIET POSSESSION
*  OR CORRESPONDENCE TO DESCRIPTION. YOU ASSUME THE ENTIRE RISK ARISING OUT OF
*  USE OR PERFORMANCE OF THE SOFTWARE.
*
*  3.     TO THE MAXIMUM EXTENT PERMITTED BY LAW, IN NO EVENT SHALL BROADCOM OR ITS
*  LICENSORS BE LIABLE FOR (i) CONSEQUENTIAL, INCIDENTAL, SPECIAL, INDIRECT, OR
*  EXEMPLARY DAMAGES WHATSOEVER ARISING OUT OF OR IN ANY WAY RELATING TO YOUR
*  USE OF OR INABILITY TO USE THE SOFTWARE EVEN IF BROADCOM HAS BEEN ADVISED OF
*  THE POSSIBILITY OF SUCH DAMAGES; OR (ii) ANY AMOUNT IN EXCESS OF THE AMOUNT
*  ACTUALLY PAID FOR THE SOFTWARE ITSELF OR U.S. $1, WHICHEVER IS GREATER. THESE
*  LIMITATIONS SHALL APPLY NOTWITHSTANDING ANY FAILURE OF ESSENTIAL PURPOSE OF
*  ANY LIMITED REMEDY.
*
* $brcm_Workfile: nexus_spi.h $
* $brcm_Revision: 18 $
* $brcm_Date: 8/24/12 4:13p $
*
* API Description:
*   API name: Spi
*    Specific APIs related to SPI Control.
*
* Revision History:
*
* $brcm_Log: /nexus/modules/spi/include/nexus_spi.h $
* 
* 18   8/24/12 4:13p erickson
* SW7358-345: add NEXUS_SpiCore and NEXUS_SPI_INDEX() macro. add
*  NEXUS_Spi_GetSettings.ebiChipSelect.
* 
* SW7358-345/1   8/20/12 5:48p vishk
* SW7358-345: Add support for HIF SPI
* 
* 17   8/3/12 3:45p erickson
* SW7425-1747: remove old code
* 
* 16   6/1/11 9:00a erickson
* SWDEPRECATED-2425: clarify comments
* 
* 15   12/7/10 5:44p erickson
* SW35230-2310: add comment
* 
* 14   11/19/10 4:42p jhaberf
* SW35230-2310: removed BCHP==35230 compile time checks
* 
* 13   11/3/10 1:54p jhaberf
* SW35125-1: Added support for 35125 DTV chip
* 
* 12   7/14/10 8:02p jhaberf
* SW35230-639: Added nexus_memory.h to included files in order to resolve
*  a build issue
* 
* 11   6/9/10 2:48p jhaberf
* SW35230-67: integrated latest nexus SPI support for the 35230
* 
* 10   5/19/10 5:20p jhaberf
* SW35230-67: Added bHeap and bufferSize structure members for 35230
* 
* 9   5/13/10 4:32p jhaberf
* SW35230-67: Added nexus spi changes for 35230 DTV chip
* 
* 8   1/6/10 11:33a mphillip
* SW3548-2660: Merge interrupt vs. polling mode changes to main
* 
* SW3548-2660/2   1/6/10 11:29a mphillip
* SW3548-2660: Rename variable exposing interrupt vs. polling mode for
*  transfers
* 
* SW3548-2660/1   1/5/10 6:06p mphillip
* SW3548-2660: Add a user setting to enable polling for transfers
* 
* 7   11/11/09 11:53a erickson
* SW7405-3362: add dtl and rdsclk fields to NEXUS_SpiSettings
*
* 6   2/4/08 4:24p vsilyaev
* PR 38682: Added more tags for the linux kernel/user proxy mode
*
* 5   2/4/08 2:47p vsilyaev
* PR 38682: Added markup for variable size pointers
*
* 4   1/25/08 2:29p erickson
* PR39016: doc update
*
* 3   1/23/08 8:36p vobadm
* PR35457: update docs
*
* 2   1/23/08 5:16p erickson
* PR35457: update docs
*
* 1   1/18/08 2:21p jgarrett
* PR 38808: Merging to main branch
*
* Nexus_Devel/1   11/20/07 1:28p erickson
* PR37423: added uart, gpio, spi modules
*
* Nexus_Devel/1   9/13/07 6:04p jgarrett
* PR 34702: Adding SPI
*
***************************************************************************/
#ifndef NEXUS_SPI_H__
#define NEXUS_SPI_H__

#include "nexus_memory.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct NEXUS_Spi *NEXUS_SpiHandle;

/***************************************************************************
Summary:
Settings structure for SPI.
****************************************************************************/
typedef struct NEXUS_SpiSettings
{
	/* the following can only be set at Open time */
    uint32_t      baud;                       /* SPI baud rate */
    uint8_t       bitsPerTransfer;            /* number of bits per transfer */
    bool          lastByteContinueEnable;     /* Last Byte Continue Enable Flag */
    bool          useUserDtlAndDsclk;         /* Use User specified DTL and DSCLK */
    bool          interruptMode;              /* If true (default), transfers will wait for interrupt, if false transfers will poll for completion */
    unsigned      ebiChipSelect;              /* EBI chip select line to use as slave select */ 

	/* the following can be set at Open or SetSettings */
    bool          clockActiveLow;             /* If true, the SCK will be active low. */
    bool          txLeadingCapFalling;        /* If true, data will be transmitted on the leading edge and captured on the falling edge */
    uint8_t       dtl;                        /* DTL field - length of delay after transfer. See RDB documentation for usage. */
    uint8_t       rdsclk;                     /* RDSCLK field - the length of delay from PCS valid to SCK transition. See RDB documentation for usage. */
} NEXUS_SpiSettings;

/***************************************************************************
Summary:
Get default settings for a SPI channel.
****************************************************************************/
void NEXUS_Spi_GetDefaultSettings(
    NEXUS_SpiSettings *pSettings    /* [out] Default Settings */
    );

/**
Summary:
SPI core selection used in NEXUS_SPI_INDEX()

Description:
To open SPI channels on different SPI cores, use the NEXUS_SPI_INDEX() macro.

Example Code:
   spi = NEXUS_Spi_Open(NEXUS_SPI_INDEX(Upg,1), &settings);
   spi = NEXUS_Spi_Open(NEXUS_SPI_INDEX(Hif,0), &settings);
**/
typedef enum NEXUS_SpiCore
{
    NEXUS_SpiCore_eUpg = 0,                  /* UPG SPI core */
    NEXUS_SpiCore_eHif,                      /* HIF SPI core */
    NEXUS_SpiCore_eMax 
} NEXUS_SpiCore;
#define NEXUS_SPI_INDEX(TYPE,INDEX) (NEXUS_SpiCore_e##TYPE<<16|(INDEX))

/***************************************************************************
Summary:
Open a SPI channel
****************************************************************************/
NEXUS_SpiHandle NEXUS_Spi_Open(  /* attr{destructor=NEXUS_Spi_Close} */
    unsigned index, /* SPI channel index. Use NEXUS_SPI_INDEX() to select different core types. */
    const NEXUS_SpiSettings *pSettings
    );

/***************************************************************************
Summary:
Close a SPI channel
****************************************************************************/
void NEXUS_Spi_Close(
    NEXUS_SpiHandle handle
    );

/***************************************************************************
Summary:
Set settings for a SPI channel.
****************************************************************************/
NEXUS_Error NEXUS_Spi_SetSettings(
    NEXUS_SpiHandle handle,
    const NEXUS_SpiSettings *pSettings
    );

/***************************************************************************
Summary:
Get settings for a SPI channel.
****************************************************************************/
void NEXUS_Spi_GetSettings(
    NEXUS_SpiHandle handle,
    NEXUS_SpiSettings *pSettings    /* [out] */
    );

/***************************************************************************
Summary:
Write out data over SPI.
****************************************************************************/
NEXUS_Error NEXUS_Spi_Write(
    NEXUS_SpiHandle handle,
    const uint8_t *pWriteData,      /* attr{nelem=length;reserved=8} pointer to write memory location */
    size_t length                   /* size of data in pWriteData[] in bytes */
    );

/***************************************************************************
Summary:
Read in data from SPI.

Description:
The SPI protocol always writes data out as it is reading data in.
The incoming data is stored in pReadData while the data
going out is sourced from pWriteData.
****************************************************************************/
NEXUS_Error NEXUS_Spi_Read(
    NEXUS_SpiHandle handle,
    const uint8_t *pWriteData,      /* attr{nelem=length;reserved=8} pointer to memory location where data is to sent  */
    uint8_t *pReadData,             /* [out] attr{nelem=length;reserved=8} pointer to memory location to store read data  */
    size_t length                   /* length is the size of pWriteData[] in bytes and is also the size of pReadData[] in bytes. They have the same size. */
    );

#if defined(HUMAX_PLATFORM_BASE)
/***************************************************************************
Summary:
Write out data over SPI.
****************************************************************************/
NEXUS_Error NEXUS_Spi_WritebyMode(
    NEXUS_SpiHandle handle,
	unsigned char mode,
    const uint8_t *pWriteData,      /* attr{nelem=length;reserved=8} pointer to write memory location */
    size_t length                   /* size of data in pWriteData[] in bytes */
    );

/***************************************************************************
Summary:
Read in data from SPI.

Description:
The SPI protocol always writes data out as it is reading data in.
The incoming data is stored in pReadData while the data
going out is sourced from pWriteData.
****************************************************************************/
NEXUS_Error NEXUS_Spi_ReadbyMode(
    NEXUS_SpiHandle handle,
	unsigned char mode,
    const uint8_t *pWriteData,      /* attr{nelem=length;reserved=8} pointer to memory location where data is to sent  */
    uint8_t *pReadData,             /* [out] attr{nelem=length;reserved=8} pointer to memory location to store read data  */
    size_t length                   /* length is the size of pWriteData[] in bytes and is also the size of pReadData[] in bytes. They have the same size. */
    );
#endif

#ifdef __cplusplus
}
#endif

#endif /* #ifndef NEXUS_SPI_H__ */


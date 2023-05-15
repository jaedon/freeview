/******************************************************************************
 *    (c)2011-2012 Broadcom Corporation
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
 * $brcm_Workfile: NetAppTester.c $
 * $brcm_Revision: 3 $
 * $brcm_Date: 2/3/12 4:18p $
 *
 * Module Description:
 *
 * Revision History:
 *
 * $brcm_Log: /NetApp/examples/NetAppTester.c $
 *
 * 3   2/3/12 4:18p steven
 * SW7231-638: NetApp Version 6.2
 *
 * 3   11/24/11 5:13p steven
 * SW7425-1404: Bluetooh, WLAN Driver, and Multi-Wired Interface Support
 *
 * SW7425-1404/4   11/24/11 4:02p steven
 * SW7425-1404: Bluetooth, WLAN driver build, and multi-interface support
 *
 * SW7425-1404/2   11/4/11 10:14a steven
 * SW7425-1404: Backup of BT work
 *
 * SW7425-1404/1   10/7/11 5:33p steven
 * SW7425-1404: initial pass fo BT support
 *
 * 2   6/15/11 5:27p steven
 * SW7425-714: Adding NetApp support to AppLibs
 *
 * 1   6/14/11 10:41a steven
 * SW7425-714: Moved NetApp to AppLibs
 *
 *****************************************************************************/
/* NetApp Test Application */
#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>
#include <stdint.h>
#include <unistd.h>
#include <string.h>
#include <pthread.h>
#include <errno.h>
#include <signal.h>
#include <fcntl.h> //O_RDONLY
#include <sys/stat.h>  //stat

#include "netapp.h"

#ifdef HAS_NEXUS
#include "nexus_types.h"
#include "nexus_platform.h"
#include "nexus_gpio.h"
#include "bstd.h"
#include "bkni.h"
#endif

/* Yes I know a lot of stack stuff but this is a test app not the impl. :-) */
static NETAPP_HANDLE        hNetApp = NULL;
static NETAPP_HANDLE        hNetApp2 = NULL;
static NETAPP_HANDLE        hNetApp3 = NULL;

static bool                 scanning = false;
static NETAPP_BT_DEV_INFO   *pBtDevInfoList = NULL;
static uint32_t             ulBtDevCount = 0;
static bool                 connectOnlyMode = false;
static bool                 bNoIP = false;

static NETAPP_BT_AUDIO_FORMAT tBtAudioFormat;
pthread_t                     app_uipc_tx_thread_struct;
static bool                   bIsConnected = false;
static bool                   bPlayStarted = false;
static bool                   bIsbe = false;

typedef void (APP_THREAD_ENTRY)(void);
typedef void *(*PTHREAD_ENTRY)(void*);

#define TEST_WAV_FILE             "Wherever1.wav"
#define APP_AV_MAX_AUDIO_BUF       240
#define APP_AV_MAX_AUDIO_BUF_MAX   10000

#define MIRACAST_BIN                "wfd_player"
#define PROC_MODULES                "/proc/modules"
struct
{
    int      lAv_fd;
    short    sAudioBuf[APP_AV_MAX_AUDIO_BUF_MAX];
} app_av_cb;

#define CHECK(api)                          \
{                                           \
    if (api != NETAPP_SUCCESS)              \
    {                                       \
        printf("%s() line %d failed\n", __FUNCTION__, __LINE__);       \
        goto err_out;                       \
    }                                       \
}

static void signal_handler (int signal)
{
    printf("NetAppTester: Received signal %s\n", strsignal(signal));
    if (hNetApp != NULL)
    {
        NetAppClose(hNetApp);
        exit(-1);
    }
}


int app_wav_read_format(const char *p_fname, NETAPP_BT_AUDIO_FORMAT *p_format)
{
    int         status = 0;
    int         fd;
    ssize_t     headersize;
    ssize_t     size;
    uint8_t     riff[12];
    uint8_t     fmt[8];
    uint8_t     fmt_params[40];
    uint8_t     data[8];
    off_t       file_size;
    struct stat stat_info;
    uint32_t    tmpU32;
    uint16_t    tmpU16;

    /* Check dynamically if computer is little or big endian */
    tmpU32 = 1;
    if (*((char *)(&tmpU32)) == 0) bIsbe = true;

    if (p_format == NULL)
    {
        printf("format is NULL\n");
        return -1;
    }
    if (p_fname == NULL)
    {
        printf("no filename\n");
        return -1;
    }
    if ((fd = open(p_fname, O_RDONLY)) < 0)
    {
        printf("open(%s) failed: %d", p_fname, errno);
        return -1;
    }
    app_av_cb.lAv_fd = fd;
    if (fstat(fd, &stat_info) < 0)
    {
        printf("stat(%s) failed: %d", p_fname, errno);
        status = -1;
        goto app_wav_get_format_exit;
    }
    file_size = stat_info.st_size;


    /* Read the RIFF header */
    size = read(fd, riff, sizeof(riff));
    if (size < 0)
    {
        printf("read(%s) failed: %d", p_fname, errno);
        status = -1;
        goto app_wav_get_format_exit;
    }

    if (size != sizeof(riff))
    {
        printf("Length read does not match RIFF header (%d != %d)", (int)size, sizeof(riff));
        status = -1;
        goto app_wav_get_format_exit;
    }
    /* Check ChunkId field */
    if (memcmp(&riff[0], "RIFF", 4) != 0)
    {
        printf("RIFF not found\n");
        status = -1;
        goto app_wav_get_format_exit;
    }

    /* Read ChunkSize field */
    tmpU32 = riff[4];
    tmpU32 |= (riff[5] << 8);
    tmpU32 |= (riff[6] << 16);
    tmpU32 |= (unsigned long) (riff[7] << 24);

    /* Check ChunkSize field */
    if ((tmpU32 + 8) != file_size)
    {
        printf("WARNING: RIFF chunk size (%d + 8) does not match file size(%d)\n", tmpU32, (uint32_t)file_size);
    }

    /* Check WAVE ID field */
    if (memcmp(&riff[8], "WAVE", 4) != 0)
    {
        printf("WAVE not found\n");
        status = -1;
        goto app_wav_get_format_exit;
    }

    /* Read the FMT header */
    size = read(fd, fmt, sizeof(fmt));
    if (size < 0)
    {
        printf("read(%s) failed: %d", p_fname, errno);
        status = -1;
        goto app_wav_get_format_exit;
    }
    if (size != sizeof(fmt))
    {
        printf("Length read does not match FMT header (%d != %d)", (int)size, sizeof(fmt));
        status = -1;
        goto app_wav_get_format_exit;
    }

    /* Check ChunkId field */
    if (memcmp(&fmt[0], "fmt ", 4) != 0)
    {
        printf("fmt not found\n");
        status = -1;
        goto app_wav_get_format_exit;
    }

    /* Check ChunkSize field */
    tmpU32 = fmt[4];
    tmpU32 |= (fmt[5] << 8);
    tmpU32 |= (fmt[6] << 16);
    tmpU32 |= (unsigned long) (fmt[7] << 24);
    if ((tmpU32 != 16) && (tmpU32 != 18) && (tmpU32 != 40))
    {
        printf("WARNING: format chunk size is not supported (%d != (16, 18, 40))\n", tmpU32);
        status = -1;
        goto app_wav_get_format_exit;
    }

    /* At this point, the headersize is:
     * RIFF + WAVE + FMT + size of FMT header + DATA
     */
    headersize = 28 + tmpU32;

    /* Read the FMT params */
    size = read(fd, fmt_params, tmpU32);
    if (size < 0)
    {
        printf("read(%s) failed: %d", p_fname, errno);
        status = -1;
        goto app_wav_get_format_exit;
    }
    if ((unsigned)size != tmpU32)
    {
        printf("Length read does not match FMT params (%d != %d)", (int)size, (int)tmpU32);
        status = -1;
        goto app_wav_get_format_exit;
    }

   /* Read AudioFormat field */
    tmpU16 = fmt_params[0];
    tmpU16 |= (fmt_params[1] << 8);

    /* Read NumChannels field */
    tmpU16 = fmt_params[2];
    tmpU16 |= (fmt_params[3] << 8);

    /* Check NumChannels field => 1:Mono 2:Stereo */
    p_format->tMode = (tmpU16 == 1) ? NETAPP_BT_AV_MODE_MONO : NETAPP_BT_AV_MODE_STEREO;

    /* Read SampleRate field */
    tmpU32 = fmt_params[4];
    tmpU32 |= (fmt_params[5] << 8);
    tmpU32 |= (fmt_params[6] << 16);
    tmpU32 |= (unsigned long) (fmt_params[7] << 24);
    p_format->ulSampleRate = tmpU32;

    /* Read BitsPerSample field (read before ByteRate and BlockAlign) */
    tmpU16 = fmt_params[14];
    tmpU16 |= (fmt_params[15] << 8);
    p_format->ucBitsPerSample = tmpU16;

    /* Read ByteRate field */
    tmpU32 = fmt_params[8];
    tmpU32 |= (fmt_params[9] << 8);
    tmpU32 |= (fmt_params[10] << 16);
    tmpU32 |= (unsigned long)(fmt_params[11] << 24);
    /* Check ByteRate field: should be = SampleRate * NumChannels * BitsPerSample/8 */
    if (tmpU32 != (p_format->ulSampleRate * (uint8_t)p_format->tMode * p_format->ucBitsPerSample / 8))
    {
        printf("WARNING: byte rate does not match PCM rate calculation (%d != (%d * %u * %u / 8))\n",
                tmpU32, p_format->ulSampleRate, (uint8_t)p_format->tMode, p_format->ucBitsPerSample);
    }

    /* Read BlockAlign field */
    tmpU16 = fmt_params[12];
    tmpU16 |= (fmt_params[13] << 8);
    /* Check BlockAlign field: should be NumChannels * BitsPerSample/8 */
    if (tmpU16 != (p_format->tMode * p_format->ucBitsPerSample / 8))
    {
        printf("Block alignment does not match calculation (%u != (%u * %u / 8))",
                tmpU16, p_format->tMode, p_format->ucBitsPerSample);
        status = -1;
        goto app_wav_get_format_exit;
    }

    /* Read the FACT or DATA header */
    size = read(fd, data, sizeof(data));
    if (size < 0)
    {
        printf("read(%s) failed: %d", p_fname, errno);
        status = -1;
        goto app_wav_get_format_exit;
    }
    if (size != sizeof(data))
    {
        printf("Length read does not match header (%d != %d)", (int)size, sizeof(data));
        status = -1;
        goto app_wav_get_format_exit;
    }

    /* Check ChunkId field */
    if (!memcmp(&data[0], "fact", 4))
    {
        /* FACT: bypass it and read again */
        tmpU32 = data[4];
        tmpU32 |= (data[5] << 8);
        tmpU32 |= (data[6] << 16);
        tmpU32 |= (unsigned long) (data[7] << 24);
        if (tmpU32 > 8)
        {
            printf("FACT chunk size is not supported (%d)", (int)tmpU32);
            status = -1;
            goto app_wav_get_format_exit;
        }

       /* Add the fact header to the header size */
        headersize += 8 + tmpU32;

        size = read(fd, data, tmpU32);
        if (size < 0)
        {
            printf("read(%s) failed: %d", p_fname, errno);
            status = -1;
            goto app_wav_get_format_exit;
        }
        if (size != sizeof(data))
        {
            printf("Length read does not match FACT chunk (%d != %d)", (int)size, sizeof(data));
            status = -1;
            goto app_wav_get_format_exit;
        }

        size = read(fd, data, sizeof(data));
        if (size < 0)
        {
            printf("read(%s) failed: %d", p_fname, errno);
            status = -1;
            goto app_wav_get_format_exit;
        }
        if (size != sizeof(data))
        {
            printf("Length read does not match header (%d != %d)", (int)size, sizeof(data));
            status = -1;
            goto app_wav_get_format_exit;
        }
    }

    /* Check ChunkId field */
    if (memcmp(&data[0], "data", 4) != 0)
    {
        printf("data not found\n");
        status = -1;
        goto app_wav_get_format_exit;
    }

    /* Read data size field */
    tmpU32 = data[4];
    tmpU32 |= (data[5] << 8);
    tmpU32 |= (data[6] << 16);
    tmpU32 |= (unsigned long) (data[7] << 24);

    /* Check size matches file size (1 pad byte if chunk size is odd) */
    if (((tmpU32 + 1) & ~1) != (file_size - headersize))
    {
        printf("WARNING: data chunk size does not match calculation (%d != (%d - %d))\n",
                (tmpU32 + 1) & ~1, (uint32_t)file_size, headersize);
    }

    app_wav_get_format_exit:
    if(fd > 0)
        close(fd);

    return status;
}

int app_wav_read_data(int fd, const NETAPP_BT_AUDIO_FORMAT *p_format, char *p_data, int len)
{
    int nbytes;
    int index;
    int bytes_per_sample = (p_format->ucBitsPerSample + 7)/8;
    unsigned short tmp16;
    unsigned short *tmp16_ptr;
    unsigned long tmp32;
    unsigned long *tmp32_ptr;

    nbytes = read(fd, p_data, len);

    /* Check that the buffer is aligned */
    if (((bytes_per_sample == 2) && ((size_t)p_data & 1)) ||
        ((bytes_per_sample == 4) && ((size_t)p_data & 3)))
    {
        printf("Audio buffer start is not aligned on PCM sample (%d bytes) word boundary, this could degrade system perf (%p)\n",
                bytes_per_sample, p_data);
    }

    if (nbytes > 0)
    {
        switch (bytes_per_sample)
        {
        case 1:
            break;
        case 2:
            if (nbytes & 1)
            {
                printf("Number of PCM samples read not multiple of sample size(%d)\n", bytes_per_sample);
            }
            if (bIsbe)
            {
                tmp16_ptr = (unsigned short *)p_data;
                for (index = 0; index < nbytes; index += 2)
                {
                    tmp16 = *tmp16_ptr;
                    *tmp16_ptr = (tmp16 << 8 | tmp16 >> 8);
                    tmp16_ptr++;
                }
            }
            break;
       case 4:
            if (nbytes & 3)
            {
                printf("Number of PCM samples read not multiple of sample size(%d)\n", bytes_per_sample);
            }
            if (bIsbe)
            {
                tmp32_ptr = (unsigned long *)p_data;
                for (index = 0; index < nbytes; index += 4)
                {
                    tmp32 = *tmp32_ptr;
                    *tmp32_ptr = (tmp32 >> 24) |
                        ((tmp32 >> 8) & 0xff00) |
                        ((tmp32 << 8) & 0xff0000) |
                        ((tmp32 << 24) & 0xff000000);
                    tmp32_ptr++;
                }
            }
            break;
        default:
            printf("Sample size is not supported (%d)\n", bytes_per_sample);
            break;
        }
    }
    return nbytes;
}


int app_create_thread(APP_THREAD_ENTRY task_entry, short *stack, short stacksize, pthread_t *p_thread)
{
    pthread_attr_t thread_attr;
    int status = 0;
    (void)stack;
    (void)stacksize;
    pthread_attr_init(&thread_attr);

    pthread_attr_setdetachstate(&thread_attr, PTHREAD_CREATE_DETACHED);

    if (pthread_create(p_thread, &thread_attr, (PTHREAD_ENTRY)task_entry, NULL) < 0 )
    {
        printf("pthread_create failed:%d", errno);
        status = -1;
    }
    return 0;
}

static void app_uipc_tx_thread(void)
{
    int nb_bytes = 0;
    int index = 0;
    int file_desc;

    printf("Starting UIPC Tx thread\n");
    while(bPlayStarted)
    {
        file_desc = open(TEST_WAV_FILE, O_RDONLY);
        if (file_desc < 0)
        {
            printf("open(%s) failed(%d) -> stop playing\n", TEST_WAV_FILE, errno);
            /* Wait for the stop event */
            printf("Waiting for AV to stop\n");

            /* Restart the loop */
            continue;
        }

        /* Check that the file format matches the feeding */
        /* Get past the WAV header */
        lseek(file_desc, 44, SEEK_SET);

        printf("starting to play file:%s\n", TEST_WAV_FILE);

        index = 0;

        do
        {
            nb_bytes = app_wav_read_data(file_desc, &tBtAudioFormat, (char *)app_av_cb.sAudioBuf, 512);

            if (NetAppBluetoothSendAudioBuffer(hNetApp, (void*) app_av_cb.sAudioBuf, nb_bytes) != NETAPP_SUCCESS)
            {
                    printf("UIPC send could not send data\n");
                    nb_bytes = 0;
            }
            index++;

            /* print once per sec */
            if ((index % ((44100 * 2 * sizeof(short)) / APP_AV_MAX_AUDIO_BUF)) == 0)
                printf("samples %d\n", index++ * APP_AV_MAX_AUDIO_BUF / 4);

        } while((nb_bytes != 0) && (bPlayStarted == true));

        printf("stop playing\n");

        if (file_desc > 0)
            file_desc = close(file_desc);

    }
    printf("Exit TX THREAD\n");
    pthread_exit(NULL);
}


void print_bt_dev_list()
{
    unsigned int i;
    if ( (ulBtDevCount == 0) || (pBtDevInfoList == NULL) )
    {
        printf("Discovered list is empty\n");
        printf("Run a discovery first \n");
        return;
    }
    for (i = 0; i < ulBtDevCount; i++)
    {
        printf("\n%d:**************************\n", i);
        printf("Name:       %s\n", pBtDevInfoList[i].cName);
        printf("BD Addr:    %s\n", pBtDevInfoList[i].cAddr);
        printf("RSSI:       %d dB\n", pBtDevInfoList[i].lRssi);
        printf("COD Major:  0x%02x\n", pBtDevInfoList[i].ucMajorClassDev);
        printf("COD Minor:  0x%02x\n", pBtDevInfoList[i].ucMinorClassDev);
        printf("COD Service:0x%04x\n", pBtDevInfoList[i].usServiceClassDev);
        printf("%d:**************************\n", i);
    }
}


void print_ip_settings(NETAPP_IFACE iface)
{
    char                *pIfaceName = NULL;
    NETAPP_IP_SETTINGS  tSettings;

    memset(&tSettings, 0, sizeof(tSettings));

    NetAppGetNetworkSettings(hNetApp, iface, &tSettings);
    NetAppGetIfaceName(hNetApp, iface, &pIfaceName);

    printf("**************************\n");
    printf("IP Settings:   %s\n", pIfaceName);
    printf("MAC Address:   %s\n", tSettings.cMacAddress);
    printf("IP Address:    %s\n", NetAppNtoA(tSettings.tIpAddress));
    printf("Subnet Mask:   %s\n", NetAppNtoA(tSettings.tSubnetMask));
    printf("Gateway:       %s\n", NetAppNtoA(tSettings.tGateway));
    printf("Primary DNS:   %s\n", NetAppNtoA(tSettings.tPrimaryDNS));
    printf("Secondary DNS: %s\n", NetAppNtoA(tSettings.tSecondaryDNS));
    printf("**************************\n");
    if (pIfaceName != NULL)
    {
        free(pIfaceName);
    }
}


static struct
{
    NETAPP_WIFI_RSSI    tRSSI;
    const char          *pName;
} RSSIPrintTable [] =
{
   {  NETAPP_WIFI_RSSI_NONE ,           "No signal (0 bar)"     },
   {  NETAPP_WIFI_RSSI_POOR,            "Poor (1 bar)"          },
   {  NETAPP_WIFI_RSSI_FAIR,            "Fair (2 bars)"         },
   {  NETAPP_WIFI_RSSI_GOOD,            "Good (3 bars)"         },
   {  NETAPP_WIFI_RSSI_EXCELLENT,       "Excellent (4 bars)"    },
};

static struct
{
    NETAPP_WIFI_SECURITY tSecurity;
    const char           *pName;
} SecurityPrintTable[] =
{
    { NETAPP_WIFI_SECURITY_NONE,          "No Security"                   },
    { NETAPP_WIFI_SECURITY_WEP,           "WEP (Open & Shared)"           },
    { NETAPP_WIFI_SECURITY_WPA_PSK_AES,   "WPA-Personal, AES encryp."     },
    { NETAPP_WIFI_SECURITY_WPA_PSK_TKIP,  "WPA-Personal, TKIP encryp."    },
    { NETAPP_WIFI_SECURITY_WPA2_PSK_AES,  "WPA2-Personal, AES encryp."    },
    { NETAPP_WIFI_SECURITY_WPA2_PSK_TKIP, "WPA2-Personal, TKIP encryp."   },
    { NETAPP_WIFI_SECURITY_NOT_SUPPORTED, "Security format not supported" },
};


void print_connected_info()
{
    unsigned int i;
    NETAPP_WIFI_AP_INFO tApInfo;

    memset(&tApInfo, 0, sizeof(tApInfo));
    if (NetAppWiFiGetConnectedApInfo(hNetApp, &tApInfo) == NETAPP_SUCCESS)
    {
        printf("**************************\n");
        printf("SSID:     %s\n", tApInfo.cSSID);
        printf("BSSID:    %s\n", tApInfo.cBSSID);
        printf("Password: %s\n", tApInfo.cPassword);
        printf("ulChan:   %d\n", tApInfo.ulChannel);
        printf("bAdHoc:   %s\n", (tApInfo.bAdHoc) ? "YES" : "NO");
        printf("bWPS:     %s\n", (tApInfo.bWPS)   ? "YES" : "NO");

        for (i = 0; i < sizeof(RSSIPrintTable)/sizeof(RSSIPrintTable[0]); i++)
        {
            if (RSSIPrintTable[i].tRSSI == tApInfo.tRSSI)
            {
                printf("tRSSI:    %s\n", RSSIPrintTable[i].pName);
                break;
            }
        }

        for (i = 0; i < sizeof(SecurityPrintTable)/sizeof(SecurityPrintTable[0]); i++)
        {
            if (SecurityPrintTable[i].tSecurity == tApInfo.tSecurity)
            {
                printf("tSecurity:%s\n", SecurityPrintTable[i].pName);
                break;
            }
        }

        printf("802.11:   %s%s%s%s\n",
            (tApInfo.tMode & NETAPP_WIFI_802_11_MODE_A) ? "a " : "",
            (tApInfo.tMode & NETAPP_WIFI_802_11_MODE_B) ? "b " : "",
            (tApInfo.tMode & NETAPP_WIFI_802_11_MODE_G) ? "g " : "",
            (tApInfo.tMode & NETAPP_WIFI_802_11_MODE_N) ? "n" : "");

        printf("Rate:     %d%s Mbps\n", tApInfo.lRate/2, (tApInfo.lRate & 1) ? ".5" : "");
        printf("Noise:    %d dBm\n", tApInfo.lPhyNoise);


        printf("**************************\n");
    }
    else
    {
        printf ("Not Associated to an AP\n");
    }
}


void callback(
    void                *pParam,        //!< The pParam from NetAppOpen
    NETAPP_CB_TYPE      tCbType,        //!< The Callback type
    const void          *pvBuffer,      //!< Pointer to data specific to the callback
    uint32_t            ulData0,        //!< Callback specific data 0
    NETAPP_RETCODE      tResult,        //!< Callback results from the
    NETAPP_IFACE        tIFace)         //!< Callback Info structure
{
    (void)pParam;
    printf("%s() ENTERED, CB=%d\n", __FUNCTION__, tCbType);
    switch(tCbType)
    {
    case NETAPP_CB_LINK:
        if (ulData0 == NETAPP_LINK_UP)
        {
            if (connectOnlyMode)
            {
                connectOnlyMode = false;
            }
            print_ip_settings(tIFace);
        }
        else if ( (ulData0 == NETAPP_LINK_DOWN) && (tIFace == NETAPP_IFACE_P2P) )
        {
            struct stat stat_info;
            if (stat(MIRACAST_BIN, &stat_info) == 0)
            {
                char    cTmp[256];
                snprintf(cTmp, sizeof(cTmp), "killall %s &", MIRACAST_BIN);
                system(cTmp);
            }
        }
        break;


    case NETAPP_CB_INVITE:
        printf("%s() Received NETAPP_CB_INVITE, result=%d\n", __FUNCTION__, tResult);
        break;


    case NETAPP_CB_SCAN_DONE:
    {
        NETAPP_WIFI_AP_INFO *pApInfoList = NULL;
        uint32_t            ulCount =0;
        printf("%s() Received NETAPP_CB_SCAN_RESULTS, result=%d\n", __FUNCTION__, tResult);

        if ( (tResult == NETAPP_SUCCESS) &&
             (NetAppWiFiGetScanResults(hNetApp, &pApInfoList, &ulCount) == NETAPP_SUCCESS) )
        {
            unsigned int i;
            for (i = 0; i < ulCount; i++)
            {
                printf("\n**************************\n");
                printf("SSID:      %s\n", pApInfoList[i].cSSID);
                printf("BSSID:     %s\n", pApInfoList[i].cBSSID);
                switch (pApInfoList[i].tRSSI)
                {
                case NETAPP_WIFI_RSSI_POOR:
                    printf("RSSI:      I    (%d dB)\n", pApInfoList[i].lRSSI);
                    break;
                case NETAPP_WIFI_RSSI_FAIR:
                    printf("RSSI:      II   (%d dB)\n", pApInfoList[i].lRSSI);
                    break;
                case NETAPP_WIFI_RSSI_GOOD:
                    printf("RSSI:      III  (%d dB)\n", pApInfoList[i].lRSSI);
                    break;
                case NETAPP_WIFI_RSSI_EXCELLENT:
                    printf("RSSI:      IIII (%d dB)\n", pApInfoList[i].lRSSI);
                    break;
                default:
                case NETAPP_WIFI_RSSI_NONE:
                    printf("RSSI:      NONE\n");
                    break;
                }

                printf("802.11:    ");
                if (pApInfoList[i].tMode & NETAPP_WIFI_802_11_MODE_A)
                {
                    printf("a ");
                }
                if (pApInfoList[i].tMode & NETAPP_WIFI_802_11_MODE_B)
                {
                    printf("b ");
                }
                if (pApInfoList[i].tMode & NETAPP_WIFI_802_11_MODE_G)
                {
                    printf("g ");
                }
                if (pApInfoList[i].tMode & NETAPP_WIFI_802_11_MODE_N)
                {
                    printf("n ");
                }
                printf("\n");

                printf("Security:  ");
                switch (pApInfoList[i].tSecurity)
                {
                case NETAPP_WIFI_SECURITY_NONE:
                    printf("No Security\n");
                    break;
                case NETAPP_WIFI_SECURITY_WEP:
                    printf("Shared or Open, WEP\n");
                    break;
                case NETAPP_WIFI_SECURITY_WPA_PSK_AES:
                    printf("WPA-Personal, AES encryp\n");
                    break;
                case NETAPP_WIFI_SECURITY_WPA_PSK_TKIP:
                    printf("WPA-Personal, TKIP encryp.\n");
                    break;
                case NETAPP_WIFI_SECURITY_WPA2_PSK_AES:
                    printf("WPA2-Personal, AES encryp.\n");
                    break;
                case NETAPP_WIFI_SECURITY_WPA2_PSK_TKIP:
                    printf("WPA-Personal, TKIP encryp.\n");
                    break;
                case NETAPP_WIFI_SECURITY_NOT_SUPPORTED:
                    printf("Security format not supported\n");
                    break;
                default:
                    printf("\n");
                    break;
                }

                printf("adHoc:     %s\n", pApInfoList[i].bAdHoc ? "YES" : "NO");
                printf("WPS:       %s\n", pApInfoList[i].bWPS ? "YES" : "NO");
                printf("Channel:   %d\n", pApInfoList[i].ulChannel);
                printf("Rate:      %d Mbps\n", pApInfoList[i].lRate/2);
                printf("SSID:      %d dB\n", pApInfoList[i].lPhyNoise);
                printf("**************************\n");
            }
            free(pApInfoList);
        }

        break;
    }

    case NETAPP_CB_FETCHED_APINFO:
        printf("%s() Received NETAPP_CB_FETCHED_APINFO, result=%d\n", __FUNCTION__, tResult);
        break;


    case NETAPP_CB_CONNECT:
        printf("%s() Received NETAPP_CB_CONNECT (%s) result=%d\n",
            __FUNCTION__, (tIFace == NETAPP_IFACE_WIRELESS) ? "Wireless" : "Bluetooth", tResult);

        if (tIFace == NETAPP_IFACE_WIRELESS)
        {
            if (tResult == NETAPP_SUCCESS)
            {
                print_connected_info();
                if (!bNoIP)
                {
                    NetAppSetNetworkSettings(hNetApp, tIFace, NETAPP_IP_MODE_DYNAMIC, NULL);
                }
                else
                {
                    connectOnlyMode = false;
                }
            }
            else
            {
                connectOnlyMode = false;
            }
        }
        if (tIFace == NETAPP_IFACE_BLUETOOTH)
        {
            if (tResult == NETAPP_SUCCESS)
            {
                NETAPP_BT_DEV_INFO  *pDevInfo = (NETAPP_BT_DEV_INFO*) pvBuffer;

                printf("\n***CONNECTED BT DEVICE******\n");
                printf("Name:       %s\n", pDevInfo->cName);
                printf("HD Addr:    %s\n", pDevInfo->cAddr);
                printf("RSSI:       %d dB\n", pDevInfo->lRssi);
                printf("****************************\n");

            }
        }
        break;

    case NETAPP_CB_INPUT_EVENT:
        printf("%s() Received NETAPP_CB_INPUT_EVENT, result=%d\n", __FUNCTION__, tResult);
        if (tIFace == NETAPP_IFACE_BLUETOOTH)
        {
            if (tResult == NETAPP_SUCCESS)
            {
                NETAPP_INPUT_INFO  *pInputInfo = (NETAPP_INPUT_INFO*) pvBuffer;
                printf("\n********INPUT INFO*********\n");
                printf("Key :       0x%x\n", pInputInfo->ulKey);
                printf("State:        %d\n", pInputInfo->bPressed);
                printf("****************************\n");

            }
        }
        break;

    case NETAPP_CB_PING:
        printf("%s() Received NETAPP_CB_PING, result=%d\n", __FUNCTION__, tResult);
        break;


    case NETAPP_CB_DNSLOOKUP:
        printf("%s() Received NETAPP_CB_DNSLOOKUP, result=%d\n", __FUNCTION__, tResult);
        break;


    case NETAPP_CB_SCANNED_APINFO:
        printf("%s() Received NETAPP_CB_SCANNED_APINFO, result=%d\n", __FUNCTION__, tResult);
        break;


    case NETAPP_CB_BT_DISCOVERY_RESULTS:
    {
        uint32_t            ulCount = 0;

        if (pBtDevInfoList != NULL)
        {
            free(pBtDevInfoList);
        }

        NetAppBluetoothGetDiscoveryResults(hNetApp, &pBtDevInfoList, &ulBtDevCount);
        printf("%s() Received NETAPP_CB_BT_DISCOVERY_RESULTS, result=%d count=%d\n", __FUNCTION__, tResult, ulCount);
        print_bt_dev_list();

        break;
    }

    case NETAPP_CB_P2P_CONNECT:
    {
        NETAPP_P2P_PEER_INFO* pPeerInfo = (NETAPP_P2P_PEER_INFO*)pvBuffer;
        struct stat stat_info;
        char    cTmp[256];

        if (pPeerInfo == NULL)
        {
            printf("Passed garbage!\n");
            return;
        }

        printf("%s(): Received P2P Connect Notification from %s:[ip:%s isGO=%s] \n",
            __FUNCTION__, pPeerInfo->tInfo.cSSID, NetAppNtoA(pPeerInfo->tIpAddress), pPeerInfo->bIsGO ? "YES" : "NO");

        if (stat(MIRACAST_BIN, &stat_info) != 0)
        {
            printf("\n********* W A R N I N G***********\n");
            printf("Standalone Miracast application %s not found.\n", MIRACAST_BIN);
            printf("********* W A R N I N G***********\n");
            break;
        }

        snprintf(cTmp, sizeof(cTmp), "%s -w rtsp://%s:%d -t -b 2 -p %d",
            MIRACAST_BIN, NetAppNtoA(pPeerInfo->tIpAddress), pPeerInfo->ulRtspPort, pPeerInfo->ulRtpPort);
        if (system(cTmp) < 0)
        {
            printf("Failed to execute: %s\n", cTmp);
            printf("Did you load nexus module first?\n");
        }

        break;
    }
    default:
        break;
    }
}

int menugetchoice(const char *querystring)
{
    int neg, value, c;

    neg = 1;
    printf("%s => ", querystring);
    value = 0;
    do
    {
        c = getchar();
        if ((c >= '0') && (c <= '9'))
        {
            value = (value * 10) + (c - '0');
        }
        else if (c == '-')
        {
            neg *= -1;
        }

    } while ((c != EOF) && (c != '\n'));

    return value * neg;
}

void print_p2p_menu()
{
    int c ;
    do
    {
        printf("WiFi Direct Command Options:\n");
        printf("\t1: Enable Discovery\n");
        printf("\t2: Disable Discovery\n");
        printf("\t3: Disconnect\n");
        printf("\t4: Toggle WiFi Direct Auto Discovery\n");
        printf("\t5: Start WPS Window\n");
        printf("\t99: Exit menu\n");

        switch (c = menugetchoice("Select action"))
        {
        case 1:
        {
            NetAppWiFiP2PDiscover(hNetApp);
            break;
        }
        case 2:
            NetAppWiFiP2PStopDiscovery(hNetApp);
            break;

        case 3:
            NetAppWiFiP2PDisconnect(hNetApp);
            NetAppSetNetworkSettings(hNetApp, NETAPP_IFACE_WIRELESS, NETAPP_IP_MODE_OFF, NULL);
            break;

        case 4:
        {
            NETAPP_SETTINGS tSettings;

            NetAppGetSettings(hNetApp, &tSettings);
            tSettings.bAutoP2PDiscover  = !tSettings.bAutoP2PDiscover;
            NetAppSetSettings(hNetApp, tSettings);

            printf("Auto P2P Discovery [%s]\n", tSettings.bAutoP2PDiscover ? "ON": "OFF");
            break;
        }

        case 5:
            NetAppWiFiP2PWpsPushButton(hNetApp);
            break;

        default:
            break;
        }
    } while (c != 99);
}


void print_usage()
{
    printf("NetAppTester: [-i | -c]\n");
    printf("Examples: \n");
    printf("Interactive:     NetAppTester -i \n");
    printf("WiFi Connect:    NetAppTester [-wowl | -noip] -c <ssid> <password optional>\n");
    printf("\t-wowl enables magic pattern wake up and puts the dongle in WoWL mode\n");
    printf("\t-noip Do not try and fetch an IP address when we associate to an AP\n");
    printf("\n");
}


void connect_menu()
{
    NETAPP_WIFI_AP_INFO tApInfo;
    char str[4];

    memset(&tApInfo, 0, sizeof(NETAPP_WIFI_AP_INFO));

    printf("Please enter SSID:\n");
    scanf("%s", tApInfo.cSSID);

    printf("\nPlease enter Password (enter '.' for nothing):\n");
    scanf("%s", tApInfo.cPassword);
    if (tApInfo.cPassword[0] == '.')
    {
        memset(tApInfo.cPassword, 0, sizeof(tApInfo.cPassword));
    }

    printf("\nChoose security mode: \n");
    printf("0: Auto Detect\n");
    printf("1: Open\n");
    printf("2: WPA-PSK-AES\n");
    printf("3: WPA-PSK-TKIP\n");
    printf("4: WPA2-PSK-AES\n");
    printf("5: WPA2-PSK-TKIP\n");
    printf("6: WEP\n");
    printf("7: Invalid\n");

    scanf("%s", str);

    switch (str[0])
    {
    case '0':
        tApInfo.tSecurity = NETAPP_WIFI_SECURITY_AUTO_DETECT;
        break;
    case '1':
        tApInfo.tSecurity = NETAPP_WIFI_SECURITY_NONE;
        break;
    case '2':
        tApInfo.tSecurity = NETAPP_WIFI_SECURITY_WPA_PSK_AES;
        break;
    case '3':
        tApInfo.tSecurity = NETAPP_WIFI_SECURITY_WPA_PSK_TKIP;
        break;
    case '4':
        tApInfo.tSecurity = NETAPP_WIFI_SECURITY_WPA2_PSK_AES;
        break;
    case '5':
        tApInfo.tSecurity = NETAPP_WIFI_SECURITY_WPA2_PSK_TKIP;
        break;
    case '6':
        tApInfo.tSecurity = NETAPP_WIFI_SECURITY_WEP;
        break;
    case '7':
        tApInfo.tSecurity = NETAPP_WIFI_SECURITY_INVALID;
        break;
    default:
        return;
    }

    printf("Connecting with SSID[%s] Password[%s]\n", tApInfo.cSSID, tApInfo.cPassword);
    NetAppWiFiConnect(hNetApp, &tApInfo);
}


void print_bluetooth_menu()
{
    int c ;
    int index_to_connect = 0;
    int status;

    do
    {
        printf("\n\nBluetooth Command Options:\n");
        printf("\t1: Discovery\n");
        printf("\t2: Connect\n");
        printf("\t3: Disconnect\n");
        printf("\t4: Start AV\n");
        printf("\t5: Stop AV\n");
        printf("\t6: Play file\n");
        printf("\n\t7: Get Saved List\n");
        printf("\t99: Exit\n");

        switch (c = menugetchoice("Select action"))
        {
        case 1:
        {
            if (NetAppBluetoothDiscovery(hNetApp, NETAPP_BT_SERVICE_HID | NETAPP_BT_SERVICE_A2DP) == NETAPP_SUCCESS)
            {
                printf("Discovery Started\n");
            }
            else
            {
                printf("Discovery Failed\n");
            }
            break;
        }


        case 2:
        case 3:
            if(ulBtDevCount == 0)
            {
                printf("Discovered list is empty\n");
                printf("Run a discovery first \n");
            }
            else
            {
                printf("Choose the device: \n");
                print_bt_dev_list();

                scanf("%d",&index_to_connect);
                if((unsigned)index_to_connect > ulBtDevCount)
                {
                    printf("Bad index !!! \n");
                }
                else
                {
                    NETAPP_RETCODE result;

                    result = (c == 2) ? NetAppBluetoothConnect(hNetApp, &pBtDevInfoList[index_to_connect]) :
                         NetAppBluetoothDisconnect(hNetApp, &pBtDevInfoList[index_to_connect]);

                    if (result == NETAPP_SUCCESS)
                    {
                        printf("%s \n", (c == 2) ? "Connecting..." : "Disconnected");
                        if(c == 2)
                          bIsConnected = true;
                        else
                          bIsConnected = false;
                    }
                    else
                    {
                        printf("Command failed.\n");
                    }
                }
            }
            break;
        case 4:
            {
                    NETAPP_RETCODE result;
                    app_wav_read_format(TEST_WAV_FILE, &tBtAudioFormat);

                    result = NetAppBluetoothAvStart(hNetApp, false, &tBtAudioFormat);

                    if (result == NETAPP_SUCCESS)
                    {
                        printf("%s \n", "Starting AV...");
                        bPlayStarted = true;
                    }
                    else
                    {
                        printf("Command failed.\n");
                    }

            }
            break;

        case 5:
            if(bPlayStarted == false)
            {
                printf("Stream is not started\n");
            }
            else
            {
                    NETAPP_RETCODE result;

                    result = NetAppBluetoothAvStop(hNetApp);

                    if (result == NETAPP_SUCCESS)
                    {
                        printf("%s \n", "Stopping AV...");
                        bPlayStarted = false;
                    }
                    else
                    {
                        printf("Command failed.\n");
                    }

            }
            break;

        case 6:
            if(bPlayStarted == false)
            {
                printf("Streaming not started\n");
            }
            else
            {
                printf("Start thread to play file \n");

                status = app_create_thread(app_uipc_tx_thread, 0, 0, &app_uipc_tx_thread_struct);

            }
            break;
        case 7:
        {
            NETAPP_BT_DEV_INFO  *pDevInfoList = NULL;
            uint32_t            ulCount = 0;
            unsigned int i;
            NetAppBluetoothGetSavedBtDevList(hNetApp, &pDevInfoList, &ulCount);
            printf("Saved Bluetooth Device List:\n");
            for (i = 0; i < ulCount; i++)
            {
                printf("\n%d:**************************\n", i);
                printf("Name:       %s\n",      pDevInfoList[i].cName);
                printf("BD Addr:    %s\n",      pDevInfoList[i].cAddr);
                printf("RSSI:       %d dB\n",   pDevInfoList[i].lRssi);
                printf("COD Major:  0x%02x\n",  pDevInfoList[i].ucMajorClassDev);
                printf("COD Minor:  0x%02x\n",  pDevInfoList[i].ucMinorClassDev);
                printf("COD Service:0x%04x\n",  pDevInfoList[i].usServiceClassDev);
                printf("%d:**************************\n", i);
            }
            if (pDevInfoList != NULL)
            {
                free(pDevInfoList);
            }
            break;
        }

        case 99:
            break;
        }
    } while (c != 99);

    if (pBtDevInfoList != NULL)
    {
        free(pBtDevInfoList);
        ulBtDevCount = 0;
    }
}

#ifdef HAS_NEXUS
void gpio_interrupt(void *context, int param)
{
    BSTD_UNUSED(param);
    printf("GOT THE EVENT!!!\n");
    BKNI_SetEvent((BKNI_EventHandle)context);
}
#endif

int interactive_loop()
{
    NETAPP_SETTINGS tSettings;
    char c;
    do
    {
        printf("\n\nMain Command Menu:\n");
        printf("\t0: Help\n");
        printf("\t1: Bluetooth menu\n");
        printf("\n\tWiFi Command Options:\n");
        printf("\t2:  Connect\n");
        printf("\t3:  WPS Connect (Push Button)\n");
        printf("\t4:  Disconnect\n");
        printf("\t5:  Scan (Start/Stop)\n");
        printf("\t6:  WiFi Direct Menu\n");
        printf("\t7:  Print IP Settings\n");
        printf("\t8:  Print WiFi Connected Info\n");
        printf("\t9:  WoWL Test\n");
        printf("\t10: Join a Bridge\n");
        printf("\t11: Leave Bridge\n");
        printf("\t12: Get Ap Information\n");
        printf("\t13: Select Antenna(s)\n");
        printf("\t14: Get Antenna Count\n");
        printf("\t15: Query active antenna(s)\n");
        printf("\t16: Control debug output\n");

        printf("\n\t20: Get Interface List\n");
        printf("\t21: Bridge Test\n");
        printf("\t22: Interface toggle Tests\n");
        printf("\t23: Toggle AP/STA mode\n");
        printf("\t99: Exit\n");

        switch (c = menugetchoice("Select action"))
        {
        /* Bluetooth Menu */
        case 1:
        {
            print_bluetooth_menu();
            break;
        }

        /* WiFi Connect */
        case 2:
        {
            connect_menu();
            break;
        }

        /* WiFi WPS Connect Push Button */
        case 3:
        {
            NetAppWiFiConnectByPb(hNetApp);
            printf("WPS Push Button Started...\n");
            break;
        }


        /* WiFi Disconnect */
        case 4:
            NetAppWiFiDisconnect(hNetApp);
            break;


        /* WiFi Scan Stop/Start */
        case 5:
        {
            if (scanning)
                NetAppWiFiStopScan(hNetApp);
            else
                NetAppWiFiStartScan(hNetApp, 4000, 200);

            scanning = !scanning;
            printf("Scanning %s.\n", scanning ? "Started" : "Stopped");
            break;
        }


        /* WiFi Direct Menu */
        case 6:
           print_p2p_menu();
           break;

        case 7:
        {
            NETAPP_IFACE iface = NETAPP_IFACE_MAX;
            printf("\n\n Interface:\n");
            printf("\t0: eth0\n");
            printf("\t1: eth1\n");
            printf("\t2: eth2\n");
            printf("\t3: eth3\n");
            printf("\t4: eth4\n");
            printf("\t5: Wireless\n");
            printf("\t99: Exit\n");

            switch (c = menugetchoice("Select action"))
            {
            /* Bluetooth Menu */
            case 0:
            case 1:
            case 2:
            case 3:
            case 4:
                iface = c;
                break;

            case 5:
                iface = NETAPP_IFACE_WIRELESS;
                break;

            default:
                break;
            }

            if (iface != NETAPP_IFACE_MAX)
            {
                print_ip_settings(iface);
            }
            break;
        }

        case 8:
            print_connected_info();
            break;

        case 9:
        {
            NETAPP_WIFI_AP_INFO tApInfo;

            memset(&tApInfo, 0, sizeof(tApInfo));
            if (NetAppWiFiGetConnectedApInfo(hNetApp, &tApInfo) != NETAPP_SUCCESS)
            {
                printf("Not Associated with an AP, please connect first\n");
                break;
            }

#ifdef HAS_NEXUS
            NEXUS_Error rc;
            NEXUS_GpioHandle pin;
            NEXUS_GpioSettings gpioSettings;
            BKNI_EventHandle event;
            NEXUS_PlatformSettings platformSettings;
            unsigned pinNumber = 0;

            pinNumber = menugetchoice("Please Enter GPIO Pin #");

            /************* Nexus GPIO Setup  *********************/
            NEXUS_Platform_GetDefaultSettings(&platformSettings);
            platformSettings.openFrontend = false;
            NEXUS_Platform_Init(&platformSettings);
            BKNI_CreateEvent(&event);

            printf("Enabling GPIO...\n");
            NEXUS_Gpio_GetDefaultSettings(NEXUS_GpioType_eAonStandard, &gpioSettings);
            gpioSettings.mode = NEXUS_GpioMode_eInput;
            gpioSettings.interruptMode = NEXUS_GpioInterrupt_eEdge;
            gpioSettings.interrupt.callback = gpio_interrupt;
            gpioSettings.interrupt.context = event;
            pin = NEXUS_Gpio_Open(NEXUS_GpioType_eAonStandard, pinNumber, &gpioSettings);

#else
            printf("WARNING: no built-in Nexus support available. \n");
            printf("You can still test WoWL using a separate Nexus app while this app is running\n");
#endif
            printf("Enabling WoWL...\n");
            NetAppGetSettings(hNetApp, &tSettings);
            tSettings.tWoWLSettings.ulMask  = NETAPP_WOWL_EVENT_MAGIC_PATTERN;
            tSettings.bAutoP2PDiscover = false;
            NetAppSetSettings(hNetApp, tSettings);
            sleep(1);
            printf("Closing NetApp...\n");
            NetAppClose(hNetApp);
            hNetApp = NULL;

#ifdef HAS_NEXUS
            printf("waiting for GPIO%d interrupt...\n", pinNumber);
            while (1) {
                NEXUS_GpioStatus gpioStatus;

                rc = BKNI_WaitForEvent(event, 0xFFFFFFFF);
                BDBG_ASSERT(!rc);

                rc = NEXUS_Gpio_GetStatus(pin, &gpioStatus);
                BDBG_ASSERT(!rc);

                printf("GPIO%d: value %d, interrupt status %d\n",
                    pinNumber,
                    gpioStatus.value,
                    gpioStatus.interruptPending);
            }

#endif
            break;
        }

        /* Join a bridge */
        case 10:
        {
            char str[12];

            printf("Please enter Bridge name (ex. br0):\n");
            scanf("%s", str);
            if (NetAppWiFiJoinBridge(hNetApp, str) == NETAPP_SUCCESS)
            {
                printf("Joined the bridge %s\n", str);
            }
            else
            {
                printf("Failed to join bridge %s\n", str);
            }
            break;
        }

        case 11:
            NetAppWiFiLeaveBridge(hNetApp);
            printf("Left the Bridge\n");
            break;

        case 12:
        {
            char str[NETAPP_MAX_SSID_LEN+1];

            printf("Please enter AP name:\n");
            scanf("%s", str);
            NetAppWiFiGetApInfo(hNetApp, str);
            printf("Searching....\n");
            break;
        }

        case 13:
        {
            uint32_t antennas = NETAPP_WIFI_ANTENNA_INVALID;

            printf("\n\nChoices:\n");
            printf("\t1: ANT_1\n");
            printf("\t2: ANT_2\n");
            printf("\t3: ANT_1, ANT_2\n");

            switch (c = menugetchoice("Select antenna(s)"))
            {
                case 1: antennas = NETAPP_WIFI_ANTENNA_1;
                    break;
                case 2: antennas = NETAPP_WIFI_ANTENNA_2;
                    break;
                case 3: antennas = NETAPP_WIFI_ANTENNA_1 | NETAPP_WIFI_ANTENNA_2;
                    break;
                default: printf("Ignoring invalid selection.\n");
                    break;
            }

            if (NETAPP_WIFI_ANTENNA_INVALID != antennas)
            {
                NetAppWiFiSetAntenna(hNetApp, NETAPP_WIFI_ANTENNA_MODE_RX, antennas);
                NetAppWiFiSetAntenna(hNetApp, NETAPP_WIFI_ANTENNA_MODE_TX, antennas);
            }
        }
        break;

        case 14:
        {
            uint32_t antennas = 0;
            NetAppWiFiGetAntennaCount(hNetApp, &antennas);
            printf("System reports %d antennas present\n", antennas);
        }
        break;

        case 15:
        {
            uint32_t antennas = 0;

            printf("Active RX antenna(s): ");
            NetAppWiFiGetAntenna(hNetApp, NETAPP_WIFI_ANTENNA_MODE_RX, &antennas);
            if ((NETAPP_WIFI_ANTENNA_1 & antennas) == NETAPP_WIFI_ANTENNA_1)
            {
                printf("ANT_1 ");
            }
            if ((NETAPP_WIFI_ANTENNA_2 & antennas) == NETAPP_WIFI_ANTENNA_2)
            {
                printf("ANT_2 ");
            }
            printf("\n");

            printf("Active TX antenna(s): ");
            NetAppWiFiGetAntenna(hNetApp, NETAPP_WIFI_ANTENNA_MODE_TX, &antennas);
            if ((NETAPP_WIFI_ANTENNA_1 & antennas) == NETAPP_WIFI_ANTENNA_1)
            {
                printf("ANT_1 ");
            }
            if ((NETAPP_WIFI_ANTENNA_2 & antennas) == NETAPP_WIFI_ANTENNA_2)
            {
                printf("ANT_2 ");
            }
            printf("\n");
        }
        break;

        case 16:
        {
            bool valid = true;
            NETAPP_DEBUG_DESTINATION dest = NETAPP_DEBUG_DESTINATION_NONE;

            printf("\n\nChoices:\n");
            printf("\t1: Only stdout\n");
            printf("\t2: Only syslog\n");
            printf("\t3: Both stdout and syslog\n");
            printf("\t4: No output whatsoever\n");

            switch (c = menugetchoice("Control debug output"))
            {
                case 1: dest = NETAPP_DEBUG_DESTINATION_STDOUT;
                    break;
                case 2: dest = NETAPP_DEBUG_DESTINATION_SYSLOG;
                    break;
                case 3: dest = NETAPP_DEBUG_DESTINATION_STDOUT | NETAPP_DEBUG_DESTINATION_SYSLOG;
                    break;
                case 4: dest = NETAPP_DEBUG_DESTINATION_NONE;
                    break;
                default:
                    valid = false;
                    printf("Ignoring invalid selection.\n");
                    break;
            }

            if (valid)
            {
                NetAppSetDebugOutput(hNetApp, dest);
            }
        }
        break;

        case 20:
        {
            NETAPP_IFACE_INFO   *pInfoList = NULL;
            uint32_t            ulLength = 0;
            NetAppGetIfaceInfo(hNetApp, &pInfoList, &ulLength);

            if (pInfoList != NULL)
            {
                unsigned int j;
                printf("***************\n");

                for (j = 0; j < ulLength; j++)
                {
                    if (pInfoList[j].bPresent)
                    {
                        printf("%s:\t PRESENT\n", pInfoList[j].cName);
                    }
                }
                printf("***************\n");
                free(pInfoList);
            }
            break;
        }
        case 21:
        {
            char    pTmp[256];
            char    cBridgeName[20];
            char    *pIfaceName = NULL;

            printf("Enter Bridge Name: ");
            scanf("%s", cBridgeName);
            snprintf(pTmp, sizeof(pTmp), "brctl addbr %s", cBridgeName);
            system(pTmp);

            NetAppWiFiJoinBridge(hNetApp, cBridgeName);
            NetAppGetIfaceName(hNetApp, NETAPP_IFACE_WIRELESS, &pIfaceName);

            if (pIfaceName == NULL)
            {
                printf("DIE!\n");
                return -1;
            }
            printf("Adding %s to the Bridge:\n", pIfaceName);

            snprintf(pTmp, sizeof(pTmp), "brctl addif %s %s", cBridgeName, pIfaceName);
            system(pTmp);

            printf("Bringing up bridge %s...", cBridgeName);
            snprintf(pTmp, sizeof(pTmp), "ifconfig %s up", cBridgeName);
            system(pTmp);
            printf("Done.\n");
            printf("Bridge is up and ready to use!\n");
            break;
        }

        case 22:
        {
            NETAPP_WIFI_AP_INFO tApInfo;

            memset(&tApInfo, 0, sizeof(tApInfo));
            if (NetAppWiFiGetConnectedApInfo(hNetApp, &tApInfo) != NETAPP_SUCCESS)
            {
                printf("Not Associated with an AP, please connect first\n");
                break;
            }

            printf("Calling Disconnect...\n");
            NetAppWiFiDisconnect(hNetApp);
            printf("Calling link down wifi...\n");
            NetAppSetNetworkSettings(hNetApp, NETAPP_IFACE_WIRELESS, NETAPP_IP_MODE_OFF, NULL);
            printf("Calling link up eth0...\n");
            NetAppSetNetworkSettings(hNetApp, NETAPP_IFACE_WIRED, NETAPP_IP_MODE_DYNAMIC, NULL);
            printf("Done test!\n");
            break;
        }

        case 23:
        {
            NetAppGetSettings(hNetApp, &tSettings);
            tSettings.bIsSoftAp = !tSettings.bIsSoftAp;
            NetAppSetSettings(hNetApp, tSettings);
            printf("NetApp is %s\n", tSettings.bIsSoftAp ? "AP" : "STA");
            break;
        }

        default:
            break;
        }
    } while (c != 99);

    return 0;
}

void callback2(
    void                *pParam,        //!< The pParam from NetAppOpen
    NETAPP_CB_TYPE      tCbType,        //!< The Callback type
    const void          *pvBuffer,      //!< Pointer to data specific to the callback
    uint32_t            ulData0,        //!< Callback specific data 0
    NETAPP_RETCODE      tResult,        //!< Callback results from the
    NETAPP_IFACE        tIFace)         //!< Callback Info structure
{
    (void)pParam;
    (void)pvBuffer;
    (void)ulData0;
    (void)tResult;
    (void)tIFace;
    printf("%s() ENTERED, CB=%d\n", __FUNCTION__, tCbType);;
}


void callback3(
    void                *pParam,        //!< The pParam from NetAppOpen
    NETAPP_CB_TYPE      tCbType,        //!< The Callback type
    const void          *pvBuffer,      //!< Pointer to data specific to the callback
    uint32_t            ulData0,        //!< Callback specific data 0
    NETAPP_RETCODE      tResult,        //!< Callback results from the
    NETAPP_IFACE        tIFace)         //!< Callback Info structure
{
    (void)pParam;
    (void)pvBuffer;
    (void)ulData0;
    (void)tResult;
    (void)tIFace;
    printf("%s() ENTERED, CB=%d\n", __FUNCTION__, tCbType);
}


/* Main Routine */
int main(int argc, char* argv[])
{
    NETAPP_SETTINGS         tSettings;
    NETAPP_INIT_SETTINGS    tInitSettings;
    NETAPP_OPEN_SETTINGS    tOpenSettings;
    NETAPP_VERSION_INFO    tVersionInfo;
    int             arg = 1;
    bool            bInteractive = false;

    NetAppGetVersionInfo(&tVersionInfo);

    NetAppGetVersionInfo(&tVersionInfo);

    printf("######################################################\n");
    printf("           NetApp Test (version %d.%d.%d)\n", tVersionInfo.ulMajor, tVersionInfo.ulMinor, tVersionInfo.ulInc);
    printf("#####################################################\n");

    if (argc < 2)
    {
        print_usage();
        return -1;
    }

    signal(SIGTERM, &signal_handler);
    signal(SIGTERM, &signal_handler);

    NetAppGetDefaultSettings(&tSettings);
    NetAppGetDefaultInitSettings(&tInitSettings);

    tOpenSettings.tCallback         = callback;

    tInitSettings.WiFiIfacePrefix   = "sdh";
    tInitSettings.tMimoBwCap        = NETAPP_MIMO_BW_CAP_40MHZ;

    tSettings.bForceWiFi            = true;
    tSettings.bAutoReConnect        = true;
    tSettings.tBtSettings.bAutoPair = true;
    tSettings.bHideDuplicateAPs     = true;
    tSettings.bAutoP2PDiscover      = false;
    tSettings.bZeroconfOn           = false;
    tSettings.bIsSoftAp             = false; /* Device is a client or SoftAP */
    tSettings.bWiFiBridgingMode     = true;  /* enable bridging support */

    CHECK(NetAppOpen(&hNetApp, &tOpenSettings, &tInitSettings, &tSettings));

    /* Testing multiple API Open */
    tOpenSettings.tCallback         = callback2;
    CHECK(NetAppOpen(&hNetApp2, &tOpenSettings, NULL, NULL));

    tOpenSettings.tCallback         = callback3;
    CHECK(NetAppOpen(&hNetApp3, &tOpenSettings, NULL, NULL));

    while (arg < argc)
    {
        if (strcmp(argv[arg], "-c") == 0)
        {
            NETAPP_WIFI_AP_INFO tApInfo;
            connectOnlyMode = true;
            memset(&tApInfo, 0, sizeof(NETAPP_WIFI_AP_INFO));
            if (++arg >= argc)
            {
                print_usage();
                return -1;
            }
            strncpy(tApInfo.cSSID, argv[arg], sizeof(tApInfo.cSSID));

            if (++arg < argc)
            {
                strncpy(tApInfo.cPassword, argv[arg], sizeof(tApInfo.cPassword));
            }
            tApInfo.tSecurity = NETAPP_WIFI_SECURITY_AUTO_DETECT;
            printf("Connecting to SSID[%s] Password[%s]\n", tApInfo.cSSID, (tApInfo.cPassword[0] == '\0') ? "NULL" : tApInfo.cPassword);
            CHECK(NetAppWiFiConnect(hNetApp, &tApInfo));
        }

        else if (strcmp(argv[arg], "-wowl") == 0)
        {
            NetAppGetSettings(hNetApp, &tSettings);
            tSettings.tWoWLSettings.ulMask  = NETAPP_WOWL_EVENT_MAGIC_PATTERN;
            NetAppSetSettings(hNetApp, tSettings);
            printf("Finished enabling WoWL\n");
        }

        else if (strcmp(argv[arg], "-noip") == 0)
        {
            bNoIP = true;
        }

        /* Interactive mode */
        else if (strcmp(argv[arg], "-i") == 0)
        {
            bInteractive = true;
        }
        else if (strcmp(argv[arg], "-d") == 0)
        {
            NetAppWiFiDisconnect(hNetApp);
            break;
        }
        else if (strcmp(argv[arg], "-softap") == 0)
        {
            NetAppGetSettings(hNetApp, &tSettings);
            tSettings.bIsSoftAp = true;
            NetAppSetSettings(hNetApp, tSettings);
        }
        else
        {
            print_usage();
            return -1;
        }
        arg++;
    }

    if (bInteractive)
    {
        interactive_loop();
    }

    while (connectOnlyMode)
    {
        sleep(1);
    }

err_out:
    printf("Exiting now!\n");
    if (hNetApp != NULL)
    {
        NetAppClose(hNetApp);
    }
    return 0;
}


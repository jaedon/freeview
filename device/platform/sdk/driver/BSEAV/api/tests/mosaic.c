/***************************************************************************
 *     Copyright (c) 2003-2008, Broadcom Corporation
 *     All Rights Reserved
 *     Confidential Property of Broadcom Corporation
 *
 *  THIS SOFTWARE MAY ONLY BE USED SUBJECT TO AN EXECUTED SOFTWARE LICENSE
 *  AGREEMENT  BETWEEN THE USER AND BROADCOM.  YOU HAVE NO RIGHT TO USE OR
 *  EXPLOIT THIS MATERIAL EXCEPT SUBJECT TO THE TERMS OF SUCH AN AGREEMENT.
 *
 * $brcm_Workfile: mosaic.c $
 * $brcm_Revision: 6 $
 * $brcm_Date: 1/13/08 10:34a $
 *
 * Module Description:
 *
 * Revision History:
 *
 * $brcm_Log: /BSEAV/api/tests/mosaic.c $
 * 
 * 6   1/13/08 10:34a katrep
 * PR36809: Fixed compile error with the nptl toolchain
 ***************************************************************************/ 
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include "bsettop.h"
#include "bstd.h"
#include "bkni.h"
#include "bkni_multi.h"
#include "blst_queue.h"

#define MAX_NUM_MOSAICS (12)

/* Reserve 10% of screen as un-drawable */
#define SAFE_TOP (1080/10)
#define SAFE_LEFT (1920/10)
#define SAFE_BOTTOM (1080-SAFE_TOP)
#define SAFE_RIGHT (1920-SAFE_LEFT)

/* Size of highlight box around window */
#define SELECTION_BOX_WIDTH 8

/* "UI" size & colors */
#define SELECTION_BOX_COLOR (0xff117711) /* Opaque White */
#define BACKGROUND_COLOR (0xff373755) /* Opaque Blue */
/* #define BACKGROUND_COLOR (0xff101010)*/ /* Opaque near-black */
#define HOLE_COLOR (0x0) /* Transparent Black */
#define FRAMEBUFFER_HEIGHT 1080
#define FRAMEBUFFER_WIDTH 1920

/* Timer tick interval for time-based updates */
#define TIMER_INTERVAL (500)

/* Settop Resources */
/* Display_Init */
static bdisplay_t  g_display;
static bgraphics_t g_graphics;
static bsurface_t  g_framebuffer;

/* Window_Init */
static bdecode_window_t g_primary_window;

/* Decode_Init */
static bdecode_t        g_primary_decode;
static bplayback_t      g_playback;
static bplaypump_t      g_playpump;
static btuner_t         g_qam;
static btuner_t         g_vsb;
static btuner_t         g_sds;

/* Decode_Start/Stop */
static bdecode_t        g_audio_decode;         /* bdecode_t containing the audio decoder, NULL if none */
static baudio_decode_t  g_audio_decode_handle;  /* return value from bdecode_detach_audio.  Only this or g_audio_decode is valid */
static bband_t          g_band;
static bdecode_t        g_mosaic_decodes[MAX_NUM_MOSAICS];
static bdecode_window_t g_mosaic_windows[MAX_NUM_MOSAICS];
static bstream_t        g_mosaic_streams[MAX_NUM_MOSAICS];

/* LED_Init */
static buser_output_t g_led;

/* UI Events */
typedef enum EventType
{
    EVENT_TYPE_eNONE,
    EVENT_TYPE_ePOWER,
    EVENT_TYPE_eLEFT,
    EVENT_TYPE_eRIGHT,
    EVENT_TYPE_eUP,
    EVENT_TYPE_eDOWN,
    EVENT_TYPE_eSELECT,
    EVENT_TYPE_eCHAN_UP,
    EVENT_TYPE_eCHAN_DOWN,
    EVENT_TYPE_eVOL_UP,
    EVENT_TYPE_eVOL_DOWN,
    EVENT_TYPE_eMENU,
    EVENT_TYPE_eGUIDE,
    EVENT_TYPE_eINFO,
    EVENT_TYPE_eEXIT,
    EVENT_TYPE_eMUTE,
    EVENT_TYPE_e0,
    EVENT_TYPE_e1,
    EVENT_TYPE_e2,
    EVENT_TYPE_e3,
    EVENT_TYPE_e4,
    EVENT_TYPE_e5,
    EVENT_TYPE_e6,
    EVENT_TYPE_e7,
    EVENT_TYPE_e8,
    EVENT_TYPE_e9,
    EVENT_TYPE_eTIMER,
    EVENT_TYPE_eMAX
} EventType;

const char *g_pszEventNames[EVENT_TYPE_eMAX] = 
{
    "EVENT_TYPE_eNONE",
    "EVENT_TYPE_ePOWER",
    "EVENT_TYPE_eLEFT",
    "EVENT_TYPE_eRIGHT",
    "EVENT_TYPE_eUP",
    "EVENT_TYPE_eDOWN",
    "EVENT_TYPE_eSELECT",
    "EVENT_TYPE_eCHAN_UP",
    "EVENT_TYPE_eCHAN_DOWN",
    "EVENT_TYPE_eVOL_UP",
    "EVENT_TYPE_eVOL_DOWN",
    "EVENT_TYPE_eMENU",
    "EVENT_TYPE_eGUIDE",
    "EVENT_TYPE_eINFO",
    "EVENT_TYPE_eEXIT",
    "EVENT_TYPE_eMUTE",
    "EVENT_TYPE_e0",
    "EVENT_TYPE_e1",
    "EVENT_TYPE_e2",
    "EVENT_TYPE_e3",
    "EVENT_TYPE_e4",
    "EVENT_TYPE_e5",
    "EVENT_TYPE_e6",
    "EVENT_TYPE_e7",
    "EVENT_TYPE_e8",
    "EVENT_TYPE_e9",
    "EVENT_TYPE_eTIMER"
};

/* UI State */
static bool g_bRunning = true;
typedef void (*EventHandler)(EventType);
static EventHandler g_pEventHandler = NULL;
static int g_iProgramNum;

/* Programs */
typedef enum SourceType
{
    SOURCE_TYPE_eSTREAMER=1,
    SOURCE_TYPE_eFILE,
    SOURCE_TYPE_eQAM,
    SOURCE_TYPE_eVSB,
    SOURCE_TYPE_eSDS
} SourceType;

typedef struct ProgramNode
{
    BLST_Q_ENTRY(ProgramNode) stNode;

    SourceType eSourceType;
    unsigned  uiFrequency; /* Hz, only if eSourceType is a tuner */
    union
    {
        bstream_mpeg_type eMpegType;
        bplayback_file_t  hFile;
        btuner_qam_params stQam;
        btuner_vsb_params stVsb;
        btuner_sds_params stSds;
    } stSource;

    unsigned uiNumPrograms;
    struct
    {
        bstream_mpeg stPids;
        bdecode_mosaic_settings stMosaicSettings;
        bsettop_rect stPosition;
    } stProgramInfo[MAX_NUM_MOSAICS];

    int iRows;
    int iCols;
    int iChannelNum;
    char szChannelName[256];
} ProgramNode;
static BLST_Q_HEAD(ProgramList, ProgramNode) g_stProgramList;
static ProgramNode *g_pCurrentProgram;

static int Program_Col(ProgramNode *pNode, unsigned uiProgramNum)
{
    return uiProgramNum % pNode->iCols;
}

static int Program_Row(ProgramNode *pNode, unsigned uiProgramNum)
{
    return uiProgramNum / pNode->iCols;
}

static void Program_LayoutWindows(ProgramNode *pNode)
{
    unsigned i;

    switch ( pNode->uiNumPrograms )
    {
    case 1:
        pNode->iCols = 1;
        pNode->iRows = 1;
        break;
    case 2:
        pNode->iCols = 2;
        pNode->iRows = 1;
        break;
    case 3:
    case 4:
        pNode->iCols = 2;
        pNode->iRows = 2;
        break;
    case 5:
    case 6:
        pNode->iCols = 3;
        pNode->iRows = 2;
        break;
    case 7:
    case 8:
    case 9:
        pNode->iCols = 3;
        pNode->iRows = 3;
        break;
    case 10:
    case 11:
    case 12:
        pNode->iCols = 4;
        pNode->iRows = 3;
        break;
    default:
        pNode->iCols = 4;
        pNode->iRows = 4;
        break;
    }
    /* Loop done, now position rectangles */
    for ( i = 0; i < pNode->uiNumPrograms; i++ )
    {
        int iRow, iCol;
        iCol = i % pNode->iCols;
        iRow = i / pNode->iCols;
        pNode->stProgramInfo[i].stPosition.height = pNode->stProgramInfo[i].stMosaicSettings.height;
        pNode->stProgramInfo[i].stPosition.width = pNode->stProgramInfo[i].stMosaicSettings.width;
        pNode->stProgramInfo[i].stPosition.x = SAFE_LEFT + SELECTION_BOX_WIDTH + (iCol * (SELECTION_BOX_WIDTH+((SAFE_RIGHT-SAFE_LEFT)/pNode->iCols)));
        pNode->stProgramInfo[i].stPosition.y = SAFE_TOP + SELECTION_BOX_WIDTH + (iRow * (SELECTION_BOX_WIDTH+((SAFE_BOTTOM-SAFE_TOP)/pNode->iRows)));
        #if 0
        printf("Layout - window %d [row %d col %d] at %d,%d size %dx%d\n", i, iRow, iCol, 
               pNode->stProgramInfo[i].stPosition.x, pNode->stProgramInfo[i].stPosition.y,
               pNode->stProgramInfo[i].stPosition.width, pNode->stProgramInfo[i].stPosition.height);
        #endif
    }
}

static baudio_format ProgramList_ParseAudioType(const char *pszArg)
{
    static const struct
    {
        char *pszName;
        baudio_format format;
    } astAudioTable[] = 
    {
        {"MPEG", baudio_format_mpeg},
        {"MPEG1", baudio_format_mpeg},
        {"MPEG1L2", baudio_format_mpeg},
        {"MP3", baudio_format_mp3},
        {"MPEG1L3", baudio_format_mp3},
        {"AC3", baudio_format_ac3},
        {"AAC", baudio_format_aac},
        {"AAC+", baudio_format_aac_plus},
        {"AAC+_LOAS", baudio_format_aac_plus_loas},
        {"DTS", baudio_format_dts},
        {"AC3+", baudio_format_ac3_plus},
        {"DDP", baudio_format_ac3_plus},
        {"DTSHD", baudio_format_dts_hd},
        {"WMA", baudio_format_wma_std},
        {"WMASTD", baudio_format_wma_std},
        {"WMAPRO", baudio_format_wma_pro},
        {NULL, baudio_format_ac3}
    };

    int i;

    for ( i = 0; astAudioTable[i].pszName; i++ )
    {
        if ( !strcasecmp(pszArg, astAudioTable[i].pszName) )
        {
            return astAudioTable[i].format;
        }
    }

    return (baudio_format)strtoul(pszArg, NULL, 16);
}

static bvideo_codec ProgramList_ParseVideoType(const char *pszArg)
{
    static const struct
    {
        char *pszName;
        bvideo_codec codec;
    } astVideoTable[] = 
    {
        {"MPEG", bvideo_codec_mpeg2},
        {"MPEG2", bvideo_codec_mpeg2},
        {"MPEG1", bvideo_codec_mpeg1},
        {"AVC", bvideo_codec_h264},
        {"H.264", bvideo_codec_h264},
        {"H264", bvideo_codec_h264},
        {"H263", bvideo_codec_h263},
        {"H.263",bvideo_codec_h263},
        {"VC1", bvideo_codec_vc1},
        {"VC-1", bvideo_codec_vc1},
        {"VC1-SM", bvideo_codec_vc1_sm},
        {"VC1SM", bvideo_codec_vc1_sm},
        {"VC-1SM", bvideo_codec_vc1_sm},
        {"VC-1-SM", bvideo_codec_vc1_sm},
        {"DivX3", bvideo_codec_divx_311},
        {"DivX-3", bvideo_codec_divx_311},
        {"MPEG4", bvideo_codec_mpeg4_part2},
        {"MPEG4-2", bvideo_codec_mpeg4_part2},
        {NULL, 0}
    };

    int i;

    for ( i = 0; astVideoTable[i].pszName; i++ )
    {
        if ( !strcasecmp(pszArg, astVideoTable[i].pszName) )
        {
            return astVideoTable[i].codec;
        }
    }

    return (bvideo_codec)strtoul(pszArg, NULL, 16);
}

static void ProgramList_ParseVideoRes(bdecode_mosaic_settings *pSettings, const char *pszArg)
{
    static const struct
    {
        char *pszName;
        int width;
        int height;
    } astSettingsTable[] = 
    {
        {"QCIF", 176, 144},
        {"CIF", 352, 288},
        {"SD", 720, 480},
        {"HD", 1920, 1080},
        {NULL, 0, 0}        /* Default */
    };

    int i;

    for ( i = 0; astSettingsTable[i].pszName; i++ )
    {
        if ( !strcasecmp(pszArg, astSettingsTable[i].pszName) )
        {
            pSettings->height = astSettingsTable[i].height;
            pSettings->width = astSettingsTable[i].width;
            return;
        }
    }

    pSettings->height = pSettings->width = 0;
    sscanf(pszArg, "%ux%u", &pSettings->width, &pSettings->height);
    return;
}

static bresult ProgramList_Parse(void)
{
    unsigned i;
    unsigned uiHeight=288, uiWidth=352;
    FILE *pFile;
    int iNumChannels=0;

    pFile = fopen("mosaic_channels.txt", "r");
    if ( NULL == pFile )
    {
        printf("mosaic_channels.txt file not found.\n");
        return berr_external_error;
    }
    else
    {
        #define LINE_BUF_SIZE 256
        char szLineBuf[LINE_BUF_SIZE];
        ProgramNode *pNode = NULL;

        while ( fgets(szLineBuf, LINE_BUF_SIZE, pFile) )
        {
            #define MAX_ARGS 16
            char *pszArgs[MAX_ARGS] = {0};
            char *pszCurrent;
            int iCount=0;
            int i;

            /* Strip Comment */
            if ( pszCurrent = strchr(szLineBuf, '#') )
            {
                pszCurrent[0] = '\0';
            }
            if ( pszCurrent = strchr(szLineBuf, '\n') )
            {
                pszCurrent[0] = '\0';
            }

            /* Split line into args */
            pszCurrent = szLineBuf;

            while ( pszCurrent && pszCurrent[0] && iCount < MAX_ARGS )
            {
                pszCurrent += strspn(pszCurrent, " \t");
                pszArgs[iCount++] = strsep(&pszCurrent, " \t");
            }

            for ( i = iCount; i < MAX_ARGS; i++ )
            {
                pszArgs[i] = ""; /* Prevent many crashes */
            }

            /* Drop out if we have nothing on this line */
            if ( 0 == iCount )
            {
                /* A blank line will terminate the current program */
                if ( pNode )
                {
                    if ( pNode->uiNumPrograms > 0 )
                    {
                        Program_LayoutWindows(pNode);
                        BLST_Q_INSERT_TAIL(&g_stProgramList, pNode, stNode);
                    }
                    else
                        free(pNode);
                    pNode = NULL;
                }
                continue;
            }

            /* Are we adding to an existing service? */
            if ( 0 == strcasecmp("PROGRAM", pszArgs[0]) )
            {
                if ( NULL == pNode || pNode->uiNumPrograms >= MAX_NUM_MOSAICS )
                {
                    /* Must have a tuner type beforehand , and can't exceed the max # of mosaics */
                    continue;
                }

                /* Set Default audio and video formats */
                bstream_mpeg_init(&pNode->stProgramInfo[pNode->uiNumPrograms].stPids);
                bdecode_mosaic_settings_init(g_primary_decode, &pNode->stProgramInfo[pNode->uiNumPrograms].stMosaicSettings);
                pNode->stProgramInfo[pNode->uiNumPrograms].stPids.video[0].format = bvideo_codec_mpeg2;
                pNode->stProgramInfo[pNode->uiNumPrograms].stPids.audio[0].format = baudio_format_ac3;

                /* For each argument */
                for ( i = 1; i < iCount; i++ )
                {
                    /* pcr video audio audio_type video_type video_res */
                    if ( !strncasecmp("pcr=", pszArgs[i], 3) )
                    {
                        pNode->stProgramInfo[pNode->uiNumPrograms].stPids.pcr_pid = (uint16_t)strtoul(pszArgs[i]+4, NULL, 16);
                    }
                    else if ( !strncasecmp("video=", pszArgs[i], 6) )
                    {
                        pNode->stProgramInfo[pNode->uiNumPrograms].stPids.video[0].pid = (uint16_t)strtoul(pszArgs[i]+6, NULL, 16);
                    }
                    else if ( !strncasecmp("audio=", pszArgs[i], 6) )
                    {
                        pNode->stProgramInfo[pNode->uiNumPrograms].stPids.audio[0].pid = (uint16_t)strtoul(pszArgs[i]+6, NULL, 16);
                    }
                    else if ( !strncasecmp("audio_type=", pszArgs[i], 11) )
                    {
                        pNode->stProgramInfo[pNode->uiNumPrograms].stPids.audio[0].format = ProgramList_ParseAudioType(pszArgs[i]+11);
                    }
                    else if ( !strncasecmp("video_type=", pszArgs[i], 11) )
                    {
                        pNode->stProgramInfo[pNode->uiNumPrograms].stPids.video[0].format = ProgramList_ParseVideoType(pszArgs[i]+11);
                    }
                    else if ( !strncasecmp("video_res=", pszArgs[i], 10) )
                    {
                        ProgramList_ParseVideoRes(&pNode->stProgramInfo[pNode->uiNumPrograms].stMosaicSettings, pszArgs[i]+10);
                    }
                }

                pNode->uiNumPrograms++;
            }
            else
            {
                /* We're starting a new service. */
                if ( pNode )
                {
                    if ( pNode->uiNumPrograms > 0 )
                    {
                        Program_LayoutWindows(pNode);
                        BLST_Q_INSERT_TAIL(&g_stProgramList, pNode, stNode);
                    }
                    else
                        free(pNode);
                }

                pNode = calloc(1, sizeof(ProgramNode));
                iNumChannels++;
                pNode->iChannelNum = iNumChannels;

                /* Determine input type */
                if ( !strcasecmp(pszArgs[0], "STREAMER") )
                {
                    pNode->eSourceType = SOURCE_TYPE_eSTREAMER;
                    pNode->stSource.eMpegType = bstream_mpeg_type_ts;
                    sprintf(pNode->szChannelName, "CH %d - TS IN", iNumChannels);
                }
                else if ( g_qam && 
                          (!strcasecmp(pszArgs[0], "QAM64") ||
                           !strcasecmp(pszArgs[0], "QAM256")) )
                {
                    float fFreq=0.0;
                    pNode->eSourceType = SOURCE_TYPE_eQAM;
                    btuner_qam_params_init(&pNode->stSource.stQam, g_qam);
                    if ( pszArgs[0][3] == '6' )
                        pNode->stSource.stQam.mode = btuner_qam_mode_64;
                    else
                        pNode->stSource.stQam.mode = btuner_qam_mode_256;

                    /* Freq is second argument */
                    fFreq = atof(pszArgs[2]);
                    fFreq *= 1000000;
                    pNode->uiFrequency = (unsigned)fFreq;
                    sprintf(pNode->szChannelName, "CH %d - QAM", iNumChannels);
                }
                else if ( g_sds && !strncasecmp(pszArgs[0], "QPSK", 4) )
                {
                    float fFreq=0.0, fSymRate=0.0;
                    pNode->eSourceType = SOURCE_TYPE_eSDS;
                    btuner_sds_params_init(&pNode->stSource.stSds, g_sds);
                    if ( !strcasecmp(pszArgs[0]+4, "_DVB") )
                    {
                        pNode->stSource.stSds.mode = btuner_sds_mode_qpsk_dvb;
                    }
                    else
                    {
                        /* DirecTV not supported */
                        fprintf(stderr, "DSS not supported at this time");
                        free(pNode);
                        continue;
                    }
                    /* Params are Freq, Symbol Rate, voltage, tone */
                    fFreq = atof(pszArgs[2]);
                    fFreq *= 1000000;
                    pNode->uiFrequency = (unsigned)fFreq;
                    fSymRate = atof(pszArgs[3]);
                    fSymRate *= 1000000;
                    pNode->stSource.stSds.symbol_rate = (unsigned)fSymRate;
                    if ( !strcasecmp(pszArgs[4], "13v") )
                        pNode->stSource.stSds.diseqc_polarity = btuner_sds_diseqc_polarity_13v;
                    else
                        pNode->stSource.stSds.diseqc_polarity = btuner_sds_diseqc_polarity_18v;
                     if ( !strcasecmp(pszArgs[5], "ToneOn") )
                         pNode->stSource.stSds.diseqc_tone = true;
                     else
                         pNode->stSource.stSds.diseqc_tone = false;
                     sprintf(pNode->szChannelName, "CH %d - QPSK", iNumChannels);
                }
                else if ( g_vsb && !strcasecmp(pszArgs[0], "VSB8") )
                {
                    float fFreq=0.0;
                    pNode->eSourceType = SOURCE_TYPE_eVSB;
                    btuner_vsb_params_init(&pNode->stSource.stVsb, g_vsb);
                    fFreq = atof(pszArgs[2]);
                    fFreq *= 1000000;
                    pNode->uiFrequency = (unsigned)fFreq;
                    sprintf(pNode->szChannelName, "CH %d - VSB", iNumChannels);
                }
                else if ( !strcasecmp(pszArgs[0], "FILE") )
                {
                    char *pszFilePart;
                    pNode->eSourceType = SOURCE_TYPE_eFILE;
                    pNode->stSource.hFile = bplayback_file_open(pszArgs[2], NULL);
                    if ( NULL == pNode->stSource.hFile )
                    {
                        fprintf(stderr, "Unable to open file '%s'\n", pszArgs[2]);
                        free(pNode);
                        continue;
                    }
                    /* Strip path to file */
                    pszFilePart = rindex(pszArgs[2], '/');
                    if ( NULL == pszFilePart )
                    {
                        pszFilePart = pszArgs[2];
                    }
                    sprintf(pNode->szChannelName, "CH %d - %s", iNumChannels, pszFilePart);
                }
                else
                {
                    fprintf(stderr, "Unrecognized program type '%s'\n", pszArgs[0]);
                    free(pNode);
                    continue;
                }
            }
        }

        /* Add last program to list */
        if ( pNode )
        {
            if ( pNode->uiNumPrograms > 0 )
            {
                Program_LayoutWindows(pNode);
                BLST_Q_INSERT_TAIL(&g_stProgramList, pNode, stNode);
            }
            else
                free(pNode);
        }

        fclose(pFile);
    }
    return b_ok;
}

static bresult Display_Init(void)
{
    bresult rc;
    bdisplay_settings display_settings;
    bsurface_create_settings surface_create_settings;
    bgraphics_settings graphics_settings;

    g_display = bdisplay_open(B_ID(0));
    if ( NULL == g_display )
    {
        fprintf(stderr, "Unable to open display\n");
        return berr_external_error;
    }   

    bdisplay_get(g_display, &display_settings);
    display_settings.composite = NULL;
    display_settings.svideo = NULL;
    display_settings.rf = NULL;
    display_settings.format = bvideo_format_1080i;  

    rc = bdisplay_set(g_display, &display_settings);
    if ( rc )
    {
        fprintf(stderr, "Unable to set display settings\n");
        return rc;
    }

    g_graphics = bgraphics_open(B_ID(0), g_display);
    if ( NULL == g_graphics )
    {
        fprintf(stderr, "Unable to open graphics\n");
        return berr_external_error;
    }

    bsurface_create_settings_init(&surface_create_settings, g_graphics);
    surface_create_settings.pixel_format = bgraphics_pixel_format_a1_r5_g5_b5;
    surface_create_settings.height = FRAMEBUFFER_HEIGHT;
    surface_create_settings.width = FRAMEBUFFER_WIDTH;
    g_framebuffer = bgraphics_create_framebuffer(g_graphics, &surface_create_settings, 1);  /* double-buffered */
    if ( NULL == g_framebuffer )
    {
        fprintf(stderr, "Unable to create framebuffer\n");
        return berr_external_error;
    }

    bgraphics_get(g_graphics, &graphics_settings);
    graphics_settings.async_blits = true;
    graphics_settings.source_height = FRAMEBUFFER_HEIGHT;
    graphics_settings.source_width = FRAMEBUFFER_WIDTH;
    graphics_settings.destination_height = FRAMEBUFFER_HEIGHT;
    graphics_settings.destination_width = FRAMEBUFFER_WIDTH;
    rc = bgraphics_set(g_graphics, &graphics_settings);
    if ( rc )
    {
        fprintf(stderr, "Unable to set graphics settings\n");
        return rc;
    }

    /* Success */
    return b_ok;
}

static bresult Display_Uninit(void)
{
    bgraphics_close(g_graphics);
    bdisplay_close(g_display);
    return b_ok;
}

static bresult Window_Init(void)
{
    bresult rc;
    int i;
    
    g_primary_window = bdecode_window_open(B_ID(0), g_display);
    if ( NULL == g_primary_window )
    {
        fprintf(stderr, "Unable to open primary decode window\n");
        return berr_external_error;
    }

    return b_ok;
}

bresult Window_Uninit(void)
{
    bdecode_window_close(g_primary_window);

    return b_ok;
}

bresult Decode_Init(void)
{
    if ( bconfig->resources.qam.total > 0 )
    {
        g_qam = btuner_open(bconfig->resources.qam.objects[0]);
        if ( NULL == g_qam )
        {
            fprintf(stderr, "Unable to open QAM tuner\n");
            return berr_external_error;
        }
    }
    if ( bconfig->resources.vsb.total > 0 )
    {
        /* QAM and VSB tuners are generally shared */
        if ( bconfig->resources.qam.total > 0 && 
             bconfig->resources.vsb.objects[0] == bconfig->resources.qam.objects[0] )
        {
            g_vsb = g_qam;
        }
        else
        {
            g_vsb = btuner_open(bconfig->resources.vsb.objects[0]);
            if ( NULL == g_vsb )
            {
                fprintf(stderr, "Unable to open VSB tuner\n");
                return berr_external_error;
            }
        }
    }
    if ( bconfig->resources.sds.total > 0 )
    {
        g_sds = btuner_open(bconfig->resources.sds.objects[0]);
        if ( NULL == g_sds )
        {
            fprintf(stderr, "Unable to open SDS tuner\n");
            return berr_external_error;
        }
    }
    if ( bconfig->resources.playback.total > 0 )
    {
        g_playback = bplayback_open();
        if ( NULL == g_playback )
        {
            fprintf(stderr, "Unable to open playback\n");
            return berr_external_error;
        }
        g_playpump = bplaypump_open(B_ID(0), NULL);
        if ( NULL == g_playpump )
        {
            fprintf(stderr, "Unable to open playpump\n");
            return berr_external_error;
        }
    }
    g_primary_decode = bdecode_open(B_ID(0));
    if ( NULL == g_primary_decode )
    {
        fprintf(stderr, "Unable to open primary decoder\n");
        return berr_external_error;
    }

    g_audio_decode = NULL;
    g_audio_decode_handle = bdecode_detach_audio(g_primary_decode);

    return b_ok;
}

static bresult Decode_Uninit(void)
{
    if ( g_qam )
    {
        btuner_close(g_qam);
    }
    if ( g_vsb && g_qam != g_vsb )
    {
        btuner_close(g_vsb);
    }
    if ( g_sds )
    {
        btuner_close(g_sds);
    }
    if ( g_playpump )
    {
        bplaypump_close(g_playpump);
        bplayback_close(g_playback);
    }
    bdecode_close(g_primary_decode);

    return b_ok;
}

static bplayback_loopmode Playback_EOS(void *param)
{
    /* Always loop */
    return bplayback_loopmode_loop;
}

static bresult Decode_Start(ProgramNode *pProgram)
{
    unsigned i;
    bplayback_params playback_params;
    baudio_decode_t audio;
    bresult rc;

    if ( NULL == pProgram )
    {
        return b_ok;
    }

    /* Mosaic Mode Window Setup */
    if ( pProgram->uiNumPrograms > 1 )
    {
        /* Open required number of mosaic decoders & windows at proper resolutions */
        for ( i = 0; i < pProgram->uiNumPrograms; i++ )
        {
            bdecode_window_settings window_settings;

            g_mosaic_decodes[i] = bdecode_open_mosaic(g_primary_decode, B_ID(i), &pProgram->stProgramInfo[i].stMosaicSettings);
            if ( NULL == g_mosaic_decodes[i] )
            {
                fprintf(stderr, "Unable to open mosaic decoder %d", i);
                return berr_external_error;
            }

            g_mosaic_windows[i] = bdecode_window_open_mosaic(g_primary_window, B_ID(i));
            rc = bdecode_window_get(g_mosaic_windows[i], &window_settings);
            if ( rc )
            {
                fprintf(stderr, "Unable to get window %d settings\n", i);
                return rc;
            }

            window_settings.position = pProgram->stProgramInfo[i].stPosition;
            window_settings.visible = true;
            rc = bdecode_window_set(g_mosaic_windows[i], &window_settings);
            if ( rc )
            {
                fprintf(stderr, "Unable to set window %d settings\n", i);
                return rc;
            }
        }
    }

    /* Open the band for tuner or streamer inputs */
    switch ( pProgram->eSourceType  )
    {
    case SOURCE_TYPE_eSTREAMER:
        g_band = bstreamer_attach(B_ID(0), pProgram->stSource.eMpegType);
        break;
    case SOURCE_TYPE_eQAM:
        g_band = btuner_tune_qam(g_qam, pProgram->uiFrequency, &pProgram->stSource.stQam);
        break;
    case SOURCE_TYPE_eVSB:
        g_band = btuner_tune_vsb(g_vsb, pProgram->uiFrequency, &pProgram->stSource.stVsb);
        break;
    case SOURCE_TYPE_eSDS:
        g_band = btuner_tune_sds(g_vsb, pProgram->uiFrequency, &pProgram->stSource.stSds);
        break;
    case SOURCE_TYPE_eFILE:
        g_band = NULL;
        break;
    default:
        fprintf(stderr, "Invalid source type\n");
        return berr_external_error;
    }

    /* Open the primary stream */
    switch ( pProgram->eSourceType )
    {
    case SOURCE_TYPE_eSTREAMER:
    case SOURCE_TYPE_eQAM:
    case SOURCE_TYPE_eVSB:
    case SOURCE_TYPE_eSDS:
        if ( NULL == g_band )
        {
            fprintf(stderr, "Unable to open tuner input\n");
            return berr_external_error;
        }
        g_mosaic_streams[0] = bstream_open(g_band, &pProgram->stProgramInfo[0].stPids);
        break;
    case SOURCE_TYPE_eFILE:
        bplayback_params_init(&playback_params, g_playback);
        playback_params.callback_context = g_playback;
        playback_params.end_of_stream = Playback_EOS;
        g_mosaic_streams[0] = bplayback_start(g_playback, 
                                              g_playpump, 
                                              &pProgram->stProgramInfo[0].stPids,
                                              pProgram->stSource.hFile,
                                              &playback_params);
        break;
    default:
        fprintf(stderr, "Invalid source type\n");
        return berr_external_error;
    }

    if ( NULL == g_mosaic_streams[0] )
    {
        fprintf(stderr, "Unable to open primary stream\n");
        return berr_external_error;
    }

    /* Open children */
    for ( i = 1; i < pProgram->uiNumPrograms; i++ )
    {
        /* Open the primary stream first */
        g_mosaic_streams[i] = bstream_open_child(g_mosaic_streams[0], &pProgram->stProgramInfo[i].stPids);
        if ( NULL == g_mosaic_streams[i] )
        {
            fprintf(stderr, "Unable to open stream %d\n", i);
            return berr_external_error;
        }
    }

    /* Always detach audio.  App must restart it via Decode_SetAudio. */
    if ( g_audio_decode )
    {
        printf("Detaching audio from audio decode %p\n", g_audio_decode);
        g_audio_decode_handle = bdecode_detach_audio(g_audio_decode);
        g_audio_decode = NULL;
    }

    if ( pProgram->uiNumPrograms > 1 )
    {
        /* Now, start decode on all streams */
        for ( i = 0; i < pProgram->uiNumPrograms; i++ )
        {
            bresult rc;

            printf("Starting Decode %d\n", i);
            fflush(stdout);
            rc = bdecode_start(g_mosaic_decodes[i], g_mosaic_streams[i], g_mosaic_windows[i]);
            if ( rc )
            {
                fprintf(stderr, "Unable to start decode %d\n", i);
                return rc;
            }
        }
    }
    else
    {
        /* Single Program Case */
        rc = bdecode_start(g_primary_decode, g_mosaic_streams[0], g_primary_window);
    }

    /* All done */
    return b_ok;
}

static bresult Decode_SetAudio(ProgramNode *pProgram, unsigned uiProgramNum)
{
    bdecode_t dest_decoder=NULL;
    bresult rc;

    assert(pProgram);
    assert(uiProgramNum < pProgram->uiNumPrograms);

    /* Determine destination decode object */
    if ( pProgram->uiNumPrograms == 1 )
    {
        dest_decoder = g_primary_decode;
    }
    else
    {
        dest_decoder = g_mosaic_decodes[uiProgramNum];
    }

    /* Detach from current decoder if attached */
    if ( g_audio_decode )
    {
        g_audio_decode_handle = bdecode_detach_audio(g_audio_decode);
    }

    /* Attach to new decoder */
    rc = bdecode_attach_audio(dest_decoder, g_audio_decode_handle);
    if ( rc )
    {
        fprintf(stderr, "Unable to start audio decoder\n");
        return rc;
    }
    /* Success, setup handles */
    g_audio_decode = dest_decoder;
    g_audio_decode_handle = NULL;
    return rc;
}

static bresult Decode_Stop(ProgramNode *pProgram)
{
    int i;
    baudio_decode_t audio;
    bresult rc;

    if ( NULL == pProgram )
    {
        return b_ok;
    }

    if ( pProgram->uiNumPrograms > 1 )
    {
        /* Move audio back to parent */
        if ( g_audio_decode && g_audio_decode != g_primary_decode )
        {
            audio = bdecode_detach_audio(g_audio_decode);
            if ( audio )
            {
                bdecode_attach_audio(g_primary_decode, audio);
            }
            g_audio_decode = g_primary_decode;
        }

        /* Close decoders & windows */
        for ( i = pProgram->uiNumPrograms-1; i >= 0; i-- )
        {
            if ( g_mosaic_decodes[i] )
            {
                bdecode_stop(g_mosaic_decodes[i]);
            }
            if ( g_mosaic_windows[i] )
            {
                bdecode_window_close(g_mosaic_windows[i]);
                g_mosaic_windows[i] = NULL;
            }
        }
    }
    else
    {
        /* Single Program */
        bdecode_stop(g_primary_decode);
    }

    /* Close child streams */
    for ( i = pProgram->uiNumPrograms-1; i > 0; i-- )
    {
        if ( g_mosaic_streams[i] )
        {
            bstream_close(g_mosaic_streams[i]);
            g_mosaic_streams[i] = NULL;
        }
    }

    if ( g_mosaic_streams[0] )
    {
        /* Stop primary stream */
        switch ( pProgram->eSourceType )
        {
        default:
            /* Tuner/Streamer */
                bstream_close(g_mosaic_streams[0]);
            break;
        case SOURCE_TYPE_eFILE:
                bplayback_stop(g_playback);
            break;
        }
        g_mosaic_streams[0] = NULL;
    }

    if ( pProgram->uiNumPrograms > 1 )
    {
        /* Close decoders */
        for ( i = pProgram->uiNumPrograms-1; i >= 0; i-- )
        {
            if ( g_mosaic_decodes[i] )
            {
                bdecode_close(g_mosaic_decodes[i]);
                g_mosaic_decodes[i] = NULL;
            }
        }
    }

    /* Band needs no cleanup */
    g_band = NULL;

    return b_ok;
}

static bresult LED_Init(void)
{
    g_led = buser_output_open(B_ID(0));
    if ( NULL == g_led )
    {
        fprintf(stderr, "Unable to open LED\n");
        return berr_external_error;
    }
    buser_output_set_led(g_led, 0, 1); /* POWER ON */
    buser_output_set_led(g_led, 1, 0); /* REC OFF */
    buser_output_set_led(g_led, 2, 0); /* PLAY OFF */
    buser_output_set_led(g_led, 3, 0); /* MSG OFF */
    buser_output_display_message(g_led, ""); /* Clear text */
    return b_ok;
}

static bresult LED_Uninit(void)
{   
    buser_output_set_led(g_led, 0, 1); /* POWER OFF */
    buser_output_close(g_led);
    return b_ok;
}

#define LED_SIZE 4
static int g_iLEDMsgSize;
static char g_szLEDMsg[256];
static int g_iLEDMsgPosition;

static bresult LED_Write(const char *pszMsg)
{
    g_iLEDMsgSize = strlen(pszMsg);
    if ( g_iLEDMsgSize > 255 )
    {
        g_iLEDMsgSize = 255;
    }
    strncpy(g_szLEDMsg, pszMsg, g_iLEDMsgSize);
    g_szLEDMsg[g_iLEDMsgSize] = '\0';
    g_iLEDMsgPosition = 0;
    /* This will display the first n characters of the stream */
    return buser_output_display_message(g_led, &g_szLEDMsg[g_iLEDMsgPosition]);
}

static bresult LED_Update(void)
{
    /* This function will advance the display by one character if longer than the size */
    if ( g_iLEDMsgSize > LED_SIZE )
    {
        g_iLEDMsgPosition++;
        /* Reset after display is blank */
        if ( g_iLEDMsgPosition > g_iLEDMsgSize )
        {
            g_iLEDMsgPosition = 0;
        }
        return buser_output_display_message(g_led, &g_szLEDMsg[g_iLEDMsgPosition]);
    }
    return b_ok;
}

static void Input_Callback(void *context)
{
    (void)BKNI_SetEvent((BKNI_EventHandle)context);
}

static void Event_Print(EventType eEvent)
{
    if ( eEvent < EVENT_TYPE_eTIMER )
    {
        printf("%s\n", g_pszEventNames[eEvent]);
    }
}

static void Event_Handle(EventType eEvent)
{
    if ( g_pEventHandler )
    {
        g_pEventHandler(eEvent);
    }
    else
    {
        Event_Print(eEvent);
    }
}

static void IR_Dispatch(uint32_t code)
{
    EventType eEvent = EVENT_TYPE_eNONE;

    if ( code & 0x80000000 )
    {
        /* Key Repeat.  Drop. */
        return;
    }

    switch ( code & 0xffff )
    {
    case 0x600a: /* POWER */
        eEvent = EVENT_TYPE_ePOWER;
        break;
    case 0x7036: /* LEFT */
        eEvent = EVENT_TYPE_eLEFT;
        break;
    case 0x6037: /* RIGHT */
        eEvent = EVENT_TYPE_eRIGHT;
        break;
    case 0x9034: /* UP */
        eEvent = EVENT_TYPE_eUP;
        break;
    case 0x8035: /* DOWN */
        eEvent = EVENT_TYPE_eDOWN;
        break;
    case 0xe011: /* SELECT */
        eEvent = EVENT_TYPE_eSELECT;
        break;
    case 0x500b: /* CHANUP */
        eEvent = EVENT_TYPE_eCHAN_UP;
        break;
    case 0x400c: /* CHANDN */
        eEvent = EVENT_TYPE_eCHAN_DOWN;
        break;
    case 0x300d: /* VOLUP */
        eEvent = EVENT_TYPE_eVOL_UP;
        break;
    case 0x200e: /* VOLDN */
        eEvent = EVENT_TYPE_eVOL_DOWN;
        break;
    case 0x6019: /* MENU */
        eEvent = EVENT_TYPE_eMENU;
        break;
    case 0xd030: /* GUIDE */
        eEvent = EVENT_TYPE_eGUIDE;
        break;  
    case 0xa033: /* INFO */
        eEvent = EVENT_TYPE_eINFO;
        break;
    case 0xd012: /* EXIT */
        eEvent = EVENT_TYPE_eEXIT;
        break;  
    case 0x100f: /* MUTE */
        eEvent = EVENT_TYPE_eMUTE;
        break;
    case 0x0000: /* 0 */
        eEvent = EVENT_TYPE_e0;
        break;
    case 0xf001: /* 1 */
        eEvent = EVENT_TYPE_e1;
        break;
    case 0xe002: /* 2 */
        eEvent = EVENT_TYPE_e2;
        break;
    case 0xd003: /* 3 */
        eEvent = EVENT_TYPE_e3;
        break;
    case 0xc004: /* 4 */
        eEvent = EVENT_TYPE_e4;
        break;
    case 0xb005: /* 5 */
        eEvent = EVENT_TYPE_e5;
        break;
    case 0xa006: /* 6 */
        eEvent = EVENT_TYPE_e6;
        break;
    case 0x9007: /* 7 */
        eEvent = EVENT_TYPE_e7;
        break;
    case 0x8008: /* 8 */
        eEvent = EVENT_TYPE_e8;
        break;
    case 0x7009: /* 9 */
        eEvent = EVENT_TYPE_e9;
        break;
    default:
        fprintf(stderr, "Unhandled IR code %#x\n", code);
        return;
    }

    Event_Handle(eEvent);
}

static void KPD_Dispatch(uint32_t code)
{
    EventType eEvent=EVENT_TYPE_eNONE;

    if ( code & 0x80000000 )
    {
        /* Key Repeat.  Drop. */
        return;
    }

    switch ( code )
    {
    case 0xfffe: /* POWER */
        eEvent = EVENT_TYPE_ePOWER;
        break;
    case 0xffef: /* LEFT */
        eEvent = EVENT_TYPE_eLEFT;
        break;
    case 0xffdf: /* RIGHT */
        eEvent = EVENT_TYPE_eRIGHT;
        break;
    case 0xfeff: /* UP */
        eEvent = EVENT_TYPE_eUP;
        break;
    case 0xfffd: /* DOWN */
        eEvent = EVENT_TYPE_eDOWN;
        break;
    case 0xefff: /* SELECT */
        eEvent = EVENT_TYPE_eSELECT;
        break;
    case 0xfffb: /* CHANUP */
        eEvent = EVENT_TYPE_eCHAN_UP;
        break;
    case 0xffbf: /* CHANDN */
        eEvent = EVENT_TYPE_eCHAN_DOWN;
        break;
    case 0xfbff: /* VOLUP */
        eEvent = EVENT_TYPE_eVOL_UP;
        break;
    case 0xbfff: /* VOLDN */
        eEvent = EVENT_TYPE_eVOL_DOWN;
        break;
    case 0xdfff: /* MENU */
        eEvent = EVENT_TYPE_eMENU;
        break;
    case 0xfdff: /* GUIDE */
        eEvent = EVENT_TYPE_eGUIDE;
        break;
    default:
        fprintf(stderr, "Unhandled KPD code %#x\n", code);
        return;
    }

    Event_Handle(eEvent);
}

static void App_Redraw(void)
{
    bsettop_rect stRect;
    ProgramNode *pProgram = g_pCurrentProgram;

    stRect.x = stRect.y = 0;
    stRect.height = FRAMEBUFFER_HEIGHT;
    stRect.width = FRAMEBUFFER_WIDTH;

#if 0
    #define PRINT_RECT(r) printf("Rect %dx%d %d,%d\n", r.width, r.height, r.x, r.y)
#else
    #define PRINT_RECT(r) (void)0
#endif

    if ( NULL == pProgram )
    {
        PRINT_RECT(stRect);
        bsurface_fill(g_framebuffer, &stRect, BACKGROUND_COLOR);
        bgraphics_flip(g_graphics);
        return;
    }

    if ( pProgram->uiNumPrograms == 1 )
    {
        /* Single program, go transparent */
        PRINT_RECT(stRect);
        bsurface_fill(g_framebuffer, &stRect, HOLE_COLOR);
    }
    else
    {
        int i;
        /* Mosaic, draw holes */
        /* Start by filling the screen with the background */
        PRINT_RECT(stRect);
        bsurface_fill(g_framebuffer, &stRect, BACKGROUND_COLOR);

        for ( i = 0; i < (int)pProgram->uiNumPrograms; i++ )
        {
            /* Blast a hole for each program */
            PRINT_RECT(pProgram->stProgramInfo[i].stPosition);
            bsurface_fill(g_framebuffer, &pProgram->stProgramInfo[i].stPosition, HOLE_COLOR);
        }
    }

    /* Update current program index, if we are out of bounds snap to last one */
    if ( g_iProgramNum >= (int)pProgram->uiNumPrograms )
    {
        g_iProgramNum = pProgram->uiNumPrograms-1;
    }

    if ( pProgram->uiNumPrograms > 1 )
    {
        /* Draw focus only for mosaics */
        int iProgramNum = g_iProgramNum;
        assert(iProgramNum < (int)pProgram->uiNumPrograms);

        /* Draw Left and Right Sides First */
        stRect.height = pProgram->stProgramInfo[iProgramNum].stPosition.height+(SELECTION_BOX_WIDTH*2);
        stRect.width = SELECTION_BOX_WIDTH;
        stRect.y = pProgram->stProgramInfo[iProgramNum].stPosition.y - SELECTION_BOX_WIDTH;
        stRect.x = pProgram->stProgramInfo[iProgramNum].stPosition.x - SELECTION_BOX_WIDTH;

        /* Left */
        PRINT_RECT(stRect);
        bsurface_fill(g_framebuffer, &stRect, SELECTION_BOX_COLOR);

        stRect.x = pProgram->stProgramInfo[iProgramNum].stPosition.x + 
            pProgram->stProgramInfo[iProgramNum].stPosition.width;

        /* Right */
        PRINT_RECT(stRect);
        bsurface_fill(g_framebuffer, &stRect, SELECTION_BOX_COLOR);

        /* Draw top and bottom */
        stRect.height = SELECTION_BOX_WIDTH;
        stRect.width = pProgram->stProgramInfo[iProgramNum].stPosition.width;
        stRect.x = pProgram->stProgramInfo[iProgramNum].stPosition.x;
        /* Y is the same for top */
        PRINT_RECT(stRect);
        bsurface_fill(g_framebuffer, &stRect, SELECTION_BOX_COLOR);

        stRect.y = pProgram->stProgramInfo[iProgramNum].stPosition.y + 
            pProgram->stProgramInfo[iProgramNum].stPosition.height;

        /* Bottom */
        PRINT_RECT(stRect);
        bsurface_fill(g_framebuffer, &stRect, SELECTION_BOX_COLOR);

    }
    /* Done, flip. */
    bgraphics_flip(g_graphics);
}

static void App_Tune(ProgramNode *pNextProgram)
{
    if ( pNextProgram && pNextProgram != g_pCurrentProgram )
    {
        if ( g_pCurrentProgram )
            Decode_Stop(g_pCurrentProgram);

        g_pCurrentProgram = pNextProgram;
        App_Redraw();
        /*LED_Write(pNextProgram->szChannelName);*/
        Decode_Start(pNextProgram);
        Decode_SetAudio(pNextProgram, g_iProgramNum);
    }
}

static void App_Event(EventType eEvent)
{
    ProgramNode *pNextProgram;
    static int iPowerCount=0;

    if ( eEvent == EVENT_TYPE_ePOWER )
    {
        iPowerCount++;
        if ( iPowerCount == 2 )
        {
            /* Two consecutive power key presses will exit */
            g_bRunning = false;
            return;
        }
        else
            printf("POWER Key Pressed -- Press again to exit\n");
    }
    else
        iPowerCount = 0;

    switch ( eEvent )
    {
        /* TODO: Need to update these to handle odd cases like 3, 5, 7, 8, etc. Do it based on program number */
    case EVENT_TYPE_eLEFT:
        if ( g_iProgramNum > 0 )
        {
            g_iProgramNum--;
            App_Redraw();
        }
        break;
    case EVENT_TYPE_eRIGHT:
        if ( g_iProgramNum + 1 < (int)g_pCurrentProgram->uiNumPrograms )
        {
            g_iProgramNum++;
            App_Redraw();
        }
        break;
    case EVENT_TYPE_eUP:
        if ( g_iProgramNum >= g_pCurrentProgram->iCols )
        {
            g_iProgramNum -= g_pCurrentProgram->iCols;
            App_Redraw();
        }
        break;
    case EVENT_TYPE_eDOWN:
        if ( (g_iProgramNum + g_pCurrentProgram->iCols) < (int)g_pCurrentProgram->uiNumPrograms )
        {
            g_iProgramNum += g_pCurrentProgram->iCols;
            App_Redraw();
        }
        break;
    case EVENT_TYPE_eCHAN_UP:
        pNextProgram = BLST_Q_NEXT(g_pCurrentProgram, stNode);
        if ( NULL == pNextProgram )
        {
            pNextProgram = BLST_Q_FIRST(&g_stProgramList);
        }
        App_Tune(pNextProgram);
        break;
    case EVENT_TYPE_eCHAN_DOWN:
        pNextProgram = BLST_Q_PREV(g_pCurrentProgram, stNode);
        if ( NULL == pNextProgram )
        {
            pNextProgram = BLST_Q_LAST(&g_stProgramList);
        }
        App_Tune(pNextProgram);
        break;
    case EVENT_TYPE_eSELECT:
        Decode_SetAudio(g_pCurrentProgram, g_iProgramNum);
        break;
    case EVENT_TYPE_eTIMER:
        LED_Update();
        break;
    default:
        break;
    }
}

static void Main_Loop()
{
    buser_input_t hInputIr, hInputKpd;
    buser_input_settings stInputSettings;
    BKNI_EventHandle hEvent;
    BERR_Code eMagnumErr;
    bresult eResult;

    hInputIr = buser_input_open(B_ID(0));
    if ( NULL == hInputIr )
    {
        fprintf(stderr, "Unable to open IR input device\n");
        return;
    }

    hInputKpd = buser_input_open(B_ID(3));
    if ( NULL == hInputKpd )
    {
        fprintf(stderr, "Unable to open KPD input device\n");
        return;
    }

    eMagnumErr = BKNI_CreateEvent(&hEvent);
    if ( eMagnumErr != BERR_SUCCESS )
    {
        fprintf(stderr, "Unable to create event\n");
        return;
    }

    stInputSettings.callback_context = hEvent;
    stInputSettings.data_ready_callback = Input_Callback;
    buser_input_set_settings(hInputIr, &stInputSettings);
    buser_input_set_settings(hInputKpd, &stInputSettings);

    /* Initial Msg on display */
    LED_Write("7400");

    /* Setup App Callback */
    g_pEventHandler = App_Event;

    /* Tune to first program */
    App_Tune(BLST_Q_FIRST(&g_stProgramList));

    while ( g_bRunning )
    {
        unsigned uiNumEvents;
        buser_input_event stInputEvent;

        eMagnumErr = BKNI_WaitForEvent(hEvent, TIMER_INTERVAL);

        switch ( eMagnumErr )
        {
        case BERR_SUCCESS:
            break;
        case BERR_TIMEOUT:
            Event_Handle(EVENT_TYPE_eTIMER);
            continue;
        default:
            fprintf(stderr, "Event Error\n");
            return;
        }

        /* Check IR first */
        for ( ;; )
        {
            eResult = buser_input_get_event(hInputIr, &stInputEvent, 1, &uiNumEvents);
            if ( eResult == b_ok && uiNumEvents > 0 )
                IR_Dispatch(stInputEvent.code);
            else
                break;
        }
        for ( ;; )
        {
            eResult = buser_input_get_event(hInputKpd, &stInputEvent, 1, &uiNumEvents);
            if ( eResult == b_ok && uiNumEvents > 0 )
                KPD_Dispatch(stInputEvent.code);
            else
                break;
        }
    }
    if ( g_pCurrentProgram )
        Decode_Stop(g_pCurrentProgram);
}

int main(int argc, char **argv)
{
    bresult rc;

    #if 1
    fprintf(stderr, "Forcing sync_disabled=1\n");
    putenv("sync_disabled=1");
    fprintf(stderr, "Forcing vsync mode\n");
    putenv("force_vsync=y");
    #endif

    rc = bsettop_init(BSETTOP_VERSION);
    if ( rc ) 
    {
        fprintf(stderr, "Unable to init settop API\n");
        return -1;
    }

    rc = Display_Init();
    if ( rc )
    {
        fprintf(stderr, "Unable to init display\n");
        return -1;
    }   

    rc = Window_Init();
    if ( rc )
    {
        fprintf(stderr, "Unable to init windows\n");
        return -1;      
    }

    rc = Decode_Init();
    if ( rc )
    {
        fprintf(stderr, "Unable to init decoders\n");
        return -1;
    }

    rc = ProgramList_Parse();
    if ( rc )
    {
        fprintf(stderr, "Unable to parse program list\n");
        return -1;
    }

    rc = LED_Init();
    if ( rc )
    {
        fprintf(stderr, "Unable to init LED\n");
        return -1;
    }

    Main_Loop();

    rc = LED_Uninit();
    if ( rc )
    {
        fprintf(stderr, "Unable to uninit LED\n");
        return -1;
    }

    rc = Decode_Uninit();
    if ( rc )
    {
        fprintf(stderr, "Unable to close decoders\n");
        return -1;
    }

    rc = Window_Uninit();
    if ( rc )
    {
        fprintf(stderr, "Unable to close windows\n");
        return -1;
    }

    rc = Display_Uninit();
    if ( rc )
    {
        fprintf(stderr, "Unable to close display\n");
        return -1;
    }

    bsettop_uninit();

    printf("Program Exited Successfully\n");

    return 0;
}


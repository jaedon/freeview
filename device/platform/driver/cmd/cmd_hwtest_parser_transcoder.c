#if defined(CONFIG_VK_STDLIB)
#else
#include 	<stdio.h>
#include 	<stdlib.h>
#include 	<string.h>
#endif

#include <sys/mount.h>
#include <unistd.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <dirent.h>
#include <fcntl.h>

#include "taskdef.h"

#include "di_demux.h"
#include "di_pvr.h"
#include "di_audio.h"
#include "di_video.h"
#include "di_transcoder.h"
#include "bstd_defs.h"
#include "vkernel.h"
#include "htype.h"
#include "taskdef.h"
#include "cmdtool.h"

#include "cmd_hwtest_parser_transcoder.h"

/*******************************************************************************/
#define HWTEST_CMD		szCmd
#define HWTEST_PARAM	szParam
#define HWTEST_PARAM1	szParam1
#define HWTEST_PARAM2	szParam2
#define GET_ARGS		int iResult=CMD_ERR; \
						char *HWTEST_CMD=CMD_NextArg((char **)&szArgs); \
						char *HWTEST_PARAM=CMD_NextArg((char **)&szArgs); \
						char *HWTEST_PARAM1=CMD_NextArg((char **)&szArgs); \
						char *HWTEST_PARAM2=CMD_NextArg((char **)&szArgs);
#define CMD_IS(x)		(HWTEST_CMD!=NULL && VK_strcmp(HWTEST_CMD, x)==0)
#define PARAM_IS(x)		(HWTEST_PARAM!=NULL && VK_strcmp(HWTEST_PARAM, x)==0)
#define PARAM1_IS(x)	(HWTEST_PARAM1!=NULL && VK_strcmp(HWTEST_PARAM1, x)==0)
#define PARAM2_IS(x)	(HWTEST_PARAM2!=NULL && VK_strcmp(HWTEST_PARAM2, x)==0)
#define READABLE_IN_HEX(sz,val) CMD_ReadAsHex(sz, &val)
#define READABLE_IN_DEC(sz,val) CMD_ReadAsDec(sz, &val)

#define GET_NEXT_ARG(variable)		char *variable = CMD_NextArg((char **)&szArgs)

#define AssertDI(arg1, arg2) if(arg1 != DI_ERR_OK) {CMD_Printf(\
					"ERROR code : 0x%x, At %s() function, FILE: %s, LINE: %d\n",arg1, __func__, __FILE__,__LINE__); return arg2;}

//#define CMD_TRANSCODER_DEBUG
/*******************************************************************************/

static HUINT8 s_aucEncryptionKey[DI_PVR_CRYPTO_KEY_LEN] = {
0x0, 0x10, 0x32, 0x54, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0};

static HUINT8 s_aucEncryptionIV[DI_PVR_CRYPTO_KEY_LEN] = {
0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0};

static HUINT8 s_aucEncryptionProtectionKey[DI_PVR_CRYPTO_KEY_LEN] = {
0x34, 0x29, 0x83, 0x1, 0x28, 0x55, 0x92, 0x2, 0x48, 0x47, 0x55, 0x2, 0x1, 0x0, 0x0, 0x0};

static unsigned char patsectiondata[] =
{
	0x00,0xB0,0x2D,0x04,0x47,0xD3,0x00,0x00,0x00,0x00,0xE0,0x10,0x00,0xE1,0xE4,0x4C,
	0x4F,0x6A,0xE4,0x00,0x4F,0x6B,0xE4,0x01,0x4F,0x6C,0xE4,0x02,0x4F,0x6D,0xE4,0x03,
	0x4F,0x6E,0xE4,0x04,0x4F,0x6F,0xE4,0x05,0x4F,0x70,0xE4,0x06,0x5F,0xB7,0x49,0x44
};

static unsigned char pmtsectiondata[] =
{
	0x02,0xB0,0x29,0x4F,0x6C,0xF4,0x00,0x00,0xE0,0xA2,0xF0,0x06,0x09,0x04,0x0D,0x02,
	0xE3,0xC6,0x02,0xE0,0xA2,0xF0,0x03,0x52,0x01,0x01,0x03,0xE0,0x58,0xF0,0x09,0x0A,
	0x04,0x70,0x6F,0x6C,0x01,0x52,0x01,0x02,0x0F,0x3D,0x5B,0xD4
};

static unsigned char eitsectiondata[] =
{
	0x4F,0xF0,0xEA,0x4F,0x6B,0xDD,0x00,0x01,0x04,0x47,0x00,0x01,0x01,0x4E,0x66,
	0x7B,0xC7,0xFF,0x19,0x00,0x00,0x01,0x35,0x00,0x90,0xCF,0x0A,0x14,0x63,0x7A,
	0x65,0x00,0x65,0x6E,0x67,0x00,0x68,0x75,0x6E,0x00,0x70,0x6F,0x6C,0x00,0x72,
	0x75,0x6D,0x00,0x4D,0x5A,0x70,0x6F,0x6C,0x13,0x52,0x61,0x73,0x63,0x61,0x6C,
	0x73,0x20,0x61,0x6E,0x64,0x20,0x52,0x6F,0x62,0x63,0x65,0x72,0x73,0x42,0x46,
	0x69,0x6C,0x6D,0x20,0x6E,0x61,0x20,0x70,0x6F,0x64,0x73,0x74,0x61,0x77,0x69,
	0x65,0x20,0x70,0x6F,0x77,0x69,0x65,0xC2,0x73,0x63,0x69,0x20,0x4D,0x61,0x72,
	0x6B,0x61,0x20,0x54,0x77,0x61,0x69,0x6E,0x61,0x20,0x6F,0x20,0x70,0x72,0x7A,
	0x79,0x67,0x6F,0x64,0x61,0x63,0x68,0x20,0x54,0x6F,0x6D,0x61,0x20,0x69,0x20,
	0x48,0x75,0x63,0x6B,0x61,0x50,0x0D,0xF1,0x01,0x01,0x20,0x20,0x20,0x56,0x69,
	0x64,0x65,0x6F,0x20,0x31,0x50,0x0D,0xF2,0x01,0x02,0x70,0x6F,0x6C,0x41,0x75,
	0x64,0x69,0x6F,0x20,0x32,0x50,0x0D,0xF2,0x01,0x03,0x68,0x75,0x6E,0x41,0x75,
	0x64,0x69,0x6F,0x20,0x33,0x50,0x0D,0xF2,0x01,0x04,0x65,0x6E,0x67,0x41,0x75,
	0x64,0x69,0x6F,0x20,0x34,0x50,0x0D,0xF2,0x01,0x05,0x63,0x7A,0x65,0x41,0x75,
	0x64,0x69,0x6F,0x20,0x35,0x50,0x10,0xF3,0x01,0x06,0x72,0x75,0x6D,0x54,0x65,
	0x6C,0x65,0x74,0x65,0x78,0x74,0x20,0x36,0xFB,0xED,0x8B,0x14
};


int CMD_HwTest_Parser_Transcoder(void *szArgs)
{
    GET_ARGS;

    if CMD_IS("start")
    {
        HUINT32 unDeviceId;
        HUINT32 unTunerId;
        HUINT32 unDemuxId;

        HUINT32 SourceType;
        HUINT32 EncodeMode;
        HUINT32 unVideoPid_Src;
        HUINT32 unAudioPid_Src;
        HUINT32 unPcrPid_Src;
        HUINT32 unVideoCodec_Src;
        HUINT32 unAudioCodec_Src;
        HUINT32 unCasId_Src;

        HUINT32 unDest;
        HUINT32 unContainer_Dest;
        HUINT32 unVideoCodec_Dest;
        HUINT32 unVideoResolution_Dest;
        HUINT32 unVideoFramerate_Dest;
        HUINT32 unVideoAspectRatio_Dest;
        HUINT32 unAudioCodec_Dest;

        HUINT32 unEncType;

        HUINT32 unVideoDefaultProfile;
        HUINT32 unNonPcrInsert;

        HUINT32 ulMuxId = 0;
        HUINT32 ulHDDId =0x01;
        HUINT32 ulNumOfDevice;
        DI_TRANSCODER_CAP_t ppCapInfo[2];
        DI_TRANSCODER_Settings_t s_stSetting;
        DI_TRANSCODER_MUX_INFO s_stMuxInfo;
        HUINT16 patPid = 0, pmtPid = 0,psiPid = 0;
        HUINT16 psicontinuos = 0;
        DI_ERR_CODE diErr = 0;
        struct dirent **items;
        int nitems, i;
        char* next;
        HUINT32 unVideoCodec;
        HUINT32 unAudioCodec;
        HUINT32 nCnt = 0;
        HUINT8 aBuf[255],aInfo[255];
        int   fd;
        DI_PVR_REC_SETUP_t s_stRecSetup;

		char* szParm3=CMD_NextArg((char **)&szArgs);
		char* szParm4=CMD_NextArg((char **)&szArgs);
		char* szParm5=CMD_NextArg((char **)&szArgs);
		char* szParm6=CMD_NextArg((char **)&szArgs);
		char* szParm7=CMD_NextArg((char **)&szArgs);
		char* szParm8=CMD_NextArg((char **)&szArgs);
		char* szParm9=CMD_NextArg((char **)&szArgs);
		char* szParm10=CMD_NextArg((char **)&szArgs);
        char* szParm11=CMD_NextArg((char **)&szArgs);
        char* szParm12=CMD_NextArg((char **)&szArgs);
        char* szParm13=CMD_NextArg((char **)&szArgs);
        char* szParm14=CMD_NextArg((char **)&szArgs);
        char* szParm15=CMD_NextArg((char **)&szArgs);
        char* szParm16=CMD_NextArg((char **)&szArgs);
        char* szParm17=CMD_NextArg((char **)&szArgs);

        READABLE_IN_DEC(HWTEST_PARAM, unDeviceId);
        READABLE_IN_DEC(HWTEST_PARAM1, unTunerId);
        READABLE_IN_DEC(HWTEST_PARAM2, unDemuxId);
        READABLE_IN_DEC(szParm3, SourceType);
        READABLE_IN_DEC(szParm4, unVideoPid_Src);
        READABLE_IN_DEC(szParm5, unAudioPid_Src);
        READABLE_IN_DEC(szParm6, unPcrPid_Src);
        READABLE_IN_DEC(szParm7, unVideoCodec_Src);
        READABLE_IN_DEC(szParm8, unAudioCodec_Src);
        READABLE_IN_DEC(szParm9, unCasId_Src);
        READABLE_IN_DEC(szParm10, unDest);
        READABLE_IN_DEC(szParm11, unContainer_Dest);
        READABLE_IN_DEC(szParm12, unVideoCodec_Dest);
        READABLE_IN_DEC(szParm13, unVideoResolution_Dest);
        READABLE_IN_DEC(szParm14, unVideoFramerate_Dest);
        READABLE_IN_DEC(szParm15, unVideoAspectRatio_Dest);
        READABLE_IN_DEC(szParm16, unAudioCodec_Dest);
        READABLE_IN_DEC(szParm17, unEncType);

        /*
                        Last Test Status
                        Below is only OK.
                        Video : MPEG2 -> H.264
                        Audio : MPEG -> AAC
        */
        EncodeMode = DI_TRANSCODER_ENCODE_AV;
        unNonPcrInsert = 0;
        unVideoDefaultProfile = 1;

        #ifdef CMD_TRANSCODER_DEBUG
        VK_printf("%s(%d) : unDeviceId = 0x%X\n", __FUNCTION__, __LINE__, unDeviceId);
        VK_printf("%s(%d) : unTunerId = 0x%X\n", __FUNCTION__, __LINE__, unTunerId);
        VK_printf("%s(%d) : unDemuxId = 0x%X\n", __FUNCTION__, __LINE__, unDemuxId);
        VK_printf("%s(%d) : SourceType = 0x%X\n", __FUNCTION__, __LINE__, SourceType);
        VK_printf("%s(%d) : EncodeMode = 0x%X\n", __FUNCTION__, __LINE__, EncodeMode);
        VK_printf("%s(%d) : unVideoPid_Src = 0x%X\n", __FUNCTION__, __LINE__, unVideoPid_Src);
        VK_printf("%s(%d) : unAudioPid_Src = 0x%X\n", __FUNCTION__, __LINE__, unAudioPid_Src);
        VK_printf("%s(%d) : unPcrPid_Src = 0x%X\n", __FUNCTION__, __LINE__, unPcrPid_Src);
        VK_printf("%s(%d) : unVideoCodec_Src = 0x%X\n", __FUNCTION__, __LINE__, unVideoCodec_Src);
        VK_printf("%s(%d) : unAudioCodec_Src = 0x%X\n", __FUNCTION__, __LINE__, unAudioCodec_Src);
        VK_printf("%s(%d) : unCasId_Src = 0x%X\n", __FUNCTION__, __LINE__, unCasId_Src);
        VK_printf("%s(%d) : unDest = 0x%X\n", __FUNCTION__, __LINE__, unDest);
        VK_printf("%s(%d) : unContainer_Dest = 0x%X\n", __FUNCTION__, __LINE__, unContainer_Dest);
        VK_printf("%s(%d) : unVideoCodec_Dest = 0x%X\n", __FUNCTION__, __LINE__, unVideoCodec_Dest);
        VK_printf("%s(%d) : unVideoResolution_Dest = 0x%X\n", __FUNCTION__, __LINE__, unVideoResolution_Dest);
        VK_printf("%s(%d) : unVideoFramerate_Dest = 0x%X\n", __FUNCTION__, __LINE__, unVideoFramerate_Dest);
        VK_printf("%s(%d) : unVideoAspectRatio_Dest = 0x%X\n", __FUNCTION__, __LINE__, unVideoAspectRatio_Dest);
        VK_printf("%s(%d) : unAudioCodec_Dest = 0x%X\n", __FUNCTION__, __LINE__, unAudioCodec_Dest);
        VK_printf("%s(%d) : unEncType = 0x%X\n", __FUNCTION__, __LINE__, unEncType);
        VK_printf("%s(%d) : unNonPcrInsert = 0x%X\n", __FUNCTION__, __LINE__, unNonPcrInsert);
        VK_printf("%s(%d) : unVideoDefaultProfile = 0x%X\n", __FUNCTION__, __LINE__, unVideoDefaultProfile);
        #endif

        if(unDeviceId == 0x00 || unDeviceId == 0x01)
        {
           ulMuxId = unDeviceId;
        }
        else
        {
            CMD_Printf("%s(%d) : not support error\n",__func__,__LINE__);
            iResult = CMD_ERR;
            goto __GOTO_RETURN;
        }

        VK_memset(&s_stSetting, 0, sizeof(DI_TRANSCODER_Settings_t));
        VK_memset(&s_stMuxInfo, 0, sizeof(DI_TRANSCODER_MUX_INFO));
        VK_memset(&s_stRecSetup, 0, sizeof(DI_PVR_REC_SETUP_t));

        if(SourceType == DI_TRANSCODER_SOURCE_WATCH)
        {
            /* curennt ch ¿Œ ∞ÊøÏ current ¿« path ∏¶ ±◊µ•∑Œ ªÁøÎ */
        }
        else
        {
            #if 1 /* not yet test */
            CMD_Printf("%s(%d) : only Support DI_TRANSCODER_SOURCE_WATCH\n\r");
            iResult = CMD_ERR;
            goto __GOTO_RETURN;
            #else
            diErr = DI_DEMUX_ReleasePath(unDemuxId);
            if(diErr != DI_ERR_OK)
            {
                CMD_Printf("error in DI_DEMUX_ReleasePath\n\r");
                iResult = CMD_ERR;
                goto __GOTO_RETURN;
            }

            if(unTunerId == 0xFF)
            {
                diErr = DI_DEMUX_SetPath(unDemuxId, DI_DEMUX_INPUT_PLAYER, unTunerId, DI_DEMUX_OUTPUT_RECORDER, ulRecId, -1);
            }
            else
            {
                diErr = DI_DEMUX_SetPath(unDemuxId, DI_DEMUX_INPUT_TUNER, unTunerId, DI_DEMUX_OUTPUT_RECORDER, ulRecId, -1);
            }

            if(diErr != DI_ERR_OK)
            {
                CMD_Printf("error in DI_DEMUX_SetPath\n\r");
                iResult = CMD_ERR;
                goto __GOTO_RETURN;
            }
            #endif
        }

        /* Audio Video Transcoding */
        s_stSetting.eTransSourceType = SourceType;
        s_stSetting.eTransEncodeMode = EncodeMode;
        s_stSetting.eInputVideoFormat = unVideoCodec_Src;
        s_stSetting.eOutputVideoFormat = unVideoCodec_Dest;
        s_stSetting.eInputAudioFormat = unAudioCodec_Src;
        s_stSetting.eOutputAudioFormat = unAudioCodec_Dest;
        s_stSetting.bVideoDefaultProfile = unVideoDefaultProfile;
        if(s_stSetting.bVideoDefaultProfile == TRUE)
        {
            /* refer to s_stVidEncMpeg2PL[], s_stVidEncH264PL[] in drv_video_enc.c */
            /*
                        static DI_VidEncProfileLevel_t s_stVidEncMpeg2PL[] = {
                            {NEXUS_VideoFormat_eNtsc, NEXUS_VideoFrameRate_e30, NEXUS_VideoCodecLevel_eMain, NEXUS_VideoCodecProfile_eMain, 187500,10,2},
                            {NEXUS_VideoFormat_ePal, NEXUS_VideoFrameRate_e25, NEXUS_VideoCodecLevel_eMain, NEXUS_VideoCodecProfile_eMain, 187500,8,2},
                            {NEXUS_VideoFormat_e576p, NEXUS_VideoFrameRate_e25, NEXUS_VideoCodecLevel_eMain, NEXUS_VideoCodecProfile_eMain, 187500,8,2},
                            {NEXUS_VideoFormat_e720p, NEXUS_VideoFrameRate_e30, NEXUS_VideoCodecLevel_eHigh, NEXUS_VideoCodecProfile_eMain, 7500000,10,2},
                            {NEXUS_VideoFormat_e720p, NEXUS_VideoFrameRate_e60, NEXUS_VideoCodecLevel_eHigh, NEXUS_VideoCodecProfile_eMain, 10000000,20,2},
                            {NEXUS_VideoFormat_e720p50hz, NEXUS_VideoFrameRate_e25, NEXUS_VideoCodecLevel_eHigh, NEXUS_VideoCodecProfile_eMain, 7500000,8,2},
                            {NEXUS_VideoFormat_e720p50hz, NEXUS_VideoFrameRate_e50, NEXUS_VideoCodecLevel_eHigh, NEXUS_VideoCodecProfile_eMain, 10000000,17,2},
                            {NEXUS_VideoFormat_e1080i, NEXUS_VideoFrameRate_e30, NEXUS_VideoCodecLevel_eHigh, NEXUS_VideoCodecProfile_eMain, 10000000,29,0},
                            {NEXUS_VideoFormat_e1080i50hz, NEXUS_VideoFrameRate_e25, NEXUS_VideoCodecLevel_eHigh, NEXUS_VideoCodecProfile_eMain, 10000000,24,0}
                        };
                        static DI_VidEncProfileLevel_t s_stVidEncH264PL[] = {
                            {NEXUS_VideoFormat_eNtsc, NEXUS_VideoFrameRate_e30, NEXUS_VideoCodecLevel_e30, NEXUS_VideoCodecProfile_eHigh, 2768554,10,2},
                            {NEXUS_VideoFormat_ePal, NEXUS_VideoFrameRate_e25, NEXUS_VideoCodecLevel_e30, NEXUS_VideoCodecProfile_eHigh, 2768554,8,2},
                            {NEXUS_VideoFormat_e576p, NEXUS_VideoFrameRate_e25, NEXUS_VideoCodecLevel_e30, NEXUS_VideoCodecProfile_eHigh, 2768554,8,2},
                            {NEXUS_VideoFormat_e720p, NEXUS_VideoFrameRate_e30, NEXUS_VideoCodecLevel_e31, NEXUS_VideoCodecProfile_eHigh, 7382812,10,2},
                            {NEXUS_VideoFormat_e720p, NEXUS_VideoFrameRate_e60, NEXUS_VideoCodecLevel_e32, NEXUS_VideoCodecProfile_eHigh, 1476562,20,2},
                            {NEXUS_VideoFormat_e720p50hz, NEXUS_VideoFrameRate_e25, NEXUS_VideoCodecLevel_e31, NEXUS_VideoCodecProfile_eHigh, 6152343,8,2},
                            {NEXUS_VideoFormat_e720p50hz, NEXUS_VideoFrameRate_e50, NEXUS_VideoCodecLevel_e32, NEXUS_VideoCodecProfile_eHigh, 12304687,17,2},
                            {NEXUS_VideoFormat_e1080i, NEXUS_VideoFrameRate_e30, NEXUS_VideoCodecLevel_e40, NEXUS_VideoCodecProfile_eHigh, 16611328,29,0},
                            {NEXUS_VideoFormat_e1080i50hz, NEXUS_VideoFrameRate_e25, NEXUS_VideoCodecLevel_e40, NEXUS_VideoCodecProfile_eHigh, 13842773,24,0}
                        };
                    */
        }
        else
        {
            CMD_Printf("%s(%d) : only support bVideoDefaultProfile = TRUE\n",__func__,__LINE__);
            iResult = CMD_ERR;
            goto __GOTO_RETURN;
        }

        s_stSetting.stVideoSettings.eRes = unVideoResolution_Dest;
        s_stSetting.stVideoSettings.eFrameRate = unVideoFramerate_Dest;
        s_stSetting.stVideoSettings.eAspectRatio = unVideoAspectRatio_Dest;

        s_stSetting.ulDemuxId = unDemuxId;
        s_stSetting.usVideoPid = unVideoPid_Src;
        s_stSetting.usAudioPid = unAudioPid_Src;
        s_stSetting.usPcrPid = unPcrPid_Src;

        diErr = DI_TRANSCODER_SetSettings(unDeviceId, &s_stSetting);
        if(diErr != DI_ERR_OK)
        {
            CMD_Printf("%s(%d) : DI_TRANSCODER_SetSettings(%d) error\n",__func__,__LINE__, diErr);
            iResult = CMD_ERR;
            goto __GOTO_RETURN;
        }

        s_stMuxInfo.index = ulMuxId;
        s_stMuxInfo.videoCodec = unVideoCodec_Dest;
        s_stMuxInfo.videoPid = unVideoPid_Src;
        s_stMuxInfo.audioCodec = unAudioCodec_Dest;
        s_stMuxInfo.audioPid = unAudioPid_Src;
        s_stMuxInfo.pcrPid = unPcrPid_Src;
        s_stMuxInfo.NonPcrInsert = unNonPcrInsert;
        if(s_stMuxInfo.videoPid == s_stMuxInfo.pcrPid)
        {
            s_stMuxInfo.pcrPid += 1;
            if(s_stMuxInfo.pcrPid == s_stMuxInfo.audioPid)
            {
                s_stMuxInfo.pcrPid += 1;
            }
        }
        diErr = DI_TRANSCODER_Start(unDeviceId, DI_TRANSCODER_MUX_TS, &s_stMuxInfo);
        if(diErr != DI_ERR_OK)
        {
            CMD_Printf("%s(%d) : DI_TRANSCODER_Start(%d) error\n",__func__,__LINE__, diErr);
            iResult = CMD_ERR;
            goto __GOTO_RETURN;
        }

        psicontinuos = 0;

    #if 0 /* Insert PAT, PMT */
    {
        static HUINT32 psiIndex[3] = {0,};

        patPid = 0x00;
        pmtPid = 0x55;
        psiPid = 0x12;

        s_auiTSPacket_PAT[182] |= (pmtPid >> 8);
        s_auiTSPacket_PAT[183] = (pmtPid & 0x00FF);
        diErr = DI_TRANSCODER_SetPSIData(unDeviceId,patPid,PSI_DEFAULT_SIZE,s_auiTSPacket_PAT,psicontinuos,1000,&psiIndex[0]);
        VK_printf("== Trans PSI: PAT (%d:%d) ===\r\n",diErr,psiIndex[0]);

        s_auiTSPacket_PMT[170] |= (s_stMuxInfo.pcrPid >> 8);
        s_auiTSPacket_PMT[171] = (s_stMuxInfo.pcrPid & 0x00FF);
        s_auiTSPacket_PMT[175] |= (s_stMuxInfo.videoPid >> 8);
        s_auiTSPacket_PMT[176] = (s_stMuxInfo.videoPid & 0x00FF);
        s_auiTSPacket_PMT[180] |= (s_stMuxInfo.audioPid >> 8);
        s_auiTSPacket_PMT[181] = (s_stMuxInfo.audioPid & 0x00FF);
        diErr = DI_TRANSCODER_SetPSIData(unDeviceId,pmtPid,PSI_DEFAULT_SIZE,s_auiTSPacket_PMT,psicontinuos,1000,&psiIndex[1]);
        VK_printf("== Trans PSI: PMT (%d:%d) ===\r\n",diErr,psiIndex[1]);


        diErr = DI_TRANSCODER_SetPSIData(unDeviceId,psiPid,PSI_DEFAULT_SIZE,s_auiTSPacket_EIT,psicontinuos,1000,&psiIndex[2]);
        VK_printf("== Trans PSI: EIT (%d:%d) ===\r\n",diErr,psiIndex[2]);
    }
    #endif

        s_stRecSetup.eFileType = eDI_PVR_FileType_Linear;
        s_stRecSetup.eStreamType = eDI_PVR_Stream_Video;
        s_stRecSetup.ulDemuxId = unDemuxId;
        if(s_stSetting.eOutputVideoFormat == DI_VIDEO_FORMAT_MPEG2)
        {
            s_stRecSetup.eVideoType = eDI_PVR_VideoType_Mpeg2;
        }
        else if(s_stSetting.eOutputVideoFormat == DI_VIDEO_FORMAT_H264)
        {
            s_stRecSetup.eVideoType = eDI_PVR_VideoType_H264;
        }
        else
        {
            CMD_Printf("%s(%d) : Not Support OutputVideoFormat(%d)\n", __FUNCTION__, __LINE__, s_stSetting.eOutputVideoFormat);
            iResult = CMD_ERR;
            goto __GOTO_RETURN;
        }

        if(unEncType == DI_PVR_ENCRYPTION_TYPE_3DES)
        {
           s_stRecSetup.stEncryptSettings.eEncType = DI_PVR_ENCRYPTION_TYPE_3DES;

           VK_memcpy(s_stRecSetup.stEncryptSettings.ucKey, s_aucEncryptionKey, DI_PVR_CRYPTO_KEY_LEN);
           VK_memcpy(s_stRecSetup.stEncryptSettings.ucIv, s_aucEncryptionIV, DI_PVR_CRYPTO_KEY_LEN);
           VK_memcpy(s_stRecSetup.stEncryptSettings.ucProtectionKey, s_aucEncryptionProtectionKey, DI_PVR_CRYPTO_KEY_LEN);
        }
        else
        {
            /* not yet ~~ */
            s_stRecSetup.stEncryptSettings.eEncType = DI_PVR_ENCRYPTION_TYPE_NONE;
        }

        /* «ˆ¿Á HDD ø° ¿˙¿Â«“ ¿Ã∏ß¿ª ±∏«—¥Ÿ. */
        VK_sprintf(aBuf,"/mnt/hd%d",ulHDDId+1);
        if (chdir(aBuf) < 0)
        {
           CMD_Printf("%s(%d) : chdir(%s) error\n",__func__,__LINE__, aBuf);
           iResult = CMD_ERR;
           goto __GOTO_RETURN;
        }

        nitems = scandir(".", &items, NULL, alphasort);

        for (i = 0; i < nitems; i++)
        {
           if ( (!VK_strcmp(items[i]->d_name, ".")) || (!VK_strcmp(items[i]->d_name, "..")) )
           {
               VK_free(items[i]);
               continue;
           }
           if(VK_strstr( items[i]->d_name, ".ts"))
           {
               nCnt++;
           }
           VK_free(items[i]);
        }
        VK_free(items);

        VK_sprintf(s_stRecSetup.aucFileName,"%s/hdd_write_test%03d",aBuf,nCnt);

        if (chdir("/") < 0)
        {
           CMD_Printf("%s(%d) : chdir(%s) error\n",__func__,__LINE__, aBuf);
           iResult = CMD_ERR;
           goto __GOTO_RETURN;
        }

        VK_printf("####### Name %s \n",s_stRecSetup.aucFileName);

        #ifdef CMD_TRANSCODER_DEBUG
        VK_printf("########## DI_TRANSCODE_REC_Setup ##########\n");
        VK_printf("unDeviceId = 0x%x\n", unDeviceId);
        VK_printf("ulMuxId = 0x%x\n", ulMuxId);
        VK_printf("s_stRecSetup.eFileType = 0x%x\n", s_stRecSetup.eFileType);
        VK_printf("s_stRecSetup.eStreamType = 0x%x\n", s_stRecSetup.eStreamType);
        VK_printf("s_stRecSetup.eVideoType = 0x%x\n", s_stRecSetup.eVideoType);
        VK_printf("s_stRecSetup.aucFileName = %s\n", s_stRecSetup.aucFileName);
        VK_printf("s_stRecSetup.stEncryptSettings.eEncType = 0x%x\n", s_stRecSetup.stEncryptSettings.eEncType);
        VK_printf("s_stRecSetup.ulDemuxId = 0x%x\n", s_stRecSetup.ulDemuxId);
        VK_printf("s_stRecSetup.ulPacketSize = 0x%x\n", s_stRecSetup.ulPacketSize);
        VK_printf("s_stRecSetup.bIsAllPidChannel = 0x%x\n", s_stRecSetup.bIsAllPidChannel);
        VK_printf("\n");
        #endif
        diErr = DI_TRANSCODE_REC_Setup(unDeviceId, ulMuxId, &s_stRecSetup);
        if(diErr != DI_ERR_OK)
        {
           CMD_Printf("%s(%d) : DI_TRANSCODE_REC_Setup(%d) error\n",__func__,__LINE__, diErr);
           iResult = CMD_ERR;
           goto __GOTO_RETURN;
        }

        #ifdef CMD_TRANSCODER_DEBUG
        VK_printf("########## DI_TRANSCODE_REC_Start ##########\n");
        VK_printf("unDeviceId = 0x%x\n", unDeviceId);
        VK_printf("\n");
        #endif
        diErr = DI_TRANSCODE_REC_Start(unDeviceId);
        if(diErr != DI_ERR_OK)
        {
           CMD_Printf("%s(%d) : DI_TRANSCODE_REC_Start(%d) error\n",__func__,__LINE__, diErr);
           iResult = CMD_ERR;
           goto __GOTO_RETURN;
        }


        VK_sprintf(aBuf,"%s/hdd_write_test%03d.info",aBuf,nCnt);
        /*
                   03ff 00000001 0403 00000004 03ff
                   Vpid   VCodec Apid   ACodec  Ppid
                   ==> ∫Ø∞Ê
                   03ff   00000001 0000       0403      00000004   0403     00000004   0403     00000004   03ff
                   Vpid   VCodec  audio cnt Apid[0]   ACodec[0] Apid[1]   ACodec[1] Apid[2]   ACodec[2]  Ppid
               */
        /* Listø° ∫∏ø© ¡Ÿ ø©∫Œ.. */
        if(s_stMuxInfo.videoCodec == DI_VIDEO_FORMAT_MPEG2)
        {
           unVideoCodec = 0x00;
        }
        else if(s_stMuxInfo.videoCodec == DI_VIDEO_FORMAT_H264)
        {
           unVideoCodec = 0x01;
        }
        else
        {
           unVideoCodec = 0x00;
        }

        if(s_stMuxInfo.audioCodec == DI_AUDFORMAT_MPEG)
        {
           unAudioCodec = 0x00;
        }
        else if(s_stMuxInfo.audioCodec == DI_AUDFORMAT_MP3)
        {
           unAudioCodec = 0x01;
        }
        else if(s_stMuxInfo.audioCodec == DI_AUDFORMAT_AAC)
        {
           unAudioCodec = 0x02;
        }
        else if(s_stMuxInfo.audioCodec == DI_AUDFORMAT_AAC_PLUS)
        {
           unAudioCodec = 0x03;
        }
        else if(s_stMuxInfo.audioCodec == DI_AUDFORMAT_AC3)
        {
           unAudioCodec = 0x04;
        }
        else if(s_stMuxInfo.audioCodec == DI_AUDFORMAT_AC3_PLUS)
        {
           unAudioCodec = 0x05;
        }
        else if(s_stMuxInfo.audioCodec == DI_AUDFORMAT_PCM)
        {
           unAudioCodec = 0x06;
        }
        else
        {
           unAudioCodec = 0x00;
        }

        VK_sprintf(aInfo,"%04x%08x%04x%04x%08x%04x%08x%04x%08x%04x",s_stMuxInfo.videoPid,unVideoCodec,1,s_stMuxInfo.audioPid,unAudioCodec,s_stMuxInfo.audioPid,unAudioCodec,s_stMuxInfo.audioPid,unAudioCodec,s_stMuxInfo.audioPid,s_stMuxInfo.pcrPid);
        if ( 0 < ( fd = open( aBuf, O_WRONLY | O_CREAT | O_EXCL | O_SYNC, 0644)))
        {
           write( fd, aInfo, VK_strlen(aInfo));
           close( fd);
        }
        else
        {
           CMD_Printf( "file open fail");
           iResult = CMD_ERR;
           goto __GOTO_RETURN;
        }

        CMD_Printf("hw test s/w transcoding start!!\n");
        iResult = CMD_OK;
    }
    else if CMD_IS("stop")
    {
        HUINT32 unDeviceId;
        HUINT32 ulMuxId;
        DI_ERR_CODE diErr = 0;

        READABLE_IN_DEC(HWTEST_PARAM, unDeviceId);

        ulMuxId = unDeviceId;

        VK_printf("[%s %d] unDeviceId = 0x%x\n",__func__,__LINE__,unDeviceId);
        VK_printf("[%s %d] ulMuxId = 0x%x\n",__func__,__LINE__,ulMuxId);

        diErr = DI_TRANSCODE_REC_Stop(unDeviceId);
        if(diErr != DI_ERR_OK)
        {
            CMD_Printf("%s(%d) : DI_TRANSCODE_REC_Stop(%d) error\n",__func__,__LINE__, diErr);
            iResult = CMD_ERR;
            goto __GOTO_RETURN;
        }

        diErr = DI_TRANSCODE_REC_Unset(unDeviceId, ulMuxId);
        if(diErr != DI_ERR_OK)
        {
            CMD_Printf("%s(%d) : DI_TRANSCODE_REC_Unset(%d) error\n",__func__,__LINE__, diErr);
            iResult = CMD_ERR;
            goto __GOTO_RETURN;
        }

        diErr = DI_TRANSCODER_Stop(unDeviceId);
        if(diErr != DI_ERR_OK)
        {
            CMD_Printf("%s(%d) : DI_TRANSCODER_Stop(%d) error\n",__func__,__LINE__, diErr);
            iResult = CMD_ERR;
            goto __GOTO_RETURN;
        }

        diErr =DI_TRANSCODER_Close(unDeviceId);
        if(diErr != DI_ERR_OK)
        {
            CMD_Printf("%s(%d) : DI_TRANSCODER_Close(%d) error\n",__func__,__LINE__, diErr);
            iResult = CMD_ERR;
            goto __GOTO_RETURN;
        }

        CMD_Printf("hw test s/w transcoding stop!!\n");
        iResult = CMD_OK;
    }
    else if CMD_IS("info")
    {
        HUINT32 unDeviceId;
        DI_TRANSCODER_DEC_StreamInfo_t stream_info;
        DI_ERR_CODE diErr = 0;

        READABLE_IN_DEC(HWTEST_PARAM, unDeviceId);

        diErr = DI_TRANSCODER_GetDecSequenceHeaderInfo(unDeviceId,  &stream_info);
        if(diErr != DI_ERR_OK)
        {
            CMD_Printf("%s(%d) : DI_TRANSCODER_GetDecSequenceHeaderInfo error!!!\n",__func__,__LINE__);
            iResult = CMD_ERR;
        }
        else
        {
            VK_printf("ulWidth %d \n", stream_info.ulWidth);
            VK_printf("ulHeight %d \n", stream_info.ulHeight);
            VK_printf("eAspectRatio %d \n", stream_info.eAspectRatio);
            VK_printf("usSampleAspectRatioX %d \n", stream_info.usSampleAspectRatioX);
            VK_printf("usSampleAspectRatioY %d \n", stream_info.usSampleAspectRatioY);
            VK_printf("eVideoFormat %d \n", stream_info.eVideoFormat);
            VK_printf("bProgressive %d \n", stream_info.bProgressive);
            VK_printf("ucColorPrimarie %d \n", stream_info.ucColorPrimarie);
            VK_printf("eCodecLevel %d \n", stream_info.eCodecLevel);
            VK_printf("eCodecProfile %d \n", stream_info.eCodecProfile);

            iResult = CMD_OK;
        }
    }
	else if CMD_IS("stereo")
    {
        HUINT32 unDeviceId;
		HUINT32 unAudioMode;
        DI_TRANSCODER_DEC_StreamInfo_t stream_info;
        DI_ERR_CODE diErr = 0;

        READABLE_IN_DEC(HWTEST_PARAM, unDeviceId);
		READABLE_IN_DEC(HWTEST_PARAM1, unAudioMode);
        diErr = DI_TRANSCODER_SetStereo(unDeviceId, unAudioMode);
        if(diErr != DI_ERR_OK)
        {
            CMD_Printf("%s(%d) : DI_TRANSCODER_SetStereo error!!!\n",__func__,__LINE__);
            iResult = CMD_ERR;
        }
        else
        {
            iResult = CMD_OK;
        }
    }
	else if CMD_IS("test1")
	{
		DI_ERR_CODE diErr = DI_ERR_OK;
		DI_TRANSCODER_Settings_t s_stSetting;
		DI_TRANSCODER_MUX_INFO s_stMuxInfo;
		DI_PVR_PLAY_SETUP_t playSetup;
		DI_PVR_PLAY_SETUP_t *stSetup;
		DI_VIDEO_FORMAT nVidFmt;
		DI_DEMUX_INPUTDEVICETYPE inDevType;
		HUINT16 patPid = 0, pmtPid = 0,psiPid = 0;
		HUINT16 usVideoPid;
		HUINT16 usAudioPid;
		HUINT16 usPcrPid;
		int buffer[128];
		HUINT32 ulDeviceId = 0;
		HUINT32 ulRecDeviceId;
		HUINT32 ulPbDeviceId;
		HUINT32 ulDemuxId;
		HUINT32 ulMuxId = 0;
		HUINT16 psicontinuos = 0;
		static HUINT32 psiIndex[3] = {0,};
		HUINT32 inDevId;
		Pid_Info pidInfo;
		DI_PVR_REC_SETUP_t s_stRecSetup;

		usVideoPid = 0x100;/*256*/
		usAudioPid = 0x110;/*272*/
		usPcrPid = 0x1FF;/*511*/

		VK_memset(&playSetup, 0, sizeof(DI_PVR_PLAY_SETUP_t));
		VK_snprintf(buffer, 127, "/mnt/hd2/hdd_write_test1\0");
		VK_memcpy((void *)playSetup.aucFileName, (void *)buffer, VK_strlen((char *)buffer)+1);
		playSetup.bIsRecording		= false;
		playSetup.eFileType			= eDI_PVR_FileType_Linear;
		playSetup.eStartCondition	= DI_PVR_PLAY_START_MODE_BEGIN;
		playSetup.eStreamType		= eDI_PVR_Stream_Video;
		playSetup.ulPacketSize		= 192;
		playSetup.ulStartTime		= 0;

		stSetup = &playSetup;

		inDevType = DI_DEMUX_INPUT_PLAYER;
		inDevId = stSetup->ulStartTime;

		ulPbDeviceId 	= 0;
		ulRecDeviceId	= 0;
		ulDemuxId = DRV_DEMUX_GetFirstPlaybackParserIndex();

		CMD_Printf("(demuxId = %lu recordId = %lu, pbId = %lu) for transcoder device %lu\n", ulDemuxId, ulRecDeviceId, ulPbDeviceId, ulDeviceId);

		if PARAM_IS("start")
		{
			diErr = DI_DEMUX_SetPath(ulDemuxId, inDevType, inDevId, DI_DEMUX_OUTPUT_TRANSCODER, 0, -1);
			if(diErr != DI_ERR_OK)
			{
				CMD_Printf("error in DI_DEMUX_SetPath\n\r");
				return CMD_ERR;
			}
			stSetup->ulDemuxId = ulDemuxId;

			diErr = DI_PVR_PLAY_Setup(ulPbDeviceId, stSetup);
			if(diErr != DI_ERR_OK)
			{
				CMD_Printf("error in DI_PVR_PLAY_Setup\n\r");
				return CMD_ERR;
			}

			VK_memset(&pidInfo,0x00,sizeof(pidInfo));
			pidInfo.pidBitmask = PID_BITMASK_VIDEO_TRANS_1 | PID_BITMASK_AUDIO_TRANS_1 | PID_BITMASK_PCR_TRANS_1;
			pidInfo.usVideoPid = usVideoPid;
			pidInfo.usAudioMainPid = usAudioPid;
			pidInfo.usPCRPid = usPcrPid;
			diErr = DI_DEMUX_StartPID(ulDemuxId, &pidInfo);
			if(diErr != DI_ERR_OK)
			{
				CMD_Printf("error in DI_DEMUX_StartPID\n\r");
				//return CMD_ERR;
			}

			diErr = DI_PVR_PLAY_Start(ulPbDeviceId);
			if(diErr != DI_ERR_OK)
			{
				CMD_Printf("error in DI_PVR_PLAY_Start\n\r");
				return CMD_ERR;
			}

			VK_memset(&s_stSetting,0x00,sizeof(s_stSetting));
			s_stSetting.eTransEncodeMode			= DI_TRANSCODER_ENCODE_AV;
			s_stSetting.eTransSourceType			= DI_TRANSCODER_SOURCE_BACKGROUND;
			s_stSetting.eInputVideoFormat			= DI_VIDEO_FORMAT_MPEG2;
			s_stSetting.eOutputVideoFormat			= DI_VIDEO_FORMAT_H264;
			s_stSetting.stVideoSettings.eRes		= DI_VIDEO_HD_RESOLUTION_1080I;
			s_stSetting.stVideoSettings.eFrameRate	= DI_VIDEO_FRAMERATE_25;
			s_stSetting.eInputAudioFormat			= DI_AUDFORMAT_AAC;
			s_stSetting.eOutputAudioFormat			= DI_AUDFORMAT_AAC;
			s_stSetting.stVideoSettings.stCustomRes.bCustomRes = FALSE;
			s_stSetting.stVideoSettings.ulMaxBitRate = 6*1024*1024;
			s_stSetting.stAudioSettings.eEncType = DI_TRANSCODER_AUDIO_BYPASS;

			s_stSetting.eCodecLevel = DI_TRANSCODER_VIDEO_CODEC_LEVEL_31;
			s_stSetting.eCodecProfile = DI_TRANSCODER_VIDEO_CODEC_PROFILE_MAIN;

			s_stSetting.stAudioSettings.bMultiAudio = FALSE;
			s_stSetting.stAudioSettings.ulMultiAudioCnt = 1;
			s_stSetting.ulDemuxId = ulDemuxId;
			DI_TRANSCODER_SetSettings(ulDeviceId,&s_stSetting);

			VK_memset(&s_stMuxInfo,0x00,sizeof(s_stMuxInfo));
			s_stMuxInfo.index		= 0;
			s_stMuxInfo.videoCodec	= DI_VIDEO_FORMAT_H264;
			s_stMuxInfo.videoPid	= 0x1100;
			s_stMuxInfo.audioCodec	= DI_AUDFORMAT_AAC;
			s_stMuxInfo.audioPid	= 0x1101;
			s_stMuxInfo.pcrPid		= 0x1103;
			s_stMuxInfo.bMultiAudio = TRUE;
			s_stMuxInfo.ulMultiAudioCnt = 1;

			DI_TRANSCODER_Start(ulDeviceId,DI_TRANSCODER_MUX_TS,&s_stMuxInfo);

			VK_printf("=== Trans : 2 (%d) ===\r\n",diErr);
			psicontinuos = 1;

			patPid = 0x00;
			pmtPid = 0x402;
			psiPid = 0x12;

			diErr = DI_TRANSCODER_SetPSIData(ulDeviceId,patPid,48,patsectiondata,psicontinuos,1000,&psiIndex[0]);
			VK_printf("== Trans PSI: PAT (%d:%d) ===\r\n",diErr,psiIndex[0]);
			pmtsectiondata[8] |= (s_stMuxInfo.pcrPid >> 8);
			pmtsectiondata[9] = (s_stMuxInfo.pcrPid & 0x00FF);

			if(s_stMuxInfo.videoCodec == DI_VIDEO_FORMAT_H264)
			{
				pmtsectiondata[18] = 0x1b;
			}
			else
			{
				pmtsectiondata[18] = 0x02;
			}

			pmtsectiondata[19] |= (s_stMuxInfo.videoPid >> 8);
			pmtsectiondata[20] = (s_stMuxInfo.videoPid & 0x00FF);

			if(s_stMuxInfo.audioCodec == DI_AUDFORMAT_AAC)
			{
				pmtsectiondata[26] = 0x0F;
			}
			else
			{
				pmtsectiondata[26] = 0x04;
			}
			pmtsectiondata[27] |= (s_stMuxInfo.audioPid >> 8);
			pmtsectiondata[28] = (s_stMuxInfo.audioPid & 0x00FF);

			diErr = DI_TRANSCODER_SetPSIData(ulDeviceId,pmtPid,44,pmtsectiondata,psicontinuos,1000,&psiIndex[1]);
			if(diErr != DI_ERR_OK)
			{
				CMD_Printf("error in DI_PVR_PLAY_Start\n\r");
				return CMD_ERR;
			}

			VK_printf("== Trans PSI: PMT (%d:%d) ===\r\n",diErr,psiIndex[1]);

			diErr = DI_TRANSCODER_SetPSIData(ulDeviceId,psiPid,237,eitsectiondata,psicontinuos,1000,&psiIndex[2]);
			if(diErr != DI_ERR_OK)
			{
				CMD_Printf("error in DI_PVR_PLAY_Start\n\r");
				return CMD_ERR;
			}

			VK_memset(&s_stRecSetup,0x00,sizeof(s_stRecSetup));
			s_stRecSetup.eFileType = eDI_PVR_FileType_Linear;
			s_stRecSetup.eStreamType = eDI_PVR_Stream_Video;
			s_stRecSetup.eVideoType = eDI_PVR_VideoType_H264;
			s_stRecSetup.stEncryptSettings.eEncType = DI_PVR_ENCRYPTION_TYPE_NONE;
			VK_snprintf(s_stRecSetup.aucFileName, 17,"/mnt/hd2/trans_%d",ulMuxId);
			DI_TRANSCODE_REC_Setup(ulRecDeviceId, ulMuxId, &s_stRecSetup);
			DI_TRANSCODE_REC_Start(ulRecDeviceId);
			CMD_Printf("hw test file transcoding start!!\n");
			iResult = CMD_OK;
		}
		else if PARAM_IS("stop")
		{
			DI_PVR_REC_RegisterEventCallback(ulRecDeviceId, DI_PVR_EVENT_TIMECODE_CHANGED, NULL);
			DI_TRANSCODE_REC_Stop(ulRecDeviceId);
			DI_TRANSCODE_REC_Unset(ulRecDeviceId, ulMuxId);
			DI_TRANSCODER_Stop(ulDeviceId);
			DI_DEMUX_StopPID(ulDemuxId, (HUINT32)PID_BITMASK_VIDEO_TRANS_1| PID_BITMASK_AUDIO_TRANS_1| PID_BITMASK_PCR_TRANS_1);
			DI_TRANSCODER_Close(ulDeviceId);
			DI_PVR_PLAY_Stop(ulPbDeviceId);
			DI_DEMUX_ReleasePath(ulDemuxId);
			CMD_Printf("hw test file transcoding stop!!\n");
			iResult = CMD_OK;
		}
		else
		{
			iResult = CMD_ERR;
		}
	}
	else if CMD_IS("mbypass")
	{
		DI_ERR_CODE diErr = DI_ERR_OK;
		DI_TRANSCODER_Settings_t s_stSetting;
		DI_TRANSCODER_MUX_INFO s_stMuxInfo;
		DI_PVR_PLAY_SETUP_t playSetup;
		DI_PVR_PLAY_SETUP_t *stSetup;
		DI_VIDEO_FORMAT nVidFmt;
		DI_DEMUX_INPUTDEVICETYPE inDevType;
		HUINT16 patPid = 0, pmtPid = 0,psiPid = 0;
		HUINT16 usVideoPid;
		HUINT16 usAudioPid,usAudioSubPid;
		HUINT16 usPcrPid;
		int buffer[128];
		HUINT32 ulDeviceId = 0;
		HUINT32 ulRecDeviceId;
		HUINT32 ulPbDeviceId;
		HUINT32 ulDemuxId;
		HUINT32 ulMuxId = 0;
		HUINT16 psicontinuos = 0;
		static HUINT32 psiIndex[3] = {0,};
		HUINT32 inDevId;
		Pid_Info pidInfo;

		usVideoPid = 0x100;/*256*/
		usAudioPid = 0x110;/*272*/
		usAudioSubPid = 0x111;/*273*/
		usPcrPid = 0x1FF;/*511*/

		VK_memset(&playSetup, 0, sizeof(DI_PVR_PLAY_SETUP_t));
		VK_snprintf(buffer, 127, "test_multi_aac_audio\0");
		VK_memcpy((void *)playSetup.aucFileName, (void *)buffer, VK_strlen((char *)buffer)+1);
		playSetup.bIsRecording = false;
		playSetup.eFileType = eDI_PVR_FileType_Linear;
		playSetup.eStartCondition = DI_PVR_PLAY_START_MODE_BEGIN;
		playSetup.eStreamType = eDI_PVR_Stream_Video;
		playSetup.ulPacketSize = 192;
		playSetup.ulStartTime = 0;

		stSetup = &playSetup;

		inDevType = DI_DEMUX_INPUT_PLAYER;
		inDevId = stSetup->ulStartTime;


		ulPbDeviceId = 0;
		ulRecDeviceId = 0;
		ulDemuxId = DRV_DEMUX_GetFirstPlaybackParserIndex();

		CMD_Printf("(demuxId = %lu recordId = %lu, pbId = %lu) for transcoder device %lu\n", ulDemuxId, ulRecDeviceId, ulPbDeviceId, ulDeviceId);

		if PARAM_IS("start")
		{
			diErr = DI_DEMUX_SetPath(ulDemuxId, inDevType, inDevId, DI_DEMUX_OUTPUT_TRANSCODER, 0, -1);
			if(diErr != DI_ERR_OK)
			{
				CMD_Printf("error in DI_DEMUX_SetPath\n\r");
				return CMD_ERR;
			}
			stSetup->ulDemuxId = ulDemuxId;
			diErr = DI_PVR_PLAY_Setup(ulPbDeviceId, stSetup);
			if(diErr != DI_ERR_OK)
			{
				CMD_Printf("error in DI_PVR_PLAY_Setup\n\r");
				return CMD_ERR;
			}
			pidInfo.pidBitmask = PID_BITMASK_VIDEO_TRANS_1 | PID_BITMASK_AUDIO_TRANS_1 |PID_BITMASK_AUDIO_SUB_TRANS_1| PID_BITMASK_PCR_TRANS_1;
			//pidInfo.pidBitmask = PID_BITMASK_VIDEO_TRANS_1 | PID_BITMASK_AUDIO_TRANS_1 | PID_BITMASK_PCR_TRANS_1;
			pidInfo.usVideoPid = usVideoPid;
			pidInfo.usAudioMainPid = usAudioPid;
			pidInfo.usAudioSubPid = usAudioSubPid;
			pidInfo.usPCRPid = usPcrPid;
			diErr = DI_DEMUX_StartPID(ulDemuxId, &pidInfo);
			if(diErr != DI_ERR_OK)
			{
				CMD_Printf("error in DI_DEMUX_StartPID\n\r");
				return CMD_ERR;
			}
			diErr = DI_PVR_PLAY_Start(ulPbDeviceId);


			s_stSetting.eTransEncodeMode = DI_TRANSCODER_ENCODE_AV;
			s_stSetting.eTransSourceType = DI_TRANSCODER_SOURCE_BACKGROUND;
			s_stSetting.eInputVideoFormat = DI_VIDEO_FORMAT_MPEG2;
			s_stSetting.eOutputVideoFormat = DI_VIDEO_FORMAT_H264;
			s_stSetting.stVideoSettings.eRes = DI_VIDEO_HD_RESOLUTION_480I;
			s_stSetting.stVideoSettings.eFrameRate = DI_VIDEO_FRAMERATE_25;
			s_stSetting.eInputAudioFormat = DI_AUDFORMAT_AAC;
			s_stSetting.eOutputAudioFormat = DI_AUDFORMAT_AAC;
			s_stSetting.stVideoSettings.stCustomRes.bCustomRes = FALSE;
			s_stSetting.stVideoSettings.ulMaxBitRate = 6*1024*1024;

			s_stSetting.eCodecLevel = DI_TRANSCODER_VIDEO_CODEC_LEVEL_31;
			s_stSetting.eCodecProfile = DI_TRANSCODER_VIDEO_CODEC_PROFILE_MAIN;

			//s_stSetting.stAudioSettings.eEncType = DI_TRANSCODER_AUDIO_ENCODING;
			s_stSetting.stAudioSettings.bMultiAudio = TRUE;
			s_stSetting.stAudioSettings.ulMultiAudioCnt = 2;
			s_stSetting.ulDemuxId = ulDemuxId;
			DI_TRANSCODER_SetSettings(ulDeviceId,&s_stSetting);

			s_stMuxInfo.index=0;
			s_stMuxInfo.videoCodec=DI_VIDEO_FORMAT_H264;
			s_stMuxInfo.videoPid=0x1100;
			s_stMuxInfo.audioCodec=DI_AUDFORMAT_AAC;
			s_stMuxInfo.audioPid=0x1101;
			s_stMuxInfo.audioSubPid=0x1102;
			s_stMuxInfo.pcrPid=0x1103;
			s_stMuxInfo.bMultiAudio = TRUE;
			s_stMuxInfo.ulMultiAudioCnt = 2;
//			DI_PVR_REC_RegisterEventCallback(ulRecDeviceId, DI_PVR_EVENT_TIMECODE_CHANGED, hwtest_TRANS_TEST_CB);
			DI_TRANSCODER_Start(ulDeviceId,DI_TRANSCODER_MUX_TS,&s_stMuxInfo);

			VK_printf("=== Trans : 2 (%d) ===\r\n",diErr);
			psicontinuos = 1; /* 0 ==> √É¬≥?¬Ω √á√ë¬π√∏ , 1==> ¬ø¬¨¬º√ì?√ª?¬∏¬∑√é */

			patPid = 0x00;
			pmtPid = 0x402;
			psiPid = 0x12;

			diErr = DI_TRANSCODER_SetPSIData(ulDeviceId,patPid,48,patsectiondata,psicontinuos,1000,&psiIndex[0]);
			VK_printf("== Trans PSI: PAT (%d:%d) ===\r\n",diErr,psiIndex[0]);
			pmtsectiondata[8] |= (s_stMuxInfo.pcrPid >> 8);
			pmtsectiondata[9] = (s_stMuxInfo.pcrPid & 0x00FF);

			if(s_stMuxInfo.videoCodec == DI_VIDEO_FORMAT_H264)
				pmtsectiondata[18] = 0x1b;
			else
				pmtsectiondata[18] = 0x02;

			pmtsectiondata[19] |= (s_stMuxInfo.videoPid >> 8);
			pmtsectiondata[20] = (s_stMuxInfo.videoPid & 0x00FF);

			if(s_stMuxInfo.audioCodec == DI_AUDFORMAT_AAC)
				pmtsectiondata[26] = 0x0F;
			else
				pmtsectiondata[26] = 0x04;

			pmtsectiondata[27] |= (s_stMuxInfo.audioPid >> 8);
			pmtsectiondata[28] = (s_stMuxInfo.audioPid & 0x00FF);


			diErr = DI_TRANSCODER_SetPSIData(ulDeviceId,pmtPid,44,pmtsectiondata,psicontinuos,1000,&psiIndex[1]);
			VK_printf("== Trans PSI: PMT (%d:%d) ===\r\n",diErr,psiIndex[1]);

			diErr = DI_TRANSCODER_SetPSIData(ulDeviceId,psiPid,237,eitsectiondata,psicontinuos,1000,&psiIndex[2]);
			if(diErr != DI_ERR_OK)
			{
				CMD_Printf("error in DI_PVR_PLAY_Start\n\r");
				return CMD_ERR;
			}

			{
				DI_PVR_REC_SETUP_t s_stRecSetup;
				s_stRecSetup.eFileType = eDI_PVR_FileType_Linear;
				s_stRecSetup.eStreamType = eDI_PVR_Stream_Video;
				s_stRecSetup.eVideoType = eDI_PVR_VideoType_H264;
				s_stRecSetup.stEncryptSettings.eEncType = DI_PVR_ENCRYPTION_TYPE_NONE;
				VK_snprintf(s_stRecSetup.aucFileName, 17,"trans_%d",ulMuxId);
				DI_TRANSCODE_REC_Setup(ulRecDeviceId, ulMuxId, &s_stRecSetup);
				DI_TRANSCODE_REC_Start(ulRecDeviceId);
			}
			CMD_Printf("hw test s/w tsr transcoding start!!\n");
			iResult = CMD_OK;
		}
		else if PARAM_IS("stop")
		{
			DI_PVR_REC_RegisterEventCallback(ulRecDeviceId, DI_PVR_EVENT_TIMECODE_CHANGED, NULL);
			DI_TRANSCODE_REC_Stop(ulRecDeviceId);
			DI_TRANSCODE_REC_Unset(ulRecDeviceId, ulMuxId);
			DI_TRANSCODER_Stop(ulDeviceId);
			DI_DEMUX_StopPID(ulDemuxId, (HUINT32)PID_BITMASK_VIDEO_TRANS_1| PID_BITMASK_AUDIO_TRANS_1|PID_BITMASK_AUDIO_SUB_TRANS_1| PID_BITMASK_PCR_TRANS_1);
			DI_TRANSCODER_Close(ulDeviceId);
			DI_PVR_PLAY_Stop(ulPbDeviceId);
			DI_DEMUX_ReleasePath(ulDemuxId);
			CMD_Printf("hw test s/w tsr transcoding stop!!\n");
			iResult = CMD_OK;
		}
		else
		{
			iResult = CMD_ERR;
		}
	}
	else if CMD_IS("mencoder")
	{
		DI_ERR_CODE diErr = DI_ERR_OK;
		DI_TRANSCODER_Settings_t s_stSetting;
		DI_TRANSCODER_MUX_INFO s_stMuxInfo;
		DI_PVR_PLAY_SETUP_t playSetup;
		DI_PVR_PLAY_SETUP_t *stSetup;
		DI_VIDEO_FORMAT nVidFmt;
		DI_DEMUX_INPUTDEVICETYPE inDevType;
		HUINT16 patPid = 0, pmtPid = 0,psiPid = 0;
		HUINT16 usVideoPid;
		HUINT16 usAudioPid,usAudioSubPid;
		HUINT16 usPcrPid;
		int buffer[128];
		HUINT32 ulDeviceId = 0;
		HUINT32 ulRecDeviceId;
		HUINT32 ulPbDeviceId;
		HUINT32 ulDemuxId;
		HUINT32 ulMuxId = 0;
		HUINT16 psicontinuos = 0;
		static HUINT32 psiIndex[3] = {0,};
		HUINT32	inDevId;
		Pid_Info pidInfo;

		usVideoPid = 0x100;/*256*/
		usAudioPid = 0x110;/*272*/
		usAudioSubPid = 0x111;/*273*/
		usPcrPid = 0x1FF;/*511*/

		VK_memset(&playSetup, 0, sizeof(DI_PVR_PLAY_SETUP_t));
		VK_snprintf(buffer, 127, "test_multi_aac_audio\0");
		VK_memcpy((void *)playSetup.aucFileName, (void *)buffer, VK_strlen((char *)buffer)+1);
		playSetup.bIsRecording = false;
		playSetup.eFileType = eDI_PVR_FileType_Linear;
		playSetup.eStartCondition = DI_PVR_PLAY_START_MODE_BEGIN;
		playSetup.eStreamType = eDI_PVR_Stream_Video;
		playSetup.ulPacketSize = 192;
		playSetup.ulStartTime = 0;

		stSetup = &playSetup;

		inDevType = DI_DEMUX_INPUT_PLAYER;
		inDevId = stSetup->ulStartTime;


		ulPbDeviceId = 0;
		ulRecDeviceId = 0;
		ulDemuxId = DRV_DEMUX_GetFirstPlaybackParserIndex();

		CMD_Printf("(demuxId = %lu recordId = %lu, pbId = %lu) for transcoder device %lu\n", ulDemuxId, ulRecDeviceId, ulPbDeviceId, ulDeviceId);

		if PARAM_IS("start")
		{
			diErr = DI_DEMUX_SetPath(ulDemuxId, inDevType, inDevId, DI_DEMUX_OUTPUT_TRANSCODER, 0, -1);
			if(diErr != DI_ERR_OK)
			{
				CMD_Printf("error in DI_DEMUX_SetPath\n\r");
				return CMD_ERR;
			}
			stSetup->ulDemuxId = ulDemuxId;
			diErr = DI_PVR_PLAY_Setup(ulPbDeviceId, stSetup);
			if(diErr != DI_ERR_OK)
			{
				CMD_Printf("error in DI_PVR_PLAY_Setup\n\r");
				return CMD_ERR;
			}
			pidInfo.pidBitmask = PID_BITMASK_VIDEO_TRANS_1 | PID_BITMASK_AUDIO_TRANS_1 |PID_BITMASK_AUDIO_SUB_TRANS_1| PID_BITMASK_PCR_TRANS_1;
			pidInfo.usVideoPid = usVideoPid;
			pidInfo.usAudioMainPid = usAudioPid;
			pidInfo.usAudioSubPid = usAudioSubPid;
			pidInfo.usPCRPid = usPcrPid;
			diErr = DI_DEMUX_StartPID(ulDemuxId, &pidInfo);
			if(diErr != DI_ERR_OK)
			{
				CMD_Printf("error in DI_DEMUX_StartPID\n\r");
				return CMD_ERR;
			}
			diErr = DI_PVR_PLAY_Start(ulPbDeviceId);


			s_stSetting.eTransEncodeMode = DI_TRANSCODER_ENCODE_AV;
			s_stSetting.eTransSourceType = DI_TRANSCODER_SOURCE_BACKGROUND;
			s_stSetting.eInputVideoFormat = DI_VIDEO_FORMAT_MPEG2;
			s_stSetting.eOutputVideoFormat = DI_VIDEO_FORMAT_H264;
			s_stSetting.stVideoSettings.eRes = DI_VIDEO_HD_RESOLUTION_480I;
			s_stSetting.stVideoSettings.eFrameRate = DI_VIDEO_FRAMERATE_25;
			s_stSetting.eInputAudioFormat = DI_AUDFORMAT_AAC;
			s_stSetting.eOutputAudioFormat = DI_AUDFORMAT_AAC;
			s_stSetting.stVideoSettings.stCustomRes.bCustomRes = FALSE;
			s_stSetting.stVideoSettings.ulMaxBitRate = 6*1024*1024;

			s_stSetting.eCodecLevel = DI_TRANSCODER_VIDEO_CODEC_LEVEL_31;
			s_stSetting.eCodecProfile = DI_TRANSCODER_VIDEO_CODEC_PROFILE_MAIN;

			s_stSetting.stAudioSettings.eEncType = DI_TRANSCODER_AUDIO_ENCODING;
			s_stSetting.stAudioSettings.bMultiAudio = TRUE;
			s_stSetting.stAudioSettings.ulMultiAudioCnt = 2;
			s_stSetting.ulDemuxId = ulDemuxId;
			DI_TRANSCODER_SetSettings(ulDeviceId,&s_stSetting);

			s_stMuxInfo.index=0;
			s_stMuxInfo.videoCodec=DI_VIDEO_FORMAT_H264;
			s_stMuxInfo.videoPid=0x1100;
			s_stMuxInfo.audioCodec=DI_AUDFORMAT_AAC;
			s_stMuxInfo.audioPid=0x1101;
			s_stMuxInfo.audioSubPid=0x1102;
			s_stMuxInfo.pcrPid=0x1103;
			s_stMuxInfo.bMultiAudio = TRUE;
			s_stMuxInfo.ulMultiAudioCnt = 2;
//			DI_PVR_REC_RegisterEventCallback(ulRecDeviceId,	DI_PVR_EVENT_TIMECODE_CHANGED, hwtest_TRANS_TEST_CB);
			DI_TRANSCODER_Start(ulDeviceId,DI_TRANSCODER_MUX_TS,&s_stMuxInfo);

			VK_printf("=== Trans : 2 (%d) ===\r\n",diErr);
			psicontinuos = 1; /* 0 ==> √É¬≥?¬Ω √á√ë¬π√∏ , 1==> ¬ø¬¨¬º√ì?√ª?¬∏¬∑√é */

			patPid = 0x00;
			pmtPid = 0x402;
			psiPid = 0x12;

			diErr = DI_TRANSCODER_SetPSIData(ulDeviceId,patPid,48,patsectiondata,psicontinuos,1000,&psiIndex[0]);
			VK_printf("== Trans PSI: PAT (%d:%d) ===\r\n",diErr,psiIndex[0]);
			pmtsectiondata[8] |= (s_stMuxInfo.pcrPid >> 8);
			pmtsectiondata[9] = (s_stMuxInfo.pcrPid & 0x00FF);

			if(s_stMuxInfo.videoCodec == DI_VIDEO_FORMAT_H264)
				pmtsectiondata[18] = 0x1b;
			else
				pmtsectiondata[18] = 0x02;

			pmtsectiondata[19] |= (s_stMuxInfo.videoPid >> 8);
			pmtsectiondata[20] = (s_stMuxInfo.videoPid & 0x00FF);

			if(s_stMuxInfo.audioCodec == DI_AUDFORMAT_AAC)
				pmtsectiondata[26] = 0x0F;
			else
				pmtsectiondata[26] = 0x04;

			pmtsectiondata[27] |= (s_stMuxInfo.audioPid >> 8);
			pmtsectiondata[28] = (s_stMuxInfo.audioPid & 0x00FF);


			diErr = DI_TRANSCODER_SetPSIData(ulDeviceId,pmtPid,44,pmtsectiondata,psicontinuos,1000,&psiIndex[1]);
			VK_printf("== Trans PSI: PMT (%d:%d) ===\r\n",diErr,psiIndex[1]);

			diErr = DI_TRANSCODER_SetPSIData(ulDeviceId,psiPid,237,eitsectiondata,psicontinuos,1000,&psiIndex[2]);
			if(diErr != DI_ERR_OK)
			{
				CMD_Printf("error in DI_PVR_PLAY_Start\n\r");
				return CMD_ERR;
			}
			{
				DI_PVR_REC_SETUP_t s_stRecSetup;
				s_stRecSetup.eFileType = eDI_PVR_FileType_Linear;
				s_stRecSetup.eStreamType = eDI_PVR_Stream_Video;
				s_stRecSetup.eVideoType = eDI_PVR_VideoType_H264;
				s_stRecSetup.stEncryptSettings.eEncType = DI_PVR_ENCRYPTION_TYPE_NONE;
				VK_snprintf(s_stRecSetup.aucFileName, 17,"trans_%d",ulMuxId);
				DI_TRANSCODE_REC_Setup(ulRecDeviceId, ulMuxId, &s_stRecSetup);
				DI_TRANSCODE_REC_Start(ulRecDeviceId);
			}
			CMD_Printf("hw test s/w tsr transcoding start!!\n");
			iResult = CMD_OK;
		}
		else if PARAM_IS("stop")
		{
			DI_PVR_REC_RegisterEventCallback(ulRecDeviceId,	DI_PVR_EVENT_TIMECODE_CHANGED, NULL);
			DI_TRANSCODE_REC_Stop(ulRecDeviceId);
			DI_TRANSCODE_REC_Unset(ulRecDeviceId, ulMuxId);
			DI_TRANSCODER_Stop(ulDeviceId);
			DI_DEMUX_StopPID(ulDemuxId, (HUINT32)PID_BITMASK_VIDEO_TRANS_1| PID_BITMASK_AUDIO_TRANS_1|PID_BITMASK_AUDIO_SUB_TRANS_1| PID_BITMASK_PCR_TRANS_1);
			DI_TRANSCODER_Close(ulDeviceId);
			DI_PVR_PLAY_Stop(ulPbDeviceId);
			DI_DEMUX_ReleasePath(ulDemuxId);
			CMD_Printf("hw test s/w tsr transcoding stop!!\n");
			iResult = CMD_OK;
		}
		else
		{
			iResult = CMD_ERR;
		}
	}
	else if CMD_IS("restart")
    {
        HUINT32 unDeviceId;
        DI_ERR_CODE diErr = 0;

        READABLE_IN_DEC(HWTEST_PARAM, unDeviceId);
        diErr = DI_TRANSCODER_ReStart(unDeviceId);
        if(diErr != DI_ERR_OK)
        {
            CMD_Printf("%s(%d) : DI_TRANSCODER_ReStart error!!!\n",__func__,__LINE__);
            iResult = CMD_ERR;
        }
        else
        {
            iResult = CMD_OK;
        }
    }
    else
    {
        iResult = CMD_ERR;
    }


__GOTO_RETURN:

    return iResult;
}

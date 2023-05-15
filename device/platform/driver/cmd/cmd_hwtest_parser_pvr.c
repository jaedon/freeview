#if defined(CONFIG_VK_STDLIB)
#else
#include 	<stdio.h>
#endif
#include <sys/mount.h>
#include <unistd.h>
#include <sys/stat.h>
#include <sys/types.h>

#include "bstd_defs.h"
#include "vkernel.h"
#include "htype.h"
#include "taskdef.h"
#include "cmdtool.h"
#include "cmd_hwtest_parser_pvr.h"
#include "di_err.h"
#include "di_video.h"
#include "di_demux.h"
#include "di_audio.h"
#include "di_scart.h"
#include "di_pvr.h"
#include "drv_pvr.h"
#include "drv_scart.h"
#include "linden_trace.h"

#include "nexus_pid_channel_scrambling.h"

#define HWTEST_CMD		szCmd
#define HWTEST_PARAM	szParam
#define HWTEST_PARAM1	szParam1
#define HWTEST_PARAM2	szParam2
#define HWTEST_PARAM3	szParam3
#define HWTEST_PARAM4	szParam4
#define HWTEST_PARAM5	szParam5
#define HWTEST_PARAM6	szParam6
#define HWTEST_PARAM7	szParam7
#define HWTEST_PARAM8	szParam8
#define HWTEST_PARAM9	szParam9
#define HWTEST_PARAM10	szParam10


#define GET_ARGS		int iResult=CMD_ERR; \
						char *HWTEST_CMD=CMD_NextArg((char **)&szArgs); \
						char *HWTEST_PARAM=CMD_NextArg((char **)&szArgs); \
						char *HWTEST_PARAM1=CMD_NextArg((char **)&szArgs); \
						char *HWTEST_PARAM2=CMD_NextArg((char **)&szArgs); \
						char *HWTEST_PARAM3=CMD_NextArg((char **)&szArgs); \
						char *HWTEST_PARAM4=CMD_NextArg((char **)&szArgs); \
						char *HWTEST_PARAM5=CMD_NextArg((char **)&szArgs); \
						char *HWTEST_PARAM6=CMD_NextArg((char **)&szArgs); \
						char *HWTEST_PARAM7=CMD_NextArg((char **)&szArgs); \
						char *HWTEST_PARAM8=CMD_NextArg((char **)&szArgs); \
						char *HWTEST_PARAM9=CMD_NextArg((char **)&szArgs); \
						char *HWTEST_PARAM10=CMD_NextArg((char **)&szArgs);

#define CMD_IS(x)		(HWTEST_CMD!=NULL && VK_strcmp(HWTEST_CMD, x)==0)
#define PARAM_IS(x)		(HWTEST_PARAM!=NULL && VK_strcmp(HWTEST_PARAM, x)==0)
#define PARAM1_IS(x)	(HWTEST_PARAM1!=NULL && VK_strcmp(HWTEST_PARAM1, x)==0)
#define PARAM2_IS(x)	(HWTEST_PARAM2!=NULL && VK_strcmp(HWTEST_PARAM2, x)==0)
#define PARAM3_IS(x)	(HWTEST_PARAM3!=NULL && VK_strcmp(HWTEST_PARAM3, x)==0)
#define PARAM4_IS(x)	(HWTEST_PARAM4!=NULL && VK_strcmp(HWTEST_PARAM4, x)==0)
#define PARAM5_IS(x)	(HWTEST_PARAM5!=NULL && VK_strcmp(HWTEST_PARAM5, x)==0)
#define PARAM6_IS(x)	(HWTEST_PARAM6!=NULL && VK_strcmp(HWTEST_PARAM6, x)==0)
#define PARAM7_IS(x)	(HWTEST_PARAM7!=NULL && VK_strcmp(HWTEST_PARAM7, x)==0)
#define PARAM9_IS(x)	(HWTEST_PARAM8!=NULL && VK_strcmp(HWTEST_PARAM8, x)==0)
#define PARAM10_IS(x)	(HWTEST_PARAM9!=NULL && VK_strcmp(HWTEST_PARAM9, x)==0)
#define PARAM11_IS(x)	(HWTEST_PARAM10!=NULL && VK_strcmp(HWTEST_PARAM10, x)==0)


#define READABLE_IN_HEX(sz,val) CMD_ReadAsHex(sz, &val)
#define READABLE_IN_DEC(sz,val) CMD_ReadAsDec(sz, &val)

#define GET_NEXT_ARG(variable)		char *variable = CMD_NextArg((char **)&szArgs)

#define DEFAULT_TUNER 0
#define DEFAULT_PLAY_DEMUX 8
#define DEFAULT_REC_DEMUX 2
#define SECOND_REC_DEMUX 3
#define HDD_TEST_REC1_DEV_ID 1
#define HDD_TEST_REC2_DEV_ID 2
#define DEFAULT_PLAYER 0
#define DEFAULT_VIDEO 0
#define DEFAULT_AUDIO 0
#define MAX_REC_NUM 7
#define MAX_PID_NUM 5

#define THIRD_REC_DEMUX 4
#define HDD_TEST_REC3_DEV_ID 3
#define FOURTH_REC_DEMUX 5
#define HDD_TEST_REC4_DEV_ID 4
#define FIFTH_REC_DEMUX 6
#define HDD_TEST_REC5_DEV_ID 5
#define SIXTH_REC_DEMUX 7
#define HDD_TEST_REC6_DEV_ID 6

#define	CMD_PB_EOF_EVENTS	(1)
#define	CMD_PB_TASK_DESTROY (2)
#define DEF_DEV_NODE "sdb"

#if defined(CONFIG_DI20)
#define CONFIG_TEMP_MPLAY
#endif

#if defined(CONFIG_PRODUCT_IMAGE_HWTEST)

#if defined(CONFIG_MAX_RECORD_INSTANCE) && (CONFIG_MAX_RECORD_INSTANCE == 7)
#define ENABLE_6_RECORDING
#endif

typedef enum {
	CMD_HDD_TEST_NONE,
	CMD_HDD_TEST_2REC_1PB,
	CMD_HDD_TEST_CHANGE_REC_PB
} CMD_HDD_TEST_STATUS;

typedef struct {
	HUINT16 videoPid;
	HUINT16 audioPid;
	HUINT16 pcrPid;
	int videotype;
	int audiotype;
	int extHdd;
} CMD_HDD_TEST_REC_INFO;
#endif

#if defined(ENABLE_6_RECORDING)
#define RECORDING_DEVICE_NUM		6
#else
#define RECORDING_DEVICE_NUM		2
#endif

#define MAX_RECORDING_PATH	128
static char RecordingPath[MAX_RECORDING_PATH] = "/mnt/hd2";
static HUINT32 recPidChHandle[MAX_REC_NUM][MAX_PID_NUM];
static unsigned long cmdPbEventId;
static unsigned long cmdPbTaskId;
static unsigned char taskDestroyProcess = false;
#if defined(CONFIG_PRODUCT_IMAGE_HWTEST)
static CMD_HwTest_Pvr_Event_Callback pvrEvtCbFunc[CMD_HWTEST_PVR_EVENT_MAX_NUMBER];
static CMD_HDD_TEST_STATUS hddTestStatus = CMD_HDD_TEST_NONE;
static int hddTestNextRecIdForPbSw = HDD_TEST_REC1_DEV_ID;
static CMD_HDD_TEST_REC_INFO hddTestRecInfo[RECORDING_DEVICE_NUM];

#if defined(CONFIG_JAPAN_SPEC)
#if defined(CONFIG_SIT_PACKET_INSERTION)
const unsigned char pucSITDummy[] =
{
	/* below capured data was real SIT data on BS148(at 2012.12.12.13:00...) */
	0x7f , 0xf0 , 0xbf , 0xff , 0xff , 0xc3 , 0x00 , 0x00 , 0xf0 , 0x27 , 0xc3 , 0x12 , 0xff , 0xff , 0xff , 0xff ,
	0xff , 0xff , 0xff , 0xff , 0xff , 0x00 , 0x00 , 0x00 , 0xf9 , 0xdb , 0xd1 , 0x04 , 0x22 , 0x33 , 0xc2 , 0x07 ,
	0x4a , 0x50 , 0x4e , 0x42 , 0x53 , 0x00 , 0x04 , 0x63 , 0x08 , 0xc0 , 0xea , 0x60 , 0xff , 0xff , 0xff , 0xff ,
	0xff , 0x00 , 0x8d , 0x00 , 0x89 , 0xc3 , 0x0d , 0x01 , 0xdb , 0xd1 , 0x04 , 0x00 , 0x00 , 0x00 , 0x30 , 0x00 ,
	0x00 , 0x00 , 0x00 , 0xf8 , 0x85 , 0x07 , 0x00 , 0x04 , 0x40 , 0xd0 , 0x34 , 0xbd , 0x02 , 0x4d , 0x12 , 0x6a ,
	0x70 , 0x6e , 0x0b , 0x1b , 0x7c , 0xc6 , 0xec , 0xd3 , 0xb7 , 0xe7 , 0xc3 , 0xd4 , 0xf3 , 0xb0 , 0x02 , 0x21 ,
	0x21 , 0x50 , 0x06 , 0xf1 , 0xb3 , 0x00 , 0x6a , 0x70 , 0x6e , 0x54 , 0x02 , 0x24 , 0xff , 0xd6 , 0x0d , 0x13 ,
	0x00 , 0x8d , 0x34 , 0xbd , 0x00 , 0x8e , 0x34 , 0xbd , 0x00 , 0x8f , 0x34 , 0xbd , 0xc1 , 0x01 , 0x84 , 0xc4 ,
	0x09 , 0xf2 , 0x03 , 0x10 , 0x0f , 0xff , 0x6f , 0x6a , 0x70 , 0x6e , 0xc7 , 0x1b , 0x00 , 0x07 , 0x40 , 0x0f ,
	0x23 , 0x4e , 0x6a , 0x70 , 0x6e , 0x04 , 0x40 , 0xf0 , 0x52 , 0xf0 , 0x53 , 0xf0 , 0x54 , 0xf0 , 0xbf , 0x03 ,
	0x52 , 0x53 , 0x54 , 0x6a , 0x70 , 0x6e , 0x00 , 0x48 , 0x15 , 0x01 , 0x08 , 0x0e , 0x42 , 0x53 , 0x0f , 0x46 ,
	0x7c , 0x4b , 0x5c , 0x0a , 0x0e , 0x42 , 0x53 , 0x0f , 0x46 , 0x7c , 0x1b , 0x7c , 0xc6 , 0xec , 0x52 , 0xd8 ,
	0xee , 0x8a
};
static HUINT32 ulHandle_PacketInsetion[MAX_REC_NUM];
#endif
#endif

#endif

DI_PVR_ENCRYPTION_TYPE g_erEncryptyinType = DI_PVR_ENCRYPTION_TYPE_NONE;

static HUINT8 s_aucAESEncryptionKey[DI_PVR_CRYPTO_KEY_LEN] = { 0x12, 0x34, 0x56, 0x78, 0x9a, 0xbc, 0xde, 0xf0, 0x12, 0x34, 0xfe, 0xed, 0xba, 0xbe, 0xbe, 0xef};
static HUINT8 s_aucEncryptionKey[DI_PVR_CRYPTO_KEY_LEN] = {0x0, 0x10, 0x32, 0x54, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0};
static HUINT8 s_aucEncryptionIV[DI_PVR_CRYPTO_KEY_LEN] = {0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0};
static HUINT8 s_aucEncryptionProtectionKey[DI_PVR_CRYPTO_KEY_LEN] = {0x34, 0x29, 0x83, 0x1, 0x28, 0x55, 0x92, 0x2, 0x48, 0x47, 0x55, 0x2, 0x1, 0x0, 0x0, 0x0};
static HUINT8 s_aucDecryptionProtectionKey[DI_PVR_CRYPTO_KEY_LEN] = {0x2f, 0x6d, 0x6e, 0x74, 0x2f, 0x68, 0x64, 0x32, 0x2f, 0x4d, 0x79, 0x20, 0x56, 0x69, 0x64, 0x65};

static int HwTest_Pvr_TPRecStart(HUINT32 ulDeviceId, HUINT32 ulDmxId, HUINT32 ulInputDevId, DI_PVR_REC_SETUP_t *stSetup);
static int HwTest_Pvr_TPRecStop(HUINT32 ulDeviceId, HUINT32 ulDmxId);
static int HwTest_Pvr_RecStart(HUINT32 ulDeviceId, HUINT32 ulDmxId, HUINT32 ulInputDevId, DI_PVR_REC_SETUP_t *stSetup,
									HUINT16 usVideoPid, HUINT16 usAudioPid, HUINT16 usPcrPid, HUINT16 usPmtPid);
static int HwTest_Pvr_RecStop(HUINT32 ulDeviceId, HUINT32 ulDmxId);
static void HwTest_Pvr_PlayEofCallback(void *pMsg);
static int HwTest_Pvr_PlayStart(DI_PVR_PLAY_SETUP_t *stSetup, DI_VIDEO_FORMAT nVidFmt, DI_AUDIO_FORMAT nAudFmt,
									HUINT16 usVideoPid, HUINT16 usAudioPid, HUINT16 usPcrPid);
static int HwTest_Pvr_PlayStop(void);
static void HwTestPvr_PlayEventTask(void *param);
HUINT8 hwtest_FindPmtPid( HUINT32 demuxChannel, HUINT32 tunerNum, HUINT16 audioPid, HUINT16 videoPid, HUINT16 *pmtPid );

int CMD_HwTest_Parser_PvrEx(void *szArgs)
{
#if defined(CONFIG_PRODUCT_IMAGE_HWTEST)
    char *argValue = NULL;
    unsigned int argIndex = 0;
    unsigned int testtype = 3;
    unsigned int videopid = 0xFFFF;
    unsigned int videotype = 0;
    unsigned int audiopid = 0xFFFF;
    unsigned int audiotype = 0;
    unsigned int pcrpid = 0xFFFF;
    unsigned int tunerId = 3;
    int iReturn;
    unsigned short pmtpid = 0xFFFF;
	char buffer[MAX_RECORDING_PATH];
	int i;

    argValue = CMD_NextArg((char **)&szArgs);
    if(!READABLE_IN_DEC(argValue, testtype))
    {
        return;
    }
    if ( testtype == 1 || testtype == 2 )
    {
        argValue = CMD_NextArg((char **)&szArgs);
        if(!READABLE_IN_DEC(argValue, videopid))
        {
            VK_printf("%s(%d)\n", __FUNCTION__, __LINE__);
            return;
        }
        argValue = CMD_NextArg((char **)&szArgs);
        if(!READABLE_IN_DEC(argValue, videotype))
        {
            VK_printf("%s(%d)\n", __FUNCTION__, __LINE__);
            return;
        }
        argValue = CMD_NextArg((char **)&szArgs);
        if(!READABLE_IN_DEC(argValue, audiopid))
        {
            VK_printf("%s(%d)\n", __FUNCTION__, __LINE__);
            return;
        }
        argValue = CMD_NextArg((char **)&szArgs);
        if(!READABLE_IN_DEC(argValue, audiotype))
        {
            VK_printf("%s(%d)\n", __FUNCTION__, __LINE__);
            return;
        }
        argValue = CMD_NextArg((char **)&szArgs);
        if(!READABLE_IN_DEC(argValue, pcrpid))
        {
            VK_printf("%s(%d)\n", __FUNCTION__, __LINE__);
            return;
        }
        argValue = CMD_NextArg((char **)&szArgs);
        if(!READABLE_IN_DEC(argValue, tunerId))
        {
            VK_printf("%s(%d)\n", __FUNCTION__, __LINE__);
            return;
        }
    }
    if ( testtype == 1 )
    {
        DI_PVR_REC_SETUP_t recSetup;
        DI_PVR_PLAY_SETUP_t playSetup;
        VK_memset(&recSetup, 0, sizeof(DI_PVR_REC_SETUP_t));
		VK_snprintf(buffer, MAX_RECORDING_PATH-1, "%s/hdd_thermal_test1", RecordingPath);
		VK_memcpy((void *)recSetup.aucFileName, (void *)buffer, VK_strlen((char *)buffer)+1);

		recSetup.eFileType = eDI_PVR_FileType_Circular;
		recSetup.eStreamType = eDI_PVR_Stream_Video;
		recSetup.eVideoType = videotype;
		iReturn = HwTest_Pvr_RecStart(HDD_TEST_REC1_DEV_ID, DEFAULT_REC_DEMUX, (HUINT32)tunerId, &recSetup, (HUINT16)videopid, (HUINT16)audiopid, (HUINT16)pcrpid, pmtpid);
		if(iReturn
!= CMD_OK)
		{
			VK_printf("%s(%d)\n", __FUNCTION__, __LINE__);
			return;
		}

		VK_snprintf(buffer, MAX_RECORDING_PATH-1, "%s/hdd_thermal_test2", RecordingPath);
		VK_memcpy((void *)recSetup.aucFileName, (void *)buffer, VK_strlen((char *)buffer)+1);

		recSetup.eFileType = eDI_PVR_FileType_Linear;
		recSetup.eStreamType = eDI_PVR_Stream_Video;
		recSetup.eVideoType = videotype;
		iReturn = HwTest_Pvr_RecStart(HDD_TEST_REC2_DEV_ID, SECOND_REC_DEMUX, (HUINT32)tunerId, &recSetup, (HUINT16)videopid, (HUINT16)audiopid, (HUINT16)pcrpid, pmtpid);
		if(iReturn
!= CMD_OK)
		{
			VK_printf("%s(%d)\n", __FUNCTION__, __LINE__);
            return;
		}

		VK_TASK_Sleep(10000); /* Recorded data가 어느 정도 쌓일 때까지 wait.. */

		VK_memset(&playSetup, 0, sizeof(DI_PVR_PLAY_SETUP_t));

		VK_snprintf(buffer, MAX_RECORDING_PATH-1, "%s/hdd_thermal_test1", RecordingPath);
		VK_memcpy((void *)playSetup.aucFileName, (void *)buffer, VK_strlen((char *)buffer)+1);

		playSetup.bIsRecording = true;
		playSetup.eFileType = eDI_PVR_FileType_Circular;
		playSetup.eStartCondition = DI_PVR_PLAY_START_MODE_BEGIN;
		playSetup.eStreamType = eDI_PVR_Stream_Video;
		playSetup.ulStartTime = HDD_TEST_REC1_DEV_ID; /* eFileType이 eDI_PVR_FileType_Circular인 경우에는 ulStartTime에 관련 record ID를 넣어준다.(only for command) */
		iReturn = HwTest_Pvr_PlayStart(&playSetup,
									videotype,
									audiotype,
									(HUINT16)videopid,
									(HUINT16)audiopid,
									(HUINT16)pcrpid);
		if(iReturn
!= CMD_OK)
		{
			VK_printf("%s(%d)\n", __FUNCTION__, __LINE__);
            return;
		}

		hddTestStatus = CMD_HDD_TEST_2REC_1PB;
		hddTestNextRecIdForPbSw = HDD_TEST_REC2_DEV_ID;

		for (i = 0; i < RECORDING_DEVICE_NUM; i++)
		{
			hddTestRecInfo[i].videoPid = videopid;
			hddTestRecInfo[i].audioPid = audiopid;
			hddTestRecInfo[i].pcrPid = pcrpid;
			hddTestRecInfo[i].audiotype = audiotype;
			hddTestRecInfo[i].videotype = videotype;
			hddTestRecInfo[i].extHdd = 0;
		}

        if ( CMD_GetPVRTestReadyCallback() != NULL )
        {
            (int *)(CMD_GetPVRTestReadyCallback())();
        }


    }
    else if ( testtype == 2 )
    {
        DI_PVR_REC_SETUP_t recSetup;
        DI_PVR_PLAY_SETUP_t playSetup;

        VK_memset(&recSetup, 0, sizeof(DI_PVR_REC_SETUP_t));

        iReturn = VK_EVENT_Create(&cmdPbEventId, "cmdPbEvt");
        if(iReturn != 0)
        {
        	VK_printf("error in VK_EVENT_Create\n\r");
            return;
        }
        iReturn = VK_TASK_Create(HwTestPvr_PlayEventTask,
        					PLAYBACK_EVENT_TASK_PRIORITY,
        					PLAYBACK_EVENT_TASK_STACK_SIZE,
        					"cmdPb",
        					NULL,
        					&cmdPbTaskId,
        					false);
        if(iReturn != 0)
        {
        	VK_printf("error in VK_EVENT_Create\n\r");
            return;
        }
        VK_TASK_Start(cmdPbTaskId);

        iReturn = DRV_PVR_PLAY_BackupCallback(DEFAULT_PLAYER, TRUE);
        if(iReturn != 0)
        {
        	VK_printf("error in VK_EVENT_Create\n\r");
            return;
        }
        iReturn = DI_PVR_PLAY_RegisterEventCallback(DEFAULT_PLAYER, DI_PVR_EVENT_EOF, HwTest_Pvr_PlayEofCallback);
        if(iReturn != DI_ERR_OK)
        {
        	VK_printf("error in VK_EVENT_Create\n\r");
            return;
        }

        VK_memset(&playSetup, 0, sizeof(DI_PVR_PLAY_SETUP_t));

        VK_snprintf(buffer, MAX_RECORDING_PATH-1, "%s/hdd_thermal_stream", RecordingPath);
		VK_memcpy((void *)playSetup.aucFileName, (void *)buffer, VK_strlen((char *)buffer)+1);

        playSetup.bIsRecording = false;
        playSetup.eFileType = eDI_PVR_FileType_Linear;
        playSetup.eStartCondition = DI_PVR_PLAY_START_MODE_BEGIN;
        playSetup.eStreamType = eDI_PVR_Stream_Video;
        playSetup.ulStartTime = 0;
        iReturn = HwTest_Pvr_PlayStart(&playSetup,
        							videotype,
        							audiotype,
        							(HUINT16)videopid,
        							(HUINT16)audiopid,
        							(HUINT16)pcrpid);
        if(iReturn
!= CMD_OK)
        {
        	VK_printf("error in VK_EVENT_Create\n\r");
            return;
        }

        VK_memset(&recSetup, 0, sizeof(DI_PVR_REC_SETUP_t));

		VK_snprintf(buffer, MAX_RECORDING_PATH-1, "%s/hdd_thermal_test3", RecordingPath);
       	VK_memcpy((void *)recSetup.aucFileName, (void *)buffer, VK_strlen((char *)buffer)+1);

        recSetup.eFileType = eDI_PVR_FileType_Circular;
        recSetup.eStreamType = eDI_PVR_Stream_Video;
        recSetup.eVideoType = videotype;
        iReturn = HwTest_Pvr_RecStart(HDD_TEST_REC1_DEV_ID, DRV_DEMUX_GetFirstPlaybackParserIndex(), 0, &recSetup, (HUINT16)videopid, (HUINT16)audiopid, (HUINT16)pcrpid,pmtpid);
        if(iReturn
!= CMD_OK)
        {
        	VK_printf("error in VK_EVENT_Create\n\r");
            return;
        }

		VK_snprintf(buffer, MAX_RECORDING_PATH-1, "%s/hdd_thermal_test4", RecordingPath);
		VK_memcpy((void *)recSetup.aucFileName, (void *)buffer, VK_strlen((char *)buffer)+1);

        iReturn = HwTest_Pvr_RecStart(HDD_TEST_REC2_DEV_ID, DRV_DEMUX_GetFirstPlaybackParserIndex(), 0, &recSetup, (HUINT16)videopid, (HUINT16)audiopid, (HUINT16)pcrpid,pmtpid);
        if(iReturn
!= CMD_OK)
        {
        	VK_printf("error in VK_EVENT_Create\n\r");
            return;
        }
        if ( CMD_GetPVRTestReadyCallback() != NULL )
        {
            (int *)(CMD_GetPVRTestReadyCallback())();
        }
    }
    else if ( testtype == 3 )
    {
        iReturn = HwTest_Pvr_PlayStop();
        if(iReturn
!= CMD_OK)
        {
			VK_printf("%s(%d)\n", __FUNCTION__, __LINE__);
            return;
        }

        iReturn = HwTest_Pvr_RecStop(HDD_TEST_REC1_DEV_ID, DEFAULT_REC_DEMUX);
        if(iReturn
!= CMD_OK)
        {
			VK_printf("%s(%d)\n", __FUNCTION__, __LINE__);
            return;
        }
        iReturn = HwTest_Pvr_RecStop(HDD_TEST_REC2_DEV_ID, SECOND_REC_DEMUX);
        if(iReturn
!= CMD_OK)
        {
			VK_printf("%s(%d)\n", __FUNCTION__, __LINE__);
            return;
        }
    }
#endif
}



/*********************************************************************************************************************/
/* Add  Function for recording PAT/PMT
/*********************************************************************************************************************/

#include <ctype.h>


#define MAX_AudioTrk		64
#define MAX_VideoTrk		8
#define MAX_SvcName			81
#define MAX_ProviderName	20
#define N_ScanSvc			200


#define DESCRIPTOR_ISO_LAN							0x0a
#define DESCRIPTOR_CA			 					0x09
#define DESCRIPTOR_CAROUSEL_ID						0x13
#define DESCRIPTOR_MPEG2_AAC						0x2B
#define DESCRIPTOR_MPEG4_AAC_PLUS					0x1C

#define DESCRIPTOR_AVC								0x28
#define DESCRIPTOR_NW_NAME			 				0x40
#define DESCRIPTOR_SERVICE_LIST				   		0x41
#define DESCRIPTOR_STUFFING						 	0x42
#define DESCRIPTOR_SAT_DEL_SYS	   					0x43
#define DESCRIPTOR_CABLE_DEL_SYS					0x44
#define DESCRIPTOR_BOUQUET_NAME						0x47
#define DESCRIPTOR_SERVICE		    				0x48
#define DESCRIPTOR_COUNTRY_AVAIL	  			  	0x49
#define DESCRIPTOR_LINKAGE		    				0x4A
#define DESCRIPTOR_NVOD_REF		    				0x4B
#define DESCRIPTOR_TIME_SHIFTED_SERVICE				0x4C
#define DESCRIPTOR_SHORT_EVENT						0x4D
#define DESCRIPTOR_EXTENDED_EVENT					0x4E
#define DESCRIPTOR_TIME_SHIFTED_EVENT				0x4F
#define DESCRIPTOR_COMPONENT		   			 	0x50
#define DESCRIPTOR_MOSAIC							0x51
#define DESCRIPTOR_STREAM_ID						0x52
#define DESCRIPTOR_CA_IDENT		    				0x53
#define DESCRIPTOR_CONTENT		    				0x54
#define DESCRIPTOR_PARENTAL_RATING	    			0x55
#define DESCRIPTOR_TELETEXT		    				0x56
#define DESCRIPTOR_TELEPHONE						0x57
#define DESCRIPTOR_LOCAL_TIME_OFF					0x58
#define DESCRIPTOR_SUBTITLING						0x59
#define DESCRIPTOR_TERR_DEL_SYS						0x5A
#define DESCRIPTOR_ML_NW_NAME						0x5B
#define DESCRIPTOR_ML_BQ_NAME						0x5C
#define DESCRIPTOR_ML_SERVICE_NAME					0x5D
#define DESCRIPTOR_ML_COMPONENT						0x5E
#define DESCRIPTOR_PRIV_DATA_SPEC					0x5F
#define DESCRIPTOR_SERVICE_MOVE						0x60
#define DESCRIPTOR_SHORT_SMOOTH_BUF					0x61
#define DESCRIPTOR_FREQUENCY_LIST					0x62
#define DESCRIPTOR_PARTIAL_TP_STREAM				0x63
#define DESCRIPTOR_DATA_BROADCAST					0x64
#define DESCRIPTOR_CA_SYSTEM						0x65
#define DESCRIPTOR_DATA_BROADCAST_ID				0x66
#define DESCRIPTOR_AC3								0x6A
#define DESCRIPTOR_CONTENT_IDENTIFIER				0x76
#define DESCRIPTOR_ENHANCED_AC3						0x7A
#define DESCRIPTOR_DVB_AAC							0x7C
#define DESCRIPTOR_CONTENT_MANAGEMENT				0x7E


#define DESCRIPTOR_LOGICAL_CHANNEL_NUM				0x83
#define DESCRIPTOR_LOGICAL_CHANNEL_NUM_V2			0x87
#define DESCRIPTOR_CIT_REFERENCE					0x90

#define DESCRIPTOR_LESRADIOS						0xC5
#define DESCRIPTOR_CANAL_AUDIO_NAME					0xC5
#define DESCRIPTOR_CANAL_DATA						0xC6
#define DESCRIPTOR_MHW_DATA							0xC2


#define DESCRIPTOR_CONTENTS_ORDER					0xF0
#define DESCRIPTOR_PARENTAL_INFORMATION				0xF1
#define DESCRIPTOR_CONTENTS_TRANSMISSION			0xF2

#define STREAM_RESERVED         			0x00
#define STREAM_AUDIO_MP3      				0x01
#define STREAM_VIDEO_MPEG1      			0x01
#define STREAM_VIDEO_MPEG2      			0x02
#define STREAM_AUDIO_MPEG1      			0x03
#define STREAM_AUDIO_MPEG2      			0x04
#define STREAM_PRIVATE_SECTIONS 			0x05
#define STREAM_PRIVATE_PES      			0x06
#define STREAM_AUDIO_MPEG4_AC3_PLUS  		0x06
#define STREAM_MHEG             			0x07
#define STREAM_DSM_CC           			0x08
#define STREAM_TYPE_H2221       			0x09
#define STREAM_TYPE_A           			0x0A
#define STREAM_TYPE_B           			0x0B
#define STREAM_TYPE_C           			0x0C
#define STREAM_TYPE_D           			0x0D
#define STREAM_TYPE_AUX         			0x0E
#define STREAM_VIDEO_MPEG4_PART2     		0x10
#define STREAM_VIDEO_MPEG4_H263     		0x1A
#define STREAM_VIDEO_MPEG4_H264      		0x1B
#define STREAM_AUDIO_MPEG4_AAC  			0x0F
#define STREAM_AUDIO_MPEG4_AAC_PLUS  		0x11
#define STREAM_AUDIO_MPEG4_AC3	  			0x81
#define STREAM_AUDIO_MPEG4_DTS  			0x82
#define STREAM_VIDEO_VC1     	 			0xEA
#define STREAM_VIDEO_VC1SM	      			0xEB

#define AUDIO_PID_AUTO						0x8000
#define AUDIO_PID_AAC_PLUS_TRANSCODE		0x4000
#define AUDIO_PID_DOLBY						0x2000
#define AUDIO_PID_MASK						0x7FFF

#define PAT_PID						0x0000
#define	HWTEST_PSI_REQID			2

HINT32	hwtest_psi_reqid_idx_test = 0;

// service type
typedef enum
{
	SVC_TYPE_Tv,
	SVC_TYPE_Radio,
	N_SvcType,
	SVC_TYPE_NDef = 0x80
}	TYPE_SvcType;

typedef enum
{
	SOUND_MODE_Stereo,
	SOUND_MODE_Mono,
	SOUND_MODE_Left,
	SOUND_MODE_Right,
	N_SoundMode
}	TYPE_SoundMode;


typedef struct
{
	HUINT8		cas;
	HUINT8		svcType;
	HUINT16 	svcId;
	HUINT16 	pmtPid;
	HUINT16 	pmtEvt;
	HUINT16 	pcrPid;
	HUINT8 		vformat[MAX_VideoTrk]; // etcinfo에서 사용하는 video Format의 의미와 다름.  codec의 의미.
	HUINT8 		aformat[MAX_AudioTrk]; // audio format
	HUINT16 	videoPid[MAX_VideoTrk];
	HUINT16 	audioPid[MAX_AudioTrk];
	HUINT8		soundModeForced[MAX_AudioTrk];
	HUINT8 		soundType[MAX_AudioTrk];
	HCHAR		lan[MAX_AudioTrk][3]; // To present string
	HUINT8		nAudioPid;
	HUINT8		nVideoPid;
	HUINT8		eitAvail;

	HUINT16 	nvodIdx;
	HUINT16 	refTsId;
	HUINT16 	refNetId;
	HUINT16 	refSvcId;
	HCHAR		name[MAX_SvcName+1];
	HCHAR		providerName[MAX_ProviderName+1];
	HUINT16 	orgNetId;
	HUINT16 	tsId;
}	TYPE_ScanSvc;


TYPE_ScanSvc	_scanSvc[N_ScanSvc];
HUINT16 		_nScanSvc = 0;
HUINT16 		_ScanIndex = 0xffff;


HUINT32 Get12bit(const HUINT8 *p)
{
	return ((HUINT32)p[0] & 0x0f) << 8 | (HUINT32)p[1];
} /* Get12bit */


HUINT32 Get13bit(const HUINT8 *p)
{
	return ((HUINT32)p[0] & 0x1f) << 8 | (HUINT32)p[1];
} /* Get13bit */

HUINT32 Get16bit(const HUINT8 *p)
{
	return (HUINT32)p[0] << 8 | (HUINT32)p[1];
} /* Get16bit */


HCHAR StrNCmp(const void *s1, const void *s2, HUINT32 n)
{
	return VK_strncmp(s1,s2,n);
}

HCHAR *StrNToLower(HCHAR *str, HUINT32 n)
{
	HUINT32 i = 0;

	for(i=0; i<n; i++)
	{
		str[i] = VK_tolower(str[i]);
	}

	return str;
}


HUINT16 hwtest_CB_Parse_PAT(HUINT8 *pucBuf)
{
	HUINT16	secLen = 0;
	HUINT16	svcId = 0, pmtPid = 0, nScanSvc = 0;
	HUINT16 tsId = 0, result = 0;
	HUINT8	*p = NULL;

	nScanSvc = 0;

	if ((p = pucBuf) == NULL)
		return 0;

	secLen = Get12bit(p + 1) + 3;
	if( secLen < 8 )
		return 0;

	tsId = (unsigned short)Get16bit(p + 3);

	p += 8;
	secLen -= 8 + 4;

	while (secLen > 0)
	{
		svcId  = (unsigned short)Get16bit(p+0);
		pmtPid = Get13bit(p+2);

		if (svcId != 0 && pmtPid!=0)
		{
			VK_MEM_Memset( &_scanSvc[nScanSvc], 0xff, sizeof( TYPE_ScanSvc ) );
			_scanSvc[nScanSvc].svcId	= svcId;
			_scanSvc[nScanSvc].pmtPid	= pmtPid;
			_scanSvc[nScanSvc].pmtEvt	= 0xf000;
			_scanSvc[nScanSvc].nAudioPid = 0;
			_scanSvc[nScanSvc].nVideoPid = 0;
			_scanSvc[nScanSvc].cas		= 0;
			_scanSvc[nScanSvc].nvodIdx = 0;
			_scanSvc[nScanSvc].name[0] = 0;
			_scanSvc[nScanSvc].providerName[0] = 0;
			nScanSvc++;
		}

		p += 4;
		secLen -= 4;
	}

	return nScanSvc;
} // Parse_PAT

HUINT16 hwtest_CB_Parse_PMT(HUINT8 *pucBuf, HUINT16 nScanSvc)
{
	HUINT32	i = 0;
	HINT32	isAudioStream = 0;
	HUINT16	pid = 0, privatePid = 0xffff;
	HUINT16	vPid = 0, result = 0;
	HUINT16	secLen = 0, dscrLoopLen = 0, dscrLen = 0;
	HUINT16	svcId = 0, scanSvcIdx = 0xffff;

	HCHAR	ISO_639Code[3] = {0,};
	HUINT8	ac3Des = 0;
	HUINT8	iso639LanDesc[10];
	HUINT8	streamType = 0;
	HUINT8	notCas = 0;
	HUINT8	*p = NULL;

	vPid = 0xffff;

	if ((p = pucBuf) == NULL)
		return 0;

	secLen = Get12bit(p + 1) + 3;
	if( secLen < 8 ) return 0;


	svcId = (unsigned short)Get16bit(p + 3);

	for (scanSvcIdx=0; scanSvcIdx < nScanSvc; scanSvcIdx++)
		if (_scanSvc[scanSvcIdx].svcId == svcId)
			break;

	if (scanSvcIdx < nScanSvc)
	{
		_scanSvc[scanSvcIdx].pcrPid = Get13bit(p + 8);
	}


	VK_MEM_Memset( _scanSvc[scanSvcIdx].lan , 0 , 3 * MAX_AudioTrk );
	for(i = 0; i < MAX_AudioTrk; i++)
		_scanSvc[scanSvcIdx].soundType[i]= 0;

	dscrLoopLen = Get12bit(p + 10);

	p += 12;
	secLen -= 12 + dscrLoopLen;

	while (dscrLoopLen > 0)
	{
		dscrLen = p[1];

		switch (p[0])
		{
			case DESCRIPTOR_CA :
				_scanSvc[scanSvcIdx].cas = 1;
				break;
			default:
				break;
		}

		dscrLen += 2;
		dscrLoopLen -= dscrLen;
		p += dscrLen;
	}

	secLen -= 4;	// 4 = CRC32 field
	if( secLen <= 0 )
	{
		//Print(" secLen = %d, NO Stream Info!!\n", secLen );
	}

	while (secLen > 0)
	{
		notCas = 1;
		streamType = p[0];
		pid = Get13bit(p+1);

		ac3Des = 0;
		iso639LanDesc[0] = 0;
		isAudioStream = FALSE;

		switch (streamType)	// stream_type
		{
			case STREAM_VIDEO_MPEG1:
			case STREAM_VIDEO_MPEG2:
			case STREAM_VIDEO_MPEG4_PART2:
			case STREAM_VIDEO_MPEG4_H263:
			case STREAM_VIDEO_MPEG4_H264:
			case STREAM_VIDEO_VC1:
			case STREAM_VIDEO_VC1SM:
				if (scanSvcIdx >= nScanSvc) break;
				if( _scanSvc[scanSvcIdx].nVideoPid >= MAX_VideoTrk )
					break;
				_scanSvc[scanSvcIdx].vformat[_scanSvc[scanSvcIdx].nVideoPid] = streamType;
				_scanSvc[scanSvcIdx].videoPid[_scanSvc[scanSvcIdx].nVideoPid++] = Get13bit(p+1);
				_scanSvc[scanSvcIdx].svcType = SVC_TYPE_Tv;
				notCas = 0;

				break;

			case STREAM_AUDIO_MPEG4_AAC:
			case STREAM_AUDIO_MPEG4_AAC_PLUS:
				ac3Des = 1;
			case STREAM_AUDIO_MPEG1:
			case STREAM_AUDIO_MPEG2:
					if (scanSvcIdx >= nScanSvc) break;
					if( _scanSvc[scanSvcIdx].nAudioPid >= MAX_AudioTrk )
						break;
					_scanSvc[scanSvcIdx].aformat[_scanSvc[scanSvcIdx].nAudioPid] = streamType;
					_scanSvc[scanSvcIdx].audioPid[_scanSvc[scanSvcIdx].nAudioPid++] = Get13bit(p+1);

					{
						if( _scanSvc[scanSvcIdx].svcType == SVC_TYPE_NDef)
						{
							_scanSvc[scanSvcIdx].svcType = SVC_TYPE_Radio;
						}
					}
				notCas = 0;
				break;

			case STREAM_PRIVATE_PES:
				privatePid = Get13bit(p+1);
				break;

		}

		dscrLoopLen = Get12bit(p+3);

		p += 5;
		secLen -= 5 + dscrLoopLen;

		while (dscrLoopLen > 0)
		{
			dscrLen = p[1]; // descriptor_length

			switch (p[0])	// descriptor_tag
			{
				case DESCRIPTOR_CA :
					if( notCas )
						break;
					_scanSvc[scanSvcIdx].cas = 1;
					break;


				case DESCRIPTOR_AC3: // Reserved This is dolby digital audio description
				case DESCRIPTOR_ENHANCED_AC3: // Reserved This is dolby digital audio description
				case DESCRIPTOR_MPEG2_AAC: // Reserved This is dolby digital audio description
				case DESCRIPTOR_MPEG4_AAC_PLUS: // Reserved This is dolby digital audio description
				case DESCRIPTOR_DVB_AAC: // Reserved This is dolby digital audio description
					if (streamType == STREAM_PRIVATE_PES)
					{
						unsigned char aformat = STREAM_AUDIO_MPEG4_AC3;

						if (scanSvcIdx >= nScanSvc) break;
						if( _scanSvc[scanSvcIdx].nAudioPid >= MAX_AudioTrk )
							break;

						switch(p[0])
						{
							case DESCRIPTOR_AC3: aformat = STREAM_AUDIO_MPEG4_AC3; break;
							case DESCRIPTOR_ENHANCED_AC3: aformat = STREAM_AUDIO_MPEG4_AC3_PLUS; break;
							case DESCRIPTOR_MPEG2_AAC: aformat = STREAM_AUDIO_MPEG4_AAC; break;
							case DESCRIPTOR_MPEG4_AAC_PLUS:
							case DESCRIPTOR_DVB_AAC: aformat = STREAM_AUDIO_MPEG4_AAC_PLUS; break;
						}

						ac3Des = 1;
						isAudioStream = TRUE;
						_scanSvc[scanSvcIdx].aformat[_scanSvc[scanSvcIdx].nAudioPid] = aformat;
						_scanSvc[scanSvcIdx].audioPid[_scanSvc[scanSvcIdx].nAudioPid] = privatePid|AUDIO_PID_DOLBY;

						if(iso639LanDesc[0])
						{
							VK_memcpy( ISO_639Code, iso639LanDesc + 2, 3 );
							StrNToLower(ISO_639Code, 3);
							VK_memcpy(_scanSvc[scanSvcIdx].lan[_scanSvc[scanSvcIdx].nAudioPid], ISO_639Code, 3 );
						}

						if( _scanSvc[scanSvcIdx].svcType == SVC_TYPE_NDef )
							_scanSvc[scanSvcIdx].svcType = SVC_TYPE_Radio;
						notCas = 0;

						_scanSvc[scanSvcIdx].nAudioPid++;
					}
					break;

				case DESCRIPTOR_ISO_LAN:
					if(!isAudioStream)
					{
						unsigned int copyLen = dscrLen > 10 ? 12 : dscrLen + 2;
						VK_memcpy(iso639LanDesc, p, copyLen);
						break;
					}

					VK_memcpy( ISO_639Code, p + 2, 3 );
					StrNToLower(ISO_639Code, 3);

					VK_memcpy(_scanSvc[scanSvcIdx].lan[_scanSvc[scanSvcIdx].nAudioPid-1], ISO_639Code, 3);

					if(dscrLen >= 8 && _scanSvc[scanSvcIdx].nAudioPid < MAX_AudioTrk)
					{
						if(StrNCmp(p + 2, p + 2 + 4, 3) != 0)
						{
							unsigned char leftTrack = _scanSvc[scanSvcIdx].nAudioPid-1;
							unsigned char rightTrack = _scanSvc[scanSvcIdx].nAudioPid;

							_scanSvc[scanSvcIdx].soundModeForced[leftTrack] = SOUND_MODE_Left;

							//duplicate previous audiotrack.
							_scanSvc[scanSvcIdx].audioPid[rightTrack] = _scanSvc[scanSvcIdx].audioPid[leftTrack];
							_scanSvc[scanSvcIdx].aformat[rightTrack] = _scanSvc[scanSvcIdx].aformat[leftTrack];
							_scanSvc[scanSvcIdx].soundModeForced[rightTrack] = SOUND_MODE_Right;


							VK_memcpy( ISO_639Code, p + 2 + 4, 3 );
							StrNToLower(ISO_639Code, 3);
							VK_memcpy(_scanSvc[scanSvcIdx].lan[rightTrack], ISO_639Code, 3); // ISO_639_language_code

							_scanSvc[scanSvcIdx].nAudioPid++;
						}
					}
					break;


				default:
					break;
			}

			dscrLen += 2;
			dscrLoopLen -= dscrLen;
			p += dscrLen;
		}
	} // while

	if( _scanSvc[scanSvcIdx].svcType == SVC_TYPE_NDef )
	{
		_scanSvc[scanSvcIdx].svcType = 0xff;
		_scanSvc[scanSvcIdx].vformat[0] = 0xff;
		_scanSvc[scanSvcIdx].aformat[0] = 0xff;
		_scanSvc[scanSvcIdx].videoPid[0] = 0x2000;
		_scanSvc[scanSvcIdx].nAudioPid = 1;
		_scanSvc[scanSvcIdx].nVideoPid = 1;
		_scanSvc[scanSvcIdx].audioPid[0] = 0;
	}

	return scanSvcIdx;
} // Parse_Pmt


DI_ERR_CODE hwtest_CB_DEMUX_GetData(HUINT32 ulDemuxId, HUINT8 *pucBuf, HUINT32 ulRequestId)
{
	BSTD_UNUSED(ulDemuxId);
	BSTD_UNUSED(ulRequestId);


	if(pucBuf[0] == 0x00)
	{

		_nScanSvc = hwtest_CB_Parse_PAT(pucBuf);

	}
	else if(pucBuf[0] == 0x02)
	{

		if(_nScanSvc > 0)
		{
			_ScanIndex = hwtest_CB_Parse_PMT(pucBuf,_nScanSvc);
		}
	}

	return DI_ERR_OK;
}

DI_ERR_CODE	hwtest_CB_PSI_AllocBuffer(HUINT32 usBufLen, HUINT8 **ppucBuf)
{
	HUINT8 *pBuf = NULL;

	pBuf = DD_MEM_Alloc(usBufLen);
	if(pBuf == NULL)
		return DI_ERR_ERROR;
	*ppucBuf = pBuf;
	return DI_ERR_OK;
}

DI_ERR_CODE hwtest_StartPAT( HUINT32 demuxChannel, HUINT32 tunerNum )
{

	HUINT8 ucValue[8] = {0,};
	HUINT8 ucMask[8] = {0,};
	HUINT8 ucExclusion[8] = {0,};

	HUINT16 pid = 0x0000;
	HUINT16 tblId = 0xff00;
	DI_ERR_CODE	diErr = DI_ERR_OK;


	VK_MEM_Memset(ucValue,0x0,8);
	VK_MEM_Memset(ucMask,0x0,8);
	VK_MEM_Memset(ucExclusion,0x0,8);

	ucMask[0] = (HUINT8)( tblId >> 8 );
	ucValue[0] = (HUINT8)( tblId );

	diErr = DI_DEMUX_SetPath(demuxChannel, DI_DEMUX_INPUT_TUNER, tunerNum, DI_DEMUX_OUTPUT_DECODER, 0, -1);
	if(diErr != 0)
		return diErr;


	diErr =DI_DEMUX_SetSectionFilter( demuxChannel, pid ,
									HWTEST_PSI_REQID+hwtest_psi_reqid_idx_test++,
									FALSE,
									8,
									ucValue,
									ucMask,
									ucExclusion,
									FALSE);

	if(diErr != DI_ERR_OK)
	{
		diErr = DI_DEMUX_ReleasePath(demuxChannel);

		return diErr;
	}

	diErr =DRV_DEMUX_BackupCallback(TRUE);
	if(diErr != DI_ERR_OK)
	{
		diErr = DI_DEMUX_ReleasePath(demuxChannel);
		return diErr;
	}

	diErr = DI_DEMUX_RegisterSectionCallback(demuxChannel,hwtest_CB_DEMUX_GetData, hwtest_CB_PSI_AllocBuffer);
	if(diErr != DI_ERR_OK)
	{
		DRV_DEMUX_BackupCallback(FALSE);
		diErr = DI_DEMUX_ReleasePath(demuxChannel);
		return diErr;
	}

	return diErr;

}

DI_ERR_CODE hwtest_StartPMT( HUINT32 demuxChannel, HUINT32 tunerNum , HUINT16 pmtPID, HUINT16 svcId )
{

	HUINT8 ucValue[8] = {0,};
	HUINT8 ucMask[8] = {0,};
	HUINT8 ucExclusion[8] = {0,};

	HUINT16 pid = 0x0000;
	HUINT16 tblId = 0xff02;

	DI_ERR_CODE	diErr = DI_ERR_OK;


	VK_MEM_Memset(ucValue,0x0,8);
	VK_MEM_Memset(ucMask,0x0,8);
	VK_MEM_Memset(ucExclusion,0x0,8);

	ucMask[0]	= (unsigned char)( tblId >> 8 );
	ucValue[0] = (unsigned char)( tblId );

	ucMask[1] = 0xff;
	ucMask[2] = 0xff;
	ucValue[1] = (HUINT8)(svcId >> 8);
	ucValue[2] = (HUINT8)svcId;


	diErr = DI_DEMUX_SetPath(demuxChannel, DI_DEMUX_INPUT_TUNER, tunerNum, DI_DEMUX_OUTPUT_DECODER, 0, -1);
	if(diErr != DI_ERR_OK)
	{
		diErr = DI_DEMUX_ReleasePath(demuxChannel);

		return diErr;
	}


	diErr = DI_DEMUX_SetSectionFilter(demuxChannel,
							pmtPID,
							HWTEST_PSI_REQID+hwtest_psi_reqid_idx_test++,
							FALSE,
							8,
							ucValue,
							ucMask,
							ucExclusion,
							FALSE);
	if(diErr != DI_ERR_OK)
	{
		diErr = DI_DEMUX_ReleasePath(demuxChannel);
		return diErr;
	}


	diErr = DRV_DEMUX_BackupCallback(TRUE);
	if(diErr != DI_ERR_OK)
	{
		diErr = DI_DEMUX_ReleasePath(demuxChannel);
		return diErr;
	}


	diErr = DI_DEMUX_RegisterSectionCallback(demuxChannel,hwtest_CB_DEMUX_GetData, hwtest_CB_PSI_AllocBuffer);
	if(diErr != DI_ERR_OK)
	{
		DRV_DEMUX_BackupCallback(FALSE);
		diErr = DI_DEMUX_ReleasePath(demuxChannel);
		return diErr;
	}

	return diErr;


}

DI_ERR_CODE hwtest_StopData(HUINT32 demuxChannel)
{

	DI_ERR_CODE	diErr = DI_ERR_OK;

	diErr = DRV_DEMUX_BackupCallback(FALSE);

	hwtest_psi_reqid_idx_test--;
	if(hwtest_psi_reqid_idx_test >= 0)
		diErr = DI_DEMUX_ReleaseSectionFilter(demuxChannel, HWTEST_PSI_REQID+hwtest_psi_reqid_idx_test);
	else
		CMD_Printf("No more psi filters to release. (%d)\n",hwtest_psi_reqid_idx_test);

	diErr = DI_DEMUX_ReleasePath(demuxChannel);

	return diErr;
}


HUINT8 hwtest_FindPmtPid( HUINT32 demuxChannel, HUINT32 tunerNum, HUINT16 audioPid, HUINT16 videoPid, HUINT16 *pmtPid )
{

	HUINT16 numofSvc = 0, k	= 0,i	= 0, count = 0,bScanIndex = 0xff,fPid = 0xff;
	HUINT8 check = 0;
	DI_ERR_CODE	diErr = DI_ERR_OK;


	_nScanSvc = 0;

	hwtest_StartPAT( demuxChannel, tunerNum );

	while(1)
	{
		if(_nScanSvc > 0)
		{
			hwtest_StopData(demuxChannel);
			break;
		}
		else
		{
			count++;
			if(count == 10)
			{
				count = 0;
				break;
			}
		}

		sleep(1);
	}


	if(_nScanSvc > 0)
	{
		_ScanIndex	 = 0;
		for(k  = 0; k < _nScanSvc; k++)
		{
			hwtest_StartPMT(demuxChannel,tunerNum,_scanSvc[k].pmtPid, _scanSvc[k].svcId);

			while(1)
			{

				if(_ScanIndex != 0xff)
				{
					if(bScanIndex != _ScanIndex)
					{
						count = 0;

						bScanIndex = _ScanIndex;
						hwtest_StopData(demuxChannel);
						break;
					}
					else
					{
						count++;
						if(count == 10)
						{
							count = 0;
							break;
						}
					}
				}
				else
				{
					count++;
					if(count == 10)
					{
						count = 0;
						break;
					}
				}

				sleep(1);
			}

		}
	}

	for(k  = 0; k < _nScanSvc; k++)
	{
		for(i = 0; i < _scanSvc[k].nAudioPid; i++)
		{
			if(_scanSvc[k].audioPid[i] == audioPid)
			{
				check += 1;
				break;
			}
		}

		for(i = 0; i < _scanSvc[k].nVideoPid; i++)
		{
			if(_scanSvc[k].videoPid[i] == videoPid)
			{
				check += 1;
				break;
			}

		}

		if(check == 2)
		{
			fPid = _scanSvc[k].pmtPid;
			break;
		}

	}

	if(fPid != 0xff)
	{
		*pmtPid = fPid;
		return 0;
	}
	else
		return 1;

}


/************************************************************************************************************************************/
#if defined(CONFIG_JAPAN_SPEC) && !defined(CONFIG_TEMP_HMX4KJP)
#if defined(CONFIG_PRODUCT_IMAGE_HWTEST)
static unsigned long RecTimerId0;
static unsigned long RecTimerId1;
static unsigned long RecTimerId2;
static void cmdhwtest_rec0(unsigned long tid, void *arg)
{
    VK_printf("#######################REC0######################\n");
    PVRTest(0, 1);
}
static void cmdhwtest_rec1(unsigned long tid, void *arg)
{
    VK_printf("#######################REC1######################\n");
    PVRTest(1, 1);
}
static void cmdhwtest_rec2(unsigned long tid, void *arg)
{
    VK_printf("#######################REC2######################\n");
    PVRTest(2, 1);
}
static void cmdhwtest_recmount(unsigned long tid, void *arg)
{
    VK_printf("#######################REC2######################\n");
    PVRTest(0xff, 1);   /* 0xff means rec with mount/unmount */
}

static void cmdhwtest_aging_rec0(unsigned long tid, void *arg)
{
    int nHddNumber;

    VK_printf("#######################REC0######################\n");

    nHddNumber = *(int *)arg;
    VK_printf("nHddNumber = %d\n");

    PVRTest(0, nHddNumber);
}
static void cmdhwtest_aging_rec1(unsigned long tid, void *arg)
{
    int nHddNumber;

    VK_printf("#######################REC1######################\n");

    nHddNumber = *(int *)arg;
    VK_printf("nHddNumber = %d\n");

    PVRTest(1, nHddNumber);
}

void cmdhwtest_aging_2rec_start(int nHddNumber)
{
    CMD_RecStart(0,nHddNumber);
    CMD_RecStart(1,nHddNumber);
    VK_TIMER_EventEvery(45*1000*60, cmdhwtest_aging_rec0, (void *)&nHddNumber, sizeof(nHddNumber), &RecTimerId0);
    VK_TIMER_EventEvery(90*1000*60, cmdhwtest_aging_rec1, (void *)&nHddNumber, sizeof(nHddNumber), &RecTimerId1);
}

void cmdhwtest_aging_2rec_stop(void)
{
    CMD_RecStop(0);
    CMD_RecStop(1);
    VK_TIMER_Cancel(RecTimerId0);
    VK_TIMER_Cancel(RecTimerId1);
}
#endif
#endif


void hwtest_REC_TEST_CB(void *pvParam)
{
	return;
}

int CMD_HwTest_Parser_Pvr(void *szArgs)
{
	GET_ARGS;

	DI_ERR_CODE diErr = DI_ERR_OK;

	HUINT16 pmtPid = 0xffff;
	HUINT8 result = 0;
	int buffer[MAX_RECORDING_PATH];
	int i;

#if defined(CONFIG_JAPAN_SPEC) && !defined(CONFIG_TEMP_HMX4KJP)
#if defined(CONFIG_PRODUCT_IMAGE_HWTEST)
	if CMD_IS("rec3")
	{
		CMD_RecStart(0,1);
		CMD_RecStart(1,1);
		CMD_RecStart(2,1);
		VK_TIMER_EventEvery(60*1000*60, cmdhwtest_rec0, NULL, 0, &RecTimerId0);
		VK_TIMER_EventEvery(45*1000*60, cmdhwtest_rec1, NULL, 0, &RecTimerId1);
		VK_TIMER_EventEvery(90*1000*60, cmdhwtest_rec2, NULL, 0, &RecTimerId2);
		iResult = CMD_OK;
	}
	else if CMD_IS("rec2")
	{
		CMD_RecStart(0,1);
		CMD_RecStart(1,1);
		VK_TIMER_EventEvery(45*1000*60, cmdhwtest_rec0, NULL, 0, &RecTimerId0);
		VK_TIMER_EventEvery(90*1000*60, cmdhwtest_rec1, NULL, 0, &RecTimerId1);
		iResult = CMD_OK;
	}
	else if CMD_IS("rec1")
	{
		CMD_RecStart(0,1);
		VK_TIMER_EventEvery(45*1000*60, cmdhwtest_rec0, NULL, 0, &RecTimerId0);
		iResult = CMD_OK;
	}
	else if CMD_IS("rec_mount")
	{
		CMD_RecMountStart(0,1);
		VK_TIMER_EventEvery(10*1000*60, cmdhwtest_recmount,NULL,0,&RecTimerId0);
		iResult = CMD_OK;
	}

#if defined(CONFIG_REC_MEMIO)
	if CMD_IS("rec_memio")
	{
		CMD_RecMemioStart(0,1);
		iResult = CMD_OK;
	}
#endif
	else if CMD_IS("recstop")
	{
		CMD_RecStop(0);
		CMD_RecStop(1);
		CMD_RecStop(2);
		VK_TIMER_Cancel(RecTimerId0);
		VK_TIMER_Cancel(RecTimerId1);
		VK_TIMER_Cancel(RecTimerId2);
	}
	else
#endif
#endif
	if CMD_IS("rstart")
	{
		int deviceId, videoFormat, videoPid, audioPid, pcrPid;
		DI_PVR_REC_SETUP_t recSetup;

		if(!READABLE_IN_DEC(HWTEST_PARAM, deviceId))
		{
			iResult=CMD_ERR;
			goto TEST_PVR_EXIT;
		}
		if(!READABLE_IN_DEC(HWTEST_PARAM1, videoFormat))
		{
			iResult=CMD_ERR;
			goto TEST_PVR_EXIT;
		}
		if(!READABLE_IN_DEC(HWTEST_PARAM2, videoPid))
		{
			iResult=CMD_ERR;
			goto TEST_PVR_EXIT;
		}
		if(!READABLE_IN_DEC(HWTEST_PARAM3, audioPid))
		{
			iResult=CMD_ERR;
			goto TEST_PVR_EXIT;
		}
		if(!READABLE_IN_DEC(HWTEST_PARAM4, pcrPid))
		{
			iResult=CMD_ERR;
			goto TEST_PVR_EXIT;
		}

		if(deviceId >= MAX_REC_NUM)
		{
			CMD_Printf("Invalid device ID\n\r");
			iResult=CMD_ERR;
			goto TEST_PVR_EXIT;
		}

		VK_memset(&recSetup, 0, sizeof(DI_PVR_REC_SETUP_t));
		VK_snprintf(buffer, MAX_RECORDING_PATH-1, "%s/rec_test", RecordingPath);
		VK_memcpy((void *)recSetup.aucFileName, (void *)buffer, VK_strlen((char *)buffer)+1);

		recSetup.eFileType = eDI_PVR_FileType_Linear;
		recSetup.eStreamType = eDI_PVR_Stream_Video;
		recSetup.eVideoType = videoFormat == 0 ? DI_VIDEO_FORMAT_MPEG2 : DI_VIDEO_FORMAT_H264;
		iResult = HwTest_Pvr_RecStart(deviceId, DEFAULT_REC_DEMUX, 0, &recSetup, (HUINT16)videoPid, (HUINT16)audioPid, (HUINT16)pcrPid, pmtPid);
		if(iResult != CMD_OK)
		{
			CMD_Printf("error in HwTest_Pvr_RecStart\n\r");
			goto TEST_PVR_EXIT;
		}

		iResult = CMD_OK;
	}
#if defined(CONFIG_TEMP_MPLAY)
	else if CMD_IS("mplay")
	{
		HwTest_Pvr_FilePlay();
		iResult = CMD_OK;

	}
	else if CMD_IS("mstop")
	{
		HwTest_Pvr_FileStop();
		iResult = CMD_OK;
	}
#endif
	else if CMD_IS("rstop")
	{
		int deviceId;

		if(!READABLE_IN_DEC(HWTEST_PARAM, deviceId))
		{
			iResult=CMD_ERR;
			goto TEST_PVR_EXIT;
		}

		iResult = HwTest_Pvr_RecStop(deviceId, DEFAULT_REC_DEMUX);
		if(iResult != CMD_OK)
		{
			CMD_Printf("error in HwTest_Pvr_RecStop\n\r");
			goto TEST_PVR_EXIT;
		}

		iResult = CMD_OK;
	}
	else if CMD_IS("tsrtest")
	{
		DRV_Error drvErr = DRV_OK;
		int vkErr = VK_OK;
		int live;
		int onOff;
		unsigned short videoPid, audioPid, pcrPid, pmtPidknown;
		int videoType, audioType;
		int tunerId=0;
		DI_PVR_REC_SETUP_t recSetup;

		CMD_Printf("pvr tsrtest\n\r");

		videoPid = 0x17DE;
		audioPid = 0x17E8;
		pcrPid = 0x17DE;
		videoType = eDI_PVR_VideoType_H264;
		audioType = DI_AUDFORMAT_MPEG;
#if 1
		pmtPidknown = 0xB380;
#else
		result = hwtest_FindPmtPid(DEFAULT_REC_DEMUX, tunerId, (HUINT16)audioPid, (HUINT16)videoPid, &pmtPidknown);
#endif
		CMD_Printf("PMT PID = 0x%lx\n", pmtPidknown);
		VK_memset(&recSetup, 0, sizeof(DI_PVR_REC_SETUP_t));

		recSetup.eFileType = eDI_PVR_FileType_Circular;
		recSetup.eStreamType = eDI_PVR_Stream_Video;
		recSetup.eVideoType = videoType;

		VK_snprintf(buffer, MAX_RECORDING_PATH-1, "/mnt/hd2/hdd_tsr_test\0");
		VK_memcpy((void *)recSetup.aucFileName, (void *)buffer, VK_strlen((char *)buffer)+1);

		DI_PVR_REC_RegisterEventCallback(0,	DI_PVR_EVENT_TIMECODE_CHANGED, hwtest_REC_TEST_CB);
		iResult = HwTest_Pvr_RecStart(0, 2, (HUINT32)tunerId, &recSetup, (HUINT16)videoPid, (HUINT16)audioPid, (HUINT16)pcrPid,(HUINT16)pmtPid);
		if (iResult != CMD_OK)
		{
			CMD_Printf("error in HwTest_Pvr_RecStart\n\r");
		}

		iResult = CMD_OK;
	}
	else if CMD_IS("pbtest")
	{
		unsigned short videoPid, audioPid, pcrPid;

		DI_PVR_PLAY_SETUP_t playSetup;

		DI_VIDEO_Stop(0);
		DI_AUDIO_Stop(0);
		DI_DEMUX_StopPID(0, PID_BITMASK_AUDIO_MAIN | PID_BITMASK_VIDEO | PID_BITMASK_PCR);
		DI_DEMUX_ReleasePath(0);

		videoPid = 0x17DE;
		audioPid = 0x17E8;
		pcrPid = 0x17DE;

		VK_memset(&playSetup, 0, sizeof(DI_PVR_PLAY_SETUP_t));
		VK_snprintf(buffer, MAX_RECORDING_PATH-1, "/mnt/hd2/hdd_tsr_test\0");
		VK_memcpy((void *)playSetup.aucFileName, (void *)buffer, VK_strlen((char *)buffer)+1);
		playSetup.bIsRecording = true;
		playSetup.eFileType = eDI_PVR_FileType_Circular;
		playSetup.eStartCondition = DI_PVR_PLAY_START_MODE_BEGIN;
		playSetup.eStreamType = eDI_PVR_Stream_Video;
		playSetup.ulStartTime = 0;
		iResult = HwTest_Pvr_PlayStart(&playSetup,
									DI_VIDEO_FORMAT_H264,
									DI_AUDFORMAT_MPEG,
									(HUINT16)videoPid,
									(HUINT16)audioPid,
									(HUINT16)pcrPid);
		if(iResult != CMD_OK)
		{
			CMD_Printf("error in HwTest_Pvr_PlayStart\n\r");
		}
		iResult = CMD_OK;
	}
	else if CMD_IS("tpbtest")
	{
		unsigned short videoPid, audioPid, pcrPid;

		DI_PVR_PLAY_SETUP_t playSetup;

		DI_VIDEO_Stop(0);
		DI_AUDIO_Stop(0);
		DI_DEMUX_StopPID(0, PID_BITMASK_AUDIO_MAIN | PID_BITMASK_VIDEO | PID_BITMASK_PCR);
		DI_DEMUX_ReleasePath(0);

		videoPid = 0x1100;
		audioPid = 0x1101;
		pcrPid = 0x1102;

		VK_memset(&playSetup, 0, sizeof(DI_PVR_PLAY_SETUP_t));
		VK_snprintf(buffer, MAX_RECORDING_PATH-1, "/mnt/hd2/trans_0\0");
		VK_memcpy((void *)playSetup.aucFileName, (void *)buffer, VK_strlen((char *)buffer)+1);
		playSetup.bIsRecording = true;
		playSetup.eFileType = eDI_PVR_FileType_Linear;
		playSetup.eStartCondition = DI_PVR_PLAY_START_MODE_BEGIN;
		playSetup.eStreamType = eDI_PVR_Stream_Video;
		playSetup.ulStartTime = 7;
		iResult = HwTest_Pvr_PlayStart(&playSetup,
									DI_VIDEO_FORMAT_H264,
									DI_AUDFORMAT_AAC,
									(HUINT16)videoPid,
									(HUINT16)audioPid,
									(HUINT16)pcrPid);
		if(iResult != CMD_OK)
		{
			CMD_Printf("error in HwTest_Pvr_PlayStart\n\r");
		}
		iResult = CMD_OK;
	}
	else if CMD_IS("rdelay")
	{
		int deviceId, dlyRecStartTime;
		DI_PVR_REC_DELAYRECORD_SETUP_t dlyRecSetup;

		if(!READABLE_IN_DEC(HWTEST_PARAM, deviceId))
		{
			iResult=CMD_ERR;
			goto TEST_PVR_EXIT;
		}
		if(!READABLE_IN_DEC(HWTEST_PARAM1, dlyRecStartTime))
		{
			iResult=CMD_ERR;
			goto TEST_PVR_EXIT;
		}

		VK_memset(&dlyRecSetup, 0, sizeof(DI_PVR_REC_DELAYRECORD_SETUP_t));
		VK_snprintf(buffer, MAX_RECORDING_PATH-1, "%s/rec_test", RecordingPath);
		VK_memcpy((void *)dlyRecSetup.aucFileName, (void *)buffer, VK_strlen((char *)buffer)+1);
		VK_snprintf(buffer, MAX_RECORDING_PATH-1, "%s/tsr_temp", RecordingPath);
		VK_memcpy((void *)dlyRecSetup.aucTempFileName, (void *)buffer, VK_strlen((char *)buffer)+1);
		dlyRecSetup.ulStartTime = dlyRecStartTime;
		iResult = DI_PVR_REC_SetDelayRecording((HUINT32)deviceId, &dlyRecSetup);
		if(iResult != CMD_OK)
		{
			CMD_Printf("error in DI_PVR_REC_SetDelayRecording\n\r");
			goto TEST_PVR_EXIT;
		}

		iResult = CMD_OK;
	}
	else if CMD_IS("pstart")
	{
		int videoFormat, audioFormat, videoPid, audioPid, pcrPid;
		DI_PVR_PLAY_SETUP_t playSetup;


		if(!READABLE_IN_DEC(HWTEST_PARAM, videoFormat))
		{
			iResult=CMD_ERR;
			goto TEST_PVR_EXIT;
		}
		if(!READABLE_IN_DEC(HWTEST_PARAM1, audioFormat))
		{
			iResult=CMD_ERR;
			goto TEST_PVR_EXIT;
		}
		if(!READABLE_IN_DEC(HWTEST_PARAM2, videoPid))
		{
			iResult=CMD_ERR;
			goto TEST_PVR_EXIT;
		}
		if(!READABLE_IN_DEC(HWTEST_PARAM3, audioPid))
		{
			iResult=CMD_ERR;
			goto TEST_PVR_EXIT;
		}
		if(!READABLE_IN_DEC(HWTEST_PARAM4, pcrPid))
		{
			iResult=CMD_ERR;
			goto TEST_PVR_EXIT;
		}

		VK_memset(&playSetup, 0, sizeof(DI_PVR_PLAY_SETUP_t));
		VK_snprintf(buffer, MAX_RECORDING_PATH-1, "%s/rec_test", RecordingPath);
		VK_memcpy((void *)playSetup.aucFileName, (void *)buffer, VK_strlen((char *)buffer)+1);
		playSetup.bIsRecording = false;
		playSetup.eFileType = eDI_PVR_FileType_Linear;
		playSetup.eStartCondition = DI_PVR_PLAY_START_MODE_BEGIN;
		playSetup.eStreamType = eDI_PVR_Stream_Video;
		playSetup.ulStartTime = 0;
		iResult = HwTest_Pvr_PlayStart(&playSetup,
									videoFormat==0? DI_VIDEO_FORMAT_MPEG2 : DI_VIDEO_FORMAT_H264,
									audioFormat==0? DI_AUDFORMAT_MPEG : DI_AUDFORMAT_AC3,
									(HUINT16)videoPid,
									(HUINT16)audioPid,
									(HUINT16)pcrPid);
		if(iResult != CMD_OK)
		{
			CMD_Printf("error in HwTest_Pvr_PlayStart\n\r");
			goto TEST_PVR_EXIT;
		}

		iResult = CMD_OK;
	}
	else if CMD_IS("pstop")
	{
		iResult = HwTest_Pvr_PlayStop();
		if(iResult != CMD_OK)
		{
			CMD_Printf("error in HwTest_Pvr_PlayStop\n\r");
			goto TEST_PVR_EXIT;
		}

		iResult = CMD_OK;
	}
	else if CMD_IS("pjp")
	{
		int time;

		if(!READABLE_IN_DEC(HWTEST_PARAM, time))
		{
			iResult=CMD_ERR;
			goto TEST_PVR_EXIT;
		}

		diErr = DI_PVR_PLAY_SetPosition(DEFAULT_PLAYER, time);
		if(diErr != DI_ERR_OK)
		{
			CMD_Printf("error in DI_PVR_PLAY_SetPosition\n\r");
			iResult=CMD_ERR;
			goto TEST_PVR_EXIT;
		}

		iResult = CMD_OK;
	}
	else if CMD_IS("pfa")
	{
		int i;
		int dir_FrmNum;
		unsigned long sleepTime;
		HBOOL fwd = 1;

		if(!READABLE_IN_DEC(HWTEST_PARAM, dir_FrmNum))
		{
			dir_FrmNum = 1;
		}
		if(!READABLE_IN_DEC(HWTEST_PARAM1, sleepTime))
		{
			sleepTime = 0;
		}

		if(dir_FrmNum < 0)
		{
			fwd = 0;
			dir_FrmNum = -1*dir_FrmNum;
		}

		for(i=0; i<dir_FrmNum; i++)
		{
			diErr = DI_PVR_PLAY_FrameAdvance(DEFAULT_PLAYER, fwd);
			if(diErr != DI_ERR_OK)
			{
				CMD_Printf("error in DI_PVR_PLAY_FrameAdvance\n\r");
				iResult=CMD_ERR;
				goto TEST_PVR_EXIT;
			}
			VK_TASK_Sleep(sleepTime);
		}

		iResult = CMD_OK;
	}
	else if CMD_IS("ptr")
	{
		int speed;

		if(!READABLE_IN_DEC(HWTEST_PARAM, speed))
		{
			iResult=CMD_ERR;
			goto TEST_PVR_EXIT;
		}

		diErr = DI_PVR_PLAY_SetSpeed(DEFAULT_PLAYER, speed);
		if(diErr != DI_ERR_OK)
		{
			CMD_Printf("error in DI_PVR_PLAY_SetSpeed\n\r");
			iResult=CMD_ERR;
			goto TEST_PVR_EXIT;
		}

		iResult = CMD_OK;
	}
	else if CMD_IS("dev")
	{
		if (HWTEST_PARAM != NULL) {
			CMD_Printf("Command : pvr dev %s\n", szParam);

			if (0 == VK_strncmp("sata", szParam, 3))
			{
				VK_memcpy((void *)RecordingPath, (void *)"/mnt/hd2", VK_strlen((char *)"/mnt/hd2")+1);
			}
			else if(0 == VK_strncmp("esata", szParam, 3))
			{
				VK_memcpy((void *)RecordingPath, (void *)"/mnt/ehd2", VK_strlen((char *)"/mnt/ehd2")+1);
			}
			else if(0 == VK_strncmp("usb", szParam, 3))
			{
				VK_memcpy((void *)RecordingPath, (void *)"/mnt/media", VK_strlen((char *)"/mnt/media")+1);
			}
			else
			{
				CMD_Printf("[CMD_HwTest_Parser_Fs] Error, unknown device: right device -> [sata|esata|usb)]\r\n");
			}
		} else {
			CMD_Printf("[CMD_HwTest_Parser_Fs] Error, unknown action: right action -> pvr dev [sata|esata|usb)]\r\n");
		}

	}
	else if CMD_IS("testhdd")
	{
#if defined(CONFIG_PRODUCT_IMAGE_HWTEST)
		DRV_Error drvErr = DRV_OK;
		int vkErr = VK_OK;
		int live;
		int onOff;
		int videoPid, audioPid, pcrPid;
		int videoType, audioType;
		int intExtHdd=0;
		int FileType=0;
		int EncryptType=0;
		int tunerId;
		DI_PVR_PLAY_SETUP_t playSetup;
		DI_PVR_REC_SETUP_t recSetup;
		CMD_HWTEST_PVR_MSG callbackMsg;
		unsigned char tempString[100] = { 0, };

		if(!READABLE_IN_DEC(HWTEST_PARAM, onOff))
		{
			iResult=CMD_ERR;
			goto TEST_PVR_EXIT;
		}

		if(onOff == 2)
		{
			VK_snprintf(buffer, MAX_RECORDING_PATH-1, "cp /media/hdd_read_test.nts %s/", RecordingPath);
			vkErr = VK_SYSTEM_Command(buffer);
			if(vkErr != VK_OK)
			{
				CMD_Printf("error in VK_SYSTEM_Command\n\r");
				iResult=CMD_ERR;
				goto TEST_PVR_EXIT;
			}
			VK_snprintf(buffer, MAX_RECORDING_PATH-1, "cp /media/hdd_read_test.ts %s/", RecordingPath);
			vkErr = VK_SYSTEM_Command(buffer);
			if(vkErr != VK_OK)
			{
				CMD_Printf("error in VK_SYSTEM_Command\n\r");
				iResult=CMD_ERR;
				goto TEST_PVR_EXIT;
			}
			if(pvrEvtCbFunc[CMD_HWTEST_PVR_EVENT_COPY_DONE] != NULL)
			{
				callbackMsg.ulEvent = CMD_HWTEST_PVR_EVENT_COPY_DONE;
				pvrEvtCbFunc[CMD_HWTEST_PVR_EVENT_COPY_DONE]((void *)&callbackMsg);
			}

			iResult=CMD_OK;
			goto TEST_PVR_EXIT;
		}
		else if(onOff == 3 && hddTestStatus != CMD_HDD_TEST_NONE) /* Live인 경우 playback switch */
		{
			if(!READABLE_IN_DEC(HWTEST_PARAM1, FileType))
			{
				iResult=CMD_ERR;
				goto TEST_PVR_EXIT;
			}

			if(!READABLE_IN_DEC(HWTEST_PARAM2, EncryptType))
			{
				iResult=CMD_ERR;
				goto TEST_PVR_EXIT;
			}

			/* Stop Playback */
			iResult = HwTest_Pvr_PlayStop();
			if(iResult != CMD_OK)
			{
				CMD_Printf("error in HwTest_Pvr_PlayStop\n\r");
				goto TEST_PVR_EXIT;
			}

			/* Restart Playback */
			VK_memset(&playSetup, 0, sizeof(DI_PVR_PLAY_SETUP_t));

			switch (hddTestRecInfo[hddTestNextRecIdForPbSw - 1].extHdd)
			{
				case 0:
					/* Internal */
					VK_snprintf(buffer, MAX_RECORDING_PATH-1, "/mnt/hd2/hdd_write_test%d", hddTestNextRecIdForPbSw);
					VK_memcpy((void *)playSetup.aucFileName, (void *)buffer, VK_strlen((char *)buffer)+1);
					break;

				case 1:
					/* External */
					VK_snprintf(buffer, MAX_RECORDING_PATH-1, "/media/hdd_write_test%d", hddTestNextRecIdForPbSw);
					VK_memcpy((void *)playSetup.aucFileName, (void *)buffer, VK_strlen((char *)buffer)+1);
					break;

				case 2:
					/* Detachable */
					VK_snprintf(buffer, MAX_RECORDING_PATH-1, "/mnt/ehd2/hdd_write_test%d", hddTestNextRecIdForPbSw);
					VK_memcpy((void *)playSetup.aucFileName, (void *)buffer, VK_strlen((char *)buffer)+1);
					break;

				case 3:
					/* Internal + External */
					if ((hddTestNextRecIdForPbSw % 2) == 0)
					{
						/* Detachable */
						VK_snprintf(buffer, MAX_RECORDING_PATH-1, "/mnt/ehd2/hdd_write_test%d", hddTestNextRecIdForPbSw);
					}
					else
					{
						/* Internal */
						VK_snprintf(buffer, MAX_RECORDING_PATH-1, "/mnt/hd2/hdd_write_test%d", hddTestNextRecIdForPbSw);
					}
					VK_memcpy((void *)playSetup.aucFileName, (void *)buffer, VK_strlen((char *)buffer)+1);
					break;

				default:
					iResult = CMD_ERR;
					goto TEST_PVR_EXIT;
			}

			playSetup.bIsRecording = true;
			playSetup.eFileType = FileType==0 ? eDI_PVR_FileType_Circular : eDI_PVR_FileType_Linear;
			g_erEncryptyinType = EncryptType==1 ? DI_PVR_ENCRYPTION_TYPE_3DES : EncryptType==2 ? DI_PVR_ENCRYPTION_TYPE_AES : EncryptType==3 ? DI_PVR_ENCRYPTION_TYPE_3DES_SECURED_KEY : EncryptType==4 ? DI_PVR_ENCRYPTION_TYPE_AES_SECURED_KEY : DI_PVR_ENCRYPTION_TYPE_NONE;
			playSetup.eStartCondition = DI_PVR_PLAY_START_MODE_BEGIN;
			playSetup.eStreamType = eDI_PVR_Stream_Video;
			playSetup.ulStartTime = hddTestNextRecIdForPbSw; /* eFileType이 eDI_PVR_FileType_Circular인 경우에는 ulStartTime에 관련 record ID를 넣어준다.(only for command) */
			iResult = HwTest_Pvr_PlayStart(&playSetup,
										hddTestRecInfo[hddTestNextRecIdForPbSw - 1].videotype,
										hddTestRecInfo[hddTestNextRecIdForPbSw - 1].audiotype,
										hddTestRecInfo[hddTestNextRecIdForPbSw - 1].videoPid,
										hddTestRecInfo[hddTestNextRecIdForPbSw - 1].audioPid,
										hddTestRecInfo[hddTestNextRecIdForPbSw - 1].pcrPid);
			if(iResult != CMD_OK)
			{
				CMD_Printf("error in HwTest_Pvr_PlayStart(%d)\n\r", hddTestNextRecIdForPbSw - 1);
				goto TEST_PVR_EXIT;
			}

			CMD_Printf("HwTest_Pvr_PlayStart(%d) OK, path(%s)\n", hddTestNextRecIdForPbSw - 1, playSetup.aucFileName);

			VK_snprintf(tempString, 100, "Record(%d) playback started\n", hddTestNextRecIdForPbSw - 1);
			(int *)(CMD_GetMsgUI())(tempString);

			hddTestNextRecIdForPbSw = (hddTestNextRecIdForPbSw % RECORDING_DEVICE_NUM) + 1;

			iResult=CMD_OK;
			goto TEST_PVR_EXIT;
		}

		if(!READABLE_IN_DEC(HWTEST_PARAM1, live))
		{
			iResult=CMD_ERR;
			goto TEST_PVR_EXIT;
		}

		if (onOff)
		{
			if (!READABLE_IN_DEC(HWTEST_PARAM2, videoPid))
			{
				iResult=CMD_ERR;
				goto TEST_PVR_EXIT;
			}

			if (!READABLE_IN_DEC(HWTEST_PARAM3, audioPid))
			{
				iResult=CMD_ERR;
				goto TEST_PVR_EXIT;
			}

			if (!READABLE_IN_DEC(HWTEST_PARAM4, pcrPid))
			{
				iResult=CMD_ERR;
				goto TEST_PVR_EXIT;
			}

			if (!READABLE_IN_DEC(HWTEST_PARAM5, intExtHdd))
			{
				iResult=CMD_ERR;
				goto TEST_PVR_EXIT;
			}

			if (!READABLE_IN_DEC(HWTEST_PARAM6, videoType))
			{
				iResult=CMD_ERR;
				goto TEST_PVR_EXIT;
			}

			if ( videoType == 0 )
			{
				videoType = eDI_PVR_VideoType_Mpeg2;
			}
			else if ( videoType == 1 )
			{
				videoType = eDI_PVR_VideoType_H264;
			}
			else
			{
				videoType = eDI_PVR_VideoType_H265;
			}

			if (!READABLE_IN_DEC(HWTEST_PARAM7, audioType))
			{
				iResult=CMD_ERR;
				goto TEST_PVR_EXIT;
			}

			if (audioType == 0)
				audioType = DI_AUDFORMAT_MPEG;
			else if (audioType == 1)
				audioType = DI_AUDFORMAT_MP3;
			else if (audioType == 2)
			 	audioType = DI_AUDFORMAT_AAC;
			else if (audioType == 3)
			 	audioType = DI_AUDFORMAT_AAC_PLUS;
			else if (audioType == 4)
			 	audioType = DI_AUDFORMAT_AC3;
			else if (audioType == 5)
			 	audioType = DI_AUDFORMAT_AC3_PLUS;
			else if (audioType == 6)
			 	audioType = DI_AUDFORMAT_PCM;

			/* Live test인 경우 처음 onOff==1일 때 -> CMD_HDD_TEST_2REC_1PB, 다시 onOff==1일 때 -> CMD_HDD_TEST_CHANGE_REC_PB,
				이후 onOff==1이면 계속 CMD_HDD_TEST_CHANGE_REC_PB status를 유지.
			 	즉, onOff==0일 때 까지 2nd record, playback을 계속 restart */
			if (live == 1)
			{
				if (!READABLE_IN_DEC(HWTEST_PARAM8, tunerId))
				{
					iResult = CMD_ERR;

					goto TEST_PVR_EXIT;
				}

				if (!READABLE_IN_DEC(HWTEST_PARAM9, FileType))
				{
					iResult=CMD_ERR;
					goto TEST_PVR_EXIT;
				}

				if (!READABLE_IN_DEC(HWTEST_PARAM10, EncryptType))
				{
					iResult=CMD_ERR;
					goto TEST_PVR_EXIT;
				}

				if (hddTestStatus == CMD_HDD_TEST_NONE)
				{
					/* 해당 video/audio의 PMT PID를 가져오는 함수 추가 */
					result = hwtest_FindPmtPid(DEFAULT_REC_DEMUX, tunerId, (HUINT16)audioPid, (HUINT16)videoPid, &pmtPid);

					VK_memset(&recSetup, 0, sizeof(DI_PVR_REC_SETUP_t));

					recSetup.eFileType = FileType==0 ? eDI_PVR_FileType_Circular : eDI_PVR_FileType_Linear;
					g_erEncryptyinType = EncryptType==1 ? DI_PVR_ENCRYPTION_TYPE_3DES : EncryptType==2 ? DI_PVR_ENCRYPTION_TYPE_AES : EncryptType==3 ? DI_PVR_ENCRYPTION_TYPE_3DES_SECURED_KEY : EncryptType==4 ? DI_PVR_ENCRYPTION_TYPE_AES_SECURED_KEY : DI_PVR_ENCRYPTION_TYPE_NONE;

					recSetup.eStreamType = eDI_PVR_Stream_Video;
					recSetup.eVideoType = videoType;

					switch(intExtHdd)
					{
					case 4: /* emmc for tsr */
						/* delete tsr files */
						(void)VK_SYSTEM_Command("rm -rf /mnt/sd0/hdd_write_test*");

						recSetup.stUsbFlash.bIsUsbFlashRecord = TRUE;
						recSetup.stUsbFlash.ulLimitTimeSecond = 600; /* (10 min) */
#if	defined(CONFIG_LOCAL_TIMESTAMP_ENABLED_RECORD)
						recSetup.stUsbFlash.ullLimitDataSize = 1572865152; /* 18048 * 87149 */
#else
						recSetup.stUsbFlash.ullLimitDataSize = 1572525248; /* 17672 * 88984 */
#endif
						recSetup.stUsbFlash.ullLimitIndexSize = 17*1024*1024;
						break;
					default:
						break;
					}

					for (i = 0; i < RECORDING_DEVICE_NUM; i++)
					{
						switch (intExtHdd)
						{
							case 0:
								/* Internal */
								VK_snprintf(buffer, MAX_RECORDING_PATH-1, "/mnt/hd2/hdd_write_test%d", i + 1);
								VK_memcpy((void *)recSetup.aucFileName, (void *)buffer, VK_strlen((char *)buffer)+1);
								break;

							case 1:
								/* External */
								VK_snprintf(buffer, MAX_RECORDING_PATH-1, "/media/hdd_write_test%d", i + 1);
								VK_memcpy((void *)recSetup.aucFileName, (void *)buffer, VK_strlen((char *)buffer)+1);
								break;

							case 2:
								/* Detachable */
								VK_snprintf(buffer, MAX_RECORDING_PATH-1, "/mnt/ehd2/hdd_write_test%d", i + 1);
								VK_memcpy((void *)recSetup.aucFileName, (void *)buffer, VK_strlen((char *)buffer)+1);
								break;

							case 3:
								/* Internal + External */
								if (((i + 1) % 2) == 0)
								{
									/* Detachable */
									VK_snprintf(buffer, MAX_RECORDING_PATH-1, "/mnt/ehd2/hdd_write_test%d", i + 1);
								}
								else
								{
									/* Internal */
									VK_snprintf(buffer, MAX_RECORDING_PATH-1, "/mnt/hd2/hdd_write_test%d", i + 1);
								}
								VK_memcpy((void *)recSetup.aucFileName, (void *)buffer, VK_strlen((char *)buffer)+1);
								break;

							case 4: /* emmc for tsr */
								VK_snprintf(buffer, MAX_RECORDING_PATH-1, "/mnt/sd0/hdd_write_test%d", i + 1);
								VK_memcpy((void *)recSetup.aucFileName, (void *)buffer, VK_strlen((char *)buffer)+1);
								break;

							default:
								iResult = CMD_ERR;

								goto TEST_PVR_EXIT;
						}

						iResult = HwTest_Pvr_RecStart(i + 1, i + 2, (HUINT32)tunerId, &recSetup, (HUINT16)videoPid, (HUINT16)audioPid, (HUINT16)pcrPid,(HUINT16)pmtPid);
						if (iResult != CMD_OK)
						{
							CMD_Printf("error in HwTest_Pvr_RecStart(%d)\n\r", i);

							goto TEST_PVR_EXIT;
						}

						CMD_Printf("HwTest_Pvr_RecStart(%d) OK, path(%s)\n", i, recSetup.aucFileName);

						hddTestRecInfo[i].videoPid = videoPid;
						hddTestRecInfo[i].audioPid = audioPid;
						hddTestRecInfo[i].pcrPid = pcrPid;
						hddTestRecInfo[i].videotype = videoType;
						hddTestRecInfo[i].audiotype = audioType;
						hddTestRecInfo[i].extHdd = intExtHdd;
					}

					VK_TASK_Sleep(10000); /* Recorded data가 어느 정도 쌓일 때까지 wait.. */

					VK_memset(&playSetup, 0, sizeof(DI_PVR_PLAY_SETUP_t));

					switch (intExtHdd)
					{
						case 0:
							/* Internal */
							VK_snprintf(buffer, MAX_RECORDING_PATH-1, "/mnt/hd2/hdd_write_test1");
							VK_memcpy((void *)playSetup.aucFileName, (void *)buffer, VK_strlen((char *)buffer)+1);
							break;

						case 1:
							/* External */
							VK_snprintf(buffer, MAX_RECORDING_PATH-1, "/media/hdd_write_test1");
							VK_memcpy((void *)playSetup.aucFileName, (void *)buffer, VK_strlen((char *)buffer)+1);
							break;

						case 2:
							/* Detachable */
							VK_snprintf(buffer, MAX_RECORDING_PATH-1, "/mnt/ehd2/hdd_write_test1");
							VK_memcpy((void *)playSetup.aucFileName, (void *)buffer, VK_strlen((char *)buffer)+1);
							break;

						case 3:
							/* Internal + External */
							VK_snprintf(buffer, MAX_RECORDING_PATH-1, "/mnt/hd2/hdd_write_test1");
							VK_memcpy((void *)playSetup.aucFileName, (void *)buffer, VK_strlen((char *)buffer)+1);
							break;

						case 4: /* emmc for tsr */
							playSetup.bIsUsbFlashRecord = TRUE;
							VK_snprintf(buffer, MAX_RECORDING_PATH-1, "/mnt/sd0/hdd_write_test1");
							VK_memcpy((void *)playSetup.aucFileName, (void *)buffer, VK_strlen((char *)buffer)+1);
							break;

						default:
							iResult = CMD_ERR;

							goto TEST_PVR_EXIT;
					}

					playSetup.bIsRecording = true;
					playSetup.eFileType = FileType==0 ? eDI_PVR_FileType_Circular : eDI_PVR_FileType_Linear;
					g_erEncryptyinType = EncryptType==1 ? DI_PVR_ENCRYPTION_TYPE_3DES : EncryptType==2 ? DI_PVR_ENCRYPTION_TYPE_AES : EncryptType==3 ? DI_PVR_ENCRYPTION_TYPE_3DES_SECURED_KEY : EncryptType==4 ? DI_PVR_ENCRYPTION_TYPE_AES_SECURED_KEY : DI_PVR_ENCRYPTION_TYPE_NONE;

					playSetup.eStartCondition = DI_PVR_PLAY_START_MODE_BEGIN;
					playSetup.eStreamType = eDI_PVR_Stream_Video;
					playSetup.ulStartTime = HDD_TEST_REC1_DEV_ID; /* eFileType이 eDI_PVR_FileType_Circular인 경우에는 ulStartTime에 관련 record ID를 넣어준다.(only for command) */

					iResult = HwTest_Pvr_PlayStart(&playSetup,
												videoType,
												audioType,
												(HUINT16)videoPid,
												(HUINT16)audioPid,
												(HUINT16)pcrPid);
					if (iResult != CMD_OK)
					{
						CMD_Printf("error in HwTest_Pvr_PlayStart\n\r");

						goto TEST_PVR_EXIT;
					}

					VK_snprintf(tempString, 100, "Record(%d) playback started\n", HDD_TEST_REC1_DEV_ID - 1);
					(int *)(CMD_GetMsgUI())(tempString);

					hddTestStatus = CMD_HDD_TEST_2REC_1PB;
					hddTestNextRecIdForPbSw = HDD_TEST_REC2_DEV_ID;
				}
				else if (hddTestStatus == CMD_HDD_TEST_2REC_1PB || CMD_HDD_TEST_CHANGE_REC_PB)
				{
					/* Stop Playback */
					iResult = HwTest_Pvr_PlayStop();
					if (iResult != CMD_OK)
					{
						CMD_Printf("error in HwTest_Pvr_PlayStop\n\r");

						goto TEST_PVR_EXIT;
					}

					/* Stop REC2 */
					iResult = HwTest_Pvr_RecStop(HDD_TEST_REC2_DEV_ID, SECOND_REC_DEMUX);
					if (iResult != CMD_OK)
					{
						CMD_Printf("error in HwTest_Pvr_RecStop\n\r");

						goto TEST_PVR_EXIT;
					}

					/* Restart REC2 */
					VK_memset(&recSetup, 0, sizeof(DI_PVR_REC_SETUP_t));

					switch(intExtHdd)
					{
					case 0:
						VK_snprintf(buffer, MAX_RECORDING_PATH-1, "%s/hdd_write_test2", RecordingPath);
						VK_memcpy((void *)recSetup.aucFileName, (void *)buffer, VK_strlen((char *)buffer)+1);
						break;
					case 4:
						recSetup.stUsbFlash.bIsUsbFlashRecord = TRUE;
						VK_snprintf(buffer, MAX_RECORDING_PATH-1, "/mnt/sd0/hdd_write_test2");
						VK_memcpy((void *)recSetup.aucFileName, (void *)buffer, VK_strlen((char *)buffer)+1);
						break;
					default:
						VK_memcpy((void *)recSetup.aucFileName, (void *)"/media/hdd_write_test2", VK_strlen((char *)"/media/hdd_write_test2")+1);
						break;
					}

					recSetup.eFileType = FileType==0 ? eDI_PVR_FileType_Circular : eDI_PVR_FileType_Linear;
					g_erEncryptyinType = EncryptType==1 ? DI_PVR_ENCRYPTION_TYPE_3DES : EncryptType==2 ? DI_PVR_ENCRYPTION_TYPE_AES : EncryptType==3 ? DI_PVR_ENCRYPTION_TYPE_3DES_SECURED_KEY : EncryptType==4 ? DI_PVR_ENCRYPTION_TYPE_AES_SECURED_KEY : DI_PVR_ENCRYPTION_TYPE_NONE;

					recSetup.eStreamType = eDI_PVR_Stream_Video;
					recSetup.eVideoType = videoType;
					iResult = HwTest_Pvr_RecStart(HDD_TEST_REC2_DEV_ID, SECOND_REC_DEMUX, (HUINT32)tunerId, &recSetup, (HUINT16)videoPid, (HUINT16)audioPid, (HUINT16)pcrPid,pmtPid);
					if (iResult != CMD_OK)
					{
						CMD_Printf("error in HwTest_Pvr_RecStart\n\r");

						goto TEST_PVR_EXIT;
					}

					/* Restart Playback */
					VK_TASK_Sleep(10000); /* Recorded data가 어느 정도 쌓일 때까지 wait.. */

					VK_memset(&playSetup, 0, sizeof(DI_PVR_PLAY_SETUP_t));

					playSetup.bIsRecording = true;
					playSetup.eFileType = FileType==0 ? eDI_PVR_FileType_Circular : eDI_PVR_FileType_Linear;
					g_erEncryptyinType = EncryptType==1 ? DI_PVR_ENCRYPTION_TYPE_3DES : EncryptType==2 ? DI_PVR_ENCRYPTION_TYPE_AES : EncryptType==3 ? DI_PVR_ENCRYPTION_TYPE_3DES_SECURED_KEY : EncryptType==4 ? DI_PVR_ENCRYPTION_TYPE_AES_SECURED_KEY : DI_PVR_ENCRYPTION_TYPE_NONE;

					playSetup.eStartCondition = DI_PVR_PLAY_START_MODE_BEGIN;
					playSetup.eStreamType = eDI_PVR_Stream_Video;
					playSetup.ulStartTime = HDD_TEST_REC2_DEV_ID; /* eFileType이 eDI_PVR_FileType_Circular인 경우에는 ulStartTime에 관련 record ID를 넣어준다.(only for command) */
					switch(intExtHdd)
					{
					case 0:
						VK_snprintf(buffer, MAX_RECORDING_PATH-1, "%s/hdd_write_test2", RecordingPath);
						VK_memcpy((void *)playSetup.aucFileName, (void *)buffer, VK_strlen((char *)buffer)+1);
						break;
					case 4:
						playSetup.bIsUsbFlashRecord = TRUE;
						VK_snprintf(buffer, MAX_RECORDING_PATH-1, "/mnt/sd0/hdd_write_test2");
						VK_memcpy((void *)playSetup.aucFileName, (void *)buffer, VK_strlen((char *)buffer)+1);
						break;
					default:
						VK_memcpy((void *)playSetup.aucFileName, (void *)"/media/hdd_write_test2", VK_strlen((char *)"/media/hdd_write_test2")+1);
						break;
					}

					iResult = HwTest_Pvr_PlayStart(&playSetup,
												videoType,
												audioType,
												(HUINT16)videoPid,
												(HUINT16)audioPid,
												(HUINT16)pcrPid);
					if (iResult != CMD_OK)
					{
						CMD_Printf("error in HwTest_Pvr_PlayStart\n\r");

						goto TEST_PVR_EXIT;
					}

					hddTestStatus = CMD_HDD_TEST_CHANGE_REC_PB;
					hddTestNextRecIdForPbSw = HDD_TEST_REC1_DEV_ID;

					hddTestRecInfo[1].videoPid = videoPid;
					hddTestRecInfo[1].audioPid = audioPid;
					hddTestRecInfo[1].pcrPid = pcrPid;
					hddTestRecInfo[1].extHdd = intExtHdd;
				}
			}
			else if (live == 2) /*External 1PB with no record*/
			{
				vkErr = VK_EVENT_Create(&cmdPbEventId, "cmdPbEvt");
				if (vkErr != VK_OK)
				{
					CMD_Printf("error in VK_EVENT_Create\n\r");

					iResult=CMD_ERR;

					goto TEST_PVR_EXIT;
				}

				vkErr = VK_TASK_Create(HwTestPvr_PlayEventTask,
									PLAYBACK_EVENT_TASK_PRIORITY,
									PLAYBACK_EVENT_TASK_STACK_SIZE,
									"cmdPb",
									NULL,
									&cmdPbTaskId,
									false);
				if (vkErr != VK_OK)
				{
					CMD_Printf("error in VK_TASK_Create\n\r");

					iResult=CMD_ERR;

					goto TEST_PVR_EXIT;
				}

				VK_TASK_Start(cmdPbTaskId);

				drvErr = DRV_PVR_PLAY_BackupCallback(DEFAULT_PLAYER, TRUE);
				if (drvErr != DRV_OK)
				{
					CMD_Printf("error in DRV_PVR_PLAY_BackupCallback\n\r");

					iResult=CMD_ERR;

					goto TEST_PVR_EXIT;
				}

				diErr = DI_PVR_PLAY_RegisterEventCallback(DEFAULT_PLAYER, DI_PVR_EVENT_EOF, HwTest_Pvr_PlayEofCallback);
				if (diErr != DI_ERR_OK)
				{
					CMD_Printf("error in DI_PVR_PLAY_Setup\n\r");

					iResult=CMD_ERR;

					goto TEST_PVR_EXIT;
				}

				VK_memset(&playSetup, 0, sizeof(DI_PVR_PLAY_SETUP_t));

				if (intExtHdd == 0)
				{
					VK_memcpy((void *)playSetup.aucFileName, (void *)"/mnt/hd2/hdd_read_test", VK_strlen((char *)"/mnt/hd2/hdd_read_test")+1);
				}
				else if (intExtHdd == 1)
				{
					VK_memcpy((void *)playSetup.aucFileName, (void *)"/media/hdd_read_test", VK_strlen((char *)"/media/hdd_read_test")+1);
				}
				else if (intExtHdd == 2)
				{
					VK_memcpy((void *)playSetup.aucFileName, (void *)"/mnt/ehd2/hdd_read_test", VK_strlen((char *)"/mnt/ehd2/hdd_read_test")+1);
				}
				else
					goto TEST_PVR_EXIT;

				playSetup.bIsRecording = false;
				playSetup.eFileType = eDI_PVR_FileType_Linear;
				playSetup.eStartCondition = DI_PVR_PLAY_START_MODE_BEGIN;
				playSetup.eStreamType = eDI_PVR_Stream_Video;
				playSetup.ulStartTime = 0;
				iResult = HwTest_Pvr_PlayStart(&playSetup,
											videoType,
											audioType,
											(HUINT16)videoPid,
											(HUINT16)audioPid,
											(HUINT16)pcrPid);
				if (iResult != CMD_OK)
				{
					CMD_Printf("error in HwTest_Pvr_PlayStart\n\r");

					goto TEST_PVR_EXIT;
				}
			}
			else /*External 1PB+2REC*/
			{
				vkErr = VK_EVENT_Create(&cmdPbEventId, "cmdPbEvt");
				if (vkErr != VK_OK)
				{
					CMD_Printf("error in VK_EVENT_Create\n\r");

					iResult=CMD_ERR;

					goto TEST_PVR_EXIT;
				}

				vkErr = VK_TASK_Create(HwTestPvr_PlayEventTask,
									PLAYBACK_EVENT_TASK_PRIORITY,
									PLAYBACK_EVENT_TASK_STACK_SIZE,
									"cmdPb",
									NULL,
									&cmdPbTaskId,
									false);
				if (vkErr != VK_OK)
				{
					CMD_Printf("error in VK_TASK_Create\n\r");

					iResult=CMD_ERR;

					goto TEST_PVR_EXIT;
				}

				VK_TASK_Start(cmdPbTaskId);

				drvErr = DRV_PVR_PLAY_BackupCallback(DEFAULT_PLAYER, TRUE);
				if (drvErr != DRV_OK)
				{
					CMD_Printf("error in DRV_PVR_PLAY_BackupCallback\n\r");

					iResult=CMD_ERR;

					goto TEST_PVR_EXIT;
				}

				diErr = DI_PVR_PLAY_RegisterEventCallback(DEFAULT_PLAYER, DI_PVR_EVENT_EOF, HwTest_Pvr_PlayEofCallback);
				if (diErr != DI_ERR_OK)
				{
					CMD_Printf("error in DI_PVR_PLAY_Setup\n\r");

					iResult=CMD_ERR;

					goto TEST_PVR_EXIT;
				}

				VK_memset(&playSetup, 0, sizeof(DI_PVR_PLAY_SETUP_t));

				if (intExtHdd == 0)
				{
					VK_snprintf(buffer, MAX_RECORDING_PATH-1, "%s/hdd_read_test", RecordingPath);
					VK_memcpy((void *)playSetup.aucFileName, (void *)buffer, VK_strlen((char *)buffer)+1);
				}
				else if (intExtHdd == 1)
					VK_memcpy((void *)playSetup.aucFileName, (void *)"/media/hdd_read_test", VK_strlen((char *)"/media/hdd_read_test")+1);
				else
					goto TEST_PVR_EXIT;

				playSetup.bIsRecording = false;
				playSetup.eFileType = eDI_PVR_FileType_Linear;
				playSetup.eStartCondition = DI_PVR_PLAY_START_MODE_BEGIN;
				playSetup.eStreamType = eDI_PVR_Stream_Video;
				playSetup.ulStartTime = 0;
				iResult = HwTest_Pvr_PlayStart(&playSetup,
											videoType,
											audioType,
											(HUINT16)videoPid,
											(HUINT16)audioPid,
											(HUINT16)pcrPid);
				if (iResult != CMD_OK)
				{
					CMD_Printf("error in HwTest_Pvr_PlayStart\n\r");

					goto TEST_PVR_EXIT;
				}

				VK_memset(&recSetup, 0, sizeof(DI_PVR_REC_SETUP_t));

				if (intExtHdd == 0)
				{
					VK_snprintf(buffer, MAX_RECORDING_PATH-1, "%s/hdd_write_test1", RecordingPath);
					VK_memcpy((void *)recSetup.aucFileName, (void *)buffer, VK_strlen((char *)buffer)+1);
				}
				else if (intExtHdd == 1)
					VK_memcpy((void *)recSetup.aucFileName, (void *)"/media/hdd_write_test1", VK_strlen((char *)"/media/hdd_write_test1")+1);
				else
					goto TEST_PVR_EXIT;

				recSetup.eFileType = eDI_PVR_FileType_Circular;
				recSetup.eStreamType = eDI_PVR_Stream_Video;
				recSetup.eVideoType = videoType;
				iResult = HwTest_Pvr_RecStart(HDD_TEST_REC1_DEV_ID, DRV_DEMUX_GetFirstPlaybackParserIndex(), 0, &recSetup, (HUINT16)videoPid, (HUINT16)audioPid, (HUINT16)pcrPid,pmtPid);
				if (iResult != CMD_OK)
				{
					CMD_Printf("error in HwTest_Pvr_RecStart\n\r");

					goto TEST_PVR_EXIT;
				}

				if (intExtHdd == 0)
				{
					VK_snprintf(buffer, MAX_RECORDING_PATH-1, "%s/hdd_write_test2", RecordingPath);
					VK_memcpy((void *)recSetup.aucFileName, (void *)buffer, VK_strlen((char *)buffer)+1);
				}
				else if (intExtHdd == 1)
					VK_memcpy((void *)recSetup.aucFileName, (void *)"/media/hdd_write_test2", VK_strlen((char *)"/media/hdd_write_test2")+1);
				else
					goto TEST_PVR_EXIT;

				iResult = HwTest_Pvr_RecStart(HDD_TEST_REC2_DEV_ID, DRV_DEMUX_GetFirstPlaybackParserIndex(), 0, &recSetup, (HUINT16)videoPid, (HUINT16)audioPid, (HUINT16)pcrPid,pmtPid);
				if (iResult != CMD_OK)
				{
					CMD_Printf("error in HwTest_Pvr_RecStart\n\r");

					goto TEST_PVR_EXIT;
				}
			}

			iResult = CMD_OK;
		}
		else
		{
			if (live)
			{
				iResult = HwTest_Pvr_PlayStop();
				if (iResult != CMD_OK)
				{
					CMD_Printf("error in HwTest_Pvr_PlayStop\n\r");

					goto TEST_PVR_EXIT;
				}

				for (i = 0; i < RECORDING_DEVICE_NUM; i++)
				{
					iResult = HwTest_Pvr_RecStop(i + 1, i + 2);
					if (iResult != CMD_OK)
					{
						CMD_Printf("error in HwTest_Pvr_RecStop, device ID(%d)\n\r", i + 1);

						return iResult;
					}
				}

				hddTestStatus = CMD_HDD_TEST_NONE;
			}
			else
			{
				iResult = HwTest_Pvr_RecStop(HDD_TEST_REC1_DEV_ID, DRV_DEMUX_GetFirstPlaybackParserIndex());
				if (iResult != CMD_OK)
				{
					CMD_Printf("error in HwTest_Pvr_RecStop\n\r");

					goto TEST_PVR_EXIT;
				}

				iResult = HwTest_Pvr_RecStop(HDD_TEST_REC2_DEV_ID, DRV_DEMUX_GetFirstPlaybackParserIndex());
				if (iResult != CMD_OK)
				{
					CMD_Printf("error in HwTest_Pvr_RecStop\n\r");

					goto TEST_PVR_EXIT;
				}
				if (iResult != CMD_OK)
				{
					CMD_Printf("error in HwTest_Pvr_PlayStop\n\r");

					goto TEST_PVR_EXIT;
				}

				drvErr = DRV_PVR_PLAY_BackupCallback(DEFAULT_PLAYER, FALSE);
				if (drvErr != DRV_OK)
				{
					CMD_Printf("error in DRV_PVR_PLAY_BackupCallback\n\r");

					iResult=CMD_ERR;

					goto TEST_PVR_EXIT;
				}

				taskDestroyProcess = true;

				VK_EVENT_Send(cmdPbEventId, CMD_PB_TASK_DESTROY);

				while (taskDestroyProcess == true)
				{
					VK_TASK_Sleep(10);
				}

				VK_TASK_Stop(cmdPbTaskId);

				vkErr = VK_TASK_Destroy(cmdPbTaskId);

				if (vkErr != VK_OK)
				{
					CMD_Printf("error in VK_TASK_Destroy\n\r");

					iResult=CMD_ERR;

					goto TEST_PVR_EXIT;
				}

				VK_EVENT_Destroy(cmdPbEventId);
			}

			iResult = CMD_OK;
		}
#else
		CMD_Printf("Supported in HW test mode!\n\r");
		iResult = CMD_OK;
#endif
	}
#if	defined(CONFIG_TEST_COPY_TASK)
	else if CMD_IS("testcopy")
	{
		DRV_PVR_REC_StartCopyTask();
	}
#endif

	else if CMD_IS("fullts")
	{

		HUINT32 iReturn;

		if PARAM_IS("start")
		{
			DI_PVR_REC_SETUP_t recSetup;
			VK_memset(&recSetup, 0, sizeof(DI_PVR_REC_SETUP_t));
			VK_snprintf(buffer, MAX_RECORDING_PATH-1, "%s/hdd_fullts_test", RecordingPath);
			VK_memcpy((void *)recSetup.aucFileName, (void *)buffer, VK_strlen((char *)buffer)+1);

			recSetup.eFileType = eDI_PVR_FileType_Linear;
			recSetup.eStreamType = eDI_PVR_Stream_Video;
			recSetup.eVideoType = eDI_PVR_VideoType_H264;

			iReturn = HwTest_Pvr_TPRecStart(HDD_TEST_REC1_DEV_ID, DEFAULT_REC_DEMUX, 0, &recSetup);
			if(iReturn != CMD_OK)
			{
				VK_printf("%s(%d)\n", __FUNCTION__, __LINE__);
				return;
			}

			iReturn = CMD_OK;
		}
		else if PARAM_IS("stop")
		{
			iReturn = HwTest_Pvr_TPRecStop(HDD_TEST_REC1_DEV_ID, DEFAULT_REC_DEMUX);
			iReturn = CMD_OK;

		}

	}

	else if CMD_IS("rec_pause")
	{
		HULONG	ulDeviceId = 0;
		if(HWTEST_PARAM != NULL )
		{
			if(!READABLE_IN_DEC(HWTEST_PARAM, ulDeviceId))
			{
				iResult=CMD_ERR;
				goto TEST_PVR_EXIT;
			}
			diErr = DI_PVR_REC_Pause(ulDeviceId) ;
			if(diErr != DI_ERR_OK)
			{
				CMD_Printf("error in DI_PVR_REC_Pause\n\r");
				iResult=CMD_ERR;
				goto TEST_PVR_EXIT;
			}
			iResult = CMD_OK;
		}
	}
	else if CMD_IS("rec_resume")
	{
		HULONG	ulDeviceId = 0;

		if(HWTEST_PARAM != NULL )
		{
			if(!READABLE_IN_DEC(HWTEST_PARAM, ulDeviceId))
			{
				iResult=CMD_ERR;
				goto TEST_PVR_EXIT;
			}
			diErr = DI_PVR_REC_Resume(ulDeviceId);
			if(diErr != DI_ERR_OK)
			{
				CMD_Printf("error in DI_PVR_REC_Pause\n\r");
				iResult=CMD_ERR;
				goto TEST_PVR_EXIT;
			}
			iResult = CMD_OK;
		}
	}
	else if CMD_IS("pb_info")
	{
		HULONG	ulDeviceId = 0;
		NEXUS_PlaybackStatus tPlaybackStatus;
		NEXUS_Error tNexusError = NEXUS_SUCCESS;
		NEXUS_PlaybackHandle	pbHandle;
		DRV_Error drvErr = DRV_OK;

		if(HWTEST_PARAM != NULL )
		{
			if(!READABLE_IN_DEC(HWTEST_PARAM, ulDeviceId))
			{
				iResult=CMD_ERR;
				goto TEST_PVR_EXIT;
			}
			drvErr = DRV_PVR_PB_GetPBHandle(ulDeviceId, &pbHandle);
			if(drvErr != DRV_OK)
			{
				iResult=CMD_ERR;
				goto TEST_PVR_EXIT;
			}

			(void)VK_memset(&tPlaybackStatus, 0x0, sizeof(tPlaybackStatus));
			/** Get Playback Status */
			tNexusError = NEXUS_Playback_GetStatus(pbHandle, &tPlaybackStatus);

			if(tNexusError == NEXUS_SUCCESS)
			{
				CMD_Printf("\n== NEXUS_Playback_GetStatus == \n");

				CMD_Printf("tPlaybackStatus.position = %d (0x%x)\n", tPlaybackStatus.position, tPlaybackStatus.position);
				CMD_Printf("tPlaybackStatus.state = 0x%x \n", tPlaybackStatus.state);
				CMD_Printf("tPlaybackStatus.trickModeSetting.rate = 0x%x \n", tPlaybackStatus.trickModeSettings.rate);
				CMD_Printf("tPlaybackStatus.trickModeSetting.mode = 0x%x \n", tPlaybackStatus.trickModeSettings.mode);
				CMD_Printf("tPlaybackStatus.trickModeSetting.mode_modifier = 0x%x \n", tPlaybackStatus.trickModeSettings.mode_modifier);
				CMD_Printf("tPlaybackStatus.trickModeSetting.rateControl = 0x%x \n", tPlaybackStatus.trickModeSettings.rateControl);
				CMD_Printf("tPlaybackStatus.trickModeSetting.skipControl = 0x%x \n", tPlaybackStatus.trickModeSettings.skipControl);
				CMD_Printf("tPlaybackStatus.trickModeSetting.maxDecoderRate = 0x%x \n", tPlaybackStatus.trickModeSettings.maxDecoderRate);
				CMD_Printf("tPlaybackStatus.trickModeSetting.avoidFlush = 0x%x \n", tPlaybackStatus.trickModeSettings.avoidFlush);
				CMD_Printf("tPlaybackStatus.trickModeSetting.brcmTrickMode = 0x%x \n", tPlaybackStatus.trickModeSettings.brcmTrickMode);
				CMD_Printf("tPlaybackStatus.fifoDepth = 0x%x \n", tPlaybackStatus.fifoDepth);
				CMD_Printf("tPlaybackStatus.fifoSize = 0x%x \n", tPlaybackStatus.fifoSize);
				CMD_Printf("tPlaybackStatus.bytesPlayed = 0x%x \n", tPlaybackStatus.bytesPlayed);
				CMD_Printf("tPlaybackStatus.first = 0x%x \n", tPlaybackStatus.first);
				CMD_Printf("tPlaybackStatus.last = 0x%x \n", tPlaybackStatus.last);
				CMD_Printf("tPlaybackStatus.indexErrors = 0x%x \n", tPlaybackStatus.indexErrors);
				CMD_Printf("tPlaybackStatus.dataErrors = 0x%x \n", tPlaybackStatus.dataErrors);
				CMD_Printf("tPlaybackStatus.readPosition = 0x%x \n", tPlaybackStatus.readPosition);
			}
			else
			{
				CMD_Printf("NEXUS_Playback_GetStatus Fail\n");
			}

			iResult = CMD_OK;
		}
	}
#if	defined(CONFIG_RECORD)	
	else if CMD_IS("rec_info")
	{
		HULONG	ulDeviceId = 0;
		NEXUS_RecordStatus	tRecStatus;
		NEXUS_RecordSettings NexusRecordingSettings;
		NEXUS_Error tNexusError = NEXUS_SUCCESS;
		NEXUS_RecordHandle	recHandle;
		NEXUS_PidChannelHandle	idxPidChHandle;
		NEXUS_PidChannelScramblingStatus ScrStatus;
		DRV_Error drvErr = DRV_OK;

		if(HWTEST_PARAM != NULL)
		{
			if(!READABLE_IN_DEC(HWTEST_PARAM, ulDeviceId))
			{
				iResult=CMD_ERR;
				goto TEST_PVR_EXIT;
			}
			drvErr = DRV_PVR_REC_GetRECHandle(ulDeviceId, &recHandle);
			if(drvErr != DRV_OK)
			{
				iResult=CMD_ERR;
				goto TEST_PVR_EXIT;
			}

			(void)VK_memset(&tRecStatus, 0x0, sizeof(tRecStatus));
			/** Get Playback Status */
			tNexusError = NEXUS_Record_GetStatus(recHandle, &tRecStatus);

			if(tNexusError == NEXUS_SUCCESS)
			{
				CMD_Printf("\n== NEXUS_Record_GetStatus from recHandle 0x%x == \n",recHandle);

				CMD_Printf("tRecStatus.lastTimestamp = %d (0x%x)\n", tRecStatus.lastTimestamp, tRecStatus.lastTimestamp);
				CMD_Printf("tRecStatus.picturesIndexed = 0x%x \n", tRecStatus.picturesIndexed);
				CMD_Printf("tRecStatus.error = 0x%x \n", tRecStatus.error);
				CMD_Printf("tRecStatus.indexParsingErrors = 0x%x \n", tRecStatus.indexParsingErrors);

				CMD_Printf("tRecStatus.recpumpStatus.hasIndex = 0x%x \n", tRecStatus.recpumpStatus.hasIndex);
				CMD_Printf("tRecStatus.recpumpStatus.rave.index= 0x%x \n", tRecStatus.recpumpStatus.rave.index);
	/*			CMD_Printf("tRecStatus.recpumpStatus.data.bytesRecorded = %u(0x%x) \n", tRecStatus.recpumpStatus.data.bytesRecorded,tRecStatus.recpumpStatus.data.bytesRecorded);*/
				CMD_Printf("tRecStatus.recpumpStatus.data.fifoDepth= 0x%x \n", tRecStatus.recpumpStatus.data.fifoDepth);
				CMD_Printf("tRecStatus.recpumpStatus.data.fifoSize= 0x%x \n", tRecStatus.recpumpStatus.data.fifoSize);
				CMD_Printf("tRecStatus.recpumpStatus.data.bufferBase= 0x%x \n", tRecStatus.recpumpStatus.data.bufferBase);
	/*			CMD_Printf("tRecStatus.recpumpStatus.index.bytesRecorded = %u(0x%x) \n", tRecStatus.recpumpStatus.index.bytesRecorded,tRecStatus.recpumpStatus.index.bytesRecorded);*/
				CMD_Printf("tRecStatus.recpumpStatus.index.fifoDepth= 0x%x \n", tRecStatus.recpumpStatus.index.fifoDepth);
				CMD_Printf("tRecStatus.recpumpStatus.index.fifoSize= 0x%x \n", tRecStatus.recpumpStatus.index.fifoSize);
				CMD_Printf("tRecStatus.recpumpStatus.index.bufferBase= 0x%x \n", tRecStatus.recpumpStatus.index.bufferBase);
				CMD_Printf("tRecStatus.recpumpStatus.openSettings.nullifyVct= 0x%x \n", tRecStatus.recpumpStatus.openSettings.nullifyVct);
				CMD_Printf("tRecStatus.recpumpStatus.openSettings.useSecureHeap= 0x%x \n", tRecStatus.recpumpStatus.openSettings.useSecureHeap);
	/*			CMD_Printf("tRecStatus.recpumpStatus.index.bufferBase= 0x%x \n", tRecStatus.recpumpStatus.openSettings.);
				not yet~~~
	*/
				/* 3. Get defaultRecordingValue */
				VK_memset( &NexusRecordingSettings, 0x00, sizeof(NexusRecordingSettings));
				NEXUS_Record_GetSettings(recHandle, &NexusRecordingSettings);

				CMD_Printf("\n== NEXUS_Record_GetSettings == \n");
				CMD_Printf("  NexusRecordingSettings.indexFormat = 0x%x \n", NexusRecordingSettings.indexFormat);
#if (NEXUS_VERSION >= 1402) || ((NEXUS_VERSION == 1401) && (NEXUS_VERSION_MINOR >= 1))
#else
				CMD_Printf("  NexusRecordingSettings.stopOnOverflow = 0x%x \n", NexusRecordingSettings.stopOnOverflow);
#endif
				CMD_Printf("  NexusRecordingSettings.writeAllTimeout = 0x%x \n", NexusRecordingSettings.writeAllTimeout);
				CMD_Printf("  NexusRecordingSettings.allowLargeTimeGaps = 0x%x \n", NexusRecordingSettings.allowLargeTimeGaps);
				CMD_Printf("  NexusRecordingSettings.pollingTimer = 0x%x \n", NexusRecordingSettings.pollingTimer);
				CMD_Printf("  NexusRecordingSettings.recpumpSettings.adjustTimestampUsingPcrs = 0x%x \n", NexusRecordingSettings.recpumpSettings.adjustTimestampUsingPcrs);
				CMD_Printf("  NexusRecordingSettings.ecpumpSettings.pcrPidChannel = 0x%x \n", NexusRecordingSettings.recpumpSettings.pcrPidChannel);
				CMD_Printf("  NexusRecordingSettings.recpumpSettings.outputTransportType = 0x%x \n", NexusRecordingSettings.recpumpSettings.outputTransportType);
				CMD_Printf("  NexusRecordingSettings.recpumpSettings.data.useBufferSize = 0x%x \n", NexusRecordingSettings.recpumpSettings.data.useBufferSize);
				CMD_Printf("  NexusRecordingSettings.recpumpSettings.index.useBufferSize = 0x%x \n", NexusRecordingSettings.recpumpSettings.index.useBufferSize);

				/* 4. Get defaultRecordingValue */
				VK_memset( &NexusRecordingSettings, 0x00, sizeof(NexusRecordingSettings));
				NEXUS_Record_GetDefaultSettings(&NexusRecordingSettings);

				CMD_Printf("\n== NEXUS_Record_GetDefaultSettings == \n");
				CMD_Printf("  NexusRecordingSettings.indexFormat = 0x%x \n", NexusRecordingSettings.indexFormat);
#if (NEXUS_VERSION >= 1402) || ((NEXUS_VERSION == 1401) && (NEXUS_VERSION_MINOR >= 1))
#else
				CMD_Printf("  NexusRecordingSettings.stopOnOverflow = 0x%x \n", NexusRecordingSettings.stopOnOverflow);
#endif
				CMD_Printf("  NexusRecordingSettings.writeAllTimeout = 0x%x , sizeof(writeAlltimeout)=0x%x\n", NexusRecordingSettings.writeAllTimeout, sizeof(NexusRecordingSettings.writeAllTimeout));
				CMD_Printf("  NexusRecordingSettings.allowLargeTimeGaps = 0x%x \n", NexusRecordingSettings.allowLargeTimeGaps);
				CMD_Printf("  NexusRecordingSettings.pollingTimer = 0x%x \n", NexusRecordingSettings.pollingTimer);
				CMD_Printf("  NexusRecordingSettings.recpumpSettings.adjustTimestampUsingPcrs = 0x%x \n", NexusRecordingSettings.recpumpSettings.adjustTimestampUsingPcrs);
				CMD_Printf("  NexusRecordingSettings.ecpumpSettings.pcrPidChannel = 0x%x \n", NexusRecordingSettings.recpumpSettings.pcrPidChannel);
				CMD_Printf("  NexusRecordingSettings.recpumpSettings.outputTransportType = 0x%x \n", NexusRecordingSettings.recpumpSettings.outputTransportType);
				CMD_Printf("  NexusRecordingSettings.recpumpSettings.data.useBufferSize = 0x%x \n", NexusRecordingSettings.recpumpSettings.data.useBufferSize);
				CMD_Printf("  NexusRecordingSettings.recpumpSettings.index.useBufferSize = 0x%x \n", NexusRecordingSettings.recpumpSettings.index.useBufferSize);
			}
			else
			{
				CMD_Printf("NEXUS_Record_GetStatus Fail\n");
			}

			drvErr = DRV_PVR_REC_GetIDXPidHandle(ulDeviceId, &idxPidChHandle);
			if(drvErr != DRV_OK)
			{
				iResult=CMD_ERR;
				goto TEST_PVR_EXIT;
			}

			(void)VK_memset(&ScrStatus, 0x0, sizeof(ScrStatus));
			/** Get Scrabling Status */
			tNexusError= NEXUS_PidChannel_GetScramblingStatus(idxPidChHandle, &ScrStatus);
			if(tNexusError == NEXUS_SUCCESS)
			{
				CMD_Printf("\n== NEXUS_PidChannel_GetScramblingStatus == \n");
				CMD_Printf("  ScrStatus.pidExists = 0x%x \n", ScrStatus.pidExists);
				CMD_Printf("  ScrStatus.scrambled = 0x%x \n", ScrStatus.scrambled);
			}
			else
			{
				CMD_Printf("NEXUS_PidChannel_GetScramblingStatus Fail\n");
			}
			if(ScrStatus.scrambled == true)
			{
				NEXUS_PidChannel_ClearScramblingStatus(idxPidChHandle);
			}

			iResult = CMD_OK;
		}
	}
#endif	
#if defined(BDBG_DEBUG_BUILD)
	else if CMD_IS("tracelevel")
	{
		int traceLevel;
		if(!READABLE_IN_DEC(HWTEST_PARAM, traceLevel))
		{
			iResult=CMD_ERR;
			goto TEST_PVR_EXIT;
		}
		if(traceLevel < TRACE_LEVEL_NONE || traceLevel > TRACE_LEVEL_ALL)
		{
			CMD_Printf("Trace level %d is out of range. (0<=level<= 6)\n", traceLevel);
			iResult=CMD_ERR;
			goto TEST_PVR_EXIT;
		}
		CMD_Printf("Set pvr tracelevel to %d\n",traceLevel);
		TraceSetLevel(TRACE_MODULE_DI_PVR, traceLevel);
		iResult = CMD_OK;
	}
#if (NEXUS_VERSION == 1290)
	/*temp: only for unified version debugging purpose*/
	else if CMD_IS("bcmdbg")
	{
		int iDbgLevel;
		extern int iHmxPvrDbgLevel;
		if(!READABLE_IN_DEC(HWTEST_PARAM, iDbgLevel))
		{
			iResult=CMD_ERR;
			goto TEST_PVR_EXIT;
		}
		CMD_Printf("Set bcmdbg level to %d\n",iDbgLevel);
		iHmxPvrDbgLevel = iDbgLevel;
		iResult = CMD_OK;
	}
	else if CMD_IS("bcmrecdbg")
	{
		int iDbgLevel;
		extern int iHmxPvrRecDbgLevel;
		if(!READABLE_IN_DEC(HWTEST_PARAM, iDbgLevel))
		{
			iResult=CMD_ERR;
			goto TEST_PVR_EXIT;
		}
		CMD_Printf("Set bcmrecdbg level to %d\n",iDbgLevel);
		iHmxPvrRecDbgLevel = iDbgLevel;
		iResult = CMD_OK;
	}
#endif
#endif
#if defined(CONFIG_DI20)
	else if CMD_IS("timebasedseek")
	{
		HULONG	ulRequestTimeMs =NULL, ulReceivedTimeMs=NULL, ulDurationTimeMs=NULL;
		HULONG	ulOffsetHi = NULL, ulOffsetLo = NULL;

		if(HWTEST_PARAM != NULL && HWTEST_PARAM1 != NULL)
		{
			if(!READABLE_IN_DEC(HWTEST_PARAM1, ulRequestTimeMs))
			{
				iResult=CMD_ERR;
				goto TEST_PVR_EXIT;
			}

			DI_PVR_PLAY_ConvertTimeToBytesByIndex(HWTEST_PARAM, ulRequestTimeMs,&ulOffsetHi,&ulOffsetLo,&ulReceivedTimeMs);

			DI_PVR_PLAY_GetDurationByIndex(HWTEST_PARAM, &ulDurationTimeMs);

			CMD_Printf("\n %s : ", HWTEST_PARAM);
			CMD_Printf(" %u ms 's offset is \n", ulRequestTimeMs);
			CMD_Printf(" 0x%x : 0x%x and, real time is %u ms. and total duration is %u \n", ulOffsetHi,ulOffsetLo,ulReceivedTimeMs,ulDurationTimeMs);


			DI_PVR_PLAY_ConvertBytesByIndexToTime(HWTEST_PARAM, ulOffsetHi,ulOffsetLo,&ulReceivedTimeMs);
			CMD_Printf("DI_PVR_PLAY_ConvertBytesByIndexToTime TimeMs %u\n",ulReceivedTimeMs);
			iResult = CMD_OK;
		}
	}
#endif

TEST_PVR_EXIT:
	return iResult;
}

#if defined(CONFIG_PRODUCT_IMAGE_HWTEST)
void CMD_HwTest_PvrRegisterEventCallback(CMD_HWTEST_PVR_EVENT eEvent, CMD_HwTest_Pvr_Event_Callback pCallbackFunc)
{
	pvrEvtCbFunc[eEvent] = pCallbackFunc;
}
#endif

static int HwTest_Pvr_RecStart(HUINT32 ulDeviceId, HUINT32 ulDmxId, HUINT32 ulInputDevId, DI_PVR_REC_SETUP_t *stSetup,
									HUINT16 usVideoPid, HUINT16 usAudioPid, HUINT16 usPcrPid,HUINT16 usPmtPid )
{
	DI_ERR_CODE diErr = DI_ERR_OK;
	DI_DEMUX_INPUTDEVICETYPE inDevType;
	DI_PVR_CRYPTO_SETTINGS stCryptoSetting;

	if(ulDmxId == DRV_DEMUX_GetFirstPlaybackParserIndex())
	{
		inDevType = DI_DEMUX_INPUT_PLAYER;
	}
	else
	{
		inDevType = DI_DEMUX_INPUT_TUNER;
	}

	diErr = DI_DEMUX_SetPath(ulDmxId, inDevType, ulInputDevId, DI_DEMUX_OUTPUT_RECORDER, ulDeviceId, -1);
	if(diErr != DI_ERR_OK)
	{
		CMD_Printf("error in DI_DEMUX_SetPath\n\r");
		return CMD_ERR;
	}

	diErr = DI_PVR_REC_Setup(ulDeviceId, stSetup);
	if(diErr != DI_ERR_OK)
	{
		CMD_Printf("error in DI_PVR_REC_Setup\n\r");
		return CMD_ERR;
	}

	if(g_erEncryptyinType != DI_PVR_ENCRYPTION_TYPE_NONE)
	{
		stCryptoSetting.eEncType = g_erEncryptyinType;
		VK_memcpy(stCryptoSetting.ucKey, s_aucEncryptionKey, DI_PVR_CRYPTO_KEY_LEN);
		VK_memcpy(stCryptoSetting.ucIv, s_aucEncryptionIV, DI_PVR_CRYPTO_KEY_LEN);
		VK_memcpy(stCryptoSetting.ucProtectionKey, s_aucEncryptionProtectionKey, DI_PVR_CRYPTO_KEY_LEN);

		diErr = DI_PVR_REC_SetEncryption(ulDeviceId, &stCryptoSetting);
		if(diErr != DI_ERR_OK)
		{
			CMD_Printf("error in DI_PVR_REC_SetEncryption\n\r");
			return CMD_ERR;
		}
	}

	diErr = DI_PVR_REC_AddPidChannel(ulDeviceId, usVideoPid, DI_PVR_REC_PID_PES_TYPE_VIDEO, &recPidChHandle[ulDeviceId][0]);
	if(diErr != DI_ERR_OK)
	{
		CMD_Printf("error in DI_PVR_REC_AddPidChannel\n\r");
		return CMD_ERR;
	}
	diErr = DI_PVR_REC_AddPidChannel(ulDeviceId, usAudioPid, DI_PVR_REC_PID_PES_TYPE_AUDIO, &recPidChHandle[ulDeviceId][1]);
	if(diErr != DI_ERR_OK)
	{
		CMD_Printf("error in DI_PVR_REC_AddPidChannel\n\r");
		return CMD_ERR;
	}
	diErr = DI_PVR_REC_AddPidChannel(ulDeviceId, usPcrPid, DI_PVR_REC_PID_PES_TYPE_PCR, &recPidChHandle[ulDeviceId][2]);
	if(diErr != DI_ERR_OK)
	{
		CMD_Printf("error in DI_PVR_REC_AddPidChannel\n\r");
		return CMD_ERR;
	}

	diErr = DI_PVR_REC_AddPidChannel(ulDeviceId, PAT_PID, DI_PVR_REC_PID_PES_NONE, &recPidChHandle[ulDeviceId][3]);
	if(diErr != DI_ERR_OK)
	{
		CMD_Printf("error in DI_PVR_REC_AddPidChannel\n\r");
		return CMD_ERR;
	}

	if(usPmtPid != 0xffff)
	{
		diErr = DI_PVR_REC_AddPidChannel(ulDeviceId, usPmtPid, DI_PVR_REC_PID_PES_NONE, &recPidChHandle[ulDeviceId][4]);
		if(diErr != DI_ERR_OK)
		{
			CMD_Printf("error in DI_PVR_REC_AddPidChannel\n\r");
			return CMD_ERR;
		}
	}

#if defined(CONFIG_JAPAN_SPEC)
#if defined(CONFIG_SIT_PACKET_INSERTION)
	/* SIT packet insertion for Japan*/
	 /* 1. packet insertion */
	 /* 2. add pid channel */
	{
		unsigned long ulSectionSize ;
		unsigned long ulInterval;
		unsigned short usSitPid;

		ulSectionSize = 194;	 /* sizeof(pucSITDummy) */
		usSitPid = 0x1f;	 	/* Arib spec */
		ulInterval = 1000;

		diErr = DI_DEMUX_InsertPacketModeOn(ulDmxId, usSitPid, pucSITDummy, ulSectionSize, ulInterval, &ulHandle_PacketInsetion[ulDeviceId]);
		if(diErr != DI_ERR_OK)
		{
			CMD_Printf("error in DI_PVR_REC_AddPidChannel\n\r");
			return CMD_ERR;
		}

		diErr = DI_PVR_REC_AddPidChannel(ulDeviceId, usSitPid, DI_PVR_REC_PID_PES_NONE, &recPidChHandle[ulDeviceId][5]);
		if(diErr != DI_ERR_OK)
		{
			CMD_Printf("error in DI_PVR_REC_AddPidChannel\n\r");
			return CMD_ERR;
		}
	}
#endif
#endif

	diErr = DI_PVR_REC_Start(ulDeviceId);
	if(diErr != DI_ERR_OK)
	{
		CMD_Printf("error in DI_PVR_REC_Start\n\r");
		return CMD_ERR;
	}

	CMD_Printf("[HwTest_Pvr_RecStart] ulDeviceId(%d) ulDmxId(%d) recPidHandle(0x%x, 0x%x, 0x%x, 0x%x, 0x%x)\n",
				ulDeviceId,
				ulDmxId,
				recPidChHandle[ulDeviceId][0],
				recPidChHandle[ulDeviceId][1],
				recPidChHandle[ulDeviceId][2],
				recPidChHandle[ulDeviceId][3],
				recPidChHandle[ulDeviceId][4]);

	return CMD_OK;
}

static int HwTest_Pvr_TPRecStart(HUINT32 ulDeviceId, HUINT32 ulDmxId, HUINT32 ulInputDevId, DI_PVR_REC_SETUP_t *stSetup)

{
	DI_ERR_CODE diErr = DI_ERR_OK;
	DI_DEMUX_INPUTDEVICETYPE inDevType;
	DI_PVR_CRYPTO_SETTINGS stCryptoSetting;

	if(ulDmxId == DRV_DEMUX_GetFirstPlaybackParserIndex())
	{
		inDevType = DI_DEMUX_INPUT_PLAYER;
	}
	else
	{
		inDevType = DI_DEMUX_INPUT_TUNER;
	}

	diErr = DI_DEMUX_SetPath(ulDmxId, inDevType, ulInputDevId, DI_DEMUX_OUTPUT_RECORDER, ulDeviceId, -1);
	if(diErr != DI_ERR_OK)
	{
		CMD_Printf("error in DI_DEMUX_SetPath\n\r");
		return CMD_ERR;
	}

	diErr = DI_PVR_REC_Setup(ulDeviceId, stSetup);
	if(diErr != DI_ERR_OK)
	{
		CMD_Printf("error in DI_PVR_REC_Setup\n\r");
		return CMD_ERR;
	}

	diErr = DI_PVR_REC_AddPidChannel(ulDeviceId, 0, DI_PVR_REC_PID_PES_TYPE_ALL, &recPidChHandle[ulDeviceId][0]);
	if(diErr != DI_ERR_OK)
	{
		CMD_Printf("error in DI_PVR_REC_AddPidChannel\n\r");
		return CMD_ERR;
	}

	diErr = DI_PVR_REC_Start(ulDeviceId);
	if(diErr != DI_ERR_OK)
	{
		CMD_Printf("error in DI_PVR_REC_Start\n\r");
		return CMD_ERR;
	}

	CMD_Printf("[HwTest_Pvr_RecStart] ulDeviceId(%d) ulDmxId(%d) recPidHandle(0x%x, 0x%x, 0x%x, 0x%x, 0x%x)\n",
				ulDeviceId,
				ulDmxId,
				recPidChHandle[ulDeviceId][0],
				recPidChHandle[ulDeviceId][1],
				recPidChHandle[ulDeviceId][2],
				recPidChHandle[ulDeviceId][3],
				recPidChHandle[ulDeviceId][4]);

	return CMD_OK;
}



static int HwTest_Pvr_RecStop(HUINT32 ulDeviceId, HUINT32 ulDmxId)
{
	DI_ERR_CODE diErr = DI_ERR_OK;

	CMD_Printf("[HwTest_Pvr_RecStop] ulDeviceId(%d) ulDmxId(%d) recPidHandle(0x%x, 0x%x, 0x%x, 0x%x, 0x%x)\n",
				ulDeviceId,
				ulDmxId,
				recPidChHandle[ulDeviceId][0],
				recPidChHandle[ulDeviceId][1],
				recPidChHandle[ulDeviceId][2],
				recPidChHandle[ulDeviceId][3],
				recPidChHandle[ulDeviceId][4]);

	diErr = DI_PVR_REC_Stop(ulDeviceId);
	if(diErr != DI_ERR_OK)
	{
		CMD_Printf("error in DI_PVR_REC_Stop\n\r");
		return CMD_ERR;
	}

	diErr = DI_PVR_REC_RemovePidChannel(ulDeviceId, recPidChHandle[ulDeviceId][0]);
	if(diErr != DI_ERR_OK)
	{
		CMD_Printf("error in DI_PVR_REC_RemovePidChannel\n\r");
		return CMD_ERR;
	}

	diErr = DI_PVR_REC_RemovePidChannel(ulDeviceId, recPidChHandle[ulDeviceId][1]);
	if(diErr != DI_ERR_OK)
	{
		CMD_Printf("error in DI_PVR_REC_RemovePidChannel\n\r");
		return CMD_ERR;
	}

	diErr = DI_PVR_REC_RemovePidChannel(ulDeviceId, recPidChHandle[ulDeviceId][2]);
	if(diErr != DI_ERR_OK)
	{
		CMD_Printf("error in DI_PVR_REC_RemovePidChannel\n\r");
		return CMD_ERR;
	}

	diErr = DI_PVR_REC_RemovePidChannel(ulDeviceId, recPidChHandle[ulDeviceId][3]);
	if(diErr != DI_ERR_OK)
	{
		CMD_Printf("error in DI_PVR_REC_RemovePidChannel\n\r");
		return CMD_ERR;
	}

	if (recPidChHandle[ulDeviceId][4] != 0)
	{
		diErr = DI_PVR_REC_RemovePidChannel(ulDeviceId, recPidChHandle[ulDeviceId][4]);
		if(diErr != DI_ERR_OK)
		{
			CMD_Printf("error in DI_PVR_REC_RemovePidChannel\n\r");
			return CMD_ERR;
		}
	}
#if defined(CONFIG_JAPAN_SPEC)
#if defined(CONFIG_SIT_PACKET_INSERTION)
		/* SIT for Japan*/
		{
			unsigned long ulSectionSize ;
			unsigned long ulInterval;
			unsigned short usSitPid;

			ulSectionSize = 194;	 /* sizeof(pucSITDummy) */
			usSitPid = 0x1f;		/* Arib spec */
			ulInterval = 1000;

			diErr = DI_DEMUX_InsertPacketModeOff(ulHandle_PacketInsetion[ulDeviceId]);
			if(diErr != DI_ERR_OK)
			{
				CMD_Printf("error in DI_DEMUX_InsertPacketModeOff\n\r");
				return CMD_ERR;
			}
			if (recPidChHandle[ulDeviceId][5] != 0)
			{
				diErr = DI_PVR_REC_RemovePidChannel(ulDeviceId, recPidChHandle[ulDeviceId][5]);
				if(diErr != DI_ERR_OK)
				{
					CMD_Printf("error in DI_PVR_REC_RemovePidChannel\n\r");
					return CMD_ERR;
				}
			}
		}
#endif
#endif

	diErr = DI_DEMUX_ReleasePath(ulDmxId);
	if(diErr != DI_ERR_OK)
	{
		CMD_Printf("error in DI_DEMUX_ReleasePath\n\r");
		return CMD_ERR;
	}

	return CMD_OK;
}


static int HwTest_Pvr_TPRecStop(HUINT32 ulDeviceId, HUINT32 ulDmxId)
{
	DI_ERR_CODE diErr = DI_ERR_OK;

	CMD_Printf("[HwTest_Pvr_RecStop] ulDeviceId(%d) ulDmxId(%d) recPidHandle(0x%x, 0x%x, 0x%x, 0x%x, 0x%x)\n",
				ulDeviceId,
				ulDmxId,
				recPidChHandle[ulDeviceId][0],
				recPidChHandle[ulDeviceId][1],
				recPidChHandle[ulDeviceId][2],
				recPidChHandle[ulDeviceId][3],
				recPidChHandle[ulDeviceId][4]);

	diErr = DI_PVR_REC_Stop(ulDeviceId);
	if(diErr != DI_ERR_OK)
	{
		CMD_Printf("error in DI_PVR_REC_Stop\n\r");
		return CMD_ERR;
	}

	diErr = DI_PVR_REC_RemovePidChannel(ulDeviceId, recPidChHandle[ulDeviceId][0]);
	if(diErr != DI_ERR_OK)
	{
		CMD_Printf("error in DI_PVR_REC_RemovePidChannel\n\r");
		return CMD_ERR;
	}

	diErr = DI_DEMUX_ReleasePath(ulDmxId);
	if(diErr != DI_ERR_OK)
	{
		CMD_Printf("error in DI_DEMUX_ReleasePath\n\r");
		return CMD_ERR;
	}

	return CMD_OK;
}


static void HwTest_Pvr_PlayEofCallback(void *pMsg)
{
	BSTD_UNUSED(pMsg);

	VK_EVENT_Send(cmdPbEventId, CMD_PB_EOF_EVENTS);
}

static int HwTest_Pvr_PlayStart(DI_PVR_PLAY_SETUP_t *stSetup, DI_VIDEO_FORMAT nVidFmt, DI_AUDIO_FORMAT nAudFmt,
									HUINT16 usVideoPid, HUINT16 usAudioPid, HUINT16 usPcrPid)
{
	DI_ERR_CODE diErr = DI_ERR_OK;
	DI_DEMUX_INPUTDEVICETYPE inDevType;
	HUINT32	inDevId;
	HUINT32 ulDemuxId;
	#if defined(CONFIG_SCART)
	DRV_SCART_OPERATION* pstScartOp;
	#endif
	Pid_Info pidInfo;
	DI_PVR_CRYPTO_SETTINGS stCryptoSetting;

	if((stSetup->eFileType == eDI_PVR_FileType_Circular) || (stSetup->bIsRecording == TRUE))
	{
		inDevType = DI_DEMUX_INPUT_RECORDER;
		inDevId = stSetup->ulStartTime;
	}
	else
	{
		inDevType = DI_DEMUX_INPUT_PLAYER;
		inDevId = DEFAULT_PLAYER;
	}

	ulDemuxId = DRV_DEMUX_GetFirstPlaybackParserIndex();
	diErr = DI_DEMUX_SetPath(ulDemuxId, inDevType, inDevId, DI_DEMUX_OUTPUT_DECODER, 0, -1);
	if(diErr != DI_ERR_OK)
	{
		CMD_Printf("error in DI_DEMUX_SetPath\n\r");
		return CMD_ERR;
	}
#if defined(CONFIG_DI20)
	stSetup->ulDemuxId = ulDemuxId;
#endif
	diErr = DI_PVR_PLAY_Setup(DEFAULT_PLAYER, stSetup);
	if(diErr != DI_ERR_OK)
	{
		CMD_Printf("error in DI_PVR_PLAY_Setup\n\r");
		return CMD_ERR;
	}

	if(g_erEncryptyinType != DI_PVR_ENCRYPTION_TYPE_NONE)
	{
		stCryptoSetting.eEncType = g_erEncryptyinType;
		VK_memcpy(stCryptoSetting.ucKey, s_aucEncryptionKey, DI_PVR_CRYPTO_KEY_LEN);
		VK_memcpy(stCryptoSetting.ucIv, s_aucEncryptionIV, DI_PVR_CRYPTO_KEY_LEN);
		VK_memcpy(stCryptoSetting.ucProtectionKey, s_aucEncryptionProtectionKey, DI_PVR_CRYPTO_KEY_LEN);

		diErr = DI_PVR_PLAY_SetDecryption(DEFAULT_PLAYER, &stCryptoSetting);
		if(diErr != DI_ERR_OK)
		{
			CMD_Printf("error in DI_PVR_PLAY_SetDecryption\n\r");
			return CMD_ERR;
		}
	}

	pidInfo.pidBitmask = PID_BITMASK_VIDEO | PID_BITMASK_AUDIO_MAIN | PID_BITMASK_PCR;
	pidInfo.usVideoPid = usVideoPid;
	pidInfo.usAudioMainPid = usAudioPid;
	pidInfo.usPCRPid = usPcrPid;
	diErr = DI_DEMUX_StartPID(ulDemuxId, &pidInfo);
	if(diErr != DI_ERR_OK)
	{
		CMD_Printf("error in DI_DEMUX_StartPID\n\r");
		return CMD_ERR;
	}

	diErr = DI_AUDIO_Start(DEFAULT_AUDIO, nAudFmt);
	if(diErr != DI_ERR_OK)
	{
		CMD_Printf("error in DI_AUDIO_Start\n\r");
		return CMD_ERR;
	}
	diErr = DI_VIDEO_Start(DEFAULT_VIDEO, nVidFmt);
	if(diErr != DI_ERR_OK)
	{
		CMD_Printf("error in DI_VIDEO_Start\n\r");
		return CMD_ERR;
	}

	diErr = DI_PVR_PLAY_Start(DEFAULT_PLAYER);
	if(diErr != DI_ERR_OK)
	{
		CMD_Printf("error in DI_PVR_PLAY_Start\n\r");
		return CMD_ERR;
	}
	VK_TASK_Sleep(1000);
	diErr = DI_VIDEO_Show(DEFAULT_VIDEO, TRUE);
	if(diErr != DI_ERR_OK)
	{
		CMD_Printf("error in DI_VIDEO_Show\n\r");
		return CMD_ERR;
	}

	#if defined(CONFIG_SCART)
	pstScartOp = DRV_SCART_GetOpHandle();
	pstScartOp->pfSet_Normal_Mode();
	#endif

	return CMD_OK;
}

static int HwTest_Pvr_PlayStop(void)
{
	DI_ERR_CODE diErr = DI_ERR_OK;

	diErr = DI_AUDIO_Stop(DEFAULT_AUDIO);
	if(diErr != DI_ERR_OK)
	{
		CMD_Printf("error in DI_AUDIO_Stop\n\r");
		return CMD_ERR;
	}
	diErr = DI_VIDEO_Stop(DEFAULT_VIDEO);
	if(diErr != DI_ERR_OK)
	{
		CMD_Printf("error in DI_VIDEO_Stop\n\r");
		return CMD_ERR;
	}

	diErr = DI_DEMUX_StopPID(DRV_DEMUX_GetFirstPlaybackParserIndex(), (HUINT32)(PID_BITMASK_VIDEO | PID_BITMASK_AUDIO_MAIN | PID_BITMASK_PCR));
	if(diErr != DI_ERR_OK)
	{
		CMD_Printf("error in DI_DEMUX_StopPID\n\r");
		return CMD_ERR;
	}

	diErr = DI_PVR_PLAY_Stop(DEFAULT_PLAYER);
	if(diErr != DI_ERR_OK)
	{
		CMD_Printf("error in DI_PVR_PLAY_Stop\n\r");
		return CMD_ERR;
	}

	diErr = DI_DEMUX_ReleasePath(DRV_DEMUX_GetFirstPlaybackParserIndex());
	if(diErr != DI_ERR_OK)
	{
		CMD_Printf("error in DI_DEMUX_ReleasePath\n\r");
		return CMD_ERR;
	}

	return CMD_OK;
}
#if defined(CONFIG_TEMP_MPLAY)
#include "di_media20.h"

NEXUS_PlaybackHandle g_playback;
NEXUS_VideoDecoderHandle g_videoDecoder;
NEXUS_AudioDecoderHandle g_audioDecoder;
NEXUS_FilePlayHandle g_file;
NEXUS_PlaypumpHandle g_playpump;

int HwTest_Pvr_FileStop(void)
{
    NEXUS_VideoDecoder_Stop(g_videoDecoder);
    NEXUS_AudioDecoder_Stop(g_audioDecoder);
    NEXUS_Playback_Stop(g_playback);
    NEXUS_FilePlay_Close(g_file);
    NEXUS_Playback_Destroy(g_playback);
    NEXUS_Playpump_Close(g_playpump);
	DRV_AUDIO_RemoveDacOutputPath(0);

	return 0;
}
int HwTest_Pvr_FilePlay(void)
{
	FILE *f;
	HINT8 *p;
	HINT8 pbListBuf[256] = {0,};
	HINT8 pbFileBuf[256] = {0,};
	HINT8 keyBuf[2048]={0,};
	HBOOL	isHdd = FALSE;
	HUINT8 *pUrl;
	SESSION_HANDLE hSession;
	HUINT8 unMediaId;
	DI_MEDIA_INFO_t ProbeInfo;
	HUINT32 dResult = 0;

	f = fopen("/mnt/hd2/playback_list.txt", "r");
	if (f != NULL)
	{
		isHdd = TRUE;
	}
	else
	{
		f = fopen("/media/playback_list.txt", "r");
		if (f != NULL)
		{
			isHdd = FALSE;
		}
		else
		{
			VK_printf("cannot open playback_list.txt\n");
			return 1;
		}
	}

	CMD_Printf("\n====================== * LIST * ======================\n\n");
	while(1)
	{
		if(fgets(pbListBuf,sizeof(pbListBuf),f) == NULL)
		{
			break;
		}

		if( (p = VK_strchr(pbListBuf,'\r')) )
		{
			*p = '\0';
		}
		if( (p = VK_strchr(pbListBuf,'\n')) )
		{
			*p = '\0';
		}

		if(pbListBuf[0] == '#')
		{
			CMD_Printf("[X] ");
			CMD_Printf("%s\n",pbListBuf);
		}
		else
		{
			CMD_Printf("[O] ");

			if (isHdd)
				VK_strcpy(pbFileBuf,"/mnt/hd2/");
			else
				VK_strcpy(pbFileBuf,"/media/");
			VK_strcat(pbFileBuf,pbListBuf);

			if (isHdd)
				VK_strcpy(keyBuf,"file://mnt/hd2/");
			else
				VK_strcpy(keyBuf,"file://media/");

			VK_strcat(keyBuf,pbListBuf);

			CMD_Printf("%s\n",pbListBuf);
		}

	}

	CMD_Printf("\n======================================================\n");

	fclose(f);
	pUrl = VK_StrDup(keyBuf);
	CMD_Printf("pUrl [[%s]] \n",pUrl);
#if defined(CONFIG_MEDIA20)
	dResult = DI_MEDIA_CreateSession (pUrl, &(hSession));
	if(dResult != DI_ERR_OK)
	{
		CMD_Printf("###DI_MEDIA_CreateSession error\n");
	}
	dResult = DI_MEDIA_ProbeSession(hSession);
	if(dResult != DI_ERR_OK)
	{
		CMD_Printf("###DI_MEDIA_ProbeSession error\n");

		dResult = DI_MEDIA_CloseSession(hSession);
		if(dResult != DI_ERR_OK)
		{
			CMD_Printf("###DI_MEDIA_CloseSession error\n");
		}
		dResult = DI_MEDIA_DestroySession(hSession);
		if(dResult != DI_ERR_OK)
		{
			CMD_Printf("###DI_MEDIA_DestroySession error\n");
		}
		P_HwTest_Pvr_FilePlay(pbFileBuf);

		return 0;
	}

	dResult = DI_MEDIA_GetProbeInfo(hSession, &ProbeInfo);
	if(dResult != DI_ERR_OK)
	{
		CMD_Printf("###DI_MEDIA_GetProbeInfo error\n");
	}

	dResult = DI_MEDIA_ProbeCancel(hSession);
	if(dResult != DI_ERR_OK)
	{
		CMD_Printf("###DI_MEDIA_ProbeCancel error\n");
	}

	dResult = DI_MEDIA_CloseSession(hSession);
	if(dResult != DI_ERR_OK)
	{
		CMD_Printf("###DI_MEDIA_CloseSession error\n");
	}
	dResult = DI_MEDIA_DestroySession(hSession);
	if(dResult != DI_ERR_OK)
	{
		CMD_Printf("###DI_MEDIA_DestroySession error\n");
	}
	CMD_Printf("################ stream_type %d \n",ProbeInfo.stream_type);
	CMD_Printf("################ video_pes_id %d \n",ProbeInfo.video_pes_id);
	CMD_Printf("################ video_stream_id %d \n",ProbeInfo.video_stream_id);
	CMD_Printf("################ video_codec %d \n",ProbeInfo.video_codec);
	CMD_Printf("################ audio_pes_id %d \n",ProbeInfo.audio_pes_id);
	CMD_Printf("################ audio_stream_id %d \n",ProbeInfo.audio_stream_id);
	CMD_Printf("################ audio_codec %d \n",ProbeInfo.audio_codec);
#else
	P_HwTest_Pvr_FilePlay(pbFileBuf);
#endif
	CMD_Printf("\nURL: ");
	CMD_Printf("%s\n",pbFileBuf);
	//P_HwTest_Pvr_FilePlay(pbFileBuf);
	P_HwTest_Pvr_FilePlayTS(pbFileBuf,ProbeInfo.video_stream_id,DRV_VIDEO_GetCodecType(0,ProbeInfo.video_codec),ProbeInfo.audio_stream_id,DRV_AUDIO_ConvertCodecToNexus(ProbeInfo.audio_codec));

	return 0;
}

void P_HwTest_Pvr_FilePlay(unsigned char* ucName)
{
	NEXUS_PlatformConfiguration platformConfig;
	NEXUS_PidChannelHandle videoPidChannel;
	NEXUS_VideoDecoderStartSettings videoProgram;
	NEXUS_VideoDecoderSettings videoDecoderSettings;
	NEXUS_PlaybackSettings playbackSettings;
	NEXUS_PlaybackPidChannelSettings playbackPidSettings;
	NEXUS_Error rc;

	DRV_PVR_GetPlaypumpHandle(&g_playpump);

	BDBG_ASSERT(g_playpump);
	g_playback = NEXUS_Playback_Create();
	BDBG_ASSERT(g_playback);

	g_file = NEXUS_FilePlay_OpenPosix(ucName, ucName);
	if (!g_file) {
		CMD_Printf("can't open file:%s\n", ucName);
		return ;
	}

	NEXUS_Playback_GetSettings(g_playback, &playbackSettings);
	playbackSettings.playpump = g_playpump;
	/* set a stream format, it could be any audio video transport type or file format, i.e NEXUS_TransportType_eMp4, NEXUS_TransportType_eAvi ... */
	playbackSettings.playpumpSettings.transportType = NEXUS_TransportType_eEs;
	playbackSettings.stcChannel = NULL;//stcChannel;
	NEXUS_Playback_SetSettings(g_playback, &playbackSettings);

	DRV_VIDEO_GetVideoHandle(0,&g_videoDecoder);
	/* Open the audio and video pid channels */
	NEXUS_Playback_GetDefaultPidChannelSettings(&playbackPidSettings);
	playbackPidSettings.pidSettings.pidType = NEXUS_TransportType_eEs;
#if (NEXUS_VERSION >= 1303)
	playbackPidSettings.pidTypeSettings.video.codec = NEXUS_VideoCodec_eH265; /* must be told codec for correct handling */
#else
	playbackPidSettings.pidTypeSettings.video.codec = NEXUS_VideoCodec_eH264; /* must be told codec for correct handling */
#endif
	playbackPidSettings.pidTypeSettings.video.index = true;
	playbackPidSettings.pidTypeSettings.video.decoder = g_videoDecoder;
	videoPidChannel = NEXUS_Playback_OpenPidChannel(g_playback, 1, &playbackPidSettings);

	/* Set up decoder Start structures now. We need to know the audio codec to properly set up the audio outputs. */
	NEXUS_VideoDecoder_GetDefaultStartSettings(&videoProgram);
#if (NEXUS_VERSION >= 1303)
	videoProgram.codec = NEXUS_VideoCodec_eH265;
#else
	videoProgram.codec = NEXUS_VideoCodec_eH264;
#endif
	videoProgram.pidChannel = videoPidChannel;
	videoProgram.stcChannel = NULL;//stcChannel;
	videoProgram.frameRate = NEXUS_VideoFrameRate_e30;

	/* Start decoders */
	NEXUS_VideoDecoder_Start(g_videoDecoder, &videoProgram);


	/* Start playback */
	NEXUS_Playback_Start(g_playback, g_file, NULL);

	NEXUS_VideoDecoder_GetSettings(g_videoDecoder, &videoDecoderSettings);
	videoDecoderSettings.mute=false;
	NEXUS_VideoDecoder_SetSettings(g_videoDecoder, &videoDecoderSettings);

	DRV_VIDEO_Show(0,TRUE);

	return ;
}

void P_HwTest_Pvr_FilePlayTS(unsigned char* ucName,HUINT32 VPID,NEXUS_VideoCodec vCodec,HUINT32 APID,NEXUS_AudioCodec aCodec)
{
	NEXUS_PlatformConfiguration platformConfig;
	NEXUS_PidChannelHandle videoPidChannel,audioPidChannel;
	NEXUS_VideoDecoderStartSettings videoProgram;
	NEXUS_VideoDecoderSettings videoDecoderSettings;
	NEXUS_PlaybackSettings playbackSettings;
	NEXUS_PlaybackPidChannelSettings playbackPidSettings;
    NEXUS_AudioDecoderStartSettings audioProgram;
	NEXUS_Error rc;

	DRV_PVR_GetPlaypumpHandle(&g_playpump);

	BDBG_ASSERT(g_playpump);
	g_playback = NEXUS_Playback_Create();
	BDBG_ASSERT(g_playback);

	g_file = NEXUS_FilePlay_OpenPosix(ucName, ucName);
	if (!g_file) {
		CMD_Printf("can't open file:%s\n", ucName);
		return;
	}

	NEXUS_Playback_GetSettings(g_playback, &playbackSettings);
	playbackSettings.playpump = g_playpump;
	/* set a stream format, it could be any audio video transport type or file format, i.e NEXUS_TransportType_eMp4, NEXUS_TransportType_eAvi ... */
	playbackSettings.playpumpSettings.transportType = NEXUS_TransportType_eTs;
	playbackSettings.stcChannel = NULL;//stcChannel;
	NEXUS_Playback_SetSettings(g_playback, &playbackSettings);

/*******************VIDEO***************************************************/
	DRV_VIDEO_GetVideoHandle(0,&g_videoDecoder);
	/* Open the audio and video pid channels */
	NEXUS_Playback_GetDefaultPidChannelSettings(&playbackPidSettings);
	playbackPidSettings.pidSettings.pidType = NEXUS_PidType_eVideo;
	playbackPidSettings.pidTypeSettings.video.codec = vCodec; /* must be told codec for correct handling */
	playbackPidSettings.pidTypeSettings.video.index = true;
	playbackPidSettings.pidTypeSettings.video.decoder = g_videoDecoder;
	videoPidChannel = NEXUS_Playback_OpenPidChannel(g_playback, VPID, &playbackPidSettings);

	/* Set up decoder Start structures now. We need to know the audio codec to properly set up the audio outputs. */
	NEXUS_VideoDecoder_GetDefaultStartSettings(&videoProgram);
	videoProgram.codec = vCodec;
	videoProgram.pidChannel = videoPidChannel;
	videoProgram.stcChannel = NULL;//stcChannel;
	videoProgram.frameRate = NEXUS_VideoFrameRate_e30;

	/* Start decoders */
	NEXUS_VideoDecoder_Start(g_videoDecoder, &videoProgram);

	/*******************AUDIO***************************************************/
	DRV_AUDIO_AddDacOutputPath(0);

	DRV_AUDIO_GetDecoderHandle(0,&g_audioDecoder);
    NEXUS_Playback_GetDefaultPidChannelSettings(&playbackPidSettings);
    playbackPidSettings.pidSettings.pidType = NEXUS_PidType_eAudio;
    playbackPidSettings.pidTypeSettings.audio.primary = g_audioDecoder;
    audioPidChannel = NEXUS_Playback_OpenPidChannel(g_playback, APID, &playbackPidSettings);

	NEXUS_AudioDecoder_GetDefaultStartSettings(&audioProgram);
    audioProgram.codec = aCodec;
    audioProgram.pidChannel = audioPidChannel;
    audioProgram.stcChannel = NULL;//stcChannel;
    NEXUS_AudioDecoder_Start(g_audioDecoder, &audioProgram);


	/* Start playback */
	NEXUS_Playback_Start(g_playback, g_file, NULL);

	NEXUS_VideoDecoder_GetSettings(g_videoDecoder, &videoDecoderSettings);
	videoDecoderSettings.mute=false;
	NEXUS_VideoDecoder_SetSettings(g_videoDecoder, &videoDecoderSettings);

	DRV_VIDEO_Show(0,TRUE);

	return ;
}


#endif
static void HwTestPvr_PlayEventTask(void *param)
{
	DI_ERR_CODE diErr = DI_ERR_OK;
	int vkErr = VK_OK;
	unsigned long retrieveEvents;

	BSTD_UNUSED(param);

	while(1)
	{
		vkErr = VK_EVENT_Receive(cmdPbEventId, &retrieveEvents);
		if(vkErr != VK_OK)
		{
			CMD_Printf("error in VK_EVENT_Receive\n\r");
			continue;
		}

		if(retrieveEvents & (unsigned long)CMD_PB_EOF_EVENTS)
		{
			diErr = DI_PVR_PLAY_SetPosition(DEFAULT_PLAYER, 0);
			if(diErr != DI_ERR_OK)
			{
				CMD_Printf("error in DI_PVR_PLAY_SetPosition\n\r");
			}
#if defined(CONFIG_DI10)
			diErr = DI_PVR_PLAY_SetSpeed(DEFAULT_PLAYER, eDI_PVR_Speed_Normal);
#else
			diErr = DI_PVR_PLAY_SetSpeed(DEFAULT_PLAYER, eDI_MEDIA_Speed_Normal);
#endif
			if(diErr != DI_ERR_OK)
			{
				CMD_Printf("error in DI_PVR_PLAY_SetSpeed\n\r");
			}
		}
		else if(retrieveEvents & (unsigned long)CMD_PB_TASK_DESTROY)
		{
			taskDestroyProcess = false;
			while(1)
			{
				VK_TASK_Sleep(10000);
			}
		}
	}
}




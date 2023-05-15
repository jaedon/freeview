#ifndef _DI_DEMUX_PRIV_H_
#define _DI_DEMUX_PRIV_H_

#include "nexus_platform.h"
#include "drv_pvr.h"
#if defined(CONFIG_USE_EXTERNAL_CI_CONTROLLER)
#include "drv_cimax.h"
#else
#include "drv_ci.h"
#endif
#include "di_config.h"
#include "drv_demux.h"

#define DEBUG_MODULE						TRACE_MODULE_DI_DEMUX

#define DEMUX_DEBUG

#define PRINT_DMX_AV_CALLSEQUENCE
#define SAVE_FILTER_INFO
#if defined(DEMUX_DEBUG)
#define PRINT_KNOWN_SECTION_INFO
#define SECTION_FILTER_DEBUGGING
#endif

//Reserved 1 Parserband and 1 PB_Parserband for Allpass-mode.
#if defined(CONFIG_BMEM_OPTIMIZE)
#define NUM_OF_RESERVED_CHANNEL				NUM_REMUX	/* live용 remux parser band와 playback용 remux 용 parser band 용 */
#else
#define NUM_OF_RESERVED_CHANNEL				2	/* live용 remux parser band와 playback용 remux 용 parser band 용 */
#endif
#define NUM_OF_RESERVED_PIDCHANNEL			1	/* live용으로 미리 channel을 reserved해놓는 경우 */
#define RESERVED_CHANNEL_LIVE1_LOGICAL		0
#define RESERVED_CHANNEL_PB1_LOGICAL		1

#if defined(CONFIG_BMEM_OPTIMIZE)

#define RESERVED_CHANNEL_LIVE1_PHYSICAL (CONFIG_NUM_PARSER_BANDS -1)

/*Demux settings*/
#define MAX_INPUT_BAND_NUM					CONFIG_NUM_INPUT_BANDS
#define MAX_IBPARSER_BAND_NUM				CONFIG_NUM_PARSER_BANDS
#define MAX_PBPARSER_BAND_NUM				CONFIG_NUM_PLAYPUMPS
#else
#define RESERVED_CHANNEL_LIVE1_PHYSICAL (NEXUS_NUM_PARSER_BANDS -1)

/*Demux settings*/
#define MAX_INPUT_BAND_NUM					NEXUS_NUM_INPUT_BANDS
#define MAX_IBPARSER_BAND_NUM				NEXUS_NUM_PARSER_BANDS
#define MAX_PBPARSER_BAND_NUM				NEXUS_NUM_PLAYPUMPS
#endif

#define LIVE_DEMUX_NUM						(MAX_IBPARSER_BAND_NUM - NUM_OF_RESERVED_CHANNEL)
#define PLAYBACK_DEMUX_NUM					(MAX_PLAY_INST_NUM+1) /*+1  for 'descramble a file'*/
#define MAX_DEMUX_NUM						(LIVE_DEMUX_NUM + PLAYBACK_DEMUX_NUM)
#define MAX_PID_CHANNEL_NUM					128 // 7405 has 256 pid channel but we use 128

#define	MAX_TIMEBASE_NUM					NEXUS_NUM_TIMEBASES

#define NUM_CI_SLOT_CONCURRENT				1
#define MAX_HIDDEN_PARSER_NUM				1

#define MAX_RAVE_CHANNEL_NUM				1
#define MAX_RAVE_CONTEXT_NUM				24

#define MAX_PKTSUB_MODULE_NUM				6	/*Number of packet substitution module*/
#define MAX_TSCMON_MODULE_NUM				14	/*Number of tsc monitor module: not fixed*/

#define SECTION_FILTER_LENGTH				15
#define MAX_PES_FILTER_NUM					4

#if defined(CONFIG_DI_DEMUX_EXT)
/* Total Neuxs Message Filters - DI Demux Ext Message Filter - PES Filter Number */
#if ((DRV_DMX_NUM_PID_CHANNEL) <= (NEXUS_NUM_MESSAGE_FILTERS-MAX_PES_FILTER_NUM-CONFIG_DEMUX_EXT_NUM_SECTION_FILTER))
#define DEMUX_EXT_MAX_SECTION_FILTER_NUM	0
#else
#define DEMUX_EXT_MAX_SECTION_FILTER_NUM	CONFIG_DEMUX_EXT_NUM_SECTION_FILTER
#endif
#else
#define DEMUX_EXT_MAX_SECTION_FILTER_NUM	0
#endif

#if defined(CONFIG_BMEM_OPTIMIZE)
#define MAX_SECTION_FILTER_NUM				(DRV_DMX_NUM_PID_CHANNEL-MAX_PES_FILTER_NUM-DEMUX_EXT_MAX_SECTION_FILTER_NUM)
#else
#define FILTER_NUM_IN_GROUP					32
#if defined(CONFIG_ZAPPING_ONLY)
#define MAX_FILTER_GROUP_NUM				1
#else
#define MAX_FILTER_GROUP_NUM				4
#endif
#define MAX_SECTION_FILTER_NUM				((FILTER_NUM_IN_GROUP * MAX_FILTER_GROUP_NUM)-MAX_PES_FILTER_NUM-DEMUX_EXT_MAX_SECTION_FILTER_NUM)
#endif

#if defined(CONFIG_MAX_RECORD_INSTANCE)
#define MAX_REC_INST						CONFIG_MAX_RECORD_INSTANCE
#else
#define MAX_REC_INST						3
#endif

#if defined(CONFIG_MAX_TRANSCODER_CHANNEL)
#define MAX_TRANS_CHANNEL					CONFIG_MAX_TRANSCODER_CHANNEL
#define P_DEMUX_TRANSCODER_ID_1				0
#define P_DEMUX_TRANSCODER_ID_2				1
#else
#define MAX_TRANS_CHANNEL					0
#endif

#if defined(CONFIG_MAX_SATIP_CHANNEL)
#define MAX_SATIP_CHANNEL					CONFIG_MAX_SATIP_CHANNEL
#else
#define MAX_SATIP_CHANNEL					0
#endif

#define MAX_REC_CHANNEL						((MAX_REC_INST)+(MAX_TRANS_CHANNEL)+(MAX_SATIP_CHANNEL))

#define	DI_DEMUX_INVALID_PID				0x2000
#define	DI_DEMUX_INVALID_CHANNEL_ID			0xFF

#define DI_MANAGED_FILTER_SIZE				(64*1024)	/* was 32*1024 */
#define DI_MANAGED_PES_FILTER_SIZE			(128*1024)


#define	DRV_DEMUX_SEF_HANDLER_TASK_STACK_SIZE 		(DEMUX_TASK_STACK_SIZE)
#define	DRV_DEMUX_SEF_HANDLER_TASK_PRIORITY   		(DEMUX_TASK_PRIORITY)
#define	DRV_DEMUX_PES_HANDLER_TASK_STACK_SIZE 		(DEMUX_TASK_STACK_SIZE)
#define	DRV_DEMUX_PES_HANDLER_TASK_PRIORITY   		(DEMUX_TASK_PRIORITY-1)

//Demux capability
#define DEMUX_FOR_LIVE_CAPABILITY DI_DEMUX_CAP_LIVE | DI_DEMUX_CAP_SECTIONFILTER | DI_DEMUX_CAP_PESFILTER \
                                              | DI_DEMUX_CAP_DESCRAMBLE | DI_DEMUX_CAP_DESCRAMBLE_CI | DI_DEMUX_CAP_RECORD | DI_DEMUX_CAP_NOTIFY_SCRAMBLE_TS
#define DEMUX_FOR_PLAYBACK_CAPABILITY DI_DEMUX_CAP_PLAYBACK | DI_DEMUX_CAP_DESCRAMBLE | DI_DEMUX_CAP_DESCRAMBLE_CI \
										| DI_DEMUX_CAP_SECTIONFILTER | DI_DEMUX_CAP_PESFILTER | DI_DEMUX_CAP_NOTIFY_SCRAMBLE_TS

#define IS_CI_REQUIRED(nCI_Id)  (nCI_Id >= 0)

#define PESH_SIZE							6
#define DEF_PB_DEV_ID						0
#define DEF_STC_CHANNEL_ID					0
#define DEF_VIDEO_DEV_ID					0


#define P_CHANNEL_NOT_ALLOCATED				0xFF000000

#define INPUTBAND_FROM_REMUX_OUTPUT			NEXUS_InputBand_e4
#define NEXUS_ParserBand_ePb0				NEXUS_ParserBand_eMax
#define NEXUS_ParserBand_ePb1				(NEXUS_ParserBand_ePb0+1)
#define NEXUS_ParserBand_eInvalid			0x7fffffff

#define DMX_MSGQ_COUNT						1024
#define INTER_BUFFER_SIZE					5120
#define PES_INTER_BUFFER_SIZE				(65535+6)

#define PSUB_PACKET_SIZE					188

#define P_DEMUX_PAT_PID						0
#define P_DEMUX_NULLPKT_PID					0x1FFF

#define P_DEMUX_TS_PKT_SIZE					188
#define P_DEMUX_PAYLOAD_SIZE				184
#define P_DEMUX_PACKET_INSERT_MAX_INTERVAL	(912)	/* 912 ms */
#define P_DEMUX_PACKET_REPLACE_INTERVAL		(120)	/* 120 ms */
#define P_DEMUX_PACKET_INSERT_OUTPUT_RATE_CONSTANT	(1649)

//For VBI
#define DRV_DEMUX_LINE_NUM_MASK				0x1F	/* line number mask */
#define DRV_DEMUX_ODD_FIELD_FLAG			0x20	/* on line number byte */
#define DRV_DEMUX_DATA_UNIT_ID_EBU_TTXT_DATA		0x02 /* EBU Teletext non-subtitle data */
#define DRV_DEMUX_DATA_UNIT_ID_EBU_TTXT_SUBTITLE	0x03 /* EBU Teletext subtitle data */
#define SWAP_NIBBLE(ABYTE)	(((ABYTE&0xF0)>>4) | ((ABYTE&0x0F)<<4))

/**********************************************************************/
/****************************** simple macro ****************************/
/**********************************************************************/
#if defined(SECTION_FILTER_DEBUGGING)
#define GETFILTERTYPE(filterType) (filterType==P_DEMUX_PesFilter?"PES":(filterType==P_DEMUX_PsiFilter?"PSI":(filterType==P_DEMUX_DEBUG_PsiFilter?"PSIDBG":"TS")))
#else
#define GETFILTERTYPE(filterType) (filterType==P_DEMUX_PesFilter?"PES":(filterType==P_DEMUX_PsiFilter?"PSI":"TS"))
#endif

#define GET_FILTERTYPE(value)		((value & 0xffff0000)>>16)
#define GET_FILTERINDEX(value)		(value & 0x0000ffff)
#define MAKE_FILTERPAIRVALUE(type ,index)	(((type<<16) & 0xffff0000) |  (index & 0x0000ffff))

/*******************************************************************/
/****************************** typedef ******************************/
/*******************************************************************/
#define DRV_DEMUX_ID_eHidden	100
typedef enum DRV_DEMUX_ID {
	DRV_DEMUX_ID_e0 = 0,
	DRV_DEMUX_ID_e1,
	DRV_DEMUX_ID_e2,
	DRV_DEMUX_ID_e3,
	DRV_DEMUX_ID_e4,
	DRV_DEMUX_ID_e5,
	DRV_DEMUX_ID_e6,
	DRV_DEMUX_ID_e7,
	DRV_DEMUX_ID_e8,
	DRV_DEMUX_ID_e9,
	DRV_DEMUX_ID_eMax
} DRV_DEMUX_ID;

typedef enum _P_DEMUX_ChannelSubType {
	P_DEMUX_CHSUBT_eDSC,
	P_DEMUX_CHSUBT_eTSC,
	P_DEMUX_CHSUBT_eNone,
	P_DEMUX_CHSUBT_eMAX
} P_DEMUX_ChannelSubType;

typedef enum DRV_DEMUX_ResourceStatus{
	DRV_DEMUX_RESOURCE_FREE = 0,
	DRV_DEMUX_RESOURCE_ALLOCATED,
	DRV_DEMUX_RESOURCE_ENABLED,
}DRV_DEMUX_ResourceStatus;

typedef enum _P_DEMUX_FilterType{
	P_DEMUX_PesFilter = 0,
	P_DEMUX_PsiFilter,
#if defined(CONFIG_TSC_CHECK_USING_TS_FILTER)
	P_DEMUX_TsFilter,
#endif // #if defined(CONFIG_TSC_CHECK_USING_TS_FILTER)
#if defined(SECTION_FILTER_DEBUGGING)
	P_DEMUX_DEBUG_PsiFilter,
#endif //#if defined(SECTION_FILTER_DEBUGGING)
	P_DEMUX_FilterType_MAX
}P_DEMUX_FilterType;

typedef enum P_DEMUX_ResourceRunningStatus{
	DRV_DEMUX_RESOURCE_STOPPED = 0,
	DRV_DEMUX_RESOURCE_RUNNING
}P_DEMUX_ResourceRunningStatus;

#if defined(CONFIG_TSC_CHECK_USING_TS_FILTER)
#define NUM_TSC_CHECK_RES 3

#define TSC_MSG_TS_ARRIVED			0x01
#define TSC_MSG_RESTART_FILTER	0x02

typedef struct _TSC_RESC
{
	HBOOL bEnabled;
	HBOOL bFilterOn;
	HUINT32 pidChannel;
	NEXUS_PidChannelHandle pidHandle;
	HUINT32 filterIndex;
	HUINT32 footPrint;
	HUINT32 TSCValue;
	HUINT32 myPidChannel;
	HUINT32 ulDemuxId;
	HUINT16 usPid;
	unsigned long timerId;
} TSC_RESC;

typedef struct _TSC_STRUCT
{
	HULONG	Lock;
	TSC_RESC stResource[NUM_TSC_CHECK_RES];
} stTscCheck;

typedef struct _P_DEMUX_TSC_MSG
{
	HUINT32	ulMsg;
	HUINT32 footPrint;
	HUINT8	ucTSCValue;
}P_DEMUX_TSC_MSG;
#endif

typedef struct _P_INBAND_MONITOR
{
	unsigned int numFilter[LIVE_DEMUX_NUM];
	HBOOL	bMonitor[LIVE_DEMUX_NUM];
	HBOOL 	bNoInputReported[LIVE_DEMUX_NUM];
	unsigned int ulCheckCount[LIVE_DEMUX_NUM];
	HUINT32 ulPrevSyncCount[LIVE_DEMUX_NUM];
	pfnDI_DEMUX_NoInputData NoInputData;
}P_INBAND_MONITOR;

typedef struct DRV_DEMUX_PidChannel{
	HUINT32	demuxId;
	NEXUS_ParserBand parserBand;
	NEXUS_PidChannelHandle pidHandle;
	DRV_DEMUX_ResourceStatus status;
	DI_DEMUX_CHANNELTYPE channelType;
	HBOOL bTSCMonitornigOn;
	HUINT32 ulTSCMonitorIndex;
	P_DEMUX_ChannelSubType channelsubInfo; /*Used to find detailed info for DI_DEMUX_CHANNELTYPE_PRIVATE*/
	HUINT16 pid;
	HULONG lock;
#if defined(CONFIG_SI_PARSING_FOR_MEDIA_PLAYBACK)
	HBOOL	bFromMedia; /*Set TRUE if this channel is opened thru Media Module.*/
#endif
}DRV_DEMUX_PidChannel;

#if defined(CONFIG_FAST_CHANNEL_CHANGE)|| defined(CONFIG_FAST_AUDIO_CHANGE)
typedef struct DRV_DEMUX_StcChannel{
	NEXUS_StcChannelHandle stcHandle;
	HBOOL bStcChannelIsAllocated;
	HULONG lock;
}DRV_DEMUX_StcChannel;
#endif

typedef struct DRV_DEMUX_MsgFilter{
	NEXUS_MessageHandle msgHandle;
	DRV_DEMUX_ResourceStatus status;
	HUINT8 channelIndex;
	HUINT8 physicalChannelIndex;
	HUINT32 requestId;
	P_DEMUX_FilterType filterType;
	DI_DEMUX_PESTYPE etPesType;
}DRV_DEMUX_MsgFilter;

typedef struct _PATH_INFO
{
	DI_DEMUX_INPUTDEVICETYPE etProducer;
	HUINT32 ulInputDevice_id;
	DI_DEMUX_OUTPUTDEVICETYPE etConsumer;
	HUINT32 ulConsumerGroupId;
	HINT32 nCI_Id;
	NEXUS_InputBand inputBand;
	HINT32 state;
	HUINT32 ulRemuxId;
	HUINT32 ulRequestedTunerId;
	NEXUS_ParserBand parserBand;
}P_DEMUX_PathInfo;

typedef struct _CI_INFO
{
	HUINT32	ciUsageCount;
	DI_DEMUX_INPUTDEVICETYPE etProducer;
	HUINT32 ulInputDevice_id;
	HBOOL	bPassthrough;
	HBOOL	bShadowOn;
	HUINT32 OutputDevice_id;
	HUINT32  	ulDemuxId;
}P_DEMUX_CIInfo;

typedef struct _P_DEMUX_CALLBACK
{
	pfnDI_DEMUX_Notify	pfnSectionNotify;
	pfnDI_DEMUX_AllocBuffer	pfnAllocBuffer;
	pfnDI_DEMUX_FreeBufferForError pfnFreeBuffer;
	pfnDI_DEMUX_PES_Notify pfnPesNotify[DI_DEMUX_PESTYPE_MAX];
	pfnDI_DEMUX_AllocBuffer pfnPesAllocBuffer[DI_DEMUX_PESTYPE_MAX];
	pfnDI_DEMUX_FreeBufferForError pfnPesFreeBuffer[DI_DEMUX_PESTYPE_MAX];
	pfnDI_DEMUX_TSStateNotify pfnTSStateNotify;
}P_DEMUX_Callback;

typedef struct _P_PER_DMX_STRUCT
{
	HUINT32  	ulDemuxID;
	DI_DEMUX_CAPABILITY stCapability;
	HBOOL bIsUsing;
	HULONG lock;
	HINT32 nHiddenParserIndex;
	P_DEMUX_PathInfo pathInfo;
	HUINT32  	ulRequestTunerID;
}P_DEMUX_Device;

typedef struct _P_HIDDEN_PARSER_STRUCT
{
	HBOOL bIsUsing;
	NEXUS_ParserBand orgParserBand;
}P_HIDDEN_PARSER_STRUCT;

typedef struct _P_DEMUX_PktSub_Struct
{
	DRV_DEMUX_ResourceStatus Status;
	HUINT32 ulDemuxId;
	HUINT16 usPid;
	HUINT32 buffSize;
	HUINT8 *pBuffer;
	NEXUS_PacketSubHandle hPacketSub;
	HUINT32 ulIntervalMS;/*JC_NGS_PSUB */
}P_DEMUX_PktSub_Struct;

typedef struct _P_DEMUX_TscMon_Struct
{
	DRV_DEMUX_ResourceStatus Status;
}P_DEMUX_TscMon_Struct;

typedef struct _P_DEMUX_ShadowPlaybackPath
{
	HBOOL	bEnabled;
	HUINT32	ulCount;
	NEXUS_RemuxHandle hRmx;
}P_DEMUX_ShadowPlaybackPath;

typedef struct _P_DEMUX_STRUCT
{
	P_DEMUX_Device dmxDevice[MAX_DEMUX_NUM];
	DRV_DEMUX_PidChannel pidChannel[DRV_DMX_NUM_PID_CHANNEL];
#if defined(CONFIG_FAST_CHANNEL_CHANGE) || defined(CONFIG_FAST_AUDIO_CHANGE)
	DRV_DEMUX_StcChannel stcChannel[MAX_TIMEBASE_NUM];
#endif
	DRV_DEMUX_MsgFilter msgFilter[MAX_SECTION_FILTER_NUM];
	void *msgFilterBuff[DRV_DMX_NUM_PID_CHANNEL];  /*msgBuffer's allocated only one per pidChannel*/
	HULONG	eventID[P_DEMUX_FilterType_MAX];
#if defined(CONFIG_TSC_CHECK_USING_TS_FILTER)
	HULONG	tscID;
#endif
	P_DEMUX_Callback callback;
	NEXUS_RemuxHandle rmxHandle;
	P_DEMUX_ShadowPlaybackPath shadowPBPath;
	P_DEMUX_CIInfo ciInfo[CAM_MAXSLOTID];
	P_DEMUX_PktSub_Struct stPktSubstitution[MAX_PKTSUB_MODULE_NUM];
	P_DEMUX_TscMon_Struct stTscMonitor[MAX_TSCMON_MODULE_NUM];
	P_HIDDEN_PARSER_STRUCT stHiddenParser[MAX_HIDDEN_PARSER_NUM];
}P_DEMUX_Module;

typedef struct _P_DEMUX_MSG_Struct
{
	unsigned int ulReqID;
	unsigned int ulFilterIDX;
	HBOOL bSectionOverflow;
}P_DEMUX_MSG_Struct;

#if defined(SAVE_FILTER_INFO)
typedef struct P_DEMUX_FilterDbgInfo{
	HUINT16 usPid;
	HUINT32 ulFilterLen;
	HUINT8  ucValue[SECTION_FILTER_LENGTH];
	HUINT8  ucMask[SECTION_FILTER_LENGTH];
	HUINT8  ucExclusion[SECTION_FILTER_LENGTH];
	HUINT32	ulRequestId;
}P_DEMUX_FilterDbgInfo;
#endif

#if defined(CONFIG_FAST_CHANNEL_CHANGE) || defined(CONFIG_FAST_AUDIO_CHANGE)
typedef struct DI_DEMUX_PRIMER_PID_CHANNEL
{
	HUINT16 usPid[3];
	HUINT32 ulDemuxId;
	HBOOL bVideoPrimerIsEnable;
	HBOOL bAudioPrimerIsEnable;
	HUINT32 ulVideoPrimerPIDChannel;
	HUINT32 ulAudioPrimerPIDChannel;
	HUINT32 ulPcrPrimerPIDChannel;
}P_DEMUX_PRIMER_PID_CHANNEL_INFO_t;
#endif

/*************************************************************************************************************************************/
#define	REMUX_MAX_NUM	2
typedef struct _P_REMUX_STRUCT
{
	HUINT8	used;
	HUINT8	status;
	HUINT8	playback;
	NEXUS_RemuxHandle 		rmxHandle;
	NEXUS_PidChannelHandle	rmxPlayPidChannel;
	HULONG					rmxSemId;
}P_REMUX_Module;
/*************************************************************************************************************************************/

//Debug function prototype
#if defined(SECTION_FILTER_DEBUGGING)
static void P_DEMUX_DEBUG_SectionEventTask(void*);
DI_ERR_CODE DI_DEMUX_DEBUG_SetSectionFilter (HUINT32, HUINT16, HUINT32, HBOOL, HUINT32, HUINT8*,HUINT8*, HUINT8*, HBOOL);
DI_ERR_CODE DI_DEMUX_DEBUG_ReleaseSectionFilter (HUINT32, HUINT32);
static DRV_Error P_DEMUX_DEBUG_PrepareSectionHandling(void);
#endif /*defined(SECTION_FILTER_DEBUGGING)*/

#if defined(CONFIG_TSC_CHECK_USING_TS_FILTER)
static void P_DEMUX_TSCMonTask(void *);
static void P_DEMUX_TsEventTask(void*);
static DRV_Error P_DEMUX_PrepareTSHandling(void);
DI_ERR_CODE P_DEMUX_StartTSFilter (int); //(HUINT32,HUINT16);
DI_ERR_CODE P_DEMUX_StopTSFilter (int);
#endif /*defined(TS_STATE_TEST)*/

#if defined(CONFIG_DI20)
static DRV_Error P_DEMUX_InsertPacketModeOn(HUINT32 ulDemuxId, HUINT16 usVirtualPid, HUINT16 usPid, HUINT8 *pucSection, HUINT32 ulSectionSize,
							HUINT32 *pulHandle, HUINT32 ulIntervalMS);
static DRV_Error P_DEMUX_InsertPacketModeOff(HUINT32 ulHandle);
#endif

#endif //_DI_DEMUX_PRIV_H_


/*********************************************************************

DI Demux Extension module

Module : DI Demux Extension
File name : di_demux_priv_ext.h

**********************************************************************/

#ifndef __DI_DEMUX_EXT_PRIV_H__
#define __DI_DEMUX_EXT_PRIV_H__

#include "di_err.h"
#include "di_config.h"
#include "nexus_platform.h"

#if defined(CONFIG_MAX_PLAYBACK_INSTANCE)
#define P_DEMUX_EXT_MAX_PLAY_INST_FOR_PB		CONFIG_MAX_PLAYBACK_INSTANCE
#else
#define P_DEMUX_EXT_MAX_PLAY_INST_FOR_PB		1
#endif

#if defined(CONFIG_MAX_TRANSCODER_CHANNEL)
#define P_DEMUX_EXT_MAX_PVR_INST_FOR_TRANS		CONFIG_MAX_TRANSCODER_CHANNEL
#else
#define P_DEMUX_EXT_MAX_PVR_INST_FOR_TRANS		0
#endif

#define	P_DEMUX_EXT_MAX_PLAY_INST_NUM			((P_DEMUX_EXT_MAX_PLAY_INST_FOR_PB)+(P_DEMUX_EXT_MAX_PVR_INST_FOR_TRANS))

/* Reserved 1 Parserband and 1 PB_Parserband for Allpass-mode. */
#if defined(CONFIG_BMEM_OPTIMIZE)
#define P_DEMUX_EXT_NUM_OF_RESERVED_CHANNEL		NUM_REMUX	/* live'remux parser band and playback' remux parser band */
#define P_DEMUX_EXT_MAX_IBPARSER_BAND_NUM		CONFIG_NUM_PARSER_BANDS
#else
#define P_DEMUX_EXT_NUM_OF_RESERVED_CHANNEL		2			/* live'remux parser band and playback' remux parser band */
#define P_DEMUX_EXT_MAX_IBPARSER_BAND_NUM		NEXUS_NUM_PARSER_BANDS
#endif

#define P_DEMUX_EXT_LIVE_DEMUX_NUM				(P_DEMUX_EXT_MAX_IBPARSER_BAND_NUM - P_DEMUX_EXT_NUM_OF_RESERVED_CHANNEL)
#define P_DEMUX_EXT_PLAYBACK_DEMUX_NUM			(P_DEMUX_EXT_MAX_PLAY_INST_NUM + 1)	/* +1 for 'descramble a file' */
#define P_DEMUX_EXT_MAX_DEMUX_NUM				(P_DEMUX_EXT_LIVE_DEMUX_NUM + P_DEMUX_EXT_PLAYBACK_DEMUX_NUM)

#define P_DEMUX_EXT_MAX_SECTION_FILTER_NUM		CONFIG_DEMUX_EXT_NUM_SECTION_FILTER
#define P_DEMUX_EXT_NUM_PID_CHANNEL				CONFIG_DEMUX_EXT_NUM_SECTION_FILTER

#define P_DEMUX_EXT_FOR_LIVE_CAPABILITY			DI_DEMUX_EXT_CAP_SECTIONFILTER

#define P_DEMUX_EXT_SECTION_FILTER_LENGTH		15
#define P_DEMUX_EXT_INTER_BUFFER_SIZE			5120
#define P_DEMUX_EXT_NUM_PHYS_CH_LOCK			CONFIG_DEMUX_EXT_NUM_SECTION_FILTER

#define P_DEMUX_EXT_ParserBand_ePb0				NEXUS_ParserBand_eMax
#define P_DEMUX_EXT_ParserBand_ePb1				(P_DEMUX_EXT_ParserBand_ePb0+1)


typedef enum _P_DEMUX_EXT_ChannelSubType {
	P_DEMUX_EXT_CHSUBT_eDSC,
	P_DEMUX_EXT_CHSUBT_eTSC,
	P_DEMUX_EXT_CHSUBT_eNone,
	P_DEMUX_EXT_CHSUBT_eMAX
} P_DEMUX_EXT_ChannelSubType;

#define P_DEMUX_EXT_ID_eHidden	100
typedef enum P_DEMUX_EXT_ID {
	P_DEMUX_EXT_ID_e0 = 0,
	P_DEMUX_EXT_ID_e1,
	P_DEMUX_EXT_ID_e2,
	P_DEMUX_EXT_ID_e3,
	P_DEMUX_EXT_ID_e4,
	P_DEMUX_EXT_ID_e5,
	P_DEMUX_EXT_ID_e6,
	P_DEMUX_EXT_ID_e7,
	P_DEMUX_EXT_ID_e8,
	P_DEMUX_EXT_ID_e9,
	P_DEMUX_EXT_ID_eMax
} P_DEMUX_EXT_ID;

typedef enum _P_DEMUX_EXT_FilterType{
	P_DEMUX_EXT_PesFilter = 0,
	P_DEMUX_EXT_PsiFilter,
	P_DEMUX_EXT_FilterType_MAX
}P_DEMUX_EXT_FilterType;

typedef struct _P_DEMUX_EXT_MSG_Struct
{
	unsigned int ulReqID;
	unsigned int ulFilterIDX;
}P_DEMUX_EXT_MSG_Struct;

typedef struct _P_DEMUX_EXT_PATH_INFO
{
	DI_DEMUX_EXT_INPUTDEVICETYPE etProducer;
	HUINT32 ulInputDevice_id;
	DI_DEMUX_EXT_OUTPUTDEVICETYPE etConsumer;
	HUINT32 ulConsumerGroupId;
	HINT32 nCI_Id;
	NEXUS_InputBand inputBand;
	HINT32 state;
	HUINT32  	ulRemuxId;
}P_DEMUX_EXT_PathInfo;

typedef struct _P_PER_DMX_EXT_STRUCT
{
	HUINT32  	ulDemuxID;
	DI_DEMUX_EXT_CAPABILITY stCapability;
	HBOOL bIsUsing;
	HULONG lock;
	HINT32 nHiddenParserIndex;
	P_DEMUX_EXT_PathInfo pathInfo;
}P_DEMUX_EXT_Device;

typedef enum P_DEMUX_EXT_ResourceStatus{
	P_DEMUX_EXT_RESOURCE_FREE = 0,
	P_DEMUX_EXT_RESOURCE_ALLOCATED,
	P_DEMUX_EXT_RESOURCE_ENABLED,
}P_DEMUX_EXT_ResourceStatus;

typedef struct P_DEMUX_EXT_PidChannel{
	HUINT32	demuxId;
	NEXUS_PidChannelHandle pidHandle;
	P_DEMUX_EXT_ResourceStatus status;
	DI_DEMUX_EXT_CHANNELTYPE channelType;
	HBOOL bTSCMonitornigOn;
	HUINT32 ulTSCMonitorIndex;
	P_DEMUX_EXT_ChannelSubType channelsubInfo; /*Used to find detailed info for DI_DEMUX_EXT_CHANNELTYPE_PRIVATE*/
	HUINT16 pid;
	HULONG lock;
}P_DEMUX_EXT_PidChannel;

typedef struct P_DEMUX_EXT_MsgFilter{
	NEXUS_MessageHandle msgHandle;
	P_DEMUX_EXT_ResourceStatus status;
	HUINT8 channelIndex;
	HUINT8 physicalChannelIndex;
	HUINT32 requestId;
	P_DEMUX_EXT_FilterType filterType;
	DI_DEMUX_EXT_PESTYPE etPesType;
}P_DEMUX_EXT_MsgFilter;

typedef struct _P_DEMUX_EXT_CALLBACK
{
	pfnDI_DEMUX_EXT_Notify		pfnSectionNotify;
	pfnDI_DEMUX_EXT_AllocBuffer	pfnAllocBuffer;
}P_DEMUX_EXT_Callback;

typedef struct _P_DEMUX_EXT_STRUCT
{
	P_DEMUX_EXT_Device			dmxDevice[P_DEMUX_EXT_MAX_DEMUX_NUM];
	P_DEMUX_EXT_PidChannel		pidChannel[P_DEMUX_EXT_NUM_PID_CHANNEL];
	P_DEMUX_EXT_MsgFilter		msgFilter[P_DEMUX_EXT_MAX_SECTION_FILTER_NUM];
	void						*msgFilterBuff[P_DEMUX_EXT_NUM_PID_CHANNEL];  /*msgBuffer's allocated only one per pidChannel*/
	HULONG						eventID[P_DEMUX_EXT_FilterType_MAX];
	P_DEMUX_EXT_Callback		callback;
}P_DEMUX_EXT_Module;

#endif


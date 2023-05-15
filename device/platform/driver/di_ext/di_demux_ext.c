/*********************************************************************

DI Demux Extension module

Module : DI Demux Extension
File name : di_demux_ext.c

**********************************************************************/
/* header */
#include <stdlib.h>
#include <stdarg.h>
#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/msg.h>

#include "nexus_base_types.h"
#include "nexus_platform.h"
#include "nexus_pid_channel.h"
#include "nexus_message.h"

#include "berr.h"
#include "bstd_defs.h"
#include "bdbg.h"
#include "blst_squeue.h"

#if defined(CONFIG_DEBUG)
#include <sys/resource.h>
#endif

#include "htype.h"
#include "vkernel.h"
#include "di_err.h"
#include "di_err_ext.h"
#include "di_demux_ext.h"
#include "di_demux_priv_ext.h"

/*********************************************************************/
/* define */

//#define DI_DEMUX_EXT_SEE_PARAMETERS

#define P_DEMUX_EXT_GET_FILTERTYPE(value)				((value & 0xffff0000)>>16)
#define P_DEMUX_EXT_GET_FILTERINDEX(value)				(value & 0x0000ffff)
#define P_DEMUX_EXT_MAKE_FILTERPAIRVALUE(type ,index)	(((type<<16) & 0xffff0000) |  (index & 0x0000ffff))
#define P_DEMUX_EXT_DMX_MSGQ_COUNT						1024

BLST_SQ_HEAD(msgfilterlist_head, msgfilterlist);
struct msgfilterlist_head gMsgHead;

struct msgfilterlist
{
	BLST_SQ_ENTRY(msgfilterlist) link;
	unsigned long filterIndex;
};

struct msgfilterlist gMsgFilter[P_DEMUX_EXT_MAX_SECTION_FILTER_NUM];

/*********************************************************************/
/* function */

/*********************************************************************/
/* static */
static HULONG sqLock;
static HULONG cbLockSection;
static HULONG chLock[P_DEMUX_EXT_NUM_PHYS_CH_LOCK];
static P_DEMUX_EXT_Module demuxInstance;

HUINT8 *inter_buffer = NULL;
HULONG sectionTaskId;

/*********************************************************************/
/* code */

static P_DEMUX_EXT_Module * P_DEMUX_EXT_GetInstance(void)
{
	return &demuxInstance;
}

static NEXUS_ParserBand P_DEMUX_EXT_GetIBParserForHiddenChannel(void)
{
	return (NEXUS_ParserBand)(P_DEMUX_EXT_MAX_IBPARSER_BAND_NUM-2);
}

static void P_DEMUX_EXT_NEXUS_NotifyCB(void *context, int param)
{
	P_DEMUX_EXT_Module *pDemux;
	P_DEMUX_EXT_MSG_Struct msg;
	int ret;
	unsigned long qID;

	pDemux = P_DEMUX_EXT_GetInstance();

	qID = pDemux->eventID[P_DEMUX_EXT_GET_FILTERTYPE((unsigned int)context)];

	msg.ulFilterIDX = P_DEMUX_EXT_GET_FILTERINDEX((unsigned int)context);
	msg.ulReqID = (unsigned long)param;
#if defined(DI_DEMUX_EXT_SEE_PARAMETERS)
	PrintDebug("P_DEMUX_EXT_NEXUS_NotifyCB(), filtertype = %x, filter index = %d reqid=%ld\n", P_DEMUX_EXT_GET_FILTERTYPE((unsigned int)context), msg.ulFilterIDX,msg.ulReqID );
#endif

	ret = VK_MSG_SendTimeout(qID, &msg, sizeof(P_DEMUX_EXT_MSG_Struct),0);
	if(ret == VK_TIMEOUT)
	{
		PrintError("Timeout occured! (filter_index = %d)\n", param);
	}

	return;
}

static void P_DEMUX_EXT_NEXUS_OverflowCB(void *context, int param)
{
#if !defined(CONFIG_DEBUG)
	BSTD_UNUSED(context);
	BSTD_UNUSED(param);
#endif
	PrintDebug("P_DEMUX_EXT_NEXUS_OverflowCB(), context = 0x%lx, param = %ld\n", context, param);

	return;
}

static HUINT32 P_DEMUX_EXT_GetSizeAligned(HUINT32 ulSizeNotAligned)
{
	if(ulSizeNotAligned%4)
	{
		ulSizeNotAligned += 4-(ulSizeNotAligned%4);
	}
	return ulSizeNotAligned;
}

static NEXUS_ParserBand P_DEMUX_EXT_GetParserBandByDemuxId(HUINT32 demuxId)
{
#if 0
	NEXUS_ParserBand parserBand;

	switch(demuxId)
	{
		case P_DEMUX_EXT_ID_e0 :
			parserBand = NEXUS_ParserBand_e0;
			break;
		default :
			parserBand = NEXUS_ParserBand_eInvalid;
			break;
	}

	return parserBand;
#else
	P_DEMUX_EXT_Module *pDemux;
	NEXUS_ParserBand parserBand = NEXUS_ParserBand_eInvalid;

#if P_DEMUX_EXT_MAX_IBPARSER_BAND_NUM == 5
	switch(demuxId){
		case P_DEMUX_EXT_ID_eHidden:
			parserBand = NEXUS_ParserBand_e3;
			break;
		case P_DEMUX_EXT_ID_e0 :
			parserBand = NEXUS_ParserBand_e0;
			break;
		case P_DEMUX_EXT_ID_e1 :
			parserBand = NEXUS_ParserBand_e1;
			break;
		case P_DEMUX_EXT_ID_e2 :
			parserBand = NEXUS_ParserBand_e2;
			break;
		case P_DEMUX_EXT_ID_e3 :
			pDemux = P_DEMUX_EXT_GetInstance();
			if(pDemux->dmxDevice[demuxId].nHiddenParserIndex == -1)
			{
				parserBand = P_DEMUX_EXT_ParserBand_ePb0;  /* parserBand is invalid if demuxId is for playback. Settings to a large value will flush out programming bugs. */
			}
			else
			{
				parserBand = NEXUS_ParserBand_e4;
			}
			break;
		case DRV_DEMUX_ID_e4 :
			pDemux = P_DEMUX_EXT_GetInstance();
			if(pDemux->dmxDevice[demuxId].nHiddenParserIndex == -1)
			{
				parserBand = P_DEMUX_EXT_ParserBand_ePb1;  /* parserBand is invalid if demuxId is for playback. Settings to a large value will flush out programming bugs. */
			}
			else
			{
				parserBand = NEXUS_ParserBand_e3;
			}
			break;
		default :
			parserBand = NEXUS_ParserBand_eInvalid;
			break;

	}
#else
	if(demuxId == P_DEMUX_EXT_ID_eHidden)
	{
		parserBand = P_DEMUX_EXT_GetIBParserForHiddenChannel();
	}
	else if(demuxId >= P_DEMUX_EXT_MAX_DEMUX_NUM)
	{
		parserBand = NEXUS_ParserBand_eInvalid;
	}
	else if(demuxId < P_DEMUX_EXT_LIVE_DEMUX_NUM)
	{
		parserBand = (NEXUS_ParserBand)demuxId;
	}
	else /*LIVE_DEMUX_NUM <= demuxId < MAX_DEMUX_NUM*/
	{
		pDemux = P_DEMUX_EXT_GetInstance();
		if(pDemux->dmxDevice[demuxId].nHiddenParserIndex == -1)
		{
			parserBand = P_DEMUX_EXT_ParserBand_ePb0+(demuxId-P_DEMUX_EXT_LIVE_DEMUX_NUM);
		}
		else
		{
			parserBand = (P_DEMUX_EXT_MAX_IBPARSER_BAND_NUM-1)-(demuxId-P_DEMUX_EXT_LIVE_DEMUX_NUM);
		}
	}
#endif

#if 0
 	//if(demuxId == DRV_DEMUX_ID_eHidden)
 	if (0)
	{
		//parserBand = P_DEMUX_EXT_GetIBParserForHiddenChannel();
	}
	else if(demuxId >= P_DEMUX_EXT_MAX_DEMUX_NUM)
	{
		parserBand = NEXUS_ParserBand_eInvalid;
	}
	else if(demuxId < P_DEMUX_EXT_LIVE_DEMUX_NUM)
	{
		parserBand = (NEXUS_ParserBand)demuxId;
	}
	else /*LIVE_DEMUX_NUM <= demuxId < MAX_DEMUX_NUM*/
	{
		pDemux = P_DEMUX_EXT_GetInstance();
		if(pDemux->dmxDevice[demuxId].nHiddenParserIndex == -1)
		{
			//parserBand = NEXUS_ParserBand_ePb0+(demuxId-P_DEMUX_EXT_LIVE_DEMUX_NUM);
		}
		else
		{
			parserBand = (P_DEMUX_EXT_MAX_IBPARSER_BAND_NUM-1)-(demuxId-P_DEMUX_EXT_LIVE_DEMUX_NUM);
		}
	}

#endif
	return parserBand;
#endif
}

static HBOOL P_DEMUX_EXT_HasCapability(HUINT32 ulDemuxId, DI_DEMUX_EXT_CAP cap)
{
	P_DEMUX_EXT_Module *pDemux;

	pDemux = P_DEMUX_EXT_GetInstance();
	return (pDemux->dmxDevice[ulDemuxId].stCapability.ulCapability & cap ? TRUE : FALSE);
}

static DI_EXT_Error P_DEMUX_EXT_AllocateChannel(DI_DEMUX_EXT_CHANNELTYPE channelType, HUINT16 pid, HUINT32 *channelIndex)
{
	P_DEMUX_EXT_Module *pDemux;
	HUINT8 channel_index;

	if(channelIndex == NULL)
	{
		PrintError(" invalid parameter \n");
		return DI_EXT_ERR_INVALID_PARAMETER;
	}

	pDemux = P_DEMUX_EXT_GetInstance();

	for(channel_index = 0; channel_index < P_DEMUX_EXT_NUM_PID_CHANNEL; channel_index++)
	{
		VK_SEM_Get(pDemux->pidChannel[channel_index].lock);
		if(pDemux->pidChannel[channel_index].status == P_DEMUX_EXT_RESOURCE_FREE)
		{
			pDemux->pidChannel[channel_index].status = P_DEMUX_EXT_RESOURCE_ALLOCATED;
			pDemux->pidChannel[channel_index].channelType = channelType;
			pDemux->pidChannel[channel_index].pid = pid;
			VK_SEM_Release(pDemux->pidChannel[channel_index].lock);
			*channelIndex = channel_index;
			PrintDebug("P_DEMUX_EXT_AllocateChannel(), channel %d is allocated\n", channel_index);
			return DI_EXT_OK;
		}
		VK_SEM_Release(pDemux->pidChannel[channel_index].lock);
	}

	PrintError(" no more pid channel to allocate\n");

	*channelIndex = DI_DEMUX_EXT_INVALID_CHANNEL_ID;
	return DI_EXT_ERR;
}

static DI_EXT_Error P_DEMUX_EXT_DeallocateChannel(HUINT32 channelIndex)
{
	P_DEMUX_EXT_Module *pDemux;

	if(channelIndex >= P_DEMUX_EXT_NUM_PID_CHANNEL)
	{
		PrintError(" invalid parameter (chIdx = %ld)\n",channelIndex);
		return DI_EXT_ERR_INVALID_PARAMETER;
	}

	pDemux = P_DEMUX_EXT_GetInstance();

	VK_SEM_Get(pDemux->pidChannel[channelIndex].lock);
	if(pDemux->pidChannel[channelIndex].status != P_DEMUX_EXT_RESOURCE_ALLOCATED)
	{
		VK_SEM_Release(pDemux->pidChannel[channelIndex].lock);
		PrintError(" trying to deallocate free or enabled channel\n");
		return DI_EXT_ERR;
	}
	pDemux->pidChannel[channelIndex].status = P_DEMUX_EXT_RESOURCE_FREE;
	VK_SEM_Release(pDemux->pidChannel[channelIndex].lock);

	return DI_EXT_OK;
}

static DI_EXT_Error P_DEMUX_EXT_EnableChannel(DI_DEMUX_EXT_CHANNELTYPE channelType, HUINT32 demuxId, HUINT32 channelIndex, P_DEMUX_EXT_ChannelSubType eChSubtype)
{
	P_DEMUX_EXT_Module *pDemux;
	NEXUS_ParserBand parserBand;
	NEXUS_PidChannelSettings pidChSettings;

	BSTD_UNUSED(channelType);

	parserBand = P_DEMUX_EXT_GetParserBandByDemuxId(demuxId);

	if(parserBand == NEXUS_ParserBand_eInvalid || channelIndex >= P_DEMUX_EXT_NUM_PID_CHANNEL)
	{
		PrintError(" invalid parameter\n");
		return DI_EXT_ERR_INVALID_PARAMETER;
	}

	pDemux = P_DEMUX_EXT_GetInstance();
	VK_SEM_Get(pDemux->pidChannel[channelIndex].lock);
	if(pDemux->pidChannel[channelIndex].status != P_DEMUX_EXT_RESOURCE_ALLOCATED)
	{
		VK_SEM_Release(pDemux->pidChannel[channelIndex].lock);
		PrintError(" pid channel %d is not allocated or already enabled\n", channelIndex);
		return DI_EXT_ERR;
	}
	pDemux->pidChannel[channelIndex].status = P_DEMUX_EXT_RESOURCE_ENABLED;

	NEXUS_PidChannel_GetDefaultSettings(&pidChSettings);
	pidChSettings.pidChannelIndex = NEXUS_PID_CHANNEL_OPEN_MESSAGE_CAPABLE;

	pDemux->pidChannel[channelIndex].pidHandle = NEXUS_PidChannel_Open(parserBand,
												pDemux->pidChannel[channelIndex].pid, &pidChSettings);
	PrintDebug(" Enable Channel(%d): 0x%x\n", channelIndex, pDemux->pidChannel[channelIndex].pidHandle);

	if(pDemux->pidChannel[channelIndex].pidHandle == NULL)
	{
		pDemux->pidChannel[channelIndex].status = P_DEMUX_EXT_RESOURCE_ALLOCATED;
		VK_SEM_Release(pDemux->pidChannel[channelIndex].lock);
		PrintError(" pid channel %d is not allocated or already enabled\n", channelIndex);
		return DI_EXT_ERR;
	}

	pDemux->pidChannel[channelIndex].demuxId = demuxId;
	pDemux->pidChannel[channelIndex].channelsubInfo = eChSubtype;

	VK_SEM_Release(pDemux->pidChannel[channelIndex].lock);

	return DI_EXT_OK;
}

static DI_EXT_Error P_DEMUX_EXT_DisableChannel(HUINT32 channelIndex)
{
	P_DEMUX_EXT_Module *pDemux;
	NEXUS_ParserBand parserBand;
	HUINT32	demuxId = 0;
	DI_DEMUX_EXT_CHANNELTYPE channelType = DI_DEMUX_EXT_CHANNELTYPE_MAX;

	if(channelIndex >= P_DEMUX_EXT_NUM_PID_CHANNEL)
	{
		PrintError("channelIndex %d is invalid\n", channelIndex);
		return DI_EXT_ERR_INVALID_PARAMETER;
	}

	pDemux = P_DEMUX_EXT_GetInstance();

	demuxId = pDemux->pidChannel[channelIndex].demuxId;
	parserBand = P_DEMUX_EXT_GetParserBandByDemuxId(pDemux->pidChannel[channelIndex].demuxId);
	if(parserBand == NEXUS_ParserBand_eInvalid)
	{
		PrintError("Invalid parameter\n");
		return DI_EXT_ERR_INVALID_PARAMETER;
	}

	VK_SEM_Get(pDemux->pidChannel[channelIndex].lock);
	if(pDemux->pidChannel[channelIndex].status != P_DEMUX_EXT_RESOURCE_ENABLED)
	{
		VK_SEM_Release(pDemux->pidChannel[channelIndex].lock);
		PrintError("trying to disable channel which hasn't been enabled\n");
		return DI_EXT_ERR;
	}

	pDemux->pidChannel[channelIndex].status = P_DEMUX_EXT_RESOURCE_ALLOCATED;

	NEXUS_PidChannel_Close(pDemux->pidChannel[channelIndex].pidHandle);

	channelType = pDemux->pidChannel[channelIndex].channelType;

	pDemux->pidChannel[channelIndex].pidHandle = NULL;
	pDemux->pidChannel[channelIndex].pid = DI_DEMUX_EXT_INVALID_PID;
	pDemux->pidChannel[channelIndex].demuxId = -1;
	pDemux->pidChannel[channelIndex].channelType = DI_DEMUX_EXT_CHANNELTYPE_MAX;
	pDemux->pidChannel[channelIndex].channelsubInfo = P_DEMUX_EXT_CHSUBT_eNone;

	VK_SEM_Release(pDemux->pidChannel[channelIndex].lock);

	return DI_EXT_OK;
}

static DI_EXT_Error P_DEMUX_EXT_AllocateFilter(P_DEMUX_EXT_FilterType eFilterType, DI_DEMUX_EXT_PESTYPE ePesType, HUINT8* filterIndex, HUINT32 requestId)
{
	P_DEMUX_EXT_Module *pDemux;
	HUINT8 filter_index;
	struct msgfilterlist *filtElem;

	if(filterIndex == NULL)
	{
		PrintError(" Invalid parameter\n");
		return DI_EXT_ERR_INVALID_PARAMETER;
	}

	pDemux = P_DEMUX_EXT_GetInstance();

	if(eFilterType >= P_DEMUX_EXT_FilterType_MAX)
	{
		PrintError(" Unknown filter type. (%d)\n", eFilterType);
		*filterIndex = 0xFF;
		return DI_EXT_ERR;
	}

	VK_SEM_Get(sqLock);
	if(BLST_SQ_EMPTY(&gMsgHead))
	{
		VK_SEM_Release(sqLock);
		PrintError(" no more available filter\n");
		*filterIndex = 0xff;
		return DI_EXT_ERR;
	}
	filtElem = BLST_SQ_FIRST(&gMsgHead);
	BLST_SQ_REMOVE_HEAD(&gMsgHead, link);
	filter_index = filtElem->filterIndex;
	VK_SEM_Release(sqLock);

	if(pDemux->msgFilter[filter_index].status != P_DEMUX_EXT_RESOURCE_FREE)
	{

		PrintError(" filter is not in free state\n");
		*filterIndex = 0xff;
		return DI_EXT_ERR;
	}
	pDemux->msgFilter[filter_index].status = P_DEMUX_EXT_RESOURCE_ALLOCATED;
	*filterIndex = filter_index;

	PrintDebug(" P_DEMUX_EXT_AllocateFilter: filter %d is allocated\n", filter_index);
	pDemux->msgFilter[filter_index].requestId = requestId;
	pDemux->msgFilter[filter_index].filterType = eFilterType;
	pDemux->msgFilter[filter_index].etPesType = ePesType;

	return DI_EXT_OK;
}

static DI_EXT_Error P_DEMUX_EXT_DeallocateFilter(P_DEMUX_EXT_FilterType eFilterType, HUINT8 filterIndex)
{
	P_DEMUX_EXT_Module *pDemux;

	if((eFilterType >= P_DEMUX_EXT_FilterType_MAX)||(filterIndex >= P_DEMUX_EXT_MAX_SECTION_FILTER_NUM))
	{
		PrintError(" Unknown filter type. (%d)\n", eFilterType);
		return DI_EXT_ERR;
	}

	pDemux = P_DEMUX_EXT_GetInstance();

	if(pDemux->msgFilter[filterIndex].status != P_DEMUX_EXT_RESOURCE_ALLOCATED)
	{
		PrintError(" filter %d is free or enabled\n", filterIndex);
		return DI_EXT_ERR;
	}
	pDemux->msgFilter[filterIndex].status = P_DEMUX_EXT_RESOURCE_FREE;
	VK_SEM_Get(sqLock);
	BLST_SQ_INSERT_TAIL(&gMsgHead, &gMsgFilter[filterIndex], link);
	VK_SEM_Release(sqLock);

	return DI_EXT_OK;
}

static NEXUS_MessageFormat P_DEMUX_EXT_GetNexusMessageFormat(P_DEMUX_EXT_FilterType eFilterType)
{
	NEXUS_MessageFormat nexusMsgFmt;

	switch(eFilterType)
	{
		case P_DEMUX_EXT_PsiFilter:
			nexusMsgFmt = NEXUS_MessageFormat_ePsi;
			break;

		default:
			nexusMsgFmt = NEXUS_MessageFormat_eMax;
			break;
	}
	return nexusMsgFmt;
}

static HUINT32 P_DEMUX_EXT_GetPhysicalPidChannelIndex(NEXUS_PidChannelHandle pidChannel)
{
	NEXUS_Error	nexusError;
	NEXUS_PidChannelStatus pidChStatus;

	nexusError = NEXUS_PidChannel_GetStatus(pidChannel, &pidChStatus );
	if(nexusError != NEXUS_SUCCESS)
	{
		/*Just display error and keep going*/
		PrintError(" Failed to get status of pidChannel 0x%lx\n", pidChannel);
		return 255;
	}
	return pidChStatus.pidChannelIndex;
}

static DI_EXT_Error P_DEMUX_EXT_EnableFilter(P_DEMUX_EXT_FilterType eFilterType, HUINT8 filterIndex, HUINT8 channelIndex,
										HUINT8 *filterValue, HUINT8 *filterMask, HUINT8 *filterExclusion,
										HUINT32 filterLen, HBOOL filterMode, HBOOL bCheckCRC, HUINT32 ulRequestID)
{
	P_DEMUX_EXT_Module *pDemux;
	NEXUS_MessageSettings openSettings;
	NEXUS_MessageStartSettings startSettings;
	HUINT32 i;
	HUINT32 buffIndex;

#if !defined(CONFIG_DEBUG)
	BSTD_UNUSED(filterExclusion);
#endif

	if((eFilterType >= P_DEMUX_EXT_FilterType_MAX)||(filterIndex >= P_DEMUX_EXT_MAX_SECTION_FILTER_NUM))
	{
		PrintError(" Unknown filter type. (%d)\n", eFilterType);
		return DI_EXT_ERR;
	}

	pDemux = P_DEMUX_EXT_GetInstance();

	pDemux->msgFilter[filterIndex].channelIndex = channelIndex;
	pDemux->msgFilter[filterIndex].physicalChannelIndex = filterIndex;
	VK_SEM_Get(chLock[pDemux->msgFilter[filterIndex].physicalChannelIndex]);

	if(pDemux->msgFilter[filterIndex].status != P_DEMUX_EXT_RESOURCE_ALLOCATED)
	{
		VK_SEM_Release(chLock[pDemux->msgFilter[filterIndex].physicalChannelIndex]);
		PrintError(" filter %d is free or enabled\n", filterIndex);
		return DI_EXT_ERR;
	}
	pDemux->msgFilter[filterIndex].status = P_DEMUX_EXT_RESOURCE_ENABLED;

	NEXUS_Message_GetSettings(pDemux->msgFilter[filterIndex].msgHandle, &openSettings);
	openSettings.dataReady.callback = P_DEMUX_EXT_NEXUS_NotifyCB;
	openSettings.dataReady.context =  (void *)P_DEMUX_EXT_MAKE_FILTERPAIRVALUE(eFilterType ,filterIndex);
	openSettings.dataReady.param = ulRequestID;
	openSettings.overflow.callback = P_DEMUX_EXT_NEXUS_OverflowCB;
	openSettings.overflow.context = (void *)P_DEMUX_EXT_MAKE_FILTERPAIRVALUE(eFilterType ,filterIndex);
	openSettings.overflow.param = ulRequestID;
	openSettings.bufferSize =0;
	NEXUS_Message_SetSettings(pDemux->msgFilter[filterIndex].msgHandle, &openSettings);
	NEXUS_Message_GetDefaultStartSettings(pDemux->msgFilter[filterIndex].msgHandle,&startSettings);

	PrintDebug(" filter (%d) Handle : 0x%x\n", filterIndex, pDemux->msgFilter[filterIndex]);
	PrintDebug(" pid channel (%d) Handle : 0x%x\n", channelIndex, pDemux->pidChannel[channelIndex].pidHandle);

	startSettings.pidChannel = pDemux->pidChannel[channelIndex].pidHandle;
	startSettings.format = P_DEMUX_EXT_GetNexusMessageFormat(eFilterType);
	startSettings.filterGroup = false;
	startSettings.bufferMode = (filterMode == TRUE ? NEXUS_MessageBufferMode_eOneMessage : NEXUS_MessageBufferMode_eContinuous);
	startSettings.bank = -1;
	startSettings.psiCrcDisabled = !bCheckCRC;
	startSettings.psfCrcDisabled = !bCheckCRC;

	buffIndex = P_DEMUX_EXT_GetPhysicalPidChannelIndex(pDemux->pidChannel[channelIndex].pidHandle);
	if(buffIndex == 255)
	{
		/*Don't close filter*/
		VK_SEM_Release(chLock[pDemux->msgFilter[filterIndex].physicalChannelIndex]);
		pDemux->msgFilter[filterIndex].status = P_DEMUX_EXT_RESOURCE_ALLOCATED;
		PrintError("This pid Channel can't be used for Message filtering!. [pidchannel = %d]\n", buffIndex);
		return DI_EXT_ERR;

	}
	PrintDebug("BUFFINDEX = %d filterIndex = %d\n",buffIndex, filterIndex);
	startSettings.buffer = pDemux->msgFilterBuff[filterIndex];
	startSettings.bufferSize = DI_DEMUX_EXT_MANAGED_FILTER_SIZE;

	if(eFilterType == P_DEMUX_EXT_PsiFilter)
	{
		PrintDebug("[SEC] value : ");
		for(i=0; i<filterLen; i++)
			PrintDebug("%02x ", filterValue[i]);
		PrintDebug("\n");

		PrintDebug("[SEC] mask : ");
		for(i=0; i<filterLen; i++)
			PrintDebug("%02x ", filterMask[i]);
		PrintDebug("\n");

		PrintDebug("[SEC] exclu : ");
		for(i=0; i<filterLen; i++)
			PrintDebug("%02x ", filterExclusion[i]);
		PrintDebug("\n");

		VK_memset(startSettings.filter.coefficient, 0x00, 16);
		VK_memset(startSettings.filter.exclusion, 0xff, 16);
		VK_memset(startSettings.filter.mask, 0xff, 16);

		startSettings.filter.coefficient[0] = filterValue[0];
		startSettings.filter.mask[0] = ~filterMask[0];

		for(i=1; i<filterLen; i++)
		{
			startSettings.filter.coefficient[i+1] = filterValue[i];
			startSettings.filter.mask[i+1] = ~filterMask[i];
		}
	}

	if(NEXUS_Message_Start(pDemux->msgFilter[filterIndex].msgHandle, &startSettings) != NEXUS_SUCCESS)
	{
		/*Don't close filter*/
		VK_SEM_Release(chLock[pDemux->msgFilter[filterIndex].physicalChannelIndex]);
		pDemux->msgFilter[filterIndex].status = P_DEMUX_EXT_RESOURCE_ALLOCATED;
		PrintError(" NEXUS_Message_Start() return error\n");
		return DI_EXT_ERR;
	}
	VK_SEM_Release(chLock[pDemux->msgFilter[filterIndex].physicalChannelIndex]);

	return DI_EXT_OK;
}

static DI_EXT_Error P_DEMUX_EXT_DisableFilter(P_DEMUX_EXT_FilterType eFilterType, HUINT8 filterIndex)
{
	P_DEMUX_EXT_Module *pDemux;
	HUINT8 tempChannelIndex;

	if((eFilterType >= P_DEMUX_EXT_FilterType_MAX)||(filterIndex >= P_DEMUX_EXT_MAX_SECTION_FILTER_NUM))
	{
		PrintError(" Unknown filter type. (%d)\n", eFilterType);
		return DI_EXT_ERR;
	}

	pDemux = P_DEMUX_EXT_GetInstance();

	VK_SEM_Get(chLock[pDemux->msgFilter[filterIndex].physicalChannelIndex]);

	if(pDemux->msgFilter[filterIndex].status != P_DEMUX_EXT_RESOURCE_ENABLED)
	{
		VK_SEM_Release(chLock[pDemux->msgFilter[filterIndex].physicalChannelIndex]);
		PrintError(" filter %d is not enabled\n", filterIndex);
		return DI_EXT_ERR;
	}
	pDemux->msgFilter[filterIndex].status = P_DEMUX_EXT_RESOURCE_ALLOCATED;

	NEXUS_Message_Stop(pDemux->msgFilter[filterIndex].msgHandle);
	tempChannelIndex = pDemux->msgFilter[filterIndex].channelIndex;
	pDemux->msgFilter[filterIndex].channelIndex = DI_DEMUX_EXT_INVALID_CHANNEL_ID;
	pDemux->msgFilter[filterIndex].requestId = 0;

	VK_SEM_Release(chLock[pDemux->msgFilter[filterIndex].physicalChannelIndex]);

	return DI_EXT_OK;
}

static void P_DEMUX_EXT_SectionEventTask(void *pParam)
{
	P_DEMUX_EXT_Module *pDemux;
	P_DEMUX_EXT_MSG_Struct msg;

	DI_EXT_Error eDiExtErr = DI_EXT_ERR;
	HULONG filter_index;
	HULONG requestId;

	HUINT32 message_length;
	HUINT32 vkRet = 0;
	HUINT32 ulRetrieveCount = 0;

	HUINT8 *buffer, *tmp_buf;

	HBOOL bEventReceived = FALSE;

	NEXUS_Error eNexusErr;

	size_t size;

	BSTD_UNUSED(pParam);
	pDemux = P_DEMUX_EXT_GetInstance();

	while(TRUE)
	{
		vkRet = VK_MSG_Receive(pDemux->eventID[P_DEMUX_EXT_PsiFilter], &msg, sizeof(P_DEMUX_EXT_MSG_Struct));
		if(vkRet != VK_OK)
		{
			PrintError("VK_MSG_Receive returns error. (err=%ld)\n", vkRet);
			continue;
		}

		bEventReceived = TRUE;
		filter_index = msg.ulFilterIDX;
		requestId = msg.ulReqID;
#if defined(DI_DEMUX_EXT_SEE_PARAMETERS)
		PrintDebug("P_DEMUX_EXT_SectionEventTask(), filter_index = %d  requestId = 0x%lx\n", filter_index, requestId);
#endif
		//Check if filter is still alive.
		VK_SEM_Get(chLock[pDemux->msgFilter[filter_index].physicalChannelIndex]);
		ulRetrieveCount = 0;
RETRIEVE_AGAIN:
		if(ulRetrieveCount > 0)
		{
			ulRetrieveCount--;
		}
		if(pDemux->msgFilter[filter_index].status== P_DEMUX_EXT_RESOURCE_ENABLED &&
			requestId == pDemux->msgFilter[filter_index].requestId)
		{
			if((eNexusErr=NEXUS_Message_GetBuffer(pDemux->msgFilter[filter_index].msgHandle, (const void **)&buffer, &size)))
			{
				VK_SEM_Release(chLock[pDemux->msgFilter[filter_index].physicalChannelIndex]);
				PrintError("P_DEMUX_EXT_SectionEventTask(), NEXUS_Message_GetBuffer() return error (%ld)[filter_index=%d]\n",eNexusErr, filter_index);
				continue;
			}

#if defined(DI_DEMUX_EXT_SEE_PARAMETERS)
			if(buffer[0] == 0x0)
			{
				PrintDebug("[%s] PAT message!\n", __func__);
				int programNum;
#define TS_READ_16( BUF ) ((uint16_t)((BUF)[0]<<8|(BUF)[1]))
#define TS_PSI_GET_SECTION_LENGTH( BUF )    (uint16_t)(TS_READ_16( &(BUF)[1] ) & 0x0FFF)

				/* We should always get whole PAT's because maxContiguousMessageSize is 4K */
				message_length = TS_PSI_GET_SECTION_LENGTH(buffer) + 3;
				BDBG_ASSERT(size >= (size_t)message_length);

				PrintDebug("[%s] Found PAT: id=%d size=%d\n", __func__, buffer[0], message_length);
				for (programNum=0;programNum<(TS_PSI_GET_SECTION_LENGTH(buffer)-7)/4;programNum++) {
					unsigned byteOffset = 8 + programNum*4;
					PrintDebug("[%s]  program %d: pid 0x%x\n", __func__,
						TS_READ_16( &buffer[byteOffset] ),
						(uint16_t)(TS_READ_16( &buffer[byteOffset+2] ) & 0x1FFF));
				}
			}
#endif

			if(!size)
			{
				VK_SEM_Release(chLock[pDemux->msgFilter[filter_index].physicalChannelIndex]);
				/*size can be zero in normal condition. printf("P_DEMUX_EXT_SectionEventTask(), NEXUS_Message_GetBuffer() read size 0\n");*/
				continue;
			}
			if(size < 3)
			{
				VK_SEM_Release(chLock[pDemux->msgFilter[filter_index].physicalChannelIndex]);
				/*nothing to do without section size info. this case can be happen in pid2buf mode.*/
				continue;
			}
			message_length = (((buffer[1]&0xF)<<8)|(buffer[2]))+3;
			message_length = P_DEMUX_EXT_GetSizeAligned(message_length);

			if(size == message_length && message_length <= P_DEMUX_EXT_INTER_BUFFER_SIZE)
			{
				VK_memcpy(inter_buffer, buffer, message_length);
				NEXUS_Message_ReadComplete(pDemux->msgFilter[filter_index].msgHandle, message_length);
			}
			else if(size > message_length && message_length <= P_DEMUX_EXT_INTER_BUFFER_SIZE)
			{
				VK_memcpy(inter_buffer, buffer, message_length);
				NEXUS_Message_ReadComplete(pDemux->msgFilter[filter_index].msgHandle, message_length);
				if(message_length <= 94 && bEventReceived == TRUE)
				{
					bEventReceived = FALSE;
					ulRetrieveCount = (188/message_length)-1;
				}
			}
			else if(size < message_length)
			{

				NEXUS_Message_ReadComplete(pDemux->msgFilter[filter_index].msgHandle, 0);
				VK_SEM_Release(chLock[pDemux->msgFilter[filter_index].physicalChannelIndex]);
				/*waits next event. this case can be happen in pid2buf mode.*/
				continue;
			}
			else
			{
				NEXUS_Message_ReadComplete(pDemux->msgFilter[filter_index].msgHandle, size);
				VK_SEM_Release(chLock[pDemux->msgFilter[filter_index].physicalChannelIndex]);
				PrintError(" message_length(%ld) != size(%ld) or section is too big  [filter_index=%d]\n", message_length, size, filter_index);
				while(1);
			}

#if defined(DI_DEMUX_EXT_SEE_PARAMETERS)
			if(inter_buffer[0] == 0x00)
			{
				PrintDebug("[recv] : ");
				for(i=0; i<6; i++)
					PrintDebug("%02x ", inter_buffer[i]);
				PrintDebug("\n");
			}
#endif
			VK_SEM_Get(cbLockSection);
			if( pDemux->callback.pfnAllocBuffer != NULL  && pDemux->callback.pfnSectionNotify!= NULL)
			{
				eDiExtErr = pDemux->callback.pfnAllocBuffer(message_length, &tmp_buf);
				if(eDiExtErr != DI_EXT_OK)
				{
					VK_SEM_Release(cbLockSection);
					VK_SEM_Release(chLock[pDemux->msgFilter[filter_index].physicalChannelIndex]);
					PrintError("pfnAllocBuffer() return error\n");
					continue;
				}
				VK_memcpy(tmp_buf, inter_buffer, message_length);
				eDiExtErr = pDemux->callback.pfnSectionNotify(pDemux->pidChannel[pDemux->msgFilter[filter_index].channelIndex].demuxId \
												, tmp_buf, pDemux->msgFilter[filter_index].requestId);
				if(eDiExtErr != DI_EXT_OK)
				{
					PrintError("pfnSectionNotify() return error filter_index=%ld, reqid=0x%x\n", filter_index, pDemux->msgFilter[filter_index].requestId);
				}
			}
			VK_SEM_Release(cbLockSection);

			if(ulRetrieveCount > 0)
			{
				goto RETRIEVE_AGAIN;
			}

			VK_SEM_Release(chLock[pDemux->msgFilter[filter_index].physicalChannelIndex]);

		}
		else
		{
			VK_SEM_Release(chLock[pDemux->msgFilter[filter_index].physicalChannelIndex]);
			PrintError("%s, Message Filter (filterIdx: %ld) is already stopped.\n",__FUNCTION__, filter_index);\
			continue;
		}
	}
}

static DI_EXT_Error P_DEMUX_EXT_PrepareSectionHandling(void)
{
	P_DEMUX_EXT_Module *pDemux;
	HUINT32 i;
	HUINT32 vkErr = 0;

	pDemux = P_DEMUX_EXT_GetInstance();
	for(i=0; i<P_DEMUX_EXT_MAX_SECTION_FILTER_NUM; i++){
		pDemux->msgFilter[i].status = P_DEMUX_EXT_RESOURCE_FREE;
	}

	vkErr = VK_SEM_Create(&sqLock, "P_DEMUX_EXT_DmxSQ_SEM", VK_SUSPENDTYPE_FIFO);
	if(vkErr != VK_OK)
	{
		PrintError("%s->VK_SEM_Create(): Error P_DEMUX_EXT_DmxSQ_SEM, Code(%d), Line(%d), %s\n", __FUNCTION__, vkErr, __LINE__, __FILE__);
		return DI_EXT_ERR;
	}

	BLST_SQ_INIT(&gMsgHead);
	for(i=0; i<P_DEMUX_EXT_MAX_SECTION_FILTER_NUM; i++)
	{
		gMsgFilter[i].filterIndex = i;
		BLST_SQ_INSERT_TAIL(&gMsgHead, &gMsgFilter[i], link);
	}

	vkErr = VK_MSG_Create(P_DEMUX_EXT_DMX_MSGQ_COUNT, sizeof(P_DEMUX_EXT_MSG_Struct),"P_DEMUX_EXT_PsiMsg",&pDemux->eventID[P_DEMUX_EXT_PsiFilter],VK_SUSPENDTYPE_FIFO);
	if(vkErr != VK_OK)
	{
		PrintError("%s->VK_MSG_Create(): Error P_DEMUX_EXT_PsiMsg, Code(%d), Line(%d), %s\n", __FUNCTION__, vkErr, __LINE__, __FILE__);
		return DI_EXT_ERR;
	}

	vkErr = VK_TASK_Create(P_DEMUX_EXT_SectionEventTask,
						40,
						(1024*16),
						"P_DEMUX_EXT_SecTask",
						NULL,
						&sectionTaskId, FALSE);
	if( vkErr != VK_OK )
	{
		VK_MSG_Destroy(pDemux->eventID[P_DEMUX_EXT_PsiFilter]);
		PrintError("%s->VK_TASK_Create(): Error P_DEMUX_EXT_SecTask, Code(%d), Line(%d), %s\n", __FUNCTION__, vkErr, __LINE__, __FILE__);
		return DI_EXT_ERR;
	}

	vkErr = VK_TASK_Start(sectionTaskId);
	if(vkErr != VK_OK)
	{
		VK_MSG_Destroy(pDemux->eventID[P_DEMUX_EXT_PsiFilter]);
		VK_TASK_Destroy(sectionTaskId);
		PrintError("%s->VK_TASK_Start(): Error P_DEMUX_EXT_SecTask, Code(%d), Line(%d), %s\n", __FUNCTION__, vkErr, __LINE__, __FILE__);
		return DI_EXT_ERR;
	}

	return DI_EXT_OK;
}

static DI_EXT_Error P_DEMUX_EXT_ReleaseFilter(void)
{
	P_DEMUX_EXT_Module *pDemux;
	HUINT8 filter_index;

	pDemux = P_DEMUX_EXT_GetInstance();
	for(filter_index = 0; filter_index < P_DEMUX_EXT_NUM_PID_CHANNEL; filter_index++)
	{
		if(pDemux->msgFilterBuff[filter_index] != NULL)
		{
			NEXUS_Memory_Free(pDemux->msgFilterBuff[filter_index]);
		}
	}

	return DI_EXT_OK;
}

static DI_EXT_Error P_DEMUX_EXT_InitFilter(void)
{
	P_DEMUX_EXT_Module *pDemux;
	HUINT8 filter_index;
	NEXUS_Error eNexusErr;
	NEXUS_MemoryAllocationSettings memAllocationSettings;

	pDemux = P_DEMUX_EXT_GetInstance();

	for(filter_index = 0; filter_index < P_DEMUX_EXT_NUM_PID_CHANNEL; filter_index++)
	{
		NEXUS_Memory_GetDefaultAllocationSettings(&memAllocationSettings);
		memAllocationSettings.alignment = 1024;
		eNexusErr = NEXUS_Memory_Allocate(DI_DEMUX_EXT_MANAGED_FILTER_SIZE, &memAllocationSettings, &pDemux->msgFilterBuff[filter_index]);
		if(eNexusErr != BERR_SUCCESS)
		{
			PrintError("%s->Failed allocating from Nexus Heap. (%d)\n", __FUNCTION__, eNexusErr);
			goto InitFilter_ReleaseResources;
		}
	}

	return DI_EXT_OK;

InitFilter_ReleaseResources:

	P_DEMUX_EXT_ReleaseFilter();

	return DI_EXT_ERR;
}

DI_ERR_CODE DI_DEMUX_EXT_GetTotalCapability(DI_DEMUX_EXT_TOTALCAPABILITY * ptotalstCapability)
{
	PrintEnter();

	if(ptotalstCapability == NULL)
	{
		return DI_ERR_INVALID_PARAM;
	}

	ptotalstCapability->NumOfDemux = (HUINT32)P_DEMUX_EXT_MAX_DEMUX_NUM;
	ptotalstCapability->NumOfTotalSectoinFilter = (HUINT32)P_DEMUX_EXT_MAX_SECTION_FILTER_NUM;

	PrintExit();

	return DI_ERR_OK;
}

DI_ERR_CODE DI_DEMUX_EXT_GetCapability(HUINT32 ulDemuxID, DI_DEMUX_EXT_CAPABILITY * pstCapability)
{
	P_DEMUX_EXT_Module *pDemux;

	PrintEnter();

	pDemux = P_DEMUX_EXT_GetInstance();
	if(pstCapability == NULL || ulDemuxID >= P_DEMUX_EXT_MAX_DEMUX_NUM)
	{
		return DI_ERR_INVALID_PARAM;
	}

	VK_memcpy(pstCapability, &pDemux->dmxDevice[ulDemuxID].stCapability, sizeof(DI_DEMUX_EXT_CAPABILITY));

	PrintExit();

	return DI_ERR_OK;
}

DI_ERR_CODE DI_DEMUX_EXT_RegisterSectionCallback (HUINT32 ulDemuxId, pfnDI_DEMUX_EXT_Notify pfnNotify, pfnDI_DEMUX_EXT_AllocBuffer pfnAllocBuffer)
{
	P_DEMUX_EXT_Module *pDemux;

	BSTD_UNUSED(ulDemuxId);

	PrintEnter();

	pDemux = P_DEMUX_EXT_GetInstance();

	if(ulDemuxId >= P_DEMUX_EXT_MAX_DEMUX_NUM )
	{
		PrintError(" failed: invalid parameter \n");
		return DI_ERR_INVALID_PARAM;
	}

	VK_SEM_Get(cbLockSection);
	pDemux->callback.pfnSectionNotify = pfnNotify;
	pDemux->callback.pfnAllocBuffer = pfnAllocBuffer;
	VK_SEM_Release(cbLockSection);

	PrintExit();

	return DI_ERR_OK;

}

DI_ERR_CODE DI_DEMUX_EXT_SetSectionFilter (HUINT32 ulDemuxId, HUINT16 usPid, HUINT32 ulRequestID, HBOOL bIsOnce,
											HUINT32 ulFilterLen, HUINT8 * pucValue,HUINT8 * pucMask,HUINT8 * pucExclusion,
											HBOOL bCheckCRC)
{
	DI_EXT_Error eDIExtErr = DI_EXT_ERR;
	HUINT32 ulChannelIndex;
	HUINT8 ucFilterIindex;

	PrintEnter();

	PrintDebug(" ulDemuxId %d, usPid=0x%x, ulRequestID 0x%lx, bIsOnce %d\n", ulDemuxId, usPid, ulRequestID, bIsOnce);

	if(ulDemuxId >= P_DEMUX_EXT_MAX_DEMUX_NUM || usPid >= DI_DEMUX_EXT_INVALID_PID || pucValue == NULL || pucMask == NULL || pucExclusion == NULL)
	{
		PrintError(" Failed: invalid parameter \n");
		return DI_ERR_INVALID_PARAM;
	}

	if(ulFilterLen > P_DEMUX_EXT_SECTION_FILTER_LENGTH)
	{
		PrintError(" Failed: not supported filter length %d\n", ulFilterLen);
		return DI_ERR_INVALID_PARAM;
	}

	if(P_DEMUX_EXT_HasCapability(ulDemuxId, DI_DEMUX_EXT_CAP_SECTIONFILTER) == FALSE)
	{
		PrintError(" demux %d has no capability SECTION_FILTER\n", ulDemuxId);
		return DI_ERR_ERROR;
	}

	eDIExtErr = P_DEMUX_EXT_AllocateChannel(DI_DEMUX_EXT_CHANNELTYPE_SECTION, usPid, &ulChannelIndex);
	if(eDIExtErr != DI_EXT_OK || ulChannelIndex == DI_DEMUX_EXT_INVALID_CHANNEL_ID)
	{
		PrintError(" Failed: P_DEMUX_EXT_AllocateChannel() return error %d \n", eDIExtErr);
		return DI_ERR_ERROR;
	}

	eDIExtErr = P_DEMUX_EXT_EnableChannel(DI_DEMUX_EXT_CHANNELTYPE_SECTION, ulDemuxId, ulChannelIndex, P_DEMUX_EXT_CHSUBT_eNone);
	if(eDIExtErr != DI_EXT_OK)
	{
		PrintError(" Failed: P_DEMUX_EXT_EnableChannel() return error %d \n", eDIExtErr);
		P_DEMUX_EXT_DeallocateChannel(ulChannelIndex);
		return DI_ERR_ERROR;
	}

	eDIExtErr = P_DEMUX_EXT_AllocateFilter(P_DEMUX_EXT_PsiFilter, 0, &ucFilterIindex, ulRequestID);
	if(eDIExtErr != DI_EXT_OK)
	{
		PrintError(" P_DEMUX_EXT_AllocateFilter() return error\n");
		P_DEMUX_EXT_DisableChannel(ulChannelIndex);
		P_DEMUX_EXT_DeallocateChannel(ulChannelIndex);
		return DI_ERR_ERROR;
	}

	eDIExtErr = P_DEMUX_EXT_EnableFilter(P_DEMUX_EXT_PsiFilter, ucFilterIindex, ulChannelIndex, pucValue, pucMask, pucExclusion, ulFilterLen, bIsOnce, bCheckCRC, ulRequestID);
	if(eDIExtErr != DI_EXT_OK)
	{
		PrintError(" P_DEMUX_EXT_EnableFilter() return error\n");
		P_DEMUX_EXT_DeallocateFilter(P_DEMUX_EXT_PsiFilter, ucFilterIindex);
		P_DEMUX_EXT_DisableChannel(ulChannelIndex);
		P_DEMUX_EXT_DeallocateChannel(ulChannelIndex);
		return DI_ERR_ERROR;
	}

	PrintExit();

	return DI_ERR_OK;
}

DI_ERR_CODE DI_DEMUX_EXT_ReleaseSectionFilter (HUINT32 ulDemuxId, HUINT32	ulRequestID)
{
	DI_EXT_Error eDIExtErr = DI_EXT_ERR;
	P_DEMUX_EXT_Module *pDemux;
	HUINT8 filter_index, channel_index;

	BSTD_UNUSED(ulDemuxId);

	PrintEnter();

	PrintDebug("ulDemuxId %d, ulRequestID 0x%lx\n", ulDemuxId, ulRequestID);

	pDemux = P_DEMUX_EXT_GetInstance();

	for(filter_index=0; filter_index<P_DEMUX_EXT_MAX_SECTION_FILTER_NUM; filter_index++)
	{
		if(pDemux->msgFilter[filter_index].requestId == ulRequestID && pDemux->msgFilter[filter_index].filterType == P_DEMUX_EXT_PsiFilter
			&& pDemux->msgFilter[filter_index].status == P_DEMUX_EXT_RESOURCE_ENABLED
			&& pDemux->pidChannel[pDemux->msgFilter[filter_index].channelIndex].demuxId == ulDemuxId){
			break;
		}
	}

	if(filter_index == P_DEMUX_EXT_MAX_SECTION_FILTER_NUM)
	{
		PrintError(" no filter to release with RequestID(0x%lx)\n", ulRequestID);
		return DI_ERR_ERROR;
	}

	channel_index = pDemux->msgFilter[filter_index].channelIndex;

	eDIExtErr = P_DEMUX_EXT_DisableFilter(P_DEMUX_EXT_PsiFilter, filter_index);
	if(eDIExtErr != DI_EXT_OK)
	{
		PrintError(" P_DEMUX_EXT_DisableFilter() returns error.\n");
		return DI_ERR_ERROR;
	}

	eDIExtErr = P_DEMUX_EXT_DeallocateFilter(P_DEMUX_EXT_PsiFilter, filter_index);
	if(eDIExtErr != DI_EXT_OK)
	{
		PrintError(" P_DEMUX_EXT_DeallocateFilter() returns error.\n");
		return DI_ERR_ERROR;
	}

	eDIExtErr = P_DEMUX_EXT_DisableChannel(channel_index);
	if(eDIExtErr != DI_EXT_OK)
	{
		PrintError(" P_DEMUX_EXT_DisableChannel() returns error.\n");
		return DI_ERR_ERROR;
	}

	eDIExtErr = P_DEMUX_EXT_DeallocateChannel(channel_index);
	if(eDIExtErr != DI_EXT_OK)
	{
		PrintError(" P_DEMUX_EXT_DeallocateChannel() returns error.\n");
		return DI_ERR_ERROR;
	}
	PrintExit();

	return DI_ERR_OK;
}

DI_ERR_CODE DI_DEMUX_EXT_Init(void)
{
	P_DEMUX_EXT_Module *pDemux = NULL;

	NEXUS_MessageSettings openSettings;

	HUINT32 i = 0, vkErr = VK_ERROR;
	DI_EXT_Error eDiExtErr = DI_EXT_ERR;

	pDemux = P_DEMUX_EXT_GetInstance();

	for(i = 0; i < P_DEMUX_EXT_MAX_DEMUX_NUM; i++)
	{
		pDemux->dmxDevice[i].ulDemuxID = i;
		pDemux->dmxDevice[i].stCapability.ulCapability = DI_DEMUX_EXT_CAP_SECTIONFILTER;
		pDemux->dmxDevice[i].stCapability.NumOfPESFilter = 0;
		pDemux->dmxDevice[i].stCapability.NumOfSectoinFilter = P_DEMUX_EXT_MAX_SECTION_FILTER_NUM;
		pDemux->dmxDevice[i].stCapability.NumOfSectoinFilterPerPid = 32;	/* 일단 32개로 제한 한다 */
		pDemux->dmxDevice[i].bIsUsing = FALSE;
		pDemux->dmxDevice[i].stCapability.NumOfMaximumFilterLength = 15;
		pDemux->dmxDevice[i].pathInfo.nCI_Id = -1; /*default value: -1 (CI not used)*/
		pDemux->dmxDevice[i].pathInfo.etProducer = DI_DEMUX_EXT_INPUT_NONE;
		pDemux->dmxDevice[i].pathInfo.ulInputDevice_id = 0xff;
		pDemux->dmxDevice[i].pathInfo.etConsumer = DI_DEMUX_EXT_OUTPUT_NONE;
		pDemux->dmxDevice[i].pathInfo.ulConsumerGroupId = 0;
		pDemux->dmxDevice[i].pathInfo.inputBand = 0xff; /*default value: 255 (not used)*/
		pDemux->dmxDevice[i].pathInfo.ulRemuxId = 0xff; /*default value: 255 (not used)*/
		pDemux->dmxDevice[i].nHiddenParserIndex = -1;
		vkErr = VK_SEM_Create(&pDemux->dmxDevice[i].lock, "DI_DEMUX_EXT_SEM", VK_SUSPENDTYPE_FIFO);
		if(vkErr != VK_OK)
		{
			PrintError(" VK_SEM_Create(): Error DI_DEMUX_EXT_SEM, Code(%d)\n", vkErr);
			return DI_ERR_ERROR;
		}
	}

	for(i = 0; i < P_DEMUX_EXT_NUM_PID_CHANNEL; i++)
	{
		pDemux->pidChannel[i].pidHandle = NULL;
		pDemux->pidChannel[i].status = P_DEMUX_EXT_RESOURCE_FREE;
		pDemux->pidChannel[i].pid = 0xffff;
		pDemux->pidChannel[i].channelType = DI_DEMUX_EXT_CHANNELTYPE_MAX;
		pDemux->pidChannel[i].bTSCMonitornigOn = FALSE; /*TSC monitor sets false by default.*/

		vkErr = VK_SEM_Create(&pDemux->pidChannel[i].lock, "P_DEMUX_EXT_PID_CHANNEL_SEM", VK_SUSPENDTYPE_FIFO);
		if(vkErr != VK_OK)
		{
			PrintError(" VK_SEM_Create(): Error P_DEMUX_EXT_PID_CHANNEL_SEM, Code(%d)\n", vkErr);
			return DI_ERR_ERROR;
		}
	}

	for(i = 0; i < P_DEMUX_EXT_NUM_PID_CHANNEL; i++)
	{
		vkErr = VK_SEM_Create(&chLock[i], "P_DEMUX_EXT_GPIDCH_SEM", VK_SUSPENDTYPE_FIFO);
		if(vkErr != VK_OK)
		{
			PrintError(" VK_SEM_Create() error, P_DEMUX_EXT_GPIDCH_SEM\n");
			return DI_ERR_ERROR;
		}
	}

	vkErr = VK_SEM_Create(&cbLockSection, "P_DEMUX_EXT_CBSEC_SEM", VK_SUSPENDTYPE_FIFO);
	if(vkErr != VK_OK)
	{
		PrintError(" VK_SEM_Create(): Error P_DEMUX_EXT_CBSEC_SEM, Code(%d)\n", vkErr);
		return DI_ERR_ERROR;
	}

	eDiExtErr = P_DEMUX_EXT_PrepareSectionHandling();
	if(eDiExtErr != DI_EXT_OK)
	{
		PrintError(" P_DEMUX_EXT_PrepareSectionHandling(): Error Code(%d)\n", eDiExtErr);
		return DI_ERR_ERROR;
	}

	eDiExtErr = P_DEMUX_EXT_InitFilter();
	if(eDiExtErr != DI_EXT_OK)
	{
		PrintError(" P_DEMUX_EXT_InitFilter(): Error Code(%d)\n", eDiExtErr);
		return DI_ERR_ERROR;
	}

	for(i = 0; i < P_DEMUX_EXT_MAX_SECTION_FILTER_NUM; i++)
	{
		NEXUS_Message_GetDefaultSettings(&openSettings);
		openSettings.dataReady.callback = P_DEMUX_EXT_NEXUS_NotifyCB;
		openSettings.dataReady.context = (void *)P_DEMUX_EXT_MAKE_FILTERPAIRVALUE(0,0);
		openSettings.dataReady.param = 0;
		openSettings.overflow.callback = P_DEMUX_EXT_NEXUS_OverflowCB;
		openSettings.overflow.context = (void *)P_DEMUX_EXT_MAKE_FILTERPAIRVALUE(0,0);
		openSettings.overflow.param = 0;
		openSettings.bufferSize = 0;

		pDemux->msgFilter[i].msgHandle = NEXUS_Message_Open(&openSettings);
		if(pDemux->msgFilter[i].msgHandle == NULL)
		{
			PrintError(" NEXUS_Message_Open(): Error NULL handle!\n");
			return DI_ERR_ERROR;
		}
		else
		{
			PrintDebug(" NEXUS_Message_Open(%d, 0x%x)\n", i, pDemux->msgFilter[i].msgHandle);
		}
	}

	inter_buffer = VK_MEM_Alloc(P_DEMUX_EXT_INTER_BUFFER_SIZE);
	if(inter_buffer == NULL)
	{
		PrintError(" Failed allocation intermediate buffer for section processing\n");
		return DI_ERR_ERROR;
	}

	return DI_ERR_OK;
}


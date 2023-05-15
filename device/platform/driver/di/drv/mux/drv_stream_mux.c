/**
 * di_mux.c
 */

/**
 * @defgroup		DI_MUX DI_MUX : DI_MUX module
 * @author
 * @note			DI_MUX APIs
 * @brief			Define DI_MUX APIs
 * @file 			di_mux.c
 */

/*******************************************************************/
/**************************** Header Files *************************/
/*******************************************************************/
#if defined(CONFIG_VK_STDLIB)
#else
#include 	<stdio.h>
#include 	<stdlib.h>
#include 	<string.h>
#endif
#include <fcntl.h>
#include <sys/mman.h>
#include <assert.h>
#include "bstd.h"
#include "bkni.h"

#include "nexus_platform_features.h"
#include "nexus_memory.h"
#include "nexus_base_types.h"


#include "nexus_video_encoder.h"
#include "nexus_audio_encoder.h"
#include "nexus_stream_mux.h"
#include "nexus_audio_types.h"
#include "nexus_video_types.h"

#include "linden_trace.h"
#include "drv_stream_mux.h"
#include "drv_demux.h"
#include "drv_pvr.h"
#include "drv_audio_enc.h"
#include "drv_video_enc.h"

#include "di_err.h"
#include "drv_err.h"
#include "vkernel.h"
#include "htype.h"
#include "taskdef.h"

#include "di_demux.h"

#define DEBUG_MODULE	TRACE_MODULE_DI_DEMUX

#define	MAX_MUX_PIDS_NUM				2
#define	TIMESTAMPDELTA					0
#define TRANSCODE_PAT_PID				0x00
#define INVALID_MUX_INDEX				0xff

#define TS_TOTAL_SIZE		188
#define TS_CONTENTS_SIZE	183
#define TS_CONTINUOUS_MAX	0x0f
#define TS_FIRST_PALOADINDEX	5
#define TS_SECOND_PALOADINDEX	4

typedef struct MUX_PSIData_STRUCT
{
	unsigned char 					usedflag;
	unsigned char 					contflag;
	unsigned short					psiPid;
	unsigned int					psiSize;
	unsigned char 					continousvalue;
	void							*tsData;
	unsigned char					*psiData;
	NEXUS_PidChannelHandle	   		hPidChannelTranscodePsi;
	NEXUS_StreamMuxSystemData		systemData;
	unsigned long					streamMuxTimeOut;
	unsigned long					streamMuxTimerId;
	unsigned long 					ulSemId_Psi;
}PSI_DATA;

typedef struct MUX_PESData_STRUCT
{
	unsigned char 					usedflag;
	unsigned short					pesPid;
	HUINT32							InDemux;
	NEXUS_PidChannelHandle			hPidChannelPes;
	NEXUS_PidChannelHandle	   		hPidChannelTranscodePes;
}PES_DATA;

typedef struct _P_MUX_STRUCT
{
	HUINT32							muxIndex;
	HUINT8							useflag;
	NEXUS_StreamMuxHandle			hStreamMux;
	BKNI_EventHandle				hfinishEvent;
	NEXUS_PlaybackHandle			hPbHandle;
	NEXUS_StreamMuxStartSettings	muxConfig;
	NEXUS_StreamMuxOutput			muxOutput;
	NEXUS_PidChannelHandle	   		hPidChannelTranscodePcr;
	PSI_DATA						psiData[MAX_NUM_PSI];
	PES_DATA						pesData[MAX_NUM_PES];
	HBOOL							bIsWatch;
	HBOOL							bIsVideo;
	HBOOL							bIsAudio;
	unsigned long 					ulSemId_DRV;
	HUINT32							ulAudCnt;
}P_MUX_Module;

static const unsigned int _mpegCrc32Tbl[256] =
{
	0x00000000, 0x04c11db7, 0x09823b6e, 0x0d4326d9,
	0x130476dc, 0x17c56b6b, 0x1a864db2, 0x1e475005,
	0x2608edb8, 0x22c9f00f, 0x2f8ad6d6, 0x2b4bcb61,
	0x350c9b64, 0x31cd86d3, 0x3c8ea00a, 0x384fbdbd,
	0x4c11db70, 0x48d0c6c7, 0x4593e01e, 0x4152fda9,
	0x5f15adac, 0x5bd4b01b, 0x569796c2, 0x52568b75,
	0x6a1936c8, 0x6ed82b7f, 0x639b0da6, 0x675a1011,
	0x791d4014, 0x7ddc5da3, 0x709f7b7a, 0x745e66cd,
	0x9823b6e0, 0x9ce2ab57, 0x91a18d8e, 0x95609039,
	0x8b27c03c, 0x8fe6dd8b, 0x82a5fb52, 0x8664e6e5,
	0xbe2b5b58, 0xbaea46ef, 0xb7a96036, 0xb3687d81,
	0xad2f2d84, 0xa9ee3033, 0xa4ad16ea, 0xa06c0b5d,
	0xd4326d90, 0xd0f37027, 0xddb056fe, 0xd9714b49,
	0xc7361b4c, 0xc3f706fb, 0xceb42022, 0xca753d95,
	0xf23a8028, 0xf6fb9d9f, 0xfbb8bb46, 0xff79a6f1,
	0xe13ef6f4, 0xe5ffeb43, 0xe8bccd9a, 0xec7dd02d,
	0x34867077, 0x30476dc0, 0x3d044b19, 0x39c556ae,
	0x278206ab, 0x23431b1c, 0x2e003dc5, 0x2ac12072,
	0x128e9dcf, 0x164f8078, 0x1b0ca6a1, 0x1fcdbb16,
	0x018aeb13, 0x054bf6a4, 0x0808d07d, 0x0cc9cdca,
	0x7897ab07, 0x7c56b6b0, 0x71159069, 0x75d48dde,
	0x6b93dddb, 0x6f52c06c, 0x6211e6b5, 0x66d0fb02,
	0x5e9f46bf, 0x5a5e5b08, 0x571d7dd1, 0x53dc6066,
	0x4d9b3063, 0x495a2dd4, 0x44190b0d, 0x40d816ba,
	0xaca5c697, 0xa864db20, 0xa527fdf9, 0xa1e6e04e,
	0xbfa1b04b, 0xbb60adfc, 0xb6238b25, 0xb2e29692,
	0x8aad2b2f, 0x8e6c3698, 0x832f1041, 0x87ee0df6,
	0x99a95df3, 0x9d684044, 0x902b669d, 0x94ea7b2a,
	0xe0b41de7, 0xe4750050, 0xe9362689, 0xedf73b3e,
	0xf3b06b3b, 0xf771768c, 0xfa325055, 0xfef34de2,
	0xc6bcf05f, 0xc27dede8, 0xcf3ecb31, 0xcbffd686,
	0xd5b88683, 0xd1799b34, 0xdc3abded, 0xd8fba05a,
	0x690ce0ee, 0x6dcdfd59, 0x608edb80, 0x644fc637,
	0x7a089632, 0x7ec98b85, 0x738aad5c, 0x774bb0eb,
	0x4f040d56, 0x4bc510e1, 0x46863638, 0x42472b8f,
	0x5c007b8a, 0x58c1663d, 0x558240e4, 0x51435d53,
	0x251d3b9e, 0x21dc2629, 0x2c9f00f0, 0x285e1d47,
	0x36194d42, 0x32d850f5, 0x3f9b762c, 0x3b5a6b9b,
	0x0315d626, 0x07d4cb91, 0x0a97ed48, 0x0e56f0ff,
	0x1011a0fa, 0x14d0bd4d, 0x19939b94, 0x1d528623,
	0xf12f560e, 0xf5ee4bb9, 0xf8ad6d60, 0xfc6c70d7,
	0xe22b20d2, 0xe6ea3d65, 0xeba91bbc, 0xef68060b,
	0xd727bbb6, 0xd3e6a601, 0xdea580d8, 0xda649d6f,
	0xc423cd6a, 0xc0e2d0dd, 0xcda1f604, 0xc960ebb3,
	0xbd3e8d7e, 0xb9ff90c9, 0xb4bcb610, 0xb07daba7,
	0xae3afba2, 0xaafbe615, 0xa7b8c0cc, 0xa379dd7b,
	0x9b3660c6, 0x9ff77d71, 0x92b45ba8, 0x9675461f,
	0x8832161a, 0x8cf30bad, 0x81b02d74, 0x857130c3,
	0x5d8a9099, 0x594b8d2e, 0x5408abf7, 0x50c9b640,
	0x4e8ee645, 0x4a4ffbf2, 0x470cdd2b, 0x43cdc09c,
	0x7b827d21, 0x7f436096, 0x7200464f, 0x76c15bf8,
	0x68860bfd, 0x6c47164a, 0x61043093, 0x65c52d24,
	0x119b4be9, 0x155a565e, 0x18197087, 0x1cd86d30,
	0x029f3d35, 0x065e2082, 0x0b1d065b, 0x0fdc1bec,
	0x3793a651, 0x3352bbe6, 0x3e119d3f, 0x3ad08088,
	0x2497d08d, 0x2056cd3a, 0x2d15ebe3, 0x29d4f654,
	0xc5a92679, 0xc1683bce, 0xcc2b1d17, 0xc8ea00a0,
	0xd6ad50a5, 0xd26c4d12, 0xdf2f6bcb, 0xdbee767c,
	0xe3a1cbc1, 0xe760d676, 0xea23f0af, 0xeee2ed18,
	0xf0a5bd1d, 0xf464a0aa, 0xf9278673, 0xfde69bc4,
	0x89b8fd09, 0x8d79e0be, 0x803ac667, 0x84fbdbd0,
	0x9abc8bd5, 0x9e7d9662, 0x933eb0bb, 0x97ffad0c,
	0xafb010b1, 0xab710d06, 0xa6322bdf, 0xa2f33668,
	0xbcb4666d, 0xb8757bda, 0xb5365d03, 0xb1f740b4
};

static P_MUX_Module muxInstance[MAX_MUX_NUM];

static P_MUX_Module * P_STREAM_MUX_GetInstance(HUINT32 muxIndex)
{
	return &(muxInstance[muxIndex]);
}

static void	P_STREAM_MUX_FinishCallback(void *context, int param)
{
	BSTD_UNUSED(param);

	BKNI_EventHandle finishEvent = (BKNI_EventHandle)context;
	BKNI_SetEvent(finishEvent);
	return;
}

static HUINT32 P_STREAM_MUX_GetMpegCrc32(const void *data, HUINT32 n)
{
	HUINT32	crc = -1;

	while (n--)
	{
		crc = (crc << 8) ^ _mpegCrc32Tbl[((crc >> 24) ^ *(HUINT8 *)data) & 0xff];
		data = (unsigned char *)data + 1;
	}
	return crc;
}

static DRV_Error P_STREAM_MUX_MakeTsData(HUINT16 psiPid,HUINT32 psiSize,HUINT8 *psiSectionData,HUINT8 psicontinuous,HUINT8 startflag,HUINT32 *numofTS,HUINT8 **psiTSdata,HUINT8 *sPsicontinuous)
{
	HUINT32	index = 0,totalsize = 0, i = 0;
	HUINT8 *ptmpTSData = NULL;
	HUINT8 *ptmpPSIData = NULL;
	HUINT8 upid = 0,dpid = 0;
	HUINT16 tmppid = 0,scontinous = 0;
	HUINT32	tssize = 0, indicator = 0,sindex = 0;

	HUINT32	sectionCRC = 0,crcIndicator = 0;

	totalsize = psiSize;	/* psi section size */

	if(psiSize > TS_CONTENTS_SIZE )
	{
		do
		{
			if(index == 0)
			{
				if(totalsize > TS_CONTENTS_SIZE)
				{
					totalsize -= TS_CONTENTS_SIZE;
				}

			}
			else
			{
				if(totalsize > (TS_CONTENTS_SIZE + 1))
				{
					totalsize -= (TS_CONTENTS_SIZE + 1);
				}
			}

			index += 1;

		}while(totalsize > (TS_CONTENTS_SIZE + 1));

		index += 1;

	}
	else
	{

		index = 1;

	}


	if(index > 0)
	{
		tssize = TS_TOTAL_SIZE*index;
		ptmpTSData = DD_MEM_Alloc(tssize);
		if(ptmpTSData == NULL)
		{
			DD_MEM_Free(ptmpPSIData);
			return DRV_ERR_INVALID_PARAMETER;
		}

		VK_memset(ptmpTSData,0xff,tssize);
	}

	totalsize = psiSize;

	if(	totalsize > 4)
		totalsize -= 4;

	sectionCRC = P_STREAM_MUX_GetMpegCrc32((void *)psiSectionData, totalsize);

	tmppid = psiPid;
	tmppid &= 0x1fff;
	dpid = tmppid;
	tmppid >>=8;
	upid = tmppid;

	totalsize = psiSize;	/* psi section size*/

	for(i = 0; i < index; i++)
	{
		ptmpTSData[i*TS_TOTAL_SIZE] = 0x47;
		if((i == 0)&&(startflag == 1))
			ptmpTSData[i*TS_TOTAL_SIZE + 1] = 0x40;
		else
			ptmpTSData[i*TS_TOTAL_SIZE + 1] = 0x00;

		ptmpTSData[i*TS_TOTAL_SIZE + 1] |= upid;
		ptmpTSData[i*TS_TOTAL_SIZE + 2] = dpid;


		if(i == 0)
		{
			ptmpTSData[i*TS_TOTAL_SIZE + 3] = 0x10;

			ptmpTSData[i*TS_TOTAL_SIZE + 3] |= psicontinuous;

			scontinous = ptmpTSData[i*TS_TOTAL_SIZE + 3];
			scontinous &= 0x0f;

			if(scontinous == 0x0f)
				scontinous = 0;
			else
				scontinous += 1;

			ptmpTSData[i*TS_TOTAL_SIZE + 4] = 0;

			if(totalsize > TS_CONTENTS_SIZE)
			{
				VK_memcpy(&(ptmpTSData[i*TS_TOTAL_SIZE + TS_FIRST_PALOADINDEX]),psiSectionData,TS_CONTENTS_SIZE);
				totalsize -= TS_CONTENTS_SIZE;
			}
			else
			{
				VK_memcpy(&(ptmpTSData[i*TS_TOTAL_SIZE + TS_FIRST_PALOADINDEX]),psiSectionData,totalsize);
				crcIndicator = TS_FIRST_PALOADINDEX + totalsize;
				crcIndicator -= 4;
				ptmpTSData[crcIndicator] = ((sectionCRC>>24)&0x000000ff);
				ptmpTSData[crcIndicator + 1] = ((sectionCRC>>16)&0x000000ff);
				ptmpTSData[crcIndicator + 2] = ((sectionCRC>>8)&0x000000ff);
				ptmpTSData[crcIndicator + 3] = (sectionCRC&0x000000ff);

			}

		}
		else
		{
			ptmpTSData[i*TS_TOTAL_SIZE + 3] = 0x10;

			if(scontinous <= TS_CONTINUOUS_MAX)
			{
				ptmpTSData[i*TS_TOTAL_SIZE + 3] |= scontinous;
			}
			else
			{
				ptmpTSData[i*TS_TOTAL_SIZE + 3] |= 0;
			}

			scontinous = ptmpTSData[i*TS_TOTAL_SIZE + 3];
			scontinous &= 0x0f;

			if(scontinous == 0x0f)
				scontinous = 0;
			else
				scontinous += 1;

			sindex = i;
			sindex -= 1;

			indicator = TS_CONTENTS_SIZE + (sindex*(TS_CONTENTS_SIZE + 1));
			if(totalsize > (TS_CONTENTS_SIZE + 1))
			{
				VK_memcpy(&(ptmpTSData[i*TS_TOTAL_SIZE + TS_SECOND_PALOADINDEX]),&(psiSectionData[indicator]),(TS_CONTENTS_SIZE + 1));
				totalsize -= (TS_CONTENTS_SIZE + 1);
			}
			else
			{
				VK_memcpy(&(ptmpTSData[i*TS_TOTAL_SIZE + TS_SECOND_PALOADINDEX]),&(psiSectionData[indicator]),totalsize);
				crcIndicator = i*TS_TOTAL_SIZE + TS_SECOND_PALOADINDEX + totalsize;
				crcIndicator -= 4;
				ptmpTSData[crcIndicator] = ((sectionCRC>>24)&0x000000ff);
				ptmpTSData[crcIndicator + 1] = ((sectionCRC>>16)&0x000000ff);
				ptmpTSData[crcIndicator + 2] = ((sectionCRC>>8)&0x000000ff);
				ptmpTSData[crcIndicator + 3] = (sectionCRC&0x000000ff);

			}
		}
	}

	*numofTS = index;
	*psiTSdata = ptmpTSData;
	*sPsicontinuous = scontinous;

	return DRV_OK;
}

static void P_STREAM_MUX_AddPSIDataTimer(unsigned long tid,void *arg )
{
	P_MUX_Module *pMux = NULL;
	int				nRet = 0;
	NEXUS_Error		nErr = 0;
	HUINT32	PSIArg[3] = {0,};
	HUINT32 deviceId = 0,index = 0,numofTS = 0;
	DRV_Error		drvErr = 0;
	HUINT8 psicontinuous = 0;
	HUINT8 *psiTSdata = NULL;

	BSTD_UNUSED(tid);

	if(arg == NULL)
		return;

	VK_memcpy(PSIArg,(HUINT32 *)arg,sizeof(PSIArg));

	deviceId = PSIArg[0];
	index = PSIArg[1];
	numofTS = PSIArg[2];

	if((deviceId >= MAX_MUX_NUM) ||(index >= MAX_NUM_PSI)||(numofTS < 1))
		return;

	pMux = P_STREAM_MUX_GetInstance(deviceId);

	if(pMux == NULL)
		return;

	nRet = VK_SEM_Get(pMux->psiData[index].ulSemId_Psi);
	if(nRet != 0)
		return;

	if(pMux->psiData[index].usedflag == 0)
	{
		VK_SEM_Release(pMux->psiData[index].ulSemId_Psi);
		return;
	}

	drvErr = P_STREAM_MUX_MakeTsData(pMux->psiData[index].psiPid,pMux->psiData[index].psiSize,pMux->psiData[index].psiData,pMux->psiData[index].continousvalue,1,&numofTS,&psiTSdata,&psicontinuous);

	pMux->psiData[index].continousvalue = psicontinuous;
	BKNI_Memcpy(pMux->psiData[index].tsData,psiTSdata, TS_TOTAL_SIZE*numofTS);

	pMux->psiData[index].systemData.pData = pMux->psiData[index].tsData;
	pMux->psiData[index].systemData.size = TS_TOTAL_SIZE*numofTS;
	nErr = NEXUS_StreamMux_AddSystemDataBuffer(pMux->hStreamMux, &(pMux->psiData[index].systemData));
	if(nErr != 0)
	{
		DD_MEM_Free(psiTSdata);
		VK_SEM_Release(pMux->psiData[index].ulSemId_Psi);
		return;
	}

	DD_MEM_Free(psiTSdata);
	VK_SEM_Release(pMux->psiData[index].ulSemId_Psi);

	return;

}

static DRV_Error P_STREAM_MUX_GetNumofTS(HUINT32 psiSize,HUINT32 *numofTS)
{
	unsigned int index = 0,totalsize = 0;

	totalsize = psiSize;	/* psi section size */

	if(psiSize > TS_CONTENTS_SIZE )
	{
		do
		{
			if(index == 0)
			{
				if(totalsize > TS_CONTENTS_SIZE)
				{
					totalsize -= TS_CONTENTS_SIZE;
				}

			}
			else
			{
				if(totalsize > (TS_CONTENTS_SIZE + 1))
				{
					totalsize -= (TS_CONTENTS_SIZE + 1);
				}
			}

			index += 1;

		}while(totalsize > (TS_CONTENTS_SIZE + 1));

		index += 1;

	}
	else
	{
		index = 1;
	}

	*numofTS = index;

	return DRV_OK;

}

#if 0 /* org */
static HUINT32 P_STREAM_MUX_CRC32_mpeg(HUINT32 crc, HUINT8 *data, HUINT32 length)
{
	HUINT32 j = 0;

	crc = ~crc;
	while (length--)
	{
		for (j=0; j<8; j++)
			crc = (crc<<1) ^ ((((*data >> (7-j)) ^ (crc >> 31)) & 1) ? 0x04c11db7 : 0);
		data++;
	}
	return crc;
}
#endif

DRV_Error	DRV_STREAM_MUX_GetTranscodeVideo(HUINT32 ulDeviceId,NEXUS_PidChannelHandle *ppPidChannelTranscodeVideo)
{
	P_MUX_Module	*pMux = NULL;
	int nRet = 0;

	if(ulDeviceId >= MAX_MUX_NUM)
		return DRV_ERR_INVALID_PARAMETER;

	pMux = P_STREAM_MUX_GetInstance(ulDeviceId);

	if(pMux == NULL)
		return DRV_ERR_OUTOFMEMORY;

	nRet = VK_SEM_Get(pMux->ulSemId_DRV);
	if(nRet != 0)
		return	DRV_ERR_EVENT_INITIALIZATION;

	if(pMux->muxOutput.video[0] != NULL)
	{
		*ppPidChannelTranscodeVideo = pMux->muxOutput.video[0];
		VK_SEM_Release(pMux->ulSemId_DRV);
		return DRV_OK;
	}
	else
	{
		VK_SEM_Release(pMux->ulSemId_DRV);
		return DRV_ERR;
	}

}

DRV_Error	DRV_STREAM_MUX_GetTranscodeAudio(HUINT32 ulDeviceId,NEXUS_PidChannelHandle *ppPidChannelTranscodeAudio,NEXUS_PidChannelHandle *ppPidChannelTranscodeSubAudio)
{
	P_MUX_Module	*pMux = NULL;
	int nRet = 0;

	if(ulDeviceId >= MAX_MUX_NUM)
		return DRV_ERR_INVALID_PARAMETER;

	pMux = P_STREAM_MUX_GetInstance(ulDeviceId);

	if(pMux == NULL)
		return DRV_ERR_OUTOFMEMORY;

	nRet = VK_SEM_Get(pMux->ulSemId_DRV);
	if(nRet != 0)
		return	DRV_ERR_EVENT_INITIALIZATION;

	*ppPidChannelTranscodeAudio = NULL;
	*ppPidChannelTranscodeSubAudio = NULL;

	if(pMux->muxOutput.audio[0] != NULL)
	{
		*ppPidChannelTranscodeAudio = pMux->muxOutput.audio[0];

		if(pMux->muxOutput.audio[1] != NULL)
		{
			*ppPidChannelTranscodeSubAudio = pMux->muxOutput.audio[1];
		}
		VK_SEM_Release(pMux->ulSemId_DRV);
		return DRV_OK;
	}
	else
	{
		VK_SEM_Release(pMux->ulSemId_DRV);
		return DRV_ERR;
	}

}

DRV_Error	DRV_STREAM_MUX_GetTranscodePCR(HUINT32 ulDeviceId,NEXUS_PidChannelHandle *ppPidChannelTranscodePcr)
{
	P_MUX_Module	*pMux = NULL;
	int nRet = 0;

	if(ulDeviceId >= MAX_MUX_NUM)
		return DRV_ERR_INVALID_PARAMETER;

	pMux = P_STREAM_MUX_GetInstance(ulDeviceId);

	if(pMux == NULL)
		return DRV_ERR_OUTOFMEMORY;

	nRet = VK_SEM_Get(pMux->ulSemId_DRV);
	if(nRet != 0)
		return	DRV_ERR_EVENT_INITIALIZATION;

	if(pMux->hPidChannelTranscodePcr != NULL)
	{
		*ppPidChannelTranscodePcr = pMux->hPidChannelTranscodePcr;
		VK_SEM_Release(pMux->ulSemId_DRV);
		return DRV_OK;
	}
	else
	{
		VK_SEM_Release(pMux->ulSemId_DRV);
		return DRV_ERR;
	}

}

DRV_Error	DRV_STREAM_MUX_GetUsedPSI(HUINT32 ulDeviceId,HUINT32 *pNumber)
{
	P_MUX_Module	*pMux = NULL;
	HINT32 nRet = 0;
	HUINT32 index = 0;
	HUINT32 count = 0;

	if(ulDeviceId >= MAX_MUX_NUM)
		return DRV_ERR_INVALID_PARAMETER;

	pMux = P_STREAM_MUX_GetInstance(ulDeviceId);

	if(pMux == NULL)
		return DRV_ERR_OUTOFMEMORY;

	nRet = VK_SEM_Get(pMux->ulSemId_DRV);
	if(nRet != 0)
		return	DRV_ERR_EVENT_INITIALIZATION;

	for(index = 0; index < MAX_NUM_PSI; index++)
	{
		if(pMux->psiData[index].usedflag == 1)
		{
			count++;
		}
	}

	if(count >= MAX_NUM_PSI)
	{
		VK_SEM_Release(pMux->ulSemId_DRV);
		return DRV_ERR_INVALID_PARAMETER;
	}
	else
	{
		*pNumber = count;
	}
	VK_SEM_Release(pMux->ulSemId_DRV);

	return DRV_OK;
}


DRV_Error	DRV_STREAM_MUX_GetTranscodePSI(HUINT32 ulDeviceId,HUINT32 ulIndex,NEXUS_PidChannelHandle *ppPidChannelTranscodePsi)
{
	P_MUX_Module	*pMux = NULL;
	int nRet = 0;

	if((ulDeviceId >= MAX_MUX_NUM)||(ulIndex >= MAX_NUM_PSI))
		return DRV_ERR_INVALID_PARAMETER;

	pMux = P_STREAM_MUX_GetInstance(ulDeviceId);

	if(pMux == NULL)
		return DRV_ERR_OUTOFMEMORY;

	nRet = VK_SEM_Get(pMux->ulSemId_DRV);
	if(nRet != 0)
		return	DRV_ERR_EVENT_INITIALIZATION;


	if((pMux->psiData[ulIndex].usedflag == 1)&&(pMux->psiData[ulIndex].hPidChannelTranscodePsi != NULL))
	{
		*ppPidChannelTranscodePsi = pMux->psiData[ulIndex].hPidChannelTranscodePsi;
		VK_SEM_Release(pMux->ulSemId_DRV);
		return DRV_OK;
	}
	else
	{
		VK_SEM_Release(pMux->ulSemId_DRV);
		return DRV_ERR;
	}

}

DRV_Error	DRV_STREAM_MUX_GetUsedPES(HUINT32 ulDeviceId,HUINT32 *pNumber)
{
	P_MUX_Module	*pMux = NULL;
	HINT32 nRet = 0;
	HUINT32 index = 0;
	HUINT32 count = 0;

	if(ulDeviceId >= MAX_MUX_NUM)
	{
		return DRV_ERR_INVALID_PARAMETER;
	}
	pMux = P_STREAM_MUX_GetInstance(ulDeviceId);

	if(pMux == NULL)
	{
		return DRV_ERR_OUTOFMEMORY;
	}

	nRet = VK_SEM_Get(pMux->ulSemId_DRV);
	if(nRet != 0)
	{
		return	DRV_ERR_EVENT_INITIALIZATION;
	}

	for(index = 0; index < MAX_NUM_PES; index++)
	{
		if(pMux->pesData[index].usedflag == 1)
		{
			count++;
		}
	}

	if(count >= MAX_NUM_PES)
	{
		VK_SEM_Release(pMux->ulSemId_DRV);
		return DRV_ERR_INVALID_PARAMETER;
	}
	else
	{
		*pNumber = count;
	}
	VK_SEM_Release(pMux->ulSemId_DRV);

	return DRV_OK;
}


DRV_Error	DRV_STREAM_MUX_GetTranscodePES(HUINT32 ulDeviceId,HUINT32 ulIndex,NEXUS_PidChannelHandle *ppPidChannelTranscodePes)
{
	P_MUX_Module	*pMux = NULL;
	int nRet = 0;

	if((ulDeviceId >= MAX_MUX_NUM)||(ulIndex >= MAX_NUM_PES))
	{
		return DRV_ERR_INVALID_PARAMETER;
	}

	pMux = P_STREAM_MUX_GetInstance(ulDeviceId);

	if(pMux == NULL)
	{
		return DRV_ERR_OUTOFMEMORY;
	}

	nRet = VK_SEM_Get(pMux->ulSemId_DRV);
	if(nRet != 0)
	{
		return	DRV_ERR_EVENT_INITIALIZATION;
	}

	if((pMux->pesData[ulIndex].usedflag == 1)&&(pMux->pesData[ulIndex].hPidChannelTranscodePes != NULL))
	{
		*ppPidChannelTranscodePes = pMux->pesData[ulIndex].hPidChannelTranscodePes;
		VK_SEM_Release(pMux->ulSemId_DRV);
		return DRV_OK;
	}
	else
	{
		VK_SEM_Release(pMux->ulSemId_DRV);
		return DRV_ERR;
	}

}

DRV_Error	DRV_STREAM_MUX_CloseTotalTranscodePSI(HUINT32 ulDeviceId)
{

	P_MUX_Module	*pMux = NULL;
	int nRet = 0;
	HUINT32 index = 0;
	NEXUS_Error		err = 0;

	if(ulDeviceId >= MAX_MUX_NUM)
		return DRV_ERR_INVALID_PARAMETER;

	pMux = P_STREAM_MUX_GetInstance(ulDeviceId);

	if(pMux == NULL)
		return DRV_ERR_OUTOFMEMORY;

	nRet = VK_SEM_Get(pMux->ulSemId_DRV);
	if(nRet != 0)
		return	DRV_ERR_EVENT_INITIALIZATION;

	for(index = 0; index < MAX_NUM_PSI; index++)
	{
		if(pMux->psiData[index].usedflag == 1)
		{
			if(pMux->psiData[index].hPidChannelTranscodePsi != NULL)
			{
				err = NEXUS_Playpump_ClosePidChannel(pMux->muxConfig.pcr.playpump, pMux->psiData[index].hPidChannelTranscodePsi);
				pMux->psiData[index].hPidChannelTranscodePsi = NULL;
			}
		}
	}

	VK_SEM_Release(pMux->ulSemId_DRV);

	return DRV_OK;

}


DRV_Error DRV_STREAM_MUX_Init(HUINT32 ulDeviceId)
{
	P_MUX_Module *pMux = NULL;
	NEXUS_StreamMuxCreateSettings muxCreateSettings;
	char ucSemName[20];
	int nRet = 0;

	pMux = P_STREAM_MUX_GetInstance(ulDeviceId);

	VK_MEM_Memset(pMux, 0x00, sizeof(P_MUX_Module));
	VK_MEM_Memset(pMux->psiData, 0x00, sizeof(pMux->psiData));
	VK_MEM_Memset(pMux->pesData, 0x00, sizeof(pMux->pesData));
	pMux->muxIndex = INVALID_MUX_INDEX;

	BKNI_CreateEvent(&pMux->hfinishEvent);

	VK_MEM_Memset(ucSemName, 0x00, 20);
	VK_snprintf(ucSemName, 18, "drv_mux%d_drv", ulDeviceId);
	nRet = VK_SEM_Create(&(pMux->ulSemId_DRV), ucSemName, DEFAULT_SUSPEND_TYPE);
	if( nRet != VK_OK )
	{
		BKNI_DestroyEvent(pMux->hfinishEvent);
		return DRV_ERR_EVENT_INITIALIZATION;
	}

	NEXUS_StreamMux_GetDefaultCreateSettings(&muxCreateSettings);
	muxCreateSettings.finished.callback = P_STREAM_MUX_FinishCallback;
	muxCreateSettings.finished.context = pMux->hfinishEvent;
	pMux->hStreamMux = NEXUS_StreamMux_Create(&muxCreateSettings);
	if(pMux->hStreamMux == NULL)
	{
		BKNI_DestroyEvent(pMux->hfinishEvent);
		VK_SEM_Destroy(pMux->ulSemId_DRV);
		return DRV_ERR;
	}

	pMux->muxIndex = ulDeviceId;
	pMux->bIsVideo = FALSE;
	pMux->bIsAudio = FALSE;

	return DRV_OK;
}


DRV_Error DRV_STREAM_MUX_UnInit(HUINT32 ulDeviceId)
{

	P_MUX_Module *pMux = NULL;
	NEXUS_Error		err = 0;
	HUINT32 index = 0;

	pMux = P_STREAM_MUX_GetInstance(ulDeviceId);

	/* finish stream mux to clean shutdown */
	NEXUS_StreamMux_Finish(pMux->hStreamMux);

	if(BKNI_WaitForEvent(pMux->hfinishEvent, 2000)!=BERR_SUCCESS)
	{
		return DRV_ERR;
	}

	NEXUS_StreamMux_Stop(pMux->hStreamMux);

	for(index = 0; index < MAX_NUM_PSI; index++)
	{
		if(pMux->psiData[index].usedflag)
		{
			if(pMux->psiData[index].contflag)
			{
				VK_TIMER_Cancel(pMux->psiData[index].streamMuxTimerId);
				pMux->psiData[index].contflag = 0;
				pMux->psiData[index].streamMuxTimerId = 0;
				pMux->psiData[index].streamMuxTimeOut = 0;
			}
			else
			{
				pMux->psiData[index].streamMuxTimerId = 0;
			}
		}
	}

	BKNI_DestroyEvent(pMux->hfinishEvent);
	NEXUS_StreamMux_Destroy(pMux->hStreamMux);

	for(index = 0; index < MAX_NUM_PSI; index++)
	{
		if(pMux->psiData[index].usedflag == 1)
		{
			if(pMux->psiData[index].hPidChannelTranscodePsi != NULL)
			{
				err = NEXUS_Playpump_ClosePidChannel(pMux->muxConfig.pcr.playpump, pMux->psiData[index].hPidChannelTranscodePsi);
				pMux->psiData[index].hPidChannelTranscodePsi = NULL;
				NEXUS_Memory_Free(pMux->psiData[index].tsData);
				pMux->psiData[index].tsData = NULL;
				pMux->psiData[index].psiPid = 0;
				pMux->psiData[index].psiSize = 0;
				BKNI_Memset(&(pMux->psiData[index].systemData), 0, sizeof(NEXUS_StreamMuxSystemData));
				pMux->psiData[index].usedflag = 0;
				VK_SEM_Destroy(pMux->psiData[index].ulSemId_Psi);
			}
		}
	}

	err = NEXUS_Playpump_ClosePidChannel(pMux->muxConfig.pcr.playpump, pMux->hPidChannelTranscodePcr);
	pMux->hPidChannelTranscodePcr = NULL;


	if(pMux->muxConfig.pcr.playpump != NULL)
	{
		DRV_PVR_ReleasePlaypumpHandle(pMux->muxConfig.pcr.playpump);
		pMux->muxConfig.pcr.playpump = NULL;
	}

	if(pMux->muxConfig.video[0].playpump != NULL)
	{
		DRV_PVR_ReleasePlaypumpHandle(pMux->muxConfig.video[0].playpump);
		pMux->muxConfig.video[0].playpump = NULL;
	}


	if(pMux->muxConfig.audio[0].playpump != NULL)
	{
		DRV_PVR_ReleasePlaypumpHandle(pMux->muxConfig.audio[0].playpump);
		pMux->muxConfig.audio[0].playpump = NULL;
	}

	VK_SEM_Destroy(pMux->ulSemId_DRV);


	return DRV_OK;

}

DRV_Error	DRV_STREAM_MUX_Start(HUINT32 ulDeviceId,DRV_STREAM_MUX_INFORMATION	*pMuxInfo)
{

	NEXUS_VideoEncoderHandle	videoEncoder = NULL;
	NEXUS_PlaypumpHandle		playpumpTranscodeVideo = NULL;
	NEXUS_PlaypumpHandle		playpumpTranscodeAudio = NULL;
	NEXUS_PlaypumpHandle		playpumpTranscodePcr = NULL;
	NEXUS_StcChannelHandle		stcChannelTranscode = NULL;
	NEXUS_AudioMuxOutputHandle	audioMuxOutput = NULL;
	NEXUS_Error		result = 0;
	int				nRet = 0;

	P_MUX_Module *pMux = NULL;
	HUINT32			ulIndex = 0;
	DRV_Error drvErr = DRV_OK;
#if defined(CONFIG_LOCAL_TIMESTAMP_TYPE_32BIT_BINARY)
	NEXUS_PlaypumpSettings playpumpSettings;
#endif
	HUINT32 ulMulAudCnt = 0;

	if(ulDeviceId >= MAX_MUX_NUM)
		return DRV_ERR_INVALID_PARAMETER;

	pMux = P_STREAM_MUX_GetInstance(ulDeviceId);

	if((pMuxInfo == NULL)||(pMux == NULL))
		return	DRV_ERR_INVALID_PARAMETER;

	nRet = VK_SEM_Get(pMux->ulSemId_DRV);
	if(nRet != 0)
		return	DRV_ERR_EVENT_INITIALIZATION;

	if((pMux->muxIndex == INVALID_MUX_INDEX)||(pMux->useflag == 1))
	{
		VK_SEM_Release(pMux->ulSemId_DRV);
		return	DRV_ERR_INVALID_PARAMETER;
	}

	ulMulAudCnt = pMuxInfo->ulAudCnt;

	pMux->hPbHandle = NULL;

	NEXUS_StreamMux_GetDefaultStartSettings(&(pMux->muxConfig));
	pMux->muxConfig.transportType = NEXUS_TransportType_eTs;
	if(pMuxInfo->bIsWatch == FALSE)
	{
		pMux->bIsWatch = FALSE;
	}
	else
	{
		pMux->bIsWatch = TRUE;
	}

	stcChannelTranscode = DRV_DEMUX_GetSTCChannelTranscode(ulDeviceId);

	if(stcChannelTranscode != NULL)
		pMux->muxConfig.stcChannel = stcChannelTranscode;

	DRV_VIDEO_GetEncHandle(ulDeviceId,&videoEncoder);

	if(videoEncoder != NULL)
	{
		DRV_PVR_GetPlaypumpHandleByClass(&playpumpTranscodeVideo,BUFF_GROUP_3);
		if(playpumpTranscodeVideo != NULL)
		{
			pMux->muxConfig.video[0].pid = pMuxInfo->videoPid;
			pMux->muxConfig.video[0].encoder = videoEncoder;
			pMux->muxConfig.video[0].playpump = playpumpTranscodeVideo;
			pMux->bIsVideo = TRUE;
#if defined(CONFIG_LOCAL_TIMESTAMP_TYPE_32BIT_BINARY)
			NEXUS_Playpump_GetSettings(playpumpTranscodeVideo, &playpumpSettings);
			playpumpSettings.timestamp.forceRestamping = true;
			playpumpSettings.timestamp.parityCheckDisable = true;
			result = NEXUS_Playpump_SetSettings(playpumpTranscodeVideo,&playpumpSettings);
			if(result != NEXUS_SUCCESS)
			{
				PrintError("NEXUS_Playpump_SetSettings start fails.\n");
				goto mux_start_error;
			}
#endif
		}
		else
		{
			PrintError("Unable to get playpump for video channel.\n");
			goto 	video_pump_error;
		}
	}
	else
	{
		pMux->muxConfig.video[0].encoder = NULL;
		pMux->bIsVideo = FALSE;
	}

	for(ulIndex = 0; ulIndex < ulMulAudCnt ; ulIndex++)
	{
		DRV_AUDIO_GetAudioMuxOutput(ulDeviceId, ulIndex, &audioMuxOutput);
		if(audioMuxOutput != NULL)
		{
			DRV_PVR_GetPlaypumpHandleByClass(&playpumpTranscodeAudio,BUFF_GROUP_3);
			if(playpumpTranscodeAudio != NULL)
			{
				pMux->muxConfig.audio[ulIndex].pid = (ulIndex == 0) ? pMuxInfo->audioPid:pMuxInfo->audioSubPid;
				pMux->muxConfig.audio[ulIndex].pesId += ulIndex;
				pMux->muxConfig.audio[ulIndex].muxOutput = audioMuxOutput;
				pMux->muxConfig.audio[ulIndex].playpump = playpumpTranscodeAudio;
				pMux->bIsAudio = TRUE;
#if defined(CONFIG_LOCAL_TIMESTAMP_TYPE_32BIT_BINARY)
				NEXUS_Playpump_GetSettings(playpumpTranscodeAudio, &playpumpSettings);
				playpumpSettings.timestamp.forceRestamping = true;
				playpumpSettings.timestamp.parityCheckDisable = true;
				result = NEXUS_Playpump_SetSettings(playpumpTranscodeAudio,&playpumpSettings);
				if(result != NEXUS_SUCCESS)
				{
					PrintError("NEXUS_Playpump_SetSettings start fails.\n");
					goto mux_start_error;
				}
#endif
			}
			else
			{
				PrintError("Unable to get playpump for video channel.\n");
				goto 	audio_pump_error;
			}
		}
		else
		{
			pMux->muxConfig.audio[ulIndex].muxOutput = NULL;
			pMux->bIsAudio = FALSE;
		}
	}
	pMux->ulAudCnt = ulMulAudCnt;

	DRV_PVR_GetPlaypumpHandleByClass(&playpumpTranscodePcr,BUFF_GROUP_3);
	if(playpumpTranscodePcr != NULL)
	{
		pMux->muxConfig.pcr.pid = pMuxInfo->pcrPid;
		pMux->muxConfig.pcr.playpump = playpumpTranscodePcr;
		pMux->muxConfig.pcr.interval = 50;
		pMux->muxConfig.pcr.nonPcrInsert = pMuxInfo->NonPcrInsert;

		/* open PidChannels */
		pMux->hPidChannelTranscodePcr = NEXUS_Playpump_OpenPidChannel(playpumpTranscodePcr, pMuxInfo->pcrPid, NULL);
		if(pMux->hPidChannelTranscodePcr == NULL)
		{
			PrintError("[%s]_%d NEXUS_Playpump_OpenPidChannel PCR PID 0x%x \n",__func__,__LINE__,pMuxInfo->pcrPid);
			goto pcr_pump_channel_error;
		}
#if defined(CONFIG_LOCAL_TIMESTAMP_TYPE_32BIT_BINARY)
		NEXUS_Playpump_GetSettings(playpumpTranscodePcr, &playpumpSettings);
		playpumpSettings.timestamp.forceRestamping = true;
		playpumpSettings.timestamp.parityCheckDisable = true;
		playpumpSettings.blindSync = true; /* PCR channel has full transport packets, so blind sync mode */
		result = NEXUS_Playpump_SetSettings(playpumpTranscodePcr,&playpumpSettings);
		if(result != NEXUS_SUCCESS)
		{
			PrintError("NEXUS_Playpump_SetSettings start fails.\n");
			goto mux_start_error;
		}
#endif
	}

	for(ulIndex = 0; ulIndex < MAX_NUM_PES; ulIndex++)
	{
		if(pMux->pesData[ulIndex].usedflag == 1)
		{
			NEXUS_PlaybackPidChannelSettings playbackPidSettings;
			NEXUS_PlaybackHandle	pbHandle;
			NEXUS_PidChannelHandle pidChannel;
			NEXUS_MessageSettings messageSettings;
			NEXUS_MessageStartSettings messageStartSettings;
			HUINT32 unPbDevId = ulDeviceId;
			DI_DEMUX_OUTPUTDEVICETYPE eOutputType;

			if(pMux->bIsWatch == FALSE)
			{
				extern DRV_Error DRV_PVR_PB_GetPBHandle(HUINT32 unDeviceId, NEXUS_PlaybackHandle *ppstPlaybackHandle);
				drvErr = DRV_DEMUX_GetOutPutDeviceType(pMuxInfo->ulDemuxId,&eOutputType);
				if((drvErr == DRV_OK)&&(eOutputType == DI_DEMUX_OUTPUT_TRANSCODER))
				{
					DI_DEMUX_INPUTDEVICETYPE etProducer;
					HUINT32 ulInputDeviceId;
					drvErr = DRV_DEMUX_GetInputDevice(pMuxInfo->ulDemuxId , &etProducer, &ulInputDeviceId);
					if((drvErr == DRV_OK)&&(etProducer == DI_DEMUX_INPUT_PLAYER))
					{
						unPbDevId = ulInputDeviceId;
					}
				}
				/* open source user data PID channel */
				NEXUS_Playback_GetDefaultPidChannelSettings(&playbackPidSettings);
				playbackPidSettings.pidSettings.pidType = NEXUS_PidType_eOther; /* capture the TS packets with the user data PES */
				playbackPidSettings.pidSettings.pidSettings.pidChannelIndex = NEXUS_PID_CHANNEL_OPEN_MESSAGE_CAPABLE;
				drvErr = DRV_PVR_PB_GetPBHandle(unPbDevId, &pbHandle);
				if(drvErr != DRV_OK)
				{
					PrintError("DRV_PVR_PB_GetPBHandle fails.\n");
					goto mux_start_error;
				}

				pidChannel = NEXUS_Playback_OpenPidChannel(pbHandle, pMux->pesData[ulIndex].pesPid, &playbackPidSettings);
				if(pidChannel == NULL)
				{
					PrintError("[%s] Error in NEXUS_Playback_OpenPidChannel\n\r",__FUNCTION__);
					goto mux_start_error;
				}
				pMux->hPbHandle = pbHandle;

			}
			else
			{
				NEXUS_PidChannelSettings pidChSettings;
				NEXUS_PidChannel_GetDefaultSettings(&pidChSettings);
				pidChSettings.pidChannelIndex = NEXUS_PID_CHANNEL_OPEN_MESSAGE_CAPABLE;
				pidChannel = NEXUS_PidChannel_Open(DRV_DEMUX_GetParserBandByDemuxId(pMux->pesData[ulIndex].InDemux),pMux->pesData[ulIndex].pesPid, &pidChSettings);
			}

		    NEXUS_Message_GetDefaultSettings(&messageSettings);
			/* SCTE 270 spec max TS VBI user data bitrate=270Kbps, 256KB buffer can hold 7.5 seconds;
			worthy user data for video synchronization; TODO: may be reduced if unnecessary */
			messageSettings.bufferSize = 256*1024;
			messageSettings.maxContiguousMessageSize = 0;
			messageSettings.overflow.callback = NULL; /* report overflow error */
			messageSettings.overflow.param = 0;
			pMux->muxConfig.userdata[ulIndex].message = NEXUS_Message_Open(&messageSettings);
			if(pMux->muxConfig.userdata[ulIndex].message == NULL)
			{
				if(pMux->bIsWatch == FALSE)
				{
					NEXUS_Playback_ClosePidChannel(pbHandle,pidChannel);
				}
				else
				{
					NEXUS_PidChannel_Close(pidChannel);
				}
				PrintError("[%s] Error in NEXUS_Message_Open\n\r",__FUNCTION__);
				goto mux_start_error;
			}

			NEXUS_Message_GetDefaultStartSettings(pMux->muxConfig.userdata[ulIndex].message, &messageStartSettings);
			messageStartSettings.format = NEXUS_MessageFormat_eTs;
			messageStartSettings.pidChannel = pidChannel;
			/* must start message before stream mux starts */
			result = NEXUS_Message_Start(pMux->muxConfig.userdata[ulIndex].message, &messageStartSettings);
			if(result != NEXUS_SUCCESS)
			{
				if(pMux->bIsWatch == FALSE)
				{
					NEXUS_Playback_ClosePidChannel(pbHandle,pidChannel);
				}
				else
				{
					NEXUS_PidChannel_Close(pidChannel);
				}
				NEXUS_Message_Close(pMux->muxConfig.userdata[ulIndex].message);
				PrintError("NEXUS_Message_Start Error\n");
				goto mux_start_error;
			}
			pMux->pesData[ulIndex].hPidChannelTranscodePes = NEXUS_Playpump_OpenPidChannel(pMux->muxConfig.pcr.playpump, pMux->pesData[ulIndex].pesPid, NULL);
			if(pMux->pesData[ulIndex].hPidChannelTranscodePes == NULL)
			{
				if(pMux->bIsWatch == FALSE)
				{
					NEXUS_Playback_ClosePidChannel(pbHandle,pidChannel);
				}
				else
				{
					NEXUS_PidChannel_Close(pidChannel);
				}
				NEXUS_Message_Stop(pMux->muxConfig.userdata[ulIndex].message);
				NEXUS_Message_Close(pMux->muxConfig.userdata[ulIndex].message);
				PrintError("[%s]_%d NEXUS_Playpump_OpenPidChannel \n",__func__,__LINE__);
				goto mux_start_error;
			}
			pMux->pesData[ulIndex].hPidChannelPes = pidChannel;
		}
	}

#if defined(CONFIG_LOCAL_TIMESTAMP_TYPE_32BIT_BINARY)
	pMux->muxConfig.servicePeriod = 50;
	pMux->muxConfig.latencyTolerance = 50;
#if (NEXUS_VERSION >= 1404)
	pMux->muxConfig.supportTts = true;
#endif
#endif
	/* start mux */
	result = NEXUS_StreamMux_Start(pMux->hStreamMux,&(pMux->muxConfig), &(pMux->muxOutput));
	if(result != 0)
	{
		PrintError("StreamMux start fails.\n");
		goto mux_start_error;
	}

	pMux->useflag = 1;
	VK_SEM_Release(pMux->ulSemId_DRV);

	return DRV_OK;

mux_start_error:
	if(pMux->muxConfig.pcr.playpump != NULL)
	{
		NEXUS_Playpump_ClosePidChannel(pMux->muxConfig.pcr.playpump, pMux->hPidChannelTranscodePcr);
	}
pcr_pump_channel_error:
	if(pMux->muxConfig.pcr.playpump != NULL)
	{
		DRV_PVR_ReleasePlaypumpHandle(pMux->muxConfig.pcr.playpump);
		pMux->muxConfig.pcr.playpump = NULL;
	}
	if(pMux->bIsAudio == TRUE)
	{
		DRV_PVR_ReleasePlaypumpHandle(pMux->muxConfig.audio[0].playpump);
		pMux->muxConfig.audio[0].muxOutput = NULL;
		pMux->muxConfig.audio[0].playpump = NULL;
	}
audio_pump_error:
	if(pMux->bIsVideo == TRUE)
	{
		DRV_PVR_ReleasePlaypumpHandle(pMux->muxConfig.video[0].playpump);
		pMux->muxConfig.video[0].encoder = NULL;
		pMux->muxConfig.video[0].playpump = NULL;
	}
video_pump_error:
	VK_SEM_Release(pMux->ulSemId_DRV);
	return DRV_ERR;
}

DRV_Error	DRV_STREAM_MUX_SetPSIData(HUINT32 ulDeviceId,HUINT16 psiPid,HUINT32 psiSize,HUINT8 *psiData,HUINT8 contflag,unsigned long timeOut,HUINT32 *pIndexId )
{
	P_MUX_Module *pMux = NULL;
	int				nRet = 0;
	HUINT32	index = 0;
	NEXUS_Error		nErr = 0;
	DRV_Error		drvErr = 0;

	HUINT32	PSIArg[3] = {0,};
	char ucSemName[20];

	HUINT32 numofTS = 0;
	HUINT8 *psiTSdata = NULL;
	HUINT8 psicontinuous = 0;

	if((ulDeviceId >= MAX_MUX_NUM) ||(psiSize == 0)||(psiData == NULL))
		return DRV_ERR_INVALID_PARAMETER;

	pMux = P_STREAM_MUX_GetInstance(ulDeviceId);

	if(pMux == NULL)
		return	DRV_ERR_INVALID_PARAMETER;

	nRet = VK_SEM_Get(pMux->ulSemId_DRV);
	if(nRet != 0)
		return	DRV_ERR_EVENT_INITIALIZATION;

	for(index = 0; index < MAX_NUM_PSI; index++)
	{
		if(pMux->psiData[index].usedflag == 0)
			break;
	}

	if(index >= MAX_NUM_PSI)
	{
		VK_SEM_Release(pMux->ulSemId_DRV);
		return DRV_ERR;
	}

	VK_MEM_Memset(ucSemName, 0x00, 20);
	VK_snprintf(ucSemName, 18, "drv_mux%d_psi", index);

	nRet = VK_SEM_Create(&(pMux->psiData[index].ulSemId_Psi),ucSemName, DEFAULT_SUSPEND_TYPE);
	if( nRet != VK_OK )
	{
		VK_SEM_Release(pMux->ulSemId_DRV);
		return DRV_ERR;
	}
	pMux->psiData[index].psiData = DD_MEM_Alloc(psiSize);
	VK_memcpy(pMux->psiData[index].psiData,psiData,psiSize);

	pMux->psiData[index].hPidChannelTranscodePsi = NEXUS_Playpump_OpenPidChannel(pMux->muxConfig.pcr.playpump, psiPid, NULL);
	if(pMux->psiData[index].hPidChannelTranscodePsi == NULL)
	{
		VK_SEM_Destroy(pMux->psiData[index].ulSemId_Psi);
		VK_SEM_Release(pMux->ulSemId_DRV);
		return DRV_ERR;
	}


	/* add sectiondata to tsdata */
	drvErr = P_STREAM_MUX_MakeTsData(psiPid,psiSize,psiData,pMux->psiData[index].continousvalue,1,&numofTS,&psiTSdata,&psicontinuous);
	if((drvErr != 0)||(psiTSdata == NULL))
	{
		NEXUS_Playpump_ClosePidChannel(pMux->muxConfig.pcr.playpump,pMux->psiData[index].hPidChannelTranscodePsi);
		VK_SEM_Destroy(pMux->psiData[index].ulSemId_Psi);
		VK_SEM_Release(pMux->ulSemId_DRV);
		return DRV_ERR;
	}

	pMux->psiData[index].psiPid = psiPid;
	pMux->psiData[index].continousvalue = psicontinuous;

	nErr = NEXUS_Memory_Allocate(TS_TOTAL_SIZE*numofTS, NULL, &(pMux->psiData[index].tsData));
	if(nErr != 0)
	{
		if(psiTSdata != NULL)
		{
			DD_MEM_Free(psiTSdata);
		}
		NEXUS_Playpump_ClosePidChannel(pMux->muxConfig.pcr.playpump,pMux->psiData[index].hPidChannelTranscodePsi);
		VK_SEM_Destroy(pMux->psiData[index].ulSemId_Psi);
		VK_SEM_Release(pMux->ulSemId_DRV);
		return DRV_ERR;
	}

	BKNI_Memcpy(pMux->psiData[index].tsData,psiTSdata, TS_TOTAL_SIZE*numofTS);
	NEXUS_Memory_FlushCache(pMux->psiData[index].tsData,TS_TOTAL_SIZE*numofTS);

	BKNI_Memset(&(pMux->psiData[index].systemData), 0, sizeof(NEXUS_StreamMuxSystemData));

	pMux->psiData[index].systemData.size = TS_TOTAL_SIZE*numofTS;
	pMux->psiData[index].systemData.pData = pMux->psiData[index].tsData;
	pMux->psiData[index].systemData.timestampDelta = TIMESTAMPDELTA;

	nErr = NEXUS_StreamMux_AddSystemDataBuffer(pMux->hStreamMux, &(pMux->psiData[index].systemData));
	if(nErr != 0)
	{
		if(psiTSdata != NULL)
		{
			DD_MEM_Free(psiTSdata);
		}

		NEXUS_Playpump_ClosePidChannel(pMux->muxConfig.pcr.playpump,pMux->psiData[index].hPidChannelTranscodePsi);
		NEXUS_Memory_Free(pMux->psiData[index].tsData);
		VK_SEM_Destroy(pMux->psiData[index].ulSemId_Psi);
		VK_SEM_Release(pMux->ulSemId_DRV);
		return DRV_ERR;
	}

	pMux->psiData[index].psiSize = psiSize;
	pMux->psiData[index].usedflag = 1;
	*pIndexId = index;

	if(contflag == 1)
	{
		PSIArg[0] = ulDeviceId;
		PSIArg[1] = index;
		PSIArg[2] = numofTS;

		nRet = VK_TIMER_EventEvery(timeOut,
								P_STREAM_MUX_AddPSIDataTimer,
								PSIArg,
								sizeof(PSIArg),
								&pMux->psiData[index].streamMuxTimerId);
		if(nRet != VK_OK)
		{
			if(psiTSdata != NULL)
			{
				DD_MEM_Free(psiTSdata);
			}

			NEXUS_Playpump_ClosePidChannel(pMux->muxConfig.pcr.playpump,pMux->psiData[index].hPidChannelTranscodePsi);
			NEXUS_Memory_Free(pMux->psiData[index].tsData);
			VK_SEM_Destroy(pMux->psiData[index].ulSemId_Psi);
			VK_SEM_Release(pMux->ulSemId_DRV);
			return DRV_ERR;
		}

		pMux->psiData[index].streamMuxTimeOut = timeOut;
		pMux->psiData[index].contflag = contflag;

	}
	DD_MEM_Free(psiTSdata);
	VK_SEM_Release(pMux->ulSemId_DRV);

	return DRV_OK;
}

DRV_Error	DRV_STREAM_MUX_SetPesData(HUINT32 ulDeviceId,HUINT32 ulInputDemuxId,HUINT16 pesPid,HUINT32 *pIndexId )
{
	P_MUX_Module *pMux = NULL;
	int				nRet = 0;
	HUINT32	index = 0;

	if(ulDeviceId >= MAX_MUX_NUM)
	{
		return DRV_ERR_INVALID_PARAMETER;
	}

	pMux = P_STREAM_MUX_GetInstance(ulDeviceId);

	if(pMux == NULL)
	{
		return	DRV_ERR_INVALID_PARAMETER;
	}

	nRet = VK_SEM_Get(pMux->ulSemId_DRV);
	if(nRet != 0)
	{
		return	DRV_ERR_EVENT_INITIALIZATION;
	}

	for(index = 0; index < MAX_NUM_PES; index++)
	{
		if(pMux->pesData[index].usedflag == 0)
		{
			break;
		}
	}

	if(index >= MAX_NUM_PES)
	{
		VK_SEM_Release(pMux->ulSemId_DRV);
		return DRV_ERR;
	}

	pMux->pesData[index].usedflag = 1;
	pMux->pesData[index].pesPid = pesPid;
	pMux->pesData[index].InDemux = ulInputDemuxId;
	*pIndexId = index;
	VK_SEM_Release(pMux->ulSemId_DRV);

	return DRV_OK;
}

DRV_Error	DRV_STREAM_MUX_UpdatePSIData(HUINT32 ulDeviceId,HUINT32 ulIndex,HUINT32 psiSize,HUINT8 *pPSIData,HUINT8 startflag)
{
	P_MUX_Module *pMux = NULL;
	int				nRet = 0;
	DRV_Error		drvErr = 0;
	HUINT32 numofTS = 0,onumofTS = 0;
	HUINT8 *psiTSdata = NULL;
	HUINT8 psicontinuous = 0;
	NEXUS_Error		nErr = 0;
	HUINT32	PSIArg[3] = {0,};


	if((ulDeviceId >= MAX_MUX_NUM) ||(ulIndex >= MAX_NUM_PSI)||(pPSIData == NULL))
		return DRV_ERR_INVALID_PARAMETER;

	pMux = P_STREAM_MUX_GetInstance(ulDeviceId);

	if(pMux == NULL)
		return	DRV_ERR_INVALID_PARAMETER;

	nRet = VK_SEM_Get(pMux->psiData[ulIndex].ulSemId_Psi);
	if(nRet != 0)
		return	DRV_ERR_EVENT_INITIALIZATION;

	if(pMux->psiData[ulIndex].usedflag == 0)
	{
		VK_SEM_Release(pMux->psiData[ulIndex].ulSemId_Psi);
		return	DRV_ERR_EVENT_INITIALIZATION;
	}

	if(pMux->psiData[ulIndex].contflag )
	{
		nRet = VK_TIMER_Cancel(pMux->psiData[ulIndex].streamMuxTimerId);
		if(nRet != 0)
		{
			VK_SEM_Release(pMux->psiData[ulIndex].ulSemId_Psi);
			return	DRV_ERR_EVENT_INITIALIZATION;
		}
	}

	pMux->psiData[ulIndex].streamMuxTimerId	= 0;

	if(pMux->psiData[ulIndex].psiData)
	{
		DD_MEM_Free(pMux->psiData[ulIndex].psiData);

		pMux->psiData[ulIndex].psiData = DD_MEM_Alloc(psiSize);
		VK_memcpy(pMux->psiData[ulIndex].psiData,pPSIData,psiSize);
	}


	/* add sectiondata to tsdata */
	drvErr = P_STREAM_MUX_MakeTsData(pMux->psiData[ulIndex].psiPid,psiSize,pPSIData,pMux->psiData[ulIndex].continousvalue,startflag,&numofTS,&psiTSdata,&psicontinuous);
	if(drvErr != 0)
	{
		VK_SEM_Release(pMux->psiData[ulIndex].ulSemId_Psi);
		return DRV_ERR;
	}

	if(psiTSdata == NULL)
	{
		VK_SEM_Release(pMux->psiData[ulIndex].ulSemId_Psi);
		return DRV_ERR;
	}

	if(pMux->psiData[ulIndex].psiSize != psiSize )
	{
		P_STREAM_MUX_GetNumofTS(pMux->psiData[ulIndex].psiSize,&onumofTS);

		if(onumofTS != numofTS)
		{
			NEXUS_Memory_Free(pMux->psiData[ulIndex].tsData);
			pMux->psiData[ulIndex].tsData = NULL;
			nErr = NEXUS_Memory_Allocate(TS_TOTAL_SIZE*numofTS, NULL, &(pMux->psiData[ulIndex].tsData));
			if(nErr != 0)
			{
				DD_MEM_Free(psiTSdata);
				VK_SEM_Release(pMux->psiData[ulIndex].ulSemId_Psi);
				return DRV_ERR;
			}
		}
	}

	BKNI_Memcpy(pMux->psiData[ulIndex].tsData,psiTSdata, TS_TOTAL_SIZE*numofTS);
	NEXUS_Memory_FlushCache(pMux->psiData[ulIndex].tsData,TS_TOTAL_SIZE*numofTS);

	pMux->psiData[ulIndex].continousvalue = psicontinuous;
	pMux->psiData[ulIndex].psiSize = psiSize;


	PSIArg[0] = ulDeviceId;
	PSIArg[1] = ulIndex;
	PSIArg[2] = numofTS;

	if(pMux->psiData[ulIndex].contflag == 1)
	{
		nRet = VK_TIMER_EventEvery(pMux->psiData[ulIndex].streamMuxTimeOut,
								P_STREAM_MUX_AddPSIDataTimer,
								PSIArg,
								sizeof(PSIArg),
								&pMux->psiData[ulIndex].streamMuxTimerId);
		if(nRet != VK_OK)
		{
			DD_MEM_Free(psiTSdata);
			VK_SEM_Release(pMux->psiData[ulIndex].ulSemId_Psi);
			return DRV_ERR;
		}

	}
	else
	{
		nRet = VK_TIMER_EventAfter(0,
								P_STREAM_MUX_AddPSIDataTimer,
								PSIArg,
								sizeof(PSIArg),
								&pMux->psiData[ulIndex].streamMuxTimerId);
		if(nRet != VK_OK)
		{
			DD_MEM_Free(psiTSdata);
			VK_SEM_Release(pMux->psiData[ulIndex].ulSemId_Psi);
			return DRV_ERR;
		}

		pMux->psiData[ulIndex].streamMuxTimeOut = 0;

	}

	DD_MEM_Free(psiTSdata);
	VK_SEM_Release(pMux->psiData[ulIndex].ulSemId_Psi);

	return DRV_OK;
}

DRV_Error DRV_STREAM_MUX_Stop(HUINT32 ulDeviceId)
{

	P_MUX_Module *pMux = NULL;
	DRV_Error	drvErr = DRV_OK;
	NEXUS_Error		err = 0;
	HUINT32 index = 0;
	int				nRet = 0;

	if(ulDeviceId >= MAX_MUX_NUM)
		return DRV_ERR_INVALID_PARAMETER;

	pMux = P_STREAM_MUX_GetInstance(ulDeviceId);

	if(pMux == NULL)
		return	DRV_ERR_OUTOFMEMORY;

	nRet = VK_SEM_Get(pMux->ulSemId_DRV);
	if(nRet != 0)
		return	DRV_ERR_EVENT_INITIALIZATION;

	if((pMux->muxIndex == INVALID_MUX_INDEX)||(pMux->useflag == 0))
	{
		VK_SEM_Release(pMux->ulSemId_DRV);
		return	DRV_ERR_INVALID_PARAMETER;
	}

	/* finish stream mux to clean shutdown */
	if(pMux->bIsVideo == TRUE)
	{
		NEXUS_StreamMux_Finish(pMux->hStreamMux);

		if(BKNI_WaitForEvent(pMux->hfinishEvent, 4000)!=BERR_SUCCESS)
		{
			PrintError("[%d]%s TimeOut for MUX, Force Stop mux\n",__LINE__,__func__);
		}
	}

	for(index = 0; index < MAX_NUM_PSI; index++)
	{
		if(pMux->psiData[index].usedflag)
		{
			if(pMux->psiData[index].contflag)
			{
				VK_TIMER_Cancel(pMux->psiData[index].streamMuxTimerId);
				pMux->psiData[index].streamMuxTimerId = 0;
				pMux->psiData[index].streamMuxTimeOut = 0;
			}
			else
			{
				pMux->psiData[index].streamMuxTimerId = 0;
			}
		}
	}

	NEXUS_StreamMux_Stop(pMux->hStreamMux);

	for(index = 0; index < MAX_NUM_PSI; index++)
	{
		if(pMux->psiData[index].usedflag)
		{
			nRet = VK_SEM_Get(pMux->psiData[index].ulSemId_Psi);
			if(nRet != 0)
			{
				VK_SEM_Release(pMux->ulSemId_DRV);
				return DRV_ERR_EVENT_INITIALIZATION;
			}

			if(pMux->psiData[index].hPidChannelTranscodePsi != NULL)
			{
				err = NEXUS_Playpump_ClosePidChannel(pMux->muxConfig.pcr.playpump, pMux->psiData[index].hPidChannelTranscodePsi);
				if(err != NEXUS_SUCCESS)
				{
					PrintError("[%s] NEXUS_Playpump_ClosePidChannel error!\n", __func__);
					drvErr = DRV_ERR;
				}
			}

			NEXUS_Memory_Free(pMux->psiData[index].tsData);
			DD_MEM_Free(pMux->psiData[index].psiData);
			pMux->psiData[index].psiData = NULL;
			pMux->psiData[index].tsData = NULL;
			pMux->psiData[index].psiPid = 0;
			pMux->psiData[index].psiSize = 0;
			BKNI_Memset(&(pMux->psiData[index].systemData), 0, sizeof(NEXUS_StreamMuxSystemData));
			pMux->psiData[index].usedflag = 0;

			VK_SEM_Release(pMux->psiData[index].ulSemId_Psi);
			if(pMux->psiData[index].contflag)
			{
				pMux->psiData[index].contflag = 0;
				VK_SEM_Destroy(pMux->psiData[index].ulSemId_Psi);
			}
		}
	}

	for(index = 0; index < MAX_NUM_PES; index++)
	{
		NEXUS_PlaybackHandle	pbHandle;
		DRV_Error drvErr = DRV_OK;


		if(pMux->pesData[index].usedflag)
		{
			if(pMux->pesData[index].hPidChannelTranscodePes!= NULL)
			{
				err = NEXUS_Playpump_ClosePidChannel(pMux->muxConfig.pcr.playpump, pMux->pesData[index].hPidChannelTranscodePes);
				if(err != NEXUS_SUCCESS)
				{
					PrintError("[%s] NEXUS_Playpump_ClosePidChannel error!\n", __func__);
					drvErr = DRV_ERR;
				}
			}
			if(pMux->bIsWatch == FALSE)
			{
				if(pMux->hPbHandle)
				{
					NEXUS_Playback_ClosePidChannel(pMux->hPbHandle,pMux->pesData[index].hPidChannelPes);
				}
			}
			else
			{
				NEXUS_PidChannel_Close(pMux->pesData[index].hPidChannelPes);
			}

			NEXUS_Message_Stop(pMux->muxConfig.userdata[index].message);
			NEXUS_Message_Close(pMux->muxConfig.userdata[index].message);

			pMux->pesData[index].pesPid = 0;
			pMux->pesData[index].usedflag = 0;
			pMux->pesData[index].hPidChannelPes = NULL;
		}
	}

	err = NEXUS_Playpump_ClosePidChannel(pMux->muxConfig.pcr.playpump, pMux->hPidChannelTranscodePcr);
	{
		if(err != NEXUS_SUCCESS)
		{
			PrintError("[%s] NEXUS_Playpump_ClosePidChannel error!\n", __func__);
			drvErr = DRV_ERR;
		}
	}
	pMux->hPidChannelTranscodePcr = NULL;

	if(pMux->muxConfig.pcr.playpump != NULL)
	{
		DRV_PVR_ReleasePlaypumpHandle(pMux->muxConfig.pcr.playpump);
		pMux->muxConfig.pcr.playpump = NULL;
	}

	if(pMux->muxConfig.video[0].playpump != NULL)
	{
		DRV_PVR_ReleasePlaypumpHandle(pMux->muxConfig.video[0].playpump);
		pMux->muxConfig.video[0].playpump = NULL;
	}

	for(index = 0; index < pMux->ulAudCnt; index++)
	{
		if(pMux->muxConfig.audio[index].playpump != NULL)
		{
			DRV_PVR_ReleasePlaypumpHandle(pMux->muxConfig.audio[index].playpump);
			pMux->muxConfig.audio[index].playpump = NULL;
		}
	}
	pMux->useflag = 0;
	pMux->bIsVideo = FALSE;
	pMux->bIsAudio = FALSE;
	pMux->ulAudCnt = 0;

	VK_SEM_Release(pMux->ulSemId_DRV);
	return drvErr;
}

DRV_Error	DRV_STREAM_MUX_GetDuration(HUINT32 ulDeviceId,HUINT32 *ulTime)
{
	NEXUS_StreamMuxStatus status;
	NEXUS_Error		err = 0;
	P_MUX_Module *pMux = NULL;

	if(ulDeviceId >= MAX_MUX_NUM)
		return DRV_ERR_INVALID_PARAMETER;

	pMux = P_STREAM_MUX_GetInstance(ulDeviceId);

	if(pMux == NULL)
		return	DRV_ERR_OUTOFMEMORY;

	err = NEXUS_StreamMux_GetStatus(pMux->hStreamMux, &status );
	if(err != NEXUS_SUCCESS)
	{
		return DRV_ERR;
	}

	*ulTime = (status.duration/1000);

	return DRV_OK;
}



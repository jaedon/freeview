/***************************************************************************
 *     Copyright (c) 1999-2013, Broadcom Corporation
 *     All Rights Reserved
 *     Confidential Property of Broadcom Corporation
 *
 *
 * THIS SOFTWARE MAY ONLY BE USED SUBJECT TO AN EXECUTED SOFTWARE LICENSE
 * AGREEMENT  BETWEEN THE USER AND BROADCOM.  YOU HAVE NO RIGHT TO USE OR
 * EXPLOIT THIS MATERIAL EXCEPT SUBJECT TO THE TERMS OF SUCH AN AGREEMENT.
 *
 ***************************************************************************/
#ifdef TESTBENCH_BUILD
#include "data_types.h"
#else
#include "bdsp_types.h"
#endif

#include "bdsp_raaga_fw.h"

const BDSP_VOM_Algo_Start_Addr BDSP_sAlgoStartAddr =  
{
	{
		/* BDSP_AF_P_AlgoId_eMpegDecode */
		0x30011000,

		/* BDSP_AF_P_AlgoId_eAc3Decode */
		0x3001f088,

		/* BDSP_AF_P_AlgoId_eAacDecode */
		0x00000000,

		/* BDSP_AF_P_AlgoId_eAacHeLpSbrDecode */
		0x10081000,

		/* BDSP_AF_P_AlgoId_eDdpDecode */
		0x3003f088,

		/* BDSP_AF_P_AlgoId_eDdLosslessDecode */
		0x00000000,

		/* BDSP_AF_P_AlgoId_eLpcmCustomDecode */
		0x30210000,

		/* BDSP_AF_P_AlgoId_eBdLpcmDecode */
		0x00000000,

		/* BDSP_AF_P_AlgoId_eDvdLpcmDecode */
		0x30210000,

		/* BDSP_AF_P_AlgoId_eHdDvdLpcmDecode */
		0x00000000,

		/* BDSP_AF_P_AlgoId_eMpegMcDecode */
		0x00000000,

		/* BDSP_AF_P_AlgoId_eWmaStdDecode */
		0x100c0860,

		/* BDSP_AF_P_AlgoId_eWmaProStdDecode */
		0x30104800,

		/* BDSP_AF_P_AlgoId_eMlpDecode */
		0x103e9800,

		/* BDSP_AF_P_AlgoId_eDdp71Decode */
		0x00000000,

		/* BDSP_AF_P_AlgoId_eDtsDecode */
		0x301c4000,

		/* BDSP_AF_P_AlgoId_eDtsLbrDecode */
		0x100d3000,

		/* BDSP_AF_P_AlgoId_eDtsHdDecode */
		0x301c4000,

		/* BDSP_AF_P_AlgoId_ePcmWavDecode */
		0x100ff000,

		/* BDSP_AF_P_AlgoId_eAmrDecode */
		0x102d4800,

		/* BDSP_AF_P_AlgoId_eDraDecode */
		0x1015f800,

		/* BDSP_AF_P_AlgoId_eRealAudioLbrDecode */
		0x30216000,

		/* BDSP_AF_P_AlgoId_eDolbyPulseDecode */
		0x10169800,

		/* BDSP_AF_P_AlgoId_eMs10DdpDecode */
		0x3005f088,

		/* BDSP_AF_P_AlgoId_eAdpcmDecode */
		0x3025e000,

		/* BDSP_AF_P_AlgoId_eG711G726Decode */
		0x30314000,

		/* BDSP_AF_P_AlgoId_eG729Decode */
		0x30319000,

		/* BDSP_AF_P_AlgoId_eVorbisDecode */
		0x30350800,

		/* BDSP_AF_P_AlgoId_eG723_1Decode */
		0x1037c000,

		/* BDSP_AF_P_AlgoId_eFlacDecode */
		0x103dd658,

		/* BDSP_AF_P_AlgoId_eMacDecode */
		0x303e3000,

		/* BDSP_AF_P_AlgoId_eAmrWbDecode */
		0x10403800,

		/* BDSP_AF_P_AlgoId_eiLBCDecode */
		0x10467f38,

		/* BDSP_AF_P_AlgoId_eiSACDecode */
		0x304b7044,

		/* BDSP_AF_P_AlgoId_eEndOfAudioDecodeAlgos */
		0x00000000,

		/* BDSP_VF_P_AlgoId_eRealVideo9Decode */
		0x00000000,

		/* BDSP_VF_P_AlgoId_eVP6Decode */
		0x1026f800,

		/* BDSP_AF_P_AlgoId_eEndOfDecodeAlgos */
		0x00000000,

		/* BDSP_AF_P_AlgoId_eMpegFrameSync */
		0x30000004,

		/* BDSP_AF_P_AlgoId_eMpegMcFrameSync */
		0x00000000,

		/* BDSP_AF_P_AlgoId_eAdtsFrameSync */
		0x30000004,

		/* BDSP_AF_P_AlgoId_eLoasFrameSync */
		0x30000004,

		/* BDSP_AF_P_AlgoId_eWmaStdFrameSync */
		0x30000004,

		/* BDSP_AF_P_AlgoId_eWmaProFrameSync */
		0x30000004,

		/* BDSP_AF_P_AlgoId_eAc3FrameSync */
		0x30000004,

		/* BDSP_AF_P_AlgoId_eDdpFrameSync */
		0x30000004,

		/* BDSP_AF_P_AlgoId_eDdp71FrameSync */
		0x00000000,

		/* BDSP_AF_P_AlgoId_eDtsFrameSync */
		0x30000004,

		/* BDSP_AF_P_AlgoId_eDtsLbrFrameSync */
		0x30000004,

		/* BDSP_AF_P_AlgoId_eDtsHdFrameSync */
		0x30000004,

		/* BDSP_AF_P_AlgoId_eDtsHdFrameSync_1 */
		0x00000000,

		/* BDSP_AF_P_AlgoId_eDtsHdHdDvdFrameSync */
		0x00000000,

		/* BDSP_AF_P_AlgoId_eDdLosslessFrameSync */
		0x00000000,

		/* BDSP_AF_P_AlgoId_eMlpFrameSync */
		0x30000004,

		/* BDSP_AF_P_AlgoId_eMlpHdDvdFrameSync */
		0x00000000,

		/* BDSP_AF_P_AlgoId_ePesFrameSync */
		0x30000004,

		/* BDSP_AF_P_AlgoId_eBdLpcmFrameSync */
		0x00000000,

		/* BDSP_AF_P_AlgoId_eHdDvdLpcmFrameSync */
		0x00000000,

		/* BDSP_AF_P_AlgoId_eDvdLpcmFrameSync */
		0x30000004,

		/* BDSP_AF_P_AlgoId_eDvdLpcmFrameSync_1 */
		0x00000000,

		/* BDSP_AF_P_AlgoId_ePcmWavFrameSync */
		0x30000004,

		/* BDSP_AF_P_AlgoId_eAmrFrameSync */
		0x30000004,

		/* BDSP_AF_P_AlgoId_eDraFrameSync */
		0x30000004,

		/* BDSP_AF_P_AlgoId_eRealAudioLbrFrameSync */
		0x30000004,

		/* BDSP_AF_P_AlgoId_eMs10DdpFrameSync */
		0x30000004,

		/* BDSP_AF_P_AlgoId_eAdpcmFrameSync */
		0x30000004,

		/* BDSP_AF_P_AlgoId_eG711G726FrameSync */
		0x30000004,

		/* BDSP_AF_P_AlgoId_eG729FrameSync */
		0x30000004,

		/* BDSP_AF_P_AlgoId_eVorbisFrameSync */
		0x30000004,

		/* BDSP_AF_P_AlgoId_eG723_1FrameSync */
		0x30000004,

		/* BDSP_AF_P_AlgoId_eFlacFrameSync */
		0x30000004,

		/* BDSP_AF_P_AlgoId_eMacFrameSync */
		0x30000004,

		/* BDSP_AF_P_AlgoId_eAmrWbFrameSync */
		0x30000004,

		/* BDSP_AF_P_AlgoId_eiLBCFrameSync */
		0x30000004,

		/* BDSP_AF_P_AlgoId_eiSACFrameSync */
		0x30000004,

		/* BDSP_AF_P_AlgoId_eEndOfAudioDecFsAlgos */
		0x00000000,

		/* BDSP_AF_P_AlgoId_eRealVideo9FrameSync */
		0x00000000,

		/* BDSP_AF_P_AlgoId_eVP6FrameSync */
		0x30000004,

		/* BDSP_AF_P_AlgoId_eEndOfDecFsAlgos */
		0x00000000,

		/* BDSP_AF_P_AlgoId_eAc3Encode */
		0x00000000,

		/* BDSP_AF_P_AlgoId_eMpegL2Encode */
		0x00000000,

		/* BDSP_AF_P_AlgoId_eMpegL3Encode */
		0x10305800,

		/* BDSP_AF_P_AlgoId_eAacLcEncode */
		0x00000000,

		/* BDSP_AF_P_AlgoId_eAacHeEncode */
		0x1028df80,

		/* BDSP_AF_P_AlgoId_eDtsEncode */
		0x101fd800,

		/* BDSP_AF_P_AlgoId_eDtsBroadcastEncode */
		0x00000000,

		/* BDSP_AF_P_AlgoId_eSbcEncode */
		0x00000000,

		/* BDSP_AF_P_AlgoId_eMs10DDTranscode */
		0x101df800,

		/* BDSP_AF_P_AlgoId_eG711G726Encode */
		0x10324800,

		/* BDSP_AF_P_AlgoId_eG729Encode */
		0x1032a000,

		/* BDSP_AF_P_AlgoId_eWmaEncode */
		0x30360800,

		/* BDSP_AF_P_AlgoId_eG723_1Encode */
		0x10387800,

		/* BDSP_AF_P_AlgoId_eG722Encode */
		0x103ff000,

		/* BDSP_AF_P_AlgoId_eAmrEncode */
		0x10415800,

		/* BDSP_AF_P_AlgoId_eAmrwbEncode */
		0x30437000,

		/* BDSP_AF_P_AlgoId_eiLBCEncode */
		0x30454038,

		/* BDSP_AF_P_AlgoId_eiSACEncode */
		0x304a7000,

		/* BDSP_AF_P_AlgoId_eEndOfAudioEncodeAlgos */
		0x00000000,

		/* BDSP_VF_P_AlgoId_eH264Encode */
		0x103ac000,

		/* BDSP_VF_P_AlgoId_eVP8Encode */
		0x10487800,

		/* BDSP_AF_P_AlgoId_eEndOfEncodeAlgos */
		0x00000000,

		/* BDSP_AF_P_AlgoId_eAc3EncFrameSync */
		0x00000000,

		/* BDSP_AF_P_AlgoId_eMpegL3EncFrameSync */
		0x00000000,

		/* BDSP_AF_P_AlgoId_eMpegL2EncFrameSync */
		0x00000000,

		/* BDSP_AF_P_AlgoId_eAacLcEncFrameSync */
		0x00000000,

		/* BDSP_AF_P_AlgoId_eAacHeEncFrameSync */
		0x00000000,

		/* BDSP_AF_P_AlgoId_eDtsEncFrameSync */
		0x00000000,

		/* BDSP_AF_P_AlgoId_eWmaEncFrameSync */
		0x00000000,

		/* BDSP_AF_P_AlgoId_eEndOfEncFsAlgos */
		0x00000000,

		/* BDSP_AF_P_AlgoId_ePassThru */
		0x300b4674,

		/* BDSP_AF_P_AlgoId_eMlpPassThru */
		0x30401800,

		/* BDSP_AF_P_AlgoId_eEndOfAuxAlgos */
		0x00000000,

		/* BDSP_AF_P_AlgoId_eSrsTruSurroundPostProc */
		0x00000000,

		/* BDSP_AF_P_AlgoId_eSrcPostProc */
		0x101d8800,

		/* BDSP_AF_P_AlgoId_eDdbmPostProc */
		0x00000000,

		/* BDSP_AF_P_AlgoId_eDownmixPostProc */
		0x00000000,

		/* BDSP_AF_P_AlgoId_eCustomSurroundPostProc */
		0x00000000,

		/* BDSP_AF_P_AlgoId_eCustomBassPostProc */
		0x00000000,

		/* BDSP_AF_P_AlgoId_eKaraokeCapablePostProc */
		0x00000000,

		/* BDSP_AF_P_AlgoId_eCustomVoicePostProc */
		0x10156800,

		/* BDSP_AF_P_AlgoId_ePeqPostProc */
		0x00000000,

		/* BDSP_AF_P_AlgoId_eAvlPostProc */
		0x30260800,

		/* BDSP_AF_P_AlgoId_ePl2PostProc */
		0x00000000,

		/* BDSP_AF_P_AlgoId_eXenPostProc */
		0x00000000,

		/* BDSP_AF_P_AlgoId_eBbePostProc */
		0x00000000,

		/* BDSP_AF_P_AlgoId_eDsolaPostProc */
		0x3020d800,

		/* BDSP_AF_P_AlgoId_eDtsNeoPostProc */
		0x00000000,

		/* BDSP_AF_P_AlgoId_eDDConvert */
		0x00000000,

		/* BDSP_AF_P_AlgoId_eAudioDescriptorFadePostProc */
		0x00000000,

		/* BDSP_AF_P_AlgoId_eAudioDescriptorPanPostProc */
		0x00000000,

		/* BDSP_AF_P_AlgoId_ePCMRouterPostProc */
		0x00000000,

		/* BDSP_AF_P_AlgoId_eWMAPassThrough */
		0x00000000,

		/* BDSP_AF_P_AlgoId_eSrsTruSurroundHDPostProc */
		0x1023d000,

		/* BDSP_AF_P_AlgoId_eSrsTruVolumePostProc */
		0x10233800,

		/* BDSP_AF_P_AlgoId_eDolbyVolumePostProc */
		0x00000000,

		/* BDSP_AF_P_AlgoId_eAudysseyVolumePostProc */
		0x3021f800,

		/* BDSP_AF_P_AlgoId_eBrcm3DSurroundPostProc */
		0x30264000,

		/* BDSP_AF_P_AlgoId_eFWMixerPostProc */
		0x301ef800,

		/* BDSP_AF_P_AlgoId_eMonoDownMixPostProc */
		0x00000000,

		/* BDSP_AF_P_AlgoId_eMs10DDConvert */
		0x00000000,

		/* BDSP_AF_P_AlgoId_eAudysseyABXPostProc */
		0x30267800,

		/* BDSP_AF_P_AlgoId_eSrsCircleSurrPostProc */
		0x30246000,

		/* BDSP_AF_P_AlgoId_eSrsEqualizerPostProc */
		0x30254000,

		/* BDSP_AF_P_AlgoId_eDdrePostProc */
		0x102e4000,

		/* BDSP_AF_P_AlgoId_eDv258PostProc */
		0x102f2000,

		/* BDSP_AF_P_AlgoId_eCustomDbePostProc */
		0x10227000,

		/* BDSP_AF_P_AlgoId_eCustomAcfPostProc */
		0x1022a000,

		/* BDSP_AF_P_AlgoId_eCustomAvlpPostProc */
		0x1022d000,

		/* BDSP_AF_P_AlgoId_eGenCdbItbPostProc */
		0x1026a000,

		/* BDSP_AF_P_AlgoId_eBtscEncoderPostProc */
		0x10349060,

		/* BDSP_AF_P_AlgoId_eSpeexAECPostProc */
		0x10396ca8,

		/* BDSP_AF_P_AlgoId_eEndOfPpAlgos */
		0x00000000,

		/* BDSP_AF_P_AlgoId_eMixerFrameSync */
		0x30304000,

		/* BDSP_AF_P_AlgoId_eEndOfPpFsAlgos */
		0x00000000,

		/* BDSP_AF_P_AlgoId_eSysLib */
		0x00000000,

		/* BDSP_AF_P_AlgoId_eAlgoLib */
		0x00000000,

		/* BDSP_AF_P_AlgoId_eIdsCommon */
		0x00000000,

		/* BDSP_AF_P_AlgoId_eVidIdsCommon */
		0x00000000,

		/* BDSP_AF_P_AlgoId_eEndOfLibAlgos */
		0x00000000,

		/* BDSP_AF_P_AlgoId_eScm1 */
		0x304c2000,

		/* BDSP_AF_P_AlgoId_eScm2 */
		0x304c2800,

		/* BDSP_AF_P_AlgoId_eScm3 */
		0x304c3000,

		/* BDSP_AF_P_AlgoId_eEndOfScmAlgos */
		0x00000000,

		/* BDSP_AF_P_AlgoId_eScmTask */
		0x304e3000,

		/* BDSP_AF_P_AlgoId_eEndOfTaskAlgos */
		0x00000000,

		/* BDSP_AF_P_AlgoId_eEndOfAlgos */
		0x00000000,

	},
};

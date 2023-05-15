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
		0x03011000,

		/* BDSP_AF_P_AlgoId_eAc3Decode */
		0x0301f088,

		/* BDSP_AF_P_AlgoId_eAacDecode */
		0x00000000,

		/* BDSP_AF_P_AlgoId_eAacHeLpSbrDecode */
		0x01081000,

		/* BDSP_AF_P_AlgoId_eDdpDecode */
		0x0303f088,

		/* BDSP_AF_P_AlgoId_eDdLosslessDecode */
		0x00000000,

		/* BDSP_AF_P_AlgoId_eLpcmCustomDecode */
		0x03210000,

		/* BDSP_AF_P_AlgoId_eBdLpcmDecode */
		0x00000000,

		/* BDSP_AF_P_AlgoId_eDvdLpcmDecode */
		0x03210000,

		/* BDSP_AF_P_AlgoId_eHdDvdLpcmDecode */
		0x00000000,

		/* BDSP_AF_P_AlgoId_eMpegMcDecode */
		0x00000000,

		/* BDSP_AF_P_AlgoId_eWmaStdDecode */
		0x010c0860,

		/* BDSP_AF_P_AlgoId_eWmaProStdDecode */
		0x03104800,

		/* BDSP_AF_P_AlgoId_eMlpDecode */
		0x013e9800,

		/* BDSP_AF_P_AlgoId_eDdp71Decode */
		0x00000000,

		/* BDSP_AF_P_AlgoId_eDtsDecode */
		0x031c4000,

		/* BDSP_AF_P_AlgoId_eDtsLbrDecode */
		0x010d3000,

		/* BDSP_AF_P_AlgoId_eDtsHdDecode */
		0x031c4000,

		/* BDSP_AF_P_AlgoId_ePcmWavDecode */
		0x010ff000,

		/* BDSP_AF_P_AlgoId_eAmrDecode */
		0x012d4800,

		/* BDSP_AF_P_AlgoId_eDraDecode */
		0x0115f800,

		/* BDSP_AF_P_AlgoId_eRealAudioLbrDecode */
		0x03216000,

		/* BDSP_AF_P_AlgoId_eDolbyPulseDecode */
		0x01169800,

		/* BDSP_AF_P_AlgoId_eMs10DdpDecode */
		0x0305f088,

		/* BDSP_AF_P_AlgoId_eAdpcmDecode */
		0x0325e000,

		/* BDSP_AF_P_AlgoId_eG711G726Decode */
		0x03314000,

		/* BDSP_AF_P_AlgoId_eG729Decode */
		0x03319000,

		/* BDSP_AF_P_AlgoId_eVorbisDecode */
		0x03350800,

		/* BDSP_AF_P_AlgoId_eG723_1Decode */
		0x0137c000,

		/* BDSP_AF_P_AlgoId_eFlacDecode */
		0x013dc388,

		/* BDSP_AF_P_AlgoId_eMacDecode */
		0x033e3000,

		/* BDSP_AF_P_AlgoId_eAmrWbDecode */
		0x01403800,

		/* BDSP_AF_P_AlgoId_eiLBCDecode */
		0x01467f40,

		/* BDSP_AF_P_AlgoId_eiSACDecode */
		0x034b7050,

		/* BDSP_AF_P_AlgoId_eEndOfAudioDecodeAlgos */
		0x00000000,

		/* BDSP_VF_P_AlgoId_eRealVideo9Decode */
		0x00000000,

		/* BDSP_VF_P_AlgoId_eVP6Decode */
		0x0126f800,

		/* BDSP_AF_P_AlgoId_eEndOfDecodeAlgos */
		0x00000000,

		/* BDSP_AF_P_AlgoId_eMpegFrameSync */
		0x03000004,

		/* BDSP_AF_P_AlgoId_eMpegMcFrameSync */
		0x00000000,

		/* BDSP_AF_P_AlgoId_eAdtsFrameSync */
		0x03000004,

		/* BDSP_AF_P_AlgoId_eLoasFrameSync */
		0x03000004,

		/* BDSP_AF_P_AlgoId_eWmaStdFrameSync */
		0x03000004,

		/* BDSP_AF_P_AlgoId_eWmaProFrameSync */
		0x03000004,

		/* BDSP_AF_P_AlgoId_eAc3FrameSync */
		0x03000004,

		/* BDSP_AF_P_AlgoId_eDdpFrameSync */
		0x03000004,

		/* BDSP_AF_P_AlgoId_eDdp71FrameSync */
		0x00000000,

		/* BDSP_AF_P_AlgoId_eDtsFrameSync */
		0x03000004,

		/* BDSP_AF_P_AlgoId_eDtsLbrFrameSync */
		0x03000004,

		/* BDSP_AF_P_AlgoId_eDtsHdFrameSync */
		0x03000004,

		/* BDSP_AF_P_AlgoId_eDtsHdFrameSync_1 */
		0x00000000,

		/* BDSP_AF_P_AlgoId_eDtsHdHdDvdFrameSync */
		0x00000000,

		/* BDSP_AF_P_AlgoId_eDdLosslessFrameSync */
		0x00000000,

		/* BDSP_AF_P_AlgoId_eMlpFrameSync */
		0x03000004,

		/* BDSP_AF_P_AlgoId_eMlpHdDvdFrameSync */
		0x00000000,

		/* BDSP_AF_P_AlgoId_ePesFrameSync */
		0x03000004,

		/* BDSP_AF_P_AlgoId_eBdLpcmFrameSync */
		0x00000000,

		/* BDSP_AF_P_AlgoId_eHdDvdLpcmFrameSync */
		0x00000000,

		/* BDSP_AF_P_AlgoId_eDvdLpcmFrameSync */
		0x03000004,

		/* BDSP_AF_P_AlgoId_eDvdLpcmFrameSync_1 */
		0x00000000,

		/* BDSP_AF_P_AlgoId_ePcmWavFrameSync */
		0x03000004,

		/* BDSP_AF_P_AlgoId_eAmrFrameSync */
		0x03000004,

		/* BDSP_AF_P_AlgoId_eDraFrameSync */
		0x03000004,

		/* BDSP_AF_P_AlgoId_eRealAudioLbrFrameSync */
		0x03000004,

		/* BDSP_AF_P_AlgoId_eMs10DdpFrameSync */
		0x03000004,

		/* BDSP_AF_P_AlgoId_eAdpcmFrameSync */
		0x03000004,

		/* BDSP_AF_P_AlgoId_eG711G726FrameSync */
		0x03000004,

		/* BDSP_AF_P_AlgoId_eG729FrameSync */
		0x03000004,

		/* BDSP_AF_P_AlgoId_eVorbisFrameSync */
		0x03000004,

		/* BDSP_AF_P_AlgoId_eG723_1FrameSync */
		0x03000004,

		/* BDSP_AF_P_AlgoId_eFlacFrameSync */
		0x03000004,

		/* BDSP_AF_P_AlgoId_eMacFrameSync */
		0x03000004,

		/* BDSP_AF_P_AlgoId_eAmrWbFrameSync */
		0x03000004,

		/* BDSP_AF_P_AlgoId_eiLBCFrameSync */
		0x03000004,

		/* BDSP_AF_P_AlgoId_eiSACFrameSync */
		0x03000004,

		/* BDSP_AF_P_AlgoId_eEndOfAudioDecFsAlgos */
		0x00000000,

		/* BDSP_AF_P_AlgoId_eRealVideo9FrameSync */
		0x00000000,

		/* BDSP_AF_P_AlgoId_eVP6FrameSync */
		0x03000004,

		/* BDSP_AF_P_AlgoId_eEndOfDecFsAlgos */
		0x00000000,

		/* BDSP_AF_P_AlgoId_eAc3Encode */
		0x00000000,

		/* BDSP_AF_P_AlgoId_eMpegL2Encode */
		0x00000000,

		/* BDSP_AF_P_AlgoId_eMpegL3Encode */
		0x01305800,

		/* BDSP_AF_P_AlgoId_eAacLcEncode */
		0x00000000,

		/* BDSP_AF_P_AlgoId_eAacHeEncode */
		0x0128e098,

		/* BDSP_AF_P_AlgoId_eDtsEncode */
		0x011fd800,

		/* BDSP_AF_P_AlgoId_eDtsBroadcastEncode */
		0x00000000,

		/* BDSP_AF_P_AlgoId_eSbcEncode */
		0x00000000,

		/* BDSP_AF_P_AlgoId_eMs10DDTranscode */
		0x011df800,

		/* BDSP_AF_P_AlgoId_eG711G726Encode */
		0x01324800,

		/* BDSP_AF_P_AlgoId_eG729Encode */
		0x0132a000,

		/* BDSP_AF_P_AlgoId_eWmaEncode */
		0x03360800,

		/* BDSP_AF_P_AlgoId_eG723_1Encode */
		0x01387800,

		/* BDSP_AF_P_AlgoId_eG722Encode */
		0x013ff000,

		/* BDSP_AF_P_AlgoId_eAmrEncode */
		0x01415800,

		/* BDSP_AF_P_AlgoId_eAmrwbEncode */
		0x03437000,

		/* BDSP_AF_P_AlgoId_eiLBCEncode */
		0x03454038,

		/* BDSP_AF_P_AlgoId_eiSACEncode */
		0x034a7000,

		/* BDSP_AF_P_AlgoId_eEndOfAudioEncodeAlgos */
		0x00000000,

		/* BDSP_VF_P_AlgoId_eH264Encode */
		0x013ac000,

		/* BDSP_VF_P_AlgoId_eVP8Encode */
		0x01487800,

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
		0x030b45f4,

		/* BDSP_AF_P_AlgoId_eMlpPassThru */
		0x03401800,

		/* BDSP_AF_P_AlgoId_eEndOfAuxAlgos */
		0x00000000,

		/* BDSP_AF_P_AlgoId_eSrsTruSurroundPostProc */
		0x00000000,

		/* BDSP_AF_P_AlgoId_eSrcPostProc */
		0x011d8800,

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
		0x01156800,

		/* BDSP_AF_P_AlgoId_ePeqPostProc */
		0x00000000,

		/* BDSP_AF_P_AlgoId_eAvlPostProc */
		0x03260800,

		/* BDSP_AF_P_AlgoId_ePl2PostProc */
		0x00000000,

		/* BDSP_AF_P_AlgoId_eXenPostProc */
		0x00000000,

		/* BDSP_AF_P_AlgoId_eBbePostProc */
		0x00000000,

		/* BDSP_AF_P_AlgoId_eDsolaPostProc */
		0x0320d800,

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
		0x0123d000,

		/* BDSP_AF_P_AlgoId_eSrsTruVolumePostProc */
		0x01233800,

		/* BDSP_AF_P_AlgoId_eDolbyVolumePostProc */
		0x00000000,

		/* BDSP_AF_P_AlgoId_eAudysseyVolumePostProc */
		0x0321f800,

		/* BDSP_AF_P_AlgoId_eBrcm3DSurroundPostProc */
		0x03264000,

		/* BDSP_AF_P_AlgoId_eFWMixerPostProc */
		0x031ef800,

		/* BDSP_AF_P_AlgoId_eMonoDownMixPostProc */
		0x00000000,

		/* BDSP_AF_P_AlgoId_eMs10DDConvert */
		0x00000000,

		/* BDSP_AF_P_AlgoId_eAudysseyABXPostProc */
		0x03267800,

		/* BDSP_AF_P_AlgoId_eSrsCircleSurrPostProc */
		0x03246000,

		/* BDSP_AF_P_AlgoId_eSrsEqualizerPostProc */
		0x03254000,

		/* BDSP_AF_P_AlgoId_eDdrePostProc */
		0x012e4000,

		/* BDSP_AF_P_AlgoId_eDv258PostProc */
		0x012f2000,

		/* BDSP_AF_P_AlgoId_eCustomDbePostProc */
		0x01227000,

		/* BDSP_AF_P_AlgoId_eCustomAcfPostProc */
		0x0122a000,

		/* BDSP_AF_P_AlgoId_eCustomAvlpPostProc */
		0x0122d000,

		/* BDSP_AF_P_AlgoId_eGenCdbItbPostProc */
		0x0126a000,

		/* BDSP_AF_P_AlgoId_eBtscEncoderPostProc */
		0x01349060,

		/* BDSP_AF_P_AlgoId_eSpeexAECPostProc */
		0x01396c90,

		/* BDSP_AF_P_AlgoId_eEndOfPpAlgos */
		0x00000000,

		/* BDSP_AF_P_AlgoId_eMixerFrameSync */
		0x03304000,

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
		0x034c2000,

		/* BDSP_AF_P_AlgoId_eScm2 */
		0x034c2800,

		/* BDSP_AF_P_AlgoId_eScm3 */
		0x034c3000,

		/* BDSP_AF_P_AlgoId_eEndOfScmAlgos */
		0x00000000,

		/* BDSP_AF_P_AlgoId_eScmTask */
		0x034e3000,

		/* BDSP_AF_P_AlgoId_eEndOfTaskAlgos */
		0x00000000,

		/* BDSP_AF_P_AlgoId_eEndOfAlgos */
		0x00000000,

	},
};

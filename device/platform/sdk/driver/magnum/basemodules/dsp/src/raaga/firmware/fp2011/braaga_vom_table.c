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

const BDSP_VOM_Table BDSP_sVomTable =	
{
	{
		/* BDSP_AF_P_AlgoId_eMpegDecode */
		{
			0x00000022,
			0x00000034,
			0x00000000,
		},
		/* BDSP_AF_P_AlgoId_eAc3Decode */
		{
			0x0000003c,
			0x00000078,
			0x00000000,
		},
		/* BDSP_AF_P_AlgoId_eAacDecode */
		{
			0x00000000,
			0x00000000,
			0x00000000,
		},
		/* BDSP_AF_P_AlgoId_eAacHeLpSbrDecode */
		{
			0x00000102,
			0x0000015e,
			0x00000000,
		},
		/* BDSP_AF_P_AlgoId_eDdpDecode */
		{
			0x0000007c,
			0x000000b8,
			0x00000000,
		},
		/* BDSP_AF_P_AlgoId_eDdLosslessDecode */
		{
			0x00000000,
			0x00000000,
			0x00000000,
		},
		/* BDSP_AF_P_AlgoId_eLpcmCustomDecode */
		{
			0x00000420,
			0x00000429,
			0x00000000,
		},
		/* BDSP_AF_P_AlgoId_eBdLpcmDecode */
		{
			0x00000000,
			0x00000000,
			0x00000000,
		},
		/* BDSP_AF_P_AlgoId_eDvdLpcmDecode */
		{
			0x00000420,
			0x00000429,
			0x00000000,
		},
		/* BDSP_AF_P_AlgoId_eHdDvdLpcmDecode */
		{
			0x00000000,
			0x00000000,
			0x00000000,
		},
		/* BDSP_AF_P_AlgoId_eMpegMcDecode */
		{
			0x00000000,
			0x00000000,
			0x00000000,
		},
		/* BDSP_AF_P_AlgoId_eWmaStdDecode */
		{
			0x00000181,
			0x000001a0,
			0x00000000,
		},
		/* BDSP_AF_P_AlgoId_eWmaProStdDecode */
		{
			0x00000209,
			0x000002a6,
			0x00000000,
		},
		/* BDSP_AF_P_AlgoId_eMlpDecode */
		{
			0x000007d3,
			0x000007f9,
			0x00000000,
		},
		/* BDSP_AF_P_AlgoId_eDdp71Decode */
		{
			0x00000000,
			0x00000000,
			0x00000000,
		},
		/* BDSP_AF_P_AlgoId_eDtsDecode */
		{
			0x00000388,
			0x000003a9,
			0x00000000,
		},
		/* BDSP_AF_P_AlgoId_eDtsLbrDecode */
		{
			0x000001a6,
			0x000001ef,
			0x00000000,
		},
		/* BDSP_AF_P_AlgoId_eDtsHdDecode */
		{
			0x00000388,
			0x000003a9,
			0x00000000,
		},
		/* BDSP_AF_P_AlgoId_ePcmWavDecode */
		{
			0x000001fe,
			0x00000205,
			0x00000000,
		},
		/* BDSP_AF_P_AlgoId_eAmrDecode */
		{
			0x000005a9,
			0x000005c4,
			0x00000000,
		},
		/* BDSP_AF_P_AlgoId_eDraDecode */
		{
			0x000002bf,
			0x000002d0,
			0x00000000,
		},
		/* BDSP_AF_P_AlgoId_eRealAudioLbrDecode */
		{
			0x0000042c,
			0x0000043a,
			0x00000000,
		},
		/* BDSP_AF_P_AlgoId_eDolbyPulseDecode */
		{
			0x000002d3,
			0x0000036f,
			0x00000000,
		},
		/* BDSP_AF_P_AlgoId_eMs10DdpDecode */
		{
			0x000000bc,
			0x000000f8,
			0x00000000,
		},
		/* BDSP_AF_P_AlgoId_eAdpcmDecode */
		{
			0x000004bc,
			0x000004bf,
			0x00000000,
		},
		/* BDSP_AF_P_AlgoId_eG711G726Decode */
		{
			0x00000628,
			0x0000062e,
			0x00000000,
		},
		/* BDSP_AF_P_AlgoId_eG729Decode */
		{
			0x00000632,
			0x00000646,
			0x00000000,
		},
		/* BDSP_AF_P_AlgoId_eVorbisDecode */
		{
			0x000006a1,
			0x000006bb,
			0x00000000,
		},
		/* BDSP_AF_P_AlgoId_eG723_1Decode */
		{
			0x000006f8,
			0x0000070b,
			0x00000000,
		},
		/* BDSP_AF_P_AlgoId_eFlacDecode */
		{
			0x000007ac,
			0x000007bc,
			0x00000000,
		},
		/* BDSP_AF_P_AlgoId_eMacDecode */
		{
			0x000007c6,
			0x000007cd,
			0x00000000,
		},
		/* BDSP_AF_P_AlgoId_eAmrWbDecode */
		{
			0x00000807,
			0x00000827,
			0x00000000,
		},
		/* BDSP_AF_P_AlgoId_eiLBCDecode */
		{
			0x000008cf,
			0x000008e2,
			0x00000000,
		},
		/* BDSP_AF_P_AlgoId_eiSACDecode */
		{
			0x0000096e,
			0x0000097f,
			0x00000000,
		},
		/* BDSP_AF_P_AlgoId_eEndOfAudioDecodeAlgos */
		{
			0x00000000,
			0x00000000,
			0x00000000,
		},
		/* BDSP_VF_P_AlgoId_eRealVideo9Decode */
		{
			0x00000000,
			0x00000000,
			0x00000000,
		},
		/* BDSP_VF_P_AlgoId_eVP6Decode */
		{
			0x000004df,
			0x00000503,
			0x00000000,
		},
		/* BDSP_AF_P_AlgoId_eEndOfDecodeAlgos */
		{
			0x00000000,
			0x00000000,
			0x00000000,
		},
		/* BDSP_AF_P_AlgoId_eMpegFrameSync */
		{
			0x00000036,
			0x0000003a,
			0x00000000,
		},
		/* BDSP_AF_P_AlgoId_eMpegMcFrameSync */
		{
			0x00000000,
			0x00000000,
			0x00000000,
		},
		/* BDSP_AF_P_AlgoId_eAdtsFrameSync */
		{
			0x00000161,
			0x00000162,
			0x00000000,
		},
		/* BDSP_AF_P_AlgoId_eLoasFrameSync */
		{
			0x00000164,
			0x00000166,
			0x00000000,
		},
		/* BDSP_AF_P_AlgoId_eWmaStdFrameSync */
		{
			0x0000017d,
			0x0000017f,
			0x00000000,
		},
		/* BDSP_AF_P_AlgoId_eWmaProFrameSync */
		{
			0x000002a9,
			0x000002ab,
			0x00000000,
		},
		/* BDSP_AF_P_AlgoId_eAc3FrameSync */
		{
			0x000000fc,
			0x00000100,
			0x00000000,
		},
		/* BDSP_AF_P_AlgoId_eDdpFrameSync */
		{
			0x000000fc,
			0x00000100,
			0x00000000,
		},
		/* BDSP_AF_P_AlgoId_eDdp71FrameSync */
		{
			0x00000000,
			0x00000000,
			0x00000000,
		},
		/* BDSP_AF_P_AlgoId_eDtsFrameSync */
		{
			0x00000380,
			0x00000382,
			0x00000000,
		},
		/* BDSP_AF_P_AlgoId_eDtsLbrFrameSync */
		{
			0x000001f2,
			0x000001fc,
			0x00000000,
		},
		/* BDSP_AF_P_AlgoId_eDtsHdFrameSync */
		{
			0x00000384,
			0x00000386,
			0x00000000,
		},
		/* BDSP_AF_P_AlgoId_eDtsHdFrameSync_1 */
		{
			0x00000000,
			0x00000000,
			0x00000000,
		},
		/* BDSP_AF_P_AlgoId_eDtsHdHdDvdFrameSync */
		{
			0x00000000,
			0x00000000,
			0x00000000,
		},
		/* BDSP_AF_P_AlgoId_eDdLosslessFrameSync */
		{
			0x00000000,
			0x00000000,
			0x00000000,
		},
		/* BDSP_AF_P_AlgoId_eMlpFrameSync */
		{
			0x000007cf,
			0x000007d1,
			0x00000000,
		},
		/* BDSP_AF_P_AlgoId_eMlpHdDvdFrameSync */
		{
			0x00000000,
			0x00000000,
			0x00000000,
		},
		/* BDSP_AF_P_AlgoId_ePesFrameSync */
		{
			0x0000017a,
			0x0000017b,
			0x00000000,
		},
		/* BDSP_AF_P_AlgoId_eBdLpcmFrameSync */
		{
			0x00000000,
			0x00000000,
			0x00000000,
		},
		/* BDSP_AF_P_AlgoId_eHdDvdLpcmFrameSync */
		{
			0x00000000,
			0x00000000,
			0x00000000,
		},
		/* BDSP_AF_P_AlgoId_eDvdLpcmFrameSync */
		{
			0x00000413,
			0x00000415,
			0x00000000,
		},
		/* BDSP_AF_P_AlgoId_eDvdLpcmFrameSync_1 */
		{
			0x00000000,
			0x00000000,
			0x00000000,
		},
		/* BDSP_AF_P_AlgoId_ePcmWavFrameSync */
		{
			0x00000625,
			0x00000626,
			0x00000000,
		},
		/* BDSP_AF_P_AlgoId_eAmrFrameSync */
		{
			0x00000625,
			0x00000626,
			0x00000000,
		},
		/* BDSP_AF_P_AlgoId_eDraFrameSync */
		{
			0x000002bc,
			0x000002bd,
			0x00000000,
		},
		/* BDSP_AF_P_AlgoId_eRealAudioLbrFrameSync */
		{
			0x00000417,
			0x00000419,
			0x00000000,
		},
		/* BDSP_AF_P_AlgoId_eMs10DdpFrameSync */
		{
			0x000000fc,
			0x00000100,
			0x00000000,
		},
		/* BDSP_AF_P_AlgoId_eAdpcmFrameSync */
		{
			0x00000625,
			0x00000626,
			0x00000000,
		},
		/* BDSP_AF_P_AlgoId_eG711G726FrameSync */
		{
			0x00000625,
			0x00000626,
			0x00000000,
		},
		/* BDSP_AF_P_AlgoId_eG729FrameSync */
		{
			0x00000625,
			0x00000626,
			0x00000000,
		},
		/* BDSP_AF_P_AlgoId_eVorbisFrameSync */
		{
			0x0000069d,
			0x0000069f,
			0x00000000,
		},
		/* BDSP_AF_P_AlgoId_eG723_1FrameSync */
		{
			0x00000625,
			0x00000626,
			0x00000000,
		},
		/* BDSP_AF_P_AlgoId_eFlacFrameSync */
		{
			0x000007a8,
			0x000007aa,
			0x00000000,
		},
		/* BDSP_AF_P_AlgoId_eMacFrameSync */
		{
			0x000007c2,
			0x000007c4,
			0x00000000,
		},
		/* BDSP_AF_P_AlgoId_eAmrWbFrameSync */
		{
			0x00000625,
			0x00000626,
			0x00000000,
		},
		/* BDSP_AF_P_AlgoId_eiLBCFrameSync */
		{
			0x00000625,
			0x00000626,
			0x00000000,
		},
		/* BDSP_AF_P_AlgoId_eiSACFrameSync */
		{
			0x00000625,
			0x00000626,
			0x00000000,
		},
		/* BDSP_AF_P_AlgoId_eEndOfAudioDecFsAlgos */
		{
			0x00000000,
			0x00000000,
			0x00000000,
		},
		/* BDSP_AF_P_AlgoId_eRealVideo9FrameSync */
		{
			0x00000000,
			0x00000000,
			0x00000000,
		},
		/* BDSP_AF_P_AlgoId_eVP6FrameSync */
		{
			0x000004dc,
			0x000004dd,
			0x00000000,
		},
		/* BDSP_AF_P_AlgoId_eEndOfDecFsAlgos */
		{
			0x00000000,
			0x00000000,
			0x00000000,
		},
		/* BDSP_AF_P_AlgoId_eAc3Encode */
		{
			0x00000000,
			0x00000000,
			0x00000000,
		},
		/* BDSP_AF_P_AlgoId_eMpegL2Encode */
		{
			0x00000000,
			0x00000000,
			0x00000000,
		},
		/* BDSP_AF_P_AlgoId_eMpegL3Encode */
		{
			0x0000060b,
			0x00000623,
			0x00000000,
		},
		/* BDSP_AF_P_AlgoId_eAacLcEncode */
		{
			0x00000000,
			0x00000000,
			0x00000000,
		},
		/* BDSP_AF_P_AlgoId_eAacHeEncode */
		{
			0x00000506,
			0x000005a4,
			0x00000000,
		},
		/* BDSP_AF_P_AlgoId_eDtsEncode */
		{
			0x000003fb,
			0x0000040d,
			0x00000000,
		},
		/* BDSP_AF_P_AlgoId_eDtsBroadcastEncode */
		{
			0x00000000,
			0x00000000,
			0x00000000,
		},
		/* BDSP_AF_P_AlgoId_eSbcEncode */
		{
			0x00000000,
			0x00000000,
			0x00000000,
		},
		/* BDSP_AF_P_AlgoId_eMs10DDTranscode */
		{
			0x000003bf,
			0x000003d8,
			0x00000000,
		},
		/* BDSP_AF_P_AlgoId_eG711G726Encode */
		{
			0x00000649,
			0x00000651,
			0x00000000,
		},
		/* BDSP_AF_P_AlgoId_eG729Encode */
		{
			0x00000654,
			0x0000068d,
			0x00000000,
		},
		/* BDSP_AF_P_AlgoId_eWmaEncode */
		{
			0x000006c1,
			0x000006f4,
			0x00000000,
		},
		/* BDSP_AF_P_AlgoId_eG723_1Encode */
		{
			0x0000070f,
			0x00000729,
			0x00000000,
		},
		/* BDSP_AF_P_AlgoId_eG722Encode */
		{
			0x000007fe,
			0x00000800,
			0x00000000,
		},
		/* BDSP_AF_P_AlgoId_eAmrEncode */
		{
			0x0000082b,
			0x00000864,
			0x00000000,
		},
		/* BDSP_AF_P_AlgoId_eAmrwbEncode */
		{
			0x0000086e,
			0x00000898,
			0x00000000,
		},
		/* BDSP_AF_P_AlgoId_eiLBCEncode */
		{
			0x000008a8,
			0x000008ce,
			0x00000000,
		},
		/* BDSP_AF_P_AlgoId_eiSACEncode */
		{
			0x0000094e,
			0x00000969,
			0x00000000,
		},
		/* BDSP_AF_P_AlgoId_eEndOfAudioEncodeAlgos */
		{
			0x00000000,
			0x00000000,
			0x00000000,
		},
		/* BDSP_VF_P_AlgoId_eH264Encode */
		{
			0x00000758,
			0x000007a4,
			0x00000000,
		},
		/* BDSP_VF_P_AlgoId_eVP8Encode */
		{
			0x0000090f,
			0x0000094a,
			0x00000000,
		},
		/* BDSP_AF_P_AlgoId_eEndOfEncodeAlgos */
		{
			0x00000000,
			0x00000000,
			0x00000000,
		},
		/* BDSP_AF_P_AlgoId_eAc3EncFrameSync */
		{
			0x00000000,
			0x00000000,
			0x00000000,
		},
		/* BDSP_AF_P_AlgoId_eMpegL3EncFrameSync */
		{
			0x00000000,
			0x00000000,
			0x00000000,
		},
		/* BDSP_AF_P_AlgoId_eMpegL2EncFrameSync */
		{
			0x00000000,
			0x00000000,
			0x00000000,
		},
		/* BDSP_AF_P_AlgoId_eAacLcEncFrameSync */
		{
			0x00000000,
			0x00000000,
			0x00000000,
		},
		/* BDSP_AF_P_AlgoId_eAacHeEncFrameSync */
		{
			0x00000000,
			0x00000000,
			0x00000000,
		},
		/* BDSP_AF_P_AlgoId_eDtsEncFrameSync */
		{
			0x00000000,
			0x00000000,
			0x00000000,
		},
		/* BDSP_AF_P_AlgoId_eWmaEncFrameSync */
		{
			0x00000000,
			0x00000000,
			0x00000000,
		},
		/* BDSP_AF_P_AlgoId_eEndOfEncFsAlgos */
		{
			0x00000000,
			0x00000000,
			0x00000000,
		},
		/* BDSP_AF_P_AlgoId_ePassThru */
		{
			0x00000168,
			0x00000176,
			0x00000000,
		},
		/* BDSP_AF_P_AlgoId_eMlpPassThru */
		{
			0x00000803,
			0x00000805,
			0x00000000,
		},
		/* BDSP_AF_P_AlgoId_eEndOfAuxAlgos */
		{
			0x00000000,
			0x00000000,
			0x00000000,
		},
		/* BDSP_AF_P_AlgoId_eSrsTruSurroundPostProc */
		{
			0x00000000,
			0x00000000,
			0x00000000,
		},
		/* BDSP_AF_P_AlgoId_eSrcPostProc */
		{
			0x000003b1,
			0x000003ba,
			0x00000000,
		},
		/* BDSP_AF_P_AlgoId_eDdbmPostProc */
		{
			0x00000000,
			0x00000000,
			0x00000000,
		},
		/* BDSP_AF_P_AlgoId_eDownmixPostProc */
		{
			0x00000000,
			0x00000000,
			0x00000000,
		},
		/* BDSP_AF_P_AlgoId_eCustomSurroundPostProc */
		{
			0x00000000,
			0x00000000,
			0x00000000,
		},
		/* BDSP_AF_P_AlgoId_eCustomBassPostProc */
		{
			0x00000000,
			0x00000000,
			0x00000000,
		},
		/* BDSP_AF_P_AlgoId_eKaraokeCapablePostProc */
		{
			0x00000000,
			0x00000000,
			0x00000000,
		},
		/* BDSP_AF_P_AlgoId_eCustomVoicePostProc */
		{
			0x000002ad,
			0x000002b9,
			0x00000000,
		},
		/* BDSP_AF_P_AlgoId_ePeqPostProc */
		{
			0x00000000,
			0x00000000,
			0x00000000,
		},
		/* BDSP_AF_P_AlgoId_eAvlPostProc */
		{
			0x000004c1,
			0x000004c4,
			0x00000000,
		},
		/* BDSP_AF_P_AlgoId_ePl2PostProc */
		{
			0x00000000,
			0x00000000,
			0x00000000,
		},
		/* BDSP_AF_P_AlgoId_eXenPostProc */
		{
			0x00000000,
			0x00000000,
			0x00000000,
		},
		/* BDSP_AF_P_AlgoId_eBbePostProc */
		{
			0x00000000,
			0x00000000,
			0x00000000,
		},
		/* BDSP_AF_P_AlgoId_eDsolaPostProc */
		{
			0x0000041b,
			0x0000041e,
			0x00000000,
		},
		/* BDSP_AF_P_AlgoId_eDtsNeoPostProc */
		{
			0x00000000,
			0x00000000,
			0x00000000,
		},
		/* BDSP_AF_P_AlgoId_eDDConvert */
		{
			0x00000000,
			0x00000000,
			0x00000000,
		},
		/* BDSP_AF_P_AlgoId_eAudioDescriptorFadePostProc */
		{
			0x00000000,
			0x00000000,
			0x00000000,
		},
		/* BDSP_AF_P_AlgoId_eAudioDescriptorPanPostProc */
		{
			0x00000000,
			0x00000000,
			0x00000000,
		},
		/* BDSP_AF_P_AlgoId_ePCMRouterPostProc */
		{
			0x00000000,
			0x00000000,
			0x00000000,
		},
		/* BDSP_AF_P_AlgoId_eWMAPassThrough */
		{
			0x00000000,
			0x00000000,
			0x00000000,
		},
		/* BDSP_AF_P_AlgoId_eSrsTruSurroundHDPostProc */
		{
			0x0000047a,
			0x00000489,
			0x00000000,
		},
		/* BDSP_AF_P_AlgoId_eSrsTruVolumePostProc */
		{
			0x00000467,
			0x00000475,
			0x00000000,
		},
		/* BDSP_AF_P_AlgoId_eDolbyVolumePostProc */
		{
			0x00000000,
			0x00000000,
			0x00000000,
		},
		/* BDSP_AF_P_AlgoId_eAudysseyVolumePostProc */
		{
			0x0000043f,
			0x0000044b,
			0x00000000,
		},
		/* BDSP_AF_P_AlgoId_eBrcm3DSurroundPostProc */
		{
			0x000004c8,
			0x000004cb,
			0x00000000,
		},
		/* BDSP_AF_P_AlgoId_eFWMixerPostProc */
		{
			0x000003df,
			0x000003ef,
			0x00000000,
		},
		/* BDSP_AF_P_AlgoId_eMonoDownMixPostProc */
		{
			0x00000000,
			0x00000000,
			0x00000000,
		},
		/* BDSP_AF_P_AlgoId_eMs10DDConvert */
		{
			0x00000000,
			0x00000000,
			0x00000000,
		},
		/* BDSP_AF_P_AlgoId_eAudysseyABXPostProc */
		{
			0x000004cf,
			0x000004d1,
			0x00000000,
		},
		/* BDSP_AF_P_AlgoId_eSrsCircleSurrPostProc */
		{
			0x0000048c,
			0x000004a5,
			0x00000000,
		},
		/* BDSP_AF_P_AlgoId_eSrsEqualizerPostProc */
		{
			0x000004a8,
			0x000004b9,
			0x00000000,
		},
		/* BDSP_AF_P_AlgoId_eDdrePostProc */
		{
			0x000005c8,
			0x000005dc,
			0x00000000,
		},
		/* BDSP_AF_P_AlgoId_eDv258PostProc */
		{
			0x000005e4,
			0x00000604,
			0x00000000,
		},
		/* BDSP_AF_P_AlgoId_eCustomDbePostProc */
		{
			0x0000044e,
			0x00000450,
			0x00000000,
		},
		/* BDSP_AF_P_AlgoId_eCustomAcfPostProc */
		{
			0x00000454,
			0x00000456,
			0x00000000,
		},
		/* BDSP_AF_P_AlgoId_eCustomAvlpPostProc */
		{
			0x0000045a,
			0x00000462,
			0x00000000,
		},
		/* BDSP_AF_P_AlgoId_eGenCdbItbPostProc */
		{
			0x000004d4,
			0x000004da,
			0x00000000,
		},
		/* BDSP_AF_P_AlgoId_eBtscEncoderPostProc */
		{
			0x00000692,
			0x0000069b,
			0x00000000,
		},
		/* BDSP_AF_P_AlgoId_eSpeexAECPostProc */
		{
			0x0000072d,
			0x00000750,
			0x00000000,
		},
		/* BDSP_AF_P_AlgoId_eEndOfPpAlgos */
		{
			0x00000000,
			0x00000000,
			0x00000000,
		},
		/* BDSP_AF_P_AlgoId_eMixerFrameSync */
		{
			0x00000608,
			0x00000609,
			0x00000000,
		},
		/* BDSP_AF_P_AlgoId_eEndOfPpFsAlgos */
		{
			0x00000000,
			0x00000000,
			0x00000000,
		},
		/* BDSP_AF_P_AlgoId_eSysLib */
		{
			0x00000012,
			0x00000019,
			0x00000000,
		},
		/* BDSP_AF_P_AlgoId_eAlgoLib */
		{
			0x0000001b,
			0x00000020,
			0x00000000,
		},
		/* BDSP_AF_P_AlgoId_eIdsCommon */
		{
			0x00000000,
			0x00000010,
			0x00000000,
		},
		/* BDSP_AF_P_AlgoId_eVidIdsCommon */
		{
			0x00000754,
			0x00000756,
			0x00000000,
		},
		/* BDSP_AF_P_AlgoId_eEndOfLibAlgos */
		{
			0x00000000,
			0x00000000,
			0x00000000,
		},
		/* BDSP_AF_P_AlgoId_eScm1 */
		{
			0x00000984,
			0x00000984,
			0x00000000,
		},
		/* BDSP_AF_P_AlgoId_eScm2 */
		{
			0x00000985,
			0x00000985,
			0x00000000,
		},
		/* BDSP_AF_P_AlgoId_eScm3 */
		{
			0x00000986,
			0x000009c1,
			0x00000000,
		},
		/* BDSP_AF_P_AlgoId_eEndOfScmAlgos */
		{
			0x00000000,
			0x00000000,
			0x00000000,
		},
		/* BDSP_AF_P_AlgoId_eScmTask */
		{
			0x000009c6,
			0x000009c8,
			0x00000000,
		},
		/* BDSP_AF_P_AlgoId_eEndOfTaskAlgos */
		{
			0x00000000,
			0x00000000,
			0x00000000,
		},
		/* BDSP_AF_P_AlgoId_eEndOfAlgos */
		{
			0x00000000,
			0x00000000,
			0x00000000,
		},
	},
};

/*************************************************************************
*     (c)2005-2012 Broadcom Corporation
*  
*  This program is the proprietary software of Broadcom Corporation and/or its licensors,
*  and may only be used, duplicated, modified or distributed pursuant to the terms and
*  conditions of a separate, written license agreement executed between you and Broadcom
*  (an "Authorized License").  Except as set forth in an Authorized License, Broadcom grants
*  no license (express or implied), right to use, or waiver of any kind with respect to the
*  Software, and Broadcom expressly reserves all rights in and to the Software and all
*  intellectual property rights therein.  IF YOU HAVE NO AUTHORIZED LICENSE, THEN YOU
*  HAVE NO RIGHT TO USE THIS SOFTWARE IN ANY WAY, AND SHOULD IMMEDIATELY
*  NOTIFY BROADCOM AND DISCONTINUE ALL USE OF THE SOFTWARE.  
*   
* $brcm_Workfile: btnr_init.h $
* $brcm_Revision: 11 $
* $brcm_Date: 5/17/12 5:35p $
*
* [File Description:]
*
* Revision History:
*
* $brcm_Log: /AP/ctfe/core/tnr/btnr_init.h $
* 
* 11   5/17/12 5:35p farshidf
* SW3461-211: FW release 5.0
* 
* Fw_Integration_Devel/8   5/17/12 3:56p farshidf
* SW3461-211: FW release 5.0
* 
* Fw_Integration_Devel/AP_V5_0_TNR_DEV/1   5/7/12 6:31p farshidf
* SW3472-2: Make the tuner multi channel
* 
* Fw_Integration_Devel/7   3/9/12 3:14p farshidf
* SW3461-171: merge to integ
* 
* Fw_Integration_Devel/AP_V4_0_TNR_DEV/3   3/9/12 3:05p jputnam
* SW3461-172: Undo unintentional UFE filter coefficient change for 8MHz
*  mode.  8MHz coefficients should now be the same used historically
*  prior to recent 7MHz filter changes.
* 
* Fw_Integration_Devel/AP_V4_0_TNR_DEV/2   3/6/12 5:46p jputnam
* SW3461-172: Further sharpen 7MHz filter to improve acquisition
*  probability with N-1 PAL  B/G
* 
* Fw_Integration_Devel/AP_V4_0_TNR_DEV/1   3/5/12 5:39p jputnam
* SW3461-172: Optimize UFE SAW coefficients for channel bandwidths less
*  than 8MHz
* 
* Fw_Integration_Devel/5   9/7/11 3:11p farshidf
* SW3461-1: merge to integ
* 
* Fw_Integration_Devel/AP_V2_0_TNR_DEV/2   9/7/11 11:31a jputnam
* SW3461-1: Added new UFE filter coefficients for SmartTune
*  FreqPlanAlternate
* 
* 7   8/26/11 3:45p farshidf
* SW3461-1: merge to main
* 
* Fw_Integration_Devel/4   8/26/11 3:38p farshidf
* SW3461-1: merge to integ
* 
* Fw_Integration_Devel/AP_V2_0_TNR_DEV/1   8/24/11 4:52p mbsingh
* bcm3461-1: Removing Terrestrial Init() to save code size. This function
*  is not required.
* 
* Fw_Integration_Devel/2   7/22/11 4:18p farshidf
* SW3461-1: merge to integ
* 
* Fw_Integration_Devel/AP_V0_6_TNR_DEV/3   7/14/11 5:21p cbrooks
* sw3461-1:added callback
* 
* Fw_Integration_Devel/AP_V0_6_TNR_DEV/2   7/7/11 4:47p cbrooks
* SW3461-1:working on callbacak functions and status
* 
* Fw_Integration_Devel/AP_V0_6_TNR_DEV/1   7/1/11 3:19p cbrooks
* sw3461-1:added ADS callback support
* 
* Fw_Integration_Devel/1   6/29/11 12:39p farshidf
* SW3461-13: merge to integration branch
* 
* Fw_Integration_Devel/Tnr_Fw_Devel_Rc05/1   6/22/11 6:48p cbrooks
* sw3461-1:Added Callback Support
* 
* 4   6/9/11 6:40p mpovich
* SW3461-1: Merge Ver 0.4 Integ. onto main branch.
* 
* SW_System_4_Integ_Test/1   6/6/11 2:07p mpovich
* SW3461-1: Merge disparate branches for test purposes.
* 
* Tnr_Fw_Devel_4/1   5/20/11 3:31p farshidf
* SW3461-1: latest UFE settings
* 
* 3   5/20/11 6:43a mpovich
* SW3461-1: rename UFE (BUFE) module to TNR (BTNR).
* 
* TNR_3461_1/1   5/19/11 5:13p mpovich
* SW3461-1: Change BUFE module prefix to BTNR
* 
* 2   3/9/11 7:22p cbrooks
* sw3461-1:added tnr programming code
* 
* 1   2/2/11 9:47p farshidf
* SW3461-1: new file
* 
* 
***************************************************************************/

#ifndef _BTNR_INIT_H__
#define _BTNR_INIT_H__

#ifdef __cplusplus
extern "C" {
#endif


#define BTNR_SAW_TABLE_SIZE  9
#define BTNR_SAW_COEFF_SIZE 33
static const uint32_t BTNR_SAW_Table[BTNR_SAW_TABLE_SIZE][BTNR_SAW_COEFF_SIZE] =
{
	/*Cable 8 MHz, Fs=540MHz, CIC=5->3, HB=3->2, Fs_dec=Fs/CIC/(2^(HB))=13.5MHz*/
	/*Use this setting for 6.25MB<Fb<7.15MB*/
	{0x00000076, 0x000000D6, 0x0007FF7B, 0x0007FF2A, 0x0000018A, 0x0000004B, 0x0007FD32, 0x000001A9, 
	 0x0000032C, 0x0007FB02, 0x0007FF06, 0x0000082C, 0x0007FB27, 0x0007F7F4, 0x00000D12, 0x00000149, 
	 0x0007ECD0, 0x00000CF2, 0x00001067, 0x0007E1B1, 0x000000F6, 0x000028A2, 0x0007DF25, 0x0007E192, 
	 0x000045EC, 0x0007F389, 0x0007A36C, 0x000060BC, 0x0000444C, 0x000709F6, 0x000070D3, 0x0003FFFF, 
	 0x00000000},
	
	/* Terrestrial 7.25 MHz, Fs=540MHz, CIC=5->3, HB=3->2, Fs_dec=Fs/CIC/(2^(HB)) (to pass D-Book N-1 PAL-I1) */ 	 
	 {0x0000001c, 0x000000cf, 0x000000ba, 0x0007ff05, 0x0007ff26, 0x000001fb, 0x000000c8, 0x0007fc76, 
	  0x00000000, 0x00000589, 0x0007fdf5, 0x0007f877, 0x000005db, 0x000008b2, 0x0007f44a, 0x0007f83a, 
	  0x00001365, 0x0000033d, 0x0007e401, 0x0000068b, 0x000023bc, 0x0007e8e1, 0x0007d825, 0x00003002, 
	  0x0000244e, 0x0007ac78, 0x0007edbb, 0x00008888, 0x0007e04b, 0x00070fb8, 0x0000d81e, 0x0003e4d2, 
	  0x00000000},

	/*Cable/Terrestrial 7 MHz, Fs=540MHz, CIC=5->3, HB=3->2, Fs_dec=Fs/CIC/(2^(HB))=13.5MHz*/
	/*if QAM use this setting for 5.36MB<Fb<6.25MB*/
	{0x00000061, 0x00000048, 0x0007FF04, 0x0007FEED, 0x00000143, 0x00000123, 0x0007FD54, 0x0007FE84, 
	 0x00000472, 0x0000012B, 0x0007F8EB, 0x0007FFF8, 0x00000A58, 0x0007FD68, 0x0007F1F1, 0x00000749, 
	 0x000011AE, 0x0007F147, 0x0007EB9E, 0x00001990, 0x000014F6, 0x0007D76E, 0x0007EE54, 0x00003CDF, 
	 0x000007C7, 0x0007A701, 0x00000E3E, 0x000084A6, 0x0007BF1F, 0x00071D7A, 0x0000F668, 0x0003CF49, 
	 0x00000000},

	/*Cable/Terrestrial 6 MHz, Fs=540MHz, CIC=5->3, HB=3->2, Fs_dec=Fs/CIC/(2^(HB))=13.5MHz*/
	/*if QAM  use this setting for 4.46MB<Fb<5.36MB*/
	{0x00000009, 0x0007FF5C, 0x0007FE92, 0x0007FF78, 0x000001BF, 0x00000181, 0x0007FDB8, 0x0007FCBB, 
	 0x00000265, 0x000005D4, 0x0007FE5B, 0x0007F6E5, 0x0007FF86, 0x00000CD8, 0x00000483, 0x0007EF6D, 
	 0x0007F4FC, 0x00001396, 0x00001488, 0x0007EB20, 0x0007DE62, 0x00001315, 0x0000330F, 0x0007F3CD, 
	 0x0007B58F, 0x0007FCA4, 0x00006C12, 0x0000246D, 0x000758A9, 0x0007853E, 0x000161BF, 0x00036433, 
	 0x00000000},

	/*Cable/Terrestrial 5 MHz, Fs=540MHz, CIC=2->0, HB=5->0, Fs_dec=Fs/CIC/(2^(HB))=8.4375MHz*/
	/*if QAM  use this setting for 3.58MB<Fb<4.45MB*/
	{0x00000001, 0x0000002B, 0x00000013, 0x0007FF9B, 0x0000002D, 0x000000B5, 0x0007FF03, 0x0007FF82, 
	 0x00000248, 0x0007FED7, 0x0007FD17, 0x00000486, 0x000000C0, 0x0007F822, 0x000005B7, 0x0000070E, 
   0x0007F143, 0x0000024D, 0x0000139E, 0x0007EBBC, 0x0007F535, 0x00002662, 0x0007EE48, 0x0007D83F, 
   0x00003C68, 0x00000514, 0x0007A04E, 0x00005051, 0x000053C3, 0x00071032, 0x00005C32, 0x0003FFFF, 
   0x00000000},

	/*Cable 4 MHz, Fs=540MHz, CIC=5->3, HB=4->1, Fs_dec=Fs/CIC/(2^(HB))=6.75MHz*/
	/*if QAM use this setting for 2.7MB<Fb<3.57MB*/
	/*Fb,max=4/1.12=3.57, Fb,min=2.7, Fpass=1.8, Fstop=3.8*/
	{0x0000001F, 0x0007FFCB, 0x0007FF57, 0x00000058, 0x000000DA, 0x0007FEB5, 0x0007FF4D, 0x000002BB, 
	 0x0007FF50, 0x0007FC00, 0x000003D7, 0x00000398, 0x0007F7AC, 0x00000054, 0x00000C0D, 0x0007F745, 
	 0x0007F4AC, 0x00001425, 0x0000022E, 0x0007E251, 0x0000114B, 0x00001D70, 0x0007D39B, 0x0007F580, 
	 0x00004642, 0x0007DD6B, 0x0007B0A5, 0x0000707A, 0x00002CF5, 0x00070677, 0x00008B86, 0x0003FFFF, 
	 0x00000000},
	
	/*Cable 3 MHz, Fs=540MHz, CIC=3->1, HB=5->0, Fs_dec=Fs/CIC/(2^(HB))=5.625MHz*/
	/*if QAM use this setting for 1.8MB<Fb<2.68MB*/
	/*Fb,max=3/1.12=2.68, Fb,min=1.8, Fpass=1.35, Fstop=2.6*/
	{0x00000044, 0x0007FFF3, 0x0007FEDA, 0x0007FF50, 0x00000184, 0x000000A3, 0x0007FD2F, 0x0007FF6E, 
	 0x00000496, 0x0007FFEA, 0x0007F907, 0x000001AD, 0x000009DB, 0x0007FB4F, 0x0007F30A, 0x000009AC, 
	 0x00000FC9, 0x0007EED0, 0x0007EE72, 0x00001BD2, 0x00001127, 0x0007D5B8, 0x0007F307, 0x00003DB4, 
	 0x00000266, 0x0007A755, 0x000013F7, 0x000082FF, 0x0007B979, 0x00072079, 0x0000FB8C, 0x0003CB20, 
	 0x00000000},
	
	/*Cable 2 MHz, Fs=540MHz, CIC=4->2, HB=5->0, Fs_dec=Fs/CIC/(2^(HB))=4.22MHz*/
	/*if QAM use this setting for 1.5MB<Fb<1.78MB*/
	/*Fb,max=2/1.12=1.78, Fb,min=1.5, Fpass=0.9, Fstop=2.16*/
	{0x0000008D, 0x0000007A, 0x0007FEC9, 0x0007FD94, 0x0007FFEE, 0x000002F4, 0x00000069, 0x0007FB41, 
	 0x0007FDF8, 0x00000667, 0x0000046B, 0x0007F7D2, 0x0007F7EB, 0x000009A2, 0x00000D2E, 0x0007F590, 
	 0x0007EBFE, 0x00000A13, 0x00001CDC, 0x0007F824, 0x0007D7D3, 0x000002D4, 0x000036C3, 0x000006A0, 
	 0x0007B5A2, 0x0007E845, 0x0000679D, 0x00003910, 0x000761AF, 0x000771B7, 0x00015480, 0x00037597, 
	 0x00000000},
	
	/*Cable/Terrestrial 1.7 MHz, Fs=540MHz, CIC=5->3, HB=5->0, Fs_dec=Fs/CIC/(2^(HB)) */
	/*if QAM use this setting for 1.0MB<Fb<1.5MB*/
	{0x0007FF9E, 0x0007FFDA, 0x00000146, 0x00000185, 0x0007FEE9, 0x0007FDF1, 0x000001F1, 0x00000381, 
	 0x0007FD2D, 0x0007FAA7, 0x00000425, 0x000007DA, 0x0007FA0C, 0x0007F4E1, 0x00000874, 0x00000F55, 
	 0x0007F420, 0x0007EB46, 0x00001094, 0x00001BB2, 0x0007E8DF, 0x0007DB1A, 0x00002083, 0x000031A7, 
	 0x0007D140, 0x0007BB10, 0x000046BB, 0x00006721, 0x00078773, 0x0007444C, 0x00012A35, 0x00039EF7, 
	 0x00000000}
};

static const uint32_t BTNR_SAW_Table_FreqPlanDefault[5][BTNR_SAW_COEFF_SIZE] =
{
	/* Terrestrial Channel=8MHz, Cutoff=7.25MHz, Fs=540MHz, CIC=5->3, HB=3->2, Fs_dec=Fs/CIC/(2^(HB)) */ 	  
	 {0x0000001c, 0x000000cf, 0x000000ba, 0x0007ff05, 0x0007ff26, 0x000001fb, 0x000000c8, 0x0007fc76, 
	  0x00000000, 0x00000589, 0x0007fdf5, 0x0007f877, 0x000005db, 0x000008b2, 0x0007f44a, 0x0007f83a, 
	  0x00001365, 0x0000033d, 0x0007e401, 0x0000068b, 0x000023bc, 0x0007e8e1, 0x0007d825, 0x00003002, 
	  0x0000244e, 0x0007ac78, 0x0007edbb, 0x00008888, 0x0007e04b, 0x00070fb8, 0x0000d81e, 0x0003e4d2, 
	  0x00000000},
	/* Terrestrial Channel=7MHz, Cutoff=6.15MHz, Fs=540MHz, CIC=5->3, HB=3->2, Fs_dec=Fs/CIC/(2^(HB)) */ 	 
	 {0x0000006a, 0x00000085, 0x0007ff5b, 0x0007fe41, 0x0007ffd9, 0x0000025d, 0x0000008e, 0x0007fc16, 
	  0x0007fdeb, 0x00000556, 0x00000448, 0x0007f90e, 0x0007f83b, 0x0000083a, 0x00000ca2, 0x0007f711, 
	  0x0007ecc2, 0x00000880, 0x00001be1, 0x0007f9ba, 0x0007d8fc, 0x00000142, 0x00003573, 0x00000828, 
	  0x0007b70d, 0x0007e6ca, 0x0000661e, 0x00003a7c, 0x0007633f, 0x00077062, 0x000152c8, 0x0003764e, 
	  0x00000000},  
	/* Terrestrial Channel=6MHz, Cutoff=5.44MHz, Fs=540MHz, CIC=5->3, HB=3->2, Fs_dec=Fs/CIC/(2^(HB)) */ 	 
	 {0x000000ab, 0x000001bb, 0x00000155, 0x0007feff, 0x0007fd4c, 0x0007ffb5, 0x000003e9, 0x0000032a, 
	  0x0007fc4d, 0x0007f8f7, 0x000000be, 0x00000a6f, 0x00000581, 0x0007f502, 0x0007f1bb, 0x00000640, 
	  0x000016fb, 0x00000562, 0x0007e486, 0x0007e889, 0x000016bd, 0x00002ccf, 0x0007fbf8, 0x0007c0ce, 
	  0x0007dfba, 0x00004560, 0x0000599f, 0x0007cf19, 0x00074c89, 0x0007da48, 0x00018cd8, 0x00031b0b, 
	  0x00000000},
	/* Terrestrial Channel=5MHz, Cutoff=4.53MHz, Fs=540MHz, CIC=2->0, HB=5->0, Fs_dec=Fs/CIC/(2^(HB))=Fs/64 MHz */
	 {0x0007fffa, 0x00000037, 0x00000063, 0x0007ffa6, 0x0007ff5c, 0x000000f3, 0x000000e4, 0x0007fde9, 
	  0x0007ff41, 0x000003df, 0x0007ffa7, 0x0007f9e8, 0x00000315, 0x0000081b, 0x0007f7ee, 0x0007f756, 
	  0x00000f7f, 0x00000601, 0x0007e73b, 0x000001f9, 0x00002236, 0x0007ee9a, 0x0007d730, 0x00002a4c, 
	  0x000027f8, 0x0007b0cb, 0x0007e7e0, 0x000086cd, 0x0007e724, 0x00070e34, 0x0000d1ec, 0x0003e9d5, 
	  0x00000000},
	/* Terrestrial Channel=1.7MHz, Cutoff=1.54MHz, Fs=540MHz, CIC=5->3, HB=5->0, Fs_dec=Fs/CIC/(2^(HB))=Fs/160 MHz */
	 {0x0007fff8, 0x0007ffd7, 0x0007ffd6, 0x0000003d, 0x0000008e, 0x0007ffd1, 0x0007febf, 0x0007ffb4, 
	  0x00000234, 0x0000018f, 0x0007fcdd, 0x0007fc09, 0x00000381, 0x000007c2, 0x0007fd8e, 0x0007f325, 
	  0x0007fed9, 0x000012b6, 0x00000889, 0x0007e7d8, 0x0007eb07, 0x00001b46, 0x000027bf, 0x0007e6c7, 
	  0x0007bd6e, 0x00000d75, 0x00006966, 0x000011b6, 0x00075528, 0x0007977d, 0x00016b39, 0x0003540c, 
	  0x00000000}   
};

static const uint32_t BTNR_SAW_Table_FreqPlanAlternate[5][BTNR_SAW_COEFF_SIZE] =
{
	/* Terrestrial Channel=8MHz, Cutoff=7.25MHz, Fs=513MHz, CIC=5->3, HB=3->2, Fs_dec=Fs/CIC/(2^(HB)) (to pass D-Book N-1 PAL-I1) */ 	 
	 {0x0007ffde, 0x0007ff56, 0x0007ffb9, 0x00000100, 0x0007ffec, 0x0007fe3b, 0x00000126, 0x00000256, 
	  0x0007fcbc, 0x0007fe32, 0x0000062e, 0x0007ff0d, 0x0007f74e, 0x000006a5, 0x0000089d, 0x0007f144, 
	  0x0007fcb5, 0x000016ae, 0x0007f6d6, 0x0007e624, 0x00001c68, 0x00001256, 0x0007cd70, 0x000005b4, 
	  0x000043af, 0x0007cd0c, 0x0007bcca, 0x00007b47, 0x00001a23, 0x000704c9, 0x0000a06b, 0x0003ffff, 
	  0x00000000},
	/* Terrestrial Channel=7MHz, Cutoff=6.15MHz, Fs=513MHz, CIC=5->3, HB=3->2, Fs_dec=Fs/CIC/(2^(HB)) */ 	 
	 {0x0007ffd3, 0x00000023, 0x00000104, 0x000000b1, 0x0007feb9, 0x0007fecf, 0x00000225, 0x00000230, 
	  0x0007fcb0, 0x0007fc5e, 0x000004ef, 0x000005ab, 0x0007f8df, 0x0007f78f, 0x00000a12, 0x00000c24, 
	  0x0007f203, 0x0007eef7, 0x00001338, 0x00001786, 0x0007e5b0, 0x0007dfb3, 0x0000243c, 0x00002cb2, 
	  0x0007cd07, 0x0007c04b, 0x00004b69, 0x000061be, 0x00078262, 0x000749b5, 0x00012fa1, 0x000399f2, 
	  0x00000000}, 
	/* Terrestrial Channel=6MHz, Cutoff=5.44MHz, Fs=513MHz, CIC=5->3, HB=3->2, Fs_dec=Fs/CIC/(2^(HB)) */ 	 
	 {0x0007ff99, 0x0007ffb5, 0x00000104, 0x00000226, 0x0000008e, 0x0007fd72, 0x0007fe0e, 0x00000323, 
	  0x000004b4, 0x0007fdb8, 0x0007f7fb, 0x0007ff73, 0x00000b2f, 0x000005f9, 0x0007f32d, 0x0007f1fa, 
	  0x00000b42, 0x00001809, 0x0007fb5b, 0x0007dd96, 0x0007f744, 0x00002a9d, 0x00001e5a, 0x0007d2fa, 
	  0x0007c237, 0x0000245a, 0x00006a88, 0x0007f9a8, 0x00074d8c, 0x0007ae86, 0x000178d3, 0x00033f18, 
	  0x00000000},
	/* Terrestrial Channel=5MHz, Cutoff=4.53MHz, Fs=513MHz, CIC=2->0, HB=5->0, Fs_dec=Fs/CIC/(2^(HB))=Fs/64 MHz */
	 {0x00000001, 0x0007ffd0, 0x0007ffcd, 0x0000006c, 0x0000002d, 0x0007fefd, 0x00000048, 0x000001be, 
	  0x0007fe5b, 0x0007fe01, 0x00000417, 0x000000a1, 0x0007f903, 0x0000038e, 0x00000876, 0x0007f522, 
	  0x0007fa6d, 0x00001394, 0x0007fb77, 0x0007e6bb, 0x000016cf, 0x00001541, 0x0007d1bb, 0x0007ffcc, 
	  0x000042d3, 0x0007d3e1, 0x0007b962, 0x00007640, 0x000020c4, 0x00070618, 0x000098b9, 0x0003ffff, 
	  0x00000000},
	/* Terrestrial Channel=1.7MHz, Cutoff=1.54MHz, Fs=513MHz, CIC=5->3, HB=5->0, Fs_dec=Fs/CIC/(2^(HB))=Fs/160 MHz */
	 {0x0000000d, 0x00000021, 0x0007fff9, 0x0007ffa7, 0x0007ffe9, 0x000000c1, 0x00000070, 0x0007fea1, 
	  0x0007feca, 0x00000232, 0x000002aa, 0x0007fcda, 0x0007fae3, 0x0000040b, 0x000008ec, 0x0007fb73, 
	  0x0007f184, 0x0000042d, 0x00001642, 0x0007fdce, 0x0007df33, 0x0007fd89, 0x00002efd, 0x00000b85, 
	  0x0007bd69, 0x0007e3ad, 0x00006028, 0x00003d8a, 0x000768a0, 0x00076d30, 0x00014dea, 0x00037928, 
	  0x00000000}   
};	  
	
#if 0
BERR_Code BTNR_P_TerrestrialInit(BTNR_3x7x_ChnHandle hTnr);   
#endif

BERR_Code BTNR_P_TnrMiniInit(BTNR_3x7x_ChnHandle hTnr);   
BERR_Code BTNR_P_TnrInit(BTNR_3x7x_ChnHandle hTnr); 

void BTNR_P_Program_TNR(BTNR_3x7x_ChnHandle hTnr);
void BTNR_P_Program_VID_CIC_HB_SAW(BTNR_3x7x_ChnHandle hTnr);
void BTNR_P_Program_Back_DDFS(BTNR_3x7x_ChnHandle hTnr);
void BTNR_P_InitStatus(BTNR_3x7x_ChnHandle hTnr);

#ifdef __cplusplus
}
#endif

#endif
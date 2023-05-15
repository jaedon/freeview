#if defined(CONFIG_VK_STDLIB)
#else
#include 	<stdio.h>
#include 	<stdlib.h>
#include 	<string.h>
#endif
#include <fcntl.h>

#include <unistd.h>

#include "htype.h"
#include "di_err.h"

#include "cmdtool.h"
#include "di_dsc.h"
#include "di_flash.h"

#include "di_demux.h"
#include "nexus_base_os.h"

/* return value of command function */
#define CMD_OK					0
#define CMD_ERR					1

#define CMD_DSC_DEBUG

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

//#define DSC_DEBUG_PRINT

/* global variables */

/* static variables */

/* static functions */

/* extern functions */

static HUINT32 g_vidDescId;
static HUINT32 g_audDescId;

int CMD_HwTest_Parser_DSC(void *szArgs)
{
	GET_ARGS;
//20090703 :: POOH :: single fixed key test
//                                  only video descramble.
//                                  I don't know audio key value.
//
//Stream name : Lw_Qpsk_EvenFixedKey.mpg
	if (CMD_IS("dvb"))
	{
		HUINT32 vidDescId;
		HUINT32 audDescId;

		HUINT16 videoPid = 0x45;
		HUINT16 audioPid = 0x44;

		HUINT32 key_length = 8;
		unsigned char OddControlWord[] = { 0x64, 0x63, 0x6F, 0x6D, 0x54, 0x65, 0x73, 0x74 };
		unsigned char EvenControlWord[] = { 0x42, 0x63, 0x73, 0x64, 0x42, 0x72, 0x6F, 0x61 };

		DI_DSC_Open(0, DI_DSC_DESC_DVB, &vidDescId,NULL);
		DI_DSC_Open(0, DI_DSC_DESC_DVB, &audDescId,NULL);
#if 1
		g_vidDescId = vidDescId;
		g_audDescId = audDescId;
#endif

		DI_DSC_SetPid(vidDescId, videoPid);
		DI_DSC_SetKey(vidDescId,
					DI_DSC_DESC_ODD,
					OddControlWord,
					key_length,
					NULL,
					0);
		DI_DSC_SetKey(vidDescId,
					DI_DSC_DESC_EVEN,
					EvenControlWord,
					key_length,
					NULL,
					0);

		DI_DSC_SetPid(audDescId, audioPid);
		DI_DSC_SetKey(audDescId,
					DI_DSC_DESC_ODD,
					OddControlWord,
					key_length,
					NULL,
					0);
		DI_DSC_SetKey(audDescId,
					DI_DSC_DESC_EVEN,
					EvenControlWord,
					key_length,
					NULL,
					0);

		iResult = CMD_OK;
	}
	else if (CMD_IS("close"))
	{
		HUINT16 videoPid = 0x45;
		HUINT16 audioPid = 0x44;
		
		DI_DSC_ClearPid(g_vidDescId, videoPid);
		DI_DSC_Close(g_vidDescId);

		DI_DSC_ClearPid(g_audDescId, audioPid);
		DI_DSC_Close(g_audDescId);

		iResult = CMD_OK;
	}
	else if (CMD_IS("dump"))
	{
		HUINT32 test_case;

		if (szParam == NULL)
		{
			CMD_Printf("Choose a test case [0~1]\n");
			return CMD_OK;
		}
		else
		{
			test_case = VK_atoi((char *)szParam);
		}
		if (test_case > 6)
		{
			CMD_Printf("Out of range, choose a right test case [0~6]\n");
			return CMD_OK;
		}

		DRV_DSC_DumpKeySlot(test_case);
		iResult = CMD_OK;
	}
	else if (CMD_IS("keyslot"))
	{
		HUINT16 videoPid;
		HUINT16 audioPid;

		if(READABLE_IN_HEX(HWTEST_PARAM, videoPid))
		{

			if(READABLE_IN_HEX(HWTEST_PARAM1, audioPid))
			{
				CMD_Printf("Find keyslot handle about video PID (0x%x)!\n", videoPid);
				DRV_DSC_GetKeySlotByPidNum(videoPid);

				CMD_Printf("Find keyslot handle about audio PID (0x%x)!\n", audioPid);
				DRV_DSC_GetKeySlotByPidNum(audioPid);	
			}
			else
			{
				CMD_Printf("Enter audio PID\n");
				return CMD_OK;
			}
		}
		else
		{
			CMD_Printf("Enter video PID\n");
			return CMD_OK;
		}
		iResult = CMD_OK;
	}
//20090703 :: POOH :: single fixed key test
//					only video descramble.
//					i don't know audio key value.
//
//	Stream name : \\stream\stream\functions\descrambler_test\spiderman_aes.ts
//command :: av test custom 0 17 20 17 mpeg2 ac3

	else if (CMD_IS("aes"))
	{
		HUINT32 vidDescId;
		HUINT32 audDescId;

		HUINT16 videoPid = 0x11;
		HUINT16 audioPid = 0x14;

		HUINT32 key_length = 16;
		unsigned char	VidEvenControlWord[] = {
								0x12, 0x34, 0x56, 0x78, 0x9a, 0xbc, 0xde, 0xf0,
								0x12, 0x34, 0xfe, 0xed, 0xba, 0xbe, 0xbe, 0xef	};
		unsigned char VidOddControlWord[] = {
								0x12, 0x34, 0x56, 0x78, 0x9a, 0xbc, 0xde, 0xf0,
								0x12, 0x34, 0xfe, 0xed, 0xba, 0xbe, 0xbe, 0xef	};
		unsigned char AudEvenControlWord[] = {
								0x12, 0x34, 0x56, 0x78, 0x9a, 0xbc, 0xde, 0xf0,
								0x12, 0x34, 0xfe, 0xed, 0xba, 0xbe, 0xbe, 0xef	};
		unsigned char AudOddControlWord[] = {
								0x12, 0x34, 0x56, 0x78, 0x9a, 0xbc, 0xde, 0xf0,
								0x12, 0x34, 0xfe, 0xed, 0xba, 0xbe, 0xbe, 0xef	};

		unsigned char iv[]={0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0};

		DI_DSC_Open(0, DI_DSC_DESC_AES, &vidDescId,NULL);
		DI_DSC_Open(0, DI_DSC_DESC_AES, &audDescId,NULL);

		DI_DSC_SetPid(vidDescId, videoPid);
//IV set
		DI_DSC_SetKey(vidDescId,
					DI_DSC_DESC_ODD,
					VidOddControlWord,
					key_length,
					iv,
					sizeof(iv));
		DI_DSC_SetKey(vidDescId,
					DI_DSC_DESC_EVEN,
					VidEvenControlWord,
					key_length,
					NULL,
					0);

		DI_DSC_SetPid(audDescId, audioPid);
		DI_DSC_SetKey(audDescId,
					DI_DSC_DESC_ODD,
					AudOddControlWord,
					key_length,
					NULL,
					0);
		DI_DSC_SetKey(audDescId,
					DI_DSC_DESC_EVEN,
					AudEvenControlWord,
					key_length,
					NULL,
					0);

		iResult = CMD_OK;
	}
//20090703 :: POOH ::
//
//	Stream name : Windmills-output-TvDefaultProfile-topped.ts
//
//	PID 33 (video+PCR)
//	Odd key: 5208542d2abe76d1d34cf21c413ca1c8
//	Even key: 58e64c5e0fa0ba282bf18ab900444341
//	IV: 959a49a115fc01032c6bb15138267ac1
//
//	PID 34 (audio)
//	Odd key: 08cdaa03c935a21dc12169ce15b87e8e
//	Even key: 1caab51e7e589fa675bd080ec86b9830
//	IV: 92b9992238d5da4cd45b7b3125932395
	else if (CMD_IS("aes2"))
	{
		HUINT32 vidDescId;
		HUINT32 audDescId;

		HUINT16 videoPid = 33;
		HUINT16 audioPid = 34;

		HUINT32 key_length = 16;
		unsigned char	VidEvenControlWord[] = {
							0x58,0xe6,0x4c,0x5e,0x0f,0xa0,0xba,0x28,0x2b,0xf1,0x8a,0xb9,0x00,0x44,0x43,0x41};
		unsigned char VidOddControlWord[] = {
							0x52,0x08,0x54,0x2d,0x2a,0xbe,0x76,0xd1,0xd3,0x4c,0xf2,0x1c,0x41,0x3c,0xa1,0xc8};
		unsigned char AudEvenControlWord[] = {
							0x1c,0xaa,0xb5,0x1e,0x7e,0x58,0x9f,0xa6,0x75,0xbd,0x08,0x0e,0xc8,0x6b,0x98,0x30};
		unsigned char AudOddControlWord[] = {
							0x08,0xcd,0xaa,0x03,0xc9,0x35,0xa2,0x1d,0xc1,0x21,0x69,0xce,0x15,0xb8,0x7e,0x8e};

		unsigned char video_iv[]={0x95,0x9a,0x49,0xa1,0x15,0xfc,0x01,0x03,0x2c,0x6b,0xb1,0x51,0x38,0x26,0x7a,0xc1};
		unsigned char audio_iv[]={0x92,0xb9,0x99,0x22,0x38,0xd5,0xda,0x4c,0xd4,0x5b,0x7b,0x31,0x25,0x93,0x23,0x95};

		DI_DSC_Open(0, DI_DSC_DESC_AES, &vidDescId,NULL);
		DI_DSC_Open(0, DI_DSC_DESC_AES, &audDescId,NULL);

		DI_DSC_SetPid(vidDescId, videoPid);
//IV set
		DI_DSC_SetKey(vidDescId,
					DI_DSC_DESC_ODD,
					VidOddControlWord,
					key_length,
					video_iv,
					sizeof(video_iv));
		DI_DSC_SetKey(vidDescId,
					DI_DSC_DESC_EVEN,
					VidEvenControlWord,
					key_length,
					NULL,
					0);

		DI_DSC_SetPid(audDescId, audioPid);
//IV set
		DI_DSC_SetKey(audDescId,
					DI_DSC_DESC_ODD,
					AudOddControlWord,
					key_length,
					audio_iv,
					sizeof(audio_iv));

		DI_DSC_SetKey(audDescId,
					DI_DSC_DESC_EVEN,
					AudEvenControlWord,
					key_length,
					NULL,
					0);

		iResult = CMD_OK;
	}
//20090703 :: POOH ::
//
//	Stream name : Windmills-output-TvDefaultProfile-1key.ts
//
//	Encrypted PIDs: 33,34
//	Key: 0bca73ecde43b23cfdacbcce20351426
//	IV: fea5f422a970a5448f927dc0ef161093

	else if (CMD_IS("aes3"))
	{
		HUINT32 vidDescId;
		HUINT32 audDescId;

		HUINT16 videoPid = 33;
		HUINT16 audioPid = 34;

		HUINT32 key_length = 16;
		unsigned char	VidEvenControlWord[] = {
							0x0b,0xca,0x73,0xec,0xde,0x43,0xb2,0x3c,0xfd,0xac,0xbc,0xce,0x20,0x35,0x14,0x26};
		unsigned char VidOddControlWord[] = {
							0x0b,0xca,0x73,0xec,0xde,0x43,0xb2,0x3c,0xfd,0xac,0xbc,0xce,0x20,0x35,0x14,0x26};
		unsigned char AudEvenControlWord[] = {
							0x0b,0xca,0x73,0xec,0xde,0x43,0xb2,0x3c,0xfd,0xac,0xbc,0xce,0x20,0x35,0x14,0x26};
		unsigned char AudOddControlWord[] = {
							0x0b,0xca,0x73,0xec,0xde,0x43,0xb2,0x3c,0xfd,0xac,0xbc,0xce,0x20,0x35,0x14,0x26};

		unsigned char iv[]={0xfe,0xa5,0xf4,0x22,0xa9,0x70,0xa5,0x44,0x8f,0x92,0x7d,0xc0,0xef,0x16,0x10,0x93};

		DI_DSC_Open(0, DI_DSC_DESC_AES, &vidDescId,NULL);
		DI_DSC_Open(0, DI_DSC_DESC_AES, &audDescId,NULL);

		DI_DSC_SetPid(vidDescId, videoPid);
//IV set
		DI_DSC_SetKey(vidDescId,
					DI_DSC_DESC_ODD,
					VidOddControlWord,
					key_length,
					iv,
					sizeof(iv));
		DI_DSC_SetKey(vidDescId,
					DI_DSC_DESC_EVEN,
					VidEvenControlWord,
					key_length,
					NULL,
					0);

		DI_DSC_SetPid(audDescId, audioPid);
		DI_DSC_SetKey(audDescId,
					DI_DSC_DESC_ODD,
					AudOddControlWord,
					key_length,
					iv,
					sizeof(iv));



		DI_DSC_SetKey(audDescId,
					DI_DSC_DESC_EVEN,
					AudEvenControlWord,
					key_length,
					NULL,
					0);

		iResult = CMD_OK;
	}

//20101103
//Stream name : \\stream\stream\functions\descrambler_test\DVL-GEN-LSP-Asset_17_0020-100209-1420-01.ts
//command :: av test custom 0 4384 4386 4384 mpeg2 mpeg

	else if (CMD_IS("aes4"))
	{
		HUINT32 vidDescId;
		HUINT32 audDescId;

		HUINT16 videoPid = 0x1120;
		HUINT16 audioPid = 0x1122;

		HUINT32 key_length = 16;
		unsigned char	VidEvenControlWord[] = {
						0xdc, 0x4a, 0x3e, 0x15, 0x28, 0x18, 0xfe, 0x95,
						0xa7, 0xdf, 0x23, 0x1, 0x17, 0xb0, 0x83, 0xa1	};
		unsigned char VidOddControlWord[] = {
						0xdc, 0x4a, 0x3e, 0x15, 0x28, 0x18, 0xfe, 0x95,
						0xa7, 0xdf, 0x23, 0x1, 0x17, 0xb0, 0x83, 0xa1	};
		unsigned char AudEvenControlWord[] = {
						0xdc, 0x4a, 0x3e, 0x15, 0x28, 0x18, 0xfe, 0x95,
						0xa7, 0xdf, 0x23, 0x1, 0x17, 0xb0, 0x83, 0xa1	};
		unsigned char AudOddControlWord[] = {
						0xdc, 0x4a, 0x3e, 0x15, 0x28, 0x18, 0xfe, 0x95,
						0xa7, 0xdf, 0x23, 0x1, 0x17, 0xb0, 0x83, 0xa1	};

		unsigned char iv[]={0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0};

		DI_DSC_Open(0, DI_DSC_DESC_AES, &vidDescId,NULL);
		DI_DSC_Open(0, DI_DSC_DESC_AES, &audDescId,NULL);

		DI_DSC_SetPid(vidDescId, videoPid);
//IV set
		DI_DSC_SetKey(vidDescId,
					DI_DSC_DESC_ODD,
					VidOddControlWord,
					key_length,
					iv,
					sizeof(iv));
		DI_DSC_SetKey(vidDescId,
					DI_DSC_DESC_EVEN,
					VidEvenControlWord,
					key_length,
					NULL,
					0);

		DI_DSC_SetPid(audDescId, audioPid);
		DI_DSC_SetKey(audDescId,
					DI_DSC_DESC_ODD,
					AudOddControlWord,
					key_length,
					NULL,
					0);
		DI_DSC_SetKey(audDescId,
					DI_DSC_DESC_EVEN,
					AudEvenControlWord,
					key_length,
					NULL,
					0);

		iResult = CMD_OK;
	}

//20090703 :: POOH :: single fixed key test
//					only video descramble.
//					i don't know audio key value.
//
//	Stream name : \\stream\stream\functions\descrambler_test\spiderman_3desecb_cts.ts
//command :: av test custom 0 17 20 17 mpeg2 ac3
	else if (CMD_IS("3des"))
	{
		HUINT32 vidDescId;
		HUINT32 audDescId;

		HUINT16 videoPid = 0x11;
		HUINT16 audioPid = 0x14;

		HUINT32 key_length = 16;
		unsigned char   VidEvenControlWord[] = {
								  0x2e, 0xf6, 0xb6, 0xcc, 0x5b, 0x6c, 0x86, 0xf7,
								  0x92, 0xa2, 0x48, 0x70, 0xac, 0xd9, 0x46, 0x73  };
		unsigned char VidOddControlWord[] = {
								  0x2e, 0xf6, 0xb6, 0xcc, 0x5b, 0x6c, 0x86, 0xf7,
								  0x92, 0xa2, 0x48, 0x70, 0xac, 0xd9, 0x46, 0x73  };
		unsigned char AudEvenControlWord[] = {
								  0x2e, 0xf6, 0xb6, 0xcc, 0x5b, 0x6c, 0x86, 0xf7,
								  0x92, 0xa2, 0x48, 0x70, 0xac, 0xd9, 0x46, 0x73  };
		unsigned char AudOddControlWord[] = {
								  0x2e, 0xf6, 0xb6, 0xcc, 0x5b, 0x6c, 0x86, 0xf7,
								  0x92, 0xa2, 0x48, 0x70, 0xac, 0xd9, 0x46, 0x73  };

		DI_DSC_Open(0, DI_DSC_DESC_3DESABA, &vidDescId,NULL);
		DI_DSC_Open(0, DI_DSC_DESC_3DESABA, &audDescId,NULL);

		DI_DSC_SetPid(vidDescId, videoPid);
		DI_DSC_SetKey(vidDescId,
					DI_DSC_DESC_ODD,
					VidOddControlWord,
					key_length,
					NULL,
					0);
		DI_DSC_SetKey(vidDescId,
					DI_DSC_DESC_EVEN,
					VidEvenControlWord,
					key_length,
					NULL,
					0);

		DI_DSC_SetPid(audDescId, audioPid);
		DI_DSC_SetKey(audDescId,
					DI_DSC_DESC_ODD,
					AudOddControlWord,
					key_length,
					NULL,
					0);
		DI_DSC_SetKey(audDescId,
					DI_DSC_DESC_EVEN,
					AudEvenControlWord,
					key_length,
					NULL,
					0);

		iResult = CMD_OK;
	}

//20090703 :: POOH ::
//
//	Stream name : DT21_719M.trp
/*
	Multi2 scramble test

	algorithm = NEXUS_SecurityAlgorithm_eMulti2;
	algorithmVar = NEXUS_SecurityAlgorithmVariant_eCbc;

	multi2_sys_key = {
		0x36, 0x31, 0x04, 0x66, 0x4b, 0x17, 0xea, 0x5c,
		0x32, 0xdf, 0x9c, 0xf5, 0xc4, 0xc3, 0x6c, 0x1b,
		0xec, 0x99, 0x39, 0x21, 0x68, 0x9d, 0x4b, 0xb7,
		0xb7, 0x4e, 0x40, 0x84, 0x0d, 0x2e, 0x7d, 0x98 }

	Initial_Vector = {0xfe, 0x27, 0x19, 0x99, 0x19, 0x69, 0x09, 0x11}

	EvenControlWord = {0x1f, 0x05, 0x5b, 0x1a, 0xb2, 0x81, 0x76, 0xb6}
	OddControlWord = {0x75, 0x26, 0x17, 0xdc, 0xc8, 0xac, 0xfc, 0x9b}
	*/
	else if (CMD_IS("multi2"))
	{
		HUINT32 ulKeyHandle;
		DI_DSC_EncryptionSettings encryptionSetting;
		unsigned char multi2SysKey[] = {0x36, 0x31, 0x04, 0x66, 0x4b, 0x17, 0xea, 0x5c,
			0x32, 0xdf, 0x9c, 0xf5, 0xc4, 0xc3, 0x6c, 0x1b,
			0xec, 0x99, 0x39, 0x21, 0x68, 0x9d, 0x4b, 0xb7,
			0xb7, 0x4e, 0x40, 0x84, 0x0d, 0x2e, 0x7d, 0x98 };

		if(!READABLE_IN_HEX(HWTEST_PARAM, ulKeyHandle))
		{
			HUINT32 vidDescId;
			HUINT32 audDescId;

			HUINT16 videoPid = 256;
			HUINT16 audioPid = 272;

			//DI_DSC_EncryptionSettings encryptionSetting;

			HUINT32 key_length = 8;
			unsigned char	VidEvenControlWord[] = {0x1f, 0x05, 0x5b, 0x1a, 0xb2, 0x81, 0x76, 0xb6};
			unsigned char VidOddControlWord[] = {0x75, 0x26, 0x17, 0xdc, 0xc8, 0xac, 0xfc, 0x9b};
			unsigned char AudEvenControlWord[] = {0x1f, 0x05, 0x5b, 0x1a, 0xb2, 0x81, 0x76, 0xb6};
			unsigned char AudOddControlWord[] = {0x75, 0x26, 0x17, 0xdc, 0xc8, 0xac, 0xfc, 0x9b};

			unsigned char iv[]={0xfe, 0x27, 0x19, 0x99, 0x19, 0x69, 0x09, 0x11};
			encryptionSetting.algorithmVar = DI_DSC_DESC_AlgorithmVariant_eCbc;
			//encryptionSetting.algorithmVar = DI_DSC_DESC_AlgorithmVariant_eEcb;

			VK_memcpy(encryptionSetting.multi2SysKey, multi2SysKey, 32);
			encryptionSetting.multi2KeySelect = 0;
			encryptionSetting.multi2Rounds = 4;

			encryptionSetting.bIsUsed = TRUE;
			DI_DSC_Open(0, DI_DSC_DESC_MULTI2, &vidDescId,&encryptionSetting);

			encryptionSetting.bIsUsed = FALSE;
			DI_DSC_Open(0, DI_DSC_DESC_MULTI2, &audDescId,&encryptionSetting);

			DI_DSC_SetPid(vidDescId, videoPid);
			DI_DSC_SetKey(vidDescId,
					DI_DSC_DESC_ODD,
					VidOddControlWord,
					key_length,
					NULL,
					0);

			DI_DSC_SetKey(vidDescId,
					DI_DSC_DESC_EVEN,
					VidEvenControlWord,
					key_length,
					NULL,
					0);

			DI_DSC_SetKey(vidDescId,
					DI_DSC_DESC_ODD,
					NULL,
					0,
					iv,
					sizeof(iv));

			DI_DSC_SetPid(audDescId, audioPid);
			DI_DSC_SetKey(audDescId,
					DI_DSC_DESC_ODD,
					AudOddControlWord,
					key_length,
					NULL,
					0);


			DI_DSC_SetKey(audDescId,
					DI_DSC_DESC_EVEN,
					AudEvenControlWord,
					key_length,
					NULL,
					0);
		}
		else
		{
			VK_printf("[%s,%d] SET DI_DSC_SetMulti2SysKey()!, ulKeyHandle: 0x%x\n", __FUNCTION__, __LINE__, ulKeyHandle);
			VK_memcpy(encryptionSetting.multi2SysKey, multi2SysKey, 32);
			encryptionSetting.multi2KeySelect = 0;
			encryptionSetting.multi2Rounds = 4;
			DI_DSC_SetMulti2SysKey(ulKeyHandle, &encryptionSetting);
		}
		iResult = CMD_OK;
	}
	else if (CMD_IS("des"))
	{
		HUINT32 vidDescId;
		HUINT32 audDescId;

		HUINT16 videoPid = 0x11;
		HUINT16 audioPid = 0x14;
#if 0
		HUINT32 key_length = 8;
		unsigned char   VidEvenControlWord[] = {
								  0x2e, 0xf6, 0xb6, 0xcc, 0x5b, 0x6c, 0x86, 0xf7};
		unsigned char VidOddControlWord[] = {
								  0x2e, 0xf6, 0xb6, 0xcc, 0x5b, 0x6c, 0x86, 0xf7};
		unsigned char AudEvenControlWord[] = {
								  0x2e, 0xf6, 0xb6, 0xcc, 0x5b, 0x6c, 0x86, 0xf7};
		unsigned char AudOddControlWord[] = {
								  0x2e, 0xf6, 0xb6, 0xcc, 0x5b, 0x6c, 0x86, 0xf7};
#else
		HUINT32 key_length = 16;
		unsigned char   VidEvenControlWord[] = {
								  0x2e, 0xf6, 0xb6, 0xcc, 0x5b, 0x6c, 0x86, 0xf7,
								  0x92, 0xa2, 0x48, 0x70, 0xac, 0xd9, 0x46, 0x73  };
		unsigned char VidOddControlWord[] = {
								  0x2e, 0xf6, 0xb6, 0xcc, 0x5b, 0x6c, 0x86, 0xf7,
								  0x92, 0xa2, 0x48, 0x70, 0xac, 0xd9, 0x46, 0x73  };
		unsigned char AudEvenControlWord[] = {
								  0x2e, 0xf6, 0xb6, 0xcc, 0x5b, 0x6c, 0x86, 0xf7,
								  0x92, 0xa2, 0x48, 0x70, 0xac, 0xd9, 0x46, 0x73  };
		unsigned char AudOddControlWord[] = {
								  0x2e, 0xf6, 0xb6, 0xcc, 0x5b, 0x6c, 0x86, 0xf7,
								  0x92, 0xa2, 0x48, 0x70, 0xac, 0xd9, 0x46, 0x73  };
#endif
		DI_DSC_Open(0, DI_DSC_DESC_DES_CLEARMODE, &vidDescId,NULL);
		DI_DSC_Open(0, DI_DSC_DESC_DES_CLEARMODE, &audDescId,NULL);

		DI_DSC_SetPid(vidDescId, videoPid);
		DI_DSC_SetKey(vidDescId,
					DI_DSC_DESC_ODD,
					VidOddControlWord,
					key_length,
					NULL,
					0);
		DI_DSC_SetKey(vidDescId,
					DI_DSC_DESC_EVEN,
					VidEvenControlWord,
					key_length,
					NULL,
					0);

		DI_DSC_SetPid(audDescId, audioPid);
		DI_DSC_SetKey(audDescId,
					DI_DSC_DESC_ODD,
					AudOddControlWord,
					key_length,
					NULL,
					0);
		DI_DSC_SetKey(audDescId,
					DI_DSC_DESC_EVEN,
					AudEvenControlWord,
					key_length,
					NULL,
					0);

		iResult = CMD_OK;
	}
	else
	{
		VK_printf("ERR! Bad Comman Input. \n");
	}

	return iResult;
}

#ifdef	CMD_DSC_DEBUG
#include "taskdef.h"
#include "di_video.h"

#define DSC_MIN_PID_NUMBER	2	//Maybe.......
#define DSC_MAX_PID_NUMBER	4
#define DSC_CTRL_WORD_SIZE	8	/* bytes */

#if defined(CONFIG_GRAP_KEY_TO_USB)
static char USB_DSC_TEMP_FILE[64] = "/media/drive1/dsctemp.dat";
static char USB_DSC_SAVE_FILE[64] = "/media/drive1/dscsave.dat";
static char USB_DSC_TEST_FILE[64] = "/media/drive1/dsctest.dat";
static char USB_DSC_CWPK_FILE[64] = "/media/drive1/dsccwpk.dat";
static char USB_DSC_SCWPK_FILE[64] = "/media/drive1/dscscwpk.dat";
static char USB_DSC_TCWPK_FILE[64] = "/media/drive1/dsccwpktest.dat";
#else
static char USB_DSC_TEMP_FILE[64] = "/var/lib/humaxtv/dsctemp.dat";
static char USB_DSC_SAVE_FILE[64] = "/var/lib/humaxtv/dscsave.dat";
static char USB_DSC_TEST_FILE[64] = "/var/lib/humaxtv/dsctest.dat";
static char USB_DSC_CWPK_FILE[64] = "/var/lib/humaxtv/dsccwpk.dat";
static char USB_DSC_SCWPK_FILE[64] = "/var/lib/humaxtv/dscscwpk.dat";
static char USB_DSC_TCWPK_FILE[64] = "/var/lib/humaxtv/dsccwpktest.dat";
#endif

static unsigned long Dsc_TestID = 0;

FILE * tempFileHandle=NULL;
FILE * saveFileHandle=NULL;
FILE * testFileHandle=NULL;
FILE * cwpkFileHandle=NULL;
FILE * scwpkFileHandle=NULL;
FILE * testcwpkFileHandle=NULL;

typedef struct tagDSC_TEST_INFO
{
	unsigned long			bAllocated;
	unsigned int			ulDemuxId;
	unsigned short			pid;
	unsigned int			DescId;
} DSC_TEST_INFO;

unsigned char _startflag = 0;

static void Dsc_TestTask(void *pvParam)
{
	int totalLineNum = 0;
	int lineNum=0;
	int currentLineNumber = 1;
	char lineBuf[256];
	int *line_index;
	char lineStartIndex[9];
	char keyType[5];

	unsigned int	ulTempDscHandle;
	unsigned int ulDemuxId=0;
	unsigned int usPid=0;
	unsigned int	pcr=0;
	DI_DSC_DESC_ODD_EVEN parity;
	unsigned int i=0;

	static int dscHandleNumber[2]={0,0};
	static unsigned int totalDscHandleNumber=0;

	DSC_TEST_INFO *dscHandle;

	unsigned int tempDataBuffer;

	unsigned char *pKeyData;
	unsigned int key_length = 0;
    unsigned char key_length_ch =0x00;

	unsigned int currentPcrValue=0;
	unsigned int nextPcrValue=0;

	unsigned int lastPcrValue=0;

	static unsigned int PCR_START_FLAG=0;

	DI_ERR_CODE	err = 0;
	HUINT16 tmpPid[2] = {0,};
	unsigned char data[80] = {0};


	char clineBuf[256];
	unsigned int ctotalLineNum = 0;
	int *cline_index = NULL;
	unsigned int clineNum=0;

	char klineBuf[256];
	int ktotalLineNum = 0;
	int *kline_index = NULL;
	int klineNum=0;

	char plineBuf[256];
	int ptotalLineNum = 0;
	int *pline_index = NULL;
	int plineNum=0;


	HUINT16 SavedPID[200] = {0,};
	int		SavedPos[200] = {0,};

	unsigned int numPID = 0,check = 0,count = 0,index = 0,fcnt = 0,k = 0,lcnt = 0,tcnt = 0;
	int		dsize = 0;
	int		r1 = 0, r2 = 0;
	unsigned int odize = 0;

	unsigned int	scwpk[9] ={0,};
	unsigned int	scwpkpcr[9] ={0,};
	unsigned int	tcwpk[80] ={0,};
	unsigned char	cwpkdata[16] = {0,};
	unsigned char   cwpkUsed = 0;
	unsigned int	scwpkpcrvalue = 0;
	unsigned short 	cwpkPid = 0;
	unsigned char	cwpklen = 0;

	BSTD_UNUSED(pvParam);

	/*
		현재 채널의 PID 정보 및 Service ID를 가져와서 파일에 해당 되는 곳에 match 시켜야 함.
	*/
	err = DI_DEMUX_GetInfo(0,&tmpPid[0],&tmpPid[1]);
	if(err != 0)
	{
		return;
	}

	DRV_DSC_ClearWithOnlyPid(tmpPid[0]);
	DRV_DSC_CloseWithOnlyPid(tmpPid[0]);

	DRV_DSC_ClearWithOnlyPid(tmpPid[1]);
 	DRV_DSC_CloseWithOnlyPid(tmpPid[1]);

 	VK_printf("=== Dsc_TestTask Start !!! ===\r\n");
	if ( (saveFileHandle = fopen(USB_DSC_SAVE_FILE, "r")) == NULL) {

		VK_printf("Failed to create(open) %s\n", USB_DSC_SAVE_FILE);
		return ;
	}

	rewind(saveFileHandle);

	while(fgets(clineBuf, 255, saveFileHandle) != NULL)
	{
		if (clineBuf[VK_strlen(clineBuf)-1] == '\n')
			ctotalLineNum++;
	}

	cline_index = (int *)DD_MEM_Alloc(sizeof(int) * ctotalLineNum +1);
	if(cline_index == NULL)
	{
		return;
	}

	rewind(saveFileHandle);

	cline_index[0] = 0;

	while(fgets(clineBuf, 255, saveFileHandle) != NULL)
	{
		if (clineBuf[VK_strlen(clineBuf)-1] == '\n' && clineNum+1 < ctotalLineNum)
		{
			cline_index[clineNum+1] = ftell(saveFileHandle);
			clineNum++;
		}
	}

	rewind(saveFileHandle);

	while( 0 < fscanf( saveFileHandle, "%s %X %X", lineStartIndex, &ulDemuxId, &usPid))
	{
		if( VK_strcmp(lineStartIndex, "PID") == 0 )
		{
			if(numPID >= 200)
			{
				break;
			}

			check = 0;
			if(numPID > 0)
			{
				for(count = 0; count < numPID; count++)
				{
					if(SavedPID[count] == usPid)
					{
						check++;
					}
				}

				if(check == 0)
				{
					SavedPID[numPID] = usPid;
					SavedPos[numPID] = ftell(saveFileHandle)+0x02;
					numPID++;
				}

			}
			else
			{

				SavedPID[numPID] = usPid;
				SavedPos[numPID] = ftell(saveFileHandle)+0x02;
				numPID++;
			}

		}
		VK_memset(lineStartIndex,0,9);

	}

	check = 0;
	for(count = 0; count < numPID; count++)
	{
		if((SavedPID[count] == tmpPid[0])||(SavedPID[count] == tmpPid[1]))
		{
			check++;
			if(check == 2)
			{
				break;
			}
		}
	}

	if(check != 2)
	{
		fclose(saveFileHandle);
		saveFileHandle = NULL;
		return;
	}

	for( k = 0; k < ctotalLineNum; k++)
	{

		if(SavedPos[count] == cline_index[k])
		{
			break;
		}
	}

	if((k > 0)&&(k != ctotalLineNum))
	{
		fcnt = k;
	}

	if(count == (numPID - 1))
	{
		lcnt  = ctotalLineNum;
	}
	else
	{
		for( k = 0; k < ctotalLineNum; k++)
		{
			if(SavedPos[count + 1] == cline_index[k])
			{
				break;
			}
		}

		if((k > 0)&&(k != ctotalLineNum))
		{
			lcnt = k;
			lcnt -= 2;
		}
	}

	if((fcnt >= 2)&&(lcnt > fcnt))
	{
		if ( (testFileHandle = fopen(USB_DSC_TEST_FILE, "w")) == NULL)
		{

			VK_printf("Failed to create(open) %s\n", USB_DSC_TEST_FILE);

			fclose(saveFileHandle);
			saveFileHandle = NULL;
			testFileHandle = NULL;
			return ;
		}

		tcnt = (lcnt - fcnt) + 2;
		fcnt -= 2;

		for(k = 0; k < tcnt; k++)
		{
			dsize = 0;

			if(k != (tcnt - 1))
			{
				dsize = cline_index[fcnt + k + 1] - cline_index[fcnt + k];
			}
			else
			{
				VK_memset(lineStartIndex,0,9);
				VK_memset(keyType,0,5);
				fseek(saveFileHandle, cline_index[fcnt + k], SEEK_SET);
				r1 = fscanf( saveFileHandle, "%s %X %X %s", lineStartIndex, &ulDemuxId, &usPid,keyType);
				if(VK_strcmp(keyType,"EVEN"))
					dsize = 48;
				else if(VK_strcmp(keyType,"ODD"))
					dsize = 47;
				else
					dsize = 0;

			}

			if(dsize > 0)
			{
				VK_memset(data,0,80);
				if(data != NULL)
				{
					fseek(saveFileHandle, cline_index[fcnt + k], SEEK_SET);
					fread(data,1,dsize,saveFileHandle);
					fwrite(data,1,dsize,testFileHandle);
				}
			}

			VK_memset(lineStartIndex,0,9);

		}

		fclose(saveFileHandle);
		fclose(testFileHandle);
		saveFileHandle = NULL;
		testFileHandle = NULL;

		DD_MEM_Free(cline_index);
		cline_index = NULL;

	}
	else
	{
		fclose(saveFileHandle);
		saveFileHandle = NULL;

		DD_MEM_Free(cline_index);
		cline_index = NULL;
		return;
	}

	VK_memset(lineStartIndex,0,9);
	VK_memset(keyType,0,5);

	ctotalLineNum = 0;
	clineNum = 0;
	ptotalLineNum = 0;
	plineNum = 0;

	if ( (testFileHandle = fopen(USB_DSC_TEST_FILE, "r")) != NULL)
	{
		rewind(testFileHandle);
		while(fgets(plineBuf, 255, testFileHandle) != NULL)
		{
			if (plineBuf[VK_strlen(plineBuf)-1] == '\n')
				ptotalLineNum++;
		}

		pline_index = (int *)DD_MEM_Alloc(sizeof(int) * ptotalLineNum +1);
		if(pline_index == NULL)
		{

			fclose(testFileHandle);
			testFileHandle = NULL;
			return;
		}

		rewind(testFileHandle);

		pline_index[0] = 0;

		while(fgets(plineBuf, 255, testFileHandle) != NULL)
		{
			if (plineBuf[VK_strlen(plineBuf)-1] == '\n' && plineNum+1 < ptotalLineNum)
			{
				pline_index[plineNum+1] = ftell(testFileHandle);
				plineNum++;
			}
		}
	}
	else
	{
		return;
	}


	if ( (scwpkFileHandle = fopen(USB_DSC_SCWPK_FILE, "r")) != NULL)
	{
		rewind(scwpkFileHandle);

		while(fgets(klineBuf, 255, scwpkFileHandle) != NULL)
		{
			if (klineBuf[VK_strlen(klineBuf)-1] == '\n')
				ktotalLineNum++;
		}

		kline_index = (int *)DD_MEM_Alloc(sizeof(int) * ktotalLineNum +1);
		if(kline_index == NULL)
		{
			fclose(scwpkFileHandle);
			scwpkFileHandle = NULL;
			return;
		}

		rewind(scwpkFileHandle);

		kline_index[0] = 0;

		while(fgets(klineBuf, 255, scwpkFileHandle) != NULL)
		{
			if (klineBuf[VK_strlen(klineBuf)-1] == '\n' && klineNum+1 < ktotalLineNum)
			{
				kline_index[klineNum+1] = ftell(scwpkFileHandle);
				klineNum++;
			}
		}

		rewind(testFileHandle);
		if ((testcwpkFileHandle = fopen(USB_DSC_TCWPK_FILE, "w+")) != NULL)
		{
			for(k = 0; k< ptotalLineNum; k++)
			{
				fseek(testFileHandle,pline_index[k],SEEK_SET);
				r1 = fscanf( testFileHandle, "%s %X %X", lineStartIndex, &ulDemuxId, &usPid);

				if(	VK_strcmp(	lineStartIndex,	"PID") != 0)
				{
					for(count = 0; count < ktotalLineNum; count++)
					{
						fseek(scwpkFileHandle,kline_index[count],SEEK_SET);
						r2 = fscanf(scwpkFileHandle, "%s %X %s %X",scwpk,&cwpkPid,scwpkpcr,&cwpklen);

						if((cwpkPid == usPid)&&(VK_strcmp(scwpkpcr,lineStartIndex) == 0))
						{
							if(count == 0)
							{
								dsize = kline_index[count + 1] - kline_index[count];
							}

							if(dsize > 0)
							{
								VK_memset(data,0,80);
								if(data != NULL)
								{
									fseek(scwpkFileHandle,kline_index[count],SEEK_SET);
									fread(data,1,dsize,scwpkFileHandle);
									fwrite(data,1,dsize,testcwpkFileHandle);
								}
							}
						}

						VK_memset(scwpk,0,9);
						VK_memset(scwpkpcr,0,9);
					}
				}

				VK_memset(lineStartIndex,0,9);

			}

			fclose(testcwpkFileHandle);
			testcwpkFileHandle = NULL;
			fclose(scwpkFileHandle);
			scwpkFileHandle = NULL;
			fclose(testFileHandle);
			testFileHandle = NULL;

			DD_MEM_Free(pline_index);
			pline_index = NULL;
			DD_MEM_Free(kline_index);
			kline_index = NULL;

		}
		else
		{
			fclose(testcwpkFileHandle);
			testcwpkFileHandle = NULL;

			fclose(scwpkFileHandle);
			scwpkFileHandle = NULL;

			DD_MEM_Free(pline_index);
			pline_index = NULL;
			DD_MEM_Free(kline_index);
			kline_index = NULL;

			return;
		}

	}

	if ( (testFileHandle = fopen(USB_DSC_TEST_FILE, "r")) == NULL)
	{
		VK_printf("Failed to create(open) %s\n", USB_DSC_TEST_FILE);
		return ;
	}

	if ( (testcwpkFileHandle = fopen(USB_DSC_TCWPK_FILE, "r")) == NULL)
	{
		VK_printf("Failed to create(open) %s\n", USB_DSC_TCWPK_FILE);
		testcwpkFileHandle = NULL;
	}
	else
	{

		cwpkUsed = 1;
		plineNum = 0;
		ptotalLineNum = 0;

		fseek(testFileHandle,01,SEEK_END);
		odize = ftell(testFileHandle);

		rewind(testcwpkFileHandle);

		while(fgets(plineBuf, 255, testcwpkFileHandle) != NULL)
		{
			if (plineBuf[VK_strlen(plineBuf)-1] == '\n')
				ptotalLineNum++;
		}

		pline_index = (int *)DD_MEM_Alloc(sizeof(int) * ptotalLineNum +1);
		if(pline_index == NULL)
		{

			fclose(	testFileHandle );
			testFileHandle = NULL;

			fclose(	testcwpkFileHandle );
			testcwpkFileHandle = NULL;

			return;
		}

		rewind(testcwpkFileHandle);

		pline_index[0] = 0;

		while(fgets(plineBuf, 255, testcwpkFileHandle) != NULL)
		{
			if (plineBuf[VK_strlen(plineBuf)-1] == '\n' && plineNum+1 < ptotalLineNum)
			{
				pline_index[plineNum+1] = ftell(testcwpkFileHandle);
				plineNum++;
			}
		}

	}


	rewind(testFileHandle);

	while(fgets(lineBuf, 255, testFileHandle) != NULL)
	{
		if (lineBuf[VK_strlen(lineBuf)-1] == '\n')
			totalLineNum++;
	}

	line_index = (int *)DD_MEM_Alloc(sizeof(int) * totalLineNum +1);
	if(pline_index == NULL)
	{

		fclose( testFileHandle );
		testFileHandle = NULL;

		fclose( testcwpkFileHandle );
		testcwpkFileHandle = NULL;
		DD_MEM_Free(pline_index);
		return;
	}

	rewind(testFileHandle);

	line_index[0] = 0;

	while(fgets(lineBuf, 255, testFileHandle) != NULL)
	{
		if (lineBuf[VK_strlen(lineBuf)-1] == '\n' && lineNum+1 < totalLineNum)
		{
			line_index[lineNum+1] = ftell(testFileHandle);
			lineNum++;
		}
	}

	lineNum = 1;

	rewind(testFileHandle);

	while( 0 < fscanf( testFileHandle, "%s %X %X", lineStartIndex, &ulDemuxId, &usPid))
	{
		if( VK_strcmp(lineStartIndex, "PID") == 0 )
		{
			dscHandleNumber[ulDemuxId]++;
		}
		VK_memset(lineStartIndex,0,9);
	}

	totalDscHandleNumber = dscHandleNumber[0] + dscHandleNumber[1];

	dscHandle = (DSC_TEST_INFO*)DD_MEM_Alloc(totalDscHandleNumber*sizeof(DSC_TEST_INFO));
	VK_memset (dscHandle, 0, totalDscHandleNumber*sizeof(DSC_TEST_INFO) );

	rewind(testFileHandle);

	while( _startflag )
	{

		if(_startflag == 0)
		{
			VK_TASK_Sleep(100);
			break;
		}

		if( currentLineNumber > totalLineNum )
		{
//			VK_printf("\nChange line number %d -> %d\n",currentLineNumber,lineNum);
			currentLineNumber = lineNum;
		}

		fseek(testFileHandle, line_index[currentLineNumber-1], SEEK_SET);

		fscanf( testFileHandle, "%s %X %X", lineStartIndex, &ulDemuxId, &usPid);

		if( (VK_strcmp(lineStartIndex, "PID") == 0) )
		{
			DI_DSC_Open(ulDemuxId, DI_DSC_DESC_DVB, &ulTempDscHandle,NULL);
			DI_DSC_SetPid(ulTempDscHandle, usPid);

			/* Search the freeSlot */
			for ( i=0; i< totalDscHandleNumber; i++)
			{
				if (dscHandle[i].bAllocated == FALSE)
				{
					dscHandle[i].ulDemuxId = ulDemuxId;
					dscHandle[i].pid = usPid;
					dscHandle[i].DescId = ulTempDscHandle;
					dscHandle[i].bAllocated = TRUE;
					break;
				}
			}

			lineNum++;
#ifdef DSC_DEBUG_PRINT
			VK_printf("\nChange line number %d\n",lineNum);
#endif
			currentLineNumber++;
		}
		else
		{
			if(_startflag == 0)
			{
				VK_TASK_Sleep(100);
				break;
			}

			if(PCR_START_FLAG == 1)
			{
				goto JUMP_PCR_CHECK;
			}


			DRV_VIDEO_GetCurrentStc(0, &pcr);

			fseek(testFileHandle, line_index[currentLineNumber-1], SEEK_SET);

			while( 0 < fscanf( testFileHandle, "%s %X %X", lineStartIndex, &ulDemuxId, &usPid))
			{
				currentPcrValue = strtoul(lineStartIndex, NULL, 16);

				if(_startflag == 0)
				{
					VK_TASK_Sleep(100);
					break;
				}

				if( !(currentLineNumber % 10) )
				{
					DRV_VIDEO_GetCurrentStc(0, &pcr);
				}

				if(((currentPcrValue > (pcr + 0x2000000)) || (currentPcrValue < (pcr - 0x2000000))) && ( lastPcrValue))
				{
#ifdef DSC_DEBUG_PRINT
					VK_printf("\nJump pcr timming\n");
					VK_printf("MIN pcr\t\t= 0x%08X\n",(pcr - 0x1000000));
					VK_printf("cur pcr\t\t= 0x%08X\n",currentPcrValue);
					VK_printf("MAX pcr\t\t= 0x%08X\n",(pcr + 0x1000000));
#endif
					lastPcrValue = 0;
					VK_TASK_Sleep(100);
				}
				else if ((currentPcrValue > (pcr + 0x10000000)) || (currentPcrValue < (pcr - 0x10000000)))
				{
#ifdef DSC_DEBUG_PRINT
					VK_printf("\nOther pcr timming\n");
					VK_printf("pcr\t\t= 0x%08X\n",(pcr));
					VK_printf("MIN pcr\t\t= 0x%08X\n",(pcr - 0x10000000));
					VK_printf("cur pcr\t\t= 0x%08X\n",currentPcrValue);
					VK_printf("MAX pcr\t\t= 0x%08X\n",(pcr + 0x10000000));
#endif
					VK_TASK_Sleep(100);
				}

				if (currentLineNumber+1 > totalLineNum)
				{
					currentLineNumber = lineNum;
					fseek(testFileHandle, line_index[currentLineNumber-1], SEEK_SET);
					continue;
				}

				fseek(testFileHandle, line_index[currentLineNumber+1-1], SEEK_SET);
				fscanf( testFileHandle, "%s %X %X", lineStartIndex, &ulDemuxId, &usPid);
				nextPcrValue = strtoul(lineStartIndex, NULL, 16);

				if( (currentPcrValue < pcr) && (pcr < nextPcrValue) )
				{
#ifdef DSC_DEBUG_PRINT
					VK_printf("\nMatch pcr timming\n");
					VK_printf("pcr\t\t= 0x%08X\n",pcr);
					VK_printf("currentPcr\t= 0x%08X\n",currentPcrValue);
					VK_printf("nextPcrValue\t= 0x%08X\n",nextPcrValue);
#endif
					currentLineNumber++;
					break;
				}

				if(currentLineNumber == totalLineNum)
				{
					currentLineNumber = lineNum;
				}
				else
				{
					currentLineNumber++;
				}

				fseek(testFileHandle, line_index[currentLineNumber-1], SEEK_SET);

			}

JUMP_PCR_CHECK:
			if((cwpkUsed == 1)&&(testcwpkFileHandle != NULL))
			{
				fseek(testcwpkFileHandle, pline_index[currentLineNumber-3], SEEK_SET);
				r1 = fscanf(testcwpkFileHandle, "%s %X %s %X",scwpk,&cwpkPid,scwpkpcr,&cwpklen);
				for(i=0; i<cwpklen; i++)
				{
					r2 = fscanf( testcwpkFileHandle, "%X", &(tempDataBuffer));
					cwpkdata[i] = (unsigned char)tempDataBuffer;
				}
			}

			fseek(testFileHandle, line_index[currentLineNumber-1], SEEK_SET);
			fscanf( testFileHandle, "%s %X %X", lineStartIndex, &ulDemuxId, &usPid);

			currentPcrValue = strtoul(lineStartIndex, NULL, 16);
			DRV_VIDEO_GetCurrentStc(0, &pcr);

			if((pcr < currentPcrValue) && (currentPcrValue < (pcr + 0x8000)))		//??????????????????????
			{
#ifdef DSC_DEBUG_PRINT
				VK_printf("[1]\n");
#endif

				fscanf( testFileHandle, "%s", keyType);

				if( (VK_strcmp(keyType, "ODD") == 0) )
				{
					parity = DI_DSC_DESC_ODD;
				}
				else
				{
					parity = DI_DSC_DESC_EVEN;
				}

				fscanf( testFileHandle, "%02x", (const char *)&(key_length_ch));
                if ( key_length_ch == 0x00 )
                {
                    continue;
                }
                key_length = (unsigned int)key_length_ch;
				pKeyData = (HUINT8*)DD_MEM_Alloc(key_length);
				VK_MEM_Memset(pKeyData,0,key_length);

				for(i=0; i<key_length; i++)
				{
					fscanf( testFileHandle, "%X", &(tempDataBuffer));
					pKeyData[i] = (unsigned char)tempDataBuffer;
				}

				/* Search the Slot */
				for ( i=0; i< totalDscHandleNumber; i++)
				{
					if ((dscHandle[i].ulDemuxId == ulDemuxId) && (dscHandle[i].pid == usPid))
					{

						if(cwpkUsed == 1)
						{
							DI_DSC_SetProtectionKey(dscHandle[i].DescId,cwpkdata,cwpklen);
						}


						DI_DSC_SetKey(dscHandle[i].DescId,
									parity,
									pKeyData,
									key_length,
									NULL,
									0);
						break;
					}
				}

				currentLineNumber++;
				lastPcrValue = currentPcrValue;
				PCR_START_FLAG = 1;
			}
			else if(pcr > currentPcrValue)
			{
#ifdef DSC_DEBUG_PRINT
				VK_printf("[2]\n");
#endif
				currentLineNumber++;
				PCR_START_FLAG = 0;
			}
			else
			{
#ifdef DSC_DEBUG_PRINT
				VK_printf("[3]\n");
#endif
				PCR_START_FLAG = 0;
			}

			VK_TASK_Sleep(100);
		}
	}

	if(testFileHandle != NULL)
	{
		fclose(testFileHandle);
		testFileHandle = NULL;
	}

	if(saveFileHandle != NULL)
	{
		fclose(saveFileHandle);
		saveFileHandle = NULL;
	}

	if(testcwpkFileHandle != NULL)
	{
		fclose(testcwpkFileHandle);
		testcwpkFileHandle = NULL;
	}

	for ( i=0; i< totalDscHandleNumber; i++)
	{
		if (dscHandle[i].bAllocated == TRUE)
		{
			DI_DSC_ClearPid(dscHandle[i].DescId, dscHandle[i].pid);
			DI_DSC_Close(dscHandle[i].DescId);
			dscHandle[i].bAllocated = FALSE;
		}
	}

	DD_MEM_Free(dscHandle);
	DD_MEM_Free(line_index);
	line_index = NULL;

	DD_MEM_Free(pline_index);
	pline_index = NULL;

	VK_printf("=== Dsc_TestTask :: END ===\r\n");

	return;
}

unsigned long CMD_DSCTask_Create(const char* name, void (*func)(void *), void *args, int stackSize, int priority)
{
	NEXUS_ThreadSettings settingsThread;
	NEXUS_ThreadHandle	thread;

	NEXUS_Thread_GetDefaultSettings(&settingsThread);
	if(priority >= 0) settingsThread.priority = priority;
	if(stackSize > 0) settingsThread.stackSize = stackSize;

	thread = NEXUS_Thread_Create(name, func, args, &settingsThread);
	if(thread == NULL)
		VK_printf("Task Creation Fail !!!\n");

	return (unsigned long)thread;
}

void CMD_DSCTask_Destroy(unsigned long thread)
{
	NEXUS_Thread_Destroy((NEXUS_ThreadHandle)thread);
}

static int CMD_Word_DSCTest(void *szArgs)
{
	char	 aucStr[20];
	char *szArg;
	unsigned int i=0;
	int currentLineNumber = 1;
	int result  = 0;

	/* get 1st argument */
	szArg = CMD_NextArg((char **)&szArgs);
	if( szArg == NULL )
		return CMD_ERR;

    if ( (VK_strlen(szArg) != 0) && (VK_strlen(szArg)  < 20) )
    {
        VK_strcpy(aucStr, szArg);
    }
    else
    {
		return CMD_ERR;
    }

	CMD_StrToUpper((HUINT8 *)aucStr);

	if( VK_strcmp(aucStr, "START") == 0 )
	{
		if(Dsc_TestID != 0)
			VK_printf("DSCT Test Start Fail:: already be started !!!\r\n");

		DI_VIDEO_Show(0, 1);

		Dsc_TestID  = CMD_DSCTask_Create("DSCT",Dsc_TestTask,  NULL, 64*1024,USER_PRIORITY12);

		_startflag = 1;
		VK_printf("DSCT Test Start!!!\r\n");

	}
	else if ( VK_strcmp(aucStr, "STOP") == 0 )
	{

		if(Dsc_TestID == 0)
			VK_printf("DSCT Test Stop Fail:: already be stopted !!!\r\n");

		_startflag = 0;

		CMD_DSCTask_Destroy(Dsc_TestID);

		Dsc_TestID = 0;

		VK_printf("DSCT Test END!!!\r\n");

	}

	return CMD_OK;
}

static int CMD_Word_DSCRec(void *szArgs)
{
	char	 aucStr[20];
	char *szArg;
	unsigned int i=0;
	int currentLineNumber = 1;

	/* get 1st argument */
	szArg = CMD_NextArg((char **)&szArgs);
	if( szArg == NULL )
		return CMD_ERR;

    if ( (VK_strlen(szArg) != 0) && (VK_strlen(szArg)  < 20) )
    {
        VK_strcpy(aucStr, szArg);
    }
    else
    {
		return CMD_ERR;
    }

	CMD_StrToUpper((HUINT8 *)aucStr);

	if( VK_strcmp(aucStr, "START") == 0 )
	{

		if ( (tempFileHandle = fopen(USB_DSC_TEMP_FILE, "w+")) == NULL) {

			  VK_printf("Failed to create(open) %s\n", USB_DSC_TEMP_FILE);
			  VK_printf("Check your USB memory stick\n");
			  return CMD_ERR;
		}

		if ((cwpkFileHandle = fopen(USB_DSC_CWPK_FILE, "w+")) == NULL)
		{

			  fclose(tempFileHandle);
			  tempFileHandle=NULL;

			  VK_printf("Failed to create(open) %s\n", USB_DSC_CWPK_FILE);
			  VK_printf("Check your USB memory stick\n");
			  return CMD_ERR;
		}

		VK_printf("Start DSC recording!!!\n");


	}
	else if ( VK_strcmp(aucStr, "STOP") == 0 )
	{
		int totalLineNum = 0;
		int lineNum=0;
		char lineBuf[256];
		int *line_index;

		char keyType[5];

		char lineStartIndex[9];
		unsigned int ulDemuxId=0;
		unsigned int usPid=0;

		unsigned int tempDataBuffer=0;

		unsigned char *pKeyData;
		unsigned int key_length = 0;
        unsigned char key_length_ch = 0x00;

		unsigned int currentPcrValue=0;
		unsigned int lastPcrValue=0;

		static unsigned int PCR_REC_START_FLAG=0;
//		static unsigned int PCR_REC_END_FLAG=0;


		unsigned short SavedPID[200] = {0,};

		unsigned int count = 0,numPID = 0,index = 0,check = 0;

		unsigned short cwpkPid = 0;

		unsigned int 	scwpkpcr[9] ={0,};
		unsigned int 	scwpk[9] ={0,};
		unsigned int 	tcwpk[80] ={0,};
		unsigned char 	cwpklen = 0;

		int totalLineNum1 = 0;
		int lineNum1=0;
		char lineBuf1[256];
		int *line_index1;
		unsigned int diff = 0;

		VK_printf("STOP DSC recording!!!\n");

		 if ( (saveFileHandle = fopen(USB_DSC_SAVE_FILE, "w+")) == NULL)
		 {
			   VK_printf("Failed to create(open) %s\n", USB_DSC_SAVE_FILE);
			   return CMD_ERR;
		 }

		rewind(tempFileHandle);

		while(fgets(lineBuf, 255, tempFileHandle) != NULL)
		{
			if (lineBuf[VK_strlen(lineBuf)-1] == '\n')
				totalLineNum++;
		}

		rewind(tempFileHandle);

		currentLineNumber = 0;
		while( 0 < fscanf( tempFileHandle, "%s %X %X", lineStartIndex, &ulDemuxId, &usPid))
		{
			if( VK_strcmp(lineStartIndex, "PID") == 0 )
			{
				if(numPID >= 200)
				{
					break;
				}

				check = 0;
				if(numPID > 0)
				{
					for(count = 0; count < numPID; count++)
					{

						if(SavedPID[count] == usPid)
						{
							check++;
						}
					}

					if(check == 0)
					{
						SavedPID[numPID] = usPid;
						numPID++;
					}

				}
				else
				{

					SavedPID[numPID] = usPid;
					numPID++;
				}

			}

			VK_memset(lineStartIndex,0,9);
		}


		if(numPID < DSC_MIN_PID_NUMBER)
		{
			VK_printf("Failed to record.\n");

			fclose(tempFileHandle);
			tempFileHandle=NULL;

			fclose(saveFileHandle);
			saveFileHandle=NULL;

			if(cwpkFileHandle != NULL)
			{
				fclose(cwpkFileHandle);
				cwpkFileHandle=NULL;
			}

			return CMD_ERR;
		}


		for(count = 0; count < numPID; count += 2)
		{
			rewind(tempFileHandle);
			lastPcrValue = 0;
			PCR_REC_START_FLAG = 0;

			index = count;

			VK_fprintf(saveFileHandle, "PID %01X %04X \n",0, SavedPID[index]);
			VK_fprintf(saveFileHandle, "PID %01X %04X \n",0, SavedPID[index+1]);

			while( 0 < fscanf( tempFileHandle, "%s %X %X", lineStartIndex, &ulDemuxId, &usPid))
			{
				if( VK_strcmp(lineStartIndex, "PID") != 0 )
				{
					if((usPid == SavedPID[index])||(usPid == SavedPID[index+1]))
					{
						currentPcrValue = strtoul(lineStartIndex, NULL, 16);

					if ( currentPcrValue == 0)
					{
						continue;
					}

				if ((currentPcrValue < lastPcrValue) && (PCR_REC_START_FLAG == 0))
				{
					VK_printf("Start to write pcr value\n");
					PCR_REC_START_FLAG = 1;
				}
				else if ((currentPcrValue < lastPcrValue) && (PCR_REC_START_FLAG == 1))
				{
					VK_printf("DSC Record complete for one loop\n");
					break;
				}

						if(PCR_REC_START_FLAG == 1)
						{
							VK_fprintf(saveFileHandle, "%s %01X %04X", lineStartIndex, ulDemuxId, usPid);

							fscanf( tempFileHandle, "%s", keyType);

							if( VK_strcmp(keyType, "ODD") == 0 )
							{
								VK_fprintf(saveFileHandle, " ODD");
							}
							else
							{
								VK_fprintf(saveFileHandle, " EVEN");
							}

							fscanf( tempFileHandle, "%02x", (const char *)&(key_length_ch));
							VK_fprintf(saveFileHandle, " %02x", key_length_ch);
							if ( key_length_ch == 0x00 )
							{
								continue;
							}
							key_length = key_length_ch;
							pKeyData = (HUINT8*)DD_MEM_Alloc(key_length);
							VK_MEM_Memset(pKeyData,0,key_length);

							for(i=0; i<key_length; i++)
							{
								fscanf( tempFileHandle, "%X", &(tempDataBuffer));
								pKeyData[i] = (unsigned char)tempDataBuffer;
								VK_fprintf(saveFileHandle," %02X",pKeyData[i]);
							}
							fscanf( tempFileHandle, "%*[^\n]");
							VK_fprintf(saveFileHandle,"\n");

							DD_MEM_Free(pKeyData);

						}
						else
						{
							fscanf( tempFileHandle, "%*[^\n]");
						}

						lastPcrValue = currentPcrValue;

					}
				}
			}
			VK_memset(keyType,0,5);
			VK_memset(lineStartIndex,0,9);
		}

		 fclose(tempFileHandle);
		 fclose(saveFileHandle);
		 tempFileHandle=NULL;
		 saveFileHandle=NULL;

		 totalLineNum = 0;
		 lineNum = 0;

		 if(cwpkFileHandle != NULL)
		 {
			if ( (saveFileHandle = fopen(USB_DSC_SAVE_FILE, "r")) == NULL)
			{
				fclose(cwpkFileHandle);
				cwpkFileHandle = NULL;
				return;
			}
			else
			{
				rewind(cwpkFileHandle);

				while(fgets(lineBuf, 255, cwpkFileHandle) != NULL)
				{
					if (lineBuf[VK_strlen(lineBuf)-1] == '\n')
						totalLineNum++;
				}

				line_index = (int *)DD_MEM_Alloc(sizeof(int) * totalLineNum +1);
				if(line_index == NULL)
				{

					fclose(saveFileHandle);
					saveFileHandle = NULL;

					fclose(cwpkFileHandle);
					cwpkFileHandle = NULL;

				}

				rewind(cwpkFileHandle);

				line_index[0] = 0;


				while(fgets(lineBuf, 255, cwpkFileHandle) != NULL)
				{
					if (lineBuf[VK_strlen(lineBuf)-1] == '\n' && lineNum+1 < totalLineNum)
					{
						line_index[lineNum+1] = ftell(cwpkFileHandle);
						lineNum++;
					}
				}

				rewind(saveFileHandle);

				while(fgets(lineBuf1, 255, saveFileHandle) != NULL)
				{
					if (lineBuf1[VK_strlen(lineBuf1)-1] == '\n')
						totalLineNum1++;
				}

				line_index1 = (int *)DD_MEM_Alloc(sizeof(int) * totalLineNum1 +1);
				if(line_index1 == NULL)
				{
					fclose(saveFileHandle);
					saveFileHandle = NULL;

					fclose(cwpkFileHandle);
					cwpkFileHandle = NULL;
					DD_MEM_Free(line_index1);
					line_index1 = NULL;
				}

				rewind(saveFileHandle);

				line_index1[0] = 0;

				while(fgets(lineBuf1, 255, saveFileHandle) != NULL)
				{
					if (lineBuf1[VK_strlen(lineBuf1)-1] == '\n' && lineNum1+1 < totalLineNum1)
					{
						line_index1[lineNum1+1] = ftell(saveFileHandle);
						lineNum1++;
					}
				}

				if ((scwpkFileHandle = fopen(USB_DSC_SCWPK_FILE, "w+")) != NULL)
				{

					for(count  = 0; count < totalLineNum1; count++)
					{
						fseek(saveFileHandle,line_index1[count],SEEK_SET);
						fscanf(saveFileHandle, "%s %X %X", lineStartIndex, &ulDemuxId, &usPid);

						if( VK_strcmp(lineStartIndex, "PID") != 0 )
						{
							for(index = 0; index < totalLineNum; index++)
							{
								diff = line_index[1];
								diff -= line_index[0];

								fseek(cwpkFileHandle,line_index[index],SEEK_SET);
								fscanf(cwpkFileHandle, "%s %X %s %X", scwpk, &cwpkPid,scwpkpcr,&cwpklen);

								if((usPid == cwpkPid)&&(VK_strcmp(lineStartIndex,scwpkpcr) == 0))
								{
									fseek(cwpkFileHandle,line_index[index],SEEK_SET);
									fread(tcwpk,1,diff,cwpkFileHandle);
									fwrite(tcwpk,1,diff,scwpkFileHandle);

								}

								VK_memset(scwpkpcr,0,9);
								VK_memset(scwpk,0,9);
							}

						}
						VK_memset(lineStartIndex,0,9);

					}
				}


				fclose(scwpkFileHandle);
				scwpkFileHandle=NULL;

				fclose(saveFileHandle);
				saveFileHandle=NULL;
			}

			fclose(cwpkFileHandle);
			cwpkFileHandle=NULL;

			DD_MEM_Free(line_index);
			line_index = NULL;

			DD_MEM_Free(line_index1);
			line_index1 = NULL;

		}

		 sync();
		 VK_printf("End DSC recording\n");
	}

	return 0;
}

#endif /* CMD_DSC_DEBUG */
#if 0
static unsigned int desc_vid = 0;
static    unsigned int desc_aid = 0;
void CMD_Word_DSC_CSCClearTestStart(void *szArgs)
{
    unsigned char *temp=NULL;
    unsigned char *keys = NULL;
    unsigned int record_length_field = 0;
    unsigned int tempInt = 0;
    unsigned int key_length = 0;
    unsigned int key_numbers = 0;
    unsigned int desc_id = 0;
    if ( CMD_GetMsgUI() != NULL )
    {
        (int *)(CMD_GetMsgUI())("CMD_Word_DSC_CSCClearTestStart\n");
    }
#if 0
    temp = DD_MEM_Alloc(1024);
#endif
    unsigned char test_cw1[8] = {0x36, 0xFB, 0x07,0xDE, 0x66, 0xA1, 0xFF,0xDE};
    unsigned char test_cw2[8] = {0x0C, 0xED, 0xC2,0x8D, 0xDE, 0x5F, 0xF9,0x94};

    unsigned char ctest_cw1[8] = {0x54, 0x43, 0x34,0xCB, 0x45, 0x54, 0x43,0xDC}; /* Odd */
    unsigned char ctest_cw2[8] = {0xAB, 0xBC, 0xCB,0x32, 0xBA, 0xAB, 0xBC,0x21}; /* Even */
#if 0
    DI_FLASH_GetField(DI_FLASH_FIELD_CSC_KEY, temp, 1024);

    VK_TASK_Sleep(1000);

    /* Extract total length */
    /*record length should be same as to csc length*/
    tempInt = temp[0];
    record_length_field = record_length_field | (tempInt << 24 );
    tempInt = temp[1];
    record_length_field = record_length_field | (tempInt << 16 );
    tempInt = temp[2];
    record_length_field = record_length_field | (tempInt << 8 );
    tempInt = temp[3];
    record_length_field = record_length_field | (tempInt );
    key_length = temp[16];
    key_numbers = (record_length_field - 23)/key_length;

    VK_printf("key_numbers - %d,%d\n", key_numbers, key_length);
    keys = DD_MEM_Alloc(key_numbers*key_length);

    VK_MEM_Memcpy(keys, &temp[17], key_numbers*key_length);
#endif
    DI_DSC_Open(0, DI_DSC_DESC_DVB, &desc_vid, NULL);
    DI_DSC_Open(0, DI_DSC_DESC_DVB, &desc_aid, NULL);

    DI_DSC_SetPid(desc_vid, 0x65);
    DI_DSC_SetPid(desc_aid, 0xc9);

    DI_DSC_SetKey(desc_vid, DI_DSC_DESC_ODD, ctest_cw1, 8, NULL, 0);
    DI_DSC_SetKey(desc_vid, DI_DSC_DESC_EVEN, ctest_cw2, 8, NULL, 0);

    DI_DSC_SetKey(desc_aid, DI_DSC_DESC_ODD, ctest_cw1, 8, NULL, 0);
    DI_DSC_SetKey(desc_aid, DI_DSC_DESC_EVEN, ctest_cw2, 8, NULL, 0);


#if 0
    DD_MEM_Free(keys);
    DD_MEM_Free(temp);
#endif

}
void CMD_Word_DSC_CSCTestStop(void *szArgs)
{
    if ( CMD_GetMsgUI() != NULL )
    {
        (int *)(CMD_GetMsgUI())("CMD_Word_DSC_CSCTestStop\n");
    }
    DI_DSC_ClearPid(desc_vid, 0x65);
    DI_DSC_ClearPid(desc_aid, 0xc9);
    DI_DSC_Close(desc_vid);
    DI_DSC_Close(desc_aid);
}
void CMD_Word_DSC_CSCEncryptedTestStart(void *szArgs)
{
    if ( CMD_GetMsgUI() != NULL )
    {
        (int *)(CMD_GetMsgUI())("CMD_Word_DSC_CSCEncryptedTestStart\n");
    }

    unsigned char *temp=NULL;
    unsigned char *keys = NULL;
    unsigned int record_length_field = 0;
    unsigned int tempInt = 0;
    unsigned int key_length = 0;
    unsigned int key_numbers = 0;
    unsigned int desc_id = 0;
#if 0
    temp = DD_MEM_Alloc(1024);
#endif
    unsigned char test_cw1[8] = {0x36, 0xFB, 0x07,0xDE, 0x66, 0xA1, 0xFF,0xDE}; /*Even  */
    unsigned char test_cw2[8] = {0x0C, 0xED, 0xC2,0x8D, 0xDE, 0x5F, 0xF9,0x94}; /* Odd */

    unsigned char ctest_cw1[8] = {0x54, 0x43, 0x34,0xCB, 0x45, 0x54, 0x43,0xDC}; /* Odd */
    unsigned char ctest_cw2[8] = {0xAB, 0xBC, 0xCB,0x32, 0xBA, 0xAB, 0xBC,0x21}; /* Even */
    unsigned char csc_ekey[16] = {0x0C, 0x7A, 0x9E, 0xFE,
                                 0x13, 0xF3, 0xAB, 0x55,
                                 0xA0, 0x2C, 0x4E, 0xCD,
                                 0x16, 0xC5, 0x66, 0x78};
    unsigned char test_cw[16];
    VK_MEM_Memset(test_cw, 0x00, 16);
#if 0
    DI_FLASH_GetField(DI_FLASH_FIELD_CSC_KEY, temp, 1024);

    VK_TASK_Sleep(1000);

    /* Extract total length */
    /*record length should be same as to csc length*/
    tempInt = temp[0];
    record_length_field = record_length_field | (tempInt << 24 );
    tempInt = temp[1];
    record_length_field = record_length_field | (tempInt << 16 );
    tempInt = temp[2];
    record_length_field = record_length_field | (tempInt << 8 );
    tempInt = temp[3];
    record_length_field = record_length_field | (tempInt );
    key_length = temp[16];
    key_numbers = (record_length_field - 23)/key_length;

    VK_printf("key_numbers - %d,%d\n", key_numbers, key_length);
    keys = DD_MEM_Alloc(key_numbers*key_length);

    VK_MEM_Memcpy(keys, &temp[17], key_numbers*key_length);
#endif



#if 1
    DI_DSC_Open(0, DI_DSC_DESC_DVB, &desc_vid, NULL);
    DI_DSC_Open(0, DI_DSC_DESC_DVB, &desc_aid, NULL);

    DI_DSC_SetPid(desc_vid, 0x65);
    DI_DSC_SetPid(desc_aid, 0xc9);

    tempInt = 0;
   /* while(1)*/
    {

        #if 1
        DI_DSC_SetProtectionKey(desc_vid, csc_ekey, 16);
        #else
        DI_DSC_SetProtectionKey(desc_vid, &keys[(tempInt*key_length)], key_length);
        #endif

        #if 1
        DI_DSC_SetKey(desc_vid, DI_DSC_DESC_ODD, test_cw2, 8, NULL, 0);
        #endif
        #if 1
        DI_DSC_SetKey(desc_vid, DI_DSC_DESC_EVEN, test_cw1, 8, NULL, 0);
        #endif
        DI_DSC_SetProtectionKey(desc_aid, csc_ekey, 16);
        #if 1
        DI_DSC_SetKey(desc_aid, DI_DSC_DESC_ODD, test_cw2, 8, NULL, 0);
        #endif
        #if 1
        DI_DSC_SetKey(desc_aid, DI_DSC_DESC_EVEN, test_cw1, 8, NULL, 0);
        #endif


        tempInt++;
        tempInt = tempInt % key_numbers;
/*        VK_TASK_Sleep(30000);*/
    }
#endif
#if 0
    DD_MEM_Free(keys);
    DD_MEM_Free(temp);
#endif

}
void CMD_Word_DSC_CSCInit(void *szArgs)
{
    if ( CMD_GetMsgUI() != NULL )
    {
        (int *)(CMD_GetMsgUI())("CMD_Word_DSC_CSCInit\n");
    }

}

#else
typedef struct __pid_dsc_set
{
    unsigned long dsc_channel;
    unsigned long pid;
    unsigned long demux_channel;
} CMD_PID_DSC_SET;
static CMD_PID_DSC_SET cmd_dsc_ch1;
static CMD_PID_DSC_SET cmd_dsc_ch2;
static unsigned long cmd_dsc_sem = 0x0;
void CMD_Word_DSC_CSCSetDescrambleKey(void *szArgs)
{
    char *pTempStr = NULL;
    unsigned char *pEKey = NULL;
    unsigned char *pOKey = NULL;
    unsigned int offset = 0;
    unsigned int demuxId = 0;
    unsigned int pid_set = 0;
    unsigned int key_type = 0;
    unsigned int odd_key_length = 0;
    unsigned int even_key_length = 0;
    unsigned int tempInt = 0;
    unsigned int tempInt2 = 0;
    unsigned int tempInt3 = 0;
    unsigned char tempBuf[9];
    unsigned int isUpdate = 0;
    unsigned long dsc_channel = 0x0;

    VK_SEM_Get(cmd_dsc_sem);

    while(1)
    {
        pTempStr = CMD_NextArg((char **)&szArgs);
        if ( pTempStr == 0 )
        {
            break;
        }

        if ( offset == 0 )
        {
            CMD_ReadAsDec(pTempStr, &demuxId);
        }
        else if ( offset == 1 )
        {
            CMD_ReadAsHex(pTempStr, &pid_set);
            if ( (cmd_dsc_ch1.pid == pid_set) || ( cmd_dsc_ch2.pid == pid_set) )
            {
                isUpdate = 1;
            }
        }
        else if ( (offset == 2) || ( offset == 5 ))
        {
            CMD_ReadAsDec(pTempStr, &key_type);
        }
        else if ( (offset == 3) || (offset == 6 ))
        {
            if ( key_type == 1 )
            {
                CMD_ReadAsDec(pTempStr, &odd_key_length);
                pOKey = DD_MEM_Alloc(odd_key_length);
            }
            else if ( key_type == 2 )
            {
                CMD_ReadAsDec(pTempStr, &even_key_length);
                pEKey = DD_MEM_Alloc(even_key_length);
            }
        }
        else if ( (offset == 4 ) || ( offset == 7 ))
        {
            if ( key_type == 1 )
            {
                for ( tempInt2 = 0 ; tempInt2 < odd_key_length ; tempInt2++ )
                {
                    CMD_2HexChTo1Char(&pOKey[tempInt2] , pTempStr[tempInt2*2],pTempStr[tempInt2*2+1]);
                }

            }
            else if ( key_type == 2 )
            {
                for ( tempInt2 = 0 ; tempInt2 < even_key_length ; tempInt2 = tempInt2 + 1 )
                    {
                    CMD_2HexChTo1Char(&pEKey[tempInt2] , pTempStr[tempInt2*2],pTempStr[tempInt2*2+1]);
                }

            }

        }
        offset++;
    }
    if ( isUpdate == 1 )
    {
        if ( (pid_set == cmd_dsc_ch1.pid) && (demuxId == cmd_dsc_ch1.demux_channel) )
        {
            dsc_channel = cmd_dsc_ch1.dsc_channel;
        }
        else if ( (pid_set == cmd_dsc_ch2.pid) && (demuxId == cmd_dsc_ch2.demux_channel) )
        {
            dsc_channel = cmd_dsc_ch2.dsc_channel;
        }
        else
        {
            if ( pEKey != NULL )
            {
                DD_MEM_Free(pEKey);
            }
            if ( pOKey != NULL )
            {
                DD_MEM_Free(pOKey);
            }
            if ( CMD_GetMsgUI() != NULL )
            {
                (int *)(CMD_GetMsgUI())("Error-Invalid Update Data\n");
            }
            VK_SEM_Release(cmd_dsc_sem);
            return;
        }
        if ( (pEKey != NULL) && (even_key_length != 0) )
        {
            DI_DSC_SetKey(dsc_channel, DI_DSC_DESC_EVEN, pEKey, even_key_length, NULL, 0 );
        }
        if ( (pOKey != NULL) && (odd_key_length != 0) )
        {
            DI_DSC_SetKey(dsc_channel, DI_DSC_DESC_ODD, pOKey, odd_key_length, NULL, 0 );
        }
    }
    else
    {
        if ( cmd_dsc_ch1.dsc_channel == 0 )
        {
            DI_DSC_Open(demuxId, DI_DSC_DESC_DVB, &cmd_dsc_ch1.dsc_channel, NULL);
            DI_DSC_SetPid(cmd_dsc_ch1.dsc_channel, pid_set);
            if ( (pEKey != NULL) && (even_key_length != 0) )
            {
                DI_DSC_SetKey(cmd_dsc_ch1.dsc_channel, DI_DSC_DESC_EVEN, pEKey, even_key_length, NULL, 0 );
            }
            if ( (pOKey != NULL) && (odd_key_length != 0) )
            {
                DI_DSC_SetKey(cmd_dsc_ch1.dsc_channel, DI_DSC_DESC_ODD, pOKey, odd_key_length, NULL, 0 );
            }
            cmd_dsc_ch1.pid = pid_set;
            cmd_dsc_ch1.demux_channel = demuxId;
        }
        else if ( cmd_dsc_ch2.dsc_channel == 0 )
        {
            DI_DSC_Open(demuxId, DI_DSC_DESC_DVB, &cmd_dsc_ch2.dsc_channel, NULL);
            DI_DSC_SetPid(cmd_dsc_ch2.dsc_channel, pid_set);
            if ( (pEKey != NULL) && (even_key_length != 0) )
            {
                DI_DSC_SetKey(cmd_dsc_ch2.dsc_channel, DI_DSC_DESC_EVEN, pEKey, even_key_length, NULL, 0 );
            }
            if ( (pOKey != NULL) && (odd_key_length != 0) )
            {
                DI_DSC_SetKey(cmd_dsc_ch2.dsc_channel, DI_DSC_DESC_ODD, pOKey, odd_key_length, NULL, 0 );
            }
            cmd_dsc_ch2.pid = pid_set;
            cmd_dsc_ch2.demux_channel = demuxId;
        }
        else
        {
            if ( pEKey != NULL )
            {
                DD_MEM_Free(pEKey);
            }
            if ( pOKey != NULL )
            {
                DD_MEM_Free(pOKey);
            }
    if ( CMD_GetMsgUI() != NULL )
    {
                (int *)(CMD_GetMsgUI())("Error-No Resource\n");
            }
            VK_SEM_Release(cmd_dsc_sem);
            return;

        }

    }
    if ( pEKey != NULL )
    {
        DD_MEM_Free(pEKey);
    }
    if ( pOKey != NULL )
    {
        DD_MEM_Free(pOKey);
    }
    VK_SEM_Release(cmd_dsc_sem);
    return;
}
void CMD_Word_DSC_CSCTestStop(void *szArgs)
{
    VK_SEM_Get(cmd_dsc_sem);

    if ( cmd_dsc_ch1.dsc_channel != 0 )
    {
        DI_DSC_ClearPid(cmd_dsc_ch1.dsc_channel, cmd_dsc_ch1.pid);
        DI_DSC_Close(cmd_dsc_ch1.dsc_channel);
    }
    if ( cmd_dsc_ch2.dsc_channel != 0 )
    {
        DI_DSC_ClearPid(cmd_dsc_ch2.dsc_channel, cmd_dsc_ch2.pid);
        DI_DSC_Close(cmd_dsc_ch2.dsc_channel);
    }
    cmd_dsc_ch1.dsc_channel = 0;
    cmd_dsc_ch1.pid = 0;
    cmd_dsc_ch1.demux_channel = 0;
    cmd_dsc_ch2.dsc_channel = 0;
    cmd_dsc_ch2.pid = 0;
    cmd_dsc_ch2.demux_channel = 0;
    VK_SEM_Release(cmd_dsc_sem);
    }
void CMD_Word_DSC_CSCClearProtectKey(void *szArgs)
{
#if defined(CONFIG_DSC)
    VK_SEM_Get(cmd_dsc_sem);

    if ( cmd_dsc_ch1.dsc_channel != 0 )
    {
        DI_DSC_ClearProtectionKey(cmd_dsc_ch1.dsc_channel);

    }
    if ( cmd_dsc_ch2.dsc_channel != 0 )
    {
        DI_DSC_ClearProtectionKey(cmd_dsc_ch2.dsc_channel);

    }

    VK_SEM_Release(cmd_dsc_sem);
#endif
    }

void CMD_Word_DSC_CSCSetProtectionKey(void *szArgs)
{
    char *pTempStr = NULL;
    unsigned char *pPKey = NULL;
    unsigned int offset = 0;
    unsigned int demuxId = 0;
    unsigned int pid_set = 0;
    unsigned int key_type = 0;
    unsigned int p_key_length = 0;
    unsigned int tempInt = 0;
    unsigned int tempInt2 = 0;
    unsigned int tempInt3 = 0;
    unsigned char tempBuf[9];
    unsigned int isUpdate = 0;
    unsigned long dsc_channel = 0x0;

    VK_SEM_Get(cmd_dsc_sem);

    while(1)
    {
        pTempStr = CMD_NextArg((char **)&szArgs);
        if ( (pTempStr == 0) || (offset > 4 ))
        {
            break;
        }

        if ( offset == 0 )
        {
            CMD_ReadAsDec(pTempStr, &demuxId);
        }
        else if ( offset == 1 )
        {
            CMD_ReadAsHex(pTempStr, &pid_set);
            if ( (cmd_dsc_ch1.pid == pid_set) || ( cmd_dsc_ch2.pid == pid_set) )
            {
                isUpdate = 1;
        }
        }
        else if ( (offset == 2) )
        {
            CMD_ReadAsDec(pTempStr, &key_type);
        }
        else if ( (offset == 3) )
        {
            if ( key_type == 9 )
            {
                CMD_ReadAsDec(pTempStr, &p_key_length);
                pPKey = DD_MEM_Alloc(p_key_length);
            }
        }
        else if ( (offset == 4 ) )
        {
            if ( key_type == 9 )
            {
                for ( tempInt2 = 0 ; tempInt2 < p_key_length ; tempInt2++ )
                {
                    CMD_2HexChTo1Char(&pPKey[tempInt2] , pTempStr[tempInt2*2] , pTempStr[tempInt2*2+1]);
    }

            }


        }
        offset++;
    }
#if 1
    if ( isUpdate == 1 )
    {
        if ( (pid_set == cmd_dsc_ch1.pid) && (demuxId == cmd_dsc_ch1.demux_channel) )
        {
            dsc_channel = cmd_dsc_ch1.dsc_channel;
        }
        else if ( (pid_set == cmd_dsc_ch2.pid) && (demuxId == cmd_dsc_ch2.demux_channel) )
        {
            dsc_channel = cmd_dsc_ch2.dsc_channel;
        }
        else
        {
            if ( pPKey != NULL )
            {
                DD_MEM_Free(pPKey);
            }

            if ( CMD_GetMsgUI() != NULL )
            {
                (int *)(CMD_GetMsgUI())("Error-Invalid Update Data\n");
            }
            VK_SEM_Release(cmd_dsc_sem);
            return;

        }
        if ( (pPKey != NULL) && (p_key_length != 0) )
        {
            DI_DSC_SetProtectionKey(dsc_channel, pPKey, p_key_length);
        }
    }
    else
    {
        if ( cmd_dsc_ch1.dsc_channel == 0 )
        {
            DI_DSC_Open(demuxId, DI_DSC_DESC_DVB, &cmd_dsc_ch1.dsc_channel, NULL);
            DI_DSC_SetPid(cmd_dsc_ch1.dsc_channel, pid_set);
            if ( (pPKey != NULL) && (p_key_length != 0) )
            {
                DI_DSC_SetProtectionKey(cmd_dsc_ch1.dsc_channel, pPKey, p_key_length);
            }
            cmd_dsc_ch1.pid = pid_set;
            cmd_dsc_ch1.demux_channel = demuxId;

        }
        else if ( cmd_dsc_ch2.dsc_channel == 0 )
        {
            DI_DSC_Open(demuxId, DI_DSC_DESC_DVB, &cmd_dsc_ch2.dsc_channel, NULL);
            DI_DSC_SetPid(cmd_dsc_ch2.dsc_channel, pid_set);

            if ( (pPKey != NULL) && (p_key_length != 0) )
            {
                DI_DSC_SetProtectionKey(cmd_dsc_ch2.dsc_channel, pPKey, p_key_length);
            }
            cmd_dsc_ch2.pid = pid_set;
            cmd_dsc_ch2.demux_channel = demuxId;

}
        else
        {
            if ( pPKey != NULL )
{
                DD_MEM_Free(pPKey);
            }
    if ( CMD_GetMsgUI() != NULL )
    {
                (int *)(CMD_GetMsgUI())("Error-No Resource\n");
            }
            VK_SEM_Release(cmd_dsc_sem);
            return;

    }

}
#endif
    if ( pPKey != NULL )
{
        DD_MEM_Free(pPKey);
    }

    VK_SEM_Release(cmd_dsc_sem);
}

void CMD_Word_DSC_CSCInit(void *szArgs)
{
    if ( CMD_GetMsgUI() != NULL )
    {
        (int *)(CMD_GetMsgUI())("CMD_Word_DSC_CSCInit\n");
    }
    cmd_dsc_ch1.dsc_channel = 0;
    cmd_dsc_ch1.pid = 0;
    cmd_dsc_ch1.demux_channel = 0;
    cmd_dsc_ch2.dsc_channel = 0;
    cmd_dsc_ch2.pid = 0;
    cmd_dsc_ch2.demux_channel = 0;
    VK_SEM_Create(&cmd_dsc_sem, "csc_test_sem", VK_SUSPENDTYPE_FIFO);
}
#endif
void CMD_RegisterDSC(void)
{
#ifdef	CMD_DSC_DEBUG
	CMD_RegisterWord(CMD_Word_DSCTest,
					 (char *)"dsc_test", 				/* keyword */
					 (char *)"test dsc", 				/* help */
					 (char *)"dsc_test");			/* usage */
	CMD_RegisterWord(CMD_Word_DSCRec,
					 (char *)"dsc_rec", 				/* keyword */
					 (char *)"record dsc", 			/* help */
					 (char *)"dsc_rec <start stop>");	/* usage */
#endif
#if 0
    CMD_RegisterWord(CMD_Word_DSC_CSCClearTestStart,
                     (char *)"csc_setkey",
                     (char *)"Test nagra csc pre-descrambl test",
                     (char *)"csc_setkey pidvalue 8-bytesE-Key 8-bytesO-Key");
    CMD_RegisterWord(CMD_Word_DSC_CSCTestStop,
                     (char *)"csc_teststop",
                     (char *)"Stop Test nagra csc pre-descrambl test",
                     (char *)"csc_teststop pidvalue");
    CMD_RegisterWord(CMD_Word_DSC_CSCEncryptedTestStart,
                 (char *)"csc_setpkey",
                 (char *)"Start Test nagra csc pre-descrambl test",
                 (char *)"csc_etest pidvalue 16bytesE-Key 8bytesEkey 8bytesOkey");
    CMD_RegisterWord(CMD_Word_DSC_CSCInit,
                 (char *)"csc_init",
                 (char *)"Start Test nagra csc pre-descrambl test",
                 (char *)"csc_etest pidvalue 16bytesE-Key 8bytesEkey 8bytesOkey");
#else
    CMD_RegisterWord(CMD_Word_DSC_CSCSetDescrambleKey,
                     (char *)"csc_setkey",
                     (char *)"Test nagra csc pre-descrambl test",
                     (char *)"csc_setkey pidvalue 8-bytesE-Key 8-bytesO-Key");
    CMD_RegisterWord(CMD_Word_DSC_CSCTestStop,
                     (char *)"csc_teststop",
                     (char *)"Stop Test nagra csc pre-descrambl test",
                     (char *)"csc_teststop pidvalue");
    CMD_RegisterWord(CMD_Word_DSC_CSCSetProtectionKey,
                 (char *)"csc_setpkey",
                 (char *)"Start Test nagra csc pre-descrambl test",
                 (char *)"csc_etest pidvalue 16bytesE-Key 8bytesEkey 8bytesOkey");
    CMD_RegisterWord(CMD_Word_DSC_CSCInit,
                 (char *)"csc_init",
                 (char *)"Start Test nagra csc pre-descrambl test",
                 (char *)"csc_etest pidvalue 16bytesE-Key 8bytesEkey 8bytesOkey");
    CMD_RegisterWord(CMD_Word_DSC_CSCClearProtectKey,
                 (char *)"csc_clearp",
                 (char *)"Start Test nagra csc pre-descrambl test",
                 (char *)"csc_etest pidvalue 16bytesE-Key 8bytesEkey 8bytesOkey");

#endif
}


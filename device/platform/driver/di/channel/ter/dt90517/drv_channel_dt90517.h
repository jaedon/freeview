
/************************************************************************
 *
 *	TC90517.h
 *
 *	Copyright (c) 2007 by Humax	Corp.
 *
************************************************************************/
#ifndef TC90517_H
#define TC90517_H

#include "htype.h"

#define DEMOD_TC90517_ADDRESS	0x1E //0x3C
#define TUNER_DNVS22CX_ADDRESS	0x60 //0xC0

#define QAM_DEMOD_ADDRESS		0x0c//0x18
#define JQAM_DEMOD_ADDRESS		0x0e//0x1C

#if defined (CONFIG_PLATFORM_TM201COMBO)					// JD - Add Definition related Demod I2C Addr
#define MASTER_TER_DEMOD			0					// Channel ID와 비교하여 Master / Slave를 구분하기 위함
#define SLAVE_TER_DEMOD				2
#define MASTER_DEMOD_ADDR_TC90517	0x1E				// Channel ID = 0	0x3C
#define SLAVE_DEMOD_ADDR_TC90517	0x18				// Channel ID = 2	0x30
#endif


#define TC90517_01					0x01
#define TC90517_02					0x02
#define TC90517_03					0x03
#define TC90517_04					0x04
#define TC90517_05					0x05
#define TC90517_06					0x06
#define TC90517_07					0x07
#define TC90517_08					0x08
#define TC90517_09					0x09
#define TC90517_0A					0x0A
#define TC90517_0B					0x0B
#define TC90517_0C					0x0C
#define TC90517_0D					0x0D
#define TC90517_0E					0x0E
#define TC90517_0F					0x0F
#define TC90517_10					0x10
#define TC90517_11					0x11
#define TC90517_12					0x12
#define TC90517_13					0x13
#define TC90517_14					0x14
#define TC90517_15					0x15
#define TC90517_16					0x16
#define TC90517_17					0x17
#define TC90517_18					0x18
#define TC90517_19					0x19
#define TC90517_1A					0x1A
#define TC90517_1B					0x1B
#define TC90517_1C					0x1C
#define TC90517_1D					0x1D
#define TC90517_1E					0x1E
#define TC90517_1F					0x1F
#define TC90517_20					0x20
#define TC90517_21					0x21
#define TC90517_22					0x22
#define TC90517_23					0x23
#define TC90517_24					0x24
#define TC90517_25					0x25
#define TC90517_26					0x26
#define TC90517_27					0x27
#define TC90517_28					0x28
#define TC90517_29					0x29
#define TC90517_2A					0x2A
#define TC90517_2B					0x2B
#define TC90517_2C					0x2C
#define TC90517_2D					0x2D
#define TC90517_2E					0x2E
#define TC90517_2F					0x2F
#define TC90517_30					0x30
#define TC90517_31					0x31
#define TC90517_32					0x32
#define TC90517_33					0x33
#define TC90517_34					0x34
#define TC90517_35					0x35
#define TC90517_36					0x36
#define TC90517_37					0x37
#define TC90517_38					0x38
#define TC90517_39					0x39
#define TC90517_3A					0x3A
#define TC90517_3B					0x3B
#define TC90517_3C					0x3C
#define TC90517_3D					0x3D
#define TC90517_3E					0x3E
#define TC90517_3F					0x3F
#define TC90517_40					0x40
#define TC90517_41					0x41
#define TC90517_42					0x42
#define TC90517_43					0x43
#define TC90517_44					0x44
#define TC90517_45					0x45
#define TC90517_46					0x46
#define TC90517_47					0x47
#define TC90517_48					0x48
#define TC90517_49					0x49
#define TC90517_4A					0x4A
#define TC90517_4B					0x4B
#define TC90517_4C					0x4C
#define TC90517_50					0x50
#define TC90517_51					0x51
#define TC90517_52					0x52
#define TC90517_53					0x53
#define TC90517_54					0x54
#define TC90517_55					0x55
#define TC90517_56					0x56
#define TC90517_57					0x57
#define TC90517_58					0x58
#define TC90517_59					0x59
#define TC90517_5A					0x5A
#define TC90517_5B					0x5B
#define TC90517_5C					0x5C
#define TC90517_5D					0x5D

#define TC90517_62					0x62
#define TC90517_63					0x63
#define TC90517_64					0x64
#define TC90517_65					0x65
#define TC90517_66					0x66
#define TC90517_67					0x67
#define TC90517_68					0x68

#define TC90517_70					0x70
#define TC90517_71					0x71
#define TC90517_72					0x72
#define TC90517_73					0x73
#define TC90517_74					0x74
#define TC90517_75					0x75
#define TC90517_76					0x76
#define TC90517_77					0x77
#define TC90517_7C					0x7C
#define TC90517_7D					0x7D
#define TC90517_7F					0x7F
#define TC90517_80 					0x80
#define TC90517_81 					0x81
#define TC90517_82 					0x82
#define TC90517_83 					0x83
#define TC90517_84 					0x84
#define TC90517_85 					0x85
#define TC90517_86 					0x86
#define TC90517_87 					0x87
#define TC90517_89 					0x89
#define TC90517_8A 					0x8A
#define TC90517_8B					0x8B
#define TC90517_8C					0x8C
#define TC90517_8D					0x8D
#define TC90517_8E 					0x8E
#define TC90517_8F 					0x8F
#define TC90517_90 					0x90
#define TC90517_91 					0x91
#define TC90517_92 					0x92
#define TC90517_93 					0x93
#define TC90517_94 					0x94
#define TC90517_95 					0x95
#define TC90517_96 					0x96
#define TC90517_97 					0x97
#define TC90517_98 					0x98
#define TC90517_99 					0x99
#define TC90517_9A 					0x9A
#define TC90517_9B					0x9B
#define TC90517_9C					0x9C
#define TC90517_9D					0x9D
#define TC90517_9E 					0x9E
#define TC90517_9F 					0x9F
#define TC90517_A0 					0xA0
#define TC90517_A1 					0xA1
#define TC90517_A2 					0xA2
#define TC90517_A3 					0xA3
#define TC90517_A4 					0xA4
#define TC90517_A5 					0xA5
#define TC90517_A6 					0xA6
#define TC90517_A7 					0xA7
#define TC90517_A8 					0xA8
#define TC90517_A9 					0xA9
#define TC90517_AA 					0xAA
#define TC90517_AB					0xAB
#define TC90517_AC					0xAC
#define TC90517_AD					0xAD
#define TC90517_B0					0xB0
#define TC90517_B1					0xB1
#define TC90517_B2					0xB2
#define TC90517_B3					0xB3
#define TC90517_B4					0xB4
#define TC90517_B5					0xB5
#define TC90517_B6					0xB6
#define TC90517_B7					0xB7
#define TC90517_B8					0xB8
#define TC90517_B9					0xB9
#define TC90517_BA					0xBA
#define TC90517_BB					0xBB
#define TC90517_BC					0xBC
#define TC90517_BD					0xBD
#define TC90517_C2					0xC2
#define TC90517_C7					0xC7
#define TC90517_C8					0xC8
#define TC90517_C9					0xC9
#define TC90517_CA					0xCA
#define TC90517_CB					0xCB
#define TC90517_CC					0xCC
#define TC90517_CD					0xCD
#define TC90517_CE					0xCE
#define TC90517_CF					0xCF
#define TC90517_DA					0xDA

#define TC90517_E0					0xE0

#define TC90517_E4					0xE4

#define TC90517_E5					0xE5

#define TC90517_EB					0xEB
#define TC90517_EC					0xEC
#define TC90517_EF					0xEF
#define TC90517_FE					0xFE

typedef enum {
	fft_2K = 0,
	fft_4K,
	fft_8K,
	fft_NOT
} CH_ISDB_FFT;

typedef enum {
	guard_1_4 = 0,
	guard_1_8,
	guard_1_16,
	guard_1_32
 } CH_ISDB_GUARD;

typedef enum {
	CodeRate_1_2 = 0,
	CodeRate_2_3,
	CodeRate_3_4,
	CodeRate_5_6,
	CodeRate_7_8,
	CodeRate_NOT
  } CH_ISDB_CODERATE;

typedef enum {
	constellation_DQPSK = 0,
	constellation_QPSK,
	constellation_16QAM,
	constellation_64QAM,
	constellation_NOTHING
   } CH_ISDB_CONSTELLATION;

typedef struct
{
	HUINT8  Isdb_Sync;
	HUINT8  Isdb_Level;
	HUINT8  Isdb_Tmcc;
	HUINT8  Isdb_Recvmd;
	HUINT8  Isdb_Frame;
	HUINT8  Isdb_Viterbi;
	HUINT8  Isdb_Rs;
	HUINT8  Isdb_Pkt;
 } TC90517_status;

typedef struct
{
	 HUINT8 layer_A_CodeRate;
	 HUINT8 layer_B_CodeRate;
	 HUINT8 layer_C_CodeRate;
	 HUINT8 layer_A_segments;
	 HUINT8 layer_B_segments;
	 HUINT8 layer_C_segments;
	 HUINT8 layer_A_constellation;
	 HUINT8 layer_B_constellation;
	 HUINT8 layer_C_constellation;
	 HUINT8 layer_A_Interleave;
	 HUINT8 layer_B_Interleave;
	 HUINT8 layer_C_Interleave;
	 HUINT8  layer_A_error;
	 HUINT8  layer_B_error;
	 HUINT8  layer_C_error;
 } TC90517_LayerInfo_t;

typedef struct
{
	HUINT32				frequency;
	HINT16				strength;
	HUINT32				quality;
	HUINT16				snr;
	HUINT8				emergencyFlag;
	HUINT8 				TPS_fft_info;
	HUINT8 				TPS_guard_info;
	TC90517_LayerInfo_t	layerinfo;
} TC90517_Status_t;

typedef struct
{
	HUINT32	UnErroredNo[3];
	HUINT32	CorrectedNo[3];
	HUINT32	UnCorrectedNo[3];
	HUINT32	InbandBer[3];
	HUINT32	UnCorrectedFlag[3];
} TC90517_CorrectionInfo_t;

void DT90517_Initialise(int nChannelId);
int DT90517_InitDevice(int nChannelId);
int DT90517_SetTune( int nChannelId, HUINT8 *bChecklock , HUINT32 unWorkingId );
int DT90517_CheckStatus ( int nChannelId);
HBOOL DT90517_CheckLock ( int nChannelId, HUINT32 unWorkingID );
int  DT90517_GetSignalInfo(int nChannelId, DI_CH_SignalQuality_t *pstSignalInfo);
int  DT90517_SetPowerState( int nChannelId, CH_Power_e etPower );
int DT90517_EnableTsOutput ( int nChannelId );
int DT90517_DisableTsOutput ( int nChannelId );
void	DT90517_ResetGpio(int nChannelId) ;
void DT90517_InitRegister(HUINT32 nChannelId);
int DT90517_DumpRegister( int nChannelId);
int DT90517_CheckAntState( int nChannelId );
void DT90517_ProcessRecoverAntenna( int nChannelId, HUINT32 unWorkingId );
int DT90517_SetAntState( int nChannelId, DI_CH_AntStatus_e etAntState );
DI_CH_AntStatus_e DT90517_GetAntState( int nChannelId );

void	DT90517_Stop(int nChannelId);
int DT90517_CheckTsmfStatus(int nChannelId);
void	DT90517_GetTsmfData(int nChannelId, HUINT16 *tsNo);
void	DT90517_GetCorrectionData(int nChannelId, HUINT32 *correctedNo, HUINT32 *unCorrectedNo, HUINT32 *unErroredNo, HUINT32 *inbandBer, HUINT32 *UnCorrectedFlag);
void	DT90517_GetAGC(int nChannelId, HUINT8 *ucpRFAgc, HUINT8 *ucpIFAgc);
void	DT90517_SetBERCalTime(int nChannelId, HUINT32 berCalMode);
void	DT90517_GetEwsInfo(int nChannelId, HUINT8 *ucIsEws);
void DT90517_GetChData(int nChannelId, HINT16 *strength, HUINT32 *quality, HUINT16 *snr, HUINT8 *emergencyFlag);

#endif


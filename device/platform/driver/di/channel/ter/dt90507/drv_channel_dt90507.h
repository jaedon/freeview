
/************************************************************************
 *
 *	TC90507.h
 *
 *	Copyright (c) 2007 by Humax	Corp.
 *
************************************************************************/
#ifndef TC90507_H
#define TC90507_H

#include "htype.h"


#define TC90507_DEMOD_ADDRESS	0x10//0x20
#define ISDBT_TD1686_ADDRESS		0x61//0xC2

#define TC90507_01					0x01
#define TC90507_02					0x02
#define TC90507_03					0x03
#define TC90507_04					0x04
#define TC90507_05					0x05
#define TC90507_06					0x06
#define TC90507_07					0x07
#define TC90507_08					0x08
#define TC90507_09					0x09
#define TC90507_0A					0x0A
#define TC90507_0B					0x0B
#define TC90507_0C					0x0C
#define TC90507_0D					0x0D
#define TC90507_0E					0x0E
#define TC90507_0F					0x0F
#define TC90507_10					0x10
#define TC90507_11					0x11
#define TC90507_12					0x12
#define TC90507_13					0x13
#define TC90507_14					0x14
#define TC90507_15					0x15
#define TC90507_16					0x16
#define TC90507_17					0x17
#define TC90507_18					0x18
#define TC90507_19					0x19
#define TC90507_1A					0x1A
#define TC90507_1B					0x1B
#define TC90507_1C					0x1C
#define TC90507_1D					0x1D
#define TC90507_1E					0x1E
#define TC90507_1F					0x1F
#define TC90507_20					0x20
#define TC90507_21					0x21
#define TC90507_22					0x22
#define TC90507_23					0x23
#define TC90507_24					0x24
#define TC90507_25					0x25
#define TC90507_26					0x26
#define TC90507_27					0x27
#define TC90507_28					0x28
#define TC90507_29					0x29
#define TC90507_2A					0x2A
#define TC90507_2B					0x2B
#define TC90507_2C					0x2C
#define TC90507_2D					0x2D
#define TC90507_2E					0x2E
#define TC90507_2F					0x2F
#define TC90507_30					0x30
#define TC90507_31					0x31
#define TC90507_32					0x32
#define TC90507_33					0x33
#define TC90507_34					0x34
#define TC90507_35					0x35
#define TC90507_36					0x36
#define TC90507_37					0x37
#define TC90507_38					0x38
#define TC90507_39					0x39
#define TC90507_3A					0x3A
#define TC90507_3B					0x3B
#define TC90507_3C					0x3C
#define TC90507_3D					0x3D
#define TC90507_3E					0x3E
#define TC90507_3F					0x3F
#define TC90507_40					0x40
#define TC90507_41					0x41
#define TC90507_42					0x42
#define TC90507_43					0x43
#define TC90507_44					0x44
#define TC90507_45					0x45
#define TC90507_46					0x46
#define TC90507_47					0x47
#define TC90507_48					0x48
#define TC90507_49					0x49
#define TC90507_4A					0x4A
#define TC90507_4B					0x4B
#define TC90507_4C					0x4C
#define TC90507_50					0x50
#define TC90507_51					0x51
#define TC90507_52					0x52
#define TC90507_53					0x53
#define TC90507_54					0x54
#define TC90507_55					0x55
#define TC90507_56					0x56
#define TC90507_57					0x57
#define TC90507_58					0x58
#define TC90507_59					0x59
#define TC90507_5A					0x5A
#define TC90507_5B					0x5B
#define TC90507_5C					0x5C
#define TC90507_5D					0x5D
#define TC90507_70					0x70
#define TC90507_71					0x71
#define TC90507_72					0x72
#define TC90507_73					0x73
#define TC90507_74					0x74
#define TC90507_75					0x75
#define TC90507_76					0x76
#define TC90507_77					0x77
#define TC90507_7C					0x7C
#define TC90507_7D					0x7D
#define TC90507_7F					0x7F
#define TC90507_80 					0x80
#define TC90507_81 					0x81
#define TC90507_82 					0x82
#define TC90507_83 					0x83
#define TC90507_84 					0x84
#define TC90507_85 					0x85
#define TC90507_86 					0x86
#define TC90507_87 					0x87
#define TC90507_89 					0x89
#define TC90507_8A 					0x8A
#define TC90507_8B					0x8B
#define TC90507_8C					0x8C
#define TC90507_8D					0x8D
#define TC90507_8E 					0x8E
#define TC90507_8F 					0x8F
#define TC90507_90 					0x90
#define TC90507_91 					0x91
#define TC90507_92 					0x92
#define TC90507_93 					0x93
#define TC90507_94 					0x94
#define TC90507_95 					0x95
#define TC90507_96 					0x96
#define TC90507_97 					0x97
#define TC90507_98 					0x98
#define TC90507_99 					0x99
#define TC90507_9A 					0x9A
#define TC90507_9B					0x9B
#define TC90507_9C					0x9C
#define TC90507_9D					0x9D
#define TC90507_9E 					0x9E
#define TC90507_9F 					0x9F
#define TC90507_A0 					0xA0
#define TC90507_A1 					0xA1
#define TC90507_A2 					0xA2
#define TC90507_A3 					0xA3
#define TC90507_A4 					0xA4
#define TC90507_A5 					0xA5
#define TC90507_A6 					0xA6
#define TC90507_A7 					0xA7
#define TC90507_A8 					0xA8
#define TC90507_A9 					0xA9
#define TC90507_AA 					0xAA
#define TC90507_AB					0xAB
#define TC90507_AC					0xAC
#define TC90507_AD					0xAD
#define TC90507_B0					0xB0
#define TC90507_B1					0xB1
#define TC90507_B2					0xB2
#define TC90507_B3					0xB3
#define TC90507_B4					0xB4
#define TC90507_B5					0xB5
#define TC90507_B6					0xB6
#define TC90507_B7					0xB7
#define TC90507_B8					0xB8
#define TC90507_B9					0xB9
#define TC90507_BA					0xBA
#define TC90507_BB					0xBB
#define TC90507_BC					0xBC
#define TC90507_BD					0xBD
#define TC90507_C2					0xC2
#define TC90507_C7					0xC7
#define TC90507_C8					0xC8
#define TC90507_C9					0xC9
#define TC90507_CA					0xCA
#define TC90507_CB					0xCB
#define TC90507_CC					0xCC
#define TC90507_CD					0xCD
#define TC90507_CE					0xCE
#define TC90507_CF					0xCF
#define TC90507_DA					0xDA
#define TC90507_E4					0xE4
#define TC90507_EC					0xEC
#define TC90507_EF					0xEF
#define TC90507_FE					0xFE

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
 } TC90507_status;

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
 } TC90507_LayerInfo_t;

typedef struct
{
	HUINT32				frequency;
	HINT16				strength;
	HUINT32				quality;
	HUINT16				snr;
	HUINT8				emergencyFlag;
	HUINT8 				TPS_fft_info;
	HUINT8 				TPS_guard_info;
	TC90507_LayerInfo_t	layerinfo;
} TC90507_Status_t;

typedef struct
{
	HUINT32	UnErroredNo[3];
	HUINT32	CorrectedNo[3];
	HUINT32	UnCorrectedNo[3];
	HUINT32	InbandBer[3];
	HUINT32	UnCorrectedFlag[3];
} TC90507_CorrectionInfo_t;

void DT90507_Initialise(int nChannelId);
int DT90507_InitDevice(int nChannelId);
int DT90507_SetTune( int nChannelId, HUINT8 *bChecklock , HUINT32 unWorkingId );
int DT90507_CheckStatus ( int nChannelId);
HBOOL DT90507_CheckLock ( int nChannelId, HUINT32 unWorkingID );
int  DT90507_GetSignalInfo(int nChannelId, DI_CH_SignalQuality_t *pstSignalInfo);
int  DT90507_SetPowerState( int nChannelId, CH_Power_e etPower );
int DT90507_EnableTsOutput ( int nChannelId );
int DT90507_DisableTsOutput ( int nChannelId );
void	DT90507_ResetGpio(int nChannelId) ;
void DT90507_InitRegister(HUINT32 nChannelId);
int DT90507_DumpRegister( int nChannelId);
int DT90507_CheckAntState( int nChannelId );
void DT90507_ProcessRecoverAntenna( int nChannelId, HUINT32 unWorkingId );
int DT90507_SetAntState( int nChannelId, DI_CH_AntStatus_e etAntState );
DI_CH_AntStatus_e DT90507_GetAntState( int nChannelId );

void	DT90507_Stop(int nChannelId);
int DT90507_CheckTsmfStatus(int nChannelId);
void DT90507_GetTsmfStatus(int nChannelId, int *tsmfStatus);
void	DT90507_GetTsmfData(int nChannelId, HUINT16 *tsNo);
void	DT90507_GetCorrectionData(int nChannelId, HUINT32 *correctedNo, HUINT32 *unCorrectedNo, HUINT32 *unErroredNo, HUINT32 *inbandBer, HUINT32 *UnCorrectedFlag);
void	DT90507_GetAGC(int nChannelId, HUINT8 *ucpRFAgc, HUINT8 *ucpIFAgc);
void	DT90507_SetBERCalTime(int nChannelId, HUINT32 berCalMode);
void	DT90507_GetEwsInfo(int nChannelId, HUINT8 *isEws);
void DT90507_GetChData(int nChannelId, HINT16 *strength, HUINT32 *quality, HUINT16 *snr, HUINT8 *emergencyFlag);

#endif


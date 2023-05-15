/************************************************************************
 *
 *	TC90507.h
 *
 *	Copyright (c) 2007 by Humax	Corp.
 *
************************************************************************/

#include "di_channel.h"
#include "di_channel_priv.h"
#include "di_channel_t.h"
#include "di_channel_attr.h"

#include "drv_err.h"
#include "drv_i2c.h"
#include "drv_channel_t.h"
#include "drv_gpio.h"

#include "drv_channel.h"
#include "drv_channel_dt90507.h"

#define BUS_NUM				1
#define ISDBT_LOCK_TRIAL	2
#define ISDBT_LAYER_NUM	3

static int	DT90507_SetTuner(int nChannelId, HUINT32 frequency);
static int	DT90507_GetStatus(int nChannelId);
static int DT90507_Lock( int nChannelId, unsigned char *bChecklock );

static void	GetUnCorrectedPacket(int nChannelId, HUINT32 *unCorPkt);
static void	GetUnErroredPacket(int nChannelId, HUINT32 *unErroredPkt);
static HINT16	GetIsdbStrength(int nChannelId);
static HUINT16	GetOfdmCn(int nChannelId);
static void	GetIsdbtBer(int nChannelId, HUINT32 *berVal, HUINT32 errCnt, HUINT32 errCycle, HUINT32 *ngFlag);
static int		CheckLock(int nChannelId);

static HUINT8		Set_TD1686Tuner(int nChannelId, HUINT32 freq);
static HUINT8		FastLockIsdb(int nChannelId);
static HUINT8		CheckIsdbAgcLock(int nChannelId);
static float	PowerOfExponential(float mantissa, HUINT8 exponent);

//void		ClearIsdbDemod(int nChannelId);
HUINT8		ViewFrameLock(int nChannelId);
void		ViewOfdmParam(int nChannelId);
void		ViewDemodStatus(int nChannelId);
HUINT8		ViewTmccDetect(int nChannelId);

void		DeactivateIsdbDemod(int nChannelId);
void		ActivateIsdbDemod(int nChannelId);

static void Start_Accu(int nChannelId);
//static void End_Accu(int nChannelId);
void View_Accu(int nChannelId);
static void Reset_Accu(int nChannelId);

#define MAX_NUM_TER_UNITS		2

static CH_TER_AntStatus_t s_stAntState[MAX_NUM_TER_UNITS];

HUINT8	gIsdbDemodStat;

HUINT16 HKFRQ, AFC30, AFC39, AFC32;
HUINT8 ghkfrq, gafc,gafc39, gafc32;

 HUINT8	gValCP, chgCP;
 HUINT8	gValBand, chgBand;
static HUINT32	totalCntToDisplayBER[BUS_NUM] = {0};
 HUINT8	save_stream;

TC90507_status			gTC90507Status[BUS_NUM];
TC90507_Status_t 			gTC90507Channel[BUS_NUM];
TC90507_LayerInfo_t 		gTC90507Layer[BUS_NUM];
TC90507_CorrectionInfo_t	gTC90507Correction[BUS_NUM];

static HUINT32		beforeId2[BUS_NUM];
static HUINT32		gCurrentId[BUS_NUM];
static HBOOL			sTsmfEmgOnFlag[BUS_NUM];
static HBOOL			sTsmfEmgOffFlag[BUS_NUM];

HINT32	gDT90507EmgOnFlag[BUS_NUM];


HUINT8 accuFlag[BUS_NUM];				/* accumualtion enable Flag */
HUINT32 accuCnt[BUS_NUM];				/* accumulation elapsed time : accuCnt * 4 = total elapsed time */
HUINT32 accuUnCorPkt[BUS_NUM][ISDBT_LAYER_NUM];		/* Total # of accumulated packet : [nChannelId][Layer] */

HUINT8	isdb_if_min = 0x60;
HUINT8	isdb_if_max = 0xff;
HUINT8	isdb_rf_min  = 0x00;
HUINT8	isdb_rf_max  = 0xff;
HUINT8	isdb_agc_test;

HINT16	levelNo = 41;

HINT8	dslevel[41] = {
24,23,22,21,20,19,18,17,16,15,
14,13,12,11,10,9,8,7,6,5,
4,3,2,1,0,-1,-2,-3,-4,-5,
-6,-7,-8,-9,-10,-11,-12,-13,-14,-15,
-16
};


HUINT8  RfAgcIsdb1[41] = {
87,89,92,94,97,99,102,105,107,110,
112,115,118,120,123,125,128,130,133,135,
138,140,142,145,147,150,152,154,156,159,
161,163,165,167,169,171,173,176,178,180,
182
};
HUINT8 IfAgcIsdb1[41] = {
120,120,120,120,120,120,120,120,120,120,
120,120,120,120,120,120,120,120,120,120,
120,120,120,120,120,120,120,120,120,120,
120,120,120,120,120,120,120,120,120,120,
120
};

HUINT8 RfAgcIsdb2[41] = {
71,74,76,78,81,84,87,90,94,97,
101,104,107,110,114,118,121,124,127,130,
133,136,138,141,144,146,149,151,153,156,
158,160,162,165,167,169,171,173,176,178,
180
};
HUINT8 IfAgcIsdb2[41] = {
120,120,120,120,120,120,120,120,120,120,
120,120,120,120,120,120,120,120,120,120,
120,120,120,120,120,120,120,120,120,120,
120,120,120,120,120,120,120,120,120,120,
120
};

HUINT8  RfAgcIsdb3[41] = {
56,57,59,61,62,64,66,69,71,74,
77,80,84,89,94,99,104,111,116,122,
128,133,138,142,147,152,155,159,162,165,
168,170,173,176,178,181,184,186,189,192,
195
};
HUINT8 IfAgcIsdb3[41] = {
120,120,120,120,120,120,120,120,120,120,
120,120,120,120,120,120,120,120,120,120,
120,120,120,120,120,120,120,120,120,120,
120,120,120,120,120,120,120,120,120,120,
120
};

/* 93~255MHz, 201MHz */
HUINT8  isdbtRfAgcGain1[2][41] =
{
	{
		88, 91, 94, 97, 100, 103, 105, 108, 111, 114,
		116, 119, 122, 125, 128, 130, 133, 136, 138, 141,
		143, 145, 148, 150, 152, 154, 157, 159, 161, 163,
		165, 167, 170, 172, 174, 176, 179, 181, 184, 186,
		190
	},
	{
		84, 87, 90, 93, 96, 99, 102, 105, 108, 111,
		114, 117, 120, 122, 125, 128, 130, 133, 136, 138,
		141, 143, 145, 148, 150, 152, 155, 157, 159, 161,
		163, 165, 168, 170, 172, 174, 177, 179, 182, 185,
		189
	}
};
HUINT8  isdbtIfAgcGain1[2][41] =
{
	{
		120, 120, 120, 120, 120, 120, 120, 120, 120, 120,
		120, 120, 120, 120, 120, 120, 120, 120, 120, 120,
		120, 120, 120, 120, 120, 120, 120, 120, 120, 120,
		120, 120, 120, 120, 120, 120, 120, 120, 120, 120,
		120
	},
	{
		120, 120, 120, 120, 120, 120, 120, 120, 120, 120,
		120, 120, 120, 120, 120, 120, 120, 120, 120, 120,
		120, 120, 120, 120, 120, 120, 120, 120, 120, 120,
		120, 120, 120, 120, 120, 120, 120, 120, 120, 120,
		120
	}
};

/* 261~321MHz, 309MHz */
HUINT8  isdbtRfAgcGain2[2][41] =
{
	{
		78, 81, 84, 87, 90, 93, 97, 100, 103, 107,
		110, 113, 116, 120, 122, 125, 128, 131, 134, 137,
		139, 142, 144, 147, 149, 152, 154, 156, 158, 161,
		163, 165, 167, 169, 172, 174, 176, 179, 182, 184,
		188
	},
	{
		74, 77, 80, 83, 86, 89, 93, 96, 99, 103,
		106, 109, 113, 116, 119, 122, 125, 128, 131, 134,
		136, 139, 141, 144, 146, 149, 151, 154, 156, 158,
		161, 163, 165, 167, 170, 172, 175, 177, 180, 183,
		186
	}
};
HUINT8  isdbtIfAgcGain2[2][41] =
{
	{
		120, 120, 120, 120, 120, 120, 120, 120, 120, 120,
		120, 120, 120, 120, 120, 120, 120, 120, 120, 120,
		120, 120, 120, 120, 120, 120, 120, 120, 120, 120,
		120, 120, 120, 120, 120, 120, 120, 120, 120, 120,
		120
	},
	{
		120, 120, 120, 120, 120, 120, 120, 120, 120, 120,
		120, 120, 120, 120, 120, 120, 120, 120, 120, 120,
		120, 120, 120, 120, 120, 120, 120, 120, 120, 120,
		120, 120, 120, 120, 120, 120, 120, 120, 120, 120,
		120
	}
};

/* 327~465MHz, 393MHz */
HUINT8  isdbtRfAgcGain3[2][41] =
{
	{
		68, 71, 74, 77, 79, 83, 86, 90, 93, 97,
		100, 104, 107, 111, 114, 118, 121, 125, 128, 131,
		134, 137, 140, 143, 145, 148, 151, 151, 155, 158,
		160, 162, 165, 167, 170, 172, 175, 177, 180, 184,
		187
	},
	{
		65, 68, 71, 74, 76, 79, 82, 85, 89, 93,
		97, 100, 104, 108, 111, 115, 118, 122, 125, 129,
		132, 135, 138, 141, 143, 146, 148, 151, 153, 156,
		159, 161, 163, 166, 168, 170, 173, 176, 178, 182,
		186
	}
};
HUINT8  isdbtIfAgcGain3[2][41] =
{
	{
		120, 120, 120, 120, 120, 120, 120, 120, 120, 120,
		120, 120, 120, 120, 120, 120, 120, 120, 120, 120,
		120, 120, 120, 120, 120, 120, 120, 120, 120, 120,
		120, 120, 120, 120, 120, 120, 120, 120, 120, 120,
		120
	},
	{
		120, 120, 120, 120, 120, 120, 120, 120, 120, 120,
		120, 120, 120, 120, 120, 120, 120, 120, 120, 120,
		120, 120, 120, 120, 120, 120, 120, 120, 120, 120,
		120, 120, 120, 120, 120, 120, 120, 120, 120, 120,
		120
	}
};

/* 473~551MHz, 515MHz */
HUINT8  isdbtRfAgcGain4[2][41] =
{
	{
		55, 58, 61, 64, 66, 69, 71, 75, 79, 82,
		87, 91, 96, 101, 106, 110, 115, 120, 125, 129,
		133, 136, 140, 144, 147, 150, 153, 156, 158, 161,
		164, 167, 169, 171, 174, 177, 179, 182, 185, 188,
		194
	},
	{
		51, 53, 56, 59, 62, 64, 67, 70, 73, 77,
		81, 85, 89, 95, 99, 104, 109, 115, 120, 124,
		128, 132, 136, 140, 143, 147, 150, 153, 156, 159,
		161, 164, 167, 169, 172, 175, 178, 181, 185, 188,
		194
	}
};
HUINT8  isdbtIfAgcGain4[2][41] =
{
	{
		120, 120, 120, 120, 120, 120, 120, 120, 120, 120,
		120, 120, 120, 120, 120, 120, 120, 120, 120, 120,
		120, 120, 120, 120, 120, 120, 120, 120, 120, 120,
		120, 120, 120, 120, 120, 120, 120, 120, 120, 120,
		120
	},
	{
		120, 120, 120, 120, 120, 120, 120, 120, 120, 120,
		120, 120, 120, 120, 120, 120, 120, 120, 120, 120,
		120, 120, 120, 120, 120, 120, 120, 120, 120, 120,
		120, 120, 120, 120, 120, 120, 120, 120, 120, 120,
		120
	}
};

/* 557~767MHz, 677MHz */
HUINT8  isdbtRfAgcGain5[2][41] =
{
	{
		46, 49, 52, 55, 56, 58, 60, 62, 63, 66,
		68, 71, 74, 77, 81, 86, 91, 96, 103, 109,
		116, 122, 128, 134, 139, 144, 148, 151, 154, 158,
		162, 165, 167, 171, 174, 177, 180, 183, 187, 191,
		196
	},
	{
		50, 53, 56, 59, 61, 63, 65, 68, 70, 73,
		77, 81, 85, 90, 96, 101, 107, 113, 120, 125,
		131, 136, 141, 145, 149, 153, 156, 159, 163, 166,
		169, 171, 174, 177, 180, 183, 187, 191, 197, 215,
		220
	}
};
HUINT8  isdbtIfAgcGain5[2][41] =
{
	{
		120, 120, 120, 120, 120, 120, 120, 120, 120, 120,
		120, 120, 120, 120, 120, 120, 120, 120, 120, 120,
		120, 120, 120, 120, 120, 120, 120, 120, 120, 120,
		120, 120, 120, 120, 120, 120, 120, 120, 120, 120,
		120
	},
	{
		120, 120, 120, 120, 120, 120, 120, 120, 120, 120,
		120, 120, 120, 120, 120, 120, 120, 120, 120, 120,
		120, 120, 120, 120, 120, 120, 120, 120, 120, 120,
		120, 120, 120, 120, 120, 120, 120, 120, 120, 120,
		123
	}
};

/* 167MHz only */
HUINT8  isdbtRfAgcGain167[2][41] =
{
	{
		85, 98, 101, 104, 107, 109, 112, 115, 117, 120,
		123, 125, 128, 131, 133, 136, 138, 141, 143, 146,
		148, 150, 153, 155, 157, 159, 161, 163, 166, 168,
		170, 172, 175, 177, 180, 182, 185, 187, 190, 193,
		197
	},
	{
		93, 96, 99, 102, 105, 108, 111, 113, 116, 119,
		121, 124, 127, 129, 132, 134, 137, 139, 141, 144,
		146, 149, 151, 153, 155, 158, 160, 162, 165, 166,
		169, 171, 173, 175, 178, 180, 183, 185, 188, 191,
		195
	}
};
HUINT8  isdbtIfAgcGain167[2][41] =
{
	{
		116, 116, 116, 116, 116, 116, 116, 116, 116, 116,
		116, 116, 116, 116, 116, 116, 116, 116, 116, 116,
		116, 116, 116, 116, 116, 116, 116, 116, 116, 116,
		116, 116, 116, 116, 116, 116, 116, 116, 116, 116,
		116
	},
	{
		116, 116, 116, 116, 116, 116, 116, 116, 116, 116,
		116, 116, 116, 116, 116, 116, 116, 116, 116, 116,
		116, 116, 116, 116, 116, 116, 116, 116, 116, 116,
		116, 116, 116, 116, 116, 116, 116, 116, 116, 116,
		116
	}
};




static HUINT8 tc90507Reg01; /* 01번지 레지스터는 두개의 다른 기능을 포함한다. 각 기능의 설정값을 모두 기억한다. */
static HUINT8 tc90507Reg71; /* 71번지 레지스터는 두개의 다른 기능을 포함한다. 각 기능의 설정값을 모두 기억한다. */

HUINT32 gBERCalMode[BUS_NUM] = {0}; 	 /* BER 계산 모드 */
HUINT32 gBERDisplayPeriod[BUS_NUM] = {2};	 /* BER 디스플레이 주기 */

static int Drv_Ch_I2C_DemodRead(int nUnitId, HUINT8 ucRegAddr, HUINT8 *pucBuffer, HUINT32 nNumberToRead);
static  int Drv_Ch_I2C_DemodWrite(int nUnitId,  HUINT8 ucRegAddr, HUINT8 *pucBuffer, HUINT32 nNumberToWrite);
static int Drv_Ch_I2C_TunerRead(int nUnitId, HUINT8 ucRegAddr, HUINT8 *pucBuffer, HUINT32 nNumberToRead);
static int Drv_Ch_I2C_TunerWrite(int nUnitId, HUINT8 ucRegAddr, HUINT8 *pucBuffer, HUINT32 nNumberToRead);

static void ResetSystem(int nChannelId);
//static void ResetDemodulation(HUINT32 nChannelId);
//static void ResetFFTWndSearch(HUINT32 nChannelId);
static void Set_OperationMode(int nChannelId);
static void Set_Standby(int nChannelId);
static void Set_InputOutput(int nChannelId);
static void Set_Clock(int nChannelId);
static void Set_AGC_DigitalFilter(int nChannelId);
static void Set_CarrierRecovery(int nChannelId);
static void Set_Synchronization(int nChannelId);
static void Set_CPERemoval(int nChannelId);
static void Set_CVI_CSI(int nChannelId);
static void Set_Equalization(int nChannelId);
static void Set_TMCCDetection(int nChannelId);
static void Set_ErrorCorrection(int nChannelId);
static void Set_ErrorCorrection4ChPerformanceTest(int nChannelId);
static void Set_TMCCDecodedData(int nChannelId);


/*******************************************************************************
* function: Drv_Ch_I2C_DemodRead
* input:
*	- nChannelId : 버스 번호
* output: No
* return: No
*******************************************************************************/
static int Drv_Ch_I2C_DemodRead(int nUnitId, HUINT8 ucRegAddr, HUINT8 *pucBuffer, HUINT32 nNumberToRead)
{
	int nResult = DI_CH_OK;
	DRV_Error nRet = DRV_OK;
	HUINT16	ucDevAddr = TC90507_DEMOD_ADDRESS;
	HUINT32	ucI2CBus = 0;

	CH_UNUSED(nUnitId);

	if(ucRegAddr > TC90507_FE)
	{
		/* Reg Address Error */
		CH_PrintErr("[Drv_Ch_I2C_DemodRead]Reg Address Error!");
	}

	nRet = DRV_I2c_ReadA8( ucI2CBus, ucDevAddr, ucRegAddr, pucBuffer, nNumberToRead );
	if ( nRet != DRV_OK )
	{
		CH_PrintErr(" Error[0x%x]", nRet);
		nResult = DI_CH_ERR;
	}
	else
	{
		//CH_PrintDebug("[0x%x] RegAddr [0x%x] RData [0x%x] \n", ucDevAddr, ucRegAddr, *pucBuffer );
	}

	return nResult;

}


static int Drv_Ch_I2C_DemodWrite(int nUnitId,  HUINT8 ucRegAddr, HUINT8 *pucBuffer, HUINT32 nNumberToWrite)
{
	int nResult = DI_CH_OK;
	DRV_Error nRet = DRV_OK;

	HUINT16	ucDevAddr = TC90507_DEMOD_ADDRESS;
	HUINT32	ucI2CBus = 0;

	CH_UNUSED(nUnitId);

	if(ucRegAddr > TC90507_FE)
	{
		/* Reg Address Error */
		CH_PrintErr("DemodWrite]Reg Address Error!");
	}

	nRet = DRV_I2c_WriteA8(ucI2CBus, ucDevAddr, ucRegAddr, pucBuffer, nNumberToWrite);
	if ( nRet != DRV_OK )
	{
		CH_PrintErr(" Error[0x%x]", nRet);
		nResult = DI_CH_ERR;
	}
	else
	{
		//CH_PrintDebug(" Success \n");
		//CH_PrintDebug("[0x%x] RegAddr [0x%x] WData [0x%x] \n", ucDevAddr, ucRegAddr, *pucBuffer );
	}

	return nResult;

}

/*******************************************************************************
* function: Drv_Ch_I2C_TunerRead
* input:
*	- nChannelId : 버스 번호
* output: No
* return: No
*******************************************************************************/
static int Drv_Ch_I2C_TunerRead(int nUnitId, HUINT8 ucRegAddr, HUINT8 *pucBuffer, HUINT32 nNumberToRead)
#if 1
{
	int nResult = DI_CH_OK;
	DRV_Error nRet = DRV_OK;
	HUINT16	ucDevAddr = TC90507_DEMOD_ADDRESS;
	HUINT32	ucI2CBus = 0;


	HUINT16	ucThruRegAddr = 0xfec3;

	CH_UNUSED(nUnitId);

	if(ucRegAddr != ISDBT_TD1686_ADDRESS)
	{
		/* Reg Address Error */
		CH_PrintErr (" Reg Address Error!n");
	}
	else
	{
		//CH_PrintDebug("  [0x%x] \n", (HUINT16)ucThruRegAddr );
	}

	nRet = DRV_I2c_ReadA16( ucI2CBus, ucDevAddr, ucThruRegAddr, pucBuffer, nNumberToRead );
	if ( nRet != DRV_OK )
	{
		CH_PrintErr(" Error[0x%x]", nRet);
		nResult = DI_CH_ERR;
	}
	else
	{
		//CH_PrintDebug(" ucReadVal [0x%x]\n", *pucBuffer);
	}

	return nResult;

}
#else
{
	int nResult = DI_CH_OK;
	DRV_Error nRet = DRV_OK;
	HUINT16	ucDevAddr = TC90507_DEMOD_ADDRESS;
	HUINT32	ucI2CBus = 0;

	HUINT8	tnflg = 0xfe;	/* tnflg : 1111 1110 */

	HUINT8	ucThruRegAddr = 0xc3;


	CH_UNUSED(nUnitId);

	if(ucRegAddr != ISDBT_TD1686_ADDRESS)
	{
		/* Reg Address Error */
		CH_PrintErr (" Reg Address Error!n");
	}
	else
	{
		//CH_PrintDebug("  [0x%x] \n", (HUINT16)ucThruRegAddr );
	}

	nRet = DRV_I2c_WriteA8( ucI2CBus, ucDevAddr, tnflg, &ucThruRegAddr, nNumberToRead );
	if ( nRet != DRV_OK )
	{
		CH_PrintErr(" Error[0x%x]", nRet);
		nResult = DI_CH_ERR;
	}

	nRet = DRV_I2c_ReadNoAddr( ucI2CBus, ucDevAddr, pucBuffer, nNumberToRead );
	if ( nRet != DRV_OK )
	{
		CH_PrintErr(" Error[0x%x]", nRet);
		nResult = DI_CH_ERR;
	}
	else
	{
		//CH_PrintDebug(" ucReadVal [0x%x]\n", *pucBuffer);
	}

	return nResult;

}
#endif


static int Drv_Ch_I2C_TunerWrite(int nUnitId,  HUINT8 ucRegAddr, HUINT8 *pucBuffer, HUINT32 nNumberToWrite)
#if 1
{
	int nResult = DI_CH_OK;
	DRV_Error nRet = DRV_OK;

	HUINT16	ucDevAddr = TC90507_DEMOD_ADDRESS;
	HUINT32	ucI2CBus = 0;

	HUINT16	ucThruRegAddr = 0xfec2;

	//int i=0;

	CH_UNUSED(nUnitId);

	if(ucRegAddr != ISDBT_TD1686_ADDRESS)
	{
		/* Reg Address Error */
		CH_PrintErr (" Reg Address Error!n");
	}
	else
	{
		/* Make 16bit Register Address (tnflg + Target Address) */
		//ucRegAddr = (HUINT16)0xfec2;//((((HUINT16)tnflg << 8) & 0xff00) | (ucRegAddr & 0x00ff));
		CH_PrintDebug(" [0x%04x] \n",(HUINT16)ucThruRegAddr );
	}

	/*
	for (i=0; i<nNumberToWrite; i++)
		CH_PrintDebug("[0x%x] \n", *(pucBuffer+i) );
	*/

	nRet = DRV_I2c_WriteA16(ucI2CBus, ucDevAddr, ucThruRegAddr, pucBuffer, nNumberToWrite);
	if ( nRet != DRV_OK )
	{
		CH_PrintErr(" Error[0x%x]", nRet);
		nResult = DI_CH_ERR;
	}
	else
	{
		CH_PrintDebug(" Success \n");
	}

	return nResult;

}
#else
{
	int nResult = DI_CH_OK;
	DRV_Error nRet = DRV_OK;

	HUINT16	ucDevAddr = TC90507_DEMOD_ADDRESS;
	HUINT32	ucI2CBus = 0;

	HUINT8	tnflg = 0xfe;	/* tnflg : 1111 1110 */

	HUINT16	ucThruRegAddr = 0x60;

	HUINT8	ucBuffer[5];
	HUINT32 i, nNumWrite = 5;

	CH_UNUSED(nUnitId);


	nRet = DRV_I2c_WriteNoAck( ucI2CBus, ucDevAddr, tnflg, &ucThruRegAddr, 1 );
	//nRet = DRV_I2c_WriteA8( ucI2CBus, ucDevAddr, tnflg, &ucThruRegAddr, 1 );

	nRet = DRV_I2c_WriteNoAddr(ucI2CBus, ISDBT_TD1686_ADDRESS, pucBuffer, nNumberToWrite);
	if ( nRet != DRV_OK )
	{
		CH_PrintErr(" Error[0x%x]", nRet);
		nResult = DI_CH_ERR;
	}
	else
	{
		CH_PrintDebug(" Success \n");
	}

	return nResult;

}
#endif

void DT90507_InitRegister(HUINT32 nChannelId)
{
	int i;
	HUINT8	val;

	HUINT8	TC90507[0xff] =
	{
	/*
	0x00,0x01,0x02,0x03,0x04,0x05,0x06,0x07,0x08,0x09,0x0a,0x0b,0x0c,0x0d,0x0e,0x0f
	*/
	0x99,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x99,0x99,0x99,0x00,0x99,0x99,0x99,	// 0
	0x99,0x21,0x0b,0x2c,0x10,0x40,0x02,0x70,0x31,0x13,0x31,0x13,0x00,0x00,0x00,0x00,	// 1
	0x4d,0xff,0x00,0x4c,0x4c,0x00,0x00,0x0c,0x60,0x6b,0x40,0x40,0xff,0x00,0xff,0x00,	// 2
	0x28,0x0f,0x9f,0x99,0x3f,0x99,0x99,0x99,0x01,0x5c,0x10,0x30,0x00,0x10,0x08,0x0c,	//3// 3
	0x0c,0x00,0x00,0x4f,0xff,0x99,0x20,0x00,0x90,0xe6,0x02,0x54,0x00,0x99,0x99,0x99,	// 4
	0x04,0x58,0x20,0x99,0x57,0xf1,0x20,0x70,0x60,0x99,0x99,0x99,0x50,0x00,0x99,0x99,	// 5
	0x99,0x99,0x99,0x99,0x99,0x99,0x99,0x99,0x99,0x99,0x99,0x99,0x99,0x99,0x99,0x99,	// 6
//	0x18,0x00,0x00,0x99,0x99,0x00,0x02,0x00,0x99,0x99,0x99,0x99,0x00,0x52,0x99,0x99,	// 7
	0x18,0x00,0x00,0x99,0x99,0x00,0x32,0x00,0x99,0x99,0x99,0x99,0x00,0x52,0x99,0x99,	// 7   0x76:0x02->0x32 = 512->4096
	0x99,0x99,0x99,0x99,0x99,0x99,0x99,0x99,0x99,0x99,0x99,0x99,0x99,0x99,0x99,0x99,	// 8
	0x99,0x99,0x99,0x99,0x99,0x99,0x99,0x99,0x99,0x99,0x99,0x99,0x99,0x99,0x99,0x99,	// 9
	0x99,0x99,0x99,0x99,0x99,0x99,0x99,0x99,0x99,0x99,0x99,0x99,0x99,0x99,0x99,0x99,	// a
	0x00,0x02,0x3c,0x70,0x6f,0xff,0xff,0xff,0xff,0xfc,0x00,0x00,0x00,0x99,0x99,0x99,	// b
	0x99,0x99,0x10,0x99,0x99,0x99,0x99,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,	// c
	0x99,0x99,0x99,0x99,0x99,0x99,0x99,0x99,0x99,0x99,0x00,0x99,0x99,0x99,0x99,0x99,	// d
	0x99,0x99,0x99,0x99,0x1a,0x99,0x99,0x99,0x99,0x99,0x99,0x99,0x00,0x99,0x99,0x00,	// e
	0x99,0x99,0x99,0x99,0x99,0x99,0x99,0x99,0x99,0x99,0x99,0x99,0x99,0x99,0x00		// f
	};

	CH_PrintMon ("DT90507_InitRegister()\n");

	for(i=0; i<TC90507_FE; i++)
	{
		if(TC90507[i] !=	0x99)
		{
			val = TC90507[i];
			CH_PrintMon("DT90507_Register_Init :: DT90507[%x]=[%x]\n", i, val);
			Drv_Ch_I2C_DemodWrite(nChannelId, i, &val, 1);
		}
	}
}

int DT90507_DumpRegister( int nChannelId)
{
	int i=0;
	unsigned char ucdata;

	//nUnitId = GetDeviceUnitId( nChannelId );

	CH_PrintMon("===================================================\n");
	CH_PrintMon("   TC90507 Demod [%d] Dump\n", nChannelId);
	CH_PrintMon("===================================================\n");

	for(i=0; i<= TC90507_FE; i++)
	{
		Drv_Ch_I2C_DemodRead(nChannelId,  i, &ucdata, 1);
		CH_PrintMon("[0x%02X] \t[0x%02x]\n", i, ucdata);
	}

	CH_PrintMon("===================================================\n");
	CH_PrintMon("   OFDM TUNER [%d] Dump\n", nChannelId);
	CH_PrintMon("===================================================\n");

	//for(i=0; i<= 0x3C; i++)
	{
		Drv_Ch_I2C_TunerRead(nChannelId,  ISDBT_TD1686_ADDRESS, &ucdata, 1);
		CH_PrintMon("[0x%02x]\n", ucdata);
	}
	CH_PrintMon("===================================================\n");

	return DI_CH_OK;

}


/*******************************************************************************
* function: ResetSystem
* description: System Reset
* input:
*	- nChannelId : 버스 번호
* output: No
* return: No
*******************************************************************************/
void ResetSystem(int nChannelId)
{
	HUINT8	isyrst = 0x80;		/* isyrst[0x01, D7] = 1 */

	isyrst |= tc90507Reg01;
	Drv_Ch_I2C_DemodWrite(nChannelId, TC90507_01, &isyrst, 1);
}

#if (0)
/*******************************************************************************
* function: ResetDemodulation
* description: Demodulation Reset
* input:
*	- nChannelId : 버스 번호
* output: No
* return: No
*******************************************************************************/
void ResetDemodulation(HUINT32 nChannelId)
{
	HUINT8	imsrst = 0x40;		/* imsrst[0x01, D6] = 1 */

	imsrst |= tc90507Reg01;
	Drv_Ch_I2C_DemodWrite(nChannelId, TC90507_01, &imsrst, 1);
}

/*******************************************************************************
* function: ResetFFTWndSearch
* description: FFT Window Search Reset
* input:
*	- nChannelId : 버스 번호
* output: No
* return: No
*******************************************************************************/
void ResetFFTWndSearch(HUINT32 nChannelId)
{
	HUINT8	iwsrst = 0x10;		/* iwsrst[0x01, D4] = 1 */

	iwsrst |= tc90507Reg01;
	Drv_Ch_I2C_DemodWrite(nChannelId, TC90507_01, &iwsrst, 1);
}
#endif

/*******************************************************************************
* function: Set_OperationMode
* description: Operation Mode를 설정한다.
* input:
*	- nChannelId : 버스 번호
* output: No
* return: No
*******************************************************************************/
void Set_OperationMode(int nChannelId)
{
	HUINT8	recvmd = 0x0;			/* recvmd[0x02, D7:D6] = 0 */
	HUINT8	segsel = 0x0;			/* segsel[0x02, D3:D0] = 0 */
	HUINT8	recvmdsel = 0x0;		/* recvmdsel[0x02, D3] = 0 */
	HUINT8	laysel = 0x0;			/* laysel[0x02, D2:D0] = 0 */
	HUINT8	regVal;

	regVal = ((recvmd << 6) |
			segsel);
	tc90507Reg71 |= laysel;

	Drv_Ch_I2C_DemodWrite(nChannelId,  TC90507_02, &regVal, 1);
	Drv_Ch_I2C_DemodWrite(nChannelId,  TC90507_EC, &recvmdsel, 1);
	Drv_Ch_I2C_DemodWrite(nChannelId,  TC90507_71, &tc90507Reg71, 1);
}
/*******************************************************************************
* function: Set_Standby
* description: Standby mode를 설정한다.
* input:
*	- nChannelId : 버스 번호
* output: No
* return: No
*******************************************************************************/
void Set_Standby(int nChannelId)
{
	HUINT8	slpmd = 0x0;			/* slpmd[0x01, D3] */
	HUINT8	slplkmon = 0x0;			/* slplkmon[0x01, D2] */
	HUINT8	slptim = 0x0;			/* slptim[0x03, D6:D4] */
	HUINT8	wuptim = 0x0;			/* wuptim[0x03, D3:D0] */
	HUINT8	adcpwn = 0x0;			/* adcpwn[0x04, D5] */
	HUINT8	regVal;

	regVal = ((slpmd << 3) |
			(slplkmon << 2));
	Drv_Ch_I2C_DemodWrite(nChannelId,  TC90507_01, &regVal, 1);

	regVal = ((slptim << 4) |
			wuptim);
	Drv_Ch_I2C_DemodWrite(nChannelId,  TC90507_03, &regVal, 1);

	regVal = (adcpwn << 5);
	Drv_Ch_I2C_DemodWrite(nChannelId,  TC90507_04, &regVal, 1);
}

void Set_InputOutput(int nChannelId)
{
	HUINT8	isdbt[4];
	/*	[0x5] STSFLG1 output setting			  7 			6		 5		   4			3		 2		  1 	   0
			D7:D0='0': Disable, '1': Enable (retryov,alalrm,tmunvld,mdunvld,fulock,vulock,rulock,rseorf)
			[0x6] STSFLG0 output setting
			D7:D3='0':Disable, '1':Enable (emerg,tmcchg,cdunvld,slpen,STSFLG1)
		[0x7] STSFLG1 output polarity
			D7:D0='0': Normal, '1': Invert
		[0x8] STSFLG0 output polarity
			D7:D3='0': Normal, '1': Invert
		[0xc] STSFLG1,STSFLG0 output mode
			D7:D6='0': Normal value 1, D5:D4='0': Normal value 1
			D3:D2='0': FLOCK, D1:D0='0':RLCOK/RERR */
	/* TS input setting */
		isdbt[0]=0x00;	// [0x05]
		isdbt[1]=0x00; // ssen 0x40;
		isdbt[2]=0x00;
		isdbt[3]=0x00;
		Drv_Ch_I2C_DemodWrite(nChannelId,  TC90507_05, &isdbt[0], 4);

	/* TS output setting */
	#if (0) //Parallel mode (SSEN)
		isdbt[0]=0x0a; // justin 20070111 0x00; // [0x1c] agccnti, agccntr, stsflg1, stsflg0 enable
		isdbt[1]=0xa0; // justin 20070111 0x00;  // [0x1d] rlock, RERR, RSOUT, HSEL enable parallel setting
		isdbt[2]=0x08;	// [0x1e] FLOCK, SLPEN, SRCK, RSCKO, serial disable
		isdbt[3]=0x00; // [0x1f] SBYTE, PBVAL, RSEORF
	#else // Serial mode (JC-3500)
		CH_PrintDebug("Set_InputOutput:: Serial Output \n");
		isdbt[0]=0x0a; // [0x1c] AGCCNTI, AGCCNTR enable, STSFLG0, STSFLG1 disable
		isdbt[1]=0x8a; // [0x1d] RLOCK, RSOUT, HSEL disable , REER enable
		isdbt[2]=0x02; // [0x1e] FLOCK, SLPEN, SRCK enable, PSCKO disable
		isdbt[3]=0x00; // [0x1f] SBYTE, PBVAL, RSEORF
	#endif
		Drv_Ch_I2C_DemodWrite(nChannelId, TC90507_1C, &isdbt[0], 4);
}
void Set_Clock(int nChannelId)
{
	HUINT8	isdbt[2];
	/* XT :17.467MHz, EXDIV＝XT ｘ lpdiv／MD, MD(Master CLK Freq)=69.868MHz
mdckiv reg10 [7] W 0x0 CKMD0 pin clock polarity inversion
	0: Forward phase, 1: Reverse phase
mdcksl[2:0] reg10 [6:4] W 0x0 CKMD0 pin clock output selection
	0:MD 1:MD/2 2:MD/4 3:MD/8
	4:FS 5:FS/2 6:FS/4 7:FS/8
fsckiv reg10 [3] W 0x0 CKFS0 pin clock polarity inversion
	0: Forward phase, 1: Reverse phase
fscksl[2:0] reg10 [2:0] W 0x7 CKFS0 pin clock output selection
	0:MD 1:MD/2 2:MD/4 3:MD/8
	4:FS 5:FS/2 6:FS/4 7:FS/8
mlpdiv reg11 [6] W 0x0 Flag setting that enables setting of lpdiv with I2C
	0: Disabled, 1: Enabled
lpdiv[5:0] reg11 [5:0] W 0x21 Clock dividing ratio (station originated frequency side) setting
	Divising ratio n=1 to 63
mexdiv reg12 [6] W 0x0 Flag setting that enables setting of exdiv with I2C
	0: Disabled, 1: Enabled
exdiv[4:0] reg12 [4:0] W 0x00 Clock dividing ratio (reference frequency side) setting
	Dividing ratio n=1 to 31

hkfrq[15:8] reg13 [7:0] W 0x03 Sampling clock frequency ratio setting
	(MD/FS-1) ｘ 216 (FS: Data rate frequency, MD: Master clock frequency)
	Set 0x40 or above.
hkfrq[7:0] reg14 [7:0] W 0xe0

hkncog[1:0] reg15 [7:6] W 0x1 Clock frequency control range setting (See Expression 7-5.)
	0: x1(±400ppm) 1: x1/2(±200ppm)
	2: x1/4(±100ppm) 3: x1/8(±50ppm)
adcken reg16 [7] W 0x0 AD clock (CKAD0 pin) output enabled setting
	0: Disabled, 1: Enabled
hkpwiv reg16 [4] W 0x0 Clock error PWM polarity inversion setting
	0: Normal, 1: Inverted
iadckiv reg16 [3] W 0x0 AD clock polarity inversion setting
	0: Forward phase, 1: Reverse phase
clkout regc1 [3] W 0x0 Clock output enable setting
	0: Disabled, 1: Enabled
	ｈｋｆｒｑ＝（ＭＤ／ＦＳ－１）× 2XY16
	hkfrq=0x131b=4891, FS(data freq)=65.0158MHz,
	The frequency control range Δｆ／ｆ (see Note 1) is calculated with
	Δｆ／ｆ＝（１－（ｈｋｆｒｑ＋2XY16）
	／（ｈｋｆｒｑ＋２XY（５－ｈｋｎｃｏｇ）＋2XY16）× １０XY６［ｐｐｍ］
	=(1-(70427/70443))*10XY6=227ppm */
	isdbt[0]=0x21;	  // PLL setting
	isdbt[1]=0x0b;
	Drv_Ch_I2C_DemodWrite(nChannelId, TC90507_11, &isdbt[0], 2);

	if( HKFRQ == 1)
	{
		isdbt[0]=0x2c;
		isdbt[1]= ghkfrq;
	}
	else
	{
		isdbt[0]=0x2c;
		isdbt[1]=0x0a; // justin_070115 0x10;	// default 0x0a;
	}
	Drv_Ch_I2C_DemodWrite(nChannelId, TC90507_13, &isdbt[0], 2);

	isdbt[0]=0x40;
	isdbt[1]=0x02;
	Drv_Ch_I2C_DemodWrite(nChannelId, TC90507_15, &isdbt[0], 2);
}


void Set_AGC_DigitalFilter(int nChannelId)
{
	HUINT8	isdbt[7];
	HUINT8	val;

	if(isdb_agc_test == 1)
	{
		isdbt[0] = isdb_if_min;
		isdbt[1] = isdb_rf_max;
	}
	else
	{

            if(0)// gBoardRev == BOARD_REV_0_55)
            {
        		// isdbt[0]=0xa0; // 2.1v;	// [0x20] agc change over point if min
        		// isdbt[0]=0x50; // 1.0v;	// [0x20] agc change over point if min

        		if ( gTC90507Channel[nChannelId].frequency < 168000000 )
        		{
        			isdbt[0] = 120;

        			if((gTC90507Channel[nChannelId].frequency == 167000000) || (gTC90507Channel[nChannelId].frequency == 167143000))
        			{
        				isdbt[0] = 116;
        			}
        		}
        		else if (gTC90507Channel[nChannelId].frequency <= 473000000)
        		{
        			isdbt[0] = 120;
        		}
        		else if (gTC90507Channel[nChannelId].frequency < 767000000)
        		{
        			isdbt[0] = 120;
        		}
        		else
        		{
        			isdbt[0] = 120;
        		}
        		isdbt[1]=0xff;	// [0x21] RF MAX

            }
            else
            {
                    // isdbt[0]=0xa0; // 2.1v;  // [0x20] agc change over point if min
                    // isdbt[0]=0x50; // 1.0v;  // [0x20] agc change over point if min

                    if ( gTC90507Channel[nChannelId].frequency <= 167000000 )
                    {
#if(DEMOD_INDIVIDUAL_RESET)
                        isdbt[0] = 120;
#else
                        isdbt[0] = 0x60; // 1.3v
#endif
                    }
                    else if (gTC90507Channel[nChannelId].frequency <= 473000000)
                    {
#if(DEMOD_INDIVIDUAL_RESET)
                        isdbt[0] = 120;
#else
                        isdbt[0] = 0x60; // 1.3v
#endif
                    }
                    else if (gTC90507Channel[nChannelId].frequency < 767000000)
                    {
#if(DEMOD_INDIVIDUAL_RESET)
                        isdbt[0] = 120;
#else
                        isdbt[0] = 0x60; // 1.3v
#endif
                    }
                    else
                    {
#if(DEMOD_INDIVIDUAL_RESET)
                        isdbt[0] = 120;
#else
                        isdbt[0] = 0x60; // 1.3v
#endif
                    }
                    isdbt[1]=0xff;  // [0x21] RF MAX

            }
	}

	isdbt[2]=0x00;	// [0x22] dual agc, agc operating clock 32MHz
	isdbt[3]=0x4c;	// [0x23] IF agc agin & polarity
	isdbt[4]=0x4c;	// [0x24] RF agc gain & polarity, manual agc default 0x4c
	isdbt[5]=0x00;	// [0x25] if gain manual   ssen 0x4c
	isdbt[6]=0x00;	// [0x26] rf gain manual
	Drv_Ch_I2C_DemodWrite(nChannelId,	TC90507_20, &isdbt[0], 7);

	val = 0x60; 	// [0x28] if agc gain th
	Drv_Ch_I2C_DemodWrite(nChannelId, TC90507_28, &val, 1);

	val = 0x40; 	// [0x2b]rf agc gain th
	Drv_Ch_I2C_DemodWrite(nChannelId,	TC90507_2B, &val, 1);

	isdbt[0]=0xff; // 0xc8; // [0x2c] weak signal level
	isdbt[1]=0x00; // 0x34;  // [0x2d] strong signal level
	if(isdb_agc_test == 1)
	{
		isdbt[2] = isdb_if_max;
		isdbt[3] = isdb_rf_min;
	}
	else
	{
		isdbt[2]=0xff;	// [0x2e] if max
		isdbt[3]=0x00;	// [0x2f] rf min
	}
	Drv_Ch_I2C_DemodWrite(nChannelId,	TC90507_2C, &isdbt[0], 4);

	val = 0x3f; 	// digital filter
	Drv_Ch_I2C_DemodWrite(nChannelId,	TC90507_34, &val, 1);
}

void Set_CarrierRecovery(int nChannelId)
{
	HUINT8	val;

	//Drv_Ch_I2C_DemodWrite(TC90507_30,0x28,1);
	/* frequency IFlow=4.063492 MHz
	Frequency correction ＝(Master clock frequency ＭＤ÷１６)－ＩＦｌｏｗ［Ｈｚ］
	If I/Q detection frequency inversion register ｆ_ｉｎｖ＝０,
	ｃｐｌｄ_ｄｔ＝Frequency correction volume÷９９２ */
	//Drv_Ch_I2C_DemodWrite(TC90507_CARROFF1,0x00,1);
	/* Frequency correction=(69.868MHz/16)-4.063492MHz
	ｃｐｌｄ_ｄｔ＝ 303258/992=306=0x0131*/
	//Drv_Ch_I2C_DemodWrite(TC90507_CARROFF2,0xea,1);

	if(AFC30 == 1)
	{
		val=gafc;
	}
	else
	{
		val= 0x28;
	}
	Drv_Ch_I2C_DemodWrite(nChannelId, TC90507_30, &val, 1);

	val=0x0f; // 0x0d;
	Drv_Ch_I2C_DemodWrite(nChannelId, TC90507_31, &val, 1);

	if(AFC32 == 1)
	{
		val=gafc32;  // cpld_dt
	}
	else
	{
		val= 0x0a; // justin_070115 0x9f;	// default 0xa0
	}
	Drv_Ch_I2C_DemodWrite(nChannelId,	TC90507_32, &val, 1);

	val=0x01; // 0x0d;
	Drv_Ch_I2C_DemodWrite(nChannelId,	TC90507_38, &val, 1);

	if(AFC39 == 1)
	{
		val=gafc39;
	}
	else
	{
		val= 0x70; // justin_070115 0x5d;	// default 0x70
	}
	Drv_Ch_I2C_DemodWrite(nChannelId,	TC90507_39, &val, 1);
}

void Set_Synchronization(int nChannelId)
{
	HUINT8	isdbt[9];
	HUINT8	val;

	/* initial waiting time until demod start is set*/
	isdbt[0]=0x10;isdbt[1]=0x30;isdbt[2]=0x00;
	isdbt[3]=0x10;isdbt[4]=0x08;isdbt[5]=0x0c;
	isdbt[6]=0x0c;isdbt[7]=0x00;isdbt[8]=0x00;
	Drv_Ch_I2C_DemodWrite(nChannelId, TC90507_3A, &isdbt[0], 9);

	isdbt[0]=0x4f;isdbt[1]=0xff;
	Drv_Ch_I2C_DemodWrite(nChannelId, TC90507_43, &isdbt[0], 2);

	isdbt[0]=0x20;isdbt[1]=0x00;isdbt[2]=0x90;
	isdbt[3]=0xe6;isdbt[4]=0x02;isdbt[5]=0x54;
	Drv_Ch_I2C_DemodWrite(nChannelId, TC90507_46, &isdbt[0], 6);

	val=0x20; // sssen 0x02;
	Drv_Ch_I2C_DemodWrite(nChannelId, TC90507_B1, &val, 1);

	val=0x00;
	Drv_Ch_I2C_DemodWrite(nChannelId, TC90507_BC, &val, 1);

	val=0x1a;	// FFT update period, s/n calculate number
	Drv_Ch_I2C_DemodWrite(nChannelId,	TC90507_E4, &val, 1);
}


void Set_CPERemoval(int nChannelId)
{
	HUINT8 val = 0x00;

	Drv_Ch_I2C_DemodWrite(nChannelId, TC90507_50, &val, 1);
}

void Set_CVI_CSI(int nChannelId)
{
	HUINT8	isdbt[5];
	HUINT8	val;

	val=0x68;
	Drv_Ch_I2C_DemodWrite(nChannelId, TC90507_51, &val, 1);

	val=0x20;
	Drv_Ch_I2C_DemodWrite(nChannelId, TC90507_52, &val, 1);

	isdbt[0]=0x57;	// [0x54]
	isdbt[1]=0xf1;
	isdbt[2]=0x20;
	isdbt[3]=0x70;
	isdbt[4]=0x60; // ssen 0x20;
	Drv_Ch_I2C_DemodWrite(nChannelId, TC90507_54, &isdbt[0], 5);
}

void Set_Equalization(int nChannelId)
{
	HUINT8	val=0x50;

	Drv_Ch_I2C_DemodWrite(nChannelId, TC90507_5C, &val, 1);
}

void Set_TMCCDetection(int nChannelId)
{
	HUINT8	val=0x00;

	Drv_Ch_I2C_DemodWrite(nChannelId, TC90507_5D, &val, 1);
}

void Set_ErrorCorrection4ChPerformanceTest(int nChannelId)
{
	HUINT8	isdbt[3];

	isdbt[0]=0x18;
	isdbt[1]=0x00;
	isdbt[2]=0x00;
	Drv_Ch_I2C_DemodWrite(nChannelId, TC90507_70, &isdbt[0], 3);

	isdbt[0]=0x00;
	isdbt[1]=0x32; // ssen 0x38;	// default 0x02 cyc : 3 => 16frames
	isdbt[2]=0x03;					//	Measurement cycle setting : Time mode, After Viterbi
	Drv_Ch_I2C_DemodWrite(nChannelId, TC90507_75, &isdbt[0], 3);
}

void Set_ErrorCorrection(int nChannelId)
{
	HUINT8	isdbt[3];

	isdbt[0]=0x18;
	isdbt[1]=0x00;
	isdbt[2]=0x00;
	Drv_Ch_I2C_DemodWrite(nChannelId, TC90507_70, &isdbt[0], 3);

	isdbt[0]=0x00;
	isdbt[1]=0x02; // ssen 0x38;	// default 0x00 cyc : 0 => 2frames
	isdbt[2]=0x03;					//	Measurement cycle setting : Time mode, After Viterbi
	Drv_Ch_I2C_DemodWrite(nChannelId, TC90507_75, &isdbt[0], 3);
}


void Set_TMCCDecodedData(int nChannelId)
{
	HUINT8	isdbt[8];
	HUINT8	val;

	val=0x00;
	Drv_Ch_I2C_DemodWrite(nChannelId, TC90507_B0, &val, 1);

	isdbt[0]=0x3d;	//[0xb2]
	isdbt[1]=0x25;
	isdbt[2]=0x8b;
	isdbt[3]=0x4b;
	isdbt[4]=0x3f;
	isdbt[5]=0xff;
	isdbt[6]=0xff;
	isdbt[7]=0xff;	//[0xb9]
	Drv_Ch_I2C_DemodWrite(nChannelId,  TC90507_B2, &isdbt[0], 8);
}

 void DRV_T_InstallApi( void )
 {
	 TER_InitDevice 		 = &DT90507_InitDevice;
	 TER_SetTune			 = &DT90507_SetTune;
	 TER_CheckLock			 = &DT90507_CheckLock;

	 TER_GetSignalInfo  = &DT90507_GetSignalInfo;
	 TER_SetPowerState		 = &DT90507_SetPowerState;
	 TER_EnableTsOutput 	 = &DT90507_EnableTsOutput;
	 TER_DisableTsOutput	 = &DT90507_DisableTsOutput;

	 TER_CheckAntState	 = &DT90507_CheckAntState;
	 TER_SetAntState	 = &DT90507_SetAntState;
	 TER_GetAntState	 = &DT90507_GetAntState;
	 TER_ProcessRecoverAntenna = &DT90507_ProcessRecoverAntenna;


	 TER_ResetGpio			 = &DT90507_ResetGpio;
	 TER_Initialize			 = &DT90507_Initialise;

 	CH_PrintDebug("[DT90507] DRV_T_InstallApi()  OK! \n");

	 return;
 }

 int DT90507_InitDevice(int nChannelId)
 {
	 CH_UNUSED(nChannelId);

	 DRV_GPIO_Write(GPIO_ID_RST_OFDM, GPIO_LOW);
	 CH_PrintDebug("OFDM Reset Clear \n");

	 return DI_CH_OK;
 }

 int DT90507_SetTune( int nChannelId, HUINT8 *bChecklock , HUINT32 unWorkingId )
 {

	 int				 nResult = DI_CH_OK;
	 unsigned char		 lock_ofdm = 0;

	CH_UNUSED(unWorkingId);

	//DT90507_ResetGpio(nChannelId);
	 //DT90507_Initialise(nChannelId);
	 //CH_PrintDebug("TC90507_Initialise OK ...\n");

 	nResult = DT90507_Lock(nChannelId, &lock_ofdm);

	*bChecklock = lock_ofdm;

	return nResult;
 }

 void DT90507_ResetGpio(int nChannelId)
 {
	 CH_UNUSED(nChannelId);

 	DRV_GPIO_Write(GPIO_ID_RST_QAM, GPIO_LOW);
	 VK_TASK_Sleep(30);

	 DRV_GPIO_Write(GPIO_ID_SEL_DEMOD, GPIO_HI);
	 CH_PrintDebug(" Set TD1686 TER Mode\n");

	 DRV_GPIO_Write(GPIO_ID_RST_OFDM, GPIO_HI);
	 VK_TASK_Sleep(30);
}

 void DT90507_Initialise(int nChannelId)
{
	sTsmfEmgOffFlag[nChannelId] = 1;

/* RESET */
	ResetSystem(nChannelId);
	VK_TASK_Sleep(5);

/* operating mode setting */
	Set_OperationMode(nChannelId);
//	VK_TASK_Sleep(5);

/* sleep mode setting */
	Set_Standby(nChannelId);
//	VK_TASK_Sleep(5);

/* I/O output setting */
	Set_InputOutput(nChannelId);
//	VK_TASK_Sleep(5);

/* Clock setting */
	Set_Clock(nChannelId);
//	VK_TASK_Sleep(5);

/* AGC setting */
	Set_AGC_DigitalFilter(nChannelId);
//	VK_TASK_Sleep(5);

/* carrier recovery */
	Set_CarrierRecovery(nChannelId);
//	VK_TASK_Sleep(5);

/* synchronization sequence & FFT window mornitor control */
	Set_Synchronization(nChannelId);
//	VK_TASK_Sleep(5);

/* CPE removal */
	Set_CPERemoval(nChannelId);
//	VK_TASK_Sleep(5);

/* CVI/CSI	*/
	Set_CVI_CSI(nChannelId);
//	VK_TASK_Sleep(5);

/* C/N monitor & TMCC  */
	Set_Equalization(nChannelId);
//	VK_TASK_Sleep(5);

/* TMCC detection/AC output */
	Set_TMCCDetection(nChannelId);
//	VK_TASK_Sleep(5);

/* FEC */
	if(gBERCalMode[nChannelId] == 0)		/* Normal Case */
	{
		Set_ErrorCorrection(nChannelId);
	}
	else if(gBERCalMode[nChannelId] == 1)	/* For Channel Performance Test */
	{
		Set_ErrorCorrection4ChPerformanceTest(nChannelId);
	}
	else
	{
		/* 지원하지 않는 상태 */
		CH_PrintErr("TC90507_Init:: Set_ErrorCorrection Error: Wrong BER Calculation mode.");
	}
//	VK_TASK_Sleep(5);

/* TMCC Decoded Data */
	Set_TMCCDecodedData(nChannelId);
	VK_TASK_Sleep(5);

	//CH_PrintDebug("TC90507_Initialise OK ...\n");
}

 static int DT90507_SetTuner(int nChannelId, HUINT32 frequency)
 {
 	 HUINT8	 tunerLockFlag = 0;
	 HUINT8	 tuningTrial = 0;


	 /* ISDBT용 주파수로 전환 */
	 if(((frequency -143000) / 1000000) != (frequency /1000000)) /* 만약 143MHz가 안붙어서 오는 경우 */
	 {
		 frequency += 143000;
		 CH_PrintDebug(" TC90507_SetTuner[%d]:: Modifyed ISDB Freq = %d\n", nChannelId, frequency);
	 }
	 else	 /* 143MHz가 붙어서 오는 경우 */
	 {
		 CH_PrintDebug(" TC90507_SetTuner[%d]:: ISDB Freq = %d\n", nChannelId, frequency);
	 }

	 /* Try Tuning */
	 while( (tuningTrial < 3) && !tunerLockFlag )
	 {
		 tunerLockFlag = Set_TD1686Tuner(nChannelId, frequency);	 /* TD1686 튜너를 셋팅 한다. 다른 튜너가 오면 여기에 위치시킨다. */
		 tuningTrial++;
	 }

	 gTC90507Channel[nChannelId].frequency = frequency;  /* TODO : frequency를 클로벌로 관리 안할 수도 있으므로 체크한다. */

	 if(tunerLockFlag == 1)
	 {
		 CH_PrintDebug(" TC90507_SetTuner[%d]:: Tuner Lock Ok!\n", nChannelId);
		 return DI_CH_OK;
	 }
	 else
	 {
		 CH_PrintDebug(" TC90507_SetTuner[%d]:: Tuner Lock Fail!\n", nChannelId);
		 return DI_CH_ERR;
	 }
 }

 int  DT90507_GetSignalInfo(int nChannelId, DI_CH_SignalQuality_t *pstSignalInfo)
 {
	 HUINT8 ucRfAgc=0, ucIfAgc=0;
	 HINT16 ucStrength=0;
	 HUINT16 ucSnr=0;
	 HUINT32 ulQuality=0;
	 HUINT8	ucEmergencyFlag;

	 DT90507_GetAGC( nChannelId, &ucRfAgc, &ucIfAgc );
 	 DT90507_GetChData(nChannelId, &ucStrength, &ulQuality, &ucSnr, &ucEmergencyFlag);

	 pstSignalInfo->ulStrength	 = (HUINT32)ucStrength;
	 pstSignalInfo->ulAgc		 = (HUINT32)ucRfAgc;
	 pstSignalInfo->ulQuality	 = (HUINT32)ulQuality;

	 pstSignalInfo->fSnr = (float)ucSnr;

	 CH_PrintDebug("[DT90507] ulStrength =%d , ulAgc = %d, ulQuality = %d, fSnr = %f \n ",
					 pstSignalInfo->ulStrength,pstSignalInfo->ulAgc, pstSignalInfo->ulQuality, pstSignalInfo->fSnr);

	 return DI_CH_OK;

 }



void DT90507_SetBERCalTime(int nChannelId, HUINT32 berCalMode)
{
	if(berCalMode == 0)
	{
		gBERCalMode[nChannelId] = 0;	/* Normal(Default) case (1초)*/
		gBERDisplayPeriod[nChannelId] = 2;	/* 500ms * 2 = 1(s) */
		totalCntToDisplayBER[nChannelId] = 0;
	}
	else if(berCalMode == 1)
	{
		gBERCalMode[nChannelId] = 1;	/* Channel Performance Test (8초) */
		gBERDisplayPeriod[nChannelId] = 16;		/* 500ms * 16 = 8(s) */
		totalCntToDisplayBER[nChannelId] = 0;
	}
	else
	{
		/* Error : Set to default mode */
		CH_PrintErr("TC90507_SetBERCalTime:: Error: Wrong BER Calculation mode.");
		gBERCalMode[nChannelId] = 0;	/* Other case (1초)*/
		gBERDisplayPeriod[nChannelId] = 2;	/* 500ms * 2 = 1(s) */
		totalCntToDisplayBER[nChannelId] = 0;
	}
}

static int DT90507_Lock( int nChannelId, unsigned char *bChecklock )
{
	int 	nResult = DI_CH_OK;
	int	nUnitId=0;

	unsigned int		frequency;

	CH_TER_TuneParam_t *pstTerTuneParam;

	nUnitId = GetDeviceUnitId( nChannelId );

	pstTerTuneParam = (CH_TER_TuneParam_t *)GetWorkingTuneInfo( nChannelId );

	/* Get the tuning data loaded from application to channel device */
	frequency = pstTerTuneParam->ulFrequency * KHZ_TO_HZ ;
	CH_PrintDebug("unitId(%d), frequency(%dL)\n", nUnitId, frequency);

	/* TC90507 Activation */
	ActivateIsdbDemod(nUnitId);

	gTC90507Channel[nUnitId].frequency = frequency;
	gCurrentId[nUnitId] = 0;
	beforeId2[nUnitId] = 0;

	CH_PrintDebug("PAL_ISDBT_Lock:: Unit[%d], freq= %d\n",  nUnitId, gTC90507Channel[nUnitId].frequency);

	if (save_stream == 1)
	{
		nResult = DT90507_SetTuner(nUnitId, frequency);
		if(nResult != DI_CH_OK)
		{
			CH_PrintErr("TC90507_Lock:: ERR! TC90507_SetTuner");
			return nResult;
		}

		DT90507_Initialise(nUnitId);
		CheckLock(nUnitId);

		return DI_CH_OK;
	}

	nResult = DT90507_SetTuner(nUnitId, frequency);
	if(nResult != DI_CH_OK)
	{
		CH_PrintErr("TC90507_Lock:: ERR! TC90507_SetTuner");
		return nResult;
	}

	DT90507_Initialise(nUnitId);

	nResult = CheckLock(nUnitId);

	if(nResult == DI_CH_OK) /* 파라미터 검사 OK */
	{
		//nResult = DI_CH_OK;
		*bChecklock = 1;
		CH_PrintDebug("PAL_ISDBT_Lock[%d]:: Lock OK\n", nUnitId);
	}
	else /* 파라미터 오류 */
	{
		*bChecklock = 0;
		CH_PrintDebug("PAL_ISDBT_Lock[%d]:: Lock Fail\n", nUnitId);
	}

	//DRV_CH_UnLock( nChannelId );

	return nResult;
}

void DT90507_Stop(int nChannelId)
{
	CH_PrintDebug("TC90507_Stop:: Unit[%d]\n",	nChannelId);

	gTC90507Channel[nChannelId].strength = 0;
	gTC90507Channel[nChannelId].quality = 0;
	gTC90507Channel[nChannelId].snr = 0;
	gTC90507Channel[nChannelId].emergencyFlag = 0;
	totalCntToDisplayBER[nChannelId] = 0;

	if(save_stream != 1)
	{
		DeactivateIsdbDemod(nChannelId);
	}
}

 int DT90507_CheckStatus ( int nChannelId)
{
	int 	nResult = DI_CH_OK;

	gCurrentId[nChannelId] = 1;

	nResult = DT90507_GetStatus(nChannelId);

	gDT90507EmgOnFlag[nChannelId] = -1;

	if( gTC90507Channel[nChannelId].emergencyFlag == 1)
	{
		sTsmfEmgOffFlag[nChannelId] = TRUE;
		if((sTsmfEmgOnFlag[nChannelId] == FALSE) || (beforeId2[nChannelId] != gCurrentId[nChannelId]) )
		{
			sTsmfEmgOnFlag[nChannelId] = TRUE;
			beforeId2[nChannelId] = gCurrentId[nChannelId];
			//GWM_SendMessage(MSG_CH_EWS_ON, versionNumber, CH_MOD_OFDM, 0);

			gDT90507EmgOnFlag[nChannelId] = TRUE;
			CH_PrintDebug("TC90507_CheckStatus:: TSMF Emergency On .....\n\n");
		}
	}
	else
	{
		sTsmfEmgOnFlag[nChannelId] = FALSE;
		if((sTsmfEmgOffFlag[nChannelId] == TRUE) || (beforeId2[nChannelId] != gCurrentId[nChannelId]) )
		{
			sTsmfEmgOffFlag[nChannelId] = FALSE;
			beforeId2[nChannelId] = gCurrentId[nChannelId];
			//GWM_SendMessage(MSG_CH_EWS_OFF, versionNumber, CH_MOD_OFDM, 0);
			gDT90507EmgOnFlag[nChannelId] = FALSE;
			CH_PrintDebug("TC90507_CheckStatus:: TSMF Emergency Offl .....\n\n");
		}
	}

	if(nResult == DI_CH_OK) /* locking 상태 이면 */
	{
		CH_PrintDebug("TC90507_CheckStatus[%d]:: Locking \n", nChannelId);
	}
	else /* unlock 상태이면 */
	{
		nResult = DI_CH_ERR;
		CH_PrintDebug("TC90507_CheckStatus[%d]:: Unlock \n", nChannelId);
	}

	return nResult;
 }

HBOOL DT90507_CheckLock ( int nChannelId, HUINT32 unWorkingID )
{
	int 	nResult = DI_CH_OK;

	CH_UNUSED(unWorkingID);

	gCurrentId[nChannelId] = 1;

	nResult = DT90507_GetStatus(nChannelId);
/*
	if( gTC90507Channel[nChannelId].emergencyFlag == 1)
	{
		sTsmfEmgOffFlag[nChannelId] = TRUE;
		if((sTsmfEmgOnFlag[nChannelId] == FALSE) || (beforeId2[nChannelId] != gCurrentId[nChannelId]) )
		{
			sTsmfEmgOnFlag[nChannelId] = TRUE;
			beforeId2[nChannelId] = gCurrentId[nChannelId];
			//GWM_SendMessage(MSG_CH_EWS_ON, versionNumber, CH_MOD_OFDM, 0);
			CH_PrintDebug("TC90507_CheckStatus:: TSMF Emergency On .....\n\n");
		}
	}
	else
	{
		sTsmfEmgOnFlag[nChannelId] = FALSE;
		if((sTsmfEmgOffFlag[nChannelId] == TRUE) || (beforeId2[nChannelId] != gCurrentId[nChannelId]) )
		{
			sTsmfEmgOffFlag[nChannelId] = FALSE;
			beforeId2[nChannelId] = gCurrentId[nChannelId];
			//GWM_SendMessage(MSG_CH_EWS_OFF, versionNumber, CH_MOD_OFDM, 0);
			CH_PrintDebug("TC90507_CheckStatus:: TSMF Emergency Offl .....\n\n");
		}
	}
*/
	if(nResult == DI_CH_OK) /* locking 상태 이면 */
	{
		CH_PrintDebug("TC90507_CheckStatus[%d]:: Locking \n", nChannelId);
		return TRUE;
	}
	else /* unlock 상태이면 */
	{
		nResult = DI_CH_ERR;
		CH_PrintDebug("TC90507_CheckStatus[%d]:: Unlock \n", nChannelId);
		return FALSE;
	}

 }

 void DT90507_GetTsmfStatus(int nChannelId, int *tsmfStatus)
 {
	*tsmfStatus = gDT90507EmgOnFlag[nChannelId];
 }

int DT90507_CheckTsmfStatus(int nChannelId)
 {
	int 	nResult = DI_CH_OK;

	nResult = DT90507_GetStatus(nChannelId);

	if(nResult != DI_CH_OK) /* unlock 상태이면 */
	{
		nResult = DI_CH_ERR;
		CH_PrintDebug("TC90507_CheckTsmfStatus[%d]:: Unlock \n", nChannelId);
	}
	else /* locking 상태 이면 */
	{
		CH_PrintDebug("TC90507_CheckTsmfStatus[%d]:: Lock \n", nChannelId);
	}

	return nResult;
 }

static int CheckLock(int nChannelId)
{
	int 	nResult = DI_CH_OK;

	if(FastLockIsdb(nChannelId))
	{
		nResult = DI_CH_OK;
		Start_Accu(nChannelId);		/* Locking되면 Accumulation 시작 */
		Reset_Accu(nChannelId);		/* 초기값 설정 */
		CH_PrintDebug("CheckLock:: FastLockIsdb is OK \n");
	}
	else
	{
		nResult = DI_CH_ERR;
		CH_PrintErr("CheckLock:: FastLockIsdb is ERROR\n");
	}

	return nResult;
}

void DT90507_GetEwsInfo(int nChannelId, HUINT8 *ucIsEws)
{
	*ucIsEws = gTC90507Channel[nChannelId].emergencyFlag;
	CH_PrintDebug("TC90507_GetEwsInfo:: EmergencyFlag = 0x%x\n", gTC90507Channel[nChannelId].emergencyFlag);
}


static int DT90507_GetStatus(int nChannelId)
{
	HUINT8	GetFECLock;
	HUINT8	rfAgc, ifAgc;
	HUINT8	berI;
	HUINT8	val;

	/* Check Emergency Flag */
	Drv_Ch_I2C_DemodRead(nChannelId,  TC90507_81, &val, 1);
	gTC90507Channel[nChannelId].emergencyFlag = (val >> 7);
	CH_PrintDebug("TC90507_CheckStatus::  EmergencyFlag = 0x%x\n", gTC90507Channel[nChannelId].emergencyFlag);

	Drv_Ch_I2C_DemodRead(nChannelId,   TC90507_83, &rfAgc, 1);//RF_AGC
	Drv_Ch_I2C_DemodRead(nChannelId,   TC90507_82, &ifAgc, 1);//IF_AGC

	/* RF Unlock */
	if( (rfAgc == 0xff) && (ifAgc == 0xff) )
	{
		CH_PrintDebug("TC90507_GetStatus[%d]:: RF_AGC = 0x%x (%d), IF_AGC= 0x%x (%d)\n", nChannelId, rfAgc,rfAgc,ifAgc, ifAgc);
		gTC90507Channel[nChannelId].strength = 0;
		gTC90507Channel[nChannelId].quality = 0;
		gTC90507Channel[nChannelId].snr = 0;
		gTC90507Channel[nChannelId].emergencyFlag = 0;
		totalCntToDisplayBER[nChannelId] = 0;
		CH_PrintErr("TC90507_CheckStatus:: Error!! RF Unlock!!\n");
		if(accuFlag[nChannelId] == 1)
		{
			Reset_Accu(nChannelId);
		}
		//CH_PrintMon("OFDM Demod TC905007 UNLOCK ! \n");
		return DI_CH_ERR;
	}

	/* Read the SYNC status register */
	/* Viterbi error -> RS error -> TS packet error  */
	Drv_Ch_I2C_DemodRead(nChannelId,  TC90507_80, &val, 1);
	CH_PrintDebug("TC90507_CheckStatus::  SB_SYNC = 0x%x\n", val);
	GetFECLock = val&0x07;

	/* Demodulator Unlock */
	if ( GetFECLock == 0x07 )
	{
		gTC90507Channel[nChannelId].strength = 0;
		gTC90507Channel[nChannelId].quality = 0;
		gTC90507Channel[nChannelId].snr = 0;
		gTC90507Channel[nChannelId].emergencyFlag = 0;
		totalCntToDisplayBER[nChannelId] = 0;
		CH_PrintDebug("TC90507_CheckStatus:: Error!!Demodulator Unlock!!\n");
		if(accuFlag[nChannelId] == 1)
		{
			Reset_Accu(nChannelId);
		}
		//CH_PrintMon("OFDM Demod TC905007 UNLOCK ! \n");
		return DI_CH_ERR;
	}

	CH_PrintDebug("TC90507_GetStatus[%d]:: RF_AGC = 0x%x (%d), IF_AGC= 0x%x (%d)\n", nChannelId, rfAgc,rfAgc,ifAgc, ifAgc);

	/* Get Uncorrected Packet of A,B,C Layer */
	GetUnCorrectedPacket(nChannelId, &gTC90507Correction[nChannelId].UnCorrectedNo[0]);
	/* Get Unerrored Packet of A,B,C Layer */
	GetUnErroredPacket (nChannelId, &gTC90507Correction[nChannelId].UnErroredNo[0]);

	for(berI = 0; berI < ISDBT_LAYER_NUM; berI++)
	{
		gTC90507Correction[nChannelId].CorrectedNo[berI] = 0;

		GetIsdbtBer(nChannelId, &gTC90507Correction[nChannelId].InbandBer[berI], gTC90507Correction[nChannelId].UnCorrectedNo[berI], gTC90507Correction[nChannelId].UnErroredNo[berI], &gTC90507Correction[nChannelId].UnCorrectedFlag[berI]);
		if(totalCntToDisplayBER[nChannelId] == 0)	/* 500ms * 16 = 8sec */
		{
			CH_PrintDebug("Time = %d, Uncorrected pkt(Accu)[%d] = %d(%d), UnErroredNo pkt=%d, BER = %d (%d)\n", accuCnt[nChannelId]*4,
				berI, gTC90507Correction[nChannelId].UnCorrectedNo[berI], accuUnCorPkt[nChannelId][berI], gTC90507Correction[nChannelId].UnErroredNo[berI],
				gTC90507Correction[nChannelId].InbandBer[berI], gTC90507Correction[nChannelId].UnCorrectedFlag[berI]);
		}
	}

	gTC90507Channel[nChannelId].strength = GetIsdbStrength(nChannelId);
	gTC90507Channel[nChannelId].snr = GetOfdmCn(nChannelId);

	/* A,B,C Layer 의 BER중 가장 높은 값(가장 안좋은 BER)을 quality에 넣는다. */
	gTC90507Channel[nChannelId].quality = gTC90507Correction[nChannelId].InbandBer[0];
	for(berI = 1; berI < ISDBT_LAYER_NUM; berI++)
	{
		if(gTC90507Channel[nChannelId].quality < gTC90507Correction[nChannelId].InbandBer[berI])
		{
			gTC90507Channel[nChannelId].quality = gTC90507Correction[nChannelId].InbandBer[berI];
		}
	}
	if(totalCntToDisplayBER[nChannelId] == 0)	/* 500ms * 16 = 8sec */
	{
		CH_PrintDebug("TC90507_CheckStatus[%d]:: Strength = %d, Quality = %d, SNR = %d\n", nChannelId, gTC90507Channel[nChannelId].strength ,gTC90507Channel[nChannelId].quality, gTC90507Channel[nChannelId].snr);
	}

	if(accuFlag[nChannelId] == 1)
	{
		if((totalCntToDisplayBER[nChannelId] % 8) == 0)
		{
			accuCnt[nChannelId]++;
			for(berI = 0; berI < ISDBT_LAYER_NUM; berI++)
			{
				accuUnCorPkt[nChannelId][berI] += gTC90507Correction[nChannelId].UnCorrectedNo[berI];
			}
		}
	}

	totalCntToDisplayBER[nChannelId]++;

	if(totalCntToDisplayBER[nChannelId] == gBERDisplayPeriod[nChannelId])	/* 500ms * 16 = 8sec */
	{
		totalCntToDisplayBER[nChannelId] = 0;
	}

	//CH_PrintMon("OFDM Demod DT90507 LOCK ! \n");

	return DI_CH_OK;
}


void TC90507_ReadTMCC(int nChannelId)
{
	HUINT8	data[8];
	HUINT8	temp;
	int		i;

	Drv_Ch_I2C_DemodRead(nChannelId,  TC90507_B2, &data[0], 8);

	for(i = 0; i < 8; i++)
	{
		CH_PrintDebug("[TMCC] Data[%d] : %x\n", i, data[i]);
	}

	temp = (HUINT8)((data[0] & 0xC0) >> 6);	/* B2[7:6] */
	CH_PrintDebug("[TMCC]System ID : %x\n", temp);

	temp = (HUINT8)((data[0] & 0x3C) >> 2);	/* B2[5:2] */
	CH_PrintDebug("[TMCC]Changeover indicator : %x\n", temp);

	temp = (HUINT8)((data[0] & 0x02) >> 1);	/* B2[1] */
	CH_PrintDebug("[TMCC]Emergency Alarm flag : %x\n", temp);

	temp = (HUINT8)(data[0] & 0x01);			/* B2[0] */
	CH_PrintDebug("[TMCC]Partial reception flag : %x\n", temp);

	temp = (HUINT8)((data[1] & 0xE0) >> 5);	/* B3[7:5] */
	CH_PrintDebug("[TMCC]Layer A Modulation scheme : %x\n", temp);

	temp = (HUINT8)((data[1] & 0x1C) >> 2);	/* B3[4:2] */
	CH_PrintDebug("[TMCC]Layer A Convolution coding rate : %x\n", temp);

	temp = (HUINT8)(((data[1] & 0x03) << 1) | ((data[2] & 0x80) >> 7));	/* B3[1:0] & B4[7] */
	CH_PrintDebug("[TMCC]Layer A time interleave scheme : %x\n", temp);

	temp = (HUINT8)((data[2] & 0x78) >> 3);	/* B4[6:3] */
	CH_PrintDebug("[TMCC] # of segments used by layer A : %x\n", temp);

	temp = (HUINT8)(data[2] & 0x07);			/* B4[2:0] */
	CH_PrintDebug("[TMCC]Layer B Modulation scheme : %x\n", temp);

	temp = (HUINT8)((data[3] & 0xE0) >> 5);	/* B5[7:5] */
	CH_PrintDebug("[TMCC]Layer B Convolution coding rate : %x\n", temp);

	temp = (HUINT8)((data[3] & 0x1C) >> 2);	/* B5[4:2] */
	CH_PrintDebug("[TMCC]Layer B time interleave scheme : %x\n", temp);

	temp = (HUINT8)(((data[3] & 0x03) << 1) | ((data[4] & 0xC0) >> 6));	/* B5[1:0] & B6[7:6] */
	CH_PrintDebug("[TMCC] # of segments used by layer B : %x\n", temp);

	temp = (HUINT8)((data[4] & 0x38) >> 3);	/* B6[5:3] */
	CH_PrintDebug("[TMCC]Layer C Modulation scheme : %x\n", temp);

	temp = (HUINT8)(data[4] & 0x07);			/* B6[2:0] */
	CH_PrintDebug("[TMCC]Layer C Convolution coding rate : %x\n", temp);

	temp = (HUINT8)((data[5] & 0xE0) >> 5);	/* B7[7:5] */
	CH_PrintDebug("[TMCC]Layer C time interleave scheme : %x\n", temp);

	temp = (HUINT8)((data[5] & 0x1E) >> 1);	/* B7[4:1]  */
	CH_PrintDebug("[TMCC] # of segments used by layer C : %x\n", temp);
}


void DT90507_GetChData(int nChannelId, HINT16 *strength, HUINT32 *quality, HUINT16 *snr, HUINT8 *emergencyFlag)
{
	*strength = gTC90507Channel[nChannelId].strength;
	*quality = gTC90507Channel[nChannelId].quality;
	*snr = gTC90507Channel[nChannelId].snr;
	*emergencyFlag = gTC90507Channel[nChannelId].emergencyFlag;
}

void DT90507_GetCorrectionData(int nChannelId, HUINT32 *correctedNo, HUINT32 *unCorrectedNo, HUINT32 *unErroredNo, HUINT32 *inbandBer, HUINT32 *UnCorrectedFlag)
{
	HUINT8	i;

	for(i = 0; i < 3; i++)
	{
		correctedNo[i] = gTC90507Correction[nChannelId].CorrectedNo[i];
		unCorrectedNo[i] = gTC90507Correction[nChannelId].UnCorrectedNo[i];
		unErroredNo[i] = gTC90507Correction[nChannelId].UnErroredNo[i];
		inbandBer[i] = gTC90507Correction[nChannelId].InbandBer[i];
		UnCorrectedFlag[i] = gTC90507Correction[nChannelId].UnCorrectedFlag[i];
	}
}

void DT90507_GetAGC(int nChannelId, HUINT8 *ucpRFAgc, HUINT8 *ucpIFAgc)
{
	HUINT8	ucBuf[2];

	Drv_Ch_I2C_DemodRead(nChannelId,  TC90507_82, ucBuf, 2);	/* Read IF AGC & RF AGC */

	*ucpIFAgc =	ucBuf[0];
	*ucpRFAgc = ucBuf[1];
}


int  DT90507_SetPowerState( int nChannelId, CH_Power_e etPower )
{
	/*----------------------*/
	/* Implementation*/
	/*----------------------*/
	switch (etPower)
	{
		case CH_POWER_OFF:  //tmPowerStandby = 1

			SetActiveStatus( nChannelId, CH_ACTION_OFF );
			CH_PrintDebug("\n -------------CH_T_SetPowerState standby mode -------------\n");
			break;

		case CH_POWER_ON: //tmPowerOn = 0
	        default:

			SetActiveStatus( nChannelId, CH_ACTION_ON );
			CH_PrintDebug("\n -------------CH_T_SetPowerState Power on mode -------------\n");
			break;
	}


	return DI_CH_OK;
}


int DT90507_EnableTsOutput ( int nChannelId )
{
	CH_UNUSED(nChannelId);

	CH_PrintDebug("[DT90507] TS Enable \n" );

	return DI_CH_OK;
}

int DT90507_DisableTsOutput ( int nChannelId )
{
	CH_UNUSED(nChannelId);

	CH_PrintDebug("[DT90507] TS Disable \n");

	return DI_CH_OK;
}

void DT90507_ProcessRecoverAntenna( int nChannelId, HUINT32 unWorkingId )
{
	int nRet=0;
	int nUnitId;
	int nActiveState;

	nUnitId = GetDeviceUnitId( nChannelId );

#if 0 // nhlee delete temporary
	nRet = DRV_CH_SetGpio( GPIO_ANT_PWR_EN, GPIO_HI );
#endif
	if( nRet < 0 )
	{
		CH_PrintErr("");
	}

	VK_TASK_Sleep(100);// wait 100ms

#if 0 // nhlee delete temporary
	nRet = DRV_CH_GetGpio( GPIO_ANT_PWR_CHK );
#endif
	if( nRet == GPIO_LOW )
	{
		CH_PrintFunc("Antenna continue fault... Turn 5 Volt Off");

#if 0 // nhlee delete temporary
		nRet = DRV_CH_SetGpio( GPIO_ANT_PWR_EN, GPIO_LOW );
#endif
		if( nRet < 0 )
		{
			CH_PrintErr("");
		}

		DRV_CH_CallNotifyCallback( nChannelId, unWorkingId, DI_CH_SIGNAL_ANTENNA_FAULT);
		//s_stAntState[nUnitId].etCurrentAntState = DI_CH_ANTENNA_OFF;

	}
	else if( nRet == GPIO_HI )
	{
		CH_PrintFunc("Antenna recovered...");

		s_stAntState[nUnitId].etCurrentAntState = DI_CH_ANTENNA_ON;

		DRV_CH_CallNotifyCallback( nChannelId, unWorkingId, DI_CH_SIGNAL_ANTENNA_OK );

		nActiveState = GetActiveStatus( nChannelId );
		if( nActiveState == CH_ACTION_OFF )
		{
			SetTuneStatus( nChannelId, CH_STATUS_IDLE );
		}
		else if( nActiveState == CH_ACTION_ON )
		{
			SetTuneStatus( nChannelId, CH_STATUS_UNLOCK );
		}
		else
		{
			//Do nothing
		}

	}
	else
	{
		CH_PrintErr("");
	}

	return;

}


int DT90507_SetAntState( int nChannelId, DI_CH_AntStatus_e etAntState )
{
	int nUnitId;
	int nRet=0;
	HUINT32 unWorkingId;
	int nActiveState;

	nUnitId = GetDeviceUnitId( nChannelId );
	unWorkingId = GetRequestedId( nChannelId );

	s_stAntState[nUnitId].etDesiredAntState = etAntState;

	if( s_stAntState[nUnitId].etCurrentAntState != DI_CH_ANTENNA_FAULT )
	{
		if(etAntState < DI_CH_ANTENNA_FAULT)
		{
#if 0 // nhlee delete temporary
			nRet = DRV_CH_SetGpio( GPIO_ANT_PWR_EN, etAntState );
#endif
			if( nRet < 0 )
			{
				CH_PrintErr("");
				return DI_CH_ERR;
			}
		}
		else
		{
			CH_PrintErr("");
		}
		s_stAntState[nUnitId].etCurrentAntState = etAntState;

	}
	else if( s_stAntState[nUnitId].etCurrentAntState == DI_CH_ANTENNA_FAULT )
	{

		if( etAntState == DI_CH_ANTENNA_OFF )
		{
#if 0 // nhlee delete temporary
			nRet = DRV_CH_SetGpio( GPIO_ANT_PWR_EN, etAntState );
#endif
			if( nRet < 0 )
			{
				CH_PrintErr("");
				return DI_CH_ERR;
			}

			s_stAntState[nUnitId].etCurrentAntState = etAntState;

			DRV_CH_CallNotifyCallback( nChannelId, unWorkingId, DI_CH_SIGNAL_ANTENNA_OK );

			nActiveState = GetActiveStatus( nChannelId );
			if( nActiveState == CH_ACTION_OFF )
			{
				SetTuneStatus( nChannelId, CH_STATUS_IDLE );
			}
			else if( nActiveState == CH_ACTION_ON )
			{
				SetTuneStatus( nChannelId, CH_STATUS_UNLOCK );
			}
			else
			{
				//Do nothing
			}

		}
		else
		{
			return DI_CH_HW_RESTRICTION;
		}
	}

	return DI_CH_OK;

}


DI_CH_AntStatus_e DT90507_GetAntState( int nChannelId )
{
	int nUnitId;

	nUnitId = GetDeviceUnitId( nChannelId );

	return s_stAntState[nUnitId].etCurrentAntState;

}

int DT90507_CheckAntState( int nChannelId )
{
	int nUnitId;
	int nRet=0;

	nUnitId = GetDeviceUnitId( nChannelId );

	if( s_stAntState[nUnitId].etDesiredAntState == DI_CH_ANTENNA_ON )
	{
#if 0 // nhlee delete temporary
		nRet = DRV_CH_GetGpio( GPIO_ANT_PWR_CHK );
#endif
		if( nRet == GPIO_LOW )
		{
			CH_PrintFunc(">>>>>>>>>>>>Antenna short circuit\n");
			s_stAntState[nUnitId].etCurrentAntState = DI_CH_ANTENNA_FAULT;
			return DI_CH_ANTENNA_FAULT;
		}
		else if( nRet == GPIO_HI )
		{
			if( s_stAntState[nUnitId].etCurrentAntState == DI_CH_ANTENNA_FAULT )
			{
#if 0 // nhlee delete temporary
				nRet = DRV_CH_SetGpio( GPIO_ANT_PWR_EN, GPIO_HIGH );
#endif
				if( nRet < 0 )
				{
					CH_PrintErr("");
				}
			}

			return DI_CH_OK;

		}
		else
		{
			CH_PrintErr("");
			return DI_CH_ERR;
		}
	}
	else // DI_CH_ANTENNA_OFF, DI_CH_ANTENNA_FAULT
	{
		return DI_CH_OK;
	}
}

static HUINT8 Set_TD1686Tuner(int nChannelId, HUINT32 freq)
{
	HUINT16	divierRatio;
	HUINT8	tunerLockFlag;
	HUINT8	tuningData[4];
	//HUINT32	tempCnt = 0;

	CH_PrintDebug("Set_TD1686Tuner[%d]:: Selected tuner is TD1686SR freq = %d\n", nChannelId, freq);

	/* Set TD1686 Register */
	/* 1. Frequency Divider */
	divierRatio = (freq/142860.0) + (57000000/142860.0) +0.5;
	CH_PrintDebug ("Set_TD1686Tuner[%d] :: clockDivide = %d\n", nChannelId, divierRatio);

	tuningData[0] = (divierRatio>>8) & 0x007f;
	tuningData[1] = divierRatio & 0x00ff;

	/* 2. PLL Reference Divider : 142.86kHz */
	tuningData[2] = 0xc9;

	/* 3. Charge Pump & Band Selection */
	if((freq >= 93000000) && (freq <=168000000))
	{
		tuningData[3] = 0x21;	/* Low Band */

//		if(freq == 167000000)
//		tuningData[3] = 0x22;	/* Mid Band, for test */
	}
	else if((freq > 168000000) && (freq <=182000000))
	{
		if(freq <=169000000)
		{
			tuningData[3] = 0x41;
		}
		else
		{
			tuningData[3] = 0x42;
		}
	}
#if(0)		/* 채널팀 요구, 0.35 이전 버전엔 절대 반영하지 말것. */
	if((freq >= 93000000) && (freq <=168000000))
	{
		tuningData[3] = 0x21;
		if(freq >=167000000)
		{
			tuningData[3] = 0x24;	/* 밴드 -> High, 20070528, 채널팀 요구 반영 */
		}
	}
	else if((freq > 168000000) && (freq <=182000000))
	{
		if(freq <=169000000)
		{
//			tuningData[3] = 0x41;
			tuningData[3] = 0x24;	/* 밴드 -> High, 20070528, 채널팀 요구 반영 */
		}
		else
		{
			tuningData[3] = 0x42;
		}
	}
#endif
	else if((freq > 182000000) && (freq <=201000000))
	{
		tuningData[3] = 0x62;
	}
	else if((freq > 201000000) && (freq <=226000000))
	{
		tuningData[3] = 0x82;
	}
	else if((freq > 226000000) && (freq <=347000000))
	{
		tuningData[3] = 0x22;
	}
	else if((freq> 347000000) && (freq <=427000000))
	{
		tuningData[3] = 0x42;
	}
	else if((freq> 427000000) && (freq <=469000000))
	{
		tuningData[3] = 0x62;
	}
	else if((freq> 469000000) && (freq <=526000000))
	{
		tuningData[3] = 0x84;
	}
	else if((freq> 526000000) && (freq <=597000000))
	{
		tuningData[3] = 0x44;
	}
	else if((freq> 597000000) && (freq <=677000000))
	{
		tuningData[3] = 0x64;
	}
	else if((freq> 677000000) && (freq <=757000000))
	{
		tuningData[3] = 0x84;
	}
	else if((freq> 757000000) && (freq <=855000000))
	{
		tuningData[3] = 0xa4;
	}
	else
	{
		tuningData[3] = 0x81;
		CH_PrintDebug( "Set_TD1686Tuner:: ERROR : Out of frequency range for tuner\n");
	}

	if(chgCP == 1)
	{
		tuningData[3] = gValCP;
		CH_PrintDebug ("Set_TD1686Tuner[%d]:: Charge Pump changed [4] = %x\n", nChannelId, tuningData[3]);
	}
	else
	{
		CH_PrintDebug("Set_TD1686Tuner[%d]:: Original Charge Pump \n, nChannelId", nChannelId);
	}

	if(chgBand == 1)
	{
		tuningData[3] = (tuningData[3]&0xf8) |(gValBand&0x07);
		CH_PrintDebug ("Set_TD1686Tuner[%d]:: Band changed [4] = %x\n",nChannelId, tuningData[3]);
	}
	else
	{
		CH_PrintDebug("Set_TD1686Tuner[%d]:: Original Band \n", nChannelId);
	}

	CH_PrintDebug(" Set_TD1686Tuner[%d]::tuningData :[0]=0x%x, [1]=0x%x, [2]=0x%x, [3]=0x%x\n",
								nChannelId,tuningData[0],tuningData[1],tuningData[2],tuningData[3]);

	/* Write in the tuner */
	//while(1)
	{

	CH_PrintDebug("OFDM Tuner Write \n");
	Drv_Ch_I2C_TunerWrite(nChannelId,	 ISDBT_TD1686_ADDRESS, &tuningData[0], 4);

	//VK_TASK_Sleep(2000);
	}

	/* Wating for Tuning */
	VK_TASK_Sleep(60);		/* TODO : 이 시간값에 대한 적절한 값을 찾아야 한다. */

//while(1)
{
	/* Locking Check */
	Drv_Ch_I2C_TunerRead(nChannelId,  ISDBT_TD1686_ADDRESS, &tunerLockFlag, 1);
	CH_PrintDebug("Set_TD1686Tuner[%d]:: tunerRd=0x%x\n", nChannelId, tunerLockFlag);
	//VK_TASK_Sleep(2000);
}
	tunerLockFlag = (tunerLockFlag & 0x40) ? 1:0;		/* 1 : Locked, 0 : UnLocked */

#if(0)
		if (tunerLockFlag == 1)
		{
			CH_PrintDebug("Set_TD1686Tuner[%d]:: LOCK! PLL in the tuner was  locked.\n", nChannelId);
		}
		else
		{
			CH_PrintDebug("Set_TD1686Tuner[%d]:: Error! PLL in the tuner was not locked.\n",nChannelId);
		}
#endif
		return tunerLockFlag;

#if(0)		/* 튜너 락킹이 얼마나 빨리 되는지 테스트 하는 코드 */
	/* Write in the tuner */
	TC90507_I2C_TxThru(nChannelId,   ISDBT_TD1686_UNIT, &tuningData[0], 4);

	/* Wating for Tuning & Locking Check */
	/* VK_TASK_Sleep 없고, while(1)로 돌리면 대략 50 이내에서 락킹이 된다. 대략 두배로 잡은 카운트 동안 돌다가 락킹이 안되면 나온다. (tempCnt < 100) */
	/* VK_TASK_Sleep 있는 경우에는 부팅시 대략 7이내, 채널 체인지 시 2 이내에 락킹 된다. (tempCnt < 20) */
	while(tempCnt < 20)
	{
		TC90507_I2C_RxThru(nChannelId,  ISDBT_TD1686_UNIT, &tunerLockFlag, 1);
		tunerLockFlag = (tunerLockFlag & 0x40) ? 1:0;		/* 1 : Locked, 0 : UnLocked */
		if(tunerLockFlag == 1)
		{
			break;
		}
		tempCnt++;
		VK_TASK_Sleep(5);
	}
	CH_PrintMon("[Set_TD1686Tuner]-%d\n", tempCnt);
	tempCnt = 0;

	CH_PrintDebug("Set_TD1686Tuner[%d]:: tunerRd=0x%x\n", nChannelId, tunerLockFlag);

	return tunerLockFlag;
#endif
}

void ActivateIsdbDemod(int nChannelId)
{
#if(1)
	HUINT8	val;

	CH_PrintDebug("ActivateIsdbDemod:: Activation of TC9050\n");

	/* operating mode setting */
	/* received segment mode selection */
	val=0x00;
	Drv_Ch_I2C_DemodWrite(nChannelId, TC90507_02, &val, 1);

	/* layer replaced with null packets */
	val=0x00;
	Drv_Ch_I2C_DemodWrite(nChannelId, TC90507_71, &val, 1);

	/* received segment mode selection for 3 segment reception */
	val=0x00;
	Drv_Ch_I2C_DemodWrite(nChannelId, TC90507_EC, &val, 1);

	//D3='0':Fixed 3segment,D3='1':1segment or 3segment auto selection
	/*adc power on */
	val = 0x00;
	Drv_Ch_I2C_DemodWrite(nChannelId, TC90507_04, &val, 1);

	val = 0x00;
	Drv_Ch_I2C_DemodWrite(nChannelId, TC90507_03, &val, 1);

	VK_TASK_Sleep(100); /* this delay is necessary */
#endif
}

/* make demodulator idle at the STAND-BY mode */
void DeactivateIsdbDemod(int nChannelId)
{
#if(1)
	HUINT8 val;

	CH_PrintDebug("DeactivateIsdbDemod:: Power-Down mode\n");

	/* Sleep mode */
	val = 0x72;
	Drv_Ch_I2C_DemodWrite(nChannelId, TC90507_03, &val, 1);

	/*adc power down */
	val = 0x20;
	Drv_Ch_I2C_DemodWrite(nChannelId, TC90507_04, &val, 1);
#endif
}

#if(0)
void ClearIsdbDemod(HUINT32 nChannelId)
{
	HUINT8	val;
	CH_PrintMon(" ClearIsdbDemod ::\n");
	/* soft-reset the TC90507 */
	val = 0x40; // Demode Reset
	Drv_Ch_I2C_DemodWrite(nChannelId,   TC90507_01, &val, 1);
	VK_TASK_Sleep(30);
}

HUINT8	FastLockIsdbTest(HUINT32 nChannelId)
{
	HUINT8	seqVal;
	HUINT8	val;
	HUINT32	lockingCnt = 0;

	do
	{
		Drv_Ch_I2C_DemodRead(nChannelId, 	TC90507_B0, &seqVal, 1);
		seqVal &= 0x0F;
		CH_PrintDebug("FastLockIsdb[%d]::	try=%d, Sequence =0x%x\n", nChannelId, lockingCnt, seqVal);
		lockingCnt++;
		VK_TASK_Sleep(20);
	}while(seqVal < 0x09);

//	CH_PrintDebug ("FastLockIsdb[%d]::	try=%d, Sequence =0x%x\n", nChannelId, lockingCnt, seqVal));

	lockingCnt = 0;

	do
	{
		Drv_Ch_I2C_DemodRead(nChannelId, 	TC90507_80, &val, 1);
		CH_PrintDebug("FastLockIsdb[%d]::	try=%d, SYNC=0x%x\n", nChannelId, lockingCnt, val);
		lockingCnt++;
		VK_TASK_Sleep(20);
	}while(val != 0x00);

//	CH_PrintDebug ("FastLockIsdb[%d]::	try=%d, SYNC=0x%x\n", nChannelId, lockingCnt, val));

	return TRUE;
}
#endif
static HUINT8 FastLockIsdb(int  nChannelId)
{
	HUINT8	val1;//,val;
	HUINT8	count = 0;
	HUINT8	seqVal;
	HUINT8 lockedFlag = FALSE;
	int i; // jusitn

	gTC90507Channel[nChannelId].strength = 0;
	gTC90507Channel[nChannelId].quality = 0;
	gTC90507Channel[nChannelId].snr = 0;

	Drv_Ch_I2C_DemodRead(nChannelId, TC90507_B0, &seqVal, 1);
	CH_PrintDebug("FastLockIsdb[%d]::	Sequence =0x%x\n", nChannelId, seqVal&0x07);

	while (!lockedFlag)
	{
		CH_PrintDebug("FastLockIsdb[%d]:: Count %d \n", nChannelId, count);
		count++;

		gIsdbDemodStat = 1;

		if(gIsdbDemodStat == 1)
		{
			/* wait until AGC loop settles */

			/* Check AGC Level Detect */
			if( CheckIsdbAgcLock(nChannelId) )
			{
				CH_PrintDebug("FastLockIsdb[%d]::  AGC Level Detect >> Pass\n", nChannelId);

				/* go to next step */
				gIsdbDemodStat++;
			}
			else
			{
				CH_PrintDebug("FastLockIsdb[%d]::  AGC Level Detect >> Fail\n", nChannelId);
				return FALSE;
			}
		}

		VK_TASK_Sleep(200);

		Drv_Ch_I2C_DemodRead(nChannelId, TC90507_B0, &seqVal, 1);
		CH_PrintDebug("FastLockIsdb[%d]::	Sequence =0x%x\n", nChannelId, seqVal&0x07);

		if(gIsdbDemodStat == 2)
		{
			for (i=0; i<1; i++)  // justin
			{
				Drv_Ch_I2C_DemodRead(nChannelId, TC90507_80, &val1, 1);
				CH_PrintDebug("FastLockIsdb[%d]::  SYNC=0x%x\n", nChannelId, val1&0xff);
//				VK_TASK_Sleep(10);
			}
//#if(0)
			i = 0;
			while(i < 8)
			{
				i++;
				VK_TASK_Sleep(50);
				Drv_Ch_I2C_DemodRead(nChannelId, TC90507_80, &val1, 1);

				/* check TMCC Detect */
				if( (val1&0x20)==0x20 )
				{
					CH_PrintDebug("FastLockIsdb[%d]:: TMCC Detect >> Fail\n", nChannelId);
		//			Adjust_ISDB_TCarrierRecovBW();
//					VK_TASK_Sleep(10);
					lockedFlag = FALSE;
				}
				else
				{
					CH_PrintDebug("FastLockIsdb:: TMCC Detect >> Pass\n");
					gIsdbDemodStat++;
					break;
				}
			}
//#endif
		}
		Drv_Ch_I2C_DemodRead(nChannelId, TC90507_B0, &seqVal, 1);
		CH_PrintDebug("FastLockIsdb[%d]::	Sequence =0x%x\n", nChannelId, seqVal&0x07);

		if(gIsdbDemodStat == 3)
		{
			VK_TASK_Sleep(50);
			for (i=0; i<1; i++)
			{
				Drv_Ch_I2C_DemodRead(nChannelId, TC90507_80, &val1, 1);
				CH_PrintDebug("FastLockIsdb[%d]::  SYNC=0x%x\n", nChannelId, val1&0xff);
//				VK_TASK_Sleep(10);
			}

			if( (val1&0x10)==0x10 )
			{
				lockedFlag = FALSE;
				CH_PrintDebug("FastLockIsdb[%d]:: Receive Mode >> Fail\n", nChannelId);
			}
			else
			{
				/* Check Frame Sync  */
				if( (val1&0x08)==0x08 )
				{
					CH_PrintDebug("FastLockIsdb[%d]:: Frame Sync >> Fail\n", nChannelId);
					lockedFlag = FALSE;
//					VK_TASK_Sleep(10);
				}
				else
				{
					CH_PrintDebug("FastLockIsdb[%d]:: Frame Sync >> Pass\n", nChannelId);
					gIsdbDemodStat++;
				}
			}
		}

		Drv_Ch_I2C_DemodRead(nChannelId, TC90507_B0, &seqVal, 1);
		CH_PrintDebug("FastLockIsdb[%d]::	Sequence =0x%x\n", nChannelId, seqVal&0x07);

		if(gIsdbDemodStat == 4)
		{
			VK_TASK_Sleep(150);
			for(i=0;i<3;i++)
			{
				Drv_Ch_I2C_DemodRead(nChannelId, TC90507_80, &val1, 1);
				CH_PrintDebug("FastLockIsdb[%d]:: SYNC=0x%x\n", nChannelId, val1&0xff);

				if( (val1&0x01)==0x01 )
				{
					CH_PrintDebug("FastLockIsdb[%d]:: TS Packet >> Fail \n", nChannelId);
//					return FALSE;//unlock
				}
				else
				{
					CH_PrintDebug("FastLockIsdb[%d]::  TS Packet >> Pass \n", nChannelId);
					/* Locking 되었음 */
					return TRUE;
				}
			}
//			if(i == 5)
//				return FALSE;
			return FALSE;
//			return TRUE;//lock
		}

		Drv_Ch_I2C_DemodRead(nChannelId, TC90507_B0, &seqVal, 1);
		CH_PrintDebug("FastLockIsdb[%d]::	Sequence =0x%x\n", nChannelId, seqVal&0x07);

		/* This part do multiple trials for QAM LOCK, so possibility of lock
		   is increased, but this process takes much time.				   */
		if (count == ISDBT_LOCK_TRIAL)
		{
			CH_PrintDebug("FastLockIsdb[%d]:: Locking failed!\n", nChannelId);
			return FALSE;
		}
	} /* end of while */

	return TRUE;
}


static HUINT8 CheckIsdbAgcLock(int  nChannelId)
{
	HUINT8	agcFlag,val1, val2;

	/* Read the value of AGC loops */
	Drv_Ch_I2C_DemodRead(nChannelId,  TC90507_83, &val1, 1);//RF_AGC
	Drv_Ch_I2C_DemodRead(nChannelId,  TC90507_82, &val2, 1);//IF_AGC
	CH_PrintDebug("CheckIsdbAgcLock[%d]:: RF_AGC = 0x%x (%d), IF_AGC= 0x%x (%d)\n", nChannelId, val1,val1,val2, val2);

	if ( ((val1==0xff) && (val2==0xff)) || ((val1==0xdf) && (val2==0xff)) ||((val1==0xff) && (val2 > 130)))
		return FALSE;
	else
		return TRUE;

	Drv_Ch_I2C_DemodRead(nChannelId,   TC90507_80, &agcFlag, 1);//level error check
	if ( (agcFlag && 0x40)==0x40 )
		return FALSE;//lock
	else
		return TRUE;//unlock
}

static void GetUnCorrectedPacket(int  nChannelId, HUINT32 *unCorPkt)
{
	HUINT8	errCnt[3];
	HUINT8	val;
	HUINT8	berCalMode;		/* 0 : after RS decoding, 1 : after Viterbi decoding */

	/* Check the BER Calculation mode */
	Drv_Ch_I2C_DemodRead(nChannelId, TC90507_77, &berCalMode, 1);
	if(berCalMode & 0x01)
	{
		CH_PrintDebug("[GetUnCorrectedPacket]FEC After Viterbi Mode.\n");
	}
	else
	{
		CH_PrintDebug("[GetUnCorrectedPacket]FEC After RSDecod Mode.\n");
	}

	/* Get RS decoding Error */
	Drv_Ch_I2C_DemodRead(nChannelId, TC90507_96, &val, 1);

	/* Layer A */
	if((val & 0x80) == 0x80)	/* No error */
	{
		/* Get the number of uncorrected packet Layer A */
		Drv_Ch_I2C_DemodRead(nChannelId,   TC90507_9D, &errCnt[0], 3);
		unCorPkt[0]  = (((HUINT32)errCnt[0])<<16) | (((HUINT32)errCnt[1])<<8) |(HUINT32)errCnt[2];
		CH_PrintDebug("[GetUnCorrectedPacket]bus[%d] Layer A Corrected Packet = %d\n", nChannelId, unCorPkt[0]);
	}
	else	/* Error */
	{
		unCorPkt[0] = 0;
	   	CH_PrintDebug("[GetUnCorrectedPacket] Layer A Error-Decision Error\n");
	}

	/* Layer B */
	if((val & 0x40) == 0x40)	/* No error */
	{
		/* Get the number of uncorrected packet Layer B */
		Drv_Ch_I2C_DemodRead(nChannelId,   TC90507_A0, &errCnt[0], 3);
		unCorPkt[1]  = (((HUINT32)errCnt[0])<<16) | (((HUINT32)errCnt[1])<<8) |(HUINT32)errCnt[2] ;
		CH_PrintDebug("[GetUnCorrectedPacket]bus[%d] Layer B Corrected Packet = %d\n", nChannelId, unCorPkt[1]);
	}
	else	/* Error */
	{
		unCorPkt[1] = 0;
		CH_PrintDebug("[GetUnCorrectedPacket] Layer B Error-Decision Error\n");
	}

	/* Layer C */
	if((val & 0x20) == 0x20)	/* No error */
	{
		/*	Get the PACKET_UncorrectNumber_count :layer C */
		Drv_Ch_I2C_DemodRead(nChannelId,   TC90507_A3, &errCnt[0], 3);
		unCorPkt[2]  = (((HUINT32)errCnt[0])<<16) | (((HUINT32)errCnt[1])<<8) |(HUINT32)errCnt[2];
		CH_PrintDebug("[GetUnCorrectedPacket]bus[%d] Layer C Corrected Packet = %d\n", nChannelId, unCorPkt[2]);
	}
	else	/* Error */
	{
		unCorPkt[2] = 0;
		CH_PrintDebug("[GetUnCorrectedPacket] Layer C Error-Decision Error\n");
	}
}

static void GetUnErroredPacket(int nChannelId, HUINT32 *unErroredPkt)
{
	HUINT8	Ecnt[2];

	/* Get the number of uncorrected packet Layer A */
	Drv_Ch_I2C_DemodRead(nChannelId,   TC90507_A6, &Ecnt[0], 2);//[15:8]
	unErroredPkt[0] = ((HUINT32)Ecnt[0]<<8) |(HUINT32)Ecnt[1] ;
	CH_PrintDebug("GetUnErroredPacket[%d]:: Layer A Unerrored Packet = %d\n", nChannelId, unErroredPkt[0]);

	/* Get the number of uncorrected packet Layer B */
	Drv_Ch_I2C_DemodRead(nChannelId,   TC90507_A8, &Ecnt[0], 2);//[15:8]
	unErroredPkt[1] = ((HUINT32)Ecnt[0]<<8) |(HUINT32)Ecnt[1] ;
	CH_PrintDebug("GetUnErroredPacket[%d]:: Layer B Unerrored Packet = %d\n", nChannelId, unErroredPkt[1]);

	/*	Get the PACKET_UncorrectNumber_count :layer C */
	Drv_Ch_I2C_DemodRead(nChannelId,   TC90507_AA, &Ecnt[0], 2);//[15:8]
	unErroredPkt[2] = ((HUINT32)Ecnt[0]<<8) |(HUINT32)Ecnt[1] ;
	CH_PrintDebug("GetUnErroredPacket[%d]:: Layer C Unerrored Packet = %d\n", nChannelId, unErroredPkt[2]);
}



static HINT16 CalcIsdbStrength(int nChannelId, HINT16 levelNum, HUINT8 rfagc, HUINT8 ifagc, HUINT8 (*rfAgcTable)[levelNo], HUINT8 (*ifAgcTable)[levelNo])
{
	HINT16	strength, offset;
	int DsPwrIndexT, DsPwrIndexI;
	int i, level;

	strength = 0;
	offset = 0;
	DsPwrIndexT = 0;
	DsPwrIndexI = 0;
#if(0)
	for(j = 0; j < 2; j++)
	{
		for(i = 0; i < levelNum; i++)
		{
			CH_PrintMon("RfTable(%d)(%d) : %d, IfTable(%d)(%d) : %d\n", j, i, rfAgcTable[j][i], j, i,ifAgcTable[j][i]);
		}
	}
#endif
	for (i=0; i<levelNum; i++)
	{
		if ( (rfagc <= rfAgcTable[nChannelId][i]) )
		{
			DsPwrIndexT = i;
			if (rfagc == rfAgcTable[nChannelId][i])
			{
				offset = 0;
			}
			else
			{
				offset = 5;
			}
			break;
		}

		if (rfagc >= rfAgcTable[nChannelId][levelNum-1])   // rfagc table이 255로 끝나지 않는넘들의 경우 인덱스가 0이되서 레벨이 84dBuV가 되버리기때문에
		{
			DsPwrIndexT = 40;
			break;
		}
	}

	for (i=0; i<levelNum; i++)
	{
		if ( (ifagc <= ifAgcTable[nChannelId][i]) )
		{
			DsPwrIndexI = i;
			if(ifagc  ==  ifAgcTable[nChannelId][i])
			{
				offset = 0;
			}
			else
			{
				offset = -5;
			}
			break;
		}
	}

	CH_PrintDebug("[%d] DsPwrIndexT = %d, DsPwrIndexI  = %d\n", nChannelId, DsPwrIndexT, DsPwrIndexI);

	if( DsPwrIndexI <= DsPwrIndexT )
	{
		level = DsPwrIndexT;
	}
	else
	{
		level = DsPwrIndexI;
	}

	if( rfagc < rfAgcTable[nChannelId][0])
	{
		strength = 250;
	}
	else if ( ifagc > ifAgcTable[nChannelId][levelNum -1] )
	{
		strength = -160;
	}
	else
	{
		strength = (dslevel[level]*10)+offset;
	}

	CH_PrintDebug("GetIsdbStrength: strength = %d, level index = %d\n", strength, level);

	return strength;
}

static HINT16 GetIsdbStrength(int nChannelId)
{
	HUINT8	val1,val2;
	HUINT8	rfagc, ifagc;
	int i, level;
	int DsPwrIndexT, DsPwrIndexI;
	HINT16  strength=0, offset;

	level = 0;
	strength = 0;
	offset = 0;
	DsPwrIndexT = 0;
	DsPwrIndexI = 0;

    if (0)//( gBoardRev == BOARD_REV_0_5 )
    {
        	Drv_Ch_I2C_DemodRead(nChannelId,   TC90507_83, &val1, 1);//RF_AGC
        	rfagc = val1;
        	Drv_Ch_I2C_DemodRead(nChannelId,   TC90507_82, &val2, 1);//IF_AGC
        	ifagc = val2;


          	CH_PrintDebug("GetIsdbStrength:  rfagc = %d, ifagc = %d\n", rfagc, ifagc);

        	if (gTC90507Channel[nChannelId].frequency <= 167000000)
        	{
        		for (i=0; i<levelNo; i++)
        		{
        	       	if ( (rfagc <= RfAgcIsdb1[i]) )
        	       	{
        	       		DsPwrIndexT = i;
        	       		if (rfagc == RfAgcIsdb1[i])
        	       		{
        					offset = 0;
        	       		}
        				else
        				{
        					offset = 5;
        				}
        	      		break;
        	       	}

        			if (rfagc >= RfAgcIsdb1[levelNo-1])   // rfagc table이 255로 끝나지 않는넘들의 경우 인덱스가 0이되서 레벨이 84dBuV가 되버리기때문에
        			{
        				DsPwrIndexT = 40;
        				break;
        			}
        	   	}

        		for (i=0; i<levelNo; i++)
        		{
        	       	if ( (ifagc <= IfAgcIsdb1[i]) )
        	   		{
        				DsPwrIndexI = i;
        	   			if(ifagc  ==  IfAgcIsdb1[i])
        	   			{
        					offset = 0;
        	   			}
        				else
        				{
        					offset = -5;
        				}
        	 			break;
        	   		}
        	  	}

        		CH_PrintDebug("[%d] DsPwrIndexT = %d, DsPwrIndexI  = %d\n", nChannelId, DsPwrIndexT, DsPwrIndexI);

        		if( DsPwrIndexI <= DsPwrIndexT )
        		{
        			level = DsPwrIndexT;
        		}
        		else
        		{
        			level = DsPwrIndexI;
        		}

        		if( rfagc < RfAgcIsdb1[0])
        		{
        			strength = 250;
        		}
        		else if ( ifagc > IfAgcIsdb1[levelNo -1] )
        		{
        			strength = -160;
        		}
        		else
        		{
        			strength = (dslevel[level]*10)+offset;
        		}

        	}
        	else if (gTC90507Channel[nChannelId].frequency <= 473000000)
        	{
        		for (i=0; i<levelNo; i++)
        		{
        	       	if ( (rfagc <= RfAgcIsdb2[i]) )
        	       	{
        	       		DsPwrIndexT = i;
        	       		if (rfagc == RfAgcIsdb2[i])
        	       		{
        					offset = 0;
        	       		}
        				else
        				{
        					offset = 5;
        				}
        	      		break;
        	       	}

        			if (rfagc >= RfAgcIsdb2[levelNo-1])   // rfagc table이 255로 끝나지 않는넘들의 경우 인덱스가 0이되서 레벨이 84dBuV가 되버리기때문에
        			{
        				DsPwrIndexT = 40;
        				break;
        			}
        	   	}

        		for (i=0; i<levelNo; i++)
        		{
        	       	if ( (ifagc <= IfAgcIsdb2[i]) )
        	   		{
        				DsPwrIndexI = i;
        	   			if(ifagc  ==  IfAgcIsdb2[i])
        	   			{
        					offset = 0;
        	   			}
        				else
        				{
        					offset = -5;
        				}
        	 			break;
        	   		}
        	  	}

        		CH_PrintDebug("[%d] DsPwrIndexT = %d, DsPwrIndexI  = %d\n", nChannelId, DsPwrIndexT, DsPwrIndexI);

        		if( DsPwrIndexI <= DsPwrIndexT )
        		{
        			level = DsPwrIndexT;
        		}
        		else
        		{
        			level = DsPwrIndexI;
        		}

        		if( rfagc < RfAgcIsdb2[0])
        		{
        			strength = 250;
        		}
        		else if ( ifagc > IfAgcIsdb2[levelNo -1] )
        		{
        			strength = -160;
        		}
        		else
        		{
        			strength = (dslevel[level]*10)+offset;
        		}


        	}
        	else
        	{
        		for (i=0; i<levelNo; i++)
        		{
        	       	if ( (rfagc <= RfAgcIsdb3[i]) )
        	       	{
        	       		DsPwrIndexT = i;
        	       		if (rfagc == RfAgcIsdb3[i])
        	       		{
        					offset = 0;
        	       		}
        				else
        				{
        					offset = 5;
        				}
        	      		break;
        	       	}

        			if (rfagc >= RfAgcIsdb3[levelNo-1])   // rfagc table이 255로 끝나지 않는넘들의 경우 인덱스가 0이되서 레벨이 84dBuV가 되버리기때문에
        			{
        				DsPwrIndexT = 40;
        				break;
        			}
        	   	}

        		for (i=0; i<levelNo; i++)
        		{
        	       	if ( (ifagc <= IfAgcIsdb3[i]) )
        	   		{
        				DsPwrIndexI = i;
        	   			if(ifagc  ==  IfAgcIsdb3[i])
        	   			{
        					offset = 0;
        	   			}
        				else
        				{
        					offset = -5;
        				}
        	 			break;
        	   		}
        	  	}

        		CH_PrintDebug("[%d] DsPwrIndexT = %d, DsPwrIndexI  = %d\n", nChannelId, DsPwrIndexT, DsPwrIndexI);

        		if( DsPwrIndexI <= DsPwrIndexT )
        		{
        			level = DsPwrIndexT;
        		}
        		else
        		{
        			level = DsPwrIndexI;
        		}

        		if( rfagc < RfAgcIsdb3[0])
        		{
        			strength = 250;
        		}
        		else if ( ifagc > IfAgcIsdb3[levelNo -1] )
        		{
        			strength = -160;
        		}
        		else
        		{
        			strength = (dslevel[level]*10)+offset;
        		}

            }
    }
    else  if (1)// gBoardRev == BOARD_REV_0_55 )
    {
       	Drv_Ch_I2C_DemodRead(nChannelId,   TC90507_83, &rfagc, 1);//RF_AGC
        	Drv_Ch_I2C_DemodRead(nChannelId,   TC90507_82, &ifagc, 1);//IF_AGC

          	CH_PrintDebug(" GetIsdbStrength:  rfagc = %d, ifagc = %d\n", rfagc, ifagc);

        	if ((gTC90507Channel[nChannelId].frequency >=93000000) && (gTC90507Channel[nChannelId].frequency < 256000000))
        	{
        		if((gTC90507Channel[nChannelId].frequency == 167000000) ||(gTC90507Channel[nChannelId].frequency == 167143000))
        		{
        			strength = CalcIsdbStrength(nChannelId, levelNo, rfagc, ifagc,isdbtRfAgcGain167, isdbtIfAgcGain167);
        		}
        		else
        		{
        			strength = CalcIsdbStrength(nChannelId, levelNo, rfagc, ifagc, isdbtRfAgcGain1, isdbtIfAgcGain1);
        		}
        	}
        	else if (gTC90507Channel[nChannelId].frequency < 322000000)
        	{
        		strength = CalcIsdbStrength(nChannelId, levelNo, rfagc, ifagc, isdbtRfAgcGain2, isdbtIfAgcGain2);
        	}
        	else if (gTC90507Channel[nChannelId].frequency < 466000000)
        	{
        		strength = CalcIsdbStrength(nChannelId, levelNo, rfagc, ifagc, isdbtRfAgcGain3, isdbtIfAgcGain3);
        	}
        	else if (gTC90507Channel[nChannelId].frequency < 552000000)
        	{
        		strength = CalcIsdbStrength(nChannelId, levelNo, rfagc, ifagc, isdbtRfAgcGain4, isdbtIfAgcGain4);
        	}
        	else if (gTC90507Channel[nChannelId].frequency < 768000000)
        	{
        		strength = CalcIsdbStrength(nChannelId, levelNo, rfagc, ifagc, isdbtRfAgcGain5, isdbtIfAgcGain5);
        	}
        	else
        	{
        		CH_PrintDebug("[TC90507_GetIsdbStrength : Error!! Out of range.\n");
        		strength = CalcIsdbStrength(nChannelId, levelNo, rfagc, ifagc, isdbtRfAgcGain5, isdbtIfAgcGain5);
        	}

    }

	CH_PrintDebug("GetIsdbStrength: strength = %d, level index = %d\n", strength, level);

	return strength;
}


/********************************************************************
 C/N[dB]=
 0.000024 * (P POS 4) - 0.0016 * (P POS 3) + 0.0398 * (P POS 2) + 0.5491 * P + 3.0965
 P=10log(5505024/cndat)
*********************************************************************/
static HUINT16  GetOfdmCn(int nChannelId)
{
	HUINT8	Ecnt[3];//, RS_uncorr;

	HUINT32 	cndat=0;
	float P_value4,P_value3,P_value2,P_value1;
	float P_value=0.0;
	float CN_value=0.0;
	HUINT16 cn;

	/* read the uncorrectable error count @ R-S decoder */

	/* Get the C/N */
	Drv_Ch_I2C_DemodRead(nChannelId,    TC90507_8B, &Ecnt[0], 3);

	cndat  = (Ecnt[0]<<16) | (Ecnt[1]<<8) |(Ecnt[2]) ;

	CH_PrintDebug("\n[0]=%x, [1]=%x, [2]=%x\n", Ecnt[0], Ecnt[1], Ecnt[2]);

	if (cndat == 0)
	{
		cn = 0;
	}
	else
	{
		//P_value=(10*log10(5505024/cndat));
		CH_PrintDebug("P_value = %4.2f, cndat=%d\n", P_value, cndat);
		P_value4=PowerOfExponential(P_value,4);
		P_value3=PowerOfExponential(P_value,3);
		P_value2=PowerOfExponential(P_value,2);
		P_value1=PowerOfExponential(P_value,1);

		CH_PrintDebug("%f, %f, %f, %f\n", P_value4,P_value3,P_value2,P_value1);

		CN_value=(float)((0.000024*P_value4) - (0.0016*P_value3)
				+ (0.0398*P_value2) + (0.5491*P_value1) + 3.0965);

		CH_PrintDebug("GetOfdmCn[%d]:: CN_value=%4.2f\n", nChannelId, CN_value );
		cn = (HUINT16)(CN_value * 10.0);
	}
	return cn;
}

static void GetIsdbtBer(int nChannelId, HUINT32 *berVal, HUINT32 errCnt, HUINT32 errCycle, HUINT32 *ngFlag)
{
	HUINT8	berCalMode;		/* 0 : after RS decoding, 1 : after Viterbi decoding */

	Drv_Ch_I2C_DemodRead(nChannelId, TC90507_77, &berCalMode, 1);

	if (errCycle == 0)
	{
		*berVal = 0;
	}
	else
	{
		if(berCalMode & 0x01)
		{
			*berVal=(HUINT32)((errCnt/(errCycle*204.0*8.0)) * 100000000);
		}
		else
		{
			*berVal=(HUINT32)(((errCnt* 9.0*8*0.5)/(errCycle*204.0*8.0)) * 100000000);
		}
	}

	if(errCnt == 0)
	{
		*ngFlag = 0;	/* OK */
	}
	else
	{
		*ngFlag = 1;	/* NG */
	}
}

#if(0)
HUINT32 GetOfdmBER(HUINT32 nChannelId, HUINT32 *BERVal)
{
	HUINT8 check = 0;
	HUINT8 layerErrStatus = 0;
	HUINT8 status = 0;
	check = CheckRSDecodingErr(nChannelId);

	if(gTC90507Layer[nChannelId].layer_A_error == FALSE)
	{
		layerErrStatus |= 0x4;
	}
	if(gTC90507Layer[nChannelId].layer_B_error == FALSE)
	{
		layerErrStatus |= 0x2;
	}
	if(gTC90507Layer[nChannelId].layer_C_error == FALSE)
	{
		layerErrStatus |= 0x1;
	}

	status = check && layerErrStatus;

	CH_PrintMon("Check : %d\n", check);
	CH_PrintMon("layerErrStatus : %d\n", layerErrStatus);
	CH_PrintMon("status : %d\n", status);

	switch(status)
	{
		case 7:
			BERVal[0] = GetOfdmLayerBER(nChannelId, 1);
			BERVal[1] = GetOfdmLayerBER(nChannelId, 2);
			BERVal[2] = GetOfdmLayerBER(nChannelId, 3);
			break;
		case 6:
			BERVal[0] = GetOfdmLayerBER(nChannelId, 1);
			BERVal[1] = GetOfdmLayerBER(nChannelId, 2);
			BERVal[2] = 0;
			break;
		case 5:
			BERVal[0] = GetOfdmLayerBER(nChannelId, 1);
			BERVal[1] = 0;
			BERVal[2] = GetOfdmLayerBER(nChannelId, 3);
			break;
		case 4:
			BERVal[0] = GetOfdmLayerBER(nChannelId, 1);
			BERVal[1] = 0;
			BERVal[2] = 0;
			break;
		case 3:
			BERVal[0] = 0;
			BERVal[1] = GetOfdmLayerBER(nChannelId, 2);
			BERVal[2] = GetOfdmLayerBER(nChannelId, 3);
			break;
		case 2:
			BERVal[0] = 0;
			BERVal[1] = GetOfdmLayerBER(nChannelId, 2);
			BERVal[2] = 0;
			break;
		case 1:
			BERVal[0] = 0;
			BERVal[1] = 0;
			BERVal[2] = GetOfdmLayerBER(nChannelId, 3);
			break;
		case 0:
			BERVal[0] = 0;
			BERVal[1] = 0;
			BERVal[2] = 0;
			break;
		default:
			break;
	}

	return 0;
}

HUINT8 CheckRSDecodingErr(HUINT32 nChannelId)
{
	HUINT8	err = 0;

	Drv_Ch_I2C_DemodRead(nChannelId,   TC90507_96, &err, 1);

	CH_PrintMon("RSDecodingErr : %d\n", err);

	err = err >> 5;

	CH_PrintMon("RSDecodingErr : %d\n", err);

	return err;
}

HUINT32 GetOfdmLayerBER(HUINT32 nChannelId, HUINT8 layer)
{
	HUINT8	Ecnt[3],Ecnt_cycle[2],ber_set_cfg;
	unsigned long uncor_pkt=0,RS_Unccor_cycle=0;
	HUINT32 ber=0;
	HUINT8	rslock;
	HUINT8 regCntOffset = 0, regCycleOffset = 0;

	switch(layer)
	{
		case 1:
			regCntOffset = 0x0;
			regCycleOffset = 0x0;
			break;
		case 2:
			regCntOffset = 0x3;
			regCycleOffset = 0x2;
			break;
		case 3:
			regCntOffset = 0x6;
			regCycleOffset = 0x4;
			break;
		default :
			break;
	}

	/*	Get the PACKET_UncorrectNumber_count :layer A */
	Drv_Ch_I2C_DemodRead(nChannelId,   TC90507_9D + regCntOffset, &Ecnt[0], 3);//reg[9d]msb [23:16],reg[9e]msb[15:8],reg[9f]/lsb [7:0]
	CH_PrintMon("[Ecnt[0] : %d\n", (HUINT16)Ecnt[0]);
	CH_PrintMon("[Ecnt[1] : %d\n", (HUINT16)Ecnt[1]);
	CH_PrintMon("[Ecnt[2] : %d\n", (HUINT16)Ecnt[2]);
	uncor_pkt  =( ((unsigned long)Ecnt[0])<<16) | (((unsigned long)Ecnt[1])<<8) |(((unsigned long)Ecnt[2])) ;
	CH_PrintMon("[uncor_pkt : %ld\n", uncor_pkt);
	//Get the Number of PACKETs for layer_A count obserbvation cycle
	Drv_Ch_I2C_DemodRead(nChannelId,   TC90507_A6 + regCycleOffset, &Ecnt_cycle[0], 2);//[15:8]
//	Drv_Ch_I2C_DemodRead(nChannelId,   TC90507_A7, &Ecnt_cycle[0], 1);//lsb [7:0]
	CH_PrintMon("[Ecnt_cycle[0] : %d\n", (HUINT16)Ecnt_cycle[0]);
	CH_PrintMon("[Ecnt_cycle[1] : %d\n", (HUINT16)Ecnt_cycle[1]);
	RS_Unccor_cycle  =(((unsigned long)Ecnt_cycle[0])<<8) |(((unsigned long)Ecnt_cycle[1])) ;
	CH_PrintMon("[RS_Unccor_cycle : %ld\n", RS_Unccor_cycle);

	if (RS_Unccor_cycle == 0)
	{
		ber = 9999;

	}

	//check -> ber_set_cfg, reg[77]D[0]cor(RS decoding error count setting
	// bit로 셋팅되어 있으므로, 계산은 비트로 해주어야 한다.// BER=PER/2
	Drv_Ch_I2C_DemodRead(nChannelId,   TC90507_77, &ber_set_cfg, 1);
	if(ber_set_cfg & 0x01)		// BER : After Viterbi
	{
		ber=(HUINT32)((uncor_pkt/(RS_Unccor_cycle*204.0*8.0)) * 1000000);
				//*pdBer = (float)(iErrCnt) / (sPktCnt * 204 * 8);// 204(Bytes)*8*(bits)
	}
	else					// BER : After Reed-Solomon
	{
		ber=(HUINT32)(((uncor_pkt* 9.0*8*0.5)/(RS_Unccor_cycle*204.0*8.0)) * 1000000);
				//*pdBer = (float)(iErrCnt * 9) / (sPktCnt * 204 * 2);
	}

		//m_strVITERBI_BER_Value_L1.Format("%4.2e",ber);
	CH_PrintMon("[TEST:BER Layer A : %d\n", ber);

	return ber;

}
#endif

#if(0)
HUINT32 GetOfdmBer(HUINT32 nChannelId, HUINT32 *BERVal)
{
	HUINT8	Ecnt[3],Ecnt_cycle[2],ber_set_cfg;
	unsigned long uncor_pkt=0,RS_Unccor_cycle=0;
	HUINT32 ber=0;
	HUINT8	rslock;

	Drv_Ch_I2C_DemodRead(nChannelId,   TC90507_96, &rslock, 1);

	if((rslock&0x80) == 0x80)
	{
		/*	Get the PACKET_UncorrectNumber_count :layer A */
		Drv_Ch_I2C_DemodRead(nChannelId,   TC90507_9D, &Ecnt[0], 3);//reg[9d]msb [23:16],reg[9e]msb[15:8],reg[9f]/lsb [7:0]

		CH_PRT(7, ("[Ecnt[0] : %d\n", (HUINT16)Ecnt[0]));
		CH_PRT(7, ("[Ecnt[1] : %d\n", (HUINT16)Ecnt[1]));
		CH_PRT(7, ("[Ecnt[2] : %d\n", (HUINT16)Ecnt[2]));
		uncor_pkt  =( ((unsigned long)Ecnt[0])<<16) | (((unsigned long)Ecnt[1])<<8) |(((unsigned long)Ecnt[2])) ;
		CH_PRT(6, ("[BER] Layer A uncor_pkt [%d] : %ld\n", nChannelId, uncor_pkt));
		//Get the Number of PACKETs for layer_A count obserbvation cycle
		Drv_Ch_I2C_DemodRead(nChannelId,   TC90507_A6, &Ecnt_cycle[1], 1);//[15:8]
		Drv_Ch_I2C_DemodRead(nChannelId,   TC90507_A7, &Ecnt_cycle[0], 1);//lsb [7:0]
		CH_PRT(7, ("[Ecnt_cycle[0] : %d\n", (HUINT16)Ecnt_cycle[0]));
		CH_PRT(7, ("[Ecnt_cycle[1] : %d\n", (HUINT16)Ecnt_cycle[1]));
		RS_Unccor_cycle  =(((unsigned long)Ecnt_cycle[1])<<8) |(((unsigned long)Ecnt_cycle[0])) ;
		CH_PRT(7,("[RS_Unccor_cycle : %ld\n", RS_Unccor_cycle));

		if (RS_Unccor_cycle == 0)
		{
			BERVal[0] = 9999;
		}

		//check -> ber_set_cfg, reg[77]D[0]cor(RS decoding error count setting
		// bit로 셋팅되어 있으므로, 계산은 비트로 해주어야 한다.// BER=PER/2
		Drv_Ch_I2C_DemodRead(nChannelId,   TC90507_77, &ber_set_cfg, 1);
		if(ber_set_cfg & 0x01)		// BER : After Viterbi
		{
			BERVal[0]=(HUINT32)((uncor_pkt/(RS_Unccor_cycle*204.0*8.0)) * 100000000);
			//*pdBer = (float)(iErrCnt) / (sPktCnt * 204 * 8);// 204(Bytes)*8*(bits)
		}
		else					// BER : After Reed-Solomon
		{
			BERVal[0]=(HUINT32)(((uncor_pkt* 9.0*8*0.5)/(RS_Unccor_cycle*204.0*8.0)) * 100000000);
			//*pdBer = (float)(iErrCnt * 9) / (sPktCnt * 204 * 2);
		}
		//m_strVITERBI_BER_Value_L1.Format("%4.2e",ber);
	}
	CH_PRT(6, ("[BER] Layer A[%d] : %d\n", nChannelId, BERVal[0]));
//#if(0)
	if((rslock&0x40) == 0x40)
	{
		/*	Get the PACKET_UncorrectNumber_count :layer B */
		Drv_Ch_I2C_DemodRead(nChannelId,   TC90507_A0, &Ecnt[0], 3);//reg[a0]msb [23:16],reg[a1]msb[15:8],reg[a2]/lsb [7:0]
		uncor_pkt  =( ((unsigned long)Ecnt[0])<<16) | (((unsigned long)Ecnt[1])<<8) |(((unsigned long)Ecnt[2])) ;
			//Get the Number of PACKETs for layer_B count obserbvation cycle

		CH_PRT(6, ("[BER] Layer B uncor_pkt [%d] : %ld\n", nChannelId, uncor_pkt));
		Drv_Ch_I2C_DemodRead(nChannelId,   TC90507_A8, &Ecnt_cycle[1], 1);//[15:8]
		Drv_Ch_I2C_DemodRead(nChannelId,   TC90507_A9, &Ecnt_cycle[0], 1);//lsb [7:0]
			RS_Unccor_cycle  = (((unsigned long)Ecnt_cycle[1])<<8) |(((unsigned long)Ecnt_cycle[0])) ;
			//uncor_pkt=0x00800e;//test
			//RS_Unccor_cycle=0x1000;//test
		if (RS_Unccor_cycle == 0)
		{
			BERVal[1] = 9999;
		}

		if(ber_set_cfg & 0x01)		// BER : After Viterbi
		{
			BERVal[1]=(HUINT32)((uncor_pkt/(RS_Unccor_cycle*204.0*8.0)) * 100000000);
			//*pdBer = (float)(iErrCnt) / (sPktCnt * 204 * 8);// 204(Bytes)*8*(bits)
		}
		else					// BER : After Reed-Solomon
		{
			BERVal[1]=(HUINT32)(((uncor_pkt* 9.0*8*0.5)/(RS_Unccor_cycle*204.0*8.0)) * 100000000);
			//*pdBer = (float)(iErrCnt * 9) / (sPktCnt * 204 * 2);
		}
		//m_strVITERBI_BER_Value_L2.Format("%4.2e",ber);
	}

	CH_PRT(6, ("[BER] Layer B [%d]: %d\n", nChannelId, BERVal[1]));

	if((rslock&0x20) == 0x20)
	{
		/*	Get the PACKET_UncorrectNumber_count :layer C */
		Drv_Ch_I2C_DemodRead(nChannelId,   TC90507_A3, &Ecnt[0], 3);//reg[a3]msb [23:16],reg[a4]msb[15:8],reg[a5]/lsb [7:0]
		uncor_pkt  =( ((unsigned long)Ecnt[0])<<16) | (((unsigned long)Ecnt[1])<<8) |(((unsigned long)Ecnt[2])) ;
		CH_PRT(6, ("[BER] Layer C uncor_pkt [%d] : %ld\n", nChannelId, uncor_pkt));
		//Get the Number of PACKETs for layer_C count obserbvation cycle
		Drv_Ch_I2C_DemodRead(nChannelId,   TC90507_AA, &Ecnt_cycle[1], 1);//[15:8]
		Drv_Ch_I2C_DemodRead(nChannelId,   TC90507_AB, &Ecnt_cycle[0], 1);//lsb [7:0]
		RS_Unccor_cycle  = (((unsigned long)Ecnt_cycle[1])<<8) |(((unsigned long)Ecnt_cycle[0])) ;
		if (RS_Unccor_cycle == 0)
		{
			BERVal[2] = 9999;
		}

		if(ber_set_cfg & 0x01)		// BER : After Viterbi
		{
			BERVal[2]=(HUINT32)((uncor_pkt/(RS_Unccor_cycle*204.0*8.0)) * 100000000);
			//*pdBer = (float)(iErrCnt) / (sPktCnt * 204 * 8);// 204(Bytes)*8*(bits)
		}
		else					// BER : After Reed-Solomon
		{
			BERVal[2]=(HUINT32)(((uncor_pkt* 9.0*8*0.5)/(RS_Unccor_cycle*204.0*8.0)) * 100000000);
			//*pdBer = (float)(iErrCnt * 9) / (sPktCnt * 204 * 2);
		}
		//m_strVITERBI_BER_Value_L3.Format("%4.2e",ber);
	}
	CH_PRT(6, ("[BER] Layer C[%d] : %d\n", nChannelId, BERVal[2]));

	return 0;
}
#endif

/* math function which calulate power of exponential */
static float PowerOfExponential(float mantissa, HUINT8 exponent)
{
	HUINT8	i;
	float	result = 1.0;

	if(mantissa == 0.0)
	{
		CH_PrintDebug("pow_positive:: PowerOfExponential(mantissa=0) error\n");
	}
	else
	{
		for(i=0;i<exponent;i++)
		{
			result = result*mantissa;
		}
	}

	return (result);
}

void ViewOfdmParam(int nChannelId)
{
	HINT16 index1=0x0000, index2=0x0000, index3=0x0000, index4=0x0000;
	HUINT8 val, ffsize, gdleng, Num_seg;

	Drv_Ch_I2C_DemodRead(nChannelId,   TC90507_96, &val, 1);
	CH_PrintDebug("ViewOfdmParam[%d]:: Layer Error [96] :%x\n", nChannelId, val);

	if((val & 0x80) == 0x80)
	{
		gTC90507Layer[nChannelId].layer_A_error=FALSE;  // no error
		CH_PrintDebug("ViewOfdmParam:: layer_A_error : NO ERROR \n");
	}//D[6]
	else
	{
		gTC90507Layer[nChannelId].layer_A_error=TRUE;
		CH_PrintDebug("ViewOfdmParam:: layer_A_error : ERROR\n");
	}

	if((val & 0x40) == 0x40)
	{
		gTC90507Layer[nChannelId].layer_B_error=FALSE;
		CH_PrintDebug("ViewOfdmParam:: layer_B_error : NO ERROR\n");
	}//D[3]
	else
	{
		gTC90507Layer[nChannelId].layer_B_error=TRUE;
		CH_PrintDebug("ViewOfdmParam:: layer_B_error : ERROR\n");
	}
	if((val & 0x20) == 0x20)
	{
		gTC90507Layer[nChannelId].layer_C_error=FALSE;
		CH_PrintDebug("ViewOfdmParam:: layer_C_error : NO ERROR\n");
	}//D[5]
	else
	{
		gTC90507Layer[nChannelId].layer_C_error=TRUE;
		CH_PrintDebug("ViewOfdmParam:: layer_C_error : ERROR\n");
	}

//fft & guard interval
		Drv_Ch_I2C_DemodRead(nChannelId,    TC90507_B0, &val, 1);//0xb0
		CH_PrintDebug("ViewOfdmParam:: FFT & Guard [0xb0] : %x\n", val);
		ffsize = val & 0xc0;//D7:6
		gdleng = val & 0x30;//D5:4

	   switch( ffsize )
	   {
		case 0x00:
			gTC90507Channel[nChannelId].TPS_fft_info=fft_2K;
			val=0x04;
			Drv_Ch_I2C_DemodWrite(nChannelId,    TC90507_BA, &val, 1);
			val=0x00;
			Drv_Ch_I2C_DemodWrite(nChannelId,    TC90507_BB, &val, 1);
			CH_PrintDebug("ViewOfdmParam:: FFT mode : fft_2K \n");
			break;//1024
		case 0x40:
			gTC90507Channel[nChannelId].TPS_fft_info=fft_4K;
			val=0x08;
			Drv_Ch_I2C_DemodWrite(nChannelId,    TC90507_BA, &val, 1);
			val=0x00;
			Drv_Ch_I2C_DemodWrite(nChannelId,    TC90507_BB, &val, 1);
			CH_PrintDebug("ViewOfdmParam:: FFT mode : fft_4K \n");
			break;//2048
		case 0x80:
			gTC90507Channel[nChannelId].TPS_fft_info=fft_8K;
			val=0x0f;
			Drv_Ch_I2C_DemodWrite(nChannelId,    TC90507_BA, &val, 1);
			val=0x00;
			Drv_Ch_I2C_DemodWrite(nChannelId,    TC90507_BB, &val, 1);
			CH_PrintDebug("ViewOfdmParam:: FFT mode : fft_8K \n");
			break;//4000
		case 0xc0:
			gTC90507Channel[nChannelId].TPS_fft_info=fft_NOT;
	   		CH_PrintDebug("ViewOfdmParam:: FFT mode : Nothing\n");
	   		break;
		}

	   switch( gdleng )
	   {
		case 0x00:
			gTC90507Channel[nChannelId].TPS_guard_info=guard_1_32;
   			CH_PrintDebug("ViewOfdmParam:: Guard Interval : 1/32 \n");
			break;
	   	case 0x10:
			gTC90507Channel[nChannelId].TPS_guard_info=guard_1_16;
   			CH_PrintDebug("ViewOfdmParam:: Guard Interval : 1/16 \n");
			break;
	   	case 0x20:
			gTC90507Channel[nChannelId].TPS_guard_info=guard_1_8;
   			CH_PrintDebug("ViewOfdmParam:: Guard Interval : 1/8\n");
			break;
	   	case 0x30:
			gTC90507Channel[nChannelId].TPS_guard_info=guard_1_4;
   			CH_PrintDebug("ViewOfdmParam:: Guard Interval : 1/4\n");
			break;
	   }

	/* LAYER A */
		Drv_Ch_I2C_DemodRead(nChannelId,    TC90507_B3, &val, 1);//0xb3//D15:8
		CH_PrintDebug("ViewOfdmParam:: Layer A [0xB3] :0x%x  \n",val);
		index1 = (int)val;
		Drv_Ch_I2C_DemodRead(nChannelId,    TC90507_B4, &val, 1);//0xb4//D7:0
		CH_PrintDebug("ViewOfdmParam:: Layer A [0xB4] :0x%x \n",val);
		index2 = (int)val;
		index4 = ((index1<<8)&0xff00)|(index2&0x00ff);

	 // Segment number, reg[b4][6:3]

		Num_seg= (HUINT8 )((index2 & 0x0078)>>3);
		gTC90507Channel[nChannelId].layerinfo.layer_A_segments=Num_seg;
		CH_PrintDebug("ViewOfdmParam:: Layer A segment : %d \n", Num_seg);

	  // Modulation
	   switch(index4 & 0xe000)//reg[b3][7:5]
	   {
	   	case 0x0000:
	   		gTC90507Channel[nChannelId].layerinfo.layer_A_constellation=constellation_DQPSK;
	   		CH_PrintDebug("ViewOfdmParam:: Layer A Modulation : DQPSK\n");
			break;
	   	case 0x2000:
			gTC90507Channel[nChannelId].layerinfo.layer_A_constellation=constellation_QPSK;
	   		CH_PrintDebug("ViewOfdmParam:: Layer A Modulation : QPSK\n");
			break;
		case 0x4000:
			gTC90507Channel[nChannelId].layerinfo.layer_A_constellation=constellation_16QAM;
	   		CH_PrintDebug("ViewOfdmParam:: Layer A Modulation : 16QAM\n");
			break;
	   	case 0x6000:
			gTC90507Channel[nChannelId].layerinfo.layer_A_constellation=constellation_64QAM;
	   		CH_PrintDebug("ViewOfdmParam:: Layer A Modulation : 64QAM\n");
			break;
	   	case 0xE000:
			gTC90507Channel[nChannelId].layerinfo.layer_A_constellation=constellation_NOTHING;
	   		CH_PrintDebug("ViewOfdmParam:: Layer A Modulation : Nothing\n");
			break;
	   }
	 // Code Rate
	   switch(index4 & 0x1C00)//reg[b3][4:2]
	   {
	   	case 0x0000:
			gTC90507Channel[nChannelId].layerinfo.layer_A_CodeRate=CodeRate_1_2;
	   		CH_PrintDebug("ViewOfdmParam:: Layer A Code Rate :1/2 \n");
			break;
	   	case 0x0400:
			gTC90507Channel[nChannelId].layerinfo.layer_A_CodeRate=CodeRate_2_3;
	   		CH_PrintDebug("ViewOfdmParam:: Layer A Code Rate : 2/3\n");
			break;
	   	case 0x0800:
			gTC90507Channel[nChannelId].layerinfo.layer_A_CodeRate=CodeRate_3_4;
	   		CH_PrintDebug("ViewOfdmParam:: Layer A Code Rate : 3/4\n");
			break;
	   	case 0x0C00:
			gTC90507Channel[nChannelId].layerinfo.layer_A_CodeRate=CodeRate_5_6;
	   		CH_PrintDebug("ViewOfdmParam:: Layer A Code Rate : 5/6\n");
			break;
	  	 case 0x1000:
		 	gTC90507Channel[nChannelId].layerinfo.layer_A_CodeRate=CodeRate_7_8;
	  		CH_PrintDebug("ViewOfdmParam:: Layer A Code Rate : 7/8\n");
			break;
	   	case 0x1C00:
			gTC90507Channel[nChannelId].layerinfo.layer_A_CodeRate=CodeRate_NOT;
	   		CH_PrintDebug("ViewOfdmParam:: Layer A Code Rate : Nothing\n");
			break;
		}
	  // Interleave
	   switch(ffsize)
	   {
	   	case 0x00://  2k
			switch(index4 & 0x0380)//reg[b3][1:0]reg[b4][7]
			{
		   	case 0x0000:
				gTC90507Channel[nChannelId].layerinfo.layer_A_Interleave=0;
				CH_PrintDebug("ViewOfdmParam:: Layer A Interleave : 0 \n");
				break;//("I = 0")
		   	case 0x0080: gTC90507Channel[nChannelId].layerinfo.layer_A_Interleave=4;
				CH_PrintDebug("ViewOfdmParam:: Layer A Interleave : 4 \n");
				 break;//("I = 4")
		   	case 0x0100: gTC90507Channel[nChannelId].layerinfo.layer_A_Interleave=8;
				CH_PrintDebug("ViewOfdmParam:: Layer A Interleave : 8\n");
				break;//("I = 8")
		   	case 0x0180: gTC90507Channel[nChannelId].layerinfo.layer_A_Interleave=16;
				CH_PrintDebug("ViewOfdmParam:: Layer A Interleave : 16 \n");
				break;//("I = 16")
		   	case 0x0200: gTC90507Channel[nChannelId].layerinfo.layer_A_Interleave=32;
				CH_PrintDebug("ViewOfdmParam:: Layer A Interleave : 32 \n");
				break;//("I = 32")
		   	case 0x0380: //gTC90507Channel[nChannelId].layerinfo.layer_A_Interleave=0;
				CH_PrintDebug("ViewOfdmParam:: Layer A Interleave : Nothing \n");
				break;
			}
			break;
	   case 0x40:////	4k
		switch(index4 & 0x0380)//reg[b3][1:0]reg[b4][7]
		{
	   		case 0x0000:
				gTC90507Channel[nChannelId].layerinfo.layer_A_Interleave=0;
				CH_PrintDebug("ViewOfdmParam:: Layer A Interleave : 0 \n");
				    break;//("I = 0")
		   	case 0x0080:
				gTC90507Channel[nChannelId].layerinfo.layer_A_Interleave=2;
				CH_PrintDebug("ViewOfdmParam:: Layer A Interleave : 2\n");
				break;//("I = 2")
		   	case 0x0100:
				gTC90507Channel[nChannelId].layerinfo.layer_A_Interleave=4;
				CH_PrintDebug("ViewOfdmParam:: Layer A Interleave : 4 \n");
				break;//("I = 4")
		   	case 0x0180:
				gTC90507Channel[nChannelId].layerinfo.layer_A_Interleave=8;
				CH_PrintDebug("ViewOfdmParam:: Layer A Interleave : 8 \n");
				break;//("I = 8")
		   	case 0x0200:
				gTC90507Channel[nChannelId].layerinfo.layer_A_Interleave=16;
				CH_PrintDebug("ViewOfdmParam:: Layer A Interleave : 16\n");
				break;//("I = 16")
			   case 0x0380: //gTC90507Channel[nChannelId].layerinfo.layer_A_Interleave=0;
			   	CH_PrintDebug("ViewOfdmParam:: Layer A Interleave : Nothing \n");
			   	break;
		}
		break;
	   case 0x80://		8k
		switch(index4 & 0x0380)//reg[b3][1:0]reg[b4][7]
		{
		   	case 0x0000:
				gTC90507Channel[nChannelId].layerinfo.layer_A_Interleave=0;
				CH_PrintDebug("ViewOfdmParam:: Layer A Interleave : 0 \n");
				break;//("I = 0 ")
		   	case 0x0080:
				gTC90507Channel[nChannelId].layerinfo.layer_A_Interleave=1;
				CH_PrintDebug("ViewOfdmParam:: Layer A Interleave : 1 \n");
				break;//("I = 1")
		   	case 0x0100:
				gTC90507Channel[nChannelId].layerinfo.layer_A_Interleave=2;
				CH_PrintDebug("ViewOfdmParam:: Layer A Interleave : 2 \n");
				break;//("I = 2")
		   	case 0x0180:
				gTC90507Channel[nChannelId].layerinfo.layer_A_Interleave=4;
				CH_PrintDebug("ViewOfdmParam:: Layer A Interleave : 4 \n");
				break;//("I = 4")
		   	case 0x0200:
				gTC90507Channel[nChannelId].layerinfo.layer_A_Interleave=8;
				CH_PrintDebug("ViewOfdmParam:: Layer A Interleave : 8 \n");
				break;//("I = 8")
		   	case 0x0380: //gTC90507Channel[nChannelId].layerinfo.layer_A_Interleave=0;
				CH_PrintDebug("ViewOfdmParam:: Layer A Interleave : Nothing\n");
		   		break;
		}
		break;
	   }

	/* LAYER B */
		Drv_Ch_I2C_DemodRead(nChannelId,    TC90507_B4, &val, 1);//0xb4///D2:0
		index1=(int)val;
		Drv_Ch_I2C_DemodRead(nChannelId,    TC90507_B5, &val, 1);//0xb5//D7:0
		CH_PrintDebug("ViewOfdmParam:: Layer B [B5] :0x%x \n",val);
		index2=(int)val;
		Drv_Ch_I2C_DemodRead(nChannelId,    TC90507_B6, &val, 1);//0xb6//D7:0
		CH_PrintDebug("ViewOfdmParam:: Layer B [B6] :0x%x \n",val);
		index3=(int)val;

		index4=((index2<<8)&0xff00)|(index3&0x00ff);
	  //number of segment//reg[b5][1:0]reg[b6][7:6]
		index4=(index4 & 0x03c0)>>6;
		Num_seg=(unsigned char)(index4);
		gTC90507Channel[nChannelId].layerinfo.layer_B_segments=Num_seg;
		CH_PrintDebug("ViewOfdmParam:: layer_B_segments : %d *****\n", Num_seg);

		//retry reading
		Drv_Ch_I2C_DemodRead(nChannelId,    TC90507_B4, &val, 1);//0xb4///D2:0
		index1=(int)val;
		Drv_Ch_I2C_DemodRead(nChannelId,    TC90507_B5, &val, 1);//0xb5//D7:0
		index2=(int)val;
		Drv_Ch_I2C_DemodRead(nChannelId,    TC90507_B6, &val, 1);//0xb6//D7:0
		index3=(int)val;
		index4=((index1<<8)&0xff00)|(index2&0x00ff);
	  // constellation
	   switch(index4 & 0x0700)//reg[b4][2:0]
	   {
	   case 0x0000:
	   	gTC90507Channel[nChannelId].layerinfo.layer_B_constellation=constellation_DQPSK;
	   	CH_PrintDebug("ViewOfdmParam:: Layer B Constellation : DQPSK\n");
		break;
	   case 0x0100:
	   	gTC90507Channel[nChannelId].layerinfo.layer_B_constellation=constellation_QPSK;
	   	CH_PrintDebug("ViewOfdmParam:: Layer B Constellation : QPSK\n");
		break;
	   case 0x0200:
	   	gTC90507Channel[nChannelId].layerinfo.layer_B_constellation=constellation_16QAM;
	   	CH_PrintDebug("ViewOfdmParam:: Layer B Constellation : 16QAM\n");
		break;
	   case 0x0300:
	   	gTC90507Channel[nChannelId].layerinfo.layer_B_constellation=constellation_64QAM;
	   	CH_PrintDebug("ViewOfdmParam:: Layer B Constellation : 64QAM \n");
		break;
	   case 0x0700:
	   	gTC90507Channel[nChannelId].layerinfo.layer_B_constellation=constellation_NOTHING;
	   	CH_PrintDebug("ViewOfdmParam:: Layer B Constellation : Nothing \n");
		 break;
	   }
	 // HP code rate(Iner code)
	   switch(index4 & 0x00e0)//reg[b5][7:5]
	   {
	   case 0x0000:
	   	gTC90507Channel[nChannelId].layerinfo.layer_B_CodeRate=CodeRate_1_2;
	   	CH_PrintDebug("ViewOfdmParam:: Layer B Code Rate : 1/2\n");
		break;
	   case 0x0020:
	   	gTC90507Channel[nChannelId].layerinfo.layer_B_CodeRate=CodeRate_2_3;
	   	CH_PrintDebug("ViewOfdmParam:: Layer B Code Rate : 2/3\n");
		break;
	   case 0x0040:
	   	gTC90507Channel[nChannelId].layerinfo.layer_B_CodeRate=CodeRate_3_4;
	   	CH_PrintDebug("ViewOfdmParam:: Layer B Code Rate : 3/4\n");
		break;
	   case 0x0060:
	   	gTC90507Channel[nChannelId].layerinfo.layer_B_CodeRate=CodeRate_5_6;
	   	CH_PrintDebug("ViewOfdmParam:: Layer B Code Rate : 5/6\n");
		break;
	   case 0x0080:
	   	gTC90507Channel[nChannelId].layerinfo.layer_B_CodeRate=CodeRate_7_8;
	   	CH_PrintDebug("ViewOfdmParam:: Layer B Code Rate : 7/8\n");
		break;
	   case 0x00e0:
	   	gTC90507Channel[nChannelId].layerinfo.layer_B_CodeRate=CodeRate_NOT;
	   	CH_PrintDebug("ViewOfdmParam:: Layer B Code Rate : Nothing\n");
		break;
       }
	  // LP code rate(Time interleave)
	   switch(ffsize)
	   {
	   case 0x00://	2k
		switch(index4 & 0x001c)//reg[b5][4:2]
		{
			case 0x0000:
				gTC90507Channel[nChannelId].layerinfo.layer_B_Interleave=0;
				CH_PrintDebug("ViewOfdmParam:: Layer B Interleave: 0 \n");
				 break;//("I = 0")
		    	case 0x0004:
				gTC90507Channel[nChannelId].layerinfo.layer_B_Interleave=4;
				CH_PrintDebug("ViewOfdmParam:: Layer B Interleave : 4 \n");
				break;//("I = 4")
		    	case 0x0008:
				gTC90507Channel[nChannelId].layerinfo.layer_B_Interleave=8;
				CH_PrintDebug("ViewOfdmParam:: Layer B Interleave : 8 \n");
				break;//("I = 8")
		    	case 0x000c:
				gTC90507Channel[nChannelId].layerinfo.layer_B_Interleave=16;
				CH_PrintDebug("ViewOfdmParam:: Layer B Interleave : 16 \n");
				break;//("I = 16")
		    	case 0x0010:
				gTC90507Channel[nChannelId].layerinfo.layer_B_Interleave=32;
				CH_PrintDebug("ViewOfdmParam:: Layer B Interleave : 32 \n");
				break;//("I = 32")
	 	    	case 0x001c: //gTC90507Channel[nChannelId].layerinfo.layer_B_Interleave=0;
	 	    		CH_PrintDebug("ViewOfdmParam:: Layer B Interleave: Nothing\n");
		   		break;
	   	}
		break;
	   case 0x40://	4k
		switch(index4 & 0x001c)//reg[b5][4:2]
		{
			case 0x0000:
				gTC90507Channel[nChannelId].layerinfo.layer_B_Interleave=0;
				CH_PrintDebug("ViewOfdmParam:: Layer B Interleave : 0 \n");
				break;//("I = 0")
		   	case 0x0004:
				gTC90507Channel[nChannelId].layerinfo.layer_B_Interleave=2;
				CH_PrintDebug("ViewOfdmParam:: Layer B Interleave : 2 \n");
				break;//("I = 2")
		   	case 0x0008:
				gTC90507Channel[nChannelId].layerinfo.layer_B_Interleave=4;
				CH_PrintDebug("ViewOfdmParam:: Layer B Interleave : 4 \n");
				break;//("I = 4")
		   	case 0x000c:
				gTC90507Channel[nChannelId].layerinfo.layer_B_Interleave=8;
				CH_PrintDebug("ViewOfdmParam:: Layer B Interleave : 8 \n");
				break;//("I = 8")
		   	case 0x0010:
				gTC90507Channel[nChannelId].layerinfo.layer_B_Interleave=16;
				CH_PrintDebug("ViewOfdmParam:: Layer B Interleave : 16 \n");
				break;//("I = 16")
		   	case 0x001c: //gTC90507Channel[nChannelId].layerinfo.layer_B_Interleave=0;
		   		CH_PrintDebug("ViewOfdmParam:: Layer B Interleave : Nothing \n");
		   		break;
	   	}
		break;
	   case 0x80://	8k
		switch(index4 & 0x001c)//reg[b5][4:2]
		{
			case 0x0000:
				gTC90507Channel[nChannelId].layerinfo.layer_B_Interleave=0;
				CH_PrintDebug("ViewOfdmParam:: Layer B Interleave : 0 \n");
				break;//("I = 0")
		   	case 0x0004:
				gTC90507Channel[nChannelId].layerinfo.layer_B_Interleave=1;
				CH_PrintDebug("ViewOfdmParam:: Layer B Interleave : 1 \n");
				break;//("I = 1")
		   	case 0x0008:
				gTC90507Channel[nChannelId].layerinfo.layer_B_Interleave=2;
				CH_PrintDebug("ViewOfdmParam:: Layer B Interleave : 2 \n");
				break;//("I = 2")
		   	case 0x000c:
				gTC90507Channel[nChannelId].layerinfo.layer_B_Interleave=4;
				CH_PrintDebug("ViewOfdmParam:: Layer B Interleave : 4 \n");
				break;//("I = 4")
		   	case 0x0010:
				gTC90507Channel[nChannelId].layerinfo.layer_B_Interleave=8;
				CH_PrintDebug("ViewOfdmParam:: Layer B Interleave : 8 \n");
				break;//("I = 8")
		   	case 0x001c: //gTC90507Channel[nChannelId].layerinfo.layer_B_Interleave=0;
		   		CH_PrintDebug("ViewOfdmParam:: Layer B Interleave : Nothing\n");
		   		break;
		}
		break;
	   }
	/* LAYER C */
		Drv_Ch_I2C_DemodRead(nChannelId,    TC90507_B6, &val, 1);//0xb6//D15:8
		index1=(int)val;
		Drv_Ch_I2C_DemodRead(nChannelId,    TC90507_B7, &val, 1);//0xb7//D7:0
		CH_PrintDebug("ViewOfdmParam:: TC90507_B7 :0x%x  Layer  *****\n",val);
		index2=(int)val;

		index4=((index1<<8)&0xff00)|(index2&0x00ff);
		index4=(index4 & 0x001e)>>1;
	  //number of segment//reg[b7][4:1]
		Num_seg=(unsigned char)(index4);
	  	gTC90507Channel[nChannelId].layerinfo.layer_C_segments=Num_seg;
		CH_PrintDebug("ViewOfdmParam:: layer_C_segments : %d *****\n", Num_seg);

		//retry reading
		Drv_Ch_I2C_DemodRead(nChannelId,    TC90507_B6, &val, 1);//0xb6//D15:8
		index1=(int)val;
		Drv_Ch_I2C_DemodRead(nChannelId,    TC90507_B7, &val, 1);//0xb7//D7:0
		index2=(int)val;
		index4=((index1<<8)&0xff00)|(index2&0x00ff);
	  // constellation
	   switch(index4 & 0x3800)//reg[b6][5:3]
	   {
		   case 0x0000:
		   	gTC90507Channel[nChannelId].layerinfo.layer_C_constellation=constellation_DQPSK;
		   	CH_PrintDebug("ViewOfdmParam:: Layer C Constllation : DQPSK\n");
			break;
		   case 0x0800:
		   	gTC90507Channel[nChannelId].layerinfo.layer_C_constellation=constellation_QPSK;
		   	CH_PrintDebug("ViewOfdmParam:: Layer C Constllation : QPSK \n");
			break;
		   case 0x1000:
		   	gTC90507Channel[nChannelId].layerinfo.layer_C_constellation=constellation_16QAM;
		   	CH_PrintDebug("ViewOfdmParam:: Layer C Constllation : 16QAM \n");
			break;
		   case 0x1800:
		   	gTC90507Channel[nChannelId].layerinfo.layer_C_constellation=constellation_64QAM;
		   	CH_PrintDebug("ViewOfdmParam:: Layer C Constllation : 64QAM \n");
			break;
		   case 0x3800:
		   	gTC90507Channel[nChannelId].layerinfo.layer_C_constellation=constellation_NOTHING;
		   	CH_PrintDebug("ViewOfdmParam:: Layer C Constllation : Nothing\n");
			break;
	   }
	 // HP code rate(Iner code)
	   switch(index4 & 0x0700)//reg[b6][2:0]
	   {
		   case 0x0000:
		   	gTC90507Channel[nChannelId].layerinfo.layer_C_CodeRate=CodeRate_1_2;
		   	CH_PrintDebug("ViewOfdmParam:: Layer C Code Rate : 1/2\n");
			break;
		   case 0x0100:
		   	gTC90507Channel[nChannelId].layerinfo.layer_C_CodeRate=CodeRate_2_3;
		   	CH_PrintDebug("ViewOfdmParam:: Layer C Code Rate : 2/3\n");
			break;
		   case 0x0200:
		   	gTC90507Channel[nChannelId].layerinfo.layer_C_CodeRate=CodeRate_3_4;
		   	CH_PrintDebug("ViewOfdmParam:: Layer C Code Rate : 3/4\n");
			break;
		   case 0x0300:
		   	gTC90507Channel[nChannelId].layerinfo.layer_C_CodeRate=CodeRate_5_6;
		   	CH_PrintDebug("ViewOfdmParam:: Layer C Code Rate : 5/6\n");
			break;
		   case 0x0400:
		   	gTC90507Channel[nChannelId].layerinfo.layer_C_CodeRate=CodeRate_7_8;
		   	CH_PrintDebug("ViewOfdmParam:: Layer C Code Rate : 7/8\n");
			break;
		   case 0x0700:
		   	gTC90507Channel[nChannelId].layerinfo.layer_C_CodeRate=CodeRate_NOT;
		   	CH_PrintDebug("ViewOfdmParam:: Layer C Code Rate : Nothing\n");
			break;
	   }
	  // LP code rate(Time interleave)
	   switch(ffsize)
	   {
	   case 0x00://	2k
		switch(index4 & 0x00e0)//reg[b7][7:5]
		{
			case 0x0000:
				gTC90507Channel[nChannelId].layerinfo.layer_C_Interleave=0;
				CH_PrintDebug("ViewOfdmParam:: Layer C Interleave : 0 \n");
				break;//("I = 0")
		    	case 0x0020:
					gTC90507Channel[nChannelId].layerinfo.layer_C_Interleave=4;
				CH_PrintDebug("ViewOfdmParam:: Layer C Interleave : 4\n");
				 break;//("I = 4")
		    	case 0x0040:
					gTC90507Channel[nChannelId].layerinfo.layer_C_Interleave=8;
				CH_PrintDebug("ViewOfdmParam:: Layer C Interleave : 8 \n");
				break;//("I = 8")
		    	case 0x0060:
					gTC90507Channel[nChannelId].layerinfo.layer_C_Interleave=16;
				CH_PrintDebug("ViewOfdmParam:: Layer C Interleave : 16 \n");
				break;//("I = 16")
		    	case 0x0080:
					gTC90507Channel[nChannelId].layerinfo.layer_C_Interleave=32;
				CH_PrintDebug("ViewOfdmParam:: Layer C Interleave : 32 \n");
				break;//("I = 32")
	 	    	case 0x00e0: //gTC90507Channel[nChannelId].layerinfo.layer_C_Interleave=0;
	 	    		CH_PrintDebug("ViewOfdmParam:: Layer C Interleave : Nothing\n");
	   			break;
		}
		break;
	   case 0x40://	4k
		switch(index4 & 0x00e0)//reg[b7][7:5]
		{
			case 0x0000:
				gTC90507Channel[nChannelId].layerinfo.layer_C_Interleave=0;
				CH_PrintDebug("ViewOfdmParam:: Layer C Interleave : 0 \n");
				break;//("I = 0")
		    	case 0x0020:
				gTC90507Channel[nChannelId].layerinfo.layer_C_Interleave=2;
				CH_PrintDebug("ViewOfdmParam:: Layer C Interleave : 2 \n");
				break;//("I = 2")
		    	case 0x0040:
				gTC90507Channel[nChannelId].layerinfo.layer_C_Interleave=4;
				CH_PrintDebug("ViewOfdmParam:: Layer C Interleave : 4 \n");
				break;//("I = 4")
		   	case 0x0060:
				gTC90507Channel[nChannelId].layerinfo.layer_C_Interleave=8;
				CH_PrintDebug("ViewOfdmParam:: Layer C Interleave : 8 \n");
				 break;//("I = 8")
		    	case 0x0080:
				gTC90507Channel[nChannelId].layerinfo.layer_C_Interleave=16;
				CH_PrintDebug("ViewOfdmParam:: Layer C Interleave : 16 \n");
				break;//("I = 16")
	 	    	case 0x00e0: //gTC90507Channel[nChannelId].layerinfo.layer_C_Interleave=0;
	 	    		CH_PrintDebug("ViewOfdmParam:: Layer C Interleave : Nothing\n");
	   			break;
		}
		break;
	   case 0x80://	8k
		switch(index4 & 0x00e0)//reg[b7][7:5]
		{
			case 0x0000:
				gTC90507Channel[nChannelId].layerinfo.layer_C_Interleave=0;
				CH_PrintDebug("ViewOfdmParam:: Layer C Interleave : 0 \n");
				break;//("I = 0")
		    	case 0x0020:
					gTC90507Channel[nChannelId].layerinfo.layer_C_Interleave=1;
				CH_PrintDebug("ViewOfdmParam:: Layer C Interleave : 1 \n");
				break;//("I = 1")
		    	case 0x0040:
					gTC90507Channel[nChannelId].layerinfo.layer_C_Interleave=2;
				CH_PrintDebug("ViewOfdmParam:: Layer C Interleave : 2\n");
				break;//("I = 2")
		    	case 0x0060:
					gTC90507Channel[nChannelId].layerinfo.layer_C_Interleave=4;
				CH_PrintDebug("ViewOfdmParam:: Layer C Interleave : 4 \n");
				break;//("I = 4")
		    	case 0x0080:
					gTC90507Channel[nChannelId].layerinfo.layer_C_Interleave=8;
				CH_PrintDebug("ViewOfdmParam:: Layer C Interleave : 8 \n");
				break;//("I = 8")
	 	    	case 0x00e0: //gTC90507Channel[nChannelId].layerinfo.layer_C_Interleave=0;
	 	    		CH_PrintDebug("ViewOfdmParam:: Layer C Interleave : Nothing\n");
	   			break;
		}
		break;
	   }

}


void ViewDemodStatus(int  nChannelId)
{

	HUINT8 isdbt=0;

	/* Get the COFDM status : '0'(normal), '1'(error) */

	Drv_Ch_I2C_DemodRead(nChannelId,   TC90507_80, &isdbt,1);
	CH_PrintDebug ("ViewDemodStatus[%d]:: SYNC=0x%x\n", nChannelId, isdbt&0xff);
	//isdbt=val;
	if((isdbt & 0x80)== 0x80)
	{
		gTC90507Status[nChannelId].Isdb_Sync=FALSE;
		CH_PrintDebug("ViewDemodStatus:: Synchronization[7] : ERROR\n");
	}//D[7]
	else
	{
		gTC90507Status[nChannelId].Isdb_Sync=TRUE;
		CH_PrintDebug("ViewDemodStatus:: Synchronization[7] : OK\n");
	}

	if((isdbt & 0x40)== 0x40)
	{
		gTC90507Status[nChannelId].Isdb_Level=FALSE;	// rf signal level error
		CH_PrintDebug("ViewDemodStatus:: Receive Level[6] : ERROR\n");

	}//D[6]
	else
	{
		gTC90507Status[nChannelId].Isdb_Level=TRUE;
		CH_PrintDebug("ViewDemodStatus:: Receive Level[6] : OK\n");
	}

	if((isdbt & 0x20)==0x20)
	{
		gTC90507Status[nChannelId].Isdb_Tmcc=FALSE;       // TMCC is not detected
		CH_PrintDebug("ViewDemodStatus:: TMCC[5] : ERROR\n");
	}//D[5]
	else
	{
		gTC90507Status[nChannelId].Isdb_Tmcc=TRUE;
		CH_PrintDebug("ViewDemodStatus:: TMCC[5] : OK\n");
	}


	if((isdbt & 0x10)==0x10)
	{
		gTC90507Status[nChannelId].Isdb_Recvmd=FALSE;
		CH_PrintDebug("ViewDemodStatus:: Receive Mode[4] : ERROR\n");
	}//D[4]//recvmd(received segment mode), partmd(NOT use)
	else
	{
		gTC90507Status[nChannelId].Isdb_Recvmd=TRUE;
		CH_PrintDebug("ViewDemodStatus:: Receive Mode[4] : OK\n");
	}

	if((isdbt & 0x08)==0x08)
	{
		gTC90507Status[nChannelId].Isdb_Frame=FALSE;
		CH_PrintDebug("ViewDemodStatus:: FRAME[3] : ERROR\n");
	}//D[3]
	else
	{
		gTC90507Status[nChannelId].Isdb_Frame=TRUE;
		CH_PrintDebug("ViewDemodStatus:: FRAME[3] : OK\n");
	}


	if((isdbt & 0x04)==0x04)
	{
		gTC90507Status[nChannelId].Isdb_Viterbi=FALSE; // vitervi decoding error
		CH_PrintDebug("ViewDemodStatus:: VITERBI[2] : ERROR\n");
	}//D[2]
	else
	{
		gTC90507Status[nChannelId].Isdb_Viterbi=TRUE;
		CH_PrintDebug("ViewDemodStatus:: VITERBI[2] : OK\n");
	}

	if((isdbt & 0x02)==0x02)
	{
		gTC90507Status[nChannelId].Isdb_Rs=FALSE;	// occured rs error
		CH_PrintDebug("ViewDemodStatus:: RS[1] : ERROR\n");
	}//D[1]
	else
	{
		gTC90507Status[nChannelId].Isdb_Rs=TRUE;
		CH_PrintDebug("ViewDemodStatus:: RS[1] : OK \n");
	}

	if((isdbt & 0x01)==0x01)
	{
		gTC90507Status[nChannelId].Isdb_Pkt=FALSE;  // occured uncorrected error
		CH_PrintDebug("ViewDemodStatus:: TS Packet[0] : ERROR*\n");
	}//D[0]
	else
	{
		gTC90507Status[nChannelId].Isdb_Pkt=TRUE;
		CH_PrintDebug("ViewDemodStatus:: TS Packet[0] : OK\n");
	}
}

HUINT8 ViewFrameLock(int  nChannelId)
{
	HUINT8	val;

	Drv_Ch_I2C_DemodRead(nChannelId,   TC90507_80, &val, 1);
	CH_PrintDebug ("ViewFrameLock[%d]:: Frame Lock = %x\n", nChannelId, val);

	if ( (val && 0x08)==0x08 )
	{
		CH_PrintDebug ("ViewFrameLock:: Frame Lock Fail\n");
		return FALSE;// unlock
	}
	else
	{
		CH_PrintDebug ("ViewFrameLock:: Frame Lock OK\n");
		return TRUE;// lock
	}
}

HUINT8 ViewTmccDetect(int nChannelId)
{
	HUINT8	val;

	Drv_Ch_I2C_DemodRead(nChannelId,   TC90507_80, &val, 1);
	CH_PrintDebug ("ViewTmccDetect[%d]:: TMCC Detection = %x\n", nChannelId, val);

	if ( (val && 0x20)==0x20 )
	{
		CH_PrintDebug ("ViewTmccDetect:: TMCC Lock Fail\n");
		return FALSE;// unlock
	}
	else
	{
		CH_PrintDebug ("ViewTmccDetect:: TMCC Lock OK\n");
		return TRUE;// lock
	}
}

void ChangeHkfrq(HUINT8 inval)
{
	HKFRQ = 1;				// register 14h
	ghkfrq = inval;
	CH_PrintMon("Change value\n");
}

void ChangeAfcgain(HUINT8 inval)
{
	AFC30 = 1;				// register 30
	gafc = inval;
	CH_PrintMon("Change value\n");
}

void ChangeAffrq(HUINT8 inval)
{
	AFC39 = 1;
	gafc39 = inval;
	CH_PrintMon("Change value\n");
}

void ChangeCpld(HUINT8 inval)
{
	AFC32 = 1;
	gafc32 = inval;
	CH_PrintMon("Change value\n");
}
void IsdbTop(HUINT8 isdb_top, HUINT8 agc_m )
{
	isdb_agc_test = agc_m;
	isdb_if_min = isdb_top;
}

void  IsdbIfset(HUINT8 if_max, HUINT8 if_min, HUINT8 agc_m)
{
	isdb_agc_test = agc_m;
	isdb_if_min = if_min;
	isdb_if_max = if_max;
}

void IsdbRfset(HUINT8 rf_max, HUINT8 rf_min, HUINT8 agc_m)
{
	isdb_agc_test = agc_m;
	isdb_rf_max = rf_max;
	isdb_rf_min = rf_min;
}

void TC90507_Rd(int nChannelId)
{
	int i, count=0;
	HUINT8	val;

	HUINT8	TC90507[0xff] =
	{
	/*
	0x00,0x01,0x02,0x03,0x04,0x05,0x06,0x07,0x08,0x09,0x0a,0x0b,0x0c,0x0d,0x0e,0x0f
	*/
	0x99,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x99,0x99,0x99,0x00,0x99,0x99,0x99,	// 0
	0x99,0x21,0x0b,0x2c,0x10,0x40,0x02,0x70,0x31,0x13,0x31,0x13,0x00,0x00,0x00,0x00,	// 1
	0x4d,0xff,0x00,0x4c,0x4c,0x00,0x00,0x0c,0x60,0x6b,0x40,0x40,0xff,0x00,0xff,0x00,	// 2
	0x28,0x0f,0x9f,0x99,0x3f,0x99,0x99,0x99,0x01,0x5c,0x10,0x30,0x00,0x10,0x08,0x0c,	//3// 3
	0x0c,0x00,0x00,0x4f,0xff,0x99,0x20,0x00,0x90,0xe6,0x02,0x54,0x00,0x99,0x99,0x99,	// 4
	0x04,0x58,0x20,0x99,0x57,0xf1,0x20,0x70,0x60,0x99,0x99,0x99,0x50,0x00,0x99,0x99,	// 5
	0x99,0x99,0x99,0x99,0x99,0x99,0x99,0x99,0x99,0x99,0x99,0x99,0x99,0x99,0x99,0x99,	// 6
	0x18,0x00,0x00,0x99,0x99,0x00,0x02,0x00,0x99,0x99,0x99,0x99,0x00,0x52,0x99,0x99,	// 7
	0x99,0x99,0x99,0x99,0x99,0x99,0x99,0x99,0x99,0x99,0x99,0x99,0x99,0x99,0x99,0x99,	// 8
	0x99,0x99,0x99,0x99,0x99,0x99,0x99,0x99,0x99,0x99,0x99,0x99,0x99,0x99,0x99,0x99,	// 9
	0x99,0x99,0x99,0x99,0x99,0x99,0x99,0x99,0x99,0x99,0x99,0x99,0x99,0x99,0x99,0x99,	// a
	0x00,0x02,0x3c,0x70,0x6f,0xff,0xff,0xff,0xff,0xfc,0x00,0x00,0x00,0x99,0x99,0x99,	// b
	0x99,0x99,0x10,0x99,0x99,0x99,0x99,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,	// c
	0x99,0x99,0x99,0x99,0x99,0x99,0x99,0x99,0x99,0x99,0x00,0x99,0x99,0x99,0x99,0x99,	// d
	0x99,0x99,0x99,0x99,0x1a,0x99,0x99,0x99,0x99,0x99,0x99,0x99,0x00,0x99,0x99,0x00,	// e
	0x99,0x99,0x99,0x99,0x99,0x99,0x99,0x99,0x99,0x99,0x99,0x99,0x99,0x99,0x00		// f
	};

	CH_PrintDebug ("TC90507_Rd()\n");

	for(i=0; i<16; i++)
	{
		CH_PrintMon("[%2x],", i);
	}

	CH_PrintMon("\n");
	CH_PrintMon("------------------------------------------------------------------\n");


	for(i=0; i<0xFE; i++)
	{
		if(TC90507[i] !=	0x99)
		{
			Drv_Ch_I2C_DemodRead(nChannelId,   i, &val, 1);
			CH_PrintMon("[%2x],",  val);
		}
		else
		{
			CH_PrintMon("[%2x],",TC90507[i]);
		}

		count++;

		if(count > 15)
		{
			count = 0;
			CH_PrintMon("\n");
		}
	}
}

#if (0)
void TC90507_RdAll(HUINT32 nChannelId)
{
	int i, count=0;
	HUINT8	val;

	HUINT8	TC90507[0xff] =
	{
	/*
	0x00,0x01,0x02,0x03,0x04,0x05,0x06,0x07,0x08,0x09,0x0a,0x0b,0x0c,0x0d,0x0e,0x0f
	*/
	0x99,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x99,0x99,0x99,0x00,0x99,0x99,0x99,	// 0
	0x99,0x21,0x0b,0x2c,0x10,0x40,0x02,0x70,0x31,0x13,0x31,0x13,0x00,0x00,0x00,0x00,	// 1
	0x4d,0xff,0x00,0x4c,0x4c,0x00,0x00,0x0c,0x60,0x6b,0x40,0x40,0xff,0x00,0xff,0x00,	// 2
	0x28,0x0f,0x9f,0x99,0x3f,0x99,0x99,0x99,0x01,0x5c,0x10,0x30,0x00,0x10,0x08,0x0c,	//3// 3
	0x0c,0x00,0x00,0x4f,0xff,0x99,0x20,0x00,0x90,0xe6,0x02,0x54,0x00,0x99,0x99,0x99,	// 4
	0x04,0x58,0x20,0x99,0x57,0xf1,0x20,0x70,0x60,0x99,0x99,0x99,0x50,0x00,0x99,0x99,	// 5
	0x99,0x99,0x99,0x99,0x99,0x99,0x99,0x99,0x99,0x99,0x99,0x99,0x99,0x99,0x99,0x99,	// 6
	0x18,0x00,0x00,0x99,0x99,0x00,0x02,0x00,0x99,0x99,0x99,0x99,0x00,0x52,0x99,0x99,	// 7
	0x99,0x99,0x99,0x99,0x99,0x99,0x99,0x99,0x99,0x99,0x99,0x99,0x99,0x99,0x99,0x99,	// 8
	0x99,0x99,0x99,0x99,0x99,0x99,0x99,0x99,0x99,0x99,0x99,0x99,0x99,0x99,0x99,0x99,	// 9
	0x99,0x99,0x99,0x99,0x99,0x99,0x99,0x99,0x99,0x99,0x99,0x99,0x99,0x99,0x99,0x99,	// a
	0x00,0x02,0x3c,0x70,0x6f,0xff,0xff,0xff,0xff,0xfc,0x00,0x00,0x00,0x99,0x99,0x99,	// b
	0x99,0x99,0x10,0x99,0x99,0x99,0x99,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,	// c
	0x99,0x99,0x99,0x99,0x99,0x99,0x99,0x99,0x99,0x99,0x00,0x99,0x99,0x99,0x99,0x99,	// d
	0x99,0x99,0x99,0x99,0x1a,0x99,0x99,0x99,0x99,0x99,0x99,0x99,0x00,0x99,0x99,0x00,	// e
	0x99,0x99,0x99,0x99,0x99,0x99,0x99,0x99,0x99,0x99,0x99,0x99,0x99,0x99,0x00		// f
	};

	CH_PrintDebug ("TC90507_Rd()\n");

	for(i=0; i<16; i++)
	{
		CH_PrintMon("[%2x],", i);
	}

	CH_PrintMon("\n");
	CH_PrintMon("------------------------------------------------------------------\n");

	for(i=0; i<0xFE; i++)
	{
		Drv_Ch_I2C_DemodRead(nChannelId,   i, &val, 1);
		CH_PrintMon("[%2x],", val);

		count++;

		if(count > 15)
		{
			count = 0;
			CH_PrintMon("\n");
		}
	}
}
#endif

/* Sleep mode 상태인지 알려주는 함수 */
void Show_Sleep(int nChannelId)
{
	HUINT8 val;

	Drv_Ch_I2C_DemodRead(nChannelId, 	TC90507_81, &val, 1);
	val = (val & 0x10) >> 4;
	CH_PrintDebug ("TC90507_CheckStatus[%d]:: Sleep Mode = %x\n", nChannelId, val);
}

/* onOff 0 : Normal, 1 : ADC power down
static void Set_ADC(int nChannelId, HUINT32 onOff)
{
	HUINT8 val = 0;

	val = (onOff << 5);

	Drv_Ch_I2C_DemodWrite(nChannelId,  TC90507_04, &val, 1);
}
*/

static void Start_Accu(int nChannelId)
{
	accuFlag[nChannelId] = 1;	/* UnCorrected Packet Accumulation Start */
}

/*
static void End_Accu(int nChannelId)
{
	Reset_Accu(nChannelId);
	accuFlag[nChannelId] = 0;	// UnCorrected Packet Accumulation End
}
*/

void View_Accu(int nChannelId)
{
	CH_PrintMon("[View_Accu]Total elapsed Time(sec) : %d\n", accuCnt[nChannelId]*4);	/* 4초마다 1씩 증가하므로 4를 곱하면 전체 경과 시간이 나온다. */
	CH_PrintMon("[View_Accu]Layer A Accumulated UnCorrected Packet : %d\n", accuUnCorPkt[nChannelId][0]);
	CH_PrintMon("[View_Accu]Layer B Accumulated UnCorrected Packet : %d\n", accuUnCorPkt[nChannelId][1]);
	CH_PrintMon("[View_Accu]Layer C Accumulated UnCorrected Packet : %d\n", accuUnCorPkt[nChannelId][2]);
}

static void Reset_Accu(int nChannelId)
{
	/* 현재까지의 결과 보여주고 리셋 */
//	View_Accu(nChannelId);

	accuCnt[nChannelId] = 0;
	accuUnCorPkt[nChannelId][0] = 0;
	accuUnCorPkt[nChannelId][1] = 0;
	accuUnCorPkt[nChannelId][2] = 0;
}

void Set_EmergencyFlag(int nChannelId, HUINT8 flag)
{
	gTC90507Channel[nChannelId].emergencyFlag = flag;
}


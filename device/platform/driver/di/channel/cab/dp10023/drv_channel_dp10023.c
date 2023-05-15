/*************************************************************************************
 *
 *	Ch-state.c
 *
 *	Copyright (c) 2004 by Humax	Corp.
 *

 *  QAM receiver : TDA10023 (Philips Semiconductor)
 -
 -
 -	Symbol rate: 0.56 ~ 9 MHz @ High sampling clock
 -
 -  HW configuration:
 -
 -  1) NTSC (IF = 44.00 MHz)
 -	Tuner (CD1536XLSZ) + SAW (X6965M) + AGC AMP(uPC3217) + XTAL (16MHz) + QAM (TDA10023)

 *
 *************************************************************************************/
#include "di_channel.h"
#include "di_channel_priv.h"
#include "di_channel_c.h"
#include "di_channel_attr.h"

#include "drv_err.h"
#include "drv_i2c.h"
#include "drv_gpio.h"

#include "drv_channel_c.h"
#include "drv_channel_dp10023.h"

#if defined (CONFIG_PLATFORM_TM201COMBO)					// JD - Header Add
#include "sembsl_mxl201.h"
#endif


/****************************************************
	Define
******************************************************/
#define NUMBER_I2C_TRIAL		3

#if defined (CONFIG_PLATFORM_TM201COMBO)
#define NUM_OF_MXLTUNER_REG		0x17
#endif

/****************************************************
	Extern  Variable
******************************************************/
extern HINT32 show_di_ch;

/****************************************************
	Groval Variable
******************************************************/
#define	level_num 	41

HUINT8 	qam_val = 0x50; // phillips 0x5a;

HINT8	downlevel[level_num] = {
84,83,82,81,80,79,78,77,76,75,
74,73,72,71,70,69,68,67,66,65,
64,63,62,61,60,59,58,57,56,55,
54,53,52,51,50,49,48,47,46,45,
44,
};



// VHF Low  ~ 170MHz
HUINT8  RfAgcGain1[level_num] = {
87,90,94,98,102,106,110,113,117,121,
124,128,132,135,140,143,146,150,153,156,
159,162,165,168,171,173,175,179,181,184,
187,189,192,195,197,199,202,205,209,212,
217

};


HUINT8 IfAgcGain1[level_num] = {
150,150,150,150,150,150,150,150,150,150,
150,150,150,150,150,150,150,150,150,150,
150,150,150,150,150,150,150,150,150,150,
150,150,150,150,150,150,150,150,150,150,
150

};


// VHF High 171MHz ~ 182MHz

HUINT8  RfAgcGain2[level_num] = {
89,95,101,108,113,119,124,129,134,139,
143,148,152,155,159,163,166,169,172,175,
178,180,183,186,189,192,194,197,200,203,
205,208,211,214,218,223,231,244,255,255,
255

};


HUINT8 IfAgcGain2[level_num] = {
150,150,150,150,150,150,150,150,150,150,
150,150,150,150,150,150,150,150,150,150,
150,150,150,150,150,150,150,150,150,150,
150,150,150,150,150,150,150,151,152,155,
158

};

// VHF 183~ 302MHz
HUINT8  RfAgcGain3[level_num] = {
60,62,66,69,73,78,84,90,96,103,
110,116,122,129,134,140,145,149,153,157,
160,165,168,172,174,178,182,185,188,191,
194,197,199,203,206,209,213,217,223,231,
252

};


HUINT8 IfAgcGain3[level_num] = {
150,150,150,150,150,150,150,150,150,150,
150,150,150,150,150,150,150,150,150,150,
150,150,150,150,150,150,150,150,150,150,
150,150,150,150,150,150,150,150,150,150,
150

};

// VHF 303~ 472MHz
HUINT8  RfAgcGain4[level_num] = {
62,65,69,73,78,84,90,98,105,113,
121,127,136,140,145,150,154,159,163,167,
169,173,177,179,183,185,188,191,195,198,
201,204,207,211,215,219,226,238,255,255,
255

};

HUINT8 IfAgcGain4[level_num] = {
150,150,150,150,150,150,150,150,150,150,
150,150,150,150,150,150,150,150,150,150,
150,150,150,150,150,150,150,150,150,150,
150,150,150,150,150,150,150,150,151,154,
157

};

// UHF 473MHz ~ 628MHz
HUINT8  RfAgcGain5[level_num] = {
84,88,92,96,100,104,108,111,115,116,
120,123,127,130,133,136,139,143,146,150,
153,158,164,169,176,183,191,202,219,233,
248,255,255,255,255,255,255,255,255,255,
255

};


HUINT8 IfAgcGain5[level_num] = {
150,150,150,150,150,150,150,150,150,150,
150,150,150,150,150,150,150,150,150,150,
150,150,150,150,150,150,150,150,150,150,
150,152,156,159,162,165,168,171,174,178,
181

};

// UHF 629MHz ~718MHz
HUINT8  RfAgcGain6[level_num] = {
70,73,78,82,86,91,95,99,103,107,
112,116,119,123,127,130,133,136,139,142,
146,150,155,159,166,171,178,186,196,206,
218,232,245,253,255,255,255,255,255,255,
255

};


HUINT8 IfAgcGain6[level_num] = {
150,150,150,150,150,150,150,150,150,150,
150,150,150,150,150,150,150,150,150,150,
150,150,150,150,150,150,150,150,150,150,
150,150,151,153,156,159,162,165,168,171,
174

};

// UHF 719MHz ~
HUINT8  RfAgcGain7[level_num] = {
66,68,71,74,78,82,86,91,96,102,
107,111,115,119,123,127,130,134,138,141,
145,149,154,159,163,170,177,185,195,205,
219,234,242,247,255,255,255,255,255,255,
255

};


HUINT8 IfAgcGain7[level_num] = {
150,150,150,150,150,150,150,150,150,150,
150,150,150,150,150,150,150,150,150,150,
150,150,150,150,150,150,150,150,150,150,
150,150,152,153,155,158,161,164,168,171,
174

};

// 671 ~ 707MHz
HUINT8  RfAgcGain8[level_num] = {
82,86,91,95,99,103,107,112,116,119,
123,127,130,133,136,139,142,146,150,155,
159,166,171,178,186,196,206,218,232,245,
253,255,255,255,255,255,255,255,255,255,
255


};


HUINT8 IfAgcGain8[level_num] = {
150,150,150,150,150,150,150,150,150,150,
150,150,150,150,150,150,150,150,150,150,
150,150,150,150,150,150,150,150,150,151,
153,156,159,162,165,168,171,174,177,181,
184


};

/* tuner settling time in msec */
HUINT8	_ucTunerWait;
HUINT8 	gValCP, chgCP;
HUINT8	gValBand, chgBand;

HUINT32	accum_error;

HUINT8	gucCABTopOn = 0; // justin for jc5000 2005-03-08

HUINT8	gucRfMinC = 0x00;
HUINT8   gucRfMaxC  = 0xff;

HUINT8	gucIfMinC = 0x60;
HUINT8	gucIfMaxC = 0xff;


HUINT32   jcom_error;
HUINT8	eq_val =  0x1a; //kansai=0x31; // 0x1a;
HUINT8	cr_val =  0x72; // kansai=0x3f; // 0x32;
HUINT8	agcFlag = 0;
HUINT8	agcVal = 0;

/* TDA10021 register contents */
HUINT8	_ConfReg=0;			// basic configuration contgent
HUINT8	_ClkconfReg=0; 		// for calculating clock offset
HUINT8	_CarconfReg=0; 		// for calculating carrier offset
HUINT8	_GainReg=0;    		// GAIN register content
HUINT8	_NYQgain=0;			// Nyquist filter gain
HUINT32	_SbCarWait=0; 		// Carrier sweep convergence time = 2*SWDYN/SWSTEP*SWLENGTH

HUINT8	_TDAstate=0;
HUINT8	_TDAGain=0;

HUINT16	tsmf_flag, test_gobad;

HUINT8 save_stream;

TDA10023_Status_t			gTDA10023ChInfo[BUS_NUM];
TDA10023_TsmfInfo_t			gTDA10023TsmfInfo[BUS_NUM];
TDA10023_CorrectionInfo_t	gTDA10023Correction[BUS_NUM];

#if defined (CONFIG_PLATFORM_TM201COMBO)
MxL201RF_TunerConfigS	g_stSemTunerConfigCab[BUS_NUM];
#endif
/****************************************************
	Static Variable
******************************************************/
static HBOOL		sTsmfGoodFlag[BUS_NUM];
static HBOOL		sTsmfBadFlag[BUS_NUM];
static HBOOL		sTsmfEmgOnFlag[BUS_NUM];
static HBOOL		sTsmfEmgOffFlag[BUS_NUM];

static HUINT32		BadRetry, GoodRetry;
static HUINT32		beforeId1[BUS_NUM], beforeId2[BUS_NUM];
static HUINT32		gCurrentId[BUS_NUM];
extern HUINT8		save_stream;

static HUINT32 totalCntToDisplayBER[BUS_NUM] = {0,0};
static HUINT32 avgBER[BUS_NUM] = {0,0};
static HUINT32 accuBER[BUS_NUM] = {0,0};


/****************************************************
	Extern Function
******************************************************/
extern	void DRV_CH_CallNotifyCallback ( int nChannelID, HUINT32 unRequestId, DI_CH_SignalInfo_e etSigalInfo );

#if defined (CONFIG_PLATFORM_TM201COMBO)
int Drv_Ch_I2C_DemodRead_CAB(int nChannelId, HUINT16 ucDevAddr , HUINT8 ucRegAddr, HUINT8 *pucBuffer, HUINT32 nNumberToRead);
int Drv_Ch_I2C_DemodWrite_CAB(int nChannelId,  HUINT16 ucDevAddr , HUINT8 ucRegAddr, HUINT8 *pucBuffer, HUINT32 nNumberToWrite);
int Drv_Ch_I2C_TunerRead_CAB(int nChannelId, HUINT8 ucRegAddr, HUINT8 *pucBuffer, HUINT32 nNumberToRead);
int Drv_Ch_I2C_TunerWrite_CAB(int nChannelId, HUINT8 ucRegAddr, HUINT8 *pucBuffer, HUINT32 nNumberToWrite);
#else
extern int Drv_Ch_I2C_TunerRead(int nUnitId, HUINT8 ucRegAddr, HUINT8 *pucBuffer, HUINT32 nNumberToRead);
extern int Drv_Ch_I2C_TunerWrite(int nUnitId, HUINT8 ucRegAddr, HUINT8 *pucBuffer, HUINT32 nNumberToRead);
extern int Drv_Ch_I2C_DemodRead(int nUnitId, HUINT16 ucDevAddr , HUINT8 ucRegAddr, HUINT8 *pucBuffer, HUINT32 nNumberToRead);
extern int Drv_Ch_I2C_DemodWrite(int nUnitId,  HUINT16 ucDevAddr , HUINT8 ucRegAddr, HUINT8 *pucBuffer, HUINT32 nNumberToWrite);
#endif

/****************************************************
	Static Function
******************************************************/
static HUINT8 Set_QAM_Tuner(int nChannelId, HUINT32 freq);

/* Static QAM demoduator-related functions */
static void AdjustCarrierRecovBW(int nChannelId);
//static HUINT8 AdjustGain(int nChannelId);
static HUINT8 calc_no_decimation(int nChannelId);
static HUINT8 set_DecimateFilt(int nChannelId);
static void set_baud_rate(int nChannelId, HUINT8 ndec);
static HUINT16 pow_positive(HUINT16 mantissa, HUINT8 exponent);
static void select_AntialiasFilt(int nChannelId);
static void set_TS_clock_rate(int nChannelId);
static HUINT16 no_to_msec(int nChannelId, HUINT32 symbols);
static HUINT8 check_agc_lock(int nChannelId);
#if 0
static HUINT8 check_fec_lock_TDA(int nChannelId);
static HUINT8 HandleSpectralInversion_TDA(int nChannelId);
static HUINT32 measure_BER_TDA(int nChannelId);
static long measure_carrier_offset(int nChannelId);
static HUINT32 measure_SNR_TDA(int nChannelId);
static HUINT8 BER_Info_PostRSD(int nChannelId);
static void gain_updown_NYQfilt(int nChannelId, HUINT8 updown);
static short get_carrier_offset(int nChannelId);
static short get_carrier_offset_new(int nChannelId);
static short get_clock_offset(int nChannelId);
#endif

static void ActivateDemodulator(int nChannelId);
static void DeactivateDemodulator(int nChannelId);
static void clearDemod(int nChannelId);

#if defined(CONFIG_PLATFORM_TM201COMBO)
static void initDemodPLL(int nChannelId);
static void SetQAMdemod(int nChannelId);
#else
static void InitDemodulator(int nChannelId);
static void DP10023_InitDemodulator(int nChannelId);
#endif

// for test
void  ifset( HUINT8 if_reg_val_max, unsigned char if_reg_val,HUINT8 agc_m);
void rfset(unsigned char rf_reg_val, HUINT8 rf_reg_val_min, HUINT8 agc_m);
void eqset(unsigned char eq_reg_val);
void crset(HUINT8 cr_set);
void mcns(void);

#if defined (CONFIG_PLATFORM_TM201COMBO)														// JD -for JC8500BR (whole section of this #if ~ #endif)
/*#############################################################################################################*/
int Drv_Ch_I2C_DemodRead_CAB(int nChannelId, HUINT16 ucDevAddr , HUINT8 ucRegAddr, HUINT8 *pucBuffer, HUINT32 nNumberToRead)
{

	int nResult = DI_CH_OK;
	DRV_Error nRet = DRV_OK;
	HUINT32 ucI2CBus = 0;
	HINT32	nUnitId;
	HUINT16	ucDemodAddr = 0;

	nUnitId = GetDeviceUnitId( nChannelId );

	if(nChannelId == MASTER_CAB_DEMOD)
	{
		/* I2C Bus Selection */
		ucI2CBus = MASTER_I2C_BUS;

		/* Device Address Selection (Demod or JQAM) */
		if(ucDevAddr == TDA10023_DEMOD_ADDRESS)
		{
			ucDemodAddr = MASTER_DEMOD_ADDR_TDA10023;
		}
		else if(ucDevAddr == TDA10023_JQAM_ADDRESS)
		{
			ucDemodAddr = MASTER_JQAM_ADDR_TDA10023;
		}
		else
		{
			CH_DI_Print(0, ("[Drv_Ch_I2C_DemodRead_CAB] ERR! Wrong I2C Addr! \n"));
		}
	}
	else if(nChannelId == SLAVE_CAB_DEMOD)
	{
		/* I2C Bus Selection */
		ucI2CBus = SLAVE_I2C_BUS;

		/* Device Address Selection (Demod or JQAM) */
		if(ucDevAddr == TDA10023_DEMOD_ADDRESS)
		{
			ucDemodAddr = SLAVE_DEMOD_ADDR_TDA10023;
		}
		else if(ucDevAddr == TDA10023_JQAM_ADDRESS)
		{
			ucDemodAddr = SLAVE_JQAM_ADDR_TDA10023;
		}
		else
		{
			CH_DI_Print(0, ("[Drv_Ch_I2C_DemodRead_CAB] ERR! Wrong I2C Addr! \n"));
		}
	}
	else
	{
		CH_DI_Print(0, ("[Drv_Ch_I2C_DemodRead] ERR! Wrong Unit ID \n"));
	}

	nRet = DRV_I2c_ReadA8( ucI2CBus, ucDemodAddr, ucRegAddr, pucBuffer, nNumberToRead );
	if ( nRet != DRV_OK )
	{
		CH_DI_Print(0, ("[Drv_Ch_I2C_DemodRead_CAB] ERR! nRet [0x%x] \n", nRet));
		nResult = DI_CH_ERR;
	}
	else
	{
		//CH_DI_Print("[0x%x] RegAddr [0x%x] RData [0x%x] \n", ucDevAddr, ucRegAddr, *pucBuffer );
	}

	return nResult;
}

int Drv_Ch_I2C_DemodWrite_CAB(int nChannelId,  HUINT16 ucDevAddr , HUINT8 ucRegAddr, HUINT8 *pucBuffer, HUINT32 nNumberToWrite)
{
	int nResult = DI_CH_OK;
	DRV_Error nRet = DRV_OK;
	HUINT32 ucI2CBus = 0;
	HINT32	nUnitId;
	HUINT16 ucDemodAddr = 0;

	nUnitId = GetDeviceUnitId( nChannelId );

	if(nChannelId == MASTER_CAB_DEMOD)
	{
		/* I2C Bus Selection */
		ucI2CBus = MASTER_I2C_BUS;

		/* Device Address Selection (Demod or JQAM) */
		if(ucDevAddr == TDA10023_DEMOD_ADDRESS)
		{
			ucDemodAddr = MASTER_DEMOD_ADDR_TDA10023;
		}
		else if(ucDevAddr == TDA10023_JQAM_ADDRESS)
		{
			ucDemodAddr = MASTER_JQAM_ADDR_TDA10023;
		}
		else
		{
			CH_DI_Print(0, ("[Drv_Ch_I2C_DemodWrite_CAB] ERR! Wrong I2C Addr! \n"));
		}
	}
	else if(nChannelId == SLAVE_CAB_DEMOD)
	{
		/* I2C Bus Selection */
		ucI2CBus = SLAVE_I2C_BUS;

		/* Device Address Selection (Demod or JQAM) */
		if(ucDevAddr == TDA10023_DEMOD_ADDRESS)
		{
			ucDemodAddr = SLAVE_DEMOD_ADDR_TDA10023;
		}
		else if(ucDevAddr == TDA10023_JQAM_ADDRESS)
		{
			ucDemodAddr = SLAVE_JQAM_ADDR_TDA10023;
		}
		else
		{
			CH_DI_Print(0, ("[Drv_Ch_I2C_DemodWrite_CAB] ERR! Wrong I2C Addr! \n"));
		}
	}
	else
	{
		CH_DI_Print(0, ("[Drv_Ch_I2C_DemodWrite_CAB] ERR! Wrong Unit ID \n"));
	}

	nRet = DRV_I2c_WriteA8( ucI2CBus, ucDemodAddr, ucRegAddr, pucBuffer, nNumberToWrite );
	if ( nRet != DRV_OK )
	{
		CH_DI_Print(0, ("[Drv_Ch_I2C_DemodWrite_CAB] ERR! nRet [0x%x] \n", nRet));
		nResult = DI_CH_ERR;
	}
	else
	{
		//CH_DI_Print("[0x%x] RegAddr [0x%x] RData [0x%x] \n", ucDevAddr, ucRegAddr, *pucBuffer );
	}

	return nResult;
}


int Drv_Ch_I2C_TunerRead_CAB(int nChannelId, HUINT8 ucRegAddr, HUINT8 *pucBuffer, HUINT32 nNumberToRead)
{
	HUINT32					nRet = 0;

	CH_UNUSED(nNumberToRead);

	nRet = MxL_I2C_Read(&g_stSemTunerConfigCab[nChannelId], ucRegAddr, pucBuffer);

	return nRet;
}

int Drv_Ch_I2C_TunerWrite_CAB(int nChannelId, HUINT8 ucRegAddr, HUINT8 *pucBuffer, HUINT32 nNumberToWrite)
{
	HUINT32 				nRet = 0;
	HUINT8					addr_data[2];

	addr_data[0] = ucRegAddr;
	addr_data[1] = *pucBuffer;

	nRet = MxL_I2C_Write(&g_stSemTunerConfigCab[nChannelId], addr_data, nNumberToWrite);

	return nRet;
}

/* Switch on: Host I2C bus is connected to the tuner for tuner programming */
void enable_tuner_iic(int nChannelId)
{
	HUINT8 val;

	/* Tuner data output on TEST pins                 */
	/* GPIO pin is used as interupt line (Active Low) */
	val = R_TEST | 0x80; // set BYPIIC bit
	Drv_Ch_I2C_DemodWrite_CAB(nChannelId,  TDA10023_DEMOD_ADDRESS, SB_TEST, &val, 1);
}

/* Switch off: Host I2C bus is disconnnected from the tuner for reduction of the phase noise */
void disable_tuner_iic(int nChannelId)
{
	HUINT8 val;

	/* No output on TEST pins */
	val = R_TEST; // reset BYPIIC bit
	Drv_Ch_I2C_DemodWrite_CAB(nChannelId,  TDA10023_DEMOD_ADDRESS, SB_TEST, &val, 1);
}

void MxL201_Init_CAB(int nChannelId)
{
	HUINT32					nRet = 0;

	g_stSemTunerConfigCab[nChannelId].nChannelId 		= nChannelId;
	g_stSemTunerConfigCab[nChannelId].Mode				= MxL_MODE_CAB_STD;
	g_stSemTunerConfigCab[nChannelId].Xtal_Freq			= MxL_XTAL_16_MHZ;
	g_stSemTunerConfigCab[nChannelId].IF_Freq			= IFFREQ * 1000;				//MxL_IF_6_MHZ;		/* equal to Demod IF Freq */
	g_stSemTunerConfigCab[nChannelId].IF_Spectrum		= MxL_INVERT_IF;
	g_stSemTunerConfigCab[nChannelId].ClkOut_Setting	= (nChannelId == MASTER_CAB_DEMOD) ? MxL_CLKOUT_ENABLE : MxL_CLKOUT_DISABLE;
	g_stSemTunerConfigCab[nChannelId].ClkOut_Amp		= MxL_CLKOUT_AMP_0;
	g_stSemTunerConfigCab[nChannelId].Xtal_Cap			= MxL_XTAL_CAP_22_PF;
	g_stSemTunerConfigCab[nChannelId].RF_Freq_Hz		= gTDA10023ChInfo[nChannelId].frequency;
	g_stSemTunerConfigCab[nChannelId].BW_MHz			= MxL_BW_6MHz;

	enable_tuner_iic(nChannelId);
	CH_DI_Print(4,("MxL201_Init_CAB[%d]:: Start! \n",nChannelId));
	nRet = MxL_Tuner_Init(&g_stSemTunerConfigCab[nChannelId]);
	if (nRet != DI_CH_OK)
	{
		CH_DI_Print(0,("MxL_Tuner_Init[%d]:: Error! (0x%x)",nChannelId,nRet));
	}
	else
	{
		CH_DI_Print(4,("MxL201_Init_CAB[%d]:: Success! \n",nChannelId));
	}
	disable_tuner_iic(nChannelId);
}

static HUINT8 Set_QAM_Tuner(int nChannelId, HUINT32 freq)
{
#if 1

	HUINT32		nRet = 0;

	enable_tuner_iic(nChannelId);
	CH_DI_Print(2,("Set_QAM_Tuner:: Start! ChannelID[%d]\n", nChannelId));

	g_stSemTunerConfigCab[nChannelId].RF_Freq_Hz		= gTDA10023ChInfo[nChannelId].frequency;
	g_stSemTunerConfigCab[nChannelId].BW_MHz			= MxL_BW_6MHz;

	nRet = MxL_Tuner_RFTune(&g_stSemTunerConfigCab[nChannelId], freq, g_stSemTunerConfigCab[nChannelId].BW_MHz);
	if (nRet != DI_CH_OK)
	{
		CH_DI_Print(0,("MxL_Tuner_RFTune[%d]:: Error! (0x%x)", nChannelId, nRet));
	}
	else
	{
		CH_DI_Print(2,("Set_QAM_Tuner[%d]:: Success! \n", nChannelId));
	}
	disable_tuner_iic(nChannelId);

	return (!nRet);
#else
	HUINT8	CB = 0x00;
	HUINT8	PLL_ReadVal;

	HUINT8	tunerData[6];
	HUINT8	tunerRead;

	float		calcFreq;
	int 		calcBuff;

	int		RSA=0, RSB=0, CP=0, T2=0, T1=0, T0=0, OS=0;
	HUINT32	T_AB, T_TEMP;

	CH_DI_Print(2, ("Set_QAM_Tuner[%d]:: freq = %d \n", nChannelId, freq));

	 /* Samsung DNV22CX */
	_ucTunerWait = 60; // justin 80;

	CH_DI_Print (6, ("SetTuner:: Selected tuner is SAMSUNG DNV22CX \n"));

	if(freq <731000000)
	{
		if(freq < 153000000)
		{
			CB = 0xfe;	/* CP = 125uA */
			CP=1; T2=1; T1=1; T0=1; OS=0;
		}
		else if((freq >= 153000000) && (freq < 173000000))
		{
			CB = 0xb6;	/* CP = 250uA */
			CP=0; T2=1; T1=1; T0=0; OS=0;
		}
		else if((freq >= 173000000) && (freq < 207000000))
		{
			CB = 0xfe;	/* CP = 125uA */
			CP=1; T2=1; T1=1; T0=1; OS=0;
		}
		else
		{
			CB = 0xb6;	/* CP = 250uA */
			CP=0; T2=1; T1=1; T0=0; OS=0;
		}
	}
	else
	{
		CB = 0xb6;	/* CP = 250uA */ 		/* LPP Tuner  731MHz이상에서 QAM uncorrected error 발생 CP 650uA -> 250uA 변경  */
		CP=0; T2=1; T1=1; T0=0; OS=0;

	}

	// QAM (Step_Size = 62.5)
	calcFreq = (freq/62500) + (44000000/62500);

	calcBuff = (int)calcFreq;

	RSA=1, RSB=1;

	/* Divider Byte1 */
	tunerData[0] = (calcBuff >> 8) & 0x007f;

	/* Divider Byte2 */
	tunerData[1] = (calcBuff) & 0x00ff;

	/* Control Byte1 */
	tunerData[2] = CB;

	/* Band Switch Byte */
	if(freq <= 169000000)
	{
		tunerData[3] = 0x01; /* Band Switch Byte : BS1 */
		CH_DI_Print(5,("SetTuner:: BS1  tunerData[3] = %x\n", tunerData[3]));
	}
	else if(freq <= 469000000)
	{
		tunerData[3] = 0x02; /* Band Switch Byte : BS2 */
		CH_DI_Print(5, ("SetTuner:: BS2  tunerData[3] = %x\n", tunerData[3]));
	}
	else if(freq <= 810000000)
	{
		tunerData[3] = 0x10; /* Band Switch Byte : BS3 */
		CH_DI_Print(5, ("SetTuner:: BS3  tunerData[3] = %x\n", tunerData[3]));
	}
	else
	{
		CH_DI_Print(5,( "SetTuner:: ERROR : Out of frequency range for tuner\n"));
	}

	// CONTROL BYTE (AB)
	T_AB = 0x30;

	// FOR making CB byte to WRITING Auxiliary Byte   For making T2,T1,T0 = 0, 1, 1
	 T_TEMP = 0x80 | CP<<6 | 0<<5 | 1<<4 | 1<<3 | RSA<<2 | RSB<<1 | OS;


	tunerData[4] = (T_TEMP&0xdf)|0x18;      // save input T_BB 8bit data into dataout[5], For making T2,T1,T0 = 0, 1, 1
	tunerData[5] = T_AB&0xff;			     // save input T_AB 8bit data into dataout[6]

	CH_DI_Print(3, (" Set_QAM_Tuner[%d]:: [0]=0x%x, [1]=0x%x, [2]=0x%x, [3]=0x%x, [4]=0x%x, [5]=0x%x \n",
								nChannelId,tunerData[0],tunerData[1],tunerData[2],tunerData[3], tunerData[4], tunerData[5]));

	/* tuner write */
	Drv_Ch_I2C_TunerWrite_CAB(nChannelId, S22CX_TUNER_ADDRESS, &tunerData[0], 6);

	/* wait for the tuner settling time */
	VK_TASK_Sleep(_ucTunerWait);

	/* check if the PLL in tuner is locked */
	Drv_Ch_I2C_TunerRead_CAB(nChannelId, S22CX_TUNER_ADDRESS, &tunerRead, 1);

	PLL_ReadVal = (tunerRead & 0x40)? 1:0;

	if (!PLL_ReadVal)
	{
		CH_DI_Print(1, ("[Set_QAM_Tuner] PLL in the tuner UNLOCK!\n"));
	}

	return PLL_ReadVal;
#endif
}

static int DP10023_SetTuner(int nChannelId)
{
	HUINT8 Tnr_StatusByte = 0, Tnr_WriteNo = 0;

	/* activate the Demodulator and set the basic QAM configuration
	   like constellation, input data format and spectral inversion */
	ActivateDemodulator(nChannelId);

	/* set the tuner's PLL register */
	while( (Tnr_WriteNo < NUMBER_I2C_TRIAL) && !Tnr_StatusByte )
	{
		MxL201_Init_CAB(nChannelId);
		Tnr_StatusByte = Set_QAM_Tuner(nChannelId, gTDA10023ChInfo[nChannelId].frequency);
		Tnr_WriteNo++;
	}

	if(!Tnr_StatusByte)
	{
		CH_DI_Print (1, (" TDA10023_SetTuner[%d]:: Tuner Lock Fail!\n", nChannelId ));
		return DI_CH_ERR;
	}
	else
	{
		CH_DI_Print (1, (" TDA10023_SetTuner[%d]:: Tuner Lock Ok!\n", nChannelId));
		return DI_CH_OK;
	}
}

/* make demodulator idle at the STAND-BY mode */
/* Called by Drv_Tuner_Standby() at channe.c  */

static void DeactivateDemodulator(int nChannelId)
{
	CH_UNUSED(nChannelId);

#ifndef JC5000N
		HUINT8 val;
		CH_DI_Print (3, ("DeactivateDemodulator[%d]:: Power-Down mode\n", nChannelId));

		/* set STDBY bit of CONF to one */
		val = 0x80 | _ConfReg;
		Drv_Ch_I2C_DemodWrite_CAB(nChannelId,  SB_CONF, &val, 1);

		/* ADC stand-by mode */
		val = 0xae; // justin 0x33;
		Drv_Ch_I2C_DemodWrite_CAB(nChannelId,  TDA10023_DEMOD_ADDRESS, SB_ADC, &val, 1);
#endif
}


/* wake up the demodulator and set the basic QAM configuration
   like constellation, input data format and spectral inversion */
static void ActivateDemodulator(int nChannelId)
{
	CH_UNUSED(nChannelId);

#ifndef JC5000N
	char    qam_name[5];
	HUINT8 	val;
	CH_DI_Print(3, ("ActivateDemodulator[%d]:: Activation of TDA10023\n", nChannelId));

   	switch (gTDA10023ChInfo[nChannelId].qamMode)
   	{
        case 1 : //16-QAM
			VK_strcpy(qam_name, "16");
        	_ConfReg    = 0x63;
			break;
        case 2 : //32-QAM
			VK_strcpy(qam_name, "32");
        	_ConfReg    = 0x67;
			break;
        case 3 : //64-QAM
			VK_strcpy(qam_name, "64");
        	_ConfReg    =  0x2b; // justin 040329  0x2b; // justin 0x6b;
			break;
        case 4 : //128-QAM
			VK_strcpy(qam_name, "128");
        	_ConfReg    = 0x6f;
			break;
		case 5 : //256-QAM
			VK_strcpy(qam_name, "256");
        	_ConfReg    = 0x73;
			break;
		default:
			VK_strcpy(qam_name, "64");
        	_ConfReg    = 0x2b;
			break;
	}

	/* set the CONF register */
	val = _ConfReg;
	Drv_Ch_I2C_DemodWrite_CAB(nChannelId,  TDA10023_DEMOD_ADDRESS, SB_CONF, &val, 1);
#endif
}

/* _________________ QAM demodulator specific functions __________________*/
// TDA10023-specific functions
/***************************************************************************
Performs QAM receiver locking trial based on Nyquist filter of the fixed
gain, then if the receiver is locked, it returns TRUE.
****************************************************************************/
HUINT8 fast_lock_TDA(int nChannelId)
{
	HUINT8	val1,  val;
	int i = 0, nDelay = 0, nRetryCount=0;

   	gTDA10023ChInfo[nChannelId].strength = 0;
   	gTDA10023ChInfo[nChannelId].quality = 0;
	gTDA10023ChInfo[nChannelId].snr = 0;
	jcom_error = 0;  // 화면 깨지는지 보기위서 넣었음.

	nDelay = 100;
	nRetryCount = 3;

	for(i=0; i< nRetryCount; i++)
	{
		val1 =	0x93; // phillips R_AGCONF_SLOW; // R_AGCONF_FAST; // justin_4 R_AGCONF_SLOW;
		Drv_Ch_I2C_DemodWrite_CAB(nChannelId,  TDA10023_DEMOD_ADDRESS, SB_AGCCONF, &val1, 1);

		VK_TASK_Sleep( (HUINT16)(no_to_msec(nChannelId, WAIT_AGCLOCK)) );	// 76ms

		if( check_agc_lock(nChannelId) )
		{
			CH_DI_Print (1, ("fast_lock_TDA[%d]::  Detected QAM signal\n", nChannelId));

		}
		else
		{
			CH_DI_Print (1, ("fast_lock_TDA[%d]::  Failed to detect QAM signal\n", nChannelId));
			return FALSE;
		}

		VK_TASK_Sleep(nDelay);

		val1 = 0x9b; // 011 011 scs_nerima phillps 0xbf; // 0x9b; // justin_4 0x93; // justin 0x13; // justin 0x03;
		Drv_Ch_I2C_DemodWrite_CAB(nChannelId, TDA10023_DEMOD_ADDRESS, SB_AGCCONF, &val1, 1);

		/* stop fine AGC using the center tap and use all coeffients of equalizer */
		val = R_EQCONF2ND; // CTADAPT bit=0
		Drv_Ch_I2C_DemodWrite_CAB(nChannelId, TDA10023_DEMOD_ADDRESS,  SB_EQCONF2, &val, 1);

		Drv_Ch_I2C_DemodRead_CAB(nChannelId, TDA10023_DEMOD_ADDRESS, SB_SYNC, &val1, 1);

		if((val1&0x4f) == 0x0f)
		{
			CH_DI_Print (1, ("fast_lock_TDA[%d]:: Locked... Num = %d\n", nChannelId, i));
			AdjustCarrierRecovBW(nChannelId);
			return TRUE;
		}
		else
		{
			clearDemod(nChannelId);
		}
	}

	CH_DI_Print(1, ("fast_lock_TDA[%d]:: Lock Fail! SB_SYNC[0x11] = %x, Num = %d\n", nChannelId, val1&0x4f, i));

       return FALSE;
}

/********************************************************************
 This soft reset does not affect the internal I2C register contents.
 There are two bits for reset operation: CLB(b7, LSB) & CLB2(b6)
 CLB is automatically return to 1 after 5 XIN clock period, but
 CLB2 does not automatically return to 1.
*********************************************************************/
static void clearDemod(int nChannelId)
{
	HUINT8 	val;

	switch (gTDA10023ChInfo[nChannelId].qamMode)
	{
	    case 1 : //16-QAM
	    	_ConfReg    = 0x63; break;
	    case 2 : //32-QAM
	    	_ConfReg    = 0x67; break;
	    case 3 : //64-QAM
	    	_ConfReg    =  0x2b; break; // justin 0x6b;	break;
	    case 4 : //128-QAM
	    	_ConfReg    = 0x6f;	break;
		case 5 : //256-QAM
	    	_ConfReg    = 0x73;	break;
		default:
	    	_ConfReg    = 0x6b;	break;
	}

	/* set the value of the CONF register */
	val = _ConfReg;
	//Drv_Ch_I2C_DemodWrite_CAB(SB_CONF, &val, 1);

	/* soft-reset the QAM logic */
	/* set the CLR bit to zero. CLB signal will automatically return to 1 after 5 XIN period */
	val = _ConfReg & 0xfe;
	Drv_Ch_I2C_DemodWrite_CAB(nChannelId,  TDA10023_DEMOD_ADDRESS, SB_CONF, &val, 1);
}

/********************************************************************
Configure QAM demodulator PLL:
	- set demodulator PLL  / refer TDA10023 NOTE
*********************************************************************/
static void initDemodPLL(int nChannelId)
{
	unsigned char	QAMreg[3];
	unsigned char	reg_val;

	CH_DI_Print(5, ("initDemodPLL[%d]... \n", nChannelId));

	/* 1) mdiv, ndiv, pll: PLL related setting */
	QAMreg[0x00] = 0x2b; // 0x23;  //0x2b; /* PLL1 */
	QAMreg[0x01] = 0x44;	/* PLL2 */
	QAMreg[0x02] = 0x0c;  /* PLL3 */

	/* 2) TDA10023 & TDA10024 Data Sheet "PDPLL"  */
	reg_val = QAMreg[0x02] | 0x03;
	Drv_Ch_I2C_DemodWrite_CAB(nChannelId,  TDA10023_DEMOD_ADDRESS, SB_PLL, &reg_val, 1);

	/* 3) M,N,P loading */
	Drv_Ch_I2C_DemodWrite_CAB(nChannelId,  TDA10023_DEMOD_ADDRESS, SB_MDIV, &QAMreg[0x00], 2);

	/* 4) Finaly launched by setting BYPPLL =0 and PDPLL = 0 */
	reg_val = QAMreg[0x02];
	Drv_Ch_I2C_DemodWrite_CAB(nChannelId,  TDA10023_DEMOD_ADDRESS, SB_PLL, &reg_val, 1);
}

/********************************************************************
Configure QAM demodulator:
	- clear 7-bit packet error counter
	- set the constellation size & spectrum polarity
	- configure the equalizer (mode and step size)
	- set AGCREF, LOCKTHR, MSETH and AREF value
	- set the carrier and symbol recovery loop parameter
	- set AAF and DF
*********************************************************************/
void InitQAMdemod(int nChannelId)
{
	HUINT8	QAMreg[0x5];
	HUINT8	val, uc_val;
	HUINT32	swdyn, swstep, swlength;
	HINT32	lDeltaF;

   	swlength = 0;
	swstep = 145;
	swdyn = 0;

	CH_DI_Print(5, ("InitQAMdemod[%d]...\n", nChannelId));

	/* Init Demod PLL */
	initDemodPLL(nChannelId);

	/* adc: internal ADC used */
	/* BYSONG, 20070723, CS = 10(2) */
	uc_val =  0xc8; // 0x88;
	Drv_Ch_I2C_DemodWrite_CAB(nChannelId,  TDA10023_DEMOD_ADDRESS, SB_ADC, &uc_val, 1);

	/* intp: set TS stream clock */
   	set_TS_clock_rate(nChannelId);

	/* rsconf: BER depth = 1.0E-7 */
	val = R_RSCONF3; // set CLB_UNC bit
	Drv_Ch_I2C_DemodWrite_CAB(nChannelId,  TDA10023_DEMOD_ADDRESS, SB_RSCONF, &val, 1);

	/* itsel: active LOW interrupt control */
	uc_val = 0x00; // justin 0x40; // interrupt disabled and output level is "1" always
	Drv_Ch_I2C_DemodWrite_CAB(nChannelId,  TDA10023_DEMOD_ADDRESS, SB_ITSEL, &uc_val, 1);

	/* pwmref: meaningful if IF AGC is inactive */
	uc_val = 0x00; // justin 0x80;
	Drv_Ch_I2C_DemodWrite_CAB(nChannelId, TDA10023_DEMOD_ADDRESS, SB_PWMREF, &uc_val, 1);

	/* pola2/ints: controls the second output stream */
	/* check bytes are not provided and all of serial
	   TS output pins will be in tri-state           */
	val = 0xa1; // INTS1 serial setting
	Drv_Ch_I2C_DemodWrite_CAB(nChannelId,  TDA10023_DEMOD_ADDRESS, SB_INTS1, &val, 1);

	/* sweep: SWEEP control */
   	QAMreg[0x00] = 0x86; // justin  추후 테스트 해볼것 SWEEP  current 2.34%  0xcb; // best, 11.7% of SR, better than 0xc9

	/* in order to calculate the time to acquire the carrier lock */
    switch ( (QAMreg[0x00]&0x70) >> 4)
    {
        case 0 :
        	swdyn = 2340; 	break;
        case 1 :
        	swdyn = 4680; 	break;
        case 2 :
        	swdyn = 7000; 	break;
        case 3 :
        	swdyn = 9375; 	break;
        case 4 :
        	swdyn = 11700; 	break;
        case 5 :
        	swdyn = 14060; 	break;
        case 6 :
        	swdyn = 16400; 	break;
        case 7 :
        	swdyn = 18750; 	break;
	}

    switch ( (QAMreg[0x00]&0x0c) >> 2 )
    {
        case 0 :
        	swstep = 145; 	break;
        case 1 :
        	swstep = 290; 	break;
        case 2 :
        	swstep = 580; 	break;
        case 3 :
        	swstep = 1170; 	break;
	}

    switch (QAMreg[0x00]&0x03)
    {
        case 0 :
        	swlength = 4096; 	break;
        case 1 :
        	swlength = 8192; 	break;
        case 2 :
        	swlength = 16384; 	break;
        case 3 :
        	swlength = 32768; 	break;
	}

	/* Factor 2 is multiplied for sweep of the range: -swdyn ~ +swdyn */
	_SbCarWait = 2*swlength*swdyn/swstep;
//	OSA_Print(" No. of symbol for carrier sweep = %d\n", _SbCarWait);

	/* agcconf2: If 5V tolerant configuration is selected, *
	 * the receiver is more sensitive to AGC linearity     *
	 * problem due to a pull-up resister used.             */
	QAMreg[0x01] = 0x30; // justin 0xb0; // justin 0x08; // both AGC outputs are configured as normal output type
	Drv_Ch_I2C_DemodWrite_CAB(nChannelId,  TDA10023_DEMOD_ADDRESS, SB_SWEEP, &QAMreg[0x00], 2);

	/* RF AGC control */
	if(gucCABTopOn == 1)
	{
		// 새로운 saw filter적용시 0xff
		QAMreg[0x00] = gucRfMaxC; //0xbf; // d0; // 0x75; /* RF AGC max threshold value */
		QAMreg[0x01] = gucRfMinC; /* RF AGC min threshold value */
	}
	else
	{
		QAMreg[0x00] = 0xff;
		QAMreg[0x01] = 0x00;
	}

	if (IFFREQ*1.6*1000 == SYSCLK )
	{
		QAMreg[0x02] =0x00;
		QAMreg[0x03] =0x80;
	}
	else
	{
#if(1)
		/* 20070723, FSAMPLING = 1 */
		lDeltaF = (HINT32)(((8.0*(IFFREQ - SYSCLK))/SYSCLK)*100);
		lDeltaF = (-4096.0*(500 + lDeltaF))/100;

		QAMreg[0x02] = (HUINT8)lDeltaF;
		QAMreg[0x03] = (HUINT8)(((lDeltaF>>8) & 0x7F) | 0x80);

		CH_DI_Print(4, ("InitQAMdemod %x, %x\n", QAMreg[0x02],QAMreg[0x03]));
#else
		/* 20070712, FSAMPLING = 0 */
		lDeltaF = (HINT32)(((4.0*(IFFREQ - (SYSCLK)))/(SYSCLK))*100);
		lDeltaF = (-8192.0*(100 + lDeltaF))/100;

		QAMreg[0x02] = (HUINT8)lDeltaF;
		QAMreg[0x03] = (HUINT8)(((lDeltaF>>8) & 0x7F) | 0x80);
		CH_DI_Print("InitQAMdemod %x, %x\n", QAMreg[0x02],QAMreg[0x03]);
#endif
	}
	Drv_Ch_I2C_DemodWrite_CAB(nChannelId,  TDA10023_DEMOD_ADDRESS, SB_TUNMAX, &QAMreg[0x00], 4);

	/* IF AGC control: IF_MIN determines TOP of the tuner */
	if(gucCABTopOn == 1)
	{
		QAMreg[0x00] = gucIfMaxC; // 0xff; // IF AGC max threshold value
		QAMreg[0x01] = gucIfMinC;
	}
	else
	{
		QAMreg[0x00] = 0xff; // IF AGC max threshold value
		QAMreg[0x01] = 0x00; // 0x96; //20101125
	}
	Drv_Ch_I2C_DemodWrite_CAB(nChannelId,  TDA10023_DEMOD_ADDRESS, SB_IFMAX, &QAMreg[0x00], 2);

	val =  0x20;
	Drv_Ch_I2C_DemodWrite_CAB(nChannelId,  TDA10023_DEMOD_ADDRESS, SB_AGCCONF2, &val, 1);

	val=  0xa3;
	Drv_Ch_I2C_DemodWrite_CAB(nChannelId,  TDA10023_DEMOD_ADDRESS, 0xbe, &val, 1);

	val= 0x01;
	Drv_Ch_I2C_DemodWrite_CAB(nChannelId,  TDA10023_DEMOD_ADDRESS, 0x13, &val, 1);
}

/* Reduce the carrier recovery loop's acquisition range according to SR */
/* Affects performance of the reciever according to the selected value  */
void AdjustCarrierRecovBW(int nChannelId)
{
	HUINT8 val;

#if 1 //v1.x. better performance than v2.0
	/* set the carrier recovery loop's acquisition range and loop BW */
	if(gTDA10023ChInfo[nChannelId].symbolRate >= 3000)
		val = R_CARCONF_HIGH_SR_TRK; //0x0A(acquision phase) => 0x02 (tracking phase)
	else if(gTDA10023ChInfo[nChannelId].symbolRate >= 1500)
		val = R_CARCONF_MID_SR_TRK; //0x0C => 0x05
	else
		val = R_CARCONF_LOW_SR_TRK; //0x0D => 0x09
#else //v2.0
	/* change the carrier recovery loop bandwidth according to to symbol rate */
	val = 0x05;
	if (gTDA10023ChInfo.symbolRate < 3000)
		val = 0x02;
	if ( (gTDA10023ChInfo.symbolRate < 1750) && (gTDA10023ChInfo.qamMode < 4) )
		val = 0x0a;
#endif

	Drv_Ch_I2C_DemodWrite_CAB(nChannelId,  TDA10023_DEMOD_ADDRESS, SB_CARCONF, &val, 1);
	_CarconfReg	= val;
}

#if 0
/* if power of the center taps of the equalizer is threshold value,
   it means that the equalizer input signal level is weak.
   ======> increase the gain of the Nyquist filter gain             */
HUINT8 AdjustGain(int nChannelId)
{
	HUINT32 	CtapPower;
	HUINT8 	ctap, ctap_I, ctap_Q;

	signed char	pCoef_I[2], pCoef_Q[2];
	signed long CentralCoef_I, CentralCoef_Q;

	while (_NYQgain < NYQ_AUTOGAIN_MAX)
	{
		/* determine the position of the central tap */
		ctap = (R_EQCONF_ON>>4) + 2;
		//OSA_Print("EQ center tap location and index: %d, ", ctap_I);

		/* determine the register index of the central tap */
		ctap_I = SB_REQCO_H0 + (ctap-1)*2;
		ctap_Q = SB_IEQCO_H0 + (ctap-1)*2;
		//OSA_Print("%d\r\n", ctap_I);

		/* read the real and imaginary part of the central coef of the equalizer */
		Drv_Ch_I2C_DemodRead_CAB(nChannelId,  TDA10023_DEMOD_ADDRESS, ctap_I, (HUINT8*)&pCoef_I[0], 2);
		Drv_Ch_I2C_DemodRead_CAB(nChannelId,  TDA10023_DEMOD_ADDRESS, ctap_Q, (HUINT8*)&pCoef_Q[0], 2);

		CentralCoef_I = (HUINT32)(pCoef_I[0] << 3 | pCoef_I[1] >> 5); // 11 bits size coefficient
		CentralCoef_Q = (HUINT32)(pCoef_Q[0] << 3 | pCoef_Q[1] >> 5); // 11 bits size coefficient

		/* convert minus vale into plus value */
		if(CentralCoef_I & 0x400)
			CentralCoef_I = -CentralCoef_I;	/* sign extention */
		if(CentralCoef_Q & 0x400)
			CentralCoef_Q = -CentralCoef_Q;	/* sign extention */

		/* CentralCoef_I amplitude calculation */
		CtapPower = CentralCoef_I + CentralCoef_Q;

		/* check if amp of the central tap coefficient exceeds the threshold value (700) */
		if ( CtapPower > 700 )
		{
			/* try next gain */
			gain_updown_NYQfilt(nChannelId, NYQ_GAIN_UP);
			CH_DI_Print("AdjustGain:: EQ Ctap power=%d: NYQ filt gain increased to %d\n",CtapPower,_NYQgain);

			/* wait for filter settlement */
			VK_TASK_Sleep( (HUINT16)(no_to_msec(nChannelId, WAIT_GNYQLOCK)) );
		}
		else
			return TRUE;
	}
	return FALSE;
}

/* Gain control: updown=0 (Gain down), updown=1 (Gain Up) */
void gain_updown_NYQfilt(int nChannelId, HUINT8 updown)
{
	HUINT8 NewGain, val;

	if (updown == NYQ_GAIN_UP)
		NewGain = _NYQgain+1;
	else if(_NYQgain > 0)
		NewGain = _NYQgain-1;
	else
	{
		NewGain = _NYQgain;
		CH_DI_Print("gain_updown_NYQfilt:: Minimun gain reached\n");
	}

	val = (_GainReg&0x8f) | (NewGain<<4); // justin val = (_GainReg&0x1f) | (NewGain<<5);

	/* Update the gain */
	_NYQgain = NewGain;
	_GainReg = val;
	CH_DI_Print ("gain_updown_NYQfilt:: New gain = %x\n", val);
	Drv_Ch_I2C_DemodWrite_CAB(nChannelId, TDA10023_DEMOD_ADDRESS,  SB_GAIN, &val, 1);
}
#endif


/**************************************************************************
 The number of decimation is determined by reference frequency and symbol
 rate. Symbol rate is variable according to payload data of a channel.
****************************************************************************/
HUINT8 calc_no_decimation(int nChannelId)
{
	HUINT32 SymbolRate;

	SymbolRate = gTDA10023ChInfo[nChannelId].symbolRate;

	if( SymbolRate > SYSCLK/16 )
		return 0;
	else if( SymbolRate > SYSCLK/32)
		return 1;
	else if( SymbolRate > SYSCLK/64)
		return 2;
	else if( SymbolRate >  SYSCLK/128)
		return 3;
	else
	{
		CH_DI_Print (1, ("calc_no_decimation[%d]:: SR of %d KHz is out of range: LOW\r\n",nChannelId,gTDA10023ChInfo[nChannelId].symbolRate));
		return 0;
	}
}

/***************************************************************
  Control the decimators in the baud rate recovery loop:
 - Set the number of decimation
 - Set the initial loop BW as large. Later the BW will be reduced.
****************************************************************/
HUINT8 set_DecimateFilt(int nChannelId)
{
	HUINT8	no_decimate, clkconf_val;

	/* Get the number of decimation */
	no_decimate = calc_no_decimation(nChannelId);
	clkconf_val = no_decimate << 6;

	/* Updates the the baud rate recovery loop register value */
	clkconf_val = _ClkconfReg | clkconf_val;
	_ClkconfReg = clkconf_val;
	Drv_Ch_I2C_DemodWrite_CAB(nChannelId,  TDA10023_DEMOD_ADDRESS, SB_CLKCONF, &clkconf_val, 1);

	/* the number of DEC will be used when the baud rate and its inverse are set */
	return no_decimate;
}

/* set the baud rate and its inverse according *
 * to symbol rate and reference frequency      */
void set_baud_rate(int nChannelId, HUINT8 ndec)
{
	HUINT32 	BaudRate, InvBaudRate;
	HUINT32	temp;
	HUINT8	val[4];
	HUINT8	no_decimate;

	no_decimate = ndec;

#if 0 // justin
	/* set the baud rate */
	/* 16,777,216 = 2**24, SYSCLK = 57800 = 7225 * 8 */
	/* = 2**24 / 57800 = 2**21 / 7225 = 2097152/7225 */
	/* = SR*16777216uL*2**no_decimate/SYSCLK         */
#endif

	temp = SYSCLK/8;
	BaudRate = ((((2097152.0/temp)*gTDA10023ChInfo[nChannelId].symbolRate))*pow_positive(2,no_decimate) + 0.5); // justin add 0.5 for Nearest integer vale

	val[0] = BaudRate & 0xff;  // justin 0x83
	val[1] = (BaudRate>>8) & 0xff; // justin 0xc0
	val[2] = (BaudRate>>16) & 0xff; // justin 0x12

	/* set the baud rate inverse */
	InvBaudRate = ((16.0*SYSCLK) / ( gTDA10023ChInfo[nChannelId].symbolRate *pow_positive(2, no_decimate)) + 0.5);  // justin add 0.5 for Nearest integer vale

	val[3] = ( InvBaudRate > 255)? 255:InvBaudRate;

//	OSA_Print("### %x, %x, %x, %x\n", val[0], val[1], val[2], val[3]); // justin_2

	Drv_Ch_I2C_DemodWrite_CAB(nChannelId,  TDA10023_DEMOD_ADDRESS, SB_BDR_LSB, &val[0], 4);
}


/* math function which calulate power of exponential */
HUINT16 pow_positive(HUINT16 mantissa, HUINT8 exponent)
{
	HUINT8	i;
	HUINT16	result = 1;

	if(mantissa == 0)
	{
		CH_DI_Print(1, ("pow_positive:: pow_positive(mantissa=0) error!  \n"));
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


/* select which anti-aliasing filter will be used */
void select_AntialiasFilt(int nChannelId)
{
	HUINT8	falias, gain_val;
	HUINT32 	SymbolRate;
	HUINT8	gain2_val;

	SymbolRate =  gTDA10023ChInfo[nChannelId].symbolRate / 10;

	if( SymbolRate > SYSCLK / 80)
	{
		CH_DI_Print (1, ("select_AntialiasFilt[%d]:: Error! SR=%d  is too HIGH\r\n",nChannelId,gTDA10023ChInfo[nChannelId].symbolRate));
		falias = 0;
	}
	else if( SymbolRate > SYSCLK/123 )
		falias = 0;
	else if( SymbolRate > SYSCLK/160 )
		falias = 1;
	else if( SymbolRate > SYSCLK/246 )
		falias = 0;
	else if( SymbolRate > SYSCLK/320 )
		falias = 1;
	else if( SymbolRate > SYSCLK/492 )
		falias = 0;
	else if( SymbolRate > SYSCLK/640 )
		falias = 1;
	else if( SymbolRate > SYSCLK/984 )
		falias = 0;
	else if( SymbolRate > SYSCLK/1280 )
		falias = 1;
	else
	{
		CH_DI_Print (1, ("select_AntialiasFilt[%d]:: Error! SR=%d  is too LOW\r\n",nChannelId,gTDA10023ChInfo[nChannelId].symbolRate));
		falias = 0;
	}

	gain2_val = ( (falias<<7) | (NYQ_ROLLOFF & 0x03) | ( (NYQ_GAAF << 4 ) & 0x70) ) ;

	CH_DI_Print (4, ("select_AntialiasFilt[%d]:: gain2 = %x\n",nChannelId, gain2_val)); // justin

	Drv_Ch_I2C_DemodWrite_CAB(nChannelId,  TDA10023_DEMOD_ADDRESS, SB_GAIN2, &gain2_val, 1);

	gain_val = 0x95; // kansai=0x94 0x95; INIT_GAIN;  // justin

	_GainReg = gain_val;
	_NYQgain = (gain_val>>4) & 0x07;      /* Nyquist filter gain factor: 0 - 7 */  // justin 0xa6

//	OSA_Print("### SB_GAIN = %x\n", gain_val); // justin_2
	CH_DI_Print (4, ("select_AntialiasFilt[%d]:: NYQgain = %d, 0xE = %x\n",nChannelId, _NYQgain, gain_val));

	Drv_Ch_I2C_DemodWrite_CAB(nChannelId, TDA10023_DEMOD_ADDRESS,  SB_GAIN, &gain_val, 1);
}

/**/
static void SetQAMdemod(int nChannelId)
{
	HUINT8	QAMreg[0x10];
	HUINT8	val, uc_val;
	HUINT8	nDECfilt;

	/* intp: set TS stream clock */
   	set_TS_clock_rate(nChannelId);

	val = 0x21; // 0x0; // phillips 0x7f;  // DVB Mode select
	Drv_Ch_I2C_DemodWrite_CAB(nChannelId, TDA10023_DEMOD_ADDRESS,  SB_RESET, &val, 1);

    switch (gTDA10023ChInfo[nChannelId].qamMode)
    {
        case 1 : //16-QAM
        	QAMreg[0x01] = 0x50; //AGCREF is same at both SW versions
        	QAMreg[0x05] = 0x6e; //LOCKTHR = 0x87 @ v1.01
        	QAMreg[0x08] = 0x6e; //MSETH = 0xa2 @ v1.01
        	QAMreg[0x09] = 0x91; //AREF is same at both SW versions

       		break;
        case 2 : //32-QAM
        	QAMreg[0x01] = 0x50; //0x8c
        	QAMreg[0x05] = 0x4b; //0x64 @ v1.01
        	QAMreg[0x08] = 0x4b; //0x74 @ v1.01
        	QAMreg[0x09] = 0x96;

        	break;
        case 3 : //64-QAM
       	QAMreg[0x01] = qam_val; // 0x50;  // TDA10023 datasheet value 0x50 justin
   		QAMreg[0x05] = 0x46; // TDA10023 datasheet value 0x46 justin
       	QAMreg[0x08] = 0x43; // 0x43; // TDA10023 datasheet value 0x46 justin
        	QAMreg[0x09] = 0x6a; // 0x6a; // TDA10023 datasheet value 0x46 justin
 		CH_DI_Print (4, ("SetQAMdemod[%d]:: 64-QAM initial setup starts\n",nChannelId));
        	break;
        case 4 : //128-QAM
           	QAMreg[0x01] = 0x78; //although perf. is best at -65 dBm or lower, it is a little degraded at mnorma level (higher than -65 dBm)
       	QAMreg[0x05] = 0x30; // @ v1.01
        	QAMreg[0x08] = 0x30; // @ v1.01
        	QAMreg[0x09] = 0x7e;

        	break;
        case 5 : //256-QAM
        	//QAMreg[0x01] = 0x50; //Philips source code
        	QAMreg[0x01] = 0x5c; //better than 0x50, from datasheet
        	QAMreg[0x05] = 0x23; //0x26 @ v1.01
		QAMreg[0x08] = 0x23; //0x23 @ v1.01
        	QAMreg[0x09] = 0x6b;
		CH_DI_Print (4, ("SetQAMdemod[%d]:: 256-QAM initial setup starts\n",nChannelId));

        	break;

	default: //64-QAM
       	QAMreg[0x01] = qam_val; // 0x50; // TDA10023 datasheet value 0x50 justin
   		QAMreg[0x05] = 0x46; // TDA10023 datasheet value 0x46 justin
       	QAMreg[0x08] = 0x43; // TDA10023 datasheet value 0x46 justin
        	QAMreg[0x09] = 0x6a; // TDA10023 datasheet value 0x46 justin
 		CH_DI_Print (4, ("SetQAMdemod[%d]:: 64-QAM initial setup starts. default\n",nChannelId));
			break;
	}

	/* agcconf: use the small AGC time constant at the initial phase and */
	/* then enlarge after Euqualizer turn-on  */
	QAMreg[0x02] =   0x93; // phillips R_AGCONF_FAST; //0x01

	/* clkconf: CLK_C coef is only programmed. Other params will be set later */
	_ClkconfReg  =  0x1a; // kansai=0x0a; // kansai R_CLKCONF_WIDE = 0x1a;
	QAMreg[0x03] =  0x1a; // kansai= 0x0a; // 0x0a;  //R_CLKCONF_WIDE = 0x1a;

	/* carconf: Set the carrier acquisition range wide regardless of the symbol rate  */
	/* If the SR is greater than 3 MHz, the range will be reduced after locking       */
	if(gTDA10023ChInfo[nChannelId].symbolRate >= 3000)
		_CarconfReg = cr_val;  // R_CARCONF_HIGH_SR_ACQ;//0x0a
	else if(gTDA10023ChInfo[nChannelId].symbolRate >= 1500)
		_CarconfReg = R_CARCONF_MID_SR_ACQ;//0x0c
	else
		_CarconfReg = R_CARCONF_LOW_SR_ACQ;//0x0d
	QAMreg[0x04] = _CarconfReg;

	/* eqconf: TDA10021 EQ is enabled contrast to VES1820x */
	QAMreg[0x06] = R_EQCONF_ON; //0x77: Center Tap = 10th tap, ENADAPT=ENEQUAL=DFE=1
	//QAMreg[0x06] = 0x47; //Center Tap = 6th tap, ENADAPT=ENEQUAL=DFE=1

	/* eqstep */
	QAMreg[0x07] = eq_val; // 0x19; //  R_EQSTEP; //0001 1010 => better than 0x1a @ long ghost (delay >= 335 ns)
	//QAMreg[0x07] = 0x1c; //0001 1100 => worse than 0x1a @ short ghost (delay <= 35 ns)
	//QAMreg[0x07] = 0x1b; //0001 1011 => worse than 0x1a @ short ghost (delay <= 35 ns)
	//QAMreg[0x07] = 0x19; //0001 1001 => better than 0x1a @ short ghost (delay <= 145 ns)
	// Conclution: Acquisition step size = xxxx x010

	//QAMreg[0x07] = 0x12; // 0001 0010 => similar to 0x1a @ short ghost (delay 1,000 ns)
	// Conclution: Tracking step size = xx01 1xxx

	/* registers programming from reg[1] to reg[9] */
	Drv_Ch_I2C_DemodWrite_CAB(nChannelId,  TDA10023_DEMOD_ADDRESS, SB_AGCREF, &QAMreg[1], 9);

	/* eqconf2: At first CTADAPT bit must to set to one
	   in order to achive the fine AGC operation       */
	uc_val = R_EQCONF2ND | 0x08; //no tap leakage & CTADAPT=1
//	uc_val = R_EQCONF2ND;
	Drv_Ch_I2C_DemodWrite_CAB(nChannelId,  TDA10023_DEMOD_ADDRESS, SB_EQCONF2, &uc_val, 1);

	/* clkconf's NDEC[1..0] bits: set the number of the decimation */
	nDECfilt = set_DecimateFilt(nChannelId);   // 실제로  계산 하면 항상 0 justin

	/* bdr_msb ~ bdr_inv: set the symbol rate and its inverse */
	set_baud_rate(nChannelId, nDECfilt); 		/* the symbol rate and its inverse */

	/* gain: set the gain of Nyquist filter and the anti-alias filter */
	select_AntialiasFilt(nChannelId); 		/* G_NYQ=2.0, G_AAF=1.0 */

}

/* calculate the sleep time in msec according to the symbol rate */
HUINT16 no_to_msec(int nChannelId, HUINT32 symbols)
{
	HUINT32	time_in_msec;
	HUINT16	t;

	/* Convert "number of symbols" to time in msec */
	time_in_msec  = symbols;
	time_in_msec += gTDA10023ChInfo[nChannelId].symbolRate/2;
	time_in_msec /= gTDA10023ChInfo[nChannelId].symbolRate; 	// msec

	t = (HUINT16) time_in_msec;

	if(gTDA10023ChInfo[nChannelId].symbolRate < 3000) t = 2 * t;

	//CH_DI_Print("select_AntialiasFilt:: no_to_msec = %d\n", t);

	return	t;
}

#if 0
/*******************************************************************
 If two DVB sync patterns(0x47 & 0xB8) are found, FEC is locked, but
 if only MPEG2 sync pattern(0x47) is found, returns FALSE (NODVB).
********************************************************************/
HUINT8 check_fec_lock_TDA(int nChannelId)
{
	HUINT8	val1,val2;

	/* Get the information of MPEG frame sync from the FEC core */
	Drv_Ch_I2C_DemodRead_CAB(nChannelId,  TDA10023_DEMOD_ADDRESS, SB_SYNC, &val1, 1);
	CH_DI_Print (2, ("check_fec_lock_TDA:: SB_SYNC = 0x%x\n", val1&0xff));
	val2 = val1 & 0xff; /* mask all bits but NODVB & FSYNC bit */

	/* check if both MPEG frame and carrier were synchronized */
	if( (val2&0x0f) ==0x0f )
		return TRUE;
	else if( ((val1&0x40)==0x40) )
	{
		CH_DI_Print (2, ("check_fec_lock_TDA:: No DVB packet\n"));
		return FALSE;
	}
	else
		return FALSE;
}
#endif

/* check if two AGC output are maximum. If then,  */
/* it means there is no signal or too weak signal */
HUINT8 check_agc_lock(int nChannelId)
{
	HUINT8	val1, val2;

	/* Read the value of AGC loops */
	Drv_Ch_I2C_DemodRead_CAB(nChannelId, TDA10023_DEMOD_ADDRESS,  SB_VAGC, &val1, 1);
	Drv_Ch_I2C_DemodRead_CAB(nChannelId, TDA10023_DEMOD_ADDRESS, SB_VAGC2, &val2, 1);
	CH_DI_Print (2, ("check_agc_lock[%d]:: RF_AGC = 0x%x (%d), IF_AGC= 0x%x (%d)\n", nChannelId, val1,val1,val2, val2));

	if ( ((val1==0xff) && (val2==0xff)) )
		return FALSE;
	else
		return TRUE;
}

#if 0
/* calculate the carrier offset. unit is kHz */
short get_carrier_offset(int nChannelId)
{
	HUINT8	val;
	long	car_off_hz;
	short	carrier_offset;

	/* VAFC is in 2's complement */
	Drv_Ch_I2C_DemodRead_CAB(nChannelId, TDA10023_DEMOD_ADDRESS, SB_VAFC, &val, 1);
	car_off_hz = (long) val;

	/* check the sign bit */
	if(val & 0x80)
		car_off_hz |=0xffffff00; //sign extension

	/* calculate the value in kHz */
	car_off_hz *= (long) ((1000*gTDA10023ChInfo[nChannelId].symbolRate+256) / 512);	// TDA10021, unit=Hz
	carrier_offset = (short) (car_off_hz/1000); // unit: kHz

	return carrier_offset;
}

short get_carrier_offset_new(int nChannelId)
{
	char	val;
	long	lAFC;
	short	sAFC;

	/* Read frequency correction */
	Drv_Ch_I2C_DemodRead_CAB(nChannelId,  TDA10023_DEMOD_ADDRESS, SB_VAFC, (HUINT8 *) &val, 1);

	/* signed value if needed -> 8 bits */
	lAFC = (long)val;
	if (val & 0x80)
		lAFC |= 0xFFFFFF00;

	/* calculate the value in kHz */
	lAFC *= (long) ( (gTDA10023ChInfo[nChannelId].symbolRate+256) / 512); // TDA10021

	/* invert the AFC */
    lAFC = -lAFC;

    sAFC = lAFC;

	/* return AFC */
	return sAFC;
}

/* Informs symbol clock offset in ppm */
short get_clock_offset(int nChannelId)
{
	HUINT8	mode;
	char	val;
	long	clk_off_int;
	short	clk_offset;

	mode = _ClkconfReg & 0x08;
	Drv_Ch_I2C_DemodRead_CAB(nChannelId, TDA10023_DEMOD_ADDRESS, SB_CKOFFSET, (HUINT8 *)&val, 1);

	/* check the sign bit */
	clk_off_int = (long) val;
	if(val & 0x80)
		clk_off_int |=0xffffff00; //sign extension

	clk_off_int *= 1000000;
	clk_off_int /= 262144; // 262144 = 2**18

	if( mode == 0x00) // DYN == 0
		clk_off_int /= 2;

	clk_offset = (short) (clk_off_int); // unit is ppm

	return clk_offset;
}
#endif

/***************************************************************************
  If FEC is locked due to spectral inversion, return 1, else return 0.
****************************************************************************/
#if 0
HUINT8 HandleSpectralInversion_TDA(int nChannelId)
{
	HUINT8	val;


	/* spectrall inversion 을 위해 추가 했음   */
	Drv_Ch_I2C_DemodRead_CAB(nChannelId,  TDA10023_DEMOD_ADDRESS, SB_CARCONF, &val, 1);

	_CarconfReg = val;

   	 val = val ^ 0x20;
	Drv_Ch_I2C_DemodWrite_CAB(nChannelId, TDA10023_DEMOD_ADDRESS,  SB_CARCONF, &val, 1);

	/* Wait for 80K symbol duration till FEC core locks to the frame sync */
	VK_TASK_Sleep( (HUINT16)(no_to_msec(nChannelId, WAIT_FRMLOCK) ));

	if( check_fec_lock_TDA(nChannelId) )
	{
	   	_CarconfReg = val; // justin  _ConfReg = val;
	   	gTDA10023ChInfo[nChannelId].SpectralInverse = 1;
		return TRUE;
	}
	else
	{
	   	gTDA10023ChInfo[nChannelId].SpectralInverse = 0;

		/* restore the original value */
	   	val  = _CarconfReg; // justin _ConfReg;
		Drv_Ch_I2C_DemodWrite_CAB(nChannelId,  TDA10023_DEMOD_ADDRESS, SB_CARCONF, &val, 1);
		return FALSE;
	}
}
#endif

/****************************************************************
Set TS output clock rate at the output mode A or B. The mode B
clock has no jitter and a duty cycle of 50%. But output TS clock
(OCLK) rate must be greater than M*Fsymbol/8.
*****************************************************************/
void set_TS_clock_rate(int nChannelId)
{
	HUINT8 val;

 	/***** Serial Mode *******/

	/* 0x12: FEL/UNCP	POINTP	MFS/PSYNCP	PUNCORP 	PDENP	POCLKP */
	/*			1		0		1		0		0		0		0		1 */
	/* 0x20 : DIVP[7-4] 					SWAPP	MSBFIRSTP	INTPSEL[1-0] */
	/*			0		0		0		1		0		1		1		1 */
	/* 0x2C : SWAPS MSBFIRSTS EXTPSYP EXTPSYS POINT47P POINT47S TRIP	TRIS */
	/*			0(df)	1(df)	1		0(df)	0		0(df)	0		1(df) */

	val = 0xa1;
	Drv_Ch_I2C_DemodWrite_CAB(nChannelId, TDA10023_DEMOD_ADDRESS, SB_INTP1, &val, 1);

	val = 0x17;
	Drv_Ch_I2C_DemodWrite_CAB(nChannelId, TDA10023_DEMOD_ADDRESS, SB_INTP2, &val, 1);

	val = 0x61;
	Drv_Ch_I2C_DemodWrite_CAB(nChannelId, TDA10023_DEMOD_ADDRESS, SB_CONTROL, &val, 1);

#if 0
	/* Parallel mode A: TS parallel with jittered clock */
	/* Parallel B mode 선택 */
		val = 0xa1;
	//	val = 0xe1;
		Drv_Ch_I2C_DemodWrite_CAB(nChannelId, TDA10023_DEMOD_ADDRESS, SB_INTP1, &val, 1);

	//	val = 0x05; // no-jittered clock, OCLK = SYSCLK / 1 = 70MHz, it does not run
	//	val = 0x15; // no-jittered clock, OCLK = SYSCLK / 2 = 35MHz, it does not run
	//	val = 0x25; // no-jittered clock, OCLK = SYSCLK / 3 = 23.33MHz, it run
	//	val = 0x35; // no-jittered clock, OCLK = SYSCLK / 4 = 17.5MHz
	//	val = 0x45; // no-jittered clock, OCLK = SYSCLK / 5 = 14MHz
	//	val = 0x55; // no-jittered clock, OCLK = SYSCLK / 6 = 11.66MHz
		val = 0x95; // no-jittered clock, OCLK = SYSCLK / 10 = 7MHz
	//	val = 0x75; // no-jittered clock, OCLK = SYSCLK / 8 = 8.75MHz
	//	val = 0x85; // no-jittered clock, OCLK = SYSCLK / 9 = 7.77MHz
	// 	val = 0x95; // no-jittered clock, OCLK = SYSCLK / 10 = 7MHz
	//	val = 0xa5; // no-jittered clock, OCLK = SYSCLK / 11 = 6.36MHz
	//	val = 0xb5; // no-jittered clock, OCLK = SYSCLK / 12 = 5.83MHz
	//	val = 0xc5; // no-jittered clock, OCLK = SYSCLK / 13 = 5.38MHz
	//	val = 0xd5; // no-jittered clock, OCLK = SYSCLK / 14 = 5MHz
	//	val = 0xe5; // no-jittered clock, OCLK = SYSCLK / 15 = 4.66MHz
	//	val = 0xf5; // no-jittered clock, OCLK = SYSCLK / 16 = 4.375MHz
		Drv_Ch_I2C_DemodWrite_CAB(nChannelId,  TDA10023_DEMOD_ADDRESS, SB_INTP2, &val, 1);
#endif

	/* Parallel mode A or B according to the average byte clock rate.
	   TS parallel with the regular clock in case of Parallel mode B.
	   If the average clock rate is low, select the parallel mode A. */

}

/********************************************************************
for signal strength display
*********************************************************************/
HUINT16 SNR_Info_TDA(int nChannelId)
{
	HUINT8	val1,val2;
	HUINT32	val, agcv;
	float fSNR, fMSE;
	HUINT32 uCounter;

	/* Read the number of saturations at ADC output */
	Drv_Ch_I2C_DemodRead_CAB(nChannelId,  TDA10023_DEMOD_ADDRESS, SB_VAGC, &val1, 1);	// read RF AGC
	Drv_Ch_I2C_DemodRead_CAB(nChannelId,  TDA10023_DEMOD_ADDRESS, SB_VAGC2, &val2, 1);	// read IF AGC

	/* Meaure RF AGC register value range over RF level of -25 ~ +15 dBmV: 156 ~ 255 */
	/* Meaure IF AGC register value range over RF level of -25 ~ +15 dBmV: 88 ~ 135 */
	/* Use RF AGC in case of high to medium signal level */

#if 1
	val = val1 + val2;
	if (val < 195) val = 195; // upper value limit
	val -=195;

	if (val > 195 ) val = 195; // lower value limit

	/* 255 max. value for OAK & OPENTV application */
	agcv = 255 - 255*val/195;

	/* 100 max. value for application */
	//agcv = 100 - 100*val/195;
#else
	agcv = (val1+val2)/2;
	agcv = 255-agcv;
#endif

	val = (HUINT8) agcv;

#if 0
	Printf("val1 : %d, val2 : %d, val : %d\n", val1, val2, val);
#endif

	fMSE = (float)0;
	for (uCounter = 0; uCounter < 10; uCounter++)
	{
		// average value of the MSE on 10 values
		Drv_Ch_I2C_DemodRead_CAB(nChannelId,  TDA10023_DEMOD_ADDRESS, SB_MSE, &val1, 1);
		//	CH_DI_Print ("SNR_Info_TDA:: MSE = %x\n", val); // justin
		fMSE += (float)val1;
	}

	fMSE /= (float)uCounter;
	fSNR = (float)((210.0/(0.40*fMSE + 5.0)) + 12.5);
	val = (HUINT16) (fSNR*10.0);

	CH_DI_Print (3, ("SNR_Info_TDA[%d]:: SNR = %d\n",nChannelId,val)); // justin

	return val;
}

/* Calcaulte the estimated SNR value */
HINT16 Find_RFIndex(HUINT8 agcval, HUINT8 *AgcTable, HINT16 *offset);
HINT16 Find_IFIndex(HUINT8 agcval, HUINT8 *AgcTable, HINT16 *offset);
HINT16 Find_SignalLevel(HINT16 rfPwrIndex, HINT16 ifPwrIndex,  HINT16 rfoffset, HINT16 ifoffset);


HINT16 Find_RFIndex(HUINT8 agcval, HUINT8 *AgcTable, HINT16 *offset)
{

	HINT16 i, DsPwrIndex=0;


	for (i=0; i<level_num; i++)
	{
		if ( (agcval <= AgcTable[i]) )
		{
			DsPwrIndex = i;
			if (agcval == AgcTable[i])
			{
				*offset = 0;
			}
			else
			{
				if( i == 0)
                            {
                                	*offset = 0;
                            }
                            else
                            {
					*offset = 5;
					 // *offset = ((agcval - AgcTable[i]) *10) / (AgcTable[i-1] - AgcTable[i]);
                            }

			}
			break;
		}
	}

	if(i == level_num)
	{
		DsPwrIndex = level_num;
		*offset = 0;
	}

	CH_DI_Print(5, ( "Find_RFIndex: Table[%d] = %d, Agc = %d, DsPwrIndex = %d\n", i, AgcTable[i], agcval, DsPwrIndex));


	return DsPwrIndex;
}


HINT16 Find_IFIndex(HUINT8 agcval, HUINT8 *AgcTable, HINT16 *offset)
{

	HINT16 i, DsPwrIndex=0;


	for (i=0; i<level_num; i++)
	{

		if ( (agcval <= AgcTable[i]) )
		{
			DsPwrIndex = i;
			if (agcval == AgcTable[i])
			{
				*offset = 0;
			}
			else
			{
				if( i == 0)
                            {
                                	*offset = 0;
                            }
                            else
                            {
					*offset = 5;
					 // *offset = ((agcval - AgcTable[i]) *10) / (AgcTable[i-1] - AgcTable[i]);

                            }

			}
			break;
		}
	}

	if(i == level_num)
	{
		DsPwrIndex = level_num;
		*offset = 0;
	}

	CH_DI_Print(5, ( "Find_IFIndex: Table[%d] = %d, Agc = %d, DsPwrIndex = %d\n", i, AgcTable[i], agcval, DsPwrIndex));

	return DsPwrIndex;
}


HINT16 Find_SignalLevel(HINT16 rfPwrIndex, HINT16 ifPwrIndex,  HINT16 rfoffset, HINT16 ifoffset)
{

	HINT16	strength;
	HINT16  level, offset;

	strength = 0;

	if( ifPwrIndex <= rfPwrIndex )
	{
		level = rfPwrIndex;
		offset = rfoffset;
	}
	else
	{
		level = ifPwrIndex;
		offset = ifoffset;
	}

	if(level == 0)
	{
		strength = 250;
	}
	else if(level == level_num)
	{
		strength = -160;
	}
	else
	{
		strength = (downlevel[level]*10)+offset-600;      /* dBuV -> dBmV    */
	}


	return strength;



}



HINT16 Strength_Info_TDA(int nChannelId)

{

	HUINT8	rfagc, ifagc;
	HINT16  strength=0;
	HINT16 	rfoffset=0, ifoffset=0;
	HINT16	levelRfIndex, levelIfIndex;



	/* Read the number of saturations at ADC output */
	Drv_Ch_I2C_DemodRead_CAB(nChannelId, TDA10023_DEMOD_ADDRESS, SB_VAGC, &rfagc, 1);	// read RF AGC
	Drv_Ch_I2C_DemodRead_CAB(nChannelId, TDA10023_DEMOD_ADDRESS, SB_VAGC2, &ifagc, 1);	// read IF AGC
	/* Meaure RF AGC register value range over RF level of -25 ~ +15 dBmV: 156 ~ 255 */
	/* Meaure IF AGC register value range over RF level of -25 ~ +15 dBmV: 88 ~ 135 */
	/* Use RF AGC in case of high to medium signal level */
	CH_DI_Print(5, ("Strength_Info_TDA[%d]: frq = %d, rfagc = %d, ifagc = %d\n",nChannelId,gTDA10023ChInfo[nChannelId].frequency, rfagc, ifagc));

	if (gTDA10023ChInfo[nChannelId].frequency < 170000)
	{
		levelRfIndex = Find_RFIndex( rfagc, RfAgcGain1, &rfoffset);
		levelIfIndex = Find_IFIndex( ifagc, IfAgcGain1, &ifoffset);
		strength = Find_SignalLevel(levelRfIndex, levelIfIndex, rfoffset, ifoffset);

	}
	else if(gTDA10023ChInfo[nChannelId].frequency < 182000)
	{
		levelRfIndex = Find_RFIndex( rfagc, RfAgcGain2, &rfoffset);
		levelIfIndex = Find_IFIndex( ifagc, IfAgcGain2, &ifoffset);
		strength = Find_SignalLevel(levelRfIndex, levelIfIndex, rfoffset, ifoffset);

	}
	else if (gTDA10023ChInfo[nChannelId].frequency < 302000)
	{
		levelRfIndex = Find_RFIndex( rfagc, RfAgcGain3, &rfoffset);
		levelIfIndex = Find_IFIndex( ifagc, IfAgcGain3, &ifoffset);
		strength = Find_SignalLevel(levelRfIndex, levelIfIndex, rfoffset, ifoffset);

	}
	else if (gTDA10023ChInfo[nChannelId].frequency < 472000)
	{
		levelRfIndex = Find_RFIndex( rfagc, RfAgcGain4, &rfoffset);
		levelIfIndex = Find_IFIndex( ifagc, IfAgcGain4, &ifoffset);
		strength = Find_SignalLevel(levelRfIndex, levelIfIndex, rfoffset, ifoffset);

	}
	else if (gTDA10023ChInfo[nChannelId].frequency < 628000)
	{

		levelRfIndex = Find_RFIndex( rfagc, RfAgcGain5, &rfoffset);
		levelIfIndex = Find_IFIndex( ifagc, IfAgcGain5, &ifoffset);
		strength = Find_SignalLevel(levelRfIndex, levelIfIndex, rfoffset, ifoffset);


	}
	else if (gTDA10023ChInfo[nChannelId].frequency < 718000)
	{
		if ((gTDA10023ChInfo[nChannelId].frequency>= 671000) && (gTDA10023ChInfo[nChannelId].frequency <= 707000) )
		{
			levelRfIndex = Find_RFIndex( rfagc, RfAgcGain8, &rfoffset);
			levelIfIndex = Find_IFIndex( ifagc, IfAgcGain8, &ifoffset);
			strength = Find_SignalLevel(levelRfIndex, levelIfIndex, rfoffset, ifoffset);

		}
		else
		{
			levelRfIndex = Find_RFIndex( rfagc, RfAgcGain6, &rfoffset);
			levelIfIndex = Find_IFIndex( ifagc, IfAgcGain6, &ifoffset);
			strength = Find_SignalLevel(levelRfIndex, levelIfIndex, rfoffset, ifoffset);
		}

	}
	else
	{
		levelRfIndex = Find_RFIndex( rfagc, RfAgcGain7, &rfoffset);
		levelIfIndex = Find_IFIndex( ifagc, IfAgcGain7, &ifoffset);
		strength = Find_SignalLevel(levelRfIndex, levelIfIndex, rfoffset, ifoffset);

	}

	CH_DI_Print(5,("Strength_Info_TDA[%d]: strength = %d\n",nChannelId, strength));
	return strength;
}



/**************************************************************************
- BER_Info_TDA(): Convert the real BER to IRD's signal quality factor.
- BER: Estimated BER at the Reed-Solomon recoder input
- 2.0E-4 before Reed-Solomon encoding is similar to 1.0e-11
  after Reed-Solomon decoding
 	O: FEC unlock
	1: 1.0e-2 <= BER @ pre Reed-Solomon decoder < 5.0e-2
	2: 7.5e-3 <= BER @ pre Reed-Solomon decoder < 1.0e-2
	3: 5.0e-3 <= BER @ pre Reed-Solomon decoder < 7.5e-3
	4: 2.5e-3 <= BER @ pre Reed-Solomon decoder < 5.0e-3
	5: 1.0e-3 <= BER @ pre Reed-Solomon decoder < 2.5e-3
	6: 7.5e-4 <= BER @ pre Reed-Solomon decoder < 1.0e-4
	7: 5.0e-4 <= BER @ pre Reed-Solomon decoder < 7.5e-4
	8: 2.5e-4 <= BER @ pre Reed-Solomon decoder < 5.0e-4
	9: 1.0e-4 <= BER @ pre Reed-Solomon decoder < 2.5e-4
   10:           BER @ pre Reed-Solomon decoder < 1.0e-4
***************************************************************************/
HUINT32 BER_Info_TDA(int nChannelId)
{
	HUINT8	Ecnt[3], BerDepth;
	HUINT32	error_count;

	char buffer[14];

	/* Get the number of bit errors */
	Drv_Ch_I2C_DemodRead_CAB(nChannelId, TDA10023_DEMOD_ADDRESS, SB_BER_LSB, &Ecnt[0], 3);

	error_count = ((((unsigned long)Ecnt[2])<<16 )| (((unsigned long)Ecnt[1])<<8 )| (((unsigned long)Ecnt[0])));

	Drv_Ch_I2C_DemodRead_CAB(nChannelId, TDA10023_DEMOD_ADDRESS, SB_RSCONF, &BerDepth, 1);

	CH_DI_Print (5, ("BER_Info_TDA[%d]:: BerDepth = %x, erro_count = %d\n",nChannelId,BerDepth, error_count));

    switch(BerDepth & 0xc0)
    {
		case 0x00:  // 1,00E+05
			error_count *= 1000;
			break;
		case 0x40:  // 1,00E+06
			error_count *= 100;
			break;
		case 0x80:  // 1,00E+07
			error_count *= 10;
			break;
		case 0xc0:  // 1,00E+08
			break;
    }

	VK_snprintf(buffer, sizeof(buffer), "%d", error_count);

	CH_DI_Print (4, ("BER[%d]  : %c.%c%c E-%02d\n",nChannelId, buffer[0], buffer[1], buffer[2], 8-VK_strlen(buffer)+1));
	CH_DI_Print (3, ("BER_Info_TDA[%d]::  Error_count = %d, accum = %d \n",nChannelId, error_count,accum_error));

/* 원래는 2^20 7자리가 최대 표시. */

	return(error_count);
}


#if 0
/**************************************************************************
- BER_Info_PostRSD_TDA(): Convert the real BER to IRD's signal quality factor.
- BER factor based on Reed-Solomon decoder output
 	O: FEC unlock
	1: 1.0e-3 <= BER @ post Reed-Solomon decoder
	2: 1.0e-4 <= BER @ post Reed-Solomon decoder  < 1.0e-3
	3: 1.0e-5 <= BER @ post Reed-Solomon decoder  < 1.0e-4
	4: 1.0e-6 <= BER @ post Reed-Solomon decoder  < 1.0e-5
	5: 1.0e-7 <= BER @ post Reed-Solomon decoder  < 1.0e-6
	6: 1.0e-8 <= BER @ post Reed-Solomon decoder  < 1.0e-7
	7: 1.0e-9 <= BER @ post Reed-Solomon decoder  < 1.0e-8
	8: 1.0e-10<= BER @ post Reed-Solomon decoder  < 1.0e-9
	9: 1.0e-11<= BER @ post Reed-Solomon decoder  < 1.0e-10
   10:           BER @ post Reed-Solomon decoder  < 1.0e-11
***************************************************************************/
HUINT8 BER_Info_PostRSD(int nChannelId)
{
	HUINT8	val;
	HUINT32	error_bits, post_RSD_ber_1e11;

	post_RSD_ber_1e11 = 0;

	/* Get the BER scale factor */
	Drv_Ch_I2C_DemodRead_CAB(nChannelId, TDA10023_DEMOD_ADDRESS, SB_RSCONF, &val, 1);
	val = val & 0xc0;

	/* Numer of the error bits */
	error_bits = measure_BER_TDA(nChannelId);

	switch (val)
	{
		case BER_DEPTH5: //ber_cnt_width=1.25e5
			post_RSD_ber_1e11 = error_bits*1000000;
			post_RSD_ber_1e11 *=4;
			post_RSD_ber_1e11 /=5;
			break;
		case BER_DEPTH6: //ber_cnt_width=1.0e6
			post_RSD_ber_1e11 = error_bits*100000;
			break;
		case BER_DEPTH7: //ber_cnt_width=1.0e7
			post_RSD_ber_1e11 = error_bits*10000;
			break;
		case BER_DEPTH8: //ber_cnt_width=1.0e8
			post_RSD_ber_1e11 = error_bits*1000;
			break;
		default:
			break;
	}

	if (post_RSD_ber_1e11 >= 100000000)	val = 1;
	else if (post_RSD_ber_1e11 >= 10000000)		val = 2;
	else if (post_RSD_ber_1e11 >= 1000000)		val = 3;
	else if (post_RSD_ber_1e11 >= 100000)		val = 4;
	else if (post_RSD_ber_1e11 >= 10000)		val = 5;
	else if (post_RSD_ber_1e11 >= 1000)			val = 6;
	else if (post_RSD_ber_1e11 >= 100)			val = 7;
	else if (post_RSD_ber_1e11 >= 10)			val = 8;
	else if (post_RSD_ber_1e11 >= 1)			val = 9;
	else 										val = 10;

	return (val );
}
#endif

/* Increase the BER counter width to 1.0e+8 */
void AdaptBerWindow(int nChannelId)
{
	HUINT8 	val;
	HUINT8	bitsPerBaud;
	HUINT32	ElapsedTime;
	HUINT32	ElapsedBits;

	bitsPerBaud = 6;

    /* Define bit per symbol according to QAM mode */
    switch (gTDA10023ChInfo[nChannelId].qamMode) {
    	case 1 : /* 16-QAM */
    		bitsPerBaud = 4;   break;
    	case 2 : /* 32-QAM */
    		bitsPerBaud = 5;   break;
    	case 3 : /* 64-QAM */
    		bitsPerBaud = 6;   break;
    	case 4 : /* 128-QAM */
    		bitsPerBaud = 7;   break;
    	case 5 : /* 256-QAM */
    		bitsPerBaud = 8;   break;
    }

	/* BER measurement period is equal to GET_DATA_INTERVAL */
	ElapsedTime = 2*GET_DATA_INTERVAL; // in msec

	/* calculate the number of the elapsed bits for GET_DATA_INTERVAL */
	ElapsedBits = bitsPerBaud*ElapsedTime*gTDA10023ChInfo[nChannelId].symbolRate;
//	OSA_Print("### ElapsedBits = %d\n", ElapsedBits);
	/* select the BER counter width equal or more than the elapsed bits */
	if     (ElapsedBits > 10000000)
		val = R_RSCONF4;
	else if(ElapsedBits > 1000000)
		val = R_RSCONF3;
	else if(ElapsedBits > 125000)
		val = R_RSCONF2;
	else
		val = R_RSCONF1;


	Drv_Ch_I2C_DemodWrite_CAB(nChannelId, TDA10023_DEMOD_ADDRESS,  SB_RSCONF, &val, 1);


}


/* additional QAM channel information */
#if 0 // justin #ifndef _FGDL_
void get_loop_inform_TDA(void)
{
	short caroff,clkoff;
	HUINT8 val1, val2;

	caroff = get_carrier_offset(); // kHz
	clkoff = get_clock_offset();   // ppm

   	gTDA10023ChInfo.CarrierOffset = caroff;
   	gTDA10023ChInfo.ClockOffset = clkoff;
}
#endif



void JQAM_Mode(int nChannelId, HUINT8 mode)
{
	HUINT8	val;

	CH_DI_Print (5, ("JQAM_Mode[%d]::  JQAM Mode = %x\n",nChannelId, mode));

	val = mode;

	Drv_Ch_I2C_DemodWrite_CAB(nChannelId, TDA10023_JQAM_ADDRESS, TDA10023_JQAM_MODE_IND, &val, 1);

}

void JQAM_TsId(int nChannelId, HINT32 tsID, HINT32 onID)
{
	HUINT8	val[2];

	CH_DI_Print (4, ("JQAM_TsId[%d]:: TS ID = %x, onid = %x\n",nChannelId, tsID, onID));
	val[0] = tsID >> 8;
	val[1] = tsID;

	Drv_Ch_I2C_DemodWrite_CAB(nChannelId, TDA10023_JQAM_ADDRESS,   TDA10023_REQ_TS_IDH_IND, &val[0], 2);

	val[0] = onID >> 8;
	val[1] = onID;

	Drv_Ch_I2C_DemodWrite_CAB(nChannelId, TDA10023_JQAM_ADDRESS,   TDA10023_REQ_ON_IDH_IND, &val[0], 2);

}

void JQAM_Status(int nChannelId)
{
	HUINT8 val;


	Drv_Ch_I2C_DemodRead_CAB(nChannelId, TDA10023_JQAM_ADDRESS,   TDA10023_REG5_IND, &val, 1);

	CH_DI_Print (4, ("JQAM_Status[%d]:: JQAM status = %x\n",  nChannelId, val));


	if ( tsmf_flag == 1)
	{
		if (test_gobad == 1)
			val = 0x2f;   // good
		else
			val = 0x1;  // bad
	}

	if((val & 0x40) == 0x40)
	{
		CH_DI_Print (4, ("JQAM_Status[%d]:: EMERGENCY!\n", nChannelId));
		gTDA10023ChInfo[nChannelId].emergencyFlag = 1;
	}
	else
	{
		gTDA10023ChInfo[nChannelId].emergencyFlag = 0;
	}

	if((val & 0x30) == 0x00)
	{
		CH_DI_Print (4, ("JQAM_Status[%d]:: Signal Good. [0x%02x]\n", nChannelId, (val & 0x30) >> 4));
		gTDA10023ChInfo[nChannelId].tsmfStatus = 0;
	}
	else if((val & 0x30) == 0x10)
	{
		CH_DI_Print (4, ("JQAM_Status[%d]:: Signal Poor. [0x%02x]\n", nChannelId, (val & 0x30) >> 4));
		gTDA10023ChInfo[nChannelId].tsmfStatus = 1;
	}
	else if((val & 0x30) == 0x20)
	{
		CH_DI_Print (4, ("JQAM_Status[%d]:: Signal Bad. [0x%02x]\n", nChannelId, (val & 0x30) >> 4));
		gTDA10023ChInfo[nChannelId].tsmfStatus = 2;
	}
	else if((val & 0x30) == 0x30)
	{
		CH_DI_Print (4, ("JQAM_Status[%d]:: Reserved\n", nChannelId));
		gTDA10023ChInfo[nChannelId].tsmfStatus =3;
	}

	CH_DI_Print (4, ("JQAM_Status[%d]:: VER = %x\n", nChannelId, (val & 0x0e) >> 1));

	if((val & 0x01) == 1)
	{
		CH_DI_Print (4, ("JQAM_Status[%d]:: MLOCK OK\n", nChannelId));
	}
	else
	{
		CH_DI_Print (4, ("JQAM_Status[%d]:: MLOCK Not OK \n", nChannelId));
		gTDA10023ChInfo[nChannelId].tsmfStatus = 0;
		gTDA10023ChInfo[nChannelId].emergencyFlag = 0;

	}

/* 이토츄 테스트시에 Bad signal 이지만 이 비드가 에러로 날라오는 경우 발생. 2005-03-31 */
	if((val & 0x80) == 0x80)
	{
		CH_DI_Print (4, ("JQAM_Status[%d]:: ERROR!\n", nChannelId));
		gTDA10023ChInfo[nChannelId].tsmfStatus = 0;
		gTDA10023ChInfo[nChannelId].emergencyFlag = 0;
	}

}




int JQAM_TsNumber(int nChannelId)
{
	HUINT8 val[2], ts_value, ts_value1;
	int number;
	int i,j;

	number = 0;
	j=0;

	Drv_Ch_I2C_DemodRead_CAB(nChannelId, TDA10023_JQAM_ADDRESS,  TDA10023_TS_STATUS_H_IND, &val[0], 2);
	ts_value = val[1];
	ts_value1 = val[0];
	CH_DI_Print (4, ("JQAM_TsNumber[%d]:: TS  = 0x%x\n",nChannelId, ts_value));

	for (i = 0; i< 8; i++)
	{

		CH_DI_Print(4, ("JQAM_TsNumber[%d] = %d, ts_value=0x%x\n",nChannelId, i,((ts_value >>(1*i) ) & 0xff)));
		if( ((ts_value >>(1*i) ) & 0x01) == 0x01)
		{
			number += 1;
#if 0
			JqamIicRx(TDA10023_TS_ID1H_IND + i*4, &val, 1);
			tsid = val;

			JqamIicRx(TDA10023_TS_ID1H_IND + i*4+1, &val, 1);
			tsid = (tsid << 8) |val ;
			CH_DI_Print ("JQAM_TsNumber:: tsid 2  = %x\n", tsid);
			gTDA10023TsmfInfo.tsID[j] = 	tsid;


			JqamIicRx(TDA10023_TS_ID1H_IND + i*4+2, &val, 1);
			netid = val;

			JqamIicRx(TDA10023_TS_ID1H_IND + i*4+3, &val, 1);
			netid = (netid << 8) |val ;

			CH_DI_Print ("JQAM_TsNumber:: netid  = 0x%x, i = %d, j = %d\n", netid, i, j);

			gTDA10023TsmfInfo.netID[j] = 	netid;

			j++;
#endif
		}

	}


	CH_DI_Print (4, ("JQAM_TsNumber[%d]:: TS[0..7] = %d\n",nChannelId, number));


//	JqamIicRx(TDA10023_TS_STATUS_H_IND, &val, 1);
	ts_value = ts_value1;

	for (i= 0; i< 7; i++)
	{

		if( ((ts_value >> (1*i) ) & 0x01) ==  0x01)
		{
			number += 1;
#if 0
			JqamIicRx((TDA10023_TS_ID1H_IND+32) + i*4, &val, 1);
			tsid = val;

			JqamIicRx((TDA10023_TS_ID1H_IND+32) + i*4+1, &val, 1);
			tsid = (tsid << 8) |val ;

			gTDA10023TsmfInfo.tsID[j] = 	tsid;


			JqamIicRx((TDA10023_TS_ID1H_IND+32) + i*4+2, &val, 1);
			netid = val;

			JqamIicRx((TDA10023_TS_ID1H_IND+32) + i*4+3, &val, 1);
			netid = (netid << 8) |val ;

			CH_DI_Print ("JQAM_TsNumber:: netid2  = 0x%x, i = %d, j = %d\n", netid, i, j);

			gTDA10023TsmfInfo.netID[j] = 	netid;

			j++;
#endif
		}

	}

	CH_DI_Print (4, ("JQAM_TsNumber[%d]:: TS[0..14] number = %d\n",nChannelId, number));

	gTDA10023TsmfInfo[nChannelId].tsNo = number;

	return number;

}


void JQAM_TSinfo(int nChannelId)
{
	HUINT8 val[4], val1[2], ts_value, ts_value1;
	int number;
	int i,j;
	HINT32 tsid, netid;

	number = 0;
	j=0;

	Drv_Ch_I2C_DemodRead_CAB(nChannelId, TDA10023_JQAM_ADDRESS,  TDA10023_TS_STATUS_H_IND, &val1[0], 2);
	ts_value = val1[1];
	ts_value1 = val1[0];

	CH_DI_Print (3, ("JQAM_TsNumber[%d]:: TS  = 0x%x\n",nChannelId, ts_value1));

	for (i = 0; i< 8; i++)
	{

		CH_DI_Print(4, ("JQAM_TSinfo[%d]:: num = %d, ts_value=0x%x\n",nChannelId, i,((ts_value >>(1*i) ) & 0xff)));
		if( ((ts_value >>(1*i) ) & 0x01) == 0x01)
		{
			number += 1;
#if 1
			Drv_Ch_I2C_DemodRead_CAB(nChannelId, TDA10023_JQAM_ADDRESS,  TDA10023_TS_ID1H_IND + i*4, &val[0], 4);
			tsid = (((int)val[0]) << 8) | ((int) val[1]);
			gTDA10023TsmfInfo[nChannelId].tsID[j] = 	tsid;

			netid = (((int)val[2]) << 8) | ((int) val[3]);
			gTDA10023TsmfInfo[nChannelId].netID[j] = netid;

			CH_DI_Print(1,("JQAM_TSinfo[%d]:: TsId = %d, NetId = %d \n", nChannelId, tsid, netid));
#else
			JqamIicRx(TDA10023_TS_ID1H_IND + i*4, &val, 1);
			tsid = val;

			JqamIicRx(TDA10023_TS_ID1H_IND + i*4+1, &val, 1);
			tsid = (tsid << 8) |val ;
			CH_DI_Print ("JQAM_TsNumber:: tsid 2  = %x\n", tsid);
			gTDA10023TsmfInfo.tsID[j] = 	tsid;


			JqamIicRx(TDA10023_TS_ID1H_IND + i*4+2, &val, 1);
			netid = val;

			JqamIicRx(TDA10023_TS_ID1H_IND + i*4+3, &val, 1);
			netid = (netid << 8) |val ;

			CH_DI_Print ("JQAM_TsNumber:: netid  = 0x%x, i = %d, j = %d\n", netid, i, j);

			gTDA10023TsmfInfo.netID[j] = 	netid;
#endif
			j++;

		}

	}


	CH_DI_Print (3, ("JQAM_TSinfo[%d]:: TS[0..7] = %d\n",nChannelId, number));


//	JqamIicRx(TDA10023_TS_STATUS_H_IND, &val1, 1);
	ts_value = ts_value1;

	for (i= 0; i< 7; i++)
	{
		if( ((ts_value >> (1*i) ) & 0x01) ==  0x01)
		{
			number += 1;
#if 1
			Drv_Ch_I2C_DemodRead_CAB(nChannelId, TDA10023_JQAM_ADDRESS,  (TDA10023_TS_ID1H_IND+32) + i*4, &val[0], 4);

			tsid = (((int)val[0]) << 8) | ((int) val[1]);
			gTDA10023TsmfInfo[nChannelId].tsID[j] = 	tsid;

			netid = (((int)val[2]) << 8) | ((int) val[3]);
			gTDA10023TsmfInfo[nChannelId].netID[j] = netid;

			CH_DI_Print(1,("JQAM_TSinfo[%d]:: TsId = %d, NetId = %d \n", nChannelId, tsid, netid));
#else
			JqamIicRx((TDA10023_TS_ID1H_IND+32) + i*4, &val, 1);
			tsid = val;

			JqamIicRx((TDA10023_TS_ID1H_IND+32) + i*4+1, &val, 1);
			tsid = (tsid << 8) |val ;

			gTDA10023TsmfInfo.tsID[j] = 	tsid;


			JqamIicRx((TDA10023_TS_ID1H_IND+32) + i*4+2, &val, 1);
			netid = val;

			JqamIicRx((TDA10023_TS_ID1H_IND+32) + i*4+3, &val, 1);
			netid = (netid << 8) |val ;

			CH_DI_Print ("JQAM_TsNumber:: netid2  = 0x%x, i = %d, j = %d\n", netid, i, j);

			gTDA10023TsmfInfo.netID[j] = 	netid;
#endif
			j++;
		}

	}

	CH_DI_Print (4, ("JQAM_TSinfo[%d]:: TS[0..14] = %d\n", nChannelId,number));

	gTDA10023TsmfInfo[nChannelId].tsNo = number;

}


HUINT32 Sig_Unerroed_PKT(int nChannelId)
{
	HUINT8	puBytes[4];
	HUINT32	ok_pkt;


	/* Get the number of unerroed packet  */
	Drv_Ch_I2C_DemodRead_CAB(nChannelId, TDA10023_DEMOD_ADDRESS, CPT_TSP_OK1, &puBytes[0], 4);

        ok_pkt = (puBytes[3]<<24) | (puBytes[2]<<16) | (puBytes[1]<<8) | puBytes[0];

	if(nChannelId == 0)
	{
        	CH_DI_Print (5, ("Sig_Unerroed_PKT[%d] :: OK Packet = %d\n", nChannelId, ok_pkt));
	}
	else
	{
		CH_DI_Print (5, ("Sig_Unerroed_PKT[%d] :: OK Packet = %d\n", nChannelId, ok_pkt));
	}

	return ok_pkt;
}


HUINT32 Sig_Corrected_PKT(int nChannelId)
{
	HUINT8	puBytes[4];
	HUINT32	cor_pkt;


	/* Get the number of corrected packet  */
	Drv_Ch_I2C_DemodRead_CAB(nChannelId,  TDA10023_DEMOD_ADDRESS, CPT_TSP_COR1, &puBytes[0], 4);

        cor_pkt = (puBytes[3]<<24) | (puBytes[2]<<16) | (puBytes[1]<<8) | puBytes[0];

	if(nChannelId == 0)
	{
       	 CH_DI_Print (5, ("Sig_Corrected_PKT[%d]:: Corrected Packet = %d\n", nChannelId, cor_pkt));
	}
	else
	{
		CH_DI_Print (5, ("Sig_Corrected_PKT[%d]:: Corrected Packet = %d\n", nChannelId, cor_pkt));
	}
	return cor_pkt;
}


HUINT32 Sig_UnCorrected_PKT(int nChannelId)
{
	HUINT8	puBytes[4];
	HUINT32	uncor_pkt;

	/* Get the number of uncorrected packet  */
	Drv_Ch_I2C_DemodRead_CAB(nChannelId,  TDA10023_DEMOD_ADDRESS, CPT_TSP_UNCOR1, &puBytes[0], 4);

	uncor_pkt = (puBytes[3]<<24) | (puBytes[2]<<16) | (puBytes[1]<<8) | puBytes[0];

	if(nChannelId == 0)
	{
		CH_DI_Print (3, ("Sig_UnCorrected_PKT[%d]:: Uncorrected Packet = %d\n", nChannelId, uncor_pkt));
	}
	else
	{
		CH_DI_Print (3, ("Sig_UnCorrected_PKT[%d]:: Uncorrected Packet = %d\n", nChannelId, uncor_pkt));
	}

	//if( (show_di_ch > 0 && uncor_pkt> 1) || uncor_pkt  > 30)
	if( uncor_pkt  > 1)
	{
		if(jcom_error != uncor_pkt)
		{
			if(nChannelId == 0)
			{
				CH_DI_Print(0, ("Sig_UnCorrected_PKT[%d]::  UnCorrected Packet   = %d\n", nChannelId, uncor_pkt));
			}
			else
			{
				CH_DI_Print(0, ("Sig_UnCorrected_PKT[%d]::  UnCorrected Packet   = %d\n", nChannelId, uncor_pkt));
			}

			jcom_error = uncor_pkt;
		}
	}

	return uncor_pkt;
}


void InitDemodulator(int nChannelId)
{
	HUINT8 val;

	CH_DI_Print (3, ("InitDemodulator[%d]:: Power-Down mode\n",nChannelId));
#if 1
	/* set STDBY bit of CONF to one */
	val = 0x80 | _ConfReg;
	Drv_Ch_I2C_DemodWrite_CAB(nChannelId,  TDA10023_DEMOD_ADDRESS, SB_CONF, &val, 1);

	/* ADC stand-by mode */
	val = 0xae; // justin 0x33;
	Drv_Ch_I2C_DemodWrite_CAB(nChannelId,  TDA10023_DEMOD_ADDRESS, SB_ADC, &val, 1);
#endif

//	Drv_Ch_I2C_DemodRead_CAB(nChannelId,  SB_RESET, &val, 1);
//	val = 0x80 | val;

//	Drv_Ch_I2C_DemodWrite_CAB(nChannelId,  SB_RESET, &val, 1);

	switch (gTDA10023ChInfo[nChannelId].qamMode)
	{
	    case 1 : //16-QAM
	    	_ConfReg    = 0x63; break;
	    case 2 : //32-QAM
	    	_ConfReg    = 0x67; break;
	    case 3 : //64-QAM
	    	_ConfReg    =  0x2b; break; // justin 0x6b;	break;
	    case 4 : //128-QAM
	    	_ConfReg    = 0x6f;	break;
		case 5 : //256-QAM
	    	_ConfReg    = 0x73;	break;
		default:
	    	_ConfReg    = 0x6b;	break;
	}

	/* set the value of the CONF register */
	val = _ConfReg;
	//Drv_Ch_I2C_DemodWrite_CAB(SB_CONF, &val, 1);

	/* soft-reset the QAM logic */
	/* set the CLR bit to zero. CLB signal will automatically return to 1 after 5 XIN period */
	val = _ConfReg & 0xfe;
	Drv_Ch_I2C_DemodWrite_CAB(nChannelId, TDA10023_DEMOD_ADDRESS, SB_CONF, &val, 1);

	val = 0x0; // phillips 0x7f;  // DVB Mode select
	Drv_Ch_I2C_DemodWrite_CAB(nChannelId,  TDA10023_DEMOD_ADDRESS, SB_RESET, &val, 1);
}

#ifdef  EAGLE_DEBUG_CH
void JQAM_REG5(int nChannelId)
{
	HUINT8 val;

	Drv_Ch_I2C_DemodRead_CAB(nChannelId, TDA10023_JQAM_ADDRESS,  TDA10023_REG5_IND, &val, 1);

	CH_DI_Print (4, ("JQAM_REG5[%d]:: JQAM status = %x\n",nChannelId,val));
	if((val & 0x80) == 1)
	{
		CH_DI_Print (4, ("JQAM_REG5[%d]:: ERROR!\n",nChannelId));
	}

	if((val & 0x40) == 1)
	{
		CH_DI_Print (4, ("JQAM_REG5[%d]:: EMERGENCY!\n",nChannelId));
	}

	if((val & 0x30) == 0x00)
	{
		CH_DI_Print (4, ("JQAM_REG5[%d]:: Signal Bad\n",nChannelId));  //  jcom에서 반대로 설정해서 내려오고  있음.
	}
	else if((val & 0x30) == 0x10)
	{
		CH_DI_Print (4, ("JQAM_REG5[%d]:: Signal Poor\n",nChannelId));
	}
	else if((val & 0x30) == 0x20)
	{
		CH_DI_Print (4, ("JQAM_REG5[%d]:: Signal Good\n",nChannelId));
	}
	else if((val & 0x30) == 0x30)
	{
		CH_DI_Print (4, ("JQAM_REG5[%d]:: Reserved\n",nChannelId));
	}

	CH_DI_Print (4, ("JQAM_REG5:: VER = %x\n", (val & 0x0e) >> 1));

	if((val & 0x01) == 1)
	{
		CH_DI_Print (4, ("JQAM_REG5[%d]:: MLOCK OK\n",nChannelId));
	}
}


void JQAM_TsStatus(int nChannelId)
{
	HUINT8 val;

	Drv_Ch_I2C_DemodRead_CAB(nChannelId, TDA10023_JQAM_ADDRESS,  TDA10023_TS_STATUS_H_IND, &val, 1);
	CH_DI_Print (4, ("JQAM_TsStatus[%d]:: 1. ts status = %x\n",nChannelId,val));

	Drv_Ch_I2C_DemodRead_CAB(nChannelId, TDA10023_JQAM_ADDRESS,  TDA10023_TS_STATUS_L_IND, &val, 1);
	CH_DI_Print (4, ("JQAM_TsStatus[%d]:: 2. ts status = %x\n",nChannelId,val));

	Drv_Ch_I2C_DemodRead_CAB(nChannelId, TDA10023_JQAM_ADDRESS,  TDA10023_RCV_STATUS4_IND, &val, 1);
	CH_DI_Print (4, ("JQAM_TsStatus[%d]:: 1. ts rcv status = %x\n",nChannelId,val));

	Drv_Ch_I2C_DemodRead_CAB(nChannelId, TDA10023_JQAM_ADDRESS,  TDA10023_RCV_STATUS3_IND, &val, 1);
	CH_DI_Print (4, ("JQAM_TsStatus[%d]:: 2. ts rcv status = %x\n",nChannelId,val));

	Drv_Ch_I2C_DemodRead_CAB(nChannelId, TDA10023_JQAM_ADDRESS,  TDA10023_RCV_STATUS2_IND, &val, 1);
	CH_DI_Print (4, ("JQAM_TsStatus[%d]:: 3. ts rcv status = %x\n",nChannelId,val));

	Drv_Ch_I2C_DemodRead_CAB(nChannelId, TDA10023_JQAM_ADDRESS,  TDA10023_RCV_STATUS1_IND, &val, 1);
	CH_DI_Print (4, ("JQAM_TsStatus[%d]:: 4. ts rcv status = %x\n",nChannelId,val));
}

void JQAM_TS(int nChannelId)
{
	HUINT8 val;
	int i, j;

	j=0;

	for (i=0; i<60; i++)
	{
		j++;
		Drv_Ch_I2C_DemodRead_CAB(nChannelId, TDA10023_JQAM_ADDRESS,  TDA10023_TS_ID1H_IND + i, &val, 1);
		CH_DI_Print (4, ("TS_ID[%d] [%d].= %x\n",nChannelId,i+1,val));

		if(j > 3)
		{
			j= 0;
			CH_DI_Print(4, ("\n"));
		}
	}
}

void Sig_Uncor_PKT(int nChannelId)
{
	HUINT8	puBytes[4];
	HUINT32	uncor_pkt, cor_pkt, ok_pkt;

	/* Get the number of uncorrected packet  */
	Drv_Ch_I2C_DemodRead_CAB(nChannelId,  TDA10023_DEMOD_ADDRESS, CPT_TSP_UNCOR1, &puBytes[0], 4);

    uncor_pkt = (puBytes[3]<<24) | (puBytes[2]<<16) | (puBytes[1]<<8) | puBytes[0];
    CH_DI_Print (4, ("Uncor_Pkt[%d]:: uncorrected packet = %d\n",nChannelId,uncor_pkt));

	/* Get the number of corrected packet  */
	Drv_Ch_I2C_DemodRead_CAB(nChannelId, TDA10023_DEMOD_ADDRESS, CPT_TSP_COR1, &puBytes[0], 4);

    cor_pkt = (puBytes[3]<<24) | (puBytes[2]<<16) | (puBytes[1]<<8) | puBytes[0];
    CH_DI_Print (4, ("Cor_Pkt[%d]:: corrected packet = %d\n",nChannelId,cor_pkt));

	/* Get the number of unerroed packet  */
	Drv_Ch_I2C_DemodRead_CAB(nChannelId, TDA10023_DEMOD_ADDRESS,  CPT_TSP_OK1, &puBytes[0], 4);

    ok_pkt = (puBytes[3]<<24) | (puBytes[2]<<16) | (puBytes[1]<<8) | puBytes[0];
    CH_DI_Print (4, ("Ok_Pkt[%d]:: ok packet = %d\n",nChannelId,ok_pkt));
    }


void d4r(int nChannelId, HUINT8 addr)
{
	HUINT8 addr1, val;

	addr1 = addr;

	Drv_Ch_I2C_DemodRead_CAB(nChannelId,  TDA10023_DEMOD_ADDRESS, addr1, &val,1);

	CH_DI_Print(5, ("#### addr = %x, val= %x\n", addr1, val));
}

void d4w(int nChannelId, HUINT8 addr, HUINT8 val)
{
	HUINT8 addr1, val1;

	addr1 = addr;
	val1 = val;

	Drv_Ch_I2C_DemodWrite_CAB(nChannelId,  TDA10023_DEMOD_ADDRESS, addr1, &val1, 1);
}

void  ifset(HUINT8 if_reg_val_max, unsigned char if_reg_val, HUINT8 agc_m)
{
	gucCABTopOn = agc_m;
	gucIfMinC = if_reg_val;
	gucIfMaxC = if_reg_val_max;
}

void rfset(unsigned char rf_reg_val, HUINT8 rf_reg_val_min, HUINT8 agc_m)
{
	gucCABTopOn = agc_m;
	gucRfMaxC = rf_reg_val;
	gucRfMinC = rf_reg_val_min;
}

void eqset(unsigned char eq_reg_val)
{
	eq_val = eq_reg_val;
}

void crset(HUINT8 cr_set)
{
	cr_val = cr_set;
}

void qamset(HUINT8 qam_set)
{
	qam_val = qam_set;
}

void go_bad(void)
{
	tsmf_flag = 1;
	test_gobad = 1;
}

void go_good(void)
{
	tsmf_flag = 1;
	test_gobad = 0;
}

void mcns(void)
{
	HUINT8 val;

	CH_DI_Print (5, ("mcns:: \n"));
     //
      Drv_Ch_I2C_DemodRead_CAB(0,  TDA10023_DEMOD_ADDRESS, 0x1f, &val, 1);
      val = 0x80 | val;
      Drv_Ch_I2C_DemodWrite_CAB(0,  TDA10023_DEMOD_ADDRESS, 0x1f, &val, 1);

      Drv_Ch_I2C_DemodRead_CAB(1, TDA10023_DEMOD_ADDRESS,  0x1f, &val, 1);
      val = 0x80 | val;
      Drv_Ch_I2C_DemodWrite_CAB(1, TDA10023_DEMOD_ADDRESS, 0x1f, &val, 1);
}
#endif

void Saclk_OnOff(int nChannelId, HUINT8 onOff)
{
	HUINT8	val;
	if(onOff == 0)	/* Off */
	{
		val = 0x23; /* For Test 0x1E, D[2] = 0, others are set by default */
		Drv_Ch_I2C_DemodWrite_CAB(nChannelId, TDA10023_DEMOD_ADDRESS, 0x1E, &val, 1);
	}
	else if(onOff == 1)	/* On */
	{
		val = 0x27; /* For Test 0x1E, D[2] = 1, others are set by default */
		Drv_Ch_I2C_DemodWrite_CAB(nChannelId, TDA10023_DEMOD_ADDRESS, 0x1E, &val, 1);
	}
	else
	{
		CH_DI_Print(3, ("Wrong param!!!!\n"));
	}
}

void ChangeCP(HUINT8 unit, HUINT8 val, HUINT8 sAct)
{
	HUINT8 tnr = 0, sCP=0;
	HUINT32	freq;

	chgCP = sAct;

	CH_DI_Print(5, ("Usage : ChangeCP 0/1, value, mode\n"));
	CH_DI_Print(5, ("	0: Master, 1: Slave\n"));
	CH_DI_Print(5, ("	value 1:38uA,2:54uA,3:83uA,4:122uA,5:163uA,6:254uA,7:400uA,8:580uA\n"));
	CH_DI_Print(5, ("	mode: 0= Inactive, 1=Active\n"));

	freq = gTDA10023ChInfo[unit].frequency;

	if ( freq <= 169000000 )
	{
		tnr = 0x81;
	}
	else if (freq <= 469000000)
	{
		tnr = 0x82;
	}
	else if (freq < 866000000)
	{
		tnr = 0x84;
	}
	else
	{
		CH_DI_Print(3, ("ChangeCP:: ERROR : Out of frequency range for tuner\n"));
	}

	switch(val)
	{
		case 1:
			sCP = 0x0; //0x1f;
			break;
		case 2:
			sCP = 0x1; //0x3f;
			break;
		case 3:
			sCP = 0x2; //0x5f;
			break;
		case 4:
			sCP = 0x3;// 0x7f;
			break;
		case 5:
			sCP = 0x4;//0x9f;
			break;
		case 6:
			sCP = 0x5;//0xbf;
			break;
		case 7:
			sCP = 0x6;//0xdf;
			break;
		case 8:
			sCP = 0x7;//0xff;
			break;
		default :
			CH_DI_Print(3, ("Out of Value %d\n", val));
			break;

	}

	gValCP = (tnr&0x1f) | ((sCP<<5)&0xe0);
	CH_DI_Print(5, ("ChangeCP = 0x%x\n", gValCP));
}


void ChangeBand(HUINT8 unit, HUINT8 val, HUINT8 sAct)
{
	CH_UNUSED(unit);

	chgBand = sAct;

	CH_DI_Print(5, ("Usage : ChangeBand  0/1, value, mode\n"));
	CH_DI_Print(5, ("	0: Master, 1: Slave\n"));
	CH_DI_Print(5, ("	value 1:Low 2:Mid, 3:High\n"));
	CH_DI_Print(5, ("	mode: 0= Inactive, 1=Active\n"));

	switch(val)
	{
		case 1:
			gValBand = 0x1;
			break;
		case 2:
			gValBand = 0x2;
			break;
		case 3:
			gValBand = 0x4;
			break;

		default :
			CH_DI_Print(3, ("Out of Value %d\n", val));
			break;
	}
}

void AgcLoopSet(HUINT8 aVal, HUINT8 aflag)
{
	if ( (aVal == 0) && (aflag == 0))
	{
		CH_DI_Print(5, ("Usage : AgcLoopSet <001[0x89], 010[0x92], 011[0x9b] ,100[0xa4]>, 1\n"));
	}
	else
	{
		agcFlag = aflag;
		agcVal = aVal;
	}
}

void DRV_C_InstallApi( void )
{
	CAB_InitDevice 			= &DP10023_InitDevice;
	CAB_SetTune 			= &DP10023_SetTune;
	CAB_CheckLock 			= &DP10023_CheckLock;
	CAB_GetStrengthQuality 	= &DP10023_GetStrengthQuality;
	CAB_SetPowerState 		= &DP10023_SetPowerState;
	CAB_EnableTsOutput 		= &DP10023_EnableTsOutput;
	CAB_DisableTsOutput 	= &DP10023_DisableTsOutput;
//CAB_UpdateTunedParam	= &DP10023_UpdateTunedParam;
//CAB_GetChannelStatus	= &DP10023_GetChannelStatus;
	CAB_ResetGpio = &DP10023_ResetGpio;
	CAB_Initialize = &DP10023_Initialize;

	CH_DI_Print(5, ("DRV_C_InstallApi()  OK! \n"));

	return;
}

int DP10023_InitDevice(int nChannelId)
{
	CH_UNUSED(nChannelId);

	return DI_CH_OK;
}

int DP10023_LockDemodulator(int nChannelId)
{
	int	err;
	HUINT8		data, data2;

	if ( fast_lock_TDA(nChannelId) )
	{
#if 0
		data =0x02; // justin 040325 0x00; // justin  0x40;
		DemodIicTx(SB_ITSEL, &data, 1);

		data = 0x04; // justin 0x00;
		DemodIicTx(SB_ITSEL2, &data, 1);

		/* read the interrupt status register to prevent
		   the previous unlock state from reflecting the current lock status */
		DemodIicRx(SB_ITSTAT1, &data, 1);
#endif

//		CH_DI_Print ("Channel Locking  Ok!\n");
		/* adapt BER counter window according to the number
		   of the elapsed bit for the GET_DATA_INTERVAL period */

/* justin 	PBER 0xf8로 셋팅하면, BER 계산이 너무오래 걸림.
             AdaptBerWindow();
*/
		/* after lock, clear the value of BER conuter */
		Drv_Ch_I2C_DemodRead_CAB(nChannelId, TDA10023_DEMOD_ADDRESS, SB_RSCONF, &data, 1);
//		OSA_Print("####  Read %x\n", data);
		data2 = data & 0xdf;
		/* CLB_CPT_TSP = 0 */
		Drv_Ch_I2C_DemodWrite_CAB(nChannelId, TDA10023_DEMOD_ADDRESS, SB_RSCONF, &data2, 1);
		/* CLB_UNC bit = 1 */
		data2  = data | 0x20;
		Drv_Ch_I2C_DemodWrite_CAB(nChannelId, TDA10023_DEMOD_ADDRESS, SB_RSCONF, &data2, 1);
		err = DI_CH_OK;
	}
	else
	{
		err = DI_CH_ERR;
	}

	return err;
}

int DP10023_CheckStatus(int nChannelId)
{
	int	err;
//	HUINT32	BERValue[3] = {0,};
//	HUINT32	UnCorPacket[3] = {0,};
//	HUINT32	MaxBER = 0, MaxUnCor = 0;
//	HUINT32	TotalAvgBER = 0;
	HUINT8	GetFEClock;
	HUINT8   	val1;
//	HUINT8 i;
//	HUINT8 ber_set_cfg = 0;		/* 1 : after Viterbi, 0 : RS decoding */
	HUINT8	rfAgc, ifAgc;

	/* RF Cable 제거시 Unlock 메시지 올리는 루틴 */
	DP10023_GetAGC(nChannelId, &rfAgc, &ifAgc);
	if( (rfAgc == 0xff) && (ifAgc == 0xff) )
	{
		gTDA10023ChInfo[nChannelId].strength = 0;
		gTDA10023ChInfo[nChannelId].quality = 0;
		gTDA10023ChInfo[nChannelId].snr = 0;
		gTDA10023ChInfo[nChannelId].emergencyFlag = 0;
		totalCntToDisplayBER[nChannelId] = 0;
		err = DI_CH_ERR;
		return err;
	}

	/* Read the SYNC status register */
	Drv_Ch_I2C_DemodRead_CAB(nChannelId, TDA10023_DEMOD_ADDRESS, SB_SYNC, &val1, 1);
	GetFEClock = val1&0x48;

	if ( GetFEClock!=0x08 )
	{
		VK_TASK_Sleep(120); /* this delay is necessary */
		Drv_Ch_I2C_DemodRead_CAB(nChannelId, TDA10023_DEMOD_ADDRESS, SB_SYNC, &val1, 1);
		CH_DI_Print(3, ("One more Try[%d] : SB_SYNC = 0x%x\n",nChannelId,val1));

		GetFEClock = val1&0x48;

		if ( GetFEClock!=0x08 )
		{
			gTDA10023ChInfo[nChannelId].strength = 0;
			gTDA10023ChInfo[nChannelId].quality = 0;
			gTDA10023ChInfo[nChannelId].snr = 0;
			gTDA10023ChInfo[nChannelId].emergencyFlag = 0;
			totalCntToDisplayBER[nChannelId] = 0;

			CH_DI_Print(2, ("DP10023_CheckStatus[%d]:: Unlock (freq= %d) \n", nChannelId, gTDA10023ChInfo[nChannelId].frequency));
			err = DI_CH_ERR;
			return err;
		}
	}

	gTDA10023ChInfo[nChannelId].strength = Strength_Info_TDA(nChannelId);
	gTDA10023ChInfo[nChannelId].quality = BER_Info_TDA(nChannelId); /* 실제 값으로 대치할 것 */
	gTDA10023ChInfo[nChannelId].snr = SNR_Info_TDA(nChannelId); /* 실제 값으로 대치할 것 */

	gCurrentId[nChannelId] = 1;

	accuBER[nChannelId] += gTDA10023ChInfo[nChannelId].quality;		/* BER 누적 */

	gTDA10023Correction[nChannelId].CorrectedNo[0] = Sig_Corrected_PKT(nChannelId);
	gTDA10023Correction[nChannelId].UnCorrectedNo[0] = Sig_UnCorrected_PKT (nChannelId);
	gTDA10023Correction[nChannelId].UnErroredNo[0] = Sig_Unerroed_PKT (nChannelId);

	gTDA10023Correction[nChannelId].InbandBer[0] = gTDA10023ChInfo[nChannelId].quality;

	CH_DI_Print (2, ("TDA10023_CheckStatus[%d]:: Strength = %d, Quality = %d, SNR = %d\n", nChannelId, gTDA10023ChInfo[nChannelId].strength ,gTDA10023ChInfo[nChannelId].quality, gTDA10023ChInfo[nChannelId].snr)); // justin_1

	totalCntToDisplayBER[nChannelId]++;
	if(totalCntToDisplayBER[nChannelId] == 16)		/* 폴링주기 500ms, 500* 16 = 8sec */
	{
		avgBER[nChannelId] = accuBER[nChannelId] / 16;		/* 8초간 누적시킨 BER을 평균 내기 */
		CH_DI_Print (3, ("TDA10023_CheckStatus[%d]:: Average Quality = %d\n", nChannelId, avgBER[nChannelId]));
		gTDA10023Correction[nChannelId].InbandBer[0] = avgBER[nChannelId];
		accuBER[nChannelId] = 0;
		totalCntToDisplayBER[nChannelId]= 0;
	}

	CH_DI_Print(2, ("DP10023_CheckStatus[%d]:: Lock (freq= %d) \n", nChannelId, gTDA10023ChInfo[nChannelId].frequency));

	err = DI_CH_OK;

	return err;
}

int  DP10023_ResetGpio(int nChannelId)
{
	HUINT32 	nUnitId = 0;

	nUnitId = GetDeviceUnitId( nChannelId );

	CH_DI_Print(2,("nChannelID[%d] / nUnitId[%d] \n", nChannelId, nUnitId));

	if(nChannelId == MASTER_CAB_DEMOD)
	{
		DRV_GPIO_Write(GPIO_ID_RST_QAM, GPIO_LOW);
		VK_TASK_Sleep(30);

		DRV_GPIO_Write(GPIO_ID_SEL_TC1, GPIO_LOW);
		CH_DI_Print(2,(" Set Tuner QAM Mode[%d] - MASTER CAB\n",nChannelId));

		DRV_GPIO_Write(GPIO_ID_RST_QAM, GPIO_HI);
		VK_TASK_Sleep(30);
	}
	else if(nChannelId== SLAVE_CAB_DEMOD)
	{
		DRV_GPIO_Write(GPIO_ID_RST_QAM2, GPIO_LOW);
		VK_TASK_Sleep(30);

		DRV_GPIO_Write(GPIO_ID_SEL_TC2, GPIO_LOW);
		CH_DI_Print(2,(" Set Tuner QAM Mode[%d] - SLAVE CAB\n",nChannelId));

		DRV_GPIO_Write(GPIO_ID_RST_QAM2, GPIO_HI);
		VK_TASK_Sleep(30);
	}
	else
	{
		CH_DI_Print(0, ("[DP10023_ResetGpio] ERR!! nChannelId[%d] \n", nChannelId));
	}
	return DI_CH_OK;
}

int DP10023_Initialize(int nChannelId)
{
	//3, 5274, 653000000
	gTDA10023ChInfo[nChannelId].qamMode = 3;
	gTDA10023ChInfo[nChannelId].symbolRate = 5274;
	gTDA10023ChInfo[nChannelId].frequency = 653000000;

	sTsmfGoodFlag[nChannelId] = 1;
	sTsmfEmgOffFlag[nChannelId] = 1;

	CH_DI_Print(5,("DP10023_Initialize[%d]... \n", nChannelId));

	clearDemod(nChannelId);

	InitQAMdemod(nChannelId);

	MxL201_Init_CAB(nChannelId);
	CH_DI_Print(5, ("DP10023_Initialize : Mxl201 Tuner Init[%d] \n", nChannelId));

	return DI_CH_OK;
}

int DP10023_SetTune(int nChannelId, HUINT32 ulWorkingId, CH_CAB_TuneParam_t *pstCabTuneParam )
{
	int	err;
	HINT32 tsid, netid, nUnitId = 0;
	HUINT8 val;

	CH_UNUSED(ulWorkingId);

	nUnitId = GetDeviceUnitId( nChannelId );
	if (nChannelId == MASTER_CAB_DEMOD)
	{
		DRV_GPIO_Write(GPIO_ID_SEL_TC1, GPIO_LOW);
		CH_DI_Print(2,("DP10023_SetTune[%d]...T/C Switch #1 is set to CAB \n",nChannelId));
	}
	else if (nChannelId == SLAVE_CAB_DEMOD)
	{
		DRV_GPIO_Write(GPIO_ID_SEL_TC2, GPIO_LOW);
		CH_DI_Print(2,("DP10023_SetTune[%d]...T/C Switch #2 is set to CAB \n",nChannelId));
	}
	else
	{
		CH_DI_Print(0,("DP10023_SetTune[%d]...Error!! Wrong Channel ID [%d] \n",nChannelId,nUnitId));
	}

	gTDA10023ChInfo[nChannelId].qamMode = pstCabTuneParam->etModulation;
	gTDA10023ChInfo[nChannelId].symbolRate = pstCabTuneParam->ulSymbolRate;
	gTDA10023ChInfo[nChannelId].frequency = pstCabTuneParam->ulFrequency*1000;
	gTDA10023ChInfo[nChannelId].netId = pstCabTuneParam->nNetId;
	gTDA10023ChInfo[nChannelId].tsId = pstCabTuneParam->nTsId;

	 CH_DI_Print(3,("DP10023_SetTune[%d].........\n",nChannelId));
	//temp
	//gTDA10023ChInfo[nChannelId].netId = 4;
	//gTDA10023ChInfo[nChannelId].tsId = 16626;

	CH_DI_Print (2, ("DP10023_SetTune[%d]::  freq= %d\n",nChannelId,gTDA10023ChInfo[nChannelId].frequency));
	//CH_DI_Print ("DP10023_SetTune::  netId= %d\n", gTDA10023ChInfo[nChannelId].netId);
	//CH_DI_Print ("DP10023_SetTune::  tsId= %d\n",  gTDA10023ChInfo[nChannelId].tsId);

	beforeId1[nChannelId] = 0;
	beforeId2[nChannelId] = 0;
	gCurrentId[nChannelId] = 0;

	if (save_stream == 1 )
	{
		err =DP10023_SetTuner(nChannelId);

		if(err != DI_CH_OK)
		{
			err = DI_CH_ERR;
			return err;
		}

		SetQAMdemod(nChannelId);
		VK_TASK_Sleep( (HUINT16)(no_to_msec(nChannelId, WAIT_AGCLOCK*3)) );   // 76ms

		DP10023_LockDemodulator(nChannelId);

		JQAM_Mode(nChannelId, 0);

		val = 0x00;
		Drv_Ch_I2C_DemodWrite_CAB(nChannelId, TDA10023_DEMOD_ADDRESS, SB_CONTROL, &val, 1);

		return DI_CH_OK;
	}

	if ( gTDA10023ChInfo[nChannelId].frequency == 999000000 )
	{
		/* TS output set to tri-state*/
		val = 0x02;  /* TRIP */
		Drv_Ch_I2C_DemodWrite_CAB(nChannelId, TDA10023_DEMOD_ADDRESS, SB_CONTROL, &val, 1);

		err = DI_CH_OK;
		return err ;
	}

	err = DP10023_SetTuner(nChannelId);
	if(err != DI_CH_OK)
	{
		err = DI_CH_ERR;
		return err;
	}

	SetQAMdemod(nChannelId);

	err=DP10023_LockDemodulator(nChannelId);		/* 락킹 시도 */

	tsid  = gTDA10023ChInfo[nChannelId].tsId;
	netid = gTDA10023ChInfo[nChannelId].netId;

	VK_TASK_Sleep(100);		/*  채널전환 또는  standby off->on 반복 시  초기에 TSMF number가 0이 되는 경우 발생해서 E203 표시되는 문제 수정  2009-12-19 */

	gTDA10023TsmfInfo[nChannelId].tsNo = JQAM_TsNumber(nChannelId);


	CH_DI_Print(1, ("DP10023_SetTune[%d]:: Lock = %d, tsid = %x, netid= %x, tsno = %d\n",  nChannelId, err, tsid, netid, gTDA10023TsmfInfo[nChannelId].tsNo));


	if ( (tsid == -1) && (netid == -1) )
	{
		gTDA10023TsmfInfo[nChannelId].tsNo = JQAM_TsNumber(nChannelId);

		JQAM_TSinfo(nChannelId);
	}
	else if (gTDA10023TsmfInfo[nChannelId].tsNo < 2 )
	{
		JQAM_Mode(nChannelId, 0);
		gTDA10023TsmfInfo[nChannelId].tsNo = 0;
	}
	else
	{
		JQAM_Mode(nChannelId, 2);
		JQAM_TsId( nChannelId, tsid, netid);

		gTDA10023TsmfInfo[nChannelId].tsNo = 0;
	}

	if(err == DI_CH_OK) /* 파라미터 검사 OK */
	{
		CH_DI_Print(1, ("[DP10023_SetTune[%d]] LOCK! \n",nChannelId));

		/* To prevent block noise, low snr at signal line plug-in and plug-out  */
		HUINT16 snr =0;
		HUINT32 uncorredPkt =0;

		snr = SNR_Info_TDA(nChannelId); /* 실제 값으로 대치할 것 */
		uncorredPkt = Sig_UnCorrected_PKT (nChannelId);

		CH_DI_Print(0, ("[DP10023_SetTune[%d]] snr = %d  uncorredPkt = %d \n",nChannelId,snr, uncorredPkt));

		if(snr < 251 && uncorredPkt > 2000)
		{
			CH_DI_Print(0, ("[DP10023_SetTune[%d]] UNLOCK! \n",nChannelId));
			err = DI_CH_ERR;
			return err;
		}

		err = DI_CH_OK;
	}
	else /* 파라미터 오류 */
	{

		CH_DI_Print(1, ("[DP10023_SetTune[%d]] UNLOCK! \n",nChannelId));
		err = DI_CH_ERR;
	}

	return err;
}

void DP10023_QAMStop(int nChannelId)
{
	gTDA10023ChInfo[nChannelId].strength = 0;
	gTDA10023ChInfo[nChannelId].quality = 0;
	gTDA10023ChInfo[nChannelId].snr = 0;
	gTDA10023ChInfo[nChannelId].emergencyFlag = 0;

	/* 동일 락킹 파라메터인 경우 다시 tune을 하지 않으므로 stop시 긴급정보 메세지 관련 초기화 해준다. */
	beforeId1[nChannelId] = 0;
	beforeId2[nChannelId] = 0;
	gCurrentId[nChannelId] = 0;

	if(save_stream != 1)
	{
		DeactivateDemodulator(nChannelId);
	}
}

HBOOL DP10023_CheckLock(int nChannelId)
{
	int	err = DI_CH_OK;

	err = DP10023_CheckStatus(nChannelId);

	gTDA10023TsmfInfo[nChannelId].tsNo = JQAM_TsNumber(nChannelId);

	JQAM_Status(nChannelId);   /* emergency, status */

	if(err != DI_CH_ERR) /* locking 상태 이면 */
	{
		return TRUE;
	}
	else /* unlock 상태이면 */
	{
		return FALSE;
	}

}

int DP10023_GetStatus(int nChannelId, HUINT32 frequency)
{
	int	err = DI_CH_OK;
	HUINT32 unRequestId = 0;

	gTDA10023ChInfo[nChannelId].frequency = frequency;

	if ( gTDA10023ChInfo[nChannelId].frequency == 999000000 )
	{
		err = DI_CH_OK;
		return err ;
	}

	err = DP10023_CheckStatus(nChannelId);

	gTDA10023TsmfInfo[nChannelId].tsNo = JQAM_TsNumber(nChannelId);

	JQAM_Status(nChannelId);   /* emergency, status */

	if(err != DI_CH_ERR) /* locking 상태 이면 */
	{
		/* 서비스 ID가 바뀔때 마다 메시지 보내고, 동일 service id일경우는 처음 한번만 보냄. */
		if( (gTDA10023ChInfo[nChannelId].snr > 25) && (gTDA10023ChInfo[nChannelId].tsmfStatus == 2) )
		{
			GoodRetry = 0;
			CH_DI_Print(5, ("### BadRetry[%d] = %d\n",nChannelId,BadRetry));
			if ( BadRetry > 4)
			{
				sTsmfGoodFlag[nChannelId] = TRUE;

				if((sTsmfBadFlag[nChannelId] == FALSE) || (beforeId1[nChannelId] != gCurrentId[nChannelId]))
				{
					sTsmfBadFlag[nChannelId] = TRUE;
					beforeId1[nChannelId] = gCurrentId[nChannelId];

					CH_DI_Print (1,("TDA10023_QAMGetStatus[%d]:: Send TSMF Bad Message .....\n\n",nChannelId));

					//GWM_SendMessage(MSG_CH_TSMF_BAD, versionNumber, CH_MOD_QAM, 0);
					DRV_CH_CallNotifyCallback( nChannelId, unRequestId, DI_CH_SIGNAL_TSMF_BAD);

					BadRetry = 0;
				}
			}
			else
			{
				BadRetry++;
			}
		}
		else
		{
			BadRetry = 0;
			CH_DI_Print (5, ("### GoodRetry[%d] = %d\n",nChannelId,GoodRetry));

			if(GoodRetry > 4)
			{
				sTsmfBadFlag[nChannelId] = FALSE;

				if((sTsmfGoodFlag[nChannelId] == TRUE) || (beforeId1[nChannelId] != gCurrentId[nChannelId]))
				{
					sTsmfGoodFlag[nChannelId] = FALSE;
					beforeId1[nChannelId] = gCurrentId[nChannelId];

					CH_DI_Print (1, ("TDA10023_QAMGetStatus[%d]:: Send TSMF Good Message .....\n\n",nChannelId));

					//GWM_SendMessage(MSG_CH_TSMF_GOOD, versionNumber, CH_MOD_QAM, 0);
					DRV_CH_CallNotifyCallback( nChannelId, unRequestId, DI_CH_SIGNAL_TSMF_GOOD);

					GoodRetry = 0;
				}
			}
			else
			{
				GoodRetry++;
			}
		}

		if( gTDA10023ChInfo[nChannelId].emergencyFlag == 1)
		{
			sTsmfEmgOffFlag[nChannelId] = TRUE;
			if((sTsmfEmgOnFlag[nChannelId] == FALSE) || (beforeId2[nChannelId] != gCurrentId[nChannelId]) )
			{
				sTsmfEmgOnFlag[nChannelId] = TRUE;
				beforeId2[nChannelId] = gCurrentId[nChannelId];

				CH_DI_Print (1, ("TDA10023_QAMGetStatus[%d]:: TSMF Emergency On ..... \n\n",nChannelId));

				//GWM_SendMessage(MSG_CH_EWS_ON, versionNumber, CH_MOD_QAM, 0);
				DRV_CH_CallNotifyCallback( nChannelId, unRequestId, DI_CH_SIGNAL_TSMF_EWS_ON);
			}
		}
		else
		{
			sTsmfEmgOnFlag[nChannelId] = FALSE;
			if((sTsmfEmgOffFlag[nChannelId] == TRUE) || (beforeId2[nChannelId] != gCurrentId[nChannelId]) )
			{
				sTsmfEmgOffFlag[nChannelId] = FALSE;
				beforeId2[nChannelId] = gCurrentId[nChannelId];

				CH_DI_Print (1, ("TDA10023_QAMGetStatus[%d]:: TSMF Emergency Offl .....\n\n",nChannelId));

				//GWM_SendMessage(MSG_CH_EWS_OFF, versionNumber, CH_MOD_QAM, 0);
				DRV_CH_CallNotifyCallback( nChannelId, unRequestId, DI_CH_SIGNAL_TSMF_EWS_OFF);
			}
		}

		/* To prevent block noise, low snr at signal line plug-in and plug-out  */
		HUINT16 snr =0;
		HUINT32 uncorredPkt =0;

		snr = SNR_Info_TDA(nChannelId); /* 실제 값으로 대치할 것 */
		uncorredPkt = Sig_UnCorrected_PKT (nChannelId);

		if(snr < 251 && uncorredPkt > 2000)
		{
			CH_DI_Print(0, ("[DP10023_GetStatus[%d]] snr = %d  uncorredPkt = %d \n",nChannelId,snr, uncorredPkt));
			CH_DI_Print(0, ("[DP10023_GetStatus[%d]] UNLOCK! \n",nChannelId));
			err = DI_CH_ERR;
			return err;
		}

	}
	else /* unlock 상태이면 */
	{
		gTDA10023ChInfo[nChannelId].strength = 0;
		gTDA10023ChInfo[nChannelId].quality = 0;
		gTDA10023ChInfo[nChannelId].snr = 0;
		gTDA10023ChInfo[nChannelId].emergencyFlag = 0;
	}

	return err;
}

int DP10023_CheckTsmfLock(int nChannelId, HUINT32 frequency)
{
	int	err = DI_CH_OK;
	int	checklock = FALSE;

	gTDA10023ChInfo[nChannelId].frequency = frequency;

	if ( gTDA10023ChInfo[nChannelId].frequency == 999000000 )
	{
		err = DI_CH_OK;
		return err ;
	}

	err = DP10023_CheckStatus(nChannelId);

	gTDA10023TsmfInfo[nChannelId].tsNo = JQAM_TsNumber(nChannelId);

	JQAM_Status(nChannelId);   /* emergency, status */

	if(err != DI_CH_ERR) /* locking 상태 이면 */
	{
		checklock = TRUE;
	}
	else /* unlock 상태이면 */
	{
		gTDA10023ChInfo[nChannelId].strength = 0;
		gTDA10023ChInfo[nChannelId].quality = 0;
		gTDA10023ChInfo[nChannelId].snr = 0;
		gTDA10023ChInfo[nChannelId].emergencyFlag = 0;

		checklock = FALSE;
	}

	return checklock;

}

void DP10023_GetChLockData(int nChannelId, HINT16 *strength, HUINT32 *quality, HUINT16 *snr)
{
	*strength = gTDA10023ChInfo[nChannelId].strength;
	*quality = gTDA10023ChInfo[nChannelId].quality;
	*snr = gTDA10023ChInfo[nChannelId].snr;
//	*spectralInverse = gTDA10023ChInfo[nChannelId].SpectralInverse;

	//DDI_UART_Print("[DP10023] strength = %d, quality = %d , snr = %d \n ",*strength , *quality, *snr );
}

int  DP10023_GetStrengthQuality(int nChannelId, DI_CH_SignalQuality_t *pstSignalInfo)
{
	HUINT8 ucRfAgc=0, ucIfAgc=0;
	HINT16 ucStrength=0;
	HUINT32 ulQuality=0;
	HUINT16 ucSnr=0;
	HFLOAT32	fInputPower = 0;

	DP10023_GetAGC( nChannelId, &ucRfAgc, &ucIfAgc );
	DP10023_GetChLockData(nChannelId, &ucStrength, &ulQuality, &ucSnr);

	enable_tuner_iic(nChannelId);
	MxL_Check_RF_Input_Power(&g_stSemTunerConfigCab[nChannelId], &fInputPower);
	disable_tuner_iic(nChannelId);

	pstSignalInfo->ulStrength	= (HUINT32)ucStrength;
	pstSignalInfo->ulAgc		= (HUINT32)ucRfAgc;
	pstSignalInfo->ulQuality	= (HUINT32)ulQuality;
	pstSignalInfo->fSnr = (float)ucSnr/10;

	pstSignalInfo->ulRfAgc		= (HUINT32)ucRfAgc;
	pstSignalInfo->ulIfAgc		= (HUINT32)ucIfAgc;
	pstSignalInfo->fSignalInputPower	= fInputPower;

	pstSignalInfo->bEmergencyflag = gTDA10023ChInfo[nChannelId].emergencyFlag;

	pstSignalInfo->ulCorrectedNo  = gTDA10023Correction[nChannelId].CorrectedNo[0];
	pstSignalInfo->ulUnCorrectedNo= gTDA10023Correction[nChannelId].UnCorrectedNo[0];
	pstSignalInfo->ulUnErroredNo = gTDA10023Correction[nChannelId].UnErroredNo[0];
	pstSignalInfo->ulInbandBer = gTDA10023Correction[nChannelId].InbandBer[0];
	pstSignalInfo->ulUnCorrectedFlag = gTDA10023Correction[nChannelId].UnCorrectedFlag[0];

	pstSignalInfo->fBer = (HFLOAT32)gTDA10023Correction[nChannelId].InbandBer[0];

	CH_DI_Print(2, ("DP10023_GetStrengthQuality[%d]:: ulStrength =%d , ulRfAgc = %d, ulIfAgc = %d, ulQuality = %d, fSnr = %f, bEmergencyflag = %d \n ",
					nChannelId,pstSignalInfo->ulStrength,pstSignalInfo->ulRfAgc,pstSignalInfo->ulIfAgc, pstSignalInfo->ulQuality, pstSignalInfo->fSnr, pstSignalInfo->bEmergencyflag));

	CH_DI_Print(3, ("DP10023_GetStrengthQuality[%d]:: ulCorrectedNo =%d , ulUnCorrectedNo = %d, ulUnErroredNo = %d, ulInbandBer = %d, ulUnCorrectedFlag = %d \n ",
					nChannelId,pstSignalInfo->ulCorrectedNo,pstSignalInfo->ulUnCorrectedNo, pstSignalInfo->ulUnErroredNo, pstSignalInfo->ulInbandBer, pstSignalInfo->ulUnCorrectedFlag));

	CH_DI_Print(3, ("DP10023_GetStrengthQuality[%d]:: fSignalInputPower =%f \n ",nChannelId,pstSignalInfo->fSignalInputPower));

	return DI_CH_OK;
}

void DP10023_TsmfData(int nChannelId, HUINT16 *tsNo, HINT32 *tsID, HINT32 *netID)
{
	int i = 0;

	*tsNo = gTDA10023TsmfInfo[nChannelId].tsNo;

	JQAM_TSinfo(nChannelId);	/* At initial locking, uncorrected errorsl happen, demode get the wrong TSMF information. Add this API for getting correct TSMF 2009-10-27  */

	for(i = 0; i < 15; i++)
	{
		tsID[i] = gTDA10023TsmfInfo[nChannelId].tsID[i];
		netID[i] = gTDA10023TsmfInfo[nChannelId].netID[i];
	}
}


void DP10023_GetAGC(int nChannelId, HUINT8 *ucpRFAgc, HUINT8 *ucpIFAgc)
{
	HUINT8	ucRFVal, ucIFVal;

	Drv_Ch_I2C_DemodRead_CAB(nChannelId, TDA10023_DEMOD_ADDRESS, TDA10023_AGCIF, &ucIFVal, 1);		/* Read IF AGC */
	Drv_Ch_I2C_DemodRead_CAB(nChannelId, TDA10023_DEMOD_ADDRESS, TDA10023_AGCTUN, &ucRFVal, 1);		/* Read RF AGC */

	*ucpRFAgc = ucRFVal;
	*ucpIFAgc = ucIFVal;

	//DDI_UART_Print("[DP10023] RF AGC = %d, IF AGC = %d \n ",ucRFVal, ucIFVal);
}

void DP10023_GetEwsInfo(int nChannelId, HUINT8 *ucIsEws)
{
	*ucIsEws = gTDA10023ChInfo[nChannelId].emergencyFlag;
	CH_DI_Print (4, ("TDA10023_GetEwsInfo[%d]:: EmergencyFlag = 0x%x\n",nChannelId, gTDA10023ChInfo[nChannelId].emergencyFlag));
}

void DP10023_DumpRegister( int nChannelId)
{
	int i=0;
	unsigned char ucdata;

	//nUnitId = GetDeviceUnitId( nChannelId );

	CH_DI_Print(0, ("===================================================\n"));
	CH_DI_Print(0, ("   CAB TUNE Unit [%d] Demod Dump\n", nChannelId));
	CH_DI_Print(0, ("===================================================\n"));

	for(i=0; i<= 0xFE; i++)
	{
		Drv_Ch_I2C_DemodRead_CAB(nChannelId, TDA10023_DEMOD_ADDRESS, i, &ucdata, 1);
		CH_DI_Print(0, ("[0x%02X] \t[0x%02x]\n", i, ucdata));
	}

	CH_DI_Print(0, ("===================================================\n"));
	CH_DI_Print(0, ("   CAB TUNE Unit [%d] Tuner Dump\n", nChannelId));
	CH_DI_Print(0, ("===================================================\n"));

	for(i=0; i<= NUM_OF_MXLTUNER_REG; i++)
	{
	 	enable_tuner_iic(nChannelId);

		Drv_Ch_I2C_TunerRead_CAB(nChannelId, (HUINT8)i, &ucdata, 1);

		disable_tuner_iic(nChannelId);
		CH_DI_Print(0, ("[0x%02x]\n", ucdata));
	}
	CH_DI_Print(0, ("===================================================\n"));

}

int  DP10023_SetPowerState( int nChannelId, CH_Power_e etPower )
{
	CH_DI_Print(6, ("[DP10023_SetPowerState[%d]] ----\n",nChannelId));

	int nUnitId;

	nUnitId = GetDeviceUnitId( nChannelId );

	switch (etPower)
	{
		case CH_POWER_OFF:
			if (nChannelId == MASTER_CAB_DEMOD)
				DRV_GPIO_Write(GPIO_ID_RST_QAM, GPIO_LOW);
			else if(nChannelId == SLAVE_CAB_DEMOD)
				DRV_GPIO_Write(GPIO_ID_RST_QAM2, GPIO_LOW);
			else
				CH_DI_Print(0,("DP10023_SetPowerState[%d] Error Wrong Channel Id! \n",nChannelId));

			CH_DI_Print(2, ("------------- standby mode[%d] -------------\n",nChannelId));
			break;

		case CH_POWER_ON:
		default:
			DP10023_ResetGpio( nChannelId );

			CH_DI_Print(2, ("------------- Power on mode[%d] -------------\n",nChannelId));
			break;
	}

	return DI_CH_OK;
}

int DP10023_EnableTsOutput ( int nChannelId )
{
	CH_UNUSED(nChannelId);

#if 0
	HUINT8 ucVal;
	int nUnitId;

	nUnitId = GetDeviceUnitId( nChannelId );

	Drv_Ch_I2C_DemodRead_CAB(nUnitId, TDA_10023_SB_INTPS, &ucVal, 1);

	ucVal &= (~0x02);// 0x2c [1] TRIP : If 1, tri-stated, else active.

	Drv_Ch_I2C_DemodWrite_CAB(nUnitId, TDA_10023_SB_INTPS, &ucVal, 1);
#endif
	return DI_CH_OK;
}



int DP10023_DisableTsOutput ( int nChannelId )
{
	CH_UNUSED(nChannelId);

#if 0
	HUINT8 ucVal;
	int nUnitId;

	nUnitId = GetDeviceUnitId( nChannelId );

	Drv_Ch_I2C_DemodRead_CAB(nUnitId, TDA_10023_SB_INTPS, &ucVal, 1);

	ucVal |= 0x02;// 0x2c [1] TRIP : If 1, tri-stated, else active.

	Drv_Ch_I2C_DemodWrite_CAB(nUnitId, TDA_10023_SB_INTPS, &ucVal, 1);
#endif

	return DI_CH_OK;

}
/*#############################################################################################################*/
#else
#if 0
/*******************************************************************************
* function: Drv_Ch_I2C_DemodRead
* input:
*	- nChannelId : 버스 번호
* output: No
* return: No
*******************************************************************************/
 int Drv_Ch_I2C_DemodRead(int nUnitId, HUINT8 ucDevAddress,HUINT8 ucRegAddr, HUINT8 *pucBuffer, HUINT32 nNumberToRead)
{
	int nResult = DI_CH_OK;
	DRV_Error nRet = DRV_OK;
	HUINT16	ucDevAddr = TDA10023_DEMOD_ADDRESS;
	HUINT32	ucI2CBus = 2;

	CH_UNUSED(nUnitId);

	if(ucDevAddress == TDA10023_JQAM_ADDRESS)
		ucDevAddr = TDA10023_JQAM_ADDRESS;
	else
		ucDevAddr = TDA10023_DEMOD_ADDRESS;

	nRet = DRV_I2c_ReadA8( ucI2CBus, ucDevAddr, ucRegAddr, pucBuffer, nNumberToRead );
	if ( nRet != DRV_OK )
	{
		CH_PrintErr("DemodWrite]Reg Address Error!");
		nResult = DI_CH_ERR;
	}
	else
	{
		//CH_DI_Print("[0x%x] ucValue[0x%x]\n", ucDevAddr, *pucBuffer);
	}

	return nResult;

}

/*******************************************************************************
* function: Drv_Ch_I2C_DemodWrite
* input:
*	- nChannelId : 버스 번호
* output: No
* return: No
*******************************************************************************/
 int Drv_Ch_I2C_DemodWrite(int nUnitId,  HUINT8 ucDevAddress, HUINT8 ucRegAddr, HUINT8 *pucBuffer, HUINT32 nNumberToWrite)
{
	int nResult = DI_CH_OK;
	DRV_Error nRet = DRV_OK;

	HUINT16	ucDevAddr = TDA10023_DEMOD_ADDRESS;
	HUINT32	ucI2CBus = 2;

	CH_UNUSED(nUnitId);

	if(ucDevAddress == TDA10023_JQAM_ADDRESS)
		ucDevAddr = TDA10023_JQAM_ADDRESS;
	else
		ucDevAddr = TDA10023_DEMOD_ADDRESS;

	//CH_DI_Print("[0x%x] , RegAddr[0x%x]\n", ucDevAddr, ucRegAddr );

	nRet = DRV_I2c_WriteA8(ucI2CBus, ucDevAddr, ucRegAddr, pucBuffer, nNumberToWrite);
	if ( nRet != DRV_OK )
	{
		CH_PrintErr("DemodWrite]Reg Address Error!");
		nResult = DI_CH_ERR;
	}
	else
	{
		//CH_DI_Print(" Success \n");
	}

	return nResult;

}


/*******************************************************************************
* function: Drv_Ch_I2C_TunerWrite
* input:
*	- nChannelId : 버스 번호
* output: No
* return: No
*******************************************************************************/
static int Drv_Ch_I2C_TunerWrite(int nUnitId,  HUINT8 ucRegAddr, HUINT8 *pucBuffer, HUINT32 nNumberToWrite)
{
	int nResult = DI_CH_OK;
	DRV_Error nRet = DRV_OK;

	HUINT32	ucI2CBus = 2;
//	int i=0;

	CH_UNUSED(nUnitId);

	if(ucRegAddr != TD1686_TUNER_ADDRESS)
	{
		/* Reg Address Error */
		CH_PrintErr (" Reg Address Error!n");
	}/*
	else
	{
		for (i=0; i<(int)nNumberToWrite; i++)
			CH_DI_Print("[0x%x] \n", *(pucBuffer+i) );
	}*/

	nRet = DRV_I2c_WriteNoAddr(ucI2CBus, TD1686_TUNER_ADDRESS, pucBuffer, nNumberToWrite);
	if ( nRet != DRV_OK )
	{
		CH_PrintErr(" Error[0x%x]", nRet);
		nResult = DI_CH_ERR;
	}
	else
	{
		//CH_DI_Print(" \n");
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
{
	int nResult = DI_CH_OK;
	DRV_Error nRet = DRV_OK;
	HUINT16	ucDevAddr = TD1686_TUNER_ADDRESS;
	HUINT32	ucI2CBus = 2;

	CH_UNUSED(nUnitId);

	if(ucRegAddr != TD1686_TUNER_ADDRESS)
	{
		/* Reg Address Error */
		CH_PrintErr (" Reg Address Error!n");
	}

	nRet = DRV_I2c_ReadNoAddr( ucI2CBus, ucDevAddr, pucBuffer, nNumberToRead );
	if ( nRet != DRV_OK )
	{
		CH_PrintErr(" Error[0x%x]", nRet);
		nResult = DI_CH_ERR;
	}
	else
	{
		//CH_DI_Print(" [0x%x] \r\n", *pucBuffer);
	}

	return nResult;
}
#endif

static HUINT8 Set_QAM_Tuner(int nChannelId, HUINT32 freq)
{
	HUINT8	CB = 0x00;
	HUINT8	PLL_ReadVal;

	HUINT8	tunerData[6];
	HUINT8	tunerRead;

	float		calcFreq;
	int 		calcBuff;

	int		RSA=0, RSB=0, CP=0, T2=0, T1=0, T0=0, OS=0;
	HUINT32	T_AB, T_TEMP;

	CH_DI_Print(2, ("Set_QAM_Tuner[%d]:: freq = %d \n", nChannelId, freq));

	 /* Samsung DNV22CX */
	_ucTunerWait = 60; // justin 80;

	CH_DI_Print (6, ("SetTuner:: Selected tuner is SAMSUNG DNV22CX \n"));

	if(freq <731000000)
	{
		if(freq < 153000000)
		{
			CB = 0xfe;	/* CP = 125uA */
			CP=1; T2=1; T1=1; T0=1; OS=0;
		}
		else if((freq >= 153000000) && (freq < 173000000))
		{
			CB = 0xb6;	/* CP = 250uA */
			CP=0; T2=1; T1=1; T0=0; OS=0;
		}
		else if((freq >= 173000000) && (freq < 207000000))
		{
			CB = 0xfe;	/* CP = 125uA */
			CP=1; T2=1; T1=1; T0=1; OS=0;
		}
		else
		{
			CB = 0xb6;	/* CP = 250uA */
			CP=0; T2=1; T1=1; T0=0; OS=0;
		}
	}
	else
	{
		CB = 0xb6;	/* CP = 250uA */ 		/* LPP Tuner  731MHz이상에서 QAM uncorrected error 발생 CP 650uA -> 250uA 변경  */
		CP=0; T2=1; T1=1; T0=0; OS=0;

	}

	// QAM (Step_Size = 62.5)
	calcFreq = (freq/62500) + (44000000/62500);

	calcBuff = (int)calcFreq;

	RSA=1, RSB=1;

	/* Divider Byte1 */
	tunerData[0] = (calcBuff >> 8) & 0x007f;

	/* Divider Byte2 */
	tunerData[1] = (calcBuff) & 0x00ff;

	/* Control Byte1 */
	tunerData[2] = CB;

	/* Band Switch Byte */
	if(freq <= 169000000)
	{
		tunerData[3] = 0x01; /* Band Switch Byte : BS1 */
		CH_DI_Print(5,("SetTuner:: BS1  tunerData[3] = %x\n", tunerData[3]));
	}
	else if(freq <= 469000000)
	{
		tunerData[3] = 0x02; /* Band Switch Byte : BS2 */
		CH_DI_Print(5, ("SetTuner:: BS2  tunerData[3] = %x\n", tunerData[3]));
	}
	else if(freq <= 810000000)
	{
		tunerData[3] = 0x10; /* Band Switch Byte : BS3 */
		CH_DI_Print(5, ("SetTuner:: BS3  tunerData[3] = %x\n", tunerData[3]));
	}
	else
	{
		CH_DI_Print(5,( "SetTuner:: ERROR : Out of frequency range for tuner\n"));
	}

	// CONTROL BYTE (AB)
	T_AB = 0x30;

	// FOR making CB byte to WRITING Auxiliary Byte   For making T2,T1,T0 = 0, 1, 1
	 T_TEMP = 0x80 | CP<<6 | 0<<5 | 1<<4 | 1<<3 | RSA<<2 | RSB<<1 | OS;


	tunerData[4] = (T_TEMP&0xdf)|0x18;      // save input T_BB 8bit data into dataout[5], For making T2,T1,T0 = 0, 1, 1
	tunerData[5] = T_AB&0xff;			     // save input T_AB 8bit data into dataout[6]

	CH_DI_Print(3, (" Set_QAM_Tuner[%d]:: [0]=0x%x, [1]=0x%x, [2]=0x%x, [3]=0x%x, [4]=0x%x, [5]=0x%x \n",
								nChannelId,tunerData[0],tunerData[1],tunerData[2],tunerData[3], tunerData[4], tunerData[5]));

	/* tuner write */
	Drv_Ch_I2C_TunerWrite(nChannelId, S22CX_TUNER_ADDRESS, &tunerData[0], 6);

	/* wait for the tuner settling time */
	VK_TASK_Sleep(_ucTunerWait);

	/* check if the PLL in tuner is locked */
	Drv_Ch_I2C_TunerRead(nChannelId, S22CX_TUNER_ADDRESS, &tunerRead, 1);

	PLL_ReadVal = (tunerRead & 0x40)? 1:0;

	if (!PLL_ReadVal)
	{
		CH_DI_Print(1, ("[Set_QAM_Tuner] PLL in the tuner UNLOCK!\n"));
	}

	return PLL_ReadVal;
}

static int DP10023_SetTuner(int nChannelId)
{
	HUINT8 Tnr_StatusByte = 0, Tnr_WriteNo = 0;

	/* activate the Demodulator and set the basic QAM configuration
	   like constellation, input data format and spectral inversion */
	ActivateDemodulator(nChannelId);

	/* set the tuner's PLL register */
	while( (Tnr_WriteNo < NUMBER_I2C_TRIAL) && !Tnr_StatusByte )
	{
		Tnr_StatusByte = Set_QAM_Tuner(nChannelId, gTDA10023ChInfo[nChannelId].frequency);
		Tnr_WriteNo++;
	}

	if(!Tnr_StatusByte)
	{
		CH_DI_Print (1, (" TDA10023_SetTuner[%d]:: Tuner Lock Fail!\n", nChannelId ));
		return DI_CH_ERR;
	}
	else
	{
		CH_DI_Print (1, (" TDA10023_SetTuner[%d]:: Tuner Lock Ok!\n", nChannelId));
		return DI_CH_OK;
	}
}

/* make demodulator idle at the STAND-BY mode */
/* Called by Drv_Tuner_Standby() at channe.c  */

static void DeactivateDemodulator(int nChannelId)
{
	CH_UNUSED(nChannelId);

#ifndef JC5000N
		HUINT8 val;
		CH_DI_Print (3, ("DeactivateDemodulator[%d]:: Power-Down mode\n", nChannelId));

		/* set STDBY bit of CONF to one */
		val = 0x80 | _ConfReg;
		Drv_Ch_I2C_DemodWrite(nChannelId,  SB_CONF, &val, 1);

		/* ADC stand-by mode */
		val = 0xae; // justin 0x33;
		Drv_Ch_I2C_DemodWrite(nChannelId,  TDA10023_DEMOD_ADDRESS, SB_ADC, &val, 1);
#endif
}


/* wake up the demodulator and set the basic QAM configuration
   like constellation, input data format and spectral inversion */
static void ActivateDemodulator(int nChannelId)
{
	CH_UNUSED(nChannelId);

#ifndef JC5000N
	char    qam_name[5];
	HUINT8 	val;
	CH_DI_Print(3, ("ActivateDemodulator[%d]:: Activation of TDA10023\n", nChannelId));

   	switch (gTDA10023ChInfo[nChannelId].qamMode)
   	{
        case 1 : //16-QAM
			VK_strcpy(qam_name, "16");
        	_ConfReg    = 0x63;
			break;
        case 2 : //32-QAM
			VK_strcpy(qam_name, "32");
        	_ConfReg    = 0x67;
			break;
        case 3 : //64-QAM
			VK_strcpy(qam_name, "64");
        	_ConfReg    =  0x2b; // justin 040329  0x2b; // justin 0x6b;
			break;
        case 4 : //128-QAM
			VK_strcpy(qam_name, "128");
        	_ConfReg    = 0x6f;
			break;
		case 5 : //256-QAM
			VK_strcpy(qam_name, "256");
        	_ConfReg    = 0x73;
			break;
		default:
			VK_strcpy(qam_name, "64");
        	_ConfReg    = 0x2b;
			break;
	}

	/* set the CONF register */
	val = _ConfReg;
	Drv_Ch_I2C_DemodWrite(nChannelId,  TDA10023_DEMOD_ADDRESS, SB_CONF, &val, 1);
#endif
}

/* _________________ QAM demodulator specific functions __________________*/
// TDA10023-specific functions
/***************************************************************************
Performs QAM receiver locking trial based on Nyquist filter of the fixed
gain, then if the receiver is locked, it returns TRUE.
****************************************************************************/
HUINT8 fast_lock_TDA(int nChannelId)
{
	HUINT8	val1,  val;
	int i = 0, nDelay = 0, nRetryCount=0;

   	gTDA10023ChInfo[nChannelId].strength = 0;
   	gTDA10023ChInfo[nChannelId].quality = 0;
	gTDA10023ChInfo[nChannelId].snr = 0;
	jcom_error = 0;  // 화면 깨지는지 보기위서 넣었음.

	nDelay = 100;
	nRetryCount = 3;

	for(i=0; i< nRetryCount; i++)
	{
		val1 =	0x93; // phillips R_AGCONF_SLOW; // R_AGCONF_FAST; // justin_4 R_AGCONF_SLOW;
		Drv_Ch_I2C_DemodWrite(nChannelId,  TDA10023_DEMOD_ADDRESS, SB_AGCCONF, &val1, 1);

		VK_TASK_Sleep( (HUINT16)(no_to_msec(nChannelId, WAIT_AGCLOCK)) );	// 76ms

		if( check_agc_lock(nChannelId) )
		{
			CH_DI_Print (1, ("fast_lock_TDA[%d]::  Detected QAM signal\n", nChannelId));

		}
		else
		{
			CH_DI_Print (1, ("fast_lock_TDA[%d]::  Failed to detect QAM signal\n", nChannelId));
			return FALSE;
		}

		VK_TASK_Sleep(nDelay);

		val1 = 0x9b; // 011 011 scs_nerima phillps 0xbf; // 0x9b; // justin_4 0x93; // justin 0x13; // justin 0x03;
		Drv_Ch_I2C_DemodWrite(nChannelId, TDA10023_DEMOD_ADDRESS, SB_AGCCONF, &val1, 1);

		/* stop fine AGC using the center tap and use all coeffients of equalizer */
		val = R_EQCONF2ND; // CTADAPT bit=0
		Drv_Ch_I2C_DemodWrite(nChannelId, TDA10023_DEMOD_ADDRESS,  SB_EQCONF2, &val, 1);

		Drv_Ch_I2C_DemodRead(nChannelId, TDA10023_DEMOD_ADDRESS, SB_SYNC, &val1, 1);

		if((val1&0x4f) == 0x0f)
		{
			CH_DI_Print (1, ("fast_lock_TDA[%d]:: Locked... Num = %d\n", nChannelId, i));
			AdjustCarrierRecovBW(nChannelId);
			return TRUE;
		}
		else
		{
			clearDemod(nChannelId);
		}
	}

	CH_DI_Print(1, ("fast_lock_TDA[%d]:: Lock Fail! SB_SYNC[0x11] = %x, Num = %d\n", nChannelId, val1&0x4f, i));

       return FALSE;
}

/* Switch on: Host I2C bus is connected to the tuner for tuner programming */
void enable_tuner_iic(int nChannelId)
{
	HUINT8 val;

	/* Tuner data output on TEST pins                 */
	/* GPIO pin is used as interupt line (Active Low) */
	val = R_TEST | 0x80; // set BYPIIC bit
	Drv_Ch_I2C_DemodWrite(nChannelId,  TDA10023_DEMOD_ADDRESS, SB_TEST, &val, 1);
}

/* Switch off: Host I2C bus is disconnnected from the tuner for reduction of the phase noise */
void disable_tuner_iic(int nChannelId)
{
	HUINT8 val;

	/* No output on TEST pins */
	val = R_TEST; // reset BYPIIC bit
	Drv_Ch_I2C_DemodWrite(nChannelId,  TDA10023_DEMOD_ADDRESS, SB_TEST, &val, 1);
}

/********************************************************************
 This soft reset does not affect the internal I2C register contents.
 There are two bits for reset operation: CLB(b7, LSB) & CLB2(b6)
 CLB is automatically return to 1 after 5 XIN clock period, but
 CLB2 does not automatically return to 1.
*********************************************************************/
static void clearDemod(int nChannelId)
{
	HUINT8 	val;

	switch (gTDA10023ChInfo[nChannelId].qamMode)
	{
	    case 1 : //16-QAM
	    	_ConfReg    = 0x63; break;
	    case 2 : //32-QAM
	    	_ConfReg    = 0x67; break;
	    case 3 : //64-QAM
	    	_ConfReg    =  0x2b; break; // justin 0x6b;	break;
	    case 4 : //128-QAM
	    	_ConfReg    = 0x6f;	break;
		case 5 : //256-QAM
	    	_ConfReg    = 0x73;	break;
		default:
	    	_ConfReg    = 0x6b;	break;
	}

	/* set the value of the CONF register */
	val = _ConfReg;
	//Drv_Ch_I2C_DemodWrite(SB_CONF, &val, 1);

	/* soft-reset the QAM logic */
	/* set the CLR bit to zero. CLB signal will automatically return to 1 after 5 XIN period */
	val = _ConfReg & 0xfe;
	Drv_Ch_I2C_DemodWrite(nChannelId,  TDA10023_DEMOD_ADDRESS, SB_CONF, &val, 1);
}

/********************************************************************
Configure QAM demodulator:
	- clear 7-bit packet error counter
	- set the constellation size & spectrum polarity
	- configure the equalizer (mode and step size)
	- set AGCREF, LOCKTHR, MSETH and AREF value
	- set the carrier and symbol recovery loop parameter
	- set AAF and DF
*********************************************************************/
void InitQAMdemod(int nChannelId)
{
	HUINT8	QAMreg[0x10];
	HUINT8	val, uc_val;
	HUINT8	nDECfilt;
	HUINT32	swdyn, swstep, swlength;
	HINT32	lDeltaF;

   	swlength = 0;
	swstep = 145;
	swdyn = 0;

//	OSA_Print("InitQAMdemod() for TDA10023\n");

	val = 0x21; // 0x0; // phillips 0x7f;  // DVB Mode select

	Drv_Ch_I2C_DemodWrite(nChannelId, TDA10023_DEMOD_ADDRESS,  SB_RESET, &val, 1);

    switch (gTDA10023ChInfo[nChannelId].qamMode)
    {
        case 1 : //16-QAM
        	QAMreg[0x01] = 0x50; //AGCREF is same at both SW versions
        	QAMreg[0x05] = 0x6e; //LOCKTHR = 0x87 @ v1.01
        	QAMreg[0x08] = 0x6e; //MSETH = 0xa2 @ v1.01
        	QAMreg[0x09] = 0x91; //AREF is same at both SW versions

       		break;
        case 2 : //32-QAM
        	QAMreg[0x01] = 0x50; //0x8c
        	QAMreg[0x05] = 0x4b; //0x64 @ v1.01
        	QAMreg[0x08] = 0x4b; //0x74 @ v1.01
        	QAMreg[0x09] = 0x96;

        	break;
        case 3 : //64-QAM
       	QAMreg[0x01] = qam_val; // 0x50;  // TDA10023 datasheet value 0x50 justin
   		QAMreg[0x05] = 0x46; // TDA10023 datasheet value 0x46 justin
       	QAMreg[0x08] = 0x43; // 0x43; // TDA10023 datasheet value 0x46 justin
        	QAMreg[0x09] = 0x6a; // 0x6a; // TDA10023 datasheet value 0x46 justin
 		CH_DI_Print (4, ("InitQAMdemod:: 64-QAM initial setup starts\n"));
        	break;
        case 4 : //128-QAM
           	QAMreg[0x01] = 0x78; //although perf. is best at -65 dBm or lower, it is a little degraded at mnorma level (higher than -65 dBm)
       	QAMreg[0x05] = 0x30; // @ v1.01
        	QAMreg[0x08] = 0x30; // @ v1.01
        	QAMreg[0x09] = 0x7e;

        	break;
        case 5 : //256-QAM
        	//QAMreg[0x01] = 0x50; //Philips source code
        	QAMreg[0x01] = 0x5c; //better than 0x50, from datasheet
        	QAMreg[0x05] = 0x23; //0x26 @ v1.01
		QAMreg[0x08] = 0x23; //0x23 @ v1.01
        	QAMreg[0x09] = 0x6b;
		CH_DI_Print (4, ("InitQAMdemod:: 256-QAM initial setup starts\n"));

        	break;

	default: //64-QAM
       	QAMreg[0x01] = qam_val; // 0x50; // TDA10023 datasheet value 0x50 justin
   		QAMreg[0x05] = 0x46; // TDA10023 datasheet value 0x46 justin
       	QAMreg[0x08] = 0x43; // TDA10023 datasheet value 0x46 justin
        	QAMreg[0x09] = 0x6a; // TDA10023 datasheet value 0x46 justin
 		CH_DI_Print (4, ("InitQAMdemod:: 64-QAM initial setup starts. default\n"));
			break;
	}

	/* agcconf: use the small AGC time constant at the initial phase and */
	/* then enlarge after Euqualizer turn-on  */
	QAMreg[0x02] =   0x93; // phillips R_AGCONF_FAST; //0x01

	/* clkconf: CLK_C coef is only programmed. Other params will be set later */
	_ClkconfReg  =  0x1a; // kansai=0x0a; // kansai R_CLKCONF_WIDE = 0x1a;
	QAMreg[0x03] =  0x1a; // kansai= 0x0a; // 0x0a;  //R_CLKCONF_WIDE = 0x1a;

	/* carconf: Set the carrier acquisition range wide regardless of the symbol rate  */
	/* If the SR is greater than 3 MHz, the range will be reduced after locking       */
	if(gTDA10023ChInfo[nChannelId].symbolRate >= 3000)
		_CarconfReg = cr_val;  // R_CARCONF_HIGH_SR_ACQ;//0x0a
	else if(gTDA10023ChInfo[nChannelId].symbolRate >= 1500)
		_CarconfReg = R_CARCONF_MID_SR_ACQ;//0x0c
	else
		_CarconfReg = R_CARCONF_LOW_SR_ACQ;//0x0d
	QAMreg[0x04] = _CarconfReg;

	/* eqconf: TDA10021 EQ is enabled contrast to VES1820x */
	QAMreg[0x06] = R_EQCONF_ON; //0x77: Center Tap = 10th tap, ENADAPT=ENEQUAL=DFE=1
	//QAMreg[0x06] = 0x47; //Center Tap = 6th tap, ENADAPT=ENEQUAL=DFE=1

	/* eqstep */
	QAMreg[0x07] = eq_val; // 0x19; //  R_EQSTEP; //0001 1010 => better than 0x1a @ long ghost (delay >= 335 ns)
	//QAMreg[0x07] = 0x1c; //0001 1100 => worse than 0x1a @ short ghost (delay <= 35 ns)
	//QAMreg[0x07] = 0x1b; //0001 1011 => worse than 0x1a @ short ghost (delay <= 35 ns)
	//QAMreg[0x07] = 0x19; //0001 1001 => better than 0x1a @ short ghost (delay <= 145 ns)
	// Conclution: Acquisition step size = xxxx x010

	//QAMreg[0x07] = 0x12; // 0001 0010 => similar to 0x1a @ short ghost (delay 1,000 ns)
	// Conclution: Tracking step size = xx01 1xxx

	/* registers programming from reg[1] to reg[9] */
	Drv_Ch_I2C_DemodWrite(nChannelId,  TDA10023_DEMOD_ADDRESS, SB_AGCREF, &QAMreg[1], 9);

	/* eqconf2: At first CTADAPT bit must to set to one
	   in order to achive the fine AGC operation       */
	uc_val = R_EQCONF2ND | 0x08; //no tap leakage & CTADAPT=1
//	uc_val = R_EQCONF2ND;
	Drv_Ch_I2C_DemodWrite(nChannelId,  TDA10023_DEMOD_ADDRESS, SB_EQCONF2, &uc_val, 1);

	/* clkconf's NDEC[1..0] bits: set the number of the decimation */
	nDECfilt = set_DecimateFilt(nChannelId);   // 실제로  계산 하면 항상 0 justin

	/* bdr_msb ~ bdr_inv: set the symbol rate and its inverse */
	set_baud_rate(nChannelId, nDECfilt); 		/* the symbol rate and its inverse */

	/* gain: set the gain of Nyquist filter and the anti-alias filter */
	select_AntialiasFilt(nChannelId); 		/* G_NYQ=2.0, G_AAF=1.0 */

    /* intp: set TS stream clock */
   	set_TS_clock_rate(nChannelId);

	/* rsconf: BER depth = 1.0E-7 */
	val = R_RSCONF3; // set CLB_UNC bit
	Drv_Ch_I2C_DemodWrite(nChannelId,  TDA10023_DEMOD_ADDRESS, SB_RSCONF, &val, 1);

	/* adc: internal ADC used */
	/* BYSONG, 20070723, CS = 10(2) */
	uc_val =  0xc8; // 0x88;
	Drv_Ch_I2C_DemodWrite(nChannelId,  TDA10023_DEMOD_ADDRESS, SB_ADC, &uc_val, 1);

	/* itsel: active LOW interrupt control */
	uc_val = 0x00; // justin 0x40; // interrupt disabled and output level is "1" always
	Drv_Ch_I2C_DemodWrite(nChannelId,  TDA10023_DEMOD_ADDRESS, SB_ITSEL, &uc_val, 1);

	/* pwmref: meaningful if IF AGC is inactive */
	uc_val = 0x00; // justin 0x80;
	Drv_Ch_I2C_DemodWrite(nChannelId, TDA10023_DEMOD_ADDRESS, SB_PWMREF, &uc_val, 1);

	/* mdiv, ndiv, pll: PLL related setting */
	QAMreg[0x00] = 0x2b; // 0x23;  //0x2b; /* PLL1 */
	QAMreg[0x01] = 0x44;	/* PLL2 */
	QAMreg[0x02] = 0x0c;  /* PLL3 */
	Drv_Ch_I2C_DemodWrite(nChannelId,  TDA10023_DEMOD_ADDRESS, SB_MDIV, &QAMreg[0x00], 3);

	/* pola2/ints: controls the second output stream */
	/* check bytes are not provided and all of serial
	   TS output pins will be in tri-state           */
	val = 0xa1; // INTS1 serial setting
	Drv_Ch_I2C_DemodWrite(nChannelId,  TDA10023_DEMOD_ADDRESS, SB_INTS1, &val, 1);

	/* sweep: SWEEP control */
   	QAMreg[0x00] = 0x86; // justin  추후 테스트 해볼것 SWEEP  current 2.34%  0xcb; // best, 11.7% of SR, better than 0xc9

	/* in order to calculate the time to acquire the carrier lock */
    switch ( (QAMreg[0x00]&0x70) >> 4)
    {
        case 0 :
        	swdyn = 2340; 	break;
        case 1 :
        	swdyn = 4680; 	break;
        case 2 :
        	swdyn = 7000; 	break;
        case 3 :
        	swdyn = 9375; 	break;
        case 4 :
        	swdyn = 11700; 	break;
        case 5 :
        	swdyn = 14060; 	break;
        case 6 :
        	swdyn = 16400; 	break;
        case 7 :
        	swdyn = 18750; 	break;
	}

    switch ( (QAMreg[0x00]&0x0c) >> 2 )
    {
        case 0 :
        	swstep = 145; 	break;
        case 1 :
        	swstep = 290; 	break;
        case 2 :
        	swstep = 580; 	break;
        case 3 :
        	swstep = 1170; 	break;
	}

    switch (QAMreg[0x00]&0x03)
    {
        case 0 :
        	swlength = 4096; 	break;
        case 1 :
        	swlength = 8192; 	break;
        case 2 :
        	swlength = 16384; 	break;
        case 3 :
        	swlength = 32768; 	break;
	}

	/* Factor 2 is multiplied for sweep of the range: -swdyn ~ +swdyn */
	_SbCarWait = 2*swlength*swdyn/swstep;
//	OSA_Print(" No. of symbol for carrier sweep = %d\n", _SbCarWait);

	/* agcconf2: If 5V tolerant configuration is selected, *
	 * the receiver is more sensitive to AGC linearity     *
	 * problem due to a pull-up resister used.             */
	QAMreg[0x01] = 0x30; // justin 0xb0; // justin 0x08; // both AGC outputs are configured as normal output type

	Drv_Ch_I2C_DemodWrite(nChannelId,  TDA10023_DEMOD_ADDRESS, SB_SWEEP, &QAMreg[0x00], 2);

	/* RF AGC control */
	if(gucCABTopOn == 1)
	{
		// 새로운 saw filter적용시 0xff
		QAMreg[0x00] = gucRfMaxC; //0xbf; // d0; // 0x75; /* RF AGC max threshold value */
		QAMreg[0x01] = gucRfMinC; /* RF AGC min threshold value */
	}
	else
	{
		QAMreg[0x00] = 0xff;
		QAMreg[0x01] = 0x00;
	}

	if (IFFREQ*1.6*1000 == SYSCLK )
	{
		QAMreg[0x02] =0x00;
		QAMreg[0x03] =0x80;
	}
	else
	{
#if(1)
		/* 20070723, FSAMPLING = 1 */
		lDeltaF = (HINT32)(((8.0*(IFFREQ - SYSCLK))/SYSCLK)*100);
		lDeltaF = (-4096.0*(500 + lDeltaF))/100;

		QAMreg[0x02] = (HUINT8)lDeltaF;
		QAMreg[0x03] = (HUINT8)(((lDeltaF>>8) & 0x7F) | 0x80);

		CH_DI_Print(4, ("InitQAMdemod %x, %x\n", QAMreg[0x02],QAMreg[0x03]));
#else
		/* 20070712, FSAMPLING = 0 */
		lDeltaF = (HINT32)(((4.0*(IFFREQ - (SYSCLK)))/(SYSCLK))*100);
		lDeltaF = (-8192.0*(100 + lDeltaF))/100;

		QAMreg[0x02] = (HUINT8)lDeltaF;
		QAMreg[0x03] = (HUINT8)(((lDeltaF>>8) & 0x7F) | 0x80);
		CH_DI_Print("InitQAMdemod %x, %x\n", QAMreg[0x02],QAMreg[0x03]);
#endif
	}

	Drv_Ch_I2C_DemodWrite(nChannelId,  TDA10023_DEMOD_ADDRESS, SB_TUNMAX, &QAMreg[0x00], 4);

	/* IF AGC control: IF_MIN determines TOP of the tuner */
	if(gucCABTopOn == 1)
	{
		QAMreg[0x00] = gucIfMaxC; // 0xff; // IF AGC max threshold value
		QAMreg[0x01] = gucIfMinC;
	}
	else
	{
		QAMreg[0x00] = 0xff; // IF AGC max threshold value
		QAMreg[0x01] = 0x96;
	}

	Drv_Ch_I2C_DemodWrite(nChannelId,  TDA10023_DEMOD_ADDRESS, SB_IFMAX, &QAMreg[0x00], 2);

	val =  0x30;
	Drv_Ch_I2C_DemodWrite(nChannelId,  TDA10023_DEMOD_ADDRESS, SB_AGCCONF2, &val, 1);

	val=  0xa3;
	Drv_Ch_I2C_DemodWrite(nChannelId,  TDA10023_DEMOD_ADDRESS, 0xbe, &val, 1);

	val= 0x01;
	Drv_Ch_I2C_DemodWrite(nChannelId,  TDA10023_DEMOD_ADDRESS, 0x13, &val, 1);
}

/* Reduce the carrier recovery loop's acquisition range according to SR */
/* Affects performance of the reciever according to the selected value  */
void AdjustCarrierRecovBW(int nChannelId)
{
	HUINT8 val;

#if 1 //v1.x. better performance than v2.0
	/* set the carrier recovery loop's acquisition range and loop BW */
	if(gTDA10023ChInfo[nChannelId].symbolRate >= 3000)
		val = R_CARCONF_HIGH_SR_TRK; //0x0A(acquision phase) => 0x02 (tracking phase)
	else if(gTDA10023ChInfo[nChannelId].symbolRate >= 1500)
		val = R_CARCONF_MID_SR_TRK; //0x0C => 0x05
	else
		val = R_CARCONF_LOW_SR_TRK; //0x0D => 0x09
#else //v2.0
	/* change the carrier recovery loop bandwidth according to to symbol rate */
	val = 0x05;
	if (gTDA10023ChInfo.symbolRate < 3000)
		val = 0x02;
	if ( (gTDA10023ChInfo.symbolRate < 1750) && (gTDA10023ChInfo.qamMode < 4) )
		val = 0x0a;
#endif

	Drv_Ch_I2C_DemodWrite(nChannelId,  TDA10023_DEMOD_ADDRESS, SB_CARCONF, &val, 1);
	_CarconfReg	= val;
}

#if 0
/* if power of the center taps of the equalizer is threshold value,
   it means that the equalizer input signal level is weak.
   ======> increase the gain of the Nyquist filter gain             */
HUINT8 AdjustGain(int nChannelId)
{
	HUINT32 	CtapPower;
	HUINT8 	ctap, ctap_I, ctap_Q;

	signed char	pCoef_I[2], pCoef_Q[2];
	signed long CentralCoef_I, CentralCoef_Q;

	while (_NYQgain < NYQ_AUTOGAIN_MAX)
	{
		/* determine the position of the central tap */
		ctap = (R_EQCONF_ON>>4) + 2;
		//OSA_Print("EQ center tap location and index: %d, ", ctap_I);

		/* determine the register index of the central tap */
		ctap_I = SB_REQCO_H0 + (ctap-1)*2;
		ctap_Q = SB_IEQCO_H0 + (ctap-1)*2;
		//OSA_Print("%d\r\n", ctap_I);

		/* read the real and imaginary part of the central coef of the equalizer */
		Drv_Ch_I2C_DemodRead(nChannelId,  TDA10023_DEMOD_ADDRESS, ctap_I, (HUINT8*)&pCoef_I[0], 2);
		Drv_Ch_I2C_DemodRead(nChannelId,  TDA10023_DEMOD_ADDRESS, ctap_Q, (HUINT8*)&pCoef_Q[0], 2);

		CentralCoef_I = (HUINT32)(pCoef_I[0] << 3 | pCoef_I[1] >> 5); // 11 bits size coefficient
		CentralCoef_Q = (HUINT32)(pCoef_Q[0] << 3 | pCoef_Q[1] >> 5); // 11 bits size coefficient

		/* convert minus vale into plus value */
		if(CentralCoef_I & 0x400)
			CentralCoef_I = -CentralCoef_I;	/* sign extention */
		if(CentralCoef_Q & 0x400)
			CentralCoef_Q = -CentralCoef_Q;	/* sign extention */

		/* CentralCoef_I amplitude calculation */
		CtapPower = CentralCoef_I + CentralCoef_Q;

		/* check if amp of the central tap coefficient exceeds the threshold value (700) */
		if ( CtapPower > 700 )
		{
			/* try next gain */
			gain_updown_NYQfilt(nChannelId, NYQ_GAIN_UP);
			CH_DI_Print("AdjustGain:: EQ Ctap power=%d: NYQ filt gain increased to %d\n",CtapPower,_NYQgain);

			/* wait for filter settlement */
			VK_TASK_Sleep( (HUINT16)(no_to_msec(nChannelId, WAIT_GNYQLOCK)) );
		}
		else
			return TRUE;
	}
	return FALSE;
}

/* Gain control: updown=0 (Gain down), updown=1 (Gain Up) */
void gain_updown_NYQfilt(int nChannelId, HUINT8 updown)
{
	HUINT8 NewGain, val;

	if (updown == NYQ_GAIN_UP)
		NewGain = _NYQgain+1;
	else if(_NYQgain > 0)
		NewGain = _NYQgain-1;
	else
	{
		NewGain = _NYQgain;
		CH_DI_Print("gain_updown_NYQfilt:: Minimun gain reached\n");
	}

	val = (_GainReg&0x8f) | (NewGain<<4); // justin val = (_GainReg&0x1f) | (NewGain<<5);

	/* Update the gain */
	_NYQgain = NewGain;
	_GainReg = val;
	CH_DI_Print ("gain_updown_NYQfilt:: New gain = %x\n", val);
	Drv_Ch_I2C_DemodWrite(nChannelId, TDA10023_DEMOD_ADDRESS,  SB_GAIN, &val, 1);
}
#endif


/**************************************************************************
 The number of decimation is determined by reference frequency and symbol
 rate. Symbol rate is variable according to payload data of a channel.
****************************************************************************/
HUINT8 calc_no_decimation(int nChannelId)
{
	HUINT32 SymbolRate;

	SymbolRate = gTDA10023ChInfo[nChannelId].symbolRate;

	if( SymbolRate > SYSCLK/16 )
		return 0;
	else if( SymbolRate > SYSCLK/32)
		return 1;
	else if( SymbolRate > SYSCLK/64)
		return 2;
	else if( SymbolRate >  SYSCLK/128)
		return 3;
	else
	{
		CH_DI_Print (1, ("calc_no_decimation:: SR of %d KHz is out of range: LOW\r\n",gTDA10023ChInfo[nChannelId].symbolRate));
		return 0;
	}
}

/***************************************************************
  Control the decimators in the baud rate recovery loop:
 - Set the number of decimation
 - Set the initial loop BW as large. Later the BW will be reduced.
****************************************************************/
HUINT8 set_DecimateFilt(int nChannelId)
{
	HUINT8	no_decimate, clkconf_val;

	/* Get the number of decimation */
	no_decimate = calc_no_decimation(nChannelId);
	clkconf_val = no_decimate << 6;

	/* Updates the the baud rate recovery loop register value */
	clkconf_val = _ClkconfReg | clkconf_val;
	_ClkconfReg = clkconf_val;
	Drv_Ch_I2C_DemodWrite(nChannelId,  TDA10023_DEMOD_ADDRESS, SB_CLKCONF, &clkconf_val, 1);

	/* the number of DEC will be used when the baud rate and its inverse are set */
	return no_decimate;
}

/* set the baud rate and its inverse according *
 * to symbol rate and reference frequency      */
void set_baud_rate(int nChannelId, HUINT8 ndec)
{
	HUINT32 	BaudRate, InvBaudRate;
	HUINT32	temp;
	HUINT8	val[4];
	HUINT8	no_decimate;

	no_decimate = ndec;

#if 0 // justin
	/* set the baud rate */
	/* 16,777,216 = 2**24, SYSCLK = 57800 = 7225 * 8 */
	/* = 2**24 / 57800 = 2**21 / 7225 = 2097152/7225 */
	/* = SR*16777216uL*2**no_decimate/SYSCLK         */
#endif

	temp = SYSCLK/8;
	BaudRate = ((((2097152.0/temp)*gTDA10023ChInfo[nChannelId].symbolRate))*pow_positive(2,no_decimate) + 0.5); // justin add 0.5 for Nearest integer vale

	val[0] = BaudRate & 0xff;  // justin 0x83
	val[1] = (BaudRate>>8) & 0xff; // justin 0xc0
	val[2] = (BaudRate>>16) & 0xff; // justin 0x12

	/* set the baud rate inverse */
	InvBaudRate = ((16.0*SYSCLK) / ( gTDA10023ChInfo[nChannelId].symbolRate *pow_positive(2, no_decimate)) + 0.5);  // justin add 0.5 for Nearest integer vale

	val[3] = ( InvBaudRate > 255)? 255:InvBaudRate;

//	OSA_Print("### %x, %x, %x, %x\n", val[0], val[1], val[2], val[3]); // justin_2

	Drv_Ch_I2C_DemodWrite(nChannelId,  TDA10023_DEMOD_ADDRESS, SB_BDR_LSB, &val[0], 4);
}


/* math function which calulate power of exponential */
HUINT16 pow_positive(HUINT16 mantissa, HUINT8 exponent)
{
	HUINT8	i;
	HUINT16	result = 1;

	if(mantissa == 0)
	{
		CH_DI_Print(1, ("pow_positive:: pow_positive(mantissa=0) error!  \n"));
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


/* select which anti-aliasing filter will be used */
void select_AntialiasFilt(int nChannelId)
{
	HUINT8	falias, gain_val;
	HUINT32 	SymbolRate;
	HUINT8	gain2_val;

	SymbolRate =  gTDA10023ChInfo[nChannelId].symbolRate / 10;

	if( SymbolRate > SYSCLK / 80)
	{
		CH_DI_Print (1, ("select_AntialiasFilt:: Error! SR=%d  is too HIGH\r\n", gTDA10023ChInfo[nChannelId].symbolRate));
		falias = 0;
	}
	else if( SymbolRate > SYSCLK/123 )
		falias = 0;
	else if( SymbolRate > SYSCLK/160 )
		falias = 1;
	else if( SymbolRate > SYSCLK/246 )
		falias = 0;
	else if( SymbolRate > SYSCLK/320 )
		falias = 1;
	else if( SymbolRate > SYSCLK/492 )
		falias = 0;
	else if( SymbolRate > SYSCLK/640 )
		falias = 1;
	else if( SymbolRate > SYSCLK/984 )
		falias = 0;
	else if( SymbolRate > SYSCLK/1280 )
		falias = 1;
	else
	{
		CH_DI_Print (1, ("select_AntialiasFilt:: Error! SR=%d  is too LOW\r\n", gTDA10023ChInfo[nChannelId].symbolRate));
		falias = 0;
	}

	gain2_val = ( (falias<<7) | (NYQ_ROLLOFF & 0x03) | ( (NYQ_GAAF << 4 ) & 0x70) ) ;

	CH_DI_Print (4, ("select_AntialiasFilt:: gain2 = %x\n", gain2_val)); // justin

	Drv_Ch_I2C_DemodWrite(nChannelId,  TDA10023_DEMOD_ADDRESS, SB_GAIN2, &gain2_val, 1);

	gain_val = 0x95; // kansai=0x94 0x95; INIT_GAIN;  // justin

	_GainReg = gain_val;
	_NYQgain = (gain_val>>4) & 0x07;      /* Nyquist filter gain factor: 0 - 7 */  // justin 0xa6

//	OSA_Print("### SB_GAIN = %x\n", gain_val); // justin_2
	CH_DI_Print (4, ("select_AntialiasFilt:: NYQgain = %d, 0xE = %x\n", _NYQgain, gain_val));

	Drv_Ch_I2C_DemodWrite(nChannelId, TDA10023_DEMOD_ADDRESS,  SB_GAIN, &gain_val, 1);
}

/* calculate the sleep time in msec according to the symbol rate */
HUINT16 no_to_msec(int nChannelId, HUINT32 symbols)
{
	HUINT32	time_in_msec;
	HUINT16	t;

	/* Convert "number of symbols" to time in msec */
	time_in_msec  = symbols;
	time_in_msec += gTDA10023ChInfo[nChannelId].symbolRate/2;
	time_in_msec /= gTDA10023ChInfo[nChannelId].symbolRate; 	// msec

	t = (HUINT16) time_in_msec;

	if(gTDA10023ChInfo[nChannelId].symbolRate < 3000) t = 2 * t;

	//CH_DI_Print("select_AntialiasFilt:: no_to_msec = %d\n", t);

	return	t;
}

#if 0
/*******************************************************************
 If two DVB sync patterns(0x47 & 0xB8) are found, FEC is locked, but
 if only MPEG2 sync pattern(0x47) is found, returns FALSE (NODVB).
********************************************************************/
HUINT8 check_fec_lock_TDA(int nChannelId)
{
	HUINT8	val1,val2;

	/* Get the information of MPEG frame sync from the FEC core */
	Drv_Ch_I2C_DemodRead(nChannelId,  TDA10023_DEMOD_ADDRESS, SB_SYNC, &val1, 1);
	CH_DI_Print (2, ("check_fec_lock_TDA:: SB_SYNC = 0x%x\n", val1&0xff));
	val2 = val1 & 0xff; /* mask all bits but NODVB & FSYNC bit */

	/* check if both MPEG frame and carrier were synchronized */
	if( (val2&0x0f) ==0x0f )
		return TRUE;
	else if( ((val1&0x40)==0x40) )
	{
		CH_DI_Print (2, ("check_fec_lock_TDA:: No DVB packet\n"));
		return FALSE;
	}
	else
		return FALSE;
}
#endif

/* check if two AGC output are maximum. If then,  */
/* it means there is no signal or too weak signal */
HUINT8 check_agc_lock(int nChannelId)
{
	HUINT8	val1, val2;

	/* Read the value of AGC loops */
	Drv_Ch_I2C_DemodRead(nChannelId, TDA10023_DEMOD_ADDRESS,  SB_VAGC, &val1, 1);
	Drv_Ch_I2C_DemodRead(nChannelId, TDA10023_DEMOD_ADDRESS, SB_VAGC2, &val2, 1);
	CH_DI_Print (2, ("check_agc_lock[%d]:: RF_AGC = 0x%x (%d), IF_AGC= 0x%x (%d)\n", nChannelId, val1,val1,val2, val2));

	if ( ((val1==0xff) && (val2==0xff)) )
		return FALSE;
	else
		return TRUE;
}

#if 0
/* calculate the carrier offset. unit is kHz */
short get_carrier_offset(int nChannelId)
{
	HUINT8	val;
	long	car_off_hz;
	short	carrier_offset;

	/* VAFC is in 2's complement */
	Drv_Ch_I2C_DemodRead(nChannelId, TDA10023_DEMOD_ADDRESS, SB_VAFC, &val, 1);
	car_off_hz = (long) val;

	/* check the sign bit */
	if(val & 0x80)
		car_off_hz |=0xffffff00; //sign extension

	/* calculate the value in kHz */
	car_off_hz *= (long) ((1000*gTDA10023ChInfo[nChannelId].symbolRate+256) / 512);	// TDA10021, unit=Hz
	carrier_offset = (short) (car_off_hz/1000); // unit: kHz

	return carrier_offset;
}

short get_carrier_offset_new(int nChannelId)
{
	char	val;
	long	lAFC;
	short	sAFC;

	/* Read frequency correction */
	Drv_Ch_I2C_DemodRead(nChannelId,  TDA10023_DEMOD_ADDRESS, SB_VAFC, (HUINT8 *) &val, 1);

	/* signed value if needed -> 8 bits */
	lAFC = (long)val;
	if (val & 0x80)
		lAFC |= 0xFFFFFF00;

	/* calculate the value in kHz */
	lAFC *= (long) ( (gTDA10023ChInfo[nChannelId].symbolRate+256) / 512); // TDA10021

	/* invert the AFC */
    lAFC = -lAFC;

    sAFC = lAFC;

	/* return AFC */
	return sAFC;
}

/* Informs symbol clock offset in ppm */
short get_clock_offset(int nChannelId)
{
	HUINT8	mode;
	char	val;
	long	clk_off_int;
	short	clk_offset;

	mode = _ClkconfReg & 0x08;
	Drv_Ch_I2C_DemodRead(nChannelId, TDA10023_DEMOD_ADDRESS, SB_CKOFFSET, (HUINT8 *)&val, 1);

	/* check the sign bit */
	clk_off_int = (long) val;
	if(val & 0x80)
		clk_off_int |=0xffffff00; //sign extension

	clk_off_int *= 1000000;
	clk_off_int /= 262144; // 262144 = 2**18

	if( mode == 0x00) // DYN == 0
		clk_off_int /= 2;

	clk_offset = (short) (clk_off_int); // unit is ppm

	return clk_offset;
}
#endif

/***************************************************************************
  If FEC is locked due to spectral inversion, return 1, else return 0.
****************************************************************************/
#if 0
HUINT8 HandleSpectralInversion_TDA(int nChannelId)
{
	HUINT8	val;


	/* spectrall inversion 을 위해 추가 했음   */
	Drv_Ch_I2C_DemodRead(nChannelId,  TDA10023_DEMOD_ADDRESS, SB_CARCONF, &val, 1);

	_CarconfReg = val;

   	 val = val ^ 0x20;
	Drv_Ch_I2C_DemodWrite(nChannelId, TDA10023_DEMOD_ADDRESS,  SB_CARCONF, &val, 1);

	/* Wait for 80K symbol duration till FEC core locks to the frame sync */
	VK_TASK_Sleep( (HUINT16)(no_to_msec(nChannelId, WAIT_FRMLOCK) ));

	if( check_fec_lock_TDA(nChannelId) )
	{
	   	_CarconfReg = val; // justin  _ConfReg = val;
	   	gTDA10023ChInfo[nChannelId].SpectralInverse = 1;
		return TRUE;
	}
	else
	{
	   	gTDA10023ChInfo[nChannelId].SpectralInverse = 0;

		/* restore the original value */
	   	val  = _CarconfReg; // justin _ConfReg;
		Drv_Ch_I2C_DemodWrite(nChannelId,  TDA10023_DEMOD_ADDRESS, SB_CARCONF, &val, 1);
		return FALSE;
	}
}
#endif

/****************************************************************
Set TS output clock rate at the output mode A or B. The mode B
clock has no jitter and a duty cycle of 50%. But output TS clock
(OCLK) rate must be greater than M*Fsymbol/8.
*****************************************************************/
void set_TS_clock_rate(int nChannelId)
{
	HUINT8 val;

 	/***** Serial Mode *******/

	/* 0x12: FEL/UNCP	POINTP	MFS/PSYNCP	PUNCORP 	PDENP	POCLKP */
	/*			1		0		1		0		0		0		0		1 */
	/* 0x20 : DIVP[7-4] 					SWAPP	MSBFIRSTP	INTPSEL[1-0] */
	/*			0		0		0		1		0		1		1		1 */
	/* 0x2C : SWAPS MSBFIRSTS EXTPSYP EXTPSYS POINT47P POINT47S TRIP	TRIS */
	/*			0(df)	1(df)	1		0(df)	0		0(df)	0		1(df) */

	val = 0xa1;
	Drv_Ch_I2C_DemodWrite(nChannelId, TDA10023_DEMOD_ADDRESS, SB_INTP1, &val, 1);

	val = 0x17;
	Drv_Ch_I2C_DemodWrite(nChannelId, TDA10023_DEMOD_ADDRESS, SB_INTP2, &val, 1);

	val = 0x61;
	Drv_Ch_I2C_DemodWrite(nChannelId, TDA10023_DEMOD_ADDRESS, SB_CONTROL, &val, 1);

#if 0
	/* Parallel mode A: TS parallel with jittered clock */
	/* Parallel B mode 선택 */
		val = 0xa1;
	//	val = 0xe1;
		Drv_Ch_I2C_DemodWrite(nChannelId, TDA10023_DEMOD_ADDRESS, SB_INTP1, &val, 1);

	//	val = 0x05; // no-jittered clock, OCLK = SYSCLK / 1 = 70MHz, it does not run
	//	val = 0x15; // no-jittered clock, OCLK = SYSCLK / 2 = 35MHz, it does not run
	//	val = 0x25; // no-jittered clock, OCLK = SYSCLK / 3 = 23.33MHz, it run
	//	val = 0x35; // no-jittered clock, OCLK = SYSCLK / 4 = 17.5MHz
	//	val = 0x45; // no-jittered clock, OCLK = SYSCLK / 5 = 14MHz
	//	val = 0x55; // no-jittered clock, OCLK = SYSCLK / 6 = 11.66MHz
		val = 0x95; // no-jittered clock, OCLK = SYSCLK / 10 = 7MHz
	//	val = 0x75; // no-jittered clock, OCLK = SYSCLK / 8 = 8.75MHz
	//	val = 0x85; // no-jittered clock, OCLK = SYSCLK / 9 = 7.77MHz
	// 	val = 0x95; // no-jittered clock, OCLK = SYSCLK / 10 = 7MHz
	//	val = 0xa5; // no-jittered clock, OCLK = SYSCLK / 11 = 6.36MHz
	//	val = 0xb5; // no-jittered clock, OCLK = SYSCLK / 12 = 5.83MHz
	//	val = 0xc5; // no-jittered clock, OCLK = SYSCLK / 13 = 5.38MHz
	//	val = 0xd5; // no-jittered clock, OCLK = SYSCLK / 14 = 5MHz
	//	val = 0xe5; // no-jittered clock, OCLK = SYSCLK / 15 = 4.66MHz
	//	val = 0xf5; // no-jittered clock, OCLK = SYSCLK / 16 = 4.375MHz
		Drv_Ch_I2C_DemodWrite(nChannelId,  TDA10023_DEMOD_ADDRESS, SB_INTP2, &val, 1);
#endif

	/* Parallel mode A or B according to the average byte clock rate.
	   TS parallel with the regular clock in case of Parallel mode B.
	   If the average clock rate is low, select the parallel mode A. */

}

/********************************************************************
for signal strength display
*********************************************************************/
HUINT16 SNR_Info_TDA(int nChannelId)
{
	HUINT8	val1,val2;
	HUINT32	val, agcv;
	float fSNR, fMSE;
	HUINT32 uCounter;

	/* Read the number of saturations at ADC output */
	Drv_Ch_I2C_DemodRead(nChannelId,  TDA10023_DEMOD_ADDRESS, SB_VAGC, &val1, 1);	// read RF AGC
	Drv_Ch_I2C_DemodRead(nChannelId,  TDA10023_DEMOD_ADDRESS, SB_VAGC2, &val2, 1);	// read IF AGC

	/* Meaure RF AGC register value range over RF level of -25 ~ +15 dBmV: 156 ~ 255 */
	/* Meaure IF AGC register value range over RF level of -25 ~ +15 dBmV: 88 ~ 135 */
	/* Use RF AGC in case of high to medium signal level */

#if 1
	val = val1 + val2;
	if (val < 195) val = 195; // upper value limit
	val -=195;

	if (val > 195 ) val = 195; // lower value limit

	/* 255 max. value for OAK & OPENTV application */
	agcv = 255 - 255*val/195;

	/* 100 max. value for application */
	//agcv = 100 - 100*val/195;
#else
	agcv = (val1+val2)/2;
	agcv = 255-agcv;
#endif

	val = (HUINT8) agcv;

#if 0
	Printf("val1 : %d, val2 : %d, val : %d\n", val1, val2, val);
#endif

	fMSE = (float)0;
	for (uCounter = 0; uCounter < 10; uCounter++)
	{
		// average value of the MSE on 10 values
		Drv_Ch_I2C_DemodRead(nChannelId,  TDA10023_DEMOD_ADDRESS, SB_MSE, &val1, 1);
		//	CH_DI_Print ("SNR_Info_TDA:: MSE = %x\n", val); // justin
		fMSE += (float)val1;
	}

	fMSE /= (float)uCounter;
	fSNR = (float)((210.0/(0.40*fMSE + 5.0)) + 12.5);
	val = (HUINT16) (fSNR*10.0);

	CH_DI_Print (3, ("SNR_Info_TDA:: SNR = %d\n", val)); // justin

	return val;
}

/* Calcaulte the estimated SNR value */
HINT16 Find_RFIndex(HUINT8 agcval, HUINT8 *AgcTable, HINT16 *offset);
HINT16 Find_IFIndex(HUINT8 agcval, HUINT8 *AgcTable, HINT16 *offset);
HINT16 Find_SignalLevel(HINT16 rfPwrIndex, HINT16 ifPwrIndex,  HINT16 rfoffset, HINT16 ifoffset);


HINT16 Find_RFIndex(HUINT8 agcval, HUINT8 *AgcTable, HINT16 *offset)
{

	HINT16 i, DsPwrIndex=0;


	for (i=0; i<level_num; i++)
	{
		if ( (agcval <= AgcTable[i]) )
		{
			DsPwrIndex = i;
			if (agcval == AgcTable[i])
			{
				*offset = 0;
			}
			else
			{
				if( i == 0)
                            {
                                	*offset = 0;
                            }
                            else
                            {
					*offset = 5;
					 // *offset = ((agcval - AgcTable[i]) *10) / (AgcTable[i-1] - AgcTable[i]);
                            }

			}
			break;
		}
	}

	if(i == level_num)
	{
		DsPwrIndex = level_num;
		*offset = 0;
	}

	CH_DI_Print(5, ( "Find_RFIndex: Table[%d] = %d, Agc = %d, DsPwrIndex = %d\n", i, AgcTable[i], agcval, DsPwrIndex));


	return DsPwrIndex;
}


HINT16 Find_IFIndex(HUINT8 agcval, HUINT8 *AgcTable, HINT16 *offset)
{

	HINT16 i, DsPwrIndex=0;


	for (i=0; i<level_num; i++)
	{

		if ( (agcval <= AgcTable[i]) )
		{
			DsPwrIndex = i;
			if (agcval == AgcTable[i])
			{
				*offset = 0;
			}
			else
			{
				if( i == 0)
                            {
                                	*offset = 0;
                            }
                            else
                            {
					*offset = 5;
					 // *offset = ((agcval - AgcTable[i]) *10) / (AgcTable[i-1] - AgcTable[i]);

                            }

			}
			break;
		}
	}

	if(i == level_num)
	{
		DsPwrIndex = level_num;
		*offset = 0;
	}

	CH_DI_Print(5, ( "Find_IFIndex: Table[%d] = %d, Agc = %d, DsPwrIndex = %d\n", i, AgcTable[i], agcval, DsPwrIndex));

	return DsPwrIndex;
}


HINT16 Find_SignalLevel(HINT16 rfPwrIndex, HINT16 ifPwrIndex,  HINT16 rfoffset, HINT16 ifoffset)
{

	HINT16	strength;
	HINT16  level, offset;

	strength = 0;

	if( ifPwrIndex <= rfPwrIndex )
	{
		level = rfPwrIndex;
		offset = rfoffset;
	}
	else
	{
		level = ifPwrIndex;
		offset = ifoffset;
	}

	if(level == 0)
	{
		strength = 250;
	}
	else if(level == level_num)
	{
		strength = -160;
	}
	else
	{
		strength = (downlevel[level]*10)+offset-600;      /* dBuV -> dBmV    */
	}


	return strength;



}



HINT16 Strength_Info_TDA(int nChannelId)

{

	HUINT8	rfagc, ifagc;
	HINT16  strength=0;
	HINT16 	rfoffset=0, ifoffset=0;
	HINT16	levelRfIndex, levelIfIndex;



	/* Read the number of saturations at ADC output */
	Drv_Ch_I2C_DemodRead(nChannelId, TDA10023_DEMOD_ADDRESS, SB_VAGC, &rfagc, 1);	// read RF AGC
	Drv_Ch_I2C_DemodRead(nChannelId, TDA10023_DEMOD_ADDRESS, SB_VAGC2, &ifagc, 1);	// read IF AGC
	/* Meaure RF AGC register value range over RF level of -25 ~ +15 dBmV: 156 ~ 255 */
	/* Meaure IF AGC register value range over RF level of -25 ~ +15 dBmV: 88 ~ 135 */
	/* Use RF AGC in case of high to medium signal level */
	CH_DI_Print(5, ("Strength_Info_TDA: frq = %d, rfagc = %d, ifagc = %d\n", gTDA10023ChInfo[nChannelId].frequency, rfagc, ifagc));

	if (gTDA10023ChInfo[nChannelId].frequency < 170000)
	{
		levelRfIndex = Find_RFIndex( rfagc, RfAgcGain1, &rfoffset);
		levelIfIndex = Find_IFIndex( ifagc, IfAgcGain1, &ifoffset);
		strength = Find_SignalLevel(levelRfIndex, levelIfIndex, rfoffset, ifoffset);

	}
	else if(gTDA10023ChInfo[nChannelId].frequency < 182000)
	{
		levelRfIndex = Find_RFIndex( rfagc, RfAgcGain2, &rfoffset);
		levelIfIndex = Find_IFIndex( ifagc, IfAgcGain2, &ifoffset);
		strength = Find_SignalLevel(levelRfIndex, levelIfIndex, rfoffset, ifoffset);

	}
	else if (gTDA10023ChInfo[nChannelId].frequency < 302000)
	{
		levelRfIndex = Find_RFIndex( rfagc, RfAgcGain3, &rfoffset);
		levelIfIndex = Find_IFIndex( ifagc, IfAgcGain3, &ifoffset);
		strength = Find_SignalLevel(levelRfIndex, levelIfIndex, rfoffset, ifoffset);

	}
	else if (gTDA10023ChInfo[nChannelId].frequency < 472000)
	{
		levelRfIndex = Find_RFIndex( rfagc, RfAgcGain4, &rfoffset);
		levelIfIndex = Find_IFIndex( ifagc, IfAgcGain4, &ifoffset);
		strength = Find_SignalLevel(levelRfIndex, levelIfIndex, rfoffset, ifoffset);

	}
	else if (gTDA10023ChInfo[nChannelId].frequency < 628000)
	{

		levelRfIndex = Find_RFIndex( rfagc, RfAgcGain5, &rfoffset);
		levelIfIndex = Find_IFIndex( ifagc, IfAgcGain5, &ifoffset);
		strength = Find_SignalLevel(levelRfIndex, levelIfIndex, rfoffset, ifoffset);


	}
	else if (gTDA10023ChInfo[nChannelId].frequency < 718000)
	{
		if ((gTDA10023ChInfo[nChannelId].frequency>= 671000) && (gTDA10023ChInfo[nChannelId].frequency <= 707000) )
		{
			levelRfIndex = Find_RFIndex( rfagc, RfAgcGain8, &rfoffset);
			levelIfIndex = Find_IFIndex( ifagc, IfAgcGain8, &ifoffset);
			strength = Find_SignalLevel(levelRfIndex, levelIfIndex, rfoffset, ifoffset);

		}
		else
		{
			levelRfIndex = Find_RFIndex( rfagc, RfAgcGain6, &rfoffset);
			levelIfIndex = Find_IFIndex( ifagc, IfAgcGain6, &ifoffset);
			strength = Find_SignalLevel(levelRfIndex, levelIfIndex, rfoffset, ifoffset);
		}

	}
	else
	{
		levelRfIndex = Find_RFIndex( rfagc, RfAgcGain7, &rfoffset);
		levelIfIndex = Find_IFIndex( ifagc, IfAgcGain7, &ifoffset);
		strength = Find_SignalLevel(levelRfIndex, levelIfIndex, rfoffset, ifoffset);

	}

	CH_DI_Print(5,("Strength_Info_TDA: strength = %d\n", strength));
	return strength;
}



/**************************************************************************
- BER_Info_TDA(): Convert the real BER to IRD's signal quality factor.
- BER: Estimated BER at the Reed-Solomon recoder input
- 2.0E-4 before Reed-Solomon encoding is similar to 1.0e-11
  after Reed-Solomon decoding
 	O: FEC unlock
	1: 1.0e-2 <= BER @ pre Reed-Solomon decoder < 5.0e-2
	2: 7.5e-3 <= BER @ pre Reed-Solomon decoder < 1.0e-2
	3: 5.0e-3 <= BER @ pre Reed-Solomon decoder < 7.5e-3
	4: 2.5e-3 <= BER @ pre Reed-Solomon decoder < 5.0e-3
	5: 1.0e-3 <= BER @ pre Reed-Solomon decoder < 2.5e-3
	6: 7.5e-4 <= BER @ pre Reed-Solomon decoder < 1.0e-4
	7: 5.0e-4 <= BER @ pre Reed-Solomon decoder < 7.5e-4
	8: 2.5e-4 <= BER @ pre Reed-Solomon decoder < 5.0e-4
	9: 1.0e-4 <= BER @ pre Reed-Solomon decoder < 2.5e-4
   10:           BER @ pre Reed-Solomon decoder < 1.0e-4
***************************************************************************/
HUINT32 BER_Info_TDA(int nChannelId)
{
	HUINT8	Ecnt[3], BerDepth;
	HUINT32	error_count;

	char buffer[14];

	/* Get the number of bit errors */
	Drv_Ch_I2C_DemodRead(nChannelId, TDA10023_DEMOD_ADDRESS, SB_BER_LSB, &Ecnt[0], 3);

	error_count = ((((unsigned long)Ecnt[2])<<16 )| (((unsigned long)Ecnt[1])<<8 )| (((unsigned long)Ecnt[0])));

	Drv_Ch_I2C_DemodRead(nChannelId, TDA10023_DEMOD_ADDRESS, SB_RSCONF, &BerDepth, 1);

	CH_DI_Print (5, ("BER_Info_TDA:: BerDepth = %x, erro_count = %d\n", BerDepth, error_count));

    switch(BerDepth & 0xc0)
    {
		case 0x00:  // 1,00E+05
			error_count *= 1000;
			break;
		case 0x40:  // 1,00E+06
			error_count *= 100;
			break;
		case 0x80:  // 1,00E+07
			error_count *= 10;
			break;
		case 0xc0:  // 1,00E+08
			break;
    }

	VK_snprintf(buffer, sizeof(buffer), "%d", error_count);

	CH_DI_Print (4, ("BER  : %c.%c%c E-%02d\n", buffer[0], buffer[1], buffer[2], 8-VK_strlen(buffer)+1));
	CH_DI_Print (3, ("BER_Info_TDA::  Error_count = %d, accum = %d \n", error_count,accum_error));

/* 원래는 2^20 7자리가 최대 표시. */

	return(error_count);
}


#if 0
/**************************************************************************
- BER_Info_PostRSD_TDA(): Convert the real BER to IRD's signal quality factor.
- BER factor based on Reed-Solomon decoder output
 	O: FEC unlock
	1: 1.0e-3 <= BER @ post Reed-Solomon decoder
	2: 1.0e-4 <= BER @ post Reed-Solomon decoder  < 1.0e-3
	3: 1.0e-5 <= BER @ post Reed-Solomon decoder  < 1.0e-4
	4: 1.0e-6 <= BER @ post Reed-Solomon decoder  < 1.0e-5
	5: 1.0e-7 <= BER @ post Reed-Solomon decoder  < 1.0e-6
	6: 1.0e-8 <= BER @ post Reed-Solomon decoder  < 1.0e-7
	7: 1.0e-9 <= BER @ post Reed-Solomon decoder  < 1.0e-8
	8: 1.0e-10<= BER @ post Reed-Solomon decoder  < 1.0e-9
	9: 1.0e-11<= BER @ post Reed-Solomon decoder  < 1.0e-10
   10:           BER @ post Reed-Solomon decoder  < 1.0e-11
***************************************************************************/
HUINT8 BER_Info_PostRSD(int nChannelId)
{
	HUINT8	val;
	HUINT32	error_bits, post_RSD_ber_1e11;

	post_RSD_ber_1e11 = 0;

	/* Get the BER scale factor */
	Drv_Ch_I2C_DemodRead(nChannelId, TDA10023_DEMOD_ADDRESS, SB_RSCONF, &val, 1);
	val = val & 0xc0;

	/* Numer of the error bits */
	error_bits = measure_BER_TDA(nChannelId);

	switch (val)
	{
		case BER_DEPTH5: //ber_cnt_width=1.25e5
			post_RSD_ber_1e11 = error_bits*1000000;
			post_RSD_ber_1e11 *=4;
			post_RSD_ber_1e11 /=5;
			break;
		case BER_DEPTH6: //ber_cnt_width=1.0e6
			post_RSD_ber_1e11 = error_bits*100000;
			break;
		case BER_DEPTH7: //ber_cnt_width=1.0e7
			post_RSD_ber_1e11 = error_bits*10000;
			break;
		case BER_DEPTH8: //ber_cnt_width=1.0e8
			post_RSD_ber_1e11 = error_bits*1000;
			break;
		default:
			break;
	}

	if (post_RSD_ber_1e11 >= 100000000)	val = 1;
	else if (post_RSD_ber_1e11 >= 10000000)		val = 2;
	else if (post_RSD_ber_1e11 >= 1000000)		val = 3;
	else if (post_RSD_ber_1e11 >= 100000)		val = 4;
	else if (post_RSD_ber_1e11 >= 10000)		val = 5;
	else if (post_RSD_ber_1e11 >= 1000)			val = 6;
	else if (post_RSD_ber_1e11 >= 100)			val = 7;
	else if (post_RSD_ber_1e11 >= 10)			val = 8;
	else if (post_RSD_ber_1e11 >= 1)			val = 9;
	else 										val = 10;

	return (val );
}
#endif

/* Increase the BER counter width to 1.0e+8 */
void AdaptBerWindow(int nChannelId)
{
	HUINT8 	val;
	HUINT8	bitsPerBaud;
	HUINT32	ElapsedTime;
	HUINT32	ElapsedBits;

	bitsPerBaud = 6;

    /* Define bit per symbol according to QAM mode */
    switch (gTDA10023ChInfo[nChannelId].qamMode) {
    	case 1 : /* 16-QAM */
    		bitsPerBaud = 4;   break;
    	case 2 : /* 32-QAM */
    		bitsPerBaud = 5;   break;
    	case 3 : /* 64-QAM */
    		bitsPerBaud = 6;   break;
    	case 4 : /* 128-QAM */
    		bitsPerBaud = 7;   break;
    	case 5 : /* 256-QAM */
    		bitsPerBaud = 8;   break;
    }

	/* BER measurement period is equal to GET_DATA_INTERVAL */
	ElapsedTime = 2*GET_DATA_INTERVAL; // in msec

	/* calculate the number of the elapsed bits for GET_DATA_INTERVAL */
	ElapsedBits = bitsPerBaud*ElapsedTime*gTDA10023ChInfo[nChannelId].symbolRate;
//	OSA_Print("### ElapsedBits = %d\n", ElapsedBits);
	/* select the BER counter width equal or more than the elapsed bits */
	if     (ElapsedBits > 10000000)
		val = R_RSCONF4;
	else if(ElapsedBits > 1000000)
		val = R_RSCONF3;
	else if(ElapsedBits > 125000)
		val = R_RSCONF2;
	else
		val = R_RSCONF1;


	Drv_Ch_I2C_DemodWrite(nChannelId, TDA10023_DEMOD_ADDRESS,  SB_RSCONF, &val, 1);


}


/* additional QAM channel information */
#if 0 // justin #ifndef _FGDL_
void get_loop_inform_TDA(void)
{
	short caroff,clkoff;
	HUINT8 val1, val2;

	caroff = get_carrier_offset(); // kHz
	clkoff = get_clock_offset();   // ppm

   	gTDA10023ChInfo.CarrierOffset = caroff;
   	gTDA10023ChInfo.ClockOffset = clkoff;
}
#endif



void JQAM_Mode(int nChannelId, HUINT8 mode)
{
	HUINT8	val;

	CH_DI_Print (5, ("JQAM_Mode::  JQAM Mode = %x\n", mode));

	val = mode;

	Drv_Ch_I2C_DemodWrite(nChannelId, TDA10023_JQAM_ADDRESS, TDA10023_JQAM_MODE_IND, &val, 1);

}

void JQAM_TsId(int nChannelId, HINT32 tsID, HINT32 onID)
{
	HUINT8	val[2];

	CH_DI_Print (4, ("JQAM_TsId:: TS ID = %x, onid = %x\n", tsID, onID));
	val[0] = tsID >> 8;
	val[1] = tsID;

	Drv_Ch_I2C_DemodWrite(nChannelId, TDA10023_JQAM_ADDRESS,   TDA10023_REQ_TS_IDH_IND, &val[0], 2);

	val[0] = onID >> 8;
	val[1] = onID;

	Drv_Ch_I2C_DemodWrite(nChannelId, TDA10023_JQAM_ADDRESS,   TDA10023_REQ_ON_IDH_IND, &val[0], 2);

}

void JQAM_Status(int nChannelId)
{
	HUINT8 val;


	Drv_Ch_I2C_DemodRead(nChannelId, TDA10023_JQAM_ADDRESS,   TDA10023_REG5_IND, &val, 1);

	CH_DI_Print (4, ("JQAM_Status[%d]:: JQAM status = %x\n",  nChannelId, val));


	if ( tsmf_flag == 1)
	{
		if (test_gobad == 1)
			val = 0x2f;   // good
		else
			val = 0x1;  // bad
	}

	if((val & 0x40) == 0x40)
	{
		CH_DI_Print (4, ("JQAM_Status:: EMERGENCY!\n"));
		gTDA10023ChInfo[nChannelId].emergencyFlag = 1;
	}
	else
	{
		gTDA10023ChInfo[nChannelId].emergencyFlag = 0;
	}

	if((val & 0x30) == 0x00)
	{
		CH_DI_Print (4, ("JQAM_Status[%d]:: Signal Good. [0x%02x]\n", nChannelId, (val & 0x30) >> 4));
		gTDA10023ChInfo[nChannelId].tsmfStatus = 0;
	}
	else if((val & 0x30) == 0x10)
	{
		CH_DI_Print (4, ("JQAM_Status[%d]:: Signal Poor. [0x%02x]\n", nChannelId, (val & 0x30) >> 4));
		gTDA10023ChInfo[nChannelId].tsmfStatus = 1;
	}
	else if((val & 0x30) == 0x20)
	{
		CH_DI_Print (4, ("JQAM_Status[%d]:: Signal Bad. [0x%02x]\n", nChannelId, (val & 0x30) >> 4));
		gTDA10023ChInfo[nChannelId].tsmfStatus = 2;
	}
	else if((val & 0x30) == 0x30)
	{
		CH_DI_Print (4, ("JQAM_Status[%d]:: Reserved\n", nChannelId));
		gTDA10023ChInfo[nChannelId].tsmfStatus =3;
	}

	CH_DI_Print (4, ("JQAM_Status[%d]:: VER = %x\n", nChannelId, (val & 0x0e) >> 1));

	if((val & 0x01) == 1)
	{
		CH_DI_Print (4, ("JQAM_Status[%d]:: MLOCK OK\n", nChannelId));
	}
	else
	{
		CH_DI_Print (4, ("JQAM_Status[%d]:: MLOCK Not OK \n", nChannelId));
		gTDA10023ChInfo[nChannelId].tsmfStatus = 0;
		gTDA10023ChInfo[nChannelId].emergencyFlag = 0;

	}

/* 이토츄 테스트시에 Bad signal 이지만 이 비드가 에러로 날라오는 경우 발생. 2005-03-31 */
	if((val & 0x80) == 0x80)
	{
		CH_DI_Print (4, ("JQAM_Status[%d]:: ERROR!\n", nChannelId));
		gTDA10023ChInfo[nChannelId].tsmfStatus = 0;
		gTDA10023ChInfo[nChannelId].emergencyFlag = 0;
	}

}




int JQAM_TsNumber(int nChannelId)
{
	HUINT8 val[2], ts_value, ts_value1;
	int number;
	int i,j;

	number = 0;
	j=0;

	Drv_Ch_I2C_DemodRead(nChannelId, TDA10023_JQAM_ADDRESS,  TDA10023_TS_STATUS_H_IND, &val[0], 2);
	ts_value = val[1];
	ts_value1 = val[0];
	CH_DI_Print (4, ("JQAM_TsNumber:: TS  = 0x%x\n", ts_value));

	for (i = 0; i< 8; i++)
	{

		CH_DI_Print(4, ("JQAM_TsNumber = %d, ts_value=0x%x\n", i,((ts_value >>(1*i) ) & 0xff)));
		if( ((ts_value >>(1*i) ) & 0x01) == 0x01)
		{
			number += 1;
#if 0
			JqamIicRx(TDA10023_TS_ID1H_IND + i*4, &val, 1);
			tsid = val;

			JqamIicRx(TDA10023_TS_ID1H_IND + i*4+1, &val, 1);
			tsid = (tsid << 8) |val ;
			CH_DI_Print ("JQAM_TsNumber:: tsid 2  = %x\n", tsid);
			gTDA10023TsmfInfo.tsID[j] = 	tsid;


			JqamIicRx(TDA10023_TS_ID1H_IND + i*4+2, &val, 1);
			netid = val;

			JqamIicRx(TDA10023_TS_ID1H_IND + i*4+3, &val, 1);
			netid = (netid << 8) |val ;

			CH_DI_Print ("JQAM_TsNumber:: netid  = 0x%x, i = %d, j = %d\n", netid, i, j);

			gTDA10023TsmfInfo.netID[j] = 	netid;

			j++;
#endif
		}

	}


	CH_DI_Print (4, ("JQAM_TsNumber:: TS[0..7] = %d\n", number));


//	JqamIicRx(TDA10023_TS_STATUS_H_IND, &val, 1);
	ts_value = ts_value1;

	for (i= 0; i< 7; i++)
	{

		if( ((ts_value >> (1*i) ) & 0x01) ==  0x01)
		{
			number += 1;
#if 0
			JqamIicRx((TDA10023_TS_ID1H_IND+32) + i*4, &val, 1);
			tsid = val;

			JqamIicRx((TDA10023_TS_ID1H_IND+32) + i*4+1, &val, 1);
			tsid = (tsid << 8) |val ;

			gTDA10023TsmfInfo.tsID[j] = 	tsid;


			JqamIicRx((TDA10023_TS_ID1H_IND+32) + i*4+2, &val, 1);
			netid = val;

			JqamIicRx((TDA10023_TS_ID1H_IND+32) + i*4+3, &val, 1);
			netid = (netid << 8) |val ;

			CH_DI_Print ("JQAM_TsNumber:: netid2  = 0x%x, i = %d, j = %d\n", netid, i, j);

			gTDA10023TsmfInfo.netID[j] = 	netid;

			j++;
#endif
		}

	}

	CH_DI_Print (4, ("JQAM_TsNumber:: TS[0..14] number = %d\n", number));

	gTDA10023TsmfInfo[nChannelId].tsNo = number;

	return number;

}


void JQAM_TSinfo(int nChannelId)
{
	HUINT8 val[4], val1[2], ts_value, ts_value1;
	int number;
	int i,j;
	HINT32 tsid, netid;

	number = 0;
	j=0;

	Drv_Ch_I2C_DemodRead(nChannelId, TDA10023_JQAM_ADDRESS,  TDA10023_TS_STATUS_H_IND, &val1[0], 2);
	ts_value = val1[1];
	ts_value1 = val1[0];

	CH_DI_Print (3, ("JQAM_TsNumber:: TS  = 0x%x\n", ts_value1));

	for (i = 0; i< 8; i++)
	{

		CH_DI_Print(4, ("JQAM_TSinfo:: num = %d, ts_value=0x%x\n", i,((ts_value >>(1*i) ) & 0xff)));
		if( ((ts_value >>(1*i) ) & 0x01) == 0x01)
		{
			number += 1;
#if 1
			Drv_Ch_I2C_DemodRead(nChannelId, TDA10023_JQAM_ADDRESS,  TDA10023_TS_ID1H_IND + i*4, &val[0], 4);
			tsid = (((int)val[0]) << 8) | ((int) val[1]);
			gTDA10023TsmfInfo[nChannelId].tsID[j] = 	tsid;

			netid = (((int)val[2]) << 8) | ((int) val[3]);
			gTDA10023TsmfInfo[nChannelId].netID[j] = netid;

#else
			JqamIicRx(TDA10023_TS_ID1H_IND + i*4, &val, 1);
			tsid = val;

			JqamIicRx(TDA10023_TS_ID1H_IND + i*4+1, &val, 1);
			tsid = (tsid << 8) |val ;
			CH_DI_Print ("JQAM_TsNumber:: tsid 2  = %x\n", tsid);
			gTDA10023TsmfInfo.tsID[j] = 	tsid;


			JqamIicRx(TDA10023_TS_ID1H_IND + i*4+2, &val, 1);
			netid = val;

			JqamIicRx(TDA10023_TS_ID1H_IND + i*4+3, &val, 1);
			netid = (netid << 8) |val ;

			CH_DI_Print ("JQAM_TsNumber:: netid  = 0x%x, i = %d, j = %d\n", netid, i, j);

			gTDA10023TsmfInfo.netID[j] = 	netid;
#endif
			j++;

		}

	}


	CH_DI_Print (3, ("JQAM_TSinfo:: TS[0..7] = %d\n", number));


//	JqamIicRx(TDA10023_TS_STATUS_H_IND, &val1, 1);
	ts_value = ts_value1;

	for (i= 0; i< 7; i++)
	{
		if( ((ts_value >> (1*i) ) & 0x01) ==  0x01)
		{
			number += 1;
#if 1
			Drv_Ch_I2C_DemodRead(nChannelId, TDA10023_JQAM_ADDRESS,  (TDA10023_TS_ID1H_IND+32) + i*4, &val[0], 4);

			tsid = (((int)val[0]) << 8) | ((int) val[1]);
			gTDA10023TsmfInfo[nChannelId].tsID[j] = 	tsid;

			netid = (((int)val[2]) << 8) | ((int) val[3]);
			gTDA10023TsmfInfo[nChannelId].netID[j] = netid;

#else
			JqamIicRx((TDA10023_TS_ID1H_IND+32) + i*4, &val, 1);
			tsid = val;

			JqamIicRx((TDA10023_TS_ID1H_IND+32) + i*4+1, &val, 1);
			tsid = (tsid << 8) |val ;

			gTDA10023TsmfInfo.tsID[j] = 	tsid;


			JqamIicRx((TDA10023_TS_ID1H_IND+32) + i*4+2, &val, 1);
			netid = val;

			JqamIicRx((TDA10023_TS_ID1H_IND+32) + i*4+3, &val, 1);
			netid = (netid << 8) |val ;

			CH_DI_Print ("JQAM_TsNumber:: netid2  = 0x%x, i = %d, j = %d\n", netid, i, j);

			gTDA10023TsmfInfo.netID[j] = 	netid;
#endif
			j++;
		}

	}

	CH_DI_Print (4, ("JQAM_TSinfo:: TS[0..14] = %d\n", number));

	gTDA10023TsmfInfo[nChannelId].tsNo = number;

}


HUINT32 Sig_Unerroed_PKT(int nChannelId)
{
	HUINT8	puBytes[4];
	HUINT32	ok_pkt;


	/* Get the number of unerroed packet  */
	Drv_Ch_I2C_DemodRead(nChannelId, TDA10023_DEMOD_ADDRESS, CPT_TSP_OK1, &puBytes[0], 4);

        ok_pkt = (puBytes[3]<<24) | (puBytes[2]<<16) | (puBytes[1]<<8) | puBytes[0];

	if(nChannelId == 0)
	{
        	CH_DI_Print (5, ("Sig_Unerroed_PKT[%d] :: OK Packet = %d\n", nChannelId, ok_pkt));
	}
	else
	{
		CH_DI_Print (5, ("Sig_Unerroed_PKT[%d] :: OK Packet = %d\n", nChannelId, ok_pkt));
	}

	return ok_pkt;
}


HUINT32 Sig_Corrected_PKT(int nChannelId)
{
	HUINT8	puBytes[4];
	HUINT32	cor_pkt;


	/* Get the number of corrected packet  */
	Drv_Ch_I2C_DemodRead(nChannelId,  TDA10023_DEMOD_ADDRESS, CPT_TSP_COR1, &puBytes[0], 4);

        cor_pkt = (puBytes[3]<<24) | (puBytes[2]<<16) | (puBytes[1]<<8) | puBytes[0];

	if(nChannelId == 0)
	{
       	 CH_DI_Print (5, ("Sig_Corrected_PKT[%d]:: Corrected Packet = %d\n", nChannelId, cor_pkt));
	}
	else
	{
		CH_DI_Print (5, ("Sig_Corrected_PKT[%d]:: Corrected Packet = %d\n", nChannelId, cor_pkt));
	}
	return cor_pkt;
}


HUINT32 Sig_UnCorrected_PKT(int nChannelId)
{
	HUINT8	puBytes[4];
	HUINT32	uncor_pkt;

	/* Get the number of uncorrected packet  */
	Drv_Ch_I2C_DemodRead(nChannelId,  TDA10023_DEMOD_ADDRESS, CPT_TSP_UNCOR1, &puBytes[0], 4);

	uncor_pkt = (puBytes[3]<<24) | (puBytes[2]<<16) | (puBytes[1]<<8) | puBytes[0];

	if(nChannelId == 0)
	{
		CH_DI_Print (3, ("Sig_UnCorrected_PKT[%d]:: Uncorrected Packet = %d\n", nChannelId, uncor_pkt));
	}
	else
	{
		CH_DI_Print (3, ("Sig_UnCorrected_PKT[%d]:: Uncorrected Packet = %d\n", nChannelId, uncor_pkt));
	}

	//if( (show_di_ch > 0 && uncor_pkt> 1) || uncor_pkt  > 30)
	if( uncor_pkt  > 1)
	{
		if(jcom_error != uncor_pkt)
		{
			if(nChannelId == 0)
			{
				CH_DI_Print(0, ("Sig_UnCorrected_PKT[%d]::  UnCorrected Packet   = %d\n", nChannelId, uncor_pkt));
			}
			else
			{
				CH_DI_Print(0, ("Sig_UnCorrected_PKT[%d]::  UnCorrected Packet   = %d\n", nChannelId, uncor_pkt));
			}

			jcom_error = uncor_pkt;
		}
	}

	return uncor_pkt;
}


void InitDemodulator(int nChannelId)
{
	HUINT8 val;

	CH_DI_Print (3, ("InitDemodulator:: Power-Down mode\n"));
#if 1
	/* set STDBY bit of CONF to one */
	val = 0x80 | _ConfReg;
	Drv_Ch_I2C_DemodWrite(nChannelId,  TDA10023_DEMOD_ADDRESS, SB_CONF, &val, 1);

	/* ADC stand-by mode */
	val = 0xae; // justin 0x33;
	Drv_Ch_I2C_DemodWrite(nChannelId,  TDA10023_DEMOD_ADDRESS, SB_ADC, &val, 1);
#endif

//	Drv_Ch_I2C_DemodRead(nChannelId,  SB_RESET, &val, 1);
//	val = 0x80 | val;

//	Drv_Ch_I2C_DemodWrite(nChannelId,  SB_RESET, &val, 1);

	switch (gTDA10023ChInfo[nChannelId].qamMode)
	{
	    case 1 : //16-QAM
	    	_ConfReg    = 0x63; break;
	    case 2 : //32-QAM
	    	_ConfReg    = 0x67; break;
	    case 3 : //64-QAM
	    	_ConfReg    =  0x2b; break; // justin 0x6b;	break;
	    case 4 : //128-QAM
	    	_ConfReg    = 0x6f;	break;
		case 5 : //256-QAM
	    	_ConfReg    = 0x73;	break;
		default:
	    	_ConfReg    = 0x6b;	break;
	}

	/* set the value of the CONF register */
	val = _ConfReg;
	//Drv_Ch_I2C_DemodWrite(SB_CONF, &val, 1);

	/* soft-reset the QAM logic */
	/* set the CLR bit to zero. CLB signal will automatically return to 1 after 5 XIN period */
	val = _ConfReg & 0xfe;
	Drv_Ch_I2C_DemodWrite(nChannelId, TDA10023_DEMOD_ADDRESS, SB_CONF, &val, 1);

	val = 0x0; // phillips 0x7f;  // DVB Mode select
	Drv_Ch_I2C_DemodWrite(nChannelId,  TDA10023_DEMOD_ADDRESS, SB_RESET, &val, 1);
}

#ifdef  EAGLE_DEBUG_CH
void JQAM_REG5(int nChannelId)
{
	HUINT8 val;

	Drv_Ch_I2C_DemodRead(nChannelId, TDA10023_JQAM_ADDRESS,  TDA10023_REG5_IND, &val, 1);

	CH_DI_Print (4, ("JQAM_REG5:: JQAM status = %x\n", val));
	if((val & 0x80) == 1)
	{
		CH_DI_Print (4, ("JQAM_REG5:: ERROR!\n"));
	}

	if((val & 0x40) == 1)
	{
		CH_DI_Print (4, ("JQAM_REG5:: EMERGENCY!\n"));
	}

	if((val & 0x30) == 0x00)
	{
		CH_DI_Print (4, ("JQAM_REG5:: Signal Bad\n"));  //  jcom에서 반대로 설정해서 내려오고  있음.
	}
	else if((val & 0x30) == 0x10)
	{
		CH_DI_Print (4, ("JQAM_REG5:: Signal Poor\n"));
	}
	else if((val & 0x30) == 0x20)
	{
		CH_DI_Print (4, ("JQAM_REG5:: Signal Good\n"));
	}
	else if((val & 0x30) == 0x30)
	{
		CH_DI_Print (4, ("JQAM_REG5:: Reserved\n"));
	}

	CH_DI_Print (4, ("JQAM_REG5:: VER = %x\n", (val & 0x0e) >> 1));

	if((val & 0x01) == 1)
	{
		CH_DI_Print (4, ("JQAM_REG5:: MLOCK OK\n"));
	}
}


void JQAM_TsStatus(int nChannelId)
{
	HUINT8 val;

	Drv_Ch_I2C_DemodRead(nChannelId, TDA10023_JQAM_ADDRESS,  TDA10023_TS_STATUS_H_IND, &val, 1);
	CH_DI_Print (4, ("JQAM_TsStatus:: 1. ts status = %x\n", val));

	Drv_Ch_I2C_DemodRead(nChannelId, TDA10023_JQAM_ADDRESS,  TDA10023_TS_STATUS_L_IND, &val, 1);
	CH_DI_Print (4, ("JQAM_TsStatus:: 2. ts status = %x\n", val));

	Drv_Ch_I2C_DemodRead(nChannelId, TDA10023_JQAM_ADDRESS,  TDA10023_RCV_STATUS4_IND, &val, 1);
	CH_DI_Print (4, ("JQAM_TsStatus:: 1. ts rcv status = %x\n", val));

	Drv_Ch_I2C_DemodRead(nChannelId, TDA10023_JQAM_ADDRESS,  TDA10023_RCV_STATUS3_IND, &val, 1);
	CH_DI_Print (4, ("JQAM_TsStatus:: 2. ts rcv status = %x\n", val));

	Drv_Ch_I2C_DemodRead(nChannelId, TDA10023_JQAM_ADDRESS,  TDA10023_RCV_STATUS2_IND, &val, 1);
	CH_DI_Print (4, ("JQAM_TsStatus:: 3. ts rcv status = %x\n", val));

	Drv_Ch_I2C_DemodRead(nChannelId, TDA10023_JQAM_ADDRESS,  TDA10023_RCV_STATUS1_IND, &val, 1);
	CH_DI_Print (4, ("JQAM_TsStatus:: 4. ts rcv status = %x\n", val));
}

void JQAM_TS(int nChannelId)
{
	HUINT8 val;
	int i, j;

	j=0;

	for (i=0; i<60; i++)
	{
		j++;
		Drv_Ch_I2C_DemodRead(nChannelId, TDA10023_JQAM_ADDRESS,  TDA10023_TS_ID1H_IND + i, &val, 1);
		CH_DI_Print (4, ("TS_ID [%d].= %x\n", i+1,val));

		if(j > 3)
		{
			j= 0;
			CH_DI_Print(4, ("\n"));
		}
	}
}

void Sig_Uncor_PKT(int nChannelId)
{
	HUINT8	puBytes[4];
	HUINT32	uncor_pkt, cor_pkt, ok_pkt;

	/* Get the number of uncorrected packet  */
	Drv_Ch_I2C_DemodRead(nChannelId,  TDA10023_DEMOD_ADDRESS, CPT_TSP_UNCOR1, &puBytes[0], 4);

    uncor_pkt = (puBytes[3]<<24) | (puBytes[2]<<16) | (puBytes[1]<<8) | puBytes[0];
    CH_DI_Print (4, ("Uncor_Pkt:: uncorrected packet = %d\n", uncor_pkt));

	/* Get the number of corrected packet  */
	Drv_Ch_I2C_DemodRead(nChannelId, TDA10023_DEMOD_ADDRESS, CPT_TSP_COR1, &puBytes[0], 4);

    cor_pkt = (puBytes[3]<<24) | (puBytes[2]<<16) | (puBytes[1]<<8) | puBytes[0];
    CH_DI_Print (4, ("Cor_Pkt:: corrected packet = %d\n", cor_pkt));

	/* Get the number of unerroed packet  */
	Drv_Ch_I2C_DemodRead(nChannelId, TDA10023_DEMOD_ADDRESS,  CPT_TSP_OK1, &puBytes[0], 4);

    ok_pkt = (puBytes[3]<<24) | (puBytes[2]<<16) | (puBytes[1]<<8) | puBytes[0];
    CH_DI_Print (4, ("Ok_Pkt:: ok packet = %d\n", ok_pkt));
    }


void d4r(int nChannelId, HUINT8 addr)
{
	HUINT8 addr1, val;

	addr1 = addr;

	Drv_Ch_I2C_DemodRead(nChannelId,  TDA10023_DEMOD_ADDRESS, addr1, &val,1);

	CH_DI_Print(5, ("#### addr = %x, val= %x\n", addr1, val));
}

void d4w(int nChannelId, HUINT8 addr, HUINT8 val)
{
	HUINT8 addr1, val1;

	addr1 = addr;
	val1 = val;

	Drv_Ch_I2C_DemodWrite(nChannelId,  TDA10023_DEMOD_ADDRESS, addr1, &val1, 1);
}

void  ifset(HUINT8 if_reg_val_max, unsigned char if_reg_val, HUINT8 agc_m)
{
	gucCABTopOn = agc_m;
	gucIfMinC = if_reg_val;
	gucIfMaxC = if_reg_val_max;
}

void rfset(unsigned char rf_reg_val, HUINT8 rf_reg_val_min, HUINT8 agc_m)
{
	gucCABTopOn = agc_m;
	gucRfMaxC = rf_reg_val;
	gucRfMinC = rf_reg_val_min;
}

void eqset(unsigned char eq_reg_val)
{
	eq_val = eq_reg_val;
}

void crset(HUINT8 cr_set)
{
	cr_val = cr_set;
}

void qamset(HUINT8 qam_set)
{
	qam_val = qam_set;
}

void go_bad(void)
{
	tsmf_flag = 1;
	test_gobad = 1;
}

void go_good(void)
{
	tsmf_flag = 1;
	test_gobad = 0;
}

void mcns(void)
{
	HUINT8 val;

	CH_DI_Print (5, ("mcns:: \n"));
     //
      Drv_Ch_I2C_DemodRead(0,  TDA10023_DEMOD_ADDRESS, 0x1f, &val, 1);
      val = 0x80 | val;
      Drv_Ch_I2C_DemodWrite(0,  TDA10023_DEMOD_ADDRESS, 0x1f, &val, 1);

      Drv_Ch_I2C_DemodRead(1, TDA10023_DEMOD_ADDRESS,  0x1f, &val, 1);
      val = 0x80 | val;
      Drv_Ch_I2C_DemodWrite(1, TDA10023_DEMOD_ADDRESS, 0x1f, &val, 1);
}
#endif

void Saclk_OnOff(int nChannelId, HUINT8 onOff)
{
	HUINT8	val;
	if(onOff == 0)	/* Off */
	{
		val = 0x23; /* For Test 0x1E, D[2] = 0, others are set by default */
		Drv_Ch_I2C_DemodWrite(nChannelId, TDA10023_DEMOD_ADDRESS, 0x1E, &val, 1);
	}
	else if(onOff == 1)	/* On */
	{
		val = 0x27; /* For Test 0x1E, D[2] = 1, others are set by default */
		Drv_Ch_I2C_DemodWrite(nChannelId, TDA10023_DEMOD_ADDRESS, 0x1E, &val, 1);
	}
	else
	{
		CH_DI_Print(3, ("Wrong param!!!!\n"));
	}
}

void ChangeCP(HUINT8 unit, HUINT8 val, HUINT8 sAct)
{
	HUINT8 tnr = 0, sCP=0;
	HUINT32	freq;

	chgCP = sAct;

	CH_DI_Print(5, ("Usage : ChangeCP 0/1, value, mode\n"));
	CH_DI_Print(5, ("	0: Master, 1: Slave\n"));
	CH_DI_Print(5, ("	value 1:38uA,2:54uA,3:83uA,4:122uA,5:163uA,6:254uA,7:400uA,8:580uA\n"));
	CH_DI_Print(5, ("	mode: 0= Inactive, 1=Active\n"));

	freq = gTDA10023ChInfo[unit].frequency;

	if ( freq <= 169000000 )
	{
		tnr = 0x81;
	}
	else if (freq <= 469000000)
	{
		tnr = 0x82;
	}
	else if (freq < 866000000)
	{
		tnr = 0x84;
	}
	else
	{
		CH_DI_Print(3, ("ChangeCP:: ERROR : Out of frequency range for tuner\n"));
	}

	switch(val)
	{
		case 1:
			sCP = 0x0; //0x1f;
			break;
		case 2:
			sCP = 0x1; //0x3f;
			break;
		case 3:
			sCP = 0x2; //0x5f;
			break;
		case 4:
			sCP = 0x3;// 0x7f;
			break;
		case 5:
			sCP = 0x4;//0x9f;
			break;
		case 6:
			sCP = 0x5;//0xbf;
			break;
		case 7:
			sCP = 0x6;//0xdf;
			break;
		case 8:
			sCP = 0x7;//0xff;
			break;
		default :
			CH_DI_Print(3, ("Out of Value %d\n", val));
			break;

	}

	gValCP = (tnr&0x1f) | ((sCP<<5)&0xe0);
	CH_DI_Print(5, ("ChangeCP = 0x%x\n", gValCP));
}


void ChangeBand(HUINT8 unit, HUINT8 val, HUINT8 sAct)
{
	CH_UNUSED(unit);

	chgBand = sAct;

	CH_DI_Print(5, ("Usage : ChangeBand  0/1, value, mode\n"));
	CH_DI_Print(5, ("	0: Master, 1: Slave\n"));
	CH_DI_Print(5, ("	value 1:Low 2:Mid, 3:High\n"));
	CH_DI_Print(5, ("	mode: 0= Inactive, 1=Active\n"));

	switch(val)
	{
		case 1:
			gValBand = 0x1;
			break;
		case 2:
			gValBand = 0x2;
			break;
		case 3:
			gValBand = 0x4;
			break;

		default :
			CH_DI_Print(3, ("Out of Value %d\n", val));
			break;
	}
}

void AgcLoopSet(HUINT8 aVal, HUINT8 aflag)
{
	if ( (aVal == 0) && (aflag == 0))
	{
		CH_DI_Print(5, ("Usage : AgcLoopSet <001[0x89], 010[0x92], 011[0x9b] ,100[0xa4]>, 1\n"));
	}
	else
	{
		agcFlag = aflag;
		agcVal = aVal;
	}
}

void DRV_C_InstallApi( void )
{
	CAB_InitDevice 			= &DP10023_InitDevice;
	CAB_SetTune 			= &DP10023_SetTune;
	CAB_CheckLock 			= &DP10023_CheckLock;
	CAB_GetStrengthQuality 	= &DP10023_GetStrengthQuality;
	CAB_SetPowerState 		= &DP10023_SetPowerState;
	CAB_EnableTsOutput 		= &DP10023_EnableTsOutput;
	CAB_DisableTsOutput 	= &DP10023_DisableTsOutput;
//CAB_UpdateTunedParam	= &DP10023_UpdateTunedParam;
//CAB_GetChannelStatus	= &DP10023_GetChannelStatus;
	CAB_ResetGpio = &DP10023_ResetGpio;
	CAB_Initialize = &DP10023_Initialize;

	CH_DI_Print(5, ("DRV_C_InstallApi()  OK! \n"));

	return;
}

int DP10023_InitDevice(int nChannelId)
{
	int nUnitId;

	nUnitId = GetDeviceUnitId( nChannelId );

	DP10023_ResetGpio(nUnitId);
	CH_DI_Print(2, ("DP10023_InitDevice : QAM GPIO Reset[%d] \n", nUnitId));

	return DI_CH_OK;
}

static void DP10023_InitDemodulator(int nChannelId)
{
	/* soft reset before register set-up */
	clearDemod(nChannelId);

	/****************************************************************
	 - set the QAM mode
	 - set the reference values and the threshold values
	 - set the registers which are set only once and then unchanged
	 - set the symbol rate
	 - set the registers which are changed at other functions
	 - for example, KAGC, CAR_C, GNYQ, CTADAPT)
	 ****************************************************************/

	InitQAMdemod(nChannelId);
}

int DP10023_LockDemodulator(int nChannelId)
{
	int	err;
	HUINT8		data, data2;

	if ( fast_lock_TDA(nChannelId) )
	{
#if 0
		data =0x02; // justin 040325 0x00; // justin  0x40;
		DemodIicTx(SB_ITSEL, &data, 1);

		data = 0x04; // justin 0x00;
		DemodIicTx(SB_ITSEL2, &data, 1);

		/* read the interrupt status register to prevent
		   the previous unlock state from reflecting the current lock status */
		DemodIicRx(SB_ITSTAT1, &data, 1);
#endif

//		CH_DI_Print ("Channel Locking  Ok!\n");
		/* adapt BER counter window according to the number
		   of the elapsed bit for the GET_DATA_INTERVAL period */

/* justin 	PBER 0xf8로 셋팅하면, BER 계산이 너무오래 걸림.
             AdaptBerWindow();
*/
		/* after lock, clear the value of BER conuter */
		Drv_Ch_I2C_DemodRead(nChannelId, TDA10023_DEMOD_ADDRESS, SB_RSCONF, &data, 1);
//		OSA_Print("####  Read %x\n", data);
		data2 = data & 0xdf;
		/* CLB_CPT_TSP = 0 */
		Drv_Ch_I2C_DemodWrite(nChannelId, TDA10023_DEMOD_ADDRESS, SB_RSCONF, &data2, 1);
		/* CLB_UNC bit = 1 */
		data2  = data | 0x20;
		Drv_Ch_I2C_DemodWrite(nChannelId, TDA10023_DEMOD_ADDRESS, SB_RSCONF, &data2, 1);
		err = DI_CH_OK;
	}
	else
	{
		err = DI_CH_ERR;
	}

	return err;
}

int DP10023_CheckStatus(int nChannelId)
{
	int	err;
//	HUINT32	BERValue[3] = {0,};
//	HUINT32	UnCorPacket[3] = {0,};
//	HUINT32	MaxBER = 0, MaxUnCor = 0;
//	HUINT32	TotalAvgBER = 0;
	HUINT8	GetFEClock;
	HUINT8   	val1;
//	HUINT8 i;
//	HUINT8 ber_set_cfg = 0;		/* 1 : after Viterbi, 0 : RS decoding */
	HUINT8	rfAgc, ifAgc;

	/* RF Cable 제거시 Unlock 메시지 올리는 루틴 */
	DP10023_GetAGC(nChannelId, &rfAgc, &ifAgc);
	if( (rfAgc == 0xff) && (ifAgc == 0xff) )
	{
		gTDA10023ChInfo[nChannelId].strength = 0;
		gTDA10023ChInfo[nChannelId].quality = 0;
		gTDA10023ChInfo[nChannelId].snr = 0;
		gTDA10023ChInfo[nChannelId].emergencyFlag = 0;
		totalCntToDisplayBER[nChannelId] = 0;
		err = DI_CH_ERR;
		return err;
	}

	/* Read the SYNC status register */
	Drv_Ch_I2C_DemodRead(nChannelId, TDA10023_DEMOD_ADDRESS, SB_SYNC, &val1, 1);
	GetFEClock = val1&0x48;

	if ( GetFEClock!=0x08 )
	{
		VK_TASK_Sleep(120); /* this delay is necessary */
		Drv_Ch_I2C_DemodRead(nChannelId, TDA10023_DEMOD_ADDRESS, SB_SYNC, &val1, 1);
		CH_DI_Print(3, ("One more Try : SB_SYNC = 0x%x\n", val1));

		GetFEClock = val1&0x48;

		if ( GetFEClock!=0x08 )
		{
			gTDA10023ChInfo[nChannelId].strength = 0;
			gTDA10023ChInfo[nChannelId].quality = 0;
			gTDA10023ChInfo[nChannelId].snr = 0;
			gTDA10023ChInfo[nChannelId].emergencyFlag = 0;
			totalCntToDisplayBER[nChannelId] = 0;

			CH_DI_Print(2, ("DP10023_CheckStatus[%d]:: Unlock (freq= %d) \n", nChannelId, gTDA10023ChInfo[nChannelId].frequency));
			err = DI_CH_ERR;
			return err;
		}
	}

	gTDA10023ChInfo[nChannelId].strength = Strength_Info_TDA(nChannelId);
	gTDA10023ChInfo[nChannelId].quality = BER_Info_TDA(nChannelId); /* 실제 값으로 대치할 것 */
	gTDA10023ChInfo[nChannelId].snr = SNR_Info_TDA(nChannelId); /* 실제 값으로 대치할 것 */

	gCurrentId[nChannelId] = 1;

	accuBER[nChannelId] += gTDA10023ChInfo[nChannelId].quality;		/* BER 누적 */

	gTDA10023Correction[nChannelId].CorrectedNo[0] = Sig_Corrected_PKT(nChannelId);
	gTDA10023Correction[nChannelId].UnCorrectedNo[0] = Sig_UnCorrected_PKT (nChannelId);
	gTDA10023Correction[nChannelId].UnErroredNo[0] = Sig_Unerroed_PKT (nChannelId);

	gTDA10023Correction[nChannelId].InbandBer[0] = gTDA10023ChInfo[nChannelId].quality;

	CH_DI_Print (2, ("TDA10023_CheckStatus[%d]:: Strength = %d, Quality = %d, SNR = %d\n", nChannelId, gTDA10023ChInfo[nChannelId].strength ,gTDA10023ChInfo[nChannelId].quality, gTDA10023ChInfo[nChannelId].snr)); // justin_1

	totalCntToDisplayBER[nChannelId]++;
	if(totalCntToDisplayBER[nChannelId] == 16)		/* 폴링주기 500ms, 500* 16 = 8sec */
	{
		avgBER[nChannelId] = accuBER[nChannelId] / 16;		/* 8초간 누적시킨 BER을 평균 내기 */
		CH_DI_Print (3, ("TDA10023_CheckStatus[%d]:: Average Quality = %d\n", nChannelId, avgBER[nChannelId]));
		gTDA10023Correction[nChannelId].InbandBer[0] = avgBER[nChannelId];
		accuBER[nChannelId] = 0;
		totalCntToDisplayBER[nChannelId]= 0;
	}

	CH_DI_Print(2, ("DP10023_CheckStatus[%d]:: Lock (freq= %d) \n", nChannelId, gTDA10023ChInfo[nChannelId].frequency));

	err = DI_CH_OK;

	return err;
}

int  DP10023_ResetGpio(int nChannelId)
{
	CH_UNUSED(nChannelId);

	DRV_GPIO_Write(GPIO_ID_RST_OFDM, GPIO_HI);

	DRV_GPIO_Write(GPIO_ID_RST_QAM, GPIO_LOW);
	VK_TASK_Sleep(30);

	DRV_GPIO_Write(GPIO_ID_SEL_DEMOD, GPIO_HI);
	CH_DI_Print(4,(" Set Tuner QAM Mode\n"));

	DRV_GPIO_Write(GPIO_ID_RST_QAM, GPIO_HI);
	VK_TASK_Sleep(30);

	return DI_CH_OK;
}

int DP10023_Initialize(int nChannelId)
{
	//3, 5274, 653000000
	gTDA10023ChInfo[nChannelId].qamMode = 3;
	gTDA10023ChInfo[nChannelId].symbolRate = 5274;
	gTDA10023ChInfo[nChannelId].frequency = 653000000;

	sTsmfGoodFlag[nChannelId] = 1;
	sTsmfEmgOffFlag[nChannelId] = 1;

	DP10023_InitDemodulator(nChannelId);

	InitDemodulator(nChannelId);

	return DI_CH_OK;
}

int DP10023_SetTune(int nChannelId, HUINT32 ulWorkingId, CH_CAB_TuneParam_t *pstCabTuneParam )
{
	int	err;
	HINT32 tsid, netid;
	HUINT8 val;

	CH_UNUSED(ulWorkingId);

	//DP10023_Initialize(nChannelId);

	//VK_TASK_Sleep(10);

	gTDA10023ChInfo[nChannelId].qamMode = pstCabTuneParam->etModulation;
	gTDA10023ChInfo[nChannelId].symbolRate = pstCabTuneParam->ulSymbolRate;
	gTDA10023ChInfo[nChannelId].frequency = pstCabTuneParam->ulFrequency*1000;
	gTDA10023ChInfo[nChannelId].netId = pstCabTuneParam -> nNetId;
	gTDA10023ChInfo[nChannelId].tsId = pstCabTuneParam -> nTsId;

	 CH_DI_Print(3,("DP10023_SetTune.........\n"));
	//temp
	//gTDA10023ChInfo[nChannelId].netId = 4;
	//gTDA10023ChInfo[nChannelId].tsId = 16626;

	CH_DI_Print (2, ("DP10023_SetTune::  freq= %d\n",  gTDA10023ChInfo[nChannelId].frequency));
	//CH_DI_Print ("DP10023_SetTune::  netId= %d\n", gTDA10023ChInfo[nChannelId].netId);
	//CH_DI_Print ("DP10023_SetTune::  tsId= %d\n",  gTDA10023ChInfo[nChannelId].tsId);

	beforeId1[nChannelId] = 0;
	beforeId2[nChannelId] = 0;
	gCurrentId[nChannelId] = 0;

	if (save_stream == 1 )
	{
		err =DP10023_SetTuner(nChannelId);

		if(err != DI_CH_OK)
		{
			err = DI_CH_ERR;
			return err;
		}

		InitDemodulator(nChannelId);		/* 수정 루틴 */
		DP10023_InitDemodulator(nChannelId);
		VK_TASK_Sleep( (HUINT16)(no_to_msec(nChannelId, WAIT_AGCLOCK*3)) );   // 76ms

		DP10023_LockDemodulator(nChannelId);

		JQAM_Mode(nChannelId, 0);

		val = 0x00;
		Drv_Ch_I2C_DemodWrite(nChannelId, TDA10023_DEMOD_ADDRESS, SB_CONTROL, &val, 1);

		return DI_CH_OK;
	}

	if ( gTDA10023ChInfo[nChannelId].frequency == 999000000 )
	{
		/* TS output set to tri-state*/
		val = 0x02;  /* TRIP */
		Drv_Ch_I2C_DemodWrite(nChannelId, TDA10023_DEMOD_ADDRESS, SB_CONTROL, &val, 1);

		err = DI_CH_OK;
		return err ;
	}

	err = DP10023_SetTuner(nChannelId);
	if(err != DI_CH_OK)
	{
		err = DI_CH_ERR;
		return err;
	}

	InitDemodulator(nChannelId);		/* 수정 루틴, 소프트 리셋 */
	DP10023_InitDemodulator(nChannelId);		/* 레지스터 초기화 */

	err=DP10023_LockDemodulator(nChannelId);		/* 락킹 시도 */

	tsid  = gTDA10023ChInfo[nChannelId].tsId;
	netid = gTDA10023ChInfo[nChannelId].netId;

	VK_TASK_Sleep(100);		/*  채널전환 또는  standby off->on 반복 시  초기에 TSMF number가 0이 되는 경우 발생해서 E203 표시되는 문제 수정  2009-12-19 */

	gTDA10023TsmfInfo[nChannelId].tsNo = JQAM_TsNumber(nChannelId);


	CH_DI_Print(1, ("DP10023_SetTune[%d]:: Lock = %d, tsid = %x, netid= %x, tsno = %d\n",  nChannelId, err, tsid, netid, gTDA10023TsmfInfo[nChannelId].tsNo));


	if ( (tsid == -1) && (netid == -1) )
	{
		gTDA10023TsmfInfo[nChannelId].tsNo = JQAM_TsNumber(nChannelId);

		JQAM_TSinfo(nChannelId);
	}
	else if (gTDA10023TsmfInfo[nChannelId].tsNo < 2 )
	{
		JQAM_Mode(nChannelId, 0);
		gTDA10023TsmfInfo[nChannelId].tsNo = 0;
	}
	else
	{
		JQAM_Mode(nChannelId, 2);
		JQAM_TsId( nChannelId, tsid, netid);

		gTDA10023TsmfInfo[nChannelId].tsNo = 0;
	}

	if(err == DI_CH_OK) /* 파라미터 검사 OK */
	{
		CH_DI_Print(1, ("[DP10023_SetTune] LOCK! \n"));

		/* To prevent block noise, low snr at signal line plug-in and plug-out  */
		HUINT16 snr =0;
		HUINT32 uncorredPkt =0;

		snr = SNR_Info_TDA(nChannelId); /* 실제 값으로 대치할 것 */
		uncorredPkt = Sig_UnCorrected_PKT (nChannelId);

		CH_DI_Print(0, ("[DP10023_SetTune] snr = %d  uncorredPkt = %d \n", snr, uncorredPkt));

		if(snr < 251 && uncorredPkt > 2000)
		{
			CH_DI_Print(0, ("[DP10023_SetTune] UNLOCK! \n"));
			err = DI_CH_ERR;
			return err;
		}

		err = DI_CH_OK;
	}
	else /* 파라미터 오류 */
	{

		CH_DI_Print(1, ("[DP10023_SetTune] UNLOCK! \n"));
		err = DI_CH_ERR;
	}

	return err;
}

void DP10023_QAMStop(int nChannelId)
{
	gTDA10023ChInfo[nChannelId].strength = 0;
	gTDA10023ChInfo[nChannelId].quality = 0;
	gTDA10023ChInfo[nChannelId].snr = 0;
	gTDA10023ChInfo[nChannelId].emergencyFlag = 0;

	/* 동일 락킹 파라메터인 경우 다시 tune을 하지 않으므로 stop시 긴급정보 메세지 관련 초기화 해준다. */
	beforeId1[nChannelId] = 0;
	beforeId2[nChannelId] = 0;
	gCurrentId[nChannelId] = 0;

	if(save_stream != 1)
	{
		DeactivateDemodulator(nChannelId);
	}
}

HBOOL DP10023_CheckLock(int nChannelId)
{
	int	err = DI_CH_OK;

	err = DP10023_CheckStatus(nChannelId);

	gTDA10023TsmfInfo[nChannelId].tsNo = JQAM_TsNumber(nChannelId);

	JQAM_Status(nChannelId);   /* emergency, status */

	if(err != DI_CH_ERR) /* locking 상태 이면 */
	{
		return TRUE;
	}
	else /* unlock 상태이면 */
	{
		return FALSE;
	}

}

int DP10023_GetStatus(int nChannelId, HUINT32 frequency)
{
	int	err = DI_CH_OK;
	HUINT32 unRequestId = 0;

	gTDA10023ChInfo[nChannelId].frequency = frequency;

	if ( gTDA10023ChInfo[nChannelId].frequency == 999000000 )
	{
		err = DI_CH_OK;
		return err ;
	}

	err = DP10023_CheckStatus(nChannelId);

	gTDA10023TsmfInfo[nChannelId].tsNo = JQAM_TsNumber(nChannelId);

	JQAM_Status(nChannelId);   /* emergency, status */

	if(err != DI_CH_ERR) /* locking 상태 이면 */
	{
		/* 서비스 ID가 바뀔때 마다 메시지 보내고, 동일 service id일경우는 처음 한번만 보냄. */
		if( (gTDA10023ChInfo[nChannelId].snr > 25) && (gTDA10023ChInfo[nChannelId].tsmfStatus == 2) )
		{
			GoodRetry = 0;
			CH_DI_Print(5, ("### BadRetry = %d\n", BadRetry));
			if ( BadRetry > 4)
			{
				sTsmfGoodFlag[nChannelId] = TRUE;

				if((sTsmfBadFlag[nChannelId] == FALSE) || (beforeId1[nChannelId] != gCurrentId[nChannelId]))
				{
					sTsmfBadFlag[nChannelId] = TRUE;
					beforeId1[nChannelId] = gCurrentId[nChannelId];

					CH_DI_Print (1,("TDA10023_QAMGetStatus:: Send TSMF Bad Message .....\n\n"));

					//GWM_SendMessage(MSG_CH_TSMF_BAD, versionNumber, CH_MOD_QAM, 0);
					DRV_CH_CallNotifyCallback( nChannelId, unRequestId, DI_CH_SIGNAL_TSMF_BAD);

					BadRetry = 0;
				}
			}
			else
			{
				BadRetry++;
			}
		}
		else
		{
			BadRetry = 0;
			CH_DI_Print (5, ("### GoodRetry = %d\n", GoodRetry));

			if(GoodRetry > 4)
			{
				sTsmfBadFlag[nChannelId] = FALSE;

				if((sTsmfGoodFlag[nChannelId] == TRUE) || (beforeId1[nChannelId] != gCurrentId[nChannelId]))
				{
					sTsmfGoodFlag[nChannelId] = FALSE;
					beforeId1[nChannelId] = gCurrentId[nChannelId];

					CH_DI_Print (1, ("TDA10023_QAMGetStatus:: Send TSMF Good Message .....\n\n"));

					//GWM_SendMessage(MSG_CH_TSMF_GOOD, versionNumber, CH_MOD_QAM, 0);
					DRV_CH_CallNotifyCallback( nChannelId, unRequestId, DI_CH_SIGNAL_TSMF_GOOD);

					GoodRetry = 0;
				}
			}
			else
			{
				GoodRetry++;
			}
		}

		if( gTDA10023ChInfo[nChannelId].emergencyFlag == 1)
		{
			sTsmfEmgOffFlag[nChannelId] = TRUE;
			if((sTsmfEmgOnFlag[nChannelId] == FALSE) || (beforeId2[nChannelId] != gCurrentId[nChannelId]) )
			{
				sTsmfEmgOnFlag[nChannelId] = TRUE;
				beforeId2[nChannelId] = gCurrentId[nChannelId];

				CH_DI_Print (1, ("TDA10023_QAMGetStatus:: TSMF Emergency On ..... \n\n"));

				//GWM_SendMessage(MSG_CH_EWS_ON, versionNumber, CH_MOD_QAM, 0);
				DRV_CH_CallNotifyCallback( nChannelId, unRequestId, DI_CH_SIGNAL_TSMF_EWS_ON);
			}
		}
		else
		{
			sTsmfEmgOnFlag[nChannelId] = FALSE;
			if((sTsmfEmgOffFlag[nChannelId] == TRUE) || (beforeId2[nChannelId] != gCurrentId[nChannelId]) )
			{
				sTsmfEmgOffFlag[nChannelId] = FALSE;
				beforeId2[nChannelId] = gCurrentId[nChannelId];

				CH_DI_Print (1, ("TDA10023_QAMGetStatus:: TSMF Emergency Offl .....\n\n"));

				//GWM_SendMessage(MSG_CH_EWS_OFF, versionNumber, CH_MOD_QAM, 0);
				DRV_CH_CallNotifyCallback( nChannelId, unRequestId, DI_CH_SIGNAL_TSMF_EWS_OFF);
			}
		}

		/* To prevent block noise, low snr at signal line plug-in and plug-out  */
		HUINT16 snr =0;
		HUINT32 uncorredPkt =0;

		snr = SNR_Info_TDA(nChannelId); /* 실제 값으로 대치할 것 */
		uncorredPkt = Sig_UnCorrected_PKT (nChannelId);

		if(snr < 251 && uncorredPkt > 2000)
		{
			CH_DI_Print(0, ("[DP10023_GetStatus] snr = %d  uncorredPkt = %d \n", snr, uncorredPkt));
			CH_DI_Print(0, ("[DP10023_GetStatus] UNLOCK! \n"));
			err = DI_CH_ERR;
			return err;
		}

	}
	else /* unlock 상태이면 */
	{
		gTDA10023ChInfo[nChannelId].strength = 0;
		gTDA10023ChInfo[nChannelId].quality = 0;
		gTDA10023ChInfo[nChannelId].snr = 0;
		gTDA10023ChInfo[nChannelId].emergencyFlag = 0;
	}

	return err;
}

int DP10023_CheckTsmfLock(int nChannelId, HUINT32 frequency)
{
	int	err = DI_CH_OK;
	int	checklock = FALSE;

	gTDA10023ChInfo[nChannelId].frequency = frequency;

	if ( gTDA10023ChInfo[nChannelId].frequency == 999000000 )
	{
		err = DI_CH_OK;
		return err ;
	}

	err = DP10023_CheckStatus(nChannelId);

	gTDA10023TsmfInfo[nChannelId].tsNo = JQAM_TsNumber(nChannelId);

	JQAM_Status(nChannelId);   /* emergency, status */

	if(err != DI_CH_ERR) /* locking 상태 이면 */
	{
		checklock = TRUE;
	}
	else /* unlock 상태이면 */
	{
		gTDA10023ChInfo[nChannelId].strength = 0;
		gTDA10023ChInfo[nChannelId].quality = 0;
		gTDA10023ChInfo[nChannelId].snr = 0;
		gTDA10023ChInfo[nChannelId].emergencyFlag = 0;

		checklock = FALSE;
	}

	return checklock;

}

void DP10023_GetChLockData(int nChannelId, HINT16 *strength, HUINT32 *quality, HUINT16 *snr)
{
	*strength = gTDA10023ChInfo[nChannelId].strength;
	*quality = gTDA10023ChInfo[nChannelId].quality;
	*snr = gTDA10023ChInfo[nChannelId].snr;
//	*spectralInverse = gTDA10023ChInfo[nChannelId].SpectralInverse;

	//DDI_UART_Print("[DP10023] strength = %d, quality = %d , snr = %d \n ",*strength , *quality, *snr );
}

int  DP10023_GetStrengthQuality(int nChannelId, DI_CH_SignalQuality_t *pstSignalInfo)
{
	HUINT8 ucRfAgc=0, ucIfAgc=0;
	HINT16 ucStrength=0;
	HUINT32 ulQuality=0;
	HUINT16 ucSnr=0;

	DP10023_GetAGC( nChannelId, &ucRfAgc, &ucIfAgc );
	DP10023_GetChLockData(nChannelId, &ucStrength, &ulQuality, &ucSnr);

	pstSignalInfo->ulStrength	= (HUINT32)ucStrength;
	pstSignalInfo->ulAgc		= (HUINT32)ucRfAgc;
	pstSignalInfo->ulQuality	= (HUINT32)ulQuality;
	pstSignalInfo->fSnr = (float)ucSnr;

	pstSignalInfo->bEmergencyflag = gTDA10023ChInfo[nChannelId].emergencyFlag;

	pstSignalInfo->ulCorrectedNo  = gTDA10023Correction[nChannelId].CorrectedNo[0];
	pstSignalInfo->ulUnCorrectedNo= gTDA10023Correction[nChannelId].UnCorrectedNo[0];
	pstSignalInfo->ulUnErroredNo = gTDA10023Correction[nChannelId].UnErroredNo[0];
	pstSignalInfo->ulInbandBer = gTDA10023Correction[nChannelId].InbandBer[0];
	pstSignalInfo->ulUnCorrectedFlag = gTDA10023Correction[nChannelId].UnCorrectedFlag[0];

	CH_DI_Print(2, (" ulStrength =%d , ulAgc = %d, ulQuality = %d, fSnr = %f, bEmergencyflag = %d \n ",
					pstSignalInfo->ulStrength,pstSignalInfo->ulAgc, pstSignalInfo->ulQuality, pstSignalInfo->fSnr, pstSignalInfo->bEmergencyflag));

	CH_DI_Print(3, (" ulCorrectedNo =%d , ulUnCorrectedNo = %d, ulUnErroredNo = %d, ulInbandBer = %d, ulUnCorrectedFlag = %d \n ",
					pstSignalInfo->ulCorrectedNo,pstSignalInfo->ulUnCorrectedNo, pstSignalInfo->ulUnErroredNo, pstSignalInfo->ulInbandBer, pstSignalInfo->ulUnCorrectedFlag));

	return DI_CH_OK;
}

void DP10023_TsmfData(int nChannelId, HUINT16 *tsNo, HINT32 *tsID, HINT32 *netID)
{
	int i = 0;

	*tsNo = gTDA10023TsmfInfo[nChannelId].tsNo;

	JQAM_TSinfo(nChannelId);	/* At initial locking, uncorrected errorsl happen, demode get the wrong TSMF information. Add this API for getting correct TSMF 2009-10-27  */

	for(i = 0; i < 15; i++)
	{
		tsID[i] = gTDA10023TsmfInfo[nChannelId].tsID[i];
		netID[i] = gTDA10023TsmfInfo[nChannelId].netID[i];
	}
}


void DP10023_GetAGC(int nChannelId, HUINT8 *ucpRFAgc, HUINT8 *ucpIFAgc)
{
	HUINT8	ucRFVal, ucIFVal;

	Drv_Ch_I2C_DemodRead(nChannelId, TDA10023_DEMOD_ADDRESS, TDA10023_AGCIF, &ucIFVal, 1);		/* Read IF AGC */
	Drv_Ch_I2C_DemodRead(nChannelId, TDA10023_DEMOD_ADDRESS, TDA10023_AGCTUN, &ucRFVal, 1);		/* Read RF AGC */

	*ucpRFAgc = ucRFVal;
	*ucpIFAgc = ucIFVal;

	//DDI_UART_Print("[DP10023] RF AGC = %d, IF AGC = %d \n ",ucRFVal, ucIFVal);
}

void DP10023_GetEwsInfo(int nChannelId, HUINT8 *ucIsEws)
{
	*ucIsEws = gTDA10023ChInfo[nChannelId].emergencyFlag;
	CH_DI_Print (4, ("TDA10023_GetEwsInfo:: EmergencyFlag = 0x%x\n", gTDA10023ChInfo[nChannelId].emergencyFlag));
}

void DP10023_DumpRegister( int nChannelId)
{
	int i=0;
	unsigned char ucdata;

	//nUnitId = GetDeviceUnitId( nChannelId );

	CH_DI_Print(0, ("===================================================\n"));
	CH_DI_Print(0, ("   CAB TUNE Unit [%d] Demod Dump\n", nChannelId));
	CH_DI_Print(0, ("===================================================\n"));

	for(i=0; i<= 0xFE; i++)
	{
		Drv_Ch_I2C_DemodRead(nChannelId, TDA10023_DEMOD_ADDRESS, i, &ucdata, 1);
		CH_DI_Print(0, ("[0x%02X] \t[0x%02x]\n", i, ucdata));
	}

	CH_DI_Print(0, ("===================================================\n"));
	CH_DI_Print(0, ("   CAB TUNE Unit [%d] Tuner Dump\n", nChannelId));
	CH_DI_Print(0, ("===================================================\n"));

	//for(i=0; i<= 0x3C; i++)
	{
	 	enable_tuner_iic(nChannelId);

		Drv_Ch_I2C_TunerRead(nChannelId, S22CX_TUNER_ADDRESS, &ucdata, 1);

		disable_tuner_iic(nChannelId);
		CH_DI_Print(0, ("[0x%02x]\n", ucdata));
	}
	CH_DI_Print(0, ("===================================================\n"));

}

int  DP10023_SetPowerState( int nChannelId, CH_Power_e etPower )
{
	CH_DI_Print(6, ("[DP10023_SetPowerState] ----\n"));

	int nUnitId;

	nUnitId = GetDeviceUnitId( nChannelId );

	switch (etPower)
	{
		case CH_POWER_OFF:
			DRV_GPIO_Write(GPIO_ID_RST_QAM, GPIO_LOW);   /* 지상파 동작시 성능 개선을 위해 qam을 리겟으로 잡음. I2C를 지상파 것을 사용하기때문에 지상파는 리셋으로 잡으면 안됨. */

			CH_DI_Print(5, ("------------- standby mode -------------\n"));
			break;

		case CH_POWER_ON:
		default:
			DP10023_ResetGpio( nChannelId );
			CH_DI_Print(5, ("------------- Power on mode -------------\n"));
			break;
	}

	return DI_CH_OK;
}

int DP10023_EnableTsOutput ( int nChannelId )
{
	CH_UNUSED(nChannelId);

#if 0
	HUINT8 ucVal;
	int nUnitId;

	nUnitId = GetDeviceUnitId( nChannelId );

	drv_ch_i2c_DEMODRead(nUnitId, TDA_10023_SB_INTPS, &ucVal, 1);

	ucVal &= (~0x02);// 0x2c [1] TRIP : If 1, tri-stated, else active.

	drv_ch_i2c_DEMODWrite(nUnitId, TDA_10023_SB_INTPS, &ucVal, 1);
#endif
	return DI_CH_OK;
}



int DP10023_DisableTsOutput ( int nChannelId )
{
	CH_UNUSED(nChannelId);

#if 0
	HUINT8 ucVal;
	int nUnitId;

	nUnitId = GetDeviceUnitId( nChannelId );

	drv_ch_i2c_DEMODRead(nUnitId, TDA_10023_SB_INTPS, &ucVal, 1);

	ucVal |= 0x02;// 0x2c [1] TRIP : If 1, tri-stated, else active.

	drv_ch_i2c_DEMODWrite(nUnitId, TDA_10023_SB_INTPS, &ucVal, 1);
#endif

	return DI_CH_OK;

}
#endif

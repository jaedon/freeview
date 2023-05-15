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
#include "drv_channel_dt90517.h"

#if defined (CONFIG_PLATFORM_TM201COMBO)						// JD - Adding Header (for JC-8500BR)
#include "sembsl_mxl201.h"
#endif


/****************************************************
	Define
******************************************************/
#define BUS_NUM				NUM_OF_CHANNEL
#define ISDBT_LOCK_TRIAL	2
#define ISDBT_LAYER_NUM	3
#define MAX_NUM_TER_UNITS		2

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
HUINT32 gCurrentId[BUS_NUM];
HUINT32 gBERCalMode[BUS_NUM] = {0,0}; 	 /* BER 계산 모드 */
HUINT32 gBERDisplayPeriod[BUS_NUM] = {2,2};	 /* BER 디스플레이 주기 */

HUINT8	gIsdbDemodStat;

HUINT16 HKFRQ, AFC30, AFC39, AFC32;
HUINT8 ghkfrq, gafc,gafc39, gafc32;

 HUINT8	gValCP, chgCP;
 HUINT8	gValBand, chgBand;
 HUINT8	save_stream;


TC90517_status			gTC90517Status[BUS_NUM];
TC90517_Status_t 			gTC90517Channel[BUS_NUM];
TC90517_LayerInfo_t 		gTC90517Layer[BUS_NUM];
TC90517_CorrectionInfo_t	gTC90517Correction[BUS_NUM];


HUINT8 accuFlag[BUS_NUM];				/* accumualtion enable Flag */
HUINT32 accuCnt[BUS_NUM];				/* accumulation elapsed time : accuCnt * 4 = total elapsed time */
HUINT32 accuUnCorPkt[BUS_NUM][ISDBT_LAYER_NUM];		/* Total # of accumulated packet : [nChannelId][Layer] */
HUINT32	ofdmUncorPkt[BUS_NUM][ISDBT_LAYER_NUM];

HUINT8	gucIfMinT = 0x60;
HUINT8	gucIfMaxT = 0xff;
HUINT8	gucRfMinT  = 0x00;
HUINT8	gucRfMaxT  = 0xff;
HUINT8	gucTERTopOn = 0;


#define	levelNo  41

HINT8	dslevel[levelNo] = {
84,83,82,81,80,79,78,77,76,75,
74,73,72,71,70,69,68,67,66,65,
64,63,62,61,60,59,58,57,56,55,
54,53,52,51,50,49,48,47,46,45,
44
};


#if 0
/*  ~ 182MHz */
HUINT8  isdbtRfAgcGain1[2][41] =
{
	{
		38,42,46,50,54,59,66,74,83,92,
		101,109,116,122,128,133,138,143,148,152,
		156,159,163,166,169,173,176,179,182,185,
		189,192,195,199,202,208,214,226,255,255,
		255


	},
	{
		38,42,46,50,54,59,66,74,83,92,
		101,109,116,122,128,133,138,143,148,152,
		156,159,163,166,169,173,176,179,182,185,
		189,192,195,199,202,208,214,226,255,255,
		255


	}
};
HUINT8  isdbtIfAgcGain1[2][41] =
{
	{
		80,80,80,80,80,80,80,80,80,80,
		80,80,80,80,80,80,80,80,80,80,
		80,80,80,80,80,80,80,80,80,80,
		80,80,80,80,80,80,80,80,80,82,
		84


	},
	{
		80,80,80,80,80,80,80,80,80,80,
		80,80,80,80,80,80,80,80,80,80,
		80,80,80,80,80,80,80,80,80,80,
		80,80,80,80,80,80,80,80,80,82,
		84


	}
};

/* 183 ~ 472MHz */
HUINT8  isdbtRfAgcGain2[2][41] =
{
	{
		50,54,59,66,74,83,92,101,109,116,
		122,128,133,138,143,148,152,156,159,163,
		166,169,173,176,179,182,185,189,192,195,
		199,202,208,214,226,255,255,255,255,255,
		255


	},
	{
		50,54,59,66,74,83,92,101,109,116,
		122,128,133,138,143,148,152,156,159,163,
		166,169,173,176,179,182,185,189,192,195,
		199,202,208,214,226,255,255,255,255,255,
		255


	}
};
HUINT8  isdbtIfAgcGain2[2][41] =
{
	{
		80,80,80,80,80,80,80,80,80,80,
		80,80,80,80,80,80,80,80,80,80,
		80,80,80,80,80,80,80,80,80,80,
		80,80,80,80,80,80,82,84,86,88,
		90


	},
	{
		80,80,80,80,80,80,80,80,80,80,
		80,80,80,80,80,80,80,80,80,80,
		80,80,80,80,80,80,80,80,80,80,
		80,80,80,80,80,80,82,84,86,88,
		90


	}
};

/*473 ~ 520MHz */
HUINT8  isdbtRfAgcGain3[2][41] =
{
	{
		68,72,75,79,83,87,90,94,97,101,
		104,107,110,113,117,120,123,126,130,134,
		137,141,146,151,156,163,170,179,188,196,
		206,216,228,240,252,255,255,255,255,255,
		255


	},
	{
		68,72,75,79,83,87,90,94,97,101,
		104,107,110,113,117,120,123,126,130,134,
		137,141,146,151,156,163,170,179,188,196,
		206,216,228,240,252,255,255,255,255,255,
		255

	}
};
HUINT8  isdbtIfAgcGain3[2][41] =
{
	{
		80,80,80,80,80,80,80,80,80,80,
		80,80,80,80,80,80,80,80,80,80,
		80,80,80,80,80,80,80,80,80,80,
		80,80,80,80,80,82,84,86,88,90,
		92

	},
	{
		80,80,80,80,80,80,80,80,80,80,
		80,80,80,80,80,80,80,80,80,80,
		80,80,80,80,80,80,80,80,80,80,
		80,80,80,80,80,82,84,86,88,90,
		92


	}
};

/* 521 ~ 622MHz */
HUINT8  isdbtRfAgcGain4[2][41] =
{
	{
		63,67,71,75,79,84,88,91,96,99,
		102,106,109,112,115,118,121,125,128,132,
		136,140,144,150,156,162,170,178,186,197,
		207,218,229,241,253,255,255,255,255,255,
		255
	},
	{
		63,67,71,75,79,84,88,91,96,99,
		102,106,109,112,115,118,121,125,128,132,
		136,140,144,150,156,162,170,178,186,197,
		207,218,229,241,253,255,255,255,255,255,
		255

	}
};
HUINT8  isdbtIfAgcGain4[2][41] =
{
	{
		80,80,80,80,80,80,80,80,80,80,
		80,80,80,80,80,80,80,80,80,80,
		80,80,80,80,80,80,80,80,80,80,
		80,80,80,80,80,82,84,86,88,90,
		92


	},
	{
		80,80,80,80,80,80,80,80,80,80,
		80,80,80,80,80,80,80,80,80,80,
		80,80,80,80,80,80,80,80,80,80,
		80,80,80,80,80,82,84,86,88,90,
		92

	}
};

/* 623 ~  */
HUINT8  isdbtRfAgcGain5[2][41] =
{
	{
		68,72,75,79,83,87,90,94,97,101,
		104,107,110,113,117,120,123,126,130,134,
		137,141,146,151,156,163,170,179,188,196,
		206,216,228,240,252,255,255,255,255,255,
		255

	},
	{
		68,72,75,79,83,87,90,94,97,101,
		104,107,110,113,117,120,123,126,130,134,
		137,141,146,151,156,163,170,179,188,196,
		206,216,228,240,252,255,255,255,255,255,
		255

	}
};

HUINT8  isdbtIfAgcGain5[2][41] =
{
	{
		80,80,80,80,80,80,80,80,80,80,
		80,80,80,80,80,80,80,80,80,80,
		80,80,80,80,80,80,80,80,80,80,
		80,80,80,80,80,82,84,86,88,90,
		92


	},
	{
		80,80,80,80,80,80,80,80,80,80,
		80,80,80,80,80,80,80,80,80,80,
		80,80,80,80,80,80,80,80,80,80,
		80,80,80,80,80,82,84,86,88,90,
		92


	}
};

/* 713MHz ~ */
HUINT8  isdbtRfAgcGain6[2][41] =
{
	{
		118,122,126,130,133,138,143,147,152,158,
		165,172,180,189,198,209,219,231,241,249,
		255,255,255,255,255,255,255,255,255,255,
		255,255,255,255,255,255,255,255,255,255,
		255

	},
	{
		118,122,126,130,133,138,143,147,152,158,
		165,172,180,189,198,209,219,231,241,249,
		255,255,255,255,255,255,255,255,255,255,
		255,255,255,255,255,255,255,255,255,255,
		255

	}
};
HUINT8  isdbtIfAgcGain6[2][41] =
{
	{
		50,50,50,50,50,50,50,50,50,50,
		50,50,50,50,50,50,50,50,50,51,
		53,56,60,63,66,69,71,74,76,78,
		80,82,83,85,87,90,91,93,95,97,
		99


	},
	{
		50,50,50,50,50,50,50,50,50,50,
		50,50,50,50,50,50,50,50,50,51,
		53,56,60,63,66,69,71,74,76,78,
		80,82,83,85,87,90,91,93,95,97,
		99


	}
};
#endif

HUINT16  NewAgcGain1[levelNo] = {
164,168,172,176,179,183,188,192,195,199,
203,207,211,215,218,222,225,229,232,235,
238,241,245,248,251,254,256,259,262,265,
268,271,274,277,280,284,288,294,306,314,
319
};

HUINT16  NewAgcGain2[levelNo] = {
171,175,179,184,188,192,196,200,205,208,
212,216,219,223,227,229,232,235,239,242,
245,248,251,254,257,260,262,265,268,271,
273,276,279,283,286,291,298,314,319,328,
339
};

HUINT16  NewAgcGain3[levelNo] = {
201,206,211,216,220,224,228,231,235,238,
242,245,249,252,255,257,260,263,266,269,
271,274,277,281,284,289,296,310,317,324,
338,340,342,344,346,348,350,352,354,356,
358
};

HUINT16  NewAgcGain4[levelNo] = {
182,188,193,199,204,208,213,217,222,226,
230,233,237,241,244,247,251,254,257,260,
263,266,269,272,275,278,282,287,293,307,
314,319,330,339,341,343,345,347,349,351,
353
};

HUINT16  NewAgcGain5[levelNo] = {
162,168,175,181,187,192,198,203,208,213,
217,222,226,230,234,237,241,244,248,251,
254,257,260,263,266,270,273,276,281,286,
291,304,312,316,323,339,341,343,345,347,
349
};

HUINT16  NewAgcGain6[levelNo] = {
151,157,164,170,177,183,190,196,202,208,
212,218,223,227,231,235,239,243,246,250,
253,257,260,263,266,269,273,277,282,287,
295,308,311,315,320,327,341,343,345,347,
349
};

HUINT16  NewAgcGain7[levelNo] = {
155,162,168,176,183,191,198,205,211,216,
222,227,232,235,240,243,247,251,255,258,
261,264,267,271,275,279,283,290,303,309,
312,316,321,329,341,343,345,347,349,351,
353
};


HUINT16  NewAgcGain8[levelNo] = {
162,166,169,173,177,180,184,187,190,193,
197,200,203,206,210,214,218,222,227,232,
238,245,252,260,269,276,287,297,306,312,
319,325,332,339,342,344,346,349,351,353,
355
};

HUINT16  NewAgcGain9[levelNo] = {
155,159,163,167,172,175,179,183,186,190,
193,196,200,203,206,210,214,218,223,227,
233,239,246,253,261,270,278,288,299,309,
315,321,328,335,341,343,345,347,349,351,
353
};


HUINT16  NewAgcGain10[levelNo] = {
148,152,156,161,165,169,174,178,181,185,
189,193,196,199,203,207,210,214,218,223,
226,232,238,245,252,260,269,279,288,300,
309,316,322,329,336,341,343,345,347,349,
351

};

static HUINT16  NewAgcGain11[levelNo] = {
139,143,148,152,157,161,166,171,175,179,
183,187,191,194,198,201,205,208,213,217,
221,225,231,237,244,250,259,268,277,288,
298,308,315,322,329,335,341,343,345,347,
349
};

static HUINT16  NewAgcGain12[levelNo] = {
131,135,139,143,148,153,158,163,168,173,
177,181,186,190,194,197,201,205,209,213,
216,221,226,231,238,245,252,261,271,281,
291,302,313,320,326,333,340,342,344,346,
348
};




HUINT8	gDT90517InitTbl[0xff] =
{
	/*
	0x00,0x01,0x02,0x03,0x04,0x05,0x06,0x07,0x08,0x09,0x0a,0x0b,0x0c,0x0d,0x0e,0x0f
	*/
	0x99,0x40,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x10,0x99,0x99,0x00,0x50,0x99,0x00,	//Register 00-0F
	0x99,0x00,0x00,0x00,0x00,0x40,0x00,0x70,0x31,0x13,0x31,0x13,0x00,0x00,0x00,0x00,	//Register 10-1F
	0xBC,0xFF,0x00,0x78,0x78,0x00,0x00,0x99,0x60,0x99,0x99,0x40,0xFF,0x00,0xFF,0x00,	//Register 20-2F
	0x20,0x00,0x00,0x99,0x0F,0x99,0x99,0x99,0x00,0x00,0x10,0x10,0x00,0x24,0x99,0x0C,	//Register 30-3F
	0x0C,0x00,0x00,0xCF,0x99,0x99,0x20,0x00,0xFF,0xE4,0x04,0x80,0x13,0x00,0x99,0x05,	//Register 40-4F
	0x00,0x68,0x20,0x99,0xD8,0xE0,0x99,0x99,0x99,0x52,0xF0,0x00,0x50,0x00,0x99,0x99,	//Register 50-5F
	0x99,0x99,0x17,0x2F,0x6F,0x50,0x5C,0x00,0x01,0x99,0x99,0x99,0x99,0x99,0x99,0x99,	//Register 60-6F
	0x18,0x00,0x00,0x01,0x12,0x00,0x02,0x00,0x99,0x99,0x99,0x99,0x99,0xD2,0x99,0x00,	//Register 70-7F
	0x00,0x00,0x00,0x83,0x00,0x00,0x00,0x00,0x99,0x00,0x00,0x00,0x00,0x00,0x00,0x00,	//Register 80-8F
	0x00,0x00,0x00,0x83,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,	//Register 90-9F
	0x00,0x00,0x00,0x83,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x99,0x99,	//Register A0-AF
	0xA0,0x02,0x3D,0x25,0x8B,0x4B,0x3F,0xFF,0xFF,0xFC,0x00,0x00,0x00,0x99,0x99,0x99,	//Register B0-BF
	0x99,0x99,0x99,0x99,0x99,0x99,0x99,0x99,0x99,0x99,0x99,0x99,0x99,0x99,0x99,0x99,	//Register C0-CF
	0x99,0x99,0x99,0x99,0x99,0x99,0x99,0x99,0x99,0x99,0x00,0x99,0x99,0x99,0x99,0x99,	//Register D0-DF
	0x99,0x99,0x99,0x06,0x84,0x04,0x99,0x52,0x99,0x99,0x13,0x99,0x99,0x99,0x99,0x01,	//Register E0-EF
	0x99,0x99,0x99,0x99,0x99,0x99,0x99,0x99,0x99,0x99,0x99,0x99,0x99,0x99,0x00 	      //Register F0-FF
};


static  HUINT16 logtable[490] = {
	0,104,132,149,161,170,179,185,191,196,		// 1
	200,205,208,212,215,218,221,223,226,228,
	230,232,234,236,238,240,242,243,245,246,		// 201
	248,249,251,252,253,255,256,257,258,259,
	260,261,262,263,264,265,266,267,268,269,		// 401
	270,271,272,273,273,274,275,276,276,277,
	278,279,279,280,281,281,282,283,283,284,		// 601
	285,285,286,286,287,288,288,289,289,290,
	290,291,291,292,292,293,294,294,294,295,		// 801
	295,296,296,297,297,298,298,299,299,300,
	300,300,301,301,302,302,303,303,303,304,		//1001
	304,305,305,305,306,306,306,307,307,308,
	308,308,309,309,309,310,310,310,311,311,		//1201
	311,312,312,312,313,313,313,314,314,314,
	315,315,315,316,316,316,316,317,317,317,		//1401
	318,318,318,318,319,319,319,320,320,320,
	320,321,321,321,322,322,322,322,323,323,		//1601
	323,323,324,324,324,324,325,325,325,325,
	326,326,326,326,327,327,327,327,327,328,		//1801
	328,328,328,329,329,329,329,329,330,330,
	330,330,331,331,331,331,331,332,332,332,		//2001
	332,332,333,333,333,333,333,334,334,334,
	334,334,335,335,335,335,335,336,336,336,		//2201
	336,336,337,337,337,337,337,337,338,338,
	338,338,338,339,339,339,339,339,339,340,		//2401
	340,340,340,340,341,341,341,341,341,341,
	342,342,342,342,342,342,343,343,343,343,		//2601
	343,343,343,344,344,344,344,344,344,345,
	345,345,345,345,345,345,346,346,346,346,		//2801
	346,346,347,347,347,347,347,347,347,348,
	348,348,348,348,348,348,349,349,349,349,		// 3001
	349,349,349,350,350,350,350,350,350,350,
	351,351,351,351,351,351,351,351,352,352,		//3201
	352,352,352,352,352,353,353,353,353,353,
	353,353,353,354,354,354,354,354,354,354,		//3401
	354,355,355,355,355,355,355,355,355,356,
	356,356,356,356,356,356,356,356,357,357,		// 3601
	357,357,357,357,357,357,358,358,358,358,
	358,358,358,358,358,359,359,359,359,359,		//3801
	359,359,359,359,360,360,360,360,360,360,
	360,360,360,361,361,361,361,361,361,361,		// 4001
	361,361,362,362,362,362,362,362,362,362,
	362,362,363,363,363,363,363,363,363,363,		// 4201
	363,363,364,364,364,364,364,364,364,364,
	364,364,365,365,365,365,365,365,365,365,		//4401
	365,365,366,366,366,366,366,366,366,366,
	366,366,366,367,367,367,367,367,367,367,		//4601
	367,367,367,367,368,368,368,368,368,368,
	368,368,368,368,368,369,369,369,369,369,		//4801
};

#if defined (CONFIG_PLATFORM_TM201COMBO)
HUINT8	ucTunerRegAddrTable[levelNo] = {
84,83,82,81,80,79,78,77,76,75,
74,73,72,71,70,69,68,67,66,65,
64,63,62,61,60,59,58,57,56,55,
54,53,52,51,50,49,48,47,46,45,
44
};

MxL201RF_TunerConfigS	g_stSemTunerConfigTer[BUS_NUM];
#endif

/****************************************************
	Static Variable
******************************************************/
static HUINT8 tc90517Reg01; /* 01번지 레지스터는 두개의 다른 기능을 포함한다. 각 기능의 설정값을 모두 기억한다. */
//static HUINT8 tc90517Reg71; /* 71번지 레지스터는 두개의 다른 기능을 포함한다. 각 기능의 설정값을 모두 기억한다. */

static HUINT32 beforeId2[BUS_NUM];
static HBOOL	 sTsmfEmgOnFlag[BUS_NUM];
static HBOOL	 sTsmfEmgOffFlag[BUS_NUM];

static HUINT32	totalCntToDisplayBER[BUS_NUM] = {0,0};

static CH_TER_AntStatus_t s_stAntState[MAX_NUM_TER_UNITS];

/****************************************************
	Extern Function
******************************************************/
int Drv_Ch_I2C_TunerRead(int nChannelId, HUINT8 ucRegAddr, HUINT8 *pucBuffer, HUINT32 nNumberToRead);
int Drv_Ch_I2C_TunerWrite(int nChannelId, HUINT8 ucRegAddr, HUINT8 *pucBuffer, HUINT32 nNumberToWrite);

int Drv_Ch_I2C_DemodRead(int nChannelId, HUINT16 ucDevAddr , HUINT8 ucRegAddr, HUINT8 *pucBuffer, HUINT32 nNumberToRead);
int Drv_Ch_I2C_DemodWrite(int nChannelId,  HUINT16 ucDevAddr , HUINT8 ucRegAddr, HUINT8 *pucBuffer, HUINT32 nNumberToWrite);


/****************************************************
	Static Function
******************************************************/
static HUINT8 Set_OFDM_Tuner(int nChannelId, HUINT32 freq);

/* Related - OFDM Demod  Function */
static void GetUnCorrectedPacket(int nChannelId, HUINT32 *unCorPkt);
static void GetUnErroredPacket(int nChannelId, HUINT32 *unErroredPkt);
static HINT16	GetIsdbStrength(int nChannelId);
static HUINT16 GetOfdmCn(int nChannelId);
static void GetIsdbtBer(int nChannelId, HUINT32 *berVal, HUINT32 errCnt, HUINT32 errCycle, HUINT32 *ngFlag);
static int	CheckLock(int nChannelId);
static HUINT8	FastLockIsdb(int nChannelId);
static HUINT8	CheckIsdbAgcLock(int nChannelId);
static float	PowerOfExponential(float mantissa, HUINT8 exponent);
//void		ClearIsdbDemod(int nChannelId);
static void DeactivateIsdbDemod(int nChannelId);
static void ActivateIsdbDemod(int nChannelId);
static void Start_Accu(int nChannelId);
//static void End_Accu(int nChannelId);
static void Reset_Accu(int nChannelId);
//static void ResetSystem(int nChannelId);
//static void ResetDemodulation(HUINT32 nChannelId);
//static void ResetFFTWndSearch(HUINT32 nChannelId);
//static void Set_OperationMode(int nChannelId);
//static void Set_Standby(int nChannelId);
//static void Set_InputOutput(int nChannelId);
//static void Set_Clock(int nChannelId);
//static void Set_ADC_Control(int nChannelId);
//static void Set_AGC_DigitalFilter(int nChannelId);
//static void Set_CarrierRecovery(int nChannelId);
//static void Set_Synchronization(int nChannelId);
//static void Set_CPERemoval(int nChannelId);
//static void Set_CVI_CSI(int nChannelId);
static void Set_Equalization(int nChannelId);
//static void Set_TMCCDetection(int nChannelId);
static void Set_ErrorCorrection(int nChannelId);
//static void Set_ErrorCorrection4ChPerformanceTest(int nChannelId);
//static void Set_TMCCDecodedData(int nChannelId);
static int	DT90517_SetTuner(int nChannelId, HUINT32 frequency);
static int	DT90517_GetStatus(int nChannelId);
static int DT90517_Lock( int nChannelId, unsigned char *bChecklock );

// for test
void 	View_Accu(int nChannelId);
void		ViewOfdmParam(int nChannelId);
void		ViewDemodStatus(int nChannelId);
HUINT8	ViewFrameLock(int nChannelId);
HUINT8	ViewTmccDetect(int nChannelId);

static HINT16 New_GetSignalLevel(HUINT32  frequency, HINT16 rfagc, HINT16 ifagc);
static HINT16 New_CalIndex(HINT16 rfagc, HINT16 ifagc, HUINT16 *AgcTable, HINT16 *offset);
static HINT16 New_ReadSignalLevel(HINT16 rfPwrIndex,  HINT16 offset);

/*******************************************************************************
* function: Drv_Ch_I2C_DemodRead
* input:
*	- nChannelId : 버스 번호
* output: No
* return: No
*******************************************************************************/
int Drv_Ch_I2C_DemodRead(int nChannelId, HUINT16 ucDevAddr , HUINT8 ucRegAddr, HUINT8 *pucBuffer, HUINT32 nNumberToRead)
{
	int nResult = DI_CH_OK;
	DRV_Error nRet = DRV_OK;
	HUINT32 ucI2CBus =0;
	HUINT16	ucDemodAddr = 0;

#if defined(CONFIG_PLATFORM_TM201COMBO)
	HINT32	nUnitId;
	CH_UNUSED(ucDevAddr);

	nUnitId = GetDeviceUnitId( nChannelId );
	if(nChannelId == MASTER_TER_DEMOD)
	{
		ucI2CBus = MASTER_I2C_BUS;
		ucDemodAddr = MASTER_DEMOD_ADDR_TC90517;
	}
	else if(nChannelId == SLAVE_TER_DEMOD)
	{
		ucI2CBus = SLAVE_I2C_BUS;
		ucDemodAddr = SLAVE_DEMOD_ADDR_TC90517;
	}
	else
	{
		CH_DI_Print(0, ("[Drv_Ch_I2C_DemodRead] ERR! Wrong Unit ID \n"));
	}

	if(ucRegAddr > TC90517_FE)
	{
		/* Reg Address Error */
		CH_DI_Print(0, ("[Drv_Ch_I2C_DemodRead] ERR! Reg Address \n"));
	}
#else
	CH_UNUSED(nChannelId);

	if(ucDevAddr == DEMOD_TC90517_ADDRESS)
	{
		ucI2CBus = 0;

		if(ucRegAddr > TC90517_FE)
		{
			/* Reg Address Error */
			CH_DI_Print(0, ("[Drv_Ch_I2C_DemodRead] ERR! Reg Address \n"));
		}
	}
	else if((ucDevAddr == QAM_DEMOD_ADDRESS) || (ucDevAddr == JQAM_DEMOD_ADDRESS))
	{
		ucI2CBus = 2;
	}
	ucDemodAddr = ucDevAddr;
#endif

	nRet = DRV_I2c_ReadA8( ucI2CBus, ucDemodAddr, ucRegAddr, pucBuffer, nNumberToRead );
	if ( nRet != DRV_OK )
	{
		CH_DI_Print(0, ("[Drv_Ch_I2C_DemodRead] ERR! nRet [0x%x] \n", nRet));
		nResult = DI_CH_ERR;
	}
	else
	{
		//CH_DI_Print("[0x%x] RegAddr [0x%x] RData [0x%x] \n", ucDemodAddr, ucRegAddr, *pucBuffer );
	}

	return nResult;

}


int Drv_Ch_I2C_DemodWrite(int nChannelId,  HUINT16 ucDevAddr , HUINT8 ucRegAddr, HUINT8 *pucBuffer, HUINT32 nNumberToWrite)
{
	int nResult = DI_CH_OK;
	DRV_Error nRet = DRV_OK;
	HUINT32	ucI2CBus=0;

#if defined(CONFIG_PLATFORM_TM201COMBO)
	HINT32	nUnitId;

	nUnitId = GetDeviceUnitId( nChannelId );
	if(nChannelId == MASTER_TER_DEMOD)
	{
		ucI2CBus = MASTER_I2C_BUS;
		ucDevAddr = MASTER_DEMOD_ADDR_TC90517;
		if(ucRegAddr > TC90517_FE)
		{
			/* Reg Address Error */
			CH_DI_Print(0, ("[Drv_Ch_I2C_DemodWrite] ERR! Reg Address \n"));
		}
	}
	else if(nChannelId == SLAVE_TER_DEMOD)
	{
		ucI2CBus = SLAVE_I2C_BUS;
		ucDevAddr = SLAVE_DEMOD_ADDR_TC90517;
		if(ucRegAddr > TC90517_FE)
		{
			/* Reg Address Error */
			CH_DI_Print(0, ("[Drv_Ch_I2C_DemodWrite] ERR! Reg Address \n"));
		}
	}
	else
	{
		CH_DI_Print(0, ("[Drv_Ch_I2C_DemodWrite] ERR! Wrong Unit ID \n"));
	}
#else
	CH_UNUSED(nChannelId);

	if(ucDevAddr == DEMOD_TC90517_ADDRESS)
	{
		ucI2CBus = 0;

		if(ucRegAddr > TC90517_FE)
		{
			/* Reg Address Error */
			CH_DI_Print(0, ("[Drv_Ch_I2C_DemodWrite] ERR! Reg Address \n"));
		}
	}
	else if((ucDevAddr == QAM_DEMOD_ADDRESS) || (ucDevAddr == JQAM_DEMOD_ADDRESS))
	{
		ucI2CBus = 2;
	}
#endif
	nRet = DRV_I2c_WriteA8(ucI2CBus, ucDevAddr, ucRegAddr, pucBuffer, nNumberToWrite);
	if ( nRet != DRV_OK )
	{
		CH_DI_Print(0, ("[Drv_Ch_I2C_DemodWrite] ERR! nRet [0x%x] \n", nRet));
		nResult = DI_CH_ERR;
	}
	else
	{
		//CH_DI_Print(" Success \n");
		//CH_DI_Print("[0x%x] RegAddr [0x%x] WData [0x%x] \n", ucDevAddr, ucRegAddr, *pucBuffer );
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
int Drv_Ch_I2C_TunerRead(int nChannelId, HUINT8 ucRegAddr, HUINT8 *pucBuffer, HUINT32 nNumberToRead)
{
#if defined(CONFIG_PLATFORM_TM201COMBO)
	HUINT32 				nRet = 0;

	CH_UNUSED(nNumberToRead);

	nRet = MxL_I2C_Read(&g_stSemTunerConfigTer[nChannelId], ucRegAddr, pucBuffer);

	return nRet;
#else
	int nResult = DI_CH_OK;
	DRV_Error nRet = DRV_OK;
	HUINT16	ucDevAddr = DEMOD_TC90517_ADDRESS;
	HUINT32	ucI2CBus = 0;

	HUINT16	ucThruRegAddr = 0xfec1;

	CH_UNUSED(nChannelId);

	if(ucRegAddr != TUNER_DNVS22CX_ADDRESS)
	{
		/* Reg Address Error */
		CH_DI_Print(0,  ("[Drv_Ch_I2C_TunerRead] ERR! Reg Address \n"));
	}
	else
	{
		//CH_DI_Print("  [0x%x] \n", (HUINT16)ucThruRegAddr );
	}

	nRet = DRV_I2c_ReadA16( ucI2CBus, ucDevAddr, ucThruRegAddr, pucBuffer, nNumberToRead );
	if ( nRet != DRV_OK )
	{
		CH_DI_Print(0, ("[Drv_Ch_I2C_TunerRead] ERR! nRet [0x%x] \n", nRet));
		nResult = DI_CH_ERR;
	}
	else
	{
		CH_DI_Print(3, ("[Drv_Ch_I2C_TunerRead] ucReadVal [0x%x]\n", *pucBuffer));
	}

	return nResult;
#endif

#if 0
	int nResult = DI_CH_OK;
	DRV_Error nRet = DRV_OK;
	HUINT16	ucDevAddr = TC90517_DEMOD_ADDRESS;
	HUINT32	ucI2CBus = 0;

	HUINT8	tnflg = 0xfe;	/* tnflg : 1111 1110 */

	HUINT8	ucThruRegAddr = 0xc3;


	CH_UNUSED(nChannelId);

	if(ucRegAddr != ISDBT_TD1686_ADDRESS)
	{
		/* Reg Address Error */
		CH_DI_Print(0,  (" Reg Address Error!n");
	}
	else
	{
		//CH_DI_Print("  [0x%x] \n", (HUINT16)ucThruRegAddr );
	}

	nRet = DRV_I2c_WriteA8( ucI2CBus, ucDevAddr, tnflg, &ucThruRegAddr, nNumberToRead );
	if ( nRet != DRV_OK )
	{
		CH_DI_Print(0, (" Error[0x%x]", nRet);
		nResult = DI_CH_ERR;
	}

	nRet = DRV_I2c_ReadNoAddr( ucI2CBus, ucDevAddr, pucBuffer, nNumberToRead );
	if ( nRet != DRV_OK )
	{
		CH_DI_Print(0, (" Error[0x%x]", nRet);
		nResult = DI_CH_ERR;
	}
	else
	{
		//CH_DI_Print(" ucReadVal [0x%x]\n", *pucBuffer);
	}

	return nResult;

#endif
}
int Drv_Ch_I2C_TunerWrite(int nChannelId,  HUINT8 ucRegAddr, HUINT8 *pucBuffer, HUINT32 nNumberToWrite)
{
#if defined(CONFIG_PLATFORM_TM201COMBO)
	HUINT32 				nRet = 0;
	HUINT8					addr_data[2];

	addr_data[0] = ucRegAddr;
	addr_data[1] = *pucBuffer;

	nRet = MxL_I2C_Write(&g_stSemTunerConfigTer[nChannelId], addr_data, nNumberToWrite);

	return nRet;
#else
	int nResult = DI_CH_OK;
	DRV_Error nRet = DRV_OK;

	HUINT16	ucDevAddr = DEMOD_TC90517_ADDRESS;
	HUINT32	ucI2CBus = 0;

	HUINT16	ucThruRegAddr = 0xfec0;

	//int i=0;

	CH_UNUSED(nChannelId);

	if(ucRegAddr != TUNER_DNVS22CX_ADDRESS)
	{
		/* Reg Address Error */
		CH_DI_Print(0,  ("[Drv_Ch_I2C_TunerWrite] ERR! Reg Address \n"));
	}
	else
	{
		/* Make 16bit Register Address (tnflg + Target Address) */
		//ucRegAddr = (HUINT16)0xfec2;//((((HUINT16)tnflg << 8) & 0xff00) | (ucRegAddr & 0x00ff));
		CH_DI_Print(5, ("[Drv_Ch_I2C_TunerWrite] [0x%04x] \n",(HUINT16)ucThruRegAddr ));
	}

	/*
	for (i=0; i<nNumberToWrite; i++)
		CH_DI_Print("[0x%x] \n", *(pucBuffer+i) );
	*/

	nRet = DRV_I2c_WriteA16(ucI2CBus, ucDevAddr, ucThruRegAddr, pucBuffer, nNumberToWrite);
	if ( nRet != DRV_OK )
	{
		CH_DI_Print(0, ("[Drv_Ch_I2C_TunerWrite] ERR! nRet [0x%x] \n", nRet));
		nResult = DI_CH_ERR;
	}
	else
	{
		CH_DI_Print(5, ("[Drv_Ch_I2C_TunerWrite] Success \n"));
	}

	return nResult;

#endif

#if 0
	int nResult = DI_CH_OK;
	DRV_Error nRet = DRV_OK;

	HUINT16	ucDevAddr = TC90517_DEMOD_ADDRESS;
	HUINT32	ucI2CBus = 0;

	HUINT8	tnflg = 0xfe;	/* tnflg : 1111 1110 */

	HUINT16	ucThruRegAddr = 0x60;

	HUINT8	ucBuffer[5];
	HUINT32 i, nNumWrite = 5;

	CH_UNUSED(nChannelId);


	nRet = DRV_I2c_WriteNoAck( ucI2CBus, ucDevAddr, tnflg, &ucThruRegAddr, 1 );
	//nRet = DRV_I2c_WriteA8( ucI2CBus, ucDevAddr, tnflg, &ucThruRegAddr, 1 );

	nRet = DRV_I2c_WriteNoAddr(ucI2CBus, ISDBT_TD1686_ADDRESS, pucBuffer, nNumberToWrite);
	if ( nRet != DRV_OK )
	{
		CH_DI_Print(0, (" Error[0x%x]", nRet);
		nResult = DI_CH_ERR;
	}
	else
	{
		CH_DI_Print(" Success \n");
	}

	return nResult;

#endif
}

void DT90517_InitRegister(HUINT32 nChannelId)
{
	int i;
	HUINT8	val;

	CH_DI_Print (6, ("DT90517_InitRegister[%d]\n",nChannelId));

	for(i=0; i<TC90517_FE; i++)
	{
		if(gDT90517InitTbl[i] != 0x99) /* 0x99 :Invalid register */
		{
			val = gDT90517InitTbl[i];
			CH_DI_Print(5,("DT90517_Register_Init[%d] :: gDT90517InitTbl[%x]=[%x]\n",nChannelId,i, val));
			Drv_Ch_I2C_DemodWrite(nChannelId, DEMOD_TC90517_ADDRESS, i, &val, 1);
		}
	}
}

int DT90517_DumpRegister( int nChannelId)
{
	int i=0;
	unsigned char ucdata;

	//nUnitId = GetDeviceUnitId( nChannelId );

	CH_DI_Print(0, ("===================================================\n"));
	CH_DI_Print(0, ("   TC90517 Demod [%d] Dump\n", nChannelId));
	CH_DI_Print(0, ("===================================================\n"));

	for(i=0; i<= TC90517_FE; i++)
	{
		Drv_Ch_I2C_DemodRead(nChannelId, DEMOD_TC90517_ADDRESS,  i, &ucdata, 1);
		CH_DI_Print(0, ("[0x%02X] \t[0x%02x]\n", i, ucdata));
	}

	CH_DI_Print(0, ("===================================================\n"));
	CH_DI_Print(0, ("   OFDM TUNER [%d] Dump\n", nChannelId));
	CH_DI_Print(0, ("===================================================\n"));

#if defined(CONFIG_PLATFORM_TM201COMBO)
	for(i=0; i<= NUM_OF_MXLTUNER_REG; i++)
#endif
	{
		Drv_Ch_I2C_TunerRead(nChannelId,  (HUINT8)i, &ucdata, 1);
		CH_DI_Print(0, ("[0x%02x]\n", ucdata));
	}
	CH_DI_Print(0, ("===================================================\n"));

	return DI_CH_OK;

}

#if 0
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

	isyrst |= tc90517Reg01;
	Drv_Ch_I2C_DemodWrite(nChannelId, DEMOD_TC90517_ADDRESS, TC90517_01, &isyrst, 1);
}
#endif

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

	imsrst |= tc90517Reg01;
	Drv_Ch_I2C_DemodWrite(nChannelId, DEMOD_TC90517_ADDRESS, TC90517_01, &imsrst, 1);
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

	iwsrst |= tc90517Reg01;
	Drv_Ch_I2C_DemodWrite(nChannelId, DEMOD_TC90517_ADDRESS, TC90517_01, &iwsrst, 1);
}

#if 0
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

	tc90517Reg71 |= laysel;

	Drv_Ch_I2C_DemodWrite(nChannelId, DEMOD_TC90517_ADDRESS,  TC90517_02, &regVal, 1);
	Drv_Ch_I2C_DemodWrite(nChannelId, DEMOD_TC90517_ADDRESS,  TC90517_EC, &recvmdsel, 1);
	Drv_Ch_I2C_DemodWrite(nChannelId, DEMOD_TC90517_ADDRESS,  TC90517_71, &tc90517Reg71, 1);
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
	HUINT8	slplkmon = 0x0;		/* slplkmon[0x01, D2] */
	HUINT8	slptim = 0x0;			/* slptim[0x03, D6:D4] */
	HUINT8	wuptim = 0x0;		/* wuptim[0x03, D3:D0] */
	//HUINT8	adcpwn = 0x0;	/* adcpwn[0x04, D5] */
	HUINT8	regVal;

	regVal = ((slpmd << 3) |
			(slplkmon << 2));
	Drv_Ch_I2C_DemodWrite(nChannelId, DEMOD_TC90517_ADDRESS,  TC90517_01, &regVal, 1);

	regVal = ((slptim << 4) |
			wuptim);
	Drv_Ch_I2C_DemodWrite(nChannelId, DEMOD_TC90517_ADDRESS,  TC90517_03, &regVal, 1);

#if defined (PLATFORM_DT90507)
	regVal = (adcpwn << 5);
	Drv_Ch_I2C_DemodWrite(nChannelId, DEMOD_TC90507_ADDRESS,  TC90507_04, &regVal, 1);
#endif
}

void Set_InputOutput(int nChannelId)
{
	HUINT8	regVal;
	HUINT8	isdbt[4];

	/* TS input setting */
	/*   [05] stdisa : STSFLG1 output setting
			[7:0]= retryov,alalrm,tmunvld,mdunvld, 'sulock', vulock,rulock,rseorf
			'0': Disable, '1': Enable
		[06] stdisb : STSFLG0 output setting
			[7:3]= emerg,tmcchg,cdunvld,slpen,STSFLG1
		[07] STSFLG1 output polarity
			[7:0]='0': Normal, '1': Invert
		[08] STSFLG0 output polarity
			[7:3]='0': Normal, '1': Invert  */

		isdbt[0]=0x00;  /*[05]*/
		isdbt[1]=0x00;  /*[06]*/
		isdbt[2]=0x00;  /*[07]*/
		isdbt[3]=0x00;  /*[08]*/
		Drv_Ch_I2C_DemodWrite(nChannelId, DEMOD_TC90517_ADDRESS,  TC90517_05, &isdbt[0], 4);

#if defined (PLATFORM_DT90517)
	/* SLADRS pin output setting */
	regVal = 0x50; /*[0D]
				   [7:6] SLADRS1 output
				   	    0: Enable,  1: High-Z, 2: Fixed '0', 3: Fixed '1'
				   [5:4] SLADRS0 output
				   	    0: Enable,  1: High-Z, 2: Fixed '0', 3: Fixed '1' */

	Drv_Ch_I2C_DemodWrite(nChannelId, DEMOD_TC90517_ADDRESS,  TC90517_0D, &regVal, 1);

	 /* TS mode selection */
	regVal = 0x00; /*[0F]
				   [7:2] disalbe 0
				   [1:0] pinsla: mode selection setting,
				   	    0: normal (seiral) TS mode, 2: Parallel TS mode */

	Drv_Ch_I2C_DemodWrite(nChannelId, DEMOD_TC90517_ADDRESS,  TC90517_0F, &regVal, 1);
#endif

	/* TS output setting */
	#if(1)
	CH_DI_Print("Set_InputOutput:: Serial Output \n");
	/* Serial */
		isdbt[0]=0x00; /* [1C]
					   0000 0000
					   AGCCNTI[7:6] Enable,
					   AGCCNTR[5:4] Enable,
					   STSFLG0[3:2] Fixed to 0,
					   STSFLG1[1:0] Fixed to 0   */

		   isdbt[1]=0xa1; /* [1D]
						  1010 0001
						  rlockoen [7:6] Fixed to 0 : RLOCK signal output
						  rerroen [5:4] Fixed to 0	: RERR signal output
						  disable [3:2] 0
						  slpout [1]  0:SLOCK, 1: SLPEN
						  hselout [0] 0: RLOCK+RERR, 1:HSEL 	 */

		isdbt[2]=0x00; /* [1E]
					   0000 0000 :
					   slockoen[7:6] Enable, SLOCK signal output
					   srckoen [5:4] Enable, SRCK signal output
					   srdtoen [3:2] Enable, SRDT signal output
					   disable [1:0] 0  			*/

		isdbt[3]=0x00; /*[1F]
					0000 0000 :
					stbyteoen[7:6] Enable, SBYTE signal output
					pbvaloen[5:4] Enable, PBVAL signal output
					rseorfoen[3:2] Enable, RSEORF signal output	*/
	#else
	/* Parallel */
		isdbt[0]=0x08;  /*[1C]*/
						/* 0000 1000 :
						   AGCCNTI[7:6] Enable,
						   AGCCNTR[5:4] Enable,
						   STSFLG0[3:2] Fixed to 0,
						   STSFLG1[1:0] Enable   */

		isdbt[1]=0xa0;  /*[1D]*/
						/* 1010 0000 :
						   rlockoen [7:6] Fixed to 0, (?) RLOCK signal output
						   rerroen [5:4] Fixed to 0,  (?) RERR signal output
						   disable [3:2] 0
						   slpout [1] Switching between SLOCK and SLPEN,  (0:SLOCK, 1: SLPEN)
						   hselout [0] Switching between RLOCK+RERR and HSEL (0: RLOCK, RERR, 1:HSEL)	*/

		isdbt[2]=0x28;  /*[1E]*/
						/* 0010 1000 :
						  slockoen[7:6] Enable, SLOCK signal output
						  srckoen [5:4] Fixed to 0 (?) SRCK signal output  // TC90507) SLPEN[5:4] Enable,
						  srdtoen [3:2] Fixed to 0 (?) SRDT signal output  // TC90507) SRCK/SRDT[3:2] Fixed to 0,
						  disable [1:0] 0  // TC90507) RSCKO[1:0] Enable				*/

		isdbt[3]=0x00;  /*[1F]*/
					   /* 0000 0000 :
						stbyteoen[7:6] Enable,
						pbvaloen[5:4] Enable,
						rseorfoen[3:2] Enable	*/
	#endif
		Drv_Ch_I2C_DemodWrite(nChannelId, DEMOD_TC90517_ADDRESS, TC90517_1C, &isdbt[0], 4);
}

void Set_Clock(int nChannelId)
{
	HUINT8	isdbt[2];
	HUINT8	regVal = 0;
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

#if defined (PLATFORM_DT90517)
	/* [필수] 클럭 위상 조정  */

	regVal =0x10;	 /*[09]
				     [4] tdif25s: Clock phase adjustment
				      0: continuous,  1: hold */
	Drv_Ch_I2C_DemodWrite(nChannelId, DEMOD_TC90517_ADDRESS, TC90517_09, &regVal, 1);
#endif

	isdbt[0]=0x21;	  // PLL setting
	isdbt[1]=0x0b;
	Drv_Ch_I2C_DemodWrite(nChannelId, DEMOD_TC90517_ADDRESS, TC90517_11, &isdbt[0], 2);

	if( HKFRQ == 1)
	{
		isdbt[0]=0x2c;
		isdbt[1]= ghkfrq;
	}
	else
	{
		isdbt[0]=0x2c;
		isdbt[1]=0x22; // justin@humax
	}
	Drv_Ch_I2C_DemodWrite(nChannelId, DEMOD_TC90517_ADDRESS, TC90517_13, &isdbt[0], 2);

	isdbt[0]=0x40;
#if defined (PLATFORM_DT90507)
	isdbt[1]=0x02; /*[16]*/
#else
	isdbt[1]=0x03; /*[16]
			            [5] difpll: Direct IF PLL mode
			             	   0: Direct IF(25MHz) ADC clock is crystal-oscillated
			             	   1: Direct IF(4MHz) ADC clock is PLL-oscillated

 				     [3] adciqin: ADC I/Q mode
					   0: IF input
					   1: IQ input
 				     [2] zifmd: I/Q mode
					   0: IF input
					   1: IQ input
 				     [1] dif25m: Direct IF (25M) selection
					   0: other than direct IF
					   1: Direct IF (25M)
 				     [0] dif25v: Direct IF selection
					   0: low IF
					   1: Direct IF		             	  */
	Drv_Ch_I2C_DemodWrite(nChannelId, DEMOD_TC90517_ADDRESS,  TC90517_15, &isdbt[0], 2);
#endif

#if defined (PLATFORM_DT90517)
	regVal =0x01;	 /*[04]
				     [0] ixosl: XO output in external reference clock mode
				      0: XO output ON ,  1: XO output OFF */
	Drv_Ch_I2C_DemodWrite(nChannelId, DEMOD_TC90517_ADDRESS, TC90517_04, &regVal, 1);
#endif
}

#if defined(PLATFORM_DT90517)
void Set_ADC_Control(int nChannelId)
{
	HUINT8	val;
	HUINT8	isdbt[2];

/*	val = 0x00;  [5B]
				 0: except 1 segment reception including partial reception
				 1: 1 segment reception including partial reception
	Drv_Ch_I2C_DemodWrite(nChannelId,	TC90517_5B, &val, 1);
*/

	/* AD 전류 설정 */
	isdbt[0]=0x50; /*[65] (추천) */
	isdbt[1]=0x5c; /*[66] (추천) */
	Drv_Ch_I2C_DemodWrite(nChannelId, DEMOD_TC90517_ADDRESS, TC90517_65, &isdbt[0], 2);

	/* AD 적응 제어 설정 */
	val = 0x01;  /*[68] (추천)
				 iaccmd: sets the AD adaptive current control ON*/
	Drv_Ch_I2C_DemodWrite(nChannelId,	DEMOD_TC90517_ADDRESS, TC90517_68, &val, 1);

}
#endif

void Set_CarrierRecovery(int nChannelId)
{
	HUINT8	val;

	//Drv_Ch_I2C_DemodWrite(TC90517_30,0x28,1);
	/* frequency IFlow=4.063492 MHz
	Frequency correction ＝(Master clock frequency ＭＤ÷１６)－ＩＦｌｏｗ［Ｈｚ］
	If I/Q detection frequency inversion register ｆ_ｉｎｖ＝０,
	ｃｐｌｄ_ｄｔ＝Frequency correction volume÷９９２ */
	//Drv_Ch_I2C_DemodWrite(TC90517_CARROFF1,0x00,1);
	/* Frequency correction=(69.868MHz/16)-4.063492MHz
	ｃｐｌｄ_ｄｔ＝ 303258/992=306=0x0131*/
	//Drv_Ch_I2C_DemodWrite(TC90517_CARROFF2,0xea,1);

	if(AFC30 == 1)
	{
		val=gafc;
	}
	else
	{
		val= 0x20;
	}
	Drv_Ch_I2C_DemodWrite(nChannelId, DEMOD_TC90517_ADDRESS,  TC90517_30, &val, 1);

	val=0x11; // justin@humax
	Drv_Ch_I2C_DemodWrite(nChannelId, DEMOD_TC90517_ADDRESS, TC90517_31, &val, 1);

	if(AFC32 == 1)
	{
		val=gafc32;  // cpld_dt
	}
	else
	{
		val= 0x2B; // justin@humax
	}
	Drv_Ch_I2C_DemodWrite(nChannelId,	DEMOD_TC90517_ADDRESS, TC90517_32, &val, 1);

	val=0x01; // 0x0d;
	Drv_Ch_I2C_DemodWrite(nChannelId,	DEMOD_TC90517_ADDRESS, TC90517_38, &val, 1);

	if(AFC39 == 1)
	{
		val=gafc39;
	}
	else
	{
		val= 0x70; // justin_070115 0x5d;	// default 0x70
	}
	Drv_Ch_I2C_DemodWrite(nChannelId,	DEMOD_TC90517_ADDRESS, TC90517_39, &val, 1);
}

void Set_Synchronization(int nChannelId)
{
	HUINT8	isdbt[6];
	HUINT8	val;

	/* initial waiting time until demod start is set*/
	isdbt[0]=0x10; /*[3A]*/
	isdbt[1]=0x10; /*[3B]*/
	isdbt[2]=0x00; /*[3C]*/
	isdbt[3]=0x10; /*[3D]*/
	Drv_Ch_I2C_DemodWrite(nChannelId, DEMOD_TC90517_ADDRESS, TC90517_3A, &isdbt[0], 4);

	//isdbt[4]=0x08; /*[3E]*/
	isdbt[5]=0x0c; /*[3F]*/
	isdbt[6]=0x0c; /*[40]*/
	isdbt[7]=0x00; /*[41]*/
	isdbt[8]=0x00; /*[42]*/
	Drv_Ch_I2C_DemodWrite(nChannelId, DEMOD_TC90517_ADDRESS, TC90517_3F, &isdbt[0], 4);

	isdbt[0]=0xcf; /*[43]*/
	/*isdbt[1]=0xff;*/
	Drv_Ch_I2C_DemodWrite(nChannelId, DEMOD_TC90517_ADDRESS, TC90517_43, &isdbt[0], 1);

	isdbt[0]=0x20; /*[46]*/
	isdbt[1]=0x00; /*[47]
					[7] mdetsel: 0: mode search ON at initial pull-in,  1: mode search OFF
					[6] mlocksel: 0: synchronization sequence completion (seqen >= 8), 1: channel search completion flag
					[5] mlockmd: 0: SP detection completion flag,  1: Guard correlation detection completion flag
					[4] mdsmd: trnasmission mode search 0: high-speed (5 mode )search, 1: 12 modes
					[3:0] disable 0 */

	isdbt[2]=0xff; /*[48]
				   C/N error decision threshold value */

	isdbt[3]=0xe4; /*[49]
				   [0] carrier AFC pull-in range */

	isdbt[4]=0x04; /*[4A] (성능 개선)
				   [3] FFT window searh limit setting
				   0: woffset unit (the 1seg/3seg mode is fixed when wslim_off =0)
				   1: FFT clock unit */

	isdbt[5]=0x80; /*[4B]*/
	Drv_Ch_I2C_DemodWrite(nChannelId, DEMOD_TC90517_ADDRESS, TC90517_46, &isdbt[0], 6);

	val=0x20; /*[B1]*/
	Drv_Ch_I2C_DemodWrite(nChannelId, DEMOD_TC90517_ADDRESS, TC90517_B1, &val, 1);

	val=0x00; /*[BC]*/
	Drv_Ch_I2C_DemodWrite(nChannelId, DEMOD_TC90517_ADDRESS, TC90517_BC, &val, 1);

	val=0x84; /*[E4]
			[7:4] schnum: FFT window continuous search one-step time interval
			[3:0] initnum: FFT window continuous search one=step time interval (pull-in) */

	Drv_Ch_I2C_DemodWrite(nChannelId,	DEMOD_TC90517_ADDRESS, TC90517_E4, &val, 1);

#if defined (PLATFORM_DT90517)
	/* FFT window search 검출 역치 설정 */
	val=0x04; /*[E5]  (필수)
			     [2:1] ws_clpsl: FFT window continuous search detection threshold value
				      0:Normal,  1:One half,  2: 4 times,  3:2times*/

	Drv_Ch_I2C_DemodWrite(nChannelId,	DEMOD_TC90517_ADDRESS,  TC90517_E5, &val, 1);

	/* FFT 출력 레벨의 AGC제어 */
	val=0x58;   /*[E0]  (성능 개선) (?)
			           [7] lvl_off: FFT output level control
				          0: ON, 1:OFF */

	Drv_Ch_I2C_DemodWrite(nChannelId,	DEMOD_TC90517_ADDRESS, TC90517_E0, &val, 1);

	/* ISIC guard내 지연파 검출 설정 */
	val=0xd2;  /*[7D]  (필수)
				[7] gripks: ISIC multipaths in inside of guard interval detection setting */

	Drv_Ch_I2C_DemodWrite(nChannelId,	DEMOD_TC90517_ADDRESS, TC90517_7D, &val, 1);

	/* ISIC 제어 설정 */
	val=0x01;  /*[EF]  (성능 개선)
				[1:0] sydfmd: ISIC/continuous FFT window search switch setting
					 0: ISIC OFF (only FFT window continuous control
					 1: ISIC ON
					 3: Disable */

	Drv_Ch_I2C_DemodWrite(nChannelId, DEMOD_TC90517_ADDRESS, TC90517_EF, &val, 1);
#endif
}


void Set_CPERemoval(int nChannelId)
{
	HUINT8 val = 0x00;

	Drv_Ch_I2C_DemodWrite(nChannelId, DEMOD_TC90517_ADDRESS, TC90517_50, &val, 1);
}

void Set_CVI_CSI(int nChannelId)
{
	HUINT8	isdbt[5];
	HUINT8	val;

	val=0x68;
	Drv_Ch_I2C_DemodWrite(nChannelId, DEMOD_TC90517_ADDRESS, TC90517_51, &val, 1);

	val=0x20;
	Drv_Ch_I2C_DemodWrite(nChannelId, DEMOD_TC90517_ADDRESS, TC90517_52, &val, 1);

#if defined (PLATFORM_DT90507)
	isdbt[0]=0x57;  /*[54]*/
	isdbt[1]=0xf1;  /*[55]*/
	isdbt[2]=0x20;  /*[56]*/
	isdbt[3]=0x70;  /*[57]*/
	isdbt[4]=0x60;  /*[58]*/  // ssen 0x20;
	Drv_Ch_I2C_DemodWrite(nChannelId, TC90507_54, &isdbt[0], 5);
#else
//	defined (PLATFORM_DT90517)
	isdbt[0]=0xd8;  /*[54] (성능 개선)
					ofsd: fading detection offset setting */

	isdbt[1]=0xe0;  /*[55] (성능 개선)
					scld: fading detection inclination setting */
	Drv_Ch_I2C_DemodWrite(nChannelId, DEMOD_TC90517_ADDRESS, TC90517_54, &isdbt[0], 2);

	val=0x52;	/*[59] (성능 개선)
				nmoff: nearness fading mode setting */
	Drv_Ch_I2C_DemodWrite(nChannelId, DEMOD_TC90517_ADDRESS, TC90517_59, &val, 1);

#endif
}

void Set_TMCCDetection(int nChannelId)
{
	HUINT8	val=0x00;

	Drv_Ch_I2C_DemodWrite(nChannelId, DEMOD_TC90517_ADDRESS, TC90517_5D, &val, 1);
}

void Set_ErrorCorrection4ChPerformanceTest(int nChannelId)
{
	HUINT8	isdbt[3];

	isdbt[0]=0x18;
	isdbt[1]=0x00;
	isdbt[2]=0x00;
	Drv_Ch_I2C_DemodWrite(nChannelId, DEMOD_TC90517_ADDRESS, TC90517_70, &isdbt[0], 3);

	isdbt[0]=0x00;
	isdbt[1]=0x32; // ssen 0x38;	// default 0x02 cyc : 3 => 16frames
	isdbt[2]=0x03;					//	Measurement cycle setting : Time mode, After Viterbi
	Drv_Ch_I2C_DemodWrite(nChannelId, DEMOD_TC90517_ADDRESS, TC90517_75, &isdbt[0], 3);
}
#endif

void Set_AGC_DigitalFilter(int nChannelId)
{
	HUINT8	isdbt[7];
	HUINT8	val;

#if defined (CONFIG_INTERNAL_AGC)
	isdbt[0] = 0x00;  	//[0x20] Delay point setting
	isdbt[1] = 0xff;	// [0x21] RF AGC MAX lebel
	isdbt[2] = 0x80;	// [0x22] Set IF only mode
	isdbt[3] = 0x4c;	// [0x23] IF agc agin & polarity
	isdbt[4] = 0x4c;	// [0x24] RF agc gain & polarity, manual agc default 0x4c
	isdbt[5] = 0x00;	// [0x25] if gain manual   ssen 0x4c
	isdbt[6] = 0x00;	// [0x26] rf gain manual
#else



	if(gucTERTopOn == 1)
	{
		isdbt[0] = gucIfMinT;  /* [0x20] Delay point setting(IF AGC MIN)	*/
		isdbt[1] = gucRfMaxT;	/* [0x21] RF AGC MAX level */
	}
	else
	{
		/* [0x20] Delay point setting	 (IF AGC MIN) */
#if defined(CONFIG_PLATFORM_TM201COMBO)
		isdbt[0] = 0x00;
#else
/*	에이와 국사 화면깨짐 문제로 수정  	isdbt[0] = 0x32;	*/
		isdbt[0] = 0x50;
#endif
		isdbt[1] = 0xff;	/* [0x21] RF AGC MAX level */
	}
#if defined(CONFIG_PLATFORM_TM201COMBO)
	isdbt[2]=0x8f;	/* [22]
					 rfif [7]: 0: dual(RF/IF) agc, 1: IF_AGC control mode / 0f - dual()
					 agcthr [4]: AGC passthrough on/off switching 0: OFF, 1: ON */
#else
	isdbt[2]=0x0f;	/* [22]
					 rfif [7]: 0: dual(RF/IF) agc, 1: IF_AGC control mode / 0f - dual()
					 agcthr [4]: AGC passthrough on/off switching 0: OFF, 1: ON */
#endif

	isdbt[3]=0x4c;	/* [23]
					IF agc agin & polarity */

	isdbt[4]=0x4c;	/* [24]
					RF agc gain & polarity, manual agc default 0x4c */

	isdbt[5]=0x00;	/* [25]
					IF gain manual setting 00:minimum, ff:maximum */

	isdbt[6]=0x00;	/* [26]
					RF gain manual */
#endif
	Drv_Ch_I2C_DemodWrite(nChannelId,	DEMOD_TC90517_ADDRESS, TC90517_20, &isdbt[0], 7);

	val = 0x60; 	/* [28]
				ifthd: IF agc gain switching level setting  */

	Drv_Ch_I2C_DemodWrite(nChannelId, DEMOD_TC90517_ADDRESS, TC90517_28, &val, 1);

	val = 0x40; 	/* [2B]
				rfthd: RF agc gain switching level setting */

	Drv_Ch_I2C_DemodWrite(nChannelId,	DEMOD_TC90517_ADDRESS, TC90517_2B, &val, 1);

	if(gucTERTopOn == 1)
	{
		isdbt[0]=gucIfMaxT;  /* [2E] IF Max */
		isdbt[1]=gucRfMinT;  /* [2F] RF Min */
	}
	else
	{
#if defined(CONFIG_PLATFORM_TM201COMBO)
		isdbt[0]=0xFF;  /* [2E] IF Max */
#else
		isdbt[0]=0x73;  /* [2E] IF Max */
#endif
		isdbt[1]=0x00;  /* [2F] RF Min */

	}

	Drv_Ch_I2C_DemodWrite(nChannelId,	DEMOD_TC90517_ADDRESS, TC90517_2E, &isdbt[0], 2);

#if defined(CONFIG_PLATFORM_TM201COMBO)
	val = 0x00; 	/* [34]
				0010 1111
				[7:6] disable: 0
				[5:4] lpfsl: Digital filter facotr (2: Master lock frequency 82 [MHz]
				[3:2] ifsch: IF agc gain correction setting (3: 1 time)
				[1:0] rfsch: RF agc gain correction setting (3: 1 time) */
#else
	val = 0x3f; 	/* [34]
				0010 1111
				[7:6] disable: 0
				[5:4] lpfsl: Digital filter facotr (2: Master lock frequency 82 [MHz]
				[3:2] ifsch: IF agc gain correction setting (3: 1 time)
				[1:0] rfsch: RF agc gain correction setting (3: 1 time) */
#endif

	Drv_Ch_I2C_DemodWrite(nChannelId,	DEMOD_TC90517_ADDRESS, TC90517_34, &val, 1);
}

void Set_Equalization(int nChannelId)
{
	HUINT8	val=0x50;

	Drv_Ch_I2C_DemodWrite(nChannelId, DEMOD_TC90517_ADDRESS, TC90517_5C, &val, 1);
}

void Set_ErrorCorrection(int nChannelId)
{
	HUINT8	isdbt[3];

	isdbt[0]=0x18;
	isdbt[1]=0x00;
	isdbt[2]=0x00;
	Drv_Ch_I2C_DemodWrite(nChannelId, DEMOD_TC90517_ADDRESS,  TC90517_70, &isdbt[0], 3);

#if defined (PLATFORM_DT90517)
	isdbt[0]=0x01;  /*[73]
				   [6] pvita: perra error output,
				       0:error after RS correction, 1: error after Viterbi correction
				   [5:4] peera layer
				       0: line a, 1: line b, 2: line c, 3: fixd to 0
				   [2] pvitab: perrb error output
				       0: error after RS correction, 1: error after Viterbi correction
				   [1:0] peerb layer
				       0: line a, 1: line b, 2: line c, 3:fixted to 0 */
	isdbt[1]=0x12;
	Drv_Ch_I2C_DemodWrite(nChannelId, DEMOD_TC90517_ADDRESS, TC90517_73, &isdbt[0], 2);
#endif

	/* [0x75]  riging edge가 실제로 되지 않아서 sw로 inversion 해줌 */
	isdbt[0]=0x02; //0x00
	isdbt[1]=0x02; // ssen 0x38;	// default 0x00 cyc : 0 => 2frames
	isdbt[2]=0x03;					//	Measurement cycle setting : Time mode, After Viterbi
	Drv_Ch_I2C_DemodWrite(nChannelId, DEMOD_TC90517_ADDRESS, TC90517_75, &isdbt[0], 3);
}


#if 0
void Set_TMCCDecodedData(int nChannelId)
{
	HUINT8	isdbt[8];
	HUINT8	val;

#if defined (PLATFORM_DT90507)
	val=0x00;
#else
	val=0xa0;
#endif
	Drv_Ch_I2C_DemodWrite(nChannelId, DEMOD_TC90517_ADDRESS, TC90517_B0, &val, 1);

	isdbt[0]=0x3d;	//[0xb2]
	isdbt[1]=0x25;
	isdbt[2]=0x8b;
	isdbt[3]=0x4b;
	isdbt[4]=0x3f;
	isdbt[5]=0xff;
	isdbt[6]=0xff;
#if defined (PLATFORM_DT90507)
	isdbt[7]=0xff;	//[0xb9]
#else
	isdbt[7]=0xfc;	/*[B9]*/
#endif
	Drv_Ch_I2C_DemodWrite(nChannelId, DEMOD_TC90517_ADDRESS, TC90517_B2, &isdbt[0], 8);
}
#endif

#if defined(CONFIG_PLATFORM_TM201COMBO)
void MxL201_Init_TER(int nChannelId)
 {
	HUINT32					nRet = 0;

	g_stSemTunerConfigTer[nChannelId].nChannelId 		= nChannelId;
	g_stSemTunerConfigTer[nChannelId].Mode				= MxL_MODE_CAB_STD;
	g_stSemTunerConfigTer[nChannelId].Xtal_Freq			= MxL_XTAL_16_MHZ;
	g_stSemTunerConfigTer[nChannelId].IF_Freq			= MxL_IF_6_MHZ;
	g_stSemTunerConfigTer[nChannelId].IF_Spectrum		= MxL_NORMAL_IF;
	g_stSemTunerConfigTer[nChannelId].ClkOut_Setting	= (nChannelId == MASTER_TER_DEMOD) ? MxL_CLKOUT_ENABLE : MxL_CLKOUT_DISABLE;
	g_stSemTunerConfigTer[nChannelId].ClkOut_Amp		= MxL_CLKOUT_AMP_0;
	g_stSemTunerConfigTer[nChannelId].Xtal_Cap			= MxL_XTAL_CAP_22_PF;
	g_stSemTunerConfigTer[nChannelId].RF_Freq_Hz		= gTC90517Channel[nChannelId].frequency;
	g_stSemTunerConfigTer[nChannelId].BW_MHz			= MxL_BW_6MHz;

	CH_DI_Print(4,("MxL201_Init_TER[%d]:: Start! \n",nChannelId));
	nRet = MxL_Tuner_Init(&g_stSemTunerConfigTer[nChannelId]);
	if (nRet != DI_CH_OK)
 {
		CH_DI_Print(0,("MxL_Tuner_Init[%d]:: Error! (0x%x)",nChannelId,nRet));
 }
	else
 {
		CH_DI_Print(4,("MxL201_Init_TER[%d]:: Success! \n",nChannelId));
 }
}
#endif

void InitISDBTdemod(int nChannelId)
{
	 HUINT8  regVal ; 	 /* isyrst[0x01, D7] = 1 */

	regVal = 0x40;
	 Drv_Ch_I2C_DemodWrite(nChannelId, DEMOD_TC90517_ADDRESS, TC90517_01, &regVal, 1);

	regVal = 0x10;
	 Drv_Ch_I2C_DemodWrite(nChannelId, DEMOD_TC90517_ADDRESS, TC90517_09, &regVal, 1);

	regVal = 0x00;
	 Drv_Ch_I2C_DemodWrite(nChannelId, DEMOD_TC90517_ADDRESS, TC90517_0D, &regVal, 1);
	/* Parallel = 0x02, Serial = 0x00	*/
#if defined (CONFIG_PLATFORM_TM201COMBO)
	regVal = 0x00;
#else
	regVal = 0x02;
#endif
	 Drv_Ch_I2C_DemodWrite(nChannelId, DEMOD_TC90517_ADDRESS, TC90517_0F, &regVal, 1);

	regVal = 0x40;
	 Drv_Ch_I2C_DemodWrite(nChannelId, DEMOD_TC90517_ADDRESS, TC90517_15, &regVal, 1);

#if defined (CONFIG_INTERNAL_AGC)
	regVal = 0x10; //STSFLG0 Signal out control
#else
	regVal = 0x00; // 0x10;
#endif
	 Drv_Ch_I2C_DemodWrite(nChannelId, DEMOD_TC90517_ADDRESS, TC90517_1C, &regVal, 1);

	regVal = 0x00;
	 Drv_Ch_I2C_DemodWrite(nChannelId, DEMOD_TC90517_ADDRESS, TC90517_1D, &regVal, 1);

	regVal = 0x00;
	 Drv_Ch_I2C_DemodWrite(nChannelId, DEMOD_TC90517_ADDRESS, TC90517_1E, &regVal, 1);

	regVal = 0x00;
	 Drv_Ch_I2C_DemodWrite(nChannelId, DEMOD_TC90517_ADDRESS, TC90517_1F, &regVal, 1);

#if 0
	regVal = 0x32; // samsung debug for SNR Poor at some Frq 20090813   0x3f; // 0x00;
	 Drv_Ch_I2C_DemodWrite(nChannelId, DEMOD_TC90517_ADDRESS, TC90517_20, &regVal, 1);

	regVal = 0xf; // samsung debug for SNR Poor at some Frq 20090813 0x00; // 0x80;
	 Drv_Ch_I2C_DemodWrite(nChannelId, DEMOD_TC90517_ADDRESS, TC90517_22, &regVal, 1);

	regVal = 0x4c;
	 Drv_Ch_I2C_DemodWrite(nChannelId, DEMOD_TC90517_ADDRESS, TC90517_23, &regVal, 1);
#endif
	/* AGC setting */
	Set_AGC_DigitalFilter(nChannelId);

	regVal = 0x00;
	 Drv_Ch_I2C_DemodWrite(nChannelId, DEMOD_TC90517_ADDRESS, TC90517_30, &regVal, 1);

#if defined(CONFIG_PLATFORM_TM201COMBO)					// JD - for JC-8500BR cpld_dt = 0x1436
	regVal = 0x14;
#else
	regVal = 0x11;
#endif
	 Drv_Ch_I2C_DemodWrite(nChannelId, DEMOD_TC90517_ADDRESS, TC90517_31, &regVal, 1);

#if defined(CONFIG_PLATFORM_TM201COMBO)					// JD - for JC-8500BR cpld_dt = 0x1436
	regVal = 0x36;
#else
	regVal = 0x22;
#endif
	 Drv_Ch_I2C_DemodWrite(nChannelId, DEMOD_TC90517_ADDRESS, TC90517_32, &regVal, 1);

	regVal = 0x00;
	 Drv_Ch_I2C_DemodWrite(nChannelId, DEMOD_TC90517_ADDRESS, TC90517_3D, &regVal, 1);

	 regVal = 0x10;  /*mdsmd=1설정,  1/32 Gurad Interval 수신을 위해 */
	 Drv_Ch_I2C_DemodWrite(nChannelId, DEMOD_TC90517_ADDRESS, TC90517_47, &regVal, 1);

	 regVal = 0x04;
	 Drv_Ch_I2C_DemodWrite(nChannelId, DEMOD_TC90517_ADDRESS, TC90517_4A, &regVal, 1);

	regVal = 0xD8;
	 Drv_Ch_I2C_DemodWrite(nChannelId, DEMOD_TC90517_ADDRESS, TC90517_54, &regVal, 1);

	regVal = 0xe0;
	 Drv_Ch_I2C_DemodWrite(nChannelId, DEMOD_TC90517_ADDRESS, TC90517_55, &regVal, 1);

	regVal = 0x52;
	 Drv_Ch_I2C_DemodWrite(nChannelId, DEMOD_TC90517_ADDRESS, TC90517_59, &regVal, 1);

	regVal = 0x50;
	 Drv_Ch_I2C_DemodWrite(nChannelId, DEMOD_TC90517_ADDRESS, TC90517_65, &regVal, 1);

	regVal = 0x5c;
	 Drv_Ch_I2C_DemodWrite(nChannelId, DEMOD_TC90517_ADDRESS, TC90517_66, &regVal, 1);

	regVal = 0x01;
	 Drv_Ch_I2C_DemodWrite(nChannelId, DEMOD_TC90517_ADDRESS, TC90517_68, &regVal, 1);

	/* riging edge가 실제로 되지 않아서 sw로 inversion 해줌 */
	regVal = 0x02;
	 Drv_Ch_I2C_DemodWrite(nChannelId, DEMOD_TC90517_ADDRESS, TC90517_75, &regVal, 1);

	regVal = 0xd2;
	 Drv_Ch_I2C_DemodWrite(nChannelId, DEMOD_TC90517_ADDRESS, TC90517_7D, &regVal, 1);

	regVal = 0x58;
	 Drv_Ch_I2C_DemodWrite(nChannelId, DEMOD_TC90517_ADDRESS, TC90517_E0, &regVal, 1);

	regVal = 0x04;
	 Drv_Ch_I2C_DemodWrite(nChannelId, DEMOD_TC90517_ADDRESS, TC90517_E5, &regVal, 1);

	regVal = 0x0f;
	 Drv_Ch_I2C_DemodWrite(nChannelId, DEMOD_TC90517_ADDRESS, TC90517_EB, &regVal, 1);

	regVal = 0x01;
	 Drv_Ch_I2C_DemodWrite(nChannelId, DEMOD_TC90517_ADDRESS, TC90517_EF, &regVal, 1);

	/* C/N monitor & TMCC  */
	Set_Equalization(nChannelId);

	/* FEC */
	Set_ErrorCorrection(nChannelId);

	sTsmfEmgOffFlag[nChannelId] = 1;


#if 0
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

#if defined(PLATFORM_DT90517)
/* ADC control setting */
	Set_ADC_Control(nChannelId);
#endif

/* AGC setting */
	Set_AGC_DigitalFilter(nChannelId);
//	VK_TASK_Sleep(5);

/* Carrier recovery */
	Set_CarrierRecovery(nChannelId);
//	VK_TASK_Sleep(5);

/* Synchronization sequence & FFT window mornitor control */
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
		CH_DI_Print(0, ("TC90517_Init:: Set_ErrorCorrection Error: Wrong BER Calculation mode.");
	}
//	VK_TASK_Sleep(5);

/* TMCC Decoded Data */
	Set_TMCCDecodedData(nChannelId);
	VK_TASK_Sleep(5);

	//CH_DI_Print("TC90517_Initialise OK ...\n");

#else

#endif
}


 void DRV_T_InstallApi( void )
 {
	 TER_InitDevice 		 = &DT90517_InitDevice;
	 TER_SetTune			 = &DT90517_SetTune;
	 TER_CheckLock			 = &DT90517_CheckLock;

	 TER_GetSignalInfo  = &DT90517_GetSignalInfo;
	 TER_SetPowerState		 = &DT90517_SetPowerState;
	 TER_EnableTsOutput 	 = &DT90517_EnableTsOutput;
	 TER_DisableTsOutput	 = &DT90517_DisableTsOutput;

	 TER_CheckAntState	 = &DT90517_CheckAntState;
	 TER_SetAntState	 = &DT90517_SetAntState;
	 TER_GetAntState	 = &DT90517_GetAntState;
	 TER_ProcessRecoverAntenna = &DT90517_ProcessRecoverAntenna;


	 TER_ResetGpio			 = &DT90517_ResetGpio;
	 TER_Initialize			 = &DT90517_Initialise;

 	CH_DI_Print(6, ("[DT90517] DRV_T_InstallApi()  OK! \n"));

	 return;
 }

 int DT90517_InitDevice(int nChannelId)
 {
#if defined(CONFIG_PLATFORM_TM201COMBO)
	CH_UNUSED(nChannelId);
	DRV_I2c_SetClock(MASTER_I2C_BUS, DRV_I2C_CLOCK_100K);
	DRV_I2c_SetClock(SLAVE_I2C_BUS, DRV_I2C_CLOCK_100K);
#else
	CH_UNUSED(nChannelId);
	DRV_GPIO_Write(GPIO_ID_RST_OFDM, GPIO_HI);
#endif

	 return DI_CH_OK;
 }

 int DT90517_SetTune( int nChannelId, HUINT8 *bChecklock , HUINT32 unWorkingId )
 {

	 int				 nResult = DI_CH_OK;
	 unsigned char		 lock_ofdm = 0;

	CH_UNUSED(unWorkingId);

#if defined(CONFIG_PLATFORM_TM201COMBO)
	HINT32		nUnitId = 0;
	nUnitId = GetDeviceUnitId( nChannelId );
	if (nChannelId == MASTER_TER_DEMOD)
	{
		DRV_GPIO_Write(GPIO_ID_SEL_TC1, GPIO_HI);
		CH_DI_Print(2,("DT90517_SetTune[%d]...T/C Switch #1 is set to TER \n",nChannelId));
	}
	else if (nChannelId == SLAVE_TER_DEMOD)
	{
		DRV_GPIO_Write(GPIO_ID_SEL_TC2, GPIO_HI);
		CH_DI_Print(2,("DT90517_SetTune[%d]...T/C Switch #2 is set to TER \n",nChannelId));
	}
	else
	{
		CH_DI_Print(2,("DT90517_SetTune[%d]...Error!! Wrong Instance ID [%d] \n",nChannelId,nUnitId));
	}
#endif

	//DT90517_ResetGpio(nChannelId);
	 //DT90517_Initialise(nChannelId);
	 //CH_DI_Print("TC90517_Initialise OK ...\n");

 	nResult = DT90517_Lock(nChannelId, &lock_ofdm);

	*bChecklock = lock_ofdm;

	return nResult;
 }

 void DT90517_ResetGpio(int nChannelId)
 {
#if defined(CONFIG_PLATFORM_TM201COMBO)
	HUINT32 	nUnitId = 0;

	nUnitId = GetDeviceUnitId( nChannelId );

	CH_DI_Print(2,("nChannelID[%d] / nUnitId[%d] \n", nChannelId, nUnitId));
	if(nChannelId == MASTER_TER_DEMOD)
	{
		DRV_GPIO_Write(GPIO_ID_RST_OFDM, GPIO_LOW);
		VK_TASK_Sleep(30);

		DRV_GPIO_Write(GPIO_ID_SEL_TC1, GPIO_HI);
		CH_DI_Print(2,(" Set Tuner OFDM Mode[%d] - MASTER TER\n",nChannelId));

		DRV_GPIO_Write(GPIO_ID_RST_OFDM, GPIO_HI);
		VK_TASK_Sleep(30);
	}
	else if(nChannelId == SLAVE_TER_DEMOD)
	{
		DRV_GPIO_Write(GPIO_ID_RST_OFDM2, GPIO_LOW);
		VK_TASK_Sleep(30);

		DRV_GPIO_Write(GPIO_ID_SEL_TC2, GPIO_HI);
		CH_DI_Print(2,(" Set Tuner OFDM Mode[%d] - SLAVE TER\n",nChannelId));

		DRV_GPIO_Write(GPIO_ID_RST_OFDM2, GPIO_HI);
		VK_TASK_Sleep(30);
	}
	else
	{
		CH_DI_Print(0, ("[DT90517_ResetGpio] ERR!! nChannelId[%d] \n", nChannelId));
	}
#else
	 CH_UNUSED(nChannelId);

 	DRV_GPIO_Write(GPIO_ID_RST_QAM, GPIO_LOW);
	DRV_GPIO_Write(GPIO_ID_RST_OFDM, GPIO_LOW);
	 VK_TASK_Sleep(30);

	 DRV_GPIO_Write(GPIO_ID_SEL_DEMOD, GPIO_LOW);
	 CH_DI_Print(4, (" Set Tuner OFDM Mode\n"));

	 DRV_GPIO_Write(GPIO_ID_RST_OFDM, GPIO_HI);
	 VK_TASK_Sleep(30);
#endif
}

 void DT90517_Initialise(int nChannelId)
{

#if defined(CONFIG_PLATFORM_TM201COMBO)
	MxL201_Init_TER(nChannelId);
	CH_DI_Print(5, ("DT90517_Initialise : Mxl201 Tuner Init[%d] \n", nChannelId));
#endif

	InitISDBTdemod(nChannelId);
}

 static int DT90517_SetTuner(int nChannelId, HUINT32 frequency)
 {
 	 HUINT8	 tunerLockFlag = 0;
	 HUINT8	 tuningTrial = 0;

	 /* ISDBT용 주파수로 전환 */
	 if(((frequency -143000) / 1000000) != (frequency /1000000)) /* 만약 143MHz가 안붙어서 오는 경우 */
	 {
		 frequency += 143000;
		 CH_DI_Print(4, (" TC90517_SetTuner[%d]:: Modifyed ISDB Freq = %d\n", nChannelId, frequency));
	 }
	 else	 /* 143MHz가 붙어서 오는 경우 */
	 {
		 CH_DI_Print(4, (" TC90517_SetTuner[%d]:: ISDB Freq = %d\n", nChannelId, frequency));
	 }

	 /* Try Tuning */
	 while( (tuningTrial < 3) && !tunerLockFlag )
	 {
 #if defined(CONFIG_PLATFORM_TM201COMBO)								/* MxL201RF 채널 전환 알고리즘 수정(settune시 무조건 TunerInit) */
	 	MxL201_Init_TER(nChannelId);
 #endif
		 tunerLockFlag = Set_OFDM_Tuner(nChannelId, frequency);	 /* TD1686 튜너를 셋팅 한다. 다른 튜너가 오면 여기에 위치시킨다. */
		 tuningTrial++;
	 }

	 gTC90517Channel[nChannelId].frequency = frequency;  /* TODO : frequency를 클로벌로 관리 안할 수도 있으므로 체크한다. */

	 if(tunerLockFlag == 1)
	 {
		 CH_DI_Print(2, (" TC90517_SetTuner[%d]:: Tuner Lock Ok!\n", nChannelId));
		 return DI_CH_OK;
	 }
	 else
	 {
		 CH_DI_Print(2, (" TC90517_SetTuner[%d]:: Tuner Lock Fail!\n", nChannelId));
		 return DI_CH_ERR;
	 }
 }

 int  DT90517_GetSignalInfo(int nChannelId, DI_CH_SignalQuality_t *pstSignalInfo)
 {
	 HUINT8 ucRfAgc=0, ucIfAgc=0;
	 HINT16 Strength=0;
	 HUINT16 ucSnr=0;
	 HUINT32 ulQuality=0;
	 HUINT8	ucEmergencyFlag;
	 HUINT32 correctedNo, unCorrectedNo,unErroredNo, inbandBer, unCorrectedFlag;

#if defined(CONFIG_PLATFORM_TM201COMBO)
	 HFLOAT32	fInputPower = 0;
#endif

	 DT90517_GetAGC( nChannelId, &ucRfAgc, &ucIfAgc );
 	 DT90517_GetChData(nChannelId, &Strength, &ulQuality, &ucSnr, &ucEmergencyFlag);
	 DT90517_GetCorrectionData(nChannelId, &correctedNo, &unCorrectedNo, &unErroredNo, &inbandBer, &unCorrectedFlag);

#if defined(CONFIG_PLATFORM_TM201COMBO)
	 MxL_Check_RF_Input_Power(&g_stSemTunerConfigTer[nChannelId], &fInputPower);
#endif

	 pstSignalInfo->ulStrength	 = (HINT16)Strength;
	 pstSignalInfo->ulAgc		 = (HUINT32)ucRfAgc;
	 pstSignalInfo->ulQuality	 = (HUINT32)ulQuality;
	 pstSignalInfo->fSnr = (float)ucSnr/10;

#if defined(CONFIG_PLATFORM_TM201COMBO)
	pstSignalInfo->ulRfAgc		= (HUINT32)ucRfAgc;
	pstSignalInfo->ulIfAgc		= (HUINT32)ucIfAgc;
	pstSignalInfo->fSignalInputPower	= fInputPower;
	pstSignalInfo->fBer			= (HFLOAT32)inbandBer;
#endif

	 pstSignalInfo->ulCorrectedNo = correctedNo;
	 pstSignalInfo->ulUnCorrectedNo = unCorrectedNo;
	 pstSignalInfo->ulUnErroredNo = unErroredNo;
	 pstSignalInfo->ulInbandBer = inbandBer;
	 pstSignalInfo->ulUnCorrectedFlag = unCorrectedFlag;
#if defined(CONFIG_PLATFORM_TM201COMBO)
	CH_DI_Print(2, ("[DT90517] GetStrengthQuality[%d]:: Strength =%d , ulRfAgc = %d, ulIfAgc = %d, ulQuality = %d, fSnr = %f \n ",
					 nChannelId,pstSignalInfo->ulStrength,pstSignalInfo->ulRfAgc,pstSignalInfo->ulIfAgc, pstSignalInfo->ulQuality, pstSignalInfo->fSnr));

 	CH_DI_Print(3, ("[DT90517] GetStrengthQuality[%d]:: ulUnCorrectedNo =%d , ulInbandBer = %d \n ",nChannelId, pstSignalInfo->ulUnCorrectedNo,pstSignalInfo->ulInbandBer));

	CH_DI_Print(3, ("[DT90517] GetStrengthQuality[%d]:: fSignalInputPower =%f \n ",nChannelId,pstSignalInfo->fSignalInputPower));
#else
	 CH_DI_Print(2, ("[DT90517] GetStrengthQuality[%d]:: Strength =%d , ulAgc = %d, ulQuality = %d, fSnr = %d \n ",
					 nChannelId,pstSignalInfo->ulStrength,pstSignalInfo->ulAgc, pstSignalInfo->ulQuality, pstSignalInfo->fSnr));

 	CH_DI_Print(3, ("[DT90517] GetStrengthQuality[%d]:: ulUnCorrectedNo =%d , ulInbandBer = %d \n ",nChannelId, pstSignalInfo->ulUnCorrectedNo,pstSignalInfo->ulInbandBer));
#endif

	//for  debugging
	//DT90517_ViewStatusParam(nChannelId);

	 return DI_CH_OK;

 }

 int  DT90517_ViewStatusParam(int nChannelId)
 {
	 ViewDemodStatus(nChannelId);

	 ViewOfdmParam(nChannelId);

	 return DI_CH_OK;
 }


void DT90517_SetBERCalTime(int nChannelId, HUINT32 berCalMode)
{
	CH_DI_Print(5, ("[TC90517] SetBERCalTime[%d]:: berCalMode = %d \n",nChannelId,berCalMode));

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
		CH_DI_Print(0, ("[TC90517_SetBERCalTime[%d]] ERR! Wrong BER Calculation mode \n",nChannelId));
		gBERCalMode[nChannelId] = 0;	/* Other case (1초)*/
		gBERDisplayPeriod[nChannelId] = 2;	/* 500ms * 2 = 1(s) */
		totalCntToDisplayBER[nChannelId] = 0;
	}
}

static int DT90517_Lock( int nChannelId, unsigned char *bChecklock )
{
#if defined(CONFIG_PLATFORM_TM201COMBO)											// JD - for JC-8500BR - To classify each parameters to channel id (in JC-3500 instance id)
	int 	nResult = DI_CH_OK;

	unsigned int		frequency;

	CH_TER_TuneParam_t	*pstTerTuneParam;

	pstTerTuneParam = (CH_TER_TuneParam_t *)GetWorkingTuneInfo( nChannelId );

	/* Get the tuning data loaded from application to channel device */
	frequency = pstTerTuneParam->ulFrequency * KHZ_TO_HZ ;

	/* TC90517 Activation */
	ActivateIsdbDemod(nChannelId);

	gTC90517Channel[nChannelId].frequency = frequency;
	gCurrentId[nChannelId] = 0;
	beforeId2[nChannelId] = 0;

	CH_DI_Print(2, ("DT90517_Lock[%d]:: freq= %d\n",nChannelId, gTC90517Channel[nChannelId].frequency));

	if (save_stream == 1)
	{
		nResult = DT90517_SetTuner(nChannelId, frequency);
		if(nResult != DI_CH_OK)
		{
			CH_DI_Print(1, ("DT90517_Lock[%d]:: Tuner Unlock ! \n",nChannelId));
			return nResult;
		}

		InitISDBTdemod(nChannelId);
		CheckLock(nChannelId);

		return DI_CH_OK;
	}

	nResult = DT90517_SetTuner(nChannelId, frequency);
	if(nResult != DI_CH_OK)
	{
		CH_DI_Print(1, ("DT90517_Lock[%d]:: Tuner Unlock ! \n",nChannelId));
		return nResult;
	}

	InitISDBTdemod(nChannelId);

	nResult = CheckLock(nChannelId);

	if(nResult == DI_CH_OK) /* 파라미터 검사 OK */
	{
		//nResult = DI_CH_OK;
		*bChecklock = 1;
		CH_DI_Print(1, ("DT90517_Lock[%d]:: Lock OK\n", nChannelId));
	}
	else /* 파라미터 오류 */
	{
		*bChecklock = 0;
		CH_DI_Print(1, ("DT90517_Lock[%d]:: Lock Fail\n", nChannelId));
	}

	//DRV_CH_UnLock( nChannelId );

	return nResult;

#else
	int 	nResult = DI_CH_OK;
	int	nUnitId=0;

	unsigned int		frequency;

	CH_TER_TuneParam_t *pstTerTuneParam;

	nUnitId = GetDeviceUnitId( nChannelId );

	pstTerTuneParam = (CH_TER_TuneParam_t *)GetWorkingTuneInfo( nChannelId );

	/* Get the tuning data loaded from application to channel device */
	frequency = pstTerTuneParam->ulFrequency * KHZ_TO_HZ ;

	/* TC90517 Activation */
	ActivateIsdbDemod(nUnitId);

	gTC90517Channel[nUnitId].frequency = frequency;
	gCurrentId[nUnitId] = 0;
	beforeId2[nUnitId] = 0;

	CH_DI_Print(2, ("DT90517_Lock:: Unit[%d], freq= %d\n",  nUnitId, gTC90517Channel[nUnitId].frequency));

	if (save_stream == 1)
	{
		nResult = DT90517_SetTuner(nUnitId, frequency);
		if(nResult != DI_CH_OK)
		{
			CH_DI_Print(1, ("DT90517_Lock:: Tuner Unlock ! \n"));
			return nResult;
		}

		DT90517_Initialise(nUnitId);
		CheckLock(nUnitId);

		return DI_CH_OK;
	}

	nResult = DT90517_SetTuner(nUnitId, frequency);
	if(nResult != DI_CH_OK)
	{
		CH_DI_Print(1, ("DT90517_Lock:: Tuner Unlock ! \n"));
		return nResult;
	}

	DT90517_Initialise(nUnitId);

	nResult = CheckLock(nUnitId);

	if(nResult == DI_CH_OK) /* 파라미터 검사 OK */
	{
		//nResult = DI_CH_OK;
		*bChecklock = 1;
		CH_DI_Print(1, ("DT90517_Lock[%d]:: Lock OK\n", nUnitId));
	}
	else /* 파라미터 오류 */
	{
		*bChecklock = 0;
		CH_DI_Print(1, ("DT90517_Lock[%d]:: Lock Fail\n", nUnitId));
	}

	//DRV_CH_UnLock( nChannelId );

	return nResult;
#endif
}

void DT90517_Stop(int nChannelId)
{
	CH_DI_Print(3, ("DT90517_Stop:: Channel[%d]\n",	nChannelId));

	gTC90517Channel[nChannelId].strength = 0;
	gTC90517Channel[nChannelId].quality = 0;
	gTC90517Channel[nChannelId].snr = 0;
	gTC90517Channel[nChannelId].emergencyFlag = 0;
	totalCntToDisplayBER[nChannelId] = 0;

	ofdmUncorPkt[nChannelId][0] = 0;
	ofdmUncorPkt[nChannelId][1] = 0;
	ofdmUncorPkt[nChannelId][2] = 0;

	if(save_stream != 1)
	{
		DeactivateIsdbDemod(nChannelId);
	}
}

 int DT90517_CheckStatus ( int nChannelId)
{
	int 	nResult = DI_CH_OK;
	HUINT32 unRequestId = 0;

	gCurrentId[nChannelId] = 1;

	nResult = DT90517_GetStatus(nChannelId);

	if( gTC90517Channel[nChannelId].emergencyFlag == 1)
	{
		sTsmfEmgOffFlag[nChannelId] = TRUE;
		if((sTsmfEmgOnFlag[nChannelId] == FALSE) || (beforeId2[nChannelId] != gCurrentId[nChannelId]) )
		{
			sTsmfEmgOnFlag[nChannelId] = TRUE;
			beforeId2[nChannelId] = gCurrentId[nChannelId];

			CH_DI_Print(3, ("DT90517_CheckStatus[%d]:: TSMF Emergency On .....\n\n",nChannelId));
			//GWM_SendMessage(MSG_CH_EWS_ON, versionNumber, CH_MOD_OFDM, 0);
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

			CH_DI_Print(3, ("DT90517_CheckStatus[%d]:: TSMF Emergency Offl .....\n\n",nChannelId));
			//GWM_SendMessage(MSG_CH_EWS_OFF, versionNumber, CH_MOD_OFDM, 0);
			DRV_CH_CallNotifyCallback( nChannelId, unRequestId, DI_CH_SIGNAL_TSMF_EWS_OFF);
		}
	}

	if(nResult == DI_CH_OK) /* locking 상태 이면 */
	{
		CH_DI_Print(2, ("DT90517_CheckStatust[%d]:: Locking (freq= %d) \n", nChannelId, gTC90517Channel[nChannelId].frequency));
	}
	else /* unlock 상태이면 */
	{
		nResult = DI_CH_ERR;
		CH_DI_Print(2, ("DT90517_CheckStatust[%d]:: Unlock (freq= %d) \n", nChannelId, gTC90517Channel[nChannelId].frequency));
	}

	return nResult;
 }

HBOOL DT90517_CheckLock ( int nChannelId, HUINT32 unWorkingID )
{
	int 	nResult = DI_CH_OK;

	CH_UNUSED(unWorkingID);

	gCurrentId[nChannelId] = 1;

	nResult = DT90517_GetStatus(nChannelId);
/*
	if( gTC90517Channel[nChannelId].emergencyFlag == 1)
	{
		sTsmfEmgOffFlag[nChannelId] = TRUE;
		if((sTsmfEmgOnFlag[nChannelId] == FALSE) || (beforeId2[nChannelId] != gCurrentId[nChannelId]) )
		{
			sTsmfEmgOnFlag[nChannelId] = TRUE;
			beforeId2[nChannelId] = gCurrentId[nChannelId];
			//GWM_SendMessage(MSG_CH_EWS_ON, versionNumber, CH_MOD_OFDM, 0);
			CH_DI_Print("TC90517_CheckStatus:: TSMF Emergency On .....\n\n");
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
			CH_DI_Print("TC90517_CheckStatus:: TSMF Emergency Offl .....\n\n");
		}
	}
*/
	if(nResult == DI_CH_OK) /* locking 상태 이면 */
	{
		CH_DI_Print(2, ("DT90517_CheckLock[%d]:: Locking \n", nChannelId));
		return TRUE;
	}
	else /* unlock 상태이면 */
	{
		nResult = DI_CH_ERR;
		CH_DI_Print(2, ("DT90517_CheckLock[%d]:: Unlock \n", nChannelId));
		return FALSE;
	}

 }


int DT90517_CheckTsmfStatus(int nChannelId)
 {
	int 	nResult = DI_CH_OK;

	nResult = DT90517_GetStatus(nChannelId);

	if(nResult != DI_CH_OK) /* unlock 상태이면 */
	{
		nResult = DI_CH_ERR;
		CH_DI_Print(3, ("TC90517_CheckTsmfStatus[%d]:: Unlock \n", nChannelId));
	}
	else /* locking 상태 이면 */
	{
		CH_DI_Print(3, ("TC90517_CheckTsmfStatus[%d]:: Lock \n", nChannelId));
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
		CH_DI_Print(2, ("CheckLock[%d]:: FastLockIsdb is OK \n",nChannelId));
	}
	else
	{
		nResult = DI_CH_ERR;
		CH_DI_Print(2,("CheckLock[%d]:: FastLockIsdb is ERROR\n",nChannelId));
	}

	return nResult;
}

void DT90517_GetEwsInfo(int nChannelId, HUINT8 *ucIsEws)
{
	*ucIsEws = gTC90517Channel[nChannelId].emergencyFlag;
	CH_DI_Print(4, ("TC90517_GetEwsInfo[%d]:: EmergencyFlag = 0x%x\n",nChannelId,gTC90517Channel[nChannelId].emergencyFlag));
}


static int DT90517_GetStatus(int nChannelId)
{
	//HUINT8	GetFECLock;
	HUINT8	rfAgc, ifAgc;
	HUINT8	berI;
	HUINT8	val, val1;

	/* Check Emergency Flag */
	Drv_Ch_I2C_DemodRead(nChannelId, DEMOD_TC90517_ADDRESS, TC90517_81, &val, 1);
	gTC90517Channel[nChannelId].emergencyFlag = (val >> 7);
	CH_DI_Print(4, ("DT90517_GetStatus[%d]::  EmergencyFlag = 0x%x\n",nChannelId,gTC90517Channel[nChannelId].emergencyFlag));

	Drv_Ch_I2C_DemodRead(nChannelId, DEMOD_TC90517_ADDRESS,  TC90517_83, &rfAgc, 1);//RF_AGC
	Drv_Ch_I2C_DemodRead(nChannelId, DEMOD_TC90517_ADDRESS,  TC90517_82, &ifAgc, 1);//IF_AGC

#if 0 // justin@humax
	/* RF Unlock */
	if( (rfAgc == 0xff) && (ifAgc == 0xff) )
	{
		CH_DI_Print("TC90517_GetStatus[%d]:: RF_AGC = 0x%x (%d), IF_AGC= 0x%x (%d)\n", nChannelId, rfAgc,rfAgc,ifAgc, ifAgc);
		gTC90517Channel[nChannelId].strength = 0;
		gTC90517Channel[nChannelId].quality = 0;
		gTC90517Channel[nChannelId].snr = 0;
		gTC90517Channel[nChannelId].emergencyFlag = 0;
		totalCntToDisplayBER[nChannelId] = 0;
		CH_DI_Print(0, ("TC90517_CheckStatus:: Error!! RF Unlock!!\n");
		if(accuFlag[nChannelId] == 1)
		{
			Reset_Accu(nChannelId);
		}
		//CH_PrintMon("OFDM Demod TC905007 UNLOCK ! \n");
		return DI_CH_ERR;
	}
#endif
	/* Read the SYNC status register */
	/* Viterbi error -> RS error -> TS packet error  */
	Drv_Ch_I2C_DemodRead(nChannelId, DEMOD_TC90517_ADDRESS,  TC90517_80, &val, 1);
	CH_DI_Print(3, ("DT90517_GetStatus[%d]::  SB_SYNC = 0x%x\n",nChannelId,val));
	//GetFECLock = val&0x07;

#if defined(CONFIG_PLATFORM_TM201COMBO)
	Drv_Ch_I2C_DemodRead(nChannelId, DEMOD_TC90517_ADDRESS, TC90517_96, &val1, 1);

	if(((val&0x28) != 0) || ((val1&0xe0) == 0) )
	{
		gTC90517Channel[nChannelId].strength = 0;
		gTC90517Channel[nChannelId].quality = 0;
		gTC90517Channel[nChannelId].snr = 0;
		gTC90517Channel[nChannelId].emergencyFlag = 0;
		totalCntToDisplayBER[nChannelId] = 0;
		CH_DI_Print(1, ("DT90517_GetStatus[%d]:: Error!!Demodulator Unlock!! \n",nChannelId));
	if(accuFlag[nChannelId] == 1)
	{
		Reset_Accu(nChannelId);
	}
	//CH_PrintMon("OFDM Demod TC905007 UNLOCK ! \n");
	return DI_CH_ERR;
	}

#else
	/* Demodulator Unlock */
	//if((val&0x39) != 0)
	if((val&0x3b) != 0)
	{
		gTC90517Channel[nChannelId].strength = 0;
		gTC90517Channel[nChannelId].quality = 0;
		gTC90517Channel[nChannelId].snr = 0;
		gTC90517Channel[nChannelId].emergencyFlag = 0;
		totalCntToDisplayBER[nChannelId] = 0;
		CH_DI_Print(1, ("DT90517_GetStatus:: Error!!Demodulator Unlock!!\n"));
		if(accuFlag[nChannelId] == 1)
		{
			Reset_Accu(nChannelId);
		}
		//CH_PrintMon("OFDM Demod TC905007 UNLOCK ! \n");
		return DI_CH_ERR;
	}
#endif
	CH_DI_Print(2, ("DT90517_GetStatus[%d]:: RF_AGC = 0x%x (%d), IF_AGC= 0x%x (%d)\n", nChannelId, rfAgc,rfAgc,ifAgc, ifAgc));

	/* Get Uncorrected Packet of A,B,C Layer */
	GetUnCorrectedPacket(nChannelId, &gTC90517Correction[nChannelId].UnCorrectedNo[0]);
	/* Get Unerrored Packet of A,B,C Layer */
	GetUnErroredPacket (nChannelId, &gTC90517Correction[nChannelId].UnErroredNo[0]);

	for(berI = 0; berI < ISDBT_LAYER_NUM; berI++)
	{
		gTC90517Correction[nChannelId].CorrectedNo[berI] = 0;

		GetIsdbtBer(nChannelId, &gTC90517Correction[nChannelId].InbandBer[berI], gTC90517Correction[nChannelId].UnCorrectedNo[berI], gTC90517Correction[nChannelId].UnErroredNo[berI], &gTC90517Correction[nChannelId].UnCorrectedFlag[berI]);
		if(totalCntToDisplayBER[nChannelId] == 0)	/* 500ms * 16 = 8sec */
		{
			CH_DI_Print(4,("Channel[%d] : Time = %d, Uncorrected pkt(Accu)[%d] = %d(%d), UnErroredNo pkt=%d, BER = %d (%d)\n",nChannelId,accuCnt[nChannelId]*4,
				berI, gTC90517Correction[nChannelId].UnCorrectedNo[berI], accuUnCorPkt[nChannelId][berI], gTC90517Correction[nChannelId].UnErroredNo[berI],
				gTC90517Correction[nChannelId].InbandBer[berI], gTC90517Correction[nChannelId].UnCorrectedFlag[berI]));
		}
	}

	gTC90517Channel[nChannelId].strength = GetIsdbStrength(nChannelId);
	gTC90517Channel[nChannelId].snr = GetOfdmCn(nChannelId);
	CH_DI_Print(2, ("TC90517_CheckStatus[%d]:: Strength = %d, SNR = %d\n", nChannelId, gTC90517Channel[nChannelId].strength,  gTC90517Channel[nChannelId].snr));

	/* A,B,C Layer 의 BER중 가장 높은 값(가장 안좋은 BER)을 quality에 넣는다. */
	gTC90517Channel[nChannelId].quality = gTC90517Correction[nChannelId].InbandBer[0];
	for(berI = 1; berI < ISDBT_LAYER_NUM; berI++)
	{
		if(gTC90517Channel[nChannelId].quality < gTC90517Correction[nChannelId].InbandBer[berI])
		{
			gTC90517Channel[nChannelId].quality = gTC90517Correction[nChannelId].InbandBer[berI];
		}
	}
	if(totalCntToDisplayBER[nChannelId] == 0)	/* 500ms * 16 = 8sec */
	{
		CH_DI_Print(2, ("TC90517_CheckStatus[%d]:: Strength = %d, Quality = %d, SNR = %d\n", nChannelId, gTC90517Channel[nChannelId].strength ,gTC90517Channel[nChannelId].quality, gTC90517Channel[nChannelId].snr));
	}

	if(accuFlag[nChannelId] == 1)
	{
		if((totalCntToDisplayBER[nChannelId] % 8) == 0)
		{
			accuCnt[nChannelId]++;
			for(berI = 0; berI < ISDBT_LAYER_NUM; berI++)
			{
				accuUnCorPkt[nChannelId][berI] += gTC90517Correction[nChannelId].UnCorrectedNo[berI];
			}
		}
	}

	totalCntToDisplayBER[nChannelId]++;

	if(totalCntToDisplayBER[nChannelId] == gBERDisplayPeriod[nChannelId])	/* 500ms * 16 = 8sec */
	{
		totalCntToDisplayBER[nChannelId] = 0;
	}

	//CH_PrintMon("OFDM Demod DT90517 LOCK ! \n");

	return DI_CH_OK;
}


void TC90517_ReadTMCC(int nChannelId)
{
	HUINT8	data[8];
	HUINT8	temp;
	int		i;

	Drv_Ch_I2C_DemodRead(nChannelId, DEMOD_TC90517_ADDRESS, TC90517_B2, &data[0], 8);

	for(i = 0; i < 8; i++)
	{
		CH_DI_Print(4, ("[TMCC] Data[%d] : %x\n", i, data[i]));
	}

	temp = (HUINT8)((data[0] & 0xC0) >> 6);	/* B2[7:6] */
	CH_DI_Print(4, ("[TMCC]System ID[%d] : %x\n",nChannelId,temp));

	temp = (HUINT8)((data[0] & 0x3C) >> 2);	/* B2[5:2] */
	CH_DI_Print(4, ("[TMCC]Changeover indicator[%d] : %x\n",nChannelId,temp));

	temp = (HUINT8)((data[0] & 0x02) >> 1);	/* B2[1] */
	CH_DI_Print(4, ("[TMCC]Emergency Alarm flag[%d] : %x\n",nChannelId,temp));

	temp = (HUINT8)(data[0] & 0x01);			/* B2[0] */
	CH_DI_Print(4, ("[TMCC]Partial reception flag[%d] : %x\n",nChannelId,temp));

	temp = (HUINT8)((data[1] & 0xE0) >> 5);	/* B3[7:5] */
	CH_DI_Print(4, ("[TMCC]Layer A Modulation scheme[%d] : %x\n",nChannelId,temp));

	temp = (HUINT8)((data[1] & 0x1C) >> 2);	/* B3[4:2] */
	CH_DI_Print(4, ("[TMCC]Layer A Convolution coding rate[%d] : %x\n",nChannelId,temp));

	temp = (HUINT8)(((data[1] & 0x03) << 1) | ((data[2] & 0x80) >> 7));	/* B3[1:0] & B4[7] */
	CH_DI_Print(4, ("[TMCC]Layer A time interleave scheme[%d] : %x\n",nChannelId,temp));

	temp = (HUINT8)((data[2] & 0x78) >> 3);	/* B4[6:3] */
	CH_DI_Print(4, ("[TMCC] # of segments used by layer A[%d] : %x\n",nChannelId,temp));

	temp = (HUINT8)(data[2] & 0x07);			/* B4[2:0] */
	CH_DI_Print(4, ("[TMCC]Layer B Modulation scheme[%d] : %x\n",nChannelId,temp));

	temp = (HUINT8)((data[3] & 0xE0) >> 5);	/* B5[7:5] */
	CH_DI_Print(4, ("[TMCC]Layer B Convolution coding rate[%d] : %x\n",nChannelId,temp));

	temp = (HUINT8)((data[3] & 0x1C) >> 2);	/* B5[4:2] */
	CH_DI_Print(4, ("[TMCC]Layer B time interleave scheme[%d] : %x\n",nChannelId,temp));

	temp = (HUINT8)(((data[3] & 0x03) << 1) | ((data[4] & 0xC0) >> 6));	/* B5[1:0] & B6[7:6] */
	CH_DI_Print(4, ("[TMCC] # of segments used by layer B[%d] : %x\n",nChannelId,temp));

	temp = (HUINT8)((data[4] & 0x38) >> 3);	/* B6[5:3] */
	CH_DI_Print(4, ("[TMCC]Layer C Modulation scheme[%d] : %x\n",nChannelId,temp));

	temp = (HUINT8)(data[4] & 0x07);			/* B6[2:0] */
	CH_DI_Print(4, ("[TMCC]Layer C Convolution coding rate[%d] : %x\n",nChannelId,temp));

	temp = (HUINT8)((data[5] & 0xE0) >> 5);	/* B7[7:5] */
	CH_DI_Print(4, ("[TMCC]Layer C time interleave scheme[%d] : %x\n",nChannelId,temp));

	temp = (HUINT8)((data[5] & 0x1E) >> 1);	/* B7[4:1]  */
	CH_DI_Print(4, ("[TMCC] # of segments used by layer C[%d] : %x\n",nChannelId,temp));
}


void DT90517_GetChData(int nChannelId, HINT16 *strength, HUINT32 *quality, HUINT16 *snr, HUINT8 *emergencyFlag)
{
	*strength = gTC90517Channel[nChannelId].strength;
	*quality = gTC90517Channel[nChannelId].quality;
	*snr = gTC90517Channel[nChannelId].snr;
	*emergencyFlag = gTC90517Channel[nChannelId].emergencyFlag;

	CH_DI_Print(4, ("[DT90517_GetChData[%d]] strength : %x (%d) \n",nChannelId,*strength, *strength ));
}

void DT90517_GetCorrectionData(int nChannelId, HUINT32 *correctedNo, HUINT32 *unCorrectedNo, HUINT32 *unErroredNo, HUINT32 *inbandBer, HUINT32 *UnCorrectedFlag)
{

	*correctedNo = gTC90517Correction[nChannelId].CorrectedNo[0] + gTC90517Correction[nChannelId].CorrectedNo[1] + gTC90517Correction[nChannelId].CorrectedNo[2];
	*unCorrectedNo = gTC90517Correction[nChannelId].UnCorrectedNo[0] + gTC90517Correction[nChannelId].UnCorrectedNo[1] + gTC90517Correction[nChannelId].UnCorrectedNo[2];
	*unErroredNo= gTC90517Correction[nChannelId].UnErroredNo[0] + gTC90517Correction[nChannelId].UnErroredNo[1] + gTC90517Correction[nChannelId].UnErroredNo[2];
	*inbandBer = gTC90517Correction[nChannelId].InbandBer[0] + gTC90517Correction[nChannelId].InbandBer[1] + gTC90517Correction[nChannelId].InbandBer[2];
	*UnCorrectedFlag = gTC90517Correction[nChannelId].UnCorrectedFlag[0] + gTC90517Correction[nChannelId].UnCorrectedFlag[1] + gTC90517Correction[nChannelId].UnCorrectedFlag[2];

}

void DT90517_GetAGC(int nChannelId, HUINT8 *ucpRFAgc, HUINT8 *ucpIFAgc)
{
	HUINT8	ucBuf[2];

	Drv_Ch_I2C_DemodRead(nChannelId, DEMOD_TC90517_ADDRESS, TC90517_82, ucBuf, 2);	/* Read IF AGC & RF AGC */

	*ucpIFAgc =	ucBuf[0];
	*ucpRFAgc = ucBuf[1];
}


int  DT90517_SetPowerState( int nChannelId, CH_Power_e etPower )
{
	HUINT8	val;

	/*----------------------*/
	/* Implementation*/
	/*----------------------*/
	switch (etPower)
	{
		case CH_POWER_OFF:  //tmPowerStandby = 1

			SetActiveStatus( nChannelId, CH_ACTION_OFF );
			CH_DI_Print(5, ("\n ------------- Sleep mode[%d] -------------\n", nChannelId));

#if defined(CONFIG_PLATFORM_TM201COMBO)														/* JD - for JC8500BR - I2C Error in HW Test SW */
			if (nChannelId == MASTER_TER_DEMOD)
				DRV_GPIO_Write(GPIO_ID_RST_OFDM, GPIO_LOW);
			else if(nChannelId == SLAVE_TER_DEMOD)
				DRV_GPIO_Write(GPIO_ID_RST_OFDM2, GPIO_LOW);
			else
				CH_DI_Print(0,("DT90517_SetPowerState[%d] Error Wrong Channel Id! \n",nChannelId));
#else
			/* adc power on & sleep, wakeup time
			[7] slpadc =0,  (set adc power up)
			[6:4] slptim = 3, (sets the sleep time)
			[3:0] wuptim = 0 (sets the wakeup time) */
			val = 0x70;
			Drv_Ch_I2C_DemodWrite(nChannelId, DEMOD_TC90517_ADDRESS, TC90517_03, &val, 1);
#endif

			break;


		case CH_POWER_ON: //tmPowerOn = 0
	        default:

			SetActiveStatus( nChannelId, CH_ACTION_ON );
			CH_DI_Print(5, ("\n ------------- WakeUp mode[%d] -------------\n",nChannelId));
		 	DT90517_ResetGpio(nChannelId);

			/* adc power on & sleep, wakeup time
			[7] slpadc =0,  (set adc power up)
			[6:4] slptim = 0, (sets the sleep time)
			[3:0] wuptim = 0 (sets the wakeup time) */
			val = 0x00;
			Drv_Ch_I2C_DemodWrite(nChannelId, DEMOD_TC90517_ADDRESS, TC90517_03, &val, 1);
			break;
	}


	return DI_CH_OK;
}


int DT90517_EnableTsOutput ( int nChannelId )
{
	CH_UNUSED(nChannelId);

	CH_DI_Print(3, ("[DT90517] TS Enable[%d] \n",nChannelId));

	return DI_CH_OK;
}

int DT90517_DisableTsOutput ( int nChannelId )
{
	CH_UNUSED(nChannelId);

	CH_DI_Print(3,("[DT90517] TS Disable[%d] \n",nChannelId));

	return DI_CH_OK;
}

void DT90517_ProcessRecoverAntenna( int nChannelId, HUINT32 unWorkingId )
{

	CH_UNUSED(nChannelId);
	CH_UNUSED(unWorkingId);

#if 0  // 일본향은 op향으로 안테나 전원 필요없음.
	int nRet=0;
	int nUnitId;
	int nActiveState;
	GPIO_STATE_t	gpioStat_t = GPIO_INVAILD;

	nUnitId = GetDeviceUnitId( nChannelId );

#if defined(PLATFORM_NO_ANTCIRCUIT)
	CH_DI_Print(5, ("DP10048_ProcessRecoverAntenna\n"));
	nRet = DRV_GPIO_Write(GPIO_ID_ANT_EN, GPIO_HI);
	VK_TASK_Sleep(2);
	nRet = DRV_GPIO_Write(GPIO_ID_ANT_EN, GPIO_LOW);
#else
	nRet = DRV_GPIO_Write(GPIO_ID_ANT_PWR_EN, GPIO_HI);
#endif
	if( nRet < 0 )
	{
		CH_DI_Print(0, ("DP10048_ProcessRecoverAntenna Write HI fail\n"));

	}

	VK_TASK_Sleep(100);// wait 100ms

	nRet = DRV_GPIO_Read(GPIO_ID_ANT_PWR_CHK, &gpioStat_t);
	if( nRet < 0 )
	{
		CH_DI_Print(0, ("DP10048_ProcessRecoverAntenna Read fail\n"));
		return;
	}
#if defined(PLATFORM_NO_ANTCIRCUIT)
	if( gpioStat_t == GPIO_HI )
	{
		CH_DI_Print(5, ("Antenna continue fault... Turn 5 Volt Off\n"));
		/* Power Off */
		//nRet = DRV_GPIO_Write(GPIO_ID_ANT_PWR_EN, GPIO_HI);
		nRet = DRV_GPIO_Write(GPIO_ID_ANT_EN, GPIO_LOW);
		if( nRet < 0 )
	{
			CH_DI_Print(0, ("DP10048_ProcessRecoverAntenna Write LOW fail\n"));
		}

		DRV_CH_CallNotifyCallback( nChannelId, unWorkingId, DI_CH_SIGNAL_ANTENNA_FAULT);
		//s_stAntState[nUnitId].etCurrentAntState = DI_CH_ANTENNA_OFF;

	}
	else if( gpioStat_t == GPIO_LOW )
	{
		CH_DI_Print(5, ("Antenna recovered...\n"));

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
#else
	if( gpioStat_t == GPIO_LOW )
	{
		CH_DI_Print(5, ("Antenna continue fault... Turn 5 Volt Off\n"));
		nRet = DRV_GPIO_Write(GPIO_ID_ANT_PWR_EN, GPIO_LOW);
		if( nRet < 0 )
		{
			CH_DI_Print(0, ("DP10048_ProcessRecoverAntenna Write LOW fail\n"));
		}

		DRV_CH_CallNotifyCallback( nChannelId, unWorkingId, DI_CH_SIGNAL_ANTENNA_FAULT);
		//s_stAntState[nUnitId].etCurrentAntState = DI_CH_ANTENNA_OFF;

	}
	else if( gpioStat_t == GPIO_HI )
	{
		CH_DI_Print(5, ("Antenna recovered...\n"));

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
#endif
	else
	{
		CH_DI_Print(0, ("DP10048_ProcessRecoverAntenna Read Unknown (%d)\n", gpioStat_t));
	}

	return;
#endif
}


int DT90517_SetAntState( int nChannelId, DI_CH_AntStatus_e etAntState )
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
				CH_DI_Print(0, (""));
				return DI_CH_ERR;
			}
		}
		else
		{
			CH_DI_Print(0, (""));
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
				CH_DI_Print(0, (""));
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


DI_CH_AntStatus_e DT90517_GetAntState( int nChannelId )
{
	int nUnitId;

	nUnitId = GetDeviceUnitId( nChannelId );

	return s_stAntState[nUnitId].etCurrentAntState;

}

int DT90517_CheckAntState( int nChannelId )
{

	CH_UNUSED(nChannelId);
#if 0	// 일본향은 op향으로 안테나 전원 필요없음.
	int nUnitId;
	int nRet=0;
	GPIO_STATE_t	gpioStat_t = GPIO_INVAILD;

	nUnitId = GetDeviceUnitId( nChannelId );

	if( s_stAntState[nUnitId].etDesiredAntState == DI_CH_ANTENNA_ON )
	{
		nRet = DRV_GPIO_Read(GPIO_ID_ANT_PWR_CHK, &gpioStat_t);
		if(nRet < 0 )
		{
			CH_DI_Print(0, ("DP10048_CheckAntState GPIO Read Error\n"));
			return DI_CH_ERR;
		}
#if defined(PLATFORM_NO_ANTCIRCUIT)
		if( gpioStat_t == GPIO_HI )
		{
			CH_DI_Print(5, (">>>>>>>>>>>>Antenna short circuit\n"));
			s_stAntState[nUnitId].etCurrentAntState = DI_CH_ANTENNA_FAULT;
			return DI_CH_ANTENNA_FAULT;
		}
		else if( gpioStat_t == GPIO_LOW)
		{
			if( s_stAntState[nUnitId].etCurrentAntState == DI_CH_ANTENNA_FAULT )
			{
				CH_DI_Print(5, ("DP10048_CheckAntState CHK(HIGH), Curr(FAULT)\n"));
				/* Power On */
				nRet = DRV_GPIO_Write(GPIO_ID_ANT_PWR_EN, GPIO_LOW);
				VK_TASK_Sleep(2);

				nRet = DRV_GPIO_Write(GPIO_ID_ANT_EN, GPIO_HI);
				nRet = DRV_GPIO_Write(GPIO_ID_ANT_EN, GPIO_LOW);

				if( nRet < 0 )
				{
					CH_DI_Print(0, ("DP10048_CheckAntState Gpio Write Error\n"));
				}
				}
			return DI_CH_OK;

			}


#else
		if( gpioStat_t == GPIO_LOW )
		{
			CH_DI_Print(5, (">>>>>>>>>>>>Antenna short circuit\n"));
			s_stAntState[nUnitId].etCurrentAntState = DI_CH_ANTENNA_FAULT;
			return DI_CH_ANTENNA_FAULT;
		}
		else if( gpioStat_t == GPIO_HI )
		{
			if( s_stAntState[nUnitId].etCurrentAntState == DI_CH_ANTENNA_FAULT )
			{
				nRet = DRV_GPIO_Write(GPIO_ID_ANT_PWR_EN, GPIO_HI);
				if( nRet < 0 )
				{
					CH_DI_Print(0, ("DP10048_CheckAntState Gpio Write Error\n"));
				}
			}
			return DI_CH_OK;

		}
#endif
		else
		{
			CH_DI_Print(0, ("DP10048_CheckAntState\n"));
			return DI_CH_ERR;
		}
	}
	else // DI_CH_ANTENNA_OFF, DI_CH_ANTENNA_FAULT
	{
		return DI_CH_OK;
	}
#endif

	return DI_CH_OK;

}

static HUINT8 Set_OFDM_Tuner(int nChannelId, HUINT32 freq)
{
#if defined(CONFIG_PLATFORM_TM201COMBO)
	HUINT32 	nRet = 0;

	CH_DI_Print(2,("Set_OFDM_Tuner[%d]:: Start! \n", nChannelId));

	g_stSemTunerConfigTer[nChannelId].RF_Freq_Hz	= gTC90517Channel[nChannelId].frequency;
	g_stSemTunerConfigTer[nChannelId].BW_MHz		= MxL_BW_6MHz;

	nRet = MxL_Tuner_RFTune(&g_stSemTunerConfigTer[nChannelId], freq, g_stSemTunerConfigTer[nChannelId].BW_MHz);
	if (nRet != DI_CH_OK)
	{
		CH_DI_Print(0,("MxL_Tuner_RFTune[%d]:: Error! (0x%x)",nChannelId,nRet));
	}
	else
	{
		CH_DI_Print(2,("Set_OFDM_Tuner[%d]:: Success! \n",nChannelId));
	}

	return (!nRet);
#else

	HUINT8	CB = 0x00;
	HUINT8	tunerLockFlag;

	HUINT8	tunerData[6];
	HUINT8	tunerRead;

	float		calcFreq;
	int 		calcBuff;

	int		RSA=0, RSB=0, CP=0, T2=0, T1=0, T0=0, OS=0;
	HUINT32	T_AB, T_TEMP;

	CH_DI_Print(3, ("Set_OFDM_Tuner[%d]:: freq = %d \n", nChannelId, freq));

	 /* Samsung DNV22CX */
	 if(freq <665000000)
	 {
		 if((freq >= 153000000) && (freq < 173000000))
		 {
			 CB = 0xb2;  /* CP = 250uA */
			 CP=0; T2=1; T1=1; T0=0; OS=0;
		 }
		 else
		 {
			 CB = 0xfa;	/* CP = 125uA */
			 CP=1; T2=1; T1=1; T0=1; OS=0;
		 }
	 }
	 else
	 {
		 CB = 0xb2;  /* CP = 250uA */
		 CP=0; T2=1; T1=1; T0=0; OS=0;
	 }

	// ISDB-T (Step_Size = 142.86)
	calcFreq = (freq/142860.0) + (44000000/142860.0) +0.5;

	calcBuff = (int)calcFreq;

	RSA=0, RSB=1;

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
		CH_DI_Print(4, ("Set_OFDM_Tuner[%d]:: BS1  tunerData[3] = %x\n",nChannelId,tunerData[3]));
	}
	else if(freq <= 469000000)
	{
		tunerData[3] = 0x02; /* Band Switch Byte : BS2 */
		CH_DI_Print(4, ("Set_OFDM_Tuner[%d]:: BS2  tunerData[3] = %x\n",nChannelId,tunerData[3]));
	}
	else if(freq <= 810000000)
	{
		tunerData[3] = 0x10; /* Band Switch Byte : BS3 */
		CH_DI_Print(4, ("Set_OFDM_Tuner[%d]:: BS3  tunerData[3] = %x\n",nChannelId,tunerData[3]));
	}
	else
	{
		CH_DI_Print(4,( "Set_OFDM_Tuner[%d]:: ERROR : Out of frequency range for tuner\n",nChannelId));
	}

	// CONTROL BYTE (AB)
	T_AB = 0x30;

	// FOR making CB byte to WRITING Auxiliary Byte   For making T2,T1,T0 = 0, 1, 1
	 T_TEMP = 0x80 | CP<<6 | 0<<5 | 1<<4 | 1<<3 | RSA<<2 | RSB<<1 | OS;

	tunerData[4] = (T_TEMP&0xdf)|0x18;      // save input T_BB 8bit data into dataout[5], For making T2,T1,T0 = 0, 1, 1
	tunerData[5] = T_AB&0xff;			     // save input T_AB 8bit data into dataout[6]

	CH_DI_Print(3, (" Set_OFDM_Tuner[%d]:: [0]=0x%x, [1]=0x%x, [2]=0x%x, [3]=0x%x, [4]=0x%x, [5]=0x%x \n",
								nChannelId,tunerData[0],tunerData[1],tunerData[2],tunerData[3], tunerData[4], tunerData[5]));

	CH_DI_Print(4, ("OFDM Tuner Write[%d] \n",nChannelId));
	Drv_Ch_I2C_TunerWrite(nChannelId, TUNER_DNVS22CX_ADDRESS, &tunerData[0], 6);

	/* Wating for Tuning */
	VK_TASK_Sleep(60);		/* TODO : 이 시간값에 대한 적절한 값을 찾아야 한다. */

//while(1)
{
	/* Locking Check */
	Drv_Ch_I2C_TunerRead(nChannelId, TUNER_DNVS22CX_ADDRESS, &tunerRead, 1);
	CH_DI_Print(2, ("Set_OFDM_Tuner[%d]:: tunerRd=0x%x\n", nChannelId, tunerRead));
	//VK_TASK_Sleep(2000);
}
	tunerLockFlag = (tunerRead & 0x40) ? 1:0;		/* 1 : Locked, 0 : UnLocked */

	return tunerLockFlag;

#endif
}


static void ActivateIsdbDemod(int nChannelId)
{
	HUINT8	val = 0x00;

	CH_DI_Print(3, ("ActivateIsdbDemod[%d]:: Activation of TC90517 \n",nChannelId));

	/* operating mode setting */
	/* received segment mode selection */
	val=0x00;
	Drv_Ch_I2C_DemodWrite(nChannelId, DEMOD_TC90517_ADDRESS, TC90517_02, &val, 1);

	/* layer replaced with null packets */
	val=0x00;
	Drv_Ch_I2C_DemodWrite(nChannelId, DEMOD_TC90517_ADDRESS, TC90517_71, &val, 1);

#if defined (PLATFORM_DT90507)
	/* received segment mode selection for 3 segment reception */
	val=0x00;
	Drv_Ch_I2C_DemodWrite(nChannelId, TC90507_EC, &val, 1);
	// '0':Fixed 3segment, ''1':1segment or 3segment auto selection
#endif

#if defined (PLATFORM_DT90507)
	/*adc power on */
	val = 0x00;
	Drv_Ch_I2C_DemodWrite(nChannelId, TC90507_04, &val, 1);

	val = 0x00;
	Drv_Ch_I2C_DemodWrite(nChannelId, TC90507_03, &val, 1);
#else
//#elif defined (PLATFORM_DT90517)
	/* adc power on & sleep, wakeup time
	[7] slpadc = 0,  (does not power down the ADC for sleep)
	[6:4] slptim = 0, (sets the sleep time)
	[3:0] wuptim = 0 (sets the wakeup time)	*/

	val = 0x00;
	Drv_Ch_I2C_DemodWrite(nChannelId, DEMOD_TC90517_ADDRESS, TC90517_03, &val, 1);
#endif

	VK_TASK_Sleep(100); /* this delay is necessary */
}

/* make demodulator idle at the STAND-BY mode */
static void DeactivateIsdbDemod(int nChannelId)
{
	HUINT8 val;

	CH_UNUSED(nChannelId);

	//CH_DI_Print(3, ("DeactivateIsdbDemod:: Power-Down mode\n"));

#if defined (PLATFORM_DT90507)
	/* Sleep mode */
	val = 0x72;
	Drv_Ch_I2C_DemodWrite(nChannelId, TC90507_03, &val, 1);

	/*adc power down */
	val = 0x20;
	Drv_Ch_I2C_DemodWrite(nChannelId, TC90507_04, &val, 1);
#else
	/* adc power on & sleep, wakeup time
	[7] slpadc = 1,  (set adc power down)
	[6:4] slptim = 3, (sets the sleep time)
	[3:0] wuptim = 2 (sets the wakeup time)	*/

	val = 0xf2;
	//Drv_Ch_I2C_DemodWrite(nChannelId, DEMOD_TC90517_ADDRESS, TC90517_03, &val, 1);
#endif

}

#if(0)
void ClearIsdbDemod(HUINT32 nChannelId)
{
	HUINT8	val;
	CH_PrintMon(" ClearIsdbDemod ::\n");
	/* soft-reset the TC90507 */
	val = 0x40; // Demode Reset
	Drv_Ch_I2C_DemodWrite(nChannelId,   TC90517_01, &val, 1);
	VK_TASK_Sleep(30);
}

HUINT8	FastLockIsdbTest(HUINT32 nChannelId)
{
	HUINT8	seqVal;
	HUINT8	val;
	HUINT32	lockingCnt = 0;

	do
	{
		Drv_Ch_I2C_DemodRead(nChannelId, 	TC90517_B0, &seqVal, 1);
		seqVal &= 0x0F;
		CH_DI_Print("FastLockIsdb[%d]::	try=%d, Sequence =0x%x\n", nChannelId, lockingCnt, seqVal);
		lockingCnt++;
		VK_TASK_Sleep(20);
	}while(seqVal < 0x09);

//	CH_DI_Print ("FastLockIsdb[%d]::	try=%d, Sequence =0x%x\n", nChannelId, lockingCnt, seqVal));

	lockingCnt = 0;

	do
	{
		Drv_Ch_I2C_DemodRead(nChannelId, 	TC90517_80, &val, 1);
		CH_DI_Print("FastLockIsdb[%d]::	try=%d, SYNC=0x%x\n", nChannelId, lockingCnt, val);
		lockingCnt++;
		VK_TASK_Sleep(20);
	}while(val != 0x00);

//	CH_DI_Print ("FastLockIsdb[%d]::	try=%d, SYNC=0x%x\n", nChannelId, lockingCnt, val));

	return TRUE;
}
#endif

#if defined(CONFIG_PLATFORM_TM201COMBO)
static HUINT8 FastLockIsdb(int  nChannelId)
{
   HUINT8 lockVal, seqVal, rlockVal = 0;
   HUINT8 i=0, nDelay = 0, nRetryCount=0;
   HUINT8 rlocka = 0, rlockb = 0, rlockc = 0;

   gTC90517Channel[nChannelId].strength = 0;
   gTC90517Channel[nChannelId].quality = 0;
   gTC90517Channel[nChannelId].snr = 0;

   Drv_Ch_I2C_DemodRead(nChannelId, DEMOD_TC90517_ADDRESS, TC90517_B0, &seqVal, 1);
   CH_DI_Print(3, ("FastLockIsdb[%d]:: Sequence =0x%x\n", nChannelId, seqVal&0x07));

   /* Check AGC Level Detect */
   if( CheckIsdbAgcLock(nChannelId) )
   {
             CH_DI_Print(2, ("FastLockIsdb[%d]::  AGC Level Detect >> Pass\n", nChannelId));

   }
   else
   {
             CH_DI_Print(1, ("FastLockIsdb[%d]::  AGC Level Detect >> Fail\n", nChannelId));

             ofdmUncorPkt[nChannelId][0] = 0;
             ofdmUncorPkt[nChannelId][1] = 0;
             ofdmUncorPkt[nChannelId][2] = 0;

             return FALSE;
   }

   VK_TASK_Sleep(400);

   nDelay = 50;
   nRetryCount = 9;
//   nRetryCount = 20;

   for(i=0; i< nRetryCount; i++)
   {
             VK_TASK_Sleep(nDelay);
             Drv_Ch_I2C_DemodRead(nChannelId, DEMOD_TC90517_ADDRESS, TC90517_80, &lockVal, 1);

             if(((lockVal&0x8) ==0x08   )&& ((lockVal&0x80) == 0x80) )
             {
                        CH_DI_Print(5, ("FastLockIsdb[%d]:: Num = %d, Retry Over\n", nChannelId, i));
                        break;
             }

             if((lockVal&0x28) == 0)
             {
                        CH_DI_Print(5, ("FastLockIsdb[%d]:: Tmcc Valid Syn Locked[%x]!, Num = %d\n", nChannelId, lockVal, i));

                        Drv_Ch_I2C_DemodRead(nChannelId, DEMOD_TC90517_ADDRESS, TC90517_96, &rlockVal, 1);
                        rlocka = (rlockVal >> 7) & 0x01;   //0: error exist, 1: non-error
                        rlockb = (rlockVal >> 6) & 0x01;
                        rlockc = (rlockVal >> 5) & 0x01;

                        if(rlocka || rlockb || rlockc) //csm, change, 셋 중 하나만 1이어도 lock 으로 인정
                        {
                                  CH_DI_Print(1, ("FastLockIsdb[%d]:: Locked[%x]!, Num = %d\n", nChannelId, rlockVal, i));
                                  return TRUE;
                        }

             }

   }
   CH_DI_Print(4, ("FastLockIsdb[%d]:: TC90517_80 = 0x%x\n", nChannelId, lockVal));

   ofdmUncorPkt[nChannelId][0] = 0;
   ofdmUncorPkt[nChannelId][1] = 0;
   ofdmUncorPkt[nChannelId][2] = 0;

   CH_DI_Print(1, ("FastLockIsdb[%d]:: Lock Fail! TC90517_80 = %x, TC90517_96 = %x, Num = %d\n", nChannelId, lockVal, rlockVal,i));

   return FALSE;


}

#else
static HUINT8 FastLockIsdb(int  nChannelId)
{
	HUINT8	lockVal, seqVal;
	HUINT8 	i=0, nDelay = 0, nRetryCount=0;

	gTC90517Channel[nChannelId].strength = 0;
	gTC90517Channel[nChannelId].quality = 0;
	gTC90517Channel[nChannelId].snr = 0;

	Drv_Ch_I2C_DemodRead(nChannelId, DEMOD_TC90517_ADDRESS, TC90517_B0, &seqVal, 1);
	CH_DI_Print(3, ("FastLockIsdb[%d]::	Sequence =0x%x\n", nChannelId, seqVal&0x07));

	Drv_Ch_I2C_DemodRead(nChannelId, DEMOD_TC90517_ADDRESS, TC90517_0F, &seqVal, 1);
	CH_DI_Print(3, ("#####FastLockIsdb[%d]::	TS Out =0x%x\n", nChannelId, seqVal));

	/* Check AGC Level Detect */
	if( CheckIsdbAgcLock(nChannelId) )
	{
		CH_DI_Print(2, ("FastLockIsdb[%d]::  AGC Level Detect >> Pass\n", nChannelId));

	}
	else
	{
		CH_DI_Print(1, ("FastLockIsdb[%d]::  AGC Level Detect >> Fail\n", nChannelId));

		ofdmUncorPkt[nChannelId][0] = 0;
		ofdmUncorPkt[nChannelId][1] = 0;
		ofdmUncorPkt[nChannelId][2] = 0;

		return FALSE;
	}

	VK_TASK_Sleep(400);

	nDelay = 50;
	nRetryCount = 9;

	for(i=0; i< nRetryCount; i++)
	{
		VK_TASK_Sleep(nDelay);
		Drv_Ch_I2C_DemodRead(nChannelId, DEMOD_TC90517_ADDRESS, TC90517_80, &lockVal, 1);

		//if((lockVal&0x39) == 0)
		if((lockVal&0x3b) == 0)
		{
			CH_DI_Print(0, ("FastLockIsdb[%d]:: Locked!, Num = %d\n", nChannelId, i));
			return TRUE;
		}
	}

	ofdmUncorPkt[nChannelId][0] = 0;
	ofdmUncorPkt[nChannelId][1] = 0;
	ofdmUncorPkt[nChannelId][2] = 0;

	CH_DI_Print(1, ("FastLockIsdb[%d]:: Lock Fail! TC90517_80 = %x, Num = %d\n", nChannelId, lockVal, i));

	return FALSE;


}
#endif
static HUINT8 CheckIsdbAgcLock(int  nChannelId)
{
	HUINT8	val1, val2;

	/* Read the value of AGC loops */
	Drv_Ch_I2C_DemodRead(nChannelId, DEMOD_TC90517_ADDRESS, TC90517_83, &val1, 1);//RF_AGC
	Drv_Ch_I2C_DemodRead(nChannelId, DEMOD_TC90517_ADDRESS, TC90517_82, &val2, 1);//IF_AGC
	CH_DI_Print(2, ("CheckIsdbAgcLock[%d]:: RF_AGC = 0x%x (%d), IF_AGC= 0x%x (%d)\n", nChannelId, val1,val1,val2, val2));

#if defined(CONFIG_PLATFORM_TM201COMBO)
	if ( ((val1==0xff) && (val2==0xff)) )
#else
	if ( ((val1==0xff) && (val2==0xff)) || ((val1==0xff) && (val2 > 114)))
#endif
		return FALSE;
	else
		return TRUE;
#if 0
	Drv_Ch_I2C_DemodRead(nChannelId, DEMOD_TC90517_ADDRESS,  TC90517_80, &agcFlag, 1);//level error check
	if ( (agcFlag && 0x40)==0x40 )
		return FALSE;//lock
	else
		return TRUE;//unlock
#endif
}

static void GetUnCorrectedPacket(int  nChannelId, HUINT32 *unCorPkt)
{
	HUINT8	errCnt[3];
	HUINT8	val;
	HUINT8	berCalMode;		/* 0 : after RS decoding, 1 : after Viterbi decoding */
	HUINT32  uncor_pkt;

	/* Check the BER Calculation mode */
	Drv_Ch_I2C_DemodRead(nChannelId, DEMOD_TC90517_ADDRESS, TC90517_77, &berCalMode, 1);
	if(berCalMode & 0x01)
	{
		CH_DI_Print(4, ("[GetUnCorrectedPacket[%d]]FEC After Viterbi Mode.\n",nChannelId));
	}
	else
	{
		CH_DI_Print(4, ("[GetUnCorrectedPacket[%d]]FEC After RSDecod Mode.\n",nChannelId));
	}

	/* Get RS decoding Error */
	Drv_Ch_I2C_DemodRead(nChannelId, DEMOD_TC90517_ADDRESS, TC90517_96, &val, 1);

	/* Layer A */
	if((val & 0x80) == 0x80)	/* No error */
	{
		/* Get the number of uncorrected packet Layer A */
		Drv_Ch_I2C_DemodRead(nChannelId, DEMOD_TC90517_ADDRESS,  TC90517_9D, &errCnt[0], 3);
		unCorPkt[0]  = (((HUINT32)errCnt[0])<<16) | (((HUINT32)errCnt[1])<<8) |(HUINT32)errCnt[2];
		CH_DI_Print(3, ("[GetUnCorrectedPacket]bus[%d] Layer A Corrected Packet = %d\n", nChannelId, unCorPkt[0]));

		uncor_pkt = unCorPkt[0] ;

		//if( (show_di_ch > 0 && uncor_pkt> 1) || uncor_pkt  > 30)
		if( uncor_pkt  > 1)
		{
			if(ofdmUncorPkt[nChannelId][0]  != uncor_pkt)
			{
				CH_DI_Print(0,("[TER] Sig_UnCorrected_PKT[%d]::  [A] layer, UnCorrected Packet   = %d\n",nChannelId,uncor_pkt));
				ofdmUncorPkt[nChannelId][0]	= uncor_pkt;
			}
		}
	}
	else	/* Error */
	{
		unCorPkt[0] = 0;
	   	CH_DI_Print(3, ("[GetUnCorrectedPacket[%d]] Layer A Error-Decision Error\n",nChannelId));
	}

	/* Layer B */
	if((val & 0x40) == 0x40)	/* No error */
	{
		/* Get the number of uncorrected packet Layer B */
		Drv_Ch_I2C_DemodRead(nChannelId, DEMOD_TC90517_ADDRESS,  TC90517_A0, &errCnt[0], 3);
		unCorPkt[1]  = (((HUINT32)errCnt[0])<<16) | (((HUINT32)errCnt[1])<<8) |(HUINT32)errCnt[2] ;
		CH_DI_Print(3, ("[GetUnCorrectedPacket]bus[%d] Layer B Corrected Packet = %d\n", nChannelId, unCorPkt[1]));

		uncor_pkt = unCorPkt[1] ;

		//if( (show_di_ch > 0 && uncor_pkt> 1) || uncor_pkt  > 30)
		if( uncor_pkt  > 1)
		{
			if(ofdmUncorPkt[nChannelId][1]  != uncor_pkt)
			{
				CH_DI_Print(0,("[TER] Sig_UnCorrected_PKT[%d]::  [B] layer, UnCorrected Packet   = %d\n",nChannelId,uncor_pkt));
				ofdmUncorPkt[nChannelId][1]	= uncor_pkt;
			}
		}
	}
	else	/* Error */
	{
		unCorPkt[1] = 0;
		CH_DI_Print(3, ("[GetUnCorrectedPacket[%d]] Layer B Error-Decision Error\n",nChannelId));
	}

	/* Layer C */
	if((val & 0x20) == 0x20)	/* No error */
	{
		/*	Get the PACKET_UncorrectNumber_count :layer C */
		Drv_Ch_I2C_DemodRead(nChannelId, DEMOD_TC90517_ADDRESS,  TC90517_A3, &errCnt[0], 3);
		unCorPkt[2]  = (((HUINT32)errCnt[0])<<16) | (((HUINT32)errCnt[1])<<8) |(HUINT32)errCnt[2];
		CH_DI_Print(3, ("[GetUnCorrectedPacket]bus[%d] Layer C Corrected Packet = %d\n", nChannelId, unCorPkt[2]));

		uncor_pkt = unCorPkt[2] ;

		//if( (show_di_ch > 0 && uncor_pkt> 1) || uncor_pkt  > 30)
		if( uncor_pkt  > 1)
		{
			if(ofdmUncorPkt[nChannelId][2]  != uncor_pkt)
			{
				CH_DI_Print(0,("[TER] Sig_UnCorrected_PKT[%d]::  [C] layer, UnCorrected Packet   = %d\n",nChannelId,uncor_pkt));
				ofdmUncorPkt[nChannelId][2]	= uncor_pkt;
			}
		}
	}
	else	/* Error */
	{
		unCorPkt[2] = 0;
		CH_DI_Print(3, ("[GetUnCorrectedPacket[%d]] Layer C Error-Decision Error\n",nChannelId));
	}
}

static void GetUnErroredPacket(int nChannelId, HUINT32 *unErroredPkt)
{
	HUINT8	Ecnt[2];

	/* Get the number of uncorrected packet Layer A */
	Drv_Ch_I2C_DemodRead(nChannelId, DEMOD_TC90517_ADDRESS,  TC90517_A6, &Ecnt[0], 2);//[15:8]
	unErroredPkt[0] = ((HUINT32)Ecnt[0]<<8) |(HUINT32)Ecnt[1] ;
	CH_DI_Print(5, ("GetUnErroredPacket[%d]:: Layer A Unerrored Packet = %d\n", nChannelId, unErroredPkt[0]));

	/* Get the number of uncorrected packet Layer B */
	Drv_Ch_I2C_DemodRead(nChannelId, DEMOD_TC90517_ADDRESS,  TC90517_A8, &Ecnt[0], 2);//[15:8]
	unErroredPkt[1] = ((HUINT32)Ecnt[0]<<8) |(HUINT32)Ecnt[1] ;
	CH_DI_Print(5, ("GetUnErroredPacket[%d]:: Layer B Unerrored Packet = %d\n", nChannelId, unErroredPkt[1]));

	/*	Get the PACKET_UncorrectNumber_count :layer C */
	Drv_Ch_I2C_DemodRead(nChannelId, DEMOD_TC90517_ADDRESS,  TC90517_AA, &Ecnt[0], 2);//[15:8]
	unErroredPkt[2] = ((HUINT32)Ecnt[0]<<8) |(HUINT32)Ecnt[1] ;
	CH_DI_Print(5, ("GetUnErroredPacket[%d]:: Layer C Unerrored Packet = %d\n", nChannelId, unErroredPkt[2]));
}


#if 0
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
				offset = 5;
			}
			break;
		}
	}

	CH_DI_Print(5, ("[%d] DsPwrIndexT = %d, DsPwrIndexI  = %d\n", nChannelId, DsPwrIndexT, DsPwrIndexI));

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
		strength = (dslevel[level]*10)+offset - 600;   /* dBuV -> dBmV  */
	}

	CH_DI_Print(4, ("GetIsdbStrength: strength = %d, level index = %d\n", strength, level));

	return strength;
}
#endif

static HINT16 GetIsdbStrength(int nChannelId)
{

	HUINT8	rfagc, ifagc;
	HINT16  strength=0;

#if 0
	int level;
	int DsPwrIndexT, DsPwrIndexI;
	HINT16  strength=0, offset;

	level = 0;
	strength = 0;
	offset = 0;
	DsPwrIndexT = 0;
	DsPwrIndexI = 0;
#endif

   	Drv_Ch_I2C_DemodRead(nChannelId, DEMOD_TC90517_ADDRESS,  TC90517_83, &rfagc, 1);//RF_AGC
    	Drv_Ch_I2C_DemodRead(nChannelId, DEMOD_TC90517_ADDRESS,  TC90517_82, &ifagc, 1);//IF_AGC

      	CH_DI_Print(4, (" GetIsdbStrength[%d]::  rfagc = %d, ifagc = %d\n",nChannelId,rfagc, ifagc));

	strength = New_GetSignalLevel(gTC90517Channel[nChannelId].frequency, rfagc, ifagc);

#if 0
    	if (gTC90517Channel[nChannelId].frequency < 183000000)
    	{
    		strength = CalcIsdbStrength(nChannelId, levelNo, rfagc, ifagc, isdbtRfAgcGain1, isdbtIfAgcGain1);
    	}
    	else if (gTC90517Channel[nChannelId].frequency < 473000000)
    	{
    		strength = CalcIsdbStrength(nChannelId, levelNo, rfagc, ifagc, isdbtRfAgcGain2, isdbtIfAgcGain2);
    	}
    	else if (gTC90517Channel[nChannelId].frequency < 521000000)
    	{
    		strength = CalcIsdbStrength(nChannelId, levelNo, rfagc, ifagc, isdbtRfAgcGain3, isdbtIfAgcGain3);
    	}
    	else if (gTC90517Channel[nChannelId].frequency < 623000000)
    	{
    		strength = CalcIsdbStrength(nChannelId, levelNo, rfagc, ifagc, isdbtRfAgcGain4, isdbtIfAgcGain4);
    	}
    	else
    	{
    		strength = CalcIsdbStrength(nChannelId, levelNo, rfagc, ifagc, isdbtRfAgcGain5, isdbtIfAgcGain5);
    	}
#endif


	CH_DI_Print(3, ("GetIsdbStrength[%d]:: strength = %d\n",nChannelId,strength));

	return strength;
}

static HINT16 New_ReadSignalLevel(HINT16 rfPwrIndex,  HINT16 offset)
{
	HINT16	strength;
	HINT16  level;

	strength = 0;

        level = rfPwrIndex;

	if(level == 0)
	{
		strength = 250;
	}
	else if(level == levelNo)
	{
		strength = -160;
	}
	else
	{
		strength = (dslevel[level]*10)+offset-600;      /* dBuV -> dBmV    */
	}


	return strength;
}



static HINT16 New_CalIndex(HINT16 rfagc, HINT16 ifagc, HUINT16 *AgcTable, HINT16 *offset)
{

	HINT16     i, DsPwrIndex=0;
       HINT16    calval =0;

       calval = rfagc+ ifagc;


	for (i=0; i<levelNo; i++)
	{
		if ( calval <= AgcTable[i] )
		{
			DsPwrIndex = i;
			if (calval == AgcTable[i])
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
                                *offset = ((AgcTable[i] - calval) *10) / (AgcTable[i] - AgcTable[i-1]);
                            }
			}
			break;
		}
	}

	if(i == levelNo)
	{
		DsPwrIndex = levelNo;
	}

	CH_DI_Print(5, ("[New_CalIndex] table[%d] = %d, agc = %d, DsPwrIndex = %d, offset = %d\n",  i, AgcTable[i], calval, DsPwrIndex, *offset));


	return DsPwrIndex;
}


static HINT16 New_GetSignalLevel(HUINT32  frequency, HINT16 rfagc, HINT16 ifagc)
{
	HINT16  strength=0;
       HINT16 offset, levelRfIndex = 0;

	if (frequency < 151000000)
	{

	        levelRfIndex = New_CalIndex(rfagc, ifagc, NewAgcGain1, &offset);
	        strength = New_ReadSignalLevel(levelRfIndex, offset);

	}
	else if(frequency < 169000000)
	{
	        levelRfIndex = New_CalIndex(rfagc, ifagc, NewAgcGain2, &offset);
	        strength = New_ReadSignalLevel(levelRfIndex, offset);

	}
	else if (frequency < 187000000)
	{
	        levelRfIndex = New_CalIndex(rfagc, ifagc, NewAgcGain3, &offset);
	        strength = New_ReadSignalLevel(levelRfIndex, offset);

	}
	else if (frequency < 211000000)
	{
		 levelRfIndex = New_CalIndex(rfagc, ifagc, NewAgcGain4, &offset);
	        strength = New_ReadSignalLevel(levelRfIndex, offset);

	}
	else if (frequency < 241000000)
	{

	        levelRfIndex = New_CalIndex(rfagc, ifagc, NewAgcGain5, &offset);
	        strength = New_ReadSignalLevel(levelRfIndex, offset);

	}
	else if (frequency < 307000000)
	{
	        levelRfIndex = New_CalIndex(rfagc, ifagc, NewAgcGain6, &offset);
	        strength = New_ReadSignalLevel(levelRfIndex, offset);

	}
	else if (frequency < 469000000)
	{
	        levelRfIndex = New_CalIndex(rfagc, ifagc, NewAgcGain7, &offset);
	        strength = New_ReadSignalLevel(levelRfIndex, offset);

	}
	else if (frequency < 513000000)
	{
	        levelRfIndex = New_CalIndex(rfagc, ifagc, NewAgcGain8, &offset);
	        strength = New_ReadSignalLevel(levelRfIndex, offset);

	}
	else if (frequency < 543000000)
	{
	        levelRfIndex = New_CalIndex(rfagc, ifagc, NewAgcGain9, &offset);
	        strength = New_ReadSignalLevel(levelRfIndex, offset);

	}
	else if (frequency < 579000000)
	{
	        levelRfIndex = New_CalIndex(rfagc, ifagc, NewAgcGain10, &offset);
	        strength = New_ReadSignalLevel(levelRfIndex, offset);

	}
	else if (frequency < 645000000)
	{
	        levelRfIndex = New_CalIndex(rfagc, ifagc, NewAgcGain11, &offset);
	        strength = New_ReadSignalLevel(levelRfIndex, offset);

	}
	else
	{
	        levelRfIndex = New_CalIndex(rfagc, ifagc, NewAgcGain12, &offset);
	        strength = New_ReadSignalLevel(levelRfIndex, offset);

	}


	CH_DI_Print(5, ("[New_GetSignalLevel] frq = %d, levelRfIndex = %d, strength = %d\n", frequency, levelRfIndex, strength));

	return strength;
}


static HUINT16 intlog10(HUINT32 value)
{

	HUINT32 indexVal, logVal, dVal, temp;


	if(value == 0)
	{
		indexVal = 0;
		return indexVal;
	}

	indexVal = value/10;
	dVal = value - (indexVal*10);

	if (indexVal < 489)
	{
		temp =  (logtable[indexVal+1] - logtable[indexVal])*(dVal/10.0);
		logVal = temp + logtable[indexVal] ;

	}
	else
	{
		logVal = logtable[489];
	}


	CH_DI_Print(4, ("intlog10: log10[%d]=%d, indexVal = %d, dVal=%d, temp=%d\n", value, logVal, indexVal, dVal, temp));

	return logVal;

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
	Drv_Ch_I2C_DemodRead(nChannelId, DEMOD_TC90517_ADDRESS, TC90517_8B, &Ecnt[0], 3);

	cndat  = (Ecnt[0]<<16) | (Ecnt[1]<<8) |(Ecnt[2]) ;

	CH_DI_Print(5, ("GetOfdmCn[%d]:: [0]=%x, [1]=%x, [2]=%x\n",nChannelId,Ecnt[0], Ecnt[1], Ecnt[2]));

	if (cndat == 0)
	{
		cn = 0;
	}
	else
	{

		P_value=(10*(intlog10(5505024/cndat)/100.0));
		CH_DI_Print(4, ("P_value = %4.2f, cndat=%d\n", P_value, cndat));
		P_value4=PowerOfExponential(P_value,4);
		P_value3=PowerOfExponential(P_value,3);
		P_value2=PowerOfExponential(P_value,2);
		P_value1=PowerOfExponential(P_value,1);

		CH_DI_Print(5, ("%f, %f, %f, %f\n", P_value4,P_value3,P_value2,P_value1));

		CN_value=(float)((0.000024*P_value4) - (0.0016*P_value3)
				+ (0.0398*P_value2) + (0.5491*P_value1) + 3.0965);

		CH_DI_Print(4, ("GetOfdmCn[%d]:: CN_value=%4.2f\n", nChannelId, CN_value ));


		cn = (HUINT16)(CN_value * 10.0);
	}
	return cn;
}

static void GetIsdbtBer(int nChannelId, HUINT32 *berVal, HUINT32 errCnt, HUINT32 errCycle, HUINT32 *ngFlag)
{
	HUINT8	berCalMode;		/* 0 : after RS decoding, 1 : after Viterbi decoding */

	Drv_Ch_I2C_DemodRead(nChannelId, DEMOD_TC90517_ADDRESS, TC90517_77, &berCalMode, 1);

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

	if(gTC90517Layer[nChannelId].layer_A_error == FALSE)
	{
		layerErrStatus |= 0x4;
	}
	if(gTC90517Layer[nChannelId].layer_B_error == FALSE)
	{
		layerErrStatus |= 0x2;
	}
	if(gTC90517Layer[nChannelId].layer_C_error == FALSE)
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

	Drv_Ch_I2C_DemodRead(nChannelId,   TC90517_96, &err, 1);

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
	Drv_Ch_I2C_DemodRead(nChannelId,   TC90517_9D + regCntOffset, &Ecnt[0], 3);//reg[9d]msb [23:16],reg[9e]msb[15:8],reg[9f]/lsb [7:0]
	CH_PrintMon("[Ecnt[0] : %d\n", (HUINT16)Ecnt[0]);
	CH_PrintMon("[Ecnt[1] : %d\n", (HUINT16)Ecnt[1]);
	CH_PrintMon("[Ecnt[2] : %d\n", (HUINT16)Ecnt[2]);
	uncor_pkt  =( ((unsigned long)Ecnt[0])<<16) | (((unsigned long)Ecnt[1])<<8) |(((unsigned long)Ecnt[2])) ;
	CH_PrintMon("[uncor_pkt : %ld\n", uncor_pkt);
	//Get the Number of PACKETs for layer_A count obserbvation cycle
	Drv_Ch_I2C_DemodRead(nChannelId,   TC90517_A6 + regCycleOffset, &Ecnt_cycle[0], 2);//[15:8]
//	Drv_Ch_I2C_DemodRead(nChannelId,   TC90517_A7, &Ecnt_cycle[0], 1);//lsb [7:0]
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
	Drv_Ch_I2C_DemodRead(nChannelId,   TC90517_77, &ber_set_cfg, 1);
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

	Drv_Ch_I2C_DemodRead(nChannelId,   TC90517_96, &rslock, 1);

	if((rslock&0x80) == 0x80)
	{
		/*	Get the PACKET_UncorrectNumber_count :layer A */
		Drv_Ch_I2C_DemodRead(nChannelId,   TC90517_9D, &Ecnt[0], 3);//reg[9d]msb [23:16],reg[9e]msb[15:8],reg[9f]/lsb [7:0]

		CH_PRT(7, ("[Ecnt[0] : %d\n", (HUINT16)Ecnt[0]));
		CH_PRT(7, ("[Ecnt[1] : %d\n", (HUINT16)Ecnt[1]));
		CH_PRT(7, ("[Ecnt[2] : %d\n", (HUINT16)Ecnt[2]));
		uncor_pkt  =( ((unsigned long)Ecnt[0])<<16) | (((unsigned long)Ecnt[1])<<8) |(((unsigned long)Ecnt[2])) ;
		CH_PRT(6, ("[BER] Layer A uncor_pkt [%d] : %ld\n", nChannelId, uncor_pkt));
		//Get the Number of PACKETs for layer_A count obserbvation cycle
		Drv_Ch_I2C_DemodRead(nChannelId,   TC90517_A6, &Ecnt_cycle[1], 1);//[15:8]
		Drv_Ch_I2C_DemodRead(nChannelId,   TC90517_A7, &Ecnt_cycle[0], 1);//lsb [7:0]
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
		Drv_Ch_I2C_DemodRead(nChannelId,   TC90517_77, &ber_set_cfg, 1);
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
		Drv_Ch_I2C_DemodRead(nChannelId,   TC90517_A0, &Ecnt[0], 3);//reg[a0]msb [23:16],reg[a1]msb[15:8],reg[a2]/lsb [7:0]
		uncor_pkt  =( ((unsigned long)Ecnt[0])<<16) | (((unsigned long)Ecnt[1])<<8) |(((unsigned long)Ecnt[2])) ;
			//Get the Number of PACKETs for layer_B count obserbvation cycle

		CH_PRT(6, ("[BER] Layer B uncor_pkt [%d] : %ld\n", nChannelId, uncor_pkt));
		Drv_Ch_I2C_DemodRead(nChannelId,   TC90517_A8, &Ecnt_cycle[1], 1);//[15:8]
		Drv_Ch_I2C_DemodRead(nChannelId,   TC90517_A9, &Ecnt_cycle[0], 1);//lsb [7:0]
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
		Drv_Ch_I2C_DemodRead(nChannelId,   TC90517_A3, &Ecnt[0], 3);//reg[a3]msb [23:16],reg[a4]msb[15:8],reg[a5]/lsb [7:0]
		uncor_pkt  =( ((unsigned long)Ecnt[0])<<16) | (((unsigned long)Ecnt[1])<<8) |(((unsigned long)Ecnt[2])) ;
		CH_PRT(6, ("[BER] Layer C uncor_pkt [%d] : %ld\n", nChannelId, uncor_pkt));
		//Get the Number of PACKETs for layer_C count obserbvation cycle
		Drv_Ch_I2C_DemodRead(nChannelId,   TC90517_AA, &Ecnt_cycle[1], 1);//[15:8]
		Drv_Ch_I2C_DemodRead(nChannelId,   TC90517_AB, &Ecnt_cycle[0], 1);//lsb [7:0]
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
		CH_DI_Print(4, ("pow_positive:: PowerOfExponential(mantissa=0) error\n"));
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

	Drv_Ch_I2C_DemodRead(nChannelId, DEMOD_TC90517_ADDRESS,  TC90517_96, &val, 1);
	CH_DI_Print(3, ("ViewOfdmParam[%d]:: Layer Error [96] :%x\n", nChannelId, val));

	if((val & 0x80) == 0x80)
	{
		gTC90517Layer[nChannelId].layer_A_error=FALSE;  // no error
		CH_DI_Print(3, ("ViewOfdmParam[%d]:: layer_A_error : NO ERROR \n",nChannelId));
	}//D[6]
	else
	{
		gTC90517Layer[nChannelId].layer_A_error=TRUE;
		CH_DI_Print(3, ("ViewOfdmParam[%d]:: layer_A_error : ERROR\n",nChannelId));
	}

	if((val & 0x40) == 0x40)
	{
		gTC90517Layer[nChannelId].layer_B_error=FALSE;
		CH_DI_Print(3, ("ViewOfdmParam[%d]:: layer_B_error : NO ERROR\n",nChannelId));
	}//D[3]
	else
	{
		gTC90517Layer[nChannelId].layer_B_error=TRUE;
		CH_DI_Print(3, ("ViewOfdmParam[%d]:: layer_B_error : ERROR\n",nChannelId));
	}
	if((val & 0x20) == 0x20)
	{
		gTC90517Layer[nChannelId].layer_C_error=FALSE;
		CH_DI_Print(3, ("ViewOfdmParam[%d]:: layer_C_error : NO ERROR\n",nChannelId));
	}//D[5]
	else
	{
		gTC90517Layer[nChannelId].layer_C_error=TRUE;
		CH_DI_Print(3, ("ViewOfdmParam[%d]:: layer_C_error : ERROR\n",nChannelId));
	}

//fft & guard interval
		Drv_Ch_I2C_DemodRead(nChannelId, DEMOD_TC90517_ADDRESS,  TC90517_B0, &val, 1);//0xb0
		CH_DI_Print(3, ("ViewOfdmParam[%d]:: FFT & Guard [0xb0] : %x\n",nChannelId, val));
		ffsize = val & 0xc0;//D7:6
		gdleng = val & 0x30;//D5:4

	   switch( ffsize )
	   {
		case 0x00:
			gTC90517Channel[nChannelId].TPS_fft_info=fft_2K;
			val=0x04;
			Drv_Ch_I2C_DemodWrite(nChannelId,  DEMOD_TC90517_ADDRESS,  TC90517_BA, &val, 1);
			val=0x00;
			Drv_Ch_I2C_DemodWrite(nChannelId, DEMOD_TC90517_ADDRESS,   TC90517_BB, &val, 1);
			CH_DI_Print(3, ("ViewOfdmParam[%d]:: FFT mode : fft_2K \n",nChannelId));
			break;//1024
		case 0x40:
			gTC90517Channel[nChannelId].TPS_fft_info=fft_4K;
			val=0x08;
			Drv_Ch_I2C_DemodWrite(nChannelId, DEMOD_TC90517_ADDRESS,  TC90517_BA, &val, 1);
			val=0x00;
			Drv_Ch_I2C_DemodWrite(nChannelId, DEMOD_TC90517_ADDRESS,   TC90517_BB, &val, 1);
			CH_DI_Print(3, ("ViewOfdmParam[%d]:: FFT mode : fft_4K \n",nChannelId));
			break;//2048
		case 0x80:
			gTC90517Channel[nChannelId].TPS_fft_info=fft_8K;
			val=0x0f;
			Drv_Ch_I2C_DemodWrite(nChannelId, DEMOD_TC90517_ADDRESS,   TC90517_BA, &val, 1);
			val=0x00;
			Drv_Ch_I2C_DemodWrite(nChannelId, DEMOD_TC90517_ADDRESS,   TC90517_BB, &val, 1);
			CH_DI_Print(3, ("ViewOfdmParam[%d]:: FFT mode : fft_8K \n",nChannelId));
			break;//4000
		case 0xc0:
			gTC90517Channel[nChannelId].TPS_fft_info=fft_NOT;
	   		CH_DI_Print(3, ("ViewOfdmParam[%d]:: FFT mode : Nothing\n",nChannelId));
	   		break;
		}

	   switch( gdleng )
	   {
		case 0x00:
			gTC90517Channel[nChannelId].TPS_guard_info=guard_1_32;
   			CH_DI_Print(3, ("ViewOfdmParam[%d]:: Guard Interval : 1/32 \n",nChannelId));
			break;
	   	case 0x10:
			gTC90517Channel[nChannelId].TPS_guard_info=guard_1_16;
   			CH_DI_Print(3, ("ViewOfdmParam[%d]:: Guard Interval : 1/16 \n",nChannelId));
			break;
	   	case 0x20:
			gTC90517Channel[nChannelId].TPS_guard_info=guard_1_8;
   			CH_DI_Print(3, ("ViewOfdmParam[%d]:: Guard Interval : 1/8\n",nChannelId));
			break;
	   	case 0x30:
			gTC90517Channel[nChannelId].TPS_guard_info=guard_1_4;
   			CH_DI_Print(3, ("ViewOfdmParam[%d]:: Guard Interval : 1/4\n",nChannelId));
			break;
	   }

	/* LAYER A */
		Drv_Ch_I2C_DemodRead(nChannelId, DEMOD_TC90517_ADDRESS,   TC90517_B3, &val, 1);//0xb3//D15:8
		CH_DI_Print(3, ("ViewOfdmParam[%d]:: Layer A [0xB3] :0x%x  \n",nChannelId,val));
		index1 = (int)val;
		Drv_Ch_I2C_DemodRead(nChannelId, DEMOD_TC90517_ADDRESS,   TC90517_B4, &val, 1);//0xb4//D7:0
		CH_DI_Print(3, ("ViewOfdmParam[%d]:: Layer A [0xB4] :0x%x \n",nChannelId,val));
		index2 = (int)val;
		index4 = ((index1<<8)&0xff00)|(index2&0x00ff);

	 // Segment number, reg[b4][6:3]

		Num_seg= (HUINT8 )((index2 & 0x0078)>>3);
		gTC90517Channel[nChannelId].layerinfo.layer_A_segments=Num_seg;
		CH_DI_Print(3, ("ViewOfdmParam[%d]:: Layer A segment : %d \n",nChannelId, Num_seg));

	  // Modulation
	   switch(index4 & 0xe000)//reg[b3][7:5]
	   {
	   	case 0x0000:
	   		gTC90517Channel[nChannelId].layerinfo.layer_A_constellation=constellation_DQPSK;
	   		CH_DI_Print(3, ("ViewOfdmParam[%d]:: Layer A Modulation : DQPSK\n",nChannelId));
			break;
	   	case 0x2000:
			gTC90517Channel[nChannelId].layerinfo.layer_A_constellation=constellation_QPSK;
	   		CH_DI_Print(3, ("ViewOfdmParam[%d]:: Layer A Modulation : QPSK\n",nChannelId));
			break;
		case 0x4000:
			gTC90517Channel[nChannelId].layerinfo.layer_A_constellation=constellation_16QAM;
	   		CH_DI_Print(3, ("ViewOfdmParam[%d]:: Layer A Modulation : 16QAM\n",nChannelId));
			break;
	   	case 0x6000:
			gTC90517Channel[nChannelId].layerinfo.layer_A_constellation=constellation_64QAM;
	   		CH_DI_Print(3, ("ViewOfdmParam[%d]:: Layer A Modulation : 64QAM\n",nChannelId));
			break;
	   	case 0xE000:
			gTC90517Channel[nChannelId].layerinfo.layer_A_constellation=constellation_NOTHING;
	   		CH_DI_Print(3, ("ViewOfdmParam[%d]:: Layer A Modulation : Nothing\n",nChannelId));
			break;
	   }
	 // Code Rate
	   switch(index4 & 0x1C00)//reg[b3][4:2]
	   {
	   	case 0x0000:
			gTC90517Channel[nChannelId].layerinfo.layer_A_CodeRate=CodeRate_1_2;
	   		CH_DI_Print(3,("ViewOfdmParam[%d]:: Layer A Code Rate :1/2 \n",nChannelId));
			break;
	   	case 0x0400:
			gTC90517Channel[nChannelId].layerinfo.layer_A_CodeRate=CodeRate_2_3;
	   		CH_DI_Print(3, ("ViewOfdmParam[%d]:: Layer A Code Rate : 2/3\n",nChannelId));
			break;
	   	case 0x0800:
			gTC90517Channel[nChannelId].layerinfo.layer_A_CodeRate=CodeRate_3_4;
	   		CH_DI_Print(3, ("ViewOfdmParam[%d]:: Layer A Code Rate : 3/4\n",nChannelId));
			break;
	   	case 0x0C00:
			gTC90517Channel[nChannelId].layerinfo.layer_A_CodeRate=CodeRate_5_6;
	   		CH_DI_Print(3, ("ViewOfdmParam[%d]:: Layer A Code Rate : 5/6\n",nChannelId));
			break;
	  	 case 0x1000:
		 	gTC90517Channel[nChannelId].layerinfo.layer_A_CodeRate=CodeRate_7_8;
	  		CH_DI_Print(3, ("ViewOfdmParam[%d]:: Layer A Code Rate : 7/8\n",nChannelId));
			break;
	   	case 0x1C00:
			gTC90517Channel[nChannelId].layerinfo.layer_A_CodeRate=CodeRate_NOT;
	   		CH_DI_Print(3, ("ViewOfdmParam[%d]:: Layer A Code Rate : Nothing\n",nChannelId));
			break;
		}
	  // Interleave
	   switch(ffsize)
	   {
	   	case 0x00://  2k
			switch(index4 & 0x0380)//reg[b3][1:0]reg[b4][7]
			{
		   	case 0x0000:
				gTC90517Channel[nChannelId].layerinfo.layer_A_Interleave=0;
				CH_DI_Print(3, ("ViewOfdmParam[%d]:: Layer A Interleave : 0 \n",nChannelId));
				break;//("I = 0")
		   	case 0x0080: gTC90517Channel[nChannelId].layerinfo.layer_A_Interleave=4;
				CH_DI_Print(3, ("ViewOfdmParam[%d]:: Layer A Interleave : 4 \n",nChannelId));
				 break;//("I = 4")
		   	case 0x0100: gTC90517Channel[nChannelId].layerinfo.layer_A_Interleave=8;
				CH_DI_Print(3, ("ViewOfdmParam[%d]:: Layer A Interleave : 8\n",nChannelId));
				break;//("I = 8")
		   	case 0x0180: gTC90517Channel[nChannelId].layerinfo.layer_A_Interleave=16;
				CH_DI_Print(3, ("ViewOfdmParam[%d]:: Layer A Interleave : 16 \n",nChannelId));
				break;//("I = 16")
		   	case 0x0200: gTC90517Channel[nChannelId].layerinfo.layer_A_Interleave=32;
				CH_DI_Print(3, ("ViewOfdmParam[%d]:: Layer A Interleave : 32 \n",nChannelId));
				break;//("I = 32")
		   	case 0x0380: //gTC90517Channel[nChannelId].layerinfo.layer_A_Interleave=0;
				CH_DI_Print(3, ("ViewOfdmParam[%d]:: Layer A Interleave : Nothing \n",nChannelId));
				break;
			}
			break;
	   case 0x40:////	4k
		switch(index4 & 0x0380)//reg[b3][1:0]reg[b4][7]
		{
	   		case 0x0000:
				gTC90517Channel[nChannelId].layerinfo.layer_A_Interleave=0;
				CH_DI_Print(3, ("ViewOfdmParam[%d]:: Layer A Interleave : 0 \n",nChannelId));
				    break;//("I = 0")
		   	case 0x0080:
				gTC90517Channel[nChannelId].layerinfo.layer_A_Interleave=2;
				CH_DI_Print(3, ("ViewOfdmParam[%d]:: Layer A Interleave : 2\n",nChannelId));
				break;//("I = 2")
		   	case 0x0100:
				gTC90517Channel[nChannelId].layerinfo.layer_A_Interleave=4;
				CH_DI_Print(3, ("ViewOfdmParam[%d]:: Layer A Interleave : 4 \n",nChannelId));
				break;//("I = 4")
		   	case 0x0180:
				gTC90517Channel[nChannelId].layerinfo.layer_A_Interleave=8;
				CH_DI_Print(3, ("ViewOfdmParam[%d]:: Layer A Interleave : 8 \n",nChannelId));
				break;//("I = 8")
		   	case 0x0200:
				gTC90517Channel[nChannelId].layerinfo.layer_A_Interleave=16;
				CH_DI_Print(3, ("ViewOfdmParam[%d]:: Layer A Interleave : 16\n",nChannelId));
				break;//("I = 16")
			   case 0x0380: //gTC90517Channel[nChannelId].layerinfo.layer_A_Interleave=0;
			   	CH_DI_Print(3, ("ViewOfdmParam[%d]:: Layer A Interleave : Nothing \n",nChannelId));
			   	break;
		}
		break;
	   case 0x80://		8k
		switch(index4 & 0x0380)//reg[b3][1:0]reg[b4][7]
		{
		   	case 0x0000:
				gTC90517Channel[nChannelId].layerinfo.layer_A_Interleave=0;
				CH_DI_Print(3, ("ViewOfdmParam[%d]:: Layer A Interleave : 0 \n",nChannelId));
				break;//("I = 0 ")
		   	case 0x0080:
				gTC90517Channel[nChannelId].layerinfo.layer_A_Interleave=1;
				CH_DI_Print(3, ("ViewOfdmParam[%d]:: Layer A Interleave : 1 \n",nChannelId));
				break;//("I = 1")
		   	case 0x0100:
				gTC90517Channel[nChannelId].layerinfo.layer_A_Interleave=2;
				CH_DI_Print(3, ("ViewOfdmParam[%d]:: Layer A Interleave : 2 \n",nChannelId));
				break;//("I = 2")
		   	case 0x0180:
				gTC90517Channel[nChannelId].layerinfo.layer_A_Interleave=4;
				CH_DI_Print(3, ("ViewOfdmParam[%d]:: Layer A Interleave : 4 \n",nChannelId));
				break;//("I = 4")
		   	case 0x0200:
				gTC90517Channel[nChannelId].layerinfo.layer_A_Interleave=8;
				CH_DI_Print(3, ("ViewOfdmParam[%d]:: Layer A Interleave : 8 \n",nChannelId));
				break;//("I = 8")
		   	case 0x0380: //gTC90517Channel[nChannelId].layerinfo.layer_A_Interleave=0;
				CH_DI_Print(3, ("ViewOfdmParam[%d]:: Layer A Interleave : Nothing\n",nChannelId));
		   		break;
		}
		break;
	   }

	/* LAYER B */
		Drv_Ch_I2C_DemodRead(nChannelId, DEMOD_TC90517_ADDRESS,   TC90517_B4, &val, 1);//0xb4///D2:0
		index1=(int)val;
		Drv_Ch_I2C_DemodRead(nChannelId, DEMOD_TC90517_ADDRESS,   TC90517_B5, &val, 1);//0xb5//D7:0
		CH_DI_Print(3, ("ViewOfdmParam[%d]:: Layer B [B5] :0x%x \n",nChannelId,val));
		index2=(int)val;
		Drv_Ch_I2C_DemodRead(nChannelId, DEMOD_TC90517_ADDRESS,   TC90517_B6, &val, 1);//0xb6//D7:0
		CH_DI_Print(3, ("ViewOfdmParam[%d]:: Layer B [B6] :0x%x \n",nChannelId,val));
		index3=(int)val;

		index4=((index2<<8)&0xff00)|(index3&0x00ff);
	  //number of segment//reg[b5][1:0]reg[b6][7:6]
		index4=(index4 & 0x03c0)>>6;
		Num_seg=(unsigned char)(index4);
		gTC90517Channel[nChannelId].layerinfo.layer_B_segments=Num_seg;
		CH_DI_Print(3, ("ViewOfdmParam[%d]:: layer_B_segments : %d *****\n",nChannelId,Num_seg));

		//retry reading
		Drv_Ch_I2C_DemodRead(nChannelId, DEMOD_TC90517_ADDRESS,   TC90517_B4, &val, 1);//0xb4///D2:0
		index1=(int)val;
		Drv_Ch_I2C_DemodRead(nChannelId, DEMOD_TC90517_ADDRESS,   TC90517_B5, &val, 1);//0xb5//D7:0
		index2=(int)val;
		Drv_Ch_I2C_DemodRead(nChannelId, DEMOD_TC90517_ADDRESS,   TC90517_B6, &val, 1);//0xb6//D7:0
		index3=(int)val;
		index4=((index1<<8)&0xff00)|(index2&0x00ff);
	  // constellation
	   switch(index4 & 0x0700)//reg[b4][2:0]
	   {
	   case 0x0000:
	   	gTC90517Channel[nChannelId].layerinfo.layer_B_constellation=constellation_DQPSK;
	   	CH_DI_Print(3, ("ViewOfdmParam[%d]:: Layer B Constellation : DQPSK\n",nChannelId));
		break;
	   case 0x0100:
	   	gTC90517Channel[nChannelId].layerinfo.layer_B_constellation=constellation_QPSK;
	   	CH_DI_Print(3, ("ViewOfdmParam[%d]:: Layer B Constellation : QPSK\n",nChannelId));
		break;
	   case 0x0200:
	   	gTC90517Channel[nChannelId].layerinfo.layer_B_constellation=constellation_16QAM;
	   	CH_DI_Print(3, ("ViewOfdmParam[%d]:: Layer B Constellation : 16QAM\n",nChannelId));
		break;
	   case 0x0300:
	   	gTC90517Channel[nChannelId].layerinfo.layer_B_constellation=constellation_64QAM;
	   	CH_DI_Print(3, ("ViewOfdmParam[%d]:: Layer B Constellation : 64QAM \n",nChannelId));
		break;
	   case 0x0700:
	   	gTC90517Channel[nChannelId].layerinfo.layer_B_constellation=constellation_NOTHING;
	   	CH_DI_Print(3, ("ViewOfdmParam[%d]:: Layer B Constellation : Nothing \n",nChannelId));
		 break;
	   }
	 // HP code rate(Iner code)
	   switch(index4 & 0x00e0)//reg[b5][7:5]
	   {
	   case 0x0000:
	   	gTC90517Channel[nChannelId].layerinfo.layer_B_CodeRate=CodeRate_1_2;
	   	CH_DI_Print(3, ("ViewOfdmParam[%d]:: Layer B Code Rate : 1/2\n",nChannelId));
		break;
	   case 0x0020:
	   	gTC90517Channel[nChannelId].layerinfo.layer_B_CodeRate=CodeRate_2_3;
	   	CH_DI_Print(3, ("ViewOfdmParam[%d]:: Layer B Code Rate : 2/3\n",nChannelId));
		break;
	   case 0x0040:
	   	gTC90517Channel[nChannelId].layerinfo.layer_B_CodeRate=CodeRate_3_4;
	   	CH_DI_Print(3, ("ViewOfdmParam[%d]:: Layer B Code Rate : 3/4\n",nChannelId));
		break;
	   case 0x0060:
	   	gTC90517Channel[nChannelId].layerinfo.layer_B_CodeRate=CodeRate_5_6;
	   	CH_DI_Print(3, ("ViewOfdmParam[%d]:: Layer B Code Rate : 5/6\n",nChannelId));
		break;
	   case 0x0080:
	   	gTC90517Channel[nChannelId].layerinfo.layer_B_CodeRate=CodeRate_7_8;
	   	CH_DI_Print(3, ("ViewOfdmParam[%d]:: Layer B Code Rate : 7/8\n",nChannelId));
		break;
	   case 0x00e0:
	   	gTC90517Channel[nChannelId].layerinfo.layer_B_CodeRate=CodeRate_NOT;
	   	CH_DI_Print(3, ("ViewOfdmParam[%d]:: Layer B Code Rate : Nothing\n",nChannelId));
		break;
       }
	  // LP code rate(Time interleave)
	   switch(ffsize)
	   {
	   case 0x00://	2k
		switch(index4 & 0x001c)//reg[b5][4:2]
		{
			case 0x0000:
				gTC90517Channel[nChannelId].layerinfo.layer_B_Interleave=0;
				CH_DI_Print(3, ("ViewOfdmParam[%d]:: Layer B Interleave: 0 \n",nChannelId));
				 break;//("I = 0")
		    	case 0x0004:
				gTC90517Channel[nChannelId].layerinfo.layer_B_Interleave=4;
				CH_DI_Print(3, ("ViewOfdmParam[%d]:: Layer B Interleave : 4 \n",nChannelId));
				break;//("I = 4")
		    	case 0x0008:
				gTC90517Channel[nChannelId].layerinfo.layer_B_Interleave=8;
				CH_DI_Print(3, ("ViewOfdmParam[%d]:: Layer B Interleave : 8 \n",nChannelId));
				break;//("I = 8")
		    	case 0x000c:
				gTC90517Channel[nChannelId].layerinfo.layer_B_Interleave=16;
				CH_DI_Print(3, ("ViewOfdmParam[%d]:: Layer B Interleave : 16 \n",nChannelId));
				break;//("I = 16")
		    	case 0x0010:
				gTC90517Channel[nChannelId].layerinfo.layer_B_Interleave=32;
				CH_DI_Print(3, ("ViewOfdmParam[%d]:: Layer B Interleave : 32 \n",nChannelId));
				break;//("I = 32")
	 	    	case 0x001c: //gTC90517Channel[nChannelId].layerinfo.layer_B_Interleave=0;
	 	    		CH_DI_Print(3, ("ViewOfdmParam[%d]:: Layer B Interleave: Nothing\n",nChannelId));
		   		break;
	   	}
		break;
	   case 0x40://	4k
		switch(index4 & 0x001c)//reg[b5][4:2]
		{
			case 0x0000:
				gTC90517Channel[nChannelId].layerinfo.layer_B_Interleave=0;
				CH_DI_Print(3, ("ViewOfdmParam[%d]:: Layer B Interleave : 0 \n",nChannelId));
				break;//("I = 0")
		   	case 0x0004:
				gTC90517Channel[nChannelId].layerinfo.layer_B_Interleave=2;
				CH_DI_Print(3, ("ViewOfdmParam[%d]:: Layer B Interleave : 2 \n",nChannelId));
				break;//("I = 2")
		   	case 0x0008:
				gTC90517Channel[nChannelId].layerinfo.layer_B_Interleave=4;
				CH_DI_Print(3, ("ViewOfdmParam[%d]:: Layer B Interleave : 4 \n",nChannelId));
				break;//("I = 4")
		   	case 0x000c:
				gTC90517Channel[nChannelId].layerinfo.layer_B_Interleave=8;
				CH_DI_Print(3, ("ViewOfdmParam[%d]:: Layer B Interleave : 8 \n",nChannelId));
				break;//("I = 8")
		   	case 0x0010:
				gTC90517Channel[nChannelId].layerinfo.layer_B_Interleave=16;
				CH_DI_Print(3, ("ViewOfdmParam[%d]:: Layer B Interleave : 16 \n",nChannelId));
				break;//("I = 16")
		   	case 0x001c: //gTC90517Channel[nChannelId].layerinfo.layer_B_Interleave=0;
		   		CH_DI_Print(3, ("ViewOfdmParam[%d]:: Layer B Interleave : Nothing \n",nChannelId));
		   		break;
	   	}
		break;
	   case 0x80://	8k
		switch(index4 & 0x001c)//reg[b5][4:2]
		{
			case 0x0000:
				gTC90517Channel[nChannelId].layerinfo.layer_B_Interleave=0;
				CH_DI_Print(3, ("ViewOfdmParam[%d]:: Layer B Interleave : 0 \n",nChannelId));
				break;//("I = 0")
		   	case 0x0004:
				gTC90517Channel[nChannelId].layerinfo.layer_B_Interleave=1;
				CH_DI_Print(3, ("ViewOfdmParam[%d]:: Layer B Interleave : 1 \n",nChannelId));
				break;//("I = 1")
		   	case 0x0008:
				gTC90517Channel[nChannelId].layerinfo.layer_B_Interleave=2;
				CH_DI_Print(3, ("ViewOfdmParam[%d]:: Layer B Interleave : 2 \n",nChannelId));
				break;//("I = 2")
		   	case 0x000c:
				gTC90517Channel[nChannelId].layerinfo.layer_B_Interleave=4;
				CH_DI_Print(3, ("ViewOfdmParam[%d]:: Layer B Interleave : 4 \n",nChannelId));
				break;//("I = 4")
		   	case 0x0010:
				gTC90517Channel[nChannelId].layerinfo.layer_B_Interleave=8;
				CH_DI_Print(3, ("ViewOfdmParam[%d]:: Layer B Interleave : 8 \n",nChannelId));
				break;//("I = 8")
		   	case 0x001c: //gTC90517Channel[nChannelId].layerinfo.layer_B_Interleave=0;
		   		CH_DI_Print(3, ("ViewOfdmParam[%d]:: Layer B Interleave : Nothing\n",nChannelId));
		   		break;
		}
		break;
	   }
	/* LAYER C */
		Drv_Ch_I2C_DemodRead(nChannelId, DEMOD_TC90517_ADDRESS,   TC90517_B6, &val, 1);//0xb6//D15:8
		index1=(int)val;
		Drv_Ch_I2C_DemodRead(nChannelId, DEMOD_TC90517_ADDRESS,   TC90517_B7, &val, 1);//0xb7//D7:0
		CH_DI_Print(3, ("ViewOfdmParam[%d]:: TC90517_B7 :0x%x  Layer  *****\n",nChannelId,val));
		index2=(int)val;

		index4=((index1<<8)&0xff00)|(index2&0x00ff);
		index4=(index4 & 0x001e)>>1;
	  //number of segment//reg[b7][4:1]
		Num_seg=(unsigned char)(index4);
	  	gTC90517Channel[nChannelId].layerinfo.layer_C_segments=Num_seg;
		CH_DI_Print(3, ("ViewOfdmParam[%d]:: layer_C_segments : %d *****\n",nChannelId, Num_seg));

		//retry reading
		Drv_Ch_I2C_DemodRead(nChannelId,  DEMOD_TC90517_ADDRESS,  TC90517_B6, &val, 1);//0xb6//D15:8
		index1=(int)val;
		Drv_Ch_I2C_DemodRead(nChannelId, DEMOD_TC90517_ADDRESS,   TC90517_B7, &val, 1);//0xb7//D7:0
		index2=(int)val;
		index4=((index1<<8)&0xff00)|(index2&0x00ff);
	  // constellation
	   switch(index4 & 0x3800)//reg[b6][5:3]
	   {
		   case 0x0000:
		   	gTC90517Channel[nChannelId].layerinfo.layer_C_constellation=constellation_DQPSK;
		   	CH_DI_Print(3, ("ViewOfdmParam[%d]:: Layer C Constllation : DQPSK\n",nChannelId));
			break;
		   case 0x0800:
		   	gTC90517Channel[nChannelId].layerinfo.layer_C_constellation=constellation_QPSK;
		   	CH_DI_Print(3, ("ViewOfdmParam[%d]:: Layer C Constllation : QPSK \n",nChannelId));
			break;
		   case 0x1000:
		   	gTC90517Channel[nChannelId].layerinfo.layer_C_constellation=constellation_16QAM;
		   	CH_DI_Print(3, ("ViewOfdmParam[%d]:: Layer C Constllation : 16QAM \n",nChannelId));
			break;
		   case 0x1800:
		   	gTC90517Channel[nChannelId].layerinfo.layer_C_constellation=constellation_64QAM;
		   	CH_DI_Print(3, ("ViewOfdmParam[%d]:: Layer C Constllation : 64QAM \n",nChannelId));
			break;
		   case 0x3800:
		   	gTC90517Channel[nChannelId].layerinfo.layer_C_constellation=constellation_NOTHING;
		   	CH_DI_Print(3, ("ViewOfdmParam[%d]:: Layer C Constllation : Nothing\n",nChannelId));
			break;
	   }
	 // HP code rate(Iner code)
	   switch(index4 & 0x0700)//reg[b6][2:0]
	   {
		   case 0x0000:
		   	gTC90517Channel[nChannelId].layerinfo.layer_C_CodeRate=CodeRate_1_2;
		   	CH_DI_Print(3, ("ViewOfdmParam[%d]:: Layer C Code Rate : 1/2\n",nChannelId));
			break;
		   case 0x0100:
		   	gTC90517Channel[nChannelId].layerinfo.layer_C_CodeRate=CodeRate_2_3;
		   	CH_DI_Print(3, ("ViewOfdmParam[%d]:: Layer C Code Rate : 2/3\n",nChannelId));
			break;
		   case 0x0200:
		   	gTC90517Channel[nChannelId].layerinfo.layer_C_CodeRate=CodeRate_3_4;
		   	CH_DI_Print(3, ("ViewOfdmParam[%d]:: Layer C Code Rate : 3/4\n",nChannelId));
			break;
		   case 0x0300:
		   	gTC90517Channel[nChannelId].layerinfo.layer_C_CodeRate=CodeRate_5_6;
		   	CH_DI_Print(3, ("ViewOfdmParam[%d]:: Layer C Code Rate : 5/6\n",nChannelId));
			break;
		   case 0x0400:
		   	gTC90517Channel[nChannelId].layerinfo.layer_C_CodeRate=CodeRate_7_8;
		   	CH_DI_Print(3, ("ViewOfdmParam[%d]:: Layer C Code Rate : 7/8\n",nChannelId));
			break;
		   case 0x0700:
		   	gTC90517Channel[nChannelId].layerinfo.layer_C_CodeRate=CodeRate_NOT;
		   	CH_DI_Print(3, ("ViewOfdmParam[%d]:: Layer C Code Rate : Nothing\n",nChannelId));
			break;
	   }
	  // LP code rate(Time interleave)
	   switch(ffsize)
	   {
	   case 0x00://	2k
		switch(index4 & 0x00e0)//reg[b7][7:5]
		{
			case 0x0000:
				gTC90517Channel[nChannelId].layerinfo.layer_C_Interleave=0;
				CH_DI_Print(3, ("ViewOfdmParam[%d]:: Layer C Interleave : 0 \n",nChannelId));
				break;//("I = 0")
		    	case 0x0020:
					gTC90517Channel[nChannelId].layerinfo.layer_C_Interleave=4;
				CH_DI_Print(3, ("ViewOfdmParam[%d]:: Layer C Interleave : 4\n",nChannelId));
				 break;//("I = 4")
		    	case 0x0040:
					gTC90517Channel[nChannelId].layerinfo.layer_C_Interleave=8;
				CH_DI_Print(3, ("ViewOfdmParam[%d]:: Layer C Interleave : 8 \n",nChannelId));
				break;//("I = 8")
		    	case 0x0060:
					gTC90517Channel[nChannelId].layerinfo.layer_C_Interleave=16;
				CH_DI_Print(3, ("ViewOfdmParam[%d]:: Layer C Interleave : 16 \n",nChannelId));
				break;//("I = 16")
		    	case 0x0080:
					gTC90517Channel[nChannelId].layerinfo.layer_C_Interleave=32;
				CH_DI_Print(3, ("ViewOfdmParam[%d]:: Layer C Interleave : 32 \n",nChannelId));
				break;//("I = 32")
	 	    	case 0x00e0: //gTC90517Channel[nChannelId].layerinfo.layer_C_Interleave=0;
	 	    		CH_DI_Print(3, ("ViewOfdmParam[%d]:: Layer C Interleave : Nothing\n",nChannelId));
	   			break;
		}
		break;
	   case 0x40://	4k
		switch(index4 & 0x00e0)//reg[b7][7:5]
		{
			case 0x0000:
				gTC90517Channel[nChannelId].layerinfo.layer_C_Interleave=0;
				CH_DI_Print(3, ("ViewOfdmParam[%d]:: Layer C Interleave : 0 \n",nChannelId));
				break;//("I = 0")
		    	case 0x0020:
				gTC90517Channel[nChannelId].layerinfo.layer_C_Interleave=2;
				CH_DI_Print(3, ("ViewOfdmParam[%d]:: Layer C Interleave : 2 \n",nChannelId));
				break;//("I = 2")
		    	case 0x0040:
				gTC90517Channel[nChannelId].layerinfo.layer_C_Interleave=4;
				CH_DI_Print(3, ("ViewOfdmParam[%d]:: Layer C Interleave : 4 \n",nChannelId));
				break;//("I = 4")
		   	case 0x0060:
				gTC90517Channel[nChannelId].layerinfo.layer_C_Interleave=8;
				CH_DI_Print(3, ("ViewOfdmParam[%d]:: Layer C Interleave : 8 \n",nChannelId));
				 break;//("I = 8")
		    	case 0x0080:
				gTC90517Channel[nChannelId].layerinfo.layer_C_Interleave=16;
				CH_DI_Print(3, ("ViewOfdmParam[%d]:: Layer C Interleave : 16 \n",nChannelId));
				break;//("I = 16")
	 	    	case 0x00e0: //gTC90517Channel[nChannelId].layerinfo.layer_C_Interleave=0;
	 	    		CH_DI_Print(3, ("ViewOfdmParam[%d]:: Layer C Interleave : Nothing\n",nChannelId));
	   			break;
		}
		break;
	   case 0x80://	8k
		switch(index4 & 0x00e0)//reg[b7][7:5]
		{
			case 0x0000:
				gTC90517Channel[nChannelId].layerinfo.layer_C_Interleave=0;
				CH_DI_Print(3, ("ViewOfdmParam[%d]:: Layer C Interleave : 0 \n",nChannelId));
				break;//("I = 0")
		    	case 0x0020:
					gTC90517Channel[nChannelId].layerinfo.layer_C_Interleave=1;
				CH_DI_Print(3, ("ViewOfdmParam[%d]:: Layer C Interleave : 1 \n",nChannelId));
				break;//("I = 1")
		    	case 0x0040:
					gTC90517Channel[nChannelId].layerinfo.layer_C_Interleave=2;
				CH_DI_Print(3, ("ViewOfdmParam[%d]:: Layer C Interleave : 2\n",nChannelId));
				break;//("I = 2")
		    	case 0x0060:
					gTC90517Channel[nChannelId].layerinfo.layer_C_Interleave=4;
				CH_DI_Print(3, ("ViewOfdmParam[%d]:: Layer C Interleave : 4 \n",nChannelId));
				break;//("I = 4")
		    	case 0x0080:
					gTC90517Channel[nChannelId].layerinfo.layer_C_Interleave=8;
				CH_DI_Print(3, ("ViewOfdmParam[%d]:: Layer C Interleave : 8 \n",nChannelId));
				break;//("I = 8")
	 	    	case 0x00e0: //gTC90517Channel[nChannelId].layerinfo.layer_C_Interleave=0;
	 	    		CH_DI_Print(3, ("ViewOfdmParam[%d]:: Layer C Interleave : Nothing\n",nChannelId));
	   			break;
		}
		break;
	   }

}


void ViewDemodStatus(int  nChannelId)
{

	HUINT8 isdbt=0;

	/* Get the COFDM status : '0'(normal), '1'(error) */

	Drv_Ch_I2C_DemodRead(nChannelId, DEMOD_TC90517_ADDRESS,   TC90517_80, &isdbt,1);
	CH_DI_Print (4, ("ViewDemodStatus[%d]:: SYNC=0x%x\n", nChannelId, isdbt&0xff));
	//isdbt=val;
	if((isdbt & 0x80)== 0x80)
	{
		gTC90517Status[nChannelId].Isdb_Sync=FALSE;
		CH_DI_Print(3, ("ViewDemodStatus[%d]:: Synchronization[7] : ERROR\n",nChannelId));
	}//D[7]
	else
	{
		gTC90517Status[nChannelId].Isdb_Sync=TRUE;
		CH_DI_Print(3, ("ViewDemodStatus[%d]:: Synchronization[7] : OK\n",nChannelId));
	}

	if((isdbt & 0x40)== 0x40)
	{
		gTC90517Status[nChannelId].Isdb_Level=FALSE;	// rf signal level error
		CH_DI_Print(3, ("ViewDemodStatus[%d]:: Receive Level[6] : ERROR\n",nChannelId));

	}//D[6]
	else
	{
		gTC90517Status[nChannelId].Isdb_Level=TRUE;
		CH_DI_Print(3, ("ViewDemodStatus[%d]:: Receive Level[6] : OK\n",nChannelId));
	}

	if((isdbt & 0x20)==0x20)
	{
		gTC90517Status[nChannelId].Isdb_Tmcc=FALSE;       // TMCC is not detected
		CH_DI_Print(3, ("ViewDemodStatus[%d]:: TMCC[5] : ERROR\n",nChannelId));
	}//D[5]
	else
	{
		gTC90517Status[nChannelId].Isdb_Tmcc=TRUE;
		CH_DI_Print(3, ("ViewDemodStatus[%d]:: TMCC[5] : OK\n",nChannelId));
	}


	if((isdbt & 0x10)==0x10)
	{
		gTC90517Status[nChannelId].Isdb_Recvmd=FALSE;
		CH_DI_Print(3, ("ViewDemodStatus[%d]:: Receive Mode[4] : ERROR\n",nChannelId));
	}//D[4]//recvmd(received segment mode), partmd(NOT use)
	else
	{
		gTC90517Status[nChannelId].Isdb_Recvmd=TRUE;
		CH_DI_Print(3, ("ViewDemodStatus[%d]:: Receive Mode[4] : OK\n",nChannelId));
	}

	if((isdbt & 0x08)==0x08)
	{
		gTC90517Status[nChannelId].Isdb_Frame=FALSE;
		CH_DI_Print(3, ("ViewDemodStatus[%d]:: FRAME[3] : ERROR\n",nChannelId));
	}//D[3]
	else
	{
		gTC90517Status[nChannelId].Isdb_Frame=TRUE;
		CH_DI_Print(3, ("ViewDemodStatus[%d]:: FRAME[3] : OK\n",nChannelId));
	}


	if((isdbt & 0x04)==0x04)
	{
		gTC90517Status[nChannelId].Isdb_Viterbi=FALSE; // vitervi decoding error
		CH_DI_Print(3, ("ViewDemodStatus[%d]:: VITERBI[2] : ERROR\n",nChannelId));
	}//D[2]
	else
	{
		gTC90517Status[nChannelId].Isdb_Viterbi=TRUE;
		CH_DI_Print(3, ("ViewDemodStatus[%d]:: VITERBI[2] : OK\n",nChannelId));
	}

	if((isdbt & 0x02)==0x02)
	{
		gTC90517Status[nChannelId].Isdb_Rs=FALSE;	// occured rs error
		CH_DI_Print(3, ("ViewDemodStatus[%d]:: RS[1] : ERROR\n",nChannelId));
	}//D[1]
	else
	{
		gTC90517Status[nChannelId].Isdb_Rs=TRUE;
		CH_DI_Print(3, ("ViewDemodStatus[%d]:: RS[1] : OK \n",nChannelId));
	}

	if((isdbt & 0x01)==0x01)
	{
		gTC90517Status[nChannelId].Isdb_Pkt=FALSE;  // occured uncorrected error
		CH_DI_Print(3, ("ViewDemodStatus[%d]:: TS Packet[0] : ERROR*\n",nChannelId));
	}//D[0]
	else
	{
		gTC90517Status[nChannelId].Isdb_Pkt=TRUE;
		CH_DI_Print(3, ("ViewDemodStatus[%d]:: TS Packet[0] : OK\n",nChannelId));
	}
}

HUINT8 ViewFrameLock(int  nChannelId)
{
	HUINT8	val;

	Drv_Ch_I2C_DemodRead(nChannelId, DEMOD_TC90517_ADDRESS,  TC90517_80, &val, 1);
	CH_DI_Print (4, ("ViewFrameLock[%d]:: Frame Lock = %x\n", nChannelId, val));

	if ( (val & 0x08)==0x08 )
	{
		CH_DI_Print (4, ("ViewFrameLock[%d]:: Frame Lock Fail\n",nChannelId));
		return FALSE;// unlock
	}
	else
	{
		CH_DI_Print (4, ("ViewFrameLock[%d]:: Frame Lock OK\n",nChannelId));
		return TRUE;// lock
	}
}

HUINT8 ViewTmccDetect(int nChannelId)
{
	HUINT8	val;

	Drv_Ch_I2C_DemodRead(nChannelId, DEMOD_TC90517_ADDRESS,  TC90517_80, &val, 1);
	CH_DI_Print (4, ("ViewTmccDetect[%d]:: TMCC Detection = %x\n", nChannelId, val));

	if ( (val & 0x20)==0x20 )
	{
		CH_DI_Print (4, ("ViewTmccDetect[%d]:: TMCC Lock Fail\n",nChannelId));
		return FALSE;// unlock
	}
	else
	{
		CH_DI_Print (4, ("ViewTmccDetect[%d]:: TMCC Lock OK\n",nChannelId));
		return TRUE;// lock
	}
}

void ChangeHkfrq(HUINT8 inval)
{
	HKFRQ = 1;				// register 14h
	ghkfrq = inval;
	CH_DI_Print(4, ("Change value\n"));
}

void ChangeAfcgain(HUINT8 inval)
{
	AFC30 = 1;				// register 30
	gafc = inval;
	CH_DI_Print(4, ("Change value\n"));
}

void ChangeAffrq(HUINT8 inval)
{
	AFC39 = 1;
	gafc39 = inval;
	CH_DI_Print(4, ("Change value\n"));
}

void ChangeCpld(HUINT8 inval)
{
	AFC32 = 1;
	gafc32 = inval;
	CH_DI_Print(4, ("Change value\n"));
}
void IsdbTop(HUINT8 isdb_top, HUINT8 agc_m )
{
	gucTERTopOn = agc_m;
	gucIfMinT = isdb_top;
}

void  IsdbIfset(HUINT8 if_max, HUINT8 if_min, HUINT8 agc_m)
{
	gucTERTopOn = agc_m;
	gucIfMinT = if_min;
	gucIfMaxT = if_max;
}

void IsdbRfset(HUINT8 rf_max, HUINT8 rf_min, HUINT8 agc_m)
{
	gucTERTopOn = agc_m;
	gucRfMaxT = rf_max;
	gucRfMinT = rf_min;
}

void TC90517_Rd(int nChannelId)
{
	int i, count=0;
	HUINT8	val;

	HUINT8	TC90517[0xff] =
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

	CH_DI_Print (0, ("TC90517_Rd[%d]\n",nChannelId));

	for(i=0; i<16; i++)
	{
		CH_DI_Print(0, ("[%2x],", i));
	}

	CH_DI_Print(0, ("\n"));
	CH_DI_Print(0, ("------------------------------------------------------------------\n"));


	for(i=0; i<0xFE; i++)
	{
		if(TC90517[i] !=	0x99)
		{
			Drv_Ch_I2C_DemodRead(nChannelId, DEMOD_TC90517_ADDRESS,  i, &val, 1);
			CH_DI_Print(0, ("[%2x],",  val));
		}
		else
		{
			CH_DI_Print(0, ("[%2x],",TC90517[i]));
		}

		count++;

		if(count > 15)
		{
			count = 0;
			CH_DI_Print(0, ("\n"));
		}
	}
}

#if (0)
void TC90517_RdAll(HUINT32 nChannelId)
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

	CH_DI_Print ("TC90517_Rd()\n");

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

	Drv_Ch_I2C_DemodRead(nChannelId, DEMOD_TC90517_ADDRESS, TC90517_81, &val, 1);
	val = (val & 0x10) >> 4;
	CH_DI_Print (3, ("TC90517_CheckStatus[%d]:: Sleep Mode = %x\n", nChannelId, val));
}

/* onOff 0 : Normal, 1 : ADC power down
static void Set_ADC(int nChannelId, HUINT32 onOff)
{
	HUINT8 val = 0;

	val = (onOff << 5);

	Drv_Ch_I2C_DemodWrite(nChannelId,  TC90517_04, &val, 1);
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
#if !defined(__CH_DEBUG__) || !defined(__CH_RELEASE_MODE_DEBUG__)
	CH_UNUSED(nChannelId);
#endif

	CH_DI_Print(4, ("[View_Accu[%d]]Total elapsed Time(sec) : %d\n",nChannelId, accuCnt[nChannelId]*4));	/* 4초마다 1씩 증가하므로 4를 곱하면 전체 경과 시간이 나온다. */
	CH_DI_Print(4, ("[View_Accu[%d]]Layer A Accumulated UnCorrected Packet : %d\n",nChannelId, accuUnCorPkt[nChannelId][0]));
	CH_DI_Print(4, ("[View_Accu[%d]]Layer B Accumulated UnCorrected Packet : %d\n",nChannelId, accuUnCorPkt[nChannelId][1]));
	CH_DI_Print(4, ("[View_Accu[%d]]Layer C Accumulated UnCorrected Packet : %d\n",nChannelId, accuUnCorPkt[nChannelId][2]));
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
	gTC90517Channel[nChannelId].emergencyFlag = flag;
}


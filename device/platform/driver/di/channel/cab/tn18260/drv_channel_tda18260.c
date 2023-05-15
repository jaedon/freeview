/* $Header: */
/*******************************************************************/
/************************* File Description ************************/
/*******************************************************************/
// File Name:		drv_channel_tn18273.c
// Original Author: J.D.CHO
// File Description:
// Module:
// Remarks:

/*******************************************************************/
/* Copyright (c) 2006 HUMAX Co., Ltd. 							   */
/* All rights reserved.											   */
/*******************************************************************/


/*******************************************************************/
/**************************** Header Files *************************/
/*******************************************************************/
// Start Including Header Files
#if defined(CONFIG_VK_STDLIB)
#else
#include 	<stdio.h>
#endif
#include <stdarg.h>

#include "tmNxTypes.h"
#include "tmCompId.h"
#include "tmFrontEnd.h"
#include "tmbslFrontEndTypes.h"
#include "tmbslTDA18260.h"
#include "tmUnitParams.h"


#include "htype.h"
#include "di_channel.h"
#include "di_channel_priv.h"
#include "drv_i2c.h"

// End Including Headers


/*******************************************************************/
/****************************** define *****************************/
/*******************************************************************/
// Start #define
//#define CONST_CHANNEL_I2C_BUS	(2)
//#define CONST_TDA18260_I2CADDR	(0xC0>>1)

#define TDA18260_I2CADDR	(0xC0>>1)

#define PRINT_BUFFER_SIZE (1040)

#define NUM_OF_REG			0x44
//#define DEBUG_TUNER_I2C
//#define DEBUG_TUNER_I2C
// End #define

#define GET_STREAM_TYPE(_tUnIt) GET_INDEX_TUNIT(_tUnIt) & 0x01

/*******************************************************************/
/****************************** typedef ****************************/
/*******************************************************************/
// Start typedef

// End typedef


/*******************************************************************/
/************************ global variables *************************/
/*******************************************************************/
// Start global variablee
HFLOAT64	g_fRfAgcTable_PldDacScale0_ES2[512]= {-9.284889221,-9.218482971,-9.196479797,-9.16053009,-9.056015015, \
	-9.034225464,-8.947891235,-8.852615356,-8.802047729,-8.712295532,-8.637908936,-8.601203918,-8.482315063, \
	-8.402374268,-8.388061523,-8.299186707,-8.200286865,-8.090675354,-7.874702454,-7.642082214,-7.559799194, \
	-7.342987061,-7.12991333,-7.024932861,-6.824813843,-6.639259338,-6.538642883,-6.348976135,-6.160423279, \
	-6.067253113,-5.893165588,-5.6927948,-5.615898132,-5.450675964,-5.257514954,-5.172348022,-4.997108459, \
	-4.795684814,-4.687637329,-4.459671021,-4.336669922,-3.97052002,-3.52779007,-3.291599274,-2.740970612, \
	-2.154891968,-1.847026825,-1.247020721,-0.940174103,-0.124290466,0.199092865,0.768997192,1.35634613, \
	1.63702774,2.161823273,2.681015015,2.932991028,3.425956726,3.87153244,4.0871315,4.532802582,4.954269409, \
	5.171031952,5.551750183,5.941642761,6.111209869,6.490184784,6.677268982,6.992370605,7.333889008,7.657276154, \
	7.81905365,8.136413574,8.283454895,8.5859375,8.880020142,9.02526474,9.316711426,9.438346863,9.713115692, \
	9.982479095,10.12246323,10.37679672,10.61443329,10.7405777,10.98389816,11.1071434,11.34189606,11.55555725, \
	11.7796936,11.89640045,12.11533356,12.21854401,12.42592239,12.63495636,12.73784637,12.94072723,13.12787628, \
	13.22608948,13.42450333,13.6119957,13.72929001,13.89394379,14.07167053,14.16572952,14.3388176,14.51385117, \
	14.66238022,14.82260895,14.90898895,15.07045364,15.22986603,15.30729675,15.46416473,15.61693192,15.69163895, \
	15.83625412,15.98129272,16.05923462,16.1916275,16.33573914,16.40139389,16.5322876,16.66906357,16.73678207, \
	16.86919022,16.98011017,17.1073494,17.1713829,17.29354477,17.41255188,17.53285217,17.5905304,17.70775223, \
	17.82307434,17.87327576,17.98230362,18.09370041,18.1480751,18.25603867,18.35641479,18.41599274,18.51314163, \
	18.6143074,18.66594696,18.78869629,18.83607101,18.93168259,18.97946167,19.07200623,19.15924454,19.24488068, \
	19.28815842,19.37853622,19.45614243,19.50020599,19.57997131,19.6615181,19.7016449,19.77576828,19.85403824, \
	19.89453888,19.96211624,20.02404785,20.09502792,20.16620255,20.1999054,20.26837921,20.32813644,20.39097595, \
	20.42510605,20.48721695,20.54817963,20.57509613,20.63460159,20.66182327,20.72128677,20.77190781,20.82831955, \
	20.85147858,20.90515518,20.95666504,21.00086975,21.02257919,21.07110596,21.11467361,21.15988922,21.17940903, \
	21.22782516,21.26656342,21.28846741,21.32760239,21.36709976,21.38526917,21.42806244,21.46038818,21.48065948, \
	21.51657867,21.55144882,21.56644058,21.59794998,21.62413788,21.65419388,21.68862915,21.70133209,21.73025894, \
	21.76054001,21.77529526,21.79944611,21.82849503,21.83871078,21.86303711,21.88910294,21.90039063,21.9250412, \
	21.94828415,21.96078491,21.98314285,22.00364685,22.01671219,22.04227448,22.0631218,22.07213974,22.09545135, \
	22.11400223,22.12631607,22.14083862,22.15642929,22.17470932,22.18447876,22.2005043,22.21487427,22.22526169, \
	22.23741531,22.2510643,22.25862885,22.27558517,22.28910065,22.2982254,22.30657196,22.31852341,22.33411026, \
	22.33848953,22.34983063,22.36303329,22.36585617,22.37933731,22.39006042,22.39466095,22.40538406,22.41231155, \
	22.42188644,22.42665482,22.4404068,22.44319916,22.45174408,22.45935059,22.46831894,22.47743988,22.48331833, \
	22.4916153,22.5006485,22.50366211,22.51078033,22.52155304,22.52415848,22.53095245,22.53822708,22.53956604, \
	22.54814529,22.55190659,22.55807877,22.56103516,22.56758499,22.57368469,22.57820129,22.58522797,22.5881424, \
	22.59205246,22.60029602,22.60068893,22.60726929,22.60995865,22.611866,22.61889267,22.61989594,22.62239075,22.62522507, \
	22.63350677,22.63333511,22.63650513,22.63941574,22.63941574,22.63941574,22.63941574,22.63941574,22.63941574, \
	22.63941574,22.63941574,22.63941574,22.63941574,22.63941574,22.63941574,22.63941574,22.63941574,22.63941574, \
	22.63941574,22.63941574,22.63941574,22.63941574,22.63941574,22.63941574,22.63941574,22.63941574,22.63941574, \
	22.63941574,22.63941574,22.63941574,22.63941574,22.63941574,22.63941574,22.63941574,22.63941574,22.63941574, \
	22.63941574,22.63941574,22.63941574,22.63941574,22.63941574,22.63941574,22.63941574,22.63941574,22.63941574, \
	22.63941574,22.63941574,22.63941574,22.63941574,22.63941574,22.63941574,22.63941574,22.63941574,22.63941574, \
	22.63941574,22.63941574,22.63941574,22.63941574,22.63941574,22.63941574,22.63941574,22.63941574,22.63941574, \
	22.63941574,22.63941574,22.63941574,22.63941574,22.63941574,22.63941574,22.63941574,22.63941574,22.63941574, \
	22.63941574,22.63941574,22.63941574,22.63941574,22.63941574,22.63941574,22.63941574,22.63941574,22.63941574, \
	22.63941574,22.63941574,22.63941574,22.63941574,22.63941574,22.63941574,22.63941574,22.63941574,22.63941574, \
	22.63941574,22.63941574,22.63941574,22.63941574,22.63941574,22.63941574,22.63941574,22.63941574,22.63941574, \
	22.63941574,22.63941574,22.63941574,22.63941574,22.63941574,22.63941574,22.63941574,22.63941574,22.63941574, \
	22.63941574,22.63941574,22.63941574,22.63941574,22.63941574,22.63941574,22.63941574,22.63941574,22.63941574, \
	22.63941574,22.63941574,22.63941574,22.63941574,22.63941574,22.63941574,22.63941574,22.63941574,22.63941574, \
	22.63941574,22.63941574,22.63941574,22.63941574,22.63941574,22.63941574,22.63941574,22.63941574,22.63941574, \
	22.63941574,22.63941574,22.63941574,22.63941574,22.63941574,22.63941574,22.63941574,22.63941574,22.63941574, \
	22.63941574,22.63941574,22.63941574,22.63941574,22.63941574,22.63941574,22.63941574,22.63941574,22.63941574, \
	22.63941574,22.63941574,22.63941574,22.63941574,22.63941574,22.63941574,22.63941574,22.63941574,22.63941574, \
	22.63941574,22.63941574,22.63941574,22.63941574,22.63941574,22.63941574,22.63941574,22.63941574,22.63941574, \
	22.63941574,22.63941574,22.63941574,22.63941574,22.63941574,22.63941574,22.63941574,22.63941574,22.63941574,\
	22.63941574,22.63941574,22.63941574,22.63941574,22.63941574,22.63941574,22.63941574,22.63941574,22.63941574, \
	22.63941574,22.63941574,22.63941574,22.63941574,22.63941574,22.63941574,22.63941574,22.63941574,22.63941574,\
	22.63941574,22.63941574,22.63941574,22.63941574,22.63941574,22.63941574,22.63941574,22.63941574,22.63941574, \
	22.63941574,22.63941574,22.63941574,22.63941574,22.63941574,22.63941574,22.63941574,22.63941574,22.63941574, \
	22.63941574,22.63941574,22.63941574,22.63941574,22.63941574,22.63941574,22.63941574,22.63941574};

HFLOAT64	g_fRfAgcTable_PldDacScale1_ES2[512]= {-9.457366943,-9.405754089,-9.371459961,-9.330497742,-9.323669434, \
	-9.218482971,-9.196479797,-9.107681274,-9.056015015,-8.981521606,-8.947891235,-8.912254333,-8.802047729, \
	-8.784339905,-8.702217102,-8.637908936,-8.54889679,-8.482315063,-8.402374268,-8.388061523,-8.325065613,-8.266555786, \
	-8.200286865,-7.988372803,-7.874702454,-7.642082214,-7.559799194,-7.342987061,-7.238555908,-7.12991333,-6.933296204, \
	-6.824813843,-6.639259338,-6.538642883,-6.348976135,-6.259635925,-6.067253113,-5.991004944,-5.789215088,-5.6927948,-5.615898132,-5.450675964, \
	-5.357673645,-5.172348022,-5.085021973,-4.922477722,-4.795684814,-4.687637329,-4.459671021,-4.336669922, \
	-3.97052002,-3.754058838,-3.291599274,-3.001045227,-2.740970612,-2.154891968,-1.847026825,-1.247020721, \
	-0.940174103,-0.399913788,0.199092865,0.481479645,0.768997192,1.35634613,1.63702774,2.161823273,2.427318573, \
	2.932991028,3.180923462,3.651008606,3.87153244,4.0871315,4.532802582,4.745109558,5.171031952,5.358081818, \
	5.740325928,5.941642761,6.111209869,6.490184784,6.677268982,6.992370605,7.159755707,7.496547699,7.657276154, \
	7.81905365,8.136413574,8.283454895,8.5859375,8.732883453,9.02526474,9.172794342,9.316711426,9.578384399, \
	9.713115692,9.850296021,10.12246323,10.24838257,10.49300385,10.61443329,10.86607361,10.98389816,11.22159195, \
	11.34189606,11.55555725,11.66732788,11.7796936,11.99822998,12.11533356,12.32095718,12.42592239,12.63495636, \
	12.73784637,12.83467484,13.04413223,13.12787628,13.32408142,13.42450333,13.6119957,13.72929001,13.81179047, \
	13.98210144,14.07167053,14.25160217,14.3388176,14.51385117,14.66238022,14.74333572,14.90898895,14.99174118, \
	15.15082169,15.22986603,15.38368988,15.46416473,15.61693192,15.69163895,15.76357269,15.91938019,15.98129272, \
	16.12339401,16.1916275,16.33573914,16.40139389,16.5322876,16.60272598,16.66906357,16.8009758,16.86919022, \
	16.98011017,17.1073494,17.1713829,17.29354477,17.35420227,17.47196579,17.53285217,17.5905304,17.70775223, \
	17.7644043,17.87327576,17.9310112,18.03879929,18.09370041,18.20084381,18.25603867,18.35641479,18.41599274, \
	18.51314163,18.56377792,18.6143074,18.69526672,18.78869629,18.83607101,18.93168259,18.97946167,19.07200623, \
	19.11646652,19.20614243,19.24488068,19.28815842,19.37853622,19.41309357,19.50020599,19.53832245,19.62316895,19.6615181, \
	19.73870468,19.77576828,19.85403824,19.89453888,19.92565918,19.99805069,20.06003571,20.12847519,20.16620255, \
	20.23265457,20.26837921,20.32813644,20.36399841,20.42510605,20.45795059,20.48721695,20.54817963,20.57509613, \
	20.63460159,20.66182327,20.72128677,20.74667358,20.80331039,20.82831955,20.85147858,20.90515518,20.93184662, \
	20.97711182,21.02257919,21.04794312,21.09230804,21.11467361,21.15988922,21.17940903,21.22782516,21.24607086, \
	21.28846741,21.30713654,21.32760239,21.36709976,21.38526917,21.42806244,21.44392395,21.48065948,21.49797821, \
	21.53347778,21.55144882,21.58146286,21.59794998,21.62413788,21.64015961,21.6717186,21.68862915,21.71474075, \
	21.73025894,21.76054001,21.77529526,21.79944611,21.81304169,21.83871078,21.85253525,21.87520981,21.88910294, \
	21.90039063,21.9250412,21.93532181,21.96078491,21.97346878,21.99204254,22.00364685,22.02363205,22.04227448, \
	22.05358887,22.07213974,22.08488464,22.10406113,22.11400223,22.13122177,22.14083862,22.15642929,22.16493607, \
	22.18447876,22.18853378,22.20718002,22.21487427,22.22526169,22.23741531,22.24607086,22.25862885,22.26612473, \
	22.27975464,22.28910065,22.2982254,22.30657196,22.31852341,22.32511902,22.33848953,22.34455872,22.35422134, \
	22.36303329,22.37545395,22.37933731,22.39006042,22.39466095,22.40221024,22.41056061,22.42188644,22.42704391,22.43163681, \
	22.4404068,22.44319916,22.45174408,22.45650482,22.4640007,22.46831894,22.47743988,22.48331833,22.4916153, \
	22.49610901,22.50366211,22.5064621,22.51734543,22.52155304,22.52799606,22.53095245,22.53822708,22.53956604, \
	22.54814529,22.54833221,22.55190659,22.55807877,22.56103516,22.56758499,22.56965256,22.57820129,22.58362961, \
	22.58522797,22.59251022,22.59205246,22.60029602,22.60068893,22.60726929,22.61056519,22.60995865,22.61655426, \
	22.61889267,22.61989594,22.62239075,22.62522507,22.62708282,22.63333511,22.63799286,22.63786697,22.63941574,22.63941574, \
	22.63941574,22.63941574,22.63941574,22.63941574,22.63941574,22.63941574,22.63941574,22.63941574,22.63941574,22.63941574, \
	22.63941574,22.63941574,22.63941574,22.63941574,22.63941574,22.63941574,22.63941574,22.63941574,22.63941574,22.63941574, \
	22.63941574,22.63941574,22.63941574,22.63941574,22.63941574,22.63941574,22.63941574,22.63941574,22.63941574,22.63941574, \
	22.63941574,22.63941574,22.63941574,22.63941574,22.63941574,22.63941574,22.63941574,22.63941574,22.63941574,22.63941574, \
	22.63941574,22.63941574,22.63941574,22.63941574,22.63941574,22.63941574,22.63941574,22.63941574,22.63941574,22.63941574, \
	22.63941574,22.63941574,22.63941574,22.63941574,22.63941574,22.63941574,22.63941574,22.63941574,22.63941574,22.63941574, \
	22.63941574,22.63941574,22.63941574,22.63941574,22.63941574,22.63941574,22.63941574,22.63941574,22.63941574,22.63941574, \
	22.63941574,22.63941574,22.63941574,22.63941574,22.63941574,22.63941574,22.63941574,22.63941574,22.63941574,22.63941574, \
	22.63941574,22.63941574,22.63941574,22.63941574,22.63941574,22.63941574,22.63941574,22.63941574,22.63941574,22.63941574, \
	22.63941574,22.63941574,22.63941574,22.63941574,22.63941574,22.63941574,22.63941574,22.63941574,22.63941574,22.63941574, \
	22.63941574,22.63941574,22.63941574,22.63941574,22.63941574,22.63941574,22.63941574,22.63941574,22.63941574,22.63941574, \
	22.63941574,22.63941574,22.63941574,22.63941574,22.63941574,22.63941574,22.63941574,22.63941574,22.63941574,22.63941574, \
	22.63941574,22.63941574,22.63941574,22.63941574,22.63941574,22.63941574,22.63941574,22.63941574,22.63941574,22.63941574, \
	22.63941574,22.63941574,22.63941574,22.63941574,22.63941574,22.63941574,22.63941574,22.63941574,22.63941574,22.63941574, \
	22.63941574,22.63941574,22.63941574,22.63941574,22.63941574,22.63941574,22.63941574,22.63941574,22.63941574,22.63941574, \
	22.63941574,22.63941574,22.63941574,22.63941574,22.63941574,22.63941574,22.63941574,22.63941574,22.63941574,22.63941574, \
	22.63941574,22.63941574,22.63941574,22.63941574,22.63941574,22.63941574,22.63941574,22.63941574,22.63941574,22.63941574, \
	22.63941574,22.63941574};

extern HINT32 show_di_ch;
// End global variable


/*******************************************************************/
/************************ static variables *************************/
/*******************************************************************/
// Start static variablee
static HUINT32 s_TunerIfFreq[2] = {0,0};
// End static variable


/****************** Function 시작점에 필히 기록 ********************/
/*******************************************************************/
/*********************** Function Description***********************/
/*******************************************************************/
// Function Name:
// Function Description:
// Parameter:
// Return Value:
static void TDA18260_SetAgcCtrl(int nUnitId);


//*--------------------------------------------------------------------------------------
//* Function Name       : DRV_TUNER_I2CRead
//* Object              :
//* Input Parameters    : 	tmUnitSelect_t tUnit
//* 						UInt32 AddrSize,
//* 						UInt8* pAddr,
//* 						UInt32 ReadLen,
//* 						UInt8* pData
//* Output Parameters   : tmErrorCode_t.
//*--------------------------------------------------------------------------------------
tmErrorCode_t DRV_TUNER_I2CRead(tmUnitSelect_t tUnit,	UInt32 AddrSize, UInt8* pAddr,
UInt32 ReadLen, UInt8* pData)
{
	/* Variable declarations */
	tmErrorCode_t err = TM_OK;
	int nRet = 0;
	HUINT8 ucSubAddr = 0;
	HUINT8	ucDevAddr = TDA18260_I2CADDR;
	HUINT8	ucI2CBus = I2C_CHANNEL_CHANNEL;
#if defined(DEBUG_TUNER_I2C)
	int i;
#endif

	CH_UNUSED(tUnit);

	if( pAddr == NULL )
	{
		return DI_CH_PARAM_ERR;
	}
	if( pData == NULL )
	{
		return DI_CH_PARAM_ERR;
	}

	if( AddrSize >= 2 || AddrSize == 0 )
	{
		CH_DI_Print(0, (">>> Error : DRV_TUNER_I2CRead()... AddrSize[%d] \n", AddrSize));
		return DI_CH_PARAM_ERR;
	}

#if 0//defined(DEBUG_TUNER_I2C)
		CH_DI_Print(0, ("\n<<< Read : Sub-address, AddrSize[%d]\n", AddrSize));
		for(i=0; i<AddrSize; i++)
		{
			CH_DI_Print(0, ("[%d]=[0x%x]\n", i, pAddr[i]));
		}
		CH_DI_Print(0, ("<<< Read : Data, ReadLen[%d]\n", ReadLen));
		for(i=0; i<ReadLen; i++)
		{
			CH_DI_Print(0, ("[%d]=[0x%x]\n", i, pData[i]));
		}
#endif

	ucDevAddr = TDA18260_I2CADDR;
	ucI2CBus = I2C_CHANNEL_CHANNEL;

	ucSubAddr = pAddr[0];

	//CH_DI_Print(0, ("DRV_TUNER_I2CRead()... ucI2CBus(%d) ucDevAddr(%x) ucSubAddr(%x)\n", ucI2CBus, ucDevAddr, ucSubAddr));
	nRet = DRV_I2c_ReadA8( ucI2CBus, ucDevAddr, ucSubAddr, pData, ReadLen );
	if ( nRet != 0 )
	{
		CH_DI_Print(0, (">>> Error : DRV_I2c_ReadA16()... in DRV_TUNER_I2CRead()..ucI2CBus(%d) ucDevAddr(%x) ucSubAddr(%x)\n", ucI2CBus, ucDevAddr, ucSubAddr));
		return TM_ERR_READ;
	}
	else
	{
#if defined(DEBUG_TUNER_I2C)
		CH_DI_Print(0, ("\n<<<<<<< Read : Sub-address, AddrSize[%d]\n", AddrSize));
		for(i=0; i<AddrSize; i++)
		{
			CH_DI_Print(0, ("[%d]=[0x%x]\n", i, pAddr[i]));
		}
		CH_DI_Print(0, ("<<<<<<< Read : Data, ReadLen[%d]\n", ReadLen));
		for(i=0; i<ReadLen; i++)
		{
			CH_DI_Print(0, ("[%d]=[0x%x]\n", i, pData[i]));
		}
#endif
	}
	return err;
}

//*--------------------------------------------------------------------------------------
//* Function Name       : DRV_TUNER_I2CWrite
//* Object              :
//* Input Parameters    : 	tmUnitSelect_t tUnit
//* 						UInt32 AddrSize,
//* 						UInt8* pAddr,
//* 						UInt32 WriteLen,
//* 						UInt8* pData
//* Output Parameters   : tmErrorCode_t.
//*--------------------------------------------------------------------------------------
tmErrorCode_t DRV_TUNER_I2CWrite (tmUnitSelect_t tUnit, 	UInt32 AddrSize, UInt8* pAddr,
UInt32 WriteLen, UInt8* pData)
{
	/* Variable declarations */
	tmErrorCode_t err = TM_OK;
	int nRet = 0;
	HUINT8 ucSubAddr = 0;
	HUINT8	ucDevAddr = TDA18260_I2CADDR;
	HUINT8	ucI2CBus = I2C_CHANNEL_CHANNEL;
#if defined(DEBUG_TUNER_I2C)
	int i;
#endif
	CH_UNUSED(tUnit);

	if( pAddr == NULL )
	{
		return DI_CH_PARAM_ERR;
	}
	if( pData == NULL )
	{
		return DI_CH_PARAM_ERR;
	}

	if( AddrSize >= 2 || AddrSize == 0 )
	{
		CH_DI_Print(0, (">>> Error : DRV_TUNER_I2CRead()... AddrSize[%d] \n", AddrSize));
		return DI_CH_PARAM_ERR;
	}
#if 0//defined(DEBUG_TUNER_I2C)
		CH_DI_Print(0, ("\n>>> Write : Sub-address, AddrSize[%d]\n", AddrSize));
		for(i=0; i<AddrSize; i++)
		{

			CH_DI_Print(0, ("[%d]=[0x%x]\n", i, pAddr[i]));
		}
		CH_DI_Print(0, (">>> Write : Data, WriteLen[%d]\n", WriteLen));
		for(i=0; i<WriteLen; i++)
		{

			CH_DI_Print(0, ("[%d]=[0x%x]\n", i, pData[i]));
		}
#endif
	ucSubAddr = pAddr[0];

	ucDevAddr = TDA18260_I2CADDR;
	ucI2CBus = I2C_CHANNEL_CHANNEL;


	//CH_DI_Print(0, ("DRV_TUNER_I2CWrite()... ucI2CBus(%d) ucDevAddr(%x) ucSubAddr(%x)\n", ucI2CBus, ucDevAddr, ucSubAddr));
	nRet = DRV_I2c_WriteA8(ucI2CBus, ucDevAddr, ucSubAddr, pData, WriteLen);
	if ( nRet != 0 )
	{
		CH_DI_Print(0, (">>> Error : DRV_I2c_ReadA16()... in DRV_TUNER_I2CWrite()..ucI2CBus(%d) ucDevAddr(%x) ucSubAddr(%x)\n", ucI2CBus, ucDevAddr, ucSubAddr));
	    return TM_ERR_WRITE;
	}
	else
	{
#if defined(DEBUG_TUNER_I2C)
		CH_DI_Print(0, ("\n>>>>>>> Write : Sub-address, AddrSize[%d]\n", AddrSize));
		for(i=0; i<AddrSize; i++)
		{

			CH_DI_Print(0, ("[%d]=[0x%x]\n", i, pAddr[i]));
		}
		CH_DI_Print(0, (">>>>>>> Write : Data, WriteLen[%d]\n", WriteLen));
		for(i=0; i<WriteLen; i++)
		{

			CH_DI_Print(0, ("[%d]=[0x%x]\n", i, pData[i]));
		}
#endif
	}
   return err;
}

//*--------------------------------------------------------------------------------------
//* Function Name       : DRV_TUNER_Wait
//* Object              :
//* Input Parameters    : 	tmUnitSelect_t tUnit
//* 						UInt32 tms
//* Output Parameters   : tmErrorCode_t.
//*--------------------------------------------------------------------------------------
tmErrorCode_t DRV_TUNER_Wait(tmUnitSelect_t tUnit, UInt32 tms)
{
	/* Variable declarations */
	tmErrorCode_t err = TM_OK;
	CH_UNUSED(tUnit);
	VK_TASK_Sleep(tms);

   return err;
}

//*--------------------------------------------------------------------------------------
//* Function Name       : DRV_TUNER_Print
//* Object              :
//* Input Parameters    : 	UInt32 level, const char* format, ...
//*
//* Output Parameters   : tmErrorCode_t.
//*--------------------------------------------------------------------------------------
tmErrorCode_t 			DRV_TUNER_Print(UInt32 level, const char* format, ...)
{
   /* Variable declarations */
   tmErrorCode_t err = TM_OK;

    static char     buf[PRINT_BUFFER_SIZE] = "\0";
    va_list         ap;

	//DEBUGLVL_VERBOSE , DEBUGLVL_MAX
	if( level<DEBUGLVL_MAX )
	{
		va_start(ap, format);
		(void)vsnprintf(buf, PRINT_BUFFER_SIZE, format, ap);
		va_end(ap);

		DI_UART_Print(buf);
		DI_UART_Print("\n");
	}

   return err;
}

//*--------------------------------------------------------------------------------------
//* Function Name       : DRV_TUNER_MutexInit
//* Object              :
//* Input Parameters    : 	ptmbslFrontEndMutexHandle *ppMutexHandle
//* Output Parameters   : tmErrorCode_t.
//*--------------------------------------------------------------------------------------
tmErrorCode_t  DRV_TUNER_MutexInit(ptmbslFrontEndMutexHandle *ppMutexHandle)
{
	/* Variable declarations */
	tmErrorCode_t err = TM_OK;
	CH_UNUSED(ppMutexHandle);

/* Customer code here */
/* ...*/

/* ...*/
/* End of Customer code here */

   return err;
}


//*--------------------------------------------------------------------------------------
//* Function Name       : DRV_TUNER_MutexDeInit
//* Object              :
//* Input Parameters    : 	 ptmbslFrontEndMutexHandle pMutex
//* Output Parameters   : tmErrorCode_t.
//*--------------------------------------------------------------------------------------
tmErrorCode_t  DRV_TUNER_MutexDeInit( ptmbslFrontEndMutexHandle pMutex)
{
	/* Variable declarations */
	tmErrorCode_t err = TM_OK;
	CH_UNUSED(pMutex);
/* Customer code here */
/* ...*/

/* ...*/
/* End of Customer code here */

   return err;
}



//*--------------------------------------------------------------------------------------
//* Function Name       : DRV_TUNER_MutexAcquire
//* Object              :
//* Input Parameters    : 	ptmbslFrontEndMutexHandle pMutex, UInt32 timeOut
//* Output Parameters   : tmErrorCode_t.
//*--------------------------------------------------------------------------------------
tmErrorCode_t  DRV_TUNER_MutexAcquire(ptmbslFrontEndMutexHandle pMutex, UInt32 timeOut)
{
	/* Variable declarations */
	tmErrorCode_t err = TM_OK;
	CH_UNUSED(pMutex);
	CH_UNUSED(timeOut);
/* Customer code here */
/* ...*/

/* ...*/
/* End of Customer code here */

   return err;
}

//*--------------------------------------------------------------------------------------
//* Function Name       : DRV_TUNER_MutexRelease
//* Object              :
//* Input Parameters    : 	ptmbslFrontEndMutexHandle pMutex
//* Output Parameters   : tmErrorCode_t.
//*--------------------------------------------------------------------------------------
tmErrorCode_t  DRV_TUNER_MutexRelease(ptmbslFrontEndMutexHandle pMutex)
{
	/* Variable declarations */
	tmErrorCode_t err = TM_OK;
	CH_UNUSED(pMutex);
/* Customer code here */
/* ...*/

/* ...*/
/* End of Customer code here */

   return err;
}


int TDA18260_Initialize( int nUnitId )
{
	tmErrorCode_t err = TM_OK;
	tmbslFrontEndDependency_t sSrvTunerFunc;
	tmUnitSelect_t              tUnit=0;
    tmTDA18260_Stream_t         eStreamType;

	CH_DI_Print(0, ("+++ TDA18260_Initialize()...nUnitId(%d)\n", nUnitId));

	tUnit = nUnitId;
    eStreamType = GET_STREAM_TYPE(tUnit);

	sSrvTunerFunc.sIo.Write				= DRV_TUNER_I2CWrite;
	sSrvTunerFunc.sIo.Read				= DRV_TUNER_I2CRead;
	sSrvTunerFunc.sTime.Get				= Null;
	sSrvTunerFunc.sTime.Wait			= DRV_TUNER_Wait;
	sSrvTunerFunc.sDebug.Print			= DRV_TUNER_Print;
	sSrvTunerFunc.sMutex.Init 			= DRV_TUNER_MutexInit;
	sSrvTunerFunc.sMutex.DeInit			= DRV_TUNER_MutexDeInit;
	sSrvTunerFunc.sMutex.Acquire		= DRV_TUNER_MutexAcquire;
	sSrvTunerFunc.sMutex.Release		= DRV_TUNER_MutexRelease;

    sSrvTunerFunc.dwAdditionalDataSize	= sizeof(tmTDA18260_Stream_t);
	sSrvTunerFunc.pAdditionalData		= &eStreamType;

	/* TDA18260 Master Driver low layer setup */
	err = tmbslTDA18260_Init(tUnit, &sSrvTunerFunc);
	if(err != TM_OK)
	{
		CH_DI_Print(0, (">>> Error : tmbslTDA18260_Init()... in TDA18260_Initialize()... \n"));
		return err;
	}
	/* TDA18260 Master Hardware initialization */
	err = tmbslTDA18260_Reset(tUnit);
	if(err != TM_OK)
	{
		CH_DI_Print(0, (">>> Error : tmbslTDA18260_Reset()... in TDA18260_Initialize()... \n"));
		return err;
	}

	TDA18260_SetAgcCtrl(nUnitId);
	CH_DI_Print(3, ("--- TDA18260_Initialize()...nUnitId(%d)\n", nUnitId));

	return err;
}



int TDA18260_SetTune( int nUnitId, HUINT32 ulRfFreqHz, HUINT8 bandWidth )
{
	tmErrorCode_t err = TM_OK;
	tmTDA18260_standardMode_t TDA18260StdMode = tmTDA18260_DIG_9MHz;//tmTDA18260_DIG_8MHz;

	//UInt32 uRF = 770000000;
	HUINT32 ulIfFreq = 0;
	tmbslFrontEndState_t etPllLockStatus = tmbslFrontEndStateUnknown;
	CH_UNUSED(bandWidth);

	/* TDA18260 Master standard mode */
	err = tmbslTDA18260_SetStandardMode(nUnitId, TDA18260StdMode);
	if(err != TM_OK)
	{
		CH_DI_Print(0, (">>> Error : tmbslTDA18260SetStandardMode()... in TDA18260_SetTune()... \n"));
		return err;
	}
	/* TDA18260 Master RF frequency */
	err = tmbslTDA18260_SetRf(nUnitId, ulRfFreqHz);
	if(err != TM_OK)
	{
		CH_DI_Print(0, (">>> Error : tmbslTDA18260SetRf()... in TDA18260_SetTune()... \n"));
		return err;
	}

	/* Get TDA18260 PLL Lock status */
	err = tmbslTDA18260_GetLockStatus(nUnitId, &etPllLockStatus);
	if(err != TM_OK)
	{
		CH_DI_Print(0, (">>> Error : TDA18260_SetTune()... err(0x%x)... \n", err));
	}
	else
	{
		if( etPllLockStatus == tmbslFrontEndStateLocked )
		{
			CH_DI_Print(1, ("<<<<<<<<<<<<<<<<<<<<<<<<< tmbslFrontEndStateLocked!!! \n"));
		}
		else
		{
			CH_DI_Print(0, (">>>>>>>>>>>>>>>>>>>>>>>>> etPllLockStatus(%d)!!! \n", etPllLockStatus));
		}
	}

	err = tmbslTDA18260_GetIF(nUnitId, (UInt32 *)&ulIfFreq);
	if(err != TM_OK)
	{
		CH_DI_Print(0, (">>> Error : TDA18260_SetTune()... err(0x%x)... \n", err));
	}
	else
	{
		CH_DI_Print(4, (">>>>>>>>>>>>>>>>>>>>>>>>> ulIfFreq(%d)!!! \n", ulIfFreq));
		s_TunerIfFreq[nUnitId] = ulIfFreq;
	}

	CH_DI_Print(4, ("&&&&&&&&&&&&&&&&&&&&&&&&&  TDA18260_SetTune()... \n"));

	return err;
}


int TDA18260_GetSignalStrength(int nUnitId, float *pfPower)
{
	tmErrorCode_t err = TM_OK;
	Int32 nRfLevel = 0;
	HUINT8 ucRssiValue = 0;//TODO_ILEE
	tmUnitSelect_t  tUnit = 0;

	tUnit = (tmUnitSelect_t)nUnitId;

	CH_DI_Print(5, ("TDA18260_GetSignalStrength()... Unit(0x%x)... \n", nUnitId));
	err = tmbslTDA18260_GetPowerLevel(tUnit, &nRfLevel);
	if(err != TM_OK)
	{
		CH_DI_Print(0, (">>> Error : TDA18260_GetSignalStrength()... err(0x%x)... \n", err));
	}
	else
	{
		CH_DI_Print(4, (">>>>>>>>>>>> nRfLevel(%d) \n", nRfLevel));
		*pfPower = (float)nRfLevel/100.0;
		CH_DI_Print(4, (">>>>>>>>>>>> fRfLevel(%f)dBmV \n", *pfPower));
	}

	if(*pfPower >= 55)	ucRssiValue = 100;
	else if(*pfPower >= 53)	ucRssiValue = 90;
	else if(*pfPower >= 51)	ucRssiValue = 80;
	else if(*pfPower >= 49)	ucRssiValue = 70;
	else if(*pfPower >= 47)	ucRssiValue = 60;
	else if(*pfPower >= 45)	ucRssiValue = 50;
	else if(*pfPower >= 43)	ucRssiValue = 40;
	else if(*pfPower >= 41)	ucRssiValue = 30;
	else if(*pfPower >= 39)	ucRssiValue = 20;
	else 	ucRssiValue = 10;

	return ucRssiValue;
}


void TDA18260_DumpRegister(int nUnitId)
{
	tmErrorCode_t 	err = TM_OK;
	HUINT8			ucAddr=0x00;
	HUINT8			ucBuffer[NUM_OF_REG];
	HUINT8			i;

	err = DRV_TUNER_I2CRead(nUnitId, 1, &ucAddr, NUM_OF_REG, ucBuffer);
	if (err == TM_OK)
	{
		for(i=0; i<NUM_OF_REG; i++)
		{
			CH_DI_Print(0, ("[0x%02X] \t[0x%02x]\n", i, ucBuffer[i]));
		}
	}
	else
	{
		CH_DI_Print(0, ("TDA18260_DumpRegister[%d] :: Fail!! Error[0x%0X]\n", nUnitId, err));
	}
}

void TDA18260_GetRfAgcParam(int nUnitId)
{
	HUINT8	ucHexVal1 = 0;
	HUINT8	ucHexVal2 = 0;
	HUINT8	ucRegAddr = 0;
	HUINT8	ucRegVal = 0;
	HINT32	nDecVal = 0;
	HFLOAT64	fRfAgcK = 0;

	ucRegAddr = 0x31;
	DRV_TUNER_I2CRead(nUnitId, 1, &ucRegAddr, 1, &ucRegVal);

	/* RFAGCs_Gain_byte_3[5:4] = RF_Filter_Gain */
	/* RFAGCs_Gain_byte_3[3:0] = LNA_Gain */
	ucHexVal1 = (ucRegVal >> 4) & 0x03;
	switch (ucHexVal1)
	{
		case	0x00 : nDecVal = -11;	break;
		case	0x01 : nDecVal = -8;	break;
		case	0x02 : nDecVal = -5;	break;
		case	0x03 : nDecVal = -2;	break;
		default : nDecVal = 0;	break;
	}
	CH_DI_Print(6, (">>>>>>>>>>>> TDA18260_GetRfAgcParam[%d] : RF_Filter_Gain = %d\n", nUnitId, nDecVal));

	ucHexVal1 = ucRegVal & 0x0F;
	switch (ucHexVal1)
	{
		case	0x00 : nDecVal = -10;	break;
		case	0x01 : nDecVal = -7;	break;
		case	0x02 : nDecVal = -4;	break;
		case	0x03 : nDecVal = -1;	break;
		case	0x04 : nDecVal =  2;	break;
		case	0x05 : nDecVal =  5;	break;
		case	0x06 : nDecVal =  8;	break;
		case	0x07 : nDecVal = 11;	break;
		case	0x08 : nDecVal = 14;	break;
		case	0x09 : nDecVal = 17;	break;
		case	0x0A : nDecVal = 20;	break;
		default : nDecVal = 0;	break;
	}
	CH_DI_Print(6, (">>>>>>>>>>>> TDA18260_GetRfAgcParam[%d] : LNA_Gain       = %d\n", nUnitId, nDecVal));

	ucRegAddr = 0x35;
	DRV_TUNER_I2CRead(nUnitId, 1, &ucRegAddr, 1, &ucRegVal);

	/* IFAGCs_Gain_byte[5:3] = LOWPASS_Gain */
	/* IFAGCs_Gain_byte[2:0] = Mixer_Gain */
	ucHexVal1 = (ucRegVal >> 3) & 0x07;
	switch (ucHexVal1)
	{
		case	0x00 : nDecVal = -6;	break;
		case	0x01 : nDecVal = -3;	break;
		case	0x02 : nDecVal =  0;	break;
		case	0x03 : nDecVal =  3;	break;
		case	0x04 : nDecVal =  6;	break;
		case	0x05 : nDecVal =  9;	break;
		case	0x06 : nDecVal = 12;	break;
		case	0x07 : nDecVal = 15;	break;
		default : nDecVal = 0;	break;
	}
	CH_DI_Print(6, (">>>>>>>>>>>> TDA18260_GetRfAgcParam[%d] : LOWPASS_Gain   = %d\n", nUnitId, nDecVal));

	ucHexVal1 = ucRegVal & 0x07;
	switch (ucHexVal1)
	{
		case	0x00 : nDecVal =  2;	break;
		case	0x01 : nDecVal =  5;	break;
		case	0x02 : nDecVal =  8;	break;
		case	0x03 : nDecVal = 11;	break;
		case	0x04 : nDecVal = 14;	break;
		default : nDecVal = 0;	break;
	}
	CH_DI_Print(6, (">>>>>>>>>>>> TDA18260_GetRfAgcParam[%d] : Mixer_Gain     = %d\n", nUnitId, nDecVal));

	/* RFAGCs_Gain_byte_5[7], RFAGCs_Gain_byte_6[7:0] = RFAGC_K_Read */
	nDecVal = 0;
	ucRegAddr = 0x33;
	DRV_TUNER_I2CRead(nUnitId, 1, &ucRegAddr, 1, &ucRegVal);

	nDecVal = ucRegVal << 1;

	ucRegAddr = 0x34;
	DRV_TUNER_I2CRead(nUnitId, 1, &ucRegAddr, 1, &ucRegVal);

	nDecVal = nDecVal | ucRegVal;

	/* RFAGCs_Gain_byte_1[7] = PLD_DAC_Scale */
	ucRegAddr = 0x24;
	DRV_TUNER_I2CRead(nUnitId, 1, &ucRegAddr, 1, &ucRegVal);

	ucHexVal1 = ((ucRegVal & 0x80)!=0) ? 1 : 0;

	/* W_Filter_byte[6] = RF_Atten_3dB */
	ucRegAddr = 0x11;
	DRV_TUNER_I2CRead(nUnitId, 1, &ucRegAddr, 1, &ucRegVal);

	ucHexVal2 = ((ucRegVal & 0x40)!=0) ? 1 : 0;

	switch (ucHexVal1)
	{
		case	0	:	fRfAgcK = g_fRfAgcTable_PldDacScale0_ES2[nDecVal] - 3*ucHexVal2;	break;
		case	1	:	fRfAgcK = g_fRfAgcTable_PldDacScale1_ES2[nDecVal] - 3*ucHexVal2;	break;
	}

	CH_DI_Print(6, (">>>>>>>>>>>> TDA18260_GetRfAgcParam[%d] : RFAGC_K_Read   = %4.3f\n", nUnitId, fRfAgcK));
}


HUINT32 TDA18260_GetIfFreq(int nUnitId)
{
	return s_TunerIfFreq[nUnitId];
}



void TDA18260_GetAgcGain(int nUnitId)
{
	HUINT8	ucHexVal1 = 0;
	HUINT8	ucRegAddr = 0;
	HUINT8	ucRegVal = 0;
	//HINT32	nDecVal = 0;
	//HFLOAT64	fRfAgcK = 0;
	float	fReadVal = 0.0;

	ucRegAddr = 0x16;
	DRV_TUNER_I2CRead(nUnitId, 1, &ucRegAddr, 1, &ucRegVal);

	ucHexVal1 = ucRegVal & 0x0f;

	switch (ucHexVal1)
	{
		case	0 :
		case	1 :
		case	2 :
		case	3 :  fReadVal = 0.0;	break;
		case	4 :  fReadVal = 3.0;	break;
		case	5 :  fReadVal = 4.875;	break;
		case	6 :  fReadVal = 6.75;	break;
		case	7 :  fReadVal = 8.625;	break;
		case	8 :  fReadVal = 10.5;	break;
		case	9 :  fReadVal = 12.375;	break;
		case	10 : fReadVal = 14.25;	break;
		case	11 : fReadVal = 16.125;	break;
		case	12 :
		case	13 :
		case	14 :
		case	15 : fReadVal = 18.0;	break;

		default : fReadVal = 0.0;	break;
	}
	CH_DI_Print(3, (">>>>>>>>>>>> TDA18260_GetAgcGain[%d] : START \n", nUnitId));
	CH_DI_Print(3, ("AGC1_GAIN_READ    = (%f)dB\n", fReadVal));

	ucRegVal = 0;
	fReadVal = 0.0;
	ucRegAddr = 0x5E;

	DRV_TUNER_I2CRead(nUnitId, 1, &ucRegAddr, 1, &ucRegVal);

	fReadVal = 0.2 * ucRegVal -12;

	CH_DI_Print(3, ("SA_AGC2_GAIN_READ = (%f)dB\n", fReadVal));

	ucRegVal = 0;
	fReadVal = 0.0;
	ucRegAddr = 0x9E;

	DRV_TUNER_I2CRead(nUnitId, 1, &ucRegAddr, 1, &ucRegVal);

	fReadVal = 0.2 * ucRegVal -12;

	CH_DI_Print(3, ("SB_AGC2_GAIN_READ = (%f)dB\n", fReadVal));
	CH_DI_Print(3, (">>>>>>>>>>>> TDA18260_GetAgcGain[%d] : END \n", nUnitId));

	return;
}



static void TDA18260_SetAgcCtrl(int nUnitId)
{
	HUINT8	ucRegAddr = 0;
	HUINT8	ucRegVal = 0;

	CH_DI_Print(6, ("+++ TDA18260_SetAgcCtrl[%d] : START \n", nUnitId));

	ucRegAddr = 0x0C;
	DRV_TUNER_I2CRead(nUnitId, 1, &ucRegAddr, 1, &ucRegVal);

	CH_DI_Print(6, ("AGC1_CTRL    = (0x%x)\n", ucRegVal));

	ucRegVal |= 0x30;///* AGC1_CTRL[5:4] = LT_OFFSET[1:0] = 3  i.e. 4.5dB  */

	DRV_TUNER_I2CWrite(nUnitId, 1, &ucRegAddr, 1, &ucRegVal);

	CH_DI_Print(6, ("--- TDA18260_SetAgcCtrl[%d] : END \n", nUnitId));

	return;
}

/*********************** End of File ******************************/


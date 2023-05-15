/**
	@file     arabic_support.c
	@brief    related arabic combination

	Description: File에 대한 설명을 적습니다.		\n
	Module: Layer와 모듈 명을 적습니다.			 	\n

	Copyright (c) 2008 HUMAX Co., Ltd.				\n
	All rights reserved.							\n
*/

/*******************************************************************/
/********************      Header Files    *************************/
/*******************************************************************/

#include <nx_common.h>

#include <nx_prism_app.h>


/*******************************************************************/
/********************      Global Variables     ********************/
/*******************************************************************/


/*******************************************************************/
/********************      Defines         *************************/
/*******************************************************************/
#define	UNICODE_CONVERSION_BUFFER	(256 + 16)	/* unsigned short * (256 + 16)*/

/*******************************************************************/
/********************      Typedef         *************************/
/*******************************************************************/
typedef struct UNICODE_PositionItem
{
	HUINT16 	usIsolated;
	HUINT16 	usFinal;
	HUINT16 	usInitial;
	HUINT16 	usMedial;
}UNICODE_toPositionItem;

STATIC UNICODE_toPositionItem UNICODE_TO_POSITION_CONTEXTUAL[] =
{
		{0xFE80,	0xFE80,		0xFE80,		0xFE80},	//0x0621
		{0xFE81,	0xFE82,		0xFE81,		0xFE81}, 	//0x0622
		{0xFE83,	0xFE84,		0xFE83,		0xFE83},	//0x0623
		{0xFE85,	0xFE86,		0xFE85,		0xFE85},	//0x0624
		{0xFE87,	0xFE88,		0xFE87,		0xFE87},	//0x0625
		{0xFE89,	0xFE8A,		0xFE8B,		0xFE8C},	//0x0626
		{0xFE8D, 	0xFE8E,		0xFE8D,		0xFE8D},	//0x0627
		{0xFE8F,	0xFE90,		0xFE91,		0xFE92},	//0x0628
		{0xFE93,	0xFE94,		0xFE93,		0xFE93},	//0x0629
		{0xFE95,	0xFE96,		0xFE97,		0xFE98},	//0x062A
		{0xFE99,	0xFE9A,		0xFE9B,		0xFE9C},	//0x062B
		{0xFE9D,	0xFE9E,		0xFE9F,		0xFEA0},	//0x062C
		{0xFEA1,	0xFEA2,		0xFEA3,		0xFEA4},	//0x062D
		{0xFEA5,	0xFEA6,		0xFEA7,		0xFEA8},	//0x062E
		{0xFEA9,	0xFEAA,		0xFEA9,		0xFEA9},	//0x062F
		{0xFEAB,	0xFEAC,		0xFEAB,		0xFEAB},	//0x0630
		{0xFEAD,	0xFEAE,		0xFEAD,		0xFEAD},	//0x0631
		{0xFEAF,	0xFEB0,		0xFEAF,		0xFEAF},	//0x0632
		{0xFEB1,	0xFEB2,		0xFEB3,		0xFEB4},	//0x0633
		{0xFEB5,	0xFEB6,		0xFEB7,		0xFEB8},	//0x0634
		{0xFEB9,	0xFEBA,		0xFEBB,		0xFEBC},	//0x0635
		{0xFEBD,	0xFEBE,		0xFEBF,		0xFEC0},	//0x0636
		{0xFEC1,	0xFEC2,		0xFEC3,		0xFEC4},	//0x0637
		{0xFEC5,	0xFEC6,		0xFEC7,		0xFEC8},	//0x0638
		{0xFEC9,	0xFECA,		0xFECB,		0xFECC},	//0x0639
		{0xFECD,	0xFECE,		0xFECF,		0xFED0},	//0x063A
		{0x063B,	0x063B,		0x063B,		0x063B},		//0x063B
		{0x063C,	0x063C,		0x063C,		0x063C},		//0x063C
		{0x063D,	0x063D,		0x063D,		0x063D},		//0x063D
		{0x063E,	0x063E,		0x063E,		0x063E},		//0x063E
		{0x063F,	0x063F,		0x063F,		0x063F},		//0x063F
		{0x0640,	0x0640,		0x0640,		0x0640},	    //0x0640
		{0xFED1,	0xFED2,		0xFED3,		0xFED4},	//0x0641
		{0xFED5,	0xFED6,		0xFED7,		0xFED8},	//0x0642
		{0xFED9,	0xFEDA,		0xFEDB,		0xFEDC},	//0x0643
		{0xFEDD,	0xFEDE,		0xFEDF,		0xFEE0},	//0x0644  //LAM
		{0xFEE1,	0xFEE2,		0xFEE3,		0xFEE4},	//0x0645
		{0xFEE5,	0xFEE6,		0xFEE7,		0xFEE8},	//0x0646
		{0xFEE9,	0xFEEA,		0xFEEB,		0xFEEC},	//0x0647
		{0xFEED,	0xFEEE,		0xFEED,		0xFEED},	//0x0648
		{0xFEEF,	0xFEF0,		0xFEEF,		0xFEEF},	//0x0649
		{0xFEF1,	0xFEF2,		0xFEF3,		0xFEF4},	//0x064A
#if defined(CONFIG_PROD_PROFILE_MINIOCTO)

		{0x0000,	0x0000,		0x0000,		0x0000},	    //0x064B   NULL SPACE
		{0x0000,	0x0000, 	0x0000, 	0x0000},		//0x064C   NULL SPACE
		{0x0000,	0x0000,		0x0000,		0x0000},	    //0x064D   NULL SPACE
		{0x0000,	0x0000,		0x0000,		0x0000},	    //0x064E   NULL SPACE
		{0x0000,	0x0000,		0x0000,		0x0000},	    //0x064F   NULL SPACE
		{0x0000,	0x0000,		0x0000,		0x0000},	    //0x0650   NULL SPACE
		{0x0000,	0x0000,		0x0000,		0x0000},	    //0x0651   NULL SPACE
		{0x0000,	0x0000,		0x0000,		0x0000},	    //0x0652   NULL SPACE
#else
		{0x064B,	0x064B,		0x064B,		0x064B},	    //0x064B
		{0x064C,	0x064C,		0x064C,		0x064C},	    //0x064C
		{0x064D,	0x064D,		0x064D,		0x064D},	    //0x064D
		{0x064E,	0x064E,		0x064E,		0x064E},	    //0x064E
		{0x064F,	0x064F,		0x064F,		0x064F},	    //0x064F
		{0x0650,	0x0650,		0x0650,		0x0650},	    //0x0650
		{0x0651,	0x0651,		0x0651,		0x0651},	    //0x0651
		{0x0652,	0x0652,		0x0652,		0x0652},	    //0x0652
#endif
		{0x0653,	0x0653,		0x0653,		0x0653},	    //0x0653
		{0x0654,	0x0654,		0x0654,		0x0654},	    //0x0654
		{0x0655,	0x0655,		0x0655,		0x0655},	    //0x0655
		{0x0656,	0x0656,		0x0656,		0x0656},	    //0x0656
		{0x0657,	0x0657,		0x0657,		0x0657},	    //0x0657
		{0x0658,	0x0658,		0x0658,		0x0658},	    //0x0658
		{0x0659,	0x0659,		0x0659,		0x0659},	    //0x0659
		{0x065A,	0x065A,		0x065A,		0x065A},	    //0x065A
		{0x065B,	0x065B,		0x065B,		0x065B},	    //0x065B
		{0x065C,	0x065C,		0x065C,		0x065C},	    //0x065C
		{0x065D,	0x065D,		0x065D,		0x065D},	    //0x065D
		{0x065E,	0x065E,		0x065E,		0x065E},	    //0x065E
		{0x0000,	0x0000,		0x0000,		0x0000},	    //0x065F   NULL SPACE
		{0x0660,	0x0660,		0x0660,		0x0660},	    //0x0660
		{0x0661,	0x0661,		0x0661,		0x0661},	    //0x0661
		{0x0662,	0x0662,		0x0662,		0x0662},	    //0x0662
		{0x0663,	0x0663,		0x0663,		0x0663},	    //0x0663
		{0x0664,	0x0664,		0x0664,		0x0664},	    //0x0664
		{0x0665,	0x0665,		0x0665,		0x0665},	    //0x0665
		{0x0666,	0x0666,		0x0666,		0x0666},	    //0x0666
		{0x0667,	0x0667,		0x0667,		0x0667},	    //0x0667
		{0x0668,	0x0668,		0x0668,		0x0668},	    //0x0668
		{0x0669,	0x0669,		0x0669,		0x0669},	    //0x0669
		{0x066A,	0x066A,		0x066A,		0x066A},	    //0x066A
		{0x066B,	0x066B,		0x066B,		0x066B},	    //0x066B
		{0x066C,	0x066C,		0x066C,		0x066C},	    //0x066C
		{0x066D,	0x066D,		0x066D,		0x066D},	    //0x066D
		{0x066E,	0x066E,		0x066E,		0x066E},	    //0x066E
		{0x066F,	0x066F,		0x066F,		0x066F},	    //0x066F
		{0x0670,	0x0670,		0x0670,		0x0670},	    //0x0670
		{0x0671,	0x0671,		0x0671,		0x0671},	    //0x0671
		{0x0672,	0x0672,		0x0672,		0x0672},	    //0x0672
		{0x0673,	0x0673,		0x0673,		0x0673},	    //0x0673
		{0x0674,	0x0674,		0x0674,		0x0674},	    //0x0674
		{0x0675,	0x0675,		0x0675,		0x0675},	    //0x0675
		{0x0676,	0x0676,		0x0676,		0x0676},	    //0x0676
		{0x0677,	0x0677,		0x0677,		0x0677},	    //0x0677
		{0x0678,	0x0678,		0x0678,		0x0678},	    //0x0678
		{0x0679,	0x0679,		0x0679,		0x0679},	    //0x0679
		{0x067A,	0x067A,		0x067A,		0x067A},	    //0x067A
		{0x067B,	0x067B,		0x067B,		0x067B},	    //0x067B
		{0x067C,	0x067C,		0x067C,		0x067C},	    //0x067C
		{0x067D,	0x067D,		0x067D,		0x067D},	    //0x067D
		{0xFB56,	0xFB57,		0xFB58,		0xFB59},	//0x067E		For Persian
		{0x067F,	0x067F,		0x067F,		0x067F},	    //0x067F
		{0x0680,	0x0680,		0x0680,		0x0680},	    //0x0680
		{0x0681,	0x0681,		0x0681,		0x0681},	    //0x0681
		{0x0682,	0x0682,		0x0682,		0x0682},	    //0x0682
		{0x0683,	0x0683,		0x0683,		0x0683},	    //0x0683
		{0x0684,	0x0684,		0x0684,		0x0684},	    //0x0684
		{0x0685,	0x0685,		0x0685,		0x0685},	    //0x0685
		{0xFB7A,	0xFB7B,		0xFB7C,		0xFB7D},	//0x0686		For Persian
		{0x0687,	0x0687,		0x0687,		0x0687},	    //0x0687
		{0x0688,	0x0688,		0x0688,		0x0688},	    //0x0688
		{0x0689,	0x0689,		0x0689,		0x0689},	    //0x0689
		{0x068A,	0x068A,		0x068A,		0x068A},	    //0x068A
		{0x068B,	0x068B,		0x068B,		0x068B},	    //0x068B
		{0x068C,	0x068C,		0x068C,		0x068C},	    //0x068C
		{0x068D,	0x068D,		0x068D,		0x068D},	    //0x068D
		{0x068E,	0x068E,		0x068E,		0x068E},	    //0x068E
		{0x068F,	0x068F,		0x068F,		0x068F},	    //0x068F
		{0x0690,	0x0690,		0x0690,		0x0690},	    //0x0690
		{0x0691,	0x0691,		0x0691,		0x0691},	    //0x0691
		{0x0692,	0x0692,		0x0692,		0x0692},	    //0x0692
		{0x0693,	0x0693,		0x0693,		0x0693},	    //0x0693
		{0x0694,	0x0694,		0x0694,		0x0694},	    //0x0694
		{0x0695,	0x0695,		0x0695,		0x0695},	    //0x0695
		{0x0696,	0x0696,		0x0696,		0x0696},	    //0x0696
		{0x0697,	0x0697,		0x0697,		0x0697},	    //0x0697
		{0xFB8A,	0xFB8B,		0xFB8A,		0xFB8A},	//0x0698		For Persian
		{0x0699,	0x0699,		0x0699,		0x0699},	    //0x0699
		{0x069A,	0x069A,		0x069A,		0x069A},	    //0x069A
		{0x069B,	0x069B,		0x069B,		0x069B},	    //0x069B
		{0x069C,	0x069C,		0x069C,		0x069C},	    //0x069C
		{0x069D,	0x069D,		0x069D,		0x069D},	    //0x069D
		{0x069E,	0x069E,		0x069E,		0x069E},	    //0x069E
		{0x069F,	0x069F,		0x069F,		0x069F},	    //0x069F
		{0x06A0,	0x06A0,		0x06A0,		0x06A0},	    //0x06A0
		{0x06A1,	0x06A1,		0x06A1,		0x06A1},	    //0x06A1
		{0x06A2,	0x06A2,		0x06A2,		0x06A2},	    //0x06A2
		{0x06A3,	0x06A3,		0x06A3,		0x06A3},	    //0x06A3
		{0x06A4,	0x06A4,		0x06A4,		0x06A4},	    //0x06A4
		{0x06A5,	0x06A5,		0x06A5,		0x06A5},	    //0x06A5
		{0x06A6,	0x06A6,		0x06A6,		0x06A6},	    //0x06A6
		{0x06A7,	0x06A7,		0x06A7,		0x06A7},	    //0x06A7
		{0x06A8,	0x06A8,		0x06A8,		0x06A8},	    //0x06A8
		{0xFB8E,	0xFB8F,		0xFB90,		0xFB91},	//0x06A9		For Persian
		{0x06AA,	0x06AA,		0x06AA,		0x06AA},	    //0x06AA
		{0x06AB,	0x06AB,		0x06AB,		0x06AB},	    //0x06AB
		{0x06AC,	0x06AC,		0x06AC,		0x06AC},	    //0x06AC
		{0x06AD,	0x06AD,		0x06AD,		0x06AD},	    //0x06AD
		{0x06AE,	0x06AE,		0x06AE,		0x06AE},	    //0x06AE
		{0xFB92,	0xFB93,		0xFB94,		0xFB95},	//0x06AF		For Persian
		{0x06B0,	0x06B0,		0x06B0,		0x06B0},	    //0x06B0
		{0x06B1,	0x06B1,		0x06B1,		0x06B1},	    //0x06B1
		{0x06B2,	0x06B2,		0x06B2,		0x06B2},	    //0x06B2
		{0x06B3,	0x06B3,		0x06B3,		0x06B3},	    //0x06B3
		{0x06B4,	0x06B4,		0x06B4,		0x06B4},	    //0x06B4
		{0x06B5,	0x06B5,		0x06B5,		0x06B5},	    //0x06B5
		{0x06B6,	0x06B6,		0x06B6,		0x06B6},	    //0x06B6
		{0x06B7,	0x06B7,		0x06B7,		0x06B7},	    //0x06B7
		{0x06B8,	0x06B8,		0x06B8,		0x06B8},	    //0x06B8
		{0x06B9,	0x06B9,		0x06B9,		0x06B9},	    //0x06B9
		{0x06BA,	0x06BA,		0x06BA,		0x06BA},	    //0x06BA
		{0x06BB,	0x06BB,		0x06BB,		0x06BB},	    //0x06BB
		{0x06BC,	0x06BC,		0x06BC,		0x06BC},	    //0x06BC
		{0x06BD,	0x06BD,		0x06BD,		0x06BD},	    //0x06BD
		{0x06BE,	0x06BE,		0x06BE,		0x06BE},	    //0x06BE
		{0x06BF,	0x06BF,		0x06BF,		0x06BF},	    //0x06BF
		{0x06C0,	0x06C0,		0x06C0,		0x06C0},	    //0x06C0
		{0x06C1,	0x06C1,		0x06C1,		0x06C1},	    //0x06C1
		{0x06C2,	0x06C2,		0x06C2,		0x06C2},	    //0x06C2
		{0x06C3,	0x06C3,		0x06C3,		0x06C3},	    //0x06C3
		{0x06C4,	0x06C4,		0x06C4,		0x06C4},	    //0x06C4
		{0x06C5,	0x06C5,		0x06C5,		0x06C5},	    //0x06C5
		{0x06C6,	0x06C6,		0x06C6,		0x06C6},	    //0x06C6
		{0x06C7,	0x06C7,		0x06C7,		0x06C7},	    //0x06C7
		{0x06C8,	0x06C8,		0x06C8,		0x06C8},	    //0x06C8
		{0x06C9,	0x06C9,		0x06C9,		0x06C9},	    //0x06C9
		{0x06CA,	0x06CA,		0x06CA,		0x06CA},	    //0x06CA
		{0x06CB,	0x06CB,		0x06CB,		0x06CB},	    //0x06CB
		{0xFBFC,	0xFBFD,		0xFBFE,		0xFBFF},	//0x06CC		For Persian
};

static const size_t utf8_skip_data_used_arabic[NX_TEXT_SIZE_256] = {
    1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,
    1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,
    1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,
    1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,
    1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,
    1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,
    2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,
    3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,4,4,4,4,4,4,4,4,5,5,5,5,6,6,1,1
};


/*******************************************************************/
/********************      Static Variables     ********************/
/*******************************************************************/

/*******************************************************************/
/********************      Functions       *************************/
/*******************************************************************/

HBOOL local_arabic_support_IsRightJoiningArabicChar(HUINT16 oneChar)
{
	if( (0x0622 <= oneChar && oneChar <= 0x0625) ||
		(oneChar == 0x0627) ||
		(oneChar == 0x0629) ||
		(0x062F <= oneChar && oneChar <= 0x0632) ||
		(0x0648 <= oneChar && oneChar <= 0x0649) ||
		(0x0671 <= oneChar && oneChar <= 0x0673) ||
		(0x0675 <= oneChar && oneChar <= 0x0677) ||
		(0x0688 <= oneChar && oneChar <= 0x0689) ||
		(0x0690 <= oneChar && oneChar <= 0x0699) ||
		(oneChar == 0x06C0) ||
		(0x06C2 <= oneChar && oneChar <= 0x06CB) ||
		(oneChar == 0x06CD) ||
		(oneChar == 0x06CF) ||
		(0x06D2 <= oneChar && oneChar <= 0x06D3) )
	{
		return TRUE;
	}

	return FALSE;

}

HBOOL local_arabic_support_IsDualJoiningArabicChar(HUINT16 oneChar)
{
	if( (oneChar == 0x0626) ||
		(oneChar == 0x0628) ||
		(0x062A <= oneChar && oneChar <= 0x062E) ||
		(0x0633 <= oneChar && oneChar <= 0x063A) ||
		(0x0641 <= oneChar && oneChar <= 0x0647) ||
		(oneChar == 0x064A) ||
		(0x0678 <= oneChar && oneChar <= 0x067E) ||
		(0x0680 <= oneChar && oneChar <= 0x0687) ||
		(0x069A <= oneChar && oneChar <= 0x06BF) ||
		(oneChar == 0x06C1) ||
		(oneChar == 0x06CC) ||
		(oneChar == 0x06CE) ||
		(0x06D0 <= oneChar && oneChar <= 0x06D1) ||
		(0x06FA <= oneChar && oneChar <= 0x06FC) )
	{
		return TRUE;
	}

	return FALSE;
}

HUINT32		local_arabic_support_UCS2Strlen(const HUINT16 *pusStr)
{
	const HUINT16	*pusIdx = pusStr;

	/* check parameter */
	if((pusStr == NULL) || (*pusStr == 0x0000))		{ return 0; }

	/* following text */
	for (/*none*/; *pusIdx != 0x0000; ++pusIdx)/* nothing */;

	/* buffer의 크기임 */
	return (pusIdx - pusStr);
}

HERROR	local_arabic_ConvertLogicalAraToVisualAra(HUINT16 *pLogStr, HINT32 nUnicodeStrLen, HUINT16 *pVisStr, HINT32 nBufSize, HINT32 *nVisualOrderStrLen)
{
	HINT32			i = 0;
	HINT32			j = 0;
	HINT32			temp = 0;
	HINT32			nStrLen = 0;
	HINT32			startPoint = 0;

	HUINT16 	*contextFormStr = NULL;
	HUINT16		nextCh = 0;
	HUINT16		prevCh = 0;
	HUINT16		currCh = 0;
	HUINT16		tempCh = 0;

	if( pLogStr == NULL || pVisStr == NULL || nVisualOrderStrLen == NULL)
	{
		return ERR_FAIL;
	}
	contextFormStr = (HUINT16 *)MW_MEM_Alloc(nBufSize * sizeof(HUINT16));
	if(contextFormStr == NULL)
	{
		return ERR_FAIL;
	}
	HxSTD_memset((HUINT16 *)(contextFormStr), 0, nBufSize * sizeof(HUINT16));


	/////////////////////// Basic glyphs for Arabic language contextual forms
	while(i <= nUnicodeStrLen)
	{
		if ( i == 0 )
		{
			prevCh = 0x0020;
		}
		else
		{
			prevCh = pLogStr[i-1];
		}

		currCh = pLogStr[i];

		if ( i == nUnicodeStrLen )
		{
			nextCh = 0x0020;
		}
		else
		{
			nextCh = pLogStr[i+1];
		}

		if( currCh >= 0x0621 && currCh <= 0x06CC )
		{
			if( local_arabic_support_IsRightJoiningArabicChar(currCh) && local_arabic_support_IsRightJoiningArabicChar(prevCh) )
			{
				if( (currCh == 0x0624 || currCh == 0x0648) && (prevCh == 0x0626 || prevCh == 0x064a)  )
				{
					contextFormStr[i] = UNICODE_TO_POSITION_CONTEXTUAL[currCh - 0x0621].usFinal;
					if ( i-2 > 0 &&
						(   pLogStr[i-2] == 0x0624 ||
						pLogStr[i-2] == 0x0648 ||
						pLogStr[i-2] == 0x062F ||
						pLogStr[i-2] == 0x0630 ||
						pLogStr[i-2] == 0x0631 ||
						pLogStr[i-2] == 0x0691 ||
						pLogStr[i-2] == 0x0692 ||
						pLogStr[i-2] == 0x0695 ||
						pLogStr[i-2] == 0x0698)	) //zain, dal, waw, reh
					{
						contextFormStr[i-1] = UNICODE_TO_POSITION_CONTEXTUAL[prevCh - 0x0621].usInitial;
					}
				}
				else
				{
					contextFormStr[i] = UNICODE_TO_POSITION_CONTEXTUAL[currCh - 0x0621].usIsolated;
				}
			}
			else if( local_arabic_support_IsRightJoiningArabicChar(currCh) && local_arabic_support_IsDualJoiningArabicChar(prevCh) )
			{
				contextFormStr[i] = UNICODE_TO_POSITION_CONTEXTUAL[currCh - 0x0621].usFinal;
			}
			else if( local_arabic_support_IsDualJoiningArabicChar(currCh) && local_arabic_support_IsDualJoiningArabicChar(prevCh) )
			{
				if( nextCh >= 0x0621 && nextCh <= 0x06CC )
				{
					contextFormStr[i] = UNICODE_TO_POSITION_CONTEXTUAL[currCh - 0x0621].usMedial;
				}
				else
				{
					contextFormStr[i] = UNICODE_TO_POSITION_CONTEXTUAL[currCh - 0x0621].usFinal;
				}
			}
			else if( local_arabic_support_IsDualJoiningArabicChar(currCh) && local_arabic_support_IsRightJoiningArabicChar(prevCh) )
			{
				if( nextCh >= 0x0621 && nextCh <= 0x06CC )
				{
					contextFormStr[i] = UNICODE_TO_POSITION_CONTEXTUAL[currCh - 0x0621].usInitial;
				}
				else
				{
					contextFormStr[i] = UNICODE_TO_POSITION_CONTEXTUAL[currCh - 0x0621].usIsolated;
				}
			}
			else if( (prevCh >= 0x0621 && prevCh <= 0x06CC) && (nextCh >= 0x0621 && nextCh <= 0x06CC) )
			{
				contextFormStr[i] = UNICODE_TO_POSITION_CONTEXTUAL[currCh - 0x0621].usMedial;
			}
			else if( !(prevCh >= 0x0621 && prevCh <= 0x06CC) && (nextCh >= 0x0621 && nextCh <= 0x06CC) )//
			{
				contextFormStr[i] = UNICODE_TO_POSITION_CONTEXTUAL[currCh - 0x0621].usInitial;
			}
			else if( !(prevCh >= 0x0621 && prevCh <= 0x06CC) && !(nextCh >= 0x0621 && nextCh <= 0x06CC) )
			{
				contextFormStr[i] = UNICODE_TO_POSITION_CONTEXTUAL[currCh - 0x0621].usIsolated;
			}
			else if( (prevCh >= 0x0621 && prevCh <= 0x06CC) && !(nextCh >= 0x0621 && nextCh <= 0x06CC) )
			{
				contextFormStr[i] = UNICODE_TO_POSITION_CONTEXTUAL[currCh - 0x0621].usFinal;
			}

			if( prevCh == 0x0622 || prevCh == 0x0623 || prevCh == 0x0625 || prevCh == 0x0627 ) //Alef
			{
				if( nextCh >= 0x0621 && nextCh <= 0x06CC )
				{
					contextFormStr[i] = UNICODE_TO_POSITION_CONTEXTUAL[currCh - 0x0621].usInitial;
				}
				else
				{
					contextFormStr[i] = UNICODE_TO_POSITION_CONTEXTUAL[currCh - 0x0621].usIsolated;
				}
			}

			////////ligature, LAM WITH ALEF form, LAM이 isolated 혹은 final일때
			if (contextFormStr[i]==0xFEDD || contextFormStr[i]==0xFEDE)//FEDD
			{
				if( nextCh==0x0622 )
				{
					contextFormStr[i] = 0xFEF5 + (contextFormStr[i] - 0xFEDD);
					i++;
				}
				else if( nextCh==0x0623 )
				{
					contextFormStr[i] = 0xFEF7 + (contextFormStr[i] - 0xFEDD);
					i++;
				}
				else if( nextCh==0x0625 )
				{
					contextFormStr[i] = 0xFEF9 + (contextFormStr[i] - 0xFEDD);
					i++;
				}
				else if( nextCh==0x0627 )
				{
					contextFormStr[i] = 0xFEFB + (contextFormStr[i] - 0xFEDD);
					i++;
				}
			}
			else if (contextFormStr[i]==0xFEDF) // ligature,  LAM 이 initial일때
			{
				if( nextCh==0x0622 )
				{
					contextFormStr[i] = 0xFEF5;
					i++;
				}
				else if( nextCh==0x0623 )
				{
					contextFormStr[i] = 0xFEF7;
					i++;
				}
				else if( nextCh==0x0625 )
				{
					contextFormStr[i] = 0xFEF9;
					i++;
				}
				else if( nextCh==0x0627 )
				{
					contextFormStr[i] = 0xFEFB;
					i++;
				}
#if 0
				if (nextCh==0x0622 || nextCh==0x0623 || nextCh==0x0625 || nextCh==0x0627)
				{
					contextFormStr[i] = 0xFEFB;
					i++;
				}
#endif

			}
			else if (contextFormStr[i]==0xFEE0) // ligature,  LAM 이 medial일때
			{
				if (nextCh==0x0622 || nextCh==0x0623 || nextCh==0x0625 || nextCh==0x0627)
				{
					contextFormStr[i] = 0xFEFC;
					i++;
				}

			}

		}
		else if( !(currCh >= 0x0621 && currCh <= 0x064A) )
		{
			contextFormStr[i] = currCh;

		}

		i++;
	}


	///////////	//reverse the string, 영어,   숫자는 뒤집지 않는다, (아랍어로 표현되든, 1,2,3..으로 표현되든)


	i=0;
	nStrLen = nUnicodeStrLen;
	while(i <= nStrLen)
	{
		if( contextFormStr[i] == 0x0000)
		{
			startPoint++;
			i++;
			continue;

		}

		//괄호의 경우 반대 모양으로.
		if( contextFormStr[i] == 0x0028)
		{
			contextFormStr[i] = 0x0029;
		}
		else if( contextFormStr[i] == 0x0029)
		{
			contextFormStr[i] = 0x0028;
		}
		else if( contextFormStr[i] == 0x005B)
		{
			contextFormStr[i] = 0x005D;
		}
		else if( contextFormStr[i] == 0x005D)
		{
			contextFormStr[i] = 0x005B;
		}
		else if( contextFormStr[i] == 0x007B)
		{
			contextFormStr[i] = 0x007D;
		}
		else if( contextFormStr[i] == 0x007D)
		{
			contextFormStr[i] = 0x007B;
		}

		if( (contextFormStr[i] >= 0x060c && contextFormStr[i] <= 0x065A) ||
			(contextFormStr[i] >= 0x066A && contextFormStr[i] <= 0x06D5) ||
			(contextFormStr[i] >= 0xFB50 && contextFormStr[i] <= 0xFEFC) ||
			(contextFormStr[i] == 0x0020) ||
			(contextFormStr[i] == 0x0028) ||
			(contextFormStr[i] == 0x0029) ||
			(contextFormStr[i] == 0x005B) ||
			(contextFormStr[i] == 0x005D) ||
			(contextFormStr[i] == 0x007B) ||
			(contextFormStr[i] == 0x007D) ||
			(contextFormStr[i] == 0x2023) ||
			(contextFormStr[i] == 0x002C)
			//||(contextFormStr[i] == 0x003A)
			)//||(contextFormStr[i] == 0x0030) )// || (contextFormStr[i] == 0x002F))
		{
			pVisStr[nUnicodeStrLen] = contextFormStr[i];

		}
		else{

			temp = i;
			while(1)
			{
				if( temp > nStrLen )
				{
					break;

				}
				else if( (contextFormStr[temp] >= 0x060c && contextFormStr[temp] <= 0x065A) ||
					(contextFormStr[temp] >= 0x066A && contextFormStr[temp] <= 0x06D5) ||
					(contextFormStr[temp] >= 0xFB50 && contextFormStr[temp] <= 0xFEFC) ||
					(contextFormStr[temp] == 0x0020) ||
					(contextFormStr[temp] == 0x0028) ||
					(contextFormStr[temp] == 0x0029) ||
					(contextFormStr[temp] == 0x005B) ||
					(contextFormStr[temp] == 0x005D) ||
					(contextFormStr[temp] == 0x007B) ||
					(contextFormStr[temp] == 0x007D) ||
					(contextFormStr[temp] == 0x2023) ||
					(contextFormStr[temp] == 0x002C)
					//||(contextFormStr[temp] == 0x003A)
					)// || (contextFormStr[temp] == 0x002F))
				{

					if((temp+1<=nStrLen && temp+2<=nStrLen) && (contextFormStr[temp] == 0x0020 && (contextFormStr[temp+1] == 0x002d || contextFormStr[temp+1] == 0x007e) && contextFormStr[temp+2] == 0x0020) )
					{
						temp++;
					}
					else if((temp-1>=0 && temp-2>=0) && (contextFormStr[temp] == 0x0020 && (contextFormStr[temp-1] == 0x002d || contextFormStr[temp-1] == 0x007e) && contextFormStr[temp-2] == 0x0020) )
					{
						temp++;
					}
					else if(contextFormStr[temp] == 0x0020 && temp + 1 < nStrLen)
					{
						tempCh = 0;
						tempCh = contextFormStr[temp+1];
						if( !((tempCh >= 0x060c && tempCh <= 0x06F9) || (tempCh >= 0xFB50 && tempCh <= 0xFEFC)) )
							temp++;
						else
							break;
					}
					else
					{
						break;
					}

				}
				else
				{
					temp++;
				}
			}
			temp--;

			for(j=temp; j >= i; j--)
			{
				pVisStr[nUnicodeStrLen] = contextFormStr[j];

				nUnicodeStrLen--;
			}
			i = temp + 1;
			continue;

		}


		nUnicodeStrLen--;
		i++;

	}

	i=0;
	while(startPoint <= (nStrLen+1))
	{
		pVisStr[i] = pVisStr[startPoint];
		i++;
		startPoint++;
	}


	if(contextFormStr != NULL)
	{
		MW_MEM_Free(contextFormStr);
	}

	*nVisualOrderStrLen = local_arabic_support_UCS2Strlen(pVisStr);

	return ERR_OK;
}

HBOOL NX_PRISM_ARABIC_SUPPORT_CheckUnicodeArabicCharacterInString(HUINT16 *pStr)
{

	if( pStr == NULL )
	{
		return FALSE;
	}
	while (*pStr)
	{
		if( (*pStr >= 0x0621 && *pStr <= 0x06F9) ||
			(*pStr >= 0xFB50 && *pStr <= 0xFEFC) )
		{
			return TRUE;
		}
		pStr++;
	}
	return FALSE;

}

HxSTR_t *NX_PRISM_ARABIC_SUPPORT_ConvertLogicalAraToVisualAra(HxSTR_t *pStr)
{
	HINT32 		nByteLen	= 0;
	HINT32		nStrLen 	= 0;
	HINT32		nCharNum 	= 0;
	HUINT8		bufUtf8[UNICODE_CONVERSION_BUFFER] = {0, };
	HUINT8 		*pTempUtf 	= NULL;
	HUINT16 	*pVisualStr	= NULL;
	HxSTR_t 	*pTempStr	=	NULL;		// return HxSTR_t

	// get char num in str
	nCharNum	= HLIB_STR_GetNumOfChars(pStr);

	//exception. 총 nCharNum 이 UNICODE_CONVERSION_BUFFER 크면 처리하지 말자.
	if (nCharNum >= UNICODE_CONVERSION_BUFFER)
	{
		HxLOG_Print("\n[%s,%d] nCharNum(%d) is so Big!!\n\n",__FUNCTION__,__LINE__,nCharNum);
		return NULL;
	}

	pVisualStr	= (HUINT16 *)HxSTR_MemAlloc(sizeof(HUINT16) * (nCharNum + 1));

	if (pVisualStr == NULL)
	{
		HxLOG_Critical("[%s.%d] Failed  Mem alloc !!\n",__FUNCTION__,__LINE__);
		return NULL;
	}

	// get 조합된 arabic string
	HxSTD_MemSet(pVisualStr, 0x00, sizeof(HUINT16) * (nCharNum+1));
	local_arabic_ConvertLogicalAraToVisualAra((HUINT16 *)HLIB_STR_toUCS2(pStr), (nCharNum-1), pVisualStr, UNICODE_CONVERSION_BUFFER, &nStrLen);

	HxSTD_MemSet(bufUtf8,0,sizeof(HUINT8) * UNICODE_CONVERSION_BUFFER);
	HLIB_CP_ConvertUCS2ToCodepage(eHxCP_UNICODE_UTF8,(HUINT16 *)pVisualStr,(void *)bufUtf8, UNICODE_CONVERSION_BUFFER);

	if (pVisualStr)	HxSTR_MemFree(pVisualStr);

	// 변환된 UTF 의 길이.
	nByteLen = HLIB_CP_NumOfBytes(eHxCP_UNICODE_UTF8,bufUtf8);
	if (nByteLen > 0)
	{
		pTempUtf = (HUINT8 *)HxSTR_MemAlloc(sizeof(HUINT8) * (nByteLen+1));

		if (pTempUtf == NULL)
		{
			HxLOG_Critical("[%s.%d] Failed  Mem alloc !!\n",__FUNCTION__,__LINE__);
			return NULL;
		}

		HxSTD_MemSet(pTempUtf, 0x00, nByteLen+1);
		HxSTD_MemCopy(pTempUtf, bufUtf8, nByteLen);

		HLIB_STR_Empty(pStr);
		pTempStr = HLIB_STR_Append(pStr, pTempUtf);

		if (pTempUtf)	HxSTR_MemFree(pTempUtf);

	}

	return pTempStr;
}


HCHAR *NX_PRISM_ARABIC_SUPPORT_ConvertToArabicStr(HUINT8 *utfstr)
{
	HxSTR_t			*pstTotal_str = NULL, *pTempStr = NULL, *pstStrline = NULL;
	HUINT8			*pUtfStr = NULL, *pTemp = NULL;
	HINT32			j = 0, strLen = 0;
	HUINT8			*pTmpBuf = NULL;
	size_t 			utf8_size;
	HINT8			*pszRetBuf = NULL;

	pstTotal_str = HLIB_STR_New((HUINT8*)utfstr);//HLIB_STR_Clone(pSelf);

	if (pstTotal_str == NULL)
	{
		HxLOG_Error("Failed HLIB_STR_New !\n");
		return NULL;
	}

	// is arabic?
	if (NX_PRISM_ARABIC_SUPPORT_CheckUnicodeArabicCharacterInString(HLIB_STR_toUCS2(pstTotal_str)) == FALSE)
	{
		HxLOG_Error("Noting Arabic in String!\n");
		HLIB_STR_Delete(pstTotal_str);
		return NULL;
	}

	pUtfStr = HLIB_STR_GetBuffer(pstTotal_str);

	if (*pUtfStr != '\0')
	{
		j = 0;
		pTmpBuf = NX_APP_Calloc(sizeof(HUINT16) * HxSTD_StrLen((HCHAR*)pUtfStr));

		if (pTmpBuf == NULL)
			goto END_FUNC;

		while( *pUtfStr != '\0')
		{
			utf8_size = utf8_skip_data_used_arabic[(int)*pUtfStr];
			switch (utf8_size) {
	            case 6: pTmpBuf[j++] = *pUtfStr ++;
	            case 5: pTmpBuf[j++] = *pUtfStr ++;
	            case 4: pTmpBuf[j++] = *pUtfStr ++;
	            case 3: pTmpBuf[j++] = *pUtfStr ++;
	            case 2: pTmpBuf[j++] = *pUtfStr ++;
	            case 1: pTmpBuf[j++] = *pUtfStr ++;	break;
				default: pTmpBuf[j++] = *pUtfStr ++;	break;
        	}
		}// end while
		pTmpBuf[j] = '\0';

		pTemp = (HUINT8 *)NX_APP_Malloc( (j + 1) * sizeof(HUINT8));


		if (pTemp != NULL)
		{
			// copy string of current line
			HxSTD_StrNCpy((HCHAR*)pTemp, (HCHAR*)pTmpBuf , (sizeof(HUINT8) * (j + 1)) );
			pstStrline = HLIB_STR_New(pTemp);

			if (pstStrline == NULL)
				goto END_FUNC;

			pTempStr = (HxSTR_t *)NX_PRISM_ARABIC_SUPPORT_ConvertLogicalAraToVisualAra(pstStrline);

			if (pTempStr == NULL)
			{
				HxLOG_Debug("[%s,%d] Failed Convert To VisualAra!!\n",__FUNCTION__,__LINE__);
			}
			else
			{
				HxLOG_Debug("\n[%s,%d] Saved Visual Arabic !!!\n",__FUNCTION__,__LINE__);
				pstStrline = pTempStr;
			}

			strLen = HxSTD_StrLen((char*)HLIB_STR_GetBuffer(pstStrline));
			pszRetBuf = (HCHAR*)NX_APP_Calloc(strLen + 1);

			HxSTD_MemSet(pszRetBuf, '\0', strLen + 1);
			HxSTD_StrNCpy(pszRetBuf, (char*)HLIB_STR_GetBuffer(pstStrline), strLen);
			pszRetBuf[strLen] = '\0';

			if (pstStrline)
				HLIB_STR_Delete(pstStrline);
		}

	}// end for

END_FUNC:
	if (pTemp)
		HLIB_MEM_Free(pTemp);

	if (pTmpBuf)
		HLIB_MEM_Free(pTmpBuf);

	if (pstTotal_str)
		HLIB_STR_Delete(pstTotal_str);

	return pszRetBuf;
}

/*********************** End of File ******************************/


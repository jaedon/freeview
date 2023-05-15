/**************************************************************
*	@file	rema.c
*	http://www.humaxdigital.com
*	@author			humax
*	@brief			rema
**************************************************************/

/*
* ??2011-2012 Humax Co., Ltd.
* This program is produced by Humax Co., Ltd. ("Humax") and
* the proprietary Software of Humax and its licensors. Humax provides you, as an Authorized Licensee,
* non-assignable, non-transferable and non-exclusive license to use this Software.
* You acknowledge that this Software contains valuable trade secrets of Humax and by using this Software
* you agree to the responsibility to take all reasonable efforts to protect the any information
* you receive from Humax. You are not permitted to duplicate, modify, distribute, sell or lease and
* reverse engineer or extract the source code of this Software unless you have Humax's written permission to do so.
* If you have no authorized license, discontinue using this Software immediately.
*
* THE SOFTWARE IS PROVIDED "AS IS" AND HUMAX MAKES NO PROMISES, REPRESENTATIONS OR WARRANTIES, EITHER EXPRESS,
* IMPLIED OR STATUTORY, OR OTHERWISE, WITH RESPECT TO THE SOFTWARE.
* IN NO EVENT SHALL HUMAX BE LIABLE FOR LOST PROFITS, REVENUES, OR DATA, FINANCIAL LOSSES OR INDIRECT, SPECIAL,
*  CONSEQUENTIAL, EXEMPLARTY OR PUNITIVE DAMAGES WHATSOEVER RELATING TO YOUR USE OR INABILITY TO USE THE SOFTWARE.
*
* This License is effective until terminated. You may terminate this License at any time by destroying all copies
* of the Software including all documentation. This License will terminate immediately without notice from Humax
* to you if you fail to comply with any provision of this License. Upon termination, you must destroy all copies
* of the Software and all documentation.
*
* The laws of the Republic of Korea will apply to any disputes arising out of or relating to this Copyright Notice.
* All claims arising out of or relating to this Copyright Notice will be litigated in the Seoul Central District Court,
* in the Republic of Korea.
*/
// ?úÍ?
#define ___HEADER_FILES___

#include <vkernel.h>
#include <hlib.h>
#include <oapi.h>

#include <hapi.h>
#include <rema_int.h>
#include "CUnit/Basic.h"
#include "test_CUnit_Api.h"
#include "hCwmpApi.h"
#include <inttypes.h>
#include <iostream>
#include <vector>


#define ___DEFINES___
using namespace std;
#define ___MACROS___

#define GetBufName(Buf) #Buf
#define GetBufName3(Buf) Buf , sizeof(Buf), (const char*) #Buf

#define ___TYPEDEF___

#define ___CLASS___
class cNULLCheck_STR
{
public:
	cNULLCheck_STR(void)
	{
		m_pBuf=NULL;
		m_nBufLen=0;
		m_nSuccess=0 ;
		//TCU_MSG(">");
	}
	cNULLCheck_STR(int nSuccess, char *pBuf, int BufLen, const char *pszBufName)
	{
		m_nSuccess = nSuccess;
		m_pBuf=pBuf;
		m_nBufLen=BufLen;
		m_strBufName = pszBufName;
		
		//TCU_MSG(">");
	}
	cNULLCheck_STR(const cNULLCheck_STR &p)
	{
		*this = p;
		
		//TCU_MSG(">");

	}
	~cNULLCheck_STR(void)
	{
		m_pBuf=NULL;
		m_nBufLen=0;
		//TCU_MSG(">");
	}
public:
	char * m_pBuf;
	int    m_nBufLen;
	string m_strBufName;
	int    m_nSuccess;
public:
	
	cNULLCheck_STR& operator=(const cNULLCheck_STR & other)
	{
		if(this == &other)
			return *this;

		this->m_nSuccess = other.m_nSuccess;
		this->m_pBuf =other.m_pBuf;
		this->m_nBufLen =other.m_nBufLen;
		this->m_strBufName = other.m_strBufName;
		//TCU_MSG(">");
		return *this;
	}

};

class cNULLCheck_STR_Bool 
	: public cNULLCheck_STR
{
	public:
		cNULLCheck_STR_Bool(void)
			: cNULLCheck_STR()
	{
		m_bValue =false;
		//TCU_MSG(">");
	}
	cNULLCheck_STR_Bool(bool bValue, int nSuccess, char *pBuf, int BufLen, char *pszBufName)
		: cNULLCheck_STR(nSuccess, pBuf, BufLen, pszBufName)
	{
		m_bValue =bValue;
		//TCU_MSG(">");
	}
	cNULLCheck_STR_Bool(const cNULLCheck_STR_Bool &p)
	{
		*this = p;
		//TCU_MSG(">");

	}
	~cNULLCheck_STR_Bool(void) 
	{
		m_bValue =false;
		//TCU_MSG(">");
	}
public:
	bool m_bValue;
public:
	
	cNULLCheck_STR_Bool& operator=(const cNULLCheck_STR_Bool & other)
	{
		if(this == &other)
			return *this;
		::cNULLCheck_STR::operator=(other);

		//TCU_MSG(">");	
		this->m_bValue =other.m_bValue;
		return *this;
	}
};

#define ___STATIC_VARIABLES___

static void test_check_NULL_1(void)
{
	CU_PASS(hCwmpCore_GetDefaultOpts(NULL));
}
static void test_check_NULL_2(void)
{
	TCU_ASSERT_NOT_EQUAL(hCwmpCore_Init(NULL), ehCwmp_R_OK);
}
static void test_check_NULL_3(void)
{
	CU_PASS(hCwmpCore_Term());
}
static void test_check_NULL_4(void)
{
	CU_PASS(hCwmpUtil_Print(NULL,NULL,-1,NULL));
}

static void test_check_NULL_5(void)
{
	TCU_ASSERT_EQUAL(hCwmpDMManager_AddObject(NULL,NULL, -1, false), -1);
}

static void test_check_NULL_6(void)
{
	TCU_ASSERT_EQUAL(hCwmpDMManager_AddObjectWithFullName(NULL,NULL,false), -1);
}
static void test_check_NULL_7(void)
{
	TCU_ASSERT_EQUAL(hCwmpDMManager_AddInParam(NULL,NULL,-1,(hCwmpParamId_e)0), -1);

}
static void test_check_NULL_8(void)
{
	TCU_ASSERT_NOT_EQUAL(hCwmpDMManager_AddAllInParams(NULL), ehCwmp_R_OK);

}
static void test_check_NULL_9(void)
{
	TCU_ASSERT_EQUAL(hCwmpDMManager_AddExParam(NULL,NULL,-1,(hCwmpParamType_e)0,NULL,NULL), -1);
}
static void test_check_NULL_10(void)
{
	TCU_ASSERT_EQUAL(hCwmpDMManager_AddExParamWithFullName(NULL,NULL,(hCwmpParamType_e)0,NULL,NULL), -1);

}
static void test_check_NULL_11(void)
{
	CU_PASS(hCwmpDMManager_DelChildren(NULL,-1));
}

static void test_check_NULL_12(void)
{
	TCU_ASSERT_EQUAL(hCwmpDMManager_FindParam(NULL,NULL), -1);
}

static void test_check_NULL_13(void)
{
	CU_PASS(hCwmpDMManager_SetNotifiable(NULL, -1, 0));
}

static void test_check_NULL_14(void)
{
	CU_PASS(hCwmpDMManager_SetForcedInform(NULL, -1));
}

static void test_check_NULL_15(void)
{
	CU_PASS(hCwmpDMManager_SetAccessListSetter(NULL, -1, NULL));
}

static void test_check_NULL_16(void)
{
	CU_PASS(hCwmpDMManager_SetObjectUpdater(NULL, NULL, -1));
}

static void test_check_NULL_17(void)
{
	CU_PASS(hCwmpEngine_GetDefaultOpts(NULL));
}

static void test_check_NULL_18(void)
{
	CU_ASSERT_PTR_EQUAL_FATAL(hCwmpEngine_Open(NULL, NULL), NULL);
}

static void test_check_NULL_19(void)
{
	CU_PASS(hCwmpEngine_Close(NULL));
}

static void test_check_NULL_20(void)
{
	CU_PASS(hCwmpEngine_Stop(NULL));
}

static void test_check_NULL_21(void)
{
	CU_PASS(hCwmpEngine_AddAutonxferData(NULL, false, NULL, NULL, false, NULL, 0 , NULL,0 , NULL, NULL, 0 , 0));
}

static void test_check_NULL_22(void)
{
	CU_PASS(hCwmpEngine_AddAutonxferData(NULL, false, NULL, NULL, false, NULL, 0 , NULL,0 , NULL, NULL, 0 , 0));
}

static void test_check_NULL_23(void)
{
	char szData[256];
	int  nTemp;
	unsigned int  unTemp;
	TCU_ASSERT_EQUAL(hCwmpUtil_StrToINT32(NULL, NULL), -1);
	TCU_ASSERT_EQUAL(hCwmpUtil_StrToINT32(szData, NULL), -1);
	TCU_ASSERT_EQUAL(hCwmpUtil_StrToINT32(NULL, &nTemp), -1);

	TCU_ASSERT_EQUAL(hCwmpUtil_StrToUINT32(NULL, NULL), -1);
	TCU_ASSERT_EQUAL(hCwmpUtil_StrToUINT32(szData, NULL), -1);
	TCU_ASSERT_EQUAL(hCwmpUtil_StrToUINT32(NULL, &unTemp), -1);
}

static void test_check_NULL_24(void)
{
	int nRs;
	int i;


typedef	struct test_INT32_t
	{
		int nValue;
		char *pBuf;
		int  nBufLen;
		int nSucess;
}test_INT32_t;
char szTemp_1[1];
char szTemp_2[2]={"\0"};
char szTemp_5[5]={"\0"};
char szTemp_6[6]={"\0"};
char szTemp_256[256]={"\0"};
	
#define D_MAX_INT32  8
	test_INT32_t sttest_INT32[D_MAX_INT32]=
	{
		{0,NULL,0, 0},
		{1,NULL,256, 0},
		{50002,szTemp_1,1, 0},
		{50003,szTemp_2,2, 0},
		{50004,szTemp_5,5, 0},
		{50005,szTemp_6,6, 1},
		{50006,szTemp_256,256, 1},
		{50007,szTemp_256,1 , 0},
		//{50008,szTemp_1,256, 1}, //crash ?¥Î?Î∂ÑÏ? codeÎ°??úÏñ¥??Î∞©Î≤ï???ÜÏùå.
	};
	test_INT32_t *pstD;
	unsigned int nuValue;
	for(i=0 ; i< D_MAX_INT32 ; i++)
	{
		pstD = &sttest_INT32[i];
		
		///##################hCwmpUtil_INT32ToStr
		if(pstD->pBuf)
			sprintf(pstD->pBuf,"0");
		if(pstD->nSucess)
		{
			//TCU_MSG("#%d ###1", i); 
			nRs = TCU_ASSERT_NOT_EQUAL(hCwmpUtil_INT32ToStr(pstD->nValue, pstD->pBuf, pstD->nBufLen), -1);
		}
		else
		{
			//TCU_MSG("#%d ###2", i); 
			nRs = TCU_ASSERT_EQUAL(hCwmpUtil_INT32ToStr(pstD->nValue, pstD->pBuf, pstD->nBufLen), -1);
		}
		if(!nRs)
		{
			if(pstD->pBuf)
			{
				TCU_MSG("#%d  nRs(%d), nValue(%d), pBuf(%s) nBufLen[%d]", i, nRs, pstD->nValue, pstD->pBuf, pstD->nBufLen); 
			}
			else
			{
				TCU_MSG("#%d  nRs(%d), nValue(%d) nBufLen[%d]", i, nRs, pstD->nValue, pstD->nBufLen); 
			}
		}
		///##################hCwmpUtil_UINT32ToStr

		nuValue = (unsigned int)pstD->nValue;
		if(pstD->pBuf)
			sprintf(pstD->pBuf,"0");
		if(pstD->nSucess)
		{
			//TCU_MSG("#%d ###1", i); 
			nRs = TCU_ASSERT_NOT_EQUAL(hCwmpUtil_UINT32ToStr(nuValue, pstD->pBuf, pstD->nBufLen), -1);
		}
		else
		{
			//TCU_MSG("#%d ###2", i); 
			nRs = TCU_ASSERT_EQUAL(hCwmpUtil_UINT32ToStr(nuValue, pstD->pBuf, pstD->nBufLen), -1);
		}
		if(!nRs)
		{
			if(pstD->pBuf)
			{
				TCU_MSG("#%d  nRs(%d), nuValue(%ud), pBuf(%s) nBufLen[%d]", i, nRs, nuValue, pstD->pBuf, pstD->nBufLen); 
			}
			else
			{
				TCU_MSG("#%d  nRs(%d), nuValue(%ud) nBufLen[%d]", i, nRs, nuValue, pstD->nBufLen); 
			}
		}

		
	}
}


static void test_check_NULL_34(void)
{
	CU_ASSERT_PTR_EQUAL_FATAL(hCwmpUtil_DateTimeToStr(0, NULL, 0), -1);
}

static void test_check_NULL_35(void)
{
	CU_ASSERT_PTR_EQUAL_FATAL(hCwmpUtil_StrToTimeVal(NULL, NULL), -1);
}
static void test_check_NULL_36(void)
{
	CU_ASSERT_PTR_EQUAL_FATAL(hCwmpUtil_TimeValToStr(0, NULL, 0), -1);
}


static void test_check_NULL_37(void)
{
	CU_ASSERT_PTR_EQUAL_FATAL(hCwmpUtil_HexStrToBin(NULL, NULL, 0), -1);
}
static void test_check_NULL_38(void)
{
	CU_ASSERT_PTR_EQUAL_FATAL(hCwmpUtil_HexBinToStr(NULL, 0, NULL, 0, false), -1);
}


static void test_check_NULL_39(void)
{
	CU_ASSERT_PTR_EQUAL_FATAL(hCwmpUtil_Base64StrToBin(NULL, NULL, 0, NULL), -1);
}
static void test_check_NULL_40(void)
{
	CU_ASSERT_PTR_EQUAL_FATAL(hCwmpUtil_Base64BinToStr(NULL, 0, NULL, 0), -1);
}
static void test_Check_valid_Value_1(void)
{
	int  nRt=-1;
	int  nTemp;
	unsigned int  unTemp;
	char szData[256];
	long long  llTemp; 
	
	int nValue;
	unsigned int unValue;
	char szValue[256];


	for(llTemp = LLONG_MIN ;llTemp < LLONG_MAX; llTemp++)
	{
		
#if !defined(FULL_TEST)
		if(llTemp == LLONG_MIN+3)
			llTemp = (long long)INT32_MIN-3;
		else if(llTemp == (long long)INT32_MIN+3)
			llTemp = (long long)-3;
		else if(llTemp == (long long)3)
			llTemp = (long long)INT32_MAX-3;
		else if(llTemp == (long long)INT32_MAX+3)
			llTemp = (long long)UINT32_MAX-3;
		else if(llTemp == (long long)UINT32_MAX+3)
			llTemp = (long long)LLONG_MAX-3;
#endif
		snprintf(szData, 256, "%" PRId64, llTemp);
		
		///##################hCwmpUtil_StrToINT32
		{
			nRt=-1;
			nTemp =0;
			nRt = hCwmpUtil_StrToINT32(szData, &nTemp);

			//TCU_MSG("#!!szData(%s) %d ",  szData, nTemp); 

			if( llTemp >= INT32_MIN && llTemp <= INT32_MAX)
			{

				if(!TCU_ASSERT_NOT_EQUAL(nRt, -1))
				{
					TCU_MSG("szData(%s) %d %" PRId64 ,  szData, nTemp, llTemp); 
				}
				if(!TCU_ASSERT_EQUAL(nTemp, llTemp))
				{
					TCU_MSG("szData(%s) %d %" PRId64 ,  szData, nTemp, llTemp); 
				}

			}
			else
			{
				if(!TCU_ASSERT_EQUAL(nRt, -1))
				{
					TCU_MSG("szData(%s) %d %"PRId64,  szData, nTemp, llTemp); 
				}
				if(!TCU_ASSERT_NOT_EQUAL((long long)nTemp, llTemp))
				{
					TCU_MSG("szData(%s) %d %"PRId64,  szData, nTemp, llTemp); 
				}
			}

			///##################hCwmpUtil_INT32ToStr
			nValue = (int)llTemp;
			sprintf(szValue,"0");
			nRt=-1;
			if(llTemp == (long long) nValue)
			{
				nRt = hCwmpUtil_INT32ToStr(nValue, szValue, 256);
				//TCU_MSG("#333 nValue(%d) %s %" PRId64 ,  nValue, szValue, llTemp);
#if 1
				if(!TCU_ASSERT_NOT_EQUAL(nRt, -1))
				{
					TCU_MSG("nValue(%d) %s %" PRId64 ,  nValue, szValue, llTemp); 
				}
				if(!TCU_ASSERT_EQUAL(nValue, atol(szValue)))
				{
					TCU_MSG("nValue(%d) %s %" PRId64 ,  nValue, szValue, llTemp); 
				}
#endif
			}
		}
		///##################hCwmpUtil_StrToUINT32
		{
			nRt=-1;
			unTemp =0;
			nRt = hCwmpUtil_StrToUINT32(szData, &unTemp);

			//TCU_MSG("#!!szData(%s) %d ",  szData, nTemp); 

			if( llTemp >= 0 && llTemp <= UINT32_MAX)
			{

				if(!TCU_ASSERT_NOT_EQUAL(nRt, -1))
				{
					TCU_MSG("szData(%s) %u %" PRId64 ,  szData, unTemp, llTemp); 
				}
				if(!TCU_ASSERT_EQUAL(unTemp, llTemp))
				{
					TCU_MSG("szData(%s) %u %" PRId64 ,  szData, unTemp, llTemp); 
				}

			}
			else
			{
				if(!TCU_ASSERT_EQUAL(nRt, -1))
				{
					TCU_MSG("szData(%s) %u %"PRId64,  szData, unTemp, llTemp); 
				}
				if(!TCU_ASSERT_NOT_EQUAL((long long)unTemp, llTemp))
				{
					TCU_MSG("szData(%s) %u %"PRId64,  szData, unTemp, llTemp); 
				}
			}

			///##################hCwmpUtil_UINT32ToStr
			unValue = (unsigned int)llTemp;
			sprintf(szValue,"0");
			nRt=-1;
			if(llTemp == (long long) unValue)
			{
				nRt = hCwmpUtil_UINT32ToStr(unValue, szValue, 256);
				//TCU_MSG("#333 nValue(%d) %s %" PRId64 ,  nValue, szValue, llTemp);
#if 1
				if(!TCU_ASSERT_NOT_EQUAL(nRt, -1))
				{
					TCU_MSG("unValue(%u) %s %" PRId64 ,  unValue, szValue, llTemp); 
				}
				if(!TCU_ASSERT_EQUAL(unValue, strtoul(szValue,NULL,10)))
				{
					TCU_MSG("unValue(%u) %s %" PRId64 ,  unValue, szValue, llTemp); 
				}
#endif
			}
		}

	}
}
static void test_Check_valid_Value_2(void)
{

	 
#define D_MAX_STR_Valid  5
	char szValid[D_MAX_STR_Valid][100]={
		"+900",
		"-0",
		"+0",
		"+2000000000",
		"+2147483647",
	};
	int  nRt=-1;
	int  nTemp;
	int i = 0 ;

	for(i = 0; i < D_MAX_STR_Valid; i++)
	{
		nRt=-1;
		nTemp =0;
		nRt = hCwmpUtil_StrToINT32(szValid[i], &nTemp);
		if(!TCU_ASSERT_NOT_EQUAL(nRt, -1))
		{
			TCU_MSG("szValid(%s) nTemp(%d) nRt(%d)",  szValid[i], nTemp, nRt); 
		}
		if(!TCU_ASSERT_EQUAL(nTemp, atol(szValid[i])))
		{
			TCU_MSG("szValid(%s) nTemp(%d) nRt(%d)",  szValid[i], nTemp, nRt); 
		}
	}
}
static void test_Check_valid_Value_3(void)
{
#define D_MAX_STR_InValid  11
	char szInValid[D_MAX_STR_InValid][100]={
		"adf",
		"3.2",
		"3.2f",
		"0x33",
		"900a",
		"x900",
		"9xxx00",
		"97x00",
		"97-20",
		"97+20",
		"+2147483648",
	};
	
	int  nRt=-1;
	int  nTemp;
	int i = 0 ; 

	for(i = 0; i < D_MAX_STR_InValid; i++)
	{
		nRt=-1;
		nTemp =0;
		nRt = hCwmpUtil_StrToINT32(szInValid[i], &nTemp);
		if(!TCU_ASSERT_EQUAL(nRt, -1))
		{
			TCU_MSG("szInValid(%s) nTemp(%d) nRt(%d)",  szInValid[i], nTemp, nRt); 
		}
		if(!TCU_ASSERT_EQUAL(nTemp, 0))
		{
			TCU_MSG("szInValid(%s) nTemp(%d) nRt(%d)",  szInValid[i], nTemp, nRt); 
		}
	}
}

static void test_Check_valid_Value_4(void)
{

	 
#define D_MAX_STR_Valid_4  7
	char szValid[D_MAX_STR_Valid_4][100]={
		"+900",
		"+0",
		"+2000000000",
		"+2147483647",
		"+2147483648",
		"+4000000000",
		"+4294967295",
	};
	int  nRt=-1;
	unsigned int  unTemp;
	int i = 0 ;

	for(i = 0; i < D_MAX_STR_Valid_4; i++)
	{
		nRt=-1;
		unTemp =0;
		nRt = hCwmpUtil_StrToUINT32(szValid[i], &unTemp);
		if(!TCU_ASSERT_NOT_EQUAL(nRt, -1))
		{
			TCU_MSG("szValid(%s) unTemp(%u) nRt(%d)",  szValid[i], unTemp, nRt); 
		}
		if(!TCU_ASSERT_EQUAL(unTemp, strtoul(szValid[i],NULL,10)))
		{
			TCU_MSG("szValid(%s) unTemp(%u) nRt(%d)",  szValid[i], unTemp, nRt); 
		}
	}
}
static void test_Check_valid_Value_5(void)
{
#define D_MAX_STR_InValid_5  12
	char szInValid[D_MAX_STR_InValid_5][100]={
		"adf",
		"3.2",
		"3.2f",
		"0x33",
		"900a",
		"x900",
		"9xxx00",
		"97x00",
		"97-20",
		"97+20",
		"-0",
		"+4294967296",
		
	};
	
	int  nRt=-1;
	unsigned int  unTemp;
	int i = 0 ; 

	for(i = 0; i < D_MAX_STR_InValid_5; i++)
	{
		nRt=-1;
		unTemp =0;
		nRt = hCwmpUtil_StrToUINT32(szInValid[i], &unTemp);
		if(!TCU_ASSERT_EQUAL(nRt, -1))
		{
			TCU_MSG("szInValid(%s) unTemp(%U) nRt(%d)",  szInValid[i], unTemp, nRt); 
		}
		if(!TCU_ASSERT_EQUAL(unTemp, 0))
		{
			TCU_MSG("szInValid(%s) unTemp(%U) nRt(%d)",  szInValid[i], unTemp, nRt); 
		}
	}
}


static void test_Check_hCwmpUtil_StrToBool(void)
{

	int i=0;
	int  nRt=-1;
	int nMax = 0; 

	bool bValue;
	char szTemp[10];
	vector<string> vs_strValid;
	vector<string>::iterator it;
	vector<string> vs_strInValid;

	vs_strValid.push_back("0");
	vs_strValid.push_back("false");
	vs_strValid.push_back("FALSE");
	vs_strValid.push_back("1");
	vs_strValid.push_back("true");
	vs_strValid.push_back("TRUE");

	vs_strInValid.push_back("-1");
	vs_strInValid.push_back("-0");
	vs_strInValid.push_back("+0");
	vs_strInValid.push_back("+1");
	vs_strInValid.push_back("2");
	vs_strInValid.push_back("0d");
	vs_strInValid.push_back("d0");
	vs_strInValid.push_back("d1");
	vs_strInValid.push_back("1d");
	vs_strInValid.push_back("True");
	vs_strInValid.push_back("fALSE");

	///##################check NULL
	CU_ASSERT_EQUAL_FATAL(hCwmpUtil_StrToBool(NULL, NULL), -1);
	CU_ASSERT_EQUAL_FATAL(hCwmpUtil_StrToBool(NULL, &bValue), -1);
	CU_ASSERT_EQUAL_FATAL(hCwmpUtil_StrToBool(szTemp,NULL), -1);
	
	
	///##################check valid
	//for(it = vs_strValid.begin(); it != vs_strValid.end() ; it++);
	nMax = vs_strValid.size(); 
	for(i=0 ; i< nMax ; i++)
	{
		nRt=-1;
		bValue =0;
		// STB ?¥ÏÉÅ ?ôÏûë.. ?úÏ?... ??
		//TCU_MSG("inx(%d) nMax(%d), %s %s %s", i, nMax, strTemp.c_str(), (*it).c_str(), vs_strValid[3].c_str());
		//strTemp  = *it;
		//TCU_MSG("szValid(%s) bTemp(%d) nRt(%d) inx(%d)",  vs_strValid[i].c_str(), bTemp, nRt, i); 
		nRt = hCwmpUtil_StrToBool(vs_strValid[i].c_str(), &bValue);
		if(!TCU_ASSERT_NOT_EQUAL(nRt, -1))
		{
			TCU_MSG("szValid(%s) bTemp(%d) nRt(%d) inx(%d)",  vs_strValid[i].c_str(), bValue, nRt, i); 
		}
		if(!TCU_ASSERT_EQUAL(bValue,  i >= 3))
		{
			TCU_MSG("szValid(%s) bTemp(%d) nRt(%d) inx(%d)",  vs_strValid[i].c_str(), bValue, nRt, i); 
		}
	}

	///##################check invalid
	nMax = vs_strInValid.size(); 
	for(i=0 ; i< nMax ; i++)
	{
		nRt=-1;
		bValue =0;
		// STB ?¥ÏÉÅ ?ôÏûë.. ?úÏ?... ??
		//TCU_MSG("inx(%d) nMax(%d), %s %s %s", i, nMax, strTemp.c_str(), (*it).c_str(), vs_strValid[3].c_str());
		//strTemp  = *it;
		//TCU_MSG("szValid(%s) bTemp(%d) nRt(%d) inx(%d)",  vs_strValid[i].c_str(), bTemp, nRt, i); 
		nRt = hCwmpUtil_StrToBool(vs_strInValid[i].c_str(), &bValue);
		if(!TCU_ASSERT_EQUAL(nRt, -1))
		{
			TCU_MSG("vs_strInValid(%s) bTemp(%d) nRt(%d) inx(%d)",  vs_strInValid[i].c_str(), bValue, nRt, i); 
		}
	}


}



static void test_check_hCwmpUtil_BoolToStr(void)
{
	

	int i=0;
	int  nRt=-1;
	int nMax = 0; 

	bool bValue;
	char szTemp_1[1];
	char szTemp_2[2];
	char szTemp_3[3];
	char szTemp_4[4];
	char szTemp_5[5];
	char szTemp_6[6];
	char szTemp_7[7];
	char szTemp[10];
	const char szRefSTR[10]="strnn";

	vector<cNULLCheck_STR_Bool> vstNULLCheck;
	cNULLCheck_STR_Bool *pstData;

	
	vstNULLCheck.push_back(cNULLCheck_STR_Bool(0, 0, GetBufName3(NULL)));
	vstNULLCheck.push_back(cNULLCheck_STR_Bool(0, 0, GetBufName3(szTemp_1)));
	vstNULLCheck.push_back(cNULLCheck_STR_Bool(0, 0, GetBufName3(szTemp_2)));
	vstNULLCheck.push_back(cNULLCheck_STR_Bool(0, 0, GetBufName3(szTemp_3)));
	vstNULLCheck.push_back(cNULLCheck_STR_Bool(0, 0, GetBufName3(szTemp_4)));
	vstNULLCheck.push_back(cNULLCheck_STR_Bool(0, 0, GetBufName3(szTemp_5)));
	vstNULLCheck.push_back(cNULLCheck_STR_Bool(0, 1, GetBufName3(szTemp_6)));
	vstNULLCheck.push_back(cNULLCheck_STR_Bool(0, 1, GetBufName3(szTemp_7)));
	vstNULLCheck.push_back(cNULLCheck_STR_Bool(1, 0, GetBufName3(NULL)));
	vstNULLCheck.push_back(cNULLCheck_STR_Bool(1, 0, GetBufName3(szTemp_1)));
	vstNULLCheck.push_back(cNULLCheck_STR_Bool(1, 0, GetBufName3(szTemp_2)));
	vstNULLCheck.push_back(cNULLCheck_STR_Bool(1, 0, GetBufName3(szTemp_3)));
	vstNULLCheck.push_back(cNULLCheck_STR_Bool(1, 0, GetBufName3(szTemp_4)));
	vstNULLCheck.push_back(cNULLCheck_STR_Bool(1, 1, GetBufName3(szTemp_5)));
	vstNULLCheck.push_back(cNULLCheck_STR_Bool(1, 1, GetBufName3(szTemp_6)));
	vstNULLCheck.push_back(cNULLCheck_STR_Bool(1, 1, GetBufName3(szTemp_7)));

	///##################check invalid
	nMax = vstNULLCheck.size(); 
	for(i=0 ; i< nMax ; i++)
	{
		pstData = &vstNULLCheck[i];

		if(pstData->m_pBuf)
			snprintf(pstData->m_pBuf, pstData->m_nBufLen, "%s",szRefSTR);

		nRt = hCwmpUtil_BoolToStr(pstData->m_bValue, pstData->m_pBuf, pstData->m_nBufLen);
		if(pstData->m_nSuccess == 0)
		{
			if(!TCU_ASSERT_EQUAL(nRt, -1))
			{
				TCU_MSG("m_bValue(%d) m_pBuf(%s)(N:%s) m_nBufLen(%d) inx(%d)", 
					pstData->m_bValue, pstData->m_pBuf, pstData->m_strBufName.c_str(),
					pstData->m_nBufLen, i); 
			}
			if(pstData->m_pBuf && !TCU_ASSERT_STRING_INCLUDE(szRefSTR, pstData->m_pBuf))
			{
				TCU_MSG("m_bValue(%d) m_pBuf(%s)(N:%s) m_nBufLen(%d) inx(%d)", 
					pstData->m_bValue, pstData->m_pBuf, pstData->m_strBufName.c_str(),
					pstData->m_nBufLen, i); 
			}
		}
		else
		{
			if(!TCU_ASSERT_NOT_EQUAL(nRt, -1))
			{
				TCU_MSG("m_bValue(%d) m_pBuf(%s)(N:%s) m_nBufLen(%d) inx(%d)", 
					pstData->m_bValue, pstData->m_pBuf, pstData->m_strBufName.c_str(),
					pstData->m_nBufLen, i); 
			}
			if(pstData->m_bValue && !TCU_ASSERT_STRING_EQUAL(pstData->m_pBuf, "true"))
			{
				TCU_MSG("m_bValue(%d) m_pBuf(%s)(N:%s) m_nBufLen(%d) inx(%d)", 
					pstData->m_bValue, pstData->m_pBuf, pstData->m_strBufName.c_str(),
					pstData->m_nBufLen, i); 
			}
			else if(!pstData->m_bValue && !TCU_ASSERT_STRING_EQUAL(pstData->m_pBuf, "false"))
			{
				TCU_MSG("m_bValue(%d) m_pBuf(%s)(N:%s) m_nBufLen(%d) inx(%d)", 
					pstData->m_bValue, pstData->m_pBuf, pstData->m_strBufName.c_str(),
					pstData->m_nBufLen, i); 
			}
		}
	}
}

static void test_check_hCwmpUtil_StrToDateTime(void)
{
#if 0
	int i=0;
	int  nRt=-1;
	int nMax = 0; 
	char szTemp[256];
	hCwmpDateTime_t stDateTime;
	vector<string> vs_strValid;
	vector<string> vs_strInValid;

	///##################check NULL
	CU_ASSERT_EQUAL_FATAL(hCwmpUtil_StrToDateTime(NULL, NULL), -1);
	CU_ASSERT_EQUAL_FATAL(hCwmpUtil_StrToDateTime(NULL, &stDateTime), -1);
	CU_ASSERT_EQUAL_FATAL(hCwmpUtil_StrToDateTime(szTemp, NULL), -1);

	///##################check Valid
	vs_strValid.push_back("");

	nMax = vs_strValid.size(); 
	for(i=0 ; i< nMax ; i++)
	{
		nRt=-1;
		bValue =0;
		nRt = hCwmpUtil_StrToDateTime(vs_strValid[i].c_str(), &stDateTime);
		if(!TCU_ASSERT_NOT_EQUAL(nRt, -1))
		{
			TCU_MSG("szValid(%s) bTemp(%d) nRt(%d) inx(%d)",  vs_strValid[i].c_str(), bValue, nRt, i); 
		}
		/*
		if(!TCU_ASSERT_EQUAL(bValue,  i >= 3))
		{
			TCU_MSG("szValid(%s) bTemp(%d) nRt(%d) inx(%d)",  vs_strValid[i].c_str(), bValue, nRt, i); 
		}
		*/
	}
#endif

}
#define ___INTERNAL_FUNCTIONS___

#define ___PUBLIC_FUNCTIONS___

 int test_hcwmp_engine_register(void)
{ 
	int nRS = -1;

	CU_pSuite pSuite = NULL;
	CU_pTest  pTest = NULL;
	pSuite = CU_add_suite("hcwmp_engine_Check_NULL", NULL, NULL);
	if (NULL == pSuite) 
		return nRS;

	TCU_add_test(pTest, pSuite, test_check_NULL_1);
	TCU_add_test(pTest, pSuite, test_check_NULL_2);
	TCU_add_test(pTest, pSuite, test_check_NULL_3);
	TCU_add_test(pTest, pSuite, test_check_NULL_4);
	TCU_add_test(pTest, pSuite, test_check_NULL_5);
	TCU_add_test(pTest, pSuite, test_check_NULL_6);
	TCU_add_test(pTest, pSuite, test_check_NULL_7);
	TCU_add_test(pTest, pSuite, test_check_NULL_8);
	TCU_add_test(pTest, pSuite, test_check_NULL_9);
	TCU_add_test(pTest, pSuite, test_check_NULL_10);
	TCU_add_test(pTest, pSuite, test_check_NULL_11);
	TCU_add_test(pTest, pSuite, test_check_NULL_12);
	TCU_add_test(pTest, pSuite, test_check_NULL_13);
	TCU_add_test(pTest, pSuite, test_check_NULL_14);
	TCU_add_test(pTest, pSuite, test_check_NULL_15);
	TCU_add_test(pTest, pSuite, test_check_NULL_16);
	TCU_add_test(pTest, pSuite, test_check_NULL_17);
	TCU_add_test(pTest, pSuite, test_check_NULL_18);
	TCU_add_test(pTest, pSuite, test_check_NULL_19);
	TCU_add_test(pTest, pSuite, test_check_NULL_20);
	TCU_add_test(pTest, pSuite, test_check_NULL_21);
	TCU_add_test(pTest, pSuite, test_check_NULL_22);
	TCU_add_test(pTest, pSuite, test_check_NULL_23);
	TCU_add_test(pTest, pSuite, test_check_NULL_24);
	//TCU_add_test(pTest, pSuite, test_check_NULL_25);
	//TCU_add_test(pTest, pSuite, test_check_NULL_26);
	//TCU_add_test(pTest, pSuite, test_check_NULL_27);
	//TCU_add_test(pTest, pSuite, test_check_NULL_28);
	//TCU_add_test(pTest, pSuite, test_check_NULL_29);
	//TCU_add_test(pTest, pSuite, test_check_NULL_30);
	
	

	TCU_add_test(pTest, pSuite, test_check_NULL_34);
	TCU_add_test(pTest, pSuite, test_check_NULL_35);
	TCU_add_test(pTest, pSuite, test_check_NULL_36);
	TCU_add_test(pTest, pSuite, test_check_NULL_37);
	TCU_add_test(pTest, pSuite, test_check_NULL_38);
	TCU_add_test(pTest, pSuite, test_check_NULL_39);
	TCU_add_test(pTest, pSuite, test_check_NULL_40);

	/*
	pTest = CU_add_test(pSuite, "test_check_NULL_1", test_check_NULL_1);
	if (NULL == pTest) 
		return nRS;
	pTest = CU_add_test(pSuite, "test_check_NULL_2", test_check_NULL_2);
	if (NULL == pTest) 
		return nRS;
	*/

	pSuite = CU_add_suite("hcwmp_engine_Check_valid_Value", NULL, NULL);
	if (NULL == pSuite) 
		return nRS;
	TCU_add_test(pTest, pSuite, test_Check_valid_Value_1);
	TCU_add_test(pTest, pSuite, test_Check_valid_Value_2);
	TCU_add_test(pTest, pSuite, test_Check_valid_Value_3);
	TCU_add_test(pTest, pSuite, test_Check_valid_Value_4);
	TCU_add_test(pTest, pSuite, test_Check_valid_Value_5);

	TCU_add_test(pTest, pSuite, test_Check_hCwmpUtil_StrToBool);
	TCU_add_test(pTest, pSuite, test_check_hCwmpUtil_BoolToStr);

	TCU_add_test(pTest, pSuite, test_check_hCwmpUtil_StrToDateTime);


	nRS = 0;
	return nRS;

}
#define ___EXTERNAL_FUNCTIONS___

#define _____END_OF_FILE_________


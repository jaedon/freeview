/* $Header:   //BASE/archives/STING_T/mw/cas/ci/ci_res/ci_lsc.c-arc   1.0   17 Dec 2004 10:32:30   jhkim1  $ */
/***************************************************************
* $Workfile:   ci_lsc.c  $
* $Modtime:   13 Dec 2004 09:27:30  $
* Auther : Jaehee Cho
*
* Desc :
*
* Copyright (c) 2002 by Humax Co., Ltd.
* All right reserved
****************************************************************/

/***************************************************************
* #ifdef statements and include
***************************************************************/
#include "ci.h"
#include "cipack_msg.h"
#include "ci_reshandler.h"
#include "ci_lsc.h"


/***************************************************************
* definition
***************************************************************/



/***************************************************************
* static variables and structures
***************************************************************/


/***************************************************************
* static function prototypes
***************************************************************/


/***************************************************************
* function bodies
***************************************************************/
int CiLsc_Init(void)
{
	return ERR_OK;
}

int CiLsc_SsOpenedProc(unsigned short usSsNo)
{
	PrintDbg(("[CILSC]---> ci_LscSsOpenedProc()---> SsNo[%d]...\n", usSsNo));
	return ERR_OK;
}

int CiLsc_SsClosedProc(unsigned short usSsNo)
{
	PrintDbg(("[CILSC]---> ci_LscSsClosedProc()---> SsNo[%d]...\n", usSsNo));
	return ERR_OK;
}

/* 현재 이 interface 는 지원하지 않음.. parkbi.. 020805 */
int CiLsc_ApduProc(unsigned short usSsNo, unsigned long ulApduTag,
					unsigned char *pucApduData, unsigned long ulApduLen)
{
	PrintDbg(("[CILSC]---> ci_LscApduProc()---> SsNo[%d], tag[0x%lx]...\n", usSsNo, ulApduTag));
	return ERR_OK;
}


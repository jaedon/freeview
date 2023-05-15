/***************************************************************
* $Workfile:   ci_auth.c  $
* $Modtime:   Aug 21 2004 07:31:20  $
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
#include "ci_auth.h"

#ifdef _INCLUDE_BD_CI_
	#include "bd_ci.h"
#endif


/***************************************************************
* definition
***************************************************************/
//#define	_CI_AUTH_PRIVATE_DATA_VIEW_


/***************************************************************
* static variables and structures
***************************************************************/


/***************************************************************
* static function prototypes
***************************************************************/


/***************************************************************
* function bodies
***************************************************************/
int CiAuth_Init(void)
{
	int	nRet;

#ifdef _INCLUDE_BD_CI_
	nRet = BD_CiAuth_Init();
	if (nRet != ERR_OK)
	{
		PrintErr(("[CIAUTH]---> CiAuth_Init()---> Error\n"));
		return nRet;
	}
#endif	//_INCLUDE_BD_CI_

	return ERR_OK;
}

int CiAuth_SsOpenedProc(unsigned short usSsNo)
{
	PrintDbg(("[CIAUTH]---> CiAuth_SsOpenedProc() SsNo[%d]...\n", usSsNo));

	return ERR_OK;
}

int CiAuth_SsClosedProc(unsigned short usSsNo)
{
	PrintDbg(("[CIAUTH]---> CiAuth_SsClosedProc() SsNo[%d]...\n", usSsNo));

	return ERR_OK;
}

int CiAuth_ApduProc(unsigned short usSsNo, unsigned long ulApduTag,
						unsigned char *pucApduData, unsigned long ulApduLen)
{
	unsigned short	usAuthProtocolId;

	PrintDbg(("[CIAUTH]---> CiAuth_ApduProc() SsNo[%d], tag[0x%x]...\n", usSsNo, ulApduTag));

	if (pucApduData == NULL || ulApduLen < 2)
	{
		PrintErr(("[CIAUTH]---> CiAuth_ApduProc()---> Error : Wrong ulApduLen[%ld] !!!\n", ulApduLen));
		return -1;
	}

#ifdef _CI_AUTH_PRIVATE_DATA_VIEW_
	{
		int	i;
		PrintDbg(("===CI_AUTH_REQ===\n"));
		for (i=0; i<ulApduLen; i++)
		{
			PrintDbg(("0x%02x ", pucApduData[i]));
			if ((i % 10) == 9)
				PrintDbg(("\n"));
		}
		PrintDbg(("\n===============\n"));
	}
#endif	//_CI_AUTH_PRIVATE_DATA_VIEW_

	usAuthProtocolId = ((unsigned short)pucApduData[0] << 8) & 0xff00;
	usAuthProtocolId |= ((unsigned short)pucApduData[1] & 0x00ff);

	switch (ulApduTag)
	{
	case CI_AUTH_REQ:
		if (usAuthProtocolId == CI_AUTH_BD_PROTOCOL_ID)
		{
#ifdef _INCLUDE_BD_CI_
			BD_CiAuth_ProcReq(usSsNo, ulApduTag, pucApduData, ulApduLen);
#endif	//_INCLUDE_BD_CI_
		}
		break;
	default:
		return -1;
	}

	return ERR_OK;
}

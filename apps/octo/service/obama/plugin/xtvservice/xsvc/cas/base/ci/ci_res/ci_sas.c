/***************************************************************
* $Workfile:   ci_sas.c  $
* $Modtime:
* Auther : hmkim
*
* Desc : Specific Application Support for MHP (CI+)
*
* Copyright (c) 2009 by Humax Co., Ltd.
* All right reserved
****************************************************************/

/***************************************************************
* #ifdef statements and include
***************************************************************/
#include <pal_ci.h>
#include "ci_plus.h"

/***************************************************************
* local definition
***************************************************************/




/***************************************************************
* typedef
***************************************************************/


/***************************************************************
* external variables and structures
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
int CiSas_Init(void)
{
	return ERR_OK;
}

int CiSas_SsOpenedProc(unsigned short usSsNo)
{
	return ERR_OK;
}

int CiSas_SsClosedProc(unsigned short usSsNo)
{
	return ERR_OK;
}

int CiSas_ApduProc(unsigned short usSsNo, unsigned long ulApduTag, unsigned char *pucApduData, unsigned long ulApduLen)
{
	int nRtnCode = ERR_OK;

	switch (ulApduTag)
	{
	case CI_SAS_CONNECT_CNF:
		HxLOG_Print("CI_SAS_CONNECT_CNF received (len=%ld)\n", ulApduLen);
		break;

	case CI_SAS_DATA_RQST:
		HxLOG_Print("CI_SAS_DATA_RQST received (len=%ld)\n", ulApduLen);
		break;

	case CI_SAS_DATA_AV:
		HxLOG_Print("CI_SAS_DATA_AV received (len=%ld)\n", ulApduLen);
		break;

	case CI_SAS_DATA_CNF:
		HxLOG_Print("CI_SAS_DATA_CNF received (len=%ld)\n", ulApduLen);
		break;

	case CI_SAS_SERVER_QUERY:
		HxLOG_Print("CI_SAS_SERVER_QUERY received (len=%ld)\n", ulApduLen);
		break;

	case CI_SAS_SERVER_REPLY:
		HxLOG_Print("CI_SAS_SERVER_REPLY received (len=%ld)\n", ulApduLen);
		break;

	case CI_SAS_ASYNC_MSG:
		HxLOG_Print("CI_SAS_ASYNC_MSG received (len=%ld)\n", ulApduLen);
		break;

	default:
		nRtnCode = -1;
		HxLOG_Error("[CISAS]---> CiSas_ApduProc()---> Error : Unknown Apdu Tag[0x%lx] !!!\n", ulApduTag);
	}

	HxLOG_Print(("[CISAS]---> CiSas_ApduProc()---> SsNo[%d], tag[0x%lx], len[%ld]---> %d\n", usSsNo, ulApduTag, ulApduLen, nRtnCode));

	return nRtnCode;
}


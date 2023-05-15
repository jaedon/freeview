/******************************************************************************
* $Workfile:$
* $Modtime:$
* Author : 
* description :		 Factory Application
*
* Copyright (c) 2008 by Humax Co., Ltd.
* All right reserved
******************************************************************************/

/*******************************************************************************
* header and definitions
*******************************************************************************/
/* global header files */
#include <string.h>

/* chip manufacture's  header files */

/* humax header files */
#include "fvk.h"
#include "fai_common.h"
#include "fai_testmenu.h"
#include "fai_tmd.h"

#include "fdi_ethernet.h"
#include "fdi_panel.h"

/* model specific configuration header */
#include "factorysw_config.h"

/*******************************************************************************
* Debug level
*******************************************************************************/

/*******************************************************************************
* Definitions, typdef & const data
*******************************************************************************/
#define IS_VALID_ADDR_IPV4(addr_ipv4) 	FVK_MEM_Compare(s_aucIpv4Null, addr_ipv4, FDI_IPV4_ADDR_LEN)
#define ETHERNET_DATA_SIZE		6

/*******************************************************************************
* External variables and functions
*******************************************************************************/
/* external variables  and functions */

/*******************************************************************************
* Global variables and structures
*******************************************************************************/
/* global variables and structures */
static unsigned char s_aucIpv4Null[FDI_IPV4_ADDR_LEN] = {0, };

#if (TMD_ETHERNET_TEST_LOCAL_PC==YES)
//PC에서 테스트 
unsigned char g_aucIpAddr[4] = {192, 168, 0, 29};
unsigned char g_aucGwAddr[4] = {192, 168, 0, 1};
unsigned char g_aucNetAddr[4] = {255, 255, 255, 0};
unsigned char g_aucMacAddr[6] = {0x00, 0x01, 0x02, 0x03, 0x00, 0x60};

#undef TMD_ETHERNET_SERVER_IP
#define TMD_ETHERNET_SERVER_IP "192.168.0.11"

#if(ETHERNET_UI_ENABLE==YES)
#undef ETHERNET_UI_SERVER_IP
#define ETHERNET_UI_SERVER_IP TMD_ETHERNET_SERVER_IP
#endif

#else
//생산 적용 
unsigned char g_aucIpAddr[4] = {10, 0, 10, 116};
unsigned char g_aucGwAddr[4] = {10, 0, 10, 1};
unsigned char g_aucNetAddr[4] = {255, 255, 255, 0};
unsigned char g_aucMacAddr[6] = {0x00, 0x01, 0x02, 0x03, 0x00, 0x60};
#endif
FAI_ERR_CODE INTRN_FAI_ETHERNET_ARPTest(char *szIpAddr, unsigned char* pucMacAddr, int waitSecond);


/*******************************************************************************
* function : 
* description : 
* input :
* output :
* return :
* 기타 참고자료 및 파일
*******************************************************************************/
FAI_ERR_CODE FAI_ETHERNET_Init(void)
{
	unsigned long ulCount = 0, i;
	FDI_ETHERNET_INFO_t stEthInfo;
	
	FDI_ETHERNET_GetDevCount(&ulCount);

	for(i=0; i<ulCount; i++)
	{
		FVK_MEM_Set(&stEthInfo, 0, sizeof(FDI_ETHERNET_INFO_t));
		FDI_ETHERNET_GetInfo(i, &stEthInfo);
		if(stEthInfo.eDevType!=FDI_ETHERNET_DEV_WIFI && !IS_VALID_ADDR_IPV4(stEthInfo.stIpv4Addr.aucIpAddr))
		{
			FVK_MEM_Copy(stEthInfo.stIpv4Addr.aucIpAddr, g_aucIpAddr, FDI_IPV4_ADDR_LEN);
			stEthInfo.stIpv4Addr.aucIpAddr[3] += i;
			FVK_MEM_Copy(stEthInfo.stIpv4Addr.aucMaskAddr, g_aucNetAddr, FDI_IPV4_ADDR_LEN);
			FVK_MEM_Copy(stEthInfo.aucGwAddr, g_aucGwAddr, FDI_IPV4_ADDR_LEN);
			FDI_ETHERNET_SetInfo(i, &stEthInfo);
		}
	}
	return FAI_NO_ERR;
}

/*******************************************************************************
* function : 
* description : 
* input :
* output :
* return :
* 기타 참고자료 및 파일
*******************************************************************************/
#if (ETHERNET_ARP_CHECK_ENABLE==YES)
FAI_ERR_CODE FAI_ETHERNET_ARPCheck(unsigned short usMsgId,unsigned char ucArgc, unsigned char *pucArgList)
{
	unsigned char aucRcvMacAddr[6];
	unsigned char aucResult[13];
	FAI_TMD_RESULT_T tTMDResult;
	unsigned char ucTmdErrCode[2];
	unsigned char ucDevId;
	unsigned char aucIpAddr[4];
	unsigned char aucIpAddrStr[20];
	FVK_MEM_Set(&tTMDResult, 0x00, sizeof(tTMDResult));

	if( ucArgc != 2 )
	{
		FAI_PRINT(FAI_PRT_ERROR, ("[%s] : Argument Count Error=%d\n",  __func__, ucArgc));
		tTMDResult.eResult = FAI_TMD_ERR_FAIL;
		ucTmdErrCode[0] = FAI_MODULE_TMD_ERR_COMMON;
		ucTmdErrCode[1] = FAI_COMMON_TMD_ERR_INVALID_ARGUMENT;
		FAI_TMD_AddResultParam(&tTMDResult, &ucTmdErrCode[0], sizeof(ucTmdErrCode));
		FAI_TMD_SetResult(&tTMDResult);
		return FAI_ERR;
	}

	FAI_TMD_GetParam(&ucDevId, pucArgList, 0);
	FAI_TMD_GetParam(aucIpAddr, pucArgList, 1);
	sprintf(aucIpAddrStr, "%d.%d.%d.%d", aucIpAddr[0], aucIpAddr[1], aucIpAddr[2], aucIpAddr[3]);
	
	if( FDI_ETHERNET_ARPTest(ucDevId, aucIpAddrStr, aucRcvMacAddr, 10) != FDI_NO_ERR )
	{
		FAI_PRINT(FAI_PRT_ERROR, ("[FAI_ETHERNET_ARPCheck] : FAI_ETHERNET_ARPCheck Error\n"));
		tTMDResult.eResult = FAI_TMD_ERR_FAIL;
		ucTmdErrCode[0] = FAI_MODULE_TMD_ERR_COMMON;
		ucTmdErrCode[1] = FAI_COMMON_TMD_ERR_FAIL;
		FAI_TMD_AddResultParam(&tTMDResult, &ucTmdErrCode[0], sizeof(ucTmdErrCode));	

		goto FAIL;
	}

	sprintf(&aucResult[1], "%02x%02x%02x%02x%02x%02x", aucRcvMacAddr[0],aucRcvMacAddr[1],aucRcvMacAddr[2],aucRcvMacAddr[3],aucRcvMacAddr[4],aucRcvMacAddr[5]);
	aucResult[0] = strlen(&aucResult[1]);					/* length of param */						

	tTMDResult.eResult = FAI_TMD_ERR_OK;
	FAI_TMD_AddResultParam(&tTMDResult, &aucResult[1], aucResult[0]);
	FAI_TMD_SetResult(&tTMDResult);
	return FAI_NO_ERR;

FAIL:	
	tTMDResult.eResult = FAI_TMD_ERR_FAIL;
	FAI_TMD_SetResult(&tTMDResult);
	return FAI_ERR;
}
#endif

/*******************************************************************************
* function : 
* description : 
* input :
* output :
* return :
* 기타 참고자료 및 파일
*******************************************************************************/
#if (ETHERNET_UI_ENABLE==YES)
FAI_ERR_CODE FAI_ETHERNET_Test_UI(void)
{
	unsigned char ucTxBuf[ETHERNET_DATA_SIZE] = {0x01, 0x02, 0x03, 0x04, 0x05, 0x06};	/* sample data */
	unsigned char ucRxBuf[ETHERNET_DATA_SIZE];
	unsigned long ulHandle;
	int i;
	unsigned char aucRcvMacAddr[6];
	unsigned char ucDevId = 0;
	
	/* Display ethernet test start */
	FAI_TESTMENU_ChangeCurrentMenuColor(RGB_COLOR_YELLOW);
	FAI_TESTMENU_ChangeCurrentMenu(TESTMENU_LINE_2, "Ethernet Test", RGB_COLOR_YELLOW);
	FAI_TESTMENU_ChangeCurrentRCUColor(RGB_COLOR_BLACK);
	FVK_TASK_Sleep(500);
	
#if defined(ETHERNET_UI_SCENARIO) && (ETHERNET_UI_SCENARIO==ETHERNET_CHECK_PING)
	/* legacy scenario : do not use! */
#endif	

#if defined(ETHERNET_UI_SCENARIO) && (ETHERNET_UI_SCENARIO==ETHERNET_CHECK_ARP)
	if( FDI_ETHERNET_ARPTest(ucDevId, ETHERNET_UI_SERVER_IP, aucRcvMacAddr, 10) != FDI_NO_ERR )
	{
		FAI_PRINT(FAI_PRT_ERROR, ("[FAI_ETHERNET_Test_UI] : FDI_ETHERNET_ARPTest Error\n"));
		goto fail;
	}
#endif	
	/* If no error */
	FAI_TESTMENU_ChangeCurrentMenuColor(RGB_COLOR_BLACK);
	FAI_TESTMENU_ChangeCurrentMenu(TESTMENU_LINE_2, "Ethernet Test", RGB_COLOR_BLACK);
	FAI_TESTMENU_ChangeCurrentMenu(TESTMENU_LINE_3, "Pass", RGB_COLOR_BLACK);		
	FDI_PANEL_Display(FAI_ETHERNET_TEST_PASS_STR);
	return FAI_NO_ERR;
	/* error */
fail:
	FAI_TESTMENU_ChangeCurrentMenuColor(RGB_COLOR_RED);
	FAI_TESTMENU_ChangeCurrentMenu(TESTMENU_LINE_2, "Ethernet Test", RGB_COLOR_RED);
	FAI_TESTMENU_ChangeCurrentMenu(TESTMENU_LINE_3, "Fail", RGB_COLOR_RED);
	FDI_PANEL_Display(FAI_ETHERNET_TEST_FAIL_STR);

	return FAI_ERR;
}
#endif


/*******************************************************************************
* function : 
* description : 
* input :
* output :
* return :
* 기타 참고자료 및 파일
*******************************************************************************/
FAI_ERR_CODE FAI_ETHERNET_LinkSpeed(unsigned char ucArgc, unsigned char *pucArgList, unsigned int *tempspeed)
{
	unsigned char aucResult[12];
	FILE * fp;
	int ret;	
	char buff[128], cmd[32];
	int linkspeed = 0;
	int linkup = 0;

	fp = fopen("/sys/class/net/eth0/operstate", "rt");
	if (!fp)
	{
		FAI_PRINT(FAI_PRT_ERROR, ("[FAI_ETHERNET_LinkSpeed] : Can't create file!! \n"));
		goto fail;
	}	

	while(1)
	{
		ret = fgets(buff, sizeof(buff), fp);
		if( ret == NULL )
			break;

		memset(cmd, 0, sizeof(cmd));
		sscanf(buff, "%s", cmd);

		if(strncmp(cmd, "up", 2) == 0)
		{
			linkup = 1;
		}
		else
		{
			linkup = 0;
		}
	}

	fclose(fp);

	if(linkup)
	{
		fp = fopen("/sys/class/net/eth0/speed", "rt");
		if (!fp)
		{
			FAI_PRINT(FAI_PRT_ERROR, ("[FAI_ETHERNET_LinkSpeed] : Can't create file!! \n"));			
			goto fail;
		}	

		while(1)
		{
			ret = fgets(buff, sizeof(buff), fp);
			if( ret == NULL )
				break;

			memset(cmd, 0, sizeof(cmd));
			sscanf(buff, "%s", cmd);

			linkspeed = atoi(cmd);
		}

		fclose(fp);

		switch(linkspeed)
		{
			case 10:
				aucResult[1] = 0x30;
				aucResult[2] = 0x31;
				break;
			case 100:
				aucResult[1] = 0x30;
				aucResult[2] = 0x32;
				break;
			case 1000:
				aucResult[1] = 0x30;
				aucResult[2] = 0x33;
				break;
			default:
				aucResult[1] = 0x30;
				aucResult[2] = 0x30;
				break;
		}
	}
	else
	{
		aucResult[1] = 0x30;
		aucResult[2] = 0x30;		
	}	

	aucResult[0] = 0x02;
	*tempspeed = linkspeed;
	FAI_PRINT(FAI_PRT_ERROR, ("linkup = %d, linkspeed = %d\n", linkup, linkspeed));
	
	FAI_TMD_SetCurrentResult(0, FAI_TMD_TEST_NO_ERROR, 1, aucResult, 3);

fail:
	aucResult[0] = 0x02;
	aucResult[1] = 0x30;
	aucResult[2] = 0x30;
	FAI_TMD_SetCurrentResult(0, FAI_TMD_TEST_NO_ERROR, 1, aucResult, 3);

	return FAI_NO_ERR;
}


#if (ETHERNET_SPEED_ENABLE==YES)
FAI_ERR_CODE FAI_ETHERNET_Speed_Test(unsigned short usMsgId,unsigned char ucArgc, unsigned char *pucArgList)
{
	unsigned long ulHandle;
	int i, temp_speed;
	unsigned char temp_link, buff[10];
	unsigned char ucDevId = 0;
	FAI_TMD_RESULT_T tTMDResult;
	unsigned char ucPortID = 0;
	unsigned char ucTmdErrCode[2];
	unsigned char aucParamType =0;  /* 0 - Interger, 1- string */


	FVK_MEM_Set( (void *)&tTMDResult, 0x00, sizeof(tTMDResult));
	if( ucArgc != 1 )
	{
		FAI_PRINT(FAI_PRT_ERROR, ("[FAI_ETHERNET_Speed_Test] : Argument Count Error=%d\n", ucArgc));

		tTMDResult.ucCategory = CATEGORY_PERIPHERAL;
		tTMDResult.ucModule = MODULE_PERIPHERAL_ETHERNET;
		tTMDResult.ucAction = 0x02;
		tTMDResult.eResult = FAI_TMD_ERR_FAIL;
		ucTmdErrCode[0] = FAI_MODULE_TMD_ERR_COMMON;
		ucTmdErrCode[1] = FAI_COMMON_TMD_ERR_INVALID_PARAMETER;
		FAI_TMD_AddResultParam(&tTMDResult, &ucTmdErrCode[0], sizeof(ucTmdErrCode));

		FAI_TMD_SetResult(&tTMDResult);
		return FAI_ERR;
	}
	
	FAI_TMD_GetParam(&ucPortID, pucArgList, 0);
	#if (PROJECT_YOUVIEW || PRODUCT_HGS1000S == YES)
	if( ucPortID != 0x00 )
	{
		FAI_PRINT(FAI_PRT_ERROR, ("[FAI_ETHERNET_Speed_Test] : ucPortID Error=%d\n", ucPortID));

		tTMDResult.ucCategory = CATEGORY_PERIPHERAL;
		tTMDResult.ucModule = MODULE_PERIPHERAL_ETHERNET;
		tTMDResult.ucAction = 0x02;
		tTMDResult.eResult = FAI_TMD_ERR_FAIL;
		ucTmdErrCode[0] = FAI_MODULE_TMD_ERR_COMMON;
		ucTmdErrCode[1] = FAI_COMMON_TMD_ERR_INVALID_PARAMETER;
		FAI_TMD_AddResultParam(&tTMDResult, &ucTmdErrCode[0], sizeof(ucTmdErrCode));

		FAI_TMD_SetResult(&tTMDResult);
		return FAI_ERR;
	}
	#endif

	FAI_ETHERNET_LinkSpeed(&temp_link, buff, &temp_speed );
	//if( temp_link )
	//{
	//	memset( buff, 0, sizeof(buff));
	//	snprintf(buff, 10, "(%d)bps\0", temp_speed);
	//}
	FAI_PRINT(FAI_PRT_ERROR, ("[FAI_ETHERNET_Speed_Test] : %x\n", buff[0], buff[1], buff[2]));

	/* Pass 인 경우 */
  tTMDResult.ucCategory = CATEGORY_PERIPHERAL;
  tTMDResult.ucModule = MODULE_PERIPHERAL_ETHERNET;
  tTMDResult.ucAction =  0x02;
  tTMDResult.eResult = FAI_TMD_ERR_OK;
  
  aucParamType = 1;  /* 0 interger, 1 string */
  //DI_UART_Dump(&aucResult[2], aucResult[1],16);
  FAI_TMD_AddResultParam(&tTMDResult, &aucParamType, sizeof(aucParamType));
	FAI_TMD_AddResultParam(&tTMDResult, &buff[1], buff[0]); /* aucResult[1] 은 aucRestul[2] 부터의 size */
  FAI_TMD_SetResult(&tTMDResult);
                   	
	return FAI_NO_ERR;
	/* error */
fail:
		tTMDResult.ucCategory = CATEGORY_PERIPHERAL;
		tTMDResult.ucModule = MODULE_PERIPHERAL_ETHERNET;
		tTMDResult.ucAction = 0x02;
		tTMDResult.eResult = FAI_TMD_ERR_FAIL;
		ucTmdErrCode[0] = FAI_MODULE_TMD_ERR_COMMON;
		ucTmdErrCode[1] = FAI_COMMON_TMD_ERR_INVALID_PARAMETER;
		FAI_TMD_AddResultParam(&tTMDResult, &ucTmdErrCode[0], sizeof(ucTmdErrCode));

		FAI_TMD_SetResult(&tTMDResult);

	return FAI_ERR;
}
#endif


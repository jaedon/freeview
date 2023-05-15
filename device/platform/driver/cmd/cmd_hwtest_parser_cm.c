#if defined(CONFIG_VK_STDLIB)
#else
#include 	<stdio.h>
#endif
#include "di_channel_priv.h"
#include "di_cm.h"
#include "cmdtool.h"
#include <netinet/in.h>	/* sockaddr_in{} and other Internet defns */
#include <netdb.h>
#include <arpa/inet.h>	/* inet(3) functions */
#include <string.h>
#include <stdlib.h>

#include "di_cm.h"
#if defined(CONFIG_CABLE_MODEM_BRCM_DSGCC)
#include "drv_cm_bdsgcc.h"
#endif
#if defined(CONFIG_CABLE_MODEM_BRCM_RPC)
#include "drv_cm_brpc.h"
#endif

#include "di_system.h"

/* return value of command function */
#define CMD_OK					0
#define CMD_ERR					1

/* global variables */

/* static variables */

/* static functions */

/* extern functions */
extern HINT32 show_di_cm;
extern HINT32 show_di_snmp;

#if defined(CONFIG_CABLE_MODEM)
#define READABLE_IN_DEC(sz,val) CMD_ReadAsDec(sz, &val)


static void P_CM_CbCheckReboot(void)
{
	CMD_Printf("\n\nCallback function [%s] is called!!\n\n", __FUNCTION__);
}

void hwtest_CB_CM_LINK_Notify(DI_CM_LinkInfo_e etLinkInfo )
{

	if (etLinkInfo == DI_CM_EVENT_ONLINE)
	{
		VK_printf("DI_CM_EVENT_ONLINE\n");

	} else if (etLinkInfo == DI_CM_EVENT_OFFLINE)
	{
		VK_printf("DI_CM_EVENT_OFFLINE\n");
	}
	else
	{
		VK_printf("DI_CM_EVENT_NONE\n");
	}
}

int CMD_HwTest_Parser_CM(void *szArgs)
{
	int iResult=CMD_ERR;
	unsigned int nCmIpAddr=0;
	unsigned int nGwIpAddr=0;
	HINT8 szHostIpAddr[15]={'\0',};
	HUINT8 cm_ver[15]={""};
	HUINT8 cm_macaddr[6]={""};
	struct in_addr stCmIpAddr;
	struct in_addr stGwIpAddr;
	char *pTempString = NULL;
	char *pDisplayString = NULL;
	int CmSavedTemp = 0,CmCurrentTemp = 0;

	char *szCmd=(char *)CMD_NextArg((char **)&szArgs);

	if (szCmd!=NULL && VK_strcmp(szCmd,"ip")==0)
	{
		pTempString = DD_MEM_Alloc(200);
		if ( pTempString == NULL )
		{
			CMD_Printf("Error %d lines in %s\n", __LINE__, __FUNCTION__);
			return CMD_ERR;
		}

		VK_memset(pTempString, 0x0, sizeof(pTempString));

		pDisplayString = CMD_AddString(pDisplayString, "\n", 1, 1);
		if ( pDisplayString == NULL )
		{
			CMD_Printf("Error %d lines in %s\n", __LINE__, __FUNCTION__);
			return CMD_ERR;
		}

		VK_snprintf(&pTempString[0], 13, " IP Address\n");
		pDisplayString = CMD_AddString(pDisplayString, &pTempString[0], 13, 0);

		DI_CM_GetCmIp((unsigned int *)&nCmIpAddr);
		if(nCmIpAddr != 0)
		{
			//stCmIpAddr.s_addr = htonl(nCmIpAddr);
			stCmIpAddr.s_addr = nCmIpAddr;
			CMD_Printf("CM Ip address \t: %s\n", (char *)inet_ntoa(stCmIpAddr));
			//VK_strcpy(szCmIpAddr, inet_ntoa(htonl(nCmIpAddr)));

			VK_snprintf(&pTempString[0], 28, " CM IP    : %s\n", (char *)inet_ntoa(stCmIpAddr));
			pDisplayString = CMD_AddString(pDisplayString, &pTempString[0], 28, 0);
		}
		else
		{
			CMD_Printf("Failed to get CM IP address.\n");

			VK_snprintf(&pTempString[0], 30, "Failed to get CM IP address.\n");
			pDisplayString = CMD_AddString(pDisplayString, &pTempString[0], 30, 0);
		}

		/* Host IP address */
		DI_CM_GetMpegIp((char *)&szHostIpAddr);
		if(szHostIpAddr[0] != 0)
		{
			CMD_Printf("Host Ip address \t: %s\n", (szHostIpAddr));

			VK_snprintf(&pTempString[0], 28, " HOST IP : %s\n", (szHostIpAddr));
			pDisplayString = CMD_AddString(pDisplayString, &pTempString[0], 28, 0);
		}
		else
		{
			CMD_Printf("Failed to get Host IP address.\n");

			VK_snprintf(&pTempString[0], 32, "Failed to get Host IP address.\n");
			pDisplayString = CMD_AddString(pDisplayString, &pTempString[0], 32, 0);
		}

		DI_CM_GetGateway((unsigned int *)&nGwIpAddr);
		if(nGwIpAddr != 0)
		{

			stGwIpAddr.s_addr = nGwIpAddr;
			CMD_Printf("GW Ip address \t: %s\n", (char *)inet_ntoa(stGwIpAddr));
			//VK_strcpy(szCmIpAddr, inet_ntoa(htonl(nCmIpAddr)));

			VK_snprintf(&pTempString[0], 28, " GW IP    : %s\n", (char *)inet_ntoa(stGwIpAddr));
			pDisplayString = CMD_AddString(pDisplayString, &pTempString[0], 28, 0);

		}
		else
		{
			CMD_Printf("Failed to get Gateway IP address.\n");

			VK_snprintf(&pTempString[0], 35, "Failed to get Gateway IP address.\n");
			pDisplayString = CMD_AddString(pDisplayString, &pTempString[0], 35, 0);
		}


		//VK_printf("%s_DisplayString(%s)\n", __FUNCTION__,pDisplayString);
		if ( CMD_GetInfoUI() != NULL )
		{
			(int *)(CMD_GetInfoUI())(pDisplayString);
		}
		DD_MEM_Free(pDisplayString);
		DD_MEM_Free(pTempString);
		pDisplayString = NULL;

	}
	else if(szCmd!=NULL && VK_strcmp(szCmd,"mac")==0)
	{

		pTempString = DD_MEM_Alloc(200);
		if ( pTempString == NULL )
		{
			CMD_Printf("Error %d lines in %s\n", __LINE__, __FUNCTION__);
			return CMD_ERR;
		}

		VK_memset(pTempString, 0x0, sizeof(pTempString));

		pDisplayString = CMD_AddString(pDisplayString, "\n", 1, 1);
		if ( pDisplayString == NULL )
		{
			CMD_Printf("Error %d lines in %s\n", __LINE__, __FUNCTION__);
			return CMD_ERR;
		}

		VK_snprintf(&pTempString[0], 17, " CM MAC Address\n");
		pDisplayString = CMD_AddString(pDisplayString, &pTempString[0], 17, 0);

		DI_CM_GetCmMac((unsigned char *)cm_macaddr);
		CMD_Printf("CM mac addr \t[%02hX:%02hX:%02hX:%02hX:%02hX:%02hX]\n",
			cm_macaddr[0], cm_macaddr[1], cm_macaddr[2],
			cm_macaddr[3], cm_macaddr[4], cm_macaddr[5] );


		VK_snprintf(&pTempString[0], 37, " CM mac addr [%02hX:%02hX:%02hX:%02hX:%02hX:%02hX]\n",
			cm_macaddr[0], cm_macaddr[1], cm_macaddr[2],
			cm_macaddr[3], cm_macaddr[4], cm_macaddr[5]);
		pDisplayString = CMD_AddString(pDisplayString, &pTempString[0], 37, 0);

		//VK_printf("%s_DisplayString(%s)\n", __FUNCTION__,pDisplayString);
		if ( CMD_GetInfoUI() != NULL )
		{
			(int *)(CMD_GetInfoUI())(pDisplayString);
		}
		DD_MEM_Free(pDisplayString);
		DD_MEM_Free(pTempString);
		pDisplayString = NULL;

	}
	else if(szCmd!=NULL && VK_strcmp(szCmd,"ver")==0)
	{
		pTempString = DD_MEM_Alloc(200);
		if ( pTempString == NULL )
		{
			CMD_Printf("Error %d lines in %s\n", __LINE__, __FUNCTION__);
			return CMD_ERR;
		}

		VK_memset(pTempString, 0x0, sizeof(pTempString));
		VK_memset(cm_ver, 0x0, sizeof(cm_ver));

		pDisplayString = CMD_AddString(pDisplayString, "\n", 1, 1);
		if ( pDisplayString == NULL )
		{
			CMD_Printf("Error %d lines in %s\n", __LINE__, __FUNCTION__);
			return CMD_ERR;
		}

		VK_snprintf(&pTempString[0], 13, " CM Version\n");
		pDisplayString = CMD_AddString(pDisplayString, &pTempString[0], 13, 0);

		DI_CM_GetCmVer((char *)cm_ver);
		CMD_Printf("CM ver :\t [%s]\n", cm_ver);

		VK_snprintf(&pTempString[0], 25, " CM ver : [%s]", cm_ver);
		pDisplayString = CMD_AddString(pDisplayString, &pTempString[0], 25, 0);

		//CMD_Printf("%s_DisplayString(%s)\n", __FUNCTION__,pDisplayString);
		if ( CMD_GetInfoUI() != NULL )
		{
			(int *)(CMD_GetInfoUI())(pDisplayString);
		}
		DD_MEM_Free(pDisplayString);
		DD_MEM_Free(pTempString);
		pDisplayString = NULL;

	}
	else if(szCmd!=NULL && VK_strcmp(szCmd,"dsinfo")==0)
	{
		HUINT32  ulUCB[4]={0,},ulChID[4]={0,}, ulLock[4]={0,};
		HINT32 lFDCPower[4]={0,}, lFDCMod[4]={0,}, lFDCSnr[4]={0,}, ulFDCFreq[4]={0,};
		HINT32 i=0;
		HCHAR *pcFDCMod=NULL;

		pTempString = DD_MEM_Alloc(200);
		if ( pTempString == NULL )
		{
			CMD_Printf("Error %d lines in %s\n", __LINE__, __FUNCTION__);
			return CMD_ERR;
		}

		pcFDCMod = DD_MEM_Alloc(16);
		if ( pcFDCMod == NULL )
		{
			CMD_Printf("Error %d lines in %s\n", __LINE__, __FUNCTION__);
			return CMD_ERR;
		}

		VK_memset(pTempString, 0x0, sizeof(pTempString));

		pDisplayString = CMD_AddString(pDisplayString, "\n", 1, 1);
		if ( pDisplayString == NULL )
		{
			CMD_Printf("Error %d lines in %s\n", __LINE__, __FUNCTION__);
			return CMD_ERR;
		}

		CMD_Printf("****************************************\n");
		CMD_Printf("Ch|Lock|Frequency|Mod|Ch ID|Power|SNR\n");
		CMD_Printf("****************************************\n");

		VK_snprintf(&pTempString[0], 48, "Ch| Lock| Freq |Mod|Ch ID|Power(dBmV)|SNR(dB)\n");
		pDisplayString = CMD_AddString(pDisplayString, &pTempString[0], 48, 0);

		for (i=0; i<4; i++)
		{
			DI_CM_GetFDCFreq30(i, (HINT32 *)&ulFDCFreq[i]);
			DI_CM_GetFDCMod30(i, (HINT32 *)&lFDCMod[i]);
			DI_CM_GetFDCPower30(i, (int *)&lFDCPower[i]);
			DI_CM_GetFDCSnr30(i, (int *)&lFDCSnr[i]);
			DI_CM_GetFDCChannelId30(i, (HINT32 *)&ulChID[i]);
			ulLock[i] = DI_CM_CmStatus();


			if((HINT32 )ulFDCFreq[i] == -1)
			{
				ulLock[i] = 3;
				lFDCPower[i] = 0;
			}

			VK_memset(pcFDCMod, 0x0, sizeof(pcFDCMod));
			switch (lFDCMod[i])
			{
				case DI_CM_FDC_MOD_ERROR :
					VK_snprintf(&pcFDCMod[0], 6, "ERROR");
					break;
				case DI_CM_FDC_MOD_UNKNOWN :
					VK_snprintf(&pcFDCMod[0], 8, "----");
					break;
				case DI_CM_FDC_MOD_OTHER :
					VK_snprintf(&pcFDCMod[0], 6, "OTHER");
					break;
				case DI_CM_FDC_MOD_QAM64 :
					VK_snprintf(&pcFDCMod[0], 7, "64");
					break;
				case DI_CM_FDC_MOD_QAM256 :
					VK_snprintf(&pcFDCMod[0], 8, "256");
					break;
				default :
					VK_snprintf(&pcFDCMod[0], 6, "ERROR");
					break;
			}

			CMD_Printf("%d     %s    %d Hz    %s    %d    %d.%d dBmV    %d dB    %d\n", i+1,(ulLock[i]>=4)?"O":"X",ulFDCFreq[i], pcFDCMod, ulChID[i], lFDCPower[i]/10, abs(lFDCPower[i]%10), lFDCSnr[i]/10,ulUCB[i]  );

			VK_snprintf(&pTempString[0], 35, "%d | %s| %dMHz| %s | %d| %d.%d| %d\n", i+1,(ulLock[i]>=4)?"O":"X",ulFDCFreq[i]/1000000, pcFDCMod, ulChID[i], lFDCPower[i]/10, abs(lFDCPower[i]%10), lFDCSnr[i]/10);
			pDisplayString = CMD_AddString(pDisplayString, &pTempString[0], 35, 0);

		}

		//VK_printf("%s_DisplayString(%s)\n", __FUNCTION__,pDisplayString);
		if ( CMD_GetInfoUI() != NULL )
		{
			(int *)(CMD_GetInfoUI())(pDisplayString);
		}
		DD_MEM_Free(pDisplayString);
		DD_MEM_Free(pTempString);
		DD_MEM_Free(pcFDCMod);
		pDisplayString = NULL;


	}
	else if(szCmd!=NULL && VK_strcmp(szCmd,"usinfo")==0)
	{
		HUINT32 ulRDCSR[4]={0,}, ulRDCType[4]={0,},ulLock[4]={0,}, ulRDCBW[4]={0,};
		HINT32 lRDCPower[4]={0,},ulChID[4]={0,}, ulRDCFreq[4]={0,} ;
		HINT32 i=0;
		HCHAR *pcRDCMod=NULL;

		pTempString = DD_MEM_Alloc(200);
		if ( pTempString == NULL )
		{
			CMD_Printf("Error %d lines in %s\n", __LINE__, __FUNCTION__);
			return CMD_ERR;
		}

		VK_memset(pTempString, 0x0, sizeof(pTempString));

		pDisplayString = CMD_AddString(pDisplayString, "\n", 1, 1);
		if ( pDisplayString == NULL )
		{
			CMD_Printf("Error %d lines in %s\n", __LINE__, __FUNCTION__);
			return CMD_ERR;
		}

		pcRDCMod = DD_MEM_Alloc(16);

		CMD_Printf("****************************************\n");
		CMD_Printf("Ch|Lock|Frequency|Type|Ch ID|Power|SR\n");
		CMD_Printf("****************************************\n");

		VK_snprintf(&pTempString[0], 49, "Ch|Lock| Freq |Type|Ch ID|Power(dBmV)|SR(Ksps)\n");
		pDisplayString = CMD_AddString(pDisplayString, &pTempString[0], 49, 0);

		for (i=0; i<4; i++)
		{
			DI_CM_GetRDCFreq30(i, (HINT32 *)&ulRDCFreq[i]);
			DI_CM_GetRDCChannelId30(i, (HINT32 *)&ulChID[i]);
			DI_CM_GetRDCPower30(i, (HUINT32 *)&lRDCPower[i]);
			DI_CM_GetRDCSymbolRate30(i, (HUINT32 *)&ulRDCSR[i]);
			DI_CM_GetRDCMod30(i, (HINT32 *)&ulRDCType[i]);
			DI_CM_GetRDCBandWidth30(i, (HUINT32 *)&ulRDCBW[i]);
			ulLock[i] = DI_CM_CmStatus();


			if((HINT32 )ulRDCFreq[i] == -1)
			{
				ulLock[i] = 3;
				ulRDCSR[i] = 0;
				lRDCPower[i] = 0;
			}

			VK_memset(pcRDCMod, 0x0, sizeof(pcRDCMod));
			switch (ulRDCType[i])
			{
				case DI_CM_RDC_MOD_ERROR :
					VK_snprintf(&pcRDCMod[0], 6, "ERROR");
					break;
				case DI_CM_RDC_MOD_UNKNOWN :
					VK_snprintf(&pcRDCMod[0], 8, "----");
					break;
				case DI_CM_RDC_MOD_TDMA :
					VK_snprintf(&pcRDCMod[0], 5, "TDMA");
					break;
				case DI_CM_RDC_MOD_ATDMA :
					VK_snprintf(&pcRDCMod[0], 6, "ATDMA");
					break;
				case DI_CM_RDC_MOD_SCDMA :
					VK_snprintf(&pcRDCMod[0], 6, "SCDMA");
					break;
				case DI_CM_RDC_MOD_TDMAANDATDMA :
					VK_snprintf(&pcRDCMod[0], 11, "MIXED");
					break;
				default :
					VK_snprintf(&pcRDCMod[0], 6, "ERROR");
					break;
			}


			CMD_Printf("%d     %s    %d Hz    %s    %d    %d.%d dBmV    %d ksps    %d\n", i+1, (ulLock[i]>=4)?"O":"X",ulRDCFreq[i], pcRDCMod, ulChID[i], lRDCPower[i]/10, abs(lRDCPower[i]%10), ulRDCSR[i], ulRDCBW[i]);

			VK_snprintf(&pTempString[0], 39, "%d | %s| %dKHz| %s| %d| %d.%d| %d\n", i+1, (ulLock[i]>=4)?"O":"X",ulRDCFreq[i]/1000, pcRDCMod, ulChID[i], lRDCPower[i]/10, abs(lRDCPower[i]%10), ulRDCSR[i]);
			pDisplayString = CMD_AddString(pDisplayString, &pTempString[0], 39, 0);
		}


			//VK_printf("%s_DisplayString(%s)\n", __FUNCTION__,pDisplayString);
		if ( CMD_GetInfoUI() != NULL )
		{
			(int *)(CMD_GetInfoUI())(pDisplayString);
		}
		DD_MEM_Free(pDisplayString);
		DD_MEM_Free(pTempString);
		DD_MEM_Free(pcRDCMod);
		pDisplayString = NULL;


	}
	else if(szCmd!=NULL && VK_strcmp(szCmd,"fdc")==0)
	{
		HUINT32 ulFDCFreq=0, ulRDCFreq;
		HINT32 lFDCPower=0, lFDCMod=0, lFDCSnr=0;
		HINT32 lRDCPower=0, lRDCChannelId=0;


		pTempString = DD_MEM_Alloc(200);
		if ( pTempString == NULL )
		{
			CMD_Printf("Error %d lines in %s\n", __LINE__, __FUNCTION__);
			return CMD_ERR;
		}

		VK_memset(pTempString, 0x0, sizeof(pTempString));

		pDisplayString = CMD_AddString(pDisplayString, "\n", 1, 1);
		if ( pDisplayString == NULL )
		{
			CMD_Printf("Error %d lines in %s\n", __LINE__, __FUNCTION__);
			return CMD_ERR;
		}

		VK_snprintf(&pTempString[0], 18, " CM FDC RDC Info\n");
		pDisplayString = CMD_AddString(pDisplayString, &pTempString[0], 18, 0);

		/* FDC Frequency */
		DI_CM_GetFDCFreq((HINT32 *)&ulFDCFreq);
		/* FDC Modulation */
		DI_CM_GetFDCMod((HINT32 *)&lFDCMod);
		/* FDC Power */
		DI_CM_GetFDCPower((int *)&lFDCPower);

		/* FDC SNR */
		DI_CM_GetFDCSnr((int *)&lFDCSnr);

		/* RDC Frequency */
		DI_CM_GetRDCFreq((HINT32 *)&ulRDCFreq);
		/* RDC Channel ID */
		DI_CM_GetRDCChannelId((HINT32 *)&lRDCChannelId);
		/* RDC Power */
		DI_CM_GetRDCPower((HUINT32 *)&lRDCPower);

		CMD_Printf("*********************\n");
		CMD_Printf("FDC Info\n");
		CMD_Printf("FDC Frequency\t\t%d Hz\n", ulFDCFreq);
		CMD_Printf("FDC Modulation\t%s\n", (lFDCMod==3)?"64-QAM":"256-QAM");
		CMD_Printf("FDC Power\t\t%d.%d dBmV\n", lFDCPower/10, abs(lFDCPower%10));
		CMD_Printf("FDC SNR\t\t%d dB\n", lFDCSnr/10);
		CMD_Printf("*********************\n");
		CMD_Printf("RDC Info\n");
		CMD_Printf("RDC Frequency\t\t%d Hz\n", ulRDCFreq);
		CMD_Printf("RDC Channel ID\t%d\n", lRDCChannelId);
		CMD_Printf("RDC Power\t\t%d.%d dBmV\n", lRDCPower/10, abs(lRDCPower%10));
		CMD_Printf("*********************\n");

		VK_snprintf(&pTempString[0], 10, "FDC Info\n");
		pDisplayString = CMD_AddString(pDisplayString, &pTempString[0], 10, 0);
		VK_snprintf(&pTempString[0], 30, "FDC Frequency %d Hz\n", ulFDCFreq);
		pDisplayString = CMD_AddString(pDisplayString, &pTempString[0], 30, 0);
		VK_snprintf(&pTempString[0], 25, "FDC Modulation %s\n", (lFDCMod==3)?"64-QAM":"256-QAM");
		pDisplayString = CMD_AddString(pDisplayString, &pTempString[0], 25, 0);
		VK_snprintf(&pTempString[0], 22, "FDC Power %d.%d dBmV\n", lFDCPower/10, abs(lFDCPower%10));
		pDisplayString = CMD_AddString(pDisplayString, &pTempString[0], 22, 0);
		VK_snprintf(&pTempString[0], 20, "FDC SNR %d dB\n", lFDCSnr/10);
		pDisplayString = CMD_AddString(pDisplayString, &pTempString[0], 20, 0);
		VK_snprintf(&pTempString[0], 22, "--------------------\n");
		pDisplayString = CMD_AddString(pDisplayString, &pTempString[0], 22, 0);
		VK_snprintf(&pTempString[0], 10, "RDC Info\n");
		pDisplayString = CMD_AddString(pDisplayString, &pTempString[0], 10, 0);
		VK_snprintf(&pTempString[0], 28, "RDC Frequency %d Hz\n", ulRDCFreq);
		pDisplayString = CMD_AddString(pDisplayString, &pTempString[0], 28, 0);
		VK_snprintf(&pTempString[0], 20, "RDC Channel ID %d\n", lRDCChannelId);
		pDisplayString = CMD_AddString(pDisplayString, &pTempString[0], 20, 0);
		VK_snprintf(&pTempString[0], 25, "RDC Power %d.%d dBmV\n", lRDCPower/10, abs(lRDCPower%10));
		pDisplayString = CMD_AddString(pDisplayString, &pTempString[0], 25, 0);

		//VK_printf("%s_DisplayString(%s)\n", __FUNCTION__,pDisplayString);
		if ( CMD_GetInfoUI() != NULL )
		{
			(int *)(CMD_GetInfoUI())(pDisplayString);
		}
		DD_MEM_Free(pDisplayString);
		DD_MEM_Free(pTempString);
		pDisplayString = NULL;


	}
	else if(szCmd!=NULL && VK_strcmp(szCmd,"scan")==0)
	{
		unsigned int ulFreq;
		char scan_list[100] = {""};

		char *szUnitId=CMD_NextArg((char **)&szArgs);
		if (szUnitId != NULL )
		{
			VK_sscanf(szUnitId, "%d", (int *)&ulFreq);
			ulFreq = ulFreq * 1000000;
			if( (ulFreq > 999000000) )
			{
				CMD_Printf("Wrong Frequency\n");
			}
			else
			{
				DI_CM_SetScanDSFreq(&ulFreq);
			}
		}
		else
		{
			DI_CM_GetScanDSFreq(scan_list);
		}
	}
	else if(szCmd!=NULL && VK_strcmp(szCmd,"temp")==0)
	{

		DI_CM_GetCmSavedTemp(&CmSavedTemp,&CmCurrentTemp);
		VK_printf("CM Temperature :\t [%d] Max Temperature [%d]\n", CmCurrentTemp,CmSavedTemp);
	}
	else if(szCmd!=NULL && VK_strcmp(szCmd,"disp")==0)
	{

		pTempString = DD_MEM_Alloc(200);
		if ( pTempString == NULL )
		{
			CMD_Printf("Error %d lines in %s\n", __LINE__, __FUNCTION__);
			return CMD_ERR;
		}

		VK_memset(pTempString, 0x0, sizeof(pTempString));

		pDisplayString = CMD_AddString(pDisplayString, "\n", 1, 1);
		if ( pDisplayString == NULL )
		{
			CMD_Printf("Error %d lines in %s\n", __LINE__, __FUNCTION__);
			return CMD_ERR;
		}

		VK_snprintf(&pTempString[0], 50, " CM S/W Upgrade in Progress....  Wait\n");
		pDisplayString = CMD_AddString(pDisplayString, &pTempString[0], 50, 0);
		if ( CMD_GetInfoUI() != NULL )
		{
			(int *)(CMD_GetInfoUI())(pDisplayString);
		}

		DD_MEM_Free(pDisplayString);
		DD_MEM_Free(pTempString);
		pDisplayString = NULL;

	}
	else if(szCmd!=NULL && VK_strcmp(szCmd,"dload")==0)
	{
		int nStat = 0;
		int nTimeOut = 1;

		pTempString = DD_MEM_Alloc(200);
		if ( pTempString == NULL )
		{
			CMD_Printf("Error %d lines in %s\n", __LINE__, __FUNCTION__);
			return CMD_ERR;
		}

		VK_memset(pTempString, 0x0, sizeof(pTempString));

		pDisplayString = CMD_AddString(pDisplayString, "\n", 1, 1);
		if ( pDisplayString == NULL )
		{
			CMD_Printf("Error %d lines in %s\n", __LINE__, __FUNCTION__);
			return CMD_ERR;
		}
#if 0
		VK_snprintf(&pTempString[0], 50, " CM S/W Upgrade in Progress....  Wait\n");
		pDisplayString = CMD_AddString(pDisplayString, &pTempString[0], 50, 0);
		if ( CMD_GetInfoUI() != NULL )
		{
			(int *)(CMD_GetInfoUI())(pDisplayString);
		}
#endif
		VK_SYSTEM_Command("/sbin/tftpd /media &");
		DRV_CM_SetDsgMode(2);
		DRV_CM_SetDsgScan(0);
		DRV_CM_SwUpgradeTrigger();
		VK_printf("CM Software upgrade.... \n");

		while(1)
		{
			VK_TASK_Sleep(1000);

			nStat = DRV_CM_GetSwOperStatus();
			if (nStat == 1)
			{

				VK_printf(" <<< CM S/W Upgrade in Progress....  [%d]\n", nStat);

			}
			else if (nStat == 4)
			{
				VK_snprintf(&pTempString[0], 50, " CM S/W Upgrade....  Fail!!!\n");
				pDisplayString = CMD_AddString(pDisplayString, &pTempString[0], 50, 0);

				if ( CMD_GetInfoUI() != NULL )
				{
					(int *)(CMD_GetInfoUI())(pDisplayString);
				}
				break;
			}
			else if( (nStat == 2) || (nStat == 3))
			{
				VK_snprintf(&pTempString[0], 50, " CM S/W Upgrade....  Complete!\n");
				pDisplayString = CMD_AddString(pDisplayString, &pTempString[0], 50, 0);
				if ( CMD_GetInfoUI() != NULL )
				{
					(int *)(CMD_GetInfoUI())(pDisplayString);
				}
				break;
			}
			else
			{
				VK_printf(" SwOpStatus Other[%d] \n", nStat);
			}

			if (nTimeOut >= 20) /* 2분 */
			{
				VK_snprintf(&pTempString[0], 50, " CM S/W Upgrade....  TIMEOUT!\n");
				pDisplayString = CMD_AddString(pDisplayString, &pTempString[0], 50, 0);

				if ( CMD_GetInfoUI() != NULL )
				{
					(int *)(CMD_GetInfoUI())(pDisplayString);
				}
				break;
			}
			nTimeOut++;

		}

		DRV_CM_SetDsgMode(0);
		DRV_CM_SetDsgScan(1);
		DD_MEM_Free(pDisplayString);
		DD_MEM_Free(pTempString);
		pDisplayString = NULL;

	}
	else if(szCmd!=NULL && VK_strcmp(szCmd,"reset")==0)
	{
		// TBD
		//DI_CM_HwGpioReset();
	}
	else if(szCmd!=NULL && VK_strcmp(szCmd,"status")==0)
	{
		int bret = 0;


		pTempString = DD_MEM_Alloc(200);
		if ( pTempString == NULL )
		{
			CMD_Printf("Error %d lines in %s\n", __LINE__, __FUNCTION__);
			return CMD_ERR;
		}

		VK_memset(pTempString, 0x0, sizeof(pTempString));

		pDisplayString = CMD_AddString(pDisplayString, "\n", 1, 1);
		if ( pDisplayString == NULL )
		{
			CMD_Printf("Error %d lines in %s\n", __LINE__, __FUNCTION__);
			return CMD_ERR;
		}

		VK_snprintf(&pTempString[0], 13, " CM STATUS\n");
		pDisplayString = CMD_AddString(pDisplayString, &pTempString[0], 13, 0);

		bret = DI_CM_CmStatus();
		/*Comment for CmStatus */
		/*
		             other(1),
		            notReady(2),
		            notSynchronized(3),
		            phySynchronized(4),
		            usParametersAcquired(5),
		            rangingComplete(6),
		            ipComplete(7),
		            todEstablished(8),
		            securityEstablished(9),
		            paramTransferComplete(10),
		            registrationComplete(11),
		            operational(12),
		            accessDenied(13)
		*/

		if(bret != -1)
		{
			switch(bret)
			{
				case 1:
					CMD_Printf("Cm Online Status = other\n");

					VK_snprintf(&pTempString[0], 26, " Cm Online Status : other\n");
					pDisplayString = CMD_AddString(pDisplayString, &pTempString[0], 26, 0);
					break;
				case 2:
					CMD_Printf("Cm Online Status = notReady\n");

					VK_snprintf(&pTempString[0], 29, " Cm Online Status : notReady\n");
					pDisplayString = CMD_AddString(pDisplayString, &pTempString[0], 29, 0);
					break;
				case 3:
					CMD_Printf("Cm Online Status = notSynchronized\n");

					VK_snprintf(&pTempString[0], 36, " Cm Online Status : notSynchronized\n");
					pDisplayString = CMD_AddString(pDisplayString, &pTempString[0], 36, 0);
					break;
				case 4:
					CMD_Printf("Cm Online Status = phySynchronized\n");

					VK_snprintf(&pTempString[0], 36, " Cm Online Status : phySynchronized\n");
					pDisplayString = CMD_AddString(pDisplayString, &pTempString[0], 36, 0);
					break;
				case 5:
					CMD_Printf("Cm Online Status = usParametersAcquired\n");

					VK_snprintf(&pTempString[0], 41, " Cm Online Status : usParametersAcquired\n");
					pDisplayString = CMD_AddString(pDisplayString, &pTempString[0], 41, 0);
					break;
				case 6:
					CMD_Printf("Cm Online Status = rangingComplete\n");

					VK_snprintf(&pTempString[0], 36, " Cm Online Status : rangingComplete\n");
					pDisplayString = CMD_AddString(pDisplayString, &pTempString[0], 36, 0);
					break;
				case 7:
					CMD_Printf("Cm Online Status = ipComplete\n");

					VK_snprintf(&pTempString[0], 31, " Cm Online Status : ipComplete\n");
					pDisplayString = CMD_AddString(pDisplayString, &pTempString[0], 31, 0);
					break;
				case 8:
					CMD_Printf("Cm Online Status = todEstablished\n");

					VK_snprintf(&pTempString[0], 35, " Cm Online Status : todEstablished\n");
					pDisplayString = CMD_AddString(pDisplayString, &pTempString[0], 35, 0);
					break;
				case 9:
					CMD_Printf("Cm Online Status = securityEstablished\n");

					VK_snprintf(&pTempString[0], 40, " Cm Online Status : securityEstablished\n");
					pDisplayString = CMD_AddString(pDisplayString, &pTempString[0], 40, 0);
					break;
				case 10:
					CMD_Printf("Cm Online Status = paramTransferComplete\n");

					VK_snprintf(&pTempString[0], 42, " Cm Online Status : paramTransferComplete\n");
					pDisplayString = CMD_AddString(pDisplayString, &pTempString[0], 42, 0);
					break;
				case 11:
					CMD_Printf("Cm Online Status = registrationComplete\n");

					VK_snprintf(&pTempString[0], 41, " Cm Online Status : registrationComplete\n");
					pDisplayString = CMD_AddString(pDisplayString, &pTempString[0], 41, 0);
					break;
				case 12:
					CMD_Printf("Cm Online Status = operational\n");

					VK_snprintf(&pTempString[0], 32, " Cm Online Status : operational\n");
					pDisplayString = CMD_AddString(pDisplayString, &pTempString[0], 32, 0);
					break;
				case 13:
					CMD_Printf("Cm Online Status = accessDenied\n");

					VK_snprintf(&pTempString[0], 33, " Cm Online Status : accessDenied\n");
					pDisplayString = CMD_AddString(pDisplayString, &pTempString[0], 33, 0);
					break;
				default:
					CMD_Printf("Cm Online Status = Unknown state = %d\n", bret);

					VK_snprintf(&pTempString[0], 35, " Cm Online Status : Unknown state\n");
					pDisplayString = CMD_AddString(pDisplayString, &pTempString[0], 35, 0);
					break;
			}
		}
		else
		{
			CMD_Printf("GetCmOnlineStatus Read Fail\n");

			VK_snprintf(&pTempString[0], 30, "GetCmOnlineStatus Read Fail\n");
			pDisplayString = CMD_AddString(pDisplayString, &pTempString[0], 30, 0);
		}

		//VK_printf("%s_DisplayString(%s)\n", __FUNCTION__,pDisplayString);
		if ( CMD_GetInfoUI() != NULL )
		{
			(int *)(CMD_GetInfoUI())(pDisplayString);
		}
		DD_MEM_Free(pDisplayString);
		DD_MEM_Free(pTempString);
		pDisplayString = NULL;

	}
	else if(szCmd!=NULL && VK_strcmp(szCmd,"ifup")==0)
	{
		VK_SYSTEM_Command("ifup eth1");
	}
	else if(szCmd!=NULL && VK_strcmp(szCmd,"sys")==0)
	{
		VK_SYSTEM_Command("ifconfig -a");
	}
	else if(szCmd!=NULL && VK_strcmp(szCmd,"route")==0)
	{
		VK_SYSTEM_Command("route");
	}
	else if (szCmd!=NULL && VK_strcmp(szCmd,"tftpd")==0)
	{
		VK_SYSTEM_Command("/sbin/tftpd /media &");

	}
	else if(szCmd!=NULL && VK_strcmp(szCmd,"fdc30")==0)
	{
		HUINT32 ulFDCFreq=0, ulFDCUncorrected=0;
		HINT32 lFDCPower=0, lFDCMod=0, lFDCSnr=0, lFDCChannelId=0, lFDCLock=0;
		HINT32 nChannelId;
		HCHAR *pcFDCMod=NULL;

		char *szChannelId=(char *)CMD_NextArg((char **)&szArgs);

		if (szChannelId!=NULL)
		{
			if (READABLE_IN_DEC(szChannelId, nChannelId) != 1 )
			{
				CMD_Printf("ERR! Bad Comman Iput for status fdc30 channelid. %d \n",nChannelId);
                		return CMD_ERR;
          		 }

			/* FDC Status */
			lFDCLock = DI_CM_CmStatus();

			/* FDC Channel ID */
			DI_CM_GetFDCChannelId30(nChannelId, (HINT32 *)&lFDCChannelId);

			/* FDC Frequency */
			DI_CM_GetFDCFreq30(nChannelId, (HINT32 *)&ulFDCFreq);

			/* FDC Modulation */
			DI_CM_GetFDCMod30(nChannelId, (HINT32 *)&lFDCMod);
			pcFDCMod = DD_MEM_Alloc(16);
			if ( pcFDCMod == NULL )
			{
				CMD_Printf("Error %d lines in %s\n", __LINE__, __FUNCTION__);
				return CMD_ERR;
			}
			VK_memset(pcFDCMod, 0x0, sizeof(pcFDCMod));
			switch (lFDCMod)
			{
				case DI_CM_FDC_MOD_ERROR :
					VK_snprintf(&pcFDCMod[0], 6, "Error");
					break;
				case DI_CM_FDC_MOD_UNKNOWN :
					VK_snprintf(&pcFDCMod[0], 8, "Unknown");
					break;
				case DI_CM_FDC_MOD_OTHER :
					VK_snprintf(&pcFDCMod[0], 6, "Other");
					break;
				case DI_CM_FDC_MOD_QAM64 :
					VK_snprintf(&pcFDCMod[0], 7, "64-QAM");
					break;
				case DI_CM_FDC_MOD_QAM256 :
					VK_snprintf(&pcFDCMod[0], 8, "256-QAM");
					break;
				default :
					VK_snprintf(&pcFDCMod[0], 6, "Error");
					break;
			}

			/* FDC Power */
			DI_CM_GetFDCPower30(nChannelId, (int *)&lFDCPower);

			/* FDC SNR */
			DI_CM_GetFDCSnr30(nChannelId, (int *)&lFDCSnr);

			/* FDC Uncorrected */
			DI_CM_GetFDCUncorrected30(nChannelId, (unsigned int *)&ulFDCUncorrected);

			CMD_Printf("Downstream ChannelId [%d] *******\n", nChannelId);
			CMD_Printf("FDC Info\n");
			CMD_Printf("FDC Status\t%s \n",(lFDCLock>=4)?"LOCKED":"IN-PROGRESS");
			CMD_Printf("FDC Channel ID\t%d \n", lFDCChannelId);
			CMD_Printf("FDC Frequency\t%d Hz\n", ulFDCFreq);
			CMD_Printf("FDC Modulation\t%s\n", pcFDCMod);
			CMD_Printf("FDC Power\t%d.%d dBmV\n", lFDCPower/10, abs(lFDCPower%10));
			CMD_Printf("FDC SNR\t\t%d dB\n", lFDCSnr/10);
			CMD_Printf("FDC Uncorrected\t%d \n", ulFDCUncorrected);
			CMD_Printf("********************************\n");

			DD_MEM_Free(pcFDCMod);
		}
	else
	{
			CMD_Printf("ERR! Bad Comman Iput for fdc30. \n");
		}

		iResult = CMD_OK;
	}
	else if(szCmd!=NULL && VK_strcmp(szCmd,"rdc30")==0)
	{
		HUINT32 ulRDCFreq=0, ulRDCSymbolRate=0;
		HINT32 lRDCPower=0, lRDCChannelId=0, lRDCMod=0, lRDCRange=0;
		HINT32 nChannelId;
		HCHAR *pcRDCRange=NULL;
		HCHAR *pcRDCMod=NULL;

		char *szChannelId=(char *)CMD_NextArg((char **)&szArgs);

		if (szChannelId!=NULL)
		{
			if (READABLE_IN_DEC(szChannelId, nChannelId) != 1 )
			{
				CMD_Printf("ERR! Bad Comman Iput for status rdc30 channelid. %d \n",nChannelId);
                		return CMD_ERR;
            		}

			/* RDC Status */
			DI_CM_GetRDCRange30(nChannelId, (HINT32 *)&lRDCRange);
			pcRDCRange = DD_MEM_Alloc(20);
			if ( pcRDCRange == NULL )
			{
				CMD_Printf("Error %d lines in %s\n", __LINE__, __FUNCTION__);
				return CMD_ERR;
			}
			VK_memset(pcRDCRange, 0x0, sizeof(pcRDCRange));
			switch (lRDCRange)
			{
				case DI_CM_RDC_RANGE_ERROR :
					VK_snprintf(&pcRDCRange[0], 6, "Error");
					break;
				case DI_CM_RDC_RANGE_OTHER :
					VK_snprintf(&pcRDCRange[0], 6, "Other");
					break;
				case DI_CM_RDC_RANGE_ABORTED :
					VK_snprintf(&pcRDCRange[0], 8, "Aborted");
					break;
				case DI_CM_RDC_RANGE_RETRIESEXCEEDED :
					VK_snprintf(&pcRDCRange[0], 16, "RetriesExceeded");
					break;
				case DI_CM_RDC_RANGE_SUCCESS :
					VK_snprintf(&pcRDCRange[0], 8, "Success");
					break;
				case DI_CM_RDC_RANGE_CONTINUE :
					VK_snprintf(&pcRDCRange[0], 9, "Continue");
					break;
				case DI_CM_RDC_RANGE_TIMEOUTT4 :
					VK_snprintf(&pcRDCRange[0], 10, "TimeoutT4");
					break;
				default :
					VK_snprintf(&pcRDCRange[0], 6, "Error");
					break;
			}

			/* RDC Channel ID */
			DI_CM_GetRDCChannelId30(nChannelId, (HINT32 *)&lRDCChannelId);

			/* RDC Frequency */
			DI_CM_GetRDCFreq30(nChannelId, (HINT32 *)&ulRDCFreq);

			/* FDC Modulation */
			DI_CM_GetRDCMod30(nChannelId, (HINT32 *)&lRDCMod);
			pcRDCMod = DD_MEM_Alloc(16);
			if ( pcRDCMod == NULL )
			{
				CMD_Printf("Error %d lines in %s\n", __LINE__, __FUNCTION__);
				return CMD_ERR;
			}
			VK_memset(pcRDCMod, 0x0, sizeof(pcRDCMod));
			switch (lRDCMod)
			{
				case DI_CM_RDC_MOD_ERROR :
					VK_snprintf(&pcRDCMod[0], 6, "Error");
					break;
				case DI_CM_RDC_MOD_UNKNOWN :
					VK_snprintf(&pcRDCMod[0], 8, "Unknown");
					break;
				case DI_CM_RDC_MOD_TDMA :
					VK_snprintf(&pcRDCMod[0], 5, "TDMA");
					break;
				case DI_CM_RDC_MOD_ATDMA :
					VK_snprintf(&pcRDCMod[0], 6, "ATDMA");
					break;
				case DI_CM_RDC_MOD_SCDMA :
					VK_snprintf(&pcRDCMod[0], 6, "SCDMA");
					break;
				case DI_CM_RDC_MOD_TDMAANDATDMA :
					VK_snprintf(&pcRDCMod[0], 11, "TDMA&ATDMA");
					break;
				default :
					VK_snprintf(&pcRDCMod[0], 6, "Error");
					break;
			}

			/* RDC Power */
			DI_CM_GetRDCPower30(nChannelId, (HUINT32 *)&lRDCPower);

			/* RDC SymbolRate */
			DI_CM_GetRDCSymbolRate30(nChannelId, (HUINT32 *)&ulRDCSymbolRate);

			CMD_Printf("Upstream ChannelId [%d] *********\n", nChannelId);
			CMD_Printf("RDC Info\n");
			CMD_Printf("RDC Ranging\t%s \n", pcRDCRange);
			CMD_Printf("RDC Channel ID\t%d\n", lRDCChannelId);
			CMD_Printf("RDC Frequency\t%d Hz\n", ulRDCFreq);
			CMD_Printf("RDC Modulation\t%s\n", pcRDCMod);
			CMD_Printf("RDC Power\t%d.%d dBmV\n", lRDCPower/10, abs(lRDCPower%10));
			CMD_Printf("RDC SymbolRate\t%d kSps\n", ulRDCSymbolRate/1250);
			CMD_Printf("********************************\n");

			DD_MEM_Free(pcRDCMod);
		}
		else
		{
			CMD_Printf("ERR! Bad Comman Iput for rdc30. \n");
		}

		iResult = CMD_OK;
	}
	else if(szCmd!=NULL && VK_strcmp(szCmd,"setcmmac")==0)
	{
		unsigned char ucCmMacL[3], ucCmMacH[3], ucCmMac[6];

		char *szMacH=(char *)CMD_NextArg((char **)&szArgs);
		char *szMacL=(char *)CMD_NextArg((char **)&szArgs);
		if (szMacH != NULL && szMacL !=NULL )
		{
			VK_sscanf(szMacH, "%x", (unsigned int *)&ucCmMacH);
			VK_sscanf(szMacL, "%x", (unsigned int *)&ucCmMacL);
			ucCmMac[0] = ucCmMacH[2];
			ucCmMac[1] = ucCmMacH[1];
			ucCmMac[2] = ucCmMacH[0];
			ucCmMac[3] = ucCmMacL[2];
			ucCmMac[4] = ucCmMacL[1];
			ucCmMac[5] = ucCmMacL[0];

			CMD_Printf("%02x, %02x, %02x, %02x, %02x, %02x\n", ucCmMac[5], ucCmMac[4], ucCmMac[3], ucCmMac[2], ucCmMac[1], ucCmMac[0]);
			DI_CM_SetCmMac(&ucCmMac[0]);

		}
		else
		{

			CMD_Printf("ERR! Bad Comman Iput. \n");
		}
	}
	else if(szCmd!=NULL && VK_strcmp(szCmd,"getcmserial")==0)
	{
		char ucCmSerial[14];

		DI_CM_GetCmSerial(&ucCmSerial[0]);

		CMD_Printf("<<< %s\n", ucCmSerial);
	}
	else if(szCmd!=NULL && VK_strcmp(szCmd,"setcmserial")==0)
	{
		char ucCmSerial[14];

		char *szSerial=(char *)CMD_NextArg((char **)&szArgs);
		if (szSerial != NULL )
		{
			VK_sscanf(szSerial, "%s", ucCmSerial);

			CMD_Printf("<<< %s\n", ucCmSerial);
			DI_CM_SetCmSerial(&ucCmSerial[0]);

		}
		else
		{
			CMD_Printf("ERR! Bad Comman Iput. \n");
		}
	}
	else if(szCmd!=NULL && VK_strcmp(szCmd,"scanstop")==0)
	{
		DRV_CM_SetDsgScan(0);
		VK_printf("CM ScanStop \n");
		iResult = CMD_OK;
	}
	else if(szCmd!=NULL && VK_strcmp(szCmd,"scango")==0)
	{
		DRV_CM_SetDsgScan(1);
		VK_printf("CM ScanGo \n");
		iResult = CMD_OK;
	}
	else if(szCmd!=NULL && VK_strcmp(szCmd,"cmstatus")==0)
	{
		HINT32 regStatus = 0;
		DRV_CM_GetCmRegStatus(&regStatus);
		VK_printf("DI_CM_CmStatus \n");
		iResult = CMD_OK;
	}
	else if(szCmd!=NULL && VK_strcmp(szCmd,"getcmmac")==0)
	{

		DRV_CM_GetEstbMac((unsigned char *)&cm_macaddr);
		VK_printf("CM Mac Address :\t [%x]:[%x]:[%x]:[%x]:[%x]:[%x]\n", cm_macaddr[0],cm_macaddr[1],cm_macaddr[2],cm_macaddr[3],cm_macaddr[4],cm_macaddr[5]);

	}
	else if(szCmd!=NULL && VK_strcmp(szCmd,"cmfan")==0)
	{
		HBOOL mode = 0;

		char *szModel=(char *)CMD_NextArg((char **)&szArgs);
		if (szModel != NULL )
		{
			VK_sscanf(szModel, "%d", &mode);

			DI_CM_SetFan(mode);

		}
		else
		{
			CMD_Printf("ERR! Bad Comman Iput. \n");
		}
	}
	else if(szCmd!=NULL && VK_strcmp(szCmd,"upgrade")==0)
	{
		int retVal =0;
		retVal = DI_CM_SwUpgrade();
		if ( retVal == DI_ERR_OK)
		{
			VK_printf("CM Upgrade Success!!\n");
		}
		else
		{
			VK_printf("CM Upgrade Fail!!\n");
		}
	}
	else if(szCmd!=NULL && VK_strcmp(szCmd,"cmtemp")==0)
	{
		HBOOL mode = 0;

		char *szModel=(char *)CMD_NextArg((char **)&szArgs);
		if (szModel != NULL )
		{
			VK_sscanf(szModel, "%d", &mode);

			DI_CM_SetTemp(mode);

		}
		else
		{
			CMD_Printf("ERR! Bad Comman Iput. \n");
		}
	}
	else if(szCmd!=NULL && VK_strcmp(szCmd,"cmallow")==0)
	{
		DRV_CM_SetAdminStatus(2);
		VK_printf("CM DocsSwOperStatus Allow \n");
		iResult = CMD_OK;
	}
	else if(szCmd!=NULL && VK_strcmp(szCmd,"cmlink")==0)
	{
		VK_printf("DI_CM_RegisterCallback.... \n");
		DI_CM_RegisterCallback(hwtest_CB_CM_LINK_Notify);
		iResult = CMD_OK;
	}
	else if(szCmd!=NULL && VK_strcmp(szCmd,"cmclear")==0)
	{
		DI_CM_SetCmDefaults();
		VK_printf("DI_CM_SetCmDefaults  \n");
		iResult = CMD_OK;
	}
	else if(szCmd!=NULL && VK_strcmp(szCmd,"cmledoff")==0)
	{
		HBOOL mode = 0;

		char *szModel=(char *)CMD_NextArg((char **)&szArgs);
		if (szModel != NULL )
		{
			VK_sscanf(szModel, "%d", &mode);

			DI_CM_SetStandbyMode(mode);

		}
		else
		{
			CMD_Printf("ERR! Bad Comman Iput. \n");
		}
	}
	else if(szCmd!=NULL && VK_strcmp(szCmd,"dssignalinfo")==0)
	{
		HINT32 nDsId = 0;
		DI_CM_DsSignalQuality_t	stCmDsInfo;

		char *szChannelId=(char *)CMD_NextArg((char **)&szArgs);

		if (szChannelId!=NULL)
		{
			if (READABLE_IN_DEC(szChannelId, nDsId) != 1 )
			{
				CMD_Printf("ERR! Downstream Channe ID is missed!\n");
				return CMD_ERR;
            }

			DI_CM_GetDsSignalInfo(nDsId, &stCmDsInfo);
			iResult = CMD_OK;
		}
		else
		{
			CMD_Printf("ERR! Bad Comman Iput. \n");
		}
	}
	else if(szCmd!=NULL && VK_strcmp(szCmd,"ussignalinfo")==0)
	{
		HINT32 nUsId = 0;
		DI_CM_UsSignalQuality_t	stCmUsInfo;

		char *szChannelId=(char *)CMD_NextArg((char **)&szArgs);

		if (szChannelId!=NULL)
		{
			if (READABLE_IN_DEC(szChannelId, nUsId) != 1 )
			{
				CMD_Printf("ERR! Upstream Channe ID is missed!\n");
				return CMD_ERR;
            }

			DI_CM_GetUsSignalInfo(nUsId, &stCmUsInfo);
			iResult = CMD_OK;
		}
		else
		{
			CMD_Printf("ERR! Bad Comman Iput. \n");
		}
	}
	else if(szCmd!=NULL && VK_strcmp(szCmd,"rebootcbreg")==0)
	{
		iResult = DI_SYSTEM_RegisterEventNotify(DI_SYS_EVENT_CM_REBOOT, P_CM_CbCheckReboot);
	}
	else
	{
		CMD_Printf("ERR! Bad Comman Iput. \n");
	}

	return iResult;
}
#endif

/*******************************************************************************
 *	FUNCTION NAME	: CMD_HwTest_Parser_CmSetLogLevel
 *	DESCRIPTION   	: DI Channel Debug Log level 설정
 * 	INPUT         	: level
 *	OUTPUT        	: NONE
 *	RETURN        	: NONE
 *	REMARK        	: NONE
 *******************************************************************************/
int CMD_HwTest_Parser_CmSetLogLevel(void *szArgs)
{
	int iResult=CMD_ERR;
	int nLevel = 0;

	char *szCmd=(char *)CMD_NextArg((char **)&szArgs);

	if (szCmd != NULL)
	{
        if ( ( READABLE_IN_DEC(szCmd, nLevel) != 1 ) )
        {
                return 1;
        }

		CMD_Printf("[CMD_HwTest_Parser_CmSetLogLevel] nLevel = %d  \n", nLevel);
		show_di_cm = nLevel;
		iResult=CMD_OK;
	}
	else
	{
		CMD_Printf("ERR! Bad Comman Iput. \n");
	}

	return iResult;
}



/*******************************************************************************
 *	FUNCTION NAME	: CMD_HwTest_Parser_SnmpSetLogLevel
 *	DESCRIPTION   	: DI Channel Debug Log level 설정
 * 	INPUT         	: level
 *	OUTPUT        	: NONE
 *	RETURN        	: NONE
 *	REMARK        	: NONE
 *******************************************************************************/
int CMD_HwTest_Parser_SnmpSetLogLevel(void *szArgs)
{
	int iResult=CMD_ERR;
	int nLevel = 0;

	char *szCmd=(char *)CMD_NextArg((char **)&szArgs);

	if (szCmd != NULL)
	{
        if ( ( READABLE_IN_DEC(szCmd, nLevel) != 1 ) )
        {
                return 1;
        }

		CMD_Printf("[CMD_HwTest_Parser_SnmpLogLevel] nLevel = %d  \n", nLevel);
		show_di_snmp = nLevel;
		iResult=CMD_OK;
	}
	else
	{
		CMD_Printf("ERR! Bad Comman Iput. \n");
	}

	return iResult;
}


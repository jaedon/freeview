#include "di_err.h"
#include "vkernel.h"
#include "ci.h"
#include "di_rtc.h"

#define MAX_CASYSID_CNT 100
typedef struct _camInfo
{
	unsigned char ucIsInitialized;
	unsigned char ModuleType;
	unsigned char ModuleManufacturer[41];
	unsigned char ucCaSysIdcount;
	unsigned short usCaSysId[MAX_CASYSID_CNT];
	unsigned char ucCaSysIdName[MAX_CASYSID_CNT][20];
}CAMINFO;

typedef struct {
	unsigned short * pusCaSysId;
	unsigned char ucCaSysIdCnt;
} CI_CAS_INST;

typedef enum eCAI_CAS_ID
{
	CAI_CAS_MG		= 0x0100,
	CAI_CAS_VA		= 0x0500,
	CAI_CAS_IR		= 0x0600,
	CAI_CAS_CX		= 0x0B00,
	CAI_CAS_CR		= 0x0D00,
	CAI_CAS_NA		= 0x1800,
	CAI_CAS_ND		= 0x0900,	// ???
	CAI_CAS_NONE	= 0xffff
} CAI_CAS_ID;

typedef struct _CA_SYS_ID_LIST
{
	unsigned short caSysIdmin;
	unsigned short caSysIdmax;
	unsigned char  caSysIdName[20];
}CA_SYS_ID_LIST;


extern HUINT32 PAL_CI_Init (void);
extern CI_CAS_INST *CiCas_GetCaSysID(CI_SLOT_ID);
extern unsigned long VK_CLOCK_SetTime(VK_CLOCK_SET_MODE, unsigned long);
extern CI_APP_INFO * CiAi_GetAppInfo(CI_SLOT_ID);

CA_SYS_ID_LIST caLIST[] =
{
	{0x0100, 0x01FF, "MediaGuard\0"},
	{0x0500, 0x05FF, "Viacess\0"},
	{0x0600, 0x06FF, "Irdeto\0"},
	{0x0900, 0x09FF, "NDS\0"},
	{0x0B00, 0x0BFF, "Conax\0"},
	{0x0D00, 0x0DFF, "CryptoWorks\0"},
	{0x1800, 0x18FF, "NagraVision\0"},
	{0x4a10, 0x4a1f, "Easycas\0"},
	{0x4a20, 0x4a2f, "AlphaCrypt\0"},
	{0x4ad6, 0x4ad7, "SKTelecom\0"},
	{0xFFFF, 0xFFFF, "Unknown\0"}
};

extern CAMINFO camInfo;

unsigned char *P_GetCasName(unsigned short caSysID)
{
	int i, listsize;
	unsigned char *pName=NULL;
	listsize = sizeof(caLIST)/sizeof(CA_SYS_ID_LIST);
	for(i=0; i<listsize; i++)
	{
		if(caLIST[i].caSysIdmin == 0xFFFF && caLIST[i].caSysIdmax == 0xFFFF)
		{
			pName = caLIST[i].caSysIdName;
			break;
		}
		else if(caLIST[i].caSysIdmin <= caSysID && caLIST[i].caSysIdmax >= caSysID)
		{
			pName = caLIST[i].caSysIdName;
			break;
		}
	}
	return pName;
}

void CI_HWTEST_Init()
{
	DI_ERR_CODE nErr;
	unsigned int		utRtcTime;

	nErr = DI_RTC_GetCurrentTime(&utRtcTime);
	if ( nErr != DI_ERR_OK )
	{
		//Failed setting time.
	}
	VK_CLOCK_SetTime(VK_CLOCK_SETBY_MICOM, utRtcTime);
	PAL_CI_Init ();
	CI_Init();
}

 void CI_HWTEST_AddAppInfo()
{
	CI_APP_INFO *appInfo = NULL;
	unsigned short slotid = 0;
	appInfo = CiAi_GetAppInfo(slotid);
	camInfo.ModuleType = appInfo->ucIsInitialized;
	VK_memcpy(camInfo.ModuleManufacturer, appInfo->szText, 41);
	camInfo.ucIsInitialized = appInfo->ucIsInitialized;
}

  void CI_HWTEST_DeleteAppInfo()
{
	camInfo.ModuleType = 0;
	VK_memset(camInfo.ModuleManufacturer, 0x0, 41);
	camInfo.ucIsInitialized = 0;
}

void CI_HWTEST_AddCaSysID()
{
	int i;
	unsigned short slotid = 0;
	CI_CAS_INST *casysId = NULL;
	casysId = CiCas_GetCaSysID(slotid);
	if(casysId->ucCaSysIdCnt > MAX_CASYSID_CNT)
	{
		casysId->ucCaSysIdCnt = MAX_CASYSID_CNT;
	}
	for(i=0; i<casysId->ucCaSysIdCnt; i++)
	{
		camInfo.usCaSysId[i] =casysId->pusCaSysId[i];
		VK_memcpy(camInfo.ucCaSysIdName[i], P_GetCasName(casysId->pusCaSysId[i]), 20);
	}
	camInfo.ucCaSysIdcount = casysId->ucCaSysIdCnt;
}

void CI_HWTEST_DeleteCaSysID()
{
	int i;
	 camInfo.ucCaSysIdcount = 0;
	 for(i=0;i<MAX_CASYSID_CNT;i++)
 	{
		camInfo.usCaSysId[i] = 0;
		VK_memset(camInfo.ucCaSysIdName[i], 0x0, 20);
 	}
}

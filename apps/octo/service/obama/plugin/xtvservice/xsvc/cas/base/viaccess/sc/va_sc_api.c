/*
	Viaccess Smartcard API
		Driver API Manual ACS 3.0 - Doc. Ref. 2217, p.147
*/

/********************************************************************
*	Including Headers
********************************************************************/
#include "va_def.h"

#include "va_sc.h"
#include "va_sc_api.h"
#include "va_sc_adapt.h"
#include "vkernel.h"

#include "va_main.h"

#include <string.h>
/*******************************************************************/
/********************      Global Variables     ********************/
/*******************************************************************/

/*******************************************************************/
/********************      Defines         *************************/
/*******************************************************************/
//#define DEBUG_SC_CMD




typedef enum
{
	VA_SC_ACTION_INIT = 0,
	VA_SC_ACTION_RESET,
	VA_SC_ACTION_SEND,
	VA_SC_ACTION_NO
} VA_SC_ACTION_STATUS_ENUM;

#define VA_SC_MAX_RESET_RETRY			1		// reset fail��, 3������ retry

typedef struct {
	tVA_SC_State eState;
	HBOOL bLocked;

	VA_SC_ACTION_STATUS_ENUM eActionType;

	UINT8 ucPpsData;
	UINT8 ucProtocol;
	UINT8 ucAtrLength;
	UINT8 aucAtrBuffer[64];

	UINT32 ulRxLength;
	UINT8 aucRxBuffer[256 + 2]; // command
} VA_SC_Slot;



/********************************************************************
*	External variables & function prototypes
********************************************************************/

/********************************************************************
*	Static variables
********************************************************************/
static UINT32 sVaScQid;
static UINT32 sVaScTid;

static VA_SC_Slot *sVaScSlot;
/********************************************************************
*	Static functions Prototypes
*	The following functions will be used as inner functions
********************************************************************/
static void VA_SC_Destroy();
static void VA_SC_Task ( void * args );
static void VA_SC_InsertProc(UINT32 ulScSlot);
static void VA_SC_ExtractProc(UINT32 ulScSlot);
static void VA_SC_ResetProc(UINT32 ulScSlot, UINT8 ucPpsValue);
static void VA_SC_SendCommandProc(UINT32 ulScSlot, UINT32 ulCommandSize, UINT8 *pCommand);

UINT32 VA_SC_GetMsgQueueId()
{
	return sVaScQid;
}

/*
	initialize variables and start task
*/
void VA_SC_Init()
{
	int nErr;
	int i;

	HxLOG_Print("[Enter]\n");

	sVaScSlot = VA_MEM_Alloc(sizeof(VA_SC_Slot) * VA_SC_SLOT_NUM);
	VA_ASSERT(sVaScSlot != NULL);

	for (i = 0; i < VA_SC_SLOT_NUM; i++)
	{
		sVaScSlot[i].eState = eCARD_EXTRACTED;
		sVaScSlot[i].bLocked = FALSE;
		sVaScSlot[i].eActionType = VA_SC_ACTION_INIT;
	}

	VA_CreateMessageQueue(VA_SC_QUEUE_NAME, &sVaScQid);

	nErr = VK_TASK_Create(VA_SC_Task, VA_SC_TASK_PRIORITY, VA_SC_TASK_STACK_SIZE, VA_SC_TASK_NAME, NULL, &sVaScTid, 0);
	if (nErr)
	{
		HxLOG_Error("failed to create task (%d)\n", nErr);
		VA_ASSERT(nErr == VK_OK);
	}

	nErr = VK_TASK_Start(sVaScTid);
	if (nErr)
	{
		HxLOG_Error("failed to start task (%d)\n", nErr);
		VA_ASSERT(nErr == VK_OK);
	}

	HxLOG_Print("[Exit]\n");
}

void VA_SC_ResetByHost(UINT32 ulScSlot) // ����Ʈī�� Ż���� ���ķ���Ʈ �ϴ� �뵵�� �Լ���. ���������� ����� ��!
{
	HUINT32			msg[4];

	msg[0] = VA_SC_MSG_CARD_OUT;
	msg[1] = ulScSlot;
	VA_PostMessage(VA_SC_GetMsgQueueId(), msg);

	msg[0] = VA_SC_MSG_CARD_IN;
	msg[1] = ulScSlot;
	VA_PostMessage(VA_SC_GetMsgQueueId(), msg);
}

static void VA_SC_Destroy()
{
	int i;

	HxLOG_Print("[Enter]\n");

	for (i = 0; i < VA_SC_SLOT_NUM; i++)
	{
		VA_SC_Deactivate(i);
	}

	VA_MEM_Free(sVaScSlot);
	sVaScSlot = NULL;

	// destory message queue
	VA_MSG_Destroy(sVaScQid);

	HxLOG_Print("[Exit]\n");
}

static void VA_SC_Task ( void * args )
{
	UINT32 aulMsg[4];
	UINT32 ulMsgType;

	UINT8 ucPpsValue;
	UINT32 ulScSlot;

	UINT32 ulCommandSize;
	UINT8 *pCommand;

	HBOOL bWaitMsg = TRUE;

	HxLOG_Print("[Enter]\n");

	// initialize Smart Card Interface in DD layer
	VA_SC_Adapt_Init();

	while (bWaitMsg)
	{
		if (VA_GetMessage(sVaScQid, (void *)&aulMsg) == VK_OK)
		{
			ulMsgType = aulMsg[0];

			HxLOG_Info("[VA_SC_Task] MSG Received msg_type(%d)\n", ulMsgType);

			switch (ulMsgType)
			{
				case VA_SC_MSG_RESET:
					ulScSlot = aulMsg[1];
					ucPpsValue = (UINT8)aulMsg[2];
					VA_SC_ResetProc(ulScSlot, ucPpsValue);
					break;

				case VA_SC_MSG_CARD_OUT:
					ulScSlot = aulMsg[1];
					VA_SC_ExtractProc(ulScSlot);
					break;

				case VA_SC_MSG_CARD_IN:
					ulScSlot = aulMsg[1];
#if 1
					VA_SC_InsertProc(ulScSlot);
#else
{
	unsigned char m[] = {0xca, 0x24,  3,  0,  0xc,  0,  0,  0,  0,  0,  0, 0x12, 0x34,  0x0, 0x11, 0x22, 0x33};
	unsigned char atr[100], atrLen;
	int err;
	unsigned read[100];
	unsigned long readN = 20;

	err = DDI_SC_Reset(0, &atr, &atrLen);
	DDI_UART_Print("<RESET:%x>, %d, %02X %02X %02X..\n", err, atrLen, atr[0], atr[1], atr[2]);
//	DDI_SC_TransferData(unsigned char ucDeviceId, unsigned char * pucWriteBuf, unsigned long ulNumToWrite, unsigned char * pucReadBuf, unsigned long * pulNumRead)
	err = DDI_SC_TransferData(0, m, 17, read, &readN);
	DDI_UART_Print("<TRANS:%x>, %d, %02X %02X %02X..\n", err, readN, read[0], read[1], read[2]);
}
#endif
					break;

				case VA_SC_MSG_SEND:
					ulScSlot = aulMsg[1];
					ulCommandSize = aulMsg[2];
					pCommand = (UINT8 *)aulMsg[3];
					VA_SC_SendCommandProc(ulScSlot, ulCommandSize, pCommand);
					break;

				case VA_SC_MSG_SYSTEM_EXIT:
					bWaitMsg = FALSE;
					break;

				default:
					HxLOG_Error("Unknown Msg (%d)\n", ulMsgType);
					break;
			}
		}
	}

	// this task is going to be terminated
	VA_SC_Destroy();

	HxLOG_Print("[Exit]\n");
}

static void VA_SC_InsertProc(UINT32 ulScSlot)
{
	int nErr;

	HxLOG_Print("[Enter(slot:%d)]\n", ulScSlot);

	VA_ASSERT(ulScSlot < VA_SC_SLOT_NUM);

	if (sVaScSlot[ulScSlot].eState != eCARD_EXTRACTED)
	{
		HxLOG_Error("card already inserted (State:%d)\n", sVaScSlot[ulScSlot].eState);
	}

	sVaScSlot[ulScSlot].eState = eCARD_INSERTED;
	sVaScSlot[ulScSlot].bLocked = FALSE;
	sVaScSlot[ulScSlot].eActionType = VA_SC_ACTION_NO;

	nErr = VA_SC_CardInserted(ulScSlot);
	if (nErr != kVA_OK)
	{
		HxLOG_Error("VA_SC_CardInserted() error (%ld)\n", nErr);
	}

	HxLOG_Print("[Exit]\n");
}

static void VA_SC_ExtractProc(UINT32 ulScSlot)
{
	int nErr;

	HxLOG_Print("[Enter(slot:%d)]\n", ulScSlot);

	VA_ASSERT(ulScSlot < VA_SC_SLOT_NUM);

	if (sVaScSlot[ulScSlot].eState == eCARD_EXTRACTED)
	{
		HxLOG_Error("card already extracted\n");
	}

	if (sVaScSlot[ulScSlot].eActionType == VA_SC_ACTION_SEND)
	{
		HxLOG_Info("VA_SC_ACTION_SEND\n");

		sVaScSlot[ulScSlot].eActionType = VA_SC_ACTION_NO;

		nErr = VA_SC_CommandFailed(ulScSlot);
		if (nErr != kVA_OK)
		{
			HxLOG_Error("VA_SC_CommandFailed() error (%ld)\n", nErr);
		}
	}

	if (sVaScSlot[ulScSlot].eActionType == VA_SC_ACTION_RESET)
	{
		HxLOG_Info("VA_SC_ACTION_RESET\n");

		sVaScSlot[ulScSlot].eActionType = VA_SC_ACTION_NO;

		nErr = VA_SC_ResetFailed(ulScSlot);
		if (nErr != kVA_OK)
		{
			HxLOG_Error("VA_SC_ResetFailed() error (%ld)\n", nErr);
		}
	}

	sVaScSlot[ulScSlot].eState = eCARD_EXTRACTED;

	nErr = VA_SC_CardExtracted(ulScSlot);
	if (nErr != kVA_OK)
	{
		HxLOG_Error("VA_SC_CardExtracted() error (%ld)\n", nErr);
	}

	HxLOG_Print("[Exit]\n");
}

static void VA_SC_ResetProc(UINT32 ulScSlot, UINT8 ucPpsValue)
{
	int nErr, nRetry = 0;
	HUINT32 ulAtrLength;

	HxLOG_Print("[Enter(slot:%d,PPS:%x)]\n", ulScSlot, ucPpsValue);

//	VA_SC_Activate(ulScSlot);

VA_SC_Retry:

	// ���� pps ����� �������� �� ������ �����ؾ� ��.
	sVaScSlot[ulScSlot].ucPpsData = 0;

	nErr = VA_SC_Adapt_Reset(ulScSlot, sVaScSlot[ulScSlot].aucAtrBuffer, &ulAtrLength,
								&sVaScSlot[ulScSlot].ucProtocol, ucPpsValue, &sVaScSlot[ulScSlot].ucPpsData);
	if (nErr)
	{
		#ifdef CONFIG_MW_CAS_VIACCESS_DRIVER_TEST // VA driver test �� abnoral test case �ÿ� �ǵ��� ���� ��Ȳ�� ���ʿ��ϰ� �α� ���� �ʵ���...
		HxLOG_Info("VA_SC_Adapt_Reset() error (%d)\n", nErr);
		#else
		HxLOG_Error("VA_SC_Adapt_Reset() error (%d)\n", nErr);
		#endif

		if (sVaScSlot[ulScSlot].eActionType == VA_SC_ACTION_NO)
		{
			// when the smartcard is extracted, action type is cleared
			HxLOG_Info("aborted\n");
		}
		else
		{
			//ATR ������ ���� ���ϴ��� SC MGR�� valid ���¸� �뺸�ؾ� ��.
			//���� CAS MGR�� CAS Info�� ������Ʈ �ؾ� boot�� SC �̺�Ʈ ó���� ��������.
			//## SC Ready�� �Űܼ� ó���ϵ��� ���� //swlee3
			//VA_SC_Adapt_ValidCard(ulScSlot, FALSE, eDxCAS_GROUPID_VA, VA_MAIN_ACS_NUM);

			if (nRetry++ < VA_SC_MAX_RESET_RETRY)
			{
				VA_SC_Deactivate(ulScSlot);
				VK_TASK_Sleep(1000);
				if (sVaScSlot[ulScSlot].eActionType != VA_SC_ACTION_NO)
					goto VA_SC_Retry;
			}

			sVaScSlot[ulScSlot].eActionType = VA_SC_ACTION_NO;
			nErr = VA_SC_ResetFailed(ulScSlot);
			if (nErr != kVA_OK)
				HxLOG_Error("VA_SC_ResetFailed() error (%d)\n", nErr);
		}
	}
	else
	{
		sVaScSlot[ulScSlot].ucAtrLength = (UINT8)ulAtrLength;

		//## SC manager ���� card �� valid ���� �˸���. ���� ���� cas���� smartcard ready�� ���� ������ �Űܶ�...
		//## �ϴ� session ������ 1����.. ==> acs ���� ��ŭ����..
		//## SC Ready�� �Űܼ� ó���ϵ��� ���� //swlee3
		//VA_SC_Adapt_ValidCard(ulScSlot, TRUE, eDxCAS_GROUPID_VA, VA_MAIN_ACS_NUM);

		sVaScSlot[ulScSlot].eActionType = VA_SC_ACTION_NO;
		sVaScSlot[ulScSlot].eState = eCARD_READY;

		nErr = VA_SC_ResetDone(ulScSlot, sVaScSlot[ulScSlot].ucAtrLength, sVaScSlot[ulScSlot].aucAtrBuffer, sVaScSlot[ulScSlot].ucPpsData);
		if (nErr != kVA_OK)
			HxLOG_Error("VA_SC_ResetDone() error (%d)\n", nErr);
	}
	HxLOG_Print("[Exit]\n");
}

static void VA_SC_SendCommandProc(UINT32 ulScSlot, UINT32 ulCommandSize, UINT8 *pCommand)
{
	int nErr;
	UINT8 aucShortCommand[5];

	HxLOG_Print("[Enter(slot:%d,  size:%d)]\n", ulScSlot, ulCommandSize);

#ifdef DEBUG_SC_CMD
{
	int i;
	HxLOG_Print("[CMD (%d) : ", ulCommandSize);
	for (i =0; i < ulCommandSize; i++)
	{
		HxLOG_Print("%02X ", pCommand[i]);
	}
	HxLOG_Print("END]\n");
}
#endif

	VA_ASSERT(ulScSlot < VA_SC_SLOT_NUM);

	if (sVaScSlot[ulScSlot].eState != eCARD_READY)
	{
		HxLOG_Error("card is not ready (State:%d)\n", sVaScSlot[ulScSlot].eState);

		sVaScSlot[ulScSlot].eActionType = VA_SC_ACTION_NO;

		nErr = VA_SC_CommandFailed(ulScSlot);
		if (nErr != kVA_OK)
			HxLOG_Error("VA_SC_CommandFailed() error (%d)\n", nErr);

		return;
	}

	if (ulCommandSize == 5)
	{
		// read command

		if (pCommand[4] == 0)
			sVaScSlot[ulScSlot].ulRxLength = 256;	// read byte = 0 --> it means 256bytes
		else
			sVaScSlot[ulScSlot].ulRxLength = pCommand[4];

		nErr = VA_SC_Adapt_Read(ulScSlot, pCommand, sVaScSlot[ulScSlot].aucRxBuffer, (HUINT32 *)&sVaScSlot[ulScSlot].ulRxLength);
	}
	else
	{
		// write command

		if (ulCommandSize == 4)
		{
			VA_memcpy(aucShortCommand, pCommand, ulCommandSize);
			aucShortCommand[4] = 0;
			sVaScSlot[ulScSlot].ulRxLength = 0;
			nErr = VA_SC_Adapt_Write(ulScSlot, aucShortCommand, 5, sVaScSlot[ulScSlot].aucRxBuffer, (HUINT32 *)&sVaScSlot[ulScSlot].ulRxLength);
		}
		else
		{
			sVaScSlot[ulScSlot].ulRxLength = 0;
			nErr = VA_SC_Adapt_Write(ulScSlot, pCommand, ulCommandSize, sVaScSlot[ulScSlot].aucRxBuffer, (HUINT32 *)&sVaScSlot[ulScSlot].ulRxLength);
		}
	}

#if 1 // VHDR-3000S ���� ���� (branch rev. 36768) �� �ݿ��� ���ε� industrial test �׸� �� LABCOM_SUB_1002_ECM-SI �׽�Ʈ�ÿ� ī�� ���� �� ���⿡ �ɷ��� ���� ī�� ����� �ȵǴ� ������ �߻��Ͽ� ������. -> �����������ϴ� �ٽ� �����.
	if (nErr)
	{
		// �̺κ��� dd�ʿ��� ����ó���� ��� ���ִ��Ŀ� ���� �޶��� �� �ִ�.
		// 9X�� error�� �ö���� ���� ���̰�,
		// 6X �ܴ̿� ������ error
		if (sVaScSlot[ulScSlot].ulRxLength == 2 && (sVaScSlot[ulScSlot].aucRxBuffer[0] & 0xF0) == 0x60)
		{
				HxLOG_Error("[%d] transfer failure ignored (%02X)\n", ulCommandSize, sVaScSlot[ulScSlot].aucRxBuffer[0]);
				nErr = ERR_OK;
			}
		}
#endif

	/*
		VA_SC_CommandFailed() :

		This function is called to indicate that a smartcard command has failed due to a communication problem with the smartcard.
		Reasons for this can be either an extracted smartcard or a timeout occurrence during the processing of the smartcard command.
		Error return values :
			kVA_INVALID_PARAMETER : If dwScSlot is out of range.
			kVA_UNEXPECTED_CALLBACK : If Viaccess did not require a send command operation.

		VA_SC_CommandDone() :

		This function is called to indicate that a given smartcard command has been successfully processed.
		Error return values :
			kVA_INVALID_PARAMETER : If dwScSlot is out of range or if pReturn equals NULL.
			kVA_UNEXPECTED_CALLBACK : If Viaccess did not require a send command operation.

		���� VHDR-3000S ���� ���װ� �׿� ���õ� DI �� ������ ������ ���� �����Ǿ�� ��.

		1. Device driver �� :

			1) DI ������ ��� �� ī�尡 ���ŵǾ� ī����� ����� �ȵǴ� ��Ȳ�̳� ī�忡�� ���� �ð� ������ ���� ��쿡�� error �� ����.
			2) SW ���� valid �ϸ鼭 ��� ������ �߻��� ��찡 �־�� �ȵ�. ���� ��� 6X XX ��� ���� �����ϸ� �װ� ��� �󿡴� ������ �����ٴ� ���� �ǹ��ؾ߸� ��.
			3) SW ���� invalid �� ��쿡�� error �� ������ ��� ��. ���⼭ invalid �ϴٴ� ���� VA_SC_CommandDone() ���� ������ �����Ͱ� ���ų� �߸� �Ǿ����� �ǹ��� (SW �� 6X XX �� ���� valid �� ����).

		2. Viaccess SC porting driver �� :

			1) VA_SC_CommandDone() ȣ��� Viaccess library �� ���� �����ϴ� ��쿡 ���� �ڵ鸵 �߰� : DI �ʿ��� ���� ������ �������൵ ���޵� read data �� SW �� �߸��� ��찡 ���� �� �����Ƿ� �ڵ鸵 �ʿ���.
			2) VA_SC_CommandFailed() ȣ��� Viaccess library �� ���� �����ϴ� ��쿡 ���ؼ��� �� ��ü�� �̹� ��� ������ �߻������� Viaccess library �� �˸� ���̴ϱ� ���� �ڵ鸵�� ���ʿ��� ��.
	*/

	if (nErr)
	{
		if (sVaScSlot[ulScSlot].eActionType == VA_SC_ACTION_NO)
		{
			// when the smartcard is extracted, action type is cleared
			HxLOG_Info("[%d] aborted\n", ulCommandSize);
		}
		else
		{
			sVaScSlot[ulScSlot].eActionType = VA_SC_ACTION_NO;
			nErr = VA_SC_CommandFailed(ulScSlot);
			if (nErr != kVA_OK)
				HxLOG_Error("[%d] VA_SC_CommandFailed() error (%d)\n", ulCommandSize, nErr);
		}

		return;
	}

	sVaScSlot[ulScSlot].eActionType = VA_SC_ACTION_NO;

	nErr = VA_SC_CommandDone(ulScSlot, sVaScSlot[ulScSlot].ulRxLength, sVaScSlot[ulScSlot].aucRxBuffer);
	if (nErr != kVA_OK)
	{
		HxLOG_Error("[%d] VA_SC_CommandDone() error (%d) - %d, %02X\n", ulCommandSize, nErr, sVaScSlot[ulScSlot].ulRxLength, sVaScSlot[ulScSlot].aucRxBuffer[0]);
		VA_SC_ResetByHost(ulScSlot);
	}

	HxLOG_Print("[Exit]\n");
}

/*--------------------------------------------------------------------------------------
				VIACCESS ACS 3.0 API
--------------------------------------------------------------------------------------*/

/*
	VA_SC_Reset
		reset a given smartcard
	arguments
		dwScSlot
		uiPpsNegotiationValue: 4bit = F, 4bit = D
	return
		kVA_OK
		kVA_INVALID_PARAMETER
		kVA_RESOURCE_BUSY
		kVA_INSERT_CARD_FIRST
*/
INT VA_SC_Reset(DWORD dwScSlot, UINT8 uiPpsNegotiationValue)
{
	int nIdx;
	UINT32 aulMsg[4];

	HxLOG_Print("[Enter(slot:%d,  pps:%d)]\n", dwScSlot, uiPpsNegotiationValue);

	nIdx = (int)dwScSlot;

	// wrong dwScSlot
	VA_SC_CheckSlot(nIdx);

	// no smartcard
	if (sVaScSlot[nIdx].eState == eCARD_EXTRACTED)
	{
		HxLOG_Info("no smartcard\n");
		return kVA_INSERT_CARD_FIRST;
	}

	// reset command is already in progress
	if (sVaScSlot[nIdx].eActionType == VA_SC_ACTION_RESET)
	{
		HxLOG_Error("reset command is already in progress\n");
		return kVA_RESOURCE_BUSY;
	}

	// request to reset a smartcard
	// in another task, VA_SC_ResetDone() or VA_SC_ResetFailed() will be called
	sVaScSlot[nIdx].eActionType = VA_SC_ACTION_RESET;

	aulMsg[0] = VA_SC_MSG_RESET;
	aulMsg[1] = dwScSlot;
	aulMsg[2] = (UINT32)uiPpsNegotiationValue;
	VA_PostMessage(sVaScQid, aulMsg);

	HxLOG_Print("[Exit]\n");
	return kVA_OK;
}

/*
	VA_SC_GetState
		get information on the state of a given smartcard
	arguments
		dwScSlot
		pScState: allocated by VA
	return
		kVA_OK
		kVA_INVALID_PARAMETER
*/
INT VA_SC_GetState(DWORD dwScSlot, tVA_SC_State *pScState)
{
	int nIdx;

	HxLOG_Print("[Enter]\n");

	nIdx = (int)dwScSlot;

	// wrong dwScSlot
	VA_SC_CheckSlot(nIdx);

	if (pScState == NULL)
	{
		#ifdef CONFIG_MW_CAS_VIACCESS_DRIVER_TEST // VA driver test �� abnoral test case �ÿ� �ǵ��� ���� ��Ȳ�� ���ʿ��ϰ� �α� ���� �ʵ���...
		HxLOG_Info("pScState = NULL\n");
		#else
		HxLOG_Error("pScState = NULL\n");
		#endif
		return kVA_INVALID_PARAMETER;
	}

	// get state
	*pScState = sVaScSlot[nIdx].eState;
	HxLOG_Info("[VA_SC_GetState] slot:%ld, state:%d\n", dwScSlot, sVaScSlot[nIdx].eState);

	HxLOG_Print("[Exit]\n");
	return kVA_OK;
}

/*
	VA_SC_SendCommand
		send a command to a given smartcard
	arguments
		dwScSlot
		uiCommandSize
		pCommand
	return
		kVA_OK
		kVA_INVALID_PARAMETER
		kVA_RESOURCE_BUSY
		kVA_INSERT_CARD_FIRST
		kVA_RESET_CARD_FIRST
*/
INT VA_SC_SendCommand(DWORD dwScSlot, UINT32 uiCommandSize, BYTE *pCommand)
{
	int nIdx;
	UINT32 aulMsg[4];

	HxLOG_Print("[Enter(slot:%d,  size:%d)]\n", dwScSlot, uiCommandSize);

	nIdx = (int)dwScSlot;

	// wrong dwScSlot
	VA_SC_CheckSlot(nIdx);

	if (pCommand == NULL || uiCommandSize < 4)
	{
		#ifdef CONFIG_MW_CAS_VIACCESS_DRIVER_TEST // VA driver test �� abnoral test case �ÿ� �ǵ��� ���� ��Ȳ�� ���ʿ��ϰ� �α� ���� �ʵ���...
		HxLOG_Info("wrong parameter (%d, %x)\n", uiCommandSize, pCommand);
		#else
		HxLOG_Error("wrong parameter (%d, %x)\n", uiCommandSize, pCommand);
		#endif
		return kVA_INVALID_PARAMETER;
	}

	// no smartcard
	if (sVaScSlot[nIdx].eState == eCARD_EXTRACTED)
	{
		HxLOG_Info("no smartcard\n");
		return kVA_INSERT_CARD_FIRST;
	}

	// smartcard is not reset
	if (sVaScSlot[nIdx].eState == eCARD_INSERTED)
	{
		#ifdef CONFIG_MW_CAS_VIACCESS_DRIVER_TEST // VA driver test �� abnoral test case �ÿ� �ǵ��� ���� ��Ȳ�� ���ʿ��ϰ� �α� ���� �ʵ���...
		HxLOG_Info("smartcard is not reset\n");
		#else
		HxLOG_Error("smartcard is not reset\n");
		#endif
		return kVA_RESET_CARD_FIRST;
	}

	// send command is already in progress
	if (sVaScSlot[nIdx].eActionType == VA_SC_ACTION_SEND)
	{
		HxLOG_Error("send command is already in progress\n");
		return kVA_RESOURCE_BUSY;
	}

	// request to send a command to a smartcard
	// in another task, VA_SC_CommandDone() or VA_SC_CommandFailed() will be called
	sVaScSlot[nIdx].eActionType = VA_SC_ACTION_SEND;

	aulMsg[0] = VA_SC_MSG_SEND;
	aulMsg[1] = dwScSlot;
	aulMsg[2] = uiCommandSize;
	aulMsg[3] = (UINT32)pCommand;
	VA_PostMessage(sVaScQid, aulMsg);

	HxLOG_Print("[Exit]\n");
	return kVA_OK;
}

/*
	VA_SC_Activate
		activate a given smartcard (for ISO compatibility test only)
	arguments
		dwScSlot
	return
		kVA_OK
		kVA_INVALID_PARAMETER
		kVA_NOT_IMPLEMENTED
		kVA_INSERT_CARD_FIRST
*/
INT VA_SC_Activate(DWORD dwScSlot)
{
#ifdef CONFIG_MW_CAS_VIACCESS_DRIVER_TEST
	int nIdx;

	HxLOG_Print("[Enter(slot:%d)]\n", dwScSlot);

	nIdx = (int)dwScSlot;

	// wrong dwScSlot
	VA_SC_CheckSlot(nIdx);

	// no smartcard
	if (sVaScSlot[nIdx].eState == eCARD_EXTRACTED)
	{
		HxLOG_Info("no smartcard\n");
		return kVA_INSERT_CARD_FIRST;
	}

	// activate a smartcard
	VA_SC_Adapt_Activate(nIdx);

	HxLOG_Print("[Exit]\n");
	return kVA_OK;
#else
	HxLOG_Print("[kVA_NOT_IMPLEMENTED]\n");
	return kVA_NOT_IMPLEMENTED;
#endif
}

/*
	VA_SC_Deactivate
		deactivate a given smartcard (for ISO compatibility test only)
	arguments
		dwScSlot
	return
		kVA_OK
		kVA_INVALID_PARAMETER
		kVA_NOT_IMPLEMENTED
		kVA_INSERT_CARD_FIRST
*/
INT VA_SC_Deactivate(DWORD dwScSlot)
{
	int nIdx;

	HxLOG_Print("[Enter(slot:%d)]\n", dwScSlot);

	nIdx = (int)dwScSlot;

	// wrong dwScSlot
	VA_SC_CheckSlot(nIdx);

	// no smartcard
	if (sVaScSlot[nIdx].eState == eCARD_EXTRACTED)
	{
		HxLOG_Info("no smartcard\n");
		return kVA_INSERT_CARD_FIRST;
	}

	// deactivate a smartcard
	VA_SC_Adapt_Deactivate(nIdx);

	HxLOG_Print("[Exit]\n");
	return kVA_OK;
}

/*
	VA_SC_Lock
		lock a smartcard
	arguments
		dwScSlot
	return
		kVA_OK
		kVA_INVALID_PARAMETER
		kVA_RESOURCE_ALREADY_LOCKED
		kVA_INSERT_CARD_FIRST
*/
INT VA_SC_Lock(DWORD dwScSlot)
{
	int nIdx;

	HxLOG_Print("[Enter(slot:%d)]\n", dwScSlot);

	nIdx = (int)dwScSlot;

	// wrong dwScSlot
	VA_SC_CheckSlot(nIdx);

	// no smartcard
	if (sVaScSlot[nIdx].eState == eCARD_EXTRACTED)
	{
		HxLOG_Info("no smartcard\n");
		return kVA_INSERT_CARD_FIRST;
	}

	// smartcard is already locked
	if (sVaScSlot[nIdx].bLocked == TRUE)
	{
		HxLOG_Info("smartcard is already locked\n");
		return kVA_RESOURCE_ALREADY_LOCKED;
	}

	// lock a smartcard
	sVaScSlot[nIdx].bLocked = TRUE;

	HxLOG_Print("[Exit]\n");
	return kVA_OK;
}

/*
	VA_SC_Unlock
		unlock a smartcard
	arguments
		dwScSlot
	return
		kVA_OK
		kVA_INVALID_PARAMETER
		kVA_RESOURCE_NOT_LOCKED
		kVA_INSERT_CARD_FIRST
*/
INT VA_SC_Unlock(DWORD dwScSlot)
{
	int nIdx;

	HxLOG_Print("[Enter(slot:%d)]\n", dwScSlot);

	nIdx = (int)dwScSlot;

	// wrong dwScSlot
	VA_SC_CheckSlot(nIdx);

	// no smartcard
	if (sVaScSlot[nIdx].eState == eCARD_EXTRACTED)
	{
		HxLOG_Info("no smartcard\n");
		return kVA_INSERT_CARD_FIRST;
	}

	// smartcard is not locked
	if (sVaScSlot[nIdx].bLocked == FALSE)
	{
		HxLOG_Info("smartcard is not locked\n");
		return kVA_RESOURCE_NOT_LOCKED;
	}

	// lock a smartcard
	sVaScSlot[nIdx].bLocked = FALSE;

	HxLOG_Print("[Exit]\n");
	return kVA_OK;
}


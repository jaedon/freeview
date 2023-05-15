/*
	Viaccess Smartcard API
		Driver API Manual ACS 3.0 - Doc. Ref. 2217, p.147
*/

#ifndef VA_SC_API_H
#define VA_SC_API_H
#include <hlib.h>

void VA_SC_Init();
void VA_SC_ResetByHost(); // ����Ʈī�� Ż���� ���ķ���Ʈ �ϴ� �뵵�� �Լ���. ���������� ����� ��!

typedef enum
{
	VA_SC_MSG_INIT = 0,
	VA_SC_MSG_RESET,
	VA_SC_MSG_SEND,
	VA_SC_MSG_CARD_OUT,
	VA_SC_MSG_CARD_IN,
	VA_SC_MSG_SYSTEM_EXIT,
	VA_SC_MSG_ERROR
} VA_SC_MSG_TYPE_ENUM;

#define VA_SC_SLOT_NUM			1		// more than one smartcard reader is allowed, but here assume that STB has only one smartcard reader

#ifdef CONFIG_MW_CAS_VIACCESS_DRIVER_TEST // VA driver test �� abnoral test case �ÿ� �ǵ��� ���� ��Ȳ�� ���ʿ��ϰ� �α� ���� �ʵ���...
#define VA_SC_CheckSlot(a) do { if (a >= VA_SC_SLOT_NUM) { HxLOG_Info("wrong dwScSlot (%d)\n", a); return (kVA_INVALID_PARAMETER); } } while (0)
#else
#define VA_SC_CheckSlot(a) do { if (a >= VA_SC_SLOT_NUM) { HxLOG_Info("wrong dwScSlot (%d)\n", a); return (kVA_INVALID_PARAMETER); } } while (0)
#endif


#endif // VA_SC_API_H


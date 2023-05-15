/**
	@file	pal_ci.c
	@brief	pal_ci.c

	Description: PAL Layer											\n
	Module: PAL / CI 												\n

	Copyright (c) 2008 HUMAX Co., Ltd.								\n
	All rights reserved.											\n
*/

/*******************************************************************/
/********************      Header Files    *************************/
/*******************************************************************/

#include <di_err.h>
#include <di_ci.h>
#include <di_demux.h>

#include <pal_ci.h>

/*******************************************************************/
/********************      Global Variables     ********************/
/*******************************************************************/


/*******************************************************************/
/********************      Defines         *************************/
/*******************************************************************/



/* PAL_CI_SetTsPath() ȣ��� ���� ������ TS path (bypass �Ǵ� pass-thru) �� ������ TS path ���� ��û�̸� DI ȣ���� ���� �ʵ��� �ϴ� define �� */
/* ��ε��İ� ���� demux path �� �����Ǵ� ��� ������� �� �� */
//#define CHECK_PREV_CI_TS_PATH

/**
  *	Conax������ CIv1�� �⺻������ forbidden interface�� �����Ѵ�
  *	�׷��� OP/CONAX�� ���ǵǸ� ��밡�ɹǷ� model config�� �߰��� ����Ѵ�
  *
  *	���� Conax�� CI+�� ���ÿ� ����ϴ� ���� CXHD-5100C �ϳ��̰�, CIv1�� ���� ������ ������.
**/
#if defined(CONFIG_MW_CAS_CONAX) && defined(CONFIG_MW_CI_PLUS)
#if defined(CONFIG_PROD_CXHD5100C)
/* CI+ ķ�� �ƴϸ� passthru �� ������� ���� (��, CI v1 descrambling ����) */
#define BLOCKING_CIv1_DESCRAMBLING
#endif
#endif

#ifdef CONFIG_MW_CI_USE_EXTERNAL_CONTROLLER
#define USE_MANAGE_TS_PATH
#endif

/*******************************************************************/
/********************      Typedef         *************************/
/*******************************************************************/

#ifdef USE_MANAGE_TS_PATH
typedef struct
{
	HUINT32				unTunerId;
	PalCi_Input_e		eInput;
	PalCi_Output_e		eOutput;
	PalCi_PathWay_e	ePathWay;
}ePAL_CI_PATHWAY_Instance_t;
#endif


/*******************************************************************/
/********************      Static Variables     ********************/
/*******************************************************************/

static unsigned short numCISlot = 0;
static PalCi_Path_e s_CiTsPath[PAL_MAX_CISLOT_NUMBER] = {0xff, 0xff};

#ifdef BLOCKING_CIv1_DESCRAMBLING
static PalCi_CamType_e  s_CamType[PAL_MAX_CISLOT_NUMBER] = {ePAL_CI_CI_V1, ePAL_CI_CI_V1};
#endif

#if defined(CONFIG_MW_CI_USE_EXTERNAL_CONTROLLER)
PalCi_PathWay_e	aeCurrentPathWay[PAL_MAX_CISLOT_NUMBER];
#endif

#ifdef USE_MANAGE_TS_PATH
static ePAL_CI_PATHWAY_Instance_t	s_astCiPathInstance[PAL_MAX_CISLOT_NUMBER];
#endif

/*******************************************************************/
/********************      Extern Functions   **********************/
/*******************************************************************/
// TODO: should implement on the di_ci_xxx and use DI_CI_XXX functions
#if defined(CONFIG_MW_CI_USE_EXTERNAL_CONTROLLER)
extern 	int DRV_CI_Open (unsigned char *device_name, unsigned short int *);
#else
extern 	int DRV_CI_Open (unsigned char *device_name);
#endif
extern 	int DRV_CI_Close (unsigned short usSlotId);
extern	int DRV_CI_Write (unsigned short usSlotId, unsigned short len, unsigned char *pbytes);
extern 	int DRV_CI_Read (unsigned short usSlotId, unsigned short len, unsigned char *pbytes);
extern	int DRV_CI_Ioctl (unsigned short usSlotId, int command, void *parg);
/*******************************************************************/
/********************      Functions       *************************/
/*******************************************************************/
#if defined(CONFIG_MW_CI_USE_EXTERNAL_CONTROLLER)
static DI_CI_INPUT pal_ci_ConvertPalToDiInput(PalCi_Input_e ePalInput)
{
	switch(ePalInput)
	{
		case ePAL_CI_LIVE0:							return DI_CI_LIVE0;
		case ePAL_CI_LIVE1:							return DI_CI_LIVE1;
		case ePAL_CI_PLAYBACK0:						return DI_CI_PLAYBACK0;
		case ePAL_CI_PLAYBACK1:						return DI_CI_PLAYBACK1;
		default:									return DI_CI_INPUT_MAX;
	}
}

static DI_CI_OUTPUT pal_ci_ConvertPalToDiOutput(PalCi_Output_e ePalOutput)
{
	switch(ePalOutput)
	{
		case ePAL_CI_OUTPUT0:						return DI_CI_OUTPUT0;
		case ePAL_CI_OUTPUT1:						return DI_CI_OUTPUT1;
		default:									return DI_CI_OUTPUT_MAX;
	}
}

static DI_CI_OUTPUT pal_ci_ConvertPalToDiPathWay(PalCi_PathWay_e ePalPathWay)
{
	switch(ePalPathWay)
	{
		case ePAL_CI_PATHWAY_BYPASS:					return DI_CI_NORMAL_BYPASS;
		case ePAL_CI_PATHWAY_NORMAL_PASSTHROUGH:		return DI_CI_NORMAL_PASSTHROUGH;
		case ePAL_CI_PATHWAY_DAISYCHAIN_PASSTHROUGH:	return DI_CI_DAISYCHAIN_PASSTHROUGH;
		case ePAL_CI_PATHWAY_CUSTOM_PASSTHROUGH:		return DI_CI_CUSTOM_PASSTHROUGH;
		default:									return DI_CI_PATHWAY_MAX;
	}
}
#endif

static HERROR pal_ci_ConvertDiCiState2PalCiState(DI_CI_STATE eDiCiState, PalCi_State_e *pePalCiState)
{
	switch(eDiCiState)
	{
	case DI_CI_EXTRACTED:	*pePalCiState = ePAL_CI_EXTRACTED;	return ERR_OK;
	case DI_CI_INSERTED:	*pePalCiState = ePAL_CI_INSERTED;		return ERR_OK;
	case DI_CI_ENABLED:		*pePalCiState = ePAL_CI_INSERTED;		return ERR_OK;
	default: return ERR_FAIL;
	}
}

static HERROR pal_ci_ConvertDiCiConnectWay2PalCiConnectWay(DI_CI_CONNECT_WAY eDiCiConnectWay, PalCi_ConnectWay_e *pePalCiConnectWay)
{
	switch(eDiCiConnectWay)
	{
	case DI_CI_CONNECT_NORMAL:			*pePalCiConnectWay = ePAL_CI_CONNECT_NORMAL;		return ERR_OK;
	case DI_CI_CONNECT_DAISYCHAIN:		*pePalCiConnectWay = ePAL_CI_CONNECT_DAISYCHAIN;	return ERR_OK;
	case DI_CI_CONNECT_SPECIAL:			*pePalCiConnectWay = ePAL_CI_CONNECT_SPECIAL;		return ERR_OK;
	case DI_CI_CONNECT_PLAYBACK:		*pePalCiConnectWay = ePAL_CI_CONNECT_PLAYBACK;		return ERR_OK;
	default: return ERR_FAIL;
	}
}

HERROR PAL_CI_Init (void)
{
	numCISlot = DI_CI_GetNumberOfModule();

	if (numCISlot == 0 || numCISlot > PAL_MAX_CISLOT_NUMBER)
	{
		HxLOG_Error("[ERROR] PAL_CI_Init : wrong numCISlot %d\n", numCISlot);
		return ERR_FAIL;
	}

#ifdef USE_MANAGE_TS_PATH
{
	HINT32 i;
	for(i = 0 ; i < PAL_MAX_CISLOT_NUMBER ; i++)
	{
		s_astCiPathInstance[i].unTunerId	= -1;
		s_astCiPathInstance[i].eInput		= ePAL_CI_INPUT_MAX;
		s_astCiPathInstance[i].eOutput		= ePAL_CI_OUTPUT0 + i;
		s_astCiPathInstance[i].ePathWay		= ePAL_CI_PATHWAY_MAX;
	}
}
#endif

	return ERR_OK;
}

unsigned short PAL_CI_GetNumberOfModule (void) /* �����ϴ� CI ���� ���� ������ */
{
	return numCISlot;
}

unsigned short PAL_CI_GetMaxSlotNumber (void) /* OCTO�� ������ �� �ִ� �ִ� CI ���� ���� ������ */
{
	return PAL_MAX_CISLOT_NUMBER;
}

HERROR PAL_CI_RegisterCallback (void (*CiCallbackFunc)(CI_SLOT_ID usSlotId, unsigned short flag)) /* ķ�� ���¿� ���� CI ����̹��κ��� ȣ��Ǳ⸦ ���ϴ� �ݹ��� ����ϱ� ���� �Լ� */
{
	return DI_CI_RegisterCallback(CiCallbackFunc);
}

PalCi_State_e PAL_CI_CheckModule (CI_SLOT_ID usSlotId) /* ķ ���� ������ ������ */
{
	DI_CI_STATE state;

	if (usSlotId >= numCISlot)
	{
		HxLOG_Error("[ERROR] PAL_CI_CheckModule : wrong usSlotId %d\n", usSlotId);
		return ePAL_CI_EXTRACTED;
	}

	/* DI�κ��ʹ� DI_CI_EXTRACTED, DI_CI_INSERTED �ۿ� üũ���� �ʵ��� �Ѵ� */

	state = DI_CI_CheckModule(usSlotId);
	switch (state)
	{
		case DI_CI_EXTRACTED :
			return ePAL_CI_EXTRACTED;

		case DI_CI_INSERTED :
		case DI_CI_ENABLED :
			return ePAL_CI_INSERTED;

		default :
			HxLOG_Error("[ERROR] PAL_CI_CheckModule : unknown state %d\n", state);
			return ePAL_CI_EXTRACTED;
	}
}

HERROR PAL_CI_ResetModule (CI_SLOT_ID usSlotId) /* ķ ����� ������ */
{
	DI_ERR_CODE err;

	HxLOG_Print("%s, SlotId(%d)\n", __FUNCTION__, usSlotId);

	if (usSlotId >= numCISlot)
	{
		HxLOG_Error("[ERROR] PAL_CI_ResetModule : wrong usSlotId %d\n", usSlotId);
		return ERR_FAIL;
	}

	err = DI_CI_ResetModule(usSlotId);
	if (err)
	{
		HxLOG_Error("[ERROR] PAL_CI_ResetModule : err %d\n", err);
		return ERR_FAIL;
	}

	return ERR_OK;
}

HERROR PAL_CI_SetTsPath (CI_SLOT_ID usSlotId, PalCi_Path_e path) /* bypass �Ǵ� pass-thru�� �����ϱ� ���� �Լ� */
{
	DI_ERR_CODE err;

	if (usSlotId >= numCISlot)
	{
		HxLOG_Error("[ERROR] PAL_CI_SetTsPath : wrong usSlotId %d\n", usSlotId);
		return ERR_FAIL;
	}

	HxLOG_Print("%s, SlotId(%d) : OldPathType(%x) -> NewPathType(%x)\n", __FUNCTION__, usSlotId, s_CiTsPath[usSlotId], path);

#ifdef BLOCKING_CIv1_DESCRAMBLING // CI+ ķ�� �ƴϸ� passthru �� ������� ���� (��, CI v1 descrambling ����)
	if (s_CamType[usSlotId] == ePAL_CI_CI_V1 && path == ePAL_CI_PASSTHROUGH)
	{
		HxLOG_Error("%s, SlotId(%d), not CI+ cam -> blocking\n", __FUNCTION__, usSlotId);
		return ERR_OK;
	}
#endif

#ifdef CHECK_PREV_CI_TS_PATH
	if (path == s_CiTsPath[usSlotId])
	{
		HxLOG_Error("%s, SlotId(%d), same PathType(%x) requested\n", __FUNCTION__, usSlotId, path);
		return ERR_OK;
	}
#endif

	if(path == ePAL_CI_BYPASS)
	{
		err = DI_CI_SetTsPath(usSlotId, path);
		if (err)
		{
			s_CiTsPath[usSlotId] = 0xff;

			HxLOG_Error("[ERROR] PAL_CI_SetTsPath : err %d\n", err);
			return ERR_FAIL;
		}
	}

	s_CiTsPath[usSlotId] = path;

	return ERR_OK;
}

HERROR	PAL_CI_SetPath(HUINT32 ulSrcId, HINT32 nSlotId, HUINT32 ulDemuxId)
{
	DI_ERR_CODE err = ERR_OK;

#ifdef CONFIG_DEBUG
	{
		DI_DEMUX_INPUTDEVICETYPE etProducer;
		HUINT32 ulInputDevice_id;
		DI_DEMUX_OUTPUTDEVICETYPE etConsumer;
		HUINT32 ulConsumerGroupId;
		HINT32 nCI_Id;

		if (DI_DEMUX_GetPath(ulDemuxId, &etProducer, &ulInputDevice_id, &etConsumer, &ulConsumerGroupId, &nCI_Id))
		{
			HxLOG_Error("[ERROR] PAL_CI_SetTsPath : DI_DEMUX_GetPath\n");
		}

		HxLOG_Print("%s, DmxId(%d) : Input(%d:%d), Output(%d:%d), CI(%d) -> Input(%d:%d), Output(%d:%d), CI(%d)\n",
			__FUNCTION__, ulDemuxId,
			etProducer, ulInputDevice_id, etConsumer, ulConsumerGroupId, nCI_Id,
			DI_DEMUX_INPUT_CURRENT, ulSrcId, DI_DEMUX_OUTPUT_CURRENT, 0, nSlotId);
	}
#endif

	err = DI_DEMUX_SetPath(ulDemuxId, DI_DEMUX_INPUT_CURRENT, ulSrcId, DI_DEMUX_OUTPUT_CURRENT, 0, nSlotId);

	return err;
}

HERROR PAL_CI_GetPath(HUINT32 ulDemuxId, HINT32 *pnCI_Id)
{
	DI_ERR_CODE				diErr;
	DI_DEMUX_INPUTDEVICETYPE	etProducer;
	HUINT32						ulInputDevice_id;
	DI_DEMUX_OUTPUTDEVICETYPE	etConsumer;
	HUINT32						ulConsumerGroupId;

	if(pnCI_Id == NULL)
	{
		HxLOG_Error("[ERROR] pnCI_Id is NULL\n");
		return ERR_FAIL;
	}

	diErr = DI_DEMUX_GetPath(ulDemuxId, &etProducer, &ulInputDevice_id, &etConsumer, &ulConsumerGroupId, pnCI_Id);
	if(diErr != DI_ERR_OK)
	{
		return ERR_FAIL;
	}

	return ERR_OK;
}

HERROR PAL_CI_SetCamType(CI_SLOT_ID usSlotId, PalCi_CamType_e eType)
{
	DI_CI_CAM_TYPE eDiType;

	if (usSlotId >= numCISlot)
	{
		HxLOG_Error("[ERROR] PAL_CI_GetTsPath : wrong usSlotId %d\n", usSlotId);
		return ERR_FAIL;
	}

#ifdef BLOCKING_CIv1_DESCRAMBLING
	s_CamType[usSlotId] = eType;
#endif

	if (eType == ePAL_CI_CI_PLUS)
	{
		eDiType = DI_CI_CI_PLUS;
	}
	else
	{
		eDiType = DI_CI_CI_V1;
	}

	DI_CI_SetCAMType(usSlotId, eDiType);

	return ERR_OK;
}

HINT32 PAL_CI_Open (HUINT8 *device_name)
{
#if defined(CONFIG_MW_CI_USE_EXTERNAL_CONTROLLER)
	HUINT16		usSlotId;

	if(DRV_CI_Open(device_name, &usSlotId) == 0)
	{
		return usSlotId;
	}

	return -1;
#else
	return DRV_CI_Open(device_name);
#endif
}

HINT32 PAL_CI_Close (HUINT16 usSlotId)
{
	return DRV_CI_Close(usSlotId);
}

HINT32 PAL_CI_Write (HUINT16 usSlotId, HUINT16 len, HUINT8 *pbytes)
{
#if defined(CONFIG_MW_CI_USE_EXTERNAL_CONTROLLER)
	return ERR_OK;
#else
	return DRV_CI_Write(usSlotId, len, pbytes);
#endif
}

HINT32 PAL_CI_Read (HUINT16 usSlotId, HUINT16 len, HUINT8 *pbytes)
{
#if defined(CONFIG_MW_CI_USE_EXTERNAL_CONTROLLER)
	return ERR_OK;
#else
	return DRV_CI_Read(usSlotId, len, pbytes);
#endif
}

HINT32 PAL_CI_Ioctl (HUINT16 usSlotId, HINT32 command, void *parg)
{
#if defined(CONFIG_MW_CI_USE_EXTERNAL_CONTROLLER)
	return ERR_OK;
#else
	return DRV_CI_Ioctl(usSlotId, command, parg);
#endif
}

HERROR PAL_CI_GetStatusCI(unsigned short usSlotId, PalCi_ConnectStatus_e *pstCIStatus)
{
	HERROR						hErr;
	DI_ERR_CODE				diErr;
	DI_CI_CONNECT_STATUS_t		stDiCiConnectStatus;

	if(pstCIStatus == NULL)
	{
		return ERR_FAIL;
	}

	diErr = DI_CI_GetStatusCI(usSlotId, &stDiCiConnectStatus);
	if(diErr != DI_ERR_OK)
	{
		HxLOG_Critical("\n");
		return ERR_FAIL;
	}

	hErr = pal_ci_ConvertDiCiState2PalCiState(stDiCiConnectStatus.connectStatus, &pstCIStatus->ePalCiState);
	if(hErr != ERR_OK)
	{
		HxLOG_Critical("\n");
		return ERR_FAIL;
	}

	hErr = pal_ci_ConvertDiCiConnectWay2PalCiConnectWay(stDiCiConnectStatus.connectWay, &pstCIStatus->ePalConnectWay);
	if(hErr != ERR_OK)
	{
		HxLOG_Critical("\n");
		return ERR_FAIL;
	}

	return ERR_OK;
}

#ifdef USE_DOUGLAS_PRIMITIVE_FUNC
HINT32 PAL_CI_ReadCIS (HUINT16 usSlotId, HUINT8 *pcis, HUINT16 *plen)
{
	DRV_CI_ReadCIS(usSlotId, pcis, plen);
}

HINT32 PAL_CI_WriteCOR (HUINT16 usSlotId)
{
	return DRV_CI_WriteCOR(usSlotId);
}
#endif

#if defined(CONFIG_MW_CI_USE_EXTERNAL_CONTROLLER)
HERROR PAL_CI_GetCurrentPathWay(HUINT32 unSlotId, PalCi_PathWay_e *pePalCiPathWay)
{
	if(unSlotId >= PAL_MAX_CISLOT_NUMBER)
	{
		HxLOG_Critical("\n");
		return ERR_FAIL;
	}

#ifdef USE_MANAGE_TS_PATH
	*pePalCiPathWay = s_astCiPathInstance[unSlotId].ePathWay;
#else
	*pePalCiPathWay = aeCurrentPathWay[unSlotId];
#endif
	return ERR_OK;
}

HERROR PAL_CI_SetExtendedTsPath(HUINT32 unTunerId, HUINT32 unSlotId, HUINT32 unDemuxId, PalCi_PathWay_e ePalCiPathWay)
{
	HBOOL						bIsUsedRemux = FALSE;
	HINT32						nCI_Id;
	HUINT32						ulInputDevice_id;
	HUINT32						unPlayBackId;
	HUINT32						ulConsumerGroupId;
	DI_ERR_CODE 				diErr;
	PalCi_Input_e				ePalCiInput = ePAL_CI_INPUT_MAX;
	PalCi_Output_e				ePalCiOutput = ePAL_CI_OUTPUT_MAX;
	DI_DEMUX_INPUTDEVICETYPE	etProducer;
	DI_DEMUX_OUTPUTDEVICETYPE	etConsumer;
	DI_CI_INPUT					eDiInput;
	DI_CI_OUTPUT				eDiOutput;
	DI_CI_PATHWAY				eDiPathWay;

	/*	slot id -1�� �����ͼ� path�� ���� ���� �����ϵ��� �����ʿ�.
	*/
	if(unSlotId >= PAL_MAX_CISLOT_NUMBER)
	{
		HxLOG_Critical("\n");
		return ERR_FAIL;
	}

#ifdef USE_MANAGE_TS_PATH
	// �ش� SLOT�� CAM�� ���ŵ� ��� PATH ������ �ʱ�ȭ �Ѵ�. (MW���� CAM EXTRACT�� BYPASS�� �������� �߰���)
	if(ePalCiPathWay == ePAL_CI_PATHWAY_BYPASS)
	{
		s_astCiPathInstance[unSlotId].unTunerId	= -1;
		s_astCiPathInstance[unSlotId].eInput	= ePAL_CI_INPUT_MAX;
		/*	CI MAX+ Output�� ��� PAL_CI_Init()�ÿ� default�� �Ҵ��� �Ǿ� ������
			���� TS PATH ������Ŀ� ���� Output�� ���Ҵ� �Ǵ� ������ �Ǿ� �ִ�.
			���� Output ���� �ʱ�ȭ�� �� ��� ������ �Ǵ� ���� �𸣴� ��Ȳ�� �����ϱ⿡ �ʱ�ȭ �Ǿ�� �ȵȴ�. */
		//s_astCiPathInstance[unSlotId].eOutput	= ePAL_CI_OUTPUT_MAX;
		s_astCiPathInstance[unSlotId].ePathWay	= ePAL_CI_PATHWAY_BYPASS;
		return ERR_OK;
	}
#endif

	if(ePalCiPathWay != ePAL_CI_PATHWAY_DAISYCHAIN_PASSTHROUGH)
	{
		/*	CI MAX+ Input�� ���� �Ѵ�.
			Remux�� ����Ҷ��� Tuner�� ����Ҷ� ���� �ʿ���.
		*/
		diErr = DI_DEMUX_GetPath(unDemuxId, &etProducer, &ulInputDevice_id, &etConsumer, &ulConsumerGroupId, &nCI_Id);
		if(diErr != DI_ERR_OK)
		{
			HxLOG_Critical("\n");
			return ERR_FAIL;
		}

		if( (etProducer == DI_DEMUX_INPUT_RECORDER)
			|| (etProducer == DI_DEMUX_INPUT_SCRAMBLED_FILE_RECORDER)
			|| (etProducer == DI_DEMUX_INPUT_PLAYER)
			|| (etProducer == DI_DEMUX_INPUT_SCRAMBLED_FILE_PLAYER) )
		{
			bIsUsedRemux = TRUE;
		}

		if(bIsUsedRemux == TRUE)
		{
			diErr = DI_DEMUX_GetPlaybackId(unDemuxId , &unPlayBackId);
			if(diErr != DI_ERR_OK)
			{
				HxLOG_Critical("\n");
				return ERR_FAIL;
			}

			if(unPlayBackId == 0)
			{
				ePalCiInput = ePAL_CI_PLAYBACK0;
			}
			else if(unPlayBackId == 1)
			{
				ePalCiInput = ePAL_CI_PLAYBACK1;
			}
			else
			{
				HxLOG_Critical("\n");
				return ERR_FAIL;
			}
		}
		else
		{
			if(unSlotId == 0)
			{
				ePalCiInput = ePAL_CI_LIVE0;
			}
			else
			{
				ePalCiInput = ePAL_CI_LIVE1;
			}
		}
	}
	else
	{
		ePalCiInput = ePAL_CI_LIVE0;
	}

	/*	CI MAX+ Outtput�� ���� �Ѵ�.
		-> ���� PATH ������ �Ҵ�� OUTPUT�� �״�� ����.
	*/
#if 0
	if(unSlotId == 0)
	{
		ePalCiOutput = ePAL_CI_OUTPUT0;
	}
	else
	{
		ePalCiOutput = ePAL_CI_OUTPUT1;
	}
#else
	ePalCiOutput = s_astCiPathInstance[unSlotId].eOutput;
#endif

	HxLOG_Info("[01;32munTunerId = %d, ePalCiInput = %d, unSlotId = %d, ePalCiOutput = %d, ePalCiPathWay = %d\n[00;39;49m",
											unTunerId, ePalCiInput, unSlotId, ePalCiOutput, ePalCiPathWay);
	#ifdef USE_MANAGE_TS_PATH
	if( (bIsUsedRemux == TRUE)	/*	Playback <-> Live �ݺ��� AV �������� �ذ��� ���� DI�� W/A�ڵ尡 ����Ǿ� �ִ�.
									�̷� ���� Playback -> Playback ������ �̷�� �� ��� �ι�° Playback���� AV�� ������ �ʴ´�.
									����, Playback ����� ���� ������ TS PATH�� ȣ���ϵ��� �Ѵ�.(DI ��û�����̱⵵ ��) */
		|| (s_astCiPathInstance[unSlotId].unTunerId != unTunerId)
		|| (s_astCiPathInstance[unSlotId].eInput != ePalCiInput)
		|| (s_astCiPathInstance[unSlotId].eOutput != ePalCiOutput)
		|| (s_astCiPathInstance[unSlotId].ePathWay != ePalCiPathWay) )
	{
		eDiInput	= pal_ci_ConvertPalToDiInput(ePalCiInput);
		eDiOutput	= pal_ci_ConvertPalToDiOutput(ePalCiOutput);
		eDiPathWay	= pal_ci_ConvertPalToDiPathWay(ePalCiPathWay);

		// DAISY CHAIN ������ �ÿ��� ������ SLOT ID�� 0���θ� �ؾ� �Ѵٰ� �� - by DI
		if(ePalCiPathWay == ePAL_CI_PATHWAY_DAISYCHAIN_PASSTHROUGH)
		{
			HxLOG_Print("[%s:%d] DAISYCHAIN_PASSTHROUGH... unSlotId (%d -> 0)\n", __FUNCTION__, __HxLINE__, unSlotId);
			diErr = DI_CI_SetExtendedTsPath(unTunerId, eDiInput, 0, eDiOutput, eDiPathWay);
		}
		else
		{
			diErr = DI_CI_SetExtendedTsPath(unTunerId, eDiInput, unSlotId, eDiOutput, eDiPathWay);
		}

		if(diErr != DI_ERR_OK)
		{
			HxLOG_Critical("DI_CI_SetExtendedTsPath() error (%d)\n", diErr);
			return ERR_FAIL;
		}

		s_astCiPathInstance[unSlotId].unTunerId	= unTunerId;
		s_astCiPathInstance[unSlotId].eInput	= ePalCiInput;
		s_astCiPathInstance[unSlotId].eOutput	= ePalCiOutput;
		s_astCiPathInstance[unSlotId].ePathWay	= ePalCiPathWay;
	}
	#else
	/*	path�� �����Ѵ�.
	*/
	diErr = DI_CI_SetExtendedTsPath(unTunerId, ePalCiInput, unSlotId, ePalCiOutput, ePalCiPathWay);
	if(diErr != DI_ERR_OK)
	{
		HxLOG_Critical("\n");
		return ERR_FAIL;
	}

	//	path ������ �����ϸ� ������ �������� ����ϵ��� �Ѵ�.

	aeCurrentPathWay[unSlotId] = ePalCiPathWay;
	#endif

#if 1
	/*	�̺κ��� ���� �Ǿ�� �Ѵ�.. DI���� ó���ϰ� �ϰ� PAL_CI_GetCurrentPathWay() �Լ��� DI���� �̿��ϸ� �ȵǳ�?
		������ ������ �����ϴ°�..
			������ PAL_CI_NORMAL_PASSTHROUGH ���� �ٸ� ���� ���� �ȵǰų� PAL_CI_NORMAL_PASSTHROUGH�̴�.
			������ PAL_CI_DAISYCHAIN_PASSTHROUGH, PAL_CI_CUSTOM_PASSTHROUGH �̸� �ٸ� ���� ������ �ȵ� ���¾�� �Ѵ�.
	*/
	#ifdef USE_MANAGE_TS_PATH
	if(ePalCiPathWay == ePAL_CI_PATHWAY_NORMAL_PASSTHROUGH)
	{
		s_astCiPathInstance[(unSlotId == 0) ? 1:0].ePathWay		= ePAL_CI_PATHWAY_NORMAL_PASSTHROUGH;
	}
	else if(ePalCiPathWay == ePAL_CI_PATHWAY_CUSTOM_PASSTHROUGH)
	{
		s_astCiPathInstance[(unSlotId == 0) ? 1:0].unTunerId	= s_astCiPathInstance[unSlotId].unTunerId;
		s_astCiPathInstance[(unSlotId == 0) ? 1:0].eOutput		= s_astCiPathInstance[unSlotId].eOutput == ePAL_CI_OUTPUT0 ? ePAL_CI_OUTPUT1 : ePAL_CI_OUTPUT0;
		s_astCiPathInstance[(unSlotId == 0) ? 1:0].ePathWay		= ePAL_CI_PATHWAY_BYPASS;
	}
	else if(ePalCiPathWay == ePAL_CI_PATHWAY_DAISYCHAIN_PASSTHROUGH)
	{
		s_astCiPathInstance[(unSlotId == 0) ? 1:0].unTunerId	= s_astCiPathInstance[unSlotId].unTunerId;
		s_astCiPathInstance[(unSlotId == 0) ? 1:0].eInput		= s_astCiPathInstance[unSlotId].eInput;
		s_astCiPathInstance[(unSlotId == 0) ? 1:0].eOutput		= s_astCiPathInstance[unSlotId].eOutput;
		s_astCiPathInstance[(unSlotId == 0) ? 1:0].ePathWay		= ePAL_CI_PATHWAY_DAISYCHAIN_PASSTHROUGH;
	}
	#else
	if(ePalCiPathWay == ePAL_CI_PATHWAY_NORMAL_PASSTHROUGH)
	{
		aeCurrentPathWay[(unSlotId == 0) ? 1:0] = ePAL_CI_PATHWAY_NORMAL_PASSTHROUGH;
	}
	else if(ePalCiPathWay == ePAL_CI_PATHWAY_CUSTOM_PASSTHROUGH)
	{
		aeCurrentPathWay[(unSlotId == 0) ? 1:0] = ePAL_CI_PATHWAY_BYPASS;
	}
	else if(ePalCiPathWay == ePAL_CI_PATHWAY_DAISYCHAIN_PASSTHROUGH)
	{
		aeCurrentPathWay[(unSlotId == 0) ? 1:0] = ePAL_CI_PATHWAY_DAISYCHAIN_PASSTHROUGH;
	}
	#endif
#endif

	return ERR_OK;
}
#endif

#ifdef CONFIG_DEBUG

static const HINT8 *pal_ci_MakeStrCiState(PalCi_State_e eState)
{
	switch (eState)
	{
	ENUM_TO_STR(ePAL_CI_EXTRACTED);
	ENUM_TO_STR(ePAL_CI_INSERTED);
	}

	return ("Unknown");
}

static const HINT8 *pal_ci_MakeStrPathWay(PalCi_PathWay_e eType)
{
	switch (eType)
	{
	ENUM_TO_STR(ePAL_CI_PATHWAY_BYPASS);
	ENUM_TO_STR(ePAL_CI_PATHWAY_NORMAL_PASSTHROUGH);
	ENUM_TO_STR(ePAL_CI_PATHWAY_DAISYCHAIN_PASSTHROUGH);
	ENUM_TO_STR(ePAL_CI_PATHWAY_CUSTOM_PASSTHROUGH);
	ENUM_TO_STR(ePAL_CI_PATHWAY_MAX);
	}

	return ("Unknown");
}

void PAL_CI_PrintTsPath (CI_SLOT_ID usSlotId) /* debugging �뵵�θ� ����ϱ� ���� �Լ� */
{
	if (usSlotId >= numCISlot)
	{
		HxLOG_Error("[ERROR] PAL_CI_PrintTsPath : wrong usSlotId %d\n", usSlotId);
		return;
	}

	HxLOG_Print("\n Current CI Path is %x (0:bypass/1:passthru)\n\n", s_CiTsPath[usSlotId]);
	return;
}


void PAL_CI_PrintTsPathAll(void) /* debugging �뵵�θ� ����ϱ� ���� �Լ� */
{
	HUINT32 i;

	HLIB_CMD_Printf("============ Print TS Path All start  ===========\n");

	for(i = 0; i < numCISlot; i++)
	{
		HLIB_CMD_Printf("\n (%d) CI Path is %x (0:bypass/1:passthru)\n\n", i, s_CiTsPath[i]);
	}

	HLIB_CMD_Printf("============ Print TS Path All end ===========\n");
}

void PAL_CI_PrintDemuxPath(void) /* debugging �뵵�θ� ����ϱ� ���� �Լ� */
{
	HUINT32			i;
	HUINT32			ulNumOfDemux;
	DI_ERR_CODE	diErr;

	DI_DEMUX_INPUTDEVICETYPE	etProducer;
	HUINT32						ulInputDevice_id;
	DI_DEMUX_OUTPUTDEVICETYPE	etConsumer;
	HUINT32						ulConsumerGroupId;
	HINT32						nCI_Id;

	HLIB_CMD_Printf("+\n");

	diErr = DI_DEMUX_GetNumOfDemux(&ulNumOfDemux);
	if(diErr != DI_ERR_OK)
	{
		HxLOG_Error("[ERROR] PAL_CI_PrintDemuxPath : DI_DEMUX_GetPath\n");
		return;
	}

	HLIB_CMD_Printf("============ Print Demux Path All start ===========\n");

	for(i = 0; i < ulNumOfDemux; i++)
	{
		if (DI_DEMUX_GetPath(i, &etProducer, &ulInputDevice_id, &etConsumer, &ulConsumerGroupId, &nCI_Id))
		{
			HLIB_CMD_Printf("DI_DEMUX_GetPath(%d) error\n", i);
			continue;
		}

		HLIB_CMD_Printf("\n (%d) Demux Path is Input(%d:%d), Output(%d:%d), CI(%d)\n\n", i, etProducer, ulInputDevice_id, etConsumer, ulConsumerGroupId, nCI_Id);
	}

	HLIB_CMD_Printf("============ Print Demux Path All end ===========\n");
	return;
}

void PAL_CI_PrintCIStatus(void)
{
	HUINT32						i;
	HUINT32						unNumberOfCi;
	//DI_ERR_CODE				diErr;
	DI_CI_STATE					diCiState;
	//DI_CI_CONNECT_STATUS_t		stCIConnectStatus;

	unNumberOfCi = DI_CI_GetNumberOfModule();

	HLIB_CMD_Printf("============ Print CI Status start ===========\n");

	for(i = 0; i < unNumberOfCi; i++)
	{
		#if 0
		diErr = DI_CI_GetStatusCI(i, &stCIConnectStatus);
		if(diErr != DI_ERR_OK)
		{
			continue;
		}

		HxLOG_Print("(%d) slot connectStatus = %d\n", i, stCIConnectStatus.connectStatus);

		HxLOG_Print("(%d) slot connectWay = %d\n", i, stCIConnectStatus.connectWay);
		#endif

		diCiState = DI_CI_CheckModule(i);
		HLIB_CMD_Printf("(%d) slot Ci state = %s\n", i, pal_ci_MakeStrCiState((PalCi_State_e)diCiState));

	#if defined(CONFIG_MW_CI_USE_EXTERNAL_CONTROLLER)
		#ifdef USE_MANAGE_TS_PATH
		HLIB_CMD_Printf("\t Tuner Id = %d\n", s_astCiPathInstance[i].unTunerId);
		HLIB_CMD_Printf("\t CI MAX+ Input = %d\n", s_astCiPathInstance[i].eInput);
		HLIB_CMD_Printf("\t CI MAX+ Output = %d\n", s_astCiPathInstance[i].eOutput);
		HLIB_CMD_Printf("\t PathWay = %s\n", pal_ci_MakeStrPathWay(s_astCiPathInstance[i].ePathWay));
		#else
		HLIB_CMD_Printf("\t Ci Path way = %s\n", pal_ci_MakeStrPathWay(aeCurrentPathWay[i]));
		#endif
	#endif
	}

	HLIB_CMD_Printf("============ Print CI Status end ===========\n");

}

#endif

/*********************** End of File ******************************/

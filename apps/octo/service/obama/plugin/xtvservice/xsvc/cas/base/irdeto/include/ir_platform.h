

#ifndef	___IR_PLATFORM_H_
#define	___IR_PLATFORM_H_

#include <octo_common.h>

/* Followings are available declarartions describing specific system, functionality and test enviroment.
	*** Declaration for irdeto system
	#define IRPLAT_SYS_USING_TS_API 		//it depends on current octo mw system, and then, it's not used but reserved for next update.
	#define IRPLAT_SYS_PROCESS_MUTEX 	// it's needed due to ir api.
	#define IRPLAT_SYS_PVR
	#define CONFIG_MW_CAS_IRDETO_SECURE				// Chipset Pairing Management (for Encrypted Control Word)

	*** Declaration for adding irdeto specific functionality or meeting operator specific request.
	#define IRPLAT_FN_QUERY_VENDOR_ID 		// for following octo multi cas system.
	#define IRPLAT_FN_DDEMM_FORCE_TUNE
	#define IRPLAT_FN_NO_SAVING_SAME_MAIL
	#define IRPLAT_FN_COPY_CONTROL
	#define IRPLAT_FN_PROCESS_SDT 		// do NOT declare it.
	#define IRPLAT_FN_STANDBY_SET_MAIL 	//not yet implemeted in octo,  the functionality that set received mails while on standby mode into already read status.

	//TEST ENVIROMENT
	#define IRPLAT_TEST_FORCE_HUMAX_VARIANT // for blocking regionalization in debug mode.
	#define IRPLAT_TEST_INCOMPLETED_LOADER  // for the time when specific loader is not yet released.
*/

#include "product/irdeto_setting.h"


#if defined(CONFIG_MW_CAS_IRDETO_LOADER_DVT)
	#undef IRPLAT_SYS_PVR
#endif

#if	!defined(IRPLAT_PVR_DRM) && defined(IRPLAT_SYS_PVR)
	#define	IRPLAT_PVR_PHASE12
#endif

#ifdef	CONFIG_MW_CAS_IRDETO_PVR
	#ifndef	IRPLAT_SYS_PVR
		#error	CONFIG_MW_CAS_IRDETO_PVR is defined but IRPLAT_SYS_PVR is not defined!!! -- ir_platform.h
	#endif
#else
	#ifdef	IRPLAT_SYS_PVR
		#error IRPLAT_SYS_PVR is defined but CONFIG_MW_CAS_IRDETO_PVR is not defined!!! -- ir_platform.h
	#endif
#endif

#define IR_HUMAX_MAX_SERVICE			(NUM_MAX_ACTION)
#define IR_HUMAX_MAX_TP				(NUM_MAX_ACTION)

#define IR_SOFTCELL_MAX_SERVICE		(IR_HUMAX_MAX_SERVICE)

#define	MAX_SC_SECTOR_COUNT			8

#if defined(CONFIG_IRDETO_HMX_PVR)
#define IR_SVC_MAX_PID      			20
#else
#define IR_SVC_MAX_PID      			16 //(MXI_MAX_PID_LIST)
#endif

#define IR_CA_SYSTEM_ID_MIN		(0x600)
#define IR_CA_SYSTEM_ID_MAX	(0x6FF)

/*******************************************************************/
/********************      Macro Definition     ********************/
/*******************************************************************/


#define HMX_INVALID_INDEX			 (0xFFFFFFFF)

#define HMX_DSC_INVALID_PAL_IDX	(0xFFFFFFFF)
#define HMX_INVALID_PID			 	(0x2000)

//	DMX���� Code���� �о� �;� �ϸ� �� ��� �������̾ VD_DMX�ʹ� ��Ÿ �κ� ������ �ʿ���.
#if defined(IRPLAT_SYS_PVR) || defined(CONFIG_IRDETO_HMX_PVR)
	#define	HMX_VD_DMX_MAX_NUM					(7)
#else
	#define HMX_VD_DMX_MAX_NUM					(4) //(MAX_NUM_OF_DMX) // TODO: PAL���� �����ͼ� �����ؾ� ��.
#endif
#define HMX_VD_DMX_MAX_FILTER_PER_CHANNEL	(16)
#define HMX_VD_DMX_MAX_CHANNEL_PER_DEMUX	(16)
#define HMX_VD_DMX_MAX_TOTAL_CHANNEL		(HMX_VD_DMX_MAX_CHANNEL_PER_DEMUX * HMX_VD_DMX_MAX_NUM)
#define HMX_VD_DMX_MAX_FILTER_DEPTH			(16)
#define HMX_VD_DMX_INVALID_REQUESET_ID 		(0xFFFF)

#if defined(CONFIG_IRDETO_HMX_PVR)
#define HMX_VD_DSC_SLOT_MAX_IN_ONE_DEMUX 	20	// Ư�� ���񽺴� Video 1��, Audio 5��, Subtitle 11�� ���� �����ϱ⵵ ��.
#else
#define HMX_VD_DSC_SLOT_MAX_IN_ONE_DEMUX 	8 //16 //(16) // (1 svc has 4 es ) *  2 svc * 2 tuner, DSC �� ����
#endif
#define HMX_VD_DSC_REFID_MAX					HMX_VD_DMX_MAX_TOTAL_CHANNEL//60 // total ref dsc IDs in chipset //(VD_MAX_NUM_OF_DSC_ID * HMX_VD_DMX_MAX_NUM)

#define HMX_VD_TIMER_MAX_NUM		16

#define HMX_VD_COPYCONTROL_RSC_MAX	3


#if	defined(CONFIG_OP_DIGITURK)
#define	HMX_VD_STREAMCRYPTO_AES
#endif

/////////////////////////////////////////////////////////////
// VD DRIVER VERSION
#define VD_SMARTCARD_V1				1
#define VD_SMARTCARD_V2_SOFTCLIENT	2
#define VD_SMARTCARD_V3_JAVACARD	3
#define VD_SMARTCARD_V4_CARDFORM	4
#define VD_SMARTCARD_V5_DIRECTATR	5

#define VD_FILTER_V1					1 // TODO: Do not use Demux version 1, It's too slow than v2.
#define VD_FILTER_V2_DIRECTREAD	2
#define VD_FILTER_V3_DMB			3
#define VD_FILTER_V3_DIRECTREAD		3

#define VD_DESCRAMBLER_VERSION_V1							1
#define VD_DESCRAMBLER_VERSION_V2_SECURE_CHIP				2
#define VD_DESCRAMBLER_VERSION_V3_SECURE_CHIP_MULTICARD	3
#define VD_DESCRAMBLER_VERSION_V4_MULTIPLE_PID				4

#define VD_MACROVISION_V1			1
#define VD_MACROVISION_V2			2
#define VD_MACROVISION_V3_DIGITAL_AND_ANALOG	3

#define VD_LOADER_VERSION_V1						1
#define VD_LOADER_VERSION_V2						2
#define VD_LOADER_VERSION_V3						3

#define VD_DESCRAMBLER_VERSION_V1					1
#define VD_DESCRAMBLER_VERSION_V2_SECURE_CHIP		2
#define VD_DESCRAMBLER_VERSION_V3_SECURE_CHIP		3
#define VD_DESCRAMBLER_VERSION_V4_SECURE_CHIP		4
#define VD_DESCRAMBLER_VERSION_V5_SECURE_CHIP		5

#define VD_TIMER_V1					1
#define VD_EXTERNAL_STORAGE_V1	1
#define VD_PERSISTENT_STORAGE_V1	1

#define VD_STREAMCRYPTO_V1		1
#define	VD_STREAMCRYPTO_V2		2
#define	VD_STREAMCRYPTO_V3		3


#if defined(CONFIG_PROD_IR2020HD) || defined (CONFIG_PROD_IR2000HD) || defined(CONFIG_PROD_ICORDHDPLUSME) || defined(CONFIG_PROD_ICORDHDPLUSMEADM) \
	|| defined(CONFIG_PROD_HDFREE) || defined(CONFIG_PROD_IR3000HD) || defined(CONFIG_PROD_IR3020HD) || defined(CONFIG_PROD_HDFREECCA) || defined(CONFIG_PROD_HMS1000S) || defined(CONFIG_PROD_HMSLITE)\
	|| defined(CONFIG_PROD_IR4000HD)

#define VD_SMARTCARD_VERSION					VD_SMARTCARD_V1		// 1
#define VD_PERSISTENT_STORAGE_VERSION			VD_PERSISTENT_STORAGE_V1 // 1
#define VD_FILTER_VERSION						VD_FILTER_V2_DIRECTREAD // 2
#define VD_TIMER_VERSION						VD_TIMER_V1 // 1
#define VD_DESCRAMBLER_VERSION					VD_DESCRAMBLER_VERSION_V5_SECURE_CHIP // 5
#define VD_LOADER_VERSION						VD_LOADER_VERSION_V3 // 3
#define VD_STREAMCRYPTO_VERSION					VD_STREAMCRYPTO_V1 // 1
#define VD_MACROVISION_VERSION					VD_MACROVISION_V2 // 2
#define VD_EXTERNAL_STORAGE_VERSION				VD_EXTERNAL_STORAGE_V1 // 1

#else

#if defined(CAS_IR_ICE2_PROJECT)
#define VD_SMARTCARD_VERSION				VD_SMARTCARD_V5_DIRECTATR
#else
#define VD_SMARTCARD_VERSION				VD_SMARTCARD_V1
#endif
#define VD_PERSISTENT_STORAGE_VERSION	VD_PERSISTENT_STORAGE_V1
#if defined (CONFIG_OP_ZIGGO_RFVOD) || defined(CONFIG_OP_ZIGGO_AVN_CLIENT)
#define VD_FILTER_VERSION					VD_FILTER_V3_DIRECTREAD
#else
#define VD_FILTER_VERSION					VD_FILTER_V2_DIRECTREAD
#endif
#define VD_TIMER_VERSION					VD_TIMER_V1
#if defined(CONFIG_MW_CAS_IRDETO_SECURE)
#if defined(CONFIG_OP_SKYLINK) || defined(CONFIG_OP_VAST)
#define VD_DESCRAMBLER_VERSION			VD_DESCRAMBLER_VERSION_V5_SECURE_CHIP
#define VD_LOADER_VERSION				VD_LOADER_VERSION_V3
#else
#define VD_DESCRAMBLER_VERSION			VD_DESCRAMBLER_VERSION_V2_SECURE_CHIP
#if defined(CONFIG_MW_CAS_IRDETO_SECURE) && defined(CONFIG_OP_DIGITURK)
/*
	Softcell VD API ������ Secure Chip phase3�� ����ϱ� ���ؼ� Loader Version 3�̻� �̾�� �Ѵٰ� �Ǿ� ����.
	�׷��� ���� �����ϰ�� ����� ���� �� �ϳ� �׷��� Spec�� �ִµ��� �ϴ� ���� ���� ���Կ� V3�� �����.
*/
#define VD_LOADER_VERSION				VD_LOADER_VERSION_V3
#else //#if defined(CONFIG_MW_CAS_IRDETO_SECURE) && defined(IRPLAT_PVR_DRM)
#define VD_LOADER_VERSION				VD_LOADER_VERSION_V2
#endif //#if defined(CONFIG_MW_CAS_IRDETO_SECURE) && defined(IRPLAT_PVR_DRM)
#endif
#else
#define VD_DESCRAMBLER_VERSION			VD_DESCRAMBLER_VERSION_V1
#define VD_LOADER_VERSION				VD_LOADER_VERSION_V1
#endif

#if	defined(IRPLAT_PVR_DRM) && defined(CONFIG_MW_CAS_IRDETO_SECURE)
	#define	VD_STREAMCRYPTO_VERSION		VD_STREAMCRYPTO_V3
#elif	defined(IRPLAT_PVR_DRM)
	#define	VD_STREAMCRYPTO_VERSION		VD_STREAMCRYPTO_V2
#else
#define VD_STREAMCRYPTO_VERSION		VD_STREAMCRYPTO_V1
#endif

#define VD_MACROVISION_VERSION		VD_MACROVISION_V3_DIGITAL_AND_ANALOG
#define VD_EXTERNAL_STORAGE_VERSION	VD_EXTERNAL_STORAGE_V1

#if defined (CONFIG_OP_ZIGGO_AVN_CLIENT)
#define VD_DVB_OOB_V1			1
#endif

#endif

#define IR_CRYPTO_KEY_LEN		(16)

/* Maturity rating subscriber pin code length, BCD code */
#define IR_MR_SUBSCRIBER_PIN_LEN		2

#ifdef CONFIG_MW_CAS_NUM_SC_SLOT										// Smart card slot ����, Kconfig���� ���� ��.
#define HUMAX_VD_SC_SLOT_COUNT			CONFIG_MW_CAS_NUM_SC_SLOT
#else
#define HUMAX_VD_SC_SLOT_COUNT			1
#endif


typedef enum
{
	HIR_OK	=0,
	HIR_NOT_OK,
	HIR_INVALID_PARAMETER,
	HIR_INVALID_MEMORY,
	HIR_ERR_SCELL_INTERNAL,
	HIR_ERR_MAX
}HIR_ERR;

typedef enum IR_MSG_KIND_e
{
	/*Followins are  eIR_MSG_CLASS_CAS Action types.  */
	IR_CATASK_CARD_IN,
	IR_CATASK_CARD_OUT,
	IR_CATASK_NVRAM_WRITE,
	IR_CATASK_FILE_WRITE,
	IR_CATASK_FROM_SOFTCELL,
	IR_CATASK_KIND_MAX_NUM
} IR_MSG_KIND_E;


/*******************************************************************/
/********************      Typedef         *************************/
/*******************************************************************/




typedef struct tCAS_IR_SC_MSG
{
	HUINT8		ucSlotID;
	HUINT32		ResourceID;
	HUINT32		ReferenceID;
} CAS_IR_SC_MSG;

typedef struct tCAS_IR_FILTER_MSG
{
	void		*handle;
	HUINT8		ucSvcIndex;
	HUINT8		ucCompIndex;
	HUINT8		ucFilterIndex;
	HUINT8		ucAction;
} CAS_IR_FILTER_MSG;

typedef struct tCAS_IR_CW_MSG
{
	void		*handle;
	HUINT8		ucType;
	HUINT8		*pucEven;
	HUINT8		*pucOdd;
} CAS_IR_CW_MSG;

typedef struct tCAS_IR_EXM_MSG
{
	HUINT8		*pucData;
	HUINT32		ulLen;
	HUINT16		usLoadId;
} CAS_IR_EXM_MSG;

typedef struct tCAS_IR_SOFTCELL_MSG
{
	HUINT32		eOpCode;	// ca_drv_message_type_e
	HUINT16		wOpSeqNo;
	HUINT8		ucFreeFlag;
	HUINT16		wLength;
	void		*pvMessage;
} CAS_IR_SOFTCELL_MSG;

typedef struct tCAS_IR_NVRAM_WRITE_MSG
{
	HUINT32	RefId;
	HUINT32	ulStart;
	HUINT32	ulLen;
	HUINT8	*pucData;
} CAS_IR_STORAGE_WRITE_MSG;

typedef struct tCAS_IR_BASE_MSG
{
	union
	{
		CAS_IR_FILTER_MSG		FILTER;
		CAS_IR_SC_MSG			SC;
		CAS_IR_CW_MSG			CW;
		CAS_IR_EXM_MSG			EXM;
		CAS_IR_SOFTCELL_MSG		SOFTCELL;
		CAS_IR_STORAGE_WRITE_MSG	NVRAM;
		CAS_IR_STORAGE_WRITE_MSG	FILE;
	} msg;
} CAS_IR_BASE_MSG;


/*******************************************************************/
/********************      Public Functions     ********************/
/*******************************************************************/


#endif	//___IR_PLATFORM_H_


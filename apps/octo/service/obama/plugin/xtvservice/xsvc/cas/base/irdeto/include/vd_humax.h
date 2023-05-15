
#ifndef __VD_HUMAX_H__
#define	__VD_HUMAX_H__


/********************************************************************
*   Including Headers
********************************************************************/
#include "ir_platform.h"
#include "ir_api.h"

#include "s3_vd.h"
#include "s3_vdps.h"
#include "s3_smartcard.h"
#include "s3_vddf.h"
#include "s3_vdtmr.h"
#include "s3_vd_external_storage.h"

//#include "s3_prod_if_msg.h"
#if defined(CONFIG_OP_ZIGGO_AVN_CLIENT)
#include "s3_vd_dvb_oob.h"
#endif

#include "pal_dsc.h"
#include "pal_sc.h"


#define HVD_EXTSTORAGE_FILE_CNT_MAX			(32)
#define FM_FS_MAX_FILE_NAME_LEN			PAL_PVR_MAX_FILE_NAME_LENGTH
#define FS_READ_MODE	PAL_FS_MODE_READ
#define FS_WRITE_MODE	PAL_FS_MODE_WRITE
#define FS_RDWR_MODE	PAL_FS_MODE_RDWR

#if (CONFIG_MW_CAS_NUM_SC_SLOT > 1)
#define CONFIG_WA_2SC_SAME_ENTITLEMENT /* Softcell 3.10에서 같은 권한의 Smartcard 처리 문제를 개선한 WA Code */
#endif

#if defined(CONFIG_WA_2SC_SAME_ENTITLEMENT)
#if defined(CONFIG_PROD_IR4000HD)
//dd와 검토 필요
//2slot 에서 2개의 desc를 열고(Audio, Video), 각각 PID 세팅을 잘 해줘도, 
//처음 재핑 했을 때, s3에서 set pid 가 오는 순서에 따라, audio에  odd (혹은 even) key만 세팅 되었을 때 (이때 odd key로 descramble 가능한 구간)
//오디오가 풀리지 않는 문제.
#define WA_2SC_SAME_ENTITLEMENT_GRAP_KEY_DATA 
#endif
#endif

#define _____HUMAX_VD_RESULT_TYPE_________________

typedef enum
{
	eHVD_OK =0,
	eHVD_ERR,
	eHVD_ERR_NOT_IRDETOCARD,
	eHVD_ERR_INVALID_CARD,
}HVD_Result;


#define ______VD_TIMER________________

typedef struct
{
	vd_ref_id			ReferenceID;
	HULONG				TimerID;
	HUINT8				Status;	// 0 stop, 1 start
	vd_timer_open_st	HUMAX_vd_timer_open_st;
} HVD_TIMER_REF_INFO_STRUCT;

typedef struct
{
	vd_rsc_id		ResourceID;
	HVD_TIMER_REF_INFO_STRUCT	RefInfo[HMX_VD_TIMER_MAX_NUM];
} HVD_TIMER_INFO_STRUCT;

extern ia_result 	HDV_TIMER_Initialise(void);
extern vd_ref_id HDV_TIMER_Open( vd_rsc_id wResourceID, void *pvData );
extern ia_result 	HDV_TIMER_Close( vd_ref_id wReferenceID );
extern ia_result 	HDV_TIMER_IO( vd_ref_id wRefId, ia_word16 wOpCode, ia_word32 wDataWriteSize, void *pvDataWrite, ia_word32 *pwDataReadSize, void *pvDataRead );

#define ______VD_DSC________________


typedef struct
{
	HUINT8		ucDemuxId;
	vd_ref_id		ReferenceID;
	HUINT32		ulPalDscIdx;
	HUINT16		usPid;
	HUINT8		EvenKey[8];
	HUINT8		OddKey[8];
	HUINT32		bNeedToClose;
	HUINT16		usUsedCount;	// ewlee 20100825
} HVD_DSC_RSC_INFO;

typedef struct
{
	vd_rsc_id		ResourceID; //dmx resourcec id 와 같으나, 실제 확인이 필요하다.
	HVD_DSC_RSC_INFO	RefInfo[HMX_VD_DSC_SLOT_MAX_IN_ONE_DEMUX];
}HVD_DSC_RSC;

extern ia_result 	HVD_DSC_Initialise(void);
extern vd_ref_id HVD_DSC_Open( vd_rsc_id wResourceID, void *pvData );
#if defined (CONFIG_WA_2SC_SAME_ENTITLEMENT)	// ewlee 20100825
extern ia_result 	HVD_DSC_MultipleClose( vd_ref_id wRefId );
#endif
extern ia_result 	HVD_DSC_Close( vd_ref_id wRefId );
extern ia_result 	HVD_DSC_IO( vd_ref_id wRefId, ia_word16 wOpCode, ia_word32 wDataWriteSize, void *pvDataWrite, ia_word32 *pwDataReadSize, void *pvDataRead );
#ifdef CONFIG_DEBUG
extern void HVD_DBGAPI_DSC_INFO(void);
#endif


#define ______VD_EXTERNAL_STORAGE_____
typedef struct
{
	vd_ref_id 				referenceID;
	vd_rsc_id				resourceID;
	HUINT32					ulFileHandle;
	HUINT8					szFileName[256];
	vd_es_open_st			es_open_st;
} HUMAX_VD_ExtStorage_st;

#define ______VD_PERSISTENT_STORAGE_____
typedef struct
{
	vd_rsc_id				resourceID;
	vd_ref_id				referenceID;
	vd_ps_open_st			ps_open_st;
} HUMAX_VD_PersistentStorage_st;

typedef struct
{
	vd_ps_write_st ps_write_st;
	ia_byte *abDataToWrite;
} HUMAX_ps_write_st;

#define _____VD_SMARTCARD________________
typedef struct
{
	SciHandle_t						hIRSci;
	vd_rsc_id							ResourceID;
	vd_ref_id							ReferenceID;
	unsigned long						SemaphoreID;
	SvcCas_DevSciScStatus_e			CardPhysicalStatus;
	HUINT32							CardStatus;
	HUINT32							RetryCount;
	HUINT32							Protocol;
	HUINT32							ulAtrLen;
	HUINT8							aucATR[34];
	HUINT8							VersionFromATR[4];
	IR_API_SC_INFO					stIrScApiInfo;
#if defined(CONFIG_OP_DIGITURK)
	HUINT8								aucTMSUserData[IR_SC_INFO_TMS_DATA_LENGTH];
#endif
	HUINT8							szScID[IR_SC_INFO_ID_LENGTH]; //hashed value
	ia_byte							bSectors;		// Number Of Sector.
	ia_result							bProductInit;
	ia_result							bProductExistance;		// whether there is any sector that have product
	msg_number_of_sectors_sector_st	*p_number_of_sectors_sector_st;
	vd_sc_open_st					ScOpen;
	vd_sc_error_e						ErrorReason;

	vd_sc_card_form_e				eCardForm;



} HVD_SC_INFO, IR_DRV_SC_INFO/*to be removed*/;


#define ______VD_DEMUX________________

typedef struct
{
	vd_rsc_id awResources[HMX_VD_DMX_MAX_NUM];
} vd_demux_resource;

typedef struct
{
	HUINT16			RequestID; //it represents filter id in octo system.
	HUINT8			Allocated;
	HUINT8			Started;
	HUINT16			wFilterLength;
	HUINT8			FilterValue[HMX_VD_DMX_MAX_FILTER_DEPTH];
	HUINT8			FilterMask[HMX_VD_DMX_MAX_FILTER_DEPTH];
} HVD_DMX_FILTER_INFO_STRUCT;

typedef struct
{
	vd_ref_id		ReferenceID;

#if (VD_FILTER_VERSION == 1)
	vd_df_open_st	HUMAX_vd_df_open_st;
#else
	vd_df_open_v2_st	HUMAX_vd_df_open_v2_st;
#endif

	HUINT16			PID;
	HUINT8			HaveToStart;
	HVD_DMX_FILTER_INFO_STRUCT	IR_FilterInfo[HMX_VD_DMX_MAX_FILTER_PER_CHANNEL];
	HUINT8			*pBuffer;
	HUINT8			*pRead;
	HUINT8			*pWrite;
	HUINT32			BufferLength;
	HUINT32			ReadPosition;
	HUINT32			WritePosition;

} HVD_DMX_REF_INFO_STRUCT;

typedef struct
{
	vd_rsc_id		ResourceID;
	HVD_DMX_REF_INFO_STRUCT	RefInfo[HMX_VD_DMX_MAX_CHANNEL_PER_DEMUX];
} HVD_DMX_DEMUX_INFO_STRUCT;


extern ia_result HVD_DMX_Initialise(void);
extern HUINT32 HVD_DMX_GetResources(void **ppvList, ia_word16 *pwCount );
extern vd_ref_id HVD_DMX_Open( vd_rsc_id wResourceID, void *pvData );
extern ia_result HVD_DMX_Close( vd_ref_id wRefId );
extern ia_result HVD_DMX_IO( vd_ref_id wRefId, ia_word16 wOpCode, ia_word32 wDataWriteSize, void *pvDataWrite, ia_word32 *pwDataReadSize, void *pvDataRead );
#if defined(CONFIG_DEBUG)
extern HUINT32 HVD_DMX_PrintDemuxInfo(void);
#endif

extern HUINT32 HVD_DMX_SendJumpEvent(void);

extern void HUMAX_VdSc_HashCodeConvert(void);	// ewlee 20100817

#if defined(IRPLAT_SYS_SAVE_SMARTCARD_NUMBER)
extern HBOOL HUMAX_VdSc_IsSameWithSavedSmartcardNumber();
extern void HUMAX_VdSc_SaveSmartcardNumber ();
#endif

#if defined(CONFIG_OP_ZIGGO_AVN_CLIENT)

#define ______VD_DVB_OOB________________

#define		MAX_DVB_OOB_SECTION		32

typedef struct
{
	vd_ref_id 				referenceID;
	vd_dvb_oob_open_st		do_open_st;
	HUINT16					usHead;
	HUINT16					usTail;
	void 					*pSecData[MAX_DVB_OOB_SECTION];
	ia_uint16				usSecSize[MAX_DVB_OOB_SECTION];
} HUMAX_VD_DVB_OOB_st;
#endif

#endif	/*	__VD_HUMAX_H__	*/

/*	end of file	*/



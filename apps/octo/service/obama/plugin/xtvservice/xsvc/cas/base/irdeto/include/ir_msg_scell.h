
#ifndef ___IR_MSG_SCELL_H____
#define ___IR_MSG_SCELL_H____


/********************************************************************
*   define
********************************************************************/

typedef struct
{
	HUINT16				usCount;
	msg_version_data_st	VersionData;
	HUINT8				*pucNameString;
	HUINT8				*pucBuildInfoString;
} SOFTCELL_VERSION_DATA;



#define	MAX_TMS_SVC					IR_HUMAX_MAX_SERVICE

#ifdef _____ECM_EMM_MONITORING_____
	#define __WA_EMM_MONITOR_CLOSE_ERR__	// EMM Monitor가 Close되어 EmmCount가 초기화 되는 문제 WA
#endif


/********************************************************************
*   Global Function Prototypes definitions
*********************************************************************/

extern void CAS_IR_SVC_OPEN_REQUEST_DVB(HUINT32 SvcIndex);
extern void CAS_IR_SVC_CLOSE_DVB(HUINT32 SvcIndex, HBOOL bInstOff);
extern HIR_ERR CAS_IR_SVC_CLOSE_EMM(IR_TP_INFO *pIrTpInfo);
extern HUINT32	CAS_IR_SVC_DEFINE_EMM(HUINT16 irServiceHandle);
extern HUINT32	CAS_IR_SVC_DEFINE_DVB(HUINT32 idx);
extern HIR_ERR	CAS_IR_SVC_DEFINE_ECMMONITOR(HUINT16 ushSCELL, HUINT8 bStart);
extern HUINT32	CAS_IR_SVC_DEFINE_EMMMONITOR(HUINT16 ushSCELL, HUINT8 bStart);
#if defined(IRPLAT_SYS_PVR)
extern HUINT32	CAS_IR_SVC_OPEN_REQUEST_PVR(Handle_t hIrSvc, HUINT16 ir_service_type);
extern HUINT32	CAS_IR_SVC_CLOSE_PVR(Handle_t hIrSvc, HUINT16 ir_service_type);
#endif

extern void CAS_IR_SoftcellMsgProc(CAS_IR_BASE_MSG *pMsgIR);

#endif


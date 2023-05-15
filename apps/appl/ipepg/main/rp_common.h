/**************************************************************
 *	@file		rp_common.h
 *	Remote Programming header for OCTO 2.0
 *
 *	http://www.humaxdigital.com
 *
 *	@data			2012/08/01
 *	@author			humax
 *	@brief			Remote Programming header OCTO 2.0 Project
 **************************************************************/
#ifndef _RP_COMMON_H_
#define _RP_COMMON_H_

 #include	<hlib.h>


///////////////////////////////////////
// Rsv download

 typedef enum
 {
	 RP_RSVTYPE_RESERVED = 0 ,
	 RP_RSVTYPE_WATCHING,
	 RP_RSVTYPE_REC_EBR,
	 RP_RSVTYPE_REC_TBR,
 }RPLIST_RSVTYPE_e;

 typedef enum
 {
	 RP_REPEAT_ONCE = 1,
	 RP_REPEAT_DAILY,
	 RP_REPEAT_WEEKLY,
	 RP_REPEAT_SERIES
 }RPLIST_REPEAT_e;

 typedef enum
 {
	 RPLIST_RESULT_OK = 1,
	 RPLIST_RESULT_FAIL_CONFLICT,
	 RPLIST_RESULT_FAIL_SERVICE_IS_NOT_FOUND,
	 RPLIST_RESULT_FAIL_EVENT_IS_NOT_FOUND,
	 RPLIST_RESULT_FAIL_SLOT_ERROR,
	 RPLIST_RESULT_FAIL_RESERVATION_IS_NOT_FOUND,
	 RPLIST_RESULT_FAIL
 }RPLIST_RESULT_e;

 typedef struct tagRpListAttr
 {
	 HUINT8 			 ucVersion;
	 HUINT8 			 ucCountry;
	 HUINT8 			 ucCount;
 }RPListAttr_t;

 typedef struct tagRpData
 {
	 HUINT16			 usKind;
	 HUINT16			 usAction;
	 HUINT16			 usRpindex;
	 HUINT32			 usUniqueId;
	 UNIXTIME			 utStartTime;
	 UNIXTIME			 utEndTime;
	 HUINT16			 usRsvType;
	 HUINT8 			 ucRepeat;
	 HINT16 			 usRsvResult;
 //  SVC_TripleId_t 	 stTripleId;
	 HUINT32			 hSvc;	 // Handle_t
 }RP_Data_t;

 typedef struct tagRpList
 {
	 RPListAttr_t		 stAttr;
	 RP_Data_t			 *pstRpData;
 }RPList_t;

//////////////////////////////////////////////////////////
// STB Status Upload

/* XML Print Type */
typedef	enum
{
	PRINT_NONE,
	PRINT_OPENING,
	PRINT_CLOSING,
	PRINT_STRING,
	PRINT_DECIMAL,
	PRINT_ATTR1,
	PRINT_ATTR2,
	PRINT_ATTR3,
	PRINT_ATTR_DECIMAL,
}STB_PRINT_TYPE_e;

typedef	enum
{
	DEPTH_ROOT,
    DEPTH_1,
    DEPTH_2,
    DEPTH_3,
}STB_DEPTH_e;


typedef	enum
{
	STB_ELEMENT_LIST_NOTIFY = -1,
    STB_ELEMENT_LIST_TIMEZONE,
	STB_ELEMENT_LIST_HDD,
	STB_ELEMENT_LIST_RPLIST,
	STB_ELEMENT_LIST_RSVLIST,
	STB_ELEMENT_LIST_RECLIST,
	STB_ELEMENT_LIST_ALLOWEDLIST,
	STB_ELEMENT_LIST_MAX
}STB_ELEMENT_LISTTYPE_e;

typedef	enum
{
	STB_NOTIFY_OPEN,
	STB_NOTIFY_CLOSE,
}STB_NOTIFY_e;

typedef	enum
{
	STB_TIMEZONE,
}STB_TIMEZONE_e;


typedef	enum
{
	STB_HDD_OPEN,
	STB_HDD_PARTITION_OPEN,
	STB_HDD_SIZEFREE,
	STB_HDD_SIZEUSED,
	STB_HDD_SIZETOTAL,
	STB_HDD_PARTITION_CLOSE,
	STB_HDD_CLOSE,
}STB_HDD_e;

typedef	enum
{
	STB_RPLIST_OPEN,
	STB_RPLIST_RPDATA_OPEN,
	STB_RPLIST_RPINDEX,
	STB_RPLIST_RESULT,
	STB_RPLIST_RPDATA_CLOSE,
	STB_RPLIST_CLOSE,
}STB_RPLIST_e;

typedef	enum
{
	STB_RSVLIST_OPEN,
	STB_RSVLIST_RSV_OPEN,
	STB_RSVLIST_CHNUM,
	STB_RSVLIST_CHNAME,
#if 0 // to be deleted
	STB_RSVLIST_ONID,
	STB_RSVLIST_TSID,
	STB_RSVLIST_SVCID,
#endif
	STB_RSVLIST_SVCHANDLE,
	STB_RSVLIST_UNIQUEID,
	STB_RSVLIST_START,
	STB_RSVLIST_END,
	STB_RSVLIST_EVTNAME,
	STB_RSVLIST_RSVTYPE,
	STB_RSVLIST_REPEAT,
	STB_RSVLIST_RSV_CLOSE,
	STB_RSVLIST_CLOSE,
}STB_RSVLIST_e;

typedef	enum
{
	STB_RECLIST_OPEN,
	STB_RECLIST_REC_OPEN,
	STB_RECLIST_CHNUM,
	STB_RECLIST_CHNAME,
	STB_RECLIST_START,
	STB_RECLIST_END,
	STB_RECLIST_EVTNAME,
	STB_RECLIST_REPEAT,
	STB_RECLIST_SIZE,
	STB_RECLIST_PLAYED,
	STB_RECLIST_EVTID,
	STB_RECLIST_REC_CLOSE,
	STB_RECLIST_CLOSE
}STB_RECLIST_e;

typedef	enum
{
	STB_ALLOWEDLIST_OPEN,
	STB_ALLOWEDLIST_DEVICEID,
	STB_ALLOWEDLIST_CLOSE
}STB_ALLOWEDLIST_e;



typedef struct tagSTB_AttrElement
{
	HUINT8				*pucAttr1;
	HUINT8				*pucAttr2;
	HUINT8				*pucAttr3;
    HUINT8              ucDecimal;
} STB_ATTR_ELEMENT_t;

typedef struct tagSTB_DataType
{
	HUINT8				*pString;
	HUINT32				uiDecimal;
} STB_DataType_t;

typedef struct tagSTB_Element
{
	HUINT32				uiDepth;
	HUINT8				*pucTagName;
	HUINT32				uiPrintType;
	HUINT8				*pucAttr0Name;
	HUINT8				*pucAttr1Name;
	HUINT8				*pucAttr2Name;
} STB_ELEMENT_t;

typedef struct tagSTB_ElementList
{
	HUINT32				uiListType;
	STB_ELEMENT_t		*pstElement;
} STB_ELEMENTLIST_t;

typedef struct
{
	HUINT8		ucOrgStrLen;
	HUINT8		ucFlagReplace;
	HUINT8		szOrgStr[8];
	HUINT8		ucReplaceCode;
} RP_REPLACESTR_t;




// reservation

 typedef struct
 {
	HUINT32				chNum;
	HCHAR				chName[256];
	HUINT32				svcId;
	HUINT32				uniqueId;
	HUINT32				startTime;
	HUINT32				endTime;
	HCHAR				evtName[256];
	HUINT32				rsvType;
	HUINT32				repeat;
 } ScheduledItem_t;


typedef struct
{
	HUINT32				chNum;
	HCHAR				chName[256];
	HUINT32				startTime;
	HUINT32				endTime;
	HCHAR				evtName[256];
	HUINT32				repeat;
	HUINT32				size;	// KB
}RecordedItem_t;

 typedef struct
 {
	 void 	*  schedule;

	 HxList_t  *scheduledList;
	 HxList_t  *recordingList;

 } Scheduler_t;



#define IPEPG_RP_LIST_MAX_SIZE			(50*1024)

void 		Rp_Task(void * pstDownMgr);
 HERROR 		Rp_ParseRsvList(HUINT8 *pRawBuffer, HUINT32 ulBufferSize, HxList_t **ppRpListAttr, HxList_t **ppRpList);
 RPList_t *  	Rp_GetRpLists(void);
HERROR			Rp_UploadData(void);
#endif	// _RP_COMMON_H_

/*********************** End of File ******************************/


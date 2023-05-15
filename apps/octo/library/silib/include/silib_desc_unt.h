/* $Header: $ */

#ifndef __SILIB_UNT_H__
#define __SILIB_UNT_H__

#ifndef _____INCLUDED_ONLY_IN_SILIB_H____
#error The file [silib_desc_unt.h] shall not be included except silib.h
#endif

#ifdef __cplusplus
extern "C" {
#endif

// eSIxDESCTAG_UNT_SCHEDULING (0x01)
typedef struct
{
	SIxDescriptorHeader_t							 stHead;

	UNIXTIME		startDateTime;			/** < start_date_time of scheduling_descriptor in UNT				*/
	UNIXTIME		endDateTime;			/** < end_date_time of scheduling_descriptor in UNT					*/
	HUINT8			ucIsFinalAvailable;		/** < final_availability of scheduling_descriptor in UNT			*/
	HUINT8			ucIsPeriodic;			/** < periodicity_flag of scheduling_descriptor in UNT				*/
											/** < unit : Sec(0), Min(1), Hour(2), Day(3)						*/
	HUINT8			ucPeriodUnit;			/** < period_unit of scheduling_descriptor in UNT					*/
	HUINT8			ucDurationUnit;			/** < duration_unit of scheduling_descriptor in UNT					*/
	HUINT8			ucEstimatedCycleUnit;	/** < estimated_cycle_time_unit of scheduling_descriptor in UNT		*/
	HUINT8			ucPeriod;				/** < period of scheduling_descriptor in UNT						*/
	HUINT8			ucDuration;				/** < duration of scheduling_descriptor in UNT						*/
	HUINT8			ucEstimatedCycle;		/** < estimated_cycle_time of scheduling_descriptor in UNT			*/
//	HUINT8			*pucPrivateDataByte;
} SIxSchedulingDes_t;

// eSIxDESCTAG_UNT_UPDATE (0x02)
typedef struct
{
	SIxDescriptorHeader_t							 stHead;

	HUINT8			 ucUpdateFlag;
	HUINT8			 ucUpdateMethod;
	HUINT8			 ucUpdatePriority;
//	HUINT8			*pucPrivateDataByte;
} SIxUpdateDes_t;

// eSIxDESCTAG_UNT_SSU_LOCATION (0x03)
typedef struct
{
	SIxDescriptorHeader_t							 stHead;

	HUINT16			 usDataBroadcastId;		/** < 0x000A이면, standard SSU two layer data carousel		*/
	HUINT16			 usAssociationTag;		/** < data_broadcast_id == 0x000A 일때만 사용				*/
//	HUINT8			*pucPrivateDataByte;
} SIxSsuLocationDes_t;

// eSIxDESCTAG_UNT_MESSAGE (0x04)
typedef struct
{
	SIxDescriptorHeader_t							 stHead;

	HUINT8			 ucDesNum;
	HUINT8			 ucLastDesNum;
	HUINT8			 szIso639LangCode[4];
	HUINT8			*pszText;
} SIxMsgDes_t;

// eSIxDESCTAG_UNT_SSU_SUBGROUP_ASSOCIATION(0x0B)
typedef struct
{
	SIxDescriptorHeader_t							 stHead;

	HUINT8			 subgroupTag[6];
} SIxSsuSubgroupAssociationDes_t;


#ifdef __cplusplus
}
#endif

#endif // __SILIB_UNT_H__



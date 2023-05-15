/**************************************************************************************/
/**
 * @file oplmetadatasearch.h
 *
 *
 *
 * @attention   Copyright (c) 2011 Humax - All rights reserved.
 *
 **************************************************************************************
 **/

#ifndef __OPLMETADATASEARCH_H__
#define __OPLMETADATASEARCH_H__

#include	"opltype.h"
#include	"oplquery.h"
#include	"oplchannel.h"
#include	"oplprogramme.h"

#ifdef __cplusplus
extern "C" {
#endif	//__cplusplus

typedef enum
{
	eOPL_SEARCH_TARGET_SCHEDULED	= 1,
	eOPL_SEARCH_TARGET_ONDEMAND		= 2,

	eOPL_SEARCH_TARGET_UNKNOWN
} OPL_SearchTarget_e;


typedef struct
{
	OPL_HANDLE			search;

	int					length;
	int					offset;
	int					totalSize;
	int					count;

	OPL_Collection_t	*items;
} OPL_SearchResults_t;


typedef enum
{
	eOPL_SEARCH_STATE_MORE_DATA,
	eOPL_SEARCH_STATE_FINISHED,
	eOPL_SEARCH_STATE_UPDATED,
	eOPL_SEARCH_STATE_INVALID,

	eOPL_SEARCH_STATE_UNKNOWN
} OPL_SearchState_e;

typedef void	(* OPL_SearchListener_t) (OPL_HANDLE, OPL_SearchState_e);
typedef void 	(* OPL_SearchPfMonitor_t) (OPL_HANDLE self, OPL_ProgrammeHandle *pf, HINT32 n, void *);

/**
 * Create Metadatasearch
 *
 * @param	target @see OPL_SearchTarget_e
 */
OPL_HANDLE	OPL_METADATASEARCH_New (OPL_SearchTarget_e target);

/**
 * Destroy MetadataSearch
 *
 * @param	self instance of MetadataSearch
 */
void		OPL_METADATASEARCH_Delete (OPL_HANDLE self);

/**
 * Add/Remove dependency
 * - MetadataSearch Handle과 Programme간에 종속성을 부여하여, WebApp의 예측할 수 없는 GC 호출로 인한
 *   MetadataSearch 의 Deletion이 발생하지 않도록 한다.
 * - MetadataSearch Handle이 삭제되는 경우, 업데이트 이벤트 핸들링이 되지 않기 때문에 위와 같이 처리 함
 * - 아래 함수를 사용하면 'self' MetadataSearch Handle로 부터 얻은 모든 ProgrammeHandle이 release 되기
 *   전에는 MetadataSearch Handle을 삭제할 수 없다.
 *
 * @param	self instance of MetadataSearch
 * @param	programme	handle of Programme
 */
void		OPL_METADATASEARCH_AddDependency (OPL_HANDLE self, OPL_ProgrammeHandle programme);
void		OPL_METADATASEARCH_RemoveDependency (OPL_HANDLE self, OPL_ProgrammeHandle programme);

OPL_SearchTarget_e
			OPL_METADATASEARCH_GetTarget (OPL_HANDLE self);
/**
 * Set MetadataSearch Event Listener
 *
 * @param	self instance of MetadataSearch
 * @param	onSearch	listener
 */
void		OPL_METADATASEARCH_SetListener (OPL_HANDLE self, OPL_SearchListener_t onSearch);

/**
 * Set Query, 이전의 쿼리는 삭제됨
 *
 * @param	self instance of MetadataSearch
 * @param	query	instance of Query
 */
void		OPL_METADATASEARCH_SetQuery (OPL_HANDLE self, const OPL_HANDLE query);
OPL_HANDLE	OPL_METADATASEARCH_GetQuery (OPL_HANDLE self);

/**
 * Rating Constraint
 *
 * Not implemented yet...
 * @param	self instance of MetadataSearch
 *
 */
void		OPL_METADATASEARCH_AddRatingConstraint (OPL_HANDLE self, void *scheme, int threshold);

/**
 * Clear Rating Constraint
 * Not implemented yet...
 * @param	self instance of MetadataSearch
 */
void		OPL_METADATASEARCH_ClearRatingConstraint (OPL_HANDLE self);

/**
 * Set Target Constraint (support only EIT-P/F, EIT-S)
 *
 * @param	self 	instance of MetadataSearch
 * @param	target	"EIT-P/F", "EIT-S"
 */
void		OPL_METADATASEARCH_AddTargetConstraint (OPL_HANDLE self, const char *target);

/**
 * Add Channel Constraint (MetadataSearch 객체에 계속 쌓인다)
 *
 * @param	self instance of MetadataSearch
 * @param	ch	instance of Channel
 */
void		OPL_METADATASEARCH_AddChannelConstraint (OPL_HANDLE self, OPL_Channel_t channel);

/**
 * Clear All Channel Constraints
 *
 * @param	self instance of MetadataSearch
 */
void		OPL_METADATASEARCH_ClearChannelConstraint (OPL_HANDLE self);

/**
 * Add Ordering method (계속 쌓임)
 * 만약 field가 NULL이면, 그동안 쌓인 정렬 방법들이 모두 사라짐
 *
 * @param	self instance of MetadataSearch
 * @param	field
 * @param	ascending
 */
void		OPL_METADATASEARCH_OrderBy (OPL_HANDLE self, const char *field, OPL_BOOL ascending);

/**
 * Query, Constraint, OrderBy, 를 기준으로 서치를 시작함
 * 만약 캐싱되어 있는 데이터 중, 조건에 부합하는 데이터는 즉시 리턴하고, 그렇지 않은 데이터 들은
 * 이벤트를 받으면 업데이트 된다.
 *
 * @param	self instance of MetadataSearch
 * @return	SearchResults
 */
OPL_BOOL	OPL_METADATASEARCH_Search (OPL_HANDLE self);

/**
 * 현재 까지 받은 데이터를 전달
 *
 * @param	self instance of MetadataSearch
 * @return	SearchResults
 */
OPL_SearchResults_t	*OPL_METADATASEARCH_GetResults (OPL_HANDLE self);

void		OPL_METADATASEARCH_ReleaseResults (OPL_SearchResults_t *result);
void		OPL_METADATASEARCH_AbortResults   (OPL_SearchResults_t *result);
OPL_BOOL	OPL_METADATASEARCH_UpdateResults  (OPL_SearchResults_t *result);


/**
 * 지금까지 받은 데이터를 모두 지우고, 데이터를 받는 작업을 중단한다.
 *
 * @param	self instance of MetadataSearch
 */
void		OPL_METADATASEARCH_Abort (OPL_HANDLE self);

void		OPL_METADATASEARCH_SearchPF (OPL_HANDLE self, int svcuid, OPL_SearchPfMonitor_t monitor, void *userdata);


#ifdef __cplusplus
};
#endif	//__cplusplus

#endif	//__OPLMETADATASEARCH_H__


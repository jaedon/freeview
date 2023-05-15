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
 * - MetadataSearch Handle�� Programme���� ���Ӽ��� �ο��Ͽ�, WebApp�� ������ �� ���� GC ȣ��� ����
 *   MetadataSearch �� Deletion�� �߻����� �ʵ��� �Ѵ�.
 * - MetadataSearch Handle�� �����Ǵ� ���, ������Ʈ �̺�Ʈ �ڵ鸵�� ���� �ʱ� ������ ���� ���� ó�� ��
 * - �Ʒ� �Լ��� ����ϸ� 'self' MetadataSearch Handle�� ���� ���� ��� ProgrammeHandle�� release �Ǳ�
 *   ������ MetadataSearch Handle�� ������ �� ����.
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
 * Set Query, ������ ������ ������
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
 * Add Channel Constraint (MetadataSearch ��ü�� ��� ���δ�)
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
 * Add Ordering method (��� ����)
 * ���� field�� NULL�̸�, �׵��� ���� ���� ������� ��� �����
 *
 * @param	self instance of MetadataSearch
 * @param	field
 * @param	ascending
 */
void		OPL_METADATASEARCH_OrderBy (OPL_HANDLE self, const char *field, OPL_BOOL ascending);

/**
 * Query, Constraint, OrderBy, �� �������� ��ġ�� ������
 * ���� ĳ�̵Ǿ� �ִ� ������ ��, ���ǿ� �����ϴ� �����ʹ� ��� �����ϰ�, �׷��� ���� ������ ����
 * �̺�Ʈ�� ������ ������Ʈ �ȴ�.
 *
 * @param	self instance of MetadataSearch
 * @return	SearchResults
 */
OPL_BOOL	OPL_METADATASEARCH_Search (OPL_HANDLE self);

/**
 * ���� ���� ���� �����͸� ����
 *
 * @param	self instance of MetadataSearch
 * @return	SearchResults
 */
OPL_SearchResults_t	*OPL_METADATASEARCH_GetResults (OPL_HANDLE self);

void		OPL_METADATASEARCH_ReleaseResults (OPL_SearchResults_t *result);
void		OPL_METADATASEARCH_AbortResults   (OPL_SearchResults_t *result);
OPL_BOOL	OPL_METADATASEARCH_UpdateResults  (OPL_SearchResults_t *result);


/**
 * ���ݱ��� ���� �����͸� ��� �����, �����͸� �޴� �۾��� �ߴ��Ѵ�.
 *
 * @param	self instance of MetadataSearch
 */
void		OPL_METADATASEARCH_Abort (OPL_HANDLE self);

void		OPL_METADATASEARCH_SearchPF (OPL_HANDLE self, int svcuid, OPL_SearchPfMonitor_t monitor, void *userdata);


#ifdef __cplusplus
};
#endif	//__cplusplus

#endif	//__OPLMETADATASEARCH_H__


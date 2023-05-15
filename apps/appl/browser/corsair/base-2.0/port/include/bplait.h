/**************************************************************************************/
/**
 * @file bplait.h
 *
 * AIT Interfaces
 *
 **************************************************************************************
 **/

#ifndef __BPL_AIT_H__
#define __BPL_AIT_H__

#include "bpltype.h"

/******************************************************************
	Define
******************************************************************/

/******************************************************************
	Enum
 ******************************************************************/
typedef enum {
	BPLAIT_EVENT_AIT_NONE		= 0,
	BPLAIT_EVENT_AIT,
	BPLAIT_EVENT_AIT_NOSIGNAL,
	BPLAIT_EVENT_AIT_CHAN_STARTED,
	BPLAIT_EVENT_AIT_CHAN_STOPPED,
	BPLAIT_EVENT_AIT_CHAN_CHANGED,
	BPLAIT_EVENT_AIT_CHAN_CHANGED_BY_MHEG_NDT
} eBplAitEvent;

/******************************************************************
	Typedef
******************************************************************/

typedef struct _TableStructure {
	HUINT8		ulSecCnt;
	HUINT8		*arSecData[256];
} SiTable_t;

typedef struct _BplAitSectionData_t {
#if 1//defined(SDK_3_2)
	SiTable_t		stSiTable;
#else
	char			*pBuf;		/* this buffer SHOULD be freed by user */
#endif
	unsigned int	ulTotalLen;
	unsigned int	ulTotalSecNum;
} BplAitSectionData_t;

typedef struct _BplAitService_t{
	unsigned short	usOnID;
	unsigned short	usTSID;
	unsigned short	usSID;
	void	*chHandle;
}BplAitService_t;

typedef void (*BplAitListener_t)(eBplAitEvent eEvent, unsigned int ulParam);

/******************************************************************
	Interfaces
******************************************************************/

#ifdef __cplusplus
extern "C" {
#endif

BPLStatus BPL_AIT_SetListener(BplAitListener_t listener);

BPLStatus BPL_AIT_GetCount(unsigned int *pulCount);

BPLStatus BPL_AIT_GetSectionData(BplAitSectionData_t **ppAitInfo);

BPLStatus BPL_AIT_FreeSectionData(BplAitSectionData_t *pAitInfo);

BPLStatus BPL_AIT_GetServiceInfo( BplAitService_t *pService );

BPLStatus BPL_AIT_GetHbbTVEnabled( int *pnEnabled );

#ifdef __cplusplus
};

/******************************************************************
	class BplAitManager
******************************************************************/
typedef BplAitSectionData_t	BplAitSectionData;

class BplAitManager {
public:
	BplAitManager() {}
	virtual ~BplAitManager() {}

public:
	bool setListener(BplAitListener_t listener)
	{ return BPL_AIT_SetListener(listener) == BPL_STATUS_OK; }

	unsigned int getCount( void );

	BplAitSectionData_t *getData( void );
};

#endif

#endif //#ifndef __BPL_AIT_H__

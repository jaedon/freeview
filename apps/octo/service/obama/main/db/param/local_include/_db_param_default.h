
#ifndef	__DB_PARAM_DEFAULT_INT_H__
#define	__DB_PARAM_DEFAULT_INT_H__

/*******************************************************************/
/********************      Header Files    *************************/
/*******************************************************************/
#include <dlib.h>

#include <octo_common.h>

#include <db_param.h>

/*******************************************************************/
/********************      Macro Definition     ********************/
/*******************************************************************/
#define CFGITEM_TEXT_LEN					32

/*******************************************************************/
/********************      Typedef         *************************/
/*******************************************************************/
typedef struct
{
	HINT32 		 nItemValue;					/**< item value 1 - integer */
	HINT8 		 szItemText[CFGITEM_TEXT_LEN];	/**< item value 2 - text */
	HINT32 		 nDataSize;						/**< binary data�� size, 0�ΰ�� pucData�� NULL�̵� */
	HUINT8 		*pucData;						/**< item value 3 - binary data, ������� ������ NULL�� �����ϱ� �ٶ� */
} dbParam_CfgItemInfo_t;

typedef struct
{
	DbParam_Item_e				eItem;
	dbParam_CfgItemInfo_t 		stInfo;
} dbParam_DefaultItem_t, dbParam_KeepingItem_t;

/*******************************************************************/
/********************      Global Variables     ********************/
/*******************************************************************/

/*******************************************************************/
/********************      Public Functions     ********************/
/*******************************************************************/

#endif /* __DB_PARAM_DEFAULT_INT_H__ */


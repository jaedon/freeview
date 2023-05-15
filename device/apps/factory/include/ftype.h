/******************************************************************************
* $Workfile:$
* $Modtime:$
* Author :
* description :		 Factory Test S/W Type
*
* Copyright (c) 2008 by Humax Co., Ltd.
* All right reserved
******************************************************************************/
#ifndef _FTYPE_H_
#define _FTYPE_H_

#ifndef NULL
#define NULL 0
#endif

#ifndef TRUE
#define TRUE 1
#endif

#ifndef FALSE
#define FALSE 0
#endif

#ifndef ON
#define ON 1
#endif

#ifndef OFF
#define OFF 0
#endif

#ifndef DEFINED_BOOL
#define DEFINED_BOOL
typedef int BOOL;
#endif

#define UI_ITEM_INFO(action_item, n, sub_item)		action_item##_TEST##n##_UI_##sub_item
#define CONFIG_ITEM_INFO(item, n, sub_item)		item##_##n##_##sub_item

#endif /* _FTYPE_H_ */


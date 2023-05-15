/***************************************************************************
* Copyright (c) 2002-2012, Broadcom Corporation
* All Rights Reserved
* Confidential Property of Broadcom Corporation
*
* THIS SOFTWARE MAY ONLY BE USED SUBJECT TO AN EXECUTED SOFTWARE LICENSE
* AGREEMENT BETWEEN THE USER AND BROADCOM. YOU HAVE NO RIGHT TO USE OR
* EXPLOIT THIS MATERIAL EXCEPT SUBJECT TO THE TERMS OF SUCH AN AGREEMENT.
*
* $brcm_Workfile: controller.h $
* $brcm_Revision: 1 $
* $brcm_Date: 4/6/12 12:43p $
*
* Module Description:
*
* Revision History:
*
* $brcm_Log: /BSEAV/app/brutus2/src/controller.h $
* 
* 1   4/6/12 12:43p tokushig
* SW7231-749: fixed callback handling issue in register/notification
* mechanism which prevented control from manually notifying views.  view
* and controller classes now hide the callback mechanism from the mvc
* based classes for notifications and there is a consistent
* processNotification() method for both views and controllers (models do
* not receive notifications.
* 
* 3   2/28/12 5:03p tokushig
* SW7405-5581: add header comments
* 
* 
*********************************************************************************/

#ifndef CONTROLLER_H__
#define CONTROLLER_H__

#include "notification.h"
#include "mvc.h"

#ifdef __cplusplus
extern "C" {
#endif


class CController : public CMvcController
{
public:
    CController(void);
};


#ifdef __cplusplus
}
#endif

#endif /* CONTROLLER_H__ */

/***************************************************************************
* Copyright (c) 2002-2012, Broadcom Corporation
* All Rights Reserved
* Confidential Property of Broadcom Corporation
*
* THIS SOFTWARE MAY ONLY BE USED SUBJECT TO AN EXECUTED SOFTWARE LICENSE
* AGREEMENT BETWEEN THE USER AND BROADCOM. YOU HAVE NO RIGHT TO USE OR
* EXPLOIT THIS MATERIAL EXCEPT SUBJECT TO THE TERMS OF SUCH AN AGREEMENT.
*
* $brcm_Workfile: controller.cpp $
* $brcm_Revision: 1 $
* $brcm_Date: 4/6/12 12:43p $
*
* Module Description:
*
* Revision History:
*
* $brcm_Log: /BSEAV/app/brutus2/src/controller.cpp $
* 
* 1   4/6/12 12:43p tokushig
* SW7231-749: fixed callback handling issue in register/notification
* mechanism which prevented control from manually notifying views.  view
* and controller classes now hide the callback mechanism from the mvc
* based classes for notifications and there is a consistent
* processNotification() method for both views and controllers (models do
* not receive notifications.
* 
* 2   2/28/12 5:03p tokushig
* SW7405-5581: add header comments
* 
* 
*********************************************************************************/

#include "controller.h"
#include "brutus.h"
#include "notification.h"

BDBG_MODULE(brutus_controller);


static void controllerNotifyCallback(CObserver * context, CNotification & notification)
{
    CController * pController = (CController *)context;
    BDBG_ASSERT(NULL != pController);

    pController->processNotification(notification);
}

CController::CController() :
    CMvcController(controllerNotifyCallback, this)
{
}

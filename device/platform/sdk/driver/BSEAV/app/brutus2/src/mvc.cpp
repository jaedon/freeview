/***************************************************************************
* Copyright (c) 2002-2012, Broadcom Corporation
* All Rights Reserved
* Confidential Property of Broadcom Corporation
*
* THIS SOFTWARE MAY ONLY BE USED SUBJECT TO AN EXECUTED SOFTWARE LICENSE
* AGREEMENT BETWEEN THE USER AND BROADCOM. YOU HAVE NO RIGHT TO USE OR
* EXPLOIT THIS MATERIAL EXCEPT SUBJECT TO THE TERMS OF SUCH AN AGREEMENT.
*
* $brcm_Workfile: mvc.cpp $
* $brcm_Revision: 3 $
* $brcm_Date: 4/6/12 12:43p $
*
* Module Description:
*
* Revision History:
*
* $brcm_Log: /BSEAV/app/brutus2/src/mvc.cpp $
* 
* 3   4/6/12 12:43p tokushig
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

#include "mvc.h"

BDBG_MODULE(brutus_mvc);


CMvcView::CMvcView(NOTIFY_CALLBACK notifyCallback, CObserver * context) :
    CObserver(notifyCallback, context),
    CSubject()
{
}

void CMvcView::processNotification(CNotification & notification)
{
}

CMvcModel::CMvcModel()
{
}


CMvcController::CMvcController(NOTIFY_CALLBACK notifyCallback, CObserver * context) :
    CObserver(notifyCallback, context)
{
}

void CMvcController::processNotification(CNotification & notification)
{
}


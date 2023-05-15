/***************************************************************************
* Copyright (c) 2002-2012, Broadcom Corporation
* All Rights Reserved
* Confidential Property of Broadcom Corporation
*
* THIS SOFTWARE MAY ONLY BE USED SUBJECT TO AN EXECUTED SOFTWARE LICENSE
* AGREEMENT BETWEEN THE USER AND BROADCOM. YOU HAVE NO RIGHT TO USE OR
* EXPLOIT THIS MATERIAL EXCEPT SUBJECT TO THE TERMS OF SUCH AN AGREEMENT.
*
* $brcm_Workfile: view.h $
* $brcm_Revision: 5 $
* $brcm_Date: 9/21/12 4:25p $
*
* Module Description:
*
* Revision History:
*
* $brcm_Log: /BSEAV/app/brutus2/src/view.h $
* 
* 5   9/21/12 4:25p tokushig
* SW7231-749: remove channel manager from view class
* 
* 4   4/6/12 12:43p tokushig
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

#ifndef VIEW_H__
#define VIEW_H__

#include "notification.h"
#include "mvc.h"

class CModel;
class CModelChannelMgr;

#ifdef __cplusplus
extern "C" {
#endif


class CView : public CMvcView
{
public:
    CView(void);

    void setModel(CModel * pModel) { _pModel = pModel; };
    CModel * getModel(void) { return _pModel; };

protected:
    CModel                * _pModel;
};


#ifdef __cplusplus
}
#endif

#endif /* VIEW_H__ */

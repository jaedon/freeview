/***************************************************************************
* Copyright (c) 2002-2012, Broadcom Corporation
* All Rights Reserved
* Confidential Property of Broadcom Corporation
*
* THIS SOFTWARE MAY ONLY BE USED SUBJECT TO AN EXECUTED SOFTWARE LICENSE
* AGREEMENT BETWEEN THE USER AND BROADCOM. YOU HAVE NO RIGHT TO USE OR
* EXPLOIT THIS MATERIAL EXCEPT SUBJECT TO THE TERMS OF SUCH AN AGREEMENT.
*
* $brcm_Workfile: panel_tuner.cpp $
* $brcm_Revision: 9 $
* $brcm_Date: 11/15/12 10:46a $
*
* Module Description:
*
* Revision History:
*
* $brcm_Log: /BSEAV/app/brutus2/src/panel_tuner.cpp $
* 
* 9   11/15/12 10:46a tokushig
* SW7429-369: fix build for 97429/97241 and other platforms where
* NEXUS_HAS_FRONTEND is false
* 
* 8   10/19/12 12:57p tokushig
* SW7231-749: handle use case where tuner constellation is shown while no
* current channel exists (empty channel list)
* 
* 7   10/19/12 10:37a tokushig
* SW7231-749: fix brutus shutdown by freeing resources in the proper
* order.
* 
* 6   10/11/12 11:42a tokushig
* SW7231-749: increase grid draw cycles before clear
* 
* 5   10/11/12 11:34a tokushig
* SW7231-749: fix back button focus issue
* 
* 3   10/11/12 11:18a tokushig
* SW7231-749: shrink constellation panel and make square
* 
* 2   10/8/12 10:54p jrubio
* SW7346-1004: fix panel tuner
* 
* 1   10/8/12 4:32p jrubio
* SW7346-1004: add tuner panel
* 
* 
*********************************************************************************/
#if NEXUS_HAS_FRONTEND

#include "avtypes.h"
#include "tuner.h"
#include "channelmgr.h"
#include "channel.h"

#include "panel_tuner.h"

BDBG_MODULE(panel_tuner);

CPanelTuner::CPanelTuner(bwidget_engine_t   engine, 
                               CWidgetBase      * pParentWidget, 
                               MRect              geometry,
                               bwin_font_t        font,
                               bwin_t             parentWin) :
    CPanel(engine, pParentWidget, geometry, font, parentWin),
    _pTunerMenu(NULL),
    _pTunerGrid(NULL),
    _pSoftDecisions(NULL),
    _timerGrid(engine, this, 500),
    _counter(0),
    _Back(NULL)
{
}

CPanelTuner::~CPanelTuner()
{
    uninitialize();
}

eRet CPanelTuner::initialize(CModel * pModel, CModelConfig * pModelConfig)
{
    eRet                  ret            = eRet_Ok;
    CGraphics           * pGraphics      = NULL;
    bwin_font_t           font           = NULL;
    uint32_t              graphicsWidth  = 0;
    uint32_t              graphicsHeight = 0;
    int                   menuWidth      = 300; 
    int                   menuHeight     = 330;
    MRect                 rectPanel;

    BDBG_ASSERT(NULL != pModel);
    BDBG_ASSERT(NULL != pModelConfig);

    setModel(pModel);
    setModelConfig(pModelConfig);

    _numPoints = GET_INT(_pCfg, GRID_POINT_MAX);

    _pSoftDecisions = (NEXUS_FrontendSoftDecision *) BKNI_Malloc(_numPoints*sizeof(NEXUS_FrontendSoftDecision));
    if ( NULL == _pSoftDecisions )
    {
        ret = eRet_OutOfMemory;
        goto error;
    }

    pGraphics = _pModel->getGraphics();
    BDBG_ASSERT(NULL != pGraphics);

    font = pGraphics->getFont();
    BDBG_ASSERT(NULL != font);
        
    graphicsWidth  = GET_INT(_pCfg, GRAPHICS_SURFACE_WIDTH);
    graphicsHeight = GET_INT(_pCfg, GRAPHICS_SURFACE_HEIGHT);

    /* set the size of the panel itself */
    rectPanel.set(50, 50, menuWidth, menuHeight);
    setGeometry(rectPanel);

    _pTunerMenu = new CWidgetMenu(getEngine(), this, MRect(0, 0, menuWidth, menuHeight), font);
    CHECK_PTR_GOTO("unable to allocate menu widget", _pTunerMenu, ret, eRet_OutOfMemory, error);
    _pTunerMenu->setMenuTitle("Constellation", "Tuner");
    _pTunerMenu->show(true);
    _pTunerMenu->showListView(false);
    _pTunerMenu->showListViewSelection(false);
    _pTunerMenu->showSubTitle(false);
    _pTunerMenu->showEdit(false);

    {
        uint16_t gridMargin    = 7;
        uint16_t menuBarHeight = 30;
        MRect    rectGrid(gridMargin, 
                          menuBarHeight + gridMargin, 
                          menuWidth - (gridMargin * 2), 
                          menuHeight - menuBarHeight - (gridMargin * 2));

        _pTunerGrid = new CWidgetGrid(getEngine(),_pTunerMenu, rectGrid, font);
        _pTunerGrid->show(true);
        _pTunerGrid->setNumPoints(_numPoints);
    }

    _Back = new CWidgetButton(getEngine(), this, MRect(0, 0, 0, 0), font);
    CHECK_PTR_GOTO("unable to allocate button widget", _Back, ret, eRet_OutOfMemory, error);
    _Back->setText("Menu");
    _pTunerMenu->addBackButton(_Back);

    _Back->setFocus();

    goto done;
error:
    uninitialize();
done:
    return ret;
}

void CPanelTuner::uninitialize()
{
    DEL(_Back);
    DEL(_pTunerGrid);
    DEL(_pTunerMenu);
    if (_pSoftDecisions)
    {
       BKNI_Free(_pSoftDecisions);
    }
}


void CPanelTuner::onClick(bwidget_t widget)
{
    eNotification    notification  = eNotify_Invalid;
    CWidgetBase    * pWidget       = NULL;
    int i=0;

    /* find the widget object that corresponds to the given bwidget_t */
    {
        blabel_settings   labelSettings;
        blabel_get(widget, &labelSettings);
        pWidget = (CWidgetBase *)labelSettings.widget.data;
    }

    if (_Back == pWidget)
    {
        show(false);
        getParent()->show(true);
    }

    

done:
    return;
}

void CPanelTuner::show(bool bShow)
{
    CPanel::show(bShow);
    
    if (true == bShow)
    {
        _timerGrid.start(GET_INT(_pCfg, UI_GRID_UPDATE_TIMEOUT));
        
    }
    else
    {
        _timerGrid.stop();
    }
}

void CPanelTuner::processNotification(CNotification & notification)
{
    switch(notification.getId())
    {
    case eNotify_Timeout:
    {
        CTimer * pTimer = (CTimer *)notification.getData();
        if (&_timerGrid == pTimer)
        {
           updateGridData();
           _timerGrid.start();
        }
    }
        break;

    case eNotify_ChDown:
    case eNotify_ChUp:
    case eNotify_PlaybackStart:
       show(false);

    default:
        break;
    }

error:
    return;
}

/* Call get Soft Decisions */
eRet CPanelTuner::updateGridData()
{
    eRet                       ret              = eRet_Ok;
    CModelChannelMgr         * pChannelMgr      = _pModel->getModelChannelMgr();
    CModelChannel            * pChannel         = pChannelMgr->getCurrentChannel();
    CTuner                   * pTuner           = NULL;
    NEXUS_Error                nerror           = NEXUS_SUCCESS;
    uint32_t                   i                = 0;
    
    CHECK_PTR_GOTO("No current channel - unable to retrieve constellation data", pChannel, ret, eRet_NotAvailable, error);

    pTuner = pChannel->getTuner();

    if (_counter > 10)
    {
        /* Clear old data */
        _pTunerGrid->clearGrid();
        _counter=0;
    }
    _counter++;
    BKNI_Memset(_pSoftDecisions,0,_numPoints*sizeof(NEXUS_FrontendSoftDecision));

    /* Must be a valid Tuner */
    if (pTuner == NULL)
    {
       BDBG_WRN((" Must be a valide Tuner!"));
       ret = eRet_NotAvailable;
       goto error;
    }

    /* Get New Data*/
    nerror = pTuner->getSoftDecisions(_pSoftDecisions, _numPoints);
    if (nerror != NEXUS_SUCCESS)
    {
       ret = eRet_ExternalError;
       goto error;
    }

   for ( i = 0; i < _numPoints; i++ )
   {
      BDBG_MSG(("printint out coordinates %d x %d",_pSoftDecisions[i].i,_pSoftDecisions[i].q));
      _pTunerGrid->addCoordinate( _pSoftDecisions[i].i,_pSoftDecisions[i].q);
   }

error:
    return ret;
}

#endif /* NEXUS_HAS_FRONTEND */

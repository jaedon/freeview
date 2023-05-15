/***************************************************************************
* Copyright (c) 2002-2012, Broadcom Corporation
* All Rights Reserved
* Confidential Property of Broadcom Corporation
*
* THIS SOFTWARE MAY ONLY BE USED SUBJECT TO AN EXECUTED SOFTWARE LICENSE
* AGREEMENT BETWEEN THE USER AND BROADCOM. YOU HAVE NO RIGHT TO USE OR
* EXPLOIT THIS MATERIAL EXCEPT SUBJECT TO THE TERMS OF SUCH AN AGREEMENT.
*
* $brcm_Workfile: config.h $
* $brcm_Revision: 3 $
* $brcm_Date: 10/7/12 2:56p $
*
* Module Description:
*
* Revision History:
*
* $brcm_Log: /BSEAV/app/brutus2/src/config.h $
* 
* 3   10/7/12 2:56p tokushig
* SW7231-749: change video format enum to use nexus type
* 
* 2   2/28/12 5:03p tokushig
* SW7405-5581: add header comments
* 
* 
*********************************************************************************/

#ifndef CONFIG_H__
#define CONFIG_H__

#include "brutus_board.h"
#include "resource.h"
#include "mvc.h"
#include "nexus_platform_init.h"

/*
    CModelConfig manages the board features, board resources and runtime options
    for brutus2.
*/
class CModelConfig : public CMvcModel
{
public:
    virtual eRet initChipInfo(void);
    virtual eRet initFeatures(NEXUS_PlatformSettings platformSettings);
    virtual eRet initResources(NEXUS_PlatformConfiguration platformConfig);

    NEXUS_VideoFormat findMaxHdFormat(NEXUS_PlatformSettings platformSettings);
    eRet addChip(CChipInfo info);
    eRet removeChip(CChipInfo info);

    eRet initialize(void);

    CConfiguration * getCfg(void) { return &_cfg; };
    CBoardResources * getBoardResources(void) { return &_resources; };
    CBoardFeatures * getBoardFeatures(void) { return &_features; };

    CModelConfig(void);
    ~CModelConfig(void);

    void dump();

protected:
    MString                 _name;          /* name of the board */
    uint16_t                _revision;      /* board revision */
    MAutoList <CChipInfo>   _chipList;      /* list of Broadcom chips on the board */
    CBoardFeatures          _features;      /* features supported by the board */
    CBoardResources         _resources;     /* resources supported by the board */
    CConfiguration          _cfg;           /* application runtime options */
};



#endif /* CONFIG_H__ */

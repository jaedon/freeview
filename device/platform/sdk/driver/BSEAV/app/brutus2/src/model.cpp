/***************************************************************************
* Copyright (c) 2002-2012, Broadcom Corporation
* All Rights Reserved
* Confidential Property of Broadcom Corporation
*
* THIS SOFTWARE MAY ONLY BE USED SUBJECT TO AN EXECUTED SOFTWARE LICENSE
* AGREEMENT BETWEEN THE USER AND BROADCOM. YOU HAVE NO RIGHT TO USE OR
* EXPLOIT THIS MATERIAL EXCEPT SUBJECT TO THE TERMS OF SUCH AN AGREEMENT.
*
* $brcm_Workfile: model.cpp $
* $brcm_Revision: 5 $
* $brcm_Date: 9/21/12 8:28p $
*
* Module Description:
*
* Revision History:
*
* $brcm_Log: /BSEAV/app/brutus2/src/model.cpp $
* 
* 5   9/21/12 8:28p tokushig
* SW7231-749: move mode from control to model
* 
* 4   9/10/12 2:02p tokushig
* SW7231-749: add simple audio/video decoders to model
* 
* 3   5/4/12 2:04p tokushig
* SW7231-749: add buffers screen, checkbox widget, and new fonts
* 
* 2   2/28/12 5:03p tokushig
* SW7405-5581: add header comments
* 
* 
*********************************************************************************/

#include "model.h"
#include "notification.h"
#include "brutus.h"

BDBG_MODULE(brutus_model);

CModel::CModel() :
    _pDisplay(NULL),
    _pStc(NULL),
    _pMixer(NULL),
    _pVideoDecode(NULL),
    _pSimpleVideoDecode(NULL),
    _pAudioDecode(NULL),
    _pSimpleAudioDecode(NULL),
    _pIrRemote(NULL),
    _pPlayback(NULL),
    _pPlaybackList(NULL),
    _mode(eMode_Live),
    _id(0)
{
}


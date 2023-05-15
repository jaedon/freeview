/***************************************************************************
* Copyright (c) 2002-2012, Broadcom Corporation
* All Rights Reserved
* Confidential Property of Broadcom Corporation
*
* THIS SOFTWARE MAY ONLY BE USED SUBJECT TO AN EXECUTED SOFTWARE LICENSE
* AGREEMENT BETWEEN THE USER AND BROADCOM. YOU HAVE NO RIGHT TO USE OR
* EXPLOIT THIS MATERIAL EXCEPT SUBJECT TO THE TERMS OF SUCH AN AGREEMENT.
*
* $brcm_Workfile: resource.cpp $
* $brcm_Revision: 3 $
* $brcm_Date: 11/15/12 10:46a $
*
* Module Description:
*
* Revision History:
*
* $brcm_Log: /BSEAV/app/brutus2/src/resource.cpp $
* 
* 3   11/15/12 10:46a tokushig
* SW7429-369: fix build for 97429/97241 and other platforms where
* NEXUS_HAS_FRONTEND is false
* 
* 2   2/28/12 5:03p tokushig
* SW7405-5581: add header comments
* 
* 
*********************************************************************************/

#include "resource.h"

BDBG_MODULE(brutus_resource);


CResource::CResource(const char           * name, 
                     const uint16_t         number,   
                     const eBoardResource   type,
                     CConfiguration       * pCfg) :
    _name(name),
    _number(number),
    _type(type),
    _pCfg(pCfg),
    _checkedOut(false),
    _reserveId(NULL)
{
}

CResource::~CResource() 
{
}

eRet CResource::initialize()
{
    eRet ret = eRet_Ok;
    return ret;
}

void CResource::dump()
{
    BDBG_WRN(("<%d>%s:%d", _type, _name.s(), _number));
}

bool CResource::isOutput()
{
    bool isAnOutput = false;

    switch(_type)
    {
    case eBoardResource_outputSpdif:
    case eBoardResource_outputAudioDac:
    case eBoardResource_outputComponent:
    case eBoardResource_outputSVideo:
    case eBoardResource_outputComposite:
    case eBoardResource_outputHdmi:
        isAnOutput = true;
        break;
    default:
        isAnOutput = false;
        break;
    }

    return isAnOutput;
}

bool CResource::isVideoOutput()
{
    bool isAnOutput = false;

    switch(_type)
    {
    case eBoardResource_outputComponent:
    case eBoardResource_outputSVideo:
    case eBoardResource_outputComposite:
    case eBoardResource_outputHdmi:
        isAnOutput = true;
        break;
    default:
        isAnOutput = false;
        break;
    }

    return isAnOutput;
}

bool CResource::isAudioOutput()
{
    bool isAnOutput = false;

    switch(_type)
    {
    case eBoardResource_outputSpdif:
    case eBoardResource_outputAudioDac:
    case eBoardResource_outputHdmi:
        isAnOutput = true;
        break;
    default:
        isAnOutput = false;
        break;
    }

    return isAnOutput;
}

#if NEXUS_HAS_FRONTEND
bool CResource::isFrontend()
{
    bool isAFrontend = false;

    switch(_type)
    {
    case eBoardResource_frontendQam:
    case eBoardResource_frontendVsb:
    case eBoardResource_frontendSds:
    case eBoardResource_frontendIp:
    case eBoardResource_frontendOfdm:
        isAFrontend = true;
        break;
    default:
        isAFrontend = false;
        break;
    }

    return isAFrontend;
}
#endif


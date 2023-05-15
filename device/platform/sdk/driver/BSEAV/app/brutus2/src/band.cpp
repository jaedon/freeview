/***************************************************************************
* Copyright (c) 2002-2012, Broadcom Corporation
* All Rights Reserved
* Confidential Property of Broadcom Corporation
*
* THIS SOFTWARE MAY ONLY BE USED SUBJECT TO AN EXECUTED SOFTWARE LICENSE
* AGREEMENT BETWEEN THE USER AND BROADCOM. YOU HAVE NO RIGHT TO USE OR
* EXPLOIT THIS MATERIAL EXCEPT SUBJECT TO THE TERMS OF SUCH AN AGREEMENT.
*
* $brcm_Workfile: band.cpp $
* $brcm_Revision: 2 $
* $brcm_Date: 2/28/12 5:03p $
*
* Module Description:
*
* Revision History:
*
* $brcm_Log: /BSEAV/app/brutus2/src/band.cpp $
* 
* 2   2/28/12 5:03p tokushig
* SW7405-5581: add header comments
* 
* 
*********************************************************************************/

#include "band.h"

BDBG_MODULE(brutus_input_band);


CInputBand::CInputBand(const char * name, const uint16_t number, CConfiguration * pCfg) :
    CResource(name, number, eBoardResource_inputBand, pCfg)
{
    eRet                  ret       = eRet_Ok;

    _inputBand = (NEXUS_InputBand)number;
}

CInputBand::~CInputBand()
{
}

eRet CInputBand::initialize()
{
    eRet ret = eRet_Ok;
    return ret;
}


CParserBand::CParserBand(const char * name, const uint16_t number, CConfiguration * pCfg) :
    CResource(name, number, eBoardResource_parserBand, pCfg),
    _band((NEXUS_ParserBand)number)
{
}

CParserBand::~CParserBand()
{
}

eRet CParserBand::initialize()
{
    eRet ret = eRet_Ok;
    return ret;
}

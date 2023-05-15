/***************************************************************************
* Copyright (c) 2002-2012, Broadcom Corporation
* All Rights Reserved
* Confidential Property of Broadcom Corporation
*
* THIS SOFTWARE MAY ONLY BE USED SUBJECT TO AN EXECUTED SOFTWARE LICENSE
* AGREEMENT BETWEEN THE USER AND BROADCOM. YOU HAVE NO RIGHT TO USE OR
* EXPLOIT THIS MATERIAL EXCEPT SUBJECT TO THE TERMS OF SUCH AN AGREEMENT.
*
* $brcm_Workfile: band.h $
* $brcm_Revision: 2 $
* $brcm_Date: 2/28/12 5:03p $
*
* Module Description:
*
* Revision History:
*
* $brcm_Log: /BSEAV/app/brutus2/src/band.h $
* 
* 2   2/28/12 5:03p tokushig
* SW7405-5581: add header comments
* 
* 
*********************************************************************************/

#ifndef BAND_H__
#define BAND_H__

#include "resource.h"
#include "nexus_types.h"

#ifdef __cplusplus
extern "C" {
#endif


class CInputBand : public CResource
{
public:
    CInputBand(const char * name, const uint16_t number, CConfiguration * pCfg);
    ~CInputBand(void);
    
    virtual eRet initialize();
    NEXUS_InputBand getBand() { return _inputBand; };

protected:
    NEXUS_InputBand     _inputBand;
};


class CParserBand : public CResource
{
public:
    CParserBand(const char * name, const uint16_t number, CConfiguration * pCfg);
    ~CParserBand(void);
    
    virtual eRet initialize();
    NEXUS_ParserBand getBand() { return _band; };

protected:
    NEXUS_ParserBand          _band;
};


#ifdef __cplusplus
}
#endif

#endif /* BAND_H__ */

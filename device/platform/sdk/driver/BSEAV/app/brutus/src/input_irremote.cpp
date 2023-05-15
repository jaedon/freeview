/***************************************************************************
 *     Copyright (c) 2004-2012, Broadcom Corporation
 *     All Rights Reserved
 *     Confidential Property of Broadcom Corporation
 *
 *  THIS SOFTWARE MAY ONLY BE USED SUBJECT TO AN EXECUTED SOFTWARE LICENSE
 *  AGREEMENT  BETWEEN THE USER AND BROADCOM.  YOU HAVE NO RIGHT TO USE OR
 *  EXPLOIT THIS MATERIAL EXCEPT SUBJECT TO THE TERMS OF SUCH AN AGREEMENT.
 *
 * $brcm_Workfile: input_irremote.cpp $
 * $brcm_Revision: 5 $
 * $brcm_Date: 1/12/12 10:15a $
 *
 * Module Description: Brutus input device code
 *
 * Revision History:
 *
 * $brcm_Log: /BSEAV/app/brutus/src/input_irremote.cpp $
 * 
 * 5   1/12/12 10:15a jrubio
 * SW7346-400: add silver remote
 * 
 * 4   8/1/08 11:01a erickson
 * PR45286: rename BDBG_MODULE
 *
 * 3   5/9/06 11:40a jrubio
 * PR18491: Had to mod IR Echostar
 *
 * 2   1/16/06 4:31p jrubio
 * PR18491: added function for EchoStar UHF legacy mode remote
 *
 * 1   2/7/05 8:12p dlwin
 * Merge down for release 2005_REFSW_MERGETOMAIN:
 *
 * Irvine_BSEAVSW_Devel/6   1/19/05 4:59p biyongc
 * PR2807: BSE PR - roll back DSG demo IP video suppport
 *
 * Irvine_BSEAVSW_Devel/4   12/20/04 2:00p erickson
 * PR13604: pull out repeat code for IR remove B before forward on data
 *
 * Irvine_BSEAVSW_Devel/3   11/23/04 9:43a erickson
 * PR13336: reworked repeat key handling to allow more flexibility
 *
 * Irvine_BSEAVSW_Devel/2   4/16/04 11:27a erickson
 * PR9029: convert to BrutusCommand for device-independent command
 *
 * Irvine_BSEAVSW_Devel/1   4/16/04 10:44a erickson
 * PR9029: new object-oriented input device design
 *
 *****************************************************************/
#include "input_irremote.h"
#include "input_devicecodes.h"
#include "inputengine.h"
#include <bstd.h>
BDBG_MODULE(brutus_input_irremote);

void RemoteAInput::process(uint32_t data)
{
    unsigned long key = 0xFF & data;
    bool isrepeat = 0xFFFF0000 & data;

    BrutusCommand cmd;

    if (!translateCmd(dev_giRemote, key, &cmd))
        _inputEngine->processCmd(cmd, false, isrepeat);
}

void RemoteBInput::process(uint32_t data)
{
    unsigned long key = 0x00FFFFFF & data;
    bool isrepeat = 0x80000000 & data;

    BrutusCommand cmd;
    if (!translateCmd(dev_SARemote, key, &cmd))
        _inputEngine->processCmd(cmd, false, isrepeat);
}

void RemoteCInput::process(uint32_t data)
{
    unsigned long key = 0x00FFFFFF & data;
    bool isrepeat = 0x80000000 & data;

    BrutusCommand cmd;

    if (!translateCmd(dev_IREchoStar, key, &cmd))
        _inputEngine->processCmd(cmd, false, isrepeat);
}


void RemoteDInput::process(uint32_t data)
{
    unsigned long key = 0xFFFFFFFE & data;
    bool isrepeat = 0x00000001 & data; 

    BrutusCommand cmd;

    if (!translateCmd(dev_CirNecRemote, key, &cmd))
        _inputEngine->processCmd(cmd, false, isrepeat);
}


/***************************************************************************
 *     Copyright (c) 2006, Broadcom Corporation
 *     All Rights Reserved
 *     Confidential Property of Broadcom Corporation
 *
 *  THIS SOFTWARE MAY ONLY BE USED SUBJECT TO AN EXECUTED SOFTWARE LICENSE
 *  AGREEMENT  BETWEEN THE USER AND BROADCOM.  YOU HAVE NO RIGHT TO USE OR
 *  EXPLOIT THIS MATERIAL EXCEPT SUBJECT TO THE TERMS OF SUCH AN AGREEMENT.
 *
 * $brcm_Workfile: input_uhfremote.cpp $
 * $brcm_Revision: 3 $
 * $brcm_Date: 10/20/06 3:11p $
 *
 * Module Description: File that process UHF
 *
 * Revision History:
 *
 * $brcm_Log: /BSEAV/app/brutus/src/input_uhfremote.cpp $
 * 
 * 3   10/20/06 3:11p jrubio
 * PR24927: Ignore some EChostar inputs
 * 
 * 2   5/11/06 4:43p jrubio
 * PR18491: Corrective DirecTV mode
 * 
 * 1   5/9/06 11:53a jrubio
 * PR18491:Added new uhf remote file
 * 
 ***************************************************************************/
#include "input_uhfremote.h"
#include "input_devicecodes.h"
#include "inputengine.h"
#include <bstd.h>
BDBG_MODULE(brutus_input_uhf);


/**
 * We are using 32b instead of 40b please pre-append 0x10 if you
 * want a whole 40b value. First value is always a 1, stripping it
 * out.
 * 
 */
void uhfDirecTV::process(uint32_t data)
{
	unsigned long key = 0x0FFFFFFF & data;
	bool isrepeat = false;
	if((data>>28) == 9)
	    isrepeat = true;

	BrutusCommand cmd;

  
	if (!translateCmd(dev_uhfDirecTV, key, &cmd))
	    _inputEngine->processCmd(cmd, false, isrepeat);
      	           
}


void uhfEchostar::process(uint32_t data)
{
	unsigned long key = 0x0FFFFFFF & data;
	bool isrepeat = 0x80000000 & data;

	BrutusCommand cmd;

    /* On echostar remote we are ignoring some buttons*/
    if (key == 0x02e2033a || key == 4 || 
        key == 0x2f203cd || /* "DISH" */
        key == 0x34240ad || /* "DISH" */
        key == 0x34240a9 || /* "R" */
        key == 0x2921b32 ||/* "SAT" */
        key == 0x25202d2 ||/* "*" */
        key == 0x26203eb ||/* "#" */
        key == 0x12026a /* repeat "SAT" button press? */)
    {
        return;
    }
	if (!translateCmd(dev_uhfEchostar, key, &cmd))
		_inputEngine->processCmd(cmd, false, isrepeat);
}


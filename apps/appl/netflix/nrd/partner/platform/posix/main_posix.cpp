
/*
 * (c) 1997-2015 Netflix, Inc.  All content herein is protected by
 * U.S. copyright and other applicable intellectual property laws and
 * may not be copied without the express permission of Netflix, Inc.,
 * which reserves all rights.  Reuse of any of this content for any
 * purpose without the permission of Netflix, Inc. is strictly
 * prohibited.
 */

#include <GibbonApplication.h>

using namespace netflix::gibbon;
using namespace netflix;

void gibbon_oem_event(GibbonOEMEvent event, void *)
{
    switch(event) {
    case GibbonOEM_Init:
        break;
    case GibbonOEM_Start:
        break;
   case GibbonOEM_Suspended:
        break;
    case GibbonOEM_Started:
        break;
    case GibbonOEM_Rendered:
        break;
    case GibbonOEM_Cleanup:
        break;
    case GibbonOEM_WatchdogNoHeartbeat:
    case GibbonOEM_WatchdogSigChild:
        break;
    }
}


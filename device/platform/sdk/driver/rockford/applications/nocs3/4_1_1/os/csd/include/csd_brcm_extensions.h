#ifndef CSD_EXENTIONS_H
#define CSD_EXENTIONS_H


#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "nocs_csd_impl.h"
#include "nocs_csd.h"
#include "nexus_playpump.h"

/**********************************************************************************************
						 Start CSD API Brcm Enhancements
**********************************************************************************************/

TCsdStatus csdBrcmOpenR2RKeyPath
(
    TCsdR2RKeyPathHandle** pR2RKeyPathOut,
    unsigned int    xTransportSessionId
);


TCsdStatus csdBrcmCloseR2RKeyPath
(
    TCsdR2RKeyPathHandle* pR2RKeyPath
);


TCsdStatus csdBrcmOpenScrKeyPath
(
    TCsdScrKeyPathHandle** pScrKeyPathOut,
    unsigned int    xTransportSessionId
);

NEXUS_KeySlotHandle csdBrcmGetScrKeyHandle
(
    TCsdScrKeyPathHandle* pScrKeyPath
);

TCsdStatus csdBrcmCloseScrKeyPath
(
    TCsdScrKeyPathHandle* pScrKeyPath
);


TCsdStatus csdBrcmOpenDscKeyPath
(
    TCsdDscKeyPathHandle** pDscKeyPathOut, 
    unsigned int    xTransportSessionId,
    NEXUS_PlaypumpHandle playpumpHandle
);

NEXUS_KeySlotHandle csdBrcmGetDscKeyHandle
(
    TCsdDscKeyPathHandle* pDscKeyPath
);

TCsdStatus csdBrcmCloseDscKeyPath
(
    TCsdDscKeyPathHandle* pDscKeyPath
);

TCsdStatus csdBrcmSetDscPlaypumpHandle
(
    TCsdDscKeyPathHandle* pDscKeyPath,
    NEXUS_PlaypumpHandle playpumpHandle
);

TCsdStatus csdBrcmSetDscPid
(
    TCsdDscKeyPathHandle* pDscKeyPath,
    unsigned int xPid
);


TCsdStatus csdBrcmRemoveDscPid
(
    TCsdDscKeyPathHandle* pDscKeyPath,
    unsigned int xPid
);

/**********************************************************************************************
						 End CSD API Brcm Enhancements
**********************************************************************************************/


#endif	

/* END OF FILE */	


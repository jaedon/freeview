/*
   Copyright

    This source code is VIACCESS intellecutal property an is strictly confidential.

    The receiver undertakes to use it within the strict and sole framework of the

    licence agreement entered into with VIACCESS pertaining to the use of such

    source code. No part of VIACCESS source code may be reproduced or transmitted 

    in any form or by any means, electronic or mechanical, including photocopying, 

    recording, or by any information storage and retrieval system, without permission 

    in writing from VIACCESS. The information in this code is subject to change 

    without notice. VIACCESS does not warrant that this code is error free. If you 

    find any problems with this code or wish to make comments, please report them 

    to VIACCESS.

 

    Trademarks 

    Viaccess is a registered trademark of Viaccess ® in France and/or other 

    countries. All other product and company names mentioned herein are the 

    trademarks of their respective owners.

    Viaccess may hold patents, patent applications, trademarks, copyrights 

    or other intellectual property rights over the code hereafter. Unless 

    expressly specified otherwise in a Viaccess written license agreement, the

    delivery of this code does not imply the concession of any license over 

    these patents, trademarks, copyrights or other intellectual property.
*/
/*
    date        : 30/06/2006
    file        : va_opentv_Ocode.h
    Description : It is the Viaccess'API of CAS functionalities
*/

#ifndef _VA_OPENTV_O_CODE_H_
#define _VA_OPENTV_O_CODE_H_

#include "va_types.h"
#include "va_opentv_Otype.h"
#include "va_ui.h"

/* Function prototypes */
extern INT   O_VA_UI_SetFocus(tO_VA_UI_Action Action, WORD wEventCategory);
extern INT   O_VA_UI_Request(tVA_UI_Request * pRequest);
extern BYTE  O_VA_UI_PipeTagToPipeId (DWORD dwPipeTag);
extern DWORD O_VA_UI_PipeIdToPipeTag (BYTE dwPipeId);

#endif /* _VA_OPENTV_O_CODE_H_ */

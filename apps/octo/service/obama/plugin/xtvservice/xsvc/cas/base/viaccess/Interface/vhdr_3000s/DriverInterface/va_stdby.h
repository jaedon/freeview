/******************************************************************************
 
    Copyright
    This code is strictly confidential and the receiver is obliged to use it 
    exclusively for his or her own purposes. No part of Viaccess code may be 
    reproduced or transmitted in any form or by any means, electronic or 
    mechanical, including photocopying, recording, or by any information storage 
    and retrieval system, without permission in writing from Viaccess. 
    The information in this code is subject to change without notice. Viaccess 
    does not warrant that this code is error free. If you find any problems 
    with this code or wish to make comments, please report them to Viaccess.
 
    Trademarks 
    Viaccess is a registered trademark of Viaccess ® in France and/or other 
    countries. All other product and company names mentioned herein are the 
    trademarks of their respective owners.
    Viaccess may hold patents, patent applications, trademarks, copyrights 
    or other intellectual property rights over the code hereafter. Unless 
    expressly specified otherwise in a Viaccess written license agreement, the
    delivery of this code does not imply the concession of any license over 
    these patents, trademarks, copyrights or other intellectual property.
 
******************************************************************************/

/*
    $Revision: 3890 $
    $Date: 2007-01-16 11:43:25 +0100 (mar., 16 janv. 2007) $
*/

#ifndef _VA_STDBY_H_
#define _VA_STDBY_H_

#include "va_types.h"
#include "va_errors.h"
#include "va_os.h"


/* Implemented by Viaccess */
void VA_STDBY_EnterStandby(void);
void VA_STDBY_NotifyAlarm (void);
void VA_STDBY_ExitStandby (void);

/* Implemented by STB */
void VA_STDBY_StandbyProcessDone(void);
void VA_STDBY_AlarmProcessDone  (void);
INT  VA_STDBY_SetAlarm(tVA_OS_Tm * const pTime);

#endif /* _VA_STDBY_H_ */


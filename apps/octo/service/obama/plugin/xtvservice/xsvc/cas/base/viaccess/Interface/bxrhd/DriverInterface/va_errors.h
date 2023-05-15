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


#ifndef _VA_ERRORS_H_
#define _VA_ERRORS_H_


/* Generic error codes */
#define kVA_ILLEGAL_HANDLE               0xFFFFFFFF
#define kVA_ERROR                        -1
#define kVA_INVALID_PARAMETER            -2
#define kVA_ALREADY_DONE                 -3
#define kVA_UNEXPECTED_CALLBACK          -4
#define kVA_RESOURCE_BUSY                -5
#define kVA_RESOURCE_NOT_LOCKED          -6
#define kVA_RESOURCE_STILL_LOCKED        -7
#define kVA_RESOURCE_ALREADY_LOCKED      -8
#define kVA_CONNECTION_LOST              -9

/* Specific error codes */
#define kVA_STOP_FILTER_FIRST            -100
#define kVA_SET_FILTER_PARAMETERS_FIRST  -101
#define kVA_SWITCH_ON_PROGRAM_FIRST      -102
#define kVA_SWITCH_OFF_PROGRAM_FIRST     -103
#define kVA_INSERT_CARD_FIRST            -104
#define kVA_RESET_CARD_FIRST             -105
#define kVA_OPEN_ACS_FIRST               -106
#define kVA_TRANSITION_NOT_ALLOWED       -107

#define kVA_OK                            0

/* Warning */
#define kVA_SECTION_REJECTED              1
#define kVA_NO_LOCKED_BUFFER              2
#define kVA_TIMEOUT                       3
#define kVA_DESCRIPTOR_CORRUPTED          4
#define kVA_NEXT_DESCRIPTOR               5
#define kVA_NO_MORE_DESCRIPTOR            6
#define kVA_DATA_NOT_AVAILABLE            7
#define kVA_NOT_IMPLEMENTED               8
#define kVA_RELEASE_RESOURCES             9

/* Common definitions */
#define kVA_WAIT_FOREVER                  0xFFFFFFFF

#endif /* _VA_ERRORS_H_ */


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
    Viaccess is a registered trademark of Viaccess ?in France and/or other
    countries. All other product and company names mentioned herein are the
    trademarks of their respective owners.
    Viaccess may hold patents, patent applications, trademarks, copyrights
    or other intellectual property rights over the code hereafter. Unless
    expressly specified otherwise in a Viaccess written license agreement, the
    delivery of this code does not imply the concession of any license over
    these patents, trademarks, copyrights or other intellectual property.

******************************************************************************/

#ifndef VA_TYPES_H
#define VA_TYPES_H

#include <stdlib.h>
#include <stddef.h>

/**
 @ingroup common
 */

/**
 @defgroup types va_types.h
 @brief This is Viaccess basic types' definition.
 @{
 */


#ifdef __BASETYPE_DEFINED__

#include <basetsd.h>

#else

/** 8 bits signed integer type definition */
typedef char            INT8    ;

/** 32 bits unsigned integer type definition */
typedef unsigned long   UINT32  ;

/** 32 bits signed integer type definition */
typedef long            INT32   ;

#endif


/** Byte type definition */
typedef unsigned char   BYTE    ;

/** 8 bits unsigned integer type definition */
typedef unsigned char   UINT8   ;

/** 16 bits word type definition */
typedef unsigned short  WORD    ;

/** 16 bits unsigned integer type definition */
typedef unsigned short  UINT16  ;

/** 16 bits signed integer type definition */
typedef short           INT16   ;

/** unsigned integer type definition */
typedef unsigned int    UINT    ;

/** signed integer type definition */
typedef int             INT     ;

/** 32 bits double word type definition */
typedef unsigned long   DWORD   ;

/** boolean definition */
typedef int             BOOL    ;

/** string definition */
typedef char *          CSTRING ;

/** FALSE constant definition */
#define FALSE           0

/** TRUE constant definition */
#define TRUE            1

/** @} */

#endif /* VA_TYPES_H */

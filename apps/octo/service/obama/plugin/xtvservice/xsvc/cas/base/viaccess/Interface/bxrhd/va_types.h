/*
    (C)opyright Viaccess S.A. 2004

    This software is the property of Viaccess and must not be distributed
    without written consent
*/
/*
    $Source$
    $Revision: 1 $
    $Date: 2006-07-17 16:59:39 +0200 (lun., 17 juil. 2006) $
*/


/**
    @file       va_types.h
    @ingroup    ACS Interface
    @brief      This is Viaccess basic types' definition.
*/



#ifndef VA_TYPES_H
#define VA_TYPES_H

#include <stdlib.h>
#include <stddef.h>


typedef unsigned char   BYTE    ;
typedef unsigned char   UINT8   ;
typedef char            INT8    ;
typedef unsigned short  WORD    ;
typedef unsigned short  UINT16  ;
typedef short           INT16   ;
typedef unsigned int    UINT    ;
typedef int             INT     ;
typedef unsigned long   DWORD   ;
typedef unsigned long   UINT32  ;
typedef long            INT32   ;
typedef int             BOOL    ;
typedef char *          CSTRING ;

#define FALSE           0
#define TRUE            1

#endif /* VA_TYPES_H */

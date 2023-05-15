/*****************************************************************************
******************************************************************************
*
*         File : bc_types.h
*
*  Description : type definitions used by the betacrypt library
*
*    Copyright : COMVENIENT  2008 (c)
*    Copyright : Verimatrix  2011 (c)
*
******************************************************************************
*****************************************************************************/

/**************************** CVS Infos ****************************************
*
*  $Source: /home/boxop/cvsroot/bc2_cam_src/bc_types.h,v $
*  $Date: 2012/12/17 12:27:13 $
*  $Revision: 1.2.1.1 $
*
***************************** CVS Infos ***************************************/

#ifndef _BC_TYPES_H_
#define _BC_TYPES_H_

// Types used by the betacrypt library
#if 1 // type의 컴파일 충돌로인해 v를 add함.
typedef  signed char    vint8_t;     /*  range :  -128 to 127               */
typedef  unsigned char  vuint8_t;    /*  range :  0 to 255                  */
typedef  signed short   vint16_t;    /*  range :  -32768 to 32767           */
typedef  unsigned short vuint16_t;   /*  range :  0 to 65535                */
typedef  signed long    vint32_t;    /*  range :  -2147483648 to 2147483647 */
typedef  unsigned long  vuint32_t;   /*  range :  0 to 4294967295           */
typedef  signed long long    vint64_t;    /*  range :  -2^63 to 2^63-1      */
typedef  unsigned long  long vuint64_t;   /*  range :  0 to 2^64-1          */
typedef  unsigned short bool_t;     /*  range :  0 to 1 (false, true)      */
typedef  float          vfloat32_t;  /*  +-1.175494E-38 to +-3.402823E+38   */
typedef  void           vvoid_t;     /*  range :  n.a.                      */
#else
typedef  signed char    int8_t;     /*  range :  -128 to 127               */
typedef  unsigned char  uint8_t;    /*  range :  0 to 255                  */
typedef  signed short   int16_t;    /*  range :  -32768 to 32767           */
typedef  unsigned short uint16_t;   /*  range :  0 to 65535                */
typedef  signed long    int32_t;    /*  range :  -2147483648 to 2147483647 */
typedef  unsigned long  uint32_t;   /*  range :  0 to 4294967295           */
typedef  signed long long    int64_t;    /*  range :  -2^63 to 2^63-1      */
typedef  unsigned long  long uint64_t;   /*  range :  0 to 2^64-1          */
typedef  unsigned short bool_t;     /*  range :  0 to 1 (false, true)      */
typedef  float          float32_t;  /*  +-1.175494E-38 to +-3.402823E+38   */
typedef  void           void_t;     /*  range :  n.a.                      */
#endif

#ifndef true
#define true  (1 == 1)
#endif
#ifndef false
#define false (1 == 0)
#endif

#endif
 //_BC_TYPES_H_

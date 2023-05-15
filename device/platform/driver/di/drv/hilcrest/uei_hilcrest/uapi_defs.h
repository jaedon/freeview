/**
 * @file        uapi_defs.h
 * @brief       This file contains the type definitions of different variable sizes
 * 
**/


/*===================================================================
 COPYRIGHT 2010 UNIVERSAL ELECTRONICS INC (UEI).
 These materials are provided under license by UEI.  UEI licenses
 this file to you under the Universal Remote Control API (UAPI)
 License accompanying the UEI libuapi Software (the "License").
 You may not use this file except in compliance with the License.
 You may obtain a copy of the License from UEI. Unless required by
 applicable law or agreed to in writing, all materials distributed
 under the License is distributed on an "AS IS" BASIS, WITHOUT
 WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 See the License for the specific language governing permissions
 and limitations under the License.
===================================================================*/

/*********************************************************************************************************
* Filename:     uapi_defs.h
* Description:  This .h file contains all the typedefintions of the standard data types
**********************************************************************************************************/

#ifndef __UAPI_DEFS_H__
#define __UAPI_DEFS_H__

#ifdef __cplusplus
extern "C"{
#endif

/*! \typedef UINT8
 *   UINT8 is defined as unsigned 8 bit
 */
typedef  unsigned char UINT8;
/*! \typedef INT8                        
 *   INT8 is defined as signed 8 bit
 */
typedef  char INT8;
/*! \typedef UINT16
 *   UINT16 is defined as unsigned 16 bit
 */
typedef  unsigned short UINT16;
/*! \typedef INT16                     
 *   INT16 is defined as signed 16 bit
 */
typedef  short INT16;
/*! \typedef INT32
 *   INT32 is defined as signed 32 bit
 */
typedef  int INT32;
/*! \typedef UINT32
 *   UINT32 is defined as unsigned 32 bit
 */
typedef  unsigned int UINT32;
/*! \typedef VOID 
 *   VOID is defined as void
 */
typedef  void VOID;

/*! \def TRUE
 *   TRUE is defined as 1
 */
#define TRUE 1

/*! \def FALSE
 *   FALSE is defined as 0
 */
#define FALSE 0

#ifdef __cplusplus
}
#endif
#endif

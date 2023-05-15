/**************************************************************
*	@file	rema.c
*	http://www.humaxdigital.com
*	@author			humax
*	@brief			rema
**************************************************************/

/*
* ¨Ï 2011-2012 Humax Co., Ltd.
* This program is produced by Humax Co., Ltd. ("Humax") and
* the proprietary Software of Humax and its licensors. Humax provides you, as an Authorized Licensee,
* non-assignable, non-transferable and non-exclusive license to use this Software.
* You acknowledge that this Software contains valuable trade secrets of Humax and by using this Software
* you agree to the responsibility to take all reasonable efforts to protect the any information
* you receive from Humax. You are not permitted to duplicate, modify, distribute, sell or lease and
* reverse engineer or extract the source code of this Software unless you have Humax's written permission to do so.
* If you have no authorized license, discontinue using this Software immediately.
*
* THE SOFTWARE IS PROVIDED "AS IS" AND HUMAX MAKES NO PROMISES, REPRESENTATIONS OR WARRANTIES, EITHER EXPRESS,
* IMPLIED OR STATUTORY, OR OTHERWISE, WITH RESPECT TO THE SOFTWARE.
* IN NO EVENT SHALL HUMAX BE LIABLE FOR LOST PROFITS, REVENUES, OR DATA, FINANCIAL LOSSES OR INDIRECT, SPECIAL,
*  CONSEQUENTIAL, EXEMPLARTY OR PUNITIVE DAMAGES WHATSOEVER RELATING TO YOUR USE OR INABILITY TO USE THE SOFTWARE.
*
* This License is effective until terminated. You may terminate this License at any time by destroying all copies
* of the Software including all documentation. This License will terminate immediately without notice from Humax
* to you if you fail to comply with any provision of this License. Upon termination, you must destroy all copies
* of the Software and all documentation.
*
* The laws of the Republic of Korea will apply to any disputes arising out of or relating to this Copyright Notice.
* All claims arising out of or relating to this Copyright Notice will be litigated in the Seoul Central District Court,
* in the Republic of Korea.
*/

// ÇÑ±Û
#define	________________Header_Files_______________________________

#include <vkernel.h>
#include <hlib.h>
#include <oapi.h>

#include <hapi.h>

#include <apk.h>

#include "test_CUnit_Api.h"

#include "test_cwmp.h"
#include "test_hcwmp_engine.h"





#define ________________Static_Variables___________________________


#define ________________Internal_Functions_________________________




#define ________________Main_Function______________________________



int		main(int argc, char *argv[])
{
	 CU_pSuite pSuite = NULL;

   /* initialize the CUnit test registry */
   if (CUE_SUCCESS != CU_initialize_registry())
      return CU_get_error();
   if(test_cwmp_register() == -1)
   {
	   CU_cleanup_registry();
	   return CU_get_error();
   }

   if(test_hcwmp_engine_register() == -1)
   {
	   CU_cleanup_registry();
	   return CU_get_error();
   }





   /* Run all tests using the basic interface */
   CU_basic_set_mode(CU_BRM_VERBOSE);
   CU_basic_run_tests();
   HxLOG_Print("\n");
   CU_basic_show_failures(CU_get_failure_list());
   HxLOG_Print("\n\n");

   /* Run all tests using the automated interface */
   CU_automated_run_tests();
   CU_list_tests_to_file();

   /* Run all tests using the console interface */
  // CU_console_run_tests();

   /* Run all tests using the curses interface */
   /* (only on systems having curses) */
   //CU_curses_run_tests();

   CU_cleanup_registry();
   return CU_get_error();

}
#define _____END_OF_FILE_________


/**
**  @file NOCS3x_Test_Harness.h
**
**  @brief
**    Test Harness variables.
**
**  @ingroup
**    Test
**
**  @author
**    NOCS team
**
**  @version
**    Revision: 1.3
**
**   
*/
/*
** REMARK:
**    Comments in this file use special tags to allow automatic test
**    documentation generation in HTML format, using the
**    GNU-General Public Licensed Doxygen tool.
**    For more information about Doxygen, please check www.doxygen.org
*/

/** 
 * @mainpage
 *
 * @brief
 *    NOCS Test harness for NOCS Secure chipset
 *
 *
 *  @section hist HISTORY
 *   $Log: __________ $
 *   Revision 1.0 2011/08/22 16:45:39 Nocs team
 *   First published version
 *   Revision 1.1 17:29 mardi, 27. septembre 2011 Nocs team
 *   Revision 1.2 10:25 mardi, 15. December 2011 Nocs team. 
 *   Revision 1.3 16:55 mardi, 21. Feb 2012 Nocs team. 
 *   	- Added the copyright info
 *
 *  @section cr COPYRIGHT
 *     Copyright (c) 2011 - 2012 Nagravision SA. All rights reserved.\n
 *     NAGRA and NAGRAVISION are registered trademarks of Kudelski SA, 1033 Cheseaux,
 *     Switzerland.\n
 *     Confidential and privileged information.
 *
 *     2011  The year of creation of the source file. \n
 *     2012  The year of last modification of the source file.\n
 *
 *
 */


#ifndef NOCS3X_TEST_HARNESS_H
#define NOCS3X_TEST_HARNESS_H

/******************************************************************************/
/*                                                                            */
/*                               INCLUDE FILES                                */
/*                                                                            */
/******************************************************************************/

/******************************************************************************/
/*                                                                            */
/*                              TYPES DEFINITIONS                             */
/*                                                                            */
/******************************************************************************/
/**
 *  @brief
 *    Verbosity levels defined to output messages.
 *
*/
typedef enum
{
  VERBOSITY_LEVEL_THM,
   /**<  Only messages (success, error) related to the Test Harness.
    */ 
  VERBOSITY_LEVEL_CMS,
   /**<  In addition to TH Messages, Chipset specific messages as defined by 
         the Chipset Manufacturer. Do not need to be formatted.
    */ 
  VERBOSITY_LEVEL_ALL,
   /**<  All messages following Chipset Manufacturers own verbosity levels.
    */ 
  LAST_VERBOSITY_LEVEL
   /**<  Not used.
    */
} TTHVerbosityLevel;

/******************************************************************************/
/*                                                                            */
/*                             GENERIC TYPES DEFINITIONS                      */
/*                                                                            */
/******************************************************************************/
#define NAGRA_EVEN_TABLE_ID        							0x80
#define NAGRA_ODD_TABLE_ID         							0x81

#define scrServiceAudioPid                                  0x1112
#define scrServiceVideoPid                                  0x1110
#define scrServicePCRPid                                    0x1110

struct SCertificationEcm1
{
 	TCsdUnsignedInt8 table_id;
	TCsdUnsignedInt8 reserved [6];
	TCsdUnsignedInt8 control_word_1 [8];
	TCsdUnsignedInt8 control_word_2 [8];
	TCsdUnsignedInt8 control_word_1_index;
	TCsdUnsignedInt8 control_word_2_index;
  TCsdUnsignedInt8 cken_flag_1;
  TCsdUnsignedInt8 cken_flag_2;
	TCsdUnsignedInt8 crc32 [4];

};

struct SCertificationEcm2
{
 	TCsdUnsignedInt8 table_id;
	TCsdUnsignedInt8 reserved [6];
	TCsdUnsignedInt8 control_word_1 [16];
	TCsdUnsignedInt8 control_word_2 [16];
	TCsdUnsignedInt8 control_word_1_index;
	TCsdUnsignedInt8 control_word_2_index;
  TCsdUnsignedInt8 cken_flag_1;
  TCsdUnsignedInt8 cken_flag_2;
	TCsdUnsignedInt8 crc32 [4];

};

#endif /* NOCS3X_TEST_HARNESS_H */

/* End NOCS3x_Test_Harness.h */                       
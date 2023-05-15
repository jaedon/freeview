/****************************************************************************/
/*                          Conexant Systems Inc.                           */
/****************************************************************************/
/*                                                                          */
/* Filename:       CFGTOH.C                                                 */
/*                                                                          */
/* Description:    Build Config File to Header Converter                    */
/*                                                                          */
/* Author:         Dave Wilson                                              */
/*                                                                          */
/* Copyright Conexant Systems Inc, 2001 - 2004                              */
/* All Rights Reserved.                                                     */
/*                                                                          */
/****************************************************************************/
const char vcsid[] = "$Id: cfgtoh.c 4433 2007-12-03 19:12:37Z scott.carter $";
/****************************************************************************/


/******************************************************************************/
/* This tool is used to convert a build system configuration file comprising  */
/* a number of makefile macro definitions (KEY=VALUE) into equivalent header  */
/* files that can be included in C or assembler files during compilation or   */
/* assembly.                                                                  */
/*                                                                            */
/* The current version supports three output file formats:                    */
/*                                                                            */
/* 1. Standard C where the output is of the form "#define KEY VALUE" for all  */
/*    lines in the input file.                                                */
/* 2. ARM SDT assembler format where output is of the form KEY EQU VALUE      */
/* 3. GNU assembler format where the output is of the form #define KEY VALUE  */
/*                                                                            */
/* Several special cases are considered:                                      */
/*                                                                            */
/* 1. Comment lines containing the string "CONVERT END" in the config file    */
/*    tell the tool to stop parsing and close the output files.               */
/*                                                                            */
/* 2. The config line CHIP_NAME=<chip> is not echoed to the output but is     */
/*    used to generate a #include or get line at the end of the header        */
/*    for the relevant chip header. For example, CHIP_NAME=COLORADO in the    */
/*    config file will result in "#include "COLORADO.H"# being added to the   */
/*    end of the generated C and GNU assembler headers and "GET COLORADO.A"   */
/*    being added to the SDT assembler header.                                */
/*                                                                            */
/*    NOTE:  A command line option has been added to allow SWCONFIG files to  */
/*    NOT define CHIP_NAME, and to skip the generation of the #include line at*/
/*    the end of the translated file.                                         */
/*                                                                            */
/* 3. Several syntax options in the CFG file to allow automatic generation of */
/*    headers containing possible value labels and default values for given   */
/*    keys. This syntax is described below.                                   */
/*                                                                            */
/* 4. The -m option may be used to generate an HTML document from the input   */
/*    file. This document will list all keys, default values and legal values */
/*    along with description text entered for the key.                        */
/*                                                                            */
/******************************************************************************/


/******************************************************************************/
/* CFG File Syntax                                                            */
/*                                                                            */
/* Files passed to this tool generally contain strings of the form            */
/*                                                                            */
/*   KEY=VALUE                                                                */
/*                                                                            */
/* resulting in lines containing a definition of KEY with value VALUE in the  */
/* correct format being written to the output file. Some lines, however,      */
/* result in special actions being taken:                                     */
/*                                                                            */
/* If the -s comment line option is not specified:                            */
/*                                                                            */
/* CHIP_NAME=<string>                                                         */
/*                                                                            */
/* results in lines being written to the output file causing a specific       */
/* chip header to be included. The header may be included directly, for       */
/* example                                                                    */
/*                                                                            */
/*   #include "COLORADO.H"                                                    */
/*                                                                            */
/* where CHIP_NAME=COLORADO was specified or via another file if the          */
/* -n<filename> command line option is specified.                             */
/*                                                                            */
/* If -d is not specified, the following special syntax is recognised         */
/* (otherwise these are treated as normal KEY=VALUE lines)                    */
/*                                                                            */
/* CFG_KEY=KEY_NAME                                                           */
/*                                                                            */
/* results in the internal saved key name being updated with the new string.  */
/* This key name is used in output strings requiring it until the next        */
/* CFG_KEY value is parsed from the file.                                     */
/*                                                                            */
/* CFG_DEFAULT=VALUE                                                          */
/*                                                                            */
/* results in a protected definition of KEY_NAME=VALUE being added to         */
/* the output such that if KEY_NAME is not defined, the value VALUE           */
/* will be assigned to it. The key used here is the last one detected         */
/* in a CFG_KEY statement. A default value for each key is mandatory and      */
/* CFGTOH will generate an error if any key does not have an associated       */
/* default value.                                                             */
/*                                                                            */
/* [                                                                          */
/*                                                                            */
/* results in a comment being generated indicating that the next              */
/* block of definitions are possible values for the last KEY_NAME             */
/* read.                                                                      */
/*                                                                            */
/* ]                                                                          */
/*                                                                            */
/* results in a comment being generated indicating that the block             */
/* of definitions is complete                                                 */
/*                                                                            */
/* ?                                                                          */
/*                                                                            */
/* indicates a line of text that is to be added to the description field in   */
/* generated HTML documentation output. Multiple such lines are allowed and   */
/* at least one per key is mandatory. Lines immediately following CFG_KEY     */
/* are considered description of the key itself while lines withing the []    */
/* section describe the valid values for that key.                            */
/*                                                                            */
/* #                                                                          */
/*                                                                            */
/* is a comment. By default, comments are echoed to the output                */
/* file unless they contain the substring "CONVERT END" which                 */
/* tells the tool to stop parsing the file. Echoing of comments               */
/* can be disabled using the -x command line switch.                          */
/*                                                                            */
/* As an example, the following CFG file section:                             */
/*                                                                            */
/*   # Hardware Configuration                                                 */
/*                                                                            */
/*   SIMPLE_KEY=SIMPLE_VALUE                                                  */
/*                                                                            */
/*   CFG_KEY=HARDWARE_KEY_1                                                   */
/*   CFG_DESC=This key allows selection of hardware feature number 1.         */
/*   CFG_DESC=There are three possible values for the key.                    */
/*   [                                                                        */
/*     CFG_DEFAULT=HW_KEY_VALUE_3                                             */
/*     HW_KEY_VALUE_1=1                                                       */
/*     HW_KEY_VALUE_2=2                                                       */
/*     HW_KEY_VALUE_3=3                                                       */
/*   ]                                                                        */
/*                                                                            */
/*   CFG_KEY=HARDWARE_KEY_2                                                   */
/*   CFG_DESC=This key allows selection of hardware feature number 2          */
/*   [                                                                        */
/*     CFG_DEFAULT=128                                                        */
/*     # HARDWARE_KEY_2 can take any integer in the range 1-255               */
/*   ]                                                                        */
/*                                                                            */
/* without -d would be translated into an ARM ASM header as:                  */
/*                                                                            */
/*   ; Hardware Configuration                                                 */
/*                                                                            */
/*   SIMPLE_KEY EQU SIMPLE_VALUE                                              */
/*                                                                            */
/*   ; Possible values for key HARDWARE_KEY_1                                 */
/*   HW_KEY_VALUE_1 EQU 1                                                     */
/*   HW_KEY_VALUE_2 EQU 2                                                     */
/*   HW_KEY_VALUE_3 EQU 3                                                     */
/*   ; End of possible values for key HARDWARE_KEY_1                          */
/*                                                                            */
/*   ; Possible values for key HARDWARE_KEY_2                                 */
/*   ; HARDWARE_KEY_2 can take any integer in the range 1-255                 */
/*   ; End of possible values for key HARDWARE_KEY_2                          */
/*                                                                            */
/* with -d -x would result in:                                                */
/*                                                                            */
/*     IF :LNOT:(:DEF:HARDWARE_KEY_1)                                         */
/*   HARDWARE_KEY_1 EQU HW_KEY_VALUE_3                                        */
/*     ENDIF                                                                  */
/*                                                                            */
/*     IF :LNOT:(:DEF:HARDWARE_KEY_2)                                         */
/*   HARDWARE_KEY_2 EQU 128                                                   */
/*     ENDIF                                                                  */
/*                                                                            */
/******************************************************************************/

  
/*****************/
/* Include Files */
/*****************/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#include <fcntl.h>
#include <math.h>
#include <time.h>

/**************************/
/* Local Type Definitions */
/**************************/
#define NUM_OUTPUT_TYPES 6

#define OUTPUT_C       0
#define OUTPUT_SDT_ASM 1
#define OUTPUT_VX_ASM  2
#define OUTPUT_HTML    3
#define OUTPUT_NMAKE   4
#define OUTPUT_MAKE	   5

#define MAX_ERROR_STRING_LEN 256
#define MAX_LINE_LENGTH      512
#define MAX_EXTENSION_LEN      4

#define END_MARKER_STRING    "CONVERT END"
#define CHIP_NAME_STRING     "CHIP_NAME"
#define DEFAULT_VALUE_STRING "CFG_DEFAULT"
#define KEY_NAME_STRING      "CFG_KEY"

#define VALUE_START_DELIM    '['
#define VALUE_END_DELIM      ']'
#define COMMENT_DELIM        '#'
#define DESCRIPTION_DELIM    '?'
#define CFG_KEY_SEPERATOR    ',' /* used for cases like CFG_KEY = CFG_KEY,CFG_KEY,CFG_KEY */

typedef int BOOL;

#ifndef _MAX_PATH
  #define _MAX_PATH 1024
#endif

#ifndef FALSE
   #define FALSE 0
#endif

#ifndef TRUE
   #define TRUE 1
#endif

typedef enum _OUTPUT_TYPE_T
{
  OUTPUT_WITH_CHIP_TYPE,
  OUTPUT_WITHOUT_CHIP_TYPE,
  OUTPUT_DEFAULT_VALUES,
  OUTPUT_DOCUMENTATION,
} OUTPUT_TYPE;

typedef struct _COMMAND_PARAMS_T
{
  char        strInput[_MAX_PATH+1];
  char        strOutput[NUM_OUTPUT_TYPES][_MAX_PATH+1];
  OUTPUT_TYPE eType;
  BOOL        bOutputEnabled[NUM_OUTPUT_TYPES];
  BOOL        bQuiet;
  BOOL        bOverwrite;
  BOOL        bSeparateInclude;
  BOOL        bEchoComments;
  BOOL        bDefaultFound;
  BOOL        bDescFound;
  BOOL        bInsideStruct;
  BOOL        bIgnoreWhitespace;
  BOOL        bValuesFound;
  BOOL        bLastLineContinues;
  char        strChipName[MAX_LINE_LENGTH+1];
  char        strExtensions[NUM_OUTPUT_TYPES][MAX_EXTENSION_LEN+1];
  char        strIncludeFile[_MAX_PATH+1];
  char        strLastKey[MAX_LINE_LENGTH+1];
  char        strDefaultValue[MAX_LINE_LENGTH+1];
  char        strProtection[MAX_LINE_LENGTH+1];
} COMMAND_PARAMS;

typedef enum _WRITE_TYPE_T
{
  WRITE_IGNORE,
  WRITE_KEY_VALUE,
  WRITE_COMMENT,
  WRITE_DESCRIPTION,
  WRITE_KEY,
  WRITE_VALUE_START,
  WRITE_VALUE_STOP,
  WRITE_DEFAULT,
  WRITE_BLANK_SPACE,
  WRITE_STOP
} WRITE_TYPE;

/********************/
/* Global Variables */
/********************/
char *strCommentStart[NUM_OUTPUT_TYPES] = {"/* ", "; ", "/* ", "<!--", "#", "#"};
char *strCommentEnd[NUM_OUTPUT_TYPES]   = {" */", "",   " */", "-->", "", ""};

char *strKeyValueFormat[NUM_OUTPUT_TYPES] =
{
  "#define %s %s\n",
  "%s EQU %s\n",
  "#define %s %s\n",
  "%s (%s)<br>\n",
  "%s = %s\n",
  "%s := %s\n"
};

char *strKeyValueContinuationFormat[NUM_OUTPUT_TYPES] =
{
  "        %s\n",
  "%s\n",
  "        %s\n",
  "(%s)<br>\n",
  "%s\n",
  "    %s\n"
};

char *strDefaultValueFormat[NUM_OUTPUT_TYPES] =
{
  "\n#ifndef %s\n  #define %s %s\n#endif\n",
  "\n\tIF :LNOT:(:DEF:%s)\n%s EQU %s\n\tENDIF\n",
  "\n#ifndef %s\n  #define %s %s\n#endif\n",
  "",
  "!if \"$(%s)\" == \"\"\n%s = %s\n!endif\n",
  "ifeq ($(%s),)\n%s := %s\nendif\n"
};

char *strDefaultValueContinuationFormat[NUM_OUTPUT_TYPES] =
{
  "\n#ifndef %s\n  #define %s %s\n",
  "\n\tIF :LNOT:(:DEF:%s)\n%s EQU %s\n",
  "\n#ifndef %s\n  #define %s %s\n",
  "",
  "!if \"$(%s)\" == \"\"\n%s = %s\n",
  "ifeq ($(%s),)\n%s := %s\n"
};

char *strDefaultValueContinuationEndFormat[NUM_OUTPUT_TYPES] =
{
  "#endif\n",
  "\tENDIF\n",
  "#endif\n",
  "",
  "!endif\n",
  "endif\n"
};

char *strStructStart[NUM_OUTPUT_TYPES] =
{
    "",
    "",
    "",
    "<tr>\n<td align=left valign=top>%s</td>\n<td align=left valign=top>\n",
    "",
	 ""
};

char *strBlockStart[NUM_OUTPUT_TYPES] =
{
  "/*******************************************************/\n/* Possible values for key(s) %s */\n",
  "; *******************************************************\n; Possible values for key(s) %s\n",
  "/*******************************************************/\n/* Possible values for key(s) %s */\n",
  "</td><td align=left valign=top>\n",
  "",
  ""
};

char *strBlockEnd[NUM_OUTPUT_TYPES] =
{
  "/* End of possible values for key %s */\n/*******************************************************/\n",
  "; End of possible values for key %s\n; *******************************************************\n",
  "/* End of possible values for key %s */\n/*******************************************************/\n",
  "</td><td align=left valign=top>%s</td></tr>\n",
  "",
  ""
};
 
char *strIncludeFormat[NUM_OUTPUT_TYPES] =
{
  "\n#include \"%s.%s\"\n\n",
  "\n\tGET %s.%s\n\n",
  "\n#include \"%s.%s\"\n\n",
  "",
  "",
  ""
};  

char *strSubContinuationLine[NUM_OUTPUT_TYPES] =
{
   "",
   "",
   "",
   "<br>",
   "",
   ""
};

  
char *strDefaultExtensions[NUM_OUTPUT_TYPES] = { "H", "A", "VXA", "HTML", "MAK", "MAK" };

char *strPreamble[] =
{
  "*************************************************************",
  "*        Board hardware configuration header file           *",
  "*************************************************************",
  "* This file is automatically generated by the build process *",
  "* from the supplied configuration file. Editing it is,      *",
  "* therefore, completely futile!                             *",
  "*************************************************************",
  NULL
};

char *strSwConfigPreamble[] =
{
  "*************************************************************",
  "*        Board software configuration header file           *",
  "*************************************************************",
  "* This file is automatically generated by the build process *",
  "* from the supplied configuration file. Editing it is,      *",
  "* therefore, completely futile!                             *",
  "*************************************************************",
  NULL
};

char *strInclPreamble[] =
{
  "*************************************************************",
  "*                  Chip include header file                 *",
  "*************************************************************",
  "* This file is automatically generated by the build process *",
  "* from the supplied configuration file. Editing it is,      *",
  "* therefore, completely futile!                             *",
  "*************************************************************",
  NULL
};

char *strHTMLPreamble[] =
{
  "<html>\n",
  "<head>\n",
  "<title>Conexant Configuration File Keys and Values</title>\n",
  "<style>\n",
  "<!--\n",
  "body         { font-family: Verdana, Arial, Helvetica; font-size: 10pt }\n",
  "table        { font-size: 10pt; font-family: Verdana, Arial, Helvetica }\n"
  "-->\n",
  "</style>\n",
  "</head>\n",
  "<body bgcolor=""#FFFFFF"" text=""#000000"">\n",
  "<h1 align=""center"">Conexant Build Configuration File Keys and Values</h1>\n",
  "<p>This file (which was automatically generated by the Conexant build system)\n",
  "contains documentation of supported configuration file keys, valid values they\n",
  "may be set to and the default value that will be assigned if the specific config\n",
  "file used in a build does not explicitly set the key.</p>\n",
  "  <center>\n",
  "  <table border=""1"">\n",
  "    <tr>\n",
  "      <td align=""left"" bgcolor=""#000080""><b><font color=""#FFFFFF"">Key Name</font></b></td>\n",
  "      <td align=""left"" bgcolor=""#000080""><b><font color=""#FFFFFF"">Description</font></b></td>\n",
  "      <td align=""left"" bgcolor=""#000080""><b><font color=""#FFFFFF"">Possible\n",
  "        Values</font></b></td>\n",
  "      <td align=""left"" bgcolor=""#000080""><b><font color=""#FFFFFF"">Default\n",
  "        Value</font></b></td>\n",
  "    </tr>\n",
  NULL
};
  
#define NUM_PROTECTION_STRINGS 2
#define CONFIG_STRING   0
#define SWCONFIG_STRING 1

char *strDefaultProtectionInserts[NUM_PROTECTION_STRINGS] =
{
  "CONFIG",
  "SWCONFIG"
};

#define NUM_PROT_PREAMBLE  3
#define NUM_PROT_POSTAMBLE 2

char *strProtectionPreamble[NUM_OUTPUT_TYPES][NUM_PROT_PREAMBLE] =
{
  {"\n#ifndef __BUILD_%s__\n", "#define __BUILD_%s__\n", "\n", },
  {"\n", "", ""},
  {"\n#ifndef __BUILD_%s__\n", "#define __BUILD_%s__\n", "\n", },
  {"", "", ""},
  {"", "", ""},
  {"", "", ""}
};

char *strProtectionPostamble[NUM_OUTPUT_TYPES][NUM_PROT_POSTAMBLE] =
{
  {"#endif /* __BUILD_%s__ */\n", "\n" },
  {"\n\tEND\n", "" },
  {"#endif /* __BUILD_%s__ */\n", "\n" },
  {"", ""},
  {"", ""},
  {"", ""}
};

char *strInclProtectionPreamble[NUM_OUTPUT_TYPES][NUM_PROT_PREAMBLE] =
{
  {"\n#ifndef __BUILD_CHIP_%s_INCL__\n", "#define __BUILD_CHIP_%s_INCL__\n", "\n" },
  {"\n", "", "" },
  {"\n#ifndef __BUILD_CHIP_%s_INCL__\n", "#define __BUILD_CHIP_%s_INCL__\n", "\n" },
  {"", "", ""},
  {"", "", ""},
  {"", "", ""}
};

char *strInclProtectionPostamble[NUM_OUTPUT_TYPES][NUM_PROT_POSTAMBLE] =
{
  {"#endif /* __BUILD_CHIP_%s_INCL__ */\n", "\n" },
  {"\n\tEND\n", "" },
  {"#endif /* __BUILD_CHIP_%s_INCL__ */\n", "\n" },
  {"", ""},
  {"", ""},
  {"", ""}
};

char *strPostamble[] =
{
  "************************************************************",
  "* End of automatically generated configuration header file *",  
  "************************************************************",
  NULL
};

char *strHTMLPostamble[] =
{
  "</body>\n",
  "</html>\n",
  NULL
};
  
char *strWhiteSpaceChars = " \n\r\t";
char *strIllegalRChars   = "\r\t#@$[];'\"{}";
char *strIllegalLChars   = " \n\r\t,.#@$[];'\"\\{}*~!^*&|?/()+-";

/********************************/
/* Internal Function Prototypes */
/********************************/
void ShowUsage(void);
void ShowSplash(void);
BOOL ParseCommandLine(int argc, char *argv[], COMMAND_PARAMS *pParams);
BOOL OpenAllOutputFiles(COMMAND_PARAMS *lpParams, FILE **pHandles);
BOOL CloseAllOutputFiles(COMMAND_PARAMS *lpParams, FILE **pHandles);
BOOL SplitLineIntoKeyValue(char *strLineBuffer, char *strKey, char *strValue);
BOOL WriteKeyValueToOutputFiles(COMMAND_PARAMS *lpParams, FILE **pHandles, char *strKey, char *strValue, WRITE_TYPE *pWriteInfo, BOOL bCont);
BOOL ParseConfigLine(COMMAND_PARAMS *lpParams, char *strLineBuffer, char *strKey, char *strValue, WRITE_TYPE *pWriteInfo, BOOL *pContinuation);
char *RemoveLeadingWhitespace(char *strLineBuffer);
void RemoveTrailingWhitespace(char *strLineBuffer);
BOOL WriteSpecialOutputFileLines(COMMAND_PARAMS *lpParams, FILE **ppHandles);
BOOL StringContainsIllegalCharacters(char *szString, char *szIllegal);
int toint(int c);
void strtolower(char *src, char *dest);
int strSubContLineChar(OUTPUT_TYPE eType, char *string);

/********************************/
/* Main Application Entry Point */
/********************************/
int main(int argc, char *argv[])
{
  FILE   *fhInput;
  FILE   *fhOutput[2*NUM_OUTPUT_TYPES];
  COMMAND_PARAMS sCommandParams;
  WRITE_TYPE sWrite;
  BOOL    bRetcode;
  BOOL    bError = FALSE;
  BOOL    bStop = FALSE;
  BOOL    bCont = FALSE;
  char    strKey[MAX_LINE_LENGTH+1];
  char    strValue[MAX_LINE_LENGTH+1];
  char	  strLineBuffer[MAX_LINE_LENGTH+1];
  char    *strRetcode = strLineBuffer;

  /******************************/
  /* Get command line arguments */
  /******************************/
  if (!ParseCommandLine(argc, argv, &sCommandParams))
  {
    return(1);
  }  
  
  if(!sCommandParams.bQuiet)
    ShowSplash();

  /************************/
  /* Open the source file */
  /************************/
  fhInput   = fopen(sCommandParams.strInput, "rb");
  if(fhInput == (FILE *)NULL)
  {
	  printf("\nCFGTOH: ERROR - file %s can't be opened. Does it exist?\n", sCommandParams.strInput);
	  return(3);
  }
 
  /*************************************/
  /* Open required output output files */
  /*************************************/
  bRetcode = OpenAllOutputFiles(&sCommandParams, fhOutput);
  if (bRetcode)
  {
	  while(strRetcode && !bStop)
	  {
      /* Read a line from the input file */
	    strRetcode = fgets(strLineBuffer, MAX_LINE_LENGTH, fhInput);

	    if(strRetcode)
	    {
        /* Check for a comment. Ignore it unless the string is a key value pair that must */
        /* be written to the output file.                                                 */
        bRetcode = ParseConfigLine(&sCommandParams, strLineBuffer, strKey, strValue, &sWrite, &bCont);
      
        if (bRetcode)
 	      {
          if(sWrite != WRITE_STOP)
          {
            /* If we have been told to write something to the output, do it */
            bRetcode = WriteKeyValueToOutputFiles(&sCommandParams, fhOutput, strKey, strValue, &sWrite, bCont);
      
            if (!bRetcode)
            {
              printf("CFGTOH: ERROR - failed to write key/value pair to output file!\n");
              strRetcode = NULL;
              bError = TRUE;
            }  
          }
          else
            bStop = TRUE;
		    }  
        else
	   	  {
	        strRetcode = NULL;
          bError = TRUE;
		    }
	    } 
	  }
    
    /* Write any state-specific lines to the output files if necessary */
    if (!bError)
    {
      bRetcode = WriteSpecialOutputFileLines(&sCommandParams, fhOutput);
      if (!bRetcode)
      {
        printf("CFGTOH: ERROR - unable to write to one or more output files\n");
        bError = TRUE;
      }  
    }
      
    /* Close all the output files, writing necessary footer info to each in the process */
    bRetcode = CloseAllOutputFiles(&sCommandParams, fhOutput);
  }
  else
  {
    printf("CFGTOH: ERROR - error or user abort while opening output files\n");
  }
  
  /************************/
  /* Close the input file */
  /************************/
  fclose(fhInput);
 
  /****************************************************/
  /* Tidy up and leave with the appropriate exit code */
  /****************************************************/
  if(bRetcode && !bError)
    return(0);
  else
    return(2);  
}


/********************************************************************/
/*  FUNCTION:    SplitLineIntoKeyValue                              */
/*                                                                  */
/*  PARAMETERS:  strLineBuffer - Buffer containing string to split  */
/*               strKey        - Storage for returned key           */
/*               strValue      - Storage for returned value         */
/*                                                                  */
/*  DESCRIPTION: Split a string of the form "key=value" into two    */
/*               parts removing leading and trailing whitespace     */
/*               from each.                                         */
/*                                                                  */
/*  RETURNS:     TRUE if the split is completed successfully,       */
/*               FALSE otherwise.                                   */
/*                                                                  */
/********************************************************************/
BOOL SplitLineIntoKeyValue(char *strLineBuffer, char *strKey, char *strValue)
{
  char *pEquals1, *pEquals2;
  char szBuffer[MAX_LINE_LENGTH+1];
  char *szString;
  int  iCount;

  /* Make a copy of the string and remove leading and trailing whitespace */
  strcpy(szBuffer, strLineBuffer);
  szString = RemoveLeadingWhitespace(szBuffer);
  if(szString)
    RemoveTrailingWhitespace(szString);
  
  /* Make sure it wasn't a collection of whitespace */
  if (szString == NULL)
  {
    printf("CFGTOH: ERROR - blank input line passed to split function\n");
    return(FALSE);  
  }

  /* Ensure line has only one equals sign in it */  
  pEquals1 = strchr(szString, '=');
  if (pEquals1)
  {
    /* Look for a second equals sign */
    pEquals2 = strchr(pEquals1+1, '=');
    if (pEquals2)
    {
      /* Oops - we found another! */
      printf("CFGTOH: ERROR - incorrectly formatted input line \"%s\"\n", strLineBuffer);
      return(FALSE);
    }  
  }
  else
  {
    /* There is no '=' in the supplied string! */
    printf("CFGTOH: ERROR - incorrectly formatted input line \"%s\" is missing '='\n", strLineBuffer);
    return(FALSE);
  }
  
  /* If we get here, we found 1 '=' in the string at pEquals1 */
  iCount = pEquals1-szString;
  if (iCount)
  {
    strncpy(strKey, szString, iCount);
    strKey[iCount] = (char)0;
  }
  else
  {
    printf("CFGTOH: ERROR - string \"%s\" starts with '=' so can't be split\n", szString);
    return(FALSE);
  }
  
  RemoveTrailingWhitespace(strKey);
  
  pEquals2=RemoveLeadingWhitespace(pEquals1+1);
  if (pEquals2)
    strcpy(strValue, pEquals2);
  else
  {
    printf("CFGTOH: ERROR - string \"%s\" ends with '=' so can't be split\n", szString);
    return(FALSE);
  }
   
  /* Check split strings for illegal characters */  
  if(StringContainsIllegalCharacters(strKey, strIllegalLChars))
  {
    printf("CFGTOH: ERROR - key string \"%s\" contains illegal characters\n", strKey);
    return(FALSE);
  }  
   
  if(StringContainsIllegalCharacters(strValue, strIllegalRChars))
  {
    printf("CFGTOH: ERROR - value string \"%s\" contains illegal characters\n", strValue);
    printf("CFGTOH: ERROR - %s \n", strIllegalRChars);
    
    return(FALSE);
  }  
   
  /* If we get this far, everything was fine and string is split */ 
  return(TRUE);  
}


/********************************************************************/
/*  FUNCTION:    WriteKeyValueToOutputFiles                         */
/*                                                                  */
/*  PARAMETERS:  lpParams - command line parameters structure       */
/*               pHandles - pointer to an array of at least         */
/*                          NUM_OUTPUT_TYPES FILE *s containing     */
/*                          handles to open output files.           */
/*               strKey   - Key string to write to output           */
/*               strValue - Value string to write to output         */
/*                                                                  */
/*  DESCRIPTION: Write a key and value to each open output file     */
/*               in the format required for that file. Change the   */
/*               output format if required depending upon the       */
/*               bOutputASCII state.                                */
/*                                                                  */
/*  RETURNS:     TRUE if all files are successfully written,        */
/*               FALSE otherwise.                                   */
/*                                                                  */
/********************************************************************/
BOOL WriteKeyValueToOutputFiles(COMMAND_PARAMS *lpParams, FILE **ppHandles, char *strKey, char *strValue, WRITE_TYPE *pWrite, BOOL bCont)
{
  int iLoop;
  int iRetcode;
  int decimals = 0;
  static BOOL bDefaultValueContinues = FALSE;
  BOOL bRetcode = TRUE;
  BOOL bError   = FALSE;
  double val = 0.0;
  int val2 = 0;
  char *sP,*bufptr;
  char szDefBuffer[MAX_LINE_LENGTH+1]={0};

  /* If we are told to ignore this string, just return immediately */
  if(*pWrite == WRITE_IGNORE)
    return(TRUE);

  /* For each enabled output format... */
  for (iLoop = 0; (iLoop < NUM_OUTPUT_TYPES) && !bError; iLoop++)
  {
    if (lpParams->bOutputEnabled[iLoop])
    {
      switch(*pWrite)
      {
        /*****************************************************/
        /* Normal case - just write the key/value definition */
        /*****************************************************/
        case WRITE_KEY_VALUE:
      
          /* Special case for arm assembler files, where you can't have
           * floats, so you save the value as two variables, 
           * <name>_INT=<val1> and  <name>_FRAC=<val2>
           */
          sP = strchr(strValue, '.');
          if(sP && (iLoop == OUTPUT_SDT_ASM))
          {
             if(bCont)
             {
                printf("CFGTOH: ERROR - use of line continuation characters within a floating point value is not supported\n");
                bRetcode = FALSE;
                bError = TRUE;
                break;
             }

             /*Peel off the fractional value first */
             sP++;
             while(*sP != '\0')
             {
                decimals ++;
                if(!isdigit(*sP))
                {
                   printf("CFGTOH: ERROR, expecting a decimal digit, found something else.  Key = %s, Value = %s\n",strKey,strValue);
                   iRetcode = FALSE;
                }
                val += (float)toint((int)*sP)/(pow(10,decimals));
                sP++;
             }

             /*Add comment and print the Fractional value */ 
	          iRetcode &= fprintf(ppHandles[iLoop], 
                             ";The following is the hex representation of (the fractional portion of %f ) * 2^32\n",
                             val); 
	          iRetcode &= fprintf(ppHandles[iLoop], 
                             "%s_FRAC EQU 0x%x\n",
                             strKey,
                             (unsigned int)(val*pow(2,32)));
  
             /* Peel off the integer part of the string */
             sP = &strValue[0];
             while((*sP != '\0') && (*sP != '.') ){
                if(!isdigit(*sP))
                {
                   printf("CFGTOH: ERROR, expecting a decimal digit, found something else.  Key = %s, Value = %s\n",strKey,strValue);
                   iRetcode = FALSE;
                }
                if(val2 != 0)
                  val2 = val2 * 10;
                val2 += toint((int)*sP);
                sP++;
             }
  
             /*Add comment and print the Integer value */ 
             iRetcode = fprintf(ppHandles[iLoop],
                             ";The following is the hex representation of the integer %d\n",
                             val2); 
             iRetcode = fprintf(ppHandles[iLoop],
                             "%s_INT EQU 0x%x\n",
                             strKey,
                             val2);
          }
          else 
          { 
            /* Normal case - no floating point output */
            
            /* Write the key value pair in the relevant format */
            if(!bCont)
            {
#if 1
            if(strValue[0] == '%')
            	{
				iRetcode = fprintf(ppHandles[iLoop], 
								   "#define %s \"%s\"\n",
								   strKey,
								   &strValue[1]);
            	}
			else{
#endif
			  iRetcode = fprintf(ppHandles[iLoop], 
                                 strKeyValueFormat[iLoop], 
                                 strKey,
                                 strValue);
				}
            }
            else
            {
               /* We are continuing a previous line so just dump the value string */
               iRetcode = fprintf(ppHandles[iLoop],
                                  strKeyValueContinuationFormat[iLoop],
                                  strValue);
			      /*Is this the last value of a continued default value*/
			      if(!lpParams->bLastLineContinues && bDefaultValueContinues)
			      {
				      /*Print the termination string*/
				      iRetcode = fprintf(ppHandles[iLoop],
									     strDefaultValueContinuationEndFormat[iLoop],
									     "");
			      }
            }
          } 
      
          /* Check that we didn't get an error */
          if (iRetcode < 0)
          {
            printf("CFGTOH: ERROR - unable to write key/value to output file!\n");
            bRetcode = FALSE;
            bError = TRUE;
          }  
          break;
          
          /*************************************/
          /* Echo a comment to the output file */
          /*************************************/
          case WRITE_COMMENT:
            if(lpParams->bEchoComments)
            {
              iRetcode = fprintf(ppHandles[iLoop],
                                 "%s %s %s\n",
                                 strCommentStart[iLoop],
                                 strKey,
                                 strCommentEnd[iLoop]);
              if (iRetcode < 0)
              {
                printf("CFGTOH: ERROR - unable to write comment to output file!\n");
                bRetcode = FALSE;
                bError = TRUE;
              }  
            }  
            break;

          /**************************************************/
          /* Write a line of description to the output file */
          /**************************************************/
          case WRITE_DESCRIPTION:
            iRetcode = fprintf(ppHandles[iLoop], "%s\n", strKey);
            if (iRetcode < 0)
            {
              printf("CFGTOH: ERROR - unable to write comment to output file!\n");
              bRetcode = FALSE;
              bError = TRUE;
            }  
            break;
                        
          /**********************************************************/
          /* Write the HTML block indicating the start of a new key */
          /**********************************************************/
          case WRITE_KEY:
            iRetcode = fprintf(ppHandles[iLoop], strStructStart[iLoop], strValue);
            if (iRetcode < 0)
            {
              printf("CFGTOH: ERROR - unable to write key to output file!\n");
              bRetcode = FALSE;                     
              bError = TRUE;
            }  
            break;
                        
          /**********************************************************************/              
          /* Write a comment indicating the beginning of a block of definitions */
          /**********************************************************************/              
          case WRITE_VALUE_START:
            if(lpParams->bEchoComments || (lpParams->eType == OUTPUT_DOCUMENTATION))
            {
              iRetcode = fprintf(ppHandles[iLoop],
                                 strBlockStart[iLoop],
                                 lpParams->strLastKey);
              if (iRetcode < 0)
              {
                printf("CFGTOH: ERROR - unable to write block start comment to output file!\n");
                bRetcode = FALSE;
                bError = TRUE;
              }  
            }
            break;
            
          /****************************************************************/              
          /* Write a comment indicating the end of a block of definitions */
          /****************************************************************/              
          case WRITE_VALUE_STOP:
            if(lpParams->bEchoComments || (lpParams->eType == OUTPUT_DOCUMENTATION))
            {
               /*Some output types need to have the continuation line character
                 substituted for another, ie. html file substitutes \\ for <br>*/
               strSubContLineChar(iLoop, lpParams->strDefaultValue);

              if(lpParams->eType == OUTPUT_DOCUMENTATION)
              {
                iRetcode = fprintf(ppHandles[iLoop],
                                   strBlockEnd[iLoop],
                                   lpParams->strDefaultValue);
              }
              else
              {
                iRetcode = fprintf(ppHandles[iLoop],
                                   strBlockEnd[iLoop],
                                   lpParams->strLastKey);
              } 
              if (iRetcode < 0)
              {
                printf("CFGTOH: ERROR - unable to write block end comment to output file!\n");
                bRetcode = FALSE;
                bError = TRUE;
              }  
            }  
            break;
            
          /****************************************************/  
          /* Create a default definition stanza in the output */
          /****************************************************/  
          case WRITE_DEFAULT:
           bufptr = sP = lpParams->strLastKey;   /* Set Sp,bufptr to the key string buffer */
           
           while (sP != NULL)
           {
            sP = strchr(bufptr,CFG_KEY_SEPERATOR);

             if (sP)
             { /* ok CFG_KEY_SEPERATOR has been found we need to handle special case
                * of CFG_KEY = CFG_KEY,CFG_KEY,CFG_KEY,........,CFG_KEY
                * Basically we will seperate these definitions out
                * in cases where definitions would require the same options.
                * by detecting the token ','
                */
              strncpy(szDefBuffer,bufptr,(sP-bufptr));
              szDefBuffer[(sP-bufptr)]= '\0'; /* Replace CFG_KEY_SEPERATOR to NULL terminated */
              bufptr = (sP+1);                /* Set bufptr to next char */
             }
			 if(!bCont)
			 {
				 iRetcode = fprintf(ppHandles[iLoop],
								   strDefaultValueFormat[iLoop],
								   (sP==NULL? bufptr :szDefBuffer) ,
								   (sP==NULL? bufptr :szDefBuffer) ,
								   strValue);
			 }
			 else
			 {
				 iRetcode = fprintf(ppHandles[iLoop],
								   strDefaultValueContinuationFormat[iLoop],
								   (sP==NULL? bufptr :szDefBuffer) ,
								   (sP==NULL? bufptr :szDefBuffer) ,
								   strValue);
				 bDefaultValueContinues = TRUE;
			 }
            if (iRetcode < 0)
            {
              printf("CFGTOH: ERROR - unable to write default value to output file!\n");
              bRetcode = FALSE;
              bError = TRUE;
            }
            
             if ( *bufptr == '\0') /* if we have reached the end of this buffer, we have finished */
             {
              sP = NULL;
             }
           } /*  while (sP != NULL) */    

            break;
            
		  /********************************************/
		  /* Write a line of whitespace to the output */
		  /********************************************/
		  case WRITE_BLANK_SPACE:
              if(!lpParams->bIgnoreWhitespace)
			    iRetcode = fprintf(ppHandles[iLoop], "\n");
			  break;

          /*****************************************************/
          /* Signal to stop parsing - caller should catch this */
          /*****************************************************/
          case WRITE_STOP:
            bRetcode = FALSE;
            bError = TRUE;
            break;
            
      } /* end switch */
    } /* end OutputEnabled */  
  } /* end for */

  return(bRetcode);
}


/********************************************************************/
/*  FUNCTION:    ParseConfigLine                                    */
/*                                                                  */
/*  PARAMETERS:  strLineBuffer  - pointer to string to parse        */
/*               lpParams       - pointer to command line parameter */
/*                                settings and state information.   */
/*               strKey         - pointer to storage for parsed     */
/*                                key string.                       */
/*               strValue         pointer to storage for parsed     */
/*                                value string.                     */
/*               pWriteInfo     - value is written to indicate to   */
/*                                the caller to stop (WRITE_STOP),  */
/*                                ignore the line (WRITE_IGNORE) or */
/*                                how to format the returned data.  */
/*               pbCont         - Set to TRUE if the line being     */
/*                                processed is a continuation of a  */
/*                                previous line (line terminated    */
/*                                with a '\' character) or FALSE if */
/*                                not a continuation.               */
/*                                                                  */
/*  DESCRIPTION: Parse the supplied config file line and let the    */
/*               caller know how to process the strings returned.   */
/*                                                                  */
/*               If the line processed is a continuation of the     */
/*               previous line, *strValue will be written with the  */
/*               string passed (including the continuation char)    */
/*               but strKey and pWriteInfo will not be changed.     */
/*                                                                  */
/*  RETURNS:     TRUE if line was parsed successfully, FALSE if the */
/*               format of the line was not correct (not a comment  */
/*               or a key/value pair separated by an equals sign).  */
/*               State information in lpParams may be updated if    */
/*               line contains a control string. Current control    */
/*               strings are:                                       */
/*                                                                  */
/*               CHIP_NAME=<chip> updates lpParams->strChipName     */
/*               # END CONVERT #   causes pbStop to be set to TRUE  */
/*                                                                  */
/********************************************************************/
BOOL ParseConfigLine(COMMAND_PARAMS *lpParams, char *strLineBuffer, char *strKey, char *strValue, WRITE_TYPE *pWriteInfo, BOOL *pbCont)
{
  char *szStart;
  char *szEnd;
  BOOL bRetcode;
  BOOL bContCharPresent = FALSE;
  
  
  /* First, clip any whitespace from the start of the string */
  szStart = RemoveLeadingWhitespace(strLineBuffer);
  
  /* If we get a NULL pointer back, line is all whitespace so just return */
  if (szStart)
  {
    /* Get rid of any trailing carriage return/line feed */
    szEnd = strpbrk(szStart, "\n\r");
    if(szEnd)
      *szEnd = (char)0;

    /* Does this string end in a line continuation character? */
    if(szStart[strlen(szStart)-1] == '\\')
       bContCharPresent = TRUE;
    else
       bContCharPresent = FALSE;

    /* If we were already continuing a previous line, just copy the string */
    /* to strValue and return.                                             */
    if(lpParams->bLastLineContinues)
    {
       /* Leave the continuation character in place if it is there! */
       strcpy(strValue, szStart);
       *pbCont = TRUE;
       lpParams->bLastLineContinues = bContCharPresent;
	   if(*pWriteInfo == WRITE_DEFAULT)
			*pWriteInfo = WRITE_KEY_VALUE;

      /*Append the value if we are continuing a default value
        Some output files print the default value at the end
        of the block (i.e. html)*/
      if(!strcmp(strKey, DEFAULT_VALUE_STRING))
      {
         strcat(lpParams->strDefaultValue, szStart);
      }
       return(TRUE);
    }

    /* Temporarily remove the line continuation character if present */
    if(bContCharPresent)
      szStart[strlen(szStart)-1] = (char)0;

    /* By default, tell caller not to write to the output and not to stop*/
    *pWriteInfo = WRITE_BLANK_SPACE;
  
    switch(szStart[0])
    {
      case COMMENT_DELIM:
        /* String is a comment. Check to see if it contains our stop code */
        if(strstr(szStart, END_MARKER_STRING))
            *pWriteInfo = WRITE_STOP;
        else
        {
          /* An ordinary comment - tell the caller to echo it unless comment echo is disabled */
          if(lpParams->bEchoComments)
          {
            strcpy(strKey, &szStart[1]);  
          
            /* If the comment contains a # at the end, remove it and anything to its right */
            szEnd = strchr(strKey, (int)COMMENT_DELIM);
            if(szEnd)
              *szEnd = (char)0;
            
            *pWriteInfo = WRITE_COMMENT;  
          }
          else
          {
            *pWriteInfo = WRITE_IGNORE;
          }  
        }  
        break;
        
      /* We found an opening square bracket */  
      case VALUE_START_DELIM:
        *pWriteInfo = WRITE_VALUE_START;
        lpParams->bInsideStruct = TRUE;
        break;
        
      /* We found a closing square bracket */  
      case VALUE_END_DELIM:
        *pWriteInfo = WRITE_VALUE_STOP;
        /* Check that we found default and description values for this key */
        if(!lpParams->bInsideStruct)
        {
          printf("Error: Syntax error - missing ']' after key %s?\n", lpParams->strLastKey);
          return(FALSE);
        }
        if(!lpParams->bDefaultFound)
        {
          printf("Error: No default value specified for key %s\n", lpParams->strLastKey);
          return(FALSE);
        }
        if(!lpParams->bDescFound)
        {
          printf("Error: No description found for key %s\n", lpParams->strLastKey);
          return(FALSE);
        }
        if(!lpParams->bValuesFound)
        {
          printf("Error: No values or value description found for key %s\n", lpParams->strLastKey);
          return(FALSE);
        }
        lpParams->bInsideStruct = FALSE;
        break;
      
      /* We found a description string */
      case DESCRIPTION_DELIM:
          strcpy(strKey, szStart+1);

          if(lpParams->bInsideStruct)
            lpParams->bValuesFound = TRUE;

          if(lpParams->eType == OUTPUT_DOCUMENTATION)
            *pWriteInfo = WRITE_DESCRIPTION;
          else
          {
            if(lpParams->bInsideStruct)
              *pWriteInfo = WRITE_COMMENT;
            else
              *pWriteInfo = WRITE_IGNORE;
          }
          lpParams->bDescFound = TRUE;

          return(TRUE);

      default:
        /* Split the line into key and value, checking for illegal chars */
        bRetcode = SplitLineIntoKeyValue(strLineBuffer, strKey, strValue);
      
        /* If the line had problems, report the failure */
        if(!bRetcode)
		{
          *pWriteInfo = WRITE_IGNORE;
          return(FALSE);
        }
        else
        {
          /* Now check for special cases */
          
          /******************/
          /* CHIP_NAME line */
          /******************/
          if (!strcmp(strKey, CHIP_NAME_STRING))
          {
            /* We found CHIP_NAME so copy the value into our state structure */
            strcpy(lpParams->strChipName, strValue);
            *pWriteInfo = WRITE_IGNORE;
            break;
          }
          
          /****************/
          /* CFG_KEY line */
          /****************/
          if(!strcmp(strKey, KEY_NAME_STRING))
          {
            /* Make sure that the last key we read had values defined for it */
            if((lpParams->strLastKey[0] != (char)0) && !lpParams->bValuesFound)
            {
                printf("ERROR: No values or value descriptions for key %s\n", lpParams->strLastKey);
                return(FALSE);
            }

            /* Keep track of the last key we read */
            strcpy(lpParams->strLastKey, strValue);
            if(lpParams->eType == OUTPUT_DOCUMENTATION)
              *pWriteInfo = WRITE_KEY;
            else
              *pWriteInfo = WRITE_IGNORE;

            lpParams->bValuesFound  = FALSE;
            lpParams->bDefaultFound = FALSE;
            lpParams->bDescFound    = FALSE;
            break;
          }
          
          /***************/
          /* CFG_DEFAULT */
          /***************/
          if(!strcmp(strKey, DEFAULT_VALUE_STRING))
          {
            /*Restore the continuation character */
            if(bContCharPresent)
               strcat(strValue, " \\");
            strcpy(lpParams->strDefaultValue, strValue);
          
            if(lpParams->bDefaultFound)
            {
              printf("Error: Second default value found for key %s\n", lpParams->strLastKey);
              return(FALSE);
            }
            else
              lpParams->bDefaultFound = TRUE;
            
            if(lpParams->eType == OUTPUT_DEFAULT_VALUES)
              *pWriteInfo = WRITE_DEFAULT;
            else  
              *pWriteInfo = WRITE_DEFAULT;
            break;  
          }
          
          /* If we get here, this is a normal, key/value pair so dump it out unless */
		  /* we are generating a defaults file in which case we just ignore it      */
		  if((lpParams->eType == OUTPUT_DEFAULT_VALUES) || ((lpParams->eType == OUTPUT_DOCUMENTATION) && (!lpParams->bInsideStruct)))
            *pWriteInfo = WRITE_IGNORE;
		  else
          {
	         *pWriteInfo = WRITE_KEY_VALUE;
            /* Replace the line continuation character if one was originally present */
            if(bContCharPresent)
               strValue[strlen(strValue)] = '\\';
            *pbCont = FALSE;
          }
          lpParams->bValuesFound = TRUE;
        }  
        break;
    } /* End switch */
  } /* String was empty */
  
  else
  {
    /* String passed was empty so just ignore it */
    *pWriteInfo = WRITE_BLANK_SPACE;
    *pbCont = lpParams->bLastLineContinues;
  }
  
  /* Remember the state of the line continuation character  */
  lpParams->bLastLineContinues = *pbCont = bContCharPresent;

  return(TRUE);
}

/********************************************************************/
/*  FUNCTION:    RemoveLeadingWhitespace                            */
/*                                                                  */
/*  PARAMETERS:  strLineBuffer - pointer to string to work on       */
/*                                                                  */
/*  DESCRIPTION: Return a pointer to the first non-whitespace       */
/*               character in the provided string.                  */
/*                                                                  */
/*  RETURNS:     Pointer to first non-whitespace character or NULL  */
/*               if no non-whitespace character exists.             */
/*                                                                  */
/********************************************************************/
char *RemoveLeadingWhitespace(char *strLineBuffer)
{
  int iCharLoop;
  char *strChrRet;
  
  /* For each character starting from the left of the string */
  for (iCharLoop = 0; iCharLoop < (int)strlen(strLineBuffer); iCharLoop++)
  {
    /* ...check if the character is whitespace */
    strChrRet = strchr(strWhiteSpaceChars, strLineBuffer[iCharLoop]);
    
    /* If it isn't exit the function with a pointer to the non-whitespace character */
    if(strChrRet == (char *)NULL)
      return(&strLineBuffer[iCharLoop]);
  }
  
  return(NULL);    
}

/********************************************************************/
/*  FUNCTION:    RemoveTrailingWhitespace                           */
/*                                                                  */
/*  PARAMETERS:  strLine - String which is to be worked on          */
/*                                                                  */
/*  DESCRIPTION: Remove any and all whitespace characters from the  */
/*               end of the supplied string. Characters considered  */
/*               whitespace are space, tab, newline and carriage    */
/*               return.                                            */
/*                                                                  */
/*  RETURNS:     Nothing                                            */
/*                                                                  */
/********************************************************************/
void RemoveTrailingWhitespace(char *strLineBuffer)
{
  int iCharLoop;
  char *strChrRet;
  
  /* For each character starting from the left of the string */
  for (iCharLoop = strlen(strLineBuffer)-1; iCharLoop >= 0; iCharLoop--)
  {
    /* ...check if the character is whitespace */
    strChrRet = strchr(strWhiteSpaceChars, strLineBuffer[iCharLoop]);
    
    /* If it isn't, exit the function */
    if(strChrRet == (char *)NULL)
      return;
    else
    {
      /* else set the whitespace to a 0 character and move left to next character */
      strLineBuffer[iCharLoop] = (char)0;
    }  
  }
}


/********************************************************************/
/*  FUNCTION:    StringContainsIllegalCharacters                    */
/*                                                                  */
/*  PARAMETERS:  szString  - string to search for illegal characters*/
/*               szIllegal - string of characters considered illegal*/
/*                                                                  */
/*  DESCRIPTION: Search szString for any character that exists in   */
/*               szIllegal and return TRUE if any are found, FALSE  */
/*               otherwise.                                         */
/*                                                                  */
/*  RETURNS:     TRUE if any character in szIllegal is found inside */
/*               szString, FALSE otherwise.                         */
/*                                                                  */
/********************************************************************/
BOOL StringContainsIllegalCharacters(char *szString, char *szIllegal)
{
  char *szReturn;
  
  szReturn = strpbrk(szString, szIllegal);
  if(szReturn)
    return(TRUE);
  else
    return(FALSE);  
}

/********************************************************************/
/*  FUNCTION:    WriteSpecialOutputFileLines                        */
/*                                                                  */
/*  PARAMETERS:  lpParams  - pointer to parameter and state struct  */
/*               ppHandles - pointer to array containing output     */
/*                           file handles.                          */
/*                                                                  */
/*  DESCRIPTION: Prior to closing output files, write any special   */
/*               lines resulting from control strings in the        */
/*               input config file.                                 */
/*                                                                  */
/*  RETURNS:     TRUE on success, FALSE otherwise                   */
/*                                                                  */
/********************************************************************/
BOOL WriteSpecialOutputFileLines(COMMAND_PARAMS *lpParams, FILE **ppHandles)
{
  int    iLoop;
  int    iRetcode;
  BOOL   bRetcode = TRUE;
  time_t sTime;
  char   *strTime;

  /* Software Config Files should not have a CHIP_NAME line ... ignore it */
  if (lpParams->eType == OUTPUT_WITH_CHIP_TYPE)
  {
    /* Include the relevant chip header assuming chip name has been */
    /* defined.                                                     */
    if (lpParams->strChipName[0] != (char)0)
    {
      for (iLoop = 0; iLoop < NUM_OUTPUT_TYPES; iLoop++)
      {
        if (lpParams->bOutputEnabled[iLoop])
        {
          char strLCChipName[MAX_LINE_LENGTH+1];
          char strLCExtensions[MAX_EXTENSION_LEN+1];

          strtolower(lpParams->strChipName, strLCChipName);
          strtolower(lpParams->strExtensions[iLoop], strLCExtensions);

          /* Write the #include or get line to the relevant output file */
          if (lpParams->bSeparateInclude == TRUE)
            iRetcode = fprintf(ppHandles[iLoop+NUM_OUTPUT_TYPES], 
                               strIncludeFormat[iLoop], 
                               strLCChipName, 
                               strLCExtensions);
          else
            iRetcode = fprintf(ppHandles[iLoop], 
                               strIncludeFormat[iLoop], 
                               strLCChipName, 
                               strLCExtensions);
        
          /* Check that we didn't get an error */
          if (iRetcode < 0)
          {
            printf("CFGTOH: ERROR - unable to write to special lines to output file!\n");
            bRetcode = FALSE;
            break;
          }  
        }
      }
    }
    else
    {
      printf("CFGTOH: Warning! CHIP_NAME not found in input file.\n");
      bRetcode = FALSE;
    }
  }

  /* Now write a timestamp to each file */
  time(&sTime);
  strTime = ctime(&sTime);
  /* Get rid of the trailing '\n' */
  strTime[strlen(strTime)-1] = (char)0;

  for(iLoop = 0; iLoop < NUM_OUTPUT_TYPES; iLoop++)
  {
    if(lpParams->bOutputEnabled[iLoop])
    {
        switch(lpParams->eType)
        {
          case OUTPUT_DOCUMENTATION:
            iRetcode = fprintf(ppHandles[iLoop], "</td></tr></table></center><p>This file was generated from ""%s"" on %s\n",
            lpParams->strInput, strTime);
            break;

          default:
            iRetcode = fprintf(ppHandles[iLoop], "\n%s This file was generated from %s on %s %s\n\n",
                strCommentStart[iLoop], lpParams->strInput, strTime, strCommentEnd[iLoop]);
            break;
        }
    }
  }
  return(bRetcode);
  
}  

/********************************************************************/
/*  FUNCTION:    OpenAllOutputFiles                                 */
/*                                                                  */
/*  PARAMETERS:  lpParams - command line parameters structure       */
/*               pHandles - pointer to an array of at least         */
/*                          NUM_OUTPUT_TYPES FILE *s for storage of */
/*                          file handles.                           */
/*                                                                  */
/*  DESCRIPTION: Open output files as required taking into          */
/*               consideration the output formats enabled. If the   */
/*               output file exists already and the user has not    */
/*               specified automatic overwriting, prompt them as    */
/*               to whether file should be rewritten.               */
/*               On opening an output file, write the appropriate   */
/*               preamble information to the file.                  */
/*                                                                  */
/*  RETURNS:     TRUE if all required output files are opened       */
/*               successfully, FALSE otherwise                      */
/*                                                                  */
/********************************************************************/
BOOL OpenAllOutputFiles(COMMAND_PARAMS *lpParams, FILE **ppHandles)
{
  int iLoop, iErrorLoop;
  FILE *fhTemp1, *fhTemp2;
  char strInput[MAX_LINE_LENGTH];
  char *strReturn;
  BOOL bInputError;
  char strInclude[_MAX_PATH+1+MAX_EXTENSION_LEN+1];
  
  /* Clear all the file handle storage */
  for (iLoop = 0; iLoop < 2*NUM_OUTPUT_TYPES; iLoop++)
    ppHandles[iLoop] = (FILE *)NULL;
  
  for (iLoop = 0; iLoop < NUM_OUTPUT_TYPES; iLoop++)
  {
    /* Is the output type enabled? */
    if (lpParams->bOutputEnabled[iLoop])
    {
      if (lpParams->bSeparateInclude)
      {
        strcpy(strInclude, lpParams->strIncludeFile);
        strncat(strInclude, ".", 1);
        strncat(strInclude, lpParams->strExtensions[iLoop], MAX_EXTENSION_LEN);
      }
      
      /* If we are to prompt on overwrite, check to see if output file exists already */
      if (!lpParams->bOverwrite)
      {

        fhTemp1 = fopen(lpParams->strOutput[iLoop], "rt");
        fhTemp2 = NULL;
        if (lpParams->bSeparateInclude)
          fhTemp2 = fopen(strInclude, "rt");
        if (fhTemp1 || fhTemp2)
        {
          /* File does exist. Does the user want to overwrite it? */
          if (fhTemp1)
            fclose(fhTemp1);
          if (lpParams->bSeparateInclude && fhTemp2)
            fclose(fhTemp2);
          
          do
          {
            if (!lpParams->bSeparateInclude)
              printf("Output file %s already exists. Overwrite? (Y/N):", lpParams->strOutput[iLoop]);
            else
              printf("Output file %s or %s already exists. Overwrite? (Y/N):", lpParams->strOutput[iLoop], strInclude);
            
            strReturn = NULL;

            while(strReturn == NULL)
              strReturn = gets(strInput);
              
            strReturn = RemoveLeadingWhitespace(strInput);
            
            if((strReturn[0] != 'N') || (strReturn[0] == 'n') ||
               (strReturn[0] != 'Y') || (strReturn[0] == 'y'))
              bInputError = FALSE;
            else
              bInputError = TRUE;   
              
          } while (bInputError);  
          
          /* If the user indicated not to overwrite, close any files we have already */
          /* opened and exit gracefully with an error.                               */
          if ((strReturn[0] == 'N') || (strReturn[0] == 'n'))
          {
            for (iErrorLoop = 0; iErrorLoop < iLoop; iErrorLoop++)
            {
              if (ppHandles[iErrorLoop] != (FILE *)NULL)
                fclose(ppHandles[iErrorLoop]);
              if (lpParams->bSeparateInclude)
                if (ppHandles[iErrorLoop+NUM_OUTPUT_TYPES] != (FILE *)NULL)
                  fclose(ppHandles[iErrorLoop+NUM_OUTPUT_TYPES]);
            }
            return(FALSE);
          }    
        }  
      }
      
      /* If we get here, the output type is enabled and the user wants us to */
      /* overwrite any existing file of the same name.                       */
      ppHandles[iLoop] = fopen(lpParams->strOutput[iLoop], "wt");
      if (lpParams->bSeparateInclude)
      {
		    ppHandles[iLoop+NUM_OUTPUT_TYPES] = fopen(strInclude, "wt");
      }
      
      /* If there is an error opening the file, clean up and return */
      if ((ppHandles[iLoop] == (FILE *)NULL) || 
          (lpParams->bSeparateInclude && (ppHandles[iLoop+NUM_OUTPUT_TYPES] == (FILE *)NULL)))
      {
        for (iErrorLoop = 0; iErrorLoop < iLoop; iErrorLoop++)
        {
          if (ppHandles[iErrorLoop] != (FILE *)NULL)
            fclose(ppHandles[iErrorLoop]);
          if (lpParams->bSeparateInclude)
            if (ppHandles[iErrorLoop+NUM_OUTPUT_TYPES] != (FILE *)NULL)
              fclose(ppHandles[iErrorLoop+NUM_OUTPUT_TYPES]);
        }
        return(FALSE);
      }
      
      switch(lpParams->eType)
      {
        case OUTPUT_WITHOUT_CHIP_TYPE:
        case OUTPUT_DEFAULT_VALUES:
          iErrorLoop = 0;
          while (strSwConfigPreamble[iErrorLoop] != NULL)
          {
            fprintf(ppHandles[iLoop], "%s%s%s\n", strCommentStart[iLoop], strSwConfigPreamble[iErrorLoop], strCommentEnd[iLoop]);
            iErrorLoop++;
          }      
      
          /* Write any multiple-inclusion protection strings */
          for(iErrorLoop = 0; iErrorLoop < NUM_PROT_PREAMBLE; iErrorLoop++)
            fprintf(ppHandles[iLoop], strProtectionPreamble[iLoop][iErrorLoop], lpParams->strProtection);
          break;
          
        case OUTPUT_WITH_CHIP_TYPE:
          iErrorLoop = 0;
          while (strPreamble[iErrorLoop] != NULL)
          {
            fprintf(ppHandles[iLoop], "%s%s%s\n", strCommentStart[iLoop], strPreamble[iErrorLoop], strCommentEnd[iLoop]);
            iErrorLoop++;
          }      
      
          /* Write any multiple-inclusion protection strings */
          for(iErrorLoop = 0; iErrorLoop < NUM_PROT_PREAMBLE; iErrorLoop++)
            fprintf(ppHandles[iLoop], strProtectionPreamble[iLoop][iErrorLoop], lpParams->strProtection);

          if (lpParams->bSeparateInclude)
          {
            iErrorLoop = 0;
            while (strInclPreamble[iErrorLoop] != NULL)
            {
              fprintf(ppHandles[iLoop+NUM_OUTPUT_TYPES], "%s%s%s\n", strCommentStart[iLoop], strInclPreamble[iErrorLoop], strCommentEnd[iLoop]);
              iErrorLoop++;
            }      
        
            /* Write any multiple-inclusion protection strings */
            for(iErrorLoop = 0; iErrorLoop < NUM_PROT_PREAMBLE; iErrorLoop++)
              fprintf(ppHandles[iLoop+NUM_OUTPUT_TYPES], strInclProtectionPreamble[iLoop][iErrorLoop], lpParams->strProtection);
          }
          break;
        
        case OUTPUT_DOCUMENTATION:
          /* Write the HTML preamble to the output file */
          iErrorLoop = 0;
          while (strHTMLPreamble[iErrorLoop] != NULL)
          {
            fprintf(ppHandles[iLoop], strHTMLPreamble[iErrorLoop]);
            iErrorLoop++;
          }      
          break;
      }
    }
  }  
  return(TRUE);
}

/********************************************************************/
/*  FUNCTION:    CloseAllOutputFiles                                */
/*                                                                  */
/*  PARAMETERS:  lpParams - command line parameters structure       */
/*               pHandles - pointer to an array of at least         */
/*                          NUM_OUTPUT_TYPES FILE *s containing file*/
/*                          handles as returned by a previous call  */
/*                          to OpenAllOutputFiles.                  */
/*                                                                  */
/*  DESCRIPTION: Write the appropriate "postamble" to all open      */
/*               output files then close them.                      */
/*                                                                  */
/*  RETURNS:     TRUE if all required output files are closed       */
/*               successfully, FALSE otherwise                      */
/*                                                                  */
/********************************************************************/
BOOL CloseAllOutputFiles(COMMAND_PARAMS *lpParams, FILE **ppHandles)
{
  int iLoop, iOtherLoop;
  
  /* For each possible output file ...*/
  for (iLoop = 0; iLoop < NUM_OUTPUT_TYPES; iLoop++)
  {
    /* If it is enabled and the file is open... */
    if(lpParams->bOutputEnabled[iLoop] && (ppHandles[iLoop] != (FILE *)NULL))
    {

      switch(lpParams->eType)
      {
        case OUTPUT_WITHOUT_CHIP_TYPE:
        case OUTPUT_DEFAULT_VALUES:
          /* Write the multiple inclusion protection end strings */
          for(iOtherLoop = 0; iOtherLoop < NUM_PROT_POSTAMBLE; iOtherLoop++)
            fprintf(ppHandles[iLoop], strProtectionPostamble[iLoop][iOtherLoop], lpParams->strProtection);

          /* Write the ending block of text */
          iOtherLoop = 0;
      
          while (strPostamble[iOtherLoop] != NULL)
          {
            fprintf(ppHandles[iLoop], "%s%s%s\n", strCommentStart[iLoop], strPostamble[iOtherLoop], strCommentEnd[iLoop]);
            iOtherLoop++;
          }      
          break;
         
        case OUTPUT_WITH_CHIP_TYPE:
          /* Write the multiple inclusion protection end strings */
          for(iOtherLoop = 0; iOtherLoop < NUM_PROT_POSTAMBLE; iOtherLoop++)
            fprintf(ppHandles[iLoop], strProtectionPostamble[iLoop][iOtherLoop], lpParams->strProtection);
            
          if(lpParams->bSeparateInclude && (ppHandles[iLoop+NUM_OUTPUT_TYPES] != (FILE *)NULL))
          {
            /* Write the multiple inclusion protection end strings */
            for(iOtherLoop = 0; iOtherLoop < NUM_PROT_POSTAMBLE; iOtherLoop++)
              fprintf(ppHandles[iLoop+NUM_OUTPUT_TYPES], strInclProtectionPostamble[iLoop][iOtherLoop], lpParams->strProtection);
          }

          /* Write the ending block of text */
          iOtherLoop = 0;
      
          while (strPostamble[iOtherLoop] != NULL)
          {
            fprintf(ppHandles[iLoop], "%s%s%s\n", strCommentStart[iLoop], strPostamble[iOtherLoop], strCommentEnd[iLoop]);
            if(lpParams->bSeparateInclude && (ppHandles[iLoop+NUM_OUTPUT_TYPES] != (FILE *)NULL))
            {
              fprintf(ppHandles[iLoop+NUM_OUTPUT_TYPES], "%s%s%s\n", strCommentStart[iLoop], strPostamble[iOtherLoop], strCommentEnd[iLoop]);
            }
            iOtherLoop++;
          }      
          break;
         
        case OUTPUT_DOCUMENTATION:

          /* Write the ending block of text */
          iOtherLoop = 0;
      
          while (strHTMLPostamble[iOtherLoop] != NULL)
          {
            fprintf(ppHandles[iLoop], strHTMLPostamble[iOtherLoop]);
            iOtherLoop++;
          }      
          break;
      } 

      /* Now close the file */
      fclose(ppHandles[iLoop]);
      ppHandles[iLoop] = (FILE *)NULL;
      if((lpParams->eType == OUTPUT_WITH_CHIP_TYPE) && (ppHandles[iLoop+NUM_OUTPUT_TYPES] != (FILE *)NULL))
      {
        fclose(ppHandles[iLoop+NUM_OUTPUT_TYPES]);
        ppHandles[iLoop+NUM_OUTPUT_TYPES] = (FILE *)NULL;
      }
    }
  }
  return(TRUE);
}

/********************************************************************/
/*  FUNCTION:    ParseCommandLine                                   */
/*                                                                  */
/*  PARAMETERS:  argc     - parameter count as passed to main()     */
/*               argv     - parameter strings as passed to main()   */
/*               lpParams - structure filled in with user's parsed  */
/*                          command line strings.                   */
/*                                                                  */
/*  DESCRIPTION: Parse the command line, retrieving all necessary   */
/*               options and filenames. Check for illegal options   */
/*               and non-sensible strings.                          */
/*                                                                  */
/*  RETURNS:     TRUE if command line is parsed with no errors,     */
/*               FALSE otherwise.                                   */
/*                                                                  */
/********************************************************************/
BOOL ParseCommandLine(int argc, char *argv[], COMMAND_PARAMS *lpParams)
{
  int iLoop;
  int iStringLen;
  int iIndex;
  char *strError;
  char szBuffer[MAX_ERROR_STRING_LEN];
  BOOL bError = FALSE;
  BOOL bTypeSelected = FALSE;
  
  /***********************************************************/
  /* Check that we have at least 2 parameters + program name */
  /***********************************************************/
  if(argc < 3)
  {
    ShowSplash();
    ShowUsage();
    return(FALSE);
  }

  /**********************/
  /* Set default values */
  /**********************/
  lpParams->bQuiet             = FALSE;
  lpParams->bOverwrite         = FALSE;
  lpParams->bSeparateInclude   = FALSE;
  lpParams->bEchoComments      = TRUE;
  lpParams->bDefaultFound      = FALSE;
  lpParams->bDescFound         = FALSE;
  lpParams->bInsideStruct      = FALSE;
  lpParams->bIgnoreWhitespace  = FALSE;
  lpParams->bLastLineContinues = FALSE;
  lpParams->eType             = OUTPUT_WITH_CHIP_TYPE;
  
  for (iLoop = 0; iLoop < NUM_OUTPUT_TYPES; iLoop++)
  {
    lpParams->bOutputEnabled[iLoop] = FALSE;
    lpParams->strOutput[iLoop][0] = (char)0;
    strcpy(lpParams->strExtensions[iLoop], strDefaultExtensions[iLoop]);
  }
  
  lpParams->strInput[0]       = (char)0;
  lpParams->strChipName[0]    = (char)0;
  lpParams->strLastKey[0]     = (char)0;
  lpParams->strProtection[0]  = (char)0;
  
  /***************************************************/
  /* Run through the argument list looking for flags */
  /***************************************************/
  iLoop = 1;

  while((iLoop < argc) && !bError && ((argv[iLoop][0] == '-') || (argv[iLoop][0] == '/')))
  {
    iStringLen = strlen(argv[iLoop]);
    
    switch(argv[iLoop][1])
    {
      case '\0':
        break;

      case 'i':
      case 'I':
        if((iStringLen > 2) && (iStringLen < (_MAX_PATH+2)))
          strcpy(lpParams->strInput, argv[iLoop]+2);
        else
        {
          bError = TRUE;
          strError = "Input file name must be between 1 and _MAX_PATH characters long";
        }  
        break;

      case 'c':
      case 'C':
        if((iStringLen > 2) && (iStringLen < (_MAX_PATH+2)))
        {
          lpParams->bOutputEnabled[OUTPUT_C] = TRUE;
          strcpy(lpParams->strOutput[OUTPUT_C], argv[iLoop]+2);
        }  
        else
        {
          bError = TRUE;
          strError = "C output file name must be between 1 and _MAX_PATH characters long";
        }
        break;

      case 'a':
      case 'A':
        if((iStringLen > 2) && (iStringLen < (_MAX_PATH+2)))
        {
          lpParams->bOutputEnabled[OUTPUT_SDT_ASM] = TRUE;
          strcpy(lpParams->strOutput[OUTPUT_SDT_ASM], argv[iLoop]+2);
        }  
        else
        {
          bError = TRUE;
          strError = "SDT assembler output file name must be between 1 and _MAX_PATH characters long";
        }
        break;

      case 'v':
      case 'V':
        if((iStringLen > 2) && (iStringLen < (_MAX_PATH+2)))
        {
          lpParams->bOutputEnabled[OUTPUT_VX_ASM] = TRUE;
          strcpy(lpParams->strOutput[OUTPUT_VX_ASM], argv[iLoop]+2);
        }  
        else
        {
          bError = TRUE;
          strError = "GNU assembler output file name must be between 1 and _MAX_PATH characters long";
        }
        break;
        
      case 't':
      case 'T':
        if((iStringLen > 2) && (iStringLen < (_MAX_PATH+2)))
        {
          lpParams->bOutputEnabled[OUTPUT_NMAKE] = TRUE;
          strcpy(lpParams->strOutput[OUTPUT_NMAKE], argv[iLoop]+2);
        }  
        else
        {
          bError = TRUE;
          strError = "NMAKE macro output file name must be between 1 and _MAX_PATH characters long";
        }
        break;

      case 'r':
      case 'R':
        if((iStringLen > 2) && (iStringLen < (_MAX_PATH+2)))
        {
          lpParams->bOutputEnabled[OUTPUT_MAKE] = TRUE;
          strcpy(lpParams->strOutput[OUTPUT_MAKE], argv[iLoop]+2);
        }  
        else
        {
          bError = TRUE;
          strError = "MAKE macro output file name must be between 1 and _MAX_PATH characters long";
        }
        break;

      case 'p':
      case 'P':
        if((iStringLen > 2) && (iStringLen < (MAX_LINE_LENGTH+2)))
        {
          strcpy(lpParams->strProtection, argv[iLoop]+2);
        }  
        else
        {
          bError = TRUE;
          strError = "Protection string must be between 1 and 256 characters long";
        }
        break;

      case '1':
      case '2':
      case '3':
      case '4':
      case '5':
	  case '6':
        if((iStringLen > 2) && (iStringLen <= (MAX_EXTENSION_LEN+2)))
        {
          switch (argv[iLoop][1])
          {
            case '1': iIndex = OUTPUT_C; break;
            case '2': iIndex = OUTPUT_SDT_ASM; break;
            case '3': iIndex = OUTPUT_VX_ASM; break;
            case '4': iIndex = OUTPUT_HTML; break;
            case '5': iIndex = OUTPUT_NMAKE; break;
			case '6': iIndex = OUTPUT_MAKE; break;
          }    
          strcpy(lpParams->strExtensions[iIndex], argv[iLoop]+2);
        }  
        else
        {
          bError = TRUE;
          strError = "File extensions provided must be between 1 and 3 characters long";
        }
        break;

      case 'd':
      case 'D':
        if(bTypeSelected)
        {
          bError = TRUE;
          strError = "Multiple output types selected. Use only 1 of -d, -s or -m";
        }
        else
        {
          lpParams->eType = OUTPUT_DEFAULT_VALUES;
          bTypeSelected = TRUE;
        }
        break;

      case 'x':
      case 'X':
        lpParams->bEchoComments = FALSE;
        break;

      case 'w':
      case 'W':
        lpParams->bIgnoreWhitespace = TRUE;
        break;

      case 'q':
      case 'Q':
        lpParams->bQuiet = TRUE;
        break;

      case 'y':
      case 'Y':
        lpParams->bOverwrite = TRUE;
        break;

      case 's':
      case 'S':
        if(bTypeSelected)
        {
          bError = TRUE;
          strError = "Multiple output types selected. Use only 1 of -d, -s or -m";
        }
        else
        {
          lpParams->eType = OUTPUT_WITHOUT_CHIP_TYPE;
          bTypeSelected = TRUE;
        }
        
        break;

      case 'm':
      case 'M':
        if(bTypeSelected)
        {
          bError = TRUE;
          strError = "Multiple output types selected. Use only 1 of -d, -s or -m";
        }
        else
        {
          lpParams->eType = OUTPUT_DOCUMENTATION;
          bTypeSelected = TRUE;
          if((iStringLen > 2) && (iStringLen < (_MAX_PATH+2)))
          {
            lpParams->bOutputEnabled[OUTPUT_HTML] = TRUE;
            strcpy(lpParams->strOutput[OUTPUT_HTML], argv[iLoop]+2);
          }  
          else
          {
            bError = TRUE;
            strError = "HTML output file name must be between 1 and _MAX_PATH characters long";
          }
        }
        break;

      case 'n':
      case 'N':
        if((iStringLen > 2) && (iStringLen < (_MAX_PATH+2)))
        {
          strcpy(lpParams->strIncludeFile, argv[iLoop]+2);
          lpParams->bSeparateInclude = TRUE;
        }  
        else
        {
          bError = TRUE;
          strError = "Chip output file name must be between 1 and _MAX_PATH characters long";
        }
        break;

      case 'h':
      case '?':
      case 'H':
        ShowSplash();
        ShowUsage();
        return(FALSE);

      default:
        bError = TRUE;
        sprintf(szBuffer, "Unrecognised command line flag %2s\n\n", argv[iLoop]);
        strError = szBuffer;
        break;
    }
    iLoop++;
   }

   if(!bError)
   {
     /********************************************/
     /* Make sure an input filename was provided */
     /********************************************/
     if(lpParams->strInput[0] == (char)0)
     {
         bError = TRUE;
         strError ="An an input file must be specified using the -i<filename> switch.\n";
     }
     else
     {
       /*****************************************************/
       /* Make sure conflicting switches were not specified */
       /*****************************************************/
       if ((lpParams->eType == OUTPUT_WITHOUT_CHIP_TYPE) && (lpParams->bSeparateInclude == TRUE))
       {
         bError = TRUE;
         strError ="The -n and -s switches cannot be specified together.\n";
       }
       else
       {
         if(lpParams->eType == OUTPUT_DOCUMENTATION)
         {
           if(lpParams->bSeparateInclude)
           {
             bError = TRUE;
             strError = "The -m and -n switches cannot be specified together.\n";
           }
           
           for(iLoop = 0; iLoop < NUM_OUTPUT_TYPES; iLoop++)
           {
             if(lpParams->bOutputEnabled[iLoop] && (iLoop != OUTPUT_HTML))
             {
               bError = TRUE;
               strError = "The -a, -v, -c and -t switches may not be used in conjunction with -m\n";
               break;
             }
           }  
         }
         else
         {
           /* The nmake and make output option are only valid to generate a default definition file */
           if(( lpParams->bOutputEnabled[OUTPUT_NMAKE] ||
			    lpParams->bOutputEnabled[OUTPUT_MAKE] )&& 
			  ( lpParams->eType != OUTPUT_DEFAULT_VALUES))
           {
             bError = TRUE;
             strError = "The -t|-r option is only valid when used in conjunction with -d\n";
           }
         }
       }
     }
             
     /**********************************************************************/         
     /* Determine which multi-include protection string to use if none was */
     /* specified on the command line.                                     */
     /**********************************************************************/         
     if(lpParams->strProtection[0] == (char)0)
     {
       if(lpParams->eType == OUTPUT_WITHOUT_CHIP_TYPE)
         strcpy(lpParams->strProtection, strDefaultProtectionInserts[SWCONFIG_STRING]);
       else
         strcpy(lpParams->strProtection, strDefaultProtectionInserts[CONFIG_STRING]);
     }
   }
   
   /***********************************/
   /* Check for errors during parsing */
   /***********************************/
   if (bError)
   {
     ShowSplash();
     printf("\nERROR: %s", strError);
     return(FALSE);
   }  
   else
     return(TRUE);
}

/********************************************************************/
/*  FUNCTION:    ShowSplash                                         */
/*                                                                  */
/*  PARAMETERS:  None                                               */
/*                                                                  */
/*  DESCRIPTION: Display basic tool name and information along with */
/*               a copyright notice.                                */
/*                                                                  */
/*  RETURNS:     Nothing                                            */
/*                                                                  */
/********************************************************************/
void ShowSplash(void)
{
  printf(
     "\nCFGTOH - Convert a build config file into equivalent C and assembler headers"
     "\n(c) Conexant Systems Inc, 2001, 2002. All Rights Reserved\n"
     "\n%s\n\n", vcsid);
}

/********************************************************************/
/*  FUNCTION:    ShowUsage                                          */
/*                                                                  */
/*  PARAMETERS:  None                                               */
/*                                                                  */
/*  DESCRIPTION: Display usage information for the tool.            */
/*                                                                  */
/*  RETURNS:     Nothing                                            */
/*                                                                  */
/********************************************************************/
void ShowUsage(void)
{
  printf("  Usage: CFGTOH -i<input> [[-m<filename>] | [-c<c_output>] [-a<asm_outout>]\n");
  printf("                 [-v<vx_output>] [-t<make_outout>] [-q] [-y] [-x] [-d]\n");
  printf("                 [-1<ext>] [-2<ext>] [-3<ext>] [-4<ext>] [-5<ext>]\n");
  printf("                 [-n<filename>] [-p<string>]] [-h|H|?]\n\n");
  printf("  where -i indicates the input configuration filename\n");
  printf("        -c path and filename for output C header file\n");
  printf("        -a path and filename for output SDT assembler header file\n");
  printf("        -v path and filename for output VxWorks/GNU assembler header file\n");
  printf("        -t path and filename for output nmake macro definitions file\n");
  printf("        -r path and filename for output Gnumake macro definitions file\n");
  printf("        -q enable quiet mode generating no console output\n");
  printf("        -m output HTML file (not valid with any other output format option)\n");
  printf("        -p specify a string to use in multi-include protection lines\n");
  printf("        -y overwrite existing output files without prompting\n");
  printf("        -s process a software config file\n");
  printf("        -x do not echo comments to output\n");
  printf("        -w ignore whitespace. By default, blank lines are echoed to output\n");
  printf("        -n specify that chip header incl should be written to a separate file\n");
  printf("        -d generate default definitions using CFG_DEFAULT lines from input\n");
  printf("        -1 replace default C header extension 'H' with following string\n");
  printf("        -2 replace default SDT asm extension 'A' with following string\n");
  printf("        -3 replace default GNU asm extension 'VXA' with following string\n");
  printf("        -4 replace default HTML extension 'HTML' with following string\n");
  printf("        -5 replace default MAK extension 'MAK' with following string\n");
  printf("        -h display this help information. -H and -? are equivalent\n\n");
  printf("  Example:\n\n");
  printf("    CFGTOH -icx2249x.cfg -ccx2249x.h -q -acx2249x.a\n\n");
  printf("  converts cx2249x.cfg into C and SDT-format assembler headers\n");
  printf("  cx2249x.h and cx2249x.a respectively in the current directory.\n");
}

/********************************************************************/
/*  FUNCTION:    toint                                              */
/*                                                                  */
/*  PARAMETERS:  int c - the ascii value of a character (0-9 only)  */
/*                                                                  */
/*  DESCRIPTION: Simple function for converting characters to ints  */
/*                                                                  */
/*  RETURNS:     0-9                                                */
/*                                                                  */
/********************************************************************/
int toint( int c )
{
   if(c >= '0' && c <= '9') return c -  '0';
   /* c is not a decimal digit */
   return 0;
}

/********************************************************************/
/*  FUNCTION:    strtolower                                         */
/*                                                                  */
/*  PARAMETERS:  src - pointer to array of characters containing    */
/*                     src string to convert to lower case          */
/*               dest - pointer to array of characters for          */
/*                      converted string                            */
/*                                                                  */
/*  DESCRIPTION: Converts a string to lower case                    */
/*                                                                  */
/*  RETURNS:     nothing                                            */
/*                                                                  */
/********************************************************************/
void strtolower(char *src, char *dest)
{
   while ((src != 0) && (dest != 0) && (*src)) 
   {
      *dest++ = tolower(*src++);
   }
   *dest = 0;

}

/********************************************************************/
/*  FUNCTION:    strSubContLineChar                                 */
/*                                                                  */
/*  PARAMETERS:  OUTPUT_TYPE eType: The type of output file.        */
/*                                  Used to determine the sub char  */
/*               char *string:      The string to modify            */
/*                                                                  */
/*  DESCRIPTION: Subsitutues continuation line characters in string */
/*               for the string specified in                        */
/*               strSubContinuationLine[eType]                      */
/*                                                                  */
/*  RETURNS:     1                                                  */
/*                                                                  */
/********************************************************************/
int strSubContLineChar(OUTPUT_TYPE eType, char *string)
{
   char seps[] = "\\";
   char *token;
   char prevToken[MAX_LINE_LENGTH];
   char strSrc[MAX_LINE_LENGTH+1];
   char strDes[MAX_LINE_LENGTH+30];

   strcpy(strSrc, string);
   strcpy(strDes,"\0");
   
   token = strtok(strSrc, seps);
   strcpy(prevToken,token); 
   token = strtok(NULL,seps);
   while(token != NULL)
   {
      strcat(strDes,prevToken);
      strcat(strDes,strSubContinuationLine[eType]);
   
      strcpy(prevToken,token);
      token = strtok(NULL,seps);
   }
   strcat(strDes,prevToken);
   
   strcpy(string, strDes);
   
   return 1;
}

/****************************************************************************
 * Modifications:
 * $Log$
 * 
 ****************************************************************************/


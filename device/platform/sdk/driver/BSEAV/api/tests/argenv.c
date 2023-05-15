/***************************************************************************
 *     (c)2012 Broadcom Corporation
 *
 *  This program is the proprietary software of Broadcom Corporation and/or its licensors,
 *  and may only be used, duplicated, modified or distributed pursuant to the terms and
 *  conditions of a separate, written license agreement executed between you and Broadcom
 *  (an "Authorized License").  Except as set forth in an Authorized License, Broadcom grants
 *  no license (express or implied), right to use, or waiver of any kind with respect to the
 *  Software, and Broadcom expressly reserves all rights in and to the Software and all
 *  intellectual property rights therein.  IF YOU HAVE NO AUTHORIZED LICENSE, THEN YOU
 *  HAVE NO RIGHT TO USE THIS SOFTWARE IN ANY WAY, AND SHOULD IMMEDIATELY
 *  NOTIFY BROADCOM AND DISCONTINUE ALL USE OF THE SOFTWARE.
 *
 *  Except as expressly set forth in the Authorized License,
 *
 *  1.     This program, including its structure, sequence and organization, constitutes the valuable trade
 *  secrets of Broadcom, and you shall use all reasonable efforts to protect the confidentiality thereof,
 *  and to use this information only in connection with your use of Broadcom integrated circuit products.
 *
 *  2.     TO THE MAXIMUM EXTENT PERMITTED BY LAW, THE SOFTWARE IS PROVIDED "AS IS"
 *  AND WITH ALL FAULTS AND BROADCOM MAKES NO PROMISES, REPRESENTATIONS OR
 *  WARRANTIES, EITHER EXPRESS, IMPLIED, STATUTORY, OR OTHERWISE, WITH RESPECT TO
 *  THE SOFTWARE.  BROADCOM SPECIFICALLY DISCLAIMS ANY AND ALL IMPLIED WARRANTIES
 *  OF TITLE, MERCHANTABILITY, NONINFRINGEMENT, FITNESS FOR A PARTICULAR PURPOSE,
 *  LACK OF VIRUSES, ACCURACY OR COMPLETENESS, QUIET ENJOYMENT, QUIET POSSESSION
 *  OR CORRESPONDENCE TO DESCRIPTION. YOU ASSUME THE ENTIRE RISK ARISING OUT OF
 *  USE OR PERFORMANCE OF THE SOFTWARE.
 *
 *  3.     TO THE MAXIMUM EXTENT PERMITTED BY LAW, IN NO EVENT SHALL BROADCOM OR ITS
 *  LICENSORS BE LIABLE FOR (i) CONSEQUENTIAL, INCIDENTAL, SPECIAL, INDIRECT, OR
 *  EXEMPLARY DAMAGES WHATSOEVER ARISING OUT OF OR IN ANY WAY RELATING TO YOUR
 *  USE OF OR INABILITY TO USE THE SOFTWARE EVEN IF BROADCOM HAS BEEN ADVISED OF
 *  THE POSSIBILITY OF SUCH DAMAGES; OR (ii) ANY AMOUNT IN EXCESS OF THE AMOUNT
 *  ACTUALLY PAID FOR THE SOFTWARE ITSELF OR U.S. $1, WHICHEVER IS GREATER. THESE
 *  LIMITATIONS SHALL APPLY NOTWITHSTANDING ANY FAILURE OF ESSENTIAL PURPOSE OF
 *  ANY LIMITED REMEDY.
 *
 * $brcm_Workfile: argenv.c $
 * $brcm_Revision: 2 $
 * $brcm_Date: 9/6/12 6:06p $
 *
 * Module Description:
 *
 * Revision History:
 *
 * $brcm_Log: /BSEAV/api/tests/argenv.c $
 * 
 * 2   9/6/12 6:06p erickson
 * SW7435-349: add standard header
 * 
 **************************************************************************/
/* $Id: argenv.c,v 1.6 2005/04/26 21:39:34 lhamilton Exp $ */
/* ARGENV.CPP - Copyright (C) 2001 Sand Video, Inc., All Rights Reserved
Command line and environment variable access utilities.
Supported command line format:
  <command>   ::= <executable> [{<nonswitch>}] [{<switch>}]
  <nonswitch> ::= (string not beginning with '/' or '-')
                  Exception: nonswitches may begin with '-' if followed
                  entirely by numbers [i.e. it is a negative number]
  <switch>    ::= (/ | -)<switch name>[=<swvalue>]
  <swvalue>   ::= (any string)

  Switches may only appear after all nonswitches.

  Special indirect command mode uses exactly one argument specifying a file with
    all the items that would normally be on the command line.  There should be
    exactly one switch (with associated values) or nonswitch per line.  Format:

  <indirect command line> ::= <executable> @<indirect command filename>

Revision history:
  mm/dd/yyyy Who        Description
  01/25/2001 John Iler  initial release
*/
/* File includes *************************************************************/
#include <stdlib.h>
#include <stdio.h>
#include <ctype.h>
#include <string.h>

/* Defines *******************************************************************/
#define MAXARGLINE          128   /* longest line in indirect command file */

/* Determines if string is in command line switch format.
Args: szParam - command line parameter.  If has switch prefix and is not a
  negative number, then assume it is a switch.
Returns: 1 - szParam is switch, else 0
Effects: none
*/
#if defined(_MSC_VER)
#define IS_SWITCH(szParam) \
  (((szParam[0] == '-') && !isdigit(szParam[1])) || (szParam[0] == '/'))
#else
/* '/' switch introducer disallowed in UNIX */
#define IS_SWITCH(szParam) ((szParam[0] == '-') && !isdigit(szParam[1]))
#endif

/* Structure definitions *****************************************************/

/* Function declarations *****************************************************/

/* Global/static variables ***************************************************/
/* static variables allow utility functions to access arguments to main() */
static int nArgcCopy;             /* number of command line arguments */
static char **cpArgvCopy;   /* array of command line arguments */
static char **cpEnvpCopy;   /* array of OS environment variables */

/* Routines ******************************************************************/

int ArgEnvInit(int nArgc, char *cpArgv[], char *cpEnvp[])
{
  /* Initializes command line/environment variable processing utilities.  Must be
    called once before using any utility functions.
  Args: nArgc - # of command line arguments (from standard main() argument)
    cpArgv - command line arguments (from standard main() argument)
    cpEnvp - environment variables (from standard main() argument).  Set to NULL if
      not using environment variable routines (EnvValue, EnvExist).
  Returns: < 0 error, else 0
  Effects: sets static globals nArgcCopy, cpArgvCopy, cpEnvpCopy
  */
  if ((nArgc == 2) && (cpArgv[1][0] == '@'))
  {
    /* have indirect command file */
    FILE *fpArgs; /* indirect command file */
    int nIter;
    char szArgLine[MAXARGLINE];

    /* count number of lines (1 per [non]switch), allocate cpArgvCopy array */
    if ((fpArgs = fopen(&cpArgv[1][1], "rt")) == NULL)
      return(-1);
    
    for (nIter = 0; fgets(szArgLine, sizeof(szArgLine), fpArgs) != NULL; nIter++)
      ;
    
    nArgcCopy = nIter + 1;  /* +1 for cpArgv[0] */
    /* +1 allows for terminating NULL pointer */
    if ((cpArgvCopy = (char **)malloc(sizeof(char *) * (nArgcCopy + 1))) == NULL)
      return(-1);

    fseek(fpArgs, 0, SEEK_SET); /* rewind so can process arguments */
    if ((cpArgvCopy[0] = (char *)malloc(strlen(cpArgv[0]) + 1)) == NULL)
      return(-1);

    strcpy(cpArgvCopy[0], cpArgv[0]);

    /* copy each indirect argument line to argument array (1 [non]switch per line) */
    for (nIter = 1; nIter < nArgcCopy; nIter++)
    {
      char *szSrc, *szDst;  /* source and destination of line adjustment */

      if (fgets(szArgLine, sizeof(szArgLine), fpArgs) == NULL)
        return(-1);

      if (szArgLine[strlen(szArgLine) - 1] == '\n')
      {
        szArgLine[strlen(szArgLine) - 1] = '\0'; /* remove trailing newline */
      }

      /* put in /switch=value format (i.e. require '=', no spaces around it) */
      szSrc = szArgLine;
      szDst = szSrc;

      /* skip leading white space */
      while (*szSrc && isspace (*szSrc))
        szSrc++;

      /* shift buffer, if needed */
      if (szSrc != szDst)
      {
        while (*szSrc != '\0')
          *szDst++ = *szSrc++;

        *szDst = '\0'; /* terminate processed (in place) string */
      }
      else
          szDst = szArgLine + strlen(szArgLine);

      /* trim trailing white space? */
      while ((szDst > szArgLine) && isspace (*(--szDst)))
          *szDst = '\0';

      cpArgvCopy[nIter] = (char *)malloc(sizeof(char) * strlen(szArgLine) + 1);
      if (cpArgvCopy[nIter] == NULL) 
      {
        return(-1);
      }
      strcpy(cpArgvCopy[nIter], szArgLine);
    } /* for */
    cpArgvCopy[nArgcCopy] = NULL; /* terminates array (C convention) */
    fclose(fpArgs);
  } /* if */
  else
  {
    /* not using indirect command file */
    nArgcCopy = nArgc;
    cpArgvCopy = cpArgv;
  }
  cpEnvpCopy = cpEnvp;
  return(0);
} /* ArgEnvInit */

char *ArgValid(const char *szAllowed)
{
  /* Find any invalid switches on command line.
  Args: szAllowed - list of valid switches.  The delimiter used between switches
    must consistantly be the first character in the string (e.g. "/abc/def/ghi").
    One switch may not be a prefixing substring of another switch.  Switches must
    start with a nonnumeric character.  Switches are case sensitive.  
  Returns: NULL if all switches are valid, else pointer to first invalid switch
  Effects: requires previous call to ArgEnvInit()
  */
  int nIter;
  char cDelimit;    /* delimiter between switches in szAllowed */
  char *szArg;      /* argument iterator */

  cDelimit = szAllowed[0];
  /* loop through command line args (after program name), look for switches */
  for (nIter = 1; nIter < nArgcCopy; nIter++)
  {
    szArg = cpArgvCopy[nIter];

    if (IS_SWITCH(szArg))
    {
      /* Found switch.  If any valid switch matches the beginning of argument, it 
        cannot be called invalid since rest of switch string may be a value (since
        the value is not required to be delimited from the switch).
      */
      const char *szUnchecked;  /* szAllowed switches untested vs. parameter */

      for (szUnchecked = szAllowed; szUnchecked != NULL; 
        szUnchecked = strchr(&szUnchecked[1], cDelimit))
      {
        /* loop through switches, using delimiter to find next in list */
        char *szNextDelimit;    /* location of next delimiter within szUnchecked */
        size_t stSwitchLength;  /* trial switch length */
        
        szUnchecked++;  /* skip delimiter */
        stSwitchLength = ((szNextDelimit = strchr(szUnchecked, cDelimit)) == NULL) ?
          strlen(szUnchecked) : (size_t)(szNextDelimit - szUnchecked);
        /* break if valid switch match */
        if (!strncmp(szArg + 1, szUnchecked, stSwitchLength))
          break;
      }
      if (szUnchecked == NULL)
        return(szArg); /* no current (invalid) arg match */
    } /* if */
  } /* for */
  return(NULL); /* all command switches were matched (may be valid) */
} /* ArgValid */


char *ArgValue(const char *szId)
{
  /* Determines existance and value of command line switch or nonswitch.
  Args: szId - if first character is numeric, this specifies which nonswitch
    parameter to find ("0" is program name).  If first character is nonnumeric
    (may be preceded by optional '-' or '/') then value associated with switch is
    found (if any).
  Returns: value of command line parameter or switch.  NULL if switch does not 
    exist.  Return is always a string, which must be converted to other datatypes
    if desired, using functions such as atoi().  String will be "" if there is a
    valid switch that happens to have no value.
  Effects: requires previous call to ArgEnvInit()
  */
  int nIter;

  if (isdigit(szId[0]))
  {
    /* look for nonswitch command line parameter */
    int nNonSwitchesToGo;

    /* validate that entire nonswitch identifier is a number */
    for (nIter = 0; nIter < (int)strlen(szId); nIter++)
    {
      if (!isdigit(szId[nIter]))
        return(NULL); /* nondigit character */
    }
    nNonSwitchesToGo = atoi(szId);
    for (nIter = 0; nIter < nArgcCopy; nIter++)
    {
      if (IS_SWITCH(cpArgvCopy[nIter]))
      {
        /* found a switch, stop searching, no further nonswitches allowed */
        break;
      } /* if */
      /* have nonswitch */
      if (!nNonSwitchesToGo--)
          return(cpArgvCopy[nIter]); /* desired nonswitch */
    } /* for */
  }
  else
  {
    /* look for switch and associated value */
    const char *szDesiredSwitch;  /* switch without switch prefix */
    size_t stSwitchLength;
    szDesiredSwitch = ((szId[0] == '/') || (szId[0] == '-')) ? szId + 1 : szId;
    stSwitchLength = strlen(szDesiredSwitch);
    for (nIter = 0; nIter < nArgcCopy; nIter++)
    {
      if (IS_SWITCH(cpArgvCopy[nIter]) && 
        !strncmp(szDesiredSwitch, &cpArgvCopy[nIter][1], stSwitchLength))
      {
        /* Found switch, now retrieve (optional) value.  Value may be preceded by
          optional whitespace and/or optional '='.  Merge remainder of current
          parameter and next 2, remove optional '='.  Remainder then up to next
          switch is value.  (White space is automatically removed by command line
          parser.)
        */
        char *szVal;
        char *szNoValue;  /* pointer to '\0', indicating no value was found */

        /* validate that entire name matched */
        char term = cpArgvCopy[nIter][stSwitchLength + 1];
        if ((term != '\0') && (term != '='))
            continue;

        szVal = &cpArgvCopy[nIter][stSwitchLength + 1]; /* rest of current arg */
        szNoValue = &cpArgvCopy[nIter][strlen(cpArgvCopy[nIter])];
		if (*szVal != '=')
			return szNoValue;
        szVal++; /* skip '=' */
        if (strlen(szVal))
          return(szVal); /* if more chars, they are value */
        else
#if 1
		  return szNoValue;
#else
        {
          if (nArgcCopy > (nIter + 1))
          {
            /* no value yet, look for in next argument */
            szVal = cpArgvCopy[nIter + 1];
            if (IS_SWITCH(szVal))
              return(szNoValue); /* switch, so no value */
            if (*szVal == '=')
              szVal++; /* skip optional '=' */
            if (strlen(szVal))
              return(szVal); /* if more chars, they are value */
            else
            {
              if (nArgcCopy > (nIter + 2))
              {
                /* no value yet, look in next argument.  Only get here if had a
                  whitespace surrounded '='.
                */
                szVal = cpArgvCopy[nIter + 2];
                if (IS_SWITCH(szVal))
                  return(szNoValue); /* switch, so no value */
                else
                  return(szVal); /* must be value if not switch */
              }
              else
                return(szNoValue); /* no further arguments for value */
            }
          }
          else return(szNoValue); /* no further arguments for value */
        } /* else */
#endif
      } /* if */
    } /* for */
  } /* else */
  return(NULL); /* did not find desired (non)switch */
} /* ArgValue */

/*************************************************************
 * Parse the passed in string and place each token (separated
 * by a space) into the array of command line parameters
 *
 * This modifies the passed in string
 *
 * Returns the number of tokens matched
 *
 */
int ArgEnvSetup(char *buf)
{
    char buf2[4096];
    int nArgs = 0;
    char *token;
    int i;

    /* copy the buffer so we can process it twice */
    strncpy(buf2, buf, sizeof buf2);

    token = strtok( buf2, " \n" );
    while( token != NULL )
    {
       nArgs++;
      /* Get next token: */
      token = strtok( NULL, " \n" );
   }

    nArgcCopy = nArgs;

    /* allocate memory to hold pointers to the arguments */
    /* +1 allows for terminating NULL pointer */
    if ((cpArgvCopy = (char **)malloc(sizeof(char *) * (nArgcCopy + 1))) == NULL)
      return(0);

    /* Set the argument pointers to the argument values in the original array */
    token = strtok( buf, " \n" );
    for (i = 0; i < nArgcCopy; i++)
    {
        cpArgvCopy[i] = token;
        /* Get next token: */
        token = strtok( NULL, " \n" );
    }
    cpArgvCopy[nArgcCopy] = NULL; /* terminate the array */

/* debug */
    for (i = 0; i < nArgcCopy; i++)
        printf("Token: %s\n", cpArgvCopy[i]);
    printf ("End of Tokens\n");
    return(nArgcCopy);
}

void ArgFreeMemory()
{
	if (cpArgvCopy)
	{
		free(cpArgvCopy);
		cpArgvCopy = 0;
	}
}


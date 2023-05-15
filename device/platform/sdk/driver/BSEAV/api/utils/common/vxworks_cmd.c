/***************************************************************************
 *     Copyright (c) 2005-2006, Broadcom Corporation
 *     All Rights Reserved
 *     Confidential Property of Broadcom Corporation
 *
 *  THIS SOFTWARE MAY ONLY BE USED SUBJECT TO AN EXECUTED SOFTWARE LICENSE
 *  AGREEMENT  BETWEEN THE USER AND BROADCOM.  YOU HAVE NO RIGHT TO USE OR
 *  EXPLOIT THIS MATERIAL EXCEPT SUBJECT TO THE TERMS OF SUCH AN AGREEMENT.
 *
 * $brcm_Workfile: vxworks_cmd.c $
 * $brcm_Revision: 5 $
 * $brcm_Date: 8/25/06 11:45a $
 *
 * Module Description:
 *
 * Revision History:
 *
 * $brcm_Log: /BSEAV/api/utils/common/vxworks_cmd.c $
 * 
 * 5   8/25/06 11:45a rjlewis
 * PR20604: just set priority (its alreay in vxworks format).
 * 
 * 3   7/24/06 1:34p rjlewis
 * PR22767: Don't check null pointer. Added comments defining why this is
 * here.
 * 
 */

#include <stdio.h>
#include <string.h>
#include <taskLib.h>

/*
** An application is started in the VxWorks environment by entering (typing) the function name in the target shell.
** If not parameters a passed (in parenthesis) then all arguments to the function are provided as 0 (or null).
** Providing a list of arguments (for argv in main) is hard to do.  This provides a wrapper function that parses
** the command like (parameters passed) into space separated arguments and passes those arguments to main.
*/

extern int main(int argc, char **argv);

int go(char *cmd)
{
	int argc = 0;
	char *argv[100];
	char *s;

	/* This assumes that the first param in the script will be the app name (standard) */

    /* If they don't include any options or if they start with just the options listed,
    ** apply a dummy 'program name' option to the front of the argument list.
    */
    if (!cmd || *cmd == '-') argv[argc++] = "go";
    
	s = strtok(cmd, " ");
	while (s) {
		printf("argv[%d] = %s\n", argc, s);
		argv[argc++] = s;
		s = strtok(NULL, " ");
	}

    /* Need this to prevent a crash -- we always get an argc and argv, right? */
    if (!argc) argv[argc++] = "go";
    
    /* VxWorks starts the default task off with a high priority.
    ** Need apps to run at priority less than the other maintenence tasks.
    */
    taskPrioritySet(taskIdSelf(),40);

	return main(argc, argv);
}

/***************************************************************************
 *     Copyright (c) 2003-2012, Broadcom Corporation
 *     All Rights Reserved
 *     Confidential Property of Broadcom Corporation
 *
 *  THIS SOFTWARE MAY ONLY BE USED SUBJECT TO AN EXECUTED SOFTWARE LICENSE
 *  AGREEMENT  BETWEEN THE USER AND BROADCOM.  YOU HAVE NO RIGHT TO USE OR
 *  EXPLOIT THIS MATERIAL EXCEPT SUBJECT TO THE TERMS OF SUCH AN AGREEMENT.
 *
 * $brcm_Workfile: bdbg.h $
 * $brcm_Revision: Hydra_Software_Devel/55 $
 * $brcm_Date: 2/22/12 1:44p $
 *
 * Module Description:
 *
 * Revision History:
 *
 * $brcm_Log: /magnum/basemodules/dbg/bdbg.h $
 * 
 * Hydra_Software_Devel/55   2/22/12 1:44p erickson
 * SW7425-2130: be consistent on #if, not #ifdef
 * 
 * Hydra_Software_Devel/54   2/22/12 12:16p erickson
 * SW7425-2130: fix last checkin
 * 
 * Hydra_Software_Devel/53   2/22/12 12:01p erickson
 * SW7425-2130: fix B_REFSW_DEBUG=n mode
 * 
 * Hydra_Software_Devel/52   2/21/12 1:59p erickson
 * SW7425-2130: add new DBG options for compact debug mode
 * 
 * Hydra_Software_Devel/51   9/2/11 6:23p vsilyaev
 * SW7425-1103: Added BDBG_CWARNING
 * 
 * Hydra_Software_Devel/50   6/21/11 11:23a vsilyaev
 * SW7420-1640: Fixed defintion of BDBG_P_INSTANCE_PRINTMSG_PRIV
 * 
 * Hydra_Software_Devel/49   6/17/11 11:06a vsilyaev
 * SW7420-1640: Added missing BDBG_P_INSTANCE_PRINTMSG_PRIV
 * 
 * Hydra_Software_Devel/48   6/16/11 1:54p vsilyaev
 * SW7425-729: Added missing defines for LOG level
 * 
 * Hydra_Software_Devel/47   6/16/11 1:40p vsilyaev
 * SW7425-729,SW7400-3012: Added LOG level
 * 
 * Hydra_Software_Devel/46   6/16/11 11:12a vsilyaev
 * SW7420-1640: Streamlined Android
 * 
 * Hydra_Software_Devel/45   6/6/11 3:27p vsilyaev
 * SW7405-4477: Routed all debug output through buffer and use external
 * application to extract and print debug output
 * 
 * Hydra_Software_Devel/44   4/22/11 7:46p vsilyaev
 * SW7125-918, SW7405-4477: Fixed definitions of the multi module debug
 * output
 * 
 * Hydra_Software_Devel/43   4/20/11 10:10a ttrammel
 * SW7420-1640: Merge Android logging into main branch.
 * 
 * Hydra_Software_Devel/42   4/13/11 7:00p vsilyaev
 * SW7405-5221: Route BERR_TRACE through the debug log
 * 
 * Hydra_Software_Devel/41   1/13/11 3:40p erickson
 * SWDEPRECATED-2425: consolidate BDBG_OBJECT example code
 *
 * Hydra_Software_Devel/40   1/5/11 6:40p vsilyaev
 * SW7405-4477: Changed names to match ones used in the ticket
 *
 * Hydra_Software_Devel/39   1/5/11 6:30p vsilyaev
 * SW7405-4477: Added BDBG_XXX_EXT to support multiple modules in the same
 * compilation unit (C file)
 *
 * Hydra_Software_Devel/38   12/1/10 12:41p vsilyaev
 * SWDEPRECATED-1626: Stricter and safer BDBG_CASSERT
 *
 * Hydra_Software_Devel/37   8/12/10 6:19p ttrammel
 * SW7420-943: Fix NFE merge issue.
 *
 * Hydra_Software_Devel/36   8/12/10 4:39p ttrammel
 * SW7420-943: Merge NFE to main branch.
 *
 * Hydra_Software_Devel/35   7/7/10 11:48a vsilyaev
 * SW7420-867: Moved application level API into the bdbg_app.h
 *
 * Hydra_Software_Devel/34   6/25/10 6:26p jgarrett
 * SW7408-88: Removing implicit global from BDBG_OBJECT_DESTROY
 *
 * Hydra_Software_Devel/33   10/14/09 6:52p vsilyaev
 * SW7405-3211: Use constant pointers in  BDBG_Object_Assert
 *
 * Hydra_Software_Devel/32   6/24/09 2:25p mphillip
 * PR54926: Merge DBG extension to /main
 *
 * Hydra_Software_Devel/PR54926/1   5/18/09 6:57p vsilyaev
 * PR 54926: Added interface to capture debug output
 *
 * Hydra_Software_Devel/31   4/8/09 4:23p vsilyaev
 * PR 54015: Don't include bkni_multi.h into the bdbg.h. All thread-aware
 * modules should include explicitly bkni_multi.h
 *
 * Hydra_Software_Devel/30   2/19/09 6:45p vsilyaev
 * PR 52320: Added BDBG_RELEASE macro to unwind module from BDBG
 *
 * Hydra_Software_Devel/29   12/29/08 1:32p erickson
 * PR50746: make BDBG_OBJECT_ASSERT failures more helpful
 *
 * Hydra_Software_Devel/28   7/22/08 12:30p nilesh
 * PR44846: Added support for setting instance levels on a per module
 * basis
 *
 * Hydra_Software_Devel/27   8/15/07 2:25p vsilyaev
 * PR 34089: Optimized executuion speed and code size
 *
 * Hydra_Software_Devel/26   5/4/07 12:14p vsilyaev
 * PR 25469: Added function BDBG_EnumerateAll
 *
 * Hydra_Software_Devel/25   3/26/07 4:53p vsilyaev
 * PR 27425: Fixed use of arguments in the BDBG_OBJECT macros
 *
 * Hydra_Software_Devel/24   1/30/07 11:03a vsilyaev
 * PR 27425: Proofread documentation and expaned BDBG_OBJECT portion
 *
 * Hydra_Software_Devel/23   9/14/06 11:13a jgarrett
 * PR 23982: Adding BDBG_NOP to reduce release-mode warnings
 *
 * Hydra_Software_Devel/22   7/26/06 4:51p vsilyaev
 * PR 22695:Fixed issue with previous commit
 *
 * Hydra_Software_Devel/21   7/25/06 10:42a vsilyaev
 * PR 22695: Pulled the debug output routines into the release build
 *
 * Hydra_Software_Devel/20   7/21/06 11:28a vsilyaev
 * PR 22695: Changes in the BDBG module to make it run-time compatible
 * between debug and release builds
 *
 * Hydra_Software_Devel/19   5/1/06 5:16p vsilyaev
 * PR 21336: PR 21258: Added BDBG_CASSERT, modified BDBG_OBJECT_ID to
 * include semicolon
 *
 * Hydra_Software_Devel/18   4/27/06 10:06a vle
 * PR 21065: rework BDBG_LOCK and BDBG_UNLOCK to be OS specific
 * implementation.
 *
 * Hydra_Software_Devel/17   2/27/06 5:12p vsilyaev
 * PR 19917: Added API to check object types at runtime
 *
 * Hydra_Software_Devel/16   11/29/05 3:14p vle
 * PR18319: add timestamp to debug messages
 *
 * Hydra_Software_Devel/15   10/5/04 7:49p vsilyaev
 * PR 12726:  Fixed warnings generated in the Release build.
 *
 * Hydra_Software_Devel/14   6/17/04 3:20p erickson
 * PR11135: fixed type in release build of BDBG_GetModuleLevel
 *
 * Hydra_Software_Devel/13   12/29/03 11:24a vsilyaev
 * PR 9131: Added implementation note for the redirecting debug output
 * into the file.
 *
 * Hydra_Software_Devel/12   11/26/03 3:40p vsilyaev
 * When code compiled by GNU C, declare bdbg_module as unused variable
 * (fixed warning issue).
 *
 * Hydra_Software_Devel/11   11/13/03 1:53p vsilyaev
 * Added Trace level to use by BDBG_ENTER and BDBG_LEAVE.
 *
 * Hydra_Software_Devel/10   11/13/03 9:33a vsilyaev
 * Tag malloced and statically allocated memory, so only malloced will be
 * free'ed at UnInit time.
 *
 * Hydra_Software_Devel/9   11/12/03 2:22p vsilyaev
 * Use statically alllocated data for internal structures, it solves
 * dilemma of malloc and interrupt.
 *
 * Hydra_Software_Devel/8   8/19/03 10:37a vsilyaev
 * Use verbose initialiization for module to prevent warnings.
 *
 * Hydra_Software_Devel/7   4/1/03 9:42a vsilyaev
 * Fixed release version of the debug interface.
 *
 * Hydra_Software_Devel/6   3/20/03 12:43p erickson
 * removed dulicate BDBG_Instance decl
 *
 * Hydra_Software_Devel/5   3/12/03 6:33p vsilyaev
 * Added description for BDBG_ASSERT macro and module overview.
 *
 * Hydra_Software_Devel/4   3/12/03 4:39p vsilyaev
 * Added comments for the auto documentation tool.
 *
 * Hydra_Software_Devel/3   3/10/03 6:36p vsilyaev
 * Uses extern "C" { } brackets.
 *
 * Hydra_Software_Devel/2   3/10/03 10:43a vsilyaev
 * Integration with berr.h
 *
 * Hydra_Software_Devel/1   3/10/03 10:03a vsilyaev
 * O/S inspecific part of the debug interface.
 *
 ***************************************************************************/

#ifndef BDBG_H
#define BDBG_H

#if defined B_REFSW_ANDROID && !defined __KERNEL__
#define LOG_TAG "NEXUS"
#include <cutils/log.h>
#endif

#include "blst_slist.h"

#ifdef __cplusplus
extern "C" {
#endif

/*================== Module Overview =====================================
This Debug Interface provides console output according to pre-defined levels
with compile-time and run-time control.

Defined macros:
 o BDBG_MODULE - register module and allow using the rest of the BDBG_ macros.
 o BDBG_MSG, BDBG_WRN, BDBG_ERR - message output with 'Message', 'Warning' and 'Error' levels.
 o BDBG_ASSERT - evaluate expression (condition) and fail if it's false.

To activate debug build BDBG_DEBUG_BUILD macro should be defined to 1,
i.e. -DBDBG_DEBUG_BUILD=1 .

Implementation note:
During impementation of this interface for a given plaform, it should be
noted, that debug messages shall use a different output channel than a normal
output from the system. For example in Posix like systems it would be
a good choice to use stderr for the debug messages. In such a case the
following command could be used to redirect debug messages into a file:
  application 2>debug_file

A second feature of this debug interface is the BDBG_OBJECT code. These macros provide
a means to create a unique object type, add that object to a structure, and assert
when given a pointer to an object that should be of that type but isn't.  This is
analogous to the old unix "magic" values.  A structure element would be initialized
to this unique magic value at creation time and be checked each time that structure was
used to ensure that it is a valid type.  The element would be erased when the structure
was destroyed to ensure that the element wasn't used after it had been destroyed.

Defined macros:
 o BDBG_OBJECT_ID, BDBG_OBJECT_ID_DECLARE - register object and allow using the rest of the BDBG_OBJECT_ macros.
 o BDBG_OBJECT - allocate storage in a structure for the "magic" values.
 o BDBG_OBJECT_INIT, BDBG_OBJECT_DESTROY, BDBG_OBJECT_SET, BDBG_OBJECT_UNSET - update the object.
 o BDBG_OBJECT_ASSERT - assert when the object isn't the expected type.
========================================================================*/

/* Runtime debug level */
typedef enum {
  BDBG_P_eUnknown=0, /* special level that is used to indicate that file wasn't registered */
  BDBG_eTrace, 	 /* Trace level */
  BDBG_eMsg,     /* Message level */
  BDBG_eWrn,     /* Warning level */
  BDBG_eErr,     /* Error level */
  BDBG_eLog,     /* Log level */
  BDBG_P_eLastEntry
} BDBG_Level;

typedef void *BDBG_Instance;

/***************************************************************************
Summary:
	Initialize the debug interface before being used.

Description: StandardProgrammingEnvironment
	Initializes the debug interface. You must call this before making any
	other kernel interface call.

Input:
	<none>

Returns:
	BERR_SUCCESS - The debug interface successfully initialized.
****************************************************************************/
BERR_Code BDBG_Init(void);

/***************************************************************************
Summary:
	Releases resources allocated by the debug interface.

Description:
	Cleans up the debug interface interface. No debug interface calls can be made after this.

Input:
	<none>

Returns:
	<none>
****************************************************************************/
void BDBG_Uninit(void);

typedef struct BDBG_DebugModuleFile *BDBG_pDebugModuleFile;

#define BDBG_P_FILE_INITIALIZER(module) { BDBG_P_eUnknown, BDBG_P_eUnknown, false, {NULL}, #module, NULL}

typedef enum BDBG_ModulePrintKind  {
    BDBG_ModulePrintKind_eHeader, /* header only */
    BDBG_ModulePrintKind_eBody, /* body only */
    BDBG_ModulePrintKind_eHeaderAndBody, /* entire output - header and body */
    BDBG_ModulePrintKind_eString /* just a string */
} BDBG_ModulePrintKind;

typedef struct BDBG_DebugModuleFile BDBG_DebugModuleFile;

typedef void (*BDBG_DebugModule_Print)(
        BDBG_ModulePrintKind kind, /* type of the output */
        BDBG_Level level, /* level of the debug output */
        const BDBG_DebugModuleFile *module, /* pointer to the debug module */
        const char *fmt,  /* format */
        va_list ap /* variable list of arguments */
        );

struct BDBG_DebugModuleFile {
	int8_t level; /* current level for this module */
	uint8_t module_level; /* actual level for module */
	bool module_alloc; /* true if module was instantiated by API call, e.g. BDBG_SetModuleLevel */
    BLST_S_ENTRY(BDBG_DebugModuleFile) link; /* pointer that links all modules */
    const char *name; /* name of the module */
    BDBG_DebugModule_Print module_print;
};


struct bdbg_obj
{
	const char *bdbg_obj_id;
};


#if BDBG_DEBUG_BUILD

#if 0
/***************************************************************************
Summary:
    Registers software for the debug interface.

Description:
    This macro is used to register software module in the system. If module is used in several
    C files each file should be registered with the same module name. Every modules that uses
    BDBG_XXX macros should use BDBG_MODULE after '#include' statement.

Example:
    #include "bstd.h"
    BDBG_MODULE(some_module)

Input:
    <none>

Returns:
    <none>
****************************************************************************/
#define BDBG_MODULE(module)

/***************************************************************************
Summary:
    Registers software for the debug interface.

Description:
    This macro is used to register another software module in the same compilation unit. If module is used in several
    C files each file should be registered with the same module name. Every modules that uses
    BDBG_MODULE_XXX macros should use BDBG_FILE_MODULE after '#include' statement.

Example:
    #include "bstd.h"
    BDBG_FILE_MODULE(another_module)

Input:
    <none>

Returns:
    <none>
****************************************************************************/
#define BDBG_FILE_MODULE(module)

/***************************************************************************
Summary:
    Prints out message.

Description:
    If either module or system debug level is set to the 'Message' level this macro will print message
    on the debug output. Message can have a printf style format string but should not include an 'end
    of line' symbol ("\n") as one will be added.

Note:
    Text will not be displayed regardless of the error level on Release builds.

See Also:
    BDBG_SetLevel, BDBG_SetModuleLevel

Example:
    BDBG_MSG(("Counter value %d", counter));

Input:
    format - message to print

Returns:
    <none>
****************************************************************************/
#define BDBG_MSG(format)

/***************************************************************************
Summary:
    Prints out message.

Description:
    If either module or system debug level is set to the 'Message' level this macro will print message
    on the debug output. Message can have a printf style format string but should not include an 'end
    of line' symbol ("\n") as one will be added.

Note:
    Text will not be displayed regardless of the error level on Release builds.

See Also:
    BDBG_SetLevel, BDBG_SetModuleLevel

Example:
    BDBG_MODULE_MSG(module,("Counter value %d", counter));

Input:
    format - message to print

Returns:
    <none>
****************************************************************************/
#define BDBG_MODULE_MSG(format)

/***************************************************************************
Summary:
    Prints out message.

Description:
    If either module or system debug level is set to the 'Message' or 'Warning' level this macro will
    print the message on the debug output. Message can have a printf style format string but should
    not include an 'end of line' symbol ("\n") as one will be added.

Note:
    Text will not be displayed regardless of the error level on Release builds.

See Also:
    BDBG_SetLevel, BDBG_SetModuleLevel

Example:
    BDBG_WRN(("Counter value %d greater then %d", counter, 10));

Input:
    format - message to print

Returns:
    <none>
****************************************************************************/
#define BDBG_WRN(format)

/***************************************************************************
Summary:
    Prints out message.

Description:
    If either module or system debug level is set to the 'Message' or 'Warning' level this macro will
    print the message on the debug output. Message can have a printf style format string but should
    not include an 'end of line' symbol ("\n") as one will be added.

Note:
    Text will not be displayed regardless of the error level on Release builds.

See Also:
    BDBG_SetLevel, BDBG_SetModuleLevel

Example:
    BDBG_MODULE_WRN(module,("Counter value %d greater then %d", counter, 10));

Input:
    format - message to print

Returns:
    <none>
****************************************************************************/
#define BDBG_MODULE_WRN(format)



/***************************************************************************
Summary:
    Prints out message.

Description:
    This macro prints the message on the debug output. Message can have a printf style format string
    but should not include an 'end of line' symbol ("\n") as one will be added.

Note:
    Text will not be displayed regardless of the error level on Release builds.

Example:
    BDBG_ERR(("Counter value %d negative", counter));

Input:
    format - message to print

Returns:
    <none>
****************************************************************************/
#define BDBG_ERR(format)

/***************************************************************************
Summary:
    Prints out message.

Description:
    This macro prints the message on the debug output. Message can have a printf style format string
    but should not include an 'end of line' symbol ("\n") as one will be added.

Note:
    Text will not be displayed regardless of the error level on Release builds.

Example:
    BDBG_ERR(module,("Counter value %d negative", counter));

Input:
    format - message to print

Returns:
    <none>
****************************************************************************/
#define BDBG_MODULE_ERR(module,format)


/***************************************************************************
Summary:
    Test conditions.

Description:
    Evaluates expression and if result is false, prints out message including
    textual presentation of the expression, file name and line number where assert
    occured and then call function BKNI_Fail. The exact behaviour of fail is
    implementation specific. Usually it traps to the debugger, generate stack trace and/or core dump.

Note:
    Expression inside BDBG_ASSERT does not evaluated for release builds, therefore expressions
    in the BDBG_ASSERT macro shall be free of side effects.

See Also:
    BKNI_Fail, BDBG_CASSERT

Example:
    BDBG_ASSERT(counter>0);

Input:
    expr - the expression to evaluate

Returns:
    <none>
****************************************************************************/
#define BDBG_ASSERT(expr)


/***************************************************************************
Summary:
    Test conditions at compile time.

Description:
    Evaluates expression at compile time and if result is false, compile process terminates.
    The exact behaviour of compile error is implementation specific, usually it causes the compiler
    to generate an error about duplicate case values.

Note:
    Expression inside BDBG_CASSERT shall be reducable to the boolean constant at compile time
    in the BDBG_ASSER macro shall be free of side effects.

See Also:
    BKNI_ASSERT
    BDBG_CWARNING

Example:
    BDBG_CASSERT(sizeof(int)==4); // compilation fails if size of the 'int' type if different from 4

Input:
    expr - the expression to evaluate

Returns:
    <none>
****************************************************************************/
#define BDBG_CASSERT(expr)

/***************************************************************************
Summary:
    Test conditions at compile time.

Description:
    Evaluates expression at compile time and if result is false, compile process generates a warning.
    The exact behaviour of compile warning is implementation specific, usually it causes the compiler
    to generate an warning about division to zero.

Note:
    Expression inside BDBG_CASSERT shall be reducable to the boolean constant at compile time
    in the BDBG_ASSER macro shall be free of side effects.

See Also:
    BDBG_CASSERT

Example:
    BDBG_WARNING(sizeof(int)==4); // compilation fails if size of the 'int' type if different from 4

Input:
    expr - the expression to evaluate

Returns:
    <none>
****************************************************************************/
#define BDBG_CWARNING(expr)


/***************************************************************************
Summary:
    Prints out message.

Description:
    If either instance, module, or system debug level is set to the 'Message' level this macro will print
    the message on the debug output. The message can have a printf style format string but should not
    include and 'end of line' character ("\n") as one will be added.

See Also:
    BDBG_SetLevel, BDBG_SetModuleLevel, BDBG_SetInstanceLevel

Example:
    BDBG_INSTANCE_MSG(obj, ("Obj's counter value %d", obj->counter));

Input:
    instance - the module instance
    format - message to print

Returns:
    <none>
****************************************************************************/
#define BDBG_INSTANCE_MSG(instance, format)


/***************************************************************************
Summary:
    Prints out message.

Description:
    If either instance, module, or system debug level is set to the 'Message' or 'Warning' level this macro
    will print the message on the debug output. The message can have a printf style format string but should
    not include an 'end of line' character ("\n") as one will be added.

See Also:
    BDBG_SetLevel, BDBG_SetModuleLevel, BDBG_SetInstanceLevel

Example:
    BDBG_INSTANCE_WRN(obj, ("Counter value %d greater then %d", obj->counter, 10));

Input:
    instance - the module instance
    format - message to print

Returns:
    <none>
****************************************************************************/
#define BDBG_INSTANCE_WRN(instance, format)


/***************************************************************************
Summary:
    Prints out message.

Description:
    This macro prints the message on the debug output. Message can have a printf style format string
    but should not include an 'end of line' character ("\n") as one will be added.

Example:
    BDBG_INSTANCE_ERR(obj, ("Counter value %d negative", obj->counter));

Input:
    instance - the module instance
    format - message to print

Returns:
    <none>
****************************************************************************/
#define BDBG_INSTANCE_ERR(instance, format)


/***************************************************************************
Summary:
    Prints out message.

Description:
    If either module or system debug level is set to the 'Trace' level this macro will print the
    message that function 'function' entered.

See Also:
    BDBG_SetLevel, BDBG_SetModuleLevel

Example:
    BDBG_ENTER(BFOO_DoBar);

Input:
    function - function name

Returns:
    <none>
****************************************************************************/
#define BDBG_ENTER(function)


/***************************************************************************
Summary:
    Prints out message.

Description:
    If either module or system debug level is set to the 'Trace' level this macro will print the
    message that function 'function' exited.

See Also:
    BDBG_SetLevel, BDBG_SetModuleLevel

Example:
    BDBG_LEAVE(BFOO_DoBar);

Input:
    function - function name

Returns:
    <none>
****************************************************************************/
#define BDBG_LEAVE(function)


/***************************************************************************
Summary:
    Registers instance

Description:
    This macro registers an intance for the debug interface. An 'Instance' is any pointer like object.
    Only registered instances should be passed to other debug interface API.

See Also:
    BDBG_SetInstanceLevel, BDBG_INSTANCE_MSG, BDBG_INSTANCE_WRN, BDBG_INSTANCE_ERR

Example:
    BDBG_REGISTER_INSTANCE(obj);

Input:
    instance - the module instance

Returns:
    BERR_SUCCESS - instance has been registered
****************************************************************************/
#define BDBG_REGISTER_INSTANCE(instance)


/***************************************************************************
Summary:
    Unregisters instance

Description:
    This macro releases resources allocated for this instance.

See Also:
    BDBG_REGISTER_INSTANCE

Example:
    BDBG_UNREGISTER_INSTANCE(obj);

Input:
    instance - the module instance

Returns:
    BERR_SUCCESS - instance has been unregistered
****************************************************************************/
#define BDBG_UNREGISTER_INSTANCE(instance)

/***************************************************************************
Summary:
    Releases resources

Description:
    This macro releases resources allocated for the file(module) which used this macro.

Example:
    BDBG_RELEASE();

****************************************************************************/
#define BDBG_RELEASE()


/***************************************************************************
Summary:
    Creates object type id

Description:
    This macro creates the object type id, that to use to check the object type.
    This macro shall be used in the 'C' file.
    Only one id with the given object type (name) can exist in the system.

Example:
    // this shows a non-opaque struct object_type.
    // if struct object_type is private (e.g. opaque handle), then all this code should be in the implementation file.

    // header file
    BDBG_OBJECT_ID_DECLARE(object_type);
    struct object_type {
        BDBG_OBJECT(object_type)
        unsigned x;
    }

    // implementation file
    BDBG_OBJECT_ID(object_type);

    object_type *open_object() {
        object_type *obj = malloc(sizeof(*obj));
        memset(obj, 0, sizeof(*obj));
        BDBG_OBJECT_SET(obj, object_type);
        obj->x = 1;
        return obj;
    }
    void use_object(object_type *obj) {
        BDBG_OBJECT_ASSERT(obj, object_type);
        obj->x = 2;
    }
    void close_object(object_type *obj) {
        BDBG_OBJECT_ASSERT(obj, object_type);
        obj->x = 3;
        BDBG_OBJECT_DESTROY(obj, object_type);
        free(obj);
    }

See Also:
    BDBG_OBJECT_INIT, BDBG_OBJECT_ASSERT

Input:
    name - object type

Returns:
    <none>
****************************************************************************/
#define BDBG_OBJECT_ID(name)


/***************************************************************************
Summary:
    Declares object type id

Description:
    This macro forward declares the object type id.
    Usually this is used in an H file if its necessary to share the same object ID
    across multiple C files.

Example:
    see example for BDBG_OBJECT_ID

See Also:
    BDBG_OBJECT_ID, BDBG_OBJECT_INIT, BDBG_OBJECT_ASSERT

Input:
    name - object type

Returns:
    <none>
****************************************************************************/
#define BDBG_OBJECT_ID_DECLARE(name)


/***************************************************************************
Summary:
    Adds module id to structure

Description:
    This macro is used to add module id to a structure.
    This needs to be initialized or set before it can be used in an assert test.

Note:
    A semi-colon should not be used with this macro as some compilers will generate
    an error when creating a release build and the macro is removed.

Example:
    see example for BDBG_OBJECT_ID

See Also:
    BDBG_OBJECT_ID, BDBG_OBJECT_INIT, BDBG_OBJECT_SET, BDBG_OBJECT_ASSERT

Input:
    name - object type

Returns:
    <none>
****************************************************************************/
#define BDBG_OBJECT(name)


/***************************************************************************
Summary:
    Initializes object

Description:
    This macro is used to initialize pointer and sets the module id.
    Initialization code fills memory, equal to size of the object, with a random pattern.

Example:
    see example for BDBG_OBJECT_ID. compare with:

    object_type *open_object() {
        object_type *obj = malloc(sizeof(*obj));
        BDBG_OBJECT_INIT(obj, object_type);
        obj->x = 1;
        return obj;
    }

See Also:
    BDBG_OBJECT_ID, BDBG_OBJECT_DESTROY, BDBG_OBJECT_ASSERT

Input:
    ptr - pointer to the object
    name - object type

Returns:
    <none>
****************************************************************************/
#define BDBG_OBJECT_INIT(ptr,name)


/***************************************************************************
Summary:
    Destroys object

Description:
    This macro is used to destroy object and remove the type tag. Using this
    macro fills memory, equal to size of the object, with a random pattern.
    The assert test will fail if pointer to object is used after its destroyed.

Example:
    see example for BDBG_OBJECT_ID

See Also:
    BDBG_OBJECT_ID, BDBG_OBJECT_INIT, BDBG_OBJECT_ASSERT

Input:
    ptr - pointer to the object
    name - object type

Returns:
    <none>
****************************************************************************/
#define BDBG_OBJECT_DESTROY(ptr,name)


/***************************************************************************
Summary:
    Sets the object tag

Description:
    This macro is used tag pointer with a specific tag.

Example:
    see example for BDBG_OBJECT_ID

See Also:
    BDBG_OBJECT_ID, BDBG_OBJECT_UNSET, BDBG_OBJECT_ASSERT

Input:
    ptr - pointer to the object
    name - object type

Returns:
    <none>
****************************************************************************/
#define BDBG_OBJECT_SET(ptr,name)


/***************************************************************************
Summary:
    Removes the object tag

Description:
    This macro is used remove tag from the pointer.

Example:
    see example for BDBG_OBJECT_ID
    BDBG_OBJECT_UNSET is an alternative to BDBG_OBJECT_DESTROY.

    BDBG_OBJECT_ID(object_type);
    struct some *p = malloc(sizeof(struct some));
    BDBG_OBJECT_UNSET(p, object_type);

See Also:
    BDBG_OBJECT_ID, BDBG_OBJECT_INIT, BDBG_OBJECT_SET, BDBG_OBJECT_ASSERT

Input:
    ptr - pointer to the object
    name - object type

Returns:
    <none>
****************************************************************************/
#define BDBG_OBJECT_UNSET(ptr,name)


/***************************************************************************
Summary:
    Validates a pointer and type of the pointer

Description:
    This macro is used validates that pointer is tagged with a specific object type.
    The pointer must be valid in order for this macro to succeed. If pointer doesn't
    match the expected type, this macro will cause a software assert.

Example:
    see example for BDBG_OBJECT_ID

See Also:
    BDBG_OBJECT_ID, BDBG_OBJECT_INIT, BDBG_OBJECT_SET

Input:
    ptr - pointer to the object
    name - object type

Returns:
    <none>
****************************************************************************/
#define BDBG_OBJECT_ASSERT(ptr,name)


#define BDBG_OBJECT_INIT_INST(ptr,name,inst)
#define BDBG_OBJECT_SET_INST(ptr,name,inst)
#define BDBG_OBJECT_TEST_INST(ptr,name,inst)

#endif /* 0 */


#ifdef __GNUC__
#define BDBG_MODULE(module) static BDBG_DebugModuleFile __attribute__ ((__unused__)) b_dbg_module = BDBG_P_FILE_INITIALIZER(module)
#define BDBG_FILE_MODULE(module) static BDBG_DebugModuleFile __attribute__ ((__unused__)) b_dbg_module_##module = BDBG_P_FILE_INITIALIZER(module)
#else
#define BDBG_MODULE(module) static BDBG_DebugModuleFile b_dbg_module = BDBG_P_FILE_INITIALIZER(module)
#define BDBG_FILE_MODULE(module) static BDBG_DebugModuleFile b_dbg_module_##module = BDBG_P_FILE_INITIALIZER(module)
#endif

#if defined(__STDC_VERSION__)
# if __STDC_VERSION__ == 199901L
#  define BDBG_P_UNWRAP(...) __VA_ARGS__
# endif
#endif
#if !defined(BDBG_P_UNWRAP) && defined(__KERNEL__)
# define BDBG_P_UNWRAP(args...) args
#endif


#if defined B_REFSW_ANDROID && !defined __KERNEL__
# define BDBG_P_PRINTMSG_PRIV(module, lvl, fmt) do { if(lvl==BDBG_eErr) {LOGE fmt;} else if(lvl==BDBG_eWrn) {LOGW fmt;} else {LOGV fmt;} } while(0)
# if defined(BDBG_P_UNWRAP)
#  define BDBG_P_INSTANCE_PRINTMSG_PRIV(module, lvl, instance, fmt) (void)BDBG_P_InstTestAndPrint((lvl), &module, (instance), BDBG_P_UNWRAP fmt)
# else 
#  define BDBG_P_INSTANCE_PRINTMSG_PRIV(module, lvl, instance, fmt) ((BDBG_P_InstTestAndPrint((lvl), &module, (instance), NULL))? BDBG_P_PrintWithNewLine fmt:(void)0)
# endif
#elif defined(BDBG_P_UNWRAP)
# define BDBG_P_PRINTMSG_PRIV(module,lvl, fmt) (((lvl) >= module.level)? (void)BDBG_P_TestAndPrint((lvl), &module, BDBG_P_UNWRAP fmt) : (void)0)
# define BDBG_P_INSTANCE_PRINTMSG_PRIV(module, lvl, instance, fmt) (void)BDBG_P_InstTestAndPrint((lvl), &module, (instance), BDBG_P_UNWRAP fmt)
#else
# define BDBG_P_PRINTMSG_PRIV(module, lvl, fmt) ((((lvl) >= module.level) && BDBG_P_TestAndPrint((lvl), &module, NULL)) ? BDBG_P_PrintWithNewLine fmt:(void)0)
# define BDBG_P_INSTANCE_PRINTMSG_PRIV(module, lvl, instance, fmt) ((BDBG_P_InstTestAndPrint((lvl), &module, (instance), NULL))? BDBG_P_PrintWithNewLine fmt:(void)0)
#endif

#define BDBG_P_PRINTMSG(lvl, fmt) BDBG_P_PRINTMSG_PRIV(b_dbg_module, lvl, fmt)
#define BDBG_P_MODULE_PRINTMSG(module, lvl, fmt) BDBG_P_PRINTMSG_PRIV(b_dbg_module_##module, lvl, fmt)
#define BDBG_P_INSTANCE_PRINTMSG(lvl, instance, fmt) BDBG_P_INSTANCE_PRINTMSG_PRIV(b_dbg_module, lvl, instance, fmt)
#define BDBG_P_MODULE_INSTANCE_PRINTMSG(module, lvl, instance, fmt) BDBG_P_INSTANCE_PRINTMSG_PRIV(b_dbg_module_##module, lvl, instance, fmt)

#define BDBG_REGISTER_INSTANCE(handle) BDBG_P_RegisterInstance(handle, &b_dbg_module)
#define BDBG_UNREGISTER_INSTANCE(handle) BDBG_P_UnRegisterInstance(handle, &b_dbg_module)
#define BDBG_RELEASE() BDBG_P_Release(&b_dbg_module)

#define BDBG_MODULE_REGISTER_INSTANCE(module, handle) BDBG_P_RegisterInstance(handle, &b_dbg_module_##module)
#define BDBG_MODULE_UNREGISTER_INSTANCE(module, handle) BDBG_P_UnRegisterInstance(handle, &b_dbg_module_##module)
#define BDBG_MODULE_RELEASE(module) BDBG_P_Release(&b_dbg_module__#module)

#if B_REFSW_DEBUG_COMPACT_ASSERT
#define BDBG_ASSERT(expr) (expr) ? (void) 0 : BDBG_P_AssertFailed(NULL, __FILE__, __LINE__)
#else
#define BDBG_ASSERT(expr) (expr) ? (void) 0 : BDBG_P_AssertFailed(#expr, __FILE__, __LINE__)
#endif
#define BDBG_CASSERT(expr) do switch(0){case 0: case (expr):;} while(0)
#define BDBG_CWARNING(expr) do {if(0){int unused = 1/(expr);unused++;}} while(0)


#define BDBG_OBJECT_ID(name) const char bdbg_id__##name[]= "#" #name
#define BDBG_OBJECT_ID_DECLARE(name) extern const char bdbg_id__##name[]
#define BDBG_OBJECT(name) struct bdbg_obj bdbg_object_##name;
#define BDBG_OBJECT_INIT(ptr,name) BDBG_Object_Init((ptr),sizeof(*(ptr)),&(ptr)->bdbg_object_##name, bdbg_id__##name)
#define BDBG_OBJECT_DESTROY(ptr,name) BDBG_Object_Init((ptr),sizeof(*(ptr)),&(ptr)->bdbg_object_##name, NULL)


#define BDBG_OBJECT_SET(ptr,name) (ptr)->bdbg_object_##name.bdbg_obj_id=bdbg_id__##name
#define BDBG_OBJECT_UNSET(ptr,name) (ptr)->bdbg_object_##name.bdbg_obj_id=NULL

void BDBG_Object_Assert(const void *ptr, size_t size, const struct bdbg_obj *obj, const char *id, const char *file, unsigned line);
#define BDBG_OBJECT_ASSERT(ptr,name) (((ptr) && (ptr)->bdbg_object_##name.bdbg_obj_id==bdbg_id__##name)? (void) 0 : BDBG_Object_Assert(ptr, sizeof(*ptr), &(ptr)->bdbg_object_##name, bdbg_id__##name, __FILE__, __LINE__))

#define BDBG_OBJECT_INIT_INST(ptr,name,inst) BDBG_Object_Init(ptr,sizeof(*(ptr)),&(ptr)->bdbg_object_##name,bdbg_id__##name+(unsigned)(inst))
#define BDBG_OBJECT_SET_INST(ptr,name,inst) (ptr)->bdbg_object_##name.bdbg_obj_id=(bdbg_id__##name + (unsigned)(inst))
#define BDBG_OBJECT_ASSERT_INST(ptr,name,inst) BDBG_ASSERT((ptr)->bdbg_object_##name.bdbg_obj_id==&bdbg_id__##name+(unsigned)(inst))

void BDBG_Object_Init(void *ptr, size_t size, struct bdbg_obj *obj, const char *id);

bool BDBG_P_TestAndPrint(BDBG_Level level, BDBG_pDebugModuleFile dbg_module, const char *fmt, ...);
bool BDBG_P_InstTestAndPrint(BDBG_Level level, BDBG_pDebugModuleFile dbg_module, BDBG_Instance handle, const char *fmt, ...);
void BDBG_P_RegisterInstance(BDBG_Instance handle, BDBG_pDebugModuleFile dbg_module);
void BDBG_P_UnRegisterInstance(BDBG_Instance handle, BDBG_pDebugModuleFile dbg_module);
void BDBG_P_PrintWithNewLine(const char *fmt, ...);
void BDBG_P_Release(BDBG_pDebugModuleFile dbg_module);

void BDBG_EnterFunction(BDBG_pDebugModuleFile dbg_module, const char *function);
void BDBG_LeaveFunction(BDBG_pDebugModuleFile dbg_module, const char *function);


#else /* BDBG_DEBUG_BUILD */
/* stubs */

#define BDBG_MODULE(module) extern int bdbg_unused
#define BDBG_FILE_MODULE(module) extern int bdbg_unused

#define BDBG_RELEASE() BDBG_NOP()
#define BDBG_MODULE_RELEASE(module) BDBG_NOP()

#define BDBG_REGISTER_INSTANCE(instance)
#define BDBG_MODULE_REGISTER_INSTANCE(module,instance)
#define BDBG_UNREGISTER_INSTANCE(instance)
#define BDBG_MODULE_UNREGISTER_INSTANCE(module,instance)

#define BDBG_ASSERT(expr) BDBG_NOP()
#define BDBG_CASSERT(expr)
#define BDBG_CWARNING(expr) 


#define BDBG_OBJECT_ID(name) extern const char bdbg_id_unused_##name
#define BDBG_OBJECT_ID_DECLARE(name) extern const char bdbg_id_unused_decl_##name
#define BDBG_OBJECT(name)
#define BDBG_OBJECT_INIT(ptr,name) (void)ptr
#define BDBG_OBJECT_DESTROY(ptr,name) (void)ptr
#define BDBG_OBJECT_SET(ptr,name) (void)ptr
#define BDBG_OBJECT_UNSET(ptr,name) (void)ptr
#define BDBG_OBJECT_ASSERT(ptr,name)  (void)ptr
#define BDBG_OBJECT_INIT_INST(ptr,name,inst) (void)ptr
#define BDBG_OBJECT_SET_INST(ptr,name,inst) (void)ptr
#define BDBG_OBJECT_TEST_INST(ptr,name,inst) (void)ptr
#define BDBG_OBJECT_ASSERT_INST(ptr,name,inst) (void)ptr

/* if not BDBG_DEBUG_BUILD, force these macros to be defined. 
they can be used to avoid unused code warnings. */
#ifndef BDBG_NO_MSG
#define BDBG_NO_MSG 1
#endif
#ifndef BDBG_NO_WRN
#define BDBG_NO_WRN 1
#endif
#ifndef BDBG_NO_LOG
#define BDBG_NO_LOG 1
#endif
#endif /* BDBG_DEBUG_BUILD */

#if BDBG_NO_MSG
#define BDBG_ENTER(function) BDBG_NOP()
#define BDBG_MODULE_ENTER(module,function) BDBG_NOP()
#define BDBG_LEAVE(function) BDBG_NOP()
#define BDBG_MODULE_LEAVE(module,function) BDBG_NOP()
#define BDBG_MSG(format) BDBG_NOP()
#define BDBG_MODULE_MSG(module, format) BDBG_NOP()
#define BDBG_INSTANCE_MSG(instance, format) BDBG_NOP()
#define BDBG_MODULE_INSTANCE_MSG(module, instance, format) BDBG_NOP()
#else
#define BDBG_ENTER(function) ((BDBG_eTrace >= b_dbg_module.level)? BDBG_EnterFunction(&b_dbg_module, #function)  : (void)0)
#define BDBG_MODULE_ENTER(module, function) ((BDBG_eTrace >= b_dbg_module_##module.level)? BDBG_EnterFunction(&b_dbg_module_##module, #function)  : (void)0)
#define BDBG_LEAVE(function) ((BDBG_eTrace >= b_dbg_module.level)? BDBG_LeaveFunction(&b_dbg_module, #function)  : (void)0)
#define BDBG_MODULE_LEAVE(module, function) ((BDBG_eTrace >= b_dbg_module_##module.level)? BDBG_LeaveFunction(&b_dbg_module_##module, #function)  : (void)0)
#define BDBG_MSG(format) BDBG_P_PRINTMSG(BDBG_eMsg, format)
#define BDBG_MODULE_MSG(module, format) BDBG_P_MODULE_PRINTMSG(module, BDBG_eMsg, format)
#define BDBG_INSTANCE_MSG(instance, format) BDBG_P_INSTANCE_PRINTMSG(BDBG_eMsg, instance, format)
#define BDBG_MODULE_INSTANCE_MSG(module, instance, format) BDBG_P_MODULE_INSTANCE_PRINTMSG(module, BDBG_eMsg, instance, format)
#endif

#if BDBG_NO_WRN
#define BDBG_WRN(format) BDBG_NOP()
#define BDBG_MODULE_WRN(module,format) BDBG_NOP()
#define BDBG_INSTANCE_WRN(instance, format) BDBG_NOP()
#define BDBG_MODULE_INSTANCE_WRN(module,instance, format) BDBG_NOP()
#else
#define BDBG_WRN(format) BDBG_P_PRINTMSG(BDBG_eWrn, format)
#define BDBG_MODULE_WRN(module,format) BDBG_P_MODULE_PRINTMSG(module, BDBG_eWrn, format)
#define BDBG_INSTANCE_WRN(instance, format) BDBG_P_INSTANCE_PRINTMSG(BDBG_eWrn, instance, format)
#define BDBG_MODULE_INSTANCE_WRN(module, instance, format) BDBG_P_MODULE_INSTANCE_PRINTMSG(module, BDBG_eWrn, instance, format)
#endif

#if BDBG_DEBUG_BUILD
#if B_REFSW_DEBUG_COMPACT_ERR
#define BDBG_ERR(format)                                   BDBG_P_PrintString("### Error at %s:%d\n", __FILE__, __LINE__)
#define BDBG_MODULE_ERR(module,format)                     BDBG_ERR(format)
#define BDBG_INSTANCE_ERR(instance, format)                BDBG_ERR(format)
#define BDBG_MODULE_INSTANCE_ERR(module, instance, format) BDBG_ERR(format)
#else
#define BDBG_ERR(format) BDBG_P_PRINTMSG(BDBG_eErr, format)
#define BDBG_MODULE_ERR(module,format) BDBG_P_MODULE_PRINTMSG(module,BDBG_eErr, format)
#define BDBG_INSTANCE_ERR(instance, format) BDBG_P_INSTANCE_PRINTMSG(BDBG_eErr, instance, format)
#define BDBG_MODULE_INSTANCE_ERR(module, instance, format) BDBG_P_MODULE_INSTANCE_PRINTMSG(module,BDBG_eErr, instance, format)
#endif
#else
#define BDBG_ERR(format) BDBG_NOP()
#define BDBG_MODULE_ERR(module,format) BDBG_NOP()
#define BDBG_INSTANCE_ERR(instance, format) BDBG_NOP()
#define BDBG_MODULE_INSTANCE_ERR(module, instance, format) BDBG_NOP()
#endif

#if BDBG_NO_LOG
#define BDBG_LOG(format) BDBG_NOP()
#define BDBG_MODULE_LOG(module,format) BDBG_NOP()
#define BDBG_INSTANCE_LOG(instance, format) BDBG_NOP()
#define BDBG_MODULE_INSTANCE_LOG(module, instance, format) BDBG_NOP()
#else
#define BDBG_LOG(format) BDBG_P_PRINTMSG(BDBG_eLog, format)
#define BDBG_MODULE_LOG(module,format) BDBG_P_MODULE_PRINTMSG(module,BDBG_eLog, format)
#define BDBG_INSTANCE_LOG(instance, format) BDBG_P_INSTANCE_PRINTMSG(BDBG_eLog, instance, format)
#define BDBG_MODULE_INSTANCE_LOG(module, instance, format) BDBG_P_MODULE_INSTANCE_PRINTMSG(module,BDBG_eLog, instance, format)
#endif

#ifdef __cplusplus
}
#endif
#include  "bdbg_app.h"
#include  "bdbg_priv.h"

#endif  /* BDBG_H */

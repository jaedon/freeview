/*
 * ResidentProgramClass.c
 */

#include <stdarg.h>
#if !defined(WIN32)
#include <stdbool.h>
#endif
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#include "MHEGEngine.h"
#include "ResidentProgramClass.h"
#include "RootClass.h"
#include "Reference.h"
#include "Generic.h"
#include "VariableClass.h"
#include "clone.h"
#include "si.h"

/* internal functions */
static bool run_program(ResidentProgramClass *, LIST_OF(Parameter) *, OctetString *, bool);
static bool check_parameters(LIST_OF(Parameter) *, unsigned int, ...);
static Parameter *get_parameter(LIST_OF(Parameter) *, unsigned int);
static char *parameter_name(Parameter *, OctetString *);

/* resident programs (UK MHEG Profile (D-BOOK 5.0) 에 정의되어 있는 것들) */
static bool prog_GetCurrentDate(LIST_OF(Parameter) *, OctetString *);
static bool prog_FormatDate(LIST_OF(Parameter) *, OctetString *);
static bool prog_GetDayOfWeek(LIST_OF(Parameter) *, OctetString *);
static bool prog_Random(LIST_OF(Parameter) *, OctetString *);
static bool prog_CastToContentRef(LIST_OF(Parameter) *, OctetString *);
static bool prog_CastToObjectRef(LIST_OF(Parameter) *, OctetString *);
static bool prog_GetStringLength(LIST_OF(Parameter) *, OctetString *);
static bool prog_GetSubString(LIST_OF(Parameter) *, OctetString *);
static bool prog_SearchSubString(LIST_OF(Parameter) *, OctetString *);
static bool prog_SearchAndExtractSubString(LIST_OF(Parameter) *, OctetString *);
static bool prog_SI_GetServiceIndex(LIST_OF(Parameter) *, OctetString *);
static bool prog_SI_TuneIndex(LIST_OF(Parameter) *, OctetString *); 					/* CI+ MHEG Profile 에선 필요치 않다 : CI+ 에서 서비스 튜닝은 CI HCI (Host Control Interface Resource) 를 이용 */
static bool prog_SI_TuneIndexInfo(LIST_OF(Parameter) *, OctetString *); 			/* CI+ MHEG Profile 에선 필요치 않다 */
static bool prog_SI_GetBasicSI(LIST_OF(Parameter) *, OctetString *);
static bool prog_GetBootInfo(LIST_OF(Parameter) *, OctetString *); 					/* CI+ MHEG Profile 에선 필요치 않다 */
static bool prog_CheckContentRef(LIST_OF(Parameter) *, OctetString *);
static bool prog_CheckGroupIDRef(LIST_OF(Parameter) *, OctetString *);
static bool prog_VideoToGraphics(LIST_OF(Parameter) *, OctetString *); 				/* CI+ MHEG Profile 에선 필요치 않다 */
static bool prog_SetWidescreenAlignment(LIST_OF(Parameter) *, OctetString *);	/* CI+ MHEG Profile 에선 필요치 않다 */
static bool prog_GetDisplayAspectRatio(LIST_OF(Parameter) *, OctetString *);
static bool prog_CI_SendMessage(LIST_OF(Parameter) *, OctetString *);
static bool prog_SetSubtitleMode(LIST_OF(Parameter) *, OctetString *); 				/* CI+ MHEG Profile 에선 필요치 않다 */
static bool prog_WhoAmI(LIST_OF(Parameter) *, OctetString *);
static bool prog_Debug(LIST_OF(Parameter) *, OctetString *);

/* resident programs (UK MHEG Profile (D-BOOK 5.0) 에 정의되지 않은 것들) */
static bool prog_RequestMPEGDecoder(LIST_OF(Parameter) *, OctetString *);		/* S&T, RequestMPEGDecoder 예제 테스트를 위해 추가함 (CI+ MHEG Profile 추가 사항) */
static bool prog_TestInputMask(LIST_OF(Parameter) *params, OctetString *caller_gid); /* STestInputMask (CI+ 1.3 MHEG Profile 추가 사항)  */
static bool prog_SuppressMHEGGraphics(LIST_OF(Parameter) *params, OctetString *caller_gid); /* SuppressMHEGGraphics (CI+ 1.3 MHEG Profile 추가 사항) */


void
ResidentProgramClass_Preparation(ResidentProgramClass *t)
{
#if (CLS_VERBOSE)
	verbose_class("ResidentProgramClass: %s; Preparation", ExternalReference_name(&t->rootClass.inst.ref));
#endif

	/* has it already been prepared */
	if(t->rootClass.inst.AvailabilityStatus)
		return;

	/* retrieve the object from an entity outside the engine and set each internal attribute of the object to its initial value */
	t->inst.forked = false;

	/* Preparation inherited from the RootClass */
	RootClass_Preparation(&t->rootClass);

	return;
}

void
ResidentProgramClass_Activation(ResidentProgramClass *t)
{
#if (CLS_VERBOSE)
	verbose_class("ResidentProgramClass: %s; Activation", ExternalReference_name(&t->rootClass.inst.ref));
#endif

	/* has it already been activated */
	if(t->rootClass.inst.RunningStatus)
		return;

#if 1 // hmkim : added.
	/* has it been prepared yet */
	if(!t->rootClass.inst.AvailabilityStatus)
	{
		/* generates an IsAvailable event */
		ResidentProgramClass_Preparation(t);
	}
#endif

	/* Activation inherited from the RootClass */
	RootClass_Activation(&t->rootClass);

#if 0 // hmkim : commented out.
	/* set its RunningStatus and generate IsRunning event */
	t->rootClass.inst.RunningStatus = true;
	MHEGEngine_generateEvent(&t->rootClass.inst.ref, EventType_is_running, NULL);
#endif

	return;
}

void
ResidentProgramClass_Deactivation(ResidentProgramClass *t)
{
#if (CLS_VERBOSE)
	verbose_class("ResidentProgramClass: %s; Deactivation", ExternalReference_name(&t->rootClass.inst.ref));
#endif

	/* is it already deactivated */
	if(!t->rootClass.inst.RunningStatus)
		return;

	/* stop any forked program */
	if(t->inst.forked)
	{
		verbose_todo("%s; stop forked program", __FUNCTION__);
		t->inst.forked = false;
	}

	/* Deactivation inherited from the RootClass */
	RootClass_Deactivation(&t->rootClass);

	return;
}

void
ResidentProgramClass_Destruction(ResidentProgramClass *t)
{
#if (CLS_VERBOSE)
	verbose_class("ResidentProgramClass: %s; Destruction", ExternalReference_name(&t->rootClass.inst.ref));
#endif

	/* is it already destroyed */
	if(!t->rootClass.inst.AvailabilityStatus)
		return;

	/* Deactivate it if it is running */
	if(t->rootClass.inst.RunningStatus)
	{
		/* generates an IsStopped event */
		ResidentProgramClass_Deactivation(t);
	}

#if 0 // org
	/* generate an IsDeleted event */
	t->rootClass.inst.AvailabilityStatus = false;
	MHEGEngine_generateEvent(&t->rootClass.inst.ref, EventType_is_deleted, NULL);
#else // hmkim : modified.
	/* Destruction inherited from the RootClass */
	RootClass_Destruction(&t->rootClass);
#endif

	return;
}

void
ResidentProgramClass_Clone(ResidentProgramClass *t, Clone *params, OctetString *caller_gid)
{
//	verbose_todo("%s not yet implemented", __FUNCTION__);

#if (CLS_VERBOSE)
	verbose_class("ResidentProgramClass: %s; Clone", ExternalReference_name(&t->rootClass.inst.ref));
#endif

	CLONE_OBJECT(t, ResidentProgramClass, resident_program);

	return;
}

void
ResidentProgramClass_Call(ResidentProgramClass *p, Call *params, OctetString *caller_gid)
{
	bool rc;
	VariableClass *var;

	/* has it been prepared yet */
	if(!p->rootClass.inst.AvailabilityStatus)
		ResidentProgramClass_Preparation(p);

	/* is it already active */
	if(p->rootClass.inst.RunningStatus)
		return;

#if (CLS_VERBOSE)
	verbose_class("ResidentProgramClass: %s; Call '%.*s'", ExternalReference_name(&p->rootClass.inst.ref), p->name.size, p->name.data);
#endif

	/* run it and wait for it to complete */
	ResidentProgramClass_Activation(p);
	rc = run_program(p, params->parameters, caller_gid, false);

	/* store the return value */
	if((var = (VariableClass *) MHEGEngine_findObjectReference(&params->call_succeeded, caller_gid)) != NULL)
	{
		if(var->rootClass.inst.rtti == RTTI_VariableClass
		&& VariableClass_type(var) == OriginalValue_boolean)
		{
#if (CLS_VERBOSE)
			verbose_class("Call: set %s", rc ? "true" : "false");
#endif
			BooleanVariableClass_setBoolean(var, rc);
		}
		else
		{
			error("Call: %s is not a BooleanVariableClass", ExternalReference_name(&var->rootClass.inst.ref));
		}
	}

	/* do Deactivation */
	ResidentProgramClass_Deactivation(p);

	return;
}

void
ResidentProgramClass_Fork(ResidentProgramClass *p, Fork *params, OctetString *caller_gid)
{
	bool rc;
	VariableClass *var;

	/* has it been prepared yet */
	if(!p->rootClass.inst.AvailabilityStatus)
		ResidentProgramClass_Preparation(p);

	/* is it already active */
	if(p->rootClass.inst.RunningStatus)
		return;

#if (CLS_VERBOSE)
	verbose_class("ResidentProgramClass: %s; Fork '%.*s'", ExternalReference_name(&p->rootClass.inst.ref), p->name.size, p->name.data);
#endif

	/* run it in the background */
/******************************************************************************************/
	verbose_todo("%s not yet implemented - running in foreground", __FUNCTION__);
	ResidentProgramClass_Activation(p);
	rc = run_program(p, params->parameters, caller_gid, true);
/* return immediately */
/******************************************************************************************/

/******************************************************************************************/
/* when the program ends */
/******************************************************************************************/
	/* store the return value */
	if((var = (VariableClass *) MHEGEngine_findObjectReference(&params->fork_succeeded, caller_gid)) != NULL)
	{
		if(var->rootClass.inst.rtti == RTTI_VariableClass
		&& VariableClass_type(var) == OriginalValue_boolean)
		{
#if (CLS_VERBOSE)
			verbose_class("Fork: set %s", rc ? "true" : "false");
#endif
			BooleanVariableClass_setBoolean(var, rc);
		}
		else
		{
			error("Fork: %s is not a BooleanVariableClass", ExternalReference_name(&var->rootClass.inst.ref));
		}
	}

	/* do Deactivation */
	ResidentProgramClass_Deactivation(p);

	MHEGEngine_generateAsyncEvent(&p->rootClass.inst.ref, EventType_asynch_stopped, NULL);

	return;
}

/*
 * run the given program (identified by the name field in the ResidentProgramClass)
 * returns true if the program succeeds
 * sets the parameters to the values described by the UK MHEG Profile
 * caller_gid is used to resolve Generic variables in the parameters
 * if forked is true, return immediately and run the program in the background
 */

static struct
{
	char *short_name;
	char *long_name;	/* This long form is no longer supported in UK MHEG Profile */
	bool (*func)(LIST_OF(Parameter) *, OctetString *);
} resident_progs[] =
{
	{ "GCD", "GetCurrentDate",				prog_GetCurrentDate },
	{ "FDa", "FormatDate",					prog_FormatDate },
	{ "GDW", "GetDayOfWeek",				prog_GetDayOfWeek },
	{ "Rnd", "Random",						prog_Random },
	{ "CTC", "CastToContentRef",			prog_CastToContentRef },
	{ "CTO", "CastToObjectRef",				prog_CastToObjectRef },
	{ "GSL", "GetStringLength",				prog_GetStringLength },
	{ "GSS", "GetSubString",				prog_GetSubString },
	{ "SSS", "SearchSubString",				prog_SearchSubString },
	{ "SES", "SearchAndExtractSubString",	prog_SearchAndExtractSubString },
	{ "GSI", "SI_GetServiceIndex",			prog_SI_GetServiceIndex },
	{ "TIn", "SI_TuneIndex",				prog_SI_TuneIndex },				/* Never Fork ! */ /* CI+ MHEG Profile 에선 필요치 않다 : CI+ 에서 서비스 튜닝은 CI HCI (Host Control Interface Resource) 를 이용 */
	{ "TII", "SI_TuneIndexInfo",			prog_SI_TuneIndexInfo },			/* CI+ MHEG Profile 에선 필요치 않다 */
	{ "BSI", "SI_GetBasicSI",				prog_SI_GetBasicSI },
	{ "GBI", "GetBootInfo",					prog_GetBootInfo },				/* CI+ MHEG Profile 에선 필요치 않다 */
	{ "CCR", "CheckContentRef",				prog_CheckContentRef },			/* Typical Use : Fork */
	{ "CGR", "CheckGroupIDRef",				prog_CheckGroupIDRef },			/* Typical Use : Fork */
	{ "VTG", "VideoToGraphics",				prog_VideoToGraphics },			/* CI+ MHEG Profile 에선 필요치 않다 */
	{ "SWA", "SetWidescreenAlignment",		prog_SetWidescreenAlignment },	/* CI+ MHEG Profile 에선 필요치 않다 */
	{ "GDA", "GetDisplayAspectRatio",		prog_GetDisplayAspectRatio },
	{ "CIS", "CI_SendMessage",				prog_CI_SendMessage },			/* This shall not be invoked with Fork */
	{ "SSM", "SetSubtitleMode",				prog_SetSubtitleMode },			/* CI+ MHEG Profile 에선 필요치 않다 */
	{ "WAI", "WhoAmI",						prog_WhoAmI },
	{ "DBG", "Debug",						prog_Debug },
	{ "RMD", "RequestMPEGDecoder",			prog_RequestMPEGDecoder },		/* S&T, RequestMPEGDecoder 예제 테스트를 위해 추가함 (CI+ MHEG Profile 추가 사항) */
	{ "TIM", "TestInputMask",				prog_TestInputMask },			/* TestInputMask (CI+ 1.3 MHEG Profile 추가 사항) */
	{ "SMG", "SuppressMHEGGraphics",		prog_SuppressMHEGGraphics },	/* SSuppressMHEGGraphics (CI+ MHEG Profile 추가 사항) */

	{ "", "", NULL }
};

static ResidentProgramClass resident_class; // hmkim : added for prog_CheckContentRef() and prog_CheckGroupIDRef().

static bool
run_program(ResidentProgramClass *p, LIST_OF(Parameter) *params, OctetString *caller_gid, bool forked)
{
	bool rc;
	unsigned int i;

	/* check "Never Fork" program */
	if(forked && (OctetString_strcmp(&p->name, "TIn") == 0 || OctetString_strcmp(&p->name, "SI_TuneIndex") == 0))
	{
		error("ResidentProgram: SI_TuneIndex (TIn) shall not be invoked with Fork");
		return false;
	}

	/* remember if we were forked or not */
	p->inst.forked = forked;

	/* find it */
	for(i=0; resident_progs[i].func != NULL; i++)
	{
		if(OctetString_strcmp(&p->name, resident_progs[i].short_name) == 0
		|| OctetString_strcmp(&p->name, resident_progs[i].long_name) == 0)
			break;
	}

	/* run it */
	if(resident_progs[i].func)
	{
		VK_memcpy(&resident_class, p, sizeof(ResidentProgramClass)); // hmkim : added for prog_CheckContentRef() and prog_CheckGroupIDRef().
		rc = (*(resident_progs[i].func))(params, caller_gid);
	}
	else
	{
		error("Unknown ResidentProgram: '%.*s'", p->name.size, p->name.data);
		rc = false;
	}

	return rc;
}

/*
 * check the list of parameters is the correct length and match the given types
 * returns true if the parameters are valid
 */

static bool
check_parameters(LIST_OF(Parameter) *params, unsigned int nparams, ...)
{
	va_list ap;

	va_start(ap, nparams);

	/* check each param in the list matches the vararg types */
	while(nparams != 0)
	{
		if(params == NULL
		|| params->item.choice != va_arg(ap, unsigned int))
			return false;
		/* move on */
		params = params->next;
		nparams --;
	}

	va_end(ap);

	/* make sure no params are left */
	if(params != NULL)
		return false;

	return true;
}

/*
 * returns the given Parameter from the list
 * counting starts at 1
 */

static Parameter *
get_parameter(LIST_OF(Parameter) *params, unsigned int n)
{
	while(n > 1)
	{
		params = params->next;
		n --;
	}

	return &params->item;
}

/*
 * returns a static string that will be overwritten by the next call to this routine
 */

static char *_string = NULL;

static char *
parameter_name(Parameter *p, OctetString *caller_gid)
{
	OctetString *oct;
	ObjectReference *ref;

	switch(p->choice)
	{
	case Parameter_new_generic_boolean:
		_string = safe_realloc(_string, 16);
		snprintf(_string, 16, "%s", GenericBoolean_getBoolean(&p->u.new_generic_boolean, caller_gid) ? "true" : "false");
		return _string;

	case Parameter_new_generic_integer:
		_string = safe_realloc(_string, 64);
		snprintf(_string, 64, "%d", GenericInteger_getInteger(&p->u.new_generic_integer, caller_gid));
		return _string;

	case Parameter_new_generic_octetstring:
		oct = GenericOctetString_getOctetString(&p->u.new_generic_octetstring, caller_gid);
		_string = safe_realloc(_string, oct->size + 128);
		snprintf(_string, oct->size + 128, "%.*s", oct->size, oct->data);
		return _string;

	case Parameter_new_generic_object_reference:
		ref = GenericObjectReference_getObjectReference(&p->u.new_generic_object_reference, caller_gid);
		_string = safe_realloc(_string, PATH_MAX + 32);
		#if 1
		switch(ref->choice)
		{
		case ObjectReference_internal_reference:
			snprintf(_string, PATH_MAX + 32, "%.*s %u", caller_gid->size, caller_gid->data, ref->u.internal_reference);
			break;
		case ObjectReference_external_reference:
			snprintf(_string, PATH_MAX + 32, "%s", ExternalReference_name(&ref->u.external_reference));
			break;
		default:
			error("Unknown ObjectReference type: %d", ref->choice);
			safe_free(_string); _string = NULL;
			break;
		}
		#else // or simply... but...
		snprintf(_string, PATH_MAX + 32, "%s", ObjectReference_name(ref));
		#endif
		return _string;

	case Parameter_new_generic_content_reference:
		oct = GenericContentReference_getContentReference(&p->u.new_generic_content_reference, caller_gid);
		_string = safe_realloc(_string, oct->size + 128);
		snprintf(_string, oct->size + 128, "%.*s", oct->size, oct->data);
		return _string;

	default:
		error("Unknown Parameter type: %d", p->choice);
		safe_free(_string); _string = NULL;
		return _string;
	}
}

/*
 * resident programs
 */

/* number of days between 17/11/1858 (UK MPEG epoch) and 1/1/1970 (UNIX epoch) */
#define MHEG_EPOCH_OFFSET	40587

static bool
prog_GetCurrentDate(LIST_OF(Parameter) *params, OctetString *caller_gid)
{
	GenericInteger *date_par;
	GenericInteger *time_par;
	unsigned int mheg_date;
	unsigned int mheg_time;
	unsigned int unix_time;
	struct timeval now;
	struct timezone zone;

	if(!check_parameters(params, 2, Parameter_new_generic_integer,	/* out: date */
					Parameter_new_generic_integer))	/* out: time */
	{
		error("ResidentProgram: GetCurrentDate (GCD): wrong number or type of parameters");
		return false;
	}

	date_par = &(get_parameter(params, 1)->u.new_generic_integer);
	time_par = &(get_parameter(params, 2)->u.new_generic_integer);

	/* need to return local time, so take timezone into account */
	gettimeofday(&now, &zone);
	unix_time = now.tv_sec - (zone.tz_minuteswest * 60);

	/* number of days since 00:00:00 1/1/1970 */
	mheg_date = unix_time / (60 * 60 * 24);
	/* number of days since 00:00:00 17/11/1858 */
	mheg_date += MHEG_EPOCH_OFFSET;

	/* number of seconds since 00:00:00 */
	mheg_time = unix_time % (60 * 60 * 24);

	GenericInteger_setInteger(date_par, caller_gid, mheg_date);
	GenericInteger_setInteger(time_par, caller_gid, mheg_time);

#if (CLS_VERBOSE)
	verbose_class("ResidentProgram: GetCurrentDate(%u, %u)", mheg_date, mheg_time);
#endif

	return true;
}

static bool
prog_FormatDate(LIST_OF(Parameter) *params, OctetString *caller_gid)
{
	GenericOctetString *dateFormat_par;
	GenericInteger *date_par;
	GenericInteger *time_par;
	GenericOctetString *dateString_par;
	OctetString *dateFormat;
	unsigned int mheg_date;
	unsigned int mheg_time;
	OctetString dateString;
	unsigned int npc;
	unsigned int i;
	time_t unix_time;
	struct tm *tm;
	unsigned int year, month, hour;

	if(!check_parameters(params, 4, Parameter_new_generic_octetstring,	/* in: dateFormat */
					Parameter_new_generic_integer,		/* in: date */
					Parameter_new_generic_integer,		/* in: time */
					Parameter_new_generic_octetstring))	/* out: dateString */
	{
		error("ResidentProgram: FormatDate (FDa): wrong number or type of parameters");
		return false;
	}

	dateFormat_par = &(get_parameter(params, 1)->u.new_generic_octetstring);
	date_par = &(get_parameter(params, 2)->u.new_generic_integer);
	time_par = &(get_parameter(params, 3)->u.new_generic_integer);
	dateString_par = &(get_parameter(params, 4)->u.new_generic_octetstring);

	dateFormat = GenericOctetString_getOctetString(dateFormat_par, caller_gid);
	mheg_date = GenericInteger_getInteger(date_par, caller_gid);
	mheg_time = GenericInteger_getInteger(time_par, caller_gid);

	/*
	 * max length of the output is format length + 2x the number of % chars
	 * max is %Y = 4 output chars, but we already have 2 for the %Y in the format string
	 */
	npc = 0;
	for(i=0; i<dateFormat->size; i++)
		npc += (dateFormat->data[i] == '%') ? 1 : 0;

	/* +1 for sprintf's \0 */
	dateString.data = safe_malloc(dateFormat->size + (npc * 2) + 1);

	/* convert to UNIX time */
	unix_time = ((mheg_date - MHEG_EPOCH_OFFSET) * (24 * 60 * 60)) + mheg_time;
	/*
	 * let libc do all the hard work of working out the year etc
	 * we are passed a date/time as returned by GetCurrentDate, ie local time
	 * as GCD has already taken care of the timezone, use gmtime() not localtime() here
	 */
	tm = gmtime(&unix_time);

	/* write it out */
	dateString.size = 0;
	for(i=0; i<dateFormat->size; i++)
	{
		if(dateFormat->data[i] != '%' || i == dateFormat->size - 1)
		{
			dateString.data[dateString.size ++] = dateFormat->data[i];
		}
		else
		{
			switch(dateFormat->data[i + 1])
			{
			case 'Y':
				/* 4 digit year */
				year = tm->tm_year + 1900;
				dateString.size += sprintf((char *) &dateString.data[dateString.size], "%u", year);
				break;

			case 'y':
				/* 2 digit year */
				year = (tm->tm_year + 1900) % 100;
				dateString.size += sprintf((char *) &dateString.data[dateString.size], "%02u", year);
				break;

			case 'X':
				/* month 01-12 */
				month = tm->tm_mon + 1;
				dateString.size += sprintf((char *) &dateString.data[dateString.size], "%02u", month);
				break;

			case 'x':
				/* month 1-12 */
				month = tm->tm_mon + 1;
				dateString.size += sprintf((char *) &dateString.data[dateString.size], "%u", month);
				break;

			case 'D':
				/* day 01-31 */
				dateString.size += sprintf((char *) &dateString.data[dateString.size], "%02u", tm->tm_mday);
				break;

			case 'd':
				/* day 1-31 */
				dateString.size += sprintf((char *) &dateString.data[dateString.size], "%u", tm->tm_mday);
				break;

			case 'H':
				/* hour 00-23 */
				dateString.size += sprintf((char *) &dateString.data[dateString.size], "%02u", tm->tm_hour);
				break;

			case 'h':
				/* hour 0-23 */
				dateString.size += sprintf((char *) &dateString.data[dateString.size], "%u", tm->tm_hour);
				break;

			case 'I':
				/* hour 01-12 */
				hour = tm->tm_hour % 12;
				if(hour == 0)
					hour = 12;
				dateString.size += sprintf((char *) &dateString.data[dateString.size], "%02u", hour);
				break;

			case 'i':
				/* hour 1-12 */
				hour = tm->tm_hour % 12;
				if(hour == 0)
					hour = 12;
				dateString.size += sprintf((char *) &dateString.data[dateString.size], "%u", hour);
				break;

			case 'M':
				/* minutes 00-59 */
				dateString.size += sprintf((char *) &dateString.data[dateString.size], "%02u", tm->tm_min);
				break;

			case 'm':
				/* minutes 0-59 */
				dateString.size += sprintf((char *) &dateString.data[dateString.size], "%u", tm->tm_min);
				break;

			case 'S':
				/* seconds 00-59 */
				dateString.size += sprintf((char *) &dateString.data[dateString.size], "%02u", tm->tm_sec);
				break;

			case 's':
				/* seconds 0-59 */
				dateString.size += sprintf((char *) &dateString.data[dateString.size], "%u", tm->tm_sec);
				break;

			case 'A':
				/* AM/PM */
				if(tm->tm_hour < 12)
					dateString.size += sprintf((char *) &dateString.data[dateString.size], "AM");
				else
					dateString.size += sprintf((char *) &dateString.data[dateString.size], "PM");
				break;

			case 'a':
				/* am/pm */
				if(tm->tm_hour < 12)
					dateString.size += sprintf((char *) &dateString.data[dateString.size], "am");
				else
					dateString.size += sprintf((char *) &dateString.data[dateString.size], "pm");
				break;

			case '%':
				dateString.data[dateString.size ++] = '%';
				break;

			default:
				error("ResidentProgram: FormatDate: unknown format character '%c'", dateFormat->data[i + 1]);
				break;
			}
			i ++;
		}
	}

	GenericOctetString_setOctetString(dateString_par, caller_gid, &dateString);

#if (CLS_VERBOSE)
	verbose_class("ResidentProgram: FormatDate(\"%.*s\", %u, %u, \"%.*s\")", dateFormat->size, dateFormat->data,
									   mheg_date, mheg_time,
									   dateString.size, dateString.data);
#endif

	/* GenericOctetString_setOctetString copies it */
	safe_free(dateString.data);

	return true;
}

static bool
prog_GetDayOfWeek(LIST_OF(Parameter) *params, OctetString *caller_gid)
{
	GenericInteger *date_par;
	GenericInteger *dayOfWeek_par;
	unsigned int mheg_date;
	unsigned int dayOfWeek;

	if(!check_parameters(params, 2, Parameter_new_generic_integer,	/* in: date */
					Parameter_new_generic_integer))	/* out: dayOfWeek */
	{
		error("ResidentProgram: GetDayOfWeek (GDW): wrong number or type of parameters");
		return false;
	}

	date_par = &(get_parameter(params, 1)->u.new_generic_integer);
	dayOfWeek_par = &(get_parameter(params, 2)->u.new_generic_integer);

	mheg_date = GenericInteger_getInteger(date_par, caller_gid);

	/* Julian day number modulo 7 (+3 cuz 17/11/1858 was a Wednesday) */
	dayOfWeek = (mheg_date + 3) % 7;
	GenericInteger_setInteger(dayOfWeek_par, caller_gid, dayOfWeek);

#if (CLS_VERBOSE)
	verbose_class("ResidentProgram: GetDayOfWeek(%u, %u)", mheg_date, dayOfWeek);
#endif

	return true;
}

static bool
prog_Random(LIST_OF(Parameter) *params, OctetString *caller_gid)
{
	GenericInteger *num_par;
	GenericInteger *random_par;
	unsigned int num;
	unsigned int rnd;

	if(!check_parameters(params, 2, Parameter_new_generic_integer,	/* in: num */
					Parameter_new_generic_integer))	/* out: random */
	{
		error("ResidentProgram: Random (Rnd): wrong number or type of parameters");
		return false;
	}

	num_par = &(get_parameter(params, 1)->u.new_generic_integer);
	random_par = &(get_parameter(params, 2)->u.new_generic_integer);

	num = GenericInteger_getInteger(num_par, caller_gid);

	/* ITV like to get Random(0), returned value is supposed to be in the range 1..num */
	if(num == 0)
		rnd = 1;
	else	/* man page says low order bits are random too */
#if defined(WIN32)
		rnd = 1 + (rand() % num);
#else
		rnd = 1 + (random() % num);
#endif

	GenericInteger_setInteger(random_par, caller_gid, rnd);

#if (CLS_VERBOSE)
	verbose_class("ResidentProgram: Random(%u, %u)", num, rnd);
#endif

	return true;
}

static bool
prog_CastToContentRef(LIST_OF(Parameter) *params, OctetString *caller_gid)
{
	GenericOctetString *string_par;
	GenericContentReference *contentRef_par;
	OctetString *string;
	ContentReference ref;

	if(!check_parameters(params, 2, Parameter_new_generic_octetstring,		/* in: string */
					Parameter_new_generic_content_reference))	/* out: contentRef */
	{
		error("ResidentProgram: CastToContentRef (CTC): wrong number or type of parameters");
		return false;
	}

	string_par = &(get_parameter(params, 1)->u.new_generic_octetstring);
	contentRef_par = &(get_parameter(params, 2)->u.new_generic_content_reference);

	string = GenericOctetString_getOctetString(string_par, caller_gid);

	ref.size = string->size;
	ref.data = string->data;
	GenericContentReference_setContentReference(contentRef_par, caller_gid, &ref);

#if (CLS_VERBOSE)
	verbose_class("ResidentProgram: CastToContentRef(\"%.*s\")", string->size, string->data);
#endif

	return true;
}

static bool
prog_CastToObjectRef(LIST_OF(Parameter) *params, OctetString *caller_gid)
{
	GenericOctetString *groupId_par;
	GenericInteger *objectId_par;
	GenericObjectReference *objectRef_par;
	OctetString *groupId;
	int objectId;
	ObjectReference ref;

	if(!check_parameters(params, 3, Parameter_new_generic_octetstring,		/* in: string (gid) */
					Parameter_new_generic_integer,			/* in: objectId */
					Parameter_new_generic_object_reference))	/* out: objectRef */
	{
		error("ResidentProgram: CastToObjectRef (CTO): wrong number or type of parameters");
		return false;
	}

	groupId_par = &(get_parameter(params, 1)->u.new_generic_octetstring);
	objectId_par = &(get_parameter(params, 2)->u.new_generic_integer);
	objectRef_par = &(get_parameter(params, 3)->u.new_generic_object_reference);

	groupId = GenericOctetString_getOctetString(groupId_par, caller_gid);
	objectId = GenericInteger_getInteger(objectId_par, caller_gid);

	/* UK MHEG Profile says we can only generate external references */
	ref.choice = ObjectReference_external_reference;
	ref.u.external_reference.group_identifier.size = groupId->size;
	ref.u.external_reference.group_identifier.data = groupId->data;
	ref.u.external_reference.object_number = objectId;

	GenericObjectReference_setObjectReference(objectRef_par, caller_gid, &ref);

#if (CLS_VERBOSE)
	verbose_class("ResidentProgram: CastToObjectRef(\"%.*s\", %d)", groupId->size, groupId->data, objectId);
#endif

	return true;
}

static bool
prog_GetStringLength(LIST_OF(Parameter) *params, OctetString *caller_gid)
{
	GenericOctetString *string_par;
	GenericInteger *length_par;
	OctetString *string;

	if(!check_parameters(params, 2, Parameter_new_generic_octetstring,	/* in: string */
					Parameter_new_generic_integer))		/* out: length */
	{
		error("ResidentProgram: GetStringLength (GSL): wrong number or type of parameters");
		return false;
	}

	string_par = &(get_parameter(params, 1)->u.new_generic_octetstring);
	length_par = &(get_parameter(params, 2)->u.new_generic_integer);

	string = GenericOctetString_getOctetString(string_par, caller_gid);

	GenericInteger_setInteger(length_par, caller_gid, string->size);

#if (CLS_VERBOSE)
	//should be checked
	verbose_class("ResidentProgram: GetStringLength(\"%.*s\", %u)", string->size, string->data, string->size);
#endif
	return true;
}

static bool
prog_GetSubString(LIST_OF(Parameter) *params, OctetString *caller_gid)
{
	GenericOctetString *string_par;
	GenericInteger *beginExtract_par;
	GenericInteger *endExtract_par;
	GenericOctetString *stringResult_par;
	OctetString *string;
	int beginExtract;
	int endExtract;
	OctetString stringResult;

	if(!check_parameters(params, 4, Parameter_new_generic_octetstring,	/* in: string */
					Parameter_new_generic_integer,		/* in: beginExtract */
					Parameter_new_generic_integer,		/* in: endExtract */
					Parameter_new_generic_octetstring))	/* out: stringResult */
	{
		error("ResidentProgram: GetSubString (GSS): wrong number or type of parameters");
		return false;
	}

	string_par = &(get_parameter(params, 1)->u.new_generic_octetstring);
	beginExtract_par = &(get_parameter(params, 2)->u.new_generic_integer);
	endExtract_par = &(get_parameter(params, 3)->u.new_generic_integer);
	stringResult_par = &(get_parameter(params, 4)->u.new_generic_octetstring);

	string = GenericOctetString_getOctetString(string_par, caller_gid);
	beginExtract = GenericInteger_getInteger(beginExtract_par, caller_gid);
	endExtract = GenericInteger_getInteger(endExtract_par, caller_gid);

	/* range checks */
	beginExtract = MAX(beginExtract, 1);
	endExtract = MIN(endExtract, string->size);

	if(beginExtract > endExtract)
	{
		stringResult.size = 0;
		stringResult.data = NULL;
	}
	else
	{
		stringResult.size = (endExtract - beginExtract) + 1;	/* inclusive */
		stringResult.data = &string->data[beginExtract - 1];
	}
	GenericOctetString_setOctetString(stringResult_par, caller_gid, &stringResult);

#if (CLS_VERBOSE)
	verbose_class("ResidentProgram: GetSubString(\"%.*s\", %d, %d, \"%.*s\")", string->size, string->data,
									     beginExtract, endExtract,
									     stringResult.size, stringResult.data);
#endif

	return true;
}

/*
 * does the searching for SearchSubString and SearchAndExtractSubString
 * start counts from 1
 */

static int
search_substring(OctetString *string, int start, OctetString *search)
{
	/* assert */
	if(string->size != 0
	&& (start < 1 || start > string->size))
		fatal("search_substring: start=%d string->size=%d", start, string->size);

	/* simple cases */
	if(string->size == 0)
		return -1;
	if(search->size == 0)
		return start;

	while((start - 1) + search->size <= string->size)
	{
		if(VK_memcmp(&string->data[start-1], search->data, search->size) == 0)
			return start;
		start ++;
	}

	/* not found */
	return -1;
}

static bool
prog_SearchSubString(LIST_OF(Parameter) *params, OctetString *caller_gid)
{
	GenericOctetString *string_par;
	GenericInteger *startIndex_par;
	GenericOctetString *searchString_par;
	GenericInteger *stringPosition_par;
	OctetString *string;
	int startIndex;
	OctetString *searchString;
	int stringPosition;

	if(!check_parameters(params, 4, Parameter_new_generic_octetstring,	/* in: string */
					Parameter_new_generic_integer,		/* in: startIndex */
					Parameter_new_generic_octetstring,	/* in: searchString */
					Parameter_new_generic_integer))		/* out: stringPosition */
	{
		error("ResidentProgram: SearchSubString (SSS): wrong number or type of parameters");
		return false;
	}

	string_par = &(get_parameter(params, 1)->u.new_generic_octetstring);
	startIndex_par = &(get_parameter(params, 2)->u.new_generic_integer);
	searchString_par = &(get_parameter(params, 3)->u.new_generic_octetstring);
	stringPosition_par = &(get_parameter(params, 4)->u.new_generic_integer);

	string = GenericOctetString_getOctetString(string_par, caller_gid);
	startIndex = GenericInteger_getInteger(startIndex_par, caller_gid);
	searchString = GenericOctetString_getOctetString(searchString_par, caller_gid);

	/* range checks */
	startIndex = MAX(startIndex, 1);
	startIndex = MIN(startIndex, string->size);

	stringPosition = search_substring(string, startIndex, searchString);
	GenericInteger_setInteger(stringPosition_par, caller_gid, stringPosition);

#if (CLS_VERBOSE)
	verbose_class("ResidentProgram: SearchSubString(\"%.*s\", %d, \"%.*s\", %d)", string->size, string->data, startIndex,
										searchString->size, searchString->data,
										stringPosition);
#endif

	return true;
}

static bool
prog_SearchAndExtractSubString(LIST_OF(Parameter) *params, OctetString *caller_gid)
{
	GenericOctetString *string_par;
	GenericInteger *startIndex_par;
	GenericOctetString *searchString_par;
	GenericOctetString *stringResult_par;
	GenericInteger *stringPosition_par;
	OctetString *string;
	int startIndex;
	OctetString *searchString;
	OctetString stringResult;
	int stringPosition;
	int search_pos;

	if(!check_parameters(params, 5, Parameter_new_generic_octetstring,	/* in: string */
					Parameter_new_generic_integer,		/* in: startIndex */
					Parameter_new_generic_octetstring,	/* in: searchString */
					Parameter_new_generic_octetstring,	/* out: stringResult */
					Parameter_new_generic_integer))		/* out: stringPosition */
	{
		error("ResidentProgram: SearchAndExtractSubString (SES): wrong number or type of parameters");
		return false;
	}

	string_par = &(get_parameter(params, 1)->u.new_generic_octetstring);
	startIndex_par = &(get_parameter(params, 2)->u.new_generic_integer);
	searchString_par = &(get_parameter(params, 3)->u.new_generic_octetstring);
	stringResult_par = &(get_parameter(params, 4)->u.new_generic_octetstring);
	stringPosition_par = &(get_parameter(params, 5)->u.new_generic_integer);

	string = GenericOctetString_getOctetString(string_par, caller_gid);
	startIndex = GenericInteger_getInteger(startIndex_par, caller_gid);
	searchString = GenericOctetString_getOctetString(searchString_par, caller_gid);

	/* range checks */
	startIndex = MAX(startIndex, 1);
	startIndex = MIN(startIndex, string->size);

	if((search_pos = search_substring(string, startIndex, searchString)) != -1)
	{
		stringResult.size = search_pos - startIndex;
		stringResult.data = &string->data[startIndex - 1];
		stringPosition = search_pos + searchString->size;
	}
	else
	{
		stringResult.size = 0;
		stringResult.data = NULL;
		stringPosition = -1;
	}

	GenericOctetString_setOctetString(stringResult_par, caller_gid, &stringResult);
	GenericInteger_setInteger(stringPosition_par, caller_gid, stringPosition);

#if (CLS_VERBOSE)
	//should be checked
	verbose_class("ResidentProgram: SearchAndExtractSubString(\"%.*s\", %d, \"%.*s\", \"%.*s\", %d)",
								string->size, string->data,
								startIndex,
								searchString->size, searchString->data,
								stringResult.size, stringResult.data,
								stringPosition);
#endif
	return true;
}

static bool
prog_SI_GetServiceIndex(LIST_OF(Parameter) *params, OctetString *caller_gid)
{
	GenericOctetString *serviceReference_par;
	GenericInteger *serviceIndex_par;
	OctetString *serviceReference;
	int serviceIndex;

	if(!check_parameters(params, 2, Parameter_new_generic_octetstring,	/* in: serviceReference */
					Parameter_new_generic_integer))		/* out: serviceIndex */
	{
		error("ResidentProgram: SI_GetServiceIndex (GSI): wrong number or type of parameters");
		return false;
	}

	serviceReference_par = &(get_parameter(params, 1)->u.new_generic_octetstring);
	serviceIndex_par = &(get_parameter(params, 2)->u.new_generic_integer);

	serviceReference = GenericOctetString_getOctetString(serviceReference_par, caller_gid);
	serviceIndex = si_get_index(serviceReference);

	GenericInteger_setInteger(serviceIndex_par, caller_gid, serviceIndex);

#if (CLS_VERBOSE)
	verbose_class("ResidentProgram: SI_GetServiceIndex(\"%.*s\", %d)", serviceReference->size, serviceReference->data, serviceIndex);
#endif

	return true;
}

static bool
prog_SI_TuneIndex(LIST_OF(Parameter) *params, OctetString *caller_gid) /* CI+ MHEG Profile 에선 필요치 않다 : CI+ 에서 서비스 튜닝은 CI HCI (Host Control Interface Resource) 를 이용 */
{
	GenericInteger *serviceIndex_par;
	int serviceIndex;

	if(!check_parameters(params, 1, Parameter_new_generic_integer))	/* in: serviceIndex */
	{
		error("ResidentProgram: SI_TuneIndex (TIn): wrong number or type of parameters");
		return false;
	}

	serviceIndex_par = &(get_parameter(params, 1)->u.new_generic_integer);
	serviceIndex = GenericInteger_getInteger(serviceIndex_par, caller_gid);

	(void) si_tune_index(serviceIndex);

	return true;
}

static bool
prog_SI_TuneIndexInfo(LIST_OF(Parameter) *params, OctetString *caller_gid) /* CI+ MHEG Profile 에선 필요치 않다 */
{
	if(!check_parameters(params, 1, Parameter_new_generic_integer))	/* in: tuneinfo */
	{
		error("ResidentProgram: SI_TuneIndexInfo (TII): wrong number or type of parameters");
		return false;
	}

	verbose_todo("%s not yet implemented", __FUNCTION__);

	return true;
}

static bool
prog_SI_GetBasicSI(LIST_OF(Parameter) *params, OctetString *caller_gid)
{
	GenericInteger *serviceIndex_par;
	GenericInteger *networkId_par;
	GenericInteger *origNetworkId_par;
	GenericInteger *transportStreamId_par;
	GenericInteger *serviceId_par;
	int si;
	OctetString *url;
	unsigned int network_id;
	unsigned int orig_network_id;
	unsigned int transport_id;
	unsigned int service_id;

	if(!check_parameters(params, 5, Parameter_new_generic_integer,	/* in: serviceIndex */
					Parameter_new_generic_integer,	/* out: networkId */
					Parameter_new_generic_integer,	/* out: origNetworkId */
					Parameter_new_generic_integer,	/* out: transportStreamId */
					Parameter_new_generic_integer))	/* out: serviceId */
	{
		error("ResidentProgram: SI_GetBasicSI (BSI): wrong number or type of parameters");
		return false;
	}

	serviceIndex_par = &(get_parameter(params, 1)->u.new_generic_integer);
	networkId_par = &(get_parameter(params, 2)->u.new_generic_integer);
	origNetworkId_par = &(get_parameter(params, 3)->u.new_generic_integer);
	transportStreamId_par = &(get_parameter(params, 4)->u.new_generic_integer);
	serviceId_par = &(get_parameter(params, 5)->u.new_generic_integer);

	si = GenericInteger_getInteger(serviceIndex_par, caller_gid);
	url = si_get_url(si);

	network_id = si_get_network_id(url);
	orig_network_id = si_get_orig_network_id(url);
	transport_id = si_get_transport_id(url);
	service_id = si_get_service_id(url);

	/* not sure what the difference between the Network ID and the Original Network ID is */ /* -> 별도 처리함 */
	GenericInteger_setInteger(networkId_par, caller_gid, network_id);
	GenericInteger_setInteger(origNetworkId_par, caller_gid, orig_network_id);
	GenericInteger_setInteger(transportStreamId_par, caller_gid, transport_id);
	GenericInteger_setInteger(serviceId_par, caller_gid, service_id);

#if (CLS_VERBOSE)
	verbose_class("ResidentProgram: SI_GetBasicSI(%d, %u, %u, %u, %u)", si, network_id, orig_network_id, transport_id, service_id);
#endif

	return true;
}

static bool
prog_GetBootInfo(LIST_OF(Parameter) *params, OctetString *caller_gid) /* CI+ MHEG Profile 에선 필요치 않다 */
{
	if(!check_parameters(params, 2, Parameter_new_generic_boolean,		/* out: infoResult */
					Parameter_new_generic_octetstring))	/* out: bootInfo */
	{
		error("ResidentProgram: GetBootInfo (GBI): wrong number or type of parameters");
		return false;
	}

	verbose_todo("%s not yet implemented", __FUNCTION__);

	return true;
}

static bool
prog_CheckContentRef(LIST_OF(Parameter) *params, OctetString *caller_gid)
{
	GenericContentReference *refToCheck_par;
	GenericBoolean *refValid_par;
	GenericContentReference *refChecked_par;
	ContentReference *ref;
	bool valid;

	if(!check_parameters(params, 3, Parameter_new_generic_content_reference,	/* in: ref-to-check */
					Parameter_new_generic_boolean,			/* out: ref-valid-var */
					Parameter_new_generic_content_reference))	/* out: ref-checked-var */
	{
		error("ResidentProgram: CheckContentRef (CCR): wrong number or type of parameters");
		return false;
	}

	refToCheck_par = &(get_parameter(params, 1)->u.new_generic_content_reference);
	refValid_par = &(get_parameter(params, 2)->u.new_generic_boolean);
	refChecked_par = &(get_parameter(params, 3)->u.new_generic_content_reference);

	ref = GenericContentReference_getContentReference(refToCheck_par, caller_gid);

	valid = MHEGEngine_checkContentRef(ref);
	#if 1 // hmkim : added. TODO : 아래는 임시 구현.
	if (!valid)
	{
		int CCR_response_waiting_cnt;

		MHEGEngine_enterCriticalMissingContent;
		MHEGEngine_addMissingContent(&resident_class.rootClass, ref);
		MHEGEngine_leaveCriticalMissingContent;

		CCR_response_waiting_cnt = 100;
		while (!valid && CCR_response_waiting_cnt)
		{
			VK_TASK_Sleep(10);
			MHEGEngine_pollMissingContent(false);
			valid = MHEGEngine_checkContentRef(ref);
			CCR_response_waiting_cnt--;
		}
	}
	#endif

	/* output values */
	GenericBoolean_setBoolean(refValid_par, caller_gid, valid);
	GenericContentReference_setContentReference(refChecked_par, caller_gid, ref);

#if (CLS_VERBOSE)
	verbose_class("ResidentProgram: CheckContentRef(\"%.*s\", %s)", ref->size, ref->data, valid ? "true" : "false");
#endif

	return true;
}

static bool
prog_CheckGroupIDRef(LIST_OF(Parameter) *params, OctetString *caller_gid)
{
	if(!check_parameters(params, 3, Parameter_new_generic_object_reference,		/* in: ref-to-check */
					Parameter_new_generic_boolean,			/* out: ref-valid-var */
					Parameter_new_generic_object_reference))	/* out: ref-checked-var */
	{
		error("ResidentProgram: CheckGroupIDRef (CGR): wrong number or type of parameters");
		return false;
	}

//	verbose_todo("%s not yet implemented", __FUNCTION__);

#if 1 // hmkim : added.
{
	GenericObjectReference *refToCheck_par;
	GenericBoolean *refValid_par;
	GenericObjectReference *refChecked_par;
	ObjectReference *ref;
	OctetString obj;
	char *name, *space;
	bool valid;

	refToCheck_par = &(get_parameter(params, 1)->u.new_generic_object_reference);
	refValid_par = &(get_parameter(params, 2)->u.new_generic_boolean);
	refChecked_par = &(get_parameter(params, 3)->u.new_generic_object_reference);

	ref = GenericObjectReference_getObjectReference(refToCheck_par, caller_gid);

	/* TODO : 아래는 임시 구현 */
	name = ObjectReference_name(ref);
	space = strrchr(name, ' ');
	if (space) *space = '\0';
	obj.size = strlen(name)+1;

	if(name && obj.size>0)
	{
		obj.data = safe_malloc(obj.size);
		if(obj.data != NULL)
		{
			VK_memcpy(obj.data, name, obj.size);
			valid = MHEGEngine_findGroupObject(&obj) ? true : false;
			if (!valid) valid = MHEGEngine_checkContentRef(&obj);
			if (!valid)
			{
				int CGR_response_waiting_cnt;

				MHEGEngine_enterCriticalMissingContent;
				MHEGEngine_addMissingContent(&resident_class.rootClass, &obj);
				MHEGEngine_leaveCriticalMissingContent;

				CGR_response_waiting_cnt = 100;
				while (!valid && CGR_response_waiting_cnt)
				{
					VK_TASK_Sleep(10);
					MHEGEngine_pollMissingContent(false);
					valid = MHEGEngine_checkContentRef(&obj);
					CGR_response_waiting_cnt--;
				}
			}
			safe_free(obj.data);
		}
	}
	else
	{
		valid = false;
	}
	/* output values */
	GenericBoolean_setBoolean(refValid_par, caller_gid, valid);
	GenericObjectReference_setObjectReference(refChecked_par, caller_gid, ref);

#if (CLS_VERBOSE)
	verbose_class("ResidentProgram: CheckGroupIDRef(%s, %s)", ref ? ObjectReference_name(ref) : "NULL", valid ? "true" : "false");
#endif
}
#endif

	return true;
}

static bool
prog_VideoToGraphics(LIST_OF(Parameter) *params, OctetString *caller_gid) /* CI+ MHEG Profile 에선 필요치 않다 */
{
	if(!check_parameters(params, 4, Parameter_new_generic_integer,	/* in: videoX */
					Parameter_new_generic_integer,	/* in: videoY */
					Parameter_new_generic_integer,	/* out: graphicsX */
					Parameter_new_generic_integer))	/* out: graphicsY */
	{
		error("ResidentProgram: VideoToGraphics (VTG): wrong number or type of parameters");
		return false;
	}

	verbose_todo("%s not yet implemented", __FUNCTION__);

	return true;
}

static bool
prog_SetWidescreenAlignment(LIST_OF(Parameter) *params, OctetString *caller_gid) /* CI+ MHEG Profile 에선 필요치 않다 */
{
	if(!check_parameters(params, 1, Parameter_new_generic_integer))	/* in: mode */
	{
		error("ResidentProgram: SetWidescreenAlignment (SWA): wrong number or type of parameters");
		return false;
	}

	verbose_todo("%s not yet implemented", __FUNCTION__);

	return true;
}

static bool
prog_GetDisplayAspectRatio(LIST_OF(Parameter) *params, OctetString *caller_gid)
{
	if(!check_parameters(params, 1, Parameter_new_generic_integer))	/* out: aspectratio */
	{
		error("ResidentProgram: GetDisplayAspectRatio (GDA): wrong number or type of parameters");
		return false;
	}

//	verbose_todo("%s not yet implemented", __FUNCTION__);

#if 1 // jungmw : added.
{
	GenericInteger *aspectRatio_par;
	int aspectRatio;

	aspectRatio_par = &(get_parameter(params, 1)->u.new_generic_integer);
	aspectRatio = MHEGIB_GetDisplayAspectRatio();

	GenericInteger_setInteger(aspectRatio_par, caller_gid, aspectRatio);
}
#endif

	return true;
}

static Parameter CIS_response_param;
static OctetString CIS_response_caller_gid = {0, NULL};
static bool CIS_response_waiting = false;
static bool CIS_response_waiting_result = false;

static bool
prog_CI_SendMessage(LIST_OF(Parameter) *params, OctetString *caller_gid)
{
	if(!check_parameters(params, 2, Parameter_new_generic_octetstring,	/* in: message */
					Parameter_new_generic_octetstring))	/* out: response */
	{
		error("ResidentProgram: CI_SendMessage (CIS): wrong number or type of parameters");
		return false;
	}

//	verbose_todo("%s not yet implemented", __FUNCTION__);

#if 1 // hmkim : added.
{
	Parameter *param_1st, *param_2nd;
	char *name;
	int len;
	unsigned short session;
	int CIS_response_waiting_cnt;

	param_1st = get_parameter(params, 1);
	param_2nd = get_parameter(params, 2);

	/* assert */
	if (param_2nd->u.new_generic_octetstring.choice != GenericOctetString_indirect_reference) // UK MHEG Profile (D-BOOK 5.0) : Shall provide an IndirectReference to an OctetStringVariable
	{
		error("ResidentProgram: CI_SendMessage (CIS): wrong type of 2nd parameter (%d)", param_2nd->u.new_generic_octetstring.choice);
		return false;
	}

	/* get the name to send */
	name = parameter_name(param_1st, caller_gid);
	if (name == NULL)
	{
		error("ResidentProgram: CI_SendMessage (CIS): parameter_name() failed");
		return false;
	}

	/* get the name length to send */
	len = strlen(name);

	/* set the param for response */
	VK_memset(&CIS_response_param, 0x00, sizeof(Parameter));
	VK_memcpy(&CIS_response_param, param_2nd, sizeof(Parameter));

	/* set the caller id for response */
	CIS_response_caller_gid.size = caller_gid->size;
	CIS_response_caller_gid.data = safe_realloc(CIS_response_caller_gid.data, caller_gid->size);
	VK_memcpy(CIS_response_caller_gid.data, caller_gid->data, caller_gid->size);

	/* get the session id */
	session = MHEGIB_CiAppMmi_getSessionId();

	verbose_ammi("ResidentProgram: CI_SendMessage (CIS): \"%s\" sending...", name);

	//ycgo_Hx_Print
	Hx_Print("prog_CI_SendMessage\n");
	if (session == 0 || MHEGIB_FileRequst(session, eMhegIb_Ad_ReqType_Data, name, len))
	{
		error("MHEGIB_FileRequst (%d, %s) failed", session, name);
		return false;
	}

	/* waiting the response */ // 현 동작 구조상 async 하게 만들기가 힘듦. 일정 시간 기다리고 빠지게 하자!
	CIS_response_waiting = true;
	CIS_response_waiting_result = false;
	CIS_response_waiting_cnt = 100;
	while (CIS_response_waiting && CIS_response_waiting_cnt)
	{
		VK_TASK_Sleep(10);
		CIS_response_waiting_cnt--;
	}
	if (CIS_response_waiting)
	{
		CIS_response_waiting = false;
		// CIS_response_waiting_cnt 값을 매우 넉넉히 주지 않으면 data request 가 file request 들과 함께 수행될 경우 이 상황을 피하기 힘들다.
		// 현 동작 구조상 async 하게 만들기가 힘들고 CIS_response_waiting_cnt 값을 마냥 길게 줄 수도 없는 일이므로 일단 error 처리하도록 한다.
		// 이후 다시 try 될 때는 정상 수행될 것이다.
		verbose_ammi("ResidentProgram: CI_SendMessage (CIS): not received");
		return false;
	}
	if (CIS_response_waiting_result == false)
	{
		error("ResidentProgram: CI_SendMessage (CIS): received but result not good");
		return false;
	}
}
#endif

	return true;
}

bool
prog_CI_ReceiveMessage(unsigned long ulBytesLen, unsigned char *pucBytes) /* 이름은 이래도 public 함수 ^^;; prog_CI_SendMessage() 와 연관시키기 위함. */
{
	OctetString ret;
	VariableClass *var;

	/* assert */
	if (CIS_response_waiting == false)
	{
		// 이런 에러가 난다면 prog_CI_SendMessage()의 waiting time 설정이 짧거나 data request 를 하지 않았는데 캠에서 data ack 가 온 경우.
		// 단, Neotion 캠의 timeshift 예제에서는 pause.mhg 의 timer 동작에 의해 메인 scene 으로 갔다가 다시 timeshift 으로 복귀하는 경우 data request 를 하지 않아도 캠에서 일단 data ack 가 온다.
		// 따라서 일단 error 대신 verbose_ammi 로 찍도록 한다.
		verbose_ammi("ResidentProgram: CI_ReceiveMessage: too late response or not requested");
		return false;
	}
	if (CIS_response_param.u.new_generic_octetstring.choice != GenericOctetString_indirect_reference) // UK MHEG Profile (D-BOOK 5.0) : Shall provide an IndirectReference to an OctetStringVariable
	{
		error("ResidentProgram: CI_ReceiveMessage: wrong type of response parameter (%d)", CIS_response_param.u.new_generic_octetstring.choice);
		CIS_response_waiting = false;
		CIS_response_waiting_result = false;
		return false;
	}

	if (ulBytesLen && pucBytes)
	{
		ret.size = ulBytesLen;
		ret.data = safe_malloc(ulBytesLen);
		VK_memcpy(ret.data, pucBytes, ulBytesLen);

		if((var = (VariableClass *) MHEGEngine_findObjectReference(&(CIS_response_param.u.new_generic_octetstring.u.indirect_reference), &CIS_response_caller_gid)) != NULL)
		{
			if(var->rootClass.inst.rtti == RTTI_VariableClass
			&& VariableClass_type(var) == OriginalValue_octetstring)
			{
				OctetStringVariableClass_setOctetString(var, &ret);
			}
			else
			{
				safe_free(ret.data);
				error("ResidentProgram: CI_ReceiveMessage: %s is not a OctetStringVariableClass", ExternalReference_name(&var->rootClass.inst.ref));
				CIS_response_waiting = false;
				CIS_response_waiting_result = false;
				return false;
			}
		}

		safe_free(ret.data);
		verbose_ammi("ResidentProgram: CI_ReceiveMessage: \"%s\" received", parameter_name(&CIS_response_param, &CIS_response_caller_gid));
		CIS_response_waiting = false;
		CIS_response_waiting_result = true;
		return true;
	}
	else
	{
		error("ResidentProgram: CI_ReceiveMessage: data is not available");
		CIS_response_waiting = false;
		CIS_response_waiting_result = false;
		return false;
	}
}

static bool
prog_SetSubtitleMode(LIST_OF(Parameter) *params, OctetString *caller_gid) /* CI+ MHEG Profile 에선 필요치 않다 */
{
	if(!check_parameters(params, 1, Parameter_new_generic_boolean))	/* in: on */
	{
		error("ResidentProgram: SetSubtitleMode (SSM): wrong number or type of parameters");
		return false;
	}

	verbose_todo("%s not yet implemented", __FUNCTION__);

	return true;
}

static bool
prog_WhoAmI(LIST_OF(Parameter) *params, OctetString *caller_gid)
{
	GenericOctetString *ident_par;
	OctetString ident;
	char ident_str[] = MHEG_RECEIVER_ID " " MHEG_ENGINE_ID " " MHEG_DSMCC_ID;

	if(!check_parameters(params, 1, Parameter_new_generic_octetstring))	/* out: ident */
	{
		error("ResidentProgram: WhoAmI (WAI): wrong number or type of parameters");
		return false;
	}

	ident_par = &(get_parameter(params, 1)->u.new_generic_octetstring);

	ident.size = strlen(ident_str);
	ident.data = (unsigned char *) ident_str;

	GenericOctetString_setOctetString(ident_par, caller_gid, &ident);

#if (CLS_VERBOSE)
	verbose_class("ResidentProgram: WhoAmI(\"%s\")", ident_str);
#endif

	return true;
}

static bool
prog_Debug(LIST_OF(Parameter) *params, OctetString *caller_gid)
{
	Hx_Print("ResidentProgram: Debug message...\n");

	/* 0 or more params */
	while(params)
	{
		Hx_Print("%s", parameter_name(&params->item, caller_gid));
		params = params->next;
	}

	return true;
}

static bool
prog_RequestMPEGDecoder(LIST_OF(Parameter) *params, OctetString *caller_gid) /* S&T, RequestMPEGDecoder 예제 테스트를 위해 추가함 (CI+ MHEG Profile 추가 사항) */
{
	/* The CI Plus application is allowed to stop and start the stream in order to display an I-frame if it has permission using the resident program RequestMPEGDecoder.
	   Requests exclusive access to a MPEG decoder and video plane to display I-frames.
	   The MPEG decoder shall be available when no other application environment is active.
	   in (request) : If 'true' then the MHEG application is requesting exclusive use of the MPEG decoder and video plane. If 'false' it is releasing use of said decoder.
	   out (result) :
	      If request is 'true' then:
	         If the result is 'true' then I-frames may be used and shall remain available until the application exits, a new application starts or RequestMPEGDecoder is invoked again with request='false'
	         If the result is false then the MPEG decoder is not available and I-frames may not be used.
	      If request is 'false' then:
	         result shall be 'false', the MPEG decoder is no available and I-frames may not be used.
	   If the CI Plus application requires to stop the broadcast stream and display an I-frame then it must first get permission to use the MPEG decoder.
	   When the application has finished with the MPEG decoder it may release it by calling RequestMPEGDecoder with request='false'
	   however the application must have removed any I-frames from the display and restarted the stream with default components otherwise the results will be unpredictable. */

	if(!check_parameters(params, 2, Parameter_new_generic_boolean,	/* in: request */
					Parameter_new_generic_boolean))	/* out: result */
	{
		error("ResidentProgram: RequestMPEGDecoder (RMD): wrong number or type of parameters");
		return false;
	}

	/* TODO : 아래는 임시 코드 */
	if (GenericBoolean_getBoolean(&(get_parameter(params, 1)->u.new_generic_boolean), caller_gid) == true)
		GenericBoolean_setBoolean(&(get_parameter(params, 2)->u.new_generic_boolean), caller_gid, true);
	else
		GenericBoolean_setBoolean(&(get_parameter(params, 2)->u.new_generic_boolean), caller_gid, false);

	return true;
}

static bool
prog_TestInputMask(LIST_OF(Parameter) *params, OctetString *caller_gid) /* TestInputMask (CI+ 1.3 MHEG Profile 추가 사항)  */
{
	if(!check_parameters(params, 2, Parameter_new_generic_octetstring,	/* in: message */
					Parameter_new_generic_boolean)) /* out: response */
	{
		error("ResidentProgram: TestInputMask (TIM): wrong number or type of parameters");
		return false;
	}

//	verbose_todo("%s not yet implemented", __FUNCTION__);

#if 0 // ssgoo : added.
{
	GenericOctetString	*param_1st;
	GenericBoolean		*param_2nd;
	OctetString			*mask_oct;
	int len;
	unsigned short session;
	int CIS_response_waiting_cnt;

	param_1st = &(get_parameter(params, 1)->u.new_generic_octetstring);
	param_2nd = &(get_parameter(params, 2)->u.new_generic_boolean);

	/* assert */
	if (param_2nd->choice != GenericOctetString_indirect_reference) // DTG MHEG Spec Group White Paper:MHEG InputMaskExtension : Shall provide an IndirectReference to an BooleanVariable
	{
		error("ResidentProgram: TestInputMask (TIM): wrong type of 2nd parameter (%d)", param_2nd->choice);
		return false;
	}

	GenericBoolean_setBoolean(param_2nd, caller_gid, true);
	mask_oct = GenericOctetString_getOctetString(param_1st, caller_gid);

	verbose("%s Mask Size[%d], Value[0x%02X][0x%02X][0x%02X]\n", __FUNCTION__, mask_oct->size, mask_oct->data[0],mask_oct->data[1],mask_oct->data[2]);
}
#endif

	return true;
}



static bool
prog_SuppressMHEGGraphics(LIST_OF(Parameter) *params, OctetString *caller_gid) /* SuppressMHEGGraphics (CI+ 1.3 MHEG Profile 추가 사항) */
{
	/* If true then the Application MMI has relinquished the graphics plane and subtitles may be displayed when enabled. The graphics plane may be suppressed.
	If false then the Application MMI requires the graphics plane and subtitles shall be stopped and control of the graphics plane shall return to the Application MMI by the
	end of the resident program call.*/
	GenericBoolean	*param_1st;

	if(!check_parameters(params, 1, Parameter_new_generic_boolean)) /* in: GraphicsState */
	{
		error("ResidentProgram: TestInputMask (TIM): wrong number or type of parameters");
		return false;
	}

	param_1st = &(get_parameter(params, 1)->u.new_generic_boolean);

	verbose("%s Graphics State [%d]", __FUNCTION__, *param_1st);
	verbose_todo("%s not yet implemented", __FUNCTION__);

	return true;
}


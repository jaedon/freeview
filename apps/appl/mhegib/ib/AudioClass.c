/*
 * AudioClass.c
 */

#include "MHEGEngine.h"
#include "AudioClass.h"
#include "RootClass.h"
#include "StreamClass.h"
#include "Reference.h"
#include "Generic.h"
#include "VariableClass.h"

static void
default_AudioClassInstanceVars(AudioClass *t, AudioClassInstanceVars *v)
{
	/*
	 * don't do:
	 * VK_memset(v, 0x00, sizeof(AudioClassInstanceVars));
	 * or:
	 * v->owner = NULL;
	 * the whole AudioClass including these instance vars is zero'd when it is DER decoded
	 * we need to make sure v->owner is not set to NULL here
	 * in case our StreamClass is already active and has set our owner
	 */

	v->Volume = t->original_volume;

	return;
}

static void
free_AudioClassInstanceVars(AudioClassInstanceVars *v)
{
	return;
}

void
AudioClass_Preparation(AudioClass *t)
{
#if (CLS_VERBOSE)
	verbose_class("AudioClass: %s; Preparation", ExternalReference_name(&t->rootClass.inst.ref));
#endif

	/* has it already been prepared */
	if(t->rootClass.inst.AvailabilityStatus)
		return;

	/* retrieve the object from an entity outside the engine and set each internal attribute of the object to its initial value */
	default_AudioClassInstanceVars(t, &t->inst);

	/* Preparation inherited from the RootClass */
	RootClass_Preparation(&t->rootClass);

	return;
}

void
AudioClass_Activation(AudioClass *t)
{
#if (CLS_VERBOSE)
	verbose_class("AudioClass: %s; Activation", ExternalReference_name(&t->rootClass.inst.ref));
#endif

	/* has it already been activated */
	if(t->rootClass.inst.RunningStatus)
		return;

	/* has it been prepared yet */
	if(!t->rootClass.inst.AvailabilityStatus)
	{
		/* generates an IsAvailable event */
		AudioClass_Preparation(t);
	}

#if 0 // org
	/* set its RunningStatus and generate IsRunning event */
	t->rootClass.inst.RunningStatus = true;
	MHEGEngine_generateEvent(&t->rootClass.inst.ref, EventType_is_running, NULL);
#else // hmkim : modified.
	/* Activation inherited from the RootClass */
	RootClass_Activation(&t->rootClass);
#endif

	/*
	 * tell our StreamClass to start playing us
	 * owner maybe NULL if our StreamClass is in the process of activating itself
	 * in which case, it will start us when needed
	 */
	if(t->inst.owner != NULL)
		StreamClass_activateAudioComponent(t->inst.owner, t);

	return;
}

void
AudioClass_Deactivation(AudioClass *t)
{
#if (CLS_VERBOSE)
	verbose_class("AudioClass: %s; Deactivation", ExternalReference_name(&t->rootClass.inst.ref));
#endif

	/* is it already deactivated */
	if(!t->rootClass.inst.RunningStatus)
		return;

	/* Deactivation inherited from the RootClass */
	RootClass_Deactivation(&t->rootClass);

	/*
	 * tell our StreamClass to stop playing us
	 * owner maybe NULL if our StreamClass is in the process of deactivating itself
	 * in which case, it will stop us when needed
	 */
	if(t->inst.owner != NULL)
		StreamClass_deactivateAudioComponent(t->inst.owner, t);

	return;
}

void
AudioClass_Destruction(AudioClass *t)
{
#if (CLS_VERBOSE)
	verbose_class("AudioClass: %s; Destruction", ExternalReference_name(&t->rootClass.inst.ref));
#endif

	/* is it already destroyed */
	if(!t->rootClass.inst.AvailabilityStatus)
		return;

#if 1 // hmkim : added.
	/* Deactivate it if it is running */
	if(t->rootClass.inst.RunningStatus)
	{
		/* generates an IsStopped event */
		AudioClass_Deactivation(t);
	}
#endif

	/*
	 * spec says we should handle caching here
	 * rb-download caches everything
	 */

	free_AudioClassInstanceVars(&t->inst);

	/* Destruction inherited from the RootClass */
	RootClass_Destruction(&t->rootClass);

	return;
}

void
AudioClass_SetVolume(AudioClass *t, SetVolume *params, OctetString *caller_gid)
{
//	verbose_todo("%s not yet implemented", __FUNCTION__);

#if 1 // hmkim : added.
{
	int NewVolume = GenericInteger_getInteger(&params->new_volume, caller_gid);

	verbose_class("AudioClass: %s; SetVolume; NewVol %d; CurVol %d; OrgVol %d; ComponentTag %d", ExternalReference_name(&t->rootClass.inst.ref), NewVolume, t->inst.Volume, t->original_volume, t->component_tag);

	// If the elementary audio stream identified by the ComponentTag attribute is being played, update the rendering of the Audio by taking into account the new Volume.
	if (t->inst.Volume != NewVolume)
	{
		t->original_volume = t->inst.Volume;
		t->inst.Volume = NewVolume;

		if (NewVolume <= 0)
		{
			// audio mute
			MHEGIB_Audio_SetVolume(NewVolume, FALSE);
		}
		else
		{
			// audio unmute 하고 볼륨 레벨 셋팅하라... (t->original_volume 및 시스템 볼륨 레벨 고려 필요)
			MHEGIB_Audio_SetVolume(NewVolume, FALSE);
		}
	}
}
#endif

	return;
}

void
AudioClass_GetVolume(AudioClass *t, GetVolume *params, OctetString *caller_gid)
{
//	verbose_todo("%s not yet implemented", __FUNCTION__);

#if 1 // hmkim : added.
{
	VariableClass *var;

	verbose_class("AudioClass: %s; GetVolume", ExternalReference_name(&t->rootClass.inst.ref));

	if((var = (VariableClass *) MHEGEngine_findObjectReference(&params->volume_var, caller_gid)) == NULL)
		return;

	if(var->rootClass.inst.rtti != RTTI_VariableClass
	|| VariableClass_type(var) != OriginalValue_integer)
	{
		error("AudioClass: GetVolume: type mismatch (%d, %d)", var->rootClass.inst.rtti, VariableClass_type(var));
		return;
	}

	IntegerVariableClass_setInteger(var, t->inst.Volume);
}
#endif

	return;
}


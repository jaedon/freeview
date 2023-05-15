/*
 * TokenGroupClass.c
 */

#include "MHEGEngine.h"
#include "TokenGroupClass.h"
#include "RootClass.h"
#include "VisibleClass.h"
#include "Reference.h"
#include "Generic.h"
#include "ActionClass.h"
#include "VariableClass.h"
#include "clone.h"

static void
default_TokenGroupClassInstanceVars(TokenGroupClass *t, TokenGroupClassInstanceVars *v)
{
	VK_memset(v, 0x00, sizeof(TokenGroupClassInstanceVars));

	/* TokenManagerClass */

	/* When used in TokenGroup, the token maps into the Visibles defined in the TokenGroupItems attribute -> TokenGroupItems & MovementTable
	   When used in a ListGroup, the token maps into the cells defined by the Positions attribute -> Positions & MovementTable
	   Therefore, in a TokenGroup, the number of Visibles defined in TokenGroupItems shall equal the number of entries in each Movement structure in the MovementTable attribute (if encoded).
	   Additionally, in a ListGroup, the number of cells defined by the Positions attribute shall equal the number of entries in each Movement structure in the MovementTable attribute (if encoded). */
	v->TokenPosition = 1; /* [0, N], where N is the number of Visibles in the TokenGroup. The value 0 signifies that no Visible has the token. */

	return;
}

static void
free_TokenGroupClassInstanceVars(TokenGroupClassInstanceVars *v)
{
	return;
}

static void
TokenGroupClass_TransferToken(TokenGroupClass *t, unsigned int target) /* TokenManagerClass Internal behaviour */
{
	EventData data;

	// TODO : if target > the number of Visibles in the TokenGroup 인 경우에 대한 핸들링 필요.

	data.choice = EventData_integer;
	data.u.integer = t->inst.TokenPosition;
	MHEGEngine_generateEvent(&t->rootClass.inst.ref, EventType_token_moved_from, &data);

	t->inst.TokenPosition = target;

	data.u.integer = t->inst.TokenPosition;
	MHEGEngine_generateEvent(&t->rootClass.inst.ref, EventType_token_moved_to, &data);

	return;
}

void
TokenGroupClass_Preparation(TokenGroupClass *t)
{
#if (CLS_VERBOSE)
	verbose_class("TokenGroupClass: %s; Preparation", ExternalReference_name(&t->rootClass.inst.ref));
#endif

	/* has it already been prepared */
	if(t->rootClass.inst.AvailabilityStatus)
		return;

	/* retrieve the object from an entity outside the engine and set each internal attribute of the object to its initial value */
	default_TokenGroupClassInstanceVars(t, &t->inst);

	/* Preparation inherited from the RootClass */
	RootClass_Preparation(&t->rootClass);

	return;
}

void
TokenGroupClass_Activation(TokenGroupClass *t)
{
	LIST_TYPE(TokenGroupItem) *tgi_list;
	RootClass *tgi;
	EventData data;

#if (CLS_VERBOSE)
	verbose_class("TokenGroupClass: %s; Activation", ExternalReference_name(&t->rootClass.inst.ref));
#endif

	/* has it already been activated */
	if(t->rootClass.inst.RunningStatus)
		return;

	/* has it been prepared yet */
	if(!t->rootClass.inst.AvailabilityStatus)
	{
		TokenGroupClass_Preparation(t);
		#if 0 // hmkim : commented out (위의 TokenGroupClass_Preparation() 내부에서 처리된다).
		/* generates an IsAvailable event */
		RootClass_Preparation(&t->rootClass);
		#endif
	}

	/* do Activation of each TokenGroupItem */
	tgi_list = t->token_group_items;
	while(tgi_list)
	{
		if((tgi = MHEGEngine_findObjectReference(&tgi_list->item.a_visible, &t->rootClass.inst.ref.group_identifier)) != NULL)
			VisibleClass_Activation(tgi, NULL);
		tgi_list = tgi_list->next;
	}

	/* generate a TokenMovedTo event */
	data.choice = EventData_integer;
	data.u.integer = t->inst.TokenPosition;
	MHEGEngine_generateEvent(&t->rootClass.inst.ref, EventType_token_moved_to, &data);

#if 0 // org
	/* set its RunningStatus and generate IsRunning event */
	t->rootClass.inst.RunningStatus = true;
	MHEGEngine_generateEvent(&t->rootClass.inst.ref, EventType_is_running, NULL);
#else // hmkim : modified.
	/* Activation inherited from the RootClass */
	RootClass_Activation(&t->rootClass);
#endif

	return;
}

void
TokenGroupClass_Deactivation(TokenGroupClass *t)
{
	EventData data;

#if (CLS_VERBOSE)
	verbose_class("TokenGroupClass: %s; Deactivation", ExternalReference_name(&t->rootClass.inst.ref));
#endif

	/* is it already deactivated */
	if(!t->rootClass.inst.RunningStatus)
		return;

	/* generate a TokenMovedFrom event */
	data.choice = EventData_integer;
	data.u.integer = t->inst.TokenPosition;
	MHEGEngine_generateEvent(&t->rootClass.inst.ref, EventType_token_moved_from, &data);

	/* Deactivation inherited from the RootClass */
	RootClass_Deactivation(&t->rootClass);

	return;
}

void
TokenGroupClass_Destruction(TokenGroupClass *t)
{
#if (CLS_VERBOSE)
	verbose_class("TokenGroupClass: %s; Destruction", ExternalReference_name(&t->rootClass.inst.ref));
#endif

	/* is it already destroyed */
	if(!t->rootClass.inst.AvailabilityStatus)
		return;

#if 1 // hmkim : added.
	/* Deactivate it if it is running */
	if(t->rootClass.inst.RunningStatus)
	{
		/* generates an IsStopped event */
		TokenGroupClass_Deactivation(t);
	}
#endif

	/*
	 * spec says we should handle caching here
	 * rb-download caches everything
	 */

	free_TokenGroupClassInstanceVars(&t->inst);

	/* Destruction inherited from the RootClass */
	RootClass_Destruction(&t->rootClass);

	return;
}

void
TokenGroupClass_Clone(TokenGroupClass *t, Clone *params, OctetString *caller_gid)
{
	verbose_todo("%s not yet implemented", __FUNCTION__);

	verbose_class("TokenGroupClass: %s; Clone", ExternalReference_name(&t->rootClass.inst.ref));

	return;
}

void
TokenGroupClass_Move(TokenGroupClass *t, Move *params, OctetString *caller_gid) /* TokenManagerClass */
{
	unsigned int movement_id = (unsigned int) GenericInteger_getInteger(&params->movement_identifier, caller_gid);
	unsigned int count;
	LIST_TYPE(Movement) *movement_list;
	LIST_TYPE(int) *target_list;
	unsigned int target;

	/* see which Movement list we need */
	/* find the row in the movement_table, movement_id starts at 1 : [1,M], where M is the number of Movement structures encoded in the MovementTable exchanged attribute. */
	count = movement_id;
	movement_list = t->movement_table;
	while(movement_list && count > 1)
	{
		movement_list = movement_list->next;
		count --;
	}
	/* was movement_id valid */
	if(movement_list == NULL)
	{
		error("TokenGroupClass: Move: invalid movement ID: %u", movement_id);
		return;
	}

	/* find the new TokenPosition in the movement list (index starts at 1) */
	count = t->inst.TokenPosition;
	target_list = movement_list->item;
	while(target_list && count > 1)
	{
		target_list = target_list->next;
		count --;
	}
	/* was it valid */
	if(target_list == NULL)
	{
		error("TokenGroupClass: Move: movement list %u is too short", movement_id);
		return;
	}

	/* get the new TokenPosition */
	target = (unsigned int) target_list->item;

	/* has it moved */
	/* If the TargetElement does not have the token yet, apply the TransferToken(TargetElement) behaviour of the TokenManager. */
	verbose_class("TokenGroupClass: %s; Move; %u; %u -> %u", ExternalReference_name(&t->rootClass.inst.ref), movement_id, t->inst.TokenPosition, target);
	if(t->inst.TokenPosition != target)
		TokenGroupClass_TransferToken(t, target);

	return;
}

void
TokenGroupClass_MoveTo(TokenGroupClass *t, MoveTo *params, OctetString *caller_gid) /* TokenManagerClass */
{
	unsigned int index = (unsigned int) GenericInteger_getInteger(&params->index, caller_gid);

	verbose_class("TokenGroupClass: %s; MoveTo; %u -> %u", ExternalReference_name(&t->rootClass.inst.ref), t->inst.TokenPosition, index);

#if 1 // hmkim : added.
	/* ISO/IEC 13522-5 의 TransferToken 항목 기술을 보면
	   Index shall be set within the range [0, N], where N is the number of elements in the group...
	   The TokenMovedTo and TokenMovedFrom events will be generated even if the value of TokenPosition after (before) the token moved was 0... 이라고 되어 있으나
	   ISO/IEC 13522-5 & Cor.1 의 다른 기술이나 UK MHEG Profile (D-BOOK 5.0) 의 CallActionSlot(Index) 항목 기술을 보면
	   Index shall be set in the range [1, number of ActionSlots associated with the item that currently has the token]... 이라고 되어 있어
	   아래와 같이 index 가 0 이면 TransferToken 하지 않고 리턴하도록 처리함 (또한, 아래 코드가 없으면 SmarDTV 캠의 VOD 예제에서 S&T 엔진과 달리 처리됨). */
	if(index == 0)
		return;
#endif

	/* has it moved */
	/* If the TargetElement does not have the token yet, apply the TransferToken(TargetElement) behaviour of the TokenManager. */
	if(t->inst.TokenPosition != index)
		TokenGroupClass_TransferToken(t, index);

	return;
}

void
TokenGroupClass_GetTokenPosition(TokenGroupClass *t, GetTokenPosition *params, OctetString *caller_gid) /* TokenManagerClass */
{
	VariableClass *var;

	verbose_class("TokenGroupClass: %s; GetTokenPosition", ExternalReference_name(&t->rootClass.inst.ref));

	if((var = (VariableClass *) MHEGEngine_findObjectReference(&params->token_position_var, caller_gid)) == NULL)
		return;

	if(var->rootClass.inst.rtti != RTTI_VariableClass
	|| VariableClass_type(var) != OriginalValue_integer)
	{
		error("TokenGroupClass: GetTokenPosition: type mismatch (%d, %d)", var->rootClass.inst.rtti, VariableClass_type(var));
		return;
	}

	IntegerVariableClass_setInteger(var, t->inst.TokenPosition);

	return;
}

void
TokenGroupClass_CallActionSlot(TokenGroupClass *t, CallActionSlot *params, OctetString *caller_gid)
{
	LIST_OF(ActionSlot) *action_slots;
	LIST_TYPE(ActionSlot) *action;
	LIST_TYPE(TokenGroupItem) *tgi;
	unsigned int i, index = (unsigned int) GenericInteger_getInteger(&params->index, caller_gid);

	verbose_class("TokenGroupClass: %s; CallActionSlot; index %u; TokenPosition %u", ExternalReference_name(&t->rootClass.inst.ref), index, t->inst.TokenPosition);

	/*
	 * the spec isn't clear, but from looking at real world apps, index starts at 1
	 * the spec does allow index=0 (though I've never seen it), so we assume this means do nothing
	 * => ISO/IEC 13522-5 에는 Index shall be set in the range [0, number of ActionSlots associated with the item that currently has the token]... 이라고 되어 있으나
	 *    UK MHEG Profile (D-BOOK 5.0) 에는 Index shall be set in the range [1, number of ActionSlots associated with the item that currently has the token]... 이라고 되어 있어 아래의 처리는 적절한 것이다.
	 */
	if(index == 0)
	{
		verbose_class("TokenGroupClass: CallActionSlot: index=0");
		return;
	}

	/* if no-one has the token, use the no_token_action_slots */
	if(t->inst.TokenPosition == 0)
	{
		action_slots = t->no_token_action_slots;
	}
	else
	{
		/* use the action_slots from the TokenGroupItem that has the token */
		tgi = t->token_group_items;
		/* counting starts at 1 */
		i = t->inst.TokenPosition - 1;
		while(tgi && i != 0)
		{
			tgi = tgi->next;
			i --;
		}
		/* make sure it exists */
		if(tgi == NULL)
		{
			error("TokenGroupClass: CallActionSlot: invalid TokenPosition: %u", t->inst.TokenPosition);
			return;
		}
		action_slots = tgi->item.action_slots;
	}

	/* find the action */
	action = action_slots;
	while(action && index != 1)
	{
		action = action->next;
		index --;
	}

	/* check it exists */
	if(action == NULL)
	{
		error("TokenGroupClass: CallActionSlot: invalid ActionSlot: %u", GenericInteger_getInteger(&params->index, caller_gid));
		return;
	}

	/*
	 * if the action is not Null, add the ElementaryActions to temp_actionq
	 * note, just doing:
	 * ActionClass_execute(&action->item.u.action_class, &t->rootClass.inst.ref.group_identifier);
	 * is not the same as adding the actions to the queue
	 * (also ActionClass_execute will ignore context changing actions - TransitionTo, Launch, Spawn and Quit)
	 */
	if(action->item.choice == ActionSlot_action_class)
		MHEGEngine_addToTempActionQ(&action->item.u.action_class, &t->rootClass.inst.ref); // hmkim : org is MHEGEngine_addToTempActionQ(&action->item.u.action_class, &t->rootClass.inst.ref.group_identifier);

	return;
}


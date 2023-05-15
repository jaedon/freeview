/*
 * ListGroupClass.c
 */

#include "MHEGEngine.h"
#include "ListGroupClass.h"
#include "RootClass.h"
#include "VisibleClass.h"
#include "Reference.h"
#include "Generic.h"
#include "ActionClass.h"
#include "VariableClass.h"
#include "clone.h"

static void
default_ListGroupClassInstanceVars(ListGroupClass *t, ListGroupClassInstanceVars *v)
{
	VK_memset(v, 0x00, sizeof(ListGroupClassInstanceVars));

	/* TokenManagerClass */

	/* When used in TokenGroup, the token maps into the Visibles defined in the TokenGroupItems attribute -> TokenGroupItems & MovementTable
	   When used in a ListGroup, the token maps into the cells defined by the Positions attribute -> Positions & MovementTable
	   Therefore, in a TokenGroup, the number of Visibles defined in TokenGroupItems shall equal the number of entries in each Movement structure in the MovementTable attribute (if encoded).
	   Additionally, in a ListGroup, the number of cells defined by the Positions attribute shall equal the number of entries in each Movement structure in the MovementTable attribute (if encoded). */
	v->TokenPosition = 1; /* [0, N], where N is the number of Cells in the ListGroup. The value 0 signifies that no Cell has the token. */

	/* ListGroupClass */

	/* Add each reference listed in the TokenGroupItems attribute to the ItemList in the order they are listed in the TokenGroupItems attribute.
	   If a Visible is referenced more than once in the TokenGroupItems, it is added only at its first occurrence in the ItemList.
	   Null ObjectReferences are not added to ItemList. */
	#if 0 // org
	v->ItemList = NULL;
	#else // hmkim : modified.
	{
		LIST_TYPE(TokenGroupItem) *tgi_list = t->token_group_items;
		LIST_TYPE(ListGroupItem) *lgi_list;
		LIST_TYPE(ListGroupItem) *new_item;
		bool duplicate;
		unsigned int NumOfItems = 0;

		while(tgi_list)
		{
			if(MHEGEngine_findObjectReference(&tgi_list->item.a_visible, &t->rootClass.inst.ref.group_identifier) != NULL)
			{
				lgi_list = v->ItemList;
				duplicate = false;
				while(lgi_list && !duplicate)
				{
					if(ObjectReference_cmp(&lgi_list->item.a_visible, &t->rootClass.inst.ref.group_identifier, &tgi_list->item.a_visible, &t->rootClass.inst.ref.group_identifier) == 0)
					{
						duplicate = true;
						break;
					}
					lgi_list = lgi_list->next;
				}
				if(!duplicate)
				{
					if((new_item = safe_malloc(sizeof(LIST_TYPE(ListGroupItem)))) == NULL)
					{
						fatal("Out of memory");
						return;
					}
					ObjectReference_dup(&new_item->item.a_visible, &tgi_list->item.a_visible);
					new_item->item.ItemSelectionStatus = false;
					LIST_APPEND(&v->ItemList, new_item);
					NumOfItems++;
				}
			}
			tgi_list = tgi_list->next;
		}

		v->NumOfItems = NumOfItems;
	}
	#endif

	/* FirstItem may take any Integer value in the range [1, length of ItemList]. If the ItemList is empty then FirstItem shall take the value 1. */
	v->FirstItem = 1;

	#if 1 // hmkim : added.
	/* WindowSize = # of Cells = # of Positions = # of entries in each Movement structure in the MovementTable */
	{
		LIST_TYPE(XYPosition) *pos_list = t->positions;
		unsigned int window_size = 0;

		while(pos_list)
		{
			pos_list = pos_list->next;
			window_size++;
		}

		v->WindowSize = window_size;
	}
	#endif

	/* we need to take a copy of this, so if we change it, the originals stay the same */
	v->Positions = t->positions; // TODO : 각 position 에 대해서 XYPosition_dup() 시켜서 쓰는 것이 좋을 듯. 이 경우 free_ListGroupClassInstanceVars()도 변경해야 함. 확인 필요...

	#if 1 // hmkim : added.
	if (v->ItemList == NULL || v->NumOfItems == 0) // Item 이 없는 경우에 대한 핸들링 -> 여기선 에러만 찍고 각 액션 루틴에서 핸들링한다.
	{
		error("ListGroupClass: no item");
	}
	if (v->Positions == NULL || v->WindowSize == 0) // Cell 이 없는 경우에 대한 핸들링 -> 여기선 에러만 찍고 각 액션 루틴에서 핸들링한다.
	{
		error("ListGroupClass: no cell");
	}
	#endif

	return;
}

static void
free_ListGroupItem(ListGroupItem *i)
{
#if 0 // og
	free_TokenGroupItem(&i->item); /* free_TokenGroupItem will use der_free */
#else // hmkim : modified.
	free_ObjectReference(&i->a_visible);
#endif

	return;
}

static void
free_ListGroupClassInstanceVars(ListGroupClassInstanceVars *v)
{
	LIST_FREE_ITEMS(&v->ItemList, ListGroupItem, free_ListGroupItem, safe_free);

//	LIST_FREE_ITEMS(&v->Positions, XYPosition, free_XYPosition, safe_free); // hmkim : added. see default_ListGroupClassInstanceVars()'s TODO.

	return;
}

static void
ListGroupClass_TransferToken(ListGroupClass *t, unsigned int target) /* TokenManagerClass Internal behaviour */ // hmkim : added.
{
	EventData data;

	if (target > t->inst.WindowSize) // if target > the number of Cells in the ListGroup
	{
		error("ListGroupClass: TransferToken: invalid target %d (%d)", target, t->inst.WindowSize);
		return;
	}

	data.choice = EventData_integer;
	data.u.integer = t->inst.TokenPosition;
	MHEGEngine_generateEvent(&t->rootClass.inst.ref, EventType_token_moved_from, &data);

	t->inst.TokenPosition = target;

	data.u.integer = t->inst.TokenPosition;
	MHEGEngine_generateEvent(&t->rootClass.inst.ref, EventType_token_moved_to, &data);

	return;
}

static void
ListGroupClass_Update(ListGroupClass *t) // hmkim : added.
{
	RootClass *lgi;
	unsigned int window_start_array_index, i;
	ListGroupItem *window_item;
	ListGroupItem *no_window_item;
	XYPosition *cell_item;
	EventData data;

	#if 0 // RunningStatus 체크는 caller 쪽에서 처리해라.
	if(!t->rootClass.inst.RunningStatus)
		return;
	#endif

	/* If the RunningStatus of the ListGroup is True then execute the following sequence of actions:
	   1. For each item that will not be presented in a cell, if the item is active then apply the Deactivation behaviour to it and set its position to its OriginalPosition attribute.
	   2. For each item, if it is to be presented at a cell, apply the Preparation behaviour to it and then set its Position (internal attribute) to the position defined for that cell.
	   3. For each item that will be presented in a cell, if the item is inactive, apply the Activation behaviour to it. */

	/* present 될 Item 들과 그 좌표값들을 구하여 step 2 & 3 을 수행하고 present 안될 Item 들에 대해서는 step 1 을 수행 (ISO/IEC 13522-5 & Cor.1) :

	   스펙 내용을 엄밀히 따르면 우선 present 안될 Item 들에 대해서 일괄적으로 Deactivation 을 수행하고 나서 present 될 Item 들에 대해서 일괄적으로 Preparation 을 수행하고 그 다음 Activation 을 수행하도록 되어 있지만
	   여기서는 편의상 우선 present 될 Item 들에 대해서 각기 Preparation, Activation 을 수행하고 나서 present 안될 Item 들에 대해서 Deactivation 을 수행하는 것으로 한다. 크게 상관 없을 듯...

	   window array index = [ t->inst.FirstItem - 1 ... t->inst.FirstItem - 1 + t->inst.WindowSize - 1 ]
	     cell array index = [ 0 ... t->inst.WindowSize - 1 ]

	   예1) Cell 이 3 개이고 (즉 WindowSize : 3) Item 이 7 개일 때 (즉 NumOfItems : 7) FirstItem 이 6 이라면
	        window item 은 WrapAround 가 false 일 때 item[5], item[6], WrapAround 가 true 일 때 item[5], item[6], item[0] 이 되고
	        대응되는 cell 은 각기 cell[0], cell[1] 와 cell[0], cell[1], cell[2] 가 된다.
	        또한, no window item 은 각기 item[0], item[1], item[2], item[3], item[4] 와 item[1], item[2], item[3], item[4] 가 된다.

	   예2) Cell 이 5 개이고 (즉 WindowSize : 5) Item 이 4 개일 때 (즉 NumOfItems : 4) FirstItem 이 2 이라면
	        window item 은 WrapAround 가 false 일 때 item[1], item[2], item[3], WrapAround 가 true 일 때 item[1], item[2], item[3], item[0] 이 되고
	        대응되는 cell 은 각기 cell[0], cell[1], cell[2] 와 cell[0], cell[1], cell[2], cell[3] 가 된다.
	        또한, no window item 은 각기 item[0] 와 none 이 된다. */

	// 이하 코드에서 ItemList is NULL (NumOfItems is 0), Positions is NULL (WindowSize is 0), FirstItem < 1 또는 FirstItem > NumOfItems, TokenPosition > WindowSize 인 경우에 대한 핸들링은 필요 없음.

	window_start_array_index = t->inst.FirstItem - 1; // FirstItem starting from 1, in the range [1, length of ItemList]

	if (t->wrap_around)
	{
		// present 될 Item 들 -> do step 2 & 3
		// 예1) if WindowSize is 3 and NumOfItems is 7 then i is 0, 1, 2
		// 예2) if WindowSize is 5 and NumOfItems is 4 then i is 0, 1, 2, 3
		for (i = 0; i < t->inst.WindowSize && i < t->inst.NumOfItems; i++)
		{
			// 예1) if FirstItem is 6 then window_start_array_index is 5 and window item is 5, 6, 0. also its cell item is 0, 1, 2 respectively
			// 예2) if FirstItem is 2 then window_start_array_index is 1 and window item is 1, 2, 3, 0. also its cell item is 0, 1, 2, 3 respectively
			window_item = &t->inst.ItemList[(window_start_array_index + i) % t->inst.NumOfItems].item;
			cell_item = &t->inst.Positions[i].item;
			if((lgi = MHEGEngine_findObjectReference(&window_item->a_visible, &t->rootClass.inst.ref.group_identifier)) != NULL) // 이 시점에서 NULL 일리는 없다. see default_ListGroupClassInstanceVars(), ListGroupClass_Additem()
			{
				if (VisibleClass_Activation(lgi, cell_item))
				{
					if ((window_start_array_index + i) % t->inst.NumOfItems == 0)
					{
						/* generate a FirstItemPresented event with ItemList 의 header (the first item in the ItemList) 가 window 에 들어올 때 true, 나갈 때 false, 또는 ItemList 가 empty 가 될 때 false */
						data.choice = EventData_boolean;
						data.u.boolean = true;
						MHEGEngine_generateEvent(&t->rootClass.inst.ref, EventType_first_item_presented, &data);
					}
					if ((window_start_array_index + i) % t->inst.NumOfItems == t->inst.NumOfItems - 1)
					{
						/* generate a LastItemPresented event with ItemList 의 tail (the last item in the ItemList) 가 window 에 들어올 때 true, 나갈 때 false, 또는 ItemList 가 empty 가 될 때 false */
						data.choice = EventData_boolean;
						data.u.boolean = true;
						MHEGEngine_generateEvent(&t->rootClass.inst.ref, EventType_last_item_presented, &data);
					}
				}
			}
		}
		// present 안될 Item 들 -> do step 1
		// 예1) i is 3, 4, 5, 6
		// 예2) i is x
		for (; i < t->inst.NumOfItems; i++)
		{
			// 예1) no window item is 1, 2, 3, 4
			// 예2) no window item is none
			no_window_item = &t->inst.ItemList[(window_start_array_index + i) % t->inst.NumOfItems].item;
			if((lgi = MHEGEngine_findObjectReference(&no_window_item->a_visible, &t->rootClass.inst.ref.group_identifier)) != NULL) // 이 시점에서 NULL 일리는 없다. see default_ListGroupClassInstanceVars(), ListGroupClass_Additem()
			{
				if (VisibleClass_Deactivation(lgi))
				{
					if ((window_start_array_index + i) % t->inst.NumOfItems == 0)
					{
						/* generate a FirstItemPresented event with ItemList 의 header (the first item in the ItemList) 가 window 에 들어올 때 true, 나갈 때 false, 또는 ItemList 가 empty 가 될 때 false */
						data.choice = EventData_boolean;
						data.u.boolean = false;
						MHEGEngine_generateEvent(&t->rootClass.inst.ref, EventType_first_item_presented, &data);
					}
					if ((window_start_array_index + i) % t->inst.NumOfItems == t->inst.NumOfItems - 1)
					{
						/* generate a LastItemPresented event with ItemList 의 tail (the last item in the ItemList) 가 window 에 들어올 때 true, 나갈 때 false, 또는 ItemList 가 empty 가 될 때 false */
						data.choice = EventData_boolean;
						data.u.boolean = false;
						MHEGEngine_generateEvent(&t->rootClass.inst.ref, EventType_last_item_presented, &data);
					}
				}
			}
		}
	}
	else
	{
		// present 될 Item 들 -> do step 2 & 3
		// 예1) if WindowSize is 3, NumOfItems is 7 and FirstItem is 6 then window_start_array_index is 5 and i is 0, 1
		// 예2) if WindowSize is 5, NumOfItems is 4 and FirstItem is 2 then window_start_array_index is 1 and i is 0, 1, 2
		for (i = 0; i < t->inst.WindowSize && window_start_array_index + i < t->inst.NumOfItems; i++)
		{
			// 예1) and window item is 5, 6. also its cell item is 0, 1 respectively
			// 예2) and window item is 1, 2, 3. also its cell item is 0, 1, 2 respectively
			window_item = &t->inst.ItemList[window_start_array_index + i].item;
			cell_item = &t->inst.Positions[i].item;
			if((lgi = MHEGEngine_findObjectReference(&window_item->a_visible, &t->rootClass.inst.ref.group_identifier)) != NULL) // 이 시점에서 NULL 일리는 없다. see default_ListGroupClassInstanceVars(), ListGroupClass_Additem()
			{
				if (VisibleClass_Activation(lgi, cell_item))
				{
					if (window_start_array_index + i == 0)
					{
						/* generate a FirstItemPresented event with ItemList 의 header (the first item in the ItemList) 가 window 에 들어올 때 true, 나갈 때 false, 또는 ItemList 가 empty 가 될 때 false */
						data.choice = EventData_boolean;
						data.u.boolean = true;
						MHEGEngine_generateEvent(&t->rootClass.inst.ref, EventType_first_item_presented, &data);
					}
					if (window_start_array_index + i == t->inst.NumOfItems - 1)
					{
						/* generate a LastItemPresented event with ItemList 의 tail (the last item in the ItemList) 가 window 에 들어올 때 true, 나갈 때 false, 또는 ItemList 가 empty 가 될 때 false */
						data.choice = EventData_boolean;
						data.u.boolean = true;
						MHEGEngine_generateEvent(&t->rootClass.inst.ref, EventType_last_item_presented, &data);
					}
				}
			}
		}
		// present 안될 Item 들 -> do step 1
		// 예1) i is 2, 3, 4, 5, 6
		// 예2) i is 3
		for (; i < t->inst.NumOfItems; i++)
		{
			// 예1) no window item is 0, 1, 2, 3, 4
			// 예2) no window item is 0
			no_window_item = &t->inst.ItemList[(window_start_array_index + i) % t->inst.NumOfItems].item;
			if((lgi = MHEGEngine_findObjectReference(&no_window_item->a_visible, &t->rootClass.inst.ref.group_identifier)) != NULL) // 이 시점에서 NULL 일리는 없다. see default_ListGroupClassInstanceVars(), ListGroupClass_Additem()
			{
				if (VisibleClass_Deactivation(lgi))
				{
					if ((window_start_array_index + i) % t->inst.NumOfItems == 0)
					{
						/* generate a FirstItemPresented event with ItemList 의 header (the first item in the ItemList) 가 window 에 들어올 때 true, 나갈 때 false, 또는 ItemList 가 empty 가 될 때 false */
						data.choice = EventData_boolean;
						data.u.boolean = false;
						MHEGEngine_generateEvent(&t->rootClass.inst.ref, EventType_first_item_presented, &data);
					}
					if ((window_start_array_index + i) % t->inst.NumOfItems == t->inst.NumOfItems - 1)
					{
						/* generate a LastItemPresented event with ItemList 의 tail (the last item in the ItemList) 가 window 에 들어올 때 true, 나갈 때 false, 또는 ItemList 가 empty 가 될 때 false */
						data.choice = EventData_boolean;
						data.u.boolean = false;
						MHEGEngine_generateEvent(&t->rootClass.inst.ref, EventType_last_item_presented, &data);
					}
				}
			}
		}
	}

	if (t->inst.NumOfItems == 0)
	{
		/* generate a FirstItemPresented event with ItemList 의 header (the first item in the ItemList) 가 window 에 들어올 때 true, 나갈 때 false, 또는 ItemList 가 empty 가 될 때 false */
		data.choice = EventData_boolean;
		data.u.boolean = false;
		MHEGEngine_generateEvent(&t->rootClass.inst.ref, EventType_first_item_presented, &data);

		/* generate a LastItemPresented event with ItemList 의 tail (the last item in the ItemList) 가 window 에 들어올 때 true, 나갈 때 false, 또는 ItemList 가 empty 가 될 때 false */
		data.choice = EventData_boolean;
		data.u.boolean = false;
		MHEGEngine_generateEvent(&t->rootClass.inst.ref, EventType_last_item_presented, &data);
	}

	/* generate a HeadItems event with FirstItem 변경값보다 작은 Item 들의 수, 즉 [1, FirstItem-1] 범위의 Item 들의 수 */
	data.choice = EventData_integer;
	data.u.integer = t->inst.FirstItem - 1;
	MHEGEngine_generateEvent(&t->rootClass.inst.ref, EventType_head_items, &data);

	/* generate a TailItems event with FirstItem 변경값보다 크거나 같은 Item 들의 수, 즉 [FirstItem, NumOfItems] 범위의 Item 들의 수, 또는 ItemList 가 empty 가 될 때 0 */
	data.choice = EventData_integer;
	data.u.integer = t->inst.NumOfItems ? t->inst.NumOfItems - t->inst.FirstItem + 1 : 0;
	MHEGEngine_generateEvent(&t->rootClass.inst.ref, EventType_tail_items, &data);

	return;
}

static unsigned int /* Adjusted */
ListGroupClass_AdjustIndex(ListGroupClass *t, int Index) // hmkim : added.
{
	/* Set the Adjusted parameter to the value of the Index parameter normalised to the length of the ItemList, by applying the following logic:
	   If the length of ItemList is zero then set Adjusted to 1.
	   Otherwise, if Index is greater than the length of ItemList, set Adjusted to: ((Index-1) % Length of ItemList) + 1
	   Otherwise, if Index is less than 1, set Adjusted to: Length of ItemList - ((-Index) % Length of ItemList)
	   Otherwise, set Adjusted to Index, unmodified. */

	if (t->inst.NumOfItems == 0) // 이 경우 ItemList 가 NULL 일 수 있음에 유의. caller 에서 핸들링한다.
		return 1;

	if (Index > t->inst.NumOfItems)
		return ((Index-1) % t->inst.NumOfItems) + 1;

	if (Index < 1)
		return t->inst.NumOfItems - ((-Index) % t->inst.NumOfItems);

	return Index;
}

static void
ListGroupClass_Additem(ListGroupClass *t, int Index, ObjectReference *Item, OctetString *caller_gid) // hmkim : added.
{
	LIST_TYPE(ListGroupItem) *lgi_list;
	LIST_TYPE(ListGroupItem) *new_item;
	bool exist;
	int count;

	/* Execute the following sequence of actions:
	   1. If the Visible referenced by the Item parameter is already in the ItemList, ignore this behaviour.
	   2. If Index is less than 1 or greater than the current length of ItemList + 1, ignore this behaviour.
	   3. Add the Item reference at position Index in the ItemList.
	      (The item previously at position Index in the ItemList - if any - will now have index Index+1 and,
	      similarly, every item of index greater than Index will now have an index incremented by one.)
	   4. If Index is less than or equal to FirstItem and FirstItem is strictly less than the new length of the ItemList, then increment FirstItem by one.
	   5. Apply the Update behaviour. */

	if(!t->rootClass.inst.RunningStatus) // If the RunningStatus of the ListGroup is True then execute Update behaviour
		return;

	if(Index < 1 || Index > t->inst.NumOfItems + 1)
	{
		error("ListGroupClass: Additem: invalid Index %d (%d)", Index, t->inst.NumOfItems);
		return;
	}

	if(MHEGEngine_findObjectReference(Item, caller_gid) != NULL) // 이 시점에서 NULL 일리는 없다. see ListGroupClass_AddItem().
	{
		lgi_list = t->inst.ItemList;
		exist = false;
		while(lgi_list && !exist)
		{
			if(ObjectReference_cmp(&lgi_list->item.a_visible, &t->rootClass.inst.ref.group_identifier, Item, caller_gid) == 0)
			{
				exist = true;
				break;
			}
			lgi_list = lgi_list->next;
		}

		if(!exist)
		{
			if((new_item = safe_malloc(sizeof(LIST_TYPE(ListGroupItem)))) == NULL)
			{
				fatal("Out of memory");
				return;
			}
			ObjectReference_dup(&new_item->item.a_visible, Item);
			new_item->item.ItemSelectionStatus = false;

			lgi_list = t->inst.ItemList;
			count = Index;
			while(lgi_list && count > 1)
			{
				lgi_list = lgi_list->next;
				count--;
			}

			if (lgi_list)
				LIST_INSERT_BEFORE(&t->inst.ItemList, new_item, lgi_list);
			else
				LIST_APPEND(&t->inst.ItemList, new_item);

			t->inst.NumOfItems++;

			if (Index <= t->inst.FirstItem && t->inst.FirstItem < t->inst.NumOfItems)
				t->inst.FirstItem++;

			ListGroupClass_Update(t);
		}
	}

	return;
}

static void
ListGroupClass_Delitem(ListGroupClass *t, ObjectReference *Item, OctetString *caller_gid) // hmkim : added.
{
	LIST_TYPE(ListGroupItem) *lgi_list;
	LIST_TYPE(ListGroupItem) *next;
	RootClass *lgi;
	unsigned int count;

	/* Execute the following sequence of actions:
	   1. If the Visible referenced by the Item parameter is not in the ItemList, ignore this action.
	   2. Remove the reference indicated by the Item parameter from the ItemList.
	      (The item index of the following items in ItemList will decrement by one.)
	   3. Set the position of the referenced Visible to its OriginalPosition.
	   4. If the index of the deleted Item was less than or equal to FirstItem and FirstItem is strictly greater than one, then decrease FirstItem internal attribute by one.
	   5. Apply the Update behaviour. */

	if(!t->rootClass.inst.RunningStatus) // If the RunningStatus of the ListGroup is True then execute Update behaviour
		return;

	lgi_list = t->inst.ItemList;
	count = 1;
	while(lgi_list)
	{
		next = lgi_list->next;

		if(ObjectReference_cmp(&lgi_list->item.a_visible, &t->rootClass.inst.ref.group_identifier, Item, caller_gid) == 0)
		{
			if((lgi = MHEGEngine_findObjectReference(&lgi_list->item.a_visible, &t->rootClass.inst.ref.group_identifier)) != NULL) // 이 시점에서 NULL 일리는 없다. see default_ListGroupClassInstanceVars(), ListGroupClass_Additem()
				VisibleClass_Deactivation(lgi);

			free_ListGroupItem(&lgi_list->item);
			LIST_REMOVE(&t->inst.ItemList, lgi_list);
			safe_free(lgi_list);

			t->inst.NumOfItems--;

			if (count <= t->inst.FirstItem && t->inst.FirstItem > 1)
				t->inst.FirstItem--;

			ListGroupClass_Update(t);
			return;
		}

		lgi_list = next;
		count++;
	}

	return;
}

static void
ListGroupClass_Deselect(ListGroupClass *t, unsigned int Itemlndex) // hmkim : added.
{
	EventData data;

	/* Execute the following sequence of actions:
	   1. If ItemSelectionStatus of item with index Itemlndex is False, ignore this behaviour.
	   2. Set the ItemSelectionStatus of the item with the index ItemIndex to False.
	   3. Generate an ItemDeselected event with Itemlndex as associated data. */

	// 이 시점에 ItemList 는 NULL 일 수 없고 Itemlndex 는 ItemList 내의 유효한 Item 을 가리키게 되므로 여기선 이에 대해 별도 핸들링은 하지 않는다.

	if (t->inst.ItemList[Itemlndex-1].item.ItemSelectionStatus == false) // Itemlndex starting from 1
		return;

	t->inst.ItemList[Itemlndex-1].item.ItemSelectionStatus = false;

	data.choice = EventData_integer;
	data.u.integer = Itemlndex;
	MHEGEngine_generateEvent(&t->rootClass.inst.ref, EventType_item_deselected, &data);

	return;
}

static void
ListGroupClass_Select(ListGroupClass *t, unsigned int Itemlndex) // hmkim : added.
{
	EventData data;
	unsigned int i;

	/* Execute the following sequence of actions:
	   1. If ItemSelectionStatus of item with index Itemlndex is True, ignore this behaviour.
	   2. If MultipleSelection is False, apply Deselect(Index) internal behaviour for any item with index Index, for which the ItemSelectionStatus is True.
	   3. Set the ItemSelectionStatus of the item with index ltemlndex to True.
	   4. Generate an ItemSelected event with Itemlndex as associated data. */

	// 이 시점에 ItemList 는 NULL 일 수 없고 Itemlndex 는 ItemList 내의 유효한 Item 을 가리키게 되므로 여기선 이에 대해 별도 핸들링은 하지 않는다.

	if (t->inst.ItemList[Itemlndex-1].item.ItemSelectionStatus) // Itemlndex starting from 1
		return;

	if (t->multiple_selection == false)
	{
		for (i = 1; i <= t->inst.NumOfItems; i++)
		{
			if (i != Itemlndex && t->inst.ItemList[i-1].item.ItemSelectionStatus)
				ListGroupClass_Deselect(t, i);
		}
	}

	t->inst.ItemList[Itemlndex-1].item.ItemSelectionStatus = true;

	data.choice = EventData_integer;
	data.u.integer = Itemlndex;
	MHEGEngine_generateEvent(&t->rootClass.inst.ref, EventType_item_selected, &data);

	return;
}

void
ListGroupClass_Preparation(ListGroupClass *t)
{
//	verbose_todo("%s not yet implemented", __FUNCTION__);

#if (CLS_VERBOSE)
	verbose_class("ListGroupClass: %s; Preparation", ExternalReference_name(&t->rootClass.inst.ref));
#endif

#if 1 // hmkim : added.

	/* has it already been prepared */
	if(t->rootClass.inst.AvailabilityStatus)
		return;

	/* retrieve the object from an entity outside the engine and set each internal attribute of the object to its initial value */
	default_ListGroupClassInstanceVars(t, &t->inst);

	/* Preparation inherited from the RootClass */
	RootClass_Preparation(&t->rootClass);

#endif

	return;
}

void
ListGroupClass_Activation(ListGroupClass *t)
{
//	verbose_todo("%s not yet implemented", __FUNCTION__);

#if (CLS_VERBOSE)
	verbose_class("ListGroupClass: %s; Activation", ExternalReference_name(&t->rootClass.inst.ref));
#endif

#if 1 // hmkim : added.
{
	EventData data;

	/* has it already been activated */
	if(t->rootClass.inst.RunningStatus)
		return;

	/* has it been prepared yet */
	if(!t->rootClass.inst.AvailabilityStatus)
	{
		/* generates an IsAvailable event */
		ListGroupClass_Preparation(t);
	}

	/* generate a TokenMovedTo event */
	data.choice = EventData_integer;
	data.u.integer = t->inst.TokenPosition;
	MHEGEngine_generateEvent(&t->rootClass.inst.ref, EventType_token_moved_to, &data);

	/* Apply the Update behaviour */
	ListGroupClass_Update(t);

	/* Activation inherited from the RootClass */
	RootClass_Activation(&t->rootClass);
}
#endif

	return;
}

void
ListGroupClass_Deactivation(ListGroupClass *t)
{
//	verbose_todo("%s not yet implemented", __FUNCTION__);

#if (CLS_VERBOSE)
	verbose_class("ListGroupClass: %s; Deactivation", ExternalReference_name(&t->rootClass.inst.ref));
#endif

#if 1 // hmkim : added.
{
	RootClass *lgi;
	unsigned int window_start_array_index, i;
	ListGroupItem *window_item;
	EventData data;

	/* is it already deactivated */
	if(!t->rootClass.inst.RunningStatus)
		return;

	/* Apply the Deactivation behaviour to all active Visibles referenced in the ItemList */
	window_start_array_index = t->inst.FirstItem - 1; // FirstItem starting from 1, in the range [1, length of ItemList]
	if (t->wrap_around)
	{
		for (i = 0; i < t->inst.WindowSize && i < t->inst.NumOfItems; i++)
		{
			window_item = &t->inst.ItemList[(window_start_array_index + i) % t->inst.NumOfItems].item;
			if((lgi = MHEGEngine_findObjectReference(&window_item->a_visible, &t->rootClass.inst.ref.group_identifier)) != NULL) // 이 시점에서 NULL 일리는 없다. see default_ListGroupClassInstanceVars(), ListGroupClass_Additem()
				VisibleClass_Deactivation(lgi);
		}
	}
	else
	{
		for (i = 0; i < t->inst.WindowSize && window_start_array_index + i < t->inst.NumOfItems; i++)
		{
			window_item = &t->inst.ItemList[window_start_array_index + i].item;
			if((lgi = MHEGEngine_findObjectReference(&window_item->a_visible, &t->rootClass.inst.ref.group_identifier)) != NULL) // 이 시점에서 NULL 일리는 없다. see default_ListGroupClassInstanceVars(), ListGroupClass_Additem()
				VisibleClass_Deactivation(lgi);
		}
	}

	/* generate a TokenMovedFrom event */
	data.choice = EventData_integer;
	data.u.integer = t->inst.TokenPosition;
	MHEGEngine_generateEvent(&t->rootClass.inst.ref, EventType_token_moved_from, &data);

	/* Deactivation inherited from the RootClass */
	RootClass_Deactivation(&t->rootClass);
}
#endif

	return;
}

void
ListGroupClass_Destruction(ListGroupClass *t)
{
//	verbose_todo("%s not yet implemented", __FUNCTION__);

#if (CLS_VERBOSE)
	verbose_class("ListGroupClass: %s; Destruction", ExternalReference_name(&t->rootClass.inst.ref));
#endif

#if 1 // hmkim : added.

	/* is it already destroyed */
	if(!t->rootClass.inst.AvailabilityStatus)
		return;

	/* Reset all available Visibles of the ListGroup to their OriginalPosition -> ListGroupClass_Deactivation() 에서 수행되므로 여기서는 할 게 없음 */
	/* 여기서 말하는 OriginalPosition 이라는 것이 ListGroup class exchanged attribute 인 Positions (UK MHEG Profile (D-BOOK 5.0) 에서는 이것을 OriginalPositions 이라고 정의한다) 을 의미하는 것인지
	   아니면 각 Visible class 의 OriginalPosition 을 의미하는 것인지 확인 필요함. 일단, 각 Visible class 의 OriginalPosition 을 의미하는 것으로 봐도 무방할 듯... see VisibleClass_Deactivation(). */

	/* Deactivate it if it is running */
	if(t->rootClass.inst.RunningStatus)
	{
		/* generates an IsStopped event */
		ListGroupClass_Deactivation(t);
	}

	/*
	 * spec says we should handle caching here
	 * rb-download caches everything
	 */

	free_ListGroupClassInstanceVars(&t->inst);

	/* Destruction inherited from the RootClass */
	RootClass_Destruction(&t->rootClass);

#endif

	return;
}

void
ListGroupClass_Clone(ListGroupClass *t, Clone *params, OctetString *caller_gid)
{
	verbose_todo("%s not yet implemented", __FUNCTION__);

	verbose_class("ListGroupClass: %s; Clone", ExternalReference_name(&t->rootClass.inst.ref));

	return;
}

void
ListGroupClass_Move(ListGroupClass *t, Move *params, OctetString *caller_gid) /* TokenManagerClass */
{
//	verbose_todo("%s not yet implemented", __FUNCTION__);

#if 1 // hmkim : added.
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
		error("ListGroupClass: Move: invalid movement ID: %u", movement_id);
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
		error("ListGroupClass: Move: movement list %u is too short", movement_id);
		return;
	}

	/* get the new TokenPosition */
	target = (unsigned int) target_list->item;

	/* has it moved */
	/* If the TargetElement does not have the token yet, apply the TransferToken(TargetElement) behaviour of the TokenManager. */
	verbose_class("ListGroupClass: %s; Move; %u; %u -> %u; WindowSize %d", ExternalReference_name(&t->rootClass.inst.ref), movement_id, t->inst.TokenPosition, target, t->inst.WindowSize);
	if(t->inst.TokenPosition != target)
		ListGroupClass_TransferToken(t, target);
}
#endif

	return;
}

void
ListGroupClass_MoveTo(ListGroupClass *t, MoveTo *params, OctetString *caller_gid) /* TokenManagerClass */
{
//	verbose_todo("%s not yet implemented", __FUNCTION__);

#if 1 // hmkim : added.
{
	unsigned int index = (unsigned int) GenericInteger_getInteger(&params->index, caller_gid);

	verbose_class("ListGroupClass: %s; MoveTo; %u -> %u; WindowSize %d", ExternalReference_name(&t->rootClass.inst.ref), t->inst.TokenPosition, index, t->inst.WindowSize);

	if(index == 0)
		return;

	/* has it moved */
	/* If the TargetElement does not have the token yet, apply the TransferToken(TargetElement) behaviour of the TokenManager. */
	if(t->inst.TokenPosition != index)
		ListGroupClass_TransferToken(t, index);
}
#endif

	return;
}

void
ListGroupClass_GetTokenPosition(ListGroupClass *t, GetTokenPosition *params, OctetString *caller_gid) /* TokenManagerClass */
{
//	verbose_todo("%s not yet implemented", __FUNCTION__);

#if 1 // hmkim : added
{
	VariableClass *var;

	verbose_class("ListGroupClass: %s; GetTokenPosition", ExternalReference_name(&t->rootClass.inst.ref));

	if((var = (VariableClass *) MHEGEngine_findObjectReference(&params->token_position_var, caller_gid)) == NULL)
		return;

	if(var->rootClass.inst.rtti != RTTI_VariableClass
	|| VariableClass_type(var) != OriginalValue_integer)
	{
		error("ListGroupClass: GetTokenPosition: type mismatch (%d, %d)", var->rootClass.inst.rtti, VariableClass_type(var));
		return;
	}

	IntegerVariableClass_setInteger(var, t->inst.TokenPosition);
}
#endif

	return;
}

void
ListGroupClass_CallActionSlot(ListGroupClass *t, CallActionSlot *params, OctetString *caller_gid) /* TokenGroupClass */
{
//	verbose_todo("%s not yet implemented", __FUNCTION__);

#if 1 // hmkim : added.
{
	LIST_OF(ActionSlot) *action_slots;
	LIST_TYPE(ActionSlot) *action;
	LIST_TYPE(TokenGroupItem) *tgi;
	unsigned int i, index = (unsigned int) GenericInteger_getInteger(&params->index, caller_gid);

	verbose_class("ListGroupClass: %s; CallActionSlot; index %u; TokenPosition %u", ExternalReference_name(&t->rootClass.inst.ref), index, t->inst.TokenPosition);

	if(index == 0)
	{
		verbose_class("ListGroupClass: CallActionSlot: index=0");
		return;
	}

	/* if no-one has the token, use the no_token_action_slots */
	if(t->inst.TokenPosition == 0)
	{
		action_slots = t->no_token_action_slots;
	}
	else
	{
		/* use the action_slots from the TokenGroupItem that has the token */ /* For ListGroup, ActionSlots are attached to the cell position and not a Visible */
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
			error("ListGroupClass: CallActionSlot: invalid TokenPosition: %u", t->inst.TokenPosition);
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
		error("ListGroupClass: CallActionSlot: invalid ActionSlot: %u", GenericInteger_getInteger(&params->index, caller_gid));
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
		MHEGEngine_addToTempActionQ(&action->item.u.action_class, &t->rootClass.inst.ref);
}
#endif

	return;
}

void
ListGroupClass_AddItem(ListGroupClass *t, AddItem *params, OctetString *caller_gid)
{
//	verbose_todo("%s not yet implemented", __FUNCTION__);

#if 1 // hmkim : added.
{
	int ItemIndex = GenericInteger_getInteger(&params->item_index, caller_gid);
	ObjectReference *VisibleReference = GenericObjectReference_getObjectReference(&params->visible_reference, caller_gid);

	verbose_class("ListGroupClass: %s; AddItem; %d; %s", ExternalReference_name(&t->rootClass.inst.ref), ItemIndex, VisibleReference ? ObjectReference_name(VisibleReference) : "NULL");

	/* Apply the Additem(ItemIndex, VisibleReference) internal behaviour to add the reference to the visible,
	   as indicated by the VisibleReference parameter at the position indicated by the ItemIndex parameter.
	   VisibleReference shall refer to an available Visible object, which is not referred to by any reference in the ItemList. */

	if (VisibleReference)
	{
		ListGroupClass_Additem(t, ItemIndex, VisibleReference, caller_gid);
	}
	else
	{
		error("ListGroupClass: AddItem: NULL VisibleReference");
	}
}
#endif

	return;
}

void
ListGroupClass_DelItem(ListGroupClass *t, DelItem *params, OctetString *caller_gid)
{
//	verbose_todo("%s not yet implemented", __FUNCTION__);

#if 1 // hmkim : added.
{
	ObjectReference *VisibleReference = GenericObjectReference_getObjectReference(&params->visible_reference, caller_gid);

	verbose_class("ListGroupClass: %s; DelItem; %s", ExternalReference_name(&t->rootClass.inst.ref), VisibleReference ? ObjectReference_name(VisibleReference) : "NULL");

	/* Apply the Delitem(VisibleReference) internal behaviour to delete the reference to the visible,
	   as indicated by the VisibleReference parameter from the ItemList attribute, if it occurs in it. */

	if (VisibleReference)
	{
		ListGroupClass_Delitem(t, VisibleReference, caller_gid);
	}
	else
	{
		error("ListGroupClass: DelItem: NULL VisibleReference");
	}
}
#endif

	return;
}

void
ListGroupClass_GetListItem(ListGroupClass *t, GetListItem *params, OctetString *caller_gid)
{
//	verbose_todo("%s not yet implemented", __FUNCTION__);

#if 1 // hmkim : added.
{
	int ItemIndex = GenericInteger_getInteger(&params->item_index, caller_gid);
	unsigned int Index;
	VariableClass *var;

	verbose_class("ListGroupClass: %s; GetListItem; ItemIndex %d; NumOfItems %d", ExternalReference_name(&t->rootClass.inst.ref), ItemIndex, t->inst.NumOfItems);

	/* Using a local variable Index:
	   If WrapAround is False, if ItemIndex is in the range [1, length of ItemList] then set Index to ItemIndex, otherwise ignore this action.
	   If WrapAround is True, apply the AdjustIndex(ItemIndex,Index) behaviour.
	   Return the reference included in the ItemList attribute with the index specified by the Index parameter in the ObjectRefVariable referenced by ItemRefVar.
	   ItemRefVar shall refer to an active ObjectRefVariable object. */

	if (t->inst.ItemList == NULL || t->inst.NumOfItems == 0) // Item 이 없는 경우에 대한 핸들링
	{
		error("ListGroupClass: GetListItem: no item");
		return;
	}

	if (t->wrap_around)
	{
		Index = ListGroupClass_AdjustIndex(t, ItemIndex);
	}
	else
	{
		if (ItemIndex < 1 || ItemIndex > t->inst.NumOfItems)
		{
			error("ListGroupClass: GetListItem: invalid ItemIndex %d (%d)", ItemIndex, t->inst.NumOfItems);
			return;
		}
		Index = ItemIndex;
	}

	if((var = (VariableClass *) MHEGEngine_findObjectReference(&params->item_ref_var, caller_gid)) == NULL)
		return;

	if(var->rootClass.inst.rtti != RTTI_VariableClass
	|| VariableClass_type(var) != OriginalValue_object_reference)
	{
		error("ListGroupClass: GetListItem: type mismatch (%d, %d)", var->rootClass.inst.rtti, VariableClass_type(var));
		return;
	}

	ObjectRefVariableClass_setObjectReference(var, &t->inst.ItemList[Index-1].item.a_visible);
}
#endif

	return;
}

void
ListGroupClass_GetCellItem(ListGroupClass *t, GetCellItem *params, OctetString *caller_gid)
{
//	verbose_todo("%s not yet implemented", __FUNCTION__);

#if 1 // hmkim : added.
{
	int CellIndex = GenericInteger_getInteger(&params->cell_index, caller_gid);
	unsigned int ItemIndex;
	VariableClass *var;

	verbose_class("ListGroupClass: %s; GetCellItem; %d", ExternalReference_name(&t->rootClass.inst.ref), CellIndex);

	/* Return the reference of the visible presented at the cell indicated by the Celllndex parameter in the ObjectRefVariable referenced by ItemRefVar.
	   If the CellIndex specifies an index smaller than or equal to 1 return the reference of the visible presented at the first cell.
	   If the CellIndex specifies an index greater than or equal to the number of cells, return the reference of the visible presented in the last cell.
	   If no visible is presented at the indicated cell, return NULL.
	   NOTE - the GetTokenPosition action can be used to get the index of the cell which currently holds the token, as defined in the base class.
	   ItemRefVar shall refer to an active ObjectRefVariable object. */

	if (t->inst.ItemList == NULL || t->inst.NumOfItems == 0) // Item 이 없는 경우에 대한 핸들링
	{
		error("ListGroupClass: GetCellItem: no item");
		return;
	}

	if (t->inst.Positions == NULL || t->inst.WindowSize == 0) // Cell 이 없는 경우에 대한 핸들링
	{
		error("ListGroupClass: GetCellItem: no cell");
		return;
	}

	if (CellIndex <= 1) /* case 1 */
	{
		ItemIndex = t->inst.FirstItem;
	}
	else if (CellIndex >= t->inst.WindowSize) /* case 2 */
	{
		ItemIndex = t->inst.FirstItem + t->inst.WindowSize - 1;
	}
	else
	{
		ItemIndex = t->inst.FirstItem + CellIndex - 1;
	}

	if (t->wrap_around)
	{
		if (ItemIndex > t->inst.NumOfItems)
			ItemIndex = ((ItemIndex-1) % t->inst.NumOfItems) + 1;
	}
	else
	{
		if (ItemIndex > t->inst.NumOfItems) // If no visible is presented at the indicated cell, return NULL <- 근데 NULL 을 리턴한다는 것이 무엇인가? 일단 아래와 같이 처리함. 확인 필요...
			return;
	}

	if((var = (VariableClass *) MHEGEngine_findObjectReference(&params->item_ref_var, caller_gid)) == NULL)
		return;

	if(var->rootClass.inst.rtti != RTTI_VariableClass
	|| VariableClass_type(var) != OriginalValue_object_reference)
	{
		error("ListGroupClass: GetCellItem: type mismatch (%d, %d)", var->rootClass.inst.rtti, VariableClass_type(var));
		return;
	}

	ObjectRefVariableClass_setObjectReference(var, &t->inst.ItemList[ItemIndex-1].item.a_visible);
}
#endif

	return;
}

void
ListGroupClass_GetItemStatus(ListGroupClass *t, GetItemStatus *params, OctetString *caller_gid)
{
//	verbose_todo("%s not yet implemented", __FUNCTION__);

#if 1 // hmkim : added.
{
	int ItemIndex = GenericInteger_getInteger(&params->item_index, caller_gid);
	unsigned int Index;
	VariableClass *var;

	verbose_class("ListGroupClass: %s; GetItemStatus; ItemIndex %d; NumOfItems %d", ExternalReference_name(&t->rootClass.inst.ref), ItemIndex, t->inst.NumOfItems);

	/* Using a local variable Index:
	   If WrapAround is False, if ItemIndex is in the range [1, length of ItemList] then set Index to ItemIndex, otherwise ignore this action.
	   If WrapAround is True, apply the AdjustIndex(ItemIndex,Index) behaviour.
	   Return the value of the ItemSelectionStatus attribute of the item in the ItemList with index Index in the BooleanVariable referenced by ItemStatusVar.
	   ItemRefVar shall refer to an active BooleanVariable object. */

	if (t->inst.ItemList == NULL || t->inst.NumOfItems == 0) // Item 이 없는 경우에 대한 핸들링
	{
		error("ListGroupClass: GetItemStatus: no item");
		return;
	}

	if (t->wrap_around)
	{
		Index = ListGroupClass_AdjustIndex(t, ItemIndex);
	}
	else
	{
		if (ItemIndex < 1 || ItemIndex > t->inst.NumOfItems)
		{
			error("ListGroupClass: GetItemStatus: invalid ItemIndex %d (%d)", ItemIndex, t->inst.NumOfItems);
			return;
		}
		Index = ItemIndex;
	}

	if((var = (VariableClass *) MHEGEngine_findObjectReference(&params->item_status_var, caller_gid)) == NULL)
		return;

	if(var->rootClass.inst.rtti != RTTI_VariableClass
	|| VariableClass_type(var) != OriginalValue_boolean)
	{
		error("ListGroupClass: GetItemStatus: type mismatch (%d, %d)", var->rootClass.inst.rtti, VariableClass_type(var));
		return;
	}

	BooleanVariableClass_setBoolean(var, t->inst.ItemList[Index-1].item.ItemSelectionStatus);
}
#endif

	return;
}

void
ListGroupClass_DeselectItem(ListGroupClass *t, DeselectItem *params, OctetString *caller_gid)
{
//	verbose_todo("%s not yet implemented", __FUNCTION__);

#if 1 // hmkim : added.
{
	int ItemIndex = GenericInteger_getInteger(&params->item_index, caller_gid);
	unsigned int Index;

	verbose_class("ListGroupClass: %s; DeselectItem; ItemIndex %d; NumOfItems %d", ExternalReference_name(&t->rootClass.inst.ref), ItemIndex, t->inst.NumOfItems);

	/* Using a local variable Index:
	   If WrapAround is False, if ItemIndex is in the range [1, length of ItemList] then set Index to ItemIndex, otherwise ignore this action.
	   If WrapAround is True, apply the AdjustIndex(ItemIndex,Index) behaviour.
	   Apply the Deselect(Index) internal behaviour. */

	if (t->inst.ItemList == NULL || t->inst.NumOfItems == 0) // Item 이 없는 경우에 대한 핸들링
	{
		error("ListGroupClass: DeselectItem: no item");
		return;
	}

	if (t->wrap_around)
	{
		Index = ListGroupClass_AdjustIndex(t, ItemIndex);
	}
	else
	{
		if (ItemIndex < 1 || ItemIndex > t->inst.NumOfItems)
		{
			error("ListGroupClass: DeselectItem: invalid ItemIndex %d (%d)", ItemIndex, t->inst.NumOfItems);
			return;
		}
		Index = ItemIndex;
	}

	ListGroupClass_Deselect(t, Index);
}
#endif

	return;
}

void
ListGroupClass_SelectItem(ListGroupClass *t, SelectItem *params, OctetString *caller_gid)
{
//	verbose_todo("%s not yet implemented", __FUNCTION__);

#if 1 // hmkim : added.
{
	int ItemIndex = GenericInteger_getInteger(&params->item_index, caller_gid);
	unsigned int Index;

	verbose_class("ListGroupClass: %s; SelectItem; ItemIndex %d; NumOfItems %d", ExternalReference_name(&t->rootClass.inst.ref), ItemIndex, t->inst.NumOfItems);

	/* Using a local variable Index:
	   If WrapAround is False, if ItemIndex is in the range [1, length of ItemList] then set Index to ItemIndex, otherwise ignore this action.
	   If WrapAround is True, apply the AdjustIndex(ItemIndex,Index) behaviour.
	   Apply the Select(Index) internal behaviour. */

	if (t->inst.ItemList == NULL || t->inst.NumOfItems == 0) // Item 이 없는 경우에 대한 핸들링
	{
		error("ListGroupClass: SelectItem: no item");
		return;
	}

	if (t->wrap_around)
	{
		Index = ListGroupClass_AdjustIndex(t, ItemIndex);
	}
	else
	{
		if (ItemIndex < 1 || ItemIndex > t->inst.NumOfItems)
		{
			error("ListGroupClass: SelectItem: invalid ItemIndex %d (%d)", ItemIndex, t->inst.NumOfItems);
			return;
		}
		Index = ItemIndex;
	}

	ListGroupClass_Select(t, Index);
}
#endif

	return;
}

void
ListGroupClass_ToggleItem(ListGroupClass *t, ToggleItem *params, OctetString *caller_gid)
{
//	verbose_todo("%s not yet implemented", __FUNCTION__);

#if 1 // hmkim : added.
{
	int ItemIndex = GenericInteger_getInteger(&params->item_index, caller_gid);
	unsigned int Index;

	verbose_class("ListGroupClass: %s; ToggleItem; ItemIndex %d; NumOfItems %d", ExternalReference_name(&t->rootClass.inst.ref), ItemIndex, t->inst.NumOfItems);

	/* Using a local variable Index:
	   If WrapAround is False, if ItemIndex is in the range [1, length of ItemList] then set Index to ItemIndex, otherwise ignore this action.
	   If WrapAround is True, apply the AdjustIndex(ItemIndex,Index) behaviour.
	   If the ItemSelectionStatus of the item indicated by Index is True, apply the Deselect(Index) internal behaviour, otherwise apply the Select(Index) internal behaviour. */

	if (t->inst.ItemList == NULL || t->inst.NumOfItems == 0) // Item 이 없는 경우에 대한 핸들링
	{
		error("ListGroupClass: ToggleItem: no item");
		return;
	}

	if (t->wrap_around)
	{
		Index = ListGroupClass_AdjustIndex(t, ItemIndex);
	}
	else
	{
		if (ItemIndex < 1 || ItemIndex > t->inst.NumOfItems)
		{
			error("ListGroupClass: ToggleItem: invalid ItemIndex %d (%d)", ItemIndex, t->inst.NumOfItems);
			return;
		}
		Index = ItemIndex;
	}

	if (t->inst.ItemList[Index-1].item.ItemSelectionStatus)
	{
		ListGroupClass_Deselect(t, Index);
	}
	else
	{
		ListGroupClass_Select(t, Index);
	}
}
#endif

	return;
}

void
ListGroupClass_ScrollItems(ListGroupClass *t, ScrollItems *params, OctetString *caller_gid)
{
//	verbose_todo("%s not yet implemented", __FUNCTION__);

#if 1 // hmkim : added.
{
	int ItemsToScroll = GenericInteger_getInteger(&params->items_to_scroll, caller_gid);
	unsigned int FirstItem;

	verbose_class("ListGroupClass: %s; ScrollItems; ItemsToScroll %d; FirstItem %d; NumOfItems %d", ExternalReference_name(&t->rootClass.inst.ref), ItemsToScroll, t->inst.FirstItem, t->inst.NumOfItems);

	/* If WrapAround is False, if FirstItem + ItemsToScroll is in the range [1, length of ItemList] then set FirstItem to FirstItem + ItemsToScroll, otherwise ignore this action.
	   If WrapAround is True, apply the AdjustIndex(FirstItem+ItemsToScroll,FirstItem) behaviour.
	   Apply the Update behaviour. */

	if(!t->rootClass.inst.RunningStatus) // If the RunningStatus of the ListGroup is True then execute Update behaviour
		return;

	if (t->inst.ItemList == NULL || t->inst.NumOfItems == 0) // Item 이 없는 경우에 대한 핸들링
	{
		error("ListGroupClass: ScrollItems: no item");
		return;
	}

	if (t->wrap_around)
	{
		FirstItem = ListGroupClass_AdjustIndex(t, t->inst.FirstItem + ItemsToScroll);
	}
	else
	{
		if (t->inst.FirstItem + ItemsToScroll < 1 || t->inst.FirstItem + ItemsToScroll > t->inst.NumOfItems)
		{
			error("ListGroupClass: ScrollItems: invalid FirstItem %d + ItemsToScroll %d (%d)", t->inst.FirstItem, ItemsToScroll, t->inst.NumOfItems);
			return;
		}
		FirstItem = t->inst.FirstItem + ItemsToScroll;
	}

	if (t->inst.FirstItem != FirstItem) // 이 조건 체크는 스펙에 없는 내용이지만 조건 체크를 하는 것이 바람직한 것이라 생각한다.
	{
		t->inst.FirstItem = FirstItem;
		ListGroupClass_Update(t);
	}
}
#endif

	return;
}

void
ListGroupClass_SetFirstItem(ListGroupClass *t, SetFirstItem *params, OctetString *caller_gid)
{
//	verbose_todo("%s not yet implemented", __FUNCTION__);

#if 1 // hmkim : added.
{
	int NewFirstItem = GenericInteger_getInteger(&params->new_first_item, caller_gid);
	unsigned int FirstItem;

	verbose_class("ListGroupClass: %s; SetFirstItem; NewFirstItem %d; CurFirstItem %d; NumOfItems %d", ExternalReference_name(&t->rootClass.inst.ref), NewFirstItem, t->inst.FirstItem, t->inst.NumOfItems);

	/* If WrapAround is False, if NewFirstItem is in the range [1, length of ItemList] then set FirstItem to NewFirstItem, otherwise ignore this action.
	   If WrapAround is True, apply the AdjustIndex(NewFirstItem,FirstItem) behaviour.
	   Apply the Update behaviour. */

	if(!t->rootClass.inst.RunningStatus) // If the RunningStatus of the ListGroup is True then execute Update behaviour
		return;

	if (t->inst.ItemList == NULL || t->inst.NumOfItems == 0) // Item 이 없는 경우에 대한 핸들링
	{
		error("ListGroupClass: SetFirstItem: no item");
		return;
	}

	if (t->wrap_around)
	{
		FirstItem = ListGroupClass_AdjustIndex(t, NewFirstItem);
	}
	else
	{
		if (NewFirstItem < 1 || NewFirstItem > t->inst.NumOfItems)
		{
			error("ListGroupClass: SetFirstItem: invalid NewFirstItem %d (%d)", NewFirstItem, t->inst.NumOfItems);
			return;
		}
		FirstItem = NewFirstItem;
	}

	if (t->inst.FirstItem != FirstItem) // 이 조건 체크는 스펙에 없는 내용이지만 조건 체크를 하는 것이 바람직한 것이라 생각한다.
	{
		t->inst.FirstItem = FirstItem;
		ListGroupClass_Update(t);
	}
}
#endif

	return;
}

void
ListGroupClass_GetFirstItem(ListGroupClass *t, GetFirstItem *params, OctetString *caller_gid)
{
//	verbose_todo("%s not yet implemented", __FUNCTION__);

#if 1 // hmkim : added.
{
	VariableClass *var;

	verbose_class("ListGroupClass: %s; GetFirstItem; %d", ExternalReference_name(&t->rootClass.inst.ref), t->inst.FirstItem);

	/* Return the current value of the FirstItem attribute in the IntegerVariable referenced by FirstItemVar.
	   FirstItemVar shall refer to an active IntegerVariable object. */

	if((var = (VariableClass *) MHEGEngine_findObjectReference(&params->first_item_var, caller_gid)) == NULL)
		return;

	if(var->rootClass.inst.rtti != RTTI_VariableClass
	|| VariableClass_type(var) != OriginalValue_integer)
	{
		error("ListGroupClass: GetFirstItem: type mismatch (%d, %d)", var->rootClass.inst.rtti, VariableClass_type(var));
		return;
	}

	IntegerVariableClass_setInteger(var, t->inst.FirstItem);
}
#endif

	return;
}

void
ListGroupClass_GetListSize(ListGroupClass *t, GetListSize *params, OctetString *caller_gid)
{
//	verbose_todo("%s not yet implemented", __FUNCTION__);

#if 1 // hmkim : added.
{
	VariableClass *var;

	verbose_class("ListGroupClass: %s; GetListSize; %d", ExternalReference_name(&t->rootClass.inst.ref), t->inst.NumOfItems);

	/* Return the number of items in the ItemList in the IntegerVariable referenced by SizeVar.
	   SizeVar shall refer to an active IntegerVariable object. */

	if((var = (VariableClass *) MHEGEngine_findObjectReference(&params->size_var, caller_gid)) == NULL)
		return;

	if(var->rootClass.inst.rtti != RTTI_VariableClass
	|| VariableClass_type(var) != OriginalValue_integer)
	{
		error("ListGroupClass: GetListSize: type mismatch (%d, %d)", var->rootClass.inst.rtti, VariableClass_type(var));
		return;
	}

	IntegerVariableClass_setInteger(var, t->inst.NumOfItems);
}
#endif

	return;
}

void
ListGroupClass_SetCellPosition(ListGroupClass *t, SetCellPosition *params, OctetString *caller_gid)
{
//	verbose_todo("%s not yet implemented", __FUNCTION__);

#if 1 // hmkim : added.
{
	int CellIndex = GenericInteger_getInteger(&params->index, caller_gid);
	int NewXPosition = GenericInteger_getInteger(&params->new_x_position, caller_gid);
	int NewYPosition = GenericInteger_getInteger(&params->new_y_position, caller_gid);
	int count;
	LIST_TYPE(XYPosition) *Position;

	verbose_class("ListGroupClass: %s; SetCellPosition; %d; (%d, %d)", ExternalReference_name(&t->rootClass.inst.ref), CellIndex, NewXPosition, NewYPosition);

	/* A cell is identified by its (one based) index.
	   1. If the CellIndex specifies an index smaller than or equal to 1 then the position of the first cell is changed.
	   2. If the CellIndex specifies an index greater than or equal to the number of cells then the position of the last cell is changed. */

	// 아래와 같이 하지 않고 WindowSize 를 이용하여 심플하게 계산할 수도 있을 것이다.

	if (CellIndex <= 1) /* case 1 */
	{
		Position = t->inst.Positions;
	}
	else
	{
		Position = t->inst.Positions;
		count = CellIndex;
		while(count > 1)
		{
			if (Position->next == NULL) /* case 2 */
				break;

			Position = Position->next;
			count --;
		}
	}

	if (Position == NULL)
	{
		error("ListGroupClass: SetCellPosition: CellIndex %d has no CellPosition (something wrong)", CellIndex);
		return;
	}

	Position->item.x_position = NewXPosition;
	Position->item.y_position = NewYPosition;
}
#endif

	return;
}


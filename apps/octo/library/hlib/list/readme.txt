HList 사용예

HxList_t *list = NULL;							// HxList

HxList_t *tmp  = NULL;

CustomData * data1 = (CustomData*)malloc(sizeof(CustomData));		// 저장 데이터
CustomData * data2 = (CustomData*)malloc(sizeof(CustomData));
CustomData * data3 = (CustomData*)malloc(sizeof(CustomData));
CustomData * data4 = (CustomData*)malloc(sizeof(CustomData));
CustomData * data5 = (CustomData*)malloc(sizeof(CustomData));

list = HLIB_LIST_Append(list,data1);					// insert to list
list = HLIB_LIST_Append(list,data2);
list = HLIB_LIST_Append(list,data3);
list = HLIB_LIST_Append(list,data4);
list = HxList_insert(list,data5,3);

...

tmp = list;

for(int i=0; i < HLIB_LIST_Length(list); i++)
{
	...
	tmp = tmp->next;						// used data
	...
}


HLIB_LIST_RemoveAll(list);						// remove all data in list

...
// CustomData remove

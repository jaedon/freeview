#include<stdio.h>
#include<string.h>
#include<stdlib.h>
#include<assert.h>
#include "htree.h"

#define HPOINTER_TO_INT(p)    ((HINT32)   (p))

#define HINT32_TO_POINTER(i)    ((hpointer)  (i))


static HINT32 my_compare(hconstpointer a,hconstpointer b)
{
	HINT32 _a;
	HINT32 _b;

	_a = HPOINTER_TO_INT(a);
	_b = HPOINTER_TO_INT(b);
	return _a - _b;
}

static HINT32 my_traverse(hpointer key, hpointer value, hpointer data)
{
	HINT32 _key;
	HINT32 _value;
	
	_key = HPOINTER_TO_INT(key);
	_value = HPOINTER_TO_INT(value);

	printf("key (%d) - value (%d)\n", _key,_value);

	return FALSE;
}

int main()
{
	HTree *tree;

	HINT32 i;

	tree = h_tree_new(my_compare);

	for( i=0;i<20;++i)
	{
		HINT32 key,value;		
		key = i;
		value = 10*i;
		h_tree_insert(tree,HINT32_TO_POINTER(key),HINT32_TO_POINTER(value));
	}
	h_tree_foreach(tree,my_traverse,NULL);
	h_tree_destroy(tree);
	return 0;
}
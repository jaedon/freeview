STATIC HUINT32 my_hash(const void *key)
{
	return (HUINT32) *((const HINT32*) key);
}

STATIC HBOOL my_hash_equal(const void *a , const void *b)
{
	return *((const HINT32*) a) == *((const HINT32*)b);
}

STATIC void my_hash_print(void *key , void *value, void *userdata)
{
	char * pKey = ((char*)(key));
	char * pValue = ((char*)(value));

	printf("key   :  %s ,    value  : %s \n",pKey,pValue);
}

int main()
{

	HxHASH_t *root = HLIB_HASH_New(my_hash,my_hash_equal);

	HLIB_HASH_Insert(root,"data1","myname");
	HLIB_HASH_Insert(root,"data2","mybasehome");

	printf("hash size :  %d\n",HLIB_HASH_Size(root));

	HLIB_HASH_Foreach(root,my_hash_print,NULL);

	HLIB_HASH_RemoveAll(root);

	return 0;
}

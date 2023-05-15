#include <stdio.h>

typedef void(*cbTest)( int );


static cbTest g_cb = NULL;

void registCb( cbTest cbFn )
{
	printf( "[C API] registCb Success!!\n");
	g_cb = cbFn;

}

void testMsg( char *msg )
{
	if( msg )
	{
		printf( "[C API] testMsg : %s\n", msg );
	}
	else
	{
		printf( "[C API] testMsg is null\n" );
	}
}

void useTestCb( int nCount )
{
	int i = 0;
	printf( "[C API] useTestcb( repeat count = %d )\n", nCount );
	if( g_cb )
	{
		for( i = 0 ; i < nCount; ++i )
		{
			g_cb( i );
		}
	}
}

#include <stdio.h>
#include "framework.h"

int app_main( int argc, char **argv )
{
	BSystem_Info sysInfo;
	BFramework_Info frmInfo;

	int iErr = BSystem_Init( argc, argv, &sysInfo );
	if ( iErr )
	{
		printf( "System init FAILED!\n" );
		return iErr;
	}

	iErr = BFramework_Init( &sysInfo, &frmInfo );
	if ( iErr )
	{
		printf( "Framework init FAILED!\n" );
		return iErr;
	}


	printf( "Do some settop stuff...\n" );


	BFramework_Uninit( &frmInfo );
	BSystem_Uninit( &sysInfo );

	return 0;
}


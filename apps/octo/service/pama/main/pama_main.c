/**************************************************************
 *	@file		PowerManager.c
 *	Power Manager Application for OCTO 2.0
 *
 *	http://www.humaxdigital.com
 *
 *	@data			2012/03/07
 *	@author			humax
 *	@brief			Power Manager Application
 **************************************************************/

/*
	������ POWER���� ����, �� Shutdown���� ����� OCTO��� �����Ͽ�
	�ش� �ڵ带 �� �����Ѵ�. (2012/07/08)
*/

#include	<vkernel.h>
#include	<hlib.h>

/**************************************************************
	internal structures and variables
 **************************************************************/

/**************************************************************
	global functions
 **************************************************************/
extern int		PAMA_Main(int argc, char **argv);

/**************************************************************
	private functions
 **************************************************************/

#if !defined(CONFIG_SINGLE_PROCESS)
int		main(int argc, char **argv)
{
	PAMA_Main(argc, argv);

	while (1)
		HLIB_STD_TaskSleep(10000000);

	return 0;
}
#endif


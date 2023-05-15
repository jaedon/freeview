
/* SVC ���� ��� include */
#include <svc_common.h>


/*
	��� plug-in ����� include ��.
	��� plug-in ����� makefile�� path ������ �ȵǾ� �����Ƿ� ��� path�� include �ؾ� ��.
*/
#include "psvc_xxx.h"


/*
	�Ļ� ��� plug-in body ��� include ��.(�Ļ� ��� ���� ���� ���)
	�Ļ� ��� plug-in ����� makefile�� path ������ �ȵǾ� �����Ƿ� ��� path�� include �ؾ� ��.
*/
#if defined(CONFIG_OP_111)
	#include "./xxx/include/_xsvc_xxx_111.h"
#elif defined(CONFIG_OP_2222)
	#include "./xxx/include/_xsvc_xxx_222.h"
#elif defined(CONFIG_PROD_3)
	#include "./xxx/include/_xsvc_xxx_333.h"
#else
	#error "CONFIG_OP_XXX or CONFIG_PROD_XXX should be defined."
#endif


/* ��� typedef

	Svc : �빮�ڷ� �����ϴ� layer ��	(Psvc : X, Svc : O)
	Xxx : �빮�ڷ� �����ϴ� Module ��
	Type : �빮�ڷ� �����ϴ� type ��. Common convention �� ������.
	_t/e/b : type ����. Common convention�� ������.
*/
typedef int SvcXxx_Type_t; /* SvcXxx_Type_e, SvcXxx_Type_b */
typedef (void *)SvcXxx_DoSomethingCb_t(void);


/* ��� plug-in API

	psvc_ : Plug-in ǥ�� p + �ҹ��ڷ� layer �� ǥ��
	xxx_ : �ҹ��ڷ� module �� ǥ��
	Function Name : �������� ���� ���� + ������ �������� �빮�ڷ� �����ϴ� �ܾ �̾� ����
	
	���� :
		SVC ��� ���ο��� �Ļ��� ���� option�� ������ ��� ȣ���ϴ� API.
		Proto type�� base�� ��� internal include�� ����ǰ� body�� �Ļ� ��⿡�� ������.

	���ǻ��� :
		�ݵ�� base ��⿡���� ȣ��Ǿ�� ��.
*/
HERROR psvc_xxx_DoSomething( ... )
{
#if defined(CONFIG_OP_111)
	xsvc_xxx_DoSomething_111();
#elif defined(CONFIG_OP_222)
	xsvc_xxx_DoSomething_222();
#elif defined(CONFIG_OP_333)
	xsvc_xxx_DoSomething_333();
#else
	#error "CONFIG_OP_XXX or CONFIG_PROD_XXX should be defined."
#endif
}






/* SVC ���� ��� include */
#include <svc_common.h>



/* ����ϴ� SVC ����� ��� include */
#include <svc_111.h>
#include <svc_222.h>


/* MGR ���� ��� include */
#include <mgr_common.h>
#include <om_common.h>




/* ��� plug-in ����� include �� */
#include "../om/include_plugin/pom_xxx.h"




/*
	�Ļ� ��� plug-in body ��� include ��.(�Ļ� ��� ���� ���� ���)
	�Ļ� ��� plug-in ����� makefile�� path ������ �ȵǾ� �����Ƿ� ��� path�� include �ؾ� ��.
*/
#if defined(CONFIG_OP_111)
	#include "./xxx/include/_xom_xxx_111.h"
#elif defined(CONFIG_OP_2222)
	#include "./xxx/include/_xom_xxx_222.h"
#elif defined(CONFIG_PROD_3)
	#include "./xxx/include/_xom_xxx_333.h"
#else
	#error "CONFIG_OP_XXX or CONFIG_PROD_XXX should be defined."
#endif


/* ��� local typedef

	om : �ҹ��ڷ� �����ϴ� layer ��	(om : O, Om : X)
	Xxx : �빮�ڷ� �����ϴ� Module ��
	Type : �빮�ڷ� �����ϴ� type ��. Common convention �� ������.
	_t/e/b : type ����. Common convention�� ������.
*/
typedef int omXxx_Type_t; /* omXxx_Type_e, omXxx_Type_b */
typedef (void *)omXxx_DoSomethingCb_t(void);


/* ��� plug-in API

	pom_ : Plug-in ǥ�� p + �ҹ��ڷ� layer �� ǥ��
	xxx_ : �ҹ��ڷ� module �� ǥ��
	Function Name : �������� ���� ���� + ������ �������� �빮�ڷ� �����ϴ� �ܾ �̾� ����
	
	���� :
		OM ��� ���ο��� �Ļ��� ���� option�� ������ ��� ȣ���ϴ� API.
		Proto type�� base�� ��� internal include�� ����ǰ� body�� �Ļ� ��⿡�� ������.

	���ǻ��� :
		�ݵ�� base ��⿡���� ȣ��Ǿ�� ��.
*/
HERROR pom_xxx_DoSomething( ... )
{
#if defined(CONFIG_OP_111)
	xom_xxx_111_DoSomething();
#elif defined(CONFIG_OP_222)
	xom_xxx_222_DoSomething();
#elif defined(CONFIG_OP_333)
	xom_xxx_333_DoSomething();
#else
	#error "CONFIG_OP_XXX or CONFIG_PROD_XXX should be defined."
#endif
}



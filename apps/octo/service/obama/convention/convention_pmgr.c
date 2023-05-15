/* SVC ���� ��� include */
#include <svc_common.h>



/* ����ϴ� SVC ����� ��� include */
#include <svc_111.h>
#include <svc_222.h>


/* MGR ���� ��� include */
#include <mgr_common.h>




/* ��� plug-in ����� include �� */
#include "../tvmanager/xxx/include/pmgr_xxx.h"




/*
	�Ļ� ��� plug-in body ��� include ��.(�Ļ� ��� ���� ���� ���)
	�Ļ� ��� plug-in ����� makefile�� path ������ �ȵǾ� �����Ƿ� ��� path�� include �ؾ� ��.
*/
#if defined(CONFIG_OP_111)
	#include "./xxx/include/_xmgr_xxx_111.h"
#elif defined(CONFIG_OP_2222)
	#include "./xxx/include/_xmgr_xxx_222.h"
#elif defined(CONFIG_PROD_3)
	#include "./xxx/include/_xmgr_xxx_333.h"
#else
	#error "CONFIG_OP_XXX or CONFIG_PROD_XXX should be defined."
#endif


/* ��� typedef

	Mgr : �빮�ڷ� �����ϴ� layer ��	(Pmgr : X, Mgr : O)
	Xxx : �빮�ڷ� �����ϴ� Module ��
	Type : �빮�ڷ� �����ϴ� type ��. Common convention �� ������.
	_t/e/b : type ����. Common convention�� ������.
*/
typedef int MgrXxx_Type_t; /* MgrXxx_Type_e, MgrXxx_Type_b */
typedef (void *)MgrXxx_DoSomethingCb_t(void);


/* ��� plug-in API

	pmgr_ : Plug-in ǥ�� p + �ҹ��ڷ� layer �� ǥ��
	xxx_ : �ҹ��ڷ� module �� ǥ��
	Function Name : �������� ���� ���� + ������ �������� �빮�ڷ� �����ϴ� �ܾ �̾� ����
	
	���� :
		MGR ��� ���ο��� �Ļ��� ���� option�� ������ ��� ȣ���ϴ� API.
		Proto type�� base�� ��� internal include�� ����ǰ� body�� �Ļ� ��⿡�� ������.

	���ǻ��� :
		�ݵ�� base ��⿡���� ȣ��Ǿ�� ��.
*/
HERROR pmgr_xxx_DoSomething( ... )
{
#if defined(CONFIG_OP_111)
	xmgr_xxx_111_DoSomething();
#elif defined(CONFIG_OP_222)
	xmgr_xxx_222_DoSomething();
#elif defined(CONFIG_OP_333)
	xmgr_xxx_333_DoSomething();
#else
	#error "CONFIG_OP_XXX or CONFIG_PROD_XXX should be defined."
#endif
}




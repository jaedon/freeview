/* SVC ���� ��� include */
#include <svc_common.h>



/* ����ϴ� SVC ����� ��� include */
#include <svc_111.h>
#include <svc_222.h>


/* MGR ���� ���

	mgr_ : �ҹ��� layer�� ǥ��
	common.h : common ���� ǥ��.

	���� :
		TV manager layer ���� ��� layer ����� �������� ����ϴ� ���.

	���� ���� :
		TV service layer������ include �� �� ����.
*/
#include <mgr_common.h>



/* ��� ���

	mgr_ : �ҹ��� layer�� ǥ��
	xxx.h : �ҹ��� module�� ǥ��. 1 depth�� ���.

	���� :
		MGR ���� 1���� ����� �����Ѵ�.

	���� ���� :
		��� ������� ��� API proto type�� �־�� ��.
		��� ����� makefile���� path ������ �Ǿ� �����Ƿ� �Ʒ��� ���� include ������.
*/
#include <mgr_xxx.h>



/* ��� ���� ���� ���

	_mgr_ : Internal ǥ�� _ + �ҹ��� layer�� ǥ��
	xxx_ : �ҹ��� module�� ǥ��
	yyy.h : �ҹ��� sub module ǥ��

	���� :
		MGR ��� ���ο��� ������ type �� function ����.

	���� ���� :
		��� ���� ���� ����� makefile���� path ������ �Ǿ� ���� ����.
		��� ���ο� include directory�� ����� ��ġ�ϰ� ��� ��ġ�κ����� ���� path�� include ��.
*/
#include "./include/_mgr_xxx_yyy.h"




/* ��� plug-in ���

	pmgr_ : Plug-in ǥ�� p + �ҹ��� layer�� ǥ��
	xxx.h : �ҹ��� module�� ǥ��

	���� :
		TV manager ����� �Ļ� API ���.

	���� ���� :
		��� plug-in ����� makefile���� path ������ �Ǿ� ���� ����.
		��� ���ο� include directory�� ����� ��� ��ġ�κ����� ���� path�� include ��.
*/
#include "./include/pmgr_xxx.h"




/* ��� typedef

	Mgr : �빮�ڷ� �����ϴ� layer ��
	Xxx : �빮�ڷ� �����ϴ� Module ��
	Type : �빮�ڷ� �����ϴ� type ��. Common convention �� ������.
	_t/e/b : type ����. Common convention�� ������.
*/
typedef int MgrXxx_Type_t; /* MgrXxx_Type_e, MgrXxx_Type_b */
typedef (void *)MgrXxx_DoSomethingCb_t(void);


/* ��� ������ local typedef

	mgr : �ҹ��ڷ� �����ϴ� layer ��
	Xxx : �빮�ڷ� �����ϴ� Module ��
	Type : �빮�ڷ� �����ϴ� type ��. Common convention �� ������.
	_t/e/b : type ����. Common convention�� ������.
*/
typedef int mgrXxx_Type_t; /* MgrXxx_Type_e, MgrXxx_Type_b */
typedef (void *)mgrXxx_DoSomethingCb_t(void);


/* ��� ������ local function

	mgr_ : �ҹ��ڷ� layer �� ǥ��
	xxx_ : �ҹ��ڷ� module �� ǥ��
	Function Name : �������� ���� ���� + ������ �������� �빮�ڷ� �����ϴ� �ܾ �̾� ����
	
	���� :
		MGR ��� �ȿ����� �� ���� �ȿ����� ����ϱ� ���� function���� ���� ���� ȣ�� ������ ���.

	���ǻ��� :
		�տ� �ݵ�� static �Ǵ� STATIC�� �ٿ��� ��.
		��� �������� local function ���� ����ϸ� �ȵ�.
		local function�� recursive ȣ���� �� ���ɼ��� �����Ƿ� �������� ����ϸ� dead lock ��.
*/
STATIC HERROR mgr_xxx_DoSomething( ... )
{

	....

}



/* ��� ������ global function

	mgr_ : �ҹ��ڷ� layer �� ǥ��(��� local function�� ����)
	xxx_ : �ҹ��ڷ� module �� ǥ��(��� local function�� ����)
	Function Name : �������� ���� ���� + ������ �������� �빮�ڷ� �����ϴ� �ܾ �̾� ����(��� local function�� ����)
	
	���� :
		MGR ����� ���� file�� ������ ��� ���ϰ��� �����ϱ� ���� global function���� ��� ���� ���ϰ� ȣ�� ������ ���.
		Coding convention�� ��� ������ local function�� ���� �տ� static ���� ����.

	���ǻ��� :
		��� �������� ���� global function ���� ����ϸ� �ȵ�.
		���� global function�� recursive ȣ���� �� ���ɼ��� �����Ƿ� �������� ����ϸ� dead lock ��.
*/
HERROR mgr_xxx_DoSomething( ... )
{

	....

}




/* ��� API

	MGR_ : �빮�ڷ� layer �� ǥ��
	XXX_ : �빮�ڷ� module �� ǥ��
	Function Name : �������� ���� ���� + ������ �������� �빮�ڷ� �����ϴ� �ܾ �̾� ����
	
	���ǻ��� :
		TV manager�� ���� �з��Ǿ� �ִ��� ���� thread�̴�.
		Semaphore �� ���ǵ� ������ ������ �������� ���ǰ� �ִٸ� ö���� ���ó�� Ȯ���ؾ� �Ѵ�.
*/
HERROR MGR_XXX_DoSomething( ... )
{
	mgr_xxx_DoSomething();
	pmgr_xxx_DoSomething();

	....

}



/* ��� Procedure

	PROC_ : Manager bus �� plug-in �Ǵ� procedure ���� ��Ÿ��
	XXX_ : �빮�ڷ� module �� ǥ��
	Function Name : �빮�ڷ� �����ϴ� �ܾ �̾� ����

	���� ���� :
		�ڱ� ����� plug-in API�� ȣ���� �� �ִ�.
		�ٸ� MGR ����� plug-in API�� ȣ���� �� �ִ�.
		�ڱ� ����� ��� API�� ȣ�� �� �� �ִ�.
		�ٸ� MGR ����� ��� API�� ȣ�� �� �� �ִ�.
		SVC ��� API�� ȣ�� �� �� �ִ�.

		!!! �Ļ� SVC ��� API�� ȣ�� �Ұ�. !!!
		!!! PAL ��� API�� ȣ�� �Ұ�. !!!
*/
BUS_Result_t PROC_XXX_ProcedureName(HINT32 message, Handle_t hAction, HINT32 p1, HINT32 p2, HINT32 p3)
{

	mgr_xxx_DoSomething();

	pmgr_xxx_DoSomething();
	pmgr_yyy_DoSomething();

	MGR_XXX_DoSomething();
	MGR_YYY_DoSomething();

	SVC_111_DoSomething();
	SVC_222_DoSomething();

	....

}



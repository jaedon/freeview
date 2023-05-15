/* SVC ���� ��� include */
#include <svc_common.h>



/* ����ϴ� SVC ����� ��� include */
#include <svc_111.h>
#include <svc_222.h>
#include <xsvc_111.h>
#include <xsvc_222.h>


/* MGR ��� include */
#include <mgr_xxx.h>
#include <xmgr_xxx.h>

/* OM ���� ��� include */
#include <xom_xxx.h>



/* ��� ���

	xom_ : Extension�� Ÿ������ x + �ҹ��� layer�� ǥ��
	xxx.h : �ҹ��� module�� ǥ��. 1 depth�� ���.

	���� :
		MGR ���� 1���� ����� �����Ѵ�.

	���� ���� :
		��� ������� ��� API proto type�� �־�� ��.
		��� ����� makefile���� path ������ �Ǿ� �����Ƿ� �Ʒ��� ���� include ������.
*/
#include <xom_xxx.h>



/* ��� ���� ���� ���

	_om_ : Internal ǥ�� _ + extension x ǥ�� + �ҹ��� layer�� ǥ��
	xxx_ : �ҹ��� module�� ǥ��
	yyy.h : �ҹ��� sub module ǥ��

	���� :
		MGR ��� ���ο��� ������ type �� function ����.

	���� ���� :
		��� ���� ���� ����� makefile���� path ������ �Ǿ� ���� ����.
		��� ���ο� include directory�� ����� ��ġ�ϰ� ��� ��ġ�κ����� ���� path�� include ��.
*/
#include "./include/_xom_xxx_yyy.h"


/* ��� typedef

	Xom : �빮�ڷ� �����ϴ� layer ��
	Xxx : �빮�ڷ� �����ϴ� Module ��
	Type : �빮�ڷ� �����ϴ� type ��. Common convention �� ������.
	_t/e/b : type ����. Common convention�� ������.
*/
typedef int XomXxx_Type_t; /* XomXxx_Type_e, XomXxx_Type_b */
typedef (void *)XomXxx_DoSomethingCb_t(void);



/* ��� ������ local typedef

	xom : �ҹ��ڷ� �����ϴ� layer ��
	Xxx : �빮�ڷ� �����ϴ� Module ��
	Type : �빮�ڷ� �����ϴ� type ��. Common convention �� ������.
	_t/e/b : type ����. Common convention�� ������.
*/
typedef int xomXxx_Type_t; /* xomXxx_Type_e, xomXxx_Type_b */
typedef (void *)xomXxx_DoSomethingCb_t(void);


/* ��� ������ local function

	xom_ : Extension�� ǥ���ϴ� x + �ҹ��ڷ� layer �� ǥ��
	xxx_ : �ҹ��ڷ� module �� ǥ��
	Function Name : �������� ���� ���� + ������ �������� �빮�ڷ� �����ϴ� �ܾ �̾� ����
	ZZZ_ : �빮�ڷ� �Ļ��� ǥ��
	
	���� :
		OM ��� �ȿ����� �� ���� �ȿ����� ����ϱ� ���� function���� ���� ���� ȣ�� ������ ���.

	���ǻ��� :
		�տ� �ݵ�� static �Ǵ� STATIC�� �ٿ��� ��.
		��� �������� local function ���� ����ϸ� �ȵ�.
		local function�� recursive ȣ���� �� ���ɼ��� �����Ƿ� �������� ����ϸ� dead lock ��.
*/
STATIC HERROR xom_xxx_DoSomething_ZZZ( ... )
{

	....

}



/* ��� ������ global function

	xom_ : Extension�� ǥ���ϴ� x + �ҹ��ڷ� layer �� ǥ��
	xxx_ : �ҹ��ڷ� module �� ǥ��(��� local function�� ����)
	Function Name : �������� ���� ���� + ������ �������� �빮�ڷ� �����ϴ� �ܾ �̾� ����(��� local function�� ����)
	ZZZ_ : �빮�ڷ� �Ļ��� ǥ��
	
	���� :
		XOM ����� ���� file�� ������ ��� ���ϰ��� �����ϱ� ���� global function���� ��� ���� ���ϰ� ȣ�� ������ ���.
		Coding convention�� ��� ������ local function�� ���� �տ� static ���� ����.

	���ǻ��� :
		��� �������� ���� global function ���� ����ϸ� �ȵ�.
		���� global function�� recursive ȣ���� �� ���ɼ��� �����Ƿ� �������� ����ϸ� dead lock ��.
*/
HERROR xom_xxx_DoSomething_ZZZ( ... )
{

	....

}




/* ��� API

	XOM_ : Extension�� ǥ���ϴ� X + �빮�ڷ� layer �� ǥ��
	XXX_ : �빮�ڷ� module �� ǥ��
	Function Name : �������� ���� ���� + ������ �������� �빮�ڷ� �����ϴ� �ܾ �̾� ����
	ZZZ_ : �빮�ڷ� �Ļ��� ǥ��
	
	���ǻ��� :
*/
HERROR XOM_XXX_DoSomething_ZZZ( ... )
{
	xom_xxx_zzz_DoSomething();

	....

}



/* ��� Procedure

	xmproc_ : Extension�� ǥ���ϴ� x + Manager bus �� plug-in �Ǵ� procedure ���� ��Ÿ��
	xxx_ : �ҹ��ڷ� module �� ǥ��
	Function Name : �빮�ڷ� �����ϴ� �ܾ �̾� ����
	ZZZ_ : �빮�ڷ� �Ļ��� ǥ��

	���� ���� :
		�ڱ� ����� base�� �Ļ� ��� API�� ȣ�� �� �� �ִ�.
		�ٸ� MGR ����� base�� �Ļ� ��� API�� ȣ�� �� �� �ִ�.
		SVC ��� API�� ȣ�� �� �� �ִ�.

		!!! plug-in API ȣ�� �Ұ� !!!
		!!! PAL ��� API�� ȣ�� �Ұ�. !!!
*/
BUS_Result_t xmproc_xxx_ProcedureName_ZZZ(HINT32 message, Handle_t hAction, HINT32 p1, HINT32 p2, HINT32 p3)
{

	om_xxx_DoSomething();
	xom_xxx_DoSomething();

	OM_XXX_DoSomething();
	OM_YYY_DoSomething();

	XOM_XXX_DoSomething();
	XOM_YYY_DoSomething();

	MGR_XXX_DoSomething();
	MGR_YYY_DoSomething();

	XMGR_XXX_DoSomething();
	XMGR_YYY_DoSomething();

	SVC_111_DoSomething();
	SVC_222_DoSomething();

	XSVC_111_XXX_DoSomething();
	XSVC_222_XXX_DoSomething();

	....

}



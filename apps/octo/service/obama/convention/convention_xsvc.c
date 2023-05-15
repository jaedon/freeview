
/* SVC ���� ��� include */
#include <svc_common.h>



/* ����ϴ� PAL ����� ��� include */
#include <pal_111.h>
#include <pal_222.h>



/* ��� ���

	xsvc_ : Extension�� Ÿ������ x + �ҹ��� layer�� ǥ��
	xxx_ : �ҹ��� module�� ǥ��. 1 depth�� ���.
	yyy_ : �ҹ��� �Ļ��� ǥ��.

	���� :
		SVC ���� 1���� ����� �����Ѵ�.

	���� ���� :
		��� ������� ��� API proto type�� �־�� ��.
		��� ����� makefile���� path ������ �Ǿ� �����Ƿ� �Ʒ��� ���� include ������.
*/
#include <xsvc_xxx_yyy.h>




/* ��� ���� ���� ���

	_xsvc_ : Internal ǥ�� _ + extension x ǥ�� + �ҹ��� layer�� ǥ��
	xxx_ : �ҹ��� module�� ǥ��
	yyy_ : �ҹ��� �Ļ��� ǥ��
	yyy.h : �ҹ��� sub module ǥ��

	���� :
		SVC ��� ���ο��� ������ type �� function ����.

	���� ���� :
		��� ���� ���� ����� makefile���� path ������ �Ǿ� ���� ����.
		��� ���ο� include directory�� ����� ��ġ�ϰ� ��� ��ġ�κ����� ���� path�� include ��.
*/
#include "./include/_xsvc_xxx_yyy_zzz.h"


/* ��� typedef

	Xsvc : �빮�ڷ� �����ϴ� layer ��
	Xxx : �빮�ڷ� �����ϴ� Module ��
	Type : �빮�ڷ� �����ϴ� type ��. Common convention �� ������.
	_t/e/b : type ����. Common convention�� ������.
*/
typedef int XsvcXxx_Type_t; /* XsvcXxx_Type_e, XsvcXxx_Type_b */
typedef (void *)XsvcXxx_DoSomethingCb_t(void);



/* API �Ǵ� context�� �����ϴ� �������� �ʿ��� ��� ��� */
STATIC HULONG	s_ulModuleSema;


/* ��� ������ local typedef

	xsvc : �ҹ��ڷ� �����ϴ� layer ��
	Xxx : �빮�ڷ� �����ϴ� Module ��
	Type : �빮�ڷ� �����ϴ� type ��. Common convention �� ������.
	_t/e/b : type ����. Common convention�� ������.
*/
typedef int xsvcXxx_Type_t; /* xsvcXxx_Type_e, xsvcXxx_Type_b */
typedef (void *)xsvcXxx_DoSomethingCb_t(void);


/* ��� ������ local function

	xsvc_ : Extension�� ǥ���ϴ� x + �ҹ��ڷ� layer �� ǥ��
	xxx_ : �ҹ��ڷ� module �� ǥ��
	Function Name : �������� ���� ���� + ������ �������� �빮�ڷ� �����ϴ� �ܾ �̾� ����
	Yyy_ : �빮�� ���� �Ļ� �� ǥ��
	
	���� :
		SVC ��� �ȿ����� �� ���� �ȿ����� ����ϱ� ���� function���� ���� ���� ȣ�� ������ ���.

	���ǻ��� :
		�տ� �ݵ�� static �Ǵ� STATIC�� �ٿ��� ��.
		��� �������� local function ���� ����ϸ� �ȵ�.
		local function�� recursive ȣ���� �� ���ɼ��� �����Ƿ� �������� ����ϸ� dead lock ��.
*/
STATIC HERROR xsvc_xxx_DoSomething_Yyy( ... )
{

	....

}



/* ��� ������ global function

	xsvc_ : Extension�� ǥ���ϴ� x + �ҹ��ڷ� layer �� ǥ��
	xxx_ : �ҹ��ڷ� module �� ǥ��(��� local function�� ����)
	Function Name : �������� ���� ���� + ������ �������� �빮�ڷ� �����ϴ� �ܾ �̾� ����(��� local function�� ����)
	
	���� :
		SVC ����� ���� file�� ������ ��� ���ϰ��� �����ϱ� ���� global function���� ��� ���� ���ϰ� ȣ�� ������ ���.
		Coding convention�� ��� ������ local function�� ���� �տ� static ���� ����.

	���ǻ��� :
		��� �������� ���� global function ���� ����ϸ� �ȵ�.
		���� global function�� recursive ȣ���� �� ���ɼ��� �����Ƿ� �������� ����ϸ� dead lock ��.
*/
HERROR xsvc_xxx_yyy_DoSomething( ... )
{

	....

}



/* ��� API

	XSVC_ : Extension�� ǥ���ϴ� X + �빮�ڷ� layer �� ǥ��
	XXX_ : �빮�ڷ� module �� ǥ��
	YYY_ : �빮�ڷ� �Ļ� �� ǥ��
	Function Name : �������� ���� ���� + ������ �������� �빮�ڷ� �����ϴ� �ܾ �̾� ����
	
	���ǻ��� :
		* �Ļ� ����� service API�� �ݵ�� �Ļ� ����� TV manager������ ȣ��Ǿ�� �Ѵ�.

		* ��� �������� ��� API������ ����ؾ� ��.
		��, ��� �ܺο��� ȣ���ϴ� API�� ������ ��� ������.
		���� local function �� �״�� ��� API�� ���� �� �ִ� ����� �ص� �Ʒ��� ����
		������ ȣ��� local function ȣ���� �����Ͽ� ��� API�� ������.

		* ��� API�� ��� ���ο��� ȣ�� �Ǿ�� �ȵȴ�.
		�������� ǰ�� �ִ� ��� API�� ���� ȣ���� �Ǿ� recursive ȣ���� �ɸ��� dead lock ��.
*/
HERROR XSVC_XXX_YYY_DoSomething( ... )
{
	VK_SEM_Get(s_ulModuleSema); /* ��� �������� ��� API ������ ����ؾ� �Ѵ�. */

	xsvc_xxx_DoSomething();

	PAL_111_DoSomething();
	PAL_222_DoSomething();

	....

	VK_SEM_Release(s_ulModuleSema);
}




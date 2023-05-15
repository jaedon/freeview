
/* SVC ���� ���

	svc_ : �ҹ��� layer�� ǥ��
	common.h : common ���� ǥ��.

	���� :
		OBAMA ���� ��� layer ����� �������� ����ϴ� TV service layer ���.
		OCTO 1.0 ������ stb_param.h �� ���� ����.
		�ַ� OAPI�� ���ǵ��� ���� obama ���� type�̳� struct�� ����
		tvservice/include �� ��ġ�ϰ� makefile path ������ �Ǿ� ����.

	���� ���� :
		TV manager �� ����� include �� �� ����.
*/
#include <svc_common.h>



/* ����ϴ� PAL ����� ��� include */
#include <pal_111.h>
#include <pal_222.h>



/* ��� ���

	svc_ : �ҹ��� layer�� ǥ��
	xxx.h : �ҹ��� module�� ǥ��. 1 depth�� ���.

	���� :
		SVC ���� 1���� ����� �����Ѵ�.

	���� ���� :
		��� ������� ��� API proto type�� �־�� ��.
		��� ����� makefile���� path ������ �Ǿ� �����Ƿ� �Ʒ��� ���� include ������.
*/
#include <svc_xxx.h>




/* ��� ���� ���� ���

	_svc_ : Internal ǥ�� _ + �ҹ��� layer�� ǥ��
	xxx_ : �ҹ��� module�� ǥ��
	yyy.h : �ҹ��� sub module ǥ��

	���� :
		SVC ��� ���ο��� ������ type �� function ����.

	���� ���� :
		��� ���� ���� ����� makefile���� path ������ �Ǿ� ���� ����.
		��� ���ο� include directory�� ����� ��ġ�ϰ� ��� ��ġ�κ����� ���� path�� include ��.
*/
#include "_svc_xxx_yyy.h"




/* ��� plug-in ���

	psvc_ : Plug-in ǥ�� p + �ҹ��� layer�� ǥ��
	xxx.h : �ҹ��� module�� ǥ��

	���� :
		TV service ����� �Ļ� API ���.

	���� ���� :
		��� plug-in ����� makefile���� path ������ �Ǿ� ���� ����.
		��� ���ο� include directory�� ����� ��� ��ġ�κ����� ���� path�� include ��.
*/
#include "./include/psvc_xxx.h"



/* ��� typedef

	Svc : �빮�ڷ� �����ϴ� layer ��
	Xxx : �빮�ڷ� �����ϴ� Module ��
	Type : �빮�ڷ� �����ϴ� type ��. Common convention �� ������.
	_t/e/b : type ����. Common convention�� ������.
*/
typedef int SvcXxx_Type_t; /* SvcXxx_Type_e, SvcXxx_Type_b */
typedef (void *)SvcXxx_DoSomethingCb_t(void);




/* API �Ǵ� context�� �����ϴ� �������� �ʿ��� ��� ��� */
STATIC HULONG	s_ulModuleSema;



/* ��� ������ local function

	svc_ : �ҹ��ڷ� layer �� ǥ��
	xxx_ : �ҹ��ڷ� module �� ǥ��
	Function Name : �������� ���� ���� + ������ �������� �빮�ڷ� �����ϴ� �ܾ �̾� ����
	
	���� :
		SVC ��� �ȿ����� �� ���� �ȿ����� ����ϱ� ���� function���� ���� ���� ȣ�� ������ ���.

	���ǻ��� :
		�տ� �ݵ�� static �Ǵ� STATIC�� �ٿ��� ��.
		��� �������� local function ���� ����ϸ� �ȵ�.
		local function�� recursive ȣ���� �� ���ɼ��� �����Ƿ� �������� ����ϸ� dead lock ��.
*/
STATIC HERROR svc_xxx_DoSomething( ... )
{

	....

}


/* ��� ������ local typedef

	svc : �ҹ��ڷ� �����ϴ� layer ��
	Xxx : �빮�ڷ� �����ϴ� Module ��
	Type : �빮�ڷ� �����ϴ� type ��. Common convention �� ������.
	_t/e/b : type ����. Common convention�� ������.
*/
typedef int svcXxx_Type_t; /* svcXxx_Type_e, svcXxx_Type_b */
typedef (void *)svcXxx_DoSomethingCb_t(void);


/* ��� ������ global function

	svc_ : �ҹ��ڷ� layer �� ǥ��(��� local function�� ����)
	xxx_ : �ҹ��ڷ� module �� ǥ��(��� local function�� ����)
	Function Name : �������� ���� ���� + ������ �������� �빮�ڷ� �����ϴ� �ܾ �̾� ����(��� local function�� ����)
	
	���� :
		SVC ����� ���� file�� ������ ��� ���ϰ��� �����ϱ� ���� global function���� ��� ���� ���ϰ� ȣ�� ������ ���.
		Coding convention�� ��� ������ local function�� ���� �տ� static ���� ����.

	���ǻ��� :
		��� �������� ���� global function ���� ����ϸ� �ȵ�.
		���� global function�� recursive ȣ���� �� ���ɼ��� �����Ƿ� �������� ����ϸ� dead lock ��.
*/
HERROR svc_xxx_DoSomething( ... )
{

	....

}




/* ��� API

	SVC_ : �빮�ڷ� layer �� ǥ��
	XXX_ : �빮�ڷ� module �� ǥ��
	Function Name : �������� ���� ���� + ������ �������� �빮�ڷ� �����ϴ� �ܾ �̾� ����
	
	���ǻ��� :
		* ��� �������� ��� API������ ����ؾ� ��.
		��, ��� �ܺο��� ȣ���ϴ� API�� ������ ��� ������.
		���� local function �� �״�� ��� API�� ���� �� �ִ� ����� �ص� �Ʒ��� ����
		������ ȣ��� local function ȣ���� �����Ͽ� ��� API�� ������.

		* ��� API�� ��� ���ο��� ȣ�� �Ǿ�� �ȵȴ�.
		�������� ǰ�� �ִ� ��� API�� ���� ȣ���� �Ǿ� recursive ȣ���� �ɸ��� dead lock ��.
*/
HERROR SVC_XXX_DoSomething( ... )
{
	VK_SEM_Get(s_ulModuleSema); /* ��� �������� ��� API ������ ����ؾ� �Ѵ�. */

	svc_xxx_DoSomething();
	psvc_xxx_DoSomething();

	PAL_111_DoSomething();
	PAL_222_DoSomething();

	....

	VK_SEM_Release(s_ulModuleSema);
}




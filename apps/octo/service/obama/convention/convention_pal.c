
/* SVC ���� ���

	svc_ : �ҹ��� layer�� ǥ��
	common.h : �ҹ��� module�� ǥ��. 1 depth�� ���.

	���� :
		OBAMA ���� ��� layer ����� �������� ����ϴ� TV service layer ���.
		OCTO 1.0 ������ stb_param.h �� ���� ����.
		�ַ� OAPI�� ���ǵ��� ���� obama ���� type�̳� struct�� ����
		tvservice/include �� ��ġ�ϰ� makefile path ������ �Ǿ� ����.

	���� ���� :
*/
#include <svc_common.h>



/* ��� ���

	pal_ : �ҹ��� layer�� ǥ��
	xxx.h : �ҹ��� module�� ǥ��. 1 depth�� ���.

	���� :
		PAL ���� 1���� ����� �����Ѵ�.

	���� ���� :
		��� ������� ��� API proto type�� �־�� ��.
		��� ����� makefile���� path ������ �Ǿ� �����Ƿ� �Ʒ��� ���� include ������.
*/
#include <pal_xxx.h>




/* ��� ���� ���� ���

	_pal_ : Internal ǥ�� _ + �ҹ��� layer�� ǥ��
	xxx_ : �ҹ��� module�� ǥ��
	yyy.h : �ҹ��� sub module ǥ��

	���� :
		PAL ��� ���ο��� ������ type �� function ����.

	���� ���� :
		��� ����� makefile���� path ������ �Ǿ� ���� ����.
		��� ���ο� include directory�� ����� ��� ��ġ�κ����� ���� path�� include ��.
*/
#include "./include/_pal_xxx_yyy.h"


/* ��� typedef

	Pal : �빮�ڷ� �����ϴ� layer ��
	Type : �빮�ڷ� �����ϴ� type ��. Common convention �� ������.
	_t/e/b : type ����. Common convention�� ������.
*/
typedef int PalType_t; /* PalType_e, PalType_b */



/* API �Ǵ� context�� �����ϴ� �������� �ʿ��� ��� ��� */
STATIC HULONG	s_ulModuleSema;


/* ��� ������ local typedef

	pal : �ҹ��ڷ� �����ϴ� layer ��
	Xxx : �빮�ڷ� �����ϴ� Module ��
	Type : �빮�ڷ� �����ϴ� type ��. Common convention �� ������.
	_t/e/b : type ����. Common convention�� ������.
*/
typedef int palXxx_Type_t; /* palXxx_Type_e, palXxx_Type_b */
typedef (void *)palXxx_DoSomethingCb_t(void);



/* ��� ������ local function

	pal_ : �ҹ��ڷ� layer �� ǥ��
	xxx_ : �ҹ��ڷ� module �� ǥ��
	Function Name : �������� ���� ���� + ������ �������� �빮�ڷ� �����ϴ� �ܾ �̾� ����
	
	���� :
		PAL ��� �ȿ����� �� ���� �ȿ����� ����ϱ� ���� function���� ���� ���� ȣ�� ������ ���.

	���ǻ��� :
		�տ� �ݵ�� static �Ǵ� STATIC�� �ٿ��� ��.
		��� �������� local function ���� ����ϸ� �ȵ�.
		local function�� recursive ȣ���� �� ���ɼ��� �����Ƿ� �������� ����ϸ� dead lock ��.
*/
STATIC HERROR pal_xxx_DoSomething( ... )
{

	....

}



/* ��� ������ global function

	pal_ : �ҹ��ڷ� layer �� ǥ��(��� local function�� ����)
	xxx_ : �ҹ��ڷ� module �� ǥ��(��� local function�� ����)
	Function Name : �������� ���� ���� + ������ �������� �빮�ڷ� �����ϴ� �ܾ �̾� ����(��� local function�� ����)
	
	���� :
		PAL ����� ���� file�� ������ ��� ���ϰ��� �����ϱ� ���� global function���� ��� ���� ���ϰ� ȣ�� ������ ���.
		Coding convention�� ��� ������ local function�� ���� �տ� static ���� ����.

	���ǻ��� :
		��� �������� ���� global function ���� ����ϸ� �ȵ�.
		���� global function�� recursive ȣ���� �� ���ɼ��� �����Ƿ� �������� ����ϸ� dead lock ��.
*/
HERROR pal_xxx_DoSomething( ... )
{

	....

}




/* ��� API

	PAL_ : �빮�ڷ� layer �� ǥ��
	XXX_ : �빮�ڷ� module �� ǥ��
	Function Name : �������� ���� ���� + ������ �������� �빮�ڷ� �����ϴ� �ܾ �̾� ����
	
	���ǻ��� :
		* �Ķ���ʹ� �ǵ��� struct�� ������� ����.
		Struct �� ��� ��ä�� ���޵Ǵ� ��찡 �־� �������� platform ���۰� ������ ������� ���ؼ���
		������ ���� �Ķ���Ͱ� ��Ȯ�� ���� ������.

		* ��� �������� ��� API������ ����ؾ� ��.
		��, ��� �ܺο��� ȣ���ϴ� API�� ������ ��� ������.
		���� local function �� �״�� ��� API�� ���� �� �ִ� ����� �ص� �Ʒ��� ����
		������ ȣ��� local function ȣ���� �����Ͽ� ��� API�� ������.

		* ��� API�� ��� ���ο��� ȣ�� �Ǿ�� �ȵȴ�.
		�������� ǰ�� �ִ� ��� API�� ���� ȣ���� �Ǿ� recursive ȣ���� �ɸ��� dead lock ��.
*/
HERROR PAL_XXX_DoSomething( ... )
{
	VK_SEM_Get(s_ulModuleSema); /* ��� �������� ��� API ������ ����ؾ� �Ѵ�. */

	pal_xxx_DoSomething();

	....

	VK_SEM_Release(s_ulModuleSema);
}




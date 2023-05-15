
/* XDB ���� ���

	xdb_ : �ҹ��� layer�� ǥ��
	common.h : �ҹ��� module�� ǥ��. 1 depth�� ���.

	���� :
		OBAMA ���� ��� layer ����� �������� ����ϴ� TV service layer ���.
		OCTO 1.0 ������ stb_param.h �� ���� ����.
		�ַ� OAPI�� ���ǵ��� ���� obama ���� type�̳� struct�� ����
		tvservice/include �� ��ġ�ϰ� makefile path ������ �Ǿ� ����.

	���� ���� :
*/
#include <xdb_common.h>



/* ��� ���

	xdb_ : �ҹ��� layer�� ǥ��
	xxx.h : �ҹ��� module�� ǥ��. 1 depth�� ���.

	���� :
		DB ���� 1���� ����� �����Ѵ�.

	���� ���� :
		��� ������� ��� API proto type�� �־�� ��.
		��� ����� makefile���� path ������ �Ǿ� �����Ƿ� �Ʒ��� ���� include ������.
*/
#include <xdb_xxx.h>




/* ��� ���� ���� ���

	_xdb_ : Internal ǥ�� _ + �ҹ��� layer�� ǥ��
	xxx_ : �ҹ��� module�� ǥ��
	yyy.h : �ҹ��� sub module ǥ��

	���� :
		DB ��� ���ο��� ������ type �� function ����.

	���� ���� :
		��� ����� makefile���� path ������ �Ǿ� ���� ����.
		��� ���ο� include directory�� ����� ��� ��ġ�κ����� ���� path�� include ��.
*/
#include "./local_include/_xdb_xxx_yyy.h"


/* ��� typedef

	Xdb : �빮�ڷ� �����ϴ� layer ��
	Xxx : �빮�ڷ� �����ϴ� Module ��
	Type : �빮�ڷ� �����ϴ� type ��. Common convention �� ������.
	_t/e/b : type ����. Common convention�� ������.
*/
typedef int XdbXxx_Type_t; /* XdbXxx_Type_e, XdbXxx_Type_b */
typedef (void *)XdbXxx_DoSomethingCb_t(void);


/* Event

	XEVT_ : �빮�ڷ� EVENT Prefix
	DB_ : �빮�� Module �̸�
	NOTIFY_ : �빮�� NOTIFY Event ǥ��
	XXX_ : �빮�� Submodule�̸�
	YYY : �빮�� Command or Notify ���� ���.

*/
enum
{
	XEVT_DB_					 = eSEVT_DB_END,
	XEVT_DB_XXX_YYY,
	XEVT_DB_NOTIFY_XXX_YYY,
	...,
	_XEVT_DB_END
};



/* API �Ǵ� context�� �����ϴ� �������� �ʿ��� ��� ��� */
STATIC HULONG	s_ulModuleSema;


/* ��� ������ local typedef

	xdb : �ҹ��ڷ� �����ϴ� layer ��
	Xxx : �빮�ڷ� �����ϴ� Module ��
	Type : �빮�ڷ� �����ϴ� type ��. Common convention �� ������.
	_t/e/b : type ����. Common convention�� ������.
*/
typedef int xdbXxx_Type_t; /* xdbXxx_Type_e, xdbXxx_Type_b */
typedef (void *)xdbXxx_DoSomethingCb_t(void);


/* ��� ������ local function

	xdb_ : �ҹ��ڷ� layer �� ǥ��
	xxx_ : �ҹ��ڷ� module �� ǥ��
	Function Name : �������� ���� ���� + ������ �������� �빮�ڷ� �����ϴ� �ܾ �̾� ����
	
	���� :
		DB ��� �ȿ����� �� ���� �ȿ����� ����ϱ� ���� function���� ���� ���� ȣ�� ������ ���.

	���ǻ��� :
		�տ� �ݵ�� static �Ǵ� STATIC�� �ٿ��� ��.
		��� �������� local function ���� ����ϸ� �ȵ�.
		local function�� recursive ȣ���� �� ���ɼ��� �����Ƿ� �������� ����ϸ� dead lock ��.
*/
STATIC HERROR xdb_xxx_DoSomething( ... )
{

	....

}



/* ��� ������ global function

	db_ : �ҹ��ڷ� layer �� ǥ��(��� local function�� ����)
	xxx_ : �ҹ��ڷ� module �� ǥ��(��� local function�� ����)
	Function Name : �������� ���� ���� + ������ �������� �빮�ڷ� �����ϴ� �ܾ �̾� ����(��� local function�� ����)
	
	���� :
		DB ����� ���� file�� ������ ��� ���ϰ��� �����ϱ� ���� global function���� ��� ���� ���ϰ� ȣ�� ������ ���.
		Coding convention�� ��� ������ local function�� ���� �տ� static ���� ����.

	���ǻ��� :
		��� �������� ���� global function ���� ����ϸ� �ȵ�.
		���� global function�� recursive ȣ���� �� ���ɼ��� �����Ƿ� �������� ����ϸ� dead lock ��.
*/
HERROR db_xxx_DoSomething( ... )
{

	....

}




/* ��� API

	DB_ : �빮�ڷ� layer �� ǥ��
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
HERROR DB_XXX_DoSomething( ... )
{
	VK_SEM_Get(s_ulModuleSema); /* ��� �������� ��� API ������ ����ؾ� �Ѵ�. */

	db_xxx_DoSomething();

	....

	VK_SEM_Release(s_ulModuleSema);
}




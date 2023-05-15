#include <oapi.h>


/* ��� typedef

	Ox : OAPI layer ��
	Xxx : �빮�ڷ� �����ϴ� Category ��
	Yyy : �빮�ڷ� �����ϴ� Module ��
	Type : �빮�ڷ� �����ϴ� type ��. Common convention �� ������.
	_t/e/b : type ����. Common convention�� ������.
*/
typedef int OxXxxYyy_Type_t; /* OxXxxYyy_Type_e, OxXxxYyy_Type_b */


/* ��� ������ local typedef

	oapi : �ҹ��ڷ� �����ϴ� layer ��
	Xxx : �빮�ڷ� �����ϴ� Category ��
	Yyy : �빮�ڷ� �����ϴ� Module ��
	Type : �빮�ڷ� �����ϴ� type ��. Common convention �� ������.
	_t/e/b : type ����. Common convention�� ������.
*/
typedef int oapiXxxYyy_Type_t; /* oapiXxxYyy_Type_e, oapiXxxYyy_Type_b */
typedef (void *)oapiXxxYyy_DoSomethingCb_t(void);
typedef (void *)oapiXxxYyy_Notify_t(void);


/* ��� ������ local function

	oapi_ : �ҹ��ڷ� oapi �� ǥ��
	xxx_ : �ҹ��ڷ� category �� ǥ��
	yyy_ : �ҹ��ڷ� module �� ǥ��
	Function Name : �������� ���� ���� + ������ �������� �빮�ڷ� �����ϴ� �ܾ �̾� ����
	
	���� :
		OAPI ��� �ȿ����� �� ���� �ȿ����� ����ϱ� ���� function���� ���� ���� ȣ�� ������ ���.

	���ǻ��� :
		�տ� �ݵ�� static �Ǵ� STATIC�� �ٿ��� ��.
		��� �������� local function ���� ����ϸ� �ȵ�.
		local function�� recursive ȣ���� �� ���ɼ��� �����Ƿ� �������� ����ϸ� dead lock ��.
*/
STATIC HERROR oapi_xxx_yyy_DoSomething( ... )
{

	....

}



/* ��� ������ global function

	oapi_ : �ҹ��ڷ� layer �� ǥ��(��� local function�� ����)
	xxx_ : �ҹ��ڷ� category �� ǥ��(��� local function�� ����)
	yyy_ : �ҹ��ڷ� module �� ǥ��(��� local function�� ����)
	Function Name : �������� ���� ���� + ������ �������� �빮�ڷ� �����ϴ� �ܾ �̾� ����(��� local function�� ����)
	
	���� :
		OAPI ����� ���� file�� ������ ��� ���ϰ��� �����ϱ� ���� global function���� ��� ���� ���ϰ� ȣ�� ������ ���.
		Coding convention�� ��� ������ local function�� ���� �տ� static ���� ����.

	���ǻ��� :
		��� �������� ���� global function ���� ����ϸ� �ȵ�.
		���� global function�� recursive ȣ���� �� ���ɼ��� �����Ƿ� �������� ����ϸ� dead lock ��.
*/
HERROR oapi_xxx_yyy_DoSomething( ... )
{

	....

}




/* ��� API

	OAPI_ : �빮�ڷ� layer �� ǥ��
	XXX_ : �빮�ڷ� category �� ǥ��
	YYY_ : �빮�ڷ� module �� ǥ��
	Function Name : �������� ���� ���� + ������ �������� �빮�ڷ� �����ϴ� �ܾ �̾� ����
	
	���ǻ��� :
*/
HERROR OAPI_XXX_YYY_DoSomething( ... )
{
	oapi_xxx_yyy_DoSomething();

	....

}

/* RPC API string define
	RPC_ : RPC �� string define ǥ��
	OAPI_XXX_YYY_DoSomething : ��� API�� ����
	"xxx.yyy.DoSomething" : JAVA style�� xxx.yyy �� function �� �����ϰ� ����.
*/
#define RPC_OAPI_XXX_YYY_DoSomething	"xxx.yyy.DoSomething"

/* RPC notify string define
	RPC_ : RPC �� string define ǥ��
	OBAMA_XXX_YYY_onSomeEvent : Event handler functin�� ����
	"xxx.yyy.onSomeEvent" : JAVA style�� xxx.yyy �� event notify function �� �����ϰ� ����.
*/
#define RPC_OM_OUTPUT_VIDEO_onSomeEvent		"output.video.onSomeEvent"



#include <oapi.h>


/* ��� typedef

	Type : �빮�ڷ� �����ϴ� type ��. Common convention �� ������.
	_t/e/b : type ����. Common convention�� ������.
*/

/*	Message
	MSG_OM_XXX_YYY
*/
enum
{
	MSG_OM_XXX_YYY_DOSOMETHING,
	MSG_OM_NOTIFY_XXX_YYY_DOSOMETHING,
}


/* ��� typedef
	N/A
*/


/* ��� ������ local typedef

	om : �ҹ��ڷ� �����ϴ� layer ��
	Xxx : �빮�ڷ� �����ϴ� Module ��
	Type : �빮�ڷ� �����ϴ� type ��. Common convention �� ������.
	_t/e/b : type ����. Common convention�� ������.
*/
typedef int omXxx_Type_t; /* omXxx_Type_e, omXxx_Type_b */
typedef (void *)omXxx_DoSomethingCb_t(void);


/* ��� ������ local function

	om_ : �ҹ��ڷ� oapi manager layer �� ǥ��
	xxx_ : �ҹ��ڷ� category �� ǥ��
	yyy_ : �ҹ��ڷ� module �� ǥ��
	Function Name : �������� ���� ���� + ������ �������� �빮�ڷ� �����ϴ� �ܾ �̾� ����
	
	���� :
		OAPI manager ��� �ȿ����� �� ���� �ȿ����� ����ϱ� ���� function���� ���� ���� ȣ�� ������ ���.

	���ǻ��� :
		�տ� �ݵ�� static �Ǵ� STATIC�� �ٿ��� ��.
		��� �������� local function ���� ����ϸ� �ȵ�.
		local function�� recursive ȣ���� �� ���ɼ��� �����Ƿ� �������� ����ϸ� dead lock ��.
*/
STATIC HERROR om_xxx_yyy_DoSomething( ... )
{

	....

}



/* ��� ������ global function

	om_ : �ҹ��ڷ� OAPI manager layer �� ǥ��(��� local function�� ����)
	xxx_ : �ҹ��ڷ� category �� ǥ��(��� local function�� ����)
	yyy_ : �ҹ��ڷ� module �� ǥ��(��� local function�� ����)
	Function Name : �������� ���� ���� + ������ �������� �빮�ڷ� �����ϴ� �ܾ �̾� ����(��� local function�� ����)
	
	���� :
		OAPI manager ����� ���� file�� ������ ��� ���ϰ��� �����ϱ� ���� global function���� ��� ���� ���ϰ� ȣ�� ������ ���.
		Coding convention�� ��� ������ local function�� ���� �տ� static ���� ����.

	���ǻ��� :
		��� �������� ���� global function ���� ����ϸ� �ȵ�.
		���� global function�� recursive ȣ���� �� ���ɼ��� �����Ƿ� �������� ����ϸ� dead lock ��.
*/
HERROR om_xxx_yyy_DoSomething( ... )
{

	....

}




/* ��� API

	OM_ : �빮�ڷ� OAPI manager layer �� ǥ��
	XXX_ : �빮�ڷ� category �� ǥ��
	YYY_ : �빮�ڷ� module �� ǥ��
	Function Name : �������� ���� ���� + ������ �������� �빮�ڷ� �����ϴ� �ܾ �̾� ����
	
	���ǻ��� :
*/
HERROR OM_XXX_YYY_DoSomething( ... )
{
	om_xxx_yyy_DoSomething();

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



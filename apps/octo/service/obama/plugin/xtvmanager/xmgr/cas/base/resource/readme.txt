kraken_string.xlsm : Octo Style의 UX팀에서 관리하는 사전화일
stringout.xml : 해당 excel화일의 모든 문자열을 xml형태로 뽑아 오는 스타일 화일
resource_string.xml : kraken_string.xlsm에 정의된 모든 모든 스트링을 하나의 xml로 뽑아 낸 화일
UI Tool은 resource_string.xml을 기본으로 사용을 하게 되고 이화일에서 필요한 문자를 골라내는 식으로 이루어 진다.
kraken_string.xlsm화일이 Update되었을경우 해당 resource_string.xml화일이 함께 나와야 한다.
이 부분은 ux팀이 통합 관리가 되면 좋을듯 함.
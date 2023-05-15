/* Power 관련 */
'use strict';

// 내부 모듈 import


// 외부 모듈 import


// 서브 모듈 import
var binder = RESTAPI.binder;
var restUtil = RESTAPI.restUtil;


// Local Variable
// TODO: 구현 및 Binding 완료 되면 아래 주석을 풀어서 global 객체에 삽입할 것
//var power_api = {
//    category: 'power',
//    api_set: [
//        {method: 'GET',
//            api: RESTAPI.serverInfo.apiv1path+'/power',
//            url: undefined,
//            parameters: []}
//    ]
//};
//if (!RESTAPI.v1.power_api) {
//    RESTAPI.v1.power_api = power_api.api_set;
//}


/**
 *  Method : GET
 *  API : /stb-api/v1/power
 *  Description : Power Inform 요청
 *  */
exports.getPower = function (request, response) {
    response.send('Return Power info');
};
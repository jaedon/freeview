/* Storage 관련 */
'use strict';

// 내부 모듈 import


// 외부 모듈 import


// 서브 모듈 import
var binder = RESTAPI.binder;
var restUtil = RESTAPI.restUtil;


// TODO: 구현 및 Binding 완료 되면 아래 주석을 풀어서 global 객체에 삽입할 것
//var storage_api = {
//    category: 'storage',
//    api_set: [
//        {method: 'GET',
//            api: RESTAPI.serverInfo.apiv1path+'/storages',
//            url: undefined,
//            parameters: []},
//        {method: 'GET',
//            api: RESTAPI.serverInfo.apiv1path+'/storages/[storageUid]',
//            url: undefined,
//            parameters: []},
//        {method: 'GET',
//            api: RESTAPI.serverInfo.apiv1path+'/storages/[storageUid]/[patitionId]',
//            url: undefined,
//            parameters: []}
//    ]
//};
//if (!RESTAPI.v1.storage_api) {
//    RESTAPI.v1.storage_api = storage_api.api_set;
//}


/**
 *  Method : GET
 *  API : /stb-api/v1/storages
 *  Description : 모든 Storage 대한 요청
 *  */
exports.getStorages = function (request, response) {
    response.send('Return All Storage info');
};


/**
 *  Method : GET
 *  API : /stb-api/v1/storages/:storageUid
 *  Description : Storage Inform 요청
 *  */
exports.getStorageByUid = function (request, response) {
    var uid = request.param('storageUid');

    response.send('Return ' + uid + ' Storage info');
};


/**
 *  Method : GET
 *  API : /stb-api/v1/storages/:storageUid/:patitionId
 *  Description : Partition Inform 요청
 *  */
exports.getPartitionById = function (request, response) {
    var storageUid = request.param('storageUid');
    var partitionId = request.param('patitionId');

    response.send('Return Storage = ' + storageUid + ' Partition = ' + partitionId);
};
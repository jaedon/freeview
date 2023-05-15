'use strict';

var ffi = require('node-ffi');

console.log('ffi.Library("libnodebinder") loading...\n');

/*
 // node-ffi 0.5.7 일 경우의 사용 예제
 // Sync Call 버전인 ffi.rest.test.js 와 비교하여 볼것,,,  {async:true} 필드를 통해 sync/async 구분
 var restBind = new ffi.Library("libnodebinder", {
 "REST_BIND_Init":['void', [], {async:true}],
 "REST_BIND_CHAN_GetAllChannelList":['string', ['string', 'string'], {async:true}],
 "REST_BIND_CHAN_GetChannelList":['string', ['string', 'string', 'int', 'int'], {async:true}]
 });

 console.log('ffi.Library("libnodebinder") done!!\n');

 restBind.REST_BIND_Init().on('success', function(res) {
 // Init이 완료된 후에야 아래 함수들이 실행 되므로, 클로져 안에 위치해야 정상 동작
 console.log('restBind.REST_BIND_Init() succeeded\n\n');
 restBind.REST_BIND_CHAN_GetAllChannelList("all", "detailed").on('success', function(res) {
 console.log('restBind.REST_BIND_CHAN_GetAllChannelList() succeeded : ' + res);
 });

 });
 */

// node-ffi 1.2.6 일 경우의 사용 예제
// Sync Call 버전인 ffi.rest.test.js 와 비교하여 볼것,,,  함수명.sync( 인자, function(err, res){} );
var restBind = new ffi.Library("libnodebinder", {
    "REST_BIND_Init": ['void', []],
    "REST_BIND_CHAN_GetAllChannelList": ['string', ['string', 'string']],
    "REST_BIND_CHAN_GetChannelList": ['string', ['string', 'string', 'int', 'int']]
});

console.log('ffi.Library("libnodebinder") done!!\n');

restBind.REST_BIND_Init.async(function (err, res) {
    if (err) {
        console.log(err);
        throw err;
    }

    // Init이 완료된 후에야 아래 함수들이 실행 되므로, 클로져 안에 위치해야 정상 동작
    console.log('restBind.REST_BIND_Init() succeeded\n\n');
    restBind.REST_BIND_CHAN_GetAllChannelList.async('all', 'detailed', function (err, res) {
        if (err) {
            console.log(err);
            throw err;
        }
        console.log('restBind.REST_BIND_CHAN_GetAllChannelList() succeeded : ' + res);
    });

});




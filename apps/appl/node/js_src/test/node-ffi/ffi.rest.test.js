'use strict';

var ffi = require('node-ffi');

console.log('ffi.Library("libnodebinder") loading...\n');

var restBind = new ffi.Library("libnodebinder", {
    "REST_BIND_Init": ['void', []],
    "REST_BIND_CHAN_GetAllChannelList": ['string', ['string', 'string']],
    "REST_BIND_CHAN_GetChannelList": ['string', ['string', 'string', 'int', 'int']]
});

console.log('ffi.Library("libnodebinder") done!!\n');

restBind.REST_BIND_Init();
console.log('restBind.REST_BIND_Init() succeeded\n\n');

var chanString = restBind.REST_BIND_CHAN_GetAllChannelList('all', 'detailed');
console.log('restBind.REST_BIND_CHAN_GetAllChannelList() succeeded : ' + chanString);



'use strict';

var ffi = require('node-ffi');

var libtest = ffi.Library('libcbtest', {
    'testMsg': ['void', ['string']],
    'registCb': [ 'void', ['pointer']],
    'useTestCb': ['void', ['int']]
});


// node-ffi 0.5.7 일 경우의 사용 방법
/*
 var cb = new ffi.Callback(['void', ['int']], callback );
 libtest.registCb(cb.pointer);
 libtest.useTestCb(30);
 */

// node-ffi 1.2.6 일 경우의 사용 방법
var cb = ffi.Callback('void', ['int'], callback);
libtest.registCb(cb);
libtest.useTestCb(30);


function callback(count) {
    console.log('[JS Callback] Callback Run!!( %d )', count);
}


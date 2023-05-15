'use strict';

var ffi = require('node-ffi');

var hlib = new ffi.Library("libhlib", {
    "HLIB_STD_StrUpper": ['string', ['string']],
    "HLIB_STD_StrLower": ['string', ['string']],
    "HLIB_STD_StrReplace": ['string', ['string', 'string', 'string', 'string']],
    "HLIB_STD_IsInteger": ['int', ['string']]
});


console.log('ffi.Library("libhlib") SUCCESS!!!!\n\n\n');
var testString = 'Test String';
var strUpper = hlib.HLIB_STD_StrUpper(testString);
var strLower = hlib.HLIB_STD_StrLower(testString);

var dst = ' ';
var result = hlib.HLIB_STD_StrReplace(dst, testString, 'Test', 'Good');


console.log('HLIB_STD_StrUpper("%s") = %s', testString, strUpper);
console.log('HLIB_STD_StrLower("%s") = %s', testString, strLower);
console.log('HLIB_STD_StrReplace() = %s', result);
console.log('HLIB_STD_IsInteger("1234") = ' + hlib.HLIB_STD_IsInteger("1234"));
console.log('HLIB_STD_IsInteger("Test") = ' + hlib.HLIB_STD_IsInteger("Test"));


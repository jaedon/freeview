/**
 * Created by ssyun on 2014-07-22.
 */
'use strict';

var sSTB_IP = process.env['STB_IP'];
console.log('STB_IP=' + sSTB_IP);

var IP = sSTB_IP || '192.168.0.15';
var PORT = 37564;
var DOMAIN = 'http://' + IP + ':' + PORT;
var PINCODE = '0000';

exports.DOMAIN = DOMAIN;
exports.PINCODE = PINCODE;
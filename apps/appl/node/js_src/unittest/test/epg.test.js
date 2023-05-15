/**
 * Created by ssyun on 2014-08-12.
 */
'use strict';
var request = require('request');
var assert = require('assert');

var DOMAIN = require('./server.info').DOMAIN;


// EPG-API에 대한 Unit Test
describe('EPG API Test', function() {
    describe('GET :: /stb-api/v1/epg', function() {

        // http status 200, error code 0 으로 응답 해야 함.
        it('should be responded with status 200 and error code 0', function(done) {
            request(DOMAIN + '/stb-api/v1/epg', function(err, resp, body) {
                var bodyObj = JSON.parse(body);
                assert.equal(resp.statusCode, 200);
                assert.equal(bodyObj.error.errCode, 0);
                done();
            });
        });

        // "epgStatus" 프로퍼티가 포함되어 있어야 함.
        it('should be contain "epgStatus" property', function(done) {
            request(DOMAIN + '/stb-api/v1/epg', function(err, resp, body) {
                var bodyObj = JSON.parse(body);
                assert.ok(bodyObj.epgStatus);
                done();
            });
        });

        // 잘못된 parameter를 전달할 경우 http status 405, error code 21 로 응답 해야 함.
        it('if key-param is wrong should be respond with status 405 and error code 21', function(done) {
            request(DOMAIN + '/stb-api/v1/epg?wrongkey=test', function(err, resp, body) {
                var bodyObj = JSON.parse(body);
                assert.equal(resp.statusCode, 405);
                assert.equal(bodyObj.error.errCode, 21);
                done();
            });
        });


    });

});
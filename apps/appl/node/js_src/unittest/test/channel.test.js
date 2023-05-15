/**
 * Created by ssyun on 2014-07-22.
 */
'use strict';
var request = require('request');
var assert = require('assert');

var DOMAIN = require('./server.info').DOMAIN;


// Channel-API에 대한 Unit Test
describe('Channel API Test', function() {
    describe('GET :: /stb-api/v1/channels', function() {

        // http status 200, error code 0 으로 응답 해야 함.
        it('should be responded with status 200 and error code 0', function(done) {
            request(DOMAIN + '/stb-api/v1/channels', function(err, resp, body) {
                var bodyObj = JSON.parse(body);
                assert.equal(resp.statusCode, 200);
                assert.equal(bodyObj.error.errCode, 0);
                done();
            });
        });

        // "chanListStatus" 프로퍼티가 포함되어 있어야 함.
        it('should be contain "chanListStatus" property', function(done) {
            request(DOMAIN + '/stb-api/v1/channels', function(err, resp, body) {
                var bodyObj = JSON.parse(body);
                assert.ok(bodyObj.chanListStatus);
                done();
            });
        });

        // 잘못된 parameter를 전달할 경우 http status 405, error code 21 로 응답 해야 함.
        it('if key-param is wrong should be respond with status 405 and error code 21', function(done) {
            request(DOMAIN + '/stb-api/v1/channels?wrongkey=test', function(err, resp, body) {
                var bodyObj = JSON.parse(body);
                assert.equal(resp.statusCode, 405);
                assert.equal(bodyObj.error.errCode, 21);
                done();
            });
        });

        // 잘못된 parameter 쌍을 전달할 경우 http status 405, error code 21 로 응답 해야 함.
        it('if key-pair is wrong should be respond with status 405 and error code 21', function(done) {
            request(DOMAIN + '/stb-api/v1/channels?startidx=0&endno=100', function(err, resp, body) {
                var bodyObj = JSON.parse(body);
                assert.equal(resp.statusCode, 405);
                assert.equal(bodyObj.error.errCode, 21);
                done();
            });
        });

        // Channel의 갯수는 "maxTransferableChannel" 속성보다 작아야 한다.
        it('should be channel counts less than "maxTransferableChannel" property', function(done) {
            request(DOMAIN + '/stb-api/v1/channels', function(err, resp, body) {
                var bodyObj = JSON.parse(body);
                var maxTransferableChannel = bodyObj.chanListStatus.maxTransferableChannel;

                request(DOMAIN + '/stb-api/v1/channels?startidx=0', function(err, resp, body) {
                    var bodyObj = JSON.parse(body);
                    assert.ok((maxTransferableChannel >= bodyObj.Channels.length));
                    done();
                });
            });
        });

        // "cnt" 파라메터가 존재하는 경우, 결과값은 cnt 값보다 작거나 같아야 한다.
        it('if there is "cnt" parameter should be that channel counts less than "cnt"', function(done) {
            var cnt = 5;
            request(DOMAIN + '/stb-api/v1/channels?startidx=0&cnt=' + cnt, function(err, resp, body) {
                var bodyObj = JSON.parse(body);
                assert.ok((bodyObj.Channels.length <= cnt));
                done();
            });
        });

    });

    describe('GET :: /stb-api/v1/channels/[UID]', function() {

        // channels/1 에 대하여 적당한 error code를 제공해야 한다.
        it('should be return to proper error code about channel 1', function(done) {
            request(DOMAIN + '/stb-api/v1/channels/1', function (err, resp, body) {
                var bodyObj = JSON.parse(body);
                if (resp.statusCode === 200) {
                    assert.equal(bodyObj.error.errCode, 0);
                } else if (resp.statusCode === 404) {
                    assert.equal(bodyObj.error.errCode, 13);
                } else {
                    assert.fail();
                }
                done();
            });
        });

        // channels/10 에 대하여 적당한 error code를 제공해야 한다.
        it('should be return to proper error code about channel 10', function(done) {
            request(DOMAIN + '/stb-api/v1/channels/10', function (err, resp, body) {
                var bodyObj = JSON.parse(body);
                if (resp.statusCode === 200) {
                    assert.equal(bodyObj.error.errCode, 0);
                } else if (resp.statusCode === 404) {
                    assert.equal(bodyObj.error.errCode, 13);
                } else {
                    assert.fail();
                }
                done();
            });
        });

        // channels/100 에 대하여 적당한 error code를 제공해야 한다.
        it('should be return to proper error code about channel 100', function(done) {
            request(DOMAIN + '/stb-api/v1/channels/100', function (err, resp, body) {
                var bodyObj = JSON.parse(body);
                if (resp.statusCode === 200) {
                    assert.equal(bodyObj.error.errCode, 0);
                } else if (resp.statusCode === 404) {
                    assert.equal(bodyObj.error.errCode, 13);
                } else {
                    assert.fail();
                }
                done();
            });
        });

        // channels/1000 에 대하여 적당한 error code를 제공해야 한다.
        it('should be return to proper error code about channel 1000', function(done) {
            request(DOMAIN + '/stb-api/v1/channels/1000', function (err, resp, body) {
                var bodyObj = JSON.parse(body);
                if (resp.statusCode === 200) {
                    assert.equal(bodyObj.error.errCode, 0);
                } else if (resp.statusCode === 404) {
                    assert.equal(bodyObj.error.errCode, 13);
                } else {
                    assert.fail();
                }
                done();
            });
        });

        // channels/10000 에 대하여 적당한 error code를 제공해야 한다.
        it('should be return to proper error code about channel 10000', function(done) {
            request(DOMAIN + '/stb-api/v1/channels/10000', function (err, resp, body) {
                var bodyObj = JSON.parse(body);
                if (resp.statusCode === 200) {
                    assert.equal(bodyObj.error.errCode, 0);
                } else if (resp.statusCode === 404) {
                    assert.equal(bodyObj.error.errCode, 13);
                } else {
                    assert.fail();
                }
                done();
            });
        });

        // UID가 존재하지 않는 경우 http status 404, error code 13 으로 응답 해야 함.
        it('if UID is not exists should be responded with status 404 and error code 13', function(done) {
            request(DOMAIN + '/stb-api/v1/channels/999999', function (err, resp, body) {
                var bodyObj = JSON.parse(body);
                assert.equal(resp.statusCode, 404);
                assert.equal(bodyObj.error.errCode, 13);
                done();
            });
        });

    });

});
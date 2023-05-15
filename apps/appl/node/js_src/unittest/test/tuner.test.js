/**
 * Created by ssyun on 2014-07-28.
 */
'use strict';
var request = require('request');
var assert = require('assert');

var DOMAIN = require('./server.info').DOMAIN;


// Tuner-API에 대한 Unit Test
describe('Tuner API Test', function() {
    describe('GET :: /stb-api/v1/tuners', function() {

        // http status 200, error code 0 으로 응답 해야 함.
        it('should be responded with status 200 and error code 0', function(done) {
            request(DOMAIN + '/stb-api/v1/tuners', function(err, resp, body) {
                var bodyObj = JSON.parse(body);
                assert.equal(resp.statusCode, 200);
                assert.equal(bodyObj.error.errCode, 0);
                done();
            });
        });

        // "tunersOverview" 프로퍼티가 포함되어 있어야 함.
        it('should be contain "tunersOverview" property', function(done) {
            request(DOMAIN + '/stb-api/v1/tuners', function(err, resp, body) {
                var bodyObj = JSON.parse(body);
                assert.ok(bodyObj.tunersOverview);
                done();
            });
        });

        // 잘못된 parameter를 전달할 경우 http status 405, error code 21 로 응답 해야 함.
        it('if key-param is wrong should be respond with status 405 and error code 21', function(done) {
            request(DOMAIN + '/stb-api/v1/tuners?wrongkey=test', function(err, resp, body) {
                var bodyObj = JSON.parse(body);
                assert.equal(resp.statusCode, 405);
                assert.equal(bodyObj.error.errCode, 21);
                done();
            });
        });

        // Tuner의 갯수는 "tunerNum" 속성보다 작아야 한다.
        it('should be tuner counts less than "tunerNum" property', function(done) {
            request(DOMAIN + '/stb-api/v1/tuners', function(err, resp, body) {
                var bodyObj = JSON.parse(body);
                var maxTunerNum = bodyObj.tunersOverview.tunerNum;

                request(DOMAIN + '/stb-api/v1/tuners?startidx=0', function(err, resp, body) {
                    var bodyObj = JSON.parse(body);
                    assert.ok((maxTunerNum >= bodyObj.Tuners.length));
                    done();
                });
            });
        });

        // "cnt" 파라메터가 존재하는 경우, 결과값은 cnt 값보다 작거나 같아야 한다.
        it('if there is "cnt" parameter should be that tuner counts less than "cnt"', function(done) {
            var cnt = 2;
            request(DOMAIN + '/stb-api/v1/tuners?startidx=0&cnt=' + cnt, function(err, resp, body) {
                var bodyObj = JSON.parse(body);
                assert.ok((bodyObj.Tuners.length <= cnt));
                done();
            });
        });

    });

    describe('GET :: /stb-api/v1/tuners/[UID]', function() {

        // tuner/0 에 대하여 적당한 error code를 제공해야 한다.
        it('should be return to proper error code about tuner 0', function(done) {
            request(DOMAIN + '/stb-api/v1/tuners/0', function (err, resp, body) {
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

        // tuner/1 에 대하여 적당한 error code를 제공해야 한다.
        it('should be return to proper error code about tuner 1', function(done) {
            request(DOMAIN + '/stb-api/v1/tuners/1', function (err, resp, body) {
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

        // tuner/2 대하여 적당한 error code를 제공해야 한다.
        it('should be return to proper error code about tuner 2', function(done) {
            request(DOMAIN + '/stb-api/v1/tuners/2', function (err, resp, body) {
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

        // tuner/5 에 대하여 적당한 error code를 제공해야 한다.
        it('should be return to proper error code about tuner 5', function(done) {
            request(DOMAIN + '/stb-api/v1/tuners/5', function (err, resp, body) {
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

        // tuners/10 에 대하여 적당한 error code를 제공해야 한다.
        it('should be return to proper error code about tuners 10', function(done) {
            request(DOMAIN + '/stb-api/v1/tuners/10', function (err, resp, body) {
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
            request(DOMAIN + '/stb-api/v1/tuners/999999', function (err, resp, body) {
                var bodyObj = JSON.parse(body);
                assert.equal(resp.statusCode, 404);
                assert.equal(bodyObj.error.errCode, 13);
                done();
            });
        });

        // tuner가 lock 되어 있는 경우 tunerTransponder를 포함하여 추가 정보가 있어야 한다.
        it('if tuner is locked should be contain "tunerTransponder" property', function(done) {
            request(DOMAIN + '/stb-api/v1/tuners/0', function (err, resp, body) {
                var bodyObj = JSON.parse(body);
                if (bodyObj.tunerStatus.locked === 'undefined') {
                    assert.fail();
                } else {
                    if (bodyObj.tunerStatus.locked === 1) {
                        assert.ok(bodyObj.tunerStatus.tunedTransponder);
                    } else {
                        assert.notEqual(bodyObj.tunerStatus.tunedTransponder, true);
                    }
                }
                done();
            });
        });

    });

});
/**
 * Created by ssyun on 2014-07-22.
 */
'use strict';
var request = require('request');
var assert = require('assert');

var DOMAIN = require('./server.info').DOMAIN;


// Player-API에 대한 Unit Test
describe('Player API Test', function() {

    describe('GET :: /stb-api/v1/players', function () {

        // http status 200, error code 0 으로 응답 해야 함.
        it('should be responded with status 200 and error code 0', function(done) {
            request(DOMAIN + '/stb-api/v1/players', function(err, resp, body) {
                var bodyObj = JSON.parse(body);
                assert.equal(resp.statusCode, 200);
                assert.equal(bodyObj.error.errCode, 0);
                done();
            });
        });

        // "playersOverview" 프로퍼티가 포함되어 있어야 함.
        it('should be contain "playersOverview" property', function(done) {
            request(DOMAIN + '/stb-api/v1/players', function(err, resp, body) {
                var bodyObj = JSON.parse(body);
                assert.ok(bodyObj.playersOverview);
                done();
            });
        });

        // 잘못된 parameter를 전달할 경우 http status 405, error code 21 로 응답 해야 함.
        it('if key-param is wrong should be respond with status 405 and error code 21', function(done) {
            request(DOMAIN + '/stb-api/v1/players?wrongkey=test', function(err, resp, body) {
                var bodyObj = JSON.parse(body);
                assert.equal(resp.statusCode, 405);
                assert.equal(bodyObj.error.errCode, 21);
                done();
            });
        });

    });

    describe('GET :: /stb-api/v1/players/[UID]', function() {

        // 존재하는 모든 Player 리소스에 대하여 http status 200, error code 0으로 응답해야 함.
        it('should be responded with status 200 and error code 0 - each player', function(done) {
            request(DOMAIN + '/stb-api/v1/players', function(err, resp, body) {
                var bodyObj = JSON.parse(body);
                assert.ok(bodyObj.playersOverview);
                var numberOfPlayers = bodyObj.playersOverview.numberOfPlayers;

                for(var i = 0 ; i < numberOfPlayers ; ++i) {
                    request(DOMAIN + '/stb-api/v1/players/' + i, function(err, resp, body) {
                        var bodyObj = JSON.parse(body);
                        assert.equal(resp.statusCode, 200);
                        assert.equal(bodyObj.error.errCode, 0);
                    });
                }
                done();
            });
        });

        // 존재하지 않는 Player 리소스 요청시 http status 404, error code 13으로 응답해야 함.
        it('should be responded with status 404 and error code 13', function(done) {
            request(DOMAIN + '/stb-api/v1/players', function(err, resp, body) {
                var bodyObj = JSON.parse(body);
                assert.ok(bodyObj.playersOverview);
                var numberOfPlayers = bodyObj.playersOverview.numberOfPlayers;

                request(DOMAIN + '/stb-api/v1/players/' + numberOfPlayers, function(err, resp, body) {
                    var bodyObj = JSON.parse(body);
                    assert.equal(resp.statusCode, 404);
                    assert.equal(bodyObj.error.errCode, 13);
                    done();
                });
            });
        });

    });

    //TODO: PUT Method에 대한 TC 작성...
});
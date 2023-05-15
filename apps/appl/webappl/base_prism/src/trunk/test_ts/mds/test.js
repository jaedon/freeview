/**
 * Created by bglee on 2015-03-20.
 */
var Q = require('q');
var requirejs = require('requirejs');
var MDS = require('./mds');
requirejs.config({
    nodeRequire: require
})
describe("MDS API Test", function() {
    var mds = new MDS();
    it("IT", function(done) {
        console.log("KKK");
        if (mds) {
            done();
        }
    })
});
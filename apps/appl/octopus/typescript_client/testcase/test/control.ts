/// <reference path="../node_modules/octopus-client/octopus.d.ts" />

/// <reference path="../def/node.d.ts" />
/// <reference path="../def/mocha.d.ts" />
/// <reference path="../def/async.d.ts" />
/// <reference path="../def/should.d.ts" />

var should = require('should');
var octopus = require('octopus-client');

describe('control', function() {
    var outputaudio;
    before((done) => {
        outputaudio = new octopus.COutputAudio();

        setTimeout(() => {
            console.log('fixture set up');
            done();
        }, 500);
    });

    it('audio', (done) => {

        var testValue = 55;
        outputaudio.SetVolume(testValue, () => {});

        setTimeout(() => {
            outputaudio.GetVolume(volume => {

                should(testValue == volume).ok;
                done();
            });
        }, 1000);
    });
});
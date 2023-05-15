/*
 * (c) 1997-2015 Netflix, Inc.  All content herein is protected by
 * U.S. copyright and other applicable intellectual property laws and
 * may not be copied without the express permission of Netflix, Inc.,
 * which reserves all rights.  Reuse of any of this content for any
 * purpose without the permission of Netflix, Inc. is strictly
 * prohibited.
 */

nrdp.input = {

    addEventListener: function(evt, listener) { nrdp._addEventListener(this, evt, listener); },
    removeEventListener: function(evt, listener) { nrdp._removeEventListener(this, evt, listener); },

    supportsVoice: function(value) {
        nrdp._invoke("input", "supportsVoice", {value:value});
    },

    setVoiceGrammar: function(grammar) {
        nrdp._invoke("input", "setVoiceGrammar", {grammar:grammar});
    },

    _handleEvent: function(event) {

        if (event.name == "voice") {
            nrdp._callEventListeners(this, event.data);
        } else {
            return false;
        }

        return true;
    }
};

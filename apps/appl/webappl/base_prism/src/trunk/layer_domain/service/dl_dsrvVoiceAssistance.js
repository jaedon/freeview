/// <reference path="../../def/q.d.ts" />
/// <reference path="../../def/jquery.d.ts" />
/// <reference path="../../def/sugar.d.ts" />
/// <reference path="../../def/prism.d.ts" />
// file name : fw/service/HMX/dservice/dl_dsrvVoiceAssistance.js
/*jslint nomen:true, vars:true */
//    MQTT: ['layer_domain/service/dl_dsrvVoiceAssistance.js'],
var __extends = this.__extends || function (d, b) {
    for (var p in b) if (b.hasOwnProperty(p)) d[p] = b[p];
    function __() { this.constructor = d; }
    __.prototype = b.prototype;
    d.prototype = new __();
};
define(["require", "exports", "./dl_kernel", "../../lib/local/mqtt", "../../lib/ext/moment.min"], function (require, exports, __KERNEL__, __MQTT__, __MOMENT__) {
    var EAlexaRequestType;
    (function (EAlexaRequestType) {
        EAlexaRequestType[EAlexaRequestType["NONE"] = 0] = "NONE";
        EAlexaRequestType[EAlexaRequestType["CHANGECHANNEL"] = 1] = "CHANGECHANNEL";
        EAlexaRequestType[EAlexaRequestType["SKIPCHANNELS"] = 2] = "SKIPCHANNELS";
        EAlexaRequestType[EAlexaRequestType["TURNOFF"] = 3] = "TURNOFF";
        EAlexaRequestType[EAlexaRequestType["TURNON"] = 4] = "TURNON";
        EAlexaRequestType[EAlexaRequestType["FAST_FORWARD"] = 5] = "FAST_FORWARD";
        EAlexaRequestType[EAlexaRequestType["NEXT"] = 6] = "NEXT";
        EAlexaRequestType[EAlexaRequestType["PAUSE"] = 7] = "PAUSE";
        EAlexaRequestType[EAlexaRequestType["PLAY"] = 8] = "PLAY";
        EAlexaRequestType[EAlexaRequestType["PREVIOUS"] = 9] = "PREVIOUS";
        EAlexaRequestType[EAlexaRequestType["REWIND"] = 10] = "REWIND";
        EAlexaRequestType[EAlexaRequestType["STARTOVER"] = 11] = "STARTOVER";
        EAlexaRequestType[EAlexaRequestType["STOP"] = 12] = "STOP";
        EAlexaRequestType[EAlexaRequestType["SET_VOLUME"] = 13] = "SET_VOLUME";
        EAlexaRequestType[EAlexaRequestType["ADJUST_VOLUME"] = 14] = "ADJUST_VOLUME";
        EAlexaRequestType[EAlexaRequestType["SET_MUTE"] = 15] = "SET_MUTE";
        EAlexaRequestType[EAlexaRequestType["SEARCH_AND_DISPLAY_RESULTS"] = 16] = "SEARCH_AND_DISPLAY_RESULTS";
        EAlexaRequestType[EAlexaRequestType["SEARCH_AND_PLAY"] = 17] = "SEARCH_AND_PLAY"; // Not yet
    })(EAlexaRequestType || (EAlexaRequestType = {}));
    var CAlexaHumaxInterfaceAssistance = (function () {
        function CAlexaHumaxInterfaceAssistance(msg) {
            if (msg) {
                this.header = msg.header;
                this.payload = msg.payload;
                if (msg.header && msg.header.name) {
                    this.request = msg.header.name;
                }
            }
        }
        CAlexaHumaxInterfaceAssistance.prototype.operate = function (va) {
            throw new Error('This function is abstract');
        };
        CAlexaHumaxInterfaceAssistance.prototype.isValidRequest = function () {
            var that = this;
            if (!that.header || !that.payload) {
                return false;
            }
            if (that.validRequest.indexOf(that.request) !== -1) {
                return true;
            }
            return false;
        };
        CAlexaHumaxInterfaceAssistance.prototype.makePublishMsg = function (responseValue) {
            var that = this;
            var msg = {
                header: that.header,
                context: {
                    properties: []
                }
            };
            var responseCtxs = null;
            if (!that.isValidRequest()) {
                return null;
            }
            responseCtxs = that._makeMCFResponseContexts(responseValue);
            if (responseCtxs) {
                var i = 0, length = responseCtxs.length;
                for (i = 0; i < length; i++) {
                    msg.context.properties.push(responseCtxs[i]);
                }
            }
            return JSON.stringify(msg);
        };
        CAlexaHumaxInterfaceAssistance.prototype.getProcessData = function (param) {
            var that = this;
            var type = that._getRequestType();
            if (type === 0 /* NONE */) {
                return null;
            }
            if (param) {
                return { 'name': type, 'data': param };
            }
            else {
                return { 'name': type };
            }
        };
        CAlexaHumaxInterfaceAssistance.prototype._getAmzDate = function () {
            var time = null, dateStamp = null, amzdate = null;
            time = __MOMENT__.utc();
            dateStamp = time.format('YYYY-MM-DD');
            amzdate = dateStamp + 'T' + time.format('HH:mm:ss') + 'Z';
            return amzdate;
        };
        CAlexaHumaxInterfaceAssistance.prototype._makeMCFResponseContexts = function (responseValue) {
            return null;
        };
        CAlexaHumaxInterfaceAssistance.prototype.getInformation = function () {
            var hederStr = 'namespace : ' + this.header.namespace + ' , name : ' + this.header.name;
            return '</br>' + hederStr + '</br>' + JSON.stringify(this.payload);
        };
        CAlexaHumaxInterfaceAssistance.prototype._getRequestType = function () {
            var that = this;
            var type = 0 /* NONE */;
            if (that.isValidRequest()) {
                switch (that.request) {
                    case CAlexaHumaxInterfaceAssistance.HEADER.NAME.TURNOFF:
                        type = 3 /* TURNOFF */;
                        break;
                    case CAlexaHumaxInterfaceAssistance.HEADER.NAME.TURNON:
                        type = 4 /* TURNON */;
                        break;
                    case CAlexaHumaxInterfaceAssistance.HEADER.NAME.FAST_FORWARD:
                        type = 5 /* FAST_FORWARD */;
                        break;
                    case CAlexaHumaxInterfaceAssistance.HEADER.NAME.PAUSE:
                        type = 7 /* PAUSE */;
                        break;
                    case CAlexaHumaxInterfaceAssistance.HEADER.NAME.PLAY:
                        type = 8 /* PLAY */;
                        break;
                    case CAlexaHumaxInterfaceAssistance.HEADER.NAME.REWIND:
                        type = 10 /* REWIND */;
                        break;
                    case CAlexaHumaxInterfaceAssistance.HEADER.NAME.STOP:
                        type = 12 /* STOP */;
                        break;
                    case CAlexaHumaxInterfaceAssistance.HEADER.NAME.ADJUST_VOLUME:
                        type = 14 /* ADJUST_VOLUME */;
                        break;
                    case CAlexaHumaxInterfaceAssistance.HEADER.NAME.SET_MUTE:
                        type = 15 /* SET_MUTE */;
                        break;
                    default:
                        break;
                }
            }
            return type;
        };
        CAlexaHumaxInterfaceAssistance.HEADER = {
            NAMESPACE: {
                CHANNELCONTROLLER: "Alexa.ChannelController",
                POWERCONTROLLER: "Alexa.PowerController",
                PLAYBACKCONTROLLER: "Alexa.PlaybackController",
                SPEAKER: "Alexa.Speaker",
                REMOTEVIDEOPLAYER: "Alexa.RemoteVideoPlayer"
            },
            NAME: {
                CHANGECHANNEL: "ChangeChannel",
                SKIPCHANNELS: "SkipChannels",
                TURNOFF: "TurnOff",
                TURNON: "TurnOn",
                FAST_FORWARD: 'FastForward',
                NEXT: 'Next',
                PAUSE: 'Pause',
                PLAY: 'Play',
                PREVIOUS: 'Previous',
                REWIND: 'Rewind',
                STARTOVER: 'StartOver',
                STOP: 'Stop',
                SET_VOLUME: 'SetVolume',
                ADJUST_VOLUME: 'AdjustVolume',
                SET_MUTE: 'SetMute',
                SEARCH_AND_DISPLAY_RESULUTS: 'SearchAndDisplayResults',
                SEARCH_AND_PLAY: 'SearchAndPlay',
            }
        };
        return CAlexaHumaxInterfaceAssistance;
    })();
    var CAlexaHumaxChannelAssistance = (function (_super) {
        __extends(CAlexaHumaxChannelAssistance, _super);
        function CAlexaHumaxChannelAssistance(msg) {
            _super.call(this, msg);
            this.validRequest = [CAlexaHumaxInterfaceAssistance.HEADER.NAME.CHANGECHANNEL, CAlexaHumaxInterfaceAssistance.HEADER.NAME.SKIPCHANNELS];
        }
        CAlexaHumaxChannelAssistance.prototype.operate = function (va) {
            var that = this;
            if (!va) {
                return;
            }
            if (!that.isValidRequest()) {
                return;
            }
            switch (that.request) {
                case CAlexaHumaxInterfaceAssistance.HEADER.NAME.SKIPCHANNELS:
                    var channelCount = that._getPayloadChannelCount();
                    va.fireEvent(CAlexaHumaxChannelAssistance.CHANNEL_ACTION.SKIPCHANNELS, Number(channelCount));
                    break;
                case CAlexaHumaxInterfaceAssistance.HEADER.NAME.CHANGECHANNEL:
                    var channel = that._getPayloadChannel();
                    var channelMetadata = that._getPayloadChannelMetadata();
                    var value = that._getValue(channel, CAlexaHumaxChannelAssistance.CHANNEL.NUMBER);
                    if (value) {
                        va.fireEvent(CAlexaHumaxChannelAssistance.CHANNEL_ACTION.CHANGE_CHANNEL_BY_NUMBER, Number(value));
                    }
                    else {
                        var callsign = that._getValue(channel, CAlexaHumaxChannelAssistance.CHANNEL.CALLSIGN) ? that._getValue(channel, CAlexaHumaxChannelAssistance.CHANNEL.CALLSIGN) : that._getValue(channel, CAlexaHumaxChannelAssistance.CHANNEL.AFFILIATECALLSIGN);
                        var name = that._getValue(channelMetadata, CAlexaHumaxChannelAssistance.CHANNELMETADATA.NAME);
                        va.fireEvent(CAlexaHumaxChannelAssistance.CHANNEL_ACTION.CHANGE_CHANNEL_BY_NAME, {
                            'callsign': callsign,
                            'name': name
                        });
                    }
                    break;
                default:
                    break;
            }
        };
        CAlexaHumaxChannelAssistance.prototype._getPayloadChannelCount = function () {
            var that = this;
            var channelCount = 0;
            try {
                channelCount = that.payload.channelCount;
            }
            catch (e) {
                hx.log('error', 'exception!!! _getPayloadChannelCount');
                return 0;
            }
            return channelCount;
        };
        CAlexaHumaxChannelAssistance.prototype._getPayloadChannel = function () {
            var that = this;
            var container = null;
            try {
                container = that.payload.channel;
            }
            catch (e) {
                hx.log('error', 'exception!!! _getPayloadChannel');
                return null;
            }
            return container;
        };
        CAlexaHumaxChannelAssistance.prototype._getPayloadChannelMetadata = function () {
            var that = this;
            var container = null;
            try {
                container = that.payload.channelMetadata;
            }
            catch (e) {
                hx.log('error', 'exception!!! _getPayloadChannelMetadata');
                return null;
            }
            return container;
        };
        CAlexaHumaxChannelAssistance.prototype._getValue = function (container, key) {
            if (container) {
                return container[key];
            }
            else {
                return '';
            }
        };
        CAlexaHumaxChannelAssistance.prototype._makeMCFResponseContexts = function (responseValue) {
            var that = this;
            var channel = that._getPayloadChannel();
            var context = {
                'namespace': that.header.namespace,
                'name': "channel",
                'value': {
                    'number': responseValue && responseValue.number ? responseValue.number : that._getValue(channel, CAlexaHumaxChannelAssistance.CHANNEL.NUMBER),
                    'callSign': responseValue && responseValue.callSign ? responseValue.callSign : that._getValue(channel, CAlexaHumaxChannelAssistance.CHANNEL.CALLSIGN),
                    'affiliateCallSign': responseValue && responseValue.affiliateCallSign ? responseValue.affiliateCallSign : that._getValue(channel, CAlexaHumaxChannelAssistance.CHANNEL.AFFILIATECALLSIGN)
                },
                'timeOfSample': that._getAmzDate(),
                'uncertaintyInMilliseconds': 200
            };
            return [context];
        };
        CAlexaHumaxChannelAssistance.CHANNEL = {
            NUMBER: "number",
            CALLSIGN: "callSign",
            AFFILIATECALLSIGN: "affiliateCallSign",
            URI: "uri",
        };
        CAlexaHumaxChannelAssistance.CHANNELMETADATA = {
            NAME: "name",
            IMAGE: "image"
        };
        CAlexaHumaxChannelAssistance.CHANNEL_ACTION = {
            SKIPCHANNELS: 'SkipChannels',
            CHANGE_CHANNEL_BY_NUMBER: 'ChangeChannelByNumber',
            CHANGE_CHANNEL_BY_NAME: 'ChangeChannelByName'
        };
        return CAlexaHumaxChannelAssistance;
    })(CAlexaHumaxInterfaceAssistance);
    var CAlexaHumaxPowerAssistance = (function (_super) {
        __extends(CAlexaHumaxPowerAssistance, _super);
        function CAlexaHumaxPowerAssistance(msg) {
            _super.call(this, msg);
            this.validRequest = [CAlexaHumaxInterfaceAssistance.HEADER.NAME.TURNOFF, CAlexaHumaxInterfaceAssistance.HEADER.NAME.TURNON];
        }
        CAlexaHumaxPowerAssistance.prototype.operate = function (va) {
            var that = this;
            if (!va) {
                return;
            }
            if (!that.isValidRequest()) {
                return;
            }
            va.fireEvent(that.request);
        };
        CAlexaHumaxPowerAssistance.prototype._makeMCFResponseContexts = function () {
            var that = this;
            var context = null;
            var value = null;
            if (that.request === CAlexaHumaxInterfaceAssistance.HEADER.NAME.TURNON) {
                value = 'ON';
            }
            else if (that.request === CAlexaHumaxInterfaceAssistance.HEADER.NAME.TURNOFF) {
                value = 'OFF';
            }
            context = {
                'namespace': that.header.namespace,
                'name': "powerState",
                'value': value,
                'timeOfSample': that._getAmzDate(),
                'uncertaintyInMilliseconds': 200
            };
            return [context];
        };
        return CAlexaHumaxPowerAssistance;
    })(CAlexaHumaxInterfaceAssistance);
    var CAlexaHumaxPlaybackAssistance = (function (_super) {
        __extends(CAlexaHumaxPlaybackAssistance, _super);
        function CAlexaHumaxPlaybackAssistance(msg) {
            _super.call(this, msg);
            this.validRequest = [CAlexaHumaxInterfaceAssistance.HEADER.NAME.FAST_FORWARD, CAlexaHumaxInterfaceAssistance.HEADER.NAME.NEXT, CAlexaHumaxInterfaceAssistance.HEADER.NAME.PAUSE, CAlexaHumaxInterfaceAssistance.HEADER.NAME.PLAY, CAlexaHumaxInterfaceAssistance.HEADER.NAME.PREVIOUS, CAlexaHumaxInterfaceAssistance.HEADER.NAME.REWIND, CAlexaHumaxInterfaceAssistance.HEADER.NAME.STARTOVER, CAlexaHumaxInterfaceAssistance.HEADER.NAME.STOP];
        }
        CAlexaHumaxPlaybackAssistance.prototype.operate = function (va) {
            var that = this;
            if (!va) {
                return;
            }
            if (!that.isValidRequest()) {
                return;
            }
            switch (that.request) {
                case CAlexaHumaxChannelAssistance.HEADER.NAME.NEXT:
                case CAlexaHumaxChannelAssistance.HEADER.NAME.PREVIOUS:
                case CAlexaHumaxChannelAssistance.HEADER.NAME.STARTOVER:
                    va.fireEvent(that.request);
                    break;
                default:
                    va.processData();
                    va.publish();
                    break;
            }
        };
        return CAlexaHumaxPlaybackAssistance;
    })(CAlexaHumaxInterfaceAssistance);
    var CAlexaHumaxVolumeAssistance = (function (_super) {
        __extends(CAlexaHumaxVolumeAssistance, _super);
        function CAlexaHumaxVolumeAssistance(msg) {
            _super.call(this, msg);
            this.validRequest = [CAlexaHumaxInterfaceAssistance.HEADER.NAME.SET_VOLUME, CAlexaHumaxInterfaceAssistance.HEADER.NAME.SET_MUTE, CAlexaHumaxInterfaceAssistance.HEADER.NAME.ADJUST_VOLUME];
        }
        CAlexaHumaxVolumeAssistance.prototype.operate = function (va) {
            var that = this;
            var volume = 0;
            var data = null;
            if (!va) {
                return;
            }
            if (!that.isValidRequest()) {
                return;
            }
            switch (that.request) {
                case CAlexaHumaxInterfaceAssistance.HEADER.NAME.SET_MUTE:
                    va.fireEvent(that.request);
                    break;
                case CAlexaHumaxInterfaceAssistance.HEADER.NAME.ADJUST_VOLUME:
                    volume = that._getVolume();
                    va.processData(volume);
                    va.publish();
                    break;
                case CAlexaHumaxInterfaceAssistance.HEADER.NAME.SET_VOLUME:
                    break;
                default:
                    break;
            }
        };
        CAlexaHumaxVolumeAssistance.prototype._getVolume = function () {
            var that = this;
            var volume = 0;
            try {
                volume = that.payload.volume;
            }
            catch (e) {
                hx.log('error', 'exception!!! _getVolume');
            }
            return volume;
        };
        CAlexaHumaxVolumeAssistance.prototype._getMute = function () {
            var that = this;
            var mute = false;
            try {
                mute = that.payload.mute ? true : false;
            }
            catch (e) {
                hx.log('error', 'exception!!! _getVolume');
                return false;
            }
            return mute;
        };
        CAlexaHumaxVolumeAssistance.prototype._makeMCFResponseContexts = function () {
            var that = this;
            var contexts = [];
            var amzdate = that._getAmzDate();
            contexts.push({
                'namespace': that.header.namespace,
                'name': "volume",
                'value': that._getVolume(),
                'timeOfSample': amzdate,
                'uncertaintyInMilliseconds': 200
            });
            contexts.push({
                'namespace': that.header.namespace,
                'name': "muted",
                'value': that._getMute(),
                'timeOfSample': amzdate,
                'uncertaintyInMilliseconds': 200
            });
            return contexts;
        };
        return CAlexaHumaxVolumeAssistance;
    })(CAlexaHumaxInterfaceAssistance);
    var CAlexaHumaxRemoteVideoPlayerAssistance = (function (_super) {
        __extends(CAlexaHumaxRemoteVideoPlayerAssistance, _super);
        function CAlexaHumaxRemoteVideoPlayerAssistance(msg) {
            _super.call(this, msg);
            this.validRequest = [CAlexaHumaxInterfaceAssistance.HEADER.NAME.SEARCH_AND_DISPLAY_RESULUTS, CAlexaHumaxInterfaceAssistance.HEADER.NAME.SEARCH_AND_PLAY];
        }
        CAlexaHumaxRemoteVideoPlayerAssistance.prototype.operate = function (va) {
            var that = this;
            var entities = null;
            if (!va) {
                return;
            }
            if (!that.isValidRequest()) {
                return;
            }
            switch (that.request) {
                case CAlexaHumaxInterfaceAssistance.HEADER.NAME.SEARCH_AND_DISPLAY_RESULUTS:
                    entities = that._getEntitiesContainer();
                    if (entities && entities.length > 0) {
                        va.fireEvent(that.request, entities);
                    }
                    break;
                case CAlexaHumaxInterfaceAssistance.HEADER.NAME.SEARCH_AND_PLAY:
                    break;
                default:
                    break;
            }
        };
        CAlexaHumaxRemoteVideoPlayerAssistance.prototype._getEntitiesContainer = function () {
            var that = this;
            var entities = null; //array
            if (that.payload) {
                if (that.payload.entities.length > 0) {
                    entities = that.payload.entities;
                }
            }
            return entities;
        };
        return CAlexaHumaxRemoteVideoPlayerAssistance;
    })(CAlexaHumaxInterfaceAssistance);
    var CAssistanceFactoryAlexaHumaxInterface = (function () {
        function CAssistanceFactoryAlexaHumaxInterface() {
        }
        CAssistanceFactoryAlexaHumaxInterface.prototype.makeAssistance = function (msg) {
            if (msg && msg.header && msg.payload) {
                switch (msg.header.namespace) {
                    case CAlexaHumaxInterfaceAssistance.HEADER.NAMESPACE.CHANNELCONTROLLER:
                        switch (msg.header.name) {
                            case CAlexaHumaxInterfaceAssistance.HEADER.NAME.CHANGECHANNEL:
                            case CAlexaHumaxInterfaceAssistance.HEADER.NAME.SKIPCHANNELS:
                                return new CAlexaHumaxChannelAssistance(msg);
                            default:
                                break;
                        }
                        break;
                    case CAlexaHumaxInterfaceAssistance.HEADER.NAMESPACE.POWERCONTROLLER:
                        switch (msg.header.name) {
                            case CAlexaHumaxInterfaceAssistance.HEADER.NAME.TURNON:
                            case CAlexaHumaxInterfaceAssistance.HEADER.NAME.TURNOFF:
                                return new CAlexaHumaxPowerAssistance(msg);
                            default:
                                break;
                        }
                        break;
                    case CAlexaHumaxInterfaceAssistance.HEADER.NAMESPACE.PLAYBACKCONTROLLER:
                        switch (msg.header.name) {
                            case CAlexaHumaxInterfaceAssistance.HEADER.NAME.FAST_FORWARD:
                            case CAlexaHumaxInterfaceAssistance.HEADER.NAME.NEXT:
                            case CAlexaHumaxInterfaceAssistance.HEADER.NAME.PAUSE:
                            case CAlexaHumaxInterfaceAssistance.HEADER.NAME.PLAY:
                            case CAlexaHumaxInterfaceAssistance.HEADER.NAME.PREVIOUS:
                            case CAlexaHumaxInterfaceAssistance.HEADER.NAME.REWIND:
                            case CAlexaHumaxInterfaceAssistance.HEADER.NAME.STARTOVER:
                            case CAlexaHumaxInterfaceAssistance.HEADER.NAME.STOP:
                                return new CAlexaHumaxPlaybackAssistance(msg);
                            default:
                                break;
                        }
                        break;
                    case CAlexaHumaxInterfaceAssistance.HEADER.NAMESPACE.SPEAKER:
                        switch (msg.header.name) {
                            case CAlexaHumaxInterfaceAssistance.HEADER.NAME.SET_MUTE:
                            case CAlexaHumaxInterfaceAssistance.HEADER.NAME.ADJUST_VOLUME:
                                return new CAlexaHumaxVolumeAssistance(msg);
                            default:
                                break;
                        }
                        break;
                    case CAlexaHumaxInterfaceAssistance.HEADER.NAMESPACE.REMOTEVIDEOPLAYER:
                        switch (msg.header.name) {
                            case CAlexaHumaxInterfaceAssistance.HEADER.NAME.SEARCH_AND_DISPLAY_RESULUTS:
                                //case CAlexaHumaxInterfaceAssistance.HEADER.NAME.SEARCH_AND_PLAY:
                                return new CAlexaHumaxRemoteVideoPlayerAssistance(msg);
                            default:
                                break;
                        }
                        break;
                    default:
                        break;
                }
                return null;
            }
        };
        return CAssistanceFactoryAlexaHumaxInterface;
    })();
    var cDLVoiceAssistance = (function (_super) {
        __extends(cDLVoiceAssistance, _super);
        function cDLVoiceAssistance(aServiceName) {
            _super.call(this, aServiceName);
            this.HAlexaHelperOipfObj = this.getPluginInstance('AlexaHelper');
            this.MQTT_RETRY_TIMEOUT = 10000;
            this.timeoutNumber = 0;
            var that = this;
            hx.logadd('dsrvVoiceAssistance');
            that.defineEvents(['SkipChannels', 'ChangeChannelByNumber', 'ChangeChannelByName', 'TurnOn', 'TurnOff', 'SetMute', 'SearchAndDisplayResults', 'Next', 'Previous', 'StartOver']);
            that.assistanceFactory = new CAssistanceFactoryAlexaHumaxInterface();
        }
        cDLVoiceAssistance.prototype.getClientId = function () {
            var that = this;
            if (that.userId) {
                return that.userId + '-' + (Math.floor(Math.random() * 10000));
            }
            return '';
        };
        cDLVoiceAssistance.prototype.createMQTTClient = function (options) {
            var that = this;
            that.userId = options.userId;
            if (that.userId) {
                delete options.userId;
            }
            if (!that.mqttClient) {
                that.mqttClient = new __MQTT__(that.options, null);
                that._registerMQTTEvent(that.userId);
            }
        };
        cDLVoiceAssistance.prototype.removeMQTTClient = function () {
            var that = this;
            if (!that.mqttClient) {
                return;
            }
            that.disconnectMQTTClient();
            delete that.mqttClient;
            that.mqttClient = undefined;
        };
        cDLVoiceAssistance.prototype.connectMQTTClient = function (options) {
            var that = this;
            if (!that.mqttClient) {
                return;
            }
            if (!options.clientId || options.clientId.length == 0) {
                options.clientId = that.options.clientId = that.getClientId();
            }
            if (!that.mqttClient.connected) {
                that.mqttClient.connect(options);
            }
            hx.log('dsrvVoiceAssistance', '[dsrvVoiceAssistance] _registerMQTTEvent  ... connectMQTTClient clientId : ' + options.clientId);
        };
        cDLVoiceAssistance.prototype.disconnectMQTTClient = function () {
            var that = this;
            if (!that.mqttClient) {
                return;
            }
            if (that.mqttClient.connected) {
                that.mqttClient.disconnect();
            }
        };
        cDLVoiceAssistance.prototype.getMQTTOptions = function (type) {
            var that = this;
            var options = {
                clientId: '',
                endpoint: "azse0iws1yu7e.iot.eu-west-1.amazonaws.com".toLowerCase(),
                accessKey: "AKIAJMTTTFOLR7B7RYZA",
                secretKey: "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",
                regionName: 'eu-west-1'
            };
            if (!that.options) {
                that.options = options;
            }
            return that.options;
        };
        cDLVoiceAssistance.prototype.publish = function (responseValue) {
            var that = this;
            if (!that.assistance) {
                return;
            }
            if (that.mqttClient && that.mqttClient.connected) {
                var msg = that.assistance.makePublishMsg(responseValue);
                that.mqttClient.publish(that.publishTopic, msg);
            }
        };
        cDLVoiceAssistance.prototype.processData = function (param) {
            var that = this;
            var data = null;
            if (that.assistance) {
                data = that.assistance.getProcessData(param);
                if (data) {
                    that.HAlexaHelperOipfObj.processData(JSON.stringify(data));
                }
            }
        };
        cDLVoiceAssistance.prototype.isSubscribed = function () {
            if (!this.mqttClient) {
                return false;
            }
            return this.mqttClient.connected;
        };
        cDLVoiceAssistance.prototype.getStatus = function () {
            var msg = '';
            //MQTT Client Information
            if (this.mqttClient) {
                msg += "</br>Mqtt Client info : " + this.mqttClient.getComputedEndPointInfo();
                //lastMqttStatus
                msg += "</br>Mqtt Client Status : " + this.lastMqttStatus;
            }
            else {
                msg += "</br>Mqtt Client Status : " + this.lastMqttStatus;
            }
            return msg;
        };
        cDLVoiceAssistance.prototype._registerMQTTEvent = function (userId) {
            var that = this;
            that.userId = userId;
            var updateMqttStatusMsg = function (msg) {
                that.lastMqttStatus = msg;
            };
            if (!that.mqttClient) {
                return;
            }
            that.mqttClient.on('connectionLost', function (msg) {
                updateMqttStatusMsg('connectionLost' + ' errorCode : ' + msg.errorCode + ' , message :' + msg.errorMessage);
                hx.log('dsrvVoiceAssistance', '[dsrvVoiceAssistance] connectionLost ... errorCode : ' + msg.errorCode + ', message : ' + msg.errorMessage);
                switch (msg.errorCode) {
                    case 0:
                        hx.log('dsrvVoiceAssistance', '[dsrvVoiceAssistance] connectionLost ... OK ERROR ################');
                        break;
                    default:
                        if (!that.timeoutNumber || that.timeoutNumber === 0) {
                            that.timeoutNumber = 1;
                            that.mqttClient = undefined;
                            updateMqttStatusMsg('[dsrvVoiceAssistance] mqttClient waiting re-connecting ... ');
                            setTimeout(function () {
                                that.options.clientId = that.getClientId();
                                if (!that.mqttClient) {
                                    that.mqttClient = new __MQTT__(that.options, null);
                                    that._registerMQTTEvent(userId);
                                }
                                that.connectMQTTClient(that.options);
                                that.timeoutNumber = 0;
                                updateMqttStatusMsg('[dsrvVoiceAssistance] _registerMQTTEvent  ... retry clientId : ' + that.options.clientId);
                                hx.log('dsrvVoiceAssistance', '[dsrvVoiceAssistance] _registerMQTTEvent  ... retry clientId : ' + that.options.clientId);
                            }, that.MQTT_RETRY_TIMEOUT);
                        }
                        break;
                }
            });
            that.mqttClient.on('messageArrived', function (msg) {
                updateMqttStatusMsg('messageArrived');
                hx.log('dsrvVoiceAssistance', '[dsrvVoiceAssistance] messageArrived in ... ');
                hx.log('dsrvVoiceAssistance', 'msg.payloadString: ' + msg.payloadString);
                var payload = that._getPayload(msg);
                that.assistance = that.assistanceFactory.makeAssistance(payload);
                if (that.assistance) {
                    updateMqttStatusMsg(that.lastMqttStatus += ' - activated');
                    that.assistance.operate(that);
                    updateMqttStatusMsg(that.lastMqttStatus += that.assistance.getInformation());
                    that._destroyAssistance();
                }
                else {
                    updateMqttStatusMsg(that.lastMqttStatus += ' - discard' + '</br>' + 'namespace : ' + payload.header.namespace + ' , name : ' + payload.header.name + ' ,</br> payload : ' + JSON.stringify(payload.payload));
                }
            });
            that.mqttClient.on('connected', function () {
                hx.log('dsrvVoiceAssistance', '[dsrvVoiceAssistance] connected ... ');
                updateMqttStatusMsg('connected');
                if (userId) {
                    that.topic = userId + cDLVoiceAssistance.TOPIC_POSTFIX;
                    that.publishTopic = userId + cDLVoiceAssistance.TOPIC_PUBLISH_POSTFIX;
                    hx.log('dsrvVoiceAssistance', '[dsrvVoiceAssistance] set new Topic : ' + that.topic);
                    that.mqttClient.subscribe(that.topic);
                }
            });
            that.mqttClient.on('subscribeFailed', function (e) {
                updateMqttStatusMsg('subscribeFailed');
                hx.log('dsrvVoiceAssistance', '[dsrvVoiceAssistance] subscribeFailed ... ' + e);
            });
            that.mqttClient.on('subscribeSucess', function () {
                updateMqttStatusMsg('subscribeSucess');
                hx.log('dsrvVoiceAssistance', '[dsrvVoiceAssistance] subscribeSuccess ... ');
            });
            that.mqttClient.on('publishFailed', function (e) {
                updateMqttStatusMsg('publishFailed');
                hx.log('dsrvVoiceAssistance', '[dsrvVoiceAssistance] publishFailed ... ' + e);
            });
        };
        cDLVoiceAssistance.prototype._getPayload = function (msg) {
            var payload = null;
            if (msg && msg.payloadString) {
                try {
                    payload = JSON.parse(msg.payloadString);
                }
                catch (e) {
                    hx.log('error', 'exception!! _getPayload');
                    return null;
                }
            }
            return payload;
        };
        cDLVoiceAssistance.prototype._destroyAssistance = function () {
            var that = this;
            if (that.assistance) {
                delete that.assistance;
                that.assistance = undefined;
            }
        };
        cDLVoiceAssistance.TOPIC_POSTFIX = '/stbAgent'; //
        cDLVoiceAssistance.TOPIC_PUBLISH_POSTFIX = '/mediaCloud';
        return cDLVoiceAssistance;
    })(__KERNEL__);
    return cDLVoiceAssistance;
});
//# sourceMappingURL=dl_dsrvVoiceAssistance.js.map
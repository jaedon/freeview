/// <reference path="../../def/q.d.ts" />
/// <reference path="../../def/jquery.d.ts" />
/// <reference path="../../def/sugar.d.ts" />
/// <reference path="../../def/prism.d.ts" />
// file name : fw/service/HMX/dservice/dl_dsrvVoiceAssistance.js
/*jslint nomen:true, vars:true */
//    MQTT: ['layer_domain/service/dl_dsrvVoiceAssistance.js'],

declare var hx: ifSugar;
import __KERNEL__ = require("./dl_kernel");
import __MQTT__ = require("../../lib/local/mqtt");
import __MOMENT__ = require("../../lib/ext/moment.min");

enum EAlexaRequestType {
    NONE = 0,
    CHANGECHANNEL = 1,  // operated by web
    SKIPCHANNELS = 2,   // operated by web
    TURNOFF = 3,
    TURNON = 4,
    FAST_FORWARD = 5,
    NEXT = 6,   // operated by web
    PAUSE = 7,
    PLAY = 8,
    PREVIOUS = 9,  // operated by web
    REWIND = 10,
    STARTOVER = 11, // operated by web
    STOP = 12,
    SET_VOLUME = 13,    // Not yet
    ADJUST_VOLUME = 14,
    SET_MUTE = 15,
    SEARCH_AND_DISPLAY_RESULTS = 16,
    SEARCH_AND_PLAY = 17    // Not yet
}

interface IfAssistance {
    operate(va: ifSVCVoiceAssistance): void;
    isValidRequest(): boolean;
    makePublishMsg(responseValue?) : string;
    getProcessData(param?): any;
    _getAmzDate(): string;
    _makeMCFResponseContexts(responseValue?): Array<any>;
    getInformation() : string;
    _getRequestType(): EAlexaRequestType;
}

class CAlexaHumaxInterfaceAssistance implements IfAssistance {
    header;
    payload;
    validRequest;
    request;//header.name
    static HEADER = {
        NAMESPACE : {
            CHANNELCONTROLLER : "Alexa.ChannelController",
            POWERCONTROLLER : "Alexa.PowerController",
            PLAYBACKCONTROLLER : "Alexa.PlaybackController",
            SPEAKER : "Alexa.Speaker",
            REMOTEVIDEOPLAYER : "Alexa.RemoteVideoPlayer"
        },
        NAME : {
            CHANGECHANNEL : "ChangeChannel",
            SKIPCHANNELS : "SkipChannels",
            TURNOFF : "TurnOff",
            TURNON : "TurnOn",
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

    constructor(msg) {
        if (msg) {
            this.header = msg.header;
            this.payload = msg.payload;
            if(msg.header && msg.header.name){
                this.request = msg.header.name;
            }
        }
    }

    public operate(va: ifSVCVoiceAssistance) {
        throw new Error('This function is abstract');
    }

    public isValidRequest(): boolean {
        var that = this;

        if (!that.header || !that.payload) {
            return false;
        }

        if (that.validRequest.indexOf(that.request) !== -1) {
            return true;
        }
        return false;
    }

    public makePublishMsg(responseValue?): string {
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
    }

    public getProcessData(param?): any {
        var that = this;
        var type:EAlexaRequestType = that._getRequestType();

        if (type === EAlexaRequestType.NONE) {
            return null;
        }

        if (param) {
            return {'name': type, 'data': param};
        } else {
            return {'name': type};
        }
    }

    _getAmzDate(): string {
        var time = null, dateStamp = null, amzdate = null;

        time = __MOMENT__.utc();
        dateStamp = time.format('YYYY-MM-DD');
        amzdate = dateStamp + 'T' + time.format('HH:mm:ss') + 'Z';

        return amzdate;
    }

    _makeMCFResponseContexts(responseValue?): Array<any> {
        return null;
    }

    getInformation() : string{
        var hederStr = 'namespace : '+this.header.namespace+' , name : '+this.header.name;
        return '</br>'+hederStr+'</br>'+JSON.stringify(this.payload);
    }

    _getRequestType(): EAlexaRequestType {
        var that = this;
        var type:EAlexaRequestType = EAlexaRequestType.NONE;

        if (that.isValidRequest()) {
            switch (that.request) {
                case CAlexaHumaxInterfaceAssistance.HEADER.NAME.TURNOFF:
                    type = EAlexaRequestType.TURNOFF;
                    break;
                case CAlexaHumaxInterfaceAssistance.HEADER.NAME.TURNON:
                    type = EAlexaRequestType.TURNON;
                    break;
                case CAlexaHumaxInterfaceAssistance.HEADER.NAME.FAST_FORWARD:
                    type = EAlexaRequestType.FAST_FORWARD;
                    break;
                case CAlexaHumaxInterfaceAssistance.HEADER.NAME.PAUSE:
                    type = EAlexaRequestType.PAUSE;
                    break;
                case CAlexaHumaxInterfaceAssistance.HEADER.NAME.PLAY:
                    type = EAlexaRequestType.PLAY;
                    break;
                case CAlexaHumaxInterfaceAssistance.HEADER.NAME.REWIND:
                    type = EAlexaRequestType.REWIND;
                    break;
                case CAlexaHumaxInterfaceAssistance.HEADER.NAME.STOP:
                    type = EAlexaRequestType.STOP;
                    break;
                case CAlexaHumaxInterfaceAssistance.HEADER.NAME.ADJUST_VOLUME:
                    type = EAlexaRequestType.ADJUST_VOLUME;
                    break;
                case CAlexaHumaxInterfaceAssistance.HEADER.NAME.SET_MUTE:
                    type = EAlexaRequestType.SET_MUTE;
                    break;
                default:
                    break;
            }
        }
        return type;
    }
}

class CAlexaHumaxChannelAssistance extends CAlexaHumaxInterfaceAssistance {
    static CHANNEL = {
            NUMBER : "number",
            CALLSIGN : "callSign",
            AFFILIATECALLSIGN : "affiliateCallSign",
            URI : "uri",
    };

    static CHANNELMETADATA = {
        NAME : "name",
        IMAGE : "image"
    };

    static CHANNEL_ACTION = {
        SKIPCHANNELS : 'SkipChannels',
        CHANGE_CHANNEL_BY_NUMBER : 'ChangeChannelByNumber',
        CHANGE_CHANNEL_BY_NAME : 'ChangeChannelByName'
    };

    constructor(msg) {
        super(msg);
        this.validRequest = [CAlexaHumaxInterfaceAssistance.HEADER.NAME.CHANGECHANNEL, CAlexaHumaxInterfaceAssistance.HEADER.NAME.SKIPCHANNELS];
    }

    public operate(va: ifSVCVoiceAssistance) {
        var that = this;

        if (!va) {
            return;
        }

        if (!that.isValidRequest()) {
            return;
        }

        switch (that.request){
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
                } else {
                    var callsign = that._getValue(channel, CAlexaHumaxChannelAssistance.CHANNEL.CALLSIGN) ?
                        that._getValue(channel, CAlexaHumaxChannelAssistance.CHANNEL.CALLSIGN) :
                        that._getValue(channel, CAlexaHumaxChannelAssistance.CHANNEL.AFFILIATECALLSIGN);
                    var name = that._getValue(channelMetadata, CAlexaHumaxChannelAssistance.CHANNELMETADATA.NAME);

                    va.fireEvent(CAlexaHumaxChannelAssistance.CHANNEL_ACTION.CHANGE_CHANNEL_BY_NAME, {
                        'callsign': callsign,
                        'name': name
                    });
                }
                break;
            default :
                break;
        }
    }

    _getPayloadChannelCount() {
        var that = this;
        var channelCount = 0;

        try {
            channelCount = that.payload.channelCount;
        } catch(e) {
            hx.log('error', 'exception!!! _getPayloadChannelCount');
            return 0;
        }
        return channelCount;
    }

    _getPayloadChannel() {
        var that = this;
        var container = null;

        try {
            container = that.payload.channel;
        } catch(e) {
            hx.log('error', 'exception!!! _getPayloadChannel');
            return null;
        }
        return container;
    }

    _getPayloadChannelMetadata() {
        var that = this;
        var container = null;

        try {
            container = that.payload.channelMetadata;
        } catch(e) {
            hx.log('error', 'exception!!! _getPayloadChannelMetadata');
            return null;
        }
        return container;
    }

    _getValue(container , key) {
        if(container){
            return container[key];
        } else {
            return '';
        }
    }

    _makeMCFResponseContexts(responseValue): Array<any> {
        var that = this;
        var channel = that._getPayloadChannel();
        var context = {
            'namespace': that.header.namespace,
            'name': "channel",
            'value': {
                'number': responseValue && responseValue.number ? responseValue.number : that._getValue(channel, CAlexaHumaxChannelAssistance.CHANNEL.NUMBER),
                'callSign': responseValue && responseValue.callSign ? responseValue.callSign : that._getValue(channel,CAlexaHumaxChannelAssistance.CHANNEL.CALLSIGN),
                'affiliateCallSign': responseValue && responseValue.affiliateCallSign ? responseValue.affiliateCallSign : that._getValue(channel,CAlexaHumaxChannelAssistance.CHANNEL.AFFILIATECALLSIGN)
            },
            'timeOfSample': that._getAmzDate(),
            'uncertaintyInMilliseconds': 200
        };

        return [context];
    }
}

class CAlexaHumaxPowerAssistance extends CAlexaHumaxInterfaceAssistance {
    constructor(msg) {
        super(msg);
        this.validRequest = [CAlexaHumaxInterfaceAssistance.HEADER.NAME.TURNOFF, CAlexaHumaxInterfaceAssistance.HEADER.NAME.TURNON];
    }

    public operate(va: ifSVCVoiceAssistance) {
        var that = this;

        if (!va) {
            return;
        }

        if (!that.isValidRequest()) {
            return;
        }

        va.fireEvent(that.request);
    }

    _makeMCFResponseContexts(): Array<any> {
        var that = this;
        var context = null;
        var value = null;

        if (that.request === CAlexaHumaxInterfaceAssistance.HEADER.NAME.TURNON) {
            value = 'ON';
        } else if (that.request === CAlexaHumaxInterfaceAssistance.HEADER.NAME.TURNOFF) {
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
    }
}

class CAlexaHumaxPlaybackAssistance extends CAlexaHumaxInterfaceAssistance {
    constructor(msg) {
        super(msg);
        this.validRequest = [CAlexaHumaxInterfaceAssistance.HEADER.NAME.FAST_FORWARD, CAlexaHumaxInterfaceAssistance.HEADER.NAME.NEXT,
            CAlexaHumaxInterfaceAssistance.HEADER.NAME.PAUSE, CAlexaHumaxInterfaceAssistance.HEADER.NAME.PLAY, CAlexaHumaxInterfaceAssistance.HEADER.NAME.PREVIOUS,
            CAlexaHumaxInterfaceAssistance.HEADER.NAME.REWIND, CAlexaHumaxInterfaceAssistance.HEADER.NAME.STARTOVER, CAlexaHumaxInterfaceAssistance.HEADER.NAME.STOP];
    }

    public operate(va: ifSVCVoiceAssistance) {
        var that = this;

        if (!va) {
            return;
        }
        if (!that.isValidRequest()) {
            return;
        }

        switch(that.request) {
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
    }
}

class CAlexaHumaxVolumeAssistance extends CAlexaHumaxInterfaceAssistance {
    constructor(msg) {
        super(msg);
        this.validRequest = [CAlexaHumaxInterfaceAssistance.HEADER.NAME.SET_VOLUME,CAlexaHumaxInterfaceAssistance.HEADER.NAME.SET_MUTE,CAlexaHumaxInterfaceAssistance.HEADER.NAME.ADJUST_VOLUME];
    }

    public operate(va: ifSVCVoiceAssistance) {
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
    }

    _getVolume() {
        var that = this;
        var volume = 0;

        try {
            volume = that.payload.volume;
        } catch(e) {
            hx.log('error', 'exception!!! _getVolume');
        }
        return volume;
    }

    _getMute() : boolean{
        var that = this;
        var mute = false;

        try {
            mute = that.payload.mute ? true : false;
        } catch(e) {
            hx.log('error', 'exception!!! _getVolume');
            return false;
        }
        return mute;
    }

    _makeMCFResponseContexts(): Array<any> {
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
    }
}

class CAlexaHumaxRemoteVideoPlayerAssistance extends CAlexaHumaxInterfaceAssistance {
    constructor(msg) {
        super(msg);
        this.validRequest = [CAlexaHumaxInterfaceAssistance.HEADER.NAME.SEARCH_AND_DISPLAY_RESULUTS, CAlexaHumaxInterfaceAssistance.HEADER.NAME.SEARCH_AND_PLAY];
    }

    public operate(va: ifSVCVoiceAssistance) {
        var that = this;
        var entities = null;

        if (!va) {
            return;
        }

        if (!that.isValidRequest()) {
            return;
        }

        switch(that.request){
            case CAlexaHumaxInterfaceAssistance.HEADER.NAME.SEARCH_AND_DISPLAY_RESULUTS:
                entities = that._getEntitiesContainer();
                if (entities && entities.length > 0) {
                    va.fireEvent(that.request, entities);
                }
                break;
            case  CAlexaHumaxInterfaceAssistance.HEADER.NAME.SEARCH_AND_PLAY:
                break;
            default :
                break;
        }
    }

    _getEntitiesContainer() {
        var that = this;
        var entities = null; //array

        if (that.payload) {
            if (that.payload.entities.length > 0) {
                entities = that.payload.entities;
            }
        }
        return entities;
    }
}

interface ifAssistanceFactory {
    makeAssistance(msg): IfAssistance;
}

class CAssistanceFactoryAlexaHumaxInterface implements ifAssistanceFactory {
    constructor() {}

    public makeAssistance(msg): IfAssistance {
        if (msg && msg.header && msg.payload) {
            switch (msg.header.namespace){
                case CAlexaHumaxInterfaceAssistance.HEADER.NAMESPACE.CHANNELCONTROLLER:
                    switch (msg.header.name){
                        case CAlexaHumaxInterfaceAssistance.HEADER.NAME.CHANGECHANNEL:
                        case CAlexaHumaxInterfaceAssistance.HEADER.NAME.SKIPCHANNELS:
                            return new CAlexaHumaxChannelAssistance(msg);

                        default:
                            break;
                    }
                    break;
                case CAlexaHumaxInterfaceAssistance.HEADER.NAMESPACE.POWERCONTROLLER:
                    switch (msg.header.name){
                        case CAlexaHumaxInterfaceAssistance.HEADER.NAME.TURNON:
                        case CAlexaHumaxInterfaceAssistance.HEADER.NAME.TURNOFF:
                            return new CAlexaHumaxPowerAssistance(msg);
                        default:
                            break;
                    }
                    break;
                case CAlexaHumaxInterfaceAssistance.HEADER.NAMESPACE.PLAYBACKCONTROLLER:
                    switch (msg.header.name){
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
                        //case CAlexaHumaxInterfaceAssistance.HEADER.NAME.SET_VOLUME:
                        case CAlexaHumaxInterfaceAssistance.HEADER.NAME.ADJUST_VOLUME:
                            return new CAlexaHumaxVolumeAssistance(msg);
                        default :
                            break;
                    }
                    break;
                case CAlexaHumaxInterfaceAssistance.HEADER.NAMESPACE.REMOTEVIDEOPLAYER:
                    switch (msg.header.name) {
                        case CAlexaHumaxInterfaceAssistance.HEADER.NAME.SEARCH_AND_DISPLAY_RESULUTS:
                        //case CAlexaHumaxInterfaceAssistance.HEADER.NAME.SEARCH_AND_PLAY:
                            return new CAlexaHumaxRemoteVideoPlayerAssistance(msg);
                        default :
                            break;
                    }
                    break;
                default :
                    break;
            }
            return null;
        }
    }
}

class cDLVoiceAssistance extends __KERNEL__ implements ifSVCVoiceAssistance {
    private HAlexaHelperOipfObj = this.getPluginInstance('AlexaHelper');
    private mqttClient;
    private assistanceFactory;
    private assistance : IfAssistance;
    private MQTT_RETRY_TIMEOUT = 10000;
    private topic;
    private publishTopic;
	private lastMqttStatus : string;
    static TOPIC_POSTFIX = '/stbAgent'; //
    static TOPIC_PUBLISH_POSTFIX = '/mediaCloud';
    private timeoutNumber : number = 0;

    private options;
    private userId : string; //Mac address

    constructor(aServiceName: string) {
        super(aServiceName);
        var that = this;

        hx.logadd('dsrvVoiceAssistance');
        that.defineEvents(['SkipChannels', 'ChangeChannelByNumber', 'ChangeChannelByName', 'TurnOn', 'TurnOff', 'SetMute',
                'SearchAndDisplayResults', 'Next', 'Previous', 'StartOver']);
        that.assistanceFactory = new CAssistanceFactoryAlexaHumaxInterface();
    }

    private getClientId(){
        var that = this;

        if(that.userId){
            return that.userId + '-' + (Math.floor(Math.random()*10000));
        }

        return '';
    }

    public createMQTTClient(options) {
        var that = this;
        that.userId = options.userId;

        if(that.userId)
        {
            delete options.userId;
        }

        if (!that.mqttClient) {
            that.mqttClient = new __MQTT__(that.options, null);
            that._registerMQTTEvent(that.userId);
        }
    }

    public removeMQTTClient() {
        var that = this;

        if (!that.mqttClient) {
            return;
        }

        that.disconnectMQTTClient();

        delete that.mqttClient;
        that.mqttClient = undefined;
    }

    public connectMQTTClient(options) {
        var that = this;

        if (!that.mqttClient) {
            return ;
        }

        if(!options.clientId || options.clientId.length == 0){
            options.clientId = that.options.clientId = that.getClientId();
        }

        if (!that.mqttClient.connected) {
            that.mqttClient.connect(options);
        }

        hx.log('dsrvVoiceAssistance','[dsrvVoiceAssistance] _registerMQTTEvent  ... connectMQTTClient clientId : '+options.clientId);
    }

    public disconnectMQTTClient() {
        var that = this;

        if (!that.mqttClient) {
            return;
        }

        if (that.mqttClient.connected) {
            that.mqttClient.disconnect();
        }
    }

    public getMQTTOptions(type?) {
        var that = this;
        var options = {
            clientId : '',
            endpoint : "azse0iws1yu7e.iot.eu-west-1.amazonaws.com".toLowerCase(),
            accessKey : "AKIAJMTTTFOLR7B7RYZA",
            secretKey: "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",
            regionName: 'eu-west-1'
        };

        if(!that.options){
            that.options = options;
        }

        return that.options;
    }

    public publish(responseValue?) {
        var that = this;

        if (!that.assistance) {
            return;
        }

        if (that.mqttClient && that.mqttClient.connected) {
            var msg = that.assistance.makePublishMsg(responseValue);
            that.mqttClient.publish(that.publishTopic, msg);
        }
    }

    public processData(param?) {
        var that = this;
        var data = null;

        if (that.assistance) {
            data = that.assistance.getProcessData(param);
            if (data) {
                that.HAlexaHelperOipfObj.processData(JSON.stringify(data));
            }
        }
    }

    public isSubscribed() : boolean {
        if(!this.mqttClient)
        {
            return false;
        }
        return this.mqttClient.connected;
    }

    public getStatus() : string {
        var msg : string = '';

        //MQTT Client Information
        if(this.mqttClient)
        {
            msg += "</br>Mqtt Client info : "+ this.mqttClient.getComputedEndPointInfo();
            //lastMqttStatus
            msg += "</br>Mqtt Client Status : "+this.lastMqttStatus;
        }
        else{
            msg += "</br>Mqtt Client Status : "+this.lastMqttStatus;
        }

        return msg;
    }

    _registerMQTTEvent(userId) {
        var that = this;

        that.userId = userId;

        var updateMqttStatusMsg = (msg : string) => {
            that.lastMqttStatus = msg;
        }

        if(!that.mqttClient) {
            return;
        }
        that.mqttClient.on('connectionLost', (msg) => {
            updateMqttStatusMsg('connectionLost'+ ' errorCode : '+msg.errorCode+' , message :' + msg.errorMessage);
            hx.log('dsrvVoiceAssistance', '[dsrvVoiceAssistance] connectionLost ... errorCode : '+msg.errorCode + ', message : '+msg.errorMessage);

            /*
            *
            var ERROR = {
             OK: {code:0, text:"AMQJSC0000I OK."},
             CONNECT_TIMEOUT: {code:1, text:"AMQJSC0001E Connect timed out."},
             SUBSCRIBE_TIMEOUT: {code:2, text:"AMQJS0002E Subscribe timed out."},
             UNSUBSCRIBE_TIMEOUT: {code:3, text:"AMQJS0003E Unsubscribe timed out."},
             PING_TIMEOUT: {code:4, text:"AMQJS0004E Ping timed out."},
             INTERNAL_ERROR: {code:5, text:"AMQJS0005E Internal error. Error Message: {0}, Stack trace: {1}"},
             CONNACK_RETURNCODE: {code:6, text:"AMQJS0006E Bad Connack return code:{0} {1}."},
             SOCKET_ERROR: {code:7, text:"AMQJS0007E Socket error:{0}."},
             SOCKET_CLOSE: {code:8, text:"AMQJS0008I Socket closed."},
             MALFORMED_UTF: {code:9, text:"AMQJS0009E Malformed UTF data:{0} {1} {2}."},
             UNSUPPORTED: {code:10, text:"AMQJS0010E {0} is not supported by this browser."},
             INVALID_STATE: {code:11, text:"AMQJS0011E Invalid state {0}."},
             INVALID_TYPE: {code:12, text:"AMQJS0012E Invalid type {0} for {1}."},
             INVALID_ARGUMENT: {code:13, text:"AMQJS0013E Invalid argument {0} for {1}."},
             UNSUPPORTED_OPERATION: {code:14, text:"AMQJS0014E Unsupported operation."},
             INVALID_STORED_DATA: {code:15, text:"AMQJS0015E Invalid data in local storage key={0} value={1}."},
             INVALID_MQTT_MESSAGE_TYPE: {code:16, text:"AMQJS0016E Invalid MQTT message type {0}."},
             MALFORMED_UNICODE: {code:17, text:"AMQJS0017E Malformed Unicode string:{0} {1}."},
             BUFFER_FULL: {code:18, text:"AMQJS0018E Message buffer is full, maximum buffer size: {0}."},
             };
            * */

            switch(msg.errorCode){
                case 0:
                    hx.log('dsrvVoiceAssistance', '[dsrvVoiceAssistance] connectionLost ... OK ERROR ################');
                    break;
                /*
                case 1://CONNECT_TIMEOUT
                    that.mqttClient.connect();
                    updateMqttStatusMsg(that.lastMqttStatus += '</br> - retry connect');
                    break;
                */
                default:
                    if(!that.timeoutNumber || that.timeoutNumber === 0)
                    {
                        that.timeoutNumber = 1;
                        that.mqttClient = undefined;
                        updateMqttStatusMsg('[dsrvVoiceAssistance] mqttClient waiting re-connecting ... ');

                        setTimeout(() => {
                            that.options.clientId  = that.getClientId();

                            if (!that.mqttClient) {
                                that.mqttClient = new __MQTT__(that.options, null);
                                that._registerMQTTEvent(userId);
                            }

                            that.connectMQTTClient(that.options);
                            that.timeoutNumber = 0;
                            updateMqttStatusMsg('[dsrvVoiceAssistance] _registerMQTTEvent  ... retry clientId : '+that.options.clientId);
                            hx.log('dsrvVoiceAssistance','[dsrvVoiceAssistance] _registerMQTTEvent  ... retry clientId : '+that.options.clientId);
                        }, that.MQTT_RETRY_TIMEOUT);
                    }
                    break;
            }
        });

        that.mqttClient.on('messageArrived', (msg) => {
            updateMqttStatusMsg('messageArrived');

            hx.log('dsrvVoiceAssistance', '[dsrvVoiceAssistance] messageArrived in ... ');
            hx.log('dsrvVoiceAssistance', 'msg.payloadString: '+msg.payloadString);

            var payload = that._getPayload(msg);
            that.assistance = that.assistanceFactory.makeAssistance(payload);

            if (that.assistance) {
                updateMqttStatusMsg(that.lastMqttStatus+=' - activated');
                that.assistance.operate(that);

                updateMqttStatusMsg(that.lastMqttStatus+=that.assistance.getInformation());
                that._destroyAssistance();
            }
            else{
                updateMqttStatusMsg(that.lastMqttStatus+=' - discard'+'</br>'+'namespace : '+payload.header.namespace+' , name : '+payload.header.name+' ,</br> payload : '+ JSON.stringify(payload.payload));
            }
        });
        that.mqttClient.on('connected', () => {
            hx.log('dsrvVoiceAssistance', '[dsrvVoiceAssistance] connected ... ');
            updateMqttStatusMsg('connected');
            if(userId){
                that.topic = userId + cDLVoiceAssistance.TOPIC_POSTFIX;
                that.publishTopic = userId + cDLVoiceAssistance.TOPIC_PUBLISH_POSTFIX;
                hx.log('dsrvVoiceAssistance', '[dsrvVoiceAssistance] set new Topic : ' + that.topic);

                that.mqttClient.subscribe(that.topic);
            }
        });
        that.mqttClient.on('subscribeFailed', (e) => {
            updateMqttStatusMsg('subscribeFailed');
            hx.log('dsrvVoiceAssistance', '[dsrvVoiceAssistance] subscribeFailed ... ' + e);
        });
        that.mqttClient.on('subscribeSucess', () => {
            updateMqttStatusMsg('subscribeSucess');
            hx.log('dsrvVoiceAssistance', '[dsrvVoiceAssistance] subscribeSuccess ... ');
        });
        that.mqttClient.on('publishFailed', (e) => {
            updateMqttStatusMsg('publishFailed');
            hx.log('dsrvVoiceAssistance', '[dsrvVoiceAssistance] publishFailed ... ' + e);
        });

    }

    _getPayload(msg) {
        var payload = null;

        if (msg && msg.payloadString) {
            try {
                payload = JSON.parse(msg.payloadString);
            } catch (e) {
                hx.log('error', 'exception!! _getPayload');
                return null;
            }
        }
        return payload;
    }

    _destroyAssistance() {
        var that = this;
        if (that.assistance) {
            delete that.assistance;
            that.assistance = undefined;
        }
    }
}

export = cDLVoiceAssistance;
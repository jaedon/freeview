// file name : fw/service/OIPF/emul/emulUtil.js
/* jslint sloppy:true, nomen:true, vars:true */
/**
* @author ygkim
*/
var gOipfEmul = {
    videoBroadcastObject: null,
    createAntennaList: function (data) {
        console.log("[EMULATOR] " + "start of createAntennalist");
    },
    processServiceList: function (data) {
        console.log("[EMULATOR] " + "processServiceList");
    },
    createChannelList: function (data) {
        // console.log("[EMULATOR] " + 'createChannelList');
        var that = this;

        // that.createAntennaInfoIfNeeded(satelliteName);
        var channelList = new ChannelList();
        $.each(data.service, function (index, s) {
            var channel = new Channel();
            channel.channelType = that.svctypeCnv[s.svctype];
            channel.idType = that.delitypeCnv[s.delitype];

            //channel.ccid = 'ccid:[' + service.svc_uid + ':' + (i + 1) + '].[' + service.onid + '].[' + service.tsid + '].[' +
            // service.sid + ']';
            channel.ccid = 'ccid:' + s.svc_uid + '.' + s.onid + '.' + s.tsid + '.' + s.svcid;
            channel.onid = s.onid;
            channel.tsid = s.tsid;
            channel.sid = s.svcid;
            channel.name = s.name;
            channel.majorChannel = s.svc_uid;
            channel.minorChannel = 0;
            channel.locked = s.locked;
            channel.manualBlock = false;

            // channel.logoURL = 'emul/images/channel/icon_channelbar' + a.channelImageList[Math.floor(Math.random() * 21)] +
            // '_on.png';
            channelList.push(channel);
            // a.antenna[satelliteName].mapChannel[i.svc_uid] = channel;
        });

        // a.antenna[satelliteName].channelList = channelList;
        that.channelList = channelList;
    },
    processEventList: function (data) {
        console.log("[EMULATOR] " + 'createEventList');

        var that = this;
    },
    processScheduleList: function (json) {
        console.log("[EMULATOR] " + "processScheduleList");
    },
    processRecList: function (json) {
        console.log("[EMULATOR] " + "processRecList");
    },
    svctypeCnv: {
        "tv": Channel.TChannelType.ETYPE_TV,
        "radio": Channel.TChannelType.ETYPE_RADIO,
        "other": Channel.TChannelType.ETYPE_OTHER
    },
    delitypeCnv: {
        "sat": Channel.TIdType.EID_DVB_S
    }
};
//# sourceMappingURL=emulUtil.js.map

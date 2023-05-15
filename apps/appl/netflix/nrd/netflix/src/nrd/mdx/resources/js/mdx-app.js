/*
 * (c) 1997-2015 Netflix, Inc.  All content herein is protected by
 * U.S. copyright and other applicable intellectual property laws and
 * may not be copied without the express permission of Netflix, Inc.,
 * which reserves all rights.  Reuse of any of this content for any
 * purpose without the permission of Netflix, Inc. is strictly
 * prohibited.
 */

if (typeof mdx === "undefined") mdx = {};

$(document).ready(function() {
    var cparams = {
        startSession : {
            labels : ["handle"],
            type : ["text"],
            value : [""]
        },
        dePair : {
            labels : ["handle"],
            type : ["text"],
            value : [""]
        },
        endSession: {
            labels : [ "sid" ],
            type : ["text" ],
            value : [ 0x0 ]
        },
        sendMessage: {
            labels: [ "messageName", "messageData" ],
            type : [ "text", "text" ],
            value : ["", ""]
        }
    };

    var tparams = {
        getSessions : {
            labels : [""],
            type : ["text"],
            value : [""]
        },
        endSession: {
            labels : [ "sid" ],
            type : ["text" ],
            value : [ 0x0 ]
        },
        sendMessage: {
            labels: [ "messageName", "messageData" ],
            type : [ "text", "text" ],
            value : ["", ""]
        }
    };
    controllerMsgs = ["PLAYER_PLAY","PLAYER_PAUSE","PLAYER_RESUME",
                      "PLAYER_STOP", "PLAYER_SKIP", "PLAYER_SET_CURRENT_TIME",
                      "PLAYER_SET_AUTO_ADVANCE", "PLAYER_GET_CURRENT_STATE",
                      "PLAYER_GET_CAPABILITIES", "HANDSHAKE_ACCEPTED" ];
    targetMsgs = ["PLAYER_CURRENT_STATE", "PLAYER_STATE_CHANGED", "PLAYER_CAPABILITIES", "INVALID_SESSIONID", "HANDSHAKE" ];
    $(".pair").button({text: true, label: "Pair"});
    $(".clear").button({text: true, label: "Clear"});
    var initBtn = $('#init');
    var discoverBtn = $('#discover');
    var listBtn = $('#list');
    var stopBtn = $('#stop');
    var cExecBtn = $('#c-execute');
    var tExecBtn = $('#t-execute');
    var revealBtn = $('#reveal');
    var hideBtn = $('#hide');
    var generatorBtn = $("#t-generator");
    var eventBtn = $("#t-event");
    var activeHandle = $('#active-handle');
    var pairedUuid = $('#paired-uuid');
    var sid = $("#sid");
    var sidLabel = $("#sid-label");
    var handleLabel = $('#handle-label');
    var uuidLabel = $('#uuid-label');
    var eqTemplate = $('.eq-template');
    var eqAnchor = $('#eq-anchor');
    var myUUID = "";
    var currentPairingDevice = {};

    initBtn.button({text: true, label: "Initialize"});
    discoverBtn.button({text: true, label: "Discover"});
    listBtn.button({text: true, label: "List"});
    stopBtn.button({text: true, label: "Stop"});
    cExecBtn.button({text: true, label: "Execute", disabled: true});
    tExecBtn.button({text: true, label: "Execute", disabled: true});
    revealBtn.button({text: true, label: "Reveal"});
    hideBtn.button({text: true, label: "Hide"});
    generatorBtn.button({text: true, label: "Generate"});
    eventBtn.button({text: true, label: "Event"});

    var controllerLog = $('#controller-log');
    var targetLog = $('#target-log');
    var cLogLimit = $('#c-log-limit');
    var tLogLimit = $('#t-log-limit');
    var deviceTemplate = $(".device-template");
    var deviceAnchor = $("#mdx-device-wrapper");
    var cargAnchor = $("#c-arg-anchor");
    var targAnchor = $("#t-arg-anchor");
    var cLogLine = $('#c-log-line');
    var tLogLine = $('#t-log-line');

    var _state = { pairing : { uuid : -1, handle : -1 }, sid : -1 };
    var TRANSITION = 400;

    mdx.app = {
        fixture: { selector: {}, targets: [] },

        clearControllerConsole: function() {
            $("#controller-log > .log-entry").remove();
            cLogLine.text(0);
        },

        clearTargetConsole: function() {
            $("#target-log > .log-entry").remove();
            tLogLine.text(0);
        },

        nrdStateListener: function(evt) {
            switch(evt.type) {
            case "init":
                if (evt.status == "READY") {
               //     document.getElementById("init").removeAttribute("disabled");
                    mdx.app.log("mdx app received nrdp ready event");
                    var role = "CONTROLLER";
                    var commandMap = {};
                    mdx.app.addMdxEvtListeners();
                    mdx.init(role, commandMap);
                }
                break;
            default: break;
            }
        },
        stateDispatcher: function(evt) {

            switch(evt.type) {
            case "init":
                if (evt.status == "READY") {
               //     document.getElementById("init").removeAttribute("disabled");
                    mdx.app.log("mdx app received ready event");
                }
                break;
            case "initerror":
                mdx.app.log("Init error received??");
                break;
            default: break;
            }
      //      logFromNative.handleEvent(evt.data, "fromnative");
        }
    };

    var clickDispatcher = function(event) {
        var id = $(event.currentTarget).attr('id');
        switch(id) {
        case "init":
            mdx.init("BOTH", commandMap );
            break;

        case "list":
            mdx.app.fixture.targets = mdx.discovery.getTargets(mdx.app.fixture.selector);
            mdx.app.updateDevices();
            break;
        case "stop":
            mdx.removeEventListener("devicefound", discoverEvent);
            mdx.removeEventListener("devicebyebye", discoverEvent);
            break;
        case "reveal":
            mdx.discovery.revealPresence();
            break;
        case "hide":
            mdx.discovery.hidePresence();
            break;
        case "c-console":
            mdx.app.clearControllerConsole();
            break;
        case "t-console":
            mdx.app.clearTargetConsole();
            break;
        case "c-execute":
            var cmd = $('.c-command:checked').val();
            var input = $('input[name|="carg"]');
            var args = [];
            for (var i=0; i < input.length; i++) {
                var clz = "#carg-" + (i+1);
                if ($(clz).hasClass('carg-callback') && $(clz + ':checked').length) {
                 //   mdximpl.cfg.eventCallback = "event";
                    continue;
                }
                var arg = $(clz).val();
                if (arg) {
                    var n = new Number(arg);
                    if (n.toString() != Number.NaN.toString()) arg = n.valueOf();
                }
                args.push(arg);
            }

            try {
                if (cmd == "sendMessage") {
                    var payload = JSON.parse(args[1] || "{}");
                    var xid = mdx.session.sendMessage(_state.sid, args[0], payload);
                    mdx.app.log("Send Message to sid =" + _state.sid + " " + args[0] + " " + args[1] + " xid = " + xid);
                } else if (cmd == "dePair"){
                    mdx.app.log("Command: " + cmd + "(" + args.join(",") + ")");
                    mdx.pair[cmd].apply(mdx.app, args);
                    var uuid = currentPairingDevice.attr('uuid');
                    _state.pairing.handle = -1;
                    setDepaired(currentPairingDevice);
                    mdx.app.log("pairingDeleted ");
                    _state.sid = 0;
                    sidLabel.addClass('active');
                    sid.text(_state.sid);
                    var eq = eqTemplate.clone();
                    eq.removeClass("eq-template");
                    eq.attr('sid', _state.sid);
                    eq.find('.eq-sid').text(_state.sid);
                    eqAnchor.append(eq);
                    eq.fadeIn(TRANSITION);
                }else {
                    mdx.app.log("Command: " + cmd + "(" + args.join(",") + ")" );
                    mdx.session[cmd].apply(mdx.app, args);
                }
            } catch(e) {
                mdx.app.log("Command: " + cmd + "(" + args.join(",") + "): Exception: " + e.message, "error");
            }
            break;
        case "t-execute":
            var cmd = $('.t-command:checked').val();
            var input = $('input[name|="targ"]');
            var args = [];
            for (var i=0; i < input.length; i++) {
                var id = "#targ-" + (i+1);
                var arg = $(id).val();
                if (arg) {
                    var n = new Number(arg);
                    if (n.toString() != Number.NaN.toString()) arg = n.valueOf();
                }
                args.push(arg);
            }
            try {
                if (cmd == "sendMessage") {

                    var xid = mdx.session.sendMessage(_state.sid, args[0],args[1]);
                    mdx.app.tlog("sendMessage sid =" + _state.sid + " xid = " + xid);
                } else {
                    var xid = mdx.session[cmd].apply(mdx.app, args);
                    mdx.app.tlog("sendMessage sid =" + _state.sid + " xid = " + xid);
                }
            } catch(e) {
                mdx.app.tlog("Command: " + cmd + "(" + args.join(",") + "): Exception: " + e.message, "error");
            }
            break;
        case "t-generator":
            if (generatorBtn.button("option", "label") == "Generate") {
                _generator.generate();
                generatorBtn.button("option", "label", "Stop");
            } else {
                _generator.stop();
                generatorBtn.button("option", "label", "Generate");
            }
            break;
        case "t-event":
            _generator.send();
            break;
        case "c-log-dispatcher":
            if ($('#c-log-dispatcher:checked').length) {
           //     mdximpl.addEventListener(mdximpl.SCOPE, mdximpl._prv_events.dispatcher, logFromNative);
            } else {
            //    mdximpl.removeEventListener(mdximpl.SCOPE, mdximpl._prv_events.dispatcher, logFromNative);
            }
            break;
        case "c-log-type-fromnative":
        case "c-log-type-tonative":
        case "c-log-type-discovery":
        case "c-log-type-property":
        case "c-log-type-event":
        case "c-log-type-state":
        case "c-log-type-error":
        case "c-log-type-command":
        case "t-log-type-fromnative":
        case "t-log-type-tonative":
        case "t-log-type-property":
        case "t-log-type-event":
        case "t-log-type-state":
        case "t-log-type-error":
        case "t-log-type-command":
            var entry = ".entry-" + id.substr(id.lastIndexOf("-") + 1);
            var logger = (id.charAt(0) == "t") ? "#target-log" : "#controller-log";
            if ($('#' + id + ':checked').length) {
                $(logger + ' > ' + entry).show();
            } else {
                $(logger + ' > ' + entry).hide();
            }
            break;
        default:
        }
    }

    var argClickDispatcher = function(event) {
        var evt = $(event.currentTarget);
        var value = evt.attr("value");
        setArgs((evt.hasClass("c-command")) ? cparams : tparams, value);
    }

    var setPaired = function(device) {
        device.find('.handle').text(_state.pairing.handle);
        device.addClass('active');
        activeHandle.text(_state.pairing.handle);
        $('.carg-handle').val(_state.pairing.handle);
        pairedUuid.text(_state.pairing.uuid);
        handleLabel.removeClass('inactive');
        handleLabel.addClass('active');
        uuidLabel.removeClass('inactive');
        uuidLabel.addClass('active');
        $('.c-command').removeAttr('disabled');
        cExecBtn.button("enable");
        tExecBtn.button("enable");
    }

    var setDepaired = function(device) {
        if (!device) return;
        _state.pairing.uuid = -1;
        _state.pairing.handle = -1;
        $('.carg-handle').val(_state.pairing.handle);
        handleLabel.removeClass('active');
        handleLabel.addClass('inactive');
        uuidLabel.removeClass('active');
        uuidLabel.addClass('inactive');
        device.find('.handle').text("");
        device.removeClass('active');
        activeHandle.text("INVALID_HANDLE");
        pairedUuid.text(-1);
        $('.c-command').attr('disabled', 'disabled');
        cExecBtn.button("disable");
    }

    var pairClickHandler = function(event) {
        var device = $(event.currentTarget).parentsUntil('#mdx-device-wrapper', '[uuid]');
        currentPairingDevice = device;
        var uuid = device.attr('uuid');
        if (_state.pairing.handle != -1) {
            mdx.app.log("Already paired");
            return;
        }

    //    targets = mdx.discovery.getTargets(uuid);
        var handle =  mdx.pair.pairingRequest(uuid);
        if (_state.pairing.uuid != -1) {
            var dsel = '.device[uuid=' + _state.pairing.uuid + ']';
            setDepaired($(dsel));
        }
        _state.pairing.uuid = uuid;

        mdx.app.log("Command: pair(" + uuid + "): ");
      //  setPaired(device);
    }

        var targetSessionStarted = function(evt) {
        _state.sid = evt.sid;
        sidLabel.addClass('active');
        sid.text(_state.sid);
        var eq = eqTemplate.clone();
        eq.removeClass("eq-template");
        eq.attr('sid', _state.sid);
        eq.find('.eq-sid').text(_state.sid);
        eqAnchor.append(eq);
        eq.fadeIn(TRANSITION);
        mdx.app.tlog("targetSessionStarted sessionId = " + evt.sid);
    }

        var cstartSessionResponse = function(evt) {
        _state.sid = evt.sid;
        mdx.app.log("cstartSessionResponse sessionId = " + evt.sid);
        sidLabel.addClass('active');
        sid.text(_state.sid);
        var eq = eqTemplate.clone();
        eq.removeClass("eq-template");
        eq.attr('sid', _state.sid);
        eq.find('.eq-sid').text(_state.sid);
        eqAnchor.append(eq);
        eq.fadeIn(TRANSITION);
    }

    var cEndSession = function(evt) {
            if (_state.sid == evt.sid ) {
            mdx.app.log("end session Id = " + evt.sid);
            _state.sid = 0;
            sidLabel.addClass('active');
            sid.text(_state.sid);
            var eq = eqTemplate.clone();
            eq.removeClass("eq-template");
            eq.attr('sid', _state.sid);
            eq.find('.eq-sid').text(_state.sid);
            eqAnchor.append(eq);
            eq.fadeIn(TRANSITION);
        }
    }
    var controllerPairingResponse = function(evt) {
        var uuid = currentPairingDevice.attr('uuid');
        if (evt.errorcode) {
            mdx.app.log("controller Pairing Error - evt.errormesg ");
        } else {
            _state.pairing.handle = evt.pairingContext;
            setPaired(currentPairingDevice);
            mdx.app.log("controllerPairingResponse ");
        }
    }

    var pairingDeleted = function(evt) {
        _state.pairing.handle = -1;
        mdx.app.log("pairingDeleted ");
        _state.pairing.uuid = -1;
        _state.pairing.handle = -1;
        activeHandle.text("INVALID_HANDLE");
        pairedUuid.text(-1);
        cExecBtn.button("disable");
        _state.sid = 0;
        sidLabel.addClass('active');
        sid.text(_state.sid);
        var eq = eqTemplate.clone();
        eq.removeClass("eq-template");
        eq.attr('sid', _state.sid);
        eq.find('.eq-sid').text(_state.sid);
        eqAnchor.append(eq);
        eq.fadeIn(TRANSITION);
    }

    var targetPairingRequestReceived = function(evt) {
        mdx.app.tlog("targetPairingRequestReceived uuid " + evt.uuid);
        mdx.pair.allowPairing(evt.uuid);
    }

    var messageDelivered = function(evt) {
        mdx.app.log("messageDelivered " + " xid= " + evt.transactionId);

    }
    var targetPairingGrant = function(evt) {
        if (evt.controllerErrCode) {
            var errObj = {};
            errObj.errorcode = evt.controllerErrCode;
            errObj.errormesg = evt.controllerErrMesg;
            sendPairingReject(evt.controlleruuid, errObj);
            return;
        }
       _state.pairing.handle = evt.pairingContext;
       _state.pairing.uuid = evt.controlleruuid;
        mdx.app.tlog("targetPairingGrant ");
    //	targets = mdx.discovery.getTargets(uuid);
        mdx.pair.sendPairingGrant(evt.pairingContext);
        activeHandle.text(_state.pairing.handle);
        $('.carg-handle').val(_state.pairing.handle);
        pairedUuid.text(_state.pairing.uuid);
        handleLabel.removeClass('inactive');
        handleLabel.addClass('active');
        uuidLabel.removeClass('inactive');
        uuidLabel.addClass('active');
        $('.c-command').removeAttr('disabled');
       // cExecBtn.button("enable");
        tExecBtn.button("enable");
    }

    var appControllerMessageCb = function(evt) {
        if (evt.type == "PLAYER_PLAY") {
            mdx.app.tlog("starting to play movie " + evt.msgObject.catalogId + " track: " +  evt.msgObject.trackId);
        } else if (evt.type == "PLAYER_CURRENT_STATE"){
            mdx.app.tlog("Received current state" + evt.msgObject );
        }  else {
            mdx.app.tlog("Received " + evt.type + "data " +  + evt.msgObject );
        }
    }

    var appTargetMessageCb = function(evt) {
        if (evt.type == "INVALID_SESSIONID"){
            mdx.app.log("Received invalid session id" );
            _state.sid = 0;
            sidLabel.addClass('active');
            sid.text(_state.sid);
            var eq = eqTemplate.clone();
            eq.removeClass("eq-template");
            eq.attr('sid', _state.sid);
            eq.find('.eq-sid').text(_state.sid);
            eqAnchor.append(eq);
            eq.fadeIn(TRANSITION);
        } else {
            mdx.app.log("Received " + evt.type + "data " + evt.msgObject );
        }

    }

    var setArgs = function(params, cmd) {
        var prefix = "carg";
        var anchor = cargAnchor;
        if (params == cparams) {
            $('.carg').remove();
        } else {
            prefix = "targ";
            anchor = targAnchor;
            $('.targ').remove();
        }
        var args = params[cmd].labels;
        for (var i=0; i < args.length; i++) {
            var label = prefix + '-' + (i+1);
            var argLabel = $("<label>" + args[i] + "</label>");
            argLabel.attr("for", label);
            argLabel.addClass(prefix);
            var argInput = $('<input id="' + label + '" class="arg-text" name="' + label + '" type="' + params[cmd].type[i] + '" value="" />');
            if (params[cmd].value[i]) argInput.val(params[cmd].value[i]);
            argInput.addClass(prefix + "-" + args[i]);
            if (args[i] == "handle") argInput.val(_state.pairing.handle);
            argInput.addClass(prefix);
            anchor.append(argLabel);
            anchor.append(argInput);
            anchor.append($('<br class="' + prefix +'" />'));
        }
    }

    mdx.app.updateDevices = function() {
        mdx.app.fixture.targets = mdx.discovery.getTargets();
        deviceTemplate.siblings().each(function(i, device) {
            removeDevice(i, $(device));
        });
        mdx.app.fixture.targets.forEach(function(device) {
            addDevice(device);
        });
    };

    var discoverEvent = function(event) {
        if (event.type == "devicefound") {
            mdx.app.fixture.targets = mdx.discovery.getTargets();
            var myuuid = mdx.discovery.getUUID(event.USN);
            var device = {selectorKey:"netflix", loc:event.location, uuid:myuuid, usn:event.USN };
            addDevice(device);
            return;
        }
        if (event.type == "devicebyebye") {
            var myuuid = mdx.discovery.getUUID(event.USN);
            var jsel = '[uuid=' + myuuid + ']';
    //        mdx.app.log("Device ByeBYE uuid: " + myuuid);
            var device = $(jsel);
            if (device.length) {
              //  if (_state.paired.uuid == device.attr('uuid')) setDepaired(device);
                removeDevice(0, device);
            }
        }
    }

    var removeDevice = function(i, device) {
        device.fadeOut(TRANSITION, function() {
            $(this).remove();
        });
    }

    var addDevice = function(device) {
        var target = deviceTemplate.clone();
        target.removeClass('device-template');
        target.attr("uuid", device.uuid);
        var line = target.find("li:first");
        line.children("span:first").text(device.selectorKey);
        line = line.next();
        line.children("span:first").text(device.loc);
        line = line.next();
        line.children("span:first").text(device.uuid);
        line = line.next();
        line.children("span:first").text(device.usn);

        deviceAnchor.append(target);
        if (_state.pairing.uuid == device.uuid) setPaired(target);
        target.fadeIn(TRANSITION);
    };

    mdx.app.sessionHandler = function(event) {
        switch(event.action) {
        case "start":
            _state.sid = event.sid;
            sidLabel.addClass('active');
            sid.text(_state.sid);
            var eq = eqTemplate.clone();
            eq.removeClass("eq-template");
            eq.attr('sid', _state.sid);
            eq.find('.eq-sid').text(_state.sid);
            eqAnchor.append(eq);
            eq.fadeIn(TRANSITION);
            break;
        default:
        }
    }

    mdx.app.addMdxEvtListeners = function() {
        mdx.addEventListener("init", mdx.app.stateDispatcher);
        mdx.addEventListener("initerror", mdx.app.stateDispatcher);
        mdx.discovery.addEventListener("devicefound", discoverEvent);
        mdx.discovery.addEventListener("devicebyebye", discoverEvent);
        mdx.pair.addEventListener("pairingrequestreceived", targetPairingRequestReceived);
        mdx.pair.addEventListener("pairingresponse", controllerPairingResponse);
        mdx.pair.addEventListener("pairingdeleted", pairingDeleted);
        mdx.pair.addEventListener("pairinggrant", targetPairingGrant);
        mdx.session.addEventListener("sessionstart", targetSessionStarted);
        mdx.session.addEventListener("startSessionResponse", cstartSessionResponse);
        mdx.session.addEventListener("sessionended", cEndSession);
        mdx.session.addEventListener("messagedelivered", messageDelivered);
        for (var i = 0; i < controllerMsgs.length; i++){
             mdx.session.addMessageListener(controllerMsgs[i], appControllerMessageCb);
        }
        for (var i = 0; i < targetMsgs.length; i++){
             mdx.session.addMessageListener(targetMsgs[i], appTargetMessageCb);
        }
    }

    mdx.app.log = function(line, type) {
        var dt = new Date();
        var clz = type || "fromnative";
        var lines = $('#controller-log > .log-entry').length;
        var over = lines - cLogLimit.val() + 1;
        if (over > 0 ) $("#controller-log > .log-entry").slice(0, over).remove();
        var entry = $('<div class="log-entry">' + dt.toLocaleTimeString() + ":" + dt.getMilliseconds() + " " + line + '</div>');
        entry.addClass("entry-" + clz);
        if ($('#c-log-type-' + clz + ':checked').length == 0) {
            entry.hide();
        }
        controllerLog.append(entry);
        controllerLog.scrollTop(100000);
        lines = $('#controller-log > .log-entry').length;
        cLogLine.text(lines);
    }

    mdx.app.tlog = function(line, type) {
        var dt = new Date();
        var clz = type || "fromnative";
        var lines = $('#target-log > .log-entry').length;
        var over = lines - tLogLimit.val() + 1;
        if (over > 0) $("#target-log > .log-entry").slice(0, over).remove();
        var entry = $('<div class="log-entry">' + dt.toLocaleTimeString() + ":" + dt.getMilliseconds() + " " + line + '</div>');
        entry.addClass("entry-" + clz);
        targetLog.append(entry);
        targetLog.scrollTop(100000);
        lines = $('#target-log >.log-entry').length;
        tLogLine.text(lines);
    }

    function _LogHandler(logger, type) {
        this.logger = logger;
        this.type = type;

        this.handleEvent = function (event) {
            logger(JSON.stringify(event), type);
        }
    }


    $('.jbutton').bind('click', clickDispatcher);
    $('input.log-control[type="checkbox"]').bind('click', clickDispatcher);
    $('input[type="radio"]').bind('click', argClickDispatcher);
    $('.pair').live("click", pairClickHandler);

    var logFromNative = new _LogHandler(mdx.app.log, "fromnative");
    var logToNative = new _LogHandler(mdx.app.log, "tonative");
    var logEvent = new _LogHandler(mdx.app.log, "event");
    setArgs(cparams, "startSession");
    setArgs(cparams, "dePair");
    setArgs(tparams, "endSession");


    initialize = function() {
        nrdp.addEventListener("init", mdx.app.nrdStateListener);
        nrdp.init();

    }

    uninitialize = function() {
        console.log("mdx-app uninitialize");
        mdx.exit();
    }
});

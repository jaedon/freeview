/*
 * (c) 1997-2015 Netflix, Inc.  All content herein is protected by
 * U.S. copyright and other applicable intellectual property laws and
 * may not be copied without the express permission of Netflix, Inc.,
 * which reserves all rights.  Reuse of any of this content for any
 * purpose without the permission of Netflix, Inc. is strictly
 * prohibited.
 */

var counter = 1;
var cleanUrl = '';
var appName_1 = '';
var appName_2 = '';

var numberMessagesReceivedFromTarget = 0;
var bValidateMessagesRecv = false;


function addTarget() {

if(counter==1){
    $('#t2').show();
}
else if (counter==2){
    $('#t3').show();
}
else if (counter==3){
    $('#t4').show();
}
else if (counter==4){
    $('#t5').show();
}
else if (counter==5){
    $('#t6').show();
}

counter = counter+1;

}

function mdxConfigure(){
    var advertisingPeriod = parseInt(document.getElementById('advertisingPeriod').value);
    var advertisingTTL = parseInt(document.getElementById('advertisingTTL').value);
    var advertisingPort = parseInt(document.getElementById('advertisingPort').value);
    var listeningPort = parseInt(document.getElementById('listeningPort').value);
    mdx.configure(advertisingPeriod,advertisingTTL,advertisingPort, listeningPort);
    mdx.app.log("MDX Configure");
}

function setCommandMap() {
    map = document.getElementById('cmdMap').value;
    mdx.init('CONTROLLER', map);
}

    function makeHttpObject() {
        try {return new XMLHttpRequest();}
        catch (error) {
            mdx.app.log("new XMLHttpReqeust failed");
        }
    }

    function simpleHttpRequest(url, success, failure, method, data) {
        mdx.app.log("simpleHttpRequest " + url);
        method = method || "GET";
        var request = makeHttpObject();
        request.open(method, url, true);

        if (method === "POST")
            data = data || "DUMMY";
        request.send(data);

        request.onreadystatechange = function() {
            if (request.readyState == 4) {
                if ((request.status == 200)||(request.status == 201)){
                    mdx.app.log("simpleHttpRequest response Success: " + request.responseText);

            //PARSE for http address
            var resp = request.responseText;
            try{

            if(data=='initial'){

                var sendUrl = resp.split('http');
                    cleanUrl = sendUrl[1].split('</r');
                    cleanUrl = 'http' + cleanUrl[0];

                mdx.app.log("Getting Application List: " + cleanUrl);
                simpleHttpRequest(cleanUrl, mdx.app.log, mdx.app.log, "GET", 'application');
            }

            if(data=='application'){

                var appList = resp.split('\r\n');
                    appName_1 = appList[0];
                appName_2 = appList[1];

            }

           }
           catch(e) {
            cleanUrl = '';
            appList = '';
            appName_1 = '';
            appName_2 = '';
           }


            }
                else if (failure)
                    mdx.app.log("simpleHttpRequest response FAILED: " + request.responseText);
            }
        };
    }




var SEND_MESSAGE_C2T_TIME_INTERVAL_MSGS = 200;    // 200msec between each successive message in a loop
var SEND_MESSAGE_C2T_TIME_INTERVAL_LOOP = 5000;    // 5 seconds between each loop

function sendTargetMessages() {
        // read the messages to be sent from the data file
        var selectedTargetSlot = $('.target:checked').val();
        var targetSession = "#sid"+selectedTargetSlot;
        var targetSessionId = $(targetSession).text();
        var totalLoops = $("#c-send-messages-loops").val();
        var startSendStressTestObj = {"numbermessages":MDX_C2T_MESSAGES.mdxmessages.length * parseInt(totalLoops)};
        var endSendStressTestObj = {"numbermessages":MDX_C2T_MESSAGES.mdxmessages.length * parseInt(totalLoops)};
        var emptyObj = {};
        mdx.session.sendMessage(targetSessionId, "START_MESSAGE_SEND_STRESS_TEST", startSendStressTestObj);

        $("#c-sent-messages-number").val(0);
        setTimeout(function(){sendTargetOneMessage(0, 0)}, SEND_MESSAGE_C2T_TIME_INTERVAL_MSGS);

        //for(var numLoops=0; numLoops < totalLoops; numLoops++)
        //{
        //    for(var nindx=0; nindx < MDX_C2T_MESSAGES.mdxmessages.length; nindx++)
        //    {
        //        mdx.session.sendMessage(targetSessionId, MDX_C2T_MESSAGES.mdxmessages[nindx].name, MDX_C2T_MESSAGES.mdxmessages[nindx].object);
        //    }
            //mdx.session.sendMessage(targetSessionId, "PLAYER_GET_CURRENT_STATE", emptyObj);
        //}
        //mdx.session.sendMessage("END_MESSAGE_SEND_STRESS_TEST", "endtest", endSendStressTestObj);
}

function sendTargetOneMessage(currentLoopIndex, currentMessageIndex)
{
    var selectedTargetSlot = $('.target:checked').val();
    var targetSession = "#sid"+selectedTargetSlot;
    var targetSessionId = $(targetSession).text();
    var totalLoops = $("#c-send-messages-loops").val();
    var endSendStressTestObj = {"numbermessages":MDX_C2T_MESSAGES.mdxmessages.length};

    if((currentMessageIndex >= MDX_C2T_MESSAGES.mdxmessages.length) && (currentLoopIndex < totalLoops))
    {
        currentMessageIndex = 0;
        currentLoopIndex += 1;
    }


    //console.log("Sending message to target - currentLoopIndex:" + currentLoopIndex + "; currentMessageIndex:" + currentMessageIndex);

    mdx.session.sendMessage(targetSessionId, MDX_C2T_MESSAGES.mdxmessages[currentMessageIndex].name, MDX_C2T_MESSAGES.mdxmessages[currentMessageIndex].object);
    var currentMsgsSent = $("#c-sent-messages-number").val();
    $("#c-sent-messages-number").val(parseInt(currentMsgsSent)+1);

    if(currentLoopIndex + 1 == totalLoops && ((currentMessageIndex + 1) == MDX_C2T_MESSAGES.mdxmessages.length)) {
        mdx.session.sendMessage(targetSessionId, "END_MESSAGE_SEND_STRESS_TEST", endSendStressTestObj);
        return;
    }
    if(currentMessageIndex == MDX_C2T_MESSAGES.mdxmessages.length) {
        currentMessageIndex = 0;
        currentLoopIndex += 1;
        setTimeout(function(){sendTargetOneMessage(currentLoopIndex, currentMessageIndex)}, SEND_MESSAGE_C2T_TIME_INTERVAL_LOOP);
    }
    else {
        currentMessageIndex += 1;
        setTimeout(function(){sendTargetOneMessage(currentLoopIndex, currentMessageIndex)}, SEND_MESSAGE_C2T_TIME_INTERVAL_MSGS);
    }
}

function clearTargetMessages() {
        $("#c-sent-messages-number").val(0);
        $("#c-recv-messages-number").val(0);
        $("#c-send-messages-loops").val(1);
}


function updateTarget() {
    targetSlot = $('.target:checked').val();
    $("#c-log-type-current").removeAttr("checked");

    $('#controller-log > .entry-1').show();
    $('#controller-log > .entry-2').show();
    $('#controller-log > .entry-3').show();
    $('#controller-log > .entry-4').show();
    $('#controller-log > .entry-5').show();
    $('#controller-log > .entry-6').show();

    var hdl = "#active-handle"+targetSlot;
    var hdl_text = $(hdl).text();

    if(hdl_text == "INVALID_HANDLE"){
        //$('.c-command').attr('disabled', 'disabled');
        //$('#c-execute').button("disable");
    }
    else {
    //$('.c-command').removeAttr('disabled');
    //$('#c-execute').button("enable");
    }
}

function changeInterface(){
    mdx.app.log("Changing Interface");
    interfaceName = document.getElementById('interfaceName').value;
    interfaceConnected = document.getElementById('connected').value;
    mdx.interfaceChanged(interfaceName, interfaceConnected);
}

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
            value : [""]
        },
        sendMessage: {
            labels: [ "messageName", "messageData", "sid" ],
            type : [ "text", "text" ],
            value : ["", ""]
        },
        httpRequest: {
            labels: [ "URL", "data", "type" ],
            type : [ "text", "text", "text" ],
            value : ["", "", ""]
        },
    };

    controllerMsgs = ["PLAYER_PLAY","PLAYER_PAUSE","PLAYER_RESUME",
                      "PLAYER_STOP", "PLAYER_SKIP", "PLAYER_SET_CURRENT_TIME",
                      "PLAYER_SET_AUTO_ADVANCE", "PLAYER_GET_CURRENT_STATE",
                      "PLAYER_GET_CAPABILITIES", "HANDSHAKE_ACCEPTED"];
    targetMsgs = ["PLAYER_CURRENT_STATE", "PLAYER_STATE_CHANGED", "PLAYER_CAPABILITIES", "HANDSHAKE", "MESSAGE_IGNORED"];

    $(".pair").button({text: true, label: "Pair"});
    $(".clear").button({text: true, label: "Clear"});
    var initBtn = $('#init');
    var discoverBtn = $('#discover');
    var listBtn = $('#list');
    var stopBtn = $('#stop');
    var cExecBtn = $('#c-execute');
    var revealBtn = $('#reveal');
    var hideBtn = $('#hide');

    targetSlot = $('.target:checked').val();

    var activeHandle1 = $('#active-handle1');
    var pairedUuid1 = $('#paired-uuid1');
    var sid1 = $("#sid1");
    var sidLabel1 = $("#sid-label1");
    var handleLabel1 = $('#handle-label1');
    var uuidLabel1 = $('#uuid-label1');

    var activeHandle2 = $('#active-handle2');
    var pairedUuid2 = $('#paired-uuid2');
    var sid2 = $("#sid2");
    var sidLabel2 = $("#sid-label2");
    var handleLabel2 = $('#handle-label2');
    var uuidLabel2 = $('#uuid-label2');

    var activeHandle3 = $('#active-handle3');
    var pairedUuid3 = $('#paired-uuid3');
    var sid3 = $("#sid3");
    var sidLabel3 = $("#sid-label3");
    var handleLabel3 = $('#handle-label3');
    var uuidLabel3 = $('#uuid-label3');

    var activeHandle4 = $('#active-handle4');
    var pairedUuid4 = $('#paired-uuid4');
    var sid4 = $("#sid4");
    var sidLabel4 = $("#sid-label4");
    var handleLabel4 = $('#handle-label4');
    var uuidLabel4 = $('#uuid-label4');

    var activeHandle5 = $('#active-handle5');
    var pairedUuid5 = $('#paired-uuid5');
    var sid5 = $("#sid5");
    var sidLabel5 = $("#sid-label5");
    var handleLabel5 = $('#handle-label5');
    var uuidLabel5 = $('#uuid-label5');

    var activeHandle6 = $('#active-handle6');
    var pairedUuid6 = $('#paired-uuid6');
    var sid6 = $("#sid6");
    var sidLabel6 = $("#sid-label6");
    var handleLabel6 = $('#handle-label6');
    var uuidLabel6 = $('#uuid-label6');

    var eqTemplate = $('.eq-template');
    var eqAnchor = $('#eq-anchor');
    var myUUID = "";
    var currentPairingDevice = {};

    initBtn.button({text: true, label: "Initialize"});
    discoverBtn.button({text: true, label: "Discover"});
    listBtn.button({text: true, label: "List"});
    stopBtn.button({text: true, label: "Stop"});
    cExecBtn.button({text: true, label: "Execute"});
    revealBtn.button({text: true, label: "Reveal"});
    hideBtn.button({text: true, label: "Hide"});

    var controllerLog = $('#controller-log');
    var cLogLimit = $('#c-log-limit');
    var deviceTemplate = $(".device-template");
    var deviceAnchor = $("#mdx-device-wrapper");
    var cargAnchor = $("#c-arg-anchor");
    var cLogLine = $('#c-log-line');

    var _state = { pairing : { uuid : -1, handle : -1 }, sid : -1 };

    var TRANSITION = 400;

    var C_PS3_CPU_MEMORY_TIMEOUT_ID = 0;
    var C_CPU_MEMORY_USAGE_TIMEOUT_MSECS = 60000;  // 1 minute
    var cMinCpu=0, cCurrentCpu=0, cMaxCpu=0;
    var cMinMemory=0, cCurrentMemory=0, cMaxMemory=0;
    var numberPolls = 0;

    mdx.app = {
        fixture: { selector: {}, targets: [] },

        clearControllerConsole: function() {
            $("#controller-log > .log-entry").remove();
            cLogLine.text(0);
        },

        nrdStateListener: function(evt) {
            switch(evt.type) {
            case "init":
                if (evt.status == "READY") {
                    mdx.app.log("NRDP - " + JSON.stringify(evt, null, 2));
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
             mdx.app.log("INIT- " + JSON.stringify(evt, null, 2));
                }
                break;
            case "initerror":
                 mdx.app.log("INTERIOR- " + JSON.stringify(evt, null, 2));
                break;
            default: break;
            }
        }
    };

    var clickDispatcher = function(event) {
        var id = $(event.currentTarget).attr('id');

        switch(id) {
        case "init":
            mdx.init("CONTROLLER", commandMap );
            break;

        case "list":
            mdx.app.fixture.targets = mdx.discovery.getTargetList(mdx.app.fixture.selector);
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
        case "c-execute":
            var cmd = $('.c-command:checked').val();
            var input = $('input[name|="carg"]');
            var args = [];
            for (var i=0; i < input.length; i++) {
                var clz = "#carg-" + (i+1);
                if ($(clz).hasClass('carg-callback') && $(clz + ':checked').length) {
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
            var selectedTargetSlot = $('.target:checked').val();
                    var targetSession = "#sid"+selectedTargetSlot;
                    var targetSessionId = $(targetSession).text();

                    var payload = eval('('+args[1]+')');
                    mdx.app.log("Send Message to sid =" + targetSessionId + " " + args[0] + " " + args[1]);
                    mdx.session.sendMessage(args[2], args[0], payload);
                } else if (cmd == "dePair"){
                        mdx.app.log("Command: " + cmd + "(" + args.join(",") + "): " + mdx.pair[cmd].apply(mdx.app, args), "command");
                var uuid = document.getElementById('paired-uuid'+targetSlot).innerHTML;
                    _state.pairing.handle = -1;
                    setDepaired(uuid);
                    mdx.app.log("PAIRING DELETED - ");
                    _state.sid = 0;
                    eval("sidLabel"+targetSlot).removeClass('active');
                eval("sidLabel"+targetSlot).addClass('inactive');
                    eval("sid"+targetSlot).text(_state.sid);
                    var eq = eqTemplate.clone();
                    eq.removeClass("eq-template");
                    eq.attr('sid', targetSessionId);
                    eq.find('.eq-sid').text(targetSessionId);
                    eqAnchor.append(eq);
                    eq.fadeIn(TRANSITION);
                }
                else if (cmd == "endSession"){
                        mdx.app.log("Command: " + cmd + "(" + args.join(",") + "): " + mdx.session[cmd].apply(mdx.app, args), "command");
                    _state.sid = 0;
                    eval("sidLabel"+targetSlot).removeClass('active');
                eval("sidLabel"+targetSlot).addClass('inactive');
                    eval("sid"+targetSlot).text(_state.sid);
                    var eq = eqTemplate.clone();
                    eq.removeClass("eq-template");
                    eq.attr('sid', targetSessionId);
                    eq.find('.eq-sid').text(targetSessionId);
                    eqAnchor.append(eq);

        }

                else if (cmd == "httpRequest"){
           var postorget = $('.postorget:checked').val();
                   mdx.app.log("Send HTTPRequest: " + args[0] + " " + args[1] + " " + postorget);
               simpleHttpRequest(args[0], mdx.app.log, mdx.app.log, (postorget === '0') ? "POST" : "GET");
               //nrdp.mdx.SendMdxHttpRequest(args[0], postorget, '0000', '', args[1]);
        }


        else {
                    mdx.app.log("Command: " + cmd + "(" + args.join(",") + "): " + mdx.session[cmd].apply(mdx.app, args), "command");
                }
            } catch(e) {
                mdx.app.log("Command: " + cmd + "(" + args.join(",") + "): Exception: " + e.message, "error");
            }
            break;

        case "c-log-dispatcher":
            if ($('#c-log-dispatcher:checked').length) {
           //     mdximpl.addEventListener(mdximpl.SCOPE, mdximpl._prv_events.dispatcher, logFromNative);
            } else {
            //    mdximpl.removeEventListener(mdximpl.SCOPE, mdximpl._prv_events.dispatcher, logFromNative);
            }
            break;

        case "c-log-type-current":
        var entry = ".entry-" + targetSlot;

        var logger = "#controller-log";
            if ($('#' + id + ':checked').length) {
        if(entry ==".entry-1"){
                    $(logger + ' > .entry-2').hide();
                    $(logger + ' > .entry-3').hide();
            $(logger + ' > .entry-4').hide();
            $(logger + ' > .entry-5').hide();
            $(logger + ' > .entry-6').hide();
        }
        else if(entry ==".entry-2"){
                    $(logger + ' > .entry-1').hide();
                    $(logger + ' > .entry-3').hide();
            $(logger + ' > .entry-4').hide();
            $(logger + ' > .entry-5').hide();
            $(logger + ' > .entry-6').hide();
        }

        else if(entry ==".entry-3"){
                    $(logger + ' > .entry-1').hide();
                    $(logger + ' > .entry-2').hide();
            $(logger + ' > .entry-4').hide();
            $(logger + ' > .entry-5').hide();
            $(logger + ' > .entry-6').hide();
        }

        else if(entry ==".entry-4"){
                    $(logger + ' > .entry-1').hide();
                    $(logger + ' > .entry-2').hide();
            $(logger + ' > .entry-3').hide();
            $(logger + ' > .entry-5').hide();
            $(logger + ' > .entry-6').hide();
        }

        else if(entry ==".entry-5"){
                    $(logger + ' > .entry-1').hide();
                    $(logger + ' > .entry-2').hide();
            $(logger + ' > .entry-3').hide();
            $(logger + ' > .entry-4').hide();
            $(logger + ' > .entry-6').hide();
        }

        else if(entry ==".entry-6"){
                    $(logger + ' > .entry-1').hide();
                    $(logger + ' > .entry-2').hide();
            $(logger + ' > .entry-3').hide();
            $(logger + ' > .entry-4').hide();
            $(logger + ' > .entry-5').hide();
        }

            } else {
                $(logger + ' > .entry-1').show();
                $(logger + ' > .entry-2').show();
                $(logger + ' > .entry-3').show();
        $(logger + ' > .entry-4').hide();
        $(logger + ' > .entry-5').hide();
        $(logger + ' > .entry-6').hide();

            }
            break;

        break;

        case "c-log-type-fromnative":
        case "c-log-type-tonative":
        case "c-log-type-discovery":
        case "c-log-type-property":
        case "c-log-type-event":
        case "c-log-type-state":
        case "c-log-type-error":
        case "c-log-type-command":
            var entry = ".entry-" + id.substr(id.lastIndexOf("-") + 1);
            var logger = "#controller-log";
            if ($('#' + id + ':checked').length) {
                $(logger + ' > ' + entry).show();
            } else {
                $(logger + ' > ' + entry).hide();
            }
            break;
        case "c-is-PS3":
            if ($('#c-is-PS3:checked').length) {
                $(".c-cpu-usage-lbl").css("color", 'black');
                $(".c-memory-usage-lbl").css("color", 'black');
                $('.c-cpu-usage-val').attr('disabled', '');
                $('.c-memory-usage-val').attr('disabled', '');
                if(C_PS3_CPU_MEMORY_TIMEOUT_ID > 0) {
                    clearInterval(C_PS3_CPU_MEMORY_TIMEOUT_ID);
                    C_PS3_CPU_MEMORY_TIMEOUT_ID = 0;
                }
                getCpuMemoryUsage();
                C_PS3_CPU_MEMORY_TIMEOUT_ID = setInterval(getCpuMemoryUsage, C_CPU_MEMORY_USAGE_TIMEOUT_MSECS);
            }
            else {
                $(".c-cpu-usage-lbl").css("color", '#808080');
                $(".c-memory-usage-lbl").css("color", '#808080');
                $('.c-cpu-usage-val').attr('disabled', 'disabled');
                $('.c-memory-usage-val').attr('disabled', 'disabled');
                if(C_PS3_CPU_MEMORY_TIMEOUT_ID > 0) {
                    clearInterval(C_PS3_CPU_MEMORY_TIMEOUT_ID);
                    C_PS3_CPU_MEMORY_TIMEOUT_ID = 0;
                }
            }
            break;
        default:
        }
    }

    var getCpuMemoryUsage = function() {
        if(nrdp.ps3) {
            cCurrentCpu = nrdp.ps3.getCPUUtilization();
            cCurrentMemory = nrdp.ps3.getAvailableMemory();
            if(cCurrentCpu < cMinCpu) { cMinCpu = cCurrentCpu; }
            if(cCurrentMemory < cMinMemory) { cMinMemory = cCurrentMemory; }
            if(cCurrentCpu > cMaxCpu) { cMaxCpu = cCurrentCpu; }
            if(cCurrentMemory > cMaxMemory) { cMaxMemory = cCurrentMemory; }
            $("#c-min-cpu-usage-val").val(cMinCpu);
            $("#c-max-cpu-usage-val").val(cMaxCpu);
            $("#c-cur-cpu-usage-val").val(cCurrentCpu);
            $("#c-min-memory-usage-val").val(cMinMemory);
            $("#c-max-memory-usage-val").val(cMaxMemory);
            $("#c-cur-memory-usage-val").val(cCurrentMemory);
        }
        else {
            $("#c-min-cpu-usage-val").val("NA");
            $("#c-max-cpu-usage-val").val("NA");
            $("#c-cur-cpu-usage-val").val("NA");
            $("#c-min-memory-usage-val").val("NA");
            $("#c-max-memory-usage-val").val("NA");
            $("#c-cur-memory-usage-val").val("NA");
        }
    }

    var argClickDispatcher = function(event) {


        var evt = $(event.currentTarget);
        var value = evt.attr("value");

    if(value == "httpRequest"){
        $('#duuid').show();
    }
    else {
        $('#duuid').hide();
    }

    if((value=="1")||(value=="2")||(value=="3")||(value=="4")||(value=="5")||(value=="6")){
        updateTarget();
    } else {
            setArgs(cparams, value);
    }
    }

    var setPaired = function(device) {

        device.find('.handle').text(_state.pairing.handle);
        device.addClass('active');

        eval("activeHandle"+targetSlot).text(_state.pairing.handle);
        eval("pairedUuid"+targetSlot).text(_state.pairing.uuid);
        eval("handleLabel"+targetSlot).removeClass('inactive');
        eval("handleLabel"+targetSlot).addClass('active');
        eval("uuidLabel"+targetSlot).removeClass('inactive');
        eval("uuidLabel"+targetSlot).addClass('active');
        //$('.c-command').removeAttr('disabled');
        cExecBtn.button("enable");

        var hdl = "#active-handle"+targetSlot;
        var hdl_text = $(hdl).text();
        $('.carg-handle').val(hdl_text);

    }

    var setDepaired = function(uuid) {
        //if (!device) return;
        _state.pairing.uuid = -1;
        _state.pairing.handle = -1;
        eval("handleLabel"+targetSlot).removeClass('active');
        eval("handleLabel"+targetSlot).addClass('inactive');
        eval("uuidLabel"+targetSlot).removeClass('active');
        eval("uuidLabel"+targetSlot).addClass('inactive');
    var myuuid = '.' + uuid;

    var selDevice = $(myuuid);

    selDevice.find('.handle').text("");
        selDevice.removeClass('active');

    eval("activeHandle"+targetSlot).text("INVALID_HANDLE");
        eval("pairedUuid"+targetSlot).text(-1);
        //$('.c-command').attr('disabled', 'disabled');
        //cExecBtn.button("disable");
    }

    var setRadioButton = function(){
        $('input:radio[name=c-cmd]')[4].checked = true;
        setArgs(cparams, 'httpRequest');
    }

    var pairClickHandler = function(event) {

        var device = $(event.currentTarget).parentsUntil('#mdx-device-wrapper', '[uuid]');

    currentPairingDevice = device;
        var uuid = device.attr('uuid');

    var dlocation = currentPairingDevice.find('.dlocation').text();

    if(uuid.indexOf('DIAL')!=-1){
        $('#duuid').text(uuid);
            mdx.app.log("Getting Dial Device Location: " +dlocation);
            simpleHttpRequest(dlocation, mdx.app.log, mdx.app.log, "GET", 'initial');
        setTimeout(function(){setRadioButton()}, 1500);

    }
    else{

        if (_state.pairing.handle != -1) {
                //mdx.app.log("Already paired");
                //return;
            }

        var handle =  mdx.pair.pairingRequest(uuid);


            if (_state.pairing.uuid != -1) {
             //var dsel = '.device[uuid=' + _state.pairing.uuid + ']';
                //setDepaired($(dsel));
            }
        _state.pairing.uuid = uuid;

            mdx.app.log("Command: pair(" + uuid + ") ", "command");
       }

    }


    var cstartSessionResponse = function(evt) {

        _state.sid = evt.sid;

        mdx.app.log("START SESSION RESPONSE- " + JSON.stringify(evt, null, 2));
        eval("sidLabel"+targetSlot).addClass('active');
        eval("sid"+targetSlot).text(_state.sid);
        var eq = eqTemplate.clone();
        eq.removeClass("eq-template");
        eq.attr('sid', _state.sid);
        eq.find('.eq-sid').text(_state.sid);
        eqAnchor.append(eq);
        eq.fadeIn(TRANSITION);
    }

    var cEndSession = function(evt) {

    mdx.app.log("END SESSION- " + JSON.stringify(evt, null, 2));
            if (_state.sid == evt.sid ) {
            mdx.app.log("end session Id = " + evt.sid);
            _state.sid = 0;
            eval("sidLabel"+targetSlot).removeClass('active');
            eval("sid"+targetSlot).text(_state.sid);
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
        _state.pairing.handle = evt.pairingContext;

    mdx.app.log("PAIRING RESPONSE- " + JSON.stringify(evt, null, 2));

    if((evt.errorCode == "0")||(evt.errorCode == undefined)){
        setPaired(currentPairingDevice);
    }
    else{
        mdx.app.log("PAIRING REJECTED FROM TARGET");
    }

    }

    var pairingDeleted = function(evt) {


        _state.pairing.handle = -1;
        mdx.app.log("PAIRING DELETED- " + JSON.stringify(evt, null, 2));
        _state.pairing.uuid = -1;
        _state.pairing.handle = -1;
        eval("activeHandle"+targetSlot).text("INVALID_HANDLE");
        eval("pairedUuid"+targetSlot).text(-1);
        cExecBtn.button("disable");
        _state.sid = 0;
        eval("sidLabel"+targetSlot).removeClass('active');


        eval("handleLabel"+targetSlot).removeClass('active');
        eval("handleLabel"+targetSlot).addClass('inactive');
        eval("uuidLabel"+targetSlot).removeClass('active');
        eval("uuidLabel"+targetSlot).addClass('inactive');
    var myuuid = '.' + evt.uuid;
    var selDevice = $(myuuid);

    selDevice.find('.handle').text("");
        selDevice.removeClass('active');

        eval("sid"+targetSlot).text(_state.sid);
        var eq = eqTemplate.clone();
        eq.removeClass("eq-template");
        eq.attr('sid', _state.sid);
        eq.find('.eq-sid').text(_state.sid);
        eqAnchor.append(eq);
        eq.fadeIn(TRANSITION);
    }

    var messageDelivered = function(evt) {
        if(bValidateMessagesRecv == false) {  // don't log for the MDX stress test
            mdx.app.log("MESSAGE DELIVERED- " + JSON.stringify(evt, null, 2));
        }
    }

    var appControllerMessageCb = function(evt) {
        if(bValidateMessagesRecv == false) {  // don't log for the MDX stress test
        mdx.app.log("CONTROLLER MESSAGE CALLBACK- " + JSON.stringify(evt, null, 2));
        }

        if (evt.type == "PLAYER_PLAY") {
            if(bValidateMessagesRecv == false) {  // don't log for the MDX stress test
                mdx.app.log("starting to play movie " + evt.msgObject.catalogId + " track: " +  evt.msgObject.trackId);
            }
        } else if (evt.type == "PLAYER_CURRENT_STATE"){
            if(bValidateMessagesRecv == false) {  // don't log for the MDX stress test
                mdx.app.log("Received current state" + evt.msgObject );
            }
        } else {
            if(bValidateMessagesRecv == false) {  // don't log for the MDX stress test
                mdx.app.log("Received " + evt.type + "data " +  + evt.msgObject );
            }
        }
    }

    var appTargetMessageCb = function(evt) {

        if(bValidateMessagesRecv == false) {  // don't log for the MDX stress test
            mdx.app.log("TARGET MESSAGE CALLBACK- " + JSON.stringify(evt, null, 2));
        }

        if(evt.type == "START_MESSAGE_SEND_STRESS_TEST") {
            numberMessagesReceivedFromTarget = 0;
            bValidateMessagesRecv = true;
            $("#c-recv-messages-number").val(0);
            $("#c-matched-messages-number").val(0);
            $("#c-mismatched-messages-number").val(0);
            $("#c-expected-messages-number").val(evt.msgObject.numbermessages);
        }
        else if(evt.type == "END_MESSAGE_SEND_STRESS_TEST") {
            bValidateMessagesRecv = false;
        }
        else {
            numberMessagesReceivedFromTarget += 1;
        }

        $("#c-recv-messages-number").val(numberMessagesReceivedFromTarget);
        var matchedMessages = $("#c-matched-messages-number").val();
        var mismatchedMessages = $("#c-mismatched-messages-number").val();
        if(bValidateMessagesRecv == true && numberMessagesReceivedFromTarget > 0) {
            var mdxmsg_indx = (numberMessagesReceivedFromTarget-1) % MDX_T2C_MESSAGES.mdxmessages.length;
            if( evt.type != MDX_T2C_MESSAGES.mdxmessages[mdxmsg_indx].name ) {
                $("#c-mismatched-messages-number").val(parseInt(mismatchedMessages)+1);
                console.log("ERROR: Received message - name:" + evt.type + "; expecting:" + MDX_T2C_MESSAGES.mdxmessages[mdxmsg_indx].name);
            }
            else if(JSON.stringify(evt.msgObject) != JSON.stringify(MDX_T2C_MESSAGES.mdxmessages[mdxmsg_indx].object)) {
                $("#c-mismatched-messages-number").val(parseInt(mismatchedMessages)+1);
                console.log("ERROR: Received msgObject - name:" + JSON.stringify(evt.msgObject) + "; expecting:" + JSON.stringify(MDX_T2C_MESSAGES.mdxmessages[mdxmsg_indx].object));
            }
            else {
                $("#c-matched-messages-number").val(parseInt(matchedMessages)+1);
                //console.log("SUCCESS: Received message - name:" + evt.type + "; expecting:" + MDX_T2C_MESSAGES.mdxmessages[mdxmsg_indx].name);
            }
        }
    }

    var logThisEvent= function(evt) {
        mdx.app.log("Event Received- " + JSON.stringify(evt, null, 2));
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

        var csid = "#sid"+targetSlot;
            var currentSid = $(csid).text();

        if(label == "carg-1"){
        if(cmd=="sendMessage"){
            var argInput = $('<input id="' + label + '" class="arg-text" name="' + label + '" type="' + params[cmd].type[i] + '" value="PLAYER_PLAY" />');
        }

        else if(cmd=="httpRequest"){

            var argInputTxt = '<select name="' + label + '" id="' + label + '"><option value="'+cleanUrl + appName_1+'">'+appName_1+'</option>';

            if(appName_2 != undefined){
                argInputTxt += '<option value="'+cleanUrl + appName_2+'">'+appName_2+'</option>';
            }

            argInputTxt += '</select>';

            var argInput = $(argInputTxt);

        }

        else {
            var argInput = $('<input id="' + label + '" class="arg-text" name="' + label + '" type="' + params[cmd].type[i] + '" value="" />');
        }
        }

        else if(label == "carg-2"){

        if(cmd=="httpRequest"){
            var argInput = $('<input id="' + label + '" class="arg-text" name="' + label + '" type="' + params[cmd].type[i] + '" value="" >');
        }

        else{
            var argInput = $('<input id="' + label + '" class="arg-text" name="' + label + '" type="' + params[cmd].type[i] + '" value={"catalogId":"http://path","trackId":"1234"} />');
        }

            }
        else if(label == "carg-3"){

        if(cmd=="httpRequest"){
            var argInput = $('<input id="' + label + '"  name="postorget" type="radio" name="type" value="1" class="postorget"/><span class="arg-text">Status(GET)</span><input id="' + label + '" name="postorget" type="radio" name="type" value="0" class="postorget" /><span class="arg-text">Launch(POST)</span>');
        }
        else{
            var argInput = $('<input id="' + label + '" class="arg-text" name="' + label + '" type="' + params[cmd].type[i] + '" value='+currentSid+' />');
        }
            }

        else{
        var argInput = $('<input id="' + label + '" class="arg-text" name="' + label + '" type="' + params[cmd].type[i] + '" value="" />');
        }

            if (params[cmd].value[i]) argInput.val(params[cmd].value[i]);
            argInput.addClass(prefix + "-" + args[i]);


        var prepop = eval("activeHandle"+targetSlot).text();
        if (args[i] == "handle") argInput.val(prepop);


        if (args[i] == "sid") argInput.val(currentSid);

            argInput.addClass(prefix);
            anchor.append(argLabel);
            anchor.append(argInput);
            anchor.append($('<br class="' + prefix +'" />'));
        }
    }

    mdx.app.updateDevices = function() {

        mdx.app.fixture.targets = mdx.discovery.getTargetList();
        deviceTemplate.siblings().each(function(i, device) {
            removeDevice(i, $(device));
        });

        mdx.app.fixture.targets.forEach(function(device) {
           mdx.app.log("DEVICE FOUND- " + JSON.stringify(device, null, 2));
       var devicez = {selectorKey:"netflix", loc:device.location, uuid:device.uuid, usn:device.usn };
       addDevice(devicez);
        });
    };

    var discoverEvent = function(event) {
        if (event.type == "devicefound") {
        mdx.app.log("DEVICE FOUND- " + JSON.stringify(event, null, 2));
            mdx.app.fixture.targets = mdx.discovery.getTargetList();
            var myuuid = mdx.discovery.getUUID(event.USN);
        var uuidclass = '.'+ myuuid;
            var device = {selectorKey:"netflix", loc:event.location, uuid:myuuid, usn:event.USN };

            //loop through list of UUIDs, see if device already added, if not, then add.
        var dupe = deviceAnchor.find(uuidclass).text();

        if(dupe == ''){
            addDevice(device);
        }

        else {
            mdx.app.log("Device with UUID: " + myuuid + " already exists in device list.");
        }

        var alreadyPaired = mdx.device.getPairingContextFromUuid(myuuid);

        if (alreadyPaired != ''){

        var uuidclass = '.' + myuuid;

        var pdevice = $(uuidclass);
        pdevice.addClass('active');
        pdevice.find('.handle').text(alreadyPaired);

            eval("activeHandle"+targetSlot).text(alreadyPaired);
            eval("pairedUuid"+targetSlot).text(myuuid);
            eval("handleLabel"+targetSlot).removeClass('inactive');
            eval("handleLabel"+targetSlot).addClass('active');
            eval("uuidLabel"+targetSlot).removeClass('inactive');
            eval("uuidLabel"+targetSlot).addClass('active');
            cExecBtn.button("enable");

            var hdl = "#active-handle"+targetSlot;
            var hdl_text = $(hdl).text();
            $('.carg-handle').val(hdl_text);

        }

            return;
        }
        if (event.type == "devicebyebye") {

        mdx.app.log("DEVICE BYE- " + JSON.stringify(event, null, 2));
            mdx.app.fixture.target = mdx.discovery.getTargetList();
            var jsel = '[uuid=' + event.target.uuid + ']';
            var device = $(jsel);
            if (device.length) {
                if (_state.paired.uuid == device.attr('uuid')) setDepaired(device);
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
    target.addClass(device.uuid);
    target.attr("uuid", device.uuid);
        var line = target.find("li:first");
        line.children("span:first").text(device.selectorKey);
        line = line.next();
        line.children("span:first").text(device.loc);
        line = line.next();
        line.children("span:first").text(device.uuid);
        line = line.next();
        line.children("span:first").text(device.usn);

    if(device.uuid.indexOf('DIAL')!=-1){
        //hide pair button on dial devices
        var noPair = target.find(".ui-button-text");
        noPair.text('SELECT');
        $('#c-httpRequest').show();
        $('#c-httpRequest-label').show();

    }

        deviceAnchor.append(target);
        if (_state.pairing.uuid == device.uuid) setPaired(target);
        target.fadeIn(TRANSITION);
    };


    mdx.app.sessionHandler = function(event) {
        switch(event.action) {
        case "start":
            _state.sid = event.sid;
            eval("sidLabel"+targetSlot).addClass('active');
            eval("sid"+targetSlot).text(_state.sid);
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
        mdx.pair.addEventListener("pairingresponse", controllerPairingResponse);
        mdx.pair.addEventListener("pairingdeleted", pairingDeleted);
        mdx.session.addEventListener("startSessionResponse", cstartSessionResponse);
        mdx.session.addEventListener("sessionended", cEndSession);
        mdx.session.addEventListener("messagedelivered", messageDelivered);
        mdx.pair.addEventListener("pairingrequestreceived", logThisEvent);
        mdx.pair.addEventListener("pairinggrant", logThisEvent);
        mdx.session.addEventListener("sessionstart", logThisEvent);
        mdx.session.addEventListener("messagedelivered", logThisEvent);
        mdx.session.addEventListener("messagingerror", logThisEvent);



        for (var i = 0; i < controllerMsgs.length; i++){
            mdx.session.addMessageListener(controllerMsgs[i], appControllerMessageCb);
        }
        for (var i = 0; i < targetMsgs.length; i++){
            mdx.session.addMessageListener(targetMsgs[i], appTargetMessageCb);
        }
        mdx.session.addMessageListener("START_MESSAGE_SEND_STRESS_TEST", appTargetMessageCb);
        mdx.session.addMessageListener("END_MESSAGE_SEND_STRESS_TEST", appTargetMessageCb);
    }

    mdx.app.log = function(line, type) {
        var dt = new Date();
        var clz = type || "fromnative";
        var lines = $('#controller-log > .log-entry').length;
        var over = lines - cLogLimit.val() + 1;
        if (over > 0 ) $("#controller-log > .log-entry").slice(0, over).remove();
        var entry = $('<div class="log-entry">' + dt.toLocaleTimeString() + ":" + dt.getMilliseconds() + " " + line + '</div>');
        entry.addClass("entry-" + clz);

        entry.addClass("entry-" + targetSlot);

        if ($('#c-log-type-' + clz + ':checked').length == 0) {
            entry.hide();
        }
        controllerLog.append(entry);
        controllerLog.scrollTop(100000);
        lines = $('#controller-log > .log-entry').length;
        cLogLine.text(lines);
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
    $('input.c-is-PS3[type="checkbox"]').bind('click', clickDispatcher);
    $('input[type="radio"]').bind('click', argClickDispatcher);
    $('.pair').live("click", pairClickHandler);

    var logFromNative = new _LogHandler(mdx.app.log, "fromnative");
    var logToNative = new _LogHandler(mdx.app.log, "tonative");
    var logEvent = new _LogHandler(mdx.app.log, "event");
    setArgs(cparams, "startSession");
    setArgs(cparams, "dePair");

    initialize = function() {

        mdx.MYUUID = "NRDP-CONTROLLER-UUID-007";

    var urlParams = location.search;

    if(urlParams.indexOf('config')!=-1){
        document.getElementById("configParams").style.display="block";
    }

    //nrdp init handler code goes here. nrdp.init should no longer be called.
       mdx.app.log("NRDP - INIT " );
                    var role = "CONTROLLER";
                var commandMap = {};
                mdx.app.addMdxEvtListeners();
        mdx.app.log("NRDP - INIT: adding evetn listeners " );
                mdx.init(role, commandMap);
        mdx.app.log("NRDP - INIT : initializing MDX" );

        //nrdp.addEventListener("init", mdx.app.nrdStateListener);
        //nrdp.init();
    $('#t2').hide();
    $('#t3').hide();
    $('#t4').hide();
    $('#t5').hide();
    $('#t6').hide();

        clearTargetMessages();
    }
});

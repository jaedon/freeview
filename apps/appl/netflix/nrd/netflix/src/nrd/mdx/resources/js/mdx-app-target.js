 /*
  * (c) 1997-2015 Netflix, Inc.  All content herein is protected by
  * U.S. copyright and other applicable intellectual property laws and
  * may not be copied without the express permission of Netflix, Inc.,
  * which reserves all rights.  Reuse of any of this content for any
  * purpose without the permission of Netflix, Inc. is strictly
  * prohibited.
  */


var actions;

var doActivateButton = {
    "key.right" : function() { moveTo("sendSampleMessageButton"); },
    "key.enter" : function() {doActivate(); }
};

var sendSampleMessageButton = {
    "key.left" : function() { moveTo("doActivateButton"); },
    "key.right" : function() { moveTo("quitButton"); },
    "key.enter" : function() {sendMessage(); }
};

var quitButton = {
    "key.left" : function() { moveTo("sendSampleMessageButton"); },
    "key.enter" : function() { nrdp.exit();}
};


var pairIt = {
    "key.down" : function() { moveTo("dontPair"); },
    "key.enter" : function() {grantPairing(); }
};

var regPairIt = {
    "key.down" : function() { moveTo("dontRegPair"); },
    "key.enter" : function() {grantRegPairing(); }
};

var dontPair = {
    "key.up" : function() { moveTo("pairIt"); },
    "key.enter" : function() { denyInitialPairing(); }
};

var dontRegPair = {
    "key.up" : function() { moveTo("regPairIt"); },
    "key.enter" : function() { denyInitialRegPair(); }
};

function moveTo(n) {
    var cur = document.getElementById("focus");
    var sel = document.getElementsByClassName(n)[0];
    cur.id = "";
    sel.id = "focus";
    actions = window[n];
}

function keyEvent(evt) {
    var key = nrdpPartner.Keys[evt.which];
    var action = actions[key];
    if (action) action();
}

document.addEventListener("keydown", keyEvent);
actions = doActivateButton;

function clearControllerMessages() {
    $("#t-sent-messages-number").val(0);
    $("#t-recv-messages-number").val(0);
    $("#t-expected-messages-number").val(0);
    $("#t-matched-messages-number").val(0);
    $("#t-mismatched-messages-number").val(0);
    $("#t-send-messages-loops").val(1);
}

function updateController() {
    selectedControllerSlot = $('.controller:checked').val();

    var hdl = "#active-handle"+selectedControllerSlot;
    var hdl_text = $(hdl).text();

    if(hdl_text == "INVALID_HANDLE"){
    	//$('.t-command').attr('disabled', 'disabled');
    	//$('#t-execute').button("disable");
    }
    else {
 	//$('.t-command').removeAttr('disabled');
	//$('#t-execute').button("enable");
    }
}

function setCommandMap() {
	map = document.getElementById('cmdMap').value;
	mdx.init('TARGET', map);
}

var SEND_MESSAGE_T2C_TIME_INTERVAL_MSGS = 200;    // 20msec between each successive message in a loop
var SEND_MESSAGE_T2C_TIME_INTERVAL_LOOP = 5000;    // 5 seconds between each loop

function sendControllerMessages() {
    var selectedControllerSlot = $('.controller:checked').val();
    var controllerSession = "#sid"+selectedControllerSlot;
    var controllerSessionId = $(controllerSession).text();
    var totalLoops = $("#t-send-messages-loops").val();
    var startSendStressTestObj = {"numbermessages":MDX_T2C_MESSAGES.mdxmessages.length * parseInt(totalLoops)};
    var endSendStressTestObj = {"numbermessages":MDX_T2C_MESSAGES.mdxmessages.length * parseInt(totalLoops)};
    var emptyObj = {};
    mdx.session.sendMessage(controllerSessionId, "START_MESSAGE_SEND_STRESS_TEST", startSendStressTestObj);

    $("#t-sent-messages-number").val(0);
    setTimeout(function(){sendControllerOneMessage(0, 0)}, SEND_MESSAGE_T2C_TIME_INTERVAL_MSGS);
}

function sendControllerOneMessage(currentLoopIndex, currentMessageIndex)
{
    var selectedControllerSlot = $('.controller:checked').val();
    var controllerSession = "#sid"+selectedControllerSlot;
    var controllerSessionId = $(controllerSession).text();
    var totalLoops = $("#t-send-messages-loops").val();
    var endSendStressTestObj = {"numbermessages":MDX_T2C_MESSAGES.mdxmessages.length};

    if((currentMessageIndex >= MDX_T2C_MESSAGES.mdxmessages.length) && (currentLoopIndex <totalLoops)) {
        currentMessageIndex = 0;
        currentLoopIndex += 1;
    }
    //console.log("Sending message to controller - currentLoopIndex:" + currentLoopIndex + "; currentMessageIndex:" + currentMessageIndex);

    mdx.session.sendMessage(controllerSessionId, MDX_T2C_MESSAGES.mdxmessages[currentMessageIndex].name, MDX_T2C_MESSAGES.mdxmessages[currentMessageIndex].object);
    var currentMsgsSent = $("#t-sent-messages-number").val();
    $("#t-sent-messages-number").val(parseInt(currentMsgsSent)+1);

    if(currentLoopIndex + 1 >= totalLoops && ((currentMessageIndex + 1) >= MDX_T2C_MESSAGES.mdxmessages.length)) {
        mdx.session.sendMessage(controllerSessionId, "END_MESSAGE_SEND_STRESS_TEST", endSendStressTestObj);
        return;
    }
    if(currentMessageIndex >= MDX_T2C_MESSAGES.mdxmessages.length) {
        currentMessageIndex = 0;
        currentLoopIndex += 1;
        setTimeout(function(){sendControllerOneMessage(currentLoopIndex, currentMessageIndex)}, SEND_MESSAGE_T2C_TIME_INTERVAL_LOOP);
    }
    else {
        currentMessageIndex += 1;
        setTimeout(function(){sendControllerOneMessage(currentLoopIndex, currentMessageIndex)}, SEND_MESSAGE_T2C_TIME_INTERVAL_MSGS);
    }
}


var tPRR = '';

function mdxConfigure(){
	mdx.app.tlog("MDX Configure");
	var advertisingperiod = document.getElementById('advertisingperiod').value;
	var advertisingTTL = document.getElementById('advertisingTTL').value;
	var advertisingPort = document.getElementById('advertisingPort').value;
	var listeningPort = document.getElementById('listeningPort').value;
	mdx.configure(advertisingperiod,advertisingTTL,advertisingPort, listeningPort)
}

function grantPairing(){
	mdx.app.tlog("Pairing Granted");
    mdx.pair.allowPairing(tPRR.uuid);
	var requestScreen = $("#pairingRequestScreen");
	requestScreen.hide();
	tPRR = '';

	try{
		document.getElementById('MDXstatus').innerHTML = 'MDX is PAIRED - Session Not Started';
		moveTo("sendSampleMessageButton");
		
	}
	catch(e) {}
}

function regPairDeactivate() {
	var requestScreen = $("#regPairRequestScreen");
	requestScreen.hide();

    var dak = nrdp.registration.currentDeviceAccount;
    mdx.app.tlog("Deactivating account " + dak);
    nrdp.registration.deactivate(dak, function(data) {
        console.log(JSON.stringify(data));
        mdx.app.tlog("Device account " + dak + " deactivated");
        nrdp.registration.createDeviceAccount(function(data) {
            console.log(JSON.stringify(data));
            dak = data.key;
            mdx.app.tlog("Selecting device account " + dak);
            nrdp.registration.selectDeviceAccount(dak, function(data) {
                console.log(JSON.stringify(data));
                mdx.app.tlog("Device account " + dak + " selected");
                grantRegPairing();
            });
        });
    });
}

function grantRegPairing(){
	mdx.app.tlog("RegPairing Granted");
    var pin = $("#pin");
    mdx.pair.allowRegPairing(tPRR.uuid, pin.val());
	var requestScreen = $("#regPairRequestScreen");
	requestScreen.hide();
	tPRR = '';

	try{
		document.getElementById('MDXstatus').innerHTML = 'MDX is PAIRED - Session Not Started';
		moveTo("sendSampleMessageButton");
		
	}
	catch(e) {}
}

function denyInitialPairing(){
	mdx.app.tlog("Pairing Rejected");

	var cUUID = $("#pairUUID").val();
	errorObject = $("#pairError").val();
	var errorObj = eval('('+errorObject+')');
	mdx.pair.sendPairingReject(cUUID, errorObj);
	var requestScreen = $("#pairingRequestScreen");
	requestScreen.hide();
	tPRR = '';
	try{
		moveTo("sendSampleMessageButton");
	}
	catch(e) {}
}

function denyPairing(){
	mdx.app.tlog("Pairing Rejected");
	errorObject = document.getElementById('errorObject').value;
	userUUID = document.getElementById('rejectUUID').value;
	var errorObj = eval('('+errorObject+')');
	mdx.pair.sendPairingReject(userUUID, errorObj);
	tPRR = '';
}

function denyInitialRegPair(){
	mdx.app.tlog("RegPair Rejected");

	var cUUID = $("#regPairUUID").val();
	errorObject = $("#regPairError").val();
	var errorObj = eval('('+errorObject+')');
	mdx.pair.sendRegPairReject(cUUID, errorObj);
	var requestScreen = $("#regPairRequestScreen");
	requestScreen.hide();
	tPRR = '';
	try{
		moveTo("sendSampleMessageButton");
	}
	catch(e) {}
}

function denyRegPair(){
	mdx.app.tlog("RegPair Rejected");
	errorObject = document.getElementById('errorObject').value;
	userUUID = document.getElementById('rejectUUID').value;
	var errorObj = eval('('+errorObject+')');
	mdx.pair.sendRegPairReject(userUUID, errorObj);
	tPRR = '';
}

function changeInterface(){
	mdx.app.tlog("Changing Interface");
	interfaceName = document.getElementById('interfaceName').value;
	interfaceConnected = document.getElementById('connected').value;
	mdx.interfaceChanged(interfaceName, interfaceConnected);
}

if (typeof mdx === "undefined") mdx = {};

$(document).ready(function() {

    var tparams = {
	    endSession: {
	        labels : [ "sid" ],
	        type : ["text" ],
	        value : [ 0x0 ]
        },
        sendMessage: {
            labels: [ "messageName", "messageData", "sid" ],
            type : [ "text", "text" ],
            value : ["", ""]
        },
        dePair : {
            labels : ["handle"],
            type : ["text"],
            value : [""]
        }
    };
	controllerMsgs = ["PLAYER_PLAY","PLAYER_PAUSE","PLAYER_RESUME",
					  "PLAYER_STOP", "PLAYER_SKIP", "PLAYER_SET_CURRENT_TIME",
					  "PLAYER_SET_AUTO_ADVANCE", "PLAYER_GET_CURRENT_STATE",
					  "PLAYER_GET_CAPABILITIES", "HANDSHAKE_ACCEPTED", "MESSAGE_IGNORED" ];
	targetMsgs = ["PLAYER_CURRENT_STATE", "PLAYER_STATE_CHANGED", "PLAYER_CAPABILITIES", "HANDSHAKE" ];

    $(".pair").button({text: true, label: "Pair"});
    $(".clear").button({text: true, label: "Clear"});

    var selectedControllerSlot;
    var initBtn = $('#init');
    var discoverBtn = $('#discover');
    var listBtn = $('#list');
    var stopBtn = $('#stop');
    var tExecBtn = $('#t-execute');
    var revealBtn = $('#reveal');
    var hideBtn = $('#hide');
    var generatorBtn = $("#t-generator");
    var eventBtn = $("#t-event");

    var handleLabel1 = $('#handle-label1');
    var activeHandle1 = $('#active-handle1');
    var uuidLabel1 = $('#uuid-label1');
    var pairedUuid1 = $('#paired-uuid1');
    var sidLabel1 = $("#sid-label1");
    var sid1 = $("#sid1");

    var handleLabel2 = $('#handle-label2');
    var activeHandle2 = $('#active-handle2');
    var uuidLabel2 = $('#uuid-label2');
    var pairedUuid2 = $('#paired-uuid2');
    var sidLabel2 = $("#sid-label2");
    var sid2 = $("#sid2");

    var handleLabel3 = $('#handle-label3');
    var activeHandle3 = $('#active-handle3');
    var uuidLabel3 = $('#uuid-label3');
    var pairedUuid3 = $('#paired-uuid3');
    var sidLabel3 = $("#sid-label3");
    var sid3 = $("#sid3");

    var handleLabel4 = $('#handle-label4');
    var activeHandle4 = $('#active-handle4');
    var uuidLabel4 = $('#uuid-label4');
    var pairedUuid4 = $('#paired-uuid4');
    var sidLabel4 = $("#sid-label4");
    var sid4 = $("#sid4");

    var handleLabel5 = $('#handle-label5');
    var activeHandle5 = $('#active-handle5');
    var uuidLabel5 = $('#uuid-label5');
    var pairedUuid5 = $('#paired-uuid5');
    var sidLabel5 = $("#sid-label5");
    var sid5 = $("#sid5");

    var handleLabel6 = $('#handle-label6');
    var activeHandle6 = $('#active-handle6');
    var uuidLabel6 = $('#uuid-label6');
    var pairedUuid6 = $('#paired-uuid6');
    var sidLabel6 = $("#sid-label6");
    var sid6 = $("#sid6");


    var controllerSlot = 1;

    var eqTemplate = $('.eq-template');
    var eqAnchor = $('#eq-anchor');
    var myUUID = "";
    var currentPairingDevice = {};

    initBtn.button({text: true, label: "Initialize"});
    discoverBtn.button({text: true, label: "Discover"});
    listBtn.button({text: true, label: "List"});
    stopBtn.button({text: true, label: "Stop"});
    tExecBtn.button({text: true, label: "Execute"});
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
    var T_PS3_CPU_MEMORY_TIMEOUT_ID = 0;
    var T_CPU_MEMORY_USAGE_TIMEOUT_MSECS = 60000;  // 1 minute
    var tMinCpu=0, tCurrentCpu=0, tMaxCpu=0;
    var tMinMemory=0, tCurrentMemory=0, tMaxMemory=0;
    var numberMessagesReceivedFromController = 0;
    var bValidateMessagesRecv = false;

    mdx.app = {
        fixture: { selector: {}, targets: [] },

        clearTargetConsole: function() {
            $("#target-log > .log-entry").remove();
            tLogLine.text(0);
        },

		nrdStateListener: function(evt) {
			switch(evt.type) {
            case "init":
                if (evt.status == "READY") {
                    mdx.app.tlog("NRDP READY- "+ JSON.stringify(evt, null, 2));
                    nrdp.registration.addEventListener("activate", activateHandler);

                    var regPairDisabledElement = $("#t-regPairDisabled")[0];
                    var regPairEnabledElement = $("#t-regPairEnabled")[0];
                    var regPairDefaultPinElement = $("#t-regPairDefaultPin")[0];

                    if(regPairDisabledElement.checked) {
                        mdx.setRegistrationMode(mdx.REGISTRATION_DISABLED);
                    } else if(regPairEnabledElement.checked) {
                        mdx.setRegistrationMode(mdx.REGISTRATION_ENABLED);
                    } else if(regPairDefaultPinElement.checked) {
                        mdx.setRegistrationMode(mdx.REGISTRATION_DEFAULT_PIN);
                    }

                    var role = "TARGET";
			    	var commandMap = {};
			    	mdx.app.addMdxEvtListeners();

			        mdx.init(role, commandMap);;
				    mdx.app.tlog("MYUUID: "+ mdx.MYUUID);

                }
                break;
            default: break;
            }
		},
        stateDispatcher: function(evt) {

            switch(evt.type) {
            case "init":
                if (evt.status == "READY") {
                    mdx.app.tlog("INIT- "+ JSON.stringify(evt, null, 2));
			        try{
			            document.getElementById('MDXstatus').innerHTML = 'MDX is INITIALIZED';
			        }
			        catch(e) {}

                }
                break;
            case "initerror":
            	mdx.app.tlog("INTERIOR- "+ JSON.stringify(evt, null, 2));
            	break;
            default: break;
            }
        }
    };

    var clickDispatcher = function(event) {
        var id = $(event.currentTarget).attr('id');
        switch(id) {
        case "init":
            mdx.init("TARGET", commandMap );
            break;
        case "reveal":
            mdx.discovery.revealPresence();
            break;
        case "hide":
            mdx.discovery.hidePresence();
            break;
        case "t-console":
            mdx.app.clearTargetConsole();
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

		            selectedControllerSlot = $('.controller:checked').val();
                    var targetSession = "#sid"+selectedControllerSlot;
                    var targetSessionId = $(targetSession).text();
  					
    			    var payload = eval('('+args[1]+')');	
			        mdx.app.tlog("Send Message to sid =" + targetSessionId + " " + args[0] + " " + args[1] + " " +targetSessionId);	         		
			        var xid =  mdx.session.sendMessage(args[2], args[0], payload);
			
            	}
		        else if (cmd == "dePair"){

				    var targetSession = "#sid"+selectedControllerSlot;
                    var targetSessionId = $(targetSession).text();
                	mdx.app.tlog("Command: " + cmd + "(" + args.join(",") + "): " + mdx.pair[cmd].apply(mdx.app, args), "command");
			  	    var uuid = document.getElementById('paired-uuid'+selectedControllerSlot).innerHTML;

			    	_state.pairing.handle = -1;
			    	setDepaired(uuid);
			    	mdx.app.tlog("PAIRING DELETED");
			    	_state.sid = 0;
			    	eval("sidLabel"+selectedControllerSlot).removeClass('active');
				    eval("sidLabel"+selectedControllerSlot).addClass('inactive');
			        eval("sid"+selectedControllerSlot).text(_state.sid);
			        var eq = eqTemplate.clone();
			        eq.removeClass("eq-template");
			        eq.attr('sid', targetSessionId);
			        eq.find('.eq-sid').text(targetSessionId);
			        eqAnchor.append(eq);
			        eq.fadeIn(TRANSITION);

				    try{
					    document.getElementById('MDXstatus').innerHTML = 'MDX Not PAIRED';
				    }
				    catch(e) {}

            	}
		        else {
			        mdx.app.tlog("Command: " + cmd + "(" + args.join(",") + "): " + mdx.session[cmd].apply(mdx.app, args), "command");
	        	    _state.sid = 0;

			        eval("sidLabel"+selectedControllerSlot).addClass('inactive');
			        eval("sidLabel"+selectedControllerSlot).removeClass('active');
			        eval("sid"+selectedControllerSlot).text(_state.sid);

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
        case "t-is-PS3":
            if ($('#t-is-PS3:checked').length) {
                $(".t-cpu-usage-lbl").css("color", 'black');
                $(".t-memory-usage-lbl").css("color", 'black');
                $('.t-cpu-usage-val').attr('disabled', '');
                $('.t-memory-usage-val').attr('disabled', '');
                if(T_PS3_CPU_MEMORY_TIMEOUT_ID > 0) {
                    clearInterval(T_PS3_CPU_MEMORY_TIMEOUT_ID);
                    T_PS3_CPU_MEMORY_TIMEOUT_ID = 0;
                }
                getCpuMemoryUsage();
                T_PS3_CPU_MEMORY_TIMEOUT_ID = setInterval(getCpuMemoryUsage, T_CPU_MEMORY_USAGE_TIMEOUT_MSECS);
            }
            else {
                $(".t-cpu-usage-lbl").css("color", '#808080');
                $(".t-memory-usage-lbl").css("color", '#808080');
                $('.t-cpu-usage-val').attr('disabled', 'disabled');
                $('.t-memory-usage-val').attr('disabled', 'disabled');
                if(T_PS3_CPU_MEMORY_TIMEOUT_ID > 0) {
                    clearInterval(T_PS3_CPU_MEMORY_TIMEOUT_ID);
                    T_PS3_CPU_MEMORY_TIMEOUT_ID = 0;
                }
            }
            break;
        default:
        }
    }

    var getCpuMemoryUsage = function() {
        if(nrdp.ps3) {
            nrdp.ps3.getCPUUtilization( function(result)
                                        {
                                            tCurrentCpu = (Math.round(result*1000)/1000)*100;
                                            if(tCurrentCpu < tMinCpu || tMinCpu == 0) { tMinCpu = tCurrentCpu; }
                                            if(tCurrentCpu > tMaxCpu) { tMaxCpu = tCurrentCpu; }
                                            $("#t-min-cpu-usage-val").val(tMinCpu + '%');
                                            $("#t-max-cpu-usage-val").val(tMaxCpu + '%');
                                            $("#t-cur-cpu-usage-val").val(tCurrentCpu + '%');
                                        });
            nrdp.ps3.getAvailableMemory(function(result)
                                        {
                                            tCurrentMemory = (result/1000000)*1000/1000;
                                            nrdp.log.info("PS3-MEMORY-AVAILABLE: memory:" + result);
                                            if(tCurrentMemory < tMinMemory || tMinMemory == 0) { tMinMemory = tCurrentMemory; }
                                            if(tCurrentMemory > tMaxMemory) { tMaxMemory = tCurrentMemory; }
                                            $("#t-min-memory-usage-val").val(tMinMemory + 'MB');
                                            $("#t-max-memory-usage-val").val(tMaxMemory + 'MB');
                                            $("#t-cur-memory-usage-val").val(tCurrentMemory + 'MB');
                                        });
        }
        else {
            $("#t-min-cpu-usage-val").val("NA");
            $("#t-max-cpu-usage-val").val("NA");
            $("#t-cur-cpu-usage-val").val("NA");
            $("#t-min-memory-usage-val").val("NA");
            $("#t-max-memory-usage-val").val("NA");
            $("#t-cur-memory-usage-val").val("NA");
        }
    }

    var controllerClickDispatcher = function(event) {
		updateController();
    }

    var argClickDispatcher = function(event) {
        var evt = $(event.currentTarget);

        var value = evt.attr("value");

        setArgs(tparams, value);
    }

    var regPairModeClickDispatcher = function(event) {
        var registrationMode = $(event.currentTarget);
        console.log("Registration mode " + registrationMode.val() + " selected");
        mdx.setRegistrationMode(registrationMode.val());
    }

    var dePairIt = function(event) {
        var evt = $(event.currentTarget);

		var targetSession = "#sid"+selectedControllerSlot;
        var targetSessionId = $(targetSession).text();
 		var uuid = $("#depairHandle").val();

		_state.pairing.handle = -1;
		setDepaired(uuid);

		mdx.app.tlog("PAIRING DELETED");
		_state.sid = 0;

		eval("sidLabel"+selectedControllerSlot).removeClass('active');
		eval("sidLabel"+selectedControllerSlot).addClass('inactive');
		eval("sid"+selectedControllerSlot).text(_state.sid);

		var eq = eqTemplate.clone();
		eq.removeClass("eq-template");
		eq.attr('sid', targetSessionId);
		eq.find('.eq-sid').text(targetSessionId);
		eqAnchor.append(eq);
	    eq.fadeIn(TRANSITION);

		mdx.pair.dePair(uuid);

		var requestScreen = $("#pairingRequestScreen");
		requestScreen.hide();
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
        //$('.c-command').removeAttr('disabled');
        //tExecBtn.button("enable");

	    try{
		    document.getElementById('MDXstatus').innerHTML = 'MDX is PAIRED - Session Not Started';
	    }
	    catch(e) {}

    }

    var setDepaired = function(uuid) {
        _state.pairing.uuid = -1;
        _state.pairing.handle = -1;
        eval("handleLabel"+selectedControllerSlot).removeClass('active');
        eval("handleLabel"+selectedControllerSlot).addClass('inactive');
        eval("uuidLabel"+selectedControllerSlot).removeClass('active');
        eval("uuidLabel"+selectedControllerSlot).addClass('inactive');
	    var myuuid = '.' + uuid;
	    var selDevice = $(myuuid);
	    selDevice.find('.handle').text("");
        selDevice.removeClass('active');
	    eval("activeHandle"+selectedControllerSlot).text("INVALID_HANDLE");
        eval("pairedUuid"+selectedControllerSlot).text(-1);

	    try{
		    document.getElementById('MDXstatus').innerHTML = 'MDX is INITIALIZED';
	    }
	    catch(e) {}
    }

    var targetSessionStarted = function(evt) {
	    mdx.app.tlog("SESSION START- "+ JSON.stringify(evt, null, 2));
        _state.sid = evt.sid;
	

	    var sl1 = document.getElementById('active-handle1').innerHTML;
	    var sl2 = document.getElementById('active-handle2').innerHTML;
	    var sl3 = document.getElementById('active-handle3').innerHTML;
	    var sl4 = document.getElementById('active-handle4').innerHTML;
	    var sl5 = document.getElementById('active-handle5').innerHTML;
	    var sl6 = document.getElementById('active-handle6').innerHTML;

	    var sidController ="1";

        if(evt.pairingContext == sl1){
		    sidController =1;
	    }

	    else if(evt.pairingContext == sl2){
		    sidController =2;
	    }
 	    else if(evt.pairingContext == sl3){
		    sidController =3;
	    }
 	    else if(evt.pairingContext == sl4){
		    sidController =4;
	    }
 	    else if(evt.pairingContext == sl5){
		    sidController =5;
	    }
 	    else if(evt.pairingContext == sl6){
		    sidController =6;
	    }

	    else {
		    mdx.app.tlog("ERROR - Can't select session");
	    }

    	eval("sidLabel"+sidController).addClass('active');
        eval("sid"+sidController).text(_state.sid);

	    setArgs(tparams, "endSession");

	    tExecBtn.button("enable");
    	mdx.app.tlog("targetSessionStarted sessionId = " + evt.sid);

	    try{
		    document.getElementById('MDXstatus').innerHTML = 'MDX is PAIRED - Session Started';
	    }
	    catch(e) {}
	
    }


    var tEndSession = function(evt) {
	    mdx.app.tlog("SESSION END- "+ JSON.stringify(evt, null, 2));
	    mdx.app.tlog("end session Id = " + evt.sid);    	

	    try{
		    document.getElementById('MDXstatus').innerHTML = 'MDX is PAIRED - Session Not Started';
	    }
	    catch(e) {}

	    if ($("#sid1").text() == evt.sid){
		    sidController = 1;
	    }

 	    else if ($("#sid2").text() == evt.sid){
		    sidController = 2;
	    }

 	    else if ($("#sid3").text() == evt.sid){
		    sidController = 3;
	    }

 	    else if ($("#sid4").text() == evt.sid){
		    sidController = 4;
	    }

 	    else if ($("#sid5").text() == evt.sid){
		    sidController = 5;
	    }

 	    else if ($("#sid6").text() == evt.sid){
		    sidController = 6;
	    }

	    else {sidController =1;}

		eval("sidLabel"+sidController).removeClass('active');
       	eval("sid"+sidController).text('0');

	    var eq = eqTemplate.clone();
	    eq.removeClass("eq-template");
	    eq.attr('sid', '0');
	    eq.find('.eq-sid').text('0');
	    eqAnchor.append(eq);
	    eq.fadeIn(TRANSITION);


    }

    var pairingDeleted = function(evt) {
 	    mdx.app.tlog("PAIRING DELETED- "+ JSON.stringify(evt, null, 2));
    	_state.pairing.handle = -1;
        _state.pairing.uuid = -1;
        _state.pairing.handle = -1;
        _state.sid = 0;

	    var del = evt.uuid


	    if ($("#paired-uuid1").text() == evt.uuid){
		    sidController = 1;
	    }

 	    else if ($("#paired-uuid2").text() == evt.uuid){
		    sidController = 2;
	    }

 	    else if ($("#paired-uuid3").text() == evt.uuid){
		    sidController = 3;
	    }

 	    else if ($("#paired-uuid4").text() == evt.uuid){
		    sidController = 4;
	    }

 	    else if ($("#paired-uuid5").text() == evt.uuid){
		    sidController = 5;
	    }

 	    else if ($("#paired-uuid6").text() == evt.uuid){
		    sidController = 6;
	    }

        eval("activeHandle"+sidController).text("INVALID_HANDLE");
        eval("pairedUuid"+sidController).text(-1);

	    eval("sidLabel"+sidController).addClass('inactive');
	    eval("handleLabel"+sidController).addClass('inactive');
	    eval("uuidLabel"+sidController).addClass('inactive');

	    eval("sidLabel"+sidController).removeClass('active');
	    eval("handleLabel"+sidController).removeClass('active');
	    eval("uuidLabel"+sidController).removeClass('active');
       	eval("sid"+sidController).text(_state.sid);

        var eq = eqTemplate.clone();
        eq.removeClass("eq-template");
        eq.attr('sid', _state.sid);
        eq.find('.eq-sid').text(_state.sid);
        eqAnchor.append(eq);
        eq.fadeIn(TRANSITION);
    }

    var targetPairingRequestReceived = function(evt) {

	    tPRR = evt;
    	mdx.app.tlog("PAIRING REQUEST RECEIVED- "+ JSON.stringify(evt, null, 2));
	

 	    var requestScreen = $("#pairingRequestScreen");
	    requestScreen.show();
	
	    var pText = $("#pairText");
	    var pUUID = $("#pairUUID");
	    var pError = $("#pairError");

	    pText.html("Pairing Request received from controller " +evt.uuid+ ".  <br>Do you wish to pair?");

	    pUUID.val(evt.uuid);
	    pError.val('{"errormesg":"error","errorcode":"1"}');

	    try{
		    moveTo("pairIt");
	    }
	    catch(e) {}
    }

    var targetRegPairingRequestReceived = function(evt) {

	    tPRR = evt;
    	mdx.app.tlog("REGPAIRING REQUEST RECEIVED- "+ JSON.stringify(evt, null, 2));
	

 	    var requestScreen = $("#regPairRequestScreen");
	    requestScreen.show();
	
	    var pText = $("#regPairText");
	    var pUUID = $("#regPairUUID");
	    var pError = $("#regPairError");

	    pText.html("RegPair Request received from controller " +evt.uuid+ ".  <br>Do you wish to allow?");

	    pUUID.val(evt.uuid);
	    pError.val('{"errormesg":"error","errorcode":"1"}');

	    try {
		    moveTo("regPairIt");
	    }
	    catch(e) {}
    }

    var messageDelivered = function(evt) {
        if(bValidateMessagesRecv == false) {  // don't log for the MDX stress test
            mdx.app.tlog("MESSAGE DELIVERED- "+ JSON.stringify(evt, null, 2));
        }
    }
    var targetPairingGrant = function(evt) {
        var reject = mdx.pair.sendPairingReject,
        grant = mdx.pair.sendPairingGrant;

        if (evt.type === "regpairgrant") {
            reject = mdx.pair.sendRegPairReject;
            grant = mdx.pair.sendRegPairGrant;
        }
 	    mdx.app.tlog("TARGET PAIRING GRANT- "+ JSON.stringify(evt, null, 2));

        if (evt.controllerErrCode) {
    	    var errorObj = {};
    	    errorObj.errorcode = evt.controllerErrCode;
    	    errorObj.errormesg = evt.controllerErrMesg;
    	    reject(evt.controlleruuid, errorObj);
	        mdx.app.tlog('Pairing Reject Sent - Error Code: '+ evt.controllerErrCode);
    	    return;
        }

        if (evt.pairintContext == '') {
    	    var errorObj = {};
    	    errorObj.errorcode = evt.controllerErrCode;
    	    errorObj.errormesg = evt.controllerErrMesg;
    	    reject(evt.controlleruuid, errorObj);
	        mdx.app.tlog('Pairing Reject Sent - Pairing Context Empty');
    	    return;
        }

        if (evt.actionID != "0") {
    	    var errorObj = {};
    	    errorObj.errorcode = evt.controllerErrCode;
    	    errorObj.errormesg = evt.controllerErrMesg;
    	    reject(evt.controlleruuid, errorObj);
	        mdx.app.tlog('Pairing Reject Sent - ActionID recieved: '+ evt.actionID);
    	    return;
        }

        // set cookies (regpair only)
        var nid = "", secnid = "", cookies = evt.cookies;

        if (cookies && cookies[0] && cookies[1]) {
            var eq, semi;

            eq = cookies[0].indexOf('=') + 1;
            semi = cookies[0].indexOf(';');
            nid = cookies[0].substr(eq, semi - eq);
            eq = cookies[1].indexOf('=') + 1;
            semi = cookies[1].indexOf(';');
            secnid = cookies[1].substr(eq, semi - eq);

            nrdp.registration.activationTokens = { NetflixId: nid, SecureNetflixId: secnid };
        }

	    if (($("#active-handle1").text() == "INVALID_HANDLE") || ($("#paired-uuid1").text() == evt.controlleruuid)) {
		    controllerSlot = 1;
		    $('#c1').show();
	    }

	    else if (($("#active-handle2").text() == "INVALID_HANDLE") || ($("#paired-uuid2").text() == evt.controlleruuid)) {
		    controllerSlot = 2;
		    $('#c2').show();
	    }

	    else if (($("#active-handle3").text() == "INVALID_HANDLE") || ($("#paired-uuid3").text() == evt.controlleruuid)) {
		    controllerSlot = 3;
		    $('#c3').show();
	    }

	    else if (($("#active-handle4").text() == "INVALID_HANDLE") || ($("#paired-uuid4").text() == evt.controlleruuid)) {
		    controllerSlot = 4;
		    $('#c4').show();
	    }

	    else if (($("#active-handle5").text() == "INVALID_HANDLE") || ($("#paired-uuid5").text() == evt.controlleruuid)) {
		    controllerSlot = 5;
		    $('#c5').show();
	    }

	    else if (($("#active-handle6").text() == "INVALID_HANDLE") || ($("#paired-uuid6").text() == evt.controlleruuid)) {
		    controllerSlot = 6;
		    $('#c6').show();
	    }
	    else {
		    controllerSlot = 0;
		    mdx.app.tlog("MAX # of Targets Reached.");
	    }

	    if(controllerSlot != 0){

       		_state.pairing.handle = evt.pairingContext;
       		_state.pairing.uuid = evt.controlleruuid;
    		grant(evt.pairingContext);

    		eval("activeHandle"+controllerSlot).text(_state.pairing.handle);
        	$('.carg-handle').val(_state.pairing.handle);
        	eval("pairedUuid"+controllerSlot).text(_state.pairing.uuid);
        	eval("handleLabel"+controllerSlot).removeClass('inactive');
        	eval("handleLabel"+controllerSlot).addClass('active');
        	eval("uuidLabel"+controllerSlot).removeClass('inactive');
        	eval("uuidLabel"+controllerSlot).addClass('active');
        	//$('.t-command').removeAttr('disabled');
        	//tExecBtn.button("enable");
		    setArgs(tparams, "endSession");
	    }
    }

    var messageTypesRecv = [];
    var appControllerMessageCb = function(evt) {
        if(bValidateMessagesRecv == false) {  // don't log for the MDX stress test
	        mdx.app.tlog("CONTROLLER MESSAGE CALLBACK- "+ JSON.stringify(evt, null, 2));
        }

    	if (evt.type == "PLAYER_PLAY") {
            if(bValidateMessagesRecv == false) {  // don't log for the MDX stress test
                mdx.app.tlog("starting to play movie " + evt.msgObject.catalogId + " track: " +  evt.msgObject.trackId);
            }
            numberMessagesReceivedFromController += 1;
    	}
        else if (evt.type == "PLAYER_CURRENT_STATE"){
            mdx.app.tlog("Received current state" + evt.msgObject );
            numberMessagesReceivedFromController += 1;
    	}
        else if(evt.type == "START_MESSAGE_SEND_STRESS_TEST") {
    	    mdx.app.tlog("EVENT RECEIVED- "+ JSON.stringify(evt, null, 2));
            numberMessagesReceivedFromController = 0;
            bValidateMessagesRecv = true;
            $("#t-recv-messages-number").val(0);
            $("#t-matched-messages-number").val(0);
            $("#t-mismatched-messages-number").val(0);
            $("#t-expected-messages-number").val(evt.msgObject.numbermessages);
            messageTypesRecv = [];
            console.log("START STRESS TEST");
        }
        else if(evt.type == "END_MESSAGE_SEND_STRESS_TEST") {
    	    mdx.app.tlog("EVENT RECEIVED- "+ JSON.stringify(evt, null, 2));
            bValidateMessagesRecv = false;
            console.log("Message types received are: " + messageTypesRecv.join(" , "));
            messageTypesRecv = [];
            console.log("END STRESS TEST");
        }
        else
        {
            if(bValidateMessagesRecv == false) {  // don't log for the MDX stress test
                mdx.app.tlog("Received " + evt.type + "data " +  + evt.msgObject );
            }
            numberMessagesReceivedFromController += 1;
    	}
        $("#t-recv-messages-number").val(numberMessagesReceivedFromController);
        var matchedMessages = $("#t-matched-messages-number").val();
        var mismatchedMessages = $("#t-mismatched-messages-number").val();
        if(bValidateMessagesRecv == true && numberMessagesReceivedFromController > 0) {
            var mdxmsg_indx = (numberMessagesReceivedFromController-1) % MDX_C2T_MESSAGES.mdxmessages.length;
            messageTypesRecv.push(evt.type);
            if( evt.type != MDX_C2T_MESSAGES.mdxmessages[mdxmsg_indx].name ) {
                $("#t-mismatched-messages-number").val(parseInt(mismatchedMessages)+1);
                console.log("ERROR: Received message - name:" + evt.type + "; expecting:" + MDX_C2T_MESSAGES.mdxmessages[mdxmsg_indx].name);
            }
            else if(JSON.stringify(evt.msgObject) != JSON.stringify(MDX_C2T_MESSAGES.mdxmessages[mdxmsg_indx].object)) {
                $("#t-mismatched-messages-number").val(parseInt(mismatchedMessages)+1);
                console.log("ERROR: Received msgObject - name:" + JSON.stringify(evt.msgObject) + "; expecting:" + JSON.stringify(MDX_C2T_MESSAGES.mdxmessages[mdxmsg_indx].object));
            }
            else {
                $("#t-matched-messages-number").val(parseInt(matchedMessages)+1);
                //console.log("SUCCESS: Received message - name:" + evt.type + "; expecting:" + MDX_C2T_MESSAGES.mdxmessages[mdxmsg_indx].name);
            }
        }
    }

    var appTargetMessageCb = function(evt) {
        if(bValidateMessagesRecv == false) {  // don't log for the MDX stress test
            mdx.app.tlog("TARGET MESSAGE CALLBACK- "+ JSON.stringify(evt, null, 2));
        }
    }

    var activateHandler = function(evt) {
        logThisEvent(evt);
    }

    var logThisEvent = function(evt) {
    	mdx.app.tlog("EVENT RECEIVED- "+ JSON.stringify(evt, null, 2));
    }

    var startMessageSendStressTestCb = function(evt) {
    	mdx.app.tlog("EVENT RECEIVED- "+ JSON.stringify(evt, null, 2));
        numberMessagesReceivedFromController = 0;
        bValidateMessagesRecv = true;
        $("#t-recv-messages-number").val(0);
        console.log("START STRESS TEST");
    }
    var endMessageSendStressTestCb = function(evt) {
    	mdx.app.tlog("EVENT RECEIVED- "+ JSON.stringify(evt, null, 2));
        bValidateMessagesRecv = false;
        console.log("END STRESS TEST");
    }


    var setArgs = function(params, cmd) {

        prefix = "targ";
        anchor = targAnchor;
        $('.targ').remove();

        var args = params[cmd].labels;

        for (var i=0; i < args.length; i++) {
            var label = prefix + '-' + (i+1);
            var argLabel = $("<label>" + args[i] + "</label>");
            argLabel.attr("for", label);
            argLabel.addClass(prefix);
    	
	        selectedControllerSlot = $('.controller:checked').val();
	        var csid = "#sid"+selectedControllerSlot;
    	    var currentSid = $(csid).text();

	        if(label == "targ-1"){

		        if(cmd=="sendMessage"){
			        var argInput = $('<input id="' + label + '" class="arg-text" name="' + label + '" type="' + params[cmd].type[i] + '" value="PLAYER_CURRENT_STATE" />');
		        }
		        else {
			        var argInput = $('<input id="' + label + '" class="arg-text" name="' + label + '" type="' + params[cmd].type[i] + '" value="" />');
		        }
	        }

	        else if(label == "targ-2"){
		        var argInput = $('<input id="' + label + '" class="arg-text" name="' + label + '" type="text" value={"catalogId":"http://path","trackId":"1234"} />');
            }
	        else if(label == "targ-3"){
		        var argInput = $('<input id="' + label + '" class="arg-text" name="' + label + '" type="text" value="'+currentSid+'" />');
            }
	        else{
		        var argInput = $('<input id="' + label + '" class="arg-text" name="' + label + '" type="' + params[cmd].type[i] + '" value="" />');
	        }	

	        if (params[cmd].value[i]) argInput.val(params[cmd].value[i]);
            argInput.addClass(prefix + "-" + args[i]);


            if (args[i] == "handle") argInput.val(_state.pairing.handle);

	        if (args[i] == "sid") argInput.val(currentSid);


            argInput.addClass(prefix);
            anchor.append(argLabel);
            anchor.append(argInput);
            anchor.append($('<br class="' + prefix +'" />'));
        }
    }


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
	    mdx.discovery.addEventListener("devicefound", logThisEvent);
        mdx.discovery.addEventListener("devicebyebye", logThisEvent);
        mdx.pair.addEventListener("pairingrequestreceived", targetPairingRequestReceived);
        mdx.pair.addEventListener("regpairrequestreceived", targetRegPairingRequestReceived);
        mdx.pair.addEventListener("pairingdeleted", pairingDeleted);
        mdx.pair.addEventListener("pairinggrant", targetPairingGrant);
        mdx.pair.addEventListener("regpairgrant", targetPairingGrant);
        mdx.pair.addEventListener("pairingresponse", logThisEvent);
        mdx.session.addEventListener("sessionstart", targetSessionStarted);
        mdx.session.addEventListener("sessionended", tEndSession);
        mdx.session.addEventListener("messagedelivered", messageDelivered);
        mdx.session.addEventListener("startSessionResponse", logThisEvent);
	    mdx.session.addEventListener("messagingerror", logThisEvent);

        for (var i = 0; i < controllerMsgs.length; i++){
            mdx.session.addMessageListener(controllerMsgs[i], appControllerMessageCb);
        }
        for (var i = 0; i < targetMsgs.length; i++){
            mdx.session.addMessageListener(targetMsgs[i], appTargetMessageCb);
        }
        mdx.session.addMessageListener("START_MESSAGE_SEND_STRESS_TEST", appControllerMessageCb);
        mdx.session.addMessageListener("END_MESSAGE_SEND_STRESS_TEST", appControllerMessageCb);
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
    $('.controller[type="radio"]').bind('click', controllerClickDispatcher);
    $('.t-command').bind('click', argClickDispatcher);
    $('.t-regPairMode').bind('click', regPairModeClickDispatcher);
    $('#depairButton').bind('click', dePairIt);
    $('input.t-is-PS3[type="checkbox"]').bind('click', clickDispatcher);

    var logFromNative = new _LogHandler(mdx.app.tlog, "fromnative");
    var logToNative = new _LogHandler(mdx.app.tlog, "tonative");
    var logEvent = new _LogHandler(mdx.app.tlog, "event");

    setArgs(tparams, "endSession");
    setArgs(tparams, "dePair");
    //$('.t-command').attr('disabled', 'disabled');

    initialize = function() {

 	    var urlParams = location.search;

 	    if(urlParams.indexOf('config')!=-1){
		    document.getElementById("configParams").style.display="block";
 	    }

    	nrdp.addEventListener("init", mdx.app.nrdStateListener);
    	nrdp.init();
	    $('#c2').hide();
	    $('#c3').hide();
	    $('#c4').hide();
	    $('#c5').hide();
	    $('#c6').hide();

        clearControllerMessages();
    }

});

// file name : emul/json/casuidata.js
/* jslint sloppy:true, nomen:true, vars:true */

gOipfEmul.createCasUiData({
    "root" : {
        "uitype" : "menulist",
        "uidata" : {
            "attribute" : ["selectable", "subtitle", "title", "bottom"],
            "title" : "Viaccess Pocket dTV",
            "subtitle" : "ROOT MENU",
            "bottom" : "OK to select, EXIT to quit",
            "focusindex" : 0,
            "startindex" : 0,
            "listitem" : [{
                "index" : 0,
                "type" : "str",
                "stringlength" : 27,
                "string" : "VIACCESS POCKET INFORMATION"
            }, {
                "index" : 1,
                "type" : "str",
                "stringlength" : 13,
                "string" : "UPDATE MODULE"
            }, {
                "index" : 2,
                "type" : "str",
                "stringlength" : 15,
                "string" : "MODIFY PIN CODE"
            }, {
                "index" : 3,
                "type" : "str",
                "stringlength" : 22,
                "string" : "SERVICE OPERATORS LIST"
            }, {
                "index" : 4,
                "type" : "str",
                "stringlength" : 17,
                "string" : "LANGUAGE SETTINGS"
            }, {
                "index" : 5,
                "type" : "str",
                "stringlength" : 9,
                "string" : "LIST TEST"
            }, {
                "index" : 6,
                "type" : "str",
                "stringlength" : 9,
                "string" : "LIST TEST2"
            }, {
                "index" : 7,
                "type" : "str",
                "stringlength" : 9,
                "string" : "PIN TEST"
            }]
        },
        "castype" : "ci",
        "slotnumber" : 0
    },
    "quit" : {
        "uitype" : "undefine",
        "castype" : "ci",
        "slotnumber" : 0
    },
    "pin_success" : {
        "uitype" : "menulist",
        "uidata" : {
            "attribute" : ["title"],
            "title" : "Nice Pin..."
        },
        "castype" : "ci",
        "slotnumber" : 0
    },
    "pin_invalid" : {
        "uitype" : "menulist",
        "uidata" : {
            "attribute" : ["title"],
            "title" : "Invalid Pin!!"
        },
        "castype" : "ci",
        "slotnumber" : 0
    },
    "node_0" : {
        "uitype" : "menulist",
        "uidata" : {
            "attribute" : ["subtitle", "title"],
            "title" : "Viaccess Pocket dTV",
            "subtitle" : "Viaccess module informations",
            "focusindex" : 0,
            "startindex" : 0,
            "listitem" : [{
                "index" : 0,
                "type" : "str",
                "stringlength" : 14,
                "string" : "BootRam ID : 1"
            }, {
                "index" : 1,
                "type" : "str",
                "stringlength" : 18,
                "string" : "Target_ID 0x000001"
            }, {
                "index" : 2,
                "type" : "str",
                "stringlength" : 18,
                "string" : "System : L1.0 S4"
            }, {
                "index" : 3,
                "type" : "str",
                "stringlength" : 23,
                "string" : "Serial Number : 0 88064"
            }, {
                "index" : 4,
                "type" : "str",
                "stringlength" : 24,
                "string" : "Card UA : 3 264 744 290 "
            }, {
                "index" : 5,
                "type" : "str",
                "stringlength" : 9,
                "string" : "#SOID : 2"
            }]
        },
        "castype" : "ci",
        "slotnumber" : 0
    },
    "node_1" : {
        "uitype" : "menulist",
        "uidata" : {
            "attribute" : ["bottom", "subtitle", "title"],
            "title" : "Searching Update (S0) ...",
            "subtitle" : " ",
            "bottom" : " ",
            "focusindex" : 0,
            "startindex" : 0,
            "listitem" : [{
                "index" : 0,
                "type" : "str",
                "stringlength" : 1,
                "string" : " "
            }, {
                "index" : 1,
                "type" : "str",
                "stringlength" : 1,
                "string" : " "
            }]
        },
        "castype" : "ci",
        "slotnumber" : 0
    },
    "node_2" : {
        "uitype" : "inputdialog",
        "uidata" : {
            "attribute" : ["subtitle"],
            "timeoutsecond" : "0",
            "fontsize" : 0,
            "posx" : 0,
            "posy" : 0,
            "width" : 0,
            "height" : 0,
            "bgcolor" : 0,
            "fgcolor" : 0,
            "progress" : 0,
            "title" : "",
            "subtitle" : "Please enter your PIN code",
            "bottom" : "",
            "inputtype" : "number",
            "inputdata" : {
                "attribute" : ["hiddeninput"],
                "number" : 0,
                "answerlength" : 4,
                "answerstring" : "Plea"
            }
        },
        "castype" : "ci",
        "slotnumber" : 0
    },
    "node_3" : {
        "uitype" : "menulist",
        "uidata" : {
            "attribute" : ["selectable", "subtitle", "title"],
            "title" : "Viaccess Pocket dTV",
            "subtitle" : "Service operators list",
            "focusindex" : 0,
            "startindex" : 0,
            "listitem" : [{
                "index" : 0,
                "type" : "str",
                "stringlength" : 6,
                "string" : "SRGSSR"
            }]
        },
        "castype" : "ci",
        "slotnumber" : 0
    },
    "node_4" : {
        "uitype" : "menulist",
        "uidata" : {
            "attribute" : ["selectable", "subtitle", "title"],
            "title" : "Viaccess Pocket dTV",
            "subtitle" : "Set current language",
            "focusindex" : 0,
            "startindex" : 0,
            "listitem" : [{
                "index" : 0,
                "type" : "str",
                "stringlength" : 7,
                "string" : "English"
            }, {
                "index" : 1,
                "type" : "str",
                "stringlength" : 9,
                "string" : "Français"
            }]
        },
        "castype" : "ci",
        "slotnumber" : 0
    },
    "node_5" : {
        "uitype" : "menulist",
        "uidata" : {
            "attribute" : ["subtitle", "title", "bottom"],
            "title" : "list menu test",
            "subtitle" : "Set current language",
            "bottom" : "bottom test~~",
            "focusindex" : 0,
            "startindex" : 0,
            "listitem" : [{
                "index" : 0,
                "type" : "str",
                "stringlength" : 7,
                "string" : "English"
            }, {
                "index" : 1,
                "type" : "str",
                "stringlength" : 9,
                "string" : "1Français"
            }, {
                "index" : 2,
                "type" : "str",
                "stringlength" : 9,
                "string" : "2Français"
            }, {
                "index" : 3,
                "type" : "str",
                "stringlength" : 9,
                "string" : "3Français"
            }, {
                "index" : 4,
                "type" : "str",
                "stringlength" : 9,
                "string" : "4Français"
            }, {
                "index" : 5,
                "type" : "str",
                "stringlength" : 9,
                "string" : "5Français"
            }, {
                "index" : 6,
                "type" : "str",
                "stringlength" : 9,
                "string" : "6Français"
            }, {
                "index" : 7,
                "type" : "str",
                "stringlength" : 9,
                "string" : "7Français"
            }, {
                "index" : 8,
                "type" : "str",
                "stringlength" : 9,
                "string" : "8Français"
            }, {
                "index" : 9,
                "type" : "str",
                "stringlength" : 9,
                "string" : "9Français"
            }, {
                "index" : 10,
                "type" : "str",
                "stringlength" : 9,
                "string" : "10Français"
            }, {
                "index" : 11,
                "type" : "str",
                "stringlength" : 9,
                "string" : "11Français"
            }, {
                "index" : 128,
                "type" : "str",
                "stringlength" : 9,
                "string" : "12Français"
            }, {
                "index" : 13,
                "type" : "str",
                "stringlength" : 9,
                "string" : "13Français"
            }, {
                "index" : 14,
                "type" : "str",
                "stringlength" : 9,
                "string" : "14Français"
            }, {
                "index" : 15,
                "type" : "str",
                "stringlength" : 9,
                "string" : "15Français"
            }]
        },
        "castype" : "ci",
        "slotnumber" : 0
    },
    "node_6" : {
        "uitype" : "menulist",
        "uidata" : {
            "attribute" : ["subtitle", "title", "bottom"],
            "title" : "list menu test2",
            "subtitle" : "Set current language",
            "bottom" : "bottom test~~",
            "focusindex" : 0,
            "startindex" : 0,
            "listitem" : [{
                "index" : 0,
                "type" : "str",
                "stringlength" : 7,
                "string" : "English"
            }, {
                "index" : 1,
                "type" : "str",
                "stringlength" : 9,
                "string" : "1Français"
            }, {
                "index" : 2,
                "type" : "str",
                "stringlength" : 9,
                "string" : "2Français"
            }, {
                "index" : 3,
                "type" : "str",
                "stringlength" : 9,
                "string" : "3Français"
            }, {
                "index" : 4,
                "type" : "str",
                "stringlength" : 9,
                "string" : "4Français"
            }, {
                "index" : 5,
                "type" : "str",
                "stringlength" : 9,
                "string" : "5Français"
            }, {
                "index" : 6,
                "type" : "str",
                "stringlength" : 9,
                "string" : "6Français"
            }, {
                "index" : 7,
                "type" : "str",
                "stringlength" : 9,
                "string" : "7Français"
            }, {
                "index" : 8,
                "type" : "str",
                "stringlength" : 9,
                "string" : "8Français"
            }, {
                "index" : 9,
                "type" : "str",
                "stringlength" : 9,
                "string" : "9Français"
            }]
        },
        "castype" : "ci",
        "slotnumber" : 0
    },
    "node_7" : {
        "uitype" : "inputdialog",
        "uidata" : {
            "attribute" : ["subtitle"],
            "timeoutsecond" : "0",
            "fontsize" : 0,
            "posx" : 0,
            "posy" : 0,
            "width" : 0,
            "height" : 0,
            "bgcolor" : 0,
            "fgcolor" : 0,
            "progress" : 0,
            "title" : "",
            "subtitle" : "INPUT TEST! Please enter your PIN code",
            "bottom" : "",
            "inputtype" : "number",
            "inputdata" : {
                "attribute" : ["hiddeninput1"],
                "number" : 0,
                "answerlength" : 16,
                "answerstring" : "Plea"
            }
        },
        "castype" : "ci",
        "slotnumber" : 0
    },
    "native_response" : {
        "uitype" : "banner",
        "uidata" : {
            "attribute" : ["timeout"],
            "timeout" : 3,
            "string" : "Authentication%20succeeded.",
        },
        "castype" : "ci",
        "slotnumber" : 0
    },
    "native_dialog" : {
        "uitype" : "inputdialog",
        "uidata" : {
            "attribute" : ["subtitle", "title"],
            "timeoutsecond" : "0",
            "fontsize" : 0,
            "posx" : 0,
            "posy" : 0,
            "width" : 0,
            "height" : 0,
            "bgcolor" : 0,
            "fgcolor" : 0,
            "progress" : 0,
            "title" : "Slot%201%20%3A%20",
            "subtitle" : "CI%2B%20operation%20failed%20(Error%20%25d).%20(Error%20fffffff4)",
            "bottom" : "",
            "inputtype" : "button",
            "inputdata" : {
                "attribute" : [{
                    "1st" : "CLOSE"
                }]
            }
        },
        "castype" : "ci",
        "slotnumber" : 0
    },
    "native_dialog2" : {
        "uitype" : "inputdialog",
        "uidata" : {
            "attribute" : ["subtitle", "title"],
            "timeoutsecond" : "0",
            "fontsize" : 0,
            "posx" : 0,
            "posy" : 0,
            "width" : 0,
            "height" : 0,
            "bgcolor" : 0,
            "fgcolor" : 0,
            "progress" : 0,
            "title" : "Slot%201%20%3A%20",
            "subtitle" : "TEST2222222222",
            "bottom" : "",
            "inputtype" : "button",
            "inputdata" : {
                "attribute" : [{
                    "1st" : "CLOSE"
                }]
            }
        },
        "castype" : "ci",
        "slotnumber" : 0
    },
    "native_response_newstring" : {
        "uitype" : "banner",
        "uidata" : {
            "attribute" : ["timeout"],
            "timeoutsecond" : 7,
            "string" : ["Authentication%20succeeded. ", {
            	"id" : "LOC_CIPLUS_POPUP_TEXT8_ID",
            	"in" : [
            		999,
            		"testString",
            		777,
            		555,
            		"Authentication%20succeeded",
            		111
            	]
            }]
        },
        "castype" : "ci",
        "slotnumber" : 0
    },
    "native_confirm_newstring2" : {
        "uitype" : "inputdialog",
        "uidata" : {
            "attribute":["subtitle","title","timeout"],
			"timeoutsecond":5,
			"fontsize":0,
			"posx":0,
			"posy":0,
			"width":0,
			"height":0,
			"bgcolor":0,
			"fgcolor":0,
			"progress":0,
			"title":[{"id":"LOC_CIPLUS_SLOT_ID"}, "%201%20%3A%20APS"],
			"subtitle":[{"id":"LOC_CIPLUS_POPUP_TEXT4_ID"}],
			"inputtype":"button",
			"inputdata":{
				"attribute":[
					{
						"1st":[
							{"id":"LOC_CLOSE_ID"}
						]
					}
				]
			}
        },
        "castype" : "ci",
        "slotnumber" : 0
    },
    "native_pin" : {
        "uitype" : "inputdialog",
        "uidata" : {
            "attribute" : ["subtitle", "title"],
            "timeoutsecond" : "0",
            "fontsize" : 0,
            "posx" : 0,
            "posy" : 0,
            "width" : 0,
            "height" : 0,
            "bgcolor" : 0,
            "fgcolor" : 0,
            "progress" : 0,
            "title" : "Slot%201%20%3A%20HD%2B%20CI%20Plus%20Module",
            "subtitle" : "Authentication%20in%20progress...%20It%20may%20take%20a%20couple%20minutes.",
            "bottom" : "",
            "inputtype" : "number",
            "inputdata" : {
                "attribute" : ["hiddeninput"],
                "number" : 0,
                "answerlength" : 4,
                "answerstring" : ""
            }
        },
        "castype" : "ci",
        "slotnumber" : 0
    },
    "native_pin2" : {
        "uitype" : "inputdialog",
        "uidata" : {
            "attribute" : ["subtitle", "title"],
            "timeoutsecond" : 0,
            "fontsize" : 0,
            "posx" : 0,
            "posy" : 0,
            "width" : 0,
            "height" : 0,
            "bgcolor" : 0,
            "fgcolor" : 0,
            "progress" : 0,
            "title" : [{
                "id" : "LOC_CIPLUS_SLOT_ID"
            }, "%201%20%3A%20Application%20IPP"],
            "subtitle" : [{
                "id" : "LOC_CIPLUS_PIN_TEXT1_ID"
            }],
            "inputtype" : "number",
            "inputdata" : {
                "attribute" : ["hiddeninput"],
                "number" : 0,
                "answerlength" : 4,
                "answerstring" : ""
            }
        },
        "castype" : "ci",
        "slotnumber" : 0
    }
});

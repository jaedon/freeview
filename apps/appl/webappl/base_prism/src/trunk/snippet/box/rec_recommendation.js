/**
 * Created by deptno on 2014. 7. 12..
 */
define(["require", "exports"], function(require, exports) {
    return {
        fire_NotiFoundRecommendationProgramme: {
            fx: function () {
                var TheAtoZofTVCooking = "dvb://233a.4084.4440;c8d4;;76";
                hx.svc.EPG.findProgrammes({
                    clause: "programmeID == " + TheAtoZofTVCooking,
                    cb: function (aResult) {
                        var eventArray = aResult[0];
                        var rcProgramme;
                        if (eventArray) {
                            rcProgramme = eventArray[0];
                            if (rcProgramme) {
                                hx.svc.RECORDINGS.fireEvent('NotiFoundRecommendationProgramme', rcProgramme);
                            }
                        }
                    }
                });
            },
            desc: "fire NotiFoundRecommendationProgramme via hx.svc.RECORDINGS<br>"
                + "TheAtoZofTVCooking, \"dvb://233a.4084.4440;c8d4;;76\";\""
        }
    }
});

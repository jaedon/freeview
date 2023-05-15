/**
 * Created by deptno on 2014. 6. 27..
 */
console.log("ALTERNATIVE TEST");
console.log("state: number, recording: ScheduledRecording, error: number, conflicts: ScheduledRecordingCollection, recConflict: number, alternatives: ScheduledRecordingCollection, altevents: ProgrammeCollection, conflictGroupInfo: Collection");
console.log("state: ", state);
console.log("recording: ", recording);
console.log("error: ", error);
console.log("conflicts: ", conflicts);
console.log("alternatives: ", alternatives);
console.log("altevents: ", altevents);
var bConflicts = !!conflicts;
var bAlternatives = !!alternatives;
var bAltevents = !!altevents;
var i;
console.log("recording.name: " + recording.name);
if (bConflicts) {
    for (i = 0; i < conflicts.length; i ++) {
        console.log("conflict.item("+i+").name: " + conflicts.item(i).name);
    }
}
if (bAlternatives) {
    for (i = 0; i < alternatives.length; i ++) {
        console.log("alternatives.item("+i+").name: " + alternatives.item(i).name);
    }
}
if (bAltevents) {
    for (i = 0; i < altevents.length; i ++) {
        console.log("altevents.item(" + i + ").name: " + altevents.item(i).name);
    }
}



for (i = 0, lenAlt = alternatives.length; i < conflicts.length; i++) {
    for (j = 0; j < lenAlt; j++) {
        if (conflicts.item(i) === alternatives.item(j)) {
            console.log("find");
        }
    }
}

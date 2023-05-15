/**
 * Created by deptno on 2014. 6. 24..
 */
var ___makeScheduledItem = (function() {
    var count = 0;
    return function() {
        var dummy = 'dummy' + count++;
        var scheduledRecItem = new ScheduledRecording();
        scheduledRecItem.programmeID = dummy;
        scheduledRecItem.name = dummy;
        scheduledRecItem.startTime = parseInt(new Date().getTime() / 1000, 10);
        scheduledRecItem.duration = 300000;
        scheduledRecItem.factoryType = 0;
        scheduledRecItem.isSeries = false;
        scheduledRecItem.seriesID = dummy;
        scheduledRecItem.repeatDays = 0x7F;
        scheduledRecItem.channel = null;
        scheduledRecItem.repeatDays = 0;
        return scheduledRecItem;
    }
})();

var eCONFLICT = 13;
var eALTERNATIVE = 7;
var scheduledItem = ___makeScheduledItem();
var scheduledItemCollection = new ScheduledRecordingCollection();
var scheduledItemCollection2 = new ScheduledRecordingCollection();

scheduledItemCollection.push(___makeScheduledItem());
scheduledItemCollection2.push(___makeScheduledItem());

var args = [
    eCONFLICT,
    scheduledItem,
    eALTERNATIVE,
    scheduledItemCollection,
    1,
    scheduledItemCollection2,
    2,
    new Collection()
];

hx.stbObject.recordingScheduler._PVREvent.apply(hx.svc.RECORDINGS, args);

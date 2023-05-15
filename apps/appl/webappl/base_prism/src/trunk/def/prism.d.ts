/// <reference path="controls.d.ts" />
/// <reference path="oipf.humax.d.ts" />
/**
 * Created by bglee@humaxdigital.com on 2014. 4. 1..
 */

interface ifPrismDlgButtonInfo {
    text: string;
    additional_info?: any;
    fn: () => void;
}
interface ifPrismDlgButtonInfoAsync {
    text: string;
    fnAsync: (aIndex: number, aCallback: (aNextInfo) => void) => {};
}
declare var prism: {
    CDialog: any;
    CButtonDlg: any;
    CQueryPasswordDlg: any;
    clock : any;
    getTimeText : any;
    getDateText : any;
    drawers : any;
    image : string;
    setGlobalDialogApi: any;
    createGlobalDialog: any;
    isGlobalDialogOpened: any;
    drawStorageGraphRect: any;
    drawStorageGraphCircleType2: any;
    makeChannelNumberString: any;
    processRequest: any;
    focusInterrupt: any;
    log: any;
    CMusicViewItem: any;
    CVideoViewItem: any;
    CPhotoViewItem: any;
    createHubDlg: (aLayeredGroupControl) => any;
    createButtonDlg: (aLayeredGroupControl, aButtonDlgInfo?: any, aCallback?: any) => any;
    createMusicPlaybackDlg: (aLayeredGroupControl) => any;
    createVideoPlaybackDlg: (aLayeredGroupControl) => any;
    createPhotoPlaybackDlg: (aLayeredGroupControl) => any;
    createQueryPasswordDlg: (aLayeredGroupControl) => any;
    createReminderDlg: (aLayeredGroupControl, aReminderInfo: any, aCallback?: any) => any;
    createNotificationDlg: (aLayeredGroupControl, htmlText?: any, buttonDlgInfo?:any) => any;
    createLoginDlg: (aLayeredGroupControl) => any;
    createWaitingNotificationDlg: (aLayeredGroupControl) => any;
    createContextMenu: (aLayeredGroupControl, contextMenuInfo?, callback?) => any;
    createConfirmDlg: (layeredGroupControl, confirm, callback: Function) => any;
    createInfoDlg: (layeredGroupControl, info, callback) => any;

    createMusicPlayer: (item?) => any;
    createVideoPlayer: (item) => any;
    CTsrPlayerControl: any;
    createDialogWindow: (layeredGroupControl) => any;
    createHelpDlg: (layeredGroupControl, htmlText, callback) => any;
    CHubDlg: any;
    CActionViewItem: any;
    CRecentViewItem: any;
    CContentViewItem: any;
    CImageDlg: any;
    createImageDlg: (layeredGroupControl, htmlText?, callback?) => any;
    createSambaLoginDlg: (layeredGroupControl) => any;
    CMediaExtensionViewDlg: any;
    createMediaExtensionViewDlg: (layeredGroupControl, viewType, drawer, drawParam) => any;
    CPlaybackDlg: any;
    CNotificationDlg: any;
    createPasswordConfirmationDlg: (layeredGroupControl) => any;
    CPasswordConfirmationDlg: any;
    CProgrammeDetailDlg: any;
    createProgrammeDetailDlg: (layeredGroupControl) => any;

    createQueryTimeDlg: (layeredGroupControl) => any;
    createQueryNumberDlg: (layeredGroupControl, config) => any;
    createStorageRenameDlg: (layeredGroupControl) => any;
    CSelectionDlg: any;
    createProfileSelectionDlg: (layeredGroupControl) => any;
    createStorageSelectionDlg: (layeredGroupControl) => any;
    createStorageSelectionDlg2: (layeredGroupControl) => any;
    createUsbWizardSelectionDlg: (layeredGroupControl, info, callback) => any;
    CSettingDlg: any;
    createSettingDlg: (layeredGroupControl, settingInfo?, callback?) => any;
    createFullSettingDlg: (layeredGroupControl) => any;
    createSettingView: (layeredGroupControl) => any;
    createSyncDlg: (layeredControl) => any;
    createListDlg: (layeredGroupControl, listDlgInfo, callback) => any;
    createListComboDlg: (layeredGroupControl, listComboDlgInfo, callback) => any;
    CViewDlg: any;
    createViewDlg: (layeredGroupControl) => any;
    createCategoryControl: (aParam) => any;
    createRCTButtonDlg: (aLayeredGroupControl, aRctInfo: RctLinkInfoCollection, aCallback: (aRctInfo: RctLinkInfo, aIsReminder: boolean) => void, aFnClosed: () => void) => any;
    createTRDConflictButtonDlg: (aLayeredGroupControl, aTRDConflictInfo: ifPrismDlgButtonInfoAsync[], aCallback?) => any;

    createWatchinHDButtonDlg: (aRctInfo: any[], aIconInfo?: any) => any;
    createNCDButtonDlg: (aLayeredGroupControl, aNCDInfo: any, aCallback?) => any;
    createHDAlternativeButtonDlg: (aLayeredGroupControl , aButtonInfos: ifPrismDlgButtonInfo[]) => any;
    createExtendedEventLinkageDescriptorButtonDlg: (aLayeredGroupControl , aButtonInfos: ifPrismDlgButtonInfo[]) => any;

    CPage: any;
    createMediaPage: (aParent: JQuery, aContentType: number) => any;
    createRecordingsPage: (aParent: JQuery) => any;
    createMainPage: (aParent: JQuery) => any;
    createTvAppsPage: ($parent) => any;
    createTopPicksPage: (aParent: JQuery, aStartPath: string) => any;
    //createTvGuidePage: (aParent: JQuery) => any;
};

declare var tr: {
    (aString: string): string;
};

interface ifPageTvGuide {
}

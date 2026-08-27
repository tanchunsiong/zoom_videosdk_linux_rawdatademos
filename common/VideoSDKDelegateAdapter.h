#pragma once

#include "zoom_video_sdk_delegate_interface.h"

class VideoSDKDelegateAdapter : public ZOOMVIDEOSDK::IZoomVideoSDKDelegate
{
public:
    void onSessionJoin() override {}
    void onSessionLeave() override {}
    void onSessionLeave(ZOOMVIDEOSDK::ZoomVideoSDKSessionLeaveReason) override {}
    void onError(ZOOMVIDEOSDK::ZoomVideoSDKErrors, int) override {}
    void onUserJoin(ZOOMVIDEOSDK::IZoomVideoSDKUserHelper*, ZOOMVIDEOSDK::IVideoSDKVector<ZOOMVIDEOSDK::IZoomVideoSDKUser*>*) override {}
    void onUserLeave(ZOOMVIDEOSDK::IZoomVideoSDKUserHelper*, ZOOMVIDEOSDK::IVideoSDKVector<ZOOMVIDEOSDK::IZoomVideoSDKUser*>*) override {}
    void onUserVideoStatusChanged(ZOOMVIDEOSDK::IZoomVideoSDKVideoHelper*, ZOOMVIDEOSDK::IVideoSDKVector<ZOOMVIDEOSDK::IZoomVideoSDKUser*>*) override {}
    void onUserAudioStatusChanged(ZOOMVIDEOSDK::IZoomVideoSDKAudioHelper*, ZOOMVIDEOSDK::IVideoSDKVector<ZOOMVIDEOSDK::IZoomVideoSDKUser*>*) override {}
    void onUserShareStatusChanged(ZOOMVIDEOSDK::IZoomVideoSDKShareHelper*, ZOOMVIDEOSDK::IZoomVideoSDKUser*, ZOOMVIDEOSDK::IZoomVideoSDKShareAction*) override {}
    void onLiveStreamStatusChanged(ZOOMVIDEOSDK::IZoomVideoSDKLiveStreamHelper*, ZOOMVIDEOSDK::ZoomVideoSDKLiveStreamStatus) override {}
    void onChatNewMessageNotify(ZOOMVIDEOSDK::IZoomVideoSDKChatHelper*, ZOOMVIDEOSDK::IZoomVideoSDKChatMessage*) override {}
    void onUserHostChanged(ZOOMVIDEOSDK::IZoomVideoSDKUserHelper*, ZOOMVIDEOSDK::IZoomVideoSDKUser*) override {}
    void onUserManagerChanged(ZOOMVIDEOSDK::IZoomVideoSDKUser*) override {}
    void onUserNameChanged(ZOOMVIDEOSDK::IZoomVideoSDKUser*) override {}
    void onUserActiveAudioChanged(ZOOMVIDEOSDK::IZoomVideoSDKAudioHelper*, ZOOMVIDEOSDK::IVideoSDKVector<ZOOMVIDEOSDK::IZoomVideoSDKUser*>*) override {}
    void onSessionNeedPassword(ZOOMVIDEOSDK::IZoomVideoSDKPasswordHandler*) override {}
    void onSessionPasswordWrong(ZOOMVIDEOSDK::IZoomVideoSDKPasswordHandler*) override {}
    void onMixedAudioRawDataReceived(AudioRawData*) override {}
    void onOneWayAudioRawDataReceived(AudioRawData*, ZOOMVIDEOSDK::IZoomVideoSDKUser*) override {}
    void onSharedAudioRawDataReceived(AudioRawData*) override {}
    void onCommandChannelConnectResult(bool) override {}
    void onCommandReceived(ZOOMVIDEOSDK::IZoomVideoSDKUser*, const zchar_t*) override {}
    void onCloudRecordingStatus(ZOOMVIDEOSDK::RecordingStatus, ZOOMVIDEOSDK::IZoomVideoSDKRecordingConsentHandler*) override {}
    void onUserRecordingConsent(ZOOMVIDEOSDK::IZoomVideoSDKUser*) override {}
    void onHostAskUnmute() override {}
    void onInviteByPhoneStatus(ZOOMVIDEOSDK::PhoneStatus, ZOOMVIDEOSDK::PhoneFailedReason) override {}
    void onCalloutJoinSuccess(ZOOMVIDEOSDK::IZoomVideoSDKUser*, const zchar_t*) override {}
    void onMultiCameraStreamStatusChanged(ZOOMVIDEOSDK::ZoomVideoSDKMultiCameraStreamStatus, ZOOMVIDEOSDK::IZoomVideoSDKUser*, ZOOMVIDEOSDK::IZoomVideoSDKRawDataPipe*) override {}
    void onMicSpeakerVolumeChanged(unsigned int, unsigned int) override {}
    void onAudioDeviceStatusChanged(ZOOMVIDEOSDK::ZoomVideoSDKAudioDeviceType, ZOOMVIDEOSDK::ZoomVideoSDKAudioDeviceStatus) override {}
    void onTestMicStatusChanged(ZOOMVIDEOSDK::ZoomVideoSDK_TESTMIC_STATUS) override {}
    void onSelectedAudioDeviceChanged() override {}
    void onCameraListChanged() override {}
    void onLiveTranscriptionStatus(ZOOMVIDEOSDK::ZoomVideoSDKLiveTranscriptionStatus) override {}
    void onLiveTranscriptionMsgInfoReceived(ZOOMVIDEOSDK::ILiveTranscriptionMessageInfo*) override {}
    void onLiveTranscriptionMsgError(ZOOMVIDEOSDK::ILiveTranscriptionLanguage*, ZOOMVIDEOSDK::ILiveTranscriptionLanguage*) override {}
    void onOriginalLanguageMsgReceived(ZOOMVIDEOSDK::ILiveTranscriptionMessageInfo*) override {}
    void onSpokenLanguageChanged(ZOOMVIDEOSDK::ILiveTranscriptionLanguage*) override {}
    void onChatMsgDeleteNotification(ZOOMVIDEOSDK::IZoomVideoSDKChatHelper*, const zchar_t*, ZOOMVIDEOSDK::ZoomVideoSDKChatMessageDeleteType) override {}
    void onChatPrivilegeChanged(ZOOMVIDEOSDK::IZoomVideoSDKChatHelper*, ZOOMVIDEOSDK::ZoomVideoSDKChatPrivilegeType) override {}
    void onSendFileStatus(ZOOMVIDEOSDK::IZoomVideoSDKSendFile*, const ZOOMVIDEOSDK::FileTransferStatus&) override {}
    void onReceiveFileStatus(ZOOMVIDEOSDK::IZoomVideoSDKReceiveFile*, const ZOOMVIDEOSDK::FileTransferStatus&) override {}
    void onProxyDetectComplete() override {}
    void onProxySettingNotification(ZOOMVIDEOSDK::IZoomVideoSDKProxySettingHandler*) override {}
    void onSSLCertVerifiedFailNotification(ZOOMVIDEOSDK::IZoomVideoSDKSSLCertificateInfo*) override {}
    void onCameraControlRequestResult(ZOOMVIDEOSDK::IZoomVideoSDKUser*, bool) override {}
    void onCameraControlRequestReceived(ZOOMVIDEOSDK::IZoomVideoSDKUser*, ZOOMVIDEOSDK::ZoomVideoSDKCameraControlRequestType, ZOOMVIDEOSDK::IZoomVideoSDKCameraControlRequestHandler*) override {}
    void onUserVideoNetworkStatusChanged(ZOOMVIDEOSDK::ZoomVideoSDKNetworkStatus, ZOOMVIDEOSDK::IZoomVideoSDKUser*) override {}
    void onCallCRCDeviceStatusChanged(ZOOMVIDEOSDK::ZoomVideoSDKCRCCallStatus) override {}
    void onAnnotationHelperCleanUp(ZOOMVIDEOSDK::IZoomVideoSDKAnnotationHelper*) override {}
    void onAnnotationPrivilegeChange(ZOOMVIDEOSDK::IZoomVideoSDKUser*, ZOOMVIDEOSDK::IZoomVideoSDKShareAction*) override {}
    void onAnnotationHelperActived(void*) override {}
    void onAnnotationToolTypeChanged(ZOOMVIDEOSDK::IZoomVideoSDKAnnotationHelper*, void*, ZOOMVIDEOSDK::ZoomVideoSDKAnnotationToolType) override {}
    void onVideoAlphaChannelStatusChanged(bool) override {}
    void onShareContentChanged(ZOOMVIDEOSDK::IZoomVideoSDKShareHelper*, ZOOMVIDEOSDK::IZoomVideoSDKUser*, ZOOMVIDEOSDK::IZoomVideoSDKShareAction*) override {}
    void onFailedToStartShare(ZOOMVIDEOSDK::IZoomVideoSDKShareHelper*, ZOOMVIDEOSDK::IZoomVideoSDKUser*) override {}
    void onShareSettingChanged(ZOOMVIDEOSDK::ZoomVideoSDKShareSetting) override {}
    void onAudioLevelChanged(unsigned int, bool, ZOOMVIDEOSDK::IZoomVideoSDKUser*) override {}
    void onShareNetworkStatusChanged(ZOOMVIDEOSDK::ZoomVideoSDKNetworkStatus, bool) override {}
    void onUserNetworkStatusChanged(ZOOMVIDEOSDK::ZoomVideoSDKDataType, ZOOMVIDEOSDK::ZoomVideoSDKNetworkStatus, ZOOMVIDEOSDK::IZoomVideoSDKUser*) override {}
    void onUserOverallNetworkStatusChanged(ZOOMVIDEOSDK::ZoomVideoSDKNetworkStatus, ZOOMVIDEOSDK::IZoomVideoSDKUser*) override {}
    void onSpotlightVideoChanged(ZOOMVIDEOSDK::IZoomVideoSDKVideoHelper*, ZOOMVIDEOSDK::IVideoSDKVector<ZOOMVIDEOSDK::IZoomVideoSDKUser*>*) override {}
    void onBindIncomingLiveStreamResponse(bool, const zchar_t*) override {}
    void onUnbindIncomingLiveStreamResponse(bool, const zchar_t*) override {}
    void onIncomingLiveStreamStatusResponse(bool, ZOOMVIDEOSDK::IVideoSDKVector<ZOOMVIDEOSDK::IncomingLiveStreamStatus>*) override {}
    void onStartIncomingLiveStreamResponse(bool, const zchar_t*) override {}
    void onStopIncomingLiveStreamResponse(bool, const zchar_t*) override {}
    void onShareContentSizeChanged(ZOOMVIDEOSDK::IZoomVideoSDKShareHelper*, ZOOMVIDEOSDK::IZoomVideoSDKUser*, ZOOMVIDEOSDK::IZoomVideoSDKShareAction*) override {}
    void onUnsharingWindowsChanged(ZOOMVIDEOSDK::IVideoSDKVector<void*>*, ZOOMVIDEOSDK::IZoomVideoSDKShareHelper*, ZOOMVIDEOSDK::IZoomVideoSDKUser*, ZOOMVIDEOSDK::IZoomVideoSDKShareAction*) override {}
    void onSharingActiveMonitorChanged(ZOOMVIDEOSDK::IVideoSDKVector<void*>*, ZOOMVIDEOSDK::IZoomVideoSDKShareHelper*, ZOOMVIDEOSDK::IZoomVideoSDKUser*, ZOOMVIDEOSDK::IZoomVideoSDKShareAction*) override {}
    void onSubSessionStatusChanged(ZOOMVIDEOSDK::ZoomVideoSDKSubSessionStatus, ZOOMVIDEOSDK::IVideoSDKVector<ZOOMVIDEOSDK::ISubSessionKit*>*) override {}
    void onSubSessionManagerHandle(ZOOMVIDEOSDK::IZoomVideoSDKSubSessionManager*) override {}
    void onSubSessionParticipantHandle(ZOOMVIDEOSDK::IZoomVideoSDKSubSessionParticipant*) override {}
    void onSubSessionUsersUpdate(ZOOMVIDEOSDK::ISubSessionKit*) override {}
    void onBroadcastMessageFromMainSession(const zchar_t*, const zchar_t*) override {}
    void onSubSessionUserHelpRequest(ZOOMVIDEOSDK::ISubSessionUserHelpRequestHandler*) override {}
    void onSubSessionUserHelpRequestResult(ZOOMVIDEOSDK::ZoomVideoSDKUserHelpRequestResult) override {}
    void onStartBroadcastResponse(bool, const zchar_t*) override {}
    void onStopBroadcastResponse(bool) override {}
    void onGetBroadcastControlStatus(bool, ZOOMVIDEOSDK::ZoomVideoSDKBroadcastControlStatus) override {}
    void onStreamingJoinStatusChanged(ZOOMVIDEOSDK::ZoomVideoSDKStreamingJoinStatus) override {}
    void onCanvasSnapshotTaken(ZOOMVIDEOSDK::IZoomVideoSDKUser*, bool) override {}
    void onCanvasSnapshotIncompatible(ZOOMVIDEOSDK::IZoomVideoSDKUser*) override {}
};

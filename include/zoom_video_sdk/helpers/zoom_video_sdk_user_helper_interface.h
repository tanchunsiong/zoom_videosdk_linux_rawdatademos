/*!
* \file zoom_video_sdk_user_helper_interface.h
* \brief user helper
*
*/

#ifndef _ZOOM_VIDEO_SDK_USER_HELPER_INTERFACE_H_
#define _ZOOM_VIDEO_SDK_USER_HELPER_INTERFACE_H_
#include "zoom_video_sdk_def.h"
#include "zoom_video_sdk_vector_interface.h"

BEGIN_ZOOM_VIDEO_SDK_NAMESPACE
/*! \struct ZoomVideoSDKVideoStatus
	\brief Video status information on the user's video device (the detected compatible video camera device), and status (whether video is turned on or off).
*/
typedef struct
{
	bool isHasVideoDevice;	///<Determine if the user's device has a compatible camera.
	bool isOn;	///<Determine if the camera is turned on.
}ZoomVideoSDKVideoStatus;

typedef enum
{
	ZoomVideoSDKNetwork_None,
	ZoomVideoSDKNetwork_Bad,
	ZoomVideoSDKNetwork_Normal,
	ZoomVideoSDKNetwork_Good,
}ZoomVideoSDKNetworkStatus;

/*! \enum ZoomVideoSDKAudioType
	\brief Audio type: VOIP (Voice over IP), Telephony, or None.
*/
typedef enum {
	ZoomVideoSDKAudioType_VOIP,
	ZoomVideoSDKAudioType_TELEPHONY,
	ZoomVideoSDKAudioType_None,
}ZoomVideoSDKAudioType;

/*! \struct ZoomVideoSDKAudioStatus
	\brief Audio status information.
*/
typedef struct
{
	ZoomVideoSDKAudioType audioType;
	bool isMuted;
	bool isTalking;
}ZoomVideoSDKAudioStatus;

/*! \struct ZoomVideoSDKVideoStatisticInfo
	\brief Video statistic information.
*/
typedef struct
{
	int width;
	int height;
	int fps;
	int bpf;
	ZoomVideoSDKNetworkStatus videoNetworkStatus;
}ZoomVideoSDKVideoStatisticInfo;

/*! \struct ZoomVideoSDKShareStatisticInfo
	\brief Share statistic information.
*/
typedef struct
{
	int width;
	int height;
	int fps;
	int bpf;
}ZoomVideoSDKShareStatisticInfo;

typedef enum
{
	ZoomVideoSDKResolution_90P = 0,
	ZoomVideoSDKResolution_180P,
	ZoomVideoSDKResolution_360P,
	ZoomVideoSDKResolution_720P,
#if (defined _WIN32) || (defined __MACOS__) || (defined __LINUX__)
	ZoomVideoSDKResolution_1080P,
#endif
	ZoomVideoSDKResolution_NoUse = 100,
}ZoomVideoSDKResolution;

typedef enum 
{
	RawData_On,
	RawData_Off,
}RawDataStatus;

/// \brief Video/share raw data sink interface.
///
class IZoomVideoSDKRawDataPipeDelegate
{
public:
	/// \brief Call when subscribed data received.
	/// \param data_ Data object.
	virtual void onRawDataFrameReceived(YUVRawDataI420* data_) = 0;

	/// \brief Call when subscribed data status changed.
	/// \param status Current data status.
	virtual void onRawDataStatusChanged(RawDataStatus status) = 0;

	virtual ~IZoomVideoSDKRawDataPipeDelegate() {};
};

/// \brief Video/share raw data pipe interface.
///
class IZoomVideoSDKRawDataPipe
{
public:
	/// \brief Subscribe video/share.
	/// \param resolution Subscribe size.
	/// \param listener Callback sink object.
	/// \return If the function succeeds, the return value is ZoomVideoSDKErrors_Success.
	///Otherwise failed. To get extended error information, see \link ZoomVideoSDKErrors \endlink enum.
	virtual ZoomVideoSDKErrors subscribe(ZoomVideoSDKResolution resolution, IZoomVideoSDKRawDataPipeDelegate* listener) = 0;
	
	/// \brief Unsubscribe video/share.
	/// \param listener The callback sink object.
	/// \return If the function succeeds, the return value is ZoomVideoSDKErrors_Success.
	///Otherwise failed. To get extended error information, see \link ZoomVideoSDKErrors \endlink enum.
	virtual ZoomVideoSDKErrors unSubscribe(IZoomVideoSDKRawDataPipeDelegate* listener) = 0;

	/// \brief Subscribe to the computer audio shared by other user.
	/// \return If the function succeeds, the return value is ZoomVideoSDKErrors_Success.
	///Otherwise failed. To get extended error information, see \link ZoomVideoSDKErrors \endlink enum.
	/// \remark Please call this interface after the callback onUserShareStatusChanged is received and the share type is ZoomVideoSDKShareType_PureAudio.
	/// This function has been marked as deprecated, then it will be instead by "IZoomVideoSDKUser.SetUserVolume(1.0, true)". 
	virtual ZoomVideoSDKErrors subscribeToSharedComputerAudio() = 0;

	/// \brief Unsubscribe to the computer audio shared by other user.
	/// \return If the function succeeds, the return value is ZoomVideoSDKErrors_Success.
	///Otherwise failed. To get extended error information, see \link ZoomVideoSDKErrors \endlink enum.
	/// This function has been marked as deprecated, then it will be instead by "IZoomVideoSDKUser.SetUserVolume(0, true)".
	virtual ZoomVideoSDKErrors unsubscribeToSharedComputerAudio() = 0;

	/// \brief Get the raw data data type. 
	/// \return Share or Video data type, see \link ZoomVideoSDKRawDataType \endlink enum.
	virtual ZoomVideoSDKRawDataType getRawdataType() = 0;

	/// \brief Get video status.
	/// \return Video status of the user object.
	virtual ZoomVideoSDKVideoStatus getVideoStatus() = 0;

	/// \brief Get video device name.
	/// \return Video device name if the function succeeds, otherwise returns NULL.
	virtual const zchar_t* getVideoDeviceName() = 0;

	/// \brief Get share status.
	/// \return Share status of the user object.
	virtual ZoomVideoSDKShareStatus getShareStatus() = 0;

	/// \brief Get share type.
	/// \return Share type of the user object.
	virtual ZoomVideoSDKShareType getShareType() = 0;

	/// \brief Get video statistic information.
	/// \return video statistic information. For more information, see \link ZoomVideoSDKVideoStatisticInfo \endlink
	virtual ZoomVideoSDKVideoStatisticInfo getVideoStatisticInfo() = 0;
};


/// \brief Camera control interface.
///
class IZoomVideoSDKCameraControlRequestHandler
{
public:
	virtual ~IZoomVideoSDKCameraControlRequestHandler() {}

	/// \brief Approve the remote camera control request.
	/// \return If the function succeeds, the return value is ZoomVideoSDKErrors_Success.
	virtual ZoomVideoSDKErrors approve() = 0;

	/// \brief Decline the remote camera control request.
	/// \return If the function succeeds, the return value is ZoomVideoSDKErrors_Success.
	virtual ZoomVideoSDKErrors decline() = 0;
};

class IZoomVideoSDKRemoteCameraControlHelper
{
public:
	virtual ~IZoomVideoSDKRemoteCameraControlHelper() {}

	/// \brief Request to control remote camera.
	/// \return If the function succeeds, the return value is ZoomVideoSDKErrors_Success.
	virtual ZoomVideoSDKErrors requestControlRemoteCamera() = 0;

	/// \brief Give up control of the remote camera.
	/// \return If the function succeeds, the return value is ZoomVideoSDKErrors_Success.
	virtual ZoomVideoSDKErrors giveUpControlRemoteCamera() = 0;

	/// \brief Turn the camera to the left.
	/// \param range Rotation range,  10 <= range <= 100.
	/// \return If the function succeeds, the return value is ZoomVideoSDKErrors_Success.
	virtual ZoomVideoSDKErrors turnLeft(unsigned int range = 50) = 0;

	/// \brief Turn the camera to the right.
	/// \param range Rotation range,  10 <= range <= 100.
	/// \return If the function succeeds, the return value is ZoomVideoSDKErrors_Success.
	virtual ZoomVideoSDKErrors turnRight(unsigned int range = 50) = 0;

	/// \brief Turn the camera up.
	/// \param range Rotation range,  10 <= range <= 100.
	/// \return If the function succeeds, the return value is ZoomVideoSDKErrors_Success.
	virtual ZoomVideoSDKErrors turnUp(unsigned int range = 50) = 0;

	/// \brief Turn the camera down.
	/// \param range Rotation range,  10 <= range <= 100.
	/// \return If the function succeeds, the return value is ZoomVideoSDKErrors_Success.
	virtual ZoomVideoSDKErrors turnDown(unsigned int range = 50) = 0;

	/// \brief Zoom the camera in.
	/// \param range Zoom range,  10 <= range <= 100.
	/// \return If the function succeeds, the return value is ZoomVideoSDKErrors_Success.
	virtual ZoomVideoSDKErrors zoomIn(unsigned int range = 50) = 0;

	/// \brief Zoom the camera out.
	/// \param range Zoom range,  10 <= range <= 100.
	/// \return If the function succeeds, the return value is ZoomVideoSDKErrors_Success.
	virtual ZoomVideoSDKErrors zoomOut(unsigned int range = 50) = 0;
};

/**
 * @brief Enumerations of the type for live transcription status.
 */
typedef enum
{
	ZoomVideoSDKLiveTranscription_Status_Stop = 0,///<not start
	ZoomVideoSDKLiveTranscription_Status_Start = 1,  ///<start	
}ZoomVideoSDKLiveTranscriptionStatus;

/**
 * @brief Enumerations of the type for live transcription operation type.
 */
typedef enum
{
	ZoomVideoSDKLiveTranscription_OperationType_None = 0,
	ZoomVideoSDKLiveTranscription_OperationType_Add,
	ZoomVideoSDKLiveTranscription_OperationType_Update,
	ZoomVideoSDKLiveTranscription_OperationType_Delete,
	ZoomVideoSDKLiveTranscription_OperationType_Complete,	
	ZoomVideoSDKLiveTranscription_OperationType_NotSupported,
	ZoomVideoSDKLiveTranscription_OperationType_NoTranslation,
}ZoomVideoSDKLiveTranscriptionOperationType;

/// \brief live transcription language interface.
///
class ILiveTranscriptionLanguage
{
public:
	/// \brief Get id of the language.
	/// \return The id of the language.
	virtual int getLTTLanguageID() = 0;

	/// \brief Get name of the language.
	/// \return The name of the language.
	virtual const zchar_t* getLTTLanguageName() = 0;

	virtual ~ILiveTranscriptionLanguage() {};
};

/// \brief live transcription message interface.
///
class ILiveTranscriptionMessageInfo
{
public:
	/// \brief Get the message ID of the current message.
	/// \return If the function succeeds, the return value is the message ID of the current message.
	///Otherwise it fails, and the return value is the string of length zero(0)
	virtual const zchar_t* getMessageID() = 0;

	/// \brief Get the speaker's ID.
	/// \return The user object's speaker ID.
	virtual const zchar_t* getSpeakerID() = 0;

	/// \brief Get the speaker's name.
	/// \return The user object's speaker name.
	virtual const zchar_t* getSpeakerName() = 0;

	/// \brief Get the content of the current message.
	/// \return The current message's content.
	virtual const zchar_t* getMessageContent() = 0;

	/// \brief Get the timestamp of the current message.
	/// \return The current message's timestamp.
	virtual time_t getTimeStamp() = 0;

	/// \brief Get the type of the current message.
	/// \return The current message's type.
	virtual ZoomVideoSDKLiveTranscriptionOperationType getMessageType() = 0;

	virtual ~ILiveTranscriptionMessageInfo() {};
};

/// \brief live transcription helper interface.
class IZoomVideoSDKLiveTranscriptionHelper
{
public:
	/// \brief Query if the user can start live transcription.
	/// \return True if the user can start live transcription, otherwise false.
	virtual bool canStartLiveTranscription() = 0;

	/// \brief Get the current live transcription status.
	/// \param [out] status The current live transcription status. For more details, see \link ZoomVideoSDKLiveTranscriptionStatus \endlink.
	/// \return If the function succeeds, the return value is ZoomVideoSDKErrors_Success.
	///Otherwise failed. To get extended error information, see \link ZoomVideoSDKErrors \endlink enum.
	virtual ZoomVideoSDKErrors getLiveTranscriptionStatus(ZoomVideoSDKLiveTranscriptionStatus& status) = 0;

	/// \brief Start live transcription.
	/// \If the session allows multi-language transcription,all users can start live transcription.
	/// \return If the function succeeds, the return value is SDKErr_Success.
	/// Otherwise failed. To get extended error information, see \link ZoomVideoSDKErrors \endlink enum.
	virtual ZoomVideoSDKErrors startLiveTranscription() = 0;

	/// \brief Stop live transcription.
	/// \If the session allows multi-language transcription,all users can stop live transcription.
	/// \return If the function succeeds, the return value is SDKErr_Success.
	/// Otherwise failed. To get extended error information, see \link ZoomVideoSDKErrors \endlink enum.
	virtual ZoomVideoSDKErrors stopLiveTranscription() = 0;

	/// \brief Get the list of all available spoken languages in session.
	/// \return If the function succeeds, the return value is the list of the available spoken languages in a session.
	///Otherwise failed, the return value is NULL.	
	virtual IVideoSDKVector<ILiveTranscriptionLanguage*>* getAvailableSpokenLanguages() = 0;

	/// \brief Set the spoken language of the current user.
	/// \param languageID The spoken language ID.
	/// \return If the function succeeds, the return value is SDKErr_Success.
	///Otherwise failed. To get extended error information, see \link ZoomVideoSDKErrors \endlink enum.
	virtual ZoomVideoSDKErrors setSpokenLanguage(int languageID) = 0;

	/// \brief Get the spoken language of the current user.
	/// \return The spoken language of the current user.
	virtual ILiveTranscriptionLanguage* getSpokenLanguage() = 0;

	/// \brief Get the list of all available translation languages in a session.
	/// \return If the function succeeds, the return value is the list of all available translation languages in a session.
	///Otherwise failed, the return value is NULL.
	virtual IVideoSDKVector<ILiveTranscriptionLanguage*>* getAvailableTranslationLanguages() = 0;

	/// \brief Set the translation language of the current user.	
	/// \param languageID The translation language ID.
	/// If the language id is set to -1, live translation will be disabled.
	/// \return If the function succeeds, the return value is SDKErr_Success.
	///Otherwise failed. To get extended error information, see \link ZoomVideoSDKErrors \endlink enum.
	virtual ZoomVideoSDKErrors setTranslationLanguage(int languageID) = 0;

	/// \brief Get the translation language of the current user.
	/// \return The translation language of the current user.
	virtual ILiveTranscriptionLanguage* getTranslationLanguage() = 0;

	/// \brief Enable or disable to receive original and translated content.If you enable this feature,you must start live transcription.
	/// \param bEnable True to enable the temporal de-noise of video or false to disable it.
	/// \return If the function succeeds, the return value is ZoomVideoSDKErrors.
	/// Otherwise it fails. To get extended error information, see \link ZoomVideoSDKErrors \endlink enum.
	virtual ZoomVideoSDKErrors enableReceiveSpokenLanguageContent(bool bEnable) = 0;

	/// \brief Determine whether the feature to receive original and translated is available.
	/// \return True indicates that the feature to receive original and translated is available. Otherwise False.
	virtual bool isReceiveSpokenLanguageContentEnabled() = 0;

	/// \brief Determine whether the view history translation message is available.
	/// \return True indicates that the view history transcription message is available. Otherwise False.
	virtual bool isAllowViewHistoryTranslationMessageEnabled() = 0;

	/// \brief Get the list of all history translation messages in a session.
	/// \return If the function succeeds, the return value is a list of all history translation messages in a session.
	///Otherwise it fails, and the return value is NULL.
	virtual IVideoSDKVector<ILiveTranscriptionMessageInfo*>* getHistoryTranslationMessageList() = 0;
};

/// \brief User object interface.
///
class IZoomVideoSDKUser
{
public:
	virtual ~IZoomVideoSDKUser(){}
	
	/// \brief Get the user's custom id.
	/// \return The Custom id of the user object.
	virtual const zchar_t* getCustomIdentity() = 0;

	/// \brief Get the user's name.
	/// \return The name of the user object.
	virtual const zchar_t* getUserName() = 0;

	/// \brief Get the user's id.
	/// \return The user id of the user object.
	virtual const zchar_t* getUserID() = 0;

	/// \brief Get the user's video status.
	/// \return Video status of the user object.
	/// this interface be marked as deprecated, then it will be instead by IZoomVideoSDKRawDataPipe.getVideoStatus()
	virtual ZoomVideoSDKVideoStatus getVideoStatus() = 0;

	/// \brief Get the user's audio status.
	/// \return Audio status of the user object.
	virtual ZoomVideoSDKAudioStatus getAudioStatus() = 0;
	
	/// \brief Get the user's share status.
	/// \return Share status of the user object.
	/// this interface be marked as deprecated, then it will be instead by IZoomVideoSDKRawDataPipe.getShareStatus()
	virtual ZoomVideoSDKShareStatus getShareStatus() = 0;
	
	/// \brief Determine whether the user is the host.
	/// \return True indicates that the user is the host, otherwise false.
	virtual bool isHost() = 0;

	/// \brief Determine whether the user is the manager(coHost)
	/// \return True indicates that the user is the manager(coHost), otherwise false.
	virtual bool isManager() = 0;
	
	/// \brief Get the user's video statistic information.
	/// \return The video statistic information. For more information, see \link ZoomVideoSDKVideoStatisticInfo \endlink
	virtual ZoomVideoSDKVideoStatisticInfo getVideoStatisticInfo() = 0;

	/// \brief Get the user's share statistic information.
	/// \return The share statistic information. For more information, see \link ZoomVideoSDKShareStatisticInfo \endlink
	virtual ZoomVideoSDKShareStatisticInfo getShareStatisticInfo() = 0;
	
	/// \brief Get the user's video raw data pipe.
	/// \return The video pipe. For more information, see \link IZoomVideoSDKRawDataPipe \endlink
	virtual IZoomVideoSDKRawDataPipe* GetVideoPipe() = 0;

	/// \brief Get the user's share raw data pipe.
	/// \return The share pipe. For more information, see \link IZoomVideoSDKRawDataPipe \endlink
	virtual IZoomVideoSDKRawDataPipe* GetSharePipe() = 0;

	/// \brief Get the helper class instance to access the remote camera control.
	/// \return If the function succeeds, the return value is the remote camera control helper object.
	///Otherwise returns NULL. For more details, see \link IZoomVideoSDKRemoteCameraControlHelper \endlink.
	virtual IZoomVideoSDKRemoteCameraControlHelper* getRemoteCameraControlHelper() = 0;

	/// \brief Get the user's multi-camera stream list.
	/// \return A list of all streaming cameras pipe. For more information, see see \link IZoomVideoSDKRawDataPipe \endlink.
	virtual IVideoSDKVector<IZoomVideoSDKRawDataPipe*>* getMultiCameraStreamList() = 0;

	/// \brief Get the user's live transcription helper object.
	/// \return If the function succeeds, the return value is the live transcription helper object. Otherwise returns NULL. For more details, see \link IZoomVideoSDKLiveTranscriptionHelper \endlink.
	virtual IZoomVideoSDKLiveTranscriptionHelper* getLiveTranscriptionHelper() = 0;

	/// \brief Set the user's local volume. This does not affect how other participants hear the user.
	/// \param volume The value can be >= 0 and <=10. If volume is 0, you won't be able to hear the related audio.
	/// \param isShareAudio If true, sets the volume of shared audio(such as shared computer audio), otherwise sets the volume of microphone.
	/// \return If success return true, otherwise false.
	virtual bool setUserVolume(float volume, bool isShareAudio = false) = 0;

	/// \brief Get user volume.
	/// \param volume The user volume.
	/// \param isShareAudio If true, gets the volume of shared audio(such as shared computer audio), otherwise gets the volume of the microphone.
	/// \return If successful, return true, otherwise false.
	virtual bool getUserVolume(float& volume, bool isShareAudio = false) = 0;

	/// \brief Determine which audio you can set, shared audio or microphone.
	/// \param isShareAudio If true, checks whether you can set the volume of shared audio, otherwise you can set the volume of the microphone.
	/// \return If can return true, otherwise false.
	virtual bool canSetUserVolume(bool isShareAudio = false) = 0;

	/// \brief Used to determine whether I agree to individual video recording.
	/// \return If agreed return true, otherwise false.
	virtual bool hasIndividualRecordingConsent() = 0;
};

/// \brief User helper interface.
///
class IZoomVideoSDKUserHelper
{
public:
	/// \brief Change a specific user's name.
	/// \param name The new name of the user object.
	/// \param pUser User in the session
	/// \return True indicates that name change is success. Otherwise, this function returns false.
	virtual bool changeName(const zchar_t* name, IZoomVideoSDKUser* pUser) = 0;

	/// \brief Assign a user as the session host.
	/// \param pUser User in the session
	/// \return True indicates that the user is now the host. Otherwise, this function returns false.
	virtual bool makeHost(IZoomVideoSDKUser* pUser) = 0;

	/// \brief Assign a user as the session manager.
	/// \param pUser User in the session.
	/// \return True indicates that the user is now the manager. Otherwise, this function returns false.
	virtual bool makeManager(IZoomVideoSDKUser* pUser) = 0;

	/// \brief Revoke manager rights from a user.
	/// \param pUser User in the session.
	/// \return True indicates that the user no longer has manager rights. Otherwise, this function returns false.
	virtual bool revokeManager(IZoomVideoSDKUser* pUser) = 0;

	/// \brief Remove user from session.
	/// \param pUser User in the session.
	/// \return True indicates that remove user is success. Otherwise, this function returns false.
	virtual bool removeUser(IZoomVideoSDKUser* pUser) = 0;
};
END_ZOOM_VIDEO_SDK_NAMESPACE
#endif
/*!
* \file zoom_video_sdk_crc_helper_interface.h
* \brief crc helper interface.
*
*/

#ifndef _ZOOM_VIDEO_SDK_CALL_OUT_INTERFACE_H_
#define _ZOOM_VIDEO_SDK_CALL_OUT_INTERFACE_H_
#include "zoom_video_sdk_def.h"

BEGIN_ZOOM_VIDEO_SDK_NAMESPACE

/*! \enum ZoomVideoSDKCRCCallStatus
	\brief CRC call status.
	Here are more detailed structural descriptions.
*/
enum ZoomVideoSDKCRCCallStatus
{
	ZoomVideoSDKCRCCallStatus_Success = 0,	///<Call out successfully.
	ZoomVideoSDKCRCCallStatus_Ring,			///<Bell during the call.
	ZoomVideoSDKCRCCallStatus_Timeout,		///<Call timeout.
	ZoomVideoSDKCRCCallStatus_Busy,			///<Busy.
	ZoomVideoSDKCRCCallStatus_Decline,		///<Decline.
	ZoomVideoSDKCRCCallStatus_Failed		///<Call fails.
};

/*! \enum ZoomVideoSDKCRCProtocol
	\brief Call CRC protocol.
	Here are more detailed structural descriptions.
*/
enum ZoomVideoSDKCRCProtocol
{
	ZoomVideoSDKCRCProtocol_H323 = 0,	///<H.323 type.
	ZoomVideoSDKCRCProtocol_SIP			///<SIP type.
};

/// \brief CRC Helper Interface.
///
class IZoomVideoSDKCRCHelper
{
public:
	virtual ~IZoomVideoSDKCRCHelper() {}

	/// \brief Queries if the CRC feature is enabled.
	/// \return True means that the CRC is enabled, otherwise it's disabled.
	/// \remarks Only get the correct value after joining the session.
	virtual bool isCRCEnabled() = 0;

	/// \brief Calls the CRC device. Only available for the host/co-host.
	/// \param sAddress The CRC device's IP address.
	/// \param eProtocol The protocol of the CRC device.
	/// \return If the function succeeds, the return value is ZoomVideoSDKErrors_Success. Otherwise the function returns an error. For details, see \link ZoomVideoSDKErrors \endlink enum.
	/// \remarks If the function succeeds, the IZoomVideoSDKDelegate::onCallCRCDeviceStatusChanged(status) will be triggered once the call crc device status changes.
	virtual ZoomVideoSDKErrors callCRCDevice(const zchar_t* sAddress, ZoomVideoSDKCRCProtocol eProtocol) = 0;

	/// \brief Cancels the call to the CRC device. Only available for the host/co-host.
	/// \return If the function succeeds, the return value is ZoomVideoSDKErrors_Success. Otherwise the function returns an error. For details, see \link ZoomVideoSDKErrors \endlink enum.
	virtual ZoomVideoSDKErrors cancelCallCRCDevice() = 0;
};

END_ZOOM_VIDEO_SDK_NAMESPACE
#endif
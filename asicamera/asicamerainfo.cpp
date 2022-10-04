/*
    Copyright (C) 2020 by Pawel Soja <kernel32.pl@gmail.com>

    This library is free software; you can redistribute it and/or
    modify it under the terms of the GNU Lesser General Public
    License as published by the Free Software Foundation; either
    version 2.1 of the License, or (at your option) any later version.

    This library is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
    Lesser General Public License for more details.

    You should have received a copy of the GNU Lesser General Public
    License along with this library; if not, write to the Free Software
    Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA
*/
#include "asicamerainfo.h"
#include "asicamerainfo_p.h"

#include "ASICamera2.h"

#include <algorithm>

AsiCameraInfoPrivate::AsiCameraInfoPrivate()
{ }

AsiCameraInfoPrivate::~AsiCameraInfoPrivate()
{ }

AsiCameraInfo::AsiCameraInfo()
{ }

AsiCameraInfo::~AsiCameraInfo()
{ }

AsiCameraInfo::AsiCameraInfo(AsiCameraInfoPrivate &dd)
    : d_ptr(&dd)
{ }

bool AsiCameraInfo::isValid() const
{
    const AsiCameraInfoPrivate *d = d_func();
    return d != nullptr;
}

std::string AsiCameraInfo::name() const
{
    const AsiCameraInfoPrivate *d = d_func();
    return d != nullptr ? d->info.Name : "";
}

int AsiCameraInfo::cameraId() const
{
    const AsiCameraInfoPrivate *d = d_func();
    return d != nullptr ? d->info.CameraID : 0;
}

long AsiCameraInfo::maxHeight() const
{
    const AsiCameraInfoPrivate *d = d_func();
    return d != nullptr ? d->info.MaxHeight : 0;
}

long AsiCameraInfo::maxWidth() const
{
    const AsiCameraInfoPrivate *d = d_func();
    return d != nullptr ? d->info.MaxWidth : 0;
}

bool AsiCameraInfo::isColor() const
{
    const AsiCameraInfoPrivate *d = d_func();
    return d != nullptr ? d->info.IsColorCam == ASI_TRUE: false;
}

AsiCamera::BayerPattern AsiCameraInfo::bayerPattern() const
{
    const AsiCameraInfoPrivate *d = d_func();

    if (d == nullptr)
        return AsiCamera::BayerPatternUnknown;

    switch(d->info.BayerPattern)
    {
    case ASI_BAYER_RG: return AsiCamera::BayerPatternRG;
    case ASI_BAYER_BG: return AsiCamera::BayerPatternBG;
    case ASI_BAYER_GR: return AsiCamera::BayerPatternGR;
    case ASI_BAYER_GB: return AsiCamera::BayerPatternGB;
    default:           return AsiCamera::BayerPatternUnknown;
    }
}

std::string AsiCameraInfo::bayerPatternAsString() const
{
    switch(bayerPattern())
    {
    case AsiCamera::BayerPatternRG:      return "RG";
    case AsiCamera::BayerPatternBG:      return "BG";
    case AsiCamera::BayerPatternGR:      return "GR";
    case AsiCamera::BayerPatternGB:      return "GB";
    case AsiCamera::BayerPatternUnknown: return "Unknown";
    default:                             return "Unknown";
    } 
}

std::list<int> AsiCameraInfo::supportedBins() const
{
    const AsiCameraInfoPrivate *d = d_func();
    std::list<int> result;

    if (d == nullptr)
        return result;

    const int count =
        sizeof( d->info.SupportedBins) /
        sizeof(*d->info.SupportedBins);

    for(int j=0; j<count && d->info.SupportedBins[j] != 0; ++j)
        result.push_back(d->info.SupportedBins[j]);

    return result;
}

std::list<AsiCamera::ImageFormat> AsiCameraInfo::supportedVideoFormat() const
{
    const AsiCameraInfoPrivate *d = d_func();
    std::list<AsiCamera::ImageFormat> result;

    if (d == nullptr)
        return result;

    const int count =
        sizeof( d->info.SupportedVideoFormat) /
        sizeof(*d->info.SupportedVideoFormat);

    for(int j=0; j<count && d->info.SupportedVideoFormat[j] >= 0; ++j)
        switch(d->info.SupportedVideoFormat[j])
        {
        case ASI_IMG_RAW8:  result.push_back(AsiCamera::ImageFormatRaw8);    break;
        case ASI_IMG_RGB24: result.push_back(AsiCamera::ImageFormatRgb24);   break;
        case ASI_IMG_RAW16: result.push_back(AsiCamera::ImageFormatRaw16);   break;
        case ASI_IMG_Y8:    result.push_back(AsiCamera::ImageFormatY8);      break;
        default:            result.push_back(AsiCamera::ImageFormatUnknown); break;
        }

    return result;
}

std::list<std::string> AsiCameraInfo::supportedVideoFormatAsStringList() const
{
    std::list<std::string> result;
    for(auto &format: supportedVideoFormat())
        switch(format)
        {
        case AsiCamera::ImageFormatRaw8:    result.push_back("Raw8");    break;
        case AsiCamera::ImageFormatRgb24:   result.push_back("Rgb24");   break;
        case AsiCamera::ImageFormatRaw16:   result.push_back("Raw16");   break;
        case AsiCamera::ImageFormatY8:      result.push_back("Y8");      break;
        case AsiCamera::ImageFormatUnknown: result.push_back("Unknown"); break;
        default:                            result.push_back("Unknown"); break;
        }
    return result;
}

bool AsiCameraInfo::isSupportedVideoFormat(const AsiCamera::ImageFormat &videoFormat) const
{
    const auto svf = supportedVideoFormat();
    return std::find(svf.begin(), svf.end(), videoFormat) != svf.end();
}

bool AsiCameraInfo::isSupportedBins(int bins) const
{
    const auto sb = supportedBins();
    return std::find(sb.begin(), sb.end(), bins) != sb.end();
}

double AsiCameraInfo::pixelSize() const
{
    const AsiCameraInfoPrivate *d = d_func();
    return d != nullptr ? d->info.PixelSize : 0;
}

bool AsiCameraInfo::mechanicalShutter() const
{
    const AsiCameraInfoPrivate *d = d_func();
    return d != nullptr ? d->info.MechanicalShutter == ASI_TRUE : false;
}

bool AsiCameraInfo::st4Port() const
{
    const AsiCameraInfoPrivate *d = d_func();
    return d != nullptr ? d->info.ST4Port == ASI_TRUE : false;
}

bool AsiCameraInfo::isCooled() const
{
    const AsiCameraInfoPrivate *d = d_func();
    return d != nullptr ? d->info.IsCoolerCam == ASI_TRUE : false;
}

bool AsiCameraInfo::isUsb3Host() const
{
    const AsiCameraInfoPrivate *d = d_func();
    return d != nullptr ? d->info.IsUSB3Host == ASI_TRUE : false;
}

bool AsiCameraInfo::isUsb3Camera() const
{
    const AsiCameraInfoPrivate *d = d_func();
    return d != nullptr ? d->info.IsUSB3Camera == ASI_TRUE : false;
}

double AsiCameraInfo::elecPerADU() const
{
    const AsiCameraInfoPrivate *d = d_func();
    return d != nullptr ? d->info.ElecPerADU : 0;
}

int AsiCameraInfo::bitDepth() const
{
    const AsiCameraInfoPrivate *d = d_func();
    return d != nullptr ? d->info.BitDepth : 0;
}

bool AsiCameraInfo::isTriggerCamera() const
{
    const AsiCameraInfoPrivate *d = d_func();
    return d != nullptr ? d->info.IsTriggerCam == ASI_TRUE : false;
}

std::list<AsiCameraInfo> AsiCameraInfo::availableCameras()
{
    std::list<AsiCameraInfo> cameras;

    int numofConnectCameras = ASIGetNumOfConnectedCameras();
    
    for(int i=0; i<numofConnectCameras; ++i)
    {
        AsiCameraInfoPrivate *priv = new AsiCameraInfoPrivate;
        ASIGetCameraProperty(&priv->info, i);
        cameras.push_back(*priv);
    }
    
    return cameras;
}

AsiCameraInfoPrivate * AsiCameraInfo::d_func() const
{
    return static_cast<AsiCameraInfoPrivate*>(d_ptr.get());
}

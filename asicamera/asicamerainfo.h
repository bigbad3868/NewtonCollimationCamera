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
#pragma once

#include <string>
#include <memory>
#include <list>

#include "asicamera.h"

class AsiCamera;
class AsiCameraInfoPrivate;
class AsiCameraInfo
{
public:
    AsiCameraInfo();
    virtual ~AsiCameraInfo();

public:
    bool isValid() const;

    std::string name() const;
    int  cameraId() const;
    long maxHeight() const;
    long maxWidth() const;
    bool isColor() const;
    AsiCamera::BayerPattern bayerPattern() const;
    std::string bayerPatternAsString() const;
    std::list<int> supportedBins() const;
    std::list<AsiCamera::ImageFormat> supportedVideoFormat() const;
    std::list<std::string> supportedVideoFormatAsStringList() const;

    bool isSupportedVideoFormat(const AsiCamera::ImageFormat &videoFormat) const;
    bool isSupportedBins(int bins) const;

    double pixelSize() const;
    bool mechanicalShutter() const;
    bool st4Port() const;
    bool isCooled() const;
    bool isUsb3Host() const;
    bool isUsb3Camera() const;
    double elecPerADU() const;
    int bitDepth() const;
    bool isTriggerCamera() const;

public:
    static std::list<AsiCameraInfo> availableCameras();

protected:
    std::shared_ptr<AsiCameraInfoPrivate> d_ptr;

private:
    AsiCameraInfo(AsiCameraInfoPrivate &dd);

    AsiCameraInfoPrivate * d_func() const;
};

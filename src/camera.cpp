#include "camera.h"
#include <QThreadPool>
#include <vector>
#include <QDebug>
#include <QPixmap>
#include <algorithm>

Camera::Camera(QObject *parent) : CameraInterface(parent)
{
    return;
    auto availableCameras = AsiCameraInfo::availableCameras();
    if (availableCameras.size() == 0)
    {
        qWarning("No camera found!");
        return;
    }

}

Camera::~Camera()
{
    Camera::stop();
    mAsiCamera.close();
 }

bool Camera::start()
{
    if (!CameraInterface::start())
        return false;

    mAsiCamera.startVideoCapture();

    mGetVideoDataWorker.start([this](){
        while (isRunning())
        {
            if (mAsiCamera.getVideoData(mFrameBuffer->data(), mFrameBuffer->size()) == false)
            {
                stop();
                return;
            }

            emit this->onFrame(mFrameImage);
        }
    });

    return true;
}

void Camera::stop()
{
    CameraInterface::stop();
    mAsiCamera.stopVideoCapture();
    mGetVideoDataWorker.waitForDone(1000);
}

static QString uniqueName(const AsiCameraInfo &cameraInfo)
{
    return QString("%1. %2").arg(cameraInfo.cameraId()).arg(cameraInfo.name().c_str());
}

bool Camera::setCamera(const QString &name)
{
    auto availableCameras = AsiCameraInfo::availableCameras();

    AsiCameraInfo selectedCamera;

    for (const auto &it: AsiCameraInfo::availableCameras())
    {
        if (name == uniqueName(it))
        {
            setCamera(it);
            return true;
        }
    }

    qWarning("No camera found!");
    return false;
}

using std::to_string;
static std::string to_string(const std::string &str)
{
    return str;
}

template <typename T>
static std::string joinAsString(const T &arg, const std::string &separator = ", ")
{
    using element_type_t = std::remove_reference_t<decltype(*std::begin(std::declval<T&>()))>;

    return std::begin(arg) == std::end(arg) ? "" : std::accumulate(
        std::next(std::begin(arg)),
        std::end(arg),
        to_string(*std::begin(arg)),
        [&](std::string a, element_type_t b) { return std::move(a) + separator + to_string(b); }
    );
}

bool Camera::setCamera(const AsiCameraInfo &cameraInfo)
{
    stop();
    mAsiCamera.close();

    mWidth = cameraInfo.maxWidth();
    mHeight = cameraInfo.maxHeight();
    // const auto bitDepth = cameraInfo.bitDepth();
    // const auto byteDepth = (bitDepth + 7) / 8;

    mAsiCamera.setCamera(cameraInfo);
    mAsiCamera.open();

    if (cameraInfo.isSupportedVideoFormat(AsiCamera::ImageFormatRgb24))
    {
        mAsiCamera.setImageFormat(AsiCamera::ImageFormat::ImageFormatRgb24);
        mFormat = QImage::Format::Format_BGR888;
        mFrameBuffer.reset(new std::vector<unsigned char>(mWidth * mHeight * 3));
    }
    else
    {
        mAsiCamera.setImageFormat(AsiCamera::ImageFormat::ImageFormatY8);
        mFormat = QImage::Format::Format_Grayscale8;
        mFrameBuffer.reset(new std::vector<unsigned char>(mWidth * mHeight * 1));
    }

    mFrameImage = QImage(mFrameBuffer->data(), mWidth, mHeight, mFormat, [](void *arg){
        delete static_cast<QSharedPointer<std::vector<unsigned char>>*>(arg);
    }, new QSharedPointer<std::vector<unsigned char>>(mFrameBuffer));

    CameraInterface::setGain(mAsiCamera.control("Gain"));
    CameraInterface::setExposure(mAsiCamera.control("Exposure"));

#if 1
    {
        printf("Camera Information\n");
        printf("|-----------------------------------------------------------|\n");
        printf("|        Parameter       |     Value                        |\n");
        printf("|-----------------------------------------------------------|\n");
        printf("| Name                   | %-32s |\n", cameraInfo.name().data());
        printf("| Camera Id              | %-32d |\n", cameraInfo.cameraId());
        printf("| Max Height             | %-32ld |\n", cameraInfo.maxHeight());
        printf("| Max Width              | %-32ld |\n", cameraInfo.maxWidth());
        printf("| Is Color               | %-32s |\n", cameraInfo.isColor() ? "yes": "no");
        printf("| Bayer Pattern          | %-32s |\n", cameraInfo.bayerPatternAsString().data());
        printf("| Supported Bins         | %-32s |\n", joinAsString(cameraInfo.supportedBins()).data());
        printf("| Supported Video Format | %-32s |\n", joinAsString(cameraInfo.supportedVideoFormatAsStringList()).data());
        printf("| Pixel Size             | %-32g |\n", cameraInfo.pixelSize());
        printf("| Mechanical Shutter     | %-32s |\n", cameraInfo.mechanicalShutter() ? "yes" : "no");
        printf("| ST4 Port               | %-32s |\n", cameraInfo.st4Port() ? "yes" : "no");
        printf("| Is Cooled Camera       | %-32s |\n", cameraInfo.isCooled() ? "yes" : "no");
        printf("| Is USB3 Host           | %-32s |\n", cameraInfo.isUsb3Host() ? "yes" : "no");
        printf("| Is USB3 Camera         | %-32s |\n", cameraInfo.isUsb3Camera() ? "yes" : "no");
        printf("| Elec Per Adu           | %-32g |\n", cameraInfo.elecPerADU());
        printf("| Bit Depth              | %-32d |\n", cameraInfo.bitDepth());
        printf("| Is Trigger Camera      | %-32s |\n", cameraInfo.isTriggerCamera() ? "yes" : "no");
        printf("|-----------------------------------------------------------|\n\n");
    }

    printf("Camera Options\n");
    printf("|------------------------------------------------------------------------------------------------------------------------------|\n");
    printf("|        Option  Name      |  Value |   Min  |     Max    | Default  | Auto |                Description                       |\n");
    printf("|------------------------------------------------------------------------------------------------------------------------------|\n");
    for(auto &option: mAsiCamera.controls())
    {
        bool isAutoControl;
        long value = option.get(&isAutoControl);
        printf(
            "| %-24s | %-6ld | %-6ld | %-10ld | %-8ld | %-4s | %-48s |\n",
            option.name().data(),
            value,
            option.min(),
            option.max(),
            option.defaultValue(),
            isAutoControl ? "yes": "no",
            option.description().data()
        );
    }
    printf("|------------------------------------------------------------------------------------------------------------------------------|\n");
#endif
    start();

    return true;
}

QStringList Camera::availableCameras()
{
    QStringList result;
    for (const auto &it: AsiCameraInfo::availableCameras())
    {
        result.append(uniqueName(it));
    }
    return result;
}

void Camera::setExposure(int value)
{
    mAsiCamera.control("Exposure").set(value);
    CameraInterface::setExposure(value);
}

void Camera::setGain(int value)
{
    mAsiCamera.control("Gain").set(value);
    CameraInterface::setGain(value);
}

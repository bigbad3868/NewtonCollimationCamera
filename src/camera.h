#ifndef CAMERA_H
#define CAMERA_H

#include <QObject>
#include "asicamera.h"
#include "asicamerainfo.h"
#include <atomic>
#include <vector>


#include "camerainterface.h"

#include <QThreadPool>
#include <vector>
#include <QSharedPointer>

class Camera : public CameraInterface
{
    Q_OBJECT
public:
    explicit Camera(QObject *parent = nullptr);
    ~Camera();

public:
    bool start() override;
    void stop() override;

public:
    QStringList availableCameras() override;

public:
    bool setCamera(const QString &name) override;
    bool setCamera(const AsiCameraInfo &cameraInfo);

public:
    void setExposure(int value) override;
    void setGain(int value) override;

protected:
    QThreadPool mGetVideoDataWorker;
    QSharedPointer<std::vector<unsigned char>> mFrameBuffer;
    QImage mFrameImage;

    AsiCamera mAsiCamera;
};

#endif // CAMERA_H

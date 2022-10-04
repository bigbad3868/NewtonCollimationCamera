#ifndef CAMERAINTERFACE_H
#define CAMERAINTERFACE_H

#include <QObject>
#include <QImage>

class CameraInterface : public QObject
{
    Q_OBJECT
public:
    explicit CameraInterface(QObject *parent = nullptr);

public:
    virtual bool start();
    virtual void stop();

public:
    virtual QStringList availableCameras() = 0;

public:
    virtual bool setCamera(const QString &name) = 0;

public:
    virtual void setExposure(int value);
    virtual void setGain(int value);

public:
    int exposure();
    int gain();

public:
    bool isRunning() const { return mRunning; }

signals:
    void exposureChanged(int exposure);
    void gainChanged(int gain);

signals:
    void onFrame(QImage image);

protected:
    std::atomic<bool> mRunning {false};
    int mExposure {0};
    int mGain {0};
    int mWidth {0};
    int mHeight {0};
    QImage::Format mFormat { QImage::Format::NImageFormats };
};

#endif // CAMERAINTERFACE_H

#include "camerainterface.h"

CameraInterface::CameraInterface(QObject *parent) : QObject(parent)
{

}

bool CameraInterface::start()
{
    return mRunning.exchange(true) == false;
}

void CameraInterface::stop()
{
    mRunning = false;
}

void CameraInterface::setExposure(int value)
{
    if (mExposure != value)
    {
        emit exposureChanged(mExposure = value);
    }
}

void CameraInterface::setGain(int value)
{
    if (mGain != value)
    {
        emit gainChanged(mGain = value);
    }
}

int CameraInterface::exposure()
{
    return mExposure;
}

int CameraInterface::gain()
{
    return mGain;
}

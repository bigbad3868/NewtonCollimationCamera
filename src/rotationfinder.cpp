#include "rotationfinder.h"

#include <opencv2/opencv.hpp>
#include <QTimer>
#include <QDebug>

using namespace cv;

static cv::Mat image2mat(const QImage &src)
{
    QImage img = src.convertToFormat(QImage::Format_Grayscale8);
    return cv::Mat(img.height(), img.width(), CV_8UC(img.depth()/8), (uchar*)img.bits(), img.bytesPerLine()).clone();
}

class RotationMatrix: public cv::Mat
{
public:
    RotationMatrix()
        : cv::Mat(3, 3, CV_32F)
    { }

    RotationMatrix(const cv::Mat &other)
        : cv::Mat(other)
    { }

public:
    cv::Point2f centerOfRotation() const
    {
        const Mat &warp = *this;
        Mat translationMatrix = warp * (cv::Mat_<float>(3, 1) << 0, 0, 1);
        Mat rotationMatrix    = Mat::eye(2, 2, CV_32F) - warp * Mat::eye(3, 2, CV_32F);
        Mat retval = rotationMatrix.inv() * translationMatrix;
        return cv::Point2f(retval.at<float>(0), retval.at<float>(1));
    }

    QPoint centerOfRotationAsQPoint() const
    {
        cv::Point2f retval = centerOfRotation();
        return QPoint(retval.x, retval.y);
    }

    double rotationDegree() const
    {
        const Mat &warp = *this;
        return std::atan2(warp.at<float>(1,0), warp.at<float>(0,0)) * 180 / CV_PI;
    }

    void setScale(double scale)
    {
        const Mat &warp = *this;
        warp *= (cv::Mat_<float>(3, 3) << 1, 0, 0, 0, 1, 0, 0, 0, scale);
    }
};

static double findCenterOfRotation(const QImage &img1, const QImage &img2, RotationMatrix *rotationMatrix)
{
    double scale = 0.2; // to speed up calc
    scale = 1280.0 / img1.width() / 2;

    Mat m1 = image2mat(img1);
    Mat m2 = image2mat(img2);


    cv::resize(m1, m1, cv::Size(), scale, scale);
    cv::resize(m2, m2, cv::Size(), scale, scale);

    Mat warp = Mat::eye(2, 3, CV_32F);
    try
    {
        double cc = cv::findTransformECC(
                    m2,
                    m1,
                    warp,
                    MOTION_EUCLIDEAN,
                    {TermCriteria::COUNT + TermCriteria::EPS, 200, 1e-6}
        );

        RotationMatrix rm = warp;
        rm.setScale(1. / scale);
        *rotationMatrix = rm;
        return cc;

    }
    catch(const cv::Exception& e )
    {
        qWarning() << "exception caught:" << e.what();
    }
    return 0;
}

RotationFinder::RotationFinder(QObject *parent) : QObject(parent)
{ }

void RotationFinder::reset()
{
    resetPending = true;
    done = false;
}

void RotationFinder::stop()
{
    done = true;
}

void RotationFinder::process(QImage image)
{
    if (busy.exchange(true))
    {
        return;
    }

    if (resetPending)
    {
        reference = image.copy();
        done = false;
        resetPending = false;
        busy = false;
        return;
    }

    if (reference.isNull() || done)
    {
        busy = false;
        return;
    }

    QTimer::singleShot(0, this, [this, image] {

        RotationMatrix rot;
        double quality = findCenterOfRotation(
                    reference,
                    image,
                    &rot
        );
        completed(quality, rot.rotationDegree(), rot.centerOfRotationAsQPoint() - QPoint(image.width()/2, image.height()/2));
        busy = false;
    });
}

#ifndef ROTATIONFINDER_H
#define ROTATIONFINDER_H

#include <QObject>
#include <QImage>
#include <atomic>

class RotationFinder : public QObject
{
    Q_OBJECT
public:
    explicit RotationFinder(QObject *parent = nullptr);

    void reset();
    void stop();

public slots:
    void process(QImage image);

signals:
    void completed(double quality, double rotation, QPoint centerOfRotation);

private:
    bool done = false;
    bool resetPending = false;
    std::atomic_bool busy {false};
    QImage reference;
};

#endif // ROTATIONFINDER_H

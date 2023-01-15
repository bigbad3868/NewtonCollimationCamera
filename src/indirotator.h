#ifndef INDIROTATOR_H
#define INDIROTATOR_H

#include <QObject>
#include <libindi/baseclient.h>
#include <libindi/indipropertynumber.h>

class INDIRotator : public QObject
{
    Q_OBJECT
public:
    explicit INDIRotator(INDI::BaseClient *baseClient, QObject *parent = nullptr);

public:
    double angle() const;

    bool setAngle(double degree);

    bool setCurrentAngle(double degree);

signals:
    void angleChanged();

private:
    INDI::BaseClient *client;
    INDI::PropertyNumber propertyAngle = INDI::Property();
    INDI::PropertyNumber propertySyncAngle = INDI::Property();
};

#endif // INDIROTATOR_H

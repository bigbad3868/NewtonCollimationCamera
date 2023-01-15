#include "indirotator.h"
#include <libindi/basedevice.h>

INDIRotator::INDIRotator(INDI::BaseClient *baseClient, QObject *parent)
    : QObject(parent)
    , client(baseClient)
{
    client->watchDevice("Pegasus Falcon", [&](INDI::BaseDevice device)
    {
        device.watchProperty("CONNECTION", [&](INDI::Property)
        {
                client->connectDevice("Pegasus Falcon");
        });

        device.watchProperty("ABS_ROTATOR_ANGLE", [&](INDI::PropertyNumber property)
        {
            propertyAngle = property;
            emit angleChanged();
            property.onUpdate([&]{
                emit angleChanged();
            });
        });

        device.watchProperty("SYNC_ROTATOR_ANGLE", [&](INDI::PropertyNumber property)
        {
            propertySyncAngle = property;
        });
    });
}

double INDIRotator::angle() const
{
    return propertyAngle[0].getValue();
    return propertyAngle.isValid() ? propertyAngle[0].getValue() : 0;
    }

    bool INDIRotator::setAngle(double degree)
    {
    if (!propertyAngle.isValid())
    return false;

    propertyAngle[0].setValue(degree);
    client->sendNewProperty(propertyAngle);
    return true;
}

bool INDIRotator::setCurrentAngle(double degree)
{
    if (!propertySyncAngle.isValid())
        return false;

    propertySyncAngle[0].setValue(degree);
    client->sendNewProperty(propertySyncAngle);
    return true;
}

#include "cursorgrid.h"
#include <QGraphicsScene>

CursorGrid::CursorGrid(QObject *parent)
    : QObject(parent)
{

}

void CursorGrid::draw(QGraphicsScene *scene)
{
    QPen pen(QColorConstants::Red);
    pen.setWidth(mWidth);

    auto w = scene->width();
    auto h = scene->height();
    auto px = w / 2 + mOffset.x();
    auto py = h / 2 + mOffset.y();

    auto r1 = (h / 2) * 0.9;

    for (const auto &it: rings)
    {
        scene->addEllipse(QRect(px - it / 2, py - it / 2, it, it), pen);
    }

    scene->addLine(QLine(px - r1, py     , px + r1, py), pen);
    scene->addLine(QLine(px     , py - r1, px     , py + r1), pen);

    scene->setSceneRect(0, 0, w, h);
}

void CursorGrid::setWidth(qreal width)
{
    mWidth = width;
}

void CursorGrid::setOffsetX(int value)
{
    if (value != mOffset.x())
    {
        mOffset.setX(value);
        emit offsetXChanged(value);
    }
}

void CursorGrid::setOffsetY(int value)
{
    if (value != mOffset.y())
    {
        mOffset.setY(value);
        emit offsetYChanged(value);
    }
}

void CursorGrid::addOffsetX(int value)
{
    setOffsetX(offsetX() + value);
}

void CursorGrid::addOffsetY(int value)
{
    setOffsetY(offsetY() + value);
}

int CursorGrid::offsetX() const
{
    return mOffset.x();
}

int CursorGrid::offsetY() const
{
    return mOffset.y();
}

#ifndef CURSORGRID_H
#define CURSORGRID_H

#include <QObject>
#include <QPoint>
#include <QList>

class QGraphicsScene;

class CursorGrid: public QObject
{
    Q_OBJECT
public:
    CursorGrid(QObject *parent = nullptr);

public:
    void draw(QGraphicsScene *scene);

public:
    void setWidth(qreal width);

public:
    void setOffsetX(int value);
    void setOffsetY(int value);

    void addOffsetX(int value);
    void addOffsetY(int value);

public:
    int offsetX() const;
    int offsetY() const;

signals:
    void offsetXChanged(int);
    void offsetYChanged(int);

private:
    QPoint mOffset;
    qreal mWidth;

public:
    QList<int> rings;
};

#endif // CURSORGRID_H

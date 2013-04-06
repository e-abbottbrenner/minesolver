#ifndef MINEFIELDDISPLAY_H
#define MINEFIELDDISPLAY_H

#include <QGraphicsObject>

class MinefieldData;

class MinefieldDisplay : public QGraphicsObject
{
    Q_OBJECT
public:
    explicit MinefieldDisplay(MinefieldData *fieldData, QGraphicsItem *parent = 0);

    virtual void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget);

    virtual QRectF boundingRect() const;

private:
    MinefieldData *mineData;

    int width;
    int height;
};

#endif // MINEFIELDDISPLAY_H

#include "MineDisplay.h"

#include <QPainter>

const int MineDisplay::MineSize = 20;

MineDisplay::MineDisplay(QObject *parent) :
    QGraphicsObject(parent)
{
}

QRectF MineDisplay::boundingRect() const
{
    return QRectF(0, 0, MineSize, MineSize);
}

void MineDisplay::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget)
{
    painter->setBrush(Qt::gray);
    painter->setPen(Qt::darkGray);
    painter->drawRect(boundingRect());
}

#include "MinefieldDisplay.h"

#include "MinefieldData.h"
#include "MineDisplay.h"

MinefieldDisplay::MinefieldDisplay(MinefieldData *fieldData, QGraphicsItem *parent) :
    QGraphicsObject(parent), mineData(fieldData)
{
    width = mineData->getWidth() * MineDisplay::MineSize;
    height = mineData->getHeight() * MineDisplay::MineSize;
}

QRectF MinefieldDisplay::boundingRect() const
{
    return QRectF(0, 0, width, height);
}

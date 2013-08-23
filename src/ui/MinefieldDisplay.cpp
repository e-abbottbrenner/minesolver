#include "MinefieldDisplay.h"

#include "MinefieldData.h"
#include "CellDisplay.h"

MinefieldDisplay::MinefieldDisplay(MinefieldData *fieldData, QGraphicsItem *parent) :
    QGraphicsObject(parent), mineData(fieldData)
{
    fieldWidth = mineData->getWidth();
    fieldHeight = mineData->getHeight();

    pixelWidth = fieldWidth * CellDisplay::CellSize;
    pixelHeight = fieldHeight * CellDisplay::CellSize;

    cellDisplays = new CellDisplay**[fieldWidth];

    for(int x = 0; x < fieldWidth; ++x)
    {
        cellDisplays[x] = new CellDisplay*[fieldHeight];

        for(int y = 0; y < fieldHeight; ++y)
        {
            cellDisplays[x][y] = new CellDisplay(mineData, x, y, this);// deleted by qt
        }
    }

    connect(mineData, SIGNAL(cellRevealed(int,int)), this, SLOT(onCellRevealed(int,int)));
}

QRectF MinefieldDisplay::boundingRect() const
{
    return QRectF(0, 0, pixelWidth, pixelHeight);
}

void MinefieldDisplay::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget)
{
    // probably nothing to paint
}

void MinefieldDisplay::onCellRevealed(int x, int y)
{
    cellDisplays[x][y]->update();
}

MinefieldDisplay::~MinefieldDisplay()
{
    for(int i = 0; i < fieldWidth; ++i)
    {
        delete[] cellDisplays[i];
    }

    delete[] cellDisplays;
}

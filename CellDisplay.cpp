#include "CellDisplay.h"

#include <QPainter>
#include <QGraphicsSceneMouseEvent>

#include "MinefieldData.h"

const int CellDisplay::CellSize = 20;

CellDisplay::CellDisplay(MinefieldData *mineData, int x, int y, QGraphicsItem *parent) :
    QGraphicsObject(parent), x(x), y(y), mineData(mineData)
{
    setX(x * CellSize);
    setY(y * CellSize);
}

QRectF CellDisplay::boundingRect() const
{
    return QRectF(0, 0, CellSize, CellSize);
}

void CellDisplay::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget)
{
    painter->save();

    painter->setBrush(Qt::gray);
    painter->setPen(Qt::darkGray);

    painter->drawRect(boundingRect());

    painter->save();

    // TODO: paint the visible state of the cell
    MineStatus status = mineData->getCell(x, y);

    int margin = CellSize / 10;
    QRectF statusRect = boundingRect().adjusted(margin, margin, -margin, -margin);

    switch(status)
    {
    case SpecialStatus::Mine:
        painter->setPen(Qt::red);
        painter->setPen(Qt::black);
        painter->drawEllipse(statusRect);
        break;
    case SpecialStatus::Unknown:
        break;
    case SpecialStatus::GuessMine:
        painter->setPen(Qt::yellow);
        painter->setPen(Qt::black);
        painter->drawEllipse(statusRect);
        break;
    case SpecialStatus::GuessClear:
        painter->setPen(Qt::green);
        painter->setPen(Qt::black);
        painter->drawEllipse(statusRect);
        break;
    default:
        painter->setBrush(Qt::black);
        painter->setPen(Qt::black);
        painter->drawText(statusRect, QString::number(status));
        break;
    }

    painter->restore();

    painter->restore();
}

void CellDisplay::mousePressEvent(QGraphicsSceneMouseEvent *event)
{
    if(event->buttons() & (Qt::LeftButton | Qt::RightButton))
    {
        event->accept();
    }
}

void CellDisplay::mouseReleaseEvent(QGraphicsSceneMouseEvent *event)
{
    if(event->button() == Qt::LeftButton)
    {
        if(mineData->getCell(x, y) != SpecialStatus::GuessMine)
        {
            mineData->revealCell(x, y);
        }
    }
    else if(event->button() == Qt::RightButton)
    {
        mineData->toggleCellFlag(x, y);
    }
}

void CellDisplay::mouseDoubleClickEvent(QGraphicsSceneMouseEvent *event)
{

}
